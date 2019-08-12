#ifndef _HITMSG_H
#define _HITMSG_H


/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  HIT Message SERVICE HEADER FILE

GENERAL DESCRIPTION
 This file contains types and declarations associated with the HIT command 
 processing. 

EXTERNALIZED FUNCTIONS
    
REGIONAL FUNCTIONS
    None
    
Copyright (c) 2011 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE


This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ------------------------------------------------------- 
12/23/2014   sat     API to start Flex capture for RUMI Tx
10/14/2014   wsh     Changes to support Jolokia bringup
01/08/2013   dsp     Added hdrhitmsg_set_pn_long_code_mask().
12/19/2012   smd     Added RUMI Tx support.
07/27/2012   rmg     Added support for configuring RL funnel mode.
06/08/2012   dsp/rmg Changed hdrhitmsg_process_enable_modulator()
                     to hdrhitmsg_process_modulator_ctl().
05/13/2012   dsp/rmg Added hdrhitmsg_process_get_fwd_lnk_stats(), 
                     Added hdrhitmsg_process_reset_fwd_lnk_stats(),
                     Added hdrhitmsg_process_enable_modulator(). 
04/06/2012   dsp     Added  hdrhitmsg_process_hard_handoff().
02/27/2012   smd     Added updates of FTM msgr interface changes.
08/29/2011   smd     Included hdrovhd.h.
06/15/2011   smd     Created

==========================================================================*/




/*==========================================================================
                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "comdef.h"
#include "hdrhit_msg.h"
#include "hdrovhd.h"

/*==========================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/
/* Must be consistent with the max carriers supported 
   by RMAC4 driver code */
#define MAX_NUM_REV_PHY_CHANS 3

/* HDR MSG Queue size */
#define HDRHITMSG_QUEUE_SIZE  5

#ifdef FEATURE_HDR_RUMI_TX_ONLY
#define HDRHITMSG_FLEX_ADDRESS 0xEC18F800
#endif /* FEATURE_HDR_RUMI_TX_ONLY */

/* Datatype for the hit message queue */
typedef union
{
  msgr_hdr_struct_type          hdr;
  hdrhit_msg_cmd_u              hdrhit_cmd_msg;

} hdrhitmsg_msg_u;

typedef struct
{
  q_link_type                    qlink;
  hdrhitmsg_msg_u                msg;
} hdrhitmsg_s_type;

typedef struct
{
  hdrhitmsg_s_type           q_items[ HDRHITMSG_QUEUE_SIZE ];  
    /* Array for the queue */

  q_type                     msg_q;  
    /* Message queue */

  q_type                     free_q;
    /* Free queue */
} hdrhitmsg_queue_type;


typedef struct
{
    hdrhitmsg_hdr_ftm_state_enum_type     fwd_curr_state;     /* Current HDR FTM state on the forward link */
    hdrhitmsg_hdr_ftm_state_enum_type     rev_curr_state;     /* Current HDR FTM state on the reverse link */

} hdrhitmsg_hdr_ftm_state_t;


typedef struct
{
    uint8                          mac_index;
    uint8                          rablength;
    uint8                          raboffset;
    uint8                          drclocklength;
    uint8                          drclockperiod;
    uint8                          drc_cover;
    uint8                          dsc_value;
} hdrhitmsg_fwd_link_info_t;


typedef struct
{
	int8                            open_loop_adjust;
	int8                            probe_initial_adjust;
	uint8                           access_cycle_duration;
	uint8                           power_step;
	uint8                           probe_num_step;
	uint8        	                preamble_len;
	uint8							enhanced_acc_enable;
	uint8							preamble_len_slots;
	uint8							access_offset;
	uint8  							sector_max_rate;
	uint8							terminal_max_rate;
	uint8							access_max_rate;
} hdrhitmsg_access_params_t;


typedef struct
{
	uint8								power_params[6];
	uint32								tx_duty_cycle;
	uint8								tx_rri;
	uint16								tx_payload;
	uint8								tx_rri_reva;
	uint16								tx_payload_reva;
	uint8  								gain_transition_point;
	uint8                               modem_fwd_rev_link_type;	
	uint8								revb_gain_transition_point;	
	uint8								t2p_pre_transition_revb_c[MAX_NUM_REV_PHY_CHANS];
	uint8								t2p_post_transition_revb_c[MAX_NUM_REV_PHY_CHANS];
	uint8								tx_rri_revb_c[MAX_NUM_REV_PHY_CHANS];
	uint16 								tx_payload_revb_c[MAX_NUM_REV_PHY_CHANS]; 
	uint16 								revb_closed_loop_c[MAX_NUM_REV_PHY_CHANS];
	uint8					 			t2p_hicap_revb[2][13]; 									/* [pre/post][rri] */
	uint8 								t2p_lolat_revb[2][13]; 									/* [pre/post][rri] */
	int8 								rri_gain_revb[2][4];									/*[pre/post][gain trans point]*/
	uint8								rpc_enable;
	uint8								rpc_step;
	uint8								revb_num_carriers;
	uint8								revb_single_fwd_link_mode;
	uint8 								revb_drc_gating;
	uint8								revb_drc_length;
	uint8								revb_drc_cover;
	uint8 								revb_arq_mode;
	uint8  								revb_ack_mode; /* default IS890-1 */
	uint8 								revb_auxpil_payload_thresh;
	uint8 								revb_dsc_length;
	uint8								revb_dsc_value;
	uint8								revb_frame_offset;
	uint32 								revb_rtc_pattern;
	uint32 								revb_rtclc_mask_i_hi;
	uint32 								revb_rtclc_mask_i_lo;
	uint32 								revb_rtclc_mask_q_hi;
	uint32 								revb_rtclc_mask_q_lo;
	uint8  								hicap_lolatency;
	uint8								sht_pkt_enabled;
	uint8								mup_enabled;
	uint8  								t2p_pre_transition;  
	uint8  								t2p_post_transition; 
	uint8  								arq_mode;
	uint8  								auxpil_payload_thresh;
	uint8  								auxpil_chan_gain;
	int8  								rri_ch_gain_pre_transition;
	int8  								rri_ch_gain_post_transition;
	uint8  								dsc_length;
	uint32 								rtc_pattern;
} hdrhitmsg_rmac_params_t;


typedef struct
{
	hdrhitmsg_access_params_t           access_params;
	hdrhitmsg_rmac_params_t	            rmac_params;
	hdrhitmsg_fwd_link_info_t           fwd_link_info_rev0;  /* Initializing with default values */
	hdrhitmsg_fwd_link_info_t           fwd_link_info_reva;  /* Initializing with default values */
	hdrhitmsg_fwd_link_info_t           fwd_link_info_revb;  /* Initializing with default values */
        uint8                               drc_value;
	uint8                               mode_type_rev0;
	uint8                               mode_type_reva;
	uint8                               mode_type_revb;
	uint8                               first_time_rev_link_rev0;
	uint8                               first_time_rev_link_reva;
	uint8                               first_time_rev_link_revb;
	boolean                             ftm_not_doing_hho;
} hdrhitmsg_ftm_intf_t;

/*===========================================================================

                 EXTERNAL DEFINITIONS AND DECLARATIONS

This section contains external definitions for constants, macros, types,
variables and other items.

===========================================================================*/

/*--------------------------------------------------------------------------
                         Constant Definitions
--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                          Type and Variables 
--------------------------------------------------------------------------*/


/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*=========================================================================

FUNCTION     : HDRHITMSG_INIT_GLOBAL_VARS

DESCRIPTION  : This function initializes the appropriate fields of
			   hdrhitmsg_ftm_intf with their corresponding values.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrhitmsg_init_global_vars( void );

/*=========================================================================

FUNCTION     : HDRHITMSG_INIT

DESCRIPTION  : This function creates MSGR clients and queues and 
               registers the messages with MSGR.

DEPENDENCIES : None

INPUTS       : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

void hdrhitmsg_init( void );

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_RCVD_MSGS

DESCRIPTION
  This function processes messages received by hit.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_rcvd_msgs( void );


/*=========================================================================

FUNCTION HDRHITMSG_RESET_HDR_MODE_REV_LINK  

DESCRIPTION
  This function sets the values of mode_type and first_time_rev_link to 0.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_reset_hdr_mode_rev_link ( void );


/*===========================================================================

FUNCTION HDRHITMSG_INIT_MSG

DESCRIPTION
  Initialize message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/ 
void hdrhitmsg_init_msg
(
	hdrhit_msg_rsp_u *            p_hdrhit_rsp_msg,
	  /* Pointer to message */
	msgr_umid_type                msg_id	
	  /* Message ID */
);

/*===========================================================================

FUNCTION HDRHITMSG_SEND_MSG

DESCRIPTION
  Send message.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/ 
void hdrhitmsg_send_msg
(
	hdrhit_msg_rsp_u*          rsp_msg,
	  /* message to be sent */
	uint32                       len
	  /* message length */
);


/*===========================================================================

FUNCTION HDRHITMSG_ACQUIRE_CALLBACK

DESCRIPTION
  HDR Search callback funtion. Update FTM state and send task
  signal indicating successful acquisition.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/ 
void hdrhitmsg_acquire_callback
(
  const sys_channel_type *channel,
    /* Channel acquisition was attempted on */

  hdrsrch_acquire_enum_type     acq_result
    /* Acquisition success or failure status */
);


/*===========================================================================

FUNCTION HDRHITMSG_SYSTEM_LOST_CALLBACK

DESCRIPTION
  HDR Search callback funtion.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/ 

void hdrhitmsg_system_lost_callback 
(
  hdrsrch_system_lost_enum_type   reason
    /* System lost explanation */
);


/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_PIL_ACQ

DESCRIPTION
  This function processes PIL_ACQ message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_pil_acq
(
    hdrhitmsg_s_type         *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_SYNC

DESCRIPTION
  This function attempts to acquire a pilot.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_sync
(
    hdrhitmsg_s_type         *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_DEMOD_FTC

DESCRIPTION
  This function attempts to acquire a pilot.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_demod_ftc
(
    hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_MOD_ACC

DESCRIPTION
  This function starts the access probes for Rev0.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_acc
(
    hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_MOD_RTC

DESCRIPTION
  This function starts the reverse line trafific channel for Rev0.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc
(
    hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_DEACTIVATE_DONE_CB

DESCRIPTION
  Callback function for L1.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_deactivate_done_cb( void );


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_DEASSIGN

DESCRIPTION
  This function turns off HDR traffic channel .

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_deassign
(
	hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_FTM_GET_AP_MSG

DESCRIPTION
  This function sets the required value for the fields of ap_msg.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_ftm_get_ap_msg
( 
  hdrovhd_ap_msg_type * ap_msg
);


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_IDLE_STATE

DESCRIPTION
  This function configures the MAC for Rev A.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_idle_state
(
	hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_TC_ASSIGN_COMPLETE_CB

DESCRIPTION
  Callback function passed to HDR TC assignment command.

DEPENDENCIES

RETURN VALUE
  Will be called when HDR traffic channel is assigned.

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_tc_assign_complete_cb ( hdrsrch_tca_result_type result );


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_DEMOD_FW_LINK_REV0

DESCRIPTION
  This function satrts the demodulation of forward link for Rev0 without 
  reverse link transmitting.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_demod_fw_link_rev0
(
	hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_RELA_MAC_CONFIG

DESCRIPTION
  This function configures the MAC for Rev A.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_rela_mac_config
(
	hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION HDRHITMSG_PROCESS_DEMOD_FW_LINK_REVA

DESCRIPTION
  This function satrts the demodulation of forward link without reverse 
  link transmitting for RevA.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_demod_fw_link_reva
(
	hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_ACC_REVA

DESCRIPTION
  This function satrts the access probe sequence for RevA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_acc_reva
(
	hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_RTC_REVA

DESCRIPTION
  This function satrts the reverse traffic channel for RevA

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc_reva
(
	hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_MOD_RTC_REVB

DESCRIPTION
  This function satrts the reverse traffic channel for RevB

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_mod_rtc_revb
(
	hdrhitmsg_s_type *msg
);


/*===========================================================================

FUNCTION  HDRHITMSG_PROCESS_CONFIG_RTC_REVB

DESCRIPTION
  This function satrts the reverse traffic channel for RevB

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_process_config_rtc_revb
(
	hdrhitmsg_s_type *msg
);

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_HARD_HANDOFF

DESCRIPTION
  This function performs hard handoff.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void hdrhitmsg_process_hard_handoff
(
    hdrhitmsg_s_type         *msg
);

/*===========================================================================

FUNCTION  HDRHITMSG_GET_FWD_CURR_STATE

DESCRIPTION
  This function gives the value of the current HDR FTM state on the forward link.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

hdrhitmsg_hdr_ftm_state_enum_type hdrhitmsg_get_fwd_curr_state ( void );


/*===========================================================================

FUNCTION  HDRHITMSG_SET_FWD_CURR_STATE

DESCRIPTION
  This function sets the value of the current HDR FTM state on the forward link.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/

void hdrhitmsg_set_fwd_curr_state
(
	hdrhitmsg_hdr_ftm_state_enum_type fwd_curr_state
);

/*===========================================================================

FUNCTION  HDRHITMSG_GET_RMAC_PARAMS_PTR

DESCRIPTION 
	Returns pointer rmac_params field of hdrhitmsg_ftm_intf. 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
	None.

===========================================================================*/

hdrhitmsg_rmac_params_t * hdrhitmsg_get_rmac_params_ptr ( void );

/*===========================================================================

FUNCTION  HDRHITMSG_GET_FTM_INTF_PTR

DESCRIPTION 
	Returns pointer to hdrhitmsg_ftm_intf global variable. 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
	None.

===========================================================================*/

hdrhitmsg_ftm_intf_t * hdrhitmsg_get_ftm_intf_ptr ( void );

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_HARD_HANDOFF

DESCRIPTION
  This function performs hard handoff.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_process_hard_handoff
(
    hdrhitmsg_s_type         *msg
);

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_MODULATOR_CTL

DESCRIPTION
  This function enables/disables tx on carriers as specified by the input.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_process_modulator_ctl
(
    hdrhitmsg_s_type         *msg
);


/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_CONFIG_FUNNEL_MODE

DESCRIPTION
  This function configures reverse link funnel mode as specified by 
  the input.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_process_config_funnel_mode
(
    hdrhitmsg_s_type         *msg
);

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_GET_FWD_LNK_STATS

DESCRIPTION
  This function gets forward link stats.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_process_get_fwd_lnk_stats
(
    hdrhitmsg_s_type         *msg
);

/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_RESET_FWD_LNK_STATS

DESCRIPTION
  This function resets forward link stats.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_process_reset_fwd_lnk_stats
(
    hdrhitmsg_s_type         *msg
);

/*=========================================================================

FUNCTION HDRHITMSG_SET_PN_LONG_CODE_MASK

DESCRIPTION
  Load the 42-bit I and Q long code masks into the modulator.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_set_pn_long_code_mask( void );

#ifdef FEATURE_HDR_RUMI_TX_ONLY
/*=========================================================================

FUNCTION HDRHITMSG_PROCESS_SLAM_RSP

DESCRIPTION
  This function process slam response.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrhitmsg_process_slam_rsp( uint16 hstr_at_slam );


/*=========================================================================

FUNCTION HDRHITMSG_GET_DATA_PKT

DESCRIPTION
  This function returns the pointer to data packet

DEPENDENCIES
  None

RETURN VALUE
  Pointer to data packet.

SIDE EFFECTS
  None

=========================================================================*/
uint8 * hdrhitmsg_get_data_pkt( void );

#ifdef FEATURE_HDR_RUMI
/*=========================================================================

FUNCTION HDRHITMSG_GET_DATA_PKT_AT_POS

DESCRIPTION
  This function returns the data at given position from RUMI static
  data pattern

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
uint8 hdrhitmsg_get_data_pkt_at_pos
( 
  uint16 pos
);
#endif /* FEATURE_HDR_RUMI

/*===========================================================================

FUNCTION HDRHITMSG_TIME_VALID

DESCRIPTION
  This Interrupt Service Routine completes the time synchronization.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  There will be a time discontinuity.
  The ISR auto uninstalls itself.

===========================================================================*/
void hdrhitmsg_time_valid( void );

/*===========================================================================

FUNCTION HDRHITMSG_START_FLEX_CAPTURE

DESCRIPTION
 This function is to trigger flex capture.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void hdrhitmsg_start_flex_capture( void );

#endif /* FEATURE_HDR_RUMI_TX_ONLY */

#ifdef FEATURE_HDR_RUMI
typedef struct
{
/* FEATURES SUPPORTED */
  boolean y1y2_support;
  boolean tx_only_support;

/* HACKS/WORKAROUND NEEDED FOR RUMI */
  boolean force_mcpm_cfg;
    /* needed if not using mcpm_init script */

  boolean clock_on_during_sleep;
    /* needed to test sleep/wakeup with PBS streaming */

  boolean force_trm_grant;
    /* by pass TRM grant. hack needed due to TRM denial issue */

  boolean verbose_debug;
    /* Verbose F3 */

  boolean sram_check;
    /* sanity check if SRAM is refreshing */

  uint16 sync_pn_offset;
    /* pn offset of the sample (in unit of 64 chips)_*/

  uint8 slot_cycle;
    /* slot cycle index */

  uint8 eq_ctrl;
    /* Equalizer NV control bit fields */
    /* Bit 0:   Eq enable/disable
                0  - enable (default)
                1  - disable
       Bit 2:1  Demod device selection 
                00 - Best of Rake/Eq, DSA on, use gRICE config (default) 
                01 - Rake always, DSA on, gRICE disabled
                10 - Eq always, DSA on, gRICE disabled
                11 - Individual Equalizer select, best C/I mode on, gRICE disabled
       Bit 4:3  When Equalizer is enabled with equalizer select (ie, bits 2:0==110)
                Equalizer select
                00 - Eq0
                01 - Eq1
                10 - Eq2
                11 - Eq3         
       Bit 4:3: When Equalizer is enabled and in default (ie, bits 2:0 == 000)
                gRICE config
                00 - enable
                01 - disable
                10 - reserved
                11 - reserved
    */

  uint8 div_ctrl;
    /*  bit 0:   div enabled in general
        Bit2:1 : 00 : Automatic Mode
                 01 : MRC
                 10 : MMSE
                 11 : Selection
        Bit4:3 : 00 : Automatic Antenna selection
                 01 : Ant0
                 10 : Ant1
        bit 6:   access div
        bit 7:   idle div */

  uint8 div_on_hack;
    /* FW hack to trigger PRI/DIV on at the same time
       0: disabled. DIV turns on by legacy call follow
       1: SW calls enter_mode/enable_div after PRI is enabled */

  uint8 afc_override;
    /* Override/disable FLL update:
       bit 0: disable MND update
       bit 1: disable FLL enable
       bit 2: disable ROT override */

  uint8 pri_chain_override;
    /* RF Dev for PRI chain. Only used if TRM returned invalid device */

  uint8 div_chain_override;
    /* RF Dev for PRI chain. Only used if TRM returned invalid device */

#ifdef FEATURE_HDR_REVB_SUPPORTED
  uint8 revb_carriers;
  uint8 revb_drc[3];
#endif /* FEATURE_HDR_REVB_SUPPORTED */

/* SHARED VARIABLES FOR RUMI */
  uint16 cc_pkt_hstr;
    /* HSTR of the CC pkt. Not a configurable value */

  uint8 div_enable_stage;
    /* 0: disabled
       1: enter mode called
       2: enabled */
} hdrhit_rumi_cfg_type;

extern volatile hdrhit_rumi_cfg_type hdrhit_rumi_cfg;
#endif /* FEATURE_HDR_RUMI */
#endif /* _HITMSG_H */
