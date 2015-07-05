
#include "precompile.h"

#ifndef _MCU_RTP_CACHE_H
#define _MCU_RTP_CACHE_H

#include "utils.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

#define FRAME_MASK	0xFFFFFF00
#define FRAME_BUF_SIZE	0x2000
#define FRAME_OFFSET	0x100

// cacheRTPListMutex - используется при создании кэшей
// предотвращает создание в списке двух одноименных кэшей
extern PMutex cacheRTPListMutex;

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpenAudioCache(const PString & room, const OpalMediaFormat & format, const PString & cacheName);
BOOL OpenVideoCache(const PString & room, const OpalMediaFormat & format, const PString & cacheName);

class CacheRTP;
CacheRTP * CreateCacheRTP(const PString & key);
void DeleteCacheRTP(CacheRTP *& cache);
bool FindCacheRTP(const PString & key);
void PutCacheRTP(CacheRTP *& cache, RTP_DataFrame & frame, unsigned int len, unsigned int flags);
bool GetCacheRTP(CacheRTP *& cache, RTP_DataFrame & frame, unsigned & toLen, unsigned & seqN, unsigned & flags);
bool AttachCacheRTP(CacheRTP *& cache, const PString & key, unsigned & encoderSeqN);
void DetachCacheRTP(CacheRTP *& cache);

////////////////////////////////////////////////////////////////////////////////////////////////////

class CacheRTP
{
  public:
    CacheRTP(long _id, const PString & _name)
    {
      id = _id;
      name = _name;
      seqN = FRAME_BUF_SIZE;
      lastN = 0;
      iframeN = 0;
      uN = 0;
      fastUpdate = false;
    }

   ~CacheRTP()
    {
      for(CacheRTPUnitMap::iterator r = unitList.begin(); r != unitList.end(); )
      {
        CacheRTPUnit *unit = r->second;
        unitList.erase(r++);
        if(unit)
          delete unit;
      }
    }

    long GetID() const
    { return id; }

    const PString & GetName() const
    { return name; }

    void OnFastUpdatePicture()
    { fastUpdate = true; }

    bool GetMarker (unsigned char *pkt)
    { return (pkt[1] & 0x80); }

    void IncrementUsersNumber()
    { uN++; }

    void DecrementUsersNumber()
    { uN--; }

    unsigned GetUsersNumber() const
    { return uN; }

    void GetFastUpdate(unsigned & flags)
    {
      if(!fastUpdate)
        return;
      if(seqN - iframeN <= FRAME_OFFSET * 10)
        return;
      MCUTRACE(1, "CacheRTP " << name << " FastUpdate needed");
      flags |= PluginCodec_CoderForceIFrame;
      fastUpdate = false;
    }

    unsigned int GetLastFrameNum()
    {
      CacheRTPUnitMap::reverse_iterator r = unitList.rbegin();
      if(r != unitList.rend())
        return r->first;
      return 0;
    }

    void PutFrame(RTP_DataFrame & frame, unsigned len, unsigned flags)
    {
      CacheRTPUnit *unit;
      unsigned int l;
      //MCUTRACE(6, "CacheRTP " << name << " seqN/lastN " << seqN << "/" << lastN);
      CacheRTPUnitMap::iterator r = unitList.find(lastN);
      while(r == unitList.end() && seqN - lastN >= FRAME_BUF_SIZE)
      {
        lastN = (lastN & FRAME_MASK) + FRAME_OFFSET;
        r = unitList.find(lastN);
      };

      if(r == unitList.end())
        unit = new CacheRTPUnit();
      else
      { unit = r->second; unitList.erase(r); lastN++; }
      unit->PutFrame(frame);
      unit->len = len;
      unit->lock = 0;
      unitList.insert(CacheRTPUnitMap::value_type(seqN, unit));
      //MCUTRACE(6, "CacheRTP " << name << " put frame " << seqN);
      if(flags & PluginCodec_ReturnCoderIFrame && seqN > (iframeN & FRAME_MASK) + FRAME_OFFSET)
      {
        iframeN = seqN;
        MCUTRACE(6, "CacheRTP " << name << " new iframe " << iframeN);
      }
      if(GetMarker(frame.GetPointer()))
      {
	seqN = (seqN & FRAME_MASK) + FRAME_OFFSET;
        l = seqN - FRAME_BUF_SIZE / 2;
	do // lock all frame
        {
	  r = unitList.find(l);
	  l++;
          if(r != unitList.end())
            r->second->lock = 1;
	}
        while(r != unitList.end());
      }
      else
        seqN++;
    }

    void GetFrame(RTP_DataFrame & frame, unsigned & toLen, unsigned & num, unsigned & flags)
    {
      while(num >= seqN)
        MCUTime::Sleep(10);
      CacheRTPUnitMap::iterator r = unitList.find(num);
      int i=0;
      while((r == unitList.end() || r->second->lock) && num < seqN)
      { // for debug
        PTRACE_IF(3, i > 0, "H323READ\t Lost Packet " << i << " " << num);
        PTRACE_IF(3, (r != unitList.end() && r->second->lock), "H323READ\t Lost Packet " << i << " " << num << " " << r->second->lock);
        num = (num & FRAME_MASK) + FRAME_OFFSET; // may be lost frames, fix it
        r = unitList.find(num);
        i++;
      }
      while(r == unitList.end())
      {
        MCUTime::Sleep(10);
        r = unitList.find(num);
      }
      r->second->GetFrame(frame);
      toLen = r->second->len;
      flags = 0;
      if(GetMarker(frame.GetPointer()))
        flags |= PluginCodec_ReturnCoderLastFrame;
      if(num == iframeN)
        flags |= PluginCodec_ReturnCoderIFrame;
      num++;
    }

  private:

    long id;
    PString name;
    unsigned seqN;
    unsigned lastN;
    unsigned iframeN;
    bool fastUpdate;
    unsigned uN;

    class CacheRTPUnit
    {
        friend class CacheRTP;

        CacheRTPUnit() {};
        ~CacheRTPUnit() {};

        void PutFrame(RTP_DataFrame &srcFrame)
        {
          int sz = srcFrame.GetHeaderSize()+srcFrame.GetPayloadSize();
          frame.SetMinSize(sz);
          memcpy(frame.GetPointer(), srcFrame.GetPointer(), sz);
          frame.SetPayloadSize(srcFrame.GetPayloadSize());
        }

        void GetFrame(RTP_DataFrame &dstFrame)
        {
          int sz = frame.GetHeaderSize() + frame.GetPayloadSize();
          dstFrame.SetMinSize(sz);
          memcpy(dstFrame.GetPointer(), frame.GetPointer(), sz);
          dstFrame.SetPayloadSize(frame.GetPayloadSize());
        }

        int lock;
        unsigned int len;
        RTP_DataFrame frame;
    };
    typedef std::map<unsigned int, CacheRTPUnit *> CacheRTPUnitMap;
    CacheRTPUnitMap unitList;
};

extern MCUCacheRTPList cacheRTPList;

////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // _MCU_RTP_CACHE_H
