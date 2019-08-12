#ifndef MMGSDI_SS_EVENT_H
#define MMGSDI_SS_EVENT_H
/*===========================================================================



                 M M G S D I   S S   E V E N T   F U N C T I O N S


GENERAL DESCRIPTION

  This source file contains the event handling CM ss events for MMGSDI

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/14   tl      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "mmgsdi.h"
#include "cm.h"

/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define MMGSDI_SS_INFO_CHANGED(x)                                                             \
          ((x & CM_SS_EVT_MOBILITY_MGMT_MASK)    || (x & CM_SS_EVT_SRV_STATUS_MASK)       ||  \
           (x & CM_SS_EVT_ROAM_STATUS_MASK)      || (x & CM_SS_EVT_LAC_TAC_RAC_CHGD_MASK) ||  \
           (x & CM_SS_EVT_CSG_INFO_CHANGED_MASK) || (x & CM_SS_EVT_SYS_MODE_MASK))

#define MMGSDI_SRV_STATUS_CHANGED(x)                                      \
          ((x == SYS_SRV_STATUS_SRV) || (x == SYS_SRV_STATUS_LIMITED) ||  \
           (x == SYS_SRV_STATUS_LIMITED_REGIONAL))

#define MMGSDI_3GPP_SYS_MODE(x)                                      \
          ((x == SYS_SYS_MODE_GSM) || (x == SYS_SYS_MODE_WCDMA)  ||  \
           (x == SYS_SYS_MODE_GW)  || (x == SYS_SYS_MODE_LTE)    ||  \
           (x == SYS_SYS_MODE_GWL) || (x == SYS_SYS_MODE_TDS))

#define MMGSDI_3GPP_SS_INFO_CHANGED(x)                                                         \
          ((x & CM_SS_EVT_MOBILITY_MGMT_MASK)    || (x & CM_SS_EVT_SRV_STATUS_MASK)        ||  \
           (x & CM_SS_EVT_LAC_TAC_RAC_CHGD_MASK) || (x & CM_SS_EVT_CSG_INFO_CHANGED_MASK)  ||  \
           (x & CM_SS_EVT_ROAM_STATUS_MASK)      || (x & CM_SS_EVT_SYS_MODE_MASK))

#define MMGSDI_3GPP2_SYS_MODE(x)                                 \
          ((x == SYS_SYS_MODE_CDMA) || (x == SYS_SYS_MODE_HDR))

#define MMGSDI_3GPP2_SS_INFO_CHANGED(x)                                           \
          ((x & CM_SS_EVT_ROAM_STATUS_MASK) || (x & CM_SS_EVT_SRV_STATUS_MASK) || \
           (x & CM_SS_EVT_SYS_MODE_MASK))

/*===========================================================================
         E V E N T    N O T I F I C A T I O N   F U N C T I O N S
============================================================================*/

/*===========================================================================
  FUNCTION MMGSDI_EONS_COPY_PER_STACK_SS_INFO

DESCRIPTION
  Utility function to copy per stack SS info

DEPENDENCIES
  None

RETURN VALUE
  mmgsdi_return_enum_type

SIDE EFFECTS
  None
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_copy_per_stack_ss_info(
  const mmgsdi_eons_per_stack_ss_data_type  * in_ss_info_ptr,
  mmgsdi_eons_per_stack_ss_data_type        * out_ss_info_ptr,
  boolean                                     allocate_mem
);

/*===========================================================================
FUNCTION MMGSDI_CM_SS_EVENT_CB

DESCRIPTION
  Handler for CM_SS_EVENT events.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECTS
  State variables may be updated as needed
===========================================================================*/
void mmgsdi_cm_ss_event_cb (
  cm_ss_event_e_type                ss_event,
  const cm_mm_msim_ss_info_s_type * mm_ss_info_ptr
);

/*===========================================================================
FUNCTION MMGSDI_PROCESS_EONS_SS_EVENT

DESCRIPTION
  This function processes any relevant MM, PLMN, LAC, TAC, CSG information
  obtained from an SS event

DEPENDENCIES
  None

LIMITATIONS:
  None

RETURN VALUE
  mmgsdi_return_enum_type
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_eons_ss_event(
  mmgsdi_eons_ss_event_req_type * req_ptr
);

/*===========================================================================
FUNCTION MMGSDI_PROCESS_EONS_SS_EVENT_RESPONSE

DESCRIPTION
  This function, process the response for an SS event. It will determine
  if the MMGSDI EONS event needs to be broadcasted.

DEPENDENCIES
  NONE

LIMITATIONS
  NONE

RETURN VALUE
  mmgsdi_return_enum_type
  MMGSDI_SUCCESS          - Command processed,
  MMGSDI_INCORRECT_PARAMS - Wrong input parameters provided to function
  MMGSDI_ERROR            - Other unknown failure

SIDE EFFECTS
  NONE
===========================================================================*/
mmgsdi_return_enum_type mmgsdi_process_eons_ss_event_response(
  const mmgsdi_uim_report_rsp_type  * rsp_ptr
);

#endif /* MMGSDI_SS_EVENT_H */
