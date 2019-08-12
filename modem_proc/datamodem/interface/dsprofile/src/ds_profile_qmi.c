/******************************************************************************
  @file    ds_profile_qmi.c
  @brief   DS PROFILE - QMI related code, which is not technology specific

  DESCRIPTION
  DS PROFILE - QMI related code, which is not technology specific

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/src/ds_profile_qmi.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies Incorporated. 
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
#ifdef FEATURE_DS_PROFILE_ACCESS_QMI

#include "customer.h"
#include "ds_profile.h"
#include "ds_profile_qmi.h"
#include "qmi.h"
#include "qmi_platform_config.h"
#include "ds_profile_os.h"
#include "ds_util.h"
#include "qmi_wds_srvc.h"
#include "ds_profile_server.h"

/*===========================================================================
FUNCTION ds_profile_qmi_cleanup

DESCRIPTION
  cleanup routine used to clean up qmi resources

PARAMETERS 
  qmi_handle : The QMI handle to be released
  qmi_wds_client_handle : The QMI WDS service handle to be released

DEPENDENCIES

RETURN VALUE
  void
 
SIDE EFFECTS

===========================================================================*/
void ds_profile_qmi_cleanup(int *qmi_handle, int *qmi_wds_client_handle)
{
  int qmi_err;

  /* Make sure qmi handle has been acquired */
  if (*qmi_handle < 0)
  {
    DS_PROFILE_LOGE("QMI message library was never initialized,"
        "invalid qmi handle. ", 0);
    return;
  }

  /* Release QMI WDS Service client */
  if (*qmi_wds_client_handle >= 0)
  {
    DS_PROFILE_LOGD("Releasing WDS qmi_client_handle 0x%08x",
                      *qmi_wds_client_handle);
    if ( qmi_wds_srvc_release_client( *qmi_wds_client_handle, &qmi_err ) < 0 )
    {
      DS_PROFILE_LOGE("Failed to release qmi_wds_client_handle 0x%08x", *qmi_wds_client_handle);
    }

    *qmi_wds_client_handle = -1;
  }

  /* Release qmi handle. In case modem_restart is in progress do not release the qmi handle.
   * This will allow the dsprofile module to receive system events when the modem is back in service */
  if (FALSE == ds_profile_server_ss_restart_in_action())
  {
    /* Release QMI library connection */
    qmi_release(*qmi_handle);
  }
}


/*===========================================================================
FUNCTION ds_profile_sys_event_hdlr

DESCRIPTION
  System event handler for taking care of Subsystem Restart events. This is a
  callback function which will be called from QMI context.
 
  This is a stub function.
  See //source/qcom/qct/modem/datahlos/qnx/rel/AP_11.01.01.02/data/interfaces/dsprofile/src/ds_profile_server.c
  for a real implementation for QNX HLOS.

PARAMETERS 
  event_id: Type of event
  event_info: Event associated info
  user_data: User data
 
DEPENDENCIES

RETURN VALUE
  void
 
SIDE EFFECTS

===========================================================================*/
void ds_profile_sys_event_hdlr(qmi_sys_event_type event_id, 
                               const qmi_sys_event_info_type *event_info, 
                               void *user_data)
{
}

/*===========================================================================
FUNCTION ds_profile_qmi_create_client

DESCRIPTION
  Utility function to get a QMI WDS client handle on a specific port.
 
PARAMETERS 
  port : Port name, for example QMI_PORT_RMNET_1.
  qmi_client_handle : Place holder for the QMI WDS client handle.

DEPENDENCIES

RETURN VALUE
  TRUE / FALSE
 
SIDE EFFECTS

===========================================================================*/
static int ds_profile_qmi_create_client(char* port, int *qmi_client_handle)
{
  int qmi_err;

  /* Initialize QMI port */ 
  if (qmi_dev_connection_init(port, &qmi_err) < 0)
  {
    DS_PROFILE_LOGE("ds_profile_qmi_init: QMI connection init failed with error = %d ", qmi_err);
    return FALSE;
  }

  /* Initialize WDS client, using the input port */
  if ( (*qmi_client_handle = qmi_wds_srvc_init_client(port,
        NULL, NULL, &qmi_err)) >= 0 )
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================
FUNCTION ds_profile_qmi_init

DESCRIPTION 
  Initialized QMI and QMI WDS service. 

PARAMETERS 
  sys_event_hdlr_func : Pointer to the system event handler which takes care of
    Subsystem Restart events.
  qmi_handle : Place holder for the QMI handle.
  qmi_client_handle : Place holder for the QMI WDS client handle

DEPENDENCIES

RETURN VALUE
  TRUE / FALSE
 
SIDE EFFECTS

===========================================================================*/
int ds_profile_qmi_init(void* sys_event_hdlr_func, 
                        int *qmi_handle, 
                        int *qmi_client_handle)
{
  /* Initialize QMI and acquire a handle, register an event handler to handle system
   * events related to modem restart */
  if (*qmi_handle < 0)
  {
    *qmi_handle = qmi_init(sys_event_hdlr_func, NULL);
    if (*qmi_handle < 0) 
    {
      printf("ds_profile_qmi_init: QMI message library init failed\n");
      DS_PROFILE_LOGE("ds_profile_qmi_init: QMI message library init failed", 0);
      return FALSE;
    }
  }

  /* First try SMD-Lite connection */
  if ( TRUE == ds_profile_qmi_create_client(QMI_PORT_RMNET_1, qmi_client_handle) )
  {
    printf("ds_profile_qmi_init: WDS client initialized with an SMD port\n");
    DS_PROFILE_LOGD("ds_profile_qmi_init: WDS client initialized with an SMD port ", 0);
    return TRUE;
  }

  /* In case SMD connections fails, one may try to establish a connection through another port */

  /* In case we got here initialization failed */
  printf("ds_profile_qmi_init: Failed initialization of WDS client\n");
  DS_PROFILE_LOGD("ds_profile_qmi_init: Failed initialization of WDS client", 0);
  return FALSE;
}

#endif /* #ifdef FEATURE_DS_PROFILE_ACCESS_QMI */
