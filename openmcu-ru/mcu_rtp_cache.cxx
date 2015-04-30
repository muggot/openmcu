
#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUCacheRTPList cacheRTPList;
PMutex cacheRTPListMutex;

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpenAudioCache(const PString & room, const OpalMediaFormat & format, const PString & cacheName)
{
  PWaitAndSignal m(cacheRTPListMutex);

  if(FindCacheRTP(cacheName))
    return TRUE;

  ConferenceManager *manager = OpenMCU::Current().GetConferenceManager();
  // creating conference if needed
  Conference *conference = manager->MakeConferenceWithLock(room);
  if(conference == NULL)
    return FALSE;

  PTRACE(2, "OpenAudioCache " << cacheName);

  ConferenceCacheMember *member = new ConferenceCacheMember(conference, 0, format, cacheName);
  conference->AddMember(member);

  // unlock conference
  conference->Unlock();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL OpenVideoCache(const PString & room, const OpalMediaFormat & format, const PString & cacheName)
{
  PWaitAndSignal m(cacheRTPListMutex);

  if(FindCacheRTP(cacheName))
    return TRUE;

  ConferenceManager *manager = OpenMCU::Current().GetConferenceManager();
  // creating conference if needed
  Conference *conference = manager->MakeConferenceWithLock(room);
  if(conference == NULL)
    return FALSE;

  // starting new cache thread
  unsigned videoMixerNumber = 0;
  PINDEX slashPos = cacheName.Find("/");
  if(slashPos != P_MAX_INDEX)
    videoMixerNumber = atoi(cacheName.Mid(slashPos+1, P_MAX_INDEX));

  PTRACE(2, "OpenVideoCache " << cacheName);

  ConferenceCacheMember *member = new ConferenceCacheMember(conference, videoMixerNumber, format, cacheName);
  conference->AddMember(member);

  // unlock conference
  conference->Unlock();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

CacheRTP * CreateCacheRTP(const PString & key)
{
  MCUCacheRTPList::shared_iterator it = cacheRTPList.Find(key);
  if(it == cacheRTPList.end())
  {
    long id = cacheRTPList.GetNextID();
    CacheRTP *cache = new CacheRTP(id, key);
    it = cacheRTPList.Insert(cache, id, key);
  }
  return it.GetCapturedObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void DeleteCacheRTP(CacheRTP *& cache)
{
  if(cache == NULL)
    return;
  PString key = cache->GetName();
  cacheRTPList.Release(cache->GetID());
  MCUCacheRTPList::shared_iterator it = cacheRTPList.Find(key);
  if(it != cacheRTPList.end())
  {
    MCUTRACE(1, "CacheRTP " << key << " Delete");
    cache = it.GetObject();
    if(cacheRTPList.Erase(it))
      delete cache;
  }
  cache = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool FindCacheRTP(const PString & key)
{
  MCUCacheRTPList::shared_iterator it = cacheRTPList.Find(key);
  if(it == cacheRTPList.end())
    return false;
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void PutCacheRTP(CacheRTP *& cache, RTP_DataFrame & frame, unsigned int len, unsigned int flags)
{
  if(cache == NULL)
  {
    MCUTRACE(1, "CacheRTP Put - No cache!");
    return;
  }
  //cout << "PutCacheRTP length=" << len << " marker=" << frame.GetMarker() << "\n";
  cache->PutFrame(frame, len, flags);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool GetCacheRTP(CacheRTP *& cache, RTP_DataFrame & frame, unsigned & toLen, unsigned & seqN, unsigned & flags)
{
  if(!cache)
  {
    MCUTRACE(1, "CacheRTP Get - No cache!");
    seqN = 0xFFFFFFFF;
    return false;
  }
  if(flags & PluginCodec_CoderForceIFrame)
    cache->OnFastUpdatePicture();
  cache->GetFrame(frame, toLen, seqN, flags);
  return true;
  //cout << "GetCacheRTP length=" << toLen << " marker=" << frame.GetMarker() << " flags=" << flags  << "\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool AttachCacheRTP(CacheRTP *& cache, const PString & key, unsigned & encoderSeqN)
{
  MCUCacheRTPList::shared_iterator it = cacheRTPList.Find(key);
  if(it == cacheRTPList.end())
    return false;
  MCUTRACE(1, "CacheRTP " << key << " Attach");
  cache = it.GetCapturedObject();
  cache->IncrementUsersNumber();
  encoderSeqN = cache->GetLastFrameNum();
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void DetachCacheRTP(CacheRTP *& cache)
{
  if(cache == NULL)
    return;
  MCUTRACE(1, "CacheRTP " << cache->GetName() << " Detach");
  cache->DecrementUsersNumber();
  cacheRTPList.Release(cache->GetID());
  cache = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
