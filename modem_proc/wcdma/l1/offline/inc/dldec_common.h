#ifndef DLDEC_COMMON_H
#define DLDEC_COMMON_H

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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/dldec_common.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
06/14/13    ms      WL1 inform BMC once CTCH CRC error happens
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/11/12    vs      MBMS feature cleanup.
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
01/23/08    mc      Introduced in mainline from MBMS dev branch.
01/16/08    mc      Added support for MCCH weak indication
03/03/07    mc      Sync up for MBMS phase2A integration
12/12/06    mc      Synced up for MBMS demo - 3042 baseline
12/12/06    mc      MBMS related fixes on 3024 baseline
11/29/06    mc      Initial version for MBMS demo integration.
============================================================================*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

#define DEC_TOTAL_CCTRCH_TABLES  MCALWCDMA_DEC_TOTAL_CCTRCH_TABLES
/* Total number of TrCh table in mDSP */
#include "wcdma_variation.h"
#define DEC_TOTAL_TRCHT_PER_CCTRCH 8

#define DEC_TRCH_TYPE_INVALID_CB_IDX    0xFF

/* Enum type declaration */
/* --------------------- */

/* Enumerations for TrCh callback types */
typedef enum
{
  DEC_R99_BCCH_TRCH_CB_IDX,
  DEC_MBMS_MCCH_TRCH_CB_IDX,
  DEC_MBMS_MTCH_TRCH_CB_IDX,
  DEC_MBMS_MSCH_TRCH_CB_IDX,
  DEC_BMC_CTCH_TRCH_CB_IDX,
  DEC_MAX_NUM_TRCH_TYPE_CB_IDX
}dec_trch_cb_idx_enum_type;


/* Type definitions for the TrCh callbacks */
/* --------------------------------------- */

typedef void DL_NO_BLK_RECE_CB_FUNC_TYPE(
  /* CCTrCh index on which TFCI 0 was received */
  uint8 cctrch_idx,
  uint8 trch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn);

typedef void DL_BLK_CRC_FAIL_CB_FUNC_TYPE(
  /* CCTrCh index on which MCCH failed block received */
  uint8 cctrch_idx,
  /* TrCh index on which MCCH block */
  uint8 trch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn);

typedef boolean DL_QUERY_BLK_WANTED_CB_FUNC_TYPE(
  /* CCTrCh index on which MCCH failed block received */
  uint8 cctrch_idx,
  /* TrCh index on which MCCH block */
  uint8 trch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn,
  uint32 crc_bits);

typedef void DL_BLK_CRC_STATS_CB_FUNC_TYPE(
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

typedef void DL_BLK_UPDATE_HANDLER_CB_FUNC_TYPE(
  /* CCTrCh index on which MCCH failed block received */
  uint8 cctrch_idx,
  /* TrCh index on which MCCH block */
  uint8 trch_idx,
  /* CFN of its first frame of TTI */
  uint8 rece_cfn,
  /* pointer to the first DSM item containing the TB */
  dsm_item_type *dsm_ptr);

typedef void DL_TRCH_STATE_UPDATE_CB_FUNC_TYPE(
  /* CCTrCh index on which TrCh state got updated */
  uint8 cctrch_idx,
  /* TrCh index on which state got updated  */
  uint8 trch_idx);

/* Structure type declaration */
/* -------------------------- */

/* Structure containing callback functions for transport channels */
typedef struct
{
  DL_NO_BLK_RECE_CB_FUNC_TYPE*        no_block_rece_ind;
  DL_BLK_CRC_FAIL_CB_FUNC_TYPE*       block_crc_failed_ind;
  DL_QUERY_BLK_WANTED_CB_FUNC_TYPE*   query_block_wanted_cb;
  DL_BLK_CRC_STATS_CB_FUNC_TYPE*      crc_stats_updater;
  DL_BLK_UPDATE_HANDLER_CB_FUNC_TYPE* block_update_handler;
  DL_TRCH_STATE_UPDATE_CB_FUNC_TYPE*  trch_state_update_cb;
}dl_dec_decoded_block_svc_cb_func_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION dl_dec_register_decoded_block_service_cb

DESCRIPTION
  This function registers the callbacks for the transport channels.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_dec_register_decoded_block_service_cb(
  /* CCTrCh index */
  uint8 cctrch_idx,
  /* TrCh index for which the cb is registered */
  uint8 trch_idx,
  /* Type of callback */
  uint8 cb_type_idx,
  /* pointer to structure that holds various callbacks */
  dl_dec_decoded_block_svc_cb_func_struct_type* cb_ptr);

/*===========================================================================
FUNCTION dl_dec_deregister_decoded_block_service_cb

DESCRIPTION
  This function deregisters the callbacks for the transport channels.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dl_dec_deregister_decoded_block_service_cb(
  /* CCTrCh index */
  uint8 cctrch_idx,
  /* TrCh index for which the cb is registered */
  uint8 trch_idx,
  /* Type of callback */
  uint8 cb_type_idx);

#endif /* DLDEC_COMMON_H */
