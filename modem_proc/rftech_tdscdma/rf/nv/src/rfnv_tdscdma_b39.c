/*!
  @file
  rfnv_tdscdma_b39.c

  @brief

  @details
  This module retrieves all NV items that will be used by the TDSCDMA band 39(1880-1920MHz)..
  unit.

*/

/*===========================================================================
/f
Copyright (c) 2009, 2010, 2011, 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:50 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/rf/nv/src/rfnv_tdscdma_b39.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/30/14   jak     Added RFNV_TDSCDMA_<band>_MPR_VAL_V2_I support
11/03/14   jps     Updates for get PA switchpoints API
10/17/14   weiz    Support of PA static compressed NV 
10/14/14   jps     Add support for GET_PA_SWITCHPOINTS command
07/17/14   ms      Sawless RX support
05/29/14   ych     Updated for 8 PA gain states
08/25/13   jhe     Add DPD scaling NV
07/11/13   jz      Add Antenna Tuner Close Loop NV Support
06/30/13   qzh     Replace memcpy() with safer memscpy().
04/19/13   kb      Add NV support for EPT feature
07/17/12   zhzheng Fix KW warnings 
07/11/12   jps     TD-SCDMA Antenna Tuner Feature 
07/09/12   ms	   Diversity support
05/02/12   ycl     Add threshold and change MPR name.
12/12/11   hy      Fix corruption of pa_rise_fall nv
05/23/11   hy      Split Tx/Rx NV ptrs, fix hdet vs agc size
04/19/11   hy      Initial Revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stringl/stringl.h>

#include "rfcommon_nv.h"
#include "rfnv_tdscdma.h"
#include "rfnv_tdscdma_b39.h"
#include "msg.h"
#include "rfcommon_msg.h"
#include "modem_mem.h"
#include "zlib.h"

/*===========================================================================

                           CONSTANT DEFINITIONS

===========================================================================*/

/*===========================================================================

                           DATA DEFINITIONS

===========================================================================*/

/* RF NV item identifier array */
/* This array defines RF NV items that are supported and will be read */
const rfcommon_rfnv_items_list_type rfnv_tdscdma_b39_rx_items_list[ ] =
{
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_ACQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_ACQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_ACQ_2_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_ACQ_2_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_IDLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_IDLE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_IFREQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_IFREQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_PWR_SCAN_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_PWR_SCAN_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_TRACKING_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_TRACKING_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_HS_LNA_RANGE_RISE_FALL_WB_MODE_TRACKING_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_HS_LNA_RANGE_RISE_FALL_NB_MODE_TRACKING_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_RX_CAL_CHAN_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_LNA_PHASE_CTL_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_RX_GAIN_VS_FREQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
//#ifdef FEATURE_RF_TDSCDMA_RX_DIVERSITY
  {RFNV_TDSCDMA_B39_RX_DIV_GAIN_VS_FREQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_RX_DIV_CAL_CHAN_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
//#endif
  {RFNV_TDSCDMA_B39_RX_ANT_TUNER_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_RX_CAL_DATA_V2_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  
}; /* rfcommon_rfnv_items_list_type rfnv_tdscdma_B39_rx_items_list */

const rfcommon_rfnv_items_list_type rfnv_tdscdma_b39_tx_items_list[ ] =
{
  {RFNV_TDSCDMA_B39_TX_CAL_CHAN_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_MAX_TX_POWER_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_LIN_VS_TEMP_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_LIM_VS_TEMP_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_PA_RANGE_MAP_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_PA_RISE_FALL_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_AGC_PA_ON_RISE_FALL_DELAY_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_AGC_TX_ON_RISE_FALL_DELAY_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_AGC_UPDATE_TX_AGC_TIME_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_PA_GAIN_UP_DOWN_TIME_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_ROT_ANGLE_PA_STATE_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_PA_COMPENSATE_UP_DOWN_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TIMER_HYSTERISIS_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},   
  {RFNV_TDSCDMA_B39_MPR_BASED_PA_SWITCHPOINTS_SHIFT_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_MPR_VAL_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_MPR_VAL_V2_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},  
  {RFNV_TDSCDMA_B39_PA_MPR_BACKOFF_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_GAIN_SHIFT_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_TX_LIN_MASTER_0_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_GAIN_INDEX_LIN_0_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_LIN_MASTER_1_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_GAIN_INDEX_LIN_1_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_LIN_MASTER_2_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_GAIN_INDEX_LIN_2_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_LIN_MASTER_3_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_GAIN_INDEX_LIN_3_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_EXP_HDET_VS_AGC_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_COMP_VS_FREQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK}, 
  {RFNV_TDSCDMA_B39_TX_LIM_VS_FREQ_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_TX_ANT_TUNER_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  //EPT NVs
  {RFNV_TDSCDMA_B39_TX_EPT_DPD_CONFIG_PARAMS_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_TX_LIN_VS_TEMP_VS_FREQ_P_IN_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_TX_LIN_VS_TEMP_VS_FREQ_P_OUT_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  //Tuner CL NV
  {RFNV_TDSCDMA_C0_B39_ANT_TUNER_CL_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  {RFNV_TDSCDMA_B39_TX_XPT_DPD_SCALING_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

  //PA Static
  {RFNV_TDSCDMA_C0_B39_PA_STATIC_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},
  //PA Static compressed
  {RFNV_TDSCDMA_C0_B39_PA_STATIC_Z_I, NV_ITEM_ATTRIB_NON_CRTCL_BMSK},

}; /* rfcommon_rfnv_items_list_type rfnv_tdscdma_B39_tx_items_list */

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  TDSCDMA B39 RF NV tbl with default values.
  If the non-calibrated items are not read from NV, the default values are used.

*/
rfnv_tdscdma_rx_tbl_type rfnv_tdscdma_b39_rx_tbl = 
{
  /* rx calibration channels list */
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in narrow band in acquisation mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in wide band in acquisation mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in narrow band in acquisation mode if ACQ failed*/
  {20,0,0,255,20,0,255,255,20,255,255,255},
  /* lna threshold in wide band in acquisation mode if ACQ failed */
  {14,0,0,255,14,0,255,255,14,255,255,255},
  /* lna threshold in narrow band in idle mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in wide band in idle mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in narrow band in inter freq mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in wide band in inter freq mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in narrow band in power scan mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in wide band in power scan mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in narrow band in tracking mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in wide band in tracking mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in narrow band in tracking mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* lna threshold in wide band in tracking mode*/
  {0,0,0,0,0,0,0,0,0,0,0,0},
  /* absolute rx gain vs frequency */
  {  
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
//#ifdef FEATURE_RF_TDSCDMA_RX_DIVERSITY
  /* absolute rx_div gain vs frequency */
  {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },
  /* rx diversity calibration channels list */
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//#endif
  
  /* primary path LNA phase control in TDSCDMA band */
  {0,0,0,0},  

  /* antenna tuner rx */
  {  {0,0,0,0},
     {0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },
};

/*----------------------------------------------------------------------------*/
/*!
  @brief 
  TDSCDMA B39 RF NV tbl with default values.
  If the non-calibrated items are not read from NV, the default values are used.

*/
rfnv_tdscdma_tx_tbl_type rfnv_tdscdma_b39_tx_tbl = 
{
  /* tx calibration channels list */
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  /* UE maximum TX power */
  0,  
  /* tx delays and update agc time */
  {0,0},
  {0,0},
  0,
  /* pa range update time */
  {0,0},
  /* tx rotator angles */
  {0,0,0,0},
  /* PA Compensation UP/Down for PA switch threshold from gain states, max num -1  */
  {
    {0,0},
    {0,0},
    {0,0}
  },
   /* Tx linearizer versus temperature */
  {  
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  },
   /* PA Range Map */
   {0,0,0,0},
   /* timer hysterisis for swithcing from high to low gain state*/
   0,
   
   /* rise/fall value for PA switch threshold from Linearizer gain states, max num -1 */
  {
    {0,0},
    {0,0},
    {0,0}
  },
   /* offset Tx power limit versus temperature */
   {0,0,0,0,0,0,0,0},
   /* pa switch point shift */   
   {0,0,0,0,0,0,0},   
   /*  MPR power backoff */
   {0,5,10,15,20,25,15},   
   /*  MPR power backoff V2*/
   {0,5,10,15,20,25,5,10,15,20,25,5,10,15,20,25,15},   
   /* MPR analog power backoff */
   {0,0,0,0,0,0,0},   
   /* MPR digital power backoff */
    {0,0,0,0,0,0,0},
  {
   {
    /* linearizer table of 64 entry for pa range 0 */
      {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* linearizer table of 64 entry for pa range 1*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* linearizer table of 64 entry for pa range 2*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* linearizer table of 64 entry for pa range 3*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   },
   {
     /* tx gain table of 64 entry for linearizer of pa range 0*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* tx gain table of 64 entry for linearizer of pa range 1*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* tx gain table of 64 entry for linearizer of pa range 2*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     /* tx gain table of 64 entry for linearizer of pa range 3*/
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   }
  },
   /* tx power freq compensation per pa range */
   {   
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
   },   
   /* Expected HDET versus AGC */
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},   
   /* tx limiting freq compensation for high PA range */
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

   /* antenna tuner tx */
  {  {0,0,0,0},
     {0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
  },

  /* EPT DPD static params for expansion and processing */
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},

  /* Tx Lin vs Temp vs Freq Pin Comp */
  {
   0, 
   {
    {{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0},{0},{0},{0},{0},{0},{0},{0}}
   }
  },

  /* Tx Lin vs Temp vs Freq Pout Comp */
  {
   0, 
   {
    {{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0},{0},{0},{0},{0},{0},{0},{0}},
    {{0},{0},{0},{0},{0},{0},{0},{0}}
   }
  },

  {
  0,
  NULL
  },

  /* XPT DPD scaling */
  {
   0,
   {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  },

  /* lin_temp_comp_data */
  {0},

/* pa_static */
{
 {0}, //header
 {0}, //pa_range_map
 {0}, //pa_fall_swpt [0th element -> timer hystersis]
 {0}, //pa_rise_swpt
 {0}, //tx_rot_angle
 {0}, //pa_comp_up
 {0}, //pa_comp_down
 { //pin values
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
 },
 { //pout values
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
   {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},
 },
 {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}, //pa_comp_up_vs_freq
 {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}}, //pa_comp_down_vs_freq
 {{0},{0},{0},{0},{0}},  //reserved
},
};

static boolean New_NV_Used_B39 = FALSE;
static boolean Old_NV_Used_B39 = FALSE;

/*===========================================================================

                        FUNCTION DEFINITIONS

===========================================================================*/

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function copies one RF NV data item, as specified by "item_code/
  identifier" into an element of the RF NV item tbl.
  This function must be called during initialization for each and every RF
  NV item.

  @param item_code Item to put
  @param *item_ptr Pointer where to get the item

*/

void rfnv_tdscdma_b39_copy_rx_item
(
rfnv_item_id_enum_type  item_code,    /* Item to put */
rfnv_item_type          *rfnv_item_ptr     /* Pointer where to get the item */
)
{
  /* read RF NV item based on its identifier/item code */
  switch ( item_code )
  {
  /*LNA rise fall other than tracking mode is band independent.*/
  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_ACQ_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_acq[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_acq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_ACQ_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_acq[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_acq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  /*LNA rise fall other than tracking mode is band independent.*/
  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_ACQ_2_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_acq_2[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_acq_2),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_ACQ_2_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_acq_2[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_acq_2),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_IDLE_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_idle[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_idle),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_IDLE_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_idle[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_idle),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_IFREQ_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_ifreq[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_ifreq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_IFREQ_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_ifreq[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_ifreq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_PWR_SCAN_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_power_scan[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_power_scan),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_PWR_SCAN_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_power_scan[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_power_scan),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_NB_MODE_TRACKING_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_tracking[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_nb_mode_tracking),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_RANGE_RISE_FALL_WB_MODE_TRACKING_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_tracking[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_range_rise_fall_wb_mode_tracking),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_HS_LNA_RANGE_RISE_FALL_NB_MODE_TRACKING_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_hs_range_rise_fall_nb_mode_tracking[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_hs_range_rise_fall_nb_mode_tracking),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_HS_LNA_RANGE_RISE_FALL_WB_MODE_TRACKING_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_hs_range_rise_fall_wb_mode_tracking[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_hs_range_rise_fall_wb_mode_tracking),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_RX_CAL_CHAN_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.rx_cal_chan[0],sizeof(rfnv_tdscdma_b39_rx_tbl.rx_cal_chan),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_LNA_PHASE_CTL_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.lna_phase_ctrl[0],sizeof(rfnv_tdscdma_b39_rx_tbl.lna_phase_ctrl),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_RX_GAIN_VS_FREQ_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.rx_gain_vs_freq,sizeof(rfnv_tdscdma_b39_rx_tbl.rx_gain_vs_freq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

//#ifdef FEATURE_RF_TDSCDMA_RX_DIVERSITY
  case RFNV_TDSCDMA_B39_RX_DIV_GAIN_VS_FREQ_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.rx_div_gain_vs_freq,sizeof(rfnv_tdscdma_b39_rx_tbl.rx_div_gain_vs_freq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;
  case RFNV_TDSCDMA_B39_RX_DIV_CAL_CHAN_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.rx_div_cal_chan[0],sizeof(rfnv_tdscdma_b39_rx_tbl.rx_div_cal_chan),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;
//#endif

  case RFNV_TDSCDMA_B39_RX_ANT_TUNER_I:
   memscpy((void*)&rfnv_tdscdma_b39_rx_tbl.ant_tuner_rx,sizeof(rfnv_tdscdma_b39_rx_tbl.ant_tuner_rx),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_RX_CAL_DATA_V2_I:
      rfcommon_populate_rx_gain_vs_freq_v2_nv(&(rfnv_tdscdma_b39_rx_tbl.rx_band_cal_data),
                                               rfnv_item_ptr,
                                               RF_TDSCDMA_MAX_LNA_GAIN_STATES);
  break;

  default:
    MSG_ERROR( "Unsupported RF NV item identifier: %d", item_code, 0,0 );

  } /* switch ( item_code )  */

} /* end of rfnv_tdscdma_b39_copy_item */

void rfnv_tdscdma_b39_copy_tx_item
(
rfnv_item_id_enum_type  item_code,    /* Item to put */
rfnv_item_type          *rfnv_item_ptr     /* Pointer where to get the item */
)
{
	uint8 i = 0;
  /* read RF NV item based on its identifier/item code */
  switch ( item_code )
  {
  case RFNV_TDSCDMA_B39_TX_CAL_CHAN_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_cal_chan[0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_cal_chan),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   /* Load the Tx cal channel list into common Tx lin temp comp 
      data for use with characterized lin vs temp vs freq NVs
      Note: the NV is called CAL_CHAN, but actually used for TX_CHAR */
   rfnv_tdscdma_load_lin_vs_temp_tx_char_chan( &rfnv_tdscdma_b39_tx_tbl );
   break;

  case RFNV_TDSCDMA_B39_MAX_TX_POWER_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.max_tx_power,sizeof(rfnv_tdscdma_b39_tx_tbl.max_tx_power),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   if (rfnv_tdscdma_b39_tx_tbl.max_tx_power==230)
   {  rfnv_tdscdma_b39_tx_tbl.max_tx_power=930;  }
   break;

  case RFNV_TDSCDMA_B39_TX_LIN_VS_TEMP_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_lin_vs_temp,sizeof(rfnv_tdscdma_b39_tx_tbl.tx_lin_vs_temp),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;
  
  case RFNV_TDSCDMA_B39_TX_LIM_VS_TEMP_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_limit_vs_temp[0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_limit_vs_temp),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_PA_RANGE_MAP_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.pa_range_map[0],sizeof(rfnv_tdscdma_b39_tx_tbl.pa_range_map),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   for (i=0;i<sizeof(rfnv_tdscdma_b39_tx_tbl.pa_range_map)/sizeof(rfnv_tdscdma_b39_tx_tbl.pa_range_map[0]);i++)
   {
     rfnv_tdscdma_b39_tx_tbl.pa_static.pa_range_map[i] = (int16)rfnv_tdscdma_b39_tx_tbl.pa_range_map[i];
   }
   Old_NV_Used_B39 = TRUE;
   break;

  case RFNV_TDSCDMA_B39_PA_RISE_FALL_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.pa_rise_fall[0],sizeof(rfnv_tdscdma_b39_tx_tbl.pa_rise_fall),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   for (i=0;i<sizeof(rfnv_tdscdma_b39_tx_tbl.pa_rise_fall)/sizeof(rfnv_tdscdma_b39_tx_tbl.pa_rise_fall[0]);i++)
   {
     rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[i+1] = rfnv_tdscdma_b39_tx_tbl.pa_rise_fall[i].fall_threshold;
     rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[i+1] = rfnv_tdscdma_b39_tx_tbl.pa_rise_fall[i].rise_threshold;
   }
   break;

  case RFNV_TDSCDMA_B39_AGC_PA_ON_RISE_FALL_DELAY_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.agc_pa_on_rise_fall_delay,sizeof(rfnv_tdscdma_b39_tx_tbl.agc_pa_on_rise_fall_delay),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_AGC_TX_ON_RISE_FALL_DELAY_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.agc_tx_on_rise_fall_delay,sizeof(rfnv_tdscdma_b39_tx_tbl.agc_tx_on_rise_fall_delay),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_AGC_UPDATE_TX_AGC_TIME_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.agc_update_tx_agc_time,sizeof(rfnv_tdscdma_b39_tx_tbl.agc_update_tx_agc_time),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_PA_GAIN_UP_DOWN_TIME_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.pa_gain_up_down_time,sizeof(rfnv_tdscdma_b39_tx_tbl.pa_gain_up_down_time),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_ROT_ANGLE_PA_STATE_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.pa_static.tx_rot_angle[0],sizeof(rfnv_tdscdma_b39_tx_tbl.pa_static.tx_rot_angle),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_PA_COMPENSATE_UP_DOWN_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.pa_compensate_up_down[0],sizeof(rfnv_tdscdma_b39_tx_tbl.pa_compensate_up_down),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   for (i=0;i<sizeof(rfnv_tdscdma_b39_tx_tbl.pa_compensate_up_down)/sizeof(rfnv_tdscdma_b39_tx_tbl.pa_compensate_up_down[0]);i++)
   {
     /* RFLM side will process this from index 1 , but we hope to keep legacy PA compensate NV from index 0 to avoid confusion*/
     rfnv_tdscdma_b39_tx_tbl.pa_static.pa_comp_up[i+1] = rfnv_tdscdma_b39_tx_tbl.pa_compensate_up_down[i].up_comp;
     rfnv_tdscdma_b39_tx_tbl.pa_static.pa_comp_dn[i+1] = rfnv_tdscdma_b39_tx_tbl.pa_compensate_up_down[i].down_comp;
   }
   break;

  case RFNV_TDSCDMA_B39_TIMER_HYSTERISIS_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.timer_hysterisis,sizeof(rfnv_tdscdma_b39_tx_tbl.timer_hysterisis),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_MPR_BASED_PA_SWITCHPOINTS_SHIFT_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.mpr_pa_switchpoints_shift[0],sizeof(rfnv_tdscdma_b39_tx_tbl.mpr_pa_switchpoints_shift),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_MPR_VAL_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.mpr_backoff_val[0],sizeof(rfnv_tdscdma_b39_tx_tbl.mpr_backoff_val),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_MPR_VAL_V2_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.mpr_backoff_val_v2[0],sizeof(rfnv_tdscdma_b39_tx_tbl.mpr_backoff_val_v2),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;    
   
  case RFNV_TDSCDMA_B39_PA_MPR_BACKOFF_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.mpr_analog_backoff[0],sizeof(rfnv_tdscdma_b39_tx_tbl.mpr_analog_backoff),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_GAIN_SHIFT_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.mpr_digital_backoff[0],sizeof(rfnv_tdscdma_b39_tx_tbl.mpr_digital_backoff),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;


  case RFNV_TDSCDMA_B39_TX_LIN_MASTER_0_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_master[0][0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_master[0]),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_GAIN_INDEX_LIN_0_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_gain_idx[0][0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_gain_idx[0]),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_LIN_MASTER_1_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_master[1][0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_master[1]),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_GAIN_INDEX_LIN_1_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_gain_idx[1][0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_gain_idx[1]),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_LIN_MASTER_2_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_master[2][0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_master[2]),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_GAIN_INDEX_LIN_2_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_gain_idx[2][0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_gain_idx[2]),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_LIN_MASTER_3_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_master[3][0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_master[3]),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_GAIN_INDEX_LIN_3_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_gain_idx[3][0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_linearizer.tx_lin_gain_idx[3]),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_EXP_HDET_VS_AGC_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.exp_hdet_vs_agc[0],sizeof(rfnv_tdscdma_b39_tx_tbl.exp_hdet_vs_agc),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_COMP_VS_FREQ_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_comp_vs_freq,sizeof(rfnv_tdscdma_b39_tx_tbl.tx_comp_vs_freq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_LIM_VS_FREQ_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.tx_limit_vs_freq[0],sizeof(rfnv_tdscdma_b39_tx_tbl.tx_limit_vs_freq),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_ANT_TUNER_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.ant_tuner_tx,sizeof(rfnv_tdscdma_b39_tx_tbl.ant_tuner_tx),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_EPT_DPD_CONFIG_PARAMS_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.ept_dpd_cfg_params[0],sizeof(rfnv_tdscdma_b39_tx_tbl.ept_dpd_cfg_params),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   break;

  case RFNV_TDSCDMA_B39_TX_LIN_VS_TEMP_VS_FREQ_P_IN_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.p_in_comp,sizeof(rfnv_tdscdma_b39_tx_tbl.p_in_comp),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   /* Load the Lin vs Temp vs Freq Pin data into the common Tx Lin NV
      data structure */
   rfnv_tdscdma_load_lin_vs_temp_vs_freq( &rfnv_tdscdma_b39_tx_tbl, 
                                          rfnv_item_ptr, 
                                          TRUE );
   break;

  case RFNV_TDSCDMA_B39_TX_LIN_VS_TEMP_VS_FREQ_P_OUT_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.p_out_comp,sizeof(rfnv_tdscdma_b39_tx_tbl.p_out_comp),(void*)rfnv_item_ptr,sizeof(rfnv_item_type));
   /* Load the Lin vs Temp vs Freq Pout data into the common Tx Lin NV
      data structure */
   rfnv_tdscdma_load_lin_vs_temp_vs_freq( &rfnv_tdscdma_b39_tx_tbl, 
                                          rfnv_item_ptr, 
                                          FALSE );
   break;
   
  case RFNV_TDSCDMA_C0_B39_ANT_TUNER_CL_I:
   /* Load the Atuner CL Ctrl info */
   if(rfcommon_load_ant_tuner_cl_ctrl_info_from_nv(rfnv_item_ptr,
     	                                          &rfnv_tdscdma_b39_tx_tbl.ant_tuner_cl) == FALSE)
   {
     MSG_ERROR( "rfcommon_load_ant_tuner_cl_ctrl_info_from_nv() failed for the item code %d", item_code, 0,0 );
   }
   break;

  case RFNV_TDSCDMA_B39_TX_XPT_DPD_SCALING_I:
   rfcommon_xpt_load_dpd_scaling_nv(RFNV_TDSCDMA_B39_TX_XPT_DPD_SCALING_I,
                                    &rfnv_tdscdma_b39_tx_tbl.dpd_scaling_data,
                                    NULL, 0, NULL);
   break;

  case RFNV_TDSCDMA_C0_B39_PA_STATIC_I:
   memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.pa_static, sizeof(rfnv_tdscdma_b39_tx_tbl.pa_static),(rfcommon_nv_cmn_pa_static_type*)rfnv_item_ptr, sizeof(rfnv_item_type));
   rfnv_tdscdma_load_lin_vs_temp_vs_freq_from_pa_static(&rfnv_tdscdma_b39_tx_tbl);
   New_NV_Used_B39 = TRUE;
   break;

  case RFNV_TDSCDMA_C0_B39_PA_STATIC_Z_I:
  {
    rfnv_item_type    *nv_dc_data_ptr = NULL;
    uint32             nv_read_size = sizeof(rfnv_item_type);
    uint32             uncompress_size = sizeof(rfnv_item_type);

    /* NV decompressino status */
    int8 uncompress_status = Z_ERRNO;

    /* This is the PA static compressed NV item, it should be decompressed before copy to the pa_static NV data structure */
    /* Allocate memory for NV data decompression. Allocate memory with nv_item_type for PA static data, and using PA static data type (smaller) causes a problem */
    nv_dc_data_ptr = (rfnv_item_type *)modem_mem_alloc(sizeof(rfnv_item_type), MODEM_MEM_CLIENT_RFA);

    if (nv_dc_data_ptr != NULL)
    {
      /* Decompress NV data */
      uncompress_status = uncompress((uint8 *)nv_dc_data_ptr,   
                                     &uncompress_size,
                                     (const uint8 *)rfnv_item_ptr, 
                                     (uint32)nv_read_size);

      if (uncompress_status != Z_OK)
      {
        RF_MSG_2( RF_ERROR, "decompression of TDSCDMA PA static compressed NV id# %d failed!status:%d", item_code, uncompress_status ); 
        /* Free the memory for decompressed NV data */
        modem_mem_free((void*)nv_dc_data_ptr, MODEM_MEM_CLIENT_RFA);
      }
      else
      {
        RF_MSG_2( MSG_LEGACY_LOW, "rfnv_tdscdma_b39_copy_tx_item: PA static NV id# %d uncompressed successful, size = %d", item_code, uncompress_size );
        memscpy((void*)&rfnv_tdscdma_b39_tx_tbl.pa_static, sizeof(rfnv_tdscdma_b39_tx_tbl.pa_static),(rfcommon_nv_cmn_pa_static_type*)nv_dc_data_ptr, sizeof(rfnv_item_type));
        /* Free the memory for decompressed NV data */
        modem_mem_free((void*)nv_dc_data_ptr, MODEM_MEM_CLIENT_RFA);

        rfnv_tdscdma_load_lin_vs_temp_vs_freq_from_pa_static(&rfnv_tdscdma_b39_tx_tbl);
        New_NV_Used_B39 = TRUE;
      }
    } /* nv_dc_data_ptr */
    else
    {
       RF_MSG_1( RF_ERROR, "Fail to allocate memory for decompress PA static NV id# %d!", item_code); 
    } /* nv_dc_data_ptr - else */
  }
  break;

  default:
    MSG_ERROR( "Unsupported RF NV item identifier: %d", item_code, 0,0 );

  } /* switch ( item_code )  */

} /* end of rfnv_tdscdma_b39_copy_tx_item */

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the number of items in the NV list.

  @return uint16 Number of item in the NV list.

*/
uint16 rfnv_tdscdma_b39_rx_list_size_get( void )
{
    return( ARR_SIZE( rfnv_tdscdma_b39_rx_items_list ) );
}

uint16 rfnv_tdscdma_b39_tx_list_size_get( void )
{
    return( ARR_SIZE( rfnv_tdscdma_b39_tx_items_list ) );
}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the pointer to the NV table

  @return Pointer of rfnv_tdscdma_tbl_type

*/
void *rfnv_tdscdma_b39_rx_item_table_get( void )
{
    return( (void *) &rfnv_tdscdma_b39_rx_tbl );
}

void *rfnv_tdscdma_b39_tx_item_table_get( void )
{
    return( (void *) &rfnv_tdscdma_b39_tx_tbl );
}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the pointer to the NV table

  @return Flag of Mixed_NV

*/
boolean rfnv_tdscdma_b39_tx_get_mixed_nv_flag( void )
{
    if ((New_NV_Used_B39 == TRUE)&&(Old_NV_Used_B39 == TRUE))
    {
    	return TRUE;
    }
    else
    {	
        return FALSE;
    }
}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the flag of Mixed_NV

  @return flag of New NV

*/
boolean rfnv_tdscdma_b39_tx_get_new_nv_flag( void )
{
    return New_NV_Used_B39;
}

/*--------------------------------------------------------------------------*/
/*!
  @brief

  @details
  This function returns the PA switchpoints 

*/
boolean rfnv_tdscdma_b39_tx_get_pa_swpt(int16* rise_swpt, int16* fall_swpt, int16* active_pa_states)
{
   boolean status = FALSE;
   if (rfnv_tdscdma_b39_tx_get_new_nv_flag())
   {
      memscpy((void*)fall_swpt,
                 sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE,
                 &rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw,
                 sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE);
      memscpy((void*)rise_swpt,
                 sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE,
                 &rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw,
                 sizeof(int16)*RFCOMMON_NV_MAX_PA_STATE);
      /* For lowest PA state, rise and fall are min power -70 dBm */
      *rise_swpt = -700;
      *fall_swpt = -700;
      *active_pa_states = rfnv_tdscdma_b39_tx_tbl.pa_static.header[1];
      status = TRUE;
      #if 0
      MSG_8(MSG_SSID_RF, MSG_LEGACY_HIGH,"Rise, %d, %d, %d, %d, %d, %d, %d, %d", 
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[0],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[1],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[2],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[3],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[4],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[5],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[6],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_rise_sw[7]
                 );
      MSG_8(MSG_SSID_RF, MSG_LEGACY_HIGH,"Fall, %d, %d, %d, %d, %d, %d, %d, %d", 
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[0],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[1],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[2],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[3],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[4],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[5],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[6],
                 rfnv_tdscdma_b39_tx_tbl.pa_static.pa_fall_sw[7]
                 );
      #endif 
   }
   else
   {
      status = FALSE;
   }

   return status;
} /* rfnv_tdscdma_b39_tx_get_pa_swpt */
