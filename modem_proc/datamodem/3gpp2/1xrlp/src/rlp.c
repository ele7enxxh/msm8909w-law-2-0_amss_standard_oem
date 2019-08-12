/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                                  R L P

GENERAL DESCRIPTION
  This file contains all functions necessary for the IS-99 Radio Link Protocol
  (RLP) Data protocol. This includes operation as either a Primary or a 
  Secondary traffic channel service.

  This software unit does not support RLP over signaling channel(s).

  ***************************************************************************
  NOTE: This file is compiled in only when FEATURE_IS95B_MDR is NOT defined
  ***************************************************************************

EXTERNALIZED FUNCTIONS
  rlp_init()
    Initialize the RLP state machine. Must be called prior to any other
    RLP function.

  rlp_rx_process_frame()
    Perform RLP processing on the Received CDMA traffic channel frame.

  rlp_tx_get_next_frame()
    Retrieve the next RLP data frame to send out on the CDMA traffic channel 
    frame.

  rlp_reset()
    Reset all RLP state variables to prepare for next call.

  rlp_reg_srvc()
    Pass RLP the required parameters to perform RLP operations.

  ds_process_rlp_rx_frames()
   Called by mux every 20 msecs for RLP 1 & RLP 2 calls.  This function
   then passes each frame down to RLP and sets/clears the idle bits
   appropriately.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The RLP State machine has several requirements that must be followed in 
  order for the proper operation of the RLP protocol software.

  INITIALIZATION REQUIREMENTS:
  
  rlp_init() must be called prior to calling any other rlp function.

  rlp_reg_srvc() must be called using all parameters prior to using RLP as 
  a data service.

  rlp_reset() should be called at the end of each call.

Copyright (c) 1993-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: O:/src/asw/COMMON/vcs/rlp.c_v   1.3   08 Nov 2002 14:14:10   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/rlp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/14   sd      Fixed critical section release.
08/06/13   sd      Replaced memcpy with memscpy.
03/22/12   msh     KW warning fix 
12/16/11   ss      Compiler Warnings fixed.
10/28/11   ms      Fix compiler errors when MDR feature is turned off. 
01/18/11   op      Migrated to MSG 2.0 Macros
11/22/10   sn      Replaced REX premption macros with critical section for 
                   mutual exclusion.
04/01/10   ms      CMI SU level API defeaturization.
01/20/09   pp      Metainfo clean-up.
10/29/07   psng    Specified array "rlp_gen_rx_errs" & "rlp_rx_stat" to 
                   appropriate size.
07/31/06   as      Fix watermark type declaration bug
12/15/05   TMR     Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
11/15/05   atp     Moved and redesigned ds_process_rlp_rx_frames().
04/05/05    gr     Fixed the issue of populating hdlc_inst_ptr for RLP1.
02/03/04    ak     Meta info now inside dsm item data block, so extract & 
                   pull up appropriately.
11/08/02   atp     Added freeing of meta_info to prevent memory leak.
1/14/02    rsl     Added ps.h to compile with FEATURE_IS95B_MDR undefined.
08/08/01   pjb     Moved of kinds from dsm to ps caused name change
08/25/00   rc      Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK
06/07/00   rc      Removed ovld of link ptr in dsm items.
03/08/00   igt     added a memset() to fill zeroes into control frames
12/27/99   na      Changed q_put, q_insert & q_next to use the ovld of the 
                   link ptr in dsm items.
08/17/99   smp     Added include files msg.h & err.h.
07/08/99   smp     The compiler warning "actual_fr_payload may be used before
                   being set" turned out to be a real bug. Fixed the code to
                   ensure that actual_fr_payload is set for all the possible
                   cases, in rlp_tx_get_next_frame().
04/09/99   hcg     Added FEATURE_Q_NO_SELF_QPTR to support queue services memory 
                   optimization.
04/06/99   smp     Included comdef.h before the #ifdef that surrounds the
                   entire file, in order to fix a compiler warning (no external
                   declaration in translation unit).
03/25/99   smp     Initialized actual_fr_payload in rlp_tx_get_next_frame() to
                   zero, to fix a compiler warning ('actual_fr_payload' may be
                   used before being set).
01/13/99   smp     Added #ifndef FEATURE_IS95B_MDR around the entire file, so
                   that this file is compiled in only for non-MDR builds.
12/14/98   ldg     In queue_nak_frames() protect nak_frame_cnt++ with
                   INTLOCK (ARM porting).
12/12/98   ldg     Added explicit casts to avoid "loses __packed" error (ARM)
10/27/98   ldg     ARM porting changes: ROM to ROM const.
09/15/98   ldg     When a triple nak occurs, properly increment triple nak
                   counter instead of double nak counter.
12/18/97   ldg     Added new RLP stats: double & triple naks, largest
                   contig_erase.
                   Fixed rlp_reset_stats() to reset largest_re_xmit.
12/15/97   ldg     RLP_STATS_GEN_RX_ERRS total was leaving out RLP_FULL_RATE;
                   fixed this.  Fixed comment for computation of
                   RLP_STATS_ERASURES.
12/12/97   ldg     Resets count now incremented only for anomaly resets.
12/03/97   na      RLP was adding an extra byte to generic full rate 
                   RS2 frames of payload size 20 (max. Rateset1 size). Fixed.
11/09/97   na      Fixed bug - ds_last_call_var was being reset was RLP 
                   resets.
11/06/97   ldg     Removed externs.
10/21/97   ldg     Changed AT stats accessor function to compute the
                   erasures count from the proper source.  Changed stats reset
                   function to reset more stats.
10/16/97   ldg     Changed UI stats accessor function to return the total
                   number of frames since reset (not just since the last
                   invocation of accessor function).
10/16/97   na      Fixed increments of conseq. erasure count as specified
                   in the frame validity checks section of IS707.
09/26/97   na      Increased SYNC_TIMEOUT for rlp.
09/09/97   ldg     Added counter for RLP resets.  Changed UI stats accessor
                   function to return the number of frames elapsed since
                   the last call.
07/09/97   na      Fixed Erasure processing bug.
06/17/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
06/12/97   ldg     Added new FEATURE_DS_PSTATS
06/05/97   na      Added support for intersegment fill frames.
04/01/97   na      Fixed incorrect mask in rateset2 idles.
02/13/97   na      CTIA show build.
01/23/97   na      Added support for UI last call status function.
01/18/97   jjw     1st checkin of Rate Set 2 compatible RLP.
11/14/96   jjw     (Nish) made mods to operate with newer (rateset 2) header
                   file
11/09/95   jjw     Completed changes to support fully compliant IS-99
07/13/95   jjw     Changes to support IS-99 protocol integration
04/05/95   jjw     Mod'ed to use changes to DSM memory pool
01/22/95   jjw     Added stuff to support complete IS-99 data service
11/15/94   jjw     Added interface for RLP UI stats
10/18/94   jjw     DMSS code review changes
09/15/94   jjw     Upgrades for complete IS-99 RLP
08/08/94   jjw     Tune up for code review, round 1
04/21/94   jjw     Modified to support IS-99 RLP frame formats and secondary
                   channel frame formats.
05/06/93   jjw     Initial version. Migrated from the work of Phil Karn.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_IS95B_MDR
#include "mdrrlp_api.h"
#endif /* FEATURE_IS95B_MDR */
#include "msg.h"
#include "data_msg.h"
#ifdef FEATURE_DS

#ifndef FEATURE_IS95B_MDR
                              /* Compile this file only for non-MDR builds */

#include "ps.h"
#include "rlp.h"
#include "dsm.h"
#include "crc.h"
#include <memory.h>
#include "err.h"
#include "dsutil.h"

  #include "ds707_cta_rlp.h"
#include "dsrlp_api.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  General RLP definitions
---------------------------------------------------------------------------*/
#define RLP_SEQ_MODULUS       256  /* Sequence number are 0 to 255         */
#define RLP_MAX_RTT_SETUP     900  /* SYNC/ACK maximum frame count         */
   /* Setup real high for MTOM Korea builds */
#define RLP_RTT_MARGIN          4  /* Round trip frame processing margin as
                                      determined thru testing              */
#define RLP_MAX_HOLE_AGE       50  /* Maximum Age for a missing frame 
                                      ( ie. give up at this age)           */

#define RLP_MAX_CONTIG_ERASE   127  /* Maximum number of contiguous erasures
                                      as specified in IS-99                */
#define RLP_LEN_0               0  /* For zero length data fields          */

/*---------------------------------------------------------------------------
  Maximum difference allowed between VN and VR. This is set to 64 frames
  as a means to limit the RAM consumption.
---------------------------------------------------------------------------*/
#define RLP_MAX_VN_VR_DELTA     RLP_WORST_CASE_SAVED_FRAMES

/*---------------------------------------------------------------------------
  Definitions for RLP frame fields and masks
---------------------------------------------------------------------------*/
#define RLP_NAK                0xC0          /* NAK - negative acknowledge */
#define RLP_SYNC               0xD0          /* Synchronization value      */
#define RLP_ACK                0xE0          /* ACKknowledge               */

/* Generic frame field masks */

#define RLP_UNSEGDATA_UPPR_BITS  0       /* Upper 2 bits for unseg data    */
#define RLP_CONTROL_UPPR_BITS    0xC0    /* Upper 2 bits for control frame */
#define RLP_SEGDATA_UPPR_BITS    0x80    /* Upper 2 bits for segmented data*/
#define RLP_GEN_CTL_CTL_MASK     0xF0    /* Mask to get control field      */
#define RLP_GEN_CTL_LEN_MASK     0x0F    /* Mask to get length field       */
#define RLP_GEN_DATA_LEN_MASK    0x7F    /* Mask to get generic data length*/
#define RLP_GEN_CTL_CHK_MASK     0xC0    /* Mask to determine Control field*/
#define RLP_ACK_MASK             0x20    /* Mask to get ACK state          */
#define RLP_SYNC_MASK            0x10    /* Mask to get SYNC state         */
#define RLP_ACK_ENABLED          0x20    /* Mask for ACK Enabled           */
#define RLP_SYNC_ENABLED         0x10    /* Mask for SYNC Enabled          */
#define RLP1_PRI_FRATE_TYPE_MASK 0xE0    /* Mask to get RS 1 Full Rate Type*/
#define RLP2_PRI_FRATE_TYPE_MASK 0xC0    /* Mask to get RS 2 Full Rate Type*/
#define RLP_SEGFR_1STSEG_MASK    0x80    /* Mask for 1st segment           */
#define RLP_SEGFR_2NDSEG_MASK    0x90    /* Mask for 2nd segment           */
#define RLP_SEGFR_LASTSEG_MASK   0xA0    /* Mask for last segment          */
#define RLP_SEGFR_FILL_FR_BITS   0xB0    /* Upper 4 bits for non 1/8 fills */
#define RLP_RS2_IDLE_FILL_MASK   0xF0    /* Mask to check Rate Set 2 Idles */
#define RLP_RS2_IDLE_LAST_BITS   0       /* Last 4 bits for RateSet2 Idles */
#define RLP_RS2_FILL_LAST_BITS   0xF0    /* Last 4 bits for 1/8 fills      */

/* Control Frame size */

#define CTL_FRAME_SIZ                  6

/*---------------------------------------------------------------------------
  Type for return value from Generic frame validation checking. Note the
  ordering of the items must not be changed due to their use as array indices.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_FRAME_OK = 0,            /* frame passed all checks                  */
  RLP_CRC_ERR,                 /* Generic control frame with CRC error     */
  RLP_DATA_LEN_ERR,            /* Generic data frame with bad length field */
  RLP_CTL_LEN_ERR,             /* Generic control frame with bad ctl field */
  RLP_FRAME_CHK_VLDN_MAX       /* MAX limit to the frame check type  */
} rlp_gen_fr_chk_enum_type;

/*---------------------------------------------------------------------------
  Type to represent the different types of frames.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_CONTROL_FR,                                /* Control Frame          */
  RLP_UNSEG_DATA_FR,                             /* Unsegmented data frame */
  RLP_SEG_DATA_FR,                               /* Segmented data frame   */
  RLP_IDLE_FR,                                    /* Primary Idle frame     */
  RLP_FILL_FR                                    /* Intersegment Fill Frame*/
} rlp_fr_type;

/*---------------------------------------------------------------------------
  Type for storing Primary non-generic frame errors.
---------------------------------------------------------------------------*/
typedef struct 
{
  dword idle_fr_err;           /* Idle frame CRC error */
  dword full_type_err;         /* Full rate type error */
  dword rx_seg_fr_errs;        /* Count of errors in rxed segmented frames */
} rlp_pri_fr_err_type;

/*---------------------------------------------------------------------------
  Type for storing RLP RX hole info. This structure tracks the age of each 
  hole in the RLP RX stream.
---------------------------------------------------------------------------*/
typedef struct 
{
  boolean naks;             /* Indicates Nak active for this sequence #    */
  byte    age;              /* Frame counts (~ 20 ms per) since last nak   */
} rlp_hole_type;

/*---------------------------------------------------------------------------
  Type for frame pointers items. These items are used to hold pointers to
  the list of re-xmit frames.
---------------------------------------------------------------------------*/
typedef struct
{
  rlp_frame_type *frame_ptr;                    /* pointer to an RLP frame */
} rlp_frame_ptr_type;

/*---------------------------------------------------------------------------
  Type for the NAK frames and associated info.
---------------------------------------------------------------------------*/
typedef struct
{
  byte first_seq;
  byte last_seq;
} rlp_nak_type;

typedef struct
{
  byte           repeat_cnt;     /* how many times to repeat current NAK   */
  boolean        in_process;     /* whether current entry has been started */
  rlp_nak_type   nak;            /* NAK sequence number fields             */
} rlp_nak_frame_type;

/*---------------------------------------------------------------------------
  RLP control block definition. This structure contains all RLP operational
  information.
---------------------------------------------------------------------------*/
typedef struct
{
  rlp_state_enum_type state;    /* RLP State                               */
  boolean             pri_link; /* TRUE if primary channel link            */

  /* Receive variables */

  boolean first_sync_rcvd;      /* Received a SYNC since last reset        */
  word    expected_seq_num;     /* V(R), Next sequence number expected     */
  word    needed_seq_num;       /* V(N), Next sequence number needed       */
  word    contig_erase;         /* Contiguous erasures                     */
  word    rlp_delay;            /* RLP_DELAY, Round trip time in frames    */
   /* Use of delay overloaded for RLP SYNC_TIMEOUT counter                 */
  boolean sync_rcvd;            /* Last RX had SYNC, next TX needs ACK     */
  boolean hole_active;          /* TRUE when RX stream has a hole          */

  /* Transmit variables */

  byte    nak_in_index;         /* in index for control frame array        */
  byte    nak_out_index;        /* out index for control frame array       */
  byte    nak_frames_cnt;       /* number of ctl frames in array           */
  byte    re_xmit_in_index;     /* in index for re-xmit ptr array          */
  byte    re_xmit_out_index;    /* out index for re-xmit ptr array         */
  byte    re_xmit_frames_cnt;   /* num of re-xmit frames in array          */
  byte    saved_frames_index;   /* index into saved frames array           */
  byte    saved_frames_cnt;     /* num of saved data frames in buf         */
  word    next_seq_num;         /* V(S), Next sequence number to use       */

} rlp_state_info_type;


/*---------------------------------------------------------------------------
  Type to tally RLP generic statistics.
---------------------------------------------------------------------------*/
typedef struct
{
  dword single_naks;              /* single NAKs sent                      */
  word  double_naks;              /* double NAKs sent                      */
  word  triple_naks;              /* triple NAKs sent                      */
  dword re_xmits;                 /* Re-xmits sent                         */
  dword seq_timeout_cnt;          /* count of sequence # timeouts          */
  dword re_xmits_missed;          /* # of re-xmits not all done            */
  dword re_xmits_not_found;       /* # of Rexmits not found                */
  dword rx_data_cnt;              /* # of data frames received             */
  dword tx_data_cnt;              /* # of data frames Transmitted          */
  byte  largest_re_xmit;          /* most number of Re-xmited frames       */ 
  byte  fill_frames_rxed;         /* # of fill frames received             */
  byte  resets;                   /* # of resets                           */
  word  largest_contig_erase;     /* largest block of consecutive erasures */
} rlp_gen_stat_type;

/*---------------------------------------------------------------------------
  Variables to support indexing into Rate Set data structures. Default values
  represent Rate Set 1 on Primary Channel.
---------------------------------------------------------------------------*/
LOCAL byte rlp_rs_offset=0;       /* Rate Set Offset, 0 := RS 1, 2 := RS 2   */
LOCAL byte rlp_ct_offset=0;       /* Channel type Offset, 0 := Pri, 1 := Sec */

LOCAL rlp_mux_option_type rlp_mux=RLP_MUX_OPTION1;
     /* Current Mux (Rate Set) option */

/*---------------------------------------------------------------------------
  Variables to support Segmented Re-xmit frames.
---------------------------------------------------------------------------*/
LOCAL boolean rlp_rx_segfr_active;                      /* Seg RX flag     */
LOCAL byte    rlp_rx_segfr_data[ RLP2_FULL_PRI_DATA_BYTES]; /* holds payload */
LOCAL byte    rlp_rx_segfr_data_len;                    /* payload length  */
LOCAL byte    rlp_rx_segfr_seq_num;                     /* RX sequence num */

LOCAL boolean rlp_tx_segfr_active;         /* Segmented TX active flag     */
LOCAL byte    *rlp_tx_segfr_src_ptr;       /* ptr into re-xmit source frame*/
LOCAL byte    rlp_tx_segfr_remaining_bytes;/* remaining frame bytes        */

/*---------------------------------------------------------------------------
  Hole indicator data structure. This structure must contain an entry for
  each possible sequence number.
---------------------------------------------------------------------------*/
LOCAL rlp_hole_type rlp_hole[ RLP_SEQ_MODULUS];

/*---------------------------------------------------------------------------
  Reassembly queue.
---------------------------------------------------------------------------*/
LOCAL q_type rlp_reassembly_q;      

/*---------------------------------------------------------------------------
  Type and table that contains Primary Channel Full Rate mask, TYPE, and
  Payload fields values. Table is indexed by Mux Option.
---------------------------------------------------------------------------*/
typedef struct
{
  byte type_mask;               /* field that contains the TYPE field mask */
  byte format_a;                /* TYPE field Format A value */
  byte format_b;                /* TYPE field Format B value */
  byte format_b_payload;        /* Payload size for Format B (max) frames */
} rlp_prifullrate_table_type;

LOCAL const rlp_prifullrate_table_type rlp_prifull_table[2] =
{
  { RLP1_PRI_FRATE_TYPE_MASK, RLP1_PRI_FULL_RATE_A, /* Rate Set 1 */
  RLP1_PRI_FULL_RATE_B, RLP1_FULL_PRI_DATA_BYTES },
  { RLP2_PRI_FRATE_TYPE_MASK, RLP2_PRI_FULL_RATE_A, /* Rate Set 2 */
  RLP2_PRI_FULL_RATE_B, RLP2_FULL_PRI_DATA_BYTES }
};


/*---------------------------------------------------------------------------
  Data structure containing the number of data bytes for each RLP Generic
  data frame, both rate sets, primary and secondary channel types.
  NOTE: 0 denotes the particular frame rate/channel type does NOT carry data.
---------------------------------------------------------------------------*/
LOCAL const byte rlp_gendata_maxlen_table[ RLP_ERASURE][ 4] = 
{
  {0, 0, 0, 0},              /* 1/8 rate, Idle frame data byte sizes */
  {0, 0, 0, 0},              /* 3/16 rate, Rate Set 2 only           */
  {0, 0, 4, 4},              /* 1/4 rate, RLP for Rate Set 2 only */
  {0, 0, 0,10},              /* 3/8 rate Rate Set 2 only */
  {0, 0, 0,13},              /* 7/16 rate, Rate Set 2 Sec only */
  {8, 9,13,15},              /* 1/2 rate data sizes */
  {0,14, 0,23},              /* 3/4 rate, secondary rate only */
  {0,17, 0,28},              /* 7/8 rate, secondary rate only */
  {19,19,31,30}               /* Full rate, generic format only */
};

/*---------------------------------------------------------------------------
  Arrays to hold RX per call, RX total, TX per call, and TX total frame counts.
---------------------------------------------------------------------------*/

LOCAL dword rlp_rx_stat[ RLP_FRAME_RATE_MAX][ 4];
LOCAL dword rlp_tx_stat[ RLP_FRAME_RATE_MAX][ 4];
LOCAL dword rlp_rx_stat_tot[ RLP_FRAME_RATE_MAX][ 4];
LOCAL dword rlp_tx_stat_tot[ RLP_FRAME_RATE_MAX][ 4];

/*---------------------------------------------------------------------------
  This structure holds the error return values from the Validate Generic
  frame function. There are 3 different error values.
---------------------------------------------------------------------------*/
LOCAL dword rlp_gen_rx_errs[ RLP_FRAME_RATE_MAX][ RLP_FRAME_CHK_VLDN_MAX];

LOCAL rlp_pri_fr_err_type rlp_pri_frate_errs;

LOCAL rlp_gen_stat_type    rlp_gen_stats;       /* various RLP statistics  */


/*---------------------------------------------------------------------------
  RLP state info structure declaration
---------------------------------------------------------------------------*/
LOCAL rlp_state_info_type rlp_state_info;

/*---------------------------------------------------------------------------
  Buffer allocation for Saved Frames array (FIFO)
---------------------------------------------------------------------------*/
#define  SAVED_FRAME_ARRAY_SIZ   RLP_WORST_CASE_SAVED_FRAMES

LOCAL rlp_frame_type rlp_saved_fr_array[ SAVED_FRAME_ARRAY_SIZ];

/*---------------------------------------------------------------------------
  Buffer allocation for Re-xmit pointer array
---------------------------------------------------------------------------*/
LOCAL rlp_frame_ptr_type rlp_rexmit_ptr_array[ SAVED_FRAME_ARRAY_SIZ];

/*---------------------------------------------------------------------------
  Buffer allocation for NAK array
---------------------------------------------------------------------------*/
#define NAK_ARRAY_SIZ  64
LOCAL rlp_nak_frame_type rlp_nak_array[ NAK_ARRAY_SIZ];

/*---------------------------------------------------------------------------
  Data item pointer for managing outbound (Um TX) payload data.
---------------------------------------------------------------------------*/
LOCAL dsm_item_type *rlp_tx_data_ptr = NULL;

/*---------------------------------------------------------------------------
  Function pointer and queue pointer variables to hold passed registration
  parameters.
---------------------------------------------------------------------------*/
void               (*rlp_post_rx_func_ptr)(uint32)  = NULL;
dsm_watermark_type *rlp_post_rx_wm_ptr              = NULL;
LOCAL dsm_watermark_type *rlp_tx_watermark_ptr            = NULL;
LOCAL uint32             rlp_post_rx_func_cb_data         = 0;

/*---------------------------------------------------------------------------
   Pointer to hold the HDLC instance
---------------------------------------------------------------------------*/
LOCAL void               *rlp_hdlc_inst_ptr            = NULL;

/*---------------------------------------------------------------------------
  Variables for Primary channel Idle (eighth) rate monitoring/checking.
---------------------------------------------------------------------------*/
LOCAL rlp_pri_idle_type  rlp_saved_eighth;
LOCAL rlp_rate_enum_type rlp_prev_pri_fr_rate = RLP_ERASURE;

/*---------------------------------------------------------------------------
  Nordstrom-Robinson Code Table. 
  
  For the table generating algorithm see the IS-99 section on Primary channel 
  Eighth Rate Idle frames.
---------------------------------------------------------------------------*/
LOCAL const byte nr_table[256] = 
{
      7,212,160, 74,201,127, 28,147, 154, 44,118,249, 49,226,175, 69,
    189, 50, 81,231,100,142,250, 41, 107,129,204, 31,215, 88,  2,180,
    243, 25,109,190, 42,165,198,112,  64,207,149, 35,252, 22, 91,136,
     94,232,139,  4,144, 67, 55,221, 166,117, 56,210, 13,187,225,110,
    238, 97, 59,141, 82,184,245, 38,  93,183,195, 16,132, 11,104,222,
      8,219,150,124,163, 21, 79,192, 240, 70, 37,170, 62,237,153,115,
     52,130,216, 87,159, 76,  1,235, 169,122, 14,228,103,209,178, 61,
    197, 47, 98,177,121,246,172, 26,  19,156,255, 73,202, 32, 84,135,
    120,171,223, 53,182,  0, 99,236, 229, 83,  9,134, 78,157,208, 58,
    194, 77, 46,152, 27,241,133, 86,  20,254,179, 96,168, 39,125,203,
    140,102, 18,193, 85,218,185, 15,  63,176,234, 92,131,105, 36,247,
     33,151,244,123,239, 60, 72,162, 217, 10, 71,173,114,196,158, 17,
    145, 30, 68,242, 45,199,138, 89,  34,200,188,111,251,116, 23,161,
    119,164,233,  3,220,106, 48,191, 143, 57, 90,213, 65,146,230, 12,
     75,253,167, 40,224, 51,126,148, 214,  5,113,155, 24,174,205, 66,
    186, 80, 29,206,  6,137,211,101, 108,227,128, 54,181, 95, 43,248,
};

#ifdef FEATURE_DS_PSTATS

/*---------------------------------------------------------------------------
  Array of stats names, stored as character strings.
  The strings could be defined within the array of name pointers,
  but the compiler would put them in CONST.
---------------------------------------------------------------------------*/
LOCAL const byte rlp_stats_strings[RLP_STATS_COUNT][18] =
{
  "Rx Data Cnt      ",
  "Tx Data Cnt      ",
  "Single Naks      ",
  "Double Naks      ",
  "Triple Naks      ",

  "ReXmits          ",

  "Seq Timeout Cnt  ",
  "ReXmits Missed   ",
  "ReXmits Not Found",
  "Largest ReXmit   ",
  "Fill Frames Rx'ed",

  "Idle Fr Errs     ",
  "Full Type Errs   ",
  "Rx Seg Frame Errs",
  "Erasures         ",
  "Lrgst Cntg. Erase",
  "Generic Errors   ",

  "Last RTT         ",

  "Resets           "
};

/*---------------------------------------------------------------------------
  Array of pointers to stats names.
---------------------------------------------------------------------------*/
const byte * const rlp_stats_name_ptr[RLP_STATS_COUNT] =
{
  rlp_stats_strings[0],
  rlp_stats_strings[1],
  rlp_stats_strings[2],
  rlp_stats_strings[3],
  rlp_stats_strings[4],
  rlp_stats_strings[5],
  rlp_stats_strings[6],
  rlp_stats_strings[7],
  rlp_stats_strings[8],
  rlp_stats_strings[9],
  rlp_stats_strings[10],
  rlp_stats_strings[11],
  rlp_stats_strings[12],
  rlp_stats_strings[13],
  rlp_stats_strings[14],
  rlp_stats_strings[15],
  rlp_stats_strings[16],
  rlp_stats_strings[17],
  rlp_stats_strings[18]
};

/*---------------------------------------------------------------------------
  Array of names of the last call ending state.
---------------------------------------------------------------------------*/
LOCAL const byte rlp_last_call_strings[3][20] =
{
  "ESTABLISHED        ",  /* Last call was established              */
  "RLP_NOT_ESTABLISHED",  /* RLP did not sync up in last call       */
  "TCP_NOT_ESTABLISHED"   /* TCP did not establish in the last call */
};

/*---------------------------------------------------------------------------
  Array of pointers to names of the last call ending state.
---------------------------------------------------------------------------*/
const byte * const rlp_last_call_synced[3] =
{
  rlp_last_call_strings[0],
  rlp_last_call_strings[1],
  rlp_last_call_strings[2]
};

/*---------------------------------------------------------------------------
  Pointer (into rlp_last_call_synced) to name of last call ending state.
---------------------------------------------------------------------------*/
byte * rlp_last_call_synced_ptr;

/*---------------------------------------------------------------------------
  Copy of the last round trip time.
---------------------------------------------------------------------------*/
word rlp_last_rtt;

#endif /* FEATURE_DS_PSTATS */

/*----------------------------------------------------------------------------
  Function prototypes put here to allow desired ordering of functions in file.
----------------------------------------------------------------------------*/
LOCAL void dispatch_frame( dsm_item_type *data_frame_ptr);
LOCAL rlp_rx_status_enum_type check_reassembly_q( void);

/*---------------------------------------------------------------------------
  IS-99 RLP critical section variable.
---------------------------------------------------------------------------*/
LOCAL rex_crit_sect_type rlp_crit_sect;


/*===========================================================================

MACRO GE_SEQ_CHECK

DESCRIPTION
  This macro will compare the 2 passed sequence number parameters and 
  return TRUE if the 1st argument is Greater than or Equal to the 2nd 
  argument (modulo RLP_SEQ_MODULUS).

PARAMETERS
  arg1, arg2: Sequence numbers (modulo RLP_SEQ_MODULUS) for comparison.

DEPENDENCIES
  The passed arguments must be words

RETURN VALUE
  TRUE if arg_1 >= arg_2 modulo RLP_SEQ_MODULUS, else FALSE

SIDE EFFECTS
  None
===========================================================================*/

#define GE_SEQ_CHECK( arg1, arg2) \
  ((((arg1) - (arg2)) & 0x80) == 0)


/*===========================================================================

MACRO GT_SEQ_CHECK

DESCRIPTION
  This macro will compare the 2 passed sequence number parameters and 
  return TRUE if the 1st argument is Greater than the 2nd argument (modulo 
  RLP_SEQ_MODULUS).

PARAMETERS
  arg1, arg2: Sequence numbers (modulo RLP_SEQ_MODULUS) for comparison.

DEPENDENCIES
  The passed arguments must be words

RETURN VALUE
  TRUE if arg_1 > arg_2 modulo RLP_SEQ_MODULUS, else FALSE

SIDE EFFECTS
  None
===========================================================================*/

#define GT_SEQ_CHECK( arg1, arg2) \
  (((arg1) != (arg2)) && ((((arg1) - (arg2)) & 0x80) == 0))

/*===========================================================================

FUNCTION CRC_GEN

DESCRIPTION
  This function will generate the CRC for the passed byte stream and store the
  CRC at the end of the passed stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void crc_gen
(
  byte *buf,          /* pointer to Frame buffer to put CRC at the end     */
  word len            /* length of the Frame buffer, including CRC bytes   */
)
{

  word crc;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */ 

  /*-------------------------------------------------------------------------
    Calculate the CRC for the frame data. Do not compute for the CRC field.
    Store the generated value in the last 2 bytes of the frame.
  -------------------------------------------------------------------------*/
  crc    = crc_16_l_calc( buf, (len-2)*8);
  buf   += ( len - 2);
  *buf++ = (byte)crc;
  *buf   = (byte)(crc >> 8);
} /* crc_gen */

/*===========================================================================

FUNCTION QUEUE_NAK_FRAMES

DESCRIPTION
  This function will queue an RLP NAK control frame for transmission using
  the passed sequence numbers.

  NOTE: The frame rate, sequence number & CRC fields are NOT set here due to
        not knowing frame rate and V(S) until frame TX time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void queue_nak_frames
(
  word       first_seq_num,                 /* 1st sequence number to NAK  */
  word       last_seq_num,                  /* last sequence number to NAK */
  byte       attempt_num                    /* attempt number: 1st,2nd,3rd */
)
{
  byte index;                               /* index into NAK frame array  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Start by checking for a full FIFO, exit if the FIFO is full
  -------------------------------------------------------------------------*/
  if( rlp_state_info.nak_frames_cnt == NAK_ARRAY_SIZ)
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR, "NAK Frame FIFO full, attempt: %d",
                  attempt_num);
    return;
  }
  /*-------------------------------------------------------------------------
    Set the NAK first/last sequence number and unused fields and set NAK
    repeat count and reset in-process flag. Increment the FIFO count.
  -------------------------------------------------------------------------*/
  index = rlp_state_info.nak_in_index;
  rlp_nak_array[index].nak.first_seq = (byte)first_seq_num;
  rlp_nak_array[index].nak.last_seq  = (byte)last_seq_num;
  rlp_nak_array[index].in_process    = FALSE;
  rlp_state_info.nak_in_index        = (rlp_state_info.nak_in_index + 1) %
                                       NAK_ARRAY_SIZ;
  /*-----------------------------------------------------------------------
    Enter critical section to protect RLP state info.
  -----------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlp_crit_sect);

  rlp_nak_array[index].repeat_cnt    = attempt_num;
  rlp_state_info.nak_frames_cnt++;

  /*-----------------------------------------------------------------------
    Leave critical section.
  -----------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rlp_crit_sect);

} /* queue_nak_frames */

/*===========================================================================

FUNCTION QUEUE_RE_XMIT_FRAMES

DESCRIPTION
  This function will queue for retransmission the data frame(s) for the passed
  sequence numbers.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void queue_re_xmit_frames
(
  word       first_seq_num,                 /* 1st sequence number to NAK  */
  word       last_seq_num                   /* last sequence number to NAK */
)
{
  word           desired_index;        /* index of 1st sequence number     */
  word           frames_to_queue;      /* the number of xmit frames        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_RLP_MSG2(MSG_LEGACY_MED, "Preparing to RE-XMIT: %u - %u", first_seq_num, last_seq_num);
  /*-------------------------------------------------------------------------
    Return if no data frames are stored.
  -------------------------------------------------------------------------*/
  if( rlp_state_info.saved_frames_cnt == 0)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "No Saved Frames to Re-xmit");
    return;
  }
  /*-------------------------------------------------------------------------
    Check for the requested Re-xmit containing a sequence >= V(S). If so then
    reset RLP per IS-99 (in section 3.7.1.2.2) and exit. 
  -------------------------------------------------------------------------*/
  if( GE_SEQ_CHECK( last_seq_num, rlp_state_info.next_seq_num))
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid Re-xmit, reset RLP");
    rlp_gen_stats.resets++;
    rlp_reset();
    return;
  }
  /*-------------------------------------------------------------------------
    Determine the index of the 1st rexmit frame by masking off the upper 2
    bits of the 1st sequence number. Check to insure that the sequence numbers
    match. Return if no match
  -------------------------------------------------------------------------*/
  desired_index = (first_seq_num & ( SAVED_FRAME_ARRAY_SIZ - 1));
  if(rlp_saved_fr_array[desired_index].frame.gen_data.seq_num != first_seq_num)
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Re-xmit frame not found: %u",
                  first_seq_num);
    rlp_gen_stats.re_xmits_not_found++;
    return;
  }
  /*-------------------------------------------------------------------------
    'desired_index' references the 1st rexmit sequence number. Set the number 
    of frames to Rexmit and, if appropriate, the 'largest contiguous re-xmit'.
  -------------------------------------------------------------------------*/
  frames_to_queue = 
      ( last_seq_num - first_seq_num + RLP_SEQ_MODULUS + 1) % RLP_SEQ_MODULUS;
#ifdef FEATURE_DS_PSTATS
  if( frames_to_queue > rlp_gen_stats.largest_re_xmit)
  {
    rlp_gen_stats.largest_re_xmit = (byte)frames_to_queue;
  }
  rlp_gen_stats.re_xmits++;
#endif /* FEATURE_DS_PSTATS */
  /*-------------------------------------------------------------------------
    Do last check to insure that bogus frames are not queued for Re-xmit.
  -------------------------------------------------------------------------*/
  if( frames_to_queue > rlp_state_info.saved_frames_cnt)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Suspicious Re-xmit truncated");
    frames_to_queue             = rlp_state_info.saved_frames_cnt;
    rlp_gen_stats.largest_re_xmit = (byte)frames_to_queue;
    rlp_gen_stats.re_xmits_missed++;
  }
  /*-------------------------------------------------------------------------
    Loop thru queuing Re-xmits until all frames are queued or Rexmit array
    becomes full.
  -------------------------------------------------------------------------*/
  while(( frames_to_queue-- != 0) && 
        ( rlp_state_info.re_xmit_frames_cnt <= SAVED_FRAME_ARRAY_SIZ))
  {
    /*-----------------------------------------------------------------------
      Enter critical section to protect RLP state info and rexmit data.
    -----------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&rlp_crit_sect);

    /*-----------------------------------------------------------------------
      Set the pointer in the re-xmit pointer array then increment the source 
      indices and the counter.
    -----------------------------------------------------------------------*/
    rlp_rexmit_ptr_array[ rlp_state_info.re_xmit_in_index].frame_ptr = 
                                   &rlp_saved_fr_array[ desired_index];
    rlp_state_info.re_xmit_in_index = (rlp_state_info.re_xmit_in_index + 1) %
                                      SAVED_FRAME_ARRAY_SIZ;
    rlp_state_info.re_xmit_frames_cnt++;

    /*-----------------------------------------------------------------------
      Leave critical section.
    -----------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&rlp_crit_sect);

    desired_index = ( desired_index + 1) % SAVED_FRAME_ARRAY_SIZ;
  }
  /*-------------------------------------------------------------------------
    Now check to see if all needed frames were queued. Log message and 
    re-xmit error indicator if not all re-xmits were queued.
  -------------------------------------------------------------------------*/
  if(( frames_to_queue+1) != 0)
  {
    
    rlp_gen_stats.re_xmits_missed++;
  }
} /* queue_re_xmit_frames */

/*===========================================================================
FUNCTION RESET_RX_SEGFR

DESCRIPTION
  This function resets all the rx_segfr variables either due to an
  erroneous received segmented frame or after complete reassembly of
  a segmented frame.
 
DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  This results in the resetting of relevent segmented frame variables.
===========================================================================*/

void reset_rx_segfr( void)
{
/* Reseting all rx segmented frame variables */
    rlp_rx_segfr_seq_num  = 0;
    rlp_rx_segfr_data_len = 0;
    rlp_rx_segfr_active   = FALSE;
}

/*===========================================================================
FUNCTION VALIDATE_GEN_FRAME

DESCRIPTION
  This function will determine if the passed frame is a correctly formatted
  Generic RLP frame. The checking consists of field checks, CRC checking, and
  checking for segmented data frame.
  
  This function will also set the passed frame type parameter.
  
DEPENDENCIES
  The passed frame must be a generic RLP frame.

RETURN VALUE
  RLP_FRAME_OK if frame passed appropriate checks, else RLP_DATA_LEN_ERR,
  RLP_CTL_LEN_ERR, or RLP_CRC_ERR.

SIDE EFFECTS
  The reception of a segmented data frame results in the setting/resetting of
  relevent segmented frame variables.
===========================================================================*/

LOCAL rlp_gen_fr_chk_enum_type validate_gen_frame
(
  rlp_gen_frame_type       *frame_ptr,     /* pointer to generic RLP frame */
  rlp_fr_type              *frame_typ,     /* ptr to frame type variable   */
  word                     max_data_len    /* max data size for frame rate */
)
{
  boolean                  reset_seg_rx;         /* indicates reset seg RX */
  rlp_gen_fr_chk_enum_type return_val;                   /* return value   */
  static byte              seg_fr_cnt = 0;      /* segmented frame counter */
  byte                     seg_fr_ctl_len; /* segmented frame ctl len field*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Preset return value and seg frame reset flag then switch on upper 2 bits
    of the ctl_len field.
  -------------------------------------------------------------------------*/
  reset_seg_rx = TRUE;
  return_val   = RLP_FRAME_OK;
  switch( frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_CHK_MASK)
  {
    case RLP_UNSEGDATA_UPPR_BITS:
     /*======================================================================
        UNSEGMENTED DATA frame indicated. Check for proper length field.
     ======================================================================*/
      *frame_typ   = RLP_UNSEG_DATA_FR;
      if( frame_ptr->gen_data.ctl_len > max_data_len)
      {
        return_val = RLP_DATA_LEN_ERR;
      }
      break;

    case RLP_CONTROL_UPPR_BITS:
     /*======================================================================
        CONTROL frame indicated. Do checks to determine if the frame has a 
        CRC error or an invalid length field.
     ======================================================================*/
      *frame_typ = RLP_CONTROL_FR;
      if( crc_16_l_calc( (byte *)&frame_ptr->gen_ctl.seq_num,
          CTL_FRAME_SIZ*8) != CRC_16_L_OK)
      {
        return_val   = RLP_CRC_ERR;
      }
      else if(( frame_ptr->gen_ctl.ctl_len & RLP_GEN_CTL_LEN_MASK) 
                != 0)
      {
        return_val   = RLP_CTL_LEN_ERR;
      }
  else
  /* Valid control frame received  */
  {
    reset_seg_rx = FALSE;   /* Control frames allowed between segments */
  }   
      break;
      
    case RLP_SEGDATA_UPPR_BITS:
     /*======================================================================
        SEGMENTED DATA frame indicated. Further check ctl_len fields to
        determine if it is a fill frame in rateset2.
     ======================================================================*/
      if(( frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_CTL_MASK) ==
           RLP_SEGFR_FILL_FR_BITS) 
      {
         if( rlp_mux == RLP_MUX_OPTION2)
         {
         /*-----------------------------------------------------------------
          Received an intersegment fill frames. These frames should only be 
          sent by Lucent Basestations. RLP just discards these frames.
          -----------------------------------------------------------------*/
           DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Fill frame received");
           reset_seg_rx = FALSE;         /* Don't reset rx_segfr variables */
           rlp_gen_stats.fill_frames_rxed++;
           *frame_typ = RLP_FILL_FR;
         }
         else  /* Fill frame received not in rateset2 */
         {
         /* Invalid frame received - fill frames allowed only in rateset2 */
         DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid Frame rxed");
         rlp_pri_frate_errs.rx_seg_fr_errs++;
         return_val   = RLP_CTL_LEN_ERR;
         }         
      }
      else /* Not a fill frame */
      {
        /*-------------------------------------------------------------------
          Segmented frame has a valid control field. Check for valid length.
        -------------------------------------------------------------------*/
        *frame_typ = RLP_SEG_DATA_FR;
        if(( frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK)   >
              max_data_len ||
            ( frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) == 0)
       /* To ensure segmented frame lengths are not zero  */
       /* Greater than length check only useful with smaller rate frames*/
        {
          /*-----------------------------------------------------------------
            Segmented frame has an invalid length field. Indicate that 
            segmented RX needs to reset and set return value.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                        "Segment with invalid length received");
          rlp_pri_frate_errs.rx_seg_fr_errs++;
          return_val   = RLP_DATA_LEN_ERR;
        }
        else if (rlp_rx_segfr_active)
        {
          /*-----------------------------------------------------------------
            Segmented frame has a valid length field and RX already active. 
            Check for 2nd or last frame with matching sequence number and 
            nonzero current payload length.
          -----------------------------------------------------------------*/
          seg_fr_ctl_len = frame_ptr->gen_data.ctl_len & 
                           RLP_GEN_CTL_CTL_MASK;
          if((( seg_fr_ctl_len == RLP_SEGFR_2NDSEG_MASK && seg_fr_cnt == 1) ||
              ( seg_fr_ctl_len == RLP_SEGFR_LASTSEG_MASK && seg_fr_cnt >= 1)) 
                && ( frame_ptr->gen_data.seq_num == 
                rlp_rx_segfr_seq_num && rlp_rx_segfr_data_len != 0))

          /*-----------------------------------------------------------------
            Checking for count >=1 as first segment can be followed by
            last segment. 
            ---------------------------------------------------------------*/
          {
            /*---------------------------------------------------------------
              This segmented frame is a valid 2nd or Last segment.
            ---------------------------------------------------------------*/
            reset_seg_rx = FALSE;   /* Don't reset rx_segfr variables */
            seg_fr_cnt++;
          }
          else /* Invalid segmented data frame */
          {
            DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                          "Invalid 2nd or Last segmented frame received");
            rlp_pri_frate_errs.rx_seg_fr_errs++;
            return_val   = RLP_CTL_LEN_ERR;
          }
        } /* if( rlp_rx_segfr_active) */
        else if((( frame_ptr->gen_data.ctl_len & 
                   RLP_GEN_CTL_CTL_MASK)         == RLP_SEGFR_1STSEG_MASK) && 
                 ( rlp_rx_segfr_active           == FALSE) &&
                 ( rlp_hole[ frame_ptr -> seq_num].naks == TRUE) )
        {
          /*-----------------------------------------------------------------
            The current frame is a 1st segment. Set indicator and return value.
          -----------------------------------------------------------------*/
          seg_fr_cnt          = 1;
          reset_seg_rx = FALSE;         /* Don't reset rx_segfr variables */
          rlp_rx_segfr_active = TRUE;
        }
        else /* Invalid segmented frame */
        {
          DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                        "Invalid first segmented frame received");
          return_val   = RLP_CTL_LEN_ERR;
          rlp_pri_frate_errs.rx_seg_fr_errs++;
        }
      }
      break;

    default:
     /*======================================================================
        Frame error. Failed control/length field check. Exit indicating such.
     ======================================================================*/
      *frame_typ   = RLP_CONTROL_FR;
      return_val   = RLP_CTL_LEN_ERR;
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "RLP frame with invalid CTL-LEN field");
  } /* switch */

  /*-------------------------------------------------------------------------
    Now check to see if the segmented RX processing should be reset.
  -------------------------------------------------------------------------*/
  if(( rlp_rx_segfr_active) && (reset_seg_rx))
  {
   seg_fr_cnt            = 0;
   DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Resetting segmented receive");
   reset_rx_segfr();   /*Reset all global rx_segfr variables */
  }
  return( return_val);
} /* validate_gen_frame */



/*===========================================================================

FUNCTION RESET_STATE_INFO

DESCRIPTION
  This function resets all RLP call-specific state info and variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void reset_state_info( void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Enter critical section to protect RLP state info.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlp_crit_sect);

  /*-------------------------------------------------------------------------
    Reset all fields of the RLP state structure and hole structure.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_PSTATS
  rlp_last_rtt = (word)rlp_state_info.rlp_delay;
#endif
  (void)memset( &rlp_state_info, 0, sizeof( rlp_state_info_type));
  (void)memset( rlp_hole,        0, sizeof( rlp_hole));

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rlp_crit_sect);

  rlp_saved_eighth.seq_num = 0;
  rlp_tx_segfr_active      = FALSE;
  reset_rx_segfr( );   /* Resets all global rx_segfr variables */
  (void)memset( rlp_rx_stat, 0, sizeof( rlp_rx_stat));
  (void)memset( rlp_tx_stat, 0, sizeof( rlp_tx_stat));
} /* reset_state_info */  

/*===========================================================================

FUNCTION PROCESS_CTL_FRAME

DESCRIPTION
  This function is a subroutine for the RLP RX processing. It will process the
  passed RLP control frame. This processing includes the queuing of Re-xmit
  frames or RLP SYNC-ACK processing.

DEPENDENCIES
  The passed frame must be an error-free RLP control frame.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void process_ctl_frame
(
  rlp_gen_frame_type *frame_ptr          /* pointer to valid control frame */
)
{
  /*=========================================================================
    First check for a NAK control frame (request re-xmit).
  =========================================================================*/
  if(( frame_ptr->gen_ctl.ctl_len & RLP_GEN_CTL_CTL_MASK) == RLP_NAK && 
     ( rlp_state_info.state != RLP_SYNC_STATE))
  {
    /*-----------------------------------------------------------------------
      State is OK for NAK. Check and update RLP_DELAY if still in RT_MEAS 
      state then queue the re-transmission frames.
    -----------------------------------------------------------------------*/
    if( rlp_state_info.state == RLP_RT_MEAS_STATE)
    {
      rlp_state_info.rlp_delay++;
    }
    queue_re_xmit_frames( frame_ptr->gen_ctl.first_seq, 
                          frame_ptr->gen_ctl.last_seq);
  }
  /*=========================================================================
    Reset SYNC_RCVD then check for SYNC bit. If SYNC process based on RLP
    state and Set SYNC_RVCD afterward.
  =========================================================================*/
  rlp_state_info.sync_rcvd = FALSE;
  if(( frame_ptr->gen_ctl.ctl_len & RLP_SYNC_MASK) == RLP_SYNC_ENABLED)
  {
    switch( rlp_state_info.state)
    {
      case RLP_SYNC_STATE:
        /*-------------------------------------------------------------------
          If 1st SYNC not yet received set 1st SYNC received.
        -------------------------------------------------------------------*/
        if( rlp_state_info.first_sync_rcvd == FALSE)
        {
          rlp_state_info.first_sync_rcvd = TRUE;
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, "1st SYNC Received");
        }
        break;

      case RLP_RT_MEAS_STATE:
        /*-------------------------------------------------------------------
          If ACK break for ACK processing below, else fall thru to reset RLP.
        -------------------------------------------------------------------*/
        if((frame_ptr->gen_ctl.ctl_len & RLP_ACK_MASK)==RLP_ACK_ENABLED)
        {
          break;
        } /* else fall thru */

      case RLP_ESTABLISHED_STATE:
        /*-------------------------------------------------------------------
          Bad SYNC. Reset RLP then exit to void ACK processing below.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid SYNC, reset RLP",
                      rlp_state_info.state);
        rlp_gen_stats.resets++;
        rlp_reset();
        return;
     
      default:
        ERR_FATAL( "Invalid RLP State", 0, 0, 0);
    }
    rlp_state_info.sync_rcvd = TRUE;
  }    
  /*=========================================================================
    Check for ACK bit. If ACK process based on RLP state.
  =========================================================================*/
  if(( frame_ptr->gen_ctl.ctl_len & RLP_ACK_MASK) == RLP_ACK_ENABLED)
  {
    switch( rlp_state_info.state)
    {
      case RLP_SYNC_STATE:
        /*-------------------------------------------------------------------
          If SYNC was also received zero RLP_DELAY and set state to RT_MEAS.
        -------------------------------------------------------------------*/
        if( rlp_state_info.first_sync_rcvd)
        {
          rlp_state_info.rlp_delay = 0;
          rlp_state_info.state     = RLP_RT_MEAS_STATE;
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Received ACK, state = RT_MEAS");
        }
        break;

      case RLP_RT_MEAS_STATE:
        /*-------------------------------------------------------------------
          ACK received in RT_MEAS state. Increment RLP_DELAY counter.
        -------------------------------------------------------------------*/
        rlp_state_info.rlp_delay++;
        break;

      case RLP_ESTABLISHED_STATE:
        /*-------------------------------------------------------------------
          ACK received in ESTABLISHED state. Reset RLP.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "ACK when ESTABLISHED, reset RLP");
        rlp_gen_stats.resets++;
        rlp_reset();
        break;
     
      default:
        ERR_FATAL( "Invalid RLP State", 0, 0, 0);
    }
  }
} /* process_ctl_frame */

/*===========================================================================

FUNCTION CHECK_VN_VR_DELTA

DESCRIPTION
  This function will check to determine if the RLP needed sequence number V(N)
  and the expected sequence number V(R) have reached their maximum delta. If
  so then RLP will be reset.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the maximum delta ( 127 ) is reached and an RLP reset occurred, else
  FALSE.

SIDE EFFECTS
  If the maximum delta is reached the RLP reset will clear all RLP state
  information.
===========================================================================*/

LOCAL boolean check_vn_vr_delta
(
  word current_seq_num                 /* current received sequence number */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( GE_SEQ_CHECK( current_seq_num,
      (rlp_state_info.needed_seq_num + RLP_MAX_VN_VR_DELTA)))
  {
    DATA_RLP_MSG2(MSG_LEGACY_ERROR, "RLP Reset: Max V(N/R), %d/%d",
                  rlp_state_info.needed_seq_num,
                  current_seq_num);
    rlp_gen_stats.resets++;
    rlp_reset();
    return( TRUE);
  }
  return( FALSE );
}

/*===========================================================================

FUNCTION PROCESS_SEQUENCE_NUM

DESCRIPTION
  This function is a subroutine for the RLP RX processing. It performs sequence
  number checking for an RLP frame. This includes the aging of the reassembly
  list, the queueing of NAK frames, and the possible updating of the
  needed/expected sequence numbers.

  NOTE: This function only handles 'dsm_item_type' using the standard DMSS
        'q_link_type' linking (ie. no 'packet'ized items)

DEPENDENCIES
  The passed frame must be error-free.

RETURN VALUE
  RLP_RX_DATA_AVAILABLE if frame(s) queued for dispatch (from reassembly), else 
  RLP_RX_NO_DATA_AVAILABLE

SIDE EFFECTS
  None
===========================================================================*/

LOCAL rlp_rx_status_enum_type process_sequence_num
(
  byte        current_seq_num,          /* sequence number of passed frame */
  rlp_fr_type frame_type                           /* frame type indicator */
)
{
  word                    i;               /* for working thru hole list   */
  word                    last_seq_num;    /* last sequence number         */
  rlp_rx_status_enum_type sequence_chk;    /* sequence # check value holder*/
  rlp_rx_status_enum_type reassembly_chk;  /* Reassembly Q check return val*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    First age all active hole entries between V(N) and the current sequence
    number. Do not age if a Control frame.
  -------------------------------------------------------------------------*/
  sequence_chk    = RLP_RX_NO_DATA_AVAILABLE;
  reassembly_chk  = RLP_RX_NO_DATA_AVAILABLE;
  if( frame_type != RLP_CONTROL_FR)
  {
    if( GE_SEQ_CHECK( current_seq_num, rlp_state_info.expected_seq_num))
    {
      /*---------------------------------------------------------------------
        The RX'd sequence number is >= V(R) (extected sequence number). Age all
        active hole entries between V(N) and the sequence number of the just 
        received frame. If a hole gets too old, give up on it.
      ---------------------------------------------------------------------*/
      for( i=rlp_state_info.needed_seq_num; GT_SEQ_CHECK( current_seq_num,i);
           i=(i+1) % RLP_SEQ_MODULUS)
      {
        /*-------------------------------------------------------------------
          Age the hole then check to see if its time for the 2nd or 3rd NAK or
          time to give up on the sequence number.
        -------------------------------------------------------------------*/
        if( rlp_hole[i].naks)
        {
          rlp_hole[i].age++;
          if( rlp_hole[i].age == rlp_state_info.rlp_delay + RLP_RTT_MARGIN)
          {
            /*---------------------------------------------------------------
              This missing sequence num has aged long enough for the double 
              NAK. Double NAKs are sent as single sequence numbers.
            ---------------------------------------------------------------*/
#ifdef FEATURE_DS_PSTATS
            rlp_gen_stats.double_naks++;
#endif
            queue_nak_frames( i, i, 2);
          } 
          else if( rlp_hole[i].age == 
                   2 * ( rlp_state_info.rlp_delay + RLP_RTT_MARGIN))
          {
            /*---------------------------------------------------------------
              This missing sequence num has aged long enough for the triple
              NAK. Triple NAKs are sent as single sequence numbers.
            ---------------------------------------------------------------*/
#ifdef FEATURE_DS_PSTATS
            rlp_gen_stats.triple_naks++;
#endif
            queue_nak_frames( i, i, 3);
          }
          else if( rlp_hole[i].age ==
                   3 * ( rlp_state_info.rlp_delay + RLP_RTT_MARGIN) ||
                   rlp_hole[i].age ==  RLP_MAX_HOLE_AGE)
          {
            /*---------------------------------------------------------------
              Time to give up on this missing sequence number. Clean up hole
              entry, If hole array index is the same as V(N) then invoke
              function to check reassembly list and dispatch/increment V(N)
              until V(N) not at head of reassembly list.
            ---------------------------------------------------------------*/
            DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Seq timeout: %u, V(N/R): %u/%u",
                          i,
                          rlp_state_info.needed_seq_num,
                          rlp_state_info.expected_seq_num);
            rlp_gen_stats.seq_timeout_cnt++;
            rlp_hole[i].naks     = FALSE;
            if( rlp_state_info.needed_seq_num == i)
            {
              rlp_state_info.needed_seq_num = 
                ( rlp_state_info.needed_seq_num + 1) % RLP_SEQ_MODULUS;
              sequence_chk = check_reassembly_q();
            } 
          } /* hole timed out */
        } /* if active hole   */
      } /* for */
    } /* current seq_num >= expected seq_num */
    /*-----------------------------------------------------------------------
      Now check reassembly queue for V(N). Dispatch from reassembly and 
      increment V(N) until V(N) not found or queue empty.
    -----------------------------------------------------------------------*/
    reassembly_chk = check_reassembly_q();
  } /* non-control frame */
  /*-------------------------------------------------------------------------
    Now compare current frame to V(R). Queue a NAK if current is > V(R).
  -------------------------------------------------------------------------*/
  if( GT_SEQ_CHECK( current_seq_num, rlp_state_info.expected_seq_num))
  {
    /*-----------------------------------------------------------------------
      At least 1 sequence number has been lost. First check to see if this
      frame takes the V(N)-V(R) delta over the max limit. Note: the delta
      check function will reset RLP and return TRUE if max delta is reached.
      Otherwise proceed to queue the NAK.
    -----------------------------------------------------------------------*/
    if( check_vn_vr_delta( current_seq_num))
    {
      return( RLP_RX_FRAME_ERR);
    }
    else /* Queue the NAK, re-establish V(R) */
    {
      last_seq_num = (RLP_SEQ_MODULUS + current_seq_num - 1) % RLP_SEQ_MODULUS;
#ifdef FEATURE_DS_PSTATS
      rlp_gen_stats.single_naks++;
#endif /* FEATURE_DS_PSTATS */
      queue_nak_frames( rlp_state_info.expected_seq_num, last_seq_num, 1);
      if( rlp_state_info.hole_active == FALSE )
      {
        DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Hole in RLP sequence");
      }
      /*---------------------------------------------------------------------
        Set 'hole active' semaphore. V(R) is Re-establish here only
        temporarily. 'process_data_frame' to follow will then increment V(R)
      ---------------------------------------------------------------------*/
      rlp_state_info.hole_active      = TRUE;
      rlp_state_info.expected_seq_num = current_seq_num;
    }
  }
  /*-------------------------------------------------------------------------
    Set return value based on whether any data frames were dispatched.
  -------------------------------------------------------------------------*/
  if(( sequence_chk == RLP_RX_DATA_AVAILABLE) || 
     ( reassembly_chk == RLP_RX_DATA_AVAILABLE))
  {
    return( RLP_RX_DATA_AVAILABLE);
  }
  else  /* No data frames dispatched */
  {
    return( RLP_RX_NO_DATA_AVAILABLE);
  }
} /* process_sequence_num */

/*===========================================================================

FUNCTION CHECK_REASSEMBLY_Q

DESCRIPTION
  This function is a subroutine for the RLP RX sequence number and data frame
  processing. It checks the reassembly queue for sequence number V(N) and
  dispatches frames and increments V(N) until the queue is empty or V(N) is 
  not at the head of the queue.

DEPENDENCIES
    NOTE: This function only handles 'dsm_item_type' using the standard DMSS
        'q_link_type' linking (ie. no 'packet'ized items)

RETURN VALUE
  RLP_RX_DATA_AVAILABLE if frame(s) dispatched, else RLP_RX_NO_DATA_AVAILABLE

SIDE EFFECTS
  None
===========================================================================*/

LOCAL rlp_rx_status_enum_type check_reassembly_q( void)
{
  rlp_rx_status_enum_type return_value;      /* function return value      */
  dsm_item_type           *item_ptr;         /* for re-assembly list work  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*-------------------------------------------------------------------------
     Check reassembly list for V(N). Dispatch and increment V(N) from
     reassembly until V(N) not found or queue empty. 
  -------------------------------------------------------------------------*/
  return_value = RLP_RX_NO_DATA_AVAILABLE;
  item_ptr     = (dsm_item_type *)q_check( &rlp_reassembly_q);
  while(( item_ptr != NULL) && ( rlp_state_info.needed_seq_num == 
          ((rlp_gen_frame_type *)item_ptr->data_ptr)->seq_num))
  {
    return_value = RLP_RX_DATA_AVAILABLE;
    item_ptr     = (dsm_item_type *)q_get( &rlp_reassembly_q);
    dispatch_frame( item_ptr);
    rlp_state_info.needed_seq_num = 
      ( rlp_state_info.needed_seq_num + 1) % RLP_SEQ_MODULUS;
    item_ptr = (dsm_item_type *)q_check( &rlp_reassembly_q);
  } /* while */
  /*-------------------------------------------------------------------------
     Check to see if the last RX hole was patched (ie. V(N) == V(R)).
  -------------------------------------------------------------------------*/
  if(( rlp_state_info.hole_active) && 
     ( rlp_state_info.needed_seq_num == rlp_state_info.expected_seq_num))
  {
    /*---------------------------------------------------
      Reset Hole semaphore then reset NAK frames, since
      the queued NAKs are not needed.
    ---------------------------------------------------*/
    DATA_RLP_MSG0(MSG_LEGACY_HIGH, "V(N) == V(R)");
    rlp_state_info.hole_active = FALSE;

    /*-----------------------------------------------------------------------
      Enter critical section to protect RLP state info.
    -----------------------------------------------------------------------*/
    DS_ENTER_CRIT_SECTION(&rlp_crit_sect);

    rlp_state_info.nak_frames_cnt = 0;
    rlp_state_info.nak_out_index = 0;
    rlp_state_info.nak_in_index  = 0;

    /*-----------------------------------------------------------------------
      Leave critical section.
    -----------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&rlp_crit_sect);

  }
  return( return_value);
} /* check_reassembly_q */

/*===========================================================================

FUNCTION PROCESS_DATA_FRAME

DESCRIPTION
  This function is a subroutine for the RLP RX processing. It processes an
  unsegmented RLP RX data frame. This includes the dispatching or Reassembly
  queuing of the data frame.

DEPENDENCIES
  The passed frame must be an error-free data frame and its sequence number 
  must not be more than (RLP_SEQ_MODULUS/2) frames greater than V(N).

  NOTE: This function only handles 'dsm_item_type' using the standard DMSS
        'q_link_type' linking (ie. no 'packet'ized items)

RETURN VALUE
  RLP_RX_DATA_AVAILABLE if frame(s) queued for dispatch, else 
  RLP_RX_NO_DATA_AVAILABLE

SIDE EFFECTS
  None
===========================================================================*/

LOCAL rlp_rx_status_enum_type process_data_frame
(
  dsm_item_type *new_item_ptr                      /* ptr to RLP frame     */
)
{
  byte                    current_seq_num;   /* current sequence number    */
  dsm_item_type           *item_ptr;         /* for re-assembly list work  */
  rlp_gen_frame_type      *frame_ptr;        /* points to RLP frame        */
  rlp_rx_status_enum_type data_dispatch_chk; /* data frame dispatched val  */
  rlp_rx_status_enum_type reassembly_chk;    /* Reassembly check return val*/
  boolean                 add_to_reassembly; /* add-to-reassembly-q indicat*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  frame_ptr                       = (rlp_gen_frame_type *)new_item_ptr->
                                    data_ptr;
  current_seq_num                 = frame_ptr->seq_num;
  rlp_hole[ current_seq_num].naks = FALSE;
  data_dispatch_chk               = RLP_RX_NO_DATA_AVAILABLE;
  reassembly_chk                  = RLP_RX_NO_DATA_AVAILABLE;
  if( current_seq_num == rlp_state_info.expected_seq_num)
  {
    /*-----------------------------------------------------------------------
      Received V(R). If a hole is active invoke function to check the V(N)-V(R)
      delta (the function will reset RLP and return TRUE if so). Otherwise
      Increment V(R) for the next expected sequence number.
    -----------------------------------------------------------------------*/
    if( rlp_state_info.hole_active )
    {
      if( check_vn_vr_delta( current_seq_num))
      {
        (void)dsm_free_buffer( new_item_ptr);
        return( RLP_RX_NO_DATA_AVAILABLE);
      }
    }
    rlp_state_info.expected_seq_num = ( rlp_state_info.expected_seq_num + 1) % 
                                        RLP_SEQ_MODULUS;
  }
  if( current_seq_num == rlp_state_info.needed_seq_num)
  {
    /*-----------------------------------------------------------------------
     Received V(N) (needed sequence number). Dispatch frame, increment V(N).
    -----------------------------------------------------------------------*/
    dispatch_frame( new_item_ptr);
    rlp_state_info.needed_seq_num = ( rlp_state_info.needed_seq_num + 1) % 
                                      RLP_SEQ_MODULUS;
    data_dispatch_chk = RLP_RX_DATA_AVAILABLE;
  } 
  else if( GT_SEQ_CHECK( current_seq_num, rlp_state_info.needed_seq_num))
  {
    /*-----------------------------------------------------------------------
      This frame is subsequent to an active hole. Use the sequence number to 
      determine where in the reassembly list the frame belongs.
    -----------------------------------------------------------------------*/
    add_to_reassembly = TRUE;
    item_ptr          = (dsm_item_type *)q_check( &rlp_reassembly_q);
    while( item_ptr != NULL)
    {
      /*---------------------------------------------------------------------
        Compare the received sequence number to the current list item.
      ---------------------------------------------------------------------*/
      if(( (rlp_gen_frame_type *)(item_ptr->data_ptr))->seq_num == 
           current_seq_num)
      {
        /*-------------------------------------------------------------------
          The new frame is a duplicate. Set indicator then break from loop.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Reassy Dup: %u, V(N/R): %u/%u",
                      current_seq_num,
                      rlp_state_info.needed_seq_num,
                      rlp_state_info.expected_seq_num);
        (void)dsm_free_buffer( new_item_ptr);
        add_to_reassembly = FALSE;
        break;
      }
      if( !GT_SEQ_CHECK( current_seq_num,
          ((rlp_gen_frame_type *)item_ptr->data_ptr)->seq_num))
      {
        /*-------------------------------------------------------------------
          This item is the 1st one in the list with a sequence number greater 
          than the received frame. Break to allow insertion at current spot.
        -------------------------------------------------------------------*/
        break;
      }
      item_ptr = (dsm_item_type *)q_next( &rlp_reassembly_q, 
                                          &item_ptr->link);
    } /* while */
    /*-----------------------------------------------------------------------
      Check to determine if the frame is to be added to reassembly list.
    -----------------------------------------------------------------------*/
    if( add_to_reassembly)
    {
      if( item_ptr == NULL)
      {
        q_put( &rlp_reassembly_q, &new_item_ptr->link);
      }
      else /* queue not empty or item not last, insert at current spot     */
      {
#ifdef FEATURE_Q_NO_SELF_QPTR
        q_insert( &rlp_reassembly_q,&new_item_ptr->link, 
                  &item_ptr->link);
#else
        q_insert( &new_item_ptr->link, &item_ptr->link);
#endif
      }
    }
  }
  else /* Received sequence number not needed, discard as old duplicate   */
  {
    DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Old duplicate: %u, V(N/R): %u/%u",
                  current_seq_num,
                  rlp_state_info.needed_seq_num,
                  rlp_state_info.expected_seq_num);
    (void)dsm_free_buffer( new_item_ptr);
  }
  /*------------------------------------------------------------------------
     Now check reassembly list for V(N). Dispatch and increment V(N) until
     V(N) not found or queue empty then return proper indicator.
  ------------------------------------------------------------------------*/
  reassembly_chk = check_reassembly_q();
  if(( data_dispatch_chk == RLP_RX_DATA_AVAILABLE) || 
     ( reassembly_chk == RLP_RX_DATA_AVAILABLE))
  {
    return( RLP_RX_DATA_AVAILABLE);
  }
  else  /* No data frames dispatched */
  {
    return( RLP_RX_NO_DATA_AVAILABLE);
  }
} /* process_data_frame */

/*===========================================================================

FUNCTION PROCESS_SEG_DATA_FRAME

DESCRIPTION
  This function is a subroutine for the RLP RX processing. It performs the role
  of checking and consolidating RLP RX Segmented data frames. 

DEPENDENCIES
  The passed frame must contain a valid length field.

  NOTE: This function only handles 'dsm_item_type' using the standard DMSS
        'q_link_type' linking (ie. no 'packet'ized items)

RETURN VALUE
  RLP_RX_DATA_AVAILABLE if the just received sequence ( up to 3) of segmented
  data frames were properly consolidated into a single unsegmented data frame.

SIDE EFFECTS
  None
===========================================================================*/

LOCAL rlp_rx_status_enum_type process_seg_data_frame
(
  dsm_item_type *item_ptr             /* ptr to item to ref RLP frame/rate */
)
{
  rlp_rx_status_enum_type return_value;  /* function return value          */
  rlp_gen_frame_type      *frame_ptr;    /* points to RLP frame/rate       */
  byte                    *dest_ptr;     /* destination pointer for copies */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  frame_ptr = (rlp_gen_frame_type *)item_ptr->data_ptr;
  /*-------------------------------------------------------------------------
    Process a valid segmented RX frame. First check for last segmented frame.
  -------------------------------------------------------------------------*/
  if(( frame_ptr->gen_ctl.ctl_len & RLP_GEN_CTL_CTL_MASK) == 
       RLP_SEGFR_LASTSEG_MASK)
  {
    /*-----------------------------------------------------------------------
      Last segment received. Check to insure that composite frame will have
      a valid length. Build composite frame in current frame if valid.
    -----------------------------------------------------------------------*/
    if((( frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) +
          rlp_rx_segfr_data_len) <= rlp_prifull_table[ rlp_mux].format_b_payload)
     {
      /*---------------------------------------------------------------------
        Determine whether the Composite frame will be a generic frame or
        primary Full Rate frame. Set destination pointer and copy the 
        payload data.
      ---------------------------------------------------------------------*/
    if((( frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) +
          rlp_rx_segfr_data_len) < rlp_prifull_table[ rlp_mux].format_b_payload)
       {
        dest_ptr = (byte *)frame_ptr->gen_data.data;
        frame_ptr->gen_data.ctl_len = rlp_rx_segfr_data_len +
                    (frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK);
        (void)memscpy( dest_ptr + rlp_rx_segfr_data_len,
                       RLP2_FULL_GEN_DATA_BYTES - rlp_rx_segfr_data_len,
                       (void *)frame_ptr->gen_data.data,
                       (frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) );
        (void)memscpy( dest_ptr, RLP2_FULL_GEN_DATA_BYTES,
        	        rlp_rx_segfr_data, rlp_rx_segfr_data_len );
      }
      else /* Composite frame will be Primary Full Rate, set Type and dest */
      {
        dest_ptr = (byte *)frame_ptr->pri_fr_data.data;
        frame_ptr->pri_fr_data.data[rlp_prifull_table[ rlp_mux].
          format_b_payload]  = rlp_prifull_table[ rlp_mux].format_b;
        (void)memscpy( dest_ptr + rlp_rx_segfr_data_len,
                       RLP2_FULL_PRI_DATA_BYTES - rlp_rx_segfr_data_len,
                       (void *)frame_ptr->gen_data.data,
                       (frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) );
        (void)memscpy( dest_ptr, RLP2_FULL_PRI_DATA_BYTES,
      	                rlp_rx_segfr_data, rlp_rx_segfr_data_len );
      }
      /*---------------------------------------------------------------------
        Now set the return value.
      ---------------------------------------------------------------------*/
      reset_rx_segfr( );              /* Resets global rx_segfr variables */
      return_value        = RLP_RX_DATA_AVAILABLE;
    }
    else /* Invalid length, cancel Segmented RX, and exit */
    {
   reset_rx_segfr( );       /* Resets global rx_segfr variables */
      return_value          = RLP_RX_NO_DATA_AVAILABLE;
    }
  }
  else /* Not last segment */
  {
    /*-----------------------------------------------------------------------
      Save payload, update the segmented RX data length, free the frame 
      item and exit prior to sequence number and data frame processing.
    -----------------------------------------------------------------------*/
    (void)memscpy( rlp_rx_segfr_data + rlp_rx_segfr_data_len,
                   RLP2_FULL_PRI_DATA_BYTES - rlp_rx_segfr_data_len,
                  (void *)frame_ptr->gen_data.data,
                  ( frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK));
  
   rlp_rx_segfr_seq_num   = frame_ptr->gen_data.seq_num;
    rlp_rx_segfr_data_len += ( frame_ptr->gen_data.ctl_len & 
                               RLP_GEN_CTL_LEN_MASK);
  
   return_value = RLP_RX_NO_DATA_AVAILABLE;
  }
  return( return_value);
} /* process_seg_data_frame */

/*===========================================================================

FUNCTION RLP_INIT

DESCRIPTION
  This function initializes the RLP state machine. Upon completion of this
  function the RLP protocol unit will be ready for operation.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rlp_init( void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize critical section.
  -------------------------------------------------------------------------*/
  DS_INIT_CRIT_SECTION( &rlp_crit_sect );

  /*-------------------------------------------------------------------------
    Init the reassembly queue. Then reset all RLP state info then zero stats.
  -------------------------------------------------------------------------*/
  (void)q_init( &rlp_reassembly_q);

  reset_state_info();
  
  (void)memset( rlp_rx_stat, 0, sizeof( rlp_rx_stat));
  (void)memset( rlp_tx_stat, 0, sizeof( rlp_tx_stat));
  (void)memset( rlp_rx_stat_tot, 0, sizeof( rlp_rx_stat_tot));
  (void)memset( rlp_tx_stat_tot, 0, sizeof( rlp_tx_stat_tot));
  (void)memset( &rlp_gen_stats,   0, sizeof( rlp_gen_stat_type));
  (void)memset( rlp_gen_rx_errs,  0, sizeof( rlp_gen_rx_errs));
  (void)memset( &rlp_pri_frate_errs,    0, sizeof( rlp_pri_frate_errs));
#ifdef FEATURE_DS_PSTATS
  rlp_reset_stats();
#endif
} /* rlp_init */

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

void rlp_reset( void)
{
  dsm_item_type *item_ptr;         /* pointer to a data item               */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Enter critical section to protect global RLP state variables
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlp_crit_sect);

  /*--------------------------------------------------------------------------
    Reset RLP state info then flush Re-assembly queue.
  --------------------------------------------------------------------------*/
  reset_state_info();
  (void)dsm_free_buffer( rlp_tx_data_ptr);
  rlp_tx_data_ptr = NULL;
  while(( item_ptr = (dsm_item_type *)q_get( &rlp_reassembly_q)) != NULL)
  {
    (void)dsm_free_buffer( item_ptr);
  }

  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rlp_crit_sect);

} /* rlp_reset */

/*===========================================================================

FUNCTION DISPATCH_FRAME

DESCRIPTION
  Pass RLP data frame item to the queue for data frames that have completed 
  RLP RX processing.

DEPENDENCIES
  The passed frame must be a data frame

  NOTE: This function only handles 'dsm_item_type' using the standard DMSS
        'q_link_type' linking (ie. no 'packet'ized items)

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void dispatch_frame
(
  dsm_item_type *data_frame_ptr /* pointer to item referencing a data frame*/
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Set Item type and data_ptr (from RLP frame) then queue to Post-RX
    processing queue.
    Note, the decision whether the frame is a format B or generic data frame
    is taken based on the no. of bytes of available payload.
  -------------------------------------------------------------------------*/
  if(( data_frame_ptr->used == RLP1_FULL_PRI_DATA_BYTES &&
       rlp_mux              == RLP_MUX_OPTION1) ||
     ( data_frame_ptr->used == RLP2_FULL_PRI_DATA_BYTES &&
       rlp_mux              == RLP_MUX_OPTION2))
  {
    data_frame_ptr->data_ptr = (byte *)
         ((rlp_gen_frame_type *)(data_frame_ptr->data_ptr))->pri_fr_data.data;
  }
  else  /* Generic frame type */
  {
    data_frame_ptr->data_ptr = (byte *)
         ((rlp_gen_frame_type *)(data_frame_ptr->data_ptr))->gen_data.data;
  }



  /*----------------------------------------------------------------
    Set the app field to contain the hdlc instance ptr
    corresponding to this RLP
  ----------------------------------------------------------------*/
  data_frame_ptr->app_field = (uint32) rlp_hdlc_inst_ptr;
  
  dsm_enqueue( rlp_post_rx_wm_ptr, &data_frame_ptr);
  rlp_gen_stats.rx_data_cnt++;

} /* dispatch_frame */

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

void rlp_reg_srvc
(
  dsm_watermark_type *tx_watermark_ptr,     /* ptr to TX watermark for flow*/
  void               (*post_rx_func_ptr)(uint32), /* func ptr for Post-RX procsg */
  dsm_watermark_type *post_rx_wm_ptr,         /* ptr to queue to put RX data */
  uint32              post_rx_func_cb_data
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /*-------------------------------------------------------------------------
    Check for NULL pointers for each of the parameters. ERR_FATAL for NULL.
  -------------------------------------------------------------------------*/
  if( tx_watermark_ptr == NULL)
  {
    ERR_FATAL( "No TX Watermark item", 0, 0, 0);
  }
  else /* assign the watermark pointer */
  {
    rlp_tx_watermark_ptr = tx_watermark_ptr;
  }
  if( post_rx_func_ptr == NULL)
  {
    ERR_FATAL( "No Post RX Function", 0, 0, 0);
  }
  else /* assign the function pointer */
  {
    rlp_post_rx_func_ptr = post_rx_func_ptr;
  }
  if( post_rx_wm_ptr == NULL)
  {
    ERR_FATAL( "No Post RX Queue", 0, 0, 0);
  }
  else /* assign the queue pointer */
  {
    rlp_post_rx_wm_ptr = post_rx_wm_ptr;
  }

  rlp_post_rx_func_cb_data = post_rx_func_cb_data;
} /* rlp_reg_srvc */


/*===========================================================================
FUNCTION      RLP_REG_HDLC

DESCRIPTION   Registers the HDLC Instance for the RLP instance

DEPENDENCIES
  No parameter field can be NULL.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
/*lint -save -e715*/
void rlp_reg_hdlc
(
  void                *hdlc_inst_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    The input HDLC instance pointer should be populated before registering
    it for future use
  -------------------------------------------------------------------------*/
  if( hdlc_inst_ptr == NULL)
  {
    ERR_FATAL( "Invalid HDLC instance", 0, 0, 0);
  }
  else
  {
    rlp_hdlc_inst_ptr = hdlc_inst_ptr;
  }
} /* rlp_reg_hdlc */
/*lint -restore*/

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

  NOTE: This function only handles 'dsm_item_type' using the standard DMSS
        'q_link_type' linking (ie. no 'packet'ized items)

RETURN VALUE
  The status resulting from the RLP RX processing.

SIDE EFFECTS
  None
===========================================================================*/

rlp_rx_status_enum_type rlp_rx_process_frame
(
  rlp_rate_enum_type fr_rate,                 /* Rate of RLP frame         */
  boolean            primary_chan,            /* Primary Channel indicator */
  dsm_item_type      *rlp_item_ptr            /* ptr to RLP frame item     */
)
{
  rlp_rx_status_enum_type  return_value;     /* function return value      */
  rlp_rx_status_enum_type  seq_chk_return_val;/* Seq. checking return value*/
  rlp_fr_type              frame_typ;        /* indicates type of frame    */
  boolean                  generic_frame;    /* generic frame indicator    */
  rlp_gen_fr_chk_enum_type gen_frame_status; /* status of gen. frame check */
  rlp_gen_frame_type       *frame_ptr;       /* pointer to RLP frame       */
  byte                     params_offset;    /* for indexing operations    */
  static boolean           last2_idles_same       = FALSE;
               /* To remember that the last frame was an out of range idle */
  boolean                  last2_idle_reset_flag  = FALSE;
    /*-----------------------------------------------------------------------
      Note, this flag is not static - this is used in conjunction with the 
      boolean above to determine when to reset the last2_idle processing.
    -----------------------------------------------------------------------*/

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*=========================================================================
    Start by setting frame pointer, default return value, and Rate set/channel
    type offset (used for data frame processing). Increment general RX total 
    frame counts then checking to see if the time to achieve Sync has been 
    exceeded. Return with err if so.
  =========================================================================*/
  frame_ptr     = (rlp_gen_frame_type *)rlp_item_ptr->data_ptr;
  return_value  = RLP_RX_NO_DATA_AVAILABLE;
  params_offset = rlp_rs_offset + rlp_ct_offset;
  rlp_rx_stat[ fr_rate][params_offset]++;
  rlp_rx_stat_tot[ fr_rate][params_offset]++;
  if( rlp_state_info.state == RLP_SYNC_STATE && 
                       ++rlp_state_info.rlp_delay == RLP_MAX_RTT_SETUP)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "SYNC state Timeout");
    (void)dsm_free_buffer( rlp_item_ptr);
    rlp_gen_stats.resets++;
    rlp_reset();
    return( RLP_RX_SYNC_TIMEOUT);
  }
  /*=========================================================================
    Frame authenticity processing. Do frame validity checking based on rate.
    Free frame item and return if any frame errors are encountered.
  =========================================================================*/
  frame_typ     = RLP_UNSEG_DATA_FR;
  generic_frame = FALSE;
  switch( fr_rate)
  {
    case RLP_ERASURE:
      /*---------------------------------------------------------------------
        Erasure received. Tally statistics, save rate, if RLP RT_MEAS state
        then increment counter. 
      ---------------------------------------------------------------------*/
      if( rlp_state_info.state == RLP_ESTABLISHED_STATE) 
      {
        rlp_state_info.contig_erase++;
#ifdef FEATURE_DS_PSTATS
        rlp_gen_stats.largest_contig_erase =
          MAX(rlp_gen_stats.largest_contig_erase,
              rlp_state_info.contig_erase);
#endif /* FEATURE_DS_PSTATS */
         /* Increment conseq. erasure counter only when RLP is established */
      }
      rlp_prev_pri_fr_rate = fr_rate;
      if( rlp_state_info.state == RLP_RT_MEAS_STATE)
      {
        rlp_state_info.rlp_delay++;
      }
      return_value = RLP_RX_FRAME_ERR;
      break;

    case RLP_EIGHTH_RATE:
      /*---------------------------------------------------------------------
        1/8 rate (RLP Idle) frame. Ensure that the channel type is Primary 
        and the frame passes the Nordstrom-Robinson FCS check then perform 
        all Frame Validity checks as defined in IS-99 section 3.7.1.2.3.1.
      ---------------------------------------------------------------------*/
      if( primary_chan)
      {
        if( nr_table[ frame_ptr->pri_idle.seq_num] !=
            frame_ptr->pri_idle.nr_crc)
        {
          /*-----------------------------------------------------------------
            The frame fails the NR CRC check. Break to avoid saving frame.
          -----------------------------------------------------------------*/
          if( rlp_state_info.state == RLP_ESTABLISHED_STATE)
          {
            DATA_RLP_MSG0(MSG_LEGACY_ERROR, "RLP Idle NR CRC error");
            rlp_pri_frate_errs.idle_fr_err++;
            rlp_state_info.contig_erase++;
#ifdef FEATURE_DS_PSTATS
            rlp_gen_stats.largest_contig_erase =
              MAX(rlp_gen_stats.largest_contig_erase,
                  rlp_state_info.contig_erase);
#endif /* FEATURE_DS_PSTATS */
               /* Increment conseq. erasure count as per frame validity chk*/
          }
          return_value = RLP_RX_FRAME_ERR;
          break;
        }
        else if( rlp_mux == RLP_MUX_OPTION2 && 
               ((frame_ptr->pri_idle.rs2_filler & RLP_RS2_IDLE_FILL_MASK) 
                 == RLP_RS2_FILL_LAST_BITS))
        /*-----------------------------------------------------------------
          Received an intersegment fill frames. These frames should only be 
          sent by Lucent Basestations. RLP just discards these frames.
          Make sure, this check for fill frame bits is done before the next
          validity check. Both checks look at the same bits.
          -----------------------------------------------------------------*/
        {
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Received an intersegment fill frame");
          frame_typ    = RLP_FILL_FR;
          rlp_gen_stats.fill_frames_rxed++;
          return_value  = RLP_RX_NO_DATA_AVAILABLE;
            /* Default return value is no_data_available */
          break;
            /* Break from switch, so that the frame is not saved */
        }                      
        else if( rlp_mux == RLP_MUX_OPTION2 && 
               ( frame_ptr->pri_idle.rs2_filler & RLP_RS2_IDLE_FILL_MASK) 
                 != RLP_RS2_IDLE_LAST_BITS)
        {
          /*-----------------------------------------------------------------
            ERROR: Last 4 bits of a Rate Set 2 Idle has are not zeros. Break
            to avoid saving Frame. Note, we do not increment the conseq.
            erasure count because the frame validity check section in IS707
            makes a distinction between erasures and invalid frames. The 
            conseq. erasure count is incremented only for frames defined as
            erasures. 
          -----------------------------------------------------------------*/
          if( rlp_state_info.state == RLP_ESTABLISHED_STATE)
          {
            DATA_RLP_MSG0(MSG_LEGACY_ERROR, "RLP RS 2 non-zero filler");
          }
          rlp_pri_frate_errs.idle_fr_err++;
          return_value = RLP_RX_FRAME_ERR;
          break;
        }
        else if ( rlp_rx_segfr_active == TRUE &&
                  rlp_mux             == RLP_MUX_OPTION2)
          /*-----------------------------------------------------------------
            If an idle frame in rateset2 is received while reassembly is in 
            progress, reassembly is reset. The actual reset is done outside
            the switch statement.
          -----------------------------------------------------------------*/
        {
           rlp_pri_frate_errs.idle_fr_err++;
           return_value = RLP_RX_FRAME_ERR;
           break;
        }
        else if( rlp_prev_pri_fr_rate == RLP_EIGHTH_RATE && frame_ptr->
                 pri_idle.seq_num     != rlp_saved_eighth.seq_num)
        {
          /*-----------------------------------------------------------------
            If the previous frame was an Idle and the sequence numbers do not
            match then this frame is invalid. Note, according to sec. 3.1.3.1,
            this frame is not a erasure, so the consequetive erasure count is
            not incremented.
          -----------------------------------------------------------------*/
            DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Idle no match to previous");
            rlp_pri_frate_errs.idle_fr_err++;
            return_value         = RLP_RX_FRAME_ERR;
        }
        else   /* idle frame is good - so far */
        {
          /*-----------------------------------------------------------------
            This error-free Idle frame matches the previous frame. Check for 
            sequence number within acceptable range per IS-99 frame validity
            checks. The check just below determines if the Idle is within the
            acceptable range (see IS-99 section 3.7.1.2.3.1 item 6).
          -----------------------------------------------------------------*/
          if( GE_SEQ_CHECK(frame_ptr->pri_idle.seq_num, 
              rlp_state_info.expected_seq_num)  && GE_SEQ_CHECK( 
              rlp_state_info.expected_seq_num + rlp_state_info.contig_erase, 
              frame_ptr->pri_idle.seq_num))
          {
            /*---------------------------------------------------------------
              The frame is within the acceptable range of sequence numbers.
              Set return value and frame type.
            ---------------------------------------------------------------*/
            frame_typ    = RLP_IDLE_FR;
            return_value = RLP_RX_NO_DATA_AVAILABLE;
          }
          else if( last2_idles_same)
          {
            /*-------------------------------------------------------------
              This is the 3rd out-of-range in a row. Check the sequence num
              to determine if we Re-sync or re-establish expected V(R).
            -------------------------------------------------------------*/
            if( GT_SEQ_CHECK( frame_ptr->pri_idle.seq_num, 
                              rlp_state_info.expected_seq_num))
            {
              /*-----------------------------------------------------------
                Sequence number > expected V(R). Re-establish sequence num.
              -----------------------------------------------------------*/
              DATA_RLP_MSG2(MSG_LEGACY_ERROR, "3 Matching 1/8, now: %d, was: %d",
                            frame_ptr->pri_idle.seq_num,
                            rlp_state_info.expected_seq_num);
              frame_typ            = RLP_IDLE_FR;
              return_value         = RLP_RX_NO_DATA_AVAILABLE;
            }
            else /* RLP must Re-sync */
            {
              /*-----------------------------------------------------------
               Sequence number < expected V(R). Reset RLP state then break
               to avoid saving frame.
              -----------------------------------------------------------*/
              DATA_RLP_MSG0(MSG_LEGACY_ERROR, "RLP Idle Seq err, Re-Syncing");
              rlp_pri_frate_errs.idle_fr_err++;
              rlp_gen_stats.resets++;
              rlp_reset();
              return_value = RLP_RX_FRAME_ERR;
              break;
            }
          }
          else /* Out of range frame matches previous but not 3 in a row   */
          {
            /*---------------------------------------------------------------
              Frame matches previous frame but out of range and not 3 in a 
              row. Update frame variables then break.
            ---------------------------------------------------------------*/
            rlp_pri_frate_errs.idle_fr_err++;
            last2_idles_same      = TRUE;
            last2_idle_reset_flag = TRUE; 
            return_value          = RLP_RX_FRAME_ERR;
          }
        }   /* else if okay frame */
        /*-------------------------------------------------------------------
          Save frame rate and Idle frame bytes.
        -------------------------------------------------------------------*/
        rlp_saved_eighth     = frame_ptr->pri_idle;
        rlp_prev_pri_fr_rate = fr_rate;  /* Save frame rate for further proce.*/
      }  /* if primary frame */
      
      else /* Invalid Secondary Frame */
      {
        /*-------------------------------------------------------------------
          Idle frames not allowed for Secondary Service option.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid Secondary RLP Frame RX'd");
        rlp_state_info.contig_erase++;
#ifdef FEATURE_DS_PSTATS
            rlp_gen_stats.largest_contig_erase =
              MAX(rlp_gen_stats.largest_contig_erase,
                  rlp_state_info.contig_erase);
#endif /* FEATURE_DS_PSTATS */
        return_value = RLP_RX_FRAME_ERR;
      }
      break;

    case RLP_FULL_RATE:
      /*---------------------------------------------------------------------
        Full rate frame. Tally statistics then perform authenticity checking 
        to determine frame format & validity.
      ---------------------------------------------------------------------*/
      if( primary_chan && ( frame_ptr->pri_fr_data.data[rlp_prifull_table[ 
          rlp_mux].format_b_payload] & rlp_prifull_table[rlp_mux].type_mask) 
          == rlp_prifull_table[rlp_mux].format_b)
      {
        /*---------------------------------------------------
          Primary Full Rate Format B. Break to process frame.
        ---------------------------------------------------*/
        rlp_prev_pri_fr_rate = fr_rate;
        break;
      }
      else if((( frame_ptr->pri_fr_data.data[rlp_prifull_table[ rlp_mux].
                 format_b_payload]) & rlp_prifull_table[rlp_mux].type_mask) != 
                 rlp_prifull_table[rlp_mux].format_a)
      {
        /*-----------------------------------------------------------
          Frame doesn't conform to either of the Full Rate formats.
          Log error and break. Note, we do not increment conseq.
          erasure count. This is specified in the frame validity checks
          section of IS-707.
        -----------------------------------------------------------*/
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Primary Full Rate type err");
        rlp_pri_frate_errs.full_type_err++;
        rlp_prev_pri_fr_rate = RLP_ERASURE; 
        generic_frame = FALSE;
        return_value  = RLP_RX_FRAME_ERR;
        break;
      }
      /* else fall thru to do Generic Frame processing */

    case RLP_3_16TH_RATE:
    case RLP_3_8TH_RATE:
    case RLP_7_16TH_RATE:
    case RLP_HALF_RATE:
    case RLP_3_4TH_RATE:
    case RLP_7_8TH_RATE:
    case RLP_QUARTER_RATE:
      /*---------------------------------------------------------------------
        ???? received. Log frame statistics then determine frame
        validity and type.
      ---------------------------------------------------------------------*/
      /* This code also includes changes to quarter rate frame processing
         from the previous rateset 1 rlp code base  */
      if ( rlp_gendata_maxlen_table[ fr_rate][ params_offset] == 0)
      /* Double checks mux layer - if table entry is zero then this is 
         not a valid rateset & fr_rate combination */      
      {
        if ( fr_rate == RLP_QUARTER_RATE && rlp_mux == RLP_MUX_OPTION1)
        {
           DATA_RLP_MSG0(MSG_LEGACY_HIGH, " Quarter rate frame for rateset 1 received ");
        }
        else
        {
          /* ---------------------------------------------------------
             Write error to nv only if pathological mux layer error
             and not for rateset1 quarter rate 
           ---------------------------------------------------------*/
           DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                         "Pathological erasure received from mux layer");
        }
        rlp_prev_pri_fr_rate = RLP_ERASURE; 
        return_value         = RLP_RX_FRAME_ERR;
        break;
      }
 
      if(( gen_frame_status = validate_gen_frame( frame_ptr, &frame_typ, 
           rlp_gendata_maxlen_table[ fr_rate][ params_offset])) 
           != RLP_FRAME_OK)
      {
        /*-------------------------------------------------------------------
          Update error statistics and set return value. Contig. erasure
          count is only inremented when there is a CRC error. This is 
          specified in the frame validity checks section in IS707.
        -------------------------------------------------------------------*/
        if ( RLP_CRC_ERR == gen_frame_status)
        {
           DATA_RLP_MSG0(MSG_LEGACY_ERROR, " Control frame with bad CRC rxed");
           rlp_state_info.contig_erase++;
#ifdef FEATURE_DS_PSTATS
            rlp_gen_stats.largest_contig_erase =
              MAX(rlp_gen_stats.largest_contig_erase,
                  rlp_state_info.contig_erase);
#endif /* FEATURE_DS_PSTATS */
             /* increment erasure count for CRC errs not other generic errs*/
        }
        else
        {
           DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Frame Err: %u, Rate: %u, Chan: %u",
                         gen_frame_status,
                         fr_rate,
                         rlp_mux);
        }
        rlp_gen_rx_errs[ fr_rate][ gen_frame_status]++;
        rlp_prev_pri_fr_rate = RLP_ERASURE; 
        return_value  = RLP_RX_FRAME_ERR;
      }
      else /* Frame is Good. Set generic frame indicator. */
      {
        rlp_prev_pri_fr_rate = fr_rate;
        generic_frame = TRUE;
      }
      break;
  
    default:
      /*---------------------------------------------------------------------
        Invalid frame rate. Pathological software error. Cannot happen.
      ---------------------------------------------------------------------*/
      ERR_FATAL( "Unknown RLP data frame rate", 0, 0, 0);

  } /* switch */
  if ( last2_idle_reset_flag == FALSE)
  {
     /*---------------------------------------------------------------------
      Reset last2_idles_same variable in all situations, except when RLP 
      received 2 idles with out of range sequence numbers.
      ---------------------------------------------------------------------*/    
     last2_idles_same = FALSE;
  }
  
  /*-------------------------------------------------------------------------
    Check the return value for error indication. If any error do not proceed
    to the following sequence number and data frame processing.
  -------------------------------------------------------------------------*/
  if( return_value == RLP_RX_FRAME_ERR)
  {
    /*----------------------------------------------------------------------
      Error occurred. If maximum contiguous erasures received Reset RLP.
    ----------------------------------------------------------------------*/
    if( rlp_state_info.state == RLP_ESTABLISHED_STATE) 
    {
      if ( rlp_state_info.contig_erase >= RLP_MAX_CONTIG_ERASE)
      {
          DATA_RLP_MSG1(MSG_LEGACY_ERROR, "RLP Reset, Max Erasures: %d",
                        rlp_state_info.contig_erase);
          rlp_gen_stats.resets++;
          rlp_reset();
       }
       rlp_tx_segfr_active = FALSE;  
         /* Reseting txmit segmentation - RLP doesn't segment tx frames now */
       reset_rx_segfr();                       /* Reset rx segfr variables */
    }
    (void)dsm_free_buffer( rlp_item_ptr);
    return( return_value);
  }
  /*=========================================================================
   To get here a valid frame was received. Perform RLP channel type and 
   state checks to determine if subsequent sequence number and data frame 
   processing should be performed.
  =========================================================================*/
  rlp_state_info.contig_erase = 0;

  /*-------------------------------------------------------------------------
    Check for a change in the channel type. If the channel type has changed
    reset the RLP state and exit.
  -------------------------------------------------------------------------*/
  if(( rlp_state_info.state == RLP_ESTABLISHED_STATE) &&
     ( rlp_state_info.pri_link != primary_chan))
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "RLP Channel changed, Resync");
    rlp_gen_stats.resets++;
    rlp_reset();
    (void)dsm_free_buffer( rlp_item_ptr);
    return( RLP_RX_NO_DATA_AVAILABLE);
  }
  if ( frame_typ == RLP_FILL_FR)    /* This was a intersegment fill frame  */
  {
     (void)dsm_free_buffer( rlp_item_ptr);
       /* RLP receive processing frees all non-data rlp frame buffers      */
     return( RLP_RX_NO_DATA_AVAILABLE);
     /*---------------------------------------------------------------------
       No processing required for fill frames - we don't even check 
       sequence numbers or increment NAK counters.
      ---------------------------------------------------------------------*/
  }
  /*-------------------------------------------------------------------------
    Check for a control frame. Then exit if RLP is in SYNC state or a control 
    frame in RT_MEAS state.
  -------------------------------------------------------------------------*/
  if( frame_typ == RLP_CONTROL_FR)
  {
    process_ctl_frame( frame_ptr);
  }
  if(( rlp_state_info.state == RLP_SYNC_STATE) || 
    (( rlp_state_info.state == RLP_RT_MEAS_STATE) && 
     ( frame_typ == RLP_CONTROL_FR)))
  {
    (void)dsm_free_buffer( rlp_item_ptr);
    return( RLP_RX_NO_DATA_AVAILABLE);
  }
  /*-------------------------------------------------------------------------
    Check for the 1st non-control frame since RLP reset. Reset and exit if
    previous frame was an erasure otherwise set state to Established. NOTE
    this is not required in IS-99.
  -------------------------------------------------------------------------*/
  if( rlp_state_info.state == RLP_RT_MEAS_STATE)
  {
    if( rlp_prev_pri_fr_rate == RLP_ERASURE)
    {
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Bad 1st Non-ctl frame, Re-sync");
      rlp_gen_stats.resets++;
      rlp_reset();
      (void)dsm_free_buffer( rlp_item_ptr);
      return( RLP_RX_NO_DATA_AVAILABLE);
    }
    /*-----------------------------------------------------------------------
       Set the state to ESTABLISHED and set the channel type and channel type
       offset (for accessing parameter arrays).
    -----------------------------------------------------------------------*/
    rlp_state_info.state    = RLP_ESTABLISHED_STATE;


    rlp_state_info.pri_link = primary_chan;
    if( primary_chan)
    {
      rlp_ct_offset = 0;
    }
    else /* RLP on the secondary channel */
    {
      rlp_ct_offset = 1;
    }
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP ESTABLISHED, RLP_DELAY: %d", rlp_state_info.rlp_delay);
  }
  /*-------------------------------------------------------------------------
    Check for active segmented RX processing. If active then invoke function
    to process/consolidate segmented frames. Only proceed tp sequence and
    data frame processing if last segment of segmented data frame.
  -------------------------------------------------------------------------*/
  if( rlp_rx_segfr_active && frame_typ == RLP_SEG_DATA_FR )
    
  {
    DATA_RLP_MSG0(MSG_LEGACY_MED, "Processing Segmented Data Frame");
    return_value =  process_seg_data_frame( rlp_item_ptr);                    
    if( return_value != RLP_RX_DATA_AVAILABLE)
    {
      (void)dsm_free_buffer( rlp_item_ptr);   /* not last segment */
      return( return_value);
    }
    if(( frame_ptr->pri_fr_data.data[rlp_prifull_table[rlp_mux].format_b_payload]
         & rlp_prifull_table[rlp_mux].type_mask) 
         == rlp_prifull_table[rlp_mux].format_b)
    {
      /*---------------------------------------------------------------------
        Indicate change to frame format based on segmented frame processing.
      ---------------------------------------------------------------------*/
      generic_frame = FALSE;
    }
  }
  /*=========================================================================
    Do sequence number checking on the received frame then exit if Idle,
    control, a frame error, or zero length data frame. Note that a return_value
    indicating data available will be due to dispatching from the reassembly
    list. Invoke Post-RX function if there is data available.
  =========================================================================*/
  seq_chk_return_val =  
    process_sequence_num(((rlp_gen_frame_type *)rlp_item_ptr->data_ptr)->
                           seq_num, frame_typ);
  if(( frame_typ == RLP_IDLE_FR) || ( frame_typ == RLP_CONTROL_FR) ||
     ( seq_chk_return_val == RLP_RX_FRAME_ERR) || ( generic_frame  &&
     ( frame_ptr->gen_data.ctl_len & RLP_GEN_DATA_LEN_MASK) == RLP_LEN_0))
  {
    (void)dsm_free_buffer( rlp_item_ptr);
    if( seq_chk_return_val == RLP_RX_DATA_AVAILABLE)
    {
      DATA_RLP_MSG0(MSG_LEGACY_MED, "Dispatch frame from Re-assembly");
      (rlp_post_rx_func_ptr)(rlp_post_rx_func_cb_data);
    }
    return( seq_chk_return_val);
  }
  /*=========================================================================
    To get here we have a valid Data frame. Set Payload Data count (used)
    field then perform Data frame processing for the received frame. For the 
    return value use the sequence number checking return value if sequence 
    number checking yielded dispatch frames. Invoke Post-RX function if there 
    is data available.
  =========================================================================*/

  if( generic_frame == TRUE)
  {
    rlp_item_ptr->used = frame_ptr->gen_data.ctl_len & RLP_GEN_DATA_LEN_MASK;
  }
  else /* Primary Full rate frame, use Mux option to set data payload size */
  {
    if( rlp_mux == RLP_MUX_OPTION1)
    {
      rlp_item_ptr->used = RLP1_FULL_PRI_DATA_BYTES;
    }
    else
    {
      rlp_item_ptr->used = RLP2_FULL_PRI_DATA_BYTES;
    }
  }
  return_value = process_data_frame( rlp_item_ptr);
  if( seq_chk_return_val == RLP_RX_DATA_AVAILABLE || 
      return_value == RLP_RX_DATA_AVAILABLE)
  {
    (rlp_post_rx_func_ptr)(rlp_post_rx_func_cb_data);
    return( RLP_RX_DATA_AVAILABLE);
     /*---------------------------------------------------------------------
       Note, we don't free the frame buffer for data frames because the frame 
       pointer is passed to upper layers. 
      ---------------------------------------------------------------------*/
  }
  return( return_value);
} /* rlp_rx_process_frame */

/*===========================================================================

FUNCTION RLP_TX_GET_NEXT_FRAME

DESCRIPTION
  This function retrieves the next IS-99 RLP data frame to transmit on the 
  traffic channel. This function can operate on either primary or secondary
  CDMA channels.

  NOTE: This function handles both the 'q_link_type' and the packet type
        'dsm_item_type's.

DEPENDENCIES
  rlp_init must have been previously invoked.

RETURN VALUE
  An enumerated value representing the CDMA frame rate for the RLP TX frame.

SIDE EFFECTS
  1) The 'flow_enable_func_ptr' function will be invoked each time the TX
     watermark goes below the lo water mark.
  2) The 'empty_rx_buf_func_ptr' function will be invoked each time the TX
     Watermark becomes empty.
===========================================================================*/

rlp_rate_enum_type rlp_tx_get_next_frame
(
  byte               **frame_ptr,    /* address of ptr for the TX frame    */
  boolean            primary_frame,  /* Primary channel yes/no indicator   */
  rlp_rate_enum_type allowed_rate    /* required rate (for sig/sec. chans) */
)
{
  rlp_rate_enum_type    frame_rate;         /* to return the rate to use   */
  static rlp_frame_type tx_frame;           /* for Sync/segmented/or Idle  */
  word                  i;                  /* array index                 */
  word                  cur_packet_length;  /* current packet item length  */
  word                  next_packet_length; /* next packet item length     */
  byte                  actual_fr_payload;  /* actual data payload count   */
  byte                  cur_item_copy_cnt;  /* current item copy count     */
  byte                  next_item_copy_cnt; /* next item copy count        */
  byte                  data_filler_cnt;    /* for filling end-of-frames   */
  byte                  avail_fr_payload;   /* available payload data count*/
  byte                  *dest_ptr;          /* pointer for byte copy       */
  dsm_item_type         *next_item_ptr;     /* pointer for data frame work */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Enter critical section to protect RLP tx data ptr.
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlp_crit_sect);

  /*=========================================================================
    Determine the next RLP frame to send. As specified in IS-99 section 
    3.7.1.2.2 the prioritized order is 1) RLP initialization frames, 2) NAK
    control frames, 3) Re-xmit frames, and 4) Data frames.
    
    Start by checking to see if the RLP initialization is underway.
  =========================================================================*/
  if(( rlp_state_info.state == RLP_SYNC_STATE) || 
    (( rlp_state_info.state == RLP_RT_MEAS_STATE) &&
     ( rlp_state_info.sync_rcvd)))
  {
    /*-----------------------------------------------------------------------
      Sync-Ack in process. Send a SYNC, ACK or both.
    -----------------------------------------------------------------------*/
    if( rlp_state_info.state == RLP_SYNC_STATE)
    {
      tx_frame.frame.gen_ctl.ctl_len = RLP_SYNC;
      if( rlp_state_info.sync_rcvd)
      {
        tx_frame.frame.gen_ctl.ctl_len |= RLP_ACK;
      }
    }
    else /* RT_MEAS state */
    {
      tx_frame.frame.gen_ctl.ctl_len = RLP_ACK;
    }
    tx_frame.frame.gen_ctl.seq_num   = (byte)rlp_state_info.next_seq_num;
    tx_frame.frame.gen_ctl.first_seq = 0;
    tx_frame.frame.gen_ctl.last_seq  = 0;
    if( primary_frame)
    {
      frame_rate = RLP_HALF_RATE;
      (void)memset( (void *)tx_frame.frame.gen_ctl.unused, 0, 
                                              RLP2_FULL_GEN_CTL_UNUSED_BYTES);
    }
    else /* secondary/signaling channel */
    {
      /*---------------------------------------------------------------------
        Sec./Sig. channel. Set rate to required rate then fillout unused
        portion of the frame.
      ---------------------------------------------------------------------*/
      frame_rate = allowed_rate;
      (void)memset( (void *)tx_frame.frame.gen_ctl.unused, 0,
                    RLP2_FULL_GEN_CTL_UNUSED_BYTES);
    }
    *frame_ptr = ( byte *)&tx_frame.frame;
    crc_gen( ( byte *)&tx_frame.frame, CTL_FRAME_SIZ);
  } 
  /*=========================================================================
    Check for NAK FRAME(S) TO SEND. RLP in ESTABLISHED state or RT_MEAS state
    with SYNC_RCVD FALSE.
  =========================================================================*/
  else if( rlp_state_info.nak_frames_cnt != 0)
  {
    /*-----------------------------------------------------------------------
      NAK(s) to send. Set the frame rate to use check to see if the NAK frame
      sequence need to be loaded from the NAK array.
    -----------------------------------------------------------------------*/
    if( primary_frame == FALSE)
    {
      /*---------------------------------------------------------------------
        Secondary/Signaling channel. Set allowed rate.
      ---------------------------------------------------------------------*/
      frame_rate = allowed_rate;
    }
    else /* Primary channel */
    {
      frame_rate = RLP_HALF_RATE;
    }
    if( rlp_nak_array[rlp_state_info.nak_out_index].in_process == FALSE)
    {
      /*---------------------------------------------------------------------
        Frame not built yet. If a 1st NAK create hole list entries for all new
        NAK sequence numbers. Generate CRC and set NAK entry active flag.
      ----------------------------------------------------------------------*/
      DATA_RLP_MSG3(MSG_LEGACY_MED, "Send NAK: %u - %u, attempt: %d",
              rlp_nak_array[ rlp_state_info.nak_out_index].nak.first_seq,
              rlp_nak_array[rlp_state_info.nak_out_index].nak.last_seq,
              rlp_nak_array[rlp_state_info.nak_out_index].repeat_cnt);

      rlp_nak_array[rlp_state_info.nak_out_index].in_process = TRUE;
      tx_frame.frame.gen_ctl.first_seq =
        rlp_nak_array[rlp_state_info.nak_out_index].nak.first_seq;
      tx_frame.frame.gen_ctl.last_seq =
        rlp_nak_array[rlp_state_info.nak_out_index].nak.last_seq;
      if( rlp_nak_array[rlp_state_info.nak_out_index].repeat_cnt == 1)
      {
        for( i=rlp_nak_array[rlp_state_info.nak_out_index].nak.first_seq;
             GE_SEQ_CHECK( rlp_nak_array[rlp_state_info.nak_out_index].nak.
             last_seq, i); i = (i+1) % RLP_SEQ_MODULUS)
        {
          rlp_hole[ i].naks = TRUE;
          rlp_hole[ i].age  = 0;
        }
      }
      tx_frame.frame.gen_ctl.seq_num = (byte)rlp_state_info.next_seq_num;
      tx_frame.frame.gen_ctl.ctl_len   = RLP_NAK;
//      (void)memset( tx_frame.frame.gen_ctl.unused, 0,RLP_GEN_FRATE_UNUSED_SIZ);
      (void)memset( (void *)tx_frame.frame.gen_ctl.unused, 0,
                    RLP2_FULL_GEN_CTL_UNUSED_BYTES);
  
      *frame_ptr = ( byte *)&tx_frame.frame;
      crc_gen( (byte *)&tx_frame.frame, CTL_FRAME_SIZ);
    }

    /*-----------------------------------------------------------------------
      Decrement repeat counter. If zero move up index and decrement counter.
    -----------------------------------------------------------------------*/
    rlp_nak_array[rlp_state_info.nak_out_index].repeat_cnt--;
    if( rlp_nak_array[rlp_state_info.nak_out_index].repeat_cnt == 0)
    {
      rlp_state_info.nak_frames_cnt--;
      rlp_state_info.nak_out_index = (rlp_state_info.nak_out_index + 1) %
                                        NAK_ARRAY_SIZ;
    }
  } 
  /*=========================================================================
    Check for REXMIT FRAME(S) TO SEND. 
  =========================================================================*/
  else if( rlp_state_info.re_xmit_frames_cnt != 0)
  {
    /*-----------------------------------------------------------------------
      Rexmit active. Start by setting the available payload for this rexmit 
      frame.
    -----------------------------------------------------------------------*/
    avail_fr_payload = 
      rlp_gendata_maxlen_table[ allowed_rate][ (rlp_rs_offset+rlp_ct_offset)];

    if( rlp_tx_segfr_active)
    {
      /*---------------------------------------------------------------------
        Segmented frame TX in process. Do next segment of segmented Re-xmit,
      ---------------------------------------------------------------------*/
      if( avail_fr_payload > rlp_tx_segfr_remaining_bytes)
      {
        /*-------------------------------------------------------------------
          Last segment for the current Re-xmit frame. Set byte copy count,
          segmented frame indicator and fill extra part of frame.
        -------------------------------------------------------------------*/
        actual_fr_payload            = rlp_tx_segfr_remaining_bytes;
        tx_frame.frame.gen_data.ctl_len = RLP_SEGFR_LASTSEG_MASK;
        (void)memset(
          (void *)(tx_frame.frame.gen_data.data + actual_fr_payload),
          0,
          avail_fr_payload - actual_fr_payload );
      }
      else /* Not last seqment */
      {
        /*-------------------------------------------------------------------
          Set byte copy count and segmented frame indicator.
        -------------------------------------------------------------------*/
        actual_fr_payload            = avail_fr_payload;
        tx_frame.frame.gen_data.ctl_len = RLP_SEGFR_2NDSEG_MASK;
      }
      /*---------------------------------------------------------------------
        Now set seq_num, rate, and data size then copy data bytes to frame.
      ---------------------------------------------------------------------*/
      frame_rate                       = allowed_rate;
      *frame_ptr                       = (byte *)&tx_frame.frame;
      rlp_tx_segfr_remaining_bytes    -= actual_fr_payload;
      tx_frame.frame.gen_data.ctl_len |= actual_fr_payload;
      tx_frame.frame.gen_data.seq_num  = 
        rlp_rexmit_ptr_array[ rlp_state_info.re_xmit_out_index].frame_ptr->
        frame.gen_data.seq_num;
      (void)memscpy( (void *)tx_frame.frame.gen_data.data, 
      	              RLP2_FULL_GEN_DATA_BYTES,
                      rlp_tx_segfr_src_ptr, 
                      actual_fr_payload );
      if(( tx_frame.frame.gen_data.ctl_len & RLP_GEN_CTL_CTL_MASK) == 
           RLP_SEGFR_LASTSEG_MASK)
      {
        /*-------------------------------------------------------------------
          Last segmented frame. Decrement Re-xmit counter.
        -------------------------------------------------------------------*/
        rlp_tx_segfr_active = FALSE;
        rlp_state_info.re_xmit_frames_cnt--;
        rlp_state_info.re_xmit_out_index = 
          (rlp_state_info.re_xmit_out_index + 1) % SAVED_FRAME_ARRAY_SIZ;
      }
      else /* Not last segement of segmented frame */
      {
        rlp_tx_segfr_src_ptr += avail_fr_payload;
      }
    } /* if segmented RX active */
    else if( rlp_rexmit_ptr_array[ rlp_state_info.re_xmit_out_index].frame_ptr
             ->rate == allowed_rate)
    {
      /*---------------------------------------------------------------------
        No segmented TX active and Original frame/rate can be used. Decrement 
        re-xmit frame counter then set rate and increment index.
      ---------------------------------------------------------------------*/
      rlp_state_info.re_xmit_frames_cnt--;
      frame_rate = rlp_rexmit_ptr_array[rlp_state_info.re_xmit_out_index].
                   frame_ptr->rate;
      *frame_ptr = ((byte *)&(rlp_rexmit_ptr_array[ rlp_state_info.
                   re_xmit_out_index].frame_ptr->frame));
      rlp_state_info.re_xmit_out_index 
                 = (rlp_state_info.re_xmit_out_index + 1) % 
                   SAVED_FRAME_ARRAY_SIZ;
    }
    else /* Different frame rate, determine if payload fits into frame     */
    {
      /*---------------------------------------------------------------------
        Determine if the Rexmit frame payload fits the allowed rate or if
        segmented frames must be used. Start this by determining the original
        payload and setting the source byte pointer.
      ---------------------------------------------------------------------*/
      if(( rlp_rexmit_ptr_array[ rlp_state_info.re_xmit_out_index].frame_ptr->
           rate == RLP_FULL_RATE) && 
         ( rlp_rexmit_ptr_array[ rlp_state_info.re_xmit_out_index].
           frame_ptr->pri_frame) &&
         ( rlp_rexmit_ptr_array[ rlp_state_info.re_xmit_out_index].frame_ptr->
           frame.pri_fr_data.data[rlp_prifull_table[rlp_mux].format_b_payload]
           == rlp_prifull_table[rlp_mux].format_b))
      {
        /*-----------------------------------------------------------------
          Rexmit was Primary Full rate B. Set count/source ptr.
        -----------------------------------------------------------------*/
        actual_fr_payload = rlp_prifull_table[rlp_mux].format_b_payload;
        rlp_tx_segfr_src_ptr = (byte *)rlp_rexmit_ptr_array[rlp_state_info.
                         re_xmit_out_index].frame_ptr->frame.pri_fr_data.data;
      }
      else /* Generic frame format, set length and source ptr from frame */
      {
        actual_fr_payload = rlp_rexmit_ptr_array[ rlp_state_info.
                               re_xmit_out_index].frame_ptr->frame.gen_data.
                               ctl_len;
        rlp_tx_segfr_src_ptr = (byte *)rlp_rexmit_ptr_array[ rlp_state_info.
                               re_xmit_out_index].frame_ptr->frame.gen_data.
                               data;
      }
      if( avail_fr_payload >= actual_fr_payload)
      {
        /*-------------------------------------------------------------------
          Re-xmit frame fits into a frame at allowed rate. Prepare frame.
          Start by pre-loading data field with zeros.
        -------------------------------------------------------------------*/
        (void)memset( (void *)rlp_rexmit_ptr_array[
          rlp_state_info.re_xmit_out_index]
                      .frame_ptr->frame.gen_data.data, 0, avail_fr_payload);
        rlp_state_info.re_xmit_frames_cnt--;
        frame_rate = allowed_rate;
        rlp_rexmit_ptr_array[ rlp_state_info.re_xmit_out_index].frame_ptr->
          rate     = (rlp_rate_enum_type) allowed_rate;
        *frame_ptr = (byte *)&( rlp_rexmit_ptr_array[ rlp_state_info.
          re_xmit_out_index].frame_ptr->frame);
        rlp_state_info.re_xmit_out_index = 
          (rlp_state_info.re_xmit_out_index + 1) % SAVED_FRAME_ARRAY_SIZ;
      }
      else /* Use segmented frames for Re-xmit */
      {
        /*-------------------------------------------------------------------
          Set segmented Re-xmit flag then build the 1st segmented frame and
          set conditions for the next segment.
        -------------------------------------------------------------------*/
        rlp_tx_segfr_active             = TRUE;
        tx_frame.rate                   = allowed_rate;
        tx_frame.frame.gen_data.seq_num = rlp_rexmit_ptr_array[rlp_state_info
          .re_xmit_out_index].frame_ptr->frame.gen_data.seq_num;
        tx_frame.frame.gen_data.ctl_len = ( RLP_SEGFR_1STSEG_MASK | 
                                            avail_fr_payload);
        rlp_tx_segfr_remaining_bytes    = actual_fr_payload -
                                            avail_fr_payload;
        *frame_ptr                      = (byte *)&tx_frame.frame;
        frame_rate                      = allowed_rate;
        (void)memscpy( (void *)tx_frame.frame.gen_data.data,
                       RLP2_FULL_GEN_DATA_BYTES,
                       rlp_tx_segfr_src_ptr, 
                       avail_fr_payload );
        rlp_tx_segfr_src_ptr           += avail_fr_payload;
      }
    } /* else different rate        */
  } 
  /*=========================================================================
    Check for DATA FRAME(S) TO SEND.
  =========================================================================*/
  else if( rlp_tx_data_ptr != NULL || ( rlp_tx_watermark_ptr != NULL &&
            rlp_tx_watermark_ptr->current_cnt != 0))
  {
    /*-----------------------------------------------------------------------
      Send new data frame. Start by setting the sequence number then determine
      the available data payload from the channel type or requested rate.
    -----------------------------------------------------------------------*/
    rlp_gen_stats.tx_data_cnt++;
    rlp_saved_fr_array[rlp_state_info.saved_frames_index].
      frame.gen_data.seq_num    = (byte)rlp_state_info.next_seq_num;
    rlp_state_info.next_seq_num = 
                         ( rlp_state_info.next_seq_num + 1) % RLP_SEQ_MODULUS;

    /*-----------------------------------------------------------------------
      Determine the available frame payload based on the channel type and/or
      the required frame rate.
    -----------------------------------------------------------------------*/                 
    if( primary_frame)
    {
      frame_rate       = RLP_FULL_RATE;
      avail_fr_payload = rlp_prifull_table[ rlp_mux].format_b_payload;
    }
    else /* Secondary-Signaling channel */
    {
      frame_rate       = allowed_rate;
      avail_fr_payload = 
                       rlp_gendata_maxlen_table[ allowed_rate]
                                               [(rlp_rs_offset+rlp_ct_offset)];
    }

    /*-----------------------------------------------------------------------
      Get a packetized item if needed then set current packet length and
      determine if a 2nd item should be retrieved.

      This part of code does not modify any global data here and so mutual 
      exclusion is not required. Removed premption macros.
    -----------------------------------------------------------------------*/
    if( rlp_tx_data_ptr == NULL)
    {
      if(( rlp_tx_data_ptr = dsm_dequeue( rlp_tx_watermark_ptr)) == NULL)
      {
        ERR_FATAL( "No TX data item", 0, 0, 0);
      }

    }
    if( avail_fr_payload > 
      ( cur_packet_length = dsm_length_packet( rlp_tx_data_ptr)))
    {
      /*---------------------------------------------------------------------
       Available frame payload bigger than the bytes in the current data item.
       Get another data item if available then set next item copy count.
      ---------------------------------------------------------------------*/
      cur_item_copy_cnt = ( byte)cur_packet_length;
      if(( next_item_ptr = dsm_dequeue( rlp_tx_watermark_ptr)) != NULL)
      {
        /*-------------------------------------------------------------------
          Set next item copy length based on remaining payload available.
        -------------------------------------------------------------------*/

        if(( next_packet_length = dsm_length_packet( next_item_ptr)) > 
           ( avail_fr_payload - cur_item_copy_cnt))
        {
          next_item_copy_cnt = avail_fr_payload - cur_item_copy_cnt;
        }
        else /* next packet length < available payload */
        {
          next_item_copy_cnt = (byte)next_packet_length;
        }
      }
      else /* no more data available */
      {
        next_item_copy_cnt = 0;
      }
    }
    else /* available payload <= current packet length */
    {
      cur_item_copy_cnt  = avail_fr_payload;
      next_item_copy_cnt = 0;
      next_item_ptr      = NULL;
    }
    /*-----------------------------------------------------------------------
      Now set actual frame payload and blank filler count then update shared
      queue current count and re-enable interrupts. Decrement filler count if 
      original frame was to be a Primary full rate data.
    -----------------------------------------------------------------------*/
    actual_fr_payload = cur_item_copy_cnt + next_item_copy_cnt;
    data_filler_cnt   = avail_fr_payload - actual_fr_payload;

    if(( data_filler_cnt != 0) && 
       ( avail_fr_payload == RLP1_FULL_PRI_DATA_BYTES || 
         avail_fr_payload == RLP2_FULL_PRI_DATA_BYTES))
    {
      data_filler_cnt--;
    }
    /*-----------------------------------------------------------------------
      Now prepare the non-payload field(s) of the frame.
    -----------------------------------------------------------------------*/
    if( primary_frame && 
        actual_fr_payload == rlp_prifull_table[ rlp_mux].format_b_payload)
    {
      /*---------------------------------------------------------------------
        Set Type field then set dest_ptr for payload.
      ---------------------------------------------------------------------*/
      rlp_saved_fr_array[rlp_state_info.saved_frames_index].frame.pri_fr_data.
        data[rlp_prifull_table[rlp_mux].format_b_payload] = 
        rlp_prifull_table[ rlp_mux].format_b;
      dest_ptr = (byte *)rlp_saved_fr_array[
        rlp_state_info.saved_frames_index].
        frame.pri_fr_data.data;
    }
    else /* Generic frame format */
    {
      if( primary_frame)
      {
        rlp_saved_fr_array[rlp_state_info.saved_frames_index].frame.
        pri_fr_data.data[rlp_prifull_table[rlp_mux].format_b_payload] = 
        rlp_prifull_table[ rlp_mux].format_a;
      }
      dest_ptr = (byte *)rlp_saved_fr_array[
        rlp_state_info.saved_frames_index].frame.
        gen_data.data;
      rlp_saved_fr_array[rlp_state_info.saved_frames_index].frame.gen_data.
         ctl_len = actual_fr_payload;
      if( data_filler_cnt != 0)
      {
        /*-------------------------------------------------------------------- 
          Below check is just to fix Klockwork error.This works because RLP
          MUX option is always set to RLP_MUX_OPTION1 and hence payload size
          is RLP1_FULL_PRI_DATA_BYTES
          ------------------------------------------------------------------*/
        if( (actual_fr_payload+data_filler_cnt) < RLP2_FULL_GEN_DATA_BYTES )
        {
          (void)memset( (byte *)rlp_saved_fr_array[
            rlp_state_info.saved_frames_index]
                        .frame.gen_data.data + actual_fr_payload, 0,
                        data_filler_cnt);
        }             
      }             
    } /* else use generic data frame format */
    /*-----------------------------------------------------------------------
      Now set the frame pointer, rate and channel type then do the memcpy(s). 
      If 2 data items are referenced then do 2 memcpys.
    -----------------------------------------------------------------------*/
    *frame_ptr 
      = (byte *)&rlp_saved_fr_array[rlp_state_info.saved_frames_index].frame;
    rlp_saved_fr_array[rlp_state_info.saved_frames_index].pri_frame 
      = primary_frame;
    rlp_saved_fr_array[rlp_state_info.saved_frames_index].rate 
      = frame_rate;
    /*-----------------------------------------------------------------------
     Move the data bytes from 'packet's to the Saved Frame array. NOTE: All
     fields of source pointers are handled with the extract function.
    -----------------------------------------------------------------------*/
    (void)dsm_pullup( &rlp_tx_data_ptr, dest_ptr, cur_item_copy_cnt);

    if( next_item_copy_cnt != 0)
    {
      (void)dsm_pullup( &next_item_ptr, dest_ptr + cur_item_copy_cnt, 
                         next_item_copy_cnt);
      rlp_tx_data_ptr = next_item_ptr;
    }
    /*-----------------------------------------------------------------------
      Re-establish saved packet pointer then update Saved Frame array count
      if not now full.
    -----------------------------------------------------------------------*/
    if( rlp_state_info.saved_frames_cnt < SAVED_FRAME_ARRAY_SIZ)
    {
      rlp_state_info.saved_frames_cnt++;
    }
    rlp_state_info.saved_frames_index = (rlp_state_info.saved_frames_index + 1)
                                        % SAVED_FRAME_ARRAY_SIZ;
  }
  /*=========================================================================
    No data frame to xmit either. Prepare to send an Idle data frame.
  =========================================================================*/
  else if( primary_frame)
  {
    /*-----------------------------------------------------------------------
      Send Primary channel idle; Send 1/8 rate frame with seq number
    -----------------------------------------------------------------------*/
    tx_frame.frame.pri_idle.seq_num = (byte)rlp_state_info.next_seq_num;
    tx_frame.frame.pri_idle.nr_crc  = nr_table[rlp_state_info.next_seq_num];
    frame_rate                      = RLP_EIGHTH_RATE;
    *frame_ptr                      = ( byte *)&tx_frame.frame;
    if( rlp_mux == RLP_MUX_OPTION2)
    {
      tx_frame.frame.pri_idle.rs2_filler = 0;
    }
  }
  else /* Send Sec/Sig channel Idle (zero length data frame) */
  {
    frame_rate                      = allowed_rate;
    *frame_ptr                      = ( byte *)&tx_frame.frame;
    tx_frame.rate                   = allowed_rate;
    tx_frame.pri_frame              = primary_frame;
    tx_frame.frame.gen_data.seq_num = (byte)rlp_state_info.next_seq_num;
    (void)memset( (void *)&tx_frame.frame.gen_data.ctl_len, 0,
                  RLP2_FULL_GEN_DATA_BYTES + 1);
  }

  /*-------------------------------------------------------------------------
    Now update RLP statistics then return the frame rate.
  -------------------------------------------------------------------------*/
  rlp_tx_stat[ frame_rate][(rlp_rs_offset + rlp_ct_offset)]++;
  rlp_tx_stat_tot[ frame_rate][(rlp_rs_offset + rlp_ct_offset)]++;
   
  /*-------------------------------------------------------------------------
    Leave critical section.
  -------------------------------------------------------------------------*/
  DS_LEAVE_CRIT_SECTION(&rlp_crit_sect);	  
  return(frame_rate);
} /* rlp_tx_get_next_frame */


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

void rlp_get_stats
(
  rlp_ext_stats_type *stats_ptr      /* pointer to item to load with stats */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  word rx_stat_total;            /* total of rlp_rx_stat_tot[][] */
  word tx_stat_total;            /* total of rlp_tx_stat_tot[][] */
  int i,j; /* used to to index array while totalling values */
  
  /*-------------------------------------------------------------------------
   Calculate the total number of rx and tx frames of all types
  -------------------------------------------------------------------------*/
  rx_stat_total = 0;
  for (i = 0; i < (RLP_ERASURE+1); i++) {
    for (j = 0; j < 4; j++) {
      rx_stat_total += rlp_rx_stat_tot[i][j];
    }
  }
  tx_stat_total = 0;
  for (i = 0; i < (RLP_ERASURE+1); i++) {
    for (j = 0; j < 4; j++) {
      tx_stat_total += rlp_tx_stat_tot[i][j];
    }
  }
  
  /*-------------------------------------------------------------------------
   Store the latest values to the passed struct pointer.
  -------------------------------------------------------------------------*/
  stats_ptr->tx_frame_cnt = tx_stat_total;
  stats_ptr->rx_frame_cnt = rx_stat_total;
  stats_ptr->tx_data_cnt  = (word) rlp_gen_stats.tx_data_cnt;
  stats_ptr->rx_data_cnt  = (word) rlp_gen_stats.rx_data_cnt;
  stats_ptr->nak_cnt      = (byte) rlp_gen_stats.single_naks;
  stats_ptr->rexmit_cnt   = (byte) rlp_gen_stats.re_xmits;
  stats_ptr->state        = rlp_state_info.state;

} /* rlp_get_stats */


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
void rlp_get_stats_at
(
  word *stats_ptr  /* pointer to array to load with stats */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  word gen_total; /* total of rlp_gen_rx_errs array */
  int rate, type; /* loop iterators */

  /*-------------------------------------------------------------------------
   Store the latest values to the passed struct pointer.
  -------------------------------------------------------------------------*/
  stats_ptr[RLP_STATS_RX_DATA_CNT] = (word)rlp_gen_stats.rx_data_cnt;
  stats_ptr[RLP_STATS_TX_DATA_CNT] = (word)rlp_gen_stats.tx_data_cnt;
  stats_ptr[RLP_STATS_NAKS]        = (word)rlp_gen_stats.single_naks;
  stats_ptr[RLP_STATS_DOUBLE_NAKS] = (word)rlp_gen_stats.double_naks;
  stats_ptr[RLP_STATS_TRIPLE_NAKS] = (word)rlp_gen_stats.triple_naks;
  stats_ptr[RLP_STATS_REXMITS]     = (word)rlp_gen_stats.re_xmits;
  stats_ptr[RLP_STATS_SEQ_TIMEOUT_CNT]    = (word)rlp_gen_stats.seq_timeout_cnt;
  stats_ptr[RLP_STATS_RE_XMITS_MISSED]    = (word)rlp_gen_stats.re_xmits_missed;
  stats_ptr[RLP_STATS_RE_XMITS_NOT_FOUND] = (word)rlp_gen_stats.re_xmits_not_found;
  stats_ptr[RLP_STATS_LARGEST_RE_XMIT]    = (word)rlp_gen_stats.largest_re_xmit;
  stats_ptr[RLP_STATS_FILL_FRAMES_RXED]   = (word)rlp_gen_stats.fill_frames_rxed;
  stats_ptr[RLP_STATS_IDLE_FR_ERR]        = (word)rlp_pri_frate_errs.idle_fr_err;
  stats_ptr[RLP_STATS_FULL_TYPE_ERR]      = (word)rlp_pri_frate_errs.full_type_err;
  stats_ptr[RLP_STATS_RX_SEG_FR_ERRS]     = (word)rlp_pri_frate_errs.rx_seg_fr_errs;
  stats_ptr[RLP_STATS_RESETS]    = (word)rlp_gen_stats.resets;

  /* Total up all the values in the array, rlp_gen_rx_errs. */
  gen_total = 0;
  for (rate = 0; rate < RLP_ERASURE; rate++)
    {
      for (type = 0; type < 3; type++)
        {
          gen_total += (word)rlp_gen_rx_errs[rate][type];
        }
    }
  stats_ptr[RLP_STATS_GEN_RX_ERRS] = gen_total;

  /* Total up all the erasures in the array, rlp_rx_stat_tot. */
  gen_total = 0;
  for (type = 0; type < 4; type++)
    {
      gen_total += (word)rlp_rx_stat_tot[RLP_ERASURE][type];
    }
  stats_ptr[RLP_STATS_ERASURES] = gen_total;

  stats_ptr[RLP_STATS_LARGEST_CONTIG_ERASE] =
    rlp_gen_stats.largest_contig_erase;

  stats_ptr[RLP_STATS_LAST_RTT]    = (word)rlp_last_rtt;
  rlp_last_call_synced_ptr = (byte *) rlp_last_call_synced[ds_last_call_var];

} /* rlp_get_stats_at */

/*===========================================================================

FUNCTION RLP_RESET_STATS

DESCRIPTION
  Resets the stats which can be reported using rlp_get_stats_at.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rlp_reset_stats(void)
{
  (void)memset( rlp_rx_stat, 0, sizeof( rlp_rx_stat));
  (void)memset( rlp_tx_stat, 0, sizeof( rlp_tx_stat));
  (void)memset( rlp_rx_stat_tot, 0, sizeof( rlp_rx_stat_tot));
  (void)memset( rlp_tx_stat_tot, 0, sizeof( rlp_tx_stat_tot));
  rlp_gen_stats.single_naks = 0;
  rlp_gen_stats.double_naks = 0;
  rlp_gen_stats.triple_naks = 0;
  rlp_gen_stats.re_xmits = 0;
  rlp_gen_stats.seq_timeout_cnt = 0;
  rlp_gen_stats.re_xmits_missed = 0;
  rlp_gen_stats.re_xmits_not_found = 0;
  rlp_gen_stats.rx_data_cnt = 0;
  rlp_gen_stats.tx_data_cnt = 0;
  rlp_gen_stats.largest_re_xmit = 0;
  rlp_gen_stats.fill_frames_rxed = 0;
  rlp_gen_stats.resets = 0;
  rlp_pri_frate_errs.idle_fr_err = 0;
  rlp_pri_frate_errs.full_type_err = 0;
  rlp_pri_frate_errs.rx_seg_fr_errs = 0;
  rlp_gen_stats.largest_contig_erase = 0;
  /* Don't alter rlp_state_info.rlp_delay. */
  (void)memset( rlp_gen_rx_errs, 0, sizeof( rlp_gen_rx_errs ) );
}
#endif /* FEATURE_DS_PSTATS */

#endif /* !FEATURE_IS95B_MDR */

#endif /* FEATURE_DS */

/*===========================================================================

FUNCTION RLP_ESTABLISH_RATE_SET

DESCRIPTION
  This function establishes the rate set to operate RLP. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function will cause an rlp_reset that will reset the RLP state.
===========================================================================*/

void rlp_establish_rate_set
(
  rlp_mux_option_type rate_set               /* Rate Set to operate RLP at */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifndef FEATURE_IS95B_MDR

  rlp_mux = rate_set;

  if( rate_set == RLP_MUX_OPTION1)
  {
    rlp_rs_offset = 0;
  }
  else /* must be establishing Rate Set 2 */
  {
    rlp_rs_offset = 2;
  }

  rlp_reset();
#else
  DATA_RLP_MSG0(MSG_LEGACY_HIGH,
                "RLP Establish rate set not supported for IS95B MDR ");
#endif  /* !FEATURE_IS95B_MDR */
} /* rlp_establish_rate_set */
