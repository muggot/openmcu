
!verbose 3

!ifdef CURLANG
    !undef CURLANG
!endif
!define CURLANG ${LANG_ENGLISH}

LangString RemovePrevious ${CURLANG} "Please uninstall the previous HONEYNET-MCU installation first"

LangString LicenseText ${CURLANG} "HONEYNET-MCU is released under the Mozilla Public License. Please carefully read the license. Click Accept to continue."
LangString LicenseNext ${CURLANG} "&Accept"
LangString SecMainName ${CURLANG} "HONEYNET-MCU"
LangString SecMainDesc ${CURLANG} "Install HONEYNET-MCU and other required files"
LangString SecVCName ${CURLANG} "Visual C++ redistributable"
LangString SecVCDesc ${CURLANG} "Microsoft Visual C++ redistributable 2005 and 2010"
LangString SecServiceName ${CURLANG} "Install service"
LangString SecServiceDesc ${CURLANG} "Install HONEYNET-MCU as service"

LangString NotAdmin ${CURLANG} "No write permission. $\nMaybe you are not an administrator of this computer. $\nThe installation can be done only by administrator."
LangString ErrorOn ${CURLANG} "Error on:"
LangString DisplayName ${CURLANG} "Videoconference server HONEYNET-MCU"

LangString MenuService ${CURLANG} "Service"
LangString MenuServiceInstall ${CURLANG} "Install HONEYNET-MCU service"
LangString MenuServiceUninstall ${CURLANG} "Uninstall HONEYNET-MCU service"
LangString MenuServiceStart ${CURLANG} "Start HONEYNET-MCU service"
LangString MenuServiceStop ${CURLANG} "Stop HONEYNET-MCU service"
LangString MenuUninstall ${CURLANG} "Uninstall"
LangString MenuUninstallLink ${CURLANG} "Uninstall HONEYNET-MCU"
LangString MenuStartDebug ${CURLANG} "Start HONEYNET-MCU in debug mode"
LangString MenuStartTray ${CURLANG} "HONEYNET-MCU tray icon"
LangString MenuWebinterface ${CURLANG} "HONEYNET-MCU webinterface"

LangString FinishTitle ${CURLANG} "Installation complete"
LangString FinishText ${CURLANG} "Videoconference server HONEYNET-MCU successfully installed."
LangString FinishWebInt ${CURLANG} "Open HONEYNET-MCU webinterface"
LangString FinishLink ${CURLANG} "Project website:"
