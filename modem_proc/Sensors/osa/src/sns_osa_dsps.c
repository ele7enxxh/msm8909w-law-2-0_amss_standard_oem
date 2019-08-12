/*============================================================================
  @file sns_osa_dsps.c

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/osa/src/sns_osa_dsps.c#1 $
$DateTime: 2016/12/13 08:00:23 $
$Author: mplcsds1 $

when         who     what, where, why
----------   --- ---------------------------------------------------------
  2016-06-22 kn  klockwork warning fix
  2016-02-25 gju Use SNS_PRINTF_STRING macros rather than MSG macros directly
  2016-01-21 gju Use SNS_OS_MALLOC/FREE rather than sns_heap directly
  2015-10-02 hw  Remove sns_os_task_create which is not usable anymore
  2014-09-17 ps  Remove qurt_elite dependency
  2014-06-05 vy  Uses SNS_OS_U_MALLOC in sns_os_sigs_create
  2014-04-23 pn  Added implementation for sns_os_sigs_del()
  2013-10-08 sc  Fixed an issue with OS_FLAG_CLR option in sns_os_sigs_post().
  2013-10-03 sc  On behalf of Amithash from QMI team: use native qurt signal
                 instead of qurt_elite channel/signal. This is to temporarily
                 address the incompatibility recently introduced between
                 qurt_elite and QMI API.
  2013-05-21 br  Inserted appropriated functionality into sns_os_mutex_del()
  2013-05-07 sc  Address klocwork errors; check for NULL pointers
  2012-11-15 ps  Change heap ID from QURT_ELITE_HEAP_DEFAULT to sns_heap_id
  2012-08-04 ag  Bug fixes in post and pend wrappers
  2012-07-11 sc  Fixed the stack size passed into qurt thread create call
  2012-07-06 sc  Implementation for sns_os_set_qmi_csi_params() on QDSP6
  2011-02-24 br  Inserted sns_os_time_dly() and changed sns_os_task_create_ext()

============================================================================*/

/*=====================================================================
  INCLUDES
  =======================================================================*/
#include "sns_memmgr.h"
#include "sns_debug_str.h"
#include "sns_osa.h"

/*=====================================================================
  STATIC DATA
  =======================================================================*/
static qurt_thread_t thread_id_ref[256];

/*=====================================================================
  PUBLIC FUNCTIONS
  =======================================================================*/

uint8_t       sns_os_task_create           (void           (*task)(void *p_arg),
                                            void            *p_arg,
                                            OS_STK          *ptos,
                                            uint8_t          prio)
{
  uint8_t result;

  result = sns_os_task_create_ext(p_arg, p_arg, ptos, prio, 0, NULL, 0x100, NULL, 0, NULL);

  MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, "Thread %d of priority %d created with result %d",
        thread_id_ref[prio], prio, result);

  return result;
}

uint8_t       sns_os_task_create_ext       (void           (*task)(void *p_arg),
                                            void            *p_arg,
                                            OS_STK          *ptos,
                                            uint8_t          prio,
                                            uint16_t         id,
                                            OS_STK          *pbos,
                                            uint32_t         stk_size,
                                            void            *pext,
                                            uint16_t         opt,
                                            uint8_t          *name)
{
  qurt_thread_attr_t attr;
  int ret;
  char *stack_ptr;
  int new_stk_size;
  uint8_t new_prio;
  uint32_t* stack_fill;

  new_stk_size = stk_size * 4;

  // boundary checks
  if (256 > stk_size) return OS_ERR_INVALID_OPT;

  // if no stack has been provided, allocate it.
  if (!pbos)
  {
     // round up stack size
    new_stk_size = (new_stk_size + 7) & (-8);
    if(NULL == (stack_ptr = (char *) sns_heap_malloc(new_stk_size)))
    {
      return OS_ERR_MEM_FULL;
    }
  }
  else
  {
    stack_ptr = (char *)pbos;
    new_stk_size = new_stk_size & (-8);
  }

  // fill stack with a pattern.
  for (stack_fill = (uint32_t*) (stack_ptr);
       stack_fill < (uint32_t*) (stack_ptr + new_stk_size);
       stack_fill += 16)
  {
    *stack_fill = 0xF8F8F8F8L;
  }

  // reverse the priority so that we are in line with POSIX- higher number = higher priority
  new_prio = 255 - prio;

  qurt_thread_attr_init (&attr);
  qurt_thread_attr_set_stack_size (&attr, new_stk_size);
  qurt_thread_attr_set_stack_addr (&attr, stack_ptr);
  qurt_thread_attr_set_priority (&attr, (unsigned short)new_prio);

  if (NULL != name)
  {
    qurt_thread_attr_set_name(&attr, (char *)name);
  }

#ifdef SNS_USES_ISLAND
  if (opt & OS_TASK_OPT_ISLAND)
  {
    qurt_thread_attr_set_tcb_partition(&attr,
                                       QURT_THREAD_ATTR_TCB_PARTITION_TCM);
  }
#endif

  ret  = qurt_thread_create(&thread_id_ref[prio],
                            &attr,
                            task,
                            NULL);


  SNS_PRINTF_STRING_HIGH_3(SNS_DBG_MOD_DSPS_OSA,
      "Thread %d of priority %d created with result %d",
      thread_id_ref[prio], prio, ret);

  return ret;
}

OS_FLAG_GRP  *sns_os_sigs_create           (OS_FLAGS         flags,
                                            uint8_t         *perr)
{
  //add safety feature that returns perr = NULL when anything fails
  OS_FLAG_GRP *flag_to_return;

  if (perr == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_OSA, "error pointer is NULL");
    return NULL;
  }

  flag_to_return = SNS_OS_U_MALLOC(SNS_DBG_MOD_OSA, (sizeof(OS_FLAG_GRP)));

  if (flag_to_return == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_OSA, "signal group create fail");
    *perr = OS_ERR_MEM_FULL;
    return NULL;
  }
  qurt_anysignal_init(&flag_to_return->signal);
  flag_to_return->unChannelWaitMask = flags;
  *perr = OS_ERR_NONE;
  return flag_to_return;
}


void         sns_os_set_qmi_csi_params    (OS_FLAG_GRP       *pgrp,
                                           OS_FLAGS           flags,
                                           qmi_csi_os_params *os_params,
                                           uint8_t           *perr)
{
  if (pgrp == NULL)
  {
    *perr = OS_ERR_FLAG_INVALID_PGRP;
    return;
  }
  memset(os_params,0,sizeof(*os_params));
  os_params->signal = &pgrp->signal;
  os_params->sig = flags;

  *perr = OS_ERR_NONE;
}

sig_node    *sns_os_sigs_del              (OS_FLAG_GRP     *pgrp,
                                           OS_FLAGS         flags)
{
  if (pgrp == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_OSA, "signal group pointer is NULL");
    return NULL;
  }

  pgrp->unChannelWaitMask &= ~flags;

  return &(pgrp->unChannelWaitMask); /* Not used anymore */
}


OS_FLAGS      sns_os_sigs_accept           (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          wait_type,
                                            uint8_t         *perr)
{
  uint32_t result;
  result = qurt_anysignal_get(&pgrp->signal);
  *perr = OS_ERR_NONE;
  return result & flags; /* XXX ?*/
}

OS_EVENT     *sns_os_mutex_create          (uint8_t          prio,
                                            uint8_t         *perr)
{
  OS_EVENT *mutex_event = sns_heap_malloc(sizeof(OS_EVENT));

  if (mutex_event == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_OSA, "mutex create fail");
    *perr = OS_ERR_MEM_FULL;
    return NULL;
  }

  qurt_rmutex_init(&(mutex_event->mutex));

  *perr = OS_ERR_NONE;
  return mutex_event;
}

OS_EVENT     *sns_os_mutex_del             (OS_EVENT        *pevent,
                                            uint8_t          opt,
                                            uint8_t         *perr)
{
  if ( NULL == pevent || NULL == perr )
  {
    if ( NULL != perr )
    {
      *perr = OS_ERR_PDATA_NULL;
    }
    return pevent;
  }
  else
  {
    qurt_rmutex_destroy(&(pevent->mutex));
    sns_heap_free(pevent);
    *perr = OS_ERR_NONE;
    return (OS_EVENT *)0;
  }
}


#ifdef SNS_DSPS_PROFILE_ON
void sns_os_time_dly( uint16_t ticks)
{
  OSTimeDly(ticks);
}
#endif
