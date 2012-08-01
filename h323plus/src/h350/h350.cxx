
/*
 * h350.cxx
 *
 * H.350 LDAP interface class.
 *
 * h323plus Library
 *
 * Copyright (c) 2007 ISVO (Asia) Pte. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the General Public License (the  "GNU License"), in which case the
 * provisions of GNU License are applicable instead of those
 * above. If you wish to allow use of your version of this file only
 * under the terms of the GNU License and not to allow others to use
 * your version of this file under the MPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the GNU License. If you do not delete
 * the provisions above, a recipient may use your version of this file
 * under either the MPL or the GNU License."
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 *
 * The Initial Developer of the Original Code is ISVO (Asia) Pte. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: h350.cxx,v $
 * Revision 1.5  2008/01/05 11:36:02  shorne
 * More Fixes for Linux
 *
 * Revision 1.4  2008/01/05 07:09:53  shorne
 * added reference to buildopts to ensure if enabled everything gets compiled
 *
 * Revision 1.3  2008/01/02 16:19:25  willamowius
 * add newline at end-of-file
 *
 * Revision 1.2  2008/01/01 00:16:14  shorne
 * Fix compile warning on VS2005
 *
 * Revision 1.1  2007/08/06 20:51:38  shorne
 * First commit of h323plus
 *
 *
 *
 */

#include <ptlib.h>

#include "openh323buildopts.h"

#ifdef H323_H350

#include <ptclib/pldap.h>
#include "h350/h350.h"
#if P_DNS
#include <ptclib/pdns.h>
#endif

//////////////////////////////////////////////////////////////////////////
// Define H.350 schemas for H.350, H.350.1, H.350.2

static const char * commObject_SchemaName = "commObject";
static struct {
  const char * name;
  unsigned     type;
} commObject_attributes[] = {
	{"commUniqueId"      ,0},
	{"commOwner"         ,0},
	{"commPrivate"       ,0}
};
H350_Schema(commObject);

//-----

static const char * commURIObject_SchemaName = "commURI";
static struct {
  const char * name;
  unsigned     type;
} commURIObject_attributes[] = {
	{"commURI"      ,0}
};
H350_Schema(commURIObject);

//-----

static const char * H323Identity_SchemaName = "H323Identity";
static struct {
  const char * name;
  unsigned     type;
} H323Identity_attributes[] = {
	{"h323IdentityGKDomain"      ,0},
	{"h323Identityh323-ID"       ,0},
	{"h323IdentitydialedDigits"  ,0},
	{"h323Identityemail-ID"      ,0},
	{"h323IdentityURL-ID"        ,0},
	{"h323IdentitytransportID"   ,0},
	{"h323IdentitypartyNumber"   ,0},
	{"h323IdentitymobileUIM"     ,0},
	{"h323IdentityEndpointType"  ,0},
	{"h323IdentityServiceLevel"  ,0}
};

H350_Schema(H323Identity);

//-----

static const char * H235Identity_SchemaName = "H235Identity";
static struct {
  const char * name;
  unsigned     type;
} H235Identity_attributes[] = {
	{"h235IdentityEndpointID"     ,0},
	{"h235IdentityPassword"       ,0}
};

H350_Schema(H235Identity);

/////////////////////////////////////////////////////////////////////////////////////////////

BOOL H350_Session::Open(const PString & hostname, WORD port)
{

#if P_DNS
  PDNS::SRVRecordList srvRecords;

  if (PDNS::GetSRVRecords(PString("_ldap._tcp.") + hostname, srvRecords)) {
    
    PDNS::SRVRecord * rec = srvRecords.GetFirst();

    while (rec != NULL) {
	  if (PLDAPSession::Open(rec->hostName,rec->port))
		     return TRUE;

	  rec = srvRecords.GetNext();
    }
  } 
#endif
 
  return PLDAPSession::Open(hostname,port);
  
}

BOOL H350_Session::Login(const PString & who,const PString & passwd,AuthenticationMethod authMethod)
{
	   return Bind(who,passwd,authMethod);
}

int H350_Session::Search(const PString & base, const PString & filter, LDAP_RecordList & results, const PStringArray & attributes) 
{ 
	int records=0;
	PLDAPSession::SearchContext context;
	std::map<PString,PString> att;
	SetBaseDN(base);

    if (PLDAPSession::Search(context,filter,attributes)) {
      do {
		LDAP_Record rec;
		PStringToString attributeList;
        if (GetSearchResult(context, attributeList)) {

			PString key = PString();
			if (attributeList.GetSize() > 0) {
				key = attributeList.GetKeyAt(0) + PString("=") + attributeList.GetDataAt(0);
			}

			for (PINDEX i=0; i<attributeList.GetSize(); i++) {
				att.insert(make_pair(attributeList.GetKeyAt(i), attributeList.GetDataAt(i)));
			}

			/// Load the Schemas
	        for (std::map<PString,PString>::const_iterator r = att.begin(); r != att.end(); ++r) {
				if (r->first == PString("objectClass")) {
                   PStringList s = r->second.Lines();
				   for (PINDEX j=0; j < s.GetSize(); j++) {
                      PLDAPSchema * schema = PLDAPSchema::CreateSchema(s[j]);
				       if (schema != NULL) {
						   schema->LoadSchema();
						   rec.push_back(*schema);
						   PTRACE(4, "H350\tSchema Loaded " << s[j]);
				       }
				   }
				}
			}

			for (LDAP_Record::iterator y = rec.begin(); y != rec.end(); ++y) {
	                for (std::map<PString,PString>::const_iterator x = att.begin(); x != att.end(); ++x) {
						PTRACE(6, "H350\tChecking " << y->GetSchemaNames()[0] << " for " << x->first);

						PLDAPSchema::AttributeType type = y->GetAttributeType(x->first);

						if (type == PLDAPSchema::AttibuteUnknown)
							continue;
						else if (type == PLDAPSchema::AttributeBinary) {
							PArray<PBYTEArray> data;
							if (GetSearchResult(context,x->first,data))
                                y->SetAttribute(x->first, data[0]);
						} else
						   y->OnReceivedAttribute(x->first,x->second);
					}
			}

			results.insert(make_pair(key,rec));
			records++;
        }
      } while (GetNextSearchResult(context));
    }
	
	return records; 
}

void H350_Session::NewRecord(LDAP_Record & rec)
{
  
	PStringList s = PLDAPSchema::GetSchemaNames();
	for (PINDEX j=0; j < s.GetSize(); j++) {
        PLDAPSchema * schema = PLDAPSchema::CreateSchema(s[j]);
		if (schema != NULL) {
			schema->LoadSchema();
			rec.push_back(*schema);
			PTRACE(4, "H350\tSchema Loaded " << s[j]);
		}
	}
}

BOOL H350_Session::PostNew(const PString & dn, const LDAP_Record & record)
{
	PList<PLDAPSession::ModAttrib> attrib;
	PLDAPSession::ModAttrib::Operation mode = PLDAPSession::ModAttrib::Add;

	for (std::list<PLDAPSchema>::const_iterator r = record.begin(); r != record.end(); ++r) {
		PLDAPSchema schema = *r;
		schema.OnSendSchema(attrib,mode);
	}

	return Add(dn,attrib);
}


BOOL H350_Session::PostUpdate(const PString & dn, const LDAP_Record & record)
{
	PList<PLDAPSession::ModAttrib> attrib;
	PLDAPSession::ModAttrib::Operation mode = PLDAPSession::ModAttrib::Replace;

	for (std::list<PLDAPSchema>::const_iterator r = record.begin(); r != record.end(); ++r) {
		PLDAPSchema schema = *r;
		schema.OnSendSchema(attrib,mode);
	}

	return Modify(dn,attrib);
}


BOOL H350_Session::SetAttribute(LDAP_Record & record,const PString & attrib, const PString & value)
{
	for (std::list<PLDAPSchema>::const_iterator r = record.begin(); r != record.end(); ++r) {
	   PLDAPSchema schema = *r;
       if (schema.SetAttribute(attrib,value))
		         return TRUE;
	}
	return FALSE;
}

BOOL H350_Session::SetAttribute(LDAP_Record & record,const PString & attrib, const PBYTEArray & value)
{
	for (std::list<PLDAPSchema>::const_iterator r = record.begin(); r != record.end(); ++r) {
	   PLDAPSchema schema = *r;
       if (schema.SetAttribute(attrib,value))
		         return TRUE;
	}
	return FALSE;
}

BOOL H350_Session::GetAttribute(LDAP_Record & record,const PString & attrib, PString & value)
{
	for (std::list<PLDAPSchema>::const_iterator r = record.begin(); r != record.end(); ++r) {
	   PLDAPSchema schema = *r;
       if (schema.GetAttribute(attrib,value))
		         return TRUE;
	}
	return FALSE;
}
	 
BOOL H350_Session::GetAttribute(LDAP_Record & record,const PString & attrib, PBYTEArray & value)
{
	for (std::list<PLDAPSchema>::const_iterator r = record.begin(); r != record.end(); ++r) {
	   PLDAPSchema schema = *r;
       if (schema.GetAttribute(attrib,value))
		         return TRUE;
	}
	return FALSE;
}


#endif
