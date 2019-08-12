/*============================================================================

FILE:      rpm_ctl.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/inc/rpm_ctl.h#1 $
$Date: 2015/02/25 $

============================================================================*/

#ifndef RPM_CTL_H
#define RPM_CTL_H

/* initializes the rpm_ctl driver */
void rpm_ctl_init(void);

/* returns the current status of the halt enabled bit for rpm */
uint32 RPM_is_halt_enabled(void);

/* processes an incoming request for rpm_ctl */
void *RPMFW_RPMCtl_ProcessRequest(void *pCtxt, uint32 EffectiveImmediately,
                                   void *payload, uint32 payloadLen);

#endif /* RPM_CTL_H */
