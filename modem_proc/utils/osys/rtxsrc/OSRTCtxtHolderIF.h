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
/**
 * @file OSRTCtxtHolderIF.h
 * C++ run-time message buffer interface class definition.
 */
#ifndef _OSRTCTXTHOLDERIF_H_
#define _OSRTCTXTHOLDERIF_H_

#include "rtxsrc/OSRTContext.h"

/**
 * Abstract message buffer or stream interface class. This is the base class
 * for both the in-memory message buffer classes and the run-time stream
 * classes.
 */
class EXTRTCLASS OSRTCtxtHolderIF {
 protected:
   /**
    * The virtual destructor does nothing. It is overridden by derived versions
    * of this class.
    */
   virtual ~OSRTCtxtHolderIF () {}

 public:
   // Virtual methods
   /**
    * The getContext method returns the underlying context smart-pointer
    * object.
    *
    * @return Context smart pointer object.
    */

   virtual OSRTCtxtPtr getContext () = 0;

   /**
    * The getCtxtPtr method returns the underlying C runtime context. This
    * context can be used in calls to C runtime functions.
    *
    * @return The pointer to C runtime context.
    */
   virtual OSCTXT* getCtxtPtr () = 0;

   /**
    * Returns error text in a dynamic memory buffer. Buffer will be allocated by
    * 'operator new []'. The calling routine is responsible to free the memory
    * by using 'operator delete []'.
    *
    * @return             A pointer to a newly allocated buffer with error text.
    */
   virtual char* getErrorInfo () = 0;

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
    *                       Otherwise, returns newly allocated buffer with error
    *                       text. NULL, if error occurred.
    */
   virtual char* getErrorInfo (char* pBuf, size_t& bufSize) = 0;

   /**
    * This method returns the completion status of previous operation.
    * It can be used to check completion status of constructors or methods,
    * which do not return completion status. If error occurs, use
    * printErrorInfo method to print out the error's description and
    * stack trace.
    * Method resetError can be used to reset error to continue operations
    * after recovering from the error.
    *
    * @return            Runtime status code:
    *                      - 0 (0) = success,
    *                      - negative return value is error.
    */
   virtual int getStatus () const = 0;

   /**
    * The printErrorInfo method prints information on errors contained within
    * the context.
    */
   virtual void printErrorInfo () = 0;

   /**
    * The resetErrorInfo method resets information on errors contained within
    * the context.
    */
   virtual void resetErrorInfo () = 0;

};

#endif // _OSRTCTXTHOLDERIF_H_
