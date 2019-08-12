/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 RLC INTERNAL DEFINITIONS
***
***
*** DESCRIPTION
***
***  RLC data definitions.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlci.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** 03/15/01    hv     RLC Internal defines
*** 04/21/04    hv     Added EGPRS uplink data block typee
*** 09/19/12    hv     Added dual data space functionality
*****************************************************************************/

#ifndef INC_GRLCI_H
#define INC_GRLCI_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif


#include "gprs_pdu.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlcmaccmn.h"
#include "gl1rlccmn.h"
#include "geran_multi_sim.h"
#include "grlcmsgenum.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/*----------------------------------------------------------------------------
** Table of uplink resource re-allocation actions (8.1.1.1.2)
**
** During xfer of a pdu, LLC may request for xfer of another pdu resulting
** in resource re-allocation. Before countdown procedure has started there
** is a chance for a resource re-allocation request. If sucessful xfer shall
** continue using the same TBF. Requirements of the new and current pdus are
** compared in terms of rlc_mode, radio priority and peak-throughput class.
** The outcome decides the types of resource reallocation actions as follows:
**
** RLC_MODE       RADIO PRIORITY     PEAK-THROUGHPUT  ACTION
**                                   CLASS
**
** same           (new pdu) same     (new pdu)        (1) immediate res
**                or higher          higher           realloc req. Then complete
**                                                    tx of current pdu.
**
** same           (new pdu)          (new pdu)        (2) complete tx of cur
**                same or lower      lower            pdu, before getting ack
**                                                    (if in ack mode) send
**                                                    realloc req.
**
** different      (new pdu)          don't care       (3) complete tx of cur
**                higher                              pdu, release tbf, est new
**                                                    tbf (higher rp), tx it
**                                                    & other pdus (of high rp),
**                                                    release tbf, est new
**                                                    (lower rp) tbf, resume tx
**                                                    of (low rp) pdu's.
**
** Note that the current xfer continues as normal and there is no
** interruption or re-ordering of pdus. When the resource
** re-allocation request takes place in the course of the xfer, ie.
** immediate as in (1) and slightly delayed as in (2), or when the mode
** is different, the current TBF terminates and a new one is created and then
** continued if the following pdu(s) are of the same type. When they are not,
** the new high priority TBF terminates and another one created to xfer the
** remaining pdus.
**
** Action (1) means that request for resources is done right away.
** Action (3) means no action is required as it will be done when the new
** tbf is established.
** Action (2) means resource request needs to be delayed until transfer of
** current pdu is complete, but without waiting for ack if in ack mode.
**
**----------------------------------------------------------------------------
*/
/* Correction of cps value when NW sends a incorrect cps in
** MCS6/MCS3 spb=2 block when the initial transmision was MCS8
*/
#define FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING

/*
** Perform validation of Length indicators on an RLC block.
** This is done becuase sometimes L1 passes block with payload crc pass
** but has invalid LI suggesting as false CRC pass and the block bsn is mark
** as crc , and hence NACKED in DL_ACK_NACK.
*/
#define FEATURE_GSM_GPRS_RLC_VALIDATE_BSN_LI

typedef enum
{
  /* null/inactive state
  */
  RA_NULL,

  /* PRR type 1: send PRR immediately.
  */
  RA_IMMEDIATE,

  /* PRR type 2: send PRR after tx of current pdu is complete but before getting
  ** ack from the NW
  */
  RA_AT_TX_COMPLETE,

  /* PRR type 3: release current tbf and establish new tbf to transmit the next PDU,
  ** or there is no next pdu
  */
  RA_AT_NEW_TBF,

  /* PRR type 3: send PRR when all blocks in current PDU has been
  ** transmitted and acknowledged by the NW -- if in acknowledged mode
  */
  RA_AT_TX_COMPLETE_CHANGE_MODE

} realloc_action_t;


/* RLC UL and DL signal handling states
*/
typedef enum
{
  RLC_NOT_COMPLETED,
  RLC_COMPLETED

} grlc_completion_status_t;

/* Store PS before doing MCS Demotion.
*/
#define NO_MCS_DEMOTION   0

/*-----------------------------------------------
** Defines to shorten the various RLC UL queues
**-----------------------------------------------
*/
#define ULQ(x)  grlc_ul_llc_pdu_q[x]		/* uplink LLC PDU queue */
#define ULTQ(x) grlc_ul_llc_tmp_pdu_q[x]	/* uplink LLC TEMP PDU queue */
#define ULMQ(x) grlc_ul_llc_msg_q[x]		/* uplink LLC MSG queue */
#define UMQ(x)  grlc_ul_mac_q[x]			/* uplink MAC queue */
#define UTQ(x)  grlc_ul_tmr_q[x]			/* uplink TIMER queue */


/*-----------------------------------------------
** Defines to shorten the various RLC DL queues
**-----------------------------------------------
*/
#define DMQ(x)  grlc_dl_mac_q[x]		    /* downlink MAC queue */
#define DTQ(x)  grlc_dl_tmr_q[x]		    /* downlink MAC queue */
#define DPQ(x)  grlc_dl_pl1_q[x]		    /* downlink PL1 queue */

/*---------------------------------------------------------------------
** RLC data block encoded using coding scheme 4 comprising the 2-octet
** RLC header, 50-octet payload (inc RLC extended header, TLLI field),
** plus 1 extra octet for the spare bits to give 51 octets.
** (04.60 10.2)
**---------------------------------------------------------------------
*/
#define GRLC_DATA_BLOCK_MAX_LEN   53

#define E_GRLC_DATA_BLOCK_MAX_LEN   76    /* 74+1 inc the FBI/TI-E fields */
                                          /* 76 ensures word-aligned for mdsp */
#define E_GRLC_UL_HDR_TYPE_1_LEN    6
#define E_GRLC_UL_HDR_TYPE_2_LEN    5
#define E_GRLC_UL_HDR_TYPE_3_LEN    4
#define E_GRLC_DL_HDR_TYPE_1_LEN    5
#define E_GRLC_DL_HDR_TYPE_2_LEN    4
#define E_GRLC_DL_HDR_TYPE_3_LEN    4
#define E_GRLC_HDR_TYPE_MAX_LEN     6     /* For both UL and DL */

/* Coding and puncturing scheme for header type 1
*/
#define MCS_9_P1_MCS_9_P1   0
#define MCS_9_P1_MCS_9_P2   1
#define MCS_9_P1_MCS_9_P3   2
#define MCS_9_P2_MCS_9_P1   4
#define MCS_9_P2_MCS_9_P2   5
#define MCS_9_P2_MCS_9_P3   6
#define MCS_9_P3_MCS_9_P1   8
#define MCS_9_P3_MCS_9_P2   9
#define MCS_9_P3_MCS_9_P3   10
#define MCS_8_P1_MCS_8_P1   11
#define MCS_8_P1_MCS_8_P2   12
#define MCS_8_P1_MCS_8_P3   13
#define MCS_8_P2_MCS_8_P1   14
#define MCS_8_P2_MCS_8_P2   15
#define MCS_8_P2_MCS_8_P3   16
#define MCS_8_P3_MCS_8_P1   17
#define MCS_8_P3_MCS_8_P2   18
#define MCS_8_P3_MCS_8_P3   19
#define MCS_7_P1_MCS_7_P1   20
#define MCS_7_P1_MCS_7_P2   21
#define MCS_7_P1_MCS_7_P3   22
#define MCS_7_P2_MCS_7_P1   23
#define MCS_7_P2_MCS_7_P2   24
#define MCS_7_P2_MCS_7_P3   25
#define MCS_7_P3_MCS_7_P1   26
#define MCS_7_P3_MCS_7_P2   27
#define MCS_7_P3_MCS_7_P3   28

/* Coding and puncturing scheme for header type 2
*/
#define MCS_6_P1                  0
#define MCS_6_P2                  1
#define MCS_6_P1_PAD_MSC_8_RETX   2
#define MCS_6_P2_PAD_MCS_8_RETX   3
#define MCS_5_P1                  4
#define MCS_5_P2                  5

/* Coding and puncturing scheme for header type 3
*/
#define MCS_4_P1                  0
#define MCS_4_P2                  1
#define MCS_4_P3                  2
#define MCS_3_P1                  3
#define MCS_3_P2                  4
#define MCS_3_P3                  5
#define MCS_3_P1_PAD_MCS_8_RETX   6
#define MCS_3_P2_PAD_MCS_8_RETX   7
#define MCS_3_P3_PAD_MCS_8_RETX   8
#define MCS_2_P1                  9
#define MCS_2_P2                  10
#define MCS_1_P1                  11
#define MCS_1_P2                  12

/* Split Block indicator field (SPB)
*/
#define SPB_NO_RETX               0
#define SPB_RESERVED              1
#define SPB_RETX_PART_1           2
#define SPB_RETX_PART_2           3

/* EGPRS Suplementary/Polling (ES/P) Field
*/
#define ESP_NO_POLL               0         /* No polling - RRBP not valid */
#define ESP_FPB_NO_QREP           1         /* First partial bitmap no quality report */
#define ESP_NPB_NO_QREP           2         /* Next partial bitmap no quality report */
#define ESP_NPB_QREP              3         /* Next partial bitmap with quality report */

/* Fixed part of ul/dl RLC header: octet 1 and octet 2 */
#define GRLC_FIXED_HDR_LEN         2

/* RLC fixed header length is 2 octets long
*/
#define RM_RLC_HDR_LEN            2

/* MAC header length is 1 octet
*/
#define RM_MAC_HDR_LEN            1

/*-------------------------------------------------------
** Smallest LLC message is 5 octets long (info from LLC)
**-------------------------------------------------------
*/
#define LLC_PDU_MIN_LEN           5

/* The smallest LLC PDU size is 5 octets. For each PDU stored an extension
** octet may be required to delimit it. The largest RLC data block size (CS_4)
** is 52 octets. The payload size is 50 octets without the RLC header.
** So, a payload can contain upto 50/6 = 8 LLC PDUs. Hence the maximum number
** of extension octets must be 7.
*/

/* FOR PERLUTF - it does'nt round a float variable , and hence requires a
   whole value.
*/

#ifdef PERLUTF
  #error code not present
#else
#define MAX_NUM_EXTENSION_OCTETS    (((GRLC_DATA_BLOCK_MAX_LEN -    \
                                       GRLC_FIXED_HDR_LEN           \
                                      ) / (LLC_PDU_MIN_LEN+1)       \
                                     ) - 1                          \
                                    )
#endif /* PERLUTF */

/* For EGPRS the smallest LLC PDU size is 5 octets. For each PDU stored an extension
** octet may be required to delimit it. The largest data unit payload size (MCS_9)
** is 74 octets. So, a payload can contain upto 74/6 = 12.x LLC PDUs. Hence the
** maximum number of extension octets must be 12

** + 1 -  is added , considering 12 Valid L1 ( 12 * 6 = 72 bytes ) + LI=127 for
** remaining bytes which makes the expresion to have 13 LI.
*/

  /* FOR PERLUTF - it does'nt round a float variable , and hence requires a
     whole value.
  */
#ifdef PERLUTF
  #error code not present
#else
  #define E_MAX_NUM_EXTENSION_OCTETS    ((E_GRLC_DATA_BLOCK_MAX_LEN  \
                                       / (LLC_PDU_MIN_LEN+1)       \
                                     ) + 1                         \
                                    )
#endif /* PERLUTF */

/*--------------------------------------------------------------------
** Maximum length of radio block as prepared by RLC to pass to PL1 is
** GRLC_DATA_BLOCK_MAX_LEN plus the MAC header to give 54 octets
**--------------------------------------------------------------------
*/
#define GRLC_RADIO_BLOCK_MAX_LEN    (GRLC_DATA_BLOCK_MAX_LEN+RM_MAC_HDR_LEN)


/* definitions of bit fields in RLC UL and DL data blocks
*/

/*-------------------------------
** Extension (E) bit definition
**-------------------------------
*/
#define RM_EXT_OCTET_FOLLOWS          0
#define RM_EXT_NO_OCTET_FOLLOWS       1

/*-------------------------------
** RLC Filler Octet definitions
**-------------------------------
*/
#define RM_UL_FILLER_OCTET            0x2B  /* for padding the last UL RLC data block */
#define RM_DL_FILLER_OCTET            0x00  /* for substituting DL data not received */

/*---------------------------------------------
** EGPRS RLC Length indicator for Filler Octets
**----------------------------------------------
*/
#define RM_FILLER_OCTET_LEN           127

/*=======================================================================
** A FIFO is used to radio blocks in the downlink where portable layer 1
** deposits radio blocks and where RLC DL picks up the radio blocks for
** processing.
**
** When portable layer receives a valid radio block from the DSP in the
** form of a straight buffer. It adds a header which contains the coding
** scheme, frame number and time-slot to create a structure of the type
** rlc_dl_pl1_data_t. PL1 then converts the whole buffer into DMS item
** and inserts its pointer into the FIFO.
**=======================================================================
*/

/*-----------------------------------
** Portable Layer radio block header
**-----------------------------------
*/
typedef struct
{
  uint8   cs;
  uint8   time_slot;
  uint32  frame_number;

  /* Initialized to 0. It is set to number when PL1 wants this block to be
  ** sent on the UL (looped back) during TEST MODE B mode of operation.
  */
  uint8 loop_dl_blk;

} pl1_radio_block_header_t;

typedef struct
{
  pl1_radio_block_header_t  pl1_hdr;
  uint8                     block[GRLC_RADIO_BLOCK_MAX_LEN];

} pl1_radio_block_t;

/* Table of EGPRS Coding and Puncturing Sheme (CPS) values for header type 1/2/3.
*/
extern const uint8 E_GRLC_HDR1_CPS[];
extern const uint8 E_GRLC_HDR2_CPS[];
extern const uint8 E_GRLC_HDR3_CPS[];

/* EGPRS RLC DL Header Internal Definition as per TS 04.60 10.3a.3.2
*/
typedef struct
{
  /* EGPRS RLC DL common field length definitions for all header types
  */
  #define e_grlc_dl_hdr_usf_len   3
  #define e_grlc_dl_hdr_esp_len   2
  #define e_grlc_dl_hdr_rrbp_len  2
  #define e_grlc_dl_hdr_tfi_len   5
  #define e_grlc_dl_hdr_pr_len    2
  #define e_grlc_dl_hdr_bsn1_len 11

  /* EGPRS RLC DL Header Type 1 field length definitions
  */
  #define e_grlc_dl_hdr_t1_bsn2_len   10
  #define e_grlc_dl_hdr_t1_cps_len    5

  /* EGPRS RLC DL Header Type 2 field length definitions
  */
  #define e_grlc_dl_hdr_t2_cps_len    3

  /* EGPRS RLC DL Header Type 3 field length definitions
  */
  #define e_grlc_dl_hdr_t3_cps_len    4
  #define e_grlc_dl_hdr_t3_spb_len    2

  uint8     usf;
  uint8     esp;
  uint8     rrbp;
  uint8     pr;
  uint8     tfi;
  uint16    bsn1;
  uint16    bsn2;
  uint8     cps;
  uint8     spb;

} e_grlc_dl_header_t;




/* GPRS RLC / MAC Header lengths for xlate_ext_dl_data.
*/
#define  grlc_dl_data_block_t_mac_hdr_len   8

/* RLC header octet
*/
/* Power reduction
*/
#define  grlc_dl_data_block_t_pr_len      2

/* temporary field indentity
*/
#define  grlc_dl_data_block_t_tfi_len     5

/* final block indicator
*/

#define  grlc_dl_data_block_t_fbi_len     1

/* RLC header octet 2
*/

/* Block sequence number
*/
#define  grlc_dl_data_block_t_bsn_len     7

/* extension
*/
#define  grlc_dl_data_block_t_ext_len     1


/*------------------------------------------------------------------
** EGPRS downlink data block representing an element of the DL FIFO
** 22 octets
**------------------------------------------------------------------
*/
typedef struct
{
  /* RLC must know the mcs before it can parse a header
  */
  uint8               mcs;

  /* FBI/E bits for the payload(s).
  **  bit7 ... bit1 bit0
  **           FBI     E
  **
  ** fbi_e_2 is only applicable to MCS-7/8/9.
  ** fbi_e_1 and fbi_e_2 values are only considered CRC status is a pass in un-ack mode
  ** (see crc_1/2 below)
  */
  uint8               fbi_e_1;
  uint8               fbi_e_2;

  /* CRC status of payload_1 and payload_2. These vars are applicable to both
  ** ack and un-ack mode. In un-ack mode, a payload that fails CRC shall be
  ** passed to LLC instead of filler octets. However, the associated FBI/E
  ** bits are then discarded.
  **
  ** In ack mode, if a block passes CRC then the appropriate crc_1/2 must be set
  ** as well as payload_1/2_ptr set to a non-NULL value. This is because crc_1/2
  ** are used to check if RLC requires context to process the DL FIFO
  */
  uint8               crc_1;
  uint8               crc_2;

  /* RLC needs the RRBP and frame number and timeslot to pass to MAC to send
  ** when RLC requests MAC to send a PCA
  */
  uint8               rrbp;
  uint8               time_slot;
  uint32              frame_number;

  /* Already parsed by PL1 so RLC won't have to parse it
  */
  uint8               esp;

  /* RLC header array containing 5 octets to accomodate the largest DL header type 1.
  ** Not all octets and bits of this array are used. This array shall be translated
  ** into an internal structure of the type: egrlc_dl_hdr_type_t
  */
  uint8               hdr_arr[E_GRLC_DL_HDR_TYPE_1_LEN];

  /* Payloads as dsm-item pointers. ptr_2 should be NULL for MCS-1/2/3/4/5/6.
  ** A NULL value means the payload is not passed by PL1 or does not exist for that MCS.
  ** In unack mode, if a payload fails CRC, PL1 should still pass it onto to RLC.
  **
  ** The payload should be octet-aligned without padding octets and without FBI/E bits
  **
  ** If padding octets were present, they should have been removed.
  ** The FBI/E bits should have been shifted out and stored separately in fbi_e_1/2.
  **
  ** When padding octets are removed RLC should expect the length of the
  ** payload to be:
  ** a) 68 octets for MCS-6 or
  ** b) 31 octets for MCS-3
  */
  dsm_item_type*      payload_1_ptr;
  dsm_item_type*      payload_2_ptr;

/* HV to change this into OR PERL_UTF
*/
#if (defined(DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined(PERLUTF))
  #error code not present
#endif /* (defined(DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined(PERLUTF)) */

  /* Initialized to FALSE. It is set TRUE when block is put in the FIFO.
  ** It is set to FALSE when a block is read from the FIFO.
  */
  boolean             b_block_put;

  /* Initialized to 0. It is set to num when PL1 wants this block to be
  ** sent on the UL (looped back) during TEST MODE B mode of operation.
  */
  uint8              loop_dl_blk;

} e_pl1_radio_block_t;

/*--------------
** uplink data
**--------------
*/

/*--------------------------------------------------------------------------
** RLC Extension Octet - GPRS mode
**
** This octet comprises the Length Indicator (LI), (M)ore and (E)xtension
** fields. When present, they determine the boundaries between PDUs within
** an RLC data block.
**
** The length information for each item is named using the format:
** <name of base struct>_<name of member variable>_len
** To refer to it later on use the macro GSIZE.
** eg. GSIZE (grlc_ext_octet_t, li) expands to grlc_ext_octet_t_li_len
**--------------------------------------------------------------------------
*/
typedef struct
{
  uint8 li:6;
  #define  grlc_ext_octet_t_li_len       6

  uint8 more:1;
  #define  grlc_ext_octet_t_more_len     1

  uint8 ext:1;
  #define  grlc_ext_octet_t_ext_len      1

} grlc_ext_octet_t;

/*--------------------------------------------------------------------------
** EGPRS RLC Extension Octet comprises the Length Indicator (LI) and the
** (E)xtension fields. When present, they determine the boundaries between
** PDUs within an RLC data block.
**
** In EGPRS, The M field is absorbed to increase the LI field length to 7 bits
**
** The length information for each item is named using the format:
** <name of base struct>_<name of member variable>_len
** To refer to it later on use the macro GSIZE.
** eg. GSIZE (grlc_ext_octet_t, li) expands to grlc_ext_octet_t_li_len
**--------------------------------------------------------------------------
*/
typedef struct
{
  uint8 li:7;
  #define  e_grlc_ext_octet_t_li_len       7

  uint8 ext:1;
  #define  e_grlc_ext_octet_t_ext_len      1

} e_grlc_ext_octet_t;

/*-----------------------------------------------------------------------
** Union of extension octet for GPRS and EGPRS
**-----------------------------------------------------------------------
*/
typedef union
{
  grlc_ext_octet_t    gprs;
  e_grlc_ext_octet_t  egprs;

} ext_octet_u;

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */

/*-------------------------------------------------------------------
*  Receive Acknowledge State
*  Possible values for each element of the Receive state array V(N)
*--------------------------------------------------------------------
*/
typedef enum
{
  /* when an element falls outside the active rx window as per
  ** [V(R) <= BSN < V(R) - k] mod SNS
  **
  */
  RXS_INVALID = 0,

  /* Received into the RX Array bit not yet assembled into LLC PDU. May or may
  ** not been used to calculate bitmap
  */
  RXS_RECEIVED = 1,

  /* Received as well as assembled into LLC PDU and considered free to take up a
  ** new data block. May or may not been used to calculate the bitmap
  */
  RXS_RECEIVED_ASSEMBLED = 2

} grlc_rx_state_array_t;

/*---------------------------------------------------------------------------
** EGPRS RLC Downlink Internal Data Block Format. This struture repesents an
** element of the Receive Array. The structure comprises:
**
** -  DL header data fields in un-packed format. To be translated from
**    over-the-air format required by various processing functions
** -  Pointer to the data payload in dsm format so it can be appended
**    to the LLC PDU being built. Note that this payload contains PURE data and
**    does not include any LI/E fields. The Extension octets at the front have
**    already been unpacked into the Extension octet array mentioned below.
** -  Extension octet array (optional at the front of each payload) to indicate
**    the boundaries of PDU within a data block used by the Assembly functions
** -  The number of Extension octets in the data block
** -  BSN value of the block (or payload).
** -  Total number of octets in this data block
**
** Note:
**
** MCS-7/8/9 blocks will occupy 2 elements of the Receive Array. So, each
** block contains only one payload. Also, the header is duplicated for each element.
** 42 octets
**--------------------------------------------------------------------------
*/
typedef struct
{
  /*  Coding scheme for EGPRS and GPRS.
  **  Values for gprs  cs from 0->3 for CS1-CS4
  **  Values for egprs cs from 0->8 for MCS1-MCS9
  */

  uint8                 cs;
#ifdef FEATURE_GSM_GPRS_RLC_DL_CORRECT_DEPADDING
 /* Block's initial MCS - used for padding correction
  */
  boolean               b_extra_depadding;
#endif

  /* CRC status of payload. This var is applicable to both
  ** ack and un-ack mode. In un-ack mode, a payload that fails CRC shall be
  ** passed to LLC instead of filler octets. However, the associated FBI/E
  ** bits are then discarded.
  **
  ** In ack mode, if a block passes CRC then the crc must be set
  ** as well as payload_ptr set to a non-NULL value. This is because crc
  ** is used to check if RLC requires context to process the DL FIFO
  */
  uint8                 crc;

  /* Set TRUE when CRC has failed. This flag will influence the decision to
  ** respond to an EPDAN requested for a BSN that has not been received
  ** (RXS_RECEIVED) in RLC ACK mode.
  **
  ** This flag is needed due to the fact that the crc var above is initialised
  ** to "0", and we can not differentiate from a block that has been rcved and
  ** failed CRC from a block that has not been received at all.
  */
  boolean               failed_crc;

  /* split for an rlc block */
  uint8                 spb;

  /* The FBI/E fields in unpacked form.
  */
  uint8                 fbi;
  uint8                 ext;

  /* Block sequence number. Now 11 bits long
  */
  uint16                bsn;

  /* Array of Extension octets. Maximum 13 octets.
  ** Common for GPRS and EGPRS
  */
  ext_octet_u           ext_octet[E_MAX_NUM_EXTENSION_OCTETS];

  /* Number of extension octets
  */
  uint8                 num_ext_octets;

  /* Data block in dsm item format */
  dsm_item_type         *dsm_payload_ptr;


  /* Total number of data octets in all pdus within data block.
  */
  uint8                 len;

  /* V(N) Receive State
  ** receive status of each downlink data block in the rx_arr
  ** Ack mode only
  */
  grlc_rx_state_array_t vn;

} grlc_dl_data_block_t;

typedef struct
{

  /* EGPRS RLC UL common field length definitions for all header types
  */
  #define e_grlc_ul_hdr_retry_len   1
  #define e_grlc_ul_hdr_si_len      1
  #define e_grlc_ul_hdr_cv_len      4
  #define e_grlc_ul_hdr_tfi_len     5
  #define e_grlc_ul_hdr_bsn1_len    11
  #define e_grlc_ul_hdr_rsb_len     1
  #define e_grlc_ul_hdr_pi_len      1

  /* EGPRS RLC UL Header Type 1 field length definitions
  */
  #define e_grlc_ul_hdr_t1_bsn2_len   10
  #define e_grlc_ul_hdr_t1_cps_len    5
  #define e_grlc_ul_hdr_t1_spare_len  7

  /* EGPRS RLC UL Header Type 2 field length definitions
  */
  #define e_grlc_ul_hdr_t2_cps_len    3
  #define e_grlc_ul_hdr_t2_spare_len  10

  /* EGPRS RLC UL Header Type 3 field length definitions
  */
  #define e_grlc_ul_hdr_t3_cps_len    4
  #define e_grlc_ul_hdr_t3_spb_len    2
  #define e_grlc_ul_hdr_t3_spare_len  1

  /* Common fields for all header types
  */
  uint8   retry;

  uint8   si;

  uint8   cv;

  uint8   tfi;

  uint16  bsn1;

  uint16  bsn2;

  uint8   spb;

  uint8   cps;

  uint8   rsb;

  uint8   pi;

} e_grlc_ul_header_t;
/*--------------------------------------------------------------------
*  RLC Uplink Data Block
*
*  The RLC uplink data block comprises:
*  - 1 octet mac header
*  - rlc ul header octet 1
*  - rlc ul header octet 2
*  - extension octet array of variable size
*  - optional TLLI field depending on the TI field of ul hdr octet 1
*  - rlc ul data block
*
*---------------------------------------------------------------------
*/
typedef struct
{
  /* EGPRS UL MAC/RLC header
  */
  e_grlc_ul_header_t    hdr;

  /* GPRS MAC header - keep both GPRS and EGPRS headers for now
  */
  mac_hdr_t   mac_hdr;
  #define grlc_ul_data_block_t_mac_hdr_len   8

  /* rlc ul header octet 1 ?
  */

  /* Removed the spare field but keep the length definition
  */
  #define grlc_ul_data_block_t_spare_len   1

  uint8       pi;
  #define grlc_ul_data_block_t_pi_len       1

  /* Temporary field indentity
  */
  uint8       tfi;
  #define grlc_ul_data_block_t_tfi_len     5

  /* tlli indicator
  */
  uint8       ti;
  #define grlc_ul_data_block_t_ti_len      1

  /* rlc ul header octet 2
  */

  /* Block sequence number - only used for monitoring purposes
  */
  uint16      bsn;
  #define grlc_ul_data_block_t_bsn_len     7

  /* Extension bit
  */
  uint8       ext;
  #define grlc_ul_data_block_t_ext_len     1

  /* Extension octet union for GPRS and EGPRS
  ** ..rep ext_octet with ext_octet.gprs or ext_octet.egprs
  */
#ifndef PERLUTF
  ext_octet_u  ext_octet[E_MAX_NUM_EXTENSION_OCTETS];
#else
  #error code not present
#endif

  /* Optional tlli
  */
  uint32   tlli;
  #define grlc_ul_data_block_t_tlli_len    32

  uint8       pfi;
  #define grlc_ul_data_block_t_pfi_len    7
  #define e_grlc_ul_data_block_t_pfi_len  7

  #define grlc_ul_data_block_t_pfi_e_len  1
  #define e_grlc_ul_data_block_t_pfi_e_len  1

  /* This dsm item type points to the data of a RLC UL data block, ie. the
  ** RLC UL data block less than MAC header, RLC header (inc Extention
  ** Octets)
  */
  dsm_item_type         *dsm_ptr;

  /* A duplicate of the dsm_ptr above for use in re-transmission.
  ** When a pending-tx item is sent, a duplicate of dsm_ptr is made and
  ** stored in dup_dsm_ptr. gprs_pdu_strip_head() is then called with dsm_ptr.
  ** Because the 'dupped' pointer exists the memory is not freed.
  ** In all subsequent re-tx, a 'dup' version of dup_dsm_ptr is made and is
  ** stored in dsm_ptr. When a positive ack is received, dup_dsm_ptr is freed.
  */
  dsm_item_type         *dup_dsm_ptr;

#if defined (DEBUG_GSM_GPRS_RLC_TEST) ||  defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_RLC_TEST || PERLUTF*/

  /*-----------------------
  ** Internal information
  **-----------------------
  */

  /* Initialisation flag. When set, it indicates the block already contains
  ** pdu(s) and other pdu(s) may be concatenated. Also internal vars such
  ** as num_ext_octets, data_len, rem_cnt etc.. have been initialised and
  ** contain meaningful info.
  ** When clear, the block contains no pdu and other vars have not
  ** been initialised.
  */

  /* number of Extension Octets.
  */
  uint8                  num_ext_octets;

  /* number of data octets. May contain octets from more than 1 pdu.
  ** Also indexes the next free octet where new data will be stored.
  */
  uint8                  data_len;

  /* octets remained in the data block, after the RLC hdrs, ext octets,
  ** tlli field and previous pdus have been taken out of the length of
  ** the rcl block, say, 22 octets for a CS_1. For example,
  ** rem_cnt = 22 (CS_1) - 2 (rlc hdrs) - 1 (ext octet) - 4 tlli field
  **           - previous pdu (10)
  **         = 5 octets.
  **
  ** rem_cnt should never be 0. When it is 0, a new data block should
  ** be created.
  */
  uint8                 rem_cnt;

  /* rlc mode. ! to copy rlc mode from gprs_grr_ul_mes_t to grlc_ul_data_block_t
  */
  uint8                 rlc_mode;

  /* keep the coding scheme here for convenience
  */
  uint8                 cs;

  /* The initial MCS is the MCS that the block has been txed with for the
  ** first time and has been acknowledged by L1 as txed. The default value of this
  ** field is RM_MCS_UNDEF. It will take the value of the mcs filed below ONLY after L1
  ** acknowledges its transmission. Note that if a block needs to be re-processed and
  ** the initial MCS is set to RM_MCS_UNDEF then the commanded MCS should be considered
  ** as the MCS for this block.
  */
  uint8       init_mcs;
  /* MCS used last time the data block was transmited.
  ** old_mcs will be used to determine the CPS value of the block.
  */
  uint8       old_mcs;
  /* Puncturing Scheme to be used by L1 for this block.
  ** ps is the value to be used for the block's retransmission.
  ** Note that ps is rotated by the post processing function
  ** after PL1 acknowledges that the block has been transmitted.
  */
  uint8       ps;
  /* old_ps is the ps value used when the block was last transmitted.
  ** It will be used to determine the CPS value of the block.
  */
  uint8       old_ps;

  /* Default is NO_MCS_DEMOTION (0).
  */
  uint8       mcs_demoted_ps;

  /* offset into the current pdu to where this block starts. This offset
  ** mark the point where re-segmentation starts from when a coding scheme
  ** change occurs.
  */
  uint16                offset;

  /* Absolute block sequence number (BSN'). Used udring calculation of CV value
  */
  uint16                abs_bsn;

  /* Same as above for NACK block stuck as TXA_PENDING_PACK in FIFO */
  uint16      nack_miss_cnt;

  /* If PL1 miss-acknowledging a PTX block then count the number of times.
  ** When it reaches a threshold we can make the block PACK so it can be transmitted
  */
  uint16                ptx_miss_cnt;

  /* Holds the first PSN value in a multi-PDU block. When 'num_ext_octets' is 0
  ** its value is the same as last_psn
  */
  uint16                first_psn;

  /* PSN value of the last PDU in a multi-PDU data block, ie. block BSN x
  ** contains 3 PDUs:
  ** |----------|---------|----------|
  ** |-- psn ---|- psn+1 -|- psn+2 --|
  **     ^                    ^
  **     |                    |
  ** first_psn             last_psn
  **
  ** Also used to find PSN value of the block with CV=0
  */
  uint16                last_psn;

  /* PDU priority info determined by higher layer - TCP ack or NAS signaling etc..
  */
  geran_pdu_priority_t  pdu_prio;

} grlc_ul_data_block_t;

#ifdef PERLUTF
#error code not present
#endif /* PERLUTF */
/*------------------------------------------------------------------------
** Downlink RLC/MAC Control Block (not complete)
** This is defined for RLC usage, only as far as the MAC header. The rest
** of the uplink MAC control block is not required by RLC.
**------------------------------------------------------------------------
*/
typedef struct
{
  /* MAC header
  */
  mac_hdr_t mac_hdr;
  #define grlc_dl_ctrl_block_tmac_hdr_len   8

  /* the rest of the downlink MAC control block goes here
  */

} grlc_dl_ctrl_block_t;

/*------------------------------------------------------------------------
** Uplink RLC/MAC Control Block (not complete)
** This is defined for RLC usage, only as far as the MAC header. The rest
** of the uplink MAC control block is not required by RLC
**------------------------------------------------------------------------
*/
typedef struct
{
  /* MAC header
  */
  mac_hdr_t mac_hdr;
  #define  grlc_ul_ctrl_block_t_mac_hdr_len   8

  /* the rest of the uplink MAC control block goes here
  */

} grlc_ul_ctrl_block_t;


/*------------------------------------------------------------------
** System information elements relating to RLC UL and DL operations
**------------------------------------------------------------------
*/
typedef struct
{
  /* Index to the array storing durations in miliseconds for RLC DL to wait
  ** for release of the TBF after reception of the final block
  */
  uint8           t3192_index;

  /* RLC uplink related info
  */
  uint16          bs_cv_max;
  uint16          pan_inc;
  uint16          pan_dec;
  uint16          pan_max;

  /* This element may contain GPRS_TBF_MODE or EGPRS_TBF_MODE according to the
  ** network states it can support. However, the final value which determines
  ** if a TBF is EGPRS or GPRS only comes from the network's assignment via MAC's
  ** ALLOC_CNF message
  */
  uint16          tbf_mode;

  /* Extended Uplink TBF Indicator. This variable always exists and is set to 0
  ** if the feature is not supported. It is set to 1 if the feature is supported
  ** and active (cell option NW_EXT_UTBF is set)
  */
  uint16          ext_utbf;

  /* Network support of PFC procedures is indicated by the PFC_FEATURE_MODE parameter
  ** of the Extension Information (R99 extensions) of the CPRS Cell Options IE:
  ** PFC_FEATURE_MODE (1 bit field)
  **      0 The network does not support PFC procedures.
  **      1 The network supports PFC procedures.
  */
  uint16          pfc_feature_mode;

  /* Maximum multi slot class supported by MS , used for max data rate calculation.
  */
  uint16           max_gprs_multislot_class;
  uint16           max_egprs_multislot_class;

#ifdef FEATURE_GSM_DTM
  uint16           max_dtm_egprs_multislot_subclass;
#endif /* FEATURE_GSM_DTM */

#ifdef FEATURE_GSM_GPRS_MSC33
  uint16           max_hmsc_gprs_multislot_class;
  uint16           max_hmsc_egprs_multislot_class;
#endif  /* FEATURE_GSM_GPRS_MSC33 */

} gprs_sysinfo_t;


/* Number of downlink radio blocks that can buffered
** for ps_handover blind transmission
*/

#define DL_BLIND_DATA_BLKS              100


/* GPRS Downlink Block Header + Payload.
*/
typedef struct
{
  pl1_radio_block_header_t  pl1_blk_hdr;
  dsm_item_type             *pl1_blk_dsm_ptr;
} pl1_gprs_radio_block_t;

/* Union for GPRS and EGPRS downlink block for storing .
** blocks during ps_handover blind transmission.
*/
typedef union
{
  pl1_gprs_radio_block_t     dl_blk;
  e_pl1_radio_block_t        e_dl_blk;
} dl_radio_block_psho_t;


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/




/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*===========================================================================
===
===  MACRO         xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      xxx
===
===  DESCRIPTION
===
===    xxx.
===
===  DEPENDENCIES
===
===    xxx.
===
===  RETURN VALUE
===
===    xxx.
===
===  SIDE EFFECTS
===
===    xxx.
===
===========================================================================*/


#endif /* INC_GRLCI_H  */

/*** EOF: don't remove! ***/
