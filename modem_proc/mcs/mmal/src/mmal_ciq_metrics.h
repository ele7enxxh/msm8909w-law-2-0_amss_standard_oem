/*!
  @file mmal_ciq_metrics.h

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_metrics.h#1 $

when       who    what, where, why
--------   ---    -------------------------------------------------------------
11/04/11   ejv    Add query prototypes for ss25 and ss2b.
08/04/11   sm/sk  Added num_diag_response/request to stats
08/02/11   sk     Added metric submission stats to mmal_ciq_metrics_stats_s
07/20/11   sk     Fixed indentation
07/19/11   sm     added num_duplicate_lt06_events
07/14/11   sm     added num_log/event_listener_cb_registered
06/23/11   sk     Call back function support for getting the LTE RRC OTA 
                  msg content
05/08/11   sk     Modified the prototype for mmal_ciq_metrics_free_item
05/05/11   sk     Added new fields in the stats info and macros to
                  to go with those
05/04/11   sk     Added the file and line number for the dsm calls
05/03/11   sk     Added some stats collection for the dsm item used
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#ifndef MMAL_CIQ_METRICS_H
#define MMAL_CIQ_METRICS_H

#include "mcs_variation.h"
#include <customer.h>
#include "mmal_cfg.h"
#include "mmal_lte_metrics.h"

#ifdef FEATURE_MMAL_CIQ

#include <stdio.h>
#include <diag.h>
#include "mmal_task.h"
#include "mmal_msg_i.h"
#ifdef FEATURE_WCDMA
#include <rrcdata.h>
#endif
#include <IQAgent.h>
#include <IQMetricsLT.h>
#include <mmal_util.h>
#include <ps_ifacei_event.h>
#include <cm.h>
#include <sm_cm_msg_if.h>
/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

typedef uint32 mmal_ciq_metric_bmask_t;

typedef struct
{
  uint16   error_cnt;

  uint16   dsm_error_cnt;

  /*LTE Metric Error Count*/
  uint16 lte01_ul_error_cnt;
  uint16 lte01_dl_error_cnt;

  /*GS01*/
  uint16 gs01_error_cnt;

  /*GS02*/
  uint16 gs02_error_cnt;

  /*GS03*/
  uint16 gs03_error_cnt;

  /*GS30*/
  uint16 gs30_error_cnt;

  /*GS31*/
  uint16 gs31_error_cnt;

  /*GS30*/
  uint16 gs34_error_cnt;

  /*GS31*/
  uint16 gs35_error_cnt;

  /* Diag error count like the packet could not be added to the listener*/
  uint16 diag_error_cnt;

  /*ciq error cnt, any ciq related function fails*/
  uint16 ciq_error_cnt;

}mmal_ciq_metric_error_cnt_s;

typedef struct
{
  uint32 dsm_alloc_cnt;

  uint32 dsm_dealloc_cnt;

  uint16 dsm_max_usage_cnt;

  uint16 profile_update_cnt;

  uint16 num_duplicate_lt06_events;

  uint16 num_log_listener_cb_registered;

  uint16 num_event_listener_cb_registered;

  uint16 num_gs30_metric_submitted;

  uint16 num_gs31_metric_submitted;

  uint16 num_gs01_metric_submitted;

  uint16 num_gs02_metric_submitted;

  uint16 num_gs03_metric_submitted;

  uint16 num_gs34_metric_submitted;

  uint16 num_gs35_metric_submitted;

  uint16 num_lte01_metric_submitted;

  uint16 num_lte02_metric_submitted;

  uint16 num_lte06_metric_submitted;

  uint16 num_diag_response;

  uint16 num_diag_request;  
}mmal_ciq_metrics_info_s;


typedef struct
{
  mmal_ciq_metrics_info_s       metrics_info;
  mmal_ciq_metric_error_cnt_s   metrics_err_info;
}mmal_ciq_metrics_stats_s;

extern mmal_ciq_metrics_stats_s mmal_ciq_metric_stats_info;

#define MMAL_CIQ_INCR_ERR_CNT() (mmal_ciq_metric_stats_info.metrics_err_info.error_cnt++)

#define MMAL_CIQ_INCR_DIAG_ERR_CNT() (mmal_ciq_metric_stats_info.metrics_err_info.diag_error_cnt++)

#define MMAL_CIQ_INCR_CIQ_ERR_CNT() (mmal_ciq_metric_stats_info.metrics_err_info.ciq_error_cnt++)

#define MMAL_CIQ_INCR_DSM_ERR_CNT() (mmal_ciq_metric_stats_info.metrics_err_info.dsm_error_cnt++)

#define MMAL_CIQ_INCR_DSM_ALLOC_CNT() (mmal_ciq_metric_stats_info.metrics_info.dsm_alloc_cnt++)

#define MMAL_CIQ_INCR_DSM_DEALLOC_CNT() (mmal_ciq_metric_stats_info.metrics_info.dsm_dealloc_cnt++)

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

  FUNCTION:  mmal_ciq_metrics_wipe_context_id_map

===========================================================================*/
/*!
    @brief
    
    @detail
    
    @return
    none

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_metrics_wipe_context_id_map
(
  void
);

/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_free_item

===========================================================================*/
/*!
    @brief
    This is a utility function which will release a dsm items
    
    @detail
    1. Releases the dsm item
    2. Updates the stats

    @return
    none

    @Note

*/
/*=========================================================================*/
void
mmal_ciq_metrics_free_item
(
  dsm_item_type **dsm_ptr,
  const char *file,
  uint16 line
);

/*===========================================================================

  FUNCTION:  mmal_ciq_populate_stats

===========================================================================*/
/*!
    @brief
    
    
    @detail
    

    @return
    none

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_populate_stats
(
  uint8 *idx,
  uint8 limit,
  uint8 *src,
  uint8 *dst,
  uint8 size
);

#ifdef FEATURE_WCDMA
/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_wrrc_log_msg_cb

===========================================================================*/
/*!
    @brief
    This function will be called whenever W RRC is going to send a 
    UL msg or has received a DL msg

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_metrics_wrrc_log_msg_process_cb
(
  log_message_type *log_msg_ptr 
);
#endif

/*==========================================================================

  FUNCTION:  mmal_ciq_metrics_lrrc_log_msg_cb

===========================================================================*/
/*!
    @brief
    This function will be called whenever W RRC is going to send a 
    UL msg or has received a DL msg

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_metrics_lrrc_log_msg_process_cb
(
  uint16 pdu_num, 
  uint16 encoded_msg_len, 
  unsigned char *encoded_msg, 
  uint8 msg_type, 
  uint8 lc_type
);


/*==========================================================================

  FUNCTION:  mmal_ciq_metrics_pdp_context_up_cb

===========================================================================*/
/*!
    @brief
    This function will be called whenever a PDP context is activated 

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_metrics_pdp_context_up_cb
(
  ps_iface_type             *ifacePtr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type eventInfo,
  void                      *userDataPtr
);

/*==========================================================================

  FUNCTION:  mmal_ciq_metrics_pdp_context_down_cb

===========================================================================*/
/*!
    @brief
    This function will be called whenever a PDP context is ended 

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_metrics_pdp_context_down_cb
(
  ps_iface_type               *iface_ptr,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                        *user_data_ptr
);

/*===========================================================================
  FUNCTION:  mmal_ciq_metrics_sniff_diag_events

===========================================================================*/
/*!
    @brief
    This is the call back function which will be called when the diag detects that
    event of the type registered with is submitted by the AMSS code

    @detail
    1. This function will be called when the particular event of interest 
    is logged by the AMSS code
    2. This function will copy the payload to a SMALL dsm item 
    3. Subsequently a message with UMID (submit_metric) would be sent to 
    be processed in
    the MMAL task context

    @return
    none

    @Note
    This is a call back function executing in the diag task context

*/
/*=========================================================================*/
void  mmal_ciq_metrics_sniff_diag_events
(
  uint32                          seq_num,
  const diag_event_type  *event,
  void                            *param
);
/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_sniff_diag_logs

===========================================================================*/
/*!
    @brief
    This is the call back function which will be called when the diag detects that
    log of the type registered with is submitted by the AMSS code

    @detail
    1. This function will be called when the particular event of interest is 
    logged by the AMSS code
    2. This function will copy the payload to a SMALL or LARGE dsm item 
    3. Subsequently a message with UMID (submit_metric) would be sent to be
    processed in the MMAL task context
    @return
    none

    @Note
    This is a call back function executing in the diag task context

*/
/*=========================================================================*/
void mmal_ciq_metrics_sniff_diag_logs 
(
  uint32             seq_num, 
  const byte        *log, 
  unsigned int      length, 
  void                 *param
);

/*===========================================================================

  FUNCTION:  mmal_ciq_metrics_query_submit_metric

===========================================================================*/
/*!
    @brief
    This function will first call the shouldSubmitMetric and if the result is sucessful
    will call the submit metric function.
    
    @detail
    This function could be optimized such that if the shouldSubmitMetric returns
    failure, then readjust the masks so that only when the profile changes, this call
    is amde again for this metric.
    
    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_metrics_query_submit_metric
(
  const iq_metric_id_t metric_id,
  const void *payload,
  const uint32 payload_size 
);


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_submit_lt01

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_submit_lt01 
(
  /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type *log_payload_ptr 
);



/*===========================================================================

  FUNCTION:  mmal_ciq_mmode_metric_pack_and_submit_gs02

===========================================================================*/
/*!
    @brief
    This function is called when the CM LOG (0x12C1) is submitted to the diag
    by the AMSS

    @detail
    this function will peek in to the log packet and appropriately package
    it into the GS02 metric and submit

    @return
   

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_mmode_metric_pack_and_submit_gs01
(
  /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type *log_payload_ptr  
);


/*===========================================================================

  FUNCTION:  mmal_ciq_mmode_metric_pack_and_submit_gs02

===========================================================================*/
/*!
    @brief
    This function is called when the CM LOG (0x12C1) is submitted to the diag
    by the AMSS

    @detail
    this function will peek in to the log packet and appropriately package
    it into the GS02 metric and submit

    @return
   

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_mmode_metric_pack_and_submit_gs02
(
   /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type *log_payload_ptr  
);


/*===========================================================================

  FUNCTION:  mmal_ciq_mmode_metric_pack_and_submit_gs03

===========================================================================*/
/*!
    @brief
    This function is called when the CM LOG (0x12C1) is submitted to the diag
    by the AMSS

    @detail
    this function will peek in to the log packet and appropriately package
    it into the GS02 metric and submit

    @return
   
typedef IQ_PACKED_STRUCT
{
    unsigned long dwCallId;
    unsigned char ucCallState;      // Use Enum: iq_call_state_type_t
} iq_metric_GS02_t;

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_mmode_metric_pack_and_submit_gs03
(
  /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type *log_payload_ptr  
);


/*===========================================================================

  FUNCTION:  mmal_ciq_metric_pack_and_submit_msg_gs19

===========================================================================*/
/*!
    @brief
    mmal_ciq_metric_pack_and_submit_msg_gs19

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_metric_pack_and_submit_msg_gs19 
(
  sm_pdp_context_request_log_info_ind_T  *msg
);

/*===========================================================================

  FUNCTION:  mmal_ciq_gsm_metric_pack_and_submit_gs31

===========================================================================*/
/*!
    @brief
    
    This function intercepts the 0x512F GSM log packet and packages 
    into the GS31 metric

    @detail

    @return
    

    @Note 

=========================================================================*/
void  mmal_ciq_gsm_metric_pack_and_submit_gs31
(
  dsm_item_type *dsm_log_ptr
);


/*===========================================================================

  FUNCTION:  mmal_ciq_gsm_metric_pack_and_submit_gs30

===========================================================================*/
/*!
    @brief
    
    This function intercepts the 0x512F GSM log packet and packages 
    into the GS31 metric

    @detail

    @return
    

    @Note 

=========================================================================*/
void  mmal_ciq_gsm_metric_pack_and_submit_gs30
(
  dsm_item_type *dsm_log_ptr
);

#ifdef FEATURE_WCDMA
/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_pack_rrc_msg_info

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
 Metric GS34 - UMTS/HSPA Layer 3 Downlink Message

*/
/*=========================================================================*/
void mmal_ciq_wcdma_metric_pack_rrc_msg_info
(
  log_message_type  *rrc_msg_info_ptr,
  dsm_item_type      *raw_sdu_ptr
);
#endif

/*==========================================================================

  FUNCTION:  mmal_ciq_cm_ss_event_srv_changed_cb

===========================================================================*/
/*!
    @brief
    Called when serving system changes to process any CM SS EVENT

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_cm_ss_event_srv_changed_cb
(
  cm_ss_event_e_type           ss_event,     /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr   /* Serving system information */
);

/*==========================================================================

  FUNCTION:  mmal_ciq_gs46_cm_ss_event

===========================================================================*/
/*!
    @brief
    Called when serving system changes to process GS46

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_gs46_cm_ss_event
(
  cm_ss_event_e_type           ss_event,        /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr      /* Serving system information */
);

/*==========================================================================

  FUNCTION:  mmal_ciq_gs47_cm_ss_event

===========================================================================*/
/*!
    @brief
    Called when serving system changes to process GS47

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_gs47_cm_ss_event
(
  cm_ss_event_e_type           ss_event,        /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr      /* Serving system information */
);


/*==========================================================================

  FUNCTION:  mmal_ciq_gs6E_cm_ss_event

===========================================================================*/
/*!
    @brief
    Called when serving system changes to process GS6E

    @detail

    @return

    @Note

*/
/*=========================================================================*/
void mmal_ciq_gs6E_cm_ss_event
(
  cm_ss_event_e_type           ss_event,     /* The event that occurred */
  const cm_mm_ss_info_s_type  *ss_info_ptr   /* Serving system information */
);

void mmal_ciq_ss25_query( void );
void mmal_ciq_ss2b_query( void );

#endif /* FEATURE_MMAL*/
#endif /*! MMAL_CIQ_METRICS_H*/
