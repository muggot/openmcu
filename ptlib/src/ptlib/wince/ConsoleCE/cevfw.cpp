//
// (c) 2002 Yuri Kiryanov, openh323@kiryanov.com
// 
// Windows CE port of OpenH323 Open Source Project, www.openh323.org
// Video For Windows Definitions

#include "ptlib.h"

#include <mmsystemx.h>
#include <cevfw.h>

BOOL VFWAPI capDefGetDriverDescription (UINT, LPSTR, int, LPSTR, int ) 
{ 
	PTRACE(0, ">>> capDefGetDriverDescription called. Should it?" << endl );
	return FALSE; 
}

HWND VFWAPI capDefCreateCaptureWindow(LPCSTR, DWORD, int, int, int, int, HWND, int) 
{ 
	PTRACE(0, ">>> capDefCreateCaptureWindow called. Should it?" << endl );
	return NULL; 
}

// Function declarations. 
// You should reload them with your function pointers in order to 
// implement your functionality
//
CAPGETDRIVERDESCRIPTIONPROC capDriverGetDriverDescription = 
	(CAPGETDRIVERDESCRIPTIONPROC) &capDefGetDriverDescription;
CAPCREATECAPTUREWINDOWPROC capDriverCreateCaptureWindow = 
	(CAPCREATECAPTUREWINDOWPROC) &capDefCreateCaptureWindow;

BOOL VFWAPI capGetDriverDescription(UINT wDriverIndex,
	LPSTR lpszName, int cbName, LPSTR lpszVer, int cbVer)
{ 
	return (capDriverGetDriverDescription != NULL) ? \
		(*capDriverGetDriverDescription)(wDriverIndex, lpszName, cbName,lpszVer, cbVer) : \
		capDefGetDriverDescription(wDriverIndex, lpszName, cbName, lpszVer, cbVer );
}

HWND VFWAPI capCreateCaptureWindow(
	LPCSTR lpszWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hwndParent, int nID)
{ 
	return (capDriverCreateCaptureWindow != NULL) ? \
		(*capDriverCreateCaptureWindow)(lpszWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, nID) : \
		capDefCreateCaptureWindow(lpszWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, nID);
}

