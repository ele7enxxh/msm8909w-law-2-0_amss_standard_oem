/*!
  @file
  rflm_time_def.h

  @brief
  rflm time service - symbol definitions.

  @detail
  This file provides constants for use in assembly functions.
  
*/

/*===========================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  
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

#ifndef RFLM_TIME_DEF_H
#define RFLM_TIME_DEF_H


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/




/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

#if defined( HWIO_QDSP6SS_QTMR_V1_CNTPCT_LO_0_IN )
#define RFLM_TIME_Q6SS_XO_COUNTER_REG    QDSP6SS_QTMR_V1_CNTPCT_LO_0
#elif defined ( HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_0_IN )
#define RFLM_TIME_Q6SS_XO_COUNTER_REG    MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_0
#else
#error "Unknown XO HW register"
#endif


#endif /* RFLM_TIME_DEF_H */
