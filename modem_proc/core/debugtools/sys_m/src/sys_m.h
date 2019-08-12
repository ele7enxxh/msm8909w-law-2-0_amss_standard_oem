#ifndef SYS_M_H
#define SYS_M_H
/*===========================================================================

           S Y S _ M . H

DESCRIPTION

Copyright (c) 2011-15 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================
DESCRIPTION
High level system monitor
===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/sys_m/src/sys_m.h#1 $

===========================================================================*/
#include "qmi_csi.h"
#include "rcevt.h"
#include "msg.h"

#define SYS_M_PREFIX_LPASS "ADSP:"
#define SYS_M_PREFIX_MPSS  "MPSS:"
#define SYS_M_PREFIX_SLPI  "SLPI:"

#define SYS_M_SMEM_SSR_REASON_LPASS0  SMEM_SSR_REASON_LPASS0
#define SYS_M_SMEM_SSR_REASON_DSPS0  SMEM_SSR_REASON_DSPS0
#define SMEM_SSR_REASON_MSS0  SMEM_SSR_REASON_MSS0

#define SYS_M_MSG_HIGH(fmt)  \
   MSG(MSG_SSID_TMS, MSG_LEGACY_HIGH, SYS_M_PREFIX fmt)
#define SYS_M_MSG_HIGH_1(fmt, a)  \
   MSG_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, SYS_M_PREFIX fmt, a)
#define SYS_M_MSG_HIGH_2(fmt, a, b)  \
   MSG_2(MSG_SSID_TMS, MSG_LEGACY_HIGH, SYS_M_PREFIX fmt, a, b)
#define SYS_M_MSG_HIGH_3(fmt, a, b, c)  \
      MSG_3(MSG_SSID_TMS, MSG_LEGACY_HIGH, SYS_M_PREFIX fmt, a, b, c)
#define SYS_M_MSG_STR_1(fmt, a)  \
   MSG_SPRINTF_1(MSG_SSID_TMS, MSG_LEGACY_HIGH, SYS_M_PREFIX fmt, a)
#define SYS_M_MSG_ERR(fmt)  \
      MSG(MSG_SSID_TMS, MSG_LEGACY_ERROR, SYS_M_PREFIX fmt)
#define SYS_M_MSG_ERR_1(fmt, a)  \
      MSG_1(MSG_SSID_TMS, MSG_LEGACY_ERROR, SYS_M_PREFIX fmt, a)


typedef enum SYS_M_COMMUNICATION_T
{
    SYS_M_SMSM_P2P       = 0,
    SYS_M_QMI            = 1
} SYS_M_COMMUNICATION_TYPE;




//begin shutdown
void sys_m_shutdown(SYS_M_COMMUNICATION_TYPE);

//shutdown function
void sys_m_shutdown_internal(void);

/**
  call the registered callback to release all SMEM spinlock
  after entering into STM while gracefull shutdown.

  @return
   TRUE : Success
   FALSE: Failure

  @dependencies
  None.
*/
void sys_m_shutdown_terminate_in_STM(void);

/**
 *
 * @brief sys_m_signal_modules
 *
 * Signal interested modules of a system monitor event.  After all clients have completed
 * notification, acknowledge to apps
 *
*/
void sys_m_signal_modules(RCEVT_HANDLE handle);


//internal QMI init function
qmi_csi_service_handle ssctl_v02_qmi_ser_init(qmi_csi_os_params *os_params);

#define SYS_M_QMI_v02_SIG     0x00100000

#define SYS_M_SMP2P_PORT_OUT        "slave-kernel"
#define SYS_M_SMP2P_PORT_IN         "master-kernel"
#define SYS_M_XPU_ERR_STRING        "sys_m:xpu_error"
#define SYS_M_SMP2P_TZ_PORT_IN      "tz2mpss_smp2p"


#endif  /* SYS_M_H */
