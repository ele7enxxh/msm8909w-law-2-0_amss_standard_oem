#ifndef WMSCL_H
#define WMSCL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmscl.c -- Client Group Definitions

  The WMS module which implements the User API for SMS. This source file
  defines the client group data types and functions.

  -------------

    Copyright (c) 2001-2014 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmscl.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"
#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)



/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"
#include "ref_cnt_obj.h"


#define WMS_CLIENT_ALL (-1)

/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/
typedef struct
{
  _REF_CNT_OBJ;       // Reference counting object from ref_cnt_obj.h
  boolean                          activated;
  wms_client_id_type               client_id;

  wms_cl_parse_msg_cb_type         parse_msg_cb;

  wms_cfg_event_cb_type            cfg_event_cb;
  wms_msg_event_cb_type            msg_event_cb;
  wms_dc_event_cb_type             dc_event_cb;
  wms_dc_disconnect_cb_type        dc_disconnect_cb;
  wms_msg_transport_status_cb_type transport_status_cb;
  wms_bc_mm_event_cb_type          bc_mm_event_cb;
  wms_dbg_event_cb_type            dbg_event_cb;

  boolean                          active_bearers[WMS_BEARER_MAX];
  wms_bearer_e_type                preferred_bearer;
  wms_bearer_e_type                allowed_bearer;
  wms_bearer_e_type                last_used_bearer;

  wms_client_processor_e_type      processor;
  wms_client_ch_pref_e_type        ch_pref;
  wms_dc_so_e_type                 so;
  boolean                          ch_pref_set;
  boolean                          send_wms_info;
  boolean                          follow_on_dc;

  uint32                           cfg_event_mask;
  uint32                           msg_event_mask;
  uint32                           dc_event_mask;
  uint32                           bc_event_mask;
  uint32                           bc_mm_event_mask;
  uint32                           dbg_event_mask;

  uint32                           dest_port;
  uint32                           source_port;

  wms_pm_notify_e_type             cfg_notify_type;
  wms_pm_notify_e_type             msg_notify_type;
  wms_pm_notify_e_type             dc_notify_type;
  wms_pm_notify_e_type             bc_notify_type;
  wms_pm_notify_e_type             bc_mm_notify_type;
  wms_pm_notify_e_type             dbg_notify_type;
} wms_client_s_type;



/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/
/*=========================================================================
FUNCTION wms_client_is_active_bearer                         (INTERNAL)

DESCRIPTION
     Deactivate the specified bearer according the the following criteria:

     1. bearer = WMS_BEARER_CDMA_EVDO
          Deactivate EVDO bearer if 1x voice call is active.
     2) bearer = WMS_BEARER_LTE
          Deactivate LTE bearer if there's no S102 tunnel enabled.

DEPENDENCIES
  Client must have been activated before calling this function.

RETURN VALUE
  TRUE - If the specified bearer is activated
  FALSE - If the specified bearer is deactived

SIDE EFFECTS
  No internal data changed if:
    1) client_poiter is NULL
    2) client has not been activated
    3) bearer value is not within range.

=========================================================================*/
boolean wms_client_active_bearer
(
  wms_client_s_type  *client_ptr,
  wms_bearer_e_type  bearer
);

/*===========================================================================
FUNCTION wms_client_ptr_fetch                                      (INTERNAL)

DESCRIPTION
  Return the pointer to a client's data, incrementing the reference count.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to client data.

SIDE EFFECTS
  None
===========================================================================*/
wms_client_s_type * wms_client_ptr_fetch
(
  wms_client_id_type client_id
);

/*=========================================================================
FUNCTION wms_client_update_channel_pref                          (INTERNAL)

DESCRIPTION
  Updates the channel preference for clients which have not specified
  a preference with the current preference default.

DEPENDENCIES
  Only clients initialized by calling wms_client_init() and have
  not had a channel preference set via wms_client_set_channel_pref()
  are updated.

RETURN VALUE
   - WMS_CLIENT_ERR_NONE
   - WMS_CLIENT_ERR_UNSUPPORTED

SIDE EFFECTS
   None
=========================================================================*/
wms_client_err_e_type wms_client_update_channel_pref(
  void
);

/*=========================================================================
FUNCTION wms_get_client_default_channel_pref

DESCRIPTION
   Get channel pref based on the current NV settings

DEPENDENCIES
  wms_nv_init() and wms_dc_init() must have been initialized first before
  calling this function.

RETURN VALUE
  default client channel preference

SIDE EFFECTS
  None
=========================================================================*/
wms_client_ch_pref_e_type wms_get_client_default_channel_pref(
  void
);

/*=========================================================================
FUNCTION wms_get_client_default_service_option

DESCRIPTION
   Get the dedicated channel service option based on the current NV settings

DEPENDENCIES
  wms_nv_init() and wms_dc_init() must have been initialized first before
  calling this function.

RETURN VALUE
  default client DC service option

SIDE EFFECTS
  None
=========================================================================*/
wms_dc_so_e_type wms_get_client_default_service_option(
  void
);

/*===========================================================================
FUNCTION wms_client_ptr_release                                    (INTERNAL)

DESCRIPTION
  Release the held pointer to client data, decrementing its reference count

DEPENDENCIES
  None

RETURN VALUE
  New reference count after this release

SIDE EFFECTS
  None
===========================================================================*/
void wms_client_ptr_release(
  wms_client_s_type *client_ptr
);

#define WMS_CLIENT_PTR_RELEASEIF(pClient) \
  { if (NULL != pClient) { wms_client_ptr_release(pClient); pClient = NULL; } }


/*=========================================================================
FUNCTION
  wms_cl_init

DESCRIPTION
  Initialize all client data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Internal data initialized.

=========================================================================*/
void wms_cl_init
(
  void
);

/*=========================================================================
FUNCTION
  wms_client_release_all

DESCRIPTION
  Release all client memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/

void wms_client_release_all(void);

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

#endif /* WMSCL_H */
