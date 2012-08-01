# Microsoft Developer Studio Project File - Name="ASNParser" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ASNPARSER - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "asnparser.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "asnparser.mak" CFG="ASNPARSER - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ASNParser - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ASNParser - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ASNParser - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W4 /GR /GX /O2 /I "..\..\include" /D "NDEBUG" /Yu"ptlib.h" /FD /c
# ADD BASE RSC /l 0xc09 /d "NDEBUG"
# ADD RSC /l 0xc09 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 ptlib.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /libpath:"..\..\lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=Copying ASN parser to pwlib/lib directory.
PostBuild_Cmds=copy Release\asnparser.exe ..\..\lib > nul
# End Special Build Tool

!ELSEIF  "$(CFG)" == "ASNParser - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W4 /GR /GX /ZI /Od /I "..\..\include" /D "_DEBUG" /Yu"ptlib.h" /FD /c
# ADD BASE RSC /l 0xc09 /d "_DEBUG"
# ADD RSC /l 0xc09 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ptlibd.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept /libpath:"..\..\lib"

!ENDIF 

# Begin Target

# Name "ASNParser - Win32 Release"
# Name "ASNParser - Win32 Debug"
# Begin Source File

SOURCE=.\asn_grammar.cxx
# ADD CPP /D "MSDOS"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\asn_grammar.h
# End Source File
# Begin Source File

SOURCE=.\asn_grammar.y

!IF  "$(CFG)" == "ASNParser - Win32 Release"

# Begin Custom Build - Performing Bison Step
InputPath=.\asn_grammar.y
InputName=asn_grammar

BuildCmds= \
	bison -t -v -d $(InputName).y \
	copy $(InputName)_tab.c $(InputName).cxx \
	copy $(InputName)_tab.h $(InputName).h \
	

"$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ELSEIF  "$(CFG)" == "ASNParser - Win32 Debug"

# Begin Custom Build - Performing Bison Step
InputPath=.\asn_grammar.y
InputName=asn_grammar

BuildCmds= \
	bison -t -v -d $(InputName).y \
	copy $(InputName)_tab.c $(InputName).cxx \
	copy $(InputName)_tab.h $(InputName).h \
	

"$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)

"$(InputName).h" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
   $(BuildCmds)
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\asn_lex.cxx
# ADD CPP /W2 /I "..\..\include\ptlib\msos"
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\asn_lex.l

!IF  "$(CFG)" == "ASNParser - Win32 Release"

USERDEP__ASN_L="asn_grammar.h"	"asn_grammar.cxx"	
# Begin Custom Build - Performing Lex Step
InputPath=.\asn_lex.l
InputName=asn_lex

"$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -t $(InputName).l > $(InputName).cxx

# End Custom Build

!ELSEIF  "$(CFG)" == "ASNParser - Win32 Debug"

USERDEP__ASN_L="asn_grammar.h"	"asn_grammar.cxx"	
# Begin Custom Build - Performing Lex Step
InputPath=.\asn_lex.l
InputName=asn_lex

"$(InputName).cxx" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	flex -t $(InputName).l > $(InputName).cxx

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.cxx
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\PreCompile.cpp
# ADD CPP /Yc"ptlib.h"
# End Source File
# End Target
# End Project
