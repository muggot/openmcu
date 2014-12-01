
#include <ptlib.h>
#include <stdio.h>
#include <string.h>

#include "conference.h"
#include "mcu.h"

#if MCU_VIDEO
#include <ptlib/vconvert.h>
#endif

// size of a PCM data packet, in samples
#define PCM_BUFFER_LEN_MS /*ms */ 120

// number of PCM buffers to keep
#define PCM_BUFFER_COUNT        2

#define PCM_BUFFER_SIZE_CALC(freq,chans)\
  bufferSize = 2/* bytes*/ * chans * PCM_BUFFER_LEN_MS * PCM_BUFFER_COUNT * freq / 1000;\
  if(bufferSize < 4) bufferSize=200;\
  buffer.SetSize(bufferSize + 16);

////////////////////////////////////////////////////////////////////////////////////

ConferenceManager::ConferenceManager()
{
  maxConferenceCount = 0;
  monitor  = new ConferenceMonitor(*this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceManager::~ConferenceManager()
{
  monitor->running = FALSE;
  monitor->WaitForTermination();
  delete monitor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithLock(const OpalGloballyUniqueID & conferenceID)
{
  PWaitAndSignal m(conferenceListMutex);
  Conference *conference = FindConferenceWithoutLock(conferenceID);
  if(conference)
    conference->Lock();
  return conference;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithoutLock(const OpalGloballyUniqueID & conferenceID)
{
  PWaitAndSignal m(conferenceListMutex);
  ConferenceListType::const_iterator r = conferenceList.find(conferenceID);
  if(r != conferenceList.end())
    return r->second;
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithLock(const PString & room)
{
  if(room.IsEmpty())
    return NULL;
  PWaitAndSignal m(conferenceListMutex);
  Conference *conference = FindConferenceWithoutLock(room);
  if(conference)
    conference->Lock();
  return conference;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithoutLock(const PString & room)
{
  if(room.IsEmpty())
    return NULL;
  PWaitAndSignal m(conferenceListMutex);
  for(ConferenceListType::const_iterator r = conferenceList.begin(); r != conferenceList.end(); ++r)
  {
    if(r->second->GetNumber() == room)
      return r->second;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::FindConferenceWithLock(Conference * conference)
{
  if(conference == NULL)
    return FALSE;
  PWaitAndSignal m(conferenceListMutex);
  for(ConferenceListType::const_iterator r = conferenceList.begin(); r != conferenceList.end(); ++r)
  {
    if(r->second == conference)
    {
      conference->Lock();
      return conference;
    }
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::MakeConferenceWithLock(const PString & room, PString name)
{
  PWaitAndSignal m(conferenceListMutex);
  Conference * conference = MakeConferenceWithoutLock(room, name);
  if(conference)
    conference->Lock();
  return conference;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::MakeConferenceWithoutLock(const PString & room, PString name)
{
  PWaitAndSignal m(conferenceListMutex);
  Conference * conference = FindConferenceWithoutLock(room);
  if(conference == NULL)
  {
    // create the conference
    OpalGloballyUniqueID conferenceID;
    conference = CreateConference(conferenceID, room, name, mcuNumberMap.GetNumber(conferenceID));
    // insert conference into the map
    conferenceList.insert(ConferenceListType::value_type(conferenceID, conference));
    // set the conference count
    maxConferenceCount = PMAX(maxConferenceCount, (PINDEX)conferenceList.size());
    OnCreateConference(conference);
  }
  return conference;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL ConferenceManager::HasConference(const PString & number, OpalGloballyUniqueID & conferenceID)
{
  Conference *conference = FindConferenceWithLock(number);
  if(conference)
  {
    conferenceID = conference->GetID();
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

ConferenceProfile * ConferenceManager::FindProfileWithLock(const PString & roomName, const PString & memberName)
{
  Conference *conference = FindConferenceWithLock(roomName);
  if(conference == NULL)
    return NULL;
  ConferenceProfile *profile = FindProfileWithLock(conference, memberName);
  conference->Unlock();
  return profile;
}
ConferenceProfile * ConferenceManager::FindProfileWithLock(Conference * conference, const PString & memberName)
{
  PWaitAndSignal m(conference->GetProfileListMutex());
  ConferenceProfile *profile = FindProfileWithoutLock(conference, memberName);
  if(profile)
    profile->Lock();
  return profile;
}
ConferenceProfile * ConferenceManager::FindProfileWithoutLock(Conference * conference, const PString & memberName)
{
  PWaitAndSignal m(conference->GetProfileListMutex());
  Conference::ProfileList & profileList = conference->GetProfileList();
  for(Conference::ProfileList::iterator r = profileList.begin(); r != profileList.end(); ++r)
  {
    ConferenceProfile *profile = r->second;
    if(profile->GetName() == memberName)
      return profile;
  }
  return NULL;
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
  PWaitAndSignal m(conference->GetProfileListMutex());
  ConferenceMember *member = FindMemberWithoutLock(conference, memberName);
  if(member)
    member->Lock();
  return member;
}
ConferenceMember * ConferenceManager::FindMemberWithoutLock(Conference * conference, const PString & memberName)
{
  PWaitAndSignal m(conference->GetProfileListMutex());
  Conference::ProfileList & profileList = conference->GetProfileList();
  for(Conference::ProfileList::iterator r = profileList.begin(); r != profileList.end(); ++r)
  {
    ConferenceMember *member = r->second->GetMember();
    if(member && member->GetName() == memberName)
      return member;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceMember * ConferenceManager::FindMemberNameIDWithLock(const PString & roomName, const PString & memberName)
{
  Conference *conference = FindConferenceWithLock(roomName);
  if(conference == NULL)
    return NULL;
  ConferenceMember *member = FindMemberWithLock(conference, memberName);
  conference->Unlock();
  return member;
}
ConferenceMember * ConferenceManager::FindMemberNameIDWithLock(Conference * conference, const PString & memberName)
{
  PWaitAndSignal m(conference->GetProfileListMutex());
  ConferenceMember *member = FindMemberWithoutLock(conference, memberName);
  if(member)
    member->Lock();
  return member;
}
ConferenceMember * ConferenceManager::FindMemberNameIDWithoutLock(Conference * conference, const PString & memberName)
{
  PString memberNameID = MCUURL(memberName).GetMemberNameId();
  PWaitAndSignal m(conference->GetProfileListMutex());
  Conference::ProfileList & profileList = conference->GetProfileList();
  for(Conference::ProfileList::iterator r = profileList.begin(); r != profileList.end(); ++r)
  {
    ConferenceMember *member = r->second->GetMember();
    if(member && member->GetNameID() == memberNameID)
      return member;
  }
  return NULL;
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
  PWaitAndSignal m(conference->GetProfileListMutex());
  ConferenceMember *member = FindMemberWithoutLock(conference, id);
  if(member)
    member->Lock();
  return member;
}
ConferenceMember * ConferenceManager::FindMemberWithoutLock(Conference * conference, long id)
{
  PWaitAndSignal m(conference->GetProfileListMutex());
  Conference::ProfileList & profileList = conference->GetProfileList();
  for(Conference::ProfileList::iterator r = profileList.begin(); r != profileList.end(); ++r)
  {
    ConferenceMember *member = r->second->GetMember();
    if(member && (long)member->GetID() == id)
      return member;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCUSimpleVideoMixer * ConferenceManager::FindMixerWithLock(const PString & roomName, long id)
{
  Conference *conference = FindConferenceWithLock(roomName);
  if(conference == NULL)
    return NULL;
  MCUSimpleVideoMixer * mixer = FindMixerWithLock(conference, id);
  conference->Unlock();
  return mixer;
}
MCUSimpleVideoMixer * ConferenceManager::FindMixerWithLock(Conference * conference, long id)
{
  if(conference->videoMixerList)
  {
    PWaitAndSignal m(conference->videoMixerListMutex);
    MCUSimpleVideoMixer *mixer = FindMixerWithoutLock(conference, id);
    if(mixer)
      mixer->Lock();
    return mixer;
  } else {
    PWaitAndSignal m(conference->GetProfileListMutex());
    MCUSimpleVideoMixer *mixer = FindMixerWithoutLock(conference, id);
    if(mixer)
      mixer->Lock();
    return mixer;
  }
  return NULL;
}
MCUSimpleVideoMixer * ConferenceManager::FindMixerWithoutLock(Conference * conference, long id)
{
  if(conference->videoMixerList)
  {
    PWaitAndSignal m(conference->videoMixerListMutex);
    MCUVideoMixer *_mixer = conference->VMLFind(id);
    MCUSimpleVideoMixer *mixer = dynamic_cast<MCUSimpleVideoMixer *>(_mixer);
    return mixer;
  } else {
    ConferenceMember *member = FindMemberWithLock(conference, id);
    if(member)
    {
      MCUVideoMixer *_mixer = member->videoMixer;
      MCUSimpleVideoMixer *mixer = dynamic_cast<MCUSimpleVideoMixer *>(_mixer);
      member->Unlock();
      return mixer;
    }
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceManager::OnCreateConference(Conference * conference)
{
  // add monitor event
  monitor->AddMonitorEvent(new ConferenceStatusInfo(conference->GetID()));
  monitor->AddMonitorEvent(new ConferenceRecorderInfo(conference->GetID()));
  int timeLimit = GetConferenceParam(conference->GetNumber(), RoomTimeLimitKey, 0);
  if(timeLimit > 0)
    monitor->AddMonitorEvent(new ConferenceTimeLimitInfo(conference->GetID(), PTime() + timeLimit*1000));

  if(MCUConfig("Export Parameters").GetBoolean("Enable export", FALSE) == TRUE)
    conference->pipeMember = new ConferencePipeMember(conference);

  // add file recorder member
  if(GetConferenceParam(conference->GetNumber(), RoomAllowRecordKey, TRUE))
  {
    conference->conferenceRecorder = new ConferenceRecorder(conference);
  }

  if(!conference->GetForceScreenSplit())
  { PTRACE(1,"Conference\tOnCreateConference: \"Force split screen video\" unchecked, " << conference->GetNumber() << " skipping members.conf"); return; }

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

  PString jsName(number);
  jsName.Replace("\"","\\x27",TRUE,0); jsName.Replace("'","\\x22",TRUE,0);
  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(1,'" + jsName + "')", number);

  if(conference->conferenceRecorder)
  {
    conference->StopRecorder();
    delete conference->conferenceRecorder;
  }

  PTRACE(2,"MCU\tOnDestroyConference " << number <<", clearing profile list");
  conference->GetProfileListMutex().Wait();
  for(Conference::ProfileList::iterator r = conference->GetProfileList().begin(); r != conference->GetProfileList().end(); )
  {
    ConferenceProfile *profile = r->second;
    conference->GetProfileList().erase(r++);
    profile->Lock();
    delete profile;
  }
  conference->GetProfileListMutex().Signal();

  PTRACE(2,"MCU\tOnDestroyConference " << number <<", disconnect remote endpoints");
  conference->GetMemberListMutex().Wait();
  for(Conference::MemberList::iterator r = conference->GetMemberList().begin(); r != conference->GetMemberList().end(); ++r)
  {
    ConferenceMember * member = r->second;
    if(member)
      member->Close();
  }
  conference->GetMemberListMutex().Signal();

  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(2,'" + jsName + "')", number);

  PTRACE(2,"MCU\tOnDestroyConference " << number <<", waiting...");
  for(PINDEX i = 0; i < 100; i++)
  {
    if(conference->GetMemberList().size() == 0)
      break;
    PThread::Sleep(100);
  }

  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(3,'" + jsName + "')", number);

  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(5,'" + jsName + "')", number);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

Conference * ConferenceManager::CreateConference(const OpalGloballyUniqueID & _guid,
                                                              const PString & _number,
                                                              const PString & _name,
                                                              int _mcuNumber)
{
#if MCU_VIDEO
#  if ENABLE_ECHO_MIXER
     if(_number.Left(4) *= "echo")
       return new Conference(*this, _guid, "echo"+_guid.AsString(), _name, _mcuNumber, new EchoVideoMixer());
#  endif
#  if ENABLE_TEST_ROOMS
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
         return new Conference(*this, _guid, number, _name, _mcuNumber, new TestVideoMixer(count));
     }
#  endif
#endif

  BOOL forceScreenSplit = GetConferenceParam(_number, ForceSplitVideoKey, TRUE);
  //BOOL forceScreenSplit = TRUE;

  if(!forceScreenSplit)
  {
    Conference *conference = new Conference(*this, _guid, _number, _name, _mcuNumber, NULL);
    return conference;
  }

  PINDEX slashPos = _number.Find('/');
  PString number;
  if (slashPos != P_MAX_INDEX) number=_number.Left(slashPos);
  else number=_number;

  return new Conference(*this, _guid, number, _name, _mcuNumber
#if MCU_VIDEO
                        ,OpenMCU::Current().CreateVideoMixer(forceScreenSplit)
#endif
                        ); 
}

void ConferenceManager::RemoveConference(const OpalGloballyUniqueID & confId)
{
  PWaitAndSignal m(conferenceListMutex);
  ConferenceListType::iterator r = conferenceList.find(confId);
  if(r != conferenceList.end())
  {
    Conference * conf = r->second;
    conf->Lock();
    conferenceList.erase(r);
    mcuNumberMap.RemoveNumber(conf->GetMCUNumber());
    OnDestroyConference(conf);
    //monitor->RemoveForConference(conf->GetID());
    delete conf;
    PTRACE(1, "RemoveConference");
  }
}

void ConferenceManager::ClearConferenceList()
{
  PWaitAndSignal m(conferenceListMutex);
  for(ConferenceListType::iterator r = conferenceList.begin(); r != conferenceList.end(); )
  {
    Conference * conf = r->second;
    conf->Lock();
    conferenceList.erase(r++);
    mcuNumberMap.RemoveNumber(conf->GetMCUNumber());
    OnDestroyConference(conf);
    delete conf;
    PTRACE(1, "RemoveConference");
  }
}

void ConferenceManager::ClearMonitorEvents()
{
  PWaitAndSignal m(conferenceListMutex);
  for(ConferenceListType::iterator r = conferenceList.begin(); r != conferenceList.end(); ++r)
    monitor->RemoveForConference(r->second->GetID());
}

void ConferenceManager::AddMonitorEvent(ConferenceMonitorInfo * info)
{
  monitor->AddMonitorEvent(info);
}

///////////////////////////////////////////////////////////////

void ConferenceMonitor::Main()
{
  running = TRUE;

  for (;;) {

    if (!running)
      break;

    Sleep(1000);

    if (!running)
      break;

    PWaitAndSignal m(mutex);

    PTime now;
    MonitorInfoList::iterator r = monitorList.begin();
    while (r != monitorList.end()) {
      ConferenceMonitorInfo & info = **r;
      if (now < info.timeToPerform)
        ++r;
      else {
        BOOL deleteAfterPerform = TRUE;
        {
          PWaitAndSignal m2(manager.GetConferenceListMutex());
          ConferenceListType & confList = manager.GetConferenceList();
          ConferenceListType::iterator s = confList.find(info.guid);
          if (s != confList.end())
            deleteAfterPerform = info.Perform(*s->second);
        }
        if (!deleteAfterPerform)
          ++r;
        else {
          delete *r;
          monitorList.erase(r);
          r = monitorList.begin();
        }
      }
    }
  }
}

void ConferenceMonitor::AddMonitorEvent(ConferenceMonitorInfo * info)
{
  PWaitAndSignal m(mutex);
  monitorList.push_back(info);
}

void ConferenceMonitor::RemoveForConference(const OpalGloballyUniqueID & guid)
{
  PWaitAndSignal m(mutex);
  MonitorInfoList::iterator r = monitorList.begin();
  while (r != monitorList.end()) {
    ConferenceMonitorInfo & info = **r;
    if (info.guid != guid)
      ++r;
    else {
      delete *r;
      monitorList.erase(r);
      r = monitorList.begin();
    }
  }
}

BOOL ConferenceTimeLimitInfo::Perform(Conference & conference)
{
  ConferenceManager & cm = OpenMCU::Current().GetEndpoint().GetConferenceManager();
  cm.RemoveConference(conference.GetID());
  return FALSE;
}

BOOL ConferenceRepeatingInfo::Perform(Conference & conference)
{
  this->timeToPerform = PTime() + repeatTime;
  return FALSE;
}

BOOL ConferenceStatusInfo::Perform(Conference & conference)
{
  // auto delete empty room
  BOOL autoDeleteEmpty = GetConferenceParam(conference.GetNumber(), RoomAutoDeleteEmptyKey, FALSE);
  if(autoDeleteEmpty && !conference.GetVisibleMemberCount())
  {
    ConferenceManager & cm = OpenMCU::Current().GetEndpoint().GetConferenceManager();
    cm.RemoveConference(conference.GetID());
    return TRUE; // delete monitor
  }

  return ConferenceRepeatingInfo::Perform(conference);
}

BOOL ConferenceRecorderInfo::Perform(Conference & conference)
{
  // recorder
  BOOL allowRecord = GetConferenceParam(conference.GetNumber(), RoomAllowRecordKey, TRUE);
  if(!allowRecord)
  {
    conference.StopRecorder();
    return TRUE; // delete monitor
  }

  PString autoRecordStart = GetConferenceParam(conference.GetNumber(), RoomAutoRecordStartKey, "Disable");
  PString autoRecordStop = GetConferenceParam(conference.GetNumber(), RoomAutoRecordStopKey, "Disable");

  PINDEX visibleMembers = conference.GetVisibleMemberCount();

  if(autoRecordStop != "Disable" && visibleMembers <= autoRecordStop.AsInteger())
    conference.StopRecorder();
  else if(autoRecordStart != "Disable" && autoRecordStart.AsInteger() > autoRecordStop.AsInteger() && visibleMembers >= autoRecordStart.AsInteger())
    conference.StartRecorder();

  return ConferenceRepeatingInfo::Perform(conference);
}

BOOL ConferenceMCUCheckInfo::Perform(Conference & conference)
{
  // see if any member of this conference is a not an MCU
  Conference::MemberList & list = conference.GetMemberList();
  Conference::MemberList::iterator r;
  for (r = list.begin(); r != list.end(); ++r)
    if (r->second->GetType() != MEMBER_TYPE_MCU)
      break;

  // if there is any non-MCU member, check again later
  if (r != list.end())
    return ConferenceRepeatingInfo::Perform(conference);

  // else shut down the conference
  for (r = list.begin(); r != list.end(); ++r)
    r->second->Close();

  return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////

Conference::Conference(        ConferenceManager & _manager,
                       const OpalGloballyUniqueID & _guid,
                                    const PString & _number,
                                    const PString & _name,
                                                int _mcuNumber
#if MCU_VIDEO
                                    ,MCUVideoMixer * _videoMixer
#endif
)
  : manager(_manager), guid(_guid), number(_number), name(_name), mcuNumber(_mcuNumber), mcuMonitorRunning(FALSE)
{
  stopping = FALSE;
#if MCU_VIDEO
  VMLInit(_videoMixer);
#endif
  visibleMemberCount = 0;
  maxMemberCount = 0;
  moderated = FALSE;
  muteUnvisible = FALSE;
  VAdelay = 1000;
  VAtimeout = 10000;
  VAlevel = 100;
  echoLevel = 0;
  vidmembernum = 0;
  conferenceRecorder = NULL;
  forceScreenSplit = GetConferenceParam(number, ForceSplitVideoKey, TRUE);
  lockedTemplate = GetConferenceParam(number, LockTemplateKey, FALSE);
  pipeMember = NULL;
  PTRACE(3, "Conference\tNew conference started: ID=" << guid << ", number = " << number);
}

Conference::~Conference()
{
#if MCU_VIDEO
  VMLClear();
#endif
}

BOOL Conference::RecorderCheckSpace()
{
  PDirectory pd(OpenMCU::Current().vr_ffmpegDir);
  PInt64 t, f;
  DWORD cs;
  if(!pd.GetVolumeSpace(t, f, cs))
  {
    PTRACE(1,"EVRT\tRecorder space check failed");
    return TRUE;
  }
  BOOL result = ((f>>20) >= OpenMCU::Current().vr_minimumSpaceMiB);
  if(!result) OpenMCU::Current().HttpWriteEvent("<b><font color='red'>Insufficient disk space</font>: Video Recorder DISABLED</b>");
  PTRACE_IF(1,!result,"EVRT\tInsufficient disk space: Video Recorder DISABLED");
  return result;
}

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
    PTRACE(1,"EVRT\tRecorder failed to start (check recorder directory)");
    OpenMCU::Current().HttpWriteEventRoom("Recorder failed to start (check recorder directory)", number);
    return FALSE;
  }

  conferenceRecorder->Start();
  for(int i = 0; i < 10; i++) if(conferenceRecorder->IsRunning()) break; else PThread::Sleep(100);
  if(!conferenceRecorder->IsRunning())
  {
    PTRACE(1,"MCU\tConference: " << number <<", failed to start recorder");
    return FALSE;
  }

  PTRACE(1,"MCU\tConference: " << number <<", video recorder started");
  OpenMCU::Current().HttpWriteEventRoom("video recording started", number);
  OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*this), number);
  OpenMCU::Current().HttpWriteCmdRoom("build_page()", number);
  return TRUE;
}

BOOL Conference::StopRecorder()
{
  if(!conferenceRecorder)
    return TRUE;
  if(!conferenceRecorder->IsRunning())
    return TRUE;
  conferenceRecorder->Stop();

  PTRACE(1,"MCU\tConference: " << number <<", video recorder stopped");
  OpenMCU::Current().HttpWriteEventRoom("video recording stopped", number);
  OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*this), number);
  OpenMCU::Current().HttpWriteCmdRoom("build_page()", number);
  return TRUE;
}

void Conference::AddMonitorEvent(ConferenceMonitorInfo * info)
{ 
  manager.AddMonitorEvent(info); 
}

void Conference::RefreshAddressBook()
{
  PStringArray abook = OpenMCU::Current().GetRegistrar()->GetAccountList();
  PStringStream msg;
  msg = "addressbook=Array(";
  for(PINDEX i = 0; i < abook.GetSize(); i++)
  {
    PString memberName = abook[i].Tokenise(",")[0];
    PString memberNameId = MCUURL(memberName).GetMemberNameId();
    PString abook_enable = abook[i].Tokenise(",")[1];
    PString remote_application = abook[i].Tokenise(",")[2];
    PString reg_state = abook[i].Tokenise(",")[3];
    PString reg_info = abook[i].Tokenise(",")[4];
    PString conn_state = abook[i].Tokenise(",")[5];
    PString conn_info = abook[i].Tokenise(",")[6];
    PString ping_state = abook[i].Tokenise(",")[7];
    PString ping_info = abook[i].Tokenise(",")[8];
    if(i>0) msg << ",";
    memberName.Replace("&","&amp;",TRUE,0);
    memberName.Replace("\"","&quot;",TRUE,0);
    msg << "Array("
        << "0"
        << ",\"" << memberNameId << "\""
        << ",\"" << memberName << "\""
        << ",\"" << abook_enable << "\""
        << ",\"" << remote_application << "\""
        << ",\"" << reg_state << "\""
        << ",\"" << reg_info << "\""
        << ",\"" << conn_state << "\""
        << ",\"" << conn_info << "\""
        << ",\"" << ping_state << "\""
        << ",\"" << ping_info << "\""
        << ")";
  }
  msg << ");p.abr()";
  OpenMCU::Current().HttpWriteCmdRoom(msg,number);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::AddMemberToList(const PString & name, ConferenceMember *member)
{
  // memberList
  if(member)
  {
    memberListMutex.Wait();
    MemberList::iterator r = memberList.find(member->GetID());
    if(r == memberList.end())
      memberList.insert(MemberList::value_type(member->GetID(), member));
    memberListMutex.Signal();
  }

  // memberProfileList
  profileListMutex.Wait();
  PString nameID = MCUURL(name).GetMemberNameId();
  ConferenceProfile * profile = NULL;
  for(ProfileList::iterator r = profileList.begin(); r != profileList.end(); ++r)
  {
    if(r->second->GetMember())
      continue;
    if(r->second->GetNameID() == nameID)
    {
      profile = r->second;
      profile->SetMember(member);
      break;
    }
  }
  if(profile == NULL)
  {
    profile = new ConferenceProfile(this, name);
    profile->SetMember(member);
    profileList.insert(ProfileList::value_type((ConferenceMemberId)profile, profile));
  }
  profileListMutex.Signal();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::RemoveMemberFromList(const PString & name, ConferenceMember *member)
{
  // memberList
  if(member)
  {
    memberListMutex.Wait();
    memberList.erase(member->GetID());
    memberListMutex.Signal();
  }

  // profileList
  profileListMutex.Wait();
  for(ProfileList::iterator r = profileList.begin(); r != profileList.end(); )
  {
    ConferenceProfile *profile = r->second;
    profile->Lock();
    if(profile->GetName() == name && profile->GetMember() == member)
    {
      if(member && !member->GetType() & MEMBER_TYPE_GSYSTEM)
      {
        profile->SetMember(NULL);
      } else {
        profileList.erase(r++);
        delete profile;
        continue;
      }
    }
    profile->Unlock();
    ++r;
  }
  profileListMutex.Signal();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::AddMember(ConferenceMember * memberToAdd)
{

  PTRACE(3, "Conference\tAbout to add member " << memberToAdd->GetTitle() << " to conference " << guid);

  // see if the callback refuses the new member (always true)
  if (!BeforeMemberJoining(memberToAdd))
    return FALSE;

  memberToAdd->SetName();

  // lock the member lists
  PWaitAndSignal m(memberListMutex);
  PWaitAndSignal m2(profileListMutex);

  // check for duplicate name or very fast reconnect
  if(!memberToAdd->GetType() & MEMBER_TYPE_GSYSTEM)
  {
    // check for duplicate name or very fast reconnect
    PString memberName = memberToAdd->GetName();
    for(PINDEX i = 0; manager.FindMemberWithoutLock(this, memberToAdd->GetName()) != NULL; i++)
    {
      if(MCUConfig("Parameters").GetBoolean(RejectDuplicateNameKey, FALSE))
      {
        PString username = memberToAdd->GetName();
        username.Replace("&","&amp;",TRUE,0);
        username.Replace("\"","&quot;",TRUE,0);
        PStringStream msg;
        msg << username << " REJECTED - DUPLICATE NAME";
        OpenMCU::Current().HttpWriteEventRoom(msg, number);
        return FALSE;
      }
      memberToAdd->SetName(memberName+" ##"+PString(i+2));
    }
  }

#if MCU_VIDEO
  if (!UseSameVideoForAllMembers())
    memberToAdd->videoMixer = new MCUSimpleVideoMixer();
#endif

  PTRACE(3, "Conference\tAdding member: " << memberToAdd->GetName() << " to conference: " << guid);
  cout << "Conference Adding member: " << memberToAdd->GetName() << " to conference: " << guid << "\n";

  ConferenceMemberId mid = memberToAdd->GetID();
  MemberList::iterator r = memberList.find(mid);
  if(r != memberList.end())
    return FALSE;

#if MCU_VIDEO
  if(moderated==FALSE
#  if ENABLE_TEST_ROOMS
     || number=="testroom"
#  endif
    )
  {
    if(UseSameVideoForAllMembers() && memberToAdd->IsVisible())
    {
      videoMixerListMutex.Wait();
      if(!videoMixerList->GetMixer()->AddVideoSource(mid, *memberToAdd))
        memberToAdd->SetFreezeVideo(TRUE);
      videoMixerListMutex.Signal();
      PTRACE(3, "Conference\tUseSameVideoForAllMembers ");
    }
  }
  else
    memberToAdd->SetFreezeVideo(TRUE);
#endif

  // add to all lists
  AddMemberToList(memberToAdd->GetName(), memberToAdd);

  if(memberToAdd->IsVisible())
    visibleMemberCount++;

  // for H.323
  int tid = terminalNumberMap.GetNumber(memberToAdd->GetID());
  memberToAdd->SetTerminalNumber(tid);

  // make sure each member has a connection created for the new member
  // make sure the new member has a connection created for each existing member
  for(MemberList::iterator r = memberList.begin(); r != memberList.end(); r++)
  {
    ConferenceMember * conn = r->second;
    if(conn != memberToAdd)
    {
#if MCU_VIDEO
      if(moderated==FALSE
#  if ENABLE_TEST_ROOMS
         || number == "testroom"
#  endif
        )
      {
        if(!UseSameVideoForAllMembers())
        {
          if(conn->IsVisible())
            memberToAdd->AddVideoSource(conn->GetID(), *conn);
          if(memberToAdd->IsVisible())
            conn->AddVideoSource(memberToAdd->GetID(), *memberToAdd);
        }
#endif
      }
    }
  }

  // update the statistics
  if(memberToAdd->IsVisible())
  {
    maxMemberCount = PMAX(maxMemberCount, visibleMemberCount);
    // trigger H245 thread for join message
    //new NotifyH245Thread(*this, TRUE, memberToAdd);
  }

  // notify that member is joined
  memberToAdd->SetJoined(TRUE);

  // call the callback function
  OnMemberJoining(memberToAdd);

  // monitor
  if(memberToAdd->GetType() == MEMBER_TYPE_MCU && !mcuMonitorRunning)
  {
    manager.AddMonitorEvent(new ConferenceMCUCheckInfo(GetID(), 1000));
    mcuMonitorRunning = TRUE;
  }

  PStringStream msg;

  // add this member to the conference member name list
  if(!memberToAdd->GetType() & MEMBER_TYPE_GSYSTEM)
  {
    PullMemberOptionsFromTemplate(memberToAdd, confTpl);

    PString username=memberToAdd->GetName(); username.Replace("&","&amp;",TRUE,0); username.Replace("\"","&quot;",TRUE,0);
    msg="addmmbr(1";
    msg << "," << (long)memberToAdd->GetID()
        << ",\"" << username << "\""
        << "," << memberToAdd->muteMask
        << "," << memberToAdd->disableVAD
        << "," << memberToAdd->chosenVan
        << "," << memberToAdd->GetAudioLevel()
        << "," << memberToAdd->GetVideoMixerNumber()
        << ",\"" << memberToAdd->GetNameID() << "\""
        << "," << dec << (unsigned)memberToAdd->channelCheck
        << "," << memberToAdd->kManualGainDB
        << "," << memberToAdd->kOutputGainDB
        << ")";
    OpenMCU::Current().HttpWriteCmdRoom(msg,number);
  }

  msg = "<font color=green><b>+</b>";
  msg << memberToAdd->GetName() << "</font>"; OpenMCU::Current().HttpWriteEventRoom(msg,number);
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::RemoveMember(ConferenceMember * memberToRemove)
{
  if(memberToRemove == NULL) return TRUE;

  // lock memberList
  PWaitAndSignal m(memberListMutex);
  PWaitAndSignal m2(profileListMutex);

  // lock conferenceMember
  memberToRemove->Lock();

  PString username = memberToRemove->GetName();

  if(!memberToRemove->IsJoined())
  {
    PTRACE(4, "Conference\tNo need to remove call " << username << " from conference " << guid);
    return (memberList.size() == 0);
  }

  ConferenceMemberId userid = memberToRemove->GetID();

  // add this member to the conference member name list with zero id

  PTRACE(3, "Conference\tRemoving call " << username << " from conference " << guid << " with size " << (PINDEX)memberList.size());
  cout << username << " leaving conference " << number << "(" << guid << ")" << endl;


  BOOL closeConference;
  {

    PStringStream msg; msg << "<font color=red><b>-</b>" << username << "</font>"; OpenMCU::Current().HttpWriteEventRoom(msg,number);
    username.Replace("&","&amp;",TRUE,0); username.Replace("\"","&quot;",TRUE,0);
    msg="remmmbr(0";
    msg << ","  << (long)userid
        << ",\"" << username << "\""
        << ","  << memberToRemove->muteMask
        << "," << memberToRemove->disableVAD
        << ","  << memberToRemove->chosenVan
        << ","  << memberToRemove->GetAudioLevel()
        << ",\"" << MCUURL(memberToRemove->GetName()).GetMemberNameId() << "\"";
    msg << ",0";
    msg << ")";
    OpenMCU::Current().HttpWriteCmdRoom(msg,number);

    // remove from all lists
    RemoveMemberFromList(memberToRemove->GetName(), memberToRemove);

    if(memberToRemove->IsVisible())
      visibleMemberCount--;

    memberToRemove->RemoveAllConnections();

    // remove ConferenceConnection
    RemoveAudioConnection(memberToRemove);

    MemberList::iterator r;
    // remove this member from the connection lists for all other members
    for (r = memberList.begin(); r != memberList.end(); r++) {
      ConferenceMember * conn = r->second;
      if(conn != NULL)
      if (conn != memberToRemove) {
#if MCU_VIDEO
        if (!UseSameVideoForAllMembers()) {
          if (memberToRemove->IsVisible())
            conn->RemoveVideoSource(userid, *memberToRemove);
          if (conn->IsVisible())
            memberToRemove->RemoveVideoSource(conn->GetID(), *conn);
        }
#endif
      }
    }

#if MCU_VIDEO
    if (moderated==FALSE
#  if ENABLE_TEST_ROOMS
    || number == "testroom"
#  endif
    )
    { if (UseSameVideoForAllMembers())
      if (memberToRemove->IsVisible())
      {
        PWaitAndSignal m(videoMixerListMutex);
        videoMixerList->GetMixer()->RemoveVideoSource(userid, *memberToRemove);
      }
    }
    else
    {
      PWaitAndSignal m(videoMixerListMutex);
      VideoMixerRecord * vmr=videoMixerList; while(vmr!=NULL)
      {
        vmr->GetMixer()->MyRemoveVideoSourceById(userid,FALSE);
        vmr=vmr->next;
      }
    }
#endif

    // trigger H245 thread for leave message
//    if (memberToRemove->IsVisible())
//      new NotifyH245Thread(*this, FALSE, memberToRemove);

    terminalNumberMap.RemoveNumber(memberToRemove->GetTerminalNumber());


    // return TRUE if conference is empty 
    closeConference = GetVisibleMemberCount() == 0;
  }

  // notify that member is not joined anymore
  memberToRemove->SetJoined(FALSE);

  // call the callback function
  OnMemberLeaving(memberToRemove);

  return closeConference;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceAudioConnection * Conference::AddAudioConnection(ConferenceMember * member, unsigned sampleRate, unsigned channels)
{
  if(member->GetType() & MEMBER_TYPE_GSYSTEM)
    return NULL;
  ConferenceAudioConnection * conn = new ConferenceAudioConnection(member->GetID(), sampleRate, channels);
  audioConnectionList.Append((long)member->GetID(), conn);
  return conn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::RemoveAudioConnection(ConferenceMember * member)
{
  ConferenceAudioConnection * conn = (ConferenceAudioConnection *)audioConnectionList((long)member->GetID());
  audioConnectionList.Remove((long)member->GetID());
  PThread::Sleep(10);
  delete conn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::ReadMemberAudio(ConferenceMember * member, void * buffer, int amount, int sampleRate, int channels)
{
  for(int i = 0; i < audioConnectionList.GetSize(); ++i)
  {
    ConferenceAudioConnection * conn = (ConferenceAudioConnection *)audioConnectionList[i];
    if(conn == NULL || conn->GetID() == member->GetID())
      continue;
    BOOL skip = moderated&&muteUnvisible;
    if(skip)
    {
      PWaitAndSignal m(videoMixerListMutex);
      VideoMixerRecord * vmr = videoMixerList;
      while(vmr != NULL)
      {
        if(vmr->GetMixer()->GetPositionStatus(member->GetID())>=0)
        {
          skip = FALSE;
          break;
        }
        else
          vmr=vmr->next;
      }
    }
    if(!skip) // default behaviour
    {
      conn->ReadAudio(member, (BYTE *)buffer, amount, sampleRate, channels);
      member->ReadAudioOutputGain(buffer, amount);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::WriteMemberAudio(ConferenceMember * member, const void * buffer, int amount, int sampleRate, int channels)
{
  ConferenceAudioConnection * conn = (ConferenceAudioConnection *)audioConnectionList((long)member->GetID());
  if(conn && (conn->GetSampleRate() != sampleRate || conn->GetChannels() != channels))
  {
    RemoveAudioConnection(member);
    conn = NULL;
  }
  if(conn == NULL)
    conn = AddAudioConnection(member, sampleRate, channels);
  conn->WriteAudio((const BYTE *)buffer, amount);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// tint - time interval since last call in msec
void Conference::WriteMemberAudioLevel(ConferenceMember * member, int audioLevel, int tint)
{
  member->audioLevelIndicator|=audioLevel;
  if(!((++member->audioCounter)&31)){
    if (member->audioLevelIndicator < 64) member->audioLevelIndicator = 0;
    if((member->previousAudioLevel != member->audioLevelIndicator)||((member->audioLevelIndicator!=0) && ((member->audioCounter&255)==0))){
      PStringStream msg; msg << "audio(" << (long)member->GetID() << "," << member->audioLevelIndicator << ")";
      OpenMCU::Current().HttpWriteCmdRoom(msg,number);
      member->previousAudioLevel=member->audioLevelIndicator;
      member->audioLevelIndicator=audioLevel;
    }
  }
#if MCU_VIDEO
  if (UseSameVideoForAllMembers())
  {
    PWaitAndSignal m(videoMixerListMutex);
    if (videoMixerList != NULL)
    {
      if(audioLevel > VAlevel) member->vad+=tint;
      else member->vad=0;
      VideoMixerRecord * vmr=videoMixerList;
      while(vmr!=NULL)
      {
        MCUVideoMixer * videoMixer = vmr->GetMixer();
        int status = videoMixer->GetPositionStatus(member->GetID());
        if(audioLevel > VAlevel)
        {
//          cout << "audioLevel " << audioLevel << "\n";
          if(member->vad >= VAdelay) // voice-on trigger delay
          {
//            cout << "VAD=" << member->vad << " status=" << status << "\n";
            if(status > 0) videoMixer->SetPositionStatus(member->GetID(),0);
            else if(status == 0 && member->disableVAD == FALSE)
            {
              if(member->vad-VAdelay>500) // execute every 500 ms of voice activity
                videoMixer->SetVAD2Position(member);
            }
            else if(status == -1 && member->disableVAD == FALSE) //find new vad position for active member
            {
              ConferenceMemberId id = videoMixer->SetVADPosition(member,member->chosenVan,VAtimeout);
              if(id!=NULL)
              { FreezeVideo(id); member->SetFreezeVideo(FALSE); }
            }
          }
        }
        else
        {
          if(status >= 0) // increase silence counter
            videoMixer->SetPositionStatus(member->GetID(),status+tint);
        }
        if(audioLevel > VAlevel) if(status==0 && member->disableVAD==FALSE) if(member->vad-VAdelay>500) member->vad=VAdelay;
        vmr=vmr->next;
      }
    }
  }
#endif // MCU_VIDEO
}

////////////////////////////////////////////////////////////////////////////////////////////////////


#if MCU_VIDEO

void Conference::ReadMemberVideo(ConferenceMember * member, void * buffer, int width, int height, PINDEX & amount)
{
  PWaitAndSignal m(videoMixerListMutex);
  if (videoMixerList == NULL)
    return;

// PTRACE(3, "Conference\tReadMemberVideo call 1" << width << "x" << height);
  unsigned mixerNumber; if(member==NULL) mixerNumber=0; else mixerNumber=member->GetVideoMixerNumber();
  MCUVideoMixer * mixer = VMLFind(mixerNumber);

  if(mixer==NULL)
  { if(mixerNumber != 0) mixer = VMLFind(0);
    if(mixer==NULL)
    { PTRACE(3,"Conference\tCould not get video");
      return;
    } else { PTRACE(6,"Conference\tCould not get video mixer " << mixerNumber << ", reading 0 instead"); }
  }

//  if (mixer!=NULL) {
    mixer->ReadFrame(*member, buffer, width, height, amount);
//    return;
//  }

/* commented by kay27 not really understanding what he is doing, 04.09.2013
  // find the other member and copy it's video
  PWaitAndSignal m(memberListMutex);
  MemberList::iterator r;
  for (r = memberList.begin(); r != memberList.end(); r++) {
    if ((r->second != member) && r->second->IsVisible()) {
      void * frameStore = r->second->OnExternalReadVideo(member->GetID(), width, height, amount);  
      if (frameStore != NULL) {
        memcpy(buffer, frameStore, amount);
        r->second->UnlockExternalVideo();
      }
    }
  }
*/
  
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::WriteMemberVideo(ConferenceMember * member, const void * buffer, int width, int height, PINDEX amount)
{
  if (UseSameVideoForAllMembers())
  {
    PWaitAndSignal m(videoMixerListMutex);
    if (videoMixerList != NULL) {
      VideoMixerRecord * vmr = videoMixerList; BOOL writeResult=FALSE;
      while(vmr!=NULL)
      { writeResult |= vmr->GetMixer()->WriteFrame(member->GetID(), buffer, width, height, amount);
        vmr=vmr->next;
      }
      return writeResult;
    }
  }
  else
  {
    PWaitAndSignal m(memberListMutex);
    MemberList::iterator r;
    for (r = memberList.begin(); r != memberList.end(); ++r)
      r->second->OnExternalSendVideo(member->GetID(), buffer, width, height, amount);
  }
  return TRUE;
}

#endif

BOOL Conference::BeforeMemberJoining(ConferenceMember * member)
{ 
  return manager.BeforeMemberJoining(this, member); 
}

void Conference::OnMemberJoining(ConferenceMember * member)
{ 
  manager.OnMemberJoining(this, member); 
}

void Conference::OnMemberLeaving(ConferenceMember * member)
{ 
  manager.OnMemberLeaving(this, member); 
}

void Conference::FreezeVideo(ConferenceMemberId id)
{ 
  int i;
  PWaitAndSignal m(memberListMutex);
  MemberList::iterator r,s;
  if(id!=NULL)
  {
    r = memberList.find(id); if(r == memberList.end()) return;
    PWaitAndSignal m(videoMixerListMutex);
    VideoMixerRecord * vmr = videoMixerList;
    while(vmr!=NULL)
    { i=vmr->GetMixer()->GetPositionStatus(id);
      if(i>=0) {
        r->second->SetFreezeVideo(FALSE);
        return;
      }
      vmr=vmr->next;
    }
    if(!UseSameVideoForAllMembers())for(s=memberList.begin();s!=memberList.end();++s) if(s->second->videoMixer!=NULL) if(s->second->videoMixer->GetPositionStatus(id)>=0) {r->second->SetFreezeVideo(FALSE); return;}
    r->second->SetFreezeVideo(TRUE);
    return;
  }
  for (r = memberList.begin(); r != memberList.end(); r++)
  {
    ConferenceMemberId mid=r->second->GetID();
    PWaitAndSignal m(videoMixerListMutex);
    VideoMixerRecord * vmr = videoMixerList;
    while(vmr!=NULL)
    {
      i=vmr->GetMixer()->GetPositionStatus(mid);
      if(i>=0)
      {
        r->second->SetFreezeVideo(FALSE);
        break;
      }
      vmr=vmr->next;
    }
    if(vmr==NULL)
    { if(!UseSameVideoForAllMembers())
      { for(s=memberList.begin();s!=memberList.end();++s)
        { if(s->second->videoMixer!=NULL) if(s->second->videoMixer->GetPositionStatus(mid)>=0)
          { r->second->SetFreezeVideo(FALSE);
            break;
          }
        }
        if(s==memberList.end()) r->second->SetFreezeVideo(TRUE);
      }
      else r->second->SetFreezeVideo(TRUE);
    }
  }
}

BOOL Conference::PutChosenVan()
{
  BOOL put=FALSE;
  int i;
  PWaitAndSignal m(memberListMutex);
  MemberList::iterator r;
  for (r = memberList.begin(); r != memberList.end(); ++r)
  {
    if(r->second->chosenVan)
    {
      PWaitAndSignal m(videoMixerListMutex);
      VideoMixerRecord * vmr = videoMixerList;
      while(vmr!=NULL){
        i=vmr->GetMixer()->GetPositionStatus(r->second->GetID());
        if(i < 0) put |= (NULL != vmr->GetMixer()->SetVADPosition(r->second,r->second->chosenVan,VAtimeout));
        vmr = vmr->next;
      }
    }
  }
  return put;
}

void Conference::HandleFeatureAccessCode(ConferenceMember & member, PString fac){
  PTRACE(3,"Conference\tHandling feature access code " << fac << " from " << member.GetName());
  PStringArray s = fac.Tokenise("*");
  if(s[0]=="1")
  {
    int posTo=0;
    if(s.GetSize()>1) posTo=s[1].AsInteger();
    PTRACE(4,"Conference\t" << "*1*" << posTo << "#: jump into video position " << posTo);
    if(videoMixerCount==0) return;
    ConferenceMemberId id=member.GetID();
    if(id==NULL) return;

    {
      PWaitAndSignal m(videoMixerListMutex);
      if(videoMixerList==NULL) return;
      if(videoMixerList->GetMixer()==NULL) return;
      int pos=videoMixerList->GetMixer()->GetPositionNum(id);
      if(pos==posTo) return;
      videoMixerList->GetMixer()->InsertVideoSource(&member,posTo);
    }
    FreezeVideo(NULL);

    OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*this),number);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",number);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceProfile::ConferenceProfile(Conference * _conference, PString _name)
{
  conference = _conference;
  SetName(_name);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceProfile::SetName(PString _name)
{
  PWaitAndSignal m(mutex);
  name = _name;
  nameID = MCUURL(name).GetMemberNameId();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceProfile::SetMember(ConferenceMember * _member)
{
  PWaitAndSignal m(mutex);
  member = _member;
  if(member)
    SetName(member->GetName());
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceMember::ConferenceMember(Conference * _conference, ConferenceMemberId _id)
  : conference(_conference), id(_id)
{
  memberType = MEMBER_TYPE_NONE;
  channelCheck=0;
  audioLevel = 0;
  audioCounter = 0; previousAudioLevel = 65535; audioLevelIndicator = 0;
  currVolCoef = 1.0;
  kManualGain = 1.0; kManualGainDB = 0;
  kOutputGain = 1.0; kOutputGainDB = 0;
  terminalNumber = -1;
  memberIsJoined = FALSE;

#if MCU_VIDEO
  videoMixer = NULL;
//  fsConverter = PColourConverter::Create("YUV420P", "YUV420P", CIF4_WIDTH, CIF4_HEIGHT);
//  MCUVideoMixer::FillCIF4YUVFrame(memberFrameStores.GetFrameStore(CIF4_WIDTH, CIF4_HEIGHT).data.GetPointer(), 0, 0, 0);
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
#endif
}

ConferenceMember::~ConferenceMember()
{
  muteMask|=15;

  ClearAudioReaderList(TRUE);

#if MCU_VIDEO
  if(videoMixer)
  {
    videoMixer->FullLock();
    delete videoMixer;
  }
#endif
}

void ConferenceMember::ChannelBrowserStateUpdate(BYTE bitMask, BOOL bitState)
{
  if(bitState)
  {
    channelCheck|=bitMask;
  }
  else
  {
    channelCheck&=~bitMask;
  }

  PStringStream msg;
  msg << "rtp_state(" << dec << (long)id << ", " << (unsigned)channelCheck << ")";
  PTRACE(1,"channelCheck change: " << (bitState?"+":"-") << (unsigned)bitMask << ". Result: " << (unsigned)channelCheck << ", conference=" << conference << ", msg=" << msg);

  if(!conference)
  {
    OpenMCU::Current().HttpWriteCmd(msg);
    return;
  }

  OpenMCU::Current().HttpWriteCmdRoom(msg,conference->GetNumber());
}

void ConferenceMember::RemoveAllConnections()
{
  PTRACE(3, "Conference\tRemoving all members from connection " << id);
}

void AutoGainControl(const short * pcm, unsigned samplesPerFrame, unsigned codecChannels, unsigned sampleRate, unsigned level, float* currVolCoef, unsigned* signalLevel, float kManual)
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
  
  unsigned wLevel;
  if(kManual>10) wLevel = (unsigned)(level*10/kManual); else wLevel=level;

  if((unsigned)c_avg_vol > wLevel)
  {
    if(c_max_vol*cvc >= overload) // есть перегрузка
      cvc = overload / c_max_vol;
    else
    if(c_max_vol*cvc < max_vol) // нужно увеличить усиление
      cvc += inc_vol;
  }
  else // не должен срабатывать, но временно грубая защита от перегрузки:
  {
    if(c_max_vol*cvc >= overload) // есть перегрузка
      cvc = (float)overload / c_max_vol;
  }
  *signalLevel = (unsigned)((float)c_avg_vol*cvc);
  PTRACE(9,"AGC\tavg" << c_avg_vol << " max" << c_max_vol << " vc" << vc0 << ">" << cvc);

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

void ConferenceMember::WriteAudio(const void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels)
{
  if(!(channelCheck&1)) ChannelBrowserStateUpdate(1,TRUE);
  if(muteMask&1) return;

  // calculate average signal level for this member
  unsigned signalLevel=0;
  AutoGainControl((short*) buffer, amount/channels/2, channels, sampleRate, 2000, &currVolCoef, &signalLevel, kManualGain);
  audioLevel = ((signalLevel * 2) + audioLevel) / 3;

  if(conference != NULL)
  {
    conference->WriteMemberAudioLevel(this, audioLevel, amount/32);
    conference->WriteMemberAudio(this, buffer, amount, sampleRate, channels);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::ReadAudio(void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels)
{
  if(!(channelCheck&2)) ChannelBrowserStateUpdate(2,TRUE);

  // First, set the buffer to empty.
  memset(buffer, 0, amount);

  if(muteMask&2) return;

  if(conference != NULL)
    conference->ReadMemberAudio(this, buffer, amount, sampleRate, channels);

  ClearAudioReaderList();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::ReadAudioOutputGain(void * buffer, int amount)
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

void ConferenceMember::ClearAudioReaderList(BOOL force)
{
  PTime now;
  for(AudioReaderListType::iterator it = audioReaderList.begin(); it != audioReaderList.end(); )
  {
    if(force || now - it->second->readTime > 60000) // not used 60 sec
    {
      AudioReader *reader = it->second;
      audioReaderList.erase(it++);
      delete reader;
      continue;
    }
    ++it;
  }
  for(AudioResamplerListType::iterator it = audioResamplerList.begin(); it != audioResamplerList.end(); )
  {
    if(force || now - it->second->readTime > 60000) // not used 60 sec
    {
      AudioResampler *resampler = it->second;
      audioResamplerList.erase(it++);
      delete resampler;
      continue;
    }
    ++it;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#if MCU_VIDEO

// called whenever the connection needs a frame of video to send
void ConferenceMember::ReadVideo(void * buffer, int width, int height, PINDEX & amount)
{
  if(!(channelCheck&8)) ChannelBrowserStateUpdate(8,TRUE);
  if(muteMask&8) return;

  ++totalVideoFramesSent;
  if(!firstFrameSendTime.IsValid())
    firstFrameSendTime = PTime();

  if(lock.Wait())
  {
    if(conference != NULL)
    {
      if(conference->UseSameVideoForAllMembers())
        conference->ReadMemberVideo(this, buffer, width, height, amount);
      else if(videoMixer != NULL)
        videoMixer->ReadFrame(*this, buffer, width, height, amount);
    }
    lock.Signal();
  }
}

// called whenever the connection receives a frame of video
void ConferenceMember::WriteVideo(const void * buffer, int width, int height, PINDEX amount)
{
  if(!(channelCheck&4)) ChannelBrowserStateUpdate(4,TRUE);
  if(muteMask&4) return;

  ++totalVideoFramesReceived;
  rxFrameWidth=width; rxFrameHeight=height;
  if (!firstFrameReceiveTime.IsValid())
    firstFrameReceiveTime = PTime();

  if(lock.Wait())
  {
    if(conference != NULL)
    {
      if(!conference->WriteMemberVideo(this, buffer, width, height, amount))
      {
        PWaitAndSignal m(memberFrameStoreMutex);
        VideoFrameStoreList::FrameStore & fs = memberFrameStores.GetFrameStore(width, height);
        memcpy(fs.data.GetPointer(), buffer, amount);
        memberFrameStores.InvalidateExcept(width, height);
      }
    }
    lock.Signal();
  }
}

void ConferenceMember::OnExternalSendVideo(ConferenceMemberId id, const void * buffer, int width, int height, PINDEX amount)
{
//  if (lock.Wait()) {
    videoMixer->WriteFrame(id, buffer, width, height, amount);
//    lock.Signal();
//  }
}

void * ConferenceMember::OnExternalReadVideo(ConferenceMemberId id, int width, int height, PINDEX & bytesReturned)
{
  if (!lock.Wait())
    return NULL;

  memberFrameStoreMutex.Wait();

  BOOL found;
  VideoFrameStoreList::FrameStore & nearestFs = memberFrameStores.GetNearestFrameStore(width, height, found);

  // if no valid framestores, nothing we can do
/*  if (!found) {
    memberFrameStoreMutex.Signal();
    lock.Signal();
    return NULL;
  }
*/
  // if the valid framestore is a perfect match, return it
  if(found)
  if ((nearestFs.width == width) && (nearestFs.height == height))
    return nearestFs.data.GetPointer();

  // create a new destinationf framestore
  VideoFrameStoreList::FrameStore & destFs = memberFrameStores.GetFrameStore(width, height);

  if(found)
  MCUVideoMixer::ResizeYUV420P(nearestFs.data.GetPointer(), destFs.data.GetPointer(), nearestFs.width, nearestFs.height, width, height);
  else
//  OpenMCU::Current().GetPreMediaFrame(destFs.data.GetPointer(), width, height, bytesReturned);
  MCUVideoMixer::FillYUVFrame(destFs.data.GetPointer(), 0, 0, 0, width, height);
  destFs.valid = TRUE;

  return destFs.data.GetPointer();
}

void ConferenceMember::UnlockExternalVideo()
{ 
  memberFrameStoreMutex.Signal(); 
  lock.Signal();
}

BOOL ConferenceMember::AddVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  PAssert(videoMixer != NULL, "attempt to add video source to NULL video mixer");
  return videoMixer->AddVideoSource(id, mbr);
}

void ConferenceMember::RemoveVideoSource(ConferenceMemberId id, ConferenceMember & mbr)
{
  PAssert(videoMixer != NULL, "attempt to remove video source from NULL video mixer");
  videoMixer->RemoveVideoSource(id, mbr);
}

BOOL ConferenceMember::OnOutgoingVideo(void * buffer, int width, int height, PINDEX & amount)
{
  return FALSE;
}

BOOL ConferenceMember::OnIncomingVideo(const void * buffer, int width, int height, PINDEX amount)
{
  return FALSE;
}

#endif

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
  timeSize = 2 * channels * sampleRate / 1000;
  timeBufferSize = PCM_BUFFER_LEN_MS * PCM_BUFFER_COUNT;
  byteBufferSize = timeBufferSize * timeSize;
  buffer.SetSize(byteBufferSize);
  timeIndex = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceAudioConnection::~ConferenceAudioConnection()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceAudioConnection::WriteAudio(const BYTE * data, int amount)
{
  if(amount == 0)
    return;

  int frameTime = amount / timeSize;
  // большой amount, например при переключении паузы в линфоне
  // ???
  if(frameTime > 30)
    return;

  int byteIndex = (timeIndex % timeBufferSize) * timeSize;
  int byteLeft = amount;
  int byteOffset = 0;
  if(byteIndex + byteLeft > byteBufferSize)
  {
    byteOffset = byteBufferSize - byteIndex;
    memcpy(buffer.GetPointer() + byteIndex, data, byteOffset);
    byteLeft -= byteOffset;
    byteIndex = 0;
  }
  memcpy(buffer.GetPointer() + byteIndex, data + byteOffset, byteLeft);
  timeIndex += frameTime;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceAudioConnection::ReadAudio(ConferenceMember *member, BYTE * data, int amount, int dstSampleRate, int dstChannels)
{
  if(amount == 0)
    return;

  int srcTimeIndex = timeIndex; // save
  int dstFrameTime = amount * 1000 / (dstSampleRate * dstChannels * 2);

  AudioReader *reader = NULL;
  AudioReaderListType & audioReaderList = member->GetAudioReaderList();
  AudioReaderListType::iterator it = audioReaderList.find(id);
  if(it == audioReaderList.end())
  {
    reader = new AudioReader();
    reader->timeIndex = srcTimeIndex;
    if(reader->timeIndex >= dstFrameTime)
      reader->timeIndex -= dstFrameTime;
    audioReaderList.insert(AudioReaderListType::value_type(id, reader));
  }
  else
    reader = it->second;

  // update read time
  reader->readTime = PTime();

  // restart
  if(srcTimeIndex - reader->timeIndex > timeBufferSize/2)
    reader->timeIndex = srcTimeIndex - dstFrameTime;

  if(reader->timeIndex + dstFrameTime > srcTimeIndex)
  {
    //if(srcTimeIndex > reader->timeIndex)
    //  dstFrameTime = srcTimeIndex - reader->timeIndex;
    //else
      return;
  }

  // tmp buffer
  int dstBufferSize = sampleRate * channels * 2 * dstFrameTime / 1000;
  if(reader->buffer.GetSize() < dstBufferSize)
    reader->buffer.SetSize(dstBufferSize);

  int byteIndex = (reader->timeIndex % timeBufferSize) * timeSize;
  int byteLeft = dstFrameTime*timeSize;
  int byteOffset = 0;
  if(byteIndex + byteLeft > byteBufferSize)
  {
    byteOffset = byteBufferSize - byteIndex;
    memcpy(reader->buffer.GetPointer(), buffer.GetPointer() + byteIndex, byteOffset);
    byteLeft = dstBufferSize - byteOffset;
    byteIndex = 0;
    reader->timeIndex += byteOffset/timeSize;
  }

  memcpy(reader->buffer.GetPointer() + byteOffset, buffer.GetPointer() + byteIndex, byteLeft);
  reader->timeIndex += byteLeft/timeSize;

  if(sampleRate == dstSampleRate && channels == dstChannels)
  {
    Mix(reader->buffer.GetPointer(), data, dstBufferSize);
  }
  else
  {
    // при использовании одного swrc для разных соединений появляются артефакты
    // ???
    long resamplerKey = (long)id; //(channels<<24)|sampleRate;
    AudioResampler *resampler = NULL;
    AudioResamplerListType & audioResamplerList = member->GetAudioResamplerList();
    AudioResamplerListType::iterator it = audioResamplerList.find(resamplerKey);
    if(it == audioResamplerList.end())
    {
      resampler = CreateResampler(sampleRate, channels, dstSampleRate, dstChannels);
      audioResamplerList.insert(AudioResamplerListType::value_type(resamplerKey, resampler));
    }
    else
      resampler = it->second;

    if(resampler->data.GetSize() < amount)
      resampler->data.SetSize(amount + 16);

    Resample(reader->buffer.GetPointer(), dstBufferSize, sampleRate, channels, resampler, amount, dstSampleRate, dstChannels);
    Mix(resampler->data.GetPointer(), data, amount);
  }
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

AudioResampler * ConferenceAudioConnection::CreateResampler(int srcSampleRate, int srcChannels, int dstSampleRate, int dstChannels)
{
  AudioResampler * resampler = new AudioResampler();
  resampler->srcSampleRate = srcSampleRate;
  resampler->srcChannels = srcChannels;
  resampler->dstSampleRate = dstSampleRate;
  resampler->dstChannels = dstChannels;
#if USE_SWRESAMPLE
  resampler->swrc = swr_alloc_set_opts(NULL,
    MCU_AV_CH_Layout_Selector[dstChannels], AV_SAMPLE_FMT_S16, dstSampleRate,
    MCU_AV_CH_Layout_Selector[srcChannels], AV_SAMPLE_FMT_S16, srcSampleRate, 0, NULL);
  swr_init(resampler->swrc);
#elif USE_AVRESAMPLE
  resampler->swrc = avresample_alloc_context();
  av_opt_set_int(resampler->swrc, "out_channel_layout", MCU_AV_CH_Layout_Selector[dstChannels], 0);
  av_opt_set_int(resampler->swrc, "out_sample_fmt",     AV_SAMPLE_FMT_S16, 0);
  av_opt_set_int(resampler->swrc, "out_sample_rate",    dstSampleRate, 0);
  av_opt_set_int(resampler->swrc, "in_channel_layout",  MCU_AV_CH_Layout_Selector[srcChannels], 0);
  av_opt_set_int(resampler->swrc, "in_sample_fmt",      AV_SAMPLE_FMT_S16,0);
  av_opt_set_int(resampler->swrc, "in_sample_rate",     srcSampleRate, 0);
  avresample_open(resampler->swrc);
#elif USE_LIBSAMPLERATE
  resampler->swrc = src_new(SRC_LINEAR, 1, NULL);
#endif
  return resampler;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceAudioConnection::Resample(BYTE * src, int srcBytes, int srcSampleRate, int srcChannels, AudioResampler *resampler, int dstBytes, int dstSampleRate, int dstChannels)
{
#if USE_SWRESAMPLE
  void * to = resampler->data.GetPointer();
  void * from = (void*)src;
  swr_convert(resampler->swrc,
                        (uint8_t **)&to,
                        (dstBytes>>1)/dstChannels,
                        (const uint8_t **)&from,
                        (srcBytes>>1)/srcChannels);
#elif USE_AVRESAMPLE
  void * to = resampler->data.GetPointer();
  void * from = (void*)src;

  int out_samples = (dstBytes>>1)/dstChannels;
  int out_linesize = dstBytes;
  int in_samples = (srcBytes>>1)/srcChannels;
  int in_linesize = srcBytes;

  avresample_convert(resampler->swrc, (uint8_t **)&to, out_linesize, out_samples,
                                      (uint8_t **)&from, in_linesize, in_samples);
#elif USE_LIBSAMPLERATE
  SRC_DATA src_data;
  long out_samples = dstBytes/(dstChannels*sizeof(short));
  long in_samples = srcBytes/(srcChannels*sizeof(short));
  float data_out[out_samples*sizeof(float)];
  float data_in[in_samples*sizeof(float)];
  src_short_to_float_array((const short *)src, data_in, in_samples);

  src_data.data_in = data_in;
  src_data.input_frames = in_samples;
  src_data.data_out = data_out;
  src_data.output_frames = out_samples;
  src_data.src_ratio = (double)out_samples/(double)in_samples;

  int err = src_process(resampler->swrc, &src_data);
  if (err)
  {
    PTRACE(1, "libsamplerate error: " << src_strerror(err));
    return;
  }
  src_float_to_short_array(data_out, (short *)resampler->data.GetPointer(), src_data.output_frames_gen);
  //PTRACE(1, "libsamplerate: " << src_data.input_frames << " " << src_data.output_frames << " " << src_data.input_frames_used << " " << src_data.output_frames_gen);
#else
  if(srcChannels == dstChannels && srcChannels == 1)
  {
    for(PINDEX i=0;i<(dstBytes>>1);i++) ((short*)(resampler->data.GetPointer()))[i] = ((short*)src)[i*srcSampleRate/dstSampleRate];
    return;
  }
  if(srcChannels == dstChannels)
  {
    for(unsigned i=0;i<((dstBytes>>1)/dstChannels);i++)
    {
      unsigned ofs=(i*srcSampleRate/dstSampleRate)*srcChannels;
      for(unsigned j=0;j<srcChannels;j++) ((short*)(resampler->data.GetPointer()))[i*srcChannels+j] = ((short*)src)[ofs+j];
    }
    return;
  }
  for(unsigned i=0;i<(dstBytes>>1)/dstChannels;i++)
  {
    unsigned ofs=(i*srcSampleRate/dstSampleRate)*srcChannels, srcChan=0;
    for(unsigned j=0;j<dstChannels;j++)
    {
      ((short*)(resampler->data.GetPointer()))[i*dstChannels+j] = ((short*)src)[ofs+srcChan];
      srcChan++; if(srcChan>=srcChannels) srcChan=0;
    }
  }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////

MCULock::MCULock()
{
  closing = FALSE;
  count = 0;
}

BOOL MCULock::Wait(BOOL hard)
{
  mutex.Wait();
  if (hard)
    return TRUE;

  BOOL ret = TRUE;
  if (!closing)
    count++;
  else
    ret = FALSE;

  mutex.Signal();
  return ret;
}

void MCULock::Signal(BOOL hard)
{
  if (hard) {
    mutex.Signal();
    return;
  }

  mutex.Wait();
  if (count > 0)
    count--;
  if (closing)
    closeSync.Signal();
  mutex.Signal();
}

void MCULock::WaitForClose()
{
  mutex.Wait();
  closing = TRUE;
  BOOL wait = count > 0;
  mutex.Signal();
  while (wait) {
    closeSync.Wait();
    mutex.Wait();
    wait = count > 0;
    mutex.Signal();
  }
}
