/*
 * qos.cxx
 *
 * QOS class used by PWLIB dscp or Windows GQOS implementation.
 *
 * Copyright (c) 2003 AliceStreet Ltd
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
 * Contributor(s): ______________________________________.
 *
 * $Log: qos.cxx,v $
 * Revision 1.3  2006/11/02 03:05:19  csoutheren
 * Fixed incorrect comparison of QoS value
 * Thanks to Nick Hoath
 *
 * Revision 1.2  2003/10/27 04:06:13  csoutheren
 * Added code to allow compilation of new QoS code on Unix
 *
 * Revision 1.1  2003/10/27 03:20:10  csoutheren
 * Initial version of QoS implementation
 *   Thanks to Henry Harrison of AliceStreet
 *
 *
 */


#include <ptlib.h>
#include <ptlib/qos.h>

char PQoS::bestEffortDSCP = 0;
char PQoS::controlledLoadDSCP = 26;
char PQoS::guaranteedDSCP = 46;


PQoS::PQoS() 
{
    serviceType = SERVICETYPE_PNOTDEFINED;
    dscp = -1;
    tokenRate = QOS_NOT_SPECIFIED;
    tokenBucketSize = QOS_NOT_SPECIFIED;
    peakBandwidth = QOS_NOT_SPECIFIED;
}

PQoS::PQoS(int DSCPvalue) 
{
    serviceType = SERVICETYPE_PNOTDEFINED;
    dscp = DSCPvalue;
    tokenRate = QOS_NOT_SPECIFIED;
    tokenBucketSize = QOS_NOT_SPECIFIED;
    peakBandwidth = QOS_NOT_SPECIFIED;
}

PQoS::PQoS(DWORD avgBytesPerSec, 
           DWORD winServiceType,
           int DSCPalternative, 
           DWORD maxFrameBytes, 
           DWORD peakBytesPerSec)
{
    tokenRate = avgBytesPerSec;
    serviceType = winServiceType;
    dscp = DSCPalternative;
    tokenBucketSize = maxFrameBytes;
    peakBandwidth = peakBytesPerSec;
}

void PQoS::SetWinServiceType(DWORD winServiceType)
{
    serviceType = winServiceType;
}

void PQoS::SetAvgBytesPerSec(DWORD avgBytesPerSec)
{
    tokenRate = avgBytesPerSec;
}

void PQoS::SetDSCP(int DSCPvalue)
{
    if (DSCPvalue <= 63)
        dscp = DSCPvalue;
}

void PQoS::SetMaxFrameBytes(DWORD maxFrameBytes)
{
    tokenBucketSize = maxFrameBytes;
}

void PQoS::SetPeakBytesPerSec(DWORD peakBytesPerSec)
{
    peakBandwidth = peakBytesPerSec;
}


void PQoS::SetDSCPAlternative(DWORD winServiceType, UINT dscp)
{
    if (dscp < 63 &&
        winServiceType != SERVICETYPE_PNOTDEFINED)
    {
        switch (winServiceType)
        {
        case SERVICETYPE_BESTEFFORT:
             bestEffortDSCP = (char)dscp;
            break;
        case SERVICETYPE_CONTROLLEDLOAD:
            controlledLoadDSCP = (char)dscp;
            break;
        case SERVICETYPE_GUARANTEED:
            guaranteedDSCP = (char)dscp;
            break;
        }
    }
}
