//
// (c) 2000 Yuriy Govitovskiy,
// for Openh323, www.Openh323.org
//
// Windows CE Port
//   
// iostream.h - definitions/declarations for iostream classes.
//
// [Microsoft]
// [AT&T C++]
//
// [Public]

#ifdef  __cplusplus

#ifndef _INC_IOSTREAM
#define _INC_IOSTREAM

/* Define _CRTIMP */
typedef long streamoff, streampos;

#include <ios.h>                // Define ios.

#include <streamb.h>            // Define streambuf.

#include <istream.h>            // Define istream.

#include <ostream.h>            // Define ostream.


class iostream : public istream, public ostream 
{
public:
        iostream(streambuf*);
        virtual ~iostream();

protected:
        iostream();
        iostream(const iostream&);
inline iostream& operator=(streambuf*);
inline iostream& operator=(iostream&);

private:
        iostream(ios&);
        iostream(istream&);
        iostream(ostream&);
};

inline iostream& iostream::operator=(streambuf* _sb) { istream::operator=(_sb); ostream::operator=(_sb); return *this; }

inline iostream& iostream::operator=(iostream& _strm) { return operator=(_strm.rdbuf()); }

class Iostream_init 
{
public:
        Iostream_init();
        Iostream_init(ios &, int =0);   // treat as private
        ~Iostream_init();
};

#endif  // _INC_IOSTREAM

#endif  /* __cplusplus */
