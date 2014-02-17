//
// (c) 2000 Yuri Kiryanov, openh323@kiryanov.com
// and Yuriy Gorvitovskiy
//
// Windows CE port of OpenH323 Open Source Project, www.openh323.org
// PWLib extras

//Yuriy Gorvitovskiy 23.06.00

#include "ptlib.h"

#ifdef _WIN32_WCE
#pragma warning(disable : 4073)
#pragma init_seg(lib)

YWinCEOStream cerr;
YWinCEOStream cout;
YWinCEOStream clog;
#else
#include "../../../include/ptlib/wince/ceostream.h"
#endif

#define BufferSize 1024

class YWinCEOStreamBuffer : public streambuf 
{
public:
			 YWinCEOStreamBuffer();
	virtual ~YWinCEOStreamBuffer();
    
    virtual int overflow(int=EOF);
    virtual int sync();

    virtual int underflow()			{return EOF;}
	virtual streampos seekoff(streamoff,ios::seek_dir,int =ios::in|ios::out) { return 0;}

    char Buffer[BufferSize+1];

	HWND m_hWnd;
	UINT m_MessageID;
    YWinCEOStreamCB m_pCB; // pointer to callback function
};

YWinCEOStreamBuffer::YWinCEOStreamBuffer():
	m_hWnd(NULL)
{
	setp(Buffer,Buffer+BufferSize);
	setg(0,0,0);
}

YWinCEOStreamBuffer::~YWinCEOStreamBuffer()
{
	m_hWnd=NULL;
}

int YWinCEOStreamBuffer::sync()
{
	int ipos = pptr() - pbase();
	if (ipos>0)
	{
		Buffer[ipos]=0;
	    
		if (NULL != m_hWnd) // Window call
		{
			if (::IsWindow(m_hWnd))
				::SendMessage(m_hWnd,m_MessageID,(WPARAM)(void*)Buffer,0);
		}
	    else 
		if (NULL != m_pCB) // Callback call
		{
			(*m_pCB)((WPARAM)(void*)Buffer);
		}
	}
	setp(Buffer,Buffer+BufferSize);
    return 0;
}

int YWinCEOStreamBuffer::overflow(int c)
{
	if (pptr() >= epptr()) 
	{
		sync();
	}
	if (c != EOF) 
	{
		*pptr() = (char)c;
		pbump(1);
	}
	return 0;
}

YWinCEOStream::YWinCEOStream()
	:ostream(new YWinCEOStreamBuffer())
{
}

YWinCEOStream::~YWinCEOStream()
{
}

void YWinCEOStream::Subscribe(HWND Reciever,UINT MessageID)
{
	lockbuf();
	if (bp)
	{
		((YWinCEOStreamBuffer*)bp)->m_hWnd = Reciever;
		((YWinCEOStreamBuffer*)bp)->m_MessageID = MessageID;
		((YWinCEOStreamBuffer*)bp)->m_pCB = NULL;
	}
	unlockbuf();
}

void YWinCEOStream::Subscribe(YWinCEOStreamCB pCB)
{
	lockbuf();
	if (bp)
	{
		((YWinCEOStreamBuffer*)bp)->m_hWnd = NULL;
		((YWinCEOStreamBuffer*)bp)->m_pCB = pCB;
	}
	unlockbuf();
}

void YWinCEOStream::UnSubscribe()
{
	lockbuf();
	((YWinCEOStreamBuffer*)bp)->m_hWnd=NULL;
	((YWinCEOStreamBuffer*)bp)->m_pCB = NULL;
	unlockbuf();
}

