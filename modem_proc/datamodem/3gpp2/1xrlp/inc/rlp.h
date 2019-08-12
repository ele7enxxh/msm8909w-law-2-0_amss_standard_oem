#ifndef RLP_H
#define RLP_H
/*===========================================================================

                                R L P 

DESCRIPTION
  This header file contains the definitions and interfaces for the Radio Link 
  Protocol (RLP) Data protocol unit of the DMSS software.

  ***************************************************************************
  NOTE: This file is included only when FEATURE_IS95B_MDR is not defined
  ***************************************************************************

   Copyright (c) 1993-2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/rlp.h_v   1.4   21 Jan 2002 15:06:52   sramacha  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/inc/rlp.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
28/10/11   ms      Fixed compiler errors when MDR feature is turned off. 
04/01/10   ms      CMI SU level API defeaturization.
09/04/09   ss      CMI SU level modifications.
10/30/07   Psng    Array over run issue has been addressed by adding RLP_RATE_MAX. 
12/15/05   TMR     Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
11/15/05   atp     Moved and redesigned ds_process_rlp_rx_frames().
04/05/05    gr     Fixed the issue of populating hdlc_inst_ptr for RLP1.
1/21/02    rsl     Added rlp_fwd_frame_block_type required for MUX processing 
                   when FEATURE_IS95B_MDR is undefined.
1/14/02    rsl     Remove ps.h to compile with FEATURE_IS95B_MDR undefined.
07/23/01   pjb     Change location/prefix of command names.   
08/17/99   smp     Removed include files msg.h & err.h.
01/29/99   hcg     Added PACKED qualifier to union inside rlp_gen_pri_fr_type
01/14/99   smp     Added #ifndef FEATURE_IS95B_MDR around the entire file, so
                   that this file is included only for non-MDR builds.
12/10/98   ldg     Removed PACKED qualifier from rlp_rx_frame_type.
09/22/98   ldg     ARM porting: T_ARM packing, ROM to ROM const.
12/18/97   ldg     Added new RLP stats: double & triple naks, largest
                   contig_erase.
09/09/97   ldg     Promoted some members of RLP UI stats struct from byte to
                   word.  Added count of all RLP frames.
07/09/97   na      Added RLP_BLANK rlp frame type for mux processing.
06/12/97   ldg     Added new FEATURE_DS_PSTATS
11/14/96   jjw     Rateset 2 interface upgrades
11/09/95   jjw     Completed changes to support fully compliant IS-99
01/22/95   jjw     Added capablity to register a data service & IS-99 structs
11/15/94   jjw     Added function and struct to convey RLP UI stats
10/18/94   jjw     DMSS code review changes
09/19/94   jjw     Minor change to function header
09/15/94   jjw     Upgrades to support IS-99 RLP
06/07/94   jjw     Pre code review upgrades
04/21/94   jjw     IS-99 RLP upgrades
05/06/93   jjw     Initial version. 

===========================================================================*/




/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/



#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_IS95B_MDR
#include "mdrrlp_api.h"
#endif
#ifndef FEATURE_IS95B_MDR
                              /* Only include this file for non-MDR builds */

#include "queue.h"
#include "dsm.h"
#include "dsrlp_api.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------


---------------------------------------------------------------------------*/
#define RLP_PRI_FRATE_BIT_SIZ      171   /* Primary Full Rate bit count    */
#define RLP_PRI_1_8TH_RATE_BIT_SIZ  16   /* Primary 1/8 Rate bit count     */
#define RLP_GEN_HRATE_BIT_SIZ       88   /* Generic Half Rate bit count    */
#define RLP_3_4TH_RATE_BIT_SIZ     128   /* 3/4ths Rate bit count          */
#define RLP_7_8TH_RATE_BIT_SIZ     152   /* 7/8ths Rate bit count          */
#define RLP_GEN_FRATE_BIT_SIZ      168   /* Generic Full Rate bit count    */


#define RLP1_FULL_PRI_DATA_BYTES        20
#define RLP2_FULL_PRI_CTL_UNUSED_BYTES  27   /* (264/8 - 6)                */
#define RLP2_FULL_GEN_CTL_UNUSED_BYTES  27   /* (262/8 - 6)                */
#define RLP2_FULL_GEN_DATA_BYTES        30
#define RLP2_FULL_PRI_DATA_BYTES        32
#define RLP2_7_8_GEN_CTL_UNUSED_BYTES   24   /* (242/8 - 6)                */

//#define RLP_TYPE_INDEX   33             /* Access TYPE field */

/*---------------------------------------------------------------------------
  Rate Sets 1 and 2 Primary channel full rate type field definitions.
---------------------------------------------------------------------------*/
#define RLP1_PRI_FULL_RATE_A    0x20     /* Generic Primary Full Rate type */
#define RLP1_PRI_FULL_RATE_B    0x40     /* Maximum payload (20) Full type */

#define RLP2_PRI_FULL_RATE_A    0x40     /* Generic Primary Full Rate type */
#define RLP2_PRI_FULL_RATE_B    0x80     /* Maximum payload (32) Full type */

/*---------------------------------------------------------------------------
  Definition for the number of Data frames to save after transmission for
  possible retransmission. Note that this number should be enough to account 
  for 3 round trip times (RLP_DELAY) plus some margin for queue and processing
  delays. The maximum is 128. NOTE: this value must be a power of 2.
---------------------------------------------------------------------------*/
#define RLP_WORST_CASE_SAVED_FRAMES  64

/*---------------------------------------------------------------------------
  Definition for the storage size of individual items of raw data bytes. Any
  arrays storing data processed from RLP RX should be sized as a multiple of
  this value.
---------------------------------------------------------------------------*/
#define RLP_DATA_STORAGE_SIZ  RLP_PRI_FRATE_DATA_SIZ

/*---------------------------------------------------------------------------
  Type for return value from RLP RX processing function.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_RX_SYNC_TIMEOUT,                 /* RLP timed out attempting to SYNC */
  RLP_RX_DATA_AVAILABLE,               /* RLP RX processing yielded data   */
  RLP_RX_NO_DATA_AVAILABLE,            /* RLP RX processing yielded no data*/
  RLP_RX_FRAME_ERR                     /* RX frame error occurred          */
} rlp_rx_status_enum_type;

/*---------------------------------------------------------------------------
  Type for RLP State.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_SYNC_STATE = 0,                /* Initial state                      */
  RLP_RT_MEAS_STATE,                 /* ACK received from the other side   */
  RLP_ESTABLISHED_STATE              /* Data/Idle received from other side */
} rlp_state_enum_type;

/*---------------------------------------------------------------------------
  Type to convey RLP stats to an external unit.
---------------------------------------------------------------------------*/
typedef struct
{
  word                rx_data_cnt;              /* count of RX data frames */
  word                tx_data_cnt;              /* count of TX data frames */
  byte                nak_cnt;                  /* count of NAKs sent      */
  byte                rexmit_cnt;               /* count of REXMITS sent   */
  rlp_state_enum_type state;                    /* current RLP state       */
  word rx_frame_cnt;                            /* count of all RX frames  */
  word tx_frame_cnt;                            /* count of all TX frames  */
} rlp_ext_stats_type;

#ifdef FEATURE_DS_PSTATS

/*---------------------------------------------------------------------------
  Type to index the array of RLP stats.
  These stats apply to the current or most recent call.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_STATS_RX_DATA_CNT,         /* # of data frames received      */
  RLP_STATS_TX_DATA_CNT,         /* # of data frames Transmitted   */
  RLP_STATS_NAKS,                /* count of NAKs sent             */
  RLP_STATS_DOUBLE_NAKS,         /* count of 2-in-a-row NAKs       */
  RLP_STATS_TRIPLE_NAKS,         /* count of 3-in-a-row NAKs       */

  RLP_STATS_REXMITS,             /* count of REXMITs sent          */
  RLP_STATS_SEQ_TIMEOUT_CNT,     /* count of sequence # timeouts   */
  RLP_STATS_RE_XMITS_MISSED,     /* count of REXMITs not all done  */
  RLP_STATS_RE_XMITS_NOT_FOUND,  /* count of REXMITs not found     */
  RLP_STATS_LARGEST_RE_XMIT,     /* largest REXMIT                 */
  RLP_STATS_FILL_FRAMES_RXED,    /* count of fill frames received  */

  RLP_STATS_IDLE_FR_ERR,         /* count of idle frame CRC errors */
  RLP_STATS_FULL_TYPE_ERR,       /* count of full rate type errors */
  RLP_STATS_RX_SEG_FR_ERRS,      /* count of errors in rxed segmented frames */
  RLP_STATS_ERASURES,            /* count of erasures              */
  RLP_STATS_LARGEST_CONTIG_ERASE,
                                 /* largest block of contig. erasures */
  RLP_STATS_GEN_RX_ERRS,         /* count of generic rx errors     */

  RLP_STATS_LAST_RTT,            /* last round-trip time           */
  
  RLP_STATS_RESETS,              /* count of RLP resets            */

  RLP_STATS_COUNT                /* number of RLP stats            */
} rlp_stats_enum;

/*---------------------------------------------------------------------------
  Array of pointers to stats names.
---------------------------------------------------------------------------*/
extern const byte * const rlp_stats_name_ptr[RLP_STATS_COUNT];

/*---------------------------------------------------------------------------
  Array of pointers to names of the last call ending state.
---------------------------------------------------------------------------*/
extern const byte * const rlp_last_call_synced[3];

/*---------------------------------------------------------------------------
  Pointer (into rlp_last_call_synced) to name of last call ending state.
---------------------------------------------------------------------------*/
extern byte * rlp_last_call_synced_ptr;

/*---------------------------------------------------------------------------
  Copy of the last round trip time.
---------------------------------------------------------------------------*/
extern word rlp_last_rtt;

#endif /* FEATURE_DS_PSTATS */

/*---------------------------------------------------------------------------
  Maximum number of forward supplemental channels supported.
---------------------------------------------------------------------------*/
#define RLP_MAX_FWD_SUP        7

/*---------------------------------------------------------------------------
  Types for the various IS-99 RLP Frames
---------------------------------------------------------------------------*/

typedef PACKED struct PACKED_POST              /* Generic Control frame type     */
{
  byte seq_num;                                /* Sequence number          */
  byte ctl_len;                                /* Control & length fields  */
  byte first_seq;                              /* 1st seq # for NAKs       */
  byte last_seq;                               /* last seq # for NAKs      */
  word crc;                                    /* Frame check Seq. CRC     */
  byte unused [RLP2_FULL_PRI_CTL_UNUSED_BYTES];/* unused bytes field       */
} rlp_gen_ctl_type;

typedef PACKED struct PACKED_POST              /* Generic Data frame type        */
{
  byte seq_num;                                /* Sequence number          */
  byte ctl_len;                                /* Control & length fields  */
  byte data [RLP2_FULL_GEN_DATA_BYTES];        /* data (payload) bytes     */
} rlp_gen_data_type;

typedef PACKED struct PACKED_POST              /* Generic Primary Full rate type */
{
  PACKED union PACKED_POST
  {
    rlp_gen_data_type  data;                   /* generic data frame type  */
    rlp_gen_ctl_type   ctl;                    /* generic ctrl frame type  */
  } info;
  byte                 type;                   /* frame type indicator     */
} rlp_gen_pri_fr_type;

typedef PACKED struct PACKED_POST              /* Idle (Eighth Rate) Frame Type  */
{
  byte   seq_num;                              /* Next Sequence number     */
  byte   nr_crc;                               /* Nordstrom-Robinson CRC   */
  byte   rs2_filler;                           /* 4 bits: Rate Set 2 only  */
} rlp_pri_idle_type;

typedef PACKED struct PACKED_POST              /* Full Rate Data Frame Type      */
{
  byte   seq_num;                              /* Sequence number of Frame */
  byte   data[RLP2_FULL_PRI_DATA_BYTES];       /* Data bytes               */
  byte   type;                                 /* frame type indicator     */
} rlp_pri_fr_data_type;

typedef PACKED union PACKED_POST               /* Union of all the RLP Frame Types   */
{
  byte                 seq_num;            /* Generic to get sequence #    */
  rlp_gen_data_type    gen_data;           /* Generic data frame           */
  rlp_gen_ctl_type     gen_ctl;            /* Generic control frame        */
  rlp_gen_pri_fr_type  pri_gen;            /* Generic primary full rate    */
  rlp_pri_fr_data_type pri_fr_data;        /* Primary full rate data frame */
  rlp_pri_idle_type    pri_idle;           /* Eighth Rate (Idle) Frame type*/
} rlp_gen_frame_type;

/*--------------------------------------------------------------------------
  Complete frame information type. 
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  rlp_rate_enum_type rate;              /* Frame rate                      */
  boolean            pri_frame;         /* Primary channel frame indicator */
  rlp_gen_frame_type frame;             /* Actual RLP frame information    */
} rlp_frame_type;

/*--------------------------------------------------------------------------
 Type for providing RLP frames with rate and channel type to RLP RX processing.
---------------------------------------------------------------------------*/

// Packing this struct causes problems with the array dsi_rlp_rx_fr_array
// since the link element ends up misaligned in items in the array.
//
// typedef PACKED struct

typedef struct
{
  q_link_type        link;              /* Link for queuing/unqueuing      */
  rlp_rate_enum_type rate;              /* Frame rate                      */
  boolean            pri_frame;         /* Primary channel frame indicator */
  dsm_item_type      *item_ptr;         /* Pointer to item with RLP frame  */
 } rlp_rx_frame_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION RLP_RX_PROCESS_FRAME

DESCRIPTION
  This function processes the passed CDMA frame as an IS-99 RLP data frame.
  All IS-99 RLP RX processing is performed as appropriate: frame validity 
  checks, control frame processing, sequence number checking, data frame 
  processing, and data frame dispatching.

  This function can operate on either primary or secondary CDMA channels.

DEPENDENCIES
  rlp_init must have been previously invoked.

RETURN VALUE
  The status resulting from the RLP RX processing.

SIDE EFFECTS
  None
===========================================================================*/

extern rlp_rx_status_enum_type rlp_rx_process_frame
(
  rlp_rate_enum_type frame_rate,              /* Rate of RLP frame         */
  boolean            pri_chan,                /* Primary Channel indicator */
  dsm_item_type      *rlp_item_ptr            /* ptr to RLP frame item     */
);

/*===========================================================================

FUNCTION RLP_RESET

DESCRIPTION
  This function performs the work needed to reset the RLP state information 
  in preparation for the next RLP call.

  This function should be called upon completion of a data call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rlp_reset( void);

/*===========================================================================

FUNCTION RLP_INIT

DESCRIPTION
  This function initializes the RLP state machine. Upon completion of this
  function the RLP protocol unit will be ready for operation.

DEPENDENCIES
  This function must be called prior to any other RLP function.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rlp_init( void);

/*===========================================================================

FUNCTION RLP_REG_SRVC

DESCRIPTION
  This function prepares the RLP unit to operate using the passed parameters
  for interfacing to the proper data services unit.

DEPENDENCIES
  No parameter field can be NULL.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rlp_reg_srvc
(
  dsm_watermark_type *tx_watermark_ptr,     /* ptr to TX watermark for flow*/
  void               (*post_rx_func_ptr)(uint32), /* func ptr for Post-RX procsg */
  dsm_watermark_type *post_rx_wm_ptr,         /* ptr to queue to put RX data */
  uint32              post_rx_func_cb_data
);

/*===========================================================================
FUNCTION      RLP_REG_HDLC

DESCRIPTION   Registers the HDLC Instance for the RLP instance

DEPENDENCIES
  No parameter field can be NULL.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void rlp_reg_hdlc
(
  void                *hdlc_inst_ptr
);

/*===========================================================================

FUNCTION RLP_GET_STATS

DESCRIPTION
  This function returns the current external statistics to the calling 
  function by storing the current items from internal RLP stat counters.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void rlp_get_stats
(
  rlp_ext_stats_type *stats_ptr      /* pointer to item to load with stats */
);


#ifdef FEATURE_DS_PSTATS
/*===========================================================================

FUNCTION RLP_GET_STATS_AT

DESCRIPTION
  This function copies the RLP stats into the given array.
  This particular function is for use with the AT commands.

DEPENDENCIES
  The given array must be of size RLP_STATS_COUNT or greater.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rlp_get_stats_at
(
  word *stats_ptr  /* pointer to array to load with stats */
);

/*===========================================================================

FUNCTION RLP_RESET_STATS

DESCRIPTION
 Resets the stat which can be reported using rlp_get_stats_at

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rlp_reset_stats(void);

#endif /* FEATURE_DS_PSTATS */

#endif /* !FEATURE_IS95B_MDR */

#endif /* RLP_H */
