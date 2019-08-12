/*===========================================================================

           SYS_M_RESET_MPSS. C

DESCRIPTION

Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/sys_m/src/sys_m_reset_mpss.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------


===========================================================================*/

#include "sys_m_reset.h"
#include "sys_m.h"
#include "qmi_ssreq_client.h"
#include "sys_m_smsm.h"
#include "qurt.h"
#include "subsystem_request_v01.h"
#include "rcecb.h"

static qurt_mutex_t sys_m_lock_mutex;

static int sys_m_lock_inited = 0;

/**
 *
 * @brief SYS_M_LOCK_INIT
 *
 * Initialize sys_m mutex
 *
*/
void SYS_M_LOCK_INIT(void)
{
   if (!sys_m_lock_inited)
   {
      qurt_pimutex_init(&sys_m_lock_mutex);
      sys_m_lock_inited = 1;
   }
}

/**
 *
 * @brief SYS_M_LOCK
 *
 * Lock sys_m mutex
 *
*/
void SYS_M_LOCK(void)
{
   if (sys_m_lock_inited)
   {
      qurt_pimutex_lock(&sys_m_lock_mutex);
   }
}

/**
 *
 * @brief SYS_M_TRY_LOCK
 *
 * Lock sys_m mutex
 *
*/
int SYS_M_TRY_LOCK(void)
{
   int result = 1; /*since 0 is success , init with non-zero*/
   if (sys_m_lock_inited)
   {
      result =  qurt_pimutex_try_lock(&sys_m_lock_mutex);
   }
   return result;
}

/**
 *
 * @brief SYS_M_UNLOCK
 *
 * Unlock sys_m mutex
 *
*/
void SYS_M_UNLOCK(void)
{
   if (sys_m_lock_inited)
   {
      qurt_pimutex_unlock(&sys_m_lock_mutex);
   }
}



/**
 *
 * @brief sys_m_init
 *
 * System monitor initialization function
 *
*/
void sys_m_init(void)
{
   SYS_M_LOCK_INIT();
}


/**
 *
 * @brief sys_m_initiate_shutdown
 *
 * Client Exposed API
 * Request apps for a reset(restart) of the device.
 *
 *
*/
void sys_m_initiate_shutdown(void)
{

   SYS_M_LOCK(); // Concurrency protection
   SYS_M_MSG_HIGH_1("Enter sys_m_initiate_shutdown from task_tid contex :", rex_self());

   /*check if the shutdonw request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/
   if((ssreq_ser_check_for_shutdown_support())&&(TRUE == get_ssreq_client_init_status()))
   {
       ssreq_status_t ret;
       ret = ssreq_process_request(QMI_SSREQ_SYSTEM_RESTART_REQ_V01, NULL,SSREQ_QMI_RES_NONE);
       SYS_M_MSG_HIGH_2("sys_m_initiate_shutdown using SSREQ QMI ret = %d, task_tid 0x%x", ret, rex_self());
   }
   // Check for the case where a forwarding callback has been registered. Specific use
   // case example is HLOS to MPSS has established a AT Command Forward channel that
   // is utilized to forward SHUTDOWN REQUEST back to HLOS.
   else if (0 < rcecb_getregistrants_name(SYS_M_RCECB_FWD_SHUTDOWN)) // count of registrants
   {
       SYS_M_MSG_HIGH_1("sys_m_initiate_shutdown forward request task_tid %x", rex_self());
       if(rcecb_signal_name(SYS_M_RCECB_FWD_SHUTDOWN)){} // signal executes callbacks for registrants
   }
   // Else the behavior is normal standalone behavior. Specific use case example
   // is that the APPS_TN gets the command request by SMP2P state bits.
   else
   {
      int32 ret;
      ret = sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_SHUT_DWN);
      SYS_M_MSG_HIGH_1("sys_m_initiate_shutdown over smp2p  ret =%d", ret);
   }

   SYS_M_UNLOCK();
}

/**
 *
 * @brief sys_m_initiate_poweroff
 *
 * Client Exposed API
 * Request apps for a poweroff(shutdown) of the device
 *
 *
 *
*/
void sys_m_initiate_poweroff(void)
{

   SYS_M_LOCK(); // Concurrency protection

   SYS_M_MSG_HIGH_1("Enter sys_m_initiate_poweroff from task_tid contex :", rex_self());

   /*check if the shutdonw request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/
   if((ssreq_ser_check_for_shutdown_support())&&(TRUE == get_ssreq_client_init_status()))
   {
       ssreq_status_t ret;
       ret = ssreq_process_request(QMI_SSREQ_SYSTEM_SHUTDOWN_REQ_V01, NULL, SSREQ_QMI_RES_NONE);
       SYS_M_MSG_HIGH_2("sys_m_initiate_poweroff using SSREQ QMI ret = %d, task_tid 0x%x", ret, rex_self());
   }
   // Check for the case where a forwarding callback has been registered. Specific use
   // case example is HLOS to MPSS has established a AT Command Forward channel that
   // is utilized to forward SHUTDOWN REQUEST back to HLOS.
   else if (0 < rcecb_getregistrants_name(SYS_M_RCECB_FWD_POWEROFF)) // count of registrants
   {
       SYS_M_MSG_HIGH_1("sys_m_initiate_poweroff forward request task_tid %x", rex_self());
      if(rcecb_signal_name(SYS_M_RCECB_FWD_POWEROFF)){} // signal executes callbacks for registrants
   }
   // Else the behavior is normal standalone behavior. Specific use case example
   // is that the APPS_TN gets the command request by SMP2P state bits.
   else
   {
      int32 ret;
      ret = sys_m_smsm_apps_set(SYS_M_SUBSYS2AP_SMSM_PWR_OFF);
      SYS_M_MSG_HIGH_1("sys_m_initiate_poweroff over smp2p ret = %x", ret);

   }

   SYS_M_UNLOCK();
}

/**
  Initiates reset of the system using SSREQ QMI

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_initiate_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn)
{
   ssreq_status_t ssreq_ret = SSREQ_FAILURE;

   int res = SYS_M_TRY_LOCK(); // Concurrency protection
   if (res != 0)
      return SSREQ_IN_PROCESS;

   /*check if the shutdonw request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/

   if(TRUE == get_ssreq_client_init_status())
   {
       ssreq_ret = ssreq_process_request(QMI_SSREQ_SYSTEM_RESTART_REQ_V01, ind_cb_fn,SSREQ_QMI_RES_NONE);
       SYS_M_MSG_HIGH_2("sys_m_initiate_restart_ssreq using SSREQ QMI ret = %d, task_tid 0x%x", ssreq_ret, rex_self());
   }
   SYS_M_UNLOCK();
   return ssreq_ret;
}

/**
  Initiates the peripheral restart using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function(ind_cb_fn) if provided
  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_request_peripheral_restart_ssreq(ssreq_qmi_ind_cb_fn_type *ind_cb_fn)
{
   ssreq_status_t ssreq_ret = SSREQ_FAILURE;

   int res = SYS_M_TRY_LOCK(); // Concurrency protection
   if (res != 0)
      return SSREQ_IN_PROCESS;

   /*check if the shutdown request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/
   if(TRUE == get_ssreq_client_init_status())
   {
       ssreq_ret = ssreq_process_request(QMI_SSREQ_PERIPHERAL_RESTART_REQ_V01, ind_cb_fn,SSREQ_QMI_RES_NONE);
       SYS_M_MSG_HIGH_2("sys_m_request_peripheral_restart_ssreq ret = %d, task_tid 0x%x", ssreq_ret, rex_self());
   }
   SYS_M_UNLOCK();
   return ssreq_ret;
}

/**
  Initiates reset of the system using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function (ind_cb_fn)  if provided
  In addition provide the reason code(res_code) to the service for SSREQ request initiation

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_init_restart_ssreq_with_res_code(
   ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
   ssreq_qmi_request_res_code_type res_code
)
{
   ssreq_status_t ssreq_ret = SSREQ_FAILURE;


   int res = SYS_M_TRY_LOCK(); // Concurrency protection
   if (res != 0)
      return SSREQ_IN_PROCESS;

   /*check if the shutdonw request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/
   if(TRUE == get_ssreq_client_init_status())
   {
       ssreq_ret = ssreq_process_request(QMI_SSREQ_SYSTEM_RESTART_REQ_V01, ind_cb_fn, res_code);
       SYS_M_MSG_HIGH_3("sys_m_init_restart_ssreq_with_res_code ret = %d, task_tid 0x%x, res_code = %d", ssreq_ret, rex_self(), res_code);
   }
   SYS_M_UNLOCK();
   return ssreq_ret;
}
/**
  Initiates the peripheral restart using SSREQ QMI
  And notify about the indication responce status to 
  the user of this API via its callback function(ind_cb_fn) if provided
  In addition provide the reason code(res_code) to the service for SSREQ request initiation

  @return
   SSREQ_SUCCESS
   SSREQ_FAILURE
   SSREQ_IN_PROCESS

  @dependencies
  None.
*/
ssreq_status_t sys_m_req_pp_restart_ssreq_with_res_code(
   ssreq_qmi_ind_cb_fn_type *ind_cb_fn,
   ssreq_qmi_request_res_code_type res_code
)
{
   ssreq_status_t ssreq_ret = SSREQ_FAILURE;

   int res = SYS_M_TRY_LOCK(); // Concurrency protection
   if (res != 0)
      return SSREQ_IN_PROCESS;

   /*check if the shutdonw request can be forwarded to APSS(HLOS or TN ) using SSREQ QMI*/
   if(TRUE == get_ssreq_client_init_status())
   {
       ssreq_ret = ssreq_process_request(QMI_SSREQ_PERIPHERAL_RESTART_REQ_V01, ind_cb_fn, res_code);
       SYS_M_MSG_HIGH_3("sys_m_req_pp_restart_ssreq_with_res_code ret = %d, task_tid 0x%x, res_code = %d", ssreq_ret, rex_self(), res_code);
   }
   SYS_M_UNLOCK();
   return ssreq_ret;
}


