/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M E D I U M   D A T A   R A T E
                     R A D I O   L I N K   P R O T O C O L

GENERAL DESCRIPTION
  This file contains the implementation of the Medium Data Rate Radio Link
  Protocol (RLP Type 2), as defined in IS-707-A. The implementation is
  backward compatible with the Radio Link Protocol defined in IS-707.
  
EXTERNALIZED FUNCTIONS
  rlp_init()
    Initializes the RLP state machine. This function must be called prior to
    any other RLP function.

  rlp_rx_process_frames()
    Processes one or more RLP frames received over the air.

  rlp_tx_get_next_frame()
    Retrieves the next RLP frame to be transmitted over the air.

  rlp_reset()
    Resets all the RLP state variables to prepare for next call.

  rlp_reg_srvc()
    Sets up shared buffers for passing rx and tx data between RLP and the
    upper layer.

  rlp_establish_srvc_type()
    Sets up the Traffic Type, Rate Set and RLP Type to be used.

  ds_process_rlp_rx_frames()
   Called by mux every 20 msecs for RLP 1 & RLP 2 calls.  This function
   then passes each frame down to RLP and sets/clears the idle bits
   appropriately.
 
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The RLP state machine has several requirements that must be followed for
  the proper operation of the RLP protocol software. These initialization
  and sequencing requirements are listed below.

  rlp_init() must be called (on power-up) prior to calling any other RLP
  function.

  rlp_reset() must be called at the end of each data call, and whenever
  any of the service parameters established by rlp_establish_srvc_type()
  are changed.

  rlp_reg_srvc() must be called at the beginning of a data call, prior to
  invoking either rlp_rx_process_frames() or rlp_tx_get_next_frame(). This
  function must be called with all non-NULL parameters.

  rlp_establish_srvc_type() must be called at least once at the beginning
  of a data call, and subsequently, each time the traffic type, rate set or
  RLP type in use changes. This function is typically called at the end of
  service negotiation with the base station. rlp_rx_process_frames() and
  rlp_tx_get_next_frame() should NOT be invoked without first calling
  rlp_establish_srvc_type().

Copyright (c) 1998-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: O:/src/asw/COMMON/vcs/mdrrlp.c_v   1.3   08 Nov 2002 14:14:04   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/mdrrlp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/09/14   sd      Fixed critical section release.
11/14/13   vm      Fixed KW errors
08/06/13   sd      Replaced memcpy with memscpy.
13/06/12   msh     RLP reset on phys link down
12/16/11   ss      Compiler Warnings fixed.
28/10/11   ms      Fix compiler errors when MDR feature is turned off. 
06/17/11   vkp     Update CTA timer for SO15 data transfer  
01/18/11   op      Migrated to MSG 2.0 Macros
11/22/10   sn      Replaced REX premption macros with critical section for 
                   mutual exclusion.
06/24/10   rp      Fixed Complier warnings.  
04/01/10   ms      CMI SU level API defeaturization.
09/30/09   ss      Klocwork error fixes.
05/15/09   ls      Fix the lint issue
01/20/09   pp      Metainfo clean-up.
04/23/08   sn      Fixed klocwork error.
11/14/07   sn      Replaced correct array index macro for arrays which hold 
                   frame counts.
09/11/07   sn      Fixed array overrun issue.
05/10/07   ac      Lint fix
03/05/07   spn     Fixed lint errors
03/05/07   ac      Merege the change in the EMPA sandBox 5209 to the tip
12/15/05    TMR    Changed RLP RX queue to watermark and added parameter to 
                   dsrlp_reg_srvc() and RX callback function
11/15/05   atp     Moved and redesigned ds_process_rlp_rx_frames().
04/05/05    gr     Fixed the issue of populating hdlc_inst_ptr for MDRRLP
02/03/04    ak     Meta info now inside dsm item data block, so extract & 
                   pull up appropriately.
11/08/02   atp     Added freeing of meta_info to prevent memory leak.
07/15/02   ak      Tab cleanup.  Some code under FEATURE_DATA_MM.
07/23/01   pjb     Change location/prefix of command names.   
                   Fixed used field corruption 
08/25/00   rc      Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK
06/07/00   rc      Removed ovld of link ptr in dsm items.
03/08/00   igt     added a memset() to fill zeroes into control frames
12/23/99   na      Changed q_puts that use dsm items, to use the ovld of link
                   ptr in dsm items.
08/17/99   smp     Added include files msg.h & err.h.
08/10/99   smp     Print some more debug info (via F3 messages) when a hole
                   is detected.
07/08/99   smp     The compiler warning "actual_fr_payload may be used before
                   being set" turned out to be a real bug. Fixed the code to
                   ensure that actual_fr_payload is set for all the possible
                   cases, in rlp_tx_get_next_frame().
06/11/99   smp     Changed some MSGs (shortened strings/changed levels). For
                   RLP Type 2, after reassembly of a segmented data frame is
                   complete, set the frame type to RLP_REXMIT_DATA_FR (was
                   being incorrectly set to RLP_UNSEG_DATA_FR).
06/08/99   smp     Fixed a problem where L_V(R) was being set to L_SEQ+1 for
                   rx'ed Idle & NAK frames (should be set equal to L_SEQ). Also
                   changed some MSG's from MSG_HIGH to MSG_LOW.
06/02/99   smp     For RLP Type 2, Type A re-transmitted frames were not being
                   processed properly, due to the wrong mask (0x7F instead of 0x3F)
                   being used to extract the length field from the frame. This is
                   now fixed.
05/21/99   smp     In process_sequence_num(), when an 'abort' occurred, the current
                   nak_ptr was being deleted from the rlp_nak_q, and then being
                   used to get the next NAK list entry, which would be incorrect.
                   Fixed this by first getting the next entry and then deleting the
                   current one. Also fixed the problem where the 12-bit sequence
                   number (instead of 8-bit) was being used to lookup the NR CRC
                   of the RLP Idle frame to be transmitted.
04/09/99   hcg     Added FEATURE_Q_NO_SELF_QPTR to support queue services memory 
                   optimization.
04/06/99   smp     Included comdef.h before the #ifdef that surrounds the
                   entire file, in order to fix a compiler warning (no external
                   declaration in translation unit).
03/08/99   smp     More MSG fixes/enhancements - added/changed parameters to
                   some MSG macros.
02/18/99   smp     Changed the RLP saved frame array size back to 64 from 128.
                   We don't really need a bigger transmit re-sequencing buffer
                   for MDR RLP, since reverse supplementals are not supported.
02/10/99   smp     Re-formatted some MSG strings that were too long to be
                   displayed on the Diagnostic Monitor F3 screen.
01/27/99   smp     Added missing semi-colons to lines 4458 and 4463 (in
                   rlp_establish_srvc_type() ).
01/19/99   smp     Changed some ERR's to MSG_HIGH's since these are not
                   critical errors.
01/05/99   smp     Re-sized RLP Rx/Tx stat arrays to have RLP_BLANK+1 rows
                   (instead of RLP_ERASURE+1) since the Mux Layer now also
                   passes up Blank frames to RLP.
09/22/98   smp     Initial version of MDR RLP implementation, based on rlp.c
                   revision 1.32.

===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"                        /* Customer Specific Features */

#include "mdrrlp_api.h"
#include "data_msg.h"
#include "data_err_suppr.h"
#include "ds707_cta_rlp.h"

#ifdef FEATURE_DS

#include "rlp.h"
#ifdef FEATURE_IS95B_MDR
                                  /* Compile this file only for MDR builds */

#include "ps.h"
#include "mdrrlp.h"
#include "dsm.h"
#include "crc.h"
#include <memory.h>
#include "bit.h"
#include "msg.h"
#include "err.h"
#include "dsutil.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  General RLP definitions
---------------------------------------------------------------------------*/
#define RLP_MAX_RTT_SETUP       900   /* Maximum time (in frames) to synch
                                         up RLP. Set real high for MTOM
                                         Korea builds.                     */
#define RLP_RTT_MARGIN            4   /* Round trip frame processing margin
                                         as determined thru testing        */
#define RLP_MAX_HOLE_AGE         50   /* Maximum Age for a missing frame
                                          (i.e. give up at this age)        */
#define RLP_LEN_0                 0   /* For zero length data fields        */

/*---------------------------------------------------------------------------
  RLP Sequence Number Modulus. Used for all sequence number arithmetic.
---------------------------------------------------------------------------*/
#define RLP_TYPE1_SEQ_MODULUS   256   /* 8-bit sequence number, 0 to 255   */
#define RLP_TYPE2_SEQ_MODULUS  4096   /* 12-bit sequence number, 0 to 4095 */

/*---------------------------------------------------------------------------
  RLP Maximum Contiguous Erasures. If the number of consecutive erasures
  exceeds this maximum, RLP should be reset (as per IS-707/IS-707-A).
---------------------------------------------------------------------------*/
#define RLP_TYPE1_MAX_CONTIG_ERASE  127   /* Per IS-707.2, Section 3.1.3   */
#define RLP_TYPE2_MAX_CONTIG_ERASE  255   /* Per IS-707-A.8, Section 3.1.3 */

/*---------------------------------------------------------------------------
  Maximum difference allowed between V(N) and V(R). The difference between
  V(N) and V(R) must not be greater than 1/2 the sequence number space. The
  maximum allowed delta is set to a lower value here, in order to limit the
  RAM consumption.
---------------------------------------------------------------------------*/
#define RLP_TYPE1_MAX_VN_VR_DELTA    64
#define RLP_TYPE2_MAX_VN_VR_DELTA   256

/*---------------------------------------------------------------------------
  Definitions for RLP frame fields and masks
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Generic frame field masks
---------------------------------------------------------------------------*/
#define RLP_UNSEGDATA_UPPR_BITS  0       /* Upper 2 bits for unseg data    */
#define RLP_REXMIT_UPPR_BITS     0x40    /* Upper 2 bits for rexmit data   */
#define RLP_CONTROL_UPPR_BITS    0xC0    /* Upper 2 bits for control frame */
#define RLP_SEGDATA_UPPR_BITS    0x80    /* Upper 2 bits for segmented data*/
#define RLP_GEN_CTL_CTL_MASK     0xF0    /* Mask to get control field      */
#define RLP_GEN_CTL_LEN_MASK     0x0F    /* Mask to get length field       */
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

/*---------------------------------------------------------------------------
  Mask to get the Length field of an Unsegmented Data Frame (i.e. Generic
  Data Frame). This is different for RLP Type 1 & RLP Type 2.
---------------------------------------------------------------------------*/
#define RLP_TYPE1_GEN_DATA_LEN_MASK    0x7F
#define RLP_TYPE2_GEN_DATA_LEN_MASK    0x3F

/*---------------------------------------------------------------------------
  CTL field values for various types of IS-707 (RLP Type 1) Control frames.
  The length of the CTL field is 8 bits.
---------------------------------------------------------------------------*/
#define RLP_TYPE1_NAK             0xC0 /* NAK Frame                        */
#define RLP_TYPE1_SYNC            0xD0 /* Non-Encrypted Mode SYNC Frame    */
#define RLP_TYPE1_ACK             0xE0 /* Non-Encrypted Mode ACK Frame     */
#define RLP_TYPE1_SYNC_ACK        0xF0 /* Non-Encrypted Mode SYNC/ACK Frame*/

/*---------------------------------------------------------------------------
  Definition for IS-707 (RLP Type 1) Control frame size.
---------------------------------------------------------------------------*/
#define RLP_TYPE1_CTL_FRAME_SIZE    6

/*---------------------------------------------------------------------------
  CTL field values for various types of IS-707-A (RLP Type 2) Control frames.
  The length of the CTL field is 6 bits, left justified in a byte i.e. the
  2 LSB's of the values defined below should be masked off.
---------------------------------------------------------------------------*/
#define RLP_TYPE2_NAK            0xC0 /* NAK Frame                         */
#define RLP_TYPE2_SYNC           0xD8 /* Non-Encrypted Mode SYNC Frame     */
#define RLP_TYPE2_ACK            0xE8 /* Non-Encrypted Mode ACK Frame      */
#define RLP_TYPE2_SYNC_ACK       0xF8 /* Non-Encrypted Mode SYNC/ACK Frame */

#define RLP_TYPE2_CTL_MASK       0xFC /* Mask to get CTL bits              */

/*---------------------------------------------------------------------------
  NAK_TYPE values for IS-707-A (RLP Type 2) NAK frames. The length of the
  NAK_TYPE field is 2 bits, right justified in a byte i.e. the 6 MSB's of
  the values defined below should be masked off.
---------------------------------------------------------------------------*/
#define RLP_TYPE2_NAK_TYPE0      0x00 /* NAK Type 0 - First/Last NAK       */
#define RLP_TYPE2_NAK_TYPE1      0x01 /* NAK Type 1 - Bitmap NAK           */

#define RLP_TYPE2_NAK_TYPE_MASK  0x03 /* Mask to get NAK_TYPE bits         */

/*---------------------------------------------------------------------------
  Definitions for various IS-707-A (RLP Type 2) Control frame sizes.
---------------------------------------------------------------------------*/
#define RLP_TYPE2_INIT_FRAME_SIZE  4 /* Non-Encrypted Mode Init Frame Size */
#define RLP_TYPE2_NAK0_FRAME_SIZE  8 /* NAK Type 0 Frame Size              */
#define RLP_TYPE2_NAK1_MAP1_SIZE   8 /* Size of NAK Type 1 with 1 NAK Map  */
#define RLP_TYPE2_NAK1_MAP2_SIZE  10 /* Size of NAK Type 1 with 2 NAK Maps */
#define RLP_TYPE2_NAK1_MAP3_SIZE  13 /* Size of NAK Type 1 with 3 NAK Maps */
#define RLP_TYPE2_NAK1_MAP4_SIZE  15 /* Size of NAK Type 1 with 4 NAK Maps */

/*---------------------------------------------------------------------------
  Lookup table for IS-707-A (RLP Type 2) NAK Type 1 frame sizes. This table
  should be indexed by the NAK Map Count to determine the NAK frame size.
---------------------------------------------------------------------------*/
LOCAL const byte rlp_type2_nak1_size_table[4] = { RLP_TYPE2_NAK1_MAP1_SIZE,
                                                  RLP_TYPE2_NAK1_MAP2_SIZE,
                                                  RLP_TYPE2_NAK1_MAP3_SIZE,
                                                  RLP_TYPE2_NAK1_MAP4_SIZE };

/*---------------------------------------------------------------------------
  Mask to get the NAK Map Count bits.
---------------------------------------------------------------------------*/
#define RLP_TYPE2_NAK_MAP_CNT_MASK  0x0C

/*---------------------------------------------------------------------------
  Definition for the Maximum number of NAK Maps that can be contained in a
  Type 1 NAK frame, for IS-707-A (RLP Type 2).
---------------------------------------------------------------------------*/
#define RLP_TYPE2_MAX_NAK_MAPS         4

/*---------------------------------------------------------------------------
  Definitions for the field positions of the fields in a IS-707-A Type 0 NAK
  frame. The field position is computed as a bit offset from the start of the
  frame. These offsets are used to pack and unpack the fields in the NAK.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_TYPE2_NAK0_SEQ_FPOS         = 0,
  RLP_TYPE2_NAK0_CTL_FPOS         = 8,
  RLP_TYPE2_NAK0_NAK_TYPE_FPOS    = 14,
  RLP_TYPE2_NAK0_L_SEQ_HI_FPOS    = 16,
  RLP_TYPE2_NAK0_FIRST_FPOS       = 20,
  RLP_TYPE2_NAK0_LAST_FPOS        = 32,
  RLP_TYPE2_NAK0_PAD1_FPOS        = 44,
  RLP_TYPE2_NAK0_FCS_FPOS         = 48,
  RLP_TYPE2_NAK0_PAD2_FPOS        = 64
} rlp_type2_nak0_fpos_enum_type;

/*---------------------------------------------------------------------------
  Definitions for the lengths of various fields in the IS-707-A Type 0 NAK,
  in bits.
---------------------------------------------------------------------------*/
#define RLP_TYPE2_NAK0_FIRST_LEN      12
#define RLP_TYPE2_NAK0_LAST_LEN       12

/*---------------------------------------------------------------------------
  Definitions for the field positions of the fields in a IS-707-A Type 1 NAK
  frame. The field position is computed as a bit offset from the start of the
  frame. These offsets are used to pack and unpack the fields in the NAK.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_TYPE2_NAK1_SEQ_FPOS         = 0,
  RLP_TYPE2_NAK1_CTL_FPOS         = 8,
  RLP_TYPE2_NAK1_NAK_TYPE_FPOS    = 14,
  RLP_TYPE2_NAK1_L_SEQ_HI_FPOS    = 16,
  RLP_TYPE2_NAK1_NAK_MAP_CNT_FPOS = 20,
  RLP_TYPE2_NAK1_NAK_MAP_FPOS     = 22
} rlp_type2_nak1_fpos_enum_type;

/*---------------------------------------------------------------------------
  Definitions for the lengths of various fields in the IS-707-A Type 1 NAK,
  in bits.
---------------------------------------------------------------------------*/
#define RLP_TYPE2_NAK1_SEQ_LEN            8
#define RLP_TYPE2_NAK1_CTL_LEN            6
#define RLP_TYPE2_NAK1_NAK_TYPE_LEN       2
#define RLP_TYPE2_NAK1_L_SEQ_HI_LEN       4
#define RLP_TYPE2_NAK1_NAK_MAP_CNT_LEN    2
#define RLP_TYPE2_NAK1_NAK_MAP_SEQ_LEN   12
#define RLP_TYPE2_NAK1_NAK_MAP_LEN        8

/*---------------------------------------------------------------------------
  RLP rate set & channel type array index size.
---------------------------------------------------------------------------*/
#define RLP_RATE_SET_CHN_TYPE_INDEX       4

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
  RLP_FRAME_CHK_VLDN_MAX
} rlp_gen_fr_chk_enum_type;

/*---------------------------------------------------------------------------
  Type to represent the different types of RLP frames.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_CONTROL_FR,                          /* Control Frame                */
  RLP_UNSEG_DATA_FR,                       /* Unsegmented Data Frame       */
  RLP_SEG_DATA_FR,                         /* Segmented Data Frame         */
  RLP_IDLE_FR,                             /* Primary Idle Frame           */
  RLP_FILL_FR,                             /* Intersegment Fill Frame      */
  RLP_REXMIT_DATA_FR,                      /* Retransmitted Data Frame     */
  RLP_INIT_FR,                             /* SYNC, ACK or SYNC/ACK Frame  */
  RLP_NAK_TYPE0_FR,                        /* NAK Type 0 Frame             */
  RLP_NAK_TYPE1_FR                         /* NAK Type 1 Frame             */
} rlp_fr_type;

/*---------------------------------------------------------------------------
  Macro to determine if a frame is a Control frame. Will evaluate to TRUE
  if the frame type is SYNC, ACK, SYNC/ACK, NAK Type 0 or NAK Type 1.
---------------------------------------------------------------------------*/
#define IS_CTL_FR(frame_type)  ( (frame_type == RLP_INIT_FR) || \
                                 (frame_type == RLP_NAK_TYPE0_FR) || \
                                 (frame_type == RLP_NAK_TYPE1_FR) )

/*---------------------------------------------------------------------------
  Macro to determine if a frame is a NAK frame. Will evaluate to TRUE if
  the frame type is NAK Type 0 or NAK Type 1.
---------------------------------------------------------------------------*/
#define IS_NAK_FR(frame_type)  ( (frame_type == RLP_NAK_TYPE0_FR) || \
                                 (frame_type == RLP_NAK_TYPE1_FR) )

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
  Type for frame pointers items. These items are used to hold pointers to
  the list of re-xmit frames.
---------------------------------------------------------------------------*/
typedef struct
{
  rlp_frame_type *frame_ptr;                    /* pointer to an RLP frame */
} rlp_frame_ptr_type;

/*---------------------------------------------------------------------------
  Type for NAK Map entry. Used in Type 1 NAKs, which can contain up to 4 NAK
  Map entries. Each NAK Map entry contains the first missing sequence number,
  and a bitmap to identify additional missing frames. The MSB of the bitmap
  corresponds to [(first_seq+1) modulo 4096], and so on. A bit set to '1' in
  the bitmap indicates that the corresponding sequence number must be NAK'ed.
---------------------------------------------------------------------------*/
typedef struct
{
  word first_seq;           /* First sequence number to NAK                */
  byte bitmap;              /* Bitmap identifying additional frames to NAK */
} nak_map_entry_type;

/*---------------------------------------------------------------------------
  Definitions for NAK list
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Type for NAK list entry. The NAK list is used to keep track of missing
  RLP data frames, and the associated retransmission/abort timers.
---------------------------------------------------------------------------*/
typedef struct
{
  q_link_type    link;       /* Queue link used to queue/dequeue the entry */
  boolean        tx_done;    /* TRUE => NAK transmission complete          */
  word           seq;        /* Sequence number of the missing data frame  */
  byte           age;        /* Age of the NAK list entry, in frames       */
  byte           repeat_cnt; /* Number of times to NAK this sequence number*/
} rlp_nak_list_entry_type;

/*---------------------------------------------------------------------------
  NAK list size. Sets a limit on the maximum number of missing data frames
  allowed at any given time.
---------------------------------------------------------------------------*/
#define RLP_NAK_LIST_SIZE    64

/*---------------------------------------------------------------------------
  Statically allocated array of NAK list entries.
---------------------------------------------------------------------------*/
LOCAL rlp_nak_list_entry_type rlp_nak_buffer[RLP_NAK_LIST_SIZE];

/*---------------------------------------------------------------------------
  Pool of 'free' NAK list entries. Each time a new NAK list entry is created,
  the entry is borrowed from this pool. When the NAK list entry is deleted,
  the entry is returned to this pool.
---------------------------------------------------------------------------*/
LOCAL q_type rlp_free_naks_q;

/*---------------------------------------------------------------------------
  The NAK list data structure. The NAK list contains one entry (the NAK list
  entry type defined above) for each missing data frame. Uses Queue Services
  for linked list management.
---------------------------------------------------------------------------*/
LOCAL q_type rlp_nak_q;

/*---------------------------------------------------------------------------
  Count of the number of frames to NAK. This count is updated by the RLP Rx
  processing every 20ms, and checked in the RLP Tx processing in order to
  determine whether or not there are any NAKs to send.
---------------------------------------------------------------------------*/
LOCAL byte naks_to_send_cnt = 0;

/*---------------------------------------------------------------------------
  RLP control block definition. This structure contains all RLP operational
  information.
---------------------------------------------------------------------------*/
typedef struct
{
  rlp_state_enum_type state;    /* RLP State                               */

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
  RLP traffic type (primary or secondary) for the current call
---------------------------------------------------------------------------*/
LOCAL rlp_traf_type_enum_type rlp_traffic_type;

/*---------------------------------------------------------------------------
  The Rate Set in use for the current call
---------------------------------------------------------------------------*/
LOCAL rlp_rate_set_enum_type rlp_rate_set;

/*---------------------------------------------------------------------------
  The type of RLP (IS-707 or IS-707-A) in use for the current call
---------------------------------------------------------------------------*/
LOCAL rlp_type_enum_type rlp_type;

/*---------------------------------------------------------------------------
  Variables to support indexing into Rate Set data structures. Default values
  represent Rate Set 1 on Primary Channel.
---------------------------------------------------------------------------*/
LOCAL byte rlp_rs_offset = 0;     /* Rate Set Offset: 0 = RS1, 2 = RS2     */
LOCAL byte rlp_ct_offset = 0;     /* Channel type Offset: 0 = Pri, 1 = Sec */
LOCAL byte rlp_params_offset = 0; /* = rlp_rs_offset + rlp_ct_offset       */

/*---------------------------------------------------------------------------
  Type and array for a list containing the sequence numbers and frame types
  of the valid Idle, Control and Data frames rx'ed in a 20ms slot, and a count
  of these frames. This list is used to check for holes in the rx'ed data
  stream.
---------------------------------------------------------------------------*/
typedef struct
{
  rlp_rate_enum_type  frame_rate;
  rlp_fr_type         frame_type;
  rlp_seq_num_type    frame_seq;
  byte                ch_num;
  byte                frame_data[3];
} rlp_rx_frame_list_type;

LOCAL rlp_rx_frame_list_type rlp_rx_frame_list[8];
LOCAL byte rlp_rx_frame_count;

/*---------------------------------------------------------------------------
  Variable to indicate if a new data frame or valid idle frame was received
  in the current 20ms slot. Used to determine whether or not to age the NAK
  timers.
---------------------------------------------------------------------------*/
LOCAL boolean rlp_new_or_idle_rcvd;

/*---------------------------------------------------------------------------
  The following variables are set at the beginning of a call when the type of
  RLP to be used (RLP Type 1 or RLP Type 2) is determined. These variables
  are not changed for the remainder of the call, unless the RLP Type changes.
  The default values are set to the RLP Type 1 values.
---------------------------------------------------------------------------*/
LOCAL word RlpSeqModulus     = RLP_TYPE1_SEQ_MODULUS;
LOCAL byte RlpMaxContigErase = RLP_TYPE1_MAX_CONTIG_ERASE;
LOCAL word RlpMaxVnVrDelta   = RLP_TYPE1_MAX_VN_VR_DELTA;

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
  Type for RLP reassembly buffer, used to resequence RLP frames received
  out-of-sequence over the air.
---------------------------------------------------------------------------*/
typedef struct
{
  rlp_seq_num_type    seq;                    /* RLP frame sequence number */
  dsm_item_type      *item_ptr;               /* Pointer to the RLP frame  */
} rlp_reassembly_item_type;

/*---------------------------------------------------------------------------
  RLP reassembly buffer size. Note that at run time, the maximum reassembly
  buffer size is effectively the lesser of RLP_REASSEMBLY_BUF_SIZE and the
  RlpMaxVnVrDelta.
---------------------------------------------------------------------------*/
#define RLP_REASSEMBLY_BUF_SIZE  256

/*---------------------------------------------------------------------------
  RLP reassembly buffer.
---------------------------------------------------------------------------*/
LOCAL rlp_reassembly_item_type  rlp_reassembly_buf[RLP_REASSEMBLY_BUF_SIZE];

/*---------------------------------------------------------------------------
  Type and table that contains Primary Channel Full Rate mask, TYPE, and
  Payload fields values. Table is indexed by Mux Option.
---------------------------------------------------------------------------*/
typedef struct
{
  byte type_mask;         /* Field that contains the TYPE field mask       */
  byte format_a;          /* TYPE field Format A value                     */
  byte format_b_new;      /* TYPE field Format B value, new data frames    */
  byte format_b_rexmit;   /* TYPE field Format B value, rexmit data frames */
  byte format_b_payload;  /* Payload size for Format B (max thruput) frames*/
} rlp_prifullrate_table_type;

LOCAL const rlp_prifullrate_table_type rlp_prifull_table[2] =
{{
  RLP1_PRI_FRATE_TYPE_MASK, RLP1_PRI_FULL_RATE_A, /* Rate Set 1 */
  RLP1_PRI_FULL_RATE_B_NEW, RLP1_PRI_FULL_RATE_B_REXMIT,
  RLP1_FULL_PRI_DATA_BYTES},
  {RLP2_PRI_FRATE_TYPE_MASK, RLP2_PRI_FULL_RATE_A, /* Rate Set 2 */
  RLP2_PRI_FULL_RATE_B_NEW, RLP2_PRI_FULL_RATE_B_REXMIT,
  RLP2_FULL_PRI_DATA_BYTES
}};

/*---------------------------------------------------------------------------
  Data structure containing the number of data bytes for each RLP Generic
  data frame, both rate sets, primary and secondary channel types.
  NOTE: 0 denotes the particular frame rate/channel type does NOT carry data.
---------------------------------------------------------------------------*/
LOCAL const byte rlp_gendata_maxlen_table[RLP_ERASURE][RLP_RATE_SET_CHN_TYPE_INDEX] = 
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
LOCAL dword rlp_rx_stat[RLP_FRAME_RATE_MAX][RLP_RATE_SET_CHN_TYPE_INDEX];
LOCAL dword rlp_tx_stat[RLP_FRAME_RATE_MAX][RLP_RATE_SET_CHN_TYPE_INDEX];
LOCAL dword rlp_rx_stat_tot[RLP_FRAME_RATE_MAX][RLP_RATE_SET_CHN_TYPE_INDEX];
LOCAL dword rlp_tx_stat_tot[RLP_FRAME_RATE_MAX][RLP_RATE_SET_CHN_TYPE_INDEX];

/*---------------------------------------------------------------------------
  This structure holds the error return values from the Validate Generic
  frame function. There are 3 different error values.
---------------------------------------------------------------------------*/
LOCAL dword rlp_gen_rx_errs[RLP_FRAME_RATE_MAX][RLP_FRAME_CHK_VLDN_MAX];

LOCAL rlp_pri_fr_err_type  rlp_pri_frate_errs;

LOCAL rlp_gen_stat_type    rlp_gen_stats;       /* various RLP statistics  */

/*---------------------------------------------------------------------------
  RLP state info structure declaration.
---------------------------------------------------------------------------*/
LOCAL rlp_state_info_type rlp_state_info;

/*---------------------------------------------------------------------------
  Definition for the number of RLP data frames to save after transmission for
  possible retransmission. The saved frame array should be large enough to
  account for 3 rounds of NAKs for RLP Type 1, or 2 rounds of NAKs for RLP
  Type 2. The maximum is 128 for RLP Type 1. NOTE: The saved frame array size
  must be a power of 2.
---------------------------------------------------------------------------*/
#define SAVED_FRAME_ARRAY_SIZ  64

/*---------------------------------------------------------------------------
  RLP saved frames array. FIFO to store the transmitted RLP data frames, in
  case they are requested for retransmission.
---------------------------------------------------------------------------*/
LOCAL rlp_frame_type rlp_saved_fr_array[SAVED_FRAME_ARRAY_SIZ];

/*---------------------------------------------------------------------------
  Buffer allocation for Re-xmit pointer array.
---------------------------------------------------------------------------*/
LOCAL rlp_frame_ptr_type rlp_rexmit_ptr_array[SAVED_FRAME_ARRAY_SIZ];

/*---------------------------------------------------------------------------
  Data item pointer for managing outbound (Um TX) payload data.
---------------------------------------------------------------------------*/
LOCAL dsm_item_type *rlp_tx_data_ptr = NULL;

/*---------------------------------------------------------------------------
  Function pointer and queue pointer variables to hold passed registration
  parameters.
---------------------------------------------------------------------------*/
LOCAL void               (*rlp_post_rx_func_ptr)(uint32) = NULL;
LOCAL dsm_watermark_type  *rlp_post_rx_wm_ptr            = NULL;
LOCAL dsm_watermark_type *rlp_tx_watermark_ptr         = NULL;
LOCAL uint32              rlp_post_rx_func_cb_data;

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
  Array of stats names, stored as character strings. The strings could be
  defined within the array of name pointers, but the compiler would put them
  in CONST.
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
LOCAL rlp_seq_num_type get_frame_seq(rlp_fr_type frame_type, rlp_gen_frame_type *frame_ptr);

/*---------------------------------------------------------------------------
  MDR RLP critical section variable.
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
  ((((arg1) - (arg2)) & (RlpSeqModulus/2)) == 0)


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
  (((arg1) != (arg2)) && ((((arg1) - (arg2)) & (RlpSeqModulus/2)) == 0))

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

FUNCTION QUEUE_RE_XMIT_FRAMES

DESCRIPTION
  This function will queue for retransmission the data frame(s) with the 
  specified sequence numbers. This function performs various checks, and
  aborts the retransmission if the specified range contains a sequence number
  >= V(S), or if a requested sequence number is not found in the saved frames
  array. For RLP Type 2, this function also sets the REXMIT bit or TYPE field
  in the queued re-tx frame, to indicate that this is a retransmitted frame.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void queue_re_xmit_frames
(
  word              first_seq_num,  /* First sequence number to retransmit */
  word              last_seq_num    /* Last sequence number to retransmit  */
)
{
  word              desired_index;    /* index of 1st sequence number      */
  word              frames_to_queue;  /* the number of xmit frames         */
  rlp_seq_num_type  min_seq_num;      /* Min seq num in saved frames array */
  rlp_frame_type   *frame_ptr;        /* To refer to the re-tx frame       */
  byte              payload_len;      /* To calculate payload length       */
  byte              type_mask;        /* TYPE field mask                   */
  byte              format_b_new;     /* TYPE field value for Format B New */
  byte              format_a;         /* TYPE field value for Format A     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DATA_RLP_MSG2(MSG_LEGACY_MED, "Preparing to re-xmit: %u - %u", first_seq_num, last_seq_num);
  /*-------------------------------------------------------------------------
    Return if no data frames are stored.
  -------------------------------------------------------------------------*/
  if( rlp_state_info.saved_frames_cnt == 0)
  {
    
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
    Check for the requested Re-xmit containing a sequence number less than
    the lowest sequence number stored in the saved frames array. If so, then
    reset RLP and exit.
    -------------------------------------------------------------------------*/
  min_seq_num = (rlp_state_info.next_seq_num - SAVED_FRAME_ARRAY_SIZ) % RlpSeqModulus;

  if( !GE_SEQ_CHECK(first_seq_num, min_seq_num) )
  {
    DATA_RLP_MSG2(MSG_LEGACY_ERROR, "Re-xmit requested is too old, "
                  "re-xmit/min = %u/%u",
                  first_seq_num, min_seq_num);
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
  if( rlp_saved_fr_array[desired_index].frame.gen_data.seq_num != (first_seq_num & 0x00FF) )
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
      ( last_seq_num - first_seq_num + RlpSeqModulus + 1) % RlpSeqModulus;
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
    frames_to_queue = rlp_state_info.saved_frames_cnt;
    rlp_gen_stats.largest_re_xmit = (byte)frames_to_queue;
    rlp_gen_stats.re_xmits_missed++;
  }
  /*-------------------------------------------------------------------------
    Loop thru queuing Re-xmits until all frames are queued or Rexmit array
    becomes full.
  -------------------------------------------------------------------------*/
  while(( frames_to_queue > 0) &&
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
    frame_ptr = &rlp_saved_fr_array[desired_index];
    rlp_state_info.re_xmit_in_index = (rlp_state_info.re_xmit_in_index + 1) %
                                      SAVED_FRAME_ARRAY_SIZ;
    rlp_state_info.re_xmit_frames_cnt++;

    /*-----------------------------------------------------------------------
      Leave critical section.
    -----------------------------------------------------------------------*/
    DS_LEAVE_CRIT_SECTION(&rlp_crit_sect);

    desired_index = ( desired_index + 1) % SAVED_FRAME_ARRAY_SIZ;

    /*-------------------------------------------------------------------------
      For RLP Type 2 only, set the REXMIT bit for a Format A frame, or the TYPE
      field value for a Format B frame, to indicate that this is a re-tx frame.
      Note that this will change the type of the stored frame in the saved
      frames array from new to re-tx.
    -------------------------------------------------------------------------*/
    if( rlp_type == RLP_TYPE_II )
    {
      payload_len = rlp_prifull_table[rlp_rate_set].format_b_payload;
      type_mask = rlp_prifull_table[rlp_rate_set].type_mask;
      format_b_new = rlp_prifull_table[rlp_rate_set].format_b_new;
      format_a = rlp_prifull_table[rlp_rate_set].format_a;

      if( rlp_traffic_type == RLP_TRAFFIC_TYPE_PRI )
      {
        if((frame_ptr->frame.pri_fr_data.data[payload_len] & type_mask) == 
                                                                format_b_new)
        {
          /*-----------------------------------------------------------------
            Format B frame. Set the TYPE field value to indicate re-tx frame.
          -----------------------------------------------------------------*/
          frame_ptr->frame.pri_fr_data.data[payload_len] = 
                             rlp_prifull_table[rlp_rate_set].format_b_rexmit;
        }
        else if( (frame_ptr->frame.pri_fr_data.data[payload_len] & type_mask) == format_a )
        {
          /*-----------------------------------------------------------------
            Format A frame. Set the REXMIT bit to '1'.
          -----------------------------------------------------------------*/
          frame_ptr->frame.gen_data.ctl_len |= 0x40;
        }
      }
    } /* if RLP Type 2 */
    frames_to_queue--;
  } /* while frames to queue */
  /*-------------------------------------------------------------------------
    Now check to see if all needed frames were queued. Log message and 
    re-xmit error indicator if not all re-xmits were queued.
  -------------------------------------------------------------------------*/
  if( frames_to_queue > 0 )
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR, "%u re-xmits not queued", frames_to_queue);
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
  byte                     ctl;
  byte                     nak_type;
  byte                     nak_map_cnt;
  byte                     frame_size = 0;
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

    case RLP_REXMIT_UPPR_BITS:
      /*=====================================================================
        RETRANSMITTED DATA frame indicated (for RLP Type 2 only). Check for
        proper length field.
      =====================================================================*/
      *frame_typ = RLP_REXMIT_DATA_FR;

      if( rlp_type == RLP_TYPE_I )
      {
         DATA_RLP_MSG0(MSG_LEGACY_ERROR,"Rexmit bit set for RLP Type 1 frame");
        *frame_typ = RLP_CONTROL_FR;
         return_val = RLP_CTL_LEN_ERR;
      }

      else if( rlp_type == RLP_TYPE_II )
      {
        if( (frame_ptr->gen_data.ctl_len & RLP_TYPE2_GEN_DATA_LEN_MASK) > max_data_len )
        {
          return_val = RLP_DATA_LEN_ERR;
        }
      }
      break;

    case RLP_CONTROL_UPPR_BITS:
     /*======================================================================
       CONTROL frame indicated. Perform checks to determine if the frame has
       a CRC error or an invalid CTL field.
     ======================================================================*/
      *frame_typ = RLP_CONTROL_FR;

      if( rlp_type == RLP_TYPE_I )
      {
        /*-------------------------------------------------------------------
          RLP Type 1 processing. Check the CRC and CTL field value.
        --------------------------------------------------------------------*/
        if( crc_16_l_calc((byte *)(&frame_ptr->gen_ctl.seq_num), 
                          RLP_TYPE1_CTL_FRAME_SIZE*8) != CRC_16_L_OK )
        {
          return_val = RLP_CRC_ERR;
        }
        else if( (frame_ptr->gen_ctl.ctl_len & RLP_GEN_CTL_LEN_MASK) != 0 )
        {
          return_val = RLP_CTL_LEN_ERR;
        }
        else
        {
          /*-----------------------------------------------------------------
            Valid RLP Type 1 Control frame rx'ed => Don't reset receive
            segmented frame processing.
          ------------------------------------------------------------------*/
          reset_seg_rx = FALSE;
   
          /*-----------------------------------------------------------------
            Determine the frame type. If SYNC, ACK or SYNC/ACK frame, set the
            frame type to Init frame. If NAK frame, set the frame type to
            Type 0 NAK frame.
          ------------------------------------------------------------------*/
          if( (frame_ptr->gen_ctl.ctl_len == RLP_TYPE1_SYNC) ||
              (frame_ptr->gen_ctl.ctl_len == RLP_TYPE1_ACK) ||
              (frame_ptr->gen_ctl.ctl_len == RLP_TYPE1_SYNC_ACK) )
          {
            *frame_typ = RLP_INIT_FR;
          }
          else if( frame_ptr->gen_ctl.ctl_len == RLP_TYPE1_NAK )
          {
            *frame_typ = RLP_NAK_TYPE0_FR;
          } 
          else
          {
            return_val = RLP_CTL_LEN_ERR;
            DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                          "RLP Type 1 frame rx'ed with unknown CTL value: %#X",
                          frame_ptr->gen_ctl.ctl_len);
          }
        }
      } /* if RLP Type 1 */

      else if( rlp_type == RLP_TYPE_II )
      {
        /*-------------------------------------------------------------------
          RLP Type 2 processing. First, get the CTL field bits and determine
          the frame type - Initialization frame or NAK frame.
        -------------------------------------------------------------------*/
        ctl = frame_ptr->type2_init.ctl_encr_mode & RLP_TYPE2_CTL_MASK;

        if( (ctl == RLP_TYPE2_SYNC) || (ctl == RLP_TYPE2_ACK) || 
            (ctl == RLP_TYPE2_SYNC_ACK) )
        {
          /*-----------------------------------------------------------------
            Initialization frame (fixed length). Set the frame length.
          ------------------------------------------------------------------*/
          *frame_typ = RLP_INIT_FR;
          frame_size = RLP_TYPE2_INIT_FRAME_SIZE;
        }
        else if( ctl == RLP_TYPE2_NAK )
        {
          /*-----------------------------------------------------------------
           NAK frame. Get the NAK_TYPE bits and determine the type of NAK -
           Type 0 NAK or Type 1 NAK.
          -----------------------------------------------------------------*/
          nak_type = 
                 frame_ptr->type2_nak.ctl_nak_type & RLP_TYPE2_NAK_TYPE_MASK;

          if( nak_type == RLP_TYPE2_NAK_TYPE0 )
          {
            /*---------------------------------------------------------------
              Type 0 NAK frame (fixed length). Set the frame length.
            ---------------------------------------------------------------*/
            *frame_typ = RLP_NAK_TYPE0_FR;
            frame_size = RLP_TYPE2_NAK0_FRAME_SIZE;
          }
          else if( nak_type == RLP_TYPE2_NAK_TYPE1 )
          {
            /*---------------------------------------------------------------
              Type 1 NAK frame. The frame length is variable, depending on
              the number of NAK Maps. First, get the NAK Map Count, and then
              use a lookup table to determine the frame length.
            ----------------------------------------------------------------*/
            *frame_typ = RLP_NAK_TYPE1_FR;
            nak_map_cnt = (frame_ptr->type2_nak.lseq_hi_map_cnt & RLP_TYPE2_NAK_MAP_CNT_MASK) >> 2;
            frame_size = rlp_type2_nak1_size_table[nak_map_cnt];
          }
          else
          {
            /*---------------------------------------------------------------
              Invalid NAK_TYPE value.
            ----------------------------------------------------------------*/
            return_val = RLP_CTL_LEN_ERR;
            DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                          "RLP Type 2 frame rx'ed with unknown NAK_TYPE: %#X",
                          nak_type);
          }
        }
        else
        {
          /*-----------------------------------------------------------------
            Invalid CTL field value.
          -----------------------------------------------------------------*/
          return_val = RLP_CTL_LEN_ERR;
          DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                        "RLP Type 2 frame rx'ed with unknown CTL value: %#X",
                        ctl);
        }

        if( return_val == RLP_FRAME_OK )
        {
          /*-----------------------------------------------------------------
            Valid RLP Type 2 Control frame rx'ed. Check the CRC.
          ------------------------------------------------------------------*/
          if( crc_16_l_calc((byte *)(&frame_ptr->type2_init.seq_num), 
                            frame_size*8) != CRC_16_L_OK )
          {
            return_val = RLP_CRC_ERR;
          }
          else
          {
            /*---------------------------------------------------------------
              Control frame CRC is OK => Don't reset receive segmented frame
              processing.
            ----------------------------------------------------------------*/
            reset_seg_rx = FALSE;
          }
        }
      } /* if RLP Type 2 */
      break;
      
    case RLP_SEGDATA_UPPR_BITS:
     /*======================================================================
        SEGMENTED DATA frame indicated. Further check ctl_len fields to
        determine if it is a fill frame in rateset2.
     ======================================================================*/
      if(( frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_CTL_MASK) ==
           RLP_SEGFR_FILL_FR_BITS) 
      {
         if( rlp_rate_set == RLP_RATE_SET_2 )
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
                 ( rlp_rx_segfr_active           == FALSE) )
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
/*lint -save -e641 */
LOCAL void process_ctl_frame
(
  rlp_gen_frame_type *frame_ptr,        /* Pointer to valid control frame  */
  rlp_fr_type         frame_type        /* Frame type of the rx'ed frame   */
)
{
  rlp_seq_num_type    first_seq;        /* First sequence number to re-tx  */
  rlp_seq_num_type    last_seq;         /* Last sequence number to re-tx   */
  byte               *nak_ptr;          /* To refer to the rx'ed NAK frame */
  byte                nak_map_count;    /* NAK Map Count                   */
  byte                i, j;             /* Loop indices                    */
  byte                nak_map_seq_fpos; /* NAK Map Seq field position      */
  byte                nak_map_fpos;     /* NAK Map field position          */
  byte                bitmap;           /* NAK bitmap                      */
  byte                mask;             /* Mask to get each bit in bitmap  */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*=========================================================================
    First, check for a NAK control frame. 
  =========================================================================*/
  if( (IS_NAK_FR(frame_type)) && (rlp_state_info.state != RLP_SYNC_STATE) )
  {
    /*-----------------------------------------------------------------------
      State is OK for processing NAK frames. Check and update RLP_DELAY if
      still in RT_MEAS state.
    -----------------------------------------------------------------------*/   
    if( rlp_state_info.state == RLP_RT_MEAS_STATE )
    {
      rlp_state_info.rlp_delay++;
    }

    if( rlp_type == RLP_TYPE_I )
    {
      /*---------------------------------------------------------------------
        RLP Type 1 processing. Queue the requested data frames for re-tx.
      ---------------------------------------------------------------------*/
      DATA_RLP_MSG2(MSG_LEGACY_MED, "Rx'ed NAK: First = %u, Last = %u", 
               frame_ptr->gen_ctl.first_seq, frame_ptr->gen_ctl.last_seq);
      queue_re_xmit_frames(frame_ptr->gen_ctl.first_seq, 
                           frame_ptr->gen_ctl.last_seq);
    }

    else if( rlp_type == RLP_TYPE_II )
    {
      /*---------------------------------------------------------------------
        RLP Type 2 processing. Process based on the type of NAK.
      ---------------------------------------------------------------------*/
      nak_ptr = (byte *)(&frame_ptr->type2_nak);

      if( frame_type == RLP_NAK_TYPE0_FR )
      {
        /*-------------------------------------------------------------------
          Type 0 NAK (First/Last NAK). Get the First and Last fields from the
          NAK frame, and then queue the requested data frames for re-tx.
        -------------------------------------------------------------------*/
        first_seq = b_unpackw(nak_ptr, 
                              RLP_TYPE2_NAK0_FIRST_FPOS, 
                              RLP_TYPE2_NAK0_FIRST_LEN);
        last_seq  = b_unpackw(nak_ptr, 
                              RLP_TYPE2_NAK0_LAST_FPOS, 
                              RLP_TYPE2_NAK0_LAST_LEN);
        DATA_RLP_MSG2(MSG_LEGACY_MED, "Rx'ed NAK: First = %u, Last = %u", first_seq, last_seq);

        queue_re_xmit_frames(first_seq, last_seq);
      }

      else if( frame_type == RLP_NAK_TYPE1_FR )
      {
        /*-------------------------------------------------------------------
          Type 1 NAK (Bitmap NAK). Determine how many NAK Maps are included
          in the NAK frame (i.e. the NAK Map count).
        -------------------------------------------------------------------*/
        nak_map_count = b_unpackb(nak_ptr, 
                                  RLP_TYPE2_NAK1_NAK_MAP_CNT_FPOS, 
                                  RLP_TYPE2_NAK1_NAK_MAP_CNT_LEN);

        /*-------------------------------------------------------------------
          For each NAK Map, get the sequence number of the first frame to be
          re-tx, and the bitmap indicating additional frames to be re-tx.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG0(MSG_LEGACY_MED, "Rx'ed Bitmap NAK:");
        for(i = 0; i <= nak_map_count; i++)
        {
          nak_map_seq_fpos = RLP_TYPE2_NAK1_NAK_MAP_FPOS + (RLP_TYPE2_NAK1_NAK_MAP_SEQ_LEN + RLP_TYPE2_NAK1_NAK_MAP_LEN) * i;
          nak_map_fpos = nak_map_seq_fpos + RLP_TYPE2_NAK1_NAK_MAP_SEQ_LEN;     
          first_seq = b_unpackw(nak_ptr, 
                                nak_map_seq_fpos, 
                                RLP_TYPE2_NAK1_NAK_MAP_SEQ_LEN);
          bitmap = b_unpackb(nak_ptr, 
                             nak_map_fpos, 
                             RLP_TYPE2_NAK1_NAK_MAP_LEN);
          DATA_RLP_MSG2(MSG_LEGACY_MED, " 1st seq = %u, Bitmap = 0x%x", first_seq, bitmap);

          /*-----------------------------------------------------------------
            Queue the first sequence number for re-tx.
          -------------------------------------------------------------------*/
          queue_re_xmit_frames(first_seq, first_seq);

          /*-----------------------------------------------------------------
            For each bit in the NAK bitmap that is set to '1', queue the 
            corresponding sequence number for re-tx. The MSB of the bitmap
            corresponds to [(first_seq+1) modulo 4096] etc.
          -------------------------------------------------------------------*/
          mask = 0x80;
          for(j = 1; j <= 8; j++)
          {
            if( bitmap & mask )
            {
              queue_re_xmit_frames((first_seq+j)%RlpSeqModulus, (first_seq+j)%RlpSeqModulus);
            }
            mask = mask >> 1;
          }
        } /* for i = 0 to nak_map_count */
      } /* if NAK Type 1 */
    } /* if RLP Type 2 */
  } /* if NAK frame */

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
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid SYNC, reset RLP");
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
/*lint -restore 641*/

/*===========================================================================

FUNCTION CHECK_VN_VR_DELTA

DESCRIPTION
  This function will check to determine if the RLP needed sequence number,
  V(N), and the expected sequence number, V(R), have reached their maximum
  delta. If so, then RLP will be reset.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the maximum delta is reached and an RLP reset occurred, else FALSE.

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

  if( GE_SEQ_CHECK(current_seq_num, (rlp_state_info.needed_seq_num + RlpMaxVnVrDelta)) )
  {
    DATA_RLP_MSG2(MSG_LEGACY_ERROR, "RLP Reset: Max V(N/R), %d/%d",
                  rlp_state_info.needed_seq_num, current_seq_num);
    rlp_gen_stats.resets++;
    rlp_reset();
    return(TRUE);
  }
  return(FALSE);
}

/*===========================================================================

FUNCTION PROCESS_SEQUENCE_NUM

DESCRIPTION
  This function is a subroutine for the RLP RX processing. This function
  checks the sequence numbers of all the valid Control, Idle and Data frames
  received during a 20ms slot, and creates NAK list entries for any missing
  frames detected. This function is also responsible for aging entries in the
  NAK list, and updating the needed/expected sequence numbers, if necessary.

DEPENDENCIES
  The variable rlp_new_or_idle_rcvd must be set appropriately, every 20ms,
  before calling this function.
  The rlp_rx_frame_list array must contain only the valid Control, Idle and
  Data frames rx'ed in this 20ms slot, sorted in ascending order of sequence
  number. The variable rlp_rx_frame_count must contain the number of frames
  in the rlp_rx_frame_list.

RETURN VALUE
  RLP_RX_DATA_AVAILABLE if frame(s) queued for dispatch (from reassembly), 
  else RLP_RX_NO_DATA_AVAILABLE

SIDE EFFECTS
  The naks_to_send_cnt will be updated to indicate the total number of data
  frames that need to be NAK'ed.
  The needed and expected sequence numbers may be updated as a result of
  the sequence number processing.
===========================================================================*/
/*lint -save -e578 suppress 578*/
LOCAL rlp_rx_status_enum_type process_sequence_num(void)
{
  rlp_rx_status_enum_type  sequence_chk;  /* Sequence # check return value */
  rlp_rx_status_enum_type  reassembly_chk;/* Reassembly Q check return val */
  rlp_nak_list_entry_type *nak_ptr;       /* For working thru NAK list     */
  boolean                  abort;         /* TRUE if abort timer expired   */
  word                     i, j;          /* For working thru rx frame list*/
  rlp_seq_num_type         temp_fr_seq;   /* For sorting rx frame list     */
  rlp_fr_type              temp_fr_type;  /* For sorting rx frame list     */
  rlp_seq_num_type         current_seq;   /* For creating NAK list entry   */
  rlp_nak_list_entry_type *temp_nak_ptr;  /* For saving a copy of nak_ptr  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  sequence_chk    = RLP_RX_NO_DATA_AVAILABLE;
  reassembly_chk  = RLP_RX_NO_DATA_AVAILABLE;
  naks_to_send_cnt = 0;

  /*-------------------------------------------------------------------------
    Check each entry in the NAK list, and increment the entry's age, if
    appropriate.
  -------------------------------------------------------------------------*/
  nak_ptr = (rlp_nak_list_entry_type *)q_check(&rlp_nak_q);

  while( nak_ptr != NULL )
  {
    abort = FALSE;

    if( nak_ptr->tx_done )
    {
      /*---------------------------------------------------------------------
        The NAK transmission for this entry is complete. Increment the age
        if a new data frame or idle frame was received during this 20ms slot,
        then check for retransmission/abort timer expiration.
      ---------------------------------------------------------------------*/
      if( rlp_new_or_idle_rcvd )
      {
        nak_ptr->age++;

        if( nak_ptr->age == (rlp_state_info.rlp_delay + RLP_RTT_MARGIN) )
        {
          /*-----------------------------------------------------------------
            This NAK list entry has aged long enough for the second round of
            NAKs. Set the entry's repeat count depending on the RLP type in
            use, and increment the total NAKs to send.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG1(MSG_LEGACY_MED, "Sending 2nd NAK for seq #%u", nak_ptr->seq);

          if( rlp_type == RLP_TYPE_I )
          {
            nak_ptr->repeat_cnt = 2;
          }
          else
          {
            nak_ptr->repeat_cnt = 3;
          }
          nak_ptr->tx_done = FALSE;
          naks_to_send_cnt++;

#ifdef FEATURE_DS_PSTATS
          rlp_gen_stats.double_naks++;
#endif
        } /* if age == (RTT + margin) */

        else if( nak_ptr->age == 2 * (rlp_state_info.rlp_delay + RLP_RTT_MARGIN) )
        {
          /*-----------------------------------------------------------------
            This NAK list entry has aged long enough for the third round of
            NAKs (for RLP Type 1), or abort (for RLP Type 2). For RLP Type 1,
            set the entry's repeat count and increment the total NAKs to send.
            For RLP Type 2, set a flag to indicate that the frame is to be
            aborted. The actual abort processing is done later.
          -----------------------------------------------------------------*/
          if( rlp_type == RLP_TYPE_I )
          {
            DATA_RLP_MSG1(MSG_LEGACY_MED, "Sending 3rd NAK for seq #%u", nak_ptr->seq);
            nak_ptr->repeat_cnt = 3;
            nak_ptr->tx_done = FALSE;
            naks_to_send_cnt++;
          }
          else
          {
            abort = TRUE;
          }

#ifdef FEATURE_DS_PSTATS
          rlp_gen_stats.triple_naks++;
#endif
        } /* if age == 2*(RTT + margin) */

        else if( (nak_ptr->age == 3 * (rlp_state_info.rlp_delay + RLP_RTT_MARGIN)) ||
                 (nak_ptr->age ==  RLP_MAX_HOLE_AGE) )
        {
          /*-----------------------------------------------------------------
            This NAK list entry has aged long enough to abort (for RLP Type 1
            only). Set a flag to indicate that the frame is to be aborted.
            The actual abort processing is done later.
          -----------------------------------------------------------------*/  
          if( rlp_type == RLP_TYPE_I )
          {
            abort = TRUE;
          }
          else
          {
            DATA_RLP_MSG2(MSG_LEGACY_ERROR,"NAK entry too old, seq/age: %u/%u",
                          nak_ptr->seq, nak_ptr->age);
          }
        } /* if age == 3*(RTT + margin) */
      } /* if new or idle rx'ed */
    } /* if tx_done */

    else  /* if !tx_done */
    {
      /*---------------------------------------------------------------------
        The NAK transmission for this entry is still in progress (either the
        NAK transmission has not yet started, or not all of the repeats have
        been completed). Just add it to the total NAKs to send count.
      ---------------------------------------------------------------------*/
      naks_to_send_cnt++;
    }

    /*-----------------------------------------------------------------------
      If the abort flag is set, perform the processing necessary to abort
      this missing frame.
    -----------------------------------------------------------------------*/
    if( abort )
    {
      if( nak_ptr->seq >= rlp_state_info.needed_seq_num )
      {
        /*-------------------------------------------------------------------
          This is a "real" abort i.e. time to give up on this missing frame.
          If the missing sequence number is equal to V(N), invoke a function
          to check the reassembly list, and dispatch/increment V(N) until
          V(N) is not at the head of the reassembly list.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG3(MSG_LEGACY_ERROR, "RLP frame aborted: %u, V(N/R): %u/%u",
                      nak_ptr->seq,
                      rlp_state_info.needed_seq_num, 
                      rlp_state_info.expected_seq_num);

        rlp_gen_stats.seq_timeout_cnt++;

        if( nak_ptr->seq == rlp_state_info.needed_seq_num )
        {
          rlp_state_info.needed_seq_num = 
                           (rlp_state_info.needed_seq_num+1) % RlpSeqModulus;
          sequence_chk = check_reassembly_q();
        }
      }
      else  /* seq < V(N) */
      {
        /*-------------------------------------------------------------------
          This abort is really a "natural death" i.e. the retransmission for
          the missing sequence number was actually rx'ed successfully, but
          the entry was not deleted from the NAK list.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG1(MSG_LEGACY_MED, "NAK entry for seq #%u died natural death", nak_ptr->seq);
      }

      /*---------------------------------------------------------------------
        First, get the next entry from the NAK list. Then delete the current
        NAK list entry, and return it to the free NAKs queue.
      ---------------------------------------------------------------------*/
      temp_nak_ptr = nak_ptr;
      nak_ptr = (rlp_nak_list_entry_type *)q_next(&rlp_nak_q, &nak_ptr->link);
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_delete(&rlp_nak_q,&temp_nak_ptr->link);
#else
      q_delete(&temp_nak_ptr->link);
#endif
      q_put(&rlp_free_naks_q, &temp_nak_ptr->link);

    } /* if abort */
    else
    {
      /*-----------------------------------------------------------------------
        Get the next entry from the NAK list, and loop back to continue with
        the NAK list processing.
      -----------------------------------------------------------------------*/
      nak_ptr = (rlp_nak_list_entry_type *)q_next(&rlp_nak_q, &nak_ptr->link);
    }

  } /* while more NAK list entries */

  /*-------------------------------------------------------------------------
    Check the reassembly queue for V(N). The function will dispatch from
    reassembly and increment V(N) until V(N) is not found or the reassembly
    queue is empty.
    -----------------------------------------------------------------------*/
  reassembly_chk = check_reassembly_q();

  /*-------------------------------------------------------------------------
    Now sort rx_frame_list in ascending order of sequence number. This is
    necessary for the subsequent sequence number processing. Sort the list
    using a simple bubble sort algorithm.
  -------------------------------------------------------------------------*/
  if (rlp_rx_frame_count >= 8)
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR, "rlp_rx_frame_count exceeds the rlp_rx_frame_list boundary %d",
                  rlp_rx_frame_count);
    return RLP_RX_NO_DATA_AVAILABLE;
  }
  for(i = 0; i <= rlp_rx_frame_count; i++)
  {
    for(j = (i + 1); j < rlp_rx_frame_count; j++)
    {
      if( GT_SEQ_CHECK(rlp_rx_frame_list[i].frame_seq, 
                       rlp_rx_frame_list[j].frame_seq) )
      {
        temp_fr_seq = rlp_rx_frame_list[i].frame_seq;
        temp_fr_type = rlp_rx_frame_list[i].frame_type;

        rlp_rx_frame_list[i].frame_seq = rlp_rx_frame_list[j].frame_seq;
        rlp_rx_frame_list[i].frame_type = rlp_rx_frame_list[j].frame_type;

        rlp_rx_frame_list[j].frame_seq = temp_fr_seq;
        rlp_rx_frame_list[j].frame_type = temp_fr_type;
      }
    } /* sort inner loop */
  } /* sort outer loop */

  /*-------------------------------------------------------------------------
    Now check if there are any missing frames. Compare the sequence number of
    each received frame in the rx_frame_list to V(R), and create NAK list
    entries for any missing frames. Advance V(R) after each such comparision.
    NOTE: It is assumed that rx_frame_list has been sorted in ascending order
    of sequence number.
  -------------------------------------------------------------------------*/
  for(i = 0; i < rlp_rx_frame_count; i++)
  {
    if( GT_SEQ_CHECK(rlp_rx_frame_list[i].frame_seq, 
                     rlp_state_info.expected_seq_num) )
    {
      /*---------------------------------------------------------------------
        Rx'ed frame sequence number > V(R) => one or more frames are missing.
        Create a NAK list entry for each sequence number from V(R) to the
        rx'ed frame sequence number-1. 
      ---------------------------------------------------------------------*/
      for(current_seq = rlp_state_info.expected_seq_num;
          GT_SEQ_CHECK(rlp_rx_frame_list[i].frame_seq, current_seq);
          current_seq = (current_seq + 1) % RlpSeqModulus)
      {
        if( (nak_ptr = (rlp_nak_list_entry_type *)q_get(&rlp_free_naks_q)) == NULL )
        {
          DATA_RLP_MSG2(MSG_LEGACY_ERROR, "No free NAKs V(R)/rx'ed: %u/%u",
                        rlp_state_info.expected_seq_num,
                        rlp_rx_frame_list[i].frame_seq);
        }
        else
        {
          /*-------------------------------------------------------------------
            Fill in the sequence number of the missing frame, set the age to
            zero, and set up the repeat count, based on the RLP type.
          -------------------------------------------------------------------*/
          nak_ptr->tx_done = FALSE;
          nak_ptr->seq = current_seq;
          nak_ptr->age = 0;

          if( rlp_type == RLP_TYPE_I )
          {
            nak_ptr->repeat_cnt = 1;
          }
          else
          {
            nak_ptr->repeat_cnt = 2;
          }

          /*-------------------------------------------------------------------
            Add the entry to the NAK list and increment the NAKs to send count.
          -------------------------------------------------------------------*/
          q_put(&rlp_nak_q, &nak_ptr->link);
          naks_to_send_cnt++;

#ifdef FEATURE_DS_PSTATS
          rlp_gen_stats.single_naks++;
#endif
          /*-------------------------------------------------------------------
            Set the 'Hole Active' flag, if not already set.
          -------------------------------------------------------------------*/
          if( !rlp_state_info.hole_active )
          {
            DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Hole in RLP seq#, V(R)/rx'ed: %u/%u", 
                      rlp_state_info.expected_seq_num,
                      rlp_rx_frame_list[i].frame_seq
                      );
            DATA_RLP_MSG3(MSG_LEGACY_HIGH,"Rx'ed Frame Rate/Type/Ch # = %u/%u/%u", 
                      rlp_rx_frame_list[i].frame_rate,
                      rlp_rx_frame_list[i].frame_type, 
                      rlp_rx_frame_list[i].ch_num);
            DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Rx'ed Frame Data: 0x%x 0x%x 0x%x", 
                      rlp_rx_frame_list[i].frame_data[0],
                      rlp_rx_frame_list[i].frame_data[1], 
                      rlp_rx_frame_list[i].frame_data[2]);

            rlp_state_info.hole_active = TRUE;
          }
        }
      } /* for current = expected to (frame_seq - 1) */

      /*---------------------------------------------------------------------
        Now set V(R), as appropriate.
      ---------------------------------------------------------------------*/
      if( rlp_rx_frame_list[i].frame_type == RLP_UNSEG_DATA_FR )
      {
        /*-------------------------------------------------------------------
          The rx'ed frame is a data frame. Set V(R) to the rx'ed frame
          sequence number + 1.
        -------------------------------------------------------------------*/
        rlp_state_info.expected_seq_num = 
                        (rlp_rx_frame_list[i].frame_seq + 1) % RlpSeqModulus;
      }
      else if( (rlp_rx_frame_list[i].frame_type == RLP_IDLE_FR) ||
               (IS_NAK_FR(rlp_rx_frame_list[i].frame_type)) )
      {
        /*-------------------------------------------------------------------
          The rx'ed frame is a Idle or NAK frame. Set V(R) equal to the rx'ed
          frame sequence number.
        -------------------------------------------------------------------*/
        rlp_state_info.expected_seq_num = rlp_rx_frame_list[i].frame_seq;
      }
    } /* if frame_seq > expected */

    else if( rlp_rx_frame_list[i].frame_seq == rlp_state_info.expected_seq_num )
    {
      /*---------------------------------------------------------------------
        Rx'ed frame sequence number = V(R). If the rx'ed frame is a data
        frame, increment V(R). V(R) is not incremented for Idle or Control
        frames. 
      ---------------------------------------------------------------------*/
      if( rlp_rx_frame_list[i].frame_type == RLP_UNSEG_DATA_FR )
      {
        rlp_state_info.expected_seq_num = 
                       (rlp_state_info.expected_seq_num + 1) % RlpSeqModulus;
      }
    } /* if frame_seq = expected */

  } /* for i = 0 to rlp_rx_frame_count */

  /*---------------------------------------------------------------------------
    Check to see if the last RX hole was patched i.e. if V(N) = V(R).
  ---------------------------------------------------------------------------*/
  if( (rlp_state_info.hole_active) &&
      (rlp_state_info.needed_seq_num == rlp_state_info.expected_seq_num) )
  {
    /*-------------------------------------------------------------------------
      Reset the Hole semaphore and remove any pending NAKs from the NAK list,
      since these NAKs are no longer needed.
    -------------------------------------------------------------------------*/
    DATA_RLP_MSG0(MSG_LEGACY_HIGH, "V(N) = V(R)");
    rlp_state_info.hole_active = FALSE;

    while( (nak_ptr = (rlp_nak_list_entry_type *)q_get(&rlp_nak_q)) != NULL)
    {
      q_put(&rlp_free_naks_q, &nak_ptr->link);
    }
    naks_to_send_cnt = 0;
  }

  /*---------------------------------------------------------------------------
    Set the return value based on whether any data frames were dispatched.
  ---------------------------------------------------------------------------*/
  if( (sequence_chk == RLP_RX_DATA_AVAILABLE) || (reassembly_chk == RLP_RX_DATA_AVAILABLE) )
  {
    return(RLP_RX_DATA_AVAILABLE);
  }
  else
  {
    return(RLP_RX_NO_DATA_AVAILABLE);
  }
} /* process_sequence_num */
/*lint -restore e578*/

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
  rlp_rx_status_enum_type  return_value;   /* Function return value        */
  word                     needed_index;   /* V(N) index in reassembly buf */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check the reassembly list for V(N). Increment V(N) and dispatch from
    reassembly until V(N) not found or queue empty. 
  -------------------------------------------------------------------------*/
  return_value = RLP_RX_NO_DATA_AVAILABLE;
  needed_index = rlp_state_info.needed_seq_num & (RLP_REASSEMBLY_BUF_SIZE - 1);

  while( (rlp_reassembly_buf[needed_index].item_ptr != NULL) )
  {
    if( (rlp_reassembly_buf[needed_index].seq != rlp_state_info.needed_seq_num) )
    {
      DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                    "Wrapped entry for V(N) = %u in reassembly buffer",
                    rlp_state_info.needed_seq_num);
    }
    else
    {
      return_value = RLP_RX_DATA_AVAILABLE;
      dispatch_frame(rlp_reassembly_buf[needed_index].item_ptr);
      rlp_reassembly_buf[needed_index].item_ptr = NULL;
      rlp_state_info.needed_seq_num = (rlp_state_info.needed_seq_num + 1) % RlpSeqModulus;
      needed_index = rlp_state_info.needed_seq_num & (RLP_REASSEMBLY_BUF_SIZE - 1);
    }
  }

  return(return_value);

} /* check_reassembly_q */

/*===========================================================================

FUNCTION PROCESS_DATA_FRAME

DESCRIPTION
  This function is a subroutine for the RLP RX processing. It processes an
  unsegmented RLP RX data frame. This includes the dispatching or reassembly
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
  dsm_item_type       *new_item_ptr,         /* Pointer to RLP Data frame  */
  rlp_seq_num_type    current_seq_num        /* RLP Data frame seq number  */
)
{
  rlp_rx_status_enum_type data_dispatch_chk; /* Data frame dispatched val  */
  rlp_rx_status_enum_type reassembly_chk;    /* Reassembly check return val*/
  word                    buf_index;         /* Reassembly buffer index    */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  data_dispatch_chk = RLP_RX_NO_DATA_AVAILABLE;
  reassembly_chk = RLP_RX_NO_DATA_AVAILABLE;

  if( current_seq_num == rlp_state_info.needed_seq_num )
  {
    /*-----------------------------------------------------------------------
      Current = V(N) i.e. the rx'ed frame is the needed sequence number.
      Dispatch the frame and increment V(N).
    -----------------------------------------------------------------------*/
    dispatch_frame(new_item_ptr);
    rlp_state_info.needed_seq_num = (rlp_state_info.needed_seq_num + 1) % RlpSeqModulus;
    data_dispatch_chk = RLP_RX_DATA_AVAILABLE;

  } /* if current = V(N) */

  else if( GT_SEQ_CHECK( current_seq_num, rlp_state_info.needed_seq_num))
  {
    /*-----------------------------------------------------------------------
      Current > V(N) i.e. this frame is subsequent to an active hole. Check
      if Current > V(R) i.e. this is a New frame, and set the New or Idle
      rx'ed flag. Add the rx'ed frame to the reassembly list.
    -----------------------------------------------------------------------*/
    if( GE_SEQ_CHECK(current_seq_num, rlp_state_info.expected_seq_num) )
    {
      rlp_new_or_idle_rcvd = TRUE;
    }
    /*-----------------------------------------------------------------------
      Use the sequence number to determine where in the reassembly list this
      frame belongs.
    -----------------------------------------------------------------------*/
    buf_index = current_seq_num & (RLP_REASSEMBLY_BUF_SIZE - 1);

    if( rlp_reassembly_buf[buf_index].item_ptr != NULL )
    {
      if( rlp_reassembly_buf[buf_index].seq == current_seq_num )
      {
        DATA_RLP_MSG3(MSG_LEGACY_MED, "Reassembly duplicate %u, V(N/R) = %u/%u", 
                 current_seq_num,
                 rlp_state_info.needed_seq_num,
                 rlp_state_info.expected_seq_num);
        (void)dsm_free_buffer(new_item_ptr);
      }
      else
      {
        DATA_RLP_MSG3(MSG_LEGACY_ERROR,
                      "%u exceeds Max V(N)<->V(R) delta, V(N/R) = %u/%u",
                      current_seq_num,
                      rlp_state_info.needed_seq_num,
                      rlp_state_info.expected_seq_num);
      }
    }
    else
    {
      rlp_reassembly_buf[buf_index].seq = current_seq_num;
      rlp_reassembly_buf[buf_index].item_ptr = new_item_ptr;
    }
  } /* if current > V(N) */

  else
  {
    /*------------------------------------------------------------------------
      Current < V(N). Discard the frame, since it is an old duplicate.
    ------------------------------------------------------------------------*/
    DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Old duplicate: %u, V(N/R): %u/%u",
                  current_seq_num,
                  rlp_state_info.needed_seq_num,
                  rlp_state_info.expected_seq_num);
    (void)dsm_free_buffer(new_item_ptr);
  } /* current < V(N) */

  /*--------------------------------------------------------------------------
    Now check the reassembly list for V(N). Dispatch and increment V(N) until
    V(N) is not found or the queue is empty, then return the proper indicator.
  --------------------------------------------------------------------------*/
  reassembly_chk = check_reassembly_q();
  if( (data_dispatch_chk == RLP_RX_DATA_AVAILABLE) || (reassembly_chk == RLP_RX_DATA_AVAILABLE) )
  {
    return(RLP_RX_DATA_AVAILABLE);
  }
  else
  {
    return(RLP_RX_NO_DATA_AVAILABLE);
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
  rlp_rx_status_enum_type return_value = RLP_RX_NO_DATA_AVAILABLE;
                                         /* function return value          */
  rlp_gen_frame_type      *frame_ptr;    /* points to RLP frame/rate       */
  byte                    *dest_ptr;     /* destination pointer for copies */
  uint8                   dest_buff_len = 0;
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
    if( ((frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) +
        rlp_rx_segfr_data_len) <= rlp_prifull_table[rlp_rate_set].format_b_payload )
    {
      /*---------------------------------------------------------------------
        Determine whether the Composite frame will be a generic frame or
        primary Full Rate frame. Set destination pointer and copy the
        payload data.
      ---------------------------------------------------------------------*/
    if( ((frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) +
         rlp_rx_segfr_data_len) < rlp_prifull_table[rlp_rate_set].format_b_payload )
       {
        dest_ptr = (byte *)frame_ptr->gen_data.data;
        frame_ptr->gen_data.ctl_len = rlp_rx_segfr_data_len +
                    (frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK);
        dest_buff_len = RLP2_FULL_GEN_DATA_BYTES - rlp_rx_segfr_data_len; 
        if(dest_buff_len>0)
        { 
          (void)memscpy( dest_ptr + rlp_rx_segfr_data_len,
                         dest_buff_len,
                         (void *)frame_ptr->gen_data.data,
                         (frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) );
          (void)memscpy( dest_ptr, RLP2_FULL_GEN_DATA_BYTES,
                         rlp_rx_segfr_data, rlp_rx_segfr_data_len );
          return_value = RLP_RX_DATA_AVAILABLE;
        }
      }
      else /* Composite frame will be Primary Full Rate, set Type and dest */
      {
        dest_ptr = (byte *)frame_ptr->pri_fr_data.data;
        frame_ptr->pri_fr_data.data[rlp_prifull_table[rlp_rate_set].
          format_b_payload]  = rlp_prifull_table[ rlp_rate_set].format_b_new;

        dest_buff_len = RLP2_FULL_PRI_DATA_BYTES - rlp_rx_segfr_data_len; 
        if(dest_buff_len>0 && dest_buff_len<=30)
        {
          (void)memscpy( dest_ptr + rlp_rx_segfr_data_len,
                         dest_buff_len,
                         (void *)frame_ptr->gen_data.data,
                         (frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) );
          (void)memscpy( dest_ptr, RLP2_FULL_PRI_DATA_BYTES,
      	                 rlp_rx_segfr_data, rlp_rx_segfr_data_len );
          return_value = RLP_RX_DATA_AVAILABLE;
        }
      }
      reset_rx_segfr();              /* Resets global rx_segfr variables */
    }
    else /* Invalid length, cancel Segmented RX, and exit */
    {
      reset_rx_segfr();       /* Resets global rx_segfr variables */
      return_value = RLP_RX_NO_DATA_AVAILABLE;
    }
  }
  else /* Not last segment */
  {
    /*-----------------------------------------------------------------------
      Save payload, update the segmented RX data length, free the frame 
      item and exit prior to sequence number and data frame processing.
    -----------------------------------------------------------------------*/

    dest_buff_len = RLP2_FULL_PRI_DATA_BYTES - rlp_rx_segfr_data_len; 
    if(dest_buff_len>0 && dest_buff_len<=30)
    {
      (void)memscpy( rlp_rx_segfr_data + rlp_rx_segfr_data_len,
                     dest_buff_len,
                     (void *)frame_ptr->gen_data.data,
                     (frame_ptr->gen_data.ctl_len & RLP_GEN_CTL_LEN_MASK) );
  
      rlp_rx_segfr_seq_num   = frame_ptr->gen_data.seq_num;
      rlp_rx_segfr_data_len += ( frame_ptr->gen_data.ctl_len & 
                                 RLP_GEN_CTL_LEN_MASK);
    }
    else
    {
      reset_rx_segfr();       /* Resets global rx_segfr variables */ 
    }
  
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
  word  i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize critical section.
  -------------------------------------------------------------------------*/
  DS_INIT_CRIT_SECTION( &rlp_crit_sect );

  /*-------------------------------------------------------------------------
    Init the reassembly buffer. Then reset all RLP state info and zero stats.
  -------------------------------------------------------------------------*/
  for(i = 0; i < RLP_REASSEMBLY_BUF_SIZE; i++)
  {
    rlp_reassembly_buf[i].item_ptr = NULL;
  }

  reset_state_info();
  
  (void)memset(rlp_rx_stat, 0, sizeof(rlp_rx_stat));
  (void)memset(rlp_tx_stat, 0, sizeof(rlp_tx_stat));
  (void)memset(rlp_rx_stat_tot, 0, sizeof(rlp_rx_stat_tot));
  (void)memset(rlp_tx_stat_tot, 0, sizeof(rlp_tx_stat_tot));
  (void)memset(&rlp_gen_stats, 0, sizeof(rlp_gen_stat_type));
  (void)memset(rlp_gen_rx_errs, 0, sizeof(rlp_gen_rx_errs));
  (void)memset(&rlp_pri_frate_errs, 0, sizeof(rlp_pri_frate_errs));

  /*-------------------------------------------------------------------------
    Initialize the free NAKs queue and the NAK queue. Initially, put all of
    the available buffers (i.e. NAK list entries) on the free NAKs queue.
  -------------------------------------------------------------------------*/
  (void) q_init(&rlp_free_naks_q);
  (void) q_init(&rlp_nak_q);

  for(i = 0; i < RLP_NAK_LIST_SIZE; i++)
  {
    q_put(&rlp_free_naks_q, q_link(&rlp_nak_buffer[i], &rlp_nak_buffer[i].link));
  }

#ifdef FEATURE_DS_PSTATS
  rlp_reset_stats();
#endif
} /* rlp_init */

/*===========================================================================

FUNCTION RLP_RESET

DESCRIPTION
  This function performs the work needed to reset the RLP state information 
  in preparation for the next RLP call. This function should be called upon
  completion of a data call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rlp_reset(void)
{
  rlp_nak_list_entry_type  *nak_ptr;          /* Pointer to NAK list entry */
  word                      i;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Enter critical section to protect global RLP state variables
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlp_crit_sect);

  /*-------------------------------------------------------------------------
    Reset RLP state info, then flush the reassembly list and the NAK list.
  -------------------------------------------------------------------------*/
  reset_state_info();

  dsm_free_packet( &rlp_tx_data_ptr );
  rlp_tx_data_ptr = NULL;

  for(i = 0; i < RLP_REASSEMBLY_BUF_SIZE; i++)
  {
    (void)dsm_free_buffer(rlp_reassembly_buf[i].item_ptr);
    rlp_reassembly_buf[i].item_ptr = NULL;
  }

  while( (nak_ptr = (rlp_nak_list_entry_type *)q_get(&rlp_nak_q)) != NULL )
  {
    q_put(&rlp_free_naks_q, &nak_ptr->link);
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
  data_frame_ptr->kind = DSM_PS_RLP;


  
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
  uint32             post_rx_func_cb_data
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
  else /* assign the watermark pointer */
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

FUNCTION GET_FRAME_SEQ

DESCRIPTION
  This function returns the sequence number of the supplied RLP frame. For
  RLP Type 1, the 8-bit sequence number, SEQ, is simply extracted from the
  frame. For RLP Type 2, the 12-bit sequence number, L_SEQ, is computed from
  the OTA 8-bit sequence number (SEQ), as follows:

  For New Data frames and Idle frames:
      L_SEQ = {L_V(R) + [256 + SEQ - V(R)] mod 256} mod 4096

  For NAK Control frames:
      L_SEQ = (L_SEQ_HI << 4) | SEQ

  For Retransmitted Data frames:
      L_SEQ is determined by searching the NAK list (in order from oldest to
      newest entry) to find the entry whose least significant 8 bits match
      the SEQ field of the received frame. L_SEQ is then set to the 12-bit
      sequence number of the matching NAK list entry.

DEPENDENCIES
  rlp_establish_srvc_type() should have been previously invoked to set up the
  traffic type, rate set, and rlp type in use.

RETURN VALUE
  The sequence number of the supplied RLP frame.

SIDE EFFECTS
  None
===========================================================================*/

LOCAL rlp_seq_num_type get_frame_seq
(
  rlp_fr_type               frame_type, /* The frame type of the RLP frame */
  rlp_gen_frame_type       *frame_ptr   /* Pointer to the RLP frame        */
)
{
  rlp_seq_num_type          frame_seq = 0xffff;  /* The RLP frame sequence number   */
  rlp_seq_num_type          L_VR;       /* L_V(R) - 12-bit expected seq num*/
  rlp_seq_num_type          VR;         /* V(R) - 8-bit expected seq num   */
  rlp_seq_num_type          seq;        /* 8-bit OTA sequence number       */
  rlp_seq_num_type          lseq_hi;    /* L_SEQ_HI for NAK frames         */
  rlp_nak_list_entry_type  *nak_ptr;    /* For searching the NAK list      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    For RLP Type 1, get the sequence number from the frame. If this is a
    retransmitted data frame i.e. sequence number < V(R), remove the NAK list
    entry for this frame.
  -------------------------------------------------------------------------*/
  if( rlp_type == RLP_TYPE_I )
  {
    frame_seq = frame_ptr->seq_num;

    if( (frame_type == RLP_UNSEG_DATA_FR) && GT_SEQ_CHECK(rlp_state_info.expected_seq_num, frame_seq) )
    {
      /*---------------------------------------------------------------------
        This is a retransmitted data frame. Find the matching NAK list entry
        and remove it.
      ---------------------------------------------------------------------*/
      nak_ptr = (rlp_nak_list_entry_type *)q_check(&rlp_nak_q);
      while( (nak_ptr != NULL) && (nak_ptr->seq != frame_seq) )
      {
        nak_ptr = (rlp_nak_list_entry_type *)q_next(&rlp_nak_q, &nak_ptr->link);
      }
      if( nak_ptr == NULL )
      {
        DATA_RLP_MSG1(MSG_LEGACY_MED, "No NAK list entry for re-tx frame: %u", frame_seq);
      }
      else
      {
#ifdef FEATURE_Q_NO_SELF_QPTR
        q_delete(&rlp_nak_q,&nak_ptr->link);
#else
        q_delete(&nak_ptr->link);
#endif
        q_put(&rlp_free_naks_q, &nak_ptr->link);
      }
    } /* if re-tx data frame */
  } /* RLP Type 1 */
  /*-------------------------------------------------------------------------
    For RLP Type 2, calculate L_SEQ based on the frame type. If any kind of
    error occurs, return a frame sequence number of 65535 (0xFFFF) which is
    outside the range of valid sequence numbers for RLP Type 2 (0 - 4095).
  -------------------------------------------------------------------------*/
  else if( rlp_type == RLP_TYPE_II )
  {
    frame_seq = 0xFFFF;

    /*-------------------------------------------------------------------------
      For Idle frames or New Data frames, get the 8-bit OTA sequence number
      from the frame, and then compute L_SEQ as specified in IS-707-A.
    -------------------------------------------------------------------------*/
    if( (frame_type == RLP_IDLE_FR) || (frame_type == RLP_UNSEG_DATA_FR) )
    {
      seq  = frame_ptr->seq_num;
      L_VR = rlp_state_info.expected_seq_num;
      VR   = rlp_state_info.expected_seq_num & 0x00FF;
      frame_seq = (L_VR + (RLP_TYPE1_SEQ_MODULUS + seq - VR) % RLP_TYPE1_SEQ_MODULUS) % RLP_TYPE2_SEQ_MODULUS;
    }
    /*-------------------------------------------------------------------------
      For NAK Control frames, compute L_SEQ by combining the SEQ and L_SEQ_HI
      fields from the frame.
    -------------------------------------------------------------------------*/
    else if( IS_NAK_FR(frame_type) )
    {
      seq = frame_ptr->type2_nak.seq_num;
      lseq_hi = frame_ptr->type2_nak.lseq_hi_map_cnt & 0x00F0;
      frame_seq = (lseq_hi << 4) | seq;
    }
    /*-------------------------------------------------------------------------
      For Retransmitted Data frames, search the NAK list for an entry with a
      matching SEQ value, and set L_SEQ to the sequence number of the matching
      NAK list entry, if found. Cleanup the NAK list entry, if necessary.
    -------------------------------------------------------------------------*/
    else if( frame_type == RLP_REXMIT_DATA_FR )
    {
      nak_ptr = (rlp_nak_list_entry_type *)q_check(&rlp_nak_q);
      while( (nak_ptr != NULL) && ((nak_ptr->seq & 0x00FF) != frame_ptr->seq_num) )
      {
        nak_ptr = (rlp_nak_list_entry_type *)q_next(&rlp_nak_q, &nak_ptr->link);
      }
      if( nak_ptr == NULL )
      {
        DATA_RLP_MSG1(MSG_LEGACY_MED, "No NAK list entry for re-tx frame: 0x%x", 
                frame_ptr->seq_num);
      }
      else
      {
        frame_seq = nak_ptr->seq;

       /*--------------------------------------------------------------------
         Remove the NAK list entry only if the entry's retransmission timer
         has not yet expired i.e. still in the first round of NAKs.
       --------------------------------------------------------------------*/
       if( nak_ptr->age < (rlp_state_info.rlp_delay + RLP_RTT_MARGIN) )
       {
#ifdef FEATURE_Q_NO_SELF_QPTR
          q_delete(&rlp_nak_q,&nak_ptr->link);
#else
          q_delete(&nak_ptr->link);
#endif
          q_put(&rlp_free_naks_q, &nak_ptr->link);
        }
        else
        {
          DATA_RLP_MSG1(MSG_LEGACY_MED, "Re-tx rx'ed for seq #%u after 1st timeout", nak_ptr->seq);
          nak_ptr->tx_done = TRUE;
        }
      }
    }
    else
    {
      DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Unknown RLP frame type, can't compute L_SEQ");
    }
  } /* RLP Type 2 */
  else
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Unknown RLP Type, can't get frame seq num");
  }

  return(frame_seq);

} /* get_frame_seq() */

/*===========================================================================

FUNCTION RLP_RX_PROCESS_FRAMES

DESCRIPTION
  This function processes the RLP frame(s) received over the air, as per the
  procedures specified in IS-707, for RLP Type 1 or IS-707-A, for RLP Type 2.
  The RLP receive processing includes: frame validity checking, control frame
  processing, sequence number checking, data frame processing, and data frame
  dispatching.

DEPENDENCIES
  rlp_init should have been previously invoked. rlp_establish_srvc_type()
  should have been previously invoked to set up the traffic type, rate set
  and rlp type in use.

  NOTE: This function only handles 'dsm_item_type' using the standard DMSS
        'q_link_type' linking (ie. no 'packet'ized items)

RETURN VALUE
  The status resulting from the RLP RX processing.

SIDE EFFECTS
  None
===========================================================================*/

rlp_rx_retval_enum_type rlp_rx_process_frames
(
  rlp_fwd_frame_block_type *fwd_frame_block
    /* Pointer to the forward frame block which contains all the RLP frames
       (fundamental + supplementals) received during this 20ms frame       */
)
{
  byte                      ch_num;          /* Loop index for channel #   */
  byte                      i, j;            /* Loop index to free buffers */
  dsm_item_type            *item_ptr;        /* Pointer to data item       */
  rlp_gen_frame_type       *frame_ptr;       /* Pointer to RLP frame       */
  rlp_rate_enum_type        frame_rate;      /* Rate of RLP frame          */
  rlp_seq_num_type          frame_seq;       /* RLP frame sequence number  */
  rlp_fr_type               frame_type;      /* RLP frame type             */
  rlp_rx_status_enum_type   frame_status;    /* RLP frame status           */
  boolean                   generic_frame;   /* Generic frame indicator    */
  static boolean            last2_idles_same = FALSE;
    /* To remember that the last frame was an out of range Idle            */
  boolean                   last2_idle_reset_flag = FALSE;
    /* Note, this flag is NOT static - this is used in conjunction with the
       boolean above to determine when to reset the last2_idle processing  */
  rlp_gen_fr_chk_enum_type  gen_frame_status;/* Status from validate gen fr*/
  rlp_rx_status_enum_type   seq_chk_retval;  /* Process seq num return val */
  rlp_rx_retval_enum_type   return_value;    /* Function return value      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*=========================================================================
    Check if the maximum time to achieve synchronization has been exceeded.
    Discard the received frame(s) and return, if so.
  =========================================================================*/  
  if( (rlp_state_info.state == RLP_SYNC_STATE) && (++rlp_state_info.rlp_delay == RLP_MAX_RTT_SETUP) )
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "SYNC state Timeout");
    /*-----------------------------------------------------------------------
      Free the buffers for all the rx'ed RLP frames, reset RLP and return.
    -----------------------------------------------------------------------*/
    for(i = 0; i <= fwd_frame_block->num_sup; i++)
    {
      (void)dsm_free_buffer(fwd_frame_block->fwd_frames[i].rlp_item_ptr);
    }
    rlp_gen_stats.resets++;
    rlp_reset();
    return_value = RLP_RX_SYNCH_TIMEOUT;
    return(return_value);
  }

  /*=========================================================================
    Loop to process the rx'ed RLP frames. For each rx'ed RLP frame, perform
    frame validity checks, and then process the frame, if valid, based on the
    frame type. If a frame error occurs, free the frame buffer and continue
    on to process the next frame. If any condition occurs that causes an RLP
    reset, free the buffers for all the rx'ed RLP frames and return.
  =========================================================================*/  
  rlp_rx_frame_count = 0;
  rlp_new_or_idle_rcvd = FALSE;
  return_value = RLP_RX_ERROR;

  for(ch_num = 0; ch_num < (fwd_frame_block->num_sup + 1); ch_num++)
  {
    /*=======================================================================
      Start by setting frame pointer, frame rate and default frame status.
      Increment the general RX total frame counts.
    =======================================================================*/
    item_ptr = fwd_frame_block->fwd_frames[ch_num].rlp_item_ptr;
    frame_ptr = (rlp_gen_frame_type *)fwd_frame_block->fwd_frames[ch_num].rlp_item_ptr->data_ptr;
    frame_rate = fwd_frame_block->fwd_frames[ch_num].frame_rate;
    frame_status = RLP_RX_NO_DATA_AVAILABLE;
    rlp_rx_stat[frame_rate][rlp_params_offset]++;
    rlp_rx_stat_tot[frame_rate][rlp_params_offset]++;

    /*=======================================================================
      Frame authenticity processing. Perform frame validity checking based on
      the frame rate.
    =======================================================================*/
    frame_type = RLP_UNSEG_DATA_FR;
    generic_frame = FALSE;

    switch(frame_rate)
    {
    case RLP_ERASURE:
      /*---------------------------------------------------------------------
        Erasure received. Save the frame rate for future use.
      ---------------------------------------------------------------------*/
      rlp_prev_pri_fr_rate = frame_rate;

      /*---------------------------------------------------------------------
        Increment the consecutive erasure count only if RLP is established.
      ---------------------------------------------------------------------*/
      if( rlp_state_info.state == RLP_ESTABLISHED_STATE ) 
      {
        rlp_state_info.contig_erase++;

#ifdef FEATURE_DS_PSTATS
        rlp_gen_stats.largest_contig_erase = MAX(rlp_gen_stats.largest_contig_erase,
                                                 rlp_state_info.contig_erase);
#endif
      }
      /*---------------------------------------------------------------------
        Increment the Round-Trip Time Estimate (rlp_delay) only ONCE per 20ms
        i.e. only while processing the fundamental frame (ch_num = 0). Also,
        set the return value to indicate Idle (so far) if an erasure is rx'ed
        on the fundamental channel.
      ---------------------------------------------------------------------*/
      if( ch_num == 0 )
      {
        return_value = RLP_RX_IDLE;

        if( rlp_state_info.state == RLP_RT_MEAS_STATE )
        {
          rlp_state_info.rlp_delay++;
        }
      }

      frame_status = RLP_RX_FRAME_ERR;
      break;

    case RLP_EIGHTH_RATE:
      /*---------------------------------------------------------------------
        1/8 rate (RLP Idle) frame. Ensure that the channel type is Primary
        and the frame passes the Nordstrom-Robinson FCS check, then perform
        all Frame Validity checks as defined in IS-99 section 3.7.1.2.3.1.
      ---------------------------------------------------------------------*/
      if( rlp_traffic_type == RLP_TRAFFIC_TYPE_PRI )
      {
        if( nr_table[ frame_ptr->pri_idle.seq_num] != frame_ptr->pri_idle.nr_crc )
        {
          /*-----------------------------------------------------------------
            The frame fails the NR CRC check. Break to avoid saving frame.
          -----------------------------------------------------------------*/
          if( rlp_state_info.state == RLP_ESTABLISHED_STATE )
          {
            DATA_RLP_MSG3(MSG_LEGACY_ERROR, "RLP Idle CRC error: 0x%x, 0x%x, 0x%x",
                          frame_ptr->pri_idle.seq_num,
                          frame_ptr->pri_idle.nr_crc,
                          nr_table[frame_ptr->pri_idle.seq_num]);
            rlp_pri_frate_errs.idle_fr_err++;
            rlp_state_info.contig_erase++;
#ifdef FEATURE_DS_PSTATS
            rlp_gen_stats.largest_contig_erase = MAX(rlp_gen_stats.largest_contig_erase, 
                                                     rlp_state_info.contig_erase);
#endif /* FEATURE_DS_PSTATS */
          }
          frame_status = RLP_RX_FRAME_ERR;
          break;
        }
        else if( (rlp_rate_set == RLP_RATE_SET_2) && 
                 ((frame_ptr->pri_idle.rs2_filler & RLP_RS2_IDLE_FILL_MASK) == RLP_RS2_FILL_LAST_BITS) )
        {
          /*-----------------------------------------------------------------
            Received an intersegment fill frame. These frames should only be 
            sent by Lucent Basestations. RLP just discards these frames. Make
            sure that this check for fill frame bits is done before the next
            validity check. Both checks look at the same bits.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Received an intersegment fill frame");
          frame_type = RLP_FILL_FR;
          rlp_gen_stats.fill_frames_rxed++;
          frame_status = RLP_RX_NO_DATA_AVAILABLE;
          break;
         /* Break from switch, so that the frame is not saved */
        }                      
        else if( (rlp_rate_set == RLP_RATE_SET_2) && 
                 ((frame_ptr->pri_idle.rs2_filler & RLP_RS2_IDLE_FILL_MASK) != RLP_RS2_IDLE_LAST_BITS) )
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
          frame_status = RLP_RX_FRAME_ERR;
          break;
        }
        else if ( (rlp_rx_segfr_active == TRUE) && (rlp_rate_set == RLP_RATE_SET_2) )
          /*-----------------------------------------------------------------
            If an idle frame in rateset2 is received while reassembly is in 
            progress, reassembly is reset. The actual reset is done outside
            the switch statement.
          -----------------------------------------------------------------*/
        {
          rlp_pri_frate_errs.idle_fr_err++;
          frame_status = RLP_RX_FRAME_ERR;
          break;
        }
        else if( (rlp_prev_pri_fr_rate == RLP_EIGHTH_RATE) &&
                 (frame_ptr->pri_idle.seq_num != rlp_saved_eighth.seq_num) )
        {
          /*-----------------------------------------------------------------
            If the previous frame was an Idle and the sequence numbers do not
            match then this frame is invalid. Note, according to sec. 3.1.3.1,
            this frame is not a erasure, so the consecutive erasure count is
            not incremented.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG2(MSG_LEGACY_ERROR,
                        "Idle no match previous, was/is: %u/%u",
                        rlp_saved_eighth.seq_num,
                        frame_ptr->pri_idle.seq_num);
          rlp_pri_frate_errs.idle_fr_err++;
          frame_status = RLP_RX_FRAME_ERR;
        }
        else   /* idle frame is good - so far */
        {
          rlp_new_or_idle_rcvd = TRUE;

          /*-----------------------------------------------------------------
            Get the sequence number of the Idle frame.
          -----------------------------------------------------------------*/
          frame_seq = get_frame_seq(RLP_IDLE_FR, frame_ptr);

          /*-----------------------------------------------------------------
            This error-free Idle frame matches the previous frame. Check for 
            sequence number within acceptable range per IS-99 frame validity
            checks. The check just below determines if the Idle is within the
            acceptable range (see IS-99 section 3.7.1.2.3.1 item 6).
          -----------------------------------------------------------------*/
          if( GE_SEQ_CHECK(frame_seq, rlp_state_info.expected_seq_num) &&
              GE_SEQ_CHECK(((rlp_state_info.expected_seq_num + rlp_state_info.contig_erase) % RlpSeqModulus), frame_seq) )
          {
            /*---------------------------------------------------------------
              The frame is within the acceptable range of sequence numbers.
              Set return value and frame type.
            ---------------------------------------------------------------*/
            frame_type = RLP_IDLE_FR;
            frame_status = RLP_RX_NO_DATA_AVAILABLE;
          }
          else if( last2_idles_same )
          {
            /*---------------------------------------------------------------
              This is the 3rd out-of-range in a row. Check the sequence
              number to determine if we re-sync or re-establish expected
              V(R).
            ---------------------------------------------------------------*/
            if( GT_SEQ_CHECK(frame_seq, rlp_state_info.expected_seq_num) )
            {
              /*-------------------------------------------------------------
                Sequence number > expected V(R). Re-establish sequence num.
              -------------------------------------------------------------*/
              DATA_RLP_MSG2(MSG_LEGACY_ERROR,
                            "3 Matching 1/8, now: %u, was: %u",
                            frame_seq, rlp_state_info.expected_seq_num);
              frame_type = RLP_IDLE_FR;
              frame_status = RLP_RX_NO_DATA_AVAILABLE;
            }
            else /* RLP must Re-sync */
            {
              /*-------------------------------------------------------------
                Sequence number < expected V(R). Update error statistics.
                Free the buffers for the remaining rx'ed RLP frames, reset
                RLP and return.
              -------------------------------------------------------------*/
              DATA_RLP_MSG0(MSG_LEGACY_ERROR, "RLP Idle Seq err, Re-Syncing");

              rlp_pri_frate_errs.idle_fr_err++;

              for(i = ch_num; i < (fwd_frame_block->num_sup + 1); i++)
              {
                (void)dsm_free_buffer(fwd_frame_block->fwd_frames[i].rlp_item_ptr);
              }
              rlp_gen_stats.resets++;
              rlp_reset();
              return_value = RLP_RX_ERROR;
              return(return_value);
            }
          }
          else /* Out of range frame matches previous but not 3 in a row   */
          {
            /*---------------------------------------------------------------
              Frame matches previous frame but out of range and not 3 in a 
              row. Update frame variables then break.
            ---------------------------------------------------------------*/
            rlp_pri_frate_errs.idle_fr_err++;
            last2_idles_same = TRUE;
            last2_idle_reset_flag = TRUE; 
            frame_status = RLP_RX_FRAME_ERR;
          }
        }   /* else if okay frame */
        /*-------------------------------------------------------------------
          Save frame rate and Idle frame bytes.
        -------------------------------------------------------------------*/
        rlp_saved_eighth = frame_ptr->pri_idle;
        rlp_prev_pri_fr_rate = frame_rate;  /* Save frame rate for further proce.*/
      }  /* if primary frame */
      
      else /* Invalid Secondary Frame */
      {
        /*-------------------------------------------------------------------
          Idle frames not allowed for Secondary Service option.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid Secondary RLP Frame RX'd");
        rlp_state_info.contig_erase++;
#ifdef FEATURE_DS_PSTATS
        rlp_gen_stats.largest_contig_erase = MAX(rlp_gen_stats.largest_contig_erase,
                                                 rlp_state_info.contig_erase);
#endif /* FEATURE_DS_PSTATS */
        frame_status = RLP_RX_FRAME_ERR;
      }
      break;

    case RLP_FULL_RATE:
      /*---------------------------------------------------------------------
        Full rate frame. Tally statistics then perform authenticity checking
        to determine frame format & validity.
      ---------------------------------------------------------------------*/
      if( (rlp_traffic_type == RLP_TRAFFIC_TYPE_PRI) &&
          ((frame_ptr->pri_fr_data.data[rlp_prifull_table[rlp_rate_set].format_b_payload] &
          rlp_prifull_table[rlp_rate_set].type_mask) == rlp_prifull_table[rlp_rate_set].format_b_new) )
      {
        /*-------------------------------------------------------------------
          Primary full rate format B new data frame. Set the frame type and
          then break to process the frame.
        -------------------------------------------------------------------*/
        frame_type = RLP_UNSEG_DATA_FR;
        rlp_prev_pri_fr_rate = frame_rate;
        break;
      }
      else if( (rlp_traffic_type == RLP_TRAFFIC_TYPE_PRI) &&
               (rlp_type == RLP_TYPE_II ) &&
               ((frame_ptr->pri_fr_data.data[rlp_prifull_table[rlp_rate_set].format_b_payload] &
               rlp_prifull_table[rlp_rate_set].type_mask) == rlp_prifull_table[rlp_rate_set].format_b_rexmit) )
      {
        /*-------------------------------------------------------------------
          Primary full rate format B retransmitted data frame. Set the frame
          type and then break to process the frame.
        -------------------------------------------------------------------*/
        frame_type = RLP_REXMIT_DATA_FR;
        rlp_prev_pri_fr_rate = frame_rate;
        break;
      }
      else if( ((frame_ptr->pri_fr_data.data[rlp_prifull_table[rlp_rate_set].format_b_payload]) &
               rlp_prifull_table[rlp_rate_set].type_mask) != rlp_prifull_table[rlp_rate_set].format_a )
      {
        /*-------------------------------------------------------------------
          Frame doesn't conform to either of the Full Rate formats. Log error
          and break. Note, we do not increment conseq. erasure count. This is
          specified in the frame validity checks section of IS-707.
        -------------------------------------------------------------------*/
        DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Full rate type error: 0x%x, ch: %d",
                  frame_ptr->pri_fr_data.data[rlp_prifull_table[rlp_rate_set].format_b_payload],
                  ch_num
                  );
        rlp_pri_frate_errs.full_type_err++;
        rlp_prev_pri_fr_rate = RLP_ERASURE; 
        generic_frame = FALSE;
        frame_status = RLP_RX_FRAME_ERR;
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
        ???? received. Log frame statistics then determine frame validity and
        frame type.
      ---------------------------------------------------------------------*/

      if ( rlp_gendata_maxlen_table[frame_rate][rlp_params_offset] == 0 )
      {
        /*-------------------------------------------------------------------
          Double checks Mux Layer - if the table entry is zero, then this is
          not a valid Rate Set & Frame Rate combination.
        -------------------------------------------------------------------*/
        if ( (frame_rate == RLP_QUARTER_RATE) && (rlp_rate_set == RLP_RATE_SET_1) )
        {
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Rate 1/4 frame rx'ed for Rate Set 1");
        }
        else
        {
          /*-----------------------------------------------------------------
            Write error to NV only if pathological mux layer error and not
            for Rate Set 1 quarter rate.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                        "Pathological erasure received from Mux Layer");
        }
        rlp_prev_pri_fr_rate = RLP_ERASURE; 
        frame_status = RLP_RX_FRAME_ERR;
        break;
      }

      /*---------------------------------------------------------------------
        Call a function which performs further validity checks on the generic
        frame and determines the frame type.
      ---------------------------------------------------------------------*/
      gen_frame_status = validate_gen_frame(frame_ptr, 
                                            &frame_type, 
                                            rlp_gendata_maxlen_table[frame_rate][rlp_params_offset]);

      if( gen_frame_status != RLP_FRAME_OK )
      {
        /*-------------------------------------------------------------------
          Update error statistics and set return value. Contig. erasure count
          is only inremented when there is a CRC error. This is specified in
          the frame validity checks section in IS707.
        -------------------------------------------------------------------*/
        if ( gen_frame_status == RLP_CRC_ERR )
        {
          DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Control frame with bad CRC rxed");

          rlp_state_info.contig_erase++;

#ifdef FEATURE_DS_PSTATS
          rlp_gen_stats.largest_contig_erase = MAX(rlp_gen_stats.largest_contig_erase,
                                                   rlp_state_info.contig_erase);
#endif /* FEATURE_DS_PSTATS */
        }
        else
        {
          DATA_RLP_MSG3(MSG_LEGACY_ERROR, "Frame Err: %u, Rate: %u, Chan: %u",
                        gen_frame_status, frame_rate, rlp_rate_set);
        }

        rlp_gen_rx_errs[frame_rate][gen_frame_status]++;
        rlp_prev_pri_fr_rate = RLP_ERASURE; 
        frame_status = RLP_RX_FRAME_ERR;
      }
      else /* Frame is Good. Set generic frame indicator. */
      {
        rlp_prev_pri_fr_rate = frame_rate;
        generic_frame = TRUE;
      }
      break;

    case RLP_BLANK:
      /*---------------------------------------------------------------------
        Blank frame. Set the frame status to error, so that the dsm buffer is
        freed below.
      ---------------------------------------------------------------------*/
      frame_status = RLP_RX_FRAME_ERR;
      break;
  
    default:
      /*---------------------------------------------------------------------
        Invalid frame rate. Pathological software error. Cannot happen.
      ---------------------------------------------------------------------*/
      DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Unknown RLP data frame rate: %u",
                    frame_rate);

    } /* switch */

    if ( last2_idle_reset_flag == FALSE )
    {
      /*---------------------------------------------------------------------
        Reset last2_idles_same variable in all situations, except when RLP
        received 2 idles with out of range sequence numbers.
      ---------------------------------------------------------------------*/
      last2_idles_same = FALSE;
    }
  
    /*-----------------------------------------------------------------------
      Check the return value for error indication. If any error do not
      proceed to the following sequence number and data frame processing.
    -----------------------------------------------------------------------*/
    if( frame_status == RLP_RX_FRAME_ERR )
    {
      /*---------------------------------------------------------------------
        Error occurred. If maximum contiguous erasures received, reset RLP.
      ---------------------------------------------------------------------*/
      if( rlp_state_info.state == RLP_ESTABLISHED_STATE ) 
      {
        if ( rlp_state_info.contig_erase >= RlpMaxContigErase )
        {
          DATA_RLP_MSG1(MSG_LEGACY_ERROR, "RLP Reset, Max Erasures: %d",
                        rlp_state_info.contig_erase);

          for(i = ch_num; i < (fwd_frame_block->num_sup + 1); i++)
          {
            (void)dsm_free_buffer(fwd_frame_block->fwd_frames[i].rlp_item_ptr);
          }
          rlp_gen_stats.resets++;
          rlp_reset();
          return_value = RLP_RX_ERROR;
          return(return_value);
        }
        //rlp_tx_segfr_active = FALSE;  
        reset_rx_segfr();
      }
      (void)dsm_free_buffer(item_ptr);
      continue;
    }

    /*=========================================================================
      To get here a valid frame was received. Perform RLP state checks in order
      to determine if the subsequent sequence number and data frame processing
      should be performed.
    =========================================================================*/
    rlp_state_info.contig_erase = 0;

    if ( frame_type == RLP_FILL_FR )  /* This was a intersegment fill frame  */
    {
      /*-----------------------------------------------------------------------
        No processing required for fill frames - we don't even check sequence
        numbers or increment NAK counters. Note that RLP receive processing
        frees all non-data RLP frame buffers.
      -----------------------------------------------------------------------*/
      (void)dsm_free_buffer(item_ptr);
      if( return_value != RLP_RX_NON_IDLE )
      {
        return_value = RLP_RX_IDLE;
      }
      continue;
    }

    /*-------------------------------------------------------------------------
      Check for a control frame. No further processing if RLP is in SYNC state
      or a control frame in RT_MEAS state.
    -------------------------------------------------------------------------*/
    if( IS_CTL_FR(frame_type) )
    {
      process_ctl_frame(frame_ptr, frame_type);
      return_value = RLP_RX_NON_IDLE;
    }
    if( (rlp_state_info.state == RLP_SYNC_STATE) || 
        ((rlp_state_info.state == RLP_RT_MEAS_STATE) && (IS_CTL_FR(frame_type))) )
    {
      (void)dsm_free_buffer(item_ptr);
      continue;
    }

    /*-------------------------------------------------------------------------
      Check for the 1st non-control frame since RLP reset. Reset and exit if
      previous frame was an erasure otherwise set state to Established. NOTE
      this is not required in IS-99.
    -------------------------------------------------------------------------*/
    if( rlp_state_info.state == RLP_RT_MEAS_STATE )
    {
      if( rlp_prev_pri_fr_rate == RLP_ERASURE )
      {
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Bad 1st Non-ctl frame, Re-sync");

        for(i = ch_num; i < (fwd_frame_block->num_sup + 1); i++)
        {
          (void)dsm_free_buffer(fwd_frame_block->fwd_frames[i].rlp_item_ptr);
        }
        rlp_gen_stats.resets++;
        rlp_reset();
        return_value = RLP_RX_ERROR;
        return(return_value);
      }
      /*-----------------------------------------------------------------------
        Set the state to ESTABLISHED and set the channel type and channel type
        offset (for accessing parameter arrays).
      -----------------------------------------------------------------------*/
      rlp_state_info.state = RLP_ESTABLISHED_STATE;

      DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP established, RLP_DELAY = %d", rlp_state_info.rlp_delay);
    }

    /*-------------------------------------------------------------------------
      Check for active segmented RX processing. If active then invoke function
      to process/consolidate segmented frames. Only proceed to sequence and
      data frame processing if last segment of segmented data frame.
    -------------------------------------------------------------------------*/
    if( rlp_rx_segfr_active && frame_type == RLP_SEG_DATA_FR )
    {
      DATA_RLP_MSG0(MSG_LEGACY_MED, "Processing Segmented Data Frame");
      frame_status = process_seg_data_frame(item_ptr);                    
      if( frame_status != RLP_RX_DATA_AVAILABLE )
      {
        (void)dsm_free_buffer(item_ptr);   /* not last segment */
        return_value = RLP_RX_NON_IDLE;
        continue;
      }

      /*-------------------------------------------------------------------------
        At this point the RLP data frame has been completely reassembled from the
        rx'ed segments. Set the frame type and the generic frame indicator based
        on the frame format of the reassembled RLP data frame.
      -------------------------------------------------------------------------*/
      if( rlp_type == RLP_TYPE_I )
      {
        frame_type = RLP_UNSEG_DATA_FR;

        if( (frame_ptr->pri_fr_data.data[rlp_prifull_table[rlp_rate_set].format_b_payload] &
             rlp_prifull_table[rlp_rate_set].type_mask) == rlp_prifull_table[rlp_rate_set].format_b_new)
        {
          generic_frame = FALSE;
        }
      }
      else if( rlp_type == RLP_TYPE_II )
      {
        frame_type = RLP_REXMIT_DATA_FR;

        if( (frame_ptr->pri_fr_data.data[rlp_prifull_table[rlp_rate_set].format_b_payload] &
             rlp_prifull_table[rlp_rate_set].type_mask) == rlp_prifull_table[rlp_rate_set].format_b_rexmit)
        {
          generic_frame = FALSE;
        }
      }
    }

    /*=========================================================================
      To get here, we have a valid Control, Idle or Data frame. Get the frame
      sequence number (SEQ or L_SEQ), and add it to the rx_frame_list for the
      subsequent sequence number processing.
    =========================================================================*/
    frame_seq = get_frame_seq(frame_type, frame_ptr);
    DATA_RLP_MSG3(MSG_LEGACY_LOW, "L_V(R), L_V(N), L_SEQ = %d, %d, %d", rlp_state_info.expected_seq_num, rlp_state_info.needed_seq_num, frame_seq);

    /*-------------------------------------------------------------------------
      Check if any error occurred while computing the sequence number. If so,
      discard the frame.
    -------------------------------------------------------------------------*/
    if( frame_seq == 0xFFFF )
    {
      DATA_RLP_MSG0(MSG_LEGACY_MED, "Error in computing L_SEQ");
      (void)dsm_free_buffer(item_ptr);
      continue;
    }

    /*-------------------------------------------------------------------------
      Check to make sure that this frame does not take the V(N)<->V(R) delta
      over the max limit (the function will reset RLP and return TRUE, if so).
      If the max delta is exceeded, free the buffers for the remaining rx'ed
      RLP frames and return.
    -------------------------------------------------------------------------*/
    if( GE_SEQ_CHECK(frame_seq, rlp_state_info.expected_seq_num) )
    {
      if( check_vn_vr_delta(frame_seq) )
      {
        for(i = ch_num; i < (fwd_frame_block->num_sup + 1); i++)
        {
          (void)dsm_free_buffer(fwd_frame_block->fwd_frames[i].rlp_item_ptr);
        }
        return_value = RLP_RX_ERROR;
        return(return_value);
      }
    }

    /*-------------------------------------------------------------------------
      Set the frame type for a Zero-Length Data frame to IDLE to ensure proper
      sequence number processing.
    -------------------------------------------------------------------------*/
    if( generic_frame )
    {
      if( (rlp_type == RLP_TYPE_I) &&
          ((frame_ptr->gen_data.ctl_len & RLP_TYPE1_GEN_DATA_LEN_MASK) == RLP_LEN_0) )
      {
        frame_type = RLP_IDLE_FR;
      }
      else if( (rlp_type == RLP_TYPE_II) &&
               ((frame_ptr->gen_data.ctl_len & RLP_TYPE2_GEN_DATA_LEN_MASK) == RLP_LEN_0) )
      {
        frame_type = RLP_IDLE_FR;
      }
    }

    /*-------------------------------------------------------------------------
      Check for Idle frame and set the return value to indicate idle, only if
      none of the previous rx'ed frames were non-idle.
    -------------------------------------------------------------------------*/
    if( (frame_type == RLP_IDLE_FR) && (return_value != RLP_RX_NON_IDLE) )
    {
      return_value = RLP_RX_IDLE;
    }

    /*-------------------------------------------------------------------------
      Now store the frame type, sequence number and channel number of this
      frame in the rx_frame_list and increment the rx_frame_count. This list is
      used later for sequence number processing.
    -------------------------------------------------------------------------*/
    rlp_rx_frame_list[rlp_rx_frame_count].frame_rate = frame_rate;
    rlp_rx_frame_list[rlp_rx_frame_count].frame_type = frame_type;
    rlp_rx_frame_list[rlp_rx_frame_count].frame_seq = frame_seq;
    rlp_rx_frame_list[rlp_rx_frame_count].ch_num = ch_num;
    for( j = 0; j < 3; j++)
    {
      rlp_rx_frame_list[rlp_rx_frame_count].frame_data[j] = ((byte *)frame_ptr)[j];
    };

    rlp_rx_frame_count++;

    /*-------------------------------------------------------------------------
      No further processing for Idle frames, Control frames or Zero-Length
      Data frames.
    -------------------------------------------------------------------------*/
    if( (frame_type == RLP_IDLE_FR) || (IS_CTL_FR(frame_type)) )
    {
      (void)dsm_free_buffer(item_ptr);
      continue;
    }
    
    /*=========================================================================
      To get here, we have a valid Data frame. Set the payload data count, then
      perform data frame processing for the rx'ed frame. Invoke the post-RX
      function if there is data available for the upper layer.
    =========================================================================*/
    return_value = RLP_RX_NON_IDLE;


  /*-------------------------------------------------------------------------
    Set data_ptr, used and size.  Then process this frame.
  -------------------------------------------------------------------------*/
    if( generic_frame == TRUE)
    {
      if( rlp_type == RLP_TYPE_I )
      {
        item_ptr->used = frame_ptr->gen_data.ctl_len & 
          RLP_TYPE1_GEN_DATA_LEN_MASK;
      }
      else if( rlp_type == RLP_TYPE_II )
      {
        item_ptr->used = frame_ptr->gen_data.ctl_len & 
          RLP_TYPE2_GEN_DATA_LEN_MASK;
      }
      item_ptr->data_ptr += FPOS(rlp_gen_frame_type, gen_data.data);
      item_ptr->size -= FPOS(rlp_gen_frame_type, gen_data.data);
    }
    else /* Primary Full rate frame, use Mux option to set data payload size */
    {
      if( rlp_rate_set == RLP_RATE_SET_1 )
      {
        item_ptr->used = RLP1_FULL_PRI_DATA_BYTES;
      }
      else
      {
        item_ptr->used = RLP2_FULL_PRI_DATA_BYTES;
      }
      item_ptr->data_ptr += FPOS(rlp_gen_frame_type, pri_fr_data.data);
      item_ptr->size -= FPOS(rlp_gen_frame_type, pri_fr_data.data);
    }

    frame_status = process_data_frame(item_ptr, frame_seq);

    if( frame_status == RLP_RX_DATA_AVAILABLE )
    {
      (rlp_post_rx_func_ptr)(rlp_post_rx_func_cb_data);
      /*-----------------------------------------------------------------------
        Note that we DO NOT free the frame buffer for data frames, because the
        frame pointer is passed to upper layers. 
      -----------------------------------------------------------------------*/
    }
  } /* for ch_num = 0 to (num_sup+1) */

  /*===========================================================================
    All of the rx'ed RLP frames have been processed. Now do NAK list processing
    i.e. aging of NAK timers, and sequence number checking for each frame in
    the rx_frame_list. Note that a return value indicating data available will
    be due to dispatching from the reassembly list. Invoke the post-RX function
    if there is data available for the upper layer.
  ===========================================================================*/
  seq_chk_retval = process_sequence_num();

  if( seq_chk_retval == RLP_RX_DATA_AVAILABLE )
  {
    DATA_RLP_MSG0(MSG_LEGACY_MED, "Dispatch frame from Re-assembly");
    (rlp_post_rx_func_ptr)(rlp_post_rx_func_cb_data);
  }

  return(return_value);

} /* rlp_rx_process_frame */

/*=============================================================================

FUNCTION BUILD_TYPE0_NAK

DESCRIPTION
  This function builds a Type 0 (i.e. First/Last type) NAK frame using the
  supplied parameters. Note that the memory for storing the built NAK frame
  should have been allocated by the calling function.

DEPENDENCIES
  rlp_establish_srvc_type() should have been previously invoked to establish
  the type of RLP in use.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
LOCAL void build_type0_nak
(
  rlp_frame_type    *nak_ptr,              /* Pointer to the NAK frame       */
  rlp_seq_num_type   current_seq,          /* Current sequence number - V(S) */
  rlp_seq_num_type   first_seq,	           /* First sequence number to NAK   */
  rlp_seq_num_type   last_seq	           /* Last sequence number to NAK    */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------------
    Build a RLP Type 1 First/Last NAK frame.
  ---------------------------------------------------------------------------*/
  if( rlp_type == RLP_TYPE_I )
  {
    /*-------------------------------------------------------------------------
      Fill in the SEQ, CTL, FIRST and LAST fields.
    -------------------------------------------------------------------------*/
    nak_ptr->frame.gen_ctl.seq_num = (byte)(current_seq);
    nak_ptr->frame.gen_ctl.ctl_len = RLP_TYPE1_NAK;
    nak_ptr->frame.gen_ctl.first_seq = (byte)(first_seq);
    nak_ptr->frame.gen_ctl.last_seq = (byte)(last_seq);
    /*-------------------------------------------------------------------------
      Compute the CRC and append to the NAK frame.
    -------------------------------------------------------------------------*/
    crc_gen((byte *)(&nak_ptr->frame), RLP_TYPE1_CTL_FRAME_SIZE);
    /*-------------------------------------------------------------------------
      Set the unused bytes in the frame to 0's.
    -------------------------------------------------------------------------*/
    memset((void *)nak_ptr->frame.gen_ctl.unused, 0, RLP2_FULL_PRI_CTL_UNUSED_BYTES);
  }

  /*---------------------------------------------------------------------------
    Build a RLP Type 2 First/Last NAK frame.
  ---------------------------------------------------------------------------*/
  else if( rlp_type == RLP_TYPE_II )
  {
    /*-------------------------------------------------------------------------
      Fill in the SEQ, CTL, NAK_TYPE, L_SEQ_HI, FIRST and LAST fields.
    -------------------------------------------------------------------------*/
    nak_ptr->frame.type2_nak.seq_num = (byte)(current_seq & 0x00FF);
    nak_ptr->frame.type2_nak.ctl_nak_type = (byte)(RLP_TYPE2_NAK | RLP_TYPE2_NAK_TYPE0);
    nak_ptr->frame.type2_nak.lseq_hi_map_cnt = (byte)((current_seq & 0x0F00) >> 4) | ((first_seq & 0x0F00) >> 8);
    nak_ptr->frame.type2_nak.nak_body[0] = (byte)(first_seq & 0x00FF);
    nak_ptr->frame.type2_nak.nak_body[1] = (byte)((last_seq & 0x0FF0) >> 4);
    nak_ptr->frame.type2_nak.nak_body[2] = (byte)(((last_seq & 0x000F) << 4) & 0xF0);
    /*-------------------------------------------------------------------------
      Compute the CRC and append to the NAK frame.
    -------------------------------------------------------------------------*/
    crc_gen((byte *)(&nak_ptr->frame.type2_nak), RLP_TYPE2_NAK0_FRAME_SIZE);
    /*-------------------------------------------------------------------------
      Set the unused bytes in the frame to 0's.
    -------------------------------------------------------------------------*/
    memset((void *)&nak_ptr->frame.type2_nak.nak_body[5], 0, (RLP_TYPE2_NAK_MAX_LEN - 5));
    memset((void *)nak_ptr->frame.type2_nak.unused, 0, RLP_TYPE2_NAK_UNUSED_BYTES);
  }
  else
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                  "Unknown RLP Type: %d, can't build Type 0 NAK",
                  rlp_type);
  }
} /* build_type0_nak() */

/*=============================================================================

FUNCTION BUILD_TYPE1_NAK

DESCRIPTION
  This function builds a Type 1 (i.e. Bitmap type) NAK frame using the supplied
  parameters. Note that the memory for storing the built NAK frame should have
  been allocated by the calling function.

DEPENDENCIES
  rlp_establish_srvc_type() should have been previously invoked to establish
  the type of RLP in use.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
/*lint -save -e641 */
LOCAL void build_type1_nak
(
  rlp_frame_type    *nak_ptr,           /* Pointer to the NAK frame          */
  rlp_seq_num_type   current_seq,       /* Current sequence number - V(S)    */
  byte               nak_map_cnt,       /* Number of NAK Maps                */
  nak_map_entry_type nak_map[RLP_TYPE2_MAX_NAK_MAPS]  /* NAK Map array       */
)
{
  byte *frame_ptr;                      /* To treat NAK frame as byte array  */
  byte i;                               /* Loop index                        */
  byte nak_map_seq_fpos;                /* For position of NAK_MAP_SEQ field */
  byte nak_map_fpos = 0;                /* For position of NAK_MAP field     */
  byte pad_fpos;                        /* For position of pre-FCS padding   */
  byte pad_length;                      /* For length of pre-FCS padding     */
  byte nak_length;                      /* For total length of NAK frame     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------------
    Bitmap NAK frame format is not defined for RLP Type 1, log error and exit.
  ---------------------------------------------------------------------------*/
  if( rlp_type == RLP_TYPE_I )
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Bitmap NAK is not defined for RLP Type 1");
  }

  /*---------------------------------------------------------------------------
    Build a RLP Type 2 Bitmap NAK frame.
  ---------------------------------------------------------------------------*/
  else if( rlp_type == RLP_TYPE_II )
  {
    if (nak_map_cnt == 0 || nak_map_cnt > 4)
    {
      DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                    "Array index nak_map_cnt is out of bounds %d", 
                    nak_map_cnt);
      return;
    }

    frame_ptr = (byte *)(&nak_ptr->frame.type2_nak);

    /*-------------------------------------------------------------------------
      Fill in the fields in the fixed-length part of the Bitmap NAK frame - SEQ,
      CTL, NAK_TYPE, L_SEQ_HI and NAK_MAP_COUNT fields.
    -------------------------------------------------------------------------*/
    b_packw(current_seq, frame_ptr, RLP_TYPE2_NAK1_SEQ_FPOS, RLP_TYPE2_NAK1_SEQ_LEN);
    b_packb(RLP_TYPE2_NAK >> 2, frame_ptr, RLP_TYPE2_NAK1_CTL_FPOS, RLP_TYPE2_NAK1_CTL_LEN);
    b_packb(RLP_TYPE2_NAK_TYPE1, frame_ptr, RLP_TYPE2_NAK1_NAK_TYPE_FPOS, RLP_TYPE2_NAK1_NAK_TYPE_LEN);
    b_packb((byte)((current_seq & 0x0F00) >> 8), (byte *)(&nak_ptr->frame.type2_nak),
            RLP_TYPE2_NAK1_L_SEQ_HI_FPOS, RLP_TYPE2_NAK1_L_SEQ_HI_LEN);
    b_packb((byte)(nak_map_cnt - 1), frame_ptr, RLP_TYPE2_NAK1_NAK_MAP_CNT_FPOS, RLP_TYPE2_NAK1_NAK_MAP_CNT_LEN);

    /*-------------------------------------------------------------------------
      Fill in the fields in the variable-length part of the NAK frame - upto
      NAK_MAP_COUNT records, each containing a NAK_MAP_SEQ (the first sequence
      number to be NAK'ed) and NAK_MAP (bitmap representing offsets from the
      starting sequence number).
      -----------------------------------------------------------------------*/
    for(i = 0; i < nak_map_cnt; i++)
    {
      DATA_RLP_MSG2(MSG_LEGACY_MED, " 1st seq = %u, Bitmap = 0x%x", nak_map[i].first_seq, nak_map[i].bitmap);
      nak_map_seq_fpos = RLP_TYPE2_NAK1_NAK_MAP_FPOS + (RLP_TYPE2_NAK1_NAK_MAP_SEQ_LEN + RLP_TYPE2_NAK1_NAK_MAP_LEN) * i;
      nak_map_fpos = nak_map_seq_fpos + RLP_TYPE2_NAK1_NAK_MAP_SEQ_LEN;     
      b_packw(nak_map[i].first_seq, frame_ptr, nak_map_seq_fpos, RLP_TYPE2_NAK1_NAK_MAP_SEQ_LEN);
      b_packb(nak_map[i].bitmap, frame_ptr, nak_map_fpos, RLP_TYPE2_NAK1_NAK_MAP_LEN);
    }

    /*-------------------------------------------------------------------------
      Calculate the position of the pre-FCS padding field (this padding is
      needed so that a byte-oriented CRC calculation may be performed).
    -------------------------------------------------------------------------*/
    pad_fpos = nak_map_fpos + RLP_TYPE2_NAK1_NAK_MAP_LEN;

    /*-------------------------------------------------------------------------
      Calculate the length of the pre-FCS padding field - 6 bits if the NAK
      Map Count is odd, or 2 bits if even. Then set the pad bits to zero.
    -------------------------------------------------------------------------*/        
    if( (nak_map_cnt & 0x01) != 0 )
    {
      pad_length = 6;
    }
    else
    {
      pad_length = 2;
    }
    b_packb(0x0, frame_ptr, pad_fpos, pad_length);

    /*-------------------------------------------------------------------------
      Determine the total length of the NAK frame using a lookup table, indexed
      by the NAK Map Count.
    -------------------------------------------------------------------------*/
    nak_length = rlp_type2_nak1_size_table[nak_map_cnt - 1];

    /*-------------------------------------------------------------------------
      Compute the CRC and append to the NAK frame.
    -------------------------------------------------------------------------*/
    crc_gen((byte *)(&nak_ptr->frame.type2_nak), nak_length);

    /*-------------------------------------------------------------------------
      Set the unused bytes in the frame to 0's.
    -------------------------------------------------------------------------*/
    memset(&frame_ptr[nak_length], 0, (RLP2_FULL_PRI_DATA_BYTES - nak_length));
  }

  else
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                  "Unknown RLP Type: %d, can't build Type 1 NAK");
  }

} /* build_type1_nak() */
/*lint -restore 641*/
/*=============================================================================

FUNCTION GET_NAK_FRAME

DESCRIPTION
  This function searches the NAK list and builds a NAK frame for transmission.
  The type of NAK frame built depends on the RLP Type in use. For RLP Type 1,
  this function builds a Type 0 (First/Last) NAK. For RLP Type 2, this function
  builds a Type 1 (Bitmap) NAK. Note that the memory for storing the built NAK
  frame should have been allocated by the calling function.

DEPENDENCIES
  The naks_to_send_cnt should indicate the total number of entries in the NAK
  list with pending NAKs, prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  The naks_to_send_count will be decremented for each NAK list entry whose
  sequence number was included in the NAK frame.

=============================================================================*/
LOCAL void get_nak_frame
(
  rlp_seq_num_type         current_seq,               /* Current Seq # - V(S)*/
  rlp_frame_type          *nak_ptr                    /* Pointer to NAK frame*/
)
{
  rlp_nak_list_entry_type *nak_list_ptr;              /* To search NAK list  */
  boolean                  do_first_last_nak = FALSE; /* TRUE => use 1st/Last*/
  boolean                  do_bitmap_nak = FALSE;     /* TRUE => use Bitmap  */
  boolean                  nak_done = FALSE;          /* TRUE => NAK complete*/
  boolean                  found_first = FALSE;       /* TRUE => First found */
  rlp_seq_num_type         first = 0;                 /* First Seq # to NAK  */
  rlp_seq_num_type         last = 0;                  /* Last Seq # to NAK   */
  byte                     nak_map_cnt = 0;           /* Number of NAK Maps  */
  nak_map_entry_type       nak_map[RLP_TYPE2_MAX_NAK_MAPS]; /* NAK Map array */
  byte                     bitmap = 0x0;              /* Missing frame bitmap*/
  byte                     diff;                      /* Used for Bitmap NAK */
  byte                     allowed_nak_maps = 0;      /* Max NAK Maps count  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------------------------------------------------
    First determine which type of NAK to use, based on the RLP Type.
  ---------------------------------------------------------------------------*/
  if( rlp_type == RLP_TYPE_I )
  {
    /*-------------------------------------------------------------------------
      For RLP Type 1, use a Type 0 (First/Last) NAK.
    -------------------------------------------------------------------------*/
    do_first_last_nak = TRUE;
  }
  else if( rlp_type == RLP_TYPE_II )
  {
    /*-------------------------------------------------------------------------
      For RLP Type 2, use a Type 2 (Bitmap) NAK.
    -------------------------------------------------------------------------*/
    do_bitmap_nak = TRUE;

    /*-------------------------------------------------------------------------
      NAKs are always sent in Rate 1/2 frames. For Rate Set 1, no more than 2
      NAK Maps will fit in a Rate 1/2 frame, therefore we must check to see
      which Rate Set we are currently using, and set the allowed NAK Map limit
      accordingly.
    -------------------------------------------------------------------------*/
    if( rlp_rate_set == RLP_RATE_SET_1 )
    {
      allowed_nak_maps = 2;
    }
    else if( rlp_rate_set == RLP_RATE_SET_2 )
    {
      allowed_nak_maps = RLP_TYPE2_MAX_NAK_MAPS;
    }
  }
  else
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                  "Unknown RLP Type: %d, can't get a NAK frame");
    return;
  }

  /*---------------------------------------------------------------------------
    Search the NAK list to find the sequence numbers that need to be included
    in a NAK frame. For each NAK list entry with a pending NAK, include the
    sequence number in the NAK frame, until (a) no more entries are found,
    (b) all the pending NAKs have been processed, or (c) the NAK frame cannot
    acccomodate any more sequence numbers.
  ---------------------------------------------------------------------------*/
  nak_list_ptr = (rlp_nak_list_entry_type *)q_check(&rlp_nak_q);

  while( (nak_list_ptr != NULL) && (naks_to_send_cnt != 0) && (!nak_done) )
  {
    if( !nak_list_ptr->tx_done )
    {
      /*-----------------------------------------------------------------------
        This NAK list entry has a pending NAK. Proceed based on the NAK Type.
      -----------------------------------------------------------------------*/

      if( do_first_last_nak )
      {
        /*---------------------------------------------------------------------
          Currently building a First/Last NAK. Check if this is the first
          sequence number to be NAK'ed.
        ---------------------------------------------------------------------*/
        if( found_first )
        {
          /*-------------------------------------------------------------------
            This is not the first sequence number. Check if this is the next
            consecutive sequence number in the current hole. If not, terminate
            the NAK list search and proceed to build the NAK frame.
          -------------------------------------------------------------------*/
          if( nak_list_ptr->seq == (last + 1) % RlpSeqModulus )
          {
            last = nak_list_ptr->seq;
          }
          else
          {
            nak_done = TRUE;
          }
        }
        else
        {
          /*-------------------------------------------------------------------
            This is the first sequence number to be NAK'ed. Set a flag to
            indicate that the first sequence number has been found, and then
            continue to search the NAK list for consecutive sequence numbers
            with pending NAKs.
          -------------------------------------------------------------------*/
          found_first = TRUE;
          first = nak_list_ptr->seq;
          last = first;
        }
      } /* do_first_last_nak */

      if( do_bitmap_nak )
      {
        /*---------------------------------------------------------------------
          Currently building a Bitmap NAK. Check if this is the first sequence
          number to be NAK'ed.
        ---------------------------------------------------------------------*/
        if( found_first )
        {
          /*-------------------------------------------------------------------
            This is not the first sequence number i.e. building of a NAK Map is
            already in progress. Compute the offset of the current sequence
            number from the base sequence number of the NAK Map. 
          -------------------------------------------------------------------*/
          if( (diff = (nak_list_ptr->seq - first) % RlpSeqModulus) <= 8 )
          {
            /*-----------------------------------------------------------------
              Offset <= 8. This sequence number can be included in the current
              NAK Map.
            -----------------------------------------------------------------*/
            bitmap = bitmap | (0x80 >> (diff - 1));
          }
          else
          {
            /*-----------------------------------------------------------------
              Offset > 8. Need to start a new NAK Map. First, save the previous
              base sequence number and bitmap in the NAK Map array.
            -----------------------------------------------------------------*/
            if (nak_map_cnt == 0 || nak_map_cnt >= RLP_TYPE2_MAX_NAK_MAPS)
            {
              DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Array index for nak_map out of bounds %d", 
                            nak_map_cnt);
              return;
            }

            nak_map[nak_map_cnt-1].first_seq = first;
            nak_map[nak_map_cnt-1].bitmap = bitmap;
            /*-----------------------------------------------------------------
              Check if the number of allowed NAK Maps has been exceeded. If so,
              terminate the NAK list search and proceed to build the NAK frame.
            -----------------------------------------------------------------*/
            if( nak_map_cnt >= allowed_nak_maps )
            {
              nak_done = TRUE;
            }
            /*-----------------------------------------------------------------
              For the new NAK Map, set the base sequence number to the current
              sequence number, and the bitmap to zero, to start with.
            -----------------------------------------------------------------*/
            else
            {
              nak_map_cnt++;
              first = nak_list_ptr->seq;
              bitmap = 0x0;
            }
          }
        } /* if found_first */
        else
        {
          /*-------------------------------------------------------------------
            This is the first sequence number to be NAK'ed. Set the base
            sequence number to the current sequence number, and the bitmap to
            zero, to start with.
          -------------------------------------------------------------------*/
          found_first = TRUE;
          first = nak_list_ptr->seq;
          bitmap = 0x0;
          nak_map_cnt = 1;
        }
      } /* do_bitmap_nak */

      if( !nak_done )
      {
        /*---------------------------------------------------------------------
          If all the rounds of NAKs for this entry have been completed, set the
          tx_done flag, so that this entry will now be aged during the RLP Rx
          processing.
        ---------------------------------------------------------------------*/
        if( --nak_list_ptr->repeat_cnt == 0 )
        {
          nak_list_ptr->tx_done = TRUE;
        }
        naks_to_send_cnt--;
      }
    } /* if !tx_done */

    nak_list_ptr = (rlp_nak_list_entry_type *)q_next(&rlp_nak_q, &nak_list_ptr->link);
  } /* while */

  if( do_first_last_nak && found_first )
  {
    /*-------------------------------------------------------------------------
      Build the Type 0 (First/Last) NAK frame.
    -------------------------------------------------------------------------*/
    DATA_RLP_MSG2(MSG_LEGACY_MED, "Sending NAK: first = %u, last = %u", first, last);
    build_type0_nak(nak_ptr, current_seq, first, last);
  }
  else if( do_bitmap_nak && (nak_map_cnt > 0) )
  {
    /*-------------------------------------------------------------------------
      Build the Type 1 (Bitmap) NAK frame.
    -------------------------------------------------------------------------*/
    nak_map[nak_map_cnt-1].first_seq = first;
    nak_map[nak_map_cnt-1].bitmap = bitmap;

    DATA_RLP_MSG1(MSG_LEGACY_MED, "Sending NAK: #maps = %u", nak_map_cnt);
    build_type1_nak(nak_ptr, current_seq, nak_map_cnt, nak_map);
  }
  else
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR, "%u NAKs to send, but NAK list is empty!",
                  naks_to_send_cnt);
  }

} /* get_nak_frame() */

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
  uint32                cur_packet_length;  /* current packet item length  */
  uint32                next_packet_length; /* next packet item length     */
  byte                  actual_fr_payload;  /* actual data payload count   */
  byte                  cur_item_copy_cnt;  /* current item copy count     */
  byte                  next_item_copy_cnt; /* next item copy count        */
  byte                  data_filler_cnt;    /* for filling end-of-frames   */
  byte                  avail_fr_payload;   /* available payload data count*/
  byte                  *dest_ptr;          /* pointer for byte copy       */
  dsm_item_type         *next_item_ptr;     /* pointer for data frame work */
  boolean               tx_data_frs = TRUE; /* transfering non idle frames */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Enter critical section to protect global RLP state variables
  -------------------------------------------------------------------------*/
  DS_ENTER_CRIT_SECTION(&rlp_crit_sect);
  
  /*=========================================================================
    Determine the next RLP frame to send. As specified in IS-99 section 
    3.7.1.2.2 the prioritized order is 1) RLP initialization frames, 2) NAK
    control frames, 3) Re-xmit frames, and 4) Data frames.
    
    Start by checking to see if the RLP initialization is underway.
  =========================================================================*/
  if( (rlp_state_info.state == RLP_SYNC_STATE) || 
      ((rlp_state_info.state == RLP_RT_MEAS_STATE) && (rlp_state_info.sync_rcvd)) )
  {
    /*-----------------------------------------------------------------------
      Sync-Ack in process. Send a SYNC, ACK or both. Use the proper frame
      format, based on whether RLP Type 1 or RLP Type 2 is in use.
    -----------------------------------------------------------------------*/
    if( rlp_state_info.state == RLP_SYNC_STATE)
    {
      if( rlp_type == RLP_TYPE_I )
      {
        /*-------------------------------------------------------------------
          Set the CTL field to indicate that this is an RLP Type 1 SYNC or
          SYNC/ACK frame.
        -------------------------------------------------------------------*/
        tx_frame.frame.gen_ctl.ctl_len = RLP_TYPE1_SYNC;

        if( rlp_state_info.sync_rcvd)
        {
          tx_frame.frame.gen_ctl.ctl_len |= RLP_TYPE1_ACK;
        }
      }
      else if( rlp_type == RLP_TYPE_II )
      {
        /*-------------------------------------------------------------------
          Set the CTL field to indicate that this is an RLP Type 2 SYNC or
          SYNC/ACK frame.
        -------------------------------------------------------------------*/
        if( rlp_state_info.sync_rcvd )
        {
          tx_frame.frame.type2_init.ctl_encr_mode = RLP_TYPE2_SYNC_ACK;
        }
        else
        {
          tx_frame.frame.type2_init.ctl_encr_mode = RLP_TYPE2_SYNC;
        }
      }
    }
    else /* RT_MEAS state */
    {
      if( rlp_type == RLP_TYPE_I )
      {
        /*-------------------------------------------------------------------
          Set the CTL field to indicate that this is an RLP Type 1 ACK frame.
        -------------------------------------------------------------------*/
        tx_frame.frame.gen_ctl.ctl_len = RLP_TYPE1_ACK;
      }
      else if( rlp_type == RLP_TYPE_II )
      {
        /*-------------------------------------------------------------------
          Set the CTL field to indicate that this is an RLP Type 2 ACK frame.
        -------------------------------------------------------------------*/
        tx_frame.frame.type2_init.ctl_encr_mode = RLP_TYPE2_ACK;
      }
    }

    if( rlp_type == RLP_TYPE_I )
    {
      /*---------------------------------------------------------------------
        Fill in the remaining fields (SEQ, FIRST, LAST) in the RLP Type 1
        Control frame.
      ---------------------------------------------------------------------*/
      tx_frame.frame.gen_ctl.seq_num   = (byte)rlp_state_info.next_seq_num;
      tx_frame.frame.gen_ctl.first_seq = 0;
      tx_frame.frame.gen_ctl.last_seq  = 0;
    }
    else if( rlp_type == RLP_TYPE_II )
    {
      /*---------------------------------------------------------------------
        Fill in the SEQ field in the RLP Type 2 Control frame.
      ---------------------------------------------------------------------*/
      tx_frame.frame.type2_init.seq_num = (byte)rlp_state_info.next_seq_num;
    }

    if( primary_frame )
    {
      frame_rate = RLP_HALF_RATE;
      (void)memset((void *)tx_frame.frame.gen_ctl.unused, 0, RLP2_FULL_GEN_CTL_UNUSED_BYTES);
    }
    else /* secondary/signaling channel */
    {
      /*---------------------------------------------------------------------
        Sec./Sig. channel. Set rate to required rate then fillout unused
        portion of the frame.
      ---------------------------------------------------------------------*/
      frame_rate = allowed_rate;
      (void)memset((void *)tx_frame.frame.gen_ctl.unused, 0, RLP2_FULL_GEN_CTL_UNUSED_BYTES);
    }

    *frame_ptr = (byte *)&tx_frame.frame;

    /*-----------------------------------------------------------------------
      Compute the CRC and append to the frame.
    -----------------------------------------------------------------------*/
    if( rlp_type == RLP_TYPE_I )
    {
      crc_gen((byte *)&tx_frame.frame, RLP_TYPE1_CTL_FRAME_SIZE);
    }
    else if( rlp_type == RLP_TYPE_II )
    {
      crc_gen((byte *)&tx_frame.frame, RLP_TYPE2_INIT_FRAME_SIZE);
    }
  }
  /*=========================================================================
    Check for NAK(S) TO SEND. RLP in ESTABLISHED state or RT_MEAS state with
    SYNC_RCVD FALSE.
  =========================================================================*/
  else if( naks_to_send_cnt != 0 )
  {
    /*-----------------------------------------------------------------------
      There are some NAK(s) to send. Set the frame rate to use.
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

    /*-----------------------------------------------------------------------
      Call a function to search the NAK list and build a NAK frame for
      transmission.
    -----------------------------------------------------------------------*/
    get_nak_frame(rlp_state_info.next_seq_num, &tx_frame);

    *frame_ptr = (byte *)(&tx_frame.frame);
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
        (void)memset((void *)(tx_frame.frame.gen_data.data + actual_fr_payload), 0, 
                      avail_fr_payload - actual_fr_payload);
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
      	              rlp_tx_segfr_src_ptr, actual_fr_payload );
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
           frame.pri_fr_data.data[rlp_prifull_table[rlp_rate_set].
           format_b_payload] == rlp_prifull_table[rlp_rate_set].format_b_new))
      {
        /*-----------------------------------------------------------------
          Rexmit was Primary Full rate B. Set count/source ptr.
        -----------------------------------------------------------------*/
        actual_fr_payload = rlp_prifull_table[rlp_rate_set].format_b_payload;
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
        (void)memset((void *)rlp_rexmit_ptr_array[rlp_state_info.re_xmit_out_index]
                     .frame_ptr->frame.gen_data.data, 0, avail_fr_payload);
        rlp_state_info.re_xmit_frames_cnt--;
        frame_rate = allowed_rate;
        rlp_rexmit_ptr_array[ rlp_state_info.re_xmit_out_index].frame_ptr->
          rate     = allowed_rate;
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
                       rlp_tx_segfr_src_ptr, avail_fr_payload );
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
                         ( rlp_state_info.next_seq_num + 1) % RlpSeqModulus;

    /*-----------------------------------------------------------------------
      Determine the available frame payload based on the channel type and/or
      the required frame rate.
    -----------------------------------------------------------------------*/                 
    if( primary_frame)
    {
      frame_rate       = RLP_FULL_RATE;
      avail_fr_payload = rlp_prifull_table[rlp_rate_set].format_b_payload;
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
           (uint32)( avail_fr_payload - cur_item_copy_cnt))
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
        actual_fr_payload == rlp_prifull_table[rlp_rate_set].format_b_payload)
    {
      /*---------------------------------------------------------------------
        Set Type field then set dest_ptr for payload.
      ---------------------------------------------------------------------*/
      rlp_saved_fr_array[rlp_state_info.saved_frames_index].frame.pri_fr_data.
        data[rlp_prifull_table[rlp_rate_set].format_b_payload] = 
        rlp_prifull_table[rlp_rate_set].format_b_new;
      dest_ptr = (byte *)rlp_saved_fr_array[rlp_state_info.saved_frames_index].
                 frame.pri_fr_data.data;
    }
    else /* Generic frame format */
    {
      if( primary_frame)
      {
        rlp_saved_fr_array[rlp_state_info.saved_frames_index].frame.
        pri_fr_data.data[rlp_prifull_table[rlp_rate_set].format_b_payload] = 
        rlp_prifull_table[rlp_rate_set].format_a;
      }
      dest_ptr = (byte *)rlp_saved_fr_array[rlp_state_info.saved_frames_index].frame.
         gen_data.data;
      rlp_saved_fr_array[rlp_state_info.saved_frames_index].frame.gen_data.
         ctl_len = actual_fr_payload;
      if( data_filler_cnt != 0)
      {
       /* Below check is just to fix Klockwork error. Revisit for any issues. */
        if( (actual_fr_payload+data_filler_cnt) < RLP2_FULL_GEN_DATA_BYTES )
        {
          (void)memset((void *)(rlp_saved_fr_array[rlp_state_info.saved_frames_index]
                       .frame.gen_data.data + actual_fr_payload), 0,
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
    tx_data_frs = FALSE; /* Sending idle frame */
    tx_frame.frame.pri_idle.seq_num = (byte)rlp_state_info.next_seq_num;
    tx_frame.frame.pri_idle.nr_crc  = nr_table[(byte)rlp_state_info.next_seq_num];
    frame_rate                      = RLP_EIGHTH_RATE;
    *frame_ptr                      = ( byte *)&tx_frame.frame;
    if( rlp_rate_set == RLP_RATE_SET_2 )
    {
      tx_frame.frame.pri_idle.rs2_filler = 0;
    }
  }
  else /* Send Sec/Sig channel Idle (zero length data frame) */
  {
    tx_data_frs = FALSE; /* Sending idle frame */
    frame_rate                      = allowed_rate;
    *frame_ptr                      = ( byte *)&tx_frame.frame;
    tx_frame.rate                   = allowed_rate;
    tx_frame.pri_frame              = primary_frame;
    tx_frame.frame.gen_data.seq_num = (byte)rlp_state_info.next_seq_num;
    tx_frame.frame.gen_data.ctl_len = 0;
    (void)memset((void *)tx_frame.frame.gen_data.data, 0,
                  RLP2_FULL_GEN_DATA_BYTES);
  }

  ds_rlp_data_txed(tx_data_frs); /* Notify ds about tranmission */
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
/*lint -save -e641 */
void rlp_get_stats
(
  rlp_ext_stats_type *stats_ptr      /* pointer to item to load with stats */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  dword rx_stat_total;            /* total of rlp_rx_stat_tot[][] */
  dword tx_stat_total;            /* total of rlp_tx_stat_tot[][] */
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
  stats_ptr->tx_frame_cnt = (word)tx_stat_total;
  stats_ptr->rx_frame_cnt = (word)rx_stat_total;
  stats_ptr->tx_data_cnt  = (word) rlp_gen_stats.tx_data_cnt;
  stats_ptr->rx_data_cnt  = (word) rlp_gen_stats.rx_data_cnt;
  stats_ptr->nak_cnt      = (byte) rlp_gen_stats.single_naks;
  stats_ptr->rexmit_cnt   = (byte) rlp_gen_stats.re_xmits;
  stats_ptr->state        = rlp_state_info.state;

} /* rlp_get_stats */
/*lint -restore 641*/

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
  rlp_last_call_synced_ptr = (byte *)rlp_last_call_synced[ds_last_call_var];

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

#endif /* FEATURE_IS95B_MDR */
#endif /* FEATURE_DS */

#ifndef FEATURE_IS95B_MDR
extern dsm_watermark_type *rlp_post_rx_wm_ptr;
extern void               (*rlp_post_rx_func_ptr)(uint32);
#endif 

/*===========================================================================

FUNCTION RLP_ESTABLISH_SRVC_TYPE

DESCRIPTION
  This function establishes the service configuration to be used for the RLP
  session. The service configuration includes the traffic type, rate set and
  rlp type. This function should be invoked after service negotiation is
  complete, but before attempting to receive or transmit any RLP frames.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  This function will cause an RLP reset that will reset the RLP state.
===========================================================================*/

void rlp_establish_srvc_type
(
  rlp_traf_type_enum_type    traffic_typ,  /* Primary or Secondary Traffic */
  rlp_rate_set_enum_type     rate_set,     /* Rate Set 1 or Rate Set 2     */
  rlp_type_enum_type         rlp_typ       /* RLP Type 1 or RLP Type 2     */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
#ifdef FEATURE_IS95B_MDR
  

  /*-------------------------------------------------------------------------
    Establish the traffic type (Primary or Secondary) over which RLP frames
    will be carried.    
  -------------------------------------------------------------------------*/
  rlp_traffic_type = traffic_typ;

  DATA_RLP_MSG1(MSG_LEGACY_MED, "  Traffic Type = %d", rlp_traffic_type);
  if( rlp_traffic_type == RLP_TRAFFIC_TYPE_PRI )
    {
      /* RLP on primary traffic */
      rlp_ct_offset = 0;
    }
  else if( rlp_traffic_type == RLP_TRAFFIC_TYPE_SEC )
    {
      /* RLP on secondary traffic */
      rlp_ct_offset = 1;
    }

  /*-------------------------------------------------------------------------
    Establish the Rate Set (Rate Set 1 or Rate Set 2) to be used.
  -------------------------------------------------------------------------*/
  rlp_rate_set = rate_set;

  DATA_RLP_MSG1(MSG_LEGACY_MED, "  Rate Set = %d", rlp_rate_set);
  if( rlp_rate_set == RLP_RATE_SET_1 )
    {
      rlp_rs_offset = 0;
    }
  else if( rlp_rate_set == RLP_RATE_SET_2 )
    {
      rlp_rs_offset = 2;
    }

  rlp_params_offset = rlp_rs_offset + rlp_ct_offset;

  /*-------------------------------------------------------------------------
    Establish the RLP Type (Type 1 or Type 2) to be used.
  -------------------------------------------------------------------------*/
  rlp_type = rlp_typ;
  DATA_RLP_MSG1(MSG_LEGACY_MED, "RLP Type 1 (IS-707) RLP Type 2 (IS-707-A/MDR)"
                "RLP Type = %d", rlp_type );

  /*-------------------------------------------------------------------------
    Establish other RLP parameters which are dependent on the RLP Type in use.
  -------------------------------------------------------------------------*/
  if( rlp_type == RLP_TYPE_I )
  {
    RlpSeqModulus = RLP_TYPE1_SEQ_MODULUS;
    RlpMaxContigErase = RLP_TYPE1_MAX_CONTIG_ERASE;
    RlpMaxVnVrDelta = RLP_TYPE1_MAX_VN_VR_DELTA;
  }
  else if( rlp_type == RLP_TYPE_II )
  {
    RlpSeqModulus = RLP_TYPE2_SEQ_MODULUS;
    RlpMaxContigErase = RLP_TYPE2_MAX_CONTIG_ERASE;
    RlpMaxVnVrDelta = RLP_TYPE2_MAX_VN_VR_DELTA;
  }

   /*-------------------------------------------------------------------------
    Now reset RLP. After the reset, RLP will start using the established
    service configuration.
  -------------------------------------------------------------------------*/
  rlp_reset();
#else
  DATA_RLP_MSG0(MSG_LEGACY_HIGH,
                " RLP Establish SRVC TYPE Not supported for IS95A ");
#endif /* FEATURE_IS95B_MDR */

} /* rlp_establish_srvc_type */

/*===========================================================================
FUNCTION      DS_PROCESS_RLP_RX_FRAMES

DESCRIPTION   This function invokes the RLP RX function to process RLP
              frames received over the air, and terminates the call if the
              RLP protocol fails to synchronize. For packet data calls, it
              also maintains the dormancy state based on the return value
              from the RLP RX processing,

              This function should be called from the RX task, for both MDR
              and non-MDR calls.

              Note that this calls the CTA start/stop functions for all
              calls (even async/fax).  This is okay, as the pkt mgr has
              locked down the CTA timer unless there is an active Pkt
              call.  Meaning that an async call will have no effect
              on the CTA timer.

DEPENDENCIES  This is called by RLP I and RLP II.  This means that it is
              called for both async and packet calls.  However, the idle
              timeout is specific only to packet calls.  Further, need to
              be able to figure out which ps_iface to use when checking to
              see if in a call.  Since async & pkt 707 calls are not
              allowed simultaneously (assumption), can see which of the
              two ifaces is active, and make decision accordingly.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_process_rlp_rx_frames
(
  rlp_fwd_frame_block_type  *fwd_frame_block,        /* pt to RLP fwd frame block */
  rlp_rate_enum_type        frame_rate,              /* Rate of RLP frame    */
  boolean                   pri_chan,                /* Primary Channel indicator */
  dsm_item_type             *rlp_item_ptr            /* ptr to RLP frame item  */
)
{
#ifdef FEATURE_DS

#ifdef FEATURE_IS95B_MDR
  rlp_rx_retval_enum_type rlp_rx_status;   /* return status from RLP RX    */
  byte                    i;
#else
  rlp_rx_status_enum_type rlp_rx_status;   /* return status from RLP RX    */
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_IS95B_MDR
  DATA_MDM_ARG_NOT_USED(frame_rate);
  DATA_MDM_ARG_NOT_USED(pri_chan);
  DATA_MDM_ARG_NOT_USED(rlp_item_ptr);
#else
  DATA_MDM_ARG_NOT_USED(fwd_frame_block);
#endif /* FEATURE_IS95B_MDR */

  /*-------------------------------------------------------------------------
    Do not do RLP RX processing unless an active call is in progress.
  -------------------------------------------------------------------------*/
  if ( ( rlp_post_rx_wm_ptr != NULL) || ( rlp_post_rx_func_ptr != NULL) )
  {
    /*-----------------------------------------------------------------------
      Pass RLP frame rate, channel type, and frame to RLP RX for processing.
      If the return value indicates data bytes are available then latch data
      available flag and cancel Idle timer, if in Timeout mode.
    -----------------------------------------------------------------------*/
#ifdef FEATURE_IS95B_MDR
    rlp_rx_status = rlp_rx_process_frames(fwd_frame_block);
#else
    rlp_rx_status = rlp_rx_process_frame(frame_rate, pri_chan, rlp_item_ptr);
#endif

#ifdef FEATURE_IS95B_MDR
    if( rlp_rx_status == RLP_RX_NON_IDLE )
#else
    if( rlp_rx_status == RLP_RX_DATA_AVAILABLE)
#endif
    {
      /*---------------------------------------------------------------------
       Received data.  Cancel IDLE timer.
      ---------------------------------------------------------------------*/
      ds_rlp_data_rxed(TRUE);
    } /* if RLP data available */

#ifdef FEATURE_IS95B_MDR
    else if( (rlp_rx_status == RLP_RX_IDLE)   ||
             (rlp_rx_status == RLP_RX_ERROR))
#else
    else if(( rlp_rx_status    == RLP_RX_NO_DATA_AVAILABLE))
#endif
    {
      /*---------------------------------------------------------------------
        Enable the idle timer if no data.
      ---------------------------------------------------------------------*/
      ds_rlp_data_rxed(FALSE);
    } /* If no RLP RX data */
#ifdef FEATURE_IS95B_MDR
    else if( rlp_rx_status == RLP_RX_SYNCH_TIMEOUT )
#else
    else if( rlp_rx_status == RLP_RX_SYNC_TIMEOUT )
#endif
    {
      /*---------------------------------------------------------------------
        The RLP peers have failed to Sync. End the call.
      ---------------------------------------------------------------------*/
      DATA_RLP_MSG0(MSG_LEGACY_HIGH, "RLP failed to SYNC, Ending Call");
      ds707_cta_rlp_sync_timeout();
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      No data call in process, free the RLP frame buffer(s).
    -----------------------------------------------------------------------*/
#ifdef FEATURE_IS95B_MDR
    for(i = 0; i <= fwd_frame_block->num_sup; i++)
    {
      (void)dsm_free_buffer(fwd_frame_block->fwd_frames[i].rlp_item_ptr);
    }
#else
    dsm_free_buffer( rlp_item_ptr);
#endif
  }
#else
  DATA_RLP_MSG0(MSG_LEGACY_ERROR, "ds_process_rlp_rx_frames() not supported");
#endif /* FEATURE_DS */
} /* ds_process_rlp_rx_frames() */
