This file contains instruction for building installer with NSIS.
NSIS is Nullsoft Scriptable Install System:
http://nsis.sourceforge.net/
To build HONEYNET-MCU installer was used NSIS 2.46.



Preparing files
--------------------------------
Create working folder - for example "C:\BuildSetup\".
Create subfolder "C:\BuildSetup\HONEYNET-MCU\".
Put to "C:\BuildSetup\HONEYNET-MCU\" all compiled HONEYNET-MCU binary files.
All files from "C:\BuildSetup\HONEYNET-MCU\" will be installed
to "$PROGRAMFILES\HONEYNET-MCU" by installer.

Folder "C:\BuildSetup\HONEYNET-MCU\" must have file "COPYING", same as here:
https://github.com/muggot/openmcu/blob/master/COPYING
It will be used for License page in installer.

Icon for the installer is searched here:
C:\BuildSetup\HONEYNET-MCU\share\openmcu.ico

Files needed for building installer must be placed in "C:\BuildSetup\".

As result, target folder must looks like:
C:\BuildSetup\HONEYNET-MCU\etc\*
C:\BuildSetup\HONEYNET-MCU\font\*
C:\BuildSetup\HONEYNET-MCU\log\*
C:\BuildSetup\HONEYNET-MCU\share\*
C:\BuildSetup\HONEYNET-MCU\openmcu.exe (and other *.exe and *.dll must be here)
C:\BuildSetup\HONEYNET-MCU\COPYING
C:\BuildSetup\HONEYNET-MCU\share\openmcu.ico
C:\BuildSetup\HONEYNET-MCU.nsi
C:\BuildSetup\HONEYNET-MCU_en.nsh
C:\BuildSetup\HONEYNET-MCU_ru.nsh
C:\BuildSetup\vcredist_2005_x86.exe
C:\BuildSetup\vcredist_2010_x86.exe



Build installer on Windows
--------------------------------
Install NSIS for Windows (http://nsis.sourceforge.net/Download).
Right click on "HONEYNET-MCU.nsi" and choose menu item "Compile NSIS Script".



Build installer on Linux
--------------------------------
Install NSIS for Linux (apt-get install nsis).
Run in working folder:
makensis HONEYNET-MCU.nsi
