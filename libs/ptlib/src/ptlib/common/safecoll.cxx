/*
 * safecoll.cxx
 *
 * Thread safe collection classes.
 *
 * Portable Windows Library
 *
 * Copyright (c) 2002 Equivalence Pty. Ltd.
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
 * Contributor(s): ______________________________________.
 *
 * $Log: safecoll.cxx,v $
 * Revision 1.18  2007/06/09 05:43:55  rjongbloed
 * Added ability for PSafePtr to be used as an garbage collecting pointer when not
 *   contained within a collection. Last PSafePtr reference to go out of scope deletes
 *   the object, provided it has never been in a PSafeCollection.
 *
 * Revision 1.17  2007/05/09 12:38:44  csoutheren
 * Applied 1705751 - Safer and better PSafeCollection logs (2nd patch)
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.16  2007/04/20 07:35:55  csoutheren
 * Applied 1703664 - Safer and better PSafeCollection logs
 * Thanks to Fabrizio Ammollo
 *
 * Revision 1.15  2007/04/20 02:31:14  rjongbloed
 * Added ability to share a single mutex amongst multiple PSafeObjects,
 *   this can help with certain deadlock scenarios.
 *
 * Revision 1.14  2004/10/14 23:01:31  csoutheren
 * Fiuxed problem with usage of Sleep
 *
 * Revision 1.13  2004/10/14 12:31:47  rjongbloed
 * Added synchronous mode for safe collection RemoveAll() to wait until all objects
 *   have actually been deleted before returning.
 *
 * Revision 1.12  2004/10/04 12:54:33  rjongbloed
 * Added functions for locking an unlocking to "auto-unlock" classes.
 *
 * Revision 1.11  2004/08/14 07:42:31  rjongbloed
 * Added trace log at level 6 for helping find PSafeObject reference/dereference errors.
 *
 * Revision 1.10  2004/08/12 12:37:41  rjongbloed
 * Fixed bug recently introduced so removes deleted object from deletion list.
 * Also changed removal list to be correct type.
 *
 * Revision 1.9  2004/08/05 12:15:56  rjongbloed
 * Added classes for auto unlocking read only and read write mutex on
 *   PSafeObject - similar to PWaitAndSIgnal.
 * Utilised mutable keyword for mutex and improved the constness of functions.
 * Added DisallowDeleteObjects to safe collections so can have a PSafeObject in
 *   multiple collections.
 * Added a tempalte function to do casting of PSafePtr to a PSafePtr of a derived
 *   class.
 * Assured that a PSafeObject present on a collection always increments its
 *   reference count so while in collection it is not deleted.
 *
 * Revision 1.8  2004/04/03 08:22:22  csoutheren
 * Remove pseudo-RTTI and replaced with real RTTI
 *
 * Revision 1.7  2002/12/10 07:37:34  robertj
 * optimised SetLockMode() so if doesn't change mode it doesn't do anything.
 *
 * Revision 1.6  2002/10/29 00:06:24  robertj
 * Changed template classes so things like PSafeList actually creates the
 *   base collection class as well.
 * Allowed for the PSafeList::Append() to return a locked pointer to the
 *   object just appended.
 *
 * Revision 1.5  2002/10/04 08:22:50  robertj
 * Changed read/write mutex so can be called by same thread without deadlock
 *   removing the need to a lock count in safe pointer.
 * Added asserts if try and dereference a NULL safe pointer.
 * Added more documentation on behaviour.
 *
 * Revision 1.4  2002/08/29 06:53:28  robertj
 * Added optimisiation, separate mutex for toBeRemoved list.
 * Added assert for reference count going below zero.
 * Fixed incorrect usage of lockCount if target of an assignment from another
 *   safe pointer. Would not unlock the safe object which could cause deadlock.
 *
 * Revision 1.3  2002/05/06 00:44:45  robertj
 * Made the lock/unlock read only const so can be used in const functions.
 *
 * Revision 1.2  2002/05/01 04:48:05  robertj
 * GNU compatibility.
 *
 * Revision 1.1  2002/05/01 04:16:44  robertj
 * Added thread safe collection classes.
 *
 */

#ifdef __GNUC__
#pragma implementation "safecoll.h"
#endif

#include <ptlib.h>
#include <ptlib/safecoll.h>


#define new PNEW


/////////////////////////////////////////////////////////////////////////////

PSafeObject::PSafeObject(PSafeObject * indirectLock)
    : safeReferenceCount(0)
    , safelyBeingRemoved(FALSE)
    , safeInUse(indirectLock != NULL ? indirectLock->safeInUse : &safeInUseMutex)
{
}


BOOL PSafeObject::SafeReference()
{
  PStringStream str;
  {
    PWaitAndSignal mutex(safetyMutex);
    if (safelyBeingRemoved)
      return FALSE;
    safeReferenceCount++;
    str << "SafeColl\tIncrement reference count to " << safeReferenceCount << " for " << GetClass() << ' ' << (void *)this;
  }
  PTRACE(6, str);
  return TRUE;
}


BOOL PSafeObject::SafeDereference()
{
  BOOL mayBeDeleted = FALSE;

  PStringStream str;
  safetyMutex.Wait();
  if (PAssert(safeReferenceCount > 0, PLogicError)) {
    safeReferenceCount--;
    mayBeDeleted = safeReferenceCount == 0 && !safelyBeingRemoved;
    str << "SafeColl\tDecrement reference count to " << safeReferenceCount << " for " << GetClass() << ' ' << (void *)this;
  }
  safetyMutex.Signal();
  PTRACE(6, str);

  return mayBeDeleted;
}


BOOL PSafeObject::LockReadOnly() const
{
  PTRACE(6, "SafeColl\tWaiting read ("<<(void *)this<<")");
  safetyMutex.Wait();

  if (safelyBeingRemoved) {
    safetyMutex.Signal();
    PTRACE(6, "SafeColl\tBeing removed while waiting read ("<<(void *)this<<")");
    return FALSE;
  }

  safetyMutex.Signal();
  safeInUse->StartRead();
  PTRACE(6, "SafeColl\tLocked read ("<<(void *)this<<")");
  return TRUE;
}


void PSafeObject::UnlockReadOnly() const
{
  PTRACE(6, "SafeColl\tUnlocked read ("<<(void *)this<<")");
  safeInUse->EndRead();
}


BOOL PSafeObject::LockReadWrite()
{
  PTRACE(6, "SafeColl\tWaiting readWrite ("<<(void *)this<<")");
  safetyMutex.Wait();

  if (safelyBeingRemoved) {
    safetyMutex.Signal();
    PTRACE(6, "SafeColl\tBeing removed while waiting readWrite ("<<(void *)this<<")");
    return FALSE;
  }

  safetyMutex.Signal();
  safeInUse->StartWrite();
  PTRACE(6, "SafeColl\tLocked readWrite ("<<(void *)this<<")");
  return TRUE;
}


void PSafeObject::UnlockReadWrite()
{
  PTRACE(6, "SafeColl\tUnlocked readWrite ("<<(void *)this<<")");
  safeInUse->EndWrite();
}


void PSafeObject::SafeRemove()
{
  safetyMutex.Wait();
  safelyBeingRemoved = TRUE;
  safetyMutex.Signal();
}


BOOL PSafeObject::SafelyCanBeDeleted() const
{
  PWaitAndSignal mutex(safetyMutex);
  return safelyBeingRemoved && safeReferenceCount == 0;
}


/////////////////////////////////////////////////////////////////////////////

PSafeLockReadOnly::PSafeLockReadOnly(const PSafeObject & object)
  : safeObject((PSafeObject &)object)
{
  locked = safeObject.LockReadOnly();
}


PSafeLockReadOnly::~PSafeLockReadOnly()
{
  if (locked)
    safeObject.UnlockReadOnly();
}


BOOL PSafeLockReadOnly::Lock()
{
  locked = safeObject.LockReadOnly();
  return locked;
}


void PSafeLockReadOnly::Unlock()
{
  if (locked) {
    safeObject.UnlockReadOnly();
    locked = FALSE;
  }
}



/////////////////////////////////////////////////////////////////////////////

PSafeLockReadWrite::PSafeLockReadWrite(const PSafeObject & object)
  : safeObject((PSafeObject &)object)
{
  locked = safeObject.LockReadWrite();
}


PSafeLockReadWrite::~PSafeLockReadWrite()
{
  if (locked)
    safeObject.UnlockReadWrite();
}


BOOL PSafeLockReadWrite::Lock()
{
  locked = safeObject.LockReadWrite();
  return locked;
}


void PSafeLockReadWrite::Unlock()
{
  if (locked) {
    safeObject.UnlockReadWrite();
    locked = FALSE;
  }
}


/////////////////////////////////////////////////////////////////////////////

PSafeCollection::PSafeCollection(PCollection * coll)
{
  collection = coll;
  collection->DisallowDeleteObjects();
  toBeRemoved.DisallowDeleteObjects();
  deleteObjects = TRUE;
}


PSafeCollection::~PSafeCollection()
{
  deleteObjectsTimer.Stop();

  toBeRemoved.AllowDeleteObjects();
  toBeRemoved.RemoveAll();

  collection->AllowDeleteObjects();
  delete collection;
}


BOOL PSafeCollection::SafeRemove(PSafeObject * obj)
{
  if (obj == NULL)
    return FALSE;

  PWaitAndSignal mutex(collectionMutex);
  if (!collection->Remove(obj))
    return FALSE;

  SafeRemoveObject(obj);
  return TRUE;
}


BOOL PSafeCollection::SafeRemoveAt(PINDEX idx)
{
  PWaitAndSignal mutex(collectionMutex);
  PSafeObject * obj = PDownCast(PSafeObject, collection->RemoveAt(idx));
  if (obj == NULL)
    return FALSE;

  SafeRemoveObject(obj);
  return TRUE;
}


void PSafeCollection::RemoveAll(BOOL synchronous)
{
  collectionMutex.Wait();

  while (collection->GetSize() > 0)
    SafeRemoveObject(PDownCast(PSafeObject, collection->RemoveAt(0)));

  collectionMutex.Signal();

  if (synchronous) {
    // Have unfortunate busy loop here, but it should be very
    // rare that it will be here for long
    while (!DeleteObjectsToBeRemoved())
      PThread::Sleep(100);
  }
}


void PSafeCollection::SafeRemoveObject(PSafeObject * obj)
{
  if (obj == NULL)
    return;

  // Make sure SfeRemove() called before SafeDereference() to avoid race condition
  if (deleteObjects) {
    obj->SafeRemove();

    removalMutex.Wait();
    toBeRemoved.Append(obj);
    removalMutex.Signal();
  }

  obj->SafeDereference();
}


BOOL PSafeCollection::DeleteObjectsToBeRemoved()
{
  PWaitAndSignal lock(removalMutex);

  PINDEX i = 0;
  while (i < toBeRemoved.GetSize()) {
    if (toBeRemoved[i].SafelyCanBeDeleted()) {
      PObject * obj = toBeRemoved.RemoveAt(i);
      removalMutex.Signal();
      DeleteObject(obj);
      removalMutex.Wait();

      i = 0; // Restart looking through list
    }
    else
      i++;
  }

  return toBeRemoved.IsEmpty() && collection->IsEmpty();
}


void PSafeCollection::DeleteObject(PObject * object) const
{
  delete object;
}


void PSafeCollection::SetAutoDeleteObjects()
{
  if (deleteObjectsTimer.IsRunning())
    return;

  deleteObjectsTimer.SetNotifier(PCREATE_NOTIFIER(DeleteObjectsTimeout));
  deleteObjectsTimer.RunContinuous(1000); // EVery second
}


void PSafeCollection::DeleteObjectsTimeout(PTimer &, INT)
{
  DeleteObjectsToBeRemoved();
}


PINDEX PSafeCollection::GetSize() const
{
  PWaitAndSignal lock(collectionMutex);
  return collection->GetSize();
}


/////////////////////////////////////////////////////////////////////////////

PSafePtrBase::PSafePtrBase(PSafeObject * obj, PSafetyMode mode)
{
  collection = NULL;
  currentObject = obj;
  lockMode = mode;

  EnterSafetyMode(WithReference);
}


PSafePtrBase::PSafePtrBase(const PSafeCollection & safeCollection,
                           PSafetyMode mode,
                           PINDEX idx)
{
  collection = &safeCollection;
  currentObject = NULL;
  lockMode = mode;

  Assign(idx);
}


PSafePtrBase::PSafePtrBase(const PSafeCollection & safeCollection,
                           PSafetyMode mode,
                           PSafeObject * obj)
{
  collection = &safeCollection;
  currentObject = NULL;
  lockMode = mode;

  Assign(obj);
}


PSafePtrBase::PSafePtrBase(const PSafePtrBase & enumerator)
{
  collection = enumerator.collection;
  currentObject = enumerator.currentObject;
  lockMode = enumerator.lockMode;

  EnterSafetyMode(WithReference);
}


PSafePtrBase::~PSafePtrBase()
{
  ExitSafetyMode(WithDereference);
}


PObject::Comparison PSafePtrBase::Compare(const PObject & obj) const
{
  const PSafePtrBase * other = PDownCast(const PSafePtrBase, &obj);
  if (other == NULL)
    return GreaterThan;

  if (currentObject < other->currentObject)
    return LessThan;
  if (currentObject > other->currentObject)
    return GreaterThan;
  return EqualTo;
}


void PSafePtrBase::Assign(const PSafePtrBase & enumerator)
{
  if (this == &enumerator)
    return;

  // lockCount ends up zero after this
  ExitSafetyMode(WithDereference);

  collection = enumerator.collection;
  currentObject = enumerator.currentObject;
  lockMode = enumerator.lockMode;

  EnterSafetyMode(WithReference);
}


void PSafePtrBase::Assign(const PSafeCollection & safeCollection)
{
  // lockCount ends up zero after this
  ExitSafetyMode(WithDereference);

  collection = &safeCollection;
  lockMode = PSafeReadWrite;

  Assign((PINDEX)0);
}


void PSafePtrBase::Assign(PSafeObject * newObj)
{
  ExitSafetyMode(WithDereference);

  currentObject = newObj;

  if (newObj == NULL)
    return;

  if (collection == NULL) {
    lockMode = PSafeReference;
    if (!EnterSafetyMode(WithReference))
      currentObject = NULL;
    return;
  }

  collection->collectionMutex.Wait();

  if (collection->collection->GetObjectsIndex(newObj) == P_MAX_INDEX) {
    collection->collectionMutex.Signal();
    collection = NULL;
    lockMode = PSafeReference;
    if (!EnterSafetyMode(WithReference))
      currentObject = NULL;
  }
  else {
    if (!newObj->SafeReference())
      currentObject = NULL;
    collection->collectionMutex.Signal();
    EnterSafetyMode(AlreadyReferenced);
  }
}


void PSafePtrBase::Assign(PINDEX idx)
{
  ExitSafetyMode(WithDereference);

  currentObject = NULL;

  if (collection == NULL)
    return;

  collection->collectionMutex.Wait();

  while (idx < collection->collection->GetSize()) {
    currentObject = (PSafeObject *)collection->collection->GetAt(idx);
    if (currentObject != NULL) {
      if (currentObject->SafeReference())
        break;
      currentObject = NULL;
    }
    idx++;
  }

  collection->collectionMutex.Signal();

  EnterSafetyMode(AlreadyReferenced);
}


void PSafePtrBase::Next()
{
  if (collection == NULL || currentObject == NULL)
    return;

  ExitSafetyMode(NoDereference);

  collection->collectionMutex.Wait();

  PINDEX idx = collection->collection->GetObjectsIndex(currentObject);

  currentObject->SafeDereference();
  currentObject = NULL;

  if (idx != P_MAX_INDEX) {
    while (++idx < collection->collection->GetSize()) {
      currentObject = (PSafeObject *)collection->collection->GetAt(idx);
      if (currentObject != NULL) {
        if (currentObject->SafeReference())
          break;
        currentObject = NULL;
      }
    }
  }

  collection->collectionMutex.Signal();

  EnterSafetyMode(AlreadyReferenced);
}


void PSafePtrBase::Previous()
{
  if (collection == NULL || currentObject == NULL)
    return;

  ExitSafetyMode(NoDereference);

  collection->collectionMutex.Wait();

  PINDEX idx = collection->collection->GetObjectsIndex(currentObject);

  currentObject->SafeDereference();
  currentObject = NULL;

  if (idx != P_MAX_INDEX) {
    while (idx-- > 0) {
      currentObject = (PSafeObject *)collection->collection->GetAt(idx);
      if (currentObject != NULL) {
        if (currentObject->SafeReference())
          break;
        currentObject = NULL;
      }
    }
  }

  collection->collectionMutex.Signal();

  EnterSafetyMode(AlreadyReferenced);
}


BOOL PSafePtrBase::SetSafetyMode(PSafetyMode mode)
{
  if (lockMode == mode)
    return TRUE;

  ExitSafetyMode(NoDereference);
  lockMode = mode;
  return EnterSafetyMode(AlreadyReferenced);
}


BOOL PSafePtrBase::EnterSafetyMode(EnterSafetyModeOption ref)
{
  if (currentObject == NULL)
    return FALSE;

  if (ref == WithReference && !currentObject->SafeReference()) {
    currentObject = NULL;
    return FALSE;
  }

  switch (lockMode) {
    case PSafeReadOnly :
      if (currentObject->LockReadOnly())
        return TRUE;
      break;

    case PSafeReadWrite :
      if (currentObject->LockReadWrite())
        return TRUE;
      break;

    case PSafeReference :
      return TRUE;
  }

  currentObject->SafeDereference();
  currentObject = NULL;
  return FALSE;
}


void PSafePtrBase::ExitSafetyMode(ExitSafetyModeOption ref)
{
  if (currentObject == NULL)
    return;

  switch (lockMode) {
    case PSafeReadOnly :
      currentObject->UnlockReadOnly();
      break;

    case PSafeReadWrite :
      currentObject->UnlockReadWrite();
      break;

    case PSafeReference :
      break;
  }

  if (ref == WithDereference && currentObject->SafeDereference()) {
    PTRACE(6, "SafeColl\tDeleting object ("<<(void *)currentObject<<")");
    delete currentObject;
    currentObject = NULL;
  }
}


// End of File ///////////////////////////////////////////////////////////////
