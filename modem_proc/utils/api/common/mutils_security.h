/*!
  @file mutils_security.h

  @brief
  The commmon security header file for MODEM

  @detail
  OPTIONAL detailed description of this C header file.
  - DELETE this section if unused.

*/

/*==============================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/29/12   kp      Added code for security algo ZUC.
12/03/09   sm      Removed unwanted #ifndef's and #if 0's
11/13/09   dd      Changed htonl and ntohl to mutils_htonl and mutils_ntohl
10/26/09   sm      Moved sw wrapper support from /lte/common/ to /mutils/ and
                   prefix the same with mutils_instead of lte_
04/17/09   dd      Initial version  
==============================================================================*/

#ifndef MUTILS_SECURITY_H
#define MUTILS_SECURITY_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <IxErrno.h>
#include <dsm_item.h>

/*==============================================================================

                   MACROS 

==============================================================================*/


/*==============================================================================

MACRO HTONL()

DESCRIPTION
  Converts host-to-network long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The network byte-ordered value.

SIDE EFFECTS
  None.

==============================================================================*/
/*lint -emacro(572,htonl) The bit shifting is correct*/
#define mutils_htonl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
        (((uint32)(x) & 0x0000ff00U) <<  8) | \
        (((uint32)(x) & 0x00ff0000U) >>  8) | \
        (((uint32)(x) & 0xff000000U) >> 24)))

/*==============================================================================

MACRO NTOHL()

DESCRIPTION
  Converts network-to-host long integer.  Handles potential byte order
  differences between different computer architectures and different network
  protocols.

PARAMETERS
  x     unsigned long integer value to be converted.

DEPENDENCIES
  None.

RETURN VALUE
  The host byte-ordered value.

SIDE EFFECTS
  None.

==============================================================================*/
#define mutils_ntohl(x) \
        ((uint32)((((uint32)(x) & 0x000000ffU) << 24) | \
        (((uint32)(x) & 0x0000ff00U) <<  8) | \
        (((uint32)(x) & 0x00ff0000U) >>  8) | \
        (((uint32)(x) & 0xff000000U) >> 24)))


/*! @brief Cipher or Integrity key length in bytes */
#define MUTILS_CIPHER_INTEGRITY_KEY_LEN 16

/*! @brief Max PDU length that can be (de)ciphered using mutils security API
    Need to handle atleast around 4200 bytes LTE SRB PDU. Max payload size
    limit as per LTE spec is 8188 bytes. Make it cache line aligned */
#define MUTILS_SECURITY_MAX_MSG_LEN 8224

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

typedef errno_enum_type mutils_sec_errno_e;

/*! @brief Ciphering Algorithm
*/
typedef enum
{
  MUTILS_CIPHER_ALGO_NONE    = 0x7, /*!< No Ciphering; equivalent to eea0 */
  MUTILS_CIPHER_ALGO_SNOW3G  = 0x2, /*!< SNOW-3G; equivalent to eea1 */
  MUTILS_CIPHER_ALGO_AES     = 0x3, /*!< AES; equivalent to eea2 */
  MUTILS_CIPHER_ALGO_ZUC     = 0x4, /*!< ZUC; equivalent to eea3 or spare5 */
  MUTILS_CIPHER_ALGO_KASUMI  = 0x5, /*!< Kasumi; equivalent to UEA1 for WCDMA */
} mutils_cipher_algo_e;

/*! @brief Integrity Algorithm
*/
typedef enum
{
  MUTILS_INTEGRITY_ALGO_NONE   = 0x7,  /*!< No integrity; equivalent to eia0 */
  MUTILS_INTEGRITY_ALGO_SNOW3G = 0x1,  /*!< SNOW-3G; equivalent to eia1 */
  MUTILS_INTEGRITY_ALGO_AES    = 0x2,  /*!< AES; equivalent to eia2 */
  MUTILS_INTEGRITY_ALGO_ZUC    = 0x3,  /*!< ZUC; equivalent to eia3 or spare5 */
  MUTILS_INTEGRITY_ALGO_KASUMI = 0x4,  /*!< Kasumi; equivalent to UIA1 for WCDMA*/
} mutils_integrity_algo_e;

typedef enum
{
  MUTILS_CIPHER_DIRECTION_UL = 0,
  MUTILS_CIPHER_DIRECTION_DL = 1,
} mutils_cipher_direction_t;

typedef enum
{
  MUTILS_INTEGRITY_DIRECTION_UL = 0,
  MUTILS_INTEGRITY_DIRECTION_DL = 1,
} mutils_integrity_direction_t;

typedef enum
{
  MUTILS_TECHNOLOGY_NONE = 0,
  MUTILS_TECHNOLOGY_LTE = 1,
  MUTILS_TECHNOLOGY_WCDMA = 2,
  MUTILS_TECHNOLOGY_MAX = 3
} mutils_technology_t;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/


/*==============================================================================

  FUNCTION:  mutils_security_stream_compute_integrity_maci
==============================================================================*/
/*!
    @brief
    This fn computes the MAC-I for a given input message 

    @detail

    @return
    mutils_sec_errno_e
*/
/*============================================================================*/

mutils_sec_errno_e mutils_security_stream_compute_integrity_maci
(
  mutils_technology_t          technology, /*!< Technology */
  mutils_integrity_algo_e      algo,/*!<Integrity Algorithm  */
  /*! Input key*/
  uint8                        key[MUTILS_CIPHER_INTEGRITY_KEY_LEN],
  uint8                        *in_msg_ptr,/*! < Input Msg ptr  */
  uint64                       in_msg_len, /*! < Input msg len  */
  uint32*                      maci_ptr,/*!<Output MAC-I ptr */
  uint8                        bearer,/*!< Bearer ID ranges from 0 to 31*/
  uint32                       fresh, /*!< Fresh value */
  uint32                       count,/*!< Count */
  mutils_integrity_direction_t direction /*!< UL = 0, DL = 1*/
);


/*==============================================================================

  FUNCTION:  mutils_security_stream_cipher

==============================================================================*/
/*!
    @brief
    Does ciphering on the input data stream 
    @return
    mutils_sec_errno_e

 */
/*============================================================================*/


extern mutils_sec_errno_e mutils_security_stream_cipher
( 
  mutils_technology_t          technology, /*!< Technology */
  mutils_cipher_algo_e      algo, /*!<Ciphering Algorithm  */
  uint8                     key[MUTILS_CIPHER_INTEGRITY_KEY_LEN],/*!< Inputkey*/
  uint8                     *in_msg_ptr, /*!< Ptr to the input msg*/
  uint16                    in_msg_byte_len, /*!< Input Msg length in bytes*/
  uint8                     *out_msg_ptr, /*!< Output msg ptr*/
  uint8                     bearer, /*!< Bearer ID ranges from 0 to 31*/
  uint32                    count /*!< Count*/
);

/*==============================================================================

  FUNCTION:  mutils_security_stream_decipher

==============================================================================*/
/*!
    @brief
    Does deciphering on the input data stream 
    @return
    mutils_sec_errno_e

 */
/*============================================================================*/

extern mutils_sec_errno_e mutils_security_stream_decipher
( 
  mutils_technology_t       technology, /*!< Technology */
  mutils_cipher_algo_e      algo, /*!<Ciphering Algorithm  */
  uint8                     key[MUTILS_CIPHER_INTEGRITY_KEY_LEN],/*!< Inputkey*/
  uint8                     *in_msg_ptr, /*!< Ptr to the input msg*/
  uint16                    in_msg_byte_len,/*!< Input Msg length in bytes*/
  uint8                     *out_msg_ptr,/*!< Output msg ptr*/
  uint8                     bearer,/*!< Bearer ID ranges from 0 to 31*/
  uint32                    count /*!< Count*/

);


#endif /* MUTILS_SECURITY_H */

