# Microsoft Developer Studio Project File - Name="gsm0610" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=gsm0610 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gsm0610.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gsm0610.mak" CFG="gsm0610 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gsm0610 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "gsm0610 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gsm0610 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release"
# PROP Intermediate_Dir "..\..\Release\GSM"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "gsm0610_EXPORTS" /Yu"stdafx.h" /FD /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Release/gsm0610_pwplugin.dll" /implib:"..\..\Release\GSM/gsm0610.lib"

!ELSEIF  "$(CFG)" == "gsm0610 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug"
# PROP Intermediate_Dir "..\..\Debug\GSM"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "gsm0610_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W4 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_CODEC_DLL_EXPORTS" /FD /GZ /c
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
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../../Debug/gsm0610_pwplugin.dll" /implib:"..\..\Debug\GSM/gsm0610.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "gsm0610 - Win32 Release"
# Name "gsm0610 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\gsm06_10_codec.c
# End Source File
# End Group
# Begin Group "GSM Files"

# PROP Default_Filter ".c"
# Begin Group "GSM Source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\add.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\code.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\decode.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\gsm_create.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\gsm_decode.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\gsm_destroy.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\gsm_encode.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\gsm_lpc.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\gsm_option.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\long_term.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\preprocess.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\rpe.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=.\src\short_term.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# Begin Source File

SOURCE=src\table.c
# ADD CPP /w /W0 /I "./inc" /D NeedFunctionPrototypes=1 /D "WAV49"
# SUBTRACT CPP /D "PLUGIN_CODEC_DLL_EXPORTS"
# End Source File
# End Group
# Begin Group "GSM Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\gsm\inc\config.h
# End Source File
# Begin Source File

SOURCE=.\src\gsm\inc\gsm.h
# End Source File
# Begin Source File

SOURCE=.\src\gsm\inc\private.h
# End Source File
# Begin Source File

SOURCE=.\src\gsm\inc\proto.h
# End Source File
# Begin Source File

SOURCE=.\src\gsm\inc\unproto.h
# End Source File
# End Group
# End Group
# End Target
# End Project
