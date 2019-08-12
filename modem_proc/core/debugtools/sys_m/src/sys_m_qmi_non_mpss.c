/*===========================================================================

           S Y S _ M . C

DESCRIPTION

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/sys_m/src/sys_m_qmi_non_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "sys_m.h"
#include "rcinit.h"
#include "qmi_csi.h"
#include "qurt.h"

qurt_anysignal_t qmi_ssctl_ser_sig;

/**
 *
 * @brief sys_m_qmi_task
 *
 * System Monitor main task
 *
 * @param param: Task init parameter
 *
*/
void sys_m_qmi_task(dword param)
{

   unsigned int mask_fired=0;
   qmi_csi_os_params os_params_v02;
   qmi_csi_service_handle service_handle_SSCTL_v02;

   os_params_v02.sig = SYS_M_QMI_v02_SIG;
   os_params_v02.signal = &qmi_ssctl_ser_sig;

   qurt_anysignal_init(&qmi_ssctl_ser_sig);

   service_handle_SSCTL_v02 = ssctl_v02_qmi_ser_init(&os_params_v02);
   //block for start signal
   rcinit_handshake_startup();    

  //task forever loop
    for (;;)
    {
    
        mask_fired = qurt_anysignal_wait(os_params_v02.signal, SYS_M_QMI_v02_SIG);
        if(mask_fired & SYS_M_QMI_v02_SIG )
        {
           qurt_anysignal_clear(os_params_v02.signal, SYS_M_QMI_v02_SIG);
           qmi_csi_handle_event(service_handle_SSCTL_v02, &os_params_v02);
        }

    }
}
