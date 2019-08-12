#ifndef RFWCDMA_MDSP_H
#define RFWCDMA_MDSP_H

/*! @brief
  @file

  @brief
  This module contains the definitions of the WCDMA RF-mDSP AGC interface.
*/

/*===========================================================================


  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_wcdma/api/rfwcdma_mdsp.h#1 $
  $DateTime: 2016/12/13 07:59:54 $
  $Author: mplcsds1 $

when       who     what, where, why 
--------   ---     ---------------------------------------------------------- 
01/08/15   sd      Remove deprecated API rfwcdma_mdsp_get_lna_range()
11/17/14   rmb     Remove the wcdma core rxctl header from here.
11/05/14   sd      Main line feature FEATURE_WCDMA_DC_HSUPA_FRAMEWORK
08/25/14   rmb     Fix KW Error and CW on Bolt.
07/08/14   ak      Changing the LNA states to 6. Same Enum value is used to populate the rise/fall switch points.
07/31/14   ak      Correcting the actual LNA state to prevent out of bound array access while building LNA trigger scripts.
07/30/14   ac      OFT error fix
05/13/14   vbh     WFW RF interface cleanup
                   [1] Move needed enum definitions from the wfw_rf_intf.h file
                   [2] Remove unused api definitions
04/20/14   bn      WCDMA 8 PA state support and related cleanup
04/03/14   dw      Clean up obsolete functions
03/26/14   rmb     Remove unsued W2W API.
02/21/14   rmb     Add global NV pointers for each of band specific static NVs.
02/13/14   dw      [1]Change AGC command function to support multiple device
                   [2]Code cleanup
01/08/14   kai     Remove ET chain txlm buffer allocation
12/17/13   kr      added car_id support to rfwcdma_mdsp_is_tx_accum_frozen()
09/10/13   dw      Remove DBDC featurization and code clean up.
08/30/13   vs      Modifying MDSP interface to make common across DI/BO/TR
08/29/13   gh      Add support for dynamic notcc filter for Triton
08/20/13   dw      Remove FEATURE_BOLT_MODEM featurization 
08/10/13   aa      DBDC CM
08/07/13   ry      Enable RGI based phase compensation feature
08/01/13   dw      de-featurize rfwcdma_mdsp_device_to_lna_id
07/24/13   ac      DBDC bringup code cleanup and changes
07/19/13   rmb     Remove XPT initialization from band sensitive ctl()
                   and add it in rfwcdma_mdsp_init_xpt_params() API.
07/16/13   aa      Multiple LNA transaction support
07/08/13   sma     Modify rxlm dynamic update command
06/24/13   ac      tx freezeaccum api change for both car
06/23/13   kai     Added WCDMA 16channels xpt delay support
05/22/13  ka/ac    dc hsupa changes
05/06/13   tks     Added support for rxlm dynamic update command 
05/01/13   rmb     Added rfwcdma_mdsp_enable_qtf_tuner_support() API to enable QTF Tuner support in MDSP FW.
05/01/13   jmf     Implement xPT IQ Capture SWI change
04/22/13   jmf     Implement xPT processing Enum Type
04/19/13   aa      Updated SBI bus info in LNA state
04/01/13   bn      MDSP set QPOET register address in EPT/ET mode
04/01/13   ac      avoid race condition for multiple interrupts to be sent to the same event 
03/26/13   rmb/kai Changed the type of max_pa_state from uint8 to int8 in rfwcdma_mdsp_update_max_pa_state().
03/21/13   ac      tx_agc change to take pa_up/down and no pa update
03/19/13   dw/kai  Added support for HDET based linearizer compensation
03/10/13   ac      compiler warning fix
03/08/13   ac      event script and init cmd cleanup
03/07/13   bn      XPT TxAGC linearizer update
03/01/13   kai     Removed old DPD feature 
02/12/13   jmf     Modify IQ sample capture fn to take more params
02/07/13   ac      removing the ici grp dynamic mask func to hal
02/06/13   rmb     Added support for wakeup optimization.
04/02/13   ac      HS rxlm state update
01/30/13   ka      Updated FTM interface
01/21/13   rmb     AGC logging fix which now shows the actual RxAGC value by using rfwcdma_mdsp_get_rxagc_status()
                   but we still use rfwcdma_mdsp_get_rxagc_enable_status() for RF Wakeup/Sleep.
12/11/12   dw      Add support for DTx and DRx/LPM.
12/11/12   ac      pa_ range event builder support, pa q current as a txagc task
11/16/12   ac      hdet event builder support and size adjustment for the task
11/15/12   kai     Add initial support for configure ET TxLM and update swpts
11/14/12   dw      Add APT support for Dime
11/09/12   kai     Added disable_txagc_update
11/02/12   dw      [1] Add support for Dual Carrier on Dime
                   [2] Remove RxAGC enable flag
11/1/12    ac      tx_on/off event building
10/30/12   ac      pa_on/off event building and warning fix
10/29/12   aa      Disable CPC to make room for MEAS event
10/24/12   bn      Enable W sample capture for XPT
10/22/12   ac      ustmr unit parameter fix
10/18/12   dw      Fix TxAGC logging
10/16/12   ac      rxd change for rxagc update
10/12/12   ac      removing the timing from tx_event
10/06/12   ac      reverting the change , Wl1 submit once Wli dependency is added to correct branch
10/06/12   ac      rxd change for rxagc update
10/05/12   aa      update script size for wtow measurement
09/17/12   ka      update default BTF
09/12/12   ac      diversity rxagc bringup
09/10/12   ka      Tx event updates
09/05/12   vbh     Fix on target compiler warnings
08/31/12   ac      optimised the script size which was earlier increased, ran rf cal successfully
08/30/12   ac      moving max allocation per event out of helper header since it will be removed from sw side
08/24/12   ac      wait till commit is done before the start of next
08/07/12   ac      to convert chipx8 to ustmr units
07/26/12   tks     Added rfwcdma_mdsp_update_rx_events  
07/17/12    ka     Added rfwcdma_mdsp_update_tx_events 
07/11/12   shb     Updated buffer_ptr to be of rf_buffer_intf* type instead 
                   of void*
07/03/12   tks     Updated PA range map function to support MMPA
06/25/12   ac      script update for static events for ccs
06/22/12   anr     EPT Calibration Support
06/06/12   cri     FW Interface updates for EPT
06/05/12   anr     Initial EPT Support
06/04/12   cri     Add EPT support for WCDMA
05/01/12   cri     Updated EPT linearizer structure and processing
05/08/12   tks     Modified CPC DRx SSBI function prototype  
04/17/12   gh      Add support for TX_GTR_THRESH
03/26/12   av      IRAT code cleanup for W and G. Remove rfdevice_meas.
03/01/01   dw      Add support for HSPA APT
03/07/12   cri     Added initial DPD support 
03/06/12   kai     Added API for setting Tx datapath IQ gain
02/27/11   dw      Export rfwcdma_mdsp_get_rxagc_status()
02/24/12   tks     Modified function prototype for CPC DRX pll function 
02/14/12   swb     Added API for FTM Tx Frequency Adjustment
01/23/12   dw      Added rfwcdma_mdsp_update_mtpl()
01/17/12   kg      Support for QPA state switch in online mode
01/12/12   dw      Mainline FEATURE_RF_CPC_DRX
12/08/11   dw      populate TxAGC gain SSBI timing to FW
12/01/11   dw      Add support for WCDMA low power mode (DSR)
11/16/11   dw      Change to update LUT buffer index after LUT for all PA ranges are updated
10/13/11   swb     Added API for freezing Rx AGC accumulator
09/30/11   dw     [1] New Tx linearizer load functions
                  [2] Temp comp support
                  [3] MDSP Linearizer double buffer implementation
                  [4] Legacy code clean up
08/19/11   dw      Add support to get FW SSBI bus from RFC
08/11/11   vb      Support for new multi-chan linearizer/APT
08/11/11   vb      Removed old function rfwcdma_mdsp_load_linearizers()
08/02/11   ckl     Add RFWCDMA_MDSP_BPG_TIME definition.
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
06/23/11   dw      Remove rfwcdma_mdsp_set_vga_gain_offset_dual_carrier()
04/14/11   dw      Add rfwcdma_mdsp_get_rxagc_sbi_packet()
04/20/11   jhe     Delete unused api
04/06/11   ad      Port APT changes from MDM9k
03/22/11   dw      RUMI updates  
03/02/11   dw      Nikel updates
02/25/11   ac/dw   merge cpc drx support    
02/22/11   dw      Include RxLM buffer index in AGC cmd
02/17/11   dw      Add function to update beta gain
02/09/11   dw      Add support for agc cmd control
10/26/10   ka      CPC DTx support 
08/31/10   ad      compiler warning fix
05/28/10   ac      compiler warning fix
03/09/10   kguo    Added prototype for rfwcdma_mdsp_set_tx_rotator_angle
02/12/10   ka      Remove direct mdsp access from core layer.
02/03/10   ad      Add LNA offset for G5
01/27/10   ka      Remove direct mdsp access from device layer.
01/12/10   kguo    Merged from mdm9k dev branch to main\latest
12/02/09   ac      DC RxAGC limits for LNA gain states 
11/11/09   ka      Updates for dual carrier.
10/22/09   ad      Bring up changes 
08/14/09    ka     Added constant for baseband gain.
03/31/09   ckl     Deleted SCMM_MDSP_SUPPORT_READY feature
03/19/09   ckl     Delete input parameter for rfwcdma_mdsp_init_pa_ctl function
03/18/09   ckl     Added missing function proto types and deleted unnecessary
                   function proto types.
03/17/09   ckl     Comment out QDSP6 interface
10/30/08   xw      Initial Revision.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "rfa_variation.h"
#include "comdef.h"
#include "target.h"
#include "rfcom.h"
#include "wfw_mod_intf.h"
#include "wfw_drx_intf.h"
#include "rfumts.h"
#include "rfnv_wcdma.h"
#include "wfw_rxagc_intf.h"
#include "rfcommon_core_txlin.h"
#include "rflm_ccs_irat_intf.h"
#include "wfw_misc_intf.h"
#include "rfcommon_locks.h"
#include "rfcommon_msg.h"
/*===========================================================================

                        CONSTANT DEFINITIONS

===========================================================================*/
/* BTF delay is in 1/8 chips unit, 220/8 = 27.5 chips */
#define RFMDSP_AGC_BTFDELAY_VAL     -294

#define RFMDSP_AGC_RX_DELAY_VAL     -2

#define RFWCDMA_MDSP_TX_DIG_IQ_GAIN 0x8E7

/* BPG Time in mDSP, 1BPG = 66.67usec */
#define RFWCDMA_MDSP_BPG_TIME 67

/* Max possible LNA states */
#define RFWCDMA_MAX_LNA_GAIN_STATE 6  

/* Phase Resolution reduction factor */
#define RFWCDMA_ROT_ANGLE_RESOLUTION_REDUCTION_FACTOR 4 

/* 7-bit RF-FW interface limiting factor for ROT Angle */
#define RFWCDMA_FW_INTF_LIM_FACTOR 0x7F

/* Tx to Rx frequency ratios in Q15 */
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC1           29858
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC2           31430
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC3           31078
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC4           26621
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC5           31095
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC19          31097
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC8           31203
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC9           31096
#define RFAGC_TX_TO_RX_FREQ_RATIO_BC11          31709

#define WFW_RX_INIT_MAX_SIZE 45
#define WFW_RX_SLEEP_MAX_SIZE 14
#define WFW_RX_TUNE_MAX_SIZE 42
#define WFW_RX_SELECT_BW_MAX_SIZE 0
#define WFW_RX_SET_BAND_MAX_SIZE 0 
#define WFW_TX_INIT_MAX_SIZE 50
#define WFW_TX_SET_BAND_MAX_SIZE 0
#define WFW_TX_TUNE_MAX_SIZE 68
#define WFW_TX_DISABLE_MAX_SIZE 18
#define WFW_READ_THERM_MAX_SIZE 10
#define WFW_WRITE_HDET_MAX_SIZE 50
#define WFW_RF_SET_PA_RANGE_MAX_SIZE 40 
#define WFW_SET_RX_LPM_MAX_SIZE 15
#define WFW_SET_TX_PLL_MAX_SIZE 15
#define WFW_RF_SET_TX_AGC_PA_UP_MAX_SIZE 65 
#define WFW_RF_SET_TX_AGC_PA_DOWN_MAX_SIZE 65
#define WFW_RF_SET_TX_AGC_NO_PA_UPDATE_MAX_SIZE 40
#define WFW_RX_AGC_MAX_SIZE 15
#define WFW_TX_ON_MAX_SIZE 15
#define WFW_TX_OFF_MAX_SIZE 15
#define WFW_PA_ON_MAX_SIZE 15
#define WFW_PA_OFF_MAX_SIZE 15
#define WFW_RX_AGC_RXD_MAX_SIZE 15
#define WFW_RX_AGC_MEAS_MAX_SIZE 15
#define WFW_RF_W2W_H1_MAX_SIZE 200
#define WFW_RF_W2W_N1_MAX_SIZE 200
#define WFW_RF_W2W_H2_MAX_SIZE 200
#define WFW_RF_W2W_N2_MAX_SIZE 200

#define WFW_MOD_NUM_MAX_PA_STATES 4
#define WFW_NUM_PA_ON 2
#define WFW_NUM_PA_OFF 2
#define WFW_NUM_TX_ON 2
#define WFW_NUM_TX_OFF 2

#define WFW_RF_EVENT_MAX 19
#define WFW_RF_MEAS_EVENT_MAX 10
#define WFW_RF_W2W_H1 WFW_RF_EVENT_MAX
#define WFW_RF_W2W_N1 (WFW_RF_EVENT_MAX+1)
#define WFW_RF_W2W_H2 (WFW_RF_EVENT_MAX+2)
#define WFW_RF_W2W_N2 (WFW_RF_EVENT_MAX+3)

/*===========================================================================

                        TYPE DECLARATIONS

===========================================================================*/
typedef struct
{
  int16    rxagcmin;
  int16    rxagcmin2;
  int16    rxagcmin3;
  int16    rxagcmin4;
  int16    rxagcmin5;  
  int16    rxagcmax;
} rfagc_agc_limit_params_type;

/*! @name Structure definition for WCDMA AGC interface */
/*!@{*/

/*! @brief Rx AGC parameters */

typedef struct
{
  /*! @brief LNA range polarity */
  uint8    lnarangepol;

  /*! @brief LNA rise value from G0 to G1 */
  int32    lnarise1;

  /*! @brief LNA fall value from G1 to G0 */
  int32    lnafall1;

  /*! @brief Nonbypas timer */
  uint16   nonbypasstimer;

  /*! @brief Bypass timer */
  uint16   bypasstimer;

  /*! @brief LNA offset between G1 and G2 */
  int16    lnaoffset1;

  /*! @brief LNA phase offset */
  uint8    lnaphaseoffset;

  /*! @brief LNA rise value from G1 to G2 */
  int32    lnarise2;

  /*! @brief LNA rise value from G2 to G3 */
  int32    lnarise3;

  /*! @brief LNA rise value from G3 to G4 */
  int32    lnarise4;

  /*! @brief LNA rise value from G3 to G4 */
  int32    lnarise5;

  /*! @brief LNA fall value from G2 to G1 */
  int32    lnafall2;

  /*! @brief LNA fall value from G3 to G2 */
  int32    lnafall3;

  /*! @brief LNA fall value from G4 to G3 */
  int32    lnafall4;

  /*! @brief LNA fall value from G4 to G3 */
  int32    lnafall5;

  /*! @brief IM level between G0 and G1*/
  int16    imlevel;

  /*! @brief IM level between G1 and G2  */
  int16    imlevel2;

  /*! @brief IM level between G2 and G3  */
  int16    imlevel3;

  /*! @brief IM level between G3 and G4  */
  int16    imlevel4;

  /*! @brief LNA offset for G2  */
  int16    lnaoffset2;

  /*! @brief LNA offset for G3  */
  int16    lnaoffset3;

  /*! @brief LNA offset for G4  */
  int16    lnaoffset4;

  /*! @brief LNA offset for G5  */
  int16    lnaoffset5;

  /* HS parameters  */

  /*! @brief HS LNA rise value from G0 to G1 */
  int32    hslnarise1;

  /*! @brief HS LNA fall value from G1 to G0 */
  int32    hslnafall1;

  /*! @brief HS LNA rise value from G1 to G2 */
  int32    hslnarise2;

  /*! @brief HS LNA rise value from G2 to G3 */
  int32    hslnarise3;

  /*! @brief HS LNA rise value from G3 to G4 */
  int32    hslnarise4;

  /*! @brief HS LNA rise value from G4 to G5 */
  int32    hslnarise5;

  /*! @brief HS LNA fall value from G2 to G1 */
  int32    hslnafall2;

  /*! @brief HS LNA rise value from G3 to G2 */
  int32    hslnafall3;

  /*! @brief HS LNA rise value from G4 to G3 */
  int32    hslnafall4;

  /*! @brief HS LNA rise value from G5 to G4 */
  int32    hslnafall5;

  /*! @brief HS IM level between G0 and G1  */
  int16    hsimlevel;

  /*! @brief HS IM level between G1 and G2  */
  int16    hsimlevel2;

  /*! @brief HS IM level between G2 and G3  */
  int16    hsimlevel3;

  /*! @brief HS IM level between G3 and G4  */
  int16    hsimlevel4;

} rfwcdma_agc_lna_params_type;

/*! @brief LNA Rise and Fall parameters */

typedef struct
{
  /*! @brief LNA rise value from G0 to G1 */
  int32    lnarise1;

  /*! @brief LNA fall value from G1 to G0 */
  int32    lnafall1;

  /*! @brief LNA rise value from G1 to G2 */
  int32    lnarise2;

  /*! @brief LNA fall value from G2 to G1 */
  int32    lnafall2;

  /*! @brief LNA rise value from G2 to G3 */
  int32    lnarise3;

  /*! @brief LNA fall value from G3 to G2 */
  int32    lnafall3;

  /*! @brief LNA rise value from G3 to G4 */
  int32    lnarise4;

  /*! @brief LNA fall value from G4 to G3 */
  int32    lnafall4;

  /*! @brief LNA rise value from G4 to G5 */
  int32    lnarise5;

  /*! @brief LNA fall value from G5 to G4 */
  int32    lnafall5;
} rfwcdma_agc_lna_range_thresh_type;

/*! @brief PA Rise and Fall parameters */

typedef struct
{
  /*! @brief PA rise threshold from R0 to R1 */
  int32    parise1;

  /*! @brief PA fall threshold from R1 to R0 */
  int32    pafall1;

  /*! @brief PA rise threshold from R1 to R2 */
  int32    parise2;

  /*! @brief PA fall threshold from R2 to R1 */
  int32    pafall2;

  /*! @brief PA rise threshold from R2 to R3 */
  int32    parise3;

  /*! @brief PA fall threshold from R3 to R2 */
  int32    pafall3;

  /*! @brief PA rise threshold from R0 to R1 in RACH */
  int32    prach_parise1_offset;

  /*! @brief PA fall threshold from R1 to R0 in RACH */
  int32    prach_pafall1_offset;

  /*! @brief PA rise threshold from R1 to R2 in RACH */
  int32    prach_parise2_offset;

  /*! @brief PA fall threshold from R2 to R1 in RACH */
  int32    prach_pafall2_offset;

  /*! @brief PA rise threshold from R2 to R3 in RACH */
  int32    prach_parise3_offset;

  /*! @brief PA fall threshold from R3 to R2 in RACH */
  int32    prach_pafall3_offset;
} rfwcdma_agc_pa_range_thresh_type;

/*! @brief LNA Offset parameters */

typedef struct
{
  /*! @brief LNA offset for G0  */
  int16    lnaoffset0;

  /*! @brief LNA offset for G1  */
  int16    lnaoffset1;

  /*! @brief LNA offset for G2  */
  int16    lnaoffset2;

  /*! @brief LNA offset for G3  */
  int16    lnaoffset3;

  /*! @brief LNA offset for G4  */
  int16    lnaoffset4;

  /*! @brief LNA offset for G5  */
  int16    lnaoffset5;
} rfwcdma_agc_lna_offset_type;

/*! @brief Compressed Mode Rx CGAGC parameters */

typedef struct
{
  /*! @brief  Initial RSSI value for the LNA state 0  */
  uint16    rxagc_val_cm_init0;

  /*! @brief  Initial RSSI value for the LNA state 1  */
  uint16    rxagc_val_cm_init1;

  /*! @brief  Initial RSSI value for the LNA state 2  */
  uint16    rxagc_val_cm_init2;

  /*! @brief  Initial RSSI value for the LNA state 3  */
  uint16    rxagc_val_cm_init3;

  /*! @brief  Initial RSSI value for the LNA state 4  */
  uint16    rxagc_val_cm_init4;

  /*! @brief  Initial RSSI value for the LNA state 5  */
  uint16    rxagc_val_cm_init5;

  /*! @brief The threshold to switch from G1 to G0 in compressed mode */
  uint32    rxagc_cm_lnafall_1;

  /*! @brief The threshold to switch from G2 to G1 in compressed mode */
  uint32    rxagc_cm_lnafall_2;

  /*! @brief The threshold to switch from G0 to G1 in compressed mode */
  uint32    rxagc_cm_lnarise_1;

  /*! @brief The threshold to switch from G1 to G2 in compressed mode */
  uint32    rxagc_cm_lnarise_2;

  /*! @brief The threshold to switch from G1 to G2 in compressed mode */
  uint32    rxagc_cm_lnarise_3;
 
} rfwcdma_cgagc_param_type;

/*! @brief HSDPA/HSPA PA Rise and Fall parameters */

typedef struct
{
  /*! @brief PA rise threshold from R0 to R1 in HS mode */
  int32    hsparise1;

  /*! @brief PA fall threshold from R1 to R0 in HS mode */
  int32    hspafall1;

  /*! @brief PA rise threshold from R1 to R2 in HS mode */
  int32    hsparise2;

  /*! @brief PA fall threshold from R2 to R1 in HS mode */
  int32    hspafall2;

  /*! @brief PA rise threshold from R2 to R3 in HS mode */
  int32    hsparise3;

  /*! @brief PA fall threshold from R3 to R2 in HS mode */
  int32    hspafall3;
} rfwcdma_hs_pa_range_thresh_type;

#if  defined(FEATURE_TRITON_MODEM) 
typedef struct
{
  wfwRxAgcSbiPacketStruct sbi_info_lna_state0;
  wfwRxAgcSbiPacketStruct sbi_info_lna_state1;
  wfwRxAgcSbiPacketStruct sbi_info_lna_state2;
  wfwRxAgcSbiPacketStruct sbi_info_lna_state3;
  wfwRxAgcSbiPacketStruct sbi_info_lna_state4;
  wfwRxAgcSbiPacketStruct sbi_info_lna_state5;
} rfwcdma_mdsp_lna_sbi_info_type;
#endif

typedef struct
{
  rfcom_wcdma_band_type band;
  tx_linearizer_table_data_type tx_cal_data;
  uint8 pa_index;
  uint16 valid_lin_entries;
  uint16 *valid_dpd_entries;
  uint32 *dpd_config_params;
  boolean  mdsp_lut_buffer_idx;
  boolean hs_lin_enable; 
} rfwcdma_mdsp_load_linearizer_param_type;

typedef struct
{
 uint8 paCtrlSbiSel;
 uint8 paOnOffSbiAddr;
 uint8 paOnSbiData;	
 uint8 paOffSbiData;
}rfwcdma_get_pa_ctl_type;

typedef enum
{
  LNA_ID__0 = 0,
  LNA_ID__1 = 1,
  LNA_ID__2 = 2,
  LNA_ID__INVALID
} rfwcdma_mdsp_lna_id_type;

typedef enum
{
  LPM_ID__0 = 0,
  LPM_ID__1 = 1,
  LPM_ID__2 = 2,
  LPM_ID__INVALID
} rfwcdma_mdsp_lpm_id_type;
/*===========================================================================

                              FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
                       General AGC Functions
===========================================================================*/
void rfwcdma_mdsp_init_agc_for_ftm_mode(void);

void rfwcdma_mdsp_init(void);

void rfwcdma_mdsp_init_agc_ctl(void);

void rfwcdma_mdsp_set_itm_packet(rfcom_wcdma_band_type band,rfcom_device_enum_type device,rfcom_multi_carrier_hspa_id_type multi_car_type);

void rfwcdma_mdsp_set_btf_delay(int16 val);

void rfwcdma_mdsp_sync_send_rf_cfg_cmd( uint32 event_id,uint32 offset);

void rfwcdma_mdsp_sync_send_rf_init_cmd(uint32 event_id,uint32 offset);


/*===========================================================================
                        Rx AGC Functions
===========================================================================*/

void rfwcdma_mdsp_init_band_sensitive_ctl(rfcom_wcdma_band_type band,rfcom_device_enum_type device,rfcom_multi_carrier_hspa_id_type carrier_mode);

int16 rfwcdma_mdsp_get_rxagc_val
(
  rfagc_receive_chain_type rx_chain,
  rfcom_rxagc_type rxagc_type,
  rfcom_multi_carrier_id_type car_id,
  rfcom_rxagc_unit_type rxagc_unit
);

void rfwcdma_mdsp_set_rx_freeze_accum(rfcom_device_enum_type device, boolean freeze );

void rfwcdma_mdsp_agc_init_lna_ctl(rfcom_device_enum_type device);

void rfwcdma_mdsp_set_lna_offset( rfcom_device_enum_type device, rfwcdma_agc_lna_offset_type offset );

rfwcdma_mdsp_lna_id_type rfwcdma_mdsp_device_to_lna_id( rfcom_device_enum_type device );

void rfwcdma_mdsp_set_hs_lna_params(rfcom_wcdma_band_type band);

void rfwcdma_mdsp_set_hs_lna_dual_carrier_params(void );

void rfwcdma_mdsp_set_lna_range_thresh( rfagc_receive_chain_type rx_chain, rfwcdma_agc_lna_range_thresh_type val );

void rfwcdma_mdsp_set_lna_range_thresh_dual_carrier( rfagc_receive_chain_type rx_chain, rfwcdma_agc_lna_range_thresh_type val );

void rfwcdma_mdsp_set_agc_threshold_val(rfcom_wcdma_band_type rfi_curr_band);


/*===========================================================================
                        Tx AGC Functions
===========================================================================*/
void rfwcdma_mdsp_init_pa_ctl(void);

int16 rfwcdma_mdsp_get_txagctablestore(rfcom_multi_carrier_id_type car_idx);

void rfwcdma_mdsp_set_pa_range_update_time( uint16 pa_up_time, uint16 pa_down_time );

void rfwcdma_mdsp_set_pa_compensation
(
  int16 rfagc_pa_compensate_up,
  int16 rfagc_pa_compensate_dn,
  int16 rfagc_pa_compensate_up2,
  int16 rfagc_pa_compensate_dn2,
  int16 rfagc_pa_compensate_up3,
  int16 rfagc_pa_compensate_dn3
);

int16 rfwcdma_mdsp_get_tx_agc_value(void);

int16 rfwcdma_mdsp_get_hdet_tx_agc_value(void);

uint8 rfwcdma_mdsp_get_pa_range(void);

int16 rfwcdma_mdsp_get_tx_power_limit(void);

void rfwcdma_mdsp_enable_pwr_ctrl(void);

void rfwcdma_mdsp_set_txfreezeaccum( uint8 val,rfcom_multi_carrier_id_type car_idx );

uint8 rfwcdma_mdsp_is_tx_accum_frozen(rfcom_multi_carrier_id_type car_id);

void rfwcdma_mdsp_init_tx_agc_value( int16 val );

void rfwcdma_mdsp_invert_pa_range_polarity(void);
#if 0
void rfwcdma_mdsp_init_pa_map( int8 pa_range_map[], rfcom_wcdma_band_type band );
void rfwcdma_mdsp_set_pa_range_thresh(rfcom_wcdma_band_type band);
void rfwcdma_mdsp_set_tx_rotator_angle(rfcom_wcdma_band_type band);
#endif
void rfwcdma_mdsp_set_tx_agc_update_ssbi_time(int16 agc_ssbi_time);

void rfwcdma_mdsp_set_tx_agc_time(int16 tx_agc_time);

void rfwcdma_mdsp_set_pa_current_time(int32 pa_current_time);

void rfwcdma_mdsp_set_vga_gain_offset( rfcom_device_enum_type device, int16 gainoffset );


int16 rfwcdma_mdsp_get_iqgain_value(void);

void rfwcdma_mdsp_set_iqgain_value(int16 iqGain);

boolean rfwcdma_mdsp_load_linearizer_table( rfwcdma_mdsp_load_linearizer_param_type* mdsp_load_lin_param);

void rfwcdma_mdsp_write_tx_agc_sbi_ctl(rfcom_wcdma_band_type band);

void rfwcdma_mdsp_set_arm_to_mdsp_hdet_disable_flag(boolean hdet_is_disabled);

void rfwcdma_mdsp_clear_hdet_contention_flags(void);

boolean rfwcdma_mdsp_read_hdet_started_flag(void);

void rfwcdma_mdsp_set_mpr_beta_table(uint8 i, uint8 mpr, uint16 beta_backoff, int8 tx_agc_offset);

void rfwcdma_mdsp_set_pa_thresh_offset(uint8 i, uint8 offset);

void rfwcdma_mdsp_set_hdet_interval(uint16 interval);

void rfwcdma_mdsp_set_tx_lower_limit(int16 limit);

int16 rfwcdma_mdsp_get_matched_lut_index(void);

int16 rfwcdma_mdsp_get_envscale_value(void);

void rfwcdma_mdsp_send_sapt_iq_cap_cmd(uint32 first_trig,
                                       uint32 last_trig,
                                       uint8  capt_buf_id,
                                       uint8  proc_buf_id
                                       );

void rfwcdma_mdsp_set_ept_specific_params( boolean eptOnOff );

boolean rfwcdma_mdsp_get_rxagc_cmd_status(void);

void rfwcdma_mdsp_send_lpm_agc_cmd( rfcom_lpm_config_enum_type lpm_config, 
                                    uint32 c0_rxlm_buf_idx,
                                    uint32 c1_rxlm_buf_idx) ;

void rfwcdma_mdsp_update_beta_gain(uint16 beta);

void rfwcdma_mdsp_set_band_tx_delays
(
  uint16 tx_on_rise_delay,
  uint16 tx_on_fall_delay,
  uint16 pa_on_rise_delay,
  uint16 pa_on_fall_delay
);
#if  defined(FEATURE_TRITON_MODEM)
void rfwcdma_mdsp_get_rxagc_sbi_packet(rfcom_wcdma_band_type band,
                                  rfcom_multi_carrier_hspa_id_type carrier_mode,
                                  uint8 lna_state
                                  );
#else
void rfwcdma_mdsp_get_rxagc_sbi_packet(
                                  rfcom_device_enum_type device,
                                  rfcom_wcdma_band_type band,
                                  uint16 chan,
                                  rfcom_multi_carrier_hspa_id_type carrier_mode
                                  );
#endif
void rfwcdma_mdsp_update_linearizer_buffer_index(uint8 lut_idx, uint8 pa_state);
void rfwcdma_mdsp_update_mtpl(int16 mtpl);
void rfwcdma_mdsp_set_freq_adjust(int32 TxFreqError);
void rfwcdma_mdsp_set_xoslamflag(uint8 xoSlamFlag);
void rfwcdma_mdsp_set_hs_lin_agc_threshold(int16 start_power, int16 stop_power);
void rfwcdma_mdsp_update_max_pa_state(int8 max_pa_state);
uint32 rfwcdma_mdsp_get_rxagc_enable_status
(
rfagc_receive_chain_type rx_chain,
rfcom_multi_carrier_id_type car_id
);

/*! @brief WCDMA FW RF Tx PLL control Type (ON/OFF) */
typedef enum
{
  RF_MDSP_TX_PLL_ON,
  RF_MDSP_TX_PLL_OFF,
  RF_MDSP_TX_PLL_NUM,
  RF_MDSP_TX_PLL_MAX = RF_MDSP_TX_PLL_NUM
} rfwcdma_mdsp_pll_ctl_type; 

typedef enum
{
  RF_MDSP_RX_LPM_ON,
  RF_MDSP_RX_LPM_OFF,
  RF_MDSP_RX_LPM_CTL_NUM,
  RF_MDSP_RX_LPM_CTL_MAX = RF_MDSP_RX_LPM_CTL_NUM
} rfwcdma_mdsp_rx_lpm_ctl_type;

void rfwcdma_mdsp_set_tx_gtr_thresh_params
( uint8 active,
  int16 rise_value,
  int16 fall_value,
  uint8 index,
  int32 grfc_engine_num
);

int32
rfwcdma_mdsp_convert_chipx8_to_ustmr_units
(
  int32 time_chipx8
);

void rfwcdma_mdsp_update_tx_agc_event
(  
   uint32 event_id,
   rf_buffer_intf *buffer,
   rfcom_wcdma_band_type band
);

void rfwcdma_mdsp_tx_dyn_init_cmd(void);

void rfwcdma_mdsp_update_hdet_write_event
(
  rf_buffer_intf* buffer, 
  rfcom_wcdma_band_type band
);

void rfwcdma_mdsp_update_rx_events
(
  rf_buffer_intf* buffer, 
  rfcom_wcdma_band_type band,
  rfcom_device_enum_type device,
  rfcom_multi_carrier_hspa_id_type num_chan
);

boolean rfwcdma_mdsp_disable_txagc_updates(boolean disable);
uint16 rfwcdma_mdsp_get_pa_bias_val(void);
boolean rfwcdma_mdsp_is_pa_on(void);

void rfwcdma_mdsp_update_rx_lpm_event
(
   rf_buffer_intf *buffer,
   rfcom_wcdma_band_type band,
   rfcom_device_enum_type device
);
uint32 rfwcdma_mdsp_get_rxagc_status
(
rfagc_receive_chain_type rx_chain,
rfcom_multi_carrier_id_type car_id
);

void rfwcdma_mdsp_set_lna_gain_state(rfcom_device_enum_type device, uint8 state);

void rfwcdma_mdsp_set_lna_rxagcmin(rfcom_device_enum_type device, uint8 lna_state, uint16 rxagc_min);

void rfwcdma_mdsp_set_single_lna_offset(rfcom_device_enum_type device, uint8 index, int16 offset);

int16 rfwcdma_mdsp_get_raw_rx_agc(rfcom_device_enum_type device);

int16 rfwcdma_mdsp_get_lna_offset(rfcom_device_enum_type device, uint8 index);

void rfwcdma_mdsp_populate_xpt_mode(tx_linearizer_aux_data_type aux_lin_data,
                                  uint8 pa_index);

boolean rfwcdma_mdsp_populate_dpd(uint8 xpt_index,
                             rfcommon_xpt_dpd_bank_info_type* amam_bank_info,
                             rfcommon_xpt_dpd_bank_info_type* ampm_bank_info,
                             rfcommon_xpt_dpd_bank_info_type* epdt_bank_info,
                             uint16 num_rgi_with_dpd_info,
                             tx_linearizer_aux_data_type aux_data,                             
                             boolean hs_lin_enable);

void rfwcdma_mdsp_set_first_xpt_pa_index(int8 xpt_pa_index);    
                         
uint8 rfwcdma_mdsp_get_iq_offset(void);                         

rf_lock_data_type*
rfwcdma_mdsp_get_lock_data
(
  void
);
                        
void rfwcdma_mdsp_init_critical_section(void);                        

void rfwcdma_mdsp_set_qpoet_registers
(
  rfcom_wcdma_band_type band
);

void rfwcdma_mdsp_enable_qtf_tuner_support(void);

void rfwcdma_mdsp_update_xpt_path_delay(int32 xpt_delay_value);

void rfwcdma_mdsp_init_xpt_params(rfcom_wcdma_band_type band);

#ifdef FEATURE_TRITON_MODEM  /* Use Triton modem feature temporarily */
void rfwcdma_mdsp_update_notch_cfg_intf(uint8 notch_index, uint8 cfg);
#endif

#endif /* RFWCDMA_MDSP_H */
