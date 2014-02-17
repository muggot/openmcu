//
// (c) 2000 Yuri Kiryanov, openh323@kiryanov.com
// and Yuriy Gorvitovskiy
// 
// Windows CE port of OpenH323 Open Source Project, www.openh323.org
// Missing PWLib extras 

#ifndef __MMSYSTEMX_H__
#define __MMSYSTEMX_H__

#include <stdlibx.h>

#if _WIN32_WCE < 400

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

/* general MMIO information data structure */
typedef struct _MMIOINFO
{
	/* general fields */
	DWORD           dwFlags;        /* general status flags */
	FOURCC          fccIOProc;      /* pointer to I/O procedure */
	LPMMIOPROC      pIOProc;        /* pointer to I/O procedure */
	UINT            wErrorRet;      /* place for error to be returned */
	HTASK           htask;          /* alternate local task */

	/* fields maintained by MMIO functions during buffered I/O */
	LONG            cchBuffer;      /* size of I/O buffer (or 0L) */
	HPSTR           pchBuffer;      /* start of I/O buffer (or NULL) */
	HPSTR           pchNext;        /* pointer to next byte to read/write */
	HPSTR           pchEndRead;     /* pointer to last valid byte to read */
	HPSTR           pchEndWrite;    /* pointer to last byte to write */
	LONG            lBufOffset;     /* disk offset of start of buffer */

	/* fields maintained by I/O procedure */
	LONG            lDiskOffset;    /* disk offset of next read or write */
	DWORD           adwInfo[3];     /* data specific to type of MMIOPROC */

	/* other fields maintained by MMIO */
	DWORD           dwReserved1;    /* reserved for MMIO use */
	DWORD           dwReserved2;    /* reserved for MMIO use */
	HMMIO           hmmio;          /* handle to open file */
} MMIOINFO, *PMMIOINFO, NEAR *NPMMIOINFO, FAR *LPMMIOINFO;
typedef const MMIOINFO FAR *LPCMMIOINFO;

typedef struct _MMCKINFO
{
	FOURCC          ckid;
	DWORD           cksize;
	FOURCC          fccType;
	DWORD           dwDataOffset;
	DWORD           dwFlags;
} MMCKINFO, *PMMCKINFO, NEAR *NPMMCKINFO, FAR *LPMMCKINFO;
typedef const MMCKINFO *LPCMMCKINFO;

#ifndef HMMIO
typedef HANDLE          HMMIO;          // a handle to an open file
#endif

#endif // _INC_MMSYSTEM

HMMIO WINAPI mmioOpen(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);

MMRESULT WINAPI mmioClose(HMMIO hmmio, UINT fuClose);
LONG WINAPI mmioRead(HMMIO hmmio, HPSTR pch, LONG cch);
LONG WINAPI mmioWrite(HMMIO hmmio, const char * pch, LONG cch);
MMRESULT WINAPI mmioDescend(HMMIO hmmio, LPMMCKINFO pmmcki,
    const MMCKINFO FAR* pmmckiParent, UINT fuDescend);
MMRESULT WINAPI mmioAscend(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend);
MMRESULT WINAPI mmioCreateChunk(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate);

BOOL WINAPI PlaySound( LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);

MMRESULT WINAPI waveInGetErrorText(MMRESULT mmrError, char* pszText, UINT cchText);
MMRESULT WINAPI waveOutGetErrorText(MMRESULT mmrError, char* pszText, UINT cchText);

#endif // __MMSYSTEMX_H__