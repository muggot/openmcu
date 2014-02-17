/*
 * main.cxx
 *
 * PWLib application source file for DNSTest
 *
 * Main program entry point.
 *
 * Copyright 2003 Equivalence
 *
 * $Log: main.cxx,v $
 * Revision 1.8  2007/06/27 15:26:44  shorne
 * added Uniform Resource Name Resolver Discovery System (URN RDS) lookups
 *
 * Revision 1.7  2006/02/26 11:51:20  csoutheren
 * Extended DNS test program to include URL based SRV lookups
 * Re-arranged SRV lookup code to allow access to internal routine
 * Reformatted code
 *
 * Revision 1.6  2004/12/08 00:54:29  csoutheren
 * Added URL lookups
 *
 * Revision 1.5  2004/05/31 13:57:00  csoutheren
 * Added tests for ENUM resolution
 *
 * Revision 1.4  2003/09/26 13:42:16  rjongbloed
 * Added special test to give more indicative error if try to compile without DNS support.
 *
 * Revision 1.3  2003/04/22 23:25:13  craigs
 * Changed help message for SRV records
 *
 * Revision 1.2  2003/04/15 08:15:16  craigs
 * Added single string form of GetSRVRecords
 *
 * Revision 1.1  2003/04/15 04:12:38  craigs
 * Initial version
 *
 */

#include <ptlib.h>
#include <ptclib/pdns.h>
#include <ptclib/enum.h>
#include <ptclib/url.h>

#include "main.h"


#if !P_DNS
#error Must have DNS support for this application
#endif

PCREATE_PROCESS(DNSTest);

DNSTest::DNSTest()
  : PProcess("Equivalence", "DNSTest", 1, 0, AlphaCode, 1)
{
}

void Usage()
{
  PError << "usage: dnstest -t MX hostname\n"
            "       dnstest -t SRV service            (i.e. _ras._udp._example.com)\n"
            "       dnstest -t SRV service url        (i.e. _sip._udp sip:fred@example.com)\n"
            "       dnstest -t RDS url service        (i.e. mydomain.com H323+D2U)\n"
            "       dnstest -t NAPTR resource         (i.e. 2.1.2.1.5.5.5.0.0.8.1.e164.org)\n"
            "       dnstest -t NAPTR resource service (i.e. 2.1.2.1.5.5.5.0.0.8.1.e164.org E2U+SIP)\n"
            "       dnstest -t ENUM service           (i.e. +18005551212 E2U+SIP)\n"
            "       dnstest -u url                    (i.e. http://craigs@postincrement.com)\n"
  ;
}

template <class RecordListType>
void GetAndDisplayRecords(const PString & name)
{
  RecordListType records;
  if (!PDNS::GetRecords(name, records))
    PError << "Lookup for " << name << " failed" << endl;
  else
    cout << "Lookup for " << name << " returned" << endl << records << endl;
}

struct LookupRecord {
  PIPSocket::Address addr;
  WORD port;
  PString type;
  PString source;
};

ostream & operator << (ostream & strm, const LookupRecord & rec) 
{
  strm << rec.type << " " << rec.addr << ":" << rec.port << " from " << rec.source;
  return strm;
}

BOOL FindSRVRecords(std::vector<LookupRecord> & recs,
                    const PString & domain,
                    const PString & type,
                    const PString & srv)
{
  PDNS::SRVRecordList srvRecords;
  PString srvLookupStr = srv + domain;
  BOOL found = PDNS::GetRecords(srvLookupStr, srvRecords);
  if (found) {
    PDNS::SRVRecord * recPtr = srvRecords.GetFirst();
    while (recPtr != NULL) {
      LookupRecord rec;
      rec.addr = recPtr->hostAddress;
      rec.port = recPtr->port;
      rec.type = type;
      rec.source = srv;
      recs.push_back(rec);
      recPtr = srvRecords.GetNext();
    }
  } 
  return found;
}

void LookupSRVURL(const PString & url, const PString & service)
{
  PStringList addrs;
  if (!PDNS::LookupSRV(url, service, addrs)) {
    cout << "no records returned by SRV lookup of " << url << " with service " << service << endl;
  } else {
    cout << setfill('\n') << addrs << setfill(' ');
  }
}

void LookupRDSURL(const PString & url, const PString & service)
{
  PStringList addrs;
  if (!PDNS::RDSLookup(url, service, addrs)) {
    cout << "no records returned by RDS lookup of " << url << " with service " << service << endl;
  } else {
    cout << setfill('\n') << addrs << setfill(' ');
  }
}

void DNSTest::Main()
{
  PArgList & args = GetArguments();

  args.Parse("t:u.");

  if (args.GetCount() < 1) {
    Usage();
    return;
  }

  if (args.HasOption('u')) {
    if (args.GetCount() < 0) {
      Usage();
      return;
    }

    PURL url(args[0]);
    if (url.GetScheme() *= "h323") {
      PString user   = url.GetUserName();
      PString domain = url.GetHostName();
      WORD    port   = url.GetPort();
      cout << "user = " << user << ", domain = " << domain << ", port = " << port << endl;

      std::vector<LookupRecord> found;

      BOOL hasGK = FindSRVRecords(found, domain, "LRQ",         "_h323ls._udp.");
      hasGK = hasGK || FindSRVRecords(found, domain, "LRQ",         "_h323rs._udp.");
      FindSRVRecords(found, domain, "Call direct", "_h323cs._tcp.");

      // if no entries so far, see if the domain is actually a host
      if (found.size() == 0) {
        PIPSocket::Address addr;
        if (PIPSocket::GetHostAddress(domain, addr)) {
          LookupRecord rec;
          rec.addr = addr;
          rec.port = 1720;
          rec.type = "Call direct";
          rec.source = "DNS";
          found.push_back(rec);
        }
      }

      if (!hasGK) {
        PDNS::MXRecordList mxRecords;
        if (PDNS::GetRecords(domain, mxRecords)) {
          PDNS::MXRecord * recPtr = mxRecords.GetFirst();
          while (recPtr != NULL) {
            LookupRecord rec;
            rec.addr = recPtr->hostAddress;
            rec.port = 1719;
            rec.type = "LRQ";
            rec.source = "MX";
            found.push_back(rec);
            recPtr = mxRecords.GetNext();
          }
        } 
      }

      if (found.size() == 0) {
        PError << "Cannot find match" << endl;
      }
      else
      {
        std::vector<LookupRecord>::const_iterator r;
        cout << "Found\n";
        for (r = found.begin(); r != found.end(); ++r) {
          cout << *r << endl;
        }
      }
    }
    else {
      PError << "error: unsupported scheme " << url.GetScheme() << endl;
    }
  }

  else if (args.HasOption('t')) {
    PString type = args.GetOptionString('t');
    if ((type *= "SRV") && (args.GetCount() == 1)) 
      GetAndDisplayRecords<PDNS::SRVRecordList>(args[0]);

    else if ((type *= "SRV") && (args.GetCount() == 2)) 
      LookupSRVURL(args[1], args[0]);

    else if ((type *= "RDS") && (args.GetCount() == 2)) 
      LookupRDSURL(args[1], args[0]);

    else if (type *= "MX")
      GetAndDisplayRecords<PDNS::MXRecordList>(args[0]);

    else if (type *= "NAPTR") {
      if (args.GetCount() == 1)
        GetAndDisplayRecords<PDNS::NAPTRRecordList>(args[0]);
      else {
        PDNS::NAPTRRecordList records;
        if (!PDNS::GetRecords(args[0], records))
          PError << "Lookup for " << args[0] << " failed" << endl;
        else {
          cout << "Returned " << endl;
          PDNS::NAPTRRecord * rec = records.GetFirst(args[1]);
          while (rec != NULL) {
            cout << *rec;
            rec = records.GetNext(args[1]);
          }
        }
      }
    }

    else if (type *= "enum") {
      if (args.GetCount() < 2)
        Usage();
      else {
        PString e164    = args[0];
        PString service = args[1];
        PString str;
        if (!PDNS::ENUMLookup(e164, service, str))
          cout << "Could not resolve E164 number " << e164 << " with service " << service << endl;
        else
          cout << "E164 number " << e164 << " with service " << service << " resolved to " << str << endl;
      }
    }
  }
}
  
// End of File ///////////////////////////////////////////////////////////////
