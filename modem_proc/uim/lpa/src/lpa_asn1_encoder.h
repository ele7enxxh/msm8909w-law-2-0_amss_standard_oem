#ifndef LPA_ASN1_ENCODER_H
#define LPA_ASN1_ENCODER_H
/*===========================================================================


                      L P A   A S N 1   E N C O D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2016 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.


$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_asn1_encoder.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/15/16   ll      ASN1 encoder code clean up 
03/28/16   ll      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"
#include "lpa.h"
#include "lpa_util.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ASN1_PREPARE_DOWNLOAD_REQUEST_TYPE

   DESCRIPTION:
     This structure is used as input for encoding PrepareDownloadRequest request
     All the fields with _raw are either already in ASN1 format or dosen't need
     to be further encode to ASN1 format.
-----------------------------------------------------------------------------*/
typedef struct
{
  lpa_data_type             dp_signed1_raw;               /*TAG_SEQUENCE*/
  lpa_data_type             smdp_signature1_raw;          /*TAG_SMDPSIGNATURE1*/
  lpa_data_type             activation_code_token_raw;    /*TAG_UTF8STRING*/
  lpa_util_device_info_type device_info;                  /*TAG_SEQUENCE*/
  lpa_smdp_id_type          smdpoid_string;               /*TAG_OBJECT_IDENTIFIER*/
  lpa_data_type             hashed_confirmation_code_raw; /*TAG_OCTET*/
  lpa_data_type             cert_format_to_be_used_raw;   /*NOTAG CHOICE*/
  lpa_data_type             curve_to_be_used_raw;         /*TAG_OCTET*/
  lpa_data_type             smdp_certificate_raw;         /*TAG_SEQUENCE*/
}lpa_asn1_prepare_download_request_type;

/* ----------------------------------------------------------------------------
   STRUCTURE:      LPA_ASN1_SET_NICKNAME_REQUEST_TYPE

   DESCRIPTION:
     This structure specifies the LPA slot IDs
-----------------------------------------------------------------------------*/
typedef struct
{
  lpa_iccid_type            iccid;
  lpa_name_type             profile_nickname;
} lpa_asn1_set_nickname_request_type;

/*=============================================================================

                        FUNCTIONS PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION LPA_ASN1_ENCODE_PREPARE_DOWNLOAD_REQUEST

DESCRIPTION
  This function encodes PrepareDownloadRequest

DEPENDENCIES
  Caller is responsible to free the the data pointer inside of
  prepare_download_encoded_data_buff_ptr

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_encode_prepare_download_request(
  lpa_asn1_prepare_download_request_type prepare_download_request,
  lpa_data_type*                         prepare_download_encoded_data_buff_ptr
);

/*===========================================================================
FUNCTION LPA_ASN1_ENCODE_SET_NICKNAME_REQUEST

DESCRIPTION
  This function encodes the SetNicknameRequest

DEPENDENCIES
  Caller is responsible to free the the data pointer inside of
  set_nickname_request_encoded_data_buff_ptr

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_encode_set_nickname_request(
  lpa_asn1_set_nickname_request_type set_nickname_request,
  lpa_data_type*                     set_nickname_request_encoded_data_buff_ptr
);

#endif /* LPA_ASN1_ENCODER_H */
