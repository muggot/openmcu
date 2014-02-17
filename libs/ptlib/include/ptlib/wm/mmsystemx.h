//////////////////////////////////////////////////////
//
// VisualStudio 2005 PWLib Port, 
// (c) 2007 Dinsk.net
// developer@dinsk.net 
//
//////////////////////////////////////////////////////
//
// (c) 2000 Yuri Kiryanov, yuri@kiryanov.com
// and Yuriy Gorvitovskiy
// 
// Windows CE port of OpenH323 Open Source Project, www.openh323.org
// Missing PWLib extras 

#ifndef __MMSYSTEMX_H__
#define __MMSYSTEMX_H__

#include <ptlib/wm/stdlibx.h>

//
// MMSystem extras
//
#define MMIO_CREATE     0x00001000      /* create new file (or truncate file) */
#define MMIO_WRITE      0x00000001      /* open file for writing only */
#define MMIO_CREATERIFF         0x0020  /* mmioCreateChunk: make a LIST chunk */
#define MMIO_READ       0x00000000      /* open file for reading only */
#define MMIO_ALLOCBUF   0x00010000      /* mmioOpen() should allocate a buffer */
#define MMIO_FINDRIFF           0x0020  /* mmioDescend: find a LIST chunk */
#define MMIO_FINDCHUNK          0x0010  /* mmioDescend: find a chunk by ID */

typedef DWORD FOURCC;
typedef char *    HPSTR;				/* a huge version of LPSTR */
typedef LRESULT (CALLBACK MMIOPROC)(LPSTR lpmmioinfo, UINT uMsg,
	    LPARAM lParam1, LPARAM lParam2);
typedef MMIOPROC FAR *LPMMIOPROC;

HMMIO WINAPI mmioOpen(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);

MMRESULT WINAPI mmioClose(HMMIO hmmio, UINT fuClose);
LONG WINAPI mmioRead(HMMIO hmmio, HPSTR pch, LONG cch);
LONG WINAPI mmioWrite(HMMIO hmmio, const char * pch, LONG cch);
MMRESULT WINAPI mmioDescend(HMMIO hmmio, LPMMCKINFO pmmcki,
    const MMCKINFO FAR* pmmckiParent, UINT fuDescend);
MMRESULT WINAPI mmioAscend(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend);
MMRESULT WINAPI mmioCreateChunk(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate);

MMRESULT WINAPI waveInGetErrorText(MMRESULT mmrError, char* pszText, UINT cchText);
MMRESULT WINAPI waveOutGetErrorText(MMRESULT mmrError, char* pszText, UINT cchText);

#endif // __MMSYSTEMX_H__