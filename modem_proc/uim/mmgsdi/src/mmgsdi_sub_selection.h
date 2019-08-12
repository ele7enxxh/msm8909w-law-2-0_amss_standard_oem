#ifndef MMGSDI_SUB_SELECTION_H
#define MMGSDI_SUB_SELECTION_H
/*===========================================================================


               M M G S D I   SUBSCRIPTION SELECTION   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2013, 2015 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.  
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_sub_selection.h#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
03/24/15   kk      Subs prov EFS error handling
08/30/13   tl      Convert channel info table from static to dynamic allocation
08/20/13   spo     Initial version

=============================================================================*/

/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "mmgsdi.h"

/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_SUB_APP_SPECIFIC_INFO_TYPE

   DESCRIPTION:
     This is used to store provisioning application information in
     MMGSDI EFS.
     app_data_valid  : Indicates if the application data is valid or not
     session_type    : Session type of provisioned application
     app_type        : Type of application
     aid_len         : Length of Application Id
     aid_value       : Value of Application Id
------------------------------------------------------------------------------- */
typedef struct{
  boolean                             app_data_valid;
  mmgsdi_session_type_enum_type       session_type;
  mmgsdi_app_enum_type                app_type;
  mmgsdi_static_data_type             app_data;
}mmgsdi_sub_app_specific_info_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_SUBS_PROV_DATA_TYPE

   DESCRIPTION:
     This is used to store iccid and application information which are recently
     provisioned on corresponding slots. It is resricted to store USIM/SIM 
     information at index 0 and CSIM/RUIM app information at index 1 in
     subs_app_data.
     subs_app_data   : Holds the subscription information of correspondig slot.
  -------------------------------------------------------------------------------*/
typedef struct {
  uint8                               slot_id;
  mmgsdi_iccid_info_type              iccid;
  mmgsdi_sub_app_specific_info_type   subs_app_data[MMGSDI_MAX_SUPPORTED_AUTO_PROV_APPS];
} mmgsdi_subs_prov_data_type;

/* ----------------------------------------------------------------------------
   STRUCTURE: MMGSDI_SUBS_PROV_INFO_TYPE

   DESCRIPTION:
     This is used to hold the MMGSDI EFS subs_prov_info.txt which conatins provisioned 
     app information. The purpose is to avoid more reads to EFS
     is_efs_content_valid   : Indicates if this global has valid content from EFS
     subs_prov_data         : Holds the latest subscription information per slot
  ------------------------------------------------------------------------------- */
typedef struct{
  boolean                     is_efs_content_valid;
  mmgsdi_subs_prov_data_type  subs_prov_data[MMGSDI_MAX_NUM_SLOTS];
}mmgsdi_subs_prov_info_type;

/*=============================================================================

                       FUNCTION PROTOTYPES

=============================================================================*/
/*===========================================================================
FUNCTION MMGSDI_UPDATE_EFS_SUBS_PROV

DESCRIPTION
  This function will check if any new application is activated compared to 
  the one stored in EFS subs_prov_info.txt. It will update the EFS in case of
  new application activation.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_update_efs_subs_prov(
  const mmgsdi_session_changed_evt_info_type *event_ptr,
  mmgsdi_session_id_type                      session_id
);

/*===========================================================================
FUNCTION MMGSDI_CLEAR_SUBS_PROV_APP_INFO

DESCRIPTION
  This function clears the EFS content if application has been
  deactivated by client request

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
============================================================================ */
void mmgsdi_clear_subs_prov_app_info(
  const mmgsdi_session_changed_evt_info_type *event_ptr
);

/*===========================================================================
FUNCTION:  MMGSDI_PROVISION_APPLICATIONS_FROM_EFS

DESCRIPTION
  This function will try to provision applications stored in EFS if
  ICCID stored is matching with current ICCID.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_provision_applications_from_efs(
  mmgsdi_slot_id_enum_type slot_id
);

/*===========================================================================
FUNCTION:  MMGSDI_DELETE_EFS_SUBS_PROV

DESCRIPTION
  This function will delete efs subs prov data when nv is disabled
  and efs file still present.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_delete_efs_subs_prov(
  void
);

#endif /* MMGSDI_EONS_H */