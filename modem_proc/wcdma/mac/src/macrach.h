#ifndef MACRACH_H
#define MACRACH_H
/*=====================================================================

    R A C H    C O N F I G U R A T I O N    H E A D E R   F I L E
                
GENERAL DESCRIPTION
  
  This module contains the definition of the data types and functions 
  declared in "macrachcfg.c".
  
Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2007-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*/

/*=====================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mac/vcs/macrach.h_v   1.3   19 Jun 2001 20:04:00   psuvarna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/macrach.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     --------------------------------------------------------------------------
05/20/15   as      Enable RxD after back-off timer expired and turn on RxD after persistency passed
12/15/14   as       In FACH state MAC should send CMAC_STATUS_IND only for TM CCCH UL data.
05/15/14   kc      Changes for enabling RxD for FACH Signaling when HS-RACH is configured
10/29/13   ts      Added API mac_fetch_random_number() which calls secapi_get_random() having very good cryptographic properties
07/16/12   mrg     MAC_I & HS-RACH Compilation Warnings fixed
08/13/12   mrg     HS-RACH & MAC-I Feature updated code check-in.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/29/12   mrg     HS-RACH Feature latest changes
06/19/12   mrg     HS-RACH & MAC-I Feature code integrated
06/02/11   kc      Added support for logical channel to transport channel mapping info
05/07/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
09/26/07   mrg     Added get_rach_cfg_ptr function support for ATF under FEATURE_CCCH_IE_ENH  
                   which is called by eliminate_tfc_based_on_bo() 
06/19/01   ps      Converted UL TX Status and Build Frame Indications from commands to signals
04/04/01   ps      Initial release after merging "macrachcfg.h" and "mactxstatus.h"

=====================================================================*/

#include "macrrcif.h"

#ifdef FEATURE_WCDMA_HS_RACH
#include "l1macdata.h"
#endif /*FEATURE_WCDMA_HS_RACH*/


#ifdef FEATURE_WCDMA_HS_RACH

typedef enum
{
 MAC_HS_RACH_COLLISION_RESOLUTION_PHASE = 0,
 MAC_HS_RACH_COLLISION_RESOLVED_PHASE,
 MAC_HS_RACH_COLLISION_INVALID_PHASE
}mac_hsrach_collision_phase_e_type;


typedef struct
{
  boolean  ernti_hdr_included;
  boolean  tebs0_si_only_triggered;
  boolean  hsrach_dxch_tebs0_flag;
  boolean  lc_include_in_si_current_tti;
}mac_hs_rach_etfci_common_info_type;


typedef struct
{

  /*additional-E-DCH-TransmitBackoff   INTEGER   (0 .  . 15 )*/
  uint8                                add_e_dch_tx_backoff_config_value;
  
  /*Maximum E-DCH resource allocation for CCCH, values 8, 12, 16, 24, 32, 40, 80, 120 In terms of TTIs*/
  boolean                             ccch_timer_configuerd_flag;
  l1_mac_hsrach_eul_timer_status_enum ccch_timer_status;
  uint8                               max_e_dch_res_alloc_ccch_config_value;
  uint8                               max_e_dch_res_alloc_ccch_current_count;


  /*Maximum period for collision resolution phase, Integer (8..24) In terms of TTIs.*/
  boolean                             agch_coll_resol_configuerd_flag;
  l1_mac_hsrach_eul_timer_status_enum agch_coll_resol_timer_status;
  uint8                               agch_max_collision_resol_config_value;
  uint8                               agch_max_collision_resol_current_count;
 
  /*E-DCH transmission continuation back off, value Enumerated (0, 8, 16, 24, 40, 80, 120, infinity)
  *In terms of TTIs. If set to "infinity", implicit common E-DCH resource release is disabled.
  */
  boolean                              tebs_zero_timer_configuerd_flag;
  l1_mac_hsrach_eul_timer_status_enum  tebs_zero_timer_status;
  l1_e_dch_tx_cont_backoff_enum_type   edch_tx_tebs_zero_timer_config_value; 
  uint8                                edch_tx_tebs_zero_timer_current_count;
  
} mac_hs_ul_hsach_timers_type;

typedef struct
{
  boolean                           hs_rach_active;
  boolean                           data_present_on_srb;
  uint8                             hs_rach_ccch_asc;  
  boolean                           primary_e_rnti_present;
  uint16                            primary_e_rnti;
  uint16                            backoff_val;
  uint8                             t2_timer_count;
  uint32                            ccch_buff_status;/* Total CCCH Buffer Status in Bytes*/
  uint32                            dxch_buff_status;/* Total CCCH Buffer Status in Bytes*/
  
  mac_ul_rach_txcntl_config_type      hs_rach_cntl_info;
  
  mac_hsrach_collision_phase_e_type   mac_hsrach_collision_phase_info;
  
  l1_mac_hs_rach_trans_type_enum_type  mac_hsrach_logch_tx_type;
  
  rlc_lc_id_type                       mac_hsrach_ccch_rlc_id;

  mac_hs_ul_hsach_timers_type          mac_hsrach_timers_info;
  
  mac_hs_rach_etfci_common_info_type   mac_hsrach_etfci_common_info;

  boolean                                     mac_hsrach_edch_rsrc_rel_cause_valid;
  l1_mac_hs_rach_edch_rsrc_rel_cause_enum_type mac_hsrach_edch_rsrc_rel_cause;  
  
}mac_ul_hs_rach_cfg_info_type;


extern  mac_ul_hs_rach_cfg_info_type  mac_hs_rach_info;

extern rach_state_e_type hs_rach_state;

#endif /*FEATURE_WCDMA_HS_RACH*/


/*=====================================================================
FUNCTION NAME	proc_rach_cctrch_cfg

PURPOSE			Processes the RACH type CCTrCh Config data
    
PARAMETERS		rachptr : Pointer to the RACH configuration data

DESCRIPTION		If CCCH is mapped to RACH, this function calls the
				procedure that processes the CCCH config data.
=====================================================================*/
void	proc_rach_cctrch_cfg 
(
	/* Pointer to RACH configuration data */
	mac_ul_rach_cctrch_config_type  *rachptr
);

/*==============================================================================================
FUNCTION NAME	build_rach_tblks

DESCRIPTION		This function builds RACH Transport Blocks
				
===============================================================================================*/
void	build_rach_tblks
(
 	/* Pointer to Build Frame Indication Parameters */
	mac_ul_build_frame_ind_type		*bld_frame_ind_ptr

);

/*===============================================================================================
FUNCTION NAME	proc_rach_tx_parameters

DESCRIPTION		Processes RACH Transmit Control Parameters
===============================================================================================*/
void	proc_rach_tx_parameters 
(
	mac_ul_rach_txcntl_config_type		*rach_tx_prmtrs
);


/*===============================================================================================
FUNCTION NAME	get_rach_cfg_ptr

DESCRIPTION	This function returns rach_cfg_ptr
===============================================================================================*/
mac_ul_rach_cctrch_config_type *get_rach_cfg_ptr
(
  void
);

/*===============================================================================================
FUNCTION NAME mac_free_tx_dsm_items

DESCRIPTION   Frees Transmit DSM items
===============================================================================================*/
void  mac_free_tx_dsm_items
  (
/* Transmit Data Buffer Pointer */
  l1_ul_tx_data_type    *tx_buf_ptr
  );

/*==============================================================================================
 FUNCTION NAME  l1_ul_trigger_tx_req

 DESCRIPTION  This function triggers L1 to enable / disable PRACH
===============================================================================================*/
void  l1_ul_trigger_tx_req
  (
/* L1 TX Trigger Request Type */
  l1_ul_phychan_ctrl_type   action
  );

/*===============================================================================================
FUNCTION NAME mac_reset_rach

DESCRIPTION   Resets RACH state
===============================================================================================*/
void  mac_reset_rach
  (
  void
  );

/*===============================================================================================
FUNCTION NAME mac_send_tx_status_ind_to_rrc

DESCRIPTION   Sends CMAC_STATUS_IND to RRC
===============================================================================================*/
void  mac_send_tx_status_ind_to_rrc
  (
  mac_status_ind_e_type status
  );

/*===============================================================================================
FUNCTION NAME pick_mlp

DESCRIPTION   Selects the maximum MAC logical channel priority.

RETURNS     1 - 8 Depending on the selected MLP
===============================================================================================*/
uint8 pick_mlp(rlc_ul_frame_format_type *rlc_chan_info_ptr);

/*===============================================================================================
FUNCTION NAME persistence_is_ok

DESCRIPTION   Checks to see if the persistence is OK or not

RETURNS     TRUE  if the persistence is OK FALSE otherwise

===============================================================================================*/
boolean   persistence_is_ok ( void );

/*================================================================================================
FUNCTION NAME rach_tx_control

DESCRIPTION   This function controls RACH transmission and is called once every TTI
================================================================================================*/
boolean   rach_tx_control
  (
  void
  );

/*===============================================================================================
FUNCTION NAME mac_ul_check_rach_trch

DESCRIPTION   This function checks if the input rlc id and rb id pair map to a RACH transport channel
===============================================================================================*/

log_wcdma_trch_type mac_ul_check_rach_trch
(
   /* RLC logical channel buffer identifier (0-20) */
   rlc_lc_id_type rlc_id, 

   /* Radio Bearer Identifier (0-31) */
	rb_id_type rb_id
);

/*===============================================================================================
FUNCTION NAME mac_fetch_random_number

DESCRIPTION Based on featurization, it calls ran_next() or secapi_get_random()
            secapi_get_random() has very good cryptographic properties

RETURNS     uint32 Random Number value
===============================================================================================*/
uint32 mac_fetch_random_number(void);

#ifdef FEATURE_WCDMA_HS_RACH
extern uint8  mac_hsrach_pick_mlp(void);
boolean   hs_rach_tx_control(void);
boolean  mac_hsrach_buffer_status(boolean *data_present_on_srb);
void proc_hs_rach_cfg(mac_ul_config_type *cfgptr);
#endif /* FEATURE_WCDMA_HS_RACH */
void  mac_ul_trigger_tx_req
  (
#ifdef FEATURE_DSM_WM_CB
      dsm_watermark_type* wm_ptr,
      void*               func_data
#else
      void
#endif
  );
#endif

