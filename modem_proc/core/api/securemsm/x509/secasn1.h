#ifndef SECASN1_H
#define SECASN1_H

/** 
  @file secasn1.h
  @brief ASN.1 DER encoding.
  This file contains the definitions for the constants, data structures and 
  interfaces that provide ASN.1 DER encoding.
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The secx509_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      secx509_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the secx509_apis group 
      description in the secx509_mainpage.dox file. 
===========================================================================*/
/*===========================================================================
  Copyright (c) 2003 - 2011 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*==========================================================================
          EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/x509/secasn1.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/05/11   SD      (Tech Pubs) Updated the copyright year and mainpage reference.
10/07/10   SD      (Tech Pubs) Edited/added Doxygen comments and markup.
06/24/10   AF      (Tech Pubs) Edited/added Doxygen comments and markup.
06/06/10   qxu     added mainpage, descriptions for functions, constants and data types
                   to doxygenate API document.
04/03/06   lx      Add UTF8String support
01/24/03   Jay     Initial version

==========================================================================*/


/*==========================================================================

             Include Files for Module

==========================================================================*/
#include "comdef.h"
/** @addtogroup secx509_apis 
  @{ */

/**
@name ASN1 Tag Definitions
Following are input values for verify_tag_id in secasn1_next_field().
@{
*/

/* ASN.1 Constants and definitions */
#define SECASN1_NO_TYPE_CHECK         (0x00) /**< ASN.1 tag no check. */
#define SECASN1_BOOLEAN_TYPE          (0x01) /**< ASN.1 tag boolean type. */
#define SECASN1_INTEGER_TYPE          (0x02) /**< ASN.1 tag integer type. */
#define SECASN1_BIT_STRING_TYPE       (0x03) /**< ASN.1 tag bit string type. */
#define SECASN1_OCTET_STRING_TYPE     (0x04) /**< ASN.1 tag octet string type. */
#define SECASN1_NULL_TYPE             (0x05) /**< ASN.1 tag NULL type. */
#define SECASN1_OID_TYPE              (0x06) /**< ASN.1 tag object identifier 
                                                   type. */
#define SECASN1_UTF8_STRING_TYPE      (0x0c) /**< ASN.1 tag UTF8 string type. */
#define SECASN1_SEQUENCE_TYPE         (0x10) /**< ASN.1 tag sequence type. */
#define SECASN1_SET_TYPE              (0x11) /**< ASN.1 tag set type. */
#define SECASN1_PRINTABLE_STRING_TYPE (0x13) /**< ASN.1 tag printable string 
                                                   type. */
#define SECASN1_TELETEX_STRING_TYPE   (0x14) /**< ASN.1 tag teletex string type. */
#define SECASN1_UTC_TYPE              (0x17) /**< ASN.1 tag coordinated universal time type. */
/** @} */ /* end_name ASN1 Tag Definitions */
/** @} */ /* end_addtogroup_secx509_apis */
/*--------------------------------------------------------------------------
                Modular Constants and definitions
--------------------------------------------------------------------------*/
/** @addtogroup secx509_apis 
  @{ */

/**
  ASN.1 error codes.
*/
typedef enum
{
  E_ASN1_SUCCESS = 0,  /**< Success. */
  E_ASN1_INVALID_TAG,  /**< Invalid tag. */
  E_ASN1_NO_DATA,      /**< No data. */
  E_ASN1_INVALID_DATA, /**< Invalid data. */
  E_ASN1_INVALID_ARG   /**< Invalid argument. */
} secasn1_err_type;
/** @} */ /* end_addtogroup_secx509_apis */

/** @addtogroup secx509_apis 
  @{ */

/**
  @brief ASN.1 data holder.
*/
typedef struct
{
  uint8 *data; /**< ASN.1 data. */
  uint16 len;  /**< ASN.1 data length. */
} secasn1_data_type;

/**
  @brief ASN.1 bit string data holder.
*/
typedef struct
{
  uint8 *data;  /**< ASN.1 bit string data. */
  uint16 len;   /**< ASN.1 bit string data length. */
  uint8 unused; /**< Unused field. */
} secasn1_bit_string_type;
/** @} */ /* end_addtogroup_secx509_apis */


/*--------------------------------------------------------------------------
                Function Declarations
--------------------------------------------------------------------------*/
/** @addtogroup secx509_apis 
  @{ */

/**
  Starts the ASN.1 DER encoding by creating an initial container for the 
  next_field() function.

  @param[in] data         Pointer to the data.
  @param[in,out] ret_ptr  Pointer to the returned data to start the ASN.1 block.

  @return
  E_ASN1_SUCCESS     -- Sequence is properly started. \n
  E_ASN1_INVALID_ARG -- Pointer arguments have a NULL value. \n
  E_ASN1_INVALID_TAG -- No sequence tag at the beginning.

  @dependencies
  None.
*/
extern secasn1_err_type secasn1_start
(
  uint8             *data,
  secasn1_data_type *ret_ptr
);

/**
  Closes and verifies the ASN.1 DER encoding by checking that no 
  data has been left unprocessed at the end of the stream.

  @param[in] data_ptr   Pointer to the end of the data holder.

  @return
  E_ASN1_SUCCESS      -- Document has been closed successfully. \n
  E_ASN1_INVALID_ARG  -- Pointer argument has a NULL value. \n 
  E_ASN1_INVALID_DATA -- All the data was not processed. 

  @dependencies
  None.
*/
extern secasn1_err_type secasn1_end
(
  secasn1_data_type *data_ptr
);

/**
  Closes the ASN.1 sequence.

  This function verifies that all the data in the inner sequence has been 
  processed. The outer sequence is included in case a sequence has an undefined 
  length so the end marker (0x00, 0x00) can be removed.

  @param[in] outer Pointer to the outer sequence.
  @param[in] inner Pointer to the inner sequence.

  @return
  E_ASN1_SUCCESS      -- Sequence was successfully closed. \n
  E_ASN1_INVALID_ARG  -- Pointer argument has a NULL value. \n
  E_ASN1_INVALID_DATA -- Can be: \n
  - The data is not properly delimited according to the ASN.1 standard.
  - The inner and outer data pointers do not match.
  - The data has not finished parsing.

  @dependencies
  secasn1_next_field() has been called to start a sequence value. 
*/
extern secasn1_err_type secasn1_close_sequence
(
  secasn1_data_type *outer,
  secasn1_data_type *inner
);

/**
  Parses the next field of data in the ASN.1 DER encoding. This function points 
  secasn1_data_type to the data contained within the field.  

  @param[in] data_ptr      Pointer to the data to be parsed.
  @param[in] ret_ptr       Pointer to where the inner field data is returned.
  @param[in] verify_tag_id Tag value to check the tag against.

  @return
  If successful, E_ASN1_SUCCESS is returned, and ret_ptr is updated to contain 
  the data in the field. \n
  If an error occurs during parsing, an error code is returned. \n
  On an error, the data_ptr is returned to the initial position.

  @dependencies 
  ret_ptr must be allocated before it is passed into the function or the error
  E_ASN1_INVALID_ARG is returned. \n
  If verify_tag_id != 0, the function verifies the tag_id against the current 
  field's tags. \n
  If the tag_ids do not match, an error code of E_ASN1_INVALID_TAG is returned.
*/
extern secasn1_err_type secasn1_next_field
(
  secasn1_data_type *data_ptr,
  secasn1_data_type *ret_ptr,
  uint8 verify_tag_id
);

/**
  Decodes an ASN.1 data pointer to a bit string pointer.

  In the ASN.1 DER encoding, the first byte of a byte string indicates how many 
  unused bytes are at the end of the string. This function assumes that
  bit_ptr has been pre-allocated.

  @param[in] data_ptr Pointer to the bit string field.
  @param[in] bit_ptr  Pointer to where the decoded bit string is returned.

  @return
  If successful, E_ASN1_SUCCESS is returned, and bit_ptr is updated to contain 
  the bit string. \n
  If an error occurs during the conversion, an error code is returned.

  @dependencies 
  None.
*/
extern secasn1_err_type secasn1_decode_bit_string
(
  secasn1_data_type *data_ptr,
  secasn1_bit_string_type *bit_ptr
);

/**
  Decodes an ASN.1 data pointer to a Boolean pointer.

  In the ASN.1 DER encoding, the Boolean data is only a single byte, 
  with 0x00 meaning FALSE and anything else meaning TRUE.

  @param[in] data_ptr Pointer to the Boolean field.
  @param[in] b_val    Pointer to where the Boolean value is returned.

  @return
  If successful, E_ASN1_SUCCESS is returned, and the Boolean pointer is set. \n
  If the data_ptr contains more than a single byte, E_ASN1_INVALID_DATA is 
  returned.

  @dependencies 
  None.
*/
extern secasn1_err_type secasn1_decode_boolean
(
  secasn1_data_type *data_ptr,
  boolean *b_val
);

/**
  Decodes a data pointer to the number of seconds since CDMA epoch time 
  (January 6, 1980). If the time occurs before this date, the time is adjusted 
  to 0.

  @param[in] data_ptr Pointer to the time data field.
  @param[in] time     Pointer to where the time in seconds is returned.
  @param[in] utc_time Indicates whether the time is in coordinated universal time 
                      format.

  @return
  If successful, E_ASN1_SUCCESS is returned, and the time pointer is set. \n
  If an invalid format is encountered, E_ASN1_INVALID_DATA is returned.

  @dependencies
  None.
*/
extern secasn1_err_type secasn1_decode_time
(
  secasn1_data_type *data_ptr,
  uint32  *time,
  boolean utc_time
);

/** @} */ /* end_addtogroup secx509_apis */
#endif /* SECASN1_H */
