README for Microsoft ACM G.723.1 Codec
======================================

Craig Southeren, Post Increment 
23 December 2003

Contents
========

	1. INTRODUCTION
	2. BUILDING WITH WINDOWS
	3. BUILDING WITH LINUX
	4. PERFORMANCE
	5. LICENSING


1. INTRODUCTION
===============

This directory contains the files needed to use the Microsoft ACM G.723.1 codec with 
OpenH323. For obvious reasons, this codec is only available under Windows, although
theoretically is would be possible to load the appropriate DLLs under Linux using one
of the emulation packages available.


2. BUILDING WITH WINDOWS
========================

1. Put the files is in the enclosed ZIP file into a subdirectory within the OpenH323 source 
   tree called:

            openh323/src/acmcodec

2. Create acmcodec.lib by opening the MSVC project file acmcodec.dsp and compiling. Make sure 
   the release and debug versions are created (if required).

3. Ensure that the user program include the acmcodec.h file as follows:

            #include "..\src\acmcodec\acmcodec.h"

4. Ensure that the correct G.723.1 capabilities are included, using a line like the following:

            SetCapability(0, 0, new H323_ACMG7231Capability);
   
5. Ensure that the user program is linked with the correct acmcodec.lib file. 

          openh323/src/acmcodec/debug/acmcodec.lib

              or

          openh323/src/acmcodec/release/acmcodec.lib


3. PERFORMANCE
==============

The Microsoft codec is capable of quite good performance on very modest hardware. No known problems
are outstanding - this codec just works/


4. LICENSING
============

The use of Microsoft codecs by programs other than NetMeeting has not been explicitly authorised
or prohibited by Microsoft. 

This code is supplied solely for evaluation and research purposes. The provision of this 
code by Post Increment to any entity does not constitute the supply of a license to that
entity to use, modify or distribute the code in any form, nor does it indemnify the 
entity against any legal actions that may arise from use of this code in any way.

----------------------------------------------------------------------
$Log: README.txt,v $
Revision 1.1  2007/10/22 07:40:41  shorne
*** empty log message ***

Revision 1.2  2006/08/01 13:06:47  rjongbloed
Added a raft of unvalidated audio codecs from OpenH323 tree

Revision 1.1.2.1  2006/07/22 13:42:48  rjongbloed
Added more plug ins

Revision 1.1.2.1  2006/05/08 13:49:57  rjongbloed
Imported all the audio codec plug ins from OpenH323

Revision 1.1  2004/01/06 07:04:15  craigs
Initial version

