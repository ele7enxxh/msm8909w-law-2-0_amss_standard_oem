/*!
  @file
  rflm_fbrx.h

  @brief
  Internal RFLM header for FBRx
  
  @detail

  
  @todo


*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies, Inc. All Rights Reserved

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/fbrx/inc/rflm_fbrx.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/14    ka     Disable filter
04/10/14    ka     Move mem init prototype to api file
04/03/13    ka     Add error filtering macro
10/10/13    ka     Increased max mem size to match tx agc
09/23/13    ka     Fix warnings
09/11/13    ka     Initial version
==============================================================================*/
#ifndef FEATURE_D3925_FED
#ifndef RFLM_FBRX_H
#define RFLM_FBRX_H


/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "rflm_api_fbrx.h"


#ifdef __cplusplus
extern "C" {
#endif

/*!  @brief enum of fbrx memory handle       */
typedef enum
{
  RFLM_FBRX_MEM_0,
  RFLM_FBRX_MEM_1,
  RFLM_FBRX_MEM_2,
  RFLM_FBRX_MEM_3,
  RFLM_FBRX_MAX_MEM
}rflm_fbrx_handle_t;

#define RFLM_FBRX_FILTER_ERROR(accumulated_error, error_val) ((0*accumulated_error + 1*error_val)>>0)

/*==============================================================================
  FBRx prototypes
==============================================================================*/

/*----------------------------------------------------------------------------*/
extern rflm_fbrx_dm_template_t* rflm_fbrx_allocate_memory(void);

/*----------------------------------------------------------------------------*/
extern boolean rflm_fbrx_release_memory(rflm_fbrx_dm_template_t *fbrx_template);

/*----------------------------------------------------------------------------*/
extern rflm_err_t rflm_fbrx_set_data( void* fbrx_data, rflm_fbrx_cmd_e cmd, const void* payload );

/*----------------------------------------------------------------------------*/
extern rflm_err_t rflm_fbrx_get_data( void* fbrx_data, rflm_fbrx_cmd_e cmd, void* payload );

/*----------------------------------------------------------------------------*/
rflm_fbrx_gps_state_type rflm_fbrx_get_gps_state( rflm_handle_tx_t tx_handle );


#ifdef __cplusplus
} // extern "C"
#endif

#endif /* RFLM_FBRX_H */
#endif
