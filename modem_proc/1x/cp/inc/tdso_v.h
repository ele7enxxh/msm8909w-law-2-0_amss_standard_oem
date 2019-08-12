#ifndef TDSO_V_H
#define TDSO_V_H
/*===========================================================================


         T E S T   D A T A   S E R V I C E   O P T I O N

                  H E A D E R    F I L E


DESCRIPTION
  This header file contains functions that are intended to be used internally
  within the simple/Full Test Data Service Options for configuring, managing
  and updating statistics the S-TDSO service option.

  Copyright (c) 1998 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/


/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/tdso.h_v   1.15   29 Aug 2002 19:28:16   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/tdso_v.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/05/13   srk     Replace existing watchdog implementation with the new
                   Dog Heartbeat Mechanism.
11/07/12   cjb     Mainlining FEATURE_MODEM_1X_SUPPORTS_RCINIT feature.
09/15/12   ppr     FR#2370 - 1x memory optimization changes
02/04/12   ppr     Feature Cleanup
11/15/10   jtm     Initial SU API split for CP.
11/02/10   ag      Added support for dynamic watchdog.
06/10/10   jtm     Eliminated dependency on log_dmss.h by relocating CP
                   specific TDSO log types here.
04/13/09   adw     Split file for CMI phase-I.
03/23/05   fc      Merged the following from MSMSHARED:
03/21/05   an       Mainlined IS2000 features and implemented the new
                    Dynamic Feature Design.
07/09/04   ht      Added TDSO support for Rel C DV channels
01/10/03   lh      Added FEATURE_IS2000_REL_A around 32x counters.
08/28/02   fc      Cleaned lint errors.
06/28/02   lh      Reverse MuxPUD 5 support & Forward MuxPDU clean up.
05/24/02   lh      Fixed FTDSO.
05/07/02   ph      Fixed compile errors when REL A is turned off.
04/08/02   va      Fixed the array size of tdso_data_len
02/12/01   yll     Merged from common archive CP VU 116.
                   12/13/01   fc
                   Changed tdso_tst_par_type and tdso_buffer_info.
12/02/01   kk      Secondary seupport for S-TDSO
08/10/01   kk      32X support.
08/09/01   kk      Merged from common archive.
                   08/06/01   kk      DCCH Full TDSO support.
06/07/01   kk      Mixed Rate Support
05/22/01   kk      RS2 Changes.
05/14/01   fc      Added definition of new log packet for DCCH S-TDSO.
05/14/01   fc      Changed MAX_PDU_LENGTH to 68.
05/11/01   fc      Added support for DCCH STDSO.
                   Cleaned lint errors.
03/21/01   lcc     Added function prototype for tdso_end_call and replaced
                   tdso_send_handoff_complete with tdso_notify_hho_success.
03/12/01   fc      Removed "#include msg.h" and "#include err.h".
01/20/01   ks      Fixed PN initialization, ON/OFF period and counter updates.
11/13/00   ks      Added PN and RAND info to the cicular_buffer_info structure.
11/05/00   ks      Added Support for FULL TDSO Service Option.
09/20/00   fc      Added support for TDSO control directives and counter
                   retrieval directives.
08/11/00   va      Changed names of  process_rev_fch_frame and process_rev_sch_frame
                   to process_rx_fch_frame and process_rx_sch_frame respectively.
07/06/00   ks      Created file.

===========================================================================*/

/* <EJECT> */
/*==========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "1x_variation.h"
#include "comdef.h"
#include "modem_1x_defs.h"

#include "snm.h"
#include "cai.h"
#include "mccsch.h"
#include "mccsch_i.h"
#include "log.h"
#include "rex.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/* TCB for TDSO task */
extern rex_tcb_type* tdso_tcb_ptr;

/* RCInit version */
#define TDSO_TCB_PTR ( tdso_tcb_ptr )


/* -----------------------------------------------------------------------
**                     Signal Masks
** ----------------------------------------------------------------------- */
#define TDSO_CMD_SIG           0x01000
  /* This signal is set when TDSO is needed to do pre-sync */

#define TDSO_DOG_RPT_SIG       0x0001
  /* This signal is for DOG Heartbeat Mechanism for TDSO task */

/* -----------------------------------------------------------------------
** Maximum bits/frame used for construction the NUM_RAND values for the
** FORWARD/REVERSE Fundicated Channel. refer to Table 26 in TDSO standard.
** ---------------------------------------------------------------------- */
#define FCH_RC1_FRAME_BITS 172
#define FCH_RC2_FRAME_BITS 267

/* ----------------------------------------------------------------------
** Maximum bits/frame used for construction the NUM_RAND values for the
** REVERSE Supplemental Channel. refer to Table 27 in TDSO standard.
** --------------------------------------------------------------------- */
#define RSCH_RC1_FRAME_BITS 172
#define RSCH_RC2_FRAME_BITS 267
#define RSCH_RC3_FRAME_BITS 6120
#define RSCH_RC4_FRAME_BITS 4584
#define RSCH_RC5_FRAME_BITS 12264
#define RSCH_RC6_FRAME_BITS 20712

/* ----------------------------------------------------------------------
** Maximum bits/frame used for construction the NUM_RAND values for the
** FORWARD Supplemental Channel. refer to Table 27 in TDSO standard.
** --------------------------------------------------------------------- */
#define FSCH_RC1_FRAME_BITS 172
#define FSCH_RC2_FRAME_BITS 267
#define FSCH_RC3_FRAME_BITS 3048
#define FSCH_RC4_FRAME_BITS 6120
#define FSCH_RC5_FRAME_BITS 4584
#define FSCH_RC6_FRAME_BITS 6120
#define FSCH_RC7_FRAME_BITS 12264
#define FSCH_RC8_FRAME_BITS 9192
#define FSCH_RC9_FRAME_BITS 20712

/*-------------------------------------------------------------------------
                     Circular buffer generation declaration
---------------------------------------------------------------------------*/
#define DEF_TX_SEED    0x55555555
  /* Random number generator seed for TX data */

#define DEF_RX_SEED    0x2aaaaaaa
  /* Random number generator seed for RX data */

#define NUM_PN_BITS 24
  /* Number of PN bits */

#define  PRE_SYNC_COUNT 20
  /* Number of frames ahead the next SYNC frame at which circular buffer
     is generated */

/* ---------------------------------------------------------------------
** Control / Counter directive field definitions
** --------------------------------------------------------------------- */
#define TDSO_CTL_TYPE     0  /* Byte index for control record type field */
#define TDSO_CTL_CODE     1  /* Byte index for control code field        */
#define TDSO_CHN_DIR      2  /* Byte index for channel direction field   */
#define TDSO_VECT_CNT_ID  1  /* Byte index for vector counter id         */

#ifdef FEATURE_IS2000_REL_A
#define MAX_PDU_LEN      95  /* Maximum PDU length in bytes for 32X  */
#define TDSO_MAX_TURBO_LEN  560 /* in case of turbo */
#else
#define MAX_PDU_LEN      71  /* Maximum PDU length */
#endif /* FEATURE_IS2000_REL_A */

#ifdef FEATURE_IS2000_REL_A_SVD
/* following literals are to be used only when primary traffic has voice
and TDSO is used partially employed on secondary.
Table 2.11 and Table 2.12 explains data sizes (in MAC spec)
note that header is included in those cases. */
#define TDSO_RS1_SECONDARY_00   88
#define TDSO_RS1_SECONDARY_01   128
#define TDSO_RS1_SECONDARY_02   152
#define TDSO_RS1_SECONDARY_03   168

#define TDSO_RS2_SECONDARY_00   138
#define TDSO_RS2_SECONDARY_01   208
#define TDSO_RS2_SECONDARY_02   242
#define TDSO_RS2_SECONDARY_03   262

#define TDSO_NON_FULL_FRAME_ON_SEC  1
#endif /*  FEATURE_IS2000_REL_A_SVD */


/* ---------------------------------------------------------------------
** CTL_REC_TYPE codes
** --------------------------------------------------------------------- */
typedef enum
{
  TDSO_CTL_ALL = 0,          /* Control directive for all Physical Channels */
  TDSO_CTL_FCH,              /* Control directive for FCH */
  TDSO_CTL_DCCH,             /* Control directive for DCCH */
  TDSO_CTL_SCH0,             /* Control directive for SCH0 */
  TDSO_CTL_SCH1,             /* Control directive for SCH1 */
  TDSO_CNT_FCH,              /* Counter retrieval directive for FCH */
  TDSO_CNT_DCCH,             /* Counter retrieval directive for DCCH */
  TDSO_CNT_SCH0,             /* Counter retrieval directive for SCH0 */
  TDSO_CNT_SCH1              /* Counter retrieval directive for SCH1 */
} tdso_ctl_rec_type;

/* ---------------------------------------------------------------------
** FCH / DCCH Vector counter ID
** --------------------------------------------------------------------- */
typedef enum
{
  TDSO_FDCH_FER_CNT = 0,     /* FER counters */
  TDSO_FDCH_RXEX_CNT,        /* Receive expected counter response */
  TDSO_FDCH_TX_CNT,          /* Transmitted counters */
  TDSO_FDCH_5M_TX_CNT,       /* 5 ms frame transmitted counters */
  TDSO_FDCH_5M_RX_CNT        /* 5 ms frame received counters */
} tdso_fdch_vect_cnt_id_type;

/* --------------------------------------------------------------------
** SCH Vector counter ID
** -------------------------------------------------------------------- */
typedef enum
{
  TDSO_SCH_FER_CNT = 0,      /* FER counters response */
  TDSO_SCH_PER_CNT,          /* PER counters response */
  TDSO_SCH_TX_CNT            /* Transmitted counters response */
} tdso_sch_vect_cnt_id_type;

/* --------------------------------------------------------------------
** Channel direction
** -------------------------------------------------------------------- */
typedef enum
{
  FWD_AND_REV_LNK = 0,      /* Both forward and reverse link direction */
  FWD_LNK_ONLY    = 1,      /* Forward link direction only */
  REV_LNK_ONLY    = 2       /* Reverse link direction only */
} tdso_ch_dir_type;

/* --------------------------------------------------------------------
** Frame Source
** -------------------------------------------------------------------- */
typedef enum
{
  TWO_FULL_RATE_FRAME = 0,  /* 2 full-rate frames */
  N_FULL_RATE_FRAME   = 1,  /* N full-rate frames */
  NEW_FRAME           = 2   /* New frame every frame period */
} tdso_fr_src_type;

/* --------------------------------------------------------------------
** Enumeration for FCH / DCCH TDSO frame types
** -------------------------------------------------------------------- */
typedef enum
{
#ifdef FEATURE_IS2000_REL_A
  TDSO_RATE_3,             /* Rate 3 frame type */
#endif
  TDSO_RATE_2,             /* Rate 2 frame type */
  TDSO_RATE_1,             /* Rate 1 Frame type */
  TDSO_RATE_HALF,          /* Half Rate Frame type */
  TDSO_RATE_QUARTER,       /* Quarter Rate Frame type */
  TDSO_RATE_EIGHTH,        /* One Eight Rate Framte type */
  TDSO_BLANK,              /* Blank frame */
  TDSO_ERASURE,            /* Erasures */
  TDSO_DIM_BURST,          /* Dim-and-burst frame type */
  TDSO_BIT_ERROR,          /* Bit errors identified */
  TDSO_INSUFFICIENT_FRAME, /* Insufficient physical frame */
  TDSO_BLANK_BURST,        /* Blank-and-burst frame type */
  TDSO_NULL,               /* Null */
  TDSO_OTHER,              /* Other */
  TDSO_MAX_RATE            /* index for tables */
} tdso_rate_type;

/* -------------------------------------------------------------------
** Enumeration for SCH receive frame counters
** ------------------------------------------------------------------- */
typedef enum
{
  TDSO_SCH_RATE_1X,
  TDSO_SCH_RATE_2X,
  TDSO_SCH_RATE_4X,
  TDSO_SCH_RATE_8X,
  TDSO_SCH_RATE_16X,
#ifdef FEATURE_IS2000_REL_A
  TDSO_SCH_RATE_32X,
#endif /* FEATURE_IS2000_REL_A */
  TDSO_SCH_RATE_BLANK,
  TDSO_SCH_RATE_ERASURE,
  TDSO_SCH_BIT_ERROR,
  TDSO_SCH_RATE_OTHER,
  TDSO_SCH_RATE_NONE
} tdso_sch_rate_type;

/* -------------------------------------------------------------------
** Channel Type, indices for FCH / DCCH / SCH counters
** ------------------------------------------------------------------- */
typedef enum
{
  TDSO_FCH,                 /* Fundamental Channel */
  TDSO_DCCH,                /* Dedicated Common Control Channel */
  TDSO_SCH0,                /* Supplemental Channel 0 */
  TDSO_SCH1,                /* Supplemental Channel 1 */
  TDSO_UNKNOWN              /* Default, not assigned yet */
} tdso_channel_type;

/* -------------------------------------------------------------------
** Multiplex Option Type, Only Rate Set 1 is supported in S-TDSO
** ------------------------------------------------------------------- */
typedef enum
{
  TDSO_RATE_SET_1,          /* RS1 or Odd-number Multiplex Option */
  TDSO_RATE_SET_2,          /* RS2 or Even-numbered Multiplex Option */
  TDSO_MAX_RATE_SET         /* Maximum rate set, used for indexing */
} tdso_mux_type;

/* -------------------------------------------------------------------
** Link direction
** ------------------------------------------------------------------- */
typedef enum
{
  FWD_LINK,
  REV_LINK
} tdso_link_dir_type;

/* -------------------------------------------------------------------
** Service type
** ------------------------------------------------------------------- */
typedef enum
{
  TDSO_PRIMARY_SRVC = 0,
  TDSO_SECONDARY_SRVC
} tdso_srvc_type;

/* -------------------------------------------------------------------
** MuxPDU types
** ------------------------------------------------------------------- */
typedef enum
{
  TDSO_MUX_PDU_1 = 0, /* Rateset1 for FCH/DCCH/SCCH */
  TDSO_MUX_PDU_2,     /* Rateset2 for FCH/DCCH/SCCH */
  TDSO_MUX_PDU_3,     /* For SCH */
#ifdef FEATURE_IS2000_REL_A
  TDSO_MUX_PDU_5,
#endif /* FEATURE_IS2000_REL_A */
  TDSO_MUX_PDU_MAX
} tdso_mux_pdu_type;

/* -------------------------------------------------------------------
** TDSO Coding Type
** ------------------------------------------------------------------- */
typedef enum
{
  TDSO_CODING_CONV = 0,     /* Convolutional coding */
  TDSO_CODING_TURBO,        /* Turbo coding */
  TDSO_CODING_MAX
} tdso_coding_type;

/* -------------------------------------------------------------------
** TDSO Function return type
** ------------------------------------------------------------------- */
typedef enum
{
  TDSO_VALID = 1,           /* Return value for Valid request */
  TDSO_INVALID_CHAN,        /* Invalid Channel type */
  TDSO_INVALID_SRVC,        /* Invalid Service type */
  TDSO_OUTOF_SYNC,          /* IF TDSO is not synchronized */
  TDSO_NOT_SUPPORTED        /* returns Unsopported frame type */
} tdso_status_type;

/* ---------------------------------------------------------
** Table of bit data length for TDSO frames
** -------------------------------------------------------- */
extern const word tdso_data_len[TDSO_MAX_RATE][TDSO_MAX_RATE_SET];

/* ---------------------------------------
** Structure to store TDSO test parameters
** --------------------------------------- */
typedef struct
{
  uint8 ctl_type;           /* Type of control directive    */
  uint8 ch_dir;             /* Channel direction            */
  boolean cpy_cnts;         /* Copy counters                */
  boolean clr_cnts;         /* Clear counters               */
  boolean data_src;         /* Data source                  */
  uint8 frame_src;          /* Frame source                 */
  uint8 frame_act;          /* Frame activity               */
  uint8 tst_opt;            /* Test options                 */
  uint8 num_circ_buf_frames; /* Num of full-rate frames in circular buffer */
  uint8 on_to_off_prob;     /* ON to OFF state transition probability */
  uint8 off_to_on_prob;     /* OFF to ON state transition probability */
  uint8 tx_on_period;       /* Transmission on period        */
  uint8 tx_off_period;      /* Transmission off period       */
  uint8 data_pattern;       /* Data pattern                  */
} tdso_tst_par_type;

/* ------------------------------------------------------------------------
** Structure to store TDSO operational parameters for Individual Channel.
** Both Forward and Reverse Physical channels have seperate set of parms.
** --------------------------------------------------------------------- */
typedef struct
{
  uint8* pre_sync_circ_buffer; /* Circular buffer used for pre-synchronization */
  uint8* circ_buffer;       /* Circular buffer used for synchronization */
  uint32 last_seed;         /* last Seed value during RN generation     */
  uint32 pre_sync_last_seed;/* Last Seed used for pre-synchronization   */
  uint32 yn1;               /* Offset is computed by Yn1 mod NUM_RAND   */
  uint32 pn;                /* pn used for setting ON/OFF random state  */
  boolean pn_valid;         /* This field is used to set the PN value   */
  uint16  max_frame_size;   /* Max. # of bits in frame                  */
  boolean tdso_sync;        /* This field identifies Synchronization    */
  uint16 pn_offset;         /* PN offset generated for every frame      */
  uint16 data_offset;       /* Reference pointer for Circular buffer    */
  uint16 rc;                /* Radio Configuration for the channel      */
  uint16 num_rand;          /* Number of random number to generate      */
  boolean tdso_state;       /* Indicates if TDSO state is ON or OFF     */
  uint8 tx_on_period;       /* Transmission on period                   */
  uint8 tx_off_period;      /* Transmission off period                  */
  uint16 tx_on_period_cnt;  /* Counter for reducing TDSO ON period      */
  uint16 tx_off_period_cnt; /* Counter for reducing TDSO OFF period     */
  boolean frame_act;        /* Frame Activity                           */
  boolean data_src;         /* Data source                              */
  uint8 data_pattern;       /* Data pattern                             */
  uint32 tx_on_thresh;      /* ON Threshold for dynamaic transmission   */
  uint32 tx_off_thresh;     /* OFF Threshold for dynamaic transmission  */
  tdso_tst_par_type tst_par;/* Test Parms specified in Serv cntl msg    */
  boolean socm_deferred;    /* Indicates if SOCM update is deferred     */
  tdso_tst_par_type pending_tst_par;
                            /* Test Parms specifed in pendinf Serv cntl msg */
} tdso_buffer_info;

/* ----------------------------------------------
** Structure to store TDSO channel parameters.
** --------------------------------------------- */
typedef struct
{
  tdso_buffer_info pn_info[2]; /* For both FWD and REV directions */
} tdso_chan_parms;

/*-------------------------------------------------------------------------
  Structure of TDSO frames received on the forward link - each frame is put
  into this structure by the Mux and this is passed onto TDSO.
---------------------------------------------------------------------------*/
typedef struct
{
  boolean             valid_frame;    /* TRUE-good frame; FALSE- erasure */
  tdso_srvc_type      srvc_type;      /* Primary/Sec./NA                 */
  tdso_mux_pdu_type   mux_pdu_type;   /* MUX PDU type                    */
  tdso_channel_type   fr_channel;     /* Channel on which frame received */
  tdso_rate_type      fr_rate;        /* Received frame rate             */
  tdso_sch_rate_type  sch_rate;       /* SCH frame rate, 1x, 2x etc      */
  tdso_coding_type    sch_coding;     /* Turbo or convolutional          */
  uint8               fr_item_ptr[MAX_PDU_LEN]; /* Ptr to frame payload   */
                                      /* Allocate MAX data length */
} tdso_rx_frame_type;

/* ----------------------------------------------------------------------
** Type definition for TDSO frames to be transmitted on the reverse link.
** ---------------------------------------------------------------------- */
typedef struct
{
  boolean             valid_frame;     /* TRUE-good frame; FALSE- erasure */
  uint16              max_frame_size;  /*  Max. # of bits in frame         */
  tdso_srvc_type      srvc_type;       /*  Primary/Sec./NA                 */
  tdso_mux_pdu_type   mux_pdu_type;    /*  MuxPDU type                     */
  tdso_channel_type   fr_channel;      /*  Channel on which frame received */
  tdso_rate_type      fr_rate;         /*  Current frame rate              */
  tdso_sch_rate_type  sch_rate;        /*  SCH frame rate, 1x, 2x etc      */
  uint8               fr_item_ptr[MAX_PDU_LEN]; /* Ptr to frame payload             */
} tdso_tx_frame_type;

/* -----------------------------------------------------------------------
** Type definition for all TDSO frames exchanged between TDSO & MUX layer.
** ----------------------------------------------------------------------- */
#define TDSO_MAX_PDU   9  /* Max # of TDSO frames passed by Mux to TDSO */

typedef struct
{
  uint8         num_frames; /* # of PDU, 0 means erasure */
  dword         fr_time;    /* MUX: system time - used for logging              */
  word          sec_len;    /* Length on Secondary         */
  tdso_mux_type fdch_fr_mux;  /* Rate Set 1 or 2 on FCH or on DCCH  */
  tdso_mux_type sup_fr_mux;     /* Rate Set 1 or 2 on SCH */
  struct
  {
    tdso_rx_frame_type fwd_arr[ TDSO_MAX_PDU];
    tdso_tx_frame_type rev_arr[ TDSO_MAX_PDU];
  } frame_type;
                            /* array of rx'ed TDSO frames  */
} tdso_frame_block_type;

/* -----------------------------
** TDSO Pre-Synchronization info
** ----------------------------- */
typedef struct
{
  dword               prev_sync_time;     /* Previous Synchronization time */
  tdso_link_dir_type  direction;          /* Pre-Synchronization direction */
  dword               time;               /* Pre-Synchronization time */

  /* --------------------------------------------------------------------------
  ** Forward and reverse circular buffers for SCH0 channels pre-synchronization
  ** For Maximum N = 50, where N = size of number of** Full Rate Frames. These
  ** circular buffers are currently limited to N = 2.
  ** ------------------------------------------------------------------------ */
  byte                sch_tx_circ_buffer[3120];
  byte                sch_rx_circ_buffer[3120];
} tdso_pre_sync_type;

/* Pointer holds TDSO Pre-Synchronization info */
extern tdso_pre_sync_type *tdso_pre_sync_ptr;

/*---------------------------------------------------------------------------
  Simple Test Data Service Option (TDSO) Log Type (P2 mode)
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_STDSO_P2_C)
  uint16 for_mux_opt; /* Forward MUX option */
  uint16 rev_mux_opt; /* REverse MUX option */

  /* Transmit FCH frame counters */
  uint32 e1_t1;      /* Rate 1 with no signaling                */
  uint32 e2_t2;      /* Rate 1/2 with no signaling              */
  uint32 e4_t4;      /* Rate 1/4 with no signaling              */
  uint32 e8_t8;      /* Rate 1/8 with no signaling              */
  uint32 e1_td;      /* Rate 1 with dim-and-burst signaling     */
  uint32 e2_td;      /* Rate 1/2 with dim-and-burst signaling   */
  uint32 e4_td;      /* Rate 1/4 with dim-and-burst signaling   */
  uint32 e8_td;      /* Rate 1/8 with dim-and-burst signaling   */
  uint32 e1_tb;      /* Rate 1 with blank-and-burst signaling   */
  uint32 e2_tb;      /* Rate 1/2 with blank-and-burst signaling */
  uint32 e4_tb;      /* Rate 1/4 with blank-and-burst signaling */
  uint32 e8_tb;      /* Rate 1/8 with blank-and-burst signaling */
  uint32 eb_tb;      /* Blank                                   */
  uint32 eb_to;      /* Frame category other than blank         */

  /* Transmit SCH0 frame counters */
  uint32 es1x_t1x;   /* 1 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es2x_t2x;   /* 2 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es4x_t4x;   /* 4 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es8x_t8x;   /* 8 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es16x_t16x; /* 16 x (9.6 or 14.4), depending on SCH MUX option */
  uint32 es1x_tb;    /* 1 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es2x_tb;    /* 2 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es4x_tb;    /* 4 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es8x_tb;    /* 8 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es16x_tb;   /* 16 x (9.6 or 14.4) expected, blank tx'd instead */
  uint32 esb_tb;     /* Blank                                           */

  /* Received FCH frame counters */
  uint32 e1_r1;      /* Error-free Rate 1 frame with no dim-and-burst         */
  uint32 e1_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e1_rd;      /* Dim-and-burst frame                                   */
  uint32 e1_rb;      /* Blank-and-burst                                       */
  uint32 e1_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e1_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e1_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e1_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e1_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e2_r2;      /* Error-free Rate 1/2 frame with no dim-and-burst       */
  uint32 e2_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e2_rd;      /* Dim-and-burst frame                                   */
  uint32 e2_rb;      /* Blank-and-burst                                       */
  uint32 e2_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e2_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e2_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e2_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e2_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e4_r4;      /* Error-free Rate 1/4 frame with no dim-and-burst       */
  uint32 e4_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e4_rd;      /* Dim-and-burst frame                                   */
  uint32 e4_rb;      /* Blank-and-burst                                       */
  uint32 e4_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e4_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e4_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e4_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e4_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e8_r8;      /* Error-free Rate 1/8 frame with no dim-and-burst       */
  uint32 e8_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e8_rd;      /* Dim-and-burst frame                                   */
  uint32 e8_rb;      /* Blank-and-burst                                       */
  uint32 e8_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e8_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e8_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e8_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e8_re;      /* Insufficient frame quality (erasure)                  */
  uint32 en_rn;      /* Null  */
  uint32 en_rb;      /* Blank */
  uint32 en_ro;      /* Other */

  /* Received SCH0 frame counters */
  uint32 es1x_r1x;   /* Error-free 1 x (9.6 or 14.4) frame          */
  uint32 es2x_r2x;   /* Error-free 2 x (9.6 or 14.4) frame          */
  uint32 es4x_r4x;   /* Error-free 4 x (9.6 or 14.4) frame          */
  uint32 es8x_r8x;   /* Error-free 8 x (9.6 or 14.4) frame          */
  uint32 es16x_r16x; /* Error-free 16 x (9.6 or 14.4) frame         */
  uint32 es1x_rerr;  /* 1 x (9.6 or 14.4) frame with bit errors     */
  uint32 es2x_rerr;  /* 2 x (9.6 or 14.4) frame with bit errors     */
  uint32 es4x_rerr;  /* 4 x (9.6 or 14.4) frame with bit errors     */
  uint32 es8x_rerr;  /* 8 x (9.6 or 14.4) frame with bit errors     */
  uint32 es16x_rerr; /* 16 x (9.6 or 14.4) frame with bit errors    */
  uint32 es1x_re;    /* Expected 1 x (9.6 or 14.4), erasure         */
  uint32 es2x_re;    /* Expected 2 x (9.6 or 14.4), erasure         */
  uint32 es4x_re;    /* Expected 4 x (9.6 or 14.4), erasure         */
  uint32 es8x_re;    /* Expected 8 x (9.6 or 14.4), erasure         */
  uint32 es16x_re;   /* Expected 16 x (9.6 or 14.4), erasure        */
  uint32 es1x_rb;    /* 1 x (9.6 or 14.4) expected, blank received  */
  uint32 es2x_rb;    /* 2 x (9.6 or 14.4) expected, blank received  */
  uint32 es4x_rb;    /* 4 x (9.6 or 14.4) expected, blank received  */
  uint32 es8x_rb;    /* 8 x (9.6 or 14.4) expected, blank received  */
  uint32 es16x_rb;   /* 16 x (9.6 or 14.4) expected, blank received */
  uint32 esb_rb;     /* Blank                                       */
  uint32 esb_ro;     /* Frame category other than blank             */

  /* Transmit DCCH frame counters */
  uint32 dcch_e1_t1; /* Rate 1 with no signaling                */
  uint32 dcch_e2_t2; /* Rate 1/2 with no signaling              */
  uint32 dcch_e4_t4; /* Rate 1/4 with no signaling              */
  uint32 dcch_e8_t8; /* Rate 1/8 with no signaling              */
  uint32 dcch_e1_td; /* Rate 1 with dim-and-burst signaling     */
  uint32 dcch_e2_td; /* Rate 1/2 with dim-and-burst signaling   */
  uint32 dcch_e4_td; /* Rate 1/4 with dim-and-burst signaling   */
  uint32 dcch_e8_td; /* Rate 1/8 with dim-and-burst signaling   */
  uint32 dcch_e1_tb; /* Rate 1 with blank-and-burst signaling   */
  uint32 dcch_e2_tb; /* Rate 1/2 with blank-and-burst signaling */
  uint32 dcch_e4_tb; /* Rate 1/4 with blank-and-burst signaling */
  uint32 dcch_e8_tb; /* Rate 1/8 with blank-and-burst signaling */
  uint32 dcch_eb_tb; /* Blank                                   */
  uint32 dcch_eb_to; /* Frame category other than blank         */

  /* Received DCCH frame counters */
  uint32 dcch_e1_r1;   /* Error-free Rate 1 frame with no dim-and-burst         */
  uint32 dcch_e1_rerr; /* Rate 1 with bit errors detected by the service option */
  uint32 dcch_e1_rd;   /* Dim-and-burst frame                                   */
  uint32 dcch_e1_rb;   /* Blank-and-burst                                       */
  uint32 dcch_e1_r2;   /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 dcch_e1_r4;   /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 dcch_e1_r8;   /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 dcch_e1_rfl;  /* Rate 1 physical layer frame with insufficient
                          physical layer frame quality                          */
  uint32 dcch_e1_re;   /* Insufficient frame quality (erasure)                  */
  uint32 dcch_e2_r2;   /* Error-free Rate 1/2 frame with no dim-and-burst       */
  uint32 dcch_e2_rerr; /* Rate 1 with bit errors detected by the service option */
  uint32 dcch_e2_rd;   /* Dim-and-burst frame                                   */
  uint32 dcch_e2_rb;   /* Blank-and-burst                                       */
  uint32 dcch_e2_r1;   /* Rate 1 frame with no dim-and-burst                    */
  uint32 dcch_e2_r4;   /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 dcch_e2_r8;   /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 dcch_e2_rfl;  /* Rate 1 physical layer frame with insufficient
                          physical layer frame quality                          */
  uint32 dcch_e2_re;   /* Insufficient frame quality (erasure)                  */
  uint32 dcch_e4_r4;   /* Error-free Rate 1/4 frame with no dim-and-burst       */
  uint32 dcch_e4_rerr; /* Rate 1 with bit errors detected by the service option */
  uint32 dcch_e4_rd;   /* Dim-and-burst frame                                   */
  uint32 dcch_e4_rb;   /* Blank-and-burst                                       */
  uint32 dcch_e4_r1;   /* Rate 1 frame with no dim-and-burst                    */
  uint32 dcch_e4_r2;   /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 dcch_e4_r8;   /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 dcch_e4_rfl;  /* Rate 1 physical layer frame with insufficient
                          physical layer frame quality                          */
  uint32 dcch_e4_re;   /* Insufficient frame quality (erasure)                  */
  uint32 dcch_e8_r8;   /* Error-free Rate 1/8 frame with no dim-and-burst       */
  uint32 dcch_e8_rerr; /* Rate 1 with bit errors detected by the service option */
  uint32 dcch_e8_rd;   /* Dim-and-burst frame                                   */
  uint32 dcch_e8_rb;   /* Blank-and-burst                                       */
  uint32 dcch_e8_r1;   /* Rate 1 frame with no dim-and-burst                    */
  uint32 dcch_e8_r2;   /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 dcch_e8_r4;   /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 dcch_e8_rfl;  /* Rate 1 physical layer frame with insufficient
                          physical layer frame quality                          */
  uint32 dcch_e8_re;   /* Insufficient frame quality (erasure)                  */
  uint32 dcch_en_rn;   /* Null  */
  uint32 dcch_en_rb;   /* Blank */
  uint32 dcch_en_ro;   /* Other */

  /* Received SCH0 frame counters for 32X */
  uint32 es32x_r32x; /* Error-free 32 x (9.6 or 14.4) frame         */
  uint32 es32x_rerr; /* 32 x (9.6 or 14.4) frame with bit errors    */
  uint32 es32x_re;   /* Expected 32 x (9.6 or 14.4), erasure        */
  uint32 es32x_rb;   /* 32 x (9.6 or 14.4) expected, blank received */

  /* Transmit SCH0 frame counters for 32X */
  uint32 es32x_t32x; /* Error-free 32 x (9.6 or 14.4) frame         */
  uint32 es32x_tb;   /* 32 x (9.6 or 14.4) expected, blank received */

LOG_RECORD_END

typedef LOG_STDSO_P2_C_type log_stdso_p2_type;


/*---------------------------------------------------------------------------
  Full Test Data Service Option (F-TDSO) Log Type
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_FTDSO_P2_C)
  /* Start of original FTDSO packet definition */
  uint16 for_mux_opt; /* Forward MUX option */
  uint16 rev_mux_opt; /* REverse MUX option */

  /* Transmit FCH frame counters */
  uint32 tdso_e1_t1;      /* Rate 1 with no signaling                */
  uint32 tdso_e1_td;      /* Rate 1 with dim-and-burst signaling     */
  uint32 tdso_e1_tb;      /* Rate 1 with blank-and-burst signaling   */
  uint32 tdso_eb_tb;      /* Blank                                   */
  uint32 tdso_eb_to;      /* Frame Category other than blank         */

  /* Transmit SCH0 frame counters */
  uint32 tdso_e1x_t1x;   /* 1 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e2x_t2x;   /* 2 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e4x_t4x;   /* 4 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e8x_t8x;   /* 8 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e16x_t16x; /* 16 x (9.6 or 14.4), depending on SCH MUX option */
  uint32 tdso_e1x_tb;    /* 1 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e2x_tb;    /* 2 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e4x_tb;    /* 4 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e8x_tb;    /* 8 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e16x_tb;   /* 16 x (9.6 or 14.4) expected, blank tx'd instead */
  uint32 tdso_esb_tb;     /* Blank   */

  /* Received Frame counters for Fundicated channel FCH/DCCH              */
  uint32 tdso_e1_r1;    /* Error-free Rate 1 frame with no dim-and-burst  */
  uint32 tdso_e1_rerr;  /* Rate 1 with bit errors                         */
  uint32 tdso_e1_rd;    /* Dim-and-burst frame                            */
  uint32 tdso_e1_ro;    /* Other rate Frame                               */
  uint32 tdso_e1_rb;    /* Blank-and-burst                                */
  uint32 tdso_e1_rfl;   /* Rate 1 physical layer frame with insufficient
                           physical layer frame quality                   */
  uint32 tdso_e1_re;    /* Insufficient frame quality (erasure)           */
  uint32 tdso_en_rn;    /* Null  */
  uint32 tdso_en_rb;    /* Blank */
  uint32 tdso_en_ro;    /* Other */

  /* Received SCH0 frame counters */
  uint32 tdso_e1x_r1x;   /* Error-free 1 x (9.6 or 14.4) frame          */
  uint32 tdso_e2x_r2x;   /* Error-free 2 x (9.6 or 14.4) frame          */
  uint32 tdso_e4x_r4x;   /* Error-free 4 x (9.6 or 14.4) frame          */
  uint32 tdso_e8x_r8x;   /* Error-free 8 x (9.6 or 14.4) frame          */
  uint32 tdso_e16x_r16x; /* Error-free 16 x (9.6 or 14.4) frame         */
  uint32 tdso_e1x_rerr;  /* 1 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e2x_rerr;  /* 2 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e4x_rerr;  /* 4 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e8x_rerr;  /* 8 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e16x_rerr; /* 16 x (9.6 or 14.4) frame with bit errors    */
  uint32 tdso_e1x_re;    /* Expected 1 x (9.6 or 14.4), erasure         */
  uint32 tdso_e2x_re;    /* Expected 2 x (9.6 or 14.4), erasure         */
  uint32 tdso_e4x_re;    /* Expected 4 x (9.6 or 14.4), erasure         */
  uint32 tdso_e8x_re;    /* Expected 8 x (9.6 or 14.4), erasure         */
  uint32 tdso_e16x_re;   /* Expected 16 x (9.6 or 14.4), erasure        */
  uint32 tdso_e1x_rb;    /* 1 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e2x_rb;    /* 2 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e4x_rb;    /* 4 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e8x_rb;    /* 8 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e16x_rb;   /* 16 x (9.6 or 14.4) expected, blank received */
  uint32 tdso_esb_rb;     /* Blank                                       */
  uint32 tdso_esb_ro;     /* Frame category other than blank             */


  /* Received PDU counters for the Supplemental Channels                */
  uint32 tdso_e3_r3;    /* Error-free Rate 3 MuxPDU                     */
  uint32 tdso_e3_rerr;  /* Rate 3 MuxPDU with Errors                    */
  uint32 tdso_e3_re;    /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e2_r2;    /* Error-free Rate 2 MuxPDU                     */
  uint32 tdso_e2_rerr;  /* Rate 2 MuxPDU with Errors                    */
  uint32 tdso_e2_re;    /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e1a_r1a;   /* Error-free Rate 1a MuxPDU                    */
  uint32 tdso_e1a_rerr; /* Rate 1a MuxPDU with Errors                   */
  uint32 tdso_e1a_re;   /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e1b_r1b;   /* Error-free Rate 1b MuxPDU                    */
  uint32 tdso_e1b_rerr; /* Rate 1a MuxPDU with Errors                   */
  uint32 tdso_e1b_re;   /* Insufficient Frame Quality, Erasures         */

  /* End of original FTDSO packet definition */

  /* Transmit DCCH frame counters */
  uint32 dcch_e1_t1;      /* Rate 1 with no signaling                */
  uint32 dcch_e1_td;      /* Rate 1 with dim-and-burst signaling     */
  uint32 dcch_e1_tb;      /* Rate 1 with blank-and-burst signaling   */
  uint32 dcch_eb_tb;      /* Blank                                   */
  uint32 dcch_eb_to;      /* Frame Category other than blank         */

  /* Received Frame counters for Fundicated channel DCCH                  */
  uint32 dcch_e1_r1;    /* Error-free Rate 1 frame with no dim-and-burst  */
  uint32 dcch_e1_rerr;  /* Rate 1 with bit errors                         */
  uint32 dcch_e1_rd;    /* Dim-and-burst frame                            */
  uint32 dcch_e1_ro;    /* Other rate Frame                               */
  uint32 dcch_e1_rb;    /* Blank-and-burst                                */
  uint32 dcch_e1_rfl;   /* Rate 1 physical layer frame with insufficient
                           physical layer frame quality                   */
  uint32 dcch_e1_re;    /* Insufficient frame quality (erasure)           */
  uint32 dcch_en_rn;    /* Null  */
  uint32 dcch_en_rb;    /* Blank */
  uint32 dcch_en_ro;    /* Other */

#ifdef FEATURE_IS2000_REL_A
  /* Transmit SCH0 frame counters */
  uint32 tdso_e32x_t32x; /* 32 x (9.6 or 14.4), depending on SCH MUX option */
  uint32 tdso_e32x_tb;   /* 32 x (9.6 or 14.4) expected, blank tx'd instead */

  /* Received SCH0 frame counters */
  uint32 tdso_e32x_r32x; /* Error-free 32 x (9.6 or 14.4) frame         */
  uint32 tdso_e32x_rerr; /* 32 x (9.6 or 14.4) frame with bit errors    */
  uint32 tdso_e32x_re;   /* Expected 32 x (9.6 or 14.4), erasure        */
  uint32 tdso_e32x_rb;   /* 32 x (9.6 or 14.4) expected, blank received */
#endif

LOG_RECORD_END

typedef LOG_FTDSO_P2_C_type log_ftdso_p2_type;

/*---------------------------------------------------------------------------
  Test Data Service Option (TDSO) Log Type

  Refer to the following tables in the TDSO standard for more info:
  Table 13, Table 14, Table 15, Table 16, Table 17
---------------------------------------------------------------------------*/
/* LOG_FTDSO_C */
typedef PACK(struct)
{
  log_hdr_type hdr;
  uint16 for_mux_opt; /* Forward MUX option */
  uint16 rev_mux_opt; /* REverse MUX option */

  /* Transmit FCH frame counters */
  uint32 tdso_e1_t1;      /* Rate 1 with no signaling                */
  uint32 tdso_e1_td;      /* Rate 1 with dim-and-burst signaling     */
  uint32 tdso_e1_tb;      /* Rate 1 with blank-and-burst signaling   */
  uint32 tdso_eb_tb;      /* Blank                                   */
  uint32 tdso_eb_to;      /* Frame Category other than blank         */

  /* Transmit SCH0 frame counters */
  uint32 tdso_e1x_t1x;   /* 1 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e2x_t2x;   /* 2 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e4x_t4x;   /* 4 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e8x_t8x;   /* 8 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 tdso_e16x_t16x; /* 16 x (9.6 or 14.4), depending on SCH MUX option */
  uint32 tdso_e1x_tb;    /* 1 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e2x_tb;    /* 2 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e4x_tb;    /* 4 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e8x_tb;    /* 8 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 tdso_e16x_tb;   /* 16 x (9.6 or 14.4) expected, blank tx'd instead */
  uint32 tdso_esb_tb;     /* Blank   */

  /* Received Frame counters for Fundicated channel FCH/DCCH              */
  uint32 tdso_e1_r1;    /* Error-free Rate 1 frame with no dim-and-burst  */
  uint32 tdso_e1_rerr;  /* Rate 1 with bit errors                         */
  uint32 tdso_e1_rd;    /* Dim-and-burst frame                            */
  uint32 tdso_e1_ro;    /* Other rate Frame                               */
  uint32 tdso_e1_rb;    /* Blank-and-burst                                */
  uint32 tdso_e1_rfl;   /* Rate 1 physical layer frame with insufficient
                           physical layer frame quality                   */
  uint32 tdso_e1_re;    /* Insufficient frame quality (erasure)           */
  uint32 tdso_en_rn;    /* Null  */
  uint32 tdso_en_rb;    /* Blank */
  uint32 tdso_en_ro;    /* Other */

  /* Received SCH0 frame counters */
  uint32 tdso_e1x_r1x;   /* Error-free 1 x (9.6 or 14.4) frame          */
  uint32 tdso_e2x_r2x;   /* Error-free 2 x (9.6 or 14.4) frame          */
  uint32 tdso_e4x_r4x;   /* Error-free 4 x (9.6 or 14.4) frame          */
  uint32 tdso_e8x_r8x;   /* Error-free 8 x (9.6 or 14.4) frame          */
  uint32 tdso_e16x_r16x; /* Error-free 16 x (9.6 or 14.4) frame         */
  uint32 tdso_e1x_rerr;  /* 1 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e2x_rerr;  /* 2 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e4x_rerr;  /* 4 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e8x_rerr;  /* 8 x (9.6 or 14.4) frame with bit errors     */
  uint32 tdso_e16x_rerr; /* 16 x (9.6 or 14.4) frame with bit errors    */
  uint32 tdso_e1x_re;    /* Expected 1 x (9.6 or 14.4), erasure         */
  uint32 tdso_e2x_re;    /* Expected 2 x (9.6 or 14.4), erasure         */
  uint32 tdso_e4x_re;    /* Expected 4 x (9.6 or 14.4), erasure         */
  uint32 tdso_e8x_re;    /* Expected 8 x (9.6 or 14.4), erasure         */
  uint32 tdso_e16x_re;   /* Expected 16 x (9.6 or 14.4), erasure        */
  uint32 tdso_e1x_rb;    /* 1 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e2x_rb;    /* 2 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e4x_rb;    /* 4 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e8x_rb;    /* 8 x (9.6 or 14.4) expected, blank received  */
  uint32 tdso_e16x_rb;   /* 16 x (9.6 or 14.4) expected, blank received */
  uint32 tdso_esb_rb;     /* Blank                                       */
  uint32 tdso_esb_ro;     /* Frame category other than blank             */


  /* Received PDU counters for the Supplemental Channels                */
  uint32 tdso_e3_r3;    /* Error-free Rate 3 MuxPDU                     */
  uint32 tdso_e3_rerr;  /* Rate 3 MuxPDU with Errors                    */
  uint32 tdso_e3_re;    /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e2_r2;    /* Error-free Rate 2 MuxPDU                     */
  uint32 tdso_e2_rerr;  /* Rate 2 MuxPDU with Errors                    */
  uint32 tdso_e2_re;    /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e1a_r1a;   /* Error-free Rate 1a MuxPDU                    */
  uint32 tdso_e1a_rerr; /* Rate 1a MuxPDU with Errors                   */
  uint32 tdso_e1a_re;   /* Insufficient Frame Quality, Erasures         */
  uint32 tdso_e1b_r1b;   /* Error-free Rate 1b MuxPDU                    */
  uint32 tdso_e1b_rerr; /* Rate 1a MuxPDU with Errors                   */
  uint32 tdso_e1b_re;   /* Insufficient Frame Quality, Erasures         */

} mc_log_ftdso_type;

/*---------------------------------------------------------------------------
  Simple Test Data Service Option (TDSO) Log Type
---------------------------------------------------------------------------*/
/* LOG_STDSO_C */
typedef PACK(struct)
{
  log_hdr_type hdr;
  uint16 for_mux_opt; /* Forward MUX option */
  uint16 rev_mux_opt; /* REverse MUX option */

  /* Transmit FCH frame counters */
  uint32 e1_t1;      /* Rate 1 with no signaling                */
  uint32 e2_t2;      /* Rate 1/2 with no signaling              */
  uint32 e4_t4;      /* Rate 1/4 with no signaling              */
  uint32 e8_t8;      /* Rate 1/8 with no signaling              */
  uint32 e1_td;      /* Rate 1 with dim-and-burst signaling     */
  uint32 e2_td;      /* Rate 1/2 with dim-and-burst signaling   */
  uint32 e4_td;      /* Rate 1/4 with dim-and-burst signaling   */
  uint32 e8_td;      /* Rate 1/8 with dim-and-burst signaling   */
  uint32 e1_tb;      /* Rate 1 with blank-and-burst signaling   */
  uint32 e2_tb;      /* Rate 1/2 with blank-and-burst signaling */
  uint32 e4_tb;      /* Rate 1/4 with blank-and-burst signaling */
  uint32 e8_tb;      /* Rate 1/8 with blank-and-burst signaling */
  uint32 eb_tb;      /* Blank                                   */
  uint32 eb_to;      /* Frame category other than blank         */

  /* Transmit SCH0 frame counters */
  uint32 es1x_t1x;   /* 1 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es2x_t2x;   /* 2 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es4x_t4x;   /* 4 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es8x_t8x;   /* 8 x (9.6 or 14.4), depending on SCH MUX option  */
  uint32 es16x_t16x; /* 16 x (9.6 or 14.4), depending on SCH MUX option */
  uint32 es1x_tb;    /* 1 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es2x_tb;    /* 2 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es4x_tb;    /* 4 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es8x_tb;    /* 8 x (9.6 or 14.4) expected, blank tx'd instead  */
  uint32 es16x_tb;   /* 16 x (9.6 or 14.4) expected, blank tx'd instead */
  uint32 esb_tb;     /* Blank                                           */

  /* Received FCH frame counters */
  uint32 e1_r1;      /* Error-free Rate 1 frame with no dim-and-burst         */
  uint32 e1_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e1_rd;      /* Dim-and-burst frame                                   */
  uint32 e1_rb;      /* Blank-and-burst                                       */
  uint32 e1_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e1_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e1_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e1_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e1_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e2_r2;      /* Error-free Rate 1/2 frame with no dim-and-burst       */
  uint32 e2_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e2_rd;      /* Dim-and-burst frame                                   */
  uint32 e2_rb;      /* Blank-and-burst                                       */
  uint32 e2_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e2_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e2_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e2_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e2_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e4_r4;      /* Error-free Rate 1/4 frame with no dim-and-burst       */
  uint32 e4_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e4_rd;      /* Dim-and-burst frame                                   */
  uint32 e4_rb;      /* Blank-and-burst                                       */
  uint32 e4_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e4_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e4_r8;      /* Rate 1/8 frame with no dim-and-burst                  */
  uint32 e4_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e4_re;      /* Insufficient frame quality (erasure)                  */
  uint32 e8_r8;      /* Error-free Rate 1/8 frame with no dim-and-burst       */
  uint32 e8_rerr;    /* Rate 1 with bit errors detected by the service option */
  uint32 e8_rd;      /* Dim-and-burst frame                                   */
  uint32 e8_rb;      /* Blank-and-burst                                       */
  uint32 e8_r1;      /* Rate 1 frame with no dim-and-burst                    */
  uint32 e8_r2;      /* Rate 1/2 frame with no dim-and-burst                  */
  uint32 e8_r4;      /* Rate 1/4 frame with no dim-and-burst                  */
  uint32 e8_rfl;     /* Rate 1 physical layer frame with insufficient
                        physical layer frame quality                          */
  uint32 e8_re;      /* Insufficient frame quality (erasure)                  */
  uint32 en_rn;      /* Null  */
  uint32 en_rb;      /* Blank */
  uint32 en_ro;      /* Other */

  /* Received SCH0 frame counters */
  uint32 es1x_r1x;   /* Error-free 1 x (9.6 or 14.4) frame          */
  uint32 es2x_r2x;   /* Error-free 2 x (9.6 or 14.4) frame          */
  uint32 es4x_r4x;   /* Error-free 4 x (9.6 or 14.4) frame          */
  uint32 es8x_r8x;   /* Error-free 8 x (9.6 or 14.4) frame          */
  uint32 es16x_r16x; /* Error-free 16 x (9.6 or 14.4) frame         */
  uint32 es1x_rerr;  /* 1 x (9.6 or 14.4) frame with bit errors     */
  uint32 es2x_rerr;  /* 2 x (9.6 or 14.4) frame with bit errors     */
  uint32 es4x_rerr;  /* 4 x (9.6 or 14.4) frame with bit errors     */
  uint32 es8x_rerr;  /* 8 x (9.6 or 14.4) frame with bit errors     */
  uint32 es16x_rerr; /* 16 x (9.6 or 14.4) frame with bit errors    */
  uint32 es1x_re;    /* Expected 1 x (9.6 or 14.4), erasure         */
  uint32 es2x_re;    /* Expected 2 x (9.6 or 14.4), erasure         */
  uint32 es4x_re;    /* Expected 4 x (9.6 or 14.4), erasure         */
  uint32 es8x_re;    /* Expected 8 x (9.6 or 14.4), erasure         */
  uint32 es16x_re;   /* Expected 16 x (9.6 or 14.4), erasure        */
  uint32 es1x_rb;    /* 1 x (9.6 or 14.4) expected, blank received  */
  uint32 es2x_rb;    /* 2 x (9.6 or 14.4) expected, blank received  */
  uint32 es4x_rb;    /* 4 x (9.6 or 14.4) expected, blank received  */
  uint32 es8x_rb;    /* 8 x (9.6 or 14.4) expected, blank received  */
  uint32 es16x_rb;   /* 16 x (9.6 or 14.4) expected, blank received */
  uint32 esb_rb;     /* Blank                                       */
  uint32 esb_ro;     /* Frame category other than blank             */
} mc_log_stdso_type;


/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION : TDSO_INITIALIZER

DESCRIPTION:
  This function will initialize the TDSO On a per call basis.
  This initializer function is registered with SNM and will be
  called when the service option has been initialized.

  Note the two arguments are required to be compatible with
  the service negotiation manager design, but are not used.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/

void tdso_initializer
(
  const caii_srv_cfg_type* cfg, /* Service configuration info,
                                   Not used by this function, but
                                   it is required to be passed in
                                   by the initilizer */

  word connect_info /*lint -esym(715,connect_info) Connection number not used*/
);

/*===========================================================================

FUNCTION : TDSO_INIT_SO_CFG

DESCRIPTION
  Registers the TDSO service option with SNM.

 DEPENDENCIES
  An instance of snm_config_type (default configuration) and snm_neg_par_type
  (negotiation parameters) should have been defined for the TDSO service option
  being registered, and should have been initialized with meaningful values
  prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void tdso_init_so_cfg(void);

/*===========================================================================

FUNCTION :   TDSO_BUILD_TX_FRAME

DESCRIPTION

  This function is used by the Multiplex layer to fetch the FCH or SCH
  Channel transmission TDSO frame buffer.  For simple TDSO the buffer is padded
  with all '1's. For Full TDSO, the TDSO frame is packed with data pattern
  selected by the Control derivatives.

DEPENDENCIES
  This functin depends on the MUX layer to properly pass the correct number
  of PDUs based on the MUX rate of the SCH/FCH and also PDU type.

RETURN VALUE

  This function returns the FCH/SCH frame based on the frame type requested.
  During the TDSO OFF state, this function returns a BLANK frame instead of
  a requested TDSO frame type.

SIDE EFFECTS
  None

===========================================================================*/
tdso_status_type tdso_build_tx_frames
(
  tdso_link_dir_type    direction,         /* FORWARD/REVERSE LINK */
  tdso_frame_block_type *tdso_fr_blk     /* Ptr to frames block */
);

/*===========================================================================

FUNCTION : TDSO_PROCESS_RX_FRAMES


DESCRIPTION
  This function processes the TDSO frame(s) received over the air. This is the
  interface between TDSO receive and the multiplex sublayer. The multiplex
  sublayer can send all the frames it received in one frame time to TDSO or
  split it up into multiple calls of this function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

tdso_status_type tdso_process_rx_frames
(
  tdso_frame_block_type *tdso_frame_block /* Ptr to rcvd TDSO frames    */
);

/*===========================================================================

FUNCTION : TDSO_PROCESS_SOCM_MSG

DESCRIPTION:
  This function processes the received SOCM. It validates the content of the
  message. If the SOCM is received within the 1 sec boundary of SYNC frame
  and its content is valid, it saves the message content in TDSO_SOCM for
  later update of TDSO test parameters. Otherwise, it updates the TDSO test
  parameters right away if the content of SOCM is valid.

DEPENDENCIES:
  None.

RETURN VALUE:
  TRUE - valid SOCM.
  FALSE - invalid SOCM.

SIDE EFFECTS:
  None.

===========================================================================*/

boolean tdso_process_socm_msg
(
  uint8* tsf,
  uint8  tsf_len
);

/*===========================================================================

FUNCTION : TDSO_GET_VECT

DESCRIPTION
  This function updates a given table with the appropiate TDSO statistics.

DEPENDENCIES
  None

RETURN VALUE
  Length of vector (vector elements are dword here).

SIDE EFFECTS
  None

===========================================================================*/

uint8 tdso_get_vect
(
  uint8  ctl_rec_type,    /* Control record type */
  uint8  vect_counter_id, /* Vector counter id */
  uint32 *tsf             /* Address of storage for vector */
);

/*===========================================================================
FUNCTION     TDSO_NOTIFY_HHO_SUCCESS

DESCRIPTION
  This function is called by MC when a HHO has been successfully performed.
  If a full TDSO is in progress, this function sets the current TDSO state
  to FALSE so that all 1's are transmitted util the next SYNC frame.  The TDSO
  also sends the Service Option Control Message (SOCM) of the test parameters
  that were used before the Handoff.  This is a full TDSO requirement.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void tdso_notify_hho_success(void);

/*===========================================================================
FUNCTION     TDSO_END_CALL

DESCRIPTION
  This function is called by MC in the release substate.  It does some
  cleanup needed when ending a TDSO call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void tdso_end_call(void);

/*===========================================================================

FUNCTION : TDSO_DV_CALL_IS_ON

DESCRIPTION
  This function returns TRUE if the Full TDSO call is on.

DEPENDENCIES
  None

RETURN VALUE
  TRUE or FALSE.

SIDE EFFECTS
  None

===========================================================================*/
boolean tdso_dv_call_is_on(void);


#endif /* TDSO_V_H */
