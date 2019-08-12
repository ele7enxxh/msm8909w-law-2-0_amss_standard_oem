#ifndef MMGSDI_NV_REFRESH_H
#define MMGSDI_NV_REFRESH_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                 P B M   N V   R E F R E S H  F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains routines used to handle the NV Refresh
  commands.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS



                        COPYRIGHT INFORMATION

Copyright (c) 2015 - 2016 QUALCOMM Technologies, Inc (QTI) and
its licensors. All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbm_nv_refresh.h#1 $$ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/16   lm      Update ECC cache and send ready after NV Refresh
07/21/15   nr      NV Refresh: MCFG - PBM interactions
===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmgsdi.h"
#include "pbmtask_i.h"
#include "pbm.h"

/* ----------------------------------------------------------------------------
   STRUCTURE:      PBM_HANDLE_NV_REFRESH_REQ_TYPE

   DESCRIPTION:
     This structure is used when generating a status request
     for the PBM Task.
     request_header  : Status request header information
     type            : NV Refresh type - slot or session
     slot_index      : Slot on which refresh is triggered
     sub_index       : session on which refresh is triggered
     reference_id    : MCFG reference identifier
                       - provided back in done API call
  -------------------------------------------------------------------------------*/
typedef struct {
  PBM_DEFINE_CMD_HEADER()
  mcfg_refresh_type_e_type   type;            /**< Refresh for slot or sub specific items */
  mcfg_refresh_index_type    slot_index;      /**< Slot index for this refresh */
  mcfg_refresh_index_type    sub_index;       /**< Subscription index for this refresh */
  uint32                     reference_id;    /**< Reserved. */
} pbm_handle_nv_refresh_req_type;

/*===========================================================================
FUNCTION PBM_INIT_NV_ECC_PER_SUB

DESCRIPTION
  This function is called at initialization and whenever a SIM is
  inserted or removed.  If uim_present indicates no SIM is present,
  then the longer hardcoded list of emergency numbers is used.  If
  the SIM is inserted, then the shorter list is used.

DEPENDENCIES
  None

SIDE EFFECTS
===========================================================================*/
void pbm_init_nv_ecc_per_slot (
  uint8     slot_index
);

/*===========================================================================
FUNCTION PBMI_NV_REFRESH_SLOT_ITEMS

DESCRIPTION
  Function to refresh the slot-specific items from NV/EFS at the index
  corresponding to the slot on which refresh is happening.

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_nv_refresh_slot_items(
  mcfg_refresh_index_type    refresh_index
);

/*===========================================================================
FUNCTION PBM_PROCESS_NV_REFRESH_REQ

DESCRIPTION
  This function processes nv refresh req - slot and session nv/efs items
  are refreshed.

DEPENDENCIES
  None

RETURN VALUE
  pbm_return_type

SIDE EFFECTS
  None
===========================================================================*/
pbm_return_type pbm_process_nv_refresh_req(
  const pbm_handle_nv_refresh_req_type *req_ptr
);

/*===========================================================================
FUNCTION PBM_NV_REFRESH_MCFG_CB

DESCRIPTION
  Call back registered with MCFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pbm_nv_refresh_mcfg_cb(
  mcfg_refresh_info_s_type *mcfg_refresh_info_ptr
);

/*===========================================================================
FUNCTION PBM_NV_MCFG_REFRESH_REGISTER

DESCRIPTION
  This function registers for NV refresh notification with MCFG

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void pbm_nv_mcfg_refresh_register(
  void
);

/*===========================================================================
FUNCTION pbm_free_nv_ecc_list

DESCRIPTION

This function frees memory allocated for NV ECC list

DEPENDENCIES
None

SIDE EFFECTS
None
===========================================================================*/
void pbm_free_nv_ecc_list(
  uint8   slot_index
);

#endif /* MMGSDI_NV_REFRESH_H */

