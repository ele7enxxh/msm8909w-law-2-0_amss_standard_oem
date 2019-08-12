#ifndef TDSIRAT_H
#define TDSIRAT_H
/*===========================================================================

                    TDSCDMA IRAT INTERNAL DEFINES

GENERAL DESCRIPTION
  This file contains the IRAT internal type definitions for X to TDSCDMA.
 
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

 
  Copyright (c) 2000 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsirat.h#1 $ 
$DateTime: 2016/12/13 07:58:19 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/05/11   bw      Initial version 
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "tdsiratif.h"
#include "tdsl1utils.h"
#include "tfw_sw_intf_msg.h"
#ifdef FEATURE_TDSCDMA_RF_SUPPORT
#include "rf_tdscdma_msg.h"
#endif
#define TDSIRAT_MAX_JDS_CELLS     8 /* maximum number of JDS cells used for IRAT measurment */
/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef  enum
{
  TDSIRAT_INIT,
  TDSIRAT_ACQ,
  TDSIRAT_MEAS,
  TDSIRAT_L2T_ACQ,
  TDSIRAT_L2T_MEAS
}tdsirat_substate_enum_type;

typedef  enum
{
  TDSIRAT_STOP_INVALID,
  TDSIRAT_STOP_L2T,
  TDSIRAT_STOP_G2T
}tdsirat_stop_cmd_enum_type;

typedef struct
{
  /* current substate */
  tdsirat_substate_enum_type	substate;

  #ifdef FEATURE_TDSCDMA_DR_DSDS
  uint32 trm_modes;
  #endif

  /* gap start position */
  uint32 gap_start_subfn;
  uint32 gap_start_pos;
  uint32 gap_start_time;
  /* gap end position */
  uint32 gap_end_subfn;
  uint32 gap_end_pos;
  uint32 gap_end_time;
  /* gap length */
  uint32 gap_length;

  /* time for cmd send to FW */
  uint32 send_time;
  /* time for cmd received from GL1*/
  uint32 rcvd_time;

  uint32 cnf_time;

  /* gap has ended */
  boolean gap_ended;

  /* current freq index */
  uint8 freq_idx;

  /* stop message received */
  boolean stopping;

  /* abort message received */
  boolean aborting;

  /* RF meas in progress */
  boolean rf_meas_in_progress;

  boolean fw_cmd_sent;

  boolean gap_aborted;
  
  /* RF parameter */
  tdsirat_meas_rf_param_type rf_param;

  /* VCO and RGS */
  tcxomgr_vco_info_type     vco_info;

  /* meas frequency list */
  //tdsirat_freq_list_type meas_list;

  /* pending acq response */
  tdsirat_acq_rsp_type pending_acq_rsp;

  /* pending meas response */
  tdsirat_meas_rsp_type pending_meas_rsp;
  uint16 cpid_mMetric[TDSIRAT_MAX_JDS_CELLS];
  tdsirat_stop_cmd_enum_type stop_cmd_type;
}tdsirat_global_type;

extern tdsirat_global_type tdsirat;

typedef union
{
  msgr_hdr_struct_type hdr;
  /* IRAT msgs */
  tdsirat_init_cmd_type init_cmd;
  tdsirat_stop_cmd_type stop_cmd;
  tdsirat_startup_req_type startup_req;
  tdsirat_acq_req_type  acq_req;
  tdsirat_meas_req_type meas_req;
}tdsirat_msg_union_type;

/*===========================================================================
  FUNCTION     : tdsirat_init

  DESCRIPTION: This function will initilize TDSCDMA L1 IRAT state.
  
  DEPENDENCIES:
 
  INPUT PARAMETERS: void
    
  RETURN VALUE: void

  SIDE EFFECTS: None
===========================================================================*/
void tdsirat_init (void);

/*===========================================================================
  FUNCTION     : tdsirat_cleanup

  DESCRIPTION: This function will cleanup TDSCDMA L1 IRAT state.
  
  DEPENDENCIES:
 
  INPUT PARAMETERS: void
    
  RETURN VALUE: void

  SIDE EFFECTS: None
===========================================================================*/
boolean tdsirat_cleanup (void);

/*===========================================================================

FUNCTION  TDSIRAT_MSG_HANDLER

DESCRIPTION:
  This function handles all the IRAT messages received in IRAT state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdsirat_msg_handler(tdsirat_msg_union_type* msg);

/*=========================================================================

 FUNCTION     :TDSIRAT_ACQ_PROC_STEP0_RSP
 
 DESCRIPTION  :  This function process the step0 response from FW. Based on
  the result and availabe gap left, l1 can decide whether continue to try step0 
  on a different LNA setting, or start step1, or send response back to the active RAT.
              
 DEPENDENCIES : 

 INPUT PARAMETERS:
 tfw_srch_gap_detect_rsp_t *gap_detect_rsp
   
 RETURN VALUE : void
   
 SIDE EFFECTS : 
==========================================================================*/
void tdsirat_acq_proc_step0_rsp(tfw_srch_gap_detect_rsp_t *gap_detect_rsp);


/*=========================================================================

 FUNCTION     :TDSIRAT_ACQ_PROC_STEP1_RSP
 
 DESCRIPTION  :  This function process the step1 response from FW and send
 respective response back to the active RAT.
              
 DEPENDENCIES : 

 INPUT PARAMETERS:
 tfw_srch_syncdl_midamble_detect_rsp_t *syncdl_midamble_detect_rsp
   
 RETURN VALUE : void
   
 SIDE EFFECTS : 
==========================================================================*/
void tdsirat_acq_proc_step1_rsp(tfw_srch_syncdl_midamble_detect_rsp_t *syncdl_midamble_detect_rsp);

/*===========================================================================
  FUNCTION     : tdsirat_proc_rscp_rsp  

  DESCRIPTION: Handle rscp_rsp from FW. This function is only invoked in the
    IRAT_MEAS state.
  
  DEPENDENCIES:
 
  INPUT PARAMETERS:
    tfw_measure_rscp_rsp_t *rscp_rsp
 
  RETURN VALUE: void

  SIDE EFFECTS: None
===========================================================================*/

void tdsirat_proc_rscp_rsp(tfw_measure_all_rscp_rsp_t *rscp_rsp);

/*===========================================================================
  FUNCTION     : tdsirat_proc_state_cfg_rsp  

  DESCRIPTION: Handle state_cfg_rsp from FW. 
  
  DEPENDENCIES:
 
  INPUT PARAMETERS:
    tfw_state_t state
 
  RETURN VALUE: void

  SIDE EFFECTS: None
===========================================================================*/
void tdsirat_proc_state_cfg_rsp(tfw_state_t state);

#ifdef FEATURE_TDSCDMA_RF_SUPPORT

void tdsirat_x2t_rf_build_script(tdsirat_meas_rf_param_type *rf_param, uint32 uarfcn, rfa_tdscdma_rxlm_mode_t rxlm_mode);

void tdsirat_x2t_rf_meas_enter(uint32 uarfcn);

void tdsirat_x2t_rf_meas_exit(uint32 uarfcn, int buffer_idx);
#endif

/*===========================================================================
  FUNCTION     : tdsirat_send_fw_cmd  

  DESCRIPTION: if gap is not aborted, send acq/meas command to TFW
  
  DEPENDENCIES:
 
  INPUT PARAMETERS:
 
  RETURN VALUE:

  SIDE EFFECTS: None
===========================================================================*/
void tdsirat_send_fw_cmd(void);

#endif /* TDSIRAT_H */


