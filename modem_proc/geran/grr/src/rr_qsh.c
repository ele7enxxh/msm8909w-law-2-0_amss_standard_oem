/*!
 * \file rr_qsh.c 
 *  
 * This module contains functionality to interface to QSH. 
 *  
 *              Copyright (c) 2015 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_qsh.c#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"

#if defined (FEATURE_QSH_DUMP) || defined (FEATURE_QSH_EVENT_METRIC)

#include "qsh.h"
#include "rr_qsh.h"
#include "grr_qsh_ext.h"
#include "rr_qsh_dump.h"
#include "rr_qsh_metric.h"
#include "rr_defs.h"

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief QSH call-back function
 * 
 * \param params_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_cb_fn(qsh_client_cb_params_s *params_ptr)
{
  RR_NULL_CHECK_RETURN_VOID(params_ptr);

  switch(params_ptr->action)
  {
#ifdef FEATURE_QSH_DUMP
    case QSH_ACTION_DUMP_COLLECT:
    {
      rr_qsh_dump_perform_cb_action(params_ptr);
      break;
    }
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
    case QSH_ACTION_METRIC_CFG:
    {
      rr_send_imsg_qsh_metric_config_ind(params_ptr);
      break;
    }

    case QSH_ACTION_METRIC_TIMER_EXPIRY:
    {
      rr_send_imsg_qsh_metric_timer_expiry_ind(params_ptr);
      break;
    }
#endif // FEATURE_QSH_EVENT_METRIC

    default:
    {
      break;
    }
  }

  return;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*!
 * \brief Process function for RR_IMSG_METRIC_CONFIG_IND
 * 
 * \param msg_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_process_metric_config_ind(rr_imsg_qsh_metric_config_ind_t *msg_ptr)
{
  rr_qsh_metric_config_perform_cb_action(&msg_ptr->cb_params);

  return;
}

/*!
 * \brief Process function for RR_IMSG_METRIC_TIMER_EXPIRY_IND
 * 
 * \param msg_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_process_metric_timer_expiry_ind(rr_imsg_qsh_metric_timer_expiry_ind_t *msg_ptr)
{
  rr_qsh_metric_timer_expiry_perform_cb_action(&msg_ptr->cb_params);

  return;
}

/*!
 * \brief Initialisation function called once at RR task start.
 */
__attribute__((section(".uncompressible.text")))
void rr_qsh_task_start_init(void)
{
  qsh_client_reg_s client_reg;
  qsh_action_mask_t cb_action_suppport_mask = 0;

  // Initialise QSH registration structure
  qsh_client_reg_init(&client_reg);

#ifdef FEATURE_QSH_DUMP
  // Initialise the QSH dump module
  client_reg.dump_info = rr_qsh_dump_init();

  // Setup the call-back support mask for dumps
  cb_action_suppport_mask |= QSH_ACTION_DUMP_COLLECT;
#endif // FEATURE_QSH_DUMP

#ifdef FEATURE_QSH_EVENT_METRIC
  // Initialise the QSH metric module
  client_reg.metric_info = rr_qsh_metric_init();

  // Setup the call-back support mask for metrics
  cb_action_suppport_mask |= (QSH_ACTION_METRIC_CFG | QSH_ACTION_METRIC_TIMER_EXPIRY);
#endif // FEATURE_QSH_EVENT_METRIC

  client_reg.client = QSH_CLT_GRR;
  client_reg.client_cb_ptr = rr_qsh_cb_fn;
  client_reg.major_ver = GRR_QSH_MAJOR_VER;
  client_reg.minor_ver = GRR_QSH_MINOR_VER;
  client_reg.cb_action_support_mask = cb_action_suppport_mask;

  // Register with QSH
  qsh_client_reg(&client_reg);

  return;
}

#endif // FEATURE_QSH_DUMP || FEATURE_QSH_EVENT_METRIC

/* EOF */

