/*
 * main.h
 *
 * PWLib application header file for LDAP Test
 *
 * Copyright (c) 2003 Equivalence Pty. Ltd.
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
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: main.h,v $
 * Revision 1.3  2006/06/21 03:28:42  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.2  2003/03/31 03:35:20  robertj
 * Major addition of LDAP functionality.
 * Added ILS specialisation of LDAP.
 *
 * Revision 1.1  2003/03/28 01:15:44  robertj
 * OpenLDAP support.
 *
 */

#ifndef _LDAPTest_MAIN_H
#define _LDAPTest_MAIN_H

#include <ptlib/pprocess.h>

class PLDAPSession;
class PILSSession;


class LDAPTest : public PProcess
{
  PCLASSINFO(LDAPTest, PProcess)

  public:
    LDAPTest();
    virtual void Main();
    void Usage();
    void Add(PArgList & args, PLDAPSession & ldap);
    void Delete(PArgList & args, PLDAPSession & ldap);
    void Search(PArgList & args, PLDAPSession & ldap);

    void AddILS(PArgList & args, PILSSession & ils);
    void DeleteILS(PArgList & args, PILSSession & ils);
    void SearchILS(PArgList & args, PILSSession & ils);
};


#endif  // _LDAPTest_MAIN_H


// End of File ///////////////////////////////////////////////////////////////
