/*!
  @file
  cfcm_qsh.c

  @brief
  QSH client for CFCM.
*/

/*==============================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cfcm/src/cfcm_qsh.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
03/13/15   sg      Removed mutex in sync call back
12/15/14   sg      Added QSH prints for CPU and clients
07/08/14   rj      Initial check-in
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "cfcmi.h"
#include "cfcm_qsh.h"
#include "dsm.h"
#include "cfcm_dbg.h"

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_qsh_run_action_analysis

==============================================================================*/
/*!
  @brief
  Performs top-level QSH analysis.
*/
/*============================================================================*/
static void cfcm_qsh_run_action_analysis
(
  qsh_cb_params_s *   qsh_params
);

/*==============================================================================

  FUNCTION:  cfcm_qsh_run_action_reset

==============================================================================*/
/*!
  @brief
  Performs QSH reset.
*/
/*============================================================================*/
static void cfcm_qsh_run_action_reset
(
  qsh_cb_params_s *   qsh_params
);

/*==============================================================================

                                FUNCTIONS

==============================================================================*/

/*==============================================================================

  FUNCTION:  cfcm_qsh_analysis_cb

==============================================================================*/
/*!
  @brief
  Callback registered with QSH.
  
  @note
  If async_allowed = TRUE and the analysis is done in a separate thread, the 
  qsh_cb_params_s data should be copied out.
  
  @return
  For now, just return TRUE.
*/
/*============================================================================*/
boolean cfcm_qsh_analysis_cb
(
  qsh_cb_params_s *   qsh_params
)
{
  switch(qsh_params->action)
  {
    case QSH_ACTION_ANALYSIS:
    {
      cfcm_qsh_run_action_analysis(qsh_params);
      break;
    }
    case QSH_ACTION_RESET:
    {
      cfcm_qsh_run_action_reset(qsh_params);
      break;
    }
    default:
    {
      CFCM_MSG_1(ERROR, "Invalid QSH action: %d", qsh_params->action);
      break;
    }
  }
  
  return TRUE;
}

/*==============================================================================

  FUNCTION:  cfcm_qsh_dsm_config_analysis

==============================================================================*/
/*!
  @brief
  Logs DSM config on QSH.
*/
/*============================================================================*/
static void cfcm_qsh_dsm_config_analysis
(
  void
)
{
  cfcm_dbg_s* cfcm_dbg_ptr;

  /* get the config pointer */
  cfcm_dbg_ptr = cfcm_dbg_get_data();
    
  QSH_LOG(
      CFCM_CLT_QSH,
      QSH_CAT_CFG,
      QSH_MSG_TYPE_INFO,
      "LTE DL DSM: LARGE(many/few/dne):%d/%d/%d; DUP(many/few/dne):%d/%d/%d",
      cfcm_dbg_ptr->large_pool_lte_dl_many,
      cfcm_dbg_ptr->large_pool_lte_dl_few,
      cfcm_dbg_ptr->large_pool_lte_dne,
      cfcm_dbg_ptr->dup_pool_lte_dl_many,
      cfcm_dbg_ptr->dup_pool_lte_dl_few,
      cfcm_dbg_ptr->dup_pool_lte_dne);

    QSH_LOG(
      CFCM_CLT_QSH,
      QSH_CAT_CFG,
      QSH_MSG_TYPE_INFO,
      "LTE DL LVL1 DSM: LARGE(many/few):%d/%d; DUP(many/few):%d/%d",
      cfcm_dbg_ptr->large_pool_lte_dl_level1_many,
      cfcm_dbg_ptr->large_pool_lte_dl_level1_few,
      cfcm_dbg_ptr->dup_pool_lte_dl_level1_many,
      cfcm_dbg_ptr->dup_pool_lte_dl_level1_few);

    QSH_LOG(
      CFCM_CLT_QSH,
      QSH_CAT_CFG,
      QSH_MSG_TYPE_INFO,
      "A2 UL DSM: LARGE(many/few):%d/%d; DUP(many/few):%d/%d",
      cfcm_dbg_ptr->large_pool_a2_ul_per_many,
      cfcm_dbg_ptr->large_pool_a2_ul_per_few,
      cfcm_dbg_ptr->dup_pool_a2_ul_per_many,
      cfcm_dbg_ptr->dup_pool_a2_ul_per_few);

    QSH_LOG(
      CFCM_CLT_QSH,
      QSH_CAT_CFG,
      QSH_MSG_TYPE_INFO,
      "IPA DL DSM: LARGE(many/few):%d/%d;",
      cfcm_dbg_ptr->large_pool_ipa_dl_many,
      cfcm_dbg_ptr->large_pool_ipa_dl_few);

}


/*==============================================================================
  FUNCTION:  cfcm_qsh_run_action_analysis

==============================================================================*/
/*!
  @brief
  Performs QSH analysis.
*/
/*============================================================================*/
static void cfcm_qsh_run_action_analysis
(
  qsh_cb_params_s *   qsh_params
)
{  

   /* QSH analysis could be called in sync and async modes
      when called in sync mode (error fatal) API should not lock mutex */
  if((qsh_params != NULL) && 
     (qsh_params->async_allowed == FALSE))
  {
    /* run analysis on CPU stats without mutex */
    cfcm_cpu_monitor_qsh_analysis();
  }
  else
  {
    /* run analysis on CPU stats with mutex */
    cfcm_cpu_monitor_qsh_analysis_async();
  }
  
  /* run analysis on monitors */
  cfcm_monitor_qsh_analysis();
  
  /* run analysis on CPU stats */
  cfcm_client_qsh_analysis();

  /* Print all the DSM config */
  cfcm_qsh_dsm_config_analysis();
}

/*==============================================================================

  FUNCTION:  cfcm_qsh_run_action_reset

==============================================================================*/
/*!
  @brief
  Performs QSH reset.
*/
/*============================================================================*/
static void cfcm_qsh_run_action_reset
(
  qsh_cb_params_s *   qsh_params
)
{
  /* clear monitor stats */
  cfcm_monitor_qsh_reset();

  /* clear CPU stats */
  cfcm_cpu_monitor_qsh_reset();

  /* clear client stats */
  cfcm_client_qsh_reset();
}
