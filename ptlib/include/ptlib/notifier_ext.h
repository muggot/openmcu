/*
 * notifier_ext.h
 *
 * Smart Notifiers and Notifier Lists
 *
 * Portable Windows Library
 *
 * Copyright (c) 2004 Reitek S.p.A.
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
 * $Log: notifier_ext.h,v $
 * Revision 1.6  2007/04/02 05:29:54  rjongbloed
 * Tidied some trace logs to assure all have a category (bit before a tab character) set.
 *
 * Revision 1.5  2005/11/30 12:47:37  csoutheren
 * Removed tabs, reformatted some code, and changed tags for Doxygen
 *
 * Revision 1.4  2004/05/17 11:02:39  csoutheren
 * Added extra documentation
 *
 * Revision 1.3  2004/05/09 07:23:48  rjongbloed
 * More work on XMPP, thanks Federico Pinna and Reitek S.p.A.
 *
 * Revision 1.2  2004/04/26 01:34:58  rjongbloed
 * Change nofier list to be able to used in containers, thanks Federico Pinna, Reitek S.p.A.
 *
 * Revision 1.1  2004/04/22 12:31:00  rjongbloed
 * Added PNotifier extensions and XMPP (Jabber) support,
 *   thanks to Federico Pinna and Reitek S.p.A.
 *
 *
 */

#ifndef _PNOTIFIER_EXT
#define _PNOTIFIER_EXT

#ifdef P_USE_PRAGMA
#pragma interface
#endif

/** Implements a function similar to the PNotifier, but uses an "id" to link the caller
  * and callee rather than using a pointer. This has the advantage that if the pointer
  * becomes invalid, the caller can gracefully fail the notification rather than
  * simply crashing due to an invalid pointer access.
  *
  * These classes were created to support of the XMPP classes
  */

class PSmartNotifieeRegistrar
{
  public:
    PSmartNotifieeRegistrar() : m_ID(P_MAX_INDEX) {}
    ~PSmartNotifieeRegistrar() { UnregisterNotifiee(m_ID); }

    void        Init(void * obj)        { if (m_ID == P_MAX_INDEX) m_ID = RegisterNotifiee(obj); }
    unsigned    GetID() const           { return m_ID; }

    static unsigned    RegisterNotifiee(void * obj);
    static BOOL        UnregisterNotifiee(unsigned id);
    static BOOL        UnregisterNotifiee(void * obj);
    static void *      GetNotifiee(unsigned id);

  protected:
    unsigned m_ID;
};

class PSmartNotifierFunction : public PNotifierFunction
{
    PCLASSINFO(PSmartNotifierFunction, PNotifierFunction);

  protected:
    unsigned m_NotifieeID;

  public:
    PSmartNotifierFunction(unsigned id) : PNotifierFunction(&id), m_NotifieeID(id) { }
    unsigned GetNotifieeID() const { return m_NotifieeID; }
    void * GetNotifiee() const { return PSmartNotifieeRegistrar::GetNotifiee(m_NotifieeID); }
    BOOL IsValid() const { return GetNotifiee() != 0; }
};

#define PDECLARE_SMART_NOTIFIEE \
    PSmartNotifieeRegistrar   m_Registrar; \

#define PCREATE_SMART_NOTIFIEE m_Registrar.Init(this)

#define PDECLARE_SMART_NOTIFIER(notifier, notifiee, func) \
  class func##_PSmartNotifier : public PSmartNotifierFunction { \
    public: \
      func##_PSmartNotifier(unsigned id) : PSmartNotifierFunction(id) { } \
      virtual void Call(PObject & note, INT extra) const \
      { \
          void * obj = GetNotifiee(); \
          if (obj) \
            ((notifiee*)obj)->func((notifier &)note, extra); \
          else \
            PTRACE(2, "PWLib\tInvalid notifiee"); \
      } \
  }; \
  friend class func##_PSmartNotifier; \
  virtual void func(notifier & note, INT extra)

#define PCREATE_SMART_NOTIFIER(func) PNotifier(new func##_PSmartNotifier(m_Registrar.GetID()))


class PNotifierList : public PObject
{
  PCLASSINFO(PNotifierList, PObject);
  private:
    PLIST(_PNotifierList, PNotifier);

    _PNotifierList m_TheList;

    // Removes smart pointers to deleted objects
    void   Cleanup();

  public:
    PINDEX GetSize() const { return m_TheList.GetSize(); }

    void Add(PNotifier * handler)       { m_TheList.Append(handler); }
    void Remove(PNotifier * handler)    { m_TheList.Remove(handler); }
    BOOL RemoveTarget(PObject * obj);
    BOOL Fire(PObject& obj, INT val = 0);

    // Moves all the notifiers in "that" to "this"
    void  Move(PNotifierList& that);
};


#endif  // _PNOTIFIER_EXT

// End of File ///////////////////////////////////////////////////////////////



