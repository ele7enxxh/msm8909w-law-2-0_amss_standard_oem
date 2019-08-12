/*==============================================================================

FILE:      rpm_spm_handshake.h

      Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/server/rpm_spm_handshake.h#1 $
$Date: 2015/02/25 $

==============================================================================*/

#ifndef RPM_SPM_HANDSHAKE_H
#define RPM_SPM_HANDSHAKE_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "rpmserver.h"
#include "rpm_config.h"
#include "rpm_set_changer.h"

// SPM state machine defines and implementation.
typedef enum
{
  SPM_SHUTDOWN_REQ,
  SPM_BRINGUP_REQ,
  SPM_TRANSITION_COMPLETE,
  SPM_SHUTDOWN_ACK,
  SPM_BRINGUP_ACK,
} rpm_spm_entry_reason;

void rpm_spm_state_machine(unsigned ee, rpm_spm_entry_reason reason);

void rpm_spm_init(void);

void rpm_spm_soft_transition( unsigned ee, rpm_spm_entry_reason reason);

void rpm_spm_trigger_wakeup_int(unsigned ee);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif 

