#ifndef HDRRUP_H
#define HDRRUP_H

/*===========================================================================

       R O U T E  U P D A T E   P R O T O C O L  D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Route Update Protocol (RUP).

Copyright (c) 2000 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrrup.h_v   1.11   09 Jan 2003 11:01:30   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/api/hdrrup.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
03/04/14   vko     move hdrrup_process_conn_setup_timeout to global      
01/13/13   ukl     Added NV control for 1x TuneAway during session negotiation.
04/17/12   cnx     Support rfm_is_band_chan_supported_v2().
04/15/11   cnx     Removed hdrrup_band_is_supported() if RF_HAS_RFM_BAND_CHAN_V2_API defined.
06/10/10   pba     IRAT Measurement feature.
11/12/09   sju     Added support for SVDO.
09/30/08   wsh     Not checking if channel is within band while hashing
07/30/08   sju     Added support for sorting rev channels in ascending order
12/12/07   sju     Renamed hdr channel record type structure
10/08/07   sju     Updated comments. 
09/07/07   sju     Added support for revB RU msg
08/09/07   sju     Move Pilot Group macro to srch
08/08/07   mpa     Added hdrrup_get_traffic_mode()
07/13/07   sju     Added support for revB Multicarrier RUP
05/15/07   hal     Added hdrrup_increment_tca_count() to support multiple TCAs
04/10/07   mt      Added hdrrup_set_tc_handdown_mode().
11/20/06   hal     Updated feature for SupportedCDMAChannels to silently close
                   session if band preferences are changed
07/18/06   dna     Support for RouteUpdateTriggerCode, to send RUP msg and 
                   also retry following max access probes, if necessary.
05/31/06   hal     Changed parameter for hdrrup_band_is_supported()
12/13/05   pba     Added support for handle TCA + ConnectionClose bundling
09/12/05   hal     Added hdrrup_configure() for use with AT-init config
                   Added hdrrup_is_band_supported() for use with the
                   SupportedCDMAChannels attribute
09/09/05   dna     Added support for adding high prio traffic mode
07/11/05   pba     Restrict size of route update message based on maximum bytes 
                   instead of maximum pilots 
06/22/05   pba     Defined pilot report size constants.
05/24/05   pba     Added hdrrup_is_tc_assigned() to determine if RTC is assigned
03/07/05   pba     Modified RUP timers to use callback functions instead of 
                   signals.
03/07/05   pba     Added hdrrup_continue_connection_setup() to facilitate 
                   synchronization between RUP and PAC.
01/08/03   mpa     Removed unnecessary dependency on hdrsrch.h
09/07/01   vas     Added hdrrup_get_current_tca_seq_num()
09/05/01   dna     Added separate handling for in-use vs. in_config messages.
06/27/00   vas     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"
#ifdef FEATURE_IRAT_PILOT_MEAS
#include "cm.h"
#endif /* FEATURE_IRAT_PILOT_MEAS */
#include "hdrsrchtypes.h"

/* <EJECT> */
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/

/* <EJECT> */

/*===========================================================================
                        EFS NV  DECLARATIONS
===========================================================================*/
#define HDRRUP_1X_TUNEAWAY_IN_SES_CFG_ENABLED "/nv/item_files/modem/hdr/cp/rup/en_1x_ta_in_ses_cfg"
/* efs_nv item to enable 1x tuneaway during session negotiation */

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/* Max number of pilots that can be included in route update message */
#define HDRRUP_RUP_NUM_PILOTS_MAX 16

/* This macro value is used to indicate that route update message is */
/* to be formed without any restriction on size */
#define HDRRUP_RUP_SIZE_NO_RESTRICTION 0xFF

/* In the case when we want to restrict the size, 24 bytes route update message
 * would allow including maximum of 6 same frequency or 3 off-frequency pilots 
 * or other combinations of these two */
#define HDRRUP_RUP_REDUCED_SIZE 24

#define HDRRUP_MAX_NUM_SUBASETS_NOT_REVB_TCA   1

/* TODO: Following three default values are for phase 1 only. 
 *       They may change in later phases
 */
#define HDRRUP_UNIQUE_FTCMAC_INDICES_MAX_SIZE 1  /* 1 for phase1 */
#define HDRRUP_FWD_CH_PER_SUBASET_MAX_SIZE    1  /* 1 for phase1*/
#define HDRRUP_REV_CH_PER_SUBASET_MAX_SIZE    1  /* 1 for phase1*/

#define HDRRUP_TCA_FWD_CH_MAX_SIZE \
          ( HDRSRCH_MAX_NUM_SUBASETS * HDRRUP_FWD_CH_PER_SUBASET_MAX_SIZE )
#define HDRRUP_TCA_REV_CH_MAX_SIZE \
          ( HDRSRCH_MAX_NUM_SUBASETS * HDRRUP_REV_CH_PER_SUBASET_MAX_SIZE )

#define HDRRUP_SECTOR_MAX_SIZE \
          (HDRSRCH_MAX_NUM_SUBASETS * HDRSRCH_ASET_MAX_SIZE )

typedef enum
{
  HDRRUP_REV_CH_CONFIG_NO_CH, /* No rev ch associated with s-aset */
  HDRRUP_REV_CH_CONFIG_PAIRED_CH, /* Paired rev ch associated with s-aset */
  HDRRUP_REV_CH_CONFIG_CH_SPECIFIED, /* the rev ch associated with s-aset is specified */
} hdrrup_rev_ch_config_enum_type;

typedef enum
{
  HDRRUP_BUILD_NEW_RUP_MESSAGE,
  HDRRUP_REBUILD_SAME_RUP_MESSAGE
} hdrrup_rup_msg_build_type;

/* Bit Mask for Connection Setup Fail Reason */
#define HDRRUP_CONN_FAIL_TIMEOUT          0x01
#define HDRRUP_CONN_FAIL_TCA_FAILED       0X02

/*--------------------------------------------------------------------------
               TRAFFIC CHANNEL ASSIGNMENT MESSAGE STRUCTURE

--------------------------------------------------------------------------*/
/* TCA Sub-Active Set FTC information*/
typedef struct
{
  uint8 feedback_enabled;
  uint16 feedback_mux_id;
  uint8 feedback_rev_ch_id;
  uint8 dsc_enabled;
  uint8 next_3fields_same;
  uint8  drc_length;                /* DRC Length */
  int8   drc_channel_gain;          /* DRC Channel Gain */
  int8   ack_channel_gain;          /* ACK Channel Gain */
} hdrrup_tca_sub_aset_ftc_params_type;

typedef struct
{
  uint16 ftc_mac_index;
  uint8 assigned_interlaces;
} hdrrup_tca_sub_aset_sector_ftc_params_type;

/* TCA Sub-Active Set RTC information*/
typedef struct
{
  uint8 ch_config;
  sys_channel_type channel;
  uint8 drop_rank;
  uint8  a_order;
    /* rev channel in the ascending order of frequency in this TCA*/
} hdrrup_tca_sub_aset_rtc_ch_type;

typedef struct
{
  uint16 rev_mac_index;   
  uint8 rab_mac_index;
  int8  delta_t2p;
} hdrrup_tca_sub_aset_sector_rtc_params_type;

/* TCA Sector information*/
typedef struct
{
  int8                            ra_channel_gain;
  sys_pilot_pn_type               pilot_pn;
  hdrmac_drc_cover_type           drc_cover;
  boolean                         softer_handoff;
  uint8                           dsc;
  hdrmac_rab_len_type             rab_length;  /* for rev0 and A only*/
  uint8                           rab_offset;  /* for rev0 and A only*/
} hdrrup_tca_sector_info_type;

typedef struct
{
  uint8 pilot_included;
  uint8 fwd_chan_index;
  uint8 pilot_group_id;
  uint8 num_unique_ftc_mac;
  uint8 scheduler_tag;
  uint8 aux_drc_cover_incl;
  uint8 aux_drc_cover;
  uint8 ftc_mac_index_interlace_enabled;
  hdrrup_tca_sub_aset_sector_ftc_params_type ftc_params[HDRRUP_UNIQUE_FTCMAC_INDICES_MAX_SIZE];
  hdrrup_tca_sub_aset_sector_rtc_params_type rtc_params[HDRRUP_REV_CH_PER_SUBASET_MAX_SIZE];
} hdrrup_tca_sub_aset_sector_params_type;

/* TCA Sub Aset information*/
typedef struct
{
  uint8 num_fwd_channels;
    /* NumFwdChannelsThisSubActiveSet*/

  sys_hdr_channel_record_s_type fwd_channel[HDRRUP_FWD_CH_PER_SUBASET_MAX_SIZE];
    /* storage for AssignedChannel (Fwd channel)*/

  hdrrup_tca_sub_aset_ftc_params_type ftc_params;
    /* fwd params for sub-active set */

  uint8 num_rev_channels_incl;
    /* NumReverseChannelsIncluded */

  uint8 num_rev_channels;
    /* NumReverseChannels in this sub-active set*/

  hdrrup_tca_sub_aset_rtc_ch_type rev_channel[HDRRUP_REV_CH_PER_SUBASET_MAX_SIZE];
    /* Storage for rev channel information*/

  hdrrup_tca_sub_aset_sector_params_type sector_params[HDRRUP_SECTOR_MAX_SIZE];
    /* Storage for sector information in this sub-active set*/

  uint8 carries_cc;  
    /* If this sub-active set carries ControlChannel */

  uint8 not_reportable; 
    /* If this sub-active set reportable*/
} hdrrup_tca_sub_aset_type;

typedef struct
{
  uint8  msg_id;              /* Message ID */
  uint8  msg_seq;             /* Message Sequence Number */
  uint8  chan_incl;           /* Channel Included */
  uint8  frame_offset;        /* Frame Offset */
  uint8  dsc_channel_gain;    
    /* DSC Channel Gain 
     * For now dsc_channel_gain is left in uints of -0.5dB, as received
     * over the air i.e. dsc_channel_gain of 5 is -2.5dB. If different
     * uints are needed, the TCA unpacking routine should convert it */

  hdrsrch_link_id_type link_id;  /* Link ID */
  uint8 num_sectors;             /* NumSectors*/  
  uint8 num_sub_asets;           /* NumSubActiveSets*/
  uint8 scheduler_tag_incl;      /* SchedulerTagIncluded*/
  uint8 feedback_mux_enabled;    /*FeedbackMultiplexingEnabled */
  hdrrup_tca_sector_info_type sector_info[HDRRUP_SECTOR_MAX_SIZE];
    /* common sector information*/

  hdrrup_tca_sub_aset_type  sub_aset[HDRSRCH_MAX_NUM_SUBASETS]; 
    /* sub-active set information*/
} hdrrup_tca_msg_type;

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION HDRRUP_POWERUP_INIT

DESCRIPTION
  This function initializes the protocol prior to use.  It should be called
  once upon system startup.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_powerup_init 
( 
  void 
);

/*===========================================================================
FUNCTION HDRRUP_INIT

DESCRIPTION
  This function initializes the protocol prior to use.
  
DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_init 
( 
  void 
);

/*===========================================================================

FUNCTION HDRRUP_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to RUP.

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_msg_cb
(
  dsm_item_type *msg_ptr
);

/*===========================================================================

FUNCTION HDRRUP_CONFIG_MSG_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives a
  message to RUP in-config instance.

DEPENDENCIES
  None.
  
PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the message

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_config_msg_cb
(
  dsm_item_type *msg_ptr
);

/*===========================================================================

FUNCTION HDRRUP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to RUP.

DEPENDENCIES
  None.
  
PARAMETERS
  ind_name     - Indication to process, is a global enum for all possible 
                 indications in HDR signaling
  ind_data_ptr - Optional data pointer for indication specific data
 
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_ind_cb
(
  hdrind_ind_name_enum_type ind_name, 
  void *ind_data_ptr 
);

/*===========================================================================

FUNCTION HDRRUP_ACTIVATE

DESCRIPTION
  This function commands RUP to activate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrup_activate
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRRUP_DEACTIVATE

DESCRIPTION
  This function commands RUP to deactivate.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrup_deactivate
( 
  hdrhai_protocol_name_enum_type sender 
);

/*===========================================================================

FUNCTION HDRRUP_CLOSE

DESCRIPTION
  This function commands RUP to close connection.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrup_close
( 
  hdrhai_protocol_name_enum_type sender 
);

/*===========================================================================
FUNCTION HDRRUP_GET_RUP_MSG

DESCRIPTION
  This function calls hdrrup_form_rup_msg to form the route update message
  and then adds the HMP header to the DSM pointer returned by the function.
  This function is externalized and called by SLP to send RUP msg on the 
  Access channel.
  
DEPENDENCIES
  None.

PARAMETERS
  rup_msg_build_type - specifies whether we are building a new route update 
                       message or rebuilding the same with reduced size.
  max_rup_msg_size - maximum allowed size for route update message in units
                     of bytes
  
RETURN VALUE
  Pointer to the dsm item containing the packed route update message.

SIDE EFFECTS
  None.
===========================================================================*/
extern dsm_item_type *hdrrup_get_rup_msg
( 
  hdrrup_rup_msg_build_type rup_msg_build_type,
  uint16 max_rup_msg_size 
);

/*===========================================================================
FUNCTION HDRRUP_GET_CURRENT_TCA_SEQ_NUM

DESCRIPTION
  This function returns the current sequence number for Traffic Channel
  Assignment message.

DEPENDENCIES
  None.

PARAMETERS
  None
  
RETURN VALUE
  Current traffic channel assignment message sequence number.

SIDE EFFECTS
  None.
===========================================================================*/
extern uint8 hdrrup_get_current_tca_seq_num( void );

/*===========================================================================
FUNCTION HDRRUP_CONTINUE_CONNECTION_SETUP

DESCRIPTION
  This function commands RUP to continue processing connection setup

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrup_continue_connection_setup
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRRUP_CONTINUE_AFTER_CONNECTION_CLOSE

DESCRIPTION
  This function commands RUP to continue any actions that were left pending
  while the connection was closing.

DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void hdrrup_continue_after_connection_close
(
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRRUP_IS_TC_ASSIGNED

DESCRIPTION
  This function checks if the AT has been assigned a Reverse Traffic channel
  
DEPENDENCIES
  None

PARAMETERS
  caller - Protocol calling this function 

RETURN VALUE
  TRUE - If RTC has been assigned, FALSE otherwise

SIDE EFFECTS
===========================================================================*/

extern boolean hdrrup_is_tc_assigned
(
  hdrhai_protocol_name_enum_type caller 
);

/*===========================================================================
FUNCTION HDRRUP_SET_TRAFFIC_MODE

DESCRIPTION
  This function sets the HDR traffic mode.

DEPENDENCIES
  None.

PARAMETERS
  traffic_mode - The HDRSRCH traffic mode to allow applications to tweak,
                 for example, specifics of SHDR operation while connected.
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_set_traffic_mode
( 
  hdrsrch_tc_mode_enum_type traffic_mode,
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================
FUNCTION HDRRUP_GET_TRAFFIC_MODE

DESCRIPTION
  This function gets the HDR traffic mode.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  traffic_mode - The HDRSRCH traffic mode.
  
SIDE EFFECTS
  None.
===========================================================================*/
extern hdrsrch_tc_mode_enum_type hdrrup_get_traffic_mode (void);

/*===========================================================================
FUNCTION HDRRUP_SET_TC_HANDDOWN_MODE

DESCRIPTION
DESCRIPTION  : This function sets the handdown mode during HDR connected
               state. Handdown can be enabled or disabled during HDR 
               traffic connection.

DEPENDENCIES : None

PARAMETERS   : None

RETURN VALUE : None

SIDE EFFECTS : None

=========================================================================*/

extern void hdrrup_set_tc_handdown_mode
( 
  hdrsrch_tc_handdown_mode_enum_type handdown_mode,
    /* the handdown mode selected */
  hdrhai_protocol_name_enum_type sender
);

/*===========================================================================

FUNCTION HDRRUP_CONFIGURE

DESCRIPTION
  This function is called by SCP to start AT-initiated configuration.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The calling protocol.

RETURN VALUE
  TRUE if the configuration is required; FALSE if configuration is not
  needed.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrrup_configure
( 
  hdrhai_protocol_name_enum_type sender 
);

/*===========================================================================

FUNCTION HDRRUP_SEND_ROUTE_UPDATE

DESCRIPTION
  This function is called to force the AT to send a RouteUpdate message.
  
DEPENDENCIES
  None.

PARAMETERS
  sender - The protocol giving this command (only for software trace)

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrrup_send_route_update
( 
  hdrhai_protocol_name_enum_type sender 
);


/*===========================================================================

FUNCTION HDRRUP_BAND_PREF_IS_CHANGED

DESCRIPTION
  Determines if the band preference was changed.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean hdrrup_band_pref_is_changed( void );

/*===========================================================================

FUNCTION HDRRUP_INCREMENT_TCA_COUNT

DESCRIPTION
  This function increments the number of TCA messages received. The AT will
  process only the final TCA message sent by the AN, and will discard any
  intermediate TCA messages.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrrup_increment_tca_count( void );

/*===========================================================================

FUNCTION HDRRUP_UPDATE_MC_RUP_MSG

DESCRIPTION
  This function is called by slp to modify ATTotalPilotTransmission and 
  ReferencePilotChannel fields of MC RUP msg before giving it RMAC.
  We update MC RUP fields when RU msg is in SLP queue to make sure we have  
  latest SLP carrier information. 
  SLP carrier information may change every subframe, if we query RMAC for 
  SLP carrier information in the hdrrup_form_rup_msg(), information may be 
  out of date when message is queued to SLP.

DEPENDENCIES
  None.

PARAMETERS
  msg_ptr - Pointer to the DSM item chain holding the RouteUpdate message

RETURN VALUE
  None.

SIDE EFFECTS
  DSM item which holds RouteUpdate message may be modified.
===========================================================================*/
extern void hdrrup_update_mc_rup_msg
(
  dsm_item_type *msg_ptr
);

/*===========================================================================
FUNCTION HDRRUP_INIT_MC_RUP_INFO_BUFFER

DESCRIPTION
  This function initializes the MC RUP info buffer  

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrrup_init_mc_rup_info_buffer(void);

/*===========================================================================

FUNCTION HDRRUP_IS_BAND_CHAN_SUPPORTED

DESCRIPTION
  This function acts as an entry point for the LMAC function 
  hdrlmac_is_band_chan_supported(), which checks if a channel/band
  combo is supported by the RF chipset of the AT.
  
DEPENDENCIES
  None.

PARAMETERS
  channel_ptr - The band class and channel being compared against the 
  supported bands/channel of RF chipset.

RETURN VALUE
  TRUE if the chan/band combo is supported by the RF chipset in use. 
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrup_is_band_chan_supported
(
  sys_channel_type *channel_ptr
);
#ifdef FEATURE_IRAT_PILOT_MEAS
/*===========================================================================
FUNCTION hdrrup_is_irat_meas_in_progress

DESCRIPTION
  This function returns whether IRAT measurement is in progress or not.
  
DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  TRUE if IRAT measurement is in progress, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean hdrrup_is_irat_meas_in_progress( void );

/*===========================================================================
FUNCTION HDRRUP_IRAT_PILOT_MEAS_REQ

DESCRIPTION
  This function commands RUP to get pilot measurements from searcher.
  
DEPENDENCIES
  None

PARAMETERS
  sender - Protocol sending this message.
  pilot_meas - Pilots for which measurement is requested.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrrup_irat_pilot_meas_req
( 
  hdrhai_protocol_name_enum_type  sender,
  cm_ss_hdr_info_s_type           pilot_meas
);
#endif /* FEATURE_IRAT_PILOT_MEAS */

#ifdef FEATURE_LTE_TO_HDR_OH
/*===========================================================================

FUNCTION HDRRUP_ACT_HO_COMPLETED

DESCRIPTION
  This function sends ACT_HO_COMPLETED_IND so DS can send data thru
  HDR
  
DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrup_act_ho_completed( void );
#endif /* FEATURE_LTE_TO_HDR_OH */
/*===========================================================================
FUNCTION HDRRUP_PROCESS_CONN_SETUP_TIMEOUT

DESCRIPTION
  This function processes the connection setup timeout signal.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrrup_process_conn_setup_timeout( void );
#endif  /* HDRRUP_H */
