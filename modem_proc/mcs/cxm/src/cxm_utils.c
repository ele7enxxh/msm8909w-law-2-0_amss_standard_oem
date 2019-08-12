/*!
  @file cxm_utils.c

  @brief
   Provides a number of basic utilities to the Co-Existence Manager Layer

*/

/*=============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
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

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/src/cxm_utils.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/30/13   tak     Added command to shut down CXM task
10/02/12   btl     Initial rev

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <IxErrno.h>
#include "cxm_utils.h"
#include "task.h"

/*=============================================================================

                           INTERNAL DEFINES

=============================================================================*/
#define MIN_UQ_16_16 0.0
#define MAX_UQ_16_16 65536.9999847412109375
#define MIN_UQ_24_8  0.0
#define MAX_UQ_24_8  16777215.99609375
#define MIN_SQ_24_8  -8388608.0
#define MAX_SQ_24_8  8388607.99609375
/*=============================================================================

                           EXTERNAL DEFINES

=============================================================================*/
timer_group_type cxm_nondeferrable_timer_group;

/*=============================================================================

                           EXTERNAL FUNCTION PROTOTYPES

=============================================================================*/

/*===========================================================================

  FUNCTION:  cxm_timer_group_init

===========================================================================*/
/*!
  @brief
    Initialize non-deferrable timer group for cxm

  @return
    None
*/
/*=========================================================================*/
void cxm_timer_group_init(void)
{
  timer_group_set_deferrable( &cxm_nondeferrable_timer_group, FALSE );
  timer_group_enable( &cxm_nondeferrable_timer_group );
}

/*===========================================================================

  FUNCTION:  cxm_float_to_unsigned_Q16

===========================================================================*/
/*!
  @brief
    Convert from a float to a Q16.16 (fixed point, 16 integer bits and 16 
    fractional bits) format

  @return
    errno_enum_type
      E_SUCCESS
      E_FAILURE if overflow
    uint32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_float_to_unsigned_Q16(float f_in, uint32 *q_out)
{
  errno_enum_type status = E_SUCCESS;
  float temp = f_in;

  CXM_ASSERT(q_out != NULL);

  /* check bounds & cut off */
  if(f_in < MIN_UQ_16_16)
  {
    status = E_FAILURE;
    temp = MIN_UQ_16_16;
  }
  else if(f_in >= MAX_UQ_16_16)
  {
    status = E_FAILURE;
    temp = MAX_UQ_16_16;
  }

  /* perform conversion - bring 16 fractional bits into int range */
  temp = temp * 65536.0f;
  /* round - assume always positive */
  temp = temp + 0.5f;
  *q_out = (uint32) temp;

  return status;
}

/*===========================================================================

  FUNCTION:  cxm_float_to_unsigned_Q8

===========================================================================*/
/*!
  @brief
    Convert from a float to a Q24.8 (fixed point, 24 integer bits and 8 
    fractional bits) format

  @return
    errno_enum_type
      E_SUCCESS
      E_FAILURE if overflow
    uint32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_float_to_unsigned_Q8(float f_in, uint32 *q_out)
{
  errno_enum_type status = E_SUCCESS;
  float temp = f_in;

  CXM_ASSERT(q_out != NULL);

  /* check bounds & cut off */
  if(f_in < MIN_UQ_24_8)
  {
    status = E_FAILURE;
    temp = MIN_UQ_24_8;
  }
  else if(f_in >= MAX_UQ_24_8)
  {
    status = E_FAILURE;
    temp = MAX_UQ_24_8;
  }

  /* perform conversion - bring 8 fractional bits into int range */
  temp = temp * 256.0f;
  /* round - assume always positive */
  temp = temp + 0.5f;
  *q_out = (uint32) temp;

  return status;
}

/*===========================================================================

  FUNCTION:  cxm_float_to_signed_Q8

===========================================================================*/
/*!
  @brief
    Convert from a float to a signed Q24.8 (fixed point, 24 integer 
    bits and 8 fractional bits) format

  @return
    errno_enum_type
      E_SUCCESS
      E_FAILURE if overflow
    int32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_float_to_signed_Q8(float f_in, int32 *q_out)
{
  errno_enum_type status = E_SUCCESS;
  float temp = f_in;

  CXM_ASSERT(q_out != NULL);

  /* check bounds & cut off */
  if(f_in < MIN_SQ_24_8)
  {
    status = E_FAILURE;
    temp = MIN_SQ_24_8;
  }
  else if(f_in >= MAX_SQ_24_8)
  {
    status = E_FAILURE;
    temp = MAX_SQ_24_8;
  }

  /* perform conversion - bring 8 fractional bits into int range */
  temp = temp * 256.0f;
  /* round */
  temp = (temp < 0) ? temp - 0.5f : temp + 0.5f;
  *q_out = (int32) temp;

  return status;
}


/*===========================================================================

  FUNCTION:  cxm_unsigned_Q8_to_float

===========================================================================*/
/*!
  @brief
    Convert from a Q24.8 (fixed point, 24 integer bits and 8 
    fractional bits) format to a float

  @return
    errno_enum_type
    uint32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_unsigned_Q8_to_float(uint32 q_in, float *f_out)
{
  errno_enum_type status = E_SUCCESS;
  float temp;

  CXM_ASSERT(f_out != NULL);

  temp = (float) q_in;
  temp = temp / 256.0f;
  *f_out = temp;

  return status;
}

/*===========================================================================

  FUNCTION:  cxm_signed_Q8_to_float

===========================================================================*/
/*!
  @brief
    Convert from a Q24.8 (fixed point, 24 integer bits and 8 
    fractional bits) format to a float

  @return
    errno_enum_type
    int32 Q8 number
*/
/*=========================================================================*/
errno_enum_type cxm_signed_Q8_to_float(int32 q_in, float *f_out)
{
  errno_enum_type status = E_SUCCESS;
  float temp;

  CXM_ASSERT(f_out != NULL);

  temp = (float) q_in;
  temp = temp / 256.0f;
  *f_out = temp;

  return status;
}

/*=============================================================================

  FUNCTION:  cxm_terminate_task

=============================================================================*/
/*!
    @brief
    Terminates the CXM task by sending signal CXM_TASK_STOP_SIG
 
    @return
    errno_enum_type
*/
/*===========================================================================*/

void cxm_terminate_task(void)
{
  rex_set_sigs(&cxm_tcb, CXM_TASK_STOP_SIG); 
}

