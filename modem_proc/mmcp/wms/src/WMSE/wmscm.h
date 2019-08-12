#ifndef WMSCM_H
#define WMSCM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmscm.h -- Interface with CM Definitions

  The WMS module which implements the User API for SMS. This source file
  defines the WMS interfaces used by Call Manager.

  -------------

    Copyright (c) 2001-2013 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmscm.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmsi.h"
#include "cm.h"

#ifdef FEATURE_GWSMS
#include "sys.h"
#endif /* FEATURE_GWSMS */


/*===========================================================================
============================ DATA TYPES =====================================
===========================================================================*/

#ifdef FEATURE_CDSMS
typedef struct
{
  sys_modem_as_id_e_type             asubs_id;
  cm_call_state_e_type               call_state;
  cm_call_type_e_type                call_type;
  cm_call_id_type                    call_id;
  cm_cdma_call_info_s_type           mode_info_cdma_call;
} wms_cm_mm_call_info_s_type;
#endif /* FEATURE_CDSMS */


typedef struct
{
  cm_client_id_type                  cm_client;
#ifdef FEATURE_CDSMS
  wms_cm_mm_call_info_s_type         call_info;
  cm_num_s_type                      called_number;
#endif /* FEATURE_CDSMS */
} wms_cm_s_type;


/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION wms_cm_s_ptr

DESCRIPTION
  Return the pointer to the CM data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal CM data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_cm_s_type * wms_cm_s_ptr(
  void
);


void wms_cm_init
(
  void
);

void wms_cm_process_event(
  wms_cmd_type                      *cmd_ptr
);

#ifdef FEATURE_CDSMS

void wms_cm_call_orig(
  wms_dc_so_e_type                   so
);

void wms_cm_call_end(
  void
);

void wms_cm_call_answer(
  void
);


void wms_cm_awi_ack(
  uint8                              seq_no,
  boolean                            bf_bit,
  wms_error_class_e_type             error_class,
  wms_cdma_tl_status_e_type          tl_status
);

boolean wms_cm_check_call_type(
  cm_call_type_e_type                call_type,
  sys_sys_mode_e_type                sys_mode
);
#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION wms_cm_check_active_volte_call

DESCRIPTION
  Check if there is an active VoLTE call

DEPENDENCIES
  none

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cm_check_active_volte_call
(
  sys_modem_as_id_e_type         as_id,
  sys_sys_mode_e_type            sys_mode
);

#ifdef FEATURE_CDSMS_BROADCAST
void wms_cm_bc_enable(
  boolean                            bc_enable
);
#endif /* FEATURE_CDSMS_BROADCAST */

#ifdef FEATURE_CDSMS

/*===========================================================================
FUNCTION wms_cm_sms_send_cdma_msg2

DESCRIPTION
  CM Send MO SMS Message Function

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_sms_send_cdma_msg2(
  const wms_address_s_type          *address_ptr,
  const wms_OTA_message_type        *OTA_ptr,
  wms_bearer_e_type                  bearer,
  wms_client_ch_pref_e_type          ch_pref
);

#endif /* FEATURE_CDSMS */

/*===========================================================================
FUNCTION wms_cm_suspend_pscall

DESCRIPTION
  To suspended PS call on other slot

DEPENDENCIES
  none

RETURN VALUE
  error code of type cm_call_cmd_err_e_type

SIDE EFFECTS
  None.
===========================================================================*/

cm_call_cmd_err_e_type wms_cm_suspend_pscall(
  sys_modem_as_id_e_type             as_id
);

/*===========================================================================
FUNCTION wms_cm_resume_pscall

DESCRIPTION
  To resume the suspended PS call on other slot

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void wms_cm_resume_pscall(
  sys_modem_as_id_e_type             as_id
);

/*===========================================================================
FUNCTION wms_cm_resume_1x_pscall

DESCRIPTION
  To resume the suspended PS call on other subscription

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_resume_1x_pscall(
  sys_modem_as_id_e_type             as_id
);

/*===========================================================================
FUNCTION wms_cm_resume_gw_pscall

DESCRIPTION
  To resume the suspended PS call on other subscription

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_cm_resume_gw_pscall(
  sys_modem_as_id_e_type as_id
);

void wms_cm_subs_event_cb(
  cm_ph_event_e_type                 subs_event,
  const cm_subs_info_s_type         *subs_info_ptr
);

void wms_cm_init_routes(
  sys_modem_as_id_e_type             as_id
);

void wms_cm_init_domain_pref(
  wms_prov_app_info_s_type *appInfo_ptr
);

void wms_cm_ph_event_cb_proc(
  wms_cmd_type                      *cmd_ptr
);

void wms_cm_subs_event_cb_proc(
  wms_cmd_type                      *cmd_ptr
);

/*===========================================================================
FUNCTION wms_cm_CT_features_supported

DESCRIPTION
  To check if current operator is CT

DEPENDENCIES
  Gets this info from CM API

RETURN VALUE
  boolean

SIDE EFFECTS
  None.
===========================================================================*/
boolean wms_cm_CT_features_supported(
  sys_modem_as_id_e_type as_id
);

#if (defined FEATURE_GWSMS_MO_CONTROL && defined FEATURE_GSTK)
/*===========================================================================
FUNCTION wms_cm_map_sys_mode_to_gstk_access_tech_rat

DESCRIPTION
  This function will convert the sys mode to gstk_access_technology

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
gstk_access_technology_type wms_cm_map_sys_mode_to_gstk_access_tech_rat
(
  sys_sys_mode_e_type sys_mode
);
#endif /* FEATURE_GWSMS_MO_CONTROL && FEATURE_GSTK */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSCM_H */
