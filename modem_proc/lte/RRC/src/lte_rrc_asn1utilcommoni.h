
/*!
  @file
  lte_rrc_asn1utilcommoni.h

  @brief
  This file contains ASN1 utility functions for initializing, ASN1 encoding,
  ASN1 decoding, etc.

*/

/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/src/lte_rrc_asn1utilcommoni.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/09/10   aramani Initial Version
===========================================================================*/

#ifndef LTE_RRC_ASN1UTILCOMMONI_H
#define LTE_RRC_ASN1UTILCOMMONI_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Return code to indicate that more buffer is needed for encoding.
*/
#define LTE_RRC_ASN1_STATUS_MORE_BUF   RTERR_BUFOVFLW  


/*! @brief Structure containing encoded message and its length
*/
typedef struct
{
  long length;  /*!< Length of the encoded message */
  unsigned char *value;  /*!< Encoded message */
}lte_rrc_pdu_buf_s;


/*! @brief This structure is used to get ASN.1 encoding status when there is any
encoding failure
*/
typedef struct
{
  int32  encode_status;      /*!< Encoding failure cause       */ 
  int32  pdu_num;            /*!< PDU on which encoding failed */
  void   *unencoded_pdu_ptr;  /*!< Unencoded pdu ptr            */
} lte_rrc_encode_info_s;

/*! @brief This structure is used to get ASN.1 decoding status when there is any
decoding failure
*/
typedef struct
{
  int32  decode_status;      /*!< Decoding failure cause       */ 
  int32  pdu_num;            /*!< PDU on which Decoding failed */
  int32  pdu_size;           /*!< Size of the encoded string   */
  void   *encoded_pdu_ptr;    /*!< ptr to encoded string        */
  int32  decode_length;      /*!< Decoded length incase pre-allocated
                                   decode buffer is used        */
} lte_rrc_decode_info_s;

#endif /* LTE_RRC_ASN1UTILCOMMONI_H */

