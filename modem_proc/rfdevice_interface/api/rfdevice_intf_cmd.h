#ifndef RFDEVICE_INTF_CMD_H
#define RFDEVICE_INTF_CMD_H

/*!
  @file
  rfdevice_intf_cmd.h

  @brief

  @details

*/

/*==============================================================================

  Copyright (c) 2011 - 2015 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_intf_cmd.h#1 $

when       who     what, where, why
-------    ---     -------------------------------------------------------------
05/13/15   sg      Fix incorrect AFC being applied in GBTA cases
04/15/15   zg      Added support for 2 wtr voting
10/13/14   ry/bm   Added RFDEVICE_TDSCDMA_GET_TX_SPECTRAL_INVERSION cmd
10/17/14   hs      Added WTR4905 command support.
09/05/14   sb      Fix timing of IP2 cal setup writes
08/15/14   lm      Added GNSS B20/28 mode in GNSS CMD
07/17/14   ms      TDS Sawless RX support
06/27/14   hs      Added commands for GNSS.
06/14/14   sb      Add hooks for sawless support
04/23/14   sbm     New Rx command to get WTR rx gain offset
04/15/14   jyu     Added cmd RFDEVICE_TDSCDMA_CONFIGURE_DC_CAL
03/11/14   dps     Added cmd RFDEVICE_GSM_GET_BURST_SCRIPT_EXEC_TIME
03/03/14   aks     Added cmd RFDEVICE_UPDATE_SPUR_DEBUG_FLAG
02/11/14   ak      Add cmd RFDEVICE_WCDMA_GET_RX_CSR_CORRECTION_PARAMS
02/07/14   zg      Added LNA latch support.
01/08/14   whz     Added RFDEVICE_GET_TDS_MEAS_START_SCRIPTS_PREBUILD
12/04/13   whz     Added RFDEVICE_TDSCDMA_GET_WARMUP_TIME in cmd dispatch
11/25/13    pv     Support for ODD Algo for ET.
10/18/13   sma     Added RFDEVICE_WAN_TX_RESET in command dispatch
10/17/13   jn(rs)  Added WTR3925 command support.
09/19/13   sma     Added Chip reset in cmd dispatch
09/12/13   jyu     Added device cmd RFDEVICE_TDSCDMA_SCRIPT_PRELOAD
08/19/13   php     Add RxPLL last write for RxEN
08/06/13   gy      Added support for band tuning
07/31/13   nrk     PA state reversed
07/19/13   php     Add TxLO Cal support to get info from ag
06/21/13   jn      Factory calibrated PGA option for WWAN co-exist.
06/07/13   ry      Added cmd to get Rx PLL settling time
06/03/13   Saul    XPT. Reduced leakage configuration.
06/03/13   nsh     Support Get Multi Synth for Dime
05/21/13   rmb     Removed RFDEVICE_GET_TX_PLL_LOCK_STATUS as this is a
                   duplicate of RFDEVICE_GET_TX_SYNC_LOCK.
05/10/13   vss     Added cmd for FBRX TXLO
05/03/13   gy      Added GNSS RF STATUS and HEALTH commands
04/30/13   gy      Added GNSS M5HT and M5ET commands
04/14/13   jyu     Added device cmd RFDEVICE_TDSCDMA_INIT_DEVICE_DEBUG_FLAG
04/02/13   jyu     Added device cmd RFDEVICE_TDSCDMA_BUILD_SPECIAL_BSP_FOR_IRAT
03/25/13   hk      Added device cmd to retrieve Rx PLL tune script
                   size from WTR driver
03/19/13   jr      Added gnss cmd to get gnss only build
03/19/13   jyu     Added RFDEVICE_TDSCDMA_GET_HDET_INFO
03/06/13   sk      Debug Code Redesigned on MPL
02/21/13   bm      Added cmd to get JD thresholds 
02/20/13   php     Added support for Tx EN and Rx EN events
02/05/13   gy      Added gnss cmds to support gnss rfsw re-partion
01/31/13   cd      Added device command to query device whether
                   therm based Rx RSB compensation is required
01/22/13   cd      Added Device Command ID:
                   (1) Getting PLL tune time based on WTR being used
                   (2) Executing PDET Cal based on WTR
                   (3) Tune Rx PLL for Tx based on whether WTR uses single PLL or not
01/09/12   tws     Added RFDEVICE_GSM_SET_TX_FREQ_OFFSET cmd
01/07/13   tks     Added support for wcdma set tx on/rf on 
01/03/12   jyu     Added RFDEVICE_TDSCDMA_GET_SSBI_SCRIPTS_W_TIME cmd
01/02/12   jyu     Added RFDEVICE_TDSCDMA_GET_RX_ON_RF_ON_TIMING cmd
12/05/12   aka     Added RFDEVICE_GSM_GET_DELAY_BACKOFF cmd
12/03/12   aca     DIME Dac cal
11/29/12   sa      WTR register extraction - debug code
11/28/12   rmb     Added Device Command ID for reading PLL Lock Status for RX Paths 0 and 1.
11/27/12   bm      Added the cmd RFDEVICE_GET_PRX_PLL_VAL
11/26/12   sr      changed the last entry of the enum to 0xFFFFFFFF to force the 
                   compiler to treat the enum as int32 value to support rfc efs data.
11/12/12   aka     Added gsm device cmds to update freq error and HMSC state
11/16/12   tks     Added support for getting bus info from transceiver  
11/05/12   ms      Added cmd interface for TDS IRAT
11/02/12   shb     Added WTR1625/WFR1620
10/25/12   ec      Added command to support reduced rx sbi start time 
10/18/12   tks     Added support for getting tx and prx signal path status 
                   from transceiver 
10/02/12   vrb     Updated QFE1320 enum and GENeric enums for ASMs and PAs
10/02/12   vss     Add support for get RF_ON and RX_ON info for LTE
09/27/12   bm      Added WFR2600 enum
09/25/12   sr      Renamed the QFE1320 enum_id to QFE_1320 to avoid compilation err.
09/25/12   sr      Added new device enums.
09/25/12   jyu     Added SET_RF_ON and SET_RX_ON commands 
08/27/12   jr      Applied gain adjustment proportional to the PGA backoff
08/17/12   shb     Added LTE commands used by DIME
08/17/12   vrb     Added B17_PA and ASM_7Tp7T
08/13/12   vrb     Added PA, ASM and WGR Devices
07/31/12   bn      Added PWR_TRACKER ID
07/27/12   sr      renamed rfdevice related enum types.
07/25/12   tks     Added WCDMA PDET correction code command dispatch 
07/25/12   tks     Added HCPA, MMPA & ASM to list of supported enums 
07/24/12   tks     Added WTR2605 to the list of supported of enums 
07/23/12   php     Added Device cmd ID for supporting TXEN
07/21/12   sr/vrb  Added support for new device_cat types.
07/17/12   jr      Added support for non-ELNA configuration.
07/03/12   ggs     Added copy rx, tx and agc script to buffer cmd IDs
07/03/12   tks     Added PA_GAIN_RANGE_MAP to support MMPA  
06/18/12   aca     LTE DAC Cal update- LTE specific Iref programming
06/12/12   php     HDET Clean up 
05/18/12   sc      Added write script and copy script dispatch cmd IDs 
05/11/12   php      Add TX_ENABLE/DISABLE script generation 
04/01/12   tks     Added support for Tx phase compensation  
03/28/12   jyu     Added device cmd ID to support PDET cal
03/24/12   yzw     Added FTM command FTM_LTE_SET_FREQ_ADJUST
03/12/12   jyu     Added device cmd ID for RF chip information
03/12/12   jyu     Added device cmd ID to calculate hdet/therm measurment
                   from raw readings 
03/06/12   jyu     Added for TDSCDMA 
03/02/12   Saul    MCDO. Rx AGC freq comp and device droop compensation framework.
02/16/12   vss     Add commands for HDET/Therm
02/09/12   aca     Tx PLL error correction
12/22/11   ac      lna phase offset compensation
12/01/11   dw      Add support for WCDMA low power mode (DSR)
11/18/11   jyu     Added command ID for TDSCDMA
11/16/11   tnt     Support for query PDET compensation values for different BW settings
11/15/11   cri     Support for TX PLL adjust
09/25/11   vb      Support for DAC cal using different Irefs
09/21/11   tks     Added WCDMA Get Tx spectral inversion.   
09/20/11   tks     Added WCDMA Get Rx LNA state.  
09/14/11   can     Merge MDM9K tips to NikeL.
09/08/11   vrb     Get RSB and Rx spec inversion requirement from Device Intf
09/07/11   jyu     Added RFDEVICE_LTE_GET_TX_DAC_VALUE 
09/06/11   aca     Removed Spectral Inversion hardcoding
09/2/11    sb      Made GPSYNC clock requirement device dependent
08/29/11   aca     Removed hardcoded RX Gain address, number of gain states
08/26/11   whc     Cal V3 LTE optimize HDET reading 
08/25/11   dbz     Added GNSS PGA tracking loop status
07/26/11   dbz     Added GNSS RF ASIC specific setting
07/25/11   vrb     Added device interface to get GSM TxAGC info from script
07/12/11   aro     Deleted unused enumeration for JD statis reset
07/12/11   vrb     Moved the slot mode control reg info to the device driver
07/07/11   vrb     Move in device specific FTM KV and Tx DPLL Training Sequences
07/06/11   aro     Deleted unused iC command
07/05/11   shb     Added WTR1605 to rfdevice_enum_type
06/28/11   can     LTE FTM TXAGC mode support.
06/10/11   shb     Deleted CDMA commands no longer required on NikeL
06/07/11   shb     Deleted command to handle RF Device Mutex. Will be handled
                   inside RTR8600 device, no external API needed.
06/07/11   shb     Deleted RFDEVICE_CONFIG_TX_LUT
04/20/11   jhe     Remove/add wcdma cmd 
04/20/11   shb     Deleted RTR8600 ADC clock manager
04/18/11   xw      Deleted RFDEVICE_RXPLL_VCO_COARSE_TUNE 
02/25/11   dw      add RFDEVICE_GET_DRX_PLL_VAL 
11/11/10   av      Save the last good known PGA value
10/23/10   jyu     Add L2L interfreq support 
10/20/10   vb      Added common command to setup mutex for RF Device
10/20/10   shb     Added common command to access GP clock state
10/04/10   av      GPS M5 bringup
09/27/10   ac      added rsb support
09/20/10   shb     Added RFDEVICE_CONFIG_TX_LUT to configure 1x TX LUTs
09/16/10   anr     Added RFDEVICE_GET_TX_PATH_STATUS and 
09/13/10   shb     Deleted RDEVICE_RFR_IS_VALID command
09/09/10   shb     Added command to query library if TX spectral inversion is
                   needed
09/03/10   shb     Deleted some unused commands
09/03/10   shb     Added command to get WBDC SSBI info and band/chan support
09/03/10   pl      Added command to support get RX Gain word
08/06/10   sar     Updated code for application of fixed RxF I/Q mis comp 
                   for RTR8600.
08/04/10   av      GPS LTE B13 gain adjust algorithm 
08/03/10   adk     RF warmup time optimization.
06/30/10   shb     Added command to get RX PLL wait count
06/23/10   Vish    Added RFDEVICE_ADJUST_RF_CAL_FOR_HDR command.
06/21/10   av      GPS LTE B13 gain adjust algorithm
06/14/10   shb     Added commands for TX PLL lock monitoring
06/10/10   kguo    Added IRAT related command
06/08/10  xw/vrb   FW Interface update for LNA Mid Gain Setting
05/26/10   ckl     Removed invalid @todo
05/26/10   qma     Added RSB Cal and IM2 DAC read support
05/25/10   pl      Added RFDEVICE_GET_MEAS_START_SCRIPTS for IRAT script generations
05/19/10   Saul    Added Intelliceiver (Hi/Lo lin) and Jammer Detector cal support.
05/19/10   av      LTE GPS B13 gain adjust algorithm
05/19/10   qma     LTE IM2 DAC write support
05/18/10   ckl     Added Release 7 DRX sleep/wakeup command.
05/12/10    dw     Add RFDEVICE_WCDMA_SET_MULTI_CARRIER_RX
05/11/10   Vish    Added RFDEVICE_GET_RX_RECOMMENDATION command.
05/10/10   sty     Added RFDEVICE_GET_RXF_POLARITY
05/10/10   zg      Added RFDEVICE_SET_RX_GAIN_WORD
05/10/10    dw     Added command for LTE therm read
05/03/10   can     HDET support optimization.
04/22/10   xw      Added RFDEVICE_GET_DTX_PLL_VAL for WCDMA 
04/22/10   sty     Added RFDEVICE_FORCE_LOWEST_GAIN_STATE
03/13/10   Vish    Added RFDEVICE_SET_INTELLICEIVER_CAL command.
03/10/10   shb     Added commands to query RTR for LNA info
03/09/10   shb     Added commands to enable/disable HDET
02/10/10   vb      Removed the command to load Tx linearizer
                   Added command to get LUT SBI data   
02/01/10   adk     Added RFDEVICE_GET_RXF_POLARITY to retrieve spectral inv info.
01/29/10   sty     Added RFDEVICE_GET_TX_WARMUP_TIMER_VAL
01/28/10   shb     Added command to reset Jammer Detector
01/21/10   xw      Lint error fixes 
01/20/10   av      Added RFDEVICE_GNSS_RTR_HEALTH
01/06/10   dbz     Added more new commands needed for GNSS
12/25/09   sr      Added more new commands needed for 1x
12/18/09   bn      Added RFDEVICE_SET_HDET_ADC_PAGE
12/17/09   sr      Added more new commands needed for 1x
12/16/09   sr      Added new commands needed for 1x
11/23/09   shb     Added support for IntelliCeiver Threshold Calibration
11/13/09   sr      Removed RFDEVICE_SET_LOWEST_LNA_GAIN_STATE
11/09/09   sr      Added commad dispatch to get ADC samp freq
11/04/09   shb     Added command dispatch support for 1x intelliceiver
10/23/09   dbz     Seperate common/RTR specific func, cmd dispatcher, 
                   RTR library cleanup 
10/05/09   xw      Initial version.

============================================================================*/

#include "rfcom.h"
#include "rfdevice_logical_component_types.h"
#include "rfdevice_chip_ids.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief
  RF device API command IDs for the ones going through the command dispatch. 
*/
typedef enum
{
  /* ------------------------------------ 
   * Common command - mode independant  *      
   ------------------------------------ */

  RFDEVICE_ENABLE_IM2,              /*!< enable/disable IM2 DAC
                                         calibration  : boolean*/
  RFDEVICE_SET_I_VALUE,             /*!< programs i DAC value : byte */
  RFDEVICE_SET_Q_VALUE,             /*!< programs q DAC value : byte */
  RFDEVICE_SET_TRANS_VALUE,         /*!< programs transconductor value : byte */
  RFDEVICE_ENABLE_TCXO_BUFFER,      /*!< turns on and off TCXO buffer for SSBI:
                                         boolean */
  RFDEVICE_FILTER_COARSE_TUNE,      /*!< calculates and returns filter R and
                                         C-tuner value:  uint8* */
  RFDEVICE_GET_AGC_ADJ,             /*!< reads the gain control value: uint16 */
  RFDEVICE_SET_GPS_DAC_ADJUST,      /*!< adjusts the DC offset by writing to
                                         DAC control regs: rfdevice_iq_dac_type
                                    */
  RFDEVICE_SET_GPS_BB_GAIN_STEP,    /*!< sets the baseband gain for gps:
                                         rgr_bb_gain_step_type */
  RFDEVICE_SET_GPS_DC_DAC_MODE,     /*!< sets the gain of the DC DAC for GPS:
                                         rgr_dc_dac_mode_type */
  RFDEVICE_DO_HDET_AUTOCAL,         /*!< One-time HDET autocal */                                
  RFDEVICE_ENABLE_HDET,             /*!< Enable HDET & sets LPF bandwidth */
  RFDEVICE_DISABLE_HDET,            /*!< Disable HDET */
  RFDEVICE_GET_HDET_VALUE,          /*!< reads the HDET value from HKADC:
                                         uint16* */
  RFDEVICE_GET_TX_PATH_STATUS,      /*!< Checks if TX path is ON         */
  RFDEVICE_GET_MDSP_TRIGGERED_HDET, /*!< reads HDET value triggered by mdsp:
                                         uint16* */
  RFDEVICE_GET_REVISION,            /*!< reads RTR revision number:  rtr
                                         revision_type* */                                         
  RFDEVICE_GET_THERMISTER_VALUE,    /*!< reads thermister val from HKADC:
                                         uint16* */
  RFDEVICE_SET_LINEARITY_MODE,      /*!< Sets linearity mode for device:
                                         rgr_linearity_mode_type */
  RFDEVICE_SET_RC_TUNER,            /*!< Sets Rx filter R and C-tuner regs:
                                         uint8 */
  RFDEVICE_GET_PDM_LIMITS,          /*!< Substited by RFDEVICE_GET_TX_AGC_LIMITS */
  RFDEVICE_GET_TX_AGC_LIMITS,       /*!< Get Tx AGC Minimum and Maximum Value:
                                         uint16 */
  RFDEVICE_SET_LUT_INDEX,             /*!< Sets LUT index: uint8 */

  RFDEVICE_GET_LUT_SSBI_ADDR,     /*!< Gets Tx AGC LUT SSBI register address */

  RFDEVICE_GET_LUT_SSBI_DATA,     /*!< Gets Tx AGC LUT SSBI register values */

  RFDEVICE_SET_LUT_PA_RANGE_MAP,    /*!< Sets LUT vs. PA Range mapping */

  RFDEVICE_GET_TX_SYNC_LOCK,       /*!< Get Tx sync lock status */

  RFDEVICE_GET_RX_SYNC_LOCK,       /*!< Get Rx sync lock status */

  RFDEVICE_RESET_RX,               /*!< Reset all Rx regs */

  RFDEVICE_RESET_WAN,              /*!Reset Chip*/

  RFDEVICE_WAN_TX_RESET,           /*! WAN TX RESET*/

  RFDEVICE_CONFIG_DAC_CAL,       /*!< Configure device for dac cal */

  RFDEVICE_ENABLE_GP_CLK,          /*!< Enable Gp_clk */

  RFDEVICE_GET_RF_REGISTERS,        

  RFDEVICE_GET_RF_RX_IMAGE,         

  RFDEVICE_GET_LNA_STATE,          /*!< Get curent LNA gain state  */

  RFDEVICE_GET_LNA_PHASE_OFFSETS,  /*!< Get LNA Phase offsets */

  RFDEVICE_GET_NUM_LNA_GAIN_STATES, /*!< Num of LNA Gain states supported by RTR */

  RFDEVICE_GET_MEAS_START_SCRIPTS,  /*!< get the start scripts for IRAT */

  RFDEVICE_GET_TDS_MEAS_START_SCRIPTS,  /*!< get the start scripts for TDS IRAT */

  RFDEVICE_GET_RXF_POLARITY,        /*!< Rx Spectral inversion */

  RFDEVICE_GET_DTX_PLL_VAL,         /*!< Get DTx PLL value */

  RFDEVICE_GET_PRX_PLL_VAL,         /*!< Get PRx PLL value */

  RFDEVICE_GET_DRX_PLL_VAL,         /*!< Get DRx PLL value */

  RFDEVICE_GET_RX_MIDGAIN_VAL,      /*!< Get Rx LNA mid gain value */

  RFDEVICE_GET_HDET_MDSP_CONFIG_DATA, /*!< Get hdet mdsp config data */

  RFDEVICE_GET_GP_CLK_STATE,        /*!< Get device GP clk state */

  RFDEVICE_GET_TX_DAC_CAL_IREF,        /*!< Get recommended DAC Iref for <band,tech>  */

  RFDEVICE_GET_PA_GAIN_RANGE_MAP, 

  RFDEVICE_GET_TX_PLL_LOCK_STATUS,    /*!< Reads TX PLL SSBI registers to determine if it is locked or unlocked */

  RFDEVICE_GET_PRX_PLL_LOCK_STATUS, /*!< Reads PRx PLL SSBI registers to determine if it is locked or unlocked */

  RFDEVICE_GET_DRX_PLL_LOCK_STATUS, /*!< Reads DRx PLL SSBI registers to determine if it is locked or unlocked */

  RFDEVICE_INIT_SBI_SIM_CLIENT,     /*!< Initializes data structures for the SBI Simulator */

  RFDEVICE_CFG_REDUCED_LEAKAGE,     /*!< Configure Reduced Leakage */

  RFDEVICE_GET_RX_PLL_SETTLING_TIME,       /*!< Reads Rx Tune Time from the device level API*/

  RFDEVICE_UPDATE_SPUR_DEBUG_FLAG,

  RFDEVICE_GET_PWR_VOTE_STATUS,     /*! Reading back Power Voting information for debugging */ 

  RFDEVICE_PRINT_ACTIVE_PWR_VOTE,   /*! Print out active components with associated tech(s) */ 

  /* ----------------------------------
          GSM Only related commands         */

  /* ------------------------------------ 
   * 1x/HDR command                     *      
   ------------------------------------ */

  RFDEVICE_GET_TX_LUT_TABLE, /*!< Returns address of TX PDM LUT for curr band */

  RFDEVICE_GET_LUT_WRITE_ADDR,

  RFDEVICE_GET_POWER_MODE, /* Read Jammer Interrupt Status and figure current power mode */

  RFDEVICE_GET_NOMINAL_BBF_GAIN_DB10,

  RFDEVICE_GET_TX_LIN_MIN_OFFSET,

  RFDEVICE_GET_TX_LIN_MAX_OFFSET,

  RFDEVICE_GET_TX_WARMUP_TIMER_VAL,

  RFDEVICE_GET_RX_RECOMMENDATION,    /*!< Gets RF/baseband recommendations for downconverting PRx and/or DRx */ 

  RFDEVICE_ADJUST_RF_CAL_FOR_HDR, /*!< Adjusts any RF cal items that are needed for operating in HDR mode */

  RFDEVICE_GET_COMM_INFO, /*! <* Retrieves Modem<-->RF chipset communication related data */

  RFDEVICE_GET_PRE_EQ_COEFF_VAL, /*!< The filter coefficiednt to be used in the TX pre-eq block */

  RFDEVICE_GET_TX_PLL_LOCK_POLLING_COUNT, 
  /*!< Reads critical TX PLL SSBI registers to determine if it is possibly 
  unstable and needs to be monitored (polled). Determines no. of times 
  PLL status should be polled */

  RFDEVICE_GET_MIN_RX_PLL_LOCK_WAIT_COUNT,
  /*!< Minimum number of "counts" (each count has 100us wait) external 1x/HDR driver
  should wait for PLL to lock */

  RFDEVICE_GET_IQMC_COEFS,
  /*!< Return I/Q mismatch Compensation Coefficients*/

  RFDEVICE_GET_RX_GAIN_WORD,
  /*!< Return RX Gain word */

  RFDEVICE_IS_1X_BAND_CHAN_SUPPORTED,
  /*!< Returns a boolean indicating if a band and channel (passed as argument) is 
  supported */

  RFDEVICE_GET_WBDC_SBI_INFO,
  /*!< Returns WBDC loop register addresses on device */

  RFDEVICE_IS_TX_SPECTRAL_INVERSION_NEEDED,
  /*!< Determines if TX spectral inversion is needed from Modulator */

  RFDEVICE_MULTI_CARRIER_DROOP_COMPENSATION,
  /*!< Returns droop compensation values */

  RFDEVICE_SET_RF_ON,
  /*!< Set RF_ON as SSBI */

  RFDEVICE_SET_RX_ON,
  /*!< Set RX_ON as SSBI */

  RFDEVICE_GET_JD_THRESH,
  /*!< Gets the JD threshold values from Device drvier */

  /* ------------------------------------ 
   * GNSS command                     *      
   ------------------------------------ */

  RFDEVICE_GNSS_GET_RTR_STATUS,

  RFDEVICE_GNSS_RETUNE_PLL,

  RFDEVICE_GNSS_RC_TUNE_CAL,

  RFDEVICE_GNSS_FE_CAL,

  RFDEVICE_GNSS_DC_UPDATE,

  RFDEVICE_GNSS_DC_CANCELLATION,

  RFDEVICE_GNSS_GAIN_ADJUST,

  RFDEVICE_GNSS_RTR_HEALTH,

  RFDEVICE_GNSS_GAIN_ADJUST_BOOT,

  RFDEVICE_GNSS_GAIN_BACKOFF,

  RFDEVICE_GNSS_GAIN_RESTORE,

  RFDEVICE_GNSS_CRITICAL_DEMOD_NOTIFY,

  RFDEVICE_GNSS_CONTROL_PERIODIC_DC,

  RFDEVICE_GNSS_SAVE_PGA,

  RFDEVICE_GNSS_GET_RFASIC_SPECIFC,

  RFDEVICE_GNSS_SET_PGA_LOOP_STATUS,

  RFDEVICE_GNSS_GET_LNA_STATE,

  RFDEVICE_GNSS_GET_GNSS_ONLY_BUILD_STATE,

  RFDEVICE_GNSS_GET_REDUCED_PGA_BACKOFF_GAIN,

  RFDEVICE_GNSS_GET_CHIP_ID,

  RFDEVICE_GNSS_GAIN_USE_FACTORY,

  RFDEVICE_GNSS_CMD_M5_LL_INIT,

  RFDEVICE_GNSS_CMD_M5_ELNA_INIT,

  RFDEVICE_GNSS_CMD_M5_HL_INIT,

  RFDEVICE_GNSS_CMD_M5HT_INIT,

  RFDEVICE_GNSS_CMD_M5ET_INIT,

  RFDEVICE_GNSS_CMD_M5ET1_INIT,

  RFDEVICE_GNSS_CMD_MTE_INIT,

  RFDEVICE_GNSS_CMD_COMPASS_ELNA_INIT,

  RFDEVICE_GNSS_CMD_SLEEP,

  RFDEVICE_GNSS_CMD_RESUME,

  RFDEVICE_GNSS_CMD_PAUSE,

  RFDEVICE_GNSS_CMD_M5_IF,

  RFDEVICE_GNSS_CMD_M5_BP1_DOWNCONV_IF,

  RFDEVICE_GNSS_CMD_M5_BP3_DOWNCONV_IF,

  RFDEVICE_GNSS_CMD_CHIP_OFF,

  RFDEVICE_GNSS_CMD_ENABLE_TG,

  RFDEVICE_GNSS_CMD_DISABLE_TG,

  RFDEVICE_GNSS_CMD_WRITE_BT_CODE,

  RFDEVICE_GNSS_CMD_SET_TXATTN,

  RFDEVICE_GNSS_CMD_WRITE_SEL_CODE,

  RFDEVICE_GNSS_CMD_DCOC_WRITE_FINE_DAC,

  RFDEVICE_GNSS_CMD_DCOC_WRITE_COARSE_DAC,

  RFDEVICE_GNSS_CMD_DCOC_FINE1_PRE_CONFIG,

  RFDEVICE_GNSS_CMD_DCOC_FINE1_POST_CONFIG,

  RFDEVICE_GNSS_CMD_DCOC_COARSE_PRE_CONFIG,

  RFDEVICE_GNSS_CMD_DCOC_FINE2_PRE_CONFIG,

  RFDEVICE_GNSS_CMD_DCOC_FINE2_POST_CONFIG,

  RFDEVICE_GNSS_CMD_DCOC_READ_FINE_DAC,

  RFDEVICE_GNSS_CMD_DCOC_READ_COARSE_DAC,

  RFDEVICE_GNSS_CMD_PGA_WRITE,

  RFDEVICE_GNSS_CMD_WRITE_R_TUNE,

  RFDEVICE_GNSS_CMD_WRITE_RC_TUNE,

  RFDEVICE_GNSS_CMD_SET_MIXER_NOTCH,

  RFDEVICE_GNSS_CMD_GET_RF_STATUS,

  RFDEVICE_GNSS_CMD_GET_RF_HEALTH,

  RFDEVICE_GNSS_CMD_GET_CHIP_REVISION,

  RFDEVICE_GNSS_CMD_GET_CHIP_PROCESS,

  RFDEVICE_GNSS_CMD_IELNA_CONFIG,       /* WFR */

  RFDEVICE_GNSS_CMD_GET_RF_STATUS1,

  RFDEVICE_GNSS_CMD_GET_RF_HEALTH1,

  RFDEVICE_GNSS_CMD_CALCULATE_BT_GNSS_ELNA,

  RFDEVICE_GNSS_CMD_QFUSE_READ_GPS,

  RFDEVICE_GNSS_CMD_ADC_CAL_PREP,

  RFDEVICE_GNSS_CMD_ADC_CAL_AFTER,

  RFDEVICE_GNSS_CMD_SE_DCOC,

  RFDEVICE_GNSS_CMD_ENTER_2H_MODE,

  RFDEVICE_GNSS_CMD_EXIT_2H_MODE,
  
  RFDEVICE_GNSS_CMD_ENTER_B20_B28_MODE,

  RFDEVICE_GNSS_CMD_EXIT_B20_B28_MODE,

  RFDEVICE_GNSS_CMD_SET_FINE_GAIN,

  /* ------------------------------------ 
   * GSM command                        *      
   ------------------------------------ */

  RFDEVICE_GSM_UPDATE_RX_TIMING = 0x100, 

  RFDEVICE_GSM_USE_REDUCED_RX_SBI_TIME,

  RFDEVICE_GSM_UPDATE_TX_TIMING,

  RFDEVICE_GSM_TURN_OFF_PLL,

  RFDEVICE_GSM_GET_TX_DPLL_TRAINING_SEQUENCE,

  RFDEVICE_GSM_GET_RX_RSB_DATA,

  RFDEVICE_GSM_GET_FACTORY_KV_CALIBRATION_DATA,

  RFDEVICE_GSM_GET_SLOT_MODE_CTRL_REG_INFO,

  RFDEVICE_GSM_GET_UPDATED_TXAGC_INFO,

  RFDEVICE_GSM_SEND_SCRIPT_TO_HW,

  RFDEVICE_GSM_COPY_SCRIPT_TO_BUFF,

  RFDEVICE_GSM_COPY_RX_SCRIPT_TO_BUFF,

  RFDEVICE_GSM_COPY_TX_SCRIPT_TO_BUFF,

  RFDEVICE_GSM_BUILD_THERM_READ_SCRIPT,  

  RFDEVICE_GSM_SET_FREQ_ERROR,

  RFDEVICE_GSM_SET_RX_FREQ_ERROR,

  RFDEVICE_GSM_SET_TX_FREQ_ERROR,

  RFDEVICE_GSM_SET_HMSC_STATE,

  RFDEVICE_GSM_GET_DELAY_BACKOFF,

  RFDEVICE_GSM_SET_TX_FREQ_OFFSET,

  RFDEVICE_GSM_GET_BURST_SCRIPT_EXEC_TIME,

  RFDEVICE_GSM_SET_HIGH_LIN_MODE,

  /* Hook to pass timing to specfic APIs that need timing */
  RFDEVICE_GSM_SET_TIMED_WRITES,

  RFDEVICE_GSM_CLEAR_TIMED_WRITES,
 /*-----------------------------------* 
  * LTE related commands              *      
  *-----------------------------------*/

  RFDEVICE_LTE_ADC_CLK_ENABLE,

  RFDEVICE_LTE_ADC_CLK_DISABLE,

  RFDEVICE_LTE_SET_TX_CHAR_TBL_ENTRY,

  RFDEVICE_LTE_SET_TX_CHAR_TBL_ENTRY_RFC_PA_MAP,

  RFDEVICE_LTE_GET_TX_CHAR_TBL,

  RFDEVICE_LTE_GET_TX_CHAR_TBL_RFC_PA_MAP,

  RFDEVICE_LTE_SET_TX_GAIN_VALUE,

  RFDEVICE_LTE_SET_RX_GAIN_STATE,

  RFDEVICE_LTE_GET_RX_GAIN_STATE_TBL,

  RFDEVICE_LTE_GET_TX_HDET_SETTINGS,

  RFDEVICE_LTE_GET_TX_HDET_SETTINGS_V2,

  RFDEVICE_LTE_GET_TX_PDET_ADC_VALUE,

  /* These two CMD dispatch IDs are used for Optimal Delay Detection (ODD)
     And used in ET mode */
  RFDEVICE_LTE_XPT_ODD_GET_TX_PDET_ADC_VALUE,

  /* CMD dispatch ID are used for Optimal Delay Detection (ODD)
     And used in ET mode */
  RFDEVICE_LTE_XPT_ODD_GET_SCRIPT_TIMING,

  RFDEVICE_LTE_GET_THERM_ADC_VALUE_V2,

  RFDEVICE_LTE_HDET_INIT,

  RFDEVICE_LTE_HDET_ENABLE,

  RFDEVICE_LTE_HDET_DISABLE,

  RFDEVICE_LTE_HDET_READ,

  RFDEVICE_LTE_HDET_DO_AUTOCAL,

  RFDEVICE_LTE_DO_FULL_HDET,

  RFDEVICE_LTE_GET_THERMISTER_VALUE,
  
  RFDEVICE_LTE_GET_RX_GAIN_DEFAULT_PWR_VALUE,

  RFDEVICE_LTE_GET_TX_DAC_VALUE,
  
  RFDEVICE_LTE_SET_RX_IM2_DAC_CODES,

  RFDEVICE_LTE_GET_RX_IM2_DAC_CODES,

  RFDEVICE_LTE_GET_RX_RSB_OFFSET,

  RFDEVICE_LTE_GET_TX_SPECTRAL_INVERSION,

  RFDEVICE_LTE_FTM_TXAGC_READ_HDET,

  RFDEVICE_LTE_GET_HDET_READ_FLAG,

  RFDEVICE_LTE_GET_TX_PLL_VALUES,

  RFDEVICE_LTE_GET_HPM_PDET_COMP_VALUES,

  RFDEVICE_LTE_GET_LPM_PDET_COMP_VALUES,

  RFDEVICE_LTE_ENABLE_NPLER_INDICES,
  
  RFDEVICE_LTE_GET_TX_PLL_SCRIPT,

  RFDEVICE_LTE_WRITE_SBI_SCRIPT,
  
  RFDEVICE_LTE_PROGRAM_IREF,

  RFDEVICE_LTE_GET_TX_ENABLE_SCRIPT,

  RFDEVICE_LTE_GET_TX_DISABLE_SCRIPT,

  RFDEVICE_LTE_GET_TX_LOW_GAIN_MASKS,

  RFDEVICE_LTE_GET_RX_ON_INFO,  

  RFDEVICE_LTE_GET_TX_ON_INFO,
   
  RFDEVICE_LTE_GET_RX_ENABLE_SCRIPT,

  RFDEVICE_LTE_GET_RX_DISABLE_SCRIPT,

  RFDEVICE_LTE_CONFIG_FB_PATH_FOR_TXLO,
   
  RFDEVICE_LTE_GET_TXLO_CAL_PARAM,

  RFDEVICE_LTE_GET_RX_PLL_LAST_WRITE_SCRIPT,

  RFDEVICE_LTE_GET_RX_GAIN_OFFSET,
   /*-----------------------------------* 
  * WCDMA related commands              *      
  *-----------------------------------*/
  RFDEVICE_WCDMA_SET_MULTI_CARRIER_RX,

  RFDEVICE_WCDMA_GET_RX_RSB_COEFF,

  RFDEVICE_WCDMA_GET_LNA_PHASE_OFFSET,

  RFDEVICE_WCDMA_GET_RX_LNA_STATE, 

  RFDEVICE_WCDMA_GET_TX_SPECTRAL_INVERSION, 

  RFDEVICE_WCDMA_GET_RX_LINEARITY_SETTING,

  RFDEVICE_WCDMA_GET_TX_PHASE_COMP_ENABLE, 

  RFDEVICE_WCDMA_GET_TX_LINEARIZER_VERSION, 

  RFDEVICE_WCDMA_SET_PDET_CORRECTION_CODE, 

  RFDEVICE_WCDMA_GET_TX_SIG_PATH_STATUS,

  RFDEVICE_WCDMA_GET_PRX_SIG_PATH_STATUS,

  RFDEVICE_WCDMA_GET_RX_SSBI_BUS, 

  RFDEVICE_WCDMA_GET_TX_SSBI_BUS,

  RFDEVICE_WCDMA_SET_TX_ON, 

  RFDEVICE_WCDMA_GET_RX_CSR_CORRECTION_PARAMS, 

  /*-----------------------------------* 
  * IRAT related commands              *      
  *-----------------------------------*/
  RFDEVICE_WCDMA_GET_MEAS_START_TBL,

  RFDEVICE_GET_L2L_SCRIPTS,

  RFDEVICE_GET_GPSYNC_REQUIREMENT,

  /*-----------------------------------* 
  * TDSCDMA related commands           *      
  *-----------------------------------*/

  RFDEVICE_TDSCDMA_GET_RX_LNA_STATE,

  RFDEVICE_TDSCDMA_GET_TX_SLOT_MODE_CTRL_INFO,

  RFDEVICE_TDSCDMA_SET_RX_GAIN_STATE,

  RFDEVICE_TDSCDMA_SET_LNA_LATCH,

  RFDEVICE_TDSCDMA_GET_SSBI_SCRIPTS,

  RFDEVICE_TDSCDMA_GET_RX_RSB_COEFF,  

  RFDEVICE_TDSCDMA_GET_TX_SYNC_LOCK,

  RFDEVICE_TDSCDMA_GET_TX_CHAR_TBL,

  RFDEVICE_TDSCDMA_CALC_HDET_MEASUREMENT,

  RFDEVICE_TDSCDMA_CALC_THERM_MEASUREMENT,

  RFDEVICE_TDSCDMA_GET_RF_CHIP_INFO,

  RFDEVICE_TDSCDMA_DO_PDET_CAL,

  RFDEVICE_TDSCDMA_GET_RX_ON_RF_ON_TIMING,

  RFDEVICE_TDSCDMA_GET_SSBI_SCRIPTS_W_TIME,

  RFDEVICE_TDSCDMA_GET_RX_PLL_TUNE_TIME,

  RFDEVICE_TDSCDMA_GET_TX_PLL_TUNE_TIME,

  RFDEVICE_TDSCDMA_PDET_CAL_FLAG,

  RFDEVICE_TDSCDMA_TX_USES_RX_PLL,

  RFDEVICE_TDSCDMA_GET_FW_REVISION,

  RFDEVICE_SET_RX_ON_SBI,

  RFDEVICE_SET_RF_ON_SBI,

  RFDEVICE_TDSCDMA_USE_RX_RSB_COMP,

  RFDEVICE_TDSCDMA_GET_HDET_INFO,

  RFDEVICE_TDSCDMA_GET_RX_TUNE_SCRIPT_SIZE, 

  RFDEVICE_TDSCDMA_BUILD_SPECIAL_BSP_FOR_IRAT,

  RFDEVICE_TDSCDMA_INIT_DEVICE_DEBUG_FLAG,

  RFDEVICE_TDSCDMA_GET_RX_PLL_STATUS_BITMASK,

  RFDEVICE_TDSCDMA_GET_TX_PLL_STATUS_BITMASK,

  RFDEVICE_TDSCDMA_SCRIPT_PRELOAD,

  RFDEVICE_TDSCDMA_GET_WARMUP_TIME,

  RFDEVICE_GET_TDS_MEAS_START_SCRIPTS_PREBUILD,

  RFDEVICE_TDSCDMA_CONFIGURE_DC_CAL,

  RFDEVICE_TDSCDMA_JDET_ENABLE,

  RFDEVICE_TDSCDMA_JDET_DISABLE,

  RFDEVICE_TDSCDMA_JDET_READ,
  
  RFDEVICE_TDSCDMA_GET_TX_SPECTRAL_INVERSION,

  RFDEVICE_TDSCDMA_SET_RX_GAIN_STATE_WXE

} rfdevice_cmd_enum_type;

typedef enum
{
  RFDEVICE_COMM_PROTO_SSBI,
  RFDEVICE_COMM_PROTO_RFFE,
  RFDEVICE_COMM_PROTO_GRFC,
  RFDEVICE_COMM_PROTO_MAX_NUM,
  RFDEVICE_COMM_PROTO_INVALID = 0xFFFFFFFF
} rfdevice_comm_proto_enum_type;

typedef enum
{
  RFDEVICE_PRX_CHAIN,
  RFDEVICE_DRX_CHAIN,
  RFDEVICE_TX_CHAIN,
  RFDEVICE_CHAIN_NOT_APPLICABLE,
  RFDEVICE_CHAIN_MAX_NUM,
  RFDEVICE_CHAIN_INVALID = 0xFFFFFFFF
} rfdevice_chain_enum_type;

#endif /* RFDEVICE_INTF_CMD_H */
