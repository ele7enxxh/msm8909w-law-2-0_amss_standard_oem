#ifndef WCDMA_RRC_API_H
#define WCDMA_RRC_API_H


/*===========================================================================
                      RRC External APIs

DESCRIPTION

  This module contains protoypes of APIs that WCDMA RRC provides to external modules



Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/wcdma_rrc_api.h#1 $    $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/18/14   bc      Made changes to create and expose wcdma_mfbi_is_supported outside of RRC
12/30/13   bc      Made changes to optimize REL 10 Spec CRs code
11/18/13   bc      Checked in REL 10 spec crs changes.
10/11/13   gv      Exposed the API rrccsp_derive_freq_from_uarfcn outside of WRRC
02/11/09   rm      Added API to return W band
===========================================================================*/

#include <sys_plmn_selection.h>


/*===========================================================================

FUNCTION          WCDMA_RRC_GET_BAND_FROM_DL_ARFCN

DESCRIPTION       This function is returns W band corresponding to DL UARFCN and PLMN ID

DEPENDENCIES
                  None

RETURN VALUE
                  None

SIDE EFFECTS
                  None
===========================================================================*/
sys_band_mask_type wcdma_rrc_get_band_from_dl_uarfcn
(
  uint16 uarfcn,
  sys_plmn_id_s_type camped_plmn_id,
  boolean *is_band_valid
);

/*===========================================================================

FUNCTION          WCDMA_RRC_GET_WCSG_LFP_INFO

DESCRIPTION       Gives the list of FPed W CSG cells to passed L macro

DEPENDENCIES      NONE

RETURN VALUE      Number of CSG cells FPed
                  
SIDE EFFECTS      NONE

===========================================================================*/
uint8 wcdma_rrc_get_wcsg_lfp_info
(
   const source_cell_info_type *l_macro_ptr, 
   source_cell_info_type *w_csg_cells_ptr
);
/*===========================================================================

FUNCTION          wcdma_derive_freq_from_uarfcn

DESCRIPTION       This function converts the given UARFCN into absolute frequency.

DEPENDENCIES      NONE

RETURN VALUE      E_SUCCESS if conversion is succesful
                  E_FAILURE if conversion is not succesful
                  
SIDE EFFECTS      NONE

===========================================================================*/
errno_enum_type wcdma_derive_freq_from_uarfcn
(
  uint16 uarfcn,
  sys_plmn_id_s_type camped_plmn_id,
  uint16 *freq
);
/*===========================================================================
FUNCTION          WCDMA_CONVERT_UARFCN

DESCRIPTION       This function converts the UARFCN to a UARFCN corresponding
                  to the given band.

DEPENDENCIES
                  None

RETURN VALUE
                  TRUE if the conversion is succesful
                  FALSE if the conversion is not succesful

SIDE EFFECTS
                  None
===========================================================================*/
boolean wcdma_convert_uarfcn
(
  uint16 source_uarfcn,
  sys_sband_e_type band_num,
  uint16 *target_uarfcn,
  sys_plmn_id_s_type camped_plmn_id
);
/*===========================================================================

FUNCTION          WCDMA_MFBI_IS_SUPPORTED

DESCRIPTION       This function indicates WCDMA MFBI Support by UE.

DEPENDENCIES      NONE

RETURN VALUE      TRUE if UE supports MFBI
                  FALSE if UE doesnt support MFBI.
                  
SIDE EFFECTS      NONE

===========================================================================*/
boolean wcdma_mfbi_is_supported(void);
#endif /*WCDMA_RRC_API_H*/
