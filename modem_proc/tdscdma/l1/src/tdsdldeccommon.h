#ifndef TDSDLDEC_COMMON_H
#define TDSDLDEC_COMMON_H

/*============================================================================
                           D L D E C _ C O M M O N . H
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/dldec.h_v   1.33   11 Jul 2002 22:14:10   gurdeeps  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdsdldeccommon.h#1 $ $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
12/10/08    ks      Mainlining FEATURE_TDSCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
01/23/08    mc      Introduced in mainline from MBMS dev branch.
01/16/08    mc      Added support for MCCH weak indication
03/03/07    mc      Sync up for MBMS phase2A integration
12/12/06    mc      Synced up for MBMS demo - 3042 baseline
12/12/06    mc      MBMS related fixes on 3024 baseline
11/29/06    mc      Initial version for MBMS demo integration.
============================================================================*/
#include "mcaltds_dec.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define TDSDLDEC_TOTAL_CCTRCH_TABLES  MCALTDS_DEC_TOTAL_CCTRCH_TABLES
/* Total number of TrCh table in mDSP */
#define TDSDLDEC_TOTAL_TRCHT_PER_CCTRCH 8

#define TDSDLDEC_TRCH_TYPE_INVALID_CB_IDX    0xFF

/* Enum type declaration */
/* --------------------- */

/* Enumerations for TrCh callback types */
typedef enum
{
  TDSDLDEC_R99_BCCH_TRCH_CB_IDX,
  TDSDLDEC_MBMS_MCCH_TRCH_CB_IDX,
  TDSDLDEC_MBMS_MTCH_TRCH_CB_IDX,
  TDSDLDEC_MBMS_MSCH_TRCH_CB_IDX,
  TDSDLDEC_BMC_CTCH_TRCH_CB_IDX,
  TDSDLDEC_MAX_NUM_TRCH_TYPE_CB_IDX
}tdsdldec_trch_cb_idx_enum_type;


/* Type definitions for the TrCh callbacks */
/* --------------------------------------- */

typedef void TDSDLDEC_NO_BLK_RECE_CB_FUNC_TYPE(
  /* CCTrCh index on which TFCI 0 was received */
  uint8 cctrch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn);

typedef void TDSDLDEC_BLK_CRC_FAIL_CB_FUNC_TYPE(
  /* CCTrCh index on which MCCH failed block received */
  uint8 cctrch_idx,
  /* TrCh index on which MCCH block */
  uint8 trch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn);

typedef boolean TDSDLDEC_QUERY_BLK_WANTED_CB_FUNC_TYPE(
  /* CCTrCh index on which MCCH failed block received */
  uint8 cctrch_idx,
  /* TrCh index on which MCCH block */
  uint8 trch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn,
  uint32 crc_bits);

typedef void TDSDLDEC_BLK_CRC_STATS_CB_FUNC_TYPE(
  /* CCTrCh index on which block received */
  uint8 cctrch_idx,
  /* TrCh index on which block received */
  uint8 trch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn,
  /* Number of errored blocks */
  uint8 num_err_blk,
  /* Total number of blocks */
  uint8 num_total_blk);

typedef void TDSDLDEC_BLK_UPDATE_HANDLER_CB_FUNC_TYPE(
  /* CCTrCh index on which MCCH failed block received */
  uint8 cctrch_idx,
  /* TrCh index on which MCCH block */
  uint8 trch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn,
  /* pointer to the first DSM item containing the TB */
  dsm_item_type *dsm_ptr);

typedef void TDSDLDEC_TRCH_STATE_UPDATE_CB_FUNC_TYPE(
  /* CCTrCh index on which TrCh state got updated */
  uint8 cctrch_idx,
  /* TrCh index on which state got updated  */
  uint8 trch_idx);

/* Structure type declaration */
/* -------------------------- */

/* Structure containing callback functions for transport channels */
typedef struct
{
  TDSDLDEC_NO_BLK_RECE_CB_FUNC_TYPE*        no_block_rece_ind;
  TDSDLDEC_BLK_CRC_FAIL_CB_FUNC_TYPE*       block_crc_failed_ind;
  TDSDLDEC_QUERY_BLK_WANTED_CB_FUNC_TYPE*   query_block_wanted_cb;
  TDSDLDEC_BLK_CRC_STATS_CB_FUNC_TYPE*      crc_stats_updater;
  TDSDLDEC_BLK_UPDATE_HANDLER_CB_FUNC_TYPE* block_update_handler;
  TDSDLDEC_TRCH_STATE_UPDATE_CB_FUNC_TYPE*  trch_state_update_cb;
}tdsdldec_decoded_block_svc_cb_func_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION tdsl1dec_register_decoded_block_service_cb

DESCRIPTION
  This function registers the callbacks for the transport channels.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_register_decoded_block_service_cb(
  /* CCTrCh index */
  uint8 cctrch_idx,
  /* TrCh index for which the cb is registered */
  uint8 trch_idx,
  /* Type of callback */
  uint8 cb_type_idx,
  /* pointer to structure that holds various callbacks */
  tdsdldec_decoded_block_svc_cb_func_struct_type* cb_ptr);

/*===========================================================================
FUNCTION tdsl1dec_deregister_decoded_block_service_cb

DESCRIPTION
  This function deregisters the callbacks for the transport channels.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void tdsl1dec_deregister_decoded_block_service_cb(
  /* CCTrCh index */
  uint8 cctrch_idx,
  /* TrCh index for which the cb is registered */
  uint8 trch_idx,
  /* Type of callback */
  uint8 cb_type_idx);

#endif /* DLDEC_COMMON_H */
