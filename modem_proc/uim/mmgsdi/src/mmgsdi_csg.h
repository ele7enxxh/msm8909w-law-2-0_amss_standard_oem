#ifndef MMGSDI_CSG_H
#define MMGSDI_CSG_H
/*===========================================================================


           M M G S D I   C S G   H E A D E R S


===========================================================================*/

/*===========================================================================
                        COPYRIGHT INFORMATION

Copyright (c) 2012 - 2013 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary. 
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/mmgsdi/src/mmgsdi_csg.h#1 $$ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/05/13   bcho    Re-run the EONS algorithm when PLMNID/LAC/RAT is changed
07/15/13   vs      EONS updates to support single baseband SGLTE  
04/24/13   vv      Reset CSG eons info in the app. info
01/21/13   vv      Introduced new UCS2_PACKED encoding scheme
01/21/13   vv      Added support to update ACSGL record
10/15/12   vv      Added EONS support for the CSG
 
=============================================================================*/
 
/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/

#include "comdef.h"
#include "cm.h"
#include "mmgsdi.h"
#include "mmgsdi_eons.h"

/*=============================================================================

                       DEFINES DECLARATIONS

=============================================================================*/
#define MMGSDI_CSG_ID_SIZE                 4
#define MMGSDI_MAX_CSG_INFO                30
#define MMGSDI_EONS_INVALID_CSG_ID         0xFFFFFFFF
#define MMGSDI_CSG_INFO_TLV_SIZE           8
#define MMGSDI_PLMN_TLV_SIZE               5
#define MMGSDI_ACSGL_TLV_SIZE              (MMGSDI_CSG_INFO_TLV_SIZE + \
                                            MMGSDI_PLMN_TLV_SIZE +     \
                                            2)

/* Byte containing encoding-type info in hnbn data buffer */
#define MMGSDI_EONS_CSG_DATA_ENC_BYTE_POS  0
/*=============================================================================

                       STRUCTURE DECLARATIONS

=============================================================================*/
/* ----------------------------------------------------------------------------
   STRUCTURE:      MMGSDI_CSG_PLMN_INFO_TYPE

   DESCRIPTION:
     Data Structure to hold the EF-ACSGL or EF-OCSGL record content
-------------------------------------------------------------------------------*/
typedef struct{
  uint8                      plmn_id_val[MMGSDI_PLMN_ID_SIZE];
  mmgsdi_csg_id_type         csg_id;
  mmgsdi_rec_num_type        csgt_record_num;
  mmgsdi_rec_num_type        hnbn_record_num;
  boolean                    csg_display_indicator;
}mmgsdi_csg_plmn_info_type;
 

/*=============================================================================

                          FUNCTIONS

=============================================================================*/

/*==========================================================================
FUNCTION MMGSDI_EONS_RUN_ALGO_CSG

DESCRIPTION
  Function provides the home node(b) name.
  For the home node(b) it runs the below name lookup algorithm.
  EONS algorithm for HNB name source has the following priority,
  1. NITZ information Look up, NITZ info is received in CM SS Event
  2. Provide the name from EF-HNBN or EF-OHNBN
  3. If the HNB name is not retrieved, it provides the last known
     RPLMN info for the SPN and Operator name string

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_eons_run_algo_csg(
  mmgsdi_eons_call_stack_enum_type  stack_type,
  mmgsdi_session_id_type            session_id,
  mmgsdi_eons_trigger_enum_type     eons_trigger,
  mmgsdi_plmn_id_type             * plmn_csg_id,
  mmgsdi_list_plmn_name_type      * plmn_name_list_ptr,
  uint32                            index,
  boolean                           plmn_id_lac_rat_changed
);


/*==========================================================================
FUNCTION MMGSDI_CSG_REFRESH_CSGL_CACHE

DESCRIPTION
  This function clears cache and reads the CSG EF which got refreshed. It
  then updates the cache of the EF refreshed.

DEPENDENCIES
  None

RETURN VALUE
  MMGSDI_RETURN_ENUM_TYPE

SIDE EFFECTS
  None
==========================================================================*/
mmgsdi_return_enum_type mmgsdi_csg_refresh_csgl_cache(
  mmgsdi_session_id_type session_id,
  mmgsdi_client_id_type  client_id,
  mmgsdi_file_enum_type  file
);

/* =============================================================================
  FUNCTION:      MMGSDI_CSG_PROCESS_UPDATE_ACSGL_RECORD_REQUEST

  DESCRIPTION:
    This function handles adding or deleting a csg_id to the EF-ACSGL record

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    mmgsdi_return_enum_type
    MMGSDI_SUCCESS:          The command processing was successful.
    MMGSDI_ERROR:            The command processing was not successful.
    MMGSDI_INCORRECT_PARAMS: The parameters supplied to the API are not
                             within appropriate ranges.

  SIDE EFFECTS:
    None
=============================================================================*/
mmgsdi_return_enum_type mmgsdi_csg_process_update_acsgl_record_reqest
(
  mmgsdi_session_update_acsgl_record_req_type * req_ptr
);

/* =============================================================================
  FUNCTION:      MMGSDI_CSG_RESET_EONS_FLAGS_IN_APP_INFO

  DESCRIPTION:
    This function resets the CSG eons info in the global application information

  DEPENDENCIES:
    None

  LIMITATIONS:
    None

  RETURN VALUE:
    None

  SIDE EFFECTS:
    None
=============================================================================*/
void mmgsdi_csg_reset_eons_flags_in_app_info
(
  mmgsdi_session_id_type session_id
);

/*===========================================================================
FUNCTION MMGSDI_EONS_TRIGGER_ACSGL_CSGT_HNBN_CACHE

DESCRIPTION
  Function to trigger caching CSGL/CSGT/HNBN records

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mmgsdi_eons_trigger_acsgl_csgt_hnbn_cache(
  mmgsdi_session_id_type        session_id
);

#endif /* MMGSDI_CSG_H */
