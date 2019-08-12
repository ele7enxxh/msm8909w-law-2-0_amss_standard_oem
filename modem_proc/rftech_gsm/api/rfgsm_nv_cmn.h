#ifndef RFGSM_NV_H
#define RFGSM_NV_H

/*===========================================================================

                         R F   N V   M a n a g e r

DESCRIPTION
  This header file contains definitions necessary inorder to interface with
  the RF NV Manager.

Copyright (c) 2002 - 2015 by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/api/rfgsm_nv_cmn.h#1 $ 
$DateTime: 2016/12/13 07:59:51 $ 
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/16   kpa     Add debug NV check for Excessive F3 logging reduction
04/26/15   par     Add FEATURE_RF_HAS_QTUNER in GSM NV structure mapping
01/28/15   ec      Changes for new Reserved debug NV
01/27/15   piy     Logging Rx burst RxAGC info and disabling therm reads via debug NV
01/16/14   ec      Add enum and prototype for new debug NV mechanism
11/19/14  huibingl take the channel number discontinuity into account when do GSM RX cal data interpolation
09/15/14   rr      Fix for nv container loading
09/12/14   ec      Add spare NV mask to gate SAWless debug messages
09/09/14   ch      Clean up FEATURE_RF_HAS_QTUNER 
08/22/14   rr      Changes to support nv c3
08/01/14   ec      Add definitions and prototypes for HL NV items for SAWless targets
07/30/14   hoh     Add enhanced rx feature support 
07/02/14   sw      Introduce spare NV bitmasks
06/25/14   zbz     Fix data structure definition for previous check in 
06/20/14   zbz     Add feature to adjust temp comp table based on therm read during cal
04/02/14   zbz     LLVM compiler changes regarding structure allignment
03/19/14   hoh     Added support for 8 PA gain states; made adjustments for unused vbatt code
03/10/14   zbz     Add feature to control pa icq bias through NV item
02/18/14   sc      Use definition for size of amam temp bins
01/15/14   sc      Remove unused APIs using FW shared memory 
11/11/13   tsr     LLVM compiler changes regarding structure allignment
11/07/13   sc      PACK the GSM data structures
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
09/25/13   sb      Added New API to obtain coex band map table
09/25/13   sb      Add NV hooks to GRFC PA range map
09/25/13   sb      Added changes to support GSM TX calibration/online mode operation in coexistence
09/11/13   ggs     Remove PACK definition from gsm_tx_cal_data_nv_type
                   to prevent crash on RUMI due to memory misalignment.
07/31/13   sb      Make changes for BC config to be NV container based
07/18/13   svi     Adding [QTUNER][RL] GSM NV Support.
07/04/13   tws     Add function to return first supported GSM band per device
06/05/13   svi     Adding Support of Enhanced Temperature Compensation in GSM. 
04/20/13   par     Fix compilation error
04/20/13   par     Fix compilation error
04/11/13   par     Fixed RFM_DEVICE hardcoding in some APIs
03/27/13   sb      Added support for Enhanced APT
03/18/13   aka     GSM NV deinit fix
03/14/13   aka     support for new GSM err vs PCL field
03/05/12   sb      Support for Spare NV 
02/12/13   lcl     Added PA range map NV support
02/12/13   sr      changes to free the memory alloocated during rfm_init() 
01/15/13   tc      Port changes from Nikel to address CR403861 (envelope 
                   backoff values extended to achieve min Tx power levels)
12/17/12   aka     Removing legacy GSM NV support
01/03/12   aka     Added RF_GSM_NV_PATH_1 to match NV container values returned from RFC
12/05/12   aka     Added rfm device in rfgsm_nv_calc_path_delay API
12/03/12   aka     Added rfgsm_nv_init_amam_ampm_table protoype and rfgsm_nv_rxtx_enum_type
11/28/12   aka     Changes to extend path based computations
11/15/12   aka     Use a seperate data struct for AMAM NV data with uint16 type
11/08/12   sg      Fixed compiler warnings
11/07/12   sg      Added new API's to remove the NV dependency in the device driver code
08/28/12   sb/tsr  Changed TX burst offset adj timing
07/26/12   sr      re-arranged the rfnv_band enums to be similar to other
                   gsm band enums.
06/27/12   sb      Added Changes to support New GST feature for GSM when PDM for each PA range is passed during TX cal  
06/25/12   jps     Add support for characterized predistortion cal 
05/04/12   sb      Added changes to support TXP indicator for GSM
04/18/12   jps/lcl PA Range and PA R0/R1 logic decoupling 
04/01/12   lcl     Removed empty mdsp interface header file  
03/17/12   jfc     Remove deprecated mdsp_intf_v.h include
02/15/12   sar     Removed RF_HAS_KV_VS_TEMP_COMP.
02/08/12   dej     Updated max power Vbatt compensation to allow for boost
01/10/12   sr      changes to enable vbatt comp.
10/28/11    sr     made changes use the tx_nv_tbl ptr for any tx NV items
                   and rx_nv_tbl for any rx NV items.
09/15/11    sr     added antenna tuner support code.
08/09/11   av      Added NV support for Digital Baseband compensation 
08/08/11   av      Cleanup old temp comp NV items 
09/07/11   sb/rsr  Changes for Delay match setting to use new FW interface.
08/07/11   av      Port Enhanced temp comp changes
09/02/11   sr      ported SAR feature from MDM9k.
08/31/11   sr      Added Online GST related NV 
08/25/11   sb      Added Cal GST related NV 
08/02/11   sr      Made changes to remove unused nv items,
                   increased the amam and ampm tables size,
                   added band specific tx_gain nv.
05/17/11  rsr      Change PA ramp length for nikel. 
05/03/11    sr     featurized the pa_transition_tbl nv item for NikeL.
03/21/11    sr     removed un-used ocde.
03/11/11   sr      Removed un-used featurization
01/27/11   tsr     Enhanced Temp Comp Scheme for Predistorted PCL  
10/14/10   mkv/whc Integrating Reload RFNV from SCMM into MDM9x00 (CL 1315259)
07/20/10   lcl     Removed pa_gain_slope to eliminate compiler warnings.
06/15/10   rsr     Picking up Temp comp changes from QSC6x95.
01/20/10   tws     Use PACK(...) instead of PACKED for qdsp6 compatibility.
                   Add API function to get sigma_delta_max val from RFC.
12/08/09   sr      removed un-used defines.
12/04/09   jps     Updated the RFNV_LINEAR_PA_ENV_BACKOFF_DELTA_TBL_SIZE to 
                   support 12dB dynamic range instead of 6dB
11/30/09   jps     Mainlined FEATURE_GSM_PA_TEMP_COMP    
09/15/09   rsr     Picked up bug fix for Delay cal from LCU.
08/12/09   xw      Added GSM NV support for RTR VCO coarse tune  
07/24/08   can     Changes to RFNV_LINEAR_PA_ENV_BACKOFF_DELTA_TBL_SIZE.
05/15/08   rsr     Changes and additions to predistortion NV items for linear PA.
04/08/08   can     Added support for linear PA EDGE items: pmeas and rgi.
04/07/08   can     Added support for linear PA amam_linear_ and ampm tables.
03/08/08   ycl     Added support for Linear Tx Edge PA switch points.
01/28/08   can     Added rfgsm_nv_precompute_tx_gain_ranges_info.
01/28/08   can     Added rfgsm_nv_get_tx_gain_info_ptr.
01/28/08   can     Added rfnv_tx_gain_range_info_type.
01/28/08   can     Added pa range predistortion switchpoints for QSC6270 CONFIG II
01/28/08   can     Added support for RGI, PMEAS NV items for QSC6270 CONFIG II.     
12/06/07   lcl     Added prototype for rfgsm_nv_get_spare_nv().
11/28/07   can     Added kv_cal_chan to rfgsm_nv_nv_tbl_type.
09/24/07   can     Added kv_cal_value to rfgsm_nv_nv_tbl_type.
02/05/07   ycl     Merge to 7200 tip.
01/25/07   lcl     Merge RSSI temp comp support.
10/17/06   ycl     Modifications for initial MSM7600 build.
07/07/06   adm     Added internal therm changes.
06/01/06   adm     Added polar path delay compensation changes.
03/15/06   rsr     Added support for TX RSB correction. 
03/14/06   jac     Added GSM IM2 element to stucture.
03/06/06    rv     Added support for multislot power back off.
11/14/05   rv      Added defn for RFGSM_ADDNL_PWR.
08/10/05   adm     Removed common polar ramp profile item.
06/10/05   rsr     Added support for temp comp and vbatt backoff
05/27/05   rsr     Added NV for pa_transition table
05/25/05   adm     Added support for W1800 for Triple Taco
05/17/05   rsr     Added support for different precharge NV for each band.
04/21/05   rsr     Added NV item support for polar ramp, antenna timings, 
           3 frequencies autocalibration.
03/29/05    xw     Renamed rfgrfc_gwzrf6250.h to rfgrfc_zrf6275.h
                   Deleted T_RF_GWZRF6250 and RF_HAS_GSM850 etc.
03/02/05   rsr     Renamed macros with RF_ prefix
01/17/05   jtn     Added FEATURE_RF_GSM_SCALE_COMP (TX ramp distortion adjustment).
02/20/05   src     Replaced FEATURE_POLAR_RF with RF_HAS_POLAR.
01/24/05   rsr     Changed AMPM tables to be int32 to fix interpolation bug.
01/20/05   adm     Updates for 8PSK NV items and default settings.
10/26/04    rv     Reduced PA_STEP_SIZE to 8 for Raven.
08/09/04    et     added #ifdef T_RF_GWZRF6250 around #include 
                   rfgrfc_gwzrf6250.h
07/26/04   jtn     Changes related to moving antenna selects to NV.
07/16/04    rv     Changed default burst timing fudge factor to -4 for all bands.
07/14/04   jtn     Sony #1 antenna switch support.
05/19/04   jtn     Support for new NV item GSM_TX_BURST_OFFSET_ADJ.
04/11/04   jtn     Lint.
04/07/04    rv     Added missing declns.
01/12/04   dhh     Updated and featurized PA Temp Comp to cover all power 
                   levels and new bands (850/1900).
10/20/03   lcl     Added rfgsm_nv_rx_switch_type to reduce code size.
08/21/03   ap      Final modifications to support GSM 850/900/1800/1900.
06/25/03   ap      RFNV Consolidation efforts!
05/27/03   jtn     Use NV_TEST_CODE_VER_I instead of NV_RF_CAL_DATE_I to check for old rx gain settings
04/28/03   jtn     Changes for gain switchpoints in NV
04/17/03   jtn     Added rf_cal_date to RAM structure that holds NV items so only have to read it from NV once
04/02/03   jtn     Added extern boolean rfnv_cal_older_than(uint32 specified_date) called by rfgsm_init().
12/05/02   JC      Added frequency compensated switch point functions.
11/15/02   thh     Removed RFGSM_RTR6200_B2.  B2 is detected via revision #
                   read from the chip.
11/07/02   thh     Added RFGSM_RF_CONFIG_MAX to rfgsm_rf_config_type.
10/15/02   thh     Added RFGSM_RTR6200_B2 for GZIFTRIC B2 support.
08/30/02   JC      Obsoleted super-het radio support.
08/28/02   thh     Added DCS support.
08/12/02   thh     Added RF_CONFIG support.
06/10/02   JC      New APIs for Rx frequency compensation moving forwards
                   using ZIF architecture.
04/03/02   thh     Added support for new Txprofile API.
03/20/02   ih      Added PA start/stop time calibration
03/14/02   thh     Added Tx frequency compensation support.
02/12/02   JC      Removed extraneous NV declarations. Removed coupling
                   to nv.h for GPRS development.
01/21/02   JC      Initial Revision for GSM.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "rex.h"
#include "rfcommon_nv.h"
#include "mdsp_intf.h"
#include "rfgsm_core_types.h"
#include "rfm_common_sar.h"
#include "rfnv_item_types.h"
//rsr#include "rfgpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                       PUBLIC DATA DECLARATIONS

===========================================================================*/

#define IS_EGSM900(arfcn) \
  ( arfcn<=124 || (arfcn >=975 && arfcn <=1023) )

#define IS_DCS1800(arfcn) ( arfcn >= 512 && arfcn <= 885)


#define GSM_PA_TEMP_COMP_TBL_SIZE 16

/* Supports multislots from 1-5 */
#define RFGSM_MAX_MULTISLOTS 5

/* The largest delta between the requested ARFCN and the smallest channel
** number that was cal'd.  Arbitrarily select 2000.  In EGSM, for exammple
** you may cal ARFCN=1 and try to compensate for ARFCN 1023 (delta=1022).
*/
#define RF_LARGEST_DELTA_CH_SPACE 2000


/* cold , room and hot temp bins for rssi report failure    */

#define RFGSM_RSSI_TEMP_BIN 3

#define RFGSM_NV_MAX_TX_SWPT_RANGES 10

#define RFGSM_NV_MAX_LINEAR_PA_PRED_SWPTS 2

#define RFGSM_LINEAR_EXPANDED_TBL 2

#define RFNV_LINEAR_PA_ENV_BACKOFF_DELTA_TBL_SIZE 290
#define RFNV_LINEAR_MAX_MOD_TYPE                          2 

/* Added for SAR */
#define RFNV_GSM_SAR_NUM_UL_SLOT_MAX  5  /* Max number of UL slots */
#define RFNV_GSM_SAR_BACK_OFF_LIMIT_DEFAULT 12700  /* 127 dBm in dB100*/

/* Added for Characterized pre-distortion */
#define RFNV_GSM_CHAR_PREDIST_SIZE 16

/* define the number of temp bin */
#define RFNV_GSM_NUM_TEMP_BIN  16

/* The number of extended PA states*/
#define RFCOM_GSM_NUM_PA_STATE 8

/* The number of PA ranges for vbatt */
#define RFCOM_GSM_NUM_VBATT_PA_STATE 4

/*Number of channels in Rx cal*/
 #define RFCOM_GSM_NUM_RX_CAL_CHAN 16

 /*Number of Tx cal channels*/
 #define RFCOM_GSM_NUM_TX_CAL_CHAN 3

/* Number of split parts per band*/
#define RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND 2


#define RFNV_MAX_ARFCN_BINS 16

/* Size of pa_icq_bias */
#define MAX_NUM_OF_GSM_PA_GAIN_STATE 6
#define MAX_NUM_OF_GSM_MOD_TYPES 2
#define MAX_NUM_OF_GSM_RGI_INDEX 32

#define RFGSM_DEBUG_NV_WORD_SIZE  32

/* Only 16 bits shown below (to be extended to 32 bits as each NV is 32-bits wide) */
typedef enum
{
  NV_BIT0,
  NV_BIT1,
  NV_BIT2,
  NV_BIT3,
  NV_BIT4,
  NV_BIT5,
  NV_BIT6,
  NV_BIT7,
  NV_BIT8,
  NV_BIT9,
  NV_BIT10,
  NV_BIT11,
  NV_BIT12,
  NV_BIT13,
  NV_BIT14,
  NV_BIT15
} spare_nv_bits;

/* Bit masks to extract each bit in the nv item */
#define NV_BIT0_MASK (1U << NV_BIT0)
#define NV_BIT1_MASK (1U << NV_BIT1)
#define NV_BIT2_MASK (1U << NV_BIT2)
#define NV_BIT3_MASK (1U << NV_BIT3)
#define NV_BIT4_MASK (1U << NV_BIT4)
#define NV_BIT5_MASK (1U << NV_BIT5)
#define NV_BIT6_MASK (1U << NV_BIT6)
#define NV_BIT7_MASK (1U << NV_BIT7)
#define NV_BIT8_MASK (1U << NV_BIT8)
#define NV_BIT9_MASK (1U << NV_BIT9)
#define NV_BIT10_MASK (1U << NV_BIT10)
#define NV_BIT11_MASK (1U << NV_BIT11)
#define NV_BIT12_MASK (1U << NV_BIT12)
#define NV_BIT13_MASK (1U << NV_BIT13)
#define NV_BIT14_MASK (1U << NV_BIT14)
#define NV_BIT15_MASK (1U << NV_BIT15)

/* Spare NV size */
#define GSM_DEBUG_NV_MAX_SIZE 2

/* Definitions for GSM interface NV (debugging and time profiling) */
#define RFGSM_RESERVED_DEBUG_NV_IDX       0
#define RFGSM_RESERVED_TIME_PROF_NV_IDX   1
#define RFGSM_SPARE_NV_LOGGING_ENABLE_IDX 0
#define RFGSM_SPARE_NV_OVERRIDE_IDX 1

extern int16 rfgsm_mdsp_get_digital_gain( rfgsmlib_rx_gain_range_type gain_range );

/*! 
  @name rfgsm_debug_nv_supported_type
 
  @brief This enum type is the master list of debug NV code that should be
  used to access debug NV in GSM RF Drivers.
 
  @details
  
  This comment block MUST be updated to reflect additions and amendments!
 
  Magic numbers should be avoided in code; if a new check
  against a debug NV bit is required, add the code entry in this enum
  along with the necessary information in rfgsm_nv_cmn.c
 
  Access to debug NV should be exclusively through rfgsm_nv_check_debug_nv()
  whenever possible
 
  The codes are split on the assumption that word index 0 of the debug
  NV is used to enable logging, and index 1 is used to override code
  operations
 
  FOR DOCUMENATION PURPOSES, EACH ENTRY SHOULD HAVE A BRIEF DESCRIPTION
  BELOW:
 
  RFNV_GSM_DEBUG_I NV ITEM:
  ++ LOGGING ++

  RFGSM_DEBUG_NV_FREQ_ERR_LOGGING_ENABLE,
    - Used to enable logging of frequency error in WTR driver
 
  RFGSM_DEBUG_NV_TEMP_COMP_LOGGING_ENABLE,
    - Used to log information regarding temperature compensation
 
  RFGSM_DEBUG_NV_VBATT_COMP_LOGGING_ENABLE,
    - Used to log VBATT compensation information
 
  RFGSM_DEBUG_NV_SAWLESS_LOGGING_ENABLE,
    - Used to enable logging relating to SAWless feature
 
  RFGSM_DEBUG_NV_TX_PCL_CALC_LOGGING_ENABLE,
    - Used to enable logging relating to PCL calculations
 
  RFGSM_DEBUG_NV_KV_CAL_LOGGING_ENABLE,
    - Used to enable logging of information on KV calibration
    in FTM
 
  RFGSM_DEBUG_NV_RX_DEBUG_LOGGING_ENABLE
    - Used to enable logging of information while doing Rx burst
 
  ++ OVERRIDES ++
 
  RFGSM_DEBUG_NV_SAWLESS_FORCE_MODE,
    - Used to force SAWless device to use a specific linearity
    mode
 
  RFGSM_DEBUG_NV_SAWLESS_MODE_TO_FORCE,
    - If RFGSM_DEBUG_NV_SAWLESS_FORCE_MODE is set, then 0 for
    this bit will indicate Low Lin mode, 1 will indicate High Lin
    mode
 
  RFGSM_DEBUG_NV_SAWLESS_DISABLE_JDET,
    - This is used to disable JDET configuration scripts from being
    built
 
  RFGSM_DEBUG_NV_SAWLESS_ALGORITHM_IGNORE_JDET,
    - This causes the SAWless linearity update algorithm to ignore
    JDET readings
 
  RFGSM_DEBUG_NV_SAWLESS_DISABLE_HL_POWER_SCAN,
    - This causes Low Linearity mode to be used in SAWless power
    scan instead of High Linearity mode
 
  RFGSM_DEBUG_NV_DISABLE_THERM_READ
    - This is used to disable Therm Read from being executed

  RFNV_GSM_RESERVED_I NV ITEM

  ++ DEBUG ++
 
  RFGSM_DEBUG_NV_ERR_FATAL_FIRST_SW_OVERRUN
    - Used to force an ERR_FATAL on the first SW overrun
 
  RFGSM_DEBUG_NV_ERR_FATAL_EXCEED_SW_OVERRUN_THRESHOLD
    - Used to force an ERR_FATAL if we exceed the SW overrun threshold
 
  RFGSM_DEBUG_NV_ERR_FATAL_GFW_CALL_RFLM_LATE
    - Used to force an ERR_FATAL if GFW calls RFLM late in the frame
 
  RFGSM_DEBUG_NV_ERR_FATAL_CCS_CB_LATE
    - Used to ERR_FATAL if a CCS callback is late
 
  RFGSM_DEBUG_NV_ERR_FATAL_RXBLANK_DUE_TO_LATE_ENTER_WAKEUP
    - Used to ERR_FATAL if an Rx burst is missed due to a late
    Enter Mode\Wakeup

  ++ TIME PROFILING ++
 
  RFGSM_DEBUG_NV_TIME_PROFILE_CORE
    - Used to enable expanded time profiling in rfgsm core layer
 
*/
typedef enum
{
  /* RFNV_GSM_DEBUG_I NV - LOGGING */
  RFGSM_DEBUG_NV_FREQ_ERR_LOGGING_ENABLE,
  RFGSM_DEBUG_NV_TEMP_COMP_LOGGING_ENABLE,
  RFGSM_DEBUG_NV_VBATT_COMP_LOGGING_ENABLE,
  RFGSM_DEBUG_NV_SAWLESS_LOGGING_ENABLE,
  RFGSM_DEBUG_NV_TX_PCL_CALC_LOGGING_ENABLE,
  RFGSM_DEBUG_NV_KV_CAL_LOGGING_ENABLE,
  RFGSM_DEBUG_NV_RX_DEBUG_LOGGING_ENABLE,
  RFGSM_DEBUG_NV_BOOTUP_LOGGING_ENABLE,
  /* RFNV_GSM_DEBUG_I NV - OVERRIDE */
  RFGSM_DEBUG_NV_SAWLESS_FORCE_MODE,
  RFGSM_DEBUG_NV_SAWLESS_MODE_FORCE_HIGH,
  RFGSM_DEBUG_NV_SAWLESS_DISABLE_JDET,
  RFGSM_DEBUG_NV_SAWLESS_ALGORITHM_IGNORE_JDET,
  RFGSM_DEBUG_NV_SAWLESS_DISABLE_HL_POWER_SCAN,
  RFGSM_DEBUG_NV_DISABLE_THERM_READ,

  /* BOUNDARY INDICATOR BETWEEN SPARE NV AND RESERVED NV ITEMS */
  RFGSM_DEBUG_NV_ITEM_BOUNDARY,
  /* DO NOT REMOVE */

  /* RFNV_GSM_RESERVED_I NV - DEBUG */
  RFGSM_DEBUG_NV_ERR_FATAL_FIRST_SW_OVERRUN,
  RFGSM_DEBUG_NV_ERR_FATAL_EXCEED_SW_OVERRUN_THRESHOLD,
  RFGSM_DEBUG_NV_ERR_FATAL_GFW_CALL_RFLM_LATE,
  RFGSM_DEBUG_NV_ERR_FATAL_CCS_CB_LATE,
  RFGSM_DEBUG_NV_ERR_FATAL_RXBLANK_DUE_TO_LATE_ENTER_WAKEUP,
  /* RFNV_GSM_RESERVED_I NV - TIME PROFILING */
  RFGSM_DEBUG_NV_TIME_PROFILE_CORE,
  /* INSERT NEW DEBUG NV CHECKS ABOVE THIS LINE */

  RFGSM_DEBUG_NV_MAX_SUPPORTED,
} rfgsm_debug_nv_supported_type;

typedef enum
{
  RFGSM_NV_850,   //GSM 850 band NVs
  RFGSM_NV_900,   //EGSMNVs
  RFGSM_NV_1800,  //DCS NVs
  RFGSM_NV_1900,  // PCS NVs
  RFGSM_NV_1800_B,
  RFGSM_NV_MAX_SUPPORTED,
} rfgsm_nv_supported_type;

typedef enum
{
  RFGSM_NV_RX,
  RFGSM_NV_TX,
  RFGSM_NV_RXTX_MAX,
  RFGSM_NV_RXTX_INVALID = RFGSM_NV_RXTX_MAX,
}rfgsm_nv_rxtx_enum_type;

/* SAR limit data based on modulation type */
typedef struct{
   int16 sar_gmsk;  /* GMSK SAR limit in dB */
   int16 sar_8psk;  /* 8PSK SAR limit in dB */
} rfnv_gsm_sar_unit_type;

/* GSM NV LNA gain range Enum.*/
typedef enum
{
  RFNV_GSM_LNA_STATE_1,
  RFNV_GSM_LNA_STATE_2,
  RFNV_GSM_LNA_STATE_3,
  RFNV_GSM_LNA_STATE_4,
  RFNV_GSM_LNA_STATE_5,
  RFNV_GSM_NUM_LNA_STATE
} rfnv_gsm_lna_state_type;


/* SAR limit data based on proximity_sensor, hotspot, and orientation */
typedef struct{
 rfnv_gsm_sar_unit_type sar_lut[RFNV_GSM_SAR_NUM_UL_SLOT_MAX][RF_SAR_STATE_MAX-1];
} rfnv_gsm_sar_lut_type;

typedef struct
{
  boolean nv_read_status_flag;
  rfnv_item_id_enum_type last_nv_item;

}rfnv_gsm_read_status;


/*----------------------------------------------------------------------------*/

typedef uint16  gsm_char_offset_arfcn_tbl_type[RFNV_MAX_ARFCN_BINS];

/*----------------------------------------------------------------------------*/


typedef int32 gsm_tx_char_offset_data_tbl_type[RFNV_MAX_ARFCN_BINS];
/*----------------------------------------------------------------------------*/

typedef int16 gsm_rx_char_offset_data_tbl_type[RFNV_MAX_ARFCN_BINS];


/*----------------------------------------------------------------------------*/

typedef struct
{
  gsm_char_offset_arfcn_tbl_type tx_offset_arfcn_tbl;

  gsm_tx_char_offset_data_tbl_type tx_char_offset_data_tbl;
}gsm_coex_tx_char_offset_info_type;

/*----------------------------------------------------------------------------*/
typedef struct
{
  gsm_char_offset_arfcn_tbl_type rx_offset_arfcn_tbl;


  gsm_rx_char_offset_data_tbl_type rx_char_offset_tbl;

}gsm_coex_rx_char_offset_info_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure holding the mapping information for GSM RF Static NV items
*/

typedef struct
{

  /*Rx cal data NV ID RFNV_GSM_C0_GSM850_RX_CAL_DATA_I */
  rfnv_item_id_enum_type rx_cal_data;

  /*Tx cal data NV ID RFNV_GSM_C0_GSM850_TX_CAL_DATA_I*/
  rfnv_item_id_enum_type tx_cal_data[RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND];

  /*LNA switch point  NV ID RFNV_GSM_C0_GSM850_LNA_SWPT_I*/
  rfnv_item_id_enum_type lna_switch_point;

  /*TX_RX_ANT_TUNER NV ID RFNV_GSM_C0_GSM850_TX_RX_ANT_TUNER_I*/
  rfnv_item_id_enum_type ant_tuner;

  /*Tx timing data NV ID RFNV_GSM_C0_GSM850_TX_TIMING_I*/ 
  rfnv_item_id_enum_type tx_timing_data; 

  /*PA switch point  NV ID RFNV_GSM_C0_GSM850_EXTENDED_PA_SWPT_I*/
  rfnv_item_id_enum_type pa_switch_point;

  /*KV cal NV ID RFNV_GSM_C0_GSM850_KV_CAL_I*/
  rfnv_item_id_enum_type kv_cal;  

  /*TX_GTR_THRESH NV ID RFNV_GSM_C0_GSM850_TX_GTR_THRESH_I*/
  rfnv_item_id_enum_type tx_gtr_thresh;

  /*Polar path delay NV ID RFNV_GSM_C0_GSM850_POLAR_PATH_DELAY_I*/
  rfnv_item_id_enum_type polar_path_delay;
  
  /*VBATT NV ID RFNV_GSM_C0_GSM850_VBATT_I*/
  rfnv_item_id_enum_type vbatt_data;

  /*Envelope gain NV ID RFNV_GSM_C0_GSM850_ENV_GAIN_I*/
  rfnv_item_id_enum_type env_gain;

  /*AMAM Temp COMP NV ID RFNV_GSM_C0_GSM850_AMAM_TEMP_COMP_I*/
  rfnv_item_id_enum_type amam_temp_comp;

  /*Temp COMP RFNV_GSM_C0_GSM850_TEMP_COMP_I*/
  rfnv_item_id_enum_type temp_comp_data;

  /*ENH Temp COMP NV ID RFNV_GSM_C0_GSM850_ENH_TEMP_COMP_I*/
  rfnv_item_id_enum_type enh_temp_comp_data;
  
  /*LINEAR_TX_GAIN_PARAM NV ID RFNV_GSM_C0_GSM850_LINEAR_TX_GAIN_PARAM_I*/
   rfnv_item_id_enum_type tx_gain_param;

  /*MULTISLOT_MAX_TX_PWR NV ID RFNV_GSM_C0_GSM850_MULTISLOT_MAX_TX_PWR_I*/
   rfnv_item_id_enum_type multislot_max_tx_pwr;

  /*POWER_LEVELS NV ID RFNV_GSM_C0_GSM850_POWER_LEVELS_I */
   rfnv_item_id_enum_type power_levels;

  /*SMPS_PDM_TBL NV ID RFNV_GSM_C0_GSM850_SMPS_PDM_TBL_I*/
  rfnv_item_id_enum_type smps_pdm_tbl;

  /*SAR BACKOFF NV ID RFNV_GSM_C0_GSM850_SAR_BACKOFF_I*/
  rfnv_item_id_enum_type sar_lut;

  /*PA Ramp LUT NV ID RFNV_GSM_C0_GSM850_POLAR_RAMP_PROFILE_I*/
  rfnv_item_id_enum_type polar_paramp_lut;

  /* PA Range map ID RFNV_GSM_C0_GSM850_PA_RANGE_MAP_I*/
  rfnv_item_id_enum_type pa_range_map;

  /*APT related NV RFNV_GSM_C0_GSM850_ENH_APT_I*/
  rfnv_item_id_enum_type enh_apt_data;

  /*pa_icq_bias related NV RFNV_GSM_C0_GSM850_PA_ICQ_BIAS */
  rfnv_item_id_enum_type pa_icq_bias;

  /* Therm Read during Cal NV ID RFNV_GSM_C0_GSM850_CAL_TEMP_I */
  rfnv_item_id_enum_type cal_temp;

  #ifdef FEATURE_RF_HAS_QTUNER
   /*Ant Tuner CL related NV_GSM_C0_850_ANT_TUNER_CL_I */
   rfnv_item_id_enum_type ant_tuner_cl;
  #endif 

  /* For filtered path use*/

  rfnv_item_id_enum_type coex_tx_char_offset_info_tbl_id;

  rfnv_item_id_enum_type coex_rx_char_offset_info_tbl_id;

  /*LNA enhanced switch point NV ID RFNV_GSM_C0_GSM850_LNA_ENH_SWPT_I*/
  rfnv_item_id_enum_type lna_enh_switch_point;

  rfnv_item_id_enum_type hl_lna_switch_point;
  
  rfnv_item_id_enum_type hl_rx_cal_data;

} rfgsm_nv_band_items_map_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure holding the mapping information for GSM RF Static NV items
*/

typedef struct
{
  /*Spare_NV RFNV_SPARE_1_I*/
  rfnv_item_id_enum_type gsm_debug_nv_id;
	
  /*Flag to enable GSM Enh temp comp RFNV_GSM_ENH_TEMP_COMP_ENABLE_I*/
  rfnv_item_id_enum_type enh_temp_comp_enable_nv_id;

  /*Flag to enable pa_icq_bias to controlled by RFNV_GSM_C0_GSM850_PA_ICQ_BIAS*/
  rfnv_item_id_enum_type pa_icq_bias_nv_enable;

  /*Flag to enable therm read during cal RFNV_GSM_C0_GSM850_CAL_TEMP_I*/
  rfnv_item_id_enum_type cal_temp_nv_enable;

  /* Flag to enable GSM debugging and time profiling */
  rfnv_item_id_enum_type gsm_intf_debug_nv_id;

} rfgsm_nv_cmn_items_map_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of LNA switch points

  @details
   names are self explainatory 
*/

typedef PACK (struct){
  int8    max_gainrange_1;
  int8    min_gainrange_2;
  int8    max_gainrange_2;
  int8    min_gainrange_3;
  int8    max_gainrange_3;
  int8    min_gainrange_4;
  int8    max_gainrange_4;
  int8    min_gainrange_5;
} gsm_lna_switchpoint_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of TX_RX_ANT_TUNER

  @details
   names are self explainatory 
*/

typedef PACK (struct)
{
 rfcommon_nv_ant_tuner_type ant_tuner_rx;
 rfcommon_nv_ant_tuner_type ant_tuner_tx;
}gsm_ant_tuner_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of Rx static NVs

  @details
   holds the data of path, band and Rx static NVs 
*/

	
typedef struct
{
   /*!
    @brief
      Structure for LNA switch points
    */
  gsm_lna_switchpoint_type lna_switch_point;

   /*!
  @brief
    Structure for HL LNA switch points for SAWless targets
  */
  gsm_lna_switchpoint_type hl_lna_switch_point;

   /*!
    @brief
      Structure for LNA enhanced switch points
    */
  gsm_lna_switchpoint_type lna_enh_switch_point;

  /*@brief
      TX_RX_ANT_TUNER
    */
  gsm_ant_tuner_type ant_tuner;


  gsm_coex_rx_char_offset_info_type coex_rx_char_offset_info_tbl;
  
}gsm_rx_static_nv_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of tx timing related

  @details
   names are self explainatory 
*/
	
typedef PACK (struct)
{
      
  int16 tx_burst_offset_adj;
  int16 pa_en_start_offset_adj;
  int16 pa_en_stop_offset_adj;
  int16 pa_start_offset_adj;
  int16 pa_stop_offset_adj;
  int16 ant_timing_start_offset_adj;
  int16 ant_timing_stop_offset_adj;
} gsm_tx_timing_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of PA switch points: 7 each for gmsk and edge (support up to 8 gain ranges each), and 2 for predistortion

  @details
   names are self explainatory 
*/
	
typedef PACK (struct){
  uint16 gmsk_pa_swpt_r0_to_r1;
  uint16 gmsk_pa_swpt_r1_to_r2;
  uint16 gmsk_pa_swpt_r2_to_r3;
  uint16 gmsk_pa_swpt_r3_to_r4;
  uint16 gmsk_pa_swpt_r4_to_r5;
  uint16 gmsk_pa_swpt_r5_to_r6;
  uint16 gmsk_pa_swpt_r6_to_r7;
  uint16 edge_pa_swpt_r0_to_r1;
  uint16 edge_pa_swpt_r1_to_r2;
  uint16 edge_pa_swpt_r2_to_r3;
  uint16 edge_pa_swpt_r3_to_r4;
  uint16 edge_pa_swpt_r4_to_r5;
  uint16 edge_pa_swpt_r5_to_r6;
  uint16 edge_pa_swpt_r6_to_r7;
  uint16 	pa_predist_swpt1;
  uint16 	pa_predist_swpt2;
} gsm_extended_pa_switchpoint_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of KV CAL

  @details
   names are self explainatory 
*/

typedef  PACK (struct){ 
  uint32   kv_cal_value;  
  uint16   kv_cal_low_chan;
  uint16   kv_cal_high_chan;
} kv_cal_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of TX_GTR_THRESH

  @details
   names are self explainatory 
*/

typedef PACK (struct)
{
  int8 enable;
  int16 rise_thresh;
  int16 fall_thresh;
  int16 adv_time;
}gsm_tx_gtr_thresh_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of VBATT

  @details
   names are self explainatory 
*/

typedef PACK (struct)
{
  int16	vbatt_comp_dBx100;
  int16	vbatt_comp_parange_dBx100 [RFCOM_GSM_NUM_VBATT_PA_STATE];
}gsm_vbatt_comp_data_type;


typedef PACK (struct)
{
  uint16		vbatt_levels[3];
  gsm_vbatt_comp_data_type	vbatt_comp_value_lo;
  gsm_vbatt_comp_data_type	vbatt_comp_value_hi;
}gsm_vbatt_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of Envolpe gain

  @details
   names are self explainatory 
*/
typedef PACK (struct)
{
  uint16 env_gain_freq_rgi_f1[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  uint16 env_gain_freq_rgi_f2[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
  uint16 env_gain_freq_rgi_f3[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
}gsm_env_gain_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of amam_temp_comp

  @details
   names are self explainatory 
*/ 

typedef int8 amam_temp_comp[RFGSM_CORE_TEMP_COMP_16_BIN_SIZE];

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of TEMP COMP PCL

  @details
   names are self explainatory 
*/


typedef PACK (struct)
{
int16 temp_comp_pcl_pwr_scaling[RFNV_GSM_NUM_TEMP_BIN];
uint16 hot_temp_comp_pcl_pwr_offset_gsmk[RFNV_NUMBER_OF_PCLS];
uint16 cold_temp_comp_pcl_pwr_offset_gsmk[RFNV_NUMBER_OF_PCLS];
uint16 hot_temp_comp_pcl_pwr_offset_8psk[RFNV_NUMBER_OF_PCLS];
uint16 cold_temp_comp_pcl_pwr_offset_8psk[RFNV_NUMBER_OF_PCLS];
}gsm_temp_comp_data_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data for ENH TEMP COMP PCL which uses two 16 by 16 arrays for temp_compensation 

  @details
   names are self explanatory 
*/

typedef struct
{
  int16 tempcomp_table_gmsk[RFNV_GSM_NUM_TEMP_BIN][RFNV_NUMBER_OF_PCLS];
  int16 tempcomp_table_8psk[RFNV_GSM_NUM_TEMP_BIN][RFNV_NUMBER_OF_PCLS];
 
}gsm_enh_temp_comp_data_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of LINEAR_TX_GAIN_PARAM

  @details
   names are self explainatory 
*/

typedef PACK (struct)
{
uint16 gsm_linear_tx_gain_val;
uint16 edge_linear_tx_gain_val; 
uint8 charpredist_env_gain[RFNV_GSM_CHAR_PREDIST_SIZE];
}gsm_linear_tx_gain_param_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of MULTISLOT_MAX_TX_PWR

  @details
   names are self explainatory 
*/

typedef PACK (struct)
{
 int16 gsm_multislot_tx_pwr_gmsk[RFGSM_MAX_MULTISLOTS];
 int16 gsm_multislot_tx_pwr_8psk[RFGSM_MAX_MULTISLOTS];
}gsm_multislot_max_tx_pwr_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of POWER_LEVELS

  @details
   names are self explainatory 
*/

typedef int16  gsm_power_levels[RFNV_NUMBER_OF_PCLS];

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of SMPS_PDM_TBL

  @details
   names are self explainatory 
*/

typedef PACK (struct)
{
 uint16 cal_smps_pdm_tbl[RFGSM_MAX_TX_GAIN_RANGE];  /* per PA state PDM to be used for cal, for using same PDM across all PA states, use same value for all indices*/
 uint16 gsm_smps_pdm_tbl[RFNV_NUMBER_OF_PCLS];			/* PDM to be used for GSM Online mode, per PCL*/
 uint16 edge_smps_pdm_tbl[RFNV_NUMBER_OF_PCLS]; 		/* PDM to be used for EDGE Online mode, per PCL*/
}gsm_smps_pdm_tbl_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of enhanced apt

  @details
   names are self explainatory 
*/
typedef PACK(struct)
{
 uint8 pcl_threshold;      /* Below this pwr_index, forced APT mode or bypass depending upon vbatt_thres_low_pwr*/
 uint16 vbatt_thres_low_pwr; /* if for low pwr (below pcl_threshold), vbatt reading is less than this value set bypass mode else forced apt mode */
 uint16 vbatt_thresold;		 /* Above this voltage, forced apt mode else bypass mode*/
}enh_apt_data_type;


/* SAR limit data based on modulation type */
typedef struct{
   int16 sar_gmsk;  /* GMSK SAR limit in dB */
   int16 sar_8psk;  /* 8PSK SAR limit in dB */
} gsm_sar_backoff_data_type;


/* SAR limit data based on proximity_sensor, hotspot, and orientation */
typedef struct{
 gsm_sar_backoff_data_type sar_lut[RFNV_GSM_SAR_NUM_UL_SLOT_MAX][RF_SAR_STATE_MAX-1];
} gsm_sar_lut_type;

/* store pa_icq_bias from NV */
typedef struct{
 uint32 pa_icq_bias[MAX_NUM_OF_GSM_PA_GAIN_STATE][MAX_NUM_OF_GSM_MOD_TYPES][MAX_NUM_OF_GSM_RGI_INDEX];
}gsm_pa_icq_bias_type;

typedef struct
{
uint16 ramp_up[NV_TX_BURST_RAMP_SIZ];   
uint16 ramp_down[NV_TX_BURST_RAMP_SIZ];
}gsm_pa_ramp_lut_type;


typedef struct 
{
  int16   freq_comp_data[RFCOM_GSM_NUM_RX_CAL_CHAN];
} gsm_rx_freq_comp_data_type;

typedef PACK (struct)
{
 uint8  rx_cal_chan_size;	/*Number of valid entries in channel list that has freq comp data (8 for low bands, 16 for high bands)*/
 uint16 rx_cal_chan_list[RFCOM_GSM_NUM_RX_CAL_CHAN];  /*Rx calibrated channel list , capped at 16*/
 gsm_rx_freq_comp_data_type  rx_freq_comp_data[RFNV_GSM_NUM_LNA_STATE];	 /*Rx FREQ comp data for all LNA states*/
} gsm_rx_cal_data_nv_type;

typedef PACK (struct)
{
  uint16 therm_read;
} gsm_cal_temp_data_type;

#ifndef FEATURE_BOLT_MODEM
typedef struct
{   
  uint8  data_valid;
  uint16 data[GSM_AMAM_TBL_SIZE * RFGSM_LINEAR_EXPANDED_TBL];   /*total size 256*/
} gsm_amam_data_type;

typedef struct
{  
  uint8  data_valid;
  int16 data[GSM_AMPM_TBL_SIZE * RFGSM_LINEAR_EXPANDED_TBL];   /*total size 256*/
} gsm_ampm_data_type;

typedef struct
{ 
  gsm_amam_data_type amam_tbl[RFCOM_GSM_NUM_TX_CAL_CHAN];	
} gsm_amam_table_type;


typedef struct
{ 
  gsm_ampm_data_type ampm_tbl[RFCOM_GSM_NUM_TX_CAL_CHAN];	
} gsm_ampm_table_type;


typedef struct
{  
  uint8  data_valid;
  int16 data[RFNV_NUMBER_OF_PCLS];   /*total size 16*/
} gsm_tx_pwr_err_entry_type;


typedef struct
{ 
  gsm_tx_pwr_err_entry_type tx_pwr_err_data[RFCOM_GSM_NUM_TX_CAL_CHAN];
} gsm_tx_pwr_err_data_type;
	

typedef struct
{
  uint8  data_valid;
  int16 pmeas[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
} gsm_tx_pmeas_entry_type;

typedef struct
{
  gsm_tx_pmeas_entry_type  tx_pmeas_data_table[RFCOM_GSM_NUM_TX_CAL_CHAN];
} gsm_tx_pmeas_data_type;

typedef struct
{
uint8  data_valid;
uint8  rgi_list_size;
uint8  rgi_list[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
} gsm_tx_rgi_table_type;

typedef PACK (struct)
{
   /*Number of valid entries in channel list that has freq comp data (capped at 3 for now)*/
   uint8  tx_cal_chan_size;		
   
   /*Tx calibrated channel list , Max list size is 3*/
   uint16 tx_cal_chan[RFCOM_GSM_NUM_TX_CAL_CHAN];	 
   
   /* Dynamic range for PD case*/
   int16 amam_max_dbm;

   /* NV for PD case RGI*/
   uint8 rgi_for_pred;		   

   /*Variant Marker*/
   rfnv_data_variant_marker_type rfnv_data_variant_marker;

   /* list of RGI for each PA state*/
   gsm_tx_rgi_table_type gsm_rgi_table[RFCOM_GSM_NUM_PA_STATE];

   /* list of RGI for each PA state*/
   gsm_tx_rgi_table_type edge_rgi_table[RFCOM_GSM_NUM_PA_STATE];

   /* GMSK RGI vs PMEAS data*/
   gsm_tx_pmeas_data_type gsm_pmeas_table[RFCOM_GSM_NUM_PA_STATE];
   
   /* 8PSK RGI vs PMEAS data*/   	
   gsm_tx_pmeas_data_type edge_pmeas_table[RFCOM_GSM_NUM_PA_STATE]; 
   
   /* GMSK PCL vs Tx pwr err data*/
   gsm_tx_pwr_err_data_type gsm_tx_pwr_err_per_pcl;
   
   /* 8PSK PCL vs Tx pwr err data*/
   gsm_tx_pwr_err_data_type edge_tx_pwr_err_per_pcl;
   
   /* AMAM master table*/
   gsm_amam_table_type  amam_master_table; 
   
   /* AMPM master table*/
   gsm_ampm_table_type ampm_master_table;

   /* Therm read data during calibration*/
   gsm_cal_temp_data_type cal_temp;
 } gsm_tx_cal_data_nv_type;
 #else
 
typedef PACK (struct)
{   
  uint8  data_valid;
  uint16 data[GSM_AMAM_TBL_SIZE * RFGSM_LINEAR_EXPANDED_TBL];   /*total size 256*/
} gsm_amam_data_type;

typedef PACK (struct)
{  
  uint8  data_valid;
  int16 data[GSM_AMPM_TBL_SIZE * RFGSM_LINEAR_EXPANDED_TBL];   /*total size 256*/
} gsm_ampm_data_type;

typedef PACK (struct)
{ 
  gsm_amam_data_type amam_tbl[RFCOM_GSM_NUM_TX_CAL_CHAN];	
} gsm_amam_table_type;


typedef PACK (struct)
{ 
  gsm_ampm_data_type ampm_tbl[RFCOM_GSM_NUM_TX_CAL_CHAN];	
} gsm_ampm_table_type;


typedef PACK (struct)
{  
  uint8  data_valid;
  int16 data[RFNV_NUMBER_OF_PCLS];   /*total size 16*/
} gsm_tx_pwr_err_entry_type;


typedef PACK (struct)
{ 
  gsm_tx_pwr_err_entry_type tx_pwr_err_data[RFCOM_GSM_NUM_TX_CAL_CHAN];
} gsm_tx_pwr_err_data_type;
	

typedef PACK (struct)
{
  uint8  data_valid;
  int16 pmeas[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
} gsm_tx_pmeas_entry_type;

typedef PACK (struct)
{
  gsm_tx_pmeas_entry_type  tx_pmeas_data_table[RFCOM_GSM_NUM_TX_CAL_CHAN];
} gsm_tx_pmeas_data_type;

typedef PACK (struct)
{
uint8  data_valid;
uint8  rgi_list_size;
uint8  rgi_list[RFNV_LINEAR_MAX_RGI_INDEX_PER_GAIN_STATE];
} gsm_tx_rgi_table_type;

typedef PACK (struct)
{
   /*Number of valid entries in channel list that has freq comp data (capped at 3 for now)*/
   uint8  tx_cal_chan_size;		
   
   /*Tx calibrated channel list , Max list size is 3*/
   uint16 tx_cal_chan[RFCOM_GSM_NUM_TX_CAL_CHAN];	 
   
   /* Dynamic range for PD case*/
   int16 amam_max_dbm;

   /* NV for PD case RGI*/
   uint8 rgi_for_pred;		   

   /*Variant Marker*/
   rfnv_data_variant_marker_type rfnv_data_variant_marker;

   /* list of RGI for each PA state*/
   gsm_tx_rgi_table_type gsm_rgi_table[RFCOM_GSM_NUM_PA_STATE];

   /* list of RGI for each PA state*/
   gsm_tx_rgi_table_type edge_rgi_table[RFCOM_GSM_NUM_PA_STATE];

   /* GMSK RGI vs PMEAS data*/
   gsm_tx_pmeas_data_type gsm_pmeas_table[RFCOM_GSM_NUM_PA_STATE];
   
   /* 8PSK RGI vs PMEAS data*/   	
   gsm_tx_pmeas_data_type edge_pmeas_table[RFCOM_GSM_NUM_PA_STATE]; 
   
   /* GMSK PCL vs Tx pwr err data*/
   gsm_tx_pwr_err_data_type gsm_tx_pwr_err_per_pcl;
   
   /* 8PSK PCL vs Tx pwr err data*/
   gsm_tx_pwr_err_data_type edge_tx_pwr_err_per_pcl;
   
   /* AMAM master table*/
   gsm_amam_table_type  amam_master_table; 
   
   /* AMPM master table*/
   gsm_ampm_table_type ampm_master_table;

   /* Therm read data during calibration*/
   gsm_cal_temp_data_type cal_temp;
 } gsm_tx_cal_data_nv_type;

 #endif
/*
  LLVM compiler changes:
  Unaligned means aligning to next byte. Equivalent to PACK
  Note: a member of an unaligned (packed) structure, must also be considered unaligned even though it was not defined so.  
*/
#ifndef FEATURE_BOLT_MODEM
typedef gsm_amam_table_type  __attribute__((aligned(1))) gsm_amam_table_type_unalign;
typedef gsm_ampm_table_type  __attribute__((aligned(1))) gsm_ampm_table_type_unalign;
typedef gsm_amam_data_type __attribute__((aligned(1))) gsm_amam_data_type_unalign;
typedef gsm_ampm_data_type __attribute__((aligned(1))) gsm_ampm_data_type_unalign;
#else
typedef gsm_amam_table_type gsm_amam_table_type_unalign;
typedef gsm_ampm_table_type gsm_ampm_table_type_unalign;
typedef gsm_amam_data_type gsm_amam_data_type_unalign;
typedef gsm_ampm_data_type gsm_ampm_data_type_unalign;
#endif



/*-------------------------------------------------------------------------------------------------*/

typedef struct 
{
  uint16            num_gain_ranges;
  uint16            switch_point[RFGSM_NV_MAX_TX_SWPT_RANGES];  
  uint16            predistortion[RFGSM_NV_MAX_TX_SWPT_RANGES];       
  uint16            pa_state[RFGSM_NV_MAX_TX_SWPT_RANGES];
  uint16            amam_tbl_bank[RFGSM_NV_MAX_TX_SWPT_RANGES];  
} rfnv_tx_gain_range_info_type;

extern const rfcommon_rfnv_items_list_type rfgsm_nv_cmn_items_list[];

extern boolean rfnv_rf_config_read;
//extern typedef mdsp_pa_ramp_type;

extern boolean rfgsm_has_rssi_temp_comp;


typedef struct
{
  int16 cold;
  int16 room;
  int16 hot;
} rx_gain_range_temp_type;


typedef struct
{
  rx_gain_range_temp_type  rx_gain_range_rssi_temp_comp[5 /*RF_BB_MAX_GAINS*/];  /* this should be 5 */
} rx_gain_range_rssi_temp_comp_type;

typedef PACK (struct)
{
    int8 enable;
    int16 rise_thresh;
    int16 fall_thresh;
    int16 adv_time;
}rfgsm_nv_gtr_thresh_type;

typedef struct
{
  /* 16 (temp bin) by 16 (PCLs) */
  int16 tempcomp[RFNV_GSM_NUM_TEMP_BIN][RFNV_NUMBER_OF_PCLS];
}rfgsm_nv_enh_temp_comp_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Enum to hold NV path info. Each NV path maps to a RFM path

  @details
   Mapping is controlled through rfgsm_nv_get_nv_path_index API.
*/

	
typedef enum
{
	
  RF_GSM_NV_PATH_0,
  /*NV Path for RF Transceiver Path 0 */

  RF_GSM_NV_PATH_1,
  /*NV Path for RF Transceiver Path 1 */
	
  RF_GSM_NV_PATH_2,
  /*NV Path for RF Transceiver Path 2 */

  RF_GSM_NV_PATH_3,
  /*NV Path for RF Transceiver Path 2 */  
	
  RF_GSM_NV_PATH_NUM,  /*!< Total number of NV Path Available */
	
  RF_GSM_NV_PATH_MAX = RF_GSM_NV_PATH_NUM   /*!< Max Physical Path for NV */
} rfgsm_nv_path_type;

/*!
  @brief
  Structure to hold NV containers for RX and TX for a given logical device and band

  @details
  Structure to hold NV containers for RX and TX for a given logical device and band
*/


typedef struct
{
  rfgsm_nv_path_type nv_container[2]; // 0 for RX, 1 for TX

}
rfgsm_nv_container_array_type;



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of Tx static NVs

  @details
   holds the data of path, band and Tx static NVs 
*/



typedef struct 
{

  /*!
	@brief
   Structure for Tx timing 
  */
	
  gsm_tx_timing_data_type tx_timing_data;

    /*!
	@brief
	   Structure for PA switch points
    */
	  
  gsm_extended_pa_switchpoint_type pa_switch_point;

  /*!
  @brief
   Structure for PA range map
   */

  uint32 pa_range_map; 

  /*!
  @brief
   Structure for KV CAL
   */

  kv_cal_type kv_cal; 

 /*!
	@brief
   Structure for TX_GTR_THRESH
   */
	   
  gsm_tx_gtr_thresh_type tx_gtr_thresh;

  /*!
	@brief
  POLAR PATH DELAY
   */

  int16 polar_path_delay;	 

	 /*!
		@brief
	  VBATT
	   */

  gsm_vbatt_type vbatt_data;

	 
    /*!
		@brief
	   Envelope gain
	   */
	 
  gsm_env_gain_type env_gain;

    /*!
		@brief
	   AMAM TEMP COMP
	   */
	   
  amam_temp_comp gsm_amam_temp_comp;

	/*!
		@brief
	    TEMP COMP DATA
	   */
  gsm_temp_comp_data_type temp_comp_data;   


	/*!
		@brief
	    ENH TEMP COMP DATA
	   */
  gsm_enh_temp_comp_data_type enh_temp_comp_data;   

	/*!
		@brief
	    LINEAR_TX_GAIN_PARAM
	   */
  gsm_linear_tx_gain_param_type tx_gain_param;

	/*!
		@brief
	    MULTISLOT_MAX_TX_PWR
	   */
  gsm_multislot_max_tx_pwr_type multislot_max_tx_power;

	/*!
		@brief
	    POWER_LEVELS
	   */
  gsm_power_levels power_levels;

	/*!
		@brief
	    SMPS_PDM_TBL
	   */
  gsm_smps_pdm_tbl_type smps_pdm_tbl;

	/*!
		@brief
	    ENH_APT
	   */
  enh_apt_data_type enh_apt_data;

	/*!
		@brief
	    SAR-BACKOFF
	   */
  gsm_sar_lut_type sar_lut_data;

  /*!
	  @brief
	  Polar PA ramp profile
	 */
  gsm_pa_ramp_lut_type polar_paramp_lut;
	
  /*!
	  @brief
	  PA ICQ BIAS
	 */  
  gsm_pa_icq_bias_type pa_icq_bias_data;
	
 /*!
	  @brief
	  CL Tuner NV data
	 */
  rfcommon_nv_ant_tuner_cl_ctrl_type ant_tuner_cl_data;	
 
  gsm_coex_tx_char_offset_info_type coex_tx_char_offset_info_tbl;


} gsm_tx_static_nv_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Structure to hold main pointer for common NVs for a path

  @details
*/


typedef struct
{
  uint32 gsm_debug_nv[GSM_DEBUG_NV_MAX_SIZE];

  uint16 size_gsm_debug_nv;

  uint8 enh_tempcomp_enable_nv;
  
  uint8 pa_icq_bias_nv_enable;
  
  uint8 cal_temp_nv_enable;

  uint32 gsm_rf_reserved_nv[GSM_DEBUG_NV_MAX_SIZE];

}rfgsm_nv_cmn_data_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  Holds the data of GSM  NVs

  @details
*/

	
typedef struct 
{

   /*!
 	@brief
 	Indicates what NV chain this structure was initialized with.
   */
  rfgsm_nv_path_type path;

  /*!
	@brief
	Indicates what band this structure was initialized with.
  */
  rfgsm_nv_supported_type band;

  /*!
	@brief
	 static NVs for Rx
  */
  gsm_rx_static_nv_type *rx_static_nv_data_ptr;

  /*!
	@brief
	 static NVs for Tx
  */
  gsm_tx_static_nv_type *tx_static_nv_data_ptr;

  /*!
	@brief
	Holds the data of Rx cal
  */
  gsm_rx_cal_data_nv_type rx_cal_nv_data;

  /*!
	@brief
	Holds the data of Rx cal offsets for HL mode on SAWless targets
  */
  gsm_rx_cal_data_nv_type hl_rx_cal_nv_data;

  /*!
	@brief
	Holds the data of Tx cal
  */
  gsm_tx_cal_data_nv_type tx_cal_nv_data; 
}rfgsm_nv_band_data_type;

typedef struct
{
  int16 rx_switch_1_2_dBm[RFCOM_GSM_NUM_RX_CAL_CHAN];
  int16 rx_switch_2_3_dBm[RFCOM_GSM_NUM_RX_CAL_CHAN];
  int16 rx_switch_3_4_dBm[RFCOM_GSM_NUM_RX_CAL_CHAN];
  int16 rx_switch_4_5_dBm[RFCOM_GSM_NUM_RX_CAL_CHAN];
} rfgsm_nv_rx_switch_type;

typedef struct
{
  int16 enh_rx_switch_1_2_dBm[RFCOM_GSM_NUM_RX_CAL_CHAN];
  int16 enh_rx_switch_2_3_dBm[RFCOM_GSM_NUM_RX_CAL_CHAN];
  int16 enh_rx_switch_3_4_dBm[RFCOM_GSM_NUM_RX_CAL_CHAN];
  int16 enh_rx_switch_4_5_dBm[RFCOM_GSM_NUM_RX_CAL_CHAN];
} rfgsm_nv_enh_rx_switch_type;


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the lowest supported band for a given device

  @details
  
  @param rfm_dev
  The RFM device for which the lowest GSM band supported is required
 
  @return
  GSM band for passed RFM device
*/
rfcom_gsm_band_type rfgsm_nv_get_lowest_supported_band( rfm_device_enum_type rfm_dev );


/*===========================================================================

FUNCTION rfgsm_nv_get_tbl_ptr                                 

DESCRIPTION
  Returns pointer to GSM RF NV table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RF NV table.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_band_data_type *rfgsm_nv_get_tbl_ptr
( 
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band,
  rfgsm_nv_rxtx_enum_type rx_tx
);

/*===========================================================================
FUNCTION rfgsm_nv_get_cmn_tbl_ptr                                 

DESCRIPTION
  Returns pointer to GSM COMMON RF NV table.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM COMMON RF NV table.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_cmn_data_type *rfgsm_nv_get_cmn_tbl_ptr (rfm_device_enum_type rfm_dev);
/*===========================================================================

FUNCTION RFGSM_NV_GET_RX_FREQ_COMP_SWITCH_PTR                                 

DESCRIPTION
  Returns pointer to GSM RX frequency compensation switch.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RX frequency compensation switch.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_rx_switch_type *rfgsm_nv_get_rx_freq_comp_switch_ptr
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band
);

/*===========================================================================

FUNCTION RFGSM_NV_GET_HL_RX_FREQ_COMP_SWITCH_PTR                                 

DESCRIPTION
  Returns pointer to GSM RX frequency compensation switch to be used in HL mode
  on SAWless targets.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RX frequency compensation switch to be used in HL mode
  on SAWless targets.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_rx_switch_type *rfgsm_nv_get_hl_rx_freq_comp_switch_ptr
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band
);

/*===========================================================================

FUNCTION RFGSM_NV_GET_ENH_RX_SWITCH_PTR                                 

DESCRIPTION
  Returns pointer to GSM enhanced rx switch.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM enhanced rx switch.

SIDE EFFECTS
  None

===========================================================================*/
rfgsm_nv_enh_rx_switch_type *rfgsm_nv_get_enh_rx_switch_ptr
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band
);


/*===========================================================================

FUNCTION RFGSM_NV_PRECOMPUTE_RX_SWITCH_POINTS

DESCRIPTION
  This function computes the Rx switch points for the set of calibrated
  channels (ARFCNs).  It places the switch points in an internal buffer
  to reduce the run-time overhead.

  By adding them to RF_SIGMA_DELTA_MAX_dB, dBfs Min/Max gain range switch point values 
  are converted to dB to resemble RSSI units returned by MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfgsm_nv_precompute_rx_switch_points(rfm_device_enum_type rfm_dev);

/*===========================================================================

FUNCTION RFGSM_NV_PRECOMPUTE_ENH_RX_SWITCH_POINTS

DESCRIPTION
  This function computes the enhanced rx switch points. 
  It places the switch points in an internal buffer
  to reduce the run-time overhead.

  By adding them to RF_SIGMA_DELTA_MAX_dB, dBfs Min/Max gain range switch point values 
  are converted to dB to resemble RSSI units returned by MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfgsm_nv_precompute_enh_rx_switch_points(rfm_device_enum_type rfm_dev);

/*===========================================================================

FUNCTION RFGSM_NV_RX_FREQ_COMP

DESCRIPTION
  This function returns the shadowed copy of the specific NV data item
  for the specified ARFCN.  It determines the band class and applies
  the appropriate frequency compensation values.

DEPENDENCIES
  rfnv_retrieve_nv_items() must have been called apriori to populate the
  rfgsm_nv_xxxx_tbl.  Also, assumes that the ARFCNs are stored in
  ascending order by the RFCAL program.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int16 rfgsm_nv_rx_freq_comp
(
  rfm_device_enum_type rfm_dev,
  rfnv_gsm_lna_state_type rfnv_gsm_lna_state,
  uint8 arfcn_index
);


/*===========================================================================

FUNCTION RFGSM_NV_GET_SPARE_NV

DESCRIPTION
  This function returns the spare_1 NV value.  The default value for this
  item is 0.  Thus, if the NV is not active, a 0 will be returned.

DEPENDENCIES
  None

RETURN VALUE
  spare_1 NV value.

SIDE EFFECTS
  None
===========================================================================*/
uint16 rfgsm_nv_get_spare(void);

/*===========================================================================


FUNCTION rfgsm_nv_get_pa_range_swpts

DESCRIPTION
  This function returns the number of total pa range switchpoints based on
   band and also fills in the pointers passed for pa range switchpoints, 
  predistortion switchpoints, and r0 r1 values from NV.

DEPENDENCIES
  None

RETURN VALUE
  spare_1 NV value.

SIDE EFFECTS
  None
===========================================================================*/
uint16 rfgsm_nv_get_pa_range_swpts(rfgsm_nv_supported_type  band,
                   uint16* pa_range_swpts, 
                   uint16* pred_swpts, 
                   uint16* pa_r0_val, 
                   uint16* pa_r1_val);

/*===========================================================================

FUNCTION rfgsm_nv_precompute_tx_gain_ranges_info

DESCRIPTION
  This function pre-computes the TX switchpoint for linear PA, combining PA 
  switchpoints and pre-distortion switchpoints from NV.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfgsm_nv_precompute_tx_gain_ranges_info (rfm_device_enum_type rfm_dev);


/*===========================================================================

FUNCTION rfgsm_nv_init_amam_ampm_table

DESCRIPTION
  This function initializes the local AMPM table with corresponding values from
  RFNV. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void rfgsm_nv_init_amam_ampm_table(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band);

/*===========================================================================

FUNCTION rfgsm_nv_get_tx_gain_info_ptr                                 

DESCRIPTION
  Returns pointer to GSM TX frequency compensation switchpoints.

DEPENDENCIES
  None

RETURN VALUE
  Returns pointer to GSM RX frequency compensation switch.

SIDE EFFECTS
  None

===========================================================================*/
rfnv_tx_gain_range_info_type *rfgsm_nv_get_tx_gain_info_ptr
(
  rfm_device_enum_type rfm_dev,
  rfcom_gsm_band_type band,
  rfgsm_modulation_type mod_type
);


/*----------------------------------------------------------------------------*/
/*!
  @brief
  

  @details
  
  @rfm_dev         : RFM device for which NV load is required
  @param task_ptr  : Calling task's TCB pointer
  @param task_wait_sig : Task signal to wait for when reading NV.
  @param task_wait_handler : Task's wait function to be called when reading NV 

  @retval
    NV_DONE_S       - request done
    NV_NOTACTIVE_S  - item was not active

*/

rfcommon_nv_status_type rfgsm_nv_load_items(rfm_device_enum_type rfm_dev, rex_tcb_type *task_ptr, 
                                        rex_sigs_type task_wait_sig,
                                 void (*task_wait_handler)( rex_sigs_type ),
                                            uint8 * flag_ptr);


/*===========================================================================

FUNCTION RFGSM_NV_GET_CLOSEST_ARFCN_INDEX

DESCRIPTION
  This function returns the index of the the ARFCN that is closest to the 
  desired ARFCN.

DEPENDENCIES
  Assumes that the ARFCNs are stored in ascending order by the RFCAL program.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

uint8 rfgsm_nv_get_closest_arfcn_index( rfm_device_enum_type rfm_dev, uint16 arfcn);

void rfgsm_nv_cmn_get_sar_value(rfm_device_enum_type rfm_dev, uint8 num_tx_slots, rfm_common_sar_state_type sar_state, rfnv_gsm_sar_unit_type *sar_limit);

/*===========================================================================
  @brief
  This function gets the path delay value that is stored in the NV and offsets
  this with the WTR/RTR specific delay backoff value (from the base delay number) 
  and gives the final path delay that is given to FW. In FTM mode this API 
  allows to directly feed a override value that can be passed to the Modem 
  registers.

  @details
  This function gets the path delay value that is stored in the NV and offsets
  this with the WTR/RTR specific delay backoff value (from the base delay number) 
  and gives the final path delay that is given to FW. In FTM mode this API 
  allows to directly feed a override value that can be passed to the Modem 
  registers.

 
  @param rfm_dev
  RFM_DEVICE0 or RFM_DEVICE2 based on the current device that is being used

  @param delay_backoff
  Delay backoff value corresponding to the different NPLER plan that will
  offset the base delay value for the WTR/RTR paths.

  @param path_delay
  Pointer to the final path_delay variable that is finally updated in this API.

  @retval None
===========================================================================*/
void rfgsm_nv_calc_path_delay(rfm_device_enum_type rfm_dev, uint16 delay_backoff, int16 *path_delay);

/*===========================================================================
  @brief
  This function is to fetch the PA_EN start and stop offset adj values whatever
  is populated in the Tx timing NV items

  @details
  This function is to fetch the PA_EN start and stop offset adj values whatever
  is populated in the Tx timing NV items
 
  @param on_off
  Boolean value to indicate if we want to get the start or the stop adjust
  timing values

  @param timing
  Parameter that is used for getting the PA_EN timing value - start or the
  stop offset adjust values

  @retval None
===========================================================================*/
void rfgsm_nv_get_pa_en_timing (boolean on_off,
                                int16 *timing);

/*===========================================================================
  @brief
  This function is to fetch the Kv parameters (cal channels and the Kv code value
  if it is populated in the NV

  @details
  This function is to fetch the Kv parameters (cal channels and the Kv code value
  if it is populated in the NV

  @param band
  RFGSM band for which the Kv cal paramters are being requested

  @param cal_chan1
  Pointer to the variable where the cal channel1 in NV is returned

  @param cal_chan2
  Pointer to the variable where the cal channel2 in NV is returned

  @param kv_cal_value
  Pointer to the variable where the KV code value in NV is returned
  
  @retval
===========================================================================*/
void rfgsm_nv_get_kv_params (rfcom_gsm_band_type band,
                             uint16 *cal_chan1,
                             uint16 *cal_chan2, 
                             uint32 *kv_cal_value);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting Tx RGI list for a given PA range and mode
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/
boolean
rfgsm_nv_extract_tx_rgi_list
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
);


/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting Tx Pmeas list for a given PA range and mode
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/

boolean
rfgsm_nv_extract_tx_pmeas_data
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting tx AMAM  list for a given Cal channel.
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/

boolean
rfgsm_nv_extract_amam_table
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting tx AMPM list for a given Cal channel.
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param convert_freq_handler
  CB handler to convert channel to frequency
 
  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/

boolean
rfgsm_nv_extract_ampm_table
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
);

/*----------------------------------------------------------------------------*/
/*!
  @brief  
  HELPER API for extracting tx PCL vs pwr err list
 
  @param nv_data_ptr
  Pointer to data to be extracted

  @param num_bytes_available
  The number of nv data bytes available 

  @param extracted_element_size
  Output. The number of bypes extracted for the element

  @param gsm_tx_cal_nv_data
  Output. Destination for the extracted data.
 
  @return 
  TRUE on success and FALSE on failure
 
  @internal
*/

boolean
rfgsm_nv_extract_tx_pwr_err
(
  const void * const nv_data_ptr,
  const uint32 num_bytes_available,
  uint32 * const extracted_element_size,
  gsm_tx_cal_data_nv_type *gsm_tx_cal_nv_data
);

/*----------------------------------------------------------------------------*/


typedef struct
{
 rfgsm_nv_supported_type main_rfnv_band;
 rfgsm_nv_supported_type rfnv_band[RFCOM_MAX_NUM_SPLIT_PARTS_PER_BAND];
 uint8 split_band_index;  // For a given band enum Split band index 0 represents the original band enum, 1 represents next split band enum
}rfgsm_coex_band_map_type;

/*----------------------------------------------------------------------------*/
/*!
  @brief
  Free the memory allocated for NV structures.

  @details
  Free the memory allocated for NV structures. 
*/
void rfgsm_nv_deinit(rfm_device_enum_type rfm_dev);

uint16 rfgsm_debug_nv_get_bits(rfm_device_enum_type rfm_dev,uint8 i, uint8 num_bits);
uint32 rfgsm_debug_nv_get_word(rfm_device_enum_type rfm_dev,uint8 index);
uint32 rfgsm_get_intf_debug_nv(rfm_device_enum_type rfm_dev, uint8 index );

boolean rfgsm_nv_check_debug_nv ( rfm_device_enum_type rfm_dev, rfgsm_debug_nv_supported_type debug_nv );


/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function is used to return the NV container for a given band, logical device and RX/TX

  @details
  
  @param rf_chain
  The RFM device for which GSM NV path is required
  Band
  RX/TX
 
  @return
  GSM NV container for passed RFM device
*/


rfgsm_nv_path_type rfgsm_nv_get_nv_container(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band, rfgsm_nv_rxtx_enum_type rx_tx);
rfgsm_nv_path_type rfgsm_nv_get_cmn_nv_container(rfm_device_enum_type rfm_dev);
rfgsm_nv_container_array_type rfgsm_nv_get_tx_rx_nv_container(rfm_device_enum_type rfm_dev, rfcom_gsm_band_type band);

void rfgsm_nv_populate_coex_band_map_tbl(void);

boolean rfgsm_nv_get_coex_band_map_tbl(rfgsm_nv_supported_type nv_band, rfgsm_coex_band_map_type *band_info_ptr);


boolean rfgsm_nv_load_tx_rx_char_offset_arfcn_tbl
(  
  gsm_char_offset_arfcn_tbl_type* static_nv_struct,
  rfnv_item_id_enum_type arfcn_tbl_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);



boolean rfgsm_nv_load_coex_tx_char_offset_info_tbl
(  
  gsm_coex_tx_char_offset_info_type *static_nv_struct,
  rfnv_item_id_enum_type tx_char_offset_info_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);


boolean rfgsm_nv_load_coex_rx_char_offset_info_tbl
(  
  gsm_coex_rx_char_offset_info_type* static_nv_struct,
  rfnv_item_id_enum_type rx_char_offset_info_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
);


boolean rfgsm_nv_load_cal_temp
(
  gsm_cal_temp_data_type* static_nv_struct,
  rfnv_item_id_enum_type cal_therm_read_data_item,
  rfgsm_nv_supported_type band,
  rex_tcb_type* caller_tcb_ptr,
  rex_sigs_type task_nv_wait_sig,
  void (* task_wait_func_ptr)( rex_sigs_type )
); 


void rfgsm_nv_data_structure_init(void);

void rfgsm_nv_sort_rx_cal_data
(
  gsm_rx_cal_data_nv_type* rx_cal_nv_data,
  rfcom_gsm_band_type band,
  rfnv_item_size_type gsm_rx_cal_nv_read_size
);


#ifdef __cplusplus
}
#endif

#endif /* RFGSM_NV_H */
