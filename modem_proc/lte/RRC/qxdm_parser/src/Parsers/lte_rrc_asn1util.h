/*!
  @file
  lte_rrc_asn1util.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/qxdm_parser/src/Parsers/lte_rrc_asn1util.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/09   amit    Initial Version for qxdm_parser
10/03/08   sureshs Added lte_rrc_asn1_bitstring_to_int routine
09/09/08   amit    Added lte_rrc_asn1_encode_pdu_with_buf()
09/08/08   da      supressing lint error e123.  inp and oup are macro definition
                   inside comdef.h but used a func arguments in oss file.
07/31/08   amit    Initial Version
===========================================================================*/

#ifndef LTE_RRC_ASN1UTIL_H
#define LTE_RRC_ASN1UTIL_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "Common.h"
#include <lte_rrc_asn1.h>
//#include "../../../src/win_build/lte_rrc_asn1.h"
#include "IxErrno.h"
#include "comdef.h"
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Structure containing encoded message and its length
*/
typedef struct
{
  long length;  /*!< Length of the encoded message */
  unsigned char *value;  /*!< Encoded message */
} lte_rrc_pdu_buf_s;


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


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
extern IxErrnoType lte_rrc_oss_init
(
  void
);

extern void lte_rrc_oss_term
(
  void
);

extern void* lte_rrc_asn1_decode_pdu
(
  void *encoded_pdu_ptr,  /*!< The pointer to the PDU to decode */
  int pdu_num,  /*!< The PDU number to decode */
  uint32 encoded_pdu_size,   /*!< The size (in bytes) of the PDU */
  COutputBuffer *OutBufParam
);

extern int lte_rrc_asn1_free_pdu
(
  int pdunum,  /*!< The PDU number to be freed */
  void *buf  /*!< The PDU to be freed */
);

extern uint32 lte_rrc_asn1_get_number_of_pad_bits
(
  void
);

extern void lte_rrc_asn1_copy_decoded_pdu
(
  void *unencoded_pdu_ptr,  /*!< The pointer to the decoded PDU  */
  void **dest_ptr, /*!< Destination ptr - copy of decoded PDU*/
  int pdu_num  /*!< The PDU number of the decoded PDU to copy*/
);

extern int lte_rrc_asn1_print_decoded_pdu
(
  int pdu_num,  /*!< The PDU number of the decoded PDU to print*/
  void *unencoded_pdu_ptr  /*!< The pointer to the decoded PDU  */
);

/*===========================================================================
  The ossBitString is a structure containing a length field in bits and a 
  pointer to an array of one or more bytes. The bytes are ordered from most 
  significant to least significant. It is possible that the number of bits is 
  not divisible by 8. If this is the case, the remaining bits ( in the last 
  byte ) will reside in the upper bits of the byte.

  Returns integer containing contents of OSS bitstring.
*/
extern uint32 lte_rrc_asn1_bitstring_to_int
(
  uint8 max_bytes, /*!< Maximum number of bytes that are available to hold 
                        result use sizeof(uint32), sizeof(uint16), etc */
  ossBitString *bitstring_ptr /*!< Pointer to the OSS bitstring to be converted
                                   */
);
#endif /* LTE_RRC_ASN1UTIL_H */

