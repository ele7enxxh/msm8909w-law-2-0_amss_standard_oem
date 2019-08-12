#ifndef STR_UTIL_H
#define STR_UTIL_H

/**
   @file str_util.h

   @brief 
   This service will provide basic operations on a wide string.

   Externalized Functions:
   None.

   Initialization and sequencing requirements:
   None.
*/
/*============================================================================
                       S T R I N G    U T I L I T E S 
                           H E A D E R   F I L E 

Copyright (c) 2006-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
=========================================================================== */

/*=========================================================================
                            EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/str_util.h#1 $
$DateTime: 2016/12/13 07:59:23 $
$Author: mplcsds1 $  

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/24/11   nk      Removed banned string API
01/17/11   leo     (Tech Pubs) Edited Doxygen comments and markup.
12/28/10   nk      Doxygenated.
10/05/10   nk      Removed feature wrap PLATFORM_LTK and T_WINNT
07/20/10   nk      Added doxgen related string manuplation function descriptions
02/17/10   wa      Remove IxStrtok_r
08/30/06   ws      Add more utility functions
07/20/06   jay     Added a function to do file name encoding
04/20/06   rv      Added 'stricmp' for target
03/08/06   gr      Added 'strnicmp' for target
02/27/06   rv      Added functions strdup & strrev for target only
01/12/06   ws      Initial Revision

=========================================================================== */

#include <string.h>
#include "stringl.h"
#include <stdio.h>
#include <stdarg.h>
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SMECOMUTIL_STRING 
    @{
*/

/**

   Writes an output to the string buffer under control of the format string,
   which specifies how subsequent arguments are converted for output.

   @param[in] pBuffer   Pointer to the input buffer.
   @param[in] iFlag     Flag for the size of the string.
   @param[in] pszFormat Pointer to the string format.

   @return
   Returns -1 on failure or if not supported, and the number of printed
   characters on success.

   @dependencies 
   None.
*/
int snprintf(char *pBuffer, unsigned int iFlag, const char * pszFormat, ...);

/**
   Encodes the source string and output to the destination string. The Base64
   encoding is modified to use only FS-compatible characters.

   @param[in] pSrcString   Pointer to the source string.
   @param[in] SrcStringLen Length of the source string.
  @param[out] pDesString  Pointer to the destination string.

   @return
   Length of the destination string.

   @dependencies 
   None.
*/
size_t strutil_b64fs_encode( uint8*   pSrcString, 
size_t   SrcStringLen,
char*    pDesString );
/**
   String utility function.

   @param[in] pVoid   Void pointer to the write to buffer.
   @param[in] iBufLen Buffer length.
   @param[in] pFormat Pointer to the format string buffer.
   @param[in] list    Variable list.

   @return
   Returns the length of the destination string.

   @dependencies 
   None.
*/
int32 strutil_vxprintf(void *pVoid, uint32 iBufLen, const char *pFormat, va_list list);

/**
   Duplicates the source string. This function internally allocates the memory
   for a duplicate string buffer and returns a pointer of the duplicated
   string buffer.

   @param[in] pString Pointer to the source string.

   @return
   Returns a pointer to the duplicated string buffer.

   @dependencies
   None.

   @sideeffects 
   Caller must free the allocated duplicate buffer.
*/
char *IxStrdup(const char *pString);

/**
  strcasecmp - compare two strings ignoring case.

  @param[in] s1 First string.
  @param[in] s2 Second string.

   @return
  The strcasecmp() and strncasecmp() functions return an integer 
  less than, equal to, or greater than zero if s1 (or the first 
  n bytes  thereof)  is found, respectively, to be less than, to
  match, or be greater than s2. 

   @dependencies 
   None.
*/
#define IxStricmp(s1, s2) strcasecmp(s1, s2)

/**
   Reverses the order of the characters of the passed string and internally
   allocates the memory for a reverse string buffer. The ending NULL character
   (\0) remains in place.

   @param[in] pString Pointer to the string buffer.

   @return
   Returns the pointer to the reverse string buffer. There is no error return
   value.

   @dependencies 
   None.

   @sideeffects 
   Caller must free the allocated duplicate buffer.
*/
char *IxStrrev(const char *pString);

/**
   A wrapper over the standard string comparison function Strcmp().

   This function starts by comparing the first character in each string. If they
   are equal, it continues with the following pairs until the characters differ
   or until a terminanting NULL character is reached.

   @param[in] pString1 Pointer to string1 to be compared.
   @param[in] pString2 Pointer to string2 to be compared.

   @return
   Returns an integer value indicating the relationship between the strings:
   - = 0 -- Both strings are equal; also if both strings are NULL.
   - > 0 -- The first character that does not match has a greater value in
            string1 than in string2; = 1 can also mean that string2 is NULL.
   - < 0 -- The first character that does not match has a greater value in
            string2 than in string1; = -1 can also mean that string1 is NULL.

   @dependencies
   None.
*/
int IxStrcmp(const char *pString1, const char *pString2);

/**
  strncasecmp - compare two strings ignoring case (sized).

  @param[in] s1 First string.
  @param[in] s2 Second string.
  @param[in] n  The number of characters to compare (from the
                beginning).

   @return
  The strcasecmp() and strncasecmp() functions return an integer 
  less than, equal to, or greater than zero if s1 (or the first 
  n bytes  thereof)  is found, respectively, to be less than, to
  match, or be greater than s2. 

   @dependencies 
   None.
*/
#define IxStrnicmp(s1, s2, n) strncasecmp(s1, s2, n)

/**
  strlcat - Size bounded string concatenation.

  Concatenates the source string to destination string.

  This function ensures that the destination string will
  not be improperly terminated and that there will be
  no concatenation beyond the size of the destination buffer.

  @param[in,out]  dst   Destination buffer.
  @param[in]      src   Source string.
  @param[in]      siz   Size of the destination buffer in bytes.

   @return
  The length of the string that was attempted to be created,
  i.e. the sum of the source and destination strings.

   @dependencies 
   None.
*/
#define IxStrlcat(d, s, l) strlcat(d, s, l)

/**
  strlcpy - Size bounded string copy. 

  Copies the source string to the destination buffer.

  This function ensures that the destination buffer will always 
  be NULL terminated and that there will not be a copy beyond 
  the size of the destination buffer. 

  @param[out] dst   Destination buffer.
  @param[in]  src   Source String.
  @param[in]  siz   Size of the destination buffer in bytes.

   @return
  The length of the source string. 

   @dependencies 
   None.
*/
#define IxStrlcpy(d, s, l) strlcpy(d, s, l)

/**
   Searches for a specified substring in a given string, case independent.

   This function searches in a case-independent manner for the first occurrence 
   of the specified substring in the input string. 

   @param[in] pString    Pointer to the string to be searched.
   @param[in] pSubString Pointer to the search substring.

   @return
   Returns a pointer to the first occurrence of the substring or NULL.

   @dependencies 
   None.
*/
char* IxStristr(char* pString, const char* pSubString);

/** @} */  /* end_addtogroup SMECOMUTIL_STRING */

#ifdef __cplusplus
}
#endif

#endif /* STR_UTIL_H */
