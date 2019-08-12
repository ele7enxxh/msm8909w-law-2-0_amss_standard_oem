/*=============================================================================
  Copyright (c) 2003-2013,2015 Qualcomm Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/**
@file qmi_time_server_rex.c
@brief
Task loop for qmi_timer_server module.
*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time_qmi/src/qmi_time_server_rex.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/23/15   dks     Replaced qmi core framework with qcsi calls

=============================================================================*/

#include "qmi_time_server.h"
#include "ats_v.h"
#include "rcinit.h"


void time_srv_task(dword param)
{
   rex_sigs_type      wait_on,sig_received;
   qmi_csi_os_params  os_params;
   time_server* time_ser_handle;
   time_server_error rc;

   (void) param;

   memset(&os_params, 0, sizeof(os_params));
   os_params.tcb = rex_self();
   os_params.sig = QMI_TIME_SVC_WAIT_SIG;

   rc = time_server_init(&time_ser_handle, &os_params);

   rcinit_handshake_startup();

   if(rc == TIME_SERVER_NO_ERROR)
   {
      wait_on = QMI_TIME_SVC_WAIT_SIG | QMI_TIME_SVC_STOP_SIG;

      while(1) {
          sig_received = rex_wait(wait_on);
          rex_clr_sigs(os_params.tcb, sig_received);
          switch(sig_received)
          {
              case QMI_TIME_SVC_WAIT_SIG:
                 qmi_csi_handle_event(time_ser_handle->service_handle,
                                      &os_params);
                 break;

              case QMI_TIME_SVC_STOP_SIG:
                 time_server_unregister(time_ser_handle);
                 break;

              default:
                 ATS_ERR_1("QMI Time Srv received unknown signal %d",
                             sig_received);
         }
      }
   }
}

