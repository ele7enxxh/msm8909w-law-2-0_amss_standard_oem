/*!
  @file
  lte_mac_diag.h

  @brief
  LTE MAC's interface to diag messaging services.


*/

/*===========================================================================

  Copyright (c) 2011 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/mac/src/lte_mac_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/11   ax      clean up always on FEATURE_LTE_MAC_FC_UL
05/06/10   ax      compile in FEATURE_LTE_MAC_FC_UL
03/26/10   ax      removed macro definitions based on FEATURE_PROFILE
02/01/10   ax      turn off excessive f3 logging in MAC
08/26/08   bn      Initial version
===========================================================================*/

#ifndef LTE_MAC_DIAG_H
#define LTE_MAC_DIAG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <msgcfg.h>
#include <msg.h>
#include "lte_mac_cfg.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*--------------------------------------------------------------------------
                        Message Mask Definitions
--------------------------------------------------------------------------*/
/* The following are required for all subsystems and are defined in msg.h
 *
 * MSG_LEGACY_LOW      MSG_MASK_0
 * MSG_LEGACY_MED      MSG_MASK_1
 * MSG_LEGACY_HIGH     MSG_MASK_2
 * MSG_LEGACY_ERROR    MSG_MASK_3
 * MSG_LEGACY_FATAL    MSG_MASK_4
 *
 * The remaining bits are sub-subsystem specific. LTE MAC specific
 * ones are defined below. 
 */

/*! @brief ****** define diag subsystems for MAC UL task *******/
#define MSG_DBG_MACUL_RACH         MSG_MASK_5   /*!< MAC UL RACH */

#ifdef MSG_BUILD_MASK_MSG_SSID_LTE_MACUL
  #undef MSG_BUILD_MASK_MSG_SSID_LTE_MACUL
#endif /* MSG_BUILD_MASK_MSG_SSID_LTE_MACUL */

/* MASK not included here can be supressed at runtime:
   * MSG_LEGACY_LOW
   * MSG_LEGACY_MED
*/
/*! @brief This defines which messages are compiled in at build time */
#define MSG_BUILD_MASK_MSG_SSID_LTE_MACUL   \
               (MSG_LEGACY_HIGH         |   \
                MSG_LEGACY_ERROR        |   \
                MSG_LEGACY_FATAL        |   \
                MSG_DBG_MACUL_RACH)


/*! @brief ********define diag subsystems for MACCTRL task ***********/
#define MSG_DBG_MACCTRL_QOS       MSG_MASK_5   /*!< MAC QoS */
#define MSG_DBG_MACCTRL_CFG       MSG_MASK_6   /*!< MAC CTRL CFG */

#ifdef MSG_BUILD_MASK_MSG_SSID_LTE_MACCTRL
  #undef MSG_BUILD_MASK_MSG_SSID_LTE_MACCTRL
#endif /* MSG_BUILD_MASK_MSG_SSID_LTE_MACCTRL */

/* MASK not included here can be supressed at runtime:
   * MSG_LEGACY_LOW
   * MSG_LEGACY_MED
   * MSG_DBG_MACCTRL_QOS
*/

/*! This defines which messages are compiled in at build time */
#define MSG_BUILD_MASK_MSG_SSID_LTE_MACCTRL \
               (MSG_LEGACY_HIGH         |   \
                MSG_LEGACY_ERROR        |   \
                MSG_LEGACY_FATAL        |   \
                MSG_DBG_MACCTRL_CFG)

/*! @brief ********define diag subsystems for MACDL task ***********/
#define MSG_DBG_MACDL_PDU       MSG_MASK_5   /*!< MACDL PDU parser */

#ifdef MSG_BUILD_MASK_MSG_SSID_LTE_MACDL
  #undef MSG_BUILD_MASK_MSG_SSID_LTE_MACDL
#endif /* MSG_BUILD_MASK_MSG_SSID_LTE_MACDL */

/* MASK not included here can be supressed at runtime:
   * MSG_LEGACY_LOW
   * MSG_LEGACY_MED
   * MSG_DBG_MACDL_PDU
*/

/*! This defines which messages are compiled in at build time */
#define MSG_BUILD_MASK_MSG_SSID_LTE_MACDL   \
               (MSG_LEGACY_HIGH         |   \
                MSG_LEGACY_ERROR        |   \
                MSG_LEGACY_FATAL)

/* define diag F3 macros for MAC UL task */
#define LTE_MACUL_MSG_SPRINTF_0(xx_ss_mask, xx_fmt)   \
  MSG(MSG_SSID_LTE_MACUL, xx_ss_mask, xx_fmt)

#define   LTE_MACUL_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_LTE_MACUL, xx_ss_mask, xx_fmt, xx_arg1)

#define   LTE_MACUL_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_LTE_MACUL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

#define   LTE_MACUL_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_LTE_MACUL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define   LTE_MACUL_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_LTE_MACUL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define   LTE_MACUL_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_LTE_MACUL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define   LTE_MACUL_MSG_SPRINTF_6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_LTE_MACUL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

/* define diag F3 macros for MAC DL task */
#define LTE_MACDL_MSG_SPRINTF_0(xx_ss_mask, xx_fmt)   \
  MSG(MSG_SSID_LTE_MACDL, xx_ss_mask, xx_fmt)

#define   LTE_MACDL_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_LTE_MACDL, xx_ss_mask, xx_fmt, xx_arg1)

#define   LTE_MACDL_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_LTE_MACDL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

#define   LTE_MACDL_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_LTE_MACDL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define   LTE_MACDL_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_LTE_MACDL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define   LTE_MACDL_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_LTE_MACDL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define   LTE_MACDL_MSG_SPRINTF_6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_LTE_MACDL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

/* define diag F3 macros for MAC CTRL task */
#define LTE_MACCTRL_MSG_SPRINTF_0(xx_ss_mask, xx_fmt)   \
  MSG(MSG_SSID_LTE_MACCTRL, xx_ss_mask, xx_fmt)

#define   LTE_MACCTRL_MSG_SPRINTF_1(xx_ss_mask, xx_fmt, xx_arg1) \
  MSG_1(MSG_SSID_LTE_MACCTRL, xx_ss_mask, xx_fmt, xx_arg1)

#define   LTE_MACCTRL_MSG_SPRINTF_2(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  MSG_2(MSG_SSID_LTE_MACCTRL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2)

#define   LTE_MACCTRL_MSG_SPRINTF_3(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  MSG_3(MSG_SSID_LTE_MACCTRL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3)

#define   LTE_MACCTRL_MSG_SPRINTF_4(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4) \
  MSG_4(MSG_SSID_LTE_MACCTRL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4)

#define   LTE_MACCTRL_MSG_SPRINTF_5(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5) \
  MSG_5(MSG_SSID_LTE_MACCTRL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5)

#define   LTE_MACCTRL_MSG_SPRINTF_6(xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6) \
  MSG_6(MSG_SSID_LTE_MACCTRL, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, xx_arg4, xx_arg5, xx_arg6)

#endif /* LTE_MAC_DIAG_H */
