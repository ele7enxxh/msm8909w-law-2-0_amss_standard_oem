#ifndef MCALTDS_DL_H
#define MCALTDS_DL_H

/*============================================================================*/
/** @file
 * This module has definition and declaration related to MCAL UMTS downlink
 * channel configurations (add, reconfigure, drop) module APIs
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2007 - 2008 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/mcaltds_dl.h#1 $
$DateTime: 2016/12/13 07:58:19 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
7/5/11       YH      Cleanup code for TD production

===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "tdscdma_variation.h"
#ifdef FEATURE_MCAL_TDSCDMA
#include "comdef.h"
#include "log.h"
#include "log_codes.h"
//#include "log_codes_tds_int.h"
#include "wfw_sw_intf.h"

#include "msgr.h"
#include "msgr_umid.h"

//******************************* SW/FW INTERFACE ******************************************//


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* Enum type declaration */
/* --------------------- */
/** This enumerates the various downlink physical channel reference id's */
typedef enum
{
  /** Page Indicator Channel */
  MCALTDS_DL_PHYCHAN_PICH,
  /** Primary Common Control Physical Channel */
  MCALTDS_DL_PHYCHAN_PCCPCH,
  /** Secondary Common Control Physical Channel */
  MCALTDS_DL_PHYCHAN_SCCPCH,
  /** Dedicated Physical Channel */
  MCALTDS_DL_PHYCHAN_DPCH,
  /** Number of physical channel types */
  MCALTDS_NUM_DL_PHYCHAN
} mcaltds_dl_phychan_enum_type;

typedef enum {
	MCALTDS_DL_DEFAULT_MIDAMBLE,
	MCALTDS_DL_COMMON_MIDAMBLE,
	MCALTDS_DL_UE_SPECIFIC_MIDAMBLE,
	MCALTDS_DL_INVALID_MIDAMBLE
}mcaltds_dl_midamble_alloc_mode_enum_type;

typedef enum {
	MCALTDS_DL_MIDAMBLE_CONFIG_2,
	MCALTDS_DL_MIDAMBLE_CONFIG_4,
	MCALTDS_DL_MIDAMBLE_CONFIG_6,
	MCALTDS_DL_MIDAMBLE_CONFIG_8,
	MCALTDS_DL_MIDAMBLE_CONFIG_10,
	MCALTDS_DL_MIDAMBLE_CONFIG_12,
	MCALTDS_DL_MIDAMBLE_CONFIG_14,
	MCALTDS_DL_MIDAMBLE_CONFIG_16,
	MCALTDS_DL_NUM_MIDAMBLE_CONFIG
}mcaltds_dl_midamble_config_enum_type;

typedef enum {
	MCALTDS_DL_PCCPCH_KNOWN_TTI_BOUNDARY,
	MCALTDS_DL_PCCPCH_PHASE_BASED_DETECTION,
	MCALTDS_DL_PCCPCH_BLIND_TTI_DETECTION,
	MCALTDS_DL_INVALID_PCCPCH_TTI_DETECTION
}mcaltds_dl_pccpch_tti_detection_enum_type;

typedef enum {
  MCALTDS_DL_FRAME_MODE,
  MCALTDS_DL_TIMESLOT_MODE,
  MCALTDS_DL_INVALID_SEC_INTERLEAVING_MODE
}mcaltds_dl_sec_interleaving_mode_enum_type;

#if 0
typedef enum {
  MCALTDS_DL_TDM_MODE_DISABLE,
  MCALTDS_DL_TDM_MODE_ENABLE,
  MCALTDS_DL_INVALID_TDM_MODE
}mcaltds_dl_tdm_mode_enum_type;   
#endif
/* Structure type declaration */
/* -------------------------- */


/*************************************** MCAL DL *******************************************/
#define MCALTDS_DL_MAX_NUM_TS 6
#define MCALTDS_DL_MAX_NUM_CODE_PER_TS 16
#define MCALTDS_DL_MAX_NUM_CCTRCH 3
#define MCALTDS_DL_MAX_NUM_PHYCHAN 4
#define MCALTDS_DL_MAX_NUM_SLOT_FORMAT 25
#define MCALTDS_DL_MAX_PAGING_Q_VALUE 87
#define MCALTDS_DL_MAX_PCCPCH_TDM_REPETITION_PERIOD 64
#define MCALTDS_DL_MAX_SCCPCH_TDM_REPETITION_PERIOD 64
#define MCALTDS_DL_MAX_DPCH_TDM_REPETITION_PERIOD 64
#define MCALTDS_DL_MAX_CFN 255
#define MCALTDS_DL_MAX_MIDAMBLE_SHIFT 15
#define MCALTDS_DL_MAX_PI_LEMGTH 16
#define MCALTDS_DL_NUM_MAX_PHYCHAN_DB 6 
#define MCALTDS_DL_MAX_MIDAMBLE_CONFIG_VALUE 16 

typedef struct {
  uint8 ts_number;
  mcaltds_dl_midamble_alloc_mode_enum_type midamble_allocation_mode;
  uint8 midamble_config;
  uint8 midamble_shift;
  uint16 chan_code_bitmap;
  uint8 slot_format[MCALTDS_DL_MAX_NUM_CODE_PER_TS];
  uint8 num_tpc;
}mcaltds_dl_ts_cfg_parms_struct_type;

typedef struct {
  uint8 pi_length;
  uint8 q_value;
  uint8 ts_number;
  mcaltds_dl_midamble_alloc_mode_enum_type midamble_allocation_mode;
  uint8 midamble_config;
  uint8 midamble_shift;
  uint16 chan_code_bitmap;
}mcaltds_dl_pich_cfg_struct_type;

typedef struct {
  uint8 cctrch_idx;
  mcaltds_dl_pccpch_tti_detection_enum_type tti_detection_algorithm;
  boolean tstd_ind;
  mcaltds_dl_midamble_alloc_mode_enum_type midamble_allocation_mode;
  uint8 midamble_config;
  uint8 midamble_shift;  
  uint8 tdm_repetition_period;
  uint64 tdm_chanpresent_mask;
}mcaltds_dl_pccpch_cfg_struct_type;

typedef struct {
  uint8 cctrch_idx;
  mcaltds_dl_sec_interleaving_mode_enum_type second_interleaving_mode;
  /*Yong comment out TDM mode for new interface of SW and FW*/
  /*mcaltds_dl_tdm_mode_enum_type tdm_mode;  */
  /*Yong comment out TDM mode for new interface of SW and FW*/
  uint8 tdm_repetition_period;
  uint64 tdm_chanpresent_mask;
  mcaltds_dl_ts_cfg_parms_struct_type ts_cfg;

  /*index FW to insret dob when zero tb, PCH will set to 1*/
  uint8 insertDobForZeroTB;
#ifdef FEATURE_TDSCDMA_PCH_FET
  uint8 pch_fet_enable;
#endif
  /*! \brief SFN number within TTI at the action time when config is applied  Valid :0..255.*/
  //uint8  ttiSfn;
}mcaltds_dl_sccpch_cfg_struct_type;

typedef struct {
  uint8 cctrch_idx;
  mcaltds_dl_sec_interleaving_mode_enum_type second_interleaving_mode;
  boolean tstd_ind;
  uint8 cfn;
  /*Yong comment out TDM mode for new interface of SW and FW*/
  /*mcaltds_dl_tdm_mode_enum_type tdm_mode;  */
  /*Yong comment out TDM mode for new interface of SW and FW*/
  uint8 tdm_repetition_period;
  uint64 tdm_chanpresent_mask;
  uint8 num_ts;
#ifdef FEATURE_TDSCDMA_LVA_DECODE
  /*for LVA using*/
  uint8 lva_enable;
#endif
  mcaltds_dl_ts_cfg_parms_struct_type ts_cfg[MCALTDS_DL_MAX_NUM_TS];
}mcaltds_dl_dpch_cfg_struct_type;

typedef struct { 
   mcaltds_dl_pich_cfg_struct_type pich_cfg;
   mcaltds_dl_pccpch_cfg_struct_type pccpch_cfg;
   mcaltds_dl_sccpch_cfg_struct_type sccpch_cfg;
   uint8 num_cctrch;
   mcaltds_dl_dpch_cfg_struct_type  dpch_cfg[MCALTDS_DL_MAX_NUM_CCTRCH];
}mcaltds_dl_phychan_cfg_struct_type;


/** This structure is used to contain the parameters for the downlink
  * physical channel config command to the mDSP */
typedef struct
{
  uint8 notify_cfg;
  uint16  actionTime;
  mcaltds_dl_phychan_enum_type phychan_type;
  mcaltds_dl_phychan_cfg_struct_type phychan_cfg;
} mcaltds_dl_phch_cfg_parms_struct_type;

/* ---------------- END OF MCAL DL ------------------ */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* =======================================================================
**                        Function Declarations
** ======================================================================= */
/*===========================================================================
FUNCTION         mcaltds_dl_phychan_add

DESCRIPTION      
  This initializes all the downlink physical channel stuff
 

DEPENDENCIES
  None
  
RETURN VALUE 
  None

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void mcaltds_dl_init(void);


/*===========================================================================
FUNCTION         mcaltds_dl_phychan_add

DESCRIPTION      
  Function for setting up a physical layer channel based on passed arguments.
  Phycical channel is not setup immediately. Separate indication is sent in
  once physical channel is setup and can be assumed functions towards its
  frame boundary. In case this function is not successful no such indication
  is sent.
 

DEPENDENCIES
  None
  
RETURN VALUE 
 return DL channel Id that can be used to reconfigure or tear down channel.
 In case of failure invalid H/W channel Id is return and no channel cfg done
 indication is sent.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern boolean mcaltds_dl_phychan_add(
   uint8 config_id,
  /** Parameters for setting up the downlink physical channel */
  mcaltds_dl_phch_cfg_parms_struct_type *phch_cfg_parms_ptr);


/*===========================================================================
FUNCTION         mcaltds_dl_phychan_reconfigure

DESCRIPTION      
 Function for reconfiguring a physical layer channel based on updated
 arguments. This function is to be used for channel that is already
 configured using mcaltds_dl_phychan_add function. ust like add function
 recfg os also not immediately. Separate indication is sent in, once physical
 channel is setup and can be assumed functions towards its frame boundary.

DEPENDENCIES
  None
  
RETURN VALUE 
 return Boolean indicating that channel can be configured properly or not.
 In case of failure no channel cfg done indication is sent

SIDE EFFECTS
  None.
  
===========================================================================*/
extern boolean mcaltds_dl_phychan_reconfigure(
  /** config Id for the channel to be reconfigured */
  uint8 old_config_id,
  /** config Id for the channel to be reconfigured */
   uint8 new_config_id,
  /** Parameters for reconfigure the downlink physical channel */
  mcaltds_dl_phch_cfg_parms_struct_type *phch_cfg_parms_ptr);

/*===========================================================================
FUNCTION         mcaltds_dl_phychan_drop

DESCRIPTION      
 Function for dropping an existing physical layer channel. It sends indication
 of channel cfg done once tear down is assumed to be complete.
 
DEPENDENCIES
  None
  
RETURN VALUE 
 return Boolean indicating that channel can be dropped properly or not.
 In case of failure no channel cfg done indication is sent

SIDE EFFECTS
  None.
  
===========================================================================*/
extern boolean mcaltds_dl_phychan_drop(
  /** Combiner Id for the channel to be dropped */
  uint8 config_id,
  /** Parameters for droping down the downlink physical channel */ 
  mcaltds_dl_phch_cfg_parms_struct_type* phch_cfg_parms_ptr);

/*===========================================================================
FUNCTION         mcaltds_dl_phychan_cfg_done_ind

DESCRIPTION      
 Indicator function when all channel configuration is over. It is at frame
 boundry of channel getting added, reconfigured or dropped

DEPENDENCIES
  None
  
RETURN VALUE 
  TRUE or FALSE

SIDE EFFECTS
  None.
  
===========================================================================*/
void mcaltds_dl_phychan_cfg_done_ind(
  /** Combiner Id for the channel to be configured */
  uint8 cfg_id,
  
  /** cfn at which channel configuration is going to be active */
  uint8 ccfn);

/*===========================================================================
FUNCTION         mcaltds_dl_phychan_configure

DESCRIPTION      
 According to action type, mcal sends configuration cmd to FW, 
 in this function mcal notify highlayer.

DEPENDENCIES
  None
  
RETURN VALUE 
  TRUE or FALSE

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mcaltds_dl_phychan_configure(
  /** Config Id for the channel to be reconfigured */
  uint8 config_id,
  /** Config action type */
  uint8 action,
  /** Parameters for reconfigure the downlink physical channel */
  mcaltds_dl_phch_cfg_parms_struct_type *phch_cfg_parms_ptr);

/*===========================================================================
FUNCTION         mcaltds_dl_send_phychan_config_cmd

DESCRIPTION      
 Send configuration cmd to FW, in this function mcal will construct
 cmd msg and call cmd send function.

DEPENDENCIES
  None
  
RETURN VALUE 
  TRUE or FALSE

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mcaltds_dl_send_phychan_config_cmd(
  /** Config Id for the channel to be reconfigured */
  uint8 config_id,
  /** Config action type */
  uint8 action,
  /** Parameters for reconfigure the downlink physical channel */
  mcaltds_dl_phch_cfg_parms_struct_type *phch_cfg_parms_ptr);

/*===========================================================================
FUNCTION         mcaltds_dl_send_rx_time_sync_cmd

DESCRIPTION      
 When downlink finishes slaming RX timeline, 
 downlink notifys FW this difference of RX and wall time counter.

DEPENDENCIES
  None
  
RETURN VALUE 
  None

SIDE EFFECTS
  None.
  
===========================================================================*/
void mcaltds_dl_send_rx_time_sync_cmd(uint32 offsetCnt, boolean validOffset, int16 freqOffset);

/*===========================================================================
FUNCTION         mcaltds_dl_send_sync_status_cmd

DESCRIPTION      
 When downlink report sync status to RRC, at the same time tell FW.

DEPENDENCIES
  None
  
RETURN VALUE 
  None

SIDE EFFECTS
  None.
  
===========================================================================*/
void mcaltds_dl_send_sync_status_cmd(boolean sync_status,uint16 action_time);

/*===========================================================================
FUNCTION         mcaltds_dl_phychan_add_validate

DESCRIPTION      
 This validates the addition of a physical channel with specified parameters,
 Returns result of validation.

DEPENDENCIES
  None
  
RETURN VALUE 
  TRUE or FALSE.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mcaltds_dl_phychan_add_validate(
  /** Config Id for the channel to be reconfigured */
  uint8 config_id,
  /** Parameters for setting up the downlink physical channel */
  mcaltds_dl_phch_cfg_parms_struct_type *phychan_cfg_parms);


/*===========================================================================
FUNCTION         mcaltds_dl_phychan_reconfigure_validate

DESCRIPTION      
 This validates the reconfiguration of an existing physical channel 
 on the channel with specified ID. Returns result of validation.

DEPENDENCIES
  None
  
RETURN VALUE 
  TRUE or FALSE.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mcaltds_dl_phychan_reconfigure_validate(
  /** old config Id for the channel to be reconfigured */
  uint8 old_config_id,
  /** new config Id for the channel to be reconfigured */
  uint8 new_config_id,
  /** Parameters for setting up the downlink physical channel */
  mcaltds_dl_phch_cfg_parms_struct_type *phychan_cfg_parms);


/*===========================================================================
FUNCTION         mcaltds_dl_phychan_cfg_parms_validate

DESCRIPTION      
 validate physical channel configuration parameter when channel drop.

DEPENDENCIES
  None
  
RETURN VALUE 
  TRUE or FALSE.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mcaltds_dl_phychan_drop_validate(
  /** config Id for the channel to be dropped */
  uint8 config_id,
  /** Parameters for the downlink physical channel */
  mcaltds_dl_phch_cfg_parms_struct_type *phychan_cfg_parms);

/*===========================================================================
FUNCTION         mcaltds_dl_phychan_cfg_parms_validate

DESCRIPTION      
 validate physical channel configuration parameter.

DEPENDENCIES
  None
  
RETURN VALUE 
  TRUE or FALSE.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mcaltds_dl_phychan_cfg_parms_validate(
  /** Parameters for setting up the downlink physical channel */
  mcaltds_dl_phch_cfg_parms_struct_type *phychan_cfg_parms);
/*===========================================================================
FUNCTION         mcaltds_dl_send_cmd_msg

DESCRIPTION      
 send configuration cmd to FW.

DEPENDENCIES
  None
  
RETURN VALUE 
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void mcaltds_dl_send_cmd_msg( msgr_hdr_struct_type* hdr,uint32 msg_size);
/*===========================================================================
FUNCTION         mcaltds_dl_create_cmd_msg

DESCRIPTION      
 create cmd msg for configuration.

DEPENDENCIES
  None
  
RETURN VALUE 
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void mcaltds_dl_create_cmd_msg( msgr_hdr_struct_type* hdr, msgr_umid_type umid);

/*===========================================================================
FUNCTION         mcaltds_dl_get_deep_srch_hypo_num

DESCRIPTION      
  Helper funtion to return the current number of hypos used in deep search
  blind decoding.

DEPENDENCIES
  None
  
RETURN VALUE 
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
uint8 mcaltds_dl_get_deep_srch_hypo_num(void);

/*===========================================================================
FUNCTION         mcaltds_dl_get_deep_srch_freq_step_size

DESCRIPTION      
  Helper funtion to return the current freq step size used in deep search
  blind decoding.

DEPENDENCIES
  None
  
RETURN VALUE 
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
uint16 mcaltds_dl_get_deep_srch_freq_step_size(void);

/*===========================================================================
FUNCTION         mcaltds_dl_phychan_notify_after_decoding_done

DESCRIPTION      
  when recevie PCH decoding done begin response suspend done

DEPENDENCIES
  None
  
RETURN VALUE 
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void mcaltds_dl_phychan_notify_after_decoding_done(void);

#ifdef FEATURE_TDSCDMA_L1_LOGGING_ENABLED
/*===========================================================================
FUNCTION         mcaltds_dl_log_submit

DESCRIPTION      
  Callback function that mcal manager calls when it is signaled that there is
  a packet to be logged.

DEPENDENCIES
  None
  
RETURN VALUE 
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void mcaltds_dl_log_submit(void);
#endif
#endif /* #ifdef FEATURE_MCAL_TDSCDMA */
#endif /* MCALWCDMA_DL_H */

