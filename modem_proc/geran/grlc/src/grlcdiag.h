/*****************************************************************************
***
*** TITLE
***
***  GPRS RLC DIAGNOSTIC INTERFACE
***
***
*** DESCRIPTION
***
***  Definition of data and functions provided by RLC for the purpose of
***  diagnostic and monitoring of RLC UL & DL operations
***
***
*** Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/src/grlcdiag.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 08/07/02    hv     Created
***
*****************************************************************************/

#ifndef INC_GRLCDIAG_H
#define INC_GRLCDIAG_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "gprsdef.h"
#include "gprsdef_g.h"
#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

#define GRLC_DIAG_UL_PDU_BUF_MAX     50
#define GRLC_DIAG_UL_BLOCK_BUF_MAX   10
#define GRLC_DIAG_UL_TX_BUF_MAX      10
#define GRLC_DIAG_DL_PDU_BUF_MAX     20
#define GRLC_DIAG_DL_RX_BUF_MAX      10

/*------------------------------------------------
** GRLC Uplink Diagnostic Structures
**------------------------------------------------
*/

/* RLC-UL Statictics
*/
typedef struct
{
  uint32  grlc_ul_state;
  uint32  grlc_ul_substate;

  uint32  ack_tx_pdu_cnt;
  uint32  ack_tx_oct_cnt;

  uint32  unack_tx_pdu_cnt;
  uint32  unack_tx_oct_cnt;

  uint16  coding_sch_changes;

  uint8   ul_seq_va;
  uint8   ul_seq_vs;
  uint8   ul_seq_nseg;

  uint8   ul_seq_vsf;
  uint8   ul_seq_pack_vsf;
  uint8   ul_seq_nack_vsf;

  int8    ul_n3102;
  int8    ul_n3104;

  uint32  ul_gen_purpose_count_a;
  uint32  ul_gen_purpose_count_b;
  uint32  ul_gen_purpose_count_c;

} grlc_diag_ul_statistics_t;


/* RLC-UL Abnormal Release Counts
*/
typedef struct
{
  uint16  access_reject_cnt;

  uint16  arc_retry_cnt;
  uint16  arc_wo_retry_cnt;
  uint16  arc_sys_info_cnt;

  uint16  arc_norm_rel_cnt;

  /* Counters for 3 cases of coding scheme change
  */
  uint16  arc_chcs1_cnt;
  uint16  arc_chcs2_cnt;
  uint16  arc_chcs3_cnt;

  uint16  arc_cres_cnt;

} grlc_diag_ul_abnormal_release_counts_t;


/* RLC-UL Event Counts
*/
typedef struct
{
  uint16  llc_event_cnt;
  uint16  mac_event_cnt;
  uint16  pl1_event_cnt;
  uint16  tmr_event_cnt;

} grlc_diag_ul_event_counts_t;

/* RLC-UL PDU Statistics */
typedef struct
{
  uint16  psn;
  uint16  len;
  uint8   pdu_prio;
  uint8   rlc_mode;
  uint8   peak_throughput;
  uint8   radio_priority;
  uint8   cause;
  uint8   pfc;
  uint8   pfi;
} grlc_diag_ul_pdu_t;

typedef struct
{
  uint8               cnt;
  uint8               index;
  grlc_diag_ul_pdu_t  buf[GRLC_DIAG_UL_PDU_BUF_MAX];
} grlc_diag_ul_pdu_stat_t;

/* RLC-UL Data Block Statistics */
typedef struct
{
  uint16  bsn;
  uint16  first_psn;
  uint16  last_psn;
} grlc_diag_ul_block_t;

typedef struct
{
  uint8                cnt;
  uint8                index;
  grlc_diag_ul_block_t buf[GRLC_DIAG_UL_BLOCK_BUF_MAX];
} grlc_diag_ul_block_stat_t;

/* RLC-UL Tx Statistics */
typedef struct
{
  uint8   ack_state;
  uint16  bsn;
  uint8   len;
  uint8   tbc;
  uint8   abs_bsn;
  uint8   cv;
  uint8   cs;
  uint8   cps;
  uint8   si;
} grlc_diag_ul_tx_t;

typedef struct
{
  uint8              cnt;
  uint8              index;
  grlc_diag_ul_tx_t  buf[GRLC_DIAG_UL_TX_BUF_MAX];
} grlc_diag_ul_tx_stat_t;


/*------------------------------------------------
** GRLC Downlink Diagnostic Structures
**------------------------------------------------
*/

/* RLC-DL Statistics
*/
typedef struct
{
  uint32  grlc_dl_state;

  uint32  ack_rx_pdu_cnt;
  uint32  ack_rx_oct_cnt;

  uint32  unack_rx_pdu_cnt;
  uint32  unack_pdu_filled_cnt;
  uint32  unack_rx_oct_cnt;
  uint32  unack_oct_filled_cnt;

  uint32  ack_nack_req_cnt;

  uint8   dl_seq_vq;
  uint8   dl_seq_vr;
  uint8   dl_seq_ssn;

  uint32  dl_gen_purpose_count_a;
  uint32  dl_gen_purpose_count_b;
  uint32  dl_gen_purpose_count_c;

} grlc_diag_dl_statistics_t;

/* RLC-DL PDU Statistics */
typedef struct
{
  uint32  pdu_num;
  uint16  len;
  uint16  first_bsn;
  uint16  last_bsn;
} grlc_diag_dl_pdu_t;

typedef struct
{
  uint8              cnt;
  uint8              index;
  grlc_diag_dl_pdu_t buf[GRLC_DIAG_DL_PDU_BUF_MAX];
} grlc_diag_dl_pdu_stat_t;

/* RLC-DL Rx Statistics */
typedef struct
{
  uint8   time_slot;
  uint8   crc;
  uint8   cs;
  uint16  bsn;
  uint8   len;
  uint8   vn;
  uint8   sp;
  uint8   fbi;
} grlc_diag_dl_rx_t;

typedef struct
{
  uint8              cnt;
  uint8              index;
  grlc_diag_dl_rx_t  buf[GRLC_DIAG_DL_RX_BUF_MAX];
} grlc_diag_dl_rx_stat_t;


/*-----------------------------------------------------------------------
**  Use of the general purpose variable:  -- dl_gen_purpose_count_a --
**  This variable is used to indicate DSM payload inconstancies
**  in  PL1=>RLC DL FIFO handling.
**-----------------------------------------------------------------------
*/


#if 0

/*-----------------------------------------------------------------------------
** Defines to help debug PL1's DSM problem
**-----------------------------------------------------------------------------
*/

/* The 2 LSBytes will be used to indicate DSM inconstancies in the first Payload.
** - The first 11 LSbits will indicate the BSN number.
** - the 12th bit represents the CRC status.
** - and the remaining 4 bits will indicate the error conditions described bellow.
**
**        2nd Payload    1st Payload
**        FF    FF    |   FF    FF
*/

/* Invalid length of 1st payload */
#define GRLC_INVAL_1ST_PLD_DSM_LEN       0x00001000

/* 1s payload with dsm reference count different than one */
#define GRLC_INVAL_1ST_DSM_REF_CNT       0x00002000

/* Error condition 0x00004000 and 0x00008000 reserved for future use */

/* Same as above but for 2nd payload
*/

/* Invalid length of 2nd payload */
#define GRLC_INVAL_2ND_PLD_DSM_LEN       0x10000000

/* 2nd payload with dsm reference count different than one */
#define GRLC_INVAL_2ND_DSM_REF_CNT       0x20000000

/* Error condition  0x40000000 and 0x80000000 reserved for future use */

#endif /* debug */

/*-----------------------------------------------------------------------
** UL Diagnostic Statistics info
**-----------------------------------------------------------------------
*/
extern grlc_diag_ul_statistics_t              ul_diag_stats_st[NUM_GERAN_DATA_SPACES];
extern grlc_diag_ul_abnormal_release_counts_t ul_diag_abn_rel_cnts_st[NUM_GERAN_DATA_SPACES];
extern grlc_diag_ul_event_counts_t            ul_diag_evt_cnts_st[NUM_GERAN_DATA_SPACES];

extern grlc_diag_ul_pdu_stat_t    ul_diag_pdu_stat[NUM_GERAN_DATA_SPACES];
extern grlc_diag_ul_block_stat_t  ul_diag_block_stat[NUM_GERAN_DATA_SPACES];
extern grlc_diag_ul_tx_stat_t     ul_diag_tx_stat[NUM_GERAN_DATA_SPACES];

/*-----------------------------------------------------------------------
** DL Diagnostic Statistics info
**-----------------------------------------------------------------------
*/
extern grlc_diag_dl_statistics_t  dl_diag_stats_st[NUM_GERAN_DATA_SPACES];

extern grlc_diag_dl_pdu_stat_t    dl_diag_pdu_stat[NUM_GERAN_DATA_SPACES];
extern grlc_diag_dl_rx_stat_t     dl_diag_rx_stat[NUM_GERAN_DATA_SPACES];

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



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to fill and return the Uplink Statistics Info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_statistics_t* grlc_diag_get_ul_statistics (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Statistics
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_statistics (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_ABNORMAL_RELEASE_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to fill and return Abnornal Release Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_abnormal_release_counts_t*
                            grlc_diag_get_ul_abnormal_release_counts ( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_ABNORMAL_RELEASE_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Abnormal Release Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_abnormal_release_counts (gas_id_t gas_id);


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_UL_EVENT_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to fill and return Uplink Event Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_ul_event_counts_t* grlc_diag_get_ul_event_counts ( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_UL_EVENT_COUNTS()
===
===  DESCRIPTION
===
===    RLC function to clear Uplink Event Counts
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_ul_event_counts (gas_id_t gas_id);

/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_GET_DL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to fill and return Downlink Statistics info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    pointer to data structure
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
const grlc_diag_dl_statistics_t* grlc_diag_get_dl_statistics ( gas_id_t gas_id );


/*===========================================================================
===
===  FUNCTION      GRLC_DIAG_CLR_DL_STATISTICS()
===
===  DESCRIPTION
===
===    RLC function to clear Downlink Statistics info
===
===  DEPENDENCIES
===
===
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===
===========================================================================*/
void grlc_diag_clr_dl_statistics ( gas_id_t gas_id );

#endif /* INC_GRLCDIAG_H */

/*** EOF: don't remove! ***/
