#ifndef RFCOMMON_NV_MM_H
#define RFCOMMON_NV_MM_H
/*!
   @file
   rfcommon_nv_mm.h

   @brief

   @details
   
*/

/*===========================================================================
Copyright (c) 1999 - 2015  by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:52 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/nv/inc/rfcommon_nv_mm.h#1 $

when       who    what, where, why
--------   ---    ---------------------------------------------------------- 
11/17/15   jr     NV support to enable/disable LPM feature
05/07/14   npi    Enable notch threshold NV support
01/16/14   ndb    Added RFNV_RFC_DISABLE_FAILURE_FOR_NONFATAL_DEVICES_I
11/05/13   adk    Added support for extensions to NV item RFNV_ATUNER_ALGO_TYPE
07/15/13   ndb    Added Tuner Algo Type NV 
05/21/13   jr     Added notch filter support for TDSCDMA
03/14/13   sar    Fixed compiler errors in APQ target. 
03/04/16   zhw    Clean up unused SPI NV items
02/19/13   sbm    Bandedge max power backoff support in LTE(Nikel CR375880)
08/01/12   gh     Added NV support for k sensor cal
03/22/12   id     Added NV support for Tx Internal Device Cal
02/24/12   shb    Added new NV support for HDET autocal
02/16/12   ac     added support for notch for wcdma
02/15/12   sar    Removed FEATURE_ENABLE_TEST_NV_ITEMS and FEATURE_RF_AFC.
02/13/12   sar    Feature reduction changes.
01/17/12   pl     Added support for LTE spurious table
10/20/11   adk    Added support for SV diversity.
09/26/11   aro    Removed old RC tuner NV structure
09/19/11   aro    Added enum for number of RC Tuner calibration data
09/19/11   aro    Added C++ protection
09/18/11   aro    Added generic RC Tuner calibration data structure to
                  remove RTR8600 specific data structure
09/14/11   can    Merge MDM9K tips to NikeL.
09/14/11   aro    Added new RC tuner and HDET calibration data containers
09/13/11   aro    Added support to load RC tuner NV for second RTR
09/08/11   vb     Support for DAC Cal
08/12/11   jhe    Added support for TDS BC config
05/26/11   can    HDET FAST Correction tolerance NV.   
05/24/11   cd     Added support for SV path BC configuration
12/23/10   cri    Added support for initial and dac modes for SPI core
11/11/10   cri    Added TXPL debug message support
06/24/10   can    Support for LTE BC Config.
12/23/09   bn     PA MPR NV feature support
10/03/09   ra     support storage of rf asic autocal variable
08/11/09   shb    Added R/C process errors from RC Tuner calibration to common
                  NV Table
04/02/08   sr     added rfcommon_nv_get_rf_bc_config() prototype.
10/14/08   sr     Initial version.

============================================================================*/

#include "rfa_variation.h"
#include "comdef.h"
#include "nv.h"
#include "customer.h"
#include "rfcommon_core_types.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "rfcommon_msm_cal.h"
#endif
#include "rfcommon_nv.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RFNV_MPR_BACKOFF_TBL_SIZE 6
#define RFCOMMON_SPI_ENABLE              0
#define RFCOMMON_SPI_INIT_VALUE          1
#define RFCOMMON_SPI_SHUTDOWN_VALUE      2
#define RFCOMMON_MAX_RC_TUNER_CAL_ITEMS  16
#define RFNV_K_SENSOR_K0_VAL_I RFNV_SPARE_2_I

typedef struct
{
    uint8 hdet_fast_correction_enable;
    uint8 hdet_fast_correction_tolerance_db10;
} rfcommon_nv_mm_hdet_fast_correction_type;

/*! Structure that will store HDET autocal NV */
typedef struct
{
  /*! TX device 0 HDET autocal result */
  uint8 dev0;
  /*! TX device 1 HDET autocal result */
  uint8 dev1;
} rfcommon_nv_hdet_autocal_type;

/*! Structure that will store LTE GNSS Blanking delay NV */
typedef struct
{
  /*! GNSS Blanking Rise delay */
  uint16 rise_delay;
  /*! GNSS Blanking Fall delay */
  uint16 fall_delay;
}rfcommon_nv_gnss_blanking_rise_fall_delta_type;

typedef struct
{
    /* Bit mask of CDMA Band Classes for different devices  */
    uint64 rf_cdma_bc_tx_int_cal[8];
    /* Bit mask of GSM Band Classes for different devices  */
    uint8 rf_gsm_bc_tx_int_cal[8];
    /* Bit mask of WCDMA Band Classes for different devices  */
    uint64 rf_wcdma_bc_tx_int_cal[8];
    /* Bit mask of LTE Band Classes for different devices  */
    uint64 rf_lte_bc_tx_int_cal[8];
    /* Bit mask of TDSCDMA Band Classes for different devices  */
    uint64 rf_tdscdma_bc_tx_int_cal[8];
} rfcommon_nv_mm_int_dev_cal_rfmode_list;

typedef struct
{

  /* The RF configuration as read from the NV Services */
  uint8 rf_config;
  uint8 rf_hw_config;
  uint64 rf_bc_config[4];
  uint8 hdet_off;
  uint64 rf_lte_bc_config;
  boolean rf_lte_bandedge_maxpower_relaxation_enable;
 
#ifdef FEATURE_TDSCDMA
  uint64 rf_tds_bc_config[2];
  rfcommon_spur_table_type rf_tdscdma_c0_spurious_table;
  rfcommon_spur_table_type rf_tdscdma_c1_spurious_table;
#endif
  uint8  rf_lte_enable_txpl_debug_msg;
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
  rfcommon_msm_cal_result_type rf_msm_cal_result;
#endif
  uint8 rc_process_error[RFCOMMON_MAX_RC_TUNER_CAL_ITEMS];
  rfcommon_nv_mm_hdet_fast_correction_type rf_lte_hdet_fast_correction;
  rfcommon_spur_table_type rf_lte_c0_spurious_table;
  rfcommon_spur_table_type rf_lte_c1_spurious_table;
  rfcommon_spur_table_type rf_lte_c2_spurious_table;
  rfcommon_spur_table_type rf_lte_c3_spurious_table;
  rfcommon_spur_table_type rf_wcdma_c0_spurious_table;
  rfcommon_spur_table_type rf_wcdma_c1_spurious_table;
  /*! RF device HDET autocal results */
  rfcommon_nv_hdet_autocal_type hdet_autocal;
  rfcommon_nv_gnss_blanking_rise_fall_delta_type lte_gnss_blanking_rise_fall_delta;
  rfcommon_nv_mm_int_dev_cal_rfmode_list rf_int_dev_cal_rfmode_list;
  rfcommon_nv_mm_int_dev_cal_rfmode_list rf_int_dev_cal_status_rfmode_list;
  uint16 k_sensor_k0_val;
  /*The type of Algorithm that needs to be enabled in the tuner */ 
  uint8 rf_atuner_algo_type;
  /*Flag to disable the failures for non-fatal RF devices */ 
  uint8 rfc_disable_failure_nonfatal_devices;

  /*! LTE Spur threshold tables */
  rfcommon_spur_thresh_table_type rf_lte_c0_spurs_thresh_table;
  rfcommon_spur_thresh_table_type rf_lte_c1_spurs_thresh_table;
  rfcommon_spur_thresh_table_type rf_lte_c2_spurs_thresh_table;
  rfcommon_spur_thresh_table_type rf_lte_c3_spurs_thresh_table;

  uint16 rf_nv_control_lpm_feature;

 } rfcommon_nv_tbl_type;
 
/* pointer to CMN NV table */
extern rfcommon_nv_tbl_type *rfcommon_nv_tbl_ptr;
extern rfcommon_nv_tbl_type  rfcommon_nv_tbl;

typedef struct
{
  /* min and max value for VBATT ADC reading */
  uint8 vbatt[2];

  /* till here */


  /* ----------------------------------------------------------
    Frequency adjustment variables.
  ---------------------------------------------------------- */   



  /* min and max value for THERM ADC reading */
  uint8 therm[2];

  uint16 trk_lo_adj_default;

  uint16 trk_lo_adj_slope_default;

  uint8 rfr_bb_filter;

  uint8 rtr_bb_filter;

  uint8 rfr_iq_line_resistor;

  #ifdef FEATURE_WCDMA_MIN_INIT_TX_PWR
  int16 min_tx_power;
  #endif

  #ifdef RF_HAS_RX_DIVERSITY
  /* diversity mode select switch values for GPS */
  uint8    rf_gps_drx_mode_sel[3];
  #endif /* RF_HAS_RX_DIVERSITY */

  /* PMIC configuration setting for RF */
  uint8 rf_pm_config;  

  /*RF ENH THERM READ*/
  uint32 rf_enh_therm_max;
  uint32 rf_enh_therm_min;

  /* MPR based PA switchpoints backoffs */
  uint16 wcdma_rel7_pa_mpr_backoff[RFNV_MPR_BACKOFF_TBL_SIZE];

} rfcommon_nv_mm_tbl_type;

/* pointer to CMN NV table */
extern rfcommon_nv_mm_tbl_type   *rfcommon_nv_mm_tbl_ptr;
extern rfcommon_nv_mm_tbl_type rfcommon_nv_mm_tbl;

/*===========================================================================

FUNCTION rfcommon_nv_get_tbl_ptr

DESCRIPTION
  Returns pointer to Common MM RF NV table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to common MM RF NV table.

SIDE EFFECTS
  None

===========================================================================*/
rfcommon_nv_tbl_type *rfcommon_nv_get_tbl_ptr ( void );

/*===========================================================================

FUNCTION RFNV_GET_CAL_STAT

DESCRIPTION
  Returns the Calibration Status.

DEPENDENCIES
  The function rfnv_load_nv_items must have been called before this function.

RETURN VALUE
  TRUE if nv is read successfully, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean rfcommon_nv_get_cal_stat(void);

/*===========================================================================

FUNCTION rfcommon_nv_get_rf_config

DESCRIPTION
  This function returns the RF Configuration flag value.

DEPENDENCIES
  None

RETURN VALUE
  NV_RF_CONFIG id.

SIDE EFFECTS
  None
  
===========================================================================*/
rf_hw_type rfcommon_nv_get_rf_config(void);

/*===========================================================================

FUNCTION rfcommon_nv_get_rf_bc_config

DESCRIPTION
  This function returns the Band Configuration value.

DEPENDENCIES
  None

RETURN VALUE
  NV_RF_CONFIG id.

SIDE EFFECTS
  None
  
===========================================================================*/

uint64 rfcommon_nv_get_rf_bc_config(rf_path_enum_type path);
uint64 rfcommon_nv_get_rf_lte_bc_config(void);
uint64 rfcommon_nv_get_rf_tds_bc_config(rf_path_enum_type path);
rfcommon_nv_mm_int_dev_cal_rfmode_list* rfcommon_nv_get_tx_int_dev_bc_config(void);
uint32 rfcommon_nv_get_rf_atuner_algo( void);
uint32 rfcommon_nv_get_rfc_disable_failure_nonfatal_devices_flag(void);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the auto cal value for rf asic HDET.

  @details

  @retval
  NV_WCDMA_HDET_OFF_I id.... temporary use of this enum
*/
uint8 rfcommon_nv_get_rf_asic_hdet_autocal(void);

/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function sets the auto cal value for rf asic HDET to 
  the common nv table ptr

  @details
  

  @retval
  none
*/
void rfcommon_nv_set_rf_asic_hdet_autocal(uint8 hdet_autocal);

/*----------------------------------------------------------------------------*/
/*!
  @brief
    Returns the value of RFNV_SPARE_1_I , which is used to indicate if LPM feature
     flag is set 

  @retval
    TRUE - If LPM feature is enabled
    FALSE - If it is not enabled
*/

boolean rfcommon_nv_get_lpm_feature_enabled(void );


#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_NV_MM_H */

