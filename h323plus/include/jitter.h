/*
 * jitter.h
 *
 * Jitter buffer support
 *
 * Open H323 Library
 *
 * Copyright (c) 1999-2000 Equivalence Pty. Ltd.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions of this code were written with the assisance of funding from
 * Vovida Networks, Inc. http://www.vovida.com.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: jitter.h,v $
 * Revision 1.1  2007/08/06 20:50:50  shorne
 * First commit of h323plus
 *
 * Revision 1.15  2006/01/18 07:46:08  csoutheren
 * Initial version of RTP aggregation (disabled by default)
 *
 * Revision 1.14  2005/11/30 13:05:01  csoutheren
 * Changed tags for Doxygen
 *
 * Revision 1.13  2003/10/28 22:38:31  dereksmithies
 * Rework of jitter buffer. Many thanks to Henry Harrison of Alice Street.
 *
 * Revision 1.12ACC1.0 6th October 2003 henryh
 * Complete change to adaptive algorithm 
 *
 * Revision 1.12  2002/10/31 00:32:39  robertj
 * Enhanced jitter buffer system so operates dynamically between minimum and
 *   maximum values. Altered API to assure app writers note the change!
 *
 * Revision 1.11  2002/09/16 01:14:15  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.10  2002/09/03 05:40:18  robertj
 * Normalised the multi-include header prevention ifdef/define symbol.
 * Added buffer reset on excess buffer overruns.
 * Added ability to get buffer overruns for statistics display.
 *
 * Revision 1.9  2002/08/05 10:03:47  robertj
 * Cosmetic changes to normalise the usage of pragma interface/implementation.
 *
 * Revision 1.8  2001/09/11 00:21:21  robertj
 * Fixed missing stack sizes in endpoint for cleaner thread and jitter thread.
 *
 * Revision 1.7  2001/02/09 05:16:24  robertj
 * Added #pragma interface for GNU C++.
 *
 * Revision 1.6  2000/05/25 02:26:12  robertj
 * Added ignore of marker bits on broken clients that sets it on every RTP packet.
 *
 * Revision 1.5  2000/05/04 11:49:21  robertj
 * Added Packets Too Late statistics, requiring major rearrangement of jitter buffer code.
 *
 * Revision 1.4  2000/05/02 04:32:24  robertj
 * Fixed copyright notice comment.
 *
 * Revision 1.3  2000/04/30 03:56:14  robertj
 * More instrumentation to analyse jitter buffer operation.
 *
 * Revision 1.2  2000/03/20 20:51:13  robertj
 * Fixed possible buffer overrun problem in RTP_DataFrames
 *
 * Revision 1.1  1999/12/23 23:02:35  robertj
 * File reorganision for separating RTP from H.323 and creation of LID for VPB support.
 *
 */

#ifndef __OPAL_JITTER_H
#define __OPAL_JITTER_H

#ifdef P_USE_PRAGMA
#pragma interface
#endif


#include "rtp.h"

class RTP_JitterBufferAnalyser;
class RTP_AggregatedHandle;

///////////////////////////////////////////////////////////////////////////////

class RTP_JitterBuffer : public PObject
{
  PCLASSINFO(RTP_JitterBuffer, PObject);

  public:
    friend class RTP_AggregatedHandle;

    RTP_JitterBuffer(
      RTP_Session & session,   ///<  Associated RTP session tor ead data from
      unsigned minJitterDelay, ///<  Minimum delay in RTP timestamp units
      unsigned maxJitterDelay, ///<  Maximum delay in RTP timestamp units
      PINDEX stackSize = 30000 ///<  Stack size for jitter thread
    );
    ~RTP_JitterBuffer();

//    PINDEX GetSize() const { return bufferSize; }
    /**Set the maximum delay the jitter buffer will operate to.
      */
    void SetDelay(
      unsigned minJitterDelay, ///<  Minimum delay in RTP timestamp units
      unsigned maxJitterDelay  ///<  Maximum delay in RTP timestamp units
    );

    void UseImmediateReduction(BOOL state) { doJitterReductionImmediately = state; }

    /**Read a data frame from the RTP channel.
       Any control frames received are dispatched to callbacks and are not
       returned by this function. It will block until a data frame is
       available or an error occurs.
      */
    virtual BOOL ReadData(
      DWORD timestamp,        ///<  Timestamp to read from buffer.
      RTP_DataFrame & frame   ///<  Frame read from the RTP session
    );

    /**Get current delay for jitter buffer.
      */
    DWORD GetJitterTime() const { return currentJitterTime; }

    /**Get total number received packets too late to go into jitter buffer.
      */
    DWORD GetPacketsTooLate() const { return packetsTooLate; }

    /**Get total number received packets that overran the jitter buffer.
      */
    DWORD GetBufferOverruns() const { return bufferOverruns; }

    /**Get maximum consecutive marker bits before buffer starts to ignore them.
      */
    DWORD GetMaxConsecutiveMarkerBits() const { return maxConsecutiveMarkerBits; }

    /**Set maximum consecutive marker bits before buffer starts to ignore them.
      */
    void SetMaxConsecutiveMarkerBits(DWORD max) { maxConsecutiveMarkerBits = max; }

    /**Start seperate jitter thread
      */
    void Resume(
#ifdef H323_RTP_AGGREGATE
      PHandleAggregator * aggregator
#endif
      );

    PDECLARE_NOTIFIER(PThread, RTP_JitterBuffer, JitterThreadMain);

  protected:
    //virtual void Main();

    class Entry : public RTP_DataFrame
    {
      public:
        Entry * next;
        Entry * prev;
        PTimeInterval tick;
    };

    RTP_Session & session;
    PINDEX        bufferSize;
    DWORD         minJitterTime;
    DWORD         maxJitterTime;
    DWORD         maxConsecutiveMarkerBits;

    unsigned currentDepth;
    DWORD    currentJitterTime;
    DWORD    packetsTooLate;
    unsigned bufferOverruns;
    unsigned consecutiveBufferOverruns;
    DWORD    consecutiveMarkerBits;
    PTimeInterval    consecutiveEarlyPacketStartTime;
    DWORD    lastWriteTimestamp;
    PTimeInterval lastWriteTick;
    DWORD    jitterCalc;
    DWORD    targetJitterTime;
    unsigned jitterCalcPacketCount;
    BOOL     doJitterReductionImmediately;
    BOOL     doneFreeTrash;

    Entry * oldestFrame;
    Entry * newestFrame;
    Entry * freeFrames;
    Entry * currentWriteFrame;

    PMutex bufferMutex;
    BOOL   shuttingDown;
    BOOL   preBuffering;
    BOOL   doneFirstWrite;

    RTP_JitterBufferAnalyser * analyser;

    PThread * jitterThread;
    PINDEX    jitterStackSize;

#ifdef H323_RTP_AGGREGATE
    RTP_AggregatedHandle * aggregratedHandle;
#endif

    BOOL Init(Entry * & currentReadFrame, BOOL & markerWarning);
    BOOL PreRead(Entry * & currentReadFrame, BOOL & markerWarning);
    BOOL OnRead(Entry * & currentReadFrame, BOOL & markerWarning, BOOL loop);
    void DeInit(Entry * & currentReadFrame, BOOL & markerWarning);
};

#endif // __OPAL_JITTER_H


/////////////////////////////////////////////////////////////////////////////
