#ifndef _PNOTIFIER_H
#define _PNOTIFIER_H

#include <ptlib.h>
#include <ptlib/smartptr.h>

///////////////////////////////////////////////////////////////////////////////
// General notification mechanism from one object to another

/** This class is the #PSmartObject# contents of the #PNotifier#
   class.

   This is an abstract class for which a descendent is declared for every
   function that may be called. The #PDECLARE_NOTIFIER# macro makes this
   declaration.

   The #PNotifier# and PNotifierFunction classes build a completely type
   safe mechanism for calling arbitrary member functions on classes. The
   "pointer to a member function" capability built into C++ makes the
   assumption that the function name exists in an ancestor class. If you wish
   to call a member function name that does {\bf not} exist in any ancestor
   class, very type unsafe casting of the member functions must be made. Some
   compilers will even refuse to do it at all!

   To overcome this problem, as this mechanism is highly desirable for callback
   functions in the GUI part of the PWLib library, these classes and a macro
   are used to create all the classes and declarations to use polymorphism as
   the link between the caller, which has no knowledege of the function, and
   the receiver object and member function.

   This is most often used as the notification of actions being take by
   interactors in the PWLib library.
 */
class PNotifierFunction : public PSmartObject
{
  PCLASSINFO(PNotifierFunction, PSmartObject);

  public:
    /// Create a notification function instance.
    PNotifierFunction(
      void * obj    ///< Object instance that the function will be called on.
    ) { object = PAssertNULL(obj); }

    /** Execute the call to the actual notification function on the object
       instance contained in this object.
     */
    virtual void Call(
      PObject & notifier,  ///< Object that is making the notification.
      INT extra            ///< Extra information that may be passed to function.
    ) const = 0;

  protected:
    // Member variables
    /** Object instance to receive the notification function call. */
    void * object;
};


/** This class is the #PSmartPointer# to the #PNotifierFunction#
   class.

   The PNotifier and #PNotifierFunction# classes build a completely type
   safe mechanism for calling arbitrary member functions on classes. The
   "pointer to a member function" capability built into C++ makes the
   assumption that the function name exists in an ancestor class. If you wish
   to call a member function name that does {\bf not} exist in any ancestor
   class, very type unsafe casting of the member functions must be made. Some
   compilers will even refuse to do it at all!

   To overcome this problem, as this mechanism is highly desirable for callback
   functions in the GUI part of the PWLib library, these classes and a macro
   are used to create all the classes and declarations to use polymorphism as
   the link between the caller, which has no knowledege of the function, and
   the receiver object and member function.

   This is most often used as the notification of actions being take by
   interactors in the PWLib library.
 */
class PNotifier : public PSmartPointer
{
  PCLASSINFO(PNotifier, PSmartPointer);

  public:
    /** Create a new notification function smart pointer. */
    PNotifier(
      PNotifierFunction * func = NULL   ///< Notifier function to call.
    ) : PSmartPointer(func) { }

    /**Execute the call to the actual notification function on the object
       instance contained in this object. This will make a polymorphic call to
       the function declared by the #PDECLARE_NOTIFIER# macro which in
       turn calls the required function in the destination object.
     */
    virtual void operator()(
      PObject & notifier,  ///< Object that is making the notification.
      INT extra            ///< Extra information that may be passed to function.
    ) const {
      if (PAssertNULL(object) != NULL)
        ((PNotifierFunction*)object)->Call(notifier,extra);
    }
};


/** Declare a notifier object class.
  This macro declares the descendent class of #PNotifierFunction# that
  will be used in instances of #PNotifier# created by the
  #PCREATE_NOTIFIER# or #PCREATE_NOTIFIER2# macros.

  The macro is expected to be used inside a class declaration. The class it
  declares will therefore be a nested class within the class being declared.
  The name of the new nested class is derived from the member function name
  which should guarentee the class names are unique.

  The #notifier# parameter is the class of the function that will be
  calling the notification function. The #notifiee# parameter is the
  class to which the called member function belongs. Finally the
  #func# parameter is the name of the member function to be
  declared.

  This macro will also declare the member function itself. This will be:
\begin{verbatim}
      void func(notifier & n, INT extra)
\end{verbatim}

  The implementation of the function is left for the user.
 */
#define PDECLARE_NOTIFIER(notifier, notifiee, func) \
  class func##_PNotifier : public PNotifierFunction { \
    public: \
      func##_PNotifier(notifiee * obj) : PNotifierFunction(obj) { } \
      virtual void Call(PObject & note, INT extra) const \
        { ((notifiee*)object)->func((notifier &)note, extra); } \
  }; \
  friend class func##_PNotifier; \
  virtual void func(notifier & note, INT extra)

/** Create a notifier object instance.
  This macro creates an instance of the particular #PNotifier# class using
  the #func# parameter as the member function to call.

  The #obj# parameter is the instance to call the function against.
  If the instance to be called is the current instance, ie #obj# is
  to #this# the the #PCREATE_NOTIFIER# macro should be used.
 */
#define PCREATE_NOTIFIER2(obj, func) PNotifier(new func##_PNotifier(obj))

/** Create a notifier object instance.
  This macro creates an instance of the particular #PNotifier# class using
  the #func# parameter as the member function to call.

  The #this# object is used as the instance to call the function
  against. The #PCREATE_NOTIFIER2# macro may be used if the instance to be
  called is not the current object instance.
 */
#define PCREATE_NOTIFIER(func) PCREATE_NOTIFIER2(this, func)

#endif

