/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                R A D I O   L I N K   P R O T O C O L  I I I

GENERAL DESCRIPTION
  This file contains the implementation of third revision of the Radio Link
  Protocol (RLP Type 3), as defined in IS-707-A.  This file contains the 
  version specific implementation of frame processing for both receive and 
  transmit of RLP frames.  All functions in this file are internal to the
  RLP framework, and are called as function pointers from the generic RLP
  frame processing in dsrlpi.c.  Internal to the file is rlp3.

EXTERNALIZED FUNCTIONS

  rlp3_rx_frame_det()
    Determines the frame type of the received RLP frame, based on procedures 
    for revision 3 of the Radio Link Protocol, as specified in IS-707A.  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  RLP configuration and initialization procedures.

Copyright (c) 1999-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlp3.c_v   1.30   03 Dec 2002 16:35:16   jysong  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/1xrlp/src/dsrlp3.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---        -------------------------------------------------------
03/21/13   sd         Replaced assert for num_naks > 0 by graceful return in
                      dsrlp3i_bld_naktype0 and dsrlp3i_bld_naktype2
02/04/11   op         Data SU API cleanup
01/18/11   op         Migrated to MSG 2.0 Macros
10/14/10   sa         Adding debug messages before ASSERT.
07/04/09   pp         hton/ntoh* macros replaced with ps_hton/ntoh*.
05/10/07   ac         Lint fix
04/17/06   squ        Added code to send out CTL_ACK if the ack_flag is set.
                      This fixed the problem that rlp does not send ACK if 
                      rlp recv sync_ack and data frames at same time.
3/14/2006  squ        Replaced some ASSERTs with error checkings.
09/20/04   vas        Changes for QOS
08/31/04   atp        Fixed bug in rx_frame_det() when SEQ_HI present while
                      parsing MuxPDU5-FmtD combo.
06/11/04   vp         Removed inclusion of psglobal.h. Included ps_in.h.
06/08/04   atp        Support for additional MuxPDU5 format (len indicator 11)
                      introduced in IS2000 Rel C & D.
05/07/04   atp        First cut at supporting EVDV (IS707-B).
01/09/04   sy         Reverted dsm_pull16 change back to dsm_pullup and ntohs. 
12/16/03   rsl        Replaced dsm_pullup with dsm_pull16 in case of MUX PDU5.
                      Added debug messages.
03/07/03   js         Added RLP events. 
03/04/03   sy         Made changes for readability in the received nak frame
                      processing.Changes have no affect during run-time.
03/04/03   sy         Print correct info of the naks received from the BS.
03/04/03   sy         Fixed problem with the nak round when sending NAKS on
                      the reverse link.
12/03/03   js         Print out the current nak round when sending NAKS on
                      reverse link. 
11/12/02   js         Featurized F3 msgs identifying the SR_ID under 
                      FEATURE_HDR 
11/11/02   js         Fixed a few compile warnings. 
10/15/02   js         Optimized F3 messages by adding more information, 
                      removing unnecessary printouts.
07/15/02   ak         Added include psglobal.h under FEATURE_DATA_MM.
04/30/02   cc         Change some messages from ERR to MSG_HIGH for logging the 
                      reason of return(DSRLPI_RLP_ERASURE), as requested by AST.
03/15/02   snn        For MuxPDU5 added code to handle S_SEQ when SQI is not 
02/20/02   snn        While building Idle frames in Secondary service, we 
                      use a timer to decide to build IDLE or not. But, when
                      MS has only secondary service, this does not work. So
                      masked it off and a different algorithm will be
                      implemeted.
12/14/01   snn        Modified the code to have desired NAK_PER_ROUND as 
                      {3,3,3}. Note that this will not effect ordinary 
                      operation because, we will choose minimum of 
                      {default,desired} as working value.
10/18/01   snn        Changed the location for checking length field in the 
                      incoming data for segmented frames. This allows proper
                      handling of IDLE2 frames.
09/19/01   ak         Removed some ASSERTs from frame_det() and instead
                      return RLP_ERASURE
09/18/01   bkm        Removed rlp3_rx_frame_det from IRAM.
08/31/01   snn        Fixed NTOHS call for accessing 16 bit data for MuxPDU5
08/13/01   snn        Added support for MuxPDU5 in frame detection.
08/02/01   ak         Record mux_pdu type in curr_fr_info in frame_det.
07/30/01   sy         Handled the ASSERT's for DSM_NEW_BUFFER(). Moved the code
                      in rlp3_tx_idle_frame() to make sure that a new dsm small
                      item is created, before we change the counters to create a
                      new idle frame (to avoid some exteme cases).
07/19/01   ak/pjb     Use dsm_pullup in frame_det(), instead of manually
                      changing dsm's used and data_ptr fields.
06/27/01   sy         changed the rlp3_rx_idle_frames() to handle the 
                      zero nak_rounds case.
05/25/01   hcg        Made size of the transmit queue part of version specific
                      constants.
05/22/01   ak         bld_naktype0 no longer in IRAM
05/21/01   sy         changed the way how reset function is called.
05/09/01   igt        use dsm_length_packet() in rlp3_rx_frame_det to enable
                      handling of frames that consist of dsm item chains
02/27/01   ak         bld_naktype0 now in IRAM
02/21/01   ak         Support for checking against old frames and also for
                      potentially mis-RDA'd frames which cause big NAK gaps.
02/05/01   ak         No longer remove ack'd frames in Rx context (now done
                      in Tx context).
12/12/00   ak         Removed tx_naks from IRAM.
12/06/00   ak         Changes to force FILL frames every 80 frames so that
                      other side gets our V(N) appropriately.
10/04/00   ak         Added messages when sending naktype 2.
09/06/00   ak         Removed bld_naktype0 from REV_IRAM, as taking up too
                      much space.
08/25/00   rc         Changed dsm_new_buffer() calls to take pool id as input 
                      parameter.
08/02/00   na         Changed Format A frames to only look at 5bits of the
                      length field.
07/31/00   ak         Further IRAM #ifndefs for R-SCH.
06/12/00   hcg        Updated code per code review.
06/08/00   ak         Added MSM5000_IRAM_FWD #ifndef's for IRAM support.
06/05/00   ak         Frame determination now never returns DSRLPI_DONE, as 
                      RLP3 now needs to indicate to dsmgr if data was rxed in
                      the past 20 msec (for dormancy detection).
05/15/00   na/ak      Support for delayed frame detection.
05/10/00   ak         Support for sending fill frames.  If a NAK from the
                      BS passes CRC but has V(N) > our V(S), or requests
                      data that is greater than our Vs, then reset.  Cleaned 
                      up mux erasure count.  Modified tx_idle to account for
                      sending fills.  Added tx_fill().
05/02/00   na         If an element could not be added to the rexmit list,
                      frame determination now declares it DSRLPI_RLP_ERASURE.
04/28/00   igt        Update naks_received, rx_fill_fr_cnt statistics and also
                      remove resets++ before every call to dsrlp_reset().
04/20/00   ak         Added functions to handle mux erasures as a different
                      case than rlp erasures.  Detect null frames in frame
                      determination.
04/17/00   na         Added an extra check in rx_frame_det() for formatD frames
04/13/00   ak         Made some MSGs more screen friendly.  Now record
                      first 3 bytes of each frame in case frame is an 
                      erasure.  rlp_rx_erasure is better about info printed.
04/12/00   na         Changed DSRLP3_MAX_NAK constants to DSRLPI_MAX_NAK..
04/12/00   na         Fixed memory leak that happened when fill frames were
                      being received.
03/14/00   hcg        Fixed Lint errors in frame determination.  Removed 
                      init ctl frame NAK negotiation.  Added support for 
                      zero length data frames.
02/15/00   igt        Changed init of dsrlp3_constants to reflect new struct
01/31/00   na/igt     Fixed memory leak in NAK processing.
01/24/00   na         Fixed bug in rlp3_rx_fr_det that was adding extra 
                      entries in RTXQ on receiving a NAK with CRC error.
01/14/00   ak         Fixed offset calculation for SYNC/ACKs in fr_det.
01/11/00   ak         Made changes to frame det and tx_init, to account for
                      RLP_BLOB nak negotiation.
01/07/00   ak         If nak_insert fails, then do not put stuff into rsq
                      queue.
01/06/00   na         Fixed a bug where an ASSERT was being triggered in 
                      frame determination during a dsm_pullup.
08/05/99 na/hcg/ak    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "data_msg.h"

#ifdef FEATURE_DS_RLP3  
#include "dsm.h"
#include "dsrlpi.h"
#include "dsdvrlpi_rx.h"
#include "dsrlpseq.h"
#include "dsrlpnak.h"
#include "dsrlptxq.h"
#include "dsrlprtx.h"
#include "dsrlpdel.h"
#include "amssassert.h"
#include "bit.h"
#include "memory.h"
#include "dsrlp3.h"
#include "err.h"
#include "crc.h"

  #include "ps_in.h"
#include "rlp_event_defs.h"


/*===========================================================================
 
                        FORWARD DECLARATIONS
===========================================================================*/
dsrlpi_fr_det_enum_type rlp3_rx_frame_det
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  dsrlp_fwd_frame_type *fwd_frame_ptr,       /* ptr to Rx RLP frame        */
  dsrlpi_curr_fr_info_type *curr_fr_info_ptr /* ptr to frame specific info */
);
dsrlpi_fr_det_enum_type rlp3_rx_idle_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,     /* Ptr to RLP ctl block       */
  dsrlpi_curr_fr_info_type     *curr_info_ptr /* frame info ptr            */
);
boolean rlp3_tx_naks
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,  /* Ptr to RLP ctl block        */
  dsrlp_rev_frame_type         *rev_fr_ptr /* Ptr to frame to be built    */
);
boolean rlp3_tx_init_frame
(
   dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* ptr to RLP ctl block    */
   dsrlp_rev_frame_type         *rev_fr_ptr     /* ptr to frame to be built*/
);
boolean rlp3_tx_idle_frame
(
   dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,  /* Ptr to RLP ctl block        */
   dsrlp_rev_frame_type         *rev_fr_ptr /* Ptr to frame to be built    */
);
boolean rlp3_tx_fill_frame
(
   dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,  /* Ptr to RLP ctl block        */
   dsrlp_rev_frame_type         *rev_fr_ptr /* Ptr to frame to be built    */
);
dsrlpi_fr_det_enum_type rlp3_rx_rlp_erasure
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,         /* Ptr to RLP ctl block      */
  dsrlp_fwd_frame_type     *fwd_fr_ptr        /* ptr to received RLP frame */
);
dsrlpi_fr_det_enum_type rlp3_rx_mux_erasure
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,         /* Ptr to RLP ctl block      */
  dsrlp_fwd_frame_type     *fwd_fr_ptr        /* ptr to received RLP frame */
);
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  Function pointer table for all the RLP3 functions. A pointer to this goes
  into the version specific constants of the RLP control block.
---------------------------------------------------------------------------*/
struct dsrlpi_fcn_ptr_table rlp3_fcn_ptr_table =
{
  rlp3_rx_frame_det,                         /* rx_frame_det               */
  dsdvrlpi_rx_new_data,                      /* rx_new_data                */
  dsrlpi_process_rexmit_frame,               /* rx_rexmit                  */
  dsrlpi_rx_init_frame,                      /* rx_init                    */
  dsdvrlpi_rx_idle_frame,                    /* rx_idle                    */
  rlp3_rx_rlp_erasure,                       /* rx_rlp_erasure             */
  rlp3_rx_mux_erasure,                       /* rx_mux_erasure             */
  dsrlpi_tx_new_data_frame,                  /* tx_new_data                */
  dsrlpi_rexmit_data_frame,                  /* tx_rexmit                  */
  rlp3_tx_naks,                              /* tx_nak                     */
  rlp3_tx_init_frame,                        /* tx_init                    */
  rlp3_tx_idle_frame                         /* tx_idle                    */
  ,rlp3_tx_fill_frame                        /* tx_fill                    */
};

/*---------------------------------------------------------------------------
                            BIT #defines
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  bits in a byte.  Used in figuring out first set of padding in a control
  frame.
---------------------------------------------------------------------------*/
#define   BITS_IN_OCTET        (sizeof(byte) * 8)

/*===========================================================================
             R L P 3   C O N T R O L   F R A M E   D E F I N E S 
===========================================================================*/

/*---------------------------------------------------------------------------
  There are two control frames: Init (sync, sync/ack, and ack) and nak
  control frames.  These frames have the same first two fields: SEQ and
  CTL.  The following #defines define their lengths, and various bitmasks
  to use them.
  
        Common fields (SEQ & CTL) bitmasks:
---------------------------------------------------------------------------*/
#define RLP3_CTL_CTL_MASK                  0x3F /* mask for CTL field      */
#define RLP3_CTL_CTL_SYNC                  0x36 /* SYNC Frame              */ 
#define RLP3_CTL_CTL_ACK                   0x3A /* ACK Frame               */
#define RLP3_CTL_CTL_SYNC_ACK              0x3E /* SYNC/ACK Frame          */
#define RLP3_CTL_CTL_NAK                   0x30 /* NAK Frame               */
                                                                          
#define RLP3_CTL_CTL_SYNC_BIT_MASK         0x37 /* Detects SYNC bit        */
#define RLP3_CTL_CTL_ACK_BIT_MASK          0x3B /* Detects ACK bit         */

/*---------------------------------------------------------------------------
        Common fields (SEQ & CTL) bit lengths and starting positions:
---------------------------------------------------------------------------*/
#define RLP3_CTL_SEQ_LEN     8                 /* Sequence number field len*/
#define RLP3_CTL_CTL_LEN     6                 /* Control field length     */

                                              
#define RLP3_CTL_SEQ_POS     0                 /* Sequence number field pos*/

#define RLP3_CTL_CTL_POS     (RLP3_CTL_SEQ_POS + RLP3_CTL_SEQ_LEN)
                                               /* Control field position   */

/*--------------------------------------------------------------------------
   RLP 3 Control Frame FCS Field Length (bytes) 
--------------------------------------------------------------------------*/
#define RLP3_CTL_FCS_BYTE_LEN          2


/*--------------------------------------------------------------------------
   SYNC, SYNC/ACK, ACK FRAME FORMAT:

   RLP 3 FRAME FORMAT + BIT LENGTHS
   
   SEQ                8                                   
   
   CTL                6
   ENCRYPTION MODE    2
   
   RESET              1
   NAK_ROUNDS_FWD     3
   NAK_ROUNDS_REV     3
   
   NAK_ROUNDS_FWD     3x  (x <= 7) (21 bits max)
   
   NAK_ROUNDS_REV     3x  (x <= 7) (21 bits max)
   
   PADDING 1          x <= 7
   
   FCS                16
   
   PADDING 2          x (to fill out rest of frame)
--------------------------------------------------------------------------*/
                                                                           
/*--------------------------------------------------------------------------
   RLP 3 SYNC,SYNC/ACK,ACK Frame Bitmasks.  These are not shifted in any way.
--------------------------------------------------------------------------*/

#define   RLP3_SA_RESET               0x01      /* Reset Bit (assume on)  */
#define   RLP3_SA_NAK_PARAM_INC       0x00      /* nak param inc bit      */

/*--------------------------------------------------------------------------
   RLP 3 SYNC,SYNC/ACK,ACK Field Lengths (bits)
--------------------------------------------------------------------------*/
#define   RLP3_SA_RESET_LEN               1   /* Reset field length       */
#define   RLP3_SA_NAK_PARAM_INC_LEN       1   /* NAK PARAM inc length     */
                                               
#define   RLP3_SA_NAK_RNDS_FWD_LEN        3   /* Nak rounds fwd len       */
#define   RLP3_SA_NAK_RNDS_REV_LEN        3   /* Nak rounds rev len       */

#define   RLP3_SA_NAKS_PER_RND_FWD_LEN    3   /* naks per round fwd len   */
#define   RLP3_SA_NAKS_PER_RND_REV_LEN    3   /* naks per round rev len   */ 

/*--------------------------------------------------------------------------
   RLP 3 SYNC,SYNC/ACK,ACK Frame Field Bit Positions.  Assumes Encryption
   Mode is not supported.  Not all starting positions are listed, as the
   start positions become variable with variable number of naks-per-rnd
   fwd & rev.
--------------------------------------------------------------------------*/
#define   RLP3_SA_RESET_POS           (RLP3_CTL_CTL_POS + RLP3_CTL_CTL_LEN)
                                              /* Reset field pos          */
                                               
#define   RLP3_SA_NAK_PARAM_INC_POS   (RLP3_SA_RESET_POS + \
                                                         RLP3_SA_RESET_LEN)
                                              /* NAK_PARAM_INC pos        */

#define   RLP3_SA_NAKS_VIA_BLOB_FCS_POS (RLP3_SA_NAK_PARAM_INC_POS + \
                                                  RLP3_SA_NAK_PARAM_INC_LEN)
                          /* position of FCS when nak neg. via RLP_BLOB   */

#define   RLP3_SA_NAK_RNDS_FWD_POS    (RLP3_SA_NAK_PARAM_INC_POS + \
                                                 RLP3_SA_NAK_PARAM_INC_LEN)
                                              /* Nak rounds fwd pos       */

#define   RLP3_SA_NAK_RNDS_REV_POS    (RLP3_SA_NAK_RNDS_FWD_POS + \
                                                  RLP3_SA_NAK_RNDS_FWD_LEN)
                                              /* Nak rounds rev len       */

#define   RLP3_SA_NAKS_PER_RND_FWD_POS  (RLP3_SA_NAK_RNDS_REV_POS + \
                                                  RLP3_SA_NAK_RNDS_REV_LEN) 
                                              /* naks per round fwd len   */

/*--------------------------------------------------------------------------
   NAK FRAME FORMAT (Assumes either type '01' or type '10')

   RLP 3 FRAME FORMAT + BIT LENGTHS
   
   SEQ                8                                   
   
   CTL                6
   
   NAK_TYPE           2
   
   SEQ_HI             4    
       
TYPE 01:                         
   NAK_Map_Count      2

   Repeat 4 or less times:   
      NAK_Map_SEQ     12
      NAK_Map         8 
   
TYPE 10:
   NAK_SEG_COUNT      2
   
   Repeat 4 or less times:   
      FRAME_SEQ       12
      FIRST_S_SEQ     12
      LAST_S_SEQ      12   
   
   
   PADDING 1          x <= 7
   FCS                16
   PADDING 2          x (to fill out rest of frame)
--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
   RLP 3 NAK Frame Bitmasks.  These are not shifted in any way.
--------------------------------------------------------------------------*/
#define   RLP3_NAK_NAK_TYPE_0         0x00      /* NAK type 0  '00'       */
#define   RLP3_NAK_NAK_TYPE_1         0x01      /* NAK type 1  '01'       */
#define   RLP3_NAK_NAK_TYPE_2         0x02      /* NAK type 2  '10'       */
#define   RLP3_NAK_NAK_TYPE_3         0x03      /* NAK type 3  '11'       */

#define   RLP3_MAX_NAK_SEGS           4         /* maximum # of Type '10' */
                                                /* naks sent in a frame   */

/*--------------------------------------------------------------------------
   RLP 3 NAK Frame Field Lengths (bits)
--------------------------------------------------------------------------*/
#define   RLP3_NAK_NAK_TYPE_LEN            2  /* type of NAK len          */
#define   RLP3_NAK_SEQ_LEN                 8  /* SEQ field length         */
#define   RLP3_NAK_SEQ_HI_LEN              4  /* SEQ_HI field length      */
#define   RLP3_NAK_S_SEQ_LEN              12  /* S_SEQ field length       */

#define   RLP3_NAK_HOLE_CNT_LEN            2  /* # of NAK holes field len */
#define   RLP3_NAK_FIRST_LEN              12  /* seq # of first NAK'ed    */
#define   RLP3_NAK_LAST_LEN               12  /* seq # of last NAK'ed     */

#define   RLP3_NAK_MAP_CNT_LEN             2  /* # of NAK maps field len  */
#define   RLP3_NAK_MAP_SEQ_LEN            12  /* seq # of first NAK'ed    */
#define   RLP3_NAK_MAP_LEN                 8  /* length of NAK map        */

#define   RLP3_NAK_SEG_CNT_LEN             2  /* # NAK requests len       */
#define   RLP3_NAK_FRAME_SEQ_LEN          12  /* seq # of frame len       */
#define   RLP3_NAK_FIRST_S_SEQ_LEN        12  /* first octet count offset */
#define   RLP3_NAK_LAST_S_SEQ_LEN         12  /* last octet count offset  */

#define   RLP3_NAK_LENGTH_S_SEQ_LEN        8  /* octet length             */


/*--------------------------------------------------------------------------
   RLP 3 NAK Frame Field Positions (bits).  Some of the fields start in
   the same places, as the structure of the frame is highly variable.
--------------------------------------------------------------------------*/
#define   RLP3_NAK_NAK_TYPE_POS   (RLP3_CTL_CTL_POS + RLP3_CTL_CTL_LEN)
#define   RLP3_NAK_SEQ_HI_POS     (RLP3_NAK_NAK_TYPE_POS + \
                                                     RLP3_NAK_NAK_TYPE_LEN)

#define   RLP3_NAK_FIRST_POS      (RLP3_NAK_SEQ_HI_POS + RLP3_NAK_SEQ_HI_LEN)
#define   RLP3_NAK_LAST_POS       (RLP3_NAK_FIRST_POS + RLP3_NAK_FIRST_LEN)

#define   RLP3_NAK_MAP_CNT_POS    (RLP3_NAK_SEQ_HI_POS + RLP3_NAK_SEQ_HI_LEN)
#define   RLP3_NAK_SEG_CNT_POS    (RLP3_NAK_SEQ_HI_POS + RLP3_NAK_SEQ_HI_LEN)
#define   RLP3_NAK_HOLE_CNT_POS   (RLP3_NAK_SEQ_HI_POS + RLP3_NAK_SEQ_HI_LEN)


/*==========================================================================
                R L P 3  I D L E  F R A M E  D E F I N E S 
===========================================================================*/
/*---------------------------------------------------------------------------
                   RLP 3 IDLE and FILL FRAME BITMASKS
                   
  SEQ                       8
  CTL                       4
  SEQ_HI                    4
  Padding                   Variable (to octet align). Set to zero.                 
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
   RLP 3 Idle and Fill Frame CTL Field Bitmask.  For Idle frames, these bits 
   shall be set to '1010'. For transmit idle, the 4 bits are the top-half of
   a byte, so left shift 4 bits to get the value.  For Fill frames, these 
   bits shall be set to '1001'.  For received frames, the values are actually
   shifted to account for their positions within the extracted data.
---------------------------------------------------------------------------*/
#define RLP3_IDLEFILL_CTL_MASK             0x3C
#define RLP3_IDLE_CTL_FLD                  0x28
#define RLP3_IDLE_CTL_FLD_SHIFTED          0xA0
#define RLP3_FILL_CTL_FLD                  0x24
#define RLP3_FILL_CTL_FLD_SHIFTED          0x90


/*===========================================================================

            EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  All the constants for RLP3. This includes function pointers, and other
  numeric constants that are version specific.
---------------------------------------------------------------------------*/
dsrlpi_ver_const_type dsrlp3_constants =
{
  &rlp3_fcn_ptr_table,                       /* Ptr to function table      */
  DSRLP3_MAX_CONS_ERASURES,                  /* Max conseq. erasures       */

  DSRLP3_TXQ_SIZE,                           /* Size of transmit queue     */
  DSRLP_MAX_NAK_ROUNDS,                      /* Max fwd rounds of NAKs     */
  DSRLP_MAX_NAK_ROUNDS,                      /* Max rev rounds of NAKs     */
  
  DSRLP_MAX_NAK_ROUNDS,                      /* Desired fwd rounds of NAKs */
  {3, 3, 3},                                 /* Desired NAKs per round fwd */ 
  DSRLP_MAX_NAK_ROUNDS,                      /* Desired rev rounds of NAKs */
  {3, 3 ,3},                                 /* Desired NAKs per round rev.*/ 

  TRUE                                       
};
/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/


/*===========================================================================

FUNCTION       DSRLP3I_BLD_NAKTYPE0

DESCRIPTION    This function builds a NAK Type '00' frame.  This function
               does not build the entire frame, but just the part which
               consists of the nak range, which is the 12-bit FIRST and
               LAST fields.
               
               This code is specific to RLP 3.
               
DEPENDENCIES   Input pointers are non-NULL.

RETURN VALUE   NONE

SIDE EFFECTS   As each nak entry's info is put into the NAK frame, that
               particular NAK entry will have its 'naks left to send this
               round' decremented.  That may result in the NAK entry
               being removed from the nak list, if it has expired.
===========================================================================*/
boolean dsrlp3i_bld_naktype0
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  byte                         *data_ptr,   /* ptr to frame data           */
  word                         *bits_left   /* bits remaining in frame     */ 
)
{
  uint32                        nak_id;     /* id of nak entry             */
  uint32                        seq;        /* sequence number             */
  byte                          num_naks;   /* how many nak sequences done */
  word                          bit_pos;    /* current bit position        */
  word                          bits_per_nak; /* bits used per nak range   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT((rrcb_ptr  != NULL) &&
         (data_ptr  != NULL) &&
         (bits_left != NULL));

  /*-------------------------------------------------------------------------
    Calculate how many bits are put in per unsegmented nak range entry.  Use
    this when determining if there is enough room left in the frame for
    another NAK
  -------------------------------------------------------------------------*/
  bits_per_nak = RLP3_NAK_FIRST_LEN + 
                 RLP3_NAK_LAST_LEN;

  num_naks = 0;
  nak_id   = 0;
  bit_pos  = RLP3_NAK_HOLE_CNT_POS + RLP3_NAK_HOLE_CNT_LEN;

  /*-------------------------------------------------------------------------
    The nak hole count is inserted at the end, but reserve space up front.
  -------------------------------------------------------------------------*/
  *bits_left = *bits_left - RLP3_NAK_HOLE_CNT_LEN;

  DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Sending Unseg NAKS. RLP:%d",rrcb_ptr->cfg_ptr->sr_id);

  while ((*bits_left) > bits_per_nak)
  {
    /*-----------------------------------------------------------------------
      Get a nak_id and put data into frame
    -----------------------------------------------------------------------*/
    nak_id = dsrlpnak_get_nak(rrcb_ptr,nak_id,FALSE);

    if ((nak_id == 0) || (num_naks == RLP3_MAX_NAK_SEGS))
    {
      break;  /* no more unsegmented NAKs to send */ 
    }

    num_naks++;

    /*-----------------------------------------------------------------------
      Insert the 12-bit SEQ_HI + SEQ for the first seq
    -----------------------------------------------------------------------*/
    seq = DSRLPNAK_GET_FIRST_SEQ(nak_id);
    seq = DSRLPSEQ_GET_12BIT_FRAME_SEQ(seq);

    b_packw((word)seq,
            data_ptr,
            bit_pos,
            RLP3_NAK_FIRST_LEN
           );
    bit_pos += RLP3_NAK_FIRST_LEN;

    /*-----------------------------------------------------------------------
      Insert the 12-bit SEQ_HI + SEQ for the last seq.  Decrement the value
      as the nak list stores it as an exclusive value.
    -----------------------------------------------------------------------*/
    seq = DSRLPNAK_GET_LAST_SEQ(nak_id);
    seq = (word)(DSRLPSEQ_GET_12BIT_FRAME_SEQ(seq) - 1);

    b_packw((word)seq,
            data_ptr,
            bit_pos,
            RLP3_NAK_LAST_LEN
           );
    bit_pos += RLP3_NAK_LAST_LEN;

    (*bits_left)  = (*bits_left) - bits_per_nak;

    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "NAK Round:%d: %x -> %x",
	     ((dsrlpi_nak_list_entry_type *)nak_id)->curr_rnd,
             DSRLPNAK_GET_FIRST_SEQ(nak_id),
             DSRLPNAK_GET_LAST_SEQ(nak_id));

    /*------------------------------------------------------------------------
      Let the nak list know that this entry was taken care of.
    ------------------------------------------------------------------------*/
    dsrlpnak_nak_was_sent(rrcb_ptr,nak_id);
  } /* while */

  if(num_naks == 0)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                  "Graceful return as num_naks is not greater than 0");
    return FALSE;
  }
  
  ASSERT(num_naks <= RLP3_MAX_NAK_SEGS);

  /*-------------------------------------------------------------------------
    Decrement num_naks, so that it conforms to IS-707 standard when inserted.
    Then insert into the frame.
  -------------------------------------------------------------------------*/
  num_naks--;

  b_packb(num_naks,
          data_ptr,
          RLP3_NAK_HOLE_CNT_POS,
          RLP3_NAK_HOLE_CNT_LEN);
  
  return TRUE;
}

/*===========================================================================

FUNCTION       DSRLP3I_BLD_NAKTYPE2

DESCRIPTION    This function builds a NAK Type '10' frame.  This function
               does not build the entire frame, but just the part which
               consists of the nak byte sequence ranges.
               
               This code is specific to RLP 3.
               
DEPENDENCIES   Input pointers are non-NULL.

RETURN VALUE   NONE

SIDE EFFECTS   As each nak entry's info is put into the NAK frame, that
               particular NAK entry will have its 'naks left to send this
               round' decremented.  That may result in the NAK entry
               being removed from the nak list, if it has expired.
===========================================================================*/
boolean dsrlp3i_bld_naktype2
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,   /* Ptr to RLP ctl block        */
  byte                         *data_ptr,   /* ptr to frame data           */
  word                         *bits_left   /* bits remaining in frame     */ 
)
{
  uint32                        nak_id;     /* id of nak entry             */
  byte                          num_naks;   /* how many nak sequences done */
  word                          bits_per_nak; /* bits used per nak range   */
  uint32                        seq;        /* sequence number             */
  word                          bit_pos;    /* current bit position        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT((rrcb_ptr  != NULL) &&
         (data_ptr  != NULL) &&
         (bits_left != NULL));

  /*-------------------------------------------------------------------------
    Calculate how many bits are put in per segmented nak entry.  Use this
    when determining if there is enough room left in the frame for another
    NAK
  -------------------------------------------------------------------------*/
  bits_per_nak = RLP3_NAK_FRAME_SEQ_LEN     + 
                 RLP3_NAK_FIRST_S_SEQ_LEN   +
                 RLP3_NAK_LAST_S_SEQ_LEN;

  num_naks = 0;
  nak_id   = 0;
  bit_pos  = RLP3_NAK_SEG_CNT_POS + RLP3_NAK_SEG_CNT_LEN;

  /*-------------------------------------------------------------------------
    The nak seg count is inserted at the end, but reserve space up front.
  -------------------------------------------------------------------------*/
  *bits_left = *bits_left - RLP3_NAK_SEG_CNT_LEN;

  DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Sending SEG NAKS:RLP:%d",rrcb_ptr->cfg_ptr->sr_id);

  while ((*bits_left) > bits_per_nak)
  {
    /*-----------------------------------------------------------------------
      Get a nak_id and put data into frame
    -----------------------------------------------------------------------*/
    nak_id = dsrlpnak_get_nak(rrcb_ptr,nak_id,TRUE);

    if ((nak_id == 0) || (num_naks == RLP3_MAX_NAK_SEGS))
    {
      break;  /* no more unsegmented NAKs to send */ 
    }

    num_naks++;

    /*-----------------------------------------------------------------------
      Insert the 12-bit Frame Sequence number
    -----------------------------------------------------------------------*/
    seq = DSRLPNAK_GET_FIRST_SEQ(nak_id);
    seq = DSRLPSEQ_GET_12BIT_FRAME_SEQ(seq);

    b_packw((word)seq,
            data_ptr,
            bit_pos,
            RLP3_NAK_FRAME_SEQ_LEN
           );
    bit_pos += RLP3_NAK_FRAME_SEQ_LEN;

    /*-----------------------------------------------------------------------
      Insert the first 12-bit S_SEQ
    -----------------------------------------------------------------------*/
    seq = DSRLPNAK_GET_FIRST_SEQ(nak_id);
    seq = DSRLPSEQ_GET_S_SEQ(seq);

    b_packw((word)seq,
            data_ptr,
            bit_pos,
            RLP3_NAK_FIRST_S_SEQ_LEN
           );
    bit_pos += RLP3_NAK_FIRST_S_SEQ_LEN;

    /*-----------------------------------------------------------------------
      Insert the last 12-bit S_SEQ.  Decrement the value as the last seq
      is an exclusive value.  Note that if the entry defined something
      like 4a -> 5, the decrement on the last_seq's S_SEQ would automatically
      make it 0xFFFF.  This value, unsigned, is 4095, which signifies that
      bytes 4a -> end of frame are missing.  The b_packw would then only
      insert 0xFFF (12 bits) into the data_ptr, due to the input param.
    -----------------------------------------------------------------------*/
    seq = DSRLPNAK_GET_LAST_SEQ(nak_id);
    seq = (word)(DSRLPSEQ_GET_S_SEQ(seq) - 1);

    b_packw((word)seq,
            data_ptr,
            bit_pos,
            RLP3_NAK_LAST_S_SEQ_LEN
           );
    bit_pos += RLP3_NAK_FIRST_S_SEQ_LEN;

    (*bits_left)  = (*bits_left) - bits_per_nak;

    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "NAK Round:%d: %x -> %x",
	     ((dsrlpi_nak_list_entry_type *)nak_id)->curr_rnd,
             DSRLPNAK_GET_FIRST_SEQ(nak_id),
             DSRLPNAK_GET_LAST_SEQ(nak_id));


    /*-----------------------------------------------------------------------
      Let the nak list know that this entry was taken care of.
    -----------------------------------------------------------------------*/
    dsrlpnak_nak_was_sent(rrcb_ptr,nak_id);
  } /* while */

  if(num_naks == 0)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,
                  "Graceful return as num_naks is not greater than 0");
    return FALSE;
  }

  ASSERT(num_naks <= RLP3_MAX_NAK_SEGS);

  /*-------------------------------------------------------------------------
    Decrement num_naks, so that it conforms to IS-707 standard when inserted.
    Then insert into the frame.
  -------------------------------------------------------------------------*/
  num_naks--;

  b_packb(num_naks,
          data_ptr,
          RLP3_NAK_SEG_CNT_POS,
          RLP3_NAK_SEG_CNT_LEN);

  return TRUE;
}

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION DSRLP3_CONSTANTS_SET_TXQ_SIZE

DESCRIPTION
  Sets the txq_size of the dsrlp3_constants

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void dsrlp3_constants_set_txq_size(uint16 txq_size)
{
    dsrlp3_constants.txq_size = txq_size;
}

/*===========================================================================

FUNCTION RLP3_RX_FRAME_DET

DESCRIPTION
  This function determines the frame type of the received RLP frame, based 
  on procedures for revision 3 of the Radio Link Protocol, as specified in 
  IS-707A.  Based on the MuxPDU type, the function extracts the necessary
  bit fields in the RLP frame, and uses these fields to determine the exact
  type of frame that was received.  Note that the 1/8th rate idle frame 
  determination is already performed by the "generic" receive frame 
  determination function; the caller of this function.  This function also
  processes NAK frames, and queues up any NAK'ed frames for retransmission.

DEPENDENCIES
  None.

RETURN VALUE
  An enumerated value representing the CDMA frame type for the RLP RX frame.
  The frame types are defined in dsrlpi_fr_det_enum_type.

SIDE EFFECTS
  None.
===========================================================================*/
dsrlpi_fr_det_enum_type rlp3_rx_frame_det
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,        /* current RLP ctl blk        */
  dsrlp_fwd_frame_type *fwd_frame_ptr,       /* ptr to Rx RLP frame        */
  dsrlpi_curr_fr_info_type *curr_fr_info_ptr /* ptr to frame specific info */
)
{
  byte cnt;           /* number of bytes removed from packet chain         */
  byte ctl_byte;      /* temp variable to store byte containing ctrl field */
  byte type_bits;     /* contains type bits used in frame determination    */
  byte type_mask;     /* temp variable to store mask used for type bits    */
  byte fr_seq_num;    /* byte to store extracted 8-bit OTA sequence number */
  byte length;        /* byte used to store length of payload frame field  */
  byte tmp_byte;      /* temp variable to store extracted bits             */
  byte seq_hi=0x0;    /* contains the 4-bit SEQ_HI sequence number field   */
  byte nak_type;      /* format of received NAK                            */
  byte bit_pos;       /* current bit position in NAK map                   */
  byte mask;          /* tmp mask used in counting bits set in NAK map     */
  byte nak_map_byte;  /* extracted byte that contains the NAK map          */
  byte nak_count;     /* number of NAK occurrences contained in the frame  */
  byte   nak_param_inc; /* holds the nak_param_inc bit from init msgs      */
  byte   naks_inserted_in_rtxq=0;  /* # of NAKs inserted in the RTXQ       */
  word offset;        /* offset into packet chain to extract last byte     */
  int  i;             /* local looping index                               */
  uint16 s_seq;       /* contains bytes of the 12-bit S_SEQ field          */
  uint16 first_s_seq; /* temporary storage for first 12-bit S_SEQ number   */
  uint16 last_s_seq;  /* temporary storage for last 12-bit S_SEQ number    */
  uint32 nak_seq = 0; /* temporary storage for base NAK seq number         */
  uint32 tmp_seq = 0; /* temporary storage for building up seq numbers     */
  uint32 nak_first_seq = 0; /* tmp storage for bldg 32-bit FIRST/LAST pairs*/
  uint32 nak_last_seq = 0; /* tmp storage for bldg 32-bit FIRST/LAST pairs */
  dsrlp_mux_pdu_enum_type  mux_pdu_type;                    /* MuxPDU type */
  dsrlp_srvc_enum_type srvc_type; /* service type - Primary or Secondary   */
  boolean resync_rlp = FALSE; /* should rlp be reset?                      */
  byte  len_indicator=0;      /* Indicates number of bits in pdu5_length   */
  uint16 pdu5_temp;           /* temporary variable to hold pdu5 data      */
  uint16  pdu5_len = 0 ;      /* Length of the PDU 5                       */

  event_rlp_payload_type   rlp_event_payload; /* payload info for RLP evt  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Perform frame determination based on MuxPDU type, rate, type bits, and
    CTL field.  Attempt to perform "common case" optimization by checking
    against full rate data frames first, and then processing the remaining 
    frame categories.
   ------------------------------------------------------------------------*/
   curr_fr_info_ptr->mux_pdu_type = fwd_frame_ptr->mux_pdu_type;
   curr_fr_info_ptr->fr_cat = 0;

   if( fwd_frame_ptr->fr_item_ptr == NULL)
   {
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Malformed frame, fwd_frame_ptr->fr_item_ptr = NULL");
      return( DSRLPI_RLP_ERASURE);
   }
  /*-------------------------------------------------------------------------
    Record the first three bytes of the packet just in case pkt does
    not parse correctly.  Safe to get first 3 bytes, as data_ptr array
    is at least 44 bytes long.
  ------------------------------------------------------------------------*/
  dsrlpi_ctl_byte1 = fwd_frame_ptr->fr_item_ptr->data_ptr[0];
  dsrlpi_ctl_byte2 = fwd_frame_ptr->fr_item_ptr->data_ptr[1];
  dsrlpi_ctl_byte3 = fwd_frame_ptr->fr_item_ptr->data_ptr[2];
  dsrlpi_ctl_used = (byte)fwd_frame_ptr->fr_item_ptr->used;

  /*-------------------------------------------------------------------------
    1/8 rate NULLS are a special case, as they are erasures, but they are
    not really bad frames, since that is what the base station wanted to 
    send.  If in Sync state, return DSRLPI_NULL, otherwise treat as an
    actual erasure.  This helps in reducing error msgs printed out.
  -------------------------------------------------------------------------*/
  if ((dsrlpi_ctl_used == 2)     &&
      (dsrlpi_ctl_byte1 == 0xFF) &&
      (dsrlpi_ctl_byte2 == 0xFF)
     )
  {
    if (rrcb_ptr->state.state == DSRLPI_SYNC_STATE)
    {
      return(DSRLPI_NULL);
    }
    else
    {
      
	  return(DSRLPI_RLP_ERASURE);
    }
  }

  /*-------------------------------------------------------------------------
    Perform common case optimization.  Determine if this is a full-rate 
    data frame.  If so, extract the relevant information and return the 
    determined frame type.  Otherwise drop through and continue normal 
    processing of the frame.
  -------------------------------------------------------------------------*/
  if (fwd_frame_ptr->fr_rate == DSRLP_RATE_1)
  {
    /*=======================================================================
      For Rate 1, frames can either be 1) a format B data frame, 2) a 
      format C data frame, 3) a format D data frame, or 4) a format A control
      or data frame.  If the full rate frame is not determined to be of one 
      of these four types, the frame is invalid, and marked as an RLP 
      erasure.  If the frame is of format B, C, or D, then we know it is a 
      data frame (since that is the only option for these formats) and we 
      can return from frame determination after removing the appropriate 
      bits and performing any relevant processing, including sequence number
      generation.  For format A, we need to continue frame determination, as 
      it can either be a control or data frame.  
     ======================================================================*/
        
    /*-----------------------------------------------------------------------
      Initial determination based on the "type" bits.  Type bits are located 
      at the end of the frame for MuxPDU type 1 and 2, and at the beginning 
      of the frame for MuxPDU type 3.  Individual type bit positions within 
      the frame vary based on rate and service type.  Depending on which 
      service type, mask the "type" bits to determine frame type.  
     ----------------------------------------------------------------------*/
    mux_pdu_type = fwd_frame_ptr->mux_pdu_type;
    if (mux_pdu_type == DSRLP_MUX_PDU_1 || mux_pdu_type == DSRLP_MUX_PDU_2)
    {
      /*---------------------------------------------------------------------
        MuxPDU type 1 and 2, remove from the end of the frame.  The type
        bits are in the most significant bit positions in the byte.
       --------------------------------------------------------------------*/
      cnt = (byte)dsm_pullup_tail(&(fwd_frame_ptr->fr_item_ptr), 
                                  &type_bits, (word) sizeof(byte));

      /*---------------------------------------------------------------------
        Guard against pullup returning NULL ptr.
       --------------------------------------------------------------------*/
      if( fwd_frame_ptr->fr_item_ptr == NULL || cnt != sizeof(byte) )
      {
		
        return( DSRLPI_RLP_ERASURE);
      }
    }
    else if (mux_pdu_type == DSRLP_MUX_PDU_3)
    {
      /*---------------------------------------------------------------------
        MuxPDU type 3, the type bits are in the first byte of the frame 
        returned by the mux layer (the byte is padded out).  Type bits are
        in the least significant bit positions of the byte.
       ---------------------------------------------------------------------*/
      cnt = (byte)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &type_bits, 1);   

      /*---------------------------------------------------------------------
        Guard against pullup returning NULL ptr.
       --------------------------------------------------------------------*/
      if( fwd_frame_ptr->fr_item_ptr == NULL || cnt != sizeof(byte) )
      {
	   
        return( DSRLPI_RLP_ERASURE);
      }
    }
    else if (mux_pdu_type == DSRLP_MUX_PDU_5)
    {
      /*--------------------------------------------------------------------
        MuxPDU type 5 structure looks as follows:
        The structure is:
         -------------------------------------------------------
        |  sr_id   | Rsvd   | Length     | Length  | Type bits  |
        |  3 bits  |  1 bit |  Indicator |  0/8/16 |   2 bits   |
        |          |        |  2 bits    |         |            |
        ---------------------------------------------------------
        
            Length Indicator:   2 bits
            Length:             0/8/16 based on Length Indicator
            Data:               The type bits are the first two bits of the 
                                data frame 
       Read the Length Indicator from the incoming packet.
      --------------------------------------------------------------------*/
      cnt = (byte)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), 
                              &len_indicator, 1);
      /*---------------------------------------------------------------------
        Guard against pullup returning NULL ptr.
       --------------------------------------------------------------------*/
      if( fwd_frame_ptr->fr_item_ptr == NULL || cnt != sizeof(byte) )
      {
	    
        return( DSRLPI_RLP_ERASURE);
      }     

      /*---------------------------------------------------------------------
       If length indicator == 00000000 (binary),
          Then we have fixed length. 
       else if (length indicator == 00000100) binary ) i.e. 01
          Then we have 8 bits length
          Read the total length.
       else if (length indicator == 00001000)   (binary) i.e. 10
          Then we have 16 bits length
          Read the total length
       else if (length indicator == 11)   
          Reserved. We should not get.; 
          Printout a message and ASSERT()
       
       Read the type bits
      ----------------------------------------------------------------------*/
      if ((len_indicator & RLP3_MUXPDU5_LEN_IND_MASK) == 0x00)
      {
          /*-----------------------------------------------------------------
           If length indicator == 00, then we assume that the length of the
           packet is same as that in the used field. So, we donot trim the
           packet (as we do in other case)
          -----------------------------------------------------------------*/
          DATA_RLP_MSG1(MSG_LEGACY_LOW, "Received frame with Len Indicator 00 RLP:%d ",rrcb_ptr->cfg_ptr->sr_id);
          type_bits = len_indicator & 0x03;
          ASSERT(cnt == sizeof(byte));      
      }
      else if ((len_indicator & RLP3_MUXPDU5_LEN_IND_MASK) == 0x04)
      {

          /*-----------------------------------------------------------------
            The structure now is:
                   ----------------------------------------------
                  |  sr_id   | Rsvd   | Length     | MS bits of  |
            1st   |          |        |  Indicator | Length      |
           Byte   | 3 bits   | 1 bit  |  2 bits    | 2 bits      |   
                  ------------------------------------------------
                  ------------------------------------------------
           2nd   |  LS Bits of the Length     | Type Bits        |
          Byte   |         6 bits             | 2 bits           |   
                 ------------------------------------------------
            
          
            In len_indicator 2 LSBits represent the first 2 bits of the 
            PDU 5 length. 
            Read the next 6 bits to get the length. Instead, read 8 bits 
            and then obtain the 6 bits of length and 2 bits of type field.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG1(MSG_LEGACY_LOW, "Received frame with Len Indicator 01 RLP: %d ",rrcb_ptr->cfg_ptr->sr_id);
          cnt = (byte)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &pdu5_len, 1);
          ASSERT(cnt == sizeof(byte));      
          
          /*-----------------------------------------------------------------
            Since LS 2 bits of the pdu5_len are type bits obtain them.
          -----------------------------------------------------------------*/
          type_bits = pdu5_len & 0x03;
          
          /*-----------------------------------------------------------------
            Now obtain the length of the PDU5
            Since last two bits of pdu5_len are type bits, remove them.
            Then append the last two bits of len_indicator to pdu5_len
          -----------------------------------------------------------------*/
          pdu5_len = (pdu5_len >> 2);
          pdu5_len |= ((len_indicator << 6) & 0xFF);

         //needs to be removed when mux is fixed to get rid mux headers from frames passed to RLP.
         DATA_RLP_MSG2(MSG_LEGACY_LOW, "Length received is %d RLP:%d",pdu5_len,rrcb_ptr->cfg_ptr->sr_id);
         dsm_trim_packet( &(fwd_frame_ptr->fr_item_ptr), pdu5_len);
      }
      else if ((len_indicator & RLP3_MUXPDU5_LEN_IND_MASK) == 0x08)
      {
          /*-----------------------------------------------------------------
            The structure now is:
                   ----------------------------------------------
                  |  sr_id   | Rsvd   | Length     | MS bits of  |
            1st   |          |        |  Indicator | Length      |
           Byte   | 3 bits   | 1 bit  |  2 bits    | 2 bits      |   
                  -----------------------------------------------
                   ----------------------------------------------
            2nd   |  8 Bits of the Length                        |
          Byte   |         8 bits                                |   
                 ------------------------------------------------
            
                  -----------------------------------------------
           3rd   |  6 Bits of the Length     | Type Bits         |
         Byte   |         6 bits             | 2 bits            |   
                ------------------------------------------------
          
            In len_indicator 2 LSBits represent the first 2 bits of the 
            PDU 5 length. 
            Read the next 8 bits
            Read the next 6 bits to get the length. Instead, read 8 bits 
            and then obtain the 6 bits of length and 2 bits of type field.
          -----------------------------------------------------------------*/
          DATA_RLP_MSG1(MSG_LEGACY_LOW, "Received frame with Len Indicator 10 RLP: %d ",rrcb_ptr->cfg_ptr->sr_id); 
          
		  /*-----------------------------------------------------------------
		  Use dsm_pullup and ntohs to get pdu5_len in the right format (Host address 
		  format)
		  ------------------------------------------------------------------*/
		  cnt = (byte)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &pdu5_len, 2);
          ASSERT(cnt == (sizeof(byte) * 2)); 
          
		  
          /*-----------------------------------------------------------------
            The type bits are to be extracted from a 16bits.
          -----------------------------------------------------------------*/
          pdu5_temp = pdu5_len =  ps_ntohs(pdu5_len);
          type_bits = ((pdu5_temp) & 0x03);
          
          /*-----------------------------------------------------------------
            Now obtain the length of the PDU5
            Last two bits of Length indicator byte have MSBits of length. 
            Obtain them.
          -----------------------------------------------------------------*/
          pdu5_temp = (len_indicator & 0x03); 

          /*-----------------------------------------------------------------
           Now copy the last two bits of pdu5_temp to upper most two bits 
           of pdu5_len. So, shift pdu5_temp 14 to left, so that LSB 2 bits
           become MSB 2 bits.
           
           Clear the MSB2-bits of pdu5_len. This prevents any doubts about 
           state of the MSB bits, so AND with 0x3FFF
           
           Then OR it with pdu5_len so upper most bits of pdu5 len become 
           MSB bits.
          -----------------------------------------------------------------*/
          pdu5_len = (pdu5_len >> 2);
          pdu5_len &= 0x3FFF;
          pdu5_len |= (pdu5_temp << 14);

          DATA_RLP_MSG3(MSG_LEGACY_LOW, "For Len_ind 10, Len rxed is %d,Mux:%d RLP:%d",
                   pdu5_len,
                   fwd_frame_ptr->fr_item_ptr->used,
                   rrcb_ptr->cfg_ptr->sr_id);
          dsm_trim_packet( &(fwd_frame_ptr->fr_item_ptr), pdu5_len); 
      }
      else if ((len_indicator & RLP3_MUXPDU5_LEN_IND_MASK) == 0x0c )
      {
          /*-----------------------------------------------------------------
           If length indicator == 11, then we assume that the length of the
           packet is same as that in the used field. So, we donot trim the
           packet (as we do in other case)
          -----------------------------------------------------------------*/
          DATA_RLP_MSG1(MSG_LEGACY_LOW, "Received frame with Len Indicator 11 RLP:%d ",rrcb_ptr->cfg_ptr->sr_id);
          type_bits = len_indicator & 0x03;
          ASSERT(cnt == sizeof(byte));      
      }
      else
      {
          DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Invalid Length Indicator RLP: %d",rrcb_ptr->cfg_ptr->sr_id);
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
          
          return(DSRLPI_RLP_ERASURE);
      }
    }
    else
    {
        /*-----------------------------------------------------------------
          We received an unknown MUX PDU. Not acceptable. ASSERT
        -----------------------------------------------------------------*/
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Invalid PDU Type");
        dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
        DATA_RLP_MSG0(MSG_LEGACY_LOW, "return RLP_ERASURE");
        return(DSRLPI_RLP_ERASURE);
    }

    srvc_type = fwd_frame_ptr->srvc_type;
        
    /*-----------------------------------------------------------------------
      Determine if it is a format B/C new data frame.  If so, extract the 
      frame sequence number, calculate the internal sequence number and 
      return.
     ----------------------------------------------------------------------*/
    type_mask = dsrlpi_type_mask_table[mux_pdu_type][srvc_type].type_mask;

    if( (type_bits & type_mask) 
             == dsrlpi_type_mask_table[mux_pdu_type][srvc_type].format_bc_new
      )
    {
      /*---------------------------------------------------------------------
        The frame is a format B/C new data frame.  Extract, calculate, and
        store the frame sequence number for subsequent processing.  Note, 
        the 8-bit OTA sequence number is always the first byte of the frame, 
        and is now removed from the frame payload.
        -------------------------------------------------------------------*/
      (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &fr_seq_num, 1);
      curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_MASK;      

      /*---------------------------------------------------------------------
        Guard against pullup returning NULL ptr.
       --------------------------------------------------------------------*/
      if( fwd_frame_ptr->fr_item_ptr == NULL )
      {
    	
        return( DSRLPI_RLP_ERASURE);
      }

      /*---------------------------------------------------------------------
        Now calculate the frame sequence number using internal format.
       --------------------------------------------------------------------*/
      DSRLPSEQ_GEN_FROM_SEQ( curr_fr_info_ptr->seq_num, 
                             fr_seq_num, 
                             rrcb_ptr->state.expected_seq_num, 
                             rrcb_ptr->cfg_ptr->seq_modulus);
      return (DSRLPI_NEW);
    } /* new format B/C data frame */
          
    /*-----------------------------------------------------------------------
      Determine if it is a format B/C re-transmitted data frame.  If so, 
      extract the frame sequence number, create the internal sequence number,
      and return.
      ---------------------------------------------------------------------*/
    if( (type_bits & type_mask) 
          == dsrlpi_type_mask_table[mux_pdu_type][srvc_type].format_bc_rexmit
      )
    {
      /*---------------------------------------------------------------------
        The frame is a format B/C retransmitted data frame.  Extract, 
        obtain, and store the frame sequence number for subsequent 
        processing.  Note, the 8-bit OTA sequence number is always the first 
        byte of the frame, and is now removed from the frame payload.  Also 
        indicate that the sequence number contains only the 8-bit SEQ field.
        -------------------------------------------------------------------*/
      (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &fr_seq_num, 1);
      curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_MASK;      

      /*---------------------------------------------------------------------
        Guard against pullup returning NULL ptr.
       --------------------------------------------------------------------*/
      if( fwd_frame_ptr->fr_item_ptr == NULL )
      {
    	DATA_RLP_MSG0(MSG_LEGACY_LOW, "return RLP_ERASURE");
        return( DSRLPI_RLP_ERASURE);
      }

      /*---------------------------------------------------------------------
        Generate the internal frame sequence number from the 8-bit OTA SEQ 
        field.  Matching against the NAK list entries will be performed as 
        part of the retransmit frame processing.  
       --------------------------------------------------------------------*/
      DSRLPSEQ_PUT_SEQ(curr_fr_info_ptr->seq_num, fr_seq_num);
      return (DSRLPI_REXMIT);
    } /* retransmitted format B/C data frame */
      
    /*-----------------------------------------------------------------------
      The full rate frame is neither format B or format C.  Determine if it
      is a format D data frame.  If so, extract the bit fields, and perform
      appropriate processing according to 4.2.6.
      MuxPDU5 does not have Length field, so check for MuxPDU5 while 
      extracting the length from incoming packet.
      ---------------------------------------------------------------------*/
    if( ((mux_pdu_type == DSRLP_MUX_PDU_3) || 
         (mux_pdu_type == DSRLP_MUX_PDU_5)) 
                      && 
        ((type_bits & type_mask) 
                == dsrlpi_type_mask_table[mux_pdu_type][srvc_type].format_d)
      )
    {
      /*=====================================================================
        The frame is a format D data frame.  Extract, obtain, and store the 
        frame bit fields, and use for processing.  

                          format D data frame header for MuxPDU3
        -----------------------------------------------------------------
        |     |     | S | S | L | R |        |         |         | P |
        | 00  | SEQ | S | Q | S | T | Length |  SEQ_HI |  S_SEQ  | A |
        |     |     | P | I | T | X |        |         |         | D |
        -----------------------------------------------------------------
           2     8    1   1   1   1     8         4        12     var


                          format D data frame header for MuxPDU5
        -----------------------------------------------------------------
        |     |     | S | S | L | R |          |         | P |
        | 00  | SEQ | S | Q | S | T |  SEQ_HI |  S_SEQ  | A |
        |     |     | P | I | T | X |        |         | D |
        -----------------------------------------------------------------
           2     8    1   1   1   1     4        12     var

        ===================================================================*/

      /*---------------------------------------------------------------------
        The 8-bit OTA sequence number is always the first byte of the frame
        ('00' type bits have already been removed), and is removed from the
        frame payload.
        -------------------------------------------------------------------*/
      cnt = (byte)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &fr_seq_num, 1);
      /*---------------------------------------------------------------------
        Guard against pullup returning NULL ptr.
       --------------------------------------------------------------------*/
      if( fwd_frame_ptr->fr_item_ptr == NULL || cnt != sizeof(byte))
      {
        DATA_RLP_MSG0(MSG_LEGACY_HIGH, "return RLP_ERASURE");
        return( DSRLPI_RLP_ERASURE);
      }
      curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_MASK;

      /*---------------------------------------------------------------------
        The upper 4 bits of the next byte are the CTL bits and tell us which 
        fields are specified in the frame.  Remove the byte and to get the 
        upper four CTL bits.  If it is MuxPDU3, then the lower four bits are 
        used to build up the 8-bit length field. Else if it is, MuxPDU5, then
        lower 4 bits inform the Seq_hi or S-Seq or padding.
        -------------------------------------------------------------------*/
      cnt = (byte)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &ctl_byte, 1);
      /*---------------------------------------------------------------------
        Guard against pullup returning NULL ptr.
       --------------------------------------------------------------------*/
      if( fwd_frame_ptr->fr_item_ptr == NULL || cnt != sizeof(byte))
      {
        DATA_RLP_MSG0(MSG_LEGACY_HIGH, "return RLP_ERASURE");
        return( DSRLPI_RLP_ERASURE);
      }
      
      length = ( (ctl_byte & 0x0f) << 4 );

      /*---------------------------------------------------------------------
        Length field exists only for Mux PDU3. So calculate length only
        for MuxPDU3
        Now, start extracting bit fields using the bit unpack routines.
        Extract the next 4 bits of the length field.  These 4 bits are 
        combined with the 4 bits in the length variable to build the 8-bit 
        frame length field.  
        -------------------------------------------------------------------*/
      offset=0;
      if (mux_pdu_type == DSRLP_MUX_PDU_3) 
      {
      
         length = ( (ctl_byte & 0x0f) << 4 );

         /*------------------------------------------------------------------
           Now, start extracting bit fields using the bit unpack routines.
           Extract the next 4 bits of the length field.  These 4 bits are 
           combined with the 4 bits in the length variable to build the 8-bit 
           frame length field.  
         ------------------------------------------------------------------*/
         tmp_byte = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 4);
         offset += 4;
         length |= (tmp_byte & 0x0f);
      } /* MuxPDU3 */

      /*=====================================================================
        Look at the individual bit fields to determine which fields in the
        frame are relevant.  The CTL bits are as follows:

        SSP = S_SEQ Present Indicator
        SQI = SEQ 8/12 Bit Format Indicator
        LST = Last piece Indicator
        RTX = Retransmitted Indicator

        Look at the SSP and SQI bits to determine how many more bits to
        remove from the frame for sequence number generation.  Parse
        the bytes and continue processing.
        ===================================================================*/
      /*---------------------------------------------------------------------
        Check SQI bit to see if SEQ_HI field is present.
        -------------------------------------------------------------------*/
      if ( (ctl_byte & RLP3_FMT_D_DATA_SQI_MASK) 
                                                == RLP3_FMT_D_DATA_SQI_MASK )
      {
        /*-------------------------------------------------------------------
          The SQI bit is set, indicating that the 4-bit SEQ_HI field is 
          present.  Extract these 4 bits and use in calculating the internal 
          sequence number.  
         ------------------------------------------------------------------*/
        if (mux_pdu_type == DSRLP_MUX_PDU_3) 
        { 
            seq_hi = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                           RLP3_NAK_SEQ_HI_LEN);
            offset += RLP3_NAK_SEQ_HI_LEN;
        }
        else if (mux_pdu_type == DSRLP_MUX_PDU_5) 
        {
            seq_hi = (ctl_byte & 0x0F);
        }
        else
        {
          DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Bad PDU - SW ERR %d", mux_pdu_type);
          dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
	  DATA_RLP_MSG0(MSG_LEGACY_LOW, "return RLP_ERASURE");
          return(DSRLPI_RLP_ERASURE);
        }
        curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_HI_MASK;

        /*-------------------------------------------------------------------
          Begin building sequence number in internal representation. If the 
          S_SEQ field is specified, add that in later.
         ------------------------------------------------------------------*/
        DSRLPSEQ_SEQ_GEN_FROM_SEQ_HI(curr_fr_info_ptr->seq_num, seq_hi, 
                                   fr_seq_num);

        /*-------------------------------------------------------------------
          Check SSP bit to see if S_SEQ field is present.
         ------------------------------------------------------------------*/
        if ( (ctl_byte & RLP3_FMT_D_DATA_SSP_MASK) 
                                                == RLP3_FMT_D_DATA_SSP_MASK )
        {
          /*-----------------------------------------------------------------
            The SSP bit is set, indicating that the 12-bit S_SEQ field is
            present.  Parse the next 12 bits to get the S_SEQ number.  
           ----------------------------------------------------------------*/
          s_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                            12);
          offset += 12;
          curr_fr_info_ptr->fr_cat |= CURR_FRAME_S_SEQ_MASK;

          /*-----------------------------------------------------------------
            Both S_SEQ and SEQ_HI are present.  Account for the S_SEQ field
            in the sequence number (SEQ_HI was already calculated above).
           ----------------------------------------------------------------*/
          DSRLPSEQ_PUT_S_SEQ(curr_fr_info_ptr->seq_num, s_seq);
        }
      } 
      else /* SQI not set */
      {
        /*-------------------------------------------------------------------
          The SQI bit is not set, so there is no 4-bit SEQ_HI field.  Need
          to calculate L_SEQ using only the 8-bit SEQ field.
          Check SSP bit to see if S_SEQ field is present.  If it is set, 
          extract additional bytes to get the entire S_SEQ field.  If it is
          not, then we do not need to do anything, as we have all the 
          information we need.
         ------------------------------------------------------------------*/
        DSRLPSEQ_GEN_FROM_SEQ( curr_fr_info_ptr->seq_num, 
                               fr_seq_num, 
                               rrcb_ptr->state.expected_seq_num, 
                               rrcb_ptr->cfg_ptr->seq_modulus );

        if ( (ctl_byte & RLP3_FMT_D_DATA_SSP_MASK) 
                                                == RLP3_FMT_D_DATA_SSP_MASK )
        {
          /*-----------------------------------------------------------------
            The SSP bit is set, indicating that the 12-bit S_SEQ field is
            present.  Parse the next 12 bits to get the S_SEQ number.  
           ----------------------------------------------------------------*/
          if (mux_pdu_type == DSRLP_MUX_PDU_3) 
          {
            s_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr,offset,12);
            offset += 12;
          }
          else if (mux_pdu_type == DSRLP_MUX_PDU_5)
          {
          /*-----------------------------------------------------------------
              format D data frame header for MuxPDU5 if SeqHI is not present
              ---------------------------------------------------------
             |     |     | S | S | L | R |         | P |
             | 00  | SEQ | S | Q | S | T |  S_SEQ  | A |
             |     |     | P | I | T | X |         | D |
             ----------------------------------------------------------
               2     8    1   1   1   1      12     var
               
            Extract the upper nibble of S_SEQ from LS Nibble of Control byte
            and then move it to Left by 8 bits. So we now got the upper 4 bits.
            For clarity, clear off the unused bits.
            Then extract next 8 bits and put them in lower byte.
          -----------------------------------------------------------------*/
            s_seq = ctl_byte;
            s_seq = ((s_seq << 8) & 0xF00);
            s_seq |= b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr,offset,8);
            offset += 8;          
          }
          else
          {
              DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Unknown PDU type: Please verify RLP: %d",
                        rrcb_ptr->cfg_ptr->sr_id);
              dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
	      
              return(DSRLPI_RLP_ERASURE);
          }
          curr_fr_info_ptr->fr_cat |= CURR_FRAME_S_SEQ_MASK;

          /*-----------------------------------------------------------------
            Account for the S_SEQ field in the sequence number. (SEQ was 
            already calculated above).
           ----------------------------------------------------------------*/
          DSRLPSEQ_PUT_S_SEQ(curr_fr_info_ptr->seq_num, s_seq);
        }
      } /* if SQI set */

      /*---------------------------------------------------------------------
        Finished with parsing out headers of the Format D data frame.  Now
        advance the frame pointer to point to the payload data.  Advance by
        an integral number of bytes.  The offset contains the number of bits
        to remove, so convert to bytes, and remove.
        -------------------------------------------------------------------*/
      offset = ( (offset+7) >> 3 );

      /*---------------------------------------------------------------------
        Advance the data payload pointer, and update the "used" field.
       --------------------------------------------------------------------*/
      (void) dsm_pullup(&(fwd_frame_ptr->fr_item_ptr), NULL, offset);

      /*---------------------------------------------------------------------
        Ensure that the OTA length field corresponds to the number of bytes
        in the dsm item.  
       --------------------------------------------------------------------*/
      if (mux_pdu_type == DSRLP_MUX_PDU_3)       
      {
          dsm_trim_packet(&fwd_frame_ptr->fr_item_ptr, length);
          if( (fwd_frame_ptr->fr_item_ptr == NULL) || 
            (dsm_length_packet(fwd_frame_ptr->fr_item_ptr) < length) )
          {
	    
            return( DSRLPI_RLP_ERASURE);
          }
      }
      else if (mux_pdu_type == DSRLP_MUX_PDU_5)       
      {
          if(fwd_frame_ptr->fr_item_ptr == NULL)
          {
	    
             return( DSRLPI_RLP_ERASURE);
          }
      }
      else
      {
        DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Bad PDU - SW ERR %d", mux_pdu_type);
        dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
	
        return(DSRLPI_RLP_ERASURE);
      }
      /*---------------------------------------------------------------------
        Check LST bit indicating the last piece of a segmented data frame.
        If it is set, indicate this in the frame category bit mask.
        -------------------------------------------------------------------*/
      if ( (ctl_byte & RLP3_FMT_D_DATA_LST_MASK) 
                                                == RLP3_FMT_D_DATA_LST_MASK )
      {
        curr_fr_info_ptr->fr_cat |= CURR_FRAME_LST_BIT_MASK;
      }

      /*---------------------------------------------------------------------
        Check RTX bit and return the appropriate frame type.
        -------------------------------------------------------------------*/
      if ( (ctl_byte & RLP3_FMT_D_DATA_RTX_MASK) 
                                                == RLP3_FMT_D_DATA_RTX_MASK )
      {
        return (DSRLPI_REXMIT);
      }
      else
      {
        return (DSRLPI_NEW);
      }

    } /* format D data frame */

    /*-----------------------------------------------------------------------
      The Rate 1 frame was not of type B, C, or D.  This means that
      it is a format A control or data frame, or invalid.
      Validate that it is a format A frame, otherwise the frame is an RLP
      erasure.  If it is a valid frame, process it as normal.
      ---------------------------------------------------------------------*/
    if( (type_bits & type_mask) 
                  != dsrlpi_type_mask_table[mux_pdu_type][srvc_type].format_a
      )
    {
      /*---------------------------------------------------------------------
        The full-rate frame did not match any of the valid formats,
        therefore mark it as an RLP erasure.
       --------------------------------------------------------------------*/
        DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Bad type bits for RLP3 - %x RLP:%d", 
                  type_bits,rrcb_ptr->cfg_ptr->sr_id);
        return(DSRLPI_RLP_ERASURE);
    }

  } /* if DSRLP_RATE_1 */


  /*=========================================================================
    Continue processing of all frame types for all frame rates.  Frame is 
    valid so far, and can be idle, fill, control, or data frame, based on 
    other fields in the RLP header.  Parse out the appropriate fields and 
    determine frame type based on this.  Also, parse out any additional 
    fields required by subsequent frame processing.  All frame pointers 
    should point to the beginning of the data upon return from this function.
   ========================================================================*/

  /*-------------------------------------------------------------------------
    Assumption that all remaining frame types are contained within a single
    dsm item.  Only full rate frames may span a dsm item chain.
   ------------------------------------------------------------------------*/
  // ASSERT(fwd_frame_ptr->fr_item_ptr->pkt_ptr == NULL);

  /*-------------------------------------------------------------------------
    Parse through the 2nd byte, which contains the CTL bits, to determine
    frame type.  The byte is not removed from the frame, as it may be needed
    later for FCS processing.
   ------------------------------------------------------------------------*/
  (void)dsm_extract(fwd_frame_ptr->fr_item_ptr, 1, &ctl_byte, 1);

  if ( (ctl_byte & RLP3_UNSEG_DATA_MASK) == RLP3_UNSEG_DATA_MASK_RSLT )
  {
    /*-----------------------------------------------------------------------
      Extract and store the frame sequence number for subsequent 
      processing. Note, sequence number is always the first byte of the 
      frame, and is now removed from the frame payload.  We do not store
      into cur_fr_info_ptr just yet, as it may be an extended sequence
      number that needs to be built up.  For the time being, leave it in
      fr_seq_num and handle it later.  Also, remove the ctl_byte so that
      the frame pointer is pointing at the payload of the RLP frame.
     ----------------------------------------------------------------------*/
    (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &fr_seq_num, 1);
    curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_MASK;      

    (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &ctl_byte, 1);

    /*-----------------------------------------------------------------------
      Ensure that the OTA length field corresponds to the number of bytes
      in the dsm item. Trim the extra padding bytes. The frame processing
      functions need just the payload bytes.
      Note, the length field only looks at the 5 bits, because the max. len.
      of a MuxPDU1 or MuxPDU2 frame can only be 32 bytes.
    -----------------------------------------------------------------------*/
    length = ctl_byte & 0x1f;
    dsm_trim_packet(&fwd_frame_ptr->fr_item_ptr, length);

    if( (fwd_frame_ptr->fr_item_ptr == NULL) || 
		(dsm_length_packet(fwd_frame_ptr->fr_item_ptr) < length) )
    {
      DATA_RLP_MSG3(MSG_LEGACY_HIGH, "BS fr too short len = %d used = %d RLP:%d",
               length,dsrlpi_ctl_used,rrcb_ptr->cfg_ptr->sr_id);
      return( DSRLPI_RLP_ERASURE);
    }

    /*-----------------------------------------------------------------------
      Frame is an unsegmented data frame.  Determine if it is a  
      retransmitted frame, and process accordingly.
     ----------------------------------------------------------------------*/
    if ( (ctl_byte & RLP3_UNSEG_DATA_REXMIT_MASK) 
                                             == RLP3_UNSEG_DATA_REXMIT_MASK )
    {
      /*---------------------------------------------------------------------
        Generate the internal frame sequence number from the 8-bit OTA SEQ 
        field.  Matching against the NAK list entries will be performed as
        part of the retransmit frame processing.  
        number contains only the 8-bit SEQ field.
       --------------------------------------------------------------------*/
      DSRLPSEQ_PUT_SEQ(curr_fr_info_ptr->seq_num, fr_seq_num);
      return (DSRLPI_REXMIT);
      
    } /* RTX bit was set */
    else
    {
      /*---------------------------------------------------------------------
        It is a new data frame.  Calculate the frame sequence number and put
        into internal representation.
       --------------------------------------------------------------------*/
      DSRLPSEQ_GEN_FROM_SEQ( curr_fr_info_ptr->seq_num, 
                             fr_seq_num, 
                             rrcb_ptr->state.expected_seq_num, 
                             rrcb_ptr->cfg_ptr->seq_modulus);
      return (DSRLPI_NEW);
        
    } /* RTX bit */
    
  } /* unsegmented data frame */
  else if ( (ctl_byte & RLP3_SEG_DATA_MASK) == RLP3_SEG_DATA_FLD_CTL )
  {
    /*=======================================================================
      Frame is a segmented data frame.  Extract, obtain, and store the frame
      bit fields, and use for processing. Zero length segmented data frames
      are Idle Frame Format 2 type frames.

                          segmented data frame header
        -----------------------------------------------------------------
        |     |  C  | S | L | R |        |         |         | P |
        | SEQ |  T  | Q | S | T | Length |  SEQ_HI |  S_SEQ  | A |
        |     |  L  | I | T | X |        |         |         | D |
        -----------------------------------------------------------------
           8  '1000'  1   1   1     5         4        12     var

      The frame may be a new segmented frame, or a retransmitted data frame, 
      per 4.2.2.  First need to determine if the SQI bit is set, indicating 
      an extended sequence number.  If so, we will need to extract the next
      byte to get SEQ_HI and augment the SEQ field to build the full 12-bit 
      sequence number.
     ======================================================================*/

    /*-----------------------------------------------------------------------
      Extract and store the frame sequence number for subsequent 
      processing. Note, sequence number is always the first byte of the 
      frame, and is now removed from the frame payload.  We do not store
      into cur_fr_info_ptr just yet, as it may be an extended sequence
      number that needs to be built up.  For the time being, leave it in
      fr_seq_num and handle it later.  
     ----------------------------------------------------------------------*/
    (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &fr_seq_num, 1);
    curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_MASK;      

    /*-----------------------------------------------------------------------
      The 3 bits out of the ctl_byte contain extended control field 
      information.  Need to process these fields, so now actually remove the
      the ctl_byte, as opposed to just extracting it.  Once it is removed
      need to process these fields to determine which other fields to remove.
     ----------------------------------------------------------------------*/
    cnt = (byte)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &ctl_byte, 1);

    /*-----------------------------------------------------------------------
      If the frame has no more bytes then it is an incorrectly detected/
      transmitted frame. Declare it an erasure, log an error and return. 
    -----------------------------------------------------------------------*/
    if( fwd_frame_ptr->fr_item_ptr == NULL || cnt != sizeof(byte))
    {
      DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Malformed frame, ctl_byte=0x%xi RLP:%d", 
               ctl_byte, rrcb_ptr->cfg_ptr->sr_id);
      return( DSRLPI_RLP_ERASURE);
    }

    length = ( (ctl_byte & 0x01) << 4 );

    /*-----------------------------------------------------------------------
      Now, start extracting bit fields using the bit unpack routines.
      Extract the next 4 bits of the length field.  These 4 bits are combined
      with the bit in the length variable to build the 5-bit frame length 
      field.  If the length is zero, this is an idle frame format 2 type 
      frame.
     ----------------------------------------------------------------------*/
    offset = 0;
    tmp_byte = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 4);
    offset += 4;
    length |= (tmp_byte & 0x0f);
    
    /*=======================================================================
      Look at the individual bit fields to determine which fields in the
      frame are relevant.  The CTL bits are as follows:

        SQI = SEQ 8/12 Bit Format Indicator
        LST = Last piece Indicator
        RTX = Retransmitted Indicator

      Look at the SQI bit to determine how many more bits to remove from the 
      frame for sequence number generation.  Parse the bytes and continue 
      processing.  Note, segmented data frames always contain the S_SEQ  
      field.
     ======================================================================*/

    /*-----------------------------------------------------------------------
      Check SQI bit to see if SEQ_HI field is present.
     ----------------------------------------------------------------------*/
    if ( (ctl_byte & RLP3_SEG_DATA_SQI_MASK) == RLP3_SEG_DATA_SQI_MASK )
    {
      /*---------------------------------------------------------------------
        The SQI bit is set, indicating that the 4-bit SEQ_HI field is 
        present.  Extract these 4 bits and use in calculating the internal 
        sequence number.  
       --------------------------------------------------------------------*/
      seq_hi = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                         RLP3_NAK_SEQ_HI_LEN);

      curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_HI_MASK;      

      offset += RLP3_NAK_SEQ_HI_LEN;

      /*---------------------------------------------------------------------
        Begin building sequence number in internal representation.
       --------------------------------------------------------------------*/
      DSRLPSEQ_SEQ_GEN_FROM_SEQ_HI(curr_fr_info_ptr->seq_num, seq_hi, 
                                   fr_seq_num);
      /*---------------------------------------------------------------------
        The SSP bit is set, indicating that the 12-bit S_SEQ field is
        present.  Parse the next 12 bits to get the S_SEQ number.  
       --------------------------------------------------------------------*/
      s_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr,offset,12);
      offset += 12;
      curr_fr_info_ptr->fr_cat |= CURR_FRAME_S_SEQ_MASK;

      /*---------------------------------------------------------------------
        Both S_SEQ and SEQ_HI are present.  Account for the S_SEQ field in 
        the sequence number (SEQ_HI was already calculated above).
       --------------------------------------------------------------------*/
      DSRLPSEQ_PUT_S_SEQ(curr_fr_info_ptr->seq_num, s_seq);
    } 
    else /* SQI not set */
    {
      /*---------------------------------------------------------------------
        The SQI bit is not set, so there is no 4-bit SEQ_HI field.  Need
        to calculate L_SEQ using only the 8-bit SEQ field.  Then, extract 
        the S_SEQ field and calculate the complete sequence number.
       --------------------------------------------------------------------*/
      DSRLPSEQ_GEN_FROM_SEQ( curr_fr_info_ptr->seq_num, 
                             fr_seq_num, 
                             rrcb_ptr->state.expected_seq_num, 
                             rrcb_ptr->cfg_ptr->seq_modulus );

      /*---------------------------------------------------------------------
        The SSP bit is set, indicating that the 12-bit S_SEQ field is
        present.  Parse the next 12 bits to get the S_SEQ number.  
       --------------------------------------------------------------------*/
      s_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr,offset,12);
      offset += 12;
      curr_fr_info_ptr->fr_cat |= CURR_FRAME_S_SEQ_MASK;
      
      /*---------------------------------------------------------------------
        Account for the S_SEQ field in the sequence number. (SEQ was 
        already calculated above).
       --------------------------------------------------------------------*/
      DSRLPSEQ_PUT_S_SEQ(curr_fr_info_ptr->seq_num, s_seq);

    } /* if SQI set */
    
    /*-----------------------------------------------------------------------
      Check the length of the frame.  If it is zero length, this is an idle
      frame format 2 type frame.
      ---------------------------------------------------------------------*/
    if (length == 0)
    {
      return (DSRLPI_IDLE);
    }

    /*-----------------------------------------------------------------------
      Finished with parsing out headers of the segmented data frame.  Now
      advance the frame pointer to point to the payload data.  Advance by
      an integral number of bytes.  The offset contains the number of bits
      to remove, so convert to bytes, and remove.
      ---------------------------------------------------------------------*/
    offset = ( (offset+7) >> 3 );

    /*-----------------------------------------------------------------------
      Advance the data payload pointer, and update the "used" field.
     ----------------------------------------------------------------------*/
    (void) dsm_pullup(&(fwd_frame_ptr->fr_item_ptr), NULL, offset);

    /*-----------------------------------------------------------------------
      Ensure that the OTA length field corresponds to the number of bytes
      in the dsm item. Trim the extra padding bytes. The frame processing
      functions need just the payload bytes.
    -----------------------------------------------------------------------*/
    dsm_trim_packet(&fwd_frame_ptr->fr_item_ptr, length);

    if( (fwd_frame_ptr->fr_item_ptr == NULL) || 
		(dsm_length_packet(fwd_frame_ptr->fr_item_ptr) < length) )
    {
      
      return( DSRLPI_RLP_ERASURE);
    }

    /*-----------------------------------------------------------------------
      Check LST bit indicating the last piece of a segmented data frame.
      If it is set, indicate this in the frame category bit mask.
      ---------------------------------------------------------------------*/
    if ( (ctl_byte & RLP3_SEG_DATA_LST_MASK) == RLP3_SEG_DATA_LST_MASK )
    {
      curr_fr_info_ptr->fr_cat |= CURR_FRAME_LST_BIT_MASK;
    }


    /*-----------------------------------------------------------------------
      Check RTX bit and return the appropriate frame type.
      ---------------------------------------------------------------------*/
    if ( (ctl_byte & RLP3_SEG_DATA_REXMIT_MASK) 
                                               == RLP3_SEG_DATA_REXMIT_MASK )
    {
      return (DSRLPI_REXMIT);
    }
    else
    {
      return (DSRLPI_NEW);
    }
  
  } /* segmented data frame */
  else
  {
    /*=======================================================================
      Frame is a control, idle, or fill frame.  Determine which type based 
      on CTL bits.  Calculate and assign the internal sequence number, since 
      it won't contain extended header info.  Note, for these frames we 
      cannot use dsm_pullup() to remove bytes, as we need to calculate the 
      16-bit FCS across the entire frame.  Instead, make use of the bit 
      unpacking routines.  This also helps in dealing with variable-sized 
      frame formats.
      =====================================================================*/
    offset = 0;
    fr_seq_num = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                           RLP3_CTL_SEQ_LEN);

    curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_MASK;    
    offset += RLP3_CTL_SEQ_LEN;

    /*-----------------------------------------------------------------------
      Now obtain the CTL bits. The control bits indicate the RLP frame type.
      ---------------------------------------------------------------------*/
    ctl_byte = b_unpackb( fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                          RLP3_CTL_CTL_LEN);
    offset += RLP3_CTL_CTL_LEN;

    /*-----------------------------------------------------------------------
      Check if frame is an initialization control frame.
      ---------------------------------------------------------------------*/
    if( ( ( (ctl_byte & RLP3_CTL_CTL_MASK) == RLP3_CTL_CTL_SYNC)    ||
          ( (ctl_byte & RLP3_CTL_CTL_MASK) == RLP3_CTL_CTL_SYNC_ACK)||
          ( (ctl_byte & RLP3_CTL_CTL_MASK) == RLP3_CTL_CTL_ACK )
        )                                                           &&
        ( dsm_length_packet( fwd_frame_ptr->fr_item_ptr) > 3)
      )
    {
      /*=====================================================================
        Frame is SYNC/ACK type frame.  NAK negotiation for this 
        implementation of RLP 3 is only performed via the RLP_BLOB (standard 
        allows for negotiation via SYNC/ACK frames).  This is performed 
        outside of frame determination, during call setup.  Encryption is 
        also not supported by the mobile, and is also determined during call 
        setup.  Thus, the initialization control frame should only contain 
        SEQ, CTL, RESET_VAR, and NAK_PARAM_INCL (also set to zero), and then
        the FCS.  If NAK_PARAM_INCL is non-zero, then RLP is reset.  However,
        first determine if the FCS is valid.  Unpack the relevant fields
        to determine the location of the FCS bits.
      =====================================================================*/
      nak_param_inc = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr,
                                RLP3_SA_NAK_PARAM_INC_POS,
                                RLP3_SA_NAK_PARAM_INC_LEN);
      offset = RLP3_SA_NAK_PARAM_INC_POS + RLP3_SA_NAK_PARAM_INC_LEN;

      /*---------------------------------------------------------------------
        If NAK parameter negotiation is to be performed, nak_param_inc will
        be set to 1.
      ---------------------------------------------------------------------*/
      if (nak_param_inc == TRUE)
      {
        /*-------------------------------------------------------------------
          RLP is attempting to perform NAK parameter negotiation.  Determine 
          how many fwd and rev rounds of NAKs there are, in order to find 
          where FCS is located.    
        -------------------------------------------------------------------*/
        tmp_byte = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                             RLP3_SA_NAK_RNDS_FWD_POS,
                             RLP3_SA_NAK_RNDS_FWD_LEN);

        tmp_byte += b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                              RLP3_SA_NAK_RNDS_REV_POS,
                              RLP3_SA_NAK_RNDS_REV_LEN);

        offset = RLP3_SA_NAKS_PER_RND_FWD_POS + 
                 (tmp_byte * RLP3_SA_NAKS_PER_RND_FWD_LEN);

      }
      
      /*---------------------------------------------------------------------
        Modify offset to account for the padding_1 field to octet align the 
        FCS field.
       --------------------------------------------------------------------*/
      if ((offset % 8) > 0)
      {
        offset += (8 - (offset % 8));
      }

      /*---------------------------------------------------------------------
        Validate the FCS.  Generate the CRC across the entire frame, which 
        covers everything up to the FCS.  The current offset contains the 
        length of the frame.
       --------------------------------------------------------------------*/
      if( (crc_16_l_calc((byte *) fwd_frame_ptr->fr_item_ptr->data_ptr, 
                        offset+(RLP3_CTL_FCS_BYTE_LEN*8))) != CRC_16_L_OK)
      {
        /*-------------------------------------------------------------------
          Failed FCS check.  Treat as erasure and return.  Do we need to
          reset all the variables we just set?
         ------------------------------------------------------------------*/
        DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Bad FCS for INIT frame");
        return (DSRLPI_RLP_ERASURE);
      }

      /*---------------------------------------------------------------------
        Since the FCS calculation passed, thus the frame is valid.  If the 
        nak_parameter bit was set, then need to reset RLP.  This can be 
        accomplished by treating the initialization control frame as a SYNC
        frame (which will invoke a RESET.  If the bit was not set, then the 
        frame category bits based on the actual ctl_byte.
       --------------------------------------------------------------------*/
      if (nak_param_inc == TRUE)
      {
        /*-------------------------------------------------------------------
          Reset RLP by treating frame as a SYNC.
         -------------------------------------------------------------------*/
        curr_fr_info_ptr->fr_cat |= CURR_FRAME_SYNC_MASK;     
      }
      else
      {
        /*-------------------------------------------------------------------
          Set the frame category bits based on actual control field.
         -------------------------------------------------------------------*/
        if( (ctl_byte & RLP3_CTL_CTL_SYNC_BIT_MASK) == RLP3_CTL_CTL_SYNC)
        {
          curr_fr_info_ptr->fr_cat |= CURR_FRAME_SYNC_MASK;     
        }

        if( (ctl_byte & RLP3_CTL_CTL_ACK_BIT_MASK)  == RLP3_CTL_CTL_ACK)
        {
          curr_fr_info_ptr->fr_cat |= CURR_FRAME_ACK_MASK;      
        }
      }
      return (DSRLPI_INIT);
    } /* if INIT Frame */

    /*-----------------------------------------------------------------------
      Not an initialization control frame, so check to see if it is a NAK 
      frame - requests retransmission of one or more data frames.
     ----------------------------------------------------------------------*/
    if ( ( (ctl_byte & RLP3_CTL_CTL_MASK) == RLP3_CTL_CTL_NAK ) && 
         ( dsm_length_packet( fwd_frame_ptr->fr_item_ptr) > 9)
       )
    {
      /*=====================================================================
        Obtain the next two bits which contain the NAK_TYPE.  NAK_TYPE
        fields are defined as follows:

        '00' - Requests retransmission of data frames with sequence number
               FIRST through LAST, inclusive.
        '01' - Requests retransmission of data frames as specified by the 
               NAK map(s).
        '10' - Requests retransmission of segment(s) of a data frame
               containing octets with sequence numbers FIRST_S_SEQ through
               LAST_S_SEQ, inclusive.  FRAME_SEQ is the sequence number of 
               the frame.
        '11' - Requests retransmission of segment(s) of a data frame starting
               octet with sequence number FIRST_S_SEQ and S_SEQ_LENGHT octets
               in length.  FRAME_SEQ is the sequence number of the frame.

         Based on the value of this field, determine which fields in the 
         frame to extract.  From this information, build up FIRST/LAST pairs
         of sequence numbers to retransmit.
      =====================================================================*/
      tmp_byte = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                            RLP3_NAK_NAK_TYPE_LEN);
      offset += RLP3_NAK_NAK_TYPE_LEN;

      /*---------------------------------------------------------------------
        Mask off high order bits to get the NAK type.
       --------------------------------------------------------------------*/
      nak_type = tmp_byte & 0x03;

      /*---------------------------------------------------------------------
        NAK frames always contain the SEQ_HI field, containing the most 
        significant 4 bits of L_V(N).  Obtain bits and perform sequence
        number calculation.  
       --------------------------------------------------------------------*/
      seq_hi = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                         RLP3_NAK_SEQ_HI_LEN);
      offset += RLP3_NAK_SEQ_HI_LEN;

      rrcb_ptr->stats.naks_received++;
      
      DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP: %d", rrcb_ptr->cfg_ptr->sr_id);
      /*---------------------------------------------------------------------
        Based on NAK_TYPE, extract the relevant fields.
       --------------------------------------------------------------------*/            
      switch (nak_type)
      {
        case RLP3_NAK_NAK_TYPE_0:
          /*-----------------------------------------------------------------
            Type 0 - Get the NAK_Hole_Count and NAK_Map_Count + 1 occurrences
            of the FIRST and LAST fields.  NAK_Hole_Count contains one less 
            than the number of NAK hole requests in this NAK control frame.
           ----------------------------------------------------------------*/
          nak_count = 
            b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset,
                      RLP3_NAK_HOLE_CNT_LEN);
          offset += RLP3_NAK_HOLE_CNT_LEN;

          /*-----------------------------------------------------------------
            Read NAK_HOLE_CNT+1 number of NAKs.  
           ----------------------------------------------------------------*/
          for (i=0; i<(nak_count + 1); i++)
          {
            /*---------------------------------------------------------------
              Initialize tmp variables.
             --------------------------------------------------------------*/            
            nak_first_seq = nak_last_seq = 0;

            /*---------------------------------------------------------------
              Get the FIRST sequence number.
             --------------------------------------------------------------*/
            nak_first_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                      offset, 
                                      RLP3_NAK_FIRST_LEN);
                                     
            offset += RLP3_NAK_FIRST_LEN; 

            /*---------------------------------------------------------------
              Build sequence number in internal representation.
             --------------------------------------------------------------*/
            DSRLPSEQ_PUT_12BIT_FRAME_SEQ(nak_first_seq);

            /*---------------------------------------------------------------
              Get the LAST sequence number.
             --------------------------------------------------------------*/
            nak_last_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                     offset, 
                                     RLP3_NAK_LAST_LEN);
                                    
            offset += RLP3_NAK_LAST_LEN;

            /*---------------------------------------------------------------
              Build sequence number in internal representation.
             --------------------------------------------------------------*/
            DSRLPSEQ_PUT_12BIT_FRAME_SEQ(nak_last_seq);
 
            /*---------------------------------------------------------------
              Create entry for re-xmit list.
             --------------------------------------------------------------*/
            if( dsrlprtx_put_rexmit_info( rrcb_ptr, 
                                          nak_first_seq, 
                                          nak_last_seq) == FALSE)
            {
              resync_rlp = TRUE;
            }
            else
            {
              naks_inserted_in_rtxq++;
            }
            DATA_RLP_MSG3(MSG_LEGACY_MED, "BS NAK0: 0x%x -> 0x%x, V(S)=0x%x",
                   nak_first_seq, 
                   nak_last_seq,
                   rrcb_ptr->state.next_seq_num);
             
          } /* for nak_count */

        break;

        case RLP3_NAK_NAK_TYPE_1:
          /*-----------------------------------------------------------------
            Type 1 - Get the NAK_Map_Count and NAK_Map_Count + 1 occurrences
            of the NAK_Map_SEQ and NAK_Map fields.  NAK_Map_Count contains
            one less than the number of NAK Maps in this control frame.
           ----------------------------------------------------------------*/
          nak_count = 
            b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset,
                      RLP3_NAK_MAP_CNT_LEN); 
          offset += RLP3_NAK_MAP_CNT_LEN;

          /*-----------------------------------------------------------------
            Read NAK_Map_Count+1 number of NAKs.
           ----------------------------------------------------------------*/
          for (i=0; i<(nak_count + 1); i++)
          {
            /*---------------------------------------------------------------
              Obtain the base NAK_Map_SEQ.
             --------------------------------------------------------------*/
            nak_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                offset, 
                                (RLP3_NAK_SEQ_HI_LEN+RLP3_NAK_SEQ_LEN)
                               );
            offset += (RLP3_NAK_SEQ_HI_LEN + RLP3_NAK_SEQ_LEN);

            /*---------------------------------------------------------------
              Build sequence number in internal representation.
             --------------------------------------------------------------*/
            DSRLPSEQ_PUT_12BIT_FRAME_SEQ(nak_seq);

            /*---------------------------------------------------------------
              Generate the NAK_Map_SEQ as the first NAK entry.  Then, 
              process the subsequent entries in the NAK map.  Create entry
              for re-xmit list.
             --------------------------------------------------------------*/
            if( dsrlprtx_put_rexmit_info(rrcb_ptr, nak_seq, nak_seq)== FALSE)
            {
              resync_rlp = TRUE;
            }
            else
            {
              naks_inserted_in_rtxq++;
            }

            /*---------------------------------------------------------------
              Obtain the NAK_Map and begin building the entries.  Loop 
              through the NAK_Map byte and build the NAK first/last pairs
              based on consecutive bits set in the byte.
             --------------------------------------------------------------*/
            nak_map_byte = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                     offset, RLP3_NAK_MAP_LEN);
            offset += RLP3_NAK_MAP_LEN;

            /*---------------------------------------------------------------
              Start with a mask that has the most significant bit position
              set.  Loop through each bit position and generate a first/last 
              NAK pair for each bit that is set.  The MSB of the bitmap 
              corresponds to the [(first_seq+1) modulo SeqModulus], etc.
             --------------------------------------------------------------*/
            mask = 0x80;
            bit_pos = 0;
            for(bit_pos = 1; bit_pos <= 8; bit_pos++)
            {
              if (nak_map_byte & mask)
              {
                tmp_seq = nak_seq;
                tmp_seq = DSRLPSEQ_ADD_SEQ(tmp_seq, 
                                           bit_pos, 
                                           rrcb_ptr->cfg_ptr->seq_modulus );
                /*-----------------------------------------------------------
                  Create entry for re-xmit list.
                 ----------------------------------------------------------*/
                if( dsrlprtx_put_rexmit_info(rrcb_ptr, tmp_seq, tmp_seq)
                        == FALSE)
                {
                  resync_rlp = TRUE;
                }
                else
                {
                  naks_inserted_in_rtxq++;
                }
              }

              /*-------------------------------------------------------------
                Shift mask to get to next bit position.
               ------------------------------------------------------------*/
              mask = mask >> 1;

            } /* for bit_pos */
            DATA_RLP_MSG3(MSG_LEGACY_MED, "BS NAK1 0x%x, Map:0x%x :V(S)=0x%x",
                    nak_seq,
                    nak_map_byte,
                    rrcb_ptr->state.next_seq_num);
          } /* for nak_count */

#ifdef FEATURE_HDR
          DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP: %d", rrcb_ptr->cfg_ptr->sr_id);
#endif
        break;

        case RLP3_NAK_NAK_TYPE_2:
          /*-----------------------------------------------------------------
            Type 2 - Get the NAK_SEG_COUNT and the NAK_SEG_COUNT + 1 
            occurrences of FRAME_SEQ, FIRST_S_SEQ, and LAST_S_SEQ.  
            NAK_SEG_COUNT contains one less than the number of NAK requests
            in this NAK control frame.
           ----------------------------------------------------------------*/
          nak_count = 
            b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                      RLP3_NAK_SEG_CNT_LEN);
          offset += RLP3_NAK_SEG_CNT_LEN;

          /*-----------------------------------------------------------------
            Read NAK_Seg_Count+1 number of NAKs.
           ----------------------------------------------------------------*/
          for (i=0; i<(nak_count + 1); i++)
          {
 
            /*---------------------------------------------------------------
              Initialize temporary variables.
             --------------------------------------------------------------*/
            nak_seq = 0;

            /*---------------------------------------------------------------
              Obtain the base FRAME_SEQ.
             --------------------------------------------------------------*/
            nak_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                offset, 
                                RLP3_NAK_FRAME_SEQ_LEN);
                               
            offset += RLP3_NAK_FRAME_SEQ_LEN;

            /*---------------------------------------------------------------
              Build sequence number in internal representation.
             --------------------------------------------------------------*/
            DSRLPSEQ_PUT_12BIT_FRAME_SEQ(nak_seq);

            /*---------------------------------------------------------------
              Obtain the FIRST_S_SEQ.
            ---------------------------------------------------------------*/
            first_s_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                    offset, RLP3_NAK_FIRST_S_SEQ_LEN);
            offset += RLP3_NAK_FIRST_S_SEQ_LEN;

            /*---------------------------------------------------------------
              This value is added to the base FRAME_SEQ to generate the FIRST
              entry.  
             --------------------------------------------------------------*/
            nak_first_seq = nak_seq;
            nak_first_seq = DSRLPSEQ_ADD_S_SEQ(nak_first_seq, 
                                               first_s_seq,
                                               rrcb_ptr->cfg_ptr->seq_modulus );

            /*---------------------------------------------------------------
              Obtain the LAST_S_SEQ.
            ---------------------------------------------------------------*/
            last_s_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                   offset, RLP3_NAK_LAST_S_SEQ_LEN);
            offset += RLP3_NAK_LAST_S_SEQ_LEN;

            /*---------------------------------------------------------------
              This value is added to the base FRAME_SEQ to generate the LAST
              entry.  If the value is 4095, it indicates NAKing to the end
              of the frame, which is automagically handled already by our
              internal sequence number format, so just add the offset.
             --------------------------------------------------------------*/
            nak_last_seq = nak_seq;
            nak_last_seq = DSRLPSEQ_ADD_S_SEQ(nak_last_seq, last_s_seq,
                                              rrcb_ptr->cfg_ptr->seq_modulus );

            /*---------------------------------------------------------------
              Create entry for re-xmit list.
             --------------------------------------------------------------*/
            if( dsrlprtx_put_rexmit_info( rrcb_ptr, 
                                          nak_first_seq, 
                                          nak_last_seq) == FALSE)
            {
              resync_rlp = TRUE;
            }
            else
            {
              naks_inserted_in_rtxq++;
            }
            DATA_RLP_MSG3(MSG_LEGACY_MED, "BS NAK2: 0x%x -> 0x%x, V(S)=0x%x",
                   nak_first_seq,
                   nak_last_seq,
                   rrcb_ptr->state.next_seq_num);
            
          } /* for nak_count */

        break;

        case RLP3_NAK_NAK_TYPE_3:
          /*-----------------------------------------------------------------
            Type 3 - Get the NAK_SEG_COUNT and the NAK_SEG_COUNT + 1 
            occurrences of FRAME_SEQ, FIRST_S_SEQ, and LENGTH_S_SEQ.  
            NAK_SEG_COUNT contains one less than the number of NAK requests
            in this NAK control frame.
           ----------------------------------------------------------------*/
          nak_count =
            b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, offset, 
                      RLP3_NAK_SEG_CNT_LEN);
          offset += RLP3_NAK_SEG_CNT_LEN;

          /*-----------------------------------------------------------------
            Read NAK_Seg_Count+1 number of NAKs.
           ----------------------------------------------------------------*/
          for (i=0; i<(nak_count + 1); i++)
          {
            /*---------------------------------------------------------------
              Initialize temporary variables.
             --------------------------------------------------------------*/
            nak_first_seq = nak_last_seq = nak_seq = 0;

            /*---------------------------------------------------------------
              Obtain the base FRAME_SEQ.
             --------------------------------------------------------------*/
            nak_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                offset, 
                                RLP3_NAK_FRAME_SEQ_LEN
                               );
            offset += RLP3_NAK_FRAME_SEQ_LEN;

            /*---------------------------------------------------------------
              Build sequence number in internal representation.
             --------------------------------------------------------------*/
            DSRLPSEQ_PUT_12BIT_FRAME_SEQ(nak_seq);

            /*---------------------------------------------------------------
              Obtain the FIRST_S_SEQ.
            ---------------------------------------------------------------*/
            first_s_seq = b_unpackw(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                                    offset, RLP3_NAK_FIRST_S_SEQ_LEN);
            offset += RLP3_NAK_FIRST_S_SEQ_LEN;

            /*---------------------------------------------------------------
              This value is added to the base FRAME_SEQ to generate the FIRST
              entry.  
             --------------------------------------------------------------*/
            nak_first_seq = nak_seq;
            nak_first_seq = DSRLPSEQ_ADD_S_SEQ(nak_first_seq, first_s_seq, 
                                               rrcb_ptr->cfg_ptr->seq_modulus );

            /*---------------------------------------------------------------
              Obtain the LENGTH_S_SEQ byte, containing the length of the
              segment that is being NAKed.  
             --------------------------------------------------------------*/
            length = b_unpackb(fwd_frame_ptr->fr_item_ptr->data_ptr, 
                               offset, RLP3_NAK_LENGTH_S_SEQ_LEN);
            offset += RLP3_NAK_LENGTH_S_SEQ_LEN;

			/*---------------------------------------------------------------
              If the LENGTH_S_SEQ value is 0x00, the rest of the segment is
              being NAK'ed, otherwise the length is added to the FIRST NAK
              entry.
             --------------------------------------------------------------*/
            if (length == 0)
            {
              /*-------------------------------------------------------------
                NAK rest of entire frame segment.
               ------------------------------------------------------------*/
              nak_last_seq = nak_seq;
              nak_last_seq = DSRLPSEQ_ADD_S_SEQ(nak_last_seq,
                                                RLP3_NAK_LAST_S_SEQ_MAX,
                                                rrcb_ptr->cfg_ptr->seq_modulus );
            }
            else
            {
              /*-------------------------------------------------------------
                NAK the segment in LENGTH_S_SEQ.  
               ------------------------------------------------------------*/
              nak_last_seq = nak_first_seq;
              nak_last_seq = DSRLPSEQ_ADD_S_SEQ(nak_last_seq, length,
                                                rrcb_ptr->cfg_ptr->seq_modulus );
            }

            /*---------------------------------------------------------------
              Create entry for re-xmit list.
             --------------------------------------------------------------*/
            if( dsrlprtx_put_rexmit_info( rrcb_ptr, 
                                          nak_first_seq, 
                                          nak_last_seq) == FALSE)
            {
              resync_rlp = TRUE;
            }
            else
            {
              naks_inserted_in_rtxq++;
            }
            DATA_RLP_MSG3(MSG_LEGACY_MED, "BS NAK3: 0x%x Len:0x%x :V(S)=0x%x", nak_first_seq,length, rrcb_ptr->state.next_seq_num);
		  } /* for nak_count */

        break;

        default:
          ERR_FATAL("Unknown NAK Type", 0, 0, 0);

	  } /* switch (nak_type) */

      /*---------------------------------------------------------------------
        Modify offset to account for the padding_1 field to octet align the 
        FCS field.  
       --------------------------------------------------------------------*/
      offset += (8 - (offset % 8));

      /*---------------------------------------------------------------------
        Validate the FCS.  Generate the CRC across the entire frame, which 
        covers everything up to the FCS.  The current offset contains the 
        length of the frame.
       --------------------------------------------------------------------*/
      if( (crc_16_l_calc((byte *) fwd_frame_ptr->fr_item_ptr->data_ptr, 
                        offset+(RLP3_CTL_FCS_BYTE_LEN*8))) != CRC_16_L_OK)
      {
        /*-------------------------------------------------------------------
          Failed FCS check.  Treat as erasure and return.  But first go 
          remove any NAKs that were put into the retransmit list.  Do we
          need to reset all the variables we just set?
         ------------------------------------------------------------------*/
        for (i=0; i< naks_inserted_in_rtxq; i++)
        {
          dsrlprtx_remove_last_element( rrcb_ptr);
        }

        DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Bad FCS for NAK frame, type=%d, ",
                      nak_type);
        dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
	DATA_RLP_MSG0(MSG_LEGACY_LOW, "return RLP_ERASURE");
        return (DSRLPI_RLP_ERASURE);
      }

      /*---------------------------------------------------------------------
        CRC has passed... see if RLP needs to be re-synced due to a BS NAK
        which contained bad information.
       --------------------------------------------------------------------*/
      if (resync_rlp == TRUE)
      {
        DATA_RLP_MSG1(MSG_LEGACY_HIGH, "Bad NAK from BS - resync RLP:%d",rrcb_ptr->cfg_ptr->sr_id);
        dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
        dsrlpi_reset(rrcb_ptr);
        /*-------------------------------------------------------------------
           Report RLP reset event
        --------------------------------------------------------------------*/
        rlp_event_payload.event_rlp_type = event_rlp_reset;
        rlp_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
        event_report_payload (EVENT_RLP, 
                              sizeof(rlp_event_payload),
                              &rlp_event_payload);
        return(DSRLPI_NAK);
      }
     
      /*---------------------------------------------------------------------
        Build sequence number in internal representation.  This 
        number represents the peer V(N).
       --------------------------------------------------------------------*/
      DSRLPSEQ_SEQ_GEN_FROM_SEQ_HI(rrcb_ptr->state.peer_vn, seq_hi, 
                                   fr_seq_num);


      /*---------------------------------------------------------------------
        See if the calc'd V(n) is appropriate.  Know frame is not mal-formed,
        as the CRC has passed.
      ---------------------------------------------------------------------*/
      if (DSRLPSEQ_GT_SEQ_CHECK(rrcb_ptr->state.peer_vn,
                                rrcb_ptr->state.next_seq_num,
                                rrcb_ptr->cfg_ptr->seq_modulus ))
      {
        DATA_RLP_MSG3(MSG_LEGACY_HIGH, "RESET:  BS V(n) %x > MS V(S) %x RLP: %d",
                 rrcb_ptr->state.peer_vn, 
                 rrcb_ptr->state.next_seq_num, rrcb_ptr->cfg_ptr->sr_id);
        dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
        dsrlpi_reset(rrcb_ptr);
        /*-------------------------------------------------------------------
           Report RLP reset event
        --------------------------------------------------------------------*/
        rlp_event_payload.event_rlp_type = event_rlp_reset;
        rlp_event_payload.info = rrcb_ptr->cfg_ptr->sr_id;
        event_report_payload (EVENT_RLP, 
                              sizeof(rlp_event_payload),
                              &rlp_event_payload);
        return(DSRLPI_NAK);
      }

      /*---------------------------------------------------------------------
        Cleanup the transmit queue since we have an updated L_V(N).
       --------------------------------------------------------------------*/
      DATA_RLP_MSG2(MSG_LEGACY_HIGH, "BS V(n) via NAK = 0x%x %d",rrcb_ptr->state.peer_vn,rrcb_ptr->cfg_ptr->sr_id);

      /*---------------------------------------------------------------------
        All of the NAK processing has actually been performed already, so
        there is no subsequent processing to be performed.  Return that frame
        is finished.
       --------------------------------------------------------------------*/
      dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
      return (DSRLPI_NAK);

    } /* NAK frame */

    /*-----------------------------------------------------------------------
      At this point, the frame can only be an idle or fill frame, otherwise
      it is an erasure.  Continue processing.
     ----------------------------------------------------------------------*/
    if ( (ctl_byte & RLP3_IDLEFILL_CTL_MASK) == RLP3_IDLE_CTL_FLD )
    {
      /*---------------------------------------------------------------------
        Idle frame.  Remove the 8-bit SEQ field.  Also, remove the 4-bit CTL
        field and the 4-bit SEQ_HI field (these can be removed within the
        same byte).  The lower 4 bits of the removed ctl_byte contain the 
        SEQ_HI field.  Use both of these to compute the sequence number 
        using internal representation.
       --------------------------------------------------------------------*/
      (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &fr_seq_num, 1);
      curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_MASK;
      (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &ctl_byte, 1);
      seq_hi = (ctl_byte & 0x0F);
      curr_fr_info_ptr->fr_cat |= CURR_FRAME_SEQ_HI_MASK;
      DSRLPSEQ_SEQ_GEN_FROM_SEQ_HI(curr_fr_info_ptr->seq_num, seq_hi, 
                                   fr_seq_num);
      return (DSRLPI_IDLE);
       
    }
    else if ( (ctl_byte & RLP3_IDLEFILL_CTL_MASK) == RLP3_FILL_CTL_FLD )
    {
      /*---------------------------------------------------------------------
        Fill frame.  Remove the 8-bit SEQ field.  Also, remove the 4-bit CTL
        field and the 4-bit SEQ_HI field (these can be removed within the
        same byte).  The lower 4 bits of the extracted ctl_byte contain the 
        SEQ_HI field.  Use both of these to compute the peer VN sequence 
        number using internal representation.  Note that fill frames are only
        used as window updates of the remote V(N), so there is no subsequent
        processing to be performed.  Update the remote V(N) and return.
       --------------------------------------------------------------------*/
      (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &fr_seq_num, 1);
      (void)dsm_pullup( &(fwd_frame_ptr->fr_item_ptr), &ctl_byte, 1);
      seq_hi = (ctl_byte & 0x0F);

      tmp_seq = 0;
      DSRLPSEQ_SEQ_GEN_FROM_SEQ_HI(tmp_seq, seq_hi, fr_seq_num);

      if (DSRLPSEQ_GE_SEQ_CHECK(rrcb_ptr->state.next_seq_num,
                                tmp_seq,
                                rrcb_ptr->cfg_ptr->seq_modulus))
      {
        /*-------------------------------------------------------------------
          Peer Vn makes sense, so clean up rexmit array.
         ------------------------------------------------------------------*/
        rrcb_ptr->state.peer_vn = tmp_seq;
      }

      rrcb_ptr->stats.rx_fill_fr_cnt++;

      /*---------------------------------------------------------------------
        Free the received frame and return from this function.
       --------------------------------------------------------------------*/
      dsm_free_packet( &(fwd_frame_ptr->fr_item_ptr));
      return (DSRLPI_FILL);
    }
    else
    {
      /*---------------------------------------------------------------------
        Erased frame.  Log it and return.  Only print msg if frame looks to
        be something other than the bottom 6 bits of a NULL.
       --------------------------------------------------------------------*/
      if (ctl_byte != 0x3F)
      {
         DATA_RLP_MSG2(MSG_LEGACY_HIGH, "Unknown frame RLP3 fmt 0x%x RLP: %d", ctl_byte, rrcb_ptr->cfg_ptr->sr_id);
      }
      
      return (DSRLPI_RLP_ERASURE);
    }
  }  
} /* rlp3_rx_frame_det() */

/*===========================================================================

FUNCTION       RLP3_RX_IDLE_FRAME

DESCRIPTION    This function receives an idle frame from the base station
               and processes it.  If there is skew in the incoming seq
               number, then some Nak generation may occur.
               
               This code is specific to RLP 3.
               
DEPENDENCIES   Input pointers are non-NULL.

RETURN VALUE   Always returns DONE.

SIDE EFFECTS   NONE
===========================================================================*/
dsrlpi_fr_det_enum_type rlp3_rx_idle_frame
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,     /* Ptr to RLP ctl block      */
  dsrlpi_curr_fr_info_type     *curr_info_ptr /* frame info ptr            */
)
{
  uint32                        modulus;      /*     sequence modulus      */
  uint32                        nak_gap_seq;  /* v(R) + big_nak_gap        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT((rrcb_ptr != NULL) &&
         (curr_info_ptr != NULL));

  modulus = rrcb_ptr->cfg_ptr->seq_modulus;

  if (curr_info_ptr->seq_num == rrcb_ptr->state.expected_seq_num)
  {
    rrcb_ptr->state.old_frames = 0;
    rrcb_ptr->state.big_nak_gap = FALSE;
    return DSRLPI_DONE; 
  }
  
  if (DSRLPSEQ_GE_SEQ_CHECK(curr_info_ptr->seq_num,
                            rrcb_ptr->state.expected_seq_num,
                            modulus))
  {
    rrcb_ptr->state.old_frames = 0;
    /*-----------------------------------------------------------------------
      If the delayed frame array(s) are non-empty, then this idle may cause
      an erroneous NAK.  Wait to NAK until all delayed frames are processed
    -----------------------------------------------------------------------*/
    if (dsrlpdel_q_empty(rrcb_ptr->state.del_fr_id) == FALSE)
    {
      return( DSRLPI_DONE);
    }

    /*-----------------------------------------------------------------------
      If the new frame is greater than DSRLP_BIG_NAK_GAP from the prev, then
      mark it as strange, and see if next idle/data frame is also weird. 
      This handles cases of bad rate determination, which can lead to 
      erroneous NAKs.
    -----------------------------------------------------------------------*/

    nak_gap_seq = DSRLPSEQ_ADD_SEQ(rrcb_ptr->state.expected_seq_num,
                                   DSRLPI_BIG_NAK_GAP,
                                   modulus);

    if ((DSRLPSEQ_GE_SEQ_CHECK(curr_info_ptr->seq_num, nak_gap_seq, modulus)) &&
        (rrcb_ptr->state.big_nak_gap == FALSE)
       )
    {
      DATA_RLP_MSG1(MSG_LEGACY_HIGH, "RLP: %d", rrcb_ptr->cfg_ptr->sr_id);
      DATA_RLP_MSG3(MSG_LEGACY_HIGH, "big gap: V(R): 0x%x, V(N): 0x%x, idle seq: 0x%x",
               rrcb_ptr->state.expected_seq_num,
               rrcb_ptr->state.needed_seq_num, 
               curr_info_ptr->seq_num);
      rrcb_ptr->state.big_nak_gap = TRUE;
      return( DSRLPI_DONE);
    }
         
    /*-----------------------------------------------------------------------
      If the received value is greater than the expected, then make
      a new nak list entry.
    -----------------------------------------------------------------------*/
    if ((rrcb_ptr->state.nak_rounds_rev != 0) && (dsrlpnak_insert(rrcb_ptr,
                                          rrcb_ptr->state.expected_seq_num,
                                          curr_info_ptr->seq_num) == FALSE))
    {
      /*-----------------------------------------------------------------------
        Reset RLP.
      -----------------------------------------------------------------------*/
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Could not insert into nak list");
      rrcb_ptr->state.reset_rlp = TRUE;

    }
    else
    {
      rrcb_ptr->state.expected_seq_num = curr_info_ptr->seq_num;
      if (rrcb_ptr->state.nak_rounds_rev == 0)
      {
        rrcb_ptr->state.needed_seq_num = curr_info_ptr->seq_num;
      }
    }
  }
  else /* rcvd num is less than expected */
  {
    /*-----------------------------------------------------------------------
      Received frame has a sequence number less than what we expected.
      Go ahead and drop this frame on the floor.
    -----------------------------------------------------------------------*/
    DATA_RLP_MSG3(MSG_LEGACY_HIGH, "Rx idle w/old seq 0x%x V_S=0x%x RLP:%d",
              curr_info_ptr->seq_num,
              rrcb_ptr->state.expected_seq_num,rrcb_ptr->cfg_ptr->sr_id);
    rrcb_ptr->state.old_frames++;
  }                                   

  /*-------------------------------------------------------------------------
    If got this far, then idle was okay.  So go ahead an reset big_nak_gap
    value.
  -------------------------------------------------------------------------*/
  rrcb_ptr->state.big_nak_gap = FALSE;

  return (DSRLPI_DONE);
} /* rlp3_rx_idle_frame() */

/*===========================================================================

FUNCTION RLP3_RX_RLP_ERASURE

DESCRIPTION
  This function is called when RLP detects an erasure.  This means that the
  MUX layer thought that the frame was good, but for some reason RLP thought
  the frame was in error.
  This function increments statistics for erasures and also prints out the
  first few bytes of the frame (to help with debugging).
  
DEPENDENCIES
  Since NULL frames are erasures (and can come quite often), for efficiency
  it is assumed that frame_det has removed those frames, such that this
  function is not called when a NULL frame is rx'ed.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
dsrlpi_fr_det_enum_type rlp3_rx_rlp_erasure
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,         /* Ptr to RLP ctl block      */
  dsrlp_fwd_frame_type     *fwd_fr_ptr        /* ptr to received RLP frame */
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_RLP_MSG2(MSG_LEGACY_MED, "Rxed Erasure. Used  = %d RLP:%d",dsrlpi_ctl_used,rrcb_ptr->cfg_ptr->sr_id);
  DATA_RLP_MSG3(MSG_LEGACY_MED, "Rxed Erasure. Bytes = 0x%x 0x%x 0x%x",
           dsrlpi_ctl_byte1,dsrlpi_ctl_byte2,dsrlpi_ctl_byte3);
  return (DSRLPI_DONE);
} /* rlp3_rx_erasure() */

/*===========================================================================

FUNCTION RLP3_RX_MUX_ERASURE

DESCRIPTION
  This function is called when MUX passes an erasure up to RLP.  This
  function merely increments statistics, and prints out a message 
  periodically, indicating how many erasures were received.  
  
  Blanks are treated as erasures, and so are included in the count.
  
DEPENDENCIES
  None.
    
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
dsrlpi_fr_det_enum_type rlp3_rx_mux_erasure
(
  dsrlpi_rlp_ctl_blk_type  *rrcb_ptr,         /* Ptr to RLP ctl block      */
  dsrlp_fwd_frame_type     *fwd_fr_ptr        /* ptr to received RLP frame */
)
{
  rrcb_ptr->state.mux_erasure_cnt++;
  return (DSRLPI_DONE);
} /* rlp3_rx_mux_erasure() */

/*===========================================================================

FUNCTION       RLP3_TX_INIT_FRAME

DESCRIPTION    This function constructs the SYNC, SYNC/ACK, and ACK packets
               which are transmitted to the base station during RLP
               initialization.  The packets to transmit are based upon
               the current state of the RLP.
               
               This code is specific to RLP 3.
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               One reason for returning FALSE is if the max_frame_rate 
               possible is too small (control frames require 1/2 rate or
               better).  Frame must also be fundicated, or return FALSE.

SIDE EFFECTS   
===========================================================================*/
boolean rlp3_tx_init_frame
(
   dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,      /* ptr to RLP ctl block    */
   dsrlp_rev_frame_type         *rev_fr_ptr     /* ptr to frame to be built*/
)
{
   dsrlpi_state_info_type *state_ptr;           /* local state ptr         */
   byte                   *data_ptr;            /* ptr to rlp frame        */ 
   int                     bit_pos;             /* bit position in frame   */
   int                     octet_delta;         /* used to calc first set  */
                                                /* pad bits                */
   int                     byte_len;            /* length of frame,in bytes*/
   dsm_item_type          *item_ptr = NULL;     /* ptr to rlp frame payload*/
   byte                    frame_len;           /* length of frame, bytes  */
   dsrlpi_state_enum_type state_loc;            /* RLP State               */
   boolean ack_to_send_loc;                     /* pending ack to send     */
#ifdef RLP3_NAK_NEG_VIA_S_A
   int                     i;                   /* loop invariant          */

#endif /* RLP3_NAK_NEG_VIA_S_A */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------
    Also get a local ptr to the state vars as it is accessed often.  Should
    reduce pointer arithmetic. 
  ------------------------------------------------------------------------*/
  state_ptr = &rrcb_ptr->state;
  state_loc = state_ptr->state;
  ack_to_send_loc = state_ptr->ack_to_send;



  if ( !( ( (state_loc != DSRLPI_ESTABLISHED_STATE) &&
           (state_loc != DSRLPI_RT_MEAS_STATE)
         ) || (ack_to_send_loc == TRUE))
       ) 
  {
	  return(FALSE);
  }

  /*------------------------------------------------------------------------
    Figure out the minimum frame rate.  Minimum needed frame rate is 1/2.
  ------------------------------------------------------------------------*/
  if (dsrlpi_get_min_frame_and_len(rev_fr_ptr,
                                   DSRLP_RATE_1_4_MASK,
                                   &frame_len) == FALSE)
  {
    /*-----------------------------------------------------------------------
      Required frame could not be found
    -----------------------------------------------------------------------*/
    return(FALSE);
  }

  /*----------------------------------------------------------------------
    Need to store the ctl frame into a dsm item.  Get that item.  In
    all cases, should only need a small item.  Set data portion of
    item to all zero's, so that any padding issues are already taken 
    care of.  Use a local var to access the data portion of the item.
  -----------------------------------------------------------------------*/
  ASSERT(frame_len <= DSM_DS_SMALL_ITEM_SIZ);

  item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if (item_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Running out of DSM small items.");
    return (FALSE);
  }

  data_ptr = item_ptr->data_ptr;
  
  (void)memset(data_ptr,0,item_ptr->size);



  /*------------------------------------------------------------------------
    Since this is not an idle frame to be constructed, set idle_frame
    to FALSE.
  ------------------------------------------------------------------------*/
  rev_fr_ptr->idle_frame = FALSE;

  /*------------------------------------------------------------------------
    Grab 8 bit seq num to get sequence number.  Goes into first byte of ctl
    fld.
  ------------------------------------------------------------------------*/
  b_packb(DSRLPSEQ_GET_SEQ(state_ptr->next_seq_num),
          data_ptr,
          RLP3_CTL_SEQ_POS,
          RLP3_CTL_SEQ_LEN
         );

  /*------------------------------------------------------------------------
    Add in Control Field.  As part of control field processing, may have
    to set round trip frame counter to its default value.
  ------------------------------------------------------------------------*/
  switch(state_loc)
  {
    case DSRLPI_SYNC_STATE:
      b_packb((byte)RLP3_CTL_CTL_SYNC,
              data_ptr,
              RLP3_CTL_CTL_POS,
              RLP3_CTL_CTL_LEN
             );
      break;

    case DSRLPI_SYNC_ACK_STATE:
      b_packb((byte)RLP3_CTL_CTL_SYNC_ACK,
              data_ptr,
              RLP3_CTL_CTL_POS,
              RLP3_CTL_CTL_LEN
             );
      state_ptr->rnd_trip_fr_cntr = DSRLPI_RND_TRIP_FR_CNTR_DEF;
      break;

    case DSRLPI_ACK_STATE:
      b_packb((byte)RLP3_CTL_CTL_ACK,
              data_ptr,
              RLP3_CTL_CTL_POS,
              RLP3_CTL_CTL_LEN
             );
      state_ptr->rnd_trip_fr_cntr = DSRLPI_RND_TRIP_FR_CNTR_DEF;
      state_ptr->ack_to_send = FALSE;
      DATA_RLP_MSG2(MSG_LEGACY_MED, "In RLP state %d, "
                    "setting ack_to_send = %d",
                    state_ptr->state,
                    state_ptr->ack_to_send);
      break;

    case DSRLPI_RT_MEAS_STATE:
    case DSRLPI_ESTABLISHED_STATE:
      if (ack_to_send_loc == TRUE)
      {
        b_packb((byte)RLP3_CTL_CTL_ACK,
                 data_ptr,
                 RLP3_CTL_CTL_POS,
                 RLP3_CTL_CTL_LEN
               );
        state_ptr->ack_to_send = FALSE;
        DATA_RLP_MSG2(MSG_LEGACY_MED, "In RLP state %d, "
                      "setting ack_to_send = %d",
                      state_ptr->state,
                      state_ptr->ack_to_send);
        break;
      }
      /* fall through */
    default:
      DATA_RLP_MSG1(MSG_LEGACY_ERROR, "Invalid RLP state %d",
                        (int)state_ptr->state);
      ASSERT(0);
      break;
  }

  /*------------------------------------------------------------------------
    Reset bit.  Since everything done via RLP BLOB, there is no need to 
    include NAK params, etc (unless the feature is enabled).  
  ------------------------------------------------------------------------*/
  b_packb((byte)RLP3_SA_RESET,
          data_ptr,
          RLP3_SA_RESET_POS,
          RLP3_SA_RESET_LEN
         );

/*lint -save -e831 */ 
  bit_pos = RLP3_SA_NAKS_VIA_BLOB_FCS_POS;
/*lint -restore Restore lint error 831*/

#ifdef RLP3_NAK_NEG_VIA_S_A
  if ((state_ptr->state == DSRLPI_SYNC_ACK_STATE) ||
      (state_ptr->state == DSRLPI_ACK_STATE))
  {
    /*----------------------------------------------------------------------
      If doing NAK negotiation via Sync/Acks, then set teh nak_param incl
      bit in S/A and A msgs.
    ----------------------------------------------------------------------*/
    b_packb((byte)1
            data_ptr,
            RLP3_SA_NAK_PARAM_INC_POS,
            RLP3_SA_NAK_PARAM_INC_LEN
           );
  }

  /*------------------------------------------------------------------------
    Nak rounds fwd & rev.
  ------------------------------------------------------------------------*/
  ASSERT(state_ptr->nak_rounds_fwd < (1 << RLP3_SA_NAK_RNDS_FWD_LEN));

  b_packb((byte)(state_ptr->nak_rounds_fwd),
          data_ptr,
          RLP3_SA_NAK_RNDS_FWD_POS,
          RLP3_SA_NAK_RNDS_FWD_LEN
         );

  ASSERT(state_ptr->nak_rounds_rev < (1 << RLP3_SA_NAK_RNDS_REV_LEN));

  b_packb((byte)(state_ptr->nak_rounds_rev),
          data_ptr,
          RLP3_SA_NAK_RNDS_REV_POS,
          RLP3_SA_NAK_RNDS_REV_LEN
         );
    
  /*------------------------------------------------------------------------
    Add in naks per round, forward.  Start maintaining a bit count, so
    that future padding and b_packb's can be done.
  ------------------------------------------------------------------------*/
  bit_pos = RLP3_SA_NAKS_PER_RND_FWD_POS;

  for (i = 0; i < state_ptr->nak_rounds_fwd; i++)
  {
    b_packb((byte)(state_ptr->naks_per_round_fwd[i]),
            data_ptr,
            bit_pos,
            RLP3_SA_NAKS_PER_RND_FWD_LEN
           );
    bit_pos += RLP3_SA_NAKS_PER_RND_FWD_LEN;
  }

  /*-------------------------------------------------------------------------
    Do loop for naks per round, reverse.
  -------------------------------------------------------------------------*/
  for (i = 0; i < state_ptr->nak_rounds_rev; i++)
  {
    b_packb((byte)(state_ptr->naks_per_round_rev[i]),
            data_ptr,
            bit_pos,
            RLP3_SA_NAKS_PER_RND_REV_LEN
           );
    bit_pos += RLP3_SA_NAKS_PER_RND_REV_LEN;
  }
#endif /* RLP3_NAK_NEG_VIA_S_A */

  /*-------------------------------------------------------------------------
    Do the padding, so that the FCS will be octet-aligned.  bit_pos
    contains the next bit which should be written to.  Find next octet-
    aligned bit position, and convert to bytes.
  -------------------------------------------------------------------------*/
  /*lint -save -e831*/
  octet_delta = bit_pos - ((bit_pos/(int)BITS_IN_OCTET) * (int)BITS_IN_OCTET);
  /*lint -restore Restore lint error 831*/

  /*lint -save -e774*/
  if (octet_delta != 0)
  {
    /*-----------------------------------------------------------------------
       Since data_ptr was memset'd to zero, it is not necessary to
       explicitly pad out with zero's; the zero's are already there.
       The only action to take is to update bit_pos so that it is
       aligned to the next octet.
     ----------------------------------------------------------------------*/
     bit_pos += BITS_IN_OCTET - octet_delta;
  }
 /*lint -restore Restore lint error 774*/

  ASSERT((bit_pos - ((bit_pos/(int)BITS_IN_OCTET) * (int)BITS_IN_OCTET)) == 0);

  /*-------------------------------------------------------------------------
    Calculate FCS over these bytes.  Function automatically places CRC into
    frame.
  -------------------------------------------------------------------------*/
  byte_len = (bit_pos/(int)BITS_IN_OCTET) + (int)RLP3_CTL_FCS_BYTE_LEN;
  dsrlpi_crc_gen(data_ptr, byte_len);

  /*-------------------------------------------------------------------------
    Now that all of the data is copied into the frame, and the rest of
    bits are zero'd out, need to determine if frame must be inside a
    Format A frame.  If so, AND in the appropriate bitmask to the last
    byte in the frame, in order to set the Format A type bits.
  -------------------------------------------------------------------------*/
  if( rev_fr_ptr->fr_rate == DSRLP_RATE_1)
  {
    item_ptr->data_ptr[ frame_len ] |=
                            dsrlpi_type_mask_table[rev_fr_ptr->mux_pdu_type]
                                                  [rev_fr_ptr->srvc_type]
                                                  .format_a; 
  }

  /*-------------------------------------------------------------------------
    Update field in reverse frame ptr and update fields of the item.
  -------------------------------------------------------------------------*/
  rev_fr_ptr->fr_item_ptr = item_ptr;
  item_ptr->used          = dsrlp_get_used_frame_len(rev_fr_ptr->fr_rate,
                                                     rev_fr_ptr->srvc_type,
                                                     rev_fr_ptr->mux_pdu_type);
//  item_ptr->kind        = DSM_DSRLP_TX  
  
  /*------------------------------------------------------------------------
    All DONE.
  ------------------------------------------------------------------------*/
  return TRUE;
} /* rlp3_tx_init_frame() */

/*===========================================================================

FUNCTION       RLP3_TX_NAKS

DESCRIPTION    This function creates a NAK frame based on the information in
               the NAK list.
               
               To reduce complexity, only Type '00' and '10' Naks are used.
               Type '00' specifies a single range of unsegmented frames.
               Type '10' specifies multiple ranges of missing segmented
               frames.
               
               This code is specific to RLP 3.
               
DEPENDENCIES   Input pointers are non-NULL.  Only called from Tx-task.

RETURN VALUE   TRUE if a nak frame was generated, else returns FALSE.

SIDE EFFECTS   NONE
===========================================================================*/
boolean rlp3_tx_naks
(
  dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,  /* Ptr to RLP ctl block        */
  dsrlp_rev_frame_type         *rev_fr_ptr /* Ptr to frame to be built    */
)
{
  dsrlpi_state_info_type *state_ptr;           /* local state ptr         */
  byte                   *data_ptr;            /* ptr to rlp frame        */ 
  dsm_item_type          *item_ptr = NULL;     /* ptr to rlp frame payload*/
  byte                    frame_len;           /* length of frame, bytes  */
  uint32                  nak_id;              /* nak list entry id       */
  byte                    nak_type;            /* type of nak to xmit     */
  word                    bits_left;           /* space left in the frame */
  word                    bits_used;           /* bits used in the frame  */
  word                    bytes_used;          /* bytes used in the frame */
  boolean                 ret_id;              
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT((rrcb_ptr != NULL) &&
         (rev_fr_ptr != NULL));
  
  /*------------------------------------------------------------------------
    Get a local ptr to the state, as it is accessed often.  Should reduce
    pointer arithmetic. 
  ------------------------------------------------------------------------*/
  state_ptr = &rrcb_ptr->state;

  /*------------------------------------------------------------------------
    Make sure there are nak entries requiring transmission.
  ------------------------------------------------------------------------*/
  if (state_ptr->naks_to_send == FALSE)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Called tx_naks when boolean false");
    return FALSE;
  }

  nak_id = dsrlpnak_get_nak(rrcb_ptr,0,TRUE);

  /*-------------------------------------------------------------------------
    If no NAKs were found, then return from this function and try to build
    another frame.
    Note, this will happen quite frequently.
  -------------------------------------------------------------------------*/
  if (nak_id == 0)
  {
    state_ptr->naks_to_send = FALSE;
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Make sure frame is fundicated (fundamental channel or dedicated control
    channel).  NAK frames only allowed on fundicated channels.  It is an
    error if the rev_frame is not fundicated.  Upper layer will send BLANK.
  ------------------------------------------------------------------------*/
//  if (rev_fr_ptr->fundicated == FALSE) 
//  {
//    ERR("Trying to xmit idle on supp chan",0,0,0);
//    return FALSE;
//  }

  /*------------------------------------------------------------------------
    Figure out the minimum frame rate and frame length in bytes.  It is 
    an error if the frame is no good.  Upper layer processing will send
    BLANK.
    
    For a NAK frame, the minimum needed number of bits is 80 (Type '10' NAK,
    with only one bitmap being sent).  Try to get a full-rate, just so
    more NAKs can be sent.  If not, then settle for 1/2 rate.
  ------------------------------------------------------------------------*/
  if (dsrlpi_get_min_frame_and_len(rev_fr_ptr,
                                   DSRLP_RATE_1_MASK,
                                   &frame_len) == FALSE)
  {
    if (dsrlpi_get_min_frame_and_len(rev_fr_ptr,
                                     DSRLP_RATE_1_2_MASK,
                                     &frame_len) == FALSE)
    {
      DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Rev fr too small for NAK frame Tx");
      return(FALSE);
    }
  }

  /*----------------------------------------------------------------------
    Need to store the NAK frame into a dsm item.  Get that item.  In
    all cases, should only need a small item.  Set data portion of
    item to all zero's, so that any padding issues are already taken 
    care of.  Use a local var to access the data portion of the item.
  -----------------------------------------------------------------------*/
  ASSERT(frame_len <= DSM_DS_SMALL_ITEM_SIZ);

  item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if (item_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Running out of DSM small items.");
    return (FALSE);
  }

  data_ptr = item_ptr->data_ptr;

  (void)memset(data_ptr,0,item_ptr->size);

  /*------------------------------------------------------------------------
    Since this is not an idle frame to be constructed, set idle_frame to
    FALSE.
  ------------------------------------------------------------------------*/
  rev_fr_ptr->idle_frame = FALSE;

  /*------------------------------------------------------------------------
    Grab 8 bit seq num to get sequence number.  Goes into first byte of ctl
    fld.
  ------------------------------------------------------------------------*/
  b_packb(DSRLPSEQ_GET_SEQ(state_ptr->needed_seq_num),
          data_ptr,
          RLP3_CTL_SEQ_POS,
          RLP3_CTL_SEQ_LEN
         );

  /*------------------------------------------------------------------------
    Specify control field to identify a NAK control frame.
  ------------------------------------------------------------------------*/
  b_packb((byte)RLP3_CTL_CTL_NAK,
          data_ptr,
          RLP3_CTL_CTL_POS,
          RLP3_CTL_CTL_LEN
         );

  /*------------------------------------------------------------------------
    Figure out NAK type.  It can be either Type '00' (range of full
    segments) or Type '10' (ranges for segmented frames).  Make decision
    based on first nak list entry requiring xmission (look to see if entry
    has S_SEQ's). 
  ------------------------------------------------------------------------*/
  if ((DSRLPSEQ_HAS_S_SEQ(DSRLPNAK_GET_FIRST_SEQ(nak_id))) ||
      (DSRLPSEQ_HAS_S_SEQ(DSRLPNAK_GET_LAST_SEQ(nak_id))))
  {
    b_packb((byte)RLP3_NAK_NAK_TYPE_2,
            data_ptr,
            RLP3_NAK_NAK_TYPE_POS,
            RLP3_NAK_NAK_TYPE_LEN
           );
    nak_type = RLP3_NAK_NAK_TYPE_2;
  }
  else
  {
    b_packb((byte)RLP3_NAK_NAK_TYPE_0,
            data_ptr,
            RLP3_NAK_NAK_TYPE_POS,
            RLP3_NAK_NAK_TYPE_LEN
           );
    nak_type = RLP3_NAK_NAK_TYPE_0;
  }

  /*------------------------------------------------------------------------
    Store SEQ_HI.
  ------------------------------------------------------------------------*/
  b_packb(DSRLPSEQ_GET_SEQ_HI(state_ptr->needed_seq_num),
          data_ptr,
          RLP3_NAK_SEQ_HI_POS,
          RLP3_NAK_SEQ_HI_LEN
         );

  /*------------------------------------------------------------------------
    At this point, processing is different for Type '00' and Type '10'. 
    For type '00', a single range is used.
    
    For type '10', up to four S_SEQ ranges can be specified.  Loop until
    either all NAK's are stored for transmission, or the frame runs out
    of memory.
    
    Calculate the number of bits left for storing each set of NAK info.  
    Make sure to reserve space for the FCS.
  ------------------------------------------------------------------------*/

  bits_left = (frame_len * BITS_IN_OCTET) - (RLP3_CTL_SEQ_LEN      + 
                                             RLP3_CTL_CTL_LEN      +
                                             RLP3_NAK_NAK_TYPE_LEN +
                                             RLP3_NAK_SEQ_HI_LEN   +
                               RLP3_CTL_FCS_BYTE_LEN * BITS_IN_OCTET);

  /*------------------------------------------------------------------------
    Based on the NAK type, build the nak-portion of the frame.  For 
    NAK type '10', the nak seg count will also be filled in by the 
    sub-routine.
  ------------------------------------------------------------------------*/
  if (nak_type == RLP3_NAK_NAK_TYPE_0)
  {
    ret_id = dsrlp3i_bld_naktype0(rrcb_ptr,data_ptr,&bits_left);
  }
  else
  {
    ret_id = dsrlp3i_bld_naktype2(rrcb_ptr,data_ptr,&bits_left);
  }

  if (ret_id == FALSE)
  {
    state_ptr->naks_to_send = FALSE;
    dsm_free_buffer( item_ptr );
    DATA_RLP_MSG0(MSG_LEGACY_ERROR,"No naks to send, so returning false.");
    return FALSE;
  }

  /*------------------------------------------------------------------------
    Nak information has been inserted.  Figure out where the FCS should
    be inserted in the byte stream.  
  ------------------------------------------------------------------------*/
  bits_left += (RLP3_CTL_FCS_BYTE_LEN * BITS_IN_OCTET);

  bits_used = (frame_len * BITS_IN_OCTET) - bits_left;

  bytes_used = bits_used / BITS_IN_OCTET;

  /*------------------------------------------------------------------------
    Due to division, a remainder could have been lost.  Go ahead and see
    if bytes_used needs to be incremented once more.
  ------------------------------------------------------------------------*/
  if ((bits_used - (bytes_used * BITS_IN_OCTET)) > 0 )
  {
    bytes_used++;
  }

  /*-------------------------------------------------------------------------
    Calculate FCS over these bytes.  Function automatically places CRC into
    frame.  bytes_used needs to be pre-incremented to include FCS byte length
  -------------------------------------------------------------------------*/

  bytes_used += RLP3_CTL_FCS_BYTE_LEN;
  dsrlpi_crc_gen(data_ptr,bytes_used);

  /*-------------------------------------------------------------------------
    If a full rate frame was allocated, check to see if the frame supports
    1/2 rate, and if the frame would fit into half rate.  This can save
    some space.
  -------------------------------------------------------------------------*/
  if (dsrlpi_get_best_fr_rate(rev_fr_ptr,bytes_used) == DSRLP_RATE_1_2)
  {
    (void)dsrlpi_get_min_frame_and_len(rev_fr_ptr, 
                                       DSRLP_RATE_1_2_MASK,
                                       &frame_len);

    ASSERT(rev_fr_ptr->fr_rate == DSRLP_RATE_1_2);
  }

  /*-------------------------------------------------------------------------
    Now that all of the data is copied into the frame, and the rest of
    bits are zero'd out, need to determine if frame must be inside a
    Format A frame. 
    If full rate frame is being sent, then set the type bits correctly.
  -------------------------------------------------------------------------*/
  if( rev_fr_ptr->fr_rate == DSRLP_RATE_1)
  {
    item_ptr->data_ptr[ frame_len++] |=
                            dsrlpi_type_mask_table[rev_fr_ptr->mux_pdu_type]
                                                  [rev_fr_ptr->srvc_type]
                                                  .format_a;
  }

  /*------------------------------------------------------------------------
    Padding after FCS already done by memset when dsm item first allocated.
    Update the dsm_item's used field, and finish.
  ------------------------------------------------------------------------*/
  item_ptr->used          = frame_len;
  rev_fr_ptr->fr_item_ptr = item_ptr;

  return TRUE;
} /* rlp3_tx_naks() */


/*===========================================================================

FUNCTION       RLP3_TX_IDLE_FRAME

DESCRIPTION    This function constructs an idle frame and transmits it to
               the other side.
               
               This code is specific to RLP 3.
               
DEPENDENCIES   Since idle frames can only go on fundicated channels,
               the assumption is that only one idle frame can be sent
               per 20msec frame-time.
               
               rrcb_ptr->state.idle_timer is only decremented in this
               function.

RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               Returning FALSE in this case implies a Blank frame should
               be supplied to the MUX layer.

SIDE EFFECTS   NONE
===========================================================================*/
boolean rlp3_tx_idle_frame
(
   dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,  /* Ptr to RLP ctl block        */
   dsrlp_rev_frame_type         *rev_fr_ptr /* Ptr to frame to be built    */
)
{
   byte                    frame_len;       /*  length of frame, bytes     */
   dsm_item_type          *item_ptr;        /*  ptr to rlp data frame      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------
    Make sure frame is fundicated (fundamental channel or dedicated control
    channel).  Idle frames only allowed on fundicated channels.  It is an
    error if the rev_frame is not fundicated.  Upper layer will send BLANK;
    however, since frame is no good, idle timer processing will not be done
    by upper layer.
  ------------------------------------------------------------------------*/
//  if (rev_fr_ptr->fundicated == FALSE) 
//  {
//    ERR("Trying to xmit idle on supp chan",0,0,0);
//    return FALSE;
//  }

  /*------------------------------------------------------------------------
    Figure out the minimum frame rate and frame length in bytes.  It is 
    an error if the frame is no good.  Upper layer processing will send
    BLANK; however, since the frame is no good, idle timer processing will
    not be done by upper layer.
  ------------------------------------------------------------------------*/
  if (dsrlpi_get_min_frame_and_len(rev_fr_ptr,
                                   DSRLP_RATE_1_16_MASK,
                                   &frame_len) == FALSE)
  {
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, " Only %d bytes to tx idle-not enough RLP:%d", 
             frame_len, rrcb_ptr->cfg_ptr->sr_id);
    return(FALSE);
  }

  /*------------------------------------------------------------------------
    If doing secondary traffic, only generate an idle if frame
    counter > 0.  Why:  Fundicated is either fundamental or DCCH.  Idle
    frames can only be clobbered by Mux layer on DCCH.  So Idle frames on
    primary traffic which go out on fundamental are never clobbered. Ditto
    for Idle frames on secondary traffic.  i.e., idle frames on 2ndary
    traffic *may not* be clobbered by MUX layer.  It is considered bad to 
    send idle frames on secondary traffic, and so RLP makes the decision
    to send/not send an IDLE frame to the MUX layer when secondary traffic.
  ------------------------------------------------------------------------*/

#ifndef FEATURE_IS2000_REL_A_SVD
  /*------------------------------------------------------------------------
    For secondary :
    If secondary only exisits on FCH then the following code will prevent
    MS from sending any idle data. So, this code is not needed. The 
    following code is useful if, FCH has primary service and Data is 
    secondary service then Data need not send IDLE frames always. Its okay
    to send once a while.
    This featurization will be changed once an efficient algorithm is used.
  ------------------------------------------------------------------------*/
  if ((rev_fr_ptr->srvc_type == DSRLP_SECONDARY_SRVC) &&
      (rrcb_ptr->state.rnd_trip_fr_cntr == 0))
  {
    /*----------------------------------------------------------------------
      Should send BLANK to MUX layer.  However when sending blank frames,
      the idle timer must be decremented.  See if decrementing the idle
      timer causes it to expire.  If so, continue on and send IDLE as DATA
      frame.  Otherwise, return FALSE (and cause BLANK to be sent to MUX).
    ----------------------------------------------------------------------*/
    if (rrcb_ptr->state.idle_timer_active == TRUE)
    {
      ASSERT(rrcb_ptr->state.idle_timer > 0);

      if (rrcb_ptr->state.idle_timer != 1)
      {
        rrcb_ptr->state.idle_timer--; 
        return FALSE;
      }
    }
    else
    {
      return FALSE;
    }
  }
#endif /* FEATURE_IS2000_REL_A_SVD */
  /*------------------------------------------------------------------------
    Created the DSM item at this point as we are sure that we are going 
    to make an idle frame now. Also the assert has been moved from below to
    this location to make sure we don't hit the assert before creating an item.
  ----------------------------------------------------------------------------*/  
  ASSERT(frame_len <= DSM_DS_SMALL_ITEM_SIZ);
  item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);

  if (item_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Running out of DSM Small items");
    return (FALSE);
  }
  
  memset(item_ptr->data_ptr,0,item_ptr->size);
  
 
  /*------------------------------------------------------------------------
    Input frame is okay.  An idle frame is sent to the MUX layer as a data
    frame whenever the round trip frame counter > 0.  The DCCH is DTX
    (discontinuous transmission), and so can drop idle frames when it wants.
    By masquerading as a data frame, the idle is still sent.  Sending the
    idle guarantees that the base station will be able to complete its
    round-trip delay calculation.  
    
    Another reason for sending idle as data is in the case where a stream
    of data has just been sent.  To insure the last few frames are fully
    xmitted, some trailing idles are sent afterwards.  Again, sending idle
    as data prevents the MUX from clobbering idle frames on the DCCH.
  ------------------------------------------------------------------------*/
  if (rrcb_ptr->state.idle_timer_active == TRUE)
  {
    ASSERT(rrcb_ptr->state.idle_timer > 0);
    rrcb_ptr->state.idle_timer--;
  }

  if ((rrcb_ptr->state.rnd_trip_fr_cntr > 0)       ||
      ((rrcb_ptr->state.idle_timer_active == TRUE) && 
       (rrcb_ptr->state.idle_timer == 0)))
  {
    /*----------------------------------------------------------------------
      Need to send an idle as a data frame, since at least one of the
      conditions has triggered.  Then modify the state vars according to
      standard.
    ----------------------------------------------------------------------*/ 
    rev_fr_ptr->idle_frame = FALSE;

    if (rrcb_ptr->state.rnd_trip_fr_cntr > 0)
    {
      rrcb_ptr->state.rnd_trip_fr_cntr--;
    }

    if (rrcb_ptr->state.idle_timer_active == TRUE)
    {
      rrcb_ptr->state.idle_timer = DSRLPI_IDLE_TIMER_DEF;
      rrcb_ptr->state.idle_fr_xmit_cntr++;

      if (rrcb_ptr->state.idle_fr_xmit_cntr == DSRLPI_IDLE_FR_XMIT_CNTR_DEF)
      {
        rrcb_ptr->state.idle_timer_active = FALSE;
      }
    }
  }
  else
  {
    rev_fr_ptr->idle_frame = TRUE;
  }

  /*------------------------------------------------------------------------
    Actually construct the frame.  An idle frame can be constructed using
    AND's and OR's, instead of using b_packb calls. 
  ------------------------------------------------------------------------*/
  
  /*----------------------------------------------------------------------
    Copy in the least sig. bits of the L_V(S)
  -----------------------------------------------------------------------*/
  item_ptr->data_ptr[0] = 
                    DSRLPSEQ_GET_SEQ(rrcb_ptr->state.next_seq_num);

  /*----------------------------------------------------------------------
    The next byte is a bitwise OR of the CTL field (top 4 bits) and the
    4 most sig. bits of L_V(S) (bottom 4 bits).
  -----------------------------------------------------------------------*/

  item_ptr->data_ptr[1]  = 
                  DSRLPSEQ_GET_SEQ_HI(rrcb_ptr->state.next_seq_num);

  item_ptr->data_ptr[1] |= RLP3_IDLE_CTL_FLD_SHIFTED;

  /*----------------------------------------------------------------------
    Now that all of the data is copied into the frame, and the rest of
    bits are zero'd out, need to determine if frame must be inside a
    Format A frame.  This is true when Rate 1 frame and idle being sent
    as a data frame.  If so, OR in the appropriate bitmask to the last
    byte in the frame, in order to set the Format A type bits.
  -----------------------------------------------------------------------*/
  if( rev_fr_ptr->fr_rate == DSRLP_RATE_1)
  {
    item_ptr->data_ptr[ frame_len++] |=
                            dsrlpi_type_mask_table[rev_fr_ptr->mux_pdu_type]
                                                  [rev_fr_ptr->srvc_type]
                                                  .format_a;
  }

  /*----------------------------------------------------------------------
    Update field in reverse frame ptr and return TRUE.
  -----------------------------------------------------------------------*/
  item_ptr->used = frame_len;
  rev_fr_ptr->fr_item_ptr = item_ptr;

  return(TRUE);
} /* rlp3_tx_idle_frame() */


/*===========================================================================

FUNCTION       RLP3_TX_FILL_FRAME

DESCRIPTION    This function constructs an fill frame and transmits it to
               the other side.
               
               This code is specific to RLP 3.
               
DEPENDENCIES   Since fill frames can only go on fundicated channels,
               the assumption is that only one fill frame can be sent
               per 20msec frame-time.  Assumes that have already tried
               to send an idle previously.
               
RETURN VALUE   TRUE if the frame could be constructed, else returns FALSE.
               Returning FALSE in this case implies a Blank frame should
               be supplied to the MUX layer.

SIDE EFFECTS   NONE
===========================================================================*/
boolean rlp3_tx_fill_frame
(
   dsrlpi_rlp_ctl_blk_type      *rrcb_ptr,  /* Ptr to RLP ctl block        */
   dsrlp_rev_frame_type         *rev_fr_ptr /* Ptr to frame to be built    */
)
{
   byte                    frame_len;       /*  length of frame, bytes     */
   dsm_item_type          *item_ptr;        /*  ptr to rlp data frame      */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*------------------------------------------------------------------------
    Make sure frame is fundicated (fundamental channel or dedicated control
    channel).  Fill frames only allowed on fundicated channels.  It is an
    error if the rev_frame is not fundicated.  Upper layer will send BLANK.
  ------------------------------------------------------------------------*/
//  if (rev_fr_ptr->fundicated == FALSE) 
//  {
//    ERR("Trying to xmit idle on supp chan",0,0,0);
//    return FALSE;
//  }

  /*------------------------------------------------------------------------
    Figure out the minimum frame rate and frame length in bytes.  It is 
    an error if the frame is no good.
  ------------------------------------------------------------------------*/
  if (dsrlpi_get_min_frame_and_len(rev_fr_ptr,
                                   DSRLP_RATE_1_16_MASK,
                                   &frame_len) == FALSE)
  {
    DATA_RLP_MSG2(MSG_LEGACY_HIGH, " Only %d bytes to tx fill-not enough RLP:%d", 
             frame_len, rrcb_ptr->cfg_ptr->sr_id);
    return(FALSE);
  }
  
  /*------------------------------------------------------------------------
    Build the frame.
  ------------------------------------------------------------------------*/
  ASSERT(frame_len <= DSM_DS_SMALL_ITEM_SIZ);

  item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  if (item_ptr == NULL)
  {
    DATA_RLP_MSG0(MSG_LEGACY_ERROR, "Running out of DSM small items");
    return (FALSE);
  }
  
  memset(item_ptr->data_ptr,0,item_ptr->size);

  /*----------------------------------------------------------------------
    Copy in the least sig. bits of the L_V(N)
  -----------------------------------------------------------------------*/
  item_ptr->data_ptr[0] = 
                    DSRLPSEQ_GET_SEQ(rrcb_ptr->state.needed_seq_num);

  /*----------------------------------------------------------------------
    The next byte is a bitwise OR of the CTL field (top 4 bits) and the
    4 most sig. bits of L_V(N) (bottom 4 bits).
  -----------------------------------------------------------------------*/

  item_ptr->data_ptr[1]  = 
                  DSRLPSEQ_GET_SEQ_HI(rrcb_ptr->state.needed_seq_num);

  item_ptr->data_ptr[1] |= RLP3_FILL_CTL_FLD_SHIFTED;

  /*----------------------------------------------------------------------
    Now that all of the data is copied into the frame, and the rest of
    bits are zero'd out, need to determine if frame must be inside a
    Format A frame.  This is true when Rate 1 frame and idle being sent
    as a data frame.  If so, OR in the appropriate bitmask to the last
    byte in the frame, in order to set the Format A type bits.
  -----------------------------------------------------------------------*/
  if( rev_fr_ptr->fr_rate == DSRLP_RATE_1)
  {
    item_ptr->data_ptr[ frame_len++] |=
                            dsrlpi_type_mask_table[rev_fr_ptr->mux_pdu_type]
                                                  [rev_fr_ptr->srvc_type]
                                                  .format_a;
  }

  /*----------------------------------------------------------------------
    Update field in reverse frame ptr and return TRUE.
  -----------------------------------------------------------------------*/
  item_ptr->used = frame_len;
  rev_fr_ptr->fr_item_ptr = item_ptr;

  return(TRUE);
} /* rlp3_tx_fill_frame() */

#endif /* FEATURE_DS_RLP3 */

