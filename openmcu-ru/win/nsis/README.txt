This file contains instruction for building installer with NSIS.
NSIS is Nullsoft Scriptable Install System:
http://nsis.sourceforge.net/
To build OpenMCU-ru installer was used NSIS 2.46.



Preparing files
--------------------------------
Create working folder - for example "C:\BuildSetup\".
Create subfolder "C:\BuildSetup\openmcu-ru\".
Put to "C:\BuildSetup\openmcu-ru\" all compiled OpenMCU-ru binary files.
All files from "C:\BuildSetup\openmcu-ru\" will be installed
to "$PROGRAMFILES\OpenMCU-ru" by installer.

Folder "C:\BuildSetup\openmcu-ru\" must have file "COPYING", same as here:
https://github.com/muggot/openmcu/blob/master/COPYING
It will be used for License page in installer.

Icon for the installer is searched here:
C:\BuildSetup\openmcu-ru\share\mcu.ico

Files needed for building installer must be placed in "C:\BuildSetup\".

As result, target folder must looks like:
C:\BuildSetup\openmcu-ru\etc\*
C:\BuildSetup\openmcu-ru\font\*
C:\BuildSetup\openmcu-ru\log\*
C:\BuildSetup\openmcu-ru\share\*
C:\BuildSetup\openmcu-ru\openmcu.exe (and other *.exe and *.dll must be here)
C:\BuildSetup\openmcu-ru\COPYING
C:\BuildSetup\openmcu-ru\share\openmcu.ico
C:\BuildSetup\OpenMCU-ru.nsi
C:\BuildSetup\OpenMCU-ru_en.nsh
C:\BuildSetup\OpenMCU-ru_ru.nsh
C:\BuildSetup\vcredist_2005_x86.exe
C:\BuildSetup\vcredist_2010_x86.exe



Build installer on Windows
--------------------------------
Install NSIS for Windows (http://nsis.sourceforge.net/Download).
Right click on "OpenMCU-ru.nsi" and choose menu item "Compile NSIS Script".



Build installer on Linux
--------------------------------
Install NSIS for Linux (apt-get install nsis).
Run in working folder:
makensis OpenMCU-ru.nsi
