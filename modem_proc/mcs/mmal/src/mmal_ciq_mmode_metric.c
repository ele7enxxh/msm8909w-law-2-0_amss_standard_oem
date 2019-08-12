/*!
  @file mmal_ciq_mmode.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_mmode_metric.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/02/11   sk     Add stats to detect when the GS01/02/03 metric are
                  submitted
07/20/11   sk     Fixed indentation
06/23/11   sk     Incorporate the call held state changes
06/13/11   sk     Generate the GS03 only if the 0x713d packet is generated as well
06/10/11   sk     Fixes for GS03, GS01
06/03/11   sk     GS02 callstate  related fixes
05/19/11   sk     Reenable the GS01,02,03 metric
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mcs_variation.h"
#if 0
#include <customer.h>
#include "mmal_cfg.h"



#ifdef FEATURE_MMAL_CIQ

  #include <stdio.h>
  #include <cm.h>
  #include <cmlog.h>
  #include <dsm.h>
  #include <log_codes_umts.h>
  #include <log_codes_wcdma_int.h>
  #include <naslog_v.h>
//#include <cmlog_v.h>
  #include "mmal_lte_metrics.h"
  #include "IQMetricsGS.h"
//#include "cmlog_v.h"
  #include "mmal_ciq_metrics.h"
  #include <log_codes.h>
  #include <IQMetricsCommon.h>
  #include <mmal_util.h>
	
/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

typedef struct
{
  iq_metric_GS03_t                gs03_metric;
  boolean                         gs03_cause_present;
}mmal_ciq_mmode_metric_s;

mmal_ciq_mmode_metric_s  ciq_mmode_metric;

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

  FUNCTION:  mmal_ciq_mmode_metric_pack_and_submit_gs01

===========================================================================*/
/*!
    @brief
    This function is called when the CM LOG (0x12C1) is submitted to the diag
    by the AMSS

    @detail
    this function will peek in to the log packet and appropriately package
    it into the GS01 metric and submit

    @return
    
    @Note
*/
/*=========================================================================*/
void mmal_ciq_mmode_metric_pack_and_submit_gs01
(
  /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type *log_payload_ptr  
)
{
  static iq_metric_GS01_t       gs01_metric;
  LOG_CM_CALL_EVENT_C_type      *cm_call_log_ptr;
  iq_call_state_type_t          call_state;
  iq_metric_id_t                metric_id  = IQ_MAKE_ID('G','S','0','1');
  iq_iovec_t                    iov[3];

  /*! Extract the CM log pakcet log code 0x12c1 */
  cm_call_log_ptr = (LOG_CM_CALL_EVENT_C_type *)(log_payload_ptr->data_ptr);

  if ((cm_call_log_ptr->call_type != CM_CALL_TYPE_VOICE)
  && (cm_call_log_ptr->call_type != CM_CALL_TYPE_VT))
  {
    return;
  }
  /*CM_CALL_EVENT_ORIG,
     phone originated a call 

  CM_CALL_EVENT_ANSWER,
    Incoming call was answered

  CM_CALL_EVENT_END_REQ,
    Started call end process 

  CM_CALL_EVENT_END,*/
  
  if ((cm_call_log_ptr->call_event != CM_CALL_EVENT_ORIG) &&
   (cm_call_log_ptr->call_event != CM_CALL_EVENT_SETUP_IND ))
  {
    return;
  }
    
  /*! @TODO Fill the dwCallId */
  gs01_metric.dwCallId = cm_call_log_ptr->call_id;


  /*-------------------------------------------------------------------------
  Fill the first field ucCallAttr
  Bit 0 = 0 <- MO call
  Bit 0 = 1 <- MT call
  Bit 1 = 0 <- Voice Call
  Bit 1 = 1 <- Video Call
   -------------------------------------------------------------------------*/

  /*! If mobile originated call */
  if (cm_call_log_ptr->call_state == 1) //(CM_CALL_STATE_ORIG))
  {
    GS01_CALLATTR_MO(gs01_metric);
	
  }
  else if (cm_call_log_ptr->call_state == 2) //(CM_CALL_STATE_INCOM))
  {
    GS01_CALLATTR_MT(gs01_metric);
	
  }
  else
  {
	MSG_HIGH("MMAL : call_state %d", cm_call_log_ptr->call_state,0,0);
  }

  if (cm_call_log_ptr->call_type == CM_CALL_TYPE_VOICE)
  {
    GS01_CALLATTR_VOICE(gs01_metric);
  }
  else if (cm_call_log_ptr->call_type == CM_CALL_TYPE_VT)
  {
    GS01_CALLATTR_VIDEO(gs01_metric);
  }
  else
  {
    return;
  }

  /*------------------------------------------------------------------------
  CM_CALL_STATE_IDLE,< Call is in the Idle state (i.e., no call). 
  -------------------------------------------------------------------------*/
  call_state = IQ_CALL_STATE_TYPE_IDLE;

  /*-------------------------------------------------------------------------
  GS01 Metric - Fill in the call state
  -------------------------------------------------------------------------*/
  gs01_metric.ucCallState = call_state;

  /*-------------------------------------------------------------------------
  GS01 Metric - Submit the metric after query
  -------------------------------------------------------------------------*/
  iov[0].data = &gs01_metric;
  iov[0].len = sizeof(gs01_metric);

  iov[1].data=NULL;
  iov[1].len = 0;

  if (cm_call_log_ptr->num.len < 64)
  {
    iov[1].data = cm_call_log_ptr->num.buf;
    iov[1].len = cm_call_log_ptr->num.len;
  }
  
  iov[2].data=NULL;
  iov[2].len=0;

  IQ_SubmitMetricV(IQ_METRIC_ID_GS01, &iov);

  mmal_ciq_metric_stats_info.metrics_info.num_gs01_metric_submitted++;  

}

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
)
{
  iq_metric_GS02_t  gs02_metric;
  LOG_CM_CALL_EVENT_C_type *cm_call_log_ptr;
  iq_call_state_type_t  call_state;
  iq_metric_id_t metric_id  = IQ_MAKE_ID('G','S','0','2');
  boolean log_gs02 = TRUE;
  

  /*! Extract the CM log pakcet log code 0x12c1 */
  cm_call_log_ptr = (LOG_CM_CALL_EVENT_C_type *)(log_payload_ptr->data_ptr);

  MSG_HIGH("GS02 call_event %d ", cm_call_log_ptr->call_event,0,0);

  if ((cm_call_log_ptr->call_type != CM_CALL_TYPE_VOICE)
  && (cm_call_log_ptr->call_type != CM_CALL_TYPE_VT) && (cm_call_log_ptr->call_event != 43))
  {
    return;
  }

  /*! @TODO Fill the dwCallId */
  gs02_metric.dwCallId = cm_call_log_ptr->call_id;;

  
  if (cm_call_log_ptr->call_event == 43)//CM_CALL_EVENT_MNG_CALLS_CONF
  {
    if (cm_call_log_ptr->active_call_info.call_ss_success)
	{
	  if ( cm_call_log_ptr->active_call_info.size == 0)
	  {
	    call_state = IQ_CALL_STATE_TYPE_HELD;
		MSG_HIGH("GS02 call is held",0,0,0);
	  }
	  else
	  {
	    call_state = gs02_metric.ucCallState;
		MSG_HIGH("GS02 call state is %d", call_state,0,0);
	  }
	}
	else
	  log_gs02 = FALSE;
  
  }
  else
  {
    switch (cm_call_log_ptr->call_state )
    {
      case CM_CALL_STATE_IDLE:
        log_gs02 = FALSE;
      	if (cm_call_log_ptr->call_event == CM_CALL_EVENT_END)
      	{
      	  log_gs02 = TRUE;
      	}
        call_state = IQ_CALL_STATE_TYPE_IDLE;
        break;
      case CM_CALL_STATE_ORIG:
        log_gs02 = FALSE;
        if (cm_call_log_ptr->call_event == CM_CALL_EVENT_ORIG)
      	{
      	  log_gs02 = TRUE;
      	}
        call_state = IQ_CALL_STATE_TYPE_ATTEMPTING;
        break;
      case CM_CALL_STATE_INCOM:
        log_gs02 = FALSE;
        if (cm_call_log_ptr->call_event == CM_CALL_EVENT_SETUP_RES)
      	{
      	  log_gs02 = TRUE;
      	}
        call_state = IQ_CALL_STATE_TYPE_ATTEMPTING;
        break;
      case CM_CALL_STATE_CONV:
      	log_gs02 = FALSE;
      	if (cm_call_log_ptr->call_event == CM_CALL_EVENT_CONNECT)
      	{
      	  log_gs02 = TRUE;
      	}
        call_state = IQ_CALL_STATE_TYPE_CONNECTED;
        break;
      case CM_CALL_STATE_CC_IN_PROGRESS:
        call_state = IQ_CALL_STATE_TYPE_ESTABLISHED;
        break;
      case CM_CALL_STATE_RECALL_RSP_PEND:
        call_state = IQ_CALL_STATE_TYPE_UNKNOWN;
        break;
      default:
        call_state = IQ_CALL_STATE_TYPE_UNKNOWN;
        break;
    }
    /*-------------------------------------------------------------------------
    GS01 Metric - Fill in the call state
    -------------------------------------------------------------------------*/
    gs02_metric.ucCallState = call_state;
  }
  
  if (log_gs02 )
  {
    mmal_ciq_metrics_query_submit_metric(
    IQ_METRIC_ID_GS02,
    &gs02_metric,
    sizeof(gs02_metric));

    mmal_ciq_metric_stats_info.metrics_info.num_gs02_metric_submitted++;
  }
}

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
   

    @Note

 * Metric GS03 - 3GPP CS Call End

typedef IQ_PACKED_STRUCT
{
    unsigned long  dwCallId;
    unsigned long  dwErrCode;
    unsigned short wTermCode;
} iq_metric_GS03_t;

    
*/
/*=========================================================================*/
void mmal_ciq_mmode_metric_save_release_cause_gs03
(
  /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type **log_payload_ptr 
)
{
  uint16 read_len, bytes_read;
  uint8 msg[20];
  uint8 byte;

  read_len = sizeof(log_hdr_type);

  bytes_read = dsmi_pullup( log_payload_ptr,
                            msg, read_len,
                            __FILENAME__,
                            __LINE__ );

  MMAL_ASSERT(bytes_read == read_len);
  
  byte = (dsmi_pull8(log_payload_ptr,__FILENAME__, __LINE__)) ;
   						 
  ciq_mmode_metric.gs03_metric.wTermCode = (dsmi_pull8(log_payload_ptr, 
                         __FILENAME__, __LINE__)) ;

  ciq_mmode_metric.gs03_metric.dwErrCode = 0;
  
  ciq_mmode_metric.gs03_cause_present = TRUE;
}
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
   

    @Note
===========================================================================*/
void mmal_ciq_mmode_metric_pack_and_submit_gs03
(
  /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type *log_payload_ptr  
)
{

  
  LOG_CM_CALL_EVENT_C_type        *cm_call_log_ptr;
  iq_metric_id_t                  metric_id=IQ_MAKE_ID('G','S','0','3');
  iq_call_termination_reason_t    end_reason;
  
  /*! Extract the CM log pakcet log code 0x12c1 */
  cm_call_log_ptr = 
  (LOG_CM_CALL_EVENT_C_type *)(log_payload_ptr->data_ptr);
  
  if ((cm_call_log_ptr->call_type != CM_CALL_TYPE_VOICE)
  && (cm_call_log_ptr->call_type != CM_CALL_TYPE_VT))
  {
    return;
  }

  if ((cm_call_log_ptr->call_event != CM_CALL_EVENT_END ) &&
   (cm_call_log_ptr->call_event != CM_CALL_EVENT_END_VOIP_CALL))
  {
    return;
  }

  /*! @TODO Fill the dwCallId */
  ciq_mmode_metric.gs03_metric.dwCallId = cm_call_log_ptr->call_id; 

  if (ciq_mmode_metric.gs03_cause_present)
  {
    mmal_ciq_metrics_query_submit_metric(
      IQ_METRIC_ID_GS03,
      &ciq_mmode_metric.gs03_metric,
      sizeof(ciq_mmode_metric.gs03_metric));
	
	  ciq_mmode_metric.gs03_cause_present = FALSE;

    mmal_ciq_metric_stats_info.metrics_info.num_gs03_metric_submitted++;
  }
}
#endif
#endif /* FEATURE_MMAL*/
