/*
 * pdns.h
 *
 * PWLib library for DNS lookup services
 *
 * Portable Windows Library
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
 * $Log: pdns.h,v $
 * Revision 1.14  2007/09/11 13:38:56  hfriederich
 * Allow to do lookup SRV records using complete SRV query string
 *
 * Revision 1.13  2007/08/13 00:56:30  rjongbloed
 * Fixed compile on DevStudio 2003
 *
 * Revision 1.12  2007/08/10 10:07:29  dsandras
 * Fixed DNS support thanks to Vincent Luba <luba novacom be>.
 *
 * Revision 1.11  2006/02/26 11:51:20  csoutheren
 * Extended DNS test program to include URL based SRV lookups
 * Re-arranged SRV lookup code to allow access to internal routine
 * Reformatted code
 *
 * Revision 1.10  2006/02/26 09:26:17  shorne
 * Added DNS SRV record lookups
 *
 * Revision 1.9  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.8  2004/06/24 07:36:24  csoutheren
 * Added definitions of T_SRV and T_NAPTR for hosts that do not have these
 *
 * Revision 1.7  2004/05/31 12:49:47  csoutheren
 * Added handling of unknown DNS types
 *
 * Revision 1.6  2004/05/28 06:50:42  csoutheren
 * Reorganised DNS functions to use templates, and exposed more internals to allow new DNS lookup types to be added
 *
 * Revision 1.5  2003/07/22 23:52:20  dereksmithies
 * Fix from Fabrizio Ammollo to cope with when P_DNS is disabled. Thanks!
 *
 * Revision 1.4  2003/04/16 07:02:55  robertj
 * Cleaned up source.
 *
 * Revision 1.3  2003/04/15 08:14:06  craigs
 * Added single string form of GetSRVRecords
 *
 * Revision 1.2  2003/04/15 08:06:24  craigs
 * Added Unix implementation
 *
 * Revision 1.1  2003/04/15 04:06:56  craigs
 * Initial version
 *
 */

#if P_DNS
#ifndef _PDNS_H
#define _PDNS_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/sockets.h>

#include <ptclib/random.h>
#include <ptclib/url.h>

#if defined(_WIN32)

#  include <windns.h>
#  pragma comment(lib, P_DNS_LIBRARY)

// Accommodate spelling error in windns.h
enum { DnsSectionAdditional = DnsSectionAddtional };

#else

#  define  P_HAS_RESOLVER 1         // set if using Unix-style DNS routines
#  include <arpa/nameser.h>
#  include <resolv.h>
#  if defined(P_MACOSX) && (P_MACOSX >= 700)
#    include <arpa/nameser_compat.h>
#  endif

#endif  // _WIN32

#ifdef P_HAS_RESOLVER

//////////////////////////////////////////////////////////////////////////
//
// these classes provide an emulation of the Microsoft DNS API 
// on non-Window systems
//

#ifndef T_SRV
#define T_SRV   33
#endif

#ifndef T_NAPTR
#define T_NAPTR   35
#endif


#define DNS_STATUS  int
#define DNS_TYPE_SRV  T_SRV
#define DNS_TYPE_MX  T_MX
#define DNS_TYPE_A  T_A
#define DNS_TYPE_NAPTR  T_NAPTR
#define DnsFreeRecordList 0
#define DNS_QUERY_STANDARD 0
#define DNS_QUERY_BYPASS_CACHE 0

typedef struct _DnsAData {
  DWORD IpAddress;
} DNS_A_DATA;

typedef struct {
  char   pNameExchange[MAXDNAME];
  WORD   wPreference;
} DNS_MX_DATA;

typedef struct {
  char pNameHost[MAXDNAME];
} DNS_PTR_DATA;

typedef struct _DnsSRVData {
  char   pNameTarget[MAXDNAME];
  WORD   wPriority;
  WORD   wWeight;
  WORD   wPort;
} DNS_SRV_DATA;

typedef struct _DnsNULLData {
  DWORD  dwByteCount;
  char   data[1];
} DNS_NULL_DATA;

typedef struct _DnsRecordFlags
{
  unsigned   Section     : 2;
  unsigned   Delete      : 1;
  unsigned   CharSet     : 2;
  unsigned   Unused      : 3;
  unsigned   Reserved    : 24;
} DNS_RECORD_FLAGS;

typedef enum _DnsSection
{
  DnsSectionQuestion,
  DnsSectionAnswer,
  DnsSectionAuthority,
  DnsSectionAdditional,
} DNS_SECTION;


class DnsRecord {
  public:
    DnsRecord * pNext;
    char        pName[MAXDNAME];
    WORD        wType;
    WORD        wDataLength;

    union {
      DWORD               DW;     ///< flags as DWORD
      DNS_RECORD_FLAGS    S;      ///< flags as structure
    } Flags;

    union {
      DNS_A_DATA     A;
      DNS_MX_DATA    MX;
      DNS_PTR_DATA   NS;
      DNS_SRV_DATA   SRV;
      DNS_NULL_DATA  Null;
    } Data;
};

typedef DnsRecord * PDNS_RECORD;

extern void DnsRecordListFree(PDNS_RECORD rec, int FreeType);

extern DNS_STATUS DnsQuery_A(const char * service,
          WORD requestType,
          DWORD options,
          void *,
          PDNS_RECORD * results,
          void *);


#endif // P_HAS_RESOLVER

namespace PDNS {

//////////////////////////////////////////////////////////////////////////
//
//  this template automates the creation of a list of records for
//  a specific type of DNS lookup
//

template <unsigned type, class RecordListType, class RecordType>
BOOL Lookup(const PString & name, RecordListType & recordList)
{
  if (name.IsEmpty())
    return FALSE;

  recordList.RemoveAll();

  PDNS_RECORD results = NULL;
  DNS_STATUS status = DnsQuery_A((const char *)name, 
                                 type,
                                 DNS_QUERY_STANDARD, 
                                 NULL, 
                                 &results, 
                                 NULL);
  if (status != 0)
    return FALSE;

  // find records matching the correct type
  PDNS_RECORD dnsRecord = results;
  while (dnsRecord != NULL) {
    RecordType * record = recordList.HandleDNSRecord(dnsRecord, results);
    if (record != NULL)
      recordList.Append(record);
    dnsRecord = dnsRecord->pNext;
  }

  if (results != NULL)
    DnsRecordListFree(results, DnsFreeRecordList);

  return recordList.GetSize() != 0;
}

/////////////////////////////////////////////////////////////

class SRVRecord : public PObject
{
  PCLASSINFO(SRVRecord, PObject);
  public:
    SRVRecord()
    { used = FALSE; }

    Comparison Compare(const PObject & obj) const;
    void PrintOn(ostream & strm) const;

    PString            hostName;
    PIPSocket::Address hostAddress;
    BOOL               used;
    WORD port;
    WORD priority;
    WORD weight;
};

PDECLARE_SORTED_LIST(SRVRecordList, PDNS::SRVRecord)
  public:
    void PrintOn(ostream & strm) const;

    SRVRecord * GetFirst();
    SRVRecord * GetNext();

    PDNS::SRVRecord * HandleDNSRecord(PDNS_RECORD dnsRecord, PDNS_RECORD results);

  protected:
    PINDEX     priPos;
    PWORDArray priList;
};

/**
  * return a list of DNS SRV record with the specified service type
  */

inline BOOL GetRecords(const PString & service, SRVRecordList & serviceList)
{ return Lookup<DNS_TYPE_SRV, SRVRecordList, SRVRecord>(service, serviceList); }

/**
  * provided for backwards compatibility
  */
inline BOOL GetSRVRecords(
      const PString & service,
      SRVRecordList & serviceList
)
{ return GetRecords(service, serviceList); }

/**
  * return a list of DNS SRV record with the specified service, type and domain
  */

BOOL GetSRVRecords(
      const PString & service,
      const PString & type,
      const PString & domain,
      SRVRecordList & serviceList
);

/**
  * Perform a DNS lookup of the specified service
  * @return TRUE if the service could be resolved, else FALSE
  */

BOOL LookupSRV(
         const PString & srvQuery,
         WORD defaultPort,
         PIPSocketAddressAndPortVector & addrList
);

BOOL LookupSRV( 
         const PString & domain,                  ///< domain to lookup
         const PString & service,                 ///< service to use
         WORD defaultPort,                        ///< default por to use
         PIPSocketAddressAndPortVector & addrList ///< returned list of sockets and ports
); 

BOOL LookupSRV( 
         const PURL & url,          ///< URL to lookup
         const PString & service,   ///< service to use
         PStringList & returnStr    ///< resolved addresses, if return value is TRUE
);  

////////////////////////////////////////////////////////////////

class MXRecord : public PObject
{
  PCLASSINFO(MXRecord, PObject);
  public:
    MXRecord()
    { used = FALSE; }
    Comparison Compare(const PObject & obj) const;
    void PrintOn(ostream & strm) const;

    PString            hostName;
    PIPSocket::Address hostAddress;
    BOOL               used;
    WORD               preference;
};

PDECLARE_SORTED_LIST(MXRecordList, PDNS::MXRecord)
  public:
    void PrintOn(ostream & strm) const;

    MXRecord * GetFirst();
    MXRecord * GetNext();

    PDNS::MXRecord * HandleDNSRecord(PDNS_RECORD dnsRecord, PDNS_RECORD results);

  protected:
    PINDEX lastIndex;
};

/**
  * return a list of MX records for the specified domain
  */
inline BOOL GetRecords(
      const PString & domain,
      MXRecordList & serviceList
)
{ return Lookup<DNS_TYPE_MX, MXRecordList, MXRecord>(domain, serviceList); }

/**
  * provided for backwards compatibility
  */
inline BOOL GetMXRecords(
      const PString & domain,
      MXRecordList & serviceList
)
{
  return GetRecords(domain, serviceList);
}

///////////////////////////////////////////////////////////////////////////

}; // namespace PDNS

#endif // _PDNS_H
#endif // P_DNS

// End Of File ///////////////////////////////////////////////////////////////
