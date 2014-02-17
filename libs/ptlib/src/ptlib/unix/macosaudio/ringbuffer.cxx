#include <assert.h>
#include <stdlib.h>
#include <new> // so I can throw std::bad_alloc
#include <string.h>
#include "ringbuffer.h"

class AutoCriticalRegion 
{
    MPCriticalRegionID m_mutex;
public:
    AutoCriticalRegion(MPCriticalRegionID mutex)
            : m_mutex(mutex)
        {
            MPEnterCriticalRegion(m_mutex, kDurationForever);
        }
    ~AutoCriticalRegion() 
        {
            MPExitCriticalRegion(m_mutex);
        }
};

JRingBuffer::JRingBuffer(int bs)
        : m_in(0), m_out(-1), m_buffersize(bs), m_shutdown(0), m_reserved(0),
          m_mutex(0), m_readtoken(0)
{
    if ((m_buffer = new unsigned char[m_buffersize]) == 0) {
        throw std::bad_alloc();
        return;
    }
    m_bufferend = m_buffer + m_buffersize;
    if (MPCreateCriticalRegion(&m_mutex) == noErr) {
        if (MPCreateBinarySemaphore(&m_readtoken) == noErr) {
            return;
        }
        MPDeleteCriticalRegion(m_mutex);
        m_mutex = 0;
    }
    // fall through here on failures
    delete[] m_buffer;
    m_buffer = 0;
    throw std::bad_alloc();
}

// make sure no one is using it when it is destructed!
JRingBuffer::~JRingBuffer() {
    if (m_buffer) {
        delete[] m_buffer;
        m_buffer = 0;
    }
    if (m_mutex != kInvalidID)
        MPDeleteCriticalRegion(m_mutex);
    if (m_readtoken != kInvalidID)
        MPDeleteSemaphore(m_readtoken);
}

// returns 0 on success, -1 on failure
int JRingBuffer::Write(void *buf, int len)
{
    int wasempty = 0, overfill = 0;
    if (len == 0) return 0;
    // It is a programming error to stuff more than buffersize in at once.
    assert(len <= m_buffersize);

    AutoCriticalRegion mutex(m_mutex);

    if (len >= m_buffersize) {
        len = m_buffersize;
        // a quickie optimization: if we are filling the buffer, just start
        // from the beginning
        m_in = 0;
        m_out = -1;
    }
    wasempty = IsEmpty();
    overfill = (CanHold() < len);

    // OK, are we going to wrap?
    if ((m_buffersize - m_in) <= len) {
        // yes, copy part and wrap
        int part = m_buffersize - m_in;
        memmove(m_buffer + m_in, buf, part);
        m_in = 0;
        buf = (void *)((char *)buf + part);
        len -= part;
    }
    // not wrapping (anymore)
    if (len)
        memmove(m_buffer + m_in, buf, len);
    m_in += len;
    if (overfill)
        m_out = m_in;
    // if empty, signal the CV, we just dumped in some data
    if (wasempty) {
        m_out = 0;
        MPSignalSemaphore(m_readtoken);
    }
    return 0;
}

// Get a pointer to a reserved buffer
void *JRingBuffer::ReserveToWrite(int &len)
{
    if (len <= 0) return NULL;
    // It is a programming error to stuff more than buffersize in at once.
    assert(len <= m_buffersize);
    if (len > m_buffersize) len = m_buffersize;

    MPEnterCriticalRegion(m_mutex, kDurationForever);

    return ReserveToWrite_locked(len);
}

// Call with the mutex LOCKED.
void *JRingBuffer::ReserveToWrite_locked(int &len)
{
    // OK, are we going to wrap?
    if ((m_buffersize - m_in) <= len) {
        len = m_buffersize - m_in;
    }
    m_reserved = len;
    return &m_buffer[m_in];
    // MUTEX IS STILL LOCKED.
}

int JRingBuffer::CommitFinal(int wrotelen)
{
    int wasempty = IsEmpty(), overfill = CanHold() < wrotelen;
    assert(m_reserved);
    assert(wrotelen >= 0);
    assert(m_reserved >= wrotelen);
    if (wrotelen > 0) {
        m_in += wrotelen;
        if (m_in == m_buffersize) m_in = 0;
        if (overfill)
            m_out = m_in;
        if (wasempty) {
            m_out = 0;
            MPSignalSemaphore(m_readtoken);
        }
    }
    MPExitCriticalRegion(m_mutex);
    return 0;
}

void *JRingBuffer::CommitMore(int wrotelen, int &morelen)
{
    int wasempty = IsEmpty(), overfill = CanHold() < wrotelen;
    assert(m_reserved);
    assert(wrotelen >= 0);
    assert(m_reserved >= wrotelen);
    if (wrotelen != 0) {
        m_in += wrotelen;
        if (m_in == m_buffersize) m_in = 0;
        if (overfill)
            m_out = m_in;
        if (wasempty) {
            m_out = 0;
            MPSignalSemaphore(m_readtoken);
            // Interesting question:  should we drop the mutex and relock?
            // I suspect not, since the writer is probably a time-critical
            // callback.
        }
    }
    return ReserveToWrite_locked(morelen);
}

int JRingBuffer::ReadTimed(void *buf, int len, long timeout, int flag)
{
    OSStatus oops;
    AbsoluteTime expiry;
    if (flag == kAsync) timeout = 0;

    expiry = AddDurationToAbsolute((Duration)timeout, UpTime());
    
    int total_read = 0;
#ifndef NDEBUG // this is used only in an assert
    int origlen = len;
#endif
    
    while (len > 0) {
        // Obtain the read token
        // First, recalculate the timeout (we may have waited before)
        if (timeout != aLongTime) {
            timeout = AbsoluteToDuration(
                          SubAbsoluteFromAbsolute(expiry,
                                                  UpTime()));

        }
        if (IsShutdown()) {
            total_read = -1;
            break;
        }
        oops = MPWaitOnSemaphore(m_readtoken, (Duration)timeout);
    
        if (oops != noErr) {
            if (flag == kAsync)
                total_read = 0;
            else
                total_read = -1;
            break;
        }

        // lock the queue
        AutoCriticalRegion mutex(m_mutex);

        // While there is data and we need some...
        while (!IsShutdown() && !IsEmpty() && len > 0) {
            // copy out a contiguous chunk.
            int avail = Contains();
            // only read what's needed
            if (avail > len) avail = len;
            // now, are we wrapping?
            if (avail >= m_buffersize - m_out)
                avail = m_buffersize - m_out;
            memmove(buf, m_buffer + m_out, avail);
            buf = (void *)((char *)buf + avail);
            total_read += avail;
            len -= avail;
            m_out += avail;
            // Wrap?
            if (m_out == m_buffersize)
                m_out = 0;
            if (m_out == m_in) {
                m_out = -1; // we have emptied the buffer
                m_in  = 0;
            }
        }
        if (!IsEmpty() || IsShutdown()) {
            // regenerate the read token
            MPSignalSemaphore(m_readtoken);
        }
        if (IsShutdown()) {
            total_read = -1;
            break;
        }
        // we have read as much as we can or need.  Are we done?
        if (flag == kAsync || len == 0 || (flag == kSome && total_read != 0))
        {
            // yes.
            break;
        }
        // need more data.
        assert(IsEmpty());
        // loop again
    }
    assert(total_read < 0 || flag != kWait || total_read == origlen);
    
    return total_read;
}

bool JRingBuffer::WaitForData(long timeout)
{
    OSStatus err;
    err = MPWaitOnSemaphore(m_readtoken, timeout);
    if (err == 0) MPSignalSemaphore(m_readtoken);
    return err == 0;
}
            
void JRingBuffer::MakeEmpty() 
{
    MPEnterCriticalRegion(m_mutex, kDurationForever);
    m_in = 0;
    m_out = -1;
    MPExitCriticalRegion(m_mutex);
}

void JRingBuffer::Shutdown() 
{
    MPEnterCriticalRegion(m_mutex, kDurationForever);
    m_in = 0;
    m_out = -1;
    m_shutdown = 1;
    MPExitCriticalRegion(m_mutex);
    (void)MPSignalSemaphore(m_readtoken);
}

