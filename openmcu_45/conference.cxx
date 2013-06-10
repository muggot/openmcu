
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
#if ENABLE_TEST_ROOMS
  if(conference->GetNumber().Left(8)=="testroom")
  { PTRACE(6,"Conference\tOnCreateConference: \"testroom\" doesn't need extra features"); return; }
#endif
#if ENABLE_ECHO_MIXER
  if(conference->GetNumber().Left(4)*="echo")
  { PTRACE(6,"Conference\tOnCreateConference: \"ECHO\" room doesn't need extra features"); return; }
#endif
  new ConferenceFileMember(conference, (const PString) "recorder" , PFile::WriteOnly); 

  if(!OpenMCU::Current().GetForceScreenSplit())
  { PTRACE(6,"Conference\tOnCreateConference: \"Force split screen video\" unchecked, " << conference->GetNumber() << " skipping members.conf"); return; }

  FILE *membLst;
#ifdef SYS_CONFIG_DIR
#ifdef _WIN32
  PString name=SYS_CONFIG_DIR+PString("\\members_")+conference->GetNumber()+".conf";
#else
  PString name=SYS_CONFIG_DIR+PString("/members_")+conference->GetNumber()+".conf";
#endif
#else
  PString name="members_"+conference->GetNumber()+".conf";
#endif

  // read members.conf into conference->membersConf
  membLst = fopen(name,"rt");
  PStringStream membersConf;
  if(membLst!=NULL)
  { char buf [128];
    while(fgets(buf, 128, membLst)!=NULL) membersConf << buf;
    fclose(membLst);
  }
  conference->membersConf=membersConf;
  if(membersConf.Left(1)!="\n") membersConf="\n"+membersConf;

  // recall last template
  if(!OpenMCU::Current().recallRoomTemplate) return;
  PINDEX dp=membersConf.Find("\nLAST_USED ");
  if(dp!=P_MAX_INDEX)
  { PINDEX dp2=membersConf.Find('\n',dp+10);
    if(dp2!=P_MAX_INDEX)
    { PString lastUsedTemplate=membersConf.Mid(dp+11,dp2-dp-11).Trim();
      cout << "Extracting & loading last used template: " << lastUsedTemplate << "\n";
      conference->confTpl=conference->ExtractTemplate(lastUsedTemplate);
      conference->LoadTemplate(conference->confTpl);
      cout << "***" << conference->confTpl;
    }
  }
}

Conference * ConferenceManager::CreateConference(const OpalGloballyUniqueID & _guid,
                                                              const PString & _number,
                                                              const PString & _name,
                                                                          int _mcuNumber)
{ 
#if OPENMCU_VIDEO
#if ENABLE_ECHO_MIXER
  if (_number.Left(4) *= "echo")
    return new Conference(*this, _guid, "echo"+_guid.AsString(), _name, _mcuNumber, new EchoVideoMixer());
#endif
#if ENABLE_TEST_ROOMS
  if (_number.Left(8) == "testroom") {
    int count = _number.Mid(8).AsInteger();
    if (count > 0)
      return new Conference(*this, _guid, _number, _name, _mcuNumber, new TestVideoMixer(count));
  }
#endif
#endif

  if(!OpenMCU::Current().GetForceScreenSplit()) return new Conference(*this, _guid, _number, _name, _mcuNumber, NULL);

  PINDEX slashPos = _number.Find('/');
  PString number;
  if (slashPos != P_MAX_INDEX) number=_number.Left(slashPos);
  else number=_number;

  return new Conference(*this, _guid, number, _name, _mcuNumber
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

PString Conference::SaveTemplate(PString tplName)
{
  PTRACE(6,"Conference\tSaveTemplate");
  PStringStream t;
  Conference::MemberNameList::const_iterator s;
  t << "TEMPLATE " << tplName << "\n"
    << "{\n"
    << "  GLOBAL_MUTE " << (muteUnvisible?"on":"off") << "\n"
    << "  CONTROL_TYPE " << (moderated?"manual":"auto") << "\n"
    << "  VAD_VALUES " << VAdelay << ", " << VAtimeout << ", " << VAlevel << "\n";
  VideoMixerRecord * vmr = videoMixerList;
  while(vmr != NULL)
  { t << "  MIXER " << vmr->id << "\n"
      << "  {\n";
    MCUVideoMixer & m = *(vmr->mixer);
    unsigned n=m.GetPositionSet();
    VMPCfgSplitOptions & o = OpenMCU::vmcfg.vmconf[n].splitcfg;
    t << "    LAYOUT " << n << ", " << o.Id << "\n";
    unsigned skipCounter=0;
    for(unsigned i=0; i<o.vidnum; i++)
    { PStringStream vmpText;
      ConferenceMemberId id=m.GetPositionId(i);
      if((long)id==-1) vmpText << "VMP 2";
      else if((long)id==-2) vmpText << "VMP 3";
      else
      if(id!=NULL)
      { PWaitAndSignal m(memberListMutex);
        for(s = memberNameList.begin(); s != memberNameList.end(); ++s) if(s->second != NULL) if(s->second->GetID()==id)
        { vmpText << "VMP 1, " << s->first; break; }
      }
      if(vmpText=="") skipCounter++;
      else if(skipCounter>0)
      { if(skipCounter==1) t << "    VMP -\n";
        else t << "    SKIP " << skipCounter << "\n";
        skipCounter=0;
      }
      if(vmpText!="") t << "    " << vmpText << "\n";
    }
    if(skipCounter==1) t << "    VMP -\n";
    else if(skipCounter>1) t << "    SKIP " << skipCounter << "\n";
    t << "  }\n";
    vmr=vmr->next;
  }
  PWaitAndSignal m(memberListMutex);
  for(s = memberNameList.begin(); s != memberNameList.end(); ++s) if(s->second != NULL)
  t << "  MEMBER "
    << (s->second->autoDial?"1":"0") << ", "
    << (s->second->muteIncoming?"1":"0") << ", "
    << (s->second->disableVAD?"1":"0") << ", "
    << (s->second->chosenVan?"1":"0") << ", "
    << s->second->GetVideoMixerNumber() << ", "
    << s->first << "\n";
  t << "}\n\n";

  LoadTemplate(t); // temp fix

  return t;
}

void Conference::LoadTemplate(PString tpl)
{
  PTRACE(6,"Conference\tLoadTemplate");
  confTpl=tpl;
  if(tpl=="") return;
  PStringArray lines=tpl.Lines();
  int level=0;
  unsigned mixerId=0;
  unsigned maxMixerId=0;
  int vmpN=0;
  MCUVideoMixer * mixer = NULL;
  PStringArray validatedMembers;
  for(PINDEX i=0; i<lines.GetSize(); i++)
  {
    PString l=lines[i].Trim();
    if(l=="{") level++;
    else if(l=="}") level--;
    else
    {
      PINDEX space=l.Find(" ");
      if(space!=P_MAX_INDEX)
      {
        PString cmd=l.Left(space);
        PString value=l.Mid(space+1,P_MAX_INDEX).LeftTrim();
        if(cmd=="GLOBAL_MUTE") muteUnvisible=(value=="on");
        else if(cmd=="CONTROL_TYPE") moderated=(value=="manual");
        else if(cmd=="VAD_VALUES") {PStringArray v=value.Tokenise(",");if(v.GetSize()==3){ VAdelay=v[0].Trim().AsInteger(); VAtimeout=v[1].Trim().AsInteger(); VAlevel=v[2].Trim().AsInteger();}}
        else if(cmd=="MIXER")
        {
          mixerId=value.AsInteger();
          while((mixer=VMLFind(mixerId))==NULL) VMLAdd();
          if(maxMixerId<mixerId) maxMixerId=mixerId;
          vmpN=0;
        }
        else if(cmd=="LAYOUT") {if(mixer!=NULL) {PStringArray v=value.Tokenise(","); if(v.GetSize()==2) mixer->MyChangeLayout(v[0].Trim().AsInteger()); }}
        else if(cmd=="SKIP")
        { unsigned skipping=value.AsInteger();
          while(skipping>0)
          { if(mixer!=NULL) mixer->MyRemoveVideoSource(vmpN,TRUE);
            vmpN++;
            skipping--;
          }
        }
        else if(cmd=="VMP")
        {
          if (value=="-")
          {
            if(mixer!=NULL) mixer->MyRemoveVideoSource(vmpN,TRUE);
          }
          else if(value=="2") { mixer->SetPositionType(vmpN, 2); }
          else if(value=="3") { mixer->SetPositionType(vmpN, 3); }
          else
          { PINDEX commaPosition = value.Find(',');
            if(commaPosition != P_MAX_INDEX)
            { PString name=value.Mid(commaPosition+1,P_MAX_INDEX).LeftTrim();
              MemberNameList::iterator s = memberNameList.find(name);
              if(s!=memberNameList.end())
              {
                if(s->second!=NULL) // online
                {
                  if(mixer!=NULL)
                  {
                    mixer->PositionSetup(vmpN, 1, s->second->GetID());
                    s->second->SetFreezeVideo(FALSE);
                  }
                }
              }
            }
          }
          vmpN++;
        }
        else if(cmd=="MEMBER")
        {
          PStringArray v=value.Tokenise(",");
          if(v.GetSize()>4) for(int i=0; i<=4;i++) v[i]=v[i].Trim();

          BOOL memberAutoDial = (v[0]=="1");

          PString memberInternalName = v[5].Trim();
          for(int i=6; i<v.GetSize(); i++) memberInternalName += "," + v[i];

          PString memberIPAddress;
          PINDEX bp1 = memberInternalName.FindLast('[');
          if(bp1 != P_MAX_INDEX)
          {
            PINDEX bp2 = memberInternalName.FindLast(']');
            if(bp2 > bp1) memberIPAddress = memberInternalName.Mid(bp1+1,bp2-bp1-1);
          }

          PWaitAndSignal m(memberListMutex);
          MemberNameList::const_iterator r = memberNameList.find(memberInternalName);
          BOOL offline = (r == memberNameList.end());

          if(offline) memberNameList.insert(MemberNameList::value_type(memberInternalName, (ConferenceMember*)NULL)); else offline = (r->second == NULL);

          if(offline && memberAutoDial) // finally: offline and have to be called
          {
            PString token;
            PString numberWithMixer=number;
            if(v[4]!="0") numberWithMixer+="/"+v[4];
            PString * userData = new PString(numberWithMixer);
            if(OpenMCU::Current().GetEndpoint().MakeCall(memberIPAddress, token, userData) != NULL)
            {
              PStringStream msg; msg << "Inviting " << memberIPAddress;
              OpenMCU::Current().HttpWriteEventRoom(msg,number);
            }
          }
          if(!offline) // online: just tune him up
          {
            r->second->autoDial     = memberAutoDial;
            r->second->muteIncoming = (v[1]=="1");
            r->second->disableVAD   = (v[2]=="1");
            r->second->chosenVan    = (v[3]=="1");
            OpenMCU::Current().GetEndpoint().SetMemberVideoMixer(*this, r->second, v[4].AsInteger());
          }
          validatedMembers.AppendString(memberInternalName);

        } // else if(cmd=="MEMBER")

      } // if(space!=P_MAX_INDEX)

    } // if "{" else "if" } else ...

  } // for(i=0; i<lines.GetSize()

  BOOL tracingFirst=FALSE;
  for(unsigned i = videoMixerCount-1; i > maxMixerId; i--) // remove reduntant mixers
  {
    if(!tracingFirst)
    {
      PTRACE(6,"Template\tRemoving redundant mixers: " << (videoMixerCount-1) << " ... " << (maxMixerId+1) << flush);
      tracingFirst=TRUE;
    }
    VMLDel(i);
  }

  PWaitAndSignal m(memberListMutex);
  MemberNameList::const_iterator s = memberNameList.end();
  for (MemberNameList::const_iterator r = memberNameList.begin(); r != memberNameList.end(); ++r)
  {
    if(s!=memberNameList.end())
    {
      PTRACE(6,"Template\t  removing from memberNameList" << flush);
      memberNameList.erase(s->first);
      s=memberNameList.end();
    }
    if(validatedMembers.GetStringsIndex(r->first) == P_MAX_INDEX) // remove unwanted members
    {
      if(r->second == NULL) // offline: simple
      {
        PTRACE(6,"Template\tRemoving offline member " << r->first << " from list" << flush);
        memberNameList.erase(r->first);
      }
      else // online :(
      {
        PString memberName = r->first;
        ConferenceMember & member = *r->second;
        ConferenceMemberId id = member.GetID();
        PTRACE(6,"Template\tRemoving online member " << r->first << " (id " << id << ") from list" << flush);
        PTRACE(6,"Template\t  closing connection" << flush);
        member.Close();
        PTRACE(6,"Template\t  removing from memberList" << flush);
        memberList.erase(id);
        s=r;
      }
    }
  }
  if(s!=memberNameList.end())
  {
    PTRACE(6,"Template\t  removing from memberNameList" << flush);
    memberNameList.erase(s->first);
  }
}

PString Conference::GetTemplateList()
{
  PTRACE(6,"Conference\tGetTemplateList");
  PStringArray lines=membersConf.Lines();
  PStringStream result;
  result << "(";
  for(PINDEX i=0; i<lines.GetSize(); i++)
  {
    PString l = lines[i];
    if(l.Find("TEMPLATE ")==0)
    {
      PString tName = l.Mid(9,P_MAX_INDEX).Trim();
      tName.Replace("\\","\\x5c",TRUE,0);
      tName.Replace("\"","\\x22",TRUE,0);
      if(result.GetLength()>1) result << ",";
      result << "\"" << tName << "\"";
    }
  }
  result << ")";
  return result;
}

PString Conference::GetSelectedTemplateName()
{
  PTRACE(6,"Conference\tGetSelectedTemplateName");
  if(confTpl.Trim()=="") return "";
  PINDEX tp, lfp;
  tp = confTpl.Find("TEMPLATE ");
  if (tp == P_MAX_INDEX) return "";
  lfp = confTpl.Find('\n',tp+9);
  if(lfp == P_MAX_INDEX) return "";
  PString result=confTpl.Mid(tp+9,lfp-tp-9);
  result = result.Trim();
  result.Replace("\\","\\x5c",TRUE,0);
  result.Replace("\"","\\x22",TRUE,0);
  return result;
}

PString Conference::ExtractTemplate(PString tplName) // returns single template extracted from membersConf
{
  PTRACE(6,"Conference\tExtractTemplate");
  if(tplName=="") return "";
  PStringArray lines=membersConf.Lines();
  PINDEX i;
  for(i=0;i<lines.GetSize();i++)
  {
    PString s=lines[i].LeftTrim();
    PINDEX sp=s.Find(" ");
    if(sp==P_MAX_INDEX) continue;
    if(s.Left(sp)=="TEMPLATE") if(s.Mid(sp+1,P_MAX_INDEX).Trim()==tplName)
    {
      PStringStream result;
      result << s << "\n";
      PINDEX j;
      int level=-1;
      for(j=i+1; j<lines.GetSize(); j++)
      {
        result << lines[j] << "\n";
        PString s=lines[j].Trim();
        if(s=="{")
        { if(level==-1)level++; level++; }
        else if(s=="}") level--;
        if(level==0) return result;
      }
    }
  }
 return "";
}

void Conference::PullMemberOptionsFromTemplate(ConferenceMember * member, PString tpl)
{
  PTRACE(6,"Conference\tPullMemberOptionsFromTemplate");
  if(member==NULL) return;
  if(tpl.Trim()=="") return;
  PStringArray lines=tpl.Lines();
  PString memberName = member->GetName();
  unsigned mixerCounter=0;
  unsigned vmpCounter=0;
  PINDEX i;
  for(i=0;i<lines.GetSize();i++)
  {
    PString l = lines[i].Trim();
    PINDEX sp = l.Find(' ');
    if(sp==P_MAX_INDEX) continue;
    PString cmd = l.Left(sp);
    if(cmd=="MIXER") { mixerCounter++; vmpCounter=0; }
    else if(cmd=="VMP")
    {
      vmpCounter++;
      PString p=l.Mid(sp+1,P_MAX_INDEX).Trim();
      PINDEX cp=p.Find(',');
      if(cp==P_MAX_INDEX) continue;
      PString vmpMemberName=p.Mid(cp+1,P_MAX_INDEX).LeftTrim();
      if(vmpMemberName==memberName)
      {
        if(mixerCounter>0)
        {
          MCUVideoMixer * mixer = VMLFind(mixerCounter-1);
          if(mixer != NULL)
          {
            mixer->PositionSetup(vmpCounter-1, 1, member->GetID());
            member->SetFreezeVideo(FALSE);
          }
        }
      }
    }
    else if(cmd=="SKIP") vmpCounter+=l.Mid(sp+1,P_MAX_INDEX).Trim().AsInteger();
    else if(cmd=="MEMBER")
    {
      PStringArray v=l.Mid(sp+1,P_MAX_INDEX).LeftTrim().Tokenise(',');
      if(v.GetSize()>4) for(int i=0; i<=4;i++) v[i]=v[i].Trim();
      PString iterationMemberName = v[5].LeftTrim();
      for (PINDEX j=6; j<v.GetSize(); j++) iterationMemberName+=","+v[j];
      if(iterationMemberName == memberName)
      {
        member->autoDial     = (v[0] == "1");
        member->muteIncoming = (v[1] == "1");
        member->disableVAD   = (v[2] == "1");
        member->chosenVan    = (v[3] == "1");

// As we assume PullMemberOptionsFromTemplate() called from AddMember(), we don't need to SWITCH mixer here.
// That's why the following is commented and the next will just make member->videoMixerNumber set.
// Right mixer will be attached to connection via userData value during making the call.

//        OpenMCU::Current().GetEndpoint().SetMemberVideoMixer(*this, member, v[4].AsInteger());
        member->SetVideoMixerNumber(v[4].AsInteger());

        return;
      }
    }
  }
}

void Conference::TemplateInsertAndRewrite(PString tplName, PString tpl)
{
  PTRACE(6,"Conference\tTemplateInsertAndRewrite");
  PStringArray lines=membersConf.Lines();
  PINDEX i, start, stop, lastUsed;
  start=P_MAX_INDEX;
  stop=P_MAX_INDEX;
  lastUsed=P_MAX_INDEX;

  for(i=0;i<lines.GetSize();i++)
  {
    PString s=lines[i].LeftTrim();
    PINDEX sp=s.Find(' ');
    if(sp==P_MAX_INDEX) continue;
    PString cmd=s.Left(sp);
    if(cmd=="LAST_USED") lastUsed=i;
    if(start==P_MAX_INDEX) if(cmd=="TEMPLATE") if(s.Mid(sp+1,P_MAX_INDEX).Trim()==tplName)
    {
      start = i;
      PINDEX j;
      int level=-1;
      for(j=i+1; j<lines.GetSize(); j++)
      {
        PString s=lines[j].Trim();
        if(s=="{")
        { if(level==-1)level++; level++; }
        else if(s=="}") level--;
        if(level==0) { stop=j+1; break; }
      }
    }
  }

  PStringStream result;
  i = 0;
  int lfc=0;

  if(start!=P_MAX_INDEX)
  {
    while (i < start)
    {
      if(lines[i]=="") lfc++; else lfc=0;
      if(lfc<2) if(i!=lastUsed) result << lines[i] << "\n";
      i++;
    }
    if(lfc == 0) { result << "\n"; lfc=1; }
    i=stop;
  }

  while (i < lines.GetSize())
  {
    PString s = lines[i].LeftTrim();
    if(s=="") lfc++; else lfc=0;
    if(lfc<2) if(lastUsed!=i) result << lines[i] << "\n";
    i++;
  }
  membersConf = result;

  while (membersConf.Right(2)!="\n\n") membersConf+="\n";

  while (membersConf.Left(1)=="\n") membersConf=membersConf.Right(membersConf.GetLength()-1);

  membersConf += tpl;

  while (membersConf.Right(2)!="\n\n") membersConf+="\n";

  membersConf+="LAST_USED "+tplName+"\n";

  RewriteMembersConf();
}

void Conference::SetLastUsedTemplate(PString tplName)
{
  PTRACE(6,"Conference\tSetLastUsedTemplate");
  PStringArray lines=membersConf.Lines();
  BOOL set=FALSE;
  PStringStream result;
  for (PINDEX i=0; i<lines.GetSize(); i++)
  {
    PString s=lines[i].LeftTrim();
    PINDEX sp=s.Find(' ');
    if(sp!=P_MAX_INDEX)
    {
      PString cmd=s.Left(sp);
      if(cmd=="LAST_USED")
      {
        if(!set)
        {
          result << "LAST_USED " << tplName << "\n";
          set=TRUE;
        }
      } else
      {
        result << lines[i] << "\n";
      }
    } else
    {
      result << lines[i] << "\n";
    }
  }
  if(!set)
  {
    while(result.Right(2)!="\n\n") result << "\n";
    result << "LAST_USED " << tplName << "\n";
  }
  membersConf = result;
  RewriteMembersConf();
}

void Conference::DeleteTemplate(PString tplName)
{
  PTRACE(6,"Conference\tDeleteTemplate");
  PStringArray lines=membersConf.Lines();
  PStringStream result;
  int inside=0;
  int level=0;
  for(PINDEX i=0; i<lines.GetSize(); i++)
  {
    PString s=lines[i].Trim();
    PString cmd="";
    PINDEX sp = s.Find(' ');
    if(sp!=P_MAX_INDEX) cmd=s.Left(sp);
    if(s=="{") level++;
    if(s=="}") if(level>0) level--;
    if(cmd=="TEMPLATE") if(s.Mid(sp+1,P_MAX_INDEX).Trim()==tplName) inside=2;

    if(inside==1) if(s!="") inside=0;

    if(!inside) result << lines[i] << "\n";

    if(inside==1) inside=0;
    if(inside) if(s=="}") if(level==0) inside=1;
  }
  membersConf=result;
  RewriteMembersConf();
}

BOOL Conference::RewriteMembersConf()
{
  PTRACE(6,"Conference\tRewriteMembersConf");
  FILE *membLst;
#ifdef SYS_CONFIG_DIR
#ifdef _WIN32
  PString name=SYS_CONFIG_DIR+PString("\\members_")+number+".conf";
#else
  PString name=SYS_CONFIG_DIR+PString("/members_")+number+".conf";
#endif
#else
  PString name="members_"+number+".conf";
#endif
  PWaitAndSignal m(membersConfMutex);
  membLst = fopen(name,"w");
  if (membLst==NULL) return FALSE;
  int result = fputs(membersConf,membLst);
  fclose(membLst);
  return (result >= 0);
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
{ 
#if OPENMCU_VIDEO
  VMLInit(_videoMixer);
#endif
  maxMemberCount = 0;
  moderated = FALSE;
  muteUnvisible = FALSE;
  VAdelay = 1000;
  VAtimeout = 10000;
  VAlevel = 100;
  echoLevel = 0;
  vidmembernum = 0;
  externalRecorder=NULL;

  PTRACE(3, "Conference\tNew conference started: ID=" << guid << ", number = " << number);
}

Conference::~Conference()
{
#if OPENMCU_VIDEO
  VMLClear();
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
 PStringStream msg;
 msg << "Inviting " << address;
 OpenMCU::Current().HttpWriteEventRoom(msg,number);
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

    ConferenceMemberId mid = memberToAdd->GetID();

    r = memberList.find(mid);
    if(r != memberList.end()) return FALSE;

#if OPENMCU_VIDEO
//    if(!UseSameVideoForAllMembers()) memberToAdd->videoStatus = 1;

    if (moderated==FALSE)
    {
     if (UseSameVideoForAllMembers() && memberToAdd->IsVisible()) {
      if (!videoMixerList->mixer->AddVideoSource(mid, *memberToAdd)) 
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
          {
            memberToAdd->AddVideoSource(conn->GetID());
          }
          if (memberToAdd->IsVisible())
          {
            conn->AddVideoSource(memberToAdd->GetID());
          }
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

  PStringStream msg;

  // add this member to the conference member name list
  if(memberToAdd!=memberToAdd->GetID())
  {
    PString username=memberToAdd->GetName(); username.Replace("&","&amp;",TRUE,0); username.Replace("\"","&quot;",TRUE,0);
    msg="addmmbr(1,"; msg << (long)memberToAdd->GetID()
     << ",\"" << username << "\"," << memberToAdd->muteIncoming
     << "," << memberToAdd->disableVAD << ","
     << memberToAdd->chosenVan << ","
     << memberToAdd->GetAudioLevel() << ","
     << memberToAdd->GetVideoMixerNumber() << ")";
    OpenMCU::Current().HttpWriteCmdRoom(msg,number);

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


  if(memberToAdd->IsVisible())
  {
    PullMemberOptionsFromTemplate(memberToAdd, confTpl);
  }

  msg = "<font color=green><b>+</b>";
  msg << memberToAdd->GetName() << "</font>"; OpenMCU::Current().HttpWriteEventRoom(msg,number);
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
    if(memberToRemove!=memberToRemove->GetID() && s->second==memberToRemove && s!=memberNameList.end())
     {
       memberNameList.erase(memberToRemove->GetName());
       memberNameList.insert(MemberNameList::value_type(memberToRemove->GetName(),zerop));
     }

    PStringStream msg; msg << "<font color=red><b>-</b>" << memberToRemove->GetName() << "</font>"; OpenMCU::Current().HttpWriteEventRoom(msg,number);
    PString username=memberToRemove->GetName(); username.Replace("&","&amp;",TRUE,0); username.Replace("\"","&quot;",TRUE,0);
    msg="remmmbr(0,"; msg << (long)memberToRemove->GetID()
     << ",\"" << username << "\"," << memberToRemove->muteIncoming
     << "," << memberToRemove->disableVAD << ","
     << memberToRemove->chosenVan << ","
     << memberToRemove->GetAudioLevel();
    if(s==memberNameList.end()) msg << ",1";
    msg << ")";
    OpenMCU::Current().HttpWriteCmdRoom(msg,number);


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
        videoMixerList->mixer->RemoveVideoSource(memberToRemove->GetID(), *memberToRemove);
    }
    else
    {
      VideoMixerRecord * vmr=videoMixerList; while(vmr!=NULL)
      {
        vmr->mixer->MyRemoveVideoSourceById(memberToRemove->GetID(),FALSE);
        vmr=vmr->next;
      }
    }
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
  else if(moderated==TRUE) {
    VideoMixerRecord * vmr=videoMixerList; while(vmr!=NULL)
    {
      vmr->mixer->MyRemoveAllVideoSource();
      vmr=vmr->next;
    }
  }

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
     if(moderated==FALSE || muteUnvisible==FALSE || videoMixerList->mixer->GetPositionStatus(r->first)>=0) // default behaviour
      r->second->ReadAndMixAudio((BYTE *)buffer, amount, (PINDEX)connectionList.size(), 0);
    }
}

// tint - time interval since last call in msec
void Conference::WriteMemberAudioLevel(ConferenceMember * member, unsigned audioLevel, int tint)
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
#if OPENMCU_VIDEO
  if (UseSameVideoForAllMembers()) {
    if (videoMixerList != NULL)
    {
      if(audioLevel > VAlevel) member->vad+=tint;
      else member->vad=0;
      VideoMixerRecord * vmr=videoMixerList; while(vmr!=NULL)
      {
         MCUVideoMixer * videoMixer = vmr->mixer;
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
                videoMixer->SetVAD2Position(member->GetID());
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
          if(status >= 0) // increase silence counter
            videoMixer->SetPositionStatus(member->GetID(),status+tint);
        }
        if(audioLevel > VAlevel) if(status==0 && member->disableVAD==FALSE) if(member->vad-VAdelay>500) member->vad=VAdelay;
        vmr=vmr->next;
      }
    }
  }
#endif // OPENMCU_VIDEO
}


#if OPENMCU_VIDEO

void Conference::ReadMemberVideo(ConferenceMember * member, void * buffer, int width, int height, PINDEX & amount)
{
  if (videoMixerList == NULL)
    return;

// PTRACE(3, "Conference\tReadMemberVideo call 1" << width << "x" << height);
  unsigned mixerNumber; if(member==NULL) mixerNumber=0; else mixerNumber=member->GetVideoMixerNumber();
  MCUVideoMixer * mixer = VMLFind(mixerNumber);
  if (mixer!=NULL) {
    mixer->ReadFrame(*member, buffer, width, height, amount);
    return;
  }

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
    if (videoMixerList != NULL) {
      VideoMixerRecord * vmr = videoMixerList; BOOL writeResult=FALSE;
      while(vmr!=NULL)
      { writeResult |= vmr->mixer->WriteFrame(member->GetID(), buffer, width, height, amount);
        vmr=vmr->next;
      }
      return writeResult;
    }
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
  MemberList::iterator r,s;
  if(id!=NULL)
  {
    r = memberList.find(id); if(r == memberList.end()) return;
    VideoMixerRecord * vmr = videoMixerList; while(vmr!=NULL)
    { i=vmr->mixer->GetPositionStatus(id);
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
  for (r = memberList.begin(); r != memberList.end(); r++) {
    ConferenceMemberId mid=r->second->GetID();
    VideoMixerRecord * vmr = videoMixerList; while(vmr!=NULL){
      i=vmr->mixer->GetPositionStatus(mid);
      if(i>=0) {
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

void Conference::PutChosenVan()
{
  int i;
  PWaitAndSignal m(memberListMutex);
  MemberList::iterator r;
  for (r = memberList.begin(); r != memberList.end(); ++r) {
    if(r->second->chosenVan) {
      VideoMixerRecord * vmr = videoMixerList;
      while(vmr!=NULL){
        i=vmr->mixer->GetPositionStatus(r->second->GetID());
        if(i < 0) vmr->mixer->SetVADPosition(r->second->GetID(),r->second->chosenVan,VAtimeout);
        vmr = vmr->next;
      }
    }
  }
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
    int pos=videoMixerList->mixer->GetPositionNum(id);
    if(pos==posTo) return;
    videoMixerList->mixer->InsertVideoSource(id,posTo);
    FreezeVideo(NULL);
    OpenMCU::Current().HttpWriteCmdRoom(OpenMCU::Current().GetEndpoint().GetConferenceOptsJavascript(*this),number);
    OpenMCU::Current().HttpWriteCmdRoom("build_page()",number);
  }
}


///////////////////////////////////////////////////////////////////////////

ConferenceMember::ConferenceMember(Conference * _conference, ConferenceMemberId _id, BOOL _isMCU)
  : conference(_conference), id(_id), isMCU(_isMCU)
{
  audioLevel = 0;
  audioCounter = 0; previousAudioLevel = 65535; audioLevelIndicator = 0;
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
  rxFrameWidth = 0; rxFrameHeight = 0;
  vad = 0;
  autoDial = FALSE;
  muteIncoming = FALSE;
  disableVAD = FALSE;
  chosenVan = 0;
  videoMixerNumber = 0;
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
    videoMixer = new MCUSimpleVideoMixer();
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
  rxFrameWidth=width; rxFrameHeight=height;
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
