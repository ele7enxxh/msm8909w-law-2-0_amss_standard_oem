#ifndef SNS_UIMG_UTILS_H
#define SNS_UIMG_UTILS_H
/*============================================================================
  @file sns_uimg_utils.h

  @brief
  Contains definitions of utilities to make writing uImage aware code easier.

  <br><br>

  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================*/

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/* We only support the following modules */
typedef enum 
{
  SNS_UIMG_MEMGR,
  SNS_UIMG_SMR,
  SNS_UIMG_BB,
  SNS_UIMG_SMR_MAX_CLIENTS
}sns_uimg_module_e;

/*=========================================================================
  FUNCTION:  sns_utils_place_uimge_hold
  =======================================================================*/
/*!
    @brief Exits uImage if needed, and places a hold on the system's entry
           into uImage. This restriction stays in effect until the hold is
           removed for the calling module.

    @param[i] module: Module placing the hold
*/
/*=======================================================================*/
void sns_utils_place_uimge_hold(sns_uimg_module_e module);

/*=========================================================================
  FUNCTION:  sns_utils_remove_uimge_hold
  =======================================================================*/
/*!
    @brief Removes any hold on uImage entry placed by a module.
           If all modules have removed their holds, UTILS shall vote for
           uimge entry with the sensors PM module.

    @param[i] module: Module placing the hold
*/
/*=======================================================================*/
void sns_utils_remove_uimage_hold(sns_uimg_module_e module);

/*=========================================================================
  FUNCTION:  sns_utils_remove_uimge_hold
  =======================================================================*/
/*!
    @brief Initializes the utils module.
*/
/*=======================================================================*/
void sns_uimg_utils_init(void);

#endif /* SNS_UIMG_UTILS_H*/
