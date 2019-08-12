#ifndef PM_LIB_H
#define PM_LIB_H


/*! \file
 *  \n
 *  \brief pm_lib.h ---- PMAPP HEADER FILE
 *  \details      This file contains functions prototypes and variable/type/constant 
  declarations for applications developed for the Qualcomm Power
  Manager Chip Set.
 *  \n &copy; Copyright (c) 2003 - 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved. 
 */ 

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/pmic/pm_lib.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/09/10   jtn     Added "get" APIs for driver
07/29/09   Vk      Added HSED APIs
07/28/09   Vk      Added video & lcdkpd files
06/28/09   vk      Added renamed vreg file.
11/14/07   jtn     Added pm66XXmpps.h
04/27/07   cng     Featurzie with FEATURE_WINCE to choose different header file
02/07/07   vk      New container header file that would just list all header 
                   from which to export APIs;

===========================================================================*/
#include "comdef.h"

/* Note:
1. you must have the header files to be processed in the same order in .min file.
e.g HTORPC_pm_lib_ARGS = -a pm_lib -H pmgen.h -H pm_vreg.h -H pm66XXrtc.h -H pm66XXmpps.h etc
2. Make sure also that the files are listed exactly the same as some platforms are case sensitive.
*/

#include "pm_qc_pmic.h"
#include "pmgen.h"
#include "pm_vreg.h"
#include "pm_rtc.h"
#include "pm_mpps.h"
#include "pm_flashled.h"
#include "pm_video.h"
#include "pm_lcdkpd.h"
#include "pm_vibrator.h"
#include "pm_speaker.h"
#include "pm_mic.h"
#include "pm_hsed.h"
#include "pm_low_current_led.h"
#include "pm_gpio.h"
#include "pm_pwron.h"
#include "pm_chg.h"
#include "pm_nfc.h"

#endif //PM_LIB_H


