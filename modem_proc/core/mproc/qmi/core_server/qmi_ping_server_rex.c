#include "qmi_ping_server.h"
#include "core_server_platform.h"
/*---------------------------------------------------------------------------
   QMI_PING_SERVER_REX.C
---------------------------------------------------------------------------*/
/*!
  @file
    qmi_ping_server_rex.c

  @brief
    This file contains the Rex implementation of the ping service.

  ---------------------------------------------------------------------------
  Copyright (c) 2011 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/mproc/qmi/core_server/qmi_ping_server_rex.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 

===========================================================================*/

extern void ping_server_handle_event
(
  ping_server *me,
  void *event
);

void ping_server_event_loop(dword param)
{
   rex_sigs_type wait_on,sig_received;
   qmi_csi_os_params         *os_params; 

   ping_server     *ping_srv_obj;
   ping_srv_obj =  (ping_server *)param;
   os_params = ping_server_get_os_params(ping_srv_obj);
   wait_on = QMI_PING_SVC_WAIT_SIG;

   while (1) {
       sig_received = rex_wait(wait_on);
       rex_clr_sigs(os_params->tcb,sig_received);
       ping_server_handle_event(ping_srv_obj,
                                (void *)sig_received);
   }
}



