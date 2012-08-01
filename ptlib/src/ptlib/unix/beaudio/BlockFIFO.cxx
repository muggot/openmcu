//
// (c) Yuri Kiryanov, openh323@kiryanov.com
// for www.Openh323.org by Equivalence
//
// Portions: 1998-1999, Be Incorporated
// Be Sample Code License

/*----------------------
Be Sample Code License
----------------------

Copyright 1991-1999, Be Incorporated.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions, and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions, and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "BlockFIFO.h"

#if NDEBUG
#define FPRINTF(x)
#else
#define FPRINTF(x) fprintf x
#endif

//	if we decice to make the FIFO thread safe, use these macros
#define ENTER_GET
#define LEAVE_GET
#define ENTER_PUT
#define LEAVE_PUT

BBlockFIFO::BBlockFIFO(size_t blockSize, int32 blockCountPerBuffer, int32 bufferCount, uint32 placementFlags, uint32 lockFlags, const char * name)
{
	char tname[64];
	if (!name) {
		sprintf(tname, "FIFO(0x%6lx,%ld,%ld)", blockSize, blockCountPerBuffer, bufferCount);
		name = tname;
	}
	strncpy(_mName, name, 32);
	_mName[31] = 0;
	_mBuffer = 0;
	_mFlags = 0;
	_mBlockSize = blockSize;
	_mBufferSize = blockCountPerBuffer * blockSize;
	_mAreaSize = _mBufferSize * bufferCount;
	_mGetOff = 0;
	_mPutOff = 0;
	_mGetSem = -1;
	_mPutSem = -1;
	_mInitErr = B_OK;

	size_t s = (_mAreaSize + B_PAGE_SIZE-1) & -B_PAGE_SIZE;
	if ((blockSize < 1) || (blockCountPerBuffer < 1) || (bufferCount < 2) || (s < B_PAGE_SIZE) || (s > 0x1000000UL)) {
		_mInitErr = B_BAD_VALUE;
	}
	else {
		void * addr = 0;
		_mArea = create_area(name, &addr, placementFlags, s, lockFlags, B_READ_AREA | B_WRITE_AREA);
		if (_mArea < 0) {
			_mInitErr = _mArea;
		}
		else {
			_mBuffer = (char *)addr;
			_mInitErr = Reset();
		}
	}
}


BBlockFIFO::~BBlockFIFO()
{
	if (_mArea > -1) delete_area(_mArea);
	if (_mGetSem > -1) delete_sem(_mGetSem);
	if (_mPutSem > -1) delete_sem(_mPutSem);
}

status_t
BBlockFIFO::InitCheck()
{
	return _mInitErr;
}

status_t 
BBlockFIFO::Reset()
{
	if (_mInitErr < 0) return _mInitErr;
	if (_mGetSem > -1) delete_sem(_mGetSem);
	if (_mPutSem > -1) delete_sem(_mPutSem);
ENTER_GET
ENTER_PUT
	char name[32];
	sprintf(name, "%.27s Get", _mName);
	_mGetSem = create_sem(0, name);
	sprintf(name, "%.27s Put", _mName);
	_mPutSem = create_sem(_mAreaSize, name);
	if (_mGetSem < 0) _mInitErr = _mGetSem;
	if (_mPutSem < 0) _mInitErr = _mPutSem;
	_mGetOff = 0;
	_mPutOff = 0;
	_mFlags = 0;
LEAVE_PUT
LEAVE_GET
	return _mInitErr;
}

int32 
BBlockFIFO::SizeAvailableToGet()
{
ENTER_GET	//	there's always a race with put, so don't pretend to protect it
	int32 s = _mPutOff - _mGetOff;
	if (s < 0) s += _mAreaSize;
LEAVE_GET
	return s;
}

int32 
BBlockFIFO::SizeAvailableToPut()
{
ENTER_PUT	//	there's always a race with get, so don't pretend to protect it
	int32 s = _mGetOff - _mPutOff;
	if (s <= 0) s += _mAreaSize;
LEAVE_PUT
	return s;
}

int32 
BBlockFIFO::BeginGet(const void **outData, size_t requestSize, bigtime_t timeout)
{
	if (!outData) { FPRINTF((stderr, "BAD_VALUE: outData is NULL\n")); return B_BAD_VALUE; }
ENTER_GET
//FPRINTF((stderr, "requestSize %ld  _mBlockSize %ld  _mAreaSize %ld  _mGetOff %ld\n",
//		requestSize, _mBlockSize, _mAreaSize, _mGetOff));
	if (requestSize > _mBlockSize) {
		requestSize = _mBlockSize;
	}
	size_t o = _mGetOff + requestSize;
	if (o > _mAreaSize) {
		o = _mAreaSize;
	}
	int32 req = o-_mGetOff;
	if (_mFlags & flagEndOfData) {
		int32 tg = _mPutOff-_mGetOff;
		if (tg < 0) tg += _mAreaSize;
		if (tg < req) {
			req = tg;
			if (req == 0) return 0;
			o = _mGetOff + req;
		}
	}
	status_t err = acquire_sem_etc(_mGetSem, req, B_TIMEOUT, timeout);
	if (err < B_OK) {
		if (((err == B_TIMED_OUT) || (err == B_BAD_SEM_ID)) && (_mFlags & flagEndOfData)) {
			int32 tg = _mPutOff-_mGetOff;
			if (tg < 0) tg += _mAreaSize;
			if (tg < req) {
				req = tg;
				if (req == 0) return 0;
				o = _mGetOff + req;
			}
			goto got_it;
		}
LEAVE_GET
		return err;
	}
got_it:
	*outData = _mBuffer + _mGetOff;
	if (o == _mAreaSize)
		_mPendingGet = 0;
	else
		_mPendingGet = o;
	atomic_or(&_mFlags, flagPendingGet);
	return req;
}

int32
BBlockFIFO::EndGet()
{
	if (!(atomic_and(&_mFlags, ~flagPendingGet) & flagPendingGet))
		return B_ERROR;
	int32 o = _mPendingGet - _mGetOff;
	_mGetOff = _mPendingGet;
	if (o < 0) o += _mAreaSize;
	//	part of buffer is now free to put into again
	status_t err = release_sem_etc(_mPutSem, o, B_DO_NOT_RESCHEDULE);
LEAVE_GET
	return err;
}

int32 
BBlockFIFO::BeginPut(void **outData, size_t requestSize, bigtime_t timeout)
{
	if (!outData) { FPRINTF((stderr, "BAD_VALUE: outData == NULL\n")); return B_BAD_VALUE; }
	if (_mFlags & flagEndOfData) { FPRINTF((stderr, "EPERM: end of data\n")); return EPERM; }
ENTER_GET
	if (requestSize > _mBufferSize) {
		requestSize = _mBufferSize;
	}
	ssize_t o = _mPutOff + requestSize;
	if (o > (ssize_t)_mAreaSize) {
		o = _mAreaSize;
	}
	int32 req = o-_mPutOff;
	status_t err = acquire_sem_etc(_mPutSem, req, B_TIMEOUT, timeout);
	if (err < B_OK) {
LEAVE_PUT
		FPRINTF((stderr, "BeginPut: acquire_sem_etc() returns %ld (req is %ld)\n", err, req));
		return err;
	}
	*outData = _mBuffer + _mPutOff;
	if (o == (ssize_t)_mAreaSize)
		_mPendingPut = 0;
	else
		_mPendingPut = o;
	atomic_or(&_mFlags, flagPendingPut);
	return req;
}

int32 
BBlockFIFO::EndPut(bool atEndOfData)
{
	if (!(atomic_and(&_mFlags, ~flagPendingPut) & flagPendingPut))
		return B_ERROR;
	int32 o = _mPendingPut - _mPutOff;
	_mPutOff = _mPendingPut;
	if (o < 0) o += _mAreaSize;
	//	part of buffer is now full to get from again
	status_t err = release_sem_etc(_mGetSem, o, B_DO_NOT_RESCHEDULE);
	if (atEndOfData) {
		atomic_or(&_mFlags, flagEndOfData);
		delete_sem(_mGetSem);
		_mGetSem = -1;
	}
LEAVE_PUT
	return err;
}

int32 
BBlockFIFO::CopyNextBlockOut(void *destination, size_t requestSize, bigtime_t timeout)
{
	if (destination == 0) return B_BAD_VALUE;
	if (requestSize == 0) return 0;
	char * d = (char *)destination;
	ssize_t total = 0;
	while (requestSize > 0) {
		const void * ptr;
		ssize_t got = BeginGet(&ptr, requestSize, timeout);
		if (got < 0) { FPRINTF((stderr, "BeginGet returns %ld\n", got)); return (total > 0 ? total : got); }
		requestSize -= got;
		memcpy(d, ptr, got);
		(void)EndGet();
		d += got;
		total += got;
	}
	return total;
}

int32 
BBlockFIFO::CopyNextBufferIn(const void *source, size_t requestSize, bigtime_t timeout, bool atEndOfData)
{
	if (source == 0) { FPRINTF((stderr, "BAD_VALUE: source == NULL\n")); return B_BAD_VALUE; }
	if (requestSize == 0) {
		if (atEndOfData) {
ENTER_PUT
			atomic_or(&_mFlags, flagEndOfData);
			delete_sem(_mGetSem);
			_mGetSem = -1;
LEAVE_PUT
		}
		return 0;
	}
	char * s = (char *)source;
	int32 total = 0;
	while (requestSize > 0) {
		void * ptr;
		int got = BeginPut(&ptr, requestSize, timeout);
		if (got < 0) return (total > 0 ? total : got);
		requestSize -= got;
		memcpy(ptr, s, got);
		s += got;
		total += got;
		(void)EndPut((requestSize == 0) ? atEndOfData : false);
	}
	return total;
}

