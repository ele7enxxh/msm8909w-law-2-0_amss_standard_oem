#ifndef __QXBASE64_H__
#define __QXBASE64_H__

/**
   @file IxBase64.h

   @brief 
   Interface defination for base64 encoding.

   This header file defines the interface to IxBase64,
   which defines an interface to Encode and Decode the data 
   using the base64 encoding and decoding mechanism.

   Externalized Functions:
   EncodeBase64 - This function encodes the given the string.
   DecodeBase64 - This function decodes the given encoded string.

   Initialization and sequencing requirements:
   None.
*/

/*===========================================================================
                              Q X  B A S E 6 4
                            H E A D E R  F I L E
Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved. 
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/IxBase64.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ 2005/10/06 
$Author: mplcsds1 $ Madhav Chinthala  


when        who         what, where, why
--------    ---         ----------------------------------------------------
01/11/11    leo        (Tech Pubs) Edited Doxygen comments and markup.
12/10/10    nk         Doxygenated.
12/20/05    MC         Updated function headers with error info
12/09/05    MC         Updated header preprocessor directive
10/24/05    MC         Updated with 2 space indentation
10/07/05    MC         Updates with review comments
2005/10/06  Madhav C   Initial Version


===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "comdef.h"

/** @addtogroup SMECOMUTIL_ENCODERS
    @{
*/

/** @name Base64 Encoding Constants
@{ */

/*
  Error base value for Base64 encoding.
*/
//#define QXBASE64_ERR_BASE     200

/**
   Success value for Base64 encoding.
*/
#define QXBASE64_SUCCESS        0 

/**
   Error value of an invalid string length for Base64 encoding.
*/
#define QXBASE64_INVALID_LEN    -1

/**
   Error value of an invalid string for Base64 encoding.
*/
#define QXBASE64_BOGUS_STRING   -2

/**
   Error value of an invalid parameter for Base64 encoding.
*/
#define QXBASE64_INVALID_PAR    -3 

/**
   Has the value 127 for Base64 encoding.
*/
#define XX 127

/**
   Returns the Base64 string length.
*/
#define BASE64LEN(x) (((((x) + 2) / 3) * 4) + 1)

/** @} */ /* end_name base64 encoding constants */

/** @} */ /* end_addtogroup SMECOMUTIL_ENCODERS */

#ifndef __cplusplus

/** @addtogroup CAPI 
    @{
*/

/**
  @brief Dummy implementation for IxBase64 for C compatibility
*/
typedef struct
{
uint32 dummy;
} IxBase64;

/** @} */ /* end_addtogroup CAPI */

/** @addtogroup CAPI
    @{
*/

/** @name IXBASE64_C_API
    These functions are C wrappers for a subset of the IxBase64 API. The
    name of each C wrapper function indicates in an obvious way to which
    C++ method it corresponds.
    @{ */

/**
  C call to create or instantiate an IxBase64 object.

   @return
   Returns the pointer to an IxBase64 object created.

   @dependencies 
   None.
*/
IxBase64*       Call_IxBase64_New(void);

/** @} */  /* end_name IXBASE64_C_API */

/** @} */  /* end_addtogroup CAPI */

/** @addtogroup CAPI
    @{
*/

/** @name IXBASE64_C_API
    @{ */

/**
   C call to delete an already created IxBase64 object.

   @param[in] obj Pointer to the IxBase64 object to be deleted.

   @return 
   None.

   @dependencies 
   None.
*/
void Call_IxBase64_Delete(IxBase64 *obj);

/**
   C call to decode a Base64 input string.

   @param[in] obj   Pointer to the IxBase64 object.
   @param[in] cpcIn Pointer to the input location of the encoded Base64 string.
   @param[in] pcOut Pointer to the output location in which to place the decoded
                    string.

   @return
   On success, returns the number of bytes decoded.\n
   On failure, returns one of the following errors:
   - QXBASE64_INVALID_PAR -- Invalid parameter.
   - QXBASE64_INVALID_LEN -- Invalid input string length.
   - QXBASE64_BOGUS_STRING -- Invalid characters in the string.

   @dependencies 
   None.
*/
int32 Call_IxBase64_DecodeBase64(IxBase64 *obj, const char *cpcIn, char *pcOut);

/**
   C call to encode an input string to Base64.

   @param[in] obj    Pointer to the IxBase64 object.
   @param[in] cpcIn  Pointer to the input string.
   @param[in] nInLen Length of the input string.
   @param[in] pcOut  Pointer to the output location in which to place the
                     encoded Base64 string.

   @return
   Returns a pointer to the output string or NULL if the output string pointer
   is not valid.

   @dependencies 
   None.
*/
char*  Call_IxBase64_EncodeBase64(IxBase64 *obj, const char *cpcIn, int32  nInLen, char *pcOut);

/** @} */  /* end_name IXBASE64_C_API */

/** @} */  /* end_addtogroup CAPI */

#else //__cplusplus

/** @addtogroup SMECOMUTIL_ENCODERS
    @{
*/

/*
   Definitions for C++ files
*/

/**
   @brief Class to provide an interface for Base64 encoding and decoding for
   strings.
*/
class IxBase64 {

public: 
/**
   Constructor that creates an IxBase64 object.
*/
IxBase64();

/**
   Destructor that deletes the IxBase64 object and reclaims any of the
   underlying buffers of the IxBase64 object.
*/
~IxBase64();

public: 

/**
   Decodes the printable test string to binary data using a Base64 content
   encoding transfer mechanism.

   @param[in] cpcIn  Pointer to the encoded Base64 string.
   @param[out] pcOut Pointer to the output location in which to place the
                     decoded string.

   @return
   On success, returns the number of bytes decoded.\n
   On failure, returns one of the following errors:
   - QXBASE64_INVALID_PAR -- Invalid parameter.
   - QXBASE64_INVALID_LEN -- Invalid input string length.
   - QXBASE64_BOGUS_STRING -- Invalid characters in the string.
*/
int32 DecodeBase64(const char *cpcIn, char *pcOut);

/**
   Encodes the specified string.

   @param[in] cpcIn  Pointer to the input string to be encoded.
   @param[in] nInLen Length of the input string.
   @param[out] pcOut Pointer to the output location in which to place the
                     encoded string.

   @return
   Returns a pointer to the output string or NULL if the output string
   pointer is not valid.
*/
char  *EncodeBase64(const char *cpcIn, int32  nInLen, char *pcOut);

private: // data members

};

/** @} */ /* end_addtogroup SMECOMUTIL_ENCODERS */

#endif //_cplusplus

#endif // __QXBASe64_H__
