 README
==========
==========


This document explains the revised QOS implementation for PTLIB and openh323.

============
Update 25.4.05:
802.1p/q is now supported natively however is disabled 
To enable set the Static PUDPSocket::disableQoS = FALSE;

QoS support is now negotiated between the calling parties
and will only functions when both parties have.
1. Enabled QoS within Openh323 (as above)
2. Window XP OS
3. Managed NIC with 802.1p enabled.(May have to Set Manually)

============

Contents:

1) Setting a QOS specification in openh323
2) Diffserv support
3) 802.1Q support
4) Windows 2000 issues
5) Windows XP and Server 2003 issues

1) Setting a QOS specification in openh323
==========================================

The basic approach is as follows:

****

RTP_QOS * rtpqos = new RTP_QOS;
rtpqos->dataQoS.SetWinServiceType(DWORD winServiceType);
rtpqos->dataQoS.SetAvgBytesPerSec(DWORD avgBytesPerSec);
rtpqos->dataQoS.SetMaxFrameBytes(DWORD maxFrameBytes);
rtpqos->dataQoS.SetPeakBytesPerSec(DWORD peakBytesPerSec);
rtpqos->dataQoS.SetDSCP(int DSCPvalue);

[If desired:
rtpqos->controlQoS.SetWinServiceType(...)
etc.]

H323Capability * cap = new xxxxx;
cap->AttachQoS(rtpqos);

SetCapability(....);

****

Notes:

i) winServiceType is either SERVICETYPE_GUARANTEED,
SERVICETYPE_CONTROLLEDLOAD, SERVICETYPE_BESTEFFORT, or
SERVICETYPE_PNOTSPECIFIED

ii) for Windows XP or Windows Server 2003, if setting aaaaa as
SERVICETYPE_GUARANTEED or SERVICETYPE_CONTROLLEDLOAD then avgBytesPerSec,
maxFrameBytes, and peakBytesPerSec MUST be set to appropriate values

iii) Calling SetDSCP(DSCPvalue) has no effect on Windows XP or Windows Server
2003 but on other platforms (earlier versions of Windows, and Linux) it will
set the DSCP directly. SetDSCP(-1) clears the effect of any previous call to
SetDSCP()

iv) on platforms other than WinXP or Windows Server 2003, if SetDSCP(...) is
not called but SetWinServiceType(...) is, then an appropriate DSCP
substitution will be made. The substitutions can be changed globally by
calling PQoS::SetDSCPAlternative(DWORD winServiceType, UINT DSCP)

v) if an RTP_QOS is not attached to a capability, the
H323Endpoint::rtpIpTypeofService will be used to set IP_TOS but this has no
effect on WinXP or Windows Server 2003 (and see below for notes relating to
Windows 2000)


2) Diffserv support
===================

Setting of DSCP on packets is supported for most Windows and Linux platforms.
The mechanism is platform-dependent:

Windows XP and Windows Server 2003:
-----------------------------------

Based on the parameters passed to PQoS::SetWinServiceType(...),
PQoS::SetAvgBytesPerSec(...), PQoS::SetMaxFrameBytes(...) and
PQoS::SetPeakBytesPerSec(...)

Linux and earlier versions of Windows:
--------------------------------------

Based either on:

a) the parameter passed to PQoS::SetDSCP(...), or
b) the parameter passed to PQoS::SetWinServiceType(...) plus (optionally) the
parameters passed to any calls to PQoS::SetDSCPAlternative(...)

Unsupported Windows versions
----------------------------

Windows ME - setsockopt/IP_TOS appears to be unsupported and it is impossible
to turn off RSVP signalling for GQOS
Windows NT4 without SP4 - there is a known problem using setsockopt/IP_TOS on
unconnected UDP sockets. SP4 solves this problem (see Microsoft KB article
Q196358).


3) 802.1Q support
=================

Where enabled, 802.1Q is supported on Windows XP and Windows Server 2003 based
on the same calls as for Diffserv support on these platforms

A note for developers
---------------------

In principle, it is possible to support 802.1Q on Windows 2000, but this is
not implemented for the following reason.

Currently, Winsock support in openh323 writes to UDP sockets using sendto(...)
on unconnected UDP sockets. As a result, setting QOS using GQOS (which enables
802.1Q) requires that the QUALITYOFSERVICE structure include a QOS_DESTADDR.
QOS_DESTADDR is not supported on Windows 2000.

If anyone wants to rewrite pwlib winsock support to use WSASendTo, or openh323
to use connected UDP sockets, it should then - in principle - be possible to
remove the requirement for QOS_DESTADDR and use the GQOS API on Windows 2000.
However, note the "in principle". This principle has not been tested!


4) Windows 2000 support
=======================

Diffserv support on Windows 2000 is implemented using the traditional
setsockopt(...) approach with an IP_TOS parameter. The reason for using this
approach rather than the GQOS API is explained in the note to section 3 above.

In order to enable the use of setsockopt(...) with IP_TOS on Windows 2000, it
is necessary to set the following registry setting (unset by default):

HKLM\System\CurrentControlSet\Services\Tcpip\Parameters\DisableUserTOSSetting
= 0

After setting this, you will need to reboot the machine.

A note for those horrified by this
----------------------------------

Unfortunately, even if someone does enable the use of GQOS on Windows 2000, it
will still be necessary to make a non-default registry setting. This is
because by default, Windows 2000 implements RSVP (ie intserv rather than
diffserv). On the grounds that nobody actually uses intserv, PTLIB disables
RSVP in its calls to GQOS, but then to enable diffserv it is necessary to set:

HKLM\System\CurrentControlSet\Services\Qossp\EnablePriorityBoost = 1

Sorry! Blame Microsoft...


5) Windows XP and Server 2003 support
=====================================

The approach used for QOS support on these platforms does not allow the DSCP
or 802.1Q COS to be set directly. However, it should be possible for the
system administrator to modify the DSCP that is used for any of the service
types specified in the call to PQoS::SetWinServiceType(...).

To do this:

* run "mmc"
* go to File\Add/Remove Snap-in
* add the Group Policy object (shows up as Local Computer Policy)
* within Local COmputer Policy, go to:

Computer Configuration\Administrative Templates\Netrwork\QoS Packet Scheduler.

This will allow you to configure DSCP and Layer-2 priority values (802.1Q
COS). Note that only Guaranteed service type, Controlled load service type,
and Best effort service type are supported.
