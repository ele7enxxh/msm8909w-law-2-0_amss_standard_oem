#ifndef MCALTDS_HSCFG_H
#define MCALTDS_HSCFG_H

/*============================================================================
                        M C A L T D S _ H S C F G. H
DESCRIPTION
  This module has MCAL (modem core abstraction layer) TDS channels configuration
  APIs declarations.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2010 - 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/mcaltds_hscfg.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------   
02/28/2012  sq      Added HSSCCH CRC statistics logging   
01/03/2011  ow      Switched to new RRC/L1 IF 
10/18/2010  ow      initial version 
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */

#include "tdscdma_variation.h"
#include "customer.h"
#include "mcaltds_dl.h"
#include "tdsl1rrcif.h"
#include "mcaltds_deci.h"
#include "tdssrch.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
#define MCALTDS_HS_PDSCH_STATUS_MAX_SUB_FRAMES   8

#define MCALTDS_MAX_DL_SLOT_PER_SUBFAME	6

/* delta SIR for HS-SCCH OLPC */
#define MCALTDS_HS_SCCH_OLPC_DELTA_SIRX10 5 // DB
#define MCALTDS_HS_SCCH_OLPC_MAX_MISSED_HCSN 3 // TBD
#define MCALTDS_HS_SCCH_OLPC_POW_FLOOR 2.6 // TBD
#define MCALTDS_HS_SCCH_OLPC_POW_CEIL 20 // TBD

#ifdef FEATURE_TDSCDMA_DSDS_QTA
#define MCALTDS_HS_SCCH_OLPC_DSDS_QTA_DURATION_INIT_VAL 0xFFFF
#define MCALTDS_HS_SCCH_OLPC_DSDS_QTA_DURATION_THRESHOLD 1000
#endif

/* only define this locally for DOB and HW debugging over RUMI/SURF */
#ifdef FEATURE_TDSCDMA_PLT
//#define TDS_HS_NO_FW_DEBUG
#endif
//#undef TDS_HS_NO_FW_DEBUG
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
typedef tdsl1_hs_dsch_info_struct_type mcaltds_hs_dsch_info_struct_type;
typedef tdsl1_hs_scch_sich_info_common_params_struct_type mcaltds_hs_scch_sich_struct_type;
//typedef tdsl1_hsdpa_setup_ops_enum_type mcaltds_hs_setup_ops_enum_type;
typedef tdsl1_midamble_ss_burst_struct_type mcaltds_hs_midamble_info_struct_type;

typedef void (*mcal_post_cb_fn)(void);
//typedef void (*mcal_post_cb_fn)(void);

/* midamble updating information */
typedef struct
{
  boolean occupied_slot[MCALTDS_MAX_DL_SLOT_PER_SUBFAME]; 
  /* midamble information per time slot*/
  mcaltds_hs_midamble_info_struct_type dl_midambe_slot[MCALTDS_MAX_DL_SLOT_PER_SUBFAME];

} mcaltds_dl_update_fw_midamble_info_struct_type;

/*! @brief Physical channel configurations types */
typedef enum
{
  /*! Channel is disabled - Note: must be zero */
  TFW_DEMOD_PHCH_CFG_DISABLE = 0,
  /*! Channel is enabled */
  TFW_DEMOD_PHCH_CFG_ENABLE = 1,
  /*! Channel recfg */
  TFW_DEMOD_PHCH_CFG_RECFG = 2,
} mcaltds_dl_hs_channel_cfg_mdsp_action_enum_type;

/* @brief Physical channel configurations types, internal use */
typedef enum
{
  MCALTDS_DL_HS_CMD_DISABLE = 0,
  MCALTDS_DL_HS_CMD_ENABLE = 1,
  MCALTDS_DL_HS_CMD_RECFG = 2,
} mcaltds_dl_hs_cmd_action_type;

/* HS DL channels information structure, local DB */
typedef struct
{
  /* indicate if this configuration is valid or not */
  boolean valid;

  /* HS channel info variable */
  /* ------------------------ */

  /* CCTrCh Id for this configuration */
  uint8 cctrch_id;

  /* indicate info for MAC-d PDI size Qs
     Q Id goes with index of this array */
  mcaltds_dec_hs_mac_d_pdu_sz_q_info_struct_type
    mac_d_pdu_sz_q_info[TDSL1_DL_CCTRCHCFG_MAC_HS_Q_MAX_COUNT];

 #ifdef FEATURE_TDSCDMA_64QAM
  boolean hs_64_qam_configured;
 #endif

  /* HS configuration related variable */
  /* --------------------------------- */

  /* configuration CFN */
  uint8 config_cfn;

  /* HSDPA channel configuration action used by FW */
  mcaltds_dl_hs_channel_cfg_mdsp_action_enum_type action;

  /* Reset IR buffer or not */
  boolean softBufferFlush;

  /* HRNTI, add this because FW/SW interface change */
  uint16 h_rnti;

  /* hs-pdsch midamble information*/  
  tdsl1_midamble_ss_burst_struct_type  pdsch_midamble;

  /* hs-scch midamble */
  mcaltds_dl_update_fw_midamble_info_struct_type midamble_para;

} mcaltds_hs_channel_cfg_db_struct_type;

/* HS-PDSCH data packet decoding status */
typedef enum 
{
  TDSHS_PDSCH_ACK,
  TDSHS_PDSCH_NACK,
  TDSHS_PDSCH_UNKNOWN
}mcaltds_hs_pdsch_dec_result_enum_type;

/* HS-PDSCH decoding status returned to HSUPA module*/
typedef struct
{
  int16 ack_nack_pwr_offset;
  mcaltds_hs_pdsch_dec_result_enum_type  dec_result;
}mcaltds_hs_pdsch_dec_status_info_struct_type;

/* HS-PDSCH decoding status stored locally*/
typedef struct
{
  uint16 sub_frame_num;
  mcaltds_hs_pdsch_dec_result_enum_type  dec_result;
}mcaltds_hs_data_dec_result_struct_type;

extern tdssrch_meas_hsscch_config_struct_type hs_scch_idleslot_info;

/* For HS-SCCH decoding statistics*/
extern uint32 scchRcv;
extern uint32 scchCRCPass;

/* For HS-PDSCH decoding statistics*/
extern uint32 pdschRcv;
extern uint32 pdschCRCPass;

/* For HS-PDSCH New Transmission statistics*/
extern uint32 pdschNxRcv;
extern uint32 pdschNxCRCPass;

/* For HS-PDSCH Retransmission statistics*/
extern uint32 pdschRtxRcv;
extern uint32 pdschRtxCRCPass;

/* Debug flag to reduce logging severity */
extern uint8 mcal_hs_log_debug;
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION MCALTDS_HS_CHAN_CONFIG_INIT

DESCRIPTION     This function is the API to be called by the HSDPA manager to
                initiate MCAL HS channel config

DEPENDENCIES     

RETURN VALUE    

SIDE EFFECTS    

===========================================================================*/

extern void mcaltds_hs_chan_config_init(void);

/*===========================================================================

FUNCTION MCALTDS_HS_CONFIG_CLEANUP

DESCRIPTION     This is the function to cleanup memory for message sent to FW 
                and local global memory 

DEPENDENCIES     

RETURN VALUE    

SIDE EFFECTS    

===========================================================================*/

extern void mcaltds_hs_config_cleanup(void);

/*===========================================================================

FUNCTION  mcaltds_hs_update_pdsch_midamble

DESCRIPTION: 
  This is the function to pass HS-PDSCH midamble info to FW

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/


extern void mcaltds_hs_update_pdsch_midamble(
                           /* midamble for HS-PDSCH */
                           mcaltds_hs_midamble_info_struct_type* hs_midamble);

/*===========================================================================

FUNCTION MCALTDS_HS_UPDATE_DATA_CHANNEL

DESCRIPTION     This function is the API to be called by the HSDPA manager to
                 configure HS-SCCH, HS-SICH and HS-DSCH.

DEPENDENCIES     

RETURN VALUE    

SIDE EFFECTS    

===========================================================================*/

extern void mcaltds_hs_update_data_channel(
	mcaltds_hs_dsch_info_struct_type  *hs_dsch_info);


/*===========================================================================

FUNCTION MCALTDS_HS_UPDATE_CONTROL_CHANNELS

DESCRIPTION     This is the function to pass HS-SCCH and HS-SICH cfg info
				to FW/SW async shared memory

DEPENDENCIES     

RETURN VALUE    

SIDE EFFECTS    

===========================================================================*/
extern void mcaltds_hs_update_control_channels(
  mcaltds_hs_channel_cfg_db_struct_type  *cfg_db,
  mcaltds_hs_scch_sich_struct_type  *hs_scch_sich,
  boolean common_sich_pwr);


/*===========================================================================

FUNCTION MCALTDS_HS_SEND_CMD_TO_fw

DESCRIPTION     This function triggers that messages are sent to FW and the sets 
				the frame boundary event

DEPENDENCIES     

RETURN VALUE    

SIDE EFFECTS    

===========================================================================*/

extern void mcaltds_hs_send_cmd_to_fw(
  /* call back function */
  mcal_post_cb_fn  pre_cb, 
  /* call back function */
  mcal_post_cb_fn  post_cb);


/*===========================================================================
FUNCTION mcaltds_hs_check_pdsch_decode_result

DESCRIPTION
  This function provide HS-PDSCH decoding info to HSUPA module.

DEPENDENCIES
  decoding CRC status from HS decoder

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_hs_check_pdsch_decode_result(
  /* sub frame number*/
  uint16 pdsch_sub_fn, 
  /* return value for HSUPA module*/
  mcaltds_hs_pdsch_dec_status_info_struct_type* result_ptr);


/*===========================================================================
FUNCTION mcaltds_hs_data_decode_status_update

DESCRIPTION
  This function provide HS-PDSCH decoding info by decoder; the result is
  cached by HSDPA MCAL config module

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_hs_data_decode_status_update(
  /* sub frame number for HS data coming in */
  uint16 sub_frame, 
  /* HSDPA DOB FIFO information*/
  mcaltds_dec_dob_status_fifo_struct_type* dob_staus);

/*===========================================================================
FUNCTION mcaltds_hs_get_max_vir_ir_buf_size

DESCRIPTION
  This function pass HS config higher layer paras to MCAL module

DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern uint32 mcaltds_hs_get_max_vir_ir_buf_size(void);

/*===========================================================================
FUNCTION mcaltds_hs_scch_decode_done_isr

DESCRIPTION
  This function handles the interrupt generated by H/W after
  completing HS-SCCH decoding

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void mcaltds_hs_scch_decode_done_isr(void);

/*===========================================================================
FUNCTION mcaltds_hs_get_ue_category_from_nv

DESCRIPTION
  This function retrieves the UE category from NV.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
#if 0

extern void mcaltds_hs_get_ue_category_from_nv(void);
 
#endif 
/*===========================================================================
FUNCTION mcaltds_hs_get_ue_category

DESCRIPTION
  This function returns the UE category retrieved from NV.

DEPENDENCIES
  None. Assumes that the NV read has already happened.

RETURN VALUE
  UE category

SIDE EFFECTS
  None
===========================================================================*/

extern uint8 mcaltds_hs_get_ue_category(void);

/*===========================================================================
FUNCTION mcaltds_hs_set_ue_category

DESCRIPTION
  This function set the UE category.

DEPENDENCIES
  None. Assumes that the NV read has already happened.

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/

extern void mcaltds_hs_set_ue_category(uint8 ue_cat);

#ifdef FEATURE_TDSCDMA_DSDS_QTA
/*===========================================================================
FUNCTION mcaltds_hs_set_dsds_qta_hsscch_olpc_dur_ms

DESCRIPTION
  This function set the HSSCCH olpc duration time.

DEPENDENCIES
  None.

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/

extern void mcaltds_hs_set_dsds_qta_hsscch_olpc_dur_ms(uint16 qta_dur_ms);
#endif

/*===========================================================================
FUNCTION mcaltds_hs_get_harq_size_changed

DESCRIPTION
  This function returns whether MAC HARQ size changed.

DEPENDENCIES
  None. Assumes that the NV read has already happened.

RETURN VALUE
  UE category

SIDE EFFECTS
  None
===========================================================================*/

boolean mcaltds_hs_get_harq_size_changed(void);

/*===========================================================================
FUNCTION mcaltds_hs_submit_cfg_log_pkt

DESCRIPTION
  This function handles HS configuration log packet submission in task
  context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcaltds_hs_submit_cfg_log_pkt(void);

/*===========================================================================
FUNCTION mcaltds_hs_reset_counters

DESCRIPTION
  This function resets HS-SCCH statistics counters.

DEPENDENCIES
  None

RETURN VALUE
  None 
   
SIDE EFFECTS
  None
===========================================================================*/
uint8 mcaltds_hs_reset_counters(void);

/*===========================================================================
FUNCTION mcaltds_hs_print_counters

DESCRIPTION
  This function prints out HS-SCCH statistics counters.

DEPENDENCIES
  None

RETURN VALUE
  None 
   
SIDE EFFECTS
  None
===========================================================================*/
uint8 mcaltds_hs_print_counters(void);

#ifdef FEATURE_FORCE_DROP_ADD_HSDPA
/*===========================================================================
FUNCTION mcaltds_hs_force_process

DESCRIPTION
  This function force to drop hsdpa channels in order to meas G cells

DEPENDENCIES
  None

RETURN VALUE
  None 
   
SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_hs_force_process();

/*===========================================================================
FUNCTION mcaltds_hs_force_drop

DESCRIPTION
  This function force to drop hsdpa channels in order to meas G cells

DEPENDENCIES
  None

RETURN VALUE
  None 
   
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 mcaltds_hs_force_drop();

/*===========================================================================
FUNCTION mcaltds_hs_force_add

DESCRIPTION
  This function force to add hsdpa channels after force drop

DEPENDENCIES
  None

RETURN VALUE
  None 
   
SIDE EFFECTS
  None
===========================================================================*/
extern uint8 mcaltds_hs_force_add();

#endif  // FEATURE_FORCE_DROP_ADD_HSDPA

/*===========================================================================
FUNCTION mcaltds_hs_set_data_drop_flag

DESCRIPTION
  This function force to add hsdpa channels after force drop

DEPENDENCIES
  None

RETURN VALUE
  None 
   
SIDE EFFECTS
  None
===========================================================================*/
void mcaltds_hs_set_data_drop_flag(boolean flag);

/*===========================================================================
FUNCTION mcaltds_hs_get_data_drop_flag

DESCRIPTION
  This function force to add hsdpa channels after force drop

DEPENDENCIES
  None

RETURN VALUE
  None 
   
SIDE EFFECTS
  None
===========================================================================*/
boolean mcaltds_hs_get_data_drop_flag(void);

/*===========================================================================
FUNCTION mcaltds_hs_get_cfg_action_time

DESCRIPTION
  This function force to add hsdpa channels after force drop

DEPENDENCIES
  None

RETURN VALUE
  None 
   
SIDE EFFECTS
  None
===========================================================================*/
uint16 mcaltds_hs_get_cfg_action_time(void);

#endif

