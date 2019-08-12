#ifndef TDS_RRC_API_H
#define TDS_RRC_API_H


/*===========================================================================
                      RRC External APIs

DESCRIPTION

  This module contains protoypes of APIs that WCDMA RRC provides to external modules



Copyright (c) 2000-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tds_rrc_api.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
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
sys_band_mask_type tds_rrc_get_band_from_dl_uarfcn
(
  uint16 uarfcn,
  sys_plmn_id_s_type camped_plmn_id,
  boolean *is_band_valid
);

#if defined (FEATURE_TDSCDMA_TO_LTE) || defined (FEATURE_LTE_TO_TDSCDMA)
/*===========================================================================

FUNCTION          tds_rrc_derive_freq_from_uarfcn

DESCRIPTION       This function converts the given UARFCN into absolute frequency in multiple of 100KHZ.

DEPENDENCIES      NONE

RETURN VALUE      E_SUCCESS if conversion is succesful
                            E_FAILURE if conversion is not succesful
                  
SIDE EFFECTS      NONE

===========================================================================*/
errno_enum_type tds_rrc_derive_freq_from_uarfcn
(
  uint16 uarfcn,
  uint16 *freq
);
#endif

/*===========================================================================

FUNCTION          TDS_RRC_RAT_IS_SUPPORTED

DESCRIPTION       This function checks if a PLMN ID is supported by the TD-SCDMA carrier

DEPENDENCIES
                  None

RETURN VALUE
                  TRUE if the PLMN ID is supported.

SIDE EFFECTS
                  None
===========================================================================*/
boolean tds_rrc_rat_is_supported(sys_plmn_id_s_type  camped_plmn_id);

/*===========================================================================
FUNCTION   tdsrrc_vocoder_control_enabled()

DESCRIPTION

  Interface to let other procedures know that rrc is controlling the vocoder
  
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE if rrciho is controlling the vocoder,FALSE otherwise.

SIDE EFFECTS

  None.
 ===========================================================================*/
boolean tdsrrc_vocoder_control_enabled(void);

/*===========================================================================
FUNCTION   tdsrrc_vocoder_acquired()

DESCRIPTION

  This is a callback function for NAS when the mvs_acquire commmand issued
  by RRC was succesful
  
DEPENDENCIES

  None.
 
RETURN VALUE

  None

SIDE EFFECTS

  None.
 ===========================================================================*/
void tdsrrc_vocoder_acquired(void);

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION   tdsrrc_vocoder_control_enabled_sub()

DESCRIPTION

  Interface to let other procedures know that rrc is controlling the vocoder
  
DEPENDENCIES

  None.
 
RETURN VALUE

  TRUE if rrciho is controlling the vocoder,FALSE otherwise.

SIDE EFFECTS

  None.
 ===========================================================================*/
boolean tdsrrc_vocoder_control_enabled_sub(sys_modem_as_id_e_type as_id);
#endif
#endif /*WCDMA_RRC_API_H*/
