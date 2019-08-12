/*!
  @file
  qsh_legacy.h

  @brief
  Glue code to maintain backward compatability. This should be removed once
  all clients implement changes.
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/qsh/backend/qsh_legacy.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/22/15   mm      Initial check-in             
==============================================================================*/

#ifndef QSH_LEGACY_H
#define QSH_LEGACY_H

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <qsh_types.h>

/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

#define FEATURE_QSH_LEGACY

/*! since RESET was removed */
//#define QSH_ACTION_RESET  QSH_ACTION_NONE

/*! @brief Parameters passed into the call back functions 
    registered with QSH using the function qsh_reg_client_with_cb(...) . 
*/
typedef struct
{
  /*! whether to run analysis, reset stats ... */
  qsh_action_e action;   
  /*! category mask. Use enum qsh_cat_e values to define mask e.g. 
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;  
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN 
      the registered CB in the same task context which invoked their call back  
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. 
      The client should call qsh_analysis_async_done() when async analysis
      has completed. */
  boolean async_allowed; 
} qsh_cb_params_s;

/*! @brief Analysis functions for clients registered with 
    qsh_reg_client_with_cb(...). 
 
    Argument is qsh_cb_params_s. Check the definition of the struct above. 
    Returns true for CB running in sync mode and false for async mode.
*/
typedef boolean (*qsh_analysis_cb_type) (qsh_cb_params_s *);

/*! @brief Parameters for invoking QSH. 
*/
typedef struct
{ 
  /*! client */
  qsh_clt_e client;   
  /*! whether to run analysis, reset stats ... */   
  qsh_action_e action;   
  /*! category mask. Use enum qsh_cat_e values to define mask e.g. 
      QSH_CAT_CFG|QSH_CAT_PERF (since the values are bit mask simple
      "or" will work) */
  uint32 category_mask;  
  /*! whether the registered CB can run in sync/async mode.
      When called with async_allowed = false the clients MUST RUN 
      the registered CB in the same task context which invoked their call back  
      (e.g. error fatal). When called with async_allowed = true
      the clients have an option to run their registered CB in async mode. */
  boolean async_allowed; 
} qsh_invoke_params_s;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  qsh_reg_client_with_cb

==============================================================================*/
/*!
  @brief
  Function for the external modules to register their call back which
  can be called when the analysis is performed by QSH.
 
  @note
  Clients are supposed to update arguments major_ver and minor_ver when changes
  are made in their callback function

*/
/*============================================================================*/
void qsh_reg_client_with_cb
(
  qsh_clt_e client, /*!< client registering for the call back*/
  qsh_analysis_cb_type analysis_cb_ptr, /*!< call back for analysis */
  uint16 major_ver, /*!< major version for the CB */
  uint16 minor_ver /*!< minor version for the CB */
);

/*==============================================================================

  FUNCTION:  qsh_invoke_cb

==============================================================================*/
/*!
  @brief
  To invoke the call backs registered with QSH. 
 
  @note The CB registered through qsh_reg_client_with_cb(...) SHOULD NOT CALL
  this function else there will be an assertion.
 
  @return
  None.
*/
/*============================================================================*/
void qsh_invoke_cb
(
  qsh_invoke_params_s *params /*!< invoke parameters */
);

#endif /* QSH_LEGACY_H */