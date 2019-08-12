#ifndef MACDL_H
#define MACDL_H

/*=====================================================================

   D L   M A C   D A T A   I N D I C A T I O N   H E A D E R    F I L E

GENERAL DESCRIPTION

  This module contains the definition of the functions and data types
  declared in "macdldata.c".


Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002, 2004-2006, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=====================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macdl.h_v   1.2   12 Mar 2002 15:44:54   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macdl.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------
08/21/14   ts      FR22375: Fixed Medium compiler warnings in WCDMA on Bolt 2.0/2.107/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/19/12   mrg     L2 - MAC-I and HS-RACH code changes
01/02/12   grk     Feature cleanup
06/02/11   kc      Added support for logical channel to transport channel mapping info
03/18/11   grk     Fixed compiler warnings.
01/20/10   prk     Removing mac_gcf_test_flag
12/7/09    kvk     Fixed lint warning
11/30/09   prk     Function prototype of l1_phy_dl_query_allowed_buf_size_for_p_to_c
                   is added.
11/02/099  kvk     Moved l1_phy_dl_trblk_hs_data_ind() to l1macdata.h
10/28/09   kvk     Added support for A2 and seperated the MODEM ACCEL specific code into
                   FEATURE_WCDMA_DL_MODEM_ACC
10/28/09   prk     Fixed ARM compiler warning.
05/14/09   mrg     When Partial Q is empty and received TSN has Complete+Partial PDUs, then 
                   moving PC PDUs to RLC code logic now under GCF feature flag- 
                   To avoid 0x421E logging problems.
05/07/09   ssg     Updated Copyright Information
04/29/09    av     Fixed compiler warnings in L2 
10/17/08   grk     Merged taxis changes to mainline.
01/29/08    sk     Support for MBMS under FEATURE_MODEM_MBMS
10/16/06    sk     Fixed compiler warning
03/09/05    sk     Removed ## from the macro definitions/
12/30/04    sk     Fixed lint errors
11/15/02    sk     Corrected the macro DL_CT_FLD_IS_INVALID
3/12/02     ps     Fixed DL_TB_URNTI_IS_INVALID and DL_TB_CRNTI_IS_INVALID macro logic
4/3/01      ps     Initial release after merging "macdlconfig.h", "macdlhdr.h" and "macdata.h"
=====================================================================*/

/*===========================================================================================
                            MACRO DEFINITIONS
===========================================================================================*/

/* Condition for invalid DL RB ID */
#define     DL_RB_ID_IS_INVALID(rb_id)              (rb_id > MAC_DL_MAX_RB_ID)

/* Condition for invalid DL RLC ID */

#define     DL_RLC_ID_IS_INVALID(rlc_id)            (rlc_id > MAC_DL_MAX_RLC_ID)

/* Condition for invalid DL CCTrCh ID */
#define     DL_CCTRCH_ID_IS_INVALID(cctrch_id)      (cctrch_id > MAC_DL_MAX_CCTRCH_ID)

#include "wcdma_variation.h"
#include "stringl.h"

/* Condition for invalid DL Transport Channel ID */
#define     DL_TRCH_ID_IS_INVALID(trch_id)\
            ((trch_id < MAC_DL_MIN_TRCH_ID) || (trch_id > MAC_DL_MAX_TRCH_ID))

/* Condition for invalid DL RLC Mode */
#define     DL_RLC_MODE_IS_INVALID(rlc_mode)\
            (rlc_mode >= UE_MODE_MAX_NUM)

/* Condition for invalid DL MAC ID */
#define     DL_MAC_ID_IS_INVALID(mac_id)\
            ((mac_id < MAC_MIN_MAC_ID ) || (mac_id > MAC_MAX_MAC_ID))

/* Condition for invalid C/T field */
#define     DL_CT_FLD_IS_INVALID(num_dlc,ct_fld)\
            (ct_fld > MAC_MAX_MAC_ID - 1)

/* Pointer to DL Transport Block Info */
extern      dl_tb_info_type         dl_tb_info[];
#define     GET_DL_TB_INFO_PTR(tb_index)    &dl_tb_info[tb_index]

/* Pointer to internal BCH config info */
extern      bch_config_info_type        bch_cfg_info[];
#define     GET_BCH_INFO_PTR(cctrch_id)\
            &bch_cfg_info[cctrch_id - MAC_DL_MIN_CCTRCH_ID]

/* Pointer to internal PCH config info */
extern      pch_config_info_type        pch_cfg_info[];
#define     GET_PCH_INFO_PTR(cctrch_id)\
            &pch_cfg_info[cctrch_id - MAC_DL_MIN_CCTRCH_ID]

/* Pointer to internal DL DCH config info */
extern      dl_dch_config_info_type     dl_dch_cfg_info[UE_MAX_TRCH];
#define     GET_DL_DCH_INFO_PTR(trch_id)\
            &dl_dch_cfg_info[dl_dch_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID].idx]

/* Pointer to internal DSCH config info */
extern      dsch_config_info_type       dsch_cfg_info[UE_MAX_TRCH];
#define     GET_DSCH_INFO_PTR(trch_id)\
            &dsch_cfg_info[dsch_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID].idx]

/* Pointer to internal FACH config info */
extern      fach_config_info_type       fach_cfg_info[UE_MAX_TRCH];
#define     GET_FACH_INFO_PTR(trch_id)\
            &fach_cfg_info[fach_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID].idx]

/* Transport Channel ID to Index Mapping Table */
extern      trch_id_to_idx_map_type     dl_dch_id_to_idx[MAX_TRCH];
extern      trch_id_to_idx_map_type     dsch_id_to_idx[MAX_TRCH];
extern      trch_id_to_idx_map_type     fach_id_to_idx[2*MAX_TRCH];

/* Condition for enabled DL DCH */
#define     DL_DCH_IS_ENABLED(trch_id)\
            dl_dch_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID].valid

/* DL DCH ID to IDX Map Pointer */
#define     DL_DCH_ID_TO_IDX_PTR(trch_id)\
            &dl_dch_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID]

/* Condition for enabled DSCH */
#define     DSCH_IS_ENABLED(trch_id)\
            dsch_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID].valid

/* DSCH ID to IDX Map Pointer */
#define     DSCH_ID_TO_IDX_PTR(trch_id)\
            &dsch_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID]

/* Condition for enabled FACH */
#define     FACH_IS_ENABLED(trch_id)\
            fach_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID].valid

/* FACH ID to IDX Map Pointer */
#define     FACH_ID_TO_IDX_PTR(trch_id)\
            &fach_id_to_idx[trch_id - MAC_DL_MIN_TRCH_ID]

/* TRUE indicates that DL MAC Ciphering is enabled, FALSE otherwise */
extern  boolean     dl_cipher_enable;

/* DL MAC Ciphering Key ID */
extern  uint8       dl_cipher_key_id;

/* DL MAC HFN value */
extern  uint32      dl_hfn;

/* TRUE indicates that DL MAC CRNTI is valid */
/* FALSE otherwise */
extern  boolean     dl_mac_crnti_valid;

/* TRUE indicates that DL MAC URNTI is valid */
/* FALSE otherwise */
extern  boolean     dl_mac_urnti_valid;

/* DL MAC CRNTI value */
extern  uint16      dl_mac_crnti;

/* DL MAC URNTI value */
extern  uint32      dl_mac_urnti;

#define     DL_TB_CRNTI_IS_INVALID(crnti)\
    (!(dl_mac_crnti_valid) || (crnti != dl_mac_crnti))

#define     DL_TB_URNTI_IS_INVALID(urnti)\
    (!(dl_mac_urnti_valid) || (urnti != dl_mac_urnti))


/*=============================================================================================
FUNCTION NAME   config_dl_mac

DESCRIPTION     This function processes DL MAC Configaration Data received from RRC

=============================================================================================*/
void    config_dl_mac
(
    /* Pointer to DL MAC Config Data */
    mac_dl_config_type *cfgptr
);

#if defined (FEATURE_HSDPA_MAC_EHS) && defined (FEATURE_WCDMA_REL7) 
/*===========================================================================================
 FUNCTION NAME  l1_phy_dl_query_allowed_buf_size_for_p_to_c

 DESCRIPTION  This function returns the buffersize used in L1 for 
              Partial to Complete PDU preparation

============================================================================================*/
uint16 l1_phy_dl_query_allowed_buf_size_for_p_to_c(void);
#endif 

/*===========================================================================================
 FUNCTION NAME  proc_dl_cipher_info

 DESCRIPTION  This function processes the Cipher information received from RRC.

============================================================================================*/
void    proc_dl_cipher_info
  (
// Pointer to DL MAC Ciphering Config Info
  mac_cipher_config_type   cipherptr[]
  );

/*============================================================================================
 FUNCTION NAME  proc_dl_rnti_info

 DESCRIPTION  This function processes the RNTI information received from RRC.

============================================================================================*/
void    proc_dl_rnti_info
  (
/* Pointer to DL RNTI Config Info */
  mac_rnti_config_type   *rntiptr
  );

uint32 mac_copy_to_word(uint8 *src_ptr);

/*===============================================================================================
FUNCTION NAME mac_dl_get_trch_type

DESCRIPTION   This function returns the down-link transport channel type corresponding to given RLC & RB IDs
===============================================================================================*/

log_wcdma_trch_type mac_dl_get_trch_type
  (
   /* RLC logical channel buffer identifier (0-20) */
   rlc_lc_id_type rlc_id, 

   /* Radio Bearer Identifier (0-31) */
   rb_id_type rb_id
  );


#ifdef FEATURE_WCDMA_HS_RACH
extern  void  mac_hsrach_stop_tebs0_timer_dl_data_recevied(void);
#endif /* FEATURE_WCDMA_HS_RACH */

#endif

