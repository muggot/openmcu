/*
 * Nucleus Mutex.cxx
 *
 * pwlib's Mutex as implemented for Nucleus++
 *
 * Copyright (c) 1999 ISDN Communications Ltd
 *
 * Author: Chris Wayman Purvis
 *
 */

#include <ptlib.h>

#define new PNEW

PMutex::PMutex() : PSemaphore(1, 1)
  {
  }
