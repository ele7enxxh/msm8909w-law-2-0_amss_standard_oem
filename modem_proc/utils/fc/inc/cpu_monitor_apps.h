#ifndef CPU_MONITOR_APPS_H__
#define CPU_MONITOR_APPS_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               C P U    M O N I T O R     H E A D E R    F I L E

DESCRIPTION
  Public Header file for the CPU Monitor. Contains types and externalized
  functions of the Flow Controller module.

Copyright (c) 2000-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/fc/inc/cpu_monitor_apps.h#1 $ $DateTime: 2016/12/13 07:57:51 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------------------
06/23/09    pj       Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "cpu_monitor.h"

/*
  Call-back type
*/
typedef boolean (fc_update_remote_cpm_clnt_cb_type)(
                  fc_register_type     reg_data, 
                  fc_out_cmd_enum_type out_cmd,
                  uint8                cpu_load_val);
/*~ CALLBACK fc_update_remote_cpm_clnt_cb_type
    ONERROR return FALSE */

void fc_register_callback_func (fc_update_remote_cpm_clnt_cb_type *);
/*~ FUNCTION fc_register_callback_func 
    CLNTNAME fc_apps_register_callback_func */

void fc_modem_register_callback_func (fc_update_remote_cpm_clnt_cb_type *);

void fc_remote_cpu_register (fc_processor_type    clnt_cpu_id, 
                             fc_in_cmd_data_type  cmd_data);
/*~ FUNCTION fc_remote_cpu_register
    CLNTNAME fc_apps_cpu_register   */

void fc_modem_cpu_register (fc_processor_type    clnt_cpu_id, 
                             fc_in_cmd_data_type  cmd_data);


void fc_remote_cpu_deregister (fc_in_cmd_data_type cmd_data);
/*~ FUNCTION fc_remote_cpu_deregister
    CLNTNAME fc_apps_cpu_deregister */

void fc_modem_cpu_deregister (fc_in_cmd_data_type cmd_data);

void fc_remote_process_measurement_req_cmd (fc_processor_type clnt_cpu_id, 
                                            fc_in_cmd_data_type cmd_data);
/*~ FUNCTION fc_remote_process_measurement_req_cmd
    CLNTNAME fc_apps_process_measurment_req_cmd */


void fc_modem_process_measurment_req_cmd (fc_processor_type clnt_cpu_id, 
                                              fc_in_cmd_data_type cmd_data);
#endif /* CPU_MONITOR_APPS_H__*/
