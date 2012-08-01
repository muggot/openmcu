# Microsoft Developer Studio Project File - Name="PTLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=PTLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PTLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PTLib.mak" CFG="PTLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PTLib - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "PTLib - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 1
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PTLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\Lib"
# PROP Intermediate_Dir "..\..\..\Lib\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W4 /GR /GX /Zi /O2 /Ob2 /I "..\..\..\include\ptlib\msos" /I "..\..\..\include" /D "NDEBUG" /D "PTRACING" /Yu"ptlib.h" /Fd"..\..\..\Lib\PTLib.pdb" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ptclib.lib ptlibs.lib vfw32.lib winmm.lib mpr.lib snmpapi.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /dll /debug /debugtype:both /machine:I386 /libpath:"..\..\..\lib"
# Begin Special Build Tool
OutDir=.\..\..\..\Lib
TargetName=PTLib
SOURCE="$(InputPath)"
PostBuild_Desc=Extracting debug symbols
PostBuild_Cmds=rebase -b 0x10000000 -x . $(OutDir)\$(TargetName).dll
# End Special Build Tool

!ELSEIF  "$(CFG)" == "PTLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\Lib"
# PROP Intermediate_Dir "..\..\..\Lib\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W4 /GR /GX /Zi /Od /I "..\..\..\include\ptlib\msos" /I "..\..\..\include" /D "_DEBUG" /D "PTRACING" /Fd"..\..\..\Lib\PTLibd.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /fo"..\..\..\Lib\Debug/ptlib.res" /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 ptclibd.lib ptlibsd.lib vfw32.lib winmm.lib mpr.lib snmpapi.lib netapi32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\Lib\PTLibd.dll" /libpath:"..\lib"

!ENDIF 

# Begin Target

# Name "PTLib - Win32 Release"
# Name "PTLib - Win32 Debug"
# Begin Source File

SOURCE=.\dllmain.cxx
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\libver.rc

!IF  "$(CFG)" == "PTLib - Win32 Release"

# ADD BASE RSC /l 0xc09
# ADD RSC /l 0xc09 /fo"..\..\..\Lib\Release/ptlib.res" /d PRODUCT=PTLib

!ELSEIF  "$(CFG)" == "PTLib - Win32 Debug"

# ADD BASE RSC /l 0xc09
# ADD RSC /l 0xc09 /d PRODUCT=PTLib

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Lib\Release\ptlib.def

!IF  "$(CFG)" == "PTLib - Win32 Release"

!ELSEIF  "$(CFG)" == "PTLib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlib.dtf

!IF  "$(CFG)" == "PTLib - Win32 Release"

USERDEP__PTLIB="$(OutDir)\ptlibs.lib"	"$(InputDir)\ptlib.ignore"	
# Begin Custom Build - Merging exported library symbols
InputDir=\Storage\CVS_Head\pwlib\include\ptlib\msos
IntDir=.\..\..\..\Lib\Release
OutDir=.\..\..\..\Lib
TargetName=PTLib
InputPath=..\..\..\include\ptlib\msos\ptlib.dtf

"$(IntDir)\$(TargetName).def" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	MergeSym -I "$(Include)$(INCLUDE)" -x ../../../include/ptlib/msos/ptlib.ignore $(OutDir)\ptlibs.lib $(InputPath) 
	copy $(InputPath)+nul $(IntDir)\$(TargetName).def > nul 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "PTLib - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\Lib\Debug\PTLibd.def

!IF  "$(CFG)" == "PTLib - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PTLib - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\include\ptlib\msos\ptlibd.dtf

!IF  "$(CFG)" == "PTLib - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "PTLib - Win32 Debug"

# PROP Ignore_Default_Tool 1
USERDEP__PTLIBD="$(OutDir)\ptlibsd.lib"	"$(InputDir)\ptlib.ignore"	
# Begin Custom Build - Merging exported library symbols
InputDir=\Storage\CVS_Head\pwlib\include\ptlib\msos
IntDir=.\..\..\..\Lib\Debug
OutDir=.\..\..\..\Lib
TargetName=PTLibd
InputPath=..\..\..\include\ptlib\msos\ptlibd.dtf

"$(IntDir)\$(TargetName).def" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	MergeSym -I "$(Include)$(INCLUDE)" -x ../../../include/ptlib/msos/ptlib.ignore $(OutDir)\ptlibsd.lib $(InputPath) 
	copy $(InputPath)+nul $(IntDir)\$(TargetName).def  > nul 
	
# End Custom Build

!ENDIF 

# End Source File
# End Target
# End Project
