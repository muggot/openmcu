/*
 * tcpsock.h
 *
 * TCP socket I/O channel class.
 *
 * Portable Windows Library
 *
 * Copyright (c) 1993-1998 Equivalence Pty. Ltd.
 *
 * Copyright (c) 1999 ISDN Communications Ltd
 *
 */

#ifndef _PTCPSOCKET

#ifdef __GNUC__
#pragma interface
#endif

///////////////////////////////////////////////////////////////////////////////
// PTCPSocket

#include "../../tcpsock.h"

public:
  virtual BOOL Read(void * buf, PINDEX len);
};

#endif
