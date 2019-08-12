#ifndef RFCOM_H
#define RFCOM_H

/*===========================================================================


      R F  D r i v e r  C o m m o n  E x p o r t e d  I n t e r f a c e

                            H e a d e r  F i l e

DESCRIPTION
  This file contains all exported declarations and definitions that are
  common across multiple layers of the RF driver.  It breaks the ideally
  layered driver structure, in which the upper layers are only dependent
  on the lower layers of the driver.  Although maintenance is simplified
  as it is not necessary to duplicate definitions at multiple layers of
  the driver.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1999 - 2016 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in freverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/api/common/rfcom.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/23/15   jhap      LTE Band 66 support
09/05/15   npi     Added rfcom_lte_ca_mode_type
01/28/14   ska     Adding fix for reading correct NLIC delay value
11/24/14   aks     Added BW union rfcom_bw_type_u
11/06/14   JJ      Added BW type for UCLA 20MHz_plus_20MHz BW
10/29/14   stw     Enum Added for Sub band
10/21/14   yb      Added priority configuration for C-RAT scenarios
07/31/14   ndb     Added support for extended EARFCN for LTE
07/28/14   yb      MCL feature: Update Tuner Handle type
07/23/14   yb      MCL feature: structure definitions
06/13/14   kab     Add 3xCA support
04/23/14   gvn     Adding 40MHz bandwidth to enum
01/03/14   svi     LTE B27 Support for BOLT
09/18/13   ms      Support for SGTDS coexistence
09/17/13   sb      Support for SGLTE/SGTDS coexistence
08/23/13   vs      Added support for 3 carrier
08/14/13   kab     LTE B30 Support
07/29/13   tws     Add new RF task RF_TASK_1
07/26/13   vs      Add RF interface to query DBDC bands supported
06/26/13   jyu     Added RFCOM_BAND_TDSCDMA_ANY_BAND
06/24/13   ac      txlm state for dc cfg 
06/19/13   ec      Added RF_MEAS_CALL to rf_task_num_type to track IRAT calls
06/06/13   ec      Added FTM and RF_APPS to rf_task_num_type for logging purposes
05/30/13   tws     Move the RF task num enum to rfcom.h
05/20/13   bar     Added support for split bands B28 for LTE 
05/20/13   al      Added enum for TDS B40_B
05/03/12   rmb     Added rfcom_cell_type Enum.
04/25/13   gvn     Support for Split bands for LTE  
04/16/13   sd      Move rf_hw_type to product specific rfc_hwid.h
                   ( rfa/rfc/<product>/common/inc/rfc_hwid.h )
04/03/13   sd      Added WTR1605_FLUID card for Dime
04/03/13   sd      Added WTR1625_VZ_SV1 and WTR1625_VZ_SV2 cards for Dime
04/03/13   sn      Added RFCOM_INVALID_TIMING_OFFSET
04/02/13   sd      Added WTR2605_GLOBAL_DSDA_NOQFE, WTR2605_GLOBAL_DSDS_NOQFE,
                   WTR2605_TD_DSDA_NOQFE and WTR2605_TD_DSDS_NOQFE for Triton
03/28/13   gy      Add TDSCDMA band, channel status
03/25/13   sd      Added HW ID for WTR1605_NA1_APT_N5331, WTR1605_NA2_APT_N5331,
                   WTR1605_EU1_APT_N5331, WTR1625_APAC and WTR1625_NAEU cards
03/21/13   aa      Added HW ID for RF_HW_WTR1605_NAEU_TP87_PROTO2
03/21/13   aro     Added 3 RF DW IDs for ET
03/20/13   sd      Added new RF HW ID for WTR2605_DSDS_RP1, WTR2605_DSDS_RP2,
                   WTR2605_DSDA_RP1 and WTR2605_DSDA_RP2 cards
03/13/13   sd      Added new RF HW ID for LIQUID, GLOBAL_CA_QFE, RP1 cards
03/11/13   sar     Updates for APQ, GNSS only target.
03/07/13   bsh     B28/B29 support 
03/07/13   vrb     Temporarily remove support for B40_B for bootup crash
03/07/13   aro     Updated NA_ET121 hardware ID to 30
03/06/13   vrb     Support for TDSCDMA B40 split
03/06/13   aro     Added new RF hardware ID for ET-121
02/19/13   pv      Added Enums for LTE Band splits.
02/15/13   fh      Clean up RF HWID 21-25 added DSDS and DSDS_WFR
02/14/13   whz     Added support for RFC WTR2605 TD DSDS
02/08/13   whz     Added support for RFC WTR2605 TD DSDA
02/04/13   fh      Added component WTR2605_GLOBAL_WFR.
02/04/13   dyc     Added RF_HW_WTR1625_GLOBAL_CA_TP100
02/01/13   av      Remove ID 98 
01/30/13   vrb     Support new 1605 APT cards
01/21/13   vrb     Added support for RF card: WTR1605_MOCHA
01/21/13   rmb     Added two more types to the enum "rfcom_rxagc_type" for forceful read of RxAGC interface. 
01/02/13   sn      Added RF_HW_WTR1605_VZ_SV_QFE2320
12/28/12   APU     Added RF_HW_WTR1605_VZ_SV_HCPA card            
12/14/12   shb     Added WTR1625 NAEU and APAC card 
11/28/12   nrk     Added enum to query PLL synth status
11/15/12   ka      Initial support for DCHSUPA
11/14/12   cri     Add B43 so RFCOM enum is continuous
11/09/12   gvn     Add support for new LTE bands 14,23,26,42 (merge from nikel) 
09/24/12   kai     Added support for RF_WTR1605_NA717_QFE1510
10/22/12   dbz     Added enum for GNSS Beidou/Compass configuration
09/25/12   sr      Added dime rf-card hwid enums.
08/17/12   vrb     Added new RF cards for Dime: NA1, NA1_TDS, ATT_CA
07/06/12   tks     Changed CDMA band definition in rfcom_band_type_u   
07/12/12   gh      Add support for RF_WTR1605_NAEU_QF1320_QF1510
07/09/12   jf      Change HW ID for NA717_EFS to match wiki page   
07/02/12   sn      Changed cdma_band type in rfcom_band_type_u
05/29/12   zg      Added HW ID for WTR1605_CHN3_APT   
04/24/12   ac      Added HW ID for WTR1605 NA717_EFS to support binary rfc data
03/30/12   aro     Added HW ID for WTR1605_SVVZ_TP1
03/27/12   aca     Added WTR1605_NA717_BR support
03/22/12   shb     Added HW ID for WTR1605_SVOC
03/20/12   sr      Added HW ID for NA717_EFS to support binary rfc data
01/22/12   adk     Added HW ID for WTR1605_CHN3
01/20/12   kguo    Added HW ID support for SVSP
01/11/12   kg/sn   Added support for QFE1320
12/14/11   sbm     Added union rfcom_band_type_u to support rfm_get_rx_freq_from_rx_earfcn()
12/07/11   dbz     Changed GNSS WWAN indicator ENUM for IMD feature
12/01/11   dw      Add support for WCDMA low power mode (DSR)
11/16/11   ra      Rename WTR1605 Japan11 and Japan21
11/11/11   vrb     Added support for RF card RFC_RTR8600_NSV_B13_BC1
10/20/11   adk     Added support for RF card RFC_RTR8600_NA1025_SV.
10/11/11   sar     Added RF_HW_RTR8600_NA717_BR.
10/11/11   shb     Renamed existing WTR1605 cards to follow 
                   rfc_<rf chip>_<band config>_<optional specifier>
10/11/11   shb     Added NA_Gobi5, EU_Gobi5, NA717 and SVVZ RF cards
08/29/11   jps     Added NA713_NSV HWID   
08/26/11   jps     Added NA_TP50 HWID
08/26/11   pl      Adding LTE B21 support
08/05/11   whc     Added B25 Support 
07/11/11   hy      Including TD-SCDMA support.
07/06/11   tks     Added RF HW ID for WTR1605 based RF cards.  
06/29/11   sar     Removed unused definitions.
06/06/11   tnt     Correct RF HW ID to match with qwiki 
06/03/11   sar     Added RF_HW_RTR8600_NA713_SV_TP1.
06/02/11   sar     Fixed HW ID's for the latest cards added.
06/01/11   sar     Added RF_HW_RTR8600_NA713_SV, RF_HW_RTR8600_NA717, RF_HW_RTR8600_EU,
                   and RF_HW_RTR8600_JAPAN2 cards.
05/10/11   bmg     Added RF_HW_NA713_SV card
05/03/11   shb     Deleted rf_serial_device_enum
05/02/11   whc     Add RF_HW_MDM9X00_XGP = 126
4/23/11    gy      Added NIKEL GNSS ADC enum
4/08/11    ad    Increase LUT vs Pout table size in W calibration to avoid mem corruption
04/07/11   sar     Relocated for CMI and removed conditional #def's. 
03/22/11   ad      Set MAX_APT_SMPS_BIAS_VAL to 390 (used by WCDMA) 
03/21/11   sty     Deleted extern for rfm_initialized
03/18/11   sty     Deleted unused typedefs
03/11/11   sty     Added C++ protection
03/09/11   Saul    Added 1x support for RFNV_BCx_TX_LIM_VS_TEMP_SAR_LVLS
03/03/11   tks     Added support for WCDMA Band XIX.  
02/28/11   tnt     Merge to MDM9K PLF110
02/22/11   can     Added support for RF_HW_MDM9X00_EU_APT.
01/31/11   ac      gst freq comp and temp comp bringup
01/05/11   cri     Updated item 95 for CSFB
11/30/10   can      Added RF_HW_MDM9X00_TDD_GLOBAL_C.
10/29/10   ka      Updates for initial WCDMA APT functionality  
10/11/10   ad      Added preliminary support for WCDMA APT Phase 1  
10/04/10   av      GPS M5 bringup
09/28/10   qma     Added EU HW ID
09/27/10   whc     Added all LTE bands to rfcom_lte_band_type
09/17/10   av      GPS notch filter update
09/13/10   shb     Deleted unnecessary enums - synth_device_enum, 
                   vco_polarity_type, pll_path_enum_type, gps_mode_type, 
                   rf_path_0_config_type, rf_path_1_config_type
07/28/10   shb     Reserved HW ID 114 for Viper
06/25/10   gy      Added GNSS Demod Quality enum
07/06/10   tnt     Add LTE B4 support
06/14/10   tnt     Change RF_HW_MDM9X00_JAPAN2 to different enum value
06/14/10   tnt     Add RF_HW_MDM9X00_JAPAN2 support
05/12/10   ac      BC11 bringup for j8220
04/22/10   rsr     Added support for G8220
04/22/10   tws     Add XO/TCXO system clock flag.
03/04/10   tnt     Add RF_HW_MDM9X00_TDD_GLOBAL
04/07/10   jfc     Add SVLTE RF targets
03/19/10   lcl     Pull in changes from QSC6295
03/04/10   tnt     Add RF_HW_MDM9X00_TDD_GLOBAL
03/01/10   tnt     Add RF_HW_MDM9X00_NA717
02/23/10   ac      fixed compile errors for j8220
02/02/10   ac      added new BC11 for j8220 rf CCA
01/19/10   dw      Added RF_HW_MSM8660_QRF8615
01/15/10   bn      Added SetC RF config
12/17/09   dbz     Changed rfgnss_notch_cfg_type 
11/19/09   lp      Removed FEATURE_CMI featurization around rfcom_adc_logical_channel_type
11/17/09   dyc     Merged in changes from 9k: Added enum for dual carrier.
10/15/09   dbz     Added GNSS notch filter config
08/31/09   bmg     Refactored key types into smaller header components
08/18/09   can     LTE tech additions.
07/31/09   shb     Added bands to rfcom_1x_band_type
07/21/09   jps     Added QRF8600 to rf_hw_type
07/08/09   av      Added Gobi3k and qsc6x95 global to rf_hw_type.
06/22/09   can     Added fixes for compilation.
05/20/09   ckl     Changed RFCOM_BAND_INVALID as the last item of rfcom_wcdma_band_type
05/08/09   vb      Defined more CDMA Bands under rf_card_band_type
04/22/09   dbz     Added gnss linearity type/changed gnss mode type
04/07/09   gy      Added RF_PATH_2 and RF_PATH_3 to support GNSS gen8 
                   RFCOM_RECEIVER_2 and RFCOM_RECEIVER_3.
04/07/09   gy      Added RFCOM_RECEIVER_2 and RFCOM_RECEIVER_3 to support GNSS gen8
03/14/09   gy      Added GNSS Mode definition
03/13/09   ckl     Added RF_HW_PLATFORM_MDM8200_GLOBAL and deleted fixed
                   number for WCDMA types
03/12/08   sr      Added RF_HW_PLATFORM_MDM8200_GLOBAL
03/12/08   sar     Added changes needed for 1x
12/22/08   dj      Added support for 2H2L for platform F raptor2
12/04/08   dw      Added RF_HW_PLATFORM_F_EU_MDM8200 and RF_HW_PLATFORM_F_2H2L_MDM8200.
11/27/08   ahg    Added changes to support 3U2C4G RF card
11/14/08   ahg     Removed featurization (FEATURE_WCDMA_850) around RFI_WCDMA_800_BAND
10/29/08   rmd     QSC7X30 code clean up REV3: Mainline/Removed features
                   (FEATURE_..., RF_HAS_.. and T_MSM...).
10/24/08   rmd     QSC7X30 code clean up REV2: Removed support for USER_DEFINED_SYNTH.
10/07/08   ka      Added RF_HW_PLATFORM_F_JP_MDM8200.
09/06/08   adm     Fixed compiler warning
08/22/08   ka      Added RF_HW_PLATFORM_F_US_MDM8200.
08/20/08   hp      Added enum rfcom_adc_err_enum_type, rfcom_adc_logical_channel_type, adc.h
08/06/08   ad      Removed FEATURE_HDR_REVB around RFCOM_1XEVDO_MODE
07/14/08   adk     Ported DORB changes from the 7800 branch
07/14/08   kma     Added support for platform F raptor2 and UCG224
06/12/08   zg      Added AWS 3d/4d support on TRF6500.
05/27/08   zg      Added RF_HAS_FTSxS card file support.
05/27/08   ka      Changes for dB10 AGC.
05/21/08   adk     Added support for UCG224
05/08/08   zg      Updated FTS card numbers and added RF_HW_DZRF_PCS3dCELL4d.
05/08/08   zg      Added RF_HAS_FTSxS
05/06/08   ad      Export generic ADC read functionality through RFM API
04/24/08   dw      Added support to get the AGC settling time based on tune scenarios.
04/28/08   jfc     Added support for Gobi2000 data card.
04/08/08   dw      Removed obsolete targets support to free up memory
01/14/08   ans/ad  Added MXC6369 support
01/10/08   Vish    Added rfcom_multi_carrier_id_type.
12/06/07   ans     Changed sequence of devices being initialized. Sequence has a dependency
                   on OLTA parameters tables.
111/15/07  ems     Support for QSC1100
11/07/07   jfc     Added new type rfcom_rx_switchpoint_type.
10/26/07   rsr     Added support for PLATFORM_F_SMPS
09/28/07   adk     Updated RF_HW_QRF7600 to 33.
09/27/07   jfc     Change PLATFORM_H to PLATFORM_TRF6285
09/10/07   et      added PLATFORM_H for TRF6285
08/13/07   jfc/lcl Added HOT and COLD regions for GSM LNA gain range
08/02/07   avi     Added suport for QRF7600 (multimode 1x/WCDMA card)
08/09/07   ad      Added support for reading HS LNA switch point NVs during HS calls.
06/25/07   Vish    Added support for multi carrier EVDO rev B (MCDO) mode using
                   FEATURE_HDR_REVB.
06/01/07   aak     Removal of JIDZRF card support
05/21/07   ycl     Remove Platform G definition.
05/07/07   rmd     Added support for the TRF6285.
04/27/07   aak     Removed support for ZRF and JIZRF
04/27/07   hkk/ak  Reordered rf_card_band_type enums
04/13/07   ycl     Replace platform G with hw U2.
03/31/07   ra      Added  RF_HAS_ALTERNATE_GPS_ASIC
03/14/07   dyc     Updated QSC60X5 with DUAL or TRI band support.
03/09/07   bmg     Added RTR6500 support.
03/04/07   dyc     Added QSC60x5 support.
02/27/07   jfc     Moved FEATURE_RFM_IOCTL from rfcom.h to custrf.h. Fixed bug
                    in meta comments
02/15/07   ycl     Added support for U2.
02/15/07   sar     Added support for DZRF6285.
02/08/07   adk     Added RFT device RTR6285_1X
02/05/07   ycl     Merge to 7200.
01/23/07   lcl     Added platform F support.
01/24/07   jfc     Added RF_CAP_LDO_HAS_EXTRA_DELAY to rfcom_rf_cap_type
12/15/06   jfc     Moved structure definition rf_antenna_diff_type to rfcom.h
11/07/06   ra      Added RFCOM_GPS_MODE
10/31/06   ycl     Added for WDZRF 7600 card.
10/17/06   ycl     Modifications for initial MSM7600 build.
05/27/05   sar     Added extern definition for rf_init_state_3 for DUAL mode.
04/07/04   sar     Added extern definitions for state variables accessed in rfm_init().
12/08/03   sar     Allow RFCOM_RECEIVER_DUAL to have it's own value.
11/20/03   sar     Added state RFCOM_RECEIVER_DUAL to accomodate transitions
                   between Diversity and Dual states.
09/17/03   sar     Moved definition for rf_path_state_type to this file.
08/05/03   dbc     Added RFCOM_RECEIVER_DIV enum
12/13/02   sd      Removed temporary GSM synth tuning types (MON and PWR) for
                   non CDMA1X builds.
10/10/02   sar     Added RFCOM_1X_MODE to enum rfcom_mode_enum_type to support
                   cdma 1X mode.
09/10/02   sd      Added support for multimode API that includes GSM drivers.
07/01/02   sd      Initial revision.

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rex.h"
#include "sys.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "sys_v.h"
#include "sys_type.h"
#endif
#include "err.h"

#ifdef __cplusplus
extern "C" {
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#include "rfm_device_types.h"

#include "rfm_mode_types.h"

#include "rfm_cdma_band_types.h"

#include "rfc_hwid.h"

/*---------------------------------------------------------------------------
  Defines whether Rx, Tx, or both synthesizers should be tuned by RF
  driver.  Note: This enum definition must match the corresponding API
  layer enum type definition.
---------------------------------------------------------------------------*/
typedef enum
{
  RFCOM_TUNE_TX,                /* Tune Tx synthesizer only */
  RFCOM_TUNE_RX,                /* Tune Rx synthesizer only */
  RFCOM_TUNE_TX_RX,             /* Tune both Tx and Rx synthesizors */
  RFCOM_TUNE_GSM_MON_RX,        /* Temporary: Tune GSM Rx monitor until
                                   fixed in GSM driver. */
  RFCOM_TUNE_GSM_PWR_RX,        /* Temporary: Tune GSM Rx power until
                                   fixed in GSM driver. */
  RFCOM_NUM_TUNES
} rfcom_synth_tune_enum_type;

/*---------------------------------------------------------------------------
  Defines data structure used for storing captured loop values and
  restoring previously captured loop values.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 tbd1;
  uint8 tbd2;
  uint16 tbd3;
} rfcom_loop_data_type;

#include "rfm_async_types.h"

/*---------------------------------------------------------------------------
  Defines path configuration states.
---------------------------------------------------------------------------*/

typedef enum {
  RF_DISABLED_ST,
  RF_R0_ONLY_ST,
  RF_R1_ONLY_ST,
  RF_DIVERSITY_ST,
  RF_DUAL_ST
} rf_path_state_type;

/*---------------------------------------------------------------------------
  Defines whether Rx, Tx, or both synthesizers should be tuned by RF
  driver.  Note: This enum definition must match the corresponding RF
  common enum type definition.
---------------------------------------------------------------------------*/
typedef enum
{
  RFM_TUNE_TX = RFCOM_TUNE_TX,         /* Tune Tx synthesizer only */
  RFM_TUNE_RX = RFCOM_TUNE_RX,         /* Tune Rx synthesizer only */
  RFM_TUNE_TX_RX = RFCOM_TUNE_TX_RX,   /* Tune both Tx and Rx synthesizors */
  RFM_TUNE_GSM_MON_RX = RFCOM_TUNE_GSM_MON_RX,
                                       /* Temporary: Tune GSM Rx monitor until
                                          fixed in GSM driver. */
  RFM_TUNE_GSM_PWR_RX = RFCOM_TUNE_GSM_PWR_RX,
                                       /* Temporary: Tune GSM Rx power until
                                          fixed in GSM driver. */
  RFM_NUM_TUNES = RFCOM_NUM_TUNES
} rfm_synth_tune_enum_type;


/*---------------------------------------------------------------------------
  Defines data structure used for storing captured loop values and
  restoring previously captured loop values.
---------------------------------------------------------------------------*/
typedef rfcom_loop_data_type rfm_loop_data_type;

/* Defines different error codes for adc reads at rfm layer*/
typedef enum
{
  RFCOM_ADC_ERR_FLAG_SUCCESS,          /* ADC conversion was successful */
  RFCOM_ADC_ERR_FLAG_ABORT,            /* ADC conversion was aborted */
  RFCOM_ADC_ERR_FLAG_RANGE_ERR,        /* Reading out of range */
  RFCOM_ADC_ERR_FLAG_NULL_PTR,         /* Null pointer to save adc value*/
  RFCOM_ADC_ERR_FLAG_MODE_UNSUPPORTED, /* ADC conversion not supported for this mode */
  RFCOM_ADC_ERR_FLAG_DEV_UNSUPPORTED,  /* ADC conversion not supported for this device */
  RFCOM_ADC_ERR_FLAG_CHAN_NOT_PRESENT, /* Selected ADC channel not supported by RF drivers */
} rfcom_adc_err_enum_type;

typedef uint16 rfcom_adc_logical_channel_type;

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
typedef enum {
  RFI_NO_BAND_SEL     = SYS_BAND_CLASS_NONE,

  RFI_CDMA_CELL_BAND  = SYS_BAND_BC0,
  RFI_CDMA_PCS_BAND   = SYS_BAND_BC1,
  //RFI_CDMA_BC2_BAND   = SYS_BAND_BC2,
  RFI_CDMA_BC3_BAND   = SYS_BAND_BC3,
  RFI_CDMA_BC4_BAND   = SYS_BAND_BC4,
  RFI_CDMA_BC5_BAND   = SYS_BAND_BC5,
  RFI_CDMA_BC6_BAND   = SYS_BAND_BC6,
  RFI_CDMA_BC7_BAND   = SYS_BAND_BC7,
  RFI_CDMA_BC8_BAND   = SYS_BAND_BC8,
  RFI_CDMA_BC9_BAND   = SYS_BAND_BC9,
  RFI_CDMA_BC10_BAND  = SYS_BAND_BC10,
  RFI_CDMA_BC11_BAND  = SYS_BAND_BC11,
  RFI_CDMA_BC12_BAND  = SYS_BAND_BC12,
  //RFI_CDMA_BC13_BAND  = SYS_BAND_BC13,
  RFI_CDMA_BC14_BAND  = SYS_BAND_BC14,

  RFI_IMT_BAND        = SYS_BAND_WCDMA_I_IMT_2000,
  RFI_WCDMA_1900_BAND = SYS_BAND_WCDMA_II_PCS_1900,
  RFI_WCDMA_BC3_BAND  = SYS_BAND_WCDMA_III_1700,
  RFI_WCDMA_BC4_BAND  = SYS_BAND_WCDMA_IV_1700,
  RFI_WCDMA_800_BAND  = SYS_BAND_WCDMA_V_850,
  RFI_WCDMA_BC8_BAND  = SYS_BAND_WCDMA_VIII_900,
  RFI_WCDMA_BC9_BAND  = SYS_BAND_WCDMA_IX_1700,
  RFI_WCDMA_BC11_BAND  = SYS_BAND_WCDMA_XI_1500,
  RFI_WCDMA_BC19_BAND = SYS_BAND_WCDMA_XIX_850,   

  RFI_EGSM_BAND       = SYS_BAND_GSM_EGSM_900,
  RFI_DCS_BAND        = SYS_BAND_GSM_DCS_1800,
  RFI_GSM_850_BAND    = SYS_BAND_GSM_850,
  RFI_GSM_1900_BAND   = SYS_BAND_GSM_PCS_1900,

  /**< TDS Band A, 2010-2025 MHz(3GPP Band 34) */
  RFI_TDSCDMA_B34_BAND   = SYS_BAND_TDS_BANDA,
  /**< TDS Band F 1880 -1920 MHz(3GPP Band 39) */
  RFI_TDSCDMA_B39_BAND   = SYS_BAND_TDS_BANDF,
  /**< TDS Band E 2300 - 2400 MHz(3GPP Band 40) */
  RFI_TDSCDMA_B40_BAND   = SYS_BAND_TDS_BANDE,

  RFI_GPS_BAND  
} rfi_band_type;
#endif

typedef enum {
  RF_BC0_BAND = 0,                  /*    US Cellular (800MHz )*/
  RF_BC1_BAND = 1,                  /*    US PCS (1900MHz)     */
  RF_BC2_BAND = 2,                  /*    JTACS                */
  RF_BC3_BAND = 3,                  /*    JCDMA                */
  RF_BC4_BAND = 4,                  /*    Korean PCS           */
  RF_BC5_BAND = 5,                  /*    NMT (450MHz)         */
  RF_BC6_BAND = 6,                  /*    IMT (2.1GHz)         */
  RF_BC7_BAND = 7,
  RF_BC8_BAND = 8,
  RF_BC9_BAND = 9,
  RF_BC10_BAND = 10,
  RF_BC11_BAND = 11,
  RF_BC12_BAND = 12,
  RF_BC13_BAND = 13,
  RF_BC14_BAND = 14,
  RF_BC15_BAND = 15,
  RF_BC16_BAND = 16,
  RF_BC17_BAND = 17,
  RF_BC18_BAND = 18,
  RF_BC19_BAND = 19,

  RF_GPS_BAND,                      /*    1785MHz              */
  RF_FM_BAND,                       /*    ANALOG BAND          */

  RF_BCI_BAND,                      /* WCDMA IMT 2100     */
  RF_BCII_BAND,                     /* WCDMA PCS 1900     */
  RF_BCIII_BAND,                    /* WCDMA 1700         */
  RF_BCIV_BAND,                     /* WCDMA 1700-2100    */
  RF_BCV_BAND,                      /* WCDMA 850          */
  RF_BCVI_BAND,                     /* WCDMA 800          */
  RF_BCVIII_BAND,                   /* WCDMA 900          */
  RF_BCIX_BAND,                     /* WCDMA 1700         */
  RF_BCXIX_BAND,                    /* WCDMA 830-875      */
                    

  RF_EGSM_BAND,                     /* GSM 900             */
  RF_DCS_BAND,                      /* GSM 1800            */
  RF_GSM_850_BAND,                  /* GSM 850             */
  RF_GSM_1900_BAND,                 /* GSM 1900            */

  RF_LTE_B1_BAND,
  RF_LTE_B2_BAND,
  RF_LTE_B3_BAND,
  RF_LTE_B4_BAND,
  RF_LTE_B5_BAND,
  RF_LTE_B6_BAND,
  RF_LTE_B7_BAND,
  RF_LTE_B8_BAND,
  RF_LTE_B9_BAND,
  RF_LTE_B10_BAND,
  RF_LTE_B11_BAND,
  RF_LTE_B12_BAND,
  RF_LTE_B13_BAND,
  RF_LTE_B14_BAND,
  RF_LTE_B17_BAND,
  RF_LTE_B18_BAND,
  RF_LTE_B19_BAND,
  RF_LTE_B20_BAND,
  RF_LTE_B21_BAND,
  RF_LTE_B23_BAND,
  RF_LTE_B25_BAND,
  RF_LTE_B26_BAND,
  RF_LTE_B27_BAND,
  RF_LTE_B28_BAND,
  RF_LTE_B28_B_BAND,
  RF_LTE_B29_BAND,
  RF_LTE_B30_BAND,
  RF_LTE_B33_BAND,
  RF_LTE_B34_BAND,
  RF_LTE_B35_BAND,
  RF_LTE_B36_BAND,
  RF_LTE_B37_BAND,
  RF_LTE_B38_BAND,
  RF_LTE_B39_BAND,
  RF_LTE_B39_B_BAND,
  RF_LTE_B40_BAND,
  RF_LTE_B40_B_BAND,
  RF_LTE_B41_BAND,
  RF_LTE_B41_B_BAND,
  RF_LTE_B41_C_BAND,
  RF_LTE_B42_BAND,
  RF_LTE_B125_BAND,
  RF_LTE_B126_BAND,
  RF_LTE_B127_BAND,
  RF_LTE_B66_BAND,
  RF_BCXI_BAND, 

  RF_TDSCDMA_B34_BAND,
  RF_TDSCDMA_B39_BAND,
  RF_TDSCDMA_B40_BAND,  
  RF_TDSCDMA_B40_B_BAND,
  RF_TDSCDMA_B34_B_BAND,
  RF_TDSCDMA_B39_B_BAND,

  RF_MAX_BAND
} rf_card_band_type;



///////////////////////////////////////////////////////////////////////////////



/*---------------------------------------------------------------------------
  Generic call back handler type definition for RF driver.
---------------------------------------------------------------------------*/
////typedef void (*rfcom_cb_handler_type)( rfcom_cb_event_enum_type, void* );



/* RAT system type.
 */
typedef enum
{
  RFCOM_RGS_NONE=0,
  RFCOM_RGS_CDMA_1X,
  RFCOM_RGS_CDMA_HDR,
  RFCOM_RGS_WCDMA,
  RFCOM_RGS_GSM,
  RFCOM_RGS_TDSCDMA,
  RFCOM_RGS_MAX=0xFF
} rfcom_afc_rgs_type_enum_type;

/* Recent good system data type.
 */
typedef struct
{
  rfcom_afc_rgs_type_enum_type rgs_type;
  int16 vco;
  int16 rot;
  uint8 temp;
  uint32 time;
} rfcom_afc_rgs_data_type;

/* Specify if the data retrieved is a default value, an exact table entry,
 * or an interpolated value from the table.
 */
typedef enum
{
  RFCOM_AFC_DATA_EXACT=0,
  RFCOM_AFC_DATA_INTERPOLATED,
  RFCOM_AFC_DATA_DEFAULT
} rfcom_afc_data_indicator_type;

/* Data type to specify nv items. Multiple items are specified by ORing
 * each mask together.
 */
typedef enum
{
  RFCOM_AFC_NV_VCO_TABLE_M   = 0x01,
  RFCOM_AFC_NV_RGS_DATA_M    = 0x02,
  RFCOM_AFC_NV_ALL_M         = 0xFF
} rfcom_afc_nv_item_mask_type;


/* RF Max Tx Power Limit, Control Channel */
typedef enum
{
  RFCOM_TXPLIM_RACH = 0,    /* RACH */
  RFCOM_TXPLIM_DCH          /* DCH */
} rfcom_txplim_channel_type;

typedef enum
{
  RFCOM_MULTI_CARRIER_ID__0 = 0,
  RFCOM_MULTI_CARRIER_ID__1 = 1,
  RFCOM_MULTI_CARRIER_ID__2 = 2,
  RFCOM_MULTI_CARRIER_ID__CUMULATIVE,  // combined representation of (all) active carrier(s) in the set of IDs: {0, 1, 2}
  RFCOM_MULTI_CARRIER_ID__INVALID
} rfcom_multi_carrier_id_type;

/* Macro for all carriers */
#define RFCOM_MULTI_CARRIER_ID_ALL ((1<<RFCOM_MULTI_CARRIER_ID__0)|( 1<<RFCOM_MULTI_CARRIER_ID__1)|( 1<<RFCOM_MULTI_CARRIER_ID__2))

/*! Enum to list the RF Tx multi-carrier configuration types */
typedef enum
{
  /*! Configuration of RF and Baseband TxFE in single carrier mode */
  RFCOM_TX_SINGLE_CAR_CFG,
  /*! Configuration of RF and Baseband TxFE in dual carrier mode */
  RFCOM_TX_DUAL_CAR_CFG,
  /*! Configuration of RF in single carrier mode but Baseband TxFE in dual carrier mode */
  RFCOM_TX_SC_RF_DC_BB_CFG, 
  /*! Indicate invalid configuration type */
  RFCOM_TX_CFG_INVALID
} rfcom_tx_multi_carrier_cfg_type;

typedef enum
{
  RFCOM_SINGLE_CARRIER = 0,
  RFCOM_DUAL_CARRIER = 1,
  RFCOM_3_CARRIER = 2,
  RFCOM_MAX_CARRIERS = 3
} rfcom_multi_carrier_hspa_id_type;

typedef enum
{
  RFCOM_AGC_FORMAT__RAW,
  RFCOM_AGC_FORMAT__DBM256,
  RFCOM_AGC_FORMAT__DBM10,
  RFCOM_AGC_FORMAT__INVALID
} rfcom_agc_format_type;

typedef enum
{
  RFCOM_SERVINGCELL_RXAGC,
  RFCOM_INGAP_RXAGC,
  RFCOM_SERVINGCELL_RXAGC_RAW,
  RFCOM_INGAP_RXAGC_RAW,
  RFCOM_SERVINGCELL_FORCE_READ_RXAGC,
  RFCOM_SERVINGCELL_FORCE_READ_RXAGC_RAW
} rfcom_rxagc_type;

typedef enum
{
  RFCOM_RXAGC_IN_DBM,
  RFCOM_RXAGC_IN_DB10,
  RFCOM_RXAGC_IN_DB12
} rfcom_rxagc_unit_type;

typedef enum
{
   RFCOM_AGC_MODE__NORMAL_ACQUISITION,
   RFCOM_AGC_MODE__FAST_ACQUISITION,
   RFCOM_AGC_MODE__RESUME_ACQUISITION,
   RFCOM_AGC_MODE__TRACKING,
   RFCOM_AGC_MODE__STOPPED,
   // don't add modes below this
   RFCOM_AGC_MODE__INVALID
} rfcom_agc_mode_type;

/* Type to indicate whether to set Antenna select signals as GPIO or GRFC */
typedef enum
{
  RFCOM_ANT_SEL_GPIO_MODE = 0,
  RFCOM_ANT_SEL_GRFC_MODE
} rfcom_ant_sel_mode_type;

typedef enum
{
  RFCOM_BAND_1X_BC0,                  /*    US Cellular (800MHz )*/
  RFCOM_BAND_1X_BC1,                 /*    US PCS (1900MHz)     */
  RFCOM_BAND_1X_BC2,                  /*    JTACS                */
  RFCOM_BAND_1X_BC3,                  /*    JCDMA                */
  RFCOM_BAND_1X_BC4,                  /*    Korean PCS           */
  RFCOM_BAND_1X_BC5,                  /*    NMT (450MHz)         */
  RFCOM_BAND_1X_BC6,                  /*    IMT (2.1GHz)         */
  RFCOM_BAND_1X_BC7,
  RFCOM_BAND_1X_BC8,
  RFCOM_BAND_1X_BC9,
  RFCOM_BAND_1X_BC10,
  RFCOM_BAND_1X_BC11,
  RFCOM_BAND_1X_BC12,
  RFCOM_BAND_1X_BC13,
  RFCOM_BAND_1X_BC14,
  RFCOM_BAND_1X_BC15,                 /* AWS */
  RFCOM_BAND_1X_BC16,
  RFCOM_BAND_1X_BC18 = 18,
  RFCOM_BAND_1X_BC19 = 19,
  RFCOM_NUM_1X_BANDS
} rfcom_1x_band_type;

typedef enum
{
  RFCOM_BAND_IMT,
  RFCOM_BAND_1900,
  RFCOM_BAND_BC3,
  RFCOM_BAND_BC4,
  RFCOM_BAND_800,
  RFCOM_BAND_BC8,
  RFCOM_BAND_BC9,
  RFCOM_BAND_BC11,
  RFCOM_BAND_BC19,
  RFCOM_NUM_WCDMA_BANDS,
  RFCOM_BAND_INVALID
} rfcom_wcdma_band_type;

/* Enums for supported TDSCDMA bands */
typedef enum 
{
  RFCOM_BAND_TDSCDMA_B34,
  RFCOM_BAND_TDSCDMA_B39,
  RFCOM_BAND_TDSCDMA_B40,  /* This is a full band configuration */
  /* Partial band configuration begins after Band B40  */
  RFCOM_BAND_TDSCDMA_B40_B = 3, /* This is a split band configuration */
  RFCOM_BAND_TDSCDMA_B34_B,
  RFCOM_BAND_TDSCDMA_B39_B,
  RFCOM_NUM_TDSCDMA_BANDS,
  RFCOM_BAND_TDSCDMA_INVALID,
  RFCOM_BAND_TDSCDMA_ANY_BAND
} rfcom_tdscdma_band_type;

typedef enum
{
  RFCOM_BAND_GSM850,
  RFCOM_BAND_GSM900,
  RFCOM_BAND_GSM1800,
  RFCOM_BAND_GSM1900,
  RFCOM_BAND_GSM1800_B,  
  RFCOM_NUM_GSM_BANDS
} rfcom_gsm_band_type;

/* Enums for supported LTE bands */
typedef enum 
{
  RFCOM_BAND_LTE_B1,
  RFCOM_BAND_LTE_B2,
  RFCOM_BAND_LTE_B3,
  RFCOM_BAND_LTE_B4,
  RFCOM_BAND_LTE_B5,
  RFCOM_BAND_LTE_B6,
  RFCOM_BAND_LTE_B7,
  RFCOM_BAND_LTE_B8,
  RFCOM_BAND_LTE_B9,
  RFCOM_BAND_LTE_B10,
  RFCOM_BAND_LTE_B11,
  RFCOM_BAND_LTE_B12,
  RFCOM_BAND_LTE_B13,
  RFCOM_BAND_LTE_B14,
  RFCOM_BAND_LTE_B15,
  RFCOM_BAND_LTE_B16,
  RFCOM_BAND_LTE_B17,
  RFCOM_BAND_LTE_B18,
  RFCOM_BAND_LTE_B19,
  RFCOM_BAND_LTE_B20,
  RFCOM_BAND_LTE_B21,
  RFCOM_BAND_LTE_B22,
  RFCOM_BAND_LTE_B23,
  RFCOM_BAND_LTE_B24,
  RFCOM_BAND_LTE_B25,
  RFCOM_BAND_LTE_B26,
  RFCOM_BAND_LTE_B27,
  RFCOM_BAND_LTE_B28,
  RFCOM_BAND_LTE_B29,
  RFCOM_BAND_LTE_B30,
  RFCOM_BAND_LTE_B31,
  RFCOM_BAND_LTE_B32,
  RFCOM_BAND_LTE_B33,
  RFCOM_BAND_LTE_B34,
  RFCOM_BAND_LTE_B35,
  RFCOM_BAND_LTE_B36,
  RFCOM_BAND_LTE_B37,
  RFCOM_BAND_LTE_B38,   /* This is a full band configuration */ 
  RFCOM_BAND_LTE_B39,
  RFCOM_BAND_LTE_B40,   /* This is a full band configuration */ 
  RFCOM_BAND_LTE_B41,   /* This is a full band configuration */
  RFCOM_BAND_LTE_B42,
  RFCOM_BAND_LTE_B43,
  RFCOM_BAND_LTE_B44,

  /* All the partial band configurations begin after Band enum 44 */
  RFCOM_BAND_LTE_B28_B = 44, /* This is a split band configuration */
  RFCOM_BAND_LTE_B40_B = 45, /* This is a split band configuration */
  RFCOM_BAND_LTE_B41_B = 46, /* This is a split band configuration */
  RFCOM_BAND_LTE_B41_C = 47, /* This is a split band configuration */
  RFCOM_BAND_LTE_B39_B = 48, /* This is a split band configuration */
  /* LTE-U Bands */
  RFCOM_BAND_LTE_B125 = 49,
  RFCOM_BAND_LTE_B126 = 50,
  RFCOM_BAND_LTE_B127 = 51,
  
  /* Tentative definition for new LTE-U bands */
  RFCOM_BAND_LTE_B253 = 52, 
  RFCOM_BAND_LTE_B254 = 53,

  RFCOM_BAND_LTE_B66 = 54,
  RFCOM_NUM_LTE_BANDS,
  RFCOM_BAND_LTE_INVALID,

  RFCOM_BAND_LTE_B252 = RFCOM_BAND_LTE_B125, 
  RFCOM_BAND_LTE_B255 = RFCOM_BAND_LTE_B127, 
} rfcom_lte_band_type;

/* Enums for supported LTE bandwidths */
typedef enum 
{
  RFCOM_BW_LTE_1P4MHz,
  RFCOM_BW_LTE_3MHz,
  RFCOM_BW_LTE_5MHz,
  RFCOM_BW_LTE_10MHz,
  RFCOM_BW_LTE_15MHz,
  RFCOM_BW_LTE_20MHz,
  RFCOM_BW_LTE_40MHz,               /*This is the last standalone LTE BW. Please add CA BW enums after this.*/
  RFCOM_BW_LTE_20MHz_plus_20MHz,   /*BW type for intra-band ULCA*/
  RFCOM_NUM_LTE_BWS,
  RFCOM_BW_LTE_INVALID
} rfcom_lte_bw_type;

/* Enums for supported LTE CA modes */
typedef enum
{
  RFCOM_LTE_NO_CA_MODE,
  RFCOM_LTE_INTRA_BAND_CA_MODE,
  RFCOM_LTE_SRX_MODE = RFCOM_LTE_INTRA_BAND_CA_MODE, /* For now track SRx and Intraband CA as one; may need to change later*/
  RFCOM_LTE_INTER_BAND_CA_MODE,
  RFCOM_LTE_INVALID_CA_MODE
} rfcom_lte_ca_mode_type;

#define NLIC_NUM_LTE_BWS RFCOM_BW_LTE_40MHz+1

/* Enums for Sub Band Types */
typedef enum
{
  RF_LTE_SUB_BAND_NONE = 0x00,
  RF_LTE_SUB_BAND_A,
  RF_LTE_SUB_BAND_B,
  RF_LTE_SUB_BAND_C,
  RF_LTE_SUB_BAND_INVALID
} rfcom_lte_sub_band_type;

#include "../lte/rflte_earfcn_types.h"

typedef enum
{
  DIVERSITY,
  SHDR,
  DIV_MODE_MAX
}rfcom_div_mode_type;

typedef enum
{
  DBG_SCR_TX_AGC,
  DBG_SCR_TX_AGC_IN_DBM,
  DBG_SCR_RX_AGC,
  DBG_SCR_GSM_RX_LEVEL,
  DBG_SCR_LNA_STATE,
  DBG_SCR_PA_RANGE,
  DBG_SCR_HDET,
  DBG_SCR_RSSI,
  DBG_SCR_BAND_WCDMA,
  DBG_SCR_BAND_GSM,
  DBG_SCR_CHANNEL,
  DBG_SCR_SUPPORTED_HW,
  DBG_SCR_NV_BAND
} rfcom_dbg_scr_param_type;

typedef union
{
  uint16 dbg_scr_tx_agc;
  int16  dbg_scr_tx_agc_in_dbm;
  int32  dbg_scr_rx_agc;
  int8   dbg_scr_gsm_rx_level;
  uint8  dbg_scr_lna_state;
  uint8  dbg_scr_pa_range;
  uint8  dbg_scr_hdet;
  int32  dbg_scr_rssi;
  uint16 dbg_scr_channel;
  uint8  dbg_scr_supported_hw;
  uint8  dbg_scr_nv_band;
} rfcom_dbg_scr_param_data_type;

typedef enum
{
  RFCOM_IOCTL_GET_DBG_SCR_TX_AGC          = DBG_SCR_TX_AGC,
  RFCOM_IOCTL_GET_DBG_SCR_TX_AGC_IN_DBM   = DBG_SCR_TX_AGC_IN_DBM,
  RFCOM_IOCTL_GET_DBG_SCR_RX_AGC          = DBG_SCR_RX_AGC,
  RFCOM_IOCTL_GET_DBG_SCR_GSM_RX_LEVEL    = DBG_SCR_GSM_RX_LEVEL,
  RFCOM_IOCTL_GET_DBG_SCR_LNA_STATE       = DBG_SCR_LNA_STATE,
  RFCOM_IOCTL_GET_DBG_SCR_PA_RANGE        = DBG_SCR_PA_RANGE,
  RFCOM_IOCTL_GET_DBG_SCR_HDET            = DBG_SCR_HDET,
  RFCOM_IOCTL_GET_DBG_SCR_RSSI            = DBG_SCR_RSSI,
  RFCOM_IOCTL_GET_DBG_SCR_BAND_WCDMA      = DBG_SCR_BAND_WCDMA,
  RFCOM_IOCTL_GET_DBG_SCR_BAND_GSM        = DBG_SCR_BAND_GSM,
  RFCOM_IOCTL_GET_DBG_SCR_CHANNEL         = DBG_SCR_CHANNEL,
  RFCOM_IOCTL_GET_DBG_SCR_SUPPORTED_HW    = DBG_SCR_SUPPORTED_HW,
  RFCOM_IOCTL_GET_DBG_SCR_NV_BAND         = DBG_SCR_NV_BAND,
  RFCOM_IOCTL_GET_DBG_SCR_RX_AGC_IN_DBM,
  RFCOM_IOCTL_GET_DBG_SCR_RATCHET_STATE,
  RFCOM_IOCTL_GET_DBG_SCR_BAND_1X,
  RFCOM_IOCTL_GET_DBG_SCR_INTELLICEIVER_POWER_MODE,
  RFCOM_IOCTL_GET_DBG_SCR_TX_ADJ,
  RFCOM_IOCTL_GET_SLEEP_STATE,
  //RFCOM_IOCTL_SET_*
  RFCOM_IOCTL_MAX
} rfcom_ioctl_param_type;

typedef union
{
  uint8   uint8_field;
  uint16  uint16_field;
  uint32  uint32_field;
  uint64  uint64_field;
} rfcom_ioctl_pointer_type;

typedef enum
{
  RFCOM_IOCTL_NO_ERROR          = 0,
  RFCOM_IOCTL_INVALID_REQUEST   = 1,
  RFCOM_IOCTL_INVALID_DEVICE    = 2,
  RFCOM_IOCTL_NULL_BUFFER       = 3,
  RFCOM_IOCTL_INVALID_LENGTH    = 4,
  RFCOM_IOCTL_UNKNOWN_ERROR     = 5
} rfcom_ioctl_return_type;


#ifndef FEATURE_GNSS_ONLY_NO_WWAN
 #include "sys_type.h"
#endif

typedef enum
{
  RF_RX_BURST,
  RF_TX_BURST,
  RF_MON_BURST,
  RF_PWR_BURST,
  RF_WCDMA_RF_ON_BURST,
  RF_WCDMA_RF_OFF_BURST,
  RF_MAX_BURST_TYPES
} rf_burst_type;

/* Enum used to indicate when rx power is within limits of current LNA range. */
#define RFCOM_HOT_DELTA                       (4*16)  /* 4dB delta */
#define RFCOM_COLD_DELTA                      (4*16)  /* 4dB delta */

typedef enum
{
  RF_IN_RANGE,
  RF_SATURATED,
  RF_NOISY,
  RF_HOT,
  RF_COLD
} rf_measurement_T;

/* LNA gain ranges.*/
typedef enum
{
  GAIN_RANGE_0,
  GAIN_RANGE_1,
  GAIN_RANGE_2,
  GAIN_RANGE_3,
  GAIN_RANGE_4,
  GAIN_RANGE_5
} rf_gain_range_T;

typedef enum
{
  RF_GSM_RX_SWITCHPOINT_1_2,
  RF_GSM_RX_SWITCHPOINT_2_3,
  RF_GSM_RX_SWITCHPOINT_3_4,
  RF_GSM_RX_SWITCHPOINT_4_5,
  RF_GSM_RX_SWITCHPOINT_MAX
} rfcom_rx_switchpoint_type;

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
typedef enum
{
  RFGSM_BAND_GSM850=SYS_BAND_GSM_850,      /* GSM  850  */
  RFGSM_BAND_GSM900=SYS_BAND_GSM_EGSM_900, /* GSM  900  */
  RFGSM_BAND_GSM1800=SYS_BAND_GSM_DCS_1800,/* GSM  1800 */
  RFGSM_BAND_GSM1900=SYS_BAND_GSM_PCS_1900,/* GSM  1900 */
  RF_MAX_BAND_TYPES,
  RFGSM_BAND_NONE = 0xFF
} rfgsm_band_type;

typedef struct
{
  dBx16_T         valx16;
  dBx16_T         range_high_limitx16; /* Rx pwr > this level is saturated */
  dBx16_T         range_low_limitx16;  /* Rx pwr < this level is noisy     */
  rf_gain_range_T range;            /* Needed for logging purposes only */
} rf_gain_T;
#endif

typedef enum
{
  RF_INIT_SUCCESS,
  RF_INIT_HW_FAIL,
  RF_INIT_INVALID_RFCAL,
  RF_INIT_INVALID_MODE,
  RF_INIT_STATUS_MAX
} rf_init_status_type;

typedef struct
{
  int16 caseA;
  int16 caseB;
  int16 caseC;
  int16 caseD;
} rf_max_tx_pwr_limit_type;

typedef enum
{
  RF_PATH_0,    /* Primary Rx path   */
  RF_PATH_1,    /* Rx diversity path */
  RF_PATH_2,    /* Rx path 2         */
  RF_PATH_3,    /* Rx path 3         */
  RF_PATH_4,    /* Rx path 4         */
  RF_PATH_5,    /* Rx path 5         */
  RF_PATH_NUM,
  RF_PATH_MAX=RF_PATH_NUM
} rf_path_enum_type;

typedef enum
{
  SYS_CLK_XO,
  SYS_CLK_TCXO
} system_clock_enum;

/* gnss mode configuration for gen8 */
typedef enum {
  RFGNSS_MODE_CFG_NB,
  RFGNSS_MODE_CFG_WB,
  RFGNSS_MODE_CFG_RxD,
  RFGNSS_MODE_CFG_GLONASS_TEST,
  RFGNSS_MODE_CFG_BEIDOU
} rfgnss_mode_cfg_enum_type;

typedef enum {
  RFGNSS_NOTCH_CFG_NONE,
  RFGNSS_NOTCH_CFG_1X,
  RFGNSS_NOTCH_CFG_WCDMA,
  RFGNSS_NOTCH_CFG_GSM,
  RFGNSS_NOTCH_CFG_EVDO,
  RFGNSS_NOTCH_CFG_LTE,
  RFGNSS_NOTCH_CFG_TDSCDMA
}rfgnss_notch_cfg_type;

typedef enum
{
  GNSS_DEV_RTR6285,
  GNSS_DEV_RTR8600,
  GNSS_DEV_RGR6240,
  GNSS_DEV_MAX
} gnss_device_enum_type;

/* gnss adc */
typedef enum
{
  GNSS_ADC_RTR8600,
  GNSS_ADC_COLUMBUS,
  GNSS_ADC_NIKEL,
  GNSS_ADC_RGR6240,
  GNSS_ADC_MAX
} gnss_adc_enum_type;

typedef enum
{
  RFGNSS_PATH_0,    /* GNSS Primary Rx path   */
  RFGNSS_PATH_1,    /* GNSS Rx diversity path */
  RFGNSS_PATH_NUM,
  RFGNSS_PATH_MAX=RF_PATH_NUM
} rfgnss_path_enum_type;

/* gnss linearity */
typedef enum {
  RFGNSS_LIN_LL,
  RFGNSS_LIN_HL,
  RFGNSS_LIN_MAX,
} rfgnss_lin_type_enum_type;

typedef enum {
  RFGNSS_DEMOD_QUALITY_HIGH,     /* Error-free demod is required     */
  RFGNSS_DEMOD_QUALITY_NORMAL,   /* Error-free demod is not required */
  RFGNSS_DEMOD_QUALITY_RESERVED, /* Reserved                         */
} rfgnss_demod_quality_enum_type;

#define  RF_PATHS_TABLE_SIZ        2


typedef enum
{
  HDET_DM_NOT_REQUIRED,
  HDET_DM_CLK_ENABLED,
  HDET_DM_CLK_DISABLED
} rfcom_dm_for_hdet_type;

typedef enum
{
  RF_CAP_DM_FOR_HDET,
  RF_CAP_LDO_HAS_EXTRA_DELAY,
  RF_CAP_TECH_DEPENDENT_LNA_SWITCH
} rfcom_rf_cap_type;

typedef enum
{
  RFCOM_REL99_MODE,
  RFCOM_HS_MODE
}rfcom_tech_type;

typedef enum
{
  RF_MM_CMD_SHUTDOWN = RFCOM_PARKED_MODE,
  RF_MM_CMD_CDMA     = RFCOM_1X_MODE,
  RF_MM_CMD_WCDMA    = RFCOM_WCDMA_MODE,
  RF_MM_CMD_NOP      = RFCOM_NUM_MODES,
} rf_multimode_handover_cmd_type;

typedef enum
{
  RF_ANT_QLTY_UNKNOWN,
  RF_ANT_QLTY_IDENTICAL,
  RF_ANT_QLTY_BETTER,
  RF_ANT_QLTY_WORSE
} rf_antenna_diff_type;

#define RFCOM_NUM_SPUR   4  /* Can remove up to 4 spurs */
typedef struct
{
  uint16  num_spurs;
  int32   freq[RFCOM_NUM_SPUR];
} rfm_spur_type;

typedef enum
{
  RFCOMMON_CHANNEL_UL,
  RFCOMMON_CHANNEL_DL,
  RFCOMMON_CHANNEL_MAX
} rfcommon_channel_ul_dl_type;

typedef enum
{
  RF_TUNE_FINE_SCAN,
  RF_TUNE_ACQUISITION,
  RF_TUNE_MAX
} rf_tune_scenario_type;

typedef enum
{
  LINEARIZER,
  FCOMP,
  POUT_DATA_TYPE_MAX
} tx_pout_data_type;

#define NV_TEMP_TABLE_SIZ 8
#define MAX_APT_NUM_PA_RANGES 4
#define MAX_APT_PWR_LIST_SIZE 64
#define FTM_MAX_APT_PWR_LIST_SIZE 128
#define MAX_APT_CHAN_LIST_SIZE 16
#define MAX_APT_SMPS_BIAS_VAL   390 /*9-bit PDM assumed for Card Structure*/
#define MIN_APT_SMPS_BIAS_VAL   0
#define RFNV_1X_NUM_SAR_STATES  8

typedef struct {
  uint8   pa_range;
  uint8   num_pwr_list;  
  int16 tx_lin_pwr_list[FTM_MAX_APT_PWR_LIST_SIZE];
  uint16 tx_lin_pdm_list[FTM_MAX_APT_PWR_LIST_SIZE];
} apt_tx_lin_type;

typedef struct {
  uint8   pa_range;
  uint8   num_freqs;
  int16 fcomp_pwr_list[MAX_APT_CHAN_LIST_SIZE];
  uint16 fcomp_pdm_list[MAX_APT_CHAN_LIST_SIZE];
} apt_freq_comp_type;

typedef struct
{
  uint8 num_pwr_list_curr;  
  /* to be used only for linearizer second sweep table 
   for NV implementation purpose this has to be uint16 */
  uint16 lut_idx[FTM_MAX_APT_PWR_LIST_SIZE]; 
  int16 pout[FTM_MAX_APT_PWR_LIST_SIZE]; // dBm100
  tx_pout_data_type tbl_type;
}pout_tbl_type;

/* Defines the lower power mode configuration type for rfm_set_low_power_mode() */
typedef enum
{
  /* Disable low power mode, revert both modem and RF back to regular mode of operation */
  RFCOM_LPM_DISABLE,
  /* Enable modem low power mode: reduce ADC Fs */
  RFCOM_LPM_MODEM,
  /* Enable modem and RF lower power mode: reduce ADC fs and change RFIC linearity*/
  RFCOM_LPM_MODEM_RF,

  RFCOM_LPM_NUM_CONFIG
}rfcom_lpm_config_enum_type;

/* Union for band types. Used with rfm_get_rx_freq_from_rx_earfcn() function */
typedef union
{
  rfcom_lte_band_type lte_band;
  rfcom_wcdma_band_type wcdma_band;
  rfm_cdma_band_class_type cdma_band;
  rfcom_gsm_band_type gsm_band;
  rfcom_tdscdma_band_type tdscdma_band;
} rfcom_band_type_u;

/* Union for BW types */
typedef union
{
  rfcom_lte_bw_type lte_bw;
} rfcom_bw_type_u;

typedef enum
{
 /*used to query RX PLL*/
  RF_SYNTH_RX,
 /*used to query TX PLL*/
  RF_SYNTH_TX,
 /*Invalid enum*/  
  RF_SYNTH_MAX
}rfcom_synth_type;

/* Type of cell that can be associated with the logical RFM device - Useful for CA in LTE and DBDC in WCDMA */
typedef enum
{
  RFCOM_PRIMARY_CELL,
  RFCOM_SECONDARY_CELL,
  RFCOM_CELL_MAX,
  RFCOM_INVALID_CELL = RFCOM_CELL_MAX
} rfcom_cell_type;

/*===========================================================================*/
/*!
   @name Dual Band Dual carrier

    @brief
     This block will contain the structures related to the implementation of Dual band dual carrier feature.
 */ 

/*! @{ */

/*----------------------------------------------------------------------------*/

typedef struct
{
  /* band supported by the Primary carrier */
  sys_band_mask_type prim_band;
  
  /* band supported by the Secondary carrier */
  sys_band_mask_type sec_band;
  
}rfm_wcdma_dual_band_type;

/*! @} */

#define RFCOM_INVALID_TIMING_OFFSET 0


/*!
  @brief
  Different RF Tasks. 

  @details
  The different RF task that are supported. Currently only one RF task 
  RF_TASK_MAIN is supported. In future we can add low priority RF tasks
  like RF_TASK_LP1. RF_TASK_MAIN task is associated with rf_tcb.
*/

typedef enum
{
   RF_TASK_MAIN,
   RF_TASK_1,
   RF_TASK_2,
   RF_TASK_3,
   RF_MAX_TASK,
   /* Allows us to track calls in context of APPS and FTM tasks */
   RF_TASK_APPS = 0x10,
   RF_TASK_FTM  = 0x20,
   /* This will be logged (hardcoded) when IRAT APIs are called, in order not
      to slow down IRAT execution */
   RF_MEAS_CALL = 0x30,
} rf_task_num_type;

/*!
  @brief
  MCL related structures and definitions
*/
#define RFM_ANTENNA_TUNER_MAX_SCENARIOS_PER_BAND 9

typedef uint32 rfm_tuner_handle_type;

/* Structure containing scenario data */
typedef struct
{
  uint8 scenario_list[RFM_ANTENNA_TUNER_MAX_SCENARIOS_PER_BAND];
  uint8 valid_scenario_nums; 
}rfm_antenna_tuner_scenario_data;

/* Structure containing priority configuration for C-RAT scenarios */
typedef struct
{
  rfm_device_enum_type rfm_device;
}rfcom_antenna_tuner_priority_cfg;

#ifdef __cplusplus
}
#endif

#endif  /* RFCOM_H */
