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
 * @file rtxStreamFile.h
 */
#ifndef _RTXSTREAMFILE_H_
#define _RTXSTREAMFILE_H_

#ifndef _NO_STREAM

#include <stdio.h>
#include "rtxsrc/rtxStream.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup rtxStreamFile File stream functions.
 * File stream functions are used for stream operations with files. @{
 */
/**
 * Attaches the existing file structure pointer to the stream. The file should
 * be already opened either for the reading or writing. The 'flags' parameter
 * specifies the access mode for the stream - input or output.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pFile        Pointer to FILE structure. File should be already opened
 *                       either for the writing or reading.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamFileAttach (OSCTXT* pctxt,
                                   FILE* pFile, OSUINT16 flags);

/**
 * Opens a file stream. The 'flags' parameter specifies the access mode for the
 * stream - input or output.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pFilename    Pointer to null-terminated string that contains the name
 *                       of file.
 * @param flags        Specifies the access mode for the stream:
 *                       - OSRTSTRMF_INPUT = input (reading) stream;
 *                       - OSRTSTRMF_OUTPUT = output (writing) stream.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamFileOpen (OSCTXT* pctxt,
                                 const char* pFilename, OSUINT16 flags);

/**
 * This function creates an input file stream using the specified file name.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pFilename    Pointer to null-terminated string that contains the name
 *                       of file.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamFileCreateReader (OSCTXT* pctxt, const char* pFilename);

/**
 * This function creates an output file stream using the file name.
 *
 * @param pctxt        Pointer to a context structure variable that has been
 *                       initialized for stream operations.
 * @param pFilename    Pointer to null-terminated string that contains the name
 *                       of file.
 * @return             Completion status of operation: 0 = success, negative
 *                       return value is error.
 */
EXTERNRT int rtxStreamFileCreateWriter (OSCTXT* pctxt, const char* pFilename);


#ifdef __cplusplus
}
#endif
/**
 * @} rtxStreamFile
 */

#endif /* _NO_STREAM */
#endif /* _RTXSTREAMFILE_H_ */
