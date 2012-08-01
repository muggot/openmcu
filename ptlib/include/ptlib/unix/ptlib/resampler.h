/*  pwlib/src/ptlib/unix/beaudio/Resampler.h

    $Log: resampler.h,v $
    Revision 1.2  2005/11/30 12:47:38  csoutheren
    Removed tabs, reformatted some code, and changed tags for Doxygen

    Revision 1.1  2004/05/14 08:37:33  ykiryanov
    BeOS resampler related declarations

    Revision 1.4  2002/02/04 03:15:47  yurik
    VC compiler complained of invoking new on array with paramters in constructor. Fixed it.

    Revision 1.3  2001/07/09 06:16:15  yurik
    Jac Goudsmit's BeOS changes of July,6th. Cleaning up media subsystem etc.

    Copyright 2001, Be Incorporated.   All Rights Reserved.
    This file may be used under the terms of the Be Sample Code License.
*/

#include <stdlib.h>

template <class T>
class History
{
  // This class stores a number of input samples in a circular buffer
  // The data is actually stored twice so that a wrap-around is never
  // needed for retrieving the data (it IS needed for storing it, but
  // because most resampling algorithms use interpolation of a number
  // of input samples, retrieving happens much more frequent than
  // storing).
  // The history is always filled completely: the start of the history
  // is padded with neutral samples (e.g. 0 for signed integer samples).
  // Therefore we only need one index into the array: a pointer to
  // the oldest sample (we keep a couple of additional pointers for
  // efficiency).
  // For example: let's say the object is constructed with size=3
  // and samples A B C D E are added. The internal representation and the
  // way that the retrieve function sees it are as follows:
  // Internal ('>' is head):    retrieved data
  // >0 0 0 0 0 0          {0,0,0}
  //  A>0 0 A 0 0          {0,0,A}
  //  A B>0 A B 0          {0,A,B}
  // >A B C A B C          {A,B,C}
  //  D>B C D B C          {B,C,D}
  //  D E>C D E C          {C,D,E}
  // Notice that the last element in the buffer is never retrieved, so
  // we could make the buffer one element smaller, but then we would
  // have to add another "if" to the storage code to not store the second 
  // copy of the data when the head pointer reaches the end of the buffer.
  // Note: this class is not multi-thread compatible.
  private:
    size_t  mSize;    // number of unique copies of each sample in buffer
    T      *mBuf;     // twice the size indicated in mSize
    T      *mHead;    // pointer to oldest sample
    T      *mLoop;    // kept equal to mBuf+mSize
    T      *mSecHead; // kept equal to mHead+mSize

  public:
    History() {}; // default constructor to please vc's new for arrays. 

    void Init(size_t size, T neutralvalue) // init with original constructor values
    {
      // The data is stored twice, so create a buffer that's double the requested size
      mBuf=mHead=mSecHead=mLoop=new T[2*(mSize=size)];

      // Fill the buffer with empty samples
      for (unsigned i=0; i<mSize; i++) {
        *mSecHead++=neutralvalue;
      }
      mLoop=mSecHead;
    }

    History(size_t size, T neutralvalue) // original constructor
    {
      Init(size, neutralvalue);
    }

    ~History() { delete[] mBuf; }

    // Write one sample into the history
    void Write(T sample)
    {
      // replace oldest sample with this sample
      *mHead++ = *mSecHead++ = sample;
      // now next sample is the oldest
      if (mHead==mLoop) {
        mHead=mBuf;
        mSecHead=mLoop;
      }
    }

    // Read samples from the history
    T operator[](unsigned index) { return mHead[index]; }

    // Additional utilities
    size_t Size() const { return mSize; }
};

template <class ISample, class IntSample, class OSample>
class BaseResampler
{
  // - A "correspondance" is defined as the point in time when
  //   an input sample's time corresponds with its output sample's time.
  // - A "block" is defined as the time that passes between two correspondances
  // - By mathematical rule, if input frequency and output frequency are
  //   expressed in samples per second, the number of correspondances per
  //   second is the Highest Common Factor (HCF) of the two frequencies.
  // - The resolution at which time needs to be measured within a block is 
  //   defined as a "tick". The number of ticks per block is the result of
  //   multiplying the number of input samples per block and the number of
  //   output samples per block.
  protected:
    ////////// Types
    typedef History<IntSample> SampleHistory;

    ////////// Configuration
    // This data doesn't change after setup
    // The number of ticks per block is mInum*mOnum.
    // Therefore the number of ticks per input sample is (mInum*mOnum)/mInum=mOnum. The 
    // number of ticks per output sample is (mInum*mOnum)/mOnum=mInum.
    //  float     mIFreq;      // input  frequency in frames/second
    //  float     mOFreq;      // output frequency in frames/second
    unsigned      mIChannels;  // number of input  channels
    unsigned      mOChannels;  // number of output channels
    //  unsigned  mHCF;        // Highest Common Factor of mIFreq/mOFreq
    unsigned      mINum;       // number of input  samples per block = mIFreq/mHCF
    unsigned      mONum;       // number of output samples per block = mOFreq/mHCF

#define mITPS mONum  // input ticks per sample
#define mOTPS mINum  // output ticks per sample

    IntSample mNeutralValue;  // neutral value

    ////////// Status
    // This data changes with each input sample
    unsigned  mICount;  // input  samples received  since last coinciding
    unsigned  mOCount;  // output samples generated since last coinciding
    unsigned  mITicks;  // tick count for mICount (=mICount*mITPS)
    unsigned  mOTicks;  // tick count for mOCount (=mOCount*mOTPS)
    SampleHistory  *mHistory;  // one sample history per channel

    static size_t hcf(
      size_t a, 
      size_t b)
    {
      // The Highest Common Factor (HCF) is defined as the largest number that
      // two positive integer values can both be divided by without leaving
      // a remainder.
      // For example: HCF(12,21)=3, HCF(12,23)=1, HCF(12,24)=12.
      // This function uses Euclid's algorithm to determine HCF(a,b)
      // The result is only valid when a and b are both positive.
      size_t r;

      // the algorithm only works for a>=b
      if (a<b) {
        // swap a and b
        r=a; a=b; b=r;
      }

      while ((r=a%b)!=0) {
        a=b;
        b=r;
      }

      return b;
    }

  public:
    // Constructor/Destructor
    BaseResampler(
      float infreq,
      float outfreq,
      unsigned inchannels,
      unsigned outchannels,
      ISample neutralvalue,
      size_t historysize=1) // must be >=1
    {
      // Configuration (parameterized part)
      unsigned IFreq=static_cast<unsigned>(infreq);
      unsigned OFreq=static_cast<unsigned>(outfreq);
      mIChannels=inchannels;
      mOChannels=outchannels;
      mNeutralValue=ToIntSample(neutralvalue);

      // Configuration (derived part)
      unsigned HCF=hcf(IFreq, OFreq);
      mINum=IFreq/HCF;
      mONum=OFreq/HCF;

      // Status
      mICount=0; //mICount=(((delay+mINum-1)/mINum)*mINum-delay);
      mITicks=0; //mITicks=mICount*mITPS;
      mOCount=0;
      mOTicks=0;
      mHistory = new SampleHistory[mIChannels];
      for (unsigned u = 0; u < mIChannels; u++) 
        mHistory[u].Init(historysize,mNeutralValue);
    }

  public:
    virtual ~BaseResampler()
    {
      delete[] mHistory;
    }

  public:
    size_t InFrameSize()
    {
      return mIChannels*sizeof(ISample);
    }
    size_t OutFrameSize()
    {
      return mOChannels*sizeof(OSample);
    }
    size_t InBlockSize()
    {
      return mINum*InFrameSize();
    }
    size_t OutBlockSize()
    {
      return mONum*OutFrameSize();
    }
    size_t InFramesFromOutBytes(size_t outbytes)
    {
      return outbytes*mINum/(mONum*OutFrameSize());
    }
    size_t OutFramesFromInBytes(size_t inbytes)
    {
      return inbytes*mONum/(mINum*InFrameSize());
    }
    size_t InBytesFromOutBytes(size_t outbytes)
    {
      return InFramesFromOutBytes(outbytes)*InFrameSize();
    }
    size_t OutBytesFromInBytes(size_t inbytes)
    {
      return OutFramesFromInBytes(inbytes)*OutFrameSize();
    }

  protected:
    // Virtual function to convert sample from input format to internal format.
    // The default implementation is a cast, but if the signed-ness of the formats are
    // different, you will need to override this to add or subtract the neutral value
    // and if the ranges of values don't match you will need to add a multiplication or
    // division.
    virtual IntSample ToIntSample(ISample value) { return static_cast<IntSample>(value); }

    // Virtual function to convert sample from internal format to output format.
    // The default implementation is a cast, but if the signed-ness of the formats are
    // different, you will need to override this to add or subtract the neutral value
    // and if the ranges of values don't match you will need to add a multiplication or
    // division.
    virtual OSample ToOSample(IntSample value) { return static_cast<OSample>(value); }

    // Pure virtual function that does the actual resampling
    // It should use the sample history to generate a sample.
    virtual IntSample OutSample(SampleHistory &history)
    {
      IntSample y0=history[0];
      IntSample y1=history[1];

      // Note: it's possible to optimize the division in the next line
      // by creating a table in the constructor with mITPS entries
      // where table[n]=n/mITPS and changing the calculation to 
      // y0+(y1-y0)*table[deltaticks]
      return y0+(IntSample)(mOTicks-mITicks)*(y1-y0)/(IntSample)mITPS;
    }

    // Virtual function to resample a frame.
    // It needs to examine the histories for all channels and generate an
    // output frame at the given pointer, and then update the pointer.
    virtual void OutFrame(OSample **io_out)
    {
      unsigned i,j;
      for (i=0, j=0; i<mIChannels && j<mOChannels; i++, j++)
      {
        **io_out=ToOSample(OutSample(mHistory[i]));
        (*io_out)++;
      }
    }

  public:
    size_t InFrames(
            const ISample **io_indata, 
            OSample **io_outdata,
            size_t *numinframes,
            size_t maxoutframes)
    {
      // Handles one input sample
      // Generates a number of outputs samples depending on the current state
      // Updates the input and output pointers.
      // Returns number of output samples done

      size_t out_done=0;
      size_t inframe;

      for (inframe=0; inframe<*numinframes && out_done!=maxoutframes; inframe++)
      {
        // Add new samples to the history
        for (unsigned channel=0; channel<mIChannels; channel++)
        {
          mHistory[channel].Write(ToIntSample(**io_indata));
          (*io_indata)++;
        }

        // Determine upper limit of output tick counter
        unsigned endticks=mITicks;
        if (mOTicks!=0)
        {
          // Keep generating output samples until the output
          // tick count ends up at the input tick count.
          // For the beginning of the block, that's 0 (inclusive)
          // For other output samples, it's mITicks+mITPS (exclusive).
          // To make the loop inclusive ('<=') in all cases, we use -1
          // which is allowed because the tick count is an integer.
          endticks+=mITPS-1;
        }

        // Generate the necessary output samples
        while (mOTicks<=endticks)
        {
          OutFrame(io_outdata);
          out_done++;

          // Update the output side of the state
          mOCount++;
          mOTicks+=mOTPS;

          // If we're out of space, bail out
          if (out_done==maxoutframes)
          {
            break;
          }
        }

        // update the input side of the state
        mICount++;
        mITicks+=mITPS;

        // Reset at the end of a block
        if (mICount==mINum)
        {
          mICount=0;
          mITicks=0;
          mOCount=0;
          mOTicks=0;
        }
      }

      *numinframes-=inframe;

      return out_done;
    }
};

typedef class BaseResampler<short, long, short> Resampler;
