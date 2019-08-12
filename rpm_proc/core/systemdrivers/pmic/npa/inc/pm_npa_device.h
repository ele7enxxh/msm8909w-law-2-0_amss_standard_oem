#ifndef PM_NPA_DEVICE_H
#define PM_NPA_DEVICE_H
/*===========================================================================


P M    N P A    D E V I C E    H E A D E R    F I L E

DESCRIPTION
This file contains prototype definitions npa device layer

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/inc/pm_npa_device.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
1/29/13    aks      Adding support for 5v boost as a separate driver
04/11/12    hs     Removed duplicated type def pm_npa_freq_type.
02/12/12   wra     KVP based device setting initial file 
===========================================================================*/

/*===========================================================================

INCLUDE FILES

===========================================================================*/

#include "pm_qc_pmic.h"
#include "npa.h"
#include "npa_resource.h"
#include "pm_resources_and_types.h"
#include "comdef.h"
/*===========================================================================

TYPE DEFINITIONS

===========================================================================*/
/* Device Remote Resource */
#define PMIC_NPA_RMT_PROTOCOL_RPM_TYPE    "/protocol/rpm/rpm"
#define PMIC_NPA_RPM_INTERNAL_TYPE        "/protocol/rpm/internal"

typedef enum
{
  PM_NPA_VREG_LDO,
  PM_NPA_VREG_SMPS,
  PM_NPA_VREG_BOOST,
  PM_NPA_VREG_BOOST_BYP,
  PM_NPA_VREG_NCP,
  PM_NPA_VREG_VS,
  PM_NPA_CLK_BUFF,
  PM_NPA_PBS,
  PM_NPA_VREG_MAX,
}pm_npa_resouce_type;

/*!
 * Frequency Reason
 */
typedef enum
{    
    PM_NPA_FREQ_REASON_NONE = 0, // default
    PM_NPA_FREQ_REASON_BT = 1,
    PM_NPA_FREQ_REASON_GPS = 2,
    PM_NPA_FREQ_REASON_WLAN = 4,
    PM_NPA_FREQ_REASON_WAN = 8,
    PM_NPA_FREQ_REASON_INVALID
}pm_npa_freq_reason_type;

#define PM_NPA_KEY_SOFTWARE_ENABLE 0x6E657773 // swen - software enable
#define PM_NPA_KEY_LDO_SOFTWARE_MODE 0X646D736C // lsmd - LDO software mode
#define PM_NPA_KEY_SMPS_SOFTWARE_MODE 0X646D7373 // ssmd - SMPS software mode
#define PM_NPA_KEY_BOOST_BYP_MODE 0x6D796262 // bbym - BOOST BYP software mode
#define PM_NPA_KEY_PIN_CTRL_ENABLE 0x6E656370 //pcen - pin control enable
#define PM_NPA_KEY_PIN_CTRL_POWER_MODE 0x646d6370 // pcmd - pin control mode
#define PM_NPA_KEY_CURRENT 0x616D //ma
#define PM_NPA_KEY_MICRO_VOLT 0x7675 //uv
#define PM_NPA_KEY_PIN_CTRL_MICRO_VOLT 0x76756370 //pcuv
#define PM_NPA_KEY_FREQUENCY 0x71657266 //freq
#define PM_NPA_KEY_FREQUENCY_REASON 0x6E736572 //resn
#define PM_NPA_KEY_FOLLOW_QUIET_MODE 0x6D71 //qm
#define PM_NPA_KEY_HEAD_ROOM 0x7268 // hr
#define PM_NPA_KEY_PIN_CTRL_CLK_BUFFER_ENABLE_KEY 0x62636370  // pccb - clk buffer pin control
#define PM_NPA_KEY_BYPASS_ALLOWED_KEY 0x61707962  //bypa - bypass allowed
#define PM_NPA_KEY_CORNER_LEVEL_KEY 0x6E726F63 // corn - coner voltage



/*! 
 * Quiet Mode
 */
typedef enum
{
    PM_NPA_QUIET_MODE__DISABLE, // default
    PM_NPA_QUIET_MODE__QUIET,
    PM_NPA_QUIET_MODE__SUPER_QUIET,
    PM_NPA_QUIET_MODE__INVALID
}pm_npa_quiet_mode_type;








/*===========================================================================

DEFINITIONS

===========================================================================*/

#define PMIC_DEV_RSRC_NAME_VEC_IN( chipid, rtype, index )  \
    "/pmic/device/" #rtype "/" #chipid "/" #rtype #index "/vec"

#endif /* PM_NPA_DEVICE_H */

