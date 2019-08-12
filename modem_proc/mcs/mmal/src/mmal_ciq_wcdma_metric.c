/*!
  @file mmal_ciq_wcdma_metric.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_wcdma_metric.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
08/02/11   sk     Add stats to detect when the GS34/35 metric are
                  submitted
07/18/11   sk     Modified the GS35 metric collection to accomodate the changes of 
                  NOT buffering the log packets at the diag end.
06/30/11   sk     Support for back to back NAS messagess
06/24/11   sm     Fixed compilation issues
06/22/11   sm     CR293074 Fix. Free stored g35 dsm pointer before over-writing.
06/10/11   sk     Fixes for GS34/35
05/08/11   sk     Used the new mmal_ciq_metrics_free_item
05/05/11   sk     Modified the stats structure
05/04/11   sk     Added the file and line number for the dsm calls
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mcs_variation.h"
#include <customer.h>
#include "mmal_cfg.h"

#ifdef FEATURE_WCDMA
#ifdef FEATURE_MMAL_CIQ
#include <stdio.h>
#include <IQAgent.h>
#include <naslog_v.h>
#include <IQMetricsGS.h>
#include <log_codes_wcdma_int.h>
#include <mmal_ciq_metrics.h>
#include <rrcdata_v.h>
#include <diag.h>
#include <comdef.h>
#include <rrcdata.h>
#include <mmal_util.h>
//#include "mmal_lte_metrics.h"
#include <mmal_ciq_metrics.h>
#include <macrrcif.h>
#include <naslog.h>
#include "mmal_ciq_fend.h"
#include "mmal_task.h"

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/
extern mmal_ciq_metrics_stats_s mmal_ciq_metric_stats_info;
/*==============================================================================

                         INTERNAL VARIABLES

==============================================================================*/
#define MMAL_CIQ_WCDMA_METRIC_RRC_MSG_SIZE  1500 //in bytes
#define MMAL_CIQ_WCDMA_METRIC_MSG_SIZE   3

typedef struct
{
  // To keep track of the NAS portion of the metric
  uint8              nas_idx;
  // To keep track of the RRC portion of the metric
  uint8              rrc_idx;
  dsm_item_type     *gs34_ptr[MMAL_CIQ_WCDMA_METRIC_MSG_SIZE];
  iq_metric_GS34_t   gs34_metric[MMAL_CIQ_WCDMA_METRIC_MSG_SIZE];   
}mmal_ciq_wcdma_gs34_metric_s;

typedef struct
{
  // To keep track of the NAS portion of the metric
  uint8              nas_idx;
  // To keep track of the RRC portion of the metric
  uint8              rrc_idx;
  boolean            nas_info_present[MMAL_CIQ_WCDMA_METRIC_MSG_SIZE];
  dsm_item_type     *gs35_ptr[MMAL_CIQ_WCDMA_METRIC_MSG_SIZE];
  iq_metric_GS35_t   gs35_metric[MMAL_CIQ_WCDMA_METRIC_MSG_SIZE];   
}mmal_ciq_wcdma_gs35_metric_s;

typedef struct
{
  mmal_ciq_wcdma_gs34_metric_s   gs34_info;
  mmal_ciq_wcdma_gs35_metric_s   gs35_info;
}mmal_ciq_wcdma_metric_s ;

mmal_ciq_wcdma_metric_s     ciq_wcdma_metric;


/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*==============================================================================

                                FUNCTIONS  

==============================================================================*/

/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_init

===========================================================================*/
/*!
    @brief
    
     Gets the next idxs
    @detail
    

    @return
    

    @Note
    


*/
/*=========================================================================*/
void mmal_ciq_wcdma_metric_init(void)
{
  memset (&ciq_wcdma_metric, 0x0, sizeof(ciq_wcdma_metric));
}

/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_set_next_cur_idx

===========================================================================*/
/*!
    @brief
    
     Gets the next idxs
    @detail
    

    @return
    

    @Note
    


*/
/*=========================================================================*/
uint8 mmal_ciq_wcdma_metric_set_next_cur_idx(uint8 idx)
{

  (idx)++;

  if (idx >= MMAL_CIQ_WCDMA_METRIC_MSG_SIZE)
  {
    idx = 0;
  }

  return idx;
}

/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_reset

===========================================================================*/
/*!
    @brief
    
     resets the local variables
    @detail
    

    @return
    

    @Note
    


*/
/*=========================================================================*/
void mmal_ciq_wcdma_metric_reset(iq_metric_id_t metric_id, uint8 idx)
{

  switch (metric_id)
  {
    case IQ_METRIC_ID_GS34:
      mmal_ciq_metrics_free_item(
		  &ciq_wcdma_metric.gs34_info.gs34_ptr[idx],__FILENAME__, __LINE__);
      ciq_wcdma_metric.gs34_info.gs34_ptr[idx] = NULL;
      memset(
		  &ciq_wcdma_metric.gs34_info.gs34_metric[idx],0x0, sizeof(iq_metric_GS34_t));
      MSG_HIGH("GS34 reset index %d, Alloc Cnt %d, Dealloc Cnt %d", 
        idx, mmal_ciq_metric_stats_info.metrics_info.dsm_alloc_cnt, 
        mmal_ciq_metric_stats_info.metrics_info.dsm_dealloc_cnt);
      break;
    case IQ_METRIC_ID_GS35:
      mmal_ciq_metrics_free_item(
		  &ciq_wcdma_metric.gs35_info.gs35_ptr[idx],__FILENAME__, __LINE__);
      ciq_wcdma_metric.gs35_info.gs35_ptr[idx] = NULL;
      memset(
		  &ciq_wcdma_metric.gs35_info.gs35_metric[idx],0x0, sizeof(iq_metric_GS35_t));
      MSG_HIGH("GS35 reset index %d, Alloc Cnt %d, Dealloc Cnt %d", 
        idx, mmal_ciq_metric_stats_info.metrics_info.dsm_alloc_cnt, 
        mmal_ciq_metric_stats_info.metrics_info.dsm_dealloc_cnt);
      break;
    }
}


/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_pack_and_submit_gs34

===========================================================================*/
/*!
    @brief
    
     This function will be submitted only when both the NAS and the WCDMA info is
     populated in the metric. The flag is reset once the metric is submitted.
    @detail
    

    @return
    

    @Note
    Metric GS34 - UMTS/HSPA Layer 3 Downlink Message


*/
/*=========================================================================*/
void mmal_ciq_wcdma_metric_pack_and_submit_gs34( uint8 idx )
{
  iq_iovec_t                    iov[3];

  iov[0].data = &ciq_wcdma_metric.gs34_info.gs34_metric[idx];
  iov[0].len = sizeof(ciq_wcdma_metric.gs34_info.gs34_metric[idx]);

  iov[1].len = 0;
  iov[1].data=NULL;

  if (ciq_wcdma_metric.gs34_info.gs34_ptr[idx]->used <= 
	  MMAL_CIQ_WCDMA_METRIC_RRC_MSG_SIZE)
  {
    iov[1].data=ciq_wcdma_metric.gs34_info.gs34_ptr[idx]->data_ptr;
    iov[1].len =ciq_wcdma_metric.gs34_info.gs34_ptr[idx]->used;
  }

  iov[2].data = NULL;    // terminate the iq_iovec_t
  iov[2].len = 0;        // belt-and-suspenders, as either one set to NULL/0 will do

  /*! Call the RRC utility function to pase the RRC encoded bit string to obtain the 
  RRC message type, transaction id and the channel type*/
  /*! Send the metric */
  IQ_SubmitMetricV(IQ_METRIC_ID_GS34, &iov);

  mmal_ciq_metric_stats_info.metrics_info.num_gs34_metric_submitted++;

  // Clear the flags
  mmal_ciq_wcdma_metric_reset(IQ_METRIC_ID_GS34, idx);
 
}


/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_pack_and_submit_gs35

===========================================================================*/
/*!
    @brief
    
     This function will be submitted only when both the NAS and the WCDMA info is
     populated in the metric. The flag is reset once the metric is submitted.
    @detail
    

    @return
    


    @Note
    Metric GS35 - UMTS/HSPA Layer 3 Uplink Message
*/
/*=========================================================================*/
void mmal_ciq_wcdma_metric_pack_and_submit_gs35( uint8 idx)
{

  iq_iovec_t                    iov[3];

  iov[0].data = &ciq_wcdma_metric.gs35_info.gs35_metric[idx];
  iov[0].len = sizeof(ciq_wcdma_metric.gs35_info.gs35_metric[idx]);

  iov[1].len = 0;
  iov[1].data=NULL;

  if (ciq_wcdma_metric.gs35_info.gs35_ptr[idx]->used <= MMAL_CIQ_WCDMA_METRIC_RRC_MSG_SIZE)
  {
    iov[1].data=ciq_wcdma_metric.gs35_info.gs35_ptr[idx]->data_ptr;
    iov[1].len =ciq_wcdma_metric.gs35_info.gs35_ptr[idx]->used;
  }

  iov[2].data = NULL;    // terminate the iq_iovec_t
  iov[2].len = 0;        // belt-and-suspenders, as either one set to NULL/0 will do

  /*! Call the RRC utility function to pase the RRC encoded bit string to obtain the 
  RRC message type, transaction id and the channel type*/
  /*! Send the metric */
  IQ_SubmitMetricV(IQ_METRIC_ID_GS35, &iov);

  mmal_ciq_metric_stats_info.metrics_info.num_gs35_metric_submitted++;

  // Clear the flags
  mmal_ciq_wcdma_metric_reset(IQ_METRIC_ID_GS35, idx);

}

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
dsm_item_type **dsm_log_ptr
)
{
  uint8 idx, trid_proto, msg_type;

  /*! Ensure the param is good to work with */
  MMAL_ASSERT(dsm_log_ptr);
  MMAL_ASSERT(*dsm_log_ptr);

  /* Current Index */
  idx = ciq_wcdma_metric.gs34_info.nas_idx;

   
  /* Set the next current index */
  ciq_wcdma_metric.gs34_info.nas_idx = mmal_ciq_wcdma_metric_set_next_cur_idx(idx);

   
  MSG_HIGH("MMAL GS34 Storing NAS info in idx = %d", idx,0,0);

  trid_proto = dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);
  msg_type = dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);

  ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasProtocolDiscriminator = (trid_proto &0xF);
  ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasMessageType = (msg_type & 0x3F);

  switch (ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasProtocolDiscriminator )
  {
    case 0x3:
	  ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasTransactionId = ((trid_proto & 0xF0) >> 4);
	  ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasMessageType = (msg_type & 0x3F);
	  break;
    case 0xA:
	  ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasTransactionId = ((trid_proto & 0xF0) >> 4);
	  ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasMessageType = (msg_type);
	  break;
    default: 
	  ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasTransactionId = 0xFF;
	  ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasMessageType = (msg_type & 0x3F);
	  break;
  }
  MSG_HIGH("GS34 NAS MsgType %d, PD=%d, TranId=%d",
    ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasMessageType,
    ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasProtocolDiscriminator,
    ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasTransactionId);
  
  MSG_HIGH("GS34 RRC channel %d, msg_type %d, trans_id %d",
		ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucChannel,
		ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucRrcMessageType,
		ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucRrcTransactionId);
  if (ciq_wcdma_metric.gs34_info.gs34_ptr[idx])
  {
    mmal_ciq_wcdma_metric_pack_and_submit_gs34(idx);
  }

  /*Free the dsm items*/
  mmal_ciq_metrics_free_item(dsm_log_ptr,
  __FILENAME__, __LINE__);

  MSG_HIGH("GS34 nas done, Alloc Cnt %d, Dealloc Cnt %d", 
    mmal_ciq_metric_stats_info.metrics_info.dsm_alloc_cnt, 
    mmal_ciq_metric_stats_info.metrics_info.dsm_dealloc_cnt,0);

  

}
/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_pack_ul_nas_msg_info

===========================================================================*/
/*!
    @brief
    Packs the GS34 NAS related information by interecepting the NAS event 
    EVENT_NAS_MESSAGE_SENT

    @detail
    

    @return
    

    @Note
    Metric GS34 - UMTS/HSPA Layer 3 Downlink Message

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)
 
   // Indicates if this was sent to or from the UE 
   uint8 nas_message_direction;

   // Length of the NAS ota message
   uint32 nas_message_length;

   // NAS ota Message 
   uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

   LOG_RECORD_END

#define CN_TO_UE_DIRECTION            0
#define UE_TO_CN_DIRECTION            1


*/
/*=========================================================================*/
void mmal_ciq_wcdma_metric_pack_ul_nas_msg_info
(
dsm_item_type **dsm_log_ptr
)
{
  uint8 idx,trid_proto, msg_type;

  /* Current Index */
  idx = ciq_wcdma_metric.gs35_info.nas_idx;

  MSG_HIGH("MMAL GS35 Storing NAS info in idx = %d", idx,0,0);

  ciq_wcdma_metric.gs35_info.nas_info_present[idx] = TRUE;

  /* Set the next current index */
  ciq_wcdma_metric.gs35_info.nas_idx = mmal_ciq_wcdma_metric_set_next_cur_idx(idx);

  /*! Ensure the param is good to work with */
  MMAL_ASSERT(dsm_log_ptr);
  MMAL_ASSERT(*dsm_log_ptr);

  trid_proto = dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);
  msg_type = dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);

  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasProtocolDiscriminator = (trid_proto &0xF);

  switch (ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasProtocolDiscriminator)
  {
    case 0x3:
	  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasTransactionId = ((trid_proto & 0xF0) >> 4);
	  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasMessageType = (msg_type & 0x3F);
	  break;
    case 0xA:
	  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasTransactionId = ((trid_proto & 0xF0) >> 4);
	  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasMessageType = (msg_type);
	  break;
    default:
	  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasTransactionId = 0xFF;
	  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasMessageType = (msg_type & 0x3F);
	  break;
  }

  MSG_HIGH("GS35 NAS MsgType %d, PD=%d, TranId=%d",
  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasMessageType,
  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasProtocolDiscriminator,
  ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasTransactionId);
  
  MSG_HIGH("GS35 RRC channel %d, msg_type %d, trans_id %d",
		ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucChannel,
		ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucRrcMessageType,
		ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucRrcTransactionId);

 
  /* if the rrc information is present at that index as well then submit the metric */
#if 0
  if (ciq_wcdma_metric.gs35_info.gs35_ptr[idx])
  {
    mmal_ciq_wcdma_metric_pack_and_submit_gs35(idx);
  }
#endif
  
  mmal_ciq_metrics_free_item(dsm_log_ptr,
  __FILENAME__, __LINE__);
  
}

/*===========================================================================

  FUNCTION:  mmal_ciq_wcdma_metric_map_rrc_channel_type

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
 Metric GS34 - UMTS/HSPA Layer 3 Downlink Message

*/
/*=========================================================================*/
iq_channel_t mmal_ciq_wcdma_metric_map_rrc_channel_type
(
  uint8         channel_type,
  log_wcdma_trch_type   trch_type
)
{
  static iq_channel_t  iq_chan=IQ_CHANNEL_CCCH_FACH;


  switch (channel_type)
  {
    case 0: //RRCLOG_SIG_UL_CCCH
      switch (trch_type)
      {
        case WCDMA_TRCH_RACH:
          iq_chan = IQ_CHANNEL_CCCH_RACH;
          break;
        case WCDMA_TRCH_EDCH:
          iq_chan = IQ_CHANNEL_CCCH_RACH;
          break;
        default:
          break;
      }
      break;
    case 1: //RRCLOG_SIG_UL_DCCH
    switch (trch_type)
    {
      case WCDMA_TRCH_DCH:
        iq_chan = IQ_CHANNEL_DCCH_DCH;
        break;
      case WCDMA_TRCH_EDCH:
        iq_chan = IQ_CHANNEL_DCCH_EDCH;
        break;
      case WCDMA_TRCH_RACH:
        iq_chan = IQ_CHANNEL_DCCH_RACH;
        break;
      default:
        break;
    }
    break;
  case 2: //RRCLOG_SIG_DL_CCCH
    switch (trch_type)
    {
      case WCDMA_TRCH_FACH:
        iq_chan = IQ_CHANNEL_CCCH_FACH;
        break;
      case WCDMA_TRCH_HSDSCH:
        iq_chan = IQ_CHANNEL_CCCH_HSDSCH;
        break;
      default:
      break;
    }
    break;
  case 3: //RRCLOG_SIG_DL_DCCH
    switch (trch_type)
    {
      case WCDMA_TRCH_DCH:
        iq_chan = IQ_CHANNEL_DCCH_DCH;
        break;
      case WCDMA_TRCH_HSDSCH:
        iq_chan = IQ_CHANNEL_DCCH_HSDSCH;
        break;
      case WCDMA_TRCH_FACH:
        iq_chan = IQ_CHANNEL_DCCH_FACH;
        break;
      default:
        break;
    }
    break;
  case 4: //RRCLOG_SIG_DL_BCCH_BCH
    switch (trch_type)
    {
      case WCDMA_TRCH_BCH:
        iq_chan = IQ_CHANNEL_BCCH_BCH;
        break;
      case WCDMA_TRCH_HSDSCH:
        iq_chan = IQ_CHANNEL_BCCH_HSDSCH;
        break;
      default:
        break;
    }
    break;
  case 5: //RRCLOG_SIG_DL_BCCH_FACH
    switch (trch_type)
    {
      case WCDMA_TRCH_FACH:
        iq_chan = IQ_CHANNEL_BCCH_FACH;
        break;
      default:
        break;
    }
    break;
  case 6: //RRCLOG_SIG_DL_PCCH
    switch (trch_type)
    {
      case WCDMA_TRCH_PCH:
        iq_chan = IQ_CHANNEL_PCCH_PCH;
        break;
      case WCDMA_TRCH_HSDSCH:
        iq_chan = IQ_CHANNEL_PCCH_HSDSCH;
        break;
      default:
        break;
    }
    break;
  }
  return iq_chan;
}

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
dsm_item_type     *raw_sdu_ptr
)
{
  uint8 idx;

  mmal_ciq_fend_set_active_rat(WCDMA);

  /* DL Message */
  if (rrc_msg_info_ptr->direction == 0)
  {
    idx = ciq_wcdma_metric.gs34_info.rrc_idx;

	MSG_HIGH("MMAL GS34 Storing RRC info in idx = %d", idx,0,0);

#ifdef FEATURE_WCDMA	
    ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucChannel = 
    mmal_ciq_wcdma_metric_map_rrc_channel_type(
     rrc_msg_info_ptr->channel_type, rrc_msg_info_ptr->transport_channel_type);
#endif
    ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucRrcMessageType = rrc_msg_info_ptr->msg_type;
    ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucRrcTransactionId = 
    rrc_msg_info_ptr->transactionid;

	MSG_HIGH("MMAL GS34 channel %d, msg_type %d, trans_id %d",
		ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucChannel,
		ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucRrcMessageType,
		ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucRrcTransactionId);

     /*Delete the stale GS34 ptr at this idx*/  
    if (ciq_wcdma_metric.gs34_info.gs34_ptr[idx])
    {
      mmal_ciq_metrics_free_item(
		  &ciq_wcdma_metric.gs34_info.gs34_ptr[idx],__FILENAME__, __LINE__);
    }

	/* Store the current one */
    ciq_wcdma_metric.gs34_info.gs34_ptr[idx] = raw_sdu_ptr;

    /*#define RRCLOG_DOWNLINK_DIRECT_TRANSFER_MSG                       10*/
    if (rrc_msg_info_ptr->msg_type != 5) // otherwise wait for the NAS message
    {
      ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasMessageType = 0xff;
      ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasProtocolDiscriminator = 0xff;
      ciq_wcdma_metric.gs34_info.gs34_metric[idx].ucNasTransactionId = 0xff;
      mmal_ciq_wcdma_metric_pack_and_submit_gs34(idx);
      ciq_wcdma_metric.gs34_info.gs34_ptr[idx] = NULL;
    }
	else //Wait for the NAS message also 
	{
	  /*Increment the counter */
      ciq_wcdma_metric.gs34_info.rrc_idx = mmal_ciq_wcdma_metric_set_next_cur_idx(idx);
	}
  }
  else
  {
    idx = ciq_wcdma_metric.gs35_info.rrc_idx;

	MSG_HIGH("MMAL GS35 Storing RRC info in idx = %d", idx,0,0);

#ifdef FEATURE_WCDMA
    ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucChannel = 
    mmal_ciq_wcdma_metric_map_rrc_channel_type(
    rrc_msg_info_ptr->channel_type, rrc_msg_info_ptr->transport_channel_type);
#endif
    ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucRrcMessageType = rrc_msg_info_ptr->msg_type;
    ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucRrcTransactionId = 
    rrc_msg_info_ptr->transactionid;
   
	MSG_HIGH("MMAL GS35 channel %d, msg_type %d, trans_id %d",
		ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucChannel,
		ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucRrcMessageType,
		ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucRrcTransactionId);

    /*if the GS35 metric existed already then nas pkt didnt arrive*/  
    if (ciq_wcdma_metric.gs35_info.gs35_ptr[idx])
    {
      mmal_ciq_metrics_free_item(
		  &ciq_wcdma_metric.gs35_info.gs35_ptr[idx],__FILENAME__, __LINE__);
    }

    /*#define RRCLOG_UPLINK_DIRECT_TRANSFER_MSG                       57*/
    /*IDT => 5, UL_DCCH, UDT => 27 UL_DCCH, DDT => 5 DL_DCCH*/
    ciq_wcdma_metric.gs35_info.gs35_ptr[idx] = raw_sdu_ptr;

    if ((rrc_msg_info_ptr->msg_type != 27) &&
       (rrc_msg_info_ptr->msg_type != 5))
    {
      ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasMessageType = 0xff;
      ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasProtocolDiscriminator = 0xff;
      ciq_wcdma_metric.gs35_info.gs35_metric[idx].ucNasTransactionId = 0xff;

      mmal_ciq_wcdma_metric_pack_and_submit_gs35(idx);
      ciq_wcdma_metric.gs35_info.gs35_ptr[idx] = NULL;
      ciq_wcdma_metric.gs35_info.nas_info_present[idx]=FALSE;
      return;
    }
	else //Wait for the NAS message also 
	{
	  /* Check if the NAS message is already existing, if yes then submit the packet */
     if (ciq_wcdma_metric.gs35_info.nas_info_present[idx])
	  {
        mmal_ciq_wcdma_metric_pack_and_submit_gs35(idx);
	    ciq_wcdma_metric.gs35_info.nas_info_present[idx]=FALSE;
		ciq_wcdma_metric.gs35_info.rrc_idx = mmal_ciq_wcdma_metric_set_next_cur_idx(idx);
	  }
	  /*else
	  {
	    //Increment the counter 
        ciq_wcdma_metric.gs35_info.rrc_idx = mmal_ciq_wcdma_metric_set_next_cur_idx(idx);
	  }
	  */
    }
  }
}

/*===========================================================================
 FUNCTION: mmal_ciq_wcdma_metric_process_nas_msg

===========================================================================*/
/*!
@brief

This function intercepts the 0x713a NAS OTA

@detail

@return

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)

// Indicates if this was sent to or from the UE 
uint8 nas_message_direction;

// Length of the NAS ota message 
uint32 nas_message_length;

// NAS ota Message 
uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END

@Note 

=========================================================================*/
void mmal_ciq_wcdma_metric_process_nas_msg
(
uint8          dir,
dsm_item_type **dsm_log_ptr
)
{
  
  MMAL_ASSERT(dsm_log_ptr);
  MMAL_ASSERT(*dsm_log_ptr);

  if (dir == 0) //CN_TO_UE
  {
    mmal_ciq_wcdma_metric_pack_dl_nas_msg_info(dsm_log_ptr);
  }
  else
  {
    mmal_ciq_wcdma_metric_pack_ul_nas_msg_info(dsm_log_ptr);
  }
}

#endif /* FEATURE_MMAL*/
#endif