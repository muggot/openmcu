//
// (c) Yuriy Gorvitovskiy
// for Openh323, www.Openh323.org
//
// Windows CE Port
//
// streambuf implementation
//

#include <iostream.h>
#include <stdlib.h>

#ifndef BUFSIZ
#define BUFSIZ 512
#endif  /* BUFSIZ */

/*******************************************************************************/

streambuf::streambuf()
{
    _fAlloc = 0;
    _fUnbuf = 0;
    x_lastc = EOF;
    _base = NULL;
    _ebuf = NULL;
    _pbase = NULL;
    _pptr = NULL;
    _epptr = NULL;
    _eback = NULL;
    _gptr = NULL;
    _egptr = NULL;
}

/*******************************************************************************/
streambuf::streambuf( char* pBuf, int cbBuf )
{
    _fAlloc = 0;
    _fUnbuf = 0;
    x_lastc = EOF;
    _base = pBuf;
    _ebuf = pBuf + (unsigned)cbBuf;
    _pbase = NULL;
    _pptr = NULL;
    _epptr = NULL;
    _eback = NULL;
    _gptr = NULL;
    _egptr = NULL;

    if( pBuf == NULL || cbBuf == 0 )
	{
        _fUnbuf = 1;
        _base = NULL;
        _ebuf = NULL;
    }
}

/*******************************************************************************/
streambuf::~streambuf()
{
    sync();     // make sure buffer empty before possibly destroying it
    if( (_fAlloc) && (_base) )
        delete _base;
}

/*******************************************************************************/
streambuf * streambuf::setbuf(char * p, int len)
{
    if (!_base)
    {
        if ((!p) || (!len))
            _fUnbuf = 1;        // mark as unbuffered
        else
        {
            _base = p;
            _ebuf = p + (unsigned)len;
            _fUnbuf = 0;
        }
        return (this);
    }
    return((streambuf *)NULL);
}


/*******************************************************************************/
int streambuf::xsputn( const char* pBuf, int cbBuf )
{
    int cbOut;

    for (cbOut = 0; cbBuf--; cbOut++)
	{
        if ((_fUnbuf) || (_pptr >= _epptr))
        {
            if (overflow((unsigned char)*pBuf)==EOF)    // 0-extend 0xFF !=EOF
                break;
        }
        else
		{
            *(_pptr++) = *pBuf;
        }
        pBuf++;
	}
    return cbOut;
}

/*******************************************************************************/
int streambuf::xsgetn( char * pBuf, int cbBuf)
{
    int count;
    int cbIn = 0;
    if (_fUnbuf)
    {
		if (x_lastc==EOF)
			x_lastc=underflow();

		while (cbBuf--)
        {
			if (x_lastc==EOF)
				break;

			*(pBuf++) = (char)x_lastc;
			cbIn++;
			x_lastc=underflow();
        }
    }
    else
    {
	    while (cbBuf)
        {
			if (underflow()==EOF)       // make sure something to read
				break;

			count = __min(egptr() - gptr(),cbBuf);
			if (count>0)
            {
				memcpy(pBuf,gptr(),count);
				pBuf  += count;
				_gptr += count;
				cbIn  += count;
				cbBuf -= count;
            }
        }
    }
    return cbIn;
}

/*******************************************************************************/

int streambuf::sync()
{
    if ((gptr() <_egptr) || (_pptr > _pbase))
	    return EOF;

    return 0;
}

/*******************************************************************************/

int streambuf::allocate()
{
    if ((_fUnbuf) || (_base))
        return 0;
    
	if (doallocate()==EOF) 
		return EOF;

    return(1);
}

/*******************************************************************************/

int streambuf::doallocate()
{
    char * tptr;
    if (!( tptr = new char[BUFSIZ]))
        return(EOF);

    setb(tptr, tptr + BUFSIZ, 1);
    return(1);
}

/*******************************************************************************/
void streambuf::setb(char * b, char * eb, int a )
{
    if ((_fAlloc) && (_base))
        delete _base;

    _base = b;
    _fAlloc = a;
    _ebuf = eb;
}

/*******************************************************************************/
streampos streambuf::seekoff(streamoff,ios::seek_dir,int)
{
	return EOF;
}

/*******************************************************************************/
streampos streambuf::seekpos(streampos pos,int mode)
{
	return seekoff(streamoff(pos), ios::beg, mode);
}

/*******************************************************************************/
int streambuf::pbackfail(int c)
{
    if (eback()<gptr()) return sputbackc((char)c);

    if (seekoff( -1, ios::cur, ios::in)==EOF)  // always EOF for streambufs
        return EOF;

    if (!unbuffered() && egptr())
    {
        memmove((gptr()+1),gptr(),(egptr()-(gptr()+1)));
        *gptr()=(char)c;
    }
    return(c);
}

/*******************************************************************************/
int streambuf::sgetc()
{
    if (_fUnbuf)  // no buffer
    {
        if (x_lastc==EOF)
            x_lastc = underflow();
        return x_lastc;
    }
    else
        return underflow();

}
int streambuf::snextc()
{
    if (_fUnbuf)
	{
        if (x_lastc==EOF)
            underflow();                // skip 1st character
        return x_lastc = underflow();   // return next character, or EOF
	}
    else
	{
        if ((!egptr()) || (gptr()>=egptr()))
            underflow();                // make sure buffer

        if ((++_gptr) < egptr())
            return (int)(unsigned char) *gptr();
        return underflow();             // returns next character, or EOF
   }

}
int streambuf::sbumpc()
{
    int c;
    if (_fUnbuf) // no buffer
    {
        if (x_lastc==EOF)
        {
            c = underflow();
        }
        else
        {
            c = x_lastc;
            x_lastc = EOF;
        }
    }
    else
    {
        if( gptr() < egptr() )
		{
            c = (int)(unsigned char)*(gptr());
		}
        else
		{
            c = underflow();
		}
        _gptr++;
	}
    return c;
}
void streambuf::stossc()
{
	if (_fUnbuf)
    {
        if (x_lastc==EOF)
            underflow();        // throw away current character
        else
            x_lastc=EOF;        // discard current cached character
    }
    else
    {
        if (gptr() >= egptr())
            underflow();
        if (gptr() < egptr())
            _gptr++;
    }
}