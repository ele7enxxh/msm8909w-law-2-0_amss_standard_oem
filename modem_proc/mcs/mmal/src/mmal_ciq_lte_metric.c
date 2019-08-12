/*!
  @file mmal_ciq_lte_metric.c

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_lte_metric.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
09/01/11   rj     Added Metrics LT03, LT04
08/02/11   sk     Add stats to detect when the LTE01,02,06 metric are
                  submitted
07/20/11   sk     Fixed indentation
07/19/11   sm     Discard duplicate event call back for LT06
07/14/11   rj     Added Metrics LT10 and LT11 
06/29/11   sk     Tap into ESM OTA IN and OUT msgs
06/23/11   sk     Fix the LT01, LT06 related fixes
06/13/11   sk     Support for the LT06 metric
05/31/11   sk     Support for the LTE02 metric
05/08/11   sk     Used the new mmal_ciq_metrics_free_item
05/08/11   sk     CR 286992 Fixed the issue where the wrong dsm item was deleted
                  after submitting the metric
05/05/11   sk     Modified the stats structure
05/04/11   sk     Featurize the code under FEATURE_MMAL_LTE
05/03/11   sk     Added some stats collection for the dsm item used
04/27/11   sk     fixed the iovec part for the lte01 metric
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES
               
==============================================================================*/
#include "mcs_variation.h"
#include <customer.h>
#include "mmal_cfg.h"

#ifdef FEATURE_MMAL_CIQ
#ifdef FEATURE_MMAL_LTE
#include <modem_mem.h>
#include <IQMetricsLT.h>
#include <stdio.h>
#include <dsm.h>
#include <dsmutil.h>
#include <diag.h>
#include <log.h>
#include <comdef.h>
#include <dsm.h>
#include "mmal_ciq_metrics.h"
#include "mmal_lte_metrics.h"
#include <lte_log_codes.h>
#include <lte_rrc_ext_api.h>
#include <bit.h>
#include <mmal_util.h>
#include <event_defs.h>
#include <naslog_v.h>
#include <cmapi.h>

#include <lte_cphy_msg.h>
#include <lte_mac.h>
#include <lte_mac_msg.h>
#include <lte_rlc_msg.h>

/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/
#define MMAL_CIQ_LTE_METRIC_RRC_MSG_SIZE   4096 
#define MMAL_CIQ_LTE_MAX_SRB_NUM           2
#define MMAL_CIQ_LTE_MAX_DRB_NUM           11
#define MMAL_CIQ_LTE_MAX_BEARER_ID         13
#define MMAL_CIQ_LTE_MAX_TX_POWER          23
#define MMAL_CIQ_LTE_MIN_TX_POWER         -50

typedef struct
{
  iq_metric_LT01_t       lte01_metric_ul;
  iq_metric_LT01_t       lte01_metric_dl;
  iq_lte_rrc_state_t     rrc_state;
  iq_metric_LT06_t       last_submitted_lt06;
  uint8                  msg[MMAL_CIQ_LTE_METRIC_RRC_MSG_SIZE];
} mmal_ciq_lte_metric_s;

mmal_ciq_lte_metric_s lte_metric_info={{NULL},{NULL},{IQ_LTE_RRC_STATE_NULL}};

uint8 mmal_ciq_lt12_numBR_lgtb[101]
 = { 0, 0, 3, 5, 6, 7, 8, 8, 9,10,10,
       10,11,11,11,12,12,12,13,13,13,
       13,13,14,14,14,14,14,14,15,15,
       15,15,15,15,15,16,16,16,16,16,
       16,16,16,16,17,17,17,17,17,17,
       17,17,17,17,17,17,18,18,18,18,
       18,18,18,18,18,18,18,18,18,18,
       19,19,19,19,19,19,19,19,19,19,
       19,19,19,19,19,19,19,19,19,20,
       20,20,20,20,20,20,20,20,20,20
    };

boolean mmal_ciq_lt06_discard_duplicate = TRUE;

typedef struct
{
  uint8           unique_rb_id;
  lte_rb_type_e   rb_type;
  void           *rb_stat_ptr;
  void           *rb_last_stat_ptr;
} mmal_ciq_lte_rb_map_s;

/*==============================================================================

                         INTERNAL VARIABLES

==============================================================================*/

extern iq_lte_eutra_meas_t     lt03_tMeas[32];

extern iq_lte_utra_meas_t      lt04_tMeasUtra[9];
extern iq_lte_geran_meas_t     lt04_tMeasGeran[9];
extern iq_lte_cdma2000_meas_t  lt04_tMeasCdma2000[9];

extern lte_mac_stats_data_s    lt07_mac_stats;

extern lte_rlcdl_stats_s       lt09_dl_stats;
extern lte_rlcul_stats_s       lt09_ul_stats;

static iq_lte_srb_stats_t      lt09_srb_stats[MMAL_CIQ_LTE_MAX_SRB_NUM];
static iq_lte_drb_stats_t      lt09_drb_stats[MMAL_CIQ_LTE_MAX_DRB_NUM];
static iq_lte_srb_stats_t      lt09_srb_last_stats[MMAL_CIQ_LTE_MAX_SRB_NUM];
static iq_lte_drb_stats_t      lt09_drb_last_stats[MMAL_CIQ_LTE_MAX_DRB_NUM];
static iq_metric_LT09_t        lt09_metric;
static mmal_ciq_lte_rb_map_s   lt09_rb_map[MMAL_CIQ_LTE_MAX_SRB_NUM + MMAL_CIQ_LTE_MAX_DRB_NUM];

static byte                    lt10_bearer_type_map[MMAL_CIQ_LTE_MAX_BEARER_ID] = 
                               { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static byte                    lt10_pdn_conn_map[MMAL_CIQ_LTE_MAX_BEARER_ID] = 
                               { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
                                 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/*==============================================================================

                    INTERNAL FUNCTION PROTOTYPES

==============================================================================*/



/*==============================================================================

                                FUNCTIONS

==============================================================================*/


/*===========================================================================

  FUNCTION:  convert_cmapi_to_ciq_rsrp

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
uint8 convert_cmapi_to_ciq_rsrp
(
  int16 cmapi_rsrp
)
{
  uint8 ciq_rsrp;

  /* CMAPI rsrp range: -440..-1400 */
  MMAL_MSG_SPRINTF_1( "MMAL: CMAPI RSRP == %d", cmapi_rsrp );
  if ( cmapi_rsrp < -1400 ) 
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSRP < -1400" );
    cmapi_rsrp = -1400;
  }
  else if ( cmapi_rsrp > -440 )
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSRP > -440" );
    cmapi_rsrp = -440;
  }

  /* CIQ rsrp range: 0..97 / 1..97 */
  ciq_rsrp = ( cmapi_rsrp / 10 ) + 141;
  MMAL_MSG_SPRINTF_1( "MMAL: CIQ RSRP == %d", cmapi_rsrp );

  return ciq_rsrp;
} /* convert_cmapi_to_ciq_rsrp */


/*===========================================================================

  FUNCTION:  convert_cmapi_to_ciq_rsrq

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
uint8 convert_cmapi_to_ciq_rsrq
(
  int16 cmapi_rsrq
)
{
  uint8 ciq_rsrq;

  /* CMAPI rsrq range: -200..-30 */
  MMAL_MSG_SPRINTF_1( "MMAL: CMAPI RSRQ == %d", cmapi_rsrq );
  if ( cmapi_rsrq < -200 ) 
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSRQ < -200" );
    cmapi_rsrq = -200;
  }
  else if ( cmapi_rsrq > -30 )
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSRQ > -30" );
    cmapi_rsrq = -30;
  }

  /* CIQ rsrq range: 0..34 */
  ciq_rsrq = ( cmapi_rsrq / 5 ) + 40;
  MMAL_MSG_SPRINTF_1( "MMAL: CIQ RSRQ == %d", cmapi_rsrq );

  return ciq_rsrq;
} /* convert_cmapi_to_ciq_rsrq */


/*===========================================================================

  FUNCTION:  convert_cmapi_to_ciq_rscp

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
uint8 convert_cmapi_to_ciq_rscp
(
  int16 cmapi_rscp
)
{
  uint8 ciq_rscp;

  /* CMAPI rscp range: -1200..-250 */
  MMAL_MSG_SPRINTF_1( "MMAL: CMAPI RSCP == %d", cmapi_rscp );
  if ( cmapi_rscp == 0 ) 
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSCP == 0, uninitialized!" );
    cmapi_rscp = -1200;
  }
  else if ( cmapi_rscp < -1200 ) 
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSCP < -1200" );
    cmapi_rscp = -1200;
  }
  else if ( cmapi_rscp > -250 )
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSCP > -250" );
    cmapi_rscp = -250;
  }

  /* CIQ rscp range: -4..91 */
  ciq_rscp = ( cmapi_rscp / 10 ) + 116;
  MMAL_MSG_SPRINTF_1( "MMAL: CIQ RSCP == %d", ciq_rscp );

  return ciq_rscp;
} /* convert_cmapi_to_ciq_rscp */


/*===========================================================================

  FUNCTION:  convert_cmapi_to_ciq_ecn0

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
uint8 convert_cmapi_to_ciq_ecn0
(
  int16 cmapi_ecn0
)
{
  uint8 ciq_ecn0;

  /* CMAPI ecn0 range: -500..0 */
  MMAL_MSG_SPRINTF_1( "MMAL: CMAPI EC/N0 == %d", cmapi_ecn0 );
  if ( cmapi_ecn0 == 1250 )
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI EC/N0 == 1275, uninitialized" );
    cmapi_ecn0 = -490;
  }
  else if ( cmapi_ecn0 < -490 ) 
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI EC/N0 < -490" );
    cmapi_ecn0 = -490;
  }
  else if ( cmapi_ecn0 > 0 )
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI EC/N0 > 0" );
    cmapi_ecn0 = 0;
  }

  /* CIQ ecn0 range: 0..49 */
  ciq_ecn0 = ( cmapi_ecn0 / 10 ) + 49;
  MMAL_MSG_SPRINTF_1( "MMAL: CIQ EC/N0 == %d", cmapi_ecn0 );

  return ciq_ecn0;
} /* convert_cmapi_to_ciq_ecn0 */


/*===========================================================================

  FUNCTION:  convert_cmapi_to_ciq_rssi

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
uint8 convert_cmapi_to_ciq_rssi
(
  int16 cmapi_rssi
)
{
  uint8 ciq_rssi;

  /* CMAPI rssi range: -2000..0, but specs say -110 to -48 */
  MMAL_MSG_SPRINTF_1( "MMAL: CMAPI RSSI == %d", cmapi_rssi );
  if ( cmapi_rssi < -1100 ) 
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSSI < -1100" );
    cmapi_rssi = -1100;
  }
  else if ( cmapi_rssi > -480 )
  {
    MMAL_MSG_ERROR( "MMAL: CMAPI RSSI > -480" );
    cmapi_rssi = -480;
  }

  /* CIQ rssi range: 1..63 */
  ciq_rssi = ( cmapi_rssi / 10 ) + 111;
  MMAL_MSG_SPRINTF_1( "MMAL: CIQ RSSI == %d", cmapi_rssi );

  return ciq_rssi;
} /* convert_cmapi_to_ciq_rssi */


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_map_dl_msg_type

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
uint8
mmal_ciq_lte_metric_map_ul_msg_type 
(
  const uint8 msg_class
)
{
  uint8 iq_msg_class = 0xFF;

  switch (msg_class)
  {
    case 0x00 : //RRCConnectionReestablishmentRequest
      iq_msg_class = IQ_LTE_RRC_UL_CCCH_MSG_RRC_CONN_REEST_REQ;
      break;
    case 0x01 : //RRCConnectionRequest
      iq_msg_class = IQ_LTE_RRC_UL_CCCH_MSG_RRC_CONN_REQ;
      break;
    case 0x80 : //CSFBParametersRequestCDMA2000
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_CSFB_PARAM_REQ;
      break;
    case 0x81 : //MeasurementReport
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_MEAS_REPORT;
      break;
    case 0x82 : //RRCConnectionReconfigurationComplete
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_RRC_CONN_RECFG_CMP;
      break;
    case 0x83 : //RRCConnectionReestablishmentComplete
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_RRC_CONN_REEST_CMP;
      break;
    case 0x84 : //RRCConnectionSetupComplete
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_RRC_CONN_SETUP_CMP;
      break;
    case 0x85 : //SecurityModeComplete
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_SECUR_MODE_CMP;
      break;
    case 0x86 : //SecurityModeFailure
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_SECUR_MODE_FAIL;
      break;
    case 0x87 : //UECapabilityInformation
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_UE_CAP_INFO;
      break;
    case 0x88 : //ULHandoverPreparationTransfer
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_HO_PREP_TXR;
      break;
    case 0x89 : //ULInformationTransfer
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_INFO_TXR;
      break;
    case 0x8A : //CounterCheckResponse
      iq_msg_class = IQ_LTE_RRC_UL_DCCH_MSG_CNTR_CHECK_RESP;
      break;
  }
  return iq_msg_class;
}


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_map_dl_msg_type

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    
Identifies the downlink message received

*/
/*=========================================================================*/

uint8
mmal_ciq_lte_metric_map_dl_msg_type 
(
const uint8 msg_class,
uint8 *msg_type
)
{
  uint8 iq_msg_class = 0xFE;

  switch (msg_class)
  {
    case 0x00  : // MasterInformationBlock 
      iq_msg_class=IQ_LTE_RRC_BCCH_BCH_MSG_MASTER_INFO_BLK;
      *msg_type = IQ_LTE_RRC_MSG_CLASS_BCCH_BCH;
      break;

    case 0x01 : // SystemInformationBlockType1
      iq_msg_class=IQ_LTE_RRC_BCCH_DL_SCH_MSG_SYS_INFO_BLK_TYPE1;
      *msg_type = IQ_LTE_RRC_MSG_CLASS_BCCH_DL_SCH;
      break;

    case 0x02 : // SystemInformationBlockType2
    case 0x03 : // SystemInformationBlockType3
    case 0x04 : // SystemInformationBlockType4
    case 0x05 : // SystemInformationBlockType5
    case 0x06 : // SystemInformationBlockType6
    case 0x07 : // SystemInformationBlockType7
    case 0x08 : // SystemInformationBlockType8
    case 0x09  : // SystemInformationBlockType9
    case 0x0A  : // SystemInformationBlockType10
    case 0x0B  : // SystemInformationBlockType11
      iq_msg_class=IQ_LTE_RRC_BCCH_DL_SCH_MSG_SYS_INFO;
      *msg_type = IQ_LTE_RRC_MSG_CLASS_BCCH_DL_SCH;
      break;

    case 0x40  : // Paging
      iq_msg_class=IQ_LTE_RRC_PCCH_MSG_PAGING;
      break;

    case 0x48  : // RRCConnectionReestablishment
      iq_msg_class=IQ_LTE_RRC_DL_CCCH_MSG_RRC_CONN_REEST;
      break;

    case 0x49  : // RRCConnectionReestablishmentReject
      iq_msg_class=IQ_LTE_RRC_DL_CCCH_MSG_RRC_CONN_REEST_REJ;
      break;

    case 0x4A : // RRCConnectionReject
      iq_msg_class=IQ_LTE_RRC_DL_CCCH_MSG_RRC_CONN_REJ;
      break;

    case 0x4B : // RRCConnectionSetup
      iq_msg_class=IQ_LTE_RRC_DL_CCCH_MSG_RRC_CONN_SETUP;
      break;

    case 0x80 : // CSFBParametersRequestCDMA2000
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_CSFB_PARAM_RESP;
      break;

    case 0x81 : // DLInformationTransfer
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_DL_INFO_TRANSFER;
      break;

    case 0x82 : // HandoverFromEUTRAPreparationRequest
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_HO_FROM_EUTRA_REQ;
      break;

    case 0x83 : // MobilityFromEUTRACommand
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_MOB_FROM_EUTRA_CMD;
      break;

    case 0x84 : // RRCConnectionReconfiguration
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_RRC_CONN_RECFG;
      break;
    case 0x85 : // RRCConnectionRelease
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_RRC_CONN_REL;
      break;

    case 0x86 : // SecurityModeCommand
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_SECUR_MODE_CMD;
      break;

    case 0x87 : // UECapabilityEnquiry
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_UE_CAP_ENQ;
      break;

    case 0x88 : // CounterCheck
      iq_msg_class=IQ_LTE_RRC_DL_DCCH_MSG_CNTR_CHECK;
      break;
  }
  return iq_msg_class;
}


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_map_lc_type

===========================================================================*/
/*!
    @brief
    

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/

iq_lte_rrc_msg_class_t
mmal_ciq_lte_metric_map_lc_type 
(
const uint8 lc_type
)
{
  iq_lte_rrc_msg_class_t  iq_msg_type = 0xFF;

  switch (lc_type)
  {
    case 0x1: //LTE_RRC_LOG_LC_DL_BCCH_TYPE
      iq_msg_type = IQ_LTE_RRC_MSG_CLASS_BCCH_BCH;  
      break;
    case 0x2: //LTE_RRC_LOG_LC_DL_PCCH_TYPE
      iq_msg_type = IQ_LTE_RRC_MSG_CLASS_PCCH;
      break;
    case 0x3: //LTE_RRC_LOG_LC_DL_CCCH_TYPE
      iq_msg_type = IQ_LTE_RRC_MSG_CLASS_DL_CCCH;
      break;
    case 0x4: //LTE_RRC_LOG_LC_DL_DCCH_TYPE
      iq_msg_type = IQ_LTE_RRC_MSG_CLASS_DL_DCCH;
      break;
    case 0x5: //LTE_RRC_LOG_LC_UL_CCCH_TYPE
      iq_msg_type = IQ_LTE_RRC_MSG_CLASS_UL_CCCH;
      break;
    case 0x6: //LTE_RRC_LOG_LC_UL_DCCH_TYPE
      iq_msg_type = IQ_LTE_RRC_MSG_CLASS_UL_DCCH;
      break;
  }

  return iq_msg_type;
}

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_ul_msg_lt01

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_ul_msg_lt01

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_ul_msg_lt01 
(
  uint8 msg_type, 
  uint8 lc_type,
  uint16 encoded_msg_len, 
  dsm_item_type *raw_payload_ptr
)
{
  iq_iovec_t                    iov[3];
  iq_metric_LT01_t              lte01_metric_ul;

  
  lte01_metric_ul.ucMsgClass = 
  mmal_ciq_lte_metric_map_lc_type(lc_type);
  lte01_metric_ul.ucMsgType = 
  mmal_ciq_lte_metric_map_ul_msg_type(msg_type); 
   
  /*------------------------------------------------------------------------
    Fill the iovector and submit the metric 
  -------------------------------------------------------------------------*/
  iov[0].data = &lte01_metric_ul;
  iov[0].len = sizeof(lte01_metric_ul);
 
  iov[1].len = 0;
  iov[1].data=NULL;
  
  if (encoded_msg_len <= MMAL_CIQ_LTE_METRIC_RRC_MSG_SIZE)
  {
    iov[1].data=raw_payload_ptr->data_ptr;
      
    iov[1].len =encoded_msg_len;
  }

  iov[2].data = NULL;    // terminate the iq_iovec_t
  iov[2].len = 0;        // belt-and-suspenders, as either one set to NULL/0 will do

  IQ_SubmitMetricV(IQ_METRIC_ID_LT01, (iq_iovec_t *)&iov);
  
  mmal_ciq_metric_stats_info.metrics_info.num_lte01_metric_submitted++; 
}

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_dl_msg_lt01

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_dl_msg_lt01

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_dl_msg_lt01 
(
  uint8 msg_type, 
  uint8 lc_type,
  uint16 encoded_msg_len, 
  dsm_item_type *raw_payload_ptr
)
{
  iq_iovec_t                    iov[3];
  iq_metric_LT01_t              lte01_metric_dl;

  
  lte01_metric_dl.ucMsgClass = 
  mmal_ciq_lte_metric_map_lc_type(lc_type);
  lte01_metric_dl.ucMsgType = mmal_ciq_lte_metric_map_dl_msg_type(
        msg_type, &lte01_metric_dl.ucMsgClass); 
   
  /*------------------------------------------------------------------------
    Fill the iovector and submit the metric 
  -------------------------------------------------------------------------*/
  iov[0].data = &lte01_metric_dl;
  iov[0].len = sizeof(lte01_metric_dl);
 
  iov[1].len = 0;
  iov[1].data=NULL;
  
  if (encoded_msg_len <= MMAL_CIQ_LTE_METRIC_RRC_MSG_SIZE)
  {
    iov[1].data=raw_payload_ptr->data_ptr;
      
    iov[1].len =encoded_msg_len;
  }

  MSG_HIGH("LT01 enc_msg_len %d ucMsgType %d msgClass %d", 
    encoded_msg_len,lte01_metric_dl.ucMsgType, lte01_metric_dl.ucMsgClass);

  iov[2].data = NULL;  // terminate the iq_iovec_t
  iov[2].len = 0;      // belt-and-suspenders, as either one set to NULL will do

  IQ_SubmitMetricV(IQ_METRIC_ID_LT01, (iq_iovec_t *)&iov);

  mmal_ciq_metric_stats_info.metrics_info.num_lte01_metric_submitted++;
}

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
)
{
  /*0-5 corresponds to DL messages*/
  if ( pdu_num < 6 )
  {
    mmal_ciq_lte_metric_pack_and_submit_dl_msg_lt01( msg_type, 
                                                     lc_type, 
                                                     encoded_msg_len, 
                                                     log_payload_ptr );
  }
  else
  {
    mmal_ciq_lte_metric_pack_and_submit_ul_msg_lt01( msg_type, 
                                                     lc_type, 
                                                     encoded_msg_len, 
                                                     log_payload_ptr );
  }

  mmal_ciq_metrics_free_item( &log_payload_ptr, __FILENAME__, __LINE__ );
}


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
)
{
  static iq_metric_LT02_t   lte02_metric;
  iq_iovec_t                iov[3];
  uint16                    read_len;
  uint16                    bytes_read;

  /* Ensure the param is good to work with */
  MMAL_ASSERT(log_payload_ptr);

  /* Assume LTE type: set bits 0-6 */
  lte02_metric.ucDedInfoType = IQ_LTE_DED_INFO_TYPE_3GPP_NAS;

  switch (log_code)
  {
    case LOG_LTE_NAS_EMM_OTA_IN_MSG_LOG_C:
    case LOG_LTE_NAS_ESM_OTA_IN_MSG_LOG_C:
      /* do nothing for downlink */
      break;
    case LOG_LTE_NAS_EMM_OTA_OUT_MSG_LOG_C:
    case LOG_LTE_NAS_ESM_OTA_OUT_MSG_LOG_C:
      /* set bit 7 for uplink */
      lte02_metric.ucDedInfoType |= 0x80;
      break;
    default:
      /* should not trigger */
      MMAL_ASSERT(0);
      break;
  }

  /* Read the log header and the first 4 bytes - discard them */
  read_len = sizeof(log_hdr_type) + 4;

  bytes_read = dsmi_pullup( &log_payload_ptr, 
                            lte_metric_info.msg, 
                            read_len,
                            __FILENAME__,
                            __LINE__ );

  MMAL_ASSERT(bytes_read == read_len);

  /* Now read the remaining message */
  read_len = log_payload_ptr->used;

  /* Only if it is small enough to fit in the buffer */
  if (read_len <= MMAL_CIQ_LTE_METRIC_RRC_MSG_SIZE)
  {
    bytes_read = dsmi_pullup( &log_payload_ptr, 
                              lte_metric_info.msg, 
                              read_len,
                              __FILENAME__,
                              __LINE__ );
  
    MMAL_ASSERT(bytes_read == read_len);
  }
  else 
  {
    read_len = 0;
  }

  /* Send the metric */
  iov[0].data = &lte02_metric;
  iov[0].len  = sizeof(lte02_metric);

  iov[1].data = lte_metric_info.msg;
  iov[1].len  = read_len;

  iov[2].data = NULL;     // terminate the iq_iovec_t
  iov[2].len  = 0;        // belt-and-suspenders, as either one set to NULL/0 will do

  IQ_SubmitMetricV(IQ_METRIC_ID_LT02, (iq_iovec_t *)&iov);

  mmal_ciq_metric_stats_info.metrics_info.num_lte02_metric_submitted++;

  /* Release the dsm item */
  mmal_ciq_metrics_free_item( &log_payload_ptr, 
                              __FILENAME__, 
                              __LINE__ );

}


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
  timer_cb_data_type               data
)
{
  mmal_timer_cb_s      log_info; 

  NOTUSED( data );

  MMAL_MSG( "MMAL: LT03 timer callback fired" );

  log_info.hdr.id = MMAL_CIQ_METRICS_LT03_REQ;

  /* Send a message to the MMAL Task Q */
  if (!mmal_msg_send(&log_info.hdr))
  {
    MMAL_MSG_ERROR( "MMAL: LT03 timer callback failed to signal MMAL task" );
  }
} /* mmal_ciq_lte_metric_lt03_timer_cb */


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
void mmal_ciq_lte_metric_pack_and_submit_lt03()
{
  cmapi_rat_meas_info_s_type       meas_info;
  iq_metric_LT03_t                 lt03_metric;
  iq_iovec_t                       iov[3];
  cmapi_err_e_type                 return_code; 
  uint16                           i, j, k = 0;
  uint8                            digit;

  MMAL_MSG("MMAL: within LT03 Callback Function");

  memset( &meas_info, 0, sizeof(meas_info) );
  memset( &lt03_metric, 0, sizeof(lt03_metric) );

  if( (return_code = cmapi_get_rat_meas_info( CMAPI_SYS_MODE_LTE, &meas_info )) == CMAPI_SUCCESS )
  {
    MMAL_MSG_SPRINTF_1( "Return Code for CMAPI_SYS_MODE_LTE: %d", return_code );
    if( meas_info.is_service_available && 
        meas_info.sys_mode == CMAPI_SYS_MODE_LTE &&
        meas_info.meas_info_u.lte_nbr_info.valid )
    {
      /* Fill in Serving Cell Information */
      MMAL_MSG_SPRINTF_2( "MMAL: MCC/MNC for serving cell = 0x%x / 0x%x", 
                          meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.mcc,
                          meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.mnc );

      lt03_metric.wMcc = 0;
      for (i = 0; i != 3; i++)
      {
        /* each digit takes up one nibble */
        digit = ( meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.mcc >> ( i << 2 ) ) & 0xF;
        if ( digit < 10 )
        {
          lt03_metric.wMcc *= 10;
          lt03_metric.wMcc += digit;
        }
      }

      lt03_metric.wMnc = 0;
      for (i = 0; i != 3; i++)
      {
        /* each digit takes up one nibble */
        digit = ( meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.mnc >> ( i << 2 ) ) & 0xF;
        if ( digit < 10 )
        {
          lt03_metric.wMnc *= 10;
          lt03_metric.wMnc += digit;
        }
      }

      lt03_metric.dwCellId        = meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.serving_cell_id;
      lt03_metric.wTac            = meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.tac;
      lt03_metric.wServEarfcn     = meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.earfcn;
      lt03_metric.wServPhysCellId = meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.cell.pci;
      lt03_metric.ucServRsrp      = convert_cmapi_to_ciq_rsrp( meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.cell.rsrp );
      lt03_metric.ucServRsrq      = convert_cmapi_to_ciq_rsrq( meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.cell.rsrq );
      lt03_metric.scServRssnr     = meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.serv_rssnr;
      lt03_metric.ucFreqBandInd   = meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.freq_band_ind;
      lt03_metric.ucDlBandwidth   = meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.dl_bandwidth;
      lt03_metric.ucUlBandwidth   = meas_info.meas_info_u.lte_nbr_info.lte_serving_cell.ul_bandwidth;


      /* Setting ucPad to 0xFF per the spec */
      lt03_metric.ucPad           = 0xFF;

      lt03_metric.ucNumMeas       = 0;

      for (i = 0; i < meas_info.meas_info_u.lte_nbr_info.lte_inter.num_freqs; i++)
      {
        for (j = 0; j < meas_info.meas_info_u.lte_nbr_info.lte_inter.freqs[i].num_lte_cells; j++) 
        {
          k = lt03_metric.ucNumMeas;
          lt03_tMeas[k].wEarfcn     = meas_info.meas_info_u.lte_nbr_info.lte_inter.freqs[i].earfcn;
          lt03_tMeas[k].wPhysCellId = meas_info.meas_info_u.lte_nbr_info.lte_inter.freqs[i].cells[j].pci;
          lt03_tMeas[k].ucRsrp      = convert_cmapi_to_ciq_rsrp( meas_info.meas_info_u.lte_nbr_info.lte_inter.freqs[i].cells[j].rsrp );
          lt03_tMeas[k].ucRsrq      = convert_cmapi_to_ciq_rsrq( meas_info.meas_info_u.lte_nbr_info.lte_inter.freqs[i].cells[j].rsrq );
          lt03_metric.ucNumMeas++;
        }
      }

      for (i = 0; i < meas_info.meas_info_u.lte_nbr_info.lte_intra.num_lte_cells; i++)
      {
        k = lt03_metric.ucNumMeas;
        lt03_tMeas[k].wEarfcn     = meas_info.meas_info_u.lte_nbr_info.lte_intra.earfcn;
        lt03_tMeas[k].wPhysCellId = meas_info.meas_info_u.lte_nbr_info.lte_intra.cell[i].pci;
        lt03_tMeas[k].ucRsrp      = convert_cmapi_to_ciq_rsrp( meas_info.meas_info_u.lte_nbr_info.lte_intra.cell[i].rsrp );
        lt03_tMeas[k].ucRsrq      = convert_cmapi_to_ciq_rsrq( meas_info.meas_info_u.lte_nbr_info.lte_intra.cell[i].rsrq );
        lt03_metric.ucNumMeas++;
      }

      iov[0].data = &lt03_metric;
      iov[0].len  = sizeof(lt03_metric);

      iov[1].data = &lt03_tMeas;
      iov[1].len  = lt03_metric.ucNumMeas * sizeof(iq_lte_eutra_meas_t);

      iov[2].data = NULL;
      iov[2].len  = 0;

      MMAL_MSG("MMAL: Submitting LT03");
      IQ_SubmitMetricV(IQ_METRIC_ID_LT03, (iq_iovec_t *)&iov);
    }
    else
    {
      MMAL_MSG("MMAL: LT03 - Service not available, not submitting")
    }
  }
  else
  {
    MMAL_MSG_SPRINTF_1("MMAL: Unable to Submit LT03: Return Code "
                       "from cmapi_get_rat_meas_info: %d",return_code);
  }
} /* mmal_ciq_lte_metric_pack_and_submit_lt03 */


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
)
{
  mmal_timer_cb_s      log_info; 

  NOTUSED( data );

  MMAL_MSG( "MMAL: LT04 timer callback fired" );

  log_info.hdr.id = MMAL_CIQ_METRICS_LT04_REQ;

  /* Send a message to the MMAL Task Q */
  if (!mmal_msg_send(&log_info.hdr))
  {
    MMAL_MSG_ERROR( "MMAL: LT04 timer callback failed to signal MMAL task" );
  }
} /* mmal_ciq_lte_metric_lt04_timer_cb */


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
void mmal_ciq_lte_metric_pack_and_submit_lt04()
{
  cmapi_rat_meas_info_s_type       meas_info;
  iq_metric_LT04_t                 lt04_metric;
  iq_iovec_t                       iov[5];
  cmapi_err_e_type                 return_code; 
  uint16 i, j, k;

  MMAL_MSG("MMAL: within LT04 Callback Function");

  memset( &lt04_metric, 0, sizeof(lt04_metric) );

  /* Setting ucPad to 0xFF per the spec */
  lt04_metric.ucPad           =  0xFF;

  memset( &meas_info, 0, sizeof(meas_info) );

  if( (return_code = cmapi_get_rat_meas_info( CMAPI_SYS_MODE_LTE, &meas_info )) == CMAPI_SUCCESS )
  {
    MMAL_MSG_SPRINTF_1("Return Code for CMAPI_SYS_MODE_LTE: %d",return_code);
    if( meas_info.is_service_available &&
        meas_info.sys_mode == CMAPI_SYS_MODE_LTE &&
        meas_info.meas_info_u.lte_nbr_info.valid )
    {
      lt04_metric.ucNumMeasUtra      =  0;

      MMAL_MSG_SPRINTF_1( "MMAL: LT04 - num W freqs = %d",
                          meas_info.meas_info_u.lte_nbr_info.wcdma.num_wcdma_freqs );
      for (i = 0; i < meas_info.meas_info_u.lte_nbr_info.wcdma.num_wcdma_freqs; i++)
      {
        MMAL_MSG_SPRINTF_2( "MMAL: LT04 - num cells on W freq %d = %d",
                            meas_info.meas_info_u.lte_nbr_info.wcdma.freq[i].uarfcn,
                            meas_info.meas_info_u.lte_nbr_info.wcdma.freq[i].num_wcdma_cells );
        for (j = 0; j < meas_info.meas_info_u.lte_nbr_info.wcdma.freq[i].num_wcdma_cells; j++) 
        {  
          k = lt04_metric.ucNumMeasUtra;
          lt04_tMeasUtra[k].wUarfcn   =   meas_info.meas_info_u.lte_nbr_info.wcdma.freq[i].uarfcn;
          lt04_tMeasUtra[k].wPsc      =   meas_info.meas_info_u.lte_nbr_info.wcdma.freq[i].cell[j].psc;
          lt04_tMeasUtra[k].cRscp     =   convert_cmapi_to_ciq_rscp( meas_info.meas_info_u.lte_nbr_info.wcdma.freq[i].cell[j].cpich_rscp );
          lt04_tMeasUtra[k].ucEcN0    =   convert_cmapi_to_ciq_ecn0( meas_info.meas_info_u.lte_nbr_info.wcdma.freq[i].cell[j].cpich_ecno );
          lt04_metric.ucNumMeasUtra++;
        }
      }
    
      MMAL_MSG_SPRINTF_1("Return Code for CMAPI_SYS_MODE_LTE: %d",return_code);
      lt04_metric.ucNumMeasGeran     =  0;

      MMAL_MSG_SPRINTF_1( "MMAL: LT04 - num G freqs = %d",
                          meas_info.meas_info_u.lte_nbr_info.gsm.num_freq_groups );
      for (i = 0; i < meas_info.meas_info_u.lte_nbr_info.gsm.num_freq_groups; i++)
      {
        MMAL_MSG_SPRINTF_2( "MMAL: LT04 - num cells on G freq %d = %d",
                            meas_info.meas_info_u.lte_nbr_info.gsm.freq_group[i].arfcn[0].arfcn,
                            meas_info.meas_info_u.lte_nbr_info.gsm.freq_group[i].num_gsm_arfcn );
        for (j = 0; j < meas_info.meas_info_u.lte_nbr_info.gsm.freq_group[i].num_gsm_arfcn; j++) 
        {
          MMAL_MSG_SPRINTF_2( "MMAL: LT04 - double check - num cells on G freq %d = %d",
                              meas_info.meas_info_u.lte_nbr_info.gsm.freq_group[i].arfcn[j].arfcn,
                              meas_info.meas_info_u.lte_nbr_info.gsm.freq_group[i].num_gsm_arfcn );
          k = lt04_metric.ucNumMeasGeran;
          lt04_tMeasGeran[k].wArfcn   =   meas_info.meas_info_u.lte_nbr_info.gsm.freq_group[i].arfcn[j].arfcn;
          lt04_tMeasGeran[k].ucBsic   =   meas_info.meas_info_u.lte_nbr_info.gsm.freq_group[i].arfcn[j].bsic_id;
          lt04_tMeasGeran[k].ucRssi   =   convert_cmapi_to_ciq_rssi( meas_info.meas_info_u.lte_nbr_info.gsm.freq_group[i].arfcn[j].rssi );
          lt04_metric.ucNumMeasGeran++;
        }
      }
    
      MMAL_MSG_SPRINTF_1("Return Code for CMAPI_SYS_MODE_LTE: %d",return_code);
      lt04_metric.ucNumMeasCdma2000  =  0;

      for (i = 0; i < meas_info.meas_info_u.lte_nbr_info.cdma_1x.num_cdma_freqs; i++)
      {
        for (j = 0; j < meas_info.meas_info_u.lte_nbr_info.cdma_1x.freq[i].num_cdma_cells; j++) 
        {  
          k = lt04_metric.ucNumMeasCdma2000;
          
          lt04_tMeasCdma2000[k].wChannelNum      =   meas_info.meas_info_u.lte_nbr_info.cdma_1x.freq[i].channel_num;
          lt04_tMeasCdma2000[k].wPilotPnOffset   =   meas_info.meas_info_u.lte_nbr_info.cdma_1x.freq[i].cell[j].pilot_pn_offset;
          lt04_tMeasCdma2000[k].wPilotPnPhase    =   meas_info.meas_info_u.lte_nbr_info.cdma_1x.freq[i].cell[j].pilot_pn_phase;
          lt04_tMeasCdma2000[k].ucPilotStrength  =   meas_info.meas_info_u.lte_nbr_info.cdma_1x.freq[i].cell[j].pilot_strength;
          lt04_tMeasCdma2000[k].ucBandClass      =   meas_info.meas_info_u.lte_nbr_info.cdma_1x.freq[i].band_class;
          lt04_metric.ucNumMeasCdma2000++;
        }
      }

      for (i = 0; i < meas_info.meas_info_u.lte_nbr_info.cdma_hrpd.num_cdma_freqs; i++)
      {
        for (j = 0; j < meas_info.meas_info_u.lte_nbr_info.cdma_hrpd.freq[i].num_cdma_cells; j++) 
        {  
          k = lt04_metric.ucNumMeasCdma2000;
          
          lt04_tMeasCdma2000[k].wChannelNum      =   meas_info.meas_info_u.lte_nbr_info.cdma_hrpd.freq[i].channel_num;
          lt04_tMeasCdma2000[k].wPilotPnOffset   =   meas_info.meas_info_u.lte_nbr_info.cdma_hrpd.freq[i].cell[j].pilot_pn_offset;
          lt04_tMeasCdma2000[k].wPilotPnPhase    =   meas_info.meas_info_u.lte_nbr_info.cdma_hrpd.freq[i].cell[j].pilot_pn_phase;
          lt04_tMeasCdma2000[k].ucPilotStrength  =   meas_info.meas_info_u.lte_nbr_info.cdma_hrpd.freq[i].cell[j].pilot_strength;
          lt04_tMeasCdma2000[k].ucBandClass      =   meas_info.meas_info_u.lte_nbr_info.cdma_hrpd.freq[i].band_class;
          lt04_metric.ucNumMeasCdma2000++;
        }
      }
  
      iov[0].data = &lt04_metric;
      iov[0].len  = sizeof(lt04_metric);

      iov[1].data = &lt04_tMeasUtra;
      iov[1].len  = lt04_metric.ucNumMeasUtra   * sizeof(iq_lte_utra_meas_t);

      iov[2].data = &lt04_tMeasGeran;
      iov[2].len  = lt04_metric.ucNumMeasGeran  * sizeof(iq_lte_geran_meas_t);

      iov[3].data = &lt04_tMeasCdma2000;
      iov[3].len  = lt04_metric.ucNumMeasCdma2000  * sizeof(iq_lte_cdma2000_meas_t);

      iov[4].data = NULL;
      iov[4].len  = 0;

      MMAL_MSG("MMAL: Submitting LT04 ");
      IQ_SubmitMetricV(IQ_METRIC_ID_LT04, (iq_iovec_t *)&iov);
    }
    else
    {
      MMAL_MSG("MMAL: LT04 - Service not available, not submitting")
    }
  }
  else
  {
    MMAL_MSG_SPRINTF_1("MMAL: Unable to Submit LT04: Return Code "
                       "from cmapi_get_rat_meas_info: %d",return_code);
  }

} /* mmal_ciq_lte_metric_pack_and_submit_lt04 */

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
)
{
  iq_metric_LT05_t               metric;
  iq_lte_neighb_eutra_t         *l_n_list;
  iq_lte_neighb_utra_t          *w_n_list;
  iq_lte_neighb_geran_t         *g_n_list;
  iq_lte_neighb_cdma2000_t      *c2k_n_list;
  iq_iovec_t                     iov[6];

  uint32                         index, neighbor_index;
  uint32                         bitmask, neighbor_bitmask;
  lte_cphy_meas_obj_id_t         meas_obj_id;

  lte_cphy_meas_obj_eutra_s      eutra;
  lte_cphy_meas_obj_wcdma_s      wcdma;
  lte_cphy_meas_obj_gsm_s        gsm;
  lte_cphy_meas_obj_cdma_s       cdma;

/*---------------------------------------------------------------------------*/

  memset( &metric, 0, sizeof(metric) );

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - neighbor meas id bitmask = 0x%x", 
                      conn_msg->meas_id_valid_bitmask );

  /* first, scan the structure to find out how many neighbors we need */
  for ( index = 0; index != LTE_CPHY_MEAS_MAX_MEAS_ID; index++ )
  {
    bitmask = 1 << index;

    /* check if the measurment id for this bit is valid */
    if ( ( conn_msg->meas_id_valid_bitmask & bitmask ) != 0 )
    {
      /* grab the object */
      MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - meas obj id = %d",
                          conn_msg->meas_cfg_ptr->meas_id_list[index].meas_obj_id );

      meas_obj_id = conn_msg->meas_cfg_ptr->meas_id_list[index].meas_obj_id - 1;

      /* check which type of object we're dealing with */
      switch ( conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].type )
      {
        case LTE_CPHY_MEAS_OBJ_TYPE_EUTRA:
          /* store the LTE object info */
          eutra = conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].obj_info.eutra;
          MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - LTE active meas bitmask = 0x%x", 
                              eutra.neigh_cells.active_bitmask );

          /* check every neighbor in this frequency */
          for ( neighbor_index = 0; neighbor_index != LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS; neighbor_index++ )
          {
            neighbor_bitmask = 1 << neighbor_index;

            /* check if this neighbor is active */
            if ( ( eutra.neigh_cells.active_bitmask & neighbor_bitmask ) != 0 )
            {
              metric.ucNumNeighbEutra++;
            }
          }
          break;

        case LTE_CPHY_MEAS_OBJ_TYPE_WCDMA:
          /* store the WCDMA object info */
          wcdma = conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].obj_info.wcdma;
          MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - WCDMA active meas bitmask = 0x%x", 
                              wcdma.active_psc_bitmask );

          /* check every neighbor in this frequency */
          for ( neighbor_index = 0; neighbor_index != LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS; neighbor_index++ )
          {
            neighbor_bitmask = 1 << neighbor_index;

            /* check if this neighbor is active */
            if ( ( wcdma.active_psc_bitmask & neighbor_bitmask ) != 0 )
            {
              metric.ucNumNeighbUtra++;
            }
          }
          break;

        case LTE_CPHY_MEAS_OBJ_TYPE_GERAN:
          /* store the GSM object info */
          gsm = conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].obj_info.gsm;
          MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - GSM num freqs = %d", 
                              gsm.num_freqs );

          /* check every frequency in this object */
          for ( neighbor_index = 0; ( neighbor_index != gsm.num_freqs ) && 
                                    ( neighbor_index != LTE_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP ); neighbor_index++ )
          {
            metric.ucNumNeighbGeran++;
          }
          break;

        case LTE_CPHY_MEAS_OBJ_TYPE_CDMA:
          /* store the CDMA object info */
          cdma = conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].obj_info.cdma;
          MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - CDMA active meas bitmask = 0x%x", 
                              cdma.nbr_cells.active_bitmask );

          for ( neighbor_index = 0; neighbor_index != LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS; neighbor_index++ )
          {
            neighbor_bitmask = 1 << neighbor_index;

            /* check if this neighbor is active */
            if ( ( cdma.nbr_cells.active_bitmask & neighbor_bitmask ) != 0 )
            {
              metric.ucNumNeighbCdma2000++;
            }
          }
          break;
      }
    }
  }

  l_n_list   = modem_mem_alloc( sizeof(iq_lte_neighb_eutra_t)    * metric.ucNumNeighbEutra,
                                MODEM_MEM_CLIENT_MCS_CRIT );
  w_n_list   = modem_mem_alloc( sizeof(iq_lte_neighb_utra_t)     * metric.ucNumNeighbUtra,
                                MODEM_MEM_CLIENT_MCS_CRIT );
  g_n_list   = modem_mem_alloc( sizeof(iq_lte_neighb_geran_t)    * metric.ucNumNeighbGeran,
                                MODEM_MEM_CLIENT_MCS_CRIT );
  c2k_n_list = modem_mem_alloc( sizeof(iq_lte_neighb_cdma2000_t) * metric.ucNumNeighbCdma2000,
                                MODEM_MEM_CLIENT_MCS_CRIT );

  /* reset the neighbor counts, as we use them to store data */
  memset( &metric, 0, sizeof(metric) );

  /* now do it all again, but this time store the data */
  for ( index = 0; index != LTE_CPHY_MEAS_MAX_MEAS_ID; index++ )
  {
    bitmask = 1 << index;

    /* check if the measurment id for this bit is valid */
    if ( ( conn_msg->meas_id_valid_bitmask & bitmask ) != 0 )
    {
      /* grab the object */
      meas_obj_id = conn_msg->meas_cfg_ptr->meas_id_list[index].meas_obj_id - 1;

      /* check which type of object we're dealing with */
      switch ( conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].type )
      {
        case LTE_CPHY_MEAS_OBJ_TYPE_EUTRA:
          /* store the LTE object info */
          eutra = conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].obj_info.eutra;
          /* check every neighbor in this frequency */
          for ( neighbor_index = 0; neighbor_index != LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS; neighbor_index++ )
          {
            neighbor_bitmask = 1 << neighbor_index;

            /* check if this neighbor is active */
            if ( ( eutra.neigh_cells.active_bitmask & neighbor_bitmask ) != 0 )
            {
              l_n_list[metric.ucNumNeighbEutra].wEarfcn     = (uint16)eutra.carrier_freq;
              l_n_list[metric.ucNumNeighbEutra].wPhysCellId = (uint16)eutra.neigh_cells.cell_info[neighbor_index].pci;
              metric.ucNumNeighbEutra++;
            }
          }
          break;

        case LTE_CPHY_MEAS_OBJ_TYPE_WCDMA:
          /* store the WCDMA object info */
          wcdma = conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].obj_info.wcdma;
          /* check every neighbor in this frequency */
          for ( neighbor_index = 0; neighbor_index != LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS; neighbor_index++ )
          {
            neighbor_bitmask = 1 << neighbor_index;

            /* check if this neighbor is active */
            if ( ( wcdma.active_psc_bitmask & neighbor_bitmask ) != 0 )
            {
              w_n_list[metric.ucNumNeighbUtra].wUarfcn = (uint16)wcdma.freq;
              w_n_list[metric.ucNumNeighbUtra].wPsc    = wcdma.psc[neighbor_index];
              metric.ucNumNeighbUtra++;
            }
          }
          break;

        case LTE_CPHY_MEAS_OBJ_TYPE_GERAN:
          /* store the GSM object info */
          gsm = conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].obj_info.gsm;
          /* check every frequency in this object */
          for ( neighbor_index = 0; ( neighbor_index != gsm.num_freqs ) && 
                                    ( neighbor_index != LTE_CPHY_MEAS_MAX_GERAN_FREQS_IN_GROUP ); neighbor_index++ )
          {
            g_n_list[metric.ucNumNeighbGeran].wArfcn = gsm.freqs[neighbor_index];
            metric.ucNumNeighbGeran++;
          }
          break;

        case LTE_CPHY_MEAS_OBJ_TYPE_CDMA:
          /* store the CDMA object info */
          cdma = conn_msg->meas_cfg_ptr->meas_obj_list[meas_obj_id].obj_info.cdma;
          for ( neighbor_index = 0; neighbor_index != LTE_CPHY_MEAS_MAX_MEAS_OBJ_CELLS; neighbor_index++ )
          {
            neighbor_bitmask = 1 << neighbor_index;

            /* check if this neighbor is active */
            if ( ( cdma.nbr_cells.active_bitmask & neighbor_bitmask ) != 0 )
            {
              c2k_n_list[metric.ucNumNeighbCdma2000].wChannelNum    = (uint16)cdma.channel.chan_num;
              c2k_n_list[metric.ucNumNeighbCdma2000].wPilotPnOffset = (uint16)cdma.nbr_cells.phy_cell_id[neighbor_index];
              c2k_n_list[metric.ucNumNeighbCdma2000].ucBandClass    = (uint8)cdma.channel.band;
              c2k_n_list[metric.ucNumNeighbCdma2000].ucPad          = 0xFF;
              metric.ucNumNeighbCdma2000++;
            }
          }
          break;
      }
    }
  }

  index = 0;

  iov[index].data = &metric;
  iov[index].len  = sizeof(metric);

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - num LTE neighbors in metric: %d", 
                      metric.ucNumNeighbEutra );
  if ( NULL != l_n_list )
  {
    index++;
    iov[index].data = l_n_list;
    iov[index].len  = sizeof(iq_lte_neighb_eutra_t) * metric.ucNumNeighbEutra;
  }

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - num WCDMA neighbors in metric: %d", 
                      metric.ucNumNeighbUtra );
  if ( NULL != w_n_list )
  {
    index++;
    iov[index].data = w_n_list;
    iov[index].len  = sizeof(iq_lte_neighb_utra_t) * metric.ucNumNeighbUtra;
  }

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - num GSM neighbors in metric: %d", 
                      metric.ucNumNeighbGeran );
  if ( NULL != g_n_list )
  {
    index++;
    iov[index].data = g_n_list;
    iov[index].len  = sizeof(iq_lte_neighb_geran_t) * metric.ucNumNeighbGeran;
  }

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (conn) - num CDMA neighbors in metric: %d", 
                      metric.ucNumNeighbCdma2000 );
  if ( NULL != c2k_n_list )
  {
    index++;
    iov[index].data = c2k_n_list;
    iov[index].len  = sizeof(iq_lte_neighb_cdma2000_t) * metric.ucNumNeighbCdma2000;
  }

  index++;
  iov[index].data = NULL;
  iov[index].len  = 0;

  MMAL_MSG( "MMAL: LT05 (conn) - submitting metric" );
  IQ_SubmitMetricV( IQ_METRIC_ID_LT05, (iq_iovec_t *)&iov);

  if ( NULL != l_n_list )
  {
    modem_mem_free( l_n_list, MODEM_MEM_CLIENT_MCS_CRIT );
  }

  if ( NULL != w_n_list )
  {
    modem_mem_free( w_n_list, MODEM_MEM_CLIENT_MCS_CRIT );
  }

  if ( NULL != g_n_list )
  {
    modem_mem_free( g_n_list, MODEM_MEM_CLIENT_MCS_CRIT );
  }

  if ( NULL != c2k_n_list )
  {
    modem_mem_free( c2k_n_list, MODEM_MEM_CLIENT_MCS_CRIT );
  }
}


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
  lte_cphy_idle_meas_cfg_req_s           *idle_msg
)
{
  iq_metric_LT05_t                        metric;
  iq_lte_neighb_eutra_t                  *l_n_list;
  iq_lte_neighb_utra_t                   *w_n_list;
  iq_lte_neighb_geran_t                  *g_n_list;
  iq_lte_neighb_cdma2000_t               *c2k_n_list;
  iq_iovec_t                              iov[6];

  lte_earfcn_t                            serving_cell_freq;
  uint32                                  index, freq_index;
  lte_cphy_meas_intra_freq_resel_info_s  *intra;
  lte_cphy_meas_inter_freq_resel_info_s  *inter;
  lte_cphy_meas_irat_wcdma_resel_info_s  *wcdma;
  lte_cphy_meas_irat_gsm_info_s          *gsm;
  lte_cphy_meas_irat_cdma_info_s         *cdma;

/*---------------------------------------------------------------------------*/

  memset( &metric, 0, sizeof(metric) );

  /* check which types of neighbors are present */
  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the intra freq lte ptr */
    intra = idle_msg->meas_cfg.intra_freq_info_ptr;

    MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num LTE intra cells = %d", 
                        intra->whitelist_num_cells );

    /* check the cells */
    for ( index = 0; ( index != intra->whitelist_num_cells ) && 
                     ( index != LTE_CPHY_MEAS_MAX_CELL_INTRA ); index++ )
    {
      metric.ucNumNeighbEutra++;
    }
  }

  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the inter freq lte ptr */
    inter = idle_msg->meas_cfg.inter_freq_info_ptr;

    MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num LTE inter freqs = %d", 
                        inter->num_freq );

    /* check the frequencies */
    for ( freq_index = 0; ( freq_index != inter->num_freq ) &&
                          ( freq_index != LTE_CPHY_MEAS_MAX_NUM_FREQ ); freq_index++ )
    {
      MMAL_MSG_SPRINTF_2( "MMAL: LT05 (idle) - num LTE cells in freq %d = %d", 
                          inter->freq_list[freq_index].dl_carrier_freq,
                          inter->freq_list[freq_index].whitelist_num_cells );

      for ( index = 0; ( index != inter->freq_list[freq_index].whitelist_num_cells ) && 
                       ( index != LTE_CPHY_MEAS_MAX_CELL_INTER ); index++ )
      {
        metric.ucNumNeighbEutra++;
      }
    }
  }

  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the wcdma ptr */
    wcdma = idle_msg->meas_cfg.irat_wcdma_info_ptr;

    MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num WCDMA freqs = %d", 
                        wcdma->num_ngbr_freq );

    /* check the frequencies */
    for ( freq_index = 0; ( freq_index != wcdma->num_ngbr_freq ) &&
                          ( freq_index != LTE_CPHY_MEAS_MAX_WCDMA_FREQ ); freq_index++ )
    {
      metric.ucNumNeighbUtra++;
    }
  }

  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the gsm ptr */
    gsm = idle_msg->meas_cfg.irat_gsm_info_ptr;

    MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num GSM freqs = %d", 
                        gsm->num_freqs );

    /* check the frequencies */
    for ( freq_index = 0; ( freq_index != gsm->num_freqs ) &&
                          ( freq_index != LTE_CPHY_MEAS_MAX_GERAN_FREQ ); freq_index++ )
    {
      metric.ucNumNeighbGeran++;
    }
  }

  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the cdma ptr */
    cdma = idle_msg->meas_cfg.irat_cdma_info_ptr;

    MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num CDMA freqs = %d", 
                        cdma->irat_cdma_1x_resel_param.neigh_cell_info_list_size );

    for ( freq_index = 0; ( freq_index != cdma->irat_cdma_1x_resel_param.neigh_cell_info_list_size ) &&
                          ( freq_index != LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS ); freq_index++ )
    {
      metric.ucNumNeighbCdma2000++;
    }
  }

  l_n_list   = modem_mem_alloc( sizeof(iq_lte_neighb_eutra_t)    * metric.ucNumNeighbEutra,
                                MODEM_MEM_CLIENT_MCS_CRIT );
  MMAL_MSG_SPRINTF_2( "MMAL: LT05 (idle) allocating %d bytes for LTE, return addr is 0x%08x",
                      sizeof(iq_lte_neighb_eutra_t)    * metric.ucNumNeighbEutra,
                      l_n_list );

  w_n_list   = modem_mem_alloc( sizeof(iq_lte_neighb_utra_t)     * metric.ucNumNeighbUtra,
                                MODEM_MEM_CLIENT_MCS_CRIT );
  MMAL_MSG_SPRINTF_2( "MMAL: LT05 (idle) allocating %d bytes for W, return addr is 0x%08x",
                      sizeof(iq_lte_neighb_utra_t)     * metric.ucNumNeighbUtra,
                      w_n_list );

  g_n_list   = modem_mem_alloc( sizeof(iq_lte_neighb_geran_t)    * metric.ucNumNeighbGeran,
                                MODEM_MEM_CLIENT_MCS_CRIT );
  MMAL_MSG_SPRINTF_2( "MMAL: LT05 (idle) allocating %d bytes for G, return addr is 0x%08x",
                      sizeof(iq_lte_neighb_geran_t)    * metric.ucNumNeighbGeran,
                      g_n_list );

  c2k_n_list = modem_mem_alloc( sizeof(iq_lte_neighb_cdma2000_t) * metric.ucNumNeighbCdma2000,
                                MODEM_MEM_CLIENT_MCS_CRIT );
  MMAL_MSG_SPRINTF_2( "MMAL: LT05 (idle) allocating %d bytes for C2K, return addr is 0x%08x",
                      sizeof(iq_lte_neighb_cdma2000_t) * metric.ucNumNeighbCdma2000,
                      c2k_n_list );

  /* reset the neighbor counts, as we use them to store data */
  memset( &metric, 0, sizeof(metric) );

  /* check which types of neighbors are present */
  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_INTRA_FREQ & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* get the earfcn from the serving cell info */
    serving_cell_freq = idle_msg->serving_cell_info.dl_cell_frequency;
    /* store the intra freq lte ptr */
    intra = idle_msg->meas_cfg.intra_freq_info_ptr;

    /* check the cells */
    for ( index = 0; ( index != intra->whitelist_num_cells ) && 
                     ( index != LTE_CPHY_MEAS_MAX_CELL_INTRA ); index++ )
    {
      l_n_list[metric.ucNumNeighbEutra].wEarfcn     = (uint16)serving_cell_freq;
      l_n_list[metric.ucNumNeighbEutra].wPhysCellId = (uint16)intra->whitelist[index].pci;
      metric.ucNumNeighbEutra++;
    }
  }

  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_INTER_FREQ & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the inter freq lte ptr */
    inter = idle_msg->meas_cfg.inter_freq_info_ptr;

    /* check the frequencies */
    for ( freq_index = 0; ( freq_index != inter->num_freq ) &&
                          ( freq_index != LTE_CPHY_MEAS_MAX_NUM_FREQ ); freq_index++ )
    {
      for ( index = 0; ( index != inter->freq_list[freq_index].whitelist_num_cells ) && 
                       ( index != LTE_CPHY_MEAS_MAX_CELL_INTER ); index++ )
      {
        l_n_list[metric.ucNumNeighbEutra].wEarfcn     = (uint16)inter->freq_list[freq_index].dl_carrier_freq;
        l_n_list[metric.ucNumNeighbEutra].wPhysCellId = (uint16)inter->freq_list[freq_index].whitelist[index].pci;
        metric.ucNumNeighbEutra++;
      }
    }
  }

  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_WCDMA & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the wcdma ptr */
    wcdma = idle_msg->meas_cfg.irat_wcdma_info_ptr;

    /* check the frequencies */
    for ( freq_index = 0; ( freq_index != wcdma->num_ngbr_freq ) &&
                          ( freq_index != LTE_CPHY_MEAS_MAX_WCDMA_FREQ ); freq_index++ )
    {
      w_n_list[metric.ucNumNeighbUtra].wUarfcn = (uint16)wcdma->fdd_freq_list[freq_index].freq;
      w_n_list[metric.ucNumNeighbUtra].wPsc    = 0xFFFF;
      metric.ucNumNeighbUtra++;
    }

    /* verify output */
    for ( freq_index = 0; ( freq_index != metric.ucNumNeighbUtra ); freq_index++ )
    {
      MMAL_MSG_SPRINTF_2( "MMAL: LT05 (idle) freq = %d, psc = 0x%x",
                          w_n_list[freq_index].wUarfcn,
                          w_n_list[freq_index].wPsc );
    }
  }

  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_GSM & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the gsm ptr */
    gsm = idle_msg->meas_cfg.irat_gsm_info_ptr;

    /* check the frequencies */
    for ( freq_index = 0; ( freq_index != gsm->num_freqs ) &&
                          ( freq_index != LTE_CPHY_MEAS_MAX_GERAN_FREQ ); freq_index++ )
    {
      g_n_list[metric.ucNumNeighbGeran].wArfcn = (uint16)gsm->freqs[freq_index].freq;
      metric.ucNumNeighbGeran++;
    }
  }

  if ( ( LTE_CPHY_IDLE_MEAS_CFG_MASK_CDMA & idle_msg->meas_cfg.requested_cfg_bitmask ) != 0 )
  {
    /* store the cdma ptr */
    cdma = idle_msg->meas_cfg.irat_cdma_info_ptr;

    for ( freq_index = 0; ( freq_index != cdma->irat_cdma_1x_resel_param.neigh_cell_info_list_size ) &&
                          ( freq_index != LTE_CPHY_MEAS_MAX_CDMA_NEIGH_CELLS ); freq_index++ )
    {
      c2k_n_list[metric.ucNumNeighbCdma2000].wChannelNum    = 
        (uint16)cdma->irat_cdma_1x_resel_param.neigh_cell_info_list[freq_index].channel;
      c2k_n_list[metric.ucNumNeighbCdma2000].wPilotPnOffset = 
        (uint16)cdma->irat_cdma_1x_resel_param.neigh_cell_info_list[freq_index].cell_id;
      c2k_n_list[metric.ucNumNeighbCdma2000].ucBandClass    =  
        (uint8)cdma->irat_cdma_1x_resel_param.neigh_cell_info_list[freq_index].band_class;
      c2k_n_list[metric.ucNumNeighbCdma2000].ucPad          = 0xFF;
      metric.ucNumNeighbCdma2000++;
    }
  }

  index = 0;
  iov[index].data = &metric;
  iov[index].len  = sizeof(metric);

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num LTE neighbors in metric: %d", 
                      metric.ucNumNeighbEutra );
  if ( NULL != l_n_list )
  {
    index++;
    iov[index].data = l_n_list;
    iov[index].len  = sizeof(iq_lte_neighb_eutra_t) * metric.ucNumNeighbEutra;
  }

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num WCDMA neighbors in metric: %d", 
                      metric.ucNumNeighbUtra );
  if ( NULL != w_n_list )
  {
    index++;
    iov[index].data = w_n_list;
    iov[index].len  = sizeof(iq_lte_neighb_utra_t) * metric.ucNumNeighbUtra;
  }

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num GSM neighbors in metric: %d", 
                      metric.ucNumNeighbGeran );
  if ( NULL != g_n_list )
  {
    index++;
    iov[index].data = g_n_list;
    iov[index].len  = sizeof(iq_lte_neighb_geran_t) * metric.ucNumNeighbGeran;
  }

  MMAL_MSG_SPRINTF_1( "MMAL: LT05 (idle) - num CDMA neighbors in metric: %d", 
                      metric.ucNumNeighbCdma2000 );
  if ( NULL != c2k_n_list )
  {
    index++;
    iov[index].data = c2k_n_list;
    iov[index].len  = sizeof(iq_lte_neighb_cdma2000_t) * metric.ucNumNeighbCdma2000;
  }

  index++;
  iov[index].data = NULL;
  iov[index].len  = 0;

  IQ_SubmitMetricV( IQ_METRIC_ID_LT05, (iq_iovec_t *)&iov);

  if ( NULL != l_n_list )
  {
    modem_mem_free( l_n_list, MODEM_MEM_CLIENT_MCS_CRIT );
  }

  if ( NULL != w_n_list )
  {
    modem_mem_free( w_n_list, MODEM_MEM_CLIENT_MCS_CRIT );
  }

  if ( NULL != g_n_list )
  {
    modem_mem_free( g_n_list, MODEM_MEM_CLIENT_MCS_CRIT );
  }

  if ( NULL != c2k_n_list )
  {
    modem_mem_free( c2k_n_list, MODEM_MEM_CLIENT_MCS_CRIT );
  }
}


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_map_lt06_cause

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_map_lt06_cause

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_map_state_change_trigger_lt06_cause
(
 uint8                  cause,
 iq_metric_LT06_t       *lte06_metric
)
{
  lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_NA;

  switch (cause)
  {
    case 0: //LTE_RRC_LOG_STATE_CHANGE_INACTIVE_TRIGGER_OTHER = 0,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_NA;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_NULL;
      break;

    case 1: //LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_CONN_ABORTED = 1,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_FAIL_ABORTED;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_IDLE;
      break;    
    case 2: // LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_T300_EXPIRY = 2,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_FAIL_NO_RESP_FROM_CELL;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_IDLE;
      break;
    case 3: // LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_CONN_REJECT = 3,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_FAIL_REJ;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_IDLE;
      break;
    case 4: // LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_CELL_RESEL = 4,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_FAIL_CELL_RESEL;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_IDLE;
      break;
    case 5: // LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_ACCESS_BARRED = 5,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_FAIL_CELL_BARRED;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_IDLE;
      break;
    case 6: // LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_CONN_EST_FAILURE_OTHER = 6,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_NA;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_IDLE;
      break;
    case 7: // LTE_RRC_LOG_STATE_CHANGE_CAMPED_TRIGGER_OTHER = 7,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_NA;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_IDLE;
      break;
    case 8: // LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_EMERGENCY = 8,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_EMERGENCY ;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ATMPT_CONNECTION;
      break;
    case 9: // LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_HIGH_PRI_ACCESS = 9,
      lte06_metric->ucRrcCause =  IQ_LTE_RRC_CAUSE_EST_HIGH_PRIO_ACC;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ATMPT_CONNECTION;
      break;
    case 10: // LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_MT_ACCESS = 10,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_MT_ACC;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ATMPT_CONNECTION;
      break;
    case 11: // LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_MO_SIGNALING = 11,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_MO_SIGNAL;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ATMPT_CONNECTION;
      break;
    case 12: // LTE_RRC_LOG_STATE_CHANGE_CONNECTING_TRIGGER_MO_DATA = 12,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_EST_MO_DATA;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ATMPT_CONNECTION;
      break;
    case 13: // LTE_RRC_LOG_STATE_CHANGE_CONNECTED_TRIGGER_REEST_RECFG_FAILURE = 13,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REEST_MO_DATA;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_CONNECTED;
      break;
    case 14: // LTE_RRC_LOG_STATE_CHANGE_CONNECTED_TRIGGER_REEST_HO_FAILURE = 14,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REEST_HO_FAIL;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_CONNECTED ;
      break;
    case 15: // LTE_RRC_LOG_STATE_CHANGE_CONNECTED_TRIGGER_REEST_OTHER_FAILURE = 15,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REEST_OTHER_FAIL;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_CONNECTED;
      break;
    case 16: // LTE_RRC_LOG_STATE_CHANGE_CONNECTED_TRIGGER_OTHER = 16,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_NA;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_CONNECTED;
      break;
    case 17: // LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_T311_EXPIRY = 17,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_CONN_FAIL_T311_EXP;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ENDING;
      break;
    case 18: // LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_T301_EXPIRY = 18,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_CONN_FAIL_CELL_NOT_SUIT;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ENDING;
      break;
    case 19: // LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_REEST_REJECT = 19,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_CONN_FAIL_REEST_REJ;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ENDING;
      break;
    case 20: // LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_LOAD_BALANCE = 20,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_LOAD_BAL_TAU_REQD;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ENDING;
      break;
    case 21: // LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_OTHER = 21,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_OTHER;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ENDING;
      break;
    case 22: // LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_CONN_ABORT = 22,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_NA;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ENDING;
      break;
    case 23: // LTE_RRC_LOG_STATE_CHANGE_CLOSING_TRIGGER_CONN_ABORT_IRAT_SUCCESS = 23,
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_SUCC_MOB_FROM_EUTRAN;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_ENDING;
      break;
    default: // LTE_RRC_LOG_STATE_CHANGE_TRIGGER_MAX
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_NA;
      lte06_metric->ucRrcState = IQ_LTE_RRC_STATE_NULL;
  }
}

/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_map_rlf_trigger_lt06_cause

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_map_rlf_trigger_lt06_cause

    @detail
    

    @return
    

    @Note
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_map_rlf_trigger_lt06_cause
(
   uint8                  cause,
   iq_metric_LT06_t       *lte06_metric
)
{
  lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_NA;
  lte06_metric->ucRrcState = lte_metric_info.rrc_state;
  
  switch(cause)
  {
    case 0x0: //CFG Failure 9RLF due to reconfiguration failure
      break;
    case 0x1: //HO_Failure (RLF due to handover failure
      break;
    case 0x2: //RLF due to T310 expiry
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_T310_EXP;
      break;
    case 0x3: //RACH_PROBLEM
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_RND_ACC;
      break;
    case 0x4: // MAX_RETR
      lte06_metric->ucRrcCause = IQ_LTE_RRC_CAUSE_REL_MAX_RLC_RETRANS;
      break;
    case 0x5: // IP_CHECK_FAILURE
      break;
    case 0x6: // SIB_READ_FAILURE
      break;
    case 0x7: // OTHER_FAILURE
      break;
    case 0x8: //MAX
      break;
    default:
      break;
  }
}


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
)
{
  iq_metric_LT06_t              lte06_metric;
  diag_event_type               *evt_ptr;
  uint8                         cause;
  boolean                       log_state = TRUE;
  
  /*! Ensure the param is good to work with */
  MMAL_ASSERT(log_payload_ptr);

  /*The first log pkt needed was already obtained
  hence fill all the details and flush the log packet*/

  evt_ptr = (diag_event_type *)log_payload_ptr->data_ptr;

  //state = b_unpackb(evt_ptr->payload, 8, 8);
  cause = b_unpackb(evt_ptr->payload, 0, 8);
  
  lte06_metric.ucRrcState = IQ_LTE_RRC_STATE_NULL;

  switch (log_code)
  {
    case EVENT_LTE_RRC_STATE_CHANGE_TRIGGER:
      mmal_ciq_lte_metric_map_state_change_trigger_lt06_cause(cause, &lte06_metric);
    
      lte_metric_info.rrc_state = lte06_metric.ucRrcState;
      break; 

    case EVENT_LTE_RRC_RADIO_LINK_FAILURE_STAT:
      mmal_ciq_lte_metric_map_rlf_trigger_lt06_cause(cause, &lte06_metric);
      break;
  }

  if(log_state && 
     (((lte_metric_info.last_submitted_lt06.ucRrcState 
       != lte06_metric.ucRrcState) ||
      (lte_metric_info.last_submitted_lt06.ucRrcCause 
       != lte06_metric.ucRrcCause)) || 
     (mmal_ciq_lt06_discard_duplicate == FALSE)))
  {
    lte_metric_info.last_submitted_lt06.ucRrcState = lte06_metric.ucRrcState;
    lte_metric_info.last_submitted_lt06.ucRrcCause = lte06_metric.ucRrcCause;    
    
    mmal_ciq_metrics_query_submit_metric(
      IQ_METRIC_ID_LT06,
      &lte06_metric,
      sizeof(lte06_metric));

    mmal_ciq_metric_stats_info.metrics_info.num_lte06_metric_submitted++;
  }
  else
  {
    mmal_ciq_metric_stats_info.metrics_info.num_duplicate_lt06_events++;
    MSG_HIGH(
      "CIQ: skip DUP LT06 LogCode 0x%x,RRCstate %d StateCause %d", 
             log_code, lte06_metric.ucRrcState, lte06_metric.ucRrcCause);

  }
}


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
  timer_cb_data_type               data
)
{
  lte_mac_stats_update_req_msg_s   mac_stats_req_ptr;
  errno_enum_type                  ret_val;

  NOTUSED(data);

  // send a request to LTE MAC
  msgr_init_hdr( &mac_stats_req_ptr.hdr, MSGR_MCS_MMAL, LTE_MAC_STATS_UPDATE_REQ );
  mac_stats_req_ptr.payload_ptr = &lt07_mac_stats;

  ret_val = msgr_send( &mac_stats_req_ptr.hdr, sizeof( mac_stats_req_ptr ) );
  MMAL_ASSERT( ret_val == E_SUCCESS );

} /* mmal_ciq_lte_metric_request_data_lt07_cb */


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
)
{
  iq_metric_LT07_t       lt07_metric;

/*---------------------------------------------------------------------------*/

  memset( &lt07_metric, 0, sizeof(lt07_metric) );

  lt07_metric.ucBufSize0     = mac_stats->lcg_buf_size[LTE_MAC_BSR_LCGROUP_0];
  lt07_metric.ucBufSize1     = mac_stats->lcg_buf_size[LTE_MAC_BSR_LCGROUP_1];
  lt07_metric.ucBufSize2     = mac_stats->lcg_buf_size[LTE_MAC_BSR_LCGROUP_2];
  lt07_metric.ucBufSize3     = mac_stats->lcg_buf_size[LTE_MAC_BSR_LCGROUP_3];
  lt07_metric.wCrnti         = mac_stats->c_rnti;
  lt07_metric.ucTimingAdv    = mac_stats->timing_adv;
  lt07_metric.ucPowerHroom   = mac_stats->power_headroom;

  IQ_SubmitMetric( IQ_METRIC_ID_LT07, &lt07_metric, sizeof( lt07_metric ) );
}


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
  lte_cphy_raa_ind_msg_s            *msg
)
{
  iq_metric_LT08_t                   lt08_metric;
  lte_cphy_random_access_attempt_s  *payload = &msg->msg;

/*---------------------------------------------------------------------------*/

  memset( &lt08_metric, 0, sizeof(lt08_metric) );

  lt08_metric.wEarfcn         = payload->earfcn;
  lt08_metric.wPhysCellId     = payload->phys_cell_id; 
  lt08_metric.wRaRnti         = payload->ra_rnti;
  lt08_metric.ucPreambleCount = payload->preamble_count; 
  lt08_metric.ucLastTxPower   = payload->last_tx_power;
  lt08_metric.dwUlGrant       = payload->ul_grant;
  lt08_metric.wRaTempCrnti    = payload->ra_temp_c_rnti;
  lt08_metric.wTimingAdv      = payload->timing_advance;
  lt08_metric.ucRaRespSucc    = payload->ra_resp_successful;
  if ( !lt08_metric.ucRaRespSucc )
  {
    lt08_metric.dwUlGrant       = 0xFFFFFFFF;
    lt08_metric.wRaTempCrnti    = 0xFFFF;
    lt08_metric.wTimingAdv      = 0xFFFF;
  }

  IQ_SubmitMetric( IQ_METRIC_ID_LT08, &lt08_metric, sizeof( lt08_metric ) );
}


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
  timer_cb_data_type      data
)
{
  lte_rlcdl_stats_req_s   dl_stats_req_ptr;
  errno_enum_type         ret_val;

  NOTUSED(data);

  memset( &lt09_dl_stats, 0, sizeof(lt09_dl_stats) );

  // send the stats request to LTE RLC
  msgr_init_hdr( &dl_stats_req_ptr.msg_hdr, MSGR_MCS_MMAL, LTE_RLCDL_STATS_REQ );
  dl_stats_req_ptr.stats_ptr = &lt09_dl_stats;

  ret_val = msgr_send( &dl_stats_req_ptr.msg_hdr, sizeof( dl_stats_req_ptr ) );
  MMAL_ASSERT( ret_val == E_SUCCESS );
} /* mmal_ciq_lte_metric_request_data_lt09_cb */


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
  lte_rlcdl_stats_s      *dl_stats
)
{
  lte_rlcul_stats_req_s   ul_stats_req_ptr;
  errno_enum_type         ret_val;
  uint8                   rb;
  uint32                  re_rx_pdu_count;

/*-------------------------------------------------------------------------*/

  /* If there are no radio bearers, there's nothing to report, so exit */
  if ( 0 == dl_stats->num_active_rb )
  {
    return;
  }

  /* Clear the old metric */
  memset( &lt09_metric, 0, sizeof(lt09_metric) );
  lt09_metric.wPad = 0xFFFF;

  /* Clear the old metric's map*/
  memset( &lt09_rb_map, 0, sizeof(lt09_rb_map) );

  /* Set the DL data */
  for ( rb = 0; rb != dl_stats->num_active_rb; rb++ )
  {
    if ( dl_stats->rb_stats[rb].rb_type == LTE_RB_TYPE_DRB )
    {
      lt09_drb_stats[lt09_metric.ucNumDrb].ucDrbId              = dl_stats->rb_stats[rb].rb_id;
      lt09_drb_stats[lt09_metric.ucNumDrb].ucEpsBearerId        = dl_stats->rb_stats[rb].eps_id;
      lt09_drb_stats[lt09_metric.ucNumDrb].ucLogicalChanId      = dl_stats->rb_stats[rb].lc_id;
      switch ( dl_stats->rb_stats[rb].rb_mode )
      {
        case LTE_RB_RLC_MODE_AM:
          lt09_drb_stats[lt09_metric.ucNumDrb].ucRlcMode        = IQ_LTE_RLC_MODE_AM; 
          break;
        case LTE_RB_RLC_MODE_UM:
          lt09_drb_stats[lt09_metric.ucNumDrb].ucRlcMode        = IQ_LTE_RLC_MODE_UM; 
          break;
        case LTE_RB_RLC_MODE_TM:
          lt09_drb_stats[lt09_metric.ucNumDrb].ucRlcMode        = IQ_LTE_RLC_MODE_TM; 
          break;
        default:
          MMAL_ASSERT( "Invalid Bearer Mode" == NULL );
      }
      /* if the new sdu count is greater, take the difference, otherwise just take it */
      if ( lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxSduCount <= dl_stats->rb_stats[rb].num_sdu )
      {
        lt09_drb_stats[lt09_metric.ucNumDrb].dwRxSduCount       = dl_stats->rb_stats[rb].num_sdu - 
                                                                  lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxSduCount;
      }
      else
      {
        lt09_drb_stats[lt09_metric.ucNumDrb].dwRxSduCount       = dl_stats->rb_stats[rb].num_sdu;
      }
      lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxSduCount    = dl_stats->rb_stats[rb].num_sdu;

      /* if the new sdu byte count is greater, take the difference, otherwise just take it */
      if ( lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxByteCount <= dl_stats->rb_stats[rb].sdu_bytes )
      {
        lt09_drb_stats[lt09_metric.ucNumDrb].dwRxByteCount      = dl_stats->rb_stats[rb].sdu_bytes - 
                                                                  lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxByteCount;
      }
      else
      {
        lt09_drb_stats[lt09_metric.ucNumDrb].dwRxByteCount      = dl_stats->rb_stats[rb].sdu_bytes;
      }
      lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxByteCount   = dl_stats->rb_stats[rb].sdu_bytes;

      /* if the new pdu byte count is greater, take the difference, otherwise just take it */
      if ( lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxPduCount <= dl_stats->rb_stats[rb].num_data_pdu )
      {
        lt09_drb_stats[lt09_metric.ucNumDrb].dwRxPduCount       = dl_stats->rb_stats[rb].num_data_pdu - 
                                                                  lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxPduCount;
      }
      else
      {
        lt09_drb_stats[lt09_metric.ucNumDrb].dwRxPduCount       = dl_stats->rb_stats[rb].num_data_pdu;
      }
      lt09_drb_last_stats[lt09_metric.ucNumDrb].dwRxPduCount    = dl_stats->rb_stats[rb].num_data_pdu;

      /* if the new retx pdu count is greater, take the difference, otherwise just take it */
      re_rx_pdu_count = dl_stats->rb_stats[rb].num_comp_nack + dl_stats->rb_stats[rb].num_segm_nack;
      if ( lt09_drb_last_stats[lt09_metric.ucNumDrb].dwReRxPduCount <= re_rx_pdu_count )
      {
        lt09_drb_stats[lt09_metric.ucNumDrb].dwReRxPduCount     = re_rx_pdu_count - 
                                                                  lt09_drb_last_stats[lt09_metric.ucNumDrb].dwReRxPduCount;
      }
      else
      {
        lt09_drb_stats[lt09_metric.ucNumDrb].dwReRxPduCount     = re_rx_pdu_count;
      }
      lt09_drb_last_stats[lt09_metric.ucNumDrb].dwReRxPduCount  = re_rx_pdu_count;

      /* Set the mapping for the UL data */
      lt09_rb_map[rb].unique_rb_id                              = dl_stats->rb_stats[rb].rb_cfg_idx;
      lt09_rb_map[rb].rb_type                                   = LTE_RB_TYPE_DRB;
      lt09_rb_map[rb].rb_stat_ptr                               = (void *)&lt09_drb_stats[lt09_metric.ucNumDrb];
      lt09_rb_map[rb].rb_last_stat_ptr                          = (void *)&lt09_drb_last_stats[lt09_metric.ucNumDrb];

      lt09_metric.ucNumDrb++;
    }
    else if ( dl_stats->rb_stats[rb].rb_type == LTE_RB_TYPE_SRB )
    {
      lt09_srb_stats[lt09_metric.ucNumSrb].ucSrbId              = dl_stats->rb_stats[rb].rb_id;
      lt09_srb_stats[lt09_metric.ucNumSrb].ucPad                = 0xFF;

      /* if the new sdu count is greater, take the difference, otherwise just take it */
      if ( lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxSduCount <= dl_stats->rb_stats[rb].num_sdu )
      {
        lt09_srb_stats[lt09_metric.ucNumSrb].wRxSduCount        = dl_stats->rb_stats[rb].num_sdu - 
                                                                  lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxSduCount;
      }
      else
      {
        lt09_srb_stats[lt09_metric.ucNumSrb].wRxSduCount        = dl_stats->rb_stats[rb].num_sdu;
      }
      lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxSduCount     = dl_stats->rb_stats[rb].num_sdu;

      /* if the new sdu byte count is greater, take the difference, otherwise just take it */
      if ( lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxByteCount <= dl_stats->rb_stats[rb].sdu_bytes )
      {
        lt09_srb_stats[lt09_metric.ucNumSrb].wRxByteCount       = dl_stats->rb_stats[rb].sdu_bytes - 
                                                                  lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxByteCount;
      }
      else
      {
        lt09_srb_stats[lt09_metric.ucNumSrb].wRxByteCount       = dl_stats->rb_stats[rb].sdu_bytes;
      }
      lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxByteCount    = dl_stats->rb_stats[rb].sdu_bytes;

      /* if the new pdu byte count is greater, take the difference, otherwise just take it */
      if ( lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxPduCount <= dl_stats->rb_stats[rb].num_data_pdu )
      {
        lt09_srb_stats[lt09_metric.ucNumSrb].wRxPduCount        = dl_stats->rb_stats[rb].num_data_pdu - 
                                                                  lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxPduCount;
      }
      else
      {
        lt09_srb_stats[lt09_metric.ucNumSrb].wRxPduCount        = dl_stats->rb_stats[rb].num_data_pdu;
      }
      lt09_srb_last_stats[lt09_metric.ucNumSrb].wRxPduCount     = dl_stats->rb_stats[rb].num_data_pdu;

      /* if the new retx pdu count is greater, take the difference, otherwise just take it */
      re_rx_pdu_count = dl_stats->rb_stats[rb].num_comp_nack + dl_stats->rb_stats[rb].num_segm_nack;
      if ( lt09_srb_last_stats[lt09_metric.ucNumSrb].wReRxPduCount <= re_rx_pdu_count )
      {
        lt09_srb_stats[lt09_metric.ucNumSrb].wReRxPduCount      = re_rx_pdu_count - 
                                                                  lt09_srb_last_stats[lt09_metric.ucNumSrb].wReRxPduCount;
      }
      else
      {
        lt09_srb_stats[lt09_metric.ucNumSrb].wReRxPduCount      = re_rx_pdu_count;
      }
      lt09_srb_last_stats[lt09_metric.ucNumSrb].wReRxPduCount   = re_rx_pdu_count;

      /* Set the mapping for the UL data */
      lt09_rb_map[rb].unique_rb_id                              = dl_stats->rb_stats[rb].rb_cfg_idx;
      lt09_rb_map[rb].rb_type                                   = LTE_RB_TYPE_SRB;
      lt09_rb_map[rb].rb_stat_ptr                               = (void *)&lt09_srb_stats[lt09_metric.ucNumSrb];
      lt09_rb_map[rb].rb_last_stat_ptr                          = (void *)&lt09_srb_last_stats[lt09_metric.ucNumSrb];

      lt09_metric.ucNumSrb++;
    }
    else 
    { 
      MMAL_ASSERT( "Invalid RB Type" == NULL );
    }
  }

  memset( &lt09_ul_stats, 0, sizeof(lt09_ul_stats) );

  /* DL stored; send off the UL stats request */
  msgr_init_hdr( &ul_stats_req_ptr.msg_hdr, MSGR_MCS_MMAL, LTE_RLCUL_STATS_REQ );
  ul_stats_req_ptr.stats_ptr = &lt09_ul_stats;

  ret_val = msgr_send( &ul_stats_req_ptr.msg_hdr, sizeof( ul_stats_req_ptr ) );
  MMAL_ASSERT( ret_val == E_SUCCESS );
}


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
  lte_rlcul_stats_s   *ul_stats
)
{
  uint8                rb, map_index;
  iq_lte_srb_stats_t  *srb_stats;
  iq_lte_srb_stats_t  *srb_last_stats;
  iq_lte_drb_stats_t  *drb_stats;
  iq_lte_drb_stats_t  *drb_last_stats;
  iq_iovec_t           iov[4];

/*-------------------------------------------------------------------------*/

  /* Finish filling in the TX data */
  for ( rb = 0; rb != ul_stats->num_active_rb; rb++ )
  {
    /* Find the relevant stats packet */
    for ( map_index = 0; 
          map_index != MMAL_CIQ_LTE_MAX_SRB_NUM + MMAL_CIQ_LTE_MAX_DRB_NUM; 
          map_index++ )
    {
      if ( lt09_rb_map[map_index].unique_rb_id == ul_stats->rb_stats[rb].rb_cfg_idx )
      {
        /* Check type of RB */
        if ( lt09_rb_map[map_index].rb_type == LTE_RB_TYPE_SRB )
        {
          srb_stats                      = (iq_lte_srb_stats_t *)lt09_rb_map[map_index].rb_stat_ptr;
          srb_last_stats                 = (iq_lte_srb_stats_t *)lt09_rb_map[map_index].rb_last_stat_ptr;

          /* if the new sdu count is greater, take the difference, otherwise just take it */
          if ( srb_last_stats->wTxSduCount <= ul_stats->rb_stats[rb].num_sdus_txed )
          {
            srb_stats->wTxSduCount       = ul_stats->rb_stats[rb].num_sdus_txed - srb_last_stats->wTxSduCount;
          }
          else
          {
            srb_stats->wTxSduCount       = ul_stats->rb_stats[rb].num_sdus_txed;
          }
          srb_last_stats->wTxSduCount    = ul_stats->rb_stats[rb].num_sdus_txed;

          /* if the new sdu byte count is greater, take the difference, otherwise just take it */
          if ( srb_last_stats->wTxByteCount <= ul_stats->rb_stats[rb].sdu_bytes_txed )
          {
            srb_stats->wTxByteCount      = ul_stats->rb_stats[rb].sdu_bytes_txed - srb_last_stats->wTxByteCount;
          }
          else
          {
            srb_stats->wTxByteCount      = ul_stats->rb_stats[rb].sdu_bytes_txed;
          }
          srb_last_stats->wTxByteCount   = ul_stats->rb_stats[rb].sdu_bytes_txed;

          /* if the new pdu count is greater, take the difference, otherwise just take it */
          if ( srb_last_stats->wTxPduCount <= ul_stats->rb_stats[rb].num_new_data_pdus_txed )
          {
            srb_stats->wTxPduCount       = ul_stats->rb_stats[rb].num_new_data_pdus_txed - srb_last_stats->wTxPduCount;
          }
          else
          {
            srb_stats->wTxPduCount       = ul_stats->rb_stats[rb].num_new_data_pdus_txed;
          }
          srb_last_stats->wTxPduCount    = ul_stats->rb_stats[rb].num_new_data_pdus_txed;

          /* if the new retx pdu byte count is greater, take the difference, otherwise just take it */
          if ( srb_last_stats->wReTxPduCount <= ul_stats->rb_stats[rb].num_pdus_re_txed )
          {
            srb_stats->wReTxPduCount     = ul_stats->rb_stats[rb].num_pdus_re_txed - srb_last_stats->wReTxPduCount;
          }
          else
          {
            srb_stats->wReTxPduCount     = ul_stats->rb_stats[rb].num_pdus_re_txed;
          }
          srb_last_stats->wReTxPduCount  = ul_stats->rb_stats[rb].num_pdus_re_txed;
        }
        else if ( lt09_rb_map[map_index].rb_type == LTE_RB_TYPE_DRB )
        {
          drb_stats                      = (iq_lte_drb_stats_t *)lt09_rb_map[map_index].rb_stat_ptr;
          drb_last_stats                 = (iq_lte_drb_stats_t *)lt09_rb_map[map_index].rb_last_stat_ptr;

          /* if the new sdu count is greater, take the difference, otherwise just take it */
          if ( drb_last_stats->dwTxSduCount <= ul_stats->rb_stats[rb].num_sdus_txed )
          {
            drb_stats->dwTxSduCount      = ul_stats->rb_stats[rb].num_sdus_txed - drb_last_stats->dwTxSduCount;
          }
          else
          {
            drb_stats->dwTxSduCount      = ul_stats->rb_stats[rb].num_sdus_txed;
          }
          drb_last_stats->dwTxSduCount   = ul_stats->rb_stats[rb].num_sdus_txed;

          /* if the new sdu byte count is greater, take the difference, otherwise just take it */
          if ( drb_last_stats->dwTxByteCount <= ul_stats->rb_stats[rb].sdu_bytes_txed )
          {
            drb_stats->dwTxByteCount     = ul_stats->rb_stats[rb].sdu_bytes_txed - drb_last_stats->dwTxByteCount;
          }
          else
          {
            drb_stats->dwTxByteCount     = ul_stats->rb_stats[rb].sdu_bytes_txed;
          }
          drb_last_stats->dwTxByteCount  = ul_stats->rb_stats[rb].sdu_bytes_txed;

          /* if the new pdu count is greater, take the difference, otherwise just take it */
          if ( drb_last_stats->dwTxPduCount <= ul_stats->rb_stats[rb].num_new_data_pdus_txed )
          {
            drb_stats->dwTxPduCount      = ul_stats->rb_stats[rb].num_new_data_pdus_txed - drb_last_stats->dwTxPduCount;
          }
          else
          {
            drb_stats->dwTxPduCount      = ul_stats->rb_stats[rb].num_new_data_pdus_txed;
          }
          drb_last_stats->dwTxPduCount   = ul_stats->rb_stats[rb].num_new_data_pdus_txed;

          /* if the new retx pdu byte count is greater, take the difference, otherwise just take it */
          if ( drb_last_stats->dwReTxPduCount <= ul_stats->rb_stats[rb].num_pdus_re_txed )
          {
            drb_stats->dwReTxPduCount    = ul_stats->rb_stats[rb].num_pdus_re_txed - drb_last_stats->dwReTxPduCount;
          }
          else
          {
            drb_stats->dwReTxPduCount    = ul_stats->rb_stats[rb].num_pdus_re_txed;
          }
          drb_last_stats->dwReTxPduCount = ul_stats->rb_stats[rb].num_pdus_re_txed;
        }
        else
        {
          MMAL_ASSERT( "Invalid RB Type" == NULL );
        }
        break;
      }
    }
  }

  /* Build the metric vector */
  iov[0].data = &lt09_metric;
  iov[0].len  = sizeof(lt09_metric);

  iov[1].data = &lt09_srb_stats;
  iov[1].len  = sizeof(iq_lte_srb_stats_t) * lt09_metric.ucNumSrb;

  iov[2].data = &lt09_drb_stats;
  iov[2].len  = sizeof(iq_lte_drb_stats_t) * lt09_metric.ucNumDrb;

  iov[3].data = NULL;
  iov[3].len  = 0;

  /* Submit the metric vector */
  IQ_SubmitMetricV(IQ_METRIC_ID_LT09, (iq_iovec_t *)&iov);
}


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_lt10

===========================================================================*/
/*!
    @brief
    mmal_ciq_lte_metric_pack_and_submit_lt10

    @detail
    

    @return
    

    @Note
     Assumption is made that LOG_LTE_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C event
     occurs before LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C
    

*/
/*=========================================================================*/
void mmal_ciq_lte_metric_pack_and_submit_lt10
(
  /*! The log packet that has the necessary information to 
  pack this metric */
  dsm_item_type            *log_payload_ptr,
  const unsigned int        log_code
)
{
  static iq_metric_LT10_t   lt10_metric;
  static boolean            log_waiting = FALSE;
  iq_result_t               res;
  boolean                   log_state = FALSE;
  void                     *log_ptr;

  lte_nas_esm_bearer_context_state_T  *esm_bearer_ctxt_log_state;
  lte_nas_esm_bearer_context_info_T   *esm_bearer_ctxt_log_info;

  /* Ensure the param is good to work with */
  MMAL_ASSERT(log_payload_ptr);

  MMAL_MSG("MMAL: LT10 Pack Function");

  switch (log_code)
  {
    case LOG_LTE_NAS_ESM_BEARER_CONTEXT_STATE_LOG_C:
      /*! Extract the Bearer Context State log packet*/
      log_ptr = (void *)((size_t)(log_payload_ptr->data_ptr) + sizeof(log_hdr_type));
      esm_bearer_ctxt_log_state = (lte_nas_esm_bearer_context_state_T *)(log_ptr);
      
      lt10_metric.ucEpsBearerId       = esm_bearer_ctxt_log_state->bearer_id;
      switch ( esm_bearer_ctxt_log_state->bearer_state )
      {
        case 0:
          lt10_metric.ucEpsBearerNewState = IQ_EPS_BEARER_INACTIVE;
          break;
        case 1:
          lt10_metric.ucEpsBearerNewState = IQ_EPS_BEARER_ACTIVE_PENDING;
          break;
        case 2:
          lt10_metric.ucEpsBearerNewState = IQ_EPS_BEARER_ACTIVE;
          break;
        case 3:
          lt10_metric.ucEpsBearerNewState = IQ_EPS_BEARER_MODIFY;
          break;
        default:
          MMAL_ASSERT(0);
          break;
      }

      /* Check if the connection id isn't 0xFF and store it if so */
      if ( 0xFF != esm_bearer_ctxt_log_state->connection_id )
      {
        lt10_pdn_conn_map[lt10_metric.ucEpsBearerId] = esm_bearer_ctxt_log_state->connection_id;
      }
      /* Take the value from the map, as on deactivation it will be 0xFF */
      lt10_metric.ucPdnConnectionId = lt10_pdn_conn_map[lt10_metric.ucEpsBearerId];

      /* Check if the bearer type is already known - log this packet if so */
      if ( 0xFF != lt10_bearer_type_map[lt10_metric.ucEpsBearerId] )
      {
        MMAL_MSG("MMAL: LT10 State set, bearer type known");
        lt10_metric.ucEpsBearerType = lt10_bearer_type_map[lt10_metric.ucEpsBearerId];
        log_state = TRUE;
        /* If the bearer is now inactive, clear the map values */
        if ( IQ_EPS_BEARER_INACTIVE == lt10_metric.ucEpsBearerNewState )
        {
          lt10_bearer_type_map[lt10_metric.ucEpsBearerId] = 0xFF;
          lt10_pdn_conn_map[lt10_metric.ucEpsBearerId] = 0xFF;
        }
      }
      else
      {
        log_waiting = TRUE;
        MMAL_MSG("MMAL: LT10 State set, waiting for bearer type");
      }

      break; 

    case LOG_LTE_NAS_ESM_BEARER_CONTEXT_INFO_LOG_C:
      /* Check if we are waiting for bearer type */
      if ( log_waiting )
      {
        /*! Extract the Bearer Context Info log packet */
        log_ptr = (void *)((size_t)(log_payload_ptr->data_ptr) + sizeof(log_hdr_type));
        esm_bearer_ctxt_log_info = (lte_nas_esm_bearer_context_info_T *)(log_ptr);
        
        /* Store the bearer type in the map */
        lt10_bearer_type_map[lt10_metric.ucEpsBearerId] = esm_bearer_ctxt_log_info->context_type;

        lt10_metric.ucEpsBearerType                     = esm_bearer_ctxt_log_info->context_type;
        MMAL_MSG("MMAL: LT10 Packed for shipment");

        log_state = TRUE;
        log_waiting = FALSE;
      }
      
      break;
  }
  
  if(log_state)
  {
    res = IQ_SubmitMetric( IQ_METRIC_ID_LT10, &lt10_metric, sizeof(lt10_metric) );
    
    if (res != IQ_SUCCESS)
      /*! @TODO -Add stats,  have a f3 to print the Result */
    {
      MMAL_CIQ_INCR_ERR_CNT();
      MMAL_CIQ_INCR_CIQ_ERR_CNT();
    }
    MMAL_MSG("MMAL: LT10 Submitted");
  }
}


/*===========================================================================

  FUNCTION:  mmal_ciq_lte_metric_pack_and_submit_lt11

===========================================================================*/
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
  dsm_item_type                      *log_payload_ptr
)
{
  static iq_metric_LT11_t             lt11_metric;
  iq_result_t                         res;
  lte_nas_esm_bearer_context_info_T  *esm_bearer_ctxt_log_info; 
  void                               *log_ptr;

  /* Ensure the param is good to work with */
  MMAL_ASSERT(log_payload_ptr);

  MMAL_MSG("MMAL: LT11 Submit Function");

  //Initialize this pointer 
  memset( &lt11_metric, 0, sizeof(lt11_metric) );

  /*! Extract the Bearer Context Info log packet*/
  log_ptr = (void *)((size_t)(log_payload_ptr->data_ptr) + sizeof(log_hdr_type));
  esm_bearer_ctxt_log_info = (lte_nas_esm_bearer_context_info_T *)(log_ptr);

  lt11_metric.ucEpsBearerId             =   esm_bearer_ctxt_log_info->bearer_id;
  lt11_metric.ucPdnConnectionId         =   esm_bearer_ctxt_log_info->connection_id;
  lt11_metric.ucLinkedEpsBearerId       =   esm_bearer_ctxt_log_info->lbi;

  lt11_metric.tEpsQos.ucQCI             =   esm_bearer_ctxt_log_info->eps_qos[1];
  lt11_metric.tEpsQos.ucMaxBitRateUl    =   esm_bearer_ctxt_log_info->eps_qos[2];
  lt11_metric.tEpsQos.ucMaxBitRateDl    =   esm_bearer_ctxt_log_info->eps_qos[3];
  lt11_metric.tEpsQos.ucGBRUl           =   esm_bearer_ctxt_log_info->eps_qos[4];
  lt11_metric.tEpsQos.ucGBRDl           =   esm_bearer_ctxt_log_info->eps_qos[5];
  lt11_metric.tEpsQos.ucMaxBitRateUlEx  =   esm_bearer_ctxt_log_info->eps_qos[6];
  lt11_metric.tEpsQos.ucMaxBitRateDlEx  =   esm_bearer_ctxt_log_info->eps_qos[7];
  lt11_metric.tEpsQos.ucGBRUlEx         =   esm_bearer_ctxt_log_info->eps_qos[8];
  lt11_metric.tEpsQos.ucGBRDlEx         =   esm_bearer_ctxt_log_info->eps_qos[9];

  res = IQ_SubmitMetric( IQ_METRIC_ID_LT11, &lt11_metric, sizeof(lt11_metric) );

  if (res != IQ_SUCCESS)
  /*! @TODO -Add stats,  have a f3 to print the Result */
  {
    MMAL_CIQ_INCR_ERR_CNT();
    MMAL_CIQ_INCR_CIQ_ERR_CNT();
  }
}

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
)
{
  iq_metric_LT12_t     lt12_metric;
  iq_cqi_band_dist_t   lt12_cqi_dist;
  iq_cqi_band_dist_t   lt12_cqi_band_dist[4];
  uint32               index = 0;
  uint32               dist_num;
  iq_iovec_t           iov[4];
  int8                 capped_tx_power;

/*---------------------------------------------------------------------------*/

  memset( &lt12_metric, 0, sizeof(lt12_metric) );

  capped_tx_power = (int8)msg->msg_payload.mmal_report.total_pusch_tx_power;
  if ( capped_tx_power > MMAL_CIQ_LTE_MAX_TX_POWER )
  {
    capped_tx_power = MMAL_CIQ_LTE_MAX_TX_POWER;
  }
  else if ( capped_tx_power < MMAL_CIQ_LTE_MIN_TX_POWER )
  {
    capped_tx_power = MMAL_CIQ_LTE_MIN_TX_POWER;
  }

  lt12_metric.scTotalPUSCHTxPower = capped_tx_power;
  lt12_metric.scPUSCHTxPowerPerRB = capped_tx_power - mmal_ciq_lt12_numBR_lgtb[(uint8)msg->msg_payload.mmal_report.num_RB];

  MMAL_MSG_SPRINTF_3( "mmal: lt12 - total power %d over %d resource blocks = average power %d",
                      lt12_metric.scTotalPUSCHTxPower,
                      msg->msg_payload.mmal_report.num_RB,
                      lt12_metric.scPUSCHTxPowerPerRB );
  
  if ( msg->msg_payload.mmal_report.wb_report_present )
  {
    lt12_metric.ucWbReportPresent = (uint8)IQ_CQI_PRESENT;
    for ( dist_num = 0; dist_num != 16; dist_num++ )
    {
      lt12_cqi_dist.tCqiDist[dist_num].wAck = msg->msg_payload.mmal_report.cqi_wb_dist.cqi_dist[dist_num].ack;
      lt12_cqi_dist.tCqiDist[dist_num].wNack = msg->msg_payload.mmal_report.cqi_wb_dist.cqi_dist[dist_num].nack;
      lt12_cqi_dist.tCqiDist[dist_num].wDtx = msg->msg_payload.mmal_report.cqi_wb_dist.cqi_dist[dist_num].dtx;
    }
  }
  else 
  {  
    lt12_metric.ucWbReportPresent = (uint8)IQ_CQI_NOT_PRESENT;
  }

  lt12_metric.ucNumSubBandReport = msg->msg_payload.mmal_report.num_sb_report;
  MMAL_MSG_SPRINTF_1( "MMAL: LT12, Num Sub-band Reports = %d", msg->msg_payload.mmal_report.num_sb_report );
  if ( lt12_metric.ucNumSubBandReport > 4 )
  {
    MMAL_MSG_ERROR( "MMAL: ERROR in LT12, Num Sub-band Reports > 4, capping at 4" );
    lt12_metric.ucNumSubBandReport = 4;
  }

  for ( index = 0; index != lt12_metric.ucNumSubBandReport; index++ )
  {
    for ( dist_num = 0; dist_num != 16; dist_num++ )
    {
      lt12_cqi_band_dist[index].tCqiDist[dist_num].wAck = msg->msg_payload.mmal_report.cqi_sb_dist[index].cqi_dist[dist_num].ack;
      lt12_cqi_band_dist[index].tCqiDist[dist_num].wNack = msg->msg_payload.mmal_report.cqi_sb_dist[index].cqi_dist[dist_num].nack;
      lt12_cqi_band_dist[index].tCqiDist[dist_num].wDtx = msg->msg_payload.mmal_report.cqi_sb_dist[index].cqi_dist[dist_num].dtx;
    }
  }
    
  for ( index = 0; index != 5; index++ )
  {
    lt12_metric.wRankIndDist[index] = msg->msg_payload.mmal_report.rank_ind_dist[index];
  }
  for ( index = 0; index != 16; index++ )
  {
    lt12_metric.wPMIDist[index] = msg->msg_payload.mmal_report.pmi_dist[index];
  }
 
  /* Build the metric vector */
  index = 0;
  iov[index].data = &lt12_metric;
  iov[index].len  = sizeof(lt12_metric);
  index++;

  if ( msg->msg_payload.mmal_report.wb_report_present )
  {
    iov[index].data = &lt12_cqi_dist;
    iov[index].len  = sizeof(iq_cqi_band_dist_t);
    index++;
  }

  iov[index].data = &lt12_cqi_band_dist;
  iov[index].len  = sizeof(iq_cqi_band_dist_t) * msg->msg_payload.mmal_report.num_sb_report;
  index++;

  iov[index].data = NULL;
  iov[index].len  = 0;

  /* Submit the metric vector */
  IQ_SubmitMetricV(IQ_METRIC_ID_LT12, (iq_iovec_t *)&iov);
  MMAL_MSG( "MMAL: Submitting LT12" );
}

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
)
{
  iq_metric_LT13_t             lt13_metric;

/*---------------------------------------------------------------------------*/

  memset( &lt13_metric, 0, sizeof(lt13_metric) );
    
  switch (msg->msg)
  {
    case LTE_ML1_T310_STATUS_STOPPED:
      MMAL_MSG( "MMAL: LT13 - sending STOPPED" );
      lt13_metric.ucT310Status = IQ_T310_STATUS_STOPPED;
      break;
    case LTE_ML1_T310_STATUS_STARTED:
      MMAL_MSG( "MMAL: LT13 - sending STARTED" );
      lt13_metric.ucT310Status = IQ_T310_STATUS_STARTED;
      break;
    case LTE_ML1_T310_STATUS_EXPIRED:
      MMAL_MSG( "MMAL: LT13 - sending EXPIRED" );
      lt13_metric.ucT310Status = IQ_T310_STATUS_EXPIRED;
      break;
    case LTE_ML1_T310_STATUS_UNKNOWN:
      MMAL_MSG( "MMAL: LT13 - sending UNKNOWN" );
      lt13_metric.ucT310Status = IQ_T310_STATUS_UNKNOWN;
      break;
    default:
      MMAL_ASSERT(0);
      break;
  }

  IQ_SubmitMetric( IQ_METRIC_ID_LT13, &lt13_metric, sizeof( lt13_metric ) );
}


#endif /*FEATURE_MMAL_LTE */
#endif /* FEATURE_MMAL_CIQ*/

