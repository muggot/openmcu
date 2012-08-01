#ifndef _YWINCEOSTREAM_H
#define _YWINCEOSTREAM_H

// callback function type for subscribing to trace messages
typedef void (*YWinCEOStreamCB)(WPARAM);

class YWinCEOStream : public ostream
{
public:
	YWinCEOStream();
	~YWinCEOStream();

	void Subscribe(HWND Reciever,UINT MessageID);
    void Subscribe(YWinCEOStreamCB pCB);
	void UnSubscribe();
};

#endif _YWINCEOSTREAM_H
