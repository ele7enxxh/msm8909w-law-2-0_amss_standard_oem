/*===========================================================================

            H D R   E N H A N C E D  R E V   T E S T   A P P L I C A T I O N


GENERAL DESCRIPTION
  This file contains the defintiions for the HDR reverse link
  enhanced test application (RETAP).

EXTERNALIZED FUNCTIONS

  hdrretap_activate()   - One time initialization of all RETAP data structures
  hdrretap_deactivate() - One time initialization of all RETAP data structures
  hdrretap_is_in_traffic()  - If RETAP in traffic state
  hdrretap_msg_cb()         - Callback registered with HMP to deliver messages
  hdrretap_ind_cb()         - Callback registered for delivery of indications
  hdrretap_process_sig()    - RETAP signal processing from the running task
  hdrretap_put_pkt()        - Incoming pkt callback function from PCP
  hdrretap_disable_tc_ofs() - i/f with IS890 for the dip switch set/reset
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/COMMON/vcs/hdrretap.c_v   1.17   09 Apr 2003 11:23:30   maldinge  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/tap/src/hdrretap.c#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/05/14   sat     Fix to retrieve correct carrier ID from channel Idx while processing 
                   parameter assignment.
12/19/13   sat     Changed memcpy() to assignment or memscpy() (safer version).
10/29/13   sat     Reduced F3 messages (or/and) ASSERTs
08/19/13   rmg     Added support to disable DLNA in TAP mode.
05/15/12   kss     Fixed bad ASSERT in hdrretap_stats_get_from_carrier_id().
04/04/12   grl     Fixed the carrier index to rl index mapping.
05/24/11   ssu     Included "hdrpac_api.h" and "dsrlp_api.h" instead. 
03/16/12   arm     Resolve TCB errors for Dime. 
05/24/11   ssu     Included "hdrpac_api.h" and "dsrlp_api.h".
05/24/11   ssu     Removed inclusion of "hdrpac_mdm.h".
04/27/11   kss     Added RF include to remove warnings. 
03/24/11   kss     Updated RF SVDO API support.
11/09/10   cnx     Added sanity check before casting to an enum value.  
08/06/10   cnx     Fixed Klocwork warnings.
08/05/10   rmg     Shifted call to disable throttling from parameter assignment
                   complete callback to parameter assignment processing.
07/20/10   rmg     Fixed crash in pl_23_pcp_pkt_status during two carrier call.
06/21/10   cnx     Fixed compiler warnings.
11/11/09   etv     Fixed Lint warning.
10/27/09   wsh     Fixed compiler warning for ARM compiler
10/21/09   rmg     Fixed incorrect treating of StatsRecordID=1 as error in 
                   RTAPStatsClearRequest message.
10/08/09   etv     Added ASSERT with right condition to fix KW error.
09/28/09   etv     Fixed Klocwork errors.
08/27/09   wsh     CMI:Split HDRIS890 due to external reference in OEMCM.c  
07/06/09   rmg     Restrict number of RETAP makeup pkts to pkt buffer size.
06/23/09   rmg     Clear prev_frame_time in param assignment processing when
                   test pkt mode is enabled.
05/27/09   rkc     Added FEATURE_RF_SVDO_API changes.
05/20/09   kss     Include hdrpac_mdm for moved hdrpac_get_rtcmac_flow().
05/06/09   rmg     Fixed disabling throttling in multicarrier mode.
04/10/09   etv     Clear prev_frame_time after empty parameter assign msg.
                   Handle error cases in ParamAssign msg processing code.
                   Used hdrhmp_app_send_msg instead of existing_rtc API.
04/08/09   rmg     Fixed RTAP fixed to variable packet size transition.
01/21/09   tej/kss Fixed RTAP fill packet creation.
12/10/08   smd     Replaced assert.h with amssassert.h
11/18/08   rmg     Fixed bug in programming specified per-carrier data rates.
10/13/08   kss     Lint fixes.
09/02/08   rmg     Fixed rmac_flow_id value assignment for variable rate mode.
06/16/08   kss     Increment target rate based on packet accept/cancel.
06/09/08   etv     Sort SubASETIdx based on frequency.
06/07/08   kss     Fix issue with target rate updating in configured rate.
05/14/08   etv     Void the return value from hdrlmac_get_carrier_map.
02/08/08   cws     Fixed bug with seqno size from uint16 to uint32.
01/10/08   cws     CARRIER_MAP_CHANGED should only affect subtype 3.
12/21/07   cws     Added handling of HDRIND_LMAC_CARRIER_MAP_CHANGED.
12/14/07   cws     Fix PL0 test packet timestamp during silence.
11/26/07   cws     Renamed PL specific functions for consistentency.
11/21/07   cws     Moved RMAC0 calls to RTAP.
11/20/07   cws     Moved RMAC3 calls to RTAP.
11/06/07   cws     Added RRI 0 stats collection.
11/02/07   cws     Fix for PL_3 StatisticsRecordID.
10/30/07   cws     Multi-carrier statistics now use RLChannelIndex.
10/29/07   cws     Added MC PCP inteface for prev_pkt_status.
10/02/07   jyw     Added FEATURE_HDR_ETAP_DISABLE_CTA_TIMER and lint fixes.
09/12/07   cws     Added RMCTAP changes.
08/23/07   cws     Added FEATURE_COMMON_PCP_RMAC0 code.
08/07/07   cws     Cleaned up types for consistency. 
08/01/07   grl     Fixed RCVT 2.2 compiler warnings.
07/25/07   hal     Updated interfaces to compile with RevB PCP
07/25/07   cc      Used new hdrpcp_register_transmit_callbacks interface.
08/14/06   etv     Cleaned up improper DSM use in hdrretap_gen_pl_01_test_pkt.
06/15/06   jyw     Added the PCP unregistration at the connection close.
06/15/06   jyw     Used the unpack16 routine to retrieve uint16 from dsm item.
06/15/06   jyw     Added FEATURE_HDR_REVA_L1.
06/08/06   mpa     Included "hdrrmac.h" for RVCT compiler.
05/22/06   ljl     Notified HDR srch when TAP is running.
05/03/06   jyw     Removed the lint suppression.
04/25/06   jyw     Replaced the ERR with IS890 error message.
03/18/06   jyw     Readjusted the rate throttling control points.
03/20/06   jyw     Ajusted AMAC interface to disable the fixed EACH rate mode.
03/15/06   jyw     Disabled the reverse link rate throttling for RTAP calls.
03/10/06   jyw     Fixed the logic in the message processing.
03/06/06   jyw     Increased the seq number to uint32 when PL2.
02/28/06   jyw     Fixed PCP interface to support larger fmt A packets.
02/16/06   jyw     Added IS890/LMAC interface to disable TC OFS in TAP calls.
02/09/06   jyw     Removed the function predefinitions.
02/06/06   jyw     Consolidated the RETAP global variables.
12/16/05   jyw     Fixed the bug in using the qw_equ.
11/22/05   jyw     Changed a message to LEGACY_LOW to save QXDM bandwith.
11/17/05   jyw     Fixed the lint errors and added some editorial changes.
11/02/05   jyw     Included "assert.h"
10/28/05   jyw     Fixed the lint errors.
10/26/05   jyw     Fixed the bug in the flow config sanity check.
10/12/05   jyw     Added support to the Anritsu 8820 test box.
09/27/05   jyw     Fixed the bugs in the RETAP buf management and clean up.
09/08/05   jyw     Replaced TAP0 with ETAP.
09/08/05   jyw     General clean up, more comments.
08/02/05   jyw     Installed the RTCMAC/AMAC interfaces.
07/26/05   jyw     Cleaned the interface.
05/18/05   jyw     created the file

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "target.h"

#ifdef FEATURE_MEMCPY_REMOVAL
#include "stringl.h"
#endif /* FEATURE_MEMCPY_REMOVAL */

#if defined ( FEATURE_HDR_IS890 ) || defined ( FEATURE_HDR_IS890A )

#include "rex.h"

#include "dsm.h"
#include "dsmbit.h"
#include "task.h"
#include "hdrdebug.h"
#include "err.h"
#include "qw.h"
#ifdef FEATURE_RF_SVDO_API
#include "rfm_cdma.h"
#else
#include "rf.h"
#include "rf1x_core_rf.h"
#endif /* FEATURE_RF_SVDO_API */

#include "hdrhai.h"
#include "hdrind.h"
#include "hdrerrno.h"
#include "hdrtx.h"
#include "hdrts.h"
#include "hdrhmp.h"
#include "hdrpcp.h"

#include "hdris890.h"
#include "hdris890_v.h"
#include "hdrrtap.h"

#include "hdrutil.h"
#include "hdrscp.h"
#include "hdrpac_api.h"
#include "dsrlp_api.h"
#include "hdramac.h"
#include "hdrcmac.h"
#include "hdrrmac.h"
#include "hdrlmac.h"

#include "amssassert.h"
#include "hdrbit.h"

#ifdef FEATURE_HDR_ETAP_DISABLE_CTA_TIMER
#include "dsrlp.h"
#endif /* FEATURE_HDR_ETAP_DISABLE_CTA_TIMRE */
/*===========================================================================

                     DEFINITIONS  AND   MACROS

===========================================================================*/
#define HDRHAI_IS890A_RETAP HDRHAI_IS890_RTAP
  /* share the TAP0 signals */

#define HDRRETAP_PL_23_TEST_PKT_MIN_SIZE_BYTE     10
  /* RETAP PL2_3 test packet minmum size in byte */

#define HDRRETAP_PL_23_TEST_PKT_HEADER_LEN        10
  /* RETAP Pl2_3 test packet header length in byte */

#define HDRRETAP_PL_2_TEST_PKT_RESERVE_BITS       7
  /* RETAP Pl2 test packet reserve bit length */

#define HDRRETAP_PL_3_TEST_PKT_RESERVE_BITS       3
  /* RETAP Pl3 test packet reserve bit length */

#define HDRRETAP_PL_2_TEST_PKT_TYPE               2
  /* RETAP Pl3 test packet PacketType */

#define HDRRETAP_PL_3_TEST_PKT_TYPE               3
  /* RETAP Pl3 test packet PacketType */

#define HDRRETAP_PL_01_TEST_PKT_MIN_SIZE_BYTE     12
  /* RETAP PL01 test packet minmum size in byte */

#define HDRRETAP_NUM_REV_PAYLOAD_SIZE             13
  /* Number of payload size for PL2 */

#define HDRRETAP_MIN_REV_PAYLOAD_SIZE_INDEX       1
  /* Minumum payload size index for PL2 */

#define HDRRETAP_MAX_REV_PAYLOAD_SIZE_INDEX       12
  /* Max payload size index for PL2 */

#define MAX_NUM_RETAP_FLOW                        8 
  /* the max limition for the reverse tx flow */

#define HDRRETAP_TEST_PKT_BUFFER_SIZE             16
  /* Only used for PL_01 */

#define HDRRETAP_MSG_SIG    HDRTX_IS890_RTAP_MSG_SIG
  /* Share the message signal with TAP0 */

#define HDRRETAP_NUM_REV_RATES_PL_01              6  
  /* Number of reverse link rate for the PL01 */

#define HDRRETAP_MIN_REV_RATE_PL_01               1  
  /* Minimum rate/number of rates to send test packet for PHY subtype 01 */

#define HDRRETAP_MSG_QUEUE_SIZE                   4
 /* RETAP message processing queue (size = 4) */ 

#define HDRRETAP_PNFILL_BUF_LENGTH                4096
  /* The PN roll length in byte 2^15/8 = 4*1024 */

#define HDRRETAP_DEFAULT_SUBSTREAM_ID             0
  /* default substream id for the PL01 */

#define HDRRETAP_PL2_STATS_RESPONSE_LENGTH        0x3B
  /* Length of PL2 StatsGetResponse message */

#define HDRRETAP_PL3_STATS_RESPONSE_HEADER_LENGTH 0x09
  /* Length of PL3 StatsGetResponse header */

#define HDRRETAP_PL3_STATS_LENGTH                 0x34
  /* Length of PL3 StatsGetResponse stats */

#define HDRRETAP_INVALID_RL_CHANNEL_INDEX         0xFF
  /* Marker for invalid RL channel index */

#define HDRRETAP_DEFAULT_RL_CHANNEL_INDEX         0
  /* Default rl channel index for single carrier */

#define HDRRETAP_DEFAULT_CARRIER                  0
  /* Default carrier ID for single carrier */

#define HDRRETAP_PL_2_STATISTICS_RECORD_ID        0x00
  /* PL_2 StatisticsRecordID */

#define HDRRETAP_PL_3_STATISTICS_RECORD_ID        0x01
  /* PL_3 StatisticsRecordID */

#define HDRRETAP_BITS_PER_BYTE                    8
  /* Number of bits per byte */

/* Macros used in the message parsing */

#define HDRRETAP_GET_ATTR(msg_ptr, attr, num_bytes )                  \
           ( hdrerrno_enum_type )                                     \
           (( dsm_pullup( msg_ptr, &attr, num_bytes) != num_bytes ) ? \
             E_NO_DATA: E_SUCCESS)

#define HDRRETAP_SANITY_CHECK_PHY_SUBTYPE(subtype)             \
          ( hdrerrno_enum_type )                               \
          (( hdrretap.configuration.phy_subtype != subtype ) ? \
            E_FAILURE : E_SUCCESS )

#define HDRRETAP_SANITY_CHECK_NOT_PHY_SUBTYPE(subtype)         \
          ( hdrerrno_enum_type )                               \
          (( hdrretap.configuration.phy_subtype == subtype ) ? \
            E_FAILURE : E_SUCCESS )

/*===========================================================================

                  INTERNAL DATA DECLARATIONS
                     and DATA STRUCTURES 

===========================================================================*/


/* Physical layer protocol subtype */
/* this decides the RETAP operation mode */
typedef enum
{
  PHY_SUBTYPE_0 = 0x0,
  PHY_SUBTYPE_1,
  PHY_SUBTYPE_2,
  PHY_SUBTYPE_3,
  PHY_SUBTYPE_INVALID
} 
hdrretap_phy_subtype_enum_type;

typedef enum 
{
  EACH_RATE_9_6_KBPS ,
  EACH_RATE_19_2_KBPS,
  EACH_RATE_38_4_KBPS,
  EACH_RATE_INVALID
} 
hdrretap_each_rate_enum_type;

typedef enum
{
  TPM_NON_PERSISTENT = 0x0,
  TPM_PERSISTENT,
  TPM_RESERVED
} 
hdrretap_test_pkt_mode_config_enum_type;

/* Structure contains the configurable values and storage for a single instance
of PacketRateMode, PacketPayloadSizeMode and PacketPayloadSizeModeMC attributes
as defined in RMCTAP */
typedef struct
{
  uint8   min_index;
    /* MinRateIndex for PL_0_1 or MinPayloadSizeIndex for PL_2_3 */

  uint8   max_index;
    /* MaxRateIndex for PL_0_1 or MaxPayloadSizeIndex for PL_2_3 */

  uint8   tgt_index;
    /* TargetRate for current transmission */

  boolean configured;
    /* Indicates that this instance of rate control is configured (enabled) */
} 
hdrretap_pkt_size_mode_type;

typedef struct 
{
  uint8 min_rate;
  uint8 max_rate;
} 
hdrretap_pkt_rate_mode_config_type;

/* User data type per flow */

typedef struct
{
  uint8 table_index;
    /* The index to the flow config table */

  uint32 prev_pkt_size[HDRIS890B_NUM_CARRIERS]; 
    /* The pkt size for this flow in the previouse rtcmac pkt on the indexed 
    * carrier. It may or may not be accepted by the PHY in the last subframe. */

} 
hdrretap_user_data_type;

/* Mode configuration for PHY subtype_01 */

typedef struct
{ 
  boolean                             configured_pkt_rate_mode;
    /* configured packet rate mode */

  hdrretap_pkt_rate_mode_config_type  pkt_rate_mode_config;
    /* min and max rate */

  uint8                               current_rate;
    /* current rate */

  boolean                             if_tx_rgsted;
    /* indicates if any TX callback has been registerred */

  hdrretap_user_data_type             user_data;
    /* User data for PL01 */

} hdrretap_pl_01_config_type;
    

/* Common flow configuration type */

typedef struct
{
  uint8                   substream_id;
    /* flow id is determined by the AN during session ... */

  uint8                   rtcmac_flow_id;
    /* to which rtcmac_flow the stream is bound to */


  boolean                 if_tx_rgsted;
    /* is registerred to PCP transmission */

  boolean                 burst_period_mode;
  uint16                  burst_period; 
    /* burst period mode config */
  
  uint32                  elaps_time;
    /* the time is increased by slot count in the PCP 
     * get_data_size func call */

  boolean                 burst_size_mode;
  uint16                  init_burst_size;
    /* burst size mode */

  uint32                  current_buf_size;
    /* current buffer size in byte */

  hdrretap_user_data_type user_data;
    /* user data attached to this flow */
} hdrretap_link_flow_config_type;

/* Mode configuration for PHY subtype_2_3 */

typedef struct
{
  /* RETAPStatsClearRequst seqno. */
  uint8                                   stats_clear_seqno;
  boolean                                 stats_clear_is_valid; 
    

  /* RETAPStatsRequst seqno. */
  uint8                                   stats_get_seqno;
  boolean                                 stats_get_is_valid; 
    
  /* PayloadSizeMode configuration */
  hdrretap_pkt_size_mode_type             payload_size_mode_config;

  /* flow configurations */
  uint8                                   num_flows;
  hdrretap_link_flow_config_type          flow_config [MAX_NUM_RETAP_FLOW];
    
  /* PacketPayloadSizeModeMC record information Array of configs per 
  Carrier ID, indexed by Carrier ID */
  hdrretap_pkt_size_mode_type  
    payload_size_mode_config_mc [HDRIS890B_NUM_CARRIERS];

} hdrretap_pl_23_config_type;


/* RETAP configuration structure */

typedef struct
{
  /* the stream id assigned to the is890a */
  hdrhai_stream_enum_type                 stream_id;

  /* Protocol ID based off of the stream ID */
  hdrhai_protocol_name_enum_type          protocol_id;

  /* transaction ID of the last parameter assignment message */
  uint8                                   param_assign_seqno;
  boolean                                 param_assign_seqno_is_valid;

  /* test packet mode */
  boolean                                 test_pkt_mode;

  /* Test packet mode configuration storage */
  hdrretap_test_pkt_mode_config_enum_type test_pkt_mode_config;

  /* Application subtype */
  uint16                                  app_subtype;

  /* PHY protocol subtype */
  /* this decides the RETAP operation mode */
  hdrretap_phy_subtype_enum_type          phy_subtype;

  /* Configuration info for PL_0_1 */
  hdrretap_pl_01_config_type              config_pl_01;

  /* Configuration info for PL_2_3 */
  hdrretap_pl_23_config_type              config_pl_23;

  /* REACH rate configured mode */
  boolean                                 each_rate_mode;
  hdrretap_each_rate_enum_type            each_rate;

  /* If tc_ofs enabled; default is FALSE */
  boolean                                 tc_ofs_enabled;

} hdrretap_config_type;


typedef enum
{ 
  BP_MODE = 0x1,
  BS_MODE
} 
hdrretap_burst_param_mode_enum_type;

/* RETAP State the state machin is rolled by the ALMP indications */

typedef enum
{
  HDRRETAP_INACTIVE = 0,
  HDRRETAP_IDLE,
  HDRRETAP_TRAFFIC
} hdrretap_state_enum_type;


/* Common statistics type */

typedef struct {
  uint16  pkt_size;               /* Maximum packet size (r/o)        */
  uint32  seqno;                  /* Current sequence number (r/w)    */
  uint16  rate;                   /* Rate for debug output (r/o)      */
  uint8   rate_dec;               /* Rate for debug output (r/o)      */
  uint32  pkt_count;              /* 32-bit packet count for PL_01, 
                                   * PL_01 seqno is 12-bit            */
} hdrretap_stats_type; 

/* MSG buf type for the RETAP */

typedef struct
{
  dsm_item_type*  msg_ptr[HDRRETAP_MSG_QUEUE_SIZE];
  uint8           read_index;
  uint8           write_index;
} hdrretap_msg_buf_type;

/* RETAP PL01 test packet buffer type */

typedef struct
{
  uint8           wr_index;
  uint8           rd_index;
  boolean         overflow;
  dsm_item_type*  pkt_ptr[HDRRETAP_TEST_PKT_BUFFER_SIZE];
} hdrretap_pl_01_test_pkt_buf_type;

/* PN fill circ buffer, using for the PL_2 packet only */

typedef struct
{
  boolean if_initialized;
  uint8 pnfill[ HDRRETAP_PNFILL_BUF_LENGTH ];
} hdrretap_pnfill_buf_type;

/*===========================================================================

                   CONSTANTS

===========================================================================*/

/* Reverse rate table acc. to IS-856 (fields see above). For the 153.8
 * rate the actual maximum packet size is 509 bytes, but PCP can only
 * accommodate up to 255 bytes in one packet.   */

const hdrretap_stats_type 
  hdrretap_pl_01_stats_const[HDRRETAP_NUM_REV_RATES_PL_01] = 
{
  {   0,  0x0000,    0,  0,  0 },      /* HDR_REV_RATE_NULL   (0)      */
  {  32,  0x0000,    9,  6,  0 },      /* HDR_REV_RATE_9_6    (1)      */
  {  64,  0x0000,   19,  2,  0 },      /* HDR_REV_RATE_19_2   (2)      */
  { 128,  0x0000,   38,  4,  0 },      /* HDR_REV_RATE_38_4   (3)      */
  { 256,  0x0000,   76,  8,  0 },      /* HDR_REV_RATE_76_8   (4)      */
  { 512,  0x0000,  153,  6,  0 }       /* HDR_REV_RATE_153_6  (5)      */
};

const hdrretap_stats_type 
  hdrretap_pl_2_stats_const[HDRRETAP_NUM_REV_PAYLOAD_SIZE] = 
{
  {    0,  0x0000,   0,  0,  0 },
  {   96,  0x0000,  19,  2,  0 },
  {  224,  0x0000,  38,  4,  0 },
  {  480,  0x0000,  76,  8,  0 },
  {  736,  0x0000, 115,  2,  0 },
  {  992,  0x0000, 153,  6,  0 },
  { 1504,  0x0000, 230,  4,  0 },
  { 2016,  0x0000, 307,  2,  0 },
  { 3040,  0x0000, 460,  8,  0 },
  { 4064,  0x0000, 614,  4,  0 },
  { 6112,  0x0000, 921,  6,  0 },
  { 8160,  0x0000,1228,  8,  0 },
  {12256,  0x0000,1843,  2,  0 }
};

/* One carrier's worth of initialization */
const hdrretap_stats_type 
  hdrretap_pl_3_stats_const[HDRRETAP_NUM_REV_PAYLOAD_SIZE] =
{
  {    0,  0x0000,   0,  0,  0 },
  {   96,  0x0000,  19,  2,  0 },
  {  224,  0x0000,  38,  4,  0 },
  {  480,  0x0000,  76,  8,  0 },
  {  736,  0x0000, 115,  2,  0 },
  {  992,  0x0000, 153,  6,  0 },
  { 1504,  0x0000, 230,  4,  0 },
  { 2016,  0x0000, 307,  2,  0 },
  { 3040,  0x0000, 460,  8,  0 },
  { 4064,  0x0000, 614,  4,  0 },
  { 6112,  0x0000, 921,  6,  0 },
  { 8160,  0x0000,1228,  8,  0 },
  {12256,  0x0000,1843,  2,  0 }
};

/*===========================================================================

                   LOCAL DATA DEFINITIONS

===========================================================================*/

static struct
{
  hdrretap_config_type             configuration;
    /* Configuation */
  
  hdrretap_config_type             stored_configuration;
    /* Stored configuration for the new configruation */

  hdrretap_state_enum_type         state;
    /* RETAP State Machine */

  qword                            pl_23_stats_revsystime;
    /* reverse system time */

  hdrretap_msg_buf_type            msg_queue;
    /* RETAP Message buf */

  dsm_item_type*                   fill_pkt_ptr; 
    /* Fill packet for the PL_01 */

  hdrretap_pl_01_test_pkt_buf_type pl_01_test_pkt_buf;
    /* Info about generated RETAP PL_01 test packets */

  hdrretap_pnfill_buf_type         pnfill_buf;
    /* PN fill circ buffer, used for the PL_2 packet only */

  hdrretap_stats_type              pl_01_stats[HDRRETAP_NUM_REV_RATES_PL_01]; 
    /* statistics for pl_01 */

  hdrretap_stats_type              pl_2_stats[HDRRETAP_NUM_REV_PAYLOAD_SIZE];
    /* statistics for the pl_2 */

  hdrretap_stats_type              
    pl_3_stats[HDRIS890B_NUM_CARRIERS][HDRRETAP_NUM_REV_PAYLOAD_SIZE];
    /* statistics for the pl_3, indexed by RLChannelIndex based from TCA  */

  boolean                          hdrretap_called;
    /* A flag to indicate if the retap is called in the current connection
     * this is used to solve the possible configuration collistion b/w the
     * the AT cmds and ETAP */

  uint8                           
    carrier_to_rl_channel_idx[HDRIS890B_NUM_CARRIERS];

  uint16                          tca_seq_for_map;
    /* TCA sequence number for carrier_to_rl_channel_idx */

  uint8                           num_rl_carriers;
    /* */

  qword                           pl_01_prev_frame_time;
    /* Frame time last packet status was called */

} hdrretap;


/*===========================================================================

                      FUNCTION DEFINITIONS

===========================================================================*/

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_CLEAR_TRANSMIT_CALLBACKS

DESCRIPTION
  This function unregisters RETAP from PCP. 

DEPENDENCIES
  None
 
PARAMETERS
  None
   
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
static void hdrretap_clear_transmit_callbacks( void )
{

  uint8   i; 
    /* loop index */
   
  boolean if_unreged = TRUE;
    /* registration result */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------
   * PCP clean up, deregister all the transmission call backs to the 
   * PCP  if retapPktMode persistence bit is not set.
   * for the fsubtype01, the substream_id is zero; 
   * for the fsubtype2, the unregistreation has to be per substream 
  ------------------------------------------------------------------*/

  /* unregister the previouse tx call backs to the PCP*/

  if((( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 ) ||
      ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_1 ))&&
     ( hdrretap.configuration.config_pl_01.if_tx_rgsted ))

  {

    if_unreged = 
         hdrpcp_unregister_transmit_callbacks( hdrretap.configuration.stream_id,
                                               HDRRETAP_DEFAULT_SUBSTREAM_ID );
    if ( !if_unreged ) 
    {
      HDR_MSG_IS890( MSG_LEGACY_ERROR, "RETAP: cant unreg TX CBs !" );
    }
    else
    {
      hdrretap.configuration.config_pl_01.if_tx_rgsted = FALSE;
      HDR_MSG_IS890( MSG_LEGACY_HIGH, "RETAP: unreg TX CBs under ftype01!" );
    }
  }
  else if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ||
           hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {

    for ( i=0; i < hdrretap.configuration.config_pl_23.num_flows; i++ ) 
    {

      /* if the flow has been rgsted; it is unusual that some flow did 
       * not registered */

      if( hdrretap.configuration.config_pl_23.flow_config[i].if_tx_rgsted )
      {

        if_unreged = 
          hdrpcp_unregister_transmit_callbacks( hdrretap.configuration.stream_id, 
            hdrretap.configuration.config_pl_23.flow_config[i].substream_id );
          /* unrgsted the callbacks */

        if ( !if_unreged ) 
        {
          HDR_MSG_IS890_1( 
            MSG_LEGACY_ERROR, 
            "RETAP: cant unreg TX CBs for flow id%d",
            hdrretap.configuration.config_pl_23.flow_config[i].substream_id);
        }
        else
        {
          HDR_MSG_IS890_1( MSG_LEGACY_HIGH, 
            "RETAP: unreg TX CBs for flow id%d entering idle",
            hdrretap.configuration.config_pl_23.flow_config[i].substream_id);

          hdrretap.configuration.config_pl_23.flow_config[i].if_tx_rgsted
                                                                          = FALSE;
        }
      }  /* if_unreged  */
    }  /* for */
  }  /* else if */
} /* hdrretap_clear_transmit_callbacks( void ) */


/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_SANITYCHECK_PKT_RATE_MODE_PARAMS

DESCRIPTION
  Validates the packet rate mode parameters.
  1. Does the PHY subtype allow the packet rate mode?
  2. Is the max value and the min value in the allowed rage?

DEPENDENCIES
  None.

PARAMETERS
  cfg_ptr   - pointer to the new configuration

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hdrretap_sanitycheck_pkt_rate_mode_params
(
  hdrretap_config_type *cfg_ptr
    /* pointer to the new configuration */
)
{

  uint8 min_rate, max_rate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cfg_ptr != NULL );
    /* Sanity check the cfg_ptr */

  /* Sanity check the min rate */

  min_rate = cfg_ptr->config_pl_01.pkt_rate_mode_config.min_rate;
  if ( min_rate > HDR_REV_PL01_RATE_153_6 )
  {
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR,
      "RETAP: Rcvd bogus min. pkt rate value %d", min_rate );
    return FALSE;
  }

  /* Sanity check the max rate */

  max_rate = cfg_ptr->config_pl_01.pkt_rate_mode_config.max_rate;

  if ( max_rate > HDR_REV_PL01_RATE_153_6 ) 
  {
    HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR,
      "RETAP: Rcvd bogus max. pkt rate value %d", max_rate );
    return FALSE;
  }

  /* Sanity check the min < max */

  if( min_rate > max_rate )
  {
    HDR_MSG_IS890 (MSG_LEGACY_ERROR,
      "RETAP: Invalid range for pkt rate mode" );
    return FALSE; 
  }

  /* Till here, the pkt rate mode config is valid */

  return TRUE;

} /* static boolean hdrretap_sanitycheck_pkt_rate_mode_params */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_SANITYCHECK_PAYLOAD_SIZE_MODE_PARAMS

DESCRIPTION
  Validates the payload size mode parameters by making sure the configured 
  values are in the allowed rage.

DEPENDENCIES
  None.

PARAMETERS
  cfg_ptr - pointor to the RETAP configuration

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hdrretap_pkt_size_mode_check
(
  hdrretap_pkt_size_mode_type  *cfg_ptr
    /* Pointer to payload size configuration for PL_2 or PL_3 */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cfg_ptr != NULL );
    /* sanity check the cfg_ptr */

  /* Sanity check the min rate */

  if ( cfg_ptr->min_index > HDR_REV_PL2_PLSIZE_12256 )
  {
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR,
      "RETAP: Rcvd bogus min. payload size value %d", cfg_ptr->min_index );
    return FALSE;
  }

  /* Sanity check the max rate */

  if ( cfg_ptr->max_index > HDR_REV_PL2_PLSIZE_12256 ) 
  {
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR,
      "RETAP: Rcvd bogus max. pkt rate value %d", cfg_ptr->max_index );
    return FALSE;
  }

  /* Sanity check the min < max */

  if ( cfg_ptr->min_index > cfg_ptr->max_index )
  {
    HDR_MSG_IS890 (MSG_LEGACY_ERROR,
      "RETAP: Invalid range for pkt rate mode" );
    return FALSE; 
  }

  /* Till here, the pkt rate mode config is valid */
  return TRUE;

} /* static boolean hdrretap_sanitycheck_payload_size_mode_params */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_ADD_TX_FLOW_PARAM

DESCRIPTION
  The function is used to add either a Burst Period Mode parameter or a 
  Burst Size Mode Parameter into the flow table of the RETAP configurations.

DEPENDENCIES
  None.

PARAMETERS
  uint8 substream_id

  uint16 burstperiod
    0x0000    0 slots
    0x0001    600 slots
    0x0002    1,200 slots
    0x0003    1,800 slots
    ...
    0xfffe    39,320,400 slots 
    0xffff    infinite  ( one time burst )

  uint8 mode 
    0x00      BP mode ( Burst Period Mode )
    0x01      BS mode ( Burst Size Mode )

RETURN VALUE
  uint8 flow_table_index
    the index into the flow_table inside the RETAP configuration

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hdrretap_add_tx_flow_param
(
  uint8                                 substream_id,
    /* Substream id */

  uint16                                burst_param,
    /* The param for either the burst size mode or the burst period mode */

  uint8                                 mode,
    /* Either the burst size mode or the burst period mode */

  hdrretap_pl_23_config_type            *f23_cfg_ptr 
    /* Pointer to configuration structure */
)
{

  uint8 i, num_flows;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check the config pointor */
  ASSERT( f23_cfg_ptr != NULL );

  /* Sanity check if the substream id is in the valid range */

  if ( substream_id == HDRPCP_INVALID_ID )
  {
    HDR_MSG_IS890 (MSG_LEGACY_ERROR, "RETAP: invalid substream id" );
    return FALSE;
  }

  num_flows = f23_cfg_ptr->num_flows;
    /* get the number of flows */

  /* Loop through flow table to find the index for this flow */

  for ( i=0; i < f23_cfg_ptr->num_flows; i++ ) 
  {
    if ( f23_cfg_ptr->flow_config[i].substream_id == substream_id ) 
    {
      break;
    }
  }

  /* A new flow is added */

  if ( i == num_flows ) 
  {
    /* Sanity check the number of flows if a new flow comes*/
    if (( num_flows + 1 ) > MAX_NUM_RETAP_FLOW ) 
    {
      HDR_MSG_IS890 (MSG_LEGACY_ERROR,
        "RETAP: can not add a new flow to the flow table" );
      return FALSE;
    }

    f23_cfg_ptr->num_flows ++ ;
      /* increment the number of flows */

    f23_cfg_ptr->flow_config[i].substream_id = substream_id;
      /* update the substream id */

    f23_cfg_ptr->flow_config[i].user_data.table_index = i;
      /* update the table index */
  }


  if ( mode == BP_MODE ) 
  {
    /* Burst period mode */

    if ( f23_cfg_ptr->flow_config[i].burst_period_mode)
    {
      HDR_MSG_IS890 (MSG_LEGACY_ERROR,
          "RETAP: redundent parameter in BurstPeriod Mode" );
    }

    /* init the burst period mode */
    f23_cfg_ptr->flow_config[i].burst_period_mode = TRUE;
    f23_cfg_ptr->flow_config[i].burst_period      = burst_param;
  }
  else if ( mode == BS_MODE ) 
  {
    /* Burst size mode */

    if ( f23_cfg_ptr->flow_config[i].burst_size_mode)
    {
       HDR_MSG_IS890 (MSG_LEGACY_ERROR,
           "RETAP: redundent parameter in BurstPeriod Mode" );
    }

    /* init the burst size mode */
    f23_cfg_ptr->flow_config[i].burst_size_mode   = TRUE;
    f23_cfg_ptr->flow_config[i].init_burst_size   = burst_param;
    f23_cfg_ptr->flow_config[i].current_buf_size  = burst_param * 1000 ;
  }

  /* if goes here, the flow param is valid */
  HDR_MSG_IS890_2 ( MSG_LEGACY_HIGH, 
                    "RETAP: a burst attribute for flow%d is added to idx%d",
                    substream_id, i );

  return TRUE;

} /* hdrretap_add_tx_flow_param */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_SANITYCHECK_EACH_RATE_PARAMS

DESCRIPTION
  Validates the enhanced access channel rate mode parameters.
  1. Does the PHY subtype allow the packet rate mode?
  2. Are the max value and min value in the allowed rage?

DEPENDENCIES
  None.

PARAMETERS
  cfg_ptr   - pointer to the new configuration
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hdrretap_sanitycheck_each_rate_mode_params
(
  hdrretap_config_type *cfg_ptr
    /* pointer to the new configuration */
)
{

  hdrretap_each_rate_enum_type  each_rate;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cfg_ptr != NULL );
    /* Sanity check the cfg_ptr */

  /* sanity check if the each rate mode matches to the fsubtype */

  if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 )  
  {
    HDR_MSG_IS890 (MSG_LEGACY_ERROR,
      "RETAP: Invalid subtype for EACH mode" ); 
    return FALSE; 
  }

  /* sanity check ; only 3 rates are valid */

  each_rate = cfg_ptr->each_rate ;
  if (each_rate >= EACH_RATE_INVALID) 
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "RETAP: Invalid reverse REACH rate " );
    return FALSE;
  }

  return TRUE;

} /* static boolean hdrretap_sanitycheck_each_rate_mode_params */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_SANITYCHECK_FLOW_CONFIG 

DESCRIPTION
  Checks if the flow config in the parameter assignment message is valid.
  1. Checks flow ID.
  2. For each flow, checks if parameters are valid.

DEPENDENCIES
  None.

PARAMETERS

RETURN VALUE
  boolean value indicating if the flow config valid or not
SIDE EFFECTS
  None.
===========================================================================*/
static boolean hdrretap_sanitycheck_flow_config
(

  hdrretap_config_type  * cfg_ptr 
    /* RETAP configuration pointor */

)
{

  uint8 i;
  uint8 num_flows;
  uint8 mac_flow_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( cfg_ptr != NULL );
    /* Sanity check */

  /* PHY subtype sanity check again*/

  if ( cfg_ptr->phy_subtype != PHY_SUBTYPE_2 && 
       cfg_ptr->phy_subtype != PHY_SUBTYPE_3 ) 
  {
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR, "RETAP: PHYsubtype mismatch flow config",
                     cfg_ptr->phy_subtype );
    return FALSE;
  }

  /* if the flow burst parameters are not complete
   * 1. either burst_size_mode or burst_period mode is set but not both
   * 2. if neither are set 
   * 3. no rtcmac flow is found for the substream_id */

  num_flows = cfg_ptr->config_pl_23.num_flows;
  if ( num_flows == 0) 
  {
    HDR_MSG_IS890 (MSG_LEGACY_ERROR, "RETAP: No flow defined in ParaAssMsg" );
    return FALSE;
  }

  for ( i=0; i < num_flows; i++ ) 
  {
    /* burst_size_mode and burst_period_mode have to be both set */

    if ( !( cfg_ptr->config_pl_23.flow_config[i].burst_size_mode ) || 
         !( cfg_ptr->config_pl_23.flow_config[i].burst_period_mode )) 
    {
      HDR_MSG_IS890_1 (MSG_LEGACY_ERROR,
         "RETAP: flow %d does not have complete burst params",
         cfg_ptr->config_pl_23.flow_config[i].substream_id );
      return FALSE;
    }

    /* check if the substream has a valid mac_flow_id to bound to */

    mac_flow_id = hdrpac_get_rtcmac_flow (
         cfg_ptr->stream_id,
         cfg_ptr->config_pl_23.flow_config[i].substream_id);

    if ( mac_flow_id == HDRPCP_INVALID_MAC_FLOW_ID ) 
    {
      HDR_MSG_IS890_1 (MSG_LEGACY_ERROR,
         " RETAP: invalid substream id %d in the flow config",
         cfg_ptr->config_pl_23.flow_config[i].substream_id );
      return FALSE;
    }

    cfg_ptr->config_pl_23.flow_config[i].rtcmac_flow_id = mac_flow_id;
      /* store the mac_flow_id */
  }
  

  /* Till here, the flow config is valid */

  return TRUE;

} /* static boolean hdrretap_add_tx_flow_param */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_STATS_CLEAR

DESCRIPTION
  Clears the stats for each rate and for each subtype.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hdrretap_stats_clear( void )
{

  uint8 i;
  uint8 j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* pl_01 stats clear */

  for ( i=0; i<HDRRETAP_NUM_REV_RATES_PL_01; i++ ) 
  {
    hdrretap.pl_01_stats[i].seqno = 0x0;
  }

  /* pl_2 stats clear */

  qw_set( hdrretap.pl_23_stats_revsystime, 0x0, 0x0 );

  for ( i=0; i<HDRRETAP_NUM_REV_PAYLOAD_SIZE; i++ ) 
  {
    hdrretap.pl_2_stats[i].seqno = 0x0;
  }

  /* pl_3 stats clear */

  for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ )
  {
    for ( j=0; j<HDRRETAP_NUM_REV_PAYLOAD_SIZE; j++ ) 
    {
      hdrretap.pl_3_stats[i][j].seqno = 0x0;
    }
  }
  
} /* static void hdrretap_stats_clear( void ) */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_STATS_PRINT

DESCRIPTION
  Prints via diag (F3 msg) the stats of the corresponding pkt mode.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hdrretap_stats_print( void )
{

  uint8 i;
  uint8 j;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++++++++++ RETAP Statistics +++++++++++++");
  
  if (( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 ) ||
      ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_1 ))
  {
    for ( i=0; i<HDRRETAP_NUM_REV_RATES_PL_01; i++ )
    {
      HDR_MSG_IS890_3( MSG_LEGACY_HIGH,  "+++ Pkts @ %d.%d: %d",
                       hdrretap.pl_01_stats[i].rate,
                       hdrretap.pl_01_stats[i].rate_dec,
                       hdrretap.pl_01_stats[i].pkt_count );
    }
  }
  else if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
  {

    HDR_MSG_IS890_2(MSG_LEGACY_HIGH, 
                    "+++ PL_2_RevSysTime(subframe): %08x:%08x",
                    hdrretap.pl_23_stats_revsystime[0],
                    hdrretap.pl_23_stats_revsystime[1] );

    for ( i=0; i< HDRRETAP_NUM_REV_PAYLOAD_SIZE; i++ ) 
    {
      HDR_MSG_IS890_2( MSG_LEGACY_HIGH,  "+++ pkt payload size %d: %d",
                       hdrretap.pl_2_stats[i].pkt_size,
                       hdrretap.pl_2_stats[i].seqno );
    }
  }
  else if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
  {
    HDR_MSG_IS890_2(MSG_LEGACY_HIGH, 
                    "+++ PL_3_RevSysTime(subframe): 0x%08x:%08x",
                    hdrretap.pl_23_stats_revsystime[0],
                    hdrretap.pl_23_stats_revsystime[1] );

    for ( i=0; i< HDRIS890B_NUM_CARRIERS; i++ ) 
    {
      for ( j=0; j< HDRRETAP_NUM_REV_PAYLOAD_SIZE; j++ ) 
      {
        HDR_MSG_IS890_3( MSG_LEGACY_HIGH,  "+++ RL %d pkt payload size %d: %d",
                         i,
                         hdrretap.pl_3_stats[i][j].pkt_size,
                         hdrretap.pl_3_stats[i][j].seqno );
      }
    }
  }
  else
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR,  "Illegle PHY subtype" );
  }

} /* static void hdrretap_stats_print( void ) */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_GET_RL_CHANNEL_IDX_FROM_CARRIER_ID()

DESCRIPTION
  This function returns the RL channel index mapped to the specified
  carrier ID. Carrier ID is used for the data path where the RTAP attributes
  are all configured from RL channel index from the latest TCA.

DEPENDENCIES
  None
 
PARAMETERS
  carrier_id - Carrier ID to map to sub-active set index 
   
RETURN VALUE
  None
  
SIDE EFFECTS
  Updates the hdrretap.carrier_to_rl_channel_index array map.
  
===========================================================================*/
static uint8 hdrretap_get_rl_channel_idx_from_carrier_id
( 
  uint8 carrier_id 
    /* Carrier ID to map to sub-active set index */
)
{

  hdrlmac_carrier_map_type  carrier_map[HDRIS890B_NUM_CARRIERS];
  uint32                    freq_khz[HDRIS890B_NUM_CARRIERS] = {0};
  uint8                     rl_chan_idx;
  uint32                    min_freq_khz;
  uint32                    low_freq_khz;
  uint8                     low_carrier = 0;
  uint8                     i;
  uint8                     j;
  uint8                     rl_sorting_idx = 0;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 ||
      hdrretap.configuration.phy_subtype == PHY_SUBTYPE_1 || 
      hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 )
  {
    rl_chan_idx = HDRRETAP_DEFAULT_RL_CHANNEL_INDEX;
      /* Return default sub-active set info */

    hdrretap.carrier_to_rl_channel_idx[HDRRETAP_DEFAULT_CARRIER] = 
      HDRRETAP_DEFAULT_RL_CHANNEL_INDEX;
      /* Update map with default values for single carrier */
  }
  else if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {
    /* If TCA seq number is a mismatch, need to update carrier map */
    if( hdrretap.tca_seq_for_map != hdrrup_get_current_tca_seq_num() ) 
    {
      hdrretap.tca_seq_for_map = hdrrup_get_current_tca_seq_num();

      hdrretap.num_rl_carriers = 
        hdrlmac_get_carrier_map( HDRIS890B_NUM_CARRIERS, carrier_map );

      ASSERT (hdrretap.num_rl_carriers <= HDRIS890B_NUM_CARRIERS);

      /* First initialize to default values */
      for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ )
      {
        hdrretap.carrier_to_rl_channel_idx[i] = 
          HDRRETAP_INVALID_RL_CHANNEL_INDEX;

        if( i < hdrretap.num_rl_carriers )
        {
#ifdef FEATURE_RF_SVDO_API
          freq_khz[i] = 
            rfm_cdma_get_tx_carrier_freq( carrier_map[i].rev_channel ); 
#else
          freq_khz[i] = 
            rf_get_tx_carrier_freq( 
              (rf_card_band_type)carrier_map[i].rev_channel.band, 
              carrier_map[i].rev_channel.chan_num );
#endif /* FEATURE_RF_SVDO_API */
        }
      }

      min_freq_khz = 0;
      
      /* Loop through the carrier map to sort on ascending frequency of rev 
         channal */
      for( i=0; i<hdrretap.num_rl_carriers; i++ )
      {
        low_freq_khz = HDRIS890_INVALID_CHAN_FREQUENCY;

        for( j=0; j<hdrretap.num_rl_carriers; j++ )
        {
          HDR_MSG_IS890_6( 
            MSG_LEGACY_HIGH, 
            "index: %d carrier: %d chan: %d freq: %dKHz min: %dKHz low: %dKHz",
            i,
            j,
            carrier_map[j].rev_channel.chan_num,
            freq_khz[j],
            min_freq_khz,
            low_freq_khz );

          if( (freq_khz[j] > min_freq_khz) && (freq_khz[j] < low_freq_khz) ) 
          {
            low_carrier = carrier_map[j].carrier_index;
            low_freq_khz = freq_khz[j];
          }
        }

        if( low_freq_khz != HDRIS890_INVALID_CHAN_FREQUENCY ) 
        {
          min_freq_khz = low_freq_khz;
            /* The next highest channel has to be greater than cur low chan */

          hdrretap.carrier_to_rl_channel_idx[low_carrier] = rl_sorting_idx;
            /* Store the RL channel index for this next lowest carrier */

          rl_sorting_idx++;

        }

        HDR_MSG_IS890_2( MSG_LEGACY_HIGH, "RLCIndex: %d C: %d", 
                     i,
                     low_carrier );
  
      }

    } /* TCA Seq mis-match */

    rl_chan_idx = ( (carrier_id < HDRIS890B_NUM_CARRIERS)? 
                    hdrretap.carrier_to_rl_channel_idx[carrier_id]:
                    HDRRETAP_INVALID_RL_CHANNEL_INDEX );
      /* Return up to date rl channel index */
  }
  else
  {
    rl_chan_idx = HDRRETAP_INVALID_RL_CHANNEL_INDEX;
      /* Return invalid */
  }

  if( rl_chan_idx == HDRRETAP_INVALID_RL_CHANNEL_INDEX )
  {
    HDR_MSG_IS890_2( MSG_LEGACY_ERROR, "Invalid carrier %d, phy %d", 
                     carrier_id,
                     hdrretap.configuration.phy_subtype );
      /* Boundary checking */
  }

  return rl_chan_idx;

} /* hdrretap_get_rl_channel_idx_from_carrier_id */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_STATS_GET_FROM_CARRIER_ID()

DESCRIPTION
  Returns pointer to stats type that corresponds to PHY subtype and carrier
  ID. Should be used as the only means to access hdrretap.pl_2_stats or 
  hdrretap.pl_3_stats.
  
DEPENDENCIES
  None.

PARAMETERS
  carrier_id    - Index representing physical carrier for Rx or Tx packet
  
RETURN VALUE
  hdrretap_stats_type*

SIDE EFFECTS
  None.

===========================================================================*/
static hdrretap_stats_type* hdrretap_stats_get_from_carrier_id(
  
  uint8 carrier_id
    /* Index representing physical carrier for Rx or Tx packet */
)
{
  hdrretap_stats_type *stats = NULL;
  uint8               rl_channel_idx;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 ||
       hdrretap.configuration.phy_subtype == PHY_SUBTYPE_1 ) 
  {
    stats = hdrretap.pl_01_stats;
  }
  else if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 )
  {
    stats = hdrretap.pl_2_stats;
  }
  else if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {
    rl_channel_idx = hdrretap_get_rl_channel_idx_from_carrier_id(carrier_id);
    
    if( rl_channel_idx == HDRRETAP_INVALID_RL_CHANNEL_INDEX ) 
    {
      HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "RETAP: invalid carrier index %d",
                     carrier_id );
    }
    else
    {
      ASSERT (rl_channel_idx < HDRIS890B_NUM_CARRIERS);
      stats = hdrretap.pl_3_stats[rl_channel_idx];
    }
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "RETAP: invalid subtype %d",
                     hdrretap.configuration.phy_subtype );
  }  

  return stats;
} /* hdrretap_stats_get_from_carrier_id */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PKT_SIZE_MODE_GET_TGT

DESCRIPTION
  This function is called to compute the target rate for the passed-in 
  packet size mode container.

DEPENDENCIES
  None
 
PARAMETERS
  mode_config - Pointer to payload size mode storage
 
RETURN VALUE
  Target payload size index. If the test packet mode is not enable or the 
  configured payload size mode is not enabled; the function return the max 
  payload size index.
  
SIDE EFFECTS
  None
===========================================================================*/
static uint8 hdrretap_pkt_size_mode_get_tgt( 

  hdrretap_pkt_size_mode_type *mode_config
    /* Pointer to payload size mode storage */  
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  
  /*------------------------------------------------------------------
   * The rate control logic only works when
   * 1. the hdrretap is in the traffic state  AND
   * 2. PHY subtype 2 (or the rtcmac3)        AND
   * 3. test packt mode is enabled            AND
   * 4. configured payload size mode enabled
   *----------------------------------------------------------------*/
  
  if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ||
       hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {
    if (( hdrretap.state == HDRRETAP_TRAFFIC )                  &&
        ( hdrretap.configuration.test_pkt_mode )                &&
        ( mode_config->configured ))
    {
  
      /* Update our current rate for the sigsaw rate function */
  
      mode_config->tgt_index++;
      mode_config->tgt_index %= HDRRETAP_NUM_REV_PAYLOAD_SIZE;
        
      if (( mode_config->tgt_index > mode_config->max_index ) ||
          ( mode_config->tgt_index < mode_config->min_index ))
      {
          mode_config->tgt_index = mode_config->min_index;
      }

      HDR_MSG_IS890_1 ( MSG_LEGACY_HIGH,"RETAP: Target Rate Index: %d",
                        mode_config->tgt_index );
    }
    else /* not in the test packet mode */
    {
      mode_config->tgt_index = HDRRETAP_MAX_REV_PAYLOAD_SIZE_INDEX;
    }
  }

  return mode_config->tgt_index;

} /* hdrretap_pkt_size_mode_get_tgt */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_23_PACK_PNFILL

DESCRIPTION
  Fills the transmit buffer with specified number of fill bytes.

DEPENDENCIES
  None
 
PARAMETERS
  data_ptr - pointor to the target dsm item
  length   - how many bytes to fill in

RETURN VALUE
  how many bytes filled
  
SIDE EFFECTS
  None
===========================================================================*/
static uint16 hdrretap_pl_23_pack_pnfill
(
  dsm_item_type   *data_ptr,
    /* ptr to the DSM item */

  uint16          length
    /* PN filling length */
)
{

  static uint16 curr_position = 0;
  uint16        seg0_len, seg1_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( data_ptr != NULL );

  /* Sanity check if the static array of the PN fill has been generated */

  if ( !hdrretap.pnfill_buf.if_initialized ) 
  {
    ERR ("RETAP: PNFILL buf is not initialized !", 0, 0, 0);
  }

  /* Sanity check the filling length. the max length is 12288 bit which 
   * is 1536 byte*/

  if ( length > HDRRETAP_PNFILL_BUF_LENGTH )
  {
    ERR_FATAL( "RETAP: filling length is more than 4096 byte", 0, 0, 0 );
  }

  /* Consider the possible wrapaournd  */

  if ( curr_position + length >= HDRRETAP_PNFILL_BUF_LENGTH) 
  {
    ASSERT (curr_position < HDRRETAP_PNFILL_BUF_LENGTH);

    seg0_len = HDRRETAP_PNFILL_BUF_LENGTH - curr_position;
    seg1_len = length + curr_position - HDRRETAP_PNFILL_BUF_LENGTH ;

    /* Only consider one wrap around */

#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( data_ptr->data_ptr + data_ptr->used, seg0_len,
             hdrretap.pnfill_buf.pnfill + curr_position, 
             seg0_len );

    memscpy( data_ptr->data_ptr + data_ptr->used + seg0_len , seg1_len, 
             hdrretap.pnfill_buf.pnfill , 
             seg1_len );
#else
    memcpy( data_ptr->data_ptr + data_ptr->used, 
            hdrretap.pnfill_buf.pnfill + curr_position, 
            seg0_len );


    memcpy( data_ptr->data_ptr + data_ptr->used + seg0_len , 
            hdrretap.pnfill_buf.pnfill , 
            seg1_len );
#endif /* FEATURE_MEMCPY_REMOVAL */

    /* Wrap around part */

    curr_position = seg1_len ;

  }
  else
  {

#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( data_ptr->data_ptr + data_ptr->used, length, 
            hdrretap.pnfill_buf.pnfill + curr_position, 
            length );
#else
    memcpy( data_ptr->data_ptr + data_ptr->used,
            hdrretap.pnfill_buf.pnfill + curr_position, 
            length );
#endif /* FEATURE_MEMCPY_REMOVAL */

      /* Save all old settings */
    curr_position += length;

  }
  return length;

}
/* hdrretap_pl_23_pack_pnfill */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_01_GEN_TEST_PKT

DESCRIPTION
  This function is called by to generate a packet and put the generated
  PKT into the RETAP buffer

  In TAP spec 3-4 line 9 
  If the RETAP test packet mode is enabled, the AT shall generate an RETAP
  test packet at the beginning of every 16 slot interval aligned to the 
  CDMA system Time in the frames
  This is a internal function

DEPENDENCIES
  None
 
PARAMETERS
  sys_frame_time - timestamp in frames for packet
 
RETURN VALUE
  FALSE - Fail
  TRUE - Successful
  
SIDE EFFECTS
  None
===========================================================================*/

static boolean hdrretap_pl_01_gen_test_pkt(

  qword   sys_frame_time
    /* Timestamp in frames for packet */
)
{
  uint16          sys_time_slot0;
  int             i;
  dsm_item_type   *wr_ptr;
  uint8           cur_wr_index;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*---------------------------------------------------------------------
   *  Sanity Check
   *-------------------------------------------------------------------*/

  if ( hdrretap.state != HDRRETAP_TRAFFIC ) return FALSE;
    /* The RETAP Packets shall be generated only in the Connected State */

  if ( ! hdrretap.configuration.test_pkt_mode ) return FALSE;
    /* The RETAP test packet mode has to be enabled */

  /* check the wr_index in the test buffer */

  wr_ptr = 
      hdrretap.pl_01_test_pkt_buf.pkt_ptr[hdrretap.pl_01_test_pkt_buf.wr_index];
  cur_wr_index = hdrretap.pl_01_test_pkt_buf.wr_index;

  if ( wr_ptr != NULL )
  {
    /* Test packet buf Overflow */

    HDR_MSG_IS890( MSG_LEGACY_ERROR,  "RETAP: Test pkt overflow" );

    /* Free the DSM item to discard the packet at wr_index and generate a new
       one at this index */
    dsm_free_packet(&wr_ptr);

    /* Adjust the read index accordingly */
    hdrretap.pl_01_test_pkt_buf.rd_index ++;
    hdrretap.pl_01_test_pkt_buf.rd_index %= HDRRETAP_TEST_PKT_BUFFER_SIZE;

    /* set the overflow indicator */
    hdrretap.pl_01_test_pkt_buf.overflow = TRUE;
  }

  hdrretap.pl_01_test_pkt_buf.pkt_ptr[cur_wr_index] =
   hdrutil_new_buffer(DSM_DS_POOL_SIZE((HDRRETAP_PL_01_TEST_PKT_SIZE/8)+1),2);
    /* request a new dsm item with 2 bytes offset */

  wr_ptr = hdrretap.pl_01_test_pkt_buf.pkt_ptr[cur_wr_index] ;
    /* save the ptr to the wr_ptr */

  /*---------------------------------------------------------------------
   * Generation
   * Fill in the packet contents 
   * spec 3-9 defines the formate of the RETAP Teset Packet
   * The AT txs theses packets on the reverse traffic channel
   * ProtocolID : 2bits : 01
   * PacketType : 4bits : 0
   * RevSysTime : 15bit : CDMA system Time in frames mod 32k corresponding
   *                        to the slot boundary when the RETAP test packet was 
   *                        generated
   * RETAPTesetPktOverflow  : 1bit  : ??
   * Seq_0        : 12bit
   * Seq_1        : 12bit
   * Seq_2        : 12bit
   * Seq_3        : 12bit
   * Seq_4        : 12bit
   * Seq_5        : 12bit  
   -------------------------------------------------------------------*/

  /* 1st byte for the protocol ID and Packet Type    */

  *( wr_ptr->data_ptr ) = HDRRETAP_PROTOCOL_ID << 4;

  wr_ptr->used ++ ;
    
  /* System time in frames (15 bit) */

  sys_time_slot0 = (qw_lo( sys_frame_time ) & 0x00007fff);

  if ( dsmbit_pack16_tail( wr_ptr,
                           sys_time_slot0,
                           8,    
                           15 )  
        != 15 )
  {
    ERR_FATAL( "RETAP: Could not pack test pkt", 0, 0, 0 );
  }
    
  /* Overflow bit */

  if ( hdrretap.pl_01_test_pkt_buf.overflow )
  {
    *( wr_ptr->data_ptr + 2 ) |= 0x01;
  }

  /* Loop through the sequence numbers 
   * in spec 3-4, line 18
   * The information contained in the RETAP test packet 
   * shall cover RTC MAC Packets transmitted up to, 
   * but not including, the time instant of generation; */

  for ( i=0; i<HDRRETAP_NUM_REV_RATES_PL_01; i++ )
  {
     if ( dsmbit_pack16_tail( wr_ptr,
                              hdrretap.pl_01_stats[i].seqno,
                              24 + (i * 12),/*lint !e734*/
                              12 )
           != 12 )
     {
        ERR_FATAL( "RETAP: Could not pack test pkt", 0, 0, 0 );
     }
  }

  /* Update the write index for the RETAP test pkt buffer */

  hdrretap.pl_01_test_pkt_buf.wr_index++;
  hdrretap.pl_01_test_pkt_buf.wr_index %= HDRRETAP_TEST_PKT_BUFFER_SIZE;

  return TRUE;

} /* hdrretap_pl_01_gen_test_pkt*/

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_01_GET_PKT_SIZE

DESCRIPTION
  This function is called by the RMAC to determine the current reverse rate 
  in configured rate mode.

  The function replace the obsolate function " hdrretap_get_rmac_pkt_size "

DEPENDENCIES
  None
 
PARAMETERS
  None

RETURN VALUE
  Number of bytes for this packet.
  
SIDE EFFECTS
  None
===========================================================================*/
LOCAL uint16 hdrretap_pl_01_get_pkt_size( void )
{

  uint16                              max_rmac_pkt_size;
  hdrretap_pl_01_config_type          *f01_cfg_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  f01_cfg_ptr = &( hdrretap.configuration.config_pl_01 );

  /*------------------------------------------------------------------
   * Check if RETAP controls the reverse rate and set it accordingly 
   *----------------------------------------------------------------*/

  if ( ( hdrretap.state == HDRRETAP_TRAFFIC )                 &&
       ( hdrretap.configuration.phy_subtype < PHY_SUBTYPE_2 ) &&
         hdrretap.configuration.test_pkt_mode                 &&
         f01_cfg_ptr->configured_pkt_rate_mode )
  {
    /* Update our current rate for the sigsaw rate function */

    f01_cfg_ptr->current_rate++;
    f01_cfg_ptr->current_rate %= HDRRETAP_NUM_REV_RATES_PL_01;
      
    if(( f01_cfg_ptr->current_rate>f01_cfg_ptr->pkt_rate_mode_config.max_rate)||
       ( f01_cfg_ptr->current_rate<f01_cfg_ptr->pkt_rate_mode_config.min_rate ))
    {
      f01_cfg_ptr->current_rate = f01_cfg_ptr->pkt_rate_mode_config.min_rate;
    }
      
    /* Check if the current rate is bigger than the reverse rate limit */

    if ( f01_cfg_ptr->current_rate > HDR_REV_PL01_RATE_153_6 )
    {
      HDR_MSG_IS890_2 (MSG_LEGACY_HIGH,  "RETAP: Adjusting rate %d -> %d",
                       f01_cfg_ptr->current_rate,
                       HDR_REV_PL01_RATE_153_6 );
        
      f01_cfg_ptr->current_rate = HDR_REV_PL01_RATE_153_6;
    }
      
    /* As we cannot send a null-rate packet, the seq no is updated here */

    if ( f01_cfg_ptr->current_rate == 0 )
    {
      HDR_MSG_IS890_1 (MSG_LEGACY_MED,"RETAP: Tx test pkt at 0.0, seqno 0x%x",
                       hdrretap.pl_01_stats[0].seqno);
        
      hdrretap.pl_01_stats[0].seqno++;
      hdrretap.pl_01_stats[0].seqno %= 0x1000;
        /* the seq num is only 12 bit in length */
        
      hdrretap.pl_01_stats[0].pkt_count ++;
        /* no wrapping included ... */
    }
      
    max_rmac_pkt_size = f01_cfg_ptr->current_rate;

  }
  else 
  {
    max_rmac_pkt_size = HDR_REV_PL01_RATE_153_6;
  }

  HDR_MSG_IS890_1 (MSG_LEGACY_MED,"RETAP: Tx test rate %d",max_rmac_pkt_size);
  
  return max_rmac_pkt_size;

} /* hdrretap_pl_01_get_pkt_size */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_01_GET_FILL_PKT

DESCRIPTION
  This function is called by the RMAC to get an RETAP fill
  packet for the rmainder of the current RTC packet.

DEPENDENCIES
  None
  the function should be only called from RTCMAC012
  or with PHYsubtype01
 
PARAMETERS
  rmac_pkt_size - Maximum size of reverse link packet
  tx_pkt_ptr    - Pointer to DSM item containing TX packet
 
RETURN VALUE
  Number of bytes for this packet.
  
SIDE EFFECTS
  None
===========================================================================*/
uint16 hdrretap_pl_01_pack_fill_pkt
(
  uint16           rmac_pkt_size,
    /* fill packet size */

  dsm_item_type**  tx_pkt_ptr
    /* the link list pointor */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Make sure we have a valid fill packet */

  if ( hdrretap.fill_pkt_ptr == NULL ) return 0;

  /* Set RTAP Fill header and 2 bit StreamId 
     in the first byte of data_ptr as PCP Connection layer length field
     would be set in hdrpcp_build_packet */

  *(hdrretap.fill_pkt_ptr->data_ptr) = 
    (hdrretap.configuration.stream_id << 6)
    | (HDRRETAP_PROTOCOL_ID << 4)
    |  HDRRETAP_FILL_PKT_ID;
  
  if ( dsm_dup_packet( tx_pkt_ptr,
                       hdrretap.fill_pkt_ptr,
                       0,
                       rmac_pkt_size )
       !=
       rmac_pkt_size )
  {
    ERR_FATAL( "RETAP: Could not dup fill packet", 0, 0, 0 );
  }

  return rmac_pkt_size;

} /* hdrretap_pl_01_pack_fill_pkt */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_01_PCP_PKT_STATUS

DESCRIPTION
  This function is called immediately before get_pkt with the previous 
  packet's transmission status. 

DEPENDENCIES
  None
 
PARAMETERS
  combo_id            - Which combo was accepted.
  carrier_tx_outcome  - Whether the transmission was successful or not.
  num_outcomes        - Number of txoutcomes.
  user_data_ptr       - Pointer to user data passed back through callback.
 
RETURN VALUE
  Number of bytes for this packet.
  
SIDE EFFECTS
 
 None
===========================================================================*/
static void hdrretap_pl_01_pcp_pkt_status
(
  uint8                           combo_id,
    /* ID for previous transmission */

  hdrpcp_carrier_tx_outcome_type  carrier_tx_outcome[],
    /* Multi-carrier previous transmission status */

  uint8                           num_outcomes,
    /* Number of previous transmission statuses */
    
  void                            *user_data_ptr
    /* Generic PCP interface. NOT used in the RETAP */
)
{
  qword           cur_frame_time;
  qword           frame_delta;
  uint32          count, count_max;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mark the unused variables to avoid lint error */

  SYS_ARG_NOT_USED( user_data_ptr );
  SYS_ARG_NOT_USED( carrier_tx_outcome );
  SYS_ARG_NOT_USED( num_outcomes );
  SYS_ARG_NOT_USED( combo_id );

  ( void )hdrts_get_frame_time( cur_frame_time );

  if( hdrretap.configuration.test_pkt_mode )
  {
    qw_sub( frame_delta, cur_frame_time, hdrretap.pl_01_prev_frame_time );
      /* Calculate number of frames since last update. */

    /* If frame_delta is more than 1 there probably was silence period. Since
    it is a requirement to build a packet every frame, build the makeup packets
    here. Restrict number of makeup packets to the size of RETAP packet buffer.
    If prev_frame_time is 0, ignore. */
    if( qw_lo( hdrretap.pl_01_prev_frame_time ) && qw_lo( frame_delta ) > 1 )
    {
      HDR_MSG_IS890_2( MSG_LEGACY_HIGH, "SILENCE: cur %d last %d", 
                       qw_lo( cur_frame_time ) & 0xFFFF, 
                       qw_lo( hdrretap.pl_01_prev_frame_time ) & 0xFFFF );

      count_max = qw_lo( frame_delta ) - 1;

      count_max = MIN( count_max, HDRRETAP_TEST_PKT_BUFFER_SIZE );
  
      for( count=0; count<count_max; count++ ) 
      {
        qw_inc( hdrretap.pl_01_prev_frame_time, 1 );
          /* Increment timestamp for last packet by one and use this value for
          the silence period timestamp. */

        ( void )hdrretap_pl_01_gen_test_pkt( hdrretap.pl_01_prev_frame_time );
          /* Loop through and generate the packets that should have been 
          generated during silence period. */

        if( count > 0 )
        {
          /* Update seq 0 for silence, since one will be incremented from 
          last_rri, start from the 2nd silence frame here. */
          hdrretap.pl_01_stats[ 0 ].seqno++;
          hdrretap.pl_01_stats[ 0 ].seqno %= 0x1000;
          hdrretap.pl_01_stats[ 0 ].pkt_count++;
        }
      }
    }
  
    ( void )hdrretap_pl_01_gen_test_pkt( cur_frame_time );
      /* Gen test packet every frame */

    HDR_MSG_IS890_3 (
      MSG_LEGACY_MED,  "RETAP: Tx test pkt at %d.%d, seqno 0x%x",
      hdrretap.pl_01_stats[carrier_tx_outcome[ 0 ].last_rri].rate,
      hdrretap.pl_01_stats[carrier_tx_outcome[ 0 ].last_rri].rate_dec,
      hdrretap.pl_01_stats[carrier_tx_outcome[ 0 ].last_rri].seqno );
      
    /* Update the sequence numbers to include this packet (12-bit number) */
  
    hdrretap.pl_01_stats[carrier_tx_outcome[ 0 ].last_rri].seqno++;
    hdrretap.pl_01_stats[carrier_tx_outcome[ 0 ].last_rri].seqno %= 0x1000;
    hdrretap.pl_01_stats[carrier_tx_outcome[ 0 ].last_rri].pkt_count++;
  }

  qw_equ( hdrretap.pl_01_prev_frame_time, cur_frame_time );
    /* Update prev frame time with current */

  if( hdrretap.configuration.config_pl_01.configured_pkt_rate_mode )
  {
    (void) hdrrmac_set_target_ps_index( 
        HDRRETAP_DEFAULT_CARRIER, 
        hdrretap_pl_01_get_pkt_size()
    );
  }
} /* hdrretap_pl_01_pcp_pkt_status */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_01_PCP_GET_PKT

DESCRIPTION
  This function is registered to PCP. PCP calls this function every frame
  with available space. This application fills the remaining space with
  data if data is available. Data is available in TestPktMode and is only
  to be used in PL 0 or PL 1.

DEPENDENCIES
  None
 
PARAMETERS
  combo_id           - Combo ID for this packet to be built
  carrier_id         - Carrier for which packet is to be built
  max_rmac_pkt_size  - Maximum size of reverse link packet
  tx_pkt_ptr         - Pointer to DSM item containing TX packet
  tx_data_handle     - Not used in the pl_01 test packet
  user_data_ptr      - NOT used in the Pl_2 test packet
 
RETURN VALUE
  Number of bytes for this packet.
  
SIDE EFFECTS
 
 None
===========================================================================*/
static uint16 hdrretap_pl_01_pcp_get_pkt
(
  uint8           combo_id,
    /* Which transmit combination */

  uint8           carrier_id,
    /* Carrier data is to be transmitted */  

  uint16          max_rmac_pkt_size,
    /* availabe bandwidth */

  dsm_item_type   **tx_pkt_ptr,
    /* dsm item ptr (return) */

  void            **tx_data_handle,
    /* Generic PCP get_pkt interface */

  void            *user_data_ptr
    /* Generic PCP get_pkt interface */
)
{
  uint16          rmac_pkt_size = 0;
  dsm_item_type** fill_pkt_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Mark the unreferenced symbols to avoid lint error */
  SYS_ARG_NOT_USED( tx_data_handle );
  SYS_ARG_NOT_USED( user_data_ptr );
  SYS_ARG_NOT_USED( combo_id );
  SYS_ARG_NOT_USED( carrier_id );

  /* Sanity check the tx_pkt_ptr */
  ASSERT( tx_pkt_ptr != NULL );

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   * Sanity check 
   *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( max_rmac_pkt_size == 0 ) return 0;
    /* not zero bandwith */

  if ( hdrretap.state != HDRRETAP_TRAFFIC ) return 0;
    /* retap in the traffic state */

  if ( ! hdrretap.configuration.test_pkt_mode ) return 0;
    /* Check if test packet mode is enabled */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   * Check if bandwith is enough for a minmum size packet 
   *- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (( max_rmac_pkt_size < HDRRETAP_PL_01_TEST_PKT_MIN_SIZE_BYTE ) ||
      ( hdrretap.pl_01_test_pkt_buf.pkt_ptr[hdrretap.pl_01_test_pkt_buf.rd_index] 
        == NULL) )
  {

    rmac_pkt_size = 0;

    if( hdrretap.configuration.config_pl_01.configured_pkt_rate_mode ) 
    {

      /* Find the last pkt_ptr in the dsm chain */
      for ( fill_pkt_ptr = tx_pkt_ptr;
            *fill_pkt_ptr != NULL;
            fill_pkt_ptr = &((*fill_pkt_ptr)->pkt_ptr) )
      {
      }

      if( fill_pkt_ptr ) 
      {
        /* Pass the last pkt_ptr to get_fill_pkt() */
        rmac_pkt_size = hdrretap_pl_01_pack_fill_pkt( 
          max_rmac_pkt_size,
          fill_pkt_ptr );
        HDR_MSG_IS890_3 (MSG_LEGACY_MED,"RETAP: Fill %d Max %d used(%d)", 
                       rmac_pkt_size, 
                       max_rmac_pkt_size, 
                       (*tx_pkt_ptr)->used );
      }
      else
      {
        HDR_MSG_IS890_2 (MSG_LEGACY_MED,"RETAP: Didn't pack fill %d Max %d",
                       rmac_pkt_size, 
                       max_rmac_pkt_size);
      }
    }

  }
  else
  {

    /* Hand enqueued test packet to PCP */

    *tx_pkt_ptr = 
      hdrretap.pl_01_test_pkt_buf.pkt_ptr[hdrretap.pl_01_test_pkt_buf.rd_index];
    hdrretap.pl_01_test_pkt_buf.pkt_ptr[hdrretap.pl_01_test_pkt_buf.rd_index]=NULL;

    /* Update read index into test packet buffer */  

    hdrretap.pl_01_test_pkt_buf.rd_index ++;
    hdrretap.pl_01_test_pkt_buf.rd_index %= HDRRETAP_TEST_PKT_BUFFER_SIZE;

    /* Pkt size is 2-bit stream header + test pkt */

    rmac_pkt_size = (HDRRETAP_PL_01_TEST_PKT_SIZE + 2) / 8;

    HDR_MSG_IS890_2 (MSG_LEGACY_MED,"RETAP: Data %d Max %d", 
                     rmac_pkt_size, 
                     max_rmac_pkt_size);
  }
  
  return rmac_pkt_size;

} /* hdrretap_pl_01_pcp_get_pkt */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_23_PCP_PKT_STATUS

DESCRIPTION
  The function is called from the PCP to update the RMCTAP flow about the 
  status of the previous rtcmac3/4 packets. If packets are accepted, then 
  each flow needs to update its buffer size.

DEPENDENCIES
  None
 
PARAMETERS
  *user_data_ptr 
  *tx_data_handle 
  pkt_accept
 
RETURN VALUE
  target payload size index 
  
SIDE EFFECTS
 None
===========================================================================*/
static void hdrretap_pl_23_pcp_pkt_status
(
  uint8                           combo_id,
    /* ID for previous transmission */

  hdrpcp_carrier_tx_outcome_type  carrier_tx_outcome[],
    /* Multi-carrier previous transmission status */

  uint8                           num_outcomes,
    /* Number of previous transmission statuses */
    
  void                            *user_data_ptr
    /* Generic PCP interface. NOT used in the RETAP */
)
{

  uint8                                 i;
  uint8                                 flow;
  hdrretap_pl_23_config_type            *f23_cfg_ptr;
  uint32                                current_lap;
  qword                                 current_slot_time;
  uint8                                 carrier_idx;
  uint8                                 outcome_idx;
  uint32                                num_bytes_tx;
  hdrretap_stats_type                   *stats;
  boolean                               tx_success = FALSE;
  uint8                                 rri;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* sanity check the user data ptr; it should points the corresponding 
   * item in the flow config table */

  ASSERT( user_data_ptr != NULL );

  SYS_ARG_NOT_USED( combo_id );
    /* Mark the unreferenced varialbe to avoid lint error */

  /*------------------------------------------------------------------
   * Sanity check the PHY subtype 
   *------------------------------------------------------------------ */

  if ( hdrretap.configuration.phy_subtype != PHY_SUBTYPE_2 && 
       hdrretap.configuration.phy_subtype != PHY_SUBTYPE_3 ) 
  {
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR, "RETAP: Invalid subtype %d",
                     hdrretap.configuration.phy_subtype );
    return;
  }

  f23_cfg_ptr = &( hdrretap.configuration.config_pl_23 );

  /* If no combos were accepted, set each lo buff to "not accepted" */
  if( combo_id == HDRPCP_NO_COMBO_ID_ACCEPTED )
  {
    num_bytes_tx = 0;
      /* No bytes should be decremented from flow buffers */

    tx_success = FALSE;
      /* Flag indicates that there were no packets tx'd */

    for( carrier_idx=0; carrier_idx<HDRIS890B_NUM_CARRIERS; carrier_idx++ )
    {
      (( hdrretap_user_data_type *) user_data_ptr )->
        prev_pkt_size[carrier_idx] = 0 ;
        /* reset the prev pkt size */
    }
  }
  /* If combo_id is valid, something was accepted. Update across carriers */
  else
  {
    num_bytes_tx = 0;

    for( outcome_idx=0; outcome_idx<num_outcomes; outcome_idx++ ) 
    {
      carrier_idx = carrier_tx_outcome[outcome_idx].carrier_id;
      stats = hdrretap_stats_get_from_carrier_id( carrier_idx );

      if( stats )
      {
        if( carrier_tx_outcome[outcome_idx].tx_set_accepted == E_SUCCESS )
        {
          tx_success = TRUE;
            /* Flag indicates that there was at least one packet tx'd */
    
          num_bytes_tx += 
            ((( hdrretap_user_data_type * )user_data_ptr )->
              prev_pkt_size[carrier_idx] );

          /* Last packet was encoded, update rate */
          if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
          {
            /* get_tgt will set to max rate if TestPacketMode is not enabled */
            (void) hdrretap_pkt_size_mode_get_tgt( 
              &f23_cfg_ptr->payload_size_mode_config );
          }
          else if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
          {
            /* get_tgt will set to max rate if TestPacketMode is not enabled */
            (void) hdrretap_pkt_size_mode_get_tgt( 
                &f23_cfg_ptr->payload_size_mode_config_mc[carrier_idx] );
          }

        }

        rri = carrier_tx_outcome[outcome_idx].last_rri;
          /* Copy RRI */

        if( rri != HDRPCP_RRI_CANCELED && rri < HDRRETAP_NUM_REV_PAYLOAD_SIZE )
        {
          /* In silence period or empty buffer situations, no packets are 
          transmitted. SeqNo 0 needs to be updated then. To avoid updating when
          a previous packet is continuing, last_rri will be set to 
          HDRPCP_RRI_CANCELED as a marker (ie != 0). */

          stats[rri].seqno++;
          stats[rri].pkt_count++;

          HDR_MSG_IS890_3( MSG_LEGACY_HIGH, 
                           " RETAP: C %d RRI %d SEQ %d", 
                           carrier_idx, 
                           rri,
                           stats[rri].seqno );

          if( carrier_tx_outcome[outcome_idx].last_rri == 0 ) 
          {
            HDR_MSG_IS890_2( MSG_LEGACY_HIGH, "****RETAP: C %d RRI 0 %d****", 
                             carrier_idx, stats[0].seqno );
          }
        }
      }
      else
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, "RETAP: StatsGet Carrier %d", 
                         carrier_idx );
      }

      (( hdrretap_user_data_type *) user_data_ptr )->
        prev_pkt_size[carrier_idx] = 0 ;
        /* reset the prev pkt size */
    }
  }

  
  /*------------------------------------------------------------------
   * Based on the pkt status,the buffer size of the flow is updated; 
   * the previous pkt size is stored in the user_data
   *------------------------------------------------------------------ */

  flow = ((hdrretap_user_data_type *)user_data_ptr)->table_index;

  /*------------------------------------------------------------------
   * update the pl_23RevSysTime; the RevSystem time is used in 
   * packing the RETAPGetStatsRequestResponse indicating the time of last
   * successfull transmission. This is because stats are only generated
   * when a packet is successfully transmitted. The stats are valid at
   * the time of the last successful transmission so only update the stats
   * time when there is a successful transmission.
   *------------------------------------------------------------------ */

  if ( tx_success == TRUE )
  {

    /* Get the current time in cycles and slots into the last cycle. Set 
     * cycle length to 1 slot, so the result is already in slots. this is 
     * actually the OTA time of last succeful transmission*/

    hdrts_get_cycle_and_slot( current_slot_time,
                              &current_lap,
                              1 );

    qw_shift( current_slot_time , -2 );
    qw_equ( hdrretap.pl_23_stats_revsystime, current_slot_time );
      /* get the time in the subframe */

  }

  /*------------------------------------------------------------------
   * 1. the pkt is accepted by physical layer
   * 2. the current RETAP flow is in the burst size mode
   * 3. the current RETAP flow is not the full buffer mode 
   *------------------------------------------------------------------ */

  if (( num_bytes_tx > 0 ) &&
      ( f23_cfg_ptr->flow_config[flow].burst_size_mode ) &&
      ( f23_cfg_ptr->flow_config[flow].init_burst_size != 0xffff )) 
  {

    /* apply the prev pkt size to the current buf size */

    if ( f23_cfg_ptr->flow_config[flow].current_buf_size >= num_bytes_tx )
    {
      f23_cfg_ptr->flow_config[flow].current_buf_size -= num_bytes_tx;
    }
    else
    {
      f23_cfg_ptr->flow_config[flow].current_buf_size = 0;
    }

  } 

  HDR_MSG_IS890_3(MSG_LEGACY_MED,"RETAP:PCPPrePktSz: ssid%d , pps%d, bs%d",
                  f23_cfg_ptr->flow_config[flow].substream_id,
                  num_bytes_tx,
                  f23_cfg_ptr->flow_config[flow].current_buf_size );

  if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
  {
    if( f23_cfg_ptr->payload_size_mode_config.configured )
    {

      (void) hdrrmac_set_target_ps_index( 
        HDRRETAP_DEFAULT_CARRIER, 
        f23_cfg_ptr->payload_size_mode_config.tgt_index );

    }
  }
  else if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
  {
    for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ )
    {
      if( f23_cfg_ptr->payload_size_mode_config_mc[i].configured ) 
        {
          /* get_tgt returns max rate if TestPacketMode is not enabled */
          (void) hdrrmac_set_target_ps_index( 
          i, 
          f23_cfg_ptr->payload_size_mode_config_mc[i].tgt_index );
      }
    }
  }
} /* hdrretap_pl_23_pcp_pkt_status */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_01_PCP_GET_BUF_SIZE

DESCRIPTION
  The function is registered the PCP flow transmission interface; the function
  is used by PCP to retrieve the buffer size of each link flow
  In the function, the flow elaps time is updated and if the burst period is 
  reached, the current buffer size will be incremented a size as 

DEPENDENCIES
  None
 
PARAMETERS
  num_pkts_pending  - Pointer to the number of packets pending for this module.
  user_data_ptr     - a pointor to a storage per flow which includes the 
                      the link flow id 
                   
RETURN VALUE
  the buffer size of the corresponding flow
SIDE EFFECTS
  None
===========================================================================*/
static uint32 hdrretap_pl_01_pcp_get_buf_size
( 
  uint32 *num_pkts_pending,
    /* Return the number of packet pending; the parameter is used by 
     * PCP only to decide if fmtA pakcet is needed; for RETAP it should 
     * always be either 0 or 1 */

  void *user_data_ptr
    /* Used to get the flow index in the flow table */  

)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check the pointors */

  ASSERT(( num_pkts_pending != NULL ) &&
         ( user_data_ptr != NULL ));

  /*------------------------------------------------------------------
   * Sanity check 
   * 1. The test pkt mode has to be enabled
   *------------------------------------------------------------------ */

  if( !hdrretap.configuration.test_pkt_mode )
  {
    *num_pkts_pending = 0;
    return 0;
  }

  if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 || 
      hdrretap.configuration.phy_subtype == PHY_SUBTYPE_1 )
  {
    /* Advertise large buffer so that TAP can fill all available space */
    return 0xFFFF;
  }
  else
  {
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR, "RETAP: Invalid subtype %d",
                     hdrretap.configuration.phy_subtype );
    return 0;
  }
} /* hdrretap_pl_01_pcp_get_buf_size */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_23_PCP_GET_BUF_SIZE

DESCRIPTION
  The function is registered the PCP flow transmission interface; the function
  is used by PCP to retrieve the buffer size of each link flow
  In the function, the flow elapsed time is updated and if the burst period is 
  reached, the current buffer size will be incremented a size as 

DEPENDENCIES
  None
 
PARAMETERS
  num_pkts_pending  - Pointer to the number of packets pending for this module.
  user_data_ptr     - a pointor to a storage per flow which includes the 
                      the link flow id 
                   
RETURN VALUE
  the buffer size of the corresponding flow
SIDE EFFECTS
  None
===========================================================================*/
static uint32 hdrretap_pl_23_pcp_get_buf_size
( 
  uint32 *num_pkts_pending,
    /* Return the number of packet pending; the parameter is used by 
     * PCP only to decide if fmtA pakcet is needed; for RETAP it should 
     * always be either 0 or 1 */

  void *user_data_ptr
    /* Used to get the flow index in the flow table */  

)
{

  uint8                                 i;
  hdrretap_pl_23_config_type            *f23_cfg_ptr;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check the pointors */

  ASSERT(( num_pkts_pending != NULL ) &&
         ( user_data_ptr != NULL ));

  /*------------------------------------------------------------------
   * Sanity check 
   * 1. The test pkt mode has to be enabled
   *------------------------------------------------------------------ */

  if( !hdrretap.configuration.test_pkt_mode )
  {
    *num_pkts_pending = 0;
    return 0;
  }

  if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 || 
      hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 )
  {
  
    /*------------------------------------------------------------------
     * Get the flow index/config Ptr/init the num_pkts_pending
     *------------------------------------------------------------------ */
  
    i = ((hdrretap_user_data_type *) user_data_ptr)->table_index;
      /* flow index */
  
    f23_cfg_ptr = &( hdrretap.configuration.config_pl_23 );
      /* config ptr */
  
    *num_pkts_pending = 0;
      /* init the number of pkts pending */
  
    /*------------------------------------------------------------------
     * update elape time; PCP make sure the function call every RTCMAC3
     * interlace.
     *------------------------------------------------------------------ */
  
    f23_cfg_ptr->flow_config[i].elaps_time += 4; 
     
    /*------------------------------------------------------------------
     * update the buffer_size when
     * 1. the burst period has elapsed AND
     * 2. the burst period is not infinite
     *------------------------------------------------------------------ */
  
    if (( f23_cfg_ptr->flow_config[i].elaps_time >
        (uint32)(f23_cfg_ptr->flow_config[i].burst_period * 600 )) &&
        ( f23_cfg_ptr->flow_config[i].burst_period != 0xffff ))
    {
  
      f23_cfg_ptr->flow_config[i].elaps_time = 0; 
        /* reset the elaps_time */
  
      /* if not the full buffer mode, update the current buf size */
  
      if ( f23_cfg_ptr->flow_config[i].init_burst_size != 0xffff )
      {
  
        f23_cfg_ptr->flow_config[i].current_buf_size +=
          f23_cfg_ptr->flow_config[i].init_burst_size * 1000;
          /* increase the buffer size with the init_burst_size */
  
        /* Never above the full buffer size */
  
        if ( f23_cfg_ptr->flow_config[i].current_buf_size >= 0xffff*1000 ) 
        {
          f23_cfg_ptr->flow_config[i].current_buf_size = 0xffff*1000 ;
        }
  
      }
  
    }
    
    /*------------------------------------------------------------------
     * MSG output about the buf size and return
     *------------------------------------------------------------------ */
  
    HDR_MSG_IS890_3 (MSG_LEGACY_MED,"RETAP: PCPGetBufSz: ssid%d, et%d, bs%d",
                     f23_cfg_ptr->flow_config[i].substream_id,
                     f23_cfg_ptr->flow_config[i].elaps_time,
                     f23_cfg_ptr->flow_config[i].current_buf_size );
  
    if ( f23_cfg_ptr->flow_config[i].current_buf_size != 0 )
    {
  
      *num_pkts_pending = 1;
  
      return ( f23_cfg_ptr->flow_config[i].current_buf_size  + 10  );
        /* when any amount is pending, the return buf size includes the 
         * length of the test packet */
    }
    else
    {
      
      *num_pkts_pending = 0;
        /* no test packet transmission when current_buf_size is zero */
  
      return 0;
  
    }
  }
  else
  {
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR, "RETAP: Invalid subtype %d",
                     hdrretap.configuration.phy_subtype );
    return 0;
  }
} /* hdrretap_pl_23_pcp_get_buf_size */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PL_23_PCP_GET_PKT

DESCRIPTION
  This function is registered to PCP. PCP calls this function every sub-frame
  with available space. This application fills the remaining space with
  data if data is available. Data is available in TestPktMode and is only
  to be used in PL 2 or PL 3.

DEPENDENCIES
  None
 
PARAMETERS
  combo_id           - Combo ID for this packet to be built
  carrier_id         - Carrier for which packet is to be built
  max_rmac_pkt_size  - Maximum size of reverse link packet
  tx_pkt_ptr         - Pointer to DSM item containing TX packet
  tx_data_handle     - Not used in the pl_01 test packet
  user_data_ptr      - NOT used in the Pl_2 test packet
 
RETURN VALUE
  Number of bytes for this packet.
  
SIDE EFFECTS
 
 None
===========================================================================*/
static uint16 hdrretap_pl_23_pcp_get_pkt
(
  uint8         combo_id,
    /* Which transmit combination */

  uint8         carrier_id,
    /* Carrier data is to be transmitted */  

  uint16        max_data_size, 
    /* bandwidth allocated by PCP in bytes */

  dsm_item_type **tx_data_ptr,
    /* generated test packet to tx */

  void          **tx_data_handle,
    /* not used by RETAP */

  void          *user_data_ptr
    /* Per tx flow user data ( mainly prev. pkt size ) */
)
{

  int32                                 i;
  qword                                 current_slot_time;
  uint32                                current_lap; 
  uint32                                pack_time;
  uint8                                 bit_offset;
  uint8                                 curr_payload_size_index;
  uint16                                bandwz_left; 
  uint16                                filling_len;
  int32                                 bufsz_left;
  hdrretap_pl_23_config_type            *f23_cfg_ptr;
  dsm_item_type                         *head_dsm_ptr;
  dsm_item_type                         *nxt_dsm_ptr;
  dsm_item_type                         *cur_dsm_ptr;
  uint8                                 reserve_bits;
  uint8                                 packet_type;
  hdrretap_stats_type                   *stats;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Sanity check the pointors */ 

  ASSERT(( user_data_ptr != NULL ) &&
         ( tx_data_ptr != NULL ));

  SYS_ARG_NOT_USED( tx_data_handle );
  SYS_ARG_NOT_USED( combo_id );
    /* Mark the unreferenced symbol */

  /* Get the table index */

  i = ((hdrretap_user_data_type *) user_data_ptr)->table_index;
  f23_cfg_ptr = &( hdrretap.configuration.config_pl_23 );

  /* The max buf size allowed for this transmission */

  bufsz_left = f23_cfg_ptr->flow_config[i].current_buf_size - 
               ((hdrretap_user_data_type *) user_data_ptr)->
                  prev_pkt_size[carrier_id];

  /*------------------------------------------------------------------
   * Sanity check 1
   * the RETAP Pkt shall be generated 
   * 1. only in the connected state AND
   * 2. only when test pkt mode is enabled AND
   * 3. enough bandwith allocated from PCP (bigger then the header) 
   * 4. the current buf size for this link flow is nonzero
   * 5. the last PCP get packet has gather enough data
   *------------------------------------------------------------------ */

  if (( hdrretap.state != HDRRETAP_TRAFFIC ) ||
      ( !hdrretap.configuration.test_pkt_mode ) ||
      ( max_data_size < HDRRETAP_PL_23_TEST_PKT_MIN_SIZE_BYTE ) ||
      ( f23_cfg_ptr->flow_config[i].current_buf_size == 0 ) ||
      ( bufsz_left <= 0 ))
  {  

    HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR," RETAP: unexpected PCPGetPkt ssid=%d", 
                      f23_cfg_ptr->flow_config[i].substream_id );

    *tx_data_ptr = NULL;

    return 0;

  }

  /*------------------------------------------------------------------
   * Sanity check 2
   * If stats are available for this carrier id.
   *------------------------------------------------------------------ */

  stats = hdrretap_stats_get_from_carrier_id(carrier_id);
    /* Get pointer to stats for this carrier */

  if( stats == NULL )
  {  

    HDR_MSG_IS890_1 ( MSG_LEGACY_ERROR," RETAP: stats_get failed C=%d", 
                      carrier_id );

    *tx_data_ptr = NULL;

    return 0;

  }

  /*------------------------------------------------------------------
   * Sanity check 3
   * If a suitable DSM item is availabe
   *------------------------------------------------------------------ */

  /* Request a DSM item suits the current bandwith allocation */

  head_dsm_ptr = hdrutil_new_buffer( DSM_DS_POOL_SIZE( max_data_size + 2 ), 2);

  /* Sanity check the dsm ptr */

  if (( head_dsm_ptr == NULL )||
      ( head_dsm_ptr->size < 10 )||
      ( head_dsm_ptr->used != 0 ))
  {

    HDR_MSG_IS890 ( MSG_LEGACY_ERROR,
                    "RETAP: cannot alloc item or item is too small" );
    *tx_data_ptr = head_dsm_ptr ;
    return 0;

  }

  /*---------------------------------------------------------------------
   * RETAP test packet format: 
   * spec 3-9 defines the formate of the RETAP Teset Packet
   * The AT txs theses packets on the channel RTC 

   * StreamID        :  2bits : xx
   * ProtocolID      :  2bits : 1
   * PacketType      :  4bits : 2
   * ------------------------------------ 1st byte
   * LinkFlowID      :  8bits : 0x??
   * ------------------------------------ 2nd byte
   * RevSysTime      : 17bit  : CDMA system Time in sub-frames when this
                                pkt is generated
   * PayloadSizeIndex:  8bit  : 
   * Seqno           : 32bit
   * CarrierID       :  4bit  : PL_3 only
   * reserved        : 3or7bit: PL_2 = 7-bit, PL_3 = 3-bit
   * ------------------------------------ 3rd - 10th byte
   * RandomFill      : Variable
   -------------------------------------------------------------------*/

  if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
  {
    packet_type = HDRRETAP_PL_2_TEST_PKT_TYPE;
  }
  else if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
  {
    packet_type = HDRRETAP_PL_3_TEST_PKT_TYPE;
  }
  else
  {
    packet_type = HDRRETAP_PL_3_TEST_PKT_TYPE;
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR, "RETAP: Invalid subtype %d",
                     hdrretap.configuration.phy_subtype );
  }

  /* 1st byte for the protocol ID and Packet Type    */

  *( head_dsm_ptr->data_ptr ) = HDRRETAP_PROTOCOL_ID << 4;
  *( head_dsm_ptr->data_ptr ) |= packet_type ;
  head_dsm_ptr->used ++ ;

  /* 2nd byte is for the linkflow id/substream_id */

  *( head_dsm_ptr->data_ptr + 1 ) =
    hdrretap.configuration.config_pl_23.flow_config[i].substream_id;
  head_dsm_ptr->used ++ ;
 
  /* PL_2RevSysTime in subframe */ 

  hdrts_get_cycle_and_slot( current_slot_time,
                            &current_lap,
                            1 );

  qw_shift( current_slot_time , -2 );
  pack_time = qw_lo( current_slot_time ) & 0x1ffff;
    /* Per standard to mod 0d131072/0x20000 */ 

  bit_offset = 16;
    /* start packing from here */

  if ( dsmbit_pack32_tail( head_dsm_ptr ,
                           pack_time,
                           bit_offset,    
                           17 )  
        != 17 )
  {
    ERR_FATAL( "RETAP: Could not pack test pkt", 0, 0, 0 );
  }

  bit_offset += 17;    
  
#ifdef FEATURE_HDR_REVA_L1
  curr_payload_size_index = hdrrmac_get_selected_ps_index(carrier_id);
#else
  curr_payload_size_index = 0;
#endif /* FEATURE_HDR_REVA_L1 */

  if ( dsmbit_pack32_tail( head_dsm_ptr,
                           curr_payload_size_index,
                           bit_offset,    
                           8 )  
        != 8 )
  {

    ERR_FATAL( "RETAP: Could not pack test pkt", 0, 0, 0 );

  }

  bit_offset += 8;    

  if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
  {

    /* Packing the seqno for the correspoding payload size index
    * based on the standard, the seq no should include the current
    * one in build. This is the reason of the "+1"  */

    if ( dsmbit_pack32_tail( 
           head_dsm_ptr,
           stats[curr_payload_size_index].seqno + 1,
           bit_offset,
           32 ) != 32 )
    {
      ERR_FATAL( "RETAP: Cannot pack test pkt", 0, 0, 0 );
    }
    bit_offset += 32;

    reserve_bits = HDRRETAP_PL_2_TEST_PKT_RESERVE_BITS;
  }
  else if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
  {
    if( carrier_id >= HDRIS890B_NUM_CARRIERS ) 
    {
      ERR_FATAL( "Invalid carrier_id %d", carrier_id, 0, 0 );
    }

    /* Packing the seqno for the correspoding payload size index
    * based on the standard, the seq no should include the current
    * one in build. This is the reason of the "+1"  */
  
    if ( dsmbit_pack32_tail( 
           head_dsm_ptr,
           stats[curr_payload_size_index].seqno + 1,
           bit_offset,
           32 ) != 32 )
    {
      ERR_FATAL( "RETAP: Cannot pack test pkt", 0, 0, 0 );
    }
    bit_offset += 32;
  
    if ( dsmbit_pack32_tail( 
           head_dsm_ptr,
           carrier_id, 
           bit_offset,
           4 ) != 4 )
    {
      ERR_FATAL( "RETAP: Cannot pack test pkt", 0, 0, 0 );
    }
    bit_offset += 4;

    reserve_bits = HDRRETAP_PL_3_TEST_PKT_RESERVE_BITS;
  }
  else
  {
    reserve_bits = HDRRETAP_PL_2_TEST_PKT_RESERVE_BITS;
    HDR_MSG_IS890_1 (MSG_LEGACY_ERROR, "RETAP: Invalid subtype %d",
                     hdrretap.configuration.phy_subtype );
  }

  /* Packing the reserve bits with zeros */

  if ( dsmbit_pack8_tail(
         head_dsm_ptr, 
         0x0,
         bit_offset,
         reserve_bits )
       != reserve_bits )
  {
    ERR_FATAL( "RETAP: Cannot pack test pkt", 0, 0, 0 );
  }
  bit_offset += reserve_bits;

  head_dsm_ptr->used = HDRRETAP_PL_23_TEST_PKT_HEADER_LEN;
    /* 2 + 2 + 4 + 8 + 17 + 8 + 32 + 7 totaly 80 bits = 10 bytes*/

  /*------------------------------------------------------------------
   * Initialization before the filling loop
   *------------------------------------------------------------------ */

  bandwz_left = max_data_size - HDRRETAP_PL_23_TEST_PKT_HEADER_LEN;
    /* Bandwith granted and available for the PN filling */

  cur_dsm_ptr = head_dsm_ptr;
    /* Current filling DSM item */

  filling_len = 0;
    /* initial filling length */

  /*------------------------------------------------------------------
   * Create an DSM list to use up all the available bandwith if the 
   * current buffer size allows
   *------------------------------------------------------------------ */

  while ( bandwz_left > 0 ) 
  {

    if ( cur_dsm_ptr->size <= cur_dsm_ptr->used )
    {
      /* If the current dsm_item is used up then get a new dsm item */

      nxt_dsm_ptr = hdrutil_new_buffer( DSM_DS_POOL_SIZE( bandwz_left ), 0);

      if (( nxt_dsm_ptr == NULL ) ||
          ( nxt_dsm_ptr->size == 0 ) ||
          ( nxt_dsm_ptr->used != 0 ))
      {
        ERR_FATAL("DSM alloc did not succeed!",0,0,0);
      }

      /* chain to the linklist */

      cur_dsm_ptr->pkt_ptr = nxt_dsm_ptr;
      cur_dsm_ptr = nxt_dsm_ptr;

    } /* if ( cur_dsm_ptr->size <= cur_dsm_ptr->used ) */

    filling_len = (( cur_dsm_ptr->size - cur_dsm_ptr->used) < bandwz_left )? 
                           ( cur_dsm_ptr->size-cur_dsm_ptr->used ) : bandwz_left;
      /* maximum available PN filling length for the current dsm item */

    /* Math explanation: Do not transmit more than the amount buf allowed */

    if ( bufsz_left < ( filling_len + max_data_size - bandwz_left - 
                                             HDRRETAP_PL_23_TEST_PKT_HEADER_LEN ))
    {

      filling_len =  bufsz_left - ( max_data_size - bandwz_left - 
                                             HDRRETAP_PL_23_TEST_PKT_HEADER_LEN );
    }

    (void) hdrretap_pl_23_pack_pnfill( cur_dsm_ptr, filling_len );
      /* PN fill */

    cur_dsm_ptr->used += filling_len;
      /* Increased the used field by the filling length */

    bandwz_left -= filling_len;
      /* Decrease the available bandwith */

    ASSERT( cur_dsm_ptr->used <= cur_dsm_ptr->size );
      /* Sanity check the dsm item usage */

    if (( max_data_size - HDRRETAP_PL_23_TEST_PKT_HEADER_LEN - bandwz_left ) 
                                                                 >= bufsz_left )
    {
        /* Exit the loop when the buf has been drained */
        break;
    }

  } /* while ( bandwz_left > 0 ) */

  ((hdrretap_user_data_type *) user_data_ptr)->prev_pkt_size[carrier_id] += 
                 max_data_size - HDRRETAP_PL_23_TEST_PKT_HEADER_LEN - bandwz_left;
    /* The PCP might call more than one time per subframe */

  /*------------------------------------------------------------------
   * Return the DSM ptr and pkt size to the PCP
   *------------------------------------------------------------------ */

  HDR_MSG_IS890_8 (MSG_LEGACY_MED,
            "RETAP: PCPGetPkt: ssid%d, fl%d, used%d, spls%d, pt0x%05x-%x-%x-%x",
            f23_cfg_ptr->flow_config[i].substream_id,
            filling_len,
            head_dsm_ptr->used,
            curr_payload_size_index,
            pack_time,
            head_dsm_ptr->data_ptr[2],
            head_dsm_ptr->data_ptr[3],
            head_dsm_ptr->data_ptr[4] );

  *tx_data_ptr = head_dsm_ptr ;

  return ( max_data_size - bandwz_left);
    /* Returning the filling length plus the header length 10 bytes */

} /* hdrretap_pl_23_pcp_get_pkt */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_PARAM_ASSIGN_CB

DESCRIPTION
  This is the callback function that SLP calls when it receives the SLP ack
  for ParameterAssignmentComplete msg send to the Base Station.
  The call back will:
  1. copy the new configuration to RETAP_configuration
  2. register the transmition call back for each tx flow

DEPENDENCIES
  None.

PARAMETERS
  data_ptr    - pointer to the new configuration
  status      - message transmission status

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hdrretap_param_assign_cb
(

  void                *data_ptr,
    /* pointor to the new configuration */

  hdrerrno_enum_type  status
    /* param assingment response message tx status */
)
{

  uint8                                 i;
  uint8                                 rtcmac_flow_id;
  hdrretap_phy_subtype_enum_type        phy_subtype;
  hdrretap_pl_01_config_type            *f01_cfg_ptr;
  hdrretap_pl_23_config_type            *f23_cfg_ptr;
  boolean                               reg_result = TRUE;
  boolean                               disable_throttling = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( data_ptr != NULL );
    /* Sanity Check to the data_ptr */

  /* set the configuration pointors */
  f01_cfg_ptr = & (hdrretap.configuration.config_pl_01 );
  f23_cfg_ptr  = & (hdrretap.configuration.config_pl_23 );

  /*------------------------------------------------------------------
   * Wait until  we received the SLP ack before we make the new
   * parameters take effect. 
   * the old design does not do this ... we might need discussion
   * for me, it does not looks wrong at this moment
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
   * PCP clean up, deregister all the transmission call backs to the 
   * PCP; 
   * for the fsubtype01, the substream_id is zero; 
   * for the fsubtype2, the unregistreation has to be per substream 
  ------------------------------------------------------------------*/

  hdrretap_clear_transmit_callbacks();
    /* unregister the previouse tx call backs to the PCP*/

  /*------------------------------------------------------------------
   * The following is being done in  HDR RX task context 
  ------------------------------------------------------------------*/

  /* store the configuration into the hdrretap.configuration */

#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( & ( hdrretap.configuration ),
           sizeof(hdrretap_config_type), 
           data_ptr,
          sizeof( hdrretap_config_type ) );
#else
  memcpy( & ( hdrretap.configuration ),
          data_ptr,
          sizeof( hdrretap_config_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */


  /*------------------------------------------------------------------
   * Sanity check if the stream_id and phy_subtype is the same as before.
   * It seems they should not be changed in this scenario
  ------------------------------------------------------------------*/

  phy_subtype = (hdrretap_phy_subtype_enum_type)hdrscp_get_current_subtype( 
                                    HDRHAI_PHYSICAL_LAYER_PROTOCOL );

  if ( phy_subtype != hdrretap.configuration.phy_subtype ) 
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "RETAP: Unexpected phy subtype change; reread! " );
    hdrretap.configuration.phy_subtype = phy_subtype;
  }

  /*------------------------------------------------------------------
   * Display the mode configuration through the F3 message 
   * register the transmission call back to the PCP
  ------------------------------------------------------------------*/

  HDR_MSG_IS890_1 (MSG_LEGACY_HIGH, "+++++++++++ RETAP Config +++++++++++++"
                                    "+++ Stream %d", 
                                    hdrretap.configuration.stream_id);
    /* Stream id display */

  reg_result = TRUE;
                 
  if ( hdrretap.configuration.test_pkt_mode )
  {
    HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "+++ Test Pkt Mode enabled (pers. %d)",
              hdrretap.configuration.test_pkt_mode_config);

    /* Update srch to disable the TC OFS; this function is called in both TAP
     * START and STOP message; SRCH reset its own flag every time when srch 
     * transition from 3 -> 4; for a special case START is followed by another
     * START message, the TC_OFS can not be enabled;
     * To enable the TC_OFS, the AT has to go back to the idle state, set
     * the dip switch then run the TAP test */

    if ( hdrretap.configuration.tc_ofs_enabled == FALSE )
    {
      hdrlmac_disable_tc_ofs( );
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "RTAP: Disable TC OFS for the TAP call!");
    }
    else
    {
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "RTAP: TC OFS not disabled!");
    }

    hdrlmac_set_tap ( TRUE );
      /* Notify HDR srch that TAP is running */

#ifdef FEATURE_HDR_ETAP_DISABLE_CTA_TIMER
    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "RETAP: disable CTA timer at AssigmentMsg");
    (void) dsrlp_enable_inactivity_timer( DSRLP_RTAP_ID, FALSE );
      /* Disable the CTA timer at RLP */
#endif /* FEATURE_HDR_ETAP_DISABLE_CTA_TIMER */

    /* mode config status with PHY subtype 01 */

    if ( (hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 ||
          hdrretap.configuration.phy_subtype == PHY_SUBTYPE_1 ) )
    {

      /* PHY Subtype_0_1 */

      if ( f01_cfg_ptr->configured_pkt_rate_mode )
      {
        HDR_MSG_IS890_4 (MSG_LEGACY_HIGH,  "+++ Pkt Rate Mode enabled "
                                         "+++    Min Rate %d.%d "
                                         "+++    Max Rate %d.%d ",
     hdrretap.pl_01_stats[f01_cfg_ptr->pkt_rate_mode_config.min_rate].rate,
     hdrretap.pl_01_stats[f01_cfg_ptr->pkt_rate_mode_config.min_rate].rate_dec,
     hdrretap.pl_01_stats[f01_cfg_ptr->pkt_rate_mode_config.max_rate].rate,
     hdrretap.pl_01_stats[f01_cfg_ptr->pkt_rate_mode_config.max_rate].rate_dec);

        disable_throttling = TRUE;
          /* Disable the rate throttling at the rtcmac for the packet rate mode */
      }
      else
      {
        HDR_MSG_IS890 ( MSG_LEGACY_HIGH,
                     "+++ Pkt Rate Mode disabled, rtcmac free running" );
      }

      rtcmac_flow_id = ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 )?
                        HDRPCP_RMAC0_FLOW_ID: HDRPCP_RMAC1_FLOW_ID ;

      /* Register the transmission call back here */

      reg_result = 
      hdrpcp_register_transmit_callbacks(
            0,                                  /* substream id */
            rtcmac_flow_id,                     /* default mac flow id for RTCMAC01*/ 
            hdrretap.configuration.stream_id,   /* stream_id */
            hdrretap_pl_01_pcp_get_buf_size,    /* get_data_size_func_ptr not used */
            NULL,                               /* get_hp_data_func_ptr */
            hdrretap_pl_01_pcp_get_pkt,         /* get_lp_data_func_ptr */
            hdrretap_pl_01_pcp_pkt_status,      /* packet_status_func_ptr */
            NULL,                               /* packet_fail_func_ptr */
            TRUE,                               /* always_notify_packet_status */
            &( f01_cfg_ptr->user_data ) );      /* user_data_ptr */ 

      if ( reg_result ) 
      {
        f01_cfg_ptr->if_tx_rgsted = TRUE;
        HDR_MSG_IS890 ( MSG_LEGACY_HIGH,
                      "+++ a transmission call back is registerred to PCP " );
      }
      else
      {
        f01_cfg_ptr->if_tx_rgsted = FALSE;
        HDR_MSG_IS890 ( MSG_LEGACY_ERROR,
                      "+++ a transmission call back fails to register to PCP " );
      }
    } /* PHY_SUBTYPE_0 || PHY_SUBTYPE_1 */

    else if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
    {
       /* PHY subtype 2 */ 

      if ( f23_cfg_ptr->payload_size_mode_config.configured )
      {
        HDR_MSG_IS890_2 ( MSG_LEGACY_HIGH,  "+++ Pkt payload size Mode enabled "
                                            "+++    Min payload size %d "
                                            "+++    Max payload size %d ",
                              f23_cfg_ptr->payload_size_mode_config.min_index,
                              f23_cfg_ptr->payload_size_mode_config.max_index );

        disable_throttling = TRUE;
          /* Disable the rate throttling at the rtcmac for the configured
           * payload size mode */
      }
      else 
      {
        HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ payload size Mode disabled "
                                         "+++ RTCMAC3 free running ..."); 

        (void) hdrrmac_set_target_ps_index( HDRRETAP_DEFAULT_CARRIER, 
          HDRRMAC_IS856_RRI );
          /* Allow RMAC to determine the rate */

        }
    }
    else if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
    {

       /* PHY subtype 3 */ 

      for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ )
      {
        if ( f23_cfg_ptr->payload_size_mode_config_mc[i].configured )
        {
          HDR_MSG_IS890_5 ( MSG_LEGACY_HIGH,  
                            "+++ Pkt payload size Mode enabled RL %d "
                            "+++  C: %d  Min payload size %d "
                            "+++  C: %d  Max payload size %d ", 
                        i,
                        i,
                        f23_cfg_ptr->payload_size_mode_config_mc[i].min_index,
                        i,
                        f23_cfg_ptr->payload_size_mode_config_mc[i].max_index);
  
          disable_throttling = TRUE;
            /* Disable the rate throttling at the rtcmac for the configured
             * payload size mode */
        }
        else 
        {
          HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "+++ payload size Mode disabled "
                                           "+++ RTCMAC4 free running ... "); 
  
          (void) hdrrmac_set_target_ps_index( i, HDRRMAC_IS856_RRI );
            /* Allow RMAC to determine the rate, set explicitly here in case 
            some carriers enable/disable rate mode independently */
        }
      }
    }
    else
    {
      HDR_MSG_IS890_1 (MSG_LEGACY_ERROR, "RETAP: Invalid subtype %d",
                       hdrretap.configuration.phy_subtype );
    }

    if ( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ||
         hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
    {
      /* flow config status with PHY subtype 2 or 3 */

      if ( f23_cfg_ptr->num_flows != 0)
      {
        for ( i =0; i<f23_cfg_ptr->num_flows;i ++)
        {
          
          /* update the initial buffer_size and elaps_time right before the
           * the transmission call back registration */

          f23_cfg_ptr->flow_config[i].elaps_time = 0 ;
          f23_cfg_ptr->flow_config[i].current_buf_size =
            f23_cfg_ptr->flow_config[i].init_burst_size*1000 ;

          reg_result =
          hdrpcp_register_transmit_callbacks(
            f23_cfg_ptr->flow_config[i].substream_id,
            f23_cfg_ptr->flow_config[i].rtcmac_flow_id,
            hdrretap.configuration.stream_id,         /* buff_size_ptr */
            hdrretap_pl_23_pcp_get_buf_size,
            NULL,                                     /* hp_data_func_ptr */
            hdrretap_pl_23_pcp_get_pkt,               /* lp_data_func_ptr */
            hdrretap_pl_23_pcp_pkt_status,
            NULL,                                     /* packet_fail_func_ptr */
            TRUE,                                     /* always_notify_status */
            & ( f23_cfg_ptr->flow_config[i].user_data ) );

          if ( reg_result ) 
          {
            f23_cfg_ptr->flow_config[i].if_tx_rgsted = TRUE;

            HDR_MSG_IS890_4 ( MSG_LEGACY_HIGH, 
              "+++ fid%d, ssid:%d, bp:%x, bs:%x reged",
               i,
               f23_cfg_ptr->flow_config[i].substream_id,
               f23_cfg_ptr->flow_config[i].burst_period,
               f23_cfg_ptr->flow_config[i].init_burst_size );
          }
          else   /* reg_result */
          {
            f23_cfg_ptr->flow_config[i].if_tx_rgsted = FALSE;

            HDR_MSG_IS890_1 (MSG_LEGACY_ERROR, 
              "PCP reg failed for substream%d", 
              f23_cfg_ptr->flow_config[i].substream_id );
          }
        } /* for */
      }
      else  /* num_flows == 0 */
      { 
        HDR_MSG_IS890( MSG_LEGACY_ERROR,  "+++ no flow is configured " ); 
      }  

    } /* PHY_SUBTYPE_2_3*/
  }
  else  /* pkt_test_mode is off */
  {
#ifdef FEATURE_HDR_ETAP_DISABLE_CTA_TIMER
    HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "RETAP: Not disable CTA timer!");
    (void) dsrlp_enable_inactivity_timer( DSRLP_RTAP_ID, TRUE );
      /* Enable the CTA timer at RLP */
#endif /* FEATURE_HDR_ETAP_DISABLE_CTA_TIMER */

    HDR_MSG_IS890( MSG_LEGACY_HIGH,  "+++ Test Pkt Mode disabled" );

    for( i=0; i < HDRIS890B_NUM_CARRIERS;i++ )
    {
      if( i == HDRRETAP_DEFAULT_CARRIER ||
          hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
      {
        (void) hdrrmac_set_target_ps_index( i, HDRRMAC_IS856_RRI );
          /* Allow RMAC to determine the rate */
      }
    }
  }

  if (disable_throttling)
  {
    /* Disable throttling */
    hdrrmac_disable_throttling( HDRRMAC_RTAP_ID, TRUE );
  }
  else
  {
    /* Enable throttling */
    hdrrmac_disable_throttling( HDRRMAC_RTAP_ID, FALSE);
  }

  /*------------------------------------------------------------------
   * EACH rate mode display and config 
  ------------------------------------------------------------------*/
#ifdef FEATURE_HDR_REVA_L1
  if ( hdrretap.configuration.each_rate_mode ) 
  {
    /* AMAC i/f needs add 1 to match their enum type */
    hdramac_set_each_rate( (uint8) ( hdrretap.configuration.each_rate ) );

    HDR_MSG_IS890_1( MSG_LEGACY_HIGH, 
      "+++ each mode is enabled at rate %d", hdrretap.configuration.each_rate );
  }
  else
  {
    hdramac_set_each_rate( HDRAMAC_RETAP_EACH_RATE_MODE_DISABLED );
        
    HDR_MSG_IS890( MSG_LEGACY_HIGH, "+++ each mode is disabled" );
  }
#endif /* FEATURE_HDR_REVA_L1 */

} /* hdrretap_param_assign_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PROCESS_PARAMETER_ASSIGNMENT

DESCRIPTION
  This function handles the RETAP parameter assignment message.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - The pointer to the dsm item containng the incoming msg
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrretap_process_parameter_assignment
(
  dsm_item_type**  msg_ptr 
    /* the pointor to the dsm item containng the incoming msg */
)
{

  hdrretap_config_type                  new_configuration;
  dsm_item_type*                        reply_ptr = NULL;
  int32                                 i;
  int32                                 j;
  uint8                                 seqno;
  uint8                                 attribute_len;
  uint8                                 attribute_id;
  uint8                                 attribute_data;
  uint16                                attribute_data16, dummy;
  hdrretap_pl_01_config_type            *f01_cfg_ptr = NULL;
  hdrretap_pl_23_config_type            *f23_cfg_ptr  = NULL ;
  boolean                               new_flow_config = FALSE;
  uint8                                 rl_chan_count;
  uint8                                 rl_chan_idx;
  uint8                                 tca_msg_seq;
  uint8                                 carrier_index;
  uint16                                total_bits_remaining;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  total_bits_remaining = 
    ( dsm_length_packet( (*msg_ptr) ) * HDRRETAP_BITS_PER_BYTE );

  /* get the config ptrs */

  f01_cfg_ptr = &( new_configuration.config_pl_01 );
  f23_cfg_ptr = &( new_configuration.config_pl_23 );
  
  /* Get and check the validity of the seq no. of this msg */

  if ( dsm_pullup( msg_ptr, &seqno, 1 ) != 1 )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR,  "RETAP: Could not read seqno" );
    return;
  }

  /* We unpacked transaction ID already. Decrement the bits remaining */
  total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

  if ( (! hdrretap.configuration.param_assign_seqno_is_valid) ||
       (HDRIS890A_SEQNO_IS_VALID( seqno,
                                 hdrretap.configuration.param_assign_seqno )) )
      /* TD: the macro */
  {
    hdrretap.configuration.param_assign_seqno_is_valid = TRUE;
    hdrretap.configuration.param_assign_seqno = seqno;
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR,  
                     "RETAP: Rcvd bogus ParamAss. (%d); MSG dropped ", seqno );
    return;
  }

  HDR_MSG_IS890_1 (MSG_LEGACY_HIGH,  "RETAP: Rcvd ParamAss. (%d)", seqno);

  /* Disable throttling.  Throttling will be re-enabled in
     hdrretap_param_assign_cb if needed. */
  hdrrmac_disable_throttling( HDRRMAC_RTAP_ID, TRUE );

#ifdef FEATURE_HDR_DLNA
  /* Disable DLNA for RETAP test */
  hdrcmac_force_dlna_disable( HDRCMAC_RTAP_ID, TRUE );
#endif /* FEATURE_HDR_DLNA */

  memset( &new_configuration, 0, sizeof( hdrretap_config_type ));
    /* Clean the new configuration to zeros 
     * The memset turn off all the test mode */

  /* copy the fixed config from the current configuration */

  new_configuration.app_subtype = hdrretap.configuration.app_subtype;
  new_configuration.phy_subtype = hdrretap.configuration.phy_subtype;
  new_configuration.stream_id   = hdrretap.configuration.stream_id;
  f01_cfg_ptr->current_rate     = 
    hdrretap.configuration.config_pl_01.current_rate;
  f23_cfg_ptr->payload_size_mode_config.tgt_index        = 
    hdrretap.configuration.config_pl_23.payload_size_mode_config.tgt_index;

  /* Loop through the attributes in the message */

  while ( total_bits_remaining > 0 )
  {

    if( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
        ( dsm_pullup( msg_ptr, &attribute_len, 1 ) != 1 ) )
    {
      HDR_MSG_IS890( MSG_LEGACY_HIGH, "RETAP: Could not read attrib length" );
      return;
    }

    /* We unpacked Attribute length already. Decrement the bits remaining */
    total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

    if( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
        ( HDRRETAP_GET_ATTR(msg_ptr, attribute_id, 1 ) != E_SUCCESS ) )
    {
      HDR_MSG_IS890( MSG_LEGACY_HIGH, "RETAP: No more attributes found" );
      return;
    }
    
    /* We unpacked Attribute Id already. Decrement the bits remaining */
    total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

    /* get the attribute id */

    switch( attribute_id )
    {
      case HDRRETAP_ATTR_TEST_PKT_MODE:

        /* get the persistency bit */
        if( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
            ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR,  
                           "RETAP: Could not read attribute %d", 
                           attribute_id ); 
          return; 
        }

        /* We unpacked persistence bit already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        /* Sanity check on the persistency value */
        if ( attribute_data < TPM_RESERVED )
        {
          new_configuration.test_pkt_mode = TRUE;
          new_configuration.test_pkt_mode_config = 
            (attribute_data ? TPM_PERSISTENT
                            : TPM_NON_PERSISTENT );

          /* If we are switching into test packet mode reset 
           * the RETAP packet buffer and the packet counts. */
          if ( ! hdrretap.configuration.test_pkt_mode )
          {
            /* Packet Buffer & Stats init for phy subtype_01 */
            hdrretap.pl_01_test_pkt_buf.wr_index = 0;
            hdrretap.pl_01_test_pkt_buf.rd_index = 0;
            hdrretap.pl_01_test_pkt_buf.overflow = FALSE;

            for ( i=0; i<HDRRETAP_TEST_PKT_BUFFER_SIZE; i++ )
            {
              if ( hdrretap.pl_01_test_pkt_buf.pkt_ptr[i] != NULL )
              {
                dsm_free_packet( &(hdrretap.pl_01_test_pkt_buf.pkt_ptr[i]) );
              }
            }

            /* pkt_count is not cleared in hdrretap_stats_clear(), used only for 
              PL_01 where seqno is 12-bit */

            for ( i=0; i<HDRRETAP_NUM_REV_RATES_PL_01; i++ )
            {
              hdrretap.pl_01_stats[i].pkt_count = 0;
            }
          }
        }
        else
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Rcvd bogus Pers. value %d", attribute_data );
          return;
        }
        break;
      
      case HDRRETAP_ATTR_PKT_RATE_MODE:

        /* Get the minimum rate value */

        if( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
            ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }

        /* We unpacked minimum rate already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        f01_cfg_ptr->pkt_rate_mode_config.min_rate = attribute_data;
        f01_cfg_ptr->current_rate = 
          (attribute_data + HDRRETAP_NUM_REV_RATES_PL_01 - 1) % 
          HDRRETAP_NUM_REV_RATES_PL_01;
          /* store the min data rate and */

        /* Get the maximum rate value */

        if( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
            ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR,  
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }

        /* We unpacked maximum rate already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        f01_cfg_ptr->pkt_rate_mode_config.max_rate = attribute_data;

        /* Sanity check on the rate value */

        if ( hdrretap_sanitycheck_pkt_rate_mode_params( &new_configuration ) )
        {
          f01_cfg_ptr->configured_pkt_rate_mode = TRUE;
        }
        else
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                         "RETAP: Invalid for pkt rate mode parameters" );
          return;
        }
        break;
      

      case HDRRETAP_ATTR_PKT_PAYLOAD_SIZE_MODE:

        /* sanity check if fsubtype = 0x2 */

        if( HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_2 ) != E_SUCCESS )
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                         "RETAP:fsubtype/burst period mode mismatch; drop msg" );
          return; 
        }

        /* Get the minimum payload size */
        if ( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) ||  
             ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }

        /* We unpacked minimum payload size already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        f23_cfg_ptr->payload_size_mode_config.min_index = attribute_data;
        f23_cfg_ptr->payload_size_mode_config.tgt_index = attribute_data;
          /* current rate is set to the min rate for phy 2 and 3 */

        /* Get the maximum payload size value */

        if ( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
             ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }

        /* We unpacked max payload size already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        f23_cfg_ptr->payload_size_mode_config.max_index = attribute_data;
          /* Get the maximum minimum payload size */

        /* Sanity check on the params*/

        if ( hdrretap_pkt_size_mode_check( 
               &f23_cfg_ptr->payload_size_mode_config ) )
        {
          f23_cfg_ptr->payload_size_mode_config.configured = TRUE;
        }
        else
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR,  
                         "RETAP: Invalid range for payload size mode" );
          return;
        }

        /* If we got to here, the rate range is valid -> enable the mode */

        new_flow_config = TRUE;
          /* Mark that we need to validate config */

        break;

      case HDRRETAP_ATTR_BURST_PERIOD_MODE:

        /* sanity check if fsubtype = 0x2 or 0x3 */

        if( HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_2 ) != E_SUCCESS &&
            HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_3 ) != E_SUCCESS )
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                         "RETAP:fsubtype/burst period mode mismatch; drop msg" );
          return; 
        }

        /* Get the substream id in the attribute_data*/

        if( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
            ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }

        /* We unpacked substream id already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        /* Get the BurstPeriod in the attribute_data16*/

        if ( ( total_bits_remaining < (HDRRETAP_BITS_PER_BYTE * 2) ) ||
             ( HDRBIT_UNPACK16( ( *msg_ptr ), &attribute_data16, 0, 16 )
               != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }
        else
        {
          ( void )HDRRETAP_GET_ATTR(msg_ptr, dummy, 2 ); 
            /* move the data_ptr because unpack16 does not */
        }

        /* We unpacked BurstPeriod already. Decrement the bits remaining */
        total_bits_remaining -= (HDRRETAP_BITS_PER_BYTE * 2);

        /* Save into the substream table sanity check the flow id */

        if ( hdrretap_add_tx_flow_param( attribute_data,
                                         attribute_data16, 
                                         BP_MODE,
                                         f23_cfg_ptr ) )
        {
        }
        else
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                         "RETAP: Invalid bust period mode params" );
          return;
         }

        break;

      case HDRRETAP_ATTR_BURST_SIZE_MODE:

        /* Sanity check if fsubtype == 0x2 */

        if( HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_2 ) != E_SUCCESS &&
            HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_3 ) != E_SUCCESS )
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                        "RETAP:fsubtype/burst period mode mismatch; drop msg" );
          return; 
        }

        /* Get the link flow id*/

        if ( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) ||
             ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }

        /* We unpacked link flow id already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        /* Get the initial burst size id*/

        if ( ( total_bits_remaining < ( HDRRETAP_BITS_PER_BYTE * 2 ) ) ||
             ( HDRBIT_UNPACK16( ( * msg_ptr ), &attribute_data16, 0, 16 )
               != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }
        else
        {
          ( void )HDRRETAP_GET_ATTR(msg_ptr, dummy, 2 ); 
            /* move the data_ptr because unpack16 does not */
        }

        /* We unpacked initial burst size id already. Decrement the bits 
           remaining */
        total_bits_remaining -= (HDRRETAP_BITS_PER_BYTE * 2);

        /* Sanity check on the flow id and then
         * store the params in the flow table */

        if ( hdrretap_add_tx_flow_param( attribute_data,
                                         attribute_data16, 
                                         BS_MODE, 
                                         f23_cfg_ptr ) )
        {
          /* possible F3 here to indicate a new parmas comes in */
        }
        else
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                         "RETAP: Invalid burst size mode params" );
          return;
        }

        new_flow_config = TRUE;
            /* Mark that we need to validate config */

        break;    

      case HDRRETAP_ATTR_EACH_RATE_MODE:

        if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_0 )
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                         "RETAP:fsubtype/EACH rate mode mismatch; drop msg" );
          return; 
        }
          /* sanity check if fsubtype != 0x0 */

        /* Get the EACH rate index */

        if ( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) ||
             ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR,  
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }

        /* We unpacked rate index already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        if ( attribute_data >= EACH_RATE_INVALID )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR,  
                           "RETAP: EACH Rate Index (%d) is invalid", attribute_data ); 
          return;
        }
        new_configuration.each_rate = 
                               ( hdrretap_each_rate_enum_type ) attribute_data;
          /* fill the rate  */

        /* Sanity check on the each rate mode params*/

        if ( hdrretap_sanitycheck_each_rate_mode_params( &new_configuration ))
        {
          /* If we got to here, the params for this flow-> enable the mode */
          new_configuration.each_rate_mode = TRUE;
        }
        else
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR,  
                         "RETAP: Invalid fixed EACH rate mode params" );
          return;
        }
        break;

    case HDRRETAP_ATTR_PKT_PAYLOAD_SIZE_MODE_MC:

      /* sanity check if fsubtype = 0x3 */

      if( HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_3 ) != E_SUCCESS )
      {
        HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                       "RETAP:fsubtype/burst period mode mismatch; drop msg" );
        return; 
      }

      /* Get the TCAMsgSeq */

      if ( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
           ( HDRRETAP_GET_ATTR(msg_ptr, tca_msg_seq, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "RETAP: Could not read TCAMsgSeq %d", attribute_id );
        return; 
      }
      
      /* We unpacked tca msg seq already. Decrement the bits remaining */
      total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

      /* Get the RLChannelCount */

      if ( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) || 
           ( HDRRETAP_GET_ATTR(msg_ptr, rl_chan_count, 1 ) != E_SUCCESS ) )
      {
        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "RETAP: Could not read RLChannelCount %d", 
                         attribute_id );
        return; 
      }

      /* We unpacked RLChannelCount already. Decrement the bits remaining */
      total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

      for( i=0; i<rl_chan_count; i++ )
      {
        /* Get the RLChannelIndex */

        if ( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) ||
             ( HDRRETAP_GET_ATTR(msg_ptr, rl_chan_idx, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read RLChannelIndex %d", 
                           attribute_id );
          return; 
        }

        /* We unpacked RLChannelIndex already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        if( rl_chan_idx >= HDRIS890B_NUM_CARRIERS )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Invalid RLChannelIndex %d", 
                           rl_chan_idx ); 
          return; 
        }

        /* Get the MinPayloadSize */

        if( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) ||
            ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read MinPayloadSize %d", 
                           attribute_id );
          return; 
        }

        /* We unpacked MinPayloadsize already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        /* This function is called in order to have the list of
           carriers sorted by ascending order of frequencies */
        (void) hdrretap_get_rl_channel_idx_from_carrier_id(rl_chan_idx);

        /* Initializing to a high value */
        carrier_index = 0xff;

        /* Get the carrier index of the RLChannelIndex'th carrier in 
           the list of sorted carriers */

        for ( j = 0; j < HDRIS890B_NUM_CARRIERS; j++ )
        {
          if ( hdrretap.carrier_to_rl_channel_idx[j] == rl_chan_idx )
          {
            carrier_index = j;
            break;
          }
        }

        if ( carrier_index >= HDRIS890B_NUM_CARRIERS )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Invalid Carrier index %d", carrier_index);
          return; 
        }
        
        f23_cfg_ptr->payload_size_mode_config_mc[carrier_index].min_index = 
          attribute_data;
        
        f23_cfg_ptr->payload_size_mode_config_mc[carrier_index].tgt_index = 
          attribute_data;
          /* current rate is set to the min rate for phy 2 and 3 */
  
        /* Get the MaxPayloadSize value */
  
        if( ( total_bits_remaining < HDRRETAP_BITS_PER_BYTE ) ||
            ( HDRRETAP_GET_ATTR(msg_ptr, attribute_data, 1 ) != E_SUCCESS ) )
        {
          HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                           "RETAP: Could not read attribute %d", attribute_id ); 
          return; 
        }
  
        /* We unpacked MaxPayloadSize already. Decrement the bits remaining */
        total_bits_remaining -= HDRRETAP_BITS_PER_BYTE;

        f23_cfg_ptr->payload_size_mode_config_mc[carrier_index].max_index = 
          attribute_data;
          /* Get the maximum minimum payload size */
  
        /* Sanity check on the params*/
  
        if ( hdrretap_pkt_size_mode_check( 
               &f23_cfg_ptr->payload_size_mode_config_mc[carrier_index] ) )
        {
          f23_cfg_ptr->payload_size_mode_config_mc[carrier_index].configured =
            TRUE;
        }
        else
        {
          HDR_MSG_IS890( MSG_LEGACY_ERROR,  
                         "RETAP: Invalid range for payload size mode" );
          return;
        }
      }

      /* If we got to here, the rate range is valid -> enable the mode */

      new_flow_config = TRUE;
          /* Mark that we need to validate config */

      break;

    default:

        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                       "RETAP: received unknown attr. ID %d", attribute_id );
        return;

    } /* switch */

  } /* while we have bits left in the message */

  /* sanity check if the flow configuration is valid
   * 1. has flows configured for fsubtype_2
   * 2. the burstsize and burstperiod are in pair
   * 3. the burstsize and burstperiod are defined
   * otherwise the flow configuration is illegl and msg drop */

  if ( new_flow_config == TRUE )
  {
    if ( !hdrretap_sanitycheck_flow_config( &new_configuration ) )
    {
      HDR_MSG_IS890( MSG_LEGACY_ERROR, 
                     "RETAP: illegle flow configruation with . msg drop" );
      return;
    }
  }

  /* If we got a new test packet true setting, reset all seq. no's */

  if ( new_configuration.test_pkt_mode == TRUE )
  {

    hdrretap_stats_clear();
      /* Clear the statistics for a new test */

    qw_set( hdrretap.pl_01_prev_frame_time, 0, 0 );
      /* Set prev frame time to zero */
  }
  else if ( hdrretap.configuration.test_pkt_mode == TRUE )
  {
    hdrretap_stats_print();
      /* The test pkt mode is turned of in this assignment so print the stats */

    qw_set( hdrretap.pl_01_prev_frame_time, 0, 0 );
      /* Set prev frame time to zero */
  }

  /* If we made it till here, configuration was successful. send the 
   * CompleteMsg on TC only reliably.  */

  reply_ptr = dsm_new_buffer( DSM_DS_POOL_SIZE( 2 ) );
  if ( reply_ptr == NULL )
  {
    ERR_FATAL( "RETAP: Out of DSM items", 0, 0, 0 );
  }
  (void)dsmbit_pack8_tail( reply_ptr,
                     HDRRETAP_MSG_PARAMETER_COMPLETE,
                     0,
                     8 );
  (void)dsmbit_pack8_tail( reply_ptr,
                     seqno,
                     8,
                     8 );

  /* store the new config  */

#ifdef FEATURE_MEMCPY_REMOVAL
  hdrretap.stored_configuration  = new_configuration;
#else
  memcpy(&( hdrretap.stored_configuration ),
         &new_configuration,
         sizeof( hdrretap_config_type ) );
#endif /* FEATURE_MEMCPY_REMOVAL */

  /* Send the ParameterAssignmentComplete msg to AN using reliable tx */

  hdrhmp_app_send_msg( hdrretap.configuration.protocol_id, 
                       hdrretap.configuration.stream_id,
                       HDRHAI_RTC,
                       TRUE,
                       40,
                       HDRHAI_IN_USE,
                       reply_ptr,
                       hdrretap_param_assign_cb, 
                       &( hdrretap.stored_configuration ));

} /* static void hdrretap_process_parameter_assignment */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PROCESS_STATS_CLEAR_REQUEST()

DESCRIPTION
  This function handles the RETAP clear statistics request.

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrretap_process_stats_clear_request
(
  dsm_item_type**  msg_ptr 
    /* the ptr to the DSM item containing the incoming msg */
)
{

  dsm_item_type*  reply_ptr;
  uint8           seqno;
  uint8           stats_recid;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* sanity check: if !PHY_SUBTYPE_2, return w/o doing anything */

  if( HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_2 ) != E_SUCCESS &&
      HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_3 ) != E_SUCCESS )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "RETAP: unmatched fsubtype - drop msg" );
    return; 
  }

  /* Get and check the validity of this message */

  if ( dsm_pullup( msg_ptr, &seqno, 1 ) != 1 )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "RETAP: Could not read seqno" );
    return;
  }

  /* check the seq no */

  if ((! hdrretap.configuration.config_pl_23.stats_clear_is_valid ) ||
      ( HDRIS890A_SEQNO_IS_VALID( seqno,
              hdrretap.configuration.config_pl_23.stats_clear_seqno )) )
  {
    hdrretap.configuration.config_pl_23.stats_clear_is_valid = TRUE;
    hdrretap.configuration.config_pl_23.stats_clear_seqno = seqno;
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                     "RETAP: Rcvd bogus ClearStats. (%d)", seqno );
    return;
  }

  HDR_MSG_IS890_1( MSG_LEGACY_HIGH,  "RETAP: Rcvd ClearStats (%d)", seqno);

  /* get the StatisticsRecord ; and it should be 0x00 */

  if ( dsm_pullup( msg_ptr, &stats_recid, 1 ) != 1 ) 
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "RETAP: could ot read stats_recID" );
    return;
  }

  if ( ( stats_recid != 0x00 ) && ( stats_recid != 0x01 ) )
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                   "RETAP: wrong StatisticRecordID in the clearStats msg (%d)", 
                     stats_recid );
    return;
  }

  /* Message is valid, clear all the RETAP stats */ 

  hdrretap_stats_clear();

  /* If we made it til here, resetting the stats was successful, 
   * send the ConfirmationMsg on TC only reliably. */

  reply_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
  if ( reply_ptr == NULL )
  {
    ERR_FATAL( "RETAP: Out of DSM items", 0, 0, 0 );
  }

  ( void )dsmbit_pack8_tail( reply_ptr,
                             HDRRETAP_MSG_STATS_CLEAR_RESPONSE,
                             0,
                             8 );

  ( void )dsmbit_pack8_tail( reply_ptr,
                             seqno,
                             8,
                             8 );

  hdrhmp_app_send_msg_on_existing_rtc( 
                     hdrretap.configuration.protocol_id,
                     hdrretap.configuration.stream_id,
                     TRUE,
                     40,
                     HDRHAI_IN_USE,
                     reply_ptr,
                     NULL,
                     NULL );


} /* hdrretap_process_stats_clear_request */

/* <EJECT> */ 
/*===========================================================================
FUNCTION HDRRETAP_PROCESS_STATS_GET_REQUEST()

DESCRIPTION
  This function handles the RETAP statistics request.

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrretap_process_stats_get_request
(
  dsm_item_type**  msg_ptr 
    /* the dsm item pointor containing the incoming msg */
)
{

  dsm_item_type*      reply_ptr = NULL;
  uint8               i;
  uint8               seqno;
  uint8               num_flows;
  uint8               substream_id ;
  uint16              attribute_offset;
  qword               revtime;
  uint8               length = 0;
  uint8               stats_rec_id = 0;
  uint8               rl_channel_count = 1;
  uint8               rl_channel_idx;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------
   * Validate the Message
   *----------------------------------------------------------------*/

  /* Sanity check the PHY subtype */
  
  if( HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_2 ) != E_SUCCESS &&
      HDRRETAP_SANITY_CHECK_PHY_SUBTYPE( PHY_SUBTYPE_3 ) != E_SUCCESS )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR,
                   "RETAP: unmatched fsubtype - drop the statsreq msg" );
    return; 
  }
  
  /* Get and check the validity of this message */

  if ( dsm_pullup( msg_ptr, &seqno, 1 ) != 1 )
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR,  "RETAP: Could not read seqno" );
    return;
  }

  if (( !hdrretap.configuration.config_pl_23.stats_get_is_valid ) ||
      (HDRIS890A_SEQNO_IS_VALID( seqno,
          hdrretap.configuration.config_pl_23.stats_get_seqno )) )
  {
    hdrretap.configuration.config_pl_23.stats_get_is_valid = TRUE;
    hdrretap.configuration.config_pl_23.stats_get_seqno = seqno;
  }
  else
  {
    HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                     "RETAP: Rcvd bogus GetStats. (%d)", seqno );
    return;
  }

  /* sanity check num_flows */

  num_flows = hdrretap.configuration.config_pl_23.num_flows;
  if ( num_flows ==0 ||
       num_flows > MAX_NUM_RETAP_FLOW ) 
  {
    HDR_MSG_IS890( MSG_LEGACY_ERROR, "RETAP: illegal flow config " );
  }

  HDR_MSG_IS890_1( MSG_LEGACY_HIGH,  "RETAP: Rcvd GetStats (%d)", seqno);

  /*------------------------------------------------------------------
   * Prepare the header of the repsonse message
   *----------------------------------------------------------------*/

  reply_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );

  if ( reply_ptr == NULL )
  {
    ERR_FATAL( "RETAP: Out of DSM items", 0, 0, 0 );
  }

  /* Packing the protocol_id */

  ( void )dsmbit_pack8_tail( reply_ptr,
                             HDRRETAP_MSG_STATS_GET_RESPONSE,
                             0,
                             8 );

  /* Packing the transaction id */

  ( void )dsmbit_pack8_tail( reply_ptr,
                             seqno,
                             8,
                             8 );

  attribute_offset = 16;
   /* Protocol ID + Transaction ID */

  /*------------------------------------------------------------------
   * Prepare the attribute of the response message
   * in the RETAP StatsReqResp message, only 1 attribute is put in 
   * the message; 
   * Length         8bit    0x37
   * StatsRecId     8bit    0x00
   * LinkFlowId     8bit    the first element in the flow control table
   * PL_2RevSysTime 39bit   updated when a RTCMAC3 packet is accepted by
                            PHY subtype 2
   * reserved       1bit
   * seqno_0        32bit
   * ...
   * seqno_12       32bit
   *----------------------------------------------------------------*/

  if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
  {
    length        = HDRRETAP_PL2_STATS_RESPONSE_LENGTH;
    stats_rec_id  = HDRRETAP_PL_2_STATISTICS_RECORD_ID;
  }
  else if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
  {
    /* Calculate channel count */

    rl_channel_count = 0;

    for (i =0; i < HDRIS890B_NUM_CARRIERS; i ++ ) 
    {
      if( hdrretap_get_rl_channel_idx_from_carrier_id(i) !=
          HDRRETAP_INVALID_RL_CHANNEL_INDEX )
      {
        rl_channel_count++;
      }
    }

    /* Calculate length */

    length        = HDRRETAP_PL3_STATS_RESPONSE_HEADER_LENGTH +
      ( rl_channel_count * HDRRETAP_PL3_STATS_LENGTH );

    stats_rec_id  = HDRRETAP_PL_3_STATISTICS_RECORD_ID;
  }
  
  /* Packing the length */

  ( void )dsmbit_pack8_tail( reply_ptr,
                             length,
                             attribute_offset,
                             8 );
  attribute_offset += 8;

  /* packing the StatisticsRecordID */

  ( void )dsmbit_pack8_tail( reply_ptr,
                             stats_rec_id,
                             attribute_offset,
                             8 );
  attribute_offset += 8;

  substream_id = 
      hdrretap.configuration.config_pl_23.flow_config[0].substream_id;

  /* Packing the LinkFlowID */

  ( void )dsmbit_pack8_tail( reply_ptr,
                             substream_id,
                             attribute_offset,
                             8 );
  attribute_offset += 8;

  /* Packing the PL_2/3RevSysTime*/

  qw_shift( hdrretap.pl_23_stats_revsystime, 64-39 );

  qw_equ( revtime, hdrretap.pl_23_stats_revsystime );

  ( void )dsmbit_pack32_tail( reply_ptr,
                              qw_lo(revtime),
                              attribute_offset,
                              32 );
  attribute_offset += 32;

  ( void )dsmbit_pack8_tail( reply_ptr,
                             qw_hi(revtime),
                             attribute_offset,
                             7 );
  attribute_offset += 7;
  

  if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_2 ) 
  {
    attribute_offset += 1;
      /* Skip the reserved 1 bit */
  
    /* Packing the seq no for the payload_index from 0 to 12 */
  
    for (i =0; i < HDRRETAP_NUM_REV_PAYLOAD_SIZE; i ++ ) 
    {
      ( void )dsmbit_pack32_tail( reply_ptr,
                                  hdrretap.pl_2_stats[i].seqno,
                                  attribute_offset,
                                  32 );
      attribute_offset += 32;
    }

  }
  else if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
  {

    /* Packing the RLChannelCount */

    ( void )dsmbit_pack8_tail( reply_ptr,
                             rl_channel_count,
                             attribute_offset,
                             8 );

    attribute_offset += 8;

    attribute_offset += 1;
      /* Skip the reserved 1 bit */

    for( rl_channel_idx=0; rl_channel_idx<rl_channel_count;rl_channel_idx++ )
    {
      /* Packing the RLChannelIndex */

      ( void )dsmbit_pack8_tail( reply_ptr,
                               rl_channel_idx,
                               attribute_offset,
                               8 );
  
      attribute_offset += 8;

      /* Packing the seq no for the payload_index from 0 to 12 */
  
      for (i =0; i < HDRRETAP_NUM_REV_PAYLOAD_SIZE; i ++ ) 
      {
        ( void )dsmbit_pack32_tail( reply_ptr,
                                    hdrretap.pl_3_stats[rl_channel_idx][i].seqno,
                                    attribute_offset,
                                    32 );
        attribute_offset += 32;
      }
    }
  }
  
  /* Tx the response message through the SLP */
  hdrhmp_app_send_msg_on_existing_rtc( 
                       hdrretap.configuration.protocol_id,
                       hdrretap.configuration.stream_id,
                       TRUE,
                       40,
                       HDRHAI_IN_USE,
                       reply_ptr,
                       NULL,
                       NULL );

  hdrretap_stats_print();

} /* hdrretap_process_stats_get_request */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PROCESS_MSG

DESCRIPTION
  This function processes the pending message from the RETAP 
  message queue.

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrretap_process_msg( void )
{

  uint8  msg_id;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  while ( hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.read_index] != NULL )
  {
    if ( dsm_pullup( 
           &(hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.read_index]),
           &msg_id, 
           1 ) 
         != 1 )
    {
      HDR_MSG_IS890( MSG_LEGACY_ERROR,  "RETAP: Rcvd empty msg" );
      return;
    }
    
    switch( msg_id )
    {
      case HDRRETAP_MSG_PARAMETER_ASSIGNMENT:

        hdrretap_process_parameter_assignment(
          &(hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.read_index]) );
        break;

      case HDRRETAP_MSG_STATS_CLEAR_REQUEST:

        hdrretap_process_stats_clear_request( 
          &(hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.read_index]) );
        break;

      case HDRRETAP_MSG_STATS_GET_REQUEST:

        hdrretap_process_stats_get_request( 
          &(hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.read_index]) );
        break;

      default:

        HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                         "RETAP: Rcvd unknown msg id %d", msg_id );
        break;
        
    }
    
    /* After the processing the msg DSMs are freed here*/

    if ( hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.read_index] != NULL )
    {
      dsm_free_packet( 
        &(hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.read_index]) );
    }

    /* Advance the read index */

    hdrretap.msg_queue.read_index++;
    hdrretap.msg_queue.read_index %= HDRRETAP_MSG_QUEUE_SIZE;
  }

} /* static void hdrretap_process_msg( void ) */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_INIT_PNFILL_BUF()

DESCRIPTION
  The funciton reinit the PN fill buffer; the PN fill buffer is generated by
  a polynomrial generator X^15 + x + 1; the function uses the polynomial to
  generate the Pudorandom Fill pattern and store it in a static local array
  hdrretap.pnfill_buf

DEPENDENCIES
  None
 
PARAMETERS

RETURN VALUE
  how many bytes filled
  
SIDE EFFECTS
  None
===========================================================================*/
static void hdrretap_init_pnfill_buf ( void )
{

  uint8  j;
  uint16 i, shift_out_bit, shift_in_bit;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  uint16 generator_state = 0xffff;
    /* initial state of the generator 0xffff*/

  uint16  output_byte     = 0x0000;
    /* defined as uint16 for the bit operation */

  hdrretap.pnfill_buf.if_initialized = FALSE;
    /* flag to indicate if the buffer initialized */

  memset( hdrretap.pnfill_buf.pnfill, 0, sizeof( HDRRETAP_PNFILL_BUF_LENGTH ));
    /* clear the PN fill to zero */

  for(i = 0; i < HDRRETAP_PNFILL_BUF_LENGTH; i++)
  {
    for (j = 0; j < 8; j++) 
    {
      /* generate 8 bit for one storage */

      shift_out_bit = generator_state & 0x0001;
        /* output state[15]; perform right shift */

      shift_in_bit  = ( generator_state & 0x8000 ) ^ 
                  ( ( shift_out_bit << 15 ) ); /*lint !e734*/
      shift_in_bit &= 0x8000;
        /* input = state[15] XOR state[0] */

      generator_state = generator_state >> 1;
        /* right shift 1bit of the generator state */

      generator_state |= shift_in_bit;
        /* put the shift in bit */

      output_byte |= (shift_out_bit << j);
        /* acculmulat for byte output */
    }

    hdrretap.pnfill_buf.pnfill[i] = (uint8) output_byte;
      /* cast and store the byte */

    output_byte = 0x0;
  }

  hdrretap.pnfill_buf.if_initialized = TRUE;

} /* void hdrretap_init_pnfill_buf */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PUT_PKT

DESCRIPTION
  This function is called by hdris890a_put_pkt for TAP packets. This function
  does nothing and is expected to never be called.

DEPENDENCIES
  None
 
PARAMETERS
  rx_pkt_ptr  - Pointer to DSM chain containing received packet  
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_put_pkt
(
  dsm_item_type*   rx_pkt_ptr
    /* the incoming packt dsm pointor */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED( rx_pkt_ptr );
    /* mark the unused parameter for the lint sake */

  HDR_MSG_IS890( MSG_LEGACY_ERROR,  "RETAP: Rcvd unexpeced pkt !" );

} /* hdrretap_put_pkt */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_ACTIVATE

DESCRIPTION
  This function activates the RETAP once it has been negotiated
  by the stream protocol.

DEPENDENCIES
  None
  
PARAMETERS
  stream_id   - Stream ID negotiated for is890a
  app_subtype - Negotiated application subtype for TAP.

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_activate
(
  uint8  stream_id,
    /* stream ID */

  uint16 app_subtype
    /* application subtype */
)
{

  hdrretap_phy_subtype_enum_type  phy_subtype;
  uint8                           i;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED( app_subtype );
    /* mark the unused symbol to avoid lint error */

  memset( &hdrretap, 
          0, 
          sizeof( hdrretap_config_type ) );
    /* init the RETAP configuration and the old configuration is gone ifany*/

  hdrretap.configuration.stream_id = (hdrhai_stream_enum_type)stream_id;
    /* Store the stream ID negotiated for RETAP the stream id is used in the 
     * PCP registration */

  hdrretap.configuration.protocol_id = 
    (hdrhai_protocol_name_enum_type)hdris890a_prot_id[
      hdrretap.configuration.stream_id];

  hdrretap.configuration.phy_subtype = 
      (hdrretap_phy_subtype_enum_type)hdrscp_get_current_subtype( 
        HDRHAI_PHYSICAL_LAYER_PROTOCOL );
    /* retrieve the PHY subtype through the SCP interface */

  phy_subtype = hdrretap.configuration.phy_subtype;

  if (( phy_subtype == PHY_SUBTYPE_0 ) ||
      ( phy_subtype == PHY_SUBTYPE_1 ))
  {
    hdrretap.configuration.config_pl_01.current_rate 
       = HDRRETAP_NUM_REV_RATES_PL_01 - 1;
      /* Init the current rate to the maximum rate */

    memset( &( hdrretap.pl_01_test_pkt_buf ), 0, 
            sizeof( hdrretap_pl_01_test_pkt_buf_type ));
      /* Init the test pkt buffer */

    /* Create and init the static fill packet */

    if ( hdrretap.fill_pkt_ptr == NULL )
    {
      hdrretap.fill_pkt_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );

      if ( (hdrretap.fill_pkt_ptr == NULL) ||
           (hdrretap.fill_pkt_ptr->size < ( HDRRETAP_FILL_PKT_SIZE / 8 )))
      {
        ERR_FATAL( "RETAP: Coud not alloc fill packet", 0, 0, 0 );
      }

    }
    memset( hdrretap.fill_pkt_ptr->data_ptr, 0, ( HDRRETAP_FILL_PKT_SIZE / 8 ));
      /* Init to zero */

    hdrretap.fill_pkt_ptr->used = (HDRRETAP_FILL_PKT_SIZE / 8);
      /* The filling packet for pl_01 contents all zeros */

    /* Set RTAP Fill header and 2 bit StreamId 
       in the first byte of data_ptr as PCP Connection layer length field
       would be set in hdrpcp_build_packet */

    *( hdrretap.fill_pkt_ptr->data_ptr ) = ( stream_id << 6 )        |
                                            ( HDRRETAP_PROTOCOL_ID << 4) |
                                            HDRRETAP_FILL_PKT_ID;/*lint !e734*/ 

  }
  else if ( phy_subtype == PHY_SUBTYPE_2 || 
            phy_subtype == PHY_SUBTYPE_3 ) 
  {
    hdrretap.configuration.config_pl_23.payload_size_mode_config.tgt_index
                                  = HDRRETAP_NUM_REV_PAYLOAD_SIZE- 1;
    /* for PHY subtype02, no filling packet is needed ; but the reverse
     * test packet has a PN filling pattern */
    /* init pn fill circ buffer */

    hdrretap_init_pnfill_buf(); 

    hdrretap.configuration.config_pl_23.num_flows = 0;
  }
  else 
  {
    ERR_FATAL( "RETAP: Could not retrieve the correct PHY subtype", 0, 0, 0 );
  }
  
#ifdef FEATURE_MEMCPY_REMOVAL
  memscpy( hdrretap.pl_01_stats, 
           sizeof(hdrretap.pl_01_stats),  
           hdrretap_pl_01_stats_const, 
           HDRRETAP_NUM_REV_RATES_PL_01 * sizeof( hdrretap_stats_type ));
    /* Init the PL_0_1 statistics */

  memscpy( hdrretap.pl_2_stats, 
           sizeof(hdrretap.pl_2_stats), 
           hdrretap_pl_2_stats_const,
           HDRRETAP_NUM_REV_PAYLOAD_SIZE * sizeof( hdrretap_stats_type ));
    /* Init the PL_2 statistics */
#else
  memcpy( hdrretap.pl_01_stats, 
          hdrretap_pl_01_stats_const, 
          HDRRETAP_NUM_REV_RATES_PL_01 * sizeof( hdrretap_stats_type ));
    /* Init the PL_0_1 statistics */

  memcpy( hdrretap.pl_2_stats, 
          hdrretap_pl_2_stats_const,
          HDRRETAP_NUM_REV_PAYLOAD_SIZE * sizeof( hdrretap_stats_type ));
    /* Init the PL_2 statistics */
#endif /* FEATURE_MEMCPY_REMOVAL */

  for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ )
  { 
#ifdef FEATURE_MEMCPY_REMOVAL
    memscpy( hdrretap.pl_3_stats[i], 
            sizeof(hdrretap.pl_3_stats[i]), 
            hdrretap_pl_3_stats_const,
            HDRRETAP_NUM_REV_PAYLOAD_SIZE * sizeof( hdrretap_stats_type ));
#else
    memcpy( hdrretap.pl_3_stats[i], 
            hdrretap_pl_3_stats_const,
            HDRRETAP_NUM_REV_PAYLOAD_SIZE * sizeof( hdrretap_stats_type ));
#endif /* FEATURE_MEMCPY_REMOVAL */
  }
    /* Init the PL_3 statistics */

  /* Start in the IDLE state */

  hdrretap.state = HDRRETAP_IDLE;

  hdrretap.tca_seq_for_map = 0xFFFF;
    /* Initialize this to an invalid TCA sequence number value. */

  qw_set( hdrretap.pl_01_prev_frame_time, 0, 0 );
    /* Set prev frame time to zero so it will be ignored on the first update */

} /* hdrretap_activate */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_IS_IN_TRAFFIC

DESCRIPTION
  This function returns TRUE if the RETAP is negotiated on
  any stream AND in traffic state.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  TRUE or FALSE
  
SIDE EFFECTS
  None
===========================================================================*/
boolean hdrretap_is_in_traffic( void )
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ( hdrretap.state == HDRRETAP_TRAFFIC );

} /* hdrretap_is_in_traffic */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_DEACTIVATE

DESCRIPTION
  This function deactivates the RETAP and frees the fill packet.

DEPENDENCIES
  None
  
PARAMETERS
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_deactivate( void )
{

  uint8 i; 
  uint8 phy_subtype;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  phy_subtype = hdrretap.configuration.phy_subtype;

  if ( phy_subtype == PHY_SUBTYPE_0 || phy_subtype == PHY_SUBTYPE_1 ) 
  {
    /* Free the dsm item for the fill packet */

    dsm_free_packet( &hdrretap.fill_pkt_ptr );

    /* Free the dsm item test packet buffer incase 
     *  some pakcet left in the buffer */

    for ( i = 0; i < HDRRETAP_TEST_PKT_BUFFER_SIZE ; i++ ) 
    {
      if ( hdrretap.pl_01_test_pkt_buf.pkt_ptr[i] ) 
      {
        dsm_free_packet( &( hdrretap.pl_01_test_pkt_buf.pkt_ptr[i] ));
      }
    }
  }

  /* tx callback de-registration is done at the hdris890a_deactivate */

  hdrretap.state = HDRRETAP_INACTIVE;

} /* void hdrretap_deactivate( void ) */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PROCESS_IND

DESCRIPTION
  This function processes the indications for the RETAP.

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
void hdrretap_process_ind
(
  hdrind_ind_name_enum_type ind_name,
    /* indication name */

  void *ind_data_ptr 
    /* not used by RETAP */
)
{

  uint8                                 i;
  hdrretap_pl_23_config_type            *f23_cfg_ptr;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  SYS_ARG_NOT_USED( ind_data_ptr );
    /* mark the unreferenced symple to avoid lint error 751 */
  
  switch( ind_name )
  {
    /* State change related indications: */

    case HDRIND_INIT_NETWORK_ACQUIRED:

    case HDRIND_ALMP_RETURNING_TO_IDLE:

    case HDRIND_ALMP_REACQUIRING:

    case HDRIND_ALMP_EXIT_HDR_MODE:

      hdrretap.configuration.config_pl_01.configured_pkt_rate_mode = FALSE;
        /*packet rate mode is OFF */

      f23_cfg_ptr = &( hdrretap.configuration.config_pl_23 );
        /* Copy for coding sake */

      f23_cfg_ptr->payload_size_mode_config.configured = FALSE;
        /* payload size mode is OFF */

      for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ )
      {
        f23_cfg_ptr->payload_size_mode_config_mc[i].configured = FALSE;
          /* payload size mode is OFF */
        
        if( i == HDRRETAP_DEFAULT_CARRIER ||
            hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
        {
          (void) hdrrmac_set_target_ps_index( i, HDRRMAC_IS856_RRI );
            /* Allow RMAC to determine the rate */
        }
      }

      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "RETAP: State set to IDLE");
      hdrretap.state = HDRRETAP_IDLE;

      hdrrmac_disable_throttling( HDRRMAC_RTAP_ID, FALSE );
        /* Enable the throttle through RMAC once RTAP idle */

#ifdef FEATURE_HDR_DLNA
      hdrcmac_force_dlna_disable( HDRCMAC_RTAP_ID, FALSE );
        /* Remove forced DLNA disable */
#endif /* FEATURE_HDR_DLNA */

      /* Check if we should keep the test packet mode enabled */

      if ( hdrretap.configuration.test_pkt_mode_config != TPM_PERSISTENT )
      {

        hdrretap_clear_transmit_callbacks();
          /* Clear up the PCP transmit call backs if any */

        hdrretap.configuration.test_pkt_mode = FALSE;
      
      }

#ifdef FEATURE_HDR_ETAP_DISABLE_CTA_TIMER
      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "RETAP: Not disable CTA timer!");
      (void) dsrlp_enable_inactivity_timer( DSRLP_RTAP_ID, TRUE );
      /* Enable the CTA timer at RLP */
#endif /* FEATURE_HDR_ETAP_DISABLE_CTA_TIMER */

      break;

    case HDRIND_IDLE_CONNECTION_OPENED:

      HDR_MSG_IS890 (MSG_LEGACY_HIGH,  "RETAP: State set to TRAFFIC");

      qw_set( hdrretap.pl_01_prev_frame_time, 0, 0 );
        /* Set prev frame time to zero so it will be ignored on the first 
        update */

      hdrretap.state = HDRRETAP_TRAFFIC;

      break;

    case HDRIND_LMAC_CARRIER_MAP_CHANGED:

      if( hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 )
      {
        HDR_MSG_IS890( MSG_LEGACY_HIGH, "RFETAP: Carrier Map Changed " );
  
        hdrretap_stats_clear();
          /* Set seqno's to zero */
    
        hdrretap.configuration.config_pl_01.configured_pkt_rate_mode = FALSE;
          /*packet rate mode is OFF */
    
        f23_cfg_ptr = &( hdrretap.configuration.config_pl_23 );
          /* Copy for coding sake */
    
        f23_cfg_ptr->payload_size_mode_config.configured = FALSE;
          /* payload size mode is OFF */
    
        for( i=0; i<HDRIS890B_NUM_CARRIERS; i++ )
        {
          f23_cfg_ptr->payload_size_mode_config_mc[i].configured = FALSE;
            /* payload size mode is OFF */
    
          if( i == HDRRETAP_DEFAULT_CARRIER ||
              hdrretap.configuration.phy_subtype == PHY_SUBTYPE_3 ) 
          {
            (void) hdrrmac_set_target_ps_index( i, HDRRMAC_IS856_RRI );
              /* Allow RMAC to determine the rate */
          }
        }
    
        /* Check if we should keep the test packet mode enabled */
    
        if ( hdrretap.configuration.test_pkt_mode_config != TPM_PERSISTENT )
        {
          hdrretap.configuration.test_pkt_mode = FALSE;
        }
      }

      break;

    default:

      HDR_MSG_IS890_1( MSG_LEGACY_ERROR, 
                       "Unknown indication %d rcvd by RETAP", ind_name );

      break;

  }

} /* void hdrretap_process_ind */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to the RETAP.

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
void hdrretap_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
    /* Indication passed in */

  void                      *ind_data_ptr 
    /* Optional indication data */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrretap.state != HDRRETAP_INACTIVE )
  {

    /* This protocol runs in the RX and TX tasks. To make sure we update  
     * RETAP before any signaling data is sent, we process the indications
     * in the TX task context, which has the highest priority. */

    if ( rex_self() == HDRTX_TASK_PTR )
    {
      /* Indication is from another protocol in the TX task, so consider 
       * it an internal indication.   */

      hdrretap_process_ind( ind_name, ind_data_ptr );
    }
    else
    {
      hdrtx_queue_ind( HDRHAI_IS890A_RETAP,
                       ind_name, 
                       ind_data_ptr );

    }

  }

} /* void hdrretap_ind_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_MSG_CB

DESCRIPTION
  This function is called by hdris890a_msg_cb when it received an 
  RETAP message.

DEPENDENCIES
  None
 
PARAMETERS
  msg_ptr - DSM item chain containing the message
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_msg_cb
(
  dsm_item_type* msg_ptr
    /* The pointor of the DSM item containing the incoming msg */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.write_index] != NULL  )
  {

    HDR_MSG_IS890_3( MSG_LEGACY_ERROR,  "RETAP: Stale message ID 0x%x in %d",
         *(msg_ptr->data_ptr),
         hdrretap.msg_queue.write_index, 0 );

    dsm_free_packet( 
      &(hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.write_index]) );
  }

  hdrretap.msg_queue.msg_ptr[hdrretap.msg_queue.write_index] = msg_ptr;
    /* copy the msg ptr */

  hdrretap.msg_queue.write_index++;

  hdrretap.msg_queue.write_index %= HDRRETAP_MSG_QUEUE_SIZE;
    /* Advance the write index and check if there's a stale message left */

  ( void )rex_set_sigs( HDRTX_TASK_PTR, HDRRETAP_MSG_SIG );
    /* Set a signal to tx task to pass the RETAP msg */

} /* void hdrretap_msg_cb */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRRETAP_PROCESS_SIG

DESCRIPTION
  This function is the signal handler for the RETAP.

DEPENDENCIES
  None
 
PARAMETERS
  signal - REX signal
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void hdrretap_process_sig
(
  rex_sigs_type signal
    /* The incoming signal */
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch( signal )
  {

    case HDRRETAP_MSG_SIG:

      hdrretap_process_msg();
      break;

    default:

      HDR_MSG_IS890_1( MSG_LEGACY_ERROR,
                       "RETAP: Rcvd unknown signal %d", signal );
      break;

  }

} /* void hdrretap_process_sig */

/* <EJECT> */ 
/*===========================================================================

FUNCTION HDRFTAP_DISABLE_TC_OFS

DESCRIPTION
  The function modified the tc_ofs_enable based on the dip switch setting. 

DEPENDENCIES
  None
 
PARAMETERS
  None
 
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/

void hdrretap_disable_tc_ofs
(
  boolean tc_ofs_disable
)
{
  hdrretap.configuration.tc_ofs_enabled = !tc_ofs_disable;
}

#endif    /* FEATURE_HDR_IS890 || FEATURE_HDR_IS890A */
