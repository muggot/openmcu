/*
 * main.cxx
 *
 * PWLib application source file for LDAP Test
 *
 * Main program entry point.
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
 * $Log: main.cxx,v $
 * Revision 1.3  2003/09/26 13:43:49  rjongbloed
 * Added special test to give more indicative error if try to compile without LDAP support.
 *
 * Revision 1.2  2003/03/31 03:35:20  robertj
 * Major addition of LDAP functionality.
 * Added ILS specialisation of LDAP.
 *
 * Revision 1.1  2003/03/28 01:15:44  robertj
 * OpenLDAP support.
 *
 */

#include "precompile.h"
#include "main.h"
#include "version.h"

#include <ptclib/pldap.h>
#include <ptclib/pils.h>

#if !P_LDAP
#error Must have LDAP enabled for this application.
#endif


/* Test command lines:

add    -h ils.seconix.com -x "c=AU, cn=robertj@equival.com.au, objectClass=RTPerson" "cn=robertj@equival.com.au" surname=Jongbloed givenName=Robert c=AU
delete -h ils.seconix.com -x "c=AU, cn=robertj@equival.com.au, objectClass=RTPerson"
search -h ils.seconix.com -x -b "objectClass=RTPerson" "cn=*au"
search -h ils.seconix.com -x -b "objectClass=RTPerson" "cn=*" cn surname givenName -P

add    -h ils.seconix.com -x -I robertj@equival.com.au surname=Jongbloed givenName=Robert c=AU rfc822Mailbox=robertj@equival.com.au
delete -h ils.seconix.com -x -I robertj@equival.com.au
search -h ils.seconix.com -x -I "*" -P
*/


PCREATE_PROCESS(LDAPTest);



LDAPTest::LDAPTest()
  : PProcess("Equivalence", "LDAP Test", MAJOR_VERSION, MINOR_VERSION, BUILD_TYPE, BUILD_NUMBER)
{
}


void LDAPTest::Main()
{
  PArgList & args = GetArguments();
  args.Parse("h:p:x.b:s:P.I.", FALSE);

  if (args.GetCount() == 0) {
    Usage();
    return;
  }

  if (args.HasOption('I')) {
    PILSSession ils;
    if (!ils.Open(args.GetOptionString('h'), (WORD)args.GetOptionString('p').AsUnsigned())) {
      cerr << "Could not open ILS server at " << args[1];
      return;
    }

    if (args[0] *= "add") {
      AddILS(args, ils);
      return;
    }

    if (args[0] *= "delete") {
      DeleteILS(args, ils);
      return;
    }

    if (args[0] *= "search") {
      SearchILS(args, ils);
      return;
    }
  }
  else {
    PLDAPSession ldap;
    if (!ldap.Open(args.GetOptionString('h'), (WORD)args.GetOptionString('p').AsUnsigned())) {
      cerr << "Could not open LDAP server at " << args[1];
      return;
    }

    if (args[0] *= "add") {
      Add(args, ldap);
      return;
    }

    if (args[0] *= "delete") {
      Delete(args, ldap);
      return;
    }

    if (args[0] *= "search") {
      Search(args, ldap);
      return;
    }
  }

  cerr << "Invalid command: " << args[0];
}


void LDAPTest::Usage()
{
  cerr << "usage: " << GetFile().GetTitle() << " { add | delete | search } [ args ]\n"
          "   General arguments:\n"
          "      -h host    LDAP server\n"
          "      -p port    port on LDAP server\n"
          "      -s scope   one of base, one, or sub (search scope)\n"
          "      -x         Simple authentication\n"
          "      -I         use ILS schema\n"
          "\n"
          "   add arguments:\n"
          "      dn attribute=value [ attribute=value ... ]\n"
          "      Note if -I is used then dn is actually the cn.\n"
          "\n"
          "   delete arguments:\n"
          "      dn [ dn ... ]\n"
          "      Note if -I is used then dn is actually the cn.\n"
          "\n"
          "   search arguments:\n"
          "      filter [ attribute ... ]\n"
          "      -b basedn  base dn for search\n"
          "      -P         Pause between entries\n"
          "      Note if -I is used then filter is implicitly cn=filter.\n"
          "\n";
}


void LDAPTest::Add(PArgList & args, PLDAPSession & ldap)
{
  if (args.GetCount() < 3) {
    Usage();
    return;
  }

  if (ldap.Add(args[1], args.GetParameters(2)))
    cout << "Added " << args[1] << endl;
  else
    cout << "Could not add " << args[1] << ": " << ldap.GetErrorText() << endl;
}


void LDAPTest::Delete(PArgList & args, PLDAPSession & ldap)
{
  if (args.GetCount() < 2) {
    Usage();
    return;
  }

  for (PINDEX i = 1; i < args.GetCount(); i++) {
    if (ldap.Delete(args[i]))
      cout << args[i] << " removed." << endl;
    else
      cout << "Could not remove " << args[i] << ": " << ldap.GetErrorText() << endl;
  }
}


void LDAPTest::Search(PArgList & args, PLDAPSession & ldap)
{
  if (args.GetCount() < 2) {
    Usage();
    return;
  }

  ldap.SetBaseDN(args.GetOptionString('b'));

  if (args.HasOption('P')) {
    PLDAPSession::SearchContext context;
    if (ldap.Search(context, args[1], args.GetParameters(2))) {
      do {
        PStringToString entry;
        if (ldap.GetSearchResult(context, entry)) {
          cout << entry << "\n\nPress ENTER ..." << flush;
          cin.get();
          cout << '\n';
        }
      } while (ldap.GetNextSearchResult(context));
    }
    else
      cout << "Could not find entries for filter: " << args[1]
           << ": " << ldap.GetErrorText() << endl;
  }
  else {
    PList<PStringToString> data = ldap.Search(args[1], args.GetParameters(2));
    if (data.IsEmpty())
      cout << "Could not find entries for filter: " << args[1]
           << ": " << ldap.GetErrorText() << endl;
    else
      cout << setfill('\n') << data;
  }
}


void LDAPTest::AddILS(PArgList & args, PILSSession & ils)
{
  if (args.GetCount() < 2) {
    Usage();
    return;
  }

  PILSSession::RTPerson person = args.GetParameters(2);
  person.cn = args[1];
  if (person.rfc822Mailbox.IsEmpty())
    person.rfc822Mailbox = person.cn;

  if (ils.AddPerson(person))
    cout << person.GetDN() << " added." << endl;
  else
    cout << "Could not add person:\n" << person << "\nError: " << ils.GetErrorText() << endl;
}


void LDAPTest::DeleteILS(PArgList & args, PILSSession & ils)
{
  if (args.GetCount() < 1) {
    Usage();
    return;
  }

  PILSSession::RTPerson person;
  person.cn = args[1];
  if (ils.DeletePerson(person))
    cout << args[1] << " removed." << endl;
  else
    cout << "Could not remove person: " << args[1] << ": " << ils.GetErrorText() << endl;
}


void LDAPTest::SearchILS(PArgList & args, PILSSession & ils)
{
  if (args.GetCount() < 2) {
    Usage();
    return;
  }

  PString filter = args[1];
  if (filter.Find('=') != P_MAX_INDEX)
    filter.Splice("cn=", 0);

  if (args.HasOption('P')) {
    PLDAPSession::SearchContext context;
    if (ils.Search(context, filter)) {
      do {
        PILSSession::RTPerson person;
        if (ils.GetSearchResult(context, person)) {
          cout << person << "\n\nPress ENTER ..." << flush;
          cin.get();
          cout << '\n';
        }
      } while (ils.GetNextSearchResult(context));
    }
    else
      cout << "Could not find people for filter: " << filter
           << ": " << ils.GetErrorText() << endl;
  }
  else {
    PList<PILSSession::RTPerson> people = ils.SearchPeople(filter);
    if (people.IsEmpty())
      cout << "Could not find people for filter: " << filter
           << ": " << ils.GetErrorText() << endl;
    else
      cout << setfill('\n') << people;
  }
}


// End of File ///////////////////////////////////////////////////////////////
