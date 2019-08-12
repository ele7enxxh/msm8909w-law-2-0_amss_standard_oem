#ifndef DLPICHMGR_I_H
#define DLPICHMGR_I_H

/*===========================================================================
                               D L P I C H M G R I. H

GENERAL DESCRIPTION

  This file contains the declarations and definitions that are internal to
  dlpichmgr.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright(c)2007 - 2008 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/tlm.c_v   1.153   17 Jul 2002 16:36:16   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlpichmgri.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08    hk      Bringing in 7k Mailine fixes
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/15/08    vp      Added code comments and Removed static db
11/06/07    vp      Moved DRX handle from dlchmgr to channel specific managers
                    and added shared channel support in sccpchmgr.
08/20/07    vsr     Renamed dl_phychan_copy_pich_misc_info() to dl_pich_copy_misc_info()
08/20/07    vp      Added support for new sccpch0/pich reponse functions.
02/21/07    vp      Initial version
                    ===========================================================================*/
#include "wcdma_variation.h"
#include "customer.h"
#include "comdef.h"
#include "l1const.h"
#include "dlchmgr.h"

#ifndef FEATURE_NCP
extern uint16 pich_offset_is_odd;
#endif

#define DL_PICH_INVALID_DYN_DB_IDX (DLCHMGR_INVALID_CH_SPEC_IDX)

/*Macros to help in maintaining the status of completion of SCCPCH0 and 
PICH operations.*/
#define DL_PICH_NO_CHANNEL_OP_DONE (0x0)
#define DL_PICH_PICH_CHANNEL_OP_DONE (0x1)
#define DL_PICH_SCCPCH_CHANNEL_OP_DONE (0x2)
#define DL_PICH_ALL_CHANNEL_OP_DONE (DL_PICH_PICH_CHANNEL_OP_DONE | DL_PICH_SCCPCH_CHANNEL_OP_DONE)

#define DL_PICH_DEFAULT_CFG_DB_IDX (0)

#define DL_PICH_GET_PHCH_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)    \
  (DLCHMGR_GET_PHCH_HDLE_FROM_CFG_DB_IDX(DL_PICH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_PICH_IS_CFG_DB_IDX_NOT_IN_USE(cfg_db_idx)    \
  (dl_pich_cfg_db[cfg_db_idx].in_use == FALSE )

#define DL_PICH_IS_CFG_DB_IDX_IN_USE(cfg_db_idx)    \
  (dl_pich_cfg_db[cfg_db_idx].in_use == TRUE )

#define DL_PICH_IS_CFG_DB_IDX_INVALID(cfg_db_idx)    \
  (cfg_db_idx >= DL_PICH_CFG_DB_SIZE)

#define DL_PICH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)    \
    (dl_pich_cfg_db[cfg_db_idx].chmgr_cfg_db_idx)

#define DL_PICH_UPD_PICH_SCCPCH_STATUS_TO_NO_OP_DONE(cfg_db_idx)  \
  (dl_pich_cfg_db[cfg_db_idx].pich_sccpch_op_status = DL_PICH_NO_CHANNEL_OP_DONE)

#define DL_PICH_UPD_PICH_SCCPCH_STATUS_TO_PICH_DONE(cfg_db_idx)  \
  (dl_pich_cfg_db[cfg_db_idx].pich_sccpch_op_status |= DL_PICH_PICH_CHANNEL_OP_DONE)

#define DL_PICH_UPD_PICH_SCCPCH_STATUS_TO_SCCPCH_DONE(cfg_db_idx)  \
  (dl_pich_cfg_db[cfg_db_idx].pich_sccpch_op_status |= DL_PICH_SCCPCH_CHANNEL_OP_DONE)

#define DL_PICH_IS_BOTH_PICH_SCCPCH_OP_DONE(cfg_db_idx)  \
  ((dl_pich_cfg_db[cfg_db_idx].pich_sccpch_op_status & DL_PICH_ALL_CHANNEL_OP_DONE) == DL_PICH_ALL_CHANNEL_OP_DONE)

#define DL_PICH_GET_DRX_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_pich_cfg_db[cfg_db_idx].drx_handle)

#define DL_PICH_IS_DRX_HDLE_INVALID_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (DL_PICH_GET_DRX_HDLE_FROM_CFG_DB_IDX(cfg_db_idx) == DRX_ENTITY_HANDLE_INVALID)

#define DL_PICH_GET_FREQ(cfg_db_idx)    \
    (DLCHMGR_GET_FREQ_FROM_CFG_DB_IDX(DL_PICH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_PICH_GET_PSC(cfg_db_idx)    \
    (DLCHMGR_GET_PSC_FROM_CFG_DB_IDX(DL_PICH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

/*===========================================================================
FUNCTION        dl_pich_copy_misc_info

DESCRIPTION     This function copies pich information in to the DRX data structures.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_pich_copy_misc_info(
  /* pointer to the dl_phychan_db part of ordered config */
  l1_dl_phychan_db_struct_type *phychan_db, 

/* sccpch_cctrch_idx */
  uint8 sccpch_cctrch_idx
);
#endif

