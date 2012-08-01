README for Voice Age G.729 Codec
================================

Robert Jongbloed, Post Increment 

Contents
========

	1. INTRODUCTION
	2. BUILDING WITH WINDOWS
	3. LICENSING


1. INTRODUCTION
===============

This directory contains the files needed to use the Voice Age G.729 codec with 
OPAL. This codec is only available under Windows, although theoretically is would
 be possible to load the appropriate DLLs under Linux using one of the emulation
  packages available.


2. BUILDING WITH WINDOWS
========================

1. Obtain the actual codec libraries from http://www.voiceage.com/freecodecs.php

2. Unpack into the "va_g729" directory within this directory

3. Ensure that this directory contains at least the followint:

            va_g729a.h
            va_g729a.lib

4. Add VoiceAgeG729.vcproj to your solution file and compile.


3. LICENSING
============

You must obtain and license the Voice Age codec from that company. 

This code is supplied solely for evaluation and research purposes. The provision of this 
code by Post Increment to any entity does not constitute the supply of a license to that
entity to use, modify or distribute the code in any form, nor does it indemnify the 
entity against any legal actions that may arise from use of this code in any way.

----------------------------------------------------------------------
$Log: README.txt,v $
Revision 1.1  2007/10/22 07:41:07  shorne
*** empty log message ***

Revision 1.3  2006/10/26 08:39:25  rjongbloed
Updated to latest Voice Age download library

Revision 1.2  2006/08/01 13:06:51  rjongbloed
Added a raft of unvalidated audio codecs from OpenH323 tree

Revision 1.1.2.1  2006/07/22 13:52:15  rjongbloed
Added more plug ins

