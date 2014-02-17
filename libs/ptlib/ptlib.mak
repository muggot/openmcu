
!IFNDEF PWLIBDIR
PWLIBDIR=c:\work\pwlib
!ENDIF

INCLUDE=$(INCLUDE);$(PWLIBDIR)\include\ptlib\msos;$(PWLIBDIR)\include\pwlib\mswin;$(PWLIBDIR)\include
LIB=$(LIB);$(PWLIBDIR)\Lib

!IFDEF OPENSSLDIR
OPENSSLFLAG=1
OPENSSLLIBS=ssleay32.lib libeay32.lib
!ENDIF

!IFDEF EXPATDIR
EXPATFLAG=1
EXPATLIBS=expat.lib
!ENDIF

all:
	cd src\ptlib\msos
	msdev Console.dsp /MAKE "Console - Win32 Release" /USEENV
	cd ..\..\..\tools\mergesym
	msdev MergeSym.dsp /MAKE "MergeSym - Win32 Release" /USEENV
	cd ..\..\src\ptlib\msos
	msdev PTLib.dsp /MAKE "PTLib - Win32 Release" /USEENV
	msdev "Console Components.dsp" /MAKE "Console Components - Win32 Release" /USEENV
	cd ..\..\pwlib\mswin
	msdev GUI.dsp /MAKE "GUI - Win32 Release" /USEENV
	msdev PWLib.dsp /MAKE "PWLib - Win32 Release" /USEENV
	msdev "GUI Components.dsp" /MAKE "GUI Components - Win32 Release" /USEENV
	cd ..\..\..\tools\asnparser
	msdev ASNParser.dsp /MAKE "ASNParser - Win32 Release" /USEENV
	cd ..\pwrc
	msdev pwrc.dsp /MAKE "pwrc - Win32 Release" /USEENV
	cd ..\pwtest
	msdev PWTest.dsp /MAKE "pwtest - Win32 Release" /USEENV


debug: all
	cd src\ptlib\msos
	msdev Console.dsp /MAKE "Console - Win32 Debug" /USEENV
	msdev PTLib.dsp /MAKE "PTLib - Win32 Debug" /USEENV
	msdev "Console Components.dsp" /MAKE "Console Components - Win32 Debug" /USEENV
	cd ..\..\pwlib\mswin
	msdev GUI.dsp /MAKE "GUI - Win32 Debug" /USEENV
	msdev PWLib.dsp /MAKE "PWLib - Win32 Debug" /USEENV
	msdev "GUI Components.dsp" /MAKE "GUI Components - Win32 Debug" /USEENV
	cd ..\..\..\tools\pwtest
	msdev PWTest.dsp /MAKE "pwtest - Win32 Debug" /USEENV
