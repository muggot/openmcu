
#include <ptlib.h>
#include <stdio.h>
#include <string.h>

#include "conference.h"
#include "mcu.h"

#if MCU_VIDEO
#include <ptlib/vconvert.h>
#endif

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

Conference * ConferenceManager::MakeConferenceWithLock(const PString & room, PString name)
{
  PWaitAndSignal m(conferenceListMutex);
  Conference * conference = FindConferenceWithLock(room);
  if(conference == NULL)
  {
    // create the conference
    long id = conferenceList.GetNextID();
    OpalGloballyUniqueID conferenceID;
    conference = CreateConference(id, conferenceID, room, name);
    conferenceList.Insert(id, conference, room);
    //
    OnCreateConference(conference);
    // set the conference count
    maxConferenceCount = PMAX(maxConferenceCount, conferenceList.GetCurrentSize());
  }
  return conference;
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
  MCUProfileList & profileList = conference->GetProfileList();
  return profileList(memberName);
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
ConferenceMember * ConferenceManager::FindMemberWithoutLock(Conference * conference, const PString & memberName)
{
  ConferenceMember *member = FindMemberWithLock(conference, memberName);
  if(member)
    member->Unlock();
  return member;
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
  PString memberNameID = MCUURL(memberName).GetMemberNameId();
  MCUMemberList & memberList = conference->GetMemberList();
  for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
  {
    ConferenceMember *member = it.GetObject();
    if(member->GetNameID() == memberNameID)
      return it.GetCapturedObject();
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
  if(videoMixerList.GetCurrentSize() != 0)
  {
    MCUVideoMixerList::shared_iterator it(&videoMixerList, number);
    return it.GetCapturedObject();
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

int ConferenceManager::AddVideoMixer(Conference * conference)
{
  MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
  MCUSimpleVideoMixer *mixer = new MCUSimpleVideoMixer(TRUE);
  mixer->SetID(videoMixerList.GetNextID());
  mixer->SetConference(conference);
  videoMixerList.Insert(mixer->GetID(), mixer);
  videoMixerList.Release(mixer->GetID());
  return videoMixerList.GetCurrentSize();
}

int ConferenceManager::DeleteVideoMixer(Conference * conference, int number)
{
  MCUVideoMixerList & videoMixerList = conference->GetVideoMixerList();
  if(videoMixerList.GetCurrentSize() == 1)
    return videoMixerList.GetCurrentSize();

  MCUVideoMixerList::shared_iterator it(&videoMixerList, number);
  if(it != videoMixerList.end())
  {
    MCUSimpleVideoMixer *mixer = it.GetObject();
    if(videoMixerList.Erase(it))
      delete mixer;
  }
  return videoMixerList.GetCurrentSize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceManager::OnCreateConference(Conference * conference)
{
  if(MCUConfig("Export Parameters").GetBoolean("Enable export", FALSE) == TRUE)
    conference->pipeMember = new ConferencePipeMember(conference);

  // add file recorder member
  if(GetConferenceParam(conference->GetNumber(), RoomAllowRecordKey, TRUE))
  {
    conference->conferenceRecorder = new ConferenceRecorder(conference);
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

  PString jsName(number);
  jsName.Replace("\"","\\x27",TRUE,0); jsName.Replace("'","\\x22",TRUE,0);
  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(1,'" + jsName + "')", number);

  PTRACE(2,"MCU\tOnDestroyConference " << number <<", disconnect remote endpoints");
  MCUMemberList & memberList = conference->GetMemberList();
  for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
  {
    ConferenceMember * member = it.GetObject();
    if(member)
      member->Close();
  }

  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(2,'" + jsName + "')", number);

  int members = memberList.GetCurrentSize();
  while(members != 0)
  {
    MCUTRACE(0,"MCU\tOnDestroyConference " << number <<", waiting... members: " << members);
    PThread::Sleep(100);
    members = memberList.GetCurrentSize();
  }

  PTRACE(2,"MCU\tOnDestroyConference " << number <<", clearing profile list");
  MCUProfileList & profileList = conference->GetProfileList();
  for(MCUProfileList::shared_iterator it = profileList.begin(); it != profileList.end(); ++it)
  {
    ConferenceProfile *profile = it.GetObject();
    if(profileList.Erase(it))
      delete profile;
  }

  OpenMCU::Current().HttpWriteCmdRoom("notice_deletion(3,'" + jsName + "')", number);

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

void ConferenceManager::ManagerRefreshAddressBook()
{
  // refresh Address Book
  for(MCUConferenceList::shared_iterator it = conferenceList.begin(); it != conferenceList.end(); ++it)
  {
    Conference *conference = *it;
    conference->RefreshAddressBook();
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
  if(autoDeleteEmpty && !conference->GetVisibleMemberCount())
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

    PINDEX visibleMembers = conference->GetVisibleMemberCount();

    if(autoRecordStop != "Disable" && visibleMembers <= autoRecordStop.AsInteger())
      conference->StopRecorder();
    else if(autoRecordStart != "Disable" && autoRecordStart.AsInteger() > autoRecordStop.AsInteger() && visibleMembers >= autoRecordStart.AsInteger())
      conference->StartRecorder();
  }

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
#if MCU_VIDEO
  if(mixer)
  {
    mixer->SetID(videoMixerList.GetNextID());
    mixer->SetConference(this);
    videoMixerList.Insert(mixer->GetID(), mixer);
    videoMixerList.Release(mixer->GetID());
  }
#endif
  memberCount = 0;
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
  pipeMember = NULL;
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
    PTRACE(1,"EVRT\tRecorder space check failed");
    return TRUE;
  }
  BOOL result = ((f>>20) >= OpenMCU::Current().vr_minimumSpaceMiB);
  if(!result) OpenMCU::Current().HttpWriteEvent("<b><font color='red'>Insufficient disk space</font>: Video Recorder DISABLED</b>");
  PTRACE_IF(1,!result,"EVRT\tInsufficient disk space: Video Recorder DISABLED");
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

////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::RefreshAddressBook()
{
  PStringArray abook = OpenMCU::Current().GetRegistrar()->GetAccountStatusList();
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
  PWaitAndSignal m(memberListMutex);

  // memberList
  if(member)
  {
    memberList.Insert((long)member->GetID(), member, name);
    memberList.Release((long)member->GetID());
  }

  if(member && member->GetType() & MEMBER_TYPE_GSYSTEM)
    return;

  // profileList
  PString nameID = MCUURL(name).GetMemberNameId();
  for(MCUProfileList::shared_iterator it = profileList.begin(); it != profileList.end(); ++it)
  {
    ConferenceProfile *profile = it.GetObject();
    if(profile->GetMember())
      continue;
    if(profile->GetNameID() == nameID)
    {
      if(profileList.Erase(it))
        delete profile;
      break;
    }
  }
  ConferenceProfile *profile = new ConferenceProfile(profileList.GetNextID(), name, this, member);
  profileList.Insert(profile->GetID(), profile, name);
  profileList.Release(profile->GetID());

  if(member)
  {
    PStringStream msg;
    msg << "<font color=green><b>+</b>" << member->GetName() << "</font>";
    OpenMCU::Current().HttpWriteEventRoom(msg, number);
    msg = "addmmbr(1";
    msg << "," << (long)member->GetID()
        << ",\"" << member->GetNameHTML() << "\""
        << "," << member->muteMask
        << "," << member->disableVAD
        << "," << member->chosenVan
        << "," << member->GetAudioLevel()
        << "," << member->GetVideoMixerNumber()
        << ",\"" << member->GetNameID() << "\""
        << "," << dec << (unsigned)member->channelCheck
        << "," << member->kManualGainDB
        << "," << member->kOutputGainDB
        << ")";
    OpenMCU::Current().HttpWriteCmdRoom(msg, number);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::RemoveMemberFromList(const PString & name, ConferenceMember *member)
{
  PWaitAndSignal m(memberListMutex);

  // memberList
  if(member)
    memberList.Erase((long)member->GetID());

  if(member && member->GetType() & MEMBER_TYPE_GSYSTEM)
    return;

  // profileList
  for(MCUProfileList::shared_iterator it = profileList.begin(); it != profileList.end(); ++it)
  {
    ConferenceProfile *profile = it.GetObject();
    if(profile->GetName() == name && profile->GetMember() == member)
    {
      if(profileList.Erase(it))
        delete profile;
      break;
    }
  }

  if(member)
  {
    ConferenceProfile *profile = new ConferenceProfile(profileList.GetNextID(), name, this, NULL);
    profileList.Insert(profile->GetID(), profile, name);
    profileList.Release(profile->GetID());
  }

  if(member)
  {
    PStringStream msg;
    msg << "<font color=red><b>-</b>" << member->GetName() << "</font>";
    OpenMCU::Current().HttpWriteEventRoom(msg, number);
    msg = "remmmbr(0";
    msg << ","  << (long)member->GetID()
        << ",\"" << member->GetNameHTML() << "\""
        << ","  << member->muteMask
        << "," << member->disableVAD
        << ","  << member->chosenVan
        << ","  << member->GetAudioLevel()
        << ",\"" << member->GetNameID() << "\"";
    msg << ",0";
    msg << ")";
    OpenMCU::Current().HttpWriteCmdRoom(msg, number);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::AddMember(ConferenceMember * memberToAdd)
{
  memberToAdd->SetName();

  PTRACE(3, "Conference\t " << number << " Adding member: " << memberToAdd->GetName());
  cout << "Conference " << number << " Adding member: " << memberToAdd->GetName() << endl;

  // lock the member lists
  PWaitAndSignal m(memberListMutex);

  MCUMemberList::shared_iterator it = memberList.Find((long)memberToAdd->GetID());
  if(it != memberList.end())
  {
    PTRACE(1, "Conference\t" << number << "Rejected duplicate member ID: " << (long)memberToAdd->GetID() << " " << memberToAdd->GetName());
    return FALSE;
  }

  // check for duplicate name or very fast reconnect
  if(!memberToAdd->GetType() & MEMBER_TYPE_GSYSTEM)
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
        msg << memberToAdd->GetNameHTML() << " REJECTED - DUPLICATE NAME";
        OpenMCU::Current().HttpWriteEventRoom(msg, number);
        PTRACE(1, "Conference " << number << "\tRejected duplicate name: " << memberToAdd->GetName());
        return FALSE;
      }
      memberToAdd->SetName(memberName+" ##"+PString(i+2));
    }
  }

  if(!UseSameVideoForAllMembers())
    memberToAdd->videoMixer = new MCUSimpleVideoMixer();

  // add to all lists
  AddMemberToList(memberToAdd->GetName(), memberToAdd);

  memberCount++;
  if(memberToAdd->IsVisible())
  {
    visibleMemberCount++;
    maxMemberCount = PMAX(maxMemberCount, visibleMemberCount);
  }

  if(UseSameVideoForAllMembers())
  {
    if(moderated == FALSE
#if ENABLE_TEST_ROOMS
       || number == "testroom")
#endif
    {
      if(memberToAdd->IsVisible())
      {
        MCUSimpleVideoMixer * mixer = manager.GetVideoMixerWithLock(this);
        mixer->AddVideoSource(memberToAdd->GetID(), *memberToAdd);
        mixer->Unlock();
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
        if(memberToAdd->IsVisible())
          member->AddVideoSource(memberToAdd->GetID(), *memberToAdd);
      }
    }
  }

  // update the statistics
  if(memberToAdd->IsVisible())
  {
    // trigger H245 thread for join message
    //new NotifyH245Thread(*this, TRUE, memberToAdd);
  }

  // notify that member is joined
  memberToAdd->SetJoined(TRUE);

  // template
  if(!memberToAdd->GetType() & MEMBER_TYPE_GSYSTEM)
    PullMemberOptionsFromTemplate(memberToAdd, confTpl);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::RemoveMember(ConferenceMember * memberToRemove)
{
  if(memberToRemove == NULL)
    return TRUE;

  // lock memberList
  PWaitAndSignal m(memberListMutex);

  if(!memberToRemove->IsJoined())
  {
    PTRACE(4, "Conference\t" << number << "No need to remove member " << memberToRemove->GetName());
    return FALSE;
  }

  PTRACE(3, "Conference\t" << number << "Removing member " << memberToRemove->GetName());
  cout << "Conference " << number << " Removing member " << memberToRemove->GetName() << endl;

  // remove from all lists
  RemoveMemberFromList(memberToRemove->GetName(), memberToRemove);

  memberCount--;
  if(memberToRemove->IsVisible())
    visibleMemberCount--;

  memberToRemove->RemoveAllConnections();

  // remove ConferenceConnection
  RemoveAudioConnection(memberToRemove);

  if(UseSameVideoForAllMembers())
  {
    if(moderated == FALSE || number == "testroom")
    {
      if(memberToRemove->IsVisible())
      {
        MCUSimpleVideoMixer *mixer = manager.GetVideoMixerWithLock(this);
        mixer->RemoveVideoSource(memberToRemove->GetID(), *memberToRemove);
        mixer->Unlock();
      }
    }
    else
    {
      for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
      {
        MCUSimpleVideoMixer *mixer = it.GetObject();
        mixer->MyRemoveVideoSourceById(memberToRemove->GetID(), FALSE);
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
        if(memberToRemove->IsVisible())
          member->RemoveVideoSource(memberToRemove->GetID(), *memberToRemove);
        if(member->IsVisible())
          memberToRemove->RemoveVideoSource(member->GetID(), *member);
      }
    }
  }


  // trigger H245 thread for leave message
  //if (memberToRemove->IsVisible())
  //  new NotifyH245Thread(*this, FALSE, memberToRemove);

  // notify that member is not joined anymore
  memberToRemove->SetJoined(FALSE);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceAudioConnection * Conference::AddAudioConnection(ConferenceMember * member, unsigned sampleRate, unsigned channels)
{
  if(member->GetType() & MEMBER_TYPE_GSYSTEM)
    return NULL;
  ConferenceAudioConnection * conn = new ConferenceAudioConnection((long)member->GetID(), sampleRate, channels);
  audioConnectionList.Insert((long)member->GetID(), conn);
  return conn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::RemoveAudioConnection(ConferenceMember * member)
{
  ConferenceAudioConnection * conn = audioConnectionList((long)member->GetID());
  audioConnectionList.Release((long)member->GetID());
  audioConnectionList.Erase((long)member->GetID());
  delete conn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::ReadMemberAudio(ConferenceMember * member, const uint64_t & timestamp, void * buffer, int amount, int sampleRate, int channels)
{
  for(MCUAudioConnectionList::shared_iterator it = audioConnectionList.begin(); it != audioConnectionList.end(); ++it)
  {
    ConferenceAudioConnection * conn = it.GetObject();
    if(conn->GetID() == (long)member->GetID())
      continue;

    BOOL skip = moderated&&muteUnvisible;
    if(skip)
    {
      for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
      {
        MCUSimpleVideoMixer *mixer = it.GetObject();
        if(mixer->GetPositionStatus(member->GetID()) >= 0)
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
  ConferenceAudioConnection * conn = audioConnectionList((long)member->GetID());
  if(conn && (conn->GetSampleRate() != sampleRate || conn->GetChannels() != channels))
  {
    audioConnectionList.Release((long)member->GetID());
    audioConnectionList.Erase((long)member->GetID());
    delete conn;
    conn = NULL;
  }
  if(conn == NULL)
    conn = AddAudioConnection(member, sampleRate, channels);
  conn->WriteAudio(timestamp, (const BYTE *)buffer, amount);
  audioConnectionList.Release((long)member->GetID());
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
  if(UseSameVideoForAllMembers())
  {
    if(audioLevel > VAlevel)
      member->vad += tint;
    else
      member->vad = 0;

    for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
    {
      MCUSimpleVideoMixer *mixer = it.GetObject();
      int status = mixer->GetPositionStatus(member->GetID());
      if(audioLevel > VAlevel)
      {
        if(member->vad >= VAdelay) // voice-on trigger delay
        {
          if(status > 0)
            mixer->SetPositionStatus(member->GetID(),0);
          else if(status == 0 && member->disableVAD == FALSE)
          {
            if(member->vad-VAdelay>500) // execute every 500 ms of voice activity
              mixer->SetVAD2Position(member);
          }
          else if(status == -1 && member->disableVAD == FALSE) //find new vad position for active member
          {
            ConferenceMemberId id = mixer->SetVADPosition(member, member->chosenVan, VAtimeout);
            if(id != NULL)
            {
              FreezeVideo(id);
              member->SetFreezeVideo(FALSE);
            }
          }
        }
      }
      else
      {
        if(status >= 0) // increase silence counter
          mixer->SetPositionStatus(member->GetID(),status + tint);
      }
      if(audioLevel > VAlevel && status == 0 && member->disableVAD == FALSE && member->vad-VAdelay > 500)
        member->vad = VAdelay;
    }
  }
#endif // MCU_VIDEO
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::ReadMemberVideo(ConferenceMember * member, void * buffer, int width, int height, PINDEX & amount)
{
  if(videoMixerList.GetCurrentSize() == 0)
    return;

  long mixerNumber;
  if(member == NULL)
    mixerNumber = 0;
  else
    mixerNumber = member->GetVideoMixerNumber();

  MCUSimpleVideoMixer * mixer = manager.FindVideoMixerWithLock(this, mixerNumber);
  if(mixer == NULL)
  {
    if(mixerNumber != 0)
      mixer = manager.GetVideoMixerWithLock(this);
    if(mixer == NULL)
    {
      PTRACE(3,"Conference\tCould not get video");
      return;
    } else  {
      PTRACE(6,"Conference\tCould not get video mixer " << mixerNumber << ", reading 0 instead");
    }
  }

  mixer->ReadFrame(*member, buffer, width, height, amount);
  mixer->Unlock();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Conference::WriteMemberVideo(ConferenceMember * member, const void * buffer, int width, int height, PINDEX amount)
{
  if(UseSameVideoForAllMembers())
  {
    bool writeResult = FALSE;
    for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
    {
      MCUSimpleVideoMixer *mixer = it.GetObject();
      writeResult |= mixer->WriteFrame(member->GetID(), buffer, width, height, amount);
    }
    return writeResult;
  }
  else
  {
    for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
      it->OnExternalSendVideo(member->GetID(), buffer, width, height, amount);
  }
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::FreezeVideo(ConferenceMemberId id)
{
  PWaitAndSignal m(memberListMutex);

  if(id != NULL)
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
        if(mixer->GetPositionStatus(id) >= 0)
        {
          member->SetFreezeVideo(FALSE);
          return;
        }
      }
    } else {
      for(MCUMemberList::shared_iterator it2 = memberList.begin(); it2 != memberList.end(); ++it2)
      {
        ConferenceMember *member = it2.GetObject();
        if(member->videoMixer && member->videoMixer->GetPositionStatus(id) >= 0)
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
        if(mixer->GetPositionStatus(mid) >= 0)
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
        if(member->videoMixer && member->videoMixer->GetPositionStatus(mid) >= 0)
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
        if(mixer->GetPositionStatus(member->GetID()) < 0)
          put |= (NULL != mixer->SetVADPosition(member, member->chosenVan, VAtimeout));
      }
    }
  }
  return put;
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void Conference::HandleFeatureAccessCode(ConferenceMember & member, PString fac)
{
  PTRACE(3,"Conference\tHandling feature access code " << fac << " from " << member.GetName());
  PStringArray s = fac.Tokenise("*");
  if(s[0]=="1")
  {
    int posTo=0;
    if(s.GetSize() > 1)
      posTo=s[1].AsInteger();
    PTRACE(4,"Conference\t" << "*1*" << posTo << "#: jump into video position " << posTo);

    if(videoMixerList.GetCurrentSize() == 0)
      return;

    ConferenceMemberId id=member.GetID();
    if(id == NULL)
      return;

    MCUSimpleVideoMixer *mixer = manager.GetVideoMixerWithLock(this);
    int pos = mixer->GetPositionNum(id);
    if(pos == posTo)
    {
      mixer->Unlock();
      return;
    }
    mixer->InsertVideoSource(&member,posTo);
    mixer->Unlock();

    FreezeVideo(NULL);

    OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*this),number);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",number);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceProfile::ConferenceProfile(long _listID, const PString & _name, Conference * _conference, ConferenceMember *_member)
{
  listID = _listID;
  member = _member;
  conference = _conference;
  name = _name;
  nameID = MCUURL(name).GetMemberNameId();
  nameHTML = name;
  nameHTML.Replace("&","&amp;",TRUE,0);
  nameHTML.Replace("\"","&quot;",TRUE,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceProfile::Unlock()
{
  conference->GetProfileList().Release(listID);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceMember::ConferenceMember(Conference * _conference)
  : conference(_conference)
{
  id = this;
  memberType = MEMBER_TYPE_NONE;
  channelCheck=0;
  audioLevel = 0;
  audioCounter = 0;
  previousAudioLevel = 65535;
  audioLevelIndicator = 0;
  currVolCoef = 1.0;
  kManualGain = 1.0; kManualGainDB = 0;
  kOutputGain = 1.0; kOutputGainDB = 0;
  memberIsJoined = FALSE;

#if MCU_VIDEO
  videoMixer = NULL;
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

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::RemoveAllConnections()
{
  PTRACE(3, "Conference\tRemoving all members from connection " << id);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::WriteAudioAutoGainControl(const short * pcm, unsigned samplesPerFrame, unsigned codecChannels, unsigned sampleRate, unsigned level, float* currVolCoef, unsigned* signalLevel, float kManual)
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

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::WriteAudio(const uint64_t & timestamp, const void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels)
{
  if(!(channelCheck&1))
    ChannelBrowserStateUpdate(1,TRUE);

  if(conference != NULL)
  {
    // Автоматическая? регулировка усиления
    // calculate average signal level for this member
    unsigned signalLevel=0;
    WriteAudioAutoGainControl((short*) buffer, amount/channels/2, channels, sampleRate, 2000, &currVolCoef, &signalLevel, kManualGain);
    audioLevel = ((signalLevel * 2) + audioLevel) / 3;

    // Записать аудио в буфер
    conference->WriteMemberAudio(this, timestamp, buffer, amount, sampleRate, channels);

    // Индикатор уровня, VAD
    conference->WriteMemberAudioLevel(this, audioLevel, amount/32);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void ConferenceMember::ReadAudio(const uint64_t & timestamp, void * buffer, PINDEX amount, unsigned sampleRate, unsigned channels)
{
  if(!(channelCheck&2))
    ChannelBrowserStateUpdate(2,TRUE);

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
  if(!(channelCheck&8))
    ChannelBrowserStateUpdate(8,TRUE);

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
  if(!(channelCheck&4))
    ChannelBrowserStateUpdate(4,TRUE);

  ++totalVideoFramesReceived;
  rxFrameWidth = width;
  rxFrameHeight = height;
  if (!firstFrameReceiveTime.IsValid())
    firstFrameReceiveTime = PTime();

  if(lock.Wait())
  {
    if(conference != NULL)
      conference->WriteMemberVideo(this, buffer, width, height, amount);
    lock.Signal();
  }
}

void ConferenceMember::OnExternalSendVideo(ConferenceMemberId id, const void * buffer, int width, int height, PINDEX amount)
{
  if(lock.Wait())
  {
    videoMixer->WriteFrame(id, buffer, width, height, amount);
    lock.Signal();
  }
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

#endif

PString ConferenceMember::GetMonitorInfo(const PString & /*hdr*/)
{ 
  return PString::Empty(); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////

ConferenceAudioConnection::ConferenceAudioConnection(long _id, int _sampleRate, int _channels)
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
      audioBufferList.Insert(audioBufferKey, audioBuffer);
      audioBufferList.Release(audioBufferKey);
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
    uint64_t writeTimestamp = startTimestamp + srcTimeIndex*1000 + frameTime*1000;
    if(writeTimestamp + PCM_BUFFER_LAG_MS*1000 < srcTimestamp)
    {
      srcTimeIndex = srcTimestamp/1000LL - startTimestamp/1000LL - frameTime;
      PTRACE(6, "ConferenceAudioConnection\tWriter has lost " << srcTimestamp - writeTimestamp << " microseconds");
    }
  }

  for(MCUAudioBufferList::shared_iterator it = audioBufferList.begin(); it != audioBufferList.end(); ++it)
  {
    AudioBuffer *audioBuffer = it.GetObject();

    long resamplerKey = audioBuffer->GetSampleRate() + audioBuffer->GetChannels();
    AudioResamplerListType::iterator it = audioResamplerList.find(resamplerKey);
    if(it == audioResamplerList.end())
      continue;
    AudioResampler * resampler = it->second;

    PBYTEArray dstBuffer;
    int dstBufferSize = frameTime * audioBuffer->GetTimeSize();
    if(dstBuffer.GetSize() < dstBufferSize + 16)
      dstBuffer.SetSize(dstBufferSize + 16);

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
  //int dstTimeIndex = dstTimestamp/1000LL - startTimestamp/1000LL - PCM_BUFFER_MAX_WRITE_LEN_MS - PCM_BUFFER_LAG_MS;
  int dstTimeIndex = dstTimestamp/1000LL - startTimestamp/1000LL - srcFrameTime - PCM_BUFFER_LAG_MS;

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

  PBYTEArray dstBuffer;
  int dstBufferSize = dstFrameTime * audioBuffer->GetTimeSize();
  if(dstBuffer.GetSize() < dstBufferSize)
    dstBuffer.SetSize(dstBufferSize);

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
  //PTRACE(1, "libsamplerate: " << src_data.input_frames << " " << src_data.output_frames << " " << src_data.input_frames_used << " " << src_data.output_frames_gen);
#else
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

////////////////////////////////////////////////////////////////////////////////////////////////////
