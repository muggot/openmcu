
#include <ptlib.h>
#include <stdio.h>
#include <string.h>

#include "conference.h"
#include "mcu.h"

#if OPENMCU_VIDEO
#include <ptlib/vconvert.h>
#endif

// size of a PCM data packet, in samples
//#define PCM_PACKET_LEN          480
#define PCM_PACKET_LEN          1920

// size of a PCM data buffer, in bytes
#define PCM_BUFFER_LEN          (PCM_PACKET_LEN * 2)

// number of PCM buffers to keep
#define PCM_BUFFER_COUNT        2

#define PCM_BUFFER_SIZE         (PCM_BUFFER_LEN * PCM_BUFFER_COUNT)

////////////////////////////////////////////////////////////////////////////////////

ConferenceManager::ConferenceManager()
{
  maxConferenceCount = 0;
  monitor  = new ConferenceMonitor(*this);
}

ConferenceManager::~ConferenceManager()
{
  monitor->running = FALSE;
  monitor->WaitForTermination();
  delete monitor;
}


Conference * ConferenceManager::MakeAndLockConference(const PString & roomToCreate, const PString & name)
{
  PWaitAndSignal m(conferenceListMutex);

  OpalGloballyUniqueID conferenceID;
  ConferenceListType::const_iterator r;
  for (r = conferenceList.begin(); r != conferenceList.end(); ++r) {
    if (roomToCreate == r->second->GetNumber()) {
      conferenceID = r->second->GetID();
      break;
    }
  }

  return MakeAndLockConference(conferenceID, roomToCreate, name);
}


Conference * ConferenceManager::MakeAndLockConference(const OpalGloballyUniqueID & conferenceID, 
                                                                   const PString & roomToCreate, 
                                                                   const PString & name)
{
  conferenceListMutex.Wait();

  Conference * conference = NULL;
  BOOL newConference = FALSE;
  ConferenceListType::const_iterator r = conferenceList.find(conferenceID);
  if (r != conferenceList.end())
    conference = r->second;
  else {
    // create the conference
    conference = CreateConference(conferenceID, roomToCreate, name, mcuNumberMap.GetNumber(conferenceID));

    // insert conference into the map
    conferenceList.insert(ConferenceListType::value_type(conferenceID, conference));

    // set the conference count
    maxConferenceCount = PMAX(maxConferenceCount, (PINDEX)conferenceList.size());
    newConference = TRUE;
  }

  if (newConference)
    OnCreateConference(conference);

  return conference;
}

void ConferenceManager::OnCreateConference(Conference * conference)
{
  // set time limit, if there is one
  int timeLimit = OpenMCU::Current().GetRoomTimeLimit();
  if (timeLimit > 0)
    monitor->AddMonitorEvent(new ConferenceTimeLimitInfo(conference->GetID(), PTime() + timeLimit*1000));
  // add file recorder member    
  new ConferenceFileMember(conference, (const PString) "recorder" , PFile::WriteOnly); 

  // add encoder cache member
//  OpenMCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
/*
  int i=0;
  if(ep.cvCaps!=NULL)
  while(ep.cvCaps[i])
  {
   const PString name(ep.cvCaps[i]);
   new ConferenceFileMember(conference, name , PFile::WriteOnly); 
   PThread::Sleep(100);   
   i++;
  }
*/
  FILE *membLst;
  PString name="members_"+conference->GetNumber()+".conf";
  membLst = fopen(name,"rt");
  if (membLst==NULL) membLst = fopen("members.conf","rt");
  if(membLst!=NULL)
  {
   char buf[128];
   while(fscanf(membLst,"%127[^\n]",buf)!=EOF)
   {
    PString memberName = buf;
    if(buf[0]!=0) conference->AddOfflineMemberToNameList(memberName);
    fscanf(membLst,"%*[^\n]"); fscanf(membLst,"%*[\n]");
   }
   fclose(membLst);
  }
}

Conference * ConferenceManager::CreateConference(const OpalGloballyUniqueID & _guid,
                                                              const PString & _number,
                                                              const PString & _name,
                                                                          int _mcuNumber)
{ 
#if OPENMCU_VIDEO
#if ENABLE_ECHO_MIXER
  if (_number *= "echo")
    return new Conference(*this, _guid, _guid.AsString(), _name, _mcuNumber, new EchoVideoMixer());
#endif
#if ENABLE_TEST_ROOMS
  if (_number.Left(8) == "testroom") {
    int count = _number.Mid(8).AsInteger();
    if (count > 0)
      return new Conference(*this, _guid, _number, _name, _mcuNumber, new TestVideoMixer(count));
  }
#endif
#endif

  return new Conference(*this, _guid, _number, _name, _mcuNumber
#if OPENMCU_VIDEO
                        ,OpenMCU::Current().CreateVideoMixer()
#endif
                        ); 
}

BOOL ConferenceManager::HasConference(const OpalGloballyUniqueID & conferenceID, PString & number)
{
  PWaitAndSignal m(conferenceListMutex);
  ConferenceListType::const_iterator r = conferenceList.find(conferenceID);
  if (r == conferenceList.end())
    return FALSE;
  number = r->second->GetNumber();
  return TRUE;
}

BOOL ConferenceManager::HasConference(const PString & number, OpalGloballyUniqueID & conferenceID)
{
  PWaitAndSignal m(conferenceListMutex);
  ConferenceListType::const_iterator r;
  for (r = conferenceList.begin(); r != conferenceList.end(); ++r) {
    if (r->second->GetNumber() == number) {
      conferenceID = r->second->GetID();
      return TRUE;
    }
  }
  return FALSE;
}

void ConferenceManager::RemoveConference(const OpalGloballyUniqueID & confId)
{
  PWaitAndSignal m(conferenceListMutex);
  ConferenceListType::iterator r = conferenceList.find(confId);
  if (r != conferenceList.end())  {
    Conference * conf = r->second;
    OnDestroyConference(conf);
    conferenceList.erase(confId);
    mcuNumberMap.RemoveNumber(conf->GetMCUNumber());
    monitor->RemoveForConference(conf->GetID());
    cout << "RemoveConference\n";
    delete conf;
  }
}

void ConferenceManager::RemoveMember(const OpalGloballyUniqueID & confId, ConferenceMember * toRemove)
{
  PWaitAndSignal m(conferenceListMutex);
  Conference * conf = toRemove->GetConference();

  OpalGloballyUniqueID id = conf->GetID();  // make a copy of the ID because it may be about to disappear

  BOOL removeConf = conf->RemoveMember(toRemove);
  delete toRemove;
  if (removeConf)
    RemoveConference(id);
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
  Conference::MemberList & list = conference.GetMemberList();
  Conference::MemberList::iterator r;
  for (r = list.begin(); r != list.end(); ++r)
    r->second->Close();
  return TRUE;
}

BOOL ConferenceRepeatingInfo::Perform(Conference & conference)
{
  this->timeToPerform = PTime() + repeatTime;
  return FALSE;
}


BOOL ConferenceMCUCheckInfo::Perform(Conference & conference)
{
  // see if any member of this conference is a not an MCU
  Conference::MemberList & list = conference.GetMemberList();
  Conference::MemberList::iterator r;
  for (r = list.begin(); r != list.end(); ++r)
    if (!r->second->IsMCU())
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
#if OPENMCU_VIDEO
                                    ,MCUVideoMixer * _videoMixer
#endif
)
  : manager(_manager), guid(_guid), number(_number), name(_name), mcuNumber(_mcuNumber), mcuMonitorRunning(FALSE)
#if OPENMCU_VIDEO
    ,videoMixer(_videoMixer)
#endif
{ 
  maxMemberCount = 0;
  moderated = FALSE;
  muteUnvisible = FALSE;
  VAdelay = 1000;
  VAtimeout = 10000;
  VAlevel = 100;
  echoLevel = 0;
  vidmembernum = 0;

  PTRACE(3, "Conference\tNew conference started: ID=" << guid << ", number = " << number);
}

Conference::~Conference()
{
#if OPENMCU_VIDEO
  delete videoMixer;
#endif
}

int Conference::GetVisibleMemberCount() const
{
  PWaitAndSignal m(memberListMutex);
  int visibleMembers = 0;
  std::map<void *, ConferenceMember *>::const_iterator r;
  for (r = memberList.begin(); r != memberList.end(); r++) {
    if (r->second->IsVisible())
      ++visibleMembers;
  }
  return visibleMembers;
}

void Conference::AddMonitorEvent(ConferenceMonitorInfo * info)
{ 
  manager.AddMonitorEvent(info); 
}

void Conference::InviteMember(const char *membName)
{
 char buf[128];
 int i=strlen(membName)-1;
 if(membName[i]!=']') return; while(i>=0 && membName[i]!='[') i--;
 if(i<0) return;
 i++;
 sscanf(&membName[i],"%127[^]]",buf); //buf[strlen(buf)-1]=0;
 PString address = buf;
 OpenMCUH323EndPoint & ep = OpenMCU::Current().GetEndpoint();
 PString h323Token;
 PString * userData = new PString(number);
 H323TransportAddressArray taa = ep.GetInterfaceAddresses(TRUE,NULL);

 if(!OpenMCU::Current().IsLoopbackCallsAllowed()){
   for(PINDEX i=0;i<taa.GetSize();i++)
   if(taa[i].Find("ip$"+address+":") == 0) {
     PTRACE(6,"Conference\tInviteMember LOCAL IP REJECTED (" << taa[i] << "): " << membName << " -> address=" << address << ";h323Token=" << h323Token << ";userData=" << userData);
     return;
   }
 }
 if (ep.MakeCall(address, h323Token, userData) == NULL) cout << "Invite err\n";
}

BOOL Conference::AddMember(ConferenceMember * memberToAdd)
{
  PTRACE(3, "Conference\tAbout to add member " << " to conference " << guid);
 
//  PTRACE(3, "Conference\tAbout to add member " << memberToAdd->GetTitle() << " to conference " << guid);

  // see if the callback refuses the new member (always true)
  if (!BeforeMemberJoining(memberToAdd))
    return FALSE;

  // add the member to the conference
  if (!memberToAdd->AddToConference(this))
    return FALSE;

  {
    PTRACE(3, "Conference\tAdding member " << memberToAdd->GetTitle() << " to conference " << guid);
    cout << "Adding member " << memberToAdd->GetTitle() << " to conference " << guid << endl;

    // lock the member list
    PWaitAndSignal m(memberListMutex);

    std::map<void *, ConferenceMember *>::const_iterator r;
    r = memberList.find(memberToAdd->GetID());
    if(r != memberList.end()) return FALSE;

#if OPENMCU_VIDEO
//    if(!UseSameVideoForAllMembers()) memberToAdd->videoStatus = 1;

    if (moderated==FALSE)
    {
     if (UseSameVideoForAllMembers() && memberToAdd->IsVisible()) {
      if (!videoMixer->AddVideoSource(memberToAdd->GetID(), *memberToAdd)) 
        memberToAdd->SetFreezeVideo(TRUE);
       //        memberToAdd->videoStatus = 1;
     PTRACE(3, "Conference\tUseSameVideoForAllMembers ");
     }
    }
    else memberToAdd->SetFreezeVideo(TRUE);
    
#endif

    // add this member to the conference member list
    memberList.insert(MemberList::value_type(memberToAdd->GetID(), memberToAdd));

    int tid = terminalNumberMap.GetNumber(memberToAdd->GetID());
    memberToAdd->SetTerminalNumber(tid);

    // make sure each member has a connection created for the new member
    // make sure the new member has a connection created for each existing member
    PINDEX visibleMembers = 0;
//    std::map<void *, ConferenceMember *>::const_iterator r;
    for (r = memberList.begin(); r != memberList.end(); r++) {
      ConferenceMember * conn = r->second;
      if (conn != memberToAdd) {
        conn->AddConnection(memberToAdd);
        memberToAdd->AddConnection(conn);
#if OPENMCU_VIDEO
        if (moderated==FALSE)
        if (!UseSameVideoForAllMembers()) {
          if (conn->IsVisible())
            memberToAdd->AddVideoSource(conn->GetID());
          if (memberToAdd->IsVisible())
            conn->AddVideoSource(memberToAdd->GetID());
        }
#endif
      }
      if (conn->IsVisible())
        ++visibleMembers;
    }

    // update the statistics
    if (memberToAdd->IsVisible()) {
      maxMemberCount = PMAX(maxMemberCount, visibleMembers);

      // trigger H245 thread for join message
//      new NotifyH245Thread(*this, TRUE, memberToAdd);
    }
  }

  // notify that member is joined
  memberToAdd->SetJoined(TRUE);

  // call the callback function
  OnMemberJoining(memberToAdd);

  if (memberToAdd->IsMCU() && !mcuMonitorRunning) {
    manager.AddMonitorEvent(new ConferenceMCUCheckInfo(GetID(), 1000));
    mcuMonitorRunning = TRUE;
  }

  memberToAdd->SetName();

  // add this member to the conference member name list
  if(memberToAdd!=memberToAdd->GetID())
  {
    memberNameList.erase(memberToAdd->GetName());
    memberNameList.insert(MemberNameList::value_type(memberToAdd->GetName(),memberToAdd));
  }
/*  
  else
  {
   serviceMemberNameList.erase(memberToAdd->GetName());
   serviceMemberNameList.insert(MemberNameList::value_type(memberToAdd->GetName(),memberToAdd));
  }
*/  
  return TRUE;
}


BOOL Conference::RemoveMember(ConferenceMember * memberToRemove)
{
  PWaitAndSignal m(memberListMutex);
  if(!memberToRemove->IsJoined())
  {
    PTRACE(4, "Conference\tNo need to remove call " << memberToRemove->GetName() << " from conference " << guid);
    return (memberList.size() == 0);
  }

  // add this member to the conference member name list with zero id

  PTRACE(3, "Conference\tRemoving call " << memberToRemove->GetName() << " from conference " << guid << " with size " << (PINDEX)memberList.size());
  cout << memberToRemove->GetName() << " leaving conference " << number << "(" << guid << ")" << endl;

     
  BOOL closeConference;
  {

    MemberNameList::iterator s;
    s = memberNameList.find(memberToRemove->GetName());
    
    ConferenceMember *zerop=NULL;
    if(memberToRemove!=memberToRemove->GetID() && s->second==memberToRemove)
     {
      memberNameList.erase(memberToRemove->GetName());  
      memberNameList.insert(MemberNameList::value_type(memberToRemove->GetName(),zerop));
     }

    // remove this connection from the member list
    memberList.erase(memberToRemove->GetID());
    memberToRemove->RemoveAllConnections();

    MemberList::iterator r;
    // remove this member from the connection lists for all other members
    for (r = memberList.begin(); r != memberList.end(); r++) {
      ConferenceMember * conn = r->second;
      if (conn != memberToRemove) {
        r->second->RemoveConnection(memberToRemove->GetID());
#if OPENMCU_VIDEO
        if (!UseSameVideoForAllMembers()) {
          if (memberToRemove->IsVisible())
            conn->RemoveVideoSource(memberToRemove->GetID());
          if (conn->IsVisible())
            memberToRemove->RemoveVideoSource(conn->GetID());
        }
#endif
      }
    }

#if OPENMCU_VIDEO
    if (moderated==FALSE){
      if (UseSameVideoForAllMembers() && memberToRemove->IsVisible())
        videoMixer->RemoveVideoSource(memberToRemove->GetID(), *memberToRemove);
    }
    else
     videoMixer->MyRemoveVideoSourceById(memberToRemove->GetID(),FALSE);
#endif

    // trigger H245 thread for leave message
//    if (memberToRemove->IsVisible())
//      new NotifyH245Thread(*this, FALSE, memberToRemove);

    terminalNumberMap.RemoveNumber(memberToRemove->GetTerminalNumber());


    // return TRUE if conference is empty 
    closeConference = memberList.size() == 0;
  }

  // notify that member is not joined anymore
  memberToRemove->SetJoined(FALSE);

  // call the callback function
  if (!closeConference)
    OnMemberLeaving(memberToRemove);
  else if(moderated==TRUE) videoMixer->MyRemoveAllVideoSource();

  return closeConference;
}


void Conference::ReadMemberAudio(ConferenceMember * member, void * buffer, PINDEX amount)
{
  // get number of channels to mix
  ConferenceMember::ConnectionListType & connectionList = member->GetConnectionList();
  ConferenceMember::ConnectionListType::iterator r;
  for (r = connectionList.begin(); r != connectionList.end(); ++r) 
    if (r->second != NULL)
    {
     if(moderated==FALSE || muteUnvisible==FALSE || videoMixer->GetPositionStatus(r->first)>=0) // default behaviour
      r->second->ReadAndMixAudio((BYTE *)buffer, amount, (PINDEX)connectionList.size(), 0);
    }  
}

// tint - time interval since last call in msec
void Conference::WriteMemberAudioLevel(ConferenceMember * member, unsigned audioLevel, int tint)
{
#if OPENMCU_VIDEO
  if (UseSameVideoForAllMembers()) {
    if (videoMixer != NULL)
    {
     int status = videoMixer->GetPositionStatus(member->GetID());
     if(audioLevel > VAlevel)
     {
//      cout << "audioLevel " << audioLevel << "\n";
      member->vad+=tint;
      if(member->vad >= VAdelay) // voice-on trigger delay
      {
//       cout << "VAD=" << member->vad << " status=" << status << "\n";
       if(status > 0) 
       {
        videoMixer->SetPositionStatus(member->GetID(),0);
       }
       else if(status == 0 && member->disableVAD == FALSE)
       {
        if(member->vad-VAdelay>500) // execute every 500 ms of voice activity
        {
         member->vad=VAdelay;
         videoMixer->SetVAD2Position(member->GetID());
        }
       }
       else if(status == -1 && member->disableVAD == FALSE) //find new vad position for active member
       {
        ConferenceMemberId id = videoMixer->SetVADPosition(member->GetID(),member->chosenVan,VAtimeout);
        if(id!=NULL) 
         { FreezeVideo(id); member->SetFreezeVideo(FALSE); }
       }
      }
     }
     else
     {
      member->vad=0;
      if(status >= 0) // increase silence counter
       videoMixer->SetPositionStatus(member->GetID(),status+tint);
     }
    }
  }
#endif // OPENMCU_VIDEO
}


#if OPENMCU_VIDEO

void Conference::ReadMemberVideo(ConferenceMember * member, void * buffer, int width, int height, PINDEX & amount)
{
  if (videoMixer == NULL)
    return;


// PTRACE(3, "Conference\tReadMemberVideo call 1" << width << "x" << height);

  if (videoMixer->ReadFrame(*member, buffer, width, height, amount))
    return;

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
  
}

BOOL Conference::WriteMemberVideo(ConferenceMember * member, const void * buffer, int width, int height, PINDEX amount)
{
  if (UseSameVideoForAllMembers()) {
    if (videoMixer != NULL)
      return videoMixer->WriteFrame(member->GetID(), buffer, width, height, amount);
  }
  else {
    PWaitAndSignal m(memberListMutex);
    MemberList::iterator r;
    for (r = memberList.begin(); r != memberList.end(); r++)
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
  MemberList::iterator r;
  if(id!=NULL)
  {
   r = memberList.find(id); if(r == memberList.end()) return;
   i=videoMixer->GetPositionStatus(id);
   if(i>=0) r->second->SetFreezeVideo(FALSE); else r->second->SetFreezeVideo(TRUE);
   return;
  }
  for (r = memberList.begin(); r != memberList.end(); r++) {
    i=videoMixer->GetPositionStatus(r->second->GetID());
    if(i>=0) r->second->SetFreezeVideo(FALSE); else r->second->SetFreezeVideo(TRUE);
  }
}

void Conference::PutChosenVan()
{ 
  int i;
  PWaitAndSignal m(memberListMutex);
  MemberList::iterator r;
  for (r = memberList.begin(); r != memberList.end(); r++) {
    if(r->second->chosenVan!=0) {
     i=videoMixer->GetPositionStatus(r->second->GetID());
     if(i < 0) videoMixer->SetVADPosition(r->second->GetID(),r->second->chosenVan,VAtimeout);
    } 
  }
}

///////////////////////////////////////////////////////////////////////////

ConferenceMember::ConferenceMember(Conference * _conference, ConferenceMemberId _id, BOOL _isMCU)
  : conference(_conference), id(_id), isMCU(_isMCU)
{
  audioLevel = 0;
  terminalNumber = -1;
  memberIsJoined = FALSE;

#if OPENMCU_VIDEO
  videoMixer = NULL;
  fsConverter = PColourConverter::Create("YUV420P", "YUV420P", CIF4_WIDTH, CIF4_HEIGHT);
  MCUVideoMixer::FillCIF4YUVFrame(memberFrameStores.GetFrameStore(CIF4_WIDTH, CIF4_HEIGHT).data.GetPointer(), 0, 0, 0);
  totalVideoFramesReceived = 0;
  firstFrameReceiveTime = -1;
  totalVideoFramesSent = 0;
  firstFrameSendTime = -1;
  vad = 0;
  muteIncoming = FALSE;
  disableVAD = FALSE;
  chosenVan = 0;
#endif
}

ConferenceMember::~ConferenceMember()
{
#if OPENMCU_VIDEO
  delete videoMixer;
#endif
}


BOOL ConferenceMember::AddToConference(Conference * _conference)
{
  //if (conference != NULL)
  //  return FALSE;
  //conference = _conference;

#if OPENMCU_VIDEO
  if (!conference->UseSameVideoForAllMembers())
    videoMixer = conference->GetVideoMixer()->Clone();
#endif

  return TRUE;
}

void ConferenceMember::RemoveFromConference()
{
  if (conference != NULL) {
    if (conference->RemoveMember(this))
      conference->GetManager().RemoveConference(conference->GetID());
  }
}

void ConferenceMember::AddConnection(ConferenceMember * memberToAdd)
{
  ConferenceMemberId newID = memberToAdd->GetID();
  PTRACE(3, "Conference\tAdding " << newID << " to connection " << id);
  if (lock.Wait(TRUE)) {
    ConferenceConnection * conn = memberToAdd->CreateConnection();
    memberList.insert(MemberListType::value_type(newID, memberToAdd));
    connectionList.insert(ConnectionListType::value_type(newID, conn));
    lock.Signal(TRUE);
  }
}

void ConferenceMember::RemoveConnection(ConferenceMemberId idToDelete)
{
  PTRACE(3, "Conference\tRemoving member " << idToDelete << " from connection " << id);
  if (lock.Wait(TRUE)) {
    memberList.erase(idToDelete);
    connectionList.erase(idToDelete);
    lock.Signal(TRUE);
  }
}

void ConferenceMember::RemoveAllConnections()
{
  PTRACE(3, "Conference\tRemoving all members from connection " << id);
  if (lock.Wait(TRUE)) {
    memberList.clear();
    connectionList.clear();
    lock.Signal(TRUE);
  }
}

void ConferenceMember::WriteAudio(const void * buffer, PINDEX amount)
{
  if(muteIncoming) return;
  // calculate average signal level for this member
  unsigned signalLevel = 0;
  {
    int sum = 0;
    int integr = 0;
    const short * pcm = (short *)buffer;
    const short * end = pcm + (amount / 2);
    while (pcm != end) {
//    cout << *pcm << "\n";
        integr +=*pcm;
      if (*pcm < 0)
        sum -= *pcm++;
      else
        sum += *pcm++;
    }
//  cout << "audioInegr = " << integr << "\n";
    signalLevel = sum/(amount/2);
  }
  audioLevel = ((signalLevel * 2) + audioLevel) / 3;
//  cout << "audioLevel = " << audioLevel << "\n";

  if (lock.Wait()) {

    if (conference != NULL)
      conference->WriteMemberAudioLevel(this, audioLevel, amount/32);

    MemberListType::iterator r;
    for (r = memberList.begin(); r != memberList.end(); ++r) {
      if (r->second != NULL)
        r->second->OnExternalSendAudio(id, (BYTE *)buffer, amount);
    }
    lock.Signal();
  }
}

void ConferenceMember::OnExternalSendAudio(ConferenceMemberId source, const void * buffer, PINDEX amount)
{
  if (lock.Wait()) {
    ConnectionListType::iterator r = connectionList.find(source);
    if (r != connectionList.end())
      if (r->second != NULL)
        r->second->Write((BYTE *)buffer, amount);
    lock.Signal();
  }
}

void ConferenceMember::ReadAudio(void * buffer, PINDEX amount)
{
  // First, set the buffer to empty.
  memset(buffer, 0, amount);

  if (lock.Wait()) {
    if (conference != NULL)
      conference->ReadMemberAudio(this, buffer, amount);
    lock.Signal();
  }
}

#if OPENMCU_VIDEO

// called whenever the connection needs a frame of video to send
void ConferenceMember::ReadVideo(void * buffer, int width, int height, PINDEX & amount)
{
  ++totalVideoFramesSent;
  if (!firstFrameSendTime.IsValid())
    firstFrameSendTime = PTime();
  if (lock.Wait()) {
    if (conference != NULL) {
      if (conference->UseSameVideoForAllMembers())
        conference->ReadMemberVideo(this, buffer, width, height, amount);
      else if (videoMixer != NULL)
        videoMixer->ReadFrame(*this, buffer, width, height, amount);
    }
    lock.Signal();
  }
}

// called whenever the connection receives a frame of video
void ConferenceMember::WriteVideo(const void * buffer, int width, int height, PINDEX amount)
{
  ++totalVideoFramesReceived;
  if (!firstFrameReceiveTime.IsValid())
    firstFrameReceiveTime = PTime();

  if (lock.Wait()) {
    if (conference != NULL) {
      if (!conference->WriteMemberVideo(this, buffer, width, height, amount)) {
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
  if (lock.Wait()) {
    videoMixer->WriteFrame(id, buffer, width, height, amount);
    lock.Signal();
  }
}

void * ConferenceMember::OnExternalReadVideo(ConferenceMemberId id, int width, int height, PINDEX & bytesReturned)
{
  if (!lock.Wait())
    return NULL;

  memberFrameStoreMutex.Wait();

  BOOL found;
  VideoFrameStoreList::FrameStore & nearestFs = memberFrameStores.GetNearestFrameStore(width, height, found);

  // if no valid framestores, nothing we can do
  if (!found) {
    memberFrameStoreMutex.Signal();
    lock.Signal();
    return NULL;
  }

  // if the valid framestore is a perfect match, return it
  if ((nearestFs.width == width) && (nearestFs.height == height))
    return nearestFs.data.GetPointer();

  // create a new destinationf framestore
  VideoFrameStoreList::FrameStore & destFs = memberFrameStores.GetFrameStore(width, height);

  MCUVideoMixer::ResizeYUV420P(nearestFs.data.GetPointer(), destFs.data.GetPointer(), nearestFs.width, nearestFs.height, width, height);
  destFs.valid = TRUE;
/*
  // if src is qcif, and dest is cif, then do special case
  if ((nearestFs.width == QCIF_WIDTH) && 
      (nearestFs.height == QCIF_HEIGHT) &&
      (width == CIF_WIDTH) && 
      (height == CIF_HEIGHT)) {
    MCUVideoMixer::ConvertQCIFToCIF(nearestFs.data.GetPointer(), destFs.data.GetPointer());
    destFs.valid = TRUE;
  }
  if ((nearestFs.width == QCIF_WIDTH) && 
      (nearestFs.height == QCIF_HEIGHT) &&
      (width == CIF4_WIDTH) && 
      (height == CIF4_HEIGHT)) {
    MCUVideoMixer::ConvertQCIFToCIF4(nearestFs.data.GetPointer(), destFs.data.GetPointer());
    destFs.valid = TRUE;
  }
  if ((nearestFs.width == CIF_WIDTH) && 
      (nearestFs.height == CIF_HEIGHT) &&
      (width == CIF4_WIDTH) && 
      (height == CIF4_HEIGHT)) {
    MCUVideoMixer::ConvertCIFToCIF4(nearestFs.data.GetPointer(), destFs.data.GetPointer());
    destFs.valid = TRUE;
  }

  // create an image that is the right size
  else {
    fsConverter->SetSrcFrameSize(nearestFs.width, nearestFs.height);
    fsConverter->SetDstFrameSize(width, height, TRUE);
    bytesReturned = width*height*3/2;
    fsConverter->Convert(nearestFs.data.GetPointer(), destFs.data.GetPointer(), nearestFs.width * nearestFs.height * 3 / 2, &bytesReturned);
    destFs.valid = TRUE;
  }
*/
  return destFs.data.GetPointer();
}

void ConferenceMember::UnlockExternalVideo()
{ 
  memberFrameStoreMutex.Signal(); 
  lock.Signal();
}

BOOL ConferenceMember::AddVideoSource(ConferenceMemberId id)
{
  PAssert(videoMixer != NULL, "attempt to add video source to NULL video mixer");
  return videoMixer->AddVideoSource(id, *this);
}

void ConferenceMember::RemoveVideoSource(ConferenceMemberId id)
{
  PAssert(videoMixer != NULL, "attempt to remove video source from NULL video mixer");
  videoMixer->RemoveVideoSource(id, *this);
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

///////////////////////////////////////////////////////////////////////////

ConferenceConnection::ConferenceConnection(ConferenceMemberId _id)
  : id(_id), bufferSize(PCM_BUFFER_SIZE)
{
  buffer = new BYTE[bufferSize];
  bufferStart = bufferLen = 0;
}

ConferenceConnection::~ConferenceConnection()
{
  delete[] buffer;
}

void ConferenceConnection::Write(const BYTE * data, PINDEX amount)
{
  if (amount == 0)
    return;

  PWaitAndSignal mutex(audioBufferMutex);
  
  // if there is not enough room for the new data, make room
  PINDEX newLen = bufferLen + amount;
  if (newLen > bufferSize) {
    PINDEX toRemove = newLen - bufferSize;
    bufferStart = (bufferStart + toRemove) % bufferSize;
    bufferLen -= toRemove;
  }

  // copy data to the end of the new data, up to the end of the buffer
  PINDEX copyStart = (bufferStart + bufferLen) % bufferSize;
  if ((copyStart + amount) > bufferSize) {
    PINDEX toCopy = bufferSize - copyStart;
    memcpy(buffer + copyStart, data, toCopy);
    copyStart = 0;
    data      += toCopy;
    amount    -= toCopy;
    bufferLen += toCopy;
  }

  // copy the rest of the data
  if (amount > 0) {
    memcpy(buffer + copyStart, data, amount);
    bufferLen   += amount;
  }
}


void ConferenceConnection::ReadAudio(BYTE * data, PINDEX amount)
{
  if (amount == 0)
    return;

  PWaitAndSignal mutex(audioBufferMutex);
  
  if (bufferLen == 0) {
    memset(data, 0, amount); // nothing in the buffer. return silence
    return;
  }

  // fill output data block with silence if audiobuffer is
  // almost empty.
  if (amount > bufferLen) 
    memset(data + bufferLen, 0, amount - bufferLen);

  // only copy up to the amount of data remaining
  PINDEX copyLeft = PMIN(amount, bufferLen);

  // if buffer is wrapping, get first part
  if ((bufferStart + copyLeft) > bufferSize) {
    PINDEX toCopy = bufferSize - bufferStart;

    memcpy(data, buffer + bufferStart, toCopy);

    data        += toCopy;
    bufferLen   -= toCopy;
    copyLeft    -= toCopy;
    bufferStart = 0;
  }

  // get the remainder of the buffer
  if (copyLeft > 0) {

    memcpy(data, buffer + bufferStart, copyLeft);

    bufferLen -= copyLeft;
    bufferStart = (bufferStart + copyLeft) % bufferSize;
  }
}

void ConferenceConnection::ReadAndMixAudio(BYTE * data, PINDEX amount, PINDEX channels, unsigned short echoLevel)
{
  if (amount == 0) {
    PTRACE(3, "Mixer\tNo data to read");
    return;
  }

  PWaitAndSignal mutex(audioBufferMutex);
  
  if (bufferLen == 0) {
    // nothing in the buffer to mix.
    return;
  }

  // only mix up to the amount of data remaining
  PINDEX copyLeft = PMIN(amount, bufferLen);

  // if buffer is wrapping, get first part
  if ((bufferStart + copyLeft) > bufferSize) {
    PINDEX toCopy = bufferSize - bufferStart;

    Mix(data, buffer + bufferStart, toCopy, channels, echoLevel);

    data        += toCopy;
    bufferLen   -= toCopy;
    copyLeft    -= toCopy;
    bufferStart = 0;
  }

  // get the remainder of the buffer
  if (copyLeft > 0) {

    Mix(data, buffer + bufferStart, copyLeft, channels, echoLevel);

    bufferLen -= copyLeft;
    bufferStart = (bufferStart + copyLeft) % bufferSize;
  }
}

void ConferenceConnection::Mix(BYTE * dst, const BYTE * src, PINDEX count, PINDEX /*channels*/, unsigned short echoLevel)
{
#if 0
  memcpy(dst, src, count);
#else
  PINDEX i;
  for (i = 0; i < count; i += 2) {

    int srcVal = *(short *)src;
    int dstVal = *(short *)dst;
    
//    cout << srcVal << "\t";
    
//    if(*(short *)src >= echoLevel) srcVal-=echoLevel;
//    else if(*(short *)src < -echoLevel) srcVal+=echoLevel;
//    else srcVal=0;
    
//    cout << srcVal << "\t";

    int newVal = dstVal;

#if 0     //The loudest person gains the channel.
#define mix_abs(x) ((x) >= 0 ? (x) : -(x))
    if (mix_abs(newVal) > mix_abs(srcVal))
      dstVal = newVal;
    else
      dstVal = srcVal; 
#else   //Just add up all the channels.
    if ((newVal + srcVal) > 0x7fff)
      dstVal = 0x7fff;
    else
    if ((newVal + srcVal) < -0x8000) dstVal = -0x8000;
    else
      dstVal += srcVal;
#endif
    *(short *)dst = (short)dstVal;

    dst += 2;
    src += 2;
  }
#endif
// cout << "\n";
}

///////////////////////////////////////////////////////////////

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
