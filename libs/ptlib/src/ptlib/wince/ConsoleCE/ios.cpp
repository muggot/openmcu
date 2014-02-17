//
// (c) Yuriy Gorvitovskiy
// for Openh323, www.Openh323.org
//
// Windows CE Port
//
// ios implementation
//

#include <ptlib.h>
#include <iostream.h>

const long ios::basefield = (ios::dec | ios::oct | ios::hex);
const long ios::adjustfield = (ios::left | ios::right | ios::internal);
const long ios::floatfield = (ios::scientific | ios::fixed);

long*	ios::x_statebuf = NULL;
long	ios::x_maxbit = 0x8000;    // ios::openprot
int		ios::x_curindex = -1;


/*******************************************************************************/

ios::ios()
{
    bp = NULL;
    state = ios::badbit;

    ispecial = 0;
    ospecial = 0;
    x_tie = (0);
    x_flags = 0;
    x_precision = 6;
    x_fill = ' ';
    x_width = 0;
    x_delbuf = 0;
}


/*******************************************************************************/

ios::ios( streambuf* pSB )
{
    bp = pSB;
    state = (bp) ? 0 : ios::badbit;

    ispecial = 0;
    ospecial = 0;
    x_tie = (0);
    x_flags = 0;
    x_precision = 6;
    x_fill = ' ';
    x_width = 0;
    x_delbuf = 0;
}

/*******************************************************************************/
ios::ios(const ios& _strm)      // copy constructor
{
    bp = NULL;
    x_delbuf = 0;

    *this = _strm;              // invoke assignment operator
}

/*******************************************************************************/

ios::~ios()
{
    if ((x_delbuf) && (bp))
        delete bp;

    bp = NULL;
    state = badbit;
}


/*******************************************************************************/

void ios::init( streambuf* pSB )
{
    if (delbuf() && (bp))       // delete previous bp if necessary
        delete bp;

    bp = pSB;
    if (bp)
        state &= ~ios::badbit;
    else
        state |= ios::badbit;
}


/*******************************************************************************/

ios& ios::operator=(const ios& _strm)
{
        x_tie = _strm.tie();
        x_flags = _strm.flags();
        x_precision = (char)_strm.precision();
        x_fill  = _strm.fill();
        x_width = (char)_strm.width();

        state = _strm.rdstate();
        if (!bp)
            state |= ios::badbit;       // adjust state for uninitialized bp

        return *this;
}

/*******************************************************************************/
int  ios::xalloc()
{
    long * tptr;
    int i;

    if (!(tptr=new long[x_curindex+2]))    // allocate new buffer
        return EOF;

    for (i=0; i <= x_curindex; i++)     // copy old buffer, if any
        tptr[i] = x_statebuf[i];

    tptr[++x_curindex] = 0L;            // init new entry, bump size

    if (x_statebuf)                     // delete old buffer, if any
        delete x_statebuf;

    x_statebuf = tptr;                  // and assign new buffer
    return x_curindex;
}

/*******************************************************************************/
long ios::bitalloc()
{
    long b;
    lockc();            // lock to make sure mask in unique (_MT)
    b = (x_maxbit<<=1);
    unlockc();
    return b;
}

