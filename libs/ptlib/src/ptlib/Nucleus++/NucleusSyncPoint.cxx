/*
 * Nucleus Syncpoint.cxx
 *
 * pwlib's SyncPoint as implemented for Nucleus++
 *
 * Copyright (c) 1999 ISDN Communications Ltd
 *
 * Author: Chris Wayman Purvis
 *
 */

#include <ptlib.h>

#define new PNEW


PSyncPoint::PSyncPoint() : PSemaphore(0, 1)
  {
  }
