/*!
  @file mmal_ciq_lte_metric.h

  @brief

  @detail

*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
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

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/inc/mmal_lte_metrics.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
07/14/11   rj     Added Metrics LT10 and LT11
06/23/11   sk     Support for the LTE RRC cal back fn
05/31/11   sk     Support for the LTE02 metric
05/08/11   sk     Chaaged the param type for mmal_ciq_wcdma_metric_pack_dl_nas_msg_info
05/04/11   sk     Featurize the code under FEATURE_MMAL_LTE
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#ifndef MMAL_LTE_METRICS_H
#define MMAL_LTE_METRICS_H

#include "mcs_variation.h"
#include <customer.h>
#include <mmal_cfg.h>

#ifdef FEATURE_MMAL_CIQ
#ifdef FEATURE_MMAL_LTE
#include <IQMetricsLT.h>
#include <stdio.h>
#include <dsm.h>
#include <dsmutil.h>
#include <mutils_circ_q.h>
#include <mmal_util.h>
#include <mmal_msg_i.h>
#include <mmal_task.h>
#include <mmal_util.h>

#include <lte_mac.h>
#include <lte_cphy_msg.h>
#include <lte_rlc_msg.h>

#include <intf_ul.h>

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/


/*==============================================================================

                         INTERNAL VARIABLES

==============================================================================*/


/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*==============================================================================

                                FUNCTIONS

==============================================================================*/


/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_pack_dl_nas_msg_info

===========================================================================*/
/*!
    @brief
    Packs the GS34 NAS related information by interecepting the NAS event 
    EVENT_NAS_MESSAGE_RECEIVED

    @detail
    

    @return
    

    @Note
 Metric GS34 - UMTS/HSPA Layer 3 Downlink Message

*/
/*=========================================================================*/
void mmal_ciq_wcdma_metric_pack_dl_nas_msg_info
( 
   dsm_item_type **log_ptr
);

/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_pack_ul_nas_msg_info

===========================================================================*/
/*!
    @brief
    Packs the GS34 NAS related information by interecepting the NAS event 
    EVENT_NAS_MESSAGE_RECEIVED

    @detail
    

    @return
    

    @Note
 Metric GS34 - UMTS/HSPA Layer 3 Downlink Message

*/
/*=========================================================================*/
void mmal_ciq_wcdma_metric_pack_ul_nas_msg_info
( 
   dsm_item_type **log_ptr
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_lt01

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_lt01

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_lt01 
(
  uint16 pdu_num,
  uint16 encoded_msg_len,
  uint8 msg_type, 
  uint8 lc_type,
  dsm_item_type *log_payload_ptr
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_lt02

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_lt02

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_msg_lt02
(
  /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type       *log_payload_ptr,
  const unsigned int   log_code
);
/*===========================================================================

  FUNCTION:  mmal_ciq_mmode_metric_save_release_cause_gs03

===========================================================================*/
/*!
    @brief
    mmal_ciq_mmode_metric_save_release_cause_gs03

    @detail
    

    @return
    

    @Note
 

*/
/*=========================================================================*/
void mmal_ciq_mmode_metric_save_release_cause_gs03
(
/*! The log packet that has the necessary information to 
pack this metric */
dsm_item_type **log_payload_ptr 
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_lt03_timer_cb

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_lt03_timer_cb

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_lt03_timer_cb
(
  timer_cb_data_type   data
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_lt03

===========================================================================*/
/*!
    @brief
   mmal_ciq_lte_metric_pack_and_submit_lt03

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_lt03( void );

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_lt04_timer_cb

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_lt04_timer_cb

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_lt04_timer_cb
(
  timer_cb_data_type   data
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_lt04

===========================================================================*/
/*!
    @brief
   mmal_ciq_lte_metric_pack_and_submit_lt04

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_lt04( void );

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_conn_msg_lt05

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_conn_msg_lt05

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_conn_msg_lt05
(
  lte_cphy_conn_meas_cfg_req_s  *conn_msg
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_idle_msg_lt05

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_idle_msg_lt05

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_idle_msg_lt05
(
  lte_cphy_idle_meas_cfg_req_s  *idle_msg
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_lt06

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_lt06

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_lt06
(
const unsigned int log_code,
/*! The log packet that has the necessary information to 
pack this metric */
dsm_item_type *log_payload_ptr 
);


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_request_data_lt07_cb

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_request_data_lt07_cb

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_request_data_lt07_cb
(
  timer_cb_data_type data
);


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_msg_lt07

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_msg_lt07

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_msg_lt07
(
  lte_mac_stats_data_s  *mac_stats
);


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_msg_lt08

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_msg_lt08

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_msg_lt08 
(
  lte_cphy_raa_ind_msg_s  *msg
);


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_request_data_lt09_cb

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_request_data_lt09_cb

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_request_data_lt09_cb
(
  timer_cb_data_type data /*!< Data given to timer_def() to pass to this callback function */
);


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_dl_msg_lt09

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_dl_msg_lt09

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_dl_msg_lt09
(
  lte_rlcdl_stats_s   *lt09_mac_stats
);


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_ul_and_submit_msg_lt09

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_ul_and_submit_msg_lt09

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_ul_and_submit_msg_lt09
(
  lte_rlcul_stats_s  *lt09_mac_stats
);

/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_lt10

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_lt10
(
/*! The log packet that has the necessary information to 
pack this metric */
dsm_item_type *log_payload_ptr, 
const unsigned int log_code
);
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_lt11

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_lt11
(
/*! The log packet that has the necessary information to 
pack this metric */
dsm_item_type *log_payload_ptr 
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_msg_lt12

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_msg_lt12

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_msg_lt12 
(
  lte_LL1_ul_pusch_tx_status_ind_msg_struct  *msg
);

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_msg_lt13

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_msg_lt13

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_msg_lt13 
(
  lte_cphy_t310_status_ind_s  *msg
);

#endif /* FEATURE_MMAL_LTE*/ 
#endif // mmal_lte_metrics.h
#endif
