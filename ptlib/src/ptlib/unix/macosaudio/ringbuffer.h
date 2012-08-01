#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__ 1

#include <CoreServices/CoreServices.h>

// just your standard ring-buffer class.  how many times can this be
// reinvented?  (well, this time, it uses Mac MPThread syncronization
// primitives)

// This ring buffer manages an array of characters whose size is specified
// in the constructor.  You supply data with Write, and remove data with
// Read; Read comes in 3 flavors -- block until enough data is supplied,
// block until any data is supplied, and return immediately if the ring is
// empty.  Old data is less valuable than new data, so adding data when
// the ring is full deletes old data.  This introduces a subtle irritation
// in the "block until enough data" call:  the vagaries of thread scheduling
// may result in the buffer not representing truly contiguous data.  Tough.

class JRingBuffer {
  public:
    JRingBuffer(int buffersize);
    ~JRingBuffer();
    enum { kAsync, kSome, kWait };
    enum { aLongTime = 0x7FFFFFFFL };
    int ReadTimed(void *buf, int len, long timeout, int flag = kWait);
    int ReadSomeTimed(void *buf, int len, long timeout) {
        return ReadTimed(buf, len, timeout, kSome);
    }
    int Read(void *buf, int len, int flag = kWait) {
        return ReadTimed(buf, len, aLongTime, flag);
    }
    int ReadSome(void *buf, int len) {
        return ReadTimed(buf, len, aLongTime, kSome);
    }
    int ReadAsync(void *buf, int len) {
        return ReadTimed(buf, len, aLongTime, kAsync);
    }
    int Write(void *buf, int len);

    // ReserveToWrite returns the address of a buffer of at most len bytes
    // into which data can be poured; if less than len bytes is available,
    // the amount actually reserved is returned.  (Generally this is because
    // of wrapping the buffer.)
    // ReserveToWrite returns with the internal lock HELD!
    // Once you have written your data, you can commit it via CommitReserved
    // or CommitMore (qv), or you can abandon the data with AbandonReserve.
    void *ReserveToWrite(int &len);
  private:
    void *ReserveToWrite_locked(int &len);
  public:
    
    // Use CommitMore to commit the data you wrote (wrotelen) and get a new
    // chunk of (at most) morelen.  The internal lock is still HELD.
    void *CommitMore(int wrotelen, int &morelen);
    // Use CommitFinal to commit the data and release the lock.
    int CommitFinal(int wrotelen);
    // To give up the lock without advancing the write pointer, AbandonReserve
    int AbandonReserve() { return CommitFinal(0); }

    void MakeEmpty();
    void SetError(int error);
    bool IsEmpty() const { return m_out == -1; }
    bool IsFull() const  { return m_in == m_out; }
    int  Contains() const {
        if (IsEmpty()) return 0;
        if (m_out < m_in) return m_in - m_out;
        return m_buffersize - (m_out - m_in);
    }
    int CanHold() const {
        if (IsEmpty()) return m_buffersize;
        if (m_out < m_in) return m_buffersize - (m_in - m_out);
        return m_out - m_in;
    }
    bool WaitForData(long timeout = 0x7FFFFFFF);

    bool AllocFailed() const { return m_buffer == 0; }

    void Shutdown();
    bool IsShutdown() const { return m_shutdown != 0; }

 private:
    unsigned char *m_buffer, *m_bufferend;
    // If the ring is empty, out == -1 and in is zero; otherwise in and out
    // are offsets, and the ring is chock full if in == out.
    int m_in, m_out, m_buffersize;
    int m_shutdown; // if further reads are forbidden
    // m_reserved is how much of the buffer has been reserved for write
    int m_reserved;
    // Locking regime:
    // Examine/modify the queue pointers with m_mutex held.  Alas, because
    // the writer is able to "consume" old data, I don't think the pointer
    // manipulation can be correctly done with atomic operations.
    // m_readtoken approximates a queue-not-empty semaphore, but accidentally
    // optimizes the behavior somewhat:  initially it is non-signalled,
    // the first reader will block before examining the queue.  The first
    // write will signal the semaphore, allowing readers to proceed.  Each
    // reader who waits on m_readtoken successfully consumes the read token;
    // if the reader does not consume all of the data in the queue, it will
    // regenerate the read token by signalling m_readtoken when it is done.
    // When no reader is present, m_readtoken acts like a flag indicating
    // data; but it also accidentally serializes readers, which might be
    // a benefit.  (If this were pthreads, I'd use a variable and a
    // Condition Variable, but MPThreads has no such construct.)
    // XXX
    // XXX This code is broken!  If the write thread is a cooperative thread,
    // XXX it is not supposed to block in MPThread calls!  That probably means
    // XXX I need to redesign the queueing protocol.  Crap.  Leave it for now.
    // XXX
    MPCriticalRegionID m_mutex;
    MPSemaphoreID m_readtoken;
};


#endif
