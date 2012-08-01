/*
 * enum.cxx
 *
 * Portable Windows Library
 *
 * Copyright (C) 2004 Post Increment
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
 * $Log: enum.cxx,v $
 * Revision 1.12  2007/09/08 11:34:28  rjongbloed
 * Improved memory checking (leaks etc), especially when using MSVC debug library.
 *
 * Revision 1.11  2007/06/27 15:26:52  shorne
 * added Uniform Resource Name Resolver Discovery System (URN RDS) lookups
 *
 * Revision 1.10  2007/04/02 05:29:54  rjongbloed
 * Tidied some trace logs to assure all have a category (bit before a tab character) set.
 *
 * Revision 1.9  2005/11/30 12:47:41  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.8  2005/08/31 05:55:03  shorne
 * Reworked ENUM to craigs' exacting requirements
 *
 * Revision 1.7  2005/08/31 04:07:53  shorne
 * added ability to set ENUM Servers at runtime
 *
 * Revision 1.6  2004/08/04 10:26:39  csoutheren
 * Changed service to be case insignificant
 *
 * Revision 1.5  2004/08/03 13:37:45  csoutheren
 * Added ability to set ENUM search path from environment variable
 *
 * Revision 1.4  2004/07/19 13:55:41  csoutheren
 * Work-around for crash on gcc 3.5-20040704
 *
 * Revision 1.3  2004/06/05 01:58:37  rjongbloed
 * Fixed MSVC 6 compatibility
 *
 * Revision 1.2  2004/05/31 23:14:17  csoutheren
 * Fixed warnings under VS.net and fixed problem with SRV records when returning multiple records
 *
 * Revision 1.1  2004/05/31 13:56:37  csoutheren
 * Added implementation of ENUM resolution of E.164 numbers by DNS
 *
 */

#ifdef __GNUC__
#pragma implementation "enum.h"
#endif

#include <ptlib.h>
#include <ptclib/pdns.h>
#include <ptclib/enum.h>

#define new PNEW


#if P_DNS

#ifdef  _WIN32
#define PATH_SEP   ";"
#else
#define PATH_SEP   ":"
#endif

static const char * PWLIB_ENUM_PATH = "PWLIB_ENUM_PATH";

///////////////////////////////////////////////////////////////////////

PObject::Comparison PDNS::NAPTRRecord::Compare(const PObject & obj) const
{
  const NAPTRRecord * other = dynamic_cast<const NAPTRRecord *>(&obj);

  if (other == NULL)
    return LessThan;

  if (order < other->order)
    return LessThan;
  else if (order > other->order)
    return GreaterThan;

  if (preference < other->preference)
    return LessThan;
  else if (preference > other->preference)
    return GreaterThan;

  return EqualTo;
}

void PDNS::NAPTRRecord::PrintOn(ostream & strm) const
{
  strm << "order=" << order << ", "
       << "preference=" << preference << ", "
       << "flags=" << flags << ", "
       << "service=" << service << ", "
       << "regex=" << regex << ", "
       << "replacement=" << replacement;
}

///////////////////////////////////////////////////////////////////////

struct NAPTR_DNS {
  PUInt16b order;
  PUInt16b preference;

  char info[1];

  char * GetFlagsBase() const       { return (char *)&info; }
  int GetFlagsLen() const           { return (int)GetFlagsBase()[0]; }

  char * GetServiceBase() const     { return GetFlagsBase() + 1 + GetFlagsLen(); }
  int GetServiceLen() const         { return (int)GetServiceBase()[0]; }

  char * GetRegexBase() const       { return GetServiceBase() + 1 + GetServiceLen(); }
  int GetRegexLen() const           { return (int)GetRegexBase()[0]; }

  char * GetReplacementBase() const { return GetRegexBase() + 1 + GetRegexLen(); }
  int GetReplacementLen() const     { return (int)GetReplacementBase()[0]; }

  PString GetFlags() const          { return PString(GetFlagsBase()+1,       GetFlagsLen()); }
  PString GetService() const        { return PString(GetServiceBase()+1,     GetServiceLen()); }
  PString GetRegex() const          { return PString(GetRegexBase()+1,       GetRegexLen()); }
  PString GetReplacement() const    { return PString(GetReplacementBase()+1, GetReplacementLen()); }
};

PDNS::NAPTRRecord * PDNS::NAPTRRecordList::HandleDNSRecord(PDNS_RECORD dnsRecord, PDNS_RECORD /*results*/)
{
  PDNS::NAPTRRecord * record = NULL;

  if (
      (dnsRecord->Flags.S.Section == DnsSectionAnswer) && 
      (dnsRecord->wType == DNS_TYPE_NAPTR)
      ) {
    record = new NAPTRRecord();

    NAPTR_DNS * naptr = (NAPTR_DNS *)&dnsRecord->Data;

    record->order       = naptr->order;
    record->preference  = naptr->preference;
    record->flags       = naptr->GetFlags();
    record->service     = naptr->GetService();
    record->regex       = naptr->GetRegex();
    record->replacement = naptr->GetReplacement();
  }

  return record;
}


void PDNS::NAPTRRecordList::PrintOn(ostream & strm) const
{
  PINDEX i;
  for (i = 0; i < GetSize(); i++) 
    strm << (*this)[i] << endl;
}

PDNS::NAPTRRecord * PDNS::NAPTRRecordList::GetFirst(const char * service)
{
  if (GetSize() == 0)
    return NULL;

  currentPos   = 0;
  lastOrder = operator[](0).order;
  orderLocked = FALSE;

  return GetNext(service);
}

PDNS::NAPTRRecord * PDNS::NAPTRRecordList::GetNext(const char * service)
{
  if (GetSize() == 0)
    return NULL;

  while (currentPos < GetSize()) {

    NAPTRRecord & record = operator[](currentPos);

    // once we have a match, we cannot look at higher order records
    // and note that the list is already sorted by preference
    if (orderLocked && lastOrder != record.order)
      return NULL;

    else {
      currentPos++;
      lastOrder   = record.order;
      if (record.order == lastOrder) {
        if ((service == NULL) || (record.service *= service)) {
          orderLocked = TRUE;
          return &record;
        }
      }
    }
  }

  return NULL;
}

static PString ApplyRegex(const PString & orig, const PString & regexStr)
{
  // must have at least 3 delimiters and two chars of text
  if (regexStr.GetLength() < 5) { 
    PTRACE(1, "ENUM\tregex is too short: " << regexStr);
    return PString::Empty();
  }

  // first char in the regex is always the delimiter
  char delimiter = regexStr[0];

  // break the string into match and replace strings by looking for non-escaped delimiters
  PString strings[2];
  PINDEX strNum = 0;
  PINDEX pos = 1;
  PINDEX start = pos;
  for (pos = 1; strNum < 2 && pos < regexStr.GetLength(); pos++) {
    if (regexStr[pos] == '\\')
      pos++;
    else if (regexStr[pos] == delimiter) {
      strings[strNum] = regexStr(start, pos-1);
      strNum++;
      pos++;
      start = pos;
    }
  }

  // make sure we have some strings
  // CRS: this construct avoids a gcc crash with gcc 3.5-20040704/
  // when using the following:
  // if (strings[0].IsEmpty() || strings[1].IsEmpty()) {
  PString & str1 = strings[0]; 
  PString & str2 = strings[1]; 
  if (str1.IsEmpty() || str2.IsEmpty()) {
    PTRACE(1, "ENUM\tregex does not parse into two string: " << regexStr);
    return PString::Empty();
  }

  // get the flags
  PString flags;
  if (strNum == 2 && pos < regexStr.GetLength()-1) {
    pos++;
    flags = regexStr.Mid(pos+1).ToLower();
  }

  // construct the regular expression
  PRegularExpression regex;
  int regexFlags = PRegularExpression::Extended;
  if (flags.Find('i') != P_MAX_INDEX)
    regexFlags += PRegularExpression::IgnoreCase;
  if (!regex.Compile(strings[0], regexFlags)) {
    PTRACE(1, "ENUM\tregex does not compile : " << regexStr);
    return PString();
  }

  // apply the regular expression to the original string
  PIntArray starts(10), ends(10);
  if (!regex.Execute(orig, starts, ends)) {
    PTRACE(1, "ENUM\tregex does not execute : " << regexStr);
    return PString();
  }

  // replace variables in the second string
  PString value = strings[1];
  for (pos = 0; pos < value.GetLength(); pos++) {
    if (value[pos] == '\\' && pos < value.GetLength()-1) {
      int var = value[pos+1]-'1'+1;   
      PString str;
      if (var >= 0 && var < starts.GetSize() && var < ends.GetSize())
        str = orig(starts[var], ends[var]);
      value = value.Left(pos) + str + value.Mid(pos+2);
    }
  }

  return value;
}

static PStringArray & GetENUMServers()
{
  static const char * defaultDomains[] = { "e164.voxgratia.net","e164.org","e164.arpa"};
  static PStringArray servers(
          sizeof(defaultDomains)/sizeof(defaultDomains[0]),
          defaultDomains
  );
  return servers;
}

static PMutex & GetENUMServerMutex()
{
  static PMutex mutex;
  return mutex;
}

void PDNS::SetENUMServers(const PStringArray & servers)
{
     PWaitAndSignal m(GetENUMServerMutex());
     GetENUMServers() = servers;
}

BOOL PDNS::ENUMLookup(const PString & e164,
      const PString & service,PString & dn)
{
  PWaitAndSignal m(GetENUMServerMutex());
  PStringArray domains;
  char * env = ::getenv(PWLIB_ENUM_PATH);
  if (env == NULL)
    domains += GetENUMServers();
  else
    domains += PString(env).Tokenise(PATH_SEP);

  return PDNS::ENUMLookup(e164, service, domains, dn);
}

static BOOL InternalENUMLookup(const PString & e164, const PString & service, PDNS::NAPTRRecordList & records, PString & returnStr)
{
  BOOL result = FALSE;

  // get the first record that matches the service. 
  PDNS::NAPTRRecord * rec = records.GetFirst(service);

  do {

    // if no more records that match this service, then fail
    if (rec == NULL)
      break;

    // process the flags
    BOOL handled  = FALSE;
    BOOL terminal = TRUE;

    for (PINDEX f = 0; !handled && f < rec->flags.GetLength(); ++f) {
      switch (tolower(rec->flags[f])) {

        // do an SRV lookup
        case 's':
          terminal = TRUE;
          handled = FALSE;
          break;

        // do an A lookup
        case 'a':
          terminal = TRUE;
          handled = FALSE;
          break;

        // apply regex and do the lookup
        case 'u':
          returnStr = ApplyRegex(e164, rec->regex);
          result   = TRUE;
          terminal = TRUE;
          handled  = TRUE;
          break;

        // handle in a protocol specific way - not supported
        case 'p':
          handled = FALSE;
          break;
  
        default:
          handled = FALSE;
      }
    }

    // if no flags were accepted, then unlock the order on the record and get the next record
    if (!handled) {
      records.UnlockOrder();
      rec = records.GetNext(service);
      continue;
    }

    // if this was a terminal lookup, finish now
    if (terminal)
      break;

  } while (!result);

  return result;
}

BOOL PDNS::ENUMLookup(
        const PString & _e164,
        const PString & service,
   const PStringArray & enumSpaces,
              PString & returnStr
)
{
  PString e164 = _e164;

  if (e164[0] != '+')
    e164 = PString('+') + e164;

  ////////////////////////////////////////////////////////
  // convert to domain name as per RFC 2916

  // remove all non-digits
  PINDEX pos = 1;
  while (pos < e164.GetLength()) {
    if (isdigit(e164[pos]))
      pos++;
    else
      e164 = e164.Left(pos) + e164.Mid(pos+1);
  }

  // reverse the order of the digits, and add "." in between each digit
  PString domain;
  for (pos = 1; pos < e164.GetLength(); pos++) {
    if (!domain.IsEmpty())
      domain = PString('.') + domain;
    domain = PString(e164[pos]) + domain;
  }

  for (PINDEX i = 0; i < enumSpaces.GetSize(); i++) {

    PDNS::NAPTRRecordList records;

    // do the initial lookup - if no answer then the lookup failed
    if (!PDNS::GetRecords(domain + "." + enumSpaces[i], records))
      continue;

    if (InternalENUMLookup(e164, service, records, returnStr))
      return TRUE;
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////

static const char * PWLIB_RDS_PATH = "PWLIB_RDS_PATH";

static PStringArray & GetRDSServers()
{
  static const char * defaultDomains[] = {"rds.voxgratia.org"};
  static PStringArray servers(
          sizeof(defaultDomains)/sizeof(defaultDomains[0]),
          defaultDomains
  );
  return servers;
}

static BOOL RewriteDomain(const PString & original, PDNS::NAPTRRecordList & records, PString & returnStr)
{
   BOOL result = FALSE;

  // get the first record that matches the service. 
  PDNS::NAPTRRecord * rec = records.GetFirst();

  do {

    // if no more records that match this service, then fail
    if (rec == NULL)
      break;

    // process the flags
    BOOL handled  = FALSE;

	// General domain rewrites has no flag
    if (rec->flags.IsEmpty()) {
        returnStr = ApplyRegex(original, rec->regex);
		if (returnStr.GetLength() > 0) {
            result   = TRUE;
            handled  = TRUE;
			break;
		}
	} else {
	   break;   // We have other types of records which we don't want.
	}
	
    // if no flags were accepted, then unlock the order on the record and get the next record
    if (!handled) {
      records.UnlockOrder();
      rec = records.GetNext();
      continue;
    }

  } while (!result);

  return result;   
}

static BOOL InternalRDSLookup(const PString & rds, const PString & service, PDNS::NAPTRRecordList & records, PString & returnStr)
{
  BOOL result = FALSE;

  // get the first record that matches the service. 
  PDNS::NAPTRRecord * rec = records.GetFirst(service);

  do {

    // if no more records that match this service, then fail
    if (rec == NULL)
      break;

    // process the flags
    BOOL handled  = FALSE;
    BOOL terminal = TRUE;

    for (PINDEX f = 0; !handled && f < rec->flags.GetLength(); ++f) {
      switch (tolower(rec->flags[f])) {

        // do an SRV lookup
        case 's':
		  // apply regex and do the lookup
          returnStr = ApplyRegex(rds, rec->regex);
          result   = TRUE;
          terminal = TRUE;
          handled  = TRUE;
          break;
 
        case 'a':            // A lookup
        case 'u':            // U Lookup
          terminal = TRUE;
          handled = FALSE;
          break;
 
        case 'p':           // P specific
		default:
          handled = FALSE;
          break;
      }
    }

    // if no flags were accepted, then unlock the order on the record and get the next record
    if (!handled) {
      records.UnlockOrder();
      rec = records.GetNext(service);
      continue;
    }

    // if this was a terminal lookup, finish now
    if (terminal)
      break;

  } while (!result);

  return result;
}


static PMutex & GetRDSServerMutex()
{
  static PMutex mutex;
  return mutex;
}

void PDNS::SetRDSServers(const PStringArray & servers)
{
     PWaitAndSignal m(GetRDSServerMutex());
     GetRDSServers() = servers;
}

BOOL PDNS::RDSLookup(const PURL & url,
      const PString & service,PStringList & dn)
{
  PWaitAndSignal m(GetRDSServerMutex());
  PStringArray domains;
  char * env = ::getenv(PWLIB_RDS_PATH);
  if (env == NULL)
    domains += GetRDSServers();
  else
    domains += PString(env).Tokenise(PATH_SEP);

  return PDNS::RDSLookup(url, service, domains, dn);
}

BOOL PDNS::RDSLookup(
        const PURL & url,
        const PString & service,
   const PStringArray & naptrSpaces,
         PStringList & returnStr
)
{

  for (PINDEX i = 0; i < naptrSpaces.GetSize(); i++) {

    PDNS::NAPTRRecordList records;

    // do the initial lookup - if no answer then no URN RDS records for that domain
    if (!PDNS::GetRecords(naptrSpaces[i], records))
      continue;
     
	// Do a universal domain rewrite Ref: RFC 2915 sect 7.1 
    PString newURL = PString();
	if (!RewriteDomain(url.AsString(), records, newURL))
	  continue;

	// Retrieve the NAPTR records associated with that rewritten domain.
	PDNS::NAPTRRecordList subrecords;
    if (!PDNS::GetRecords(newURL, subrecords))
        continue;

	// Retrieve the SRV records for the service 
    PString srvRecord = PString(); 
	if (!InternalRDSLookup(url.AsString(),service,subrecords,srvRecord))
	    continue;

	// Should be in the form "_h323ls._udp.mydomain.com";
	// Need to find the second "." to retrieve the service record type
    PINDEX dot = 0;
	for (PINDEX i = 0;  i < 2; i++) {
	   dot = srvRecord.Find('.',dot+1);
	}

	// Rewrite the userName
	PString finaluser = url.GetScheme() + ":" + url.GetUserName() + "@" + srvRecord.Mid(dot+1); 
	// Retrieve the service record type
	PString srvrec = srvRecord.Left(dot+1);

	// Lookup the SRV record for the hosted domain.
	PStringList retStr;
	if (!PDNS::LookupSRV(finaluser,srvrec,retStr)) 
	    continue;
		
	if (retStr.GetSize() > 0) {   // We have found records 
		returnStr = retStr;
	    return TRUE;
	}
  }

  return FALSE;
}


#endif

// End of File ///////////////////////////////////////////////////////////////
