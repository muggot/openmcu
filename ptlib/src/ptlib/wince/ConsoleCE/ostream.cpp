//
// (c) 2000 Yuri Kiryanov, openh323@kiryanov.com
// and Yuriy Gorvitovskiy
//
// Windows CE port of OpenH323 Open Source Project, www.openh323.org
// PWLib extras

#include <iostream.h>
#include <ptlib.h>

#ifdef WCE_NO_EXTERNAL_STREAMS
ostream_withassign cerr;
ostream_withassign cout;
ostream_withassign clog;

static Iostream_init  __InitCerr(cerr,1);
static Iostream_init  __InitCout(cout,-1);

PStringStream cerr;
PStringStream cout;
PStringStream clog;
#endif // WCE_NO_EXTERNAL_STREAMS

/*******************************************************************************/
ostream::ostream()
{
    x_floatused = 0;
}

/*******************************************************************************/
ostream::ostream(streambuf* _inistbf)
{
    init(_inistbf);

    x_floatused = 0;
}

/*******************************************************************************/
ostream::ostream(const ostream& _ostrm)
{
    init(_ostrm.rdbuf());

    x_floatused = 0;
}

/*******************************************************************************/
ostream::~ostream()
{
}

/*******************************************************************************/
int ostream::opfx()
{
    lock();
    if (state)
	{
        state |= ios::failbit;
        unlock();
        return 0;
	}
    if (x_tie)
	{
        x_tie->flush();
	}
    lockbuf();
    return(1);  // return non-zero
}

/*******************************************************************************/
void ostream::osfx()
{
    x_width = 0;
    if (x_flags & unitbuf)
	{
        if (bp->sync()==EOF)
            state = failbit | badbit;
	}
    if (x_flags & ios::stdio)
	{
        if (fflush(stdout)==EOF)
            state |= failbit;
        if (fflush(stderr)==EOF)
            state |= failbit;
	}
    unlockbuf();
    unlock();
}


/*******************************************************************************/
ostream& ostream::operator<<(const char * s)
{
    if (opfx()) 
	{
        writepad("",s);
        osfx();
    }
    return *this;
}

/*******************************************************************************/
ostream& ostream::flush()
{
    lock();
    lockbuf();
    if (bp && bp->sync()==EOF)
        state |= ios::failbit;
    unlockbuf();
    unlock();
    return(*this);
}


/*******************************************************************************/
ostream& ostream::operator=(streambuf * _sbuf)
{
    if (delbuf() && rdbuf())
        delete rdbuf();

    bp = 0;

    this->ios::operator=(ios());    // initialize ios members
    delbuf(0);                      // important!
    init(_sbuf);

    return *this;
}

/*******************************************************************************/
ostream& ostream::writepad(const char * leader, const char * value)
{
	unsigned int len, leadlen;
	long padlen;
    leadlen = strlen(leader);
    len = strlen(value);
    padlen = (((unsigned)x_width) > (len+leadlen)) ? ((unsigned)x_width) - (len + leadlen) : 0;
    if (!(x_flags & (left|internal)))  // default is right-adjustment
	{
        while (padlen-- >0)
		{
            if (bp->sputc((unsigned char)x_fill)==EOF)
                state |= (ios::failbit|ios::badbit);
		}
	}
    
	if (leadlen)
	{
        if ((unsigned)bp->sputn(leader,leadlen)!=leadlen)
            state |= (failbit|badbit);
	}

	if (x_flags & internal)
	{
		while (padlen-- >0)
		{
            if (bp->sputc((unsigned char)x_fill)==EOF)
                state |= (ios::failbit|ios::badbit);
		}
	}

    if ((unsigned)bp->sputn(value,len)!=len)
        state |= (failbit|badbit);

    if (x_flags & left)
	{
        while ((padlen--)>0)        // left-adjust if necessary
		{
            if (bp->sputc((unsigned char)x_fill)==EOF)
                state |= (ios::failbit|ios::badbit);
		}
	}
    return (*this);
}

/*******************************************************************************/
ostream& ostream::seekp(streampos _strmp)
{
    lockbuf();

    if (bp->seekpos(_strmp, ios::out)==EOF)
        clear(state | failbit);

    unlockbuf();
    return(*this);
}

/*******************************************************************************/
ostream& ostream::seekp(streamoff _strmf, seek_dir _sd)
{
    lockbuf();

    if (bp->seekoff(_strmf, _sd, ios::out)==EOF)
        clear(state | failbit);

    unlockbuf();
    return(*this);
}

/*******************************************************************************/
streampos ostream::tellp()
{
    streampos retval;
    lockbuf();

    if ((retval=bp->seekoff(streamoff(0), ios::cur, ios::out))==EOF)
        clear(state | failbit);

    unlockbuf();
    return(retval);
}

/*******************************************************************************/
ostream& ostream::put(unsigned char c)
{
    if (opfx())
	{
        if (bp->sputc((int)c)==EOF)
            state |= (failbit|badbit);
        osfx();
	}
    return(*this);
}

/*******************************************************************************/
ostream& ostream::write(const char * s, int n)
{
    if (opfx())
	{
// Note: 'n' treated as unsigned
        if (bp->sputn(s,n)!=n)
            state |= (failbit|badbit);
        osfx();
	}
    return(*this);
}
/*******************************************************************************/
ostream& ostream::operator<<(streambuf * instm)
{
    int c;
    if (opfx())
	{
        while ((c=instm->sbumpc())!=EOF)
            if (bp->sputc(c) == EOF)
			{
                state |= failbit;
                break;
			}
        osfx();
	}
    return *this;
}


/*******************************************************************************/
ostream&  ostream::operator<<(unsigned char c)
{
    if (opfx())
    {
        if (x_width)
		{
                char outc[2];
            outc[0] = c;
            outc[1] = '\0';
            writepad("",outc);
		}
        else if (bp->sputc(c)==EOF)
        {
            if (bp->overflow(c)==EOF)
                state |= (badbit|failbit);  // fatal error?
		}
        osfx();
    }
    return *this;
}

/*******************************************************************************/
ostream& ostream::operator<<(short n)
{
    char obuffer[8];     // assumes max int is 65535
    char fmt[4] = "%hd";
    char leader[4] = "\0\0";
    if (opfx()) 
	{
        if (n)
		{
            if (x_flags & (hex|oct))
			{
                if (x_flags & hex)
				{
                    if (x_flags & uppercase)
                        fmt[2] = 'X';
                    else
                        fmt[2] = 'x';
                    leader[1] = fmt[2];   // 0x or 0X  (or \0X)
				}
                else
                    fmt[2] = 'o';
                if (x_flags & showbase)
                    leader[0] = '0';
			}
            else if ((n>0) && (x_flags & showpos))
			{
                leader[0] = '+';
			}
		}
        sprintf(obuffer,fmt,n);
        writepad(leader,obuffer);
        osfx();
    }
    return *this;
}

/*******************************************************************************/
ostream& ostream::operator<<(unsigned short n)
{
    char obuffer[8];
    char fmt[4] = "%hu";
    char leader[4] = "\0\0";
    if (opfx()) 
	{
        if (n)
		{
            if (x_flags & (hex|oct))
			{
                if (x_flags & hex)
				{
                    if (x_flags & uppercase)
                        fmt[2] = 'X';
                    else
                        fmt[2] = 'x';
                    leader[1] = fmt[2];   // 0x or 0X  (or \0X)
				}
                else
                    fmt[2] = 'o';
                if (x_flags & showbase)
                    leader[0] = '0';
			}
            else if (x_flags & showpos)
			{
                leader[0] = '+';
			}
		}
        sprintf(obuffer,fmt,n);
        writepad(leader,obuffer);
        osfx();
    }
    return *this;
}

/*******************************************************************************/
ostream& ostream::operator<<(long n)
{
    char obuffer[12];
    char fmt[4] = "%ld";
    char leader[4] = "\0\0";
    if (opfx()) 
	{
        if (n)
		{
            if (x_flags & (hex|oct))
			{
                if (x_flags & hex)
				{
                    if (x_flags & uppercase)
                        fmt[2] = 'X';
                    else
                        fmt[2] = 'x';
                    leader[1] = fmt[2];   // 0x or 0X  (or \0X)
				}
                else
                    fmt[2] = 'o';
                if (x_flags & showbase)
                    leader[0] = '0';
			}
            else if ((n>0) && (x_flags & showpos))
			{
                leader[0] = '+';
			}
		}
        sprintf(obuffer,fmt,n);
        writepad(leader,obuffer);
        osfx();
    }
    return *this;

}

/*******************************************************************************/
ostream& ostream::operator<<(unsigned long n)
{
    char obuffer[12];
    char fmt[4] = "%lu";
    char leader[4] = "\0\0";
    if (opfx()) 
	{
        if (n)
		{
            if (x_flags & (hex|oct))
			{
                if (x_flags & hex)
				{
                    if (x_flags & uppercase)
                        fmt[2] = 'X';
                    else
                        fmt[2] = 'x';
                    leader[1] = fmt[2];   // 0x or 0X  (or \0X)
				}
                else
                    fmt[2] = 'o';
                if (x_flags & showbase)
                    leader[0] = '0';
			}
            else if (x_flags & showpos)
			{
                leader[0] = '+';
			}
		}
        sprintf(obuffer,fmt,n);
        writepad(leader,obuffer);
        osfx();
    }
    return *this;
}

/*******************************************************************************/
ostream& ostream::operator<<(int n)
{
    char obuffer[12];
    char fmt[4] = "%d";
    char leader[4] = "\0\0";
    if (opfx()) 
	{
        if (n)
		{
            if (x_flags & (hex|oct))
			{
                if (x_flags & hex)
				{
                    if (x_flags & uppercase)
                        fmt[1] = 'X';
                    else
                        fmt[1] = 'x';
                    leader[1] = fmt[1];   // 0x or 0X  (or \0X)
				}
                else
                    fmt[1] = 'o';
                if (x_flags & showbase)
                    leader[0] = '0';
			}
            else if ((n>0) && (x_flags & showpos))
			{
                leader[0] = '+';
			}
		}
        sprintf(obuffer,fmt,n);
        writepad(leader,obuffer);
        osfx();
    }
    return *this;
}

/*******************************************************************************/
ostream& ostream::operator<<(unsigned int n)
{
    char obuffer[12];
    char fmt[4] = "%u";
    char leader[4] = "\0\0";
    if (opfx()) 
	{
        if (n)
		{
            if (x_flags & (hex|oct))
			{
                if (x_flags & hex)
				{
                    if (x_flags & uppercase)
                        fmt[1] = 'X';
                    else
                        fmt[1] = 'x';
                    leader[1] = fmt[1];   // 0x or 0X  (or \0X)
				}
                else
                    fmt[1] = 'o';
                if (x_flags & showbase)
                    leader[0] = '0';
			}
            else if (x_flags & showpos)
			{
                leader[0] = '+';
			}
		}
        sprintf(obuffer,fmt,n);
        writepad(leader,obuffer);
        osfx();
    }
    return *this;
}

/*******************************************************************************/
ostream& ostream::operator<<(double f)
{
    char obuffer[24];
    char fmt[8];
    char leader[4];
    char * optr = obuffer;
    int x = 0;

    // x_floatused nonzero indicates called for float, not double
    unsigned int curprecision = (x_floatused) ? FLT_DIG : DBL_DIG;
    x_floatused = 0;    // reset for next call

    curprecision = __min((unsigned)x_precision,curprecision);

    if (opfx()) 
	{
        if (x_flags & ios::showpos)
            leader[x++] = '+';
        if (x_flags & ios::showpoint)
            leader[x++] = '#';  // show decimal and trailing zeros
        leader[x] = '\0';
        x = sprintf(fmt,"%%%s.%.0ug",leader,curprecision) - 1;
        if ((x_flags & ios::floatfield)==ios::fixed)
            fmt[x] = 'f';
        else
		{
            if ((x_flags & ios::floatfield)==ios::scientific)
                fmt[x] = 'e';
            if (x_flags & uppercase)
                fmt[x] = (char)toupper(fmt[x]);
		}

        sprintf(optr,fmt,f);
        x = 0;
        if (*optr=='+' || *optr=='-')
            leader[x++] = *(optr++);
        leader[x] = '\0';
        writepad(leader,optr);
        osfx();
	}
    return *this;
}

/*******************************************************************************/
ostream& ostream::operator<<(long double f)
{
    char obuffer[28];
    char fmt[12];
    char leader[4];
    char * optr = obuffer;
    int x = 0;
    unsigned int curprecision = __min((unsigned)x_precision,LDBL_DIG);
    if (opfx()) 
	{
        if (x_flags & ios::showpos)
            leader[x++] = '+';
        if (x_flags & ios::showpoint)
            leader[x++] = '#';  // show decimal and trailing zeros
        leader[x] = '\0';
        x = sprintf(fmt,"%%%s.%.0uLg",leader,curprecision) - 1;
        if ((x_flags & ios::floatfield)==ios::fixed)
            fmt[x] = 'f';
        else
		{
            if ((x_flags & ios::floatfield)==ios::scientific)
                fmt[x] = 'e';
            if (x_flags & uppercase)
                fmt[x] = (char)toupper(fmt[x]);
		}

        sprintf(optr,fmt,f);
        x = 0;
        if (*optr=='+' || *optr=='-')
            leader[x++] = *(optr++);
        leader[x] = '\0';
        writepad(leader,optr);
        osfx();
	}
    return *this;
}


/*******************************************************************************/
ostream& ostream::operator<<(const void * ptr)
{
    char obuffer[12];
    char fmt[4] = "%p";
    char leader[4] = "0x";
    if (opfx())
	{
        if (ptr)
		{
            if (x_flags & uppercase)
                leader[1] = 'X';
		}
        sprintf(obuffer,fmt,ptr);
        writepad(leader,obuffer);
        osfx();
	}
    return *this;
}


/*******************************************************************************/
ostream_withassign::ostream_withassign()
	: ostream()
{
}


/*******************************************************************************/
ostream_withassign::ostream_withassign(streambuf* _os)
	: ostream(_os)
{
}

/*******************************************************************************/
ostream_withassign::~ostream_withassign()
{
}
