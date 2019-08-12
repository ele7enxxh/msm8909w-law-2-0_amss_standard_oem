#ifndef WL1ULCHMGR_H
#define WL1ULCHMGR_H



/*===========================================================================
                 WCDMA L1 HS RACH Controller

GENERAL DESCRIPTION
  This file contains the code for handling the HS RACH procedures and interacting
  with the other modules within Layer 1. This module also acts as an interface
  with the external modules to WCDMA L1 in relation to HS RACH feature.

EXTERNALIZED FUNCTIONS

INTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1ulchmgr.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when       who    what, where, why
--------   ---   ------------------------------
09/11/12   vr     EDCH release procedure should be triggered only after the F-DPCH 
                  event handler is processed (HSRACH)
09/05/12   vr     Moved logging and edrx_hs_notify to task context as HSRACH done
                  ISR was getting severely delayed.
07/06/12   vr     HSRACH fixes
06/19/12   vr     HSRACH code update
06/04/12   sp     Feature HS RACH Initial code check in -- File Creation
*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_WCDMA_HS_RACH

#include "l1def.h"
#include "l1sapcommon.h"
#include "l1ulcfg.h"
#include "l1rrcif.h"
#include "l1macif.h"
#include "l1task_v.h"
#ifdef FEATURE_L1_LOGGING_ENABLED
#include "ullog.h"
#endif

#include "wenc.h"
#include "ulcmd.h"
#include "l1sapcommon.h"
#include "ulstates.h"
#include "wenci.h"


typedef l1_ul_phychan_enum_type wl1_ul_phychan_enum_type;
typedef l1_ul_phychan_cfg_struct_type wl1_ul_phychan_cfg_struct_type;

/* structure used to pass the config */
typedef struct{
 wl1_ul_phychan_enum_type ul_phch;
 wl1_ul_phychan_cfg_struct_type *wl1_ul_phychan_cfg_db;
}wl1_ulchmgr_chan_spec_cfg_struct_type;


/* Database where ULMGR saves the Config */
typedef union
{
  /* uplink PRACH configuration */
    l1_ul_prach_cfg_struct_type prach_cfg;
    /* uplink DPCH configuration */
    l1_ul_dpch_cfg_struct_type  dpch_cfg;
}wl1_ulchmgr_cfg_db_union_type;

typedef struct
{
   boolean (*validate)(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);
   boolean (*add)(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);
   boolean (*recfg)(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);
   boolean (*drop)(void);
   boolean (*suspend)(void);
   boolean (*resume)(void);
}wl1_ulchmgr_processor_type;


extern wl1_ul_phychan_enum_type wl1ulphychan_active;

extern int8 wl1ulphychan_max_tx_pwr;

/* Timer to trigger new rach */
//extern timer_type rach_trigger_timer;

/* Database where ULMGR saves the Config */
extern wl1_ulchmgr_cfg_db_union_type wl1_ulchmgr_cfg_db[L1_UL_NUM_PHYCHAN];
extern uint8 wl1_ulchmgr_cfg_db_idx[L1_UL_NUM_PHYCHAN];


#define WL1ULCHMGR_SET_MAX_TX_PWR(x) (wl1ulphychan_max_tx_pwr = x)
#define WL1ULCHMGR_GET_MAX_TX_PWR() (wl1ulphychan_max_tx_pwr)

/* For all get params, channel type is needed */
#define WL1ULCHMGR_GET_PCA(x)           (wl1_ulchmgr_cfg_db[x].dpch_cfg.dpch_pwr_ctrl_info.pca)
#define WL1ULCHMGR_GET_TPC_STEP_SIZE(x) (wl1_ulchmgr_cfg_db[x].dpch_cfg.dpch_pwr_ctrl_info.tpc_step_size)
#define WL1ULCHMGR_GET_NUM_DPDCH(x)     (wl1_ulchmgr_cfg_db[x].dpch_cfg.num_dpdch)
#define WL1ULCHMGR_GET_NUMTPC(x)        (wl1_ulchmgr_cfg_db[x].dpch_cfg.numtpc)
#define WL1ULCHMGR_GET_FBI_LEN(x)       (wl1_ulchmgr_cfg_db[x].dpch_cfg.fbi_len)
#define WL1ULCHMGR_GET_TFCI_PRESENT(x)  (wl1_ulchmgr_cfg_db[x].dpch_cfg.tfci_present)


#define WL1ULCHMGR_SET_SCRAMBLING_CODE_TYPE(x, y) (wl1_ulchmgr_cfg_db[x].dpch_cfg.scrambling_code_type = y)
#define WL1ULCHMGR_SET_SCRAMBLING_CODE(x, y) (wl1_ulchmgr_cfg_db[x].dpch_cfg.scrambling_code = y)

#define WL1ULCHMGR_GET_AVAIL_SIGMASK(x) (wl1_ulchmgr_cfg_db[x].prach_cfg.avail_sig_mask)
#define WL1ULCHMGR_GET_AVAIL_SUBCHANMASK(x) (wl1_ulchmgr_cfg_db[x].prach_cfg.avail_sub_chan_mask)
#define WL1ULCHMGR_GET_PWROFFSET_P0(x)  (wl1_ulchmgr_cfg_db[x].prach_cfg.pwr_offset_P0)
#define WL1ULCHMGR_GET_PWROFFSET_PPE(x) (wl1_ulchmgr_cfg_db[x].prach_cfg.hs_rach_info.powerOffsetPp_e)
#define WL1ULCHMGR_GET_EAI_INDICATION(x) (wl1_ulchmgr_cfg_db[x].prach_cfg.hs_rach_info.e_ai_indication)
#define WL1ULCHMGR_GET_MAX_PREAMBLES(x) (wl1_ulchmgr_cfg_db[x].prach_cfg.max_preamble_tx)



/* for all set params, channel type and value is needed */
#define WL1ULCHMGR_SET_TPC_STEP_SIZE(x,y) (wl1_ulchmgr_cfg_db[x].dpch_cfg.dpch_pwr_ctrl_info.tpc_step_size = y)

#define WL1ULCHMGR_SET_UL_STATE(x) (ul_state = x)
#define WL1ULCHMGR_GET_UL_STATE() (ul_state)

#define WL1ULCHMGR_SET_UL_DPCH_STATE(x) (ul_dpch_state = x)
#define WL1ULCHMGR_SET_DPCCH_INIT_PWR_DBM(x) (dpch_init_tx_pwr_dbm = x)
#define WL1ULCHMGR_SET_DPCCH_INIT_PWR_AGC(x) (dpch_init_tx_pwr_agc = x)

#define WL1ULCHMGR_SET_TX_AGC_LOGGING(x) (tx_agc_logging = x)
#define WL1ULCHMGR_GET_TX_AGC_LOGGING() (tx_agc_logging)
#define WL1ULCHMGR_SET_LAST_AISUM_LOGGING(x) (last_aiSum_logging = x)
#define WL1ULCHMGR_GET_LAST_AISUM_LOGGING() (last_aiSum_logging)

#define WL1ULCHMGR_SET_UL_PHYCHAN_ACTIVE(x) (wl1ulphychan_active = x)
#define WL1ULCHMGR_GET_UL_PHYCHAN_ACTIVE() (wl1ulphychan_active)


#define WL1ULCHMGR_CFG_DB_INVALID_IDX 0xFF
#define RACH_TRIGGER_MSEC  100

/* ::: FUNCTION PROTOTYPES ::: */

/*===========================================================================
FUNCTION        WL1ULCHMGR_ALLOCATE_DB

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
void wl1ulchmgr_allocate_db(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);

/*===========================================================================
FUNCTION    WL1ULHSPRACHMGR_PROCESS_AICH

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsprachmgr_process_aich(void);

/*===========================================================================
FUNCTION     WL1ULHSPRACHMGR_VALIDATE_CFG

DESCRIPTION

PARAMETERS

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern boolean wl1ulhsprachmgr_validate_cfg(l1_ul_prach_cfg_struct_type *prach_cfg_ptr);

/*===========================================================================
FUNCTION        WL1ULCHMGR_DEALLOCATE_DB

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

extern void wl1ulchmgr_deallocate_db(wl1_ul_phychan_enum_type ul_phch);

/*===========================================================================
FUNCTION        WL1ULCHMGR_INIT

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/
extern void wl1ulchmgr_init(void);

/*===========================================================================
FUNCTION     WL1ULHSPRACHMGR_CLEANUP

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1ulhsprachmgr_cleanup(void);

/*===========================================================================
FUNCTION     WL1ULHSPRACHMGR_VALIDATE

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsprachmgr_validate(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);

/*===========================================================================
FUNCTION     WL1ULHSPRACHMGR_ADD

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsprachmgr_add(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);

/*===========================================================================
FUNCTION     WL1ULHSPRACHMGR_RECFG

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsprachmgr_recfg(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);

/*===========================================================================
FUNCTION     WL1ULHSPRACHMGR_DROP

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsprachmgr_drop(void);

/*===========================================================================
FUNCTION     WL1ULHSPRACHMGR_SUSPEND

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsprachmgr_suspend(void);

/*===========================================================================
FUNCTION     WL1ULHSPRACHMGR_RESUME

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsprachmgr_resume(void);

/*===========================================================================
FUNCTION     WL1ULHSDPCHMGR_VALIDATE

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsdpchmgr_validate(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);

/*===========================================================================
FUNCTION     WL1ULHSDPCHMGR_ADD

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsdpchmgr_add(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);

/*===========================================================================
FUNCTION     WL1ULHSDPCHMGR_RECFG

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsdpchmgr_recfg(wl1_ulchmgr_chan_spec_cfg_struct_type *wl1_ul_phychan_cfg_cmd);

/*===========================================================================
FUNCTION     WL1ULHSDPCHMGR_DROP

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsdpchmgr_drop(void);

/*===========================================================================
FUNCTION     WL1ULHSDPCHMGR_SUSPEND

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsdpchmgr_suspend(void);

/*===========================================================================
FUNCTION     WL1ULHSDPCHMGR_RESUME

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsdpchmgr_resume(void);

/*===========================================================================
FUNCTION     WL1ULHSDPCHMGR_ACTIVATE

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulhsdpchmgr_activate(void);

/*===========================================================================
FUNCTION     WL1ULHSDPCHMGR_DEACTIVATE

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1ulhsdpchmgr_deactivate(void);


/*===========================================================================
FUNCTION     WL1ULCHMGR_EXT_ADD_VALIDATE

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulchmgr_ext_add_validate(wl1_ul_phychan_enum_type wl1_ul_phychan,
                                            l1_ul_phychan_cfg_struct_type *ul_phychan_db);

/*===========================================================================
FUNCTION     WL1ULCHMGR_EXT_ADD

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern boolean wl1ulchmgr_ext_add(wl1_ul_phychan_enum_type wl1_ul_phychan,
                                            l1_ul_phychan_cfg_struct_type *ul_phychan_db);

/*===========================================================================
FUNCTION     WL1ULCHMGR_EXT_DROP

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1ulchmgr_ext_drop(wl1_ul_phychan_enum_type wl1_ul_phychan);

/*===========================================================================
FUNCTION     WL1ULCHMGR_PHCH_SUSPEND

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1ulchmgr_phch_suspend(wl1_ul_phychan_enum_type wl1_ul_phychan);

/*===========================================================================
FUNCTION     WL1ULCHMGR_PHCH_RESUME

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1ulchmgr_phch_resume(wl1_ul_phychan_enum_type wl1_ul_phychan);

/*===========================================================================
FUNCTION     WL1ULCHMGR_VALIDATE_PWR

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1ulchmgr_validate_pwr(int8 tx_pwr);

/*===========================================================================
FUNCTION     WL1ULCHMGR_ACCESS_DB

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern wl1_ulchmgr_cfg_db_union_type wl1ulchmgr_access_db(wl1_ul_phychan_enum_type ul_phch);

/*===========================================================================
FUNCTION     WL1ULCHMGR_MAC_RACH_TRIGGER_IND

DESCRIPTION

PARAMETERS

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void wl1ulchmgr_mac_rach_trigger_ind(timer_cb_data_type data);

#endif /* FEATURE_WCDMA_HS_RACH */
#endif
