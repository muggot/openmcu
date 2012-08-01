# Microsoft Developer Studio Project File - Name="ilbccodec" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ilbccodec - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ilbccodec.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ilbccodec.mak" CFG="ilbccodec - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ilbccodec - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ilbccodec - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ilbccodec - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release"
# PROP Intermediate_Dir "..\..\Release\ILBC"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ilbccodec_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_CODEC_DLL_EXPORTS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Release/ILBC_pwplugin.dll" /implib:"..\..\Release\ILBC/ilbccodec.lib"

!ELSEIF  "$(CFG)" == "ilbccodec - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug"
# PROP Intermediate_Dir "..\..\Debug\ILBC"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ilbccodec_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_CODEC_DLL_EXPORTS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../../Debug/ILBC_pwplugin.dll" /implib:"..\..\Debug\ILBC/ilbccodec.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "ilbccodec - Win32 Release"
# Name "ilbccodec - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ilbccodec.c
# End Source File
# End Group
# Begin Group "iLBC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\iLBC\anaFilter.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\constants.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\createCB.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\doCPLC.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\enhancer.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\filter.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\FrameClassify.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\gainquant.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\getCBvec.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\helpfun.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\hpInput.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\hpOutput.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\iCBConstruct.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\iCBSearch.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\iLBC_decode.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\iLBC_encode.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\LPCdecode.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\LPCencode.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\lsf.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\packing.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\StateConstructW.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\StateSearchW.c
# End Source File
# Begin Source File

SOURCE=.\iLBC\syntFilter.c
# End Source File
# End Group
# End Target
# End Project
