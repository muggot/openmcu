/*
 * sasl.cxx
 *
 * Simple Authentication Security Layer interface classes
 *
 * Portable Windows Library
 *
 * Copyright (c) 2004 Reitek S.p.A.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: psasl.cxx,v $
 * Revision 1.6  2006/10/31 05:16:26  csoutheren
 * Remove warning in opt mode on gcc
 *
 * Revision 1.5  2004/05/09 07:23:50  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.4  2004/04/28 11:26:43  csoutheren
 * Hopefully fixed SASL and SASL2 problems
 *
 * Revision 1.3  2004/04/24 06:27:56  rjongbloed
 * Fixed GCC 3.4.0 warnings about PAssertNULL and improved recoverability on
 *   NULL pointer usage in various bits of code.
 *
 * Revision 1.2  2004/04/18 12:34:22  csoutheren
 * Modified to compile under Unix
 *
 * Revision 1.1  2004/04/18 12:02:31  csoutheren
 * Added classes for SASL authentication
 * Thanks to Federico Pinna and Reitek S.p.A.
 *
 *
 */

#ifdef __GNUC__
#pragma implementation "psasl.h"
#endif

#include <ptlib.h>

#include <ptclib/psasl.h>
#include <ptclib/cypher.h>

#if P_SASL2

extern "C" {

#if P_HAS_SASL_SASL_H
#include <sasl/sasl.h>
#else
#include <sasl.h>
#endif

};


#ifdef _MSC_VER

#pragma comment(lib, P_SASL_LIBRARY)

#endif

///////////////////////////////////////////////////////

static int PSASL_ClientRealm(void *, int id, const char **, const char **result)
{
    if (id != SASL_CB_GETREALM)
        return SASL_FAIL;

    *result = (const char *)PSASLClient::GetRealm();
    return SASL_OK;
}

static int PSASL_ClientAuthID(void *context, int id, const char **result, unsigned *len)
{
    if (id != SASL_CB_AUTHNAME)
        return SASL_FAIL;

    if (PAssertNULL(context) == NULL)
        return SASL_FAIL;

    const PSASLClient * c = (const PSASLClient *)context;
    *result = (const char *)c->GetAuthID();

    if (len)
        *len = *result ? strlen(*result) : 0;

    return SASL_OK;
}

static int PSASL_ClientUserID(void *context, int id, const char **result, unsigned *len)
{
    if (id != SASL_CB_USER)
        return SASL_FAIL;

    if (PAssertNULL(context) == NULL)
        return SASL_FAIL;

    const PSASLClient * c = (const PSASLClient *)context;
    *result = (const char *)c->GetUserID();

    if (len)
        *len = *result ? strlen(*result) : 0;

    return SASL_OK;
}

static int PSASL_ClientPassword(sasl_conn_t *, void *context, int id, sasl_secret_t **psecret)
{
    if (id != SASL_CB_PASS)
        return SASL_FAIL;

    if (PAssertNULL(context) == NULL)
        return SASL_FAIL;

    const PSASLClient * c = (const PSASLClient *)context;
    const char * pwd = c->GetPassword();

    if (!pwd)
        return SASL_FAIL;

    size_t len = strlen(pwd);

    *psecret = (sasl_secret_t *)malloc(sizeof(sasl_secret_t) + len);
    (*psecret)->len = len;
    strcpy((char *)(*psecret)->data, pwd);
    
    return SASL_OK;
}

static int PSASL_ClientGetPath(void *, const char ** path)
{
    *path = (const char *)PSASLClient::GetPath();
    return SASL_OK;
}

static int PSASL_ClientLog(void *, int priority, const char *message) 
{
#if PTRACING
    static const char * labels[7] = { "Error", "Fail", "Warning", "Note", "Debug", "Trace", "Pass" };
#endif

    if (!message || priority > SASL_LOG_PASS)
        return SASL_BADPARAM;

    if (priority < SASL_LOG_ERR)
        return SASL_OK;

    PTRACE(priority, "SASL\t" << labels[priority - 1] << ": " << message);

    return SASL_OK;
}


static void psasl_Initialise()
{
  PINDEX max = PSASLClient::GetPath().IsEmpty() ? 3 : 4;

  sasl_callback_t * cbs = new sasl_callback_t[max];

  cbs[0].id = SASL_CB_GETREALM;
  cbs[0].proc = (int (*)())&PSASL_ClientRealm;
  cbs[0].context = 0;

  cbs[1].id = SASL_CB_LOG;
  cbs[1].proc = (int (*)())&PSASL_ClientLog;
  cbs[1].context = 0;

  if (max == 4) {
    cbs[2].id = SASL_CB_GETPATH;
    cbs[2].proc = (int (*)())&PSASL_ClientGetPath;
    cbs[2].context = 0;
  }

  cbs[max - 1].id = SASL_CB_LIST_END;
  cbs[max - 1].proc = 0;
  cbs[max - 1].context = 0;

  sasl_client_init(cbs);
}

static PAtomicInteger psasl_UsageCount(0);
PString PSASLClient::s_Realm;
PString PSASLClient::s_Path;

PSASLClient::PSASLClient(const PString& service, const PString& uid, const PString& auth, const PString& pwd) :
    m_CallBacks(NULL),
    m_ConnState(NULL),
    m_Service(service),
    m_UserID(uid.IsEmpty() ? auth : uid),
    m_AuthID(auth.IsEmpty() ? uid : auth),
    m_Password(pwd)
{
    if (++psasl_UsageCount == 1)
        psasl_Initialise();
}


PSASLClient::~PSASLClient()
{
    if (m_ConnState)
        End();

    delete (sasl_callback_t *)m_CallBacks;
}


BOOL PSASLClient::Init(const PString& fqdn, PStringSet& supportedMechanisms)
{
    if (!m_CallBacks)
    {
        sasl_callback_t * cbs = new sasl_callback_t[4];

        cbs[0].id = SASL_CB_AUTHNAME;
        cbs[0].proc = (int (*)())&PSASL_ClientAuthID;
        cbs[0].context = this;

        cbs[1].id = SASL_CB_USER;
        cbs[1].proc = (int (*)())&PSASL_ClientUserID;
        cbs[1].context = this;

        cbs[2].id = SASL_CB_PASS;
        cbs[2].proc = (int (*)())&PSASL_ClientPassword;
        cbs[2].context = this;

        cbs[3].id = SASL_CB_LIST_END;
        cbs[3].proc = 0;
        cbs[3].context = 0;

        m_CallBacks = cbs;
    }

    if (m_ConnState)
        End();

    sasl_conn_t * s = (sasl_conn_t *)m_ConnState;
    int result = sasl_client_new(m_Service, fqdn, 0, 0, (const sasl_callback_t *)m_CallBacks, 0, &s);

    if (result != SASL_OK)
        return FALSE;

    const char * list;
    unsigned plen;
    int pcount;

    sasl_listmech((sasl_conn_t *)m_ConnState, 0, 0, " ", 0, &list, &plen, &pcount); 

    PStringArray a = PString(list).Tokenise(" ");

    for (PINDEX i = 0, max = a.GetSize() ; i < max ; i++)
        supportedMechanisms.Include(a[i]);

    return  TRUE;
}


BOOL PSASLClient::Start(const PString& mechanism, PString& output)
{
    const char * _output = 0;
    unsigned _len = 0;

    if (Start(mechanism, &_output, _len))
    {
        if (_output)
        {
            PBase64 b64;
            b64.StartEncoding();
            b64.ProcessEncoding(_output, _len);
            output = b64.CompleteEncoding();
            output.Replace("\r\n", PString::Empty(), TRUE);
        }

        return TRUE;
    }

    return FALSE;
}


BOOL PSASLClient::Start(const PString& mechanism, const char ** output, unsigned& len)
{
    if (!m_ConnState)
        return FALSE;

    int result = sasl_client_start((sasl_conn_t *)m_ConnState, mechanism, 0, output, &len, 0);

    if (result == SASL_OK || result == SASL_CONTINUE)
        return TRUE;

    return FALSE;
}


PSASLClient::PSASLResult PSASLClient::Negotiate(const PString& input, PString& output)
{
    PBase64 b64;
    
    b64.StartDecoding();
    b64.ProcessDecoding(input);

    PBYTEArray _bin_input = b64.GetDecodedData();
    PString _input((const char *)(const BYTE *)_bin_input, _bin_input.GetSize());

    const char * _output;
    
    PSASLClient::PSASLResult result = Negotiate(_input, &_output);

    if (_output)
    {
        b64.StartEncoding();
        b64.ProcessEncoding(_output);
        output = b64.CompleteEncoding();
        output.Replace("\r\n", PString::Empty(), TRUE);
    }

    return result;
}


PSASLClient::PSASLResult PSASLClient::Negotiate(const char * input, const char ** output)
{
    unsigned len;

    int result = sasl_client_step((sasl_conn_t *)m_ConnState, input, strlen(input), 0, output, &len);

    if (result != SASL_OK && result != SASL_CONTINUE)
        return PSASLClient::Fail;

    if (result == SASL_OK)
        return PSASLClient::OK;
    else
        return PSASLClient::Continue;
}


BOOL PSASLClient::End()
{
    if (m_ConnState)
    {
        sasl_conn_t * s = (sasl_conn_t *)m_ConnState;
        sasl_dispose(&s);
        m_ConnState = 0;
        return TRUE;
    }

    return FALSE;
}

#endif // P_SASL2

// End of File ///////////////////////////////////////////////////////////////

