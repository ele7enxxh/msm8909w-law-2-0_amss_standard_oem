/*===========================================================================

           S Y S _ M . C

DESCRIPTION

Copyright (c) 2011- 15 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/sys_m/src/sys_m_qmi_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "sys_m.h"
#include "rex.h"
#include "rcinit.h"



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
    rex_sigs_type sigs;
    qmi_csi_os_params os_params_v02;
    qmi_csi_service_handle service_v02_handle;


    os_params_v02.tcb = rex_self();
    os_params_v02.sig = SYS_M_QMI_v02_SIG;
    
    service_v02_handle = ssctl_v02_qmi_ser_init(&os_params_v02);
    //block for start signal
    rcinit_handshake_startup();    

    //task forever loop
    for (;;)
    {
        sigs = rex_wait(SYS_M_QMI_v02_SIG);
        if (sigs & SYS_M_QMI_v02_SIG)
        {
            rex_clr_sigs(rex_self(), SYS_M_QMI_v02_SIG);
            qmi_csi_handle_event(service_v02_handle, &os_params_v02);
        }

    }
}
