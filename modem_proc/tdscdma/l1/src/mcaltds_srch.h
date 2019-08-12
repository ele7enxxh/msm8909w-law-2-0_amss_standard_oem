#ifndef __MCALTDS_SRCH_H__
#define __MCALTDS_SRCH_H__

/*============================================================================
                        M C A L W C D M A _ S R C H. H
DESCRIPTION
  This module has MCAL (modem core abstraction layer) Searcher APIs.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Function mcaltds_srch_init should be at least called once before calling 
  any other mcal searcher api. 

  Function mcaltds_srch_set_status_callback_fn installs a callback function.
  If callback function is not installed then the search requests will be rejected.

  Function mcaltds_srch_update_srch_async_parms updates the async parameters.

  Function mcaltds_srch_do_srch performs a search.

  Function mcaltds_srch_get_srch_results returns the search results.

Copyright (c) 2007 - 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

============================================================================*/

#include "tdscdma_variation.h"
#include "comdef.h"
#include "tfw_sw_intf_msg.h"
#include "tfw_sw_intf_measurement.h"
#include "tfw_sw_intf_srch.h"
#include "tdssrchset.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* --------------------------------------------------------------------------
   Defines and enumerations
   --------------------------------------------------------------------------*/

/* Peak detect mode (00: normal, 01: Bypass, 10: Shoulder) */
typedef enum
{
  MCALTDS_SRCH_PD_NORMAL,
  MCALTDS_SRCH_PD_BYPASS,
  MCALTDS_SRCH_PD_SHOULDER,
  MCALTDS_SRCH_PD_NUM_TYPES
} mcaltds_srch_pd_type;

/* List of searches */
typedef enum
{
  MCALTDS_SRCH_RSCP,
  MCALTDS_SRCH_ISCP,
} mcaltds_srch_type_tmp;


/* rxd mode (00: no rxd, 01: separate, 10: Add) */
typedef enum
{
  /* No Receive Diversity */
  MCALTDS_SRCH_RXD_MODE_NO_RXD,
  /* FW to search on one antenna at a time and report sorted peaks from
  both the antenna separtely */
  MCALTDS_SRCH_RXD_MODE_SEPARATE,
  /*combined energies from both the antenna are returned*/
  MCALTDS_SRCH_RXD_MODE_ADD,
  /* total number of Rxd modes available */
  MCALTDS_SRCH_NUM_RXD_MODE
} mcaltds_srch_rxd_mode_enum_type;

/* --------------------------------------------------------------------------
   Structures and unions
   --------------------------------------------------------------------------*/

#define MCALTDS_SRCH_RSCP_CELL_MAX 32

#define MCALTDS_SRCH_ISCP_PAIR_MAX 32

typedef struct
{
 uint8 cpi;
 //uint16 ref_pos;
 //uint32 win_size;
 //uint32 sfn; 
 uint16  oldrefpos;
}mcaltds_srch_rscp_cell_info_type;

typedef struct
{
 uint8 message_id;
 boolean measure_all;
 uint8 cell_num;
 uint8 ts0JdcsNum;
 mcaltds_srch_rscp_cell_info_type cell_info[MCALTDS_SRCH_RSCP_CELL_MAX];
 /*Rx diversity flag 
   0: One rx antenna
   1: Two rx antennas
   If set to 1, RxAntenna filed ignored*/
 boolean rxDiv;
  /*0: Antenna 0
   1: Antenna 1 */ 
 boolean rxAntenna;
 /*0: Intra freq
     1: Inter freq  */
 boolean interFreq; 
  /*If interFreq is 0, ignored
   There are multiple RF scripts buffers for working freq tuning, measurement
   freq tuning etc.
   Configuration TBD   */
 uint8 interFreqRfBufIdx;
 uint8 rxlmStaticBufIndex;
   /*0: measured freq is not the same primary freq
   1: measured freq is the same as the primary freq
   Placeholder, to be used if DwPTS based FTL/TTL loops runs when measurement
   freq is the same as the active cell primary frequency*/
 boolean activePrimaryFreq;
   /* measured frequency (Hz), Used by FW for Frequency Tracking Loop (FTL) */
  uint32   measuredFreq;

  /* SW help FW to store LNA info for inter freq measurement */ 
  #ifdef FEATURE_TDSCDMA_FW_NEW_LNA_INTF
  tfw_rx_lna_t lnaStateInfo;
  #else
  tfw_inter_freq_lna_t inter_lna_info;  
  #endif
 
}mcaltds_srch_rscp_parms_struct_type;

typedef struct
{
 uint8 ts_id;
 uint8 cpi;
}mcaltds_srch_iscp_pair_info_type;

typedef struct
{
 uint8 pair_num;
 mcaltds_srch_iscp_pair_info_type pair_info[MCALTDS_SRCH_ISCP_PAIR_MAX];
}mcaltds_srch_iscp_parms_struct_type;

/* global variable to remember current tfw state */
extern tfw_state_t mcaltds_tfw_state;

/* --------------------------------------------------------------------------
   MCAL SRCH APIs external functions
   --------------------------------------------------------------------------*/
/*===========================================================================
FUNCTION   mcaltds_srch_init

DESCRIPTION
  This is MCAL WCDMA Search initialization function. This function should be
  called before calling any other mcal apis. Currently this is called when 
  wcdma mode is entered. The following operations are done in this function.
   - Initialize the state variables related to searcher module.
   - Install the interrupt handlers with mdsp services
   - Define timers

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_srch_init( void ); 

/*===========================================================================
FUNCTION   mcaltds_srch_do_power_scan

DESCRIPTION
  This function sends power scan cmd to FW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_srch_do_power_scan(void);


/*===========================================================================
FUNCTION   mcaltds_srch_send_gap_detect_cmd

DESCRIPTION
  This function sends gap_detect_cmd to FW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_srch_send_gap_detect_cmd( tfw_srch_gap_detect_cmd_t* );

/*===========================================================================
FUNCTION   mcaltds_srch_send_syncdl_midamble_detect_cmd

DESCRIPTION
  This function sends syncdl_midamble_detect_cmd to FW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_srch_send_syncdl_midamble_detect_cmd (tfw_srch_syncdl_midamble_detect_cmd_t *);


/*=========================================================================

 FUNCTION     :MCALTDS_SRCH_FILL_SERVING_CELL
 
 DESCRIPTION  : The function is used to fill serving cell information for FW. 
              
 DEPENDENCIES : 

 RETURN VALUE : 
   
 SIDE EFFECTS : None
==========================================================================*/
LOCAL void mcaltds_srch_fill_serving_cell(void);

/*=========================================================================

 FUNCTION     :MCALTDS_SRCH_IS_TS0_JDCS_CELL
 
 DESCRIPTION  : The function is used to check if the cell belongs to TS0-JDCS. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint8 cpi : cell parameter id.
   
 RETURN VALUE : 
   boolean: if the cell belongs to current TS0-JDCS, return TRUE; otherwise, return FALSE;
   
 SIDE EFFECTS : None
==========================================================================*/
extern boolean mcaltds_srch_is_ts0_jdcs_cell(uint8 cpi);

/*=========================================================================

 FUNCTION     :MCALTDS_SRCH_FILL_TS0_JDS
 
 DESCRIPTION  : The function is used to fill TS0-JDCS. 
              
 DEPENDENCIES : 
  
 RETURN VALUE : NONE.
   
 SIDE EFFECTS : None
==========================================================================*/
LOCAL void mcaltds_srch_fill_ts0_jdcs(void);

/*=========================================================================

 FUNCTION     :MCALTDS_SRCH_IS_JDCS_CELL
 
 DESCRIPTION  : The function is used to check if the cell belongs to JDCS. 
              
 DEPENDENCIES : 

 INPUT PARAMETERS: 
   uint8 cpi : cell parameter id.
   
 RETURN VALUE : 
   boolean: if the cell belongs to current JDCS, return TRUE; otherwise, return FALSE;
   
 SIDE EFFECTS : None
==========================================================================*/
extern boolean mcaltds_srch_is_jdcs_cell(uint8 cpi);

/*=========================================================================

 FUNCTION     :MCALTDS_SRCH_FILL_JDCS
 
 DESCRIPTION  : The function is used to fill JDCS information for FW. 
              
 DEPENDENCIES : 

 RETURN VALUE : 
   
 SIDE EFFECTS : None
==========================================================================*/
LOCAL void mcaltds_srch_fill_jdcs(void);

/*===========================================================================
FUNCTION   	MCALTDS_SRCH_UPDATE_SETS

DESCRIPTION
  This function sends cell_serving_jdcs_cmd to FW

DEPENDENCIES
  None

INPUT PARAMETERS:
  boolean new_serving_cell: if it is TRUE, it denotes that serving cell need be changed.
  boolean new_working_freq: if it is TRUE, it denotes that working frequency need be changed.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_srch_update_sets(boolean new_serving_cell, boolean new_working_freq );

/*===========================================================================
FUNCTION   	MCALTDS_SRCH_UL_CELL_BHO_CMD

DESCRIPTION
  This function sends cell_baton_handover_cmd to FW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void mcaltds_srch_ul_cell_bho_cmd(uint16 startSubframe, uint8 durationSubframes);

/*===========================================================================
FUNCTION   mcaltds_srch_send_irat_gap_detect_cmd

DESCRIPTION
  This function sends gap_detect_cmd to FW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcaltds_srch_send_irat_gap_detect_cmd(tfw_irat_gap_detect_cmd_t  *gap_detect_msg );

/*===========================================================================
FUNCTION   mcaltds_srch_send_irat_syncdl_midamble_detect_cmd

DESCRIPTION
  This function sends irat_syncdl_midamble_detect_cmd to FW

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mcaltds_srch_send_irat_syncdl_midamble_detect_cmd(tfw_irat_syncdl_midamble_detect_cmd_t  *syncdl_midamble_detect_msg);

/*===========================================================================
FUNCTION   mcaltds_srch_send_irat_rscp_cmd_msg

DESCRIPTION
  This function sends RSCP measurement request to MDSP

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void mcaltds_srch_send_irat_rscp_cmd_msg(tfw_irat_measure_rscp_cmd_t *rscp_msg);

extern boolean mcaltds_srch_do_rscp(mcaltds_srch_rscp_parms_struct_type * sync_parms);

extern boolean mcaltds_srch_do_all_rscp(mcaltds_srch_rscp_parms_struct_type * sync_parms);

extern boolean mcaltds_srch_do_iscp(mcaltds_srch_iscp_parms_struct_type * sync_parms);

extern void mcaltds_srch_move_fw_to_standby_state(void);

extern void mcaltds_srch_move_fw_to_acq_state(void);

extern void mcaltds_srch_move_fw_to_trk_state(void);

extern void mcaltds_srch_move_fw_to_irat_state(void);

extern void mcaltds_srch_target_cell_cmd(tdssrchset_active_cell_info_struct_type * active_cell_info_ptr);

extern void mcaltds_srch_reacq_cmd(tfw_srch_reacq_cfg_t * reacq_cfg);

#ifdef FEATURE_TDSCDMA_ANTENNA_SWITCH_DIVERSITY
void mcaltds_srch_send_asdiv_cmd(uint32 switchConfig, uint32 priority);
void mcaltds_srch_send_aol_abort_cmd(uint32 abortConfig);
#endif
extern void mcaltds_srch_send_ts0_cell_cmd_after_ifreq_qtuner_update(void);
#endif /* __MCALTDS_SRCH_H__ */

