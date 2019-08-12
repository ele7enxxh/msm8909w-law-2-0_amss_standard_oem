#ifndef __MSM_H__
#define __MSM_H__
/*
===========================================================================
*/
/**
  @file msm.h 

  Consolidated header file for all target specific public HWIO macro
  definitions.

  This file encapsulates all the MSM hardware specific header files,
  including those for HWIO register access.  It defines no interfaces
  itself but simply pulls in all required header files.
*/
/*  
  ====================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated.  All Rights Reserved.  
  QUALCOMM Proprietary and Confidential. 

  ==================================================================== 
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/systemdrivers/msm.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
  ====================================================================
*/ 

/*=========================================================================
      Include Files
==========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Deprecated file warning.
 */
#warning THIS FILE IS DEPRECATED ON ARAGORN, DO NOT INCLUDE.  See go/hwio for details.

/*
 * Common types.
 */
#ifndef _ARM_ASM_
#include "com_dtypes.h"
#endif

/*
 * HWIO access macros.
 */
#include "msmhwio.h"


#ifdef __cplusplus
}
#endif    


#endif /* __MSM_H__ */

