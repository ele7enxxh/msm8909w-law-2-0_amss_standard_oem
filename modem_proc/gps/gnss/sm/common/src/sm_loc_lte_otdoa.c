/*----------------------------------------------------------------------------
 *  Copyright (c) 2015 Qualcomm Atheros, Inc..
 *  All Rights Reserved.
 *  Qualcomm Atheros Confidential and Proprietary.
 *----------------------------------------------------------------------------*/

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Session Manager API

GENERAL DESCRIPTION
  This module makes up the Position Determination Session Manager of the subscriber
  station software.

  The Session Manager API file is used by MGP module to interface with external
  world

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 QUALCOMM Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.  

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/sm_loc_lte_otdoa.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/25/16   pk      During OTA Reset, Call PGI api to reset SIB8/16 Decode
12/27/14   gk    cleaned up header
12/18/11   gk    LPP related fixes
05/18/11   rc    Lowering priority of some standalone messages from MSG_HIGH to MSG_LOW
02/12/10   vp      Added Sv polynomial function for OEM DRE enablement
06/10/09   gk      Added Get UTC offset from NV
01/12/09   atien   XTRA-T support

09/15/08   gk      Rename the on-demand struct to sm_ from cgps_
04/28/08   gk      On-Demand Changes
08/01/07   rw      Add GPS report time info for XTRA feature
12/01/06   jw      Made changes to support modified struct sm_GpsMeasRptStructType
04/04/06   sk      Initial version

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definition for basic types and macros */
#include "customer.h"   /* Customer configuration file */
#include "gps_common.h"
#include "loc_lte_otdoa_gnss_api.h"
#include "sm_loc_lte_otdoa.h"
#include "sm_api.h"
#include "tm_api.h"
#include "mgp_api.h"

static gnss_ClockStructType z_gnss_clock;
#ifdef FEATURE_LOC_LTE_OTDOA
static sm_OTDOA_AssistanceData z_otdoa_ad_data;
#endif /* FEATURE_LOC_LTE_OTDOA */
static sm_OTDOA_FTA_Information z_fta_ad;
static uint32 sess_type;
#if 0
sm_loc_lte_otdoa_meas_info_type z_otdoa_measurement_info;
sm_OTDOA_ProvideLocationInformation z_OtdoaMeas;

static void loc_lte_otdoa_CopyOtdoaMeasForSm(sm_OTDOA_ProvideLocationInformation *p_OtdoaMeas);
static void loc_lte_otdoa_GetOtdoaMeasFromGps(sm_loc_lte_otdoa_meas_info_type *p_OtdoaMeasInfoToSm)
{
  /* copy the canned data */
  loc_lte_otdoa_CopyOtdoaMeasForSm(&z_OtdoaMeas);

  p_OtdoaMeasInfoToSm->p_meas_info = &z_OtdoaMeas;
  p_OtdoaMeasInfoToSm->u_sess_type = sess_type;
}


static void loc_lte_otdoa_CopyOtdoaMeasForSm(sm_OTDOA_ProvideLocationInformation *p_OtdoaMeas)
{
  /* copy the dummy OTDOA meas. to be sent to SM */
  p_OtdoaMeas->m.otdoaSignalMeasurementInformationPresent = 0x1;
  p_OtdoaMeas->m.otdoa_ErrorPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.m.cellGlobalIdRefPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.m.earfcnRefPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.m.referenceQualityPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.systemFrameNumber = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.physCellIdRef = 63;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc[0] = 3;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc[1] = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.cellGlobalIdRef.mcc[2] = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc[0] = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc[1] = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.cellGlobalIdRef.mnc[2] = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.cellGlobalIdRef.cellidentity = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.earfcnRef = 2098;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.referenceQuality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.referenceQuality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.referenceQuality.error_Value = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.referenceQuality.error_NumSamples = 7;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.n = 23;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[0].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[0].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[0].physCellIdNeighbor = 66;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[0].rstd = 6406;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[0].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[0].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[0].rstd_Quality.error_Value = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[0].rstd_Quality.error_NumSamples = 7;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[1].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[1].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[1].physCellIdNeighbor = 69;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[1].rstd = 6406;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[1].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[1].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[1].rstd_Quality.error_Value = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[1].rstd_Quality.error_NumSamples = 7;
   /* No cellGlobalIdNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].m.earfcnNeighbourPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].physCellIdNeighbor = 72;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].earfcnNeighbour = 2098;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].rstd = 6406;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].rstd_Quality.error_Value = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[2].rstd_Quality.error_NumSamples = 7;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[3].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[3].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[3].physCellIdNeighbor = 75;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[3].rstd = 6406;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[3].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[3].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[3].rstd_Quality.error_Value = 3;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[3].rstd_Quality.error_NumSamples = 6;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[4].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[4].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[4].physCellIdNeighbor = 78;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[4].rstd = 6457;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[4].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[4].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[4].rstd_Quality.error_Value = 3;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[4].rstd_Quality.error_NumSamples = 6;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[5].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[5].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[5].physCellIdNeighbor = 81;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[5].rstd = 6457;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[5].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[5].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[5].rstd_Quality.error_Value = 3;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[5].rstd_Quality.error_NumSamples = 6;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[6].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[6].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[6].physCellIdNeighbor = 84;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[6].rstd = 6457;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[6].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[6].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[6].rstd_Quality.error_Value = 3;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[6].rstd_Quality.error_NumSamples = 6;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[7].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[7].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[7].physCellIdNeighbor = 87;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[7].rstd = 6457;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[7].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[7].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[7].rstd_Quality.error_Value = 10;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[7].rstd_Quality.error_NumSamples = 5;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[8].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[8].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[8].physCellIdNeighbor = 90;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[8].rstd = 6559;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[8].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[8].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[8].rstd_Quality.error_Value = 10;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[8].rstd_Quality.error_NumSamples = 5;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[9].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[9].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[9].physCellIdNeighbor = 93;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[9].rstd = 6559;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[9].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[9].rstd_Quality.error_Resolution = 0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[9].rstd_Quality.error_Value = 10;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[9].rstd_Quality.error_NumSamples = 5;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[10].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[10].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[10].physCellIdNeighbor = 96;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[10].rstd = 6559;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[10].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[10].rstd_Quality.error_Resolution = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[10].rstd_Quality.error_Value = 10;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[10].rstd_Quality.error_NumSamples = 4;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[11].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[11].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[11].physCellIdNeighbor = 99;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[11].rstd = 6559;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[11].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[11].rstd_Quality.error_Resolution = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[11].rstd_Quality.error_Value = 10;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[11].rstd_Quality.error_NumSamples = 4;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[12].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[12].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[12].physCellIdNeighbor = 102;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[12].rstd = 6867;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[12].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[12].rstd_Quality.error_Resolution = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[12].rstd_Quality.error_Value = 10;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[12].rstd_Quality.error_NumSamples = 4;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[13].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[13].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[13].physCellIdNeighbor = 104;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[13].rstd = 6867;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[13].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[13].rstd_Quality.error_Resolution = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[13].rstd_Quality.error_Value = 25;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[13].rstd_Quality.error_NumSamples = 3;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[14].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[14].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[14].physCellIdNeighbor = 108;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[14].rstd = 6867;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[14].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[14].rstd_Quality.error_Resolution = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[14].rstd_Quality.error_Value = 25;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[14].rstd_Quality.error_NumSamples = 3;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[15].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[15].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[15].physCellIdNeighbor = 111;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[15].rstd = 6867;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[15].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[15].rstd_Quality.error_Resolution = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[15].rstd_Quality.error_Value = 25;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[15].rstd_Quality.error_NumSamples = 3;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[16].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[16].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[16].physCellIdNeighbor = 114;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[16].rstd = 7379;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[16].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[16].rstd_Quality.error_Resolution = 1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[16].rstd_Quality.error_Value = 25;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[16].rstd_Quality.error_NumSamples = 3;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[17].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[17].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[17].physCellIdNeighbor = 117;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[17].rstd = 7379;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[17].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[17].rstd_Quality.error_Resolution = 2;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[17].rstd_Quality.error_Value = 25;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[17].rstd_Quality.error_NumSamples = 0;
   /* No cellGlobalIdNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].m.earfcnNeighbourPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].physCellIdNeighbor = 120;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].earfcnNeighbour = 2098;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].rstd = 7379;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].rstd_Quality.error_Resolution = 2;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].rstd_Quality.error_Value = 25;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[18].rstd_Quality.error_NumSamples = 0;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[19].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[19].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[19].physCellIdNeighbor = 123;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[19].rstd = 7379;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[19].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[19].rstd_Quality.error_Resolution = 2;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[19].rstd_Quality.error_Value = 25;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[19].rstd_Quality.error_NumSamples = 0;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[20].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[20].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[20].physCellIdNeighbor = 126;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[20].rstd = 10449;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[20].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[20].rstd_Quality.error_Resolution = 2;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[20].rstd_Quality.error_Value = 31;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[20].rstd_Quality.error_NumSamples = 0;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[21].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[21].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[21].physCellIdNeighbor = 129;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[21].rstd = 12710;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[21].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[21].rstd_Quality.error_Resolution = 3;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[21].rstd_Quality.error_Value = 31;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[21].rstd_Quality.error_NumSamples = 1;
   /* No cellGlobalIdNeighbourPresent, no earfcnNeighbourPresent */
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[22].m.cellGlobalIdNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[22].m.earfcnNeighbourPresent = 0x0;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[22].physCellIdNeighbor = 132;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[22].rstd = 12710;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[22].rstd_Quality.m.error_NumSamplesPresent = 0x1;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[22].rstd_Quality.error_Resolution = 3;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[22].rstd_Quality.error_Value = 31;
  p_OtdoaMeas->otdoaSignalMeasurementInformation.neighbourMeasurementList.elem[22].rstd_Quality.error_NumSamples = 1;
  p_OtdoaMeas->otdoa_Error = SM_OTDOA_cause_undefined; //not imp. as error_preset = 0
}
#endif
#ifdef FEATURE_LOC_LTE_OTDOA
/*===========================================================================
  sm_loc_lte_otoda_start_cmd

  Description:

     This function is used by SM to send a start to PL_LTE_OTDOA module. This
     will send an IPC to the PL_LTE_OTODA module

  Parameters: 
    sm_OTDOA_AssistanceData - pointer to the AD provided by the network
    timeout - value given by the network

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_start_cmd(const sm_OTDOA_AssistanceData *p_otdoa_ad,
                                   uint32 q_operation_mode, uint32 u_sess_type)
{
  
  loc_lte_otdoa_SmStartCmdInfoType z_StartCmd;
  z_StartCmd.p_OtdoaAssistanceData = &z_otdoa_ad_data;
  if (p_otdoa_ad)
  {
    memscpy(z_StartCmd.p_OtdoaAssistanceData, sizeof(*z_StartCmd.p_OtdoaAssistanceData),
            p_otdoa_ad, sizeof(*p_otdoa_ad));
  }
  else
  {
    z_StartCmd.p_OtdoaAssistanceData = NULL;
  }
  z_StartCmd.e_OpMode = q_operation_mode;
  z_StartCmd.e_SessType = u_sess_type;
  sess_type = u_sess_type;
  loc_lte_otdoa_StartSessCmd (&z_StartCmd);

  return TRUE;
}

#endif

/*===========================================================================
  sm_loc_lte_otoda_get_measurements

  Description:

     This function is used by SM to request OTDOA measurements from PL_LTE_OTDOA module.
     This GNSS time stamps is sent if the OTDOA measurements are to be time aligned,
     NULL if no time alignment is needed and the current measurements are to be returned.

  Parameters: 
    gnss_TimeStructType *p_gnss_clock - gnss time if time alignment needed, else NULL

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_get_measurements(gnss_ClockStructType *p_gnss_clock, uint32 u_sess_type, boolean earlyMeasurement)
{
  loc_lte_otdoa_ReqMeasRptCmdType z_MeasRptCmd;

  z_MeasRptCmd.p_Clock = &z_gnss_clock;
  z_MeasRptCmd.b_IsEarlyRptReq = earlyMeasurement;
  if (p_gnss_clock != NULL)
  {
    memscpy(z_MeasRptCmd.p_Clock, sizeof(*z_MeasRptCmd.p_Clock), p_gnss_clock, sizeof(*p_gnss_clock));
  }
  else
  {
    z_MeasRptCmd.p_Clock = NULL;
  }
  z_MeasRptCmd.e_SessType = u_sess_type;
  loc_lte_otdoa_ReqMeasRptCmd (&z_MeasRptCmd);
  #if 0
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;
  sm_loc_lte_otdoa_get_measurements_type z_get_meas_type;
  
  MSG_HIGH("SM_PL_LTE_OTDOA_API: GET measurements CMD...",0,0,0);

  msg_id      = (uint32)SM_LOC_LTE_OTDOA_MSG_ID_GET_MEAS_CMD;
  size        = sizeof(sm_loc_lte_otdoa_get_measurements_type);

  z_get_meas_type.p_clock = p_gnss_clock;
  z_get_meas_type.u_sess_type = u_sess_type;


  /* Send the MSG to OTDOA thread
  */
  ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_MGP_MC, (void *)&z_get_meas_type);

  if(ipc_msg_ptr != NULL)
  {
     MSG_HIGH("SM_PL_LTE_OTDOA_API: IPC msg send to OTDOA, id = %d",ipc_msg_ptr->q_MsgId,0,0);
     /* temp return the dummy measurements */
     loc_lte_otdoa_GetOtdoaMeasFromGps(&z_otdoa_measurement_info);
     sm_loc_lte_otdoa_recv_measurements(&z_otdoa_measurement_info); 
       
     return TRUE;
  }  
  return FALSE;
  #endif
  return TRUE;
}



/*===========================================================================
  sm_loc_lte_otoda_stop_cmd

  Description:

     This function is used by SM to STOP PL_LTE_OTDOA module

  Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_stop_cmd(uint8 u_reason, uint32 u_sess_type)
{
  loc_lte_otdoa_StopSessCmdType z_StopCmd;
  
  z_StopCmd.u_Reason = u_reason;
  z_StopCmd.e_SessType = u_sess_type;
  sess_type = u_sess_type;
  loc_lte_otdoa_StopSessCmd(&z_StopCmd);
  return TRUE;
  #if 0
  os_IpcMsgType   *ipc_msg_ptr  = NULL;
  uint32          msg_id        = 0;
  uint32          size          = 0;
  sm_loc_lte_otdoa_stop_cmd_type z_stop_info;

  MSG_HIGH("SM_PL_LTE_OTDOA_API: GET measurements CMD...",0,0,0);

  /* Depending on meas report, set the corsp msg id for sm.
  */
  msg_id      = (uint32)SM_LOC_LTE_OTDOA_MSG_ID_STOP_CMD;
  size        = sizeof(sm_loc_lte_otdoa_stop_cmd_type);

  z_stop_info.u_reason = u_reason;
  z_stop_info.u_sess_type = u_sess_type;

  /* Send the MSG to OTDOA thread
  */
  ipc_msg_ptr = sm_send_message(msg_id, size, (uint32)THREAD_ID_MGP_MC, (void *)&z_stop_info);

  if(ipc_msg_ptr != NULL)
  {
     MSG_HIGH("SM_PL_LTE_OTDOA_API: IPC msg send to OTDOA, id = %d",ipc_msg_ptr->q_MsgId,0,0);
     return TRUE;
  }  
  return FALSE;
  #endif
}


/*===========================================================================
  loc_lte_otoda_sm_recv_measurements

  Description:

     This function is used by PL_LTE_OTDOA module to send the OTDOA measurements

  Parameters:
    Reason: stop reason

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_recv_measurements(sm_loc_lte_otdoa_meas_info_type *p_otdoa_measurement_info)
{
  p_otdoa_measurement_info->u_sess_type= sess_type;
  return (tm_loc_lte_otdoa_recv_measurements(p_otdoa_measurement_info));
}



/*===========================================================================
  sm_loc_lte_otoda_update_timeout

  Description:

     This function is used by SM to provide an updated network provided timeout
     to LOC_LTE_OTDOA module
 
  Parameters:
    time out value, session type

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_otdoa_update_timeout(uint32 q_timeout,  uint32 u_sess_type )
{
  loc_lte_otdoa_UpdateTimoutInfoType z_UpdateTimeoutCmd;

  MSG_1(MSG_SSID_GPSSM, MSG_LEGACY_MED, "LTE OTDOA meas timeout %lu msec", q_timeout);

  z_UpdateTimeoutCmd.q_Timeout = q_timeout;
  z_UpdateTimeoutCmd.e_SessType = u_sess_type;
  sess_type = u_sess_type;
  loc_lte_otdoa_UpdateTimeout(&z_UpdateTimeoutCmd);
  return TRUE;
}



/*===========================================================================
  sm_loc_lte_fta_assistance_data

  Description:

     This function is used by LOC_LTE_OTDOA module to send the OTDOA measurements

  Parameters:
    measurements information

  Return value: 
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_fta_assitance_data(sm_OTDOA_FTA_Information *p_fta_ad,  uint32 u_sess_type )
{
  
  loc_lte_otdoa_FtaAidingDataInfoType z_FtaCmd;
  
  z_FtaCmd.p_FtaAidingData = &z_fta_ad;
  if (p_fta_ad)
  {
    memscpy(z_FtaCmd.p_FtaAidingData, sizeof(*z_FtaCmd.p_FtaAidingData), p_fta_ad, sizeof(*p_fta_ad));
  }
  else 
  {
     z_FtaCmd.p_FtaAidingData = NULL;
  }
  loc_lte_otdoa_UpdateFtaAidingData (&z_FtaCmd );
  return TRUE;
}

/*===========================================================================
  sm_loc_lte_ota_reset

  Description:

     This function is used to inform LOC_LTE_OTDOA module of OTA reset msg

  Parameters:
    None

  Return value:
    boolean: SUCCESS or FAIL
=============================================================================*/
boolean sm_loc_lte_ota_reset(void)
{
  loc_lte_otdoa_OtaResetCmdType z_OtaResetInfo;
  z_OtaResetInfo.u_Reserved = 0;

  /* OTA RESET - Request PGI to send a command to RRC to refresh TTR data from 
     SIB8/16  */
  mgp_pgi_SendSibTimeRefreshMsg();
  
  loc_lte_otdoa_OtaReset( &z_OtaResetInfo );
  return TRUE;
}
