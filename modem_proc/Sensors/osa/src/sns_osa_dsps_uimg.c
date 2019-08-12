/*============================================================================
  @file sns_osa_dsps_uimg.c

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/osa/src/sns_osa_dsps_uimg.c#1 $
$DateTime: 2016/12/13 08:00:23 $
$Author: mplcsds1 $

when         who     what, where, why
----------   ---     ---------------------------------------------------------
02/26/2016   gju    Use SNS_PRINTF_STRING macros rather than MSG macros directly
10/26/2014   pn     Made last param to sns_os_mutex_pend() optional
08/24/2014   vy     Replaced MSG_x by UMSG_x
06/05/2014   vy     Moved sns_osa functions to uImage

============================================================================*/

/*=====================================================================
  INCLUDES
  =======================================================================*/
#include "sns_memmgr.h"
#include "sns_debug_str.h"
#include "sns_osa.h"

/*=====================================================================
  PUBLIC FUNCTIONS
  =======================================================================*/

sig_node    *sns_os_sigs_add              (OS_FLAG_GRP     *pgrp,
                                           OS_FLAGS         flags)
{
  if (pgrp == NULL)
  {
    //SNS_UMSG_ERROR_0(SNS_DBG_MOD_DSPS_OSA, "signal group pointer is NULL");
    return NULL;
  }

  pgrp->unChannelWaitMask |= flags;

  return &(pgrp->unChannelWaitMask); /* Not used anymore */
}

OS_FLAGS      sns_os_sigs_pend             (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          wait_type,
                                            uint32_t         timeout,
                                            uint8_t         *perr)
{
  uint32_t result = 0;

  // block on channel to wait
  qurt_anysignal_wait(&pgrp->signal, pgrp->unChannelWaitMask);
  result = qurt_anysignal_get(&pgrp->signal);
  // when it receives, clear and exit

  *perr = OS_ERR_EVENT_TYPE;
  qurt_anysignal_clear(&pgrp->signal, result);
  *perr = OS_ERR_NONE;

  return result;
}


OS_FLAGS      sns_os_sigs_post             (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          opt,
                                            uint8_t         *perr)
{
  uint8_t os_err = OS_ERR_NONE;

  if (pgrp == NULL)
  {
    //SNS_UMSG_ERROR_0(SNS_DBG_MOD_DSPS_OSA, "pgrp is NULL");
    return 0;
  }

  if (opt == OS_FLAG_SET)
  {
    qurt_anysignal_set(&pgrp->signal, flags);
  }
  else if (opt == OS_FLAG_CLR)
  {
    qurt_anysignal_clear(&pgrp->signal, flags);
  }
  else
  {
    os_err = OS_ERR_INVALID_OPT;
  }

  if (perr != NULL)
  {
    *perr = os_err;
  }
  return flags;
}

void          sns_os_mutex_pend            (OS_EVENT        *pevent,
                                            uint32_t         timeout,
                                            uint8_t         *perr)
{
  qurt_rmutex_lock(&(pevent->mutex));
  if (perr != NULL)
  {
    *perr = OS_ERR_NONE;
  }
}

uint8_t       sns_os_mutex_post            (OS_EVENT        *pevent)
{
  qurt_rmutex_unlock(&(pevent->mutex));
  return 0;
}

OS_EVENT     *sns_os_mutex_create_uimg     (uint8_t          prio,
                                            uint8_t         *perr)
{
  OS_EVENT *mutex_event = SNS_OS_U_MALLOC(SNS_DBG_MOD_OSA, (sizeof(OS_EVENT)));

  if (mutex_event == NULL)
  {
    //SNS_UMSG_ERROR_0(SNS_DBG_MOD_DSPS_OSA, "mutex create fail");
    *perr = OS_ERR_MEM_FULL;
    return NULL;
  }

  qurt_rmutex_init(&(mutex_event->mutex));

  *perr = OS_ERR_NONE;
  return mutex_event;
}
