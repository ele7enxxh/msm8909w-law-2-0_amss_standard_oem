#ifndef CMCC_H
#define CMCC_H
/*===========================================================================

         C A L L   M A N A G E R   M B M S    H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions necessary to interface
  with CMCC.C

  This module - CMCC - is responsible for call control as specified by 
  SIM/RUIM spec

Copyright (c) 2004 - 2009 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmcc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/10   fj      Add support for LTE call control.
04/29/09   rn      Initial release
===========================================================================*/


/**--------------------------------------------------------------------------
** Includes
** --------------------------------------------------------------------------
*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "cm.h"        /* External interface to cm.c */

mmgsdi_srv_available_cnf_type *     cm_mmgsdi_srv_available_cnf_ptr;


#define CMCC_GET_MMGSDI_SLOT_FROM_SUBS(subs) \
  (((subs) == CM_CC_SUBSCRIPTION_SOURCE_SIM_3)?MMGSDI_SLOT_3:\
       (((subs) == CM_CC_SUBSCRIPTION_SOURCE_SIM_2)\
                          ?MMGSDI_SLOT_2:MMGSDI_SLOT_1))


/**--------------------------------------------------------------------------
** Functions - internal
** --------------------------------------------------------------------------
*/
/*===========================================================================
FUNCTION cmcc_sim_cap_cb

DESCRIPTION
 Callback for gsdi_get_sim_capabilities

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmcc_sim_cap_cb
(
  mmgsdi_return_enum_type    status,
     /* Status sent by MMGSDI */
  mmgsdi_cnf_enum_type       cnf,
     /* command conf from MMGSDI */
  mmgsdi_cnf_type*           gsdi_cnf
   /* confirmation info received from MMGSDI */
);


/*===========================================================================

FUNCTION cmcc_call_control_processing

DESCRIPTION
  Function to do call control in CM.

DEPENDENCIES
  none

RETURN VALUE
  TRUE - If CC success
  FALSE - If CC fails

SIDE EFFECTS
  none

===========================================================================*/

boolean cmcc_call_control_processing (

  cm_call_id_type                       call_id,
    /* call id */

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_call_type_e_type                   call_type,
    /* call type */

  const cm_num_s_type                         *orig_num_ptr,
    /* orginating number */

  void                                  *user_data,
    /* Clients should not modify - Data to be passed back in cb function */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  const cm_gw_ps_cc_params_s_type             *gw_ps_cc_params_ptr
    /* Call Control parameters for GSM/WCDMA packet switched call */

);

/*===========================================================================
FUNCTION cmcc_call_ctrl_after_sim_capabilities

DESCRIPTION
 Performs call control operations that depend on having SIM capabilities

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void cmcc_call_ctrl_after_sim_capabilities
(
  cmcc_object_s_type          *cc_object_ptr,
  /* Pointer to CC object type */

  cm_orig_client_categ_e_type client_categ
  /* client category */
);

/*===========================================================================
FUNCTION cmcc_call_control_processing_lte

DESCRIPTION
  Perform call control for PS data call.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
boolean cmcc_call_control_processing_lte (

  cm_cc_subscription_source_e_type      cdma_subscription_source,
    /* where CM is getting it's cdma subscription information */

  cm_cc_subscription_source_e_type      gw_subscription_source,
    /* where CM is getting it's gw subscription information */

  cm_nc_call_control_complete_cb_f_type *cm_call_control_complete_cb_func,
    /* function to be called when processing is complete */

  const cm_lte_ps_cc_params_s_type       *lte_ps_cc_params_ptr,
    /* PS call params for call control */

  cm_ss_e_type                            cm_ss,
    /* On which ss the call is made */
    
  sys_modem_as_id_e_type                  asubs_id
    /* On which asubs_id the call is made */
);


/*===========================================================================
FUNCTION cmcc_acl_read_complete

DESCRIPTION
  This is the read callback for reading ACL.  This is used in call
  control for packet data calls.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void cmcc_acl_read_complete(

  uint32     file_length,
   /* Specifies the file_lenght of ACL */
   
  uint8*     acl_file_content,
   /* Pointer to acl file content */
   
  boolean    status,
   /* Status of 'read' sent by MMGSDI */
   
  void      *client_data
   /* data block pointer */
   
);

/*===========================================================================

FUNCTION NAME cmcc_service_available_cb

DESCRIPTION
  Function is called by MMGSDI when finishing processing is_service_available request.

RETURN VALUE
  NONE

DEPENDENCIES
  None
===========================================================================*/
/*lint -esym(765, cmcc_service_available_cb)
** Can't be static, unit test uses it
*/
void cmcc_service_available_cb(
    mmgsdi_return_enum_type    status,
     /* Status sent by MMGSDI */
    mmgsdi_cnf_enum_type       cnf,
     /* command conf from MMGSDI */
    const mmgsdi_cnf_type      *cnf_ptr
     /* conf pointer returned by MMGSDI */
);

#endif /* CMCC_H */

