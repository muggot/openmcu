/*
 * svcproc.h
 *
 * Service process (daemon) class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
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
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Portions are Copyright (C) 1993 Free Software Foundation, Inc.
 * All Rights Reserved.
 *
 * Contributor(s): ______________________________________.
 *
 * $Log: svcproc.h,v $
 * Revision 1.16  2003/09/17 01:18:03  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.15  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.14  2002/04/17 03:31:48  robertj
 * Renamed system log file member variable to be common with Windows version.
 *
 * Revision 1.13  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.12  2001/03/20 06:44:25  robertj
 * Lots of changes to fix the problems with terminating threads that are I/O
 *   blocked, especially when doing orderly shutdown of service via SIGTERM.
 *
 * Revision 1.11  2001/03/20 01:02:32  robertj
 * Fixed some difficulties with terminating a service process from signals or
 *   from simply dropping out of Main().
 *
 * Revision 1.10  2001/03/13 03:47:18  robertj
 * Added ability to set pid file from command line.
 *
 * Revision 1.9  1998/11/30 22:07:15  robertj
 * New directory structure.
 *
 * Revision 1.8  1998/09/24 04:11:58  robertj
 * Added open software license.
 *
 * Revision 1.7  1998/05/30 13:30:11  robertj
 * Added ability to specify the log file as well as just console output.
 *
 * Revision 1.6  1996/09/21 05:42:12  craigs
 * Changes for new common files, PConfig changes and signal handling
 *
 * Revision 1.5  1996/09/03 11:56:56  craigs
 * Changed PSYSTEMLOG to user cerr
 *
 * Revision 1.4  1996/08/03 12:10:46  craigs
 * Changed for new common directories and added new PSystemLog macro
 *
 * Revision 1.3  1996/06/19 12:33:45  craigs
 * Added ^C handling
 *
 * Revision 1.2  1996/04/15 10:50:48  craigs
 * Last revision prior to release of MibMaster
 *
 * Revision 1.1  1996/01/26 11:06:31  craigs
 * Initial revision
 *
 */

  public:
    ~PServiceProcess();
    virtual void Terminate();
  protected:
    int  InitialiseService();
    void _PXShowSystemWarning(PINDEX num, const PString & str);
    void PXOnSignal(int);
    void PXOnAsyncSignal(int);
    PString systemLogFileName;
    PString pidFileToRemove;
    BOOL isTerminating;

// End Of File ////////////////////////////////////////////////////////////////
