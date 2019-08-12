/*==============================================================================
 Copyright (c) 2014, AKM
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
     3. Neither the name of the AKM nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
==============================================================================*/

/*==============================================================================

 Copyright (c) 2014-2015 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies Proprietary and Confidential.

==============================================================================*/

/*==============================================================================

                       EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
--------     ---     -----------------------------------------------------------
08/25/15     AKM     Add AK09913/AK09916 support.
03/31/15     AKM     Updated copyright message
01/28/15     PS      Removed qurt_elite header files
09/12/14     AKM     Add UMSG for supporting uImage
==============================================================================*/

#ifndef _SNSD_MAG_AKM099XX_DEBUG_H
#define _SNSD_MAG_AKM099XX_DEBUG_H

/*=======================================================================

                  INTERNAL DEFINITIONS

========================================================================*/

/*=======================================================================

COMPILE CONDITION

========================================================================*/
/* Enable this flag to enable uImage operation */
#define AKM_MAG_ENABLE_UIMG

/* Enable the following macro to see debug out */
//#define SNSD_MAG_AKM099XX_DEBUG

/* Enable this flag when compiling on OpenSSC HD22 builds. */
//#define AKM_COMPILE_FOR_HD22

/* Enable function trace message */
#define SNSD_MAG_AKM099XX_DEBUG_FUNC
#define SNSD_MAG_AKM099XX_DEBUG_FUNC_UIMG


/*=======================================================================

DEBUG PRINTOUT MACROS

========================================================================*/

/* AKM's debug message level
 * HIGH : ERROR
 * MED  : INFO
 * LOW  : DEBUG (VERBOSE)
 */
#ifdef SNSD_MAG_AKM099XX_DEBUG
#ifdef QDSP6
#ifdef AKM_COMPILE_FOR_HD22
#include "qurt_elite_diag.h"
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define MAG_AKM_DEBUG_MSG_0(level,msg)          UMSG(  MSG_SSID_QDSP6,DBG_##level##_PRIO, " " msg)
#define MAG_AKM_DEBUG_MSG_1(level,msg,p1)       UMSG_1(MSG_SSID_QDSP6,DBG_##level##_PRIO, " " msg,p1)
#define MAG_AKM_DEBUG_MSG_2(level,msg,p1,p2)    UMSG_2(MSG_SSID_QDSP6,DBG_##level##_PRIO, " " msg,p1,p2)
#define MAG_AKM_DEBUG_MSG_3(level,msg,p1,p2,p3) UMSG_3(MSG_SSID_QDSP6,DBG_##level##_PRIO, " " msg,p1,p2,p3)
#else
#define DBG_MEDIUM_PRIO DBG_MED_PRIO
#define MAG_AKM_DEBUG_MSG_0(level,msg)          UMSG(  MSG_SSID_SNS,DBG_##level##_PRIO, " " msg)
#define MAG_AKM_DEBUG_MSG_1(level,msg,p1)       UMSG_1(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1)
#define MAG_AKM_DEBUG_MSG_2(level,msg,p1,p2)    UMSG_2(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2)
#define MAG_AKM_DEBUG_MSG_3(level,msg,p1,p2,p3) UMSG_3(MSG_SSID_SNS,DBG_##level##_PRIO, " " msg,p1,p2,p3)
#endif /* End of AKM_COMPILE_FOR_HD22*/
#else
#define MED MEDIUM
#include "sns_debug_str.h"
#define MAG_AKM_DEBUG_MSG_0(level,msg)          SNS_PRINTF_STRING_ID_##level##_0(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING0)
#define MAG_AKM_DEBUG_MSG_1(level,msg,p1)       SNS_PRINTF_STRING_ID_##level##_1(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING1,p1)
#define MAG_AKM_DEBUG_MSG_2(level,msg,p1,p2)    SNS_PRINTF_STRING_ID_##level##_2(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING2,p1,p2)
#define MAG_AKM_DEBUG_MSG_3(level,msg,p1,p2,p3) SNS_PRINTF_STRING_ID_##level##_3(SNS_DBG_MOD_DSPS_SMGR,DBG_SMGR_GENERIC_STRING3,p1,p2,p3)
#endif /* End of QDSP6 */
#else
#define MAG_AKM_DEBUG_MSG_0(level,msg)
#define MAG_AKM_DEBUG_MSG_1(level,msg,p1)
#define MAG_AKM_DEBUG_MSG_2(level,msg,p1,p2)
#define MAG_AKM_DEBUG_MSG_3(level,msg,p1,p2,p3)
#endif /* End of SNSD_MAG_AKM099XX_DEBUG */

#endif /* End include guard  _SNSD_MAG_AKM099XX_DEBUG_H */
