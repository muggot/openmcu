# Microsoft Developer Studio Project File - Name="Console Components" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Console Components - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Console Components.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Console Components.mak" CFG="Console Components - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Console Components - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Console Components - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Console Components - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Lib"
# PROP Intermediate_Dir "..\..\..\Lib\Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /Zi /O2 /Ob2 /I "..\..\..\include" /D "NDEBUG" /D "PTRACING" /Yu"ptlib.h" /Fd"..\..\..\Lib\PTCLib.pdb" /FD /c
# ADD BASE RSC /l 0xc09
# ADD RSC /l 0xc09
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Lib\ptclib.lib"

!ELSEIF  "$(CFG)" == "Console Components - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Lib"
# PROP Intermediate_Dir "..\..\..\Lib\Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /GR /GX /Zi /Od /I "..\..\..\include" /D "_DEBUG" /D "PTRACING" /FR /Yu"ptlib.h" /Fd"..\..\..\Lib\PTCLibd.pdb" /FD /c
# ADD BASE RSC /l 0xc09
# ADD RSC /l 0xc09
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Lib\ptclibd.lib"

!ENDIF 

# Begin Target

# Name "Console Components - Win32 Release"
# Name "Console Components - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "*.cxx"
# Begin Source File

SOURCE=..\..\ptclib\asnber.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Asner.cxx

!IF  "$(CFG)" == "Console Components - Win32 Release"

!ELSEIF  "$(CFG)" == "Console Components - Win32 Debug"

# ADD CPP /Yu"ptlib.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\ptclib\asnper.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\asnxer.cxx
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Cypher.cxx
# ADD CPP /Yc"ptlib.h"
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\delaychan.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\dtmf.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\enum.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Ftp.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Ftpclnt.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Ftpsrvr.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Html.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Http.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Httpclnt.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Httpform.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Httpsrvr.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Httpsvc.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Inetmail.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Inetprot.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\ipacl.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\memfile.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\modem.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Pasn.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pdns.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pils.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pldap.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pnat.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\psasl.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Psnmp.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\psoap.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pssl.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pstun.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\ptts.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pvfiledev.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pvidfile.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pwavfile.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pxml.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pxmlrpc.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\pxmlrpcs.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\qchannel.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\random.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\shttpsvc.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Snmpclnt.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Snmpserv.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\socks.cxx
# End Source File
# Begin Source File

SOURCE=..\..\Ptclib\Telnet.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\vsdl.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\vxml.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\xmpp.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\xmpp_c2s.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\xmpp_muc.cxx
# End Source File
# Begin Source File

SOURCE=..\..\ptclib\xmpp_roster.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\..\include\ptclib\asnber.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\asner.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\asnper.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\asnxer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\cypher.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\delaychan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\dtmf.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\enum.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\ftp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\html.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\http.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\httpform.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\httpsvc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\inetmail.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\inetprot.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\ipacl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\memfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\mime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\modem.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\pasn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pdns.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pils.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pldap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pnat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\psasl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\psnmp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\psoap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\pssl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pstun.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\ptts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pvfiledev.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pvidfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pwavfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pxml.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pxmlrpc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\pxmlrpcs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\qchannel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\random.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\shttpsvc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\socks.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\telnet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtCLib\url.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\vsdl.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\vxml.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\xmpp.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\xmpp_c2s.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\xmpp_muc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptclib\xmpp_roster.h
# End Source File
# End Group
# End Target
# End Project
