//
// (c) Yuri Kiryanov, openh323@kiryanov.com and
//     Yuriy Gorvitovskiy
//
// for Openh323, www.Openh323.org
//
// Windows CE Port
//
// stdlib routines implemented through Windows CE API
//

#include <ptlib.h>
#include <Atlconv.h>
#include <winbase.h>
#include <winnt.h>
#include <snmp.h>

#include <ptlib/sockets.h>

#define DELETE (0x00010000L) // defined in <winnt.h> and undef "msos/ptlib/contain.h"

#ifndef __cplusplus
#include <tchar.h>
int isprint(int c) { return _istprint(c);}
int isxdigit(int c) { return _istxdigit(c); }
int isspace( int c ) { return _istspace(c); }
int isupper( int c ) { return _istupper(c); }
int islower( int c ) { return _istlower(c); }
int isalnum( int c ) { return _istalnum(c); }
int isalpha( int c ) { return _istalpha(c); }
int iscntrl( int c ) { return _istcntrl(c); }
int isdigit( int c ) { return _istdigit(c); }
int ispunct( int c ) { return _istpunct(c); }
#endif

void __cdecl abort(void) { 
	if(IDYES == MessageBox(NULL, _T("Abort?"), _T("stdlibx"), MB_YESNO))
		exit(3);
}

void __cdecl perror(const char * s) {
	USES_CONVERSION;
	MessageBox(NULL, s && *s ? A2T(s) : _T("null"), _T("stdlibx"), MB_OK); 
}

long _lseek(int nHandle, long off, int orig)
{
	DWORD dwMoveMethod=FILE_BEGIN;
	switch(orig)
	{
		case SEEK_SET: dwMoveMethod=FILE_BEGIN; break;
		case SEEK_CUR: dwMoveMethod=FILE_CURRENT; break;
		case SEEK_END: dwMoveMethod=FILE_END; break;
	}
	return SetFilePointer((HANDLE)nHandle,off,NULL,dwMoveMethod);
}

int  _close(int nHandle)
{
        FlushFileBuffers((HANDLE)nHandle);
	return (CloseHandle((HANDLE)nHandle)) ? 0 : -1;
}
int  _read(int nHandle, void *p, unsigned int s)
{
	DWORD size=0;
	ReadFile((HANDLE)nHandle,p,s,&size,NULL);
	return size;
}

int  _write(int nHandle, const void *p, unsigned int s)
{
	DWORD size=0;
	WriteFile((HANDLE)nHandle,p,s,&size,NULL);
	//[YG]???? FlushFileBuffers((HANDLE)nHandle);
	return size;
}
int	_open( const char *filename, int oflag , int pmode)
{
	USES_CONVERSION;
    HANDLE	osfh;                    /* OS handle of opened file */
    DWORD	fileaccess;               /* OS file access (requested) */
    DWORD	fileshare;                /* OS file sharing mode */
    DWORD	filecreate;               /* OS method of opening/creating */
    DWORD	fileattrib;               /* OS file attribute flags */
    
    /*
     * decode the access flags
     */
    switch( oflag & (_O_RDONLY | _O_WRONLY | _O_RDWR) ) 
	{
        case _O_RDONLY:         /* read access */
                fileaccess = GENERIC_READ;
                break;
        case _O_WRONLY:         /* write access */
                fileaccess = GENERIC_WRITE;
                break;
        case _O_RDWR:           /* read and write access */
                fileaccess = GENERIC_READ | GENERIC_WRITE;
                break;
        default:                /* error, bad oflag */
                set_errno(EINVAL);
                return -1;
    }

    /*
     * decode sharing flags
     */
	fileshare = FILE_SHARE_READ | FILE_SHARE_WRITE;

    /*
     * decode open/create method flags
     */
    switch ( oflag & (_O_CREAT | _O_EXCL | _O_TRUNC) ) 
	{
            case 0:
            case _O_EXCL:                   // ignore EXCL w/o CREAT
                filecreate = OPEN_EXISTING;
                break;

            case _O_CREAT:
                filecreate = OPEN_ALWAYS;
                break;

            case _O_CREAT | _O_EXCL:
            case _O_CREAT | _O_TRUNC | _O_EXCL:
                filecreate = CREATE_NEW;
                break;

            case _O_TRUNC:
            case _O_TRUNC | _O_EXCL:        // ignore EXCL w/o CREAT
                filecreate = TRUNCATE_EXISTING;
                break;

            case _O_CREAT | _O_TRUNC:
                filecreate = CREATE_ALWAYS;
                break;

            default:
                // this can't happen ... all cases are covered
                set_errno(EINVAL);
                return -1;
	}

    /*
     * decode file attribute flags if _O_CREAT was specified
     */
    fileattrib = FILE_ATTRIBUTE_NORMAL;     /* default */

    if ( oflag & _O_CREAT ) 
	{
        /*
         * set up variable argument list stuff
         */
        if ( oflag & _O_RDONLY )
            fileattrib = FILE_ATTRIBUTE_READONLY;
    }

    /*
     * Set temporary file (delete-on-close) attribute if requested.
     */
    if ( oflag & _O_TEMPORARY ) 
	{
        fileattrib |= FILE_FLAG_DELETE_ON_CLOSE;
        fileaccess |= DELETE;
    }

    /*
     * Set temporary file (delay-flush-to-disk) attribute if requested.
     */
    if ( oflag & _O_SHORT_LIVED )
        fileattrib |= FILE_ATTRIBUTE_TEMPORARY;

    /*
     * Set sequential or random access attribute if requested.
     */
    if ( oflag & _O_SEQUENTIAL )
        fileattrib |= FILE_FLAG_SEQUENTIAL_SCAN;
    else if ( oflag & _O_RANDOM )
        fileattrib |= FILE_FLAG_RANDOM_ACCESS;

    /*
     * get an available handle.
     *
     * multi-thread note: the returned handle is locked!
     */
    /*
     * try to open/create the file
     */

    if ( (osfh = CreateFile( A2T(filename),
                             fileaccess,
                             fileshare,
                             NULL,
                             filecreate,
                             fileattrib,
                             NULL ))
         == INVALID_HANDLE_VALUE )
    {
        return -1;                      /* return error to caller */
    }
	return (int)osfh;
}

int _sopen(const char *filename, int oflag , int pmode, ...)
{ 
	return _open(filename, oflag , pmode); 
}

int	_chsize( int nHandle, long size )
{
	if ((DWORD)size!=SetFilePointer((HANDLE)nHandle,size,NULL,FILE_BEGIN)) return -1;
	return (SetEndOfFile((HANDLE)nHandle)) ? 0 : -1;
}


int _mkdir(const char *sDir)
{	
	USES_CONVERSION;
        PString folderName = sDir;

        if (folderName[folderName.GetLength() - 1] == PDIR_SEPARATOR) {
           folderName.Delete(folderName.GetLength() - 1, 1);
        }

        return (CreateDirectory(A2T(folderName),NULL) ? 0 : -1);
}

int  _rmdir(const char *sDir)
{	
	USES_CONVERSION;
        PString folderName = sDir;

        if (folderName[folderName.GetLength() - 1] == PDIR_SEPARATOR) {
            folderName.Delete(folderName.GetLength() - 1, 1);
        }

        return (RemoveDirectory(A2T(folderName)) ? 0 : -1);
}

int  _access(const char *sName, int mode)
{
	USES_CONVERSION;
	WIN32_FIND_DATA FindFileData;

        PString test(sName);
        if (test[test.GetLength() - 1] == '.' && test[test.GetLength() - 2] == PDIR_SEPARATOR)
            test.Delete(test.GetLength() - 2, 2);

        HANDLE file = FindFirstFile(A2T((const char*) test), &FindFileData);
	if (file == INVALID_HANDLE_VALUE ) return -1;
	FindClose(file);
	switch(mode)
	{
		//checking for the existance
		case 0: return 0;
		//checking for read permission
		case 4: return 0;
		//checking for write permission
		case 2: return (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_READONLY) ? -1 : 0;
		//checking for read and write permission
		case 6: return (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_READONLY) ? -1 : 0;
	}
	return -1;
}
int	remove(const char *name)
{
	USES_CONVERSION;
	return (DeleteFile(A2T(name)) ? 0 : -1);
}

int	_chmod( const char *filename, int pmode )
{
	USES_CONVERSION;
	TCHAR* pFileName=A2T(filename);
	DWORD attr = GetFileAttributes(pFileName);
	if (pmode&_S_IWRITE)
		attr|=FILE_ATTRIBUTE_READONLY;
	else
		attr&=~FILE_ATTRIBUTE_READONLY;
	
	return (SetFileAttributes(pFileName,attr) ? 0: -1);	
}

int	rename( const char *oldname, const char *newname )
{
	USES_CONVERSION;
	return (DeleteAndRenameFile( A2T(newname), A2T(oldname)) ? 0 : -1);
}

//used by regex.cxx
void printchar (char n)
{
	printf(" %d ",n);	
}

long strtol (const char *nptr,char **endptr,int ibase)
{
	USES_CONVERSION;
	TCHAR* tnptr = A2T(nptr);
	TCHAR* tendptr = NULL;

	long res= _tcstoul(tnptr,&tendptr,ibase);
	if (endptr)
	{
		if (tendptr) *endptr=const_cast<char*>(nptr+(tendptr-tnptr));
		else		 *endptr=NULL;
	}
	return res;
}

unsigned long strtoul (const char *nptr,char **endptr,int ibase)
{
	USES_CONVERSION;
	TCHAR* tnptr = A2T(nptr);
	TCHAR* tendptr = NULL;

	unsigned long res= _tcstoul(tnptr,&tendptr,ibase);
	if (endptr)
	{
		if (tendptr) *endptr=const_cast<char*>(nptr+(tendptr-tnptr));
		else		 *endptr=NULL;
	}
	return res;
}

double strtod( const char *nptr, char **endptr )
{
	USES_CONVERSION;
	TCHAR* tnptr = A2T(nptr);
	TCHAR* tendptr = NULL;

	double res= _tcstod(tnptr,&tendptr);
	if (endptr)
	{
		if (tendptr) *endptr=const_cast<char*>(nptr+(tendptr-tnptr));
		else		 *endptr=NULL;
	}
	return res;
}


#if _WIN32_WCE < 300
const char * __cdecl strrchr (const char * string,int ch )
{
        char *start = (char *)string;

        while (*string++)                       /* find end of string */
                ;
                                                /* search towards front */
        while (--string != start && *string != (char)ch)
                ;

        if (*string == (char)ch)                /* char found ? */
                return( (char *)string );

        return(NULL);
}
#endif

size_t strspn( const char *string, const char *strCharSet )
{
    const unsigned char *str = (const unsigned char*)string;
    const unsigned char *ctrl = (const unsigned char*)strCharSet;

    unsigned char map[32];
    int count;

    /* Clear out bit map */
    for (count=0; count<32; count++)
        map[count] = 0;

    /* Set bits in control map */
    while (*ctrl)
    {  
		map[*ctrl >> 3] |= (1 << (*ctrl & 7));
		ctrl++;
    }

    /* 1st char NOT in control map stops search */
    if (*str)
    {
        count=0;
        while (map[*str >> 3] & (1 << (*str & 7)))
        {
                count++;
                str++;
        }
        return(count);
	}
    return(0);
}

int stricmp( const char*string1, const char* string2 )
{
	USES_CONVERSION;
	return stricmp( (const unsigned short*) A2T(string1), (const char*) string2);
}

static void x64toa (unsigned __int64 val,char *buf,unsigned radix,int is_neg)
{
        char *p;                /* pointer to traverse string */
        char *firstdig;         /* pointer to first digit */
        char temp;              /* temp char */
        unsigned digval;        /* value of digit */

        p = buf;

        if ( is_neg )
        {
            *p++ = '-';         /* negative, so output '-' and negate */
            val = (unsigned __int64)(-(__int64)val);
        }

        firstdig = p;           /* save pointer to first digit */

        do {
            digval = (unsigned) (val % radix);
            val /= radix;       /* get next digit */

            /* convert to ascii and store */
            if (digval > 9)
                *p++ = (char) (digval - 10 + 'a');  /* a letter */
            else
                *p++ = (char) (digval + '0');       /* a digit */
        } while (val > 0);

        /* We now have the digit of the number in the buffer, but in reverse
           order.  Thus we reverse them now. */

        *p-- = '\0';            /* terminate string; p points to last digit */

        do {
            temp = *p;
            *p = *firstdig;
            *firstdig = temp;   /* swap *p and *firstdig */
            --p;
            ++firstdig;         /* advance to next two digits */
        } while (firstdig < p); /* repeat until halfway */
}

/* Actual functions just call conversion helper with neg flag set correctly,
   and return pointer to buffer. */

char * _i64toa (__int64 val,char *buf,int radix)
{
        x64toa((unsigned __int64)val, buf, radix, (radix == 10 && val < 0));
        return buf;
}

char * _ui64toa (unsigned __int64 val,char *buf,int radix)
{
        x64toa(val, buf, radix, 0);
        return buf;
}

//#if _WIN32_WCE < 300
__int64 _atoi64(const char *nptr)
{
        int c;              /* current char */
        __int64 total;      /* current total */
        int sign;           /* if '-', then negative, otherwise positive */

        /* skip whitespace */
        while ( isspace((int)(unsigned char)*nptr) )
            ++nptr;

        c = (int)(unsigned char)*nptr++;
        sign = c;           /* save sign indication */
        if (c == '-' || c == '+')
            c = (int)(unsigned char)*nptr++;    /* skip sign */

        total = 0;

        while (isdigit(c)) {
            total = 10 * total + (c - '0');     /* accumulate digit */
            c = (int)(unsigned char)*nptr++;    /* get next char */
        }

        if (sign == '-')
            return -total;
        else
            return total;   /* return result, negated if necessary */
}
//#endif

char * _mktemp (char *temp)
{
        char *string = temp;
        unsigned number;
        int letter = 'a';
        int xcount = 0;
        int olderrno;

        _ASSERTE(temp != NULL);
        _ASSERTE(*temp != '\0');

  	    number = (unsigned) GetCurrentProcess();
  
        while (*string)
                string++;

#ifndef _MBCS
        while (*--string == 'X')
#else  /* _MBCS */
        while ((string>temp) && (!__isdbcscode(temp,string-1))
                && (*--string == 'X'))
#endif  /* _MBCS */
        {
                xcount++;
                *string = (char)((number % 10) + '0');
                number /= 10;
        }

        if (*++string == '\0' || xcount != 6 )
                return(NULL);

        olderrno = errno;       /* save current errno */
        set_errno(0);              /* make sure errno isn't EACCESS */

        while ((_access(temp,0) == 0) || (errno == EACCES))
        /* while file exists */
        {
                set_errno(0);
                if (letter == 'z'+1) 
				{
                        set_errno(olderrno);
                        return(NULL);
                }

                *string = (char)letter++;
        }

        set_errno(olderrno);
        return(temp);
}

LONG RegOpenKeyEx( HKEY hKey, const char* lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult )
{ USES_CONVERSION; return 
	RegOpenKeyEx( hKey, A2T(lpSubKey), ulOptions, 
	samDesired, phkResult ); 
}

LONG RegCreateKeyEx( HKEY hKey, const char* lpSub, DWORD dwr, LPSTR lpcls, DWORD dwo, 
	REGSAM sam, LPSECURITY_ATTRIBUTES lpsa, PHKEY phk, LPDWORD lpdw )
{ USES_CONVERSION; return 
	RegCreateKeyEx( hKey, A2T(lpSub), 
	dwr, A2T(lpcls), dwo, sam, lpsa, phk, lpdw ); 
}

LONG RegEnumKey(HKEY hKey, DWORD dwIndex, LPTSTR ptcsName, DWORD cbName)
{	DWORD cb = cbName; 
	return RegEnumKeyEx( hKey, dwIndex, ptcsName, &cb, 0L, NULL, NULL, NULL ); 
}

LONG RegDeleteKey( HKEY hKey, const char* lpSubKey )
{ USES_CONVERSION; return RegDeleteKey( hKey, A2T(lpSubKey) ); }

LONG RegEnumValueCe( HKEY hKey, DWORD dwIndex, LPTSTR ptcsValueName, LPDWORD lpcbValueName, 
	LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
{ 
	return RegEnumValue( hKey, dwIndex, ptcsValueName, lpcbValueName, 
	lpReserved, lpType, lpData, lpcbData );
}

LONG RegQueryValueEx( HKEY hKey, char* lpValueName, 
	LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData )
{ USES_CONVERSION; return RegQueryValueEx( hKey, A2T(lpValueName), 
	lpReserved, lpType, lpData, lpcbData );
}

LONG RegSetValueEx( HKEY hKey, const char* lpValueName, DWORD Reserved, DWORD dwType, 
	const BYTE *lpData, DWORD cbData ) 
{ USES_CONVERSION; return RegSetValueEx( hKey, A2T(lpValueName), Reserved, dwType, 
	lpData, cbData ); 
}

LONG RegDeleteValue( HKEY hKey, const char* lpValueName )
{ USES_CONVERSION; return RegDeleteValue( hKey, A2T(lpValueName) ); }

UINT GetWindowsDirectory( char* lpBuffer, UINT uSize )
{ strncpy(lpBuffer, "\\Windows", uSize ); return uSize; }


DWORD GetPrivateProfileString(
  const char* lpAppName,        // points to section name
  const char*  lpKeyName,        // points to key name
  const char* lpDefault,        // points to default string
  char* lpReturnedString,  // points to destination buffer
  DWORD nSize,              // size of destination buffer
  const char*  )        // points to initialization filename
{ 	
	return -1L;
}

BOOL WritePrivateProfileString(
  const char* lpAppName,  // pointer to section name
  const char* lpKeyName,  // pointer to key name
  const char* lpString,   // pointer to string to add
  const char* )  // pointer to initialization filename
{ 
	return FALSE;
}

