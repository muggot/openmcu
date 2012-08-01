#include <snmp.h>

#define SNMP_FUNC_TYPE   WINAPI

BOOL 
SNMP_FUNC_TYPE
SnmpExtensionInit(
    DWORD                 dwUptimeReference,    
    HANDLE *              phSubagentTrapEvent,  
    AsnObjectIdentifier * pFirstSupportedRegion 
    ) 
{ return FALSE; }

BOOL
SNMP_FUNC_TYPE
SnmpExtensionQuery(
    BYTE              bPduType,    
    SnmpVarBindList * pVarBindList,
    AsnInteger32 *    pErrorStatus, 
    AsnInteger32 *    pErrorIndex  
    )
{ return FALSE; }

SNMPAPI
SNMP_FUNC_TYPE
SnmpUtilOidCpy(
    AsnObjectIdentifier * pOidDst,
    AsnObjectIdentifier * pOidSrc
    )
{ return -1; }

SNMPAPI
SNMP_FUNC_TYPE
SnmpUtilOidAppend(
    AsnObjectIdentifier * pOidDst,
    AsnObjectIdentifier * pOidSrc
    )
{ return -1; }

SNMPAPI
SNMP_FUNC_TYPE
SnmpUtilOidNCmp(
    AsnObjectIdentifier * pOid1,
    AsnObjectIdentifier * pOid2,
    UINT                  nSubIds
    )
{ return 1; }

SNMPAPI
SNMP_FUNC_TYPE
SnmpUtilOidCmp(
    AsnObjectIdentifier * pOid1,
    AsnObjectIdentifier * pOid2
    )
{ return 1; }

VOID
SNMP_FUNC_TYPE
SnmpUtilOidFree(
    AsnObjectIdentifier * pOid
    ) 
{}

VOID
SNMP_FUNC_TYPE
SnmpUtilMemFree(
    LPVOID pMem
    ) 
{}

LPVOID
SNMP_FUNC_TYPE
SnmpUtilMemAlloc(
     UINT nBytes
    )
{ return  FALSE; }


