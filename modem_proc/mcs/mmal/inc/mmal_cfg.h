#ifndef __MMAL_CFG_H__
#define __MMAL_CFG_H__
/*!
  @file mmal_cfg.h

  @brief
   Header file used to configure the Modem Metrics Abstraction Layer.

   This file can be used to configure all parameters of mmal.

*/

/*=============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/inc/mmal_cfg.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/19/11   ars     Added FEATURE_8X55_SINGLE_RADIO_FUSION
05/05/11   ars     Added includes for customer and comdef.h
04/26/11   ars     Added FEATURE_MMAL_CIQ_FUSION for fusion specific
                   code of MMAL CIQ
03/25/11   ars     Initial checkin

=============================================================================*/
#include "mcs_variation.h"
#include "comdef.h"
#include "customer.h"

/* @brief CIQ feature should be on only if MMAL is on */
#if defined(FEATURE_MMAL_CIQ) && !defined(FEATURE_MMAL)
  #error "FEATURE_MMAL not defined"
#endif

/* @brief POSIX specific code in MMAL (like mutexes) goes under below 
 feature */
#ifdef FEATURE_POSIX
  #define FEATURE_MMAL_POSIX
#endif /* FEATURE_POSIX */

#ifdef FEATURE_LTE
#define FEATURE_MMAL_LTE
#endif

/* @brief MMAL CIQ featurization for Fusion Targets */
#if ( (defined FEATURE_8X60_SINGLE_RADIO_FUSION ||  \
       defined FEATURE_8X55_SINGLE_RADIO_FUSION)&&  \
       defined FEATURE_MMAL_CIQ )
  #define FEATURE_MMAL_CIQ_FUSION
#endif
/* @brief Maximum number of messages the MMAL task Q can hold */
#define MMAL_MAX_MSG  50

#define FEATURE_MMAL_CIQ_FUSION

#define MMAL_DSM_ITEM_POOL DSM_DS_SMALL_ITEM_POOL

/* leave Phase1 GS metrics enabled */
//#define FEATURE_MMAL_DISABLE_GS3031
//#define FEATURE_MMAL_DISABLE_GS3435

/* disable Phase2 GS metrics */
#define FEATURE_MMAL_DISABLE_GS15
#define FEATURE_MMAL_DISABLE_GS18
#define FEATURE_MMAL_DISABLE_GS19
#define FEATURE_MMAL_DISABLE_GS46
#define FEATURE_MMAL_DISABLE_GS47
#define FEATURE_MMAL_DISABLE_GS6E

#endif /* __MMAL_CFG_H__ */
