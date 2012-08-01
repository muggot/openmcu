/*
 * ipacl.cxx
 *
 * IP Access Control Lists
 *
 * Portable Windows Library
 *
 * Copyright (c) 2002 Equivalence Pty. Ltd.
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
 * $Log: ipacl.cxx,v $
 * Revision 1.18  2007/06/29 02:47:28  rjongbloed
 * Added PString::FindSpan() function (strspn equivalent) with slightly nicer semantics.
 *
 * Revision 1.17  2007/04/24 14:15:35  shorne
 * Fix for DWORD definition variation with unixODBC
 *
 * Revision 1.16  2005/01/26 05:37:58  csoutheren
 * Added ability to remove config file support
 *
 * Revision 1.15  2004/04/03 08:22:20  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.14  2002/11/06 22:47:25  robertj
 * Fixed header comment (copyright etc)
 *
 * Revision 1.13  2002/07/16 10:05:01  robertj
 * Fixed GNU warning
 *
 * Revision 1.12  2002/07/16 10:02:49  robertj
 * Fixed MSVC warning
 *
 * Revision 1.11  2002/07/16 09:58:51  robertj
 * Fixed compatibility with unix htonl(), use platform independent function!
 * Allow number of bits of 32 to be a full mask, ie a single host ip.
 *
 * Revision 1.10  2002/07/16 08:00:49  robertj
 * Fixed correct endian-ness of mask when specifed in bits.
 *
 * Revision 1.9  2002/06/19 04:03:21  robertj
 * Added default allowance boolean if ACL empty.
 * Added ability to override the creation of ACL entry objects with descendents
 *   so an application can add information/functionality to each entry.
 *
 * Revision 1.8  2002/02/13 02:08:12  robertj
 * Added const to IsAllowed() function.
 * Added missing function that takes a socket.
 *
 * Revision 1.7  1999/02/25 13:01:11  robertj
 * Fixed subtle bug in GNU compiler not automatically casting IP address.
 *
 * Revision 1.6  1999/02/25 11:10:52  robertj
 * Fixed count of non-hidden entries in config file.
 *
 * Revision 1.5  1999/02/25 05:05:15  robertj
 * Added missing test for hidden entries not to be written to config file
 *
 * Revision 1.4  1999/02/08 08:05:39  robertj
 * Changed semantics of IP access control list for empty list.
 *
 * Revision 1.3  1999/01/31 10:14:07  robertj
 * Changed about dialog to be full company name
 *
 * Revision 1.2  1999/01/31 08:10:33  robertj
 * Fixed PConfig file save, out by one error in array subscript.
 *
 * Revision 1.1  1999/01/31 00:59:26  robertj
 * Added IP Access Control List class to PTLib Components
 *
 */

#include <ptlib.h>
#include <ptclib/ipacl.h>

#define new PNEW


PIpAccessControlEntry::PIpAccessControlEntry(PIPSocket::Address addr,
                                             PIPSocket::Address msk,
                                             BOOL allow)
  : address(addr), mask(msk)
{
  allowed = allow;
  hidden = FALSE;
}


PIpAccessControlEntry::PIpAccessControlEntry(const PString & description)
  : address(0), mask(0xffffffff)
{
  Parse(description);
}


PIpAccessControlEntry & PIpAccessControlEntry::operator=(const PString & description)
{
  Parse(description);
  return *this;
}


PIpAccessControlEntry & PIpAccessControlEntry::operator=(const char * description)
{
  Parse(description);
  return *this;
}


PObject::Comparison PIpAccessControlEntry::Compare(const PObject & obj) const
{
  PAssert(PIsDescendant(&obj, PIpAccessControlEntry), PInvalidCast);
  const PIpAccessControlEntry & other = (const PIpAccessControlEntry &)obj;

  // The larger the mask value, th more specific the range, so earlier in list
  if (mask > other.mask)
    return LessThan;
  if (mask < other.mask)
    return GreaterThan;

  if (!domain && !other.domain)
    return domain.Compare(other.domain);

  if (address > other.address)
    return LessThan;
  if (address < other.address)
    return GreaterThan;

  return EqualTo;
}


void PIpAccessControlEntry::PrintOn(ostream & strm) const
{
  if (!allowed)
    strm << '-';

  if (hidden)
    strm << '@';

  if (domain.IsEmpty())
    strm << address;
  else if (domain[0] != '\xff')
    strm << domain;
  else {
    strm << "ALL";
    return;
  }

  if (mask != 0 && mask != static_cast<DWORD>(0xffffffff))
    strm << '/' << mask;
}


void PIpAccessControlEntry::ReadFrom(istream & strm)
{
  char buffer[200];
  strm >> ws >> buffer;
  Parse(buffer);
}


BOOL PIpAccessControlEntry::Parse(const PString & description)
{
  domain = PString();
  address = 0;

  if (description.IsEmpty())
    return FALSE;

  // Check for the allow/deny indication in first character of description
  BOOL offset = 1;
  if (description[0] == '-')
    allowed = FALSE;
  else {
    allowed = TRUE;
    if (description[0] != '+')
      offset = 0;
  }

  // Check for indication entry is from the hosts.allow/hosts.deny file
  hidden = FALSE;
  if (description[offset] == '@') {
    offset++;
    hidden = TRUE;
  }

  if (description.Mid(offset) *= "all") {
    domain = "\xff";
    mask = 0;
    return TRUE;
  }

  PINDEX slash = description.Find('/', offset);

  PString preSlash = description(offset, slash-1);
  if (preSlash[0] == '.') {
    // If has a leading dot then assume a domain, ignore anything after slash
    domain = preSlash;
    mask = 0;
    return TRUE;
  }

  if (preSlash.FindSpan("0123456789.") != P_MAX_INDEX) {
    // If is not all numbers and dots can't be an IP number so assume hostname
    domain = preSlash;
  }
  else if (preSlash[preSlash.GetLength()-1] != '.') {
    // Must be explicit IP number if doesn't end in dot
    address = preSlash;
  }
  else {
    // Must be partial IP number, count the dots!
    PINDEX dot = preSlash.Find('.', preSlash.Find('.')+1);
    if (dot == P_MAX_INDEX) {
      // One dot
      preSlash += "0.0.0";
      mask = "255.0.0.0";
    }
    else if ((dot = preSlash.Find('.', dot+1)) == P_MAX_INDEX) {
      // has two dots
      preSlash += "0.0";
      mask = "255.255.0.0";
    }
    else if (preSlash.Find('.', dot+1) == P_MAX_INDEX) {
      // has three dots
      preSlash += "0";
      mask = "255.255.255.0";
    }
    else {
      // Has more than three dots!
      return FALSE;
    }

    address = preSlash;
    return TRUE;
  }

  if (slash == P_MAX_INDEX) {
    // No slash so assume a full mask
    mask = 0xffffffff;
    return TRUE;
  }

  PString postSlash = description.Mid(slash+1);
  if (postSlash.FindSpan("0123456789.") != P_MAX_INDEX) {
    domain = PString();
    address = 0;
    return FALSE;
  }

  if (postSlash.Find('.') != P_MAX_INDEX)
    mask = postSlash;
  else {
    DWORD bits = postSlash.AsUnsigned();
    if (bits > 32)
      mask = PSocket::Host2Net(bits);
    else
      mask = PSocket::Host2Net((DWORD)(0xffffffff << (32 - bits)));
  }

  if (mask == 0)
    domain = "\xff";

  address = (DWORD)address & (DWORD)mask;

  return TRUE;
}


PString PIpAccessControlEntry::AsString() const
{
  PStringStream str;
  str << *this;
  return str;
}


BOOL PIpAccessControlEntry::IsValid()
{
  return address != 0 || !domain;
}


BOOL PIpAccessControlEntry::Match(PIPSocket::Address & addr)
{
  switch (domain[0]) {
    case '\0' : // Must have address field set
      break;

    case '.' :  // Are a domain name
      return PIPSocket::GetHostName(addr).Right(domain.GetLength()) *= domain;

    case '\xff' :  // Match all
      return TRUE;

    default : // All else must be a hostname
      if (!PIPSocket::GetHostAddress(domain, address))
        return FALSE;
  }

  return (address & mask) == (addr & mask);
}


///////////////////////////////////////////////////////////////////////////////

PIpAccessControlList::PIpAccessControlList(BOOL defAllow)
  : defaultAllowance(defAllow)
{
}


static BOOL ReadConfigFileLine(PTextFile & file, PString & line)
{
  line = PString();

  do {
    if (!file.ReadLine(line))
      return FALSE;
  } while (line.IsEmpty() || line[0] == '#');

  PINDEX lastCharPos;
  while (line[lastCharPos = line.GetLength()-1] == '\\') {
    PString str;
    if (!file.ReadLine(str))
      return FALSE;
    line[lastCharPos] = ' ';
    line += str;
  }

  return TRUE;
}


static void ParseConfigFileExcepts(const PString & str,
                                   PStringList & entries,
                                   PStringList & exceptions)
{
  PStringArray terms = str.Tokenise(' ', FALSE);

  BOOL hadExcept = FALSE;
  PINDEX d;
  for (d = 0; d < terms.GetSize(); d++) {
    if (terms[d] == "EXCEPT")
      hadExcept = TRUE;
    else if (hadExcept)
      exceptions.AppendString(terms[d]);
    else
      entries.AppendString(terms[d]);
  }
}


static BOOL SplitConfigFileLine(const PString & line, PString & daemons, PString & clients)
{
  PINDEX colon = line.Find(':');
  if (colon == P_MAX_INDEX)
    return FALSE;

  daemons = line.Left(colon).Trim();

  PINDEX other_colon = line.Find(':', ++colon);
  clients = line(colon, other_colon-1).Trim();

  return TRUE;
}


static BOOL IsDaemonInConfigFileLine(const PString & daemon, const PString & daemons)
{
  PStringList daemonsIn, daemonsOut;
  ParseConfigFileExcepts(daemons, daemonsIn, daemonsOut);

  for (PINDEX in = 0; in < daemonsIn.GetSize(); in++) {
    if (daemonsIn[in] == "ALL" || daemonsIn[in] == daemon) {
      PINDEX out;
      for (out = 0; out < daemonsOut.GetSize(); out++) {
        if (daemonsOut[out] == daemon)
          break;
      }
      if (out >= daemonsOut.GetSize())
        return TRUE;
    }
  }

  return FALSE;
}


static BOOL ReadConfigFile(PTextFile & file,
                           const PString & daemon,
                           PStringList & clientsIn,
                           PStringList & clientsOut)
{
  PString line;
  while (ReadConfigFileLine(file, line)) {
    PString daemons, clients;
    if (SplitConfigFileLine(line, daemons, clients) &&
        IsDaemonInConfigFileLine(daemon, daemons)) {
      ParseConfigFileExcepts(clients, clientsIn, clientsOut);
      return TRUE;
    }
  }

  return FALSE;
}


BOOL PIpAccessControlList::InternalLoadHostsAccess(const PString & daemonName,
                                                   const char * filename,
                                                   BOOL allowance)
{
  PTextFile file;
  if (!file.Open(PProcess::GetOSConfigDir() + filename, PFile::ReadOnly))
    return TRUE;

  BOOL ok = TRUE;

  PStringList clientsIn;
  PStringList clientsOut;
  while (ReadConfigFile(file, daemonName, clientsIn, clientsOut)) {
    PINDEX i;
    for (i = 0; i < clientsOut.GetSize(); i++) {
      if (!Add((allowance ? "-@" : "+@") + clientsOut[i]))
        ok = FALSE;
    }
    for (i = 0; i < clientsIn.GetSize(); i++) {
      if (!Add((allowance ? "+@" : "-@") + clientsIn[i]))
        ok = FALSE;
    }
  }

  return ok;
}


BOOL PIpAccessControlList::LoadHostsAccess(const char * daemonName)
{
  PString daemon;
  if (daemonName != NULL)
    daemon = daemonName;
  else
    daemon = PProcess::Current().GetName();

  return InternalLoadHostsAccess(daemon, "hosts.allow", TRUE) &  // Really is a single &
         InternalLoadHostsAccess(daemon, "hosts.deny", FALSE);
}

#ifdef P_CONFIG_LIST

static const char DefaultConfigName[] = "IP Access Control List";

BOOL PIpAccessControlList::Load(PConfig & cfg)
{
  return Load(cfg, DefaultConfigName);
}


BOOL PIpAccessControlList::Load(PConfig & cfg, const PString & baseName)
{
  BOOL ok = TRUE;
  PINDEX count = cfg.GetInteger(baseName & "Array Size");
  for (PINDEX i = 1; i <= count; i++) {
    if (!Add(cfg.GetString(baseName & PString(PString::Unsigned, i))))
      ok = FALSE;
  }

  return ok;
}


void PIpAccessControlList::Save(PConfig & cfg)
{
  Save(cfg, DefaultConfigName);
}


void PIpAccessControlList::Save(PConfig & cfg, const PString & baseName)
{
  PINDEX count = 0;

  for (PINDEX i = 0; i < GetSize(); i++) {
    PIpAccessControlEntry & entry = operator[](i);
    if (!entry.IsHidden()) {
      count++;
      cfg.SetString(baseName & PString(PString::Unsigned, count), entry.AsString());
    }
  }

  cfg.SetInteger(baseName & "Array Size", count);
}

#endif // P_CONFIG_LIST


BOOL PIpAccessControlList::Add(PIpAccessControlEntry * entry)
{
  if (!entry->IsValid()) {
    delete entry;
    return FALSE;
  }

  PINDEX idx = GetValuesIndex(*entry);
  if (idx == P_MAX_INDEX) {
    Append(entry);
    return TRUE;
  }

  // Return TRUE if the newly added entry is identical to an existing one
  PIpAccessControlEntry & existing = operator[](idx);
  BOOL ok = existing.IsClass(PIpAccessControlEntry::Class()) &&
            entry->IsClass(PIpAccessControlEntry::Class()) &&
            existing.IsAllowed() == entry->IsAllowed();

  delete entry;
  return ok;
}


BOOL PIpAccessControlList::Add(const PString & description)
{
  return Add(CreateControlEntry(description));
}


BOOL PIpAccessControlList::Add(PIPSocket::Address addr, PIPSocket::Address mask, BOOL allow)
{
  PStringStream description;
  description << (allow ? '+' : '-') << addr << '/' << mask;
  return Add(description);
}


BOOL PIpAccessControlList::Remove(const PString & description)
{
  PIpAccessControlEntry entry(description);

  if (!entry.IsValid())
    return FALSE;

  return InternalRemoveEntry(entry);
}


BOOL PIpAccessControlList::Remove(PIPSocket::Address addr, PIPSocket::Address mask)
{
  PIpAccessControlEntry entry(addr, mask, TRUE);
  return InternalRemoveEntry(entry);
}


BOOL PIpAccessControlList::InternalRemoveEntry(PIpAccessControlEntry & entry)
{
  PINDEX idx = GetValuesIndex(entry);
  if (idx == P_MAX_INDEX)
    return FALSE;

  RemoveAt(idx);
  return TRUE;
}


PIpAccessControlEntry * PIpAccessControlList::CreateControlEntry(const PString & description)
{
  return new PIpAccessControlEntry(description);
}


PIpAccessControlEntry * PIpAccessControlList::Find(PIPSocket::Address address) const
{
  PINDEX size = GetSize();
  if (size == 0)
    return NULL;

  for (PINDEX i = 0; i < GetSize(); i++) {
    PIpAccessControlEntry & entry = operator[](i);
    if (entry.Match(address))
      return &entry;
  }

  return NULL;
}


BOOL PIpAccessControlList::IsAllowed(PTCPSocket & socket) const
{
  if (IsEmpty())
    return defaultAllowance;

  PIPSocket::Address address;
  if (socket.GetPeerAddress(address))
    return IsAllowed(address);

  return FALSE;
}


BOOL PIpAccessControlList::IsAllowed(PIPSocket::Address address) const
{
  if (IsEmpty())
    return defaultAllowance;

  PIpAccessControlEntry * entry = Find(address);
  if (entry == NULL)
    return FALSE;

  return entry->IsAllowed();
}


// End of File ///////////////////////////////////////////////////////////////
