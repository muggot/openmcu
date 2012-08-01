//////////////////////////////////////////////////////
//
// VisualStudio 2005 PWLib Port, 
// (c) 2007 Dinsk.net
// developer@dinsk.net 
//
//////////////////////////////////////////////////////
//
// (c) 2002 Yuri Kiryanov, openh323@kiryanov.com
// 
// Windows CE port of OpenH323 Open Source Project, www.openh323.org
// Video For Windows Definitions

#include "ptlib.h"

#ifdef _WIN32_WCE

#include <ptlib/wm/mmsystemx.h>
#include <ptlib/wm/cevfw.h>

static const TCHAR* gszCEVideoCapClassName = _T("CEVideoCaptureWndClass");
static const char* gszCEVideoCapDevName = "Video Input";
static const char* gszCEVideoCapDevVersion = "0.1";
static HWND ghWndCapture = NULL;

BOOL VFWAPI capDefGetDriverDescription (UINT, LPSTR szName, int, LPSTR lpszVer, int ) 
{ 
	PTRACE(2, "capDefGetDriverDescription() called." << endl );
	if( szName )
		strcpy(szName, gszCEVideoCapDevName);
		
	if( lpszVer )
		strcpy(lpszVer, gszCEVideoCapDevVersion);
	 
	return TRUE; 
}

LRESULT WINAPI CapWindowProc(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam)
{
	if((Msg >= WM_CAP_START) && (Msg <= WM_CAP_GET_USER_DATA))
	{
		PTRACE(2, "CapWindowProc() called. Msg: " 
			<< Msg << ". wParam: " << wParam << ". lParam: " << lParam << endl );
	}

	return ::DefWindowProc(hWnd, Msg, wParam, lParam );
}

extern HANDLE hInstance;

HWND VFWAPI capDefCreateCaptureWindow(LPCSTR szTitle, 
	DWORD dwStyle, int x, int y, int width, int height, HWND hwndParent, int) 
{ 
	PTRACE(2, "capDefCreateCaptureWindow() called." << endl );
	
	USES_CONVERSION;
	WNDCLASS wc;
	ATOM atom;

	ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = CapWindowProc;
    wc.hInstance = (HINSTANCE) NULL; // Hope it would find one
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = gszCEVideoCapClassName;

	atom = RegisterClass(&wc);

	if ((ghWndCapture = ::CreateWindowExW( 0L, 
		(LPCTSTR) atom,
		A2W(szTitle && *szTitle? szTitle : gszCEVideoCapDevName),
           dwStyle,
           x, y,
           width + GetSystemMetrics(SM_CXFIXEDFRAME),
           height + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFIXEDFRAME),
           (HWND) hwndParent,
		   NULL, // hMenu
		   NULL, // hInstance
		   NULL // lParam
		   )) == NULL) 
	{ 
		PTRACE(2, "capDefCreateCaptureWindow() failed. Last error: " << GetLastError() << endl );
		return NULL; 
	}

	PTRACE(2, "capDefCreateCaptureWindow() created window. hWnd: " << ghWndCapture << endl );
	return ghWndCapture; 
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

#endif // _WIN32_WCE
