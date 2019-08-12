#ifndef WL1ULSARMGR_H
#define WL1ULSARMGR_H

/*============================================================================
                            L 1 UPLINK SAR MANAGER

DESCRIPTION
This files contains all necessary macros and definitions for SAR management.
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1ulsarmgr.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
12/04/14    ar      Fix to report correct UARFCN for SAN for additional channels in band
08/28/14    vr      Mainlining code under FEATURE_WCDMA_DIME_SW, FEATURE_WCDMA_DIME_TX_STATE_REPORTING,
                    FEATURE_TRITON_MODEM for BO.2.0.
01/24/14    pkg     Added changes to support co-ex Tx power filtering
11/27/13    ash     Fix Featurization error for SAR check-in
11/21/13    cjz     Added API to calculate UL channel number.
11/06/13    tsk     Ported SAR chnaged to Dime 3.0.
03/29/13    pkg     File created
===========================================================================*/
#include "wcdma_variation.h"
#include "wl1_msgr.h"

/* SAR feature set is a superset of DIME TX state reporting. */
#ifdef FEATURE_WCDMA_SAR_TX_PWR_FILTER

/****************************************************************************
****                  STRUCTURES AND ENUMERATIONS                       *****
*****************************************************************************/


/* Tx state indication */
typedef enum{
  WL1ULSARMGR_TX_STATE_OFF,
  WL1ULSARMGR_TX_STATE_ON
}wl1ulsarmgr_tx_state_enum_type;

typedef enum
{                                 /* Corresponding RF band types */
  WL1ULSAR_IMT2K_BAND,            /* SYS_BAND_WCDMA_I_IMT_200    */
  WL1ULSAR_PCS_BAND,              /* SYS_BAND_WCDMA_II_PCS_1900  */
  WL1ULSAR_WCDMA_1700_BAND,       /* SYS_BAND_WCDMA_IX_1700      */
  WL1ULSAR_WCDMA_850_BAND,        /* SYS_BAND_WCDMA_V_850        */
  WL1ULSAR_WCDMA_800_BAND,        /* SYS_BAND_WCDMA_VI_800       */
  WL1ULSAR_WCDMA_1800_BAND,       /* SYS_BAND_WCDMA_III_1700     */
  WL1ULSAR_WCDMA_900_BAND,        /* SYS_BAND_WCDMA_VIII_900     */
  WL1ULSAR_WCDMA_1700_2100_BAND,  /* SYS_BAND_WCDMA_IV_1700      */
  WL1ULSAR_WCDMA_1500_BAND,       /* SYS_BAND_WCDMA_XI_1500      */
  WL1ULSAR_WCDMA_BC19_BAND,       /* SYS_BAND_WCDMA_XIX_850      */
  WL1ULSAR_WCDMA_INVALID_BAND,    
}wl1ulsar_band_enum_type;

/****************************************************************************
****                        GLOBAL VARIABLES                            *****
*****************************************************************************/

/* wcdma SAR tx power timer period in ms */
extern uint8 wcdma_sar_tx_pwr_filt_tmr; 

#endif /* FEATURE_WCDMA_SAR_TX_PWR_FILTER */

#ifdef FEATURE_WCDMA_SAR_TX_PWR_FILTER
extern rex_timer_type wl1ulsar_tx_power_timer;
extern uint32 multisim_cxm_filt_val;

/****************************************************************************
****                        FUNCTION PROTOTYPES                         *****
*****************************************************************************/

/*===========================================================================
FUNCTION wl1ulsar_tx_power_filter

DESCRIPTION
  This function calculates the filtered Tx power by converting the slotwise
  TxAGC which is in dBm to linear scale. During CM and CPC - DTX FW
  propagates the previous slot TxAGC through out the gap. The gaps are
  identified and are replaced with linear power of value 0. The filtering
  is equivalent to an IIR filter with a time constant of 100ms. Scaling of
  filter constant and the slot values are done with minimal loss of precision. 

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void wl1ulsar_tx_power_filter(void);

/*===========================================================================
FUNCTION wl1ulsar_tx_power_report

DESCRIPTION
  This is the call back function after the sar tx power reporting timer expires.
  The filtered tx power is reported using MSGR interface to the MCS.

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void wl1ulsar_tx_power_report(uint32 unused);

/*===========================================================================
FUNCTION wl1ulsar_calc_linear_power_from_db10

DESCRIPTION
  This function calculates the linear power < 28 from the given dB10 value
  input from FW. 

DEPENDENCIES
  None

RETURN VALUE
  uint64 Linear value of the dB10.
 
SIDE EFFECTS
  None
===========================================================================*/
uint32 wl1ulsar_calc_linear_power_from_db10(int16 local_db10);

/*===========================================================================

FUNCTION wl1ulsar_nv_handle

DESCRIPTION
  Calls functions to create and read SAR related NV.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void wl1ulsar_nv_handle(void);


/*===========================================================================

FUNCTION wl1ulsar_create_nv

DESCRIPTION
  This function creates SAR power reporting timer NV items and sets to default values

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void wl1ulsar_create_nv(void);

/*===========================================================================

FUNCTION wl1ulsar_read_nv

DESCRIPTION
  This function reads the NV iteam and sets accordingly.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void wl1ulsar_read_nv(void);

#endif /* FEATURE_WCDMA_SAR_TX_PWR_FILTER */
/* SAR feature set is a superset of DIME TX state reporting. */
#ifdef FEATURE_WCDMA_SAR_TX_PWR_FILTER
/*===========================================================================

FUNCTION wl1ulsarmgr_cleanup

DESCRIPTION
  This function cleans up the vaiables and timers related to Wl1 uplink SAR
  manager during enc cleanup.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
 ===========================================================================*/
void wl1ulsarmgr_cleanup(void);

/*==================================================================================

FUNCTION          wl1ulsar_calculate_ul_uarfcn

DESCRIPTION       This function calculates the uplink frequency given the downlink 
                  frequency.

DEPENDENCIES

RETURN VALUE     Returns UL UARFCN for a DL UARFCN passed in the function argument. 
                 If the DL UARFCN is invalid MSG_FATAL occours.

SIDE EFFECTS     None.
=====================================================================================*/
uint16 wl1ulsar_calculate_ul_uarfcn(sys_band_class_e_type band, uint16 dl_uarfcn);

/*===========================================================================

FUNCTION        WL1ULSAR_GET_FREQUENCY_BAND

DESCRIPTION     This function looks at the passed-in frequency and returns 
                the band the frequency belongs to.

RETURN VALUE    Returns WL1ULSAR_WCDMA_INVALID_BAND if UARFCN is invalid.

SIDE EFFECTS    None
===========================================================================*/
wl1ulsar_band_enum_type wl1ulsar_get_frequency_band(uint16 freq);

/*===========================================================================
FUNCTION wl1ulsar_send_cxm_stx_state_update

DESCRIPTION
  This function sends the state update to the MCS depending upon the state
  of the transmitter. 

DEPENDENCIES
  None

RETURN VALUE
  None
 
SIDE EFFECTS
  None
===========================================================================*/
void wl1ulsar_send_cxm_stx_state_update(wl1ulsarmgr_tx_state_enum_type state);
#endif /* FEATURE_WCDMA_SAR_TX_PWR_FILTER */

#endif /* WL1ULSARMGR_H */