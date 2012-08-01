//
// (c) Yuri Kiryanov, openh323@kiryanov.com
// and Yuriy Gorvitovskiy
//
// Portions (c) 1997 Tim Kientzle
// from ``The Programmer's Guide to Sound.''
//
// Windows CE port of OpenH323 Open Source Project, www.openh323.org
// Extra Multimedia functionality 
// 

#include <ptlib.h>
#include <stdlibx.h>
#include <mmsystemx.h>


////////////////////////////////////////////////////////////////////
// Chunks
class ChunkFinder 
{
	struct Chunk {
	  unsigned long type; // Type of chunk
	  unsigned long size; // Size of chunk
	  unsigned long remaining; // Bytes left to read
	  bool isContainer;   // true if this is a container
	  unsigned long containerType; // type of container
	} m_chunkStack[5];

public:
	ChunkFinder(HANDLE hFile) : m_hFile(hFile), 
		m_fGoodFile(false), m_currentChunk(-1), m_fFormatErrorsFound(false),
		m_type(0L), m_size(0L)
	{
		m_fGoodFile = !(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == -1L);
		memset(m_chunkStack, 0, sizeof(m_chunkStack));
	};

	HANDLE m_hFile;
	bool m_fGoodFile;
	bool m_fFormatErrorsFound;

	int m_currentChunk; // top of stack
	DWORD m_type;
	DWORD m_size;

	bool FindNext(void) 
	{
		if(!m_fGoodFile)
			return false;
		
		if ((m_currentChunk >= 0) && (!m_chunkStack[m_currentChunk].isContainer)) {
		   unsigned long lastChunkSize = m_chunkStack[m_currentChunk].size;
		   if (lastChunkSize & 1) {  // Is there padding?
			  m_chunkStack[m_currentChunk].remaining++;
			  lastChunkSize++; // Account for padding in the container update
		   }
		   SetFilePointer(m_hFile, m_chunkStack[m_currentChunk].remaining, NULL, FILE_CURRENT); // Flush the chunk
		   m_currentChunk--;  // Drop chunk from the stack
		   
		   // Sanity check: containing chunk must be container
		   if ((m_currentChunk < 0) || (!m_chunkStack[m_currentChunk].isContainer)) {
			  // Chunk contained in non-Container
			  m_fFormatErrorsFound = true;
			  return false;
		   }
		   // Reduce size of container
		   if (m_currentChunk >= 0) {
			  // Sanity check: make sure container is big enough.
			  // Also, avoid a really nasty underflow situation.
			  if ((lastChunkSize+8) > m_chunkStack[m_currentChunk].remaining) {
				 m_fFormatErrorsFound = true; // Chunk is too large to fit in container
				 m_chunkStack[m_currentChunk].remaining = 0; // container is empty
			  } else
				 m_chunkStack[m_currentChunk].remaining -= lastChunkSize + 8;
		   }
		}
   
		// There may be forms that are finished, drop them too
		while (  (m_currentChunk >= 0)  // there is a chunk
			  &&  (m_chunkStack[m_currentChunk].remaining < 8)
			  )
		{
		   SetFilePointer(m_hFile, m_chunkStack[m_currentChunk].remaining, NULL, FILE_CURRENT); // Flush the chunk
		   unsigned long lastChunkSize = m_chunkStack[m_currentChunk].size;
		   m_currentChunk--;  // Drop container chunk

		   // Sanity check, containing chunk must be container
		   if (!m_chunkStack[m_currentChunk].isContainer) {
			  // Chunk contained in non-container
			  return false;
		   }
		   // Reduce size of container
		   if (m_currentChunk >= 0) {
			  if ((lastChunkSize+8) > m_chunkStack[m_currentChunk].remaining) {
				 // Error in WAVE file: Chunk is too large to fit
				 lastChunkSize = m_chunkStack[m_currentChunk].remaining;
			  }
			  m_chunkStack[m_currentChunk].remaining -= lastChunkSize + 8;
		   }
		}

	   // Read the next chunk
	   DWORD dwRead = 0L;
	   DWORD dwResult = ReadFile(m_hFile, &m_type, sizeof(long), &dwRead, NULL);
	   if ((dwResult != 0) && (dwRead == 0L)) 
	   {
		  m_currentChunk = -1; // empty the stack
		  return false;
	   }

	   dwResult = ReadFile(m_hFile, &m_size, sizeof(long), &dwRead, NULL);

	   if ((dwResult != 0) && (dwRead == 0L)) 
	   {
		  m_currentChunk = -1; // empty the stack
		  return false;
	   }

	   // Put this chunk on the stack
	   m_currentChunk++;
	   m_chunkStack[m_currentChunk].type = m_type;
	   m_chunkStack[m_currentChunk].size = m_size;
	   m_chunkStack[m_currentChunk].remaining = m_size;
	   m_chunkStack[m_currentChunk].isContainer = false;
	   m_chunkStack[m_currentChunk].containerType = 0;

 		if ((m_currentChunk >= 0) && 
			(m_chunkStack[0].type != MAKEFOURCC('R','I','F','F')))
		{
		   // Outermost chunk is not RIFF 
		   m_currentChunk = -1;
		   return false;
		}
		
		if (m_type == MAKEFOURCC('R','I','F','F')) 
		{
		   m_chunkStack[m_currentChunk].isContainer = true;
		
		   // Need to check size of container first.
		   dwResult = ReadFile(m_hFile, &m_chunkStack[m_currentChunk].containerType, 
				sizeof(long), &dwRead, NULL);
		   
		   if ((dwResult != 0) && (dwRead == 0L)) 
		   {
			  m_currentChunk = -1; // empty the stack
			  return false;
		   }

		   m_chunkStack[m_currentChunk].remaining -= 4;
		   if (m_currentChunk > 0) 
			  m_fFormatErrorsFound = true; // RIFF chunk seen at inner level

		   return true;
		}
		
		if (m_type == MAKEFOURCC('f','m','t',' ')) 
		{
		   if (m_currentChunk != 1) 
			  m_fFormatErrorsFound = true; // FMT chunk seen at wrong level?!?!\n";

		   m_chunkStack[m_currentChunk].remaining = 0;
		   return true;
		}
		
		if (m_type == MAKEFOURCC('d','a','t','a')) 
		{
		   return true;
		}

		// Some unknown chunk found
		return true;
	}

	bool FindRiffHeader() 
	{
		if ( !FindNext() || (m_currentChunk != 0)
		  || (m_chunkStack[0].type != MAKEFOURCC('R','I','F','F'))
		  || (m_chunkStack[0].isContainer != true)
		  || (m_chunkStack[0].containerType != MAKEFOURCC('W','A','V','E'))
		  )
		  return false;
		return true;
	}

	bool FindFmtChunk() 
	{
		bool found = false;
		if( m_currentChunk < 0 )
			found = FindRiffHeader();

		if ( !found || !FindNext() || (m_currentChunk != 1)
		  || (m_chunkStack[1].type != MAKEFOURCC('f','m','t',' '))
		  ) return false;

		return true;
	}
	
	bool FindDataChunk() 
	{
		bool found = false;
		if( m_currentChunk < 1 )
			found = FindFmtChunk();

		if (!found) // Skip format 
			return false;
		else
		{
			SetFilePointer(m_hFile, m_size, NULL, FILE_CURRENT);

			while ( (found = FindNext())  &&
			  (m_chunkStack[m_currentChunk].type != MAKEFOURCC('d','a','t','a'))
			  ) ;
		
			return found && 
				(m_chunkStack[m_currentChunk].type == MAKEFOURCC('d','a','t','a'));
		}
	}
};

HMMIO WINAPI mmioOpen(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen)
{
  USES_CONVERSION;
 
  DWORD dwAccess = fdwOpen & MMIO_READ ? GENERIC_READ : 0;
  dwAccess |= (fdwOpen & MMIO_WRITE) ? GENERIC_WRITE : 0xFFFF; 

  DWORD dwFlags = fdwOpen & MMIO_CREATE ? \
			CREATE_ALWAYS : OPEN_EXISTING;

  HANDLE hFile = CreateFile(A2T(pszFileName), 
	  dwAccess, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, 
	  dwFlags, FILE_ATTRIBUTE_NORMAL, NULL);
	
  if (pmmioinfo != NULL) 
  {
    memset(pmmioinfo, 0, sizeof(MMIOINFO));
	pmmioinfo->wErrorRet = GetLastError();

	if( pmmioinfo->wErrorRet == ERROR_ALREADY_EXISTS )
		pmmioinfo->wErrorRet = 0L;
  }
	
  return (HMMIO) hFile;
}

MMRESULT WINAPI mmioClose(HMMIO hmmio, UINT fuClose)
{
	return CloseHandle(hmmio) ? MMSYSERR_NOERROR : MMSYSERR_INVALHANDLE;
}

LONG WINAPI mmioRead(HMMIO hmmio, HPSTR pch, LONG cch)
{
	DWORD dwRead = 0L;
	ReadFile(hmmio, pch, cch, &dwRead, NULL);
	return dwRead;
}

LONG WINAPI mmioWrite(HMMIO hmmio, const char * pch, LONG cch)
{
	DWORD dwWritten = 0L;
	WriteFile(hmmio, pch, cch, &dwWritten, NULL);

	return dwWritten;
}

MMRESULT WINAPI mmioDescend(HMMIO hmmio, LPMMCKINFO pmmcki,
    const MMCKINFO FAR* pmmckiParent, UINT fuDescend)
{
  if( fuDescend & MMIO_FINDRIFF )
  {	
	// Locate a 'RIFF' chunk with a 'WAVE' form type
	ChunkFinder cf(hmmio);
	if( !cf.FindRiffHeader() )
		return MMSYSERR_ERROR;
  }

  if( fuDescend & MMIO_FINDCHUNK && pmmcki )
  {	
	 if(pmmcki->ckid == mmioFOURCC('f', 'm', 't', ' '))
	 {
		// Find the format chunk
		ChunkFinder cf(hmmio);
		if( !cf.FindFmtChunk() )
			return MMSYSERR_ERROR;

		pmmcki->cksize = cf.m_size + 2; 
	 }

	 if(pmmcki->ckid == mmioFOURCC('d', 'a', 't', 'a'))
	 {
		// Find the data chunk
		ChunkFinder cf(hmmio);
		if( !cf.FindDataChunk() )
			return MMSYSERR_ERROR;

		pmmcki->cksize = cf.m_size; 
	 }
  }
  return MMSYSERR_NOERROR;
}

MMRESULT WINAPI mmioAscend(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend)
{
	// Do nothing - mmioDescend traverses from beginning, no need to move up
	return MMSYSERR_NOERROR;
}

MMRESULT WINAPI mmioCreateChunk(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate)
{
	return MMSYSERR_NOERROR;
}

BOOL WINAPI PlaySound( LPCSTR pszSound, HMODULE hmod, DWORD fdwSound)
{
	// [YG] I have changed all A2W to A2T for normal
	USES_CONVERSION;
	return ::PlaySound( A2T(pszSound), hmod, fdwSound);
}

MMRESULT WINAPI waveInGetErrorText(MMRESULT mmrError, char* pszText, UINT cchText)
{
	TCHAR tch[1024];
	MMRESULT mmResult = waveInGetErrorText(mmrError, tch, 1024);
	wcstombs(pszText, tch, cchText);

	return mmResult;
}

MMRESULT WINAPI waveOutGetErrorText(MMRESULT mmrError, char* pszText, UINT cchText)
{
	TCHAR tch[1024];
	MMRESULT mmResult = waveOutGetErrorText(mmrError, tch, 1024);
	wcstombs(pszText, tch, cchText);

	return mmResult;
}

// Some missing string functions used by multimedia stuff
int __cdecl stricmp(const unsigned short* s1, const char* s2)
{
	USES_CONVERSION;
	return wcsicmp(s1, A2T(s2));
}

