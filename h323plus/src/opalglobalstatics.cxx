/*
 * opalglobalstatics.cxx
 *
 * Various global statics that need to be instantiated upon startup
 *
 * Portable Windows Library
 *
 * Copyright (C) 2004 Post Increment
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Portable Windows Library.
 *
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: opalglobalstatics.cxx,v $
 * Revision 1.1  2007/08/06 20:51:07  shorne
 * First commit of h323plus
 *
 * Revision 1.4.6.2  2007/04/19 12:17:05  shorne
 * removed duplicate H.235 factory definitions
 *
 * Revision 1.4.6.1  2006/12/23 19:08:03  shorne
 * Plugin video codecs & sundry
 *
 * Revision 1.4  2005/05/06 04:00:51  shorne
 * fixed --disable-plugins compile directive errors - Thanks Chih-Wei Huang
 *
 * Revision 1.3  2005/01/11 07:12:24  csoutheren
 * Fixed namespace collisions with plugin starup factories
 *
 * Revision 1.2  2005/01/11 07:02:48  csoutheren
 * Fixed namespace collisions with plugin starup factories
 *
 */

#ifndef _OPALGLOBALSTATIC_CXX
#define _OPALGLOBALSTATIC_CXX

#if defined(P_HAS_PLUGINS)
class PluginLoader : public PProcessStartup
{
  PCLASSINFO(PluginLoader, PProcessStartup);
  public:
    void OnStartup()
    { H323PluginCodecManager::Bootstrap(); }
};
#endif

namespace PWLibStupidLinkerHacks {

int h323Loader;

#ifdef P_WAVFILE
extern int opalwavfileLoader;
#endif

} // namespace PWLibStupidLinkerHacks

//////////////////////////////////

#if defined(P_HAS_PLUGINS)
static PFactory<PPluginModuleManager>::Worker<H323PluginCodecManager> h323PluginCodecManagerFactory("H323PluginCodecManager", true);
static PFactory<PProcessStartup>::Worker<PluginLoader> h323pluginStartupFactory("H323PluginLoader", true);
#endif

//////////////////////////////////

//
// declare a simple class to execute on startup
//
static class OpalInstantiateMe
{
  public:
    OpalInstantiateMe();
} initialiser;


OpalInstantiateMe::OpalInstantiateMe()
{
#ifdef P_WAVFILE
  PWLibStupidLinkerHacks::opalwavfileLoader = 1;
#endif

  PWLibStupidLinkerHacks::h235AuthLoader = 1;

#if P_SSL
  PWLibStupidLinkerHacks::h235AuthProcedure1Loader = 1;
#endif
}

#endif
