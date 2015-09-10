
#include "precompile.h"
#include "conference.h"
#include "mcu.h"

PString Conference::SaveTemplate(PString tplName)
{
  PTRACE(4,"Conference\tSaving template \"" << tplName << "\"");
  PStringArray previousTemplate = confTpl.Lines();
  PStringStream t;
  t << "TEMPLATE " << tplName << "\n"
    << "{\n"
    << "  GLOBAL_MUTE " << (muteUnvisible?"on":"off") << "\n"
    << "  CONTROL_TYPE " << (moderated?"manual":"auto") << "\n"
    << "  VAD_VALUES " << VAdelay << ", " << VAtimeout << ", " << VAlevel << "\n";


  for(MCUVideoMixerList::shared_iterator it = videoMixerList.begin(); it != videoMixerList.end(); ++it)
  {
    MCUSimpleVideoMixer & m = *it.GetObject();
    int mixer_number = it.GetIndex();

    t << "  MIXER " << mixer_number << "\n"
      << "  {\n";
    unsigned n=m.GetPositionSet();
    VMPCfgSplitOptions & o = OpenMCU::vmcfg.vmconf[n].splitcfg;
    PString newLayout=PString(n) + ", " + o.Id;
    t << "    LAYOUT " << newLayout << "\n";
    unsigned skipCounter=0;
    for(unsigned i=0; i<o.vidnum; i++)             // video mix position will be set here:
    { PStringStream vmpText;
      int type=m.GetPositionType((int)i);
      if(type)
      {
        ConferenceMemberId id=m.GetPositionId(i);
        vmpText << "VMP " << type; // 2=VAD or 3=VAD2
        for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
        {
          ConferenceMember * member = *it;
          if(member->GetID() == id)
          {
            vmpText << ", " << member->GetName();
            break;
          }
        }
      }
      else                                         // - nothing: trying get the value from current template:
      { int previous_level=0;
        int previous_mixer=32767;
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
                mixMatch=(previous_mixer==mixer_number);
                prev_vmpN=0;
              }
              else if(cmd=="LAYOUT") { if(mixMatch) mixMatch=(value==newLayout); }
              else if(cmd=="SKIP") prev_vmpN+=value.AsInteger();
              else if(cmd=="VMP")
              {
                if(mixMatch) if(prev_vmpN == i) if(value.Mid(1,2)==", ")
                {
                  PString memberName=value.Mid(3,P_MAX_INDEX);
                  for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
                  {
                    ConferenceMember * member = *it;
                    if(member->IsSystem())
                      continue;
                    if(member->GetName() == memberName)
                    {
                      vmpText << "VMP " << value;
                      break;
                    }
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
  }
  for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
  {
    ConferenceMember *member = *it;
    if(member->IsSystem())
      continue;
    unsigned value3 = member->resizerRule << 1;
    if(member->disableVAD) value3++;
    t << "  MEMBER "
      << (member->autoDial?"1":"0") << ", "
      << member->muteMask << ", "
      << value3 << ", "
      << (member->chosenVan?"1":"0") << ", "
      << member->GetVideoMixerNumber() << ", "
      << member->GetName() << "\n";
  }
  t << "}\n\n";

  LoadTemplate(t); // temp fix

  return t;
}

void Conference::LoadTemplate(PString tpl)
{
  PTRACE(3,"Conference\tLoadtemplate");
  if(tpl.IsEmpty()) return;
  confTpl=tpl;
  PStringArray lines=tpl.Lines();
  int level=0;
  unsigned mixerId=0;
  unsigned maxMixerId=0;
  int vmpN=0;
  MCUSimpleVideoMixer * mixer = NULL;
  PStringArray validatedMembers;
  for(PINDEX i=0; i<lines.GetSize(); i++)
  {
    PString l=lines[i].Trim();
    if(l=="{") level++; else if(l=="}") level--;
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
          PStringArray v=value.Tokenise(",");
          if(v.GetSize()==3)
          {
            VAdelay  =(unsigned short int)(v[0].Trim().AsInteger());
            VAtimeout=(unsigned short int)(v[1].Trim().AsInteger());
            VAlevel  =(unsigned short int)(v[2].Trim().AsInteger());
          }
        }
        else if(cmd=="MIXER")
        {
          mixerId = value.AsInteger();
          while(true)
          {
            mixer = manager.FindVideoMixerWithLock(this, mixerId);
            if(mixer)
            {
              mixer->Unlock();
              break;
            }
            manager.AddVideoMixer(this);
          }
          if(maxMixerId<mixerId) maxMixerId=mixerId;
          vmpN=0;
        }
        else if(cmd=="LAYOUT")
        {
          if(mixer)
          {
            PStringArray v=value.Tokenise(",");
            if(v.GetSize()==2)
            { 
              PString layoutName = v[1].Trim();
              if(!layoutName.IsEmpty())
              {
                unsigned layoutNumber = v[0].Trim().AsInteger();
                PString layoutNameRef;
                if(layoutNumber < OpenMCU::vmcfg.vmconfs)
                  layoutNameRef = OpenMCU::vmcfg.vmconf[layoutNumber].splitcfg.Id;
                if((layoutName == layoutNameRef) && (layoutNumber < OpenMCU::vmcfg.vmconfs))
                  mixer->MyChangeLayout(layoutNumber);
                else
                {
                  for(unsigned i=0;i<OpenMCU::vmcfg.vmconfs;i++)
                  {
                    if(!strcmp((const char*)OpenMCU::vmcfg.vmconf[i].splitcfg.Id,(const char*)layoutName))
                    {
                      mixer->MyChangeLayout(i);
                      break;
                    }
                  }
                }
              }
            }
          }
        }
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
          else
          {
            int type=1; if(value.Left(1)=="2") type=2; else if(value.Left(1)=="3") type=3;
            PINDEX commaPosition = value.Find(',');
            if(commaPosition == P_MAX_INDEX) mixer->SetPositionType(vmpN, type);
            else
            {
              PString name=value.Mid(commaPosition+1,P_MAX_INDEX).LeftTrim();
              ConferenceMember *member = manager.FindMemberSimilarWithLock(this, name);
              if(member==NULL)
              {
                member = new MCUConnection_ConferenceMember(this, name, "");
                MCUMemberList::shared_iterator it = AddMemberToList(member);
                if(it == memberList.end()) { delete member; member = NULL; }
                else member = it.GetCapturedObject();
              }
              if(member!=NULL)
              {
                if(mixer && member->IsVisible())
                {
                  mixer->PositionSetup(vmpN, type, member);
                  member->SetFreezeVideo(FALSE);
                }
                member->Unlock();
              }
            }
          }
          vmpN++;
        }
        else if(cmd=="MEMBER")
        {
          PStringArray v=value.Tokenise(",");
          if(v.GetSize()>4) for(int i=0; i<=4;i++) v[i]=v[i].Trim();

          PString memberInternalName = v[5].Trim();
          for(int i=6; i<v.GetSize(); i++) memberInternalName += "," + v[i];

          ConferenceMember *member = manager.FindMemberSimilarWithLock(this, memberInternalName);
          if(member == NULL)
          {
            member = new MCUConnection_ConferenceMember(this, memberInternalName, "");
            MCUMemberList::shared_iterator it = AddMemberToList(member);
            if(it == memberList.end())
            {
              delete member;
              member = NULL;
            }
            else
              member = it.GetCapturedObject();
          }
          if(member)
          {
            int oldResizerRule = member->resizerRule;
            PStringArray maskAndGain = v[1].Tokenise("/");
            BOOL hasGainOptions = (maskAndGain.GetSize() > 1);
            if(hasGainOptions)
            { // stay compatible with temp. style templates:
              member->SetChannelState(maskAndGain[0].AsInteger());
//              member->kManualGainDB = maskAndGain[1].AsInteger()-20;
//              member->kOutputGainDB = maskAndGain[2].AsInteger()-20;
//              member->kManualGain=(float)pow(10.0,((float)member->kManualGainDB)/20.0);
//              member->kOutputGain=(float)pow(10.0,((float)member->kOutputGainDB)/20.0);
            }
            else 
            {
              member->muteMask      = v[1].AsInteger();
            }
            unsigned value3=(unsigned)(v[2].AsInteger());
            member->disableVAD      = value3&1;
            member->resizerRule     = (value3>>1)&127;
            member->disableVAD      = (v[2]=="1");
            member->chosenVan       = (v[3]=="1");
            OpenMCU::Current().GetEndpoint().SetMemberVideoMixer(*this, member, v[4].AsInteger());
            member->SetAutoDial((v[0]=="1"));
            if(member->resizerRule != oldResizerRule) UpdateVideoMixOptions(member);
            member->Unlock();
          }
          validatedMembers.AppendString(memberInternalName);

        } // else if(cmd=="MEMBER")

      } // if(space!=P_MAX_INDEX)

    } // if "{" else "if" } else ...

  } // for(i=0; i<lines.GetSize()

  BOOL tracingFirst=FALSE;
  int videoMixerCount = videoMixerList.GetSize();
  for(unsigned i = videoMixerCount-1; i > maxMixerId; i--) // remove reduntant mixers
  {
    if(!tracingFirst)
    {
      PTRACE(6,"Conference\tLoading template - currently active video mixers from " << (videoMixerCount-1) << " up to " << (maxMixerId+1) << " will be removed" << flush);
      tracingFirst=TRUE;
    }
    manager.DeleteVideoMixer(this, i);
  }

  if(!lockedTemplate) return; // room not locked - don't touch member list


  for(MCUMemberList::shared_iterator it = memberList.begin(); it != memberList.end(); ++it)
  {
    ConferenceMember *member = *it;
    if(member->IsSystem())
      continue;
    PString name = member->GetName();
    if(validatedMembers.GetStringsIndex(name) == P_MAX_INDEX) // remove unwanted members
    {
      ConferenceMemberId id = member->GetID();
      PTRACE(6,"Conference\tLoading template - closing connection with " << name << " (id " << id << ")" << flush);
      member->SetAutoDial(FALSE);
      member->Close();
      if(memberList.Erase(it)) delete member;
    }
  }

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

