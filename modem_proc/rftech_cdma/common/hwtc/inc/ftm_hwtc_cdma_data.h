#ifndef FTM_HWTC_CDMA_DATA_H
#define FTM_HWTC_CDMA_DATA_H

/*! 
  @file
  ftm_hwtc_cdma_data.h
 
  @brief
  This file contains prototypes and definitions to be used by Centralized 
  FTM HWTC Global Data.
*/

/*==============================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_cdma/common/hwtc/inc/ftm_hwtc_cdma_data.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
12/13/10   aro     Initial Revision.

==============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"

#ifdef FEATURE_FACTORY_TESTMODE

#ifdef FEATURE_FTM_HWTC

#include "ftm_hwtc_iqcapture.h"

/*----------------------------------------------------------------------------*/
/*! This is the maximum number of I and Q chips that can be buffered
per antenna */
#define FTM_HWTC_CDMA_IQ_SS_MAX_CHIPS  2*4096

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by CDMA IQ capture. There should 
  not be stray global variables. If global variable is required, it must be 
  contained within this structure. 
*/ 
typedef struct
{

  enum_iq_antenna_type ant_path;
  /*!< Variable to store the Path on which IQ Capture is to be done. This 
  corresponds to path data received from Tool side. Ant0 corresponds to 
  WB0-NB0, Ant1 corresponds to WB0-NB1, ANt2 corresponds to WB0-NB2, Ant3 
  corresponds to WB1-NB0, and so on. */

  uint16 iq_buf[FTM_HWTC_CDMA_IQ_SS_MAX_CHIPS];
  /*!< Buffer to store IQ Data before pushing to Diag*/


} ftm_hwtc_cdma_iq_capture_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by 1xHDR FTM HWTC. There should 
  not be stray global variables. If global variable is required, it must be 
  contained within this structure. 
*/ 
typedef struct
{

  ftm_hwtc_cdma_iq_capture_data_type iq_capture;
  /*!< Variables related to IQ Capture */

} ftm_hwtc_cdma_data_type;

extern ftm_hwtc_cdma_data_type ftm_hwtc_cdma_data;

#endif /* FEATURE_FTM_HWTC */

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FTM_HWTC_CDMA_DATA_H */

