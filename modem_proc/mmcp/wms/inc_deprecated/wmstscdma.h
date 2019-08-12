#ifndef WMSTSCDMA_H
#define WMSTSCDMA_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmstscdma.h -- CDMA Message Encoding and Decoding Services

  This file contains the encoding and decoding functions of CDMA SMS
  messages.

  -------------

    Copyright (c) 1998-2008,2010-2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/inc_deprecated/wmstscdma.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#ifdef FEATURE_CDSMS

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "wms.h"
#include "wmspriv.h"

/*===========================================================================
                        FUNCTIONS
===========================================================================*/
/*=========================================================================
FUNCTION wms_ts_convert_cl2tl

DESCRIPTION
  This function converts the SMS TL data from the client structure
  to the internal structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_ts_convert_cl2tl(
  const wms_cdma_message_s_type      *cl_ptr,    /* IN */
  wms_tl_message_type                *tl_ptr     /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_convert_tl2cl

DESCRIPTION
  This function converts the SMS TL data from the internal structure
  to the client structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void wms_ts_convert_tl2cl(
  const wms_tl_message_type          *tl_ptr,   /* IN */
  wms_cdma_message_s_type            *cl_ptr    /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_encode_CDMA_tl

DESCRIPTION
  This function encodes the SMS TL fields in CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_encode_CDMA_tl(
  const wms_tl_message_type          *tl_msg_ptr,
  const wms_raw_ts_data_s_type       *raw_bd_ptr,
  wms_OTA_message_type               *OTA_msg_ptr   /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_decode_CDMA_tl

DESCRIPTION
  This function decodes the SMS TL fields in CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_CDMA_tl(
  const wms_OTA_message_type         *OTA_msg_ptr,   /* IN */
  wms_tl_message_type                *tl_ptr,        /* OUT */
  wms_raw_ts_data_s_type             *raw_bd_ptr     /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_decode_AWISMS_tl

DESCRIPTION
  This function decodes the AWI SMS TL fields in Analog mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_AWISMS_tl(
  const wms_OTA_message_type         *OTA_msg_ptr,
  wms_tl_message_type                *tl_msg_ptr,      /* OUT */
  wms_raw_ts_data_s_type             *raw_bd_ptr       /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_decode_CDMA_OTA

DESCRIPTION
  This function decodes the raw CDMA message into the internal structure.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_CDMA_OTA(
  const wms_OTA_message_type         *OTA_msg_ptr,   /* IN */
  wms_tl_message_type                *tl_msg_ptr     /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_encode_CDMA_OTA

DESCRIPTION
  This function encodes the SMS message from the internal structure to
  the OTA format in CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type  wms_ts_encode_CDMA_OTA(
  const wms_tl_message_type          * tl_msg_ptr,
  wms_OTA_message_type               * OTA_msg_ptr   /* IN-OUT */
);

/*=========================================================================
FUNCTION wms_ts_encode_CDMA_bd

DESCRIPTION
  This function encodes the bearer data from the client structure to
  its raw format in CDMA mode.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_encode_CDMA_bd(
  const wms_client_bd_s_type         *cl_bd_ptr,       /* IN */
  wms_raw_ts_data_s_type             *raw_bd_ptr       /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_decode_CDMA_bd

DESCRIPTION
  This function decodes the raw CDMA bearer data into the client's structure.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation.

SIDE EFFECTS
  None

COMMENTS
  If is_unpacked_user_data = TRUE, then there should be no UDH.

=========================================================================*/
wms_status_e_type  wms_ts_decode_CDMA_bd(
  const wms_raw_ts_data_s_type       *raw_bd_ptr,   /* IN */
  boolean                             is_unpacked_user_data, /* IN */
  boolean                             decode_other,    /* IN */
  wms_client_bd_s_type               *cl_bd_ptr     /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_decode_OTA

DESCRIPTION
  This function decodes the SMS message from the raw over-the-air structure
  to the internal message structure.

DEPENDENCIES
  None

RETURN VALUE
  Status of the translation.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_decode_OTA(
  const wms_OTA_message_type         *OTA_msg_ptr,
  wms_tl_message_type                *tl_msg_ptr  /* OUT */
);

/*=========================================================================
FUNCTION wms_ts_cdma_decode_smsp

DESCRIPTION
  This function decodes the SMS Parameters for CDMA

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_decode_smsp(
  const uint8                        *data,
  uint8                               rec_len,
  wms_cdma_template_s_type           *tptr
);

/*=========================================================================
FUNCTION wms_ts_cdma_encode_smsp

DESCRIPTION
  This function encodes the SMS Parameters for CDMA

DEPENDENCIES
  None

RETURN VALUE
  TRUE / FALSE

SIDE EFFECTS
  None

=========================================================================*/
boolean wms_ts_cdma_encode_smsp(
  const wms_cdma_template_s_type     *tptr,
  uint8                               rec_len,
  uint8                              *data
);

/*=========================================================================
FUNCTION wms_ts_validate_bd_subparms

DESCRIPTION
  This function checks whether all the mandatory fields of a given teleservice
  is present in the bearer data

DEPENDENCIES
  None

RETURN VALUE
  Validation result. WMS_OK_S means all the mandatory sub parms are present.

SIDE EFFECTS
  None

=========================================================================*/
wms_status_e_type wms_ts_validate_bd_subparms(
  const wms_client_bd_s_type         *cl_bd_ptr,     /* IN */
  const wms_teleservice_e_type        ts_type        /* IN */
);

#ifdef __cplusplus
}
#endif

#endif /* FEATURE_CDSMS */

#endif /* WMSTSCDMA_H */
