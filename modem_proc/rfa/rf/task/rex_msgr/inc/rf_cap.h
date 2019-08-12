#ifndef RF_CAP_H
#define RF_CAP_H
/*!
  @file rf_cap.h

  @brief
  This header file defines the features to be used.

  @todo 
  Might need to be merged with another file after moving to the 
  common archive.

*/

/*===========================================================================

  Copyright (c) 2009, 2011 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/rex_msgr/inc/rf_cap.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/31/13   sc      Added FEATURE_RF_MEAS for featurising iRAT RF Task MSGR intf
04/24/12   ggs     Removed FEATURE_RF_HAS_GSM
03/24/12   sc      Added FEATURE_RF_HAS_GSM
02/15/12   sar     Cleaned up features.
07/20/11   jhe     Added FEATURE_RF_HAS_TDSCDMA
05/23/11   bmg     Mainlined FEATURE_QTF_OFFTARGET
11/19/09   pl      Added FEATURE_LTE for defining RF_HAS_LTE and RF_LTE_DEBUG
08/26/09   mkv     Added FEATURE_QTF_OFFTARGET
07/21/09   mkv     Added FEATURE_RF_TASK 
05/14/09   mkv	   Added FEATURE_RF_HAS_FTM.
04/16/09   can     P4 Header location introduction.
04/16/09   can     Added FEATURE_RF_LTE_DEBUG.
04/15/09   can     Initial Check-in
===========================================================================*/

/*===========================================================================

						 EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*Remove RF POSIX task and keep only REX Task. Then remove this feature.*/
#include "rfa_variation.h"
#ifndef FEATURE_RF_TASK
#define FEATURE_RF_TASK
#endif

#ifdef FEATURE_GSM
#ifndef FEATURE_RF_GSM_DEBUG
#define FEATURE_RF_GSM_DEBUG
#endif
#endif

#ifndef FEATURE_RF_MEAS
#define FEATURE_RF_MEAS
#endif

/*FTM MSGR commands are being recieved by RF POSIX TASK. If RF POSIX task 
  is removed then there is no need for this feature. */
#ifndef FEATURE_RF_HAS_FTM
#define FEATURE_RF_HAS_FTM
#endif

#ifdef FEATURE_TDSCDMA
#ifndef FEATURE_RF_HAS_TDSCDMA
#define FEATURE_RF_HAS_TDSCDMA
#endif
#endif

#endif /*RF_CAP_H*/
