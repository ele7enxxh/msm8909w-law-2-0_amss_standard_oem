/*!
  @file
  qsh_invoke.h

  @brief
  Internal interface for invoking QSH.
*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/qsh_invoke.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
12/28/15   mm      CR 940426: Removing invoke_done_cb, step 1
02/13/15   mm      Updated invoke interface
01/22/15   mm      Initial check-in                 
==============================================================================*/

#ifndef QSH_INVOKE_H
#define QSH_INVOKE_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <qsh_client.h>
#include <qsh_types.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*!
  @brief
  Struct to represent client invoke parameters.
*/
typedef struct
{  
  /*! client enum */
  qsh_clt_e                 client;
  
  /*! whether or not client is allowed to run cb in async mode */
  qsh_action_mode_e         action_mode;
  
  /* todo: remove once cfcm removes this */
  void *                    invoke_done_cb;
} qsh_invoke_config_s;

/*!
  @brief
  Struct to represent action invoke parameters.
*/
typedef struct
{
  /*! action */
  qsh_action_e                        action;
  
  /*! action params */
  qsh_client_action_params_u          params;
  
  /*! pointer to action done params to set, or NULL */
  qsh_client_action_done_params_u *   done_params_ptr;
} qsh_invoke_action_s;

/*!
  @brief
  Struct for invoking QSH on a single client. 
*/
typedef struct
{
  /*! config per invocation */
  qsh_invoke_config_s   config;
  
  /*! action params */
  qsh_invoke_action_s   action;
  
  /*! QSH internal use only. Set within qsh_invoke_init().
      Client shall not change this field. */
  uint32                cookie_qsh_internal_use; 
} qsh_invoke_s;

/*!
  @brief
  Struct for invoking QSH on a single client with multiple actions.
*/
typedef struct
{ 
  /*! config per invocation */
  qsh_invoke_config_s     config;
  
  /*! array of action params */
  qsh_invoke_action_s *   action_arr_ptr;
  
  /*! number of action params */
  uint32                  action_arr_count;
  
  /*! QSH internal use only. Set within qsh_invoke_init().
      Client shall not change this field. */
  uint32                  cookie_qsh_internal_use; 
} qsh_invoke_multi_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTIONS:  qsh_invoke_init
              qsh_invoke

==============================================================================*/
/*!
  @brief
  To invoke the callback registered with QSH.
 
  @note 
  The CB registered through qsh_reg_client_with_cb(...) SHOULD NOT CALL
  this function else there will be an assertion.
 
  @return
  qsh_invoke_init:
    None
  qsh_invoke:
    Context id
*/
/*============================================================================*/
void qsh_invoke_init
(
  /*! invoke ptr */
  qsh_invoke_s *  invoke_ptr
);

qsh_context_id_t qsh_invoke
(
  /*! invoke ptr */
  qsh_invoke_s *  invoke_ptr
);

/*==============================================================================

  FUNCTIONS:  qsh_invoke_multi_init
              qsh_invoke_multi

==============================================================================*/
/*!
  @brief
  Invokes multiple actions for the same client. More efficient than 
  qsh_invoke() back-to-back as the mutex will only be acquired once.
 
  @note 
  The CB registered through qsh_reg_client_with_cb(...) SHOULD NOT CALL
  this function else there will be an assertion.
 
  @return
  qsh_invoke_init:
    None
  qsh_invoke:
    Context id
*/
/*============================================================================*/
void qsh_invoke_multi_init
(
  /*! invoke ptr */
  qsh_invoke_multi_s *  invoke_multi_ptr
);

qsh_context_id_t qsh_invoke_multi
(
  /*! invoke ptr */
  qsh_invoke_multi_s *  invoke_multi_ptr
);

#endif /* QSH_INVOKE_H */
