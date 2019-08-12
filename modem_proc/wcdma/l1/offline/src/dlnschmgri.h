#ifndef DLNSCHMGR_I_H
#define DLNSCHMGR_I_H

/*===========================================================================
                               D L N S C H M G R I. H

GENERAL DESCRIPTION

  This file contains the declarations and definitions that are internal to
  dlnschmgr.

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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlnschmgri.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     ---------------------------------------------------------
02/11/13    gsk/sr  EDRX Full Sleep initial check in.
07/24/12    geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/08/12    gv      Feature HS RACH Initial code check in
08/11/10    hk      Changes to wait for the DSD response before proceeding
                    with AICH setup
09/24/09    hk      Porting of AICH resetup feature to Offline
12/10/08    ks      Mainlining FEATURE_WCDMA_DL_ENHANCED
01/25/08    vsr     Checking in Enhanced DL files onto mainline
01/15/08    vp      Added code comments and Removed static db
11/06/07    vp      Moved DRX handle from dlchmgr to channel specific managers
                    and added shared channel support in sccpchmgr.
08/20/07    vsr     Moved in variables drom dldem files
02/21/07    vp      Initial version
                    
                    ===========================================================================*/
#include "customer.h"
#include "comdef.h"
#include "l1const.h"
#include "dlchmgr.h"

#define DL_NSCH_IS_CFG_DB_IDX_NOT_IN_USE(cfg_db_idx)    \
  (dl_nsch_cfg_db[cfg_db_idx].in_use == FALSE )

#define DL_NSCH_IS_CFG_DB_IDX_IN_USE(cfg_db_idx)    \
  (dl_nsch_cfg_db[cfg_db_idx].in_use == TRUE )

#define DL_NSCH_GET_STATIC_DB_IDX(cfg_db_idx)    \
  (dl_nsch_cfg_db[cfg_db_idx].static_db_idx)

#define DL_NSCH_GET_FREQ(cfg_db_idx)    \
    (DLCHMGR_GET_FREQ_FROM_CFG_DB_IDX(DL_NSCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_NSCH_GET_PSC(cfg_db_idx)    \
    (DLCHMGR_GET_PSC_FROM_CFG_DB_IDX(DL_NSCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_NSCH_IS_CFG_DB_IDX_INVALID(cfg_db_idx)    \
  (cfg_db_idx >= DL_NSCH_CFG_DB_SIZE)

#define DL_NSCH_IS_CCTRCH_IDX_INVALID(cctrch_idx)    \
  (DLCHMGR_IS_CCTRCH_IDX_INVALID(cctrch_idx))

#define DL_NSCH_INVALID_DYN_DB_IDX (DLCHMGR_INVALID_CH_SPEC_IDX)

#define DL_NSCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)    \
    (dl_nsch_cfg_db[cfg_db_idx].chmgr_cfg_db_idx)

#define DL_NSCH_GET_CCTRCH_IDX_FROM_DB_IDX(cfg_db_idx)    \
    (DLCHMGR_GET_CCTRCH_IDX_FROM_CFG_DB_IDX(DL_NSCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_NSCH_GET_PHCH_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (DLCHMGR_GET_PHCH_HDLE_FROM_CFG_DB_IDX(DL_NSCH_GET_CHMGR_CFG_DB_IDX(cfg_db_idx)))

#define DL_NSCH_IS_STATIC_DB_IDX_VALID_FOR_AICH(static_db_idx)  \
  ((static_db_idx == DL_NSCH_PHYCHAN_AICH) || (static_db_idx == DL_NSCH_PHYCHAN_HS_RACH_AICH))

#define DL_NSCH_GET_DRX_HDLE_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (dl_nsch_cfg_db[cfg_db_idx].drx_handle)

#define DL_NSCH_IS_DRX_HDLE_INVALID_FROM_CFG_DB_IDX(cfg_db_idx)    \
    (DL_NSCH_GET_DRX_HDLE_FROM_CFG_DB_IDX(cfg_db_idx) == DRX_ENTITY_HANDLE_INVALID)

#define DL_AICH_THRESH_TABLE_SIZE 28

extern const uint16 dl_aich_thresh_tab[DL_AICH_THRESH_TABLE_SIZE];

extern uint16 dl_aich_ack_thresh_adj;
extern uint16 dl_aich_nak_thresh_adj;

/* Macro for looking up the value to program to firware to set the AICH
   detection threshold.  The value received from UTRAN is in units of 1 dB
   from [-22..5].  The table is indexed from 0 to 27 and this macro returns
   the appropriate table value given the AICH power offset in those units. */
#define AICH_ACK_THRESH_LOOKUP(aich_pwr_offset) \
  (((dl_aich_thresh_tab[((aich_pwr_offset)) - AICH_MIN_PWR_OFFSET]) \
    *dl_aich_ack_thresh_adj)/65535)

#define AICH_NAK_THRESH_LOOKUP(aich_pwr_offset) \
  (((dl_aich_thresh_tab[((aich_pwr_offset)) - AICH_MIN_PWR_OFFSET]) \
    *dl_aich_nak_thresh_adj)/65535)

/*--------------------------------------------------------------------------
                  STRUCTURE: AICH_MISC_PARMS_STRUCT_TYPE

This structure contains the miscellaneous parameters for the AICH.
--------------------------------------------------------------------------*/
typedef struct {
  /* the power offset relative to the CPICH */
  int8 pwr_offset;

  /* the transmit time offset */
  l1_dl_aich_tt_enum_type tt;

} aich_misc_parms_struct_type;

/*===========================================================================
FUNCTION        dl_nsch_aich_copy_misc_info

DESCRIPTION     This function copies aich information(power offset and trans 
                timing values) in to local db so that they can be provided to UL when needed.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_nsch_aich_copy_misc_info(l1_aich_parms_struct_type *aich_db);

/*===========================================================================
FUNCTION        dl_nsch_write_aich_info_to_mdsp

DESCRIPTION     This function writes aich information to mdsp.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
===========================================================================*/
LOCAL void dl_nsch_write_aich_info_to_mdsp(void);

/*===========================================================================
FUNCTION        dl_nsch_resume_ch_setup_after_dsd

DESCRIPTION     This function handles the AICH resetup case on every
                DSD response which was done on CFN boundary.
                Based on the refernce finger lock status and timer expiry, the
                function either starts AICH add or drop.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None.
===========================================================================*/
LOCAL void dl_nsch_resume_ch_setup_after_dsd(void);

#ifdef FEATURE_WCDMA_HS_FACH_DRX
/*===========================================================================
FUNCTION        dl_nsch_get_cfg_db_idx

DESCRIPTION     This function returns the matching cfg db idx corresponding to
                channel manager cfg db idx

DEPENDENCIES    None

RETURN VALUE
                cfg-db-idx: The index in nsch manager cfg db.

SIDE EFFECTS    None.
===========================================================================*/
LOCAL dl_ch_spec_cfg_db_idx_type dl_nsch_get_cfg_db_idx(dlchmgr_cfg_db_idx_type chmgr_cfg_db_idx);
#endif /* FEATURE_WCDMA_HS_FACH_DRX */

#endif /* DLNSCHMGR_I_H */

