#include <ptlib.h>
#include "conference.h"
#include "mcu.h"

PString Conference::SaveTemplate(PString tplName)
{
  PTRACE(4,"Conference\tSaving template \"" << tplName << "\"");
  PStringArray previousTemplate = confTpl.Lines();
  PStringStream t;
  Conference::MemberNameList::const_iterator s;
  t << "TEMPLATE " << tplName << "\n"
    << "{\n"
    << "  GLOBAL_MUTE " << (muteUnvisible?"on":"off") << "\n"
    << "  CONTROL_TYPE " << (moderated?"manual":"auto") << "\n"
    << "  VAD_VALUES " << VAdelay << ", " << VAtimeout << ", " << VAlevel << "\n";
  PWaitAndSignal m3(videoMixerListMutex);
  VideoMixerRecord * vmr = videoMixerList;
  while(vmr != NULL)
  { t << "  MIXER " << vmr->id << "\n"
      << "  {\n";
    MCUVideoMixer & m = *(vmr->mixer);
    unsigned n=m.GetPositionSet();
    VMPCfgSplitOptions & o = OpenMCU::vmcfg.vmconf[n].splitcfg;
    PString newLayout=PString(n) + ", " + o.Id;
    t << "    LAYOUT " << newLayout << "\n";
    unsigned skipCounter=0;
    for(unsigned i=0; i<o.vidnum; i++)             // video mix position will be set here:
    { PStringStream vmpText;
      ConferenceMemberId id=m.GetPositionId(i);
      if((long)id==-1) vmpText << "VMP 2";         // - it may be "VMP 2" (VAD type)
      else if((long)id==-2) vmpText << "VMP 3";    // - or        "VMP 3" (VAD2 type)
      else
      if(id!=NULL)                                 // - or        "VMP 1, memberName" (static type)
      { PWaitAndSignal m(memberListMutex);
        for(s = memberNameList.begin(); s != memberNameList.end(); ++s) if(s->second != NULL) if(s->second->GetID()==id)
        { vmpText << "VMP 1, " << s->first; break; }
      }
      else                                         // - nothing: trying get the value from current template:
      { int previous_level=0;
        unsigned previous_mixer=32767;
        unsigned prev_vmpN=32767;
        BOOL mixMatch=FALSE;
        for(PINDEX previous_i=0; previous_i<previousTemplate.GetSize(); previous_i++)
        { PString prev_l=previousTemplate[previous_i].Trim();
          if     (prev_l=="{") previous_level++;
          else if(prev_l=="}") previous_level--;
          else
          { PINDEX space=prev_l.Find(" ");
            if(space!=P_MAX_INDEX)
            { PString cmd=prev_l.Left(space);
              PString value=prev_l.Mid(space+1,P_MAX_INDEX).LeftTrim();
              if     (cmd=="MIXER")
              { previous_mixer=value.AsInteger();
                mixMatch=(previous_mixer==vmr->id);
                prev_vmpN=0;
              }
              else if(cmd=="LAYOUT") { if(mixMatch) mixMatch=(value==newLayout); }
              else if(cmd=="SKIP") prev_vmpN+=value.AsInteger();
              else if(cmd=="VMP")
              { if(mixMatch) if(prev_vmpN == i) if(value.Left(1)=="1")
                { PWaitAndSignal m(memberListMutex);
                  for(s = memberNameList.begin(); s != memberNameList.end(); ++s)
                  if(s->second == NULL) if(("1, "+(s->first)) == value)
                  { vmpText << "VMP " << value;
                    break;
                  }
                }
                prev_vmpN++;
              }
            } // if(space!=P_MAX_INDEX)
          } // if "{" else "if" } else ...
        } // for(i=0; i<lines.GetSize()
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
  for(s = memberNameList.begin(); s != memberNameList.end(); ++s)
  {
    if(s->second != NULL)
    {
      t << "  MEMBER "
        << (s->second->autoDial?"1":"0") << ", "
        << s->second->muteMask << ", "
        << (s->second->disableVAD?"1":"0") << ", "
        << (s->second->chosenVan?"1":"0") << ", "
        << s->second->GetVideoMixerNumber() << ", "
        << s->first << "\n";
    }
    else
    {
      BOOL memberFound = FALSE;
      for(PINDEX previous_i=0; previous_i<previousTemplate.GetSize(); previous_i++)
      { PString prev_l=previousTemplate[previous_i].Trim();
        PINDEX space=prev_l.Find(" ");
        if(space!=P_MAX_INDEX)
        { PString cmd=prev_l.Left(space);
          if(cmd=="MEMBER")
          { PString value=prev_l.Mid(space+1,P_MAX_INDEX).LeftTrim();
            PStringArray options=value.Tokenise(',',TRUE);
            if(options.GetSize()==6)
            if(options[5].LeftTrim()==PString(s->first).LeftTrim())
            { t << "  MEMBER " << value << "\n";
              memberFound = TRUE;
              break;
            }
          }
        }
      }
      if(!memberFound) t << "  MEMBER 0, 0, 0, 0, 0, " << s->first << "\n";
    }
  }
  t << "}\n\n";

  LoadTemplate(t); // temp fix

  return t;
}

void Conference::LoadTemplate(PString tpl)
{
  PTRACE(4,"Conference\tLoading template");
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
        else if(cmd=="VAD_VALUES")
        {
          PStringArray v=value.Tokenise(","); if(v.GetSize()==3)
          {
            VAdelay=(unsigned short int)(v[0].Trim().AsInteger());
            VAtimeout=(unsigned short int)(v[1].Trim().AsInteger());
            VAlevel=(unsigned short int)(v[2].Trim().AsInteger());
        } }
        else if(cmd=="MIXER")
        {
          mixerId=value.AsInteger();
          while((mixer=VMLFind(mixerId))==NULL) VMLAdd();
          if(maxMixerId<mixerId) maxMixerId=mixerId;
          vmpN=0;
        }
        else if(cmd=="LAYOUT")
        {
          if(mixer!=NULL)
          {
            PStringArray v=value.Tokenise(",");
            if(v.GetSize()==2) mixer->MyChangeLayout(v[0].Trim().AsInteger());
        } }
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
              PWaitAndSignal m(memberListMutex);
              MemberNameList::iterator s;
              for(s = memberNameList.begin(); s != memberNameList.end(); ++s)
              {
                if(s->second!=NULL) // online
                {

// name:                        ВАСИЛИЙ ИВАНОВИЧ [h323:undefined@1.2.3.55]
// s->first:                    ВАСИЛИЙ ИВАНОВИЧ [h323:undefined@1.2.4.55]
// MCUURL(name).GetUrlId():     h323:undefined
// MCUURL(s->first).GetUrlId(): h323:undefined

//                  if(MCUURL(name).GetUrlId() == MCUURL(s->first).GetUrlId())
                  if(name == s->first)
                    break;
                }
              }
              if(s!=memberNameList.end())
              {
                if(s->second!=NULL) // online
                {
                  if(mixer!=NULL)
                  {
                    mixer->PositionSetup(vmpN, 1, s->second);
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
          PString memberAddress = MCUURL(memberInternalName).GetUrl();

          PWaitAndSignal m(memberListMutex);
          MemberNameList::const_iterator r;
          for(r = memberNameList.begin(); r != memberNameList.end(); ++r)
          {
            if(r->second!=NULL) // online
            {
              if(MCUURL(memberInternalName).GetUrlId() == MCUURL(r->first).GetUrlId())
              {
                memberInternalName = r->first;
                break;
              }
            }
          }
          BOOL offline = (r == memberNameList.end());

          if(offline) memberNameList.insert(MemberNameList::value_type(memberInternalName, (ConferenceMember*)NULL));
          else offline = (r->second == NULL);

          if(offline && memberAutoDial) // finally: offline and have to be called
          {
            PString token;
            PString numberWithMixer=number;
            if(v[4]!="0") numberWithMixer+="/"+v[4];
            PString * userData = new PString(numberWithMixer);
//            if(OpenMCU::Current().GetEndpoint().MakeCall(memberAddress, token, userData) != NULL)
            if(InviteMember(memberAddress, userData))
            {
//              PStringStream msg; msg << "Inviting " << memberAddress;
//              OpenMCU::Current().HttpWriteEventRoom(msg,number);
            }
          }
          if(!offline) // online: just tune him up
          {
            r->second->autoDial     = memberAutoDial;
            r->second->muteMask     = v[1].AsInteger();
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
      PTRACE(6,"Conference\tLoading template - currently active video mixers from " << (videoMixerCount-1) << " up to " << (maxMixerId+1) << " will be removed" << flush);
      tracingFirst=TRUE;
    }
    VMLDel(i);
  }

  if(!lockedTemplate) return; // room not locked - don't touch member list

  PWaitAndSignal m(memberListMutex);
  MemberNameList theCopy(memberNameList);
  for(MemberNameList::iterator r = theCopy.begin(); r != theCopy.end(); ++r)
  { if(validatedMembers.GetStringsIndex(r->first) == P_MAX_INDEX) // remove unwanted members
    { if(r->second == NULL) // offline: simple
      { PTRACE(6,"Conference\tLoading template - removing offline member " << r->first << " from memberNameList" << flush);
        memberNameList.erase(r->first);
      }
      else // online :(
      { ConferenceMember & member = *r->second;
        ConferenceMemberId id = member.GetID();
        PTRACE(6,"Conference\tLoading template - closing connection with " << r->first << " (id " << id << ")" << flush);
        member.Close();
        PTRACE(6,"Conference\tLoading template - removing " << r->first << " from memberList" << flush);
        memberList.erase(id);
        memberNameList.erase(r->first);
      }
    }
  }
  RefreshAddressBook();
}

PString Conference::GetTemplateList()
{
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
{ if(tplName=="") return "";
  PStringArray lines=membersConf.Lines();
  PINDEX i;
  for(i=0;i<lines.GetSize();i++)
  { PString s=lines[i].LeftTrim();
    PINDEX sp=s.Find(" ");
    if(sp==P_MAX_INDEX) continue;
    if(s.Left(sp)=="TEMPLATE") if(s.Mid(sp+1,P_MAX_INDEX).Trim()==tplName)
    { PStringStream result;
      result << s << "\n";
      PINDEX j;
      int level=-1;
      for(j=i+1; j<lines.GetSize(); j++)
      { result << lines[j] << "\n";
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
      if(MCUURL(vmpMemberName).GetUrlId() == MCUURL(memberName).GetUrlId())
      {
        if(mixerCounter>0)
        {
          MCUVideoMixer * mixer = VMLFind(mixerCounter-1);
          if(mixer != NULL)
          {
            mixer->PositionSetup(vmpCounter-1, 1, member);
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
      if(MCUURL(iterationMemberName).GetUrlId() == MCUURL(memberName).GetUrlId())
      {
        member->autoDial     = (v[0] == "1");
        member->muteMask     = v[1].AsInteger();
        member->disableVAD   = (v[2] == "1");
        member->chosenVan    = (v[3] == "1");
        if(member->chosenVan) if(PutChosenVan()) member->SetFreezeVideo(FALSE);

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
  PTRACE(6,"Conference\tDeleteTemplate: " << tplName);
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
  PString name=PString(SYS_CONFIG_DIR) + PATH_SEPARATOR + "members_" + number + ".conf";
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

void Conference::OnConnectionClean(const PString & remotePartyName, const PString & remotePartyAddress)
{
  PTRACE(4,"Conference\tOnConnectionClean: " << remotePartyName << " / " << remotePartyAddress);
  PString name;
  if(!remotePartyName.IsEmpty()) if(remotePartyName != remotePartyAddress) name += remotePartyName;
  if (name.Right(1)!="]")
  {
    PString url = remotePartyAddress;

    PINDEX i = url.Find("ip$");
    if(i != P_MAX_INDEX) url=url.Mid(i+3);

    if(url.Mid(1,6).Find(':') == P_MAX_INDEX) // no url prefix :(
    {
      if(url.Find('@') == P_MAX_INDEX) url=PString("@")+url;
      { // will used defaut prefix "h323:", fix it
        url=PString("h323:") + url;
        if(url.Right(5)==":1720") url=url.Left(url.GetLength()-5);
      }
    }

    if(!name.IsEmpty()) name+=' ';
    name += '[' + url +']';
  }

  PWaitAndSignal m(memberListMutex);
  Conference::MemberNameList::iterator q = memberNameList.find(name);
  if(q == memberNameList.end())
  {
    for(q=memberNameList.begin(); q!=memberNameList.end(); ++q)
    {
      if(q->first.FindLast(name) != P_MAX_INDEX)
      {
        name = q->first;
        break;
      }
    }
    if(q == memberNameList.end())
    {
      PTRACE(1,"Conference\tCould not match party name: " << remotePartyName << ", address: " << remotePartyAddress << ", result: " << name);
      return;
    }
  }
  if(q->second != NULL)
  {
    PTRACE(2,"Conference\tMember found in the list, but it's not offine (nothing to do): " << remotePartyName << ", address: " << remotePartyAddress << ", result: " << name);
    return;
  }

  if(confTpl.Trim().IsEmpty()) return;

  BOOL autoDial = FALSE;

  PStringArray lines=confTpl.Lines();
  PINDEX i;
  for(i=0;i<lines.GetSize();i++)
  {
    PString l = lines[i].Trim();
    PINDEX sp = l.Find(' ');
    if(sp==P_MAX_INDEX) continue;
    PString cmd = l.Left(sp);
    if(cmd=="MEMBER")
    {
      PStringArray v=l.Mid(sp+1,P_MAX_INDEX).LeftTrim().Tokenise(',');
      if(v.GetSize()>4) for(int i=0; i<=4;i++) v[i]=v[i].Trim();
      PString iterationMemberName = v[5].LeftTrim();
      for (PINDEX j=6; j<v.GetSize(); j++) iterationMemberName+=","+v[j];
      if(iterationMemberName == name)
      {
        autoDial = (v[0] == "1");
        break;
      }
    }
  }

  if(!autoDial) return;

  PTRACE(2,"Conference\tGetting back member " << name);
  InviteMember(name);
}






