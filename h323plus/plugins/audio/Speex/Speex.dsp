# Microsoft Developer Studio Project File - Name="Speex" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Speex - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Speex.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Speex.mak" CFG="Speex - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Speex - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Speex - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Speex - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\Release"
# PROP Intermediate_Dir "..\..\Release\Speex"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPEEX_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_CODEC_DLL_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"../../Release/speex_pwplugin.dll" /implib:"..\..\Release\Speex/speexcodec.lib"

!ELSEIF  "$(CFG)" == "Speex - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\Debug"
# PROP Intermediate_Dir "..\..\Debug\Speex"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "SPEEX_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "PLUGIN_CODEC_DLL_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /incremental:no /debug /machine:I386 /out:"../../Debug/speex_pwplugin.dll" /implib:"..\..\Debug\Speex/speexcodec.lib" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "Speex - Win32 Release"
# Name "Speex - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\speexcodec.cxx
# ADD CPP /W1
# End Source File
# End Group
# Begin Group "Speex"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libspeex\bits.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\cb_search.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\exc_10_16_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\exc_10_32_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\exc_20_32_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\exc_5_256_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\exc_5_64_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\exc_8_128_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\filters.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\gain_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\gain_table_lbr.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\hexc_10_32_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\hexc_table.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\high_lsp_tables.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\lpc.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\lsp.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\lsp_tables_nb.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\ltp.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\math_approx.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\misc.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\modes.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\nb_celp.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\quant_lsp.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\sb_celp.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\speex_callbacks.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\speex_header.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\stereo.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\vbr.c
# ADD CPP /W1
# End Source File
# Begin Source File

SOURCE=.\libspeex\vq.c
# ADD CPP /W1
# End Source File
# End Group
# End Target
# End Project
