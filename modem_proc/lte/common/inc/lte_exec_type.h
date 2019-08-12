#ifndef _LTE_EXEC_TYPE_H_
#define _LTE_EXEC_TYPE_H_
/*!
  @file
  lte_rt_exec_type.h
  
  @brief
  Defines the EXEC types to be used for bring up of Scoprion and Q6
 
  @detail

*/

/*===========================================================================

  Copyright (c) 2007 Qualcomm Technologies Incorporated. All Rights Reserved

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
 
                                INCLUDES
  
===========================================================================*/

/*--------------------------------------------------------------------------
 * Standard C Includes 
 --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
 * Target Includes
 --------------------------------------------------------------------------*/

/*===========================================================================
 
                                GLOBALS
  
===========================================================================*/
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* NOTE: Use EVEN ENUMs for QACC0 - DEB configuration */
#define LTE_QACC0_L1     			0  
#define LTE_QACC0_L1_FW				2
#define LTE_QACC0_FULL_STACK			4
#define LTE_QACC0_FULL_STACK_RRC		6

/* NOTE: Use ODD ENUMs for QACC1 - A2+DEB configuration */
#define LTE_QACC1_L1_SW				1
#define LTE_QACC1_FULL_STACK_SW			3
#define LTE_QACC1_FULL_STACK_RRC_SW		5
#define LTE_QACC1_RMNET				7
#define LTE_QACC1_PLT_A2_SW				9

/* Range checking */

#define LTE_EXEC_CONFIG_FIRST 					LTE_QACC0_L1
#define LTE_EXEC_CONFIG_LAST					LTE_QACC1_PLT_A2_SW       


#define HW_TYPE_QACC0                                   0             
#define HW_TYPE_QACC1								    1
#define HW_TYPE_UNKNOWN                                 2

typedef struct _espresso_sync_type_
{
  int      cookie; /*  polling cookie */

  int      hw_type;  /* QACC type */

  /* Add More config parameters here*/

}espresso_sync_type_t;

#endif /* _LTE_EXEC_TYPE_H_ */
