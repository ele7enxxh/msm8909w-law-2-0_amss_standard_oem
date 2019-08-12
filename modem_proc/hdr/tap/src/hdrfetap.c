/*===========================================================================

         H D R    E N H A N C E D   T E S T   A P P L I C A T I O N


GENERAL DESCRIPTION
  This file contains the Defintions and Implementations for the HDR Forward 
  link Enhanced Test Application ( FETAP ).

EXTERNALIZED FUNCTIONS

  hdrfetap_activate()       - One time initialization for fetap data 
  hdrfetap_deactivate()     - Deactive the fetap
  hdrfetap_is_in_traffic()  - Check if the fetap is in the traffic state
  hdrfetap_process_ind()    - FETAP processes the indication
  hdrfetap_msg_cb()         - Callback registered for incoming msgs
  hdrfetap_process_sig()    - FETAP processes the signals ( TX task )
  hdrfetap_put_pkt()        - FETAP incoming pkt processing
  hdrfetap_ind_cb()         - Callback registered for delivery of indications
  hdrfetap_disable_tc_ofs() - i/f with IS890a for the dip switch set/reset

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hdrfetap_activate() has to be called before the FETAP can be activated.

Copyright (c) 2001 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrfetap.c_v   1.30   09 Apr 2003 11:24:28   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/tap/src/hdrfetap.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
12/19/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
08/19/13   rmg     Added support to disable DLNA in TAP mode.
03/16/12   arm     Resolve TCB errors for Dime. 
09/16/11   wsh     Reduce heap mem usage by moving memory back to static
05/24/11   ssu     Included "hdrpac_api.h" and "dsrlp_api.h" 
05/24/11   ssu     Removed inclusion of "hdrpac_mdm.h"
03/24/11   kss     Updated RF SVDO API support.
03/15/11   arm     Fix compiler error, missing semi colon. 
10/04/10   rmg     Corrected MC connected ss stats reporting.
09/02/10   wsh     CMI: removed obsoleted clk.h for all except clk_busy_wait
08/05/10   rmg     Shifted call to disable throttling from parameter assignment
                   complete callback to parameter assignment processing.
06/21/10   cnx     Fixed compiler warnings.
03/15/10   rmg     Fixed Klocwork error: out-of-bounds access of lo->records[].
03/10/10   rmg     Added check for large time gap before idle pkt generation.
10/27/09   wsh     Fixed compiler warning for ARM compiler
10/20/09   rmg     Fixed incorrect use previous packet status based sync in 
                   Rel0 specific code.
09/28/09   etv     Fixed Klocwork errors.
09/01/09   etv     Used the right max valid value for FTC_PL_3PayloadSizeIndex.
09/09/09   wsh     CMI: to support changes in clk and ts APIs
08/27/09   wsh     CMI:Split HDRIS890 due to external reference in OEMCM.c  
06/23/09   rmg     Fix to avoid multiple registrations with PCP when disabling
                   and re-enabling loopback mode.
06/17/09   rmg     Corrected check for valid carrier_id in 
                   get_freq_from_carrier_id().
06/08/09   rmg     Change to initialize loopback pkt buffer after handoff.
05/27/09   rkc     Added FEATURE_RF_SVDO_API changes.
05/21/09   kss     Handle HO ind when it occurs during traffic (for HO count)
05/20/09   kss     Include hdrpac_mdm for moved hdrpac_get_rtcmac_flow().
04/16/09   rkc     Mainlined T_MSM7800.
04/06/09   etv     Fixed bug caused due to incorrect variable name.
                   Handle error cases in ParamAssign msg processing code.
                   Used hdrhmp_app_send_msg instead of existing_rtc API.
03/31/09   kss     Ignore handoff indication that arrives after conn.end.
02/24/08   rmg     Fixed PER error when transmitting split packets.
01/27/09   wsh     Fixed doing intra SCC sleep when sleep_cap_done recvd.
08/05/08   rmg     Added support for HDRIND_CCMAC_SCC_PKT_RXED indication.
01/26/08   rmg     Removed fix to copy full records for IDLE packets.
12/10/08   smd     Replaced assert.h with amssassert.h
11/20/08   kss     Removed/replaced intlocks.
10/13/08   kss     Lint fixes.
08/22/08   rmg     Fix to copy full records for IDLE packets to fill lower 
                   layer packet size.
07/03/08   kss     Added NULL check.
06/09/08   etv     Sort SubASETIdx based on frequency.
05/14/08   etv     Use num_sub_asets as loop bound when programming TAP modes.
01/10/08   cws     CARRIER_MAP_CHANGED should only affect subtype 3.
12/19/07   cws     Added handling of HDRIND_LMAC_CARRIER_MAP_CHANGED.
12/14/07   cws     Fixed a split packet corner case. Allocate large buffer
                   so FMCTAP can pick which carrier to transmit.
12/13/07   cws     Added back in transmission_check for T_MSM7500.
12/05/07   cws     Fixed get_buf_size bug for multi-carrier.
12/05/07   cws     Removed FEATURE_HDR_IS890_RMAC3_TRANSMISSION_CHECK.
12/05/07   cws     Added FEATURE_HDR_IS890_RMAC3_TRANSMISSION_CHECK.
12/04/07   cws     Cleaned up F3 messages. Fixed sanity checks for MC.
10/02/07   jyw     Added FEATURE_HDR_ETAP_DISABLE_CTA_TIMER.
10/02/07   cws     Added carrier index to sub-activeset mapping and some cleanup
09/13/07   cws     Added LoopbackModeMC configuration
09/12/07   cws     Added check for HDRPCP_NO_COMBO_ID_ACCEPTED
08/27/07   cws     Modularized the Loopback code in prep for MC data path.
08/24/07   cws     Added FMCTAP data path code.
08/22/07   cws     Added FMCTAP control path code.
08/07/07   cws     Cleaned up types for consistency. Removed use of hdrrx pn 
                   roll sig for system time tracking. PL0 change for RevB PCP.
08/01/07   grl     Fixed RCVT 2.2 compiler warnings.
07/24/07   etv     Removed hdrrx_reset_cc_decode_attempts.
07/25/07   cc      Used new hdrpcp_register_transmit_callbacks interface.
09/07/06   jyw     Fixed bugs in num of decoding slots and PCP registration.
07/18/06   jyw     Added transmission check.
06/15/06   jyw     Added PCP unregistration at connection close.
06/15/06   jyw     Fixed the logic for the fixed ack mode.
05/22/06   ljl     Notified HDR srch when TAP is running.
05/03/06   jyw     Removed the lint suppressions.
04/25/06   jyw     Fixed the bug in the lo management with F3 category changes.
04/25/06   mpa     Added hdrrmac.h include (RVCT compiler)
03/29/06   jyw     Fixed the packet type in the PL2 loopback packets headers.
02/16/06   jyw     Added fetap/lmac interfaces to disable the TC OFS in TAP calls.
02/09/06   jyw     Removed the function predefinitions.
01/30/06   jyw     Conditionally reset the FETAP at the connection close ind.
12/02/05   jyw     Disabled revlink throttling during the FETAP test.
11/17/05   jyw     Changed the logic to enforce the LO buf refreshement.
11/17/05   jyw     Changed the GetStatsResponse as a best effort transmission.
11/17/05   jyw     Fixed lint errors and Added some editorial changes.
11/03/05   grl/jyw Fixed serving sector handoff logging. 
10/28/05   jyw     Fixed the lint errors.
10/13/05   jyw     Changed a variable name and Added a debug message.
10/12/05   jyw     Added support to the Anritsu 8820 call box.
09/15/05   jyw     Fixed the bug in the LO buffer management and code clean up.
09/09/05   jyw     Undefined the debug feature.
09/08/05   jyw     Replaced TAP0 with TAPa.
09/08/05   jyw     General Clean up, more comments, variables consolidations.
08/02/05   jyw     Installed the FMAC interface and other cleanups.
07/23/05   jyw     Cleaned up the interface.
05/18/05   jyw     Created the file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "target.h"

#if defined ( FEATURE_HDR_IS890 ) || defined ( FEATURE_HDR_IS890A )

#include "rex.h"

#include "bit.h"
#include "dsm.h"
#include "dsmbit.h"
#include "task.h"
#include "hdrdebug.h"
#include "err.h"
#ifdef FEATURE_RF_SVDO_API
#include "rfm_cdma.h"
#else
#include "rf.h"
#include "rf1x_core_rf.h"
#endif /* FEATURE_RF_SVDO_API */

#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"
#include "hdralmp.h"
#include "hdrdsm.h"
#include "hdrtx.h"
#include "hdrfmac.h"
#include "hdrts.h"
#include "hdrhmp.h"
#include "hdrrup.h"
#include "hdrpcp.h"
#include "hdrcmac.h"
#include "hdrrmac.h"
#include "hdrsrch.h"

#include "hdris890.h"
#include "hdris890_v.h"
#include "hdrftap.h"
#include "hdrscp.h"
#include "hdrpac_api.h"
#include "dsrlp_api.h"

#include "hdridle.h"
#include "hdrlmac.h"

#include "hdrutil.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#ifdef FEATURE_HDR_DSMREUSE_OPT
  #include "hdrds.h"
#endif

#ifdef TIMETEST
  #error code not present
#endif /* TIMETEST */
#include "amssassert.h"

#ifdef FEATURE_HDR_ETAP_DISABLE_CTA_TIMER
#include "dsrlp.h"
#endif /* FEATURE_HDR_ETAP_DISABLE_CTA_TIMRE */
/*===========================================================================

                     DEFINES AND MACROS

===========================================================================*/
#define HDRFETAP_BITS_PER_BYTE                    8
  /* Number of bits per byte */

#define HDRFETAP_LO_RECORD_BUF_SIZE      ( 16 * 16 )
  /* Total size of the loopback record buffer (16 rec's for 16 lo pkts) */

#define HDRFETAP_LO_FUL_RECORD_LEN_PL01  33
  /* Loop back packet FULL Record bit length for both PHY subtype 01 */

#define HDRFETAP_LO_FUL_RECORD_LEN_PL2   35
  /* Loop back packet FULL Record bit length for both PHY subtype 2 */

#define HDRFETAP_LO_FUL_RECORD_LEN_PL3   39
  /* Loop back packet FULL Record bit length for both PHY subtype 3 */

#define HDRFETAP_LO_MIN_RECORD_LEN_PL01  11
  /* Loop back packet MIN Record bit length for both PHY subtype 01 */

#define HDRFETAP_LO_MIN_RECORD_LEN_PL2   13
  /* Loop back packet MIN Record bit length for both PHY subtype 2 */

#define HDRFETAP_LO_MIN_RECORD_LEN_PL3   17
  /* Loop back packet MIN Record bit length for both PHY subtype 3 */

#define HDRFETAP_LO_TEST_PKT_HEADER_LEN  29
  /* Loop back packet heander length in bit */

#define HDRFETAP_LO_MIN_PKT_BYTE_SIZE    9
  /* Minimum byte length of a one-full-record loopback packet */

#define HDRFETAP_IDLE_SYS_TIME_FLAG      0x8000
  /* Flag to indicate a empty record w/o forward test packets */

#define HDRFETAP_LO_PKT_DBG
  /* Define this to debug loopback packet problems */

#define HDRFETAP_MSG_SIG                 HDRTX_IS890_FTAP_MSG_SIG
  /* Message processing signal */

#define HDRFETAP_MSG_QUEUE_SIZE          4
  /* Fetap message processing queue (size = 4) */

#define HDRFETAP_SYNC_TIME_26MS_COUNT    12
    /* Minimum time in PN rolls to wait after NETWORK_ACQ indication 
      * before system time is actually valid:                        
      *  - < 10 ms before search get's the command                  
      *  - up to 30 ms if search currently processes a dump 
      *  - 160 ms for the jump to hyperspace               
      *  - one more PN roll to ensure frame time is valid 
      *  => about 300 ms (12 PN rolls)  */

#define HDRFETAP_SUBSTREAM_ID            0
  /* Fetap substream id has to be zero; per spec */

#define HDRFETAP_DEFAULT_SUBACTIVE_SET   0 
  /* Default sub-active set for single carrier, PL_0_1_2 use */

#define HDRFETAP_DEFAULT_CARRIER         0 
  /* Default carrier index for single carrier, PL_0_1_2 use */

#define HDRFETAP_INVALID_SUBACTIVE_SET   0xFF
  /* Invalid sub-active set marker for multi-carrier */

#define HDRFETAP_LO_MODE_BUF_SIZE_PL3    0xFFFF
  /* Return large number so FMCTAP can control which carrier to tx */

#define  HDRFETAP_L0_BUF_REFRESH_MAX_CATCHUP_FRAMES 8
  /* Maximum number of frames buffer refresh function should
     try to catch-up in case of a time loss */

#define  HDRFETAP_L0_BUF_REFRESH_OVERFLOW_BIT_SET_THRESHOLD 16
  /* If buffer refresh function needs to catch up more than this 
     many frames, it should set the overflow bits and not generate
     idle catch-up frames.  This is useful for long idle times 
     e.g. DRC supervision. */

#define HDRFETAP_MAX_FTC_PL_3_PAYLOAD_SIZE_INDEX 0xA
  /* Maximum valid value for FTC_PL_2PayloadSizeIndex */

/* Macro to set the loopback packet overflow bit */

#define HDRFETAP_SET_LO_OVERFLOW_BIT( di_ptr )                         \
  *((di_ptr)->data_ptr + 2) |= 0x01;

/* Macros to get, set, and increment the current packet's record count */

#define HDRFETAP_LO_RECORD_COUNT( di_ptr )                             \
  ((*((di_ptr)->data_ptr + 3) & 0xf8) >> 3)

#define HDRFETAP_SET_LO_RECORD_COUNT( di_ptr, rcnt )                   \
  *((di_ptr)->data_ptr + 3) &= 0x07;                                   \
  *((di_ptr)->data_ptr + 3) |= ((rcnt) << 3);

#define HDRFETAP_INCR_LO_RECORD_COUNT( di_ptr )                        \
  {                                                                    \
    uint8 tt = (*((di_ptr)->data_ptr + 3) & 0xf8) >> 3;                \
    HDRFETAP_SET_LO_RECORD_COUNT( di_ptr, tt + 1 );                    \
  }

#define HDRFETAP_GET_MSG_ATTR( msg_ptr, attr, num_bytes )              \
           ( hdrerrno_enum_type )                                      \
           (( dsm_pullup( msg_ptr, &attr, num_bytes) != num_bytes ) ?  \
             E_NO_DATA: E_SUCCESS)

/*===========================================================================

                  INTERNAL DATA STRUCTURES

===========================================================================*/

typedef enum
{
  PHY_SUBTYPE_0 = 0x0,
  PHY_SUBTYPE_1,
  PHY_SUBTYPE_2,
  PHY_SUBTYPE_3,
  PHY_SUBTYPE_INVALID
} 
hdrfetap_phy_subtype_enum_type;
  /* Physical layer protocol subtype */

typedef enum 
{
  LOM_NON_PERSISTENT = 0x00,
    /* None persistent mode */

  LOM_PERSISTENT,
    /* Persistent mode */

  LOM_RESERVED  

}
hdrfetap_loopback_mode_config_enum_type;
  /* Loopback configuration mode */

typedef enum 
{
  PCP_CALLED_PRE_PKT_STATUS = 0x0,
    /* Indicates the status of prev. pkt has been updated */

  PCP_CALLED_GET_PKT
    /* indicates the lb pkt has been retrieved from the PCP */

}
hdrfetap_sync_flag_enum_type;
  /* The sync flag is used to check the synchronization b/w the PCP
  call back functions "get_pkt" and "prev_pkt_status" */

typedef struct
{
  uint8 substream_id; 
    /* The substream id is expected to be zero; because the 
    FETAPParameterAssignmentMsg does not provide the substream_id */

  uint8 rtcmac_flow_id; 
} 
hdrfetap_flow_config_type;
  /* Flow configuration for RMAC */
  
typedef enum 
{
  ACFM_EARLY_TERMINATE_ALL_PKTS = 0x00,
    /* early termination for all pkts */

  ACFM_EARLY_TERMINATE_NO_PKTS, 
    /* no early termination for all pkts */

  ACFM_RESERVED  

} 
hdrfetap_ack_channel_fixed_mode_enum_type; 
  /* Ack channel mode configuration enumeration */

typedef enum
{
  ACKCH_MOD_TYPE_BPSK = 0x0,
    /* ACK channel modulation type BPSK */

  ACKCH_MOD_TYPE_OOK,
    /* ACK channel modulation type OOK */

  ACKCH_MOD_RESERVED

} 
hdrfetap_ack_channel_mod_enum_type;
  /* Ack channel modulation configuration enumeration */

typedef struct
{
  hdrhai_stream_enum_type                       stream_id;
    /* Fetap gets a stream assigned shared with retap */ 

  hdrhai_protocol_name_enum_type                protocol_id;
    /* Protocol ID based off of the stream ID */
    
  uint8                                         param_assign_seqno;
    /* Transacation ID of the last config */

  boolean                                       param_assign_seqno_is_valid;
    /* Flag indicating parameter assignment msg sequence number is valid */
    
  uint8                                         stats_clear_seqno;
    /* Transacation ID of the last stats clear*/

  boolean                                       stats_clear_seqno_is_valid;
    /* Flag indicating stats clear msg sequence number is valid */
    
  uint8                                         stats_get_seqno;
    /* Transacation ID of the last stats get */

  boolean                                       stats_get_seqno_is_valid;
    /* Flag indicates that sequence number is valid */
  
  hdrfetap_phy_subtype_enum_type                phy_subtype;
    /* Physical layer subtype value */

  uint16                                        app_subtype;
    /* Application subtype */

  boolean                                       loopback_mode;    
    /* Loopback Mode: Send info on received FL pkts back to the AN */

  hdrfetap_loopback_mode_config_enum_type       loopback_mode_config;
    /* Configures whether loopback is persistent or not */

  hdrfetap_sync_flag_enum_type                  sync_flag[HDRIS890B_NUM_CARRIERS];
    /* Flag error checks PCP calls and makes sure they are in order */

  hdrfetap_flow_config_type                     flow_config;
    /* PCP flow configuration structure */

  boolean                                       ack_channel_fixed_mode;  
    /* Fixed ACK Channel Mode: Early termination or NAL for all */

  hdrfetap_ack_channel_fixed_mode_enum_type     ack_channel_fixed_mode_config;
    /* ACK channel early termination configuration */

  boolean                                       drc_value_fixed_mode;  
    /* Fixed DRC Value Mode: Send fixed DRC value ignoring predictor */

  hdrhai_drc_value_enum_type                    drc_value_fixed_mode_config;
    /* DRC value to be used with fixed DRC mode */

  boolean                                       drc_cover_fixed_mode;    
    /* Fixed DRC Cover Mode: Send DRC's to one sector only ignoring PSMM's */

  uint8                                         drc_cover_fixed_mode_config;
    /* DRC cover value to be used with DRC cover fixed mode */

  boolean                                       ack_channel_modtype_fixed_mode;
    /* ACK channel mod type; works together with ACK channel bit fixed mode */

  hdrfetap_ack_channel_mod_enum_type            ack_channel_modtype;
    /* ACK channel modulation type to be used with ack channel modulation type 
  fixed mode */

  boolean                                       tc_ofs_enabled;
    /* if enable the TC OFS; FALSE as default */

  boolean                                       if_tx_rgsted;
    /* The if_tx_rgsted indicates if any registration has been done to the PCP.
   * this is necessary because the RETAP and FETAP may share the same 
   * substream, for example 0. */

  uint8                                         tca_msg_seq_mc;
    /* The message sequence of the TCA that the MC parameters are associated */

  boolean             loopback_mode_mc[HDRIS890B_NUM_CARRIERS];    
    /* Array of which sub-active sets have loopback mode enabled */

  hdrfetap_loopback_mode_config_enum_type       
                      loopback_mode_config_mc[HDRIS890B_NUM_CARRIERS];
    /* Configures whether loopback is persistent or not per sub-active set */

  boolean             ack_channel_fixed_mode_mc[HDRIS890B_NUM_CARRIERS];  
    /* Array of which sub-active sets (if any) to enable DRC value fixed */

  hdrfetap_ack_channel_fixed_mode_enum_type     
                      ack_channel_fixed_mode_config_mc[HDRIS890B_NUM_CARRIERS];
  /* Config value for ACK channel fixed mode */

  boolean             drc_value_fixed_mode_mc[HDRIS890B_NUM_CARRIERS];
    /* Array of which sub-active sets (if any) to enable DRC value fixed */

  hdrhai_drc_value_enum_type  
                      drc_value_fixed_mode_config_mc[HDRIS890B_NUM_CARRIERS];

    /* Config value for DRC value fixed mode */
  
  boolean             drc_cover_fixed_mode_mc[HDRIS890B_NUM_CARRIERS];
    /* Array of which sub-active sets (if any) to enable DRC cover fixed */

  uint8               drc_cover_fixed_mode_config_mc[HDRIS890B_NUM_CARRIERS];
    /* Config value for DRC cover fixed mode */

  boolean             ack_channel_modtype_fixed_mode_mc[HDRIS890B_NUM_CARRIERS];
    /* Array of which sub-active sets (if any) to enable ACK mod fixed */

  hdrfetap_ack_channel_mod_enum_type            
                      ack_channel_modtype_mc[HDRIS890B_NUM_CARRIERS];
    /* Config value for ack channel modulation type */

} hdrfetap_config_type;

/* FETAP statistics the same as the one in the FTAP */

typedef struct
{
  uint16  idle_asp_change;
    /* Number of idle handoffs */

  uint32  idle_time;             
    /* Time spent in idle state */

  uint16  connected_ss_change;   
    /* Number of traffic channel handoffs */

  uint32  connected_time;        
    /* Time spent in connected state */

  uint16  first_sync_cc_pkt;     
    /* Count of syncr. CC cycles decoded */

  uint16  cc_time;               
    /* Time spent receiving the CC */

  uint32  last_slot_time_stamp;  
    /* Internal: Calculate slot time lapses */

  uint16  connected_ss_change_mc[HDRIS890B_NUM_CARRIERS];
    /* Number of traffic channel handoffs multi-carrier, index is sas ID */
} hdrfetap_stats_type;
  /* Configuation attributes for FETAP */


typedef enum
{
  HDRFETAP_INACTIVE = 0x0,
  HDRFETAP_NOT_ACQ,
  HDRFETAP_SYNC_TIME_WAIT,
  HDRFETAP_IDLE,
  HDRFETAP_TRAFFIC
} hdrfetap_state_enum_type;
  /* Current FETAP state enumeration */

uint16 hdrfetap_sync_time_wait_duration = 0;
  /* Global sync time wait duration, this is global so initialization can occur
  prior to FETAP instantiation */

typedef struct
{
  uint16  fwd_sys_time;
    /* 16 bit system time in the uint of frame */

  uint8   full_rec[6];
    /* Full_length record for incoming test packet */

  uint8   min_rec[3];
    /* Min_length record for incoming test packet */

  uint16  fwd_seq;
    /* The fwd seq. number of the incoming test packet */

#ifdef HDRFETAP_LO_PKT_DBG
  uint8   tca_msg_seq;
    /* DEBUG: TCA message sequence number */

  uint8   drc_cover;
    /* DEBUG: DRC cover for loopback packet */

  uint8   fwd_phys_slots;
    /* DEBUG: Number of slots to decode for loopback packet */

  uint8   fwd_mac_pkts;
    /* DEBUG: Number of MAC packets for loopback packet */

  uint8   pkt_size;
    /* DEBUG: Size of loopback packet */
#endif

} hdrfetap_lo_record_type;
  /* Structure to contain individual loopback records */

typedef struct
{
  uint8                   write_index;
    /* Write index into loopback record buffer. Each loopback packet
    received increments write index. */

  uint8                   read_index;
    /* Read index into loopback record buffer. Updated when loopback 
    packet is transmitted on RL. */

  uint8                   tmp_read_index;   
    /* Temp read index for loopback record buffer. This value is saved
    as read_index when previous packet status is success. */

  uint16                  last_fwd_sys_time;
    /* The next system time in the record buffer */

  uint8                   last_drc_cover_field;
    /* The serving sector DRC cover */

  uint8                   current_tca_seqno;
    /* The current TCA seqno info */

  uint8                   current_tca_upper_byte;
    /* The current TCA seqno info upper byte stored for lo msg */

  uint8                   current_tca_lower_byte;
    /* The current TCA seqno info lower byte stored for lo msg */

  uint16                  split_pkt_rec_cnt;
    /* Split indicator used when more then one loop back pkts are needed 
    for one observation period */

  uint16                  split_pkt_fwd_sys_time;  
    /* Split indicator used when more then one loop back pkts are needed 
    for one observation period */

  hdrfetap_lo_record_type record[HDRFETAP_LO_RECORD_BUF_SIZE];
    /* The array of the records */

  boolean                 overflow;    
    /* Flag indicates that more FL packets were received than could be tx'd on
    the RL */

  boolean                 overflow_status_sent;
    /* Flag to indicate that overflow status has been sent to AN
    in an outgoing packet.  If the packet is accepted, it is okay 
    to clear the overblow bit. */

  boolean                 enabled;
    /* Flag to indicate that this lo_buff is enabled, this is updated in the
    lo_buff_get_from_carrier_id */

} hdrfetap_lo_buff_type; 
  /* The loopback record buffer is used to 
  store the records for the incoming FETAP test packet;
  each record per packet; when Loop back mode is enabled,
  the loopback packet is generated throughout this record 
  buffer */

typedef struct
{
  dsm_item_type*  msg_ptr[HDRFETAP_MSG_QUEUE_SIZE];
    /* incoming msg buffer */

  uint8           read_index;
    /* read ptr */

  uint8           write_index;
    /* write ptr */

} hdrfetap_msg_buf_type;
  /* Signalling message buffer */
  

typedef struct
{
  uint8   drc_cover; 
    /* Value of DRC cover for test packet */
  
  uint16  pkt_frm_time;
    /* Frame time of received test packet to loopback  */
  
  uint16  pkt_fwd_seq;
    /* Sequence number of received test packet */

  uint8   drc_value;
    /* DRC value test packet was received */
  
  uint8   pkt_size;
    /* Size of packet in 128-bit increments */

  uint8   num_rx_slots;
    /* Number of slots packet took to decode */

  uint8   sas_id;
    /* Sub-active set packet was received */

} hdrfetap_test_pkt;
  /* Test packet contents to pack in to loopback packet */

/*===========================================================================

                   CONSTANTS

===========================================================================*/

/* The Lookup table is the same as previous table, but bit-packed 
 * into the 2 msb's notes: it is only used in PL_01 looopback record */

const uint8 hdrfetap_drc_to_pl01_pkt_field_map[ 13 ] = 
{ 0x00,   /* NULL rate    */
  0x00,   /* 38.4         */
  0x00,   /* 76.8         */
  0x00,   /* 153.6        */
  0x00,   /* 307.2 short  */
  0x40,   /* 307.2 long   */
  0x00,   /* 614.4 short  */
  0x40,   /* 614.4 long   */
  0x80,   /* 921.6        */
  0x40,   /* 1228.8 short */
  0xc0,   /* 1228.8 long  */
  0x80,   /* 1843.2       */
  0xc0    /* 2457.6       */
};

/*===========================================================================

                   LOCAL VARIABLES

===========================================================================*/

typedef struct 
{
  hdrfetap_config_type      configuration;
    /* FETAP config */

  hdrfetap_config_type      stored_configuration;
    /* FETAP temporary config */

  hdrfetap_stats_type       statistics;
    /* FETAP statistics */

  hdrfetap_state_enum_type  state;
    /* FETAP State */

  hdrfetap_lo_buff_type     lo;
    /* loop back record buffer */

  uint32                    lo_pkt_cnt;
    /* loop back pkt counter, used for debug */

  hdrsrch_link_id_type      last_link_id[HDRIS890B_NUM_CARRIERS];
    /* last link id to the ASP */

  uint8                     last_drc_cover[HDRIS890B_NUM_CARRIERS]; 
    /* last DRC cover to the ASP */

  uint16                    previous_pkt_time;
    /* Previous pkt frame time */

  uint16                    last_fwd_seq[HDRIS890B_NUM_CARRIERS];
    /* F3 mssage out purpose */

  uint32                    tst_pkt_cnt;
    /* forward link test pkt counter */

  boolean                   fetap_called;
    /* fetap_called flag indicates if a FETAP has been called inside
     * the current connection; the flag is used to conditionally reset
     * the fetap configruation at the connection close indication */

  hdrfetap_msg_buf_type     msg_queue;
    /* msg queue for the fetap */

#ifdef HDRFETAP_LO_PKT_DBG
  uint16                    lo_start_t;
#endif /* HDRFETAP_LO_PKT_DBG */

  hdrfetap_lo_buff_type     lo_mc[HDRIS890B_NUM_CARRIERS];
    /* loop back record buffer */

  uint8                     carrier_to_freq_idx[HDRIS890B_NUM_CARRIERS];
    /* Storage for carrier to sub-active set index mapping */

  uint16                    tca_seq_for_map;
    /* TCA sequence number for carrier_to_subactive_set */

  uint8                     num_sub_asets;
    /* Number of SubActiveSets */

  boolean                   expecting_idle_ind;
    /* Flag to indicate connection was closed. Used when updating idle
       handoff count. */

} hdrfetap_type;
  /* Structure contains the FETAP specific variables and attributes */


 /* EJECT */
/*===========================================================================
 
                         DYNAMIC MEMORY STRUCTURE
                  
This section contains one large struct to hold all local variables used in 
this file.  Ensures the variables will be defined in heap memory and not ZI 
memory.  All variables will be saved in this struct.
                    
===========================================================================*/
typedef struct /* hdrrmaccmn_mem_type */
{
  /* Structure contains the FETAP specific variables and attributes */
  hdrfetap_type hdrfetap;

  #ifdef TIMETEST_DSRLP_STATS

  /* Callbacks for logging the data throughput through the TIMETEST port */
  timer_type   hdrfetap_timetest_cb;
  boolean      hdrfetap_timetest_cb_is_registered;
  uint16       hdrfetap_num_rcvd_slots;
  uint16       hdrfetap_num_sent_lo_rcds;

  #endif /* TIMETEST_DSRLP_STATS */

} hdrfetap_mem_type;

static hdrfetap_mem_type hdrfetap;

/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRFETAP_GET_SYS_TIME

DESCRIPTION
  This function returns a 16-bit representation of the current system time
  in frames.

DEPENDENCIES
  None
 
PARAMETERS
  None
   
RETURN VALUE
  System time in frames.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
static uint16 hdrfetap_get_sys_time( void )
{
  uint16  sys_time;
  qword   frame_time;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( hdrts_get_frame_time( frame_time ) )
  {
    /* Set the lo packet timestamp to the beginning of this frame 15 bit
       15 bit which is a period of RTC count */

    sys_time = (qw_lo( frame_time ) & 0x00007fff);
  }
  else
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "Could not get frame time" );
    sys_time = 0;
  }

  return sys_time;

} /* hdrfetap_get_sys_time( void ) */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRFETAP_CLEAR_TRANSMIT_CALLBACKS

DESCRIPTION
  This function clears up the FETAP TX registration to PCP when the loopback
  mode was set or if registered before.

DEPENDENCIES
  None
 
PARAMETERS
  None
   
RETURN VALUE
  None
  
SIDE EFFECTS
  None.
  
===========================================================================*/
static void hdrfetap_clear_transmit_callbacks( void )
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( hdrfetap.hdrfetap.configuration.if_tx_rgsted ) 
  {
    if ( hdrpcp_unregister_transmit_callbacks( 
              hdrfetap.hdrfetap.configuration.stream_id,
              HDRFETAP_SUBSTREAM_ID ) )
    {
      hdrfetap.hdrfetap.configuration.if_tx_rgsted = FALSE; 
      HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: dereg the prev. pcp tx cb." );
    }
    else
    {
      HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                     "FETAP: unable to dereg the prev. pcp tx cb." );
    }
  }

} /* hdrfetap_clear_transmit_callbacks( void ) */


/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRFETAP_GET_FREQ_IDX_FROM_CARRIER_ID()

DESCRIPTION
  This function returns the frequency index mapped to the specified
  carrier ID. Carrier ID is used for the data path where the FTAP attributes
  are all configured from sub-activeset index from the latest TCA.

DEPENDENCIES
  None
 
PARAMETERS
  carrier_id - Carrier ID to map to frequency index.
   
RETURN VALUE
  Sub-active set index
  
SIDE EFFECTS
  Updates the hdrfetap.hdrfetap.carrier_to_subactive_set array map.
  
===========================================================================*/
static uint8 hdrfetap_get_freq_idx_from_carrier_id
( 
  uint8 carrier_id 
    /* Carrier ID to map to sub-active set index */
)
{
  hdrlmac_carrier_map_type  carrier_map[HDRIS890B_NUM_CARRIERS];
  uint32                    freq_khz[HDRIS890B_NUM_CARRIERS] = {0};
  uint8                     freq_idx;
  uint16                    sorted_freq_idx = 0;
  uint32                    prev_freq_khz = 0;
  uint32                    next_freq_khz = HDRIS890_INVALID_CHAN_FREQUENCY;
  uint16                    next_carrier = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_0 ||
      hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_1 || 
      hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 )
  {
    freq_idx = HDRFETAP_DEFAULT_SUBACTIVE_SET;
      /* Return default sub-active set info */

    hdrfetap.hdrfetap.carrier_to_freq_idx[HDRFETAP_DEFAULT_CARRIER] = 
      HDRFETAP_DEFAULT_SUBACTIVE_SET;
      /* Update map with default values for single carrier */
  }
  else if( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {
    /* If TCA seq number is a mismatch, need to update carrier map */
    if( hdrfetap.hdrfetap.tca_seq_for_map != hdrrup_get_current_tca_seq_num() )
    {

      hdrfetap.hdrfetap.tca_seq_for_map = hdrrup_get_current_tca_seq_num();

      hdrfetap.hdrfetap.num_sub_asets = 
        hdrlmac_get_carrier_map( HDRIS890B_NUM_CARRIERS, carrier_map );

      ASSERT (hdrfetap.hdrfetap.num_sub_asets <= HDRIS890B_NUM_CARRIERS);

      for( freq_idx = 0; freq_idx < HDRIS890B_NUM_CARRIERS; ++freq_idx )
      {
        hdrfetap.hdrfetap.carrier_to_freq_idx[freq_idx] = 
          HDRFETAP_INVALID_SUBACTIVE_SET;

        /* If SubASET is valid */
        if( freq_idx < hdrfetap.hdrfetap.num_sub_asets )
        {
          /* Convert this channel number to frequency in KHz */
#ifdef FEATURE_RF_SVDO_API
          freq_khz[freq_idx] = 
            rfm_cdma_get_rx_carrier_freq( carrier_map[freq_idx].fwd_channel );
#else
          freq_khz[freq_idx] = 
            rf_get_rx_carrier_freq( 
              (rf_card_band_type) carrier_map[freq_idx].fwd_channel.band, 
              carrier_map[freq_idx].fwd_channel.chan_num );
#endif /* FEATURE_RF_SVDO_API */
        }
      }

      prev_freq_khz = 0;

      /* For each SubActiveSet index n... */
      for( sorted_freq_idx = 0; 
           sorted_freq_idx < hdrfetap.hdrfetap.num_sub_asets; ++sorted_freq_idx )
      {

        /* Initialize Candidates for next channel and carrier index */
        next_freq_khz = HDRIS890_INVALID_CHAN_FREQUENCY;
        
        /* Loop through the SubActiveSets and find next (nth) SAS in ascending
           order of frequency */
        for( freq_idx = 0; freq_idx < hdrfetap.hdrfetap.num_sub_asets; ++freq_idx )
        {

          HDR_MSG_IS890_6(
            MSG_LEGACY_HIGH, 
            "SIdx: %d CIdx: %d Chan: %d Freq: %dKHz Prev: %dKHz Next: %dKHz",
            sorted_freq_idx,
            freq_idx,
            carrier_map[freq_idx].fwd_channel.chan_num,
            freq_khz[freq_idx],
            prev_freq_khz,
            next_freq_khz );

          /* If the current frequency is betweeen prev_channel_khz and 
             next_channel_khz */
          if( ( freq_khz[freq_idx] > prev_freq_khz ) && 
              ( freq_khz[freq_idx] < next_freq_khz ) ) 
          {
            /* Mark this SAS as candidate for next in order */
            next_carrier = carrier_map[freq_idx].carrier_index;
            next_freq_khz = freq_khz[freq_idx];
          }

        } /* For each SAS */

        /* If we found a candidate for next channel */
        if( next_freq_khz != HDRIS890_INVALID_CHAN_FREQUENCY ) 
        {
          prev_freq_khz = next_freq_khz;
            /* The next highest channel has to be greater than cur low chan */
            
          hdrfetap.hdrfetap.carrier_to_freq_idx[next_carrier] = sorted_freq_idx;
            /* Store the SubactiveSet index for this next lowest carrier */
        }

        HDR_MSG_IS890_2( MSG_LEGACY_HIGH, "SAS: %d C: %d",
                         sorted_freq_idx,
                         next_carrier );

      } /* For each SAS index in sorted order */

    } /* If TCA Sequence number mis-match */

    freq_idx = ( (carrier_id < HDRIS890B_NUM_CARRIERS)?
                hdrfetap.hdrfetap.carrier_to_freq_idx[carrier_id]:
                HDRFETAP_INVALID_SUBACTIVE_SET);

  }
  else
  {
    /* Invalid subtype */
    freq_idx = HDRFETAP_INVALID_SUBACTIVE_SET;
  }

  if( freq_idx == HDRFETAP_INVALID_SUBACTIVE_SET )
  {
    HDR_MSG_IS890_2( MSG_LEGACY_ERROR, "Invalid carrier %d, phy %d", 
                     carrier_id,
                     hdrfetap.hdrfetap.configuration.phy_subtype );
      /* Boundary checking */
  }

  return freq_idx;

} /* hdrfetap_get_freq_idx_from_carrier_id */


/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_PROCESS_STATS_CLEAR_REQUEST()

DESCRIPTION
  This function handles the FETAP clear statistics request.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr   - Pointer to DSM item containing request.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Specified statistics values will be set to 0.
===========================================================================*/
static void hdrfetap_process_stats_clear_request
(
  dsm_item_type**  msg_ptr 
    /* the ptr to the DSM item containing the StatsClearReq message */
)
{

  dsm_item_type*  reply_ptr;
  uint8           seqno;
  uint8           attribute_id;
  uint8           i;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get and check the validity of this message */

  if ( dsm_pullup( msg_ptr, &seqno, 1 ) != 1 )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: Cannot read seqno" );
    return;
  }
  if ( (! hdrfetap.hdrfetap.configuration.stats_clear_seqno_is_valid) ||
       (HDRIS890A_SEQNO_IS_VALID( seqno,
                                 hdrfetap.hdrfetap.configuration.stats_clear_seqno )) )
  {
    hdrfetap.hdrfetap.configuration.stats_clear_seqno_is_valid = TRUE;
    hdrfetap.hdrfetap.configuration.stats_clear_seqno = seqno;
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: bogus ClearStats. (%d)", seqno );
    return;
  }

  HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "FETAP: ClearStats (%d)", seqno);

  /* Loop through the attributes present in the message */

  while( dsm_pullup( msg_ptr, &attribute_id, 1 ) == 1 )
  {
    switch( attribute_id )
    {
      case HDRFETAP_ATTR_IDLE_ASP_STATS:

        hdrfetap.hdrfetap.statistics.idle_asp_change = 0;
        hdrfetap.hdrfetap.statistics.idle_time       = 0;
        break;
      
      case HDRFETAP_ATTR_CONNECTED_SS_STATS:

        hdrlmac_clear_handoff_count( HDRFETAP_DEFAULT_SUBACTIVE_SET );
        
        hdrfetap.hdrfetap.statistics.connected_ss_change = 0;
        hdrfetap.hdrfetap.statistics.connected_time      = 0;
        break;
      
      case HDRFETAP_ATTR_FIRST_SYNC_CC_PKT_STATS:

        hdrfetap.hdrfetap.statistics.first_sync_cc_pkt = 0;
        hdrfetap.hdrfetap.statistics.cc_time           = 0;
        break;

      case HDRFETAP_ATTR_CONNECTED_SS_STATS_MC:

        for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ )
        {
          hdrlmac_clear_handoff_count( i );
          hdrfetap.hdrfetap.statistics.connected_ss_change_mc[i] = 0;
        }
        
        hdrfetap.hdrfetap.statistics.connected_time              = 0;
        break;
      
      default:

        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: unkwn attr. ID %d", attribute_id );
        return;
    }
  }

  /* send the ConfirmationMsg on TC only reliably. */

  reply_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
  if ( reply_ptr == NULL )
  {
    ERR_FATAL( "FETAP: Out of DSM items", 0, 0, 0 );
  }

  ( void )dsmbit_pack8_tail( reply_ptr,
                     HDRFETAP_MSG_STATS_CLEAR_RESPONSE,
                     0,
                     8 );

  ( void )dsmbit_pack8_tail( reply_ptr,
                     seqno,
                     8,
                     8 );

  hdrhmp_app_send_msg_on_existing_rtc( 
                     hdrfetap.hdrfetap.configuration.protocol_id,
                     hdrfetap.hdrfetap.configuration.stream_id,
                     FALSE,
                     40,
                     HDRHAI_IN_USE,
                     reply_ptr,
                     NULL,
                     NULL );

} /* hdrfetap_process_stats_clear_request */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_LO_BUFF_GET_FROM_CARRIER_ID()

DESCRIPTION
  Returns pointer to hdrfetap_lo_buff_type that corresponds to PHY subtype 
  and carrier ID. Should be used as the only means to access hdrfetap.hdrfetap.lo 
  or hdrfetap.hdrfetap.lo_mc.
  
DEPENDENCIES
  None.

PARAMETERS
  carrier_id    - Index representing physical carrier for Rx or Tx packet
  
RETURN VALUE
  Pointer to loopback buffer for the specified carrier ID

SIDE EFFECTS
  Updates the enabled flag in the hdrfetap_lo_buff_type returned.

===========================================================================*/
static hdrfetap_lo_buff_type* hdrfetap_lo_buff_get_from_carrier_id(
  
  uint8 carrier_id
    /* Index representing physical carrier for Rx or Tx packet */
)
{
  hdrfetap_lo_buff_type *lo = NULL;
  uint8                 sas_idx;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_0 ||
       hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_1 || 
       hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 )
  {
    lo = &hdrfetap.hdrfetap.lo;
    lo->enabled = hdrfetap.hdrfetap.configuration.loopback_mode;
  }
  else if( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {
    sas_idx = hdrfetap_get_freq_idx_from_carrier_id(carrier_id);

    if( sas_idx == HDRFETAP_INVALID_SUBACTIVE_SET ) 
    {
      HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: invalid carrier index %d",
                     carrier_id );
    }
    else
    {
      ASSERT (sas_idx < HDRIS890B_NUM_CARRIERS);

      lo = &hdrfetap.hdrfetap.lo_mc[sas_idx];
      lo->enabled = hdrfetap.hdrfetap.configuration.loopback_mode_mc[sas_idx];
    }
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: invalid subtype %d",
                     hdrfetap.hdrfetap.configuration.phy_subtype );
  }

  return lo;
} /* hdrfetap_lo_buff_get_from_carrier_id */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_LO_BUFF_REFRESH()

DESCRIPTION
  The purpose of this function is to fill the zero-record till the current 
  frame time; inside the function, the write_index of the record buffer is 
  updated and zero record was filled.
  
DEPENDENCIES
  None.

PARAMETERS
  lo               - Pointer to loopback buffer
  sys_time         - Current system time in frames

RETURN VALUE
  None.

SIDE EFFECTS
  Fills out zero looback packet records if no test packets are received for
  an observation period. 

===========================================================================*/

static void hdrfetap_lo_buff_refresh
( 
  hdrfetap_lo_buff_type *lo,
    /* Pointer to loopback buffer to initialize */

  uint16                sys_time 
    /* Current system time in frames */
)
{
  uint8  previous_write_index, i;
  uint16 expected_pkt_time;
  uint16 gen_fwd_sys_time;  /* Generated forward system time if generating of 
                               idle packets is skipped. */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( lo == NULL )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "NULL LO Pointer" );
    return;
  }

  /*-------------------------------------------------------------
   * hdrfetap_put_pkt() might not get called at least once 
   * per frame, this might cause by the decOB setup or very low
   * forward link rate 
   * Find the previously used index, write index points
   * to the next record to be filled in.              
  -------------------------------------------------------------*/

  if ( lo->write_index == 0 )
  {
    previous_write_index = HDRFETAP_LO_RECORD_BUF_SIZE - 1;
  }
  else
  {
    previous_write_index = lo->write_index - 1;
  }
  
  /* Get the next expected timestamp (previous packet's
   * timestamp + 1) and fill any idle holes til now.     */

  expected_pkt_time = 
    lo->record[previous_write_index].fwd_sys_time + 1;
  expected_pkt_time %= 0x8000;
  
  /* Let the 0-record frames lag by 6 frames to ensure 
   * we don't generate them before the DSP cancelled a 
   * sequence number. This is up to 4 frames, so we pick
   * 6 to be on the safe side but still have at least 2
   * triggers per 16-frame HSTR rollover period.     */

  if ( expected_pkt_time > 0x7ff0 && sys_time < 0x000f )
  {
    /* this is at the wrap around position of 15 bit systime;
     * is this i the time distance b/w the prev. pkt and current frame */

    i = 0x8000 - expected_pkt_time + sys_time; /*lint !e734*/
    i = ((i < 6) ? 0 : i - 6);
  }
  else if ( sys_time > 0x7ff0 && expected_pkt_time < 0x000f )
  {
    /* If we hit the seq # rollover condition, don't flag  it falsely as 
     * a huge gap */

    i = 0;
  }
  else
  {
    /* Not wrapping around */

    if ( expected_pkt_time < sys_time )
    {
      i = sys_time - expected_pkt_time; /*lint !e734*/
      i = ((i < 6) ? 0 : i - 6);
    }
    else
    {
      i= 0;
    }
  }


  /* Check if there is a large time gap between expected and system times
   * and if so, skip making a lot of idle packets.
   */
  if ( i > HDRFETAP_L0_BUF_REFRESH_OVERFLOW_BIT_SET_THRESHOLD )
  {

    /* Let the 0-record frames lag by 6 frames to ensure 
     * we don't generate them before the DSP cancelled a 
     * sequence number. This is up to 4 frames, so we pick
     * 6 to be on the safe side but still have at least 2
     * triggers per 16-frame HSTR rollover period.     */
    if ( sys_time < 6 )
    {
      gen_fwd_sys_time = 0x8000 - 6 + sys_time;
    }
    else
    {
      gen_fwd_sys_time = sys_time - 6;
    }

    HDR_MSG_IS890_4( MSG_LEGACY_HIGH, 
      "FETAP:Large idle time detected[0x%x-0x%x]. Overwriting lo[%d].FwdSysTime to 0x%x",
      sys_time,
      expected_pkt_time,
      lo->write_index,
      gen_fwd_sys_time );

    lo->record[lo->write_index].fwd_sys_time =
      gen_fwd_sys_time | HDRFETAP_IDLE_SYS_TIME_FLAG;

    lo->write_index++;
    lo->write_index %= HDRFETAP_LO_RECORD_BUF_SIZE;

    memset( &(lo->record[lo->write_index]),
            0,
            sizeof( hdrfetap_lo_record_type ) );

    /* Set the overflow since we lost some packets */
    lo->overflow = TRUE;
    lo->overflow_status_sent = FALSE;

    /* Update last_fwd_sys_time to generated sys time */
    lo->last_fwd_sys_time = gen_fwd_sys_time;

    /* Check if we caught up with the read index */
    if ( lo->write_index == (lo->read_index -1) )
    {
      /* This may happen when the reverse bandwith is not big enough to drain
       * the pending loopback packets in the loopback buffer */

      HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: Lo rec buf overflow !" );

      /* Move the read index as well, this make sure the sequentail transmission
       * of the loopback packets */
      lo->read_index ++ ;
    }

  }
  else
  {

    /* Find how many frames are missing entries in the record buffer and fill 
     * the hole up. Only do up to a limited number of frames as we otherwise
     * loop too long and bad things happen - rather miss out on some lo 
     * pkts. 
     */
  
    if ( i > HDRFETAP_L0_BUF_REFRESH_MAX_CATCHUP_FRAMES )
    {
      HDR_MSG_IS890_3 (MSG_LEGACY_HIGH,  "FETAP: Rx fill GAP (%d,0x%x,0x%x)", 
                i, sys_time, expected_pkt_time );
  
      i = HDRFETAP_L0_BUF_REFRESH_MAX_CATCHUP_FRAMES;
    }
  
    /*------------------------------------------------------------------------
     * Filling the zeroRecord
    ------------------------------------------------------------------------*/
    
    for ( ; i>0; i-- )
    {
      lo->record[lo->write_index].fwd_sys_time =
        expected_pkt_time | HDRFETAP_IDLE_SYS_TIME_FLAG;
  
      HDR_MSG_IS890_1 (MSG_LEGACY_LOW, "Adding 0-rec (0x%x)", expected_pkt_time);
               
      expected_pkt_time++;
      lo->write_index++;
      lo->write_index %= HDRFETAP_LO_RECORD_BUF_SIZE;
      memset( &(lo->record[lo->write_index]),
              0,
              sizeof( hdrfetap_lo_record_type ) );
      
      lo->last_fwd_sys_time++;
      lo->last_fwd_sys_time %= 0x8000;
      
      /* Check if we caught up with the read index */
  
      if ( lo->write_index == (lo->read_index -1) )
      {
  
        /* This may happen when the reverse bandwith is not big enough to drain
         * the pending loopback packets in the loopback buffer */
  
        HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: Lo rec buf overflow !" );
        lo->overflow = TRUE;
        lo->overflow_status_sent = FALSE;
  
        /* Move the read index as well, this make sure the sequentail transmission
         * of the loopback packets */
  
        lo->read_index ++ ;
      }
  
    }

  }

} /* hdrfetap_lo_buf_refresh */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_LO_BUFF_GET_SIZE()

DESCRIPTION
  Calculates the size of the current buffer to pass back to PCP.
  
DEPENDENCIES
  None.

PARAMETERS
  lo               - Pointer to loopback buffer
  num_pkts_pending - Number of pending loopback packets
  
RETURN VALUE
  uint32 the size of the buffer to advertise to PCP.

SIDE EFFECTS
  None.

===========================================================================*/
static uint32 hdrfetap_lo_buff_get_size
( 
  hdrfetap_lo_buff_type *lo,
    /* Pointer to loopback buffer to initialize */

  uint32                *num_pkts_pending
    /* Number of pending loopback packets */
)
{
  uint8   i;
  uint32  buf_size = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( lo == NULL )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "NULL LO Pointer" );
    return 0;
  }

  /* Check if the buffer is empty */

  if ( lo->read_index == lo->write_index ) 
  {
    buf_size = 0;
      /* The LO buffer is empty */
  }
  else if ( lo->record[lo->read_index].fwd_sys_time == 
            lo->last_fwd_sys_time) 
  {

    buf_size = 0;
      /* Do not loopback the records for the last observation period */
  }
  else 
  {

    /* Do not count the records of the last observation period */

    for( i = lo->read_index; 
         ( i != lo->write_index ) &&
         ( lo->record[i].fwd_sys_time != lo->last_fwd_sys_time);
         i++ ) 
    {
      buf_size += ( HDRFETAP_LO_FUL_RECORD_LEN_PL2/8 ) + 1;  
        /* Greedily using the max record size; can be more accurate */

      if ( i != lo->read_index )
      {
        if ( lo->record[i].fwd_sys_time != 
             lo->record[(HDRFETAP_LO_RECORD_BUF_SIZE - 1 + i)%
             HDRFETAP_LO_RECORD_BUF_SIZE].fwd_sys_time ) 
        {
          /* If the loopback packet header is needed */

          buf_size += ( HDRFETAP_LO_TEST_PKT_HEADER_LEN / 8 ) + 1;
          ( *num_pkts_pending ) ++;
        }
      }
    }
  }

  if ( buf_size > 0 ) 
  {
    *num_pkts_pending += 1;
      /* Increment by 1 to correct the number */
  }

  return buf_size;
} /* hdrfetap_lo_buff_get_size */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_PCP_GET_LINKFLOW_BUF_SIZE()

DESCRIPTION
  Calculate and report to PCP how many bytes and the number of packets 
  available for the next transmission. For PL3 a value of infinite is returned
  so that get_pkt can choose which carrier to send the loopback packet.

DEPENDENCIES
  None
 
PARAMETERS
  user_data_ptr    - Not used
  num_pkts_pending - pending data size (in bytes )
 
RETURN VALUE
  Size of the transmit data.
  
SIDE EFFECTS
  None
===========================================================================*/
static uint32 hdrfetap_pcp_get_linkflow_buf_size
(
  uint32 *num_pkts_pending,
    /* Pending data size */

  void  *user_data_ptr
    /* Generic PCP interface. NOT used in the FETAP */
)
{
  uint32                buf_size = 0;
  hdrfetap_lo_buff_type *lo;
  uint8                 carrier_id;
  boolean               lo_enabled;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SYS_ARG_NOT_USED( user_data_ptr );
    /* mark the unused symbols to avoid a lint error */

  /* Sanity check the num_pkts_pending */

  ASSERT( num_pkts_pending != NULL );

  *num_pkts_pending = 0;
    /* Initilize the number of pending packets */

  if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_0 ||
       hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_1 || 
       hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 )
  {
    lo = hdrfetap_lo_buff_get_from_carrier_id( HDRFETAP_DEFAULT_CARRIER );

    /* Sanity check */
    ASSERT (lo != NULL);

    hdrfetap_lo_buff_refresh( lo, hdrfetap_get_sys_time() );
      /* Update the lo record buffer; inside the last_fwd_sys_time is updated
       * to the current system time; this may got called 2 times for ftype 2 */

    buf_size = hdrfetap_lo_buff_get_size( lo, num_pkts_pending );
      /* Compute the current buffer size to advertise to PCP */

#ifdef HDRFETAP_LO_PKT_DBG
      HDR_MSG_IS890_6 ( 
        MSG_LEGACY_HIGH, 
        " FETAP: GetBufSz ritm=0x%x ltm=0x%x ri=%d wi=%d bufsz=%d npp=%d",
        lo->record[lo->read_index].fwd_sys_time,
        lo->last_fwd_sys_time,
        lo->read_index, 
        lo->write_index,
        buf_size,
        *num_pkts_pending );
#endif

  }
  else if( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {
    lo_enabled = FALSE;
      /* Initialize marker */

    buf_size = 0;
      /* Initialize buf_size to 0 */

    for( carrier_id=0; carrier_id<HDRIS890B_NUM_CARRIERS; carrier_id++ ) 
    {
      lo = hdrfetap_lo_buff_get_from_carrier_id( carrier_id );

      /* Check for NULL. If NULL, this carrier_id is not assigned, so
         just skip it. */
      if (lo != NULL)
      {
        hdrfetap_lo_buff_refresh( lo, hdrfetap_get_sys_time() );
          /* Update the lo record buffer; inside the last_fwd_sys_time is updated
           * to the current system time; this may got called 2 times for ftype 2 */
  
        buf_size += hdrfetap_lo_buff_get_size( lo, num_pkts_pending );
          /* Increment buf_size with pending data for each carrier */

#ifdef HDRFETAP_LO_PKT_DBG
        HDR_MSG_IS890_8 ( 
          MSG_LEGACY_HIGH, 
          " FETAP: GetBufSz c=%d sas=%d ritm=0x%x ltm=0x%x ri=%d wi=%d bsz=%d npp=%d",
          carrier_id,
          hdrfetap_get_freq_idx_from_carrier_id(carrier_id),
          lo->record[lo->read_index].fwd_sys_time,
          lo->last_fwd_sys_time,
          lo->read_index, 
          lo->write_index,
          buf_size,
          *num_pkts_pending );
#endif

        if( lo->enabled )
        {
          lo_enabled = TRUE;
            /* Mark if one loopback is enabled */
        }
      }

    }

    if( lo_enabled )
    {
      buf_size = HDRFETAP_LO_MODE_BUF_SIZE_PL3;
        /* Return large number so that FMCTAP can control which carrier to tx 
        the loopback packets on. If there is not enough data to fill all the
        carriers, the correct carriers may not get selected. The get_bug_size 
        code is essentially dead code. It might be useful though for 
        debugging */
    }
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: invalid subtype %d",
                     hdrfetap.hdrfetap.configuration.phy_subtype );
  }  

  return buf_size;

} /* hdrfetap_pcp_get_linkflow_buf_size */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_LO_BUFF_UPDATE_READ_INDEX()

DESCRIPTION
  Updates the loopback buffer's read index based on previous packet status.
  
DEPENDENCIES
  None.

PARAMETERS
  lo               - Pointer to loopback buffer
  pkt_accepted     - Previous packet status for this lo
  
RETURN VALUE
  uint32 the size of the buffer to advertise to PCP.

SIDE EFFECTS
  Read index is updated.

===========================================================================*/
static void hdrfetap_lo_buff_update_read_index
( 
  hdrfetap_lo_buff_type *lo,
    /* Pointer to loopback buffer to initialize */

  errno_enum_type       pkt_accepted
    /* Previous packet status for ths lo */
)
{
  if( lo != NULL && lo->enabled )
  {
 
#ifdef HDRFETAP_LO_PKT_DBG
    HDR_MSG_IS890_3 ( MSG_LEGACY_MED, " FETAP: LO stats ri=%d, rti=%d, wi=%d ", 
                      lo->read_index, lo->tmp_read_index,
                      lo->write_index );
#endif
    
    if ( pkt_accepted == E_SUCCESS) 
    {
      /* If pkt is accepted, update the read_index with temperory read index.
       * for the physubtype2, this is the only place where the read_index is
       * updated for the regular operations. */
  
      lo->read_index =  lo->tmp_read_index ;

      if (lo->overflow_status_sent)
      {
        /* The packet accepted had overflow bit set.  Reset the overflow
         * flag to prevent overflow bit from being set in future packets. 
         */
        lo->overflow = FALSE;
      }
    }
    else if ( pkt_accepted == E_CANCELED )
    {
      /* If cancelled, reset the temperory read index */
  
      lo->tmp_read_index =  lo->read_index ;
    }
    else
    {
      HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR, " FETAP: Invalid errno = %d",
                      pkt_accepted );
    }
  }
} /* hdrfetap_lo_buff_update_read_index */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_PCP_PREV_PKT_STATUS()

DESCRIPTION
  PCP calls this function with the status of the previous transmission.

DEPENDENCIES
  None
 
PARAMETERS
  combo_id            - Which combo was accepted.
  carrier_tx_outcome  - Whether the transmission was successful or not.
  num_outcomes        - Number of txoutcomes.
  user_data_ptr       - Pointer to user data passed back through callback.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrfetap_pcp_prev_pkt_status
(
  uint8                           combo_id,
    /* ID for previous transmission */

  hdrpcp_carrier_tx_outcome_type  carrier_tx_outcome[],
    /* Multi-carrier previous transmission status */

  uint8                           num_outcomes,
    /* Number of previous transmission statuses */
    
  void                            *user_data_ptr
    /* Generic PCP interface. NOT used in the FETAP */
)
{
  hdrfetap_lo_buff_type   *lo;
    /* Pointer to loopback buffer structure for associated carrier */

  uint8                   carrier_idx;
    /* Carrier index for looping */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Mark unused symbols to avoid lint errors */

  SYS_ARG_NOT_USED( user_data_ptr );

    /* Update the sync flag */
  for ( carrier_idx=0; carrier_idx<HDRIS890B_NUM_CARRIERS; carrier_idx++ ) 
  {
    hdrfetap.hdrfetap.configuration.sync_flag[carrier_idx] = PCP_CALLED_PRE_PKT_STATUS;
  }

  /* If no combos were accepted, set each lo buff to "not accepted" */
  if( combo_id == HDRPCP_NO_COMBO_ID_ACCEPTED )
  {
    HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: PCP pkt status, none accepted" );

    for( carrier_idx=0; carrier_idx<HDRIS890B_NUM_CARRIERS; carrier_idx++ )
    {
      lo = hdrfetap_lo_buff_get_from_carrier_id( carrier_idx );
  
      hdrfetap_lo_buff_update_read_index( lo, E_FAILURE );
    }
  }
  /* If combo_id is valid, something was accepted. Update across carriers */
  else
  {
    for( carrier_idx=0; carrier_idx<num_outcomes; carrier_idx++ ) 
    {
      HDR_MSG_IS890_2( MSG_LEGACY_HIGH, "FETAP: PCP pkt status, c=%d acc=%d",
                     carrier_idx,
                     carrier_tx_outcome[ carrier_idx ].tx_set_accepted );

      lo = hdrfetap_lo_buff_get_from_carrier_id( 
        carrier_tx_outcome[ carrier_idx ].carrier_id );
  
      hdrfetap_lo_buff_update_read_index( 
        lo, 
        carrier_tx_outcome[ carrier_idx ].tx_set_accepted );
    }
  }
} /* hdrfetap_pcp_prev_pkt_status */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRFETAP_UPDATE_SLOT_TIME()

DESCRIPTION
  This function updates the passed-in slot time by the number
  of slots passed since the last call to this function.

DEPENDENCIES
  None
 
PARAMETERS
  update_slot_time  - Pointer to slot time to be uptated
 
RETURN VALUE
  None
  
SIDE EFFECTS
  Sets hdrfetap.hdrfetap.statistics.last_slot_time_stamp to current time.
===========================================================================*/
static void hdrfetap_update_slot_time
(
  uint32  *update_slot_time
    /*  return value to accumulate the passing time in slot unit */
)
{
  qword   current_slot_time;
  uint32  current_lap;
  uint32  current_fetap_slot_time;
  uint32  overflow;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( update_slot_time != NULL );

  /* Make sure the HDR timestamp is valid and we are active */

  if ( (! hdralmp_system_is_acquired( HDRHAI_IS890_FTAP )) ||
       ( hdrfetap.hdrfetap.state == HDRFETAP_INACTIVE ) || 
       hdridle_is_sleeping() )
  {

    return;

  }

  /* Get the current time in cycles and slots into the last    
   * cycle. Set cycle length to 1 slot, so the result is already
   * in slots.                      */ 

  hdrts_get_cycle_and_slot( current_slot_time,
                            &current_lap, /* not used */
                            1 );
  
  /* Take the least significant 23 bits ... */

  current_fetap_slot_time = (qw_lo( current_slot_time ) & 0x007fffff);
  
  /* ... and calculate the time since last function call */

  if ( current_fetap_slot_time >= hdrfetap.hdrfetap.statistics.last_slot_time_stamp )
  {
    current_lap = current_fetap_slot_time - 
                    hdrfetap.hdrfetap.statistics.last_slot_time_stamp;
  }
  else
  {

    /* Account for roll-over */

    current_lap = (current_fetap_slot_time + 
                   (0x007fffff - hdrfetap.hdrfetap.statistics.last_slot_time_stamp)) 
                  & 0x007fffff;

  }

  hdrfetap.hdrfetap.statistics.last_slot_time_stamp = current_fetap_slot_time;
  
  /* Apply the time lap in slots to the statistics time passed in,
   * preserve overflow bit from last update.                */

  overflow = 0x00800000 & *update_slot_time;

  if ( (*update_slot_time + current_lap) > 0x007fffff )
  {
    overflow = 0x00800000;
  }

  *update_slot_time = (*update_slot_time + current_lap) % 0x00800000;
  *update_slot_time |= overflow;

} /* hdrfetap_update_slot_time */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRfetap_PROCESS_PN_ROLL_INT()

DESCRIPTION
  This function is registered with the HDR Serach PN Roll ISR
  to update statistics.

  NOTE: The HDRfetap_SYNC_TIME_WAIT state is intended to allow search
        to generate a valid system time after receiving the IS-856
        sync message. There is a period of up to 12 frames during which
        search has processed the sync msg, but system time is not yet 
        valid.

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrfetap_process_pn_roll_int( void )
{

  uint8 i;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Wait the correct # of PN rolls for system time to become valid */

  if ( hdrfetap.hdrfetap.state == HDRFETAP_SYNC_TIME_WAIT )
  {

    if ( hdrfetap_sync_time_wait_duration < HDRFETAP_SYNC_TIME_26MS_COUNT )
    {

      hdrfetap_update_slot_time( &( hdrfetap.hdrfetap.statistics.idle_time ));
      hdrfetap.hdrfetap.statistics.idle_time = 0;
      hdrfetap_sync_time_wait_duration++;

      return;
    }
    else
    {

      hdrfetap_update_slot_time( &( hdrfetap.hdrfetap.statistics.idle_time ) );
      hdrfetap_sync_time_wait_duration = 0;

      /* At this point we can safely intialize the DSP's ASP count */

      for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ ) 
      {
        hdrlmac_clear_handoff_count( i );
      }

      /* Change state to idle */
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FETAP: State set to IDLE");
      hdrfetap.hdrfetap.state = HDRFETAP_IDLE;

    }
  }

  /* Update our statistics timers */

  if ( hdrfetap.hdrfetap.state == HDRFETAP_TRAFFIC )
  {
    hdrfetap_update_slot_time( &( hdrfetap.hdrfetap.statistics.connected_time ));
  }
  else if ( hdrfetap.hdrfetap.state == HDRFETAP_IDLE )
  {
    hdrfetap_update_slot_time( &( hdrfetap.hdrfetap.statistics.idle_time ));
  }

  /* Decrement the inactivity frame counter to zero */

  if ( hdris890a_inactivity_frame_cnt ) hdris890a_inactivity_frame_cnt--;

} /* hdrfetap_process_pn_roll_int */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_GEN_TEST_PKT()

DESCRIPTION
  The function genterate 1 FETAP test back for the next observation interval;
  
PARAMETERS
  lo               - Pointer to loopback buffer
  data_pkt_ptr     - The returned dsm item containing the FETAP test packet
  bw_alloc         - Current bandwidth allocation

DEPENDENCIES
  None.

RETURN VALUE
  Bandwidth in bytes that was used.

SIDE EFFECTS
  None.

===========================================================================*/
static uint16 hdrfetap_lo_buff_gen_pkt
( 
  hdrfetap_lo_buff_type *lo,
    /* Pointer to loopback buffer to initialize */

  dsm_item_type         **data_pkt_ptr,
    /* the return DSM item ptr containing the generated loop back pkt */

  uint16                bw_alloc
    /* the bandwith allocated by PCP, decide the read_index mode */
)
{

  uint16          pkt_size;
  uint16          fwd_sys_time_slot0;
  uint16          sys_time_slot0_net_order;
  uint16          current_dsm_wr_offset;
  uint8           record_len, rec_cnt = 0, i, tmp_rec_cnt;
  uint8           *src_ptr;
  dsm_item_type   *dsm_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT( data_pkt_ptr != NULL );

  if( lo == NULL )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "NULL LO Pointer" );
    return 0;
  }

  /* Sanity check on the max rmac packet size and the lo record buffer */
  
  if ( ( bw_alloc < HDRFETAP_LO_MIN_PKT_BYTE_SIZE ) ||
       ( lo->tmp_read_index == lo->write_index ) )
  {
    /* If bandwith is too small OR the REC buffer is empty       */

    return 0;
  }

  /*------------------------------------------------------------------------
   * the fwd_sys_tiem_slot0 represnets the frame time of the current observation
   * period    
  ------------------------------------------------------------------------*/

  fwd_sys_time_slot0 = 
    lo->record[lo->tmp_read_index].fwd_sys_time & 0x7fff;

  /*------------------------------------------------------------------------
   * Check if the current packet is a split packet 
   * hrefetap_lo.split_pkt_fwd_sys_time == fwd_sys_time_slot0
   * if it is, then split_pkt_rec_cnt gives the total number of records
  ------------------------------------------------------------------------*/

  if ( lo->split_pkt_fwd_sys_time != fwd_sys_time_slot0 )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: DEBUG -- Split Lag, Reset rec_cnt" );
      /* For debug, seems to happen when not able to transmit for
      a long time in the middle of a split packet. */
  
    lo->split_pkt_rec_cnt = 0;
      /* reset the spliting counter */
  }

  /* ------------------------------------------------------------ 
   * if the last receive time equals to the read index time, which
   * means the lo buffer only contains 1 obsrevation period; avoid
   * processing if this is ture;
  -------------------------------------------------------------- */

  if ( fwd_sys_time_slot0 != lo->last_fwd_sys_time )
  {
    /* Get a new dsm item from the pool */

    dsm_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL ) ;

    if ( dsm_ptr == NULL ) 
    {
      HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                    "FETAP: fail to request new dsm item; return w/o pkt gen" );
      *data_pkt_ptr = dsm_ptr ;
      return 0;
    }

    /* ------------------------------------------------------------ 
     * Packing the 1st byte
     * Stream ID     2 bits    0bxx 
     * Protocol ID   2 bits    0b00
     * Packet Type   4 bits    0b0001(PL_01) : 0b0010(PL_2)
     * 1st byte(PL_01): 0b xx(s-id) 00(p-id) 0001(p-type) 
     * 1st byte(PL_2) : 0b xx(s-id) 00(p-id) 0010(p-type) 
     * 1st byte(PL_3) : 0b xx(s-id) 00(p-id) 0011(p-type) 
    ------------------------------------------------------------ */

    if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
    {
      /* Physical layer subtype 2 */
      *( dsm_ptr->data_ptr ) = 0x02;
    }
    else if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
    {
      /* Physical layer subtype 3 */
      *( dsm_ptr->data_ptr ) = 0x03;
    }
    else
    {
      /* Physical layer subtype 0 or 1 */
      *( dsm_ptr->data_ptr ) = 0x01;
    }
    dsm_ptr->used ++;

    /* ------------------------------------------------------------ 
     * Packing the 2nd and 3rd byte
     * fwd_sys_time    15 bits
     * OF-indicator     1  bits
    ------------------------------------------------------------ */

    /* Convert the slot 0 timestamp to network order for packing */

    sys_time_slot0_net_order =
      lo->record[lo->tmp_read_index].fwd_sys_time & 0x7fff;
   
    /* The 8 MSBs */

    *( dsm_ptr->data_ptr + 1 ) = 
        ( uint8 )(( sys_time_slot0_net_order & 0x7f80 ) >> 7 );

    /* The 7 LSBs plus the overflow indicator */

    if ( lo->overflow )
    {

      HDR_MSG_IS890_1 (MSG_LEGACY_MED,  "fetap Lo ovfl bit set (0x%x)", 
                       fwd_sys_time_slot0);
      *( dsm_ptr->data_ptr + 2) =
        ( uint8 )((( sys_time_slot0_net_order & 0x007f ) << 1 ) | 0x01 );

      /* Set overflow_status_sent flag so that hdrfetap_lo_buff_update_
      read_index() can clear overflow flag if this packet is accepted. */
      lo->overflow_status_sent = TRUE;

    }
    else
    {

      *( dsm_ptr->data_ptr + 2) =
        ( uint8 )((( sys_time_slot0_net_order & 0x007f ) << 1 ) & 0xfe );

    }  
    
    dsm_ptr->used += 2;
    
    /* ------------------------------------------------------------ 
     * RecordCount   5  bits; set to zero for here
    -------------------------------------------------------------- */

    memset( dsm_ptr->data_ptr + 3, 0, dsm_ptr->size - 3 );
      /* Initialize the remainder to 0 for any reserved fields */
    
    dsm_ptr->used ++;
       /* the byte contain the #ofRecords */

    /* ------------------------------------------------------------ 
     * Packing the dynamic part and then fill the rec#
    -------------------------------------------------------------- */

    current_dsm_wr_offset = HDRFETAP_LO_TEST_PKT_HEADER_LEN ; 
      /* offset is 29 in bit */

    if ( ! (lo->record[lo->tmp_read_index].fwd_sys_time &
            HDRFETAP_IDLE_SYS_TIME_FLAG) )
    {
      /* ------------------------------------------------------------ 
       * loop to Pack the records as long as they fit into 
       * max_rmac_pkt_size  
      ------------------------------------------------------------ */

      while (((lo->record[lo->tmp_read_index].fwd_sys_time&0x7fff)
               == fwd_sys_time_slot0) &&
              (rec_cnt <= 16) ) 
      {
        /* The while-loop loop thorugh all the records for the current 
         * oberservation interval AND the record number is less then
         * or equal to 16 (this is the maximum) */

        /* ------------------------------------------------------------ 
         * handle the packet split
        -------------------------------------------------------------- */
        
        if (( bw_alloc - dsm_ptr->used ) < 
            (( HDRFETAP_LO_FUL_RECORD_LEN_PL01 / 8 ) + 1 ))
          /* FULL record length is different b/w the PL_2 and PL_01 tst pkts
           * Same as the minimum records; but in byte they all are the same
           * if the bandwidth passed still can NOT hold a full length record */ 
        {

          if ( (rec_cnt > 0)
               &&
               (lo->split_pkt_rec_cnt == 0) )
          {

            /* We still have more records to send for this   
             * observation inteval, so get the total record. */

            i = lo->tmp_read_index;
            lo->split_pkt_rec_cnt = rec_cnt;
            lo->split_pkt_fwd_sys_time = fwd_sys_time_slot0;
            while ( lo->record[ i++ ].fwd_sys_time ==
                    fwd_sys_time_slot0 ) 
            {

              /* the split_pkt_rec_cnt is the total number of the records
               * in this splitted observation period */

              lo->split_pkt_rec_cnt++;
            }
          }
          break; /* while */
        }

        if ( (rec_cnt == 0) ||
             (lo->record[lo->tmp_read_index].min_rec[0] & 0x80) )
        {

          /* need a full record either when
           * the 1st record or 
           * indicator in the min record is set  */
          src_ptr = lo->record[lo->tmp_read_index].full_rec;
          switch( hdrfetap.hdrfetap.configuration.phy_subtype )
          {
          case PHY_SUBTYPE_0:
          case PHY_SUBTYPE_1:
            record_len = HDRFETAP_LO_FUL_RECORD_LEN_PL01;
            break;
          case PHY_SUBTYPE_2:
            record_len = HDRFETAP_LO_FUL_RECORD_LEN_PL2;
            break;
          case PHY_SUBTYPE_3:
            record_len = HDRFETAP_LO_FUL_RECORD_LEN_PL3;
            break;
          default:
            record_len = 0;
            HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR, " FETAP: Invalid phy subtype = %d",
                      hdrfetap.hdrfetap.configuration.phy_subtype );
            break;
          }
        }
        else
        {
          /* need a minimum record */

          src_ptr = lo->record[lo->tmp_read_index].min_rec;

          switch( hdrfetap.hdrfetap.configuration.phy_subtype )
          {
          case PHY_SUBTYPE_0:
          case PHY_SUBTYPE_1:
            record_len = HDRFETAP_LO_MIN_RECORD_LEN_PL01;
            break;
          case PHY_SUBTYPE_2:
            record_len = HDRFETAP_LO_MIN_RECORD_LEN_PL2;
            break;
          case PHY_SUBTYPE_3:
            record_len = HDRFETAP_LO_MIN_RECORD_LEN_PL3;
            break;
          default:
            record_len = 0;
            HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR, " FETAP: Invalid phy subtype = %d",
                      hdrfetap.hdrfetap.configuration.phy_subtype );
            break;
          }
        }

        /* copy the record into the dsm item based on the decision above based
         * on the PHY subtype and record type */

        b_copy( src_ptr,
                0,
                dsm_ptr->data_ptr,
                current_dsm_wr_offset,
                record_len );

        current_dsm_wr_offset += record_len ;

        /* Update the record count for this packet */

        rec_cnt++;
        
        /* Update the dsm item's used field as it is compared 
         * against the max. rmac pkt size before each record. */

        dsm_ptr->used = ((current_dsm_wr_offset / 8) + 1);

        /* Advance the read index and check that we don't 
         * overtake the write index.    */

        lo->tmp_read_index++;
        lo->tmp_read_index %= HDRFETAP_LO_RECORD_BUF_SIZE;
        if ( lo->tmp_read_index == lo->write_index )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Caught up w/ wr_i; rec buf is empty %d",
                           lo->write_index );
          break;
        }

      } /* while */

    }   /* if not an IDLE packet */
    else
    {
      /* for IDLE pkt : record section is empty
       * Just filled in an idle loopback packet, advance the read index */

      lo->tmp_read_index++;
      lo->tmp_read_index %= HDRFETAP_LO_RECORD_BUF_SIZE;

    } /* IDLE packet */

    /* ------------------------------------------------------------ 
     * Pack the static part of the loopback packet :       
     * RecordCount   5  bits
    -------------------------------------------------------------- */

    if ( lo->split_pkt_rec_cnt > 0 )
    {

      HDRFETAP_SET_LO_RECORD_COUNT( 
        dsm_ptr,
        lo->split_pkt_rec_cnt ); /*lint !e734*/

      tmp_rec_cnt = lo->split_pkt_rec_cnt & 0xFF;

    }
    else
    {

      HDRFETAP_SET_LO_RECORD_COUNT( 
        dsm_ptr,
        rec_cnt );/*lint !e734*/

      tmp_rec_cnt = rec_cnt;

    }

    /* Hand in current tx pkt */

    pkt_size = dsm_ptr->used;
    *data_pkt_ptr = dsm_ptr;

    HDR_MSG_IS890_3 (MSG_LEGACY_MED, "FETAP: Tx lo pkt (t 0x%x, rc %2x, l %d)",
             fwd_sys_time_slot0, 
             tmp_rec_cnt,  
             pkt_size );

#ifdef HDRFETAP_LO_PKT_VERBOSE_DBG
    #error code not present
#endif /* HDRFETAP_LO_PKT_DBG */

  }
  else 
  {
    *data_pkt_ptr = NULL;
    pkt_size = 0;

  } 

#ifdef HDRFETAP_LO_PKT_DBG
  /* Debuging msg for each loopback packet */
  HDR_MSG_IS890_3 ( MSG_LEGACY_MED, " FETAP: LO stats ri=%d, rti=%d, wi=%d ", 
                    lo->read_index, lo->tmp_read_index,
                    lo->write_index );
#endif

#ifdef TIMETEST_DSRLP_STATS

  /* Update loopback packet stats for profiling */

  if ( pkt_size != 0  ) hdrfetap.hdrfetap_num_sent_lo_rcds += rec_cnt;

#endif

  return pkt_size;

} /* hdrfetap_gen_test_pkt */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_LO_BUFF_INIT()

DESCRIPTION
  The function initialize the loopback records buffer; the buffer is used
  to store the records of the incoming FETAP test packets.
  
DEPENDENCIES
  None.

PARAMETERS
  lo               - Pointer to loopback buffer
  sys_time         - Current system time in frames

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void hdrfetap_lo_buff_init
( 
  hdrfetap_lo_buff_type *lo,
    /* Pointer to loopback buffer to initialize */

  uint16                sys_time 
    /* Current system time in frames */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( lo == NULL )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "NULL LO Pointer" );
    return;
  }

  /* Initialize the read, write and temporary read index */

  lo->write_index    = 0;
  lo->read_index     = 0;
  lo->tmp_read_index = 0;

  /* Read the current TCA sequence number from RUP */

  lo->current_tca_seqno = 
    hdrrup_get_current_tca_seq_num();
    /* get the latest Traffic Channel Assignment Msg seq number */

  lo->current_tca_upper_byte =
    ( lo->current_tca_seqno >> 1 ) | 0x80;
  lo->current_tca_lower_byte = 
    ( lo->current_tca_seqno << 7 ) & 0x80;
    /* For the full record */
      
  /* Set the record[0] with the current frame time OR the idle FLAG */

  memset( &( lo->record[0] ),
          0,
          sizeof( hdrfetap_lo_record_type ) );

  lo->record[0].fwd_sys_time = 
    sys_time | HDRFETAP_IDLE_SYS_TIME_FLAG;
    /* set the record[0] with the current frame time OR the idle FLAG */

  memset( &( lo->record[HDRFETAP_LO_RECORD_BUF_SIZE-1] ),
          0,
          sizeof( hdrfetap_lo_record_type ) );
  
  /* Set the start time for this loop back record buffer */

  lo->record[HDRFETAP_LO_RECORD_BUF_SIZE-1].fwd_sys_time = 
    ( sys_time | HDRFETAP_IDLE_SYS_TIME_FLAG ) - 1;

  lo->last_fwd_sys_time = sys_time;
    /* Init the last system time to the current system time */

  lo->overflow = FALSE;
  lo->overflow_status_sent = FALSE;
    /* Reset overflow and overflow_status_sent flags to false */

} /* hdrfetap_init_lo_rec_buffer() */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_GET_PKT_FRAMETIME()

DESCRIPTION
  This function uses the packet HSTR time (half slot unit) and current frame 
  time to return the packet frame time (in 16 slot unit )
  
DEPENDENCIES
  None.

PARAMETERS
  hstr_time     - the hstr time from the decoder
  frame_time    - current system frame time 

RETURN VALUE
  The packet frame time.

SIDE EFFECTS
  None.

===========================================================================*/

static uint16 hdrfetap_get_pkt_frametime 
( 
  uint16 hstr_time, 
    /* half slot time reference */

  uint16 frame_time 
    /* current frame time */
)
{

  uint16 sys_time;
  uint16 pkt_time;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* get the HSTR from the packet tag 
   * the HSTR range from 0-511 which is one Control Channel Cycle*/

  pkt_time = (hstr_time >> 5) & 0x000f; 
    /* convert to the frame time */

  sys_time =  frame_time; 
    /* this is from the PN roll in the unit of 16 slots*/
  
  /*----------------------------------------------------------------------
   * Calculate/Ajust the Packet Time 
   * On the roll-over check we allow a fuzzy check of +4 /-7 to allow
   * correct detection even if we don't receive packets every frame: 
   * Adjust system time for a rollover before the half-slot timestamp 
  ----------------------------------------------------------------------*/

  if ( pkt_time > 0x0008 && (sys_time & 0x000f) < 0x0005 )
  {
    /* hstr_time = 9, a, b, c, d, e, f
       sys_time = 0, 1, 2, 3, 4, 5 */

    HDR_MSG_IS890_2 (MSG_LEGACY_LOW,  "Before: 0x%x -- 0x%x", 
                     pkt_time, sys_time);

    /* if the sytem < 0x5, then roll back to 0x7fff
       else sys_time = (sys_time - 0x5) | 0x000f */

    sys_time = ( (sys_time < 0x0005) ?
                 0x7fff : ((sys_time - 0x0005) | 0x000f) );
  }

  /* not sure if this is possible, seems to the sys_time should always
   * happen after the pkt_time */

  if ( pkt_time < 0x0004 && (sys_time & 0x000f) > 0x000c )
  {
    /* pkt_time = 0x0, 1, 2, 3, 4
       sys_time = 0xd, e, f; seems indicating a big gap */

    HDR_MSG_IS890_2 (MSG_LEGACY_MED,  "After: 0x%x -- 0x%x", 
                     pkt_time, sys_time);

    sys_time = ( (sys_time > 0x7ffc) ? 
                 0x0000 : ((sys_time + 0x0004) & 0xfff0) );
  }

  pkt_time |= (sys_time & 0x7ff0); 
    /* update the pkt time ; and this pkt time is used for RelA TAS */

  return pkt_time;

} /* hdrfetap_get_pkt_frametime  */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_LO_BUFF_ZFILL()

DESCRIPTION
  The function detect the gap b/w the previouse pkt time and the current
  pkt time; then fill the zero records for the gap.
  
DEPENDENCIES
  None.

PARAMETERS
  lo        - Pointer to loopback buffer
  pkt_time  - the current pkt frame time

RETURN VALUE
  uint16 the pkt frame time.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrfetap_lo_buff_zfill
( 
  hdrfetap_lo_buff_type *lo,
    /* Pointer to loopback buffer to initialize */

  uint16                pkt_time 
    /* current pkt frame time */
)
{
  uint16  expect_pkt_time;
  uint16  pkt_time_gap;
  uint8   previous_write_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( lo == NULL )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "NULL LO Pointer" );
    return;
  }

  /* Find the previously used index */

  if ( lo->write_index == 0 )
  {
    previous_write_index = HDRFETAP_LO_RECORD_BUF_SIZE - 1;
  }
  else
  {
    previous_write_index = lo->write_index - 1;
  }

  /* Get the next expected timestamp (previous packet's
   * timestamp + 1) and fill any idle holes til now.    
   *  the expecting one ... the expect_pkt_time */

  expect_pkt_time = 
    lo->record[previous_write_index].fwd_sys_time + 1;

  expect_pkt_time %= 0x8000;

  /* Determine the pkt gap 
   * Sanity check to avoid looping through the whole buffer    
   * and triggering the watch dog. This needs to be less equal 
   * than our maximum number of queued loopback packets.       
   * If the packet is in the current or the next expected frame
   * then there is no gap b/w the 2 pkts.   */

  if(( pkt_time == expect_pkt_time ) ||
     ((( pkt_time + 1 ) % 0x8000 ) == expect_pkt_time ))
  {
    pkt_time_gap = 0;
  }
  else if(pkt_time > expect_pkt_time)
  {

    /* forward gap */

    pkt_time_gap = pkt_time - expect_pkt_time;

  }
  else
  {

    /* Pkt_time wrap around ; this could be a very big number 
     * if the RETAP*/

    pkt_time_gap = 0x8000 - expect_pkt_time + pkt_time;
  }

  if ( pkt_time_gap > HDRIS890A_PKT_BUFFER_SIZE )
  {

    /* For a rollover this is not an error condition, just 
     * set both packet times (received and expected) equal 
     * to avoid filling the buffer with bogus zero-packets. */

    if ( !(( expect_pkt_time < HDRIS890A_PKT_BUFFER_SIZE) &&
           ( pkt_time > (0x8000 - HDRIS890A_PKT_BUFFER_SIZE ))))
    {
        HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  "FETAP: Fwd sys_t gap: %d -> %d",
                         pkt_time, expect_pkt_time);
    }

    expect_pkt_time = pkt_time;
    pkt_time_gap = 0;

  }

  /*----------------------------------------------------------------------
   * Fill the idle holes with 0-record
  ----------------------------------------------------------------------*/

  while( pkt_time_gap > 0 )
  {

    lo->record[lo->write_index].fwd_sys_time =
      expect_pkt_time | HDRFETAP_IDLE_SYS_TIME_FLAG;

    expect_pkt_time++;
    expect_pkt_time %= 0x8000;
    pkt_time_gap--;
    lo->write_index++;
    lo->write_index %= HDRFETAP_LO_RECORD_BUF_SIZE;
    memset( &(lo->record[lo->write_index]),
              0,
              sizeof( hdrfetap_lo_record_type ) );

    /* Check if we caught up with the read index */

    if ( lo->write_index == (lo->read_index -1) )
    {

      /* Shall continue to fill the idle pkt if this condition meets */

      HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: Lo rec buf overflow !" );

      lo->overflow = TRUE;
      lo->overflow_status_sent = FALSE;
        /* Mark the overflow */

      lo->read_index ++ ;
        /* Move the read_index as well, new logic */
    }

  }

} /* hdrfetap_zfill_lo_rec_buffer */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_LO_BUFF_PACK()

DESCRIPTION
  Packs one new record on reception of test packet.
  
DEPENDENCIES
  None.

PARAMETERS
  lo               - Pointer to loopback buffer
  test_pkt         - Pointer to test packet contents for packing

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrfetap_lo_buff_pack
( 
  hdrfetap_lo_buff_type *lo,
    /* Pointer to loopback buffer to initialize */

  hdrfetap_test_pkt     *test_pkt
    /* Pointer to test packet contents for packing */
)
{
  uint8                 tca_seqno;
  uint8                 previous_write_index;
  uint8                 full_lo_record_required = 0x00;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if( lo == NULL )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "NULL LO Pointer" );
    return;
  }

  if ( lo->write_index == 0 )
  {
    previous_write_index = HDRFETAP_LO_RECORD_BUF_SIZE - 1;
  }
  else
  {
    previous_write_index = lo->write_index - 1;
  }

  lo->last_drc_cover_field = (test_pkt->drc_cover << 4) & 0x70;
    /* Fill out the DRC cover field */

  hdrfetap_lo_buff_zfill ( lo, test_pkt->pkt_frm_time );
    /* Fill in zero records from last time a pkt was received to now */

  /*----------------------------------------------------------------------
   * Fill the current record
   * FULL REC has 35 bits
   * min  Rec has 13 bits
  ----------------------------------------------------------------------*/

  /* Check if our TCA sequence number is still valid */

  if ( (tca_seqno = hdrrup_get_current_tca_seq_num()) !=
       lo->current_tca_seqno )
  {
    /* if TCA seq no. changed; TCAmsgSeq has 8 bits
     * tca_seqno is defined as 8 bits */

    lo->current_tca_seqno = tca_seqno;
    lo->current_tca_upper_byte = (tca_seqno >> 1) | 0x80; 
      /* TCAMsgSeqIncluded(MSB) + TCAMsgSeq[7:1] */

    lo->current_tca_lower_byte = (tca_seqno << 7) & 0x80;  
      /* TCAMsgSeq[0](MSB) | 0x0 */

    full_lo_record_required = 0x80;
      /* if the TCA seq is changed, need a full record */
  }
  else if ( test_pkt->pkt_fwd_seq != 
            (lo->record[previous_write_index].fwd_seq + 1) )
  {
    full_lo_record_required = 0x80;
      /* seq no gap exist, need full record */
  }

  /* Store test packet info in the current record; ... */

  lo->record[lo->write_index].fwd_sys_time = test_pkt->pkt_frm_time;
  if ( test_pkt->pkt_frm_time >= 0x8000 )
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                     "FETAP: Invalid pkt_frm_time 0x%x", 
                     test_pkt->pkt_frm_time );
  }

  lo->record[lo->write_index].full_rec[0] =
    lo->current_tca_upper_byte;

  lo->record[lo->write_index].full_rec[1] =
    lo->current_tca_lower_byte
    | lo->last_drc_cover_field
    | test_pkt->num_rx_slots;

      /* fun_rec[1] = current_tca_lower_byte | 
       *                last_drc_cover_field | 
       *                        num_rx_slots 
       * ful_rec[1] = 0bs ddd ssss */

  lo->record[lo->write_index].min_rec[0] =
    lo->record[lo->write_index].full_rec[1] & 0x7f;
      /* min_rec[0] : 7 LSB of the full_rec[1]
       * min_rec[0] = 0b0 ddd ssss */

  /* ------------------------------------------------------------------------
   * records are different for the PL_01, PL_2 and PL_3 loop back packets
   *   PL_01              
   *     full_rec[2] #MacPkts(2bits)+seqincluded(1bits)+FwdSeq(5bits)
   *     full_rec[3] FwdSeq(8bits)
   *     full_rec[4] FwdSeq(1bits)
   *   Pl_2
   *     full_rec[2] FPLidx(4bits)+seqincluded(1bits)+FwdSeq(3bits)
   *     full_rec[3] FwdSeq(8bits)
   *     full_rec[4] FwdSeq(3bits)
   *   Pl_3
   *     full_rec[2] FPLidx(4bits)+seqincluded(1bits)+FwdSeq(3bits)
   *     full_rec[3] FwdSeq(8bits)
   *     full_rec[4] FwdSeq(3bits)+SubActiveSetID(4bits)
   -------------------------------------------------------------------------*/

  if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_0 || 
       hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_1 )
  {
    /* sanity check the DRC value */

    if ( test_pkt->drc_value > 0xc ) 
    {
      HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                       "FETAP: Invalid DRC value for at ftype01 %d", 
                       test_pkt->drc_value );
      test_pkt->drc_value = 0x0;
    }

    lo->record[lo->write_index].full_rec[2] =
      hdrfetap_drc_to_pl01_pkt_field_map[test_pkt->drc_value] |
      0x20 | 
      (( test_pkt->pkt_fwd_seq >> 9 ) & 0x1f );
      /* the full_rec[2] 
       * #ofFwdMacPkts (2MSB) + FwdSeqNoIncluded (1bit) + FwdSeq (5msb) */

    lo->record[lo->write_index].min_rec[1] =
      lo->record[lo->write_index].full_rec[2] & 0xc0;
      /* min_rec[1] does not inclde the FwdSeq */

    lo->record[lo->write_index].full_rec[3] =
      (test_pkt->pkt_fwd_seq >> 1) & 0x00ff;
      /* full_rec[3] : pkt_fwd_seq  8 bit */

    lo->record[lo->write_index].full_rec[4] =
      (test_pkt->pkt_fwd_seq << 7) & 0x80;
      /* full_rec[4] : pkt_fwd_seq  1 bit  left */

  } 
  else if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
  {
    lo->record[lo->write_index].full_rec[2] =
      ((test_pkt->pkt_size << 4) & 0xf0)
      | 0x8
      | ((test_pkt->pkt_fwd_seq >> 11) & 0x7);
      /* the full_rec[2] 
       * FPLindex(4MSB) + FwdSeqNoIncluded (1bit) + FwdSeq (3msb) */

    lo->record[lo->write_index].min_rec[1] =
      lo->record[lo->write_index].full_rec[2] & 0xf0;
      /* min_rec[1] does not inclde the FwdSeq */

    lo->record[lo->write_index].full_rec[3] =
      (test_pkt->pkt_fwd_seq >> 3) & 0x00ff;
      /* full_rec[3] : pkt_fwd_seq  8 bit */

    lo->record[lo->write_index].full_rec[4] =
      (test_pkt->pkt_fwd_seq << 5) & 0xe0;
      /* full_rec[4] : pkt_fwd_seq  3 bit  left */
  }
  else if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
  {
    lo->record[lo->write_index].full_rec[2] =
      ((test_pkt->pkt_size << 4) & 0xf0)
      | 0x8
      | ((test_pkt->pkt_fwd_seq >> 11) & 0x7);
      /* the full_rec[2] 
       * FPLindex(4MSB) + FwdSeqNoIncluded (1bit) + FwdSeq (3msb) */

    lo->record[lo->write_index].min_rec[1] =
      ( lo->record[lo->write_index].full_rec[2] & 0xf0 ) | 
      ( ( test_pkt->sas_id >> 1 ) & 0x7 );
      /* min_rec[1] does not inclde the FwdSeq, 3 LSBs are SubActiveSetID 3 
      MSBs */

    lo->record[lo->write_index].min_rec[2] = ( test_pkt->sas_id << 7 ) & 0x80;
      /* Bit7 of min_rec is LSB of SubActiveSetID */

    lo->record[lo->write_index].full_rec[3] =
      (test_pkt->pkt_fwd_seq >> 3) & 0x00ff;
      /* full_rec[3] : pkt_fwd_seq  8 bit */

    lo->record[lo->write_index].full_rec[4] =
      ( (test_pkt->pkt_fwd_seq << 5) & 0xe0 ) | 
      ( (test_pkt->sas_id << 1) & 0x1e );
      /* full_rec[4] : pkt_fwd_seq 3 bit, sas_id 4bit, 1 bit remainder */
  }
  else 
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: invalid subtype %d",
                     hdrfetap.hdrfetap.configuration.phy_subtype );
  }

  lo->record[lo->write_index].fwd_seq = test_pkt->pkt_fwd_seq;

#ifdef HDRFETAP_LO_PKT_DBG

  lo->record[lo->write_index].tca_msg_seq = tca_seqno;
  lo->record[lo->write_index].drc_cover = test_pkt->drc_cover;
  lo->record[lo->write_index].fwd_phys_slots =
    (uint8) (hdris890a_num_rx_slots & 0x0000000f);
  lo->record[lo->write_index].pkt_size = test_pkt->pkt_size;

#if 0
  HDR_MSG_IS890_3( MSG_LEGACY_MED,  "FETAP: drcv%d, drcc%d: ps%d",
                   test_pkt->drc_value,
                   test_pkt->drc_cover, 
                   test_pkt->pkt_size );
#endif 
#endif /* HDRFETAP_LO_PKT_DBG */

  /*----------------------------------------------------------------------
   * Indicate if a full record is required due to a TCA 
   * sequence change or a packet sequence gap by setting
   * the msb of the minimum record (this would not be  
   * valid as the tca is per definitionem not included.
  ----------------------------------------------------------------------*/

  lo->record[lo->write_index].min_rec[0] |= 
    full_lo_record_required;

  lo->last_fwd_sys_time = test_pkt->pkt_frm_time;
    /* Update the last forward system time we received a test pkt */

  /* Advance the write index and initialize the new record */

  lo->write_index++;
  lo->write_index %= HDRFETAP_LO_RECORD_BUF_SIZE;
  memset( &(lo->record[lo->write_index]),
          0,
          sizeof( hdrfetap_lo_record_type ) );

  /* Check if we caught up with the read index; it is not a real
   * overflow, but we keep it compatible with old design;
   * Now read_index is moved as well in case of overflow */
  
  if ( lo->write_index == ((lo->read_index -1) % 0x8000))
  {

    HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: Lo rec buf overflow !" );
    lo->overflow = TRUE;
    lo->overflow_status_sent = FALSE;

    lo->read_index ++ ;
      /* move the read_index as well, new logic */

  }

#ifdef TIMETEST_DSRLP_STATS

  /* Update received slots stats for profiling */

  hdrfetap.hdrfetap_num_rcvd_slots += (hdris890a_num_rx_slots & 0x000000ff);

#endif

} /* hdrfetap_lo_buff_pack */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_PCP_GET_PKT()

DESCRIPTION
  This function is a callback to PCP which is called for the transmit data. 
  The callback specifies which carrier it is for. FMCTAP must transmit 
  loopback packets on the carrier it was received. 

  This function performs the following steps:
    1.  Get the loopback buffer associated with the specified carrier.
    2.  Refreshe the loopback buffer to generate any 0 loopback packet records.
    3.  Generate the next loopback packet to fit in to the buffer allocated by
        PCP.
    4.  Return the number of bytes in the packet generated.

DEPENDENCIES
  None
 
PARAMETERS
  combo_id           - Combo ID for this packet to be built
  carrier_id         - Carrier for which packet is to be built
  pcp_allocation     - Maximum size of reverse link packet
  tx_pkt_ptr         - Pointer to DSM item containing TX packet
  tx_set_id          - Not used in FETAP
  user_data_ptr      - NOT used in FETAP
 
RETURN VALUE
  Size of the transmit data in byte
  
SIDE EFFECTS
  None
===========================================================================*/
static uint16 hdrfetap_pcp_get_pkt
(
  uint8           combo_id,
    /* Which transmit combination */

  uint8           carrier_id,
    /* Carrier data is to be transmitted */  

  uint16          pcp_allocation,
    /* availabe bandwidth */

  dsm_item_type   **tx_pkt_ptr,
    /* dsm item ptr (return) */

  void            **tx_set_id,
    /* Generic PCP get_pkt interface; NOT used in the FETAP */

  void            *user_data_ptr
    /* Generic PCP get_pkt interface; NOT used in the FETAP */
)
{
  uint16                  fetap_pkt_len = 0;
  hdrfetap_lo_buff_type   *lo           = NULL;
  boolean                 build_packet  = TRUE;
  static uint16           last_fwd_sys_time = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Mark the unused symbols to avoid lint errors */

  SYS_ARG_NOT_USED( tx_set_id );
  SYS_ARG_NOT_USED( user_data_ptr );

  /*-------------------------------------------------------------------------
   *  Sanity Check List
   *  1. the fetap state
   *  2. the available bandwith
   *  3. the loop back mode
   *  4. if a valid allocaiton
   *  5. if an allocation bigger then a minimum pkt size
   *  6. if the record buffer is empty
  -------------------------------------------------------------------------*/

  /* Sanity check on the maximum size */

  if ( pcp_allocation == 0 )
  {
    build_packet = FALSE;
  }

  /* Check if we are on the traffic channel */

  if ( hdrfetap.hdrfetap.state != HDRFETAP_TRAFFIC ) 
  {
    build_packet = FALSE;
  }

  lo = hdrfetap_lo_buff_get_from_carrier_id( carrier_id );
    /* Get pointer to LO buf from carrier_id */

  if( lo != NULL && lo->enabled == FALSE ) 
  {
    build_packet = FALSE;
      /* Check if loopback mode is enabled */
  }
 
  if( lo != NULL && build_packet == TRUE )
  {

    hdrfetap_lo_buff_refresh( lo, hdrfetap_get_sys_time() );
      /* Update the lo record buffer; inside the last_fwd_sys_time is updated
       * to the current system time. this has to be located before the below
       * sanith check to insure the refreshment  */

    /* Sanity check the sync guard to make sure the PCP callings are in sync 
     * for RMAC01, the pre_pkt_status is also called from the PCP. */
  
    if ( hdrfetap.hdrfetap.configuration.sync_flag[carrier_id] == 
      PCP_CALLED_PRE_PKT_STATUS ) 
    {
      /* This is the 1st get_pkt since the pre_pkt_status call */
      hdrfetap.hdrfetap.configuration.sync_flag[carrier_id] = PCP_CALLED_GET_PKT;
  
      if ( lo->tmp_read_index != lo->read_index )
      {
  
        HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: index does not match!" );
        lo->tmp_read_index = lo->read_index;
  
      }
    }
    else
    {
      if ( ( last_fwd_sys_time == lo->last_fwd_sys_time ) && 
        ( ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 ) ||
        ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 ) ) )
      {
        /* Previous packet status not called.  Also there has not been 
           a change in the observation period. */
        HDR_MSG_IS890_1( MSG_LEGACY_HIGH, 
          "Ch %d: GetPkt called without PrevPktStatus - returning '0'.",
          carrier_id );

        return 0;
      }
    }

#ifdef HDRFETAP_LO_PKT_DBG
    HDR_MSG_IS890_2 ( MSG_LEGACY_MED, "FETAP: PCP get pkt c=%d alloc=%d ",
                      carrier_id,
                      pcp_allocation );
#endif /* HDRFETAP_LO_PKT_DBG */
  
    /* Generate 1 test packet */
    fetap_pkt_len = hdrfetap_lo_buff_gen_pkt( 
                      lo, 
                      tx_pkt_ptr, 
                      pcp_allocation );
      
    if ( fetap_pkt_len > 0  ) 
    {
      hdrfetap.hdrfetap.lo_pkt_cnt++;
    }

    /* Store last fwd system time */
    last_fwd_sys_time = lo->last_fwd_sys_time;
      
  }
  else
  {
    HDR_MSG_IS890_4 ( MSG_LEGACY_HIGH, 
                      "FETAP: LO 0x%x PCP %d State %d phy %d",
                      lo,
                      pcp_allocation,
                      hdrfetap.hdrfetap.state, 
                      hdrfetap.hdrfetap.configuration.phy_subtype );
  }

  return fetap_pkt_len;

} /* hdrfetap_pcp_get_pkt */

#ifdef TIMETEST_DSRLP_STATS
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_LOG_TIMETEST_THROUGHPUT()

DESCRIPTION
  This function is a clock callback function to control logging of the
  total number of slots received and loopback packets sent.
  It is controlled by the dsrlp_timetest_cb clk_cb_type and is 
  registered in dsrlp_init() to go off every 500 ms.
  To profile the test app with link utilization, use the regular
  timetest profiling setup and postprocess the profiling data
  with "hp_stats -a" to run in IS-890 mode.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hdrfetap_log_timetest_throughput
( 
  int4 ms 
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  SYS_ARG_NOT_USED( ms );

  if ( hdrfetap.hdrfetap_num_rcvd_slots == 0 ) return;

  HDR_MSG_IS890_2 (MSG_LEGACY_HIGH, "Logging TA TIMETEST (%d/%d)",
                   hdrfetap.hdrfetap_num_rcvd_slots,
                   hdrfetap.hdrfetap_num_sent_lo_rcds);

  TIMETEST_DSRLP_BYTES( TIMETEST_DSRLP_IS856,
                        (uint8)(hdrfetap.hdrfetap_num_rcvd_slots/2), 
                        (uint8)(hdrfetap.hdrfetap_num_sent_lo_rcds/2) );

  hdrfetap.hdrfetap_num_rcvd_slots = 0;
  hdrfetap.hdrfetap_num_sent_lo_rcds = 0;

} /* hdrfetap_log_timetest_throughput */

#endif /* TIMETEST_DSRLP_STATS */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_STATISTICS_INITIALIZATION()

DESCRIPTION
  Performs statistics intitialization procedure defined in FMCTAP spec
  section 6.7.2.1.1.

PARAMETERS
  None.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Clears values in statistics structure.

===========================================================================*/
static void hdrfetap_statistics_initialization( void )
{
  uint8         sas_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  hdrfetap.hdrfetap.statistics.idle_asp_change = 0;
  hdrfetap.hdrfetap.statistics.idle_time = 0;
  hdrfetap.hdrfetap.statistics.connected_ss_change = 0;
  hdrfetap.hdrfetap.statistics.connected_time = 0;
  hdrfetap.hdrfetap.statistics.first_sync_cc_pkt = 0;
  hdrfetap.hdrfetap.statistics.cc_time = 0;

  for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
  {
    hdrfetap.hdrfetap.statistics.connected_ss_change_mc[sas_id] = 0;
  }
} /* hdrfetap_stats_initialization */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_CONFIGURATION_INITIALIZATION()

DESCRIPTION
  Performs configuration intitialization procedure defined in FMCTAP spec
  section 6.7.1.1.1 or 6.7.1.1.2. If the check_persistence flag is TRUE
  then the procedure in 6.7.1.1.2 is followed. If the check_persistence
  flag is FALSE then the procedure in 6.7.1.1.1 is followed.

PARAMETERS
  config            - Pointer to configuration to initialize.
  check_persistence - Flag indicating that the persistence flag should be
                      obeyed on initialization.
DEPENDENCIES
  None.

RETURN VALUE
  TRUE if loopack mode is enabled, FALSE if it is not.

SIDE EFFECTS
  None.

===========================================================================*/
static boolean hdrfetap_configuration_initialization(

  hdrfetap_config_type  *config,
    /* Pointer to configuration object to intialize */

  boolean               check_persistence
    /* Flag to obey persistence flag on initialization */
)
{
  uint8         sas_id;
  boolean       loopback_enabled = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  config->ack_channel_fixed_mode         = FALSE;
  config->ack_channel_modtype_fixed_mode = FALSE;
    /* ACK channel fixed bit mode off and fixed modulation mode off */

  config->drc_value_fixed_mode           = FALSE;
    /* DRC value fixed mode off*/

  config->drc_cover_fixed_mode           = FALSE;
    /* DRC cover fixed mode off*/

  if( check_persistence )
  {
    if ( config->loopback_mode_config == LOM_NON_PERSISTENT )
    {
      config->loopback_mode = FALSE;
        /* Loop back mode off if persistent bit unset */
    }
    else if ( config->loopback_mode_config == LOM_PERSISTENT )
    {
      loopback_enabled = TRUE;
        /* Mark loopback enabled so transmit callbacks aren't cleared */
    }
  }
  else
  {
    config->loopback_mode = FALSE;
      /* Disable loopback if not checking persistence */
  }
  
  /* Loop through to reset values for each possible sub-active set */
  for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
  {
    config->ack_channel_fixed_mode_mc[sas_id]        = FALSE;
    config->ack_channel_modtype_fixed_mode_mc[sas_id]= FALSE;
      /* ACK channel fixed bit mode off and fixed modulation mode off */

    config->drc_value_fixed_mode_mc[sas_id]          = FALSE;
      /* DRC value fixed mode off*/

    config->drc_cover_fixed_mode_mc[sas_id]          = FALSE;
      /* DRC cover fixed mode off*/

    if( check_persistence )
    {
      if( config->loopback_mode_config_mc[sas_id] == LOM_NON_PERSISTENT )
      {
        config->loopback_mode_mc[sas_id] = FALSE;
          /* Loop back mode off if persistent bit unset */
      }
      else if ( config->loopback_mode_config_mc[sas_id] == LOM_PERSISTENT )
      {
        loopback_enabled = TRUE;
          /* Mark loopback enabled so transmit callbacks aren't cleared */
      }
    }
    else
    {
      config->loopback_mode_mc[sas_id] = FALSE;
        /* Disable loopback if not checking persistence */
    }
  }

  return loopback_enabled;
} /* hdrfetap_configuration_initialization */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_ACTIVATE()

DESCRIPTION
  This function activates the FETAP once it has been negotiated
  by the stream protocol.

DEPENDENCIES
  None
  
PARAMETERS
  fetap_stream_id - Stream ID negotiated for IS890A
  app_subtype     - Negotiated application subtype for TAP.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrfetap_activate
(
  uint8 fetap_stream_id,
    /* stream ID */
  
  uint16 app_subtype
    /* application subtype */
)
{
  uint8 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset( &( hdrfetap.hdrfetap ), 0, sizeof( hdrfetap.hdrfetap ) );
    /* init the configuration, stats, state and some miscellaneous vars */

  hdrfetap.hdrfetap.configuration.stream_id = (hdrhai_stream_enum_type)fetap_stream_id;
    /* Store the stream ID negotiated for fetap */

  hdrfetap.hdrfetap.configuration.protocol_id = 
    (hdrhai_protocol_name_enum_type)hdris890a_prot_id[
      hdrfetap.hdrfetap.configuration.stream_id];

  hdrfetap.hdrfetap.configuration.app_subtype = app_subtype;
    /* store the applciation subtype passed in */

  hdrfetap.hdrfetap.configuration.phy_subtype = 
    (hdrfetap_phy_subtype_enum_type)hdrscp_get_current_subtype( 
      HDRHAI_PHYSICAL_LAYER_PROTOCOL );
    /* get the PHY subtype */

  hdrfetap.hdrfetap.configuration.flow_config.substream_id = HDRFETAP_SUBSTREAM_ID;
    /* FETAP default substream id */

#ifdef TIMETEST_DSRLP_STATS
  /* Register the clock callback for TIMETEST throughput profiling */
  if ( ! hdrfetap.hdrfetap_timetest_cb_is_registered )
  {
    timer_def2(&hdrfetap.hdrfetap_timetest_cb, NULL);
    hdrfetap.hdrfetap_timetest_cb_is_registered = TRUE;
  }
#endif

  /* Init the last link id for each carrier */

  for( i=0; i < ARR_SIZE(hdrfetap.hdrfetap.last_link_id); i++ ) 
  {
    hdrfetap.hdrfetap.last_link_id[ i ] = HDRSRCH_NO_LINK_ID;
  }
    

  /* Start in the NOT_ACQ state, we'll go IDLE as soon 
   * as HDR is acquired the first time. */

  hdrfetap.hdrfetap.state = HDRFETAP_NOT_ACQ;

  /* If HDR has already been acquired, register for PN 
   * roll interrupt. If HDR has not been acquired, we 
   * will register this later, after HDR has been    
   * acquired.; still use HDRHAI_IS890_FTAP for now */

  if(hdralmp_system_is_acquired( HDRHAI_IS890_FTAP ))
  {
    hdrts_register_roll_callback( hdrfetap_process_pn_roll_int );
  }

  hdrfetap.hdrfetap.fetap_called = FALSE;
    /* Initialize the fetap_called flag. The flag is used to indicate 
     * if FETAP was called in the current connection */

  hdrfetap.hdrfetap.expecting_idle_ind = FALSE;
    /* Initialize the connection_closed flag. This flag is used to filter
       idle_handoff indication that occurs after a connection close.  (Idle
       handoff ind. is always given as a part of the connection close call 
       flow. Idle handoff indication is used for collecting stats in TAP, it 
       should not be counted here.) */


  hdrfetap_statistics_initialization();
    /* Initialize stats values to default */

  hdrfetap.hdrfetap.tca_seq_for_map = 0xFFFF;
    /* Initialize this to an invalid TCA sequence number value. */

} /* hdrfetap_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_PUT_PKT()

DESCRIPTION
  This function is called by hdris890a_put_pkt() to deliver
  fetap receive data.

  Information about received test packets is stored in the per carrier 
  record ring buffer. This buffer is used by hdrfetap_get_pkt() to aggregate
  records for one frame time into one (or more if the RMAC packet
  is too small) loopback packet(s). The info collected is 

  1. the packet decode timestamp, 
  2. the DRC cover 
  3. the DRC value, 
  4. the last TCA message sequence number.

  NOTE: The decoder packet timestamp is only 4 bit after converting
        to frame time. It is combined with the current system time to
        get the required 15 bit timestamp. However, due to processing
        latency and the rollover period of 16 frames of the decoder
        timestamp itself, system time needs to be adjusted accordingly.
        The following diagram shows the relation between the decoder
        timestamp and system time at processing:

        8    9    a    b    c    d    e    f    0    1    2    3
        |----|----|----|----|--r-|----|----|----|----|-p--|----|----

        (r) designates the time a packet was decoded (dec. timestamp 0xc),
        (p) designates the time a packet was delivered to fetap by the
        FMAC (0x1 is the current 4 LSB value of system time). Profiling
        shows that (p) might happen as late as 6 frames after (r), due
        to wait times in the FMAC resequencing queue. Hence for a packet
        with a decoder timestamp of 0x9 - 0xf, the system time is adjusted
        backward if the 4 LSBs of system time are 0x0 - 0x3.
   
DEPENDENCIES
  None
 
PARAMETERS
  rx_pkt_ptr    - DSM item chain pointer with received packet
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_put_pkt
(
  dsm_item_type*   rx_pkt_ptr
    /* The ptr of the DSM item containing the incoming FETAP test pkt */
)
{
  uint16                hstr_time   = 0;
  hdrsrch_link_id_type  link_id;
  uint16                sys_time;
  hdrfetap_test_pkt     test_pkt;
  hdrfetap_lo_buff_type *lo;
  uint8                 carrier_id;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*----------------------------------------------------------------------
   * Sanity CHECK the "PacketType"
  ----------------------------------------------------------------------*/

  if ( (*(rx_pkt_ptr->data_ptr) & 0x0f) != 0x00 ) 
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: unkwn pkt" );
    dsm_free_packet( &rx_pkt_ptr );
    return;
  }
  
  hdrfetap.hdrfetap.tst_pkt_cnt++;

  /*----------------------------------------------------------------------
   * carrier packet was received 
  ----------------------------------------------------------------------*/

  carrier_id = HDRDSM_GET_CARRIER_ID( rx_pkt_ptr ) & 0xF;

  if ( carrier_id >= HDRIS890B_NUM_CARRIERS ) 
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: bogus carrier id %d", 
                     carrier_id );
    carrier_id = 0;
  }

  /*----------------------------------------------------------------------
   * Extract the the seq number inside the incoming pket 
   * the format of the incoming FETAP test packet
   * Stream-id        2bits
   * Protocol-id      2bits      0x0
   * PacketType       4bits      0x0
   * Seqno           14bits      
   * Reserved         2bits      
   * PNFill          Varialbe
  ----------------------------------------------------------------------*/

  test_pkt.pkt_fwd_seq  = *(rx_pkt_ptr->data_ptr + 2);
  test_pkt.pkt_fwd_seq |= *(rx_pkt_ptr->data_ptr + 1) << 8;
  test_pkt.pkt_fwd_seq  = test_pkt.pkt_fwd_seq >> 2;

  /*----------------------------------------------------------------------
   * Sanity CHECK the "SeqNo" see if it is consec. with the previous one 
  ----------------------------------------------------------------------*/

  if ( test_pkt.pkt_fwd_seq > (hdrfetap.hdrfetap.last_fwd_seq[ carrier_id ] + 1) )
  {
    HDR_MSG_IS890_2 ( MSG_LEGACY_HIGH,  "FETAP: Fwd seq fgap %d -> %d",
                      hdrfetap.hdrfetap.last_fwd_seq[ carrier_id ], 
                      test_pkt.pkt_fwd_seq );
  }

  if ( test_pkt.pkt_fwd_seq <= hdrfetap.hdrfetap.last_fwd_seq[ carrier_id ] )
  {
    /* backward seq no, just output error message */

    HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  "FETAP: Fwd seq bgap %d <- %d",
                     hdrfetap.hdrfetap.last_fwd_seq[ carrier_id ], test_pkt.pkt_fwd_seq);
  }

  hdrfetap.hdrfetap.last_fwd_seq[ carrier_id ] = test_pkt.pkt_fwd_seq;

  /*----------------------------------------------------------------------
   * pkt_frm_time from HSTR and sys_time is from the time serivce
   * Half-slot timestamp of the received packet, convert to  
   * frames (leaves 4 bits, timestamp is a 9 bit value), and 
   * adjust with the global sys time's upper 12 bits.        
  ----------------------------------------------------------------------*/

  hstr_time = HDRDSM_GET_TIMESTAMP( rx_pkt_ptr );
  sys_time = hdrfetap_get_sys_time();

  test_pkt.pkt_frm_time = hdrfetap_get_pkt_frametime ( hstr_time, sys_time );
    /* combine the hstr and current systime to obtain the receive time */
 
#if 0 
  HDR_MSG_IS890_3 (MSG_LEGACY_HIGH,  "s_t=0x%x, pkt_t=0x%x, prev_t=0x%x",
                sys_time, test_pkt.pkt_frm_time, hdrfetap.hdrfetap.previous_pkt_time );
#endif

  hdrfetap.hdrfetap.previous_pkt_time = test_pkt.pkt_frm_time; 
    /* update the hdrfetap_previous_pkt_time here; the variable is only for
     * debug purpose here; may remove it later */

  /*----------------------------------------------------------------------
   * DRC cover is obtained via the link ID
  ----------------------------------------------------------------------*/

  link_id = HDRDSM_GET_LINK_ID( rx_pkt_ptr );

  if ( link_id != hdrfetap.hdrfetap.last_link_id[ carrier_id ] )
  {
    hdrfetap.hdrfetap.last_link_id[ carrier_id ] = link_id;
    hdrfetap.hdrfetap.last_drc_cover[ carrier_id ] = 
      (uint8) hdrsrch_get_drc_cover( link_id );
    if ( hdrfetap.hdrfetap.last_drc_cover[ carrier_id ] == 0xFF ) 
    {

      HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: Invalid DRC cover%d", 
                       hdrfetap.hdrfetap.last_drc_cover[ carrier_id ] );

    }

    HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  "FETAP: LinkID %d (DRCCover %d)",
                     hdrfetap.hdrfetap.last_link_id[ carrier_id ], 
                     hdrfetap.hdrfetap.last_drc_cover[ carrier_id ] );
  }

  test_pkt.drc_cover = hdrfetap.hdrfetap.last_drc_cover[ carrier_id ];
    /* Copy DRC Cover value to test packet for packing later */
  
  /*----------------------------------------------------------------------
   * DRC value; it is increased to 0x0e in relA FMAC
  ----------------------------------------------------------------------*/

  test_pkt.drc_value = HDRDSM_GET_DRC( rx_pkt_ptr );

  if ( test_pkt.drc_value > HDRHAI_MAX_REL_A_DRC )
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: bogus DRC value %d", 
                      test_pkt.drc_value );
    test_pkt.drc_value = 0x00;
  }
 
  /*----------------------------------------------------------------------
   * pkt length value; under same DRC, the pkt can have different length 
  ----------------------------------------------------------------------*/

  test_pkt.pkt_size = HDRDSM_GET_PACKET_SIZE( rx_pkt_ptr );

  if ( test_pkt.pkt_size > HDRFETAP_MAX_FTC_PL_3_PAYLOAD_SIZE_INDEX )
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: bogus pkt size %d", 
                      test_pkt.pkt_size );
    test_pkt.pkt_size = 0x0;
  }

  /*----------------------------------------------------------------------
   * Number of slots for packet decode
  ----------------------------------------------------------------------*/

  test_pkt.num_rx_slots = HDRDSM_GET_SLOTS_TO_DECODE( rx_pkt_ptr ) & 0xF;


  /*----------------------------------------------------------------------
   * Map carrier ID to sub-activeset index
  ----------------------------------------------------------------------*/

  test_pkt.sas_id = hdrfetap_get_freq_idx_from_carrier_id( carrier_id );

#ifdef HDRFETAP_LO_PKT_VERBOSE_DBG
  #error code not present
#else
  HDR_MSG_IS890_3 (MSG_LEGACY_LOW, "FETAP: c=%d hstr=%d drcv=%d",
                   carrier_id,
                   hstr_time,
                   test_pkt.drc_value );
#endif

  /*----------------------------------------------------------------------
   * Free the DSM item
  ----------------------------------------------------------------------*/

#ifdef FEATURE_HDR_DSMREUSE_OPT   
  HDRDSM_FREE_REUSE_DATA_PATH_PACKET( rx_pkt_ptr );
#else
  dsm_free_packet( &rx_pkt_ptr );
#endif
  
  /* Get LO buf for this carrier ID */
  lo = hdrfetap_lo_buff_get_from_carrier_id( carrier_id );

  if ( (lo != NULL) && (lo->enabled) )
  {
    /* Pack contents in to next loopback packet */
    hdrfetap_lo_buff_pack( lo, &test_pkt );
  }

} /* void hdrfetap_put_pkt */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRfetap_PROCESS_STATS_GET_REQUEST()

DESCRIPTION
  This function handles the fetap statistics request.

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrfetap_process_stats_get_request
(
  dsm_item_type**  msg_ptr 
    /* the ptr to the DSM item containing the StatsGetReq msg */
)
{
  dsm_item_type*            reply_ptr = NULL;
  uint8                     seqno;
  uint8                     attribute_id;
  uint16                    attribute_offset;
  uint8                     sas_id;
  uint8                     num_subactive_sets;
  uint8                     attribute_length;
  uint8                     current_tca_seqno;

  hdrlmac_carrier_map_type  carrier_map[HDRIS890B_NUM_CARRIERS];
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Get and check the validity of this message */

  if ( dsm_pullup( msg_ptr, &seqno, 1 ) != 1 )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: cannot read seqno" );
    return;
  }
  if ( (! hdrfetap.hdrfetap.configuration.stats_get_seqno_is_valid) ||
       (HDRIS890A_SEQNO_IS_VALID( seqno,
                                 hdrfetap.hdrfetap.configuration.stats_get_seqno )) )
  {
    hdrfetap.hdrfetap.configuration.stats_get_seqno_is_valid = TRUE;
    hdrfetap.hdrfetap.configuration.stats_get_seqno = seqno;
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: bogus GetStats. (%d)", seqno );
    return;
  }

  HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "FETAP: Rcvd GetStats (%d)", seqno);

#ifdef HDRFETAP_LO_PKT_DBG
  HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  "FETAP: Rx %d pkts, Tx %d lo pkts", 
                   hdrfetap.hdrfetap.tst_pkt_cnt, hdrfetap.hdrfetap.lo_pkt_cnt);
#endif

  /* Allocate space for the reply (20 bytes max. size) */

  reply_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
  if ( reply_ptr == NULL )
  {
    ERR_FATAL( "FETAP: Out of DSM items", 0, 0, 0 );
  }

  ( void )dsmbit_pack8_tail( reply_ptr,
                             HDRFETAP_MSG_STATS_GET_RESPONSE,
                             0,
                             8 );
  ( void )dsmbit_pack8_tail( reply_ptr,
                             seqno,
                             8,
                             8 );
  attribute_offset = 16;

  /* Loop through the attributes present in the message */

  while( dsm_pullup( msg_ptr, &attribute_id, 1 ) == 1 )
  {
    switch( attribute_id )
    {
      case HDRFETAP_ATTR_IDLE_ASP_STATS:

        /* Idle time is supposed to be in slot accuracy, so update it here */

        if ( hdrfetap.hdrfetap.state == HDRFETAP_IDLE )
        {
          hdrfetap_update_slot_time( &(hdrfetap.hdrfetap.statistics.idle_time) );
        }

        /* Length of attribute is 6 bytes */

        ( void )dsmbit_pack8_tail( reply_ptr,
                                   0x06,
                                   attribute_offset,
                                   8 );
        attribute_offset += 8;

        ( void )dsmbit_pack8_tail( reply_ptr,
                           HDRFETAP_ATTR_IDLE_ASP_STATS,
                           attribute_offset,
                           8 );
        attribute_offset += 8;

        ( void )dsmbit_pack16_tail( reply_ptr,
                                    hdrfetap.hdrfetap.statistics.idle_asp_change,
                                    attribute_offset,
                                    16 );
        attribute_offset += 16;

        ( void )dsmbit_pack32_tail( reply_ptr,
                                    hdrfetap.hdrfetap.statistics.idle_time,
                                    attribute_offset,
                                    24 );
        attribute_offset += 24;

        break;
      
      case HDRFETAP_ATTR_CONNECTED_SS_STATS:

        /* Conn. time is supposed to be in slot accuracy, so update it here */

        if ( hdrfetap.hdrfetap.state == HDRFETAP_TRAFFIC )
        {
          hdrfetap_update_slot_time( &(hdrfetap.hdrfetap.statistics.connected_time) );
        }

        /* Update the serving sector change count */

        hdrfetap.hdrfetap.statistics.connected_ss_change = hdrlmac_get_handoff_count( 
          HDRFETAP_DEFAULT_SUBACTIVE_SET );

        /* Length of attribute is 6 bytes */

        ( void )dsmbit_pack8_tail( reply_ptr,
                                   0x06,
                                   attribute_offset,
                                   8 );
        attribute_offset += 8;

        ( void )dsmbit_pack8_tail( reply_ptr,
                                   HDRFETAP_ATTR_CONNECTED_SS_STATS,
                                   attribute_offset,
                                   8 );
        attribute_offset += 8;

        ( void )dsmbit_pack16_tail( reply_ptr,
                                    hdrfetap.hdrfetap.statistics.connected_ss_change,
                                    attribute_offset,
                                    16 );
        attribute_offset += 16;

        ( void )dsmbit_pack32_tail( reply_ptr,
                                    hdrfetap.hdrfetap.statistics.connected_time,
                                    attribute_offset,
                                    24 );
        attribute_offset += 24;

        break;
      
      case HDRFETAP_ATTR_FIRST_SYNC_CC_PKT_STATS:

        /* CC time is connected + idle time, update those */

        if ( hdrfetap.hdrfetap.state == HDRFETAP_TRAFFIC )
        {
          hdrfetap_update_slot_time( &( hdrfetap.hdrfetap.statistics.connected_time ));
        }
        if ( hdrfetap.hdrfetap.state == HDRFETAP_IDLE )
        {
          hdrfetap_update_slot_time( &( hdrfetap.hdrfetap.statistics.idle_time ));
        }
        
        /* Convert slot time to CC time */

        hdrfetap.hdrfetap.statistics.cc_time = (hdrfetap.hdrfetap.statistics.connected_time
                       + hdrfetap.hdrfetap.statistics.idle_time) / 256; /*lint !e734 */
        
        /* Due to the 256-slot unit, we can be off by one after the division */    

        hdrfetap.hdrfetap.statistics.cc_time = 
          MAX( hdrfetap.hdrfetap.statistics.cc_time,
               hdrfetap.hdrfetap.statistics.first_sync_cc_pkt );

        /* Length of attribute is 5 bytes */

        ( void )dsmbit_pack8_tail( reply_ptr,
                           0x05,
                           attribute_offset,
                           8 );
        attribute_offset += 8;

        ( void )dsmbit_pack8_tail( reply_ptr,
                           HDRFETAP_ATTR_FIRST_SYNC_CC_PKT_STATS,
                           attribute_offset,
                           8 );
        attribute_offset += 8;

        ( void )dsmbit_pack16_tail( reply_ptr,
                            hdrfetap.hdrfetap.statistics.first_sync_cc_pkt,
                            attribute_offset,
                            16 );
        attribute_offset += 16;

        ( void )dsmbit_pack16_tail( reply_ptr,
                            hdrfetap.hdrfetap.statistics.cc_time,
                            attribute_offset,
                            16 );
        attribute_offset += 16;

        break;
    case HDRFETAP_ATTR_CONNECTED_SS_STATS_MC:

        /* Conn. time is supposed to be in slot accuracy, so update it here */

        if ( hdrfetap.hdrfetap.state == HDRFETAP_TRAFFIC )
        {
          hdrfetap_update_slot_time( &(hdrfetap.hdrfetap.statistics.connected_time) );
        }

        (void) hdrlmac_get_carrier_map( HDRIS890B_NUM_CARRIERS, carrier_map );
          /* Get carrier map to see which sub-active sets are valid */

        num_subactive_sets = 0;
          
        /* Update the serving sector change count */
        for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
        {
          /* Checking if carrier is valid tells use that sub-active set is 
          valid */
          if( carrier_map[sas_id].carrier_index != 
              HDRLMAC_INVALID_CARRIER_INDEX )
          {
            hdrfetap.hdrfetap.statistics.connected_ss_change_mc[sas_id] = 
             hdrlmac_get_handoff_count( sas_id );
              /* Get handoff count for this sub-activeset from LMAC */

            num_subactive_sets++;
              /* Increment sub-activeset count as carrier is valid */
          }
          else
          {
            break;
              /* Sub-activeset indices come in order so if invalid we're done */
          }
        }

        /* Length varies based on num sub-active sets. 3 bytes for beginning of 
        packet, 6 for each sub-activeset's worth of information included. */
        attribute_length = 3 + ( num_subactive_sets * 6 );

        /* Length of attribute is variable */

        ( void )dsmbit_pack8_tail( reply_ptr,
                                   attribute_length,
                                   attribute_offset,
                                   8 );
        attribute_offset += 8;

        ( void )dsmbit_pack8_tail( reply_ptr,
                                   HDRFETAP_ATTR_CONNECTED_SS_STATS_MC,
                                   attribute_offset,
                                   8 );
        attribute_offset += 8;


        /* Get the latest Traffic Channel Assignment Msg seq number */
        current_tca_seqno = hdrrup_get_current_tca_seq_num();

        ( void )dsmbit_pack8_tail( reply_ptr,
                                   current_tca_seqno,
                                   attribute_offset,
                                   8 );
        attribute_offset += 8;

        ( void )dsmbit_pack8_tail( reply_ptr,
                                   num_subactive_sets,
                                   attribute_offset,
                                   8 );
        attribute_offset += 8;

        /* Loop through sub-active set stats and pack (6-bytes per)*/
        for( sas_id=0; sas_id<num_subactive_sets; sas_id++ )
        {
          ( void )dsmbit_pack8_tail( reply_ptr,
                                   sas_id,
                                   attribute_offset,
                                   8 );
          attribute_offset += 8;

          ( void )dsmbit_pack16_tail( reply_ptr,
                          hdrfetap.hdrfetap.statistics.connected_ss_change_mc[sas_id],
                          attribute_offset,
                          16 );
          attribute_offset += 16;

          ( void )dsmbit_pack32_tail( reply_ptr,
                                      hdrfetap.hdrfetap.statistics.connected_time,
                                      attribute_offset,
                                      24 );
          attribute_offset += 24;

        }
        
        break;
      default:
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: unkwn attr. ID %d", attribute_id );
        break;
    }
  }

  /* Send message on RTC only as reliable */

  hdrhmp_app_send_msg_on_existing_rtc( 
                       hdrfetap.hdrfetap.configuration.protocol_id,
                       hdrfetap.hdrfetap.configuration.stream_id,
                       TRUE,
                       40,
                       HDRHAI_IN_USE,
                       reply_ptr,
                       NULL,
                       NULL );

  /* Let user know about this via QXDM */

  HDR_MSG_IS890_6 (MSG_LEGACY_HIGH,"+++++++++++ fetap Statistics +++++++++++++ "
                                   "+++ Idle ASP Changes:       %d "
                                   "+++ Idle Time:              %d "
                                   "+++ Connected SS Change:    %d "
                                   "+++ Connected Time:         %d "
                                   "+++ Sync. CC Cap. Count:    %d "
                                   "+++ ControlChannel Time:    %d ",
                                   hdrfetap.hdrfetap.statistics.idle_asp_change,
                                   hdrfetap.hdrfetap.statistics.idle_time,
                                   hdrfetap.hdrfetap.statistics.connected_ss_change,
                                   hdrfetap.hdrfetap.statistics.connected_time,
                                   hdrfetap.hdrfetap.statistics.first_sync_cc_pkt,
                                   hdrfetap.hdrfetap.statistics.cc_time);

  for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
  {
    HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  "+++ Connected SS Change[%d]: %d",
            sas_id, hdrfetap.hdrfetap.statistics.connected_ss_change_mc[sas_id]);
  }
} /* hdrfetap_process_stats_get_request */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRFETAP_PARAM_ASSIGN_CB

DESCRIPTION
   This is the callback function that SLP calls at the 2nd layer ack for the
   ParamaAssigmentcomplte msg sent to the Base Station 

DEPENDENCIES
  None.

PARAMETERS
  data_ptr    - pointer to the new configuration
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  If successful, will commit the configured attributes.
===========================================================================*/

static void hdrfetap_param_assign_cb
(
  void *data_ptr,
    /* the ptr to the new config */

  hdrerrno_enum_type  status
    /* the HLP status of the paramAssignmentComplete msg */
)
{
  hdrfetap_config_type      new_configuration;
  qword                     sys_frame_time;
  uint8                     rtcmac_flow_id;
  uint8                     ack_fixed_bit, ack_fixed_mod ;
  boolean                   reg_result = FALSE;
  uint16                    sys_time;
  boolean                   loopback_enabled = FALSE;
  uint16                    sas_id;
  hdrlmac_carrier_map_type  carrier_map[HDRIS890B_NUM_CARRIERS];
  uint8                     num_subactive_sets;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------
   * Wait until  we received the SLP ack before we make the new
   * parameters take effect. 
  ------------------------------------------------------------------*/

  if (status == E_SUCCESS)
  {
    HDR_MSG_IS890 (MSG_LEGACY_HIGH, "SLP ack received for Parameter Complete");
  }
  else
  {
    HDR_MSG_IS890 (MSG_LEGACY_HIGH, "ParameterComplete Transmission failed");
    return;
  }

  /*------------------------------------------------------------------
   * Update the FETAP_CALLED flag to indicate a connection with a valid
   * Paramber Assignment Msg 
  ------------------------------------------------------------------*/
  hdrfetap.hdrfetap.fetap_called = TRUE;

  /*------------------------------------------------------------------
   * The following is being done in  HDR RX task context 
  ------------------------------------------------------------------*/

  /*lint -e668 */

  /* Remove the lint error due to data_ptr always point
   * to hdrfetap.hdrfetap.stored_configuration */

#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy(&new_configuration,
          sizeof( hdrfetap_config_type ),
          data_ptr,
          sizeof( hdrfetap_config_type ) ); /*lint +e668 */
#else
  memcpy(&new_configuration,
         data_ptr,
         sizeof( hdrfetap_config_type ) ); /*lint +e668 */
#endif /* FEATURE_MEMCPY_REMOVAL */

  sys_time = hdrfetap_get_sys_time();
    /* Update the local system time */

  /*------------------------------------------------------------------
   * Initialize loopback for PL_0_1_2 
  ------------------------------------------------------------------*/
  
  if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_0 ||
       hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_1 || 
       hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 )
  {
  
    REX_DISABLE_PREMPTION();
  
    /* reset the debug loopback packet count */
    hdrfetap.hdrfetap.lo_pkt_cnt = 0;
  
    /* reset the loop back record buffer */
    hdrfetap_lo_buff_init( &hdrfetap.hdrfetap.lo, sys_time );
  
#ifdef HDRFETAP_LO_PKT_DBG
    hdrfetap.hdrfetap.lo_start_t = sys_time;
#endif
  
    REX_ENABLE_PREMPTION();
  
    HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: Lo rec buf initialized" );

    if( new_configuration.loopback_mode )
    {
      /* Set flag to enable loopback mode. */
      loopback_enabled = TRUE;
  
#ifdef HDRFETAP_LO_PKT_DBG
      HDR_MSG_IS890_1 (MSG_LEGACY_MED, "FETAP: LO start time %d", 
                       hdrfetap.hdrfetap.lo_start_t);
#endif /* HDRFETAP_LO_PKT_DBG */
    }
  }
  /*------------------------------------------------------------------
   * Initialize loopback for PL_3 (MC)
  ------------------------------------------------------------------*/

  else if( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {
    /* Loop through the sub-activeset config to see if loopback is enabled on 
    any of the carriers */
    for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
    {
      REX_DISABLE_PREMPTION();
  
      /* reset the debug loopback packet count */
        hdrfetap.hdrfetap.lo_pkt_cnt = 0;
    
      /* reset the loop back record buffer */
        hdrfetap_lo_buff_init( &hdrfetap.hdrfetap.lo_mc[sas_id], sys_time );
    
#ifdef HDRFETAP_LO_PKT_DBG
        hdrfetap.hdrfetap.lo_start_t = sys_time;
#endif /* HDRFETAP_LO_PKT_DBG */
    
      REX_ENABLE_PREMPTION();

      if( new_configuration.loopback_mode_mc[sas_id] == TRUE )
      {
        /* Set flag to enable loopback mode. */
        loopback_enabled = TRUE;

#ifdef HDRFETAP_LO_PKT_DBG
        HDR_MSG_IS890_2 (MSG_LEGACY_MED, "FETAP: LO[%d] start time %d", 
          sas_id,
          hdrfetap.hdrfetap.lo_start_t );
#endif /* HDRFETAP_LO_PKT_DBG */

        break;
      }
    }

    HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: Lo MC rec buf initialized" );
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: invalid subtype %d",
                     hdrfetap.hdrfetap.configuration.phy_subtype );
  }  

  /* Till here, all new configuration params are valid */

#ifdef FEATURE_MEMCPY_REMOVAL
  hdrfetap.hdrfetap.configuration = new_configuration;
#else
  memcpy( &( hdrfetap.hdrfetap.configuration ),
          &new_configuration, 
          sizeof( hdrfetap_config_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Deregister registered PCP transmission call backs */
  hdrfetap_clear_transmit_callbacks();

  if ( loopback_enabled == FALSE )
  {
#ifdef HDRFETAP_LO_PKT_DBG
    (void) hdrts_get_frame_time( sys_frame_time );
    HDR_MSG_IS890_2 (MSG_LEGACY_MED,  "FETAP: Lo end time %d (%d)", 
               (qw_lo( sys_frame_time ) & 0x00007fff),
               (qw_lo( sys_frame_time ) & 0x00007fff) - hdrfetap.hdrfetap.lo_start_t);
#endif /* HDRFETAP_LO_PKT_DBG */
  }               

  /* Reg trans callbacks to the PCP when loop back mode is enabled */

  if ( loopback_enabled )
  {
    switch ( hdrfetap.hdrfetap.configuration.phy_subtype )
    {
    case PHY_SUBTYPE_0:
    case PHY_SUBTYPE_1:
      rtcmac_flow_id = ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_0 )?
                       HDRPCP_RMAC0_FLOW_ID: HDRPCP_RMAC1_FLOW_ID ;
      break;
    case PHY_SUBTYPE_2:
    case PHY_SUBTYPE_3:
      rtcmac_flow_id=hdrpac_get_rtcmac_flow(hdrfetap.hdrfetap.configuration.stream_id,0);
        /* the rtcmac3/4 flow id through the PAC interface */
      break;
    default:
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  " FETAP: illegle PHY subtype" );
      return;
    }

    if ( rtcmac_flow_id != HDRPCP_INVALID_MAC_FLOW_ID )   
    {
      reg_result = hdrpcp_register_transmit_callbacks(
                     HDRFETAP_SUBSTREAM_ID,
                     rtcmac_flow_id,
                     hdrfetap.hdrfetap.configuration.stream_id,  /* buff_size_ptr */
                     hdrfetap_pcp_get_linkflow_buf_size,/* get_data_size */
                     NULL,                              /* hp_data_func_ptr */
                     hdrfetap_pcp_get_pkt,              /* lp_data_func_ptr */
                     hdrfetap_pcp_prev_pkt_status,
                     NULL,                              /* packet_fail_func_ptr */
                     TRUE,                              /* always_notify_packet_stats */
                     NULL );
    }
    
    if ( reg_result )
    {

      hdrfetap.hdrfetap.configuration.if_tx_rgsted = TRUE;
      hdrfetap.hdrfetap.configuration.flow_config.rtcmac_flow_id = rtcmac_flow_id;
        /* updated every time when a seccessful registration */
        
      HDR_MSG_IS890_1 ( MSG_LEGACY_HIGH,  
                        "FETAP: PCP registration successfull under PHYsubtype%d",
                        hdrfetap.hdrfetap.configuration.phy_subtype );

    } 
    else
    {

      hdrfetap.hdrfetap.configuration.if_tx_rgsted = FALSE ;
      HDR_MSG_IS890_1 ( MSG_LEGACY_HIGH,  
                        "FETAP: PCP registration fail to PHYsubtype%d",
                        hdrfetap.hdrfetap.configuration.phy_subtype );

    }

  }
  else /* if (loobackmode) */
  {
    /* No need to deregister the PCP callbacks here */
  }

  /* Set up the single-carrier test modes */
  if (  ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_0 ) ||
        ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_1 ) || 
        ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 ) )
  {
      
    if ( hdrfetap.hdrfetap.configuration.drc_value_fixed_mode )
    {
      HDR_MSG_IS890_1 ( MSG_LEGACY_HIGH,  "FETAP: Setting Fixed DRC Value to 0x%x", 
                        hdrfetap.hdrfetap.configuration.drc_value_fixed_mode_config );
  
      /* Write to FMAC to set the DRC value */
  
      (void)hdrfmac_set_drc_value(hdrfetap.hdrfetap.configuration.drc_value_fixed_mode_config);
    }
    else
    {
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FETAP: Setting DRC to variable mode");
  
      /* write to FMAC to set the varialbe mode */
  
      (void)hdrfmac_set_drc_value( HDRFMAC_VAR_RATE_DRC );
    }
  
    if ( hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode )
    {
  
      HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "FETAP: Setting Fixed DRC Cover to 0x%x", 
                       hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode_config );
  
      hdrfmac_set_drc_cover( hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode_config );
    }
    else
    {
  
      HDR_MSG_IS890 ( MSG_LEGACY_HIGH,  "FETAP: DRC cover fixed mode disabled " );
      hdrfmac_set_drc_cover( HDRFMAC_HANDOFF_CTRL_TO_DSP );
    }
  
    if ( hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode )
    {
      if ( hdrfetap.hdrfetap.configuration.ack_channel_modtype_fixed_mode &&
           ( hdrfetap.hdrfetap.configuration.phy_subtype != PHY_SUBTYPE_0 ))
      {
  
        ack_fixed_bit = hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode_config;
        ack_fixed_mod = hdrfetap.hdrfetap.configuration.ack_channel_modtype;
  
        hdrfmac_set_ack_mode(( ack_fixed_bit == ACFM_EARLY_TERMINATE_ALL_PKTS )? 
            (( ack_fixed_mod == ACKCH_MOD_TYPE_BPSK )? 
                IS890A_MODE_ACK_BPSK :     /* ACK && BPSK */
                IS890A_MODE_ACK_OOK ) :    /* ACK && OOK  */
            (( ack_fixed_mod == ACKCH_MOD_TYPE_BPSK )? 
                IS890A_MODE_NAK_BPSK:      /* NAK && BPSK */
                IS890A_MODE_NAK_OOK) );    /* NAK && OOK  */
      }
      else
      {
  
        /* IS890_MODE_0: first slot decode mode 
         * IS890_MODE_1: full packet decode mode */
  
        hdrfmac_set_ack_mode( 
          ( hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode_config 
                                                 == ACFM_EARLY_TERMINATE_NO_PKTS)?
          IS890_MODE_1 : IS890_MODE_0 );
  
      }
  
    }
    else
    {
  
      hdrfmac_set_ack_mode( IS856_COMPLIANT );
  
    }
  }
  /* Set up multi-carrier test modes */
  else if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
  {

    num_subactive_sets = 
      hdrlmac_get_carrier_map( HDRIS890B_NUM_CARRIERS, carrier_map );
      /* Map sub-activeset indices to carrier indices */

    /* Multi-carrier configuration values are per sub-active set */
    for( sas_id=0; sas_id<num_subactive_sets; sas_id++ )
    {
      if ( hdrfetap.hdrfetap.configuration.drc_value_fixed_mode_mc[sas_id] )
      {
        HDR_MSG_IS890_2 ( MSG_LEGACY_HIGH, "FETAP: SAS: %d DRC Value to 0x%x", 
                sas_id,
                hdrfetap.hdrfetap.configuration.drc_value_fixed_mode_config_mc[sas_id] );
  
        (void)hdrfmac_set_mc_drc_value(
          carrier_map[sas_id].carrier_index,
          hdrfetap.hdrfetap.configuration.drc_value_fixed_mode_config_mc[sas_id]);
      }
      else
      {
        HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FETAP: Setting DRC to variable mode");
    
        /* write to FMAC to set the varialbe mode */

        (void)hdrfmac_set_mc_drc_value(
          carrier_map[sas_id].carrier_index,
          HDRFMAC_VAR_RATE_DRC);
      }
  
      if ( hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode_mc[sas_id] )
      {
        HDR_MSG_IS890_2 ( MSG_LEGACY_HIGH, "FETAP: SAS: %d DRC Cover to 0x%x", 
                sas_id,
                hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode_config_mc[sas_id] );
  
        hdrfmac_set_mc_drc_cover(
          carrier_map[sas_id].carrier_index,
          hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode_config_mc[sas_id]);
      }
      else
      {
        HDR_MSG_IS890 ( MSG_LEGACY_HIGH,  "FETAP: SAS: %d DRC cover not fixed" );
    
        /* write to FMAC to set the varialbe mode */

        hdrfmac_set_mc_drc_cover(
          carrier_map[sas_id].carrier_index,
          HDRFMAC_HANDOFF_CTRL_TO_DSP );
      }
  
      if ( hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode_mc[sas_id] )
      {
        if ( hdrfetap.hdrfetap.configuration.ack_channel_modtype_fixed_mode_mc[sas_id] &&
             ( hdrfetap.hdrfetap.configuration.phy_subtype != PHY_SUBTYPE_0 ))
        {
    
          ack_fixed_bit = 
            hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode_config_mc[sas_id];
          ack_fixed_mod = hdrfetap.hdrfetap.configuration.ack_channel_modtype_mc[sas_id];
    
          hdrfmac_set_mc_ack_mode(
            carrier_map[sas_id].carrier_index,
            ( ack_fixed_bit == ACFM_EARLY_TERMINATE_ALL_PKTS )?
              (( ack_fixed_mod == ACKCH_MOD_TYPE_BPSK )? 
                  IS890A_MODE_ACK_BPSK :     /* ACK && BPSK */
                  IS890A_MODE_ACK_OOK ) :    /* ACK && OOK  */
              (( ack_fixed_mod == ACKCH_MOD_TYPE_BPSK )? 
                  IS890A_MODE_NAK_BPSK:      /* NAK && BPSK */
                  IS890A_MODE_NAK_OOK) );    /* NAK && OOK  */
        }
        else
        {
    
          /* IS890_MODE_0: first slot decode mode 
           * IS890_MODE_1: full packet decode mode */
    
          hdrfmac_set_mc_ack_mode(
            carrier_map[sas_id].carrier_index,
            ( hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode_config_mc[sas_id]
                == ACFM_EARLY_TERMINATE_NO_PKTS ) ? 
                  IS890_MODE_1 : IS890_MODE_0 );
    
        }
      }
      else
      {
        hdrfmac_set_mc_ack_mode( 
          carrier_map[sas_id].carrier_index, 
          IS856_COMPLIANT );
      }
    }
  }
  else
  {
    HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR, " FETAP: Invalid phy subtype = %d",
                      hdrfetap.hdrfetap.configuration.phy_subtype );
  }

#ifdef FEATURE_HDR_ETAP_DISABLE_CTA_TIMER
  HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FETAP: disable CTA timer!");
  (void) dsrlp_enable_inactivity_timer( DSRLP_FTAP_ID, FALSE );
    /* Disable the CTA timer at RLP */
#endif /* FEATURE_HDR_ETAP_DISABLE_CTA_TIMER */

  /* The TC OFS should be disabled for the MPS test */

  if ( !hdrfetap.hdrfetap.configuration.tc_ofs_enabled )
  {
    /* Update srch through LMAC to disable the TC OFS */
    hdrlmac_disable_tc_ofs( );

    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FTAP: Disable TC OFS for the TAP call!");
  }
  else
  {
    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FTAP: TC OFS not disabled! ");
  }

  hdrlmac_set_tap ( TRUE );
    /* Notify HDR srch that TAP is running */

  /* Output the config throught the QXDM */
  HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  "+++++++++++ FETAP Config +++++++++++++"
                                  "+++ Stream %d Phy Subtype %d",
                                  hdrfetap.hdrfetap.configuration.stream_id,
                                  hdrfetap.hdrfetap.configuration.phy_subtype);

  /* loopback mode */

  if ( hdrfetap.hdrfetap.configuration.loopback_mode )
    HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "+++ Loopback Mode enabled (pers. %d)",
                     hdrfetap.hdrfetap.configuration.loopback_mode_config);
  else
    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ Loopback Mode disabled");

  /* DRC value fixed mode */

  if ( hdrfetap.hdrfetap.configuration.drc_value_fixed_mode )
    HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "+++ Fixed DRC Value Mode (DRC 0x%x)", 
                     hdrfetap.hdrfetap.configuration.drc_value_fixed_mode_config);
  else
    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ Fixed DRC Value Mode disabled");

  /* DRC cover fixed mode */

  if ( hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode )
    HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "+++ Fixed DRC Cover Mode (Cover 0x%x)", 
                     hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode_config);
  else
    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ Fixed DRC Cover Mode disabled");

  /* ACK channel fixed mode */

  if ( hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode )
    HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "+++ Fixed Ack Channel Mode (Mode 0x%x)", 
                     hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode_config);
  else
    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ Fixed Ack Channel Mode disabled");

  /* ACK channel modtype fixed mode */

  if ( hdrfetap.hdrfetap.configuration.ack_channel_modtype_fixed_mode )
    HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "+++ Fixed Ack Channel Modtype (Mode 0x%x)", 
                     hdrfetap.hdrfetap.configuration.ack_channel_modtype);
  else
    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ Fixed Ack Channel Modtype disabled");

  /* Multi-carrier configuration values are per sub-active set */
  for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
  {
    /* loopback mode */

    if ( hdrfetap.hdrfetap.configuration.loopback_mode_mc[sas_id] )
      HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  
                       "+++ SAS %d: MC Loopback Mode enabled (pers. %d)",
                       sas_id,
                       hdrfetap.hdrfetap.configuration.loopback_mode_config_mc[sas_id]);
    else
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ MC Loopback Mode disabled");
  
    /* DRC value fixed mode */

    if ( hdrfetap.hdrfetap.configuration.drc_value_fixed_mode_mc[sas_id] )
      HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  
                 "+++ SAS %d: MC Fixed DRC Value Mode (DRC 0x%x)",
                 sas_id,
                 hdrfetap.hdrfetap.configuration.drc_value_fixed_mode_config_mc[sas_id]);
    else
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ MC Fixed DRC Value Mode disabled");
  
    /* DRC cover fixed mode */
  
    if ( hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode_mc[sas_id] )
      HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  
                 "+++ SAS %d: MC Fixed DRC Cover Mode (Cover 0x%x)",
                 sas_id,
                 hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode_config_mc[sas_id]);
    else
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ MC Fixed DRC Cover Mode disabled");
  
    /* ACK channel fixed mode */
  
    if ( hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode_mc[sas_id] )
      HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  
                 "+++ SAS %d: MC Fixed Ack Channel Mode (Mode 0x%x)", 
                 sas_id,
                 hdrfetap.hdrfetap.configuration.ack_channel_fixed_mode_config_mc[sas_id]);
    else
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ MC Fixed Ack Channel Mode disabled");
  
    /* ACK channel modtype fixed mode */
  
    if ( hdrfetap.hdrfetap.configuration.ack_channel_modtype_fixed_mode_mc[sas_id] )
      HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  
                 "+++ SAS %d: MC Fixed Ack Channel Modtype (Mode 0x%x)",
                 sas_id,
                 hdrfetap.hdrfetap.configuration.ack_channel_modtype_mc[sas_id]);
    else
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ MC Fixed Ack Channel Modtype disabled");
  }

}  /* hdrfetap_param_assign_cb */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRFETAP_PROCESS_IND()

DESCRIPTION
  This function processes the indications for the FETAP. 


DEPENDENCIES
  None
 
PARAMETERS
  ind_name      - Indication to process, is a global enum for all
                  possible indications in HDR signaling
  ind_data_ptr  - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_process_ind
(

  hdrind_ind_name_enum_type ind_name,
    /* Indication name */

  hdrind_ind_data_union_type *ind_data_ptr 
    /* Indication data */
)
{

  uint32  dummy;
  uint16  overflow;
  boolean loopback_enabled = FALSE;
  uint8   sas_id;

  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  switch( ind_name )
  {

    /* There is no break in the following 4 indications
     *   - HDRIND_ALMP_RETURNING_TO_IDLE,
     *   - HDRIND_INIT_NETWORK_ACQUIRED
     *   - HDRIND_ALMP_REACQUIRING
     *   - HDRIND_ALMP_EXIT_HDR_MODE
     * They all indicate that the AT is entering the IDLE state, either from 
     * the CONN stateor the ACQ state. Common to all these indications are
     * the all ETAP modes has to be reset to FALSE */

    case HDRIND_ALMP_RETURNING_TO_IDLE:

      hdrfetap_update_slot_time( &( hdrfetap.hdrfetap.statistics.connected_time ) );
        /* Closing the connection; Update the connected time */
      
      if ( hdrfetap.hdrfetap.state != HDRFETAP_IDLE )
      {
        if (hdrfetap.hdrfetap.expecting_idle_ind == TRUE)
        {
          hdrfetap.hdrfetap.expecting_idle_ind = FALSE;
            /* If this indication was expected from IDLE_DO indication that 
               was processed earlier, clear flag. We're done. */

        }
        else
        {
          hdrfetap.hdrfetap.expecting_idle_ind = TRUE;
            /* We are now expecting an idle_handoff indication, which 
               will not be counted. */

        }
      }

      /* No break */

    case HDRIND_INIT_NETWORK_ACQUIRED:
    case HDRIND_ALMP_REACQUIRING:

      hdrfmac_disable_flow_control( HDRFMAC_DRC_REQ_IS890 );
        /* The flow control needs the DSP to be running */
      
      /* No break */

    case HDRIND_ALMP_EXIT_HDR_MODE:

      /* No longer operating in the HDR mode */

#if defined( TIMETEST ) && defined( TIMETEST_DSRLP_STATS )
      #error code not present
#endif

      /* This is a critical section as the idle time gets *
       * updated by the PN roll ISR when in IDLE state.   */

      REX_DISABLE_PREMPTION();

      if ( hdrfetap.hdrfetap.state == HDRFETAP_NOT_ACQ )
      {
        /* If HDR is acquired for the first time,    *
         * register our PN roll interrupt processing */
        hdrts_register_roll_callback( hdrfetap_process_pn_roll_int );
        HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FETAP: State set to SYNC_TIME_WAIT");
        hdrfetap.hdrfetap.state = HDRFETAP_SYNC_TIME_WAIT;
      }
      else
      {
        if ( hdrfetap.hdrfetap.fetap_called == TRUE )
        {
          /* Reset the fetap DRC/ACK configuration if FETAP is active 
           * in the current connection */

          hdrfmac_set_ack_mode( IS856_COMPLIANT );
            /* Reset to the FMAC ack mode */

          (void) hdrfmac_set_drc_value( HDRFMAC_VAR_RATE_DRC );
            /* Reset the DRC variable rate mode */

          hdrfetap.hdrfetap.fetap_called = FALSE;
            /* reset the flag */

          HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FETAP: Reset DRC/ACK mode to FMAC");
        }

        hdrfetap.hdrfetap.state = HDRFETAP_IDLE;
          /* Update the fetap state */
        
        HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FETAP: State set to IDLE");
        
        hdrrmac_disable_throttling( HDRRMAC_FTAP_ID, FALSE );
          /* Enable the throttle through RMAC once FTAP idle */

#ifdef FEATURE_HDR_DLNA
        hdrcmac_force_dlna_disable( HDRCMAC_FTAP_ID, FALSE );
          /* Remove forced DLNA disable */
#endif /* FEATURE_HDR_DLNA */
        
        hdrfetap_update_slot_time( &dummy );
          /* Make sure we don't account for time spent searching or on IS2K */

#ifdef FEATURE_HDR_ETAP_DISABLE_CTA_TIMER
        HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "FETAP: Not disable CTA timer!");
        (void) dsrlp_enable_inactivity_timer( DSRLP_FTAP_ID, TRUE );
          /* Disable the CTA timer at RLP */
#endif /* FEATURE_HDR_ETAP_DISABLE_CTA_TIMER */
      }

      REX_ENABLE_PREMPTION();

      /* ---------------------------------------------------------------------
                      Mode Reset Once out of the connection state
       ---------------------------------------------------------------------*/

      /* Initialize configuration on connection close, check persistence flag.
      Will return whether loopback mode is still enabled. */

      loopback_enabled = 
        hdrfetap_configuration_initialization( &hdrfetap.hdrfetap.configuration, TRUE );
      
      if ( loopback_enabled == FALSE )
      {
        hdrfetap_clear_transmit_callbacks();
          /* Unregister the tx cbs if needed */
      }
      
      break;

    case HDRIND_IDLE_CONNECTION_OPENED:

      /* Enter the TRAFFIC state from the IDLE state */

      hdrfetap.hdrfetap.tst_pkt_cnt = 0;

      for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
      {
        hdrfetap.hdrfetap.last_link_id[ sas_id ] = HDRSRCH_NO_LINK_ID;
      }

      /* Set the DSP to control handoffs here to avoid writing 
       * to it when it's disabled.                             */

      if ( hdrfetap.hdrfetap.configuration.drc_cover_fixed_mode == FALSE )
      {
        hdrfmac_set_drc_cover( HDRFMAC_HANDOFF_CTRL_TO_DSP );
          /* handoff is controlled by DSP if not in DRC_COVER_FIX mode */
      }

      hdrfetap_update_slot_time( &( hdrfetap.hdrfetap.statistics.idle_time ));
        /* Update our idle time */

      if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_0 ||
           hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_1 ||
           hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
      {
        hdrfetap_lo_buff_init( &hdrfetap.hdrfetap.lo, hdrfetap_get_sys_time() );
          /* initiliize the loopback forward record buffer */

        HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: Lo rec buf initialized" );

      }
      else if ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 )
      {
        for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
        {
          hdrfetap_lo_buff_init( 
            &hdrfetap.hdrfetap.lo_mc[sas_id], 
            hdrfetap_get_sys_time() );
        }

        HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: Lo MC rec buf initialized" );

      }
      else
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: Invalid Subtype. (%d)", 
                         hdrfetap.hdrfetap.configuration.phy_subtype );
      }

      /* Since we know for sure that HDR has been active 
       * at this time, go ahead and register our PN roll    
       * interrupt processing. We need to register here in 
       * order to handle race conditions where we may       
       * transition to TRAFFIC state without going through  
       * SYNC_WAIT state. This is to ensure that the PN roll 
       * interrupt does get registered for if it wasn't     
       * previously registered for.                          */

      hdrts_register_roll_callback( hdrfetap_process_pn_roll_int );

      hdrfetap.hdrfetap.state = HDRFETAP_TRAFFIC;
        /* move the state to the TRAFFIC */

      HDR_MSG_IS890 ( MSG_LEGACY_HIGH,  "FETAP: State set to TRAFFIC" );

#if defined(TIMETEST) && defined(TIMETEST_DSRLP_STATS)
      #error code not present
#endif
      break;

    /* Statistics collection related indications: */

    case HDRIND_LMAC_IDLE_HANDOFF:

      if ( hdrfetap.hdrfetap.state == HDRFETAP_IDLE )
      {
        /* Only increment idle handoff count if AT has not closed the
           connection (ie, not expecting an IDLE HO indication). 
           There is an automatic IDLE_HANDOFF indication 
           on every connection close that should not be counted. */
        if (hdrfetap.hdrfetap.expecting_idle_ind == TRUE)
        {

          hdrfetap.hdrfetap.expecting_idle_ind = FALSE;
            /* We were expecting this indication.  
               Skip increment of idle handoff and reset flag. */
        }
        else
        {
          hdrfetap.hdrfetap.statistics.idle_asp_change++;

          HDR_MSG_IS890_1 ( MSG_LEGACY_HIGH, "FETAP: Idle HO Count: %d", 
                           hdrfetap.hdrfetap.statistics.idle_asp_change  );
        }
      }
      else
      {
        
        hdrfetap.hdrfetap.expecting_idle_ind = TRUE;
          /* If we get an IDLE HO indication while in traffic, we expect
             a RETURN_TO_IDLE indication.  Mark it so. */

      }

      break;

    case HDRIND_CCMAC_SCC_PKT_RXED:
      /* Error catching */
      ASSERT(ind_data_ptr != NULL);

      /* Check for last packet and FETAP state */
      if ( (ind_data_ptr->intra_scc_info.last_pkt) && 
           ( hdrfetap.hdrfetap.state != HDRFETAP_INACTIVE ) )
      {
        /* this condition check is redundent because the ind_cb has just
         * Checked before the indication processing */

        overflow = hdrfetap.hdrfetap.statistics.first_sync_cc_pkt & 0x8000;
          /* Take the bit 16 as the overflow bit */

        hdrfetap.hdrfetap.statistics.first_sync_cc_pkt++;
          /* Increase the CC sync counter */

        hdrfetap.hdrfetap.statistics.first_sync_cc_pkt |= overflow;
          /* OR to the rememberred overflow bit */
      }

      break;

    case HDRIND_LMAC_CARRIER_MAP_CHANGED:

      if( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_3 )
      {
        HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: Carrier Map Changed " );
    
        /* Initialize configuration and statistics when carrier map changes, 
        check persistence flag for loopback mode. */
    
        ( void ) hdrfetap_configuration_initialization( &hdrfetap.hdrfetap.configuration, 
                                                     TRUE );
    
        for( sas_id=0; sas_id<HDRIS890B_NUM_CARRIERS; sas_id++ )
        {
          hdrfetap_lo_buff_init( 
            &hdrfetap.hdrfetap.lo_mc[sas_id], 
            hdrfetap_get_sys_time() );
        }

        HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: Lo MC rec buf initialized" );

        hdrfetap_statistics_initialization();
      }
        
      break;

    default:

      HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: Unkwn ind %d ", ind_name );

      break;

  }

} /* hdrfetap_process_ind */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_PROCESS_PARAMETER_ASSIGNMENT()

DESCRIPTION
  This function processes the fetap parameter assignement message.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr   - Pointer to DSM item containing message.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrfetap_process_parameter_assignment
(
  dsm_item_type**  msg_ptr 
    /* The ptr to the DSM item containing the ParameterAssignment msg */
)
{
  hdrfetap_config_type  new_configuration;
  dsm_item_type*        reply_ptr = NULL;
  uint8                 seqno;
  uint8                 attribute_len;
  uint8                 attribute_id;
  uint8                 attribute_data;
  uint8                 tca_seq;
  uint8                 num_subactive_sets;
  uint8                 sas_id;
  uint8                 msg_index;
  uint16                total_bits_remaining;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  total_bits_remaining = 
    ( dsm_length_packet( (*msg_ptr) ) * HDRFETAP_BITS_PER_BYTE );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   * Sanity Check
   *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   
  /* Get and check the validity of this message */

  if ( dsm_pullup( msg_ptr, &seqno, 1 ) != 1 )
  {
    HDR_MSG_IS890_3( MSG_LEGACY_ERROR, "FETAP: Cannot read seqno", 0, 0, 0 );
    return;
  }

  /* We unpacked transaction ID already. Decrement the bits remaining */
  total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

  if ((!hdrfetap.hdrfetap.configuration.param_assign_seqno_is_valid) ||
      ( HDRIS890A_SEQNO_IS_VALID( seqno,
                                 hdrfetap.hdrfetap.configuration.param_assign_seqno )))
  {
    hdrfetap.hdrfetap.configuration.param_assign_seqno_is_valid = TRUE;
    hdrfetap.hdrfetap.configuration.param_assign_seqno = seqno;
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: Bogus ParamAssn. (%d)", seqno );
    return;
  }

  HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "FETAP: Rcvd ParamAss. (%d)", seqno);

  /* Reverse link throttling is disabled for FETAP test*/
  hdrrmac_disable_throttling(HDRRMAC_FTAP_ID, TRUE);

#ifdef FEATURE_HDR_DLNA
  /* Disable DLNA for FETAP test */
  hdrcmac_force_dlna_disable( HDRCMAC_FTAP_ID, TRUE );
#endif /* FEATURE_HDR_DLNA */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   * read the attributes to the new_configuration
   *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* why need store the current one into the new_configuration we do need 
   * the memcpy because the stream_id/tst appsubtype are stored */

#ifdef FEATURE_MEMCPY_REMOVAL
  new_configuration = hdrfetap.hdrfetap.configuration;
#else
  memcpy( &new_configuration, 
          &( hdrfetap.hdrfetap.configuration ), 
          sizeof( hdrfetap_config_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Missing a attribute means the corresponding mode is OFF */

  (void) hdrfetap_configuration_initialization( &new_configuration, FALSE );
    /* Initialize configuration values, force loopback mode to FALSE */
  
  /* new mode to the ETAP: ack channel modulation type fixed mode
   * selection b/w BPSK and OOK; it has to be set the same time 
   * as the FixedBitMode  */

  new_configuration.ack_channel_modtype_fixed_mode   = FALSE;

  /* Loop through the attributes present in the message */

  while ( ( total_bits_remaining >= HDRFETAP_BITS_PER_BYTE ) &&
          ( dsm_pullup( msg_ptr, &attribute_len, 1 ) == 1 ) )
  {

    /* Decrement the bits remaining */
    total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

    if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
        ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_id, 1) != E_SUCCESS ) )
    {
      HDR_MSG_IS890( MSG_LEGACY_HIGH, "FETAP: No more attributes found" ); 
      return; 
    }

    /* Decrement the bits remaining */
    total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

    switch( attribute_id )
    {
      case HDRFETAP_ATTR_DRC_VALUE_FIXED_MODE:

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        /* Sanity check on the DRC value 2 new DRC value is added here  
         * HDRHAI_DRC_1536     = 0x0d
         * HDRHAI_DRC_3072     = 0x0e         
         * we might need add the phy subtype conbination here        */

        if (( attribute_data <= HDRHAI_MAX_REL_0_DRC ) ||
            (( attribute_data <= HDRHAI_DRC_3072 ) &&
             ( hdrfetap.hdrfetap.configuration.phy_subtype == PHY_SUBTYPE_2 ))) 
        {
          new_configuration.drc_value_fixed_mode = TRUE;
          new_configuration.drc_value_fixed_mode_config = 
            (hdrhai_drc_value_enum_type) attribute_data;
        }
        else
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: bogus DRC value %d", 
                           attribute_data );
          return;
        }
        break;
      
      case HDRFETAP_ATTR_DRC_COVER_FIXED_MODE:

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attribute %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;
        
        /* NULL DRC cover is a special case */
        if ( attribute_data == 0 )
        {
          new_configuration.drc_cover_fixed_mode = TRUE;
          new_configuration.drc_cover_fixed_mode_config = 0;
        }
        /* Set value from attribute, fmac_set_drc_cover will error check */
        else
        {
          new_configuration.drc_cover_fixed_mode = TRUE;
          new_configuration.drc_cover_fixed_mode_config = attribute_data; 
        }
        
        break;
      
      case HDRFETAP_ATTR_ACK_CHN_BIT_FIXED_MODE:

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {

          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attribute %d", attribute_id );
          return; 

        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        /* Sanity check on the mode value */

        if ( attribute_data < ACFM_RESERVED )
        {
          new_configuration.ack_channel_fixed_mode = TRUE;
          new_configuration.ack_channel_fixed_mode_config = 
            ( attribute_data ? ACFM_EARLY_TERMINATE_NO_PKTS
                             : ACFM_EARLY_TERMINATE_ALL_PKTS );
        }
        else
        {

          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: bogus acfm mode value %d", attribute_data );
          return;

        }
        break;
      
      case HDRFETAP_ATTR_LOOPBACK_MODE:

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {

          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: Cannot read attr %d", attribute_id );
          return; 

        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        /* Sanity check on the mode value */

        if ( attribute_data < LOM_RESERVED )
        {
          /* Set the mode */

          new_configuration.loopback_mode = TRUE;
          new_configuration.loopback_mode_config = 
                     ( attribute_data )? LOM_PERSISTENT : LOM_NON_PERSISTENT ;

        }
        else
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: bogus lo mode value %d", attribute_data );
          return;
        }
        break;

      case HDRFETAP_ATTR_ACK_CHN_MODTYPE_FIXED_MODE :

        /* new attribute ACK channel modulation type fixed mode */

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attribute %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        /* Sanity check for the PHY subtype ; has to be 2 */

        if ( hdrfetap.hdrfetap.configuration.phy_subtype != PHY_SUBTYPE_2 ) 
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                         "FETAP: AckChanModTypeFixedMode not at ftype2" );
          return;
        }

        /* Sanity check on the modtype value */

        if ( attribute_data < ACKCH_MOD_RESERVED )
        {
          /* Set the modtype*/

          new_configuration.ack_channel_modtype_fixed_mode = TRUE;
          new_configuration.ack_channel_modtype            = 
            (hdrfetap_ack_channel_mod_enum_type)attribute_data;
        }
        else
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: bogus ack channel modtype value %d", 
                           attribute_data );
          return;
        }
        break;

    case HDRFETAP_ATTR_DRC_VALUE_FIXED_MODE_MC:

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, tca_seq, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, num_subactive_sets, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      for( msg_index=0; msg_index < num_subactive_sets; msg_index++ )
      {
        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, sas_id, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        if( sas_id >= HDRIS890B_NUM_CARRIERS )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Invalid sub-active set %d", sas_id );
          return;
        }

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        if ( attribute_data <= HDRHAI_MAX_REL_B_DRC )
        {
          new_configuration.drc_value_fixed_mode_mc[sas_id] = TRUE;
          new_configuration.drc_value_fixed_mode_config_mc[sas_id] = 
            (hdrhai_drc_value_enum_type) attribute_data;
        }
        else
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: bogus DRC value %d", 
                           attribute_data );
          return;
        }
      }
      
      break;

    case HDRFETAP_ATTR_DRC_COVER_FIXED_MODE_MC:

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, tca_seq, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, num_subactive_sets, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      for( msg_index=0; msg_index<num_subactive_sets; msg_index++ )
      {
        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, sas_id, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        if( sas_id >= HDRIS890B_NUM_CARRIERS )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Invalid sub-active set %d", sas_id );
          return;
        }

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        /* NULL DRC cover is a special case */
        if ( attribute_data == 0 )
        {
          new_configuration.drc_cover_fixed_mode_mc[sas_id] = TRUE;
          new_configuration.drc_cover_fixed_mode_config_mc[sas_id] = 0;
        }
        /* Set value from attribute, fmac_set_drc_cover will error check */
        else
        {
          new_configuration.drc_cover_fixed_mode_mc[sas_id] = TRUE;
          new_configuration.drc_cover_fixed_mode_config_mc[sas_id] = 
            attribute_data;
        }
      }

      break;

    case HDRFETAP_ATTR_LOOPBACK_MODE_MC:

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, tca_seq, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, num_subactive_sets, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      for( msg_index=0; msg_index<num_subactive_sets; msg_index++ )
      {
        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, sas_id, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        if( sas_id >= HDRIS890B_NUM_CARRIERS )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Invalid sub-active set %d", sas_id );
          return;
        }

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) ||
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        /* Sanity check on the mode value */
  
        if ( attribute_data < LOM_RESERVED )
        {
          /* Set the mode */
  
          new_configuration.loopback_mode_mc[sas_id] = TRUE;
          new_configuration.loopback_mode_config_mc[sas_id] = 
                     ( attribute_data )? LOM_PERSISTENT : LOM_NON_PERSISTENT ;
        }
        else
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: bogus lo mode value %d", attribute_data );
          return;
        }
      }
      break;

    case HDRFETAP_ATTR_ACK_CHN_BIT_FIXED_MODE_MC:

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, tca_seq, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) ||  
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, num_subactive_sets, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      for( msg_index=0; msg_index<num_subactive_sets; msg_index++ )
      {
        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, sas_id, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        if( sas_id >= HDRIS890B_NUM_CARRIERS )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Invalid sub-active set %d", sas_id );
          return;
        }

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        /* Sanity check on the mode value */

        if ( attribute_data < ACFM_RESERVED )
        {
          new_configuration.ack_channel_fixed_mode_mc[sas_id] = TRUE;
          new_configuration.ack_channel_fixed_mode_config_mc[sas_id] = 
            ( attribute_data ? ACFM_EARLY_TERMINATE_NO_PKTS
                             : ACFM_EARLY_TERMINATE_ALL_PKTS );
        }
        else
        {
  
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: bogus acfm mode value %d", attribute_data );
          return;
  
        }
      }

      break;

    case HDRFETAP_ATTR_ACK_CHN_MODTYPE_FIXED_MODE_MC :

      /* new attribute ACK channel modulation type fixed mode */

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, tca_seq, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
          ( HDRFETAP_GET_MSG_ATTR( msg_ptr, num_subactive_sets, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Cannot read attr %d", attribute_id );
        return; 
      }

      /* Decrement the bits remaining */
      total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

      for( msg_index=0; msg_index<num_subactive_sets; msg_index++ )
      {
        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, sas_id, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        if( sas_id >= HDRIS890B_NUM_CARRIERS )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: Invalid sub-active set %d", sas_id );
          return;
        }

        if( ( total_bits_remaining < HDRFETAP_BITS_PER_BYTE ) || 
            ( HDRFETAP_GET_MSG_ATTR( msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: Cannot read attr %d", attribute_id );
          return; 
        }

        /* Decrement the bits remaining */
        total_bits_remaining -= HDRFETAP_BITS_PER_BYTE;

        /* Sanity check for the PHY subtype ; has to be 3 */

        if ( hdrfetap.hdrfetap.configuration.phy_subtype != PHY_SUBTYPE_3 ) 
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                         "FETAP: AckChanModTypeFixedMode not at ftype3" );
          return;
        }
  
        /* Sanity check on the modtype value */
  
        if ( attribute_data < ACKCH_MOD_RESERVED )
        {
          /* Set the modtype*/
  
          new_configuration.ack_channel_modtype_fixed_mode_mc[sas_id] = TRUE;
          new_configuration.ack_channel_modtype_mc[sas_id]            = 
            (hdrfetap_ack_channel_mod_enum_type)attribute_data;
        }
        else
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "FETAP: bogus ack channel modtype value %d", 
                           attribute_data );
          return;
        }
      }
      
      break;

    default:

        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "FETAP: unkwn attr. ID %d", attribute_id );
        return;
    }
  }

  /* Mode conbination check; if the ACKChannelFixedModulationType is
   * enalbed, check if the FixedBitMode is on */

  if ( new_configuration.ack_channel_modtype_fixed_mode )
  {

    if ( !new_configuration.ack_channel_fixed_mode ) 
    {

      HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                     "FETAP: bad ack channel fixed mode config " );
      return;

    }

  }
  /* Configuration is successful, send the CompleteMsg on TC only reliably */

  reply_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
  if ( reply_ptr == NULL )
  {

    ERR_FATAL( "FETAP: Out of DSM items", 0, 0, 0 );

  }

  (void)dsmbit_pack8_tail( reply_ptr,
                           HDRFETAP_MSG_PARAMETER_COMPLETE,
                           0,
                           8 );

  (void)dsmbit_pack8_tail( reply_ptr,
                           seqno,
                           8,
                           8 );

 /*------------------------------------------------------------------
  * Wait until  we received the SLP ack before we make the new
  * parameters take effect. so stored the parsed configuration
  * to be used on receiving SLP ack.
 ------------------------------------------------------------------*/

#ifdef FEATURE_MEMCPY_REMOVAL
  hdrfetap.hdrfetap.stored_configuration = new_configuration;
#else
  memcpy(&( hdrfetap.hdrfetap.stored_configuration ),
         &new_configuration,
         sizeof( hdrfetap_config_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Send the msg to hmp with a call back on the slp_ack */

  hdrhmp_app_send_msg( hdrfetap.hdrfetap.configuration.protocol_id,
                       hdrfetap.hdrfetap.configuration.stream_id,
                       HDRHAI_RTC,
                       TRUE,
                       40,
                       HDRHAI_IN_USE,
                       reply_ptr,
                       hdrfetap_param_assign_cb,
                       &( hdrfetap.hdrfetap.stored_configuration ));

} /* hdrfetap_process_parameter_assignment */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_PROCESS_MSG()

DESCRIPTION
  This function handles received fetap messages.

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrfetap_process_msg( void )
{
  uint8  msg_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  while ( hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.read_index] != NULL )
  {

    if ( dsm_pullup( 
           &(hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.read_index]),
           &msg_id, 
           1 ) 
         != 1 )
    {

      HDR_MSG_IS890( MSG_LEGACY_ERROR, "FETAP: Empty message" );
      return;

    }
    
    switch( msg_id )
    {
      case HDRFETAP_MSG_PARAMETER_ASSIGNMENT:

        hdrfetap_process_parameter_assignment( 
          &(hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.read_index]) );
        break;
      
      case HDRFETAP_MSG_STATS_CLEAR_REQUEST:

        hdrfetap_process_stats_clear_request( 
          &(hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.read_index]) );
        break;
        
      case HDRFETAP_MSG_STATS_GET_REQUEST:

        hdrfetap_process_stats_get_request( 
          &(hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.read_index]) );
        break;
        
      default:

        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: unknown msg_id %d", msg_id );
        break;
    }
    
    /* Upon any parsing error the processing functions return,
     * so make sure we free any bogus received data.     */ 

    if ( hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.read_index] )
    {
      dsm_free_packet( 
        &(hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.read_index]) );
    }

    /* Advance the read index */

    hdrfetap.hdrfetap.msg_queue.read_index++;
    hdrfetap.hdrfetap.msg_queue.read_index %= HDRFETAP_MSG_QUEUE_SIZE;

  } /* while */

} /* hdrfetap_process_msg */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_PROCESS_SIG()

DESCRIPTION
  This function is the signal handler for the fetap.

DEPENDENCIES
  None
 
PARAMETERS
  signal - Pending REX signal.
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_process_sig
(
  rex_sigs_type signal
    /* Signal pending */
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  switch( signal )
  {

    case HDRFETAP_MSG_SIG:

      /* msg pending in the msg queue */

      hdrfetap_process_msg();
      break;

    default:

      HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "FETAP: unkwn sig %d", signal );
      break;
  }
} /* hdrfetap_process_sig */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_DEACTIVATE()

DESCRIPTION
  This function deactivates the FETAP.
  About the scenario of deregistration the tx call backs

  The TX call back is usually to register or deregister in the
  parmaeter assignment msg;

  The function is only called in the is890a_deactivate(). 
  is890a_deactivate() is only called when stream layer is de-mapping

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_deactivate( void )
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  hdrfetap.hdrfetap.state = HDRFETAP_INACTIVE;
    /* changed to deactive state */

#if defined(TIMETEST) && defined(TIMETEST_DSRLP_STATS)
#error code not present
#endif

#ifdef FEATURE_HDR_DSMREUSE_OPT

  HDRDSM_REUSE_DATA_PATH_ITEMS_CLEANUP();
    /* Clean up the Reusable DSM items */

#endif

} /* hdrfetap_deactivate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_IS_IN_TRAFFIC()

DESCRIPTION
  This function returns TRUE if the fetap is negotiated on
  any stream and in traffic state.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE or FALSE
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdrfetap_is_in_traffic( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  return ( hdrfetap.hdrfetap.state == HDRFETAP_TRAFFIC );

} /* hdrfetap_is_in_traffic */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_IND_CB()

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to the fetap.

DEPENDENCIES
  None
 
PARAMETERS
  ind_name      - Indication to process, is a global enum for all
                  possible indications in HDR signaling
  ind_data_ptr  - Optional data pointer for indication specific data
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
    /* Indication to process */

  void                      *ind_data_ptr
    /* Pointer to data for indication */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( hdrfetap.hdrfetap.state != HDRFETAP_INACTIVE )
  {

    /* This protocol runs in the RX and TX tasks. To make sure we update  
     * fetap before any signaling data is sent, we process the indications 
     * in the TX task context, which has the highest priority.            */

    if ( rex_self() == HDRTX_TASK_PTR )
    {
      /* Indication is from another protocol in the TX task, so consider  
       * it as an internal indication.                                    */
      hdrfetap_process_ind( ind_name, 
        (hdrind_ind_data_union_type *) ind_data_ptr );
    }
    else
    {
      hdrtx_queue_ind( HDRHAI_IS890A_FETAP,
                       ind_name, 
                       ind_data_ptr );
    }

  }

} /* hdrfetap_ind_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_MSG_CB()

DESCRIPTION
  This function is called by hdris890a_msg_cb when it received an 
  fetap message.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrfetap_msg_cb
(
  dsm_item_type* msg_ptr
    /* the ptr to the DSM item containing the incomign msg */
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.write_index] )
  {
    /* The msg_ptr has been checked in the hdris890a_msg_cb */

    HDR_MSG_IS890_3(  MSG_LEGACY_ERROR, "FETAP: Stale message ID 0x%x in %d",
         *(msg_ptr->data_ptr),
         hdrfetap.hdrfetap.msg_queue.write_index, 0 );

    dsm_free_packet( 
      &(hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.write_index]) );
  }

  hdrfetap.hdrfetap.msg_queue.msg_ptr[hdrfetap.hdrfetap.msg_queue.write_index] = msg_ptr;

  /* Advance the write index and check if there's a stale message left */

  hdrfetap.hdrfetap.msg_queue.write_index++;
  hdrfetap.hdrfetap.msg_queue.write_index %= HDRFETAP_MSG_QUEUE_SIZE;

  ( void )rex_set_sigs( HDRTX_TASK_PTR, HDRFETAP_MSG_SIG );

} /* hdrfetap_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFETAP_DISABLE_TC_OFS

DESCRIPTION
  The function modified the tc_ofs_enable based on the dip switch set. 

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void  hdrfetap_disable_tc_ofs
(
  boolean tc_ofs_disable
)
{
  hdrfetap.hdrfetap.configuration.tc_ofs_enabled = !tc_ofs_disable;

} /* void  hdrfetap_disable_tc_ofs */

#endif    /* FEATURE_HDR_IS890 || FEATURE_HDR_IS890A*/
