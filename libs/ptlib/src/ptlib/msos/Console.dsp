# Microsoft Developer Studio Project File - Name="Console" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Console - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Console.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Console.mak" CFG="Console - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Console - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Console - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Console - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W4 /GR /GX /Zi /O2 /Ob2 /I "..\..\..\include" /D "NDEBUG" /D "PTRACING" /Yu"ptlib.h" /Fd"..\..\..\Lib\PTLib.pdb" /FD /c
# ADD BASE RSC /l 0xc09
# ADD RSC /l 0xc09
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Lib/PTLib.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Lib\ptlibs.lib"

!ELSEIF  "$(CFG)" == "Console - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W4 /GR /GX /Zi /Od /I "..\..\..\include" /D "_DEBUG" /D "PTRACING" /FR /Yu"ptlib.h" /Fd"..\..\..\Lib\PTLibd.pdb" /FD /c
# ADD BASE RSC /l 0xc09
# ADD RSC /l 0xc09
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"..\..\..\Lib\PTLib.bsc"
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\Lib\ptlibsd.lib"

!ENDIF 

# Begin Target

# Name "Console - Win32 Release"
# Name "Console - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\assert.cxx
# ADD CPP /Yc"ptlib.h"
# End Source File
# Begin Source File

SOURCE=..\common\collect.cxx
# End Source File
# Begin Source File

SOURCE=..\common\contain.cxx
# End Source File
# Begin Source File

SOURCE=.\ethsock.cxx
# End Source File
# Begin Source File

SOURCE=..\common\getdate.y

!IF  "$(CFG)" == "Console - Win32 Release"

# Begin Custom Build
InputPath=..\common\getdate.y

"../common/getdate_tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison -o ../common/getdate_tab.c ../common/getdate.y

# End Custom Build

!ELSEIF  "$(CFG)" == "Console - Win32 Debug"

# Begin Custom Build
InputPath=..\common\getdate.y

"../common/getdate_tab.c" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	bison -o ../common/getdate_tab.c ../common/getdate.y

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\getdate_tab.c

!IF  "$(CFG)" == "Console - Win32 Release"

# ADD CPP /Ob0
# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "Console - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\icmp.cxx
# End Source File
# Begin Source File

SOURCE=.\mail.cxx
# End Source File
# Begin Source File

SOURCE=..\common\notifier_ext.cxx
# End Source File
# Begin Source File

SOURCE=..\common\object.cxx
# End Source File
# Begin Source File

SOURCE=..\common\osutils.cxx

!IF  "$(CFG)" == "Console - Win32 Release"

# ADD CPP /Ob0

!ELSEIF  "$(CFG)" == "Console - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Common\pchannel.cxx
# End Source File
# Begin Source File

SOURCE=..\Common\pconfig.cxx
# End Source File
# Begin Source File

SOURCE=..\common\pethsock.cxx
# End Source File
# Begin Source File

SOURCE=.\pipe.cxx
# End Source File
# Begin Source File

SOURCE=..\common\pipechan.cxx
# End Source File
# Begin Source File

SOURCE=..\common\pluginmgr.cxx

!IF  "$(CFG)" == "Console - Win32 Release"

# ADD CPP /Ob0

!ELSEIF  "$(CFG)" == "Console - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\podbc.cxx
# End Source File
# Begin Source File

SOURCE=..\common\ptime.cxx
# End Source File
# Begin Source File

SOURCE=.\ptlib.cxx
# End Source File
# Begin Source File

SOURCE=..\common\pvidchan.cxx
# End Source File
# Begin Source File

SOURCE=..\common\qos.cxx
# End Source File
# Begin Source File

SOURCE=.\remconn.cxx
# End Source File
# Begin Source File

SOURCE=..\common\safecoll.cxx
# End Source File
# Begin Source File

SOURCE=..\Common\serial.cxx
# End Source File
# Begin Source File

SOURCE=..\common\sfile.cxx
# End Source File
# Begin Source File

SOURCE=..\common\sockets.cxx
# End Source File
# Begin Source File

SOURCE=..\common\sound.cxx
# End Source File
# Begin Source File

SOURCE=.\sound_win32.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\svcproc.cxx
# End Source File
# Begin Source File

SOURCE=..\common\vconvert.cxx
# End Source File
# Begin Source File

SOURCE=..\common\vfakeio.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\vfw.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\videoio.cxx
# End Source File
# Begin Source File

SOURCE=.\win32.cxx
# End Source File
# Begin Source File

SOURCE=.\wincfg.cxx
# End Source File
# Begin Source File

SOURCE=.\winserial.cxx
# End Source File
# Begin Source File

SOURCE=.\winsock.cxx
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Args.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Array.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Channel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Contain.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\critsec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Dict.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Dynalink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Ethsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\File.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\filepath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Icmpsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Indchan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Ipdsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\ipsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\ipxsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Lists.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Mail.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\notifier_ext.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\object.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Pdirect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\pfactory.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Pipechan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\plugin.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\pluginmgr.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\pprocess.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Pstring.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Ptime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Remconn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\safecoll.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\semaphor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Serchan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Sfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\socket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\sockets.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Sound.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\spxsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Svcproc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\syncpoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\syncthrd.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Tcpsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Textfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Timeint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\PtLib\Udpsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\vconvert.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\videoio.h
# End Source File
# End Group
# Begin Group "MSOS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\channel.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\contain.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\critsec.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\debstrm.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\dynalink.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\epacket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\ethsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\file.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\filepath.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\icmpsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\ipdsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\ipsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\ipxsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\mail.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\pdirect.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\pipechan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\PODBC.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\pprocess.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\ptime.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\remconn.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\semaphor.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\serchan.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\sfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\socket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\spxsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\svcproc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\syncpoint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\tcpsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\textfile.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\timeint.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\udpsock.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\videoio.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\include\ptbuildopts.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptbuildopts.h.in

!IF  "$(CFG)" == "Console - Win32 Release"

# Begin Custom Build - Configuring Build Options
InputDir=\Storage\CVS_Head\pwlib\include
InputPath=..\..\..\include\ptbuildopts.h.in

"$(InputDir)\ptbuildopts.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd ..\..\..\tools\msvc6chk 
	.\msvc6chk 
	if errorlevel 1 exit %ERRORLEVEL% 
	cd ..\.. 
	.\configure --exclude-env=MSVC_PWLIB_CONFIGURE_EXCLUDE_DIRS 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Console - Win32 Debug"

# Begin Custom Build - Configuring Build Options
InputDir=\Storage\CVS_Head\pwlib\include
InputPath=..\..\..\include\ptbuildopts.h.in

"$(InputDir)\ptbuildopts.h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd ..\..\..\tools\msvc6chk 
	.\msvc6chk 
	if errorlevel 1 exit %ERRORLEVEL% 
	cd ..\.. 
	.\configure --exclude-env=MSVC_PWLIB_CONFIGURE_EXCLUDE_DIRS 
	
# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Inlines"

# PROP Default_Filter ".inl"
# Begin Source File

SOURCE=..\..\..\include\ptlib\contain.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\osutil.inl
# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib\ptlib.inl
# End Source File
# End Group
# Begin Group "Regex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\regex\regcomp.c
# ADD CPP /W1 /I "..\common\regex"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\regex\regerror.c
# ADD CPP /W1 /I "..\common\regex"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\regex\regexec.c
# ADD CPP /W1 /I "..\common\regex"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\common\regex\regfree.c
# ADD CPP /W1 /I "..\common\regex"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# End Target
# End Project
