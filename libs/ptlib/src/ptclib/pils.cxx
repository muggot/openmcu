/*
 * pils.cxx
 *
 * Microsoft Internet Location Server Protocol interface class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-2003 Equivalence Pty. Ltd.
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
 * $Log: pils.cxx,v $
 * Revision 1.6  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.5  2003/06/05 23:19:52  rjongbloed
 * Changed LDAP version to be compatible with ILS servers.
 *
 * Revision 1.4  2003/04/11 00:07:56  robertj
 * More for Microsoft IP address specification wierdness (registration side).
 *
 * Revision 1.3  2003/04/07 13:05:20  robertj
 * Workaround for Microsoft IP address specification wierdness.
 *
 * Revision 1.2  2003/03/31 12:18:43  robertj
 * Fixed pragma implementation
 *
 * Revision 1.1  2003/03/31 03:35:20  robertj
 * Major addition of LDAP functionality.
 * Added ILS specialisation of LDAP.
 *
 */

#ifdef __GNUC__
#pragma implementation "pils.h"
#endif

#include <ptlib.h>

#include <ptclib/pils.h>


#define new PNEW


#if P_LDAP

// Microsoft in their infinite wisdom save the IP address as an little endian
// integer from a 32 bit integer that was in network byte order (big endian)
// which causes immense confusion. Reading into a PIPSocket::Address does not
// work as it assumes that any integer forms would be in host order.
istream & operator>>(istream & s, PILSSession::MSIPAddress & a)
{
  DWORD u;
  s >> u;

#if PBYTE_ORDER==PLITTLE_ENDIAN
  a = u;
#else
  a = PIPSocket::Address((BYTE)(u>>24),(BYTE)(u>>16),(BYTE)(u>>8),(BYTE)u);
#endif

  return s;
}


ostream & operator<<(ostream & s, PILSSession::MSIPAddress & a)
{
#if PBYTE_ORDER==PLITTLE_ENDIAN
  DWORD u = a;
#else
  DWORD u = (a.Byte1()<<24)|(a.Byte2()<<16)|(a.Byte3()<<8)|a.Byte4();
#endif

  return s << u;
}


///////////////////////////////////////////////////////////////////////////////

PILSSession::PILSSession()
  : PLDAPSession("objectClass=RTPerson")
{
  protocolVersion = 2;
}


BOOL PILSSession::AddPerson(const RTPerson & person)
{
  return Add(person.GetDN(), person);
}


BOOL PILSSession::ModifyPerson(const RTPerson & person)
{
  return Modify(person.GetDN(), person);
}


BOOL PILSSession::DeletePerson(const RTPerson & person)
{
  return Delete(person.GetDN());
}


BOOL PILSSession::SearchPerson(const PString & canonicalName, RTPerson & person)
{
  SearchContext context;
  if (!Search(context, "cn="+canonicalName))
    return FALSE;

  if (!GetSearchResult(context, person))
    return FALSE;

  // Return FALSE if there is more than one match
  return !GetNextSearchResult(context);
}


PList<PILSSession::RTPerson> PILSSession::SearchPeople(const PString & filter)
{
  PList<RTPerson> persons;

  SearchContext context;
  if (Search(context, filter)) {
    do {
      RTPerson * person = new RTPerson;
      if (GetSearchResult(context, *person))
        persons.Append(person);
      else
        delete person;
    } while (GetNextSearchResult(context));
  }

  return persons;
}


PString PILSSession::RTPerson::GetDN() const
{
  PStringStream dn;

  if (!c)
    dn << "c=" << c << ", ";

  if (!o)
    dn << "o=" << o << ", ";

  dn << "cn=" + cn + ", objectClass=" + objectClass;

  return dn;
}


#endif // P_LDAP


// End of file ////////////////////////////////////////////////////////////////
