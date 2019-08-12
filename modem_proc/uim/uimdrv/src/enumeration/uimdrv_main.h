#ifndef UIMDRV_MAIN_H
#define UIMDRV_MAIN_H
/*============================================================================
  FILE:         uimdrv_main.h

  OVERVIEW:     This file holds the includes needed for HW enumeration along with
                externs of some uimdrv globals.

  DEPENDENCIES: N/A

                Copyright (c) 2012 - 2015 QUALCOMM Technologies, Inc(QTI).
                All Rights Reserved.
                QUALCOMM Technologies Confidential and Proprietary
============================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/enumeration/uimdrv_main.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when        who        what, where, why
------      ----       -----------------------------------------------------------
12/09/15    hyo        Moving to QTF Dal implementation 
05/21/15    lm         UT framework changes
03/12/15    ks         SIM tray by tieing the card detect gpios
02/03/15    xj         Add USB UICC enabled on slot interface
08/20/14    nmb        9x45 Bring up changes
07/04/14    ssr        Fix for APDU debug Mask
03/27/14    lxu        USB UICC modem implement
01/29/14    sam        Usage of newer style diag macros
09/30/13    akv        Removal of if-else cases and unused code in hw enumeration
09/16/13    yk         Removed the code for old targets and for old features
08/06/13    rm/ak      Bolt RUMI code
08/1/13     sam        APDU logging mask variable moved to uim_instance_global_type
07/25/13    sam        APDU logging mask selection based on run time
                       configuration
07/12/13    nmb        Global Reorganization
06/24/13    js         TSTS changes
05/31/13    js         Fixed compilation issue
02/21/13    ak         UIM1 support on 9x25 v2
02/11/13    akv        UIM API for querying the available/enabled UIM slots
12/05/12    js         UIM Parallel Processing changes
11/01/12    akv        Core cpu vdd voting to avoid Q6 going into power collapse
10/26/12    akv/ssr    HW Enumeration changes
============================================================================*/
#include "uimdrv_enumeration.h"


/* Standard inclusions from uimdimedrv.h */

#include "uim_variation.h"
#include "comdef.h"
#include "err.h"                   /* Error handling utilities             */
#include "uimdrv_msg.h"                   /* Message utilities                    */
#include "uimdrv.h"                /* UIM driver header file               */
#include "uimi.h"                  /* UIM Internal header file             */
#if defined( FEATURE_UIM_T_1_SUPPORT )
#include "crc.h"
#include "uimt1.h"                 /* T=1 protocol support                 */
#endif /* FEATURE_UIM_T_1_SUPPORT */
#include "HALhwio.h"                    /* Target definitions */

#ifdef FEATURE_THOR_MODEM
#include "uimthorhwiomacros.h"
#else
  #ifdef FEATURE_BOLT_MODEM
  #include "uimbolthwiomacros.h"
  #else /* FEATURE_BOLT_MODEM */
  #include "uimdimehwiomacros.h"
  #endif
#endif

#include "uimutil.h"

#include "DDITlmm.h"
#include "DDIClock.h"

#include "DDIInterruptController.h"
#include "DalDevice.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"

  /* Inclusion for MCPM APIs */
#include "mcpm_api.h"

#include "pm.h"
#include "npa.h"

/* Standard inclusions from uimdimedrv.h */

extern uim_interface_instances              uim_hw_if;
extern uimHWConfig                          m_HWConfig;
extern boolean                              uim_usb_supported[UIM_INSTANCES_CONFIGURABLE];

extern uimdrv_hw_nv_config_info_type uimdrv_hw_nv_config_info;
extern uim_hw_slot_info_type                 uim_hw_slot_info;


boolean initialize_hw_classes(uim_instance_global_type *uim_ptr);
uim_hw_slot_info_type uimdrv_hw_nv_configuration(void);
boolean uimIsDualSlotEnabled(void);
#ifdef FEATURE_UIM_USB_UICC
boolean uim_is_usb_enabled(uim_instance_global_type *uim_ptr);
#endif /* FEATURE_UIM_USB_UICC */

/* to see if slot supports USB UICC */
boolean uim_is_usb_enabled_on_slot(uim_instance_enum_type uim_id);
#endif /* UIMDRV_MAIN_H */
