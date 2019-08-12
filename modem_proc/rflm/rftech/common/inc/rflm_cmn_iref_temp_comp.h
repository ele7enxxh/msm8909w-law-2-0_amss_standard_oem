/*!
  @file
  rflm_cmn_iref_temp_comp.h

  @brief
  Header file of common iref temp comp module

  @detail

*/

/*===========================================================================


  Copyright (c) 2014 Qualcomm Technologies, Inc. (QTI). All Rights Reserved

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

#ifndef RFLM_CMN_IREF_TEMP_COMP_H
#define RFLM_CMN_IREF_TEMP_COMP_H

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/rftech/common/inc/rflm_cmn_iref_temp_comp.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/24/14   oei     Add boolean dal_attached to rflm_cmn_iref_temp_comp_t
2/01/14   hdz     Initial Revision
==============================================================================*/
#include "rflm.h"

/*! iref temp comp data buffer size*/
#define RFLM_CMN_IREF_TEMP_COMP_BUF_SIZE                                      200

/*----------------------------------------------------------------------------*/
/*!
  @brief single iref temp comp buffer data type
*/
typedef struct
{
  /*! dac therm reading*/
  int32 dac_therm_read;
  /*! compensated factor scaled by 2^(slope_scaling).
  alpha = slope*2^(slope_scaling)*(ref_t - curr_t);
  new_iq = curr_iq+ curr_iq*alpha/2^(slope_scaling);
  */
  int32 alpha;
}rflm_cmn_iref_temp_comp_single_t;

typedef struct
{
  /*! buffer index of the lastest validate reading*/
  uint32 idx;
  /*! slope*/
  int32 slope;
  /*! slope scaling factor: 2^(slope_scaling)*/
  uint32 slope_scaling;
  /*! reference temperature*/
  int32 ref_t;
  /*! max temperature to be compensated*/
  int32 max_t;
  /*! min temperature to be compensated*/
  int32 min_t;
  /*! alpha at max temperature*/
  int32 alpha_max_t;
  /*! alpha at min temperature*/
  int32 alpha_min_t;
  /*! dac sensor idx that should be the cloest one to the DAC*/
  uint32 sensor_idx;
  /*! circular buffer to store each therm read and its alpha*/
  rflm_cmn_iref_temp_comp_single_t data[RFLM_CMN_IREF_TEMP_COMP_BUF_SIZE];
  /*! attach return code */
  boolean dal_attached;
}rflm_cmn_iref_temp_comp_t;

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
void
rflm_cmn_iref_temp_comp_init
(
  void
);

/*----------------------------------------------------------------------------*/
void
rflm_cmn_iref_temp_comp_therm_read
(
  uint32 handle_id
);

/*----------------------------------------------------------------------------*/
boolean
rflm_cmn_iref_temp_comp_apply
(
  uint32 handle_id,
  uint32 *iq_backoff
);

#ifdef __cplusplus
}  // extern "C"
#endif 


#endif /* RFLM_RFCMD_MSG_H */
