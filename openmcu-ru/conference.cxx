
#include "precompile.h"
#include "mcu.h"

////////////////////////////////////////////////////////////////////////////////////

// size of a PCM data packet, in samples
#define PCM_BUFFER_LEN_MS              480
#define PCM_BUFFER_MAX_READ_LEN_MS     96 // AC3 16000
#define PCM_BUFFER_MAX_WRITE_LEN_MS    40
#define PCM_BUFFER_LAG_MS              2

const static struct audio_resolution {
  int samplerate;
  int channels;
} audio_resolutions[] = {
  { 8000,  1 },
  { 16000, 1 },
  { 24000, 1 },
  { 32000, 1 },
  { 48000, 1 },
  { 48000, 2 },
  { 0 }
};

////////////////////////////////////////////////////////////////////////////////////

ConferenceManager::ConferenceManager()
{
  maxConferenceCount = 0;
  monitor  = new ConferenceMonitor(*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceManager::~ConferenceManager()
{
  // clear all conference and leave connections
  ClearConferenceList();

  monitor->running = FALSE;
  monitor->WaitForTermination();
  delete monitor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithLock(Conference * _conference)
{
  Conference *conference = conferenceList(_conference);
  return conference;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithLock(const OpalGloballyUniqueID & conferenceID)
{
  for(MCUConferenceList::shared_iterator it = conferenceList.begin(); it != conferenceList.end(); ++it)
  {
    Conference *conference = *it;
    if(conference->GetGUID() == conferenceID)
      return it.GetCapturedObject();
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithLock(const PString & room)
{
  Conference *conference = conferenceList(room);
  return conference;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithLock(long id)
{
  Conference *conference = conferenceList(id);
  return conference;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceManager::CheckJoinConference(const PString & room)
{
  Conference * conference = FindConferenceWithLock(room);
  if(conference)
  {
    conference->Unlock();
    return TRUE;
  }
  return GetConferenceParam(room, RoomAutoCreateWhenConnectingKey, TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::MakeConferenceWithLock(const PString & room, PString name, BOOL ignoreRestriction)
{
  PWaitAndSignal m(conferenceListMutex);
  MCUConferenceList::shared_iterator it = conferenceList.Find(room);
  if(it == conferenceList.end())
  {
    if(room.Find(MCU_INTERNAL_CALL_PREFIX) == 0)
      ignoreRestriction = TRUE;
    if(ignoreRestriction == FALSE && GetConferenceParam(room, RoomAutoCreateWhenConnectingKey, TRUE) == FALSE)
    {
      PTRACE(1, "error");
      return NULL;
    }
    // create the conference
    long id = conferenceList.GetNextID();
    OpalGloballyUniqueID conferenceID;
    Conference *conference = CreateConference(id, conferenceID, room, name);
    it = conferenceList.Insert(conference, id, room);
    //
    OnCreateConference(conference);
    // set the conference count
    maxConferenceCount = PMAX(maxConferenceCount, conferenceList.GetSize());
  }
  return it.GetCapturedObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceManager::HasConference(const PString & number, OpalGloballyUniqueID & conferenceID)
{
  Conference *conference = FindConferenceWithLock(number);
  if(conference)
  {
    conferenceID = conference->GetGUID();
    conference->Unlock();
    return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceManager::HasConference(const OpalGloballyUniqueID & conferenceID, PString & number)
{
  Conference *conference = FindConferenceWithLock(conferenceID);
  if(conference)
  {
    number = conference->GetNumber();
    conference->Unlock();
    return TRUE;
  }
  return FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceMember * ConferenceManager::FindMemberWithLock(const PString & roomName, const PString & memberName)
{
  Conference *conference = FindConferenceWithLock(roomName);
  if(conference == NULL)
    return NULL;
  ConferenceMember *member = FindMemberWithLock(conference, memberName);
  conference->Unlock();
  return member;
}
ConferenceMember * ConferenceManager::FindMemberWithLock(Conference * conference, const PString & memberName)
{
  MCUMemberList & memberList = conference->GetMemberList();
  return memberList(memberName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceMember * ConferenceManager::FindMemberSimilarWithLock(const PString & roomName, const PString & memberName)
{
  Conference *conference = FindConferenceWithLock(roomName);
  if(conference == NULL)
    return NULL;
  ConferenceMember *member = FindMemberSimilarWithLock(conference, memberName);
  conference->Unlock();
  return member;
}
ConferenceMember * ConferenceManager::FindMemberSimilarWithLock(Conference * conference, const PString & memberName)
{
  MCUMemberList & memberList = conference->GetMemberList();
  ConferenceMember *member = memberList(memberName);
  if(!member)
  {
    PString memberUrl = MCUURL(memberName).GetUrl();
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
      if(memberUrl == MCUURL(it->GetName()).GetUrl())
      {
        member = it.GetCapturedObject();
        break;
      }
  }
  if(!member)
  {
    PString memberNameID = MCUURL(memberName).GetMemberNameId();
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
      if(memberNameID == MCUURL(it->GetName()).GetMemberNameId())
      {
        member = it.GetCapturedObject();
        break;
      }
  }
  return member;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceMember * ConferenceManager::FindMemberWithLock(const PString & roomName, long id)
{
  Conference *conference = FindConferenceWithLock(roomName);
  if(conference == NULL)
    return NULL;
  ConferenceMember *member = FindMemberWithLock(conference, id);
  conference->Unlock();
  return member;
}
ConferenceMember * ConferenceManager::FindMemberWithLock(Conference * conference, long id)
{
  MCUMemberList & memberList = conference->GetMemberList();
  return memberList(id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSimpleVideoMixer * ConferenceManager::FindVideoMixerWithLock(const PString & room, long number)
{
  Conference *conference = FindConferenceWithLock(room);
  if(conference == NULL)
    return NULL;
  MCUSimpleVideoMixer *mixer = FindVideoMixerWithLock(conference, number);
  conference->Unlock();
  return mixer;
}

MCUSimpleVideoMixer * ConferenceManager::FindVideoMixerWithLock(Conference * conference, long number)
{
  MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
  if(conference->UseSameVideoForAllMembers())
  {
    return videoMixerList[number];
  }
  else
  {
    ConferenceMember *member = FindMemberWithLock(conference, number);
    if(member)
    {
      MCUSimpleVideoMixer *mixer = member->videoMixer;
      member->Unlock();
      return mixer;
    }
  }
  return NULL;
}

MCUSimpleVideoMixer * ConferenceManager::GetVideoMixerWithLock(const PString & room)
{
  Conference *conference = FindConferenceWithLock(room);
  if(conference == NULL)
    return NULL;
  MCUSimpleVideoMixer *mixer = GetVideoMixerWithLock(conference);
  conference->Unlock();
  return mixer;
}

MCUSimpleVideoMixer * ConferenceManager::GetVideoMixerWithLock(Conference * conference)
{
  MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
  MCUVideoMixerList::shared_iterator it = videoMixerList.begin();
  if(it != videoMixerList.end())
    return it.GetCapturedObject();
  return NULL;
}

MCUSimpleVideoMixer * ConferenceManager::GetVideoMixerWithLock(Conference * conference, int & number)
{
  MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
  if(videoMixerList.GetSize() == 0)
    return NULL;
  if(number < 0 || number >= videoMixerList.GetMaxSize())
    number = 0;
  for(int i = number; i < videoMixerList.GetMaxSize(); ++i)
  {
    MCUSimpleVideoMixer *mixer = videoMixerList[i];
    if(mixer)
    {
      number = i;
      return mixer;
    }
  }
  for(int i = 0; i < number; ++i)
  {
    MCUSimpleVideoMixer *mixer = videoMixerList[i];
    if(mixer)
    {
      number = i;
      return mixer;
    }
  }
  return NULL;
}

int ConferenceManager::AddVideoMixer(Conference * conference)
{
  MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
  MCUSimpleVideoMixer *mixer = new MCUSimpleVideoMixer(TRUE);
  mixer->SetID(videoMixerList.GetNextID());
  mixer->SetConference(conference);
  videoMixerList.Insert(mixer, mixer->GetID());
  return videoMixerList.GetSize();
}

int ConferenceManager::DeleteVideoMixer(Conference * conference, int number)
{
  MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
  if(videoMixerList.GetSize() == 1)
    return videoMixerList.GetSize();

  MCUSimpleVideoMixer *mixer = videoMixerList[number];
  if(mixer != NULL)
  {
    long id = mixer->GetID();
    videoMixerList.Release(id);
    if(videoMixerList.Erase(id))
      delete mixer;
  }
  return videoMixerList.GetSize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceManager::OnCreateConference(Conference * conference)
{
  if(MCUConfig("Export Parameters").GetBoolean("Enable export", FALSE) == TRUE)
  {
    conference->pipeMember = new ConferencePipeMember(conference);
    conference->AddMember(conference->pipeMember);
  }

  // add file recorder member
  if(GetConferenceParam(conference->GetNumber(), RoomAllowRecordKey, TRUE))
  {
    conference->conferenceRecorder = new ConferenceRecorder(conference);
    conference->AddMember(conference->conferenceRecorder);
  }

  if(!conference->GetForceScreenSplit())
  {
    PTRACE(1,"Conference\tOnCreateConference: \"Force split screen video\" unchecked, " << conference->GetNumber() << " skipping members.conf");
    return;
  }

  FILE *membLst;

  // read members.conf into conference->membersConf
  membLst = fopen(PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + "members_" + conference->GetNumber() + ".conf","rt");
  PStringStream membersConf;
  if(membLst!=NULL)
  { char buf [128];
    while(fgets(buf, 128, membLst)!=NULL) membersConf << buf;
    fclose(membLst);
  }

  conference->membersConf=membersConf;
  if(membersConf.Left(1)!="\n") membersConf="\n"+membersConf;

  // recall last template
  if(!GetConferenceParam(conference->GetNumber(), RoomRecallLastTemplateKey, FALSE)) return;

  PINDEX dp=membersConf.Find("\nLAST_USED ");
  if(dp!=P_MAX_INDEX)
  { PINDEX dp2=membersConf.Find('\n',dp+10);
    if(dp2!=P_MAX_INDEX)
    { PString lastUsedTemplate=membersConf.Mid(dp+11,dp2-dp-11).Trim();
      PTRACE(4, "Extracting & loading last used template: " << lastUsedTemplate);
      conference->confTpl=conference->ExtractTemplate(lastUsedTemplate);
      conference->LoadTemplate(conference->confTpl);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceManager::OnDestroyConference(Conference * conference)
{
  PString number = conference->GetNumber();
  PTRACE(2,"MCU\tOnDestroyConference " << number);
  conference->stopping=TRUE;

  MCUMemberList & memberList = conference->GetMemberList();

  PString jsName(number);
  jsName.Replace("\"","\\x27",TRUE,0); jsName.Replace("'","\\x22",TRUE,0);

  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(1,'" + jsName + "')", number);
  PTRACE(2,"MCU\tOnDestroyConference " << number <<", disconnect remote endpoints");

  for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
  {
    ConferenceMember * member = it.GetObject();
    member->SetAutoDial(FALSE);
    member->Close();
  }

  //OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(2,'" + jsName + "')", number);
  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(3,'" + jsName + "')", number);
  MCUTRACE(0,"MCU\tOnDestroyConference " << number <<", waiting... members: " << memberList.GetSize());

  while(true)
  {
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      // lock member list
      PWaitAndSignal m(conference->GetMemberListMutex());
      ConferenceMember * member = it.GetObject();
      if(member->IsOnline())
        continue;
      conference->RemoveMember(member, FALSE);
      if(memberList.Erase(it))
        delete member;
    }
    if(memberList.GetSize() == 0)
      break;
    MCUTime::Sleep(100);
  }

  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(4,'" + jsName + "')", number);
  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(5,'" + jsName + "')", number);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::CreateConference(long _id, const OpalGloballyUniqueID & _guid,
                                                              const PString & _number,
                                                              const PString & _name)
{
#if ENABLE_ECHO_MIXER
  if(_number.Left(4) *= "echo")
    return new Conference(*this, _id, _guid, "echo"+_guid.AsString(), _name, new EchoVideoMixer());
#endif
#if ENABLE_TEST_ROOMS
  if(_number.Left(8) == "testroom")
  {
    PString number = _number;
    int count = 0;
    if(_number.GetLength() > 8)
    {
      count = _number.Mid(8).AsInteger();
      if(count <= 0) { count = 0; number = "testroom"; }
    }
    if(count >= 0)
      return new Conference(*this, _id, _guid, number, _name, new TestVideoMixer(count));
  }
#endif

  BOOL forceScreenSplit = GetConferenceParam(_number, ForceSplitVideoKey, TRUE);

  if(!forceScreenSplit)
  {
    Conference *conference = new Conference(*this, _id, _guid, _number, _name, NULL);
    return conference;
  }

  PINDEX slashPos = _number.Find('/');
  PString number;
  if (slashPos != P_MAX_INDEX) number=_number.Left(slashPos);
  else number=_number;

  return new Conference(*this, _id, _guid, number, _name
#if MCU_VIDEO
                        , new MCUSimpleVideoMixer(forceScreenSplit)
#endif
                        );
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceManager::RemoveConference(const PString & room)
{
  MCUConferenceList::shared_iterator it = conferenceList.Find(room);
  if(it != conferenceList.end())
  {
    Conference *conference = *it;
    if(conferenceList.Erase(it))
    {
      OnDestroyConference(conference);
      delete conference;
      PTRACE(1, "RemoveConference");
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceManager::ClearConferenceList()
{
  for(MCUConferenceList::shared_iterator it = conferenceList.begin(); it != conferenceList.end(); ++it)
  {
    Conference *conference = it.GetObject();
    if(conferenceList.Erase(it))
    {
      OnDestroyConference(conference);
      delete conference;
      PTRACE(1, "RemoveConference");
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMonitor::Main()
{
  running = TRUE;
  while(running)
  {
    Sleep(1000);
    if(!running)
      break;

    MCUConferenceList & conferenceList = manager.GetConferenceList();
    for(MCUConferenceList::shared_iterator it = conferenceList.begin(); it != conferenceList.end(); ++it)
    {
      Conference *conference = *it;
      PString room = conference->GetNumber();
      int ret = Perform(conference);
      if(ret == 1)
      {
        it.Release();
        manager.RemoveConference(room);
      }
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

int ConferenceMonitor::Perform(Conference * conference)
{
  PTime now;
  if(now < conference->GetStartTime() + 1000)
    return 0;

  // time limit
  int timeLimit = GetConferenceParam(conference->GetNumber(), RoomTimeLimitKey, 0);
  if(timeLimit > 0 && now >= conference->GetStartTime() + timeLimit*1000)
  {
    return 1; // delete conference
  }

  // auto delete empty room
  BOOL autoDeleteEmpty = GetConferenceParam(conference->GetNumber(), RoomAutoDeleteEmptyKey, FALSE);
  if(autoDeleteEmpty && !conference->GetOnlineMemberCount())
  {
    return 1; // delete conference
  }

  // recorder
  BOOL allowRecord = GetConferenceParam(conference->GetNumber(), RoomAllowRecordKey, TRUE);
  if(!allowRecord)
  {
    conference->StopRecorder();
  }
  else
  {
    PString autoRecordStart = GetConferenceParam(conference->GetNumber(), RoomAutoRecordStartKey, "Disable");
    PString autoRecordStop = GetConferenceParam(conference->GetNumber(), RoomAutoRecordStopKey, "Disable");

    PINDEX onlineMembers = conference->GetOnlineMemberCount();

    if(autoRecordStop != "Disable" && onlineMembers <= autoRecordStop.AsInteger())
      conference->StopRecorder();
    else if(autoRecordStart != "Disable" && autoRecordStart.AsInteger() > autoRecordStop.AsInteger() && onlineMembers >= autoRecordStart.AsInteger())
      conference->StartRecorder();
  }

  // autodial
  if(OpenMCU::Current().autoDialDelay != 999999) //disable
  {
    if((conference->dialCountdown--) <= 0)
    {
      MCUMemberList & memberList = conference->GetMemberList();
      for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
      {
        ConferenceMember *member = *it;
        PWaitAndSignal m(member->GetDialMutex());
        if(!member->autoDial || member->IsSystem() || member->IsOnline())
          continue;
        MCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
        if(member->dialToken != "" && ep.HasConnection(member->dialToken))
          continue;
        member->dialToken = ep.Invite(conference->GetNumber(), member->GetName());
      }
      conference->dialCountdown = OpenMCU::Current().autoDialDelay;
    }
  }

  MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
  for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
    it->Monitor(conference);

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference::Conference(ConferenceManager & _manager, long _listID,
                       const OpalGloballyUniqueID & _guid,
                                    const PString & _number,
                                    const PString & _name
#if MCU_VIDEO
                                    , MCUSimpleVideoMixer * mixer
#endif
)
  : manager(_manager), listID(_listID), guid(_guid), number(_number), name(_name)
{
  stopping = FALSE;
  trace_section = "Conference "+number+": ";
#if MCU_VIDEO
  if(mixer)
  {
    mixer->SetID(videoMixerList.GetNextID());
    mixer->SetConference(this);
    videoMixerList.Insert(mixer, mixer->GetID());
  }
#endif
  onlineMemberCount = 0;
  visibleMemberCount = 0;
  maxMemberCount = 0;
  moderated = FALSE;
  muteUnvisible = FALSE;
  VAdelay = 1000;
  VAtimeout = 10000;
  VAlevel = 100;
  echoLevel = 0;
  conferenceRecorder = NULL;
  forceScreenSplit = GetConferenceParam(number, ForceSplitVideoKey, TRUE);
  lockedTemplate = GetConferenceParam(number, LockTemplateKey, FALSE);
  muteNewUsers = FALSE;
  pipeMember = NULL;
  dialCountdown = OpenMCU::Current().autoDialDelay;
  PTRACE(3, "Conference\tNew conference started: ID=" << guid << ", number = " << number);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference::~Conference()
{
#if MCU_VIDEO
  for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
  {
    MCUSimpleVideoMixer *mixer = it.GetObject();
    if(videoMixerList.Erase(it))
      delete mixer;
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::Unlock()
{
  manager.GetConferenceList().Release(listID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::RecorderCheckSpace()
{
  PDirectory pd(OpenMCU::Current().vr_ffmpegDir);
  PInt64 t, f;
  DWORD cs;
  if(!pd.GetVolumeSpace(t, f, cs))
  {
    PTRACE(1, trace_section << "Recorder space check failed");
    return TRUE;
  }
  BOOL result = ((f>>20) >= OpenMCU::Current().vr_minimumSpaceMiB);
  if(!result) OpenMCU::Current().HttpWriteEvent("<b><font color='red'>Insufficient disk space</font>: Video Recorder DISABLED</b>");
  PTRACE_IF(1,!result, trace_section << "Insufficient disk space: Video Recorder DISABLED");
  return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::StartRecorder()
{
  if(!conferenceRecorder)
    return FALSE;
  if(conferenceRecorder->IsRunning())
    return TRUE;

  if(!RecorderCheckSpace())
    return FALSE;
  if(!PDirectory::Exists(OpenMCU::Current().vr_ffmpegDir))
  {
    PTRACE(1, trace_section << "Recorder failed to start (check recorder directory)");
    OpenMCU::Current().HttpWriteEventRoom("Recorder failed to start (check recorder directory)", number);
    return FALSE;
  }

  conferenceRecorder->Start();

  for(int i = 0; i < 10; i++)
    if(conferenceRecorder->IsRunning()) break;
  else
    MCUTime::Sleep(100);

  if(!conferenceRecorder->IsRunning())
  {
    PTRACE(1, trace_section << "failed to start recorder");
    return FALSE;
  }

  PTRACE(1, trace_section << "video recorder started");
  OpenMCU::Current().HttpWriteEventRoom("video recording started", number);
  OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*this), number);
  OpenMCU::Current().HttpWriteCmdRoom("build_page()", number);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::StopRecorder()
{
  if(!conferenceRecorder)
    return TRUE;
  if(!conferenceRecorder->IsRunning())
    return TRUE;
  conferenceRecorder->Stop();

  PTRACE(1, trace_section << "video recorder stopped");
  OpenMCU::Current().HttpWriteEventRoom("video recording stopped", number);
  OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*this), number);
  OpenMCU::Current().HttpWriteCmdRoom("build_page()", number);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUMemberList::shared_iterator Conference::AddMemberToList(ConferenceMember * memberToAdd, BOOL addToList)
{
  // lock the member lists
  PWaitAndSignal m(memberListMutex);
  MCUMemberList::shared_iterator it;

  if(memberList.Find((long)memberToAdd->GetID()) != memberList.end())
  {
    PTRACE(1, trace_section << "Rejected duplicate member ID: " << (long)memberToAdd->GetID() << " " << memberToAdd->GetName());
    return it;
  }

  // check for duplicate name or very fast reconnect
  if(!memberToAdd->IsSystem())
  {
    // check for duplicate name or very fast reconnect
    PString memberName = memberToAdd->GetName();
    for(PINDEX i = 0; ; i++)
    {
      MCUMemberList::shared_iterator mit = memberList.Find(memberToAdd->GetName());
      if(mit == memberList.end())
        break;
      if(MCUConfig("Parameters").GetBoolean(RejectDuplicateNameKey, FALSE))
      {
        PStringStream msg;
        msg << JsQuoteScreen(memberToAdd->GetName()) << " REJECTED - DUPLICATE NAME";
        OpenMCU::Current().HttpWriteEventRoom(msg, number);
        PTRACE(1, trace_section << "Rejected duplicate name: " << memberToAdd->GetName());
        return it;
      }
      memberToAdd->SetName(memberName+" ##"+PString(i+2));
    }
  }

  // add to list
  if(addToList)
    it = memberList.Insert(memberToAdd, (long)memberToAdd->GetID(), memberToAdd->GetName());

  // send event
  memberToAdd->SendRoomControl(1);

  return it;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::AddMember(ConferenceMember * memberToAdd, BOOL addToList)
{
  MCUTRACE(3, trace_section << "Adding member: " << memberToAdd << " " << memberToAdd->GetName() << " type:" << memberToAdd->GetType() << " joined:" << memberToAdd->IsJoined());

  // lock the member lists
  PWaitAndSignal m(memberListMutex);

  // notify that member is joined
  if(memberToAdd->IsJoined())
  {
    PTRACE(4, trace_section << "member already joined " << memberToAdd->GetName());
    return TRUE;
  }
  memberToAdd->SetJoined(TRUE);

  // first add to list
  if(AddMemberToList(memberToAdd, addToList) == memberList.end())
    return FALSE;

  // nothing more!
  if(!memberToAdd->IsVisible())
    return TRUE;

  // counter members
  visibleMemberCount++;
  if(memberToAdd->IsOnline())
  {
    onlineMemberCount++;
    maxMemberCount = PMAX(maxMemberCount, onlineMemberCount);
  }

  if(UseSameVideoForAllMembers())
  {
    if(moderated == FALSE
#if ENABLE_TEST_ROOMS
       || number == "testroom"
#endif
      )
    {
      MCUSimpleVideoMixer * mixer = manager.GetVideoMixerWithLock(this);
      mixer->AddVideoSource(memberToAdd->GetID(), *memberToAdd);
      mixer->Unlock();
    }
    else
    {
      for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
      {
        MCUSimpleVideoMixer *mixer = it.GetObject();
        if(!mixer->SetOnline(memberToAdd->GetID()))
        {
          if(!memberToAdd->disableVAD)
            mixer->TryOnVADPosition(memberToAdd);
        }
      }
    }
  }
  else
  {
    // classic mode
    // make sure each member has a connection created for the new member
    // make sure the new member has a connection created for each existing member
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      ConferenceMember *member = it.GetObject();
      if(member != memberToAdd)
      {
        if(member->IsVisible())
          memberToAdd->AddVideoSource(member->GetID(), *member);
        member->AddVideoSource(memberToAdd->GetID(), *memberToAdd);
      }
    }
  }

  // update the statistics
  // trigger H245 thread for join message
  //new NotifyH245Thread(*this, TRUE, memberToAdd);

  if (muteNewUsers) {
    memberToAdd->SetChannelPauses(1);
  }
  else {
    memberToAdd->UnsetChannelPauses(1);
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::RemoveMember(ConferenceMember * memberToRemove, BOOL removeFromList)
{
  MCUTRACE(3, trace_section << "Removing member: " << memberToRemove << " " << memberToRemove->GetName() << " type:" << memberToRemove->GetType() << " joined:" << memberToRemove->IsJoined());

  // lock memberList
  PWaitAndSignal m(memberListMutex);

  // notify that member is not joined anymore
  if(!memberToRemove->IsJoined())
  {
    PTRACE(4, trace_section << "member not joined " << memberToRemove->GetName());
    return TRUE;
  }
  memberToRemove->SetJoined(FALSE);

  // first remove from list
  if(removeFromList)
    memberList.Erase((long)memberToRemove->GetID());

  // Обнулить до отправки event
  memberToRemove->channelMask = 0;

  // send event
  memberToRemove->SendRoomControl(0);

  // nothing more!
  if(!memberToRemove->IsVisible())
    return TRUE;

  // counter members
  if(memberToRemove->IsOnline())
    onlineMemberCount--;

  // remove ConferenceConnection
  RemoveAudioConnection(memberToRemove);

  if(UseSameVideoForAllMembers())
  {
    if(moderated == FALSE || number == "testroom")
    {
      visibleMemberCount--;
      MCUSimpleVideoMixer *mixer = manager.GetVideoMixerWithLock(this);
      mixer->RemoveVideoSource(memberToRemove->GetID(), *memberToRemove);
      mixer->Unlock();
    }
    else
    {
      for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
      {
        MCUSimpleVideoMixer *mixer = it.GetObject();
        mixer->SetOffline(memberToRemove->GetID());
      }
    }
  }
  else
  {
    // classic mode
    // remove this member from the connection lists for all other members
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      ConferenceMember *member = it.GetObject();
      if(member != memberToRemove)
      {
        member->RemoveVideoSource(memberToRemove->GetID(), *memberToRemove);
        if(member->IsVisible())
          memberToRemove->RemoveVideoSource(member->GetID(), *member);
      }
    }
  }


  // trigger H245 thread for leave message
  //new NotifyH245Thread(*this, FALSE, memberToRemove);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::RemoveAudioConnection(ConferenceMember * member)
{
  MCUAudioConnectionList::shared_iterator it = audioConnectionList.Find((long)member->GetID());
  if(it != audioConnectionList.end())
  {
    ConferenceAudioConnection * conn = it.GetObject();
    if(audioConnectionList.Erase(it))
      delete conn;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::ReadMemberAudio(ConferenceMember * member, const uint64_t & timestamp, void * buffer, int amount, int sampleRate, int channels)
{
  for(MCUAudioConnectionList::shared_iterator it = audioConnectionList.begin(); it != audioConnectionList.end(); ++it)
  {
    ConferenceAudioConnection * conn = it.GetObject();
    if(conn->GetID() == member->GetID())
      continue;

    BOOL skip = moderated&&muteUnvisible;
    if(skip)
    {
      for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
      {
        MCUSimpleVideoMixer *mixer = it.GetObject();
        if(mixer->VMPExists((ConferenceMemberId)conn->GetID()))
        {
          skip = FALSE;
          break;
        }
      }
    }
    if(!skip) // default behaviour
      conn->ReadAudio(timestamp, (BYTE *)buffer, amount, sampleRate, channels);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::WriteMemberAudio(ConferenceMember * member, const uint64_t & timestamp, const void * buffer, int amount, int sampleRate, int channels)
{
  if(member->IsSystem())
    return;

  MCUAudioConnectionList::shared_iterator it = audioConnectionList.Find((long)member->GetID());
  ConferenceAudioConnection *conn = *it;
  if(conn && (conn->GetSampleRate() != sampleRate || conn->GetChannels() != channels))
  {
    if(audioConnectionList.Erase(it))
      delete conn;
    conn = NULL;
  }
  if(conn == NULL)
  {
    conn = new ConferenceAudioConnection(member->GetID(), sampleRate, channels);
    it = audioConnectionList.Insert(conn, (long)member->GetID());
    conn = *it;
  }
  conn->WriteAudio(timestamp, (const BYTE *)buffer, amount);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// tint - time interval since last call in msec
void Conference::WriteMemberAudioLevel(ConferenceMember * member, int audioLevel, int tint)
{
  member->audioLevelIndicator|=audioLevel;
  member->audioCounter+=tint;
  if(member->audioCounter>1999) //2s
  {
    if (member->audioLevelIndicator < 64) member->audioLevelIndicator = 0;
    if((member->previousAudioLevel != member->audioLevelIndicator)||((member->audioLevelIndicator!=0) && ((member->audioCounter&255)==0)))
    {
      PStringStream msg;
      msg << "audio(" << (long)member->GetID() << "," << member->audioLevelIndicator << ")";
      OpenMCU::Current().HttpWriteCmdRoom(msg,number);
      member->previousAudioLevel=member->audioLevelIndicator;
    }
    member->audioCounter=0;
    member->audioLevelIndicator=0;
  }
#if MCU_VIDEO
  if(UseSameVideoForAllMembers())
  {
    if(audioLevel > VAlevel)
      member->vad += tint;
    else
      member->vad = 0;

    BOOL resetMemberVad = FALSE;
    for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
    {
      MCUSimpleVideoMixer *mixer = it.GetObject();
      int silenceCounter = mixer->GetSilenceCounter(member->GetID());
      if(audioLevel > VAlevel) // we have a signal
      {
        if(member->vad >= VAdelay) // voice-on trigger delay
        {
          if(silenceCounter > 0) mixer->ResetSilenceCounter(member->GetID());
          else if(silenceCounter == 0 && member->disableVAD == FALSE)
          {
            if(member->vad-VAdelay>500) // execute every 500 ms of voice activity
            {
              mixer->SetVAD2Position(member);
            }
          }
          else if(silenceCounter == -1 && member->disableVAD == FALSE) //find new vad position for active member
          {
            if(mixer->SetVADPosition(member, member->chosenVan, VAtimeout))
            {
              FreezeVideo(member->GetID());
              member->SetFreezeVideo(FALSE);
            }
          }
        }
      }
      else
      {
        if(silenceCounter >= 0) mixer->IncreaseSilenceCounter(member->GetID(),tint);
      }
      if(audioLevel > VAlevel && silenceCounter == 0 && member->disableVAD == FALSE && member->vad-VAdelay > 500)
        resetMemberVad = TRUE;
    }
    if(resetMemberVad)
      member->vad = VAdelay;
  }
#endif // MCU_VIDEO
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::ReadMemberVideo(ConferenceMember * member, void * buffer, int width, int height, PINDEX & amount)
{
  if(videoMixerList.GetSize() == 0)
    return;

  long mixerNumber;
  if(member == NULL)
    mixerNumber = 0;
  else
    mixerNumber = member->GetVideoMixerNumber();

  MCUSimpleVideoMixer * mixer = manager.FindVideoMixerWithLock(this, mixerNumber);
  if(mixer == NULL)
  {
    mixer = manager.GetVideoMixerWithLock(this);
    if(mixer == NULL)
    {
      PTRACE(3, trace_section << "Could not get video");
      return;
    }
  }

  mixer->ReadFrame(*member, buffer, width, height, amount);
  mixer->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::WriteMemberVideo(ConferenceMember * member, const void * buffer, int width, int height)
{
  if(UseSameVideoForAllMembers())
  {
    bool writeResult = FALSE;
    for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
    {
      MCUSimpleVideoMixer *mixer = it.GetObject();
      writeResult |= mixer->WriteFrame(member->GetID(), buffer, width, height);
    }
    return writeResult;
  }
  else
  {
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
      it->OnExternalSendVideo(member->GetID(), buffer, width, height);
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::FreezeVideo(ConferenceMemberId id)
{
  PWaitAndSignal m(memberListMutex);

  if(id)
  {
    MCUMemberList::shared_iterator it = memberList.Find((long)id);
    if(it == memberList.end())
      return;
    ConferenceMember *member = it.GetObject();

    if(UseSameVideoForAllMembers())
    {
      for(MCUVideoMixerList::shared_iterator it2 = videoMixerList.begin(); it2 != videoMixerList.end(); ++it2)
      {
        MCUSimpleVideoMixer *mixer = it2.GetObject();
        if(mixer->VMPExists(id))
        {
          member->SetFreezeVideo(FALSE);
          return;
        }
      }
    } else {
      for(MCUMemberList::shared_iterator it2 = memberList.begin(); it2 != memberList.end(); ++it2)
      {
        ConferenceMember *member = it2.GetObject();
        if(member->videoMixer && member->videoMixer->VMPExists(id))
        {
          member->SetFreezeVideo(FALSE);
          return;
        }
      }
    }
    member->SetFreezeVideo(TRUE);
    return;
  }

  for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
  {
    ConferenceMember *member = it.GetObject();
    ConferenceMemberId mid = member->GetID();
    if(UseSameVideoForAllMembers())
    {
      for(MCUVideoMixerList::shared_iterator it2 = videoMixerList.begin(); it2 != videoMixerList.end(); ++it2)
      {
        MCUSimpleVideoMixer *mixer = it2.GetObject();
        if(mixer->VMPExists(mid))
        {
          member->SetFreezeVideo(FALSE);
          return;
        }
      }
    } else {
      MCUMemberList::shared_iterator it2;
      for(it2 = memberList.begin(); it2 != memberList.end(); ++it2)
      {
        ConferenceMember *member = it2.GetObject();
        if(member->videoMixer && member->videoMixer->VMPExists(mid))
        {
          member->SetFreezeVideo(FALSE);
          break;
        }
      }
      if(it2 == memberList.end())
        member->SetFreezeVideo(TRUE);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::PutChosenVan()
{
  BOOL put = FALSE;
  for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
  {
    ConferenceMember *member = it.GetObject();
    if(member->chosenVan)
    {
      for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
      {
        MCUSimpleVideoMixer *mixer = it.GetObject();
        if(mixer->VMPExists(member->GetID()))
          put |= mixer->SetVADPosition(member, member->chosenVan, VAtimeout);
      }
    }
  }
  return put;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::HandleFeatureAccessCode(ConferenceMember & member, PString fac)
{
  PTRACE(3, trace_section << "Handling feature access code " << fac << " from " << member.GetName());
  PStringArray s = fac.Tokenise("*");
  if(s[0]=="1")
  {
    int posTo=0;
    if(s.GetSize() > 1)
      posTo=s[1].AsInteger();
    PTRACE(4, trace_section << "*1*" << posTo << "#: jump into video position " << posTo);

    if(videoMixerList.GetSize() == 0)
      return;

    ConferenceMemberId id=member.GetID();
    if(!id) return;

    MCUSimpleVideoMixer *mixer = manager.GetVideoMixerWithLock(this);
    int pos = mixer->GetPositionNum(id);
    if(pos == posTo)
    {
      mixer->Unlock();
      return;
    }
    //mixer->InsertVideoSource(&member,posTo); // ???
    mixer->Unlock();

    FreezeVideo(0);

    OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*this),number);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",number);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::UpdateVideoMixOptions(ConferenceMember * member)
{
  if(videoMixerList.GetSize() != 0)
  {
    for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
    it.GetObject()->Update(member);
  }
  else // classic MCU mode
  {
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
    {
      MCUVideoMixer * mixer = (*it)->videoMixer;
      if(mixer!=NULL) mixer->Update(member);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceMember::ConferenceMember(Conference * _conference)
  : conference(_conference)
{
  memberType = MEMBER_TYPE_NONE;
  visible = FALSE;
  isMCU = FALSE;
  id = (ConferenceMemberId)OpenMCU::GetUniqueMemberID();
  channelMask = 0;
  audioLevel = 0;
  audioCounter = 0;
  previousAudioLevel = 65535;
  audioLevelIndicator = 0;
  currVolCoef = 1.0;
  kManualGain = 1.0; kManualGainDB = 0;
  kOutputGain = 1.0; kOutputGainDB = 0;
  memberIsJoined = FALSE;

#if MCU_VIDEO
  if(conference->UseSameVideoForAllMembers())
    videoMixer = NULL;
  else
    videoMixer = new MCUSimpleVideoMixer();

  totalVideoFramesReceived = 0;
  firstFrameReceiveTime = -1;
  totalVideoFramesSent = 0;
  firstFrameSendTime = -1;
  rxFrameWidth = 0; rxFrameHeight = 0;
  vad = 0;
  autoDial = FALSE;
  muteMask = 0;
  disableVAD = FALSE;
  chosenVan = 0;
  videoMixerNumber = 0;
  resizerRule = 0;
#endif
  write_audio_time_microseconds=0;
  write_audio_average_level=0;
  write_audio_write_counter=0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceMember::~ConferenceMember()
{
  muteMask|=15;

#if MCU_VIDEO
  if(videoMixer)
    delete videoMixer;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::Unlock()
{
  conference->GetMemberList().Release((long)GetID());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::Close()
{
  if(IsSystem())
    return;

  MCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
  PWaitAndSignal m(dialMutex);

  if(!IsOnline() && dialToken != "")
  {
    MCUH323Connection *conn = ep.FindConnectionWithLock(dialToken);
    if(conn)
    {
      conn->ClearCall();
      conn->Unlock();
    }
  }
  MCUH323Connection *conn = ep.FindConnectionWithLock(callToken);
  if(conn != NULL)
  {
    conn->ClearCall();
    conn->Unlock();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::Dial()
{
  Dial(autoDial);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::Dial(BOOL _autoDial)
{
  if(IsSystem())
    return;
  PWaitAndSignal m(dialMutex);
  autoDial = _autoDial;
  if((autoDial && (OpenMCU::Current().autoDialDelay < 20)) || IsOnline())
    return;
  MCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
  if(dialToken != "" && ep.HasConnection(dialToken))
    return;
  dialToken = ep.Invite(conference->GetNumber(), GetName());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::SetAutoDial(BOOL enable)
{
  PWaitAndSignal m(dialMutex);
  if(!enable && autoDial && dialToken != "" && !IsOnline())
    Close();
  autoDial = enable;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::SendRoomControl(int state)
{
  if(IsSystem())
    return;

  PStringStream msg;
  if(IsOnline())
  {
    if(state == 1)
    {
      msg << "<font color=green><b>+</b>" << GetName() << "</font>";
      OpenMCU::Current().HttpWriteEventRoom(msg, conference->GetNumber());
    } else {
      msg << "<font color=red><b>-</b>" << GetName() << "</font>";
      OpenMCU::Current().HttpWriteEventRoom(msg, conference->GetNumber());
    }
  }

  if(state == 1)
    msg = "addmmbr(";
  else
    msg = "remmmbr(";
  msg  << (state && IsOnline())
       << "," << (long)GetID()
       << "," << JsQuoteScreen(GetName())
       << "," << muteMask
       << "," << disableVAD
       << "," << chosenVan
       << "," << GetAudioLevel()
       << "," << GetVideoMixerNumber()
       << "," << JsQuoteScreen(GetNameID())
       << "," << dec << channelMask
       << "," << kManualGainDB
       << "," << kOutputGainDB
       << ",[]"
       << "," << memberType
       << "," << autoDial
       << "," << resizerRule
       << ")";
  OpenMCU::Current().HttpWriteCmdRoom(msg, conference->GetNumber());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUJSON * ConferenceMember::AsJSON(int state)
{
  MCUJSON *json = new MCUJSON(MCUJSON::JSON_ARRAY);
  json->Insert("online", (state && IsOnline()));
  json->Insert("id", id);
  json->Insert("name", name);
  json->Insert("muteMask", muteMask);
  json->Insert("disableVad", disableVAD);
  json->Insert("chosenVan", chosenVan);
  json->Insert("audioLevel", GetAudioLevel());
  json->Insert("videoMixerMumber", GetVideoMixerNumber());
  json->Insert("nameID", GetNameID());
  json->Insert("channelMask", channelMask);
  json->Insert("kManualGainDB", kManualGainDB);
  json->Insert("kOutputGainDB", kOutputGainDB);
  json->Insert("mixer", "[]");
  json->Insert("memberType", memberType);
  json->Insert("autoDial", autoDial);
  return json;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::ChannelStateUpdate(unsigned bit, BOOL state)
{
  if(state)
    channelMask|=bit;
  else
    channelMask&=~bit;

  PStringStream msg;
  msg << "rtp_state(" << dec << (long)id << "," << bit << "," << state << ")";
  PTRACE(1, name << " " << msg);

  if(conference)
    OpenMCU::Current().HttpWriteCmdRoom(msg, conference->GetNumber());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::WriteAudioAutoGainControl(const short * pcm, unsigned samplesPerFrame,
  unsigned codecChannels, unsigned sampleRate,
  unsigned level, float* currVolCoef, unsigned* signalLevel, float kManual)
{
  unsigned samplesCount = samplesPerFrame*codecChannels;
  if(!samplesCount) return;

  const short * end = pcm + samplesCount;
  short *buf = (short*)pcm;
  int c_max_vol = 0, c_avg_vol = 0;
  while (pcm != end) 
  {
    if (*pcm < 0) 
    { if(-*pcm > c_max_vol) c_max_vol = -*pcm; c_avg_vol -= *pcm++; }
    else 
    { if( *pcm > c_max_vol) c_max_vol =  *pcm; c_avg_vol += *pcm++; }
  }
  c_avg_vol /= samplesPerFrame;

  if(!level)
  {
    *signalLevel = c_avg_vol;
    return;
  }

  float   max_vol = (float)23170.0 * kManual;
  float   overload = 32768 * kManual;
  float   inc_vol = (float)0.05*(float)8000.0/sampleRate;
  float & cvc = *currVolCoef;
  float   vc0= cvc;
  
  unsigned wLevel; // AGC level
//  wLevel = (unsigned)((float)(level*10)/kManual); //worst result
  if(kManual>10) wLevel = (unsigned)((float)(level*10)/kManual);
  else wLevel=level;

  if((unsigned)c_avg_vol > wLevel) // signal detected
  {
    if(c_max_vol*cvc >= overload) // overload
      cvc = overload / c_max_vol;
    else
    if(c_max_vol*cvc < max_vol) // ++ amplification
      cvc += inc_vol;
  }
  else // no signal (just a noise) but check it for overload for safety reason
  {
    if(c_max_vol*cvc >= overload) // overload
      cvc = (float)overload / c_max_vol;
  }
  *signalLevel = (unsigned)((float)c_avg_vol*cvc);

  float delta0=(cvc-vc0)/samplesCount;

  for(unsigned i=0; i<samplesCount; i++) 
  {
    int v = buf[i];
    v=(int)(v*vc0);
    if(v > 32767) buf[i]=32767;
    else if(v < -32768) buf[i]=-32768;
    else buf[i] = (short)v;
    vc0+=delta0;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::WriteAudio(const uint64_t & timestamp, const void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels)
{
  if(conference != NULL)
  {
    // Автоматическая регулировка усиления
    // calculate average signal level for this member
    unsigned signalLevel=0;
    WriteAudioAutoGainControl((short*) buffer, amount/channels/2, channels, sampleRate, 900, &currVolCoef, &signalLevel, kManualGain);
    audioLevel = ((signalLevel * 2) + audioLevel) / 3;

    // Записать аудио в буфер // Write to buffer
    conference->WriteMemberAudio(this, timestamp, buffer, amount, sampleRate, channels);


    // Индикатор уровня, VAD // Level indication, VAD
#   define MINIMUM_VAD_INTERVAL_MS 250
    write_audio_time_microseconds += amount*1000000/2/channels/sampleRate;
    write_audio_average_level += audioLevel;
    write_audio_write_counter++;
    if(write_audio_time_microseconds >= MINIMUM_VAD_INTERVAL_MS * 1000)
    {
      conference->WriteMemberAudioLevel(this, write_audio_average_level/write_audio_write_counter, write_audio_time_microseconds/1000);
      write_audio_average_level = 0;
      write_audio_time_microseconds = 0;
      write_audio_write_counter = 0;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::ReadAudio(const uint64_t & timestamp, void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels)
{
  // First, set the buffer to empty.
  memset(buffer, 0, amount);

  if(conference != NULL)
  {
    // Получить аудио от участников
    conference->ReadMemberAudio(this, timestamp, buffer, amount, sampleRate, channels);

    // Регулировка усиления
    ReadAudioGainControl(buffer, amount);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::ReadAudioGainControl(void * buffer, int amount)
{
  if(kOutputGainDB)
  {
    float k = kOutputGain;
    unsigned i = (amount >> 1);
    char * pos = (char*)buffer;
    while(i)
    {
      int c = (int)(k*(*(short*)pos));
      if(c>32766) *(short*)pos = 32767;
      else if(c<-32767) *(short*)pos = -32768;
      else *(short*)pos = (short)c;
      pos+=2;
      i--;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCU_VIDEO

// called whenever the connection needs a frame of video to send
void ConferenceMember::ReadVideo(void * buffer, int width, int height, PINDEX & amount)
{
  ++totalVideoFramesSent;
  if(!firstFrameSendTime.IsValid())
    firstFrameSendTime = PTime();

  if(conference != NULL)
  {
    if(conference->UseSameVideoForAllMembers())
      conference->ReadMemberVideo(this, buffer, width, height, amount);
    else if(videoMixer != NULL)
      videoMixer->ReadFrame(*this, buffer, width, height, amount);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// called whenever the connection receives a frame of video
void ConferenceMember::WriteVideo(const void * buffer, int width, int height)
{
  ++totalVideoFramesReceived;
  rxFrameWidth = width;
  rxFrameHeight = height;
  if (!firstFrameReceiveTime.IsValid())
    firstFrameReceiveTime = PTime();

  if(conference != NULL)
    conference->WriteMemberVideo(this, buffer, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::OnExternalSendVideo(ConferenceMemberId id, const void * buffer, int width, int height)
{
  videoMixer->WriteFrame(id, buffer, width, height);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceMember::AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  PAssert(videoMixer != NULL, "attempt to add video source to NULL video mixer");
  return videoMixer->AddVideoSource(id, mbr);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  PAssert(videoMixer != NULL, "attempt to remove video source from NULL video mixer");
  videoMixer->RemoveVideoSource(id, mbr);
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////

PString ConferenceMember::GetMonitorInfo(const PString & /*hdr*/)
{ 
  return PString::Empty(); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceAudioConnection::ConferenceAudioConnection(ConferenceMemberId _id, int _sampleRate, int _channels)
  : ConferenceConnection(_id)
{
  sampleRate = _sampleRate;
  channels = _channels;
  maxFrameTime = 0;
  timeIndex = 0;
  startTimestamp = 0;

  // Создать все возможные варианты resampler'ов,
  // создание занимает "значительное" время
  for(int i = 0; audio_resolutions[i].samplerate != 0; ++i)
  {
    int dstSampleRate = audio_resolutions[i].samplerate;
    int dstChannels = audio_resolutions[i].channels;
    long resamplerKey = dstSampleRate + dstChannels;
    AudioResampler *resampler = AudioResampler::Create(sampleRate, channels, dstSampleRate, dstChannels);
    if(resampler)
      audioResamplerList.insert(AudioResamplerListType::value_type(resamplerKey, resampler));
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceAudioConnection::~ConferenceAudioConnection()
{
  for(AudioResamplerListType::iterator it = audioResamplerList.begin(); it != audioResamplerList.end(); )
  {
    AudioResampler *resampler = it->second;
    audioResamplerList.erase(it++);
    delete resampler;
  }
  for(MCUAudioBufferList::shared_iterator it = audioBufferList.begin(); it != audioBufferList.end(); ++it)
  {
    AudioBuffer *audioBuffer = it.GetObject();
    audioBufferList.Erase(it);
    delete audioBuffer;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AudioBuffer * ConferenceAudioConnection::GetBuffer(int _dstSampleRate, int _dstChannels)
{
  AudioBuffer * audioBuffer = NULL;
  long audioBufferKey = _dstSampleRate + _dstChannels;
  MCUAudioBufferList::shared_iterator it = audioBufferList.Find(audioBufferKey);
  if(it != audioBufferList.end())
    audioBuffer = it.GetObject();
  else
  {
    // mutex только для добавления буфера в список
    PWaitAndSignal m(audioBufferListMutex);
    // Повторная проверка
    it = audioBufferList.Find(audioBufferKey);
    if(it != audioBufferList.end())
      audioBuffer = it.GetObject();
    else
    {
      audioBuffer = new AudioBuffer(_dstSampleRate, _dstChannels);
      audioBufferList.Insert(audioBuffer, audioBufferKey);
    }
  }

  return audioBuffer;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceAudioConnection::WriteAudio(const uint64_t & srcTimestamp, const BYTE * data, int amount)
{
  if(amount == 0)
    return;

  int frameTime = amount * 1000 / (sampleRate * channels * 2);
  if(frameTime > PCM_BUFFER_MAX_WRITE_LEN_MS)
    return;

  if(frameTime > maxFrameTime)
    maxFrameTime = frameTime;

  // копия
  int srcTimeIndex = timeIndex;

  if(startTimestamp == 0)
    // константа, не меняется
    startTimestamp = srcTimestamp - frameTime*1000;
  else
  {
    uint64_t writeTimestamp = startTimestamp + (uint64_t)srcTimeIndex*1000 + frameTime*1000;
    if(writeTimestamp + PCM_BUFFER_LAG_MS*1000 < srcTimestamp)
    {
      srcTimeIndex = srcTimestamp/1000 - startTimestamp/1000 - frameTime;
      PTRACE(6, "ConferenceAudioConnection\tWriter has lost " << srcTimestamp - writeTimestamp << " us"
                << ", start=" << startTimestamp << " write=" << writeTimestamp  << " src=" << srcTimestamp
                << " index=" << timeIndex << " frame=" << frameTime);
    }
  }

  for(MCUAudioBufferList::shared_iterator r = audioBufferList.begin(); r != audioBufferList.end(); ++r)
  {
    AudioBuffer *audioBuffer = r.GetObject();

    long resamplerKey = audioBuffer->GetSampleRate() + audioBuffer->GetChannels();
    AudioResamplerListType::iterator s = audioResamplerList.find(resamplerKey);
    if(s == audioResamplerList.end())
      continue;
    AudioResampler * resampler = s->second;

    int dstBufferSize = frameTime * audioBuffer->GetTimeSize();
    MCUBuffer dstBuffer(dstBufferSize);

    resampler->Resample(data, amount, dstBuffer.GetPointer(), dstBufferSize);

    int byteIndex = (srcTimeIndex % PCM_BUFFER_LEN_MS) * audioBuffer->GetTimeSize();
    int byteLeft = dstBufferSize;
    int byteOffset = 0;
    if(byteIndex + byteLeft > audioBuffer->GetSize())
    {
      byteOffset = audioBuffer->GetSize() - byteIndex;
      memcpy(audioBuffer->GetPointer() + byteIndex, dstBuffer.GetPointer(), byteOffset);
      byteLeft -= byteOffset;
      byteIndex = 0;
    }
    memcpy(audioBuffer->GetPointer() + byteIndex, dstBuffer.GetPointer() + byteOffset, byteLeft);
  }

  timeIndex = srcTimeIndex + frameTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceAudioConnection::ReadAudio(const uint64_t & dstTimestamp, BYTE * data, int amount, int dstSampleRate, int dstChannels)
{
  if(amount == 0)
    return;

  if(timeIndex < PCM_BUFFER_MAX_WRITE_LEN_MS + PCM_BUFFER_MAX_READ_LEN_MS + PCM_BUFFER_LAG_MS)
    return;

  int dstFrameTime = amount * 1000 / (dstSampleRate * dstChannels * 2);
  if(dstFrameTime > PCM_BUFFER_MAX_READ_LEN_MS)
    return;

  // копия
  int srcTimeIndex = timeIndex;
  int srcFrameTime = maxFrameTime;

  // Позиция в буфере на время dstTimestamp
  //int dstTimeIndex = dstTimestamp/1000 - startTimestamp/1000 - PCM_BUFFER_MAX_WRITE_LEN_MS - PCM_BUFFER_LAG_MS;
  int dstTimeIndex = dstTimestamp/1000 - startTimestamp/1000 - srcFrameTime - PCM_BUFFER_LAG_MS;

  // Что то пошло не так :(
  // Позиция отрицательная или меньше размера фрейма
  if(dstTimeIndex < dstFrameTime)
    return;

  // Нет данных на это время, возможно запись прекращена
  if(dstTimeIndex > srcTimeIndex)
    return;

  // Время за пределами буфера(не хватает буфера). Проверка не точная,
  // можно не проверять т.к. буфер "круговой", но результат будет на другое время.
  if(srcTimeIndex - dstTimeIndex > PCM_BUFFER_LEN_MS - PCM_BUFFER_MAX_WRITE_LEN_MS - dstFrameTime)
    return;

  // Найти или создать буфер
  AudioBuffer * audioBuffer = GetBuffer(dstSampleRate, dstChannels);

  int dstBufferSize = dstFrameTime * audioBuffer->GetTimeSize();
  MCUBuffer dstBuffer(dstBufferSize);

  int byteIndex = ((dstTimeIndex - dstFrameTime) % PCM_BUFFER_LEN_MS) * audioBuffer->GetTimeSize();
  int byteLeft = dstBufferSize;
  int byteOffset = 0;
  if(byteIndex + byteLeft > audioBuffer->GetSize())
  {
    byteOffset = audioBuffer->GetSize() - byteIndex;
    memcpy(dstBuffer.GetPointer(), audioBuffer->GetPointer() + byteIndex, byteOffset);
    byteLeft = dstBufferSize - byteOffset;
    byteIndex = 0;
  }
  memcpy(dstBuffer.GetPointer() + byteOffset, audioBuffer->GetPointer() + byteIndex, byteLeft);

  Mix(dstBuffer.GetPointer(), data, dstBufferSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceAudioConnection::Mix(const BYTE * src, BYTE * dst, int count)
{
  int i = count >> 1;
  do
  {
    short dstVal = *(short *)dst;
    short srcVal = *(short *)src;
    int newVal = dstVal;                                     // 16-bit to 32-bit, signed
    newVal += srcVal;                                        // mix
    if     (newVal >  0x7fff) *(short *)dst =  0x7fff;       // 16-bit limiter "+"
    else if(newVal < -0x8000) *(short *)dst = -0x8000;       // 16-bit limiter "-"
    else                      *(short *)dst = (short)newVal;
    src += 2;
    dst += 2;
  } while (--i);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AudioBuffer::AudioBuffer(int _sampleRate, int _channels)
{
  sampleRate = _sampleRate;
  channels = _channels;

  bufferTimeSize = sampleRate * channels * 2 / 1000;
  bufferSize = PCM_BUFFER_LEN_MS * bufferTimeSize;
  buffer.SetSize(bufferSize);
  memset(buffer.GetPointer(), 0, bufferSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AudioBuffer::~AudioBuffer()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AudioResampler::AudioResampler(int _srcSampleRate, int _srcChannels, int _dstSampleRate, int _dstChannels)
{
  srcSampleRate = _srcSampleRate;
  srcChannels = _srcChannels;
  dstSampleRate = _dstSampleRate;
  dstChannels = _dstChannels;
#if USE_SWRESAMPLE || USE_AVRESAMPLE || USE_LIBSAMPLERATE
  swrc = NULL;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AudioResampler::~AudioResampler()
{
#if USE_SWRESAMPLE
  if(swrc) swr_free(&swrc);
#elif USE_AVRESAMPLE
  if(swrc) avresample_free(&swrc);
#elif USE_LIBSAMPLERATE
  if(swrc) src_delete(swrc);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

AudioResampler * AudioResampler::Create(int _srcSampleRate, int _srcChannels, int _dstSampleRate, int _dstChannels)
{
  AudioResampler * resampler = new AudioResampler(_srcSampleRate, _srcChannels, _dstSampleRate, _dstChannels);
  if(resampler->Initialise() == FALSE)
  {
    delete resampler;
    resampler = NULL;
  }
  return resampler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL AudioResampler::Initialise()
{
  if(srcSampleRate == dstSampleRate && srcChannels == dstChannels)
    return TRUE;

#if USE_SWRESAMPLE
  swrc = swr_alloc_set_opts(NULL,
    MCU_AV_CH_Layout_Selector[dstChannels], AV_SAMPLE_FMT_S16, dstSampleRate,
    MCU_AV_CH_Layout_Selector[srcChannels], AV_SAMPLE_FMT_S16, srcSampleRate, 0, NULL);
  if(swrc == NULL)
  {
    PTRACE(1, "AudioResampler\tcould not allocate resampler context");
    return FALSE;
  }
  int ret = swr_init(swrc);
  if(ret < 0)
  {
    PTRACE(1, "AudioResampler\tfailed to initialize the resampling context: " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }
#elif USE_AVRESAMPLE
  swrc = avresample_alloc_context();
  if(swrc == NULL)
  {
    PTRACE(1, "AudioResampler\tcould not allocate resampler context");
    return FALSE;
  }
  av_opt_set_int(swrc, "in_sample_fmt",      AV_SAMPLE_FMT_S16, 0);
  av_opt_set_int(swrc, "in_channel_layout",  MCU_AV_CH_Layout_Selector[srcChannels], 0);
  av_opt_set_int(swrc, "in_sample_rate",     srcSampleRate, 0);
  av_opt_set_int(swrc, "out_sample_fmt",     AV_SAMPLE_FMT_S16, 0);
  av_opt_set_int(swrc, "out_channel_layout", MCU_AV_CH_Layout_Selector[dstChannels], 0);
  av_opt_set_int(swrc, "out_sample_rate",    dstSampleRate, 0);
  int ret = avresample_open(swrc);
  if(ret < 0)
  {
    PTRACE(1, "AudioResampler\tfailed to initialize the resampling context: " << ret << " " << AVErrorToString(ret));
    return FALSE;
  }
#elif USE_LIBSAMPLERATE
  swrc = src_new(SRC_LINEAR, 1, NULL);
#endif
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void AudioResampler::Resample(const BYTE * src, int srcBytes, BYTE * dst, int dstBytes)
{
  if(srcSampleRate == dstSampleRate && srcChannels == dstChannels)
  {
    memcpy(dst, src, srcBytes);
    return;
  }
#if USE_SWRESAMPLE
  void * from = (void*)src;
  void * to = (void*)dst;
  int srcSamples = (srcBytes>>1)/srcChannels;
  int dstSamples = (dstBytes>>1)/dstChannels;

  int ret = swr_convert(swrc, (uint8_t **)&to, dstSamples, (const uint8_t **)&from, srcSamples);
  if(ret < 0)
  {
    PTRACE(1, "AudioResampler\terror while converting: " << ret << " " << AVErrorToString(ret));
    memset(dst, 0, dstBytes);
  }
#elif USE_AVRESAMPLE
  void * from = (void*)src;
  void * to = (void*)dst;
  int srcSamples = (srcBytes>>1)/srcChannels;
  int dstSamples = (dstBytes>>1)/dstChannels;

  int ret = avresample_convert(swrc, (uint8_t **)&to, dstBytes, dstSamples, (uint8_t **)&from, srcBytes, srcSamples);
  if(ret < 0)
  {
    PTRACE(1, "AudioResampler\terror while converting: " << ret << " " << AVErrorToString(ret));
    memset(dst, 0, dstBytes);
  }
#elif USE_LIBSAMPLERATE
  SRC_DATA src_data;
  long in_samples = (srcBytes>>1)/srcChannels;
  long out_samples = (dstBytes>>1)/dstChannels;
  float data_in[in_samples*sizeof(float)];
  float data_out[out_samples*sizeof(float)];
  src_short_to_float_array((const short *)src, data_in, in_samples);

  src_data.data_in = data_in;
  src_data.input_frames = in_samples;
  src_data.data_out = data_out;
  src_data.output_frames = out_samples;
  src_data.src_ratio = (double)out_samples/(double)in_samples;

  int err = src_process(swrc, &src_data);
  if(err)
  {
    PTRACE(1, "AudioResampler\terror while converting: " << src_strerror(err));
    memset(dst, 0, dstBytes);
    return;
  }
  src_float_to_short_array(data_out, (short *)dst, src_data.output_frames_gen);
#else
  InternalResample(src, srcBytes, srcSampleRate, srcChannels, dst, dstBytes, dstSampleRate, dstChannels);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void AudioResampler::InternalResample(const BYTE * src, int srcBytes, int srcSampleRate, int srcChannels, BYTE * dst, int dstBytes, int dstSampleRate, int dstChannels)
{
  if(srcChannels == dstChannels && srcChannels == 1)
  {
    for(int i=0;i<(dstBytes>>1);i++) ((short*)(dst))[i] = ((short*)src)[i*srcSampleRate/dstSampleRate];
    return;
  }
  if(srcChannels == dstChannels)
  {
    for(int i=0;i<((dstBytes>>1)/dstChannels);i++)
    {
      unsigned ofs=(i*srcSampleRate/dstSampleRate)*srcChannels;
      for(int j=0;j<srcChannels;j++) ((short*)(dst))[i*srcChannels+j] = ((short*)src)[ofs+j];
    }
    return;
  }
  for(int i=0;i<(dstBytes>>1)/dstChannels;i++)
  {
    int srcChan=0;
    unsigned ofs=(i*srcSampleRate/dstSampleRate)*srcChannels;
    for(int j=0;j<dstChannels;j++)
    {
      ((short*)(dst))[i*dstChannels+j] = ((short*)src)[ofs+srcChan];
      srcChan++; if(srcChan>=srcChannels) srcChan=0;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
