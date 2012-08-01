//
// (c) Yuriy Gorvitovskiy
// for Openh323, www.Openh323.org
//
// Windows CE Port
//
// iostream implementation
//

#include <iostream.h>

/*******************************************************************************/
iostream::iostream()
: istream(), ostream()
{
}

/*******************************************************************************/
iostream::iostream(streambuf* pSB)
: istream(pSB), ostream(pSB)
{
}

/*******************************************************************************/
iostream::iostream(const iostream& Strm)
: istream(Strm), ostream(Strm)
{
}

/*******************************************************************************/
iostream::~iostream()
{
if ((istream::bp==ostream::bp) && (&istream::bp!=&ostream::bp))
        istream::bp = NULL;     // let ostream::ios::~ios() do it
}

/*******************************************************************************/
//!!![YG] ostream_withassign cout(new filebuf(1));
//!!![YG] static Iostream_init  __InitCout(cout,-1);

/*******************************************************************************/
Iostream_init::Iostream_init() 
{
}

/*******************************************************************************/
Iostream_init::Iostream_init(ios& pstrm, int sflg)
{
    pstrm.delbuf(1);
    if (sflg>=0)    // make sure not cout
            pstrm.tie(&cout);
    if (sflg>0)
            pstrm.setf(ios::unitbuf);
}

/*******************************************************************************/
Iostream_init::~Iostream_init() 
{ 
}

