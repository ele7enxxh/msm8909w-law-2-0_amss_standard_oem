#ifndef L1_QSH_LOG_H
#define L1_QSH_LOG_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
            G E R A N  QSH LOG

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2014 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 when       who       what, where, why
 --------   --------  ----------------------------------------------------------
 19/02/15   ab     Initial Revision.
 20/02/15   ab     CR773948 GL1 Support for provding Dumps and Metrics for QSH

 ===========================================================================*/
                     
/*===========================================================================
                                         INCLUDE FILES FOR MODULE
===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "l1i.h"
#include "gl1_msg.h"
#include "gl1_msg_g.h" // for gl1_msg_rx_rpt & gl1_msg_chan_type enum
#include "l1_fm.h"
#include "gsmdiag.h"
#include "gsmdiag_v.h"

#include "qsh.h"
#include "qsh_util.h"
#if defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)
#include "gl1_qsh_ext.h"
#endif

#if defined(FEATURE_QSH_EVENT_METRIC) || defined(FEATURE_QSH_DUMP)

void gl1_qsh_register(void);

#ifdef GFW_QSH_NOT_REQ_ON_JO
void gl1_gfw_qsh_register(void);
#endif
#ifdef FEATURE_QSH_EVENT_METRIC
void gl1_process_qsh_metric_ind
(
  l1_qsh_metric_ind_T *msg_ptr,
  gas_id_t            gas_id
);

typedef struct{
  uint8                           *start_addr;
  gas_id_t                        gas_id;
  qsh_client_metric_context_id_t  metric_context_id;
}gl1_qsh_metric_bufffer_t;

void gl1_qsh_write_tx_metric_on_reset(int32 metric, gas_id_t gas_id);
void gl1_qsh_cache_tch_rx_params (int32 pri_rxlev_full, 
                                  int32 pri_rxlev_sub,
                                  int32 sec_rxlev_full, 
                                  int32 sec_rxlev_sub,
                                  uint8 rxqual_full, 
                                  uint8 rxqual_sub,
                                  gas_id_t gas_id);

#endif /* FEATURE_QSH_EVENT_METRIC */


#endif /* FEATURE_QSH_EVENT_METRIC || FEATURE_QSH_DUMP */

#endif /* L1_QSH_LOG_H */
