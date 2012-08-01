//
// (c) 2000 Yuriy Govitovskiy,
// for Openh323, www.Openh323.org
//
// Windows CE Port
//   
// ostream.h - definitions/declarations for the ostream class
//
// [Microsoft]
// [AT&T C++]
//
// [Public]

#ifdef  __cplusplus

#ifndef _INC_OSTREAM
#define _INC_OSTREAM

#include <ios.h>

typedef long streamoff, streampos;

class ostream : virtual public ios 
{
public:
        ostream(streambuf*);
        virtual ~ostream();

        ostream& flush();
        int  opfx();
        void osfx();

inline  ostream& operator<<(ostream& (__cdecl * _f)(ostream&));
inline  ostream& operator<<(ios& (__cdecl * _f)(ios&));
        ostream& operator<<(const char *);
inline  ostream& operator<<(const unsigned char *);
inline  ostream& operator<<(const signed char *);
inline  ostream& operator<<(char);
        ostream& operator<<(unsigned char);
inline  ostream& operator<<(signed char);
        ostream& operator<<(short);
        ostream& operator<<(unsigned short);
        ostream& operator<<(int);
        ostream& operator<<(unsigned int);
        ostream& operator<<(long);
        ostream& operator<<(unsigned long);
inline  ostream& operator<<(float);
        ostream& operator<<(double);
        ostream& operator<<(long double);
        ostream& operator<<(const void *);
        ostream& operator<<(streambuf*);
inline  ostream& put(char);
        ostream& put(unsigned char);
inline  ostream& put(signed char);
        ostream& write(const char *,int);
inline  ostream& write(const unsigned char *,int);
inline  ostream& write(const signed char *,int);
        ostream& seekp(streampos);
        ostream& seekp(streamoff,ios::seek_dir);
        streampos tellp();

protected:
        ostream();
        ostream(const ostream&);        // treat as private
        ostream& operator=(streambuf*); // treat as private
        ostream& operator=(const ostream& _os) {return operator=(_os.rdbuf()); }
        int do_opfx(int);               // not used
        void do_osfx();                 // not used

private:
        ostream(ios&);
        ostream& writepad(const char *, const char *);
        int x_floatused;
};

inline ostream& ostream::operator<<(ostream& (__cdecl * _f)(ostream&)) { (*_f)(*this); return *this; }
inline ostream& ostream::operator<<(ios& (__cdecl * _f)(ios& )) { (*_f)(*this); return *this; }

inline  ostream& ostream::operator<<(char _c) { return operator<<((unsigned char) _c); }
inline  ostream& ostream::operator<<(signed char _c) { return operator<<((unsigned char) _c); }

inline  ostream& ostream::operator<<(const unsigned char * _s) { return operator<<((const char *) _s); }
inline  ostream& ostream::operator<<(const signed char * _s) { return operator<<((const char *) _s); }

inline  ostream& ostream::operator<<(float _f) { x_floatused = 1; return operator<<((double) _f); }

inline  ostream& ostream::put(char _c) { return put((unsigned char) _c); }
inline  ostream& ostream::put(signed char _c) { return put((unsigned char) _c); }

inline  ostream& ostream::write(const unsigned char * _s, int _n) { return write((char *) _s, _n); }
inline  ostream& ostream::write(const signed char * _s, int _n) { return write((char *) _s, _n); }


class ostream_withassign : public ostream 
{
        public:
                ostream_withassign();
                ostream_withassign(streambuf* _is);
                ~ostream_withassign();
    ostream& operator=(const ostream& _os) { return ostream::operator=(_os.rdbuf()); }
    ostream& operator=(streambuf* _sb) { return ostream::operator=(_sb); }
};

#ifndef WCE_NO_EXTERNAL_STREAMS
#include "ceostream.h"
extern YWinCEOStream cout;
extern YWinCEOStream cerr;
extern YWinCEOStream clog;

#else
extern ostream_withassign cout;
extern ostream_withassign cerr;
extern ostream_withassign clog;
class PStringStream;

extern PStringStream cerr;
extern PStringStream cout;
extern PStringStream clog;
#endif // !WCE_NO_EXTERNAL_STREAMS

inline ostream& flush(ostream& _outs) { return _outs.flush(); }
inline ostream& endl(ostream& _outs) { return _outs << '\n' << flush; }
inline ostream& ends(ostream& _outs) { return _outs << char('\0'); }

ios& dec(ios&);
ios& hex(ios&);
ios& oct(ios&);

#endif  // _INC_OSTREAM

#endif  /* __cplusplus */
