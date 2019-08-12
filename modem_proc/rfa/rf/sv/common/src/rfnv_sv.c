/*===========================================================================

                         R F   SV  N V   M a n a g e r

DESCRIPTION
  This header file contains definitions necessary inorder to interface with
  the RF 1X NV Manager.

Copyright (c) 2010 - 2013      by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/sv/common/src/rfnv_sv.c#1 $
$DateTime: 2016/12/13 07:59:52 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------     
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
03/13/13   aro     Freeing the SV NV malloc
12/07/12   gvn     Added IM3 backoff for LTE B2/B25 and 1X B15 combination 
                   (Merge from Nikel CR399105)
05/16/12   php     Add IM3 backoff for LTE B2/1x BC1
05/24/12   ndb     Implementation of IM2 backoff for LTE B4 and 1xBC0
05/23/12   ndb     Implementation of IM3 backoff for GPS desense
01/10/12   whc     Fix bug in SV NV EFS read loop     
11/17/11   Saul    SV Limiting. Added LTE B25 support. New SVDO Chans Combo
11/14/11   Saul    SV Limiting. Added support for 1x-HDR Half-Duplex and 
                   DSI backoffs.
11/11/11   Saul    SV Limiting. LTE backoff support.
11/10/11   Saul    SV Limiting. Initial support.
04/12/11   kma     Added DSI based SAR NVs for SVDO/SVLTE
10/04/10   kma     Implemented fail-safe NV values for SAR
06/16/10   kma     Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfnv_sv.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"
#include "stringl.h" /* for memscpy */

void rfnv_sv_screen_sar_nv_items(int16 sar_table[MAX_NUM_OF_ROWS][2]);

rfnv_sv_items_type rfnv_cdma1x_sv_tbl =
{
    /*************SAR POWER LIMIT*******************/

  /* SAR pwr limit for 1x BC0 and DO BC0 */
  RF_HDR_BC0_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and DO BC1 */
  RF_HDR_BC1_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and DO BC4 */
  RF_HDR_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and DO BC6 */
  RF_HDR_BC6_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and DO BC10 */
  RF_HDR_BC10_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and DO BC15 */
  RF_HDR_BC15_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and DO BC0 */
  RF_HDR_BC0_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and DO BC1 */
  RF_HDR_BC1_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and DO BC4 */
  RF_HDR_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and DO BC6 */
  RF_HDR_BC6_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and DO BC10 */
  RF_HDR_BC10_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and DO BC15 */
  RF_HDR_BC15_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC4 and DO BC0 */
  RF_HDR_BC0_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC4 and DO BC1 */
  RF_HDR_BC1_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC4 and DO BC4 */
  RF_HDR_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC4 and DO BC6 */
  RF_HDR_BC6_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC4 and DO BC10 */
  RF_HDR_BC10_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC4 and DO BC15 */
  RF_HDR_BC15_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and DO BC0 */
  RF_HDR_BC0_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and DO BC1 */
  RF_HDR_BC1_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and DO BC4 */
  RF_HDR_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and DO BC6 */
  RF_HDR_BC6_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and DO BC10 */
  RF_HDR_BC10_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and DO BC15 */
  RF_HDR_BC15_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC10 and DO BC0 */
  RF_HDR_BC0_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC10 and DO BC1 */
  RF_HDR_BC1_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC10 and DO BC4 */
  RF_HDR_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC10 and DO BC6 */
  RF_HDR_BC6_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC10 and DO BC10 */
  RF_HDR_BC10_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC10 and DO BC15 */
  RF_HDR_BC15_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and DO BC0 */
  RF_HDR_BC0_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and DO BC1 */
  RF_HDR_BC1_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and DO BC4 */
  RF_HDR_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and DO BC6 */
  RF_HDR_BC6_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and DO BC10 */
  RF_HDR_BC10_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and DO BC15 */
  RF_HDR_BC15_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and LTE BC13 */
  RF_LTE_BC13_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and LTE BC13 */
  RF_LTE_BC13_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and LTE BC13 */
  RF_LTE_BC13_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and LTE BC2 */
  RF_LTE_BC2_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and LTE BC5 */
  RF_LTE_BC5_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and LTE BC12 */
  RF_LTE_BC12_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and LTE BC2 */
  RF_LTE_BC2_DEFAULT_SAR_LIMIT,

   /* SAR pwr limit for 1x BC1 and LTE BC5 */
  RF_LTE_BC5_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and LTE BC12 */
  RF_LTE_BC12_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and LTE BC2 */
  RF_LTE_BC2_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_SAR_LIMIT,

   /* SAR pwr limit for 1x BC15 and LTE BC5 */
  RF_LTE_BC5_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC15 and LTE BC12 */
  RF_LTE_BC12_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC4 and LTE BC5 */
  RF_LTE_BC5_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and LTE B25 */
  RF_LTE_B25_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and LTE B25 */
  RF_LTE_B25_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC10 and LTE B25 */
  RF_LTE_B25_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC0 and LTE BC17 */
  RF_LTE_B17_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC1 and LTE BC17 */
  RF_LTE_B17_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC6 and LTE BC17 */
  RF_LTE_B17_DEFAULT_SAR_LIMIT,

  /* SAR pwr limit for 1x BC10 and LTE BC2 */
  RF_LTE_BC2_DEFAULT_SAR_LIMIT,

/*************IM3 de-sense POWER LIMIT**************************/

  /* IM3 de-sense pwr limit for 1x BC0 and LTE BC13 */
  RF_LTE_BC13_DEFAULT_IM3_LIMIT,

  /* IM3 de-sense pwr limit for 1x BC1 and LTE BC25 */
  RF_LTE_B25_DEFAULT_IM3_LIMIT,

/* IM3 de-sense pwr limit for PCS and AWS band when GNSS is active*/
  RF_AWS_PCS_GNSS_DEFAULT_IM3_LIMIT,

  /* IM3 de-sense pwr limit for 1x BC1 and LTE BC2 */
  RF_LTE_B2_DEFAULT_IM3_LIMIT,

  /*LTE IM3 de-sense pwr limit :LTE BC2+ 1x B15 ,LTE BC25+1x B15 */ 
  RF_CDMA1X_BC15_LTE_B2_B25_LTE_IM3_LIMIT,
  
  /*CDMA 1x IM3 de-sense pwr limit for :LTE BC25 + 1x B15, LTE BC25 + 1x B8*/
  RF_CDMA1X_BC15_LTE_B25_1X_IM3_LIMIT,

/*************IM2 de-sense POWER LIMIT**************************/

  /* IM2 de-sense pwr limit for 1x BC0 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_IM2_LIMIT,

/*************Half-duplex de-sense POWER LIMIT*******************/

  /* Half-duplex de-sense pwr limit for DO BC0*/
  RF_HDR_BC0_DEFAULT_HALF_DUPLEX_LIMIT,

  /* Half-duplex de-sense pwr limit for DO BC1*/
  RF_HDR_BC1_DEFAULT_HALF_DUPLEX_LIMIT,

  /* Half-duplex de-sense pwr limit for DO BC6*/
  RF_HDR_BC6_DEFAULT_HALF_DUPLEX_LIMIT,

/************DSI based SVDO SAR pwr bias************************/

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC0*/
  RF_HDR_BC0_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC1*/
  RF_HDR_BC1_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC4*/
  RF_HDR_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC6*/
  RF_HDR_BC6_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC10*/
  RF_HDR_BC10_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC15*/
  RF_HDR_BC15_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC0*/
  RF_HDR_BC0_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC1*/
  RF_HDR_BC1_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC4*/
  RF_HDR_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC6*/
  RF_HDR_BC6_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC10*/
  RF_HDR_BC10_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC15*/
  RF_HDR_BC15_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC0*/
  RF_HDR_BC0_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC1*/
  RF_HDR_BC1_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC4*/
  RF_HDR_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC6*/
  RF_HDR_BC6_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC10*/
  RF_HDR_BC10_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC15*/
  RF_HDR_BC15_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC0*/
  RF_HDR_BC0_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC1*/
  RF_HDR_BC1_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC4*/
  RF_HDR_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC6*/
  RF_HDR_BC6_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC10*/
  RF_HDR_BC10_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC15*/
  RF_HDR_BC15_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC0*/
  RF_HDR_BC0_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC1*/
  RF_HDR_BC1_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC4*/
  RF_HDR_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC6*/
  RF_HDR_BC6_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC10*/
  RF_HDR_BC10_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC15*/
  RF_HDR_BC15_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC0*/
  RF_HDR_BC0_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC1*/
  RF_HDR_BC1_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC4*/
  RF_HDR_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC6*/
  RF_HDR_BC6_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC10*/
  RF_HDR_BC10_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC15*/
  RF_HDR_BC15_DEFAULT_DSI_SAR_BIAS,

/************DSI based SVLTE SAR pwr bias************************/

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC6 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC13 */
  RF_LTE_BC13_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC13 */
  RF_LTE_BC13_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC6 and LTE BC13 */
  RF_LTE_BC13_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC2 */
  RF_LTE_BC2_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC2 */
  RF_LTE_BC2_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC2 */
  RF_LTE_BC2_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC4 */
  RF_LTE_BC4_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC4 and LTE BC5 */
  RF_LTE_BC5_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC25 */
  RF_LTE_BC25_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC25 */
  RF_LTE_BC25_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC10 and LTE BC25 */
  RF_LTE_BC25_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC10 and LTE BC2 */
  RF_LTE_BC2_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC12 */
  RF_LTE_BC12_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC12 */
  RF_LTE_BC12_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC12 */
  RF_LTE_BC12_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC17 */
  RF_LTE_BC17_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC17 */
  RF_LTE_BC17_DEFAULT_DSI_SAR_BIAS,

  /* DSI SVLTE SAR pwr limit bias for 1x BC6 and LTE BC17 */
  RF_LTE_BC17_DEFAULT_DSI_SAR_BIAS
};

rfcommon_rfnv_items_list_type rfnv_cdma1x_sv_nv_items_list[ ] =
{
        /*************SAR POWER LIMIT*******************/

  /* SAR pwr limit for 1x BC0 and DO BC0 */
  {RFNV_CDMA1X_BC0_DO_BC0_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and DO BC1 */
  {RFNV_CDMA1X_BC0_DO_BC1_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and DO BC6 */
  {RFNV_CDMA1X_BC0_DO_BC6_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and DO BC15 */
  {RFNV_CDMA1X_BC0_DO_BC15_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and DO BC0 */
  {RFNV_CDMA1X_BC1_DO_BC0_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and DO BC1 */
  {RFNV_CDMA1X_BC1_DO_BC1_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and DO BC6 */
  {RFNV_CDMA1X_BC1_DO_BC6_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and DO BC15 */
  {RFNV_CDMA1X_BC1_DO_BC15_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC4 and DO BC4 */
  {RFNV_CDMA1X_BC4_DO_BC4_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  /* SAR pwr limit for 1x BC6 and DO BC0 */
  {RFNV_CDMA1X_BC6_DO_BC0_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC6 and DO BC1 */
  {RFNV_CDMA1X_BC6_DO_BC1_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC6 and DO BC6 */
  {RFNV_CDMA1X_BC6_DO_BC6_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC10 and DO BC0 */
  {RFNV_CDMA1X_BC10_DO_BC0_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC10 and DO BC1 */
  {RFNV_CDMA1X_BC10_DO_BC1_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC15 and DO BC0 */
  {RFNV_CDMA1X_BC15_DO_BC0_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC15 and DO BC1 */
  {RFNV_CDMA1X_BC15_DO_BC1_SAR_I,	 NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC15 and DO BC15 */
  {RFNV_CDMA1X_BC15_DO_BC15_SAR_I,	NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and LTE BC4 */
  {RFNV_CDMA1X_BC0_LTE_BC4_SAR_I,	NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and LTE BC13 */
  {RFNV_CDMA1X_BC0_LTE_BC13_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and LTE BC17 */
  {RFNV_CDMA1X_BC0_LTE_BC17_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and LTE BC4 */
  {RFNV_CDMA1X_BC1_LTE_BC4_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and LTE BC13 */
  {RFNV_CDMA1X_BC1_LTE_BC13_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and LTE BC17 */
  {RFNV_CDMA1X_BC1_LTE_BC17_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC6 and LTE BC4 */
  {RFNV_CDMA1X_BC6_LTE_BC4_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC6 and LTE BC13 */
  {RFNV_CDMA1X_BC6_LTE_BC13_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC6 and LTE BC17 */
  {RFNV_CDMA1X_BC6_LTE_BC17_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and LTE BC2 */
  {RFNV_CDMA1X_BC0_LTE_BC2_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and LTE BC5 */
  {RFNV_CDMA1X_BC0_LTE_BC5_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and LTE BC12 */
  {RFNV_CDMA1X_BC0_LTE_BC12_SAR_I,   NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and LTE BC2 */
  {RFNV_CDMA1X_BC1_LTE_BC2_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and LTE BC5 */
  {RFNV_CDMA1X_BC1_LTE_BC5_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and LTE BC12 */
  {RFNV_CDMA1X_BC1_LTE_BC12_SAR_I,   NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC15 and LTE BC2 */
  {RFNV_CDMA1X_BC15_LTE_BC2_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC15 and LTE BC4 */
  {RFNV_CDMA1X_BC15_LTE_BC4_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC15 and LTE BC5 */
  {RFNV_CDMA1X_BC15_LTE_BC5_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC15 and LTE BC12 */
  {RFNV_CDMA1X_BC15_LTE_BC12_SAR_I,   NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC4 and LTE BC5 */
  {RFNV_CDMA1X_BC4_LTE_BC5_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC0 and LTE B25 */
  {RFNV_CDMA1X_BC0_LTE_B25_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC1 and LTE B25 */
  {RFNV_CDMA1X_BC1_LTE_B25_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* SAR pwr limit for 1x BC10 and LTE B25 */
  {RFNV_CDMA1X_BC10_LTE_B25_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },
/*************IM3 de-sense POWER LIMIT**************************/

  /* IM3 de-sense pwr limit for 1x BC0 and LTE BC13 */
  {RFNV_CDMA1X_BC0_LTE_BC13_IM3_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* IM3 de-sense pwr limit for 1x BC1 and LTE B25 */
  {RFNV_CDMA1X_BC1_LTE_B25_IM3_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* IM3 de-sense pwr limit for 1x BC1 and LTE B2 */
  {RFNV_CDMA1X_BC1_LTE_B2_IM3_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /*GNSS IM3 de-sense pwr limit*/
  {RFNV_SV_AWS_PCS_GNSS_IM3_DESENSE_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /*LTE IM3 de-sense pwr limit :LTE BC2+ 1x B15 ,LTE BC25+1x B15 */
  {RFNV_CDMA1X_BC15_LTE_B2_B25_LTE_IM3_DESENSE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /*CDMA 1x IM3 de-sense pwr limit for :LTE BC25 + 1x B15, LTE BC25 + 1x B8*/
  {RFNV_CDMA1X_BC15_LTE_B25_1X_IM3_DESENSE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK },		

  /*IM2 de-sense pwr limit 1xBC0 and LTE B4*/
  {RFNV_CDMA1X_BC0_LTE_B4_IM2_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

/*************Half-duplex de-sense POWER LIMIT*******************/

  /* Half-duplex de-sense pwr limit for DO BC0*/
  {RFNV_HDR_BC0_HALF_DUPLEX_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* Half-duplex de-sense pwr limit for DO BC1*/
  {RFNV_HDR_BC1_HALF_DUPLEX_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* Half-duplex de-sense pwr limit for DO BC6*/
  {RFNV_HDR_BC6_HALF_DUPLEX_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

/************DSI based SVDO SAR pwr bias************************/

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC0*/
  {RFNV_CDMA1X_BC0_DO_BC0_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC1*/
  {RFNV_CDMA1X_BC0_DO_BC1_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC4*/
  //{RFNV_CDMA1X_BC0_DO_BC4_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC6*/
  //{RFNV_CDMA1X_BC0_DO_BC6_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC10*/
  //{RFNV_CDMA1X_BC0_DO_BC10_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC0 and DO BC15*/
  {RFNV_CDMA1X_BC0_DO_BC15_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC0*/
  {RFNV_CDMA1X_BC1_DO_BC0_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC1*/
  {RFNV_CDMA1X_BC1_DO_BC1_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC4*/
  //{RFNV_CDMA1X_BC1_DO_BC4_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC6*/
  //{RFNV_CDMA1X_BC1_DO_BC6_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC10*/
  //{RFNV_CDMA1X_BC1_DO_BC10_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC1 and DO BC15*/
  {RFNV_CDMA1X_BC1_DO_BC15_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC0*/
  //{RFNV_CDMA1X_BC4_DO_BC0_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC1*/
  //{RFNV_CDMA1X_BC4_DO_BC1_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC4*/
  {RFNV_CDMA1X_BC4_DO_BC4_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC6*/
  //{RFNV_CDMA1X_BC4_DO_BC6_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC10*/
  //{RFNV_CDMA1X_BC4_DO_BC10_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC4 and DO BC15*/
  //{RFNV_CDMA1X_BC4_DO_BC15_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC0*/
  //{RFNV_CDMA1X_BC6_DO_BC0_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC1*/
  //{RFNV_CDMA1X_BC6_DO_BC1_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC4*/
  //{RFNV_CDMA1X_BC6_DO_BC4_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC6*/
  //{RFNV_CDMA1X_BC6_DO_BC6_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC10*/
  //{RFNV_CDMA1X_BC6_DO_BC10_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC6 and DO BC15*/
  //{RFNV_CDMA1X_BC6_DO_BC15_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC0*/
  {RFNV_CDMA1X_BC10_DO_BC0_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC1*/
  {RFNV_CDMA1X_BC10_DO_BC1_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC4*/
  //{RFNV_CDMA1X_BC10_DO_BC4_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC6*/
  //{RFNV_CDMA1X_BC10_DO_BC6_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC10*/
  //{RFNV_CDMA1X_BC10_DO_BC10_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC10 and DO BC15*/
  //{RFNV_CDMA1X_BC10_DO_BC15_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC0*/
  //{RFNV_CDMA1X_BC15_DO_BC0_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC1*/
  {RFNV_CDMA1X_BC15_DO_BC1_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC4*/
  //{RFNV_CDMA1X_BC15_DO_BC4_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC6*/
  //{RFNV_CDMA1X_BC15_DO_BC6_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC10*/
  //{RFNV_CDMA1X_BC15_DO_BC10_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVDO SAR pwr limit bias for 1x BC15 and DO BC15*/
  {RFNV_CDMA1X_BC15_DO_BC15_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },
/************DSI based SVLTE SAR pwr bias************************/

 /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC4 */
  {RFNV_CDMA1X_BC0_LTE_BC4_DSI_SAR_I,	  NV_ITEM_ATTRIB_NON_CRTCL_BMSK },
		
 /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC4 */
  {RFNV_CDMA1X_BC1_LTE_BC4_DSI_SAR_I,	  NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC13 */
  {RFNV_CDMA1X_BC0_LTE_BC13_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC13 */
  {RFNV_CDMA1X_BC1_LTE_BC13_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC2 */
  {RFNV_CDMA1X_BC0_LTE_BC2_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC2 */
  {RFNV_CDMA1X_BC1_LTE_BC2_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC2 */
  {RFNV_CDMA1X_BC15_LTE_BC2_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC4 */
  {RFNV_CDMA1X_BC15_LTE_BC4_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC4 and LTE BC5 */
  {RFNV_CDMA1X_BC4_LTE_BC5_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC25 */
  {RFNV_CDMA1X_BC0_LTE_B25_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC25 */
  {RFNV_CDMA1X_BC1_LTE_B25_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC10 and LTE BC25 */
  {RFNV_CDMA1X_BC10_LTE_B25_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC12 */
  {RFNV_CDMA1X_BC0_LTE_B12_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC12 */
  {RFNV_CDMA1X_BC1_LTE_B12_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK },

  /* DSI SVLTE SAR pwr limit bias for 1x BC15 and LTE BC12 */
  {RFNV_CDMA1X_BC15_LTE_B12_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK }

  /* DSI SVLTE SAR pwr limit bias for 1x BC0 and LTE BC17 */
  //{RFNV_CDMA1X_BC0_LTE_BC17_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK }

  /* DSI SVLTE SAR pwr limit bias for 1x BC1 and LTE BC17 */
  //{RFNV_CDMA1X_BC1_LTE_BC17_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK }

  /* DSI SVLTE SAR pwr limit bias for 1x BC6 and LTE BC17 */
  //{RFNV_CDMA1X_BC6_LTE_BC17_DSI_SAR_I,    NV_ITEM_ATTRIB_NON_CRTCL_BMSK }

};

/*===========================================================================

FUNCTION RFNV_CDMA1X_SV_COPY_NV_ITEM              EXTERNALIZED FUNCTION

DESCRIPTION
  This function updates the specfied item (item_code) 
  in the rfnv_cdma1x_sv_tbl with the data 
  supplied via the item_ptr.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfnv_cdma1x_sv_copy_nv_items
(
  rfnv_item_id_enum_type item_code,
  rfnv_item_type *rfnv_item_ptr
)
{
  switch (item_code) 
  {

  case RFNV_CDMA1X_BC0_DO_BC0_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_sar);
    break;

  case RFNV_CDMA1X_BC0_DO_BC1_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_sar);
    break;

  case RFNV_CDMA1X_BC0_DO_BC6_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc6_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc6_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc6_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc6_sar);
    break;

  case RFNV_CDMA1X_BC0_DO_BC15_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_sar);
    break;

  case RFNV_CDMA1X_BC1_DO_BC0_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_sar);
    break;

  case RFNV_CDMA1X_BC1_DO_BC1_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_sar);
    break;

  case RFNV_CDMA1X_BC1_DO_BC6_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc6_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc6_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc6_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc6_sar);
    break;

  case RFNV_CDMA1X_BC1_DO_BC15_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_sar);
    break;

  case RFNV_CDMA1X_BC4_DO_BC4_SAR_I:
	  memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_sar,
          sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_sar),
			     rfnv_item_ptr,
			     sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_sar));
	  rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_sar);
	  break;

	
  case RFNV_CDMA1X_BC6_DO_BC0_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc0_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc0_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc0_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc0_sar);
    break;

  case RFNV_CDMA1X_BC6_DO_BC1_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc1_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc1_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc1_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc1_sar);
    break;

  case RFNV_CDMA1X_BC6_DO_BC6_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc6_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc6_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc6_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc6_do_bc6_sar);
    break;

  case RFNV_CDMA1X_BC10_DO_BC0_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_sar);
    break;

  case RFNV_CDMA1X_BC10_DO_BC1_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_sar);
    break;

  case RFNV_CDMA1X_BC15_DO_BC0_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc0_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc0_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc0_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc0_sar);
    break;

  case RFNV_CDMA1X_BC15_DO_BC1_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_sar);
    break;

  case RFNV_CDMA1X_BC15_DO_BC15_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_BC4_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_BC13_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_BC17_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc17_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc17_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc17_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc17_sar);
    break;

  case RFNV_CDMA1X_BC1_LTE_BC4_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_sar);
    break;

  case RFNV_CDMA1X_BC1_LTE_BC13_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_sar);
    break;

  case RFNV_CDMA1X_BC1_LTE_BC17_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc17_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc17_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc17_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc17_sar);
    break;

  case RFNV_CDMA1X_BC6_LTE_BC4_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc4_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc4_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc4_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc4_sar);
    break;

  case RFNV_CDMA1X_BC6_LTE_BC13_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc13_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc13_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc13_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc13_sar);
    break;

  case RFNV_CDMA1X_BC6_LTE_BC17_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc17_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc17_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc17_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc6_lte_bc17_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_BC13_IM3_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_im3,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_im3),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_im3));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_im3);
    break;

  case RFNV_CDMA1X_BC1_LTE_B25_IM3_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_im3,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_im3),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_im3));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_im3);
    break;

  case RFNV_CDMA1X_BC1_LTE_B2_IM3_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b2_im3,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b2_im3),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b2_im3));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b2_im3);
    break;

  case RFNV_SV_AWS_PCS_GNSS_IM3_DESENSE_I:
    memscpy(rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense,
           sizeof(rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.sv_aws_pcs_gnss_im3_desense);
    break;	

  case RFNV_CDMA1X_BC15_LTE_B2_B25_LTE_IM3_DESENSE_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b2_b25_lte_im3_desense,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b2_b25_lte_im3_desense),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b2_b25_lte_im3_desense));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b2_b25_lte_im3_desense);
    break;	 

  case RFNV_CDMA1X_BC15_LTE_B25_1X_IM3_DESENSE_I: 
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b25_1x_im3_desense,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b25_1x_im3_desense),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b25_1x_im3_desense));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_b25_1x_im3_desense);
    break;	

  case RFNV_CDMA1X_BC0_LTE_B4_IM2_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b4_im2,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b4_im2),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b4_im2));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b4_im2);
    break;

  case RFNV_HDR_BC0_HALF_DUPLEX_I:
    memscpy(rfnv_cdma1x_sv_tbl.hdr_bc0_half_duplex,
           sizeof(rfnv_cdma1x_sv_tbl.hdr_bc0_half_duplex),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.hdr_bc0_half_duplex));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.hdr_bc0_half_duplex);
    break;

  case RFNV_HDR_BC1_HALF_DUPLEX_I:
    memscpy(rfnv_cdma1x_sv_tbl.hdr_bc1_half_duplex,
           sizeof(rfnv_cdma1x_sv_tbl.hdr_bc1_half_duplex),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.hdr_bc1_half_duplex));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.hdr_bc1_half_duplex);
    break;

  case RFNV_HDR_BC6_HALF_DUPLEX_I:
    memscpy(rfnv_cdma1x_sv_tbl.hdr_bc6_half_duplex,
           sizeof(rfnv_cdma1x_sv_tbl.hdr_bc6_half_duplex),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.hdr_bc6_half_duplex));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.hdr_bc6_half_duplex);
    break;

  case RFNV_CDMA1X_BC0_DO_BC0_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc0_dsi_sar));
    break;

  case RFNV_CDMA1X_BC0_DO_BC1_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc1_dsi_sar));
    break;

  case RFNV_CDMA1X_BC0_DO_BC15_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_do_bc15_dsi_sar));
    break;

  case RFNV_CDMA1X_BC1_DO_BC0_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc0_dsi_sar));
    break;

  case RFNV_CDMA1X_BC1_DO_BC1_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc1_dsi_sar));
    break;

  case RFNV_CDMA1X_BC1_DO_BC15_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_do_bc15_dsi_sar));
    break;

  case RFNV_CDMA1X_BC4_DO_BC4_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc4_do_bc4_dsi_sar));
    break;

  case RFNV_CDMA1X_BC10_DO_BC0_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc0_dsi_sar));
    break;

  case RFNV_CDMA1X_BC10_DO_BC1_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_do_bc1_dsi_sar));
    break;

  case RFNV_CDMA1X_BC15_DO_BC1_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc1_dsi_sar));
    break;

  case RFNV_CDMA1X_BC15_DO_BC15_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_do_bc15_dsi_sar));
    break;

  case RFNV_CDMA1X_BC0_LTE_BC4_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc4_dsi_sar));
    break;

  case RFNV_CDMA1X_BC1_LTE_BC4_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc4_dsi_sar));
    break;



  case RFNV_CDMA1X_BC0_LTE_BC13_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc13_dsi_sar));
    break;

  case RFNV_CDMA1X_BC1_LTE_BC13_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc13_dsi_sar));
    break;

  case RFNV_CDMA1X_BC0_LTE_BC2_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_dsi_sar));
    break;

  case RFNV_CDMA1X_BC1_LTE_BC2_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_dsi_sar));
    break;

  case RFNV_CDMA1X_BC15_LTE_BC2_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_dsi_sar));
    break;

  case RFNV_CDMA1X_BC15_LTE_BC4_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_dsi_sar));
    break;

  case RFNV_CDMA1X_BC4_LTE_BC5_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_dsi_sar));
    break;

  case RFNV_CDMA1X_BC0_LTE_BC2_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc2_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_BC5_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc5_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc5_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc5_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc5_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_BC12_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_B12_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_bc12_dsi_sar));
    break;

  case RFNV_CDMA1X_BC1_LTE_BC2_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc2_sar);
    break;

  case RFNV_CDMA1X_BC1_LTE_BC5_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc5_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc5_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc5_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc5_sar);
    break;

  case RFNV_CDMA1X_BC1_LTE_BC12_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_sar);
    break;
    
  case RFNV_CDMA1X_BC1_LTE_B12_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_bc12_dsi_sar));
    break;
       
  case RFNV_CDMA1X_BC15_LTE_BC2_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc2_sar);
    break;

  case RFNV_CDMA1X_BC15_LTE_BC4_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc4_sar);
    break;

  case RFNV_CDMA1X_BC15_LTE_BC5_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc5_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc5_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc5_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc5_sar);
    break;
    
  case RFNV_CDMA1X_BC15_LTE_BC12_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_sar);
    break;

  case RFNV_CDMA1X_BC15_LTE_B12_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc15_lte_bc12_dsi_sar));
    break;

  case RFNV_CDMA1X_BC4_LTE_BC5_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc4_lte_bc5_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_B25_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_sar);
    break;

  case RFNV_CDMA1X_BC1_LTE_B25_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_sar);
    break;

  case RFNV_CDMA1X_BC10_LTE_B25_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_sar));
    rfnv_sv_screen_sar_nv_items(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_sar);
    break;

  case RFNV_CDMA1X_BC0_LTE_B25_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc0_lte_b25_dsi_sar));
    break;

  case RFNV_CDMA1X_BC1_LTE_B25_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc1_lte_b25_dsi_sar));
    break;

  case RFNV_CDMA1X_BC10_LTE_B25_DSI_SAR_I:
    memscpy(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_dsi_sar,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_dsi_sar),
           rfnv_item_ptr,
           sizeof(rfnv_cdma1x_sv_tbl.cdma1x_bc10_lte_b25_dsi_sar));
    break;

  default:
    MSG_ERROR( "RF NV item not accounted for: %d", item_code, 0,0 );
    break;
  }
}

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Interface to load SV NV into SV structures.

  @param task_ptr
  The TCB of the task that is requesting the RF driver to load the
  configuration data.  This must match rex_self() or the NV loading
  will fail.
  
  @param task_wait_sig
  The signal the requesting task has allocated to the RF driver to wait
  on to indicate NV completion.
  
  @param task_wait_handler
  A function provided by the task that is requesting the RF driver to
  load the configuration data which performs the correct idle-waiting
  behavior for the task.

*/
boolean 
rfsv_retrieve_nv_items
(
  rex_tcb_type *task_ptr,
  rex_sigs_type task_wait_sig,
  void (*task_wait_handler)( rex_sigs_type )
)
{
  rfnv_item_type *rfnv_item = NULL;/* NV item */
  uint16 nv_item_size = sizeof(rfnv_item_type);
  nv_stat_enum_type nv_read_status; /* NV read status */
  int16 nv_item_cnt; /* loop counter for RF NV item fetches */
  
  int16 nv_item_list_siz = sizeof(rfnv_cdma1x_sv_nv_items_list) / sizeof(rfcommon_rfnv_items_list_type);
  boolean api_status = TRUE;

  rfnv_item = modem_mem_alloc( sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA );
  if ( rfnv_item == NULL )
  {
    RF_MSG( RF_ERROR, "rfsv_retrieve_nv_items: Could not "
                      "allocate buffer memory on heap" );
    api_status = FALSE;
    return api_status;
  }

  for (nv_item_cnt = 0; nv_item_cnt < nv_item_list_siz; nv_item_cnt++)
  {
    /* read from NV into local buffer */
    nv_item_size = sizeof(rfnv_item_type);
    nv_read_status = rfcommon_rfnv_get_item(rfnv_cdma1x_sv_nv_items_list[nv_item_cnt].rfnv_item_id, 
                                            rfnv_item,
                                            &nv_item_size,
                                            task_ptr,
                                            task_wait_sig,
                                            task_wait_handler );
  
    /* if read status is good */
    if (nv_read_status == NV_DONE_S)
    {
      /* copy from local buffer into RF NV table */ 
      rfnv_cdma1x_sv_copy_nv_items( rfnv_cdma1x_sv_nv_items_list[nv_item_cnt].rfnv_item_id,
                                    rfnv_item);
      api_status &= TRUE;
    }
    else if (nv_read_status != NV_NOTACTIVE_S)
    {
      RF_MSG_2( RF_ERROR, "RF SV Limiting : RF NV read failed on item %d, attr=0x%x", 
                rfnv_cdma1x_sv_nv_items_list[nv_item_cnt].rfnv_item_id, 
                rfnv_cdma1x_sv_nv_items_list[nv_item_cnt].rfnv_item_attrib );
    
      if (rfnv_cdma1x_sv_nv_items_list[nv_item_cnt].rfnv_item_attrib & NV_ITEM_ATTRIB_CRTCL_BMSK)
      {
        api_status &= FALSE;
        break;
      }
    }
  } /*for (nv_item_cnt = 0; nv_item_cnt < nv_item_list_siz; nv_item_cnt++)*/

  modem_mem_free( rfnv_item, MODEM_MEM_CLIENT_RFA );
  
  return api_status;
}

/*===========================================================================

FUNCTION RFNV_SV_SCREEN_SAR_NV_ITEM             

DESCRIPTION
  This function screens SAR NV items and will detect error
  if SAR NVs do not satisfy the monotonic order configuration

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If NVs are configured properly, a fail-safe value is provided

===========================================================================*/
void rfnv_sv_screen_sar_nv_items(int16 sar_table[MAX_NUM_OF_ROWS][2])
{
  uint8 i = 0;
  boolean error_free = TRUE;
  for (i=0; i<MAX_NUM_OF_ROWS-1; i++)
  {
    if ((sar_table[i][0]>sar_table[i+1][0])||(sar_table[i][1]<sar_table[i+1][1]))
    {
      error_free = FALSE;
      break;
    }
  }
  if (!error_free)
  {
    /*provide default fail-safe SAR values*/
    for (i=0; i<MAX_NUM_OF_ROWS; i++)
    {
       sar_table[i][0] = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
       sar_table[i][1] = DEFAULT_FAIL_SAFE_TX_POWER_LIM;
    }
    MSG_FATAL("Invalid SAR NV values. Use fail-safe values!", NULL, NULL, NULL);
  }
}
