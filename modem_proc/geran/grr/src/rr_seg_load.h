/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            RR Segmented Loading

GENERAL DESCRIPTION
   This module contains definitions for use by the WCDMA, TD-SCDMA segmented
   loading feature.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated. All Rights Reserved.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/GSM/RR/vcs/rr_defs.h_v   1.20   17 Jul 2002 11:50:34   rodgerc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_seg_load.h#1 $   $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/04/13   rn      Initial Revision.
*============================================================================*/
#ifndef RR_SEG_LOAD_H
#define RR_SEG_LOAD_H

#include "sys_cnst.h"
#ifdef FEATURE_SEGMENT_LOADING
#include "ITDSCDMA.h"
#include "IWCDMA.h"
#endif /*FEATURE_SEGMENT_LOADING*/

typedef enum
{
  RR_SEG_LOAD_WCDMA = 0,
  RR_SEG_LOAD_TDSCDMA = 1,
  RR_SEG_LOAD_NUM_RATS = 2
} rr_geran_seg_load_rat_t;

#ifdef FEATURE_SEGMENT_LOADING
typedef struct
{
  boolean rat_loaded;
  interface_t *rat_interface_ptr;
} rr_geran_seg_load_t;

extern rr_geran_seg_load_t *rr_seg_load_ptr;

#ifndef PERLUTF
/*All calls to TD-SCDMA functions require dynamic memory mapping. 
  Convert existing calls to TDS API to use the segmented loading API*/
#define tdsrrc_get_rr_cmd_buf() ITDSCDMA_tdsrrc_get_rr_cmd_buf(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr)
#define tdsrrc_get_classmark_change_handover_info(msg_ptr, msg_len, irat_ho_info_compressed) \
        ITDSCDMA_tdsrrc_get_classmark_change_handover_info(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr, msg_ptr, msg_len, irat_ho_info_compressed)
#define tdsrrc_delete_classmark_change_handover_info() ITDSCDMA_tdsrrc_delete_classmark_change_handover_info(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr)
#define tdsrrc_put_rr_cmd(cmd_buf) ITDSCDMA_tdsrrc_put_rr_cmd(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr, cmd_buf)
#define tdsrrc_update_band_preference(band_pref_mask) ITDSCDMA_tdsrrc_update_band_preference(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr, band_pref_mask)
#define tdsrrc_is_arfcn_valid(UARFCN) ITDSCDMA_tdsrrc_is_arfcn_valid(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr, UARFCN)
#define tdsrrc_set_ue_in_deep_sleep(value) ITDSCDMA_tdsrrc_set_ue_in_deep_sleep(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr, value)
#define tdsrrc_check_cell_barred(req, cpid) ITDSCDMA_tdsrrc_check_cell_barred(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr, req, cpid)
#define tds_rrc_rat_is_supported(camped_plmn_id) ITDSCDMA_tds_rrc_rat_is_supported(rr_seg_load_ptr[RR_SEG_LOAD_TDSCDMA].rat_interface_ptr, camped_plmn_id)

/*All calls to WCDMA functions require dynamic memory mapping. 
  Convert existing calls to WCDMA API to use the segmented loading API*/
#define rrc_get_classmark_change_handover_info(msg_ptr,msg_len,irat_ho_info_compressed) \
        IWCDMA_rrc_get_classmark_change_handover_info(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,msg_ptr,msg_len,irat_ho_info_compressed)
#define rrc_delete_classmark_change_handover_info() IWCDMA_rrc_delete_classmark_change_handover_info(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr)
#define rrc_get_rr_cmd_buf() IWCDMA_rrc_get_rr_cmd_buf(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr)
#define rrc_put_rr_cmd(cmd_buf) IWCDMA_rrc_put_rr_cmd(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,cmd_buf)
#define rrc_update_band_preference(band_pref_mask) IWCDMA_rrc_update_band_preference(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,band_pref_mask)
#define rrc_is_arfcn_valid(UARFCN,plmn_id) IWCDMA_rrc_is_arfcn_valid(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,UARFCN,plmn_id)
#define rrc_set_ue_in_deep_sleep(value) IWCDMA_rrc_set_ue_in_deep_sleep(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,value)
#define rrc_rr_check_cell_barred(freq,scr_code) IWCDMA_rrc_rr_check_cell_barred(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,freq,scr_code)
#define rrc_clear_acq_chnl_info() IWCDMA_rrc_clear_acq_chnl_info(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr)
#define rrc_clear_acq_chnl_info_sub_id(as_id) IWCDMA_rrc_clear_acq_chnl_info_sub_id(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,as_id)
#define rr_get_rrc_acq_chnl_info(acq_chnl_info) IWCDMA_rr_get_rrc_acq_chnl_info(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,acq_chnl_info)
#define rr_get_rrc_acq_chnl_info_sub_id(acq_chnl_info,as_id) \
        IWCDMA_rr_get_rrc_acq_chnl_info_sub_id(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,acq_chnl_info,as_id)
#define rrc_rr_check_cell_barred_sub_id(freq,scr_code,as_id) \
        IWCDMA_rrc_rr_check_cell_barred_sub_id(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,freq,scr_code,as_id)
#define rrc_set_ue_in_deep_sleep_sub_id(value,as_id) IWCDMA_rrc_set_ue_in_deep_sleep_sub_id(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,value,as_id)
#define rrc_is_arfcn_valid_sub_id(UARFCN,plmn_id,as_id) IWCDMA_rrc_is_arfcn_valid_sub_id(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,UARFCN,plmn_id,as_id)
#define rrc_update_band_preference_sub_id(band_pref_mask,as_id) \
        IWCDMA_rrc_update_band_preference_sub_id(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,band_pref_mask,as_id)
#define rrc_delete_classmark_change_handover_info_sub_id(as_id) \
        IWCDMA_rrc_delete_classmark_change_handover_info_sub_id(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,as_id)
#define rrc_get_classmark_change_handover_info_sub_id(msg_ptr,msg_len,irat_ho_info_compressed,as_id) \
        IWCDMA_rrc_get_classmark_change_handover_info_sub_id(rr_seg_load_ptr[RR_SEG_LOAD_WCDMA].rat_interface_ptr,msg_ptr,msg_len,irat_ho_info_compressed,as_id)
#endif /*PERLUTF*/

/*===========================================================================

FUNCTION rr_initialise_seg_load

DESCRIPTION
  Populates the segmented loading database for all RAT's that are segment
  loaded.
 
PARAMS
  none

RETURN VALUE
  none

===========================================================================*/
void rr_initialise_seg_load(void);
#endif /*FEATURE_SEGMENT_LOADING*/

/*===========================================================================

FUNCTION rr_is_rat_segment_loaded

DESCRIPTION
  Query if a RAT is segment loaded.
 
PARAMS
  rat

RETURN VALUE
  boolean - TRUE - RAT loaded, FALSE - RAT not loaded

===========================================================================*/
boolean rr_is_rat_segment_loaded(rr_geran_seg_load_rat_t rat);

#endif /* #ifndef RR_SEG_LOAD_H */

