#ifndef HDRFMAC_H
#define HDRFMAC_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

         H D R   F O R W A R D   T R A F F I C   C H A N N E L   M A C   
       
                               P R O T O C O L  
                                    
                   E X T E R N A L   H E A D E R   F I L E       


GENERAL DESCRIPTION
  This file contains the public definitions and interfaces for the HDR 
  Forward Traffic Channel Media Access Control (MAC) protocol.

EXTERNALIZED FUNCTIONS
  hdrfmac_init
    Initialize the protocol.
    
  hdrfmac_activate
    Activate the protocol.

  hdrfmac_deactivate
    Deactivate the protocol.

  hdrfmac_is_active
    Determine if the protocol is active.
    
  hdrfmac_put_pkt_chain
    Process a received packet.
    
  hdrfmac_update_link_status   
    Update value of DRCLock and ForwardChannelValid fields.
    
  hdrfmac_fac_gain_check_and_suspend
    Check and suspend FAC gain calculation before going to traffic suspend
    state.
 
  hdrfmac_fac_gain_check_and_resume
    Check and resume FAC gain calculation after returning from traffic suspend
    state.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The protocol must be initialized after system startup prior to activating 
  the protocol.  The procotol must be activated before Control Channel 
  packets can be processed.

  Copyright (c) 2000 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* EJECT */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: O:/src/asw/COMMON/vcs/hdrfmac.h_v   1.19   30 Jan 2003 16:22:44   vramaswa  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/mac/api/hdrfmac.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/25/15   ljl     Fixed compiler warning.  
02/05/15   wsh     Fixed compile warning 
03/10/14   rmv     Added API that returns whether jumpstart FAC gain is computed
11/14/13   cnx     Check FT valid until TCAssign is complete.
05/15/13   rmv     Added updates for Cost of Modem Power API support 
08/23/12   smd     Added updates for Device only DTX and FlexCONN.
06/20/12   smd     Added Device only DTX and FlexCONN.
06/21/12   vlc     Added check for slot interrupt processing when suspending 
                   and  eresuming traffic state. 
03/08/12   wsh     Merged NLB support
10/24/11   smd     Moved more to SU API header files.
07/22/11   arm     Added support for memory FC logging. 
04/27/11   lyl     Supported HDR FL data driven adaptive RxD. 
03/22/11   arm     Added support for thermal pa backoff feature.
05/27/10   kss     Added API to return drc_length for given carrier.
05/18/10   rmg     Added DRC supervision timer management to FMAC.
11/21/08   etv     Added support for flow control (graceful degradation).
05/29/08   etv     Added support for CBUFF based flow control.
01/10/08   etv     Added a new flow control enum for SharedMemoryInterace.
01/08/08   etv     Added API hdrfmac_serving_sector_locked_across_sub_asets.
09/21/07   kss     Include file cleanup.
09/10/07   etv     Moved in hdrfmac_tcam_params_type from hdrfmaci.h.
08/28/07   etv     Updated hdrfmac_set_channel_params interface for Rev-B
08/14/07   etv     Defined Rev-B TCAM params type.
07/28/07   etv     Changed hdrfmac_put_pkt to hdrfmac_put_pkt_chain.
07/24/07   etv     Removed FEATURE_HDRRX_YIELD.
06/19/07   cc      Added hdrfmac_configure() declaration.
06/30/06   etv     Supported DSP Handoff algorithm changes.
05/12/06   etv     Added HDRFMAC_ prefix to global macro.
11/04/05   ksu/sy  Delete 5500-specific hdrfmac_rlp_reg_srvc().
09/06/05   etv     Created library for enhanced subtype.
08/31/05   etv     Updated hdrfmac_set_drc_value to return boolean.
07/26/05   etv     Added support for Muxing based on packet type and filtering
                   SU/MU packets with invalid MAC trailers. 
07/22/05   etv     Added FTAP interfaces and added an interface to set MAX 
                   DRC value based on negotiated FMAC subtype.
05/17/05   etv     Removed const keyword from hdrfmac_update_link_status, since
                   qword API doesn't support const parameters.
05/16/05   dna     dsc_channel_gain is unsigned, and chan_valid_bits are const
05/13/05   etv     Added support for Rev-A and moved hdrrx_put_chain to FMAC.
04/24/04   kss     Added API for checking if flow control is currently enabled.
01/30/03    vr     Added DRC REQ MASK for MAC layer flow control of base stn.
11/06/02    vr     Added support for CPU based flow control
06/03/02   mpa     Mainlined FEATURE_HDR_7_4_DDR.
03/08/02   kss     Added hdrfmac_set_ack_mode() (moved from hdrrx -- here we
                   can make sure that the DSP is active before enabling).
02/11/02   ss      Changed the declarations of hdrfmac_disable/enable_flow_
                   control functions. Changed hdrfmac_drc_req_mask to uint32.
12/09/01   vr      Added prototypes for hdrfmac_disable/enable_flow_control
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
08/24/01   kss     Changed init function name, added DRC supervision function.
08/21/01   kss     Added indication function, externalized DRC values.
06/07/01   kss     Added frame_offset to hdrfmac_set_channel_params();
                   removed the set drc length function.
05/08/01   kss     Added hdrfmac_set_channel_params().
11/03/00   dna     Added HDR event logging
03/30/00   st      Created.

===========================================================================*/

/* EJECT */
/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "customer.h"
#include "comdef.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrmac.h"
#include "hdrind.h"
#include "hdrsrchtypes.h"
#include "hdrerrno.h"
#ifdef FEATURE_HDR_THERMAL_BACKOFF
#include "ds707_ccm.h"
#endif /* FEATURE_HDR_THERMAL_BACKOFF */

/* EJECT */
/*===========================================================================
                   
                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions and declarations for constants, macros,
types, variables and other items needed by this module.

===========================================================================*/

/* DRC supervision constants and timer values */

/* The number of frames to wait for a non-null DRC (12 control cycles) */
#define HDRFMAC_DRC_RESTART_TX_FRAMES         192

#ifdef FEATURE_HDR_REVC
/*---------------------------------------------------------------------------
                       LOAD INFORMATION MESSAGE                      
---------------------------------------------------------------------------*/

/* Message ID value */
#define HDRFMAC_LOAD_INFO_MSG_ID     0x02

/* Message Field Size*/
#define HDRFMAC_LOAD_ADJ_LEN         5

#define HDRFMAC_NEIGHBOR_COUNT_LEN   5
#endif /* FEATURE_HDR_REVC */

/*--------------------------------------------------------------------------- 
  Mask for FL flow control using NULL cover. NULL cover will be set if any
  bit is set. It will be removed only if all bits are unset.
  Allocate a bit here for each entity that needs to flow control the FL.
  Current entities are the DSM small item pool watermark and the ps task
  ppp receive queue.
---------------------------------------------------------------------------*/
extern uint32 hdrfmac_drc_req_mask; 

/* When this value is used in hdrsrch_at_set_drc function for drc, it sets
 * it to variable rate DRC */
#define HDRFMAC_VAR_RATE_DRC          0x0F

/* NULL DRC Cover */
#define HDRFMAC_DRC_NULL_COVER        0x00

/* If Test App sends a DRC cover of 0x0F, FMAC handoff control to DSP */
#define HDRFMAC_HANDOFF_CTRL_TO_DSP   0x0F

#ifdef FEATURE_HDR_MODEM_API
/* Invalid Channel gain values (DRC,Ack gain)to represent failure to
   retrieve them from forward traffic channel assignment message */
#define HDRFMAC_CHAN_GAIN_INVALID     0x80
#endif /* FEATURE_HDR_MODEM_API */


#ifdef FEATURE_HDR_REVB
/* This struct encapsulates parameters of a sub-active set pertaining to 
   feedback multiplexing. Please note that FeedbackMultiplexing is not 
   supported for Phase-1. */
typedef struct
{

  boolean feedback_enabled;
    /* Should AT send DRC/DSC/ACK chans associated with this sub-active set */

  uint16 feedback_mux_index;
    /* Specifies the LongCodeMask, DRCCoverOffset and FeedbackMuxMethod */

  uint16 feedback_rev_chan_index;
    /* Rev-link CDMA channel on which the DSC, DRC and ACK channels are sent */

  boolean dsc_enabled;
    /* Is DSC enabled for this sub-active-set? */

} hdrfmac_sub_aset_feedback_mux_params_type;

#endif /* FEATURE_HDR_REVB */

/* This struct encapsulates all the per-sub-active set fields in the Traffic 
   channel assignment message. The demod_id in the struct is the index 
   generated by SRCH after re-arranging the Sub-aset parameters in TCAM. */
typedef struct
{

  uint8 demod_id;
    /* Physical Demod block ID */

  hdrmac_drc_len_type drc_length;
    /* Number of slots AT shall use to transmit a single DRC value */

  int8 drc_channel_gain;
    /* Power of DRC channel relative to the pilot channel in units of 
       [0.5, 0.25]dB. Please note that when RUP/LMAC copies to this field from 
       the parsed value of TCA, the units is 0.5dB. However, when this is 
       copied over to FMAC's instance of the struct, the unit is converted 
       to 0.25dB */

  int8 ack_channel_gain;
    /* Power of ACK channel relative to the pilot channel in units of 
       [0.5, 0.25]dB. Please note that when RUP/LMAC copies to this field from 
       the parsed value of TCA, the units is 0.5dB. However, when this is 
       copied over to FMAC's instance of the struct, the unit is converted 
       to 0.25dB */

#ifdef FEATURE_HDR_REVB

  hdrfmac_sub_aset_feedback_mux_params_type fm;
    /* Feedback Multiplexing parameters */

#endif /* FEATURE_HDR_REVB */

} hdrfmac_per_sub_aset_tca_info_type;

/* This struct encapsulates parameters pertaining to FTCMAC from traffic 
   channel assignment message. */
typedef struct
{
  int8 dsc_channel_gain;
    /* Power of DSC Channel relative to the pilot channel in units of 
       [-0.5, 0.25]dB. Please note that when RUP/LMAC stores in this 
       field after parsing from TCA, the units is in -0.5dB. However, 
       when this is copied over to FMAC's instance the units is converted 
       to 0.25dB */

  uint8 frame_offset;
    /* FrameOffset in slots that AT shall use in transmitting on RTC */

#ifdef FEATURE_HDR_REVB
  boolean feedback_mux_enabled;
    /* Is Feedback Multiplexing enabled? */
#endif /* FEATURE_HDR_REVB */

  uint8 num_carriers;
    /* Number of sub-active-sets */

  hdrfmac_per_sub_aset_tca_info_type sub_aset [HDRSRCH_MAX_NUM_CARRIERS];
    /* Per sub-aset parameters */

} hdrfmac_tcam_params_type;

/* This struct encapsulates all the FT Valid bits from the QuickConfig 
   message. */
typedef struct
{
  qword chan_valid_bits_63_to_0;
    /* QuickConfig.ForwardChannelValid field bits 0 through 63 */
  qword chan_valid_bits_127_to_64;
    /* QuickConfig.ForwardChannelValid field bits 64 through 127 */
  qword chan_valid_bits_383_to_130;
    /* QuickConfig.ForwardChannelValid field bits 130 through 383 */

} hdrfmac_qc_ft_valid_bits_type;


#ifdef FEATURE_HDR_FL_FLOW_CONTROL

/* This struct encapsulates the FW params configured by CCM */
typedef struct {
  hdrfmac_flow_control_pattern_enum_type initial_pattern;
  /* Initial pattern to start with after the first DOWN command in OFF 
     state */

  hdrfmac_flow_control_pattern_enum_type max_reduction_pattern;
  /* This is the pattern number that is associated with the largest FL 
     bandwidth reduction allowed in flow control. */

  uint16 period_ms;
    /* Step timer period in milliseconds */

} hdrfmac_flow_control_params_type;

#endif /* FEATURE_HDR_FL_FLOW_CONTROL */

#if defined(FEATURE_HDR_FL_DATA_ADAPTIVE_RXD)||defined(FEATURE_HDR_FLEX_CONN)|| \
    defined(FEATURE_HDR_DEVICE_ONLY_DTX)

typedef void (*hdrfmac_fl_active_cb_type)( boolean );
  /* FMAC FL active call back type */

typedef struct {

#ifdef FEATURE_HDR_FL_DATA_ADAPTIVE_RXD
  hdrfmac_fl_active_cb_type ard_fl_active_cb;
    /* FL activity callback function pointer for ARD */
#endif /* FEATURE_HDR_FL_DATA_ADAPTIVE_RXD */

#ifdef FEATURE_HDR_FLEX_CONN
  hdrfmac_fl_active_cb_type flexconn_fl_active_cb;
    /* FL activity callback function pointer for FlexConn */
  boolean flexconn_fl_inactive_is_notified;
    /* Whether flexconn client has been notified with FL inactive */
#endif /* FEATURE_HDR_FLEX_CONN */

#ifdef FEATURE_HDR_DEVICE_ONLY_DTX
  hdrfmac_fl_active_cb_type dtx_fl_active_cb;
    /* FL activity callback function pointer for DTX */
  boolean dtx_fl_inactive_is_notified;
    /* Whether DTX client has been notified with FL inactive */
#endif /* FEATURE_HDR_DEVICE_ONLY_DTC */

} hdrfmac_fl_active_cb_list_type;

/*  Enum type of FL activity callback clients */
typedef enum
{
	HDRFMAC_FL_ACTIVE_CB_ARD,
	HDRFMAC_FL_ACTIVE_CB_FLEXCONN,
	HDRFMAC_FL_ACTIVE_CB_DEVICE_DTX
} hdrfmac_fl_active_cb_client_type;
#endif /* defined(FEATURE_HDR_FL_DATA_ADAPTIVE_RXD)||
         defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/


#ifdef FEATURE_HDR_REVC
#define HDRFMAC_ASET_PER_CHAN 6
/*---------------------------------------------------------------------------
                       LOAD INFORMATION DATA                      
---------------------------------------------------------------------------*/
typedef enum 
{
  HDRFMAC_LOADADJ_CACHE_INVALID,
  HDRFMAC_LOADADJ_CACHE_WAITING_OVHD,
  HDRFMAC_LOADADJ_CACHE_OVHD_RCVD
}
hdrfmac_loadadj_cache_state_enum_type;

typedef struct 
{ 
  hdrfmac_loadadj_cache_state_enum_type state;
  time_type timestamp;
  sys_link_type serving_sector;

  uint8 loadadj_serving;

  uint8 loadadj_cnt;
  uint8 loadadj[HDRHAI_MAX_SP_NEIGHBORS];
} hdrfmac_loadadj_cache_type;
#endif /* FEATURE_HDR_REVC */

/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_POWERUP_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup. 
  
DEPENDENCIES
  None.
  
PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_powerup_init ( void );


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_IS_ACTIVE

DESCRIPTION
  This function queries the current state of the protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  caller - The protocol calling this externalized function (for sw trace)

RETURN VALUE
  TRUE  = protocol is Active.
  FALSE = protocol is Inactive.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern boolean hdrfmac_is_active ( hdrhai_protocol_name_enum_type caller );


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_ACTIVATE

DESCRIPTION
  This command transitions the protocol to the Variable Rate state.  If the 
  protocol is already in the Variable Rate or Fixed Rate state then no action 
  is taken.
  
DEPENDENCIES
  The protocol must be initialized by calling the hdrfmac_init function prior
  to activation.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_activate ( hdrhai_protocol_name_enum_type sender );


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_DEACTIVATE

DESCRIPTION
  This command transitions the protocol to the Inactive state.  If the 
  protocol is already in the Inactive state then no action is taken.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_deactivate ( hdrhai_protocol_name_enum_type sender );


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrfmac_msg_cb 
( 
  dsm_item_type *msg_ptr
);


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_CONFIG_MSG_CB

DESCRIPTION
  This callback function queues incoming messages for the in config instance
  of this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - pointer to receive message buffer.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrfmac_config_msg_cb 
( 
  dsm_item_type *msg_ptr
);


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_UPDATE_LINK_STATUS

DESCRIPTION
  This function alerts the protocol to an update of the ForwardTrafficValid 
  field received in the QuickConfig message.  A ForwardTrafficValid bit with 
  a value of '0' will cause the protocol to give the SupervisionFailed 
  indication and deactivate. 
  
DEPENDENCIES
  None.

PARAMETERS
  chan_valid_bits_63_to_0   - QuickConfig.ForwardChannelValid field.
  chan_valid_bits_127_to_64 - QuickConfig.ForwardChannelValid field.
  link_id                   - Link ID for QuickConfig message.
  sender                    - Name of protocol giving calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_update_link_status 
( 
  qword chan_valid_bits_63_to_0,
  qword chan_valid_bits_127_to_64,
  uint8 link_id,
  hdrhai_protocol_name_enum_type sender
);


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_SET_RAB_LENGTH

DESCRIPTION
  This function alerts the protocol to an update of the RABLength field in
  the TrafficChannelAssignment message.  The modem hardware is updated to 
  use the new RAB length when receiving the RAB channel.
  
DEPENDENCIES
  None.

PARAMETERS
  rab_len - TrafficChannelAssignment.RABLength field.
  sender - Name of protocol giving calling this function.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_set_rab_length
(
  uint8 rab_len,
  hdrhai_protocol_name_enum_type sender
);


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_PUT_PKT_CHAIN

DESCRIPTION
  This function breaks a rel-0 forward traffic channel packet from a chain of 
  DSM items, parses the MAC trailer and then forwards the connection layer 
  format information along with the enclosed Security Layer packet to the 
  Security Layer for processing.

  Note: No security layer support yet! So forwarded to Packet Consolidation 
  Protocol (Connection Layer).
  
DEPENDENCIES
  None.

PARAMETERS
  rx_pkt_chain - pointer to buffered Forward Traffic Channel packet chain.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_put_pkt_chain
( 
  dsm_item_type *rx_pkt_chain
);


/*===========================================================================

FUNCTION HDRFMAC_SET_CHANNEL_PARAMS                                 EXTERNAL

DESCRIPTION
  This function sets the FMAC channel parameters drc_channel_gain, 
  ack_channel_gain, and drc_length.
  If the negotiated FMAC subtype is enhanced, this function also sets the DSC
  channel gain, DSC length, DSC timing registers namely dscHalfSlotCount 
  and dscHalfSlotTrigger.

DEPENDENCIES
  The parameter must not be NULL.
  
PARAMETERS
  tca_params   - The FMAC channel parameter structure including parameters
                 received over Traffic channel Assignment message namely,
                    * DRC Length
                    * DRC Channel Gain
                    * Ack Channel Gain
                    * DSC Channel Gain ( only for Enhanced FMAC )
  frame_offset - Frame Offset AT shall use in transmitting the reverse traffic
                 channel in slots.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  The Fwd channel params are written to correponding DSP registers. 
===========================================================================*/
extern void hdrfmac_set_channel_params
(            
  hdrfmac_tcam_params_type *tca_fwd_chan_params
);


/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_PROCESS_IND 

DESCRIPTION
  This function processes indications given to FMAC.
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for FMAC to process
  ind_data_ptr - pointer to data associated with the indication
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrfmac_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRFMAC_IND_CB                                              EXTERNAL

DESCRIPTION
  This function processes incoming indications for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication from hdrhai.h
  ind_data_ptr - Pointer to any indication-specific data
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrfmac_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRFMAC_DRC_SUPERVISION_FAILURE                           EXTERNAL

DESCRIPTION
  The function is used to tell the FTCMAC that DRC supervision failure
  has occured, since the FTCMAC does not actually monitor this.
  
DEPENDENCIES
  None.

PARAMETERS
  None.  
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrfmac_drc_supervision_failure( void );


 /* EJECT */
/*===========================================================================
FUNCTION HDRFMAC_ENABLE_FLOW_CONTROL

DESCRIPTION
  This function will set a NULL cover for FL flow control. It maintains a
  bit mask indicating which entities have requested flow control. NULL cover
  will be set if at least one entity has requested flow control.
 
DEPENDENCIES
  None.

PARAMETERS
  mask - The entity requesting the flow control
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrfmac_enable_flow_control  
(
  uint32 mask
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRFMAC_DISABLE_FLOW_CONTROL

DESCRIPTION
  This function will remove NULL cover for FL flow control. It maintains a
  bit mask indicating which entities have requested flow control. NULL cover
  will be removed only if no entity has requested flow control.
  
DEPENDENCIES
  None.

PARAMETERS
  mask - The entity requesting the flow control disabling
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrfmac_disable_flow_control
(
  uint32 mask
); 


 /* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_SET_ACK_MODE                                       EXTERNAL

DESCRIPTION
  This function transitions the Rx packet processing into one of several
  ack test modes defined by the hdrfmac_ack_mode_enum_type enumeration.

DEPENDENCIES
  None

PARAMETERS
  ack_mode - The mode to enable.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrfmac_set_ack_mode
( 
  hdrfmac_ack_mode_enum_type ack_mode
);

/* EJECT */  
/*===========================================================================

FUNCTION HDRFMAC_SET_MC_ACK_MODE                                 EXTERNAL

DESCRIPTION
  This function transitions the Rx packet processing into one of several
  ack test modes defined by the hdrfmac_ack_mode_enum_type enumeration.

DEPENDENCIES
  None

PARAMETERS
  rl_carrier_id - Reverse link carrier id.
  ack_mode - The mode to enable.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrfmac_set_mc_ack_mode
( 
  uint8 rl_carrier_id,
  hdrfmac_ack_mode_enum_type ack_mode
);

 /* EJECT */
#ifdef FEATURE_DOG_FLOW_CONTROL
/*===========================================================================
FUNCTION HDRFMAC_DOG_FLOW_CONTROL_CB()

DESCRIPTION
  Will disable or enable the flow from the AN based on CPU
  usage. This will be called from the watchdog task.
  
DEPENDENCIES
  NONE

PARAMETERS
  boolean flow_enable - enable or disable the flow.

RETURN VALUE
  None. 

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrfmac_dog_flow_control_cb
(
  boolean flow_enable
);
#endif


 /* EJECT */
/*===========================================================================
FUNCTION HDRFMAC_IS_FLOW_CTRL_ENABLED()

DESCRIPTION
  Returns TRUE/FALSE depending on whether forward link flow control is 
  currently enabled.
  
DEPENDENCIES
  NONE

PARAMETERS
  None.

RETURN VALUE
  TRUE - the HDR forward link is currently being flow controled.
  FALSE - normal operation.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrfmac_is_flow_ctrl_enabled( void );


#if defined (FEATURE_HDR_IS890) || defined (FEATURE_HDR_IS890A)

 /* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_SET_DRC_COVER                                       EXTERNAL

DESCRIPTION
  This interface is used by FTAP to set AT's transmitted DRC cover (including
  NULL COVER). When a non-NULL cover is sent, the Test APP switches to 
  DRCCoverFixedMode. This function can also be used to switch out of 
  DRCCoverFixedMode operation by setting the drc_cover to 
  HDRFMAC_HANDOFF_CTRL_TO_DSP.

DEPENDENCIES
  None

PARAMETERS
  drc_cover - Could be set to one of the following values.
              1. HDRFMAC_DRC_NULL_COVER - Sets NULL cover
              2. HDRFMAC_HANDOFF_CTRL_TO_DSP - Switches out of 
                 DRCCoverFixedMode operation. DSP assumes control of choosing
                 the DRC cover. 
              3. Any other valid DRC cover values - would be mapped to 
                 asp_index and the uP assumes control and fixes the drc cover
                 to this mapped asp_index. 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfmac_set_drc_cover( uint8 drc_cover );

 /* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_SET_DRC_VALUE                                       EXTERNAL

DESCRIPTION
  This interface is used by FTAP to set AT's transmitted DRC value for 
  DRCValueFixedMode operation.

DEPENDENCIES
  None

PARAMETERS
  drc - DRC value to set.

RETURN VALUE
  TRUE If the given DRC is valid for current subtype.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrfmac_set_drc_value( uint8 drc );

 /* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_SET_MC_DRC_COVER                                 EXTERNAL

DESCRIPTION
  This interface is used by FTAP to set AT's transmitted DRC cover (including
  NULL COVER). When a non-NULL cover is sent, the Test APP switches to 
  DRCCoverFixedMode. This function can also be used to switch out of 
  DRCCoverFixedMode operation by setting the drc_cover to 
  HDRFMAC_HANDOFF_CTRL_TO_DSP.

DEPENDENCIES
  None

PARAMETERS
  drc_cover - Could be set to one of the following values.
              1. HDRFMAC_DRC_NULL_COVER - Sets NULL cover
              2. HDRFMAC_HANDOFF_CTRL_TO_DSP - Switches out of 
                 DRCCoverFixedMode operation. DSP assumes control of choosing
                 the DRC cover. 
              3. Any other valid DRC cover values - would be mapped to 
                 asp_index and the uP assumes control and fixes the drc cover
                 to this mapped asp_index. 

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrfmac_set_mc_drc_cover
( 
  uint8 rl_carrier_id,
  uint8 drc_cover 
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_SET_MC_DRC_VALUE                                EXTERNAL

DESCRIPTION
  This interface is used to set AT's transmitted DRC value. The given DRC is
  set if it is valid for the current negotiated FMAC subtype and ignored
  otherwise.

DEPENDENCIES
  None

PARAMETERS
  rl_carrier_id - Reverse link Carrier ID.
  drc - DRC value to set.

RETURN VALUE
  TRUE If the given DRC is valid for current subtype.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean hdrfmac_set_mc_drc_value
( 
  uint8 rl_carrier_id,
  uint8 drc 
);

#endif /* FEATURE_HDR_IS890 || FEATURE_HDR_IS890A */

 /* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_SET_TRAFFIC_MAX_DRC                                 EXTERNAL

DESCRIPTION
  This function is used by RMAC to ramp up the Max allowed DRC to 12 when 
  FMAC subtype is Default and 14 when FMAC subtype is enhanced, after 
  reception of RTC ACK.

DEPENDENCIES
  None

PARAMETERS
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfmac_set_traffic_max_drc( void );


/* <EJECT> */
/*===========================================================================

FUNCTION HDRFMAC_UPDATE_HANDOFF_REGISTERS

DESCRIPTION
  This function sets MaxCredit (and SofterHandoffCreditThresh) only if the 
  FMAC subtype is default. This function sets the Max credit based on soft 
  handoff delay if the ASET has sectors from more than one cell or sets Max 
  credit based on Softer Handoff delay if the ASET has sectors from exactly 
  one cell. For the latter case, it also adjusts softer HO credit threshold 
  for MPS testing. 

  Note: If the current subtype of FMAC is Enhanced, this function does nothing.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void hdrfmac_update_handoff_registers( void );


/* <EJECT> */
/*===========================================================================

FUNCTION HDRFMAC_CONFIGURE

DESCRIPTION
  This function is called by SCP to start AT-initiated configuration.

DEPENDENCIES
  None

PARAMETERS
  sender - The calling protocol.

RETURN VALUE
  TRUE if the configuration is required; FALSE if configuration is not
  needed.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrfmac_configure
(
  hdrhai_protocol_name_enum_type sender
);

/* <EJECT> */
#ifdef FEATURE_HDR_REVB

/* <EJECT> */
/*===========================================================================

FUNCTION HDRFMAC_SERVING_SECTOR_LOCKED_ACROSS_SUB_ASETS         EXTERNAL

DESCRIPTION
  Can AT select a different serving sector on each of the Active sets?
  
DEPENDENCIES
  None.

PARAMETERS
  locked - Pointer to the boolean where the result is stored.
           Set to TRUE for locked Mode and FALSE otherwise. 

RETURN VALUE
  E_SUCCESS     - If the session is open and current FMAC subtype is 
                  Multi-carrier FTCMAC (Subtype 2).
  E_NOT_ALLOWED - Otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
hdrerrno_enum_type hdrfmac_serving_sector_locked_across_sub_asets
( 
  boolean *locked
);

#endif /* FEATURE_HDR_REVB */

/*===========================================================================
FUNCTION HDRFMAC_ENTER_DRC_SUPERVISION

DESCRIPTION
  The function is used to the following when entering DRC supervision:
  - Gives enter DRC supervision indication to RTCMAC.
  - Starts DRC supervision restart timer for time T_FTCMPRestartTx 
    (= 12 control channel cycles = 5.12 seconds).
  
DEPENDENCIES
  None.

PARAMETERS
  phy_chan_mask - Physical channel ID mask
                  Set 'n'th bit to indicate that channel 'n' is in 
                  DRC supervision

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrfmac_enter_drc_supervision
(
  uint16 phy_chan_mask
);


 /* EJECT */
/*===========================================================================
FUNCTION HDRFMAC_EXIT_DRC_SUPERVISION                               EXTERNAL

DESCRIPTION
  The function is used to the following when exiting DRC supervision:
  - Resets DRC supervision restart timer.
  - Gives exit DRC supervision indication to RTCMAC.
  
DEPENDENCIES
  None.

PARAMETERS
  phy_chan_mask - Physical channel ID mask
                  Set 'n'th bit to indicate that channel 'n' is now out 
                  of DRC supervision

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrfmac_exit_drc_supervision
(
  uint16 phy_chan_mask
);

/*===========================================================================
FUNCTION HDRFMAC_GET_DRC_LENGTH                                     EXTERNAL

DESCRIPTION
  Returns the drc_length from the indicated carrier.
  Returns 0 if call is not active or carrier is not valid.
 
DEPENDENCIES
  None.

PARAMETERS
  carrier - The carrier id from TCA

RETURN VALUE
  The drc_length for the carrier.

SIDE EFFECTS
  None.
===========================================================================*/
extern int hdrfmac_get_drc_length
( 
  uint8 carrier 
    /* Carrier id from TCA */
);
#ifdef FEATURE_HDR_THERMAL_BACKOFF
/*===========================================================================

FUNCTION HDRFMAC_CONTROL_THERMAL_DIV_CMD                                 EXTERNAL

DESCRIPTION
  This interface processes thermal diversity command from CCM in HDRRX
  task context.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd - Thermal flow control command to be processed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/

void hdrfmac_control_thermal_div_cmd (ds707_ccm_flow_control_cmd_enum_type cmd);

#endif /* FEATURE_HDR_THERMAL_BACKOFF */

/*===========================================================================
FUNCTION HDRFMAC_GET_FC_PATTERN                                     EXTERNAL

DESCRIPTION
  Returns the current flow control pattern.
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The current FC pattern.

SIDE EFFECTS
  None.
===========================================================================*/

hdrfmac_flow_control_pattern_enum_type hdrfmac_get_fc_pattern( void );

#ifdef FEATURE_HDR_REVC
/*===========================================================================

FUNCTION HDRFMAC_IS_LOAD_INFO_MSG_RCVD

DESCRIPTION
  Check whether any loading information message is received from current
  serving sector.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  TRUE  - A loading information message has been received from current
          serving sector in the CONNECTED or IDLE state
  FALSE - None of loading information message has been received.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean hdrfmac_is_load_info_msg_rcvd ( void );


/*===========================================================================
 
FUNCTION HDRFMAC_SET_LOAD_INFO_MSG_RCVD

DESCRIPTION
  This function resets the indicator which shows whether load information
  message is received in the CONNECTED OR IDLE state from the current serving
  sector.
 
DEPENDENCIES
  None.

PARAMETERS
  load_info_msg_rcvd - TRUE, if Load Information message has been received
                       in the CONNECTED OR IDLE state from the current serving
                       sector. FALSE, if not.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
 
===========================================================================*/
void hdrfmac_set_load_info_msg_rcvd
(
  boolean load_info_msg_rcvd 
);

/*===========================================================================
 
FUNCTION HDRFMAC_FIND_LOADADJ_CACHE

DESCRIPTION
  This function searches load_info table based on given chan_ptr
  Note that this function only searches based on given chan. Caller
  need to check if the pn matches.
 
DEPENDENCIES
  None.

PARAMETERS
  chan_ptr - pointer to channel to search for

RETURN VALUE
  index into the load_info table. negative if not found

SIDE EFFECTS
  None.
 
===========================================================================*/
hdrfmac_loadadj_cache_type* hdrfmac_find_loadadj_cache
(
  sys_channel_type* chan_ptr
);

/*===========================================================================
 
FUNCTION HDRFMAC_FIND_FREE_LOADADJ_CACHE

DESCRIPTION
  This function searches for a free entry in load_info table. It will
  return the first INVALID cache entry if found, or the oldest one
  in case there is no INVALID entry.
 
DEPENDENCIES
  None.

PARAMETERS
  

RETURN VALUE
  index into the load_info table. negative if not found

SIDE EFFECTS
  None.
 
===========================================================================*/
hdrfmac_loadadj_cache_type* hdrfmac_find_free_loadadj_cache( void );

/* EJECT */
/*===========================================================================

FUNCTION HDRCMAC_PROCESS_TIMER

DESCRIPTION
  This function handles the timer expiry events assigned to this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  timer_id - ID of the timer just expired.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrfmac_process_timer
( 
  uint32 timer_id
);

/* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_PROCESS_OVHD_NLB_CACHE_UPDATED

DESCRIPTION
  This function processes OVHD_NLB_CACHE_UPDATED indication. It goes
  thru all valid LoadAdj cache and updates HDRSRCH.
  
DEPENDENCIES
  None.

PARAMETERS
  serving_sector - Sector on which the overhead cache is updated. If chan/pn
                   are set to 0, all cache are updated.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrfmac_process_ovhd_nlb_cache_updated
( 
  sys_link_type *serving_sector
);

/*===========================================================================
FUNCTION HDRFMAC_IS_LOADINFO_SUPPORTED

DESCRIPTION
  This function checks if LoadInfo is negotiated

DEPENDENCIES 
  None 
 
PARAMETERS 
  None
 
RETURN VALUE
  True  - if LoadInfo supported
  False - if LoadInfo is not supported

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrfmac_is_loadinfo_supported( void );
#endif /* FEATURE_HDR_REVC */

#ifdef FEATURE_HDR_FLEX_CONN
/*===========================================================================
FUNCTION HDRFMAC_GET_DSC_LENGTH_IN_SLOT                               EXTERNAL

DESCRIPTION
  Get dsc length in slot
 
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  The dsc_length in slots.

SIDE EFFECTS
  None.
===========================================================================*/
uint16 hdrfmac_get_dsc_length_in_slot( void );

/*===========================================================================
FUNCTION HDRFMAC_FLEXCONN_CHECK_DRC_LOCK_LEN                               EXTERNAL

DESCRIPTION
  Check if drc lock length <= dsc length (both in slots)
 
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  Whether  drc lock length <= dsc length

SIDE EFFECTS
  None.
===========================================================================*/
boolean hdrfmac_flexconn_check_drc_lock_len( void );
#endif /* FEATURE_HDR_FLEX_CONN */

#if defined(FEATURE_HDR_FL_DATA_ADAPTIVE_RXD)||defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)


/*===========================================================================

FUNCTION HDRFMAC_SET_FL_ACTIVE_CB

DESCRIPTION
  Set FMAC call back 
 
DEPENDENCIES
  None.

PARAMETERS
  fl_active_cb_ptr - call back once FL is active.
  client_id

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfmac_set_fl_active_cb
( 
  hdrfmac_fl_active_cb_type          fl_active_cb_ptr,
    /* FL active call back function ptr */
  hdrfmac_fl_active_cb_client_type   client_id,
    /* Client id */
  uint32                             payload
    /* payload indicates inact_ms if cb_ptr is null, or
       payload indicates whether to resume timer if cb_ptr is not null
    */
);

/*===========================================================================

FUNCTION HDRFMAC_REG_FL_ACTIVE_CB

DESCRIPTION
  Register FMAC call back once FL is active.
 
DEPENDENCIES
  None.

PARAMETERS
  fl_active_cb_ptr - call back once FL is active.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfmac_reg_fl_active_cb
( 
  hdrfmac_fl_active_cb_type fl_active_cb_ptr,
    /* FL active call back function ptr */

  hdrfmac_fl_active_cb_client_type   client_id,
    /* Client id */

  boolean                            resume_fl_timer
    /* Whether to resume FL activity timer when register callback */
);

/*===========================================================================

FUNCTION HDRFMAC_DEREG_FL_ACTIVE_CB

DESCRIPTION
  De-register FMAC call back once FL is active.
 
DEPENDENCIES
  None.

PARAMETERS
  fl_active_cb_ptr - call back once FL is active.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrfmac_dereg_fl_active_cb
( 
  hdrfmac_fl_active_cb_client_type   client_id,
    /* Client id */
  boolean                            suspend_fl_timer
    /* whether to suspend FL activity timer when dereg the callback */
);

/*===========================================================================

FUNCTION HDRFMAC_FL_ACTIVITY_MONITOR

DESCRIPTION
  Notify the client that FL is active again and reset the timer to track FL
  activities
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  cb function ptr.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrfmac_fl_activity_monitor( void );
#endif /* defined(FEATURE_HDR_FL_DATA_ADAPTIVE_RXD)||
          defined(FEATURE_HDR_FLEX_CONN)||defined(FEATURE_HDR_DEVICE_ONLY_DTX)*/

#ifdef FEATURE_HDR_GRICE
/*===========================================================================

FUNCTION HDRFMAC_FAC_GAIN_CHECK_AND_SUSPEND

DESCRIPTION 
  This function is called to check if it is necessary to suspend the FAC
  gain slot interrupt from FW.
 
  This function is called when DO is transitioning from traffic to traffic
  suspend state.  FAC calculation is not performed in traffic suspend
  state.  The information provided by the FAC gain slot interrupt is not
  used.
 
DEPENDENCIES
  None

RETURN VALUE 
  None 

SIDE EFFECTS 
  None 

===========================================================================*/

void hdrfmac_fac_gain_check_and_suspend( void );

/*===========================================================================

FUNCTION HDRFMAC_FAC_GAIN_CHECK_AND_RESUME

DESCRIPTION 
  This function is called to check if it is necessary to resume the FAC
  gain slot interrupt from FW.
 
  This function is called when DO is transitioning from traffic suspend state
  to traffic state.  If FAC calculation is enabled, FW provides FAC calculation
  parameters in the FAC gain slot interrupt.  FAC gain calculation only runs
  in traffic state.
 
DEPENDENCIES
  None

RETURN VALUE 
  None 

SIDE EFFECTS 
  None 

===========================================================================*/

void hdrfmac_fac_gain_check_and_resume( void );

/*===========================================================================

FUNCTION HDRFMAC_IS_FAC_GAIN_EST_AVAILABLE

DESCRIPTION 
  This function is called to check whether FAC Gain estimated value is computed
  after Power Up.
 
  This function is called by searcher when DO enters traffic. Searcher checks
  several conditions before enabling D-LNA, and this is one of those checks.
 
DEPENDENCIES
  None

RETURN VALUE 
  TRUE - If FAC gain estimate is available
  FALSE - If FAC gain estimate is NOT available

SIDE EFFECTS 
  None 

===========================================================================*/

boolean hdrfmac_is_fac_gain_est_available ( void );
#endif /* FEATURE_HDR_GRICE */

#ifdef FEATURE_HDR_MODEM_API
/*===========================================================================
FUNCTION HDRFMAC_GET_DRC_CHANNEL_GAIN                                EXTERNAL

DESCRIPTION
  Returns the drc_channel_gain from the indicated carrier.
  Returns HDRFMAC_CHAN_GAIN_INVALID if call is not active or
  carrier is not valid.
 
DEPENDENCIES
  None.

PARAMETERS
  carrier - The carrier id from TCA

RETURN VALUE
  The drc_channel_gain for the carrier.

SIDE EFFECTS
  None.
===========================================================================*/
extern int8 hdrfmac_get_drc_channel_gain
( 
  uint8 carrier 
    /* Carrier id from TCA */
);

/*===========================================================================
FUNCTION HDRFMAC_GET_ACK_CHANNEL_GAIN                                EXTERNAL

DESCRIPTION
  Returns the ack_channel_gain from the indicated carrier.
  Returns HDRFMAC_CHAN_GAIN_INVALID if call is not active or
  carrier is not valid.
 
DEPENDENCIES
  None.

PARAMETERS
  carrier - The carrier id from TCA

RETURN VALUE
  The ack_channel_gain for the carrier.

SIDE EFFECTS
  None.
===========================================================================*/
extern int8 hdrfmac_get_ack_channel_gain
( 
  uint8 carrier 
    /* Carrier id from TCA */
);
#endif /* FEATURE_HDR_MODEM_API */

 /* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_SET_AWAITING_TC_ASSIGN_COMPLETE                     EXTERNAL

DESCRIPTION
  This interface is used to set the flag indicating whether HDRFMAC is still
  waiting TCA assignment to be completed by HDR SRCH.

DEPENDENCIES
  None

PARAMETERS
  value - boolean value to set the flag.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrfmac_set_awaiting_tc_assign_complete
( 
  boolean value
);

 /* EJECT */
/*===========================================================================

FUNCTION HDRFMAC_GET_AWAITING_TC_ASSIGN_COMPLETE                     EXTERNAL

DESCRIPTION
  This interface is used to get the flag indicating whether HDRFMAC is still
  waiting TCA assignment to be completed by HDR SRCH.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  value - boolean value to set the flag.

SIDE EFFECTS
  None

===========================================================================*/
boolean hdrfmac_get_awaiting_tc_assign_complete( void );


/*===========================================================================

FUNCTION HDRFMAC_CONTROL_FLOW                              EXTERNAL

DESCRIPTION
  This interface processes flow control command given by DS.
  Runs in DS task context.
  
DEPENDENCIES
  None.

PARAMETERS
  cmd - Memory Flow control command

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
void hdrfmac_control_flow (uint32 cmd);

#endif /* HDRFMAC_H */
