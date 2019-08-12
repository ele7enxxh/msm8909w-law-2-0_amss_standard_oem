/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          C-GPS External Source file

GENERAL DESCRIPTION
  This module contains the interface functions exported outside the library

EXTERNALIZED FUNCTIONS
  cgpd_XoSupported


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2007 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2015 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015, 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

Version Control

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/mgp/me/gen8/src/cgps_ext.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/23/09   jd      Use new GNSS RF APIs.
06/17/09   jd      Added FEATURE_TRM_API_V3 - GPS support for new TRM APIs 
                   done for SVDO and PAM
09/20/07   va      Create wrapper for clk_regime_cgps_freq_offset_ppt
09/07/07   va      Initial implementation.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*
 * Include files
*/
#include "aries_os_api.h"
#include "gps_variation.h"
#include "cgps_api.h"
#include "gnss_api.h"
#include "pmapp_npa.h"
#ifndef FEATURE_GNSS_NAV_HW_VER5
#include "clkregim.h"
#endif
#ifdef FEATURE_XO_TASK
#include "tcxomgr_task.h"
#endif /* FEATURE_XO_TASK */
#if defined(FEATURE_BATTERY_CHARGER)
#include "charger.h"
#endif

/*
 * Constant definitions
*/

/*
 * Structure definitions
*/

/*
 * Local data declarations.
*/


/*
 * Local function prototypes
*/

/*=============================================================================

       Variables

=============================================================================*/



#ifndef FEATURE_GNSS_DYNAMIC_SP_MEM_ALLOCATION

#ifdef FEATURE_GPSONE_LOMEM
  #ifdef FEATURE_GPSONE_HIMEM
    #error FEATURE_GPSONE_LOMEM and FEATURE_GPSONE_HIMEM both defined.
  #endif
#endif

#if defined(FEATURE_GNSS_BUP_BURST_16BYTE)
/* Extra memory to support BUP BURST:
 * 1. 118 tasks * (12+12) extra bytes in GaccSliceOut structure * 2 [IMEM+EMEM] = 5664
 * 2. BUP_SAFETY_MARGIN - extran space between commands list (64*8)
 * 3. Support GAL CD modultion each of 12 buffers has size increased from 320 to (320*4),
 *    (320*4- 320)*12 = 11520 
 */ 
 #define BUP_BURST_MEM_SIZE   (5664 + 512 + 11520)
#else
  #define BUP_BURST_MEM_SIZE  0
#endif

/*  GACC FIFO size increase */
/* To increase FIFO size by 20 msec we need
 * (64 hyp * 2(1I,1Q)*20 msec * 118 tasks) or ~148KB*/
#define GACC_FIFO_INCREASE  (64*2*20*118)

/* Signal processing memory to be used by CC.
 * This is allocated here since the size depends on FEATURE_GPSONE_HIMEM.
 * The objective is to deliver a single library regardless of the feature
 * definitions. The buffer size must be larger than meq_GaccGridBufSizeBytes.
*/
#ifdef FEATURE_GPSONE_HIMEM
/* 2312752 - signal processing memory for 118 tasks, sizeof(cc_ResMgrExtHiMemLayoutType)
   for other numbers see below.
*/
  #ifdef FEATURE_TARGET_LPDDR4
  /* Adding 118*20*2*64=302080 bytes for 8994 to increase GACC Corr Buffer size * from 40ms to 60ms */
   uint8 meu_SigProcMem[2312752  + 92228 + 73816 + 752 + 512 + 302080 + BUP_BURST_MEM_SIZE + GACC_FIFO_INCREASE] __attribute__ ((aligned (32)));
#else
// uint8 meu_SigProcMem[2312752  + 92228 + 73816 + 752 + 512 + BUP_BURST_MEM_SIZE] __attribute__((aligned(32)));
   uint8 meu_SigProcMem[2312752  + 92228 + 73816 + 752 + 512 + BUP_BURST_MEM_SIZE + GACC_FIFO_INCREASE] __attribute__((aligned(32))); // 10 msec extra for GACC_FIFO
#endif /* FEATURE_TARGET_LPDDR4 */

   boolean mcu_HiMemFeature = TRUE;
#else /* FEATURE_GPSONE_HIMEM */

/* 1322032 - signal processing memory for 118 tasks, sizeof(cc_ResMgrExtLoMemLayoutType)
    +92228 - CP command queue, CC_CP_COMMAND_BUFFER_SIZE from cc_data.h
             This used to be in cc_task but moved here for relocatable
             modem feature to allow us to assume that ALL memory
             needed by CC for DM commands are in one continguous
             block so that we can use the SAME offset when
             converting from Virt To Phy address.
    +73816 - GACC command queue, CC_GACC_COMMAND_BUFFER_SIZE from cc_data.h
             This used to be in cc_task but moved here for relocatable
             modem feature to allow us to assume that ALL memory
             needed by CC for DM commands are in one continguous
             block so that we can use the SAME offset when
             converting from Virt To Phy address.
      +752 - GTM can only be accessed at a word (32bit) boundary.
             As a result fields of data structures being stored in
             GTM had to be aligned. This increase in memory reflects
             the alignment.
      +512 - Safety/Aligment rounding margin.
	  (64*2*10)  - incerase GACC FIFO by 10 msec.
*/
  #ifdef FEATURE_TARGET_LPDDR4
  /* Adding 106*20*2*64=302080 bytes for 8994 to increase GACC Corr Buffer size
    * from 40ms to 60ms */
   uint8 meu_SigProcMem[1322032 + 92228 + 73816 + 752 + 512 + 302080 + BUP_BURST_MEM_SIZE + GACC_FIFO_INCREASE] __attribute__((aligned(32)));
#else
   // uint8 meu_SigProcMem[1322032 + 92228 + 73816 + 752 + 512 + BUP_BURST_MEM_SIZE] __attribute__((aligned(32)));
   uint8 meu_SigProcMem[1322032 + 92228 + 73816 + 752 + 512 + BUP_BURST_MEM_SIZE + GACC_FIFO_INCREASE] __attribute__((aligned(32))); // 10 msec extra for GACC FIFO
#endif /* FEATURE_TARGET_LPDDR4 */
   boolean mcu_HiMemFeature = FALSE;

#endif /* ! FEATURE_GPSONE_HIMEM */

#else /* FEATURE_GNSS_DYNAMIC_SP_MEM_ALLOCATION */

   uint8 meu_SigProcMem[512] __attribute__ ((aligned (32)));

   boolean mcu_HiMemFeature = FALSE;
   

#endif /*FEATURE_STATIC_SP_MEM_ALLOCATION*/


/*****************************************************************************
 * API functions for C-GPS capabilities
 *****************************************************************************
*/

/*
 ******************************************************************************
 * Function cgps_XOSupported
 *
 * Description:
 *
 *  This function is a peek on C-GPS capability for XO Support
 *
 * Parameters:
 *
 *  None.
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  TRUE if XO is supported
 *  FALSE if XO is not supported and VCTCXO is supported.
 *
 ******************************************************************************
*/
boolean cgps_XoSupported( void )
{
  #ifdef FEATURE_XO
  return TRUE;
  #else
  return FALSE;
  #endif
}

/*
 ******************************************************************************
 * Function cgps_ClkRegimeFreqOffsetPpt
 *
 * Description:
 *
 *  This function calls the appropriate clk fuction for QSC targets,
 *  empty function otherwise.
 *
 * Parameters:
 *
 *  l_Offset - Freq Offset in ppt ( parts per trillion )
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
 ******************************************************************************
*/
void cgps_ClkRegimeFreqOffsetPpt(int32 l_Offset)
{
  #if defined(T_QSC60X5) || defined(T_QSC6270)
  (void)clk_regime_cgps_freq_offset_ppt(l_Offset);
  #endif
}

/*
 ******************************************************************************
 * Function cgps_SigProcMemSize
 *
 * Description:
 *
 * This function performs check on the buffer size of the signal processing
 * memory meu_SigProcMem[].
 *
 *
 * Parameters:
 *
 *  None
 *
 * Dependencies:
 *
 *  None
 *
 * Return value:
 *
 *  Size of the the buffer meu_SigProcMem.
 *
 ******************************************************************************
*/
uint32 cgps_SigProcMemSize( void )
{
  return sizeof( meu_SigProcMem );
}

#ifndef FEATURE_GNSS_TCXOMGR
/*
 ******************************************************************************
 * Function cgps_XoEstimationState
 *
 * Description:
 *
 *  This function starts or stops XO frequency estimation loop
 *
 * Parameters: 
 *
 *  u_Enable : TRUE  : set start frequency estimation sig
 *             FALSE : set stop frequenmcy estimation sig
 *
 * Dependencies:
 * 
 *  None
 *
 * Return value: 
 *
 *  None
 *
 ******************************************************************************
*/
void cgps_XoEstimationState( boolean u_Enable )
{
  #ifdef FEATURE_XO_TASK
  if (cgps_XoSupported())
  {
    tcxomgr_set_sigs(u_Enable ? TCXOMGR_START_UPDATE_TIMER_SIG :
                     TCXOMGR_STOP_UPDATE_TIMER_SIG);
  }
  #endif /* FEATURE_XO_TASK */
}
#endif /* ! FEATURE_GNSS_TCXOMGR */

/*
 ******************************************************************************
 *
 * cgps_SetExternalChargerStatus( void )
 *
 * Function description:
 *  This function sets a static indication of the external charger status.
 *  The Charger status may be used by other functions to enable / disable functions
 *  (for example, the receiver may be configured to disable low power mode of operation
 *  when it is connected to charger to give better accuracy)
 *
 * Parameters: 
 *  e_ExtChargerStatus -- Indicates whether the receiver is connected to a charger
 * 
 * Return value: None
 *  
 *
 ******************************************************************************
*/

/* Static variable to store the external charger status */
static cgps_BatteryChargerStatusEnumType cgpse_ExtChargerStatus = CGPS_BATTERY_CHARGER_STATE_UNKNOWN;

void cgps_SetExternalChargerStatus( cgps_BatteryChargerStatusEnumType e_ExtChargerStatus )
{
  cgpse_ExtChargerStatus = e_ExtChargerStatus;
} /* end cgps_SetExternalChargerStatus() */

/*
 ******************************************************************************
 *
 * cgps_GetExternalChargerStatus( void )
 *
 * Function description:
 *  This function returns the value of external charger status.
 *
 * Parameters: 
 *  e_ExtChargerStatus -- Indicates whether the receiver is connected to a charger
 * 
 * Return value: None
 *  
 *
 ******************************************************************************
*/

static cgps_BatteryChargerStatusEnumType cgps_GetExternalChargerStatus( void  )
{
  return( cgpse_ExtChargerStatus );
} /* end cgps_GetExternalChargerStatus() */

/*
 ******************************************************************************
 *
 * cgps_GetChargerStatus( void )
 *
 * Function description:
 *  This function obtains the Battery Charger status. It checks whether a charger
 *  is connected or not, and if the phone is still charging or not. 
 *  Possible return values are
 *  CGPS_BATTERY_CHARGER_STATE_UNKNOWN 
 *  CGPS_BATTERY_CHARGER_NOT_CONNECTED
 *  CGPS_BATTERY_CHARGER_CONNECTED_NOT_CHARGED
 *  CGPS_BATTERY_CHARGER_CONNECTED_CHARGED
 *
 * Parameters: 
 *  
 * Return value: cgps_BatteryChargerStatus
 *  
 *
 ******************************************************************************
*/
cgps_BatteryChargerStatusEnumType cgps_GetChargerStatus( void )
{
  cgps_BatteryChargerStatusEnumType e_Status = CGPS_BATTERY_CHARGER_STATE_UNKNOWN;

#if defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_DAL)
  if( chg_is_charging() )
  {
    /* Batter Charger is connected. Now check it is is charging */
    if( CHG_UI_EVENT__DONE == chg_ui_event_read() ) 
    {
      /* Charger is connected and charging is complete */
      e_Status = CGPS_BATTERY_CHARGER_CONNECTED_CHARGED;
    }
    else
    {
      /* Charger is connected, but not completely charged */
      e_Status = CGPS_BATTERY_CHARGER_CONNECTED_NOT_CHARGED;
    }
  }
  else
  {
    /* Charger is not connected */
    e_Status = CGPS_BATTERY_CHARGER_NOT_CONNECTED;
  }
#endif /* defined(FEATURE_BATTERY_CHARGER) || defined(FEATURE_CHG_DAL) */
  
  /* If internal charger status is unknown, use external charger status
     This status may be provided by application or other custom charging API's 
  */
  if( CGPS_BATTERY_CHARGER_STATE_UNKNOWN == e_Status )
  {
    /* Get the charger status from an external source */
    e_Status = cgps_GetExternalChargerStatus();
  }

  return e_Status;

} /* end cgps_GetChargerStatus() */

/*
******************************************************************************
* gnss_UpdateWwanTxAntennaStatus
*
* Function description:
*
*  gnss_UpdateWwanTxAntennaStatus is used to update the status of which
*  TX Antenna is being used for WWAN transmissions. This antenna selection
*  control is needed to estimate the impact of the WLAN and WWAN Tx on GPS.
*  Different antennas provide different levels of isolation from the GPS band.
*  Based on the antenna used, GNSS receiver selects appropriate thresholds
*  for cross-correlation, or IMD technique to mitigate the impact.
*
*  This function should be called at start of a GPS session, and whenever the 
*  antenna selection is changed. 
*  The function updates a GNSS global variable to set the Tx Antenna status
*  and returns immediately without performing any further processing.
*  Note: If the device has only a single antenna for WWAN Tx, this API should 
*        be called with "WWAN_TX_ANTENNA_MAIN" as the antenna type.
*
* Parameters:
*
*  e_TxAntUsed - Indicates which Tx Antenna is selected.
*
* Return value:
*
*  void
*
******************************************************************************
*/

gnss_TxAntennaEnumType gnsse_TxAntUsed = WWAN_TX_ANTENNA_MAIN;

void gnss_UpdateWwanTxAntennaStatus( gnss_TxAntennaEnumType e_TxAntUsed )
{
  /* Update the Tx Antenna used */
  gnsse_TxAntUsed = e_TxAntUsed;
}

#ifdef FEATURE_NAVRF
/*
 ******************************************************************************
 *
 * gnss_SetExternalCtrlForGNSS
 *
 * Function description:
 *  This function provides an API by which customer can modify
 *  ELNA configuration.
 *
 * Parameters: 
 *  e_LnaCfg -- ELNA config that customer opts to use
 * 
 * Return value: None
 *  
 *
 ******************************************************************************
*/

/* Customer needs to change this variable in order to override NAVRF driver ELNA setting */
gnss_ExternalGNSSConfigStruct gnssz_ExtElnaCfg =
{ GNSS_RF_DEFAULT_CFG, C_GNSS_RF_ELNA_GPIO_NUM_DEFAULT, NULL, C_NAVRF_NPA_RESOURCE_NAME_DEFAULT, C_PMIC_NPA_MODE_ID_RF_MODE_DEFAULT}; 

void gnss_SetExternalCtrlForGNSS(gnss_ExternalGNSSConfigStruct z_GnssCfg)
{
  gnssz_ExtElnaCfg.e_ExtElnaCtrl = z_GnssCfg.e_ExtElnaCtrl;
  gnssz_ExtElnaCfg.u_GpioNumber = z_GnssCfg.u_GpioNumber;
  gnssz_ExtElnaCfg.p_ExternalLnaFunc = z_GnssCfg.p_ExternalLnaFunc;
  (void)GNSS_STRLCPY(gnssz_ExtElnaCfg.gnss_ExternalNpaResourceName, 
                     z_GnssCfg.gnss_ExternalNpaResourceName, 
                     C_NAVRF_MAX_NPA_RESOURCE_NAME_LEN_DEFAULT);
  gnssz_ExtElnaCfg.gnss_ExternalNpaActiveState = z_GnssCfg.gnss_ExternalNpaActiveState;
} 

/*
  ******************************************************************************
  *
  * gnss_GetExternalCtrlForGNSS
  *
  * Function description:
  *  This function is used to get ELNA config information. This will provide
  *  an interface by which driver knows if ELNA config information has been
  *  modified externally.
  *
  * Parameters:
  *   p_ElnaCfg
  *  
  * 
  * Return value:
  *   None
  *  
  *
  ******************************************************************************
*/

void gnss_GetExternalCtrlForGNSS(gnss_ExternalGNSSConfigStruct *p_GnssCfg)
{
  p_GnssCfg->e_ExtElnaCtrl = gnssz_ExtElnaCfg.e_ExtElnaCtrl;
  p_GnssCfg->u_GpioNumber = gnssz_ExtElnaCfg.u_GpioNumber;
  p_GnssCfg->p_ExternalLnaFunc = gnssz_ExtElnaCfg.p_ExternalLnaFunc;
  if(NULL != gnssz_ExtElnaCfg.gnss_ExternalNpaResourceName)
  {
  (void)GNSS_STRLCPY(p_GnssCfg->gnss_ExternalNpaResourceName, 
                     gnssz_ExtElnaCfg.gnss_ExternalNpaResourceName,
                     C_NAVRF_MAX_NPA_RESOURCE_NAME_LEN_DEFAULT);
}
  p_GnssCfg->gnss_ExternalNpaActiveState = gnssz_ExtElnaCfg.gnss_ExternalNpaActiveState;
}

#endif /* FEATURE_NAVRF */

