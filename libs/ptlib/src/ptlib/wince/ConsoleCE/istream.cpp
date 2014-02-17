//
// (c) Yuriy Gorvitovskiy
// for Openh323, www.Openh323.org
//
// Windows CE Port
//
// istream implementation
//

#include <iostream.h>
#include <tchar.h>
#include <stdlibx.h>

#define MAXLONGSIZ 16
#define MAXFLTSIZ  20
#define MAXDBLSIZ  28
#define MAXLDBLSIZ 32



/*******************************************************************************/
istream::istream()
{
        x_flags |= ios::skipws;
        x_gcount = 0;
        _fGline = 0;
}

/*******************************************************************************/
istream::istream(streambuf* _inistbf)
{
        init(_inistbf);

        x_flags |= ios::skipws;
        x_gcount = 0;
        _fGline = 0;
}

/*******************************************************************************/
istream::istream(const istream& _istrm)
{
        init(_istrm.rdbuf());

        x_flags |= ios::skipws;
        x_gcount = 0;
        _fGline = 0;
}

/*******************************************************************************/
istream::~istream()
{
}

// used by ios::sync_with_stdio()
/*******************************************************************************/
istream& istream::operator=(streambuf * _sbuf)
{
        if (delbuf() && rdbuf())
            delete rdbuf();

        bp = 0;

        this->ios::operator=(ios());    // initialize ios members
        delbuf(0);                      // important!
        init(_sbuf);    // set up bp

        x_flags |= ios::skipws;         // init istream members too
        x_gcount = 0;

        return *this;
}

/*******************************************************************************/
int istream::ipfx(int need)
{
    lock();
    if (need)           // reset gcount if unformatted input
        x_gcount = 0;
    if (state)          // return 0 iff error condition
        {
        state |= ios::failbit;  // solves cin>>buf problem
        unlock();
        return 0;
        }
    if (x_tie && ((need==0) || (need > bp->in_avail())))
        {
        x_tie->flush();
        }
    lockbuf();
    if ((need==0) && (x_flags & ios::skipws))
        {
        eatwhite();
        if (state)      // eof or error
            {
            state |= ios::failbit;
            unlockbuf();
            unlock();
            return 0;
            }
        }
    // leave locked ; isfx() will unlock
    return 1;           // return nz if okay
}

// formatted input functions
/*******************************************************************************/
istream& istream::operator>>(char * s)
{
    int c;
    unsigned int i, lim;
    if (ipfx(0))
        {
        lim = (unsigned)(x_width-1);
        x_width = 0;
        if (!s)
            {
            state |= ios::failbit;
            }
        else
		{
            for (i=0; i< lim; i++)
			{
                c=bp->sgetc();
                if (c==EOF)
				{
                    state |= ios::eofbit;
                    if (!i)
                        state |= ios::failbit|ios::badbit;
                    break;
				}
                else if (_istspace(c))
				{
                    break;
				}
                else
				{
                    s[i] = (char)c;
                    bp->stossc(); // advance pointer
				}
			}
            if (!i)
                state |= ios::failbit;
            else
                s[i] = '\0';
            }
        isfx();
        }
    return *this;
}

/*******************************************************************************/
int istream::peek()
{
int retval;
    if (ipfx(1))
        {
        retval = (bp->sgetc());
        isfx();
        }
    else
        retval = EOF;
    return retval;
}

/*******************************************************************************/
istream& istream::putback(char c)
{
      if (good())
        {
        lockbuf();

        if (bp->sputbackc(c)==EOF)
            {
            clear(state | ios::failbit);
            }

        unlockbuf();
        }
    return *this;
}

/*******************************************************************************/
int istream::sync()
{
    int retval;
    lockbuf();

    if ((retval=bp->sync())==EOF)
        {
        clear(state | (ios::failbit|ios::badbit));
        }

    unlockbuf();
    return retval;
}

/*******************************************************************************/
void istream::eatwhite()
{
    int c;
    lockbuf();
    c = bp->sgetc();
    for ( ; ; )
        {
        if (c==EOF)
            {
            clear(state | ios::eofbit);
            break;
            }
        if (isspace(c))
        {
            c = bp->snextc();
        }
        else
            {
            break;
            }
        }
    unlockbuf();
}


/*******************************************************************************/
istream& istream::operator>>(streambuf* _sbuf)
{
    int c;
    if (ipfx(0))
    {
        while ((c=bp->sbumpc())!=EOF)
        {
            if (_sbuf->sputc(c)==EOF)
            {
                state |= ios::failbit;
            }
        }
        isfx();
    }
	return *this;
}


// unformatted input functions
/*******************************************************************************/
istream& istream::get( streambuf& sbuf, char delim)
{
    int c;
    if (ipfx(1))        // resets x_gcount
	{
        while ((c  = bp->sgetc())!=delim)
        {
            if (c==EOF)  // stop if EOF encountered
            {
                state |= ios::eofbit;
                break;
            }
            bp->stossc();       // advance get pointer
            x_gcount++;         // and increment count

            if (sbuf.sputc(c)==EOF)
                state |= ios::failbit;
        }
        isfx();
    }
    return *this;
}

/*******************************************************************************/
istream& istream::seekg(streampos _strmp)
{
    lockbuf();
    if (bp->seekpos(_strmp, ios::in)==EOF)
    {
        clear(state | failbit);
    }
    unlockbuf();
    return(*this);
}

/*******************************************************************************/
istream& istream::seekg(streamoff _strmf, seek_dir _sd)
{
    lockbuf();
    if (bp->seekoff(_strmf, _sd, ios::in)==EOF)
        clear(state | failbit);
    unlockbuf();
    return(*this);
}

/*******************************************************************************/
streampos istream::tellg()
{
    streampos retval;
    lockbuf();
    if ((retval=bp->seekoff(streamoff(0), ios::cur, ios::in))==EOF)
        clear(state | failbit);
    unlockbuf();
    return(retval);
}

/*******************************************************************************/
int istream::get()
{
    int c;
    if (ipfx(1))        // resets x_gcount
        {
        if ((c=bp->sbumpc())==EOF)
            state |= ios::eofbit;
        else
            x_gcount++;
        isfx();
        return c;
        }
    return EOF;
}

/*******************************************************************************/
// signed and unsigned char make inline calls to this:
istream& istream::get( char& c)
{
    int temp;
    if (ipfx(1))        // resets x_gcount
        {
        if ((temp=bp->sbumpc())==EOF)
            state |= (ios::failbit|ios::eofbit);
        else
            x_gcount++;
        c = (char) temp;
        isfx();
        }
    return *this;
}

/*******************************************************************************/
// called by signed and unsigned char versions
istream& istream::read(char * ptr, int n)
{
    if (ipfx(1))        // resets x_gcount
        {
        x_gcount = bp->sgetn(ptr, n);
        if ((unsigned)x_gcount < (unsigned)n)
            state |= (ios::failbit|ios::eofbit);
        isfx();
        }
    return *this;
}


/*******************************************************************************/
int istream::getint(char * buffer)  // returns length
{
    int base, i;
    int c;
    int fDigit = 0;
    int bindex = 1;

    if (x_flags & ios::dec)
        base = 10;
    else if (x_flags & ios::hex)
        base = 16;
    else if (x_flags & ios::oct)
        base = 8;
    else
        base = 0;

    if (ipfx(0))
    {
        c=bp->sgetc();
        for (i = 0; i<MAXLONGSIZ-1; buffer[i] = (char)c,c=bp->snextc(),i++)
        {
            if (c==EOF)
			{
                state |= ios::eofbit;
                break;
		    }
            if (!i)
			{
                if ((c=='-') || (c=='+'))
				{
                    bindex++;
                    continue;
				}
			}
            if ((i==bindex) && (buffer[i-1]== '0'))
			{
                if (((c=='x') || (c=='X')) && ((base==0) || (base==16)))
				{
                    base = 16;  // simplifies matters
                    fDigit = 0;
                    continue;
				}
                else if (base==0)
                {
                    base = 8;
				}
			}


            // now simply look for a digit and set fDigit if found else break
            if (base==16)
			{
                if (!_istxdigit(c))
                    break;
			}
            else if ((!_istdigit(c)) || ((base==8) && (c>'7')))
                break;

            fDigit++;
		}
        if (!fDigit)
		{
            state |= ios::failbit;
            while (i--)
			{
                if(bp->sputbackc(buffer[i])==EOF)
				{
                    state |= ios::badbit;
                    break;
				}
                else
                    state &= ~(ios::eofbit);
			}
            i=0;
		}
        // buffer contains a valid number or '\0'
        buffer[i] = '\0';
        isfx();
	}
    if (i==MAXLONGSIZ)
    {
        state |= ios::failbit;
	}
    return base;
}

/*******************************************************************************/
int     istream::getdouble(char* buffer, int buflen)   // returns length
{
    int c;
    int i = 0;
    int fDigit = 0;     // true if legal digit encountered
    int fDecimal=0;     // true if '.' encountered or no longer valid
    int fExp=0;         // true if 'E' or 'e' encounted

    if (ipfx(0))
    {
        c=bp->sgetc();
        for (; i<buflen; buffer[i] = c,c=bp->snextc(),i++)
		{
            if (c==EOF)
			{
                state |= ios::eofbit;
                break;
			}
            if ((!i) || (fExp==1))
			{
                if ((c=='-') || (c=='+'))
				{
                    continue;
				}
			}
            if ((c=='.') && (!fExp) && (!fDecimal))
			{
                fDecimal++;
                continue;
			}
            if (((c=='E') || (c=='e')) && (!fExp))
			{
                fDecimal++;     // can't allow decimal now
                fExp++;
                continue;
			}
            if (!_istdigit(c))
                break;
            if (fExp)
                fExp++;
            else
                fDigit++;
		}
        if (fExp==1)            // E or e with no number after it
		{
            if (bp->sputbackc(buffer[i])!=EOF)
			{
                i--;
                state &= ~(ios::eofbit);
			}
            else
			{
                state |= ios::failbit;
			}
		}
        if ((!fDigit) || (i==buflen))
            state |= ios::failbit;

        // buffer contains a valid number or '\0'
        buffer[i] = '\0';
        isfx();
	}
    return i;
}

/*******************************************************************************/
istream& istream::operator>>(char& c)
{
    int tchar;
    if (ipfx(0))
        {
        tchar=bp->sbumpc();
        if (tchar==EOF)
            {
            state |= ios::eofbit|ios::badbit;
            }
        else
            {
            c = (char)tchar;
            }
        isfx();
        }
    return *this;
}

/*******************************************************************************/
istream& istream::operator>>(short& n)
{
	char ibuffer[MAXLONGSIZ];
    long value;
    char** endptr = (char**)NULL;
    if (ipfx(0))
    {
        value = strtol(ibuffer, endptr, getint(ibuffer));
        if (value>SHRT_MAX)
		{
            n = SHRT_MAX;
            state |= ios::failbit;
		}
        else if (value<SHRT_MIN)
		{
            n = SHRT_MIN;
            state |= ios::failbit;
		}
        else
            n = (short) value;

        isfx();
	}
	return *this;
}

/*******************************************************************************/
istream& istream::operator>>(unsigned short& n)
{	
	char ibuffer[MAXLONGSIZ];
    unsigned long value;
    char** endptr = (char**)NULL;
    if (ipfx(0))
    {
        value = strtoul(ibuffer, endptr, getint(ibuffer));

        if ((value>USHRT_MAX) && (value<=(ULONG_MAX-(-SHRT_MIN))))
        {
			n = USHRT_MAX;
			state |= ios::failbit;
        }
        else
            n = (unsigned short) value;

        isfx();
	}
	return *this;
}

/*******************************************************************************/
istream& istream::operator>>(int& n)
{
	char ibuffer[MAXLONGSIZ];
    long value;
    char ** endptr = (char**)NULL;
    if (ipfx(0))
	{
        value = strtoul(ibuffer, endptr, getint(ibuffer));
        if (value>INT_MAX)
		{
            n = INT_MAX;
            state |= ios::failbit;
		}
        else if (value<INT_MIN)
        {
            n = INT_MIN;
            state |= ios::failbit;
        }
        else
            n = (int) value;

        isfx();
	}
	return *this;
}

/*******************************************************************************/
istream& istream::operator>>(unsigned int& n)
{
	char ibuffer[MAXLONGSIZ];
    unsigned long value;
    char ** endptr = (char**)NULL;
    if (ipfx(0)) 
	{
        value = strtoul(ibuffer, endptr, getint(ibuffer));

        if ((value>UINT_MAX) && (value<=(ULONG_MAX-(unsigned long)(-INT_MIN))))
		{
            n = UINT_MAX;
            state |= ios::failbit;
		}
        else
            n = (unsigned int) value;

        isfx();
	}
	return *this;
}

/*******************************************************************************/
istream& istream::operator>>(long& n)
{
	char ibuffer[MAXLONGSIZ];
    char ** endptr = (char**)NULL;
    if (ipfx(0)) 
	{
        n = strtoul(ibuffer, endptr, getint(ibuffer));
        isfx();
    }
	return *this;
}

/*******************************************************************************/
istream& istream::operator>>(unsigned long& n)
{
	char ibuffer[MAXLONGSIZ];
    char ** endptr = (char**)NULL;
    if (ipfx(0)) 
	{
        n = strtoul(ibuffer, endptr, getint(ibuffer));
        isfx();
	}
	return *this;
}

/*******************************************************************************/
istream& istream::operator>>(float& n)
{
	char ibuffer[MAXFLTSIZ];
    double d;
    char ** endptr = (char**)NULL;
    if (ipfx(0))
	{
        if (getdouble(ibuffer, MAXFLTSIZ)>0)
        {
            d = strtod(ibuffer, endptr);

            if (d > FLT_MAX)
                n = FLT_MAX;
            else if (d < -FLT_MAX)
                n =  -FLT_MAX;
            else if ((d>0) && (d< FLT_MIN))
                n = FLT_MIN;
            else if ((d<0) && (d> -FLT_MIN))
                n = - FLT_MIN;
            else
                n = (float) d;
		}
        isfx();
	}
	return *this;
}

/*******************************************************************************/
istream& istream::operator>>(double& n)
{
	char ibuffer[MAXDBLSIZ];
    char ** endptr = (char**)NULL;
    if (ipfx(0))
	{
        if (getdouble(ibuffer, MAXDBLSIZ)>0)
		{
            n = strtod(ibuffer, endptr);
		}
        isfx();
	}
	return *this;
}

/*******************************************************************************/
istream& istream::operator>>(long double& n)
{
	char ibuffer[MAXLDBLSIZ];
    char** endptr = (char**)NULL;
    if (ipfx(0))
	{
        if (getdouble(ibuffer, MAXLDBLSIZ)>0)
		{
            n = (long double)strtod(ibuffer, endptr);
		}
        isfx();
	}
	return *this;
}

/*******************************************************************************/
istream_withassign::istream_withassign()
: istream()
{
}

/*******************************************************************************/
istream_withassign::istream_withassign(streambuf* _is)
: istream(_is)
{
}

/*******************************************************************************/
istream_withassign::~istream_withassign()
{
}
