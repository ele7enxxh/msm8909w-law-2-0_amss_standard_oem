/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                R A D I O   L I N K   P R O T O C O L  I N T E R N A L

GENERAL DESCRIPTION
  This file contains general functions that are called from the different 
  RLP modules. The functions in this file are not exposed to modules outside
  of the dsrlp modules.
  Function prototypes for the external functions in this file are given in
  dsrlpi.h
  
  Naming convention in this file is:
  All external functions start with dsrlpi..
  All internal functions start with rlpi...
  All internal variables (in file scope) start with rlpi..
  All external variables start with dsrlpi.. and are defined in dsrlpi.h
  
EXTERNALIZED FUNCTIONS
  
  dsrlpi_determine_tx_fr_type()
    Determines the type of the RLP frame that needs to be built.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  RLP configuration and initialization procedures.

Copyright (c) 1999-2016 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlpi.c_v   1.36   28 Jan 2003 16:57:52   kdevara  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlpi.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/02/16   sd         Fix to indicate 1X to clear reverse link RLP frame 
                      parameters whenever RLP reset happens.
12/18/13   sd         Added critical sections to avoid race condition issues
04/26/13   sd         Fixed assert failure for RLP state crash.
11/18/11   vpk        HDR SU API cleanup
02/08/11   ls         Global variable cleanup
02/04/11   op         Data SU API cleanup
01/18/11   op         Migrated to MSG 2.0 Macros
10/14/10   sa         Adding debug messages before ASSERT.
06/24/10   rp         Fixed Compiler warnings.  
04/16/10   ls         Fix the featurization issues
09/30/09   ss         Klocwork error fixes.
03/19/09   ms         Fixed Lint errors.
01/20/09   pp         Metainfo clean-up.
06/25/08   as         MMPA: Fix to send Quick Repeat NAKs in a timely manner
04/23/08   sn         Fixed klocwork error.
05/10/07   ac         Lint fix
03/05/07   ac         Merge the change in the EMPA sandBox 5209 in the tip
03/05/07   spn        Fixed lint errors
07/25/06   ksu        EMPA: 1x RLP code review comments
05/30/06   ksu        EMPA: RLP init code review comments
04/17/06   squ        When recv sync_ack frame, set ack_to_send flag to
                      TRUE. This fixed the problem that rlp does not send
                      ACK if rlp recv sync_ack and data frames at same time.
03/23/06   TMR        Added #FEATURE_HDR around hdrrlphandle_rlp_reset() call
01/03/06   ksu        EMPA: hook up forward link with new phys link architecture
12/23/05   ksu        EMPA RSP: two fwd rx queues
12/15/05   TMR        Changed RLP RX queue to watermark and added parameter to 
                      dsrlp_reg_srvc() and RX callback function
12/16/05   ksu/sy     Introduced a critical section between RX and TX task to 
                      prevent RX task from resetting the RLP while the
                      the TX task build the packets and waiting for a 
                      response from PCP.
12/12/05   ksu/sy     Added runtime checks for dsrlpdel functions as it is
                      used by 1x only.
11/15/05   ksu        EMPA: packet stream, octet sequencing (forward link also)
11/03/05   ksu        EMPA: packet stream, octet sequencing (reverse link only)
10/25/05   sy/as      Removed dsrlpi_get_free_rscb_index() as it is not used
                      anywhere.
09/28/05   ksu/sy     Reset resequencing queue before zeroing LV(N)
09/28/05   ksu/sy     HDR: use one-shot abort timer instead of 20ms ticks
08/29/05   ksu/sy     Remove dsrlp_reg_hdlc() in favor of uint32 app_field
                      parameter to dsrlp_reg_srvc().
09/28/05   ksu/sy     Reset resequencing queue before zeroing LV(N)
09/16/05   ksu/sy     MRLP reset should clear flush timer.
09/01/05   ksu/sy     RLP reset should clear flush timer.
08/29/05   ksu        Move tx_item_ptr cleanup from dsrlpi_reset() to
                      dsrlpi_cleanup() to prevent loss of unsent data.
08/11/05   tc         Updated dsrlpi_reset() to free handle_tx_new_data_count.
07/25/05   tc         Updated dsrlpi_reset() to free local_tx_item_len.
07/18/05   tc         Supported multiple format B connection layer packets in
                      one RTCMAC sub frame.
05/27/05   tc         Updated dsrlpi_reset() to free local_rlp_packet_ptr
11/29/04   gr         Merged PZID hysteresis changes into main line
09/20/04   vas        Changes for QOS
09/08/04   vr         Notify the PZID hysterisis engine if a PPP packet is
                      exchanged first time after dormancy
07/22/04   atp        Got rid of some minor compiler warnings.
04/07/04    sy        Made payload_in_frame and payload_in_wm variable from
                      uint16 to uint32 as the watermark items have changed to
                      uint32.
02/03/04    ak        Meta info now inside dsm item data block, so extract & 
                      pull up appropriately.
01/28/03   kvd        Modified 4 dsrlpseq macros DSRLPSEQ_GET_NEXT_SEQ,
                      DSRLPSEQ_INCREMENT_SEQ, DSRLPSEQ_INCREMENT_S_SEQ,
                      DSRLPSEQ_INCREMENT_32BIT_SEQ to include modulus as
                      an argument
12/02/02   sy         Fixed problems in tx_new_data and rexmit_data_frame
                      which were happening when the fill frame freqency
                      has been changed for mux PDU5.
11/08/02   atp        Added freeing of meta_info to prevent memory leak.
10/18/02   sy         Fixed the wrong mux PDU5 partial frame padding.
10/15/02   js         Optimized F3 messages by adding more information,
                      and removing unnecessary printouts. 
08/26/02   js         Changed erroneous PDU type checking in
                      dsrlpi_tx_new_data_frame() and
                      dsrlpi_rexmit_data_frame().
           js         Changed memory allocation in dsrlpi_tx_new_data_frame()
                      to small DSM item only, which fixed the 16x MuxPDU5 on
                      r-sch transmission problems.
08/08/02   js         Fixed minor compile-time warnings for non-MuxPDU5 
                      build. 
07/25/02   js         Merged in the MuxPDU5 support from branch to the tip.
07/25/02   snn        Fix the DSM memory access violation problems with
                      MuxPDU5/Turbo/FER. 
07/03/02   sy         Added support for reverse MuxPDU5. The functions that
                      are affected by this are dsrlpi_tx_new_data_frame()
                      and dsrlpi_rexmit_data_frame().
04/18/01   cc         Change MSG_HIGH("BURST GAP ....") to MSG_ERROR and
                      MSG_HIGH("FCH RDA gap ....") to MSG_ERROR to help testers
                      Change ERR("Format B frame, but not correct length") to MSG_MED
02/20/02   snn        Edited the Error messages to give more info.
02/04/02   ss         Changed DSM_LESS_THAN_FEW_FREE_ITEMS to 
                      DSM_LESS_THAN_LINK_LAYER_FEW_FREE_ITEMS
01/31/02   snn        Modified bit mask in the dsrlpi_type_mask_table for 
                      secondary data.
09/14/01   rama       SR_ID fix. Differentiate between SR_ID and RLP Instance. 
                      Set a mapping between SR_ID and RLP Instance.
09/10/01   igt        Changed the location rx_total_bytes gets updated to avoid
                      potential double-counting.
08/13/01   snn        Added extra field in dsrlpi_type_mask_table for 
                      MuxPDU5.
08/02/01   ak         Set a flag indicating when dsrlpi_reset occuring. Look
                      for potential burst alignment issues when rx_new_data.
07/23/01   pjb        Removed call to Zone bits. 
07/01/01   ak         Removed call to dsrlprsq_remove().
06/27/01   sy         changed dsrlpi_rx_new_data() to handle the 
                      zero nak_rounds.case.
06/13/01   ak         Fixed bad if() statement in tx_new_data_frames().
05/27/01   snn        Updated the dsrlpi_tx_new_data_frame function for Rs2.
05/24/01   hcg        Made changes to support HDR (1xEV).
05/21/01   sy         Added extra flag for reset to check if this flag is set
                      every 20msec.
05/11/01   ak         MAX_CHAINS changed.  Now only remove a certain # of
                      items from the reseq-queue.
03/30/01   na         Fixed potential bug in dsrlpi_tx_new_data_frame() which
                      could go into an infinite while loop is a zero length
                      item was queued on the tx_wm.
02/22/01   ak         Process_rexmit_Frames now in IRAM.
02/21/01   ak         Support for checking against old frames and also for
                      potentially mis-RDA'd frames which cause big NAK gaps.
01/23/00   ak         Fixes to handle less-than-full rate frames when doing
                      tx_new_data and rexmit_data_frame.
12/06/00   ak         Changes to force FILL frames every 80 frames so that
                      other side gets our V(N) appropriately.
11/15/00   igt        Change calls to dsrlptxq_add_frame() and dsrlptxq_reset()
                      to reflect the new interface.
08/25/00   rc         Changed dsm_new_buffer() calls to take pool id as input 
                      parameter. So, a new macro DSM_DS_POOL_SIZE is used to 
                      return pool id based on pool size. Added Macros that 
                      check if running out of dsm items.                    
08/04/00   igt        changed updating for tx_total_bytes so it is accurate
07/31/00   ak         Further IRAM #ifndefs for R-SCH and F-SCH.
06/15/00   ak         Fixes for 1x R-SCH: make sure used field is set to max
                      value, as SCH takes only full rate frames.
06/08/00   ak         Added MSM5000_IRAM_FWD #ifndef's for IRAM support.
06/05/00   ak         Change to support use of a callback rx func ptr instead
                      of the rx watermark, in rx_new_data.
06/03/00   ak         Added check in process_rexmit for null dsm item ptr.
06/01/00   ak         Added check in rx_new_data for a null dsm item ptr.
05/15/00   na/ak      Support for delayed frame detection.
05/10/00   ak         Added some fields to state, to help in sending fills
                      and improve rlp mux erasure reporting.  No longer 
                      reset on rx'ing Ack or S/A when in EST'D state.  Fixed
                      SEQ_HI checks in tx_new_fr and tx_rexmit.
05/02/00   na         If V(S)-Peer-V(N) > 255, the SEQ_HI is now included in
                      new frames.
04/28/00   igt        move resets++ into dsrlpi_reset(), update rx_total_bytes
                      tx_total_bytes and re_xmits_not_found statistics.
04/19/00   ak         Now return DSRLPI_RLP_ERASURE instead of DSRLPI_ERASURE  
04/13/00   ak         Made MSGs more screen friendly.  Added bytes to record
                      first 3 bytes of each frame for debugging.  Also inc
                      rlp_delay on sync->ack transition.
04/12/00   na         Fixed problem in tx_new_data(), where used field was
                      not being incremented to account for zero padding.
02/18/00   ak         Make memory checks and dump incoming data if memory
                      is low.
01/20/00   ak         Re-ordered execution of dsrlpnak_remove and
                      dsrlprsq_remove, to account for dsrlpnak_remove
                      updating V(N).
01/19/00   ak         Fixed some ADD_S_SEQ issues with adding the correct
                      amount.
01/19/00   na         Fixed problem with transmitting new frames on DCCH/SCH.
01/14/00   ak         Removed typebit_pos array and references to it in the
                      code.
01/12/00   na         Fixed a memory leak when a duplicate rexmit was
                      received
01/11/00   ak         In dsrlpi_reset, removed resetting of nak params, as
                      that is now done in dsrlp_init().
01/10/00   na         Added a MSG_MED to show when a NAK will be sent.
01/07/00   ak         Handle dsrlpnak_remove and dsrlpnak_remove returning
                      boolean values.  Also in fr_det, remove item from 
                      rexmit queue if its CRC fails.
01/06/00   na         Added MSG_MED to print rexmit frame seq#; added generic
                      srvc type to dsrlpi_type_mask_table.
08/09/99 na/hcg/ak    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "data_msg.h"

#if (defined (FEATURE_DS_RLP3) || defined (FEATURE_HDR))
#include "dsrlp_v.h"
#include "dsrlpi.h"
#include "dsrlpnak.h"
#include "dsrlpseq.h"
#include "dsrlptxq.h"
#include "dsrlprtx.h"
#include "dsrlprsq.h"
#include "dsrlpdel.h"
#include "amssassert.h"
#include "memory.h"
#include "bit.h"
#include "crc.h"
#include "err.h"
#include "time_svc.h"
#include "time_tod.h"
#include "txc.h"
#ifdef FEATURE_DS_RLP3
#include "dsrlp3.h"
#endif
#ifdef FEATURE_HDR
#include "hdrrlp.h"
#include "hdrrlpnak.h"
#include "hdrrlptxph.h"
#include "hdrrlprsq.h"
#include "hdrrlptxq.h"
#include "hdrrlptxsh.h"
#include "hdrrlpmarq.h"
#include "hdrrlpqn.h"
#endif

#ifdef FEATURE_HDR_REVB
#include "hdrrlpmlnak.h"
#include "hdrrlpdnak.h"
#endif

#ifdef FEATURE_HDR_QOS
#include "hdrmrlp.h"
#include "hdrmrlpi.h"
#endif
#include "rex.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Struct used in retrieve rexmit information.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32       nak_first_seq;     /* first seq number nak'd                */
  uint32       nak_last_seq;      /* last seq number nak'd                 */
  word         data_payload_len;  /* length of payload data in dsm chain   */
} rlpi_rexmit_info_type;

/*---------------------------------------------------------------------------
  Definitions for masks to determine if an 1/8th rate frame is an idle or 
  a fill frame in rateset2.
---------------------------------------------------------------------------*/
#define RLPI_IDLE_FILL_MASK   0xF0       /* Mask to check Rate Set 2 Idles */
#define RLPI_IDLE_LAST_BITS   0          /* Last 4 bits for RateSet2 Idles */
#define RLPI_FILL_LAST_BITS   0xF0       /* Last 4 bits for 1/8 fills      */

/*---------------------------------------------------------------------------
  Defines the minimum number of bytes in a new data frame.  For FCH, 
  it depends on if bytes-sequenced mode or not (which changse the size of
  the header).  Also have to account for if handling rexmit ambiguity (which
  requires us to send SEQ_HI).
  
  For SCH, usually has to be full-rate.  So we use a value which covers
  the worst case header, which is Format D, byte-sequenced.   Don't count
  the two bits which make up the "type" bits.
  
  Since it's 6 bytes for FCH and SCH as a first threshold, create that 
  #define.
---------------------------------------------------------------------------*/
#define RLPI_FCH_UNSEG_DATA_MIN_SIZE          3

#define RLPI_FCH_SEG_DATA_MIN_SIZE            6
 
#define RLPI_SCH_MIN_DATA_SIZE                6

#define RLPI_THRESH_BYTES_IN_DATA_FRAME       6

/*---------------------------------------------------------------------------
  Enums used in rexmitting data, to indicate what kind of frame format
  to use.
---------------------------------------------------------------------------*/
typedef enum
{
  RLPI_SND_FR_FMT_NONE = 0,
  RLPI_SND_FR_FMT_A,
  RLPI_SND_FR_FMT_B,
  RLPI_SND_FR_FMT_C,
  RLPI_SND_FR_FMT_D
}rlpi_snd_fr_fmt_type;



/*===========================================================================

            EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  This is the array of session control blocks, which contains
  various pieces of info for each type of RLP.
---------------------------------------------------------------------------*/
dsrlpi_sess_ctl_blk_type  dsrlpi_rscb_array[ DSRLP_MAX_RSCB_ELEMENTS ];

/*---------------------------------------------------------------------------
  This is the mapping of sr_id to index in the session control block array.
---------------------------------------------------------------------------*/
byte dsrlpi_sr_id_rscb_map[ DSRLPI_MAX_SP_SR_ID_VALUE];

/*---------------------------------------------------------------------------
  Table is indexed by MuxPDU type and service class (primary or secondary). 
  Fields: type_mask, Format A type bits, Format B/C type bits for new frames,
  Format B/C type bits for rexmits,      Format D type bits.
---------------------------------------------------------------------------*/
const dsrlpi_type_mask_table_type dsrlpi_type_mask_table[4][3] =
{
 /* MuxPDU type 1 */
  {{0xE0, 0x20, 0x40, 0x60, 0x00},  /* primary                                */
  {0xFF, 0x01, 0x02, 0x03, 0x00},  /* secondary                              */
  {0x00, 0x00, 0x00, 0x00, 0x00}},  /* Generic Service - not applicable       */
  
  /* MuxPDU type 2 */
  {{0xC0, 0x40, 0x80, 0xC0, 0x00},  /* primary                                */
  {0xFC, 0x04, 0x08, 0x0C, 0x00},  /* secondary                              */
  {0x00, 0x00, 0x00, 0x00, 0x00}},  /* Generic Service - not applicable       */
 
  /* MuxPDU type 3 */
  {{0x03,    0, 0x02, 0x03, 0x00},  /* primary                                */
  {0x03,    0, 0x02, 0x03, 0x00},  /* secondary                              */ 
  {0x03,    0, 0x02, 0x03, 0x00}},  /* Generic Service                        */ 

  /* MuxPDU type 5 */
  {{0x03,    0, 0x02, 0x03, 0x00},  /* primary                                */
  {0x03,    0, 0x02, 0x03, 0x00},  /* secondary                              */ 
  {0x03,    0, 0x02, 0x03, 0x00}}   /* Generic Service                        */ 
};

/*---------------------------------------------------------------------------
  Bytes used in printing out RLP-level erasures, to help in debugging.
---------------------------------------------------------------------------*/
byte dsrlpi_ctl_byte1,dsrlpi_ctl_byte2,dsrlpi_ctl_byte3,dsrlpi_ctl_used;


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION       RLPI_GET_REXMIT_DATA

DESCRIPTION    This function gets the information on the frame to be
               re-transmitted.
               
DEPENDENCIES   Since idle frames can only go on fundicated channels,
               the assumption is that only one idle frame can be sent
               per 20msec frame-time.
               
               rrcb_ptr->state.idle_timer is only decremented in this
               function.
               
               This function assumes RLP is in DSRLPI_ESTABLISHED_STATE
               or DSRLPI_RT_MEAS_STATE. This function should not be called
               if RLP cannot transmit new data frames.

RETURN VALUE   TRUE if a frame to rexmit has been found.  FALSE otherwise.

SIDE EFFECTS   NONE
===========================================================================*/
boolean rlpi_get_rexmit_data
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to ctl block            */
  dsrlptxq_fr_param_type       *fr_info,    /* struct passed to txq func   */
  rlpi_rexmit_info_type        *rexmit_info /* holds rexmit info           */
)
{
  uint32               modulus;             /* modulus for this rlp version*/
  uint32               nak_first_seq;       /* first seq of frames to rxmit*/ 
  uint32               nak_last_seq;        /* last seq of frames to rxmit */
  uint32               next_seq_num;        /* next seq num in the txq     */
  word                 data_payload_len;    /* length of frame's data      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - -*/
  /*-------------------------------------------------------------------------
    For code readability, use a local var for freq. accessed values.
  -------------------------------------------------------------------------*/
  modulus = rrcb_ptr->cfg_ptr->seq_modulus;

  /*-------------------------------------------------------------------------
    Get info from rexmit queue on what frame needs retransmitting.
  -------------------------------------------------------------------------*/
  if (dsrlprtx_get_rexmit_first_seq(rrcb_ptr, &nak_first_seq) == FALSE)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR," No frames to rexmit, but in rexmit func");
    return FALSE;
  }

  if (dsrlprtx_get_rexmit_last_seq(rrcb_ptr, &nak_last_seq) == FALSE)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, " bad last seq info in rtx queue");
    dsrlprtx_reset(rrcb_ptr);
    return FALSE;
  }

  do
  {
    /*-----------------------------------------------------------------------
      Set next_seq_num to a value that is impossible for the 32-bit internal
      seq_num representation.  This can be done by setting the top 4-bits
      and the bottom 4-bits to non-zero values.  This way, if next_seq_num
      is changed by dsrlptxq_access_frame, then it can be detected by
      next_seq_num conforming to the 32-bit internal seq. num format.
    -----------------------------------------------------------------------*/
    next_seq_num      = DSRLPSEQ_NO_SEQ;
    fr_info->seq_num  = nak_first_seq;
    data_payload_len  = dsrlptxq_access_frame(rrcb_ptr,
                                              fr_info,
                                              &next_seq_num);

    if (data_payload_len != 0)
    {
      break;            //frame has been found
    }
    else if (next_seq_num == DSRLPSEQ_NO_SEQ)
    {
      /*---------------------------------------------------------------------
        Since next_seq_num is unchanged, can deduce that txq is empty.
        Therefore, this function is done But first empty out rxmit info,
        since no data can be retransmitted.
      ---------------------------------------------------------------------*/
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Need to Rxmit, but txq is empty");
      dsrlprtx_reset(rrcb_ptr);
      return FALSE;
    }
    else
    {
      DATA_RLP_MSG1(MSG_LEGACY_MED, "Did not find first_seq 0x%x in txq",nak_first_seq);
      /*---------------------------------------------------------------------
        See if next_seq_num is in the first_seq -> last_seq range. If so,
        then update first_seq to next_seq_num, and re-get the txq info.
        Otherwise, delete this rexmit data, and get some new rexmit data.
      ---------------------------------------------------------------------*/
      if ((DSRLPSEQ_GE_SEQ_CHECK(nak_last_seq,
                                 next_seq_num,
                                 modulus ))    &&
          (DSRLPSEQ_GE_SEQ_CHECK(next_seq_num,
                                 nak_first_seq,
                                 modulus )))
      {
        nak_first_seq = next_seq_num;
        continue;                
      }
      else
      {
        DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Rxmit info no longer in txq");                   
        /*-------------------------------------------------------------------
          This retransmit info is not in the transmit array.  Remove this
          rexmit info, and try the next one.
        -------------------------------------------------------------------*/
        dsrlprtx_remove_first_element(rrcb_ptr);
        
        /*-------------------------------------------------------------------
          Get info from rexmit queue on what frame needs retransmitting.
        -------------------------------------------------------------------*/
        if (dsrlprtx_get_rexmit_first_seq(rrcb_ptr,&nak_first_seq) == FALSE)
        {
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, " No more rxmit info");
          return FALSE;
        }

        if (dsrlprtx_get_rexmit_last_seq(rrcb_ptr,&nak_last_seq) == FALSE)
        {
          DATA_RLP_MSG0(MSG_LEGACY_ERROR, " bad last seq info in rtx queue");
          dsrlprtx_reset(rrcb_ptr);
          return FALSE;
        }
      } /* get new rxmit info */
    } /* else txq not empty */
  }while (data_payload_len == 0);

  /*-------------------------------------------------------------------------
    If this spot has been reached, then a frame has been found successfully.
  -------------------------------------------------------------------------*/
  rexmit_info->nak_first_seq    = nak_first_seq;
  rexmit_info->nak_last_seq     = nak_last_seq;
  rexmit_info->data_payload_len = data_payload_len;

  return TRUE;  
} /* rlpi_get_rexmit_data() */
#endif /* FEATURE_DS_RLP3 */

/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION       DSRLPI_GET_BEST_FR_RATE

DESCRIPTION    This function returns the smallest available rate that a frame
               of the given length can be transmitted at. RLP conveys this
               rate to the Mux layer.
               
DEPENDENCIES   Assumes that non-NULL pointers are passed in.
               This will only work for MuxPDU type1 and MuxPDU type2.

RETURN VALUE   Returns the rate at which this frame should be sent at.

SIDE EFFECTS   NONE
===========================================================================*/
dsrlp_rate_enum_type dsrlpi_get_best_fr_rate
(
   dsrlp_rev_frame_type    *rev_fr_ptr,  /*  contains rev frame info.      */
   word                     fr_len       /* Num of bytes in frame          */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( (rev_fr_ptr != NULL) &&
          (fr_len != 0) &&
          (( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_1) || 
          ( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_2)) );
   
  /*-------------------------------------------------------------------------
    Go through all the possible rates, starting from the smallest and find
    which rate has a length greater than the passed length. The resultant 
    rate will be the one at which this frame needs to be sent.
    
    The exception is 1x, where the fr rate is mandatory full rate.
  -------------------------------------------------------------------------*/
  if (rev_fr_ptr->fr_rate_mask == DSRLP_RATE_1_MASK)
  {
    if (dsrlp_get_frame_len(DSRLP_RATE_1,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len)
    {
      return(DSRLP_RATE_1);
    }
    else
    {
      return( DSRLP_RATE_ERASURE);
    }
  }
  
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_1_16_MASK) &&
      (dsrlp_get_frame_len(DSRLP_RATE_1_16,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_1_16); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_1_8_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_1_8,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_1_8); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_3_16_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_3_16,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_3_16); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_1_4_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_1_4,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_1_4); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_3_8_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_3_8,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_3_8); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_7_16_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_7_16,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_7_16); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_1_2_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_1_2,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_1_2); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_3_4_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_3_4,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_3_4); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_7_8_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_7_8,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_7_8); 
  }
  if( (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_1_MASK) &&
      (dsrlp_get_frame_len( DSRLP_RATE_1,
                            rev_fr_ptr->srvc_type,
                            rev_fr_ptr->mux_pdu_type) >= fr_len))
  {
    return( DSRLP_RATE_1); 
  }
       
  /*-------------------------------------------------------------------------
    If the flow reaches this point, it means none of the available rates 
    had enough length to support the given frame. So, return erasure.
  -------------------------------------------------------------------------*/
  return( DSRLP_RATE_ERASURE);
  
} /* dsrlpi_get_best_fr_rate() */
#endif /* FEATURE_DS_RLP3 */


/*===========================================================================

FUNCTION DSRLPI_INIT_CONSTANTS

DESCRIPTION
  This function is called from dsrlp_init. All the version specific constants
  including function pointers are initialized here.  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsrlpi_init_constants
(
  dsrlpi_sess_ctl_blk_type  *rscb_ptr,       /* RLP-specific control block */
  dsrlp_ver_enum_type        ver             /* RLP version                */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Set the ver_ptr to the version specific constants for each version.
  -------------------------------------------------------------------------*/
  switch ( ver)
  {
#ifdef FEATURE_DS_RLP3
     case DSRLP_VER_1X_3:
       rscb_ptr->cfg.ver_ptr = &dsrlp3_constants;
       break;
#endif 

#ifdef FEATURE_HDR
     case DSRLP_VER_HDR:
       rscb_ptr->cfg.ver_ptr = get_hdrrlp_constants_ptr();
       break;
#endif 

#ifdef FEATURE_HDR_QOS
    case DSRLP_VER_HDR_MULTIFLOW_FORWARD:
    case DSRLP_VER_HDR_MULTIFLOW_REVERSE:
      rscb_ptr->cfg.ver_ptr = get_hdrmrlp_constants_ptr();
      break;
#endif /* FEATURE_HDR_QOS */

     default:
       DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid RLP Version %d",(int)ver);
       ASSERT( 0);
  }  /* switch on RLP version */
} /* dsrlpi_init_constants() */


/*===========================================================================

FUNCTION       DSRLPI_RESET

DESCRIPTION    Resets RLP. This involves clearing various values and
               resetting the RLP state machine.
               
               Note, stats.resets counter is incremented inside this 
               function. Because it only needs to be incremented for RLP 
               resets in the middle of a call (error conditions), we actually 
               set this counter back to zero after the very first call
               to this function from dsrlp_init().

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void dsrlpi_reset
(
  dsrlpi_rlp_ctl_blk_type        *rrcb_ptr   /* rlp-specific control block */
)
{
  uint8   i;      /* loop counter */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  if ( rrcb_ptr == NULL )
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid rrcb_ptr : %d", rrcb_ptr);
    return;     
  }

  rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));

  rrcb_ptr->state.reset_active = TRUE;

  /*-------------------------------------------------------------------------
    We do not clear out the config or stats information, as those are per
    call.  During a call, RLP can reset multiple times, and so clearing
    those structs would not be correct.
  -------------------------------------------------------------------------*/
  rrcb_ptr->stats.resets++;                          
  (void)time_get( rrcb_ptr->stats.last_rlp_reset_time) ;

#if defined (FEATURE_HDR) || defined(FEATURE_HDR_QOS)
  if ((rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR) ||
      (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) ||
      (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE))
  {

    /*-----------------------------------------------------------------------
      For HDR, start in the ESTABLISHED state.
    -----------------------------------------------------------------------*/
    rrcb_ptr->state.state             = DSRLPI_ESTABLISHED_STATE;
  }
  else
#endif
  /*-------------------------------------------------------------------------
    Reset the state info...this is kept only per RLP session. Don't free the 
    tx_item_ptr, which holds current item to be transmitted, or else some
    unsent data might be lost.
  -------------------------------------------------------------------------*/
  {
    rrcb_ptr->state.state             = DSRLPI_SYNC_STATE;

  /*-------------------------------------------------------------------------
    We don't want 1X to build frames while RLP reset happens as this may lead 
    to memory corruption issues. So we clear out frame parameters whenever
    RLP reset happens.
  -------------------------------------------------------------------------*/
    txc_rev_rlp_params_reset();
  }

  /*------------------------------------------------------------------------
    Clean up the resequencing  queue. Send data to upper layers till LV(N). 
    Needs to happen before V(N) is reset to zero.
  -------------------------------------------------------------------------*/
#if defined (FEATURE_HDR)
  if ((rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR) || 
      (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) ||
      (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE))
  {
    hdrrlprsq_reset(rrcb_ptr);
  } 
  else
#endif /* defined (FEATURE_HDR) */
  {
    dsrlprsq_reset(rrcb_ptr);
  }

  rrcb_ptr->state.ack_to_send    = FALSE;
  rrcb_ptr->state.expected_seq_num  = 0;
  rrcb_ptr->state.needed_seq_num    = 0;
  rrcb_ptr->state.peer_vn           = 0;
  rrcb_ptr->state.contig_erase      = 0;
  rrcb_ptr->state.rlp_delay         = 0;
  rrcb_ptr->state.hole_active       = FALSE;
  rrcb_ptr->state.saved_frames_index = 0;
  rrcb_ptr->state.saved_frames_cnt   = 0;
  rrcb_ptr->state.next_seq_num       = 0;
  rrcb_ptr->state.naks_to_send       = 0;
  rrcb_ptr->state.quick_repeat_naks_to_send = 0;

  rrcb_ptr->state.last_fill_nak_cnt  = 0;

  rrcb_ptr->state.mux_erasure_cnt    = 0;
  
  DATA_RLP_MSG2(MSG_LEGACY_MED, "In dsrlpi_reset, RLP state = %d "
                "and setting ack_to_send = %d",
                rrcb_ptr->state.state,
                rrcb_ptr->state.ack_to_send);
  
  /*------------------------------------------------------------------------
    DO_rev_A_RLP.
    Free Local RLP packet, and rset local_rlp_packet_ptr to NULL.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR
  for ( i=0; i< HDRCP_PCP_MAX_COMBOS;i++ )
  {
    hdrrlp_reset_new_data_local_state(&rrcb_ptr->state, i );
    rrcb_ptr->state.local_state[i].txed_flush_pkt = FALSE;
  }  

  /*---------------------------------------------------------------------- 
     reset any outstanding handles to pcp 
 ------------------------------------------------------------------------*/
  rrcb_ptr->state.combo_index = 0;
  hdrrlptxsh_reset(rrcb_ptr);
  hdrrlpmarq_reset(rrcb_ptr);
#endif /* FEATURE_HDR */

  rrcb_ptr->state.idle_timer_active  = FALSE;
  rrcb_ptr->state.idle_timer         = 0;
  rrcb_ptr->state.idle_fr_xmit_cntr  = 0;

  rrcb_ptr->state.rnd_trip_fr_cntr   = 0;

  rrcb_ptr->state.old_frames         = 0;
  rrcb_ptr->state.big_nak_gap        = FALSE;
  rrcb_ptr->state.reset_rlp          = FALSE;
  /*------------------------------------------------------------------------
    Clean up the nak list, transmitted queue, retxq for this RLP instance.
  -------------------------------------------------------------------------*/
#if defined (FEATURE_HDR)
  if ((rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR) || 
      (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) ||
      (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE))
  {
    hdrrlpnak_reset(rrcb_ptr);
    hdrrlptxq_reset(rrcb_ptr);
    hdrrlprtx_reset(rrcb_ptr);
  } 
  else
#endif /* defined (FEATURE_HDR) */
  {
    dsrlpnak_reset(rrcb_ptr);
    dsrlptxq_reset(rrcb_ptr);
    dsrlprtx_reset(rrcb_ptr);
  }

  /* Reset all active QN instances */
#if defined (FEATURE_HDR)
  for (i = 0; i < DSRLP_MAX_QN_INSTANCES_PER_RLP; i++)
  {
    if (rrcb_ptr->qn_instance_table[i].activated)
    {
      hdrrlpqn_reset(&rrcb_ptr->qn_instance_table[i]);
    }
  }

  /* Reset the buffered_rx_q and return all pkt_info_ptr to free_q */
  hdrrlpi_reset_rxbuffer(rrcb_ptr);
#endif

#if defined (FEATURE_HDR_REVB)
  hdrrlpdnak_reset(rrcb_ptr);
  hdrrlpmlnak_reset(rrcb_ptr);
#endif

  /*------------------------------------------------------------------------
    Clean up the delayed frame arrays
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DS_RLP3
  if( rrcb_ptr->cfg_ptr->ver == DSRLP_VER_1X_3)
  {
    dsrlpdel_reset(rrcb_ptr->state.del_fr_id);
  }
#endif /* FEATURE_DS_RLP3 */

#if defined (FEATURE_HDR)
  if ((rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR) ||
      (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_FORWARD) ||
      (rrcb_ptr->cfg_ptr->ver == DSRLP_VER_HDR_MULTIFLOW_REVERSE))
  {
    hdrrlp_reset( rrcb_ptr );
 
  }
#endif /* FEATURE_HDR */

  rrcb_ptr->state.reset_active = FALSE;

  rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
} /* dsrlpi_reset() */

#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION       DSRLPI_RX_INIT_FRAME

DESCRIPTION    This function handles incoming SYNC, SYNC/ACK, and ACK frames
               and increments/resets the RLP initialization state machine
               accordingly.

DEPENDENCIES   Assumes that the incoming frame is either a SYNC, SYNC/ACK,
               or ACK.

RETURN VALUE   Always returns success DSRLPI_DONE.

SIDE EFFECTS   Depending on the state and incoming frame, the round trip
               delay timer may be incremented.
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_rx_init_frame
(
  dsrlpi_rlp_ctl_blk_type   *rrcb_ptr,       /* rlp-specific control block */
  dsrlpi_curr_fr_info_type  *frame_info_ptr  /* current frame information  */
)
{
  dsrlpi_state_info_type    *state_info_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));

  state_info_ptr = &(rrcb_ptr->state);

  /*-----------------------------------------------------------------------
     Based on the frame category, we will update our SYNC state machine.  
     We don't care about sequence numbers during the SYNC stage.  The enum
     is actually a bitmask, and so we do a bit-compare to see what kind of
     control frame we rec'd.  The state transitions are listed below.
     The incrementing of the Round trip timer is not shown in the table.
     
                                 *Received Frame*
                               
    *State*     _SYNC_      _SYNC/ACK_           _ACK_            _DATA_
     
    _SYNC_        S/A           ACK                 X                X
     
    _S/A_          X            ACK               RT_MEAS           RESET
     
    _ACK_         RESET          X                RT_MEAS           ESTD
     
    _RT_MEAS_     RESET         RESET              X                ESTD
     
    _ESTD_        RESET         RESET             RESET              X
    
    
    Please note that this function only handles SYNCs, SYNC/ACKs, and
    ACKs.  Therefore, the rlp state is bumped into the ESTABLISHED state
    in some other function.
  -----------------------------------------------------------------------*/

  DATA_RLP_MSG2(MSG_LEGACY_MED, "Previous State = %d "
                "and ack_to_send = %d",
                state_info_ptr->state,
                state_info_ptr->ack_to_send);
  if ((frame_info_ptr->fr_cat & CURR_FRAME_SYNC_ACK_DETECT_MASK) 
                             == CURR_FRAME_SYNC_MASK)
  {
    switch(state_info_ptr->state)
    {
      case DSRLPI_SYNC_STATE:
        state_info_ptr->state = DSRLPI_SYNC_ACK_STATE;
        break;

      case DSRLPI_SYNC_ACK_STATE:
        break;

      case DSRLPI_ACK_STATE:
      case DSRLPI_RT_MEAS_STATE:
      case DSRLPI_ESTABLISHED_STATE:
        rrcb_ptr->state.reset_rlp = TRUE; 
        DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid SYNC, reset RLP, state=%d",
                      state_info_ptr->state);
        break;

      default:
        DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid RLP state %d",
                                 (int)state_info_ptr->state);
        ASSERT( 0);
    }
    DATA_RLP_MSG2(MSG_LEGACY_MED, 
                  "Moved to State = %d after getting a SYNC packet "
                  "and ack_to_send = %d",
                  state_info_ptr->state,
                  state_info_ptr->ack_to_send);
  }
  else if ((frame_info_ptr->fr_cat & CURR_FRAME_SYNC_ACK_DETECT_MASK)
            == (CURR_FRAME_SYNC_MASK | CURR_FRAME_ACK_MASK))
  {
    switch(state_info_ptr->state)
    {
      case DSRLPI_SYNC_STATE:
      case DSRLPI_SYNC_ACK_STATE:
        state_info_ptr->state = DSRLPI_ACK_STATE;
        state_info_ptr->ack_to_send = TRUE;
        state_info_ptr->rlp_delay++;
        break;

      case DSRLPI_ACK_STATE:
        state_info_ptr->rlp_delay++;
        break;

      case DSRLPI_RT_MEAS_STATE:
      case DSRLPI_ESTABLISHED_STATE:
        DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid SYNC/ACK, drop frame, "
                      "state=%d", state_info_ptr->state);
        break;

      default:
        ERR_FATAL( "Invalid RLP State", 0, 0, 0);
        break;
    }
    DATA_RLP_MSG2(MSG_LEGACY_MED, 
                  "Moved to State = %d after getting a SYNC/ACK packet "
                  "and ack_to_send = %d",
                  state_info_ptr->state,
                  state_info_ptr->ack_to_send);
  }
  else if ((frame_info_ptr->fr_cat & CURR_FRAME_SYNC_ACK_DETECT_MASK)
                                 ==  CURR_FRAME_ACK_MASK)
  {
    switch(state_info_ptr->state)
    {
      case DSRLPI_SYNC_STATE:
        break;

      case DSRLPI_SYNC_ACK_STATE:
      case DSRLPI_ACK_STATE:
        state_info_ptr->rlp_delay++;
        state_info_ptr->state = DSRLPI_RT_MEAS_STATE;
        break;

      case DSRLPI_RT_MEAS_STATE:
        state_info_ptr->rlp_delay++;
        break;

      case DSRLPI_ESTABLISHED_STATE:
        DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid ACK, drop frame, state=%d",
                      state_info_ptr->state);
        break;

      default:
        
        ERR_FATAL( "Invalid RLP State", 0, 0, 0);
        break;
    }
    DATA_RLP_MSG2(MSG_LEGACY_MED, 
                  "Moved to State = %d after getting an ACK packet "
                  "and ack_to_send = %d",
                  state_info_ptr->state,
                  state_info_ptr->ack_to_send);
  }
  else 
  {
    /*------------------------------------------------------------
      This function is only meant to handle SYNCS, SYNC/ACKs, and
      ACKs.  All other frame types are handled elsewhere.  Note
      that this means the state machine gets bumped into the 
      ESTABLISHED state elsewhere...
    ------------------------------------------------------------*/
    
    ASSERT(0);
  }

  rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));

  /*------------------------------------------------------------
     With idle processing, we are always done with the frame
     after one pass..
  ------------------------------------------------------------*/
  return DSRLPI_DONE;
} /* dsrlpi_rx_init_frame() */
#endif /* FEATURE_DS_RLP3 */

#ifndef MSM5000_IRAM_FWD
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION       DSRLPI_RX_NEW_DATA

DESCRIPTION    This function receives new data (segmented or unsegmented)
               and processes it.  If there is skew in the incoming seq
               number, then some Nak generation may occur.
               
               This code is same for all versions of RLP.
               
DEPENDENCIES   CRC checks done by frame determination.
               Input pointers are non-NULL.  
               The dsm_item embedded within fwd_fr_ptr contains only the RLP
                frame's data payload, and does not contain any header info
                or CRC info.

RETURN VALUE   Always returns DONE.

SIDE EFFECTS   V(R) and/or V(N) may get updated.  Entries may be inserted
               into the NAK list.  Entries may be added or removed from
               the resequencing buffer.
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_rx_new_data
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* Ptr to RLP ctl block       */
  dsm_item_type            **fr_item_pkt_ptr,
  dsrlpi_curr_fr_info_type *curr_info_ptr    /* frame info ptr             */
)
{
  uint32                       exp_seq;
  uint32                       need_seq;
  uint32                       rxed_seq;
  uint32                       new_seq_num;
  word                         data_len;
  uint32                       modulus;       /*     sequence modulus      */
  uint32                       nak_gap_seq;   /* v(R) + big_nak_gap        */
  uint32                       vr_seq_hi_plus;/* v(R) + 0xFF               */
  uint32                       vr_plus_diff;  /* v(R)_seq_hi_plus-rxed seq */
  dsm_item_type  *temp_item_ptr     = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT((rrcb_ptr != NULL) &&
         (curr_info_ptr != NULL));

  if (*fr_item_pkt_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "null dsm item ptr");
    return (DSRLPI_DONE);
  }

  exp_seq  = rrcb_ptr->state.expected_seq_num;
  need_seq = rrcb_ptr->state.needed_seq_num;
  rxed_seq = curr_info_ptr->seq_num;
  modulus  = rrcb_ptr->cfg_ptr->seq_modulus ;

  /*-------------------------------------------------------------------------
    Since this is a new data frame, the sequence number should be >= to 
    V(R).  It should not be less than V(R) (that would only make sense if
    this was a retransmission).
  -------------------------------------------------------------------------*/
  if (DSRLPSEQ_GT_SEQ_CHECK(exp_seq,rxed_seq,modulus))
  {
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "New seq 0x%x < Vr 0x%x Vn 0x%x",
             rxed_seq,
             exp_seq, 
             need_seq
            );
    dsm_free_packet(fr_item_pkt_ptr);
    rrcb_ptr->state.old_frames++;
    rrcb_ptr->state.big_nak_gap = FALSE;
    return (DSRLPI_DONE);
  }
  
  else if ((rrcb_ptr->state.nak_rounds_rev == 0) 
           || ((rxed_seq == exp_seq) && (exp_seq == need_seq)) )
  {
    rrcb_ptr->state.old_frames = 0;
    rrcb_ptr->state.big_nak_gap = FALSE;
    
    
    if( (rrcb_ptr->state.nak_rounds_rev == 0) && 
        !(curr_info_ptr->fr_cat & CURR_FRAME_IN_DELQ_MASK))
    {
       /*-------------------------------------------------------------------
          For nak_rounds zero, initially all the frames are placed 
          in the delayed queue. This loop is executed only once per 
          frame, so that next time we can continue with the rest 
          of the function, to make V(N) == V(R) == rxed_seq. 
        
          Note: Doing this way causes a delay of 20ms to all the frames 
          but this avoids major changes in the code structure.
        -------------------------------------------------------------------*/

        dsrlpdel_add( rrcb_ptr, *fr_item_pkt_ptr, curr_info_ptr);
        return( DSRLPI_DONE);
      }


    /*----------------------------------------------------------------------
      V(R) == V(N) == Rx'd seq #.
      
      Insert item onto Rx watermark, and then update V(R) and V(N).  Note
      that the code works for both segmented and unsegmented data. If
      unsegmented, then addition of the length is non-harmful, as last_seg 
      == TRUE, and so the sequence number will be inc'd up to the next SEQ.  
      If segmented, then adding the length is important, as this could
      result in the S_SEQ rolling over, and SEQ being incremented.  If the
      last_seg was also set, then SEQ would be incremented once more.
      
      After this addition, new_seq_num equals the seq_num of the last byte
      in the frame.
    ----------------------------------------------------------------------*/
    data_len    = (word)dsm_length_packet(*fr_item_pkt_ptr);

    if (data_len > 0)
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ( rxed_seq, 
                                        (data_len - 1),
                                        rrcb_ptr->cfg_ptr->seq_modulus );
    }
    else
    {
      new_seq_num = rxed_seq;
    }

    /*----------------------------------------------------------------------
      Increment V(R) now. If in byte sequenced mode (new frames have LST
      bit), increment SEQ, otherwise increment S_SEQ.
    ----------------------------------------------------------------------*/
    if (DSRLPI_HAS_LAST_SEG_MASK(curr_info_ptr->fr_cat))
    {
      new_seq_num = DSRLPSEQ_GET_NEXT_SEQ(new_seq_num,
                                        rrcb_ptr->cfg_ptr->seq_modulus);
    }
    else
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ( new_seq_num, 1, 
                                        rrcb_ptr->cfg_ptr->seq_modulus);
    }
    
    /*-----------------------------------------------------------------------
      put the item onto the rx queue, and call the callback.  Callback
      assumes non-chained items, so enqueue each item in the pkt list
      separately.
      
      However, may be possible still stuff in reseq queue, since not all 
      items are pushed up the stack at once.  make sure things go up to SIO
      in order.
    -----------------------------------------------------------------------*/
    if (DSRLPRSQ_CNT(rrcb_ptr) == 0)
    {
      do
      {
        temp_item_ptr          = *fr_item_pkt_ptr;
        *fr_item_pkt_ptr       = (*fr_item_pkt_ptr)->pkt_ptr;
        temp_item_ptr->pkt_ptr = NULL;

        dsm_enqueue(rrcb_ptr->cfg_ptr->rx_wm_ptr[rrcb_ptr->route], &temp_item_ptr);
      }while (*fr_item_pkt_ptr != NULL);

      rrcb_ptr->cfg_ptr->rx_func_ptr(rrcb_ptr->cfg_ptr->rx_data);
      
      /*---------------------------------------------------------------------
        Set the flag if a PPP packet is received first time after dormancy
      ---------------------------------------------------------------------*/
      rrcb_ptr->state.ppp_data_transfer_after_dormancy = TRUE;
      
      /*---------------------------------------------------------------------
        Update the statistic for Total Received Bytes only here. It also
        gets updated when data gets out of the re-sequencing queue.
      ---------------------------------------------------------------------*/
      rrcb_ptr->stats.rx_total_bytes  += data_len;
    }
    else
    {
      dsrlprsq_insert(rrcb_ptr,curr_info_ptr,*fr_item_pkt_ptr);
    }
  
    /*-----------------------------------------------------------------------
      Update the state seq num's.  This must be done after the potential
      insertion into the resequencing queue above.  Otherwise, if L_VN is
      updated to 'a+1', and try to insert 'a', then reseq queue will drop
      the frame.
    -----------------------------------------------------------------------*/
    rrcb_ptr->state.expected_seq_num = new_seq_num;
    rrcb_ptr->state.needed_seq_num   = new_seq_num;
  }
  else
  {
    rrcb_ptr->state.old_frames = 0;
    /*----------------------------------------------------------------------
      Either V(R) != V(N) and/or V(R) != Rx'd seq #.
      
      In both cases, this results in an entry into the resequencing queue,
      since the needed frame was not received.  To convince:
      
      V(R) == V(N), but V(R) < Rxed seq:  So a hole is inserted at V(N).
                                           Nak entry from V(R) to Rxed.
                                           V(R) is updated to Rxed seq + 1.
                                           
                                           
      V(R) != V(N), but V(R) == Rxed seq:  Existing hole still not filled.
                                           Update V(R) to Rxed seq + 1.
                                           
      V(R) != V(N), and V(R) < Rxed seq:  Existing hole still not filled,
                                           Nak entry from V(R) to Rxed.
                                           V(R) is updated to Rxed seq + 1.
                                           
      Know from first if conditional at top of function that rxed seq num
      is greater than or equal to V(R).                                     
    ----------------------------------------------------------------------*/
    if (DSM_LESS_THAN_LINK_LAYER_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL) 
          == TRUE)
    {
      /*-------------------------------------------------------------------
        Running out of items, so dump new frames which would cause
        holes.
      -------------------------------------------------------------------*/
      DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Too few small items, dump new frame");
      dsm_free_packet(fr_item_pkt_ptr);
      return (DSRLPI_DONE);
    }

    if (rxed_seq != exp_seq)
    {
      if( !(curr_info_ptr->fr_cat & CURR_FRAME_IN_DELQ_MASK))
      {
        /*-------------------------------------------------------------------
          This looks like it would add a hole, but first see if it is a case
          of delayed frames from base station.
        -------------------------------------------------------------------*/
        dsrlpdel_add( rrcb_ptr, *fr_item_pkt_ptr, curr_info_ptr);
        return( DSRLPI_DONE);
      }
      else
      {
        if (DSRLPSEQ_GT_SEQ_CHECK(rxed_seq,exp_seq,modulus))
        {
          /*-----------------------------------------------------------------
            See if the upcoming NAK gap is too big.
          -----------------------------------------------------------------*/
          nak_gap_seq = DSRLPSEQ_ADD_SEQ(rrcb_ptr->state.expected_seq_num,
                                         DSRLPI_BIG_NAK_GAP,
                                         modulus);

          /*-----------------------------------------------------------------
            If a frame is on the SCH, then it is not going to be garbled
            due to RDA.  however, if burst alignment is an issue, then it is
            possible that old SCH data is being read from the decoder.  For
            example, our L_V(R) = 0x6ab.  We read a frame from SCH decoder,
            with SEQ = 0xa8.  Guessing SEQ_HI, we come up with 0x7a8.  In
            actuality, it is an old frame.  We should drop it.  To guard 
            against legitimate frames, we require
            
             (L_V(R) + 0xff - rxed seq) < NAK_GAP.
             
             Note that the DIFF_SEQ requires vr_plus_diff > curr_info->seq.
             That value is used below, after an if() which include a check
             for SEQ_HI being received in the frame.  If SEQ_HI is not in
             the frame, then the rxed frame should be within 256 of our
             L_V(N) which is <= L_V(R).
          -----------------------------------------------------------------*/
          vr_seq_hi_plus = DSRLPSEQ_ADD_SEQ(rrcb_ptr->state.expected_seq_num, 
                                            0xFF, 
                                            modulus);

          vr_plus_diff   = DSRLPSEQ_DIFF_SEQ(vr_seq_hi_plus,
                                             curr_info_ptr->seq_num,
                                             modulus);

          vr_plus_diff   = ((vr_plus_diff >> 12) & 0x0FFF);

          if ((DSRLPSEQ_GE_SEQ_CHECK(curr_info_ptr->seq_num, nak_gap_seq, modulus)) &&
              (rrcb_ptr->state.big_nak_gap == FALSE)
             )
          {
            if (
                (curr_info_ptr->mux_pdu_type != DSRLP_MUX_PDU_1)        &&
                (curr_info_ptr->mux_pdu_type != DSRLP_MUX_PDU_2)        &&
                ((curr_info_ptr->fr_cat & CURR_FRAME_SEQ_HI_MASK) == 0) &&
                (vr_plus_diff < DSRLPI_BIG_NAK_GAP)
               )
            {   /* changed DATA_RLP_MSG2(MSG_LEGACY_HIGH to DATA_RLP_MSG2(MSG_LEGACY_ERROR to help testers. */
                DATA_RLP_MSG2(MSG_LEGACY_ERROR, "BURST GAP seq %x Vr %x",
                          curr_info_ptr->seq_num, 
                          rrcb_ptr->state.expected_seq_num
                        );
            }
            else
            {
              /*-------------------------------------------------------------
                This frame is dumped, but next one is going to be accepted
                regardless.
              -------------------------------------------------------------*/
              rrcb_ptr->state.big_nak_gap = TRUE;
                /* changed DATA_RLP_MSG2(MSG_LEGACY_HIGH to DATA_RLP_MSG2(MSG_LEGACY_ERROR to help testers. */
              DATA_RLP_MSG2(MSG_LEGACY_ERROR, "FCH RDA gap->seq %x Vr %x",
                        curr_info_ptr->seq_num,
                        rrcb_ptr->state.expected_seq_num
                      );
            }
            dsm_free_packet(fr_item_pkt_ptr);
            return( DSRLPI_DONE);
          }
        }
        /*-------------------------------------------------------------------
          This function was called from a delayed frame function, meaning
          that we are processing a delayed frame, and so should NOT put it
          back in the delayed frame arrays.
        -------------------------------------------------------------------*/
        curr_info_ptr->fr_cat &= ~CURR_FRAME_IN_DELQ_MASK;
        DATA_RLP_MSG3(MSG_LEGACY_MED, "hole-Rx=0x%x, V(R)=0x%x, V(N)=0x%x",
                 rxed_seq, exp_seq, need_seq);
      }
    }
    dsrlprsq_insert( rrcb_ptr, curr_info_ptr, *fr_item_pkt_ptr); 

    /*----------------------------------------------------------------------
      Lastly, update V(R) only.  V(N) remains unchanged.  
    ----------------------------------------------------------------------*/
    data_len    = (word)dsm_length_packet(*fr_item_pkt_ptr);

    if (data_len > 0)
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ(rxed_seq,
                                       (data_len - 1),
                                       rrcb_ptr->cfg_ptr->seq_modulus );
    }
    else
    {
      new_seq_num = rxed_seq;
    }

    /*----------------------------------------------------------------------
      Increment V(R) now. If in byte sequenced mode (new frames have LST
      bit), increment SEQ, otherwise increment S_SEQ.
    ----------------------------------------------------------------------*/
    if (DSRLPI_HAS_LAST_SEG_MASK(curr_info_ptr->fr_cat))
    {
      new_seq_num = DSRLPSEQ_GET_NEXT_SEQ(new_seq_num,
                                        rrcb_ptr->cfg_ptr->seq_modulus);
    }
    else
    {
      new_seq_num = DSRLPSEQ_ADD_S_SEQ( new_seq_num, 1, 
                                        rrcb_ptr->cfg_ptr->seq_modulus);
    }

    rrcb_ptr->state.expected_seq_num = new_seq_num;

    rrcb_ptr->state.big_nak_gap = FALSE;
    /*----------------------------------------------------------------------
      A NAK list entry may be required, if Rx'ed seq > V(R).  This is done
      last because if it fails, then a reset is done.
    ----------------------------------------------------------------------*/
    if (DSRLPSEQ_GT_SEQ_CHECK(rxed_seq,exp_seq,modulus))
    {
      if (dsrlpnak_insert(rrcb_ptr,exp_seq,rxed_seq) == FALSE)
      {
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Could not insert into NAK list");
        rrcb_ptr->state.reset_rlp = TRUE;
      }
    }
  }

  return (DSRLPI_DONE);
} /* dsrlpi_rx_new_data() */
#endif /* FEATURE_DS_RLP3 */
#endif /* MSM5000_IRAM_FWD */
     
#ifndef MSM5000_IRAM_REV
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION       DSRLPI_PROCESS_REXMIT_FRAME

DESCRIPTION    This function will process re-tansmitted frames. It will 
               first remove them from the nak list and then insert into the
               resequencing queue. After that any frames that can be passed 
               to higher layers will be moved from the resequencing q into 
               the rx watermark structure.
              
DEPENDENCIES   Non-null pointers passed into this function

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
dsrlpi_fr_det_enum_type dsrlpi_process_rexmit_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  dsrlp_fwd_frame_type         *fwd_fr_ptr, /* ptr to received RLP frame   */
  dsrlpi_curr_fr_info_type     *fr_info_ptr /* current frame info          */
)
{
  word           fr_len    = 0;             /* Length of rexmit fr payload */
  dsm_item_type *fr_ptr    = NULL;          /* the rx data frame           */
  uint32                   nak_id;          /* ID to reference NAK         */
  uint32                   end_seq;         /* for calculating fr end seq  */
  uint32                   need_seq;        /* local copy of V(N)          */
  uint32                   first_seq;       /* received seq number         */
  uint32                   modulus;         /* modulus                     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(fwd_fr_ptr  != NULL); 

  if (fwd_fr_ptr->fr_item_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Null ptr to rexmit processing");
    return (DSRLPI_DONE);
  }
 
  fr_ptr = fwd_fr_ptr->fr_item_ptr;
  ASSERT((rrcb_ptr    != NULL) &&
         (fr_info_ptr != NULL) &&
         (fr_ptr      != NULL));
  
 
  /*-----------------------------------------------------------------------
      get the length of the frame in data bytes
  -----------------------------------------------------------------------*/
  fr_len = (word)dsm_length_packet(fr_ptr);
    
  /*-----------------------------------------------------------------------
    Check if there exists an entry for this sequence number in the NAK list
    and get the SEQ_HI for the rexmit frame.
    If a NAK list entry exists, insert into resequencing queue, remove from
    NAK list and then remove from resequencing queue.
    
    Note that dsrlpnak_remove updates V(N), and so the dsrlprsq_remove
    MUST happen AFTER the dsrlpnak_remove.

    Enter into critical section before getting the NAK id, as we can have a 
    stale nak_id which was already deleted as part of NAK abort processing
    for tuneaway gap.
  -----------------------------------------------------------------------*/
  rex_enter_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
  if( dsrlpnak_get_seq_hi( rrcb_ptr, fr_info_ptr, fr_len, &nak_id)
      == TRUE)
  {
    if (DSM_LESS_THAN_LINK_LAYER_FEW_FREE_ITEMS(DSM_DS_SMALL_ITEM_POOL) 
          == TRUE)
    {
      /*-------------------------------------------------------------------
        If the rexmitted frame does not include V(N), then dump frame,
        as the system is running out of small items.
      -------------------------------------------------------------------*/
      modulus   = rrcb_ptr->cfg_ptr->seq_modulus ;
      need_seq  = rrcb_ptr->state.needed_seq_num;
      first_seq = fr_info_ptr->seq_num;
      end_seq   = DSRLPSEQ_ADD_S_SEQ(first_seq,
                                     fr_len,
                                     rrcb_ptr->cfg_ptr->seq_modulus);
      if (DSRLPSEQ_GT_SEQ_CHECK(first_seq,need_seq,modulus) ||
          DSRLPSEQ_GT_SEQ_CHECK(need_seq,end_seq,modulus))
      {
        DATA_RLP_MSG0(MSG_LEGACY_HIGH, " Few small items, rexmit not first hole");
        dsm_free_packet( &(fwd_fr_ptr->fr_item_ptr));
        rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
        return (DSRLPI_DONE);
      }
    }

    /*-------------------------------------------------------------------
       Either there is no memory shortages in the phone, or if there is,
       then the rexmitted frame will fill the first hole.
    -------------------------------------------------------------------*/
    dsrlprsq_insert(rrcb_ptr, fr_info_ptr, fr_ptr);
    if (dsrlpnak_remove(rrcb_ptr, fr_info_ptr, fr_len, nak_id) == FALSE)
    {
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "nak remove failed, so reset");
      rrcb_ptr->state.reset_rlp = TRUE;
    }
  }
  else
  {
    DATA_RLP_MSG3(MSG_LEGACY_MED, " Got dup rxit Rx=%x, V(R)=0x%x, V(N)=%x", 
              fr_info_ptr->seq_num,
              rrcb_ptr->state.expected_seq_num,
              rrcb_ptr->state.needed_seq_num);
    dsm_free_packet( &(fwd_fr_ptr->fr_item_ptr));
  }

  rex_leave_crit_sect(&(rrcb_ptr->state.data_path_crit_sect));
  return (DSRLPI_DONE);
}/* dsrlpi_process_rexmit_frame() */
#endif /* FEATURE_DS_RLP3 */
#endif /* MSM5000_IRAM_FWD */

#ifndef MSM5000_IRAM_REV
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION       DSRLPI_TX_NEW_DATA_FRAME

DESCRIPTION    This function constructs an new data frame from payload 
               available in the transmit watermark.
               
               Only non segmented new frames are constructed.
               
               This code is specific to RLP 1, RLP2 and RLP3.
               
DEPENDENCIES   Since idle frames can only go on fundicated channels,
               the assumption is that only one idle frame can be sent
               per 20msec frame-time.
               
               rrcb_ptr->state.idle_timer is only decremented in this
               function.
               
               This function assumes RLP is in DSRLPI_ESTABLISHED_STATE
               or DSRLPI_RT_MEAS_STATE. This function should not be called
               if RLP cannot transmit new data frames.

RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               Returning FALSE in this case implies a Blank frame should
               be supplied to the MUX layer.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_tx_new_data_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  dsrlp_rev_frame_type         *rev_fr_ptr  /* Ptr to frame to be built    */
)
{
  dsrlpi_state_info_type *state_ptr;        /* local state ptr             */
  boolean tx_seq_hi           = FALSE;      /* Is SEQ_HI transmitted?      */
  boolean tx_s_seq            = FALSE;      /* Is S_SEQ transmitted?       */
  byte    seq_hi              = 0;          /* SEQ_HI of frame to be sent  */
  uint16  s_seq               = 0;          /* SEQ_HI of frame to be sent  */
  boolean ctl_len_present     = FALSE;      /* If len field is sent in fr. */
  uint32  payload_in_frame    = 0;          /* # payload bytes in new frame*/
  uint32  payload_in_wm       = 0;          /* # payload bytes in txmit wm */
  dsm_item_type *new_fr_item_ptr= NULL;     /* dsm item to put new fr. in  */
  dsm_item_type *head_item_ptr= NULL;       /* dsm item to put new fr. in  */
  dsm_item_type *tail_item_ptr= NULL;       /* dsm item to put new fr. in  */
  byte    num_ctl_bits        = 0;          /* To keep track of bits used  */
  dsm_watermark_type *tx_wm_ptr;            /* Output watermark for txmit  */
  byte   *ctl_field_ptr       = NULL;       /* Ptr to ctl len field        */
  word    tx_item_len         = 0;          /* len of tx_item_ptr          */
  word    bytes_to_copy       = 0;          /* Num bytes to copy into frame*/
  word    frame_len           = 0;          /* Num bytes in the output fr. */
  dsrlptxq_fr_param_type      txq_fr_param; /* Info. of fr. to be inserted */
  uint32  seq_num_shift,peer_num_shift;     /* to help calc if seq_hi needs*/
                                            /* to be sent                  */
  
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
  byte     mux_len_bits = 0 ;               /* Used to tell the number
                                               of bits used for length     */   
  byte     rlp_hdr_bytes = 0;               /* RLP header bytes            */
  boolean mux_pdu5_fmt_d = FALSE;
#endif /* FEATURE_IS2000_REL_A_MUXPDU5 */
  byte    ctl_offset = 0;                   /* The bit offset at which 
                                               control bits are loaded.    */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Initializing variables that will be used later in the function.
    Set free_dsm_pkt to FALSE, so that the mux layer will not free this frame
    because it will be stored in the transmit queue.
  -------------------------------------------------------------------------*/
  state_ptr           = &rrcb_ptr->state;
  tx_wm_ptr           = rrcb_ptr->cfg_ptr->tx_wm_ptr;
  txq_fr_param.header = 0;
  txq_fr_param.footer = rev_fr_ptr->max_frame_size;

  /*-------------------------------------------------------------------------
    Find out if byte-sequenced and if there is rexmit ambiguity.  This will
    help in figuring out the min frame size needed to xmit the data, as well
    as help in later actions.
  
    If V( S) - Peer-V( N) > 255, include SEQ_HI.
  -------------------------------------------------------------------------*/
  seq_num_shift = DSRLPSEQ_GET_12BIT_FRAME_SEQ(state_ptr->next_seq_num);
  peer_num_shift = DSRLPSEQ_GET_12BIT_FRAME_SEQ(rrcb_ptr->state.peer_vn);

  if (((seq_num_shift + 4096 - peer_num_shift) % 4096) > 255)
  {
    tx_seq_hi = TRUE;
  }

  /*-------------------------------------------------------------------------
    If RLP has been configured for byte sequenced operation, both SEQ and 
    SEQ_HI will always be included in data frams.
  -------------------------------------------------------------------------*/
  if ( rrcb_ptr->cfg_ptr->octet_sequenced == TRUE)
  {
    tx_s_seq  = TRUE;
    tx_seq_hi = TRUE;
  }

  /*-------------------------------------------------------------------------
    See if there is a potential issue with the size of the frame.  Account
    for the SCH/FCH (via the PDU), and if need to embed the s_seq or seq_hi.
  -------------------------------------------------------------------------*/
  if (((rev_fr_ptr->max_frame_size < RLPI_THRESH_BYTES_IN_DATA_FRAME) &&
       ((rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_3)  || 
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
        (rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_5)  || 
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
        (rrcb_ptr->cfg_ptr->octet_sequenced == TRUE)        ||
        (tx_s_seq == TRUE)                             ||
        (tx_seq_hi == TRUE)
       )
      )                                                               ||
      (rev_fr_ptr->max_frame_size < RLPI_FCH_UNSEG_DATA_MIN_SIZE)
     )
  {
    DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Can't make data frame size=%d", 
              rev_fr_ptr->max_frame_size);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    If the current tx_item_ptr is NULL, get a new dsm packet from the txmit
    watermark. There should be something in the watermark, otherwise this
    function should not have been called.
  -------------------------------------------------------------------------*/
  if ( state_ptr->tx_item_ptr == NULL)
  {
    state_ptr->tx_item_ptr = dsm_dequeue( tx_wm_ptr);
    ASSERT( state_ptr->tx_item_ptr != NULL);
    tx_item_len = (word)dsm_length_packet( state_ptr->tx_item_ptr);
    rrcb_ptr->stats.tx_total_bytes +=  tx_item_len;
  }
  else
  {
    tx_item_len = (word)dsm_length_packet( state_ptr->tx_item_ptr);
  }
    
  
  /*-------------------------------------------------------------------------
    Get a new item to start building the frame in. Ask for about 1/4th of the
    max. size allowed by the mux layer. For MuxPDUs1,2 and 3 this should give
    a small item.
    On success, set the data field of the item to all 0s.
  -------------------------------------------------------------------------*/
  new_fr_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
                                       
  if ( new_fr_item_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Memory allocation failed");
    return FALSE;
  }
  tail_item_ptr = head_item_ptr = new_fr_item_ptr;
  memset( new_fr_item_ptr->data_ptr, 0, new_fr_item_ptr->size);


  /*-------------------------------------------------------------------------
    payload_in_frame is the number of available bytes to put in RLP data and
    headers.
    Note, max_frame_size does not include the byte containing the type bits.
    So, max_frame_size passed by the Mux Layer is 43 for MuxPDU3 and 21 for
    full rate MuxPDU1.  1 is subtracted from max_frame_size to get payload_in
    _frame to account for the SEQ added at the beginning of every frame.
    payload_in_wm is the number of bytes available in the transmit
    watermark to be sent out in RLP. For MuxPDU5 the payload size varies for
    each rate.
  -------------------------------------------------------------------------*/
  payload_in_frame  = rev_fr_ptr->max_frame_size - 1;
  payload_in_wm     = tx_item_len + tx_wm_ptr->current_cnt;

/*-------------------------------------------------------------------------
    For MuxPDU3, add the sr_id to the 1st byte. The type bits will be added
    in later.
    The footer of the frame is increased by 1 to account for the extra byte.
    At present, the footer contains all the bytes in the frame. Later in 
    the function, the header and payload will be subtracted to get the real
    footer count.
  -------------------------------------------------------------------------*/
  if (rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_3)
  {
    new_fr_item_ptr->data_ptr[ new_fr_item_ptr->used++]= rrcb_ptr->cfg_ptr->sr_id << 5;    
    txq_fr_param.footer ++;
  }
 
  /*-------------------------------------------------------------------------
    For MuxPDU5, add the sr_id to the 1st byte. If the frame is using full
    payload then we use format C frame type(same as PDU3).
    If the payload is filled partially then we use format-D frame type 
    in which the Lenght Indicator tells us about the Lenght(bits) of the frame.
    Lenght Indicator       Length(bits)  Format
     00                     0             C
     01                     8             D
     10                     16            D
    The type bits will be added in later.
    The footer of the frame is increased by mux header bits to account 
    for the extra bytes.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
  else if( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_5 ) 
  {
    new_fr_item_ptr->used = 0;
    /*-----------------------------------------------------------------------
     Until now we have these fields with these values
     new_fr_item_ptr->used =0;
     txq_fr_param.header = 0
     txq_fr_param.footer = MaxFrameSize
     payload_in_frame = MuxPayload = MaxFrameSize-1;
     payload_in_wm    = PayloadInWm
     ------------------------------------------------------------------------*/
    /*-----------------------------------------------------------------------
       Build partial MuxHDR: Add Sr_ID at item[0] location. Since we changed the
       MuxHDR we should increase footer only now.
     ------------------------------------------------------------------------*/
    new_fr_item_ptr->data_ptr[new_fr_item_ptr->used ] = rrcb_ptr->cfg_ptr->sr_id << 5;
    txq_fr_param.footer += 1;
    
    if( payload_in_wm >= payload_in_frame )
    { 
     /*-----------------------------------------------------------------------
      LenIndicator is 00 already.
      Type bits may change later.
     ------------------------------------------------------------------------*/
      new_fr_item_ptr->used += 1;
         
    }

     /*-----------------------------------------------------------------------
        For MaxFrameSize < 0xFF , Length Ind is 01. (1byte)
        and for MaxFrameSize < 0xFFFF , Length Ind is 10. (2bytes)
     ------------------------------------------------------------------------*/
    else
    {
	  /*lint -save -e831*/
      if( payload_in_frame < 0xFF)
      {
         new_fr_item_ptr->data_ptr[new_fr_item_ptr->used ] |= 0x04;
		 
         mux_len_bits  = 8 ;
		 
      }
      else if( payload_in_frame < 0xFFFF)
      {
        new_fr_item_ptr->data_ptr[new_fr_item_ptr->used ] |= 0x08;
        mux_len_bits = 16;
      }
      else
      {
         DATA_RLP_MSG1(MSG_LEGACY_FATAL, "payload_in_frame : %ld:Feature unsupported", 
                    payload_in_frame);
         return FALSE;
      }
      /*lint -restore Restore lint error 831*/
	  
      /*---------------------------------------------------------------------
        ---------------------   
       |   Mux Payload        |
        ----------------------
        ---------------------------
       | RLP Header |RLP Payload  |
       ---------------------------
     ----------------------------------------------------------------------*/
     /*---------------------------------------------------------------------
        Calculate RLP Header based on Seq,PDU5_FmtD_Ctl_Len, Seq_hi, S_Seq,
        Padding.
     ----------------------------------------------------------------------*/
     rlp_hdr_bytes = ( RLP3_SEQ_LEN            
                      + RLP3_FMT_D_PDU5_CTL_LEN
                      + ((tx_seq_hi) ? (RLP3_FMT_D_SEQ_HI_LEN) : 0)  
                      + ((tx_s_seq) ? (RLP3_FMT_D_S_SEQ_LEN) : 0 )  
                      + 7)                    /* Padding */
                      >> 3;                   /* Bits to bytes conversion. */
      mux_pdu5_fmt_d = TRUE;
      /*sr_id is already accounted for it */
      payload_in_frame = (payload_in_frame) - (((mux_len_bits+7) >> 3) + rlp_hdr_bytes);
      payload_in_frame = MIN( payload_in_frame, payload_in_wm);

	  /*lint -save -e774*/
	  /*lint -save -e831*/
      if(mux_len_bits != 0)
      {
        b_packw(  (word)(payload_in_frame +rlp_hdr_bytes), 
                   &new_fr_item_ptr->data_ptr[new_fr_item_ptr->used ], 
                   RLP3_FMT_D_PDU5_LEN_IND_POS, 
                   mux_len_bits);
        /* +1 to account for typebit */
        new_fr_item_ptr->used += ((mux_len_bits >> 3)+ 1);
      }
	  /*lint -restore Restore lint error 831*/
      /*lint -restore Restore lint error 774*/
    } /*PDU5 format D frame*/
  }

#endif
 
  /*-------------------------------------------------------------------------
    Add the 8bit sequence number which goes in each MuxPDU.
  -------------------------------------------------------------------------*/
  new_fr_item_ptr->data_ptr[new_fr_item_ptr->used++]= 
                      DSRLPSEQ_GET_SEQ( state_ptr->next_seq_num);

  txq_fr_param.seq_num = state_ptr->next_seq_num;

  /*-------------------------------------------------------------------------
    Check if the ctl-len fields need to be added to the frame. In general
    the ctl-len fields need to be added to all frames except Format B and
    Format C frame.
  -------------------------------------------------------------------------*/
  if ( ( payload_in_wm < payload_in_frame)              || 
       !( rev_fr_ptr->fr_rate_mask & DSRLP_RATE_1_MASK) ||
       (tx_s_seq== TRUE)                                || 
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
       (mux_pdu5_fmt_d)                                 ||
#endif
       (tx_seq_hi==TRUE))
   {
    ctl_field_ptr   = new_fr_item_ptr->data_ptr + new_fr_item_ptr->used;
    ctl_len_present = TRUE;

    /*-----------------------------------------------------------------------
      For MuxPDU1 and 2, determine if a segmented or a unsegmented frame 
      needs to be sent out.
    -----------------------------------------------------------------------*/
    if( ( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_1) || 
        ( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_2))
    {
      /*---------------------------------------------------------------------
        If either SEQ_HI or S_SEQ were to be included, then a segmented frame
        needs to be sent.
      ---------------------------------------------------------------------*/
      if ( (tx_seq_hi==TRUE) || (tx_s_seq==TRUE))
      {
        (*ctl_field_ptr)   |= RLP3_SEG_DATA_FLD_CTL;  /* 0x80 */
        num_ctl_bits = 12;

        /*-------------------------------------------------------------------
          If SEQ_HI needs to included in the new frame, get the SEQ_HI of 
          V( S) and put it in the 4bits for SEQ_HI in the segmented frame.
        -------------------------------------------------------------------*/
        if ( tx_seq_hi==TRUE)
        {
          (*ctl_field_ptr)   |= RLP3_SEG_DATA_SQI_MASK; /* 0x08 */

          /*-----------------------------------------------------------------
            If RLP is not transmitting the s_seq, it is in frame sequenced
            mode, so it needs to set the LST bit.
            Note, the LST bit is set to 1 for frame sequenced mode and set
            to 0 for octet-sequenced.
          -----------------------------------------------------------------*/
          if ( tx_s_seq == FALSE)
          {
            (*ctl_field_ptr) |= RLP3_SEG_DATA_LST_MASK; /* 0x04 */
          }

          seq_hi= (byte) DSRLPSEQ_GET_SEQ_HI((state_ptr->next_seq_num));
          b_packb( seq_hi, ctl_field_ptr, 12, 4);
          new_fr_item_ptr->used ++;
          num_ctl_bits += 4;
        }

        /*-------------------------------------------------------------------
          If S_SEQ needs to be sent for new frames, this means RLP is doing
          byte oriented operation. Get the S_SEQ of V(S) and put into frame.
          Note, LST bit is not set to 1 when transmiting new segmented data
          frames.
        -------------------------------------------------------------------*/
        if ( tx_s_seq==TRUE)
        {
          s_seq = (uint16) DSRLPSEQ_GET_S_SEQ((state_ptr->next_seq_num));
          b_packw( s_seq, ctl_field_ptr, num_ctl_bits, 12);
        }

        /*-------------------------------------------------------------------
          Calculate the LEN field of the segmented frame. payload_in_frame
          will return the number of payload bytes to be copied.
        -------------------------------------------------------------------*/
        new_fr_item_ptr->used += 3;
        payload_in_frame       = rev_fr_ptr->max_frame_size 
                               - new_fr_item_ptr->used;
        payload_in_frame       = MIN( payload_in_frame, payload_in_wm);
        ASSERT( payload_in_frame < 32);
        b_packw( (word)payload_in_frame, ctl_field_ptr, 7, 5);
      }

      /*---------------------------------------------------------------------
        This is a new unsegmented data frame (common case).
      ---------------------------------------------------------------------*/
      else
      {
        /*-------------------------------------------------------------------
          Calculate the LEN field of the unsegmented frame. payload_in_frame
          will return the number of payload bytes to be copied.
        -------------------------------------------------------------------*/
        new_fr_item_ptr->used += 1;
        payload_in_frame       = rev_fr_ptr->max_frame_size 
                               - new_fr_item_ptr->used;
        payload_in_frame       = MIN( payload_in_frame, payload_in_wm);
        ASSERT( payload_in_frame < 64);
        b_packw( (word)payload_in_frame, ctl_field_ptr, 2, 6);
      }
    }

    /*-----------------------------------------------------------------------
      For MuxPDU3/5, frames with ctl-len fields are Format D
    -----------------------------------------------------------------------*/
    else  /* MuxPDU type 3 and type 5 - format D*/
    {
      num_ctl_bits = 0;

      /*-------------------------------------------------------------------
        Along with the 4bit control flags,
        for PDU3, the lenght field(8bits) is present in the rlp frame,
        whereas for PDU5 it is present in the mux header. 
        -------------------------------------------------------------------*/
      if ( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_3 )
      {
        ctl_offset = 12;
      }
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
      else if ( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_5 )
      {
        ctl_offset = 4;
      }
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
      else
      {
        ERR_FATAL( "Invalid MUX PDU path", 0, 0, 0 );
      }


      /*-------------------------------------------------------------------
        If SEQ_HI needs to included in the new frame, get the SEQ_HI of 
        V( S) and put it in the 4bits for SEQ_HI in the Format D frame.
      -------------------------------------------------------------------*/
      
      if ( tx_seq_hi==TRUE)
      {
        (*ctl_field_ptr) |= RLP3_FMT_D_DATA_SQI_MASK;    /*0x40 */

        seq_hi= (byte) DSRLPSEQ_GET_SEQ_HI((state_ptr->next_seq_num));
        b_packb( seq_hi, ctl_field_ptr, ctl_offset, RLP3_FMT_D_SEQ_HI_LEN);
        num_ctl_bits +=  RLP3_FMT_D_SEQ_HI_LEN;
      }

      /*-------------------------------------------------------------------
        If S_SEQ needs to be sent for new frames, this means RLP is doing
        byte oriented operation. Get the S_SEQ of V(S) and put into frame.
        Note, LST bit is not set to 1 when transmiting new Format D data
        frames.
      -------------------------------------------------------------------*/
      if ( tx_s_seq==TRUE)
      {
        (*ctl_field_ptr) |= RLP3_FMT_D_DATA_SSP_MASK;    /*0x80 */

        s_seq = (uint16) DSRLPSEQ_GET_S_SEQ((state_ptr->next_seq_num));
        b_packw( s_seq, ctl_field_ptr, ctl_offset+num_ctl_bits,
                  RLP3_FMT_D_S_SEQ_LEN);
        num_ctl_bits += RLP3_FMT_D_S_SEQ_LEN;
      }
      
      /*-------------------------------------------------------------------
        For MuxPDU3, include the length field. The field is calculated 
        based on how many bits have been used up.
        The length field is written into the 8bit LEN.
      -------------------------------------------------------------------*/
      if ( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_3)   
      {
        new_fr_item_ptr->used += ( 2 + (num_ctl_bits >> 3));
        payload_in_frame       = rev_fr_ptr->max_frame_size
                               - new_fr_item_ptr->used; 
        payload_in_frame       = MIN( payload_in_frame, payload_in_wm);
        ASSERT( payload_in_frame < 256);
        b_packw( (word)payload_in_frame, 
                 ctl_field_ptr, 4, 8);
      }
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
      else if( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_5 ) 
      {
        /*----------------------------------------------------------------
         Add 4 bits of control flags and account for padding making byte
         boundary.
         ----------------------------------------------------------------*/
         new_fr_item_ptr->used += ((num_ctl_bits + RLP3_FMT_D_PDU5_CTL_LEN 
                                   + 7) >> 3);

         /*----------------------------------------------------------------
           Adjust the payload_in_frame for the copying below. 
           For full frames we have to reduce the Payload_in_frame if we
           add any control bits.
         ------------------------------------------------------------------*/
         if (mux_len_bits == 0) 
         {
             payload_in_frame -=((num_ctl_bits + RLP3_FMT_D_PDU5_CTL_LEN + 7) >> 3);         
         }
         payload_in_frame       = MIN( payload_in_frame, payload_in_wm);
      }
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/

    }  /* Format D */

  }  /* if need ctl-len fields */


  /*-------------------------------------------------------------------------
    Update V(S). If this was a frame increment the SEQ by 1, otherwise add in
    the S_SEQ.
  -------------------------------------------------------------------------*/
  if ( tx_s_seq == TRUE)
  {
    state_ptr->next_seq_num = 
                      DSRLPSEQ_ADD_S_SEQ( (state_ptr->next_seq_num), 
                                          payload_in_frame,
                                          (rrcb_ptr->cfg_ptr->seq_modulus));
  }
  else
  {
    state_ptr->next_seq_num = 
                    DSRLPSEQ_INCREMENT_SEQ( state_ptr->next_seq_num,
                                        rrcb_ptr->cfg_ptr->seq_modulus);
  }

  /*-------------------------------------------------------------------------
    Set the overhead header bytes to the size of the frame that has been 
    built till now. (only header bytes have been filled).
    Footer bytes are set to the max allowed bytes minus (payload +header).
    Fill in payload from the txmit watermark into the new frame. payload_in_
    frame is the number of bytes to be copied.
  -------------------------------------------------------------------------*/
  txq_fr_param.header  = (byte)new_fr_item_ptr->used;
  txq_fr_param.footer -= (word)( payload_in_frame + txq_fr_param.header);
  
  while( payload_in_frame >0)
  {
    /*-----------------------------------------------------------------------
      Get a new item for the RLP frame if the old one is all used up
    -----------------------------------------------------------------------*/
    if ( new_fr_item_ptr->size == new_fr_item_ptr->used)
    {
      new_fr_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL);
                                        
      if ( new_fr_item_ptr == NULL)
      {
        DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Memory allocation failed");
        dsm_free_packet( &head_item_ptr);
        return FALSE;
      }
      tail_item_ptr->pkt_ptr = new_fr_item_ptr;
      tail_item_ptr          = new_fr_item_ptr;
      memset( new_fr_item_ptr->data_ptr, 0, new_fr_item_ptr->size);
    } /* if need more new items */

    /*-----------------------------------------------------------------------
      Pull up bytes from the tx_item_ptr. Number of bytes pulled up is the 
      minimum of the payload space, length in tx_item_ptr and available
      room in the new_fr_item_ptr.
    -----------------------------------------------------------------------*/
    bytes_to_copy = (word)MIN( payload_in_frame, tx_item_len);
    ASSERT( new_fr_item_ptr->size >= new_fr_item_ptr->used );
    bytes_to_copy = MIN( bytes_to_copy, 
                          (new_fr_item_ptr->size - (new_fr_item_ptr->used )));

    (void) dsm_pullup( & (state_ptr->tx_item_ptr), 
                (new_fr_item_ptr->data_ptr+ new_fr_item_ptr->used), 
                bytes_to_copy);
    payload_in_frame      -= bytes_to_copy;
    new_fr_item_ptr->used += bytes_to_copy;   
    ASSERT( new_fr_item_ptr->size >= new_fr_item_ptr->used);

    /*-----------------------------------------------------------------------
      If all the bytes from tx_item_ptr have been copied into the new frame,
      get a new item from the transmit watermark.
    -----------------------------------------------------------------------*/
    if ( state_ptr->tx_item_ptr == NULL)
    {
      state_ptr->tx_item_ptr = dsm_dequeue( tx_wm_ptr);
      if ( state_ptr->tx_item_ptr == NULL)
      {
        break;
      } 
      else
      {
        tx_item_len = (word)dsm_length_packet( state_ptr->tx_item_ptr);
        rrcb_ptr->stats.tx_total_bytes += tx_item_len;
      }
    }
    else /* tx_item_ptr != NULL */
    {
      tx_item_len = (word)dsm_length_packet( state_ptr->tx_item_ptr);

      /*---------------------------------------------------------------------
        It was observed (with HDR USB endpoints) that 0 byte items can be 
        queued onto the tx watermark.
        If the size of the item is 0, dsm_pullup above does not free it - so
        we need to explicitly free the item.
        If we do not, then the while loop will continue forever because 
        payload_in_frame does not decrement if tx_item_len is 0.
      ---------------------------------------------------------------------*/
      if( tx_item_len == 0)
      {
        DATA_RLP_MSG2(MSG_LEGACY_ERROR, "Zero len item curr=%d dont_ex=%d",
                      tx_wm_ptr->current_cnt,
                      tx_wm_ptr->dont_exceed_cnt);
        dsm_free_packet( &(state_ptr->tx_item_ptr));
      }
    }  /* end if tx_item_ptr == NULL */

  } /* while copying bytes */

  if ( payload_in_frame > 0)
  {
    DATA_RLP_MSG1(MSG_LEGACY_ERROR,
                  "%d bytes missing from watermark on making RLP frame",
                  payload_in_frame);
    dsm_free_packet( &head_item_ptr);
    return( FALSE);
  }

  /*-------------------------------------------------------------------------
    The frame is now built up. Now decide what frame rate to transmit it at.
    For the common case (SCH/SCCH) only full rate is allowed, so avoid extra
    function calls in that case.
  -------------------------------------------------------------------------*/
  if ( rev_fr_ptr->fr_rate_mask == DSRLP_RATE_1_MASK)
  {
    rev_fr_ptr->fr_rate = DSRLP_RATE_1;
    if ((rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_1) || 
        (rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_2))
    {
      /*---------------------------------------------------------------------
        Have special treatment for 1x, as the frame must be full-rate,
        even if the data did not fit.
      ---------------------------------------------------------------------*/
      frame_len  = dsrlp_get_frame_len( rev_fr_ptr->fr_rate,
                                        rev_fr_ptr->srvc_type,
                                        rev_fr_ptr->mux_pdu_type);
      head_item_ptr->used = frame_len;
    }
  }
  else  /* Not rate_1 - FCH frames */
  {
    frame_len     = (word)dsm_length_packet( head_item_ptr);
    rev_fr_ptr->fr_rate = dsrlpi_get_best_fr_rate( rev_fr_ptr, frame_len); 

    if ( rev_fr_ptr->fr_rate == DSRLP_RATE_ERASURE)
    {
      DATA_RLP_MSG2(MSG_LEGACY_ERROR, " fr_rate mask=%x len=%d Donot match",
                    rev_fr_ptr->fr_rate_mask, frame_len);
      dsm_free_packet( &head_item_ptr);
      return( FALSE);
    }
    frame_len  = dsrlp_get_frame_len( rev_fr_ptr->fr_rate,
                                      rev_fr_ptr->srvc_type,
                                      rev_fr_ptr->mux_pdu_type);
    head_item_ptr->used = frame_len;
    txq_fr_param.footer -= ( rev_fr_ptr->max_frame_size - frame_len);
  }

  /*-------------------------------------------------------------------------
    If a full rate frame was made, type bits need to be added. For MuxPDU1 &2
    the bits are added at the end of the frame. For MuxPDU3, the bits are 
    added at the beginning.
  -------------------------------------------------------------------------*/
  if ( rev_fr_ptr->fr_rate == DSRLP_RATE_1)
  {
    if( ( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_1) || 
        ( rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_2))
    {
      ASSERT( head_item_ptr->pkt_ptr == NULL);
      if ( ctl_len_present == TRUE)  
      {
        head_item_ptr->data_ptr[ head_item_ptr->used] 
                     |= dsrlpi_type_mask_table[rev_fr_ptr->mux_pdu_type]
                                              [rev_fr_ptr->srvc_type]
                                              .format_a;
      } /* format A */
      else /* format B*/
      {
        head_item_ptr->data_ptr[ head_item_ptr->used] 
                     |= dsrlpi_type_mask_table[rev_fr_ptr->mux_pdu_type]
                                              [rev_fr_ptr->srvc_type]
                                              .format_bc_new;
      }

      /*---------------------------------------------------------------------
        For MuxPDU1 and MuxPDU2 frames, there is only one dsm item per frame.
        So, set the used field to the frame length from the length table.
        Increment the footer to account for the type bits at the end of the
        frame.
      ---------------------------------------------------------------------*/  
      head_item_ptr->used++;
      txq_fr_param.footer++;
    }
    else if( (rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_3) 
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
             || (rev_fr_ptr->mux_pdu_type == DSRLP_MUX_PDU_5) 
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
             )
    {
      if ( ctl_len_present == FALSE)
      {
        head_item_ptr->data_ptr[ 0 ] 
                     |= dsrlpi_type_mask_table[rev_fr_ptr->mux_pdu_type]
                                              [rev_fr_ptr->srvc_type]
                                              .format_bc_new;
      }

      /*---------------------------------------------------------------------
        For MuxPDU3, the following loop fills all the footer bytes with 
        zeros. The logic gets complicated when another dsm item is needed to
        fill the MuxPDU.
      ---------------------------------------------------------------------*/
      bytes_to_copy = txq_fr_param.footer;
      while( bytes_to_copy > 0)
      {
        if( (tail_item_ptr->size  - tail_item_ptr->used) >= bytes_to_copy)
        {
          tail_item_ptr->used += bytes_to_copy;
          break;
        }
        bytes_to_copy       -= (tail_item_ptr->size  - (tail_item_ptr->used));
        tail_item_ptr->used  = tail_item_ptr->size;

        new_fr_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
        /*new_fr_item_ptr = dsm_new_buffer(DSM_DS_POOL_SIZE(
             (word) ( bytes_to_copy >> DSRLPI_ITEM_SIZ_DIVISOR)));*/
                                          
        if ( new_fr_item_ptr == NULL)
        {
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Memory allocation failed");
          dsm_free_packet( &head_item_ptr);
          return FALSE;
        }
        tail_item_ptr->pkt_ptr = new_fr_item_ptr;
        tail_item_ptr          = new_fr_item_ptr;
        memset( new_fr_item_ptr->data_ptr, 0, new_fr_item_ptr->size);
      } /* while */
    }
  }

  /*-------------------------------------------------------------------------
    Add the transmitted frame to the transmit queue. It will be used later
    if a retransmission is requested.
  -------------------------------------------------------------------------*/
  txq_fr_param.fr_ptr = head_item_ptr;
  

  dsrlptxq_add_frame ( rrcb_ptr, &txq_fr_param);

  /*-------------------------------------------------------------------------
    Set the fr_item_ptr to the start of the frame to transmit. Instruct the
    mux layer not to free this packet because it will be saved in the 
    transmit queue. Finally send the frame.
  -------------------------------------------------------------------------*/
  rev_fr_ptr->fr_item_ptr     = head_item_ptr;
  rev_fr_ptr->free_dsm_pkt    = FALSE; 
  rev_fr_ptr->idle_frame      = FALSE;

  return( TRUE);

} /* dsrlpi_tx_new_data_frame() */
#endif /* MSM5000_IRAM_REV */

#endif /* RLP3 defined */

#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION        DSRLPI_CRC_GEN

DESCRIPTION    This function will generate the CRC for the passed byte stream
               and store the CRC at the end of the passed stream.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None
===========================================================================*/

void dsrlpi_crc_gen
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
} /* dsrlpi_crc_gen */
#endif /* FEATURE_DS_RLP3 */

#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION       DSRLPI_GET_MIN_FRAME_AND_LEN

DESCRIPTION    This function returns the minimum frame size supported by
               a reverse frame and also returns the length (in bytes) of
               that frame.  Values are returned via passed-in ptrs.
               
               The user can specify a minimum size (i.e, the returned
               frame size must be at least a certain frame rate).
               
               If the proper frame is not found, then the input parameters
               are unchanged.
               
DEPENDENCIES   Assumes that the frame rate bitmasks are defined such 
               that the smallest frame rate is 0x01, and each successively
               larger frame rate is that value shifted one bit to the left.
               
               Assumes that non-NULL pointers are passed in.
               
               Assumes that the input min_fr_rate is equivalent to one
               of the frame rate #defines.

RETURN VALUE   TRUE if the frame info could be generated.  FALSE if the
               frame info could not be generated (for example, the 
               requested minimum size was not supported by the rev frame).

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_get_min_frame_and_len
(
   dsrlp_rev_frame_type    *rev_fr_ptr,  /*  contains rev frame info.      */
                                         /* fr_rate_ptr updated in function*/
   uint16                   min_fr_rate, /*  min frame rate needed, using  */
                                         /*  bitmask                       */
   byte                    *fr_len       /*  returned frame len (bytes)    */
)
{
   uint16                   i;           /*  loop counter                  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT((rev_fr_ptr != NULL) &&
         (fr_len != NULL));

  for (i = min_fr_rate; i <= DSRLP_RATE_1_MASK; i = i << 1)
  {
    if (rev_fr_ptr->fr_rate_mask & i)
    {
      break;
    }
  }

  /*------------------------------------------------------------------------
    At this point, an acceptable frame rate was found, or i has been left
    shifted enough that is even greater than DSRLP_RATE_1_MASK.  Need to
    set the frame_rate_enum based on the selected frame rate mask.
  ------------------------------------------------------------------------*/
  switch (i)
  {
    case DSRLP_RATE_1_16_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_1_16;
      break;
    case DSRLP_RATE_1_8_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_1_8;
      break;
    case DSRLP_RATE_3_16_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_3_16;
      break;
    case DSRLP_RATE_1_4_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_1_4;
      break;
    case DSRLP_RATE_3_8_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_3_8;
      break;
    case DSRLP_RATE_7_16_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_7_16;
      break;
    case DSRLP_RATE_1_2_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_1_2;
      break;
    case DSRLP_RATE_3_4_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_3_4;
      break;
    case DSRLP_RATE_7_8_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_7_8;
      break;
    case DSRLP_RATE_1_MASK:
      rev_fr_ptr->fr_rate = DSRLP_RATE_1;
      break;
    default:
      return(FALSE);
  }

  if( (rev_fr_ptr->srvc_type >= DSRLP_GENERIC_SRVC) ||     /* Klocwork Fix */
      (rev_fr_ptr->mux_pdu_type >= DSRLP_MUX_PDU_3) )
  {
    DATA_RLP_MSG2(MSG_LEGACY_ERROR,
                  "Invalid Service type index %d or MUX PDU type index %d",
                  rev_fr_ptr->srvc_type, rev_fr_ptr->mux_pdu_type);
    return FALSE;
  }

  /*------------------------------------------------------------------------
    If here, then acceptable enum was found.  Figure out frame length,
    and then return TRUE.
  ------------------------------------------------------------------------*/
  *fr_len  = dsrlp_get_frame_len( rev_fr_ptr->fr_rate,
                                  rev_fr_ptr->srvc_type,
                                  rev_fr_ptr->mux_pdu_type);

  ASSERT(rev_fr_ptr->max_frame_size >= *fr_len);

  return(TRUE);
}
#endif /* FEATURE_DS_RLP3 */

#ifndef MSM5000_IRAM_REV
#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION       DSRLPI_REXMIT_DATA_FRAME

DESCRIPTION    This function constructs an new data frame from payload 
               available in the transmit watermark.
               
               Only non segmented new frames are constructed.
               
               This code is specific to RLP 1, RLP2 and RLP3.
               
DEPENDENCIES   Since idle frames can only go on fundicated channels,
               the assumption is that only one idle frame can be sent
               per 20msec frame-time.
               
               rrcb_ptr->state.idle_timer is only decremented in this
               function.
               
               This function assumes RLP is in DSRLPI_ESTABLISHED_STATE
               or DSRLPI_RT_MEAS_STATE. This function should not be called
               if RLP cannot transmit new data frames.

RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               Returning FALSE in this case implies a Blank frame should
               be supplied to the MUX layer.

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlpi_rexmit_data_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  dsrlp_rev_frame_type         *rev_fr_ptr  /* Ptr to frame to be built    */
)
{
  dsrlpi_state_info_type *state_ptr;        /* local state ptr             */
  uint32                  modulus;          /* modulus for this RLP ver    */
  boolean                 octet_seq;        /* octet seq used ?            */
  boolean                 seg_allowed;      /* segmentation allowed        */

  uint32                  nak_first_seq;    /* nak first seq               */ 
  uint32                  nak_last_seq;     /* nak last seq                */

  uint32                  fr_first_seq;     /* first seq of dsm item chain */
  uint32                  fr_last_seq;      /* last seq of dsm item chain  */
  
  dsrlptxq_fr_param_type  fr_info;          /* struct passed to txq func   */
  rlpi_rexmit_info_type   rexmit_info;      /* holds rexmit info           */
  word                    data_payload_len; /* length of frame's data      */

  dsrlp_mux_pdu_enum_type mux_pdu;          /* rev fr's mux pdu            */
  dsrlp_srvc_enum_type    srvc;             /* rev fr's srvc type          */

  boolean                 seg_frame_fits;   /* fr fits within rev_fr       */
  boolean                 sending_whole_fr; /* peer requesting whole fr?   */
  boolean                 last_seg;         /* is this the last seg?       */

  rlpi_snd_fr_fmt_type    snd_fr_type;      /* type of rxmit frame to build*/

  dsm_item_type          *fr_item_ptr;      /* pts to rexmit frame         */
  dsm_item_type          *curr_item_ptr;    /* pts to item in chain        */
  dsm_item_type          *temp_item_ptr;    /* temp dsm item ptr           */

  byte                   *byte_ptr;         /* points to indiv bytes       */
  byte                    typebit_pos;      /* byte index for typebits     */
  word                    copy_len;         /* total amount to copy        */
  word                    seg_copy_len;     /* amount to copy into seg fr  */

  word                    dsm_offset;       /* where payload starts in dsm */
                                            /* item chain                  */
  byte                    table_len;        /* rlp frame size from dsrlp_  */
                                            /* _len_table                  */
  word                    bytes_remaining;  /* used when copy rlp frame to */
                                            /* new dsm item chain          */
  word                    offset;           /* used when copy rlp frame    */
  word                    bytes_to_copy;    /* bytes copied per dsm_extract*/
  boolean                 rexmit_ambig;     /* transmit ambiguity?         */
  uint32                  nak_shift_seq;    /* used to calc if need to send*/
                                            /* seq hi                      */
  uint32                  seq_shift;        /* used to calc if need send   */
                                            /* seq_hi                      */
  byte                    length_bits = 0;   /* To save the number of bits
                                               used for lenght in PDU5     */
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
  byte                   max_rlp_header_bytes = 0;
#endif
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  memset(&rexmit_info, 0, sizeof(rlpi_rexmit_info_type));
  /*-------------------------------------------------------------------------
    For code readability, use some local vars for freq. accessed values.
  -------------------------------------------------------------------------*/
  state_ptr   = &rrcb_ptr->state;
  modulus     = rrcb_ptr->cfg_ptr->seq_modulus;
  octet_seq   = rrcb_ptr->cfg_ptr->octet_sequenced;
  seg_allowed = (rrcb_ptr->cfg_ptr->ver_ptr)->seg_allowed;

  mux_pdu     = rev_fr_ptr->mux_pdu_type;
  srvc        = rev_fr_ptr->srvc_type;
  
  seg_frame_fits   = FALSE;
  sending_whole_fr = FALSE;
  last_seg         = FALSE;
  rexmit_ambig     = FALSE;

  seg_copy_len     = 0xFFFF;   /* these are impossible values for length of*/
  copy_len         = 0xFFFF;   /* data in a single RLP frame               */

  if (rlpi_get_rexmit_data(rrcb_ptr,&fr_info, &rexmit_info) == FALSE)
  {
    rrcb_ptr->stats.re_xmits_not_found++;
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    The frame to rexmit has been found.  Use some local vars for readability.
    Do some calculations on how much will be copied.  First find out the seq
    num of the last byte in the dsm item chain.
  -------------------------------------------------------------------------*/
  nak_first_seq    = rexmit_info.nak_first_seq;
  nak_last_seq     = rexmit_info.nak_last_seq;
  data_payload_len = rexmit_info.data_payload_len;

  fr_first_seq     = fr_info.seq_num;

  ASSERT(data_payload_len > 0);
  fr_last_seq = DSRLPSEQ_ADD_S_SEQ(fr_first_seq,
                                   (data_payload_len - 1),
                                   modulus);

  /*-------------------------------------------------------------------------
    If nak_last_seq indicates "end of frame", then update nak_last_seq to
    the exact end of frame value.  This removes headaches later on.
  -------------------------------------------------------------------------*/
  if ((DSRLPSEQ_GET_S_SEQ(nak_last_seq) == RLP3_NAK_LAST_S_SEQ_MAX) &&
      (octet_seq == FALSE))
  {
    nak_last_seq = fr_last_seq;
  }

  /*-------------------------------------------------------------------------
    Find out how many bytes are to be copied out of this dsm packet chain.
    Use some local vars for readability.
  -------------------------------------------------------------------------*/
  if (octet_seq == TRUE)
  {
    ASSERT(seg_allowed == TRUE);

    /*-----------------------------------------------------------------------
      With byte-sequencing, the whole sequence space is used - there is no
      concept of SEQ or SEQ_HI or S_SEQ - just a flat 24-bit seq number.
      
      This format is only used for RLP 3 and HDR.
    -----------------------------------------------------------------------*/
    if (DSRLPSEQ_GE_SEQ_CHECK(nak_last_seq, fr_last_seq, modulus))
    {
      copy_len = (word)DSRLPSEQ_DIFF_SEQ(fr_last_seq, nak_first_seq, modulus);
    }
    else
    {
      copy_len = (word)DSRLPSEQ_DIFF_SEQ(nak_last_seq, nak_first_seq, modulus);
    }
    copy_len++;  /* inc since difference is between inclusive numbers      */
  }
  else if (seg_allowed == TRUE)
  {
    /*-----------------------------------------------------------------------
      Segmentation allowed.  The other side can NAK for whole frames or
      parts of frames.  Each dsm packet chain, however, contains the
      bytes for an entire RLP frame (i.e, one SEQ space).  
      
      This format is only used for RLP 3.
    -----------------------------------------------------------------------*/
    if ((nak_first_seq == nak_last_seq) &&
        !(DSRLPSEQ_HAS_S_SEQ(nak_last_seq) || 
          DSRLPSEQ_HAS_S_SEQ(nak_first_seq)))
    {
      /*---------------------------------------------------------------------
        Peer is nak'ing one whole frame, and so first_seq == last_seq.
      ---------------------------------------------------------------------*/
      copy_len         = data_payload_len;
      last_seg         = TRUE;
      sending_whole_fr = TRUE;
    }
    else if (DSRLPSEQ_GT_SEQ_CHECK(fr_last_seq, nak_last_seq, modulus))
    {
      /*---------------------------------------------------------------------
        NAK contained completely within this frame.
      ---------------------------------------------------------------------*/
      ASSERT(DSRLPSEQ_HAS_S_SEQ(nak_first_seq) ||
             DSRLPSEQ_HAS_S_SEQ(nak_last_seq));

      copy_len = (word)DSRLPSEQ_DIFF_SEQ(nak_last_seq, nak_first_seq, modulus);
      copy_len++;  /* inc since difference is between inclusive numbers    */
    }
    else
    {
      /*---------------------------------------------------------------------
        The NAK is either at the end of this frame or beyond.  Calculate
        as such.
      ---------------------------------------------------------------------*/
      copy_len = (word)DSRLPSEQ_DIFF_SEQ(fr_last_seq, nak_first_seq, modulus);

      copy_len++;  /* inc since difference is between inclusive numbers    */
      last_seg = TRUE;
      
      if (nak_first_seq == fr_first_seq)
      {
        sending_whole_fr = TRUE;
      }
    }
  } /* segmentation allowed */
  else
  {
    /*-----------------------------------------------------------------------
      Segmentation not allowed, even for re-xmits.  Used in RLP 1 & 2.
      Will end up transmitting entire frame.
    -----------------------------------------------------------------------*/
    if ((nak_first_seq != fr_first_seq)  ||
        (DSRLPSEQ_HAS_S_SEQ(nak_first_seq)) ||
        (DSRLPSEQ_HAS_S_SEQ(nak_last_seq)))
    {
      /*---------------------------------------------------------------------
        The peer has sent strange NAK info.  Remove it from the list, as it
        doesn't make sense.  Return FALSE, and on next NAK rexmit, try
        again.
      ---------------------------------------------------------------------*/
      DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Strange NAK info from peer");
      dsrlprtx_remove_first_element(rrcb_ptr);
      return FALSE;
    }

    copy_len         = data_payload_len;
    sending_whole_fr = TRUE;
    last_seg         = TRUE;
  }

  /*-------------------------------------------------------------------------
    Figure out offset into dsm packet chain to figure out where to start
    copying the data payload.  Account for any header that may be in the
    packet.
  -------------------------------------------------------------------------*/
  dsm_offset = (word)DSRLPSEQ_DIFF_SEQ(nak_first_seq, fr_first_seq, modulus);
  dsm_offset += fr_info.header;

  /*-------------------------------------------------------------------------
    Determine if there is a rexmit ambiguity problem.  This only occurs if
    segmentation is allowed (i.e., RLP 3).
  -------------------------------------------------------------------------*/
  if (seg_allowed == TRUE)
  {
    nak_shift_seq = DSRLPSEQ_GET_12BIT_FRAME_SEQ(nak_first_seq);
    seq_shift     = DSRLPSEQ_GET_12BIT_FRAME_SEQ(state_ptr->peer_vn);
    if (((nak_shift_seq + 4096 - seq_shift) % 4096) > 255)
    {
      rexmit_ambig = TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    Based on MuxPDU, can decide if Fmt A, B, C, D or none of the above.
     
    Fmt A, B, C, D all require Rate 1 or 2 frames.
      
    MuxPDU1 implies Fmt A or B or no format.
    MuxPDU3 implies Fmt C or D  & supp. channel.
      
    Fmt B & C imply retransmission of an unsegmented frame (most likely,
    the original transmission was also Fmt B or C (respectively) since
    the frames are fixed length (so padding or segmentation is not 
    possible)).  
      
    To determine if it was a B or C frame, compare the header, footer,
    and payload len against the fixed sizes mandated by the standard.
      
    Format C & D are restricted to RLP 3.
  -------------------------------------------------------------------------*/

  if ((mux_pdu == DSRLP_MUX_PDU_1)   ||
      (mux_pdu == DSRLP_MUX_PDU_2))
  {
    if (!(rev_fr_ptr->fr_rate_mask & DSRLP_RATE_1_MASK))
    {
      /*---------------------------------------------------------------------
        Format A & B require Rate 1..., so no format to be used
      ---------------------------------------------------------------------*/
      snd_fr_type = RLPI_SND_FR_FMT_NONE;
    }
    else if ((sending_whole_fr == TRUE)               &&
             (fr_info.header == 1)                    && 
             (fr_info.footer == 1)                    &&
             (rexmit_ambig   == FALSE)                &&
             (octet_seq == FALSE))
    {
      /*---------------------------------------------------------------------
        Only format B has 1 byte header and footer and has MUX PDU 1 or
        2.  Check to see if the max frame size matches they payload len.
        do this based on primary/secondary and if odd/even mux options.
        
        2 should subtracted from table_len to make up for the hdr and
        footer, but due to rounding-down in the table, only 1 is 
        subtracted.
      ---------------------------------------------------------------------*/
      table_len = dsrlp_get_frame_len(DSRLP_RATE_1,
                                      srvc,
                                      mux_pdu);

      if ((data_payload_len == (table_len - 1)) && 
          ((rev_fr_ptr->max_frame_size) == table_len))
      {
        snd_fr_type = RLPI_SND_FR_FMT_B;
      }
      else
      {
        DATA_RLP_MSG3(MSG_LEGACY_MED, "tbl_len:%d;max_size:%d;payld:%d; Format B frame, but not correct length",
            table_len,
            (rev_fr_ptr->max_frame_size),
            data_payload_len);
        snd_fr_type = RLPI_SND_FR_FMT_A;
      }
    } /* header_len == footer_len == 1 */
    else
    {
      /*---------------------------------------------------------------------
        The lengths do not match up or sending segmented.  Must send a
        format A frame, since still Mux PDU 1 or 2 and Rate 1.
      ---------------------------------------------------------------------*/
      snd_fr_type = RLPI_SND_FR_FMT_A;
    }
  } /* mux pdu 1 or 2 */
  else
  {
     


    /*------------------------------------------------------------------------
      Mux PDU 3... so must be either Fmt C or D.  As for FMT B, check to
      see if it was a format C frame.  Specific to RLP 3.
    ------------------------------------------------------------------------*/
    if ((sending_whole_fr  == TRUE)              &&
        (fr_info.header == 2)                    && 
        (fr_info.footer == 0)                    &&
        (octet_seq == FALSE)                     &&
        (rexmit_ambig   == FALSE)                &&
        (rev_fr_ptr->fr_rate_mask & DSRLP_RATE_1_MASK))
    {
      /*---------------------------------------------------------------------
        Only format C has 2 byte header and 0-byte footer and has MUX 
        PDU 3.  Check to see if the payload len matches the max_frame_size.
        Subtract 1 to make up for the header and footer.  2 is not
        subtracted from max_frame_size because is a "rounded-down" value.
      ---------------------------------------------------------------------*/
      if (data_payload_len == (rev_fr_ptr->max_frame_size - 1))
      {
        snd_fr_type = RLPI_SND_FR_FMT_C;
      }
      else
      {
        snd_fr_type = RLPI_SND_FR_FMT_D;
      }
    } /* sending_whole_fr == TRUE, etc */
    else
    {
      /*---------------------------------------------------------------------
        The lengths do not match up or byte oriented.  Must send a
        format D frame.
      ---------------------------------------------------------------------*/
      snd_fr_type = RLPI_SND_FR_FMT_D;
    }
  } /* else muxpdu 3/5 */
    
  /*------------------------------------------------------------------------
    If segmented allowed, see if in fact transmitting the whole frame.
    Can then use a simple Format A frame to xmit the data.
  -------------------------------------------------------------------------*/
  if ((seg_allowed == TRUE)          && 
      (sending_whole_fr == TRUE)     &&
      (octet_seq == FALSE)           &&
      (rexmit_ambig   == FALSE)      &&
      ((mux_pdu == DSRLP_MUX_PDU_1) || (mux_pdu == DSRLP_MUX_PDU_2)))
  {
    if (copy_len + RLP3_DATA_UNSEG_HDR_SIZE <= rev_fr_ptr->max_frame_size)
    {
      /*--------------------------------------------------------------------
        Segmentation is allowed, and sending a full frame, so go ahead 
        and send it with the unsegmented frame format
      --------------------------------------------------------------------*/
      seg_frame_fits = TRUE;
    }
  }

  /*-------------------------------------------------------------------------
    The following key pieces of info are now known:
      
    dsm_offset:  how many bytes into the payload copying should start
    copy_len  :  how many bytes of payload to copy.
                   
    snd_fr_type: Frame fmt to use when sending data.
    sending_whole_fr:  if the whole RLP frame (in terms of SEQ) is being
                       NAK'd.
                       
    seg_frames_fits:  Segmented allowed, but still sending one whole frame
                      which fits in the rev_fr_ptr frame.                  
     
    At this point, ready to copy data into the dsm items.  Get a dsm item
    to kick things off.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
  if(( snd_fr_type == RLPI_SND_FR_FMT_D)  && ( mux_pdu == DSRLP_MUX_PDU_5)) {
    /* ---------------------------------------------------------------------
     This is required because the rlp header is also accouneted for payload
     in the PDU5.
     This scenario will also include control_bits. First of all
     arrive at the number of controls preceding the data.
    ---------------------------------------------------------------------*/
    
    max_rlp_header_bytes = ( RLP3_SEQ_LEN
                         + RLP3_FMT_D_PDU5_CTL_LEN
                         + RLP3_FMT_D_SEQ_HI_LEN
                         + RLP3_FMT_D_S_SEQ_LEN
                         + 7)                   /* Padding                  */
                         >> 3;                  /* Bits to bytes conversion.*/
    

    fr_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );  
  }
  else 
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
  {
      fr_item_ptr = dsm_new_buffer(DSM_DS_POOL_SIZE( (word) 
                                  (copy_len >> DSRLPI_ITEM_SIZ_DIVISOR)));
  }
  if (fr_item_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Memory allocation failed");
    return FALSE;
  }
  memset(fr_item_ptr->data_ptr,0,fr_item_ptr->size);
  curr_item_ptr = fr_item_ptr;

  /*-------------------------------------------------------------------------
    Everything is now ready to go.  For FMT B & C frames, do a copy of the
    entire frame, and only replace the type bits, to indicate that this
    is a retransmission.  For FMT NONE, A, & D, more work required.
  -------------------------------------------------------------------------*/
  if ((snd_fr_type == RLPI_SND_FR_FMT_B) ||
      (snd_fr_type == RLPI_SND_FR_FMT_C))
  {
    bytes_remaining = copy_len + 2;  /* 2 = hdr & ftr for fmt b.  For fmt c*/
                                     /* it includes hdr + type bits        */
    offset          = 0;

    do
    {
      if (curr_item_ptr->used > 0)
      {
        ASSERT(curr_item_ptr->used == curr_item_ptr->size);

		temp_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
                                                
        if (temp_item_ptr == NULL)
        {
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Memory allocation failed");
          dsm_free_packet( &fr_item_ptr);
          return FALSE;
        }
        memset(temp_item_ptr->data_ptr,0,temp_item_ptr->size);
        curr_item_ptr->pkt_ptr = temp_item_ptr;
        curr_item_ptr = temp_item_ptr;
      }

      bytes_to_copy = MIN(curr_item_ptr->size, bytes_remaining);

      (void) dsm_extract(fr_info.fr_ptr,
                  offset,
                  curr_item_ptr->data_ptr,
                  bytes_to_copy);

      curr_item_ptr->used = bytes_to_copy;
      offset             += bytes_to_copy;
      bytes_remaining     = bytes_remaining - bytes_to_copy;
    }
    while (bytes_remaining > 0);

    /*---------------------------------------------------------------------
      now fix up the type bits to indicate a retransmission.
    ---------------------------------------------------------------------*/
    if (snd_fr_type == RLPI_SND_FR_FMT_B)
    {
      if (srvc >= DSRLP_GENERIC_SRVC || mux_pdu >= DSRLP_MUX_PDU_3)
      {
        DATA_RLP_MSG2(MSG_LEGACY_ERROR, "Invalid index for dsrlp_len_table %d, %d", 
                      srvc, mux_pdu);
        return FALSE;
      }
      
      typebit_pos = dsrlp_get_frame_len(DSRLP_RATE_1,srvc,mux_pdu);

      /*------------------------------------------------------------------- 
        Format B fits in one small item, so the "used' field should
        correspond to the byte pos of the type bits.
      -------------------------------------------------------------------*/
      ASSERT((curr_item_ptr->used > 0) &&
             (curr_item_ptr->used == (typebit_pos + 1)));

      byte_ptr = &(curr_item_ptr->data_ptr[typebit_pos]);

      *byte_ptr |= dsrlpi_type_mask_table[mux_pdu][srvc].format_bc_rexmit;
    } /* else format c */
    else
    {
      /*-------------------------------------------------------------------
        Format C frames use the first byte for the mux header and the
        type bits.
      -------------------------------------------------------------------*/
      fr_item_ptr->data_ptr[0]  = rrcb_ptr->cfg_ptr->sr_id << 5;;
      fr_item_ptr->data_ptr[0] |= dsrlpi_type_mask_table[mux_pdu]
                                                        [srvc].
                                                        format_bc_rexmit;
    }

    /*---------------------------------------------------------------------
      Fill in the frame rate and update some vars so that rtx update
      happens correctly.
    ---------------------------------------------------------------------*/
    rev_fr_ptr->fr_rate = DSRLP_RATE_1;

    seg_copy_len  = copy_len;

  } /* fmt B or C frame */
  else if ((seg_allowed == FALSE) || (seg_frame_fits == TRUE))
  {
    /*-----------------------------------------------------------------------
      Sending a whole frame via FORMAT A.  Segmented may not be allowed,
      or it is allowed and the whole frame fits in the supplied rev fr.
    -----------------------------------------------------------------------*/
    ASSERT( (octet_seq == FALSE) &&
            ((mux_pdu == DSRLP_MUX_PDU_1) || (mux_pdu == DSRLP_MUX_PDU_2)) &&
            ((snd_fr_type == RLPI_SND_FR_FMT_A)     || 
           (snd_fr_type == RLPI_SND_FR_FMT_NONE)) &&
           (data_payload_len == copy_len) &&
           (nak_first_seq == fr_first_seq) );

    
    /*-----------------------------------------------------------------------
      See if the max_frame_size is big enough.  Add 2 to the data_payload_len
      to account for the header required.
    -----------------------------------------------------------------------*/
    if ((copy_len + RLP3_DATA_UNSEG_HDR_SIZE) > rev_fr_ptr->max_frame_size)
    {
      DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Frame not large enough for rexmit");
      dsm_free_packet(&fr_item_ptr);
      return FALSE;
    }
    
    /*-----------------------------------------------------------------------
      Copy the data in and be done.  It should take less than one item.
    -----------------------------------------------------------------------*/
    ASSERT((copy_len <= (fr_item_ptr->size - RLP3_DATA_UNSEG_HDR_SIZE)) &&
           (copy_len <= RLP3_DATA_UNSEG_DATA_MAX_LEN));

    fr_item_ptr->data_ptr[0] = DSRLPSEQ_GET_SEQ(fr_first_seq);
    fr_item_ptr->data_ptr[1] = (byte)(RLP3_UNSEG_DATA_REXMIT_MASK | 
                                      (byte)copy_len);

    (void) dsm_extract(fr_info.fr_ptr, 
                dsm_offset,
                &(fr_item_ptr->data_ptr[2]),
                copy_len);

    /*-----------------------------------------------------------------------
      Put in type bits.  Format A and NONE frames both fit within a single
      small dsm item.  In either case, go ahead and put the type bits in -
      if a FMT_NONE, frame, then it'll be less than RATE 1, and the type
      bits will not be sent by the MUX layer.  Otherwise if RATE 1, then 
      it becomes TYPE A.
    -----------------------------------------------------------------------*/
    if (srvc >= DSRLP_GENERIC_SRVC || mux_pdu >= DSRLP_MUX_PDU_3)
    {
      DATA_RLP_MSG2(MSG_LEGACY_ERROR, "Invalid index for dsrlp_len_table %d, %d", 
                    srvc, mux_pdu);
      return FALSE;
    }
    
    typebit_pos = dsrlp_get_frame_len(DSRLP_RATE_1,srvc,mux_pdu);

    byte_ptr = &(fr_item_ptr->data_ptr[typebit_pos]);
    *byte_ptr |= dsrlpi_type_mask_table[mux_pdu][srvc].format_a;
  
    /*-----------------------------------------------------------------------
      Find out what rate to use.  If rate 1, then add a byte to the "used"
      field to account for the type bits.
    -----------------------------------------------------------------------*/
    rev_fr_ptr->fr_rate = dsrlpi_get_best_fr_rate(rev_fr_ptr,
                                        copy_len + RLP3_DATA_UNSEG_HDR_SIZE);

    
    fr_item_ptr->used = dsrlp_get_used_frame_len(rev_fr_ptr->fr_rate,
                                                 srvc,
                                                 mux_pdu);

    seg_copy_len  = copy_len;
  } /* sending whole frame via FORMAT A*/
  else if ((mux_pdu == DSRLP_MUX_PDU_1) ||
           (mux_pdu == DSRLP_MUX_PDU_2))
  {
    /*-----------------------------------------------------------------------
      Segmented allowed, and mux_pdu implies format A or NONE.
      
      Copy as much as possible into the frame.  For segmented, include all
      optional header fields always.  Figure out how much can be copied.
    -----------------------------------------------------------------------*/
    if (RLP3_DATA_SEG_HDR_SIZE >= rev_fr_ptr->max_frame_size)
    {
      DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Frame not large enough for rexmit");
      dsm_free_packet(&fr_item_ptr);
      return FALSE;
    }

    seg_copy_len = MIN(copy_len,
                       (rev_fr_ptr->max_frame_size - 
                                                  RLP3_DATA_SEG_HDR_SIZE));

    /*-----------------------------------------------------------------------
      Insert the 8-bit SEQ
    -----------------------------------------------------------------------*/
    fr_item_ptr->data_ptr[0] = DSRLPSEQ_GET_SEQ(nak_first_seq);

    /*-----------------------------------------------------------------------
      Set the CTL field.
    -----------------------------------------------------------------------*/
    fr_item_ptr->data_ptr[1] = RLP3_SEG_DATA_FLD_CTL  |
                               RLP3_SEG_DATA_SQI_MASK;
                               
    /*-----------------------------------------------------------------------
      if last_seg, then set the last seg bit.  For octet sequencing,
      the last_seg is never set.
    -----------------------------------------------------------------------*/
    if ((last_seg == TRUE)    && 
        (octet_seq == FALSE)  &&
        (seg_copy_len == copy_len))
    {
      fr_item_ptr->data_ptr[1] |= RLP3_SEG_DATA_LST_MASK;
    }

    /*-----------------------------------------------------------------------
      Set the rexmit bit.
    -----------------------------------------------------------------------*/
    fr_item_ptr->data_ptr[1] |= RLP3_SEG_DATA_REXMIT_MASK;

    /*-----------------------------------------------------------------------
      Insert the length.  The payload carriable depends on the max fr.
      size and the size of the header vs. the copy_len.  The max value
      is 15 (per standard).
    -----------------------------------------------------------------------*/
    b_packb((byte)seg_copy_len,
            fr_item_ptr->data_ptr,
            RLP3_DATA_SEG_LEN_POS,
            RLP3_DATA_SEG_LEN_LEN);

    /*-----------------------------------------------------------------------
      Insert the SEQ_HI
    -----------------------------------------------------------------------*/
    b_packb(DSRLPSEQ_GET_SEQ_HI(nak_first_seq),                          
            fr_item_ptr->data_ptr,
            RLP3_DATA_SEG_SEQ_HI_POS,
            RLP3_DATA_SEG_SEQ_HI_LEN);

    /*-----------------------------------------------------------------------
      Insert S_SEQ
    -----------------------------------------------------------------------*/
    b_packw(DSRLPSEQ_GET_S_SEQ(nak_first_seq),
            fr_item_ptr->data_ptr,
            RLP3_DATA_SEG_S_SEQ_POS,
            RLP3_DATA_SEG_S_SEQ_LEN);

    /*-----------------------------------------------------------------------
      Copy in the data
    -----------------------------------------------------------------------*/
    (void) dsm_extract(fr_info.fr_ptr, 
                dsm_offset,
                &(fr_item_ptr->data_ptr[RLP3_DATA_SEG_HDR_SIZE]),
                seg_copy_len);

    /*-----------------------------------------------------------------------
      Put in the type bits, and then figure out the required frame rate.
      If RATE 1, add a byte to account for type bits.
    -----------------------------------------------------------------------*/
    typebit_pos = dsrlp_get_frame_len(DSRLP_RATE_1,srvc,mux_pdu);

    byte_ptr = &(fr_item_ptr->data_ptr[typebit_pos]);
    *byte_ptr |= dsrlpi_type_mask_table[mux_pdu][srvc].format_a;

    rev_fr_ptr->fr_rate = dsrlpi_get_best_fr_rate(rev_fr_ptr, 
                                     RLP3_DATA_SEG_HDR_SIZE + seg_copy_len);

    if (rev_fr_ptr->fr_rate == DSRLP_RATE_ERASURE)
    {
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "seg frame not big enough");
      (void) dsm_free_buffer(fr_item_ptr);
      return FALSE;
    }

    fr_item_ptr->used = dsrlp_get_used_frame_len(rev_fr_ptr->fr_rate,
                                                 srvc,
                                                 mux_pdu);
  }
  else   /* MUXPDU3or5 Format D  */
  {
    
    length_bits = 0;
    fr_item_ptr->used = 0;
    
	/*-----------------------------------------------------------------------
      A format D frame.  Put in the sr_id bits.  The type bits are zero,
      and so not explicitly put in.  The SEQ number then starts on the next
      byte boundary.  All optional fields are included in the header.
    -----------------------------------------------------------------------*/
    fr_item_ptr->data_ptr[fr_item_ptr->used]= rrcb_ptr->cfg_ptr->sr_id << 5;    
    
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
    
    /* Need to insert lenght and length Indicator for PDU5
    ** RLP3_FMT_D_HDR_SIZE -2, to incorporate for typebits and Lenght filed present in 
    ** the format D frame
    ** Lenght Indicator     Length (bits)
    **    00                  0
    **    01                  8
    **    10                  16
    */
    if(mux_pdu == DSRLP_MUX_PDU_5)
    {
       /*----------------------------------------------------------------------
         if we came here, we are building a FORMAT D frame.
          
         Since we are building a Format D frame, let's decide about Mux Payload
         
         PDU :         MuxHeader + MuxPayload
         MuxHeader:    For FormatD, MuxHeader will be either 2 bytes or 3 bytes 
                       depending on "01" or "10" case. 2 bytes because of SR_ID
                       byte and other for holding length.
                       3 Bytes will be for Sr_ID + 2 bytes for Length.
         Mux Payload = RLP Header + RLP Payload bytes
         RLP Header: For Format D frame retransmit frames, RLP header is always 
                     same number of bytes, as SeqHi and S-Seq always. 
         RLP Payload: Total payload that can be accomodated is given by,
                      (MaxFrameSize+1) - (MuxHeader+ RLP Header). NOTE: We have
                      MaxFrameSize+1 ,because SR_ID related byte is added. 
                      So find out if we have this much data to send.
         
         Notation in Code: 
            seg_copy_len: Data that can be sent in this frame.
            copy_len: Total payload available to be sent.
                      
       ------------------------------------------------------------------------*/
      if(rev_fr_ptr->max_frame_size < 0xFF) 
      {     
         seg_copy_len = MIN((rev_fr_ptr->max_frame_size-(1+max_rlp_header_bytes)), 
                             copy_len); 
         length_bits = 8;
         fr_item_ptr->data_ptr[fr_item_ptr->used ] |= 0x04;
         b_packw( (seg_copy_len+max_rlp_header_bytes) , 
                   &fr_item_ptr->data_ptr[ fr_item_ptr->used ], 
                   RLP3_FMT_D_PDU5_LEN_IND_POS, 
                   length_bits);
         fr_item_ptr->used += 2;
       }
       else if(rev_fr_ptr->max_frame_size < 0xFFFF)
       {
          seg_copy_len = MIN((rev_fr_ptr->max_frame_size-(2+max_rlp_header_bytes)), copy_len); 
          length_bits = 16;
          fr_item_ptr->data_ptr[ fr_item_ptr->used ] |= 0x08;
          b_packw( (seg_copy_len+max_rlp_header_bytes), 
                    &fr_item_ptr->data_ptr[ fr_item_ptr->used ], 
                    RLP3_FMT_D_PDU5_LEN_IND_POS, 
                    length_bits);
          fr_item_ptr->used += 3;
       } 
    }
    /*-----------------------------------------------------------------------
      Calculate how much can be copied into the frame.  Note that the
      RLP_FMT_D_HDR_SIZE includes the type bits.  Subtract those out, as
      the max_frame_size does not include type bits either.
    -----------------------------------------------------------------------*/
    
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
    if(mux_pdu == DSRLP_MUX_PDU_3)
    {
       fr_item_ptr->used += 1;
       seg_copy_len = MIN(copy_len,
                       (rev_fr_ptr->max_frame_size - 
                                             (RLP3_FMT_D_HDR_SIZE - 1)));

    }
     /* Insert the SEQ of the frame */
    fr_item_ptr->data_ptr[fr_item_ptr->used++]= DSRLPSEQ_GET_SEQ(nak_first_seq);
     
    /*-----------------------------------------------------------------------
      Set SSP and SQI to indicate both the S_SEQ and SEQ_HI will be
      included.
    -----------------------------------------------------------------------*/
    fr_item_ptr->data_ptr[fr_item_ptr->used] = RLP3_FMT_D_DATA_SSP_MASK |
                               RLP3_FMT_D_DATA_SQI_MASK;

    /*-----------------------------------------------------------------------
      put in the last seg bit.  Also set the rexmit bit.  For octet_seq,
      last_seg is never set.
    -----------------------------------------------------------------------*/
    if ((last_seg == TRUE)        &&
        (octet_seq == FALSE)      &&
        (seg_copy_len == copy_len))
    {
      fr_item_ptr->data_ptr[fr_item_ptr->used] |= RLP3_FMT_D_DATA_LST_MASK;
    }

    /*-----------------------------------------------------------------------
      Set rexmit bit.
    -----------------------------------------------------------------------*/
    fr_item_ptr->data_ptr[fr_item_ptr->used] |= RLP3_FMT_D_DATA_RTX_MASK;

    /*-----------------------------------------------------------------------
      Insert the copy length, SEQ_HI and S_SEQ.
    -----------------------------------------------------------------------*/
    /* Length field is present only for PDU 3*/
    if(mux_pdu == DSRLP_MUX_PDU_3)
    {
        b_packb((byte)seg_copy_len,
            fr_item_ptr->data_ptr,
            RLP3_FMT_D_LEN_POS,
            RLP3_FMT_D_LEN_LEN);

    

        b_packb(DSRLPSEQ_GET_SEQ_HI(nak_first_seq),
            fr_item_ptr->data_ptr,
            RLP3_FMT_D_SEQ_HI_POS,
            RLP3_FMT_D_SEQ_HI_LEN);

        b_packw(DSRLPSEQ_GET_S_SEQ(nak_first_seq),
            fr_item_ptr->data_ptr,
            RLP3_FMT_D_S_SEQ_POS,
            RLP3_FMT_D_S_SEQ_LEN);
    }
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
    else if(mux_pdu == DSRLP_MUX_PDU_5)
    {
        
        b_packb(DSRLPSEQ_GET_SEQ_HI(nak_first_seq),
            fr_item_ptr->data_ptr,
            (RLP3_FMT_D_LEN_POS + length_bits),
            RLP3_FMT_D_SEQ_HI_LEN);

        b_packw(DSRLPSEQ_GET_S_SEQ(nak_first_seq),
            fr_item_ptr->data_ptr,
            (RLP3_FMT_D_LEN_POS + RLP3_FMT_D_SEQ_HI_LEN  + length_bits ),
            RLP3_FMT_D_S_SEQ_LEN);
       /*----------------------------------------------------------------
        Used variable has accounted for control bits (4) + seq_hi (4) +
         s_seq(12) + padding(4) = 24 bits = 3bytes
        ----------------------------------------------------------------*/
        fr_item_ptr->used += 3; 
     }
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/


    /*-----------------------------------------------------------------------
      By now we decide how much data we can send in this Frame  and it is
      seg_copy_len
    -----------------------------------------------------------------------*/  
    /*-----------------------------------------------------------------------      
      copy the data... it may take more than one dsm item.  Note that
      the dsm item includes not only the 5-byte header, but also the
      preceding 1-byte of TYPE bits and MUX header.
    -----------------------------------------------------------------------*/
    bytes_to_copy = 0;
    if(mux_pdu == DSRLP_MUX_PDU_3)
    { 
       bytes_to_copy = MIN(seg_copy_len,
                        (fr_item_ptr->size - RLP3_FMT_D_HDR_SIZE));

       (void) dsm_extract(fr_info.fr_ptr,
                   dsm_offset,
                   &(fr_item_ptr->data_ptr[RLP3_FMT_D_HDR_SIZE]),
                   bytes_to_copy);
    }
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
    else if(mux_pdu == DSRLP_MUX_PDU_5)
    {
        bytes_to_copy = MIN(seg_copy_len,
                        ((fr_item_ptr->size) - (fr_item_ptr->used)) );
        (void) dsm_extract(fr_info.fr_ptr,
                    dsm_offset,
                    &(fr_item_ptr->data_ptr[fr_item_ptr->used]),
                    bytes_to_copy);

    }
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
    
    bytes_remaining   = seg_copy_len - bytes_to_copy;
    offset            = dsm_offset + bytes_to_copy;
    if(mux_pdu == DSRLP_MUX_PDU_3)
    {
        fr_item_ptr->used = RLP3_FMT_D_HDR_SIZE + bytes_to_copy;

    }
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
    else if(mux_pdu == DSRLP_MUX_PDU_5)
    {
        fr_item_ptr->used += bytes_to_copy;
    }
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
     
    curr_item_ptr = fr_item_ptr;

    while (bytes_remaining > 0)
    {
      if (curr_item_ptr->used > 0)
      {
        ASSERT(curr_item_ptr->used == curr_item_ptr->size);

		temp_item_ptr =dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
        /*temp_item_ptr = dsm_new_buffer(DSM_DS_POOL_SIZE( 
                        (word) (copy_len >> DSRLPI_ITEM_SIZ_DIVISOR)));*/
        if (temp_item_ptr == NULL)
        {
          dsm_free_packet( &fr_item_ptr);
          DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Memory allocation failed");
          return FALSE;
        }
        memset(temp_item_ptr->data_ptr,0,temp_item_ptr->size);
        curr_item_ptr->pkt_ptr = temp_item_ptr;
        curr_item_ptr = temp_item_ptr;
      }

      bytes_to_copy = MIN(curr_item_ptr->size, bytes_remaining);

      (void) dsm_extract(fr_info.fr_ptr,
                  offset,
                  curr_item_ptr->data_ptr,
                  bytes_to_copy);

      curr_item_ptr->used = bytes_to_copy;
      offset             += bytes_to_copy;
      bytes_remaining     = bytes_remaining - bytes_to_copy;
    } /* while */

    /*-----------------------------------------------------------------------
      Fmt D on the supplemental is always rate 1.
    -----------------------------------------------------------------------*/
    rev_fr_ptr->fr_rate = DSRLP_RATE_1;
  } /* FMT D segmented */


  /*-------------------------------------------------------------------------
    Update the retransmit list.  Refresh on what the vars mean:
    
    nak_first_seq = the seq num of the first byte to transmit, as requested
                    by the peer.
                    
    nak_last_seq  = the seq num of the last byte to transmit, as requested
                    by the peer.
                    
    seg_copy_len  = number bytes actually copied into frame > 0.
    copy_len      = number of bytes we wanted to copy, from this SEQ frame.
    last_seg      = transmitted the end of a SEQ frame
    
    octet_seq     = doing byte-sequenced op - no SEQ concept
  -------------------------------------------------------------------------*/
  ASSERT(seg_copy_len > 0);

  DATA_RLP_MSG1(MSG_LEGACY_MED, "Sending Rexmit, with SEQ=%x", nak_first_seq);

  if (octet_seq == TRUE)
  {
    /*-----------------------------------------------------------------------
      Increment nak_first_seq and see if all requested bytes have been
      rexmitted.  Update list accordingly.
    -----------------------------------------------------------------------*/
    nak_first_seq = DSRLPSEQ_ADD_S_SEQ(nak_first_seq, 
                                       (seg_copy_len - 1),
                                       modulus);
    if (DSRLPSEQ_GE_SEQ_CHECK(nak_first_seq, nak_last_seq, modulus))
    {
      ASSERT(nak_first_seq == nak_last_seq);
      dsrlprtx_remove_first_element(rrcb_ptr);
    }
    else
    {
      nak_first_seq = DSRLPSEQ_INCREMENT_S_SEQ(nak_first_seq,
                                               rrcb_ptr->cfg_ptr->seq_modulus);
      dsrlprtx_update_first_element(rrcb_ptr,
                                    nak_first_seq,
                                    nak_last_seq);
    }
  } /* if byte-sequenced operation */
  else if (seg_copy_len == copy_len)
  {
    /*-----------------------------------------------------------------------
      Doing frame-based operation.  If here, then all bytes to be xmitted
      from this frame have been sent.  Cases are that peer NAK'd the 
      following:
      
      1  -> 1
      1a -> 1a
      1b -> 1d
      1  -> 5
      1f -> 2a
      
      Add bytes xmitted to nak_first_seq, and see if that causes removal
      of element from rtx queue (i.e., nak_first_seq > nak_last_seq )
    -----------------------------------------------------------------------*/
    if (DSRLPSEQ_HAS_SAME_SEQ(nak_first_seq,nak_last_seq))
    {
      /*---------------------------------------------------------------------
        Covers cases where peer nak'd info in one frame only, and all that
        info was sent:  1 -> 1, 1a -> 1a, 1b -> 1d
      ---------------------------------------------------------------------*/
      dsrlprtx_remove_first_element(rrcb_ptr);
    }
    else
    {
      /*---------------------------------------------------------------------
        nak last_seq in the next frame.  Since all info from this frame was
        sent, must be the case that last part of this frame was sent, and
        so can bump up the SEQ for nak_first_seq.
      ---------------------------------------------------------------------*/
      ASSERT(last_seg == TRUE);
      nak_first_seq = DSRLPSEQ_GET_NEXT_SEQ(nak_first_seq,
                                        rrcb_ptr->cfg_ptr->seq_modulus);

      dsrlprtx_update_first_element(rrcb_ptr,
                                    nak_first_seq,
                                    nak_last_seq);
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
      Frame based operation, but not all bytes within this SEQ frame could
      be sent.  simply update the record.
    -----------------------------------------------------------------------*/
    nak_first_seq = DSRLPSEQ_ADD_S_SEQ(nak_first_seq, 
                                       seg_copy_len,
                                       modulus);
    if (DSRLPSEQ_HAS_SAME_SEQ(nak_first_seq,nak_last_seq) &&
        !DSRLPSEQ_HAS_S_SEQ(nak_last_seq))
    {
      nak_last_seq = DSRLPSEQ_ADD_S_SEQ( nak_last_seq,
                                         RLP3_NAK_LAST_S_SEQ_MAX,
                                         modulus);
    }

    dsrlprtx_update_first_element(rrcb_ptr,
                                  nak_first_seq,
                                  nak_last_seq);
  }

  /*-------------------------------------------------------------------------
    MuxPDU3 frames fill up the entire MuxPDU. So, RLP has to pad the item
    chain with zeros, and make sure the length of the item chain is 44 bytes.
 --------------------------------------------------------------------------*/
  if( mux_pdu == DSRLP_MUX_PDU_3 
#ifdef FEATURE_IS2000_REL_A_MUXPDU5
      || mux_pdu == DSRLP_MUX_PDU_5
#endif /*FEATURE_IS2000_REL_A_MUXPDU5*/
      )
  {
    /*-----------------------------------------------------------------------
      Figure out the number of zeros to be appended and store it in 
      bytes_to_copy.
    -----------------------------------------------------------------------*/
    bytes_to_copy = rev_fr_ptr->max_frame_size + 1
                    - (word)dsm_length_packet( fr_item_ptr);  

    /*-----------------------------------------------------------------------
      Increase used and add new dsm items at the end of the chain, till all
      the zeros are appended.
    -----------------------------------------------------------------------*/
    while ( bytes_to_copy > 0)
    {
      if( (curr_item_ptr->size  - curr_item_ptr->used) >= bytes_to_copy)
      {
        curr_item_ptr->used += bytes_to_copy;
        break;
      }
      bytes_to_copy       -= (curr_item_ptr->size  - curr_item_ptr->used);
      curr_item_ptr->used  = curr_item_ptr->size;

	  temp_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
      /*temp_item_ptr = dsm_new_buffer(DSM_DS_POOL_SIZE( 
                   (word) (bytes_to_copy >> DSRLPI_ITEM_SIZ_DIVISOR)));*/                                           
      if ( temp_item_ptr == NULL)
      {
        DATA_RLP_MSG0(MSG_LEGACY_HIGH, "Memory allocation failed");
        dsm_free_packet( &fr_item_ptr);
        return FALSE;
      }
      curr_item_ptr->pkt_ptr = temp_item_ptr;
      curr_item_ptr          = temp_item_ptr;
      memset( temp_item_ptr->data_ptr, 0, temp_item_ptr->size);
    }
  }

  /*-------------------------------------------------------------------------
    set the rest of the fields and return true
  -------------------------------------------------------------------------*/
  rev_fr_ptr->idle_frame   = FALSE;
  rev_fr_ptr->free_dsm_pkt = TRUE;
  rev_fr_ptr->fr_item_ptr  = fr_item_ptr;
  
  return TRUE;
} /* dsrlpi_rexmit_data_frame() */
#endif /* FEATURE_DS_RLP3 */
#endif /* MSM5000_IRAM_REV */
#endif /* RLP 3 || HDR defined */
