#ifndef DLSHMGR_H
#define DLSHMGR_H

/*==========================================================================
             WCDMA L1 Downlink shared resource manager

DESCRIPTION
  This file contains definition, declaration required for WCDMA L1 Downlink
  shared resource manager.

  Copyright (c) 2006 - 2008 by Qualcomm Technologies, Inc.
  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/dlshmgr.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when         who    what, where, why
----------   ---    ----------------------------------------------------------
07/24/12     geg    Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
06/11/12     vs     MBMS feature cleanup.
12/10/08     ks     Mainlining FEATURE_WCDMA_DL_ENHANCED
10/03/08     hk     Bringing in 7k Mailine fixes
12/19/07     MC     Wrap MBMS specific services under MBMS feature
11/30/07     MC     Removed the phch drop mechanism. The channel manager
                    would call the DL drop directly.
11/06/07     vp     Added mapping from trch to service and service to trch
                    within every cctrch-idx. Added a API to get services
                    enabled within each cctrch.
05/10/07     vsr    Lint Fixes
03/03/07     mc     Sync up for MBMS phase2A integration
01/05/07     MC     Synced up for MBMS Phase1 IOT
12/12/06     mc     Synced up for MBMS demo - 3042 baseline
11/30/06     MC     Changed name of an include file.
10/30/2006   GS     Initial rev of DL shared manager
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"

#include "dlphch.h"
#include "dldec.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* indicate unused physical channel handle */
#define DLSHMGR_PHCH_HANDLE_UNUSED 0xFF

#define DLSHMGR_SVC_TO_TRCH_MAP_NONE 0

#define DLSHMGR_SVC_MASK_NONE 0

/* Convert service type to bit mask */
#define DLSHMGR_SVC_BMSK(svc_type) (1 << (svc_type))

/* Convert Trch Idx to bit mask */
#define DLSHMGR_TRCH_BMSK(trch_idx) (1 << (trch_idx))

/* Set the service bit in the transport channel bitmask */
#define DLSHMGR_SET_TRCH_TO_L1_ENAB_SVC_BMSK(cctrch_idx, trch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].trch_to_l1_enab_svc_mask[trch_idx] |= \
    DLSHMGR_SVC_BMSK(svc_type)

/* Clear the service bit in the transport channel bitmask */
#define DLSHMGR_CLEAR_TRCH_TO_L1_ENAB_SVC_BMSK(cctrch_idx, trch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].trch_to_l1_enab_svc_mask[trch_idx] &= \
    (~(uint32)(DLSHMGR_SVC_BMSK(svc_type)))

/* Get the service bit in the transport channel bitmask */
#define DLSHMGR_GET_TRCH_TO_L1_ENAB_SVC_BMSK(cctrch_idx, trch_idx, svc_type) \
    ((dlshmgr_cctrch_shared_svc_info[cctrch_idx].trch_to_l1_enab_svc_mask[trch_idx] & \
      DLSHMGR_SVC_BMSK(svc_type)) >> svc_type)

typedef uint16 dlshmgr_svc_to_trch_map_mask_type;

typedef uint16 dlshmgr_svc_mask_type;

/* Set the service bit in the transport channel bitmask */
#define DLSHMGR_SET_L1_ENAB_SVC_TO_TRCH_BMSK(cctrch_idx, trch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].l1_enab_svc_to_trch_map[svc_type] |= \
    DLSHMGR_TRCH_BMSK(trch_idx)

/* Clear the service bit in the transport channel bitmask */
#define DLSHMGR_CLEAR_L1_ENAB_SVC_TO_TRCH_BMSK(cctrch_idx, trch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].l1_enab_svc_to_trch_map[svc_type] &= \
    (~(dlshmgr_svc_to_trch_map_mask_type)(DLSHMGR_TRCH_BMSK(trch_idx)))

/* Set the service bit in the transport channel bitmask */
#define DLSHMGR_SET_CCTRCH_TO_L1_ENAB_SVC_BMSK(cctrch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].cctrch_to_l1_enab_svc_map |= \
    DLSHMGR_SVC_BMSK(svc_type)

/* Clear the service bit in the transport channel bitmask */
#define DLSHMGR_CLEAR_CCTRCH_TO_L1_ENAB_SVC_BMSK(cctrch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].cctrch_to_l1_enab_svc_map &= \
    (~(dlshmgr_svc_mask_type)(DLSHMGR_SVC_BMSK(svc_type)))


/* Set the service bit in the transport channel bitmask */
#define DLSHMGR_SET_RRC_CFG_SVC_TO_TRCH_BMSK(cctrch_idx, trch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].rrc_cfg_svc_to_trch_map[svc_type] |= \
    DLSHMGR_TRCH_BMSK(trch_idx)

/* Clear the service bit in the transport channel bitmask */
#define DLSHMGR_CLEAR_RRC_CFG_SVC_TO_TRCH_BMSK(cctrch_idx, trch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].rrc_cfg_svc_to_trch_map[svc_type] &= \
    (~(dlshmgr_svc_to_trch_map_mask_type)(DLSHMGR_TRCH_BMSK(trch_idx)))

/* Set the service bit in the transport channel bitmask */
#define DLSHMGR_SET_CCTRCH_TO_RRC_CFG_SVC_BMSK(cctrch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].cctrch_to_rrc_cfg_svc_map |= \
    DLSHMGR_SVC_BMSK(svc_type)

/* Clear the service bit in the transport channel bitmask */
#define DLSHMGR_CLEAR_CCTRCH_TO_RRC_CFG_SVC_BMSK(cctrch_idx, svc_type) \
  dlshmgr_cctrch_shared_svc_info[cctrch_idx].cctrch_to_rrc_cfg_svc_map &= \
    (~(dlshmgr_svc_mask_type)(DLSHMGR_SVC_BMSK(svc_type)))


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* Call back function type declaration */
/* ----------------------------------- */

/* Call back function for PhCh handle drop done */
typedef void DLSHMGR_PHCH_DROP_DONE_CB_FUNC_TYPE(uint8 cctrch_idx);

/* Enum type declaration */
/* --------------------- */

/* Various type of shared services on SCCPCH channel */
typedef enum
{
  DLSHMGR_SVC_TYPE_R99_PCH,  /* R99 PCH channel */
  DLSHMGR_SVC_TYPE_R99_FACH, /* R99 FACH channel */
  DLSHMGR_NUM_SVC_TYPE       /* Number of DL shared service types */
} dlshmgr_svr_type_enum_type;

/* Structure type declaration */
/* -------------------------- */

/* This structure defines the shared information for a CCTrCh */
typedef struct
{
  /* indicate if this information buffer in use or not */
  boolean in_use;
  /* Physical channel handle */
  dl_phch_cfg_handle_type phch_handle;

  /*Bit Mask of the services currently enabled on a particular CCTrCh*/
  dlshmgr_svc_mask_type cctrch_to_l1_enab_svc_map;

  /* TrCh service mask */
  uint16 trch_to_l1_enab_svc_mask[DEC_TOTAL_TRCHT_PER_CCTRCH];

  /*TrCh mapping on to currently enabled service*/
  dlshmgr_svc_to_trch_map_mask_type l1_enab_svc_to_trch_map[DLSHMGR_NUM_SVC_TYPE];

  /*Bit Mask of the services configured by RRC on a particular CCTrCh*/
  dlshmgr_svc_mask_type cctrch_to_rrc_cfg_svc_map;


  /*TrCh mapping on to currently configured service*/
  dlshmgr_svc_to_trch_map_mask_type rrc_cfg_svc_to_trch_map[DLSHMGR_NUM_SVC_TYPE];
} dlshmgr_cctrch_shared_svc_info_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================
FUNCTION dlshmgr_init

DESCRIPTION
  This function initialize DL shared manager database

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dlshmgr_init(void);

/*===========================================================================
FUNCTION dlshmgr_enable_service

DESCRIPTION
  This function enables service on given TrCh of CCTrCh. If there was no
  service active on that TrCh, it is enabled in decoder

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dlshmgr_enable_service(
  /* CCTrCh of TrCh in which service is to be enabled */
  uint8 cctrch_idx,
  /* TrCh index in which service is to be enabled */
  uint8 trch_idx,
  /* Service type to be enabled */
  dlshmgr_svr_type_enum_type service_type);

/*===========================================================================
FUNCTION dlshmgr_configure_service

DESCRIPTION
  This function configures service on given TrCh of CCTrCh.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dlshmgr_configure_service(
  /* CCTrCh of TrCh in which service is to be enabled */
  uint8 cctrch_idx,
  /* TrCh index in which service is to be enabled */
  uint8 trch_idx,
  /* Service type to be enabled */
  dlshmgr_svr_type_enum_type service_type);

/*===========================================================================
FUNCTION dlshmgr_update_phch_handle

DESCRIPTION
  This function saves the physical channel handle for this CCTrCh. Since there
  are many shared services on TrChs of CCTrCh hence one of service activation
  will setup physical channel, however physical channel may end up being dropped
  by another service when it is disabled. DL shared manager maintains physical
  channel handle so that any service channel manager can drop physical channel.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dlshmgr_update_phch_handle(
  /* CCTrCh index corresponding to physical channel handle */
  uint8 cctrch_idx,
  /* Physical channel handle type */
  dl_phch_cfg_handle_type phch_handle);

/*===========================================================================
FUNCTION dlshmgr_get_phch_handle

DESCRIPTION
  This function returns physical channel handle corresponding to CCTrCh index

DEPENDENCIES
  None.

RETURN VALUE
  Physical channel handle of the CCrCh 

SIDE EFFECTS
  None
===========================================================================*/

extern dl_phch_cfg_handle_type dlshmgr_get_phch_handle(
  /* CCTrCh index of physical channel handle */
  uint8 cctrch_idx);

/*===========================================================================
FUNCTION dlshmgr_get_trch_for_l1_enab_svc

DESCRIPTION
  This function returns the TrCh bit mask that has the service enabled in it.
  Bit 0 (LSB) corresponds to TrCh index 0. Currently there are 12 TrCh in CCTrCh
  hence 16 bit mask is sufficient

DEPENDENCIES
  None.

RETURN VALUE
  Bit mask indicating TrCh with service

SIDE EFFECTS
  None
===========================================================================*/

extern dlshmgr_svc_to_trch_map_mask_type dlshmgr_get_trch_for_l1_enab_svc(
  /* CCTrCh index */
  uint8 cctrch_idx,
  /* Service type */
  dlshmgr_svr_type_enum_type service_type);

/*===========================================================================
FUNCTION dlshmgr_get_l1_enab_svc_for_cctrch

DESCRIPTION
  This function returns the TrCh bit mask that has the service enabled in it.
  Bit 0 (LSB) corresponds to TrCh index 0. Currently there are 12 TrCh in CCTrCh
  hence 16 bit mask is sufficient

DEPENDENCIES
  None.

RETURN VALUE
  Bit mask indicating TrCh with service

SIDE EFFECTS
  None
===========================================================================*/

extern dlshmgr_svc_mask_type dlshmgr_get_l1_enab_svc_for_cctrch(
  /* CCTrCh index */
  uint8 cctrch_idx);

/*===========================================================================
FUNCTION dlshmgr_get_l1_enab_svc_for_trch

DESCRIPTION
  This function returns the TrCh bit mask that has the service enabled in it.
  Bit 0 (LSB) corresponds to TrCh index 0. Currently there are 12 TrCh in CCTrCh
  hence 16 bit mask is sufficient

DEPENDENCIES
  None.

RETURN VALUE
  Bit mask indicating TrCh with service

SIDE EFFECTS
  None
===========================================================================*/

extern dlshmgr_svc_mask_type dlshmgr_get_l1_enab_svc_for_trch(
  /* CCTrCh of TrCh in which service has to be found out */
  uint8 cctrch_idx,
  /* TrCh index in which service has to be found out */
  uint8 trch_idx);

/*===========================================================================
FUNCTION dlshmgr_get_trch_for_rrc_cfg_svc

DESCRIPTION
  This function returns the TrCh bit mask that has the service configured in it.
  Bit 0 (LSB) corresponds to TrCh index 0. Currently there are 12 TrCh in CCTrCh
  hence 16 bit mask is sufficient

DEPENDENCIES
  None.

RETURN VALUE
  Bit mask indicating TrCh with service

SIDE EFFECTS
  None
===========================================================================*/

dlshmgr_svc_to_trch_map_mask_type dlshmgr_get_trch_for_rrc_cfg_svc(
  /* CCTrCh index */
  uint8 cctrch_idx,
  /* Service type */
  dlshmgr_svr_type_enum_type service_type);

/*===========================================================================
FUNCTION dlshmgr_get_rrc_cfg_svc_for_cctrch

DESCRIPTION
  This function returns the TrCh bit mask that has the service configured in it.
  Bit 0 (LSB) corresponds to TrCh index 0. Currently there are 12 TrCh in CCTrCh
  hence 16 bit mask is sufficient

DEPENDENCIES
  None.

RETURN VALUE
  Bit mask indicating TrCh with service

SIDE EFFECTS
  None
===========================================================================*/

dlshmgr_svc_mask_type dlshmgr_get_rrc_cfg_svc_for_cctrch(
  /* CCTrCh index */
  uint8 cctrch_idx);


/*===========================================================================
FUNCTION dlshmgr_disable_service

DESCRIPTION
  This function disables service on given TrCh of CCTrCh. If there is no
  service active on that TrCh, it is disabled in decoder

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicating if service disable was successful or not. Function may
  fail if CCTrCh information doesn't exists

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dlshmgr_disable_service(
  /* CCTrCh of TrCh in which service is to be disabled */
  uint8 cctrch_idx,
  /* TrCh index in which service is to be disabled */
  uint8 trch_idx,
  /* Service type to be disabled */
  dlshmgr_svr_type_enum_type service_type);

/*===========================================================================
FUNCTION dlshmgr_deconfigure_service

DESCRIPTION
  This function deconfigures service on given TrCh of CCTrCh.

DEPENDENCIES
  None.

RETURN VALUE
  Boolean indicating if service deconfiguration was successful or not. Function may
  fail if CCTrCh information doesn't exists

SIDE EFFECTS
  None
===========================================================================*/

extern boolean dlshmgr_deconfigure_service(
  /* CCTrCh of TrCh in which service is to be disabled */
  uint8 cctrch_idx,
  /* TrCh index in which service is to be disabled */
  uint8 trch_idx,
  /* Service type to be disabled */
  dlshmgr_svr_type_enum_type service_type);

/*===========================================================================
FUNCTION dlshmgr_delete_db

DESCRIPTION
  This function clears the database of this CCTrCh and marks it as invalid.
  If database corresponding to this CCTrCh is not valid then this is NOOP.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void dlshmgr_delete_db(
  /* CCTrCh index */
  uint8 cctrch_idx);
#endif /* DLSHMGR_H */

