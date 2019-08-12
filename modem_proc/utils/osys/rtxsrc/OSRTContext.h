/*
 * Copyright (c) 2003-2013 Objective Systems, Inc.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by Objective Systems, Inc.
 *
 * PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 *****************************************************************************/
/*The following is the text for the main C++ Runtime index page.*/
/*! \mainpage
 * <H1>C++ Common Runtime Library Classes</H1>
 *
 * The <B>OSRT C++ run-time classes</B> are wrapper classes that provide an
 * object-oriented interface to the common C run-time library functions. The
 * categories of classes provided are as follows:
 * <UL>
 * <LI>Context management classes manage the context structure (OSCTXT)
 * used to keep track of the working variables required to encode or
 * decode XML messages.</LI>
 * <LI>Message buffer classes are used to manage message buffers for
 * encoding or decoding XML messages.</LI>
 * <LI>XSD type base classes are used as the base for compiler-
 * generated C++ data structures. </LI>
 * <LI>Stream classes are used to read and write messages to and from
 * files, sockets, and memory buffers.</LI>
 * </UL>
 */
/**
 * @file OSRTContext.h
 * C++ run-time context class definition.
 */
#ifndef _OSRTCONTEXT_H_
#define _OSRTCONTEXT_H_

#include "rtxsrc/rtxContext.h"
#include "rtxsrc/rtxDiag.h"
#include "rtxsrc/rtxError.h"
#include "rtxsrc/rtxMemory.h"

#if defined(_MSC_VER)
// this disables 'unreferenced inline function has been removed' warning
#pragma warning(disable: 4514)
#endif

/**
 * Reference counted context class. This keeps track of all encode/decode
 * function variables between function invocations. It is reference counted to
 * allow a message buffer and type class to share access to it.
 */
class EXTRTCLASS OSRTContext {
 private:
   EXTRTMETHOD OSRTContext (OSCTXT*);

 protected:
   /**
    * The mCtxt member variable is a standard C runtime context variable used
    * in most C runtime function calls.
    */
   OSCTXT mCtxt;

   /**
    * The mCount member variable holds the reference count of this context.
    */
   OSUINT32 mCount;

   /**
    * TRUE, if initialized correctly, FALSE otherwise
    */
   OSBOOL mbInitialized;

   /**
    * The mStatus variable holds the return status from C run-time function
    * calls.  The getStatus method will either return this status or the
    * last status on the context error list.
    */
   int mStatus;

 public:
   /**
    * The default constructor initializes the mCtxt member variable and sets
    * the reference count variable (mCount) to zero.
    */
   EXTRTMETHOD OSRTContext ();

   /**
    * The destructor frees all memory held by the context.
    */
   virtual EXTRTMETHOD ~OSRTContext ();

   /**
    * The getPtr method returns a pointer to the mCtxt member variable. A user
    * can use this function to get the the context pointer variable for use in
    * a C runtime function call.
    */
   inline OSCTXT* getPtr () { return &mCtxt; }

   inline const OSCTXT* getPtr () const { return &mCtxt; }

   /**
    * The getRefCount method returns the current reference count.
    */
   EXTRTMETHOD OSUINT32 getRefCount();

   /**
    * The getStatus method returns the runtime status code value.
    * @return            Runtime status code:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   inline int getStatus () const {
      return (0 == mStatus) ? rtxErrGetLastError (&mCtxt) : mStatus;
   }

   /**
    * Returns TRUE, if initialized correctly, FALSE otherwise.
    * @return TRUE, if initialized correctly, FALSE otherwise.
    */
   inline OSBOOL isInitialized () { return mbInitialized; }

   /**
    * The _ref method increases the reference count by one.
    */
   EXTRTMETHOD void _ref();

   /**
    * The _unref method decreases the reference count by one.
    */
   EXTRTMETHOD void _unref();

   /**
    * Returns error text in a dynamic memory buffer. Buffer will be allocated
    * using 'operator new []'. The calling routine is responsible for freeing
    * the memory by using 'operator delete []'.
    *
    * @return  A pointer to a newly allocated buffer with error text, or NULL
               if an error occurred.
    */
   EXTRTMETHOD char* getErrorInfo ();

   /**
    * Returns error text in a dynamic memory buffer. Buffer will be allocated
    * using 'operator new []'. The calling routine is responsible for freeing
    * the memory by using 'operator delete []'.
    *
    * @param pBufSize     A pointer to buffer size.  It will
    *                     receive the size of allocated dynamic buffer, or
    *                     (size_t)-1 if an error occurred.
    *
    * @return  A pointer to a newly allocated buffer with error text, or NULL
    *          if an error occurred.
    */
   EXTRTMETHOD char* getErrorInfo (size_t* pBufSize);

   /**
    * Returns error text in a memory buffer. If buffer pointer is specified in
    * parameters (not NULL) then error text will be copied in the passed
    * buffer. Otherwise, this method allocates memory using the
    * 'operator new []' function. The calling routine is responsible to free
    * the memory by using 'operator delete []'.
    *
    * @param pBuf         A pointer to a destination buffer to obtain the error
    *                       text. If NULL, dynamic buffer will be allocated.
    * @param bufSize      A reference to buffer size. If pBuf is NULL it will
    *                       receive the size of allocated dynamic buffer.
    * @return             A pointer to a buffer with error text. If pBuf is not
    *                       NULL, the return pointer will be equal to it.
    *                       Otherwise, returns newly allocated buffer with
    *                       error text. NULL, if error occurred.
    */
   EXTRTMETHOD char* getErrorInfo (char* pBuf, size_t& bufSize);

   /**
    * The memAlloc method allocates memory using the C runtime memory
    * management functions. The memory is tracked in the underlying context
    * structure. When both this OSXSDGlobalElement derived control class
    * object and the message buffer object are destroyed, this memory will
    * be freed.
    *
    * @param numocts      - Number of bytes of memory to allocate
    */
   inline void* memAlloc (size_t numocts) {
      return rtxMemAlloc (&mCtxt, numocts);
   }

   /**
    * The \c memFreeAll method will free all memory currently tracked within
    * the context. This includes all memory allocated with the memAlloc method
    * as well as any memory allocated using the C \c rtxMemAlloc function with
    * the context returned by the \c getCtxtPtr method.
    */
   inline void memFreeAll () {
      rtxMemFree (&mCtxt);
   }

   /**
    * The memFreePtr method frees the memory at a specific location. This
    * memory must have been allocated using the memAlloc method described
    * earlier.
    *
    * @param ptr          - Pointer to a block of memory allocated with \c
    *                       memAlloc
    */
   inline void memFreePtr (void* ptr) {
      rtxMemFreePtr (&mCtxt, ptr);
   }

   /**
    * The memRealloc method reallocates memory using the C runtime memory
    * management functions.
    *
    * @param ptr          - Original pointer containing dynamic memory to be
    *                       resized.
    * @param numocts      - Number of bytes of memory to allocate
    * @return             Reallocated memory pointer
    */
   inline void* memRealloc (void* ptr, size_t numocts) {
      return rtxMemRealloc (&mCtxt, ptr, numocts);
   }

   /**
    * The memReset method resets dynamic memory using the C runtime memory
    * management functions.
    */
   inline void memReset () {
      rtxMemReset (&mCtxt);
   }

   /**
    * The printErrorInfo method prints information on errors contained within
    * the context.
    */
   inline void printErrorInfo () {
      rtxErrPrint (&mCtxt);
   }

   /**
    * The resetErrorInfo method resets information on errors contained within
    * the context.
    */
   inline void resetErrorInfo () {
      rtxErrReset (&mCtxt);
   }

   /**
    * The setDiag method will turn diagnostic tracing on or off.
    *
    * @param value - Boolean value (default = TRUE = on)
    * @return - Previous state of the diagnostics enabled boolean
    */
   inline OSBOOL setDiag (OSBOOL value=TRUE) {
      return rtxSetDiag (&mCtxt, value);
   }

   /**
    * This method sets run-time key to the context. This method does nothing
    * for unlimited redistribution libraries.
    *
    * @param key - array of octets with the key
    * @param keylen - number of octets in key array.
    * @return            Completion status of operation:
    *                      - 0 = success,
    *                      - negative return value is error.
    */
   virtual EXTRTMETHOD int setRunTimeKey (const OSOCTET* key, size_t keylen);

   /**
    * This method sets error status in the context.
    *
    * @param stat Status value.
    * @return     Error status value being set.
    */
   inline int setStatus (int stat) {
      mStatus = stat;
      return stat;
   }

};

/**
 * Context reference counted pointer class. This class allows a
 * context object to automatically be released when its reference count
 * goes to zero. It is very similar to the standard C++ library auto_ptr
 * smart pointer class but only works with an OSRTContext object.
 */
class EXTRTCLASS OSRTCtxtPtr {
 protected:
   /**
    * The mPointer member variable is a pointer to a reference-counted ASN.1
    * context wrapper class object.
    */
   OSRTContext* mPointer;

 public:
   /**
    * This constructor set the internal context pointer to the given value and,
    * if it is non-zero, increases the reference count by one.
    *
    * @param rf           - Pointer to OSRTContext object
    */
   OSRTCtxtPtr (OSRTContext* rf = 0) : mPointer(rf) {
      if (mPointer != 0) mPointer->_ref();
   }

   /**
    * The copy constructor copies the pointer from the source pointer object
    * and, if it is non-zero, increases the reference count by one.
    *
    * @param o            - Reference to OSRTCtxtPtr object to be copied
    */
   OSRTCtxtPtr (const OSRTCtxtPtr& o) : mPointer(o.mPointer) {
      if (mPointer != 0) mPointer->_ref();
   }

   /**
    * The destructor decrements the reference counter to the internal context
    * pointer object. The context object will delete itself if its reference
    * count goes to zero.
    */
   virtual ~OSRTCtxtPtr() { if (mPointer != 0) mPointer->_unref(); }

   /**
    * This assignment operator assigns this OSRTCtxtPtr to another. The
    * reference count of the context object managed by this object is first
    * decremented. Then the new pointer is assigned and that object's reference
    * count is incremented.
    *
    * @param rf           - Pointer to OSRTCtxtPtr smart-pointer object
    */
   OSRTCtxtPtr& operator= (const OSRTCtxtPtr& rf) {
      if (mPointer != 0) mPointer->_unref();
      mPointer = rf.mPointer;
      if (mPointer != 0) mPointer->_ref();
      return *this;
   }

   /**
    * This assignment operator assigns does a direct assignment of an
    * OSRTContext object to this OSRTCtxtPtr object.
    */
   OSRTCtxtPtr& operator= (OSRTContext* rf) {
      if (mPointer != 0) mPointer->_unref();
      mPointer = rf;
      if (mPointer != 0) mPointer->_ref();
      return *this;
   }

   /**
    * The 'OSRTContext*' operator returns the context object pointer.
    */
   operator OSRTContext* () { return mPointer; }
   operator const OSRTContext* () const { return mPointer; }

   /**
    * The '->' operator returns the context object pointer.
    */
   OSRTContext* operator -> () { return mPointer; }
   const OSRTContext* operator -> () const { return mPointer; }

   /**
    * The '==' operator compares two OSRTContext pointer values.
    */
   OSBOOL operator == (const OSRTContext* o) const {
      return (mPointer == o);
   }

   /**
    * The isNull method returns TRUE if the underlying context pointer is NULL.
    */
   inline OSBOOL isNull() const { return (mPointer == 0); }

   /**
    * This method returns the standard context pointer used in C function
    * calls.
    */
   inline OSCTXT* getCtxtPtr() { return isNull() ? 0 : mPointer->getPtr(); }

};

/**
 * Custom placement new function to allocate memory using context
 * memory-management functions.
 */
EXTERNRT void* operator new (size_t nbytes, OSCTXT* pctxt);

/**
 * Custom placement delete function to free memory using context
 * memory-management functions.
 */
EXTERNRT void operator delete (void* pmem, OSCTXT* pctxt);

#endif // _OSRTCONTEXT_H_
