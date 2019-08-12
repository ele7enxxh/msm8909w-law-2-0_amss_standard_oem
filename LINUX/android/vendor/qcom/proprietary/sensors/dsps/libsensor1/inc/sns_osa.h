#ifndef _SNS_OSA_H_
#define _SNS_OSA_H_
/*============================================================================
  @file sns_osa.h

  @brief
  OS Abstraction layer for sensors.

  This is a common header file; however, each processor & target OS will have
  a specific implementation.

  <br><br>

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*=====================================================================
  INCLUDES
  =======================================================================*/
#if defined (SNS_LA)
#include <includes.h>
#else
#include "os_qcom_port.h"
#endif
#include "sensor1.h"

/*=====================================================================
  INTERNAL FUNCTION PROTOTYPES
  =======================================================================*/
/* Task */
uint8_t       sns_os_task_create           (void           (*task)(void *p_arg),
                                            void            *p_arg,
                                            OS_STK          *ptos,
                                            uint8_t          prio);

uint8_t       sns_os_task_create_ext       (void           (*task)(void *p_arg),
                                            void            *p_arg,
                                            OS_STK          *ptos,
                                            uint8_t          prio,
                                            uint16_t         id,
                                            OS_STK          *pbos,
                                            uint32_t         stk_size,
                                            void            *pext,
                                            uint16_t         opt,
                                            uint8_t          *name);

uint8_t       sns_os_task_del              (uint8_t          prio);
uint8_t       sns_os_task_del_req          (uint8_t          prio);

uint8_t       sns_os_task_once             (OS_THREAD       *thread,
                                            void (*task)(void));

/* Flag */

OS_FLAG_GRP  *sns_os_sigs_create           (OS_FLAGS         flags,
                                            uint8_t         *perr);

OS_FLAG_GRP  *sns_os_sigs_del              (OS_FLAG_GRP     *pgrp,
                                            uint8_t          opt,
                                            uint8_t         *perr);

OS_FLAGS      sns_os_sigs_pend             (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          wait_type,
                                            uint32_t         timeout,
                                            uint8_t         *perr);

OS_FLAGS      sns_os_sigs_post             (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          opt,
                                            uint8_t         *perr);


OS_FLAGS      sns_os_sigs_accept           (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags,
                                            uint8_t          wait_type,
                                            uint8_t         *perr);

void         *sns_os_sigs_add              (OS_FLAG_GRP     *pgrp,
                                            OS_FLAGS         flags);

/* Mutex */

OS_EVENT     *sns_os_mutex_create          (uint8_t          prio,
                                            uint8_t         *perr);

OS_EVENT     *sns_os_mutex_del             (OS_EVENT        *pevent,
                                            uint8_t          opt,
                                            uint8_t         *perr);

void          sns_os_mutex_pend            (OS_EVENT        *pevent,
                                            uint32_t         timeout,
                                            uint8_t         *perr);

uint8_t       sns_os_mutex_post            (OS_EVENT        *pevent);

OS_EVENT     *sns_os_mutex_create_uimg     (uint8_t          prio,
                                            uint8_t         *perr);

/* Semaphore */

OS_EVENT*     sns_os_sem_create            (unsigned int     value,
                                            uint8_t         *perr);

void          sns_os_sem_del               (OS_EVENT        *sem);

void          sns_os_sem_post              (OS_EVENT        *sem,
                                            uint8_t         *perr);

void          sns_os_sem_wait              (OS_EVENT        *sem,
                                            const struct timespec *timeout,
                                            uint8_t         *perr);

#ifdef SNS_DSPS_PROFILE_ON
void          sns_os_time_dly( uint16_t ticks);
#endif

#endif /* _SNS_OSA_H_ */
