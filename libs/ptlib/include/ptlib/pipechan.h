/*
 * pipechan.h
 *
 * Sub-process with communications using pipe I/O channel class.
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
 * $Log: pipechan.h,v $
 * Revision 1.24  2006/06/21 03:28:41  csoutheren
 * Various cleanups thanks for Frederic Heem
 *
 * Revision 1.23  2005/11/25 03:43:47  csoutheren
 * Fixed function argument comments to be compatible with Doxygen
 *
 * Revision 1.22  2003/09/17 05:41:59  csoutheren
 * Removed recursive includes
 *
 * Revision 1.21  2003/09/17 01:18:02  csoutheren
 * Removed recursive include file system and removed all references
 * to deprecated coooperative threading support
 *
 * Revision 1.20  2002/09/16 01:08:59  robertj
 * Added #define so can select if #pragma interface/implementation is used on
 *   platform basis (eg MacOS) rather than compiler, thanks Robert Monaghan.
 *
 * Revision 1.19  2001/05/22 12:49:32  robertj
 * Did some seriously wierd rewrite of platform headers to eliminate the
 *   stupid GNU compiler warning about braces not matching.
 *
 * Revision 1.18  1999/03/09 02:59:50  robertj
 * Changed comments to doc++ compatible documentation.
 *
 * Revision 1.17  1999/02/16 08:11:09  robertj
 * MSVC 6.0 compatibility changes.
 *
 * Revision 1.16  1998/11/02 10:06:39  robertj
 * Added capability of pip output to go to stdout/stderr.
 *
 * Revision 1.15  1998/10/30 10:42:29  robertj
 * Better function arrangement for multi platforming.
 *
 * Revision 1.14  1998/10/29 11:29:17  robertj
 * Added ability to set environment in sub-process.
 *
 * Revision 1.13  1998/10/26 09:11:05  robertj
 * Added ability to separate out stdout from stderr on pipe channels.
 *
 * Revision 1.12  1998/09/23 06:21:08  robertj
 * Added open source copyright license.
 *
 * Revision 1.11  1997/01/03 05:25:05  robertj
 * Added Wait and Kill functions.
 *
 * Revision 1.10  1996/03/31 08:50:51  robertj
 * Changed string list to array.
 * Added function to idicate if sub-process is running.
 *
 * Revision 1.9  1995/07/31 12:15:45  robertj
 * Removed PContainer from PChannel ancestor.
 *
 * Revision 1.8  1995/06/17 11:12:53  robertj
 * Documentation update.
 *
 * Revision 1.7  1995/03/14 12:42:02  robertj
 * Updated documentation to use HTML codes.
 *
 * Revision 1.6  1995/01/09  12:39:01  robertj
 * Documentation.
 *
 * Revision 1.5  1994/10/23  04:50:55  robertj
 * Further refinement of semantics after implementation.
 *
 * Revision 1.4  1994/09/25  10:43:19  robertj
 * Added more implementation.
 *
 * Revision 1.3  1994/08/23  11:32:52  robertj
 * Oops
 *
 * Revision 1.2  1994/08/22  00:46:48  robertj
 * Added pragma fro GNU C++ compiler.
 *
 * Revision 1.1  1994/04/20  12:17:44  robertj
 * Initial revision
 *
 */

#ifndef _PPIPECHANNEL
#define _PPIPECHANNEL

#ifdef P_USE_PRAGMA
#pragma interface
#endif

#include <ptlib/channel.h>


/**A channel that uses a operating system pipe between the current process and
   a sub-process. On platforms that support {\it multi-processing}, the
   sub-program is executed concurrently with the calling process.
   
   Where full multi-processing is not supported then the sub-program is run
   with its input supplied from, or output captured to, a disk file. The
   current process is then suspended during the execution of the sub-program.
   In the latter case the semantics of the #Execute()# and #Close()#
   functions change from the usual for channels.

   Note that for platforms that do not support multi-processing, the current
   process is suspended until the sub-program terminates. The input and output
   of the sub-program is transferred via a temporary file. The exact moment of
   execution of the sub-program depends on the mode. If mode is
   #ReadOnly# then it is executed immediately and its output
   captured. In #WriteOnly# mode the sub-program is run when the
   #Close()# function is called, or when the pipe channel is destroyed.
   In #ReadWrite# mode the sub-program is run when the
   #Execute()# function is called indicating that the output from the
   current process to the sub-program has completed and input is now desired.
   
   The #CanReadAndWrite()# function effectively determines whether full
   multi-processing is supported by the platform. Note that this is different
   to whether {\it multi-threading} is supported.
 */
 
class PChannel;


class PPipeChannel : public PChannel
{
  PCLASSINFO(PPipeChannel, PChannel);

  public:
  /**@name Construction */
  //@{
    /// Channel mode for the pipe to the sub-process.
    enum OpenMode {
      /// Pipe is only from the sub-process to the current process.
      ReadOnly,   
      /// Pipe is only from the current process to the sub-process.
      WriteOnly,  
      /// Pipe is bidirectional between current and sub-processes.
      ReadWrite,  
      /**Pipe is bidirectional between current and sub-processes but the write
         side goes to stdout and stderr */
      ReadWriteStd
    };

    /**Create a new pipe channel.
     */
    PPipeChannel();
    /**Create a new pipe channel.
       This executes the subProgram and transfers data from its stdin/stdout/stderr.
       
       See the #Open()# function for details of various parameters.
     */
    PPipeChannel(
      const PString & subProgram,  ///< Sub program name or command line.
      OpenMode mode = ReadWrite,   ///< Mode for the pipe channel.
      BOOL searchPath = TRUE,      ///< Flag for system PATH to be searched.
      BOOL stderrSeparate = FALSE  ///< Standard error is on separate pipe
    );
    /**Create a new pipe channel.
       This executes the subProgram and transfers data from its stdin/stdout/stderr.
       
       See the #Open()# function for details of various parameters.
     */
    PPipeChannel(
      const PString & subProgram,  ///< Sub program name or command line.
      const PStringArray & argumentList, ///< Array of arguments to sub-program.
      OpenMode mode = ReadWrite,   ///< Mode for the pipe channel.
      BOOL searchPath = TRUE,      ///< Flag for system PATH to be searched.
      BOOL stderrSeparate = FALSE  ///< Standard error is on separate pipe
    );
    /**Create a new pipe channel.
       This executes the subProgram and transfers data from its stdin/stdout/stderr.
       
       See the #Open()# function for details of various parameters.
     */
    PPipeChannel(
      const PString & subProgram,  ///< Sub program name or command line.
      const PStringToString & environment, ///< Array of arguments to sub-program.
      OpenMode mode = ReadWrite,   ///< Mode for the pipe channel.
      BOOL searchPath = TRUE,      ///< Flag for system PATH to be searched.
      BOOL stderrSeparate = FALSE  ///< Standard error is on separate pipe
    );
    /**Create a new pipe channel.
       This executes the subProgram and transfers data from its stdin/stdout/stderr.
       
       See the #Open()# function for details of various parameters.
     */
    PPipeChannel(
      const PString & subProgram,  ///< Sub program name or command line.
      const PStringArray & argumentList, ///< Array of arguments to sub-program.
      const PStringToString & environment, ///< Array of arguments to sub-program.
      OpenMode mode = ReadWrite,   ///< Mode for the pipe channel.
      BOOL searchPath = TRUE,      ///< Flag for system PATH to be searched.
      BOOL stderrSeparate = FALSE  ///< Standard error is on separate pipe
    );

    /// Close the pipe channel, killing the sub-process.
    ~PPipeChannel();
  //@}

  /**@name Overrides from class PObject */
  //@{
    /**Determine if the two objects refer to the same pipe channel. This
       actually compares the sub-program names that are passed into the
       constructor.

       @return
       Comparison value of the sub-program strings.
     */
    Comparison Compare(
      const PObject & obj   ///< Another pipe channel to compare against.
    ) const;
  //@}


  /**@name Overrides from class PChannel */
  //@{
    /**Get the name of the channel.
    
       @return
       string for the sub-program that is run.
     */
    virtual PString GetName() const;

    /**Low level read from the channel. This function may block until the
       requested number of characters were read or the read timeout was
       reached. The GetLastReadCount() function returns the actual number
       of bytes read.

       If there are no more characters available as the sub-program has
       stopped then the number of characters available is returned. This is
       similar to end of file for the PFile channel.

       The GetErrorCode() function should be consulted after Read() returns
       FALSE to determine what caused the failure.

       @return
       TRUE indicates that at least one character was read from the channel.
       FALSE means no bytes were read due to timeout or some other I/O error.
     */
    virtual BOOL Read(
      void * buf,   ///< Pointer to a block of memory to receive the read bytes.
      PINDEX len    ///< Maximum number of bytes to read into the buffer.
    );

    /**Low level write to the channel. This function will block until the
       requested number of characters are written or the write timeout is
       reached. The GetLastWriteCount() function returns the actual number
       of bytes written.

       If the sub-program has completed its run then this function will fail
       returning FALSE.

       The GetErrorCode() function should be consulted after Write() returns
       FALSE to determine what caused the failure.

       @return
       TRUE if at least len bytes were written to the channel.
     */
    virtual BOOL Write(
      const void * buf, ///< Pointer to a block of memory to write.
      PINDEX len        ///< Number of bytes to write.
    );

    /**Close the channel. This will kill the sub-program's process (on
       platforms where that is relevent).
       
       For #WriteOnly# or #ReadWrite# mode pipe channels
       on platforms that do no support concurrent multi-processing and have
       not yet called the #Execute()# function this will run the
       sub-program.
     */
    virtual BOOL Close();
  //@}

  /**@name New member functions */
  //@{
    /** Open a channel. */
    BOOL Open(
      const PString & subProgram,  ///< Sub program name or command line.
      OpenMode mode = ReadWrite,   ///< Mode for the pipe channel.
      BOOL searchPath = TRUE,      ///< Flag for system PATH to be searched.
      BOOL stderrSeparate = FALSE  ///< Standard error is on separate pipe
    );
    /** Open a channel. */
    BOOL Open(
      const PString & subProgram,  ///< Sub program name or command line.
      const PStringArray & argumentList, ///< Array of arguments to sub-program.
      OpenMode mode = ReadWrite,   ///< Mode for the pipe channel.
      BOOL searchPath = TRUE,      ///< Flag for system PATH to be searched.
      BOOL stderrSeparate = FALSE  ///< Standard error is on separate pipe
    );
    /** Open a channel. */
    BOOL Open(
      const PString & subProgram,  ///< Sub program name or command line.
      const PStringToString & environment, ///< Array of arguments to sub-program.
      OpenMode mode = ReadWrite,   ///< Mode for the pipe channel.
      BOOL searchPath = TRUE,      ///< Flag for system PATH to be searched.
      BOOL stderrSeparate = FALSE  ///< Standard error is on separate pipe
    );
    /**Open a new pipe channel allowing the subProgram to be executed and
       data transferred from its stdin/stdout/stderr.
       
       If the mode is #ReadOnly# then the #stdout# of the
       sub-program is supplied via the #Read()# calls of the PPipeChannel.
       The sub-programs input is set to the platforms null device (eg
       /dev/nul).

       If mode is #WriteOnly# then #Write()# calls of the
       PPipeChannel are suppied to the sub-programs #stdin# and its
       #stdout# is sent to the null device.
       
       If mode is #ReadWrite# then both read and write actions can
       occur.

       The #subProgram# parameter may contain just the path of the
       program to be run or a program name and space separated arguments,
       similar to that provided to the platforms command processing shell.
       Which use of this parameter is determiend by whether arguments are
       passed via the #argumentPointers# or
       #argumentList# parameters.

       The #searchPath# parameter indicates that the system PATH
       for executables should be searched for the sub-program. If FALSE then
       only the explicit or implicit path contained in the
       #subProgram# parameter is searched for the executable.

       The #stderrSeparate# parameter indicates that the standard
       error stream is not included in line with the standard output stream.
       In this case, data in this stream must be read using the
       #ReadStandardError()# function.

       The #environment# parameter is a null terminated sequence
       of null terminated strings of the form name=value. If NULL is passed
       then the same invironment as calling process uses is passed to the
       child process.
     */
    BOOL Open(
      const PString & subProgram,  ///< Sub program name or command line.
      const PStringArray & argumentList, ///< Array of arguments to sub-program.
      const PStringToString & environment, ///< Array of arguments to sub-program.
      OpenMode mode = ReadWrite,   ///< Mode for the pipe channel.
      BOOL searchPath = TRUE,      ///< Flag for system PATH to be searched.
      BOOL stderrSeparate = FALSE  ///< Standard error is on separate pipe
    );

    /**Get the full file path for the sub-programs executable file.

       @return
       file path name for sub-program.
     */
    const PFilePath & GetSubProgram() const;

    /**Start execution of sub-program for platforms that do not support
       multi-processing, this will actually run the sub-program passing all
       data written to the PPipeChannel.
       
       For platforms that do support concurrent multi-processing this will
       close the pipe from the current process to the sub-process.
      
       As the sub-program is run immediately and concurrently, this will just
       give an end of file to the stdin of the remote process. This is often
       necessary.

       @return TRUE if execute was successful.
     */
    BOOL Execute();

    /**Determine if the program associated with the PPipeChannel is still
       executing. This is useful for determining the status of PPipeChannels
       which take a long time to execute on operating systems which support
       concurrent multi-processing.
       
       @return
       TRUE if the program associated with the PPipeChannel is still running
     */
    BOOL IsRunning() const;

    /**Get the return code from the most recent Close;

       @return
       Return code from the closing process
     */
    int GetReturnCode() const;

    /**This function will block and wait for the sub-program to terminate.
    
       @return
       Return code from the closing process
     */
    int WaitForTermination();
    
    /**This function will block and wait for the sub-program to terminate.
       It will wait only for the specified amount of time.
    
       @return
       Return code from the closing process, -1 if timed out.
     */
    int WaitForTermination(
      const PTimeInterval & timeout  ///< Amount of time to wait for process.
    );

    /**This function will terminate the sub-program using the signal code
       specified.
     
       @return
       TRUE if the process received the signal. Note that this does not mean
       that the process has actually terminated.
     */
    BOOL Kill(
      int signal = 9  ///< Signal code to be sent to process.
    );

    /**Read all available data on the standard error stream of the
       sub-process. If the #wait# parameter is FALSE then only
       the text currently available is returned. If TRUE then the function
       blocks as long as necessary to get some number of bytes.

       @return
       TRUE indicates that at least one character was read from stderr.
       FALSE means no bytes were read due to timeout or some other I/O error.
     */
    BOOL ReadStandardError(
      PString & errors,   ///< String to receive standard error text.
      BOOL wait = FALSE   ///< Flag to indicate if function should block
    );

    /**Determine if the platform can support simultaneous read and writes from
       the PPipeChannel (eg MSDOS returns FALSE, Unix returns TRUE).
       
       @return
       TRUE if platform supports concurrent multi-processing.
     */
    static BOOL CanReadAndWrite();
  //@}


  protected:
    // Member variables
    /// The fully qualified path name for the sub-program executable.
    PFilePath subProgName;


  private:
    BOOL PlatformOpen(const PString & subProgram,
                      const PStringArray & arguments,
                      OpenMode mode,
                      BOOL searchPath,
                      BOOL stderrSeparate,
                      const PStringToString * environment);


// Include platform dependent part of class
#ifdef _WIN32
#include "msos/ptlib/pipechan.h"
#else
#include "unix/ptlib/pipechan.h"
#endif
};

#endif

// End Of File ///////////////////////////////////////////////////////////////
