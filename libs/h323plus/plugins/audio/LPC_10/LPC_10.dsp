# Microsoft Developer Studio Project File - Name="LPC_10" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=LPC_10 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LPC_10.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LPC_10.mak" CFG="LPC_10 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LPC_10 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "LPC_10 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release"
# PROP Intermediate_Dir "..\..\Release\LPC_10"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LPC_10_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_CODEC_DLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Release/LPC_10_pwplugin.dll" /implib:"..\..\Release\LPC_10/LPC_10.lib"

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug"
# PROP Intermediate_Dir "..\..\Debug\LPC_10"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "LPC_10_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_CODEC_DLL_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../../Debug/LPC_10_pwplugin.dll" /implib:"..\..\Debug\LPC_10/LPC_10.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "LPC_10 - Win32 Release"
# Name "LPC_10 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\lpc_10_codec.c
# End Source File
# End Group
# Begin Group "LPC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\analys.c
# ADD CPP /I "."
# End Source File
# Begin Source File

SOURCE=.\src\bsynz.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\chanwr.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\dcbias.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\decode_.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\deemp.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\difmag.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\dyptrk.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\encode_.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\energy.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\f2clib.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\ham84.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\hp100.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\invert.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\irc2pc.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\ivfilt.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\lpcdec.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\lpcenc.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\lpcini.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\lpfilt.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\median.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\mload.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\onset.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\pitsyn.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\placea.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\placev.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\preemp.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\prepro.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\random.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\rcchk.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\synths.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\tbdm.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\voicin.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\vparms.c

!IF  "$(CFG)" == "LPC_10 - Win32 Release"

# ADD CPP /I "."

!ELSEIF  "$(CFG)" == "LPC_10 - Win32 Debug"

# ADD CPP /I ".." /I "."

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
