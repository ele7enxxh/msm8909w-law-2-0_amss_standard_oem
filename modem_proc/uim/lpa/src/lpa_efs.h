#ifndef LPA_EFS_H
#define LPA_EFS_H
/*===========================================================================


        L P A   F I L E S Y S T E M   R E L A T E D   O P E R A T I O N S
                              H E A D E R S


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_efs.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
-------------------------------------------------------------------------------
04/11/16    av     Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "fs_public.h"
#include "lpalib.h"
#include "uim_common_efs.h"

/*=============================================================================

                       DATA DECLARATIONS

=============================================================================*/

#define LPA_DIRECTORY                          "/lpa"
#define LPA_FILE_JSON_ENCODED_INIT_AUTH        "/lpa/json_enc_init_auth"
#define LPA_FILE_JSON_ENCODED_GET_BPP          "/lpa/json_enc_get_bpp"
#define LPA_FILE_JSON_ENCODED_HANDLE_INST_RSLT "/lpa/json_enc_handle_inst_rslt"
#define LPA_FILE_JSON_INIT_AUTH_FROM_DS        "/lpa/json_init_auth_from_ds"
#define LPA_FILE_JSON_GET_BPP_FROM_DS          "/lpa/json_get_bpp_from_ds"
#define LPA_FILE_ASN1_ENCODED_PREP_DOWNLOAD    "/lpa/asn1_enc_prep_download"
#define LPA_FILE_PREP_DOWNLOAD_RESP_FROM_CARD  "/lpa/prep_download_resp_from_card"
#define LPA_FILE_EUICC_INFO_FROM_CARD          "/lpa/euicc_info_from_card"
#define LPA_FILE_LOAD_BPP_FROM_CARD            "/lpa/load_bpp_from_card"
#define LPA_FILE_JSON_DECODED_GET_BPP          "/lpa/json_dec_get_bpp"
#define LPA_FILE_EUICC_CHAL_FROM_CARD          "/lpa/euicc_chal_from_card"
#define LPA_FILE_ACTIVATION_CODE_FROM_USER     "/lpa/activation_code_from_user"
#define LPA_FILE_STORE_DATA_RESP_FROM_CARD     "/lpa/store_data_resp_from_card"
#define LPA_FILE_ASN1_ENCODED_NICKNAME         "/lpa/asn1_enc_nickname"
/*===========================================================================

                        FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION LPA_EFS_INIT

DESCRIPTION
  Function creates the LPA EFS directory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lpa_efs_init (
  void
);

/*===========================================================================
FUNCTION LPA_EFS_CLEAN

DESCRIPTION
  Deletes all the LPA EFS files

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type:
           LPA_SUCCESS:          File was written
           LPA_GENERIC_ERROR:    Failure occurred

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_efs_cleanup(
  lpa_slot_id_enum_type        slot_id
);

/*===========================================================================
FUNCTION LPA_EFS_WRITE

DESCRIPTION
  Generic Function called by LPA to write to the file in EFS. If the file does
  note exist, it creates it. If the file exists, it furst truncates it and
  then writes to it.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type:
           LPA_SUCCESS:          File was written
           LPA_GENERIC_ERROR:    Failure occurred

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_efs_write(
  const char                  *file_path_ptr,
  uint32                       data_buffer_len,
  uint8                       *data_buffer_ptr,
  lpa_slot_id_enum_type        slot_id
);

#endif /* LPA_EFS_H */

