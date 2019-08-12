#ifndef LPA_ASN1_DECODER_H
#define LPA_ASN1_DECODER_H
/*===========================================================================


            L P A   A S N 1   D E C O D E R S


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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/lpa/src/lpa_asn1_decoder.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/16   ll      Peek into profile installation result
03/28/16   av      Initial revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "uim_variation.h"

#include "lpa.h"
#include "lpa_util.h"
#include "mmgsdilib_common.h"

/*=============================================================================

                     FUNCTION DECLARATIONS

=============================================================================*/
/*===========================================================================
FUNCTION LPA_ASN1_DECODE_PROFILE_INSTALLATION_RESULT

DESCRIPTION
  Function parses the ASN1 formatted APDU response received as a result of
  LoadBoundProfilePackage or GetProfileInstallationResult request.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_decode_profile_installation_result (
  uint32                                               data_len,
  uint8                                               *data_ptr,
  lpa_lpd_profile_installation_result_code_array_type  result_code_array
);

/*===========================================================================
FUNCTION LPA_ASN1_DECODE_GET_PROFILES_INFO_APDU

DESCRIPTION
  Function parses the ASN formatted APDU response received as a result of
  Get Profiles Info request. This function might MALLOC
  resp_ptr->message.get_profiles_info_resp.profile_info[p].icon.data_ptr
  (where p ranges from 0 to LPA_NUM_PROFILES_MAX) which should be freed by
  the caller.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_decode_get_profiles_info_apdu (
  uint32                                 data_len,
  uint8                                 *data_ptr,
  lpa_response_data_type                *resp_ptr
);

/*===========================================================================
FUNCTION LPA_ASN1_DECODE_EUICC_INFO_APDU

DESCRIPTION
  Function parses the ASN formatted APDU response of Get EUICC Info.
  This function might MALLOC resp_ptr->euicc_info.data_ptr which should
  be freed by the caller.

DEPENDENCIES
  None

RETURN VALUE
  lpa_result_enum_type

SIDE EFFECTS
  None
===========================================================================*/
lpa_result_enum_type lpa_asn1_decode_euicc_info_apdu (
  uint32                                 data_len,
  uint8                                 *data_ptr,
  lpa_lpd_get_euicc_info_resp_type      *resp_ptr
);

#endif /* LPA_ASN1_DECODER_H */

