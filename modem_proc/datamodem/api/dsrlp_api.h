#ifndef DSRLP_API_H
#define DSRLP_API_H
/*===========================================================================

  D A T A   S E R V I C E S   R L P   E X T E R N A L   H E A D E R  F I L E

DESCRIPTION
   This file contains external variable and function declarations for the
   Radio Link Protocol.

   This should be included by files which interface with RLP, like the
   MuxLayer and Call Processing.

   Note, order of fields in structs is optimized to save memory in 32bit
   ARM environment. This should be kept in mind when changing the ordering
   of fields.

Copyright (c) 2009 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: O:/src/asw/COMMON/vcs/dsrlp.h_v   1.29   31 Jan 2003 14:39:18   sarithay  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/dsrlp_api.h#1 $DateTime: 2009/06/01 19:43:14 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
28/10/11   ms      Fix compiler errors when MDR feature is turned off. 
03/01/11   ls      Define FEATURE_RLP_API_VERSION_1
02/04/11   op      Data SU API cleanup
01/25/11   ss      SU API defeaturization and removal of customer.h.
06/17/10   ms      Fix RLP Partial logging Crash issue on Q6.
09/04/09   ss      New file created for SU level CMI changes.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "dsm.h"
#include "dsrlp.h"
#include "dsrlp_stats.h"

/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/
#define FEATURE_RLP_API_VERSION_1

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Type for frame rates. These rates are for both Rate Set 1 and Rate Set 2.
  NOTE: These frame rates are used as indexes into arrays, DO NOT ALTER.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_EIGHTH_RATE = 0,                /* Idle frame, Primary channel only  */
  RLP_3_16TH_RATE,                    /* 3/16th rate, Rate Set 2 only      */
  RLP_QUARTER_RATE,                   /* 1/4 rate frame, not used for RLP  */
  RLP_3_8TH_RATE,                     /* 3/8th rate, Rate Set 2 only       */
  RLP_7_16TH_RATE,                    /* 7/16th rate, Rate Set 2 only      */
  RLP_HALF_RATE,                      /* 1/2 rate frame                    */
  RLP_3_4TH_RATE,                     /* Secondary/signaling channels only */
  RLP_7_8TH_RATE,                     /* Secondary/signaling channels only */
  RLP_FULL_RATE,                      /* Full rate frame                   */
  RLP_ERASURE,                        /* Erasure frame                     */  
  RLP_BLANK,                          /* No RLP data                       */
  RLP_FRAME_RATE_MAX
} rlp_rate_enum_type;

/*--------------------------------------------------------------------------
 Type for RLP Type I - Multiplex Option. NOTE: Used as an index.
---------------------------------------------------------------------------*/
typedef enum
{
  RLP_MUX_OPTION1 = 0,   /* Rate Set 1 channel type */
  RLP_MUX_OPTION2        /* Rate Set 2 channel type */
} rlp_mux_option_type;

/*---------------------------------------------------------------------------
  Enum of the different frame types. Each type has a separate processing 
  module. The DONE_FRAME signifies completion of frame processing.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLPI_DONE    = 0, /* Finished processing this received frame           */
  DSRLPI_NEW     = 1, /* frame with new sequence number                    */
  DSRLPI_REXMIT   = 2,    /* re-xmit frame -                               */
  DSRLPI_NAK      = 3,    /* NAK control frame -                           */
  DSRLPI_INIT    = 4, /* RLP Init - SYNC or SYNC/ACK control frame         */
  DSRLPI_IDLE    = 5, /* Idle frame - No data was sent                     */
  DSRLPI_MUX_ERASURE = 6, /* Mux level erasure                             */
  DSRLPI_RLP_ERASURE = 7, /* RLP level erasure                             */
  DSRLPI_NULL    = 8, /* 1/8 rate null                                     */
  DSRLPI_FILL    = 9, /* Fill frame                                        */
  DSRLPI_RESET    = 10,   /* free frames before reset                      */
  DSRLPI_DCCH_DTX = 11    /* DTX on the DCCH                               */
} dsrlpi_fr_det_enum_type;

/*---------------------------------------------------------------------------
  Enum definition for different MuxPDU types.
  MuxPDU_1 - Rateset1 for FCH/DCCH/SCCH
  MuxPDU_2 - Rateset2 for FCH/DCCH/SCCH
  MuxPDU_3 - For SCH.
  MuxPDU_5 - For SCH.
    
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_MUX_PDU_1 = 0,
  DSRLP_MUX_PDU_2,
  DSRLP_MUX_PDU_3,
  DSRLP_MUX_PDU_5,
  DSRLP_MUX_PDU_MAX
} dsrlp_mux_pdu_enum_type;

/*---------------------------------------------------------------------------
  Enum definition for the service type (primary/secondary/ dont care).
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_PRIMARY_SRVC = 0,
  DSRLP_SECONDARY_SRVC,
  DSRLP_GENERIC_SRVC
} dsrlp_srvc_enum_type;

/*---------------------------------------------------------------------------
  Enum for the different frame rates supported by RLP.
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_RATE_1 =0,
  DSRLP_RATE_7_8,
  DSRLP_RATE_3_4,
  DSRLP_RATE_1_2,
  DSRLP_RATE_7_16,
  DSRLP_RATE_3_8,
  DSRLP_RATE_1_4,
  DSRLP_RATE_3_16,
  DSRLP_RATE_1_8,
  DSRLP_RATE_1_16,
  DSRLP_RATE_ERASURE,                                  /* implies FCH      */
  DSRLP_RATE_BLANK,                                    /* implies FCH      */
  DSRLP_RATE_ERASURE_SCH,                              /* on the SCH       */
  DSRLP_RATE_INACTIVE,                                 /* inactive channel */
  DSRLP_RATE_DCCH_DTX                                  /* DCCH is DTX      */
} dsrlp_rate_enum_type;

/*---------------------------------------------------------------------------
  Enum for different RLP signalling frame priorities on Reverse link. This
  is used by MUX to decide what rate it should give to RLP.
  
  Return priority to MUX in function dsrlp_any_sig_to_send().
  DSRLP_TX_NONE         = DSRLP_DATA_OR_IDLE_OR_FILL, 
                          No RLP signalling message to send
  DSRLP_TX_ACCEPT_DELAY = NOT USED FOR NOW; May be used for IDLE or FILL if 
                          needed. If any time , after profiling we find that
                          Voice is not giving even 1/8th rate for RLP, only
                          then we need to use this.
                          Send sig when channel bandwidth allows
  DSRLP_TX_NO_DELAY     = DSRLP_NAK,
                          High priority sig message, send ASAP   
  DSRLP_TX_CONTINUOUS   = DSRLP_SYNC_OR_ACK_OR_SYNC_ACK 
                          These can be delayed for the first time, but once
                                                  we send them, we need to continously send them. 
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_TX_NONE =0,             /* We have Data or Rexmit or Idle or Fill  */
  DSRLP_TX_ACCEPT_DELAY,        /* Not used for now                        */
  DSRLP_TX_NO_DELAY,            /* NAKs to be sent. Send NOW w/o delay     */
  DSRLP_TX_CONTINUOUS           /* used for SYNC/ACK/SYNC-ACK              */
} dsrlp_tx_sig_enum_type;

/*---------------------------------------------------------------------------
  Structure of RLP frames received on the forward link - each frame is put
  into this structure by the Mux and this is passed onto RLP.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean valid_frame;                  /* TRUE-good frame; FALSE- erasure */
  dsrlp_srvc_enum_type srvc_type;       /* Primary/Sec./NA                 */
  dsrlp_mux_pdu_enum_type mux_pdu_type; /* MuxPDU type                     */
  dsrlp_rate_enum_type    fr_rate;      /* Received frame rate             */
  dsm_item_type *fr_item_ptr;           /* Ptr to frame payload            */
} dsrlp_fwd_frame_type;

/*---------------------------------------------------------------------------
  Type definition for all the frames passed to RLP by the mux layer.
---------------------------------------------------------------------------*/
#define RLP_MAX_FWD_PDU   10 /* Max # of RLP frames passed by Mux to RLP    */

typedef struct
{
  byte sr_id;           /* Service ID - identifies service                 */
  byte num_rlp_frames;  /* # of RLP frames in frame_array; 0 means erasure */
  uint64 fr_time;       /* MUX: system time - used for logging             */
  dsrlp_fwd_frame_type fwd_arr[RLP_MAX_FWD_PDU];
                                              /* array of rx'ed RLP frames */
} dsrlp_fwd_frame_block_type;

/*---------------------------------------------------------------------------
  Masks for different RLP rates. These masks are used in the fr_rate_mask
  field in dsrlp_rev_frame_struct. These masks are ored together by the Mux
  layer to tell RLP, which rates are available.
---------------------------------------------------------------------------*/
#define DSRLP_RATE_1_16_MASK  0x0001
#define DSRLP_RATE_1_8_MASK   0x0002
#define DSRLP_RATE_3_16_MASK  0x0004
#define DSRLP_RATE_1_4_MASK   0x0008
#define DSRLP_RATE_3_8_MASK   0x0010
#define DSRLP_RATE_7_16_MASK  0x0020
#define DSRLP_RATE_1_2_MASK   0x0040
#define DSRLP_RATE_3_4_MASK   0x0080
#define DSRLP_RATE_7_8_MASK   0x0100
#define DSRLP_RATE_1_MASK     0x0200
#define DSRLP_RATE_BLANK_MASK 0x0400

/*---------------------------------------------------------------------------
  Masks for the different frame categories that RLP can use to make a frame.
  The MUX layer uses these masks depending on the physical channel in use.
---------------------------------------------------------------------------*/
#define DSRLP_CAT_DATA_MASK   0x01      /* RLP can make new/rexmit data fr */
#define DSRLP_CAT_CTL_MASK    0x02      /* RLP can make init/NAK ctl frames*/
#define DSRLP_CAT_IDLE_MASK   0x04      /* RLP can make idle/fill frames   */
#define DSRLP_CAT_BLANK_MASK  0x08      /* RLP can make BLANK frames       */

/*---------------------------------------------------------------------------
  Structure of RLP frames to be transmitted on the reverse link. The fields
  with "MUX:" are set by the Mux layer, and the fields with "RLP:" are set
  by RLP.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean data_on_ch;                   /* MUX: Is data present on the ch ?*/ 
  uint16 max_frame_rate;                /* MUX: Max frame rate for RLP     */
  uint16 max_frame_size;                /* MUX: Max. # of bytes in frame   */
  uint16 fr_rate_mask;                  /* MUX: Avail. rates for this frame*/
  dsrlp_srvc_enum_type srvc_type;       /* MUX: Primary/Sec./NA            */
  dsrlp_mux_pdu_enum_type mux_pdu_type; /* MUX: MuxPDU type                */
  byte fr_cat_mask;                     /* MUX: Can send New/Ctl/Idle/Init */
  boolean idle_frame;                   /* RLP: idle fr- can DTX if TRUE   */
  dsrlp_rate_enum_type fr_rate;         /* RLP: Current frame rate         */
  boolean free_dsm_pkt;                 /* RLP: Free the dsm pkt after tx  */
  dsm_item_type *fr_item_ptr;           /* RLP: Ptr to frame payload       */
} dsrlp_rev_frame_type;

/*---------------------------------------------------------------------------
  Type definition for all the frames the MUX layer gets from RLP. This is
  passed once every frame time to RLP.
---------------------------------------------------------------------------*/
#define RLP_MAX_REV_PDU   10 /* Max # of RLP frames passed by Mux to RLP    */

typedef struct
{
  byte sr_id;               /* MUX: Service Identifier                     */
  byte max_frames;          /* MUX: Max. # of RLP frames allowed in fr_arr */
  byte num_frames;          /* RLP: # of RLP frames in frame_array         */
  uint64 fr_time;           /* MUX: system time - used for logging         */
  dsrlp_rev_frame_type rev_arr[ RLP_MAX_REV_PDU];
                            /* RLP: Actual frame payload                   */
} dsrlp_rev_frame_block_type;

/*---------------------------------------------------------------------------
  Enum for the client ID in function call dsrlp_enable_inactivity_timer
---------------------------------------------------------------------------*/
typedef enum
{
  DSRLP_FTAP_ID,                /* Forward TAP client ID    */
  DSRLP_RTAP_ID                /* Reverse TAP client ID    */
} dsrlp_cta_client_id_enum_type;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION      DSRLP_ENABLE_INACTIVITY_TIMER

DESCRIPTION   Enable the inactivity timer in each RLP by setting the flag 
              "enable_inactivity_timer" to TRUE.

DEPENDENCIES  None

RETURN VALUE  TRUE:  The inactivity timer is enabled
              FALSE: The inactivity timer is disabled

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_enable_inactivity_timer( 
                                       dsrlp_cta_client_id_enum_type client, 
                                       boolean enable
                                     );

/*===========================================================================

FUNCTION DSRLP_BUILD_TX_FRAMES

DESCRIPTION
  This function builds RLP frame(s) to be transmitted over the air.
  This is the interface between RLP transmit and the multiplex sublayer.
  The multiplex sublayer can either call this function once, to build all
  the frames or call this several times to build a few frames at a time.

  Input Parameters:

  The parameter to this function is a pointer to a structure which contains
  an array of frames that the mux sublayer wants RLP to process. Each element
  of this array includes all the information that RLP needs to process the
  frame independently.   This per frame data structure is passed to a number
  of functions within the RLP frame building code.The information passed
  with each frame includes fields which are set by the mux layer, and fields
  which are set by RLP, when RLP builds the frame.

  Fields set by Mux layer for entire se of frames asked.

    sr_id         : service id for this frame. For SCH frames, sr_id is in
                    the MuxPDU header. For FCH/DCCH frames, this is set by
                    the Mux layer after looking up the appropriate sr_id for
                    primary or secondary. MUX layer must ask for frames of
                    one sr_id in one invocation of this function.

  The fields set by the mux layer per frame:

    srvc_type     : For FCH/DCCH frames, the mux layer sets the srvc_type
                    to DSRLP_PRIMARY_SRVC or DSRLP_SECONDARY. For SCH/SCCH,
                    Mux layer sets the srvc_type to DSRLP_GENERIC_SRVC.

    mux_pdu_type  : Set to DSRLP_MUX_PDU_1 for rateset1 FCH/DCCH; set to
                    DSRLP_MUX_PDU_2 for rateset2 FCH/DCCH; set to
                    DSRLP_MUX_PDU_3 for SCH operation.

    fr_cat_mask   : For FCH/DCCH, Mux layer sets this mask to
                    ( DSRLP_CAT_DATA_MASK | DSRLP_CAT_CTL_MASK |
                      DSRLP_CAT_IDLE_MASK), because data, ctl/idle frames
                    can be sent on FCH/DCCH.
                    For SCH/SCCH, Mux layer sets this mask to
                    DSRLP_CAT_DATA_MASK, because only data frames can be
                    sent on SCH/SCCH.

    max_frame_size: This specifies the max. number of bytes avail. for RLP to
                    put in the frame. For MuxPDU type1 and 2, when multiple
                    frame rates are available, the Mux layer should set
                    this field to the size of the highest frame rate.
                    The Mux layer sets this field to the number of bytes
                    available for RLP to put itsheader and payload excluding
                    the type bits.
                    Basically, take the number of bits of RLP payload as
                    specified in the standard, subtract 8 from it, divide it
                    by 8 and round up to the next byte.
                    e.g for secondary rate1 frames on FCH rateset1,
                    max_frame_size = (168-8)/8 = 20 bytes.

                    For MuxPDU3, this will be set to MuxPDU size-1. e.g. for
                    SCH double size LTUs in Service Option 33, this will be
                    43 bytes.

    fr_rate_mask  : The mux layer ors in all the different frame rates
                    allowed for this frame in this mask. For FCH, the mask
                    can take on a whole range of values depending on primary
                    or secondary service. For DCCH/SCH/SCCH, the mask is
                    usually DSRLP_RATE_1_MASK.


  The fields set by the RLP:

    idle_frame    : RLP sets this to TRUE, if this frame can be DTXed by
                    the Mux layer (for DCCH/secondary operation). When this
                    is FALSE, the Mux layer has to send out this frame.

    fr_rate       : RLP sets this to the frame rate this frame should be
                    used by the Mux layer to send out this frame.

    fr_item_ptr   : Pointer to dsm packet chain (may have multiple dsm items)
                    RLP should ensure that the used fields are properly set
                    in the item, to the number of bits used by RLP rounded
                    up to the next byte.
                    The mux layer shall transmit the number of bytes in the
                    item chain. If there are any left over bits, the Mux
                    layer shall pad the bits with zeros.
                    RLP shall set the unused part of the item chain to zeros

DEPENDENCIES
  dsrlp_init() should have been called.

  Mux layer must ask for frames for only one sr_id in one invocation of this
  funciton.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_build_tx_frames
(
  dsrlp_rev_frame_block_type *rev_fr_blk     /* Ptr to frames to trasmit   */
);

/*===========================================================================

FUNCTION DSRLP_UPDATE_TA_GAP

DESCRIPTION
  This function RLP NAK(s) age so that the NAK can be sent at the right time
  and not slowed because of the Tuneaway
  This is the interface between RLP transmit and the multiplex sublayer.
  The multiplex sublayer can call this function once, to update all
  the RLP nak at a time.

  Input Parameters:

  Fields set by Mux layer for entire se of frames asked.

    sr_id         : service id for this frame. For SCH frames, sr_id is in
                    the MuxPDU header. For FCH/DCCH frames, this is set by
                    the Mux layer after looking up the appropriate sr_id for
                    primary or secondary. MUX layer must ask for frames of
                    one sr_id in one invocation of this function.

  
    fr_missed    :  Number of frames that have been missed as tuneaway was 
                    in progress.

DEPENDENCIES
  dsrlp_init() should have been called.

  Mux layer must ask for frames for only one sr_id in one invocation of this
  funciton.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsrlp_update_ta_gap
( 
  byte sr_id,            /* MUX: Service Identifier   */
  int16 fr_missed        /* MUX: Frames missed in TA gap  */
);
/*===========================================================================

FUNCTION DSRLP_GET_MAX_MS_NAK_ROUNDS_FWD

DESCRIPTION
  This function the MAX number of nak rounds the mobile will support
  on the forward channel

  Input Parameters: None

DEPENDENCIES
  None

RETURN VALUE
  The max number of nak rounds

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_get_max_ms_nak_rounds_fwd(void);

/*===========================================================================

FUNCTION DSRLP_GET_MAX_MS_NAK_ROUNDS_REV

DESCRIPTION
  This function the MAX number of nak rounds the mobile will support
  on the reverse channel

  Input Parameters: None

DEPENDENCIES
  None

RETURN VALUE
  The max number of nak rounds

SIDE EFFECTS
  None
===========================================================================*/
byte dsrlp_get_max_ms_nak_rounds_rev(void);

/*===========================================================================
FUNCTION      DSRLP_CHECK_SR_ID

DESCRIPTION   Check if the SR_ID for the service running on a particular RLP
              instance (rscb array index) is the same as in the MUX header in
			  the frame

DEPENDENCIES  The passed in RSCB_INDEX is good.

RETURN VALUE  TRUE if the SR_ID matches, FALSE otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_check_sr_id
(
   byte rscb_index,                      /* RLP instance                   */
   dsm_item_type *frame_ptr              /* MUX frame                      */
);

/*===========================================================================
FUNCTION      DSRLP_GET_SR_ID

DESCRIPTION   Return the SR_ID for the service running on a particular RLP
              instance (rscb array index)

DEPENDENCIES  The passed in RSCB_INDEX is good. Also dsrlp_init must have 
              been invoked first for the returned SR_ID to be good.

RETURN VALUE  The SR_ID corresponding to the RLP instance

SIDE EFFECTS  None
===========================================================================*/
byte dsrlp_get_sr_id
(
   byte rscb_index                       /* RLP instance                   */
);

/*===========================================================================
  FUNCTION      DSRLP_ANY_SIG_TO_SEND

  DESCRIPTION   This function will return TRUE if any  important RLP 
                frames ( called as Signalling in name of fn) are to be 
                transmitted. 
                Important frames are:
                    a) SYNC/SYNC-ACK/AC
                    b) NAK's:
                    c) Retransmissions to be done
               
               If need be add IDLE and Fill frames also.

  DEPENDENCIES  
              This function should be called only if Data is secondary
              service. If Data is primary service this function should
              not be called.(because, entire channel is for data to 
              send)

  RETURN VALUE
              TRUE:  If RLP has control frames to send
              FALSE: If RLP has NO control frames to send

  SIDE EFFECTS  None
===========================================================================*/
boolean dsrlp_any_sig_to_send(dsrlp_tx_sig_enum_type *);

/*===========================================================================
  FUNCTION      DSRLP_GET_NEXT_DSM_ITEM_CHAIN

  DESCRIPTION   This function takes the pointer to the current beginning of
                a chain of DSM items, which contains RLP paypload to be
                transmitted by the MUX layer, as input, and returns pointer
                to the next item in chain. If there is no more item in
                chain, NULL is returned.


  DEPENDENCIES  None

  RETURN VALUE  Pointer to the next item in chain; if there is no more
                item in chain, return NULL.

  SIDE EFFECTS  None
===========================================================================*/
dsm_item_type *dsrlp_get_next_dsm_item_chain( 
                                              dsm_item_type *cur_ptr_to_payload 
                                            );

/*===========================================================================
  FUNCTION      DSRLP_INSERT_PDU5NULL_HDR_TURBO
  
  DESCRIPTION   

  DEPENDENCIES  None

  RETURN VALUE  None

  SIDE EFFECTS  None
===========================================================================*/
void dsrlp_insert_pdu5null_hdr_turbo(dsm_item_type *out_pkt_ptr);

/*===========================================================================
FUNCTION       DSRLP_GET_RLP_LOGGING_LOG_ID

DESCRIPTION    Accessor method for dsrlp_log_id_table

PARAMETERS
  log_id_index

RETURN VALUE   
  log id from dsrlp_log_id_table
  DS_LOG_RLP_INVALID(-1) - incorrect parameter

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
ds_log_rlp_phy_channel_enum_type dsrlp_get_rlp_logging_log_id
(
  byte log_id_index
);

/*===========================================================================
FUNCTION       DSRLP_GET_FRAME_LEN

DESCRIPTION    Accessor method for dsrlp_len_table

PARAMETERS
  fr_rate
  srvc_type
  mux_pdu_type

RETURN VALUE   
  frame length from dsrlp_len_table
  0 - incorrect parameters or rate is not applicable.

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
byte dsrlp_get_frame_len
(
  dsrlp_rate_enum_type fr_rate,
  dsrlp_srvc_enum_type srvc_type,
  dsrlp_mux_pdu_enum_type mux_pdu_type
);

/*===========================================================================
FUNCTION       DSRLP_GET_USED_FRAME_LEN

DESCRIPTION    Accessor method for dsrlp_used_len_table

PARAMETERS
  fr_rate
  srvc_type
  mux_pdu_type

RETURN VALUE   
  frame length from dsrlp_used_len_table
  0 - incorrect parameters or rate is not applicable.

DEPENDENCIES   
  None

SIDE EFFECTS   
  None
===========================================================================*/
byte dsrlp_get_used_frame_len
(
  dsrlp_rate_enum_type fr_rate,
  dsrlp_srvc_enum_type srvc_type,
  dsrlp_mux_pdu_enum_type mux_pdu_type
);

#endif  /* DSRLP_API_H */
