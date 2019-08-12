#ifndef DS3G_QMI_NAS_CLIENT_H
#define DS3G_QMI_NAS_CLIENT_H
/*===========================================================================
             D S 3 G _ Q M I _ N A S _ C L I E N T _ I . H

DESCRIPTION


EXTERNALIZED FUNCTIONS


 Copyright (c)2011 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3g_qmi_nas_client.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/02/12    var    Initial revision
 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_8960_SGLTE_FUSION
#include "sys.h"
#include "dstask_v.h"
#include "qmi_client.h"
#include "ds_dsd_ext_i.h"
#include "network_access_service_v01.h"
#include "rex.h"

/*===========================================================================

                        EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_INIT

DESCRIPTION    Initializes the QMI-NAS client during power-up and register 
               with QMI NAS service for system information indication

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds3g_qmi_nas_client_init(void);

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_PROCESS_SIGNALS

DESCRIPTION    Funtion to process NAS service notify signal. This function is 
               used to process signal sent to DS task by QCCI notifying NAS
               service.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Signal DS_NOTIFY_NAS_SVC_SIG will be cleared
===========================================================================*/
void ds3g_qmi_nas_client_process_signal(
  rex_sigs_type    set_sigs /* Mask containing set signals */
);

/*===========================================================================

                        LOCAL FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_IND_CB

DESCRIPTION    QMI-NAS indication callback handler. This function is called 
               by QCCI whenver there is a NAS indication message.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void  ds3g_qmi_nas_client_ind_cb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf_ptr,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
);

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_RECV_GET_SYS_INFO_RESP

DESCRIPTION    QMI-NAS receive GET_SYS_INFO_RESP message handler. This 
               callback is called by QCCI in response to async NAS message
               QMI_NAS_GET_SYS_INFO_REQ message.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Release the memory assigned for GET_SYS_INFO_RESP message
===========================================================================*/
void ds3g_qmi_nas_client_recv_get_sys_info_resp
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                          *resp_c_struct,
 unsigned int                   resp_c_struct_len,
 void                          *resp_cb_data,
 qmi_client_error_type          transp_err
);

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_RECV_IND_REG_RESP

DESCRIPTION    QMI-NAS receive IND_REG_RESP message handler. This callback 
               is called by QCCI in response to async NAS message
               QMI_NAS_INDICATION_REGISTER_REQ message.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   Release the memory assigned for IND_REG_RESP message
===========================================================================*/
void ds3g_qmi_nas_client_recv_ind_reg_resp
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *resp_c_struct,
 unsigned int                   resp_c_struct_len,
 void                           *resp_cb_data,
 qmi_client_error_type          transp_err
);

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_ERROR_CB

DESCRIPTION    QMI NAS error callback handler. This callback is called by QCCI 
               to notify error.

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
void ds3g_qmi_nas_client_error_cb
(
  qmi_client_type user_handle,
  qmi_client_error_type error,
  void *err_cb_data
);

/*===========================================================================
FUNCTION       DS3G_QMI_NAS_CLIENT_CONV_SRV_STATUS_TO_RADIO_EVENT

DESCRIPTION    Utility function to covert NAS Service status type to 
               DSD Radio event type.

DEPENDENCIES   NONE

RETURN VALUE   ds_dsd_client_radio_event_type

SIDE EFFECTS   NONE
===========================================================================*/
ds_dsd_client_radio_event_type 
ds3g_qmi_nas_client_conv_srv_domain_to_radio_event
(
  nas_service_domain_enum_type_v01 srv_domain
);

/*===========================================================================
FUNCTION      DS3G_QMI_NAS_CLIENT_IS_DSD_SYNC_ENABLED

DESCRIPTION    Utility to to check if DSD sync is enabled

DEPENDENCIES  None

RETURN VALUE  TRUE:  DSD Sync is enabled.
              FALSE: DSD Sync is disabled.

SIDE EFFECTS
===========================================================================*/
boolean ds3g_qmi_nas_client_is_dsd_sync_enabled(void);

#endif /* FEATURE_8960_SGLTE_FUSION */
#endif /* DS3G_QMI_NAS_CLIENT_H */


