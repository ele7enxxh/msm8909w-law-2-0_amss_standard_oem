#ifndef MCCSYNCID_H
#define MCCSYNCID_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     S Y N C   ID   M A N A G E R
         S U P P O R T   F O R   F A S T   C A L L   S E T U P

GENERAL DESCRIPTION
  This module contains definitions and interfaces for SYNC ID manager. These
  functions are created for the support of fast call setup.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 - 2013 Qualcomm Technologies, Inc. 
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

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/inc/mccsyncid.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/12   ppr     FR#2370 - 1x memory optimization changes
05/24/12   ppr     Feature Cleanup
02/04/12   ppr     Feature Cleanup
01/17/12   ppr     Feature Cleanup: Mainlined Always ON features
11/15/10   jtm     Initial SU API split for CP.
03/28/05   fc      Added support for DCA.
01/31/05   sb      Added support for the SYNC ID info rec.
08/09/04   fc      Service negotiation changes for Release C and D.
06/16/04   fc      Support for Release C Addendum changes on SYNC ID.
04/20/04   fc      Fixed warnings.
03/12/04   fc      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "msg.h"
#include "m1x_diag.h"
#include "err.h"
#include "cai.h"
#include "caii.h"
#include "mccdma.h"
#include "mccdma_v.h"
#include "mccdma_i.h"
#include "mccsrid.h"

#if defined(FEATURE_IS2000_REL_C)

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
                            Types
-------------------------------------------------------------------------*/

#define MCCSYNCID_INVALID         255
#define MCCSYNCID_RESTORE_ALL_CON 0x3F
#define MCCSYNCID_RESTORE_ONE_CON 0x01

/* Type which contains Sync ID specific information */
typedef struct
{
  uint8 sync_id_len;              /* SYNC ID length */
  uint8 sync_id[CAI_SYNC_ID_LEN_MAX]; /* SYNC ID */
  uint16 sid;                     /* SID */
  uint16 nid;                     /* NID */
  uint8 band_class;               /* Band class */
  caii_complete_srv_cfg_type cfg; /* Service configuration associated */
} sync_id_info_type;

#ifdef FEATURE_1X_CP_MEM_OPT
/* Pointer holds list of SYNC ID entries */
extern sync_id_info_type *sync_id_info;
#endif /* FEATURE_1X_CP_MEM_OPT */

typedef struct
{
  uint8 num_sync_id;
  uint8 mru[CAI_MAX_NUM_STORED_SYNC_ID];
} mru_sync_id_info_type;

/* Information about Service Configuration to be restored */
typedef struct
{
  uint8 mru_index;
  uint8 cci_con_index;
  caii_complete_srv_cfg_type cfg;
} restore_srv_cfg_info_type;

/* Pointer to hold info of service configuration to be verified */
extern restore_srv_cfg_info_type *restore_srv_cfg_info_ptr;

/* Indicates the type of service configuration to be restored */
typedef enum
{
  MCCSYNCID_SRV_CFG_RESTORE_WITHOUT_MOD,
    /* Restore stored service configuration without modifications */
  MCCSYNCID_SRV_CFG_RESTORE_WITH_MOD,
    /* Restore stored service configuration with modifications */
  MCCSYNCID_SRV_CON_RESTORE
    /* Restore service connection */
} restore_srv_cfg_type;

/* Indicates the type of update needed on internal information about SYNC_ID */
typedef enum
{
  MCCSYNCID_ACTIVATE_OLD_SYNC_ID, /* Update MRU SYNC_ID index list */
  MCCSYNCID_ACTIVATE_NEW_SYNC_ID, /* Update both SYNC_ID list and MRU SYNC_ID index list */
  MCCSYNCID_DEACTIVATE_SYNC_ID,   /* Mark SYNC_ID inactive */
  MCCSYNCID_NO_OP                 /* No action */
} restore_action_type;

/* Information about SYNC ID to be restored */
typedef struct
{
  restore_action_type action;

  /* Only index to MRU entry is available if restore action is
     MCCSYNCID_ACTIVATE_OLD_SYNC_ID.

     Only SYNC ID info is available if restore action is
     MCCSYNCID_ACTIVATE_NEW_SYNC_ID.

     Only SYNC ID info are available if restore action is
     MCCSYNCID_UPDATE_SYNC_ID. */

  uint8 mru_index;                    /* Index to MRU entry */
  uint8 sync_id_len;                  /* SYNC ID length */
  uint8 sync_id[CAI_SYNC_ID_LEN_MAX]; /* SYNC ID */
} restore_sync_id_info_type;

/*-------------------------------------------------------------------------
    Function prototypes
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION MCCSYNCID_POWERUP_INIT

DESCRIPTION
  This procedure initializes Sync ID database.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_powerup_init
(
  void
);

/*===========================================================================

FUNCTION MCCSYNCID_CLEAR_MS_REQ_INFO

DESCRIPTION
  This procedure initializes all SYNC ID variables related to Mobile request
  for service configuration restoration.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_clear_ms_req_info
(
  void
);

/*===========================================================================

FUNCTION MCCSYNCID_CLEAR_CALL_INFO

DESCRIPTION
  This procedure initializes all SYNC ID variables related to call setup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_clear_call_info
(
  void
);

/*===========================================================================

FUNCTION MCCSYNCID_FIND_COMPATIBLE_SYNC_ID

DESCRIPTION
  This procedure checks whether there is a compatible service econfiguration
  having the same SID, NID (and CDMA frequency) as current system.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if a compatible Sync ID is found. Otherwise, FALSE.

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccsyncid_find_compatible_sync_id
(
  boolean propose_new_sync_id,
    /* Indicates if new stored service configuration can be proposed */
  uint16 so,
    /* Service option */
  boolean bs_supported_sync_id,
    /* Indicates if BS supports SYNC ID */
  boolean *given_sr_id_valid_ptr,
    /* Pointer to variable that indicates if user specified SR ID */
  uint8 *given_sr_id_ptr,
    /* Pointer to variable that has user specified SR ID */
  uint8 *sync_id_len_ptr,
    /* Pointer to variable that saves SYNC ID length to be requested */
  uint8 *sync_id_ptr,
    /* Pointer to variable that saves SYNC ID to be requested */
  uint8 *sr_id_restore_ptr
    /* Pointer to variable that saves SR ID to be requested */
);

/*===========================================================================

FUNCTION MCCSYNCID_GET_RESTORE_CONFIG

DESCRIPTION
  This procedure retrieves the service configuration to be restored.

DEPENDENCIES
  restore_srv_cfg points to the service configuration to be restored.

RETURN VALUE
  Pointer to service configuration to be restored.

SIDE EFFECTS
  None.

===========================================================================*/

const restore_srv_cfg_info_type* mccsyncid_get_restore_config
(
  void
);

/*===========================================================================

FUNCTION MCCSYNCID_VALIDATE_ECAM

DESCRIPTION
  This function validates the Extened Channel Assignment Message on the
  aspect of stored service configuration restoration request.

DEPENDENCIES
  None.

RETURN VALUE
  0 if specified ECAM is valid. Otherwise, reject reason.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mccsyncid_validate_ecam
(
  mccdma_chnasn_type *chn_assign
);

/*===========================================================================

FUNCTION MCCSYNCID_ECAM_INIT

DESCRIPTION
  This function updates the internal information about stored service
  configuration to be restored for the Extended Channel Assignment Message.

DEPENDENCIES
  pending_restore_info will be updated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_ecam_init
(
  mccdma_chnasn_type *chn_assign
);

/*===========================================================================

FUNCTION MCCSYNCID_VALIDATE_SCM

DESCRIPTION
  This function validates the Service Connect Message on the aspect of stored
  service configuration restoration request.
  restored.

DEPENDENCIES
  None.

RETURN VALUE
  0 if specified SCM is valid. Otherwise, reject reason.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mccsyncid_validate_scm
(
  caii_srv_con_type *scm
);

/*===========================================================================

FUNCTION MCCSYNCID_SCM_INIT

DESCRIPTION
  This function updates the internal information about stored service
  configuration to be restored for the Service Connect Message.

DEPENDENCIES
  pending_restore_info will be updated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_scm_init
(
  caii_srv_con_type *scm
);

/*===========================================================================

FUNCTION MCCSYNCID_VALIDATE_CLAM

DESCRIPTION
  This function validates the Call Assignment Message on the aspect of stored
  service configuration restoration request.

DEPENDENCIES
  None.

RETURN VALUE
  0 if specified CLAM is valid. Otherwise, reject reason.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mccsyncid_validate_clam
(
  caii_call_assignment_type *clam
);

/*===========================================================================

FUNCTION MCCSYNCID_CLAM_INIT

DESCRIPTION
  This function updates the internal information about stored service
  configuration to be restored for the Call Assignment Message.

DEPENDENCIES
  restore_sync_id_info will be updated.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_clam_init
(
  caii_call_assignment_type *clam
);

/*===========================================================================

FUNCTION MCCSYNCID_HDM_INIT

DESCRIPTION
  It updates the internal information about stored service configuration to be
  restored for Handoff Direction Message.

DEPENDENCIES
  pending_restore_info will be updated.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_hdm_init
(
  caii_ho_msg_type *hdm
);

/*===========================================================================

FUNCTION MCCSYNCID_UPDATE_RESTORE_INFO

DESCRIPTION
  This function is called precisely at the action time of SCR/HO transaction
  to update SYNC ID info to be restored.

DEPENDENCIES
  pending_restore_info must be point to the Sync ID to be restored.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_update_restore_info
(
  void
);

/*===========================================================================

FUNCTION MCCSYNCID_UPDATE_ACTIVE_SYNC_ID

DESCRIPTION
  This procedure updates the active SYNC_ID information.

DEPENDENCIES
  current_restore_info must be point to the Sync ID to be restored.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_update_active_sync_id
(
  void
);

/*===========================================================================

FUNCTION MCCSYNCID_GET_ACTIVE_SYNC_ID_LEN

DESCRIPTION
  This function retrieves the length of active Sync ID.

DEPENDENCIES
  None.

RETURN VALUE
  Zero if Sync ID is not active. Otherwise, length of active Sync ID.

SIDE EFFECTS
  None.

===========================================================================*/

uint8 mccsyncid_get_active_sync_id_len
(
  void
);

/*===========================================================================

FUNCTION MCCSYNCID_GET_ACTIVE_SYNC_ID

DESCRIPTION
  This function copies the value of active Sync ID to user specified buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void mccsyncid_get_active_sync_id
(
  uint8 *buf_ptr
);
#endif /* FEATURE_IS2000_REL_C */

#endif /* MCCSYNCID_H */
