/*!
  @file
  rflm_time_profile_meas.h

  @brief
  macros and data structure for measuring time durations

  @detail
  To perform profiling measurement, use:
    RFLM_TIME_PROFILE_MEAS_MARK_START()and 
    RFLM_TIME_PROFILE_MEAS_MARK_END() macros.
  
 
*/

/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
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

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef RFLM_TIME_PROFILE_MEAS_H
#define RFLM_TIME_PROFILE_MEAS_H


/*===========================================================================

      Macros

===========================================================================*/
/* ! @brief
     Compile time switch to enable/disable profiling.
*/
#define RFLM_TIME_PROFILE_EN  1
   

#if (RFLM_TIME_PROFILE_EN==1)

#define RFLM_TIME_PROFILE_MEAS_MARK_START    rflm_time_profile_meas_mark_start
#define RFLM_TIME_PROFILE_MEAS_MARK_END      rflm_time_profile_meas_mark_end
        
#else

#define RFLM_TIME_PROFILE_MEAS_MARK_START(a)
#define RFLM_TIME_PROFILE_MEAS_MARK_END(a)

#endif


/* ! @brief
     # measurements to store in duration hist
*/
#define RFLM_TIME_PROFILE_MEAS_HIST_SIZE  (32)


/*===========================================================================

      Typedefs

===========================================================================*/

/* ! @brief profile measurement data structure     
*/
typedef struct
{
  int32 max_time;           ///< maximum time duration
  int32 start_time;         ///< start time of measured time duration
  int32 hist[RFLM_TIME_PROFILE_MEAS_HIST_SIZE];
                            ///< history of measured time duration
  int16 hist_index;         ///< index into hist[] for storing next duration
  int16 avg_index;          ///< index used for averaging
  int32 avg_accum;          ///< accumulator used for averaging
  int32 avg;                ///< average        
} rflm_time_profile_meas_s;


/*===========================================================================

      Functions

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

extern void   rflm_time_profile_meas_init( rflm_time_profile_meas_s *meas );
extern uint32 rflm_time_profile_meas_mark_start( rflm_time_profile_meas_s *meas );
extern uint32 rflm_time_profile_meas_mark_end( rflm_time_profile_meas_s *meas );

#ifdef __cplusplus
}
#endif


#endif

