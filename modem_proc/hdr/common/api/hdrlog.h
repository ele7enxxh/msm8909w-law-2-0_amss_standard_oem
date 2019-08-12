#ifndef HDRLOG_H
#define HDRLOG_H
/*===*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               H D R    L O G    F U N C T I O N S

GENERAL DESCRIPTION

   This file contains the typedefs, enums and defines used in
   - generic HDR logging functions
   - reset commands
   - AT Config diag command
   
EXTERNALIZED FUNCTIONS
  hdrlog_start_conn_state_stats_periodic_timer
  hdrlog_mdsp_debug_record
  others...

REGIONAL FUNCTIONS 

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

============================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrlog.h_v   1.13   31 Mar 2003 13:59:38   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrlog.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     -------------------------------------------------------- 
03/11/14   tnp     Increase maximum number of supported MAC flows to 8
01/14/14   tnp     Reduce maximum number of supported MAC flows to 5
10/30/13  kss/rmv  FR3272 - Modem API for Cost of Power estimate changes 
09/03/13   wsh     Replaced FEATURE_DIME/TRITON_BOLT_MODEM with HDR version
07/31/13   sat     Mainlining Modem Statistics Feature
06/04/13   kss     Added HDRLOG_AMAC_MDSP_SLOT_LOGGING_CLIENT. 
05/22/13   rmv     Added diag command to get cost of modem power estimate
03/08/13   rmv     Added support for stats cdma_evdo_rx0_agc [rx0_dbm], 
                   cdma_evdo_rx1_agc [rx1_dbm]
03/19/12   ljl     Fixed Dime compiler errors.
11/11/11   arm     HDR idle and access state diversity implemetation.
10/10/11   arm     Supported second chain test call feature. 
09/23/11   smd     SU API cleanup.
09/07/11   lyl     Added HDRLOG_FMAC_MDSP_SLOT_LOGGING_CLIENT.
06/16/11   cnx     Fixed compiler warnings.
05/31/11   cnx     Added HDR_PROTOCOL_DATA command.
07/12/10   cnx     Added Diag cmd to change diversity preference in runtime. Fixed CR 239355. 
06/01/10   pba     Added support for FTD stats.
02/20/10   kss     Initial support for FW logging on MDM9X00
09/15/09   kss     Use new PACK() macro for Q6 compatibility.
06/18/08   etv     Added diag command to set/reset DTXMode
03/06/08   etv     DSP gives Log timestamp in slots insteads of half-slots.
01/14/08   etv     Simplified MDSP Slot log design by removing Event type.
12/05/07   etv     Added support for mDSP slot based logging.
02/12/07   mpa     Fixed RVCT warnings.
10/04/06   pba     Added HDR_WRITE_DATABASE_TO_EFS command.
06/30/06   etv     Added a DIP Sw to toggle MUP Decode Filter
05/22/06   ljl     Added dip switch for FLAT
12/14/05   jyw     Added dip switch definition for TAP to disable TC OFS.
10/26/05   dna     Support dipswitch to enable/disable VT rate parameters
07/05/05   ds      Added support for generic MDSP logging
06/24/05   sk      Added HDR_RST_MRLP_STATS command
11/29/04   grl     Added support for connected state stat logging.
11/19/04   etv     Added HDR_MCS_CLR_STATS_COMMAND
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
06/25/04   kss     Added HDR_MCS_FLOW_COMMAND.
06/14/04   mpa     Removed HDRLOG_MCS_REGISTER_FLOW, HDRLOG_MCS_DEREGISTER_FLOW
05/05/04   kss     Added HDR BCMCS (Gold Multicast) commands.
02/20/03   lad     Updated to be compatible with MSMSHARED_DIAG.
02/19/03   mpa     Fixed dip switch num boundary issue
02/10/03   mpa     Added HDRLOG_DIPSW_DDTM.
02/10/03   kss(mpa)Added hdrlog_set_dipsw_cb
01/09/03   mpa     Added RL rate inertia and ant1 test mode dip switches.
11/19/02   mpa     Moved dip switches from old diag over to HDR subsystem
11/11/02   mpa     Added HDR_DIP_SWITCH cmd and requestor fn is_dipsw_set()
10/15/02   mpa     Added HDR_STATE diag cmd
02/08/02   mpa     Added hdrlog_powerup_init and hdrlog_on_request_dispatch
10/29/01   mpa     Deleted all '#if 0' blocks of code.
09/12/01   mpa     Created this file
==========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

==========================================================================*/
#include "hdr_variation.h"
#include "hdrlogi.h"
#include "diagpkt.h"
#include "sys.h"
#include "hdrcom_api.h"
/*==========================================================================


                     DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

==========================================================================*/

typedef enum 
{
  HDR_RST_ACCESS_CNT = 0,         /* 0  Reset Access Attempt Count */
  HDR_RST_FWD_STATS,              /* 1  Reset Fwd Stats */
  HDR_RST_RTC_RATE_CNT,           /* 2  Reset Rev Traffic Count */
  HDR_RST_CONN_ATTEMPT_CNT,       /* 3  Reset Connection Attempt Cnt */
  HDR_RST_PAGE_MSG_CNT,           /* 4  Reset Page Message Count */
  HDR_RST_SESSION_ATTEMPT_CNT,    /* 5  Reset Session Attempt Count */
  HDR_RST_RLP_STATS,              /* 6  Reset RLP Stats */
  HDR_AT_CONFIG,                  /* 7  DIAG AT Config packet */
  HDR_STATE,                      /* 8  DIAG State Info packet */
  HDR_DIP_SWITCH,                 /* 9  Set/retrieve dip switch */
  HDR_MCS_FLOW_COMMAND,           /* 10 MCS flow-related commands */
  HDR_MCS_DEBUG_COMMAND,          /* 11 Other MCS related commands */
  HDR_MCS_CLR_STATS_COMMAND,      /* 12 MCS clear statistics commands */
  HDR_RST_CONNECTED_STATE_STATS,  /* 13 Reset connected state stats */
  HDR_RST_MRLP_STATS,             /* 14 Reset MRLP Stats */
  HDR_WRITE_DATABASE_TO_EFS,      /* 15 Write session database to EFS */
  HDR_DTX_MODE,                   /* 16 Dis-continuous Transmission Mode */
  HDR_DIV_PREF,                   /* 17 Diversity Preference */
  HDR_PROTOCOL_DATA               /* 18 HDR protocol data */
#ifdef FEATURE_HDR_MODEM_API
  ,HDR_MODEM_API_DATA_REPORT          /* 19 HDR - Modem API Data report */
#endif /* FEATURE_HDR_MODEM_API */
} hdrlog_diag_subsys_cmd_enum_type;

typedef struct
{
  hdrlog_diag_subsys_cmd_enum_type name;
    /* Name of the command */
} hdrlog_cmd_type;

typedef struct
{
  int16   c2i;         /* SINR of the serving sector in db */
  int32   rx_dbm;      /* Rx power in dbm */
  int32   tx_dbm;      /* Tx power in dbm */
  int16   ecio;        /* Ec/Io of reference sector in db */
  uint16  mac_index;   /* Current MAC index */
  sys_channel_type channel;
  int32   rx0_dbm;     /* Rx0 power in dbm */
  int32   rx1_dbm;     /* Rx1 power in dbm */

} hdrlog_ftd_l1_stats_type;

/*=========================================================================

HDR_AT_CONFIG

DESCRIPTION
  Identifies and defines the access terminal.  It also summaries the sw 
  versions that are being run on the AT>  This record is logged as soon as 
  HDR logging is enabled, whether or not the client requests it.  
  
  This is implemented as a diag pkt with the following chain of cmd codes:
  - command_code       = DIAG_SUBSYS_CMD_F (75)
  - subsystem_id       = DIAG_SUBSYS_HDR   (5)
  - subsystem_cmd_code = HDR_AT_CONFIG     (7)
  
=========================================================================*/
#define HDRLOG_VERNO_DATE_STRLEN 11
#define HDRLOG_VERNO_TIME_STRLEN 11
#define HDRLOG_VERNO_DIR_STRLEN   8

typedef PACK( struct )
{
  diagpkt_subsys_header_type xx_hdr;

  uint16    hdr_protocol_rev;
    /* Version number of the HDR Air Interface */
    
  uint16    hdr_firmware_version;
    /* MSB = major version, LSB = minor Version */

  uint16    at_model;
    /* Make and model of the Access Terminal */

  uint32    msm_version;
    /* MSB = MSM major version, LSB = MSM minor version */

  uint8     software_version[HDRLOG_VERNO_DIR_STRLEN];
    /* Software release version */

  uint8     compile_date[HDRLOG_VERNO_DATE_STRLEN];    
    /* format: mmm dd yyyy e.g. "Jun 26 2001" */

  uint8     release_date[HDRLOG_VERNO_TIME_STRLEN];
    /* format: mmm dd yyyy e.g. "Jun 29 2001" */

} hdrdiag_at_config_rsp_type;

/*=========================================================================

HDR_STATE_INFO
  Contains the state information for various modules in the Access Terminal

DESCRIPTION
   
  This is implemented as a diag pkt with the following chain of cmd codes:
  - command_code       = DIAG_SUBSYS_CMD_F   (75)
  - subsystem_id       = DIAG_SUBSYS_HDR     (5)
  - subsystem_cmd_code = HDR_STATE (8)
  
=========================================================================*/
/* Not packed since all fields are aligned. */
typedef struct
{
  diagpkt_subsys_header_type xx_hdr;

  uint8    at_state;
  uint8    session_state;
  uint8    almp_state;
  uint8    init_state;
  uint8    idle_state;
  uint8    conn_state;
  uint8    rup_state;
  uint8    ovhd_state;
  boolean  hybrid_mode;   /* hybrid mode flag TRUE=ON, FALSE=OFF */

} hdrdiag_state_rsp_type;

/*=========================================================================

HDR_DIP_SWITCH
  

DESCRIPTION
   
  This is implemented as a diag pkt with the following chain of cmd codes:
  - command_code       = DIAG_SUBSYS_CMD_F   (75)
  - subsystem_id       = DIAG_SUBSYS_HDR     (5)
  - subsystem_cmd_code = HDR_DIP_SWITCH (9)
  
=========================================================================*/

typedef uint32 hdrlog_dipsw_mask_type;

typedef enum 
{
  HDRLOG_DIPSW_RESET = 0,     /* Reset the dip switch (off) */
  HDRLOG_DIPSW_SET,           /* Set the dip switch (on) */
  HDRLOG_DIPSW_CLEAR_ALL,     /* Reset all dip switches */ 
  HDRLOG_DIPSW_READ_MASK      /* No action on the dip switches, 
                                 return current mask state */
} hdrlog_dipsw_action_enum_type;




/* Dip switch request */
typedef struct
{
  diagpkt_subsys_header_type xx_hdr;

  uint8 action; /* use hdrlog_dipsw_action_enum_type */
    /* Action to take on the dipsw mask */
  
  uint8 dipsw;   
    /* dip switch to set or reset (only valid if action=0,1) */

} hdrdiag_dip_switch_req_type;

/* Dip switch response */
typedef PACK( struct )
{
  diagpkt_subsys_header_type xx_hdr;
  
  uint8 action; /* use hdrlog_dipsw_action_enum_type */
    /* Action to take on the dipsw mask */
    /* Request must be part of the rsp */

  uint8 dipsw;   
    /* dip switch to set or reset (only valid if action=0,1) */
    /* Request must be part of the rsp */
  
  hdrlog_dipsw_mask_type dipsw_mask; 
    /* dip switch mask */

} hdrdiag_dip_switch_rsp_type;

/*=========================================================================

HDR_MCS_FLOW_COMMAND
  

DESCRIPTION
   
  This is implemented as a diag pkt with the following chain of cmd codes:
  - command_code       = DIAG_SUBSYS_CMD_F     (75)
  - subsystem_id       = DIAG_SUBSYS_HDR       (5)
  - subsystem_cmd_code = HDR_MCS_FLOW_COMMAND  (10)
  
=========================================================================*/

/* HDR MCS command actions */
typedef enum 
{
  HDRLOG_MCS_ENABLE_TEST_FLOW  = 0,    /* Enable flow with test app */
  HDRLOG_MCS_DISABLE_TEST_FLOW = 1,    /* Disable flow with test app */
  HDRLOG_MCS_FLOW_ACTION_INVALID = 0xFF  /* Invalid action */
} hdrlog_hdr_mcs_flow_action_enum_type;

/* MCS Enable Flow ID request/response */
typedef PACK( struct )
{
  diagpkt_subsys_header_type xx_hdr;

  uint8  action; 
    /* Enable/Disable */
  
  uint32 flow_id;   
    /* Flow ID to enable/disable */

} hdrdiag_hdr_mcs_flow_req_type;

/*=========================================================================

HDR_MCS_DEBUG_COMMAND


DESCRIPTION

  This is implemented as a diag pkt with the following chain of cmd codes:
  - command_code       = DIAG_SUBSYS_CMD_F     (75)
  - subsystem_id       = DIAG_SUBSYS_HDR       (5)
  - subsystem_cmd_code = HDR_MCS_DEBUG_COMMAND (11)

=========================================================================*/

/* HDR MCS command actions */
typedef enum
{
  HDRLOG_MCS_SET_ECB_DROP_MASK      = 0,  /* Set drop mask in ECB */
  HDRLOG_MCS_DISABLE_SOFT_COMBINING = 1,  /* Disable soft combine for broadcast */
  HDRLOG_MCS_ENABLE_SOFT_COMBINING  = 2,  /* Re-enable soft combine */
  HDRLOG_MCS_INVALID = 0xFF
} hdrlog_hdr_mcs_action_enum_type;

/* MCS debug cmd request/response */
typedef PACK( struct )
{
  diagpkt_subsys_header_type xx_hdr;

  uint8  action;
    /* Specifies specific command */

  uint32 payload;
    /* Payload for command */

} hdrdiag_hdr_mcs_debug_req_type;


/*=========================================================================

HDR_MCS_CLR_STATS_COMMAND


DESCRIPTION

  This is implemented as a diag pkt with the following chain of cmd codes:
  - command_code       = DIAG_SUBSYS_CMD_F     (75)
  - subsystem_id       = DIAG_SUBSYS_HDR       (5)
  - subsystem_cmd_code = HDR_MCS_DEBUG_COMMAND (12)

=========================================================================*/

/* HDR MCS Reset Statistics command actions */
typedef enum
{
  /* Reset Multicast Packet Rate Metrics */
  HDRLOG_MCS_CLR_PKT_RATE_METRICS_MASK = BITMASK(0),

  /* Reset Physical Channel Metrics */
  HDRLOG_MCS_CLR_PHY_CHAN_METRICS_MASK = BITMASK(1),

  /* Reset Logical Channel Metrics */
  HDRLOG_MCS_CLR_LOGI_CHAN_METRICS_MASK = BITMASK(2)

} hdrlog_hdr_mcs_clr_stats_action_enum_type;


/* Not PACKED since all fields are aligned */
typedef struct
{
  diagpkt_subsys_header_type xx_hdr;

  /* specifies the specific packet to reset */
  uint8 stats_to_reset;

} hdrdiag_hdr_mcs_clr_stats_req_type;


/* HDR diversity preference command actions */
typedef enum
{
  /* Turn off diversity preference */
  HDRLOG_DIV_PREF_OFF,

  /* Turn on diversity preference */
  HDRLOG_DIV_PREF_ON

#ifdef FEATURE_HDR_SECOND_CHAIN_TEST_CALL
   
  /* Turn only second chain on */
  ,HDRLOG_SECOND_CHAIN_ONLY

#endif

} hdrlog_hdr_div_pref_ctrl_action_enum_type;

/* Not PACKED since all fields are aligned */
typedef struct
{
  diagpkt_subsys_header_type xx_hdr;

  /* Specifies whether turn diversity preference on or off */
  hdrlog_hdr_div_pref_ctrl_action_enum_type action;

} hdrdiag_hdr_div_pref_ctrl_type;


/*=========================================================================

HDR_PROTOCOL_DATA_COMMAND

DESCRIPTION

  This is implemented as a diag pkt with the following chain of cmd codes:
  - command_code       = DIAG_SUBSYS_CMD_F     (75)
  - subsystem_id       = DIAG_SUBSYS_HDR       (5)
  - subsystem_cmd_code = HDR_PROTOCOL_DATA       (18)

=========================================================================*/

#define HDRLOG_MAX_SP_NEIGHBORS         32 /* Same as HDRHAI_MAX_SP_NEIGHBORS */
#define HDRLOG_RMAC3_MAX_NUM_MAC_FLOWS  8  /* Same as HDRSCMRMAC3_MAX_NUM_MAC_FLOWS */
#define HDRLOG_PROTOCOL_TYPE_ENUM_MAX   28 /* Same as HDRHAI_PROTOCOL_TYPE_ENUM_MAX */
#define HDRLOG_MAX_NUM_REL_A_DRCS       14 /* Same as HDRHAI_MAX_REL_A_DRC */

/* HDR subpacket ID */
typedef enum
{
  HDRLOG_SUBPKT_HDR_PROTOCOL_DATA,  /* HDR protocol data */
  HDRLOG_SUBPKT_MAX
} hdrlog_subpkt_id_enum_type;

/* HDR subpacket header format */
typedef PACK(struct)
{
  uint8 subpkt_id;
  uint8 subpkt_version;
  uint16 subpkt_size;
} hdrdiag_hdr_protocol_data_subpkt_header_type;

/* HDR protocol data subpacket */
typedef PACK(struct)
{
  hdrdiag_hdr_protocol_data_subpkt_header_type subpkt_header;

  /* AT state. */
  uint8 at_state;

  /* Searcher state. */
  uint8 srch_state;

  /* Number of personalities. */ 
  uint16 personality_count;

  /* Current personality. */
  uint16 curr_personality;
  
  /* Protocol subtypes. */
  uint16 protocol_subtypes[HDRLOG_PROTOCOL_TYPE_ENUM_MAX];
      
  /* Config token. */
  uint16 support_gaup_session_config_token;

  /* Color code. */
  uint8 color_code;

  /* Least Significant 24 bit of SectorID. */
  uint32 sector_id_24;

  /* Country code. */
  uint16 country_code;

  /* Subnet mask. */
  uint8 subnet_mask;

  /* Channel number. */
  uint16 channel_num;

  /* Band class. */
  uint8 channel_band;

  /* Pilot PN. */
  uint16 pilot_pn;

  /* Number of neighbors. */
  uint8 num_neighbors;

  /* Neighbor's channel number. */
  uint16 nset_channel_num[HDRLOG_MAX_SP_NEIGHBORS];

  /* Neighbor's pilot PN. */
  uint16 nset_pilot_pn[HDRLOG_MAX_SP_NEIGHBORS];

  /* Number of Route Update Trigger Code in the list. */
  uint8 rutc_list_cnt;

  /* Current interval of keep alive timeout, in unit of seconds. */
  uint32 ka_current_interval;

  /* Current PCCC value. */
  uint16 current_pccc;

  /* Whether attrib Random Handoff is accepted. */
  uint8 ran_ho_is_accept;

  /* Attrib Random Handoff's value */
  uint8 ran_ho_value;

  /* Sector access maximum rate */
  uint8 sector_access_max_rate;

  /* Maximum rate at which an AT is allowed to xmit access probes. */
  uint8 term_acc_rate_max;

  /* Inter-probe backoff. */
  uint8 probe_backoff;
  
  /* Inter-probe sequence backoff. */
  uint8 probe_seq_backoff;

  /* Maximum number of probe sequneces for a single access attempt. */
  uint8 probe_seq_max;

  /* Whether MU pkt is enabled. */
  uint8 mup_enabled;

  /* Whether DRC 1 is enabled for NULL rate. */
  uint8 null_rate_drc_38_4_enable;

  /* Current packet RRI. */
  uint8 cur_pkt_rri;
   
  /* Max allowed packet RRI. */
  uint8 max_pkt_rri;

  /* LoLat or HiCap transmission mode. */
  uint8 rmac3_flow_tx_mode[HDRLOG_RMAC3_MAX_NUM_MAC_FLOWS];

  /* Whether AN supports 1.8 Mbps rate transmission on Reverse channel. */
  uint8 rmac3_rate_1m8_supported;
  
  /* DRC Translation Offset attribute. */
  uint8 drc_trans_offset[HDRLOG_MAX_NUM_REL_A_DRCS];

  /* Minimum payload size that requires Aux Pilot to be transmitted. */
  uint8 aux_pilot_min_payload;

  /* DRC channel gain */
  uint16 drc;
   
  /* Requested DRC. */
  uint16 drc_requested;

  /* Link ID for packet's serving sector. */
  uint8 link_id;
	
  /* Session close reason. */
  uint8 session_close_reason;

  /* BCMCS sleep cycle. */
  uint8 bcmcs_sleep_cycle;

  /* HDR Ec/Io. */
  int16 ecio;

} hdrdiag_hdr_protocol_data_subpkt_protocol_data_type;


/* Response for HDR generic packet request */
typedef PACK( struct )
{
  diagpkt_subsys_header_type xx_hdr;
  
  /* Version */
  uint8 gen_pkt_version;
  
  /* Number of subpackets in the response */	
  uint8 num_subpkts;

  /* Reserved for future use */
  uint16 reserved;
    
  /* HDR system parameter subpacket */
  hdrdiag_hdr_protocol_data_subpkt_protocol_data_type subpkt_protocol_data;

} hdrdiag_hdr_protocol_data_rsp_type;

/*------------------------------------------------------------------------
                  HDR MDSP Slot Logging Data types
-------------------------------------------------------------------------*/
/* Number of slots logged by DSP at a time for slot logging */
#define HDRLOG_MDSP_LOG_BUFF_SIZE_SLOTS             4

/* MDSP Slot logging clients type */
typedef enum
{
  HDRLOG_SRCH_MDSP_SLOT_LOGGING_CLIENT,
  HDRLOG_FIRST_MDSP_SLOT_LOGGING_CLIENT = HDRLOG_SRCH_MDSP_SLOT_LOGGING_CLIENT,
  HDRLOG_RMAC_MDSP_SLOT_LOGGING_CLIENT,
  HDRLOG_FMAC_MDSP_SLOT_LOGGING_CLIENT,
  HDRLOG_SRCHDIV_MDSP_SLOT_LOGGING_CLIENT,
  HDRLOG_AMAC_MDSP_SLOT_LOGGING_CLIENT,
  HDRLOG_MAX_NUM_MDSP_SLOT_LOGGING_CLIENTS
} hdrlog_mdsp_slot_logging_client_type;

/* MDSP Slot logging client registration callback parameters type */
typedef struct
{
  uint16 slot_time;
    /* Slot time corresponding to first index of the buffer */
  uint8 rx_log_buff_idx;
    /* Index of the double-buffered mDSP Rx log buffer to read from */
  uint8 tx_log_buff_idx;
    /* Index of the double-buffered mDSP Tx log buffer to read from */
  uint8 rx_buff_valid_mask;
    /* Bit mask denoting if Rx log buffer is valid */
  uint8 rx_tx_buff_valid_mask;
    /* Bit mask denoting the carriers for which both Rx and Tx log buffers
       were valid. */
} hdrlog_mdsp_slot_logging_data_type;

/* MDSP Slot logging call back type */
typedef void (*hdrlog_mdsp_slot_logging_client_cb_type)
(
  hdrlog_mdsp_slot_logging_data_type *sl_params 
);

/*=========================================================================

FUNCTION HDRLOG_IS_DIPSW_SET

DESCRIPTION
  This helper function checks to see if a dip switch is set in the mask

DEPENDENCIES
  None

PARAMETERS   
  dipsw - The dip switch to check.

RETURN VALUE
  TRUE - dip switch is set
  FALSE - dip switch is not set

SIDE EFFECTS
  None

=========================================================================*/
extern boolean hdrlog_is_dipsw_set( hdrlog_dipsw_enum_type dipsw );

/*=========================================================================

FUNCTION HDRLOG_SET_DIPSW_CB

DESCRIPTION
  Installs callback functions which are called when the indicated dip 
  switch is set or reset.  This allows clients to use callback functions
  to trigger events rather than polling (although polling is certainly
  supported).

  Note that callback functions will be run in diag task context.
  
DEPENDENCIES
  None

PARAMETERS
  dipsw - The dip switch which is to be associated with callbacks.
  set  - Function to call when dip switch is set.
  reset - Function to call when dip switch is reset (cleared). 
  
RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern void hdrlog_set_dipsw_cb
(
  hdrlog_dipsw_enum_type dipsw,
  void (*set)(void),
  void (*reset)(void)
);

/*==========================================================================

                     FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/

/*=========================================================================

FUNCTION HDRLOG_POWERUP_INIT

DESCRIPTION
  This function initializes the diag log on demand
  
DEPENDENCIES
  None
  
PARAMETERS
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None

=========================================================================*/
void hdrlog_powerup_init( void );
         
/*=========================================================================

FUNCTION HDRLOG_ON_REQUEST_DISPATCH

DESCRIPTION
  Diagnostic command - Send a log packet upon request.  I log packet can 
  only be sent on demand if it has previously been registered with diag.

DEPENDENCIES
  None
  
PARAMETERS
  log_code - the log_code of the log packet we would like to log

RETURN VALUE
  log_on_demand_status_enum_type - status of the log request.  
  We are currently only supporting the following:
    LOG_ON_DEMAND_SENT_S
    LOG_ON_DEMAND_NOT_SUPPORTED_S

SIDE EFFECTS
  None

=========================================================================*/

extern log_on_demand_status_enum_type hdrlog_on_request_dispatch
( 
    log_code_type  log_code
);

/*=========================================================================

FUNCTION HDRLOG_LOG_FWD_PKT_SNAPSHOT_RECORD

DESCRIPTION
 ^@Logs a TBD snapshot of the forward link packet supplied by the decoder
  at the AT.  As this packet needs to be used for debugging only, the exact
  details of byte fields can be decided on need basis.

DEPENDENCIES
  None

PARAMETERS
  type      - Packet Type ( 0=Control, 1=Traffic )
  num_bytes - number of bytes in this packet
  data_ptr  - pointer to the data to be logged

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern void hdrlog_log_fwd_pkt_snapshot_record 
( 
        uint8  type,    
        uint8  num_bytes, 
  const uint8  *data_ptr  
);

/*=========================================================================

FUNCTION HDRLOG_LOG_REV_PKT_SNAPSHOT_RECORD

DESCRIPTION
  Logs a TBD snapshot of the reverse link packet supplied by the decoder
  at the AT.  As this packet needs to be used for debugging only, the exact
  details of byte fields can be decided on need basis.

DEPENDENCIES
  None

PARAMETERS
  type      - packet type ( 0=Access, 1=Traffic )
  num_bytes - number of bytes in the packet
  data_ptr  - pointer to the data this is logged

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern void hdrlog_log_rev_pkt_snapshot_record 
( 
        uint8  type,    
        uint8  num_bytes, 
  const uint8  *data_ptr  
);

/*=========================================================================

FUNCTION HDRLOG_LOG_DEBUG_HEX_DUMP_RECORD

DESCRIPTION
  This packet is a generic HDR log packet that used for debugging
  purposes

DEPENDENCIES
  None

PARAMETERS
  num_bytes - number of bytes in the packet
  data_ptr  - pointer to the data this is logged

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern void hdrlog_log_debug_hex_dump_record
(
        uint16  num_bytes,
  const uint8  *data_ptr
);

#ifdef FEATURE_HDR_BCMCS
/*=========================================================================

FUNCTION HDRLOG_LOG_MCS_CHAN_METRICS

DESCRIPTION
  Logs HDR BCMCS logical channel metrics (the HDR_MCS_LOG_CHAN_METRICS
  packet).

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern void hdrlog_log_mcs_chan_metrics(void);
#endif

/*=========================================================================

FUNCTION HDRLOG_START_CONN_STATE_STATS_PERIODIC_TIMER

DESCRIPTION
  This function starts the connected state statistics timer so that we periodically
  log statistics.

DEPENDENCIES
  None

PARAMETERS
  timer_is_on - Whether to turn the timer on or off.

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern void hdrlog_start_conn_state_stats_periodic_timer
(
  boolean timer_is_on
);

#ifdef FEATURE_MDSP_DEBUG_LOGGING
/*=========================================================================

FUNCTION HDRLOG_MDSP_DEBUG_RECORD

DESCRIPTION
  This function gets a log record from the diag log buffer, fills it with 
  the latest mdsp debug logging information and commits it for delivery. 
  
DEPENDENCIES
  None

PARAMETERS
  read_offset  - Offset from the start of the MDSP Log buffer
  chunk_length - Length of the MDSP chunk/packet in words

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
extern void hdrlog_mdsp_debug_record
( 
  uint16 read_offset, 
    /* Offset from the start of the MDSP Log buffer */ 

  uint16 chunk_length 
    /* Length of the MDSP chunk/packet in words */
);
#endif /* FEATURE_MDSP_DEBUG_LOGGING */

/*===========================================================================

FUNCTION HDRLOG_PROCESS_CMD

DESCRIPTION
  This function processes commands given to logging module.

DEPENDENCIES
  None

PARAMETERS
  log_cmd_ptr - Pointer to the command for LOG to process  

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrlog_process_cmd( hdrlog_cmd_type * log_cmd_ptr );

/*===========================================================================

FUNCTION HDRLOG_REGISTER_SLOT_LOGGING_CB

DESCRIPTION
  This function is used by other modules to register a call back for mDSP
  slot based logging.

DEPENDENCIES
  None

PARAMETERS
  client_id - Registering client Identifier
  cb - Registered Call back for mDSP slot based logging.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void hdrlog_register_slot_logging_cb
(
  hdrlog_mdsp_slot_logging_client_type client_id,
  hdrlog_mdsp_slot_logging_client_cb_type cb
);

/*===========================================================================

FUNCTION HDRLOG_MDSP_ISR

DESCRIPTION
  This function is the interrupt service routine for the logging interrupt
  given by the mDSP.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
#if defined T_MDM9X00 || defined FEATURE_HDR_DIME_MODEM
void hdrlog_mdsp_isr( int active_buffer );
#else
void hdrlog_mdsp_isr( void );
#endif 

/*===========================================================================

FUNCTION HDRLOG_ENABLE_MDSP_LOG_ISR

DESCRIPTION
  This function enables mDSP logging ISR if necessary. It checks if there are
  any registered call backs. If there is none, the log ISR is not enabled.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_enable_mdsp_log_isr( void );

/*=========================================================================

HDR_DTX_MODE
  Command is used to set Dis-continuous Transmission Mode. It takes two 
  arguments as follows.
    action - Override DTXMode with user preference/Reset DTXMode to
             negotiated value.
    mode - DTXMode (HDRLOG_DTX_MODE_DISABLED or HDRLOG_DTX_MODE_ENABLED)
    
DESCRIPTION
   
  This is implemented as a diag pkt with the following chain of cmd codes:
  - command_code       = DIAG_SUBSYS_CMD_F     (75)
  - subsystem_id       = DIAG_SUBSYS_HDR       (5)
  - subsystem_cmd_code = HDR_DTX_MODE          (16)
  
=========================================================================*/

/* HDR DTX Mode Command actions */
typedef enum 
{
  HDRLOG_RESET_DTX_MODE  = 0,      
    /* Reset DTX Mode to negotiated value */

  HDRLOG_OVERRIDE_DTX_MODE = 1,    
    /* Override DTX Mode with user preference */

  HDRLOG_DTX_MODE_ACTION_INVALID = 0xFF
    /* Invalid action */

} hdrlog_dtx_mode_action_enum_type;

typedef enum
{
  HDRLOG_DTX_MODE_DISABLED = 0x0,
    /* DTXMode off */

  HDRLOG_DTX_MODE_ENABLED = 0x1
    /* DTXMode on */

} hdrlog_dtx_mode_enum_type;

/* Set/Reset DTXMode request/response */
typedef PACK( struct )
{
  diagpkt_subsys_header_type xx_hdr;
    /* Command Header */

  hdrlog_dtx_mode_action_enum_type action; 
    /* Reset/Override DTX Mode */
  
  hdrlog_dtx_mode_enum_type mode;
    /* DTXMode on/off */

} hdrdiag_dtx_mode_req_type;


/*===========================================================================

FUNCTION HDRLOG_PROCESS_COLLECT_FTD_STATS

DESCRIPTION
  This function processes command to start or stop collecting FTD stats.

DEPENDENCIES
  None

PARAMETERS
  bit_mask - bit mask that indicates what data to collect and report.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_process_collect_ftd_stats( uint32 bit_mask );

/*===========================================================================

FUNCTION HDRLOG_CM_GET_FTD_STATS_REPORT

DESCRIPTION
  This function is used by CM to get all FTD stats.

DEPENDENCIES
  None

PARAMETERS
  ftd_stats_report - Pointer to location where the FTD stats need to be copied.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void hdrlog_cm_get_ftd_stats_report
( 
  sys_hdr_information_s_type *ftd_stats_report 
);

/*===========================================================================

FUNCTION HDRLOG_PROCESS_TIMER

DESCRIPTION
  This function processes all timer expiration events for LOG.
  
DEPENDENCIES
  None

PARAMETERS
  timer_id - Timer ID of the expired timer

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrlog_process_timer
(
  uint32 timer_id
);

#ifdef FEATURE_HDR_MODEM_API
/*=========================================================================

HDRDIAG_MODEM_API_DATA_RSP_TYPE
  Contains the Modem API Data report

DESCRIPTION

  - cost_of_power_est       = INVALID(0), LOW(1), MDEIUM(2), HIGH(3)
  
=========================================================================*/
/* Not packed since all fields are aligned. */
typedef struct
{
  diagpkt_subsys_header_type xx_hdr;

  uint8    cost_of_power_est;

} hdrdiag_modem_api_data_rsp_type;
#endif /* FEATURE_HDR_MODEM_API */

#endif /* HDRLOG_H */
