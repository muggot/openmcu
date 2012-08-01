#ifndef _PSMARTPTR_H
#define _PSMARTPTR_H

#include <ptlib.h>
#include <ptlib/object.h>

///////////////////////////////////////////////////////////////////////////////
// "Smart" pointers.

/** This is the base class for objects that use the {\it smart pointer} system.
   In conjunction with the #PSmartPointer# class, this class creates
   objects that can have the automatic deletion of the object instance when
   there are no more smart pointer instances pointing to it.

   A #PSmartObject# carries the reference count that the #PSmartPointer# 
   requires to determine if the pointer is needed any more and should be
   deleted.
 */
class PSmartObject : public PObject
{
  PCLASSINFO(PSmartObject, PObject);

  public:
    /** Construct a new smart object, subject to a #PSmartPointer# instance
       referencing it.
     */
    PSmartObject()
      :referenceCount(1) { }

  protected:
    /** Count of number of instances of #PSmartPointer# that currently
       reference the object instance.
     */
    PAtomicInteger referenceCount;


  friend class PSmartPointer;
};


/** This is the class for pointers to objects that use the {\it smart pointer}
   system. In conjunction with the #PSmartObject# class, this class
   references objects that can have the automatic deletion of the object
   instance when there are no more smart pointer instances pointing to it.

   A PSmartPointer carries the pointer to a #PSmartObject# instance which
   contains a reference count. Assigning or copying instances of smart pointers
   will automatically increment and decrement the reference count. When the
   last instance that references a #PSmartObject# instance is destroyed or
   overwritten, the #PSmartObject# is deleted.

   A NULL value is possible for a smart pointer. It can be detected via the
   #IsNULL()# function.
 */
class PSmartPointer : public PObject
{
  PCLASSINFO(PSmartPointer, PObject);

  public:
  /**@name Construction */
  //@{
    /** Create a new smart pointer instance and have it point to the specified
       #PSmartObject# instance.
     */
    PSmartPointer(
      PSmartObject * obj = NULL   /// Smart object to point to.
    ) { object = obj; }

    /** Create a new smart pointer and point it at the data pointed to by the
       #ptr# parameter. The reference count for the object being
       pointed at is incremented.
     */
    PSmartPointer(
      const PSmartPointer & ptr  /// Smart pointer to make a copy of.
    );

    /** Destroy the smart pointer and decrement the reference count on the
       object being pointed to. If there are no more references then the
       object is deleted.
     */
    virtual ~PSmartPointer();

    /** Assign this pointer to the value specified in the #ptr#
       parameter.

       The previous object being pointed to has its reference count
       decremented as this will no longer point to it. If there are no more
       references then the object is deleted.

       The new object being pointed to after the assignment has its reference
       count incremented.
     */
    PSmartPointer & operator=(
      const PSmartPointer & ptr  /// Smart pointer to assign.
    );
  //@}

  /**@name Overrides from class PObject */
  //@{
    /** Determine the relative rank of the pointers. This is identical to
       determining the relative rank of the integer values represented by the
       memory pointers.

       @return
       #EqualTo# if objects point to the same object instance,
       otherwise #LessThan# and #GreaterThan# may be
       returned depending on the relative values of the memory pointers.
     */
    virtual Comparison Compare(
      const PObject & obj   // Other smart pointer to compare against.
    ) const;
  //@}

  /**@name Pointer access functions */
  //@{
    /** Determine if the smart pointer has been set to point to an actual
       object instance.

       @return
       TRUE if the pointer is NULL.
     */
    BOOL IsNULL() const { return object == NULL; }

    /** Get the current value if the internal smart object pointer.

       @return
       pointer to object instance.
     */
    PSmartObject * GetObject() const { return object; }
  //@}

  protected:
    // Member variables
    /// Object the smart pointer points to.
    PSmartObject * object;
};


/** This template class creates a type safe version of PSmartPointer.
*/
template <class T> class PSmartPtr : public PSmartPointer
{
  PCLASSINFO(PSmartPtr, PSmartPointer);
  public:
    /// Constructor
    PSmartPtr(T * ptr = NULL)
      : PSmartPointer(ptr) { }

    /// Access to the members of the smart object in the smart pointer.
    T * operator->() const
      { return (T *)PAssertNULL(object); }

    /// Access to the dereferenced smart object in the smart pointer.
    T & operator*() const
      { return *(T *)PAssertNULL(object); }

    /// Access to the value of the smart pointer.
    operator T*() const
      { return (T *)object; }
};


#endif

