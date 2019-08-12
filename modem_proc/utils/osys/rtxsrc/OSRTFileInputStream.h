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
 * @file OSRTFileInputStream.h
 * C++ base class definitions for operations with input file streams.
 */
#ifndef _OSRTFILEINPUTSTREAM_H_
#define _OSRTFILEINPUTSTREAM_H_

#include "rtxsrc/OSRTInputStream.h"

/**
 * Generic file input stream. This class opens an existing file for input in
 * binary mode and reads data from it.
 */
class EXTRTCLASS OSRTFileInputStream : public OSRTInputStream {
 public:
   /**
    * Creates and initializes a file input stream using the name of file.
    *
    * @param pFilename                Name of file.
    * @see                           ::rtxStreamFileOpen
    */
   EXTRTMETHOD OSRTFileInputStream (const char* pFilename);

   /**
    * Creates and initializes a file input stream using the name of file.
    *
    * @param pContext                 Pointer to a context to use.
    * @param pFilename                Name of file.
    * @see                           ::rtxStreamFileOpen
    */
   EXTRTMETHOD OSRTFileInputStream (OSRTContext* pContext, const char* pFilename);

   /**
    * Initializes the file input stream using the opened FILE structure
    * descriptor.
    *
    * @param file                     Pointer to FILE structure.
    * @see                           ::rtxStreamFileAttach
    */
   EXTRTMETHOD OSRTFileInputStream (FILE* file);

   /**
    * Initializes the file input stream using the opened FILE structure
    * descriptor.
    *
    * @param pContext                 Pointer to a context to use.
    * @param file                     Pointer to FILE structure.
    * @see                           ::rtxStreamFileAttach
    */
   EXTRTMETHOD OSRTFileInputStream (OSRTContext* pContext, FILE* file);

   /**
    * This method is used to query a stream object in order to determine
    * its actual type.
    *
    * @param id  Enumerated stream identifier
    * @return    True if the stream matches the identifier
    */
   virtual OSBOOL isA (StreamID id) const {
      return id == File;
   }

} ;

#endif /* _OSRTFILEINPUTSTREAM_H_ */
