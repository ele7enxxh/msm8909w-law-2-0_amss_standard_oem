#ifndef RR_QSH_H
#define RR_QSH_H

/*!
 * \file rr_qsh.h 
 *  
 * This module contains functionality to interface to QSH. 
 *  
 *              Copyright (c) 2015 Qualcomm Technologies, Inc.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 */
/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grr/src/rr_qsh.h#1 $ */
/* $DateTime: 2016/12/13 08:00:21 $$Author: mplcsds1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "geran_variation.h"

#if defined (FEATURE_QSH_DUMP) || defined (FEATURE_QSH_EVENT_METRIC)

#include "rr_if.h"

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/*!
 * \brief Process function for RR_IMSG_METRIC_CONFIG_IND
 * 
 * \param msg_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_process_metric_config_ind(rr_imsg_qsh_metric_config_ind_t *msg_ptr);

/*!
 * \brief Process function for RR_IMSG_METRIC_TIMER_EXPIRY_IND
 * 
 * \param msg_ptr (in)
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_process_metric_timer_expiry_ind(rr_imsg_qsh_metric_timer_expiry_ind_t *msg_ptr);

/*!
 * \brief Initialisation function called once at RR task start.
 */
__attribute__((section(".uncompressible.text")))
extern void rr_qsh_task_start_init(void);

#endif // FEATURE_QSH_DUMP || FEATURE_QSH_EVENT_METRIC

#endif /* #ifndef RR_MSC_H */

