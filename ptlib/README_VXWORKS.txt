Cummulative VxWorks patch for PWLIB
===================================
Patch is done against CVS-snapshot dated: 9 juli 2004

This patch includes VxWorks specific updates (platform and compiler), 
including a minor general update.


Disclaimer
==========
We have not yet thoroughly tested this patch-set against the latest 
PWLIB. However all patches are already use in our 30-channel IP-gateway 
product, which is using Openh323 based on version 1.10.4 (PWLib:1.4.4)
with some general community bugfixes added and is running very stable.
Our VxWorks environment is Tornado 2.1.1 for MIPS platform building the
MIPS images on Windows platform.


Patch history
=============
- Added Mozilla Public License header and revision history in tlibvx.cxx 
  (this was missing...)
- Implemention of critical section (for semaphores)
- Rewritten semaphore handling (was incredible buggy)
- Fixed synchronisation bug during launching of new task in PThread
  (related to priority handling of VxWorks task scheduler)
- Fixed administration handling with PThreads (especially when PThread
  was ending/terminating). Did show up under heavier load.
- Implemention of tracing facility in PThread (::Trace)
- Less use of asserts (for embedded environment)
- Increased stacksize for House-Keeping Thread (+ giving that beast a name)
- Included here and there lot of comments regarding specific behaviour
  of VxWorks and/or Tornado-IDE version.
- Update in socket handling:
  - Now supporting Non-Blocking sockets  
  - Fixed memory-leak in inet_ntoa(). 
    VxWorks does do malloc but no free!! Now using inet_ntoa_b.
  - os_select now more directly with same behaviour
- Some P_TORNADO defines renamed to P_VXWORKS.
  The P_TORNADO compiler define now applies to some specific TORNADO versions. 
  When using  version 2.1.0 or earlier. This flag now only covers the compiler 
  bug and missing iostream. Suggested is to try to build VxWorks first without 
  P_TORNADO flag. In case of errors (compiler crashes or error in iostreams), 
  enable this flag.
- Resolved conflict due to VxWorks headerfile mbuf.h
  Above mentioned headerfile includes a "#define m_data <to_something_else>" 
  (also for m_type) ?!?!. This resulted into the requirement to manually edit 
  some generated H.323 files (ASN.1) by placing #undef's after the includes 
  because the m_data in these files were overwritten by that #define. By 
  locating a central place to put "#undef m_data" and m_type this is history 
  now. Another step closer to full automatic generation of openh323.
- Adapted PMEMORY_CHECK for VxWorks variant
- Routed cfree to PMEMORY_CHECK (as counterpart of calloc)
- Moved other build variants from os_select to PSocket::Select
- Simplified remaining PXBlockOnIO
- Added copy constructors of PSemaphore, PMutex and PSyncPoint
- Removed unnecessary assignment in PMutex::WillBlock
- Removed unused PXBlockOnIO definition (not VxWorks specific)


Some comments for seting up PWLib for VxWorks
=============================================
- Get PWLib, and if If applicable: apply all patches
- Current way of working with ./configure.exe and ./make is not supported, so 
  you need to make/adapt your own makefile.
- WindRiver has a special adapted GCC compiler for VxWorks, which possible 
  requires to include -fpermissive in your CFLAG to make everything compiling. 
- Add next VxWorks specific defines:
	-DP_VXWORKS
        -DVX_TASKS
	-DP_TORNADO  (only if your compiler *crashes*)  
 	-DNO_LONG_DOUBLE
	-DP_USE_PRAGMA
	-DPHAS_TEMPLATES  
- Big change you need to ask WindRiver support for a compiler patch to solve
  the "Virtual memory Exhaust" problem during compile/link...
  This has actually nothing to do with your internal PC memory, but all with
  the applied stacksizes internally in the compiler.
- In CVS a module exist called pwlib_tornado. Here you can find the Tornado SDE
  workspace files for ARM environment (like MS Developer Studio workspace files). 
  Sadly.. these are corrupt because they are saved/stored including CR+LF while 
  it should only contain <CR> (or LF?) => UNIX style. We're not using this but
  just want to mention it here.
- Surely I forgot something... just drop me a mail.



Happy VxWorks'ing with these patches.
If you have any questions or comments, just drop me a mail (see below)


Eize Slange

--
Department: Development - Platform Enhancement
Philips Business Communications, The Netherlands (NL)
Homepage: http://www.sopho.philips.com
Tel: +31 (0) 35 6893604
Fax: +31 (0) 35 6891290
e-Mail: Eize.Slange@philips.com
