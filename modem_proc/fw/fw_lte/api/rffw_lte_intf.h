/*!
  @file rffw_lte_intf.h 

  @brief
  This file contains the interface between RFSW on Q6SW and FW on Q6FW for 
  NiKeL based modems.



*/

/*===========================================================================

  Copyright (c) 2009-2012 Qualcomm Technologies Incorporated. All Rights Reserved 

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


								EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_lte/api/rffw_lte_intf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/13    bc     Added support for TX CW tone
08/26/13   php     Increase PAPM CCS buffer size
08/22/13    pv     Added support for dynamic update of separate PA scripts.
08/01/13   sbm     Increase rffe_buf size to fix buffer overflow.
07/25/13   php     Increase TxON CCS buffer size to fix buffer overflow
07/18/13   bar     Add DPD cfg to support ET SV
07/11/13   php     Add Intraband CA Active flag
07/09/13    pv     Added support for dynamic update of separate TDD Tx and Rx ASM scripts.
07/08/13   php     Add LNA thresholds for Intraband CA RxAGC
07/03/13   php     Add HDET update active flag to indicate HDET scripts update
06/18/13   jf      Add XPT logging
06/13/13   sbm     Added suuport for PA,ASM and QPOET trigger script.
06/06/13    pv     Added support for multple TDD Rx/Tx ASM scripts.
06/03/13   php     Add double buffering for TM8 SRS Ant switch scripts
06/03/13   jf      Update desired TXAGC pwr based on number of RBs
05/07/13   jf      Add multiple freq comp bins support for XPT
05/03/13   php     Increase GRFC buffer size to fix buffer overflow
05/03/13   php     Add Tx antenna indices ENUM to support Ant Swap
04/26/13   php     Added scripts support for Antenna Switch Diversity
03/29/13   php     Add support for Tx Antenna switch
07/04/13   jf      Support multiple freq comp sweeps for XPT(reverted)
03/20/13   jf      Add flag to indicate if first sweep lin is populated
03/18/13   php     Add timings for Tx ON/Tx EN and add SCell Rx ON CCS events support
03/05/13   jf      Add first sweep linearizer and Pin Pout compensation offset in SM
02/28/13   jf      Add IQ gain and Env scale nominal for XPT 
02/21/13   jf      Add ET delay values for each slot
02/12/13   jf      Add flag to indicate if DPD is in use
02/04/13   pv      Added extra headers to manage multiple Slave IDs for ASM for cascaded 
                   design in LTE TDD 
01/14/13   jf      Add tbl size for AMAM, AMPM, and DET
12/03/12   qma     Added RGI SSBI bus in RF FW interface to support TX AGC on CA cards
11/30/12   pv/pl   RF FW interface change for Rx Gain settings for Intra band CA for 
                   standalone and L2L ncell meas.
11/30/12   jf      Combine PAPM table for EPT and ET
11/19/12   qma     APT interfaces changes
11/13/12   qma     Added HDET read and RF timing structure support
11/01/12   qma     Added more CCS events and RF scripts support
09/21/12   qma     Added TX_ON,TX_EN,SMPS_EN, RX_ON, TDD ASM RX/TX switch CCS events  
09/17/12   qma     RF FW interface clean up and add tx on events
07/31/12 qma\vss   Added HDET CCS task support
03/08/12   php     Added support for TXP2 signals
02/16/12   pl      Add RX config DONE flag
02/15/12   vss     Remove GNSS blanking enabled/disabled field and add INVALID member to the grfc engine type
02/15/12   vss     Add field to enable/disable gnss blanking 
02/02/12   vss     Add documentation for tx agc addr and setting 
02/02/12   vss     Reverse Tx AGC addr and setting sequence to fix LTE open loop failures 
01/20/12   vss     Add support for GNSS blanking
09/13/11   mcheng  Enable APT
08/31/11   isalman Don't use q6 headers when compiling for UMPLT.
05/11/11   qma     LTE Rx_ON rise and fall delay
04/25/11   qma     LTE DCC support
04/12/11   can     LTE PA SMPS bias logging support for APT and GST.
04/10/11   can     LTE APT track by UE output power support.
03/01/11  whc\can  LTE APT support
12/21/10   can     Basic APT\GST support
11/17/10   can     Change to support LTE FW making the temperature reads.
11/11/10   whc     Added lin_vs_temp and lin_vs_freq to lte_sm fw2sw
09/23/10   can     Support for future slot1, slot2 and SRS TXAGC interface.
08/09/10   whc     Added a function to pass more values from txagc FW to SW using SM. 
05/27/10   can     Support for Low power HDET mode.
05/25/10   can     Support for LIN vs. TEMP.
05/11/10   can     Added an additional flag to avoid temp and PDET read clashes.
05/03/10   can     HDET support optimization.
04/16/10   can     Support for HDET, buffer to be backwards compatible, and 
                   FW to take GRFC control.
03/10/10   can     Support to increase the LTE Shared Memory counter so the values are
                   updated on the FW side and TX MPR and TX Freq. Comp. Support.
01/26/10   qma     Added RXAGC delta and default rx gain offset support in 
                   shared mem interface
01/19/10   qma     RXAGC shared mem interface cleanup
01/18/10   can     Changes to support Shared Memory and TXAGC.
11/19/09   qma     Added RXAGC override interface.
11/18/09   can     Changes and RXAGC interface addition.
08/05/09  can\pl   Added Shared Memory size Macro def.  
08/05/09   can     Initial version. 
  

===========================================================================*/

#ifndef RFFW_LTE_INTF_H
#define RFFW_LTE_INTF_H

#include "comdef.h"

#define RFFW_LTE_MAX_NUM_PA_RANGES	   				4
#define RFFW_LTE_MAX_NUM_RX_GAIN_RANGES	   			6
#define RFFW_LTE_NUM_RX_PATHS	   			        4
#define RFFW_LTE_MAX_NUM_FREQ_COMP				8
#define RFFW_LTE_NUM_WAVEFORM_COMP_ENTRIES			8
#define RFFW_LTE_NUM_OF_SINGLE_TX_GAIN_SETTING_TBL              2
#define RFFW_LTE_SINGLE_TX_GAIN_SETTING_TBL_SIZE                64
#define RFFW_LTE_PA_ICQ_ADDR_MAX_NUM                            10
#define RFFW_LTE_PAPM_ADDR_MAX_NUM                              10
#define RFFW_LTE_NUM_SUPPORTED_EPT_STATES                       2
#define RFFW_LTE_NUM_SUPPORTED_ET_STATES                        2

/* This macro helps control the total number of ASM allowed per Path on FE */
#define RFFW_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED                2

/* This macro helps control the total allowed unique settings per PATH on each FE */
#define RFFW_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH             2

/* This macro helps control the total allowed unique settings per PATH on each FE */
#define RFFW_LTE_MAX_PA_SCRIPTS_SUPPORTED_PER_PATH             2

/*! 
 * @brief The size of all the characterization tables should be set to the 
 * following value. The entries that will be left empty should be set
 * to 0xFF.
 */
#define RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE RFFW_LTE_SINGLE_TX_GAIN_SETTING_TBL_SIZE*RFFW_LTE_NUM_OF_SINGLE_TX_GAIN_SETTING_TBL

#define RFFW_LTE_RX_GAIN_TIMER_HYSTERESIS_CONSTANT  0

/*! 
 * @brief The following is holds the number of MPR indexes.
 */
#define NUM_MPR_IND									16

/*! 
 * @brief The following is holds the number of TXAGC SBIs.
 */
#define RFFW_LTE_NUM_TX_GAIN_SBI_REGS               4

/*! 
 * @brief The following is used to see the maximum number of mod 
 * schemes requiring an MPR adjustment.
 */
#define RFFW_LTE_NUM_OF_MOD_FOR_MPR                 2

#define RFFW_INTF_LTE_NUM_ANT_SEL 4

#define RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME  10

/*! 
 * @brief The RFFW_LTE_MAX_ACTIONS_PER_HDET_WR_SEQ macro is used to specify the maximum number of actions per 
 * HDET write sequence.
 */
#define RFFW_LTE_MAX_ACTIONS_PER_HDET_WR_SEQ 10

/*! 
 * @brief The RFFW_LTE_MAX_ACTIONS_PER_HDET_RD_SEQ macro is used to specify the maximum number of actions per 
 * HDET read sequence.
 */
#define RFFW_LTE_MAX_ACTIONS_PER_HDET_RD_SEQ 10

/*! 
 * @brief The RFFW_LTE_MAX_ACTIONS_PER_THERM_WR_SEQ macro is used to specify the maximum number of actions 
 * per THERM write sequence.
 */
#define RFFW_LTE_MAX_ACTIONS_PER_THERM_WR_SEQ 19

/*! 
 * @brief The RFFW_LTE_MAX_ACTIONS_PER_THERM_WR_SEQ macro is used to specify the maximum number of actions 
 * per THERM read sequence.
 */
#define RFFW_LTE_MAX_ACTIONS_PER_THERM_RD_SEQ 8

/*! 
 * @brief The following is used to specify the maximum number of Resource Block Configs. Currently 
 * only 3 RB Config combinations are supported. one each for Low BW, Mid BW, High BW.
 */
#define RFFW_LTE_HDET_RB_CONFIGS 3

/*! 
 * @brief The following is used to specify the maximum number of tx power configurations supported by HDET. 
 * Currently, only High Power and Low Power HDET are supported.
 */
#define RFFW_LTE_HDET_TX_PWR_CONFIGS 2

/*! Max number of ant sel need to control in TDD*/
#define MAX_ENGINES  10

/*! Max number of LNA Gain word (GW) SSBI to program to tune to a Carrier. 
    This is to incorporate LNA + PLL split for CA on WTR1625 + WFR1620 */
#define RFFW_MAX_LNA_GW_SSBI_PER_PATH 2

#define RFFW_HDET_WR_MAX_START_SIZE 20
#define RFFW_HDET_WR_MAX_LPM_HIGH_BW_SIZE 35
#define RFFW_HDET_WR_MAX_LPM_MID_BW_SIZE 35
#define RFFW_HDET_WR_MAX_LPM_LOW_BW_SIZE 35
#define RFFW_HDET_WR_MAX_HPM_HIGH_BW_SIZE 35
#define RFFW_HDET_WR_MAX_HPM_MID_BW_SIZE 35
#define RFFW_HDET_WR_MAX_HPM_LOW_BW_SIZE 35
#define RFFW_HDET_RD_SIZE 30
#define RFFW_THERM_WR_MAX_INIT_SIZE 60
#define RFFW_THERM_WR_MAX_CONV_DISABLE_SIZE 60
#define RFFW_LTE_TDET_TEMP_COMP_SIZE 70
#define RFFW_THERM_RD_SIZE 60
#define RFFW_RX_ON_WR_SIZE 20
#define RFFW_TX_ON_WR_SIZE 30
#define RFFW_TX_EN_WR_SIZE 30
#define RFFW_SMPS_EN_WR_SIZE 20
#define RFFW_ASM_RX_WR_SIZE 35
#define RFFW_ASM_TX_WR_SIZE 35
#define RFFW_RX_EN_WR_SIZE 20
#define RFFW_RX_EN_RX_ON_WR_SIZE 30
#define RFFW_TX_EN_TX_ON_WR_SIZE 30
#define RFFW_PAPM_ON_SIZE 40
#define RFFW_PAPM_OFF_SIZE 40
#define RFFW_HDET_RD_RESULT_NUM_TRANS 8
#define RFFW_THERM_RD_RESULT_NUM_TRANS 8

/*! Max number of transactions for SSBI/RFFE/GRFC buffer
*/
#define RFFW_LTE_GRFC_TRANS_MAX  40
#define RFFW_LTE_SBI_TRANS_MAX  40
#define RFFW_LTE_RFFE_TRANS_MAX  65

#define RFFW_LTE_WRITE     0
#define RFFW_LTE_READ      1

#define RFFW_LTE_TIMED     0
#define RFFW_LTE_IMMEDIATE 1

/* May need to add more bus/channel ID for SSBI/RFFE */
#define RFFW_LTE_SBI_TX_BUS_ID 1
#define RFFW_LTE_SBI_RX_BUS_ID 2

/* Tx Antenna Indices for TM8 SRS Ant Swap */
#define RFFW_LTE_TX_ANT_0 0
#define RFFW_LTE_TX_ANT_1 1

typedef enum
{
    RFFW_LTE_MOD_QPSK  = 0,
    RFFW_LTE_MOD_16QAM = 1,
    RFFW_LTE_MOD_BPSK,
    RFFW_LTE_MOD_64QAM,
    
}rffw_lte_mod_type_s;

typedef enum
{
    RFFW_LTE_SLOT1      = 0,
    RFFW_LTE_SLOT2      = 1,
    RFFW_LTE_SRS        = 2,
}rffw_lte_slot_type_s;

typedef enum
{
  RFFW_LTE_PA_SMPS_MODE_GST = 0,
  RFFW_LTE_PA_SMPS_MODE_APT_IDX_TRK = 1,
  RFFW_LTE_PA_SMPS_MODE_APT_PWR_TRK = 2,
  RFFW_LTE_PA_SMPS_MODE_PAPM = 3,
  RFFW_LTE_PA_SMPS_MODE_DISABLED = 4,
  RFFW_LTE_PA_SMPS_MODE_MAX_PDM = 5
} rffw_lte_smps_char_table_type;

/*! @brief PA control mode  */
typedef enum
{
  RFFW_LTE_XPT_MODE_APT,
  RFFW_LTE_XPT_MODE_EPT,
  RFFW_LTE_XPT_MODE_ET,
  RFFW_LTE_XPT_MODE_MAX
}rffw_lte_xpt_mode_type;

typedef enum
{
   RFFW_LTE_APT_ONLY,
   RFFW_LTE_EPT_ONLY,
   RFFW_LTE_ET_OR_EPT,
   RFFW_LTE_ET_AND_EPT
}rffw_lte_dpd_cfg_type;

typedef enum
{
    RFFW_LTE_BW_1P4     = 0,
    RFFW_LTE_BW_3       = 1,
    RFFW_LTE_BW_5       = 2,
    RFFW_LTE_BW_10      = 3,
    RFFW_LTE_BW_15      = 4,
    RFFW_LTE_BW_20      = 5,
    RFFW_LTE_NUM_OF_BW
    
}rffw_lte_bw_type_s;

/*! @brief This is the enum for specifying engine type for grfc engines. If the engine is invalid,
 *  then FW will not do the toggling for that engine. The invalid case is in effect for future grfc
 *  engines starting from gnss_grfc.
 */
typedef enum
{
  RFFW_INTF_GRFC_ENGINE      = 0,
  RFFW_INTF_RF_GRFC_ENGINE   = 1,	
  RFFW_INTF_INVALID_GRFC_ENGINE   = 2,
} rffw_intf_grfc_hw_source_s;

/*! 
 * @brief The following is used to specify the list of HDET sequences to be populated by RF-SW and 
 * executed by FW, if the sequence is enabled.  
 */
typedef enum
{
 RFFW_LTE_HDET_START, 
 RFFW_LTE_HDET_HPM_TRIGGER_CONV_DIS_LOW_BW,
 RFFW_LTE_HDET_HPM_TRIGGER_CONV_DIS_MID_BW, 
 RFFW_LTE_HDET_HPM_TRIGGER_CONV_DIS_HIGH_BW,
 RFFW_LTE_HDET_LPM_TRIGGER_CONV_DIS_LOW_BW,
 RFFW_LTE_HDET_LPM_TRIGGER_CONV_DIS_MID_BW,
 RFFW_LTE_HDET_LPM_TRIGGER_CONV_DIS_HIGH_BW,
 RFFW_LTE_HDET_WR_SEQ_NUM,
 RFFW_LTE_HDET_WR_SEQ_INVALID = RFFW_LTE_HDET_WR_SEQ_NUM
}rffw_lte_hdet_wr_seq_type_s;

/*! 
 * @brief The following is used to specify the list of HDET RD sequences to be populated by RF-SW and 
 * executed by FW, if the sequence is enabled. 
 *  
 * Please note that the HDET Reads are done in the FW 1 Subframe after HDET write sequence 
 *  
 */
typedef enum
{
 RFFW_LTE_HDET_RD,
 RFFW_LTE_HDET_RD_SEQ_NUM,
 RFFW_LTE_HDET_RD_SEQ_INVALID = RFFW_LTE_HDET_RD_SEQ_NUM
}rffw_lte_hdet_rd_seq_type_s;

/*! 
 * @brief The following is used to specify the list of Thermistor sequences. Each sequence contains a set of 
 * actions. FW will need to execute all of these sequences for every therm read.
 */
typedef enum
{
 RFFW_LTE_THERM_INIT,
 RFFW_LTE_THERM_CONV_DISABLE,
 RFFW_LTE_THERM_WR_SEQ_NUM,
 RFFW_LTE_THERM_WR_SEQ_INVALID = RFFW_LTE_THERM_WR_SEQ_NUM
}rffw_lte_therm_wr_seq_type_s;

/*! 
 * @brief The following is used to specify the list of Thermistor sequences. Each sequence contains a set of 
 * actions. FW will need to execute all of these sequences for every therm write.
 */
typedef enum
{
 RFFW_LTE_THERM_RD,
 RFFW_LTE_THERM_RD_SEQ_NUM,
 RFFW_LTE_THERM_RD_SEQ_INVALID = RFFW_LTE_THERM_RD_SEQ_NUM
}rffw_lte_therm_rd_seq_type_s;

/*! 
 * @brief The following is used to specify the list of SSBI actions supported. These are sepcified in the HDET/Therm 
 * sequence.
 */
typedef enum
{
 RFFW_LTE_SSBI_WRITE,
 RFFW_LTE_SSBI_READ,
 RFFW_LTE_DELAY,
 RFFW_LTE_SSBI_MASKED_WRITE,
 RFFW_LTE_ACTION_NUM,
 RFFW_LTE_ACTION_INVALID = RFFW_LTE_ACTION_NUM
}rffw_lte_actions_type_s;

/*! 
 * @brief The following is used to specify the list of HDET sequences to be populated by RF-SW and 
 * executed by FW, if the sequence is enabled.  
 */
typedef enum
{
 RFFW_LTE_HDET_WR_CCS_START, 
 RFFW_LTE_HDET_WR_CCS_LPM_TRIGGER_CONV_DIS_HIGH_BW, 
 RFFW_LTE_HDET_WR_CCS_LPM_TRIGGER_CONV_DIS_MID_BW, 
 RFFW_LTE_HDET_WR_CCS_LPM_TRIGGER_CONV_DIS_LOW_BW, 
 RFFW_LTE_HDET_WR_CCS_HPM_TRIGGER_CONV_DIS_HIGH_BW, 
 RFFW_LTE_HDET_WR_CCS_HPM_TRIGGER_CONV_DIS_MID_BW, 
 RFFW_LTE_HDET_WR_CCS_HPM_TRIGGER_CONV_DIS_LOW_BW, 
 RFFW_LTE_HDET_RD_CCS,
 RFFW_LTE_THERM_WR_CCS_INIT,
 RFFW_LTE_THERM_WR_CCS_CONV_DISABLE,
 RFFW_LTE_THERM_RD_CCS,
 RFFW_LTE_RX_ON_WR_ON_CCS,
 RFFW_LTE_RX_ON_WR_OFF_CCS,
 RFFW_LTE_TX_ON_WR_ON_CCS,
 RFFW_LTE_TX_ON_WR_OFF_CCS,
 RFFW_LTE_TX_EN_WR_ON_CCS,
 RFFW_LTE_TX_EN_WR_OFF_CCS,
 RFFW_LTE_SMPS_EN_WR_ON_CCS,
 RFFW_LTE_SMPS_EN_WR_OFF_CCS,
 RFFW_LTE_TDD_ASM_RX_WR_CCS,
 RFFW_LTE_TDD_ASM_TX_WR_CCS,
 RFFW_LTE_RX_EN_WR_ON_CCS,
 RFFW_LTE_RX_EN_WR_OFF_CCS,
 RFFW_LTE_RX_EN_RX_ON_WR_OFF_CCS,
 RFFW_LTE_TX_EN_TX_ON_WR_ON_CCS,
 RFFW_LTE_TX_EN_TX_ON_WR_OFF_CCS,
 RFFW_LTE_PAPM_ON_CCS,
 RFFW_LTE_PAPM_OFF_CCS,
 RFFW_LTE_SCELL_RX_ON_WR_ON_CCS,
 RFFW_LTE_SCELL_RX_ON_WR_OFF_CCS,
 RFFW_LTE_TDET_TEMP_COMP_CCS,
 RFFW_LTE_CSS_EVENT_NUM
}rffw_lte_ccs_event_type_s;

typedef struct 
{
   uint16   grfc_engine_no;
   rffw_intf_grfc_hw_source_s   grfc_engine_type;
} rffw_lte_grfc_unit_s;

/*! @brief This is the datatype for SSBI setting information for the TXAGC gain from RF Cal.
 * 
 */
typedef struct
{
   /*! @brief this field contains the gain index(RGI) from the Tx Linearizer
    *  cal
	*/ 
   uint16 tx_gain_index;

   /*! @brief this field contains the information for the measured tx gain during 
    *  RF calibration. FW should use only the last 10 bits of tx_gain_meas 
    *  is used.
	*/
   uint16 tx_gain_meas;

   /*! @brief this union contains the information for the actual SBI settings for the 
    *  gain index. Each Tx gain index has four SSBI writes. This field contains the value
    *  to be programmed for each of the SSBI addresses mentioned as part of rffw_lte_tx_gain_addr_s
    *  RF SW will use the sbi_setting_word to populate the setting and RF FW will use the sbi0_setting
    *  sbi1_setting, sbi2_setting, sbi3_setting fields to program the device. It is important that
    *  the sequence of SSBI writes are done from MSB->LSB.
    *  
    *  For example: if ssbi_addr_word field from rffw_lte_tx_gain_addr_s type has 0xB8B9BABB and sbi_setting_word
    *  has 0xFE9FBC01, the following sequence should be followed by FW while doing TxAGC writes.
    *  Register Address     Value
    *  0xB8            ->   0xFE
    *  0xB9            ->   0x9F
    *  0xBA            ->   0xBC
    *  0xBB            ->   0x01
    *  
    *  Note: The LSB of the ssbi_addr_word contains the register for TxAGC latch. If that is not written at the end,
    *  as the last write; the change made for Tx Gain Index will not take effect; which cause Tx characteristics
    *  tests to fail.
    *  
    */
   union 
   {
     struct 
     {
       uint8 sbi3_setting;
       uint8 sbi2_setting;
       uint8 sbi1_setting;
       uint8 sbi0_setting;
     };
     uint32 sbi_setting_word;
   };
} rffw_lte_tx_gain_s;

/*! @brief This is the datatype for SSBI address information for the TXAGC gain from RF Cal.
 * 
 */
typedef struct
{
  /*! @brief this union contains the information for the actual SBI address for the 
   *  gain index. Each Tx gain index has four SSBI writes. This field contains the address of the
   *  register to be programmed. The values to be programmed for this register are mentioned in the
   *  rffw_lte_tx_gain_s structure. RF SW will use the sbi_addr_word to populate the setting and RF FW
   *  will use the sbi0_addr, sbi1_addr, sbi2_addr, sbi3_addr fields to program the device. It is important that
   *  the sequence of SSBI writes are done from MSB->LSB.
   *  
   *  For example: if ssbi_addr_word field from rffw_lte_tx_gain_addr_s type has 0xB8B9BABB and sbi_setting_word
   *  field from rffw_lte_tx_gain_s structure has 0xFE9FBC01, the following sequence should be followed by FW while doing
   *  TxAGC writes.
   *  
   *  Register Address     Value
   *  0xB8            ->   0xFE
   *  0xB9            ->   0x9F
   *  0xBA            ->   0xBC
   *  0xBB            ->   0x01
   *  
   *  Note: The LSB of the ssbi_addr_word contains the register for TxAGC latch. If that is not written at the end,
   *  as the last write; the change made for Tx Gain Index will not take effect; which cause Tx characteristics
   *  tests to fail.
   *  
   */
  union 
  {
    struct 
    {
     uint8 sbi3_addr;
     uint8 sbi2_addr;
     uint8 sbi1_addr;
     uint8 sbi0_addr;
    };
    uint32 sbi_addr_word;    
  };
  uint8 sbi_bus;
} rffw_lte_tx_gain_addr_s;

typedef struct
{

   uint8 sbi0_page;
   uint8 sbi1_page;
   uint8 sbi2_page;
   uint8 sbi3_page;
   uint8 sbi4_page;
   uint8 sbi5_page;
   uint8 sbi6_page;
   uint8 sbi7_page;

   uint8 sbi0_addr;
   uint8 sbi1_addr;
   uint8 sbi2_addr;
   uint8 sbi3_addr;
   uint8 sbi4_addr;
   uint8 sbi5_addr;
   uint8 sbi6_addr;
   uint8 sbi7_addr;

   uint8 sbi0_setting;
   uint8 sbi1_setting;
   uint8 sbi2_setting;
   uint8 sbi3_setting;
   uint8 sbi4_setting;
   uint8 sbi5_setting;
   uint8 sbi6_setting;
   uint8 sbi7_setting;

} rffw_lte_hdet_filter_sbi_s;

typedef struct
{
  boolean et_flag;
  uint32 txlm_buf_handle; 
} rffw_et_mode_enable_s;

typedef struct
{
  uint16 am_offsets[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  uint16 am_bank_id[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  uint16 pm_offsets[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  uint16 pm_bank_id[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  uint16 det_offsets[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  uint16 det_bank_id[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  uint16 iq_gain_lut[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  uint8 am_tbl_size;
  uint8 pm_tbl_size;
  uint8 det_tbl_size;
}rffw_lte_xpt_txagc_s;


/*!
  @brief
  Holds the PA related settings.
*/
typedef struct 
{
   boolean   pa_r0;
   boolean   pa_r1;
} rffw_lte_txagc_pa_s;


/*! @brief The following structure contains the PA rise thresholds */
typedef struct
{
	int32    pa_rise_r0_to_r1;
	int32    pa_rise_r1_to_r2;
	int32    pa_rise_r2_to_r3;

} rffw_lte_pa_threshold_s;

/*! @brief The following structure contains the PA minimum supported pwr thresholds */
typedef struct 
{
	int32    pa_min_pwr_r1_to_r0;
	int32    pa_min_pwr_r2_to_r1;
	int32    pa_min_pwr_r3_to_r2;
} rffw_lte_pa_min_pwr_threshold_s;

/*!
 * @brief The following structure holds the current PA state.
 */
typedef struct
{
   uint8 data;
   uint8 srs;
} rffw_lte_current_pa_state_s;

/*! 
 * @brief This structure to define the GRFC engines and logic that FW need to 
 * act/deact when transition from RX<->TX 
 */
typedef struct ALIGN(32)
{
  uint32 number_of_engines;
  uint16 engine_number[MAX_ENGINES];          /* valid value: 0-23 */
  uint16 engine_value[MAX_ENGINES];           /* 1 = ACTIVATE, 0 = DEACTIVATE */
}rffw_lte_tdd_grfc_s;

/*!
 * @brief The following structure is designed for the variables that are updated
 * during runtime and whose updates are sent to QDSP6 via a MSGR message
 * and the update in the Shared Memory is performed by FW code when they
 * empty their MSGR queue.
 */
typedef struct ALIGN(32)
{
    /*! @brief RX_ON GRFC information.*/
    rffw_lte_grfc_unit_s rx_on;
    /*! @brief RX_ON GRFC information.*/
    rffw_lte_grfc_unit_s tx_on;
    /*! @brief RX_ON GRFC information.*/
    rffw_lte_grfc_unit_s pa_on;
    /*! @brief PA RANGE GRFC information.*/
    rffw_lte_grfc_unit_s pa_r0;
    rffw_lte_grfc_unit_s pa_r1;

    /*! @brief DRX DP4T GRFC information.*/
    rffw_lte_grfc_unit_s drx_dp4t;
    
    /*! @brief GRFC engines for TDD */
    /*! @brief RX: In TDD when transittion from TX->RX the following grfc engines need to ACT/DEACT accordingly */
    rffw_lte_tdd_grfc_s rx_grfc;

    /*! @brief TX: In TDD when transittion from RX->TX the following grfc engines need to ACT/DEACT accordingly */
    rffw_lte_tdd_grfc_s tx_grfc;

    /*! @brief GNSS Blanking: In TDD, when transition from RX->TX the following grfc engine need to ACT/DEACT 
      to disable GNSS Baseband. The timing for this grfc is with reference to PA_ON rise and fall and delays are
      specififed through Rise/Fall shared memory variables.     
      */
    rffw_lte_grfc_unit_s gnss_grfc;

    /*! @brief The following buffer is added to basically make sure when new items
    * are added into this structure, backwards compaitbility is maintained
    * and if FW and SW have different versions of the interface file, it will not cause 
    * a shift. Everytime a new item is added into this structure, a similar amount needs 
    * to be reduced from the buffer below.*/
   uint32 rffw_lte_grfc_buffer[54];    
    

} rffw_lte_grfc_s;

/*!
 * @brief The following structure is designed for the variables of TXAGC. These 
 * variables are updated during band change, channel change or initialization
 * and hence can be directly written to the uncached memory space by
 * Q6SW. On the FW side, they will be invalidated if the counter is different 
 * than the previous value. The items in this list should be one way only 
 * SW --> FW. These items should not be read by SW back. 
 */
typedef struct ALIGN(32)
{

   /*!
    * @brief The following variable contains the default timer hysteresis value.
    * UPDATED: Initialization.
    */
   uint8 timer_hyst_val;

   /*! 
    * @brief The following array contains the PA swpts.
    * UPDATED: Band change.
    */
   int32 pa_swtpts[RFFW_LTE_MAX_NUM_PA_RANGES];

   /*! 
    * @brief The following array contains the "panic threshold" value for each PA state.
    * UPDATED: Band change.
    */
   int32 min_pwr_per_pa_state[RFFW_LTE_MAX_NUM_PA_RANGES];

   /*! 
    * @brief The following value holds the current SBI settings.
    * UPDATED: Possibly every sub-frame by RFFW.
    */
   uint8 sbi_setting[RFFW_LTE_NUM_TX_GAIN_SBI_REGS];
   uint8 srs_sbi_setting[RFFW_LTE_NUM_TX_GAIN_SBI_REGS];

   /*! 
    * @brief The following variable denotes which PA states are actively being used for
    * a given band. Not all four PA states are used for every band.
    * UPDATED: Band change.
    */
   boolean active_pa_states[RFFW_LTE_MAX_NUM_PA_RANGES];

   /*! 
    * @brief The following variables denote the necessary PA_R0 and PA_R1 GRFC setting 
    * for each of the entries of the active_pa_states variable.
    * UPDATED: Initialization.
    */
   rffw_lte_txagc_pa_s pa_setting[RFFW_LTE_MAX_NUM_PA_RANGES];

   /*! 
    * @brief The following tables denote the measured power level of the respective settings 
    * UPDATED: Channel Change.
    */
   rffw_lte_tx_gain_s tx_lin_tbl0[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
   rffw_lte_tx_gain_s tx_lin_tbl1[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
   rffw_lte_tx_gain_s tx_lin_tbl2[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
   rffw_lte_tx_gain_s tx_lin_tbl3[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];

   /*! 
    * @brief The following variables contain the SBI addresses for the settings provided 
    * in the settings table. 
    * UPDATED: Band Change.
    */
   rffw_lte_tx_gain_addr_s tx_gain_addr;

   /*! 
    * @brief The following table is OBSOLETE, as the freq comp method is changed.
    */
   int32 freq_comp_table[RFFW_LTE_MAX_NUM_PA_RANGES*RFFW_LTE_MAX_NUM_FREQ_COMP];


   /*!
    * @brief Specifies the time between early TX time and gain SBI 
    */
   uint16  tx_agc_update_time;

   /*!
    * @brief The below variables hold the MPR based backoff values 
    * for PWR level and PA swpts. 
    */
   int16 mpr_array[NUM_MPR_IND];
   int16 mpr_array_for_pa_swpts[NUM_MPR_IND];
   int16 mpr_txagc_offset[NUM_MPR_IND];
   int16 mpr_digital_gain_comp[NUM_MPR_IND];

   /*!
    * @brief contains the time delta between the time reference                                                                              
    * signal and PA_RANGE updated for the transition from low to                                                                             
    * the high gain state, or from high to low gain state                                                                                     
    * The unit is in LTE 1 sample time. Conversion to us is as follows: 
    * int(time_in_us * 30.72 ).                                                                                                               
    */
   uint16 pa_up_time;
   uint16 pa_down_time;

   /*! 
    * @brief contains time delta between PA_ON\TX_ON signal rise\fall and 
    * the start\end of the TX frame boundary. The TX Gain SBI should be sent 
    * right after the TX_ON on is turned on but before PA_ON si turned on. 
    * The unit is in LTE 1 sample time. Conversion to us is as follows: 
    * int(time_in_us * 30.72 ). 
    */
   uint16  pa_on_rise_delay;
   uint16  pa_on_fall_delay;
   uint16  tx_on_rise_delay;
   uint16  tx_on_fall_delay;

   /*! 
    * @brief contains time delta between GNSS_ON and PA_ON rise/fall delay.
    * The unit is in LTE 1 sample time. Conversion to us is as follows: 
    * int(time_in_us * 30.72 ). 
    */
   uint16  gnss_on_rise_delay;
   uint16  gnss_on_fall_delay;

   /*! 
    * @brief The following value holds the maximum timer 
    * hysteresis value. Please do not confuse it with 
    * the current_timer_hyst_val which is decremented. 
    * The following holds the maximum timer hysteresis 
    * value. The unit is in LTE subframes.
    */
   int16   max_timer_hysterisis;

   /*!
    * @brief PA rotator angles comp values for each PA state
    */
   int16  pa_rot_angle_comp[RFFW_LTE_MAX_NUM_PA_RANGES];

   /*! 
    * @brief These variables are used to bypass the FW provided
    * values for debugging purposes.
    */
   boolean oride;
   int32 oride_val;

   boolean pa_oride;
   boolean pa_oride_r0_val;
   boolean pa_oride_r1_val;

   boolean sbi_oride;
   uint8 oride_sbi0;
   uint8 oride_sbi1;
   uint8 oride_sbi2;
   uint8 oride_sbi3;

   boolean dig_backoff_oride;
   int32 dig_backoff_val;

   int32 temp_comp[RFFW_LTE_MAX_NUM_PA_RANGES];

   /*! @brief The following structures will hold the lin vs. freq values per PA state for each slot. 
    * The reason it is done per slot is to ensure that the right value is used for the right SF. 
    * In the old approach, we did not have this precision, and this is to avoid corner case errors. 
    * This is also especially needed for TDD where the update from RFSW for the freq. comp. can come                                                                                           .
    * up to 7 SF ahead of the actual transmission SF. The support for slot2 and SRS is also added for                                                                                                                                                                                        .
    * the first time.                                                                                                                                                                                                                                                                                       .
    */
   int32 lin_vs_freq_slot1[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES];
   int32 lin_vs_freq_slot2[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES];
   int32 lin_vs_freq_srs[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES];


   uint8 pa_smps_mode;

   boolean ftm_txagc_flag;

   /*! @brief The variable is_ftm_cw should be set to FALSE by RF SW team by default. They should 
	 set it to TRUE when ALL the following conditions are met:
	 1. we're in FTM mode
	 2. the phychan schedule for the FTM mode is PUSCH
	 3. RF FTM wishes to send CW instead of LTE waveform.
	*/
   boolean is_ftm_cw;
   uint8   padding_for_is_ftm_cw[3];  // padding for 32 bit alignment
   uint32  ftm_cw_rot_phase_inc;	  // tone offset converted to rotator phase increment, 29S28


   /*! @brief These address and value pairs are used to tune the Tx PLL by FW. 
       A flag is also used to communicate if the TX PLL has been tuned by RF SW.
    */
   uint8 tx_pll_int_msbs_addr;
   uint8 tx_pll_int_lsbs_addr;
   uint8 tx_pll_frac_msbs_addr;
   uint8 tx_pll_frac_mids_addr;
   uint8 tx_pll_frac_lsbs_addr;

   uint8 tx_pll_int_msbs_val;
   uint8 tx_pll_int_lsbs_val;
   uint8 tx_pll_frac_msbs_val;
   uint8 tx_pll_frac_mids_val;
   uint8 tx_pll_frac_lsbs_val;

   uint8 tx_pll_tuned_flag;

   /*!
    * @brief Contains current Tx antenna index that needs to be configured
    */
   uint8 curr_tx_ant_index;

    /*!
    * @brief Flag to tell TM8 SRS that a switch from AsDiv is in progress to 
    * gracefully handle concurrency scenarios 
    */
   boolean sw_in_proc;

   /*!
    * @brief Index to specify the double buffer script index to be used to 
    * populate TM8 SRS Antenna Switch settings
    */
   uint8 ant_s_buf_idx;

   /*! @brief The following buffer is added to basically making sure when new items
    * are added into this structure, backwards compaitbility is maintained
    * and if FW and SW have different versions of the interface file, it will not cause 
    * a shift. Everytime a new item is added into this structure, a similar amount needs 
    * to be reduced from the buffer below.*/
   uint8 rffw_lte_txagc_buffer_8[29];
   uint32 rffw_lte_txagc_buffer[118];

}rffw_lte_txagc_s; 

/*! @brief The rffw_lte_enable_hdet_sequence_bw_data_type contains the lookup table for enable sequences 
  based on the number of Resource Blocks(RB's). This structure is used for swtiching enablement of HDET sequences based on
  tx_agc power and LOW_MID_HIGH HDET filter BW. RFSW will populate this data and FW will use this data to retrieve
  the sequences to be enabled based on RB's in the subframe. */
typedef struct 
{ 
  /*! @brief The start_rb field contains the minimum num of RB's required to use this 
  enable sequence comination.*/
  uint8 start_rb;

  /*! @brief The end_rb field contains the minimum num of RB's required to use this 
  enable sequence comination.*/
  uint8 end_rb;

  /*! @brief This field contains the  list of hdet writesequences to be executed for this 
    agc and rb combination.*/
  boolean seq_enable[RFFW_LTE_HDET_WR_SEQ_NUM];
} rffw_lte_enable_hdet_wr_seq_bw_data_type;

/*! @brief The rffw_lte_enable_hdet_sequence_agc_data_type contains the lookup table for tx agc 
  switch point. This structure is used for switching between High power HDET and Low power HDET.
  RFSW will populate this data and FW will use this data to retrieve the sequences to be enabled
  based on tx_agc*/
typedef struct
{
/*! @brief The start_agc_pwr field contains the minimum tx agc cntr (power) required to use this 
  enable sequence comination.*/
  uint16 start_agc_pwr;

/*! @brief The end_agc_pwr field contains the maximum tx agc cntr (power) required to use this 
  enable sequence combination. */
  uint16 end_agc_pwr;

/*! @brief The rffw_lte_enable_hdet_sequence_bw_data_type is an array of RFFW_LTE_HDET_RB_CONFIGS which 
  provides the look up table for enable_sequences based on number of resource blocks in that sf. */
  rffw_lte_enable_hdet_wr_seq_bw_data_type enable_seq_bw[RFFW_LTE_HDET_RB_CONFIGS];
} rffw_lte_enable_hdet_wr_seq_agc_data_type;

/*! @brief
  SSBI Value : 1 Byte SSBI Data read from HW
  SSBI Status (LSByte of SSBI status REG)
*/
typedef struct
{
 uint8 value;
 uint8 status;   
                 
} rffw_lte_ssbi_rd_data_type;

/*! @brief The rffw_lte_ssbi_action_data_type contains details about the action to be performed. 
  This data structure is typically used for HDET Sequences, Therm Sequences. */
typedef struct
{
   /*! @brief The action_id field contains the type of action to be performed. The actions could be any of 
    the types listed in the rffw_lte_actions_type_s enum. */
    rffw_lte_actions_type_s action_id;

    /*! @brief The ssbi_reg_addr field contains the SSBI register to be used for performing actions pertaining 
     to SSBI write or read. RFSW will populate the SSBI register to be used for the action and FW will use this
     field while performing the action*/
    uint8 ssbi_reg_addr;

    /*! @brief If the action is a SSBI Write operation, then ssbi_value contains the value of the 
      register to be written. This value is populated by RF SW for every action.
     
      If the action is a SSBI Masked Write operation, then ssbi_value contains the masked_value of the 
      register to be written. This value is populated by RF SW for every action.
     
      If the action is SSBI Read operation, then ssbi_value contains the value of the register after
      the operation is completed. This value is populated by FW after read operation is completed.
     
      The mask field is added in the union for documentation purposes only so that it is intuitive that
      this value is intended for ssbi_valuie and mask.
      */
    union
    {
     uint8 ssbi_value;
     uint8 mask;
     uint16 delay;
     rffw_lte_ssbi_rd_data_type rd_value_status;
    };
} rffw_lte_ssbi_action_data_type;

/*! @brief The following structure holds the HDET related info.*/
typedef struct ALIGN(32)
{
    
    /*! @brief The following variable is used in accordance hdet_trigger_allowed 
     * flag. 
     *  
     * When FW's HDET timer is up, FW checks for the hdet_trigger_allowed flag. If 
     * hdet_trigger_allowed is TRUE, FW schedules HDET SBIs and sets hdet_process_active 
     * to TRUE and clear the shared Memory cache. hdet_process_active flag is to make sure that 
     * SW will not issue any temperature reads while HDET loop is active. When FW wakes up 
     * one subframe later, FW sends the HDET read message to RFSW and FW also sets the  
     * hdet_trigger_allowed flag to FALSE, to not to trigger another HDET write until the 
     * previous HDET value is read by RFSW. 
     *  
     * RFSW sets hdet_trigger_allowed flag to TRUE and hdet_process_active flag to 
     * FALSE at init. Once it runs the HDET loop, it sets the hdet_trigger_allowed flag back to 
     * TRUE and hdet_process_active flag to FALSE. 
     *  
     * Also while running the TEMP COMP, RFSW first needs to check hdet_process_active to be FALSE. 
     * If it is TRUE, RFSW skips the temperature read. Then, while reading temperature, RFSW needs 
     * to set hdet_trigger_allowed flag to FALSE.
     *  
     */
    boolean hdet_trigger_allowed; 
        
    /*! @brief The following structure holds the HDET loop timer, 
     *  and this changes whether we are in ACK or TRACK mode.
     *  The unit will be in subframe counts (i.e. ms)
     */
    uint32 loop_timer_period;

    /*! @brief The following value is obsolete. A new 
     * item is created.
     */
    int32 loop_output;
    
    /*! @brief The following holds the HDET SBI bus ID.
     */
    uint8 hdet_sbi_bus;    
        
    /*! @brief The following variable holds the threshold 
     * that is used to switch from low power PDET mode to 
     * high power PDET mode.
     */
    uint16 pdet_mode_swpt;

    /*! @brief The following variable holds the threshold 
     *  that is used to decide whether or not to trigger the
     *  hdet process. This variable is updated by RFSW when
     *  there is a change in the MPR, AMPR, PEMAX, etc.
     */
    int16 enable_swpt;

    /*! @brief The following holds the filter SBI settings that needs to 
     * be sent out to start the filter. 
     *  
     * Since each filter will have a different settling time, these are 
     * captured respectively in filter_lo_bw_tc, filter_mid_bw_tc and 
     * filter_hi_bw_tc. 
     *  
     * In LTE, there will also be a lower threshold then UMTS, and there 
     * is a hysteresis based on the power levels in choosing which mode 
     * PDET operates in. These thresholds are held in lo_pwr_swpt and 
     * hi_pwr_swpt. The unit for these swpts are in TXAGC counts.
     *  
     * The SBI settings are in lo_pwr_mode_start or hi_pwr_mode_start. 
     * There is a possibility of 8 SBIs for each case. The num_sbi 
     * variable will tell us how many of them are valid.  
     *  
     * In LTE, the way we will decide which filter to use will depend on 
     * the number of RBs. If the number of RBs is less than filter_lo_bw_rb_swpt, 
     * then we use low bw (suc as 4KHz) filter. If it is greater than 
     * filter_lo_bw_rb_swpt but less than filter_mid_bw_rb_swpt, then we use the 
     * mid bw (such as 11KHz) filter. If it is greater than filter_mid_bw_rb_swpt but 
     * less than filter_hi_bw_rb_swpt, then we use the hi bw (such as 38KHz) filter.
     * SRS implementation: filter_hi_bw_rb_swpt_srs is for SRS implementation. We always 
     * use high bw for SRS if the number of RBs is higher than filter_hi_bw_rb_swpt_srs.
     * 
     *  
     * The SBI settings are in respective structures based on the PDET power mode 
     * and the filter type. 
     *  
     */
    uint16 filter_lo_bw_rb_swpt;
    uint16 filter_mid_bw_rb_swpt;
    uint16 filter_hi_bw_rb_swpt;

    /*! @brief The following variable is used in accordance hdet_trigger_allowed 
     * flag. 
     *  
     * When FW's HDET timer is up, FW checks for the hdet_trigger_allowed flag. If 
     * hdet_trigger_allowed is TRUE, FW schedules HDET SBIs and sets hdet_process_active 
     * to TRUE and clear the shared Memory cache. hdet_process_active flag is to make sure that 
     * SW will not issue any temperature reads while HDET loop is active. When FW wakes up 
     * one subframe later, FW sends the HDET read message to RFSW and FW also sets the  
     * hdet_trigger_allowed flag to FALSE, to not to trigger another HDET write until the 
     * previous HDET value is read by RFSW. 
     *  
     * RFSW sets hdet_trigger_allowed flag to TRUE and hdet_process_active flag to 
     * FALSE at init. Once it runs the HDET loop, it sets the hdet_trigger_allowed flag back to 
     * TRUE and hdet_process_active flag to FALSE. 
     *  
     * Also while running the TEMP COMP, RFSW first needs to check hdet_process_active to be FALSE. 
     * If it is TRUE, RFSW skips the temperature read. Then, while reading temperature, RFSW needs 
     * to set hdet_trigger_allowed flag to FALSE.
     *  
     */
    boolean hdet_process_active;

   /*! @brief The following structures will hold the MTPL values for each slot. 
    * The reason it is done per slot is to ensure that the right value is used for the right SF. 
    * In the old approach, we did not have this precision, and this is to avoid corner case errors. 
    * This is also especially needed for TDD where the update from RFSW for the MTPL can come                                                                                           .
    * up to 7 SF ahead of the actual transmission SF. The SRS MTPL is also supported for the first time.                                                                                                                                                                                        .
    */
    int32 hdet_mtpl[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME];
    int32 hdet_mtpl_srs[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME];

    /* Amount of time before the next therm read  */  
    int32 therm_state_next_read_offset;  /* in msecs */
    int32 tdet_temp_comp_next_read_offset;
    boolean tdet_temp_comp_enable;

    /*! @brief The enable_hdet_therm_v2 flag is used to distinguish between the old interface and sequence of scripts based
    interface. This flag is set to FALSE for RTR8600 based targets by RFSW and set to TRUE for WTR1605 and
    future targets.*/  
    boolean enable_hdet_therm_v2;

    /*! @brief These are for future use if different SSBI Core masters
      will be used. */
    uint8 hdet_ssbi_master_id_1;
    uint8 hdet_ssbi_master_id_2;
    /*! @brief This byte is not used, but is needed to be word aligned.*/
    uint8 hdet_byte_buffer2;

    /*! @brief The hdet_sequences is a two-dimensional array of HDET sequences and maximum actions per sequence.
      The maximum no of actions per sequence is determined by RFFW_LTE_MAX_ACTIONS_PER_HDET_WR_SEQ macro. However,
      each sequence usually has less than the maximum no of actions per sequence. Hence, to mark the termination
      point to the list of actions per sequence, RF SW will populate the action_id=RFFW_LTE_ACTION_INVALID
      only one of the below list should be used as action_id:
       RFFW_LTE_SSBI_WRITE
       RFFW_LTE_DELAY
       RFFW_LTE_ACTION_INVALID
     */
    rffw_lte_ssbi_action_data_type hdet_wr_seqs[RFFW_LTE_HDET_WR_SEQ_NUM][RFFW_LTE_MAX_ACTIONS_PER_HDET_WR_SEQ];

    /*! @brief The hdet_enable_sequences is a list of sequences to be executed based on Tx AGC power 
      threshold and num of resource blocks per sub frame. This information is populated by RFSW.
      FW will use this information to make a decision on the sequences to be executed based on the
      number of resource blocks and tx agc power*/
    rffw_lte_enable_hdet_wr_seq_agc_data_type hdet_enable_seqs [RFFW_LTE_HDET_TX_PWR_CONFIGS];

    rffw_lte_ssbi_action_data_type hdet_rd_seqs[RFFW_LTE_HDET_RD_SEQ_NUM][RFFW_LTE_MAX_ACTIONS_PER_HDET_RD_SEQ];

    /*! @brief The therm_sequences is a two-dimensional array of Therm sequences and no of max actions per sequence */
    rffw_lte_ssbi_action_data_type therm_wr_seqs[RFFW_LTE_THERM_WR_SEQ_NUM][RFFW_LTE_MAX_ACTIONS_PER_THERM_WR_SEQ];

    rffw_lte_ssbi_action_data_type therm_rd_seqs[RFFW_LTE_THERM_RD_SEQ_NUM][RFFW_LTE_MAX_ACTIONS_PER_THERM_RD_SEQ];

     /*! @brief The following flag is used to let FW know that the HDET CCS scripts 
     * are being updated and hence FW should not try to access the HDET scripts during 
     * this time whenever the flag is TRUE. If FW accesses the HDET scripts when its 
     * being updated, the update will fail and the HDET CCS scripts will be invalid. 
     * This is a race condition and can cause the UE to crash.   
     */
    boolean hdet_update_active;

   /*! @brief The following buffer is added to basically making sure when new items
    * are added into this structure, backwards compaitbility is maintained
    * and if FW and SW have different versions of the interface file, it will not cause 
    * a shift. Everytime a new item is added into this structure, a similar amount needs 
    * to be reduced from the buffer below.*/
    uint32 rffw_lte_hdet_buffer[36]; 

    uint8 rffw_lte_hdet_buffer_8[15];
} rffw_lte_hdet_s;

/*! This structure describes the RxAGC gain word control settings per path */
typedef struct
{
   /*! SSBI bus for writing the Gainword (GW) settings */
   uint8 sbi_bus;

   /*! SSBI Address for the GW register */
   uint8 sbi_addr;

   /*! RxAGC GW settings for all the gain states supported */
   uint8 rx_gain_sbi_value[RFFW_LTE_MAX_NUM_RX_GAIN_RANGES];

   /*! RxAGC GW write time delta to be provided by RFSW */ 
   int8 rx_gain_sbi_time_delta;
} rffw_lte_rxagc_lna_gw_ctrl_s;

typedef struct ALIGN(32)
{
    	/*! 
	 * @brief The following value holds the current PA state.
     * UPDATED: Possibly every sub-frame by RFFW.
	 */
	rffw_lte_current_pa_state_s current_pa_state;		

    /*! 
     * @brief The following value holds the current timer hysteresis value.
     */
    int16 current_timer_hyst_val;

    /*! 
     * @brief The start RB index passed to the TXAGC helper function by FW.
     */
    uint8 start_rb_index;        

    /*! 
     * @brief The number of RBs passed to the TXAGC helper function by FW.
     */
    uint8 num_rbs;                        
    
    /*! 
     * @brief This variable holds the TXAGC value passed to the helper function 
     * by FW. This value is the actual power level used to calculate the HW settings, 
     * after all the compensation and limiting is applied.
     */            
    int32 rffw_lte_pwr_db10;
        
    /*! 
     * @brief This variable is the actual TX Gain Index used for the given TX 
     * PWR level to the TXAGC helper function by FW. 
     */            
    uint16 tx_gain_index;

    /*! 
     * @brief This variable holds the digital backoff needs to be performed by 
     *  FW in the baseband. 
     */  
    uint16 baseband_backoff;                      
   
    /*! 
     * @brief This variable holds the frequency compensation used most recently 
     */  
    int32 lin_vs_freq;

    /*! 
     * @brief This variable holds the temperature compensation used most recently 
     */  
    int32 lin_vs_temp;

    /*! 
     * @brief This variable holds the PA BIAS SMPS value.
     */  
    uint16 tx_pa_bias_val;

     /*! 
     * @brief  Add XPT related settings
     */
    rffw_lte_xpt_mode_type current_xpt_mode;
    boolean et_flag;
    boolean xpt_transition;
    rffw_lte_dpd_cfg_type dpd_cfg;
    uint8 am_tbl_size;
    uint8 pm_tbl_size;
    uint8 det_tbl_size;
    uint16 am_offsets;
    uint16 am_bank_id;
    uint16 pm_offsets;
    uint16 pm_bank_id;
    uint16 det_offsets;
    uint16 det_bank_id;
    uint16 iq_gain_lut;
    int32 p_out_comp;
    int32 p_in_comp;
    uint32 selected_rgi_with_pout;
    uint32 selected_rgi_with_pin;
    uint32 env_scale_backoff;
    uint32 iq_backoff;
    int32  delay;

   /*! @brief The following buffer is added to basically making sure when new items
    * are added into this structure, backwards compaitbility is maintained
    * and if FW and SW have different versions of the interface file, it will not cause 
    * a shift. Everytime a new item is added into this structure, a similar amount needs 
    * to be reduced from the buffer below. 
    */ 
     uint8 rffw_lte_txagc_fw2sw_buffer_2[3];
    uint32 rffw_lte_txagc_fw2sw_buffer[46];

} rffw_lte_txagc_fw2sw_s;


/*!
 * @brief The following structure is designed for the variables of RXAGC. These 
 * variables are updated during band change, channel change or initialization
 * and hence can be directly written to the uncached memory space by
 * Q6SW as there will be no read\write collision.
 */
typedef struct ALIGN(32)
{
   /*!
    * @brief The following table contains the DELTA_RX_GAIN_OFFSET values (one for each gain state).
    * The entries for this table will be initialized to 0 before calibration. During online mode,
    * it will be updated with the calibrated offsets for the operating RF band.
    * The unit is dB/10.
	 */
    int16 delta_rx_gain_offset[RFFW_LTE_MAX_NUM_RX_GAIN_RANGES];

   /*!
    * @brief The following table contains the DEFAULT_RX_GAIN_OFFSET values (one for each gain state).
    * The entries for this table will be initialized to the nominal gain offsets as per the device spec.
    * The unit is dB/10.
	 */  
    uint16 default_rx_gain_offset[RFFW_LTE_MAX_NUM_RX_GAIN_RANGES];

    /*!
    * @brief The following varible contains the nominal gain for G0. It will be initialized to the 
    * nominal gain for G0 as per device spec. 
    * The unit is dB/10.
	 */
    uint16 nominal_g0_gain;

   /*!
    * @brief The following variable contain the RX_GAIN rise and fall threshold values.
    * UPDATED: Band Change                                                   .
    *                    [||RX_GAIN_RISE||,||0x8000||] --> Note that 0x8000 is the largest negative value.
    *           bits:    31            16 15             0
    *  
    *                   [||0x7FFF||,||RX_GAIN_FALL||] --> Note that 0x7FFF is the largest positive value.
    *           bits:    31            16 15             0.
    * 
    *  The first RFFW_LTE_MAX_NUM_RX_GAIN_RANGES int32 values will be for RISE and the remaining
    *  RFFW_LTE_MAX_NUM_RX_GAIN_RANGES values will be for FALL.
    *  
    * 6 gain states hence 5 sets of rise/fall thresholds
    */
    int32 rx_gain_rise_fall_thrshlds[(RFFW_LTE_MAX_NUM_RX_GAIN_RANGES-1)*2];

    /* To be Deprecated going ahead after WTR 1625 BU */
    uint8 rx_gain_sbi_bus;  

    /* To be Deprecated going ahead after WTR 1625 BU */
    uint8 rx_gain_sbi_address;

    /* To be Deprecated going ahead after WTR 1625 BU */
    uint8 rx_gain_sbi_value[RFFW_LTE_MAX_NUM_RX_GAIN_RANGES];

    uint8 rx_gain_phase[RFFW_LTE_MAX_NUM_RX_GAIN_RANGES];

    /* To be Deprecated going ahead after WTR 1625 BU */
    uint8 rx_gain_sbi_time_delta;

    /*! @brief Specifies a flag for overriding RX gain state */
    uint8 rx_gain_force_oride_flag;

    uint8 rx_gain_force_write;

	int32 rx_gain_expected_rssi;

    uint16 rx_on_rise_delay;

    uint16 rx_on_fall_delay;

    /*! New structure to compact the RF Device settings 
       This is to incoporate LNA + PLL split for WTR 1625, 1620 combo */
    rffw_lte_rxagc_lna_gw_ctrl_s rx_gain_ctrl[RFFW_MAX_LNA_GW_SSBI_PER_PATH];
    
    /*! This field provide the total number of SSBI transactions to be done to tune the path
       The value of this field will depend on whether the current scenario is
       Inter Band CA, Intra-Band CA, Standalone. Currently the max this number
       can go to is 2 per path. These transactions   */
    uint8 rxagc_lna_write_trans_num;

    /*! This array contains the 5 LNA gain thresholds for rise and fall for 
        Intra Band CA case. This case needs to be taken care of separately */
    int32 intra_ca_rx_gain_rise_fall_thrshlds[(RFFW_LTE_MAX_NUM_RX_GAIN_RANGES-1)*2];

   /*! @brief The following buffer is added to basically making sure when new items
    * are added into this structure, backwards compaitbility is maintained
    * and if FW and SW have different versions of the interface file, it will not cause 
    * a shift. Everytime a new item is added into this structure, a similar amount needs 
    * to be reduced from the buffer below.*/

    uint8 rffw_lte_rxagc_buffer2[4];
    uint32 rffw_lte_rxagc_buffer[52];

} rffw_lte_rxagc_s;

typedef struct ALIGN(32)
{
    /*!
     * @brief 
     * The following variable is for reading the current RXAGC value (RSSI) from Q6SW. 
     * The unit is in 1/10 dBm. Q6FW updates this variable every 1 ms when rx agc loop
     * is running. This variable should not be written from Q6SW side. 
     */
    uint16 current_rx_agc_value;

    /*!
     * @brief 
     * The following variable is for reading the current RXAGC state from Q6SW. It is 
     * also used in Q6FW to track the current RXAGC state. This variable should not be written from Q6SW side. 
     *  
     * Everytime the other rx_gain  Shared Memory thresholds are updated, this following value needs 
     * to be reset to 2 (see todo below). 
     *  
     * @todo: ask systems what this needs to be (high\mid\low gain state); 
     */
    uint8 current_rx_gain_state;

    /*! @brief
     *  
     * The below hystheresis counters are for RXAGC Rise and Fall. 
     * They are provided here for read purposes. FW also uses the structure below 
     * to keep track of them. 
     *  
     * The values below should be set to RFFW_LTE_RX_GAIN_TIMER_HYSTERESIS_CONSTANT 
     * when all the other RXAGC stuff is initialized. 
     */

    uint8 current_rx_gain_hyst_cntr;  

    /*!
     * @brief 
     * The following variable is for reading the current inband RXAGC value 
     * (RSSI) from Q6SW. 
     * The unit is in 1/10 dBm. Q6FW updates this variable every 1 ms when 
     * rx agc loop is running. This variable should not be written from 
     * Q6SW side.
     */
    int16 current_rx_agc_inband_value;


   /*! @brief The following buffer is added to basically making sure when new items
    * are added into this structure, backwards compaitbility is maintained
    * and if FW and SW have different versions of the interface file, it will not cause
    * a shift. Everytime a new item is added into this structure, a similar amount needs
    * to be reduced from the buffer below.*/
    uint16 rffw_lte_rxagc_fw2sw_buffer[29];
} rffw_lte_rxagc_fw2sw_s;

typedef struct ALIGN(32)
{
    boolean tx_pa_bias_enabled;
    uint32 tx_pa_bias_time;
    uint16 tx_pa_bias_val0[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE]; 
    uint16 tx_pa_bias_val1[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE]; 
    uint16 tx_pa_bias_val2[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE]; 
    uint16 tx_pa_bias_val3[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE]; 

} rffw_lte_txagc_pa_bias_s;

typedef struct ALIGN(32)
{

   uint32 tx_cache_counter;
   uint32 rx_cache_counter;
   uint32 hdet_cache_counter;

   /*! @brief The following buffer is added to basically making sure when new items
    * are added into this structure, backwards compaitbility is maintained
    * and if FW and SW have different versions of the interface file, it will not cause 
    * a shift. Everytime a new item is added into this structure, a similar amount needs 
    * to be reduced from the buffer below.*/ 
   uint32 rffw_lte_counter_buffer[32];

} rffw_lte_counter_s;

/* To be Deprecated going ahead after WTR 1625 BU */
typedef struct ALIGN(32)
{
  int16 delta_rx_gain_offset[RFFW_LTE_MAX_NUM_RX_GAIN_RANGES];
  uint8 rx_gain_sbi_address;
  uint8 rx_gain_sbi_value[RFFW_LTE_MAX_NUM_RX_GAIN_RANGES];
} rffw_lte_rxagc_ncell_s;

/*! To enable Intra band CA on WTR1625 + WFR 1620, using PLL + LNA split */
typedef struct ALIGN(32)
{
  /*! Rx Gain offsets for the neighbor cell */
  int16 delta_rx_gain_offset[RFFW_LTE_MAX_NUM_RX_GAIN_RANGES];

  /*! Number of LNA GW writes to program for ncell path*/
  rffw_lte_rxagc_lna_gw_ctrl_s rx_gain_ctrl[RFFW_MAX_LNA_GW_SSBI_PER_PATH];

  /*! Number of transactions on this path */
  uint8 rxagc_lna_write_trans_num;

} rffw_lte_rxagc_ncell_ca_enb_s; 

/*!
 * @brief This structure specifies the control parameters that are required for the TXP2
 * signal. TXP2 is a feature which sets a GRFC pin HIGH when the TX power crosses a certain
 * threshold. This structure will have information about the hysterisis values. There is not
 * a single threshold value. There will be a power value (dBx10) for TXP2 GRFC pin to go
 * high and a different power value (dBx10) for TXP2 GRFC pin to go low, thus providing
 * hysterisis - avoid fast switching between high and low if power is fluctuating around the
 * threshold. hysterisis might be very narrrow like 1 dB difference. */
 
typedef struct ALIGN(32)
{
 /*!
 * @brief The following variable is the Rise threshold value in dBx10 to provide hysteresis
 * and avoid fast switching of this signal
 */
  int16 rise_tx_pwr_dBx10;

  /*!
 * @brief The following variable is the Fall threshold value in dBx10 to provide hysteresis
 * and avoid fast switching of this signal
 */
  int16 fall_tx_pwr_dBx10;

  /*!
 * @brief The following variable defines the delay between actual Tx ON and the instant when TXP2
 * GRFC Pin goes HIGH. TXP2 needs to be turned on around 0 to 200 us in advance to give the other
 * devices enough time to respond.
 * @note The unit should be in MSTMR ts (1 ms = 30720 Ts). For negative values, TXP2 will be changed
 * before the actual Tx ON; for positive values, TXP2 GRFC will be changed after actual Tx ON.
 */
  int16 advance_time;

  /*!
 * @brief The following variable defines the GRFC engine number into which we want to write 
 * the TXP2 signal
 */
  int8 grfc_engine_number;

 /*!
 * @brief This following flag indicates whether we want the TXP2 feature to be enabled or not
 */
  int8 enable;
}  rffw_lte_txp2_ctrl_info_s;

 /*!
 * @brief This structure contains the CCS header, task and payload for each event. The bytes for this events will be 
 *  used by FW to executethe sequence at the time of need.
 *  FW can get the address for each event using event_ptr for that event.
 */
typedef struct ALIGN(32)
{
    uint32 hdet_wr_start_bytes[RFFW_HDET_WR_MAX_START_SIZE];
    uint32 hdet_wr_lpm_high_bw_bytes[RFFW_HDET_WR_MAX_LPM_HIGH_BW_SIZE];
    uint32 hdet_wr_lpm_mid_bw_bytes[RFFW_HDET_WR_MAX_LPM_MID_BW_SIZE];
    uint32 hdet_wr_lpm_low_bw_bytes[RFFW_HDET_WR_MAX_LPM_LOW_BW_SIZE];
    uint32 hdet_wr_hpm_high_bw_bytes[RFFW_HDET_WR_MAX_HPM_HIGH_BW_SIZE];
    uint32 hdet_wr_hpm_mid_bw_bytes[RFFW_HDET_WR_MAX_HPM_MID_BW_SIZE];
    uint32 hdet_wr_hpm_low_bw_bytes[RFFW_HDET_WR_MAX_HPM_LOW_BW_SIZE];
    uint32 hdet_rd_bytes[RFFW_HDET_RD_SIZE];
    uint32 therm_wr_init_bytes[RFFW_THERM_WR_MAX_INIT_SIZE];
    uint32 therm_wr_conv_disable_bytes[RFFW_THERM_WR_MAX_CONV_DISABLE_SIZE];
    uint32 therm_rd_bytes[RFFW_THERM_RD_SIZE];
    uint32 rx_on_wr_on_bytes[RFFW_RX_ON_WR_SIZE];
    uint32 rx_on_wr_off_bytes[RFFW_RX_ON_WR_SIZE];
    uint32 tx_on_wr_on_bytes[RFFW_TX_ON_WR_SIZE];
    uint32 tx_on_wr_off_bytes[RFFW_TX_ON_WR_SIZE];
    uint32 tx_en_wr_on_bytes[RFFW_TX_EN_WR_SIZE];
    uint32 tx_en_wr_off_bytes[RFFW_TX_EN_WR_SIZE];
    uint32 smps_en_wr_on_bytes[RFFW_SMPS_EN_WR_SIZE];
    uint32 smps_en_wr_off_bytes[RFFW_SMPS_EN_WR_SIZE];
    uint32 tdd_asm_rx_wr_bytes[RFFW_ASM_RX_WR_SIZE];
    uint32 tdd_asm_tx_wr_bytes[RFFW_ASM_TX_WR_SIZE];
    uint32 rx_en_wr_on_bytes[RFFW_RX_EN_WR_SIZE];
    uint32 rx_en_wr_off_bytes[RFFW_RX_EN_WR_SIZE];
    uint32 rx_en_rx_on_wr_off_bytes[RFFW_RX_EN_RX_ON_WR_SIZE];
    uint32 tx_en_tx_on_wr_on_bytes[RFFW_TX_EN_TX_ON_WR_SIZE];
    uint32 tx_en_tx_on_wr_off_bytes[RFFW_TX_EN_TX_ON_WR_SIZE];
    uint32 papm_on_bytes[RFFW_PAPM_ON_SIZE];
    uint32 papm_off_bytes[RFFW_PAPM_OFF_SIZE];
    uint32 scell_rx_on_wr_on_bytes[RFFW_RX_ON_WR_SIZE];
    uint32 scell_rx_on_wr_off_bytes[RFFW_RX_ON_WR_SIZE];
    uint32 tdet_temp_comp_bytes[RFFW_LTE_TDET_TEMP_COMP_SIZE];
    uint32 event_ptr[RFFW_LTE_CSS_EVENT_NUM];
}rffw_lte_css_events_types_s;

typedef enum 
  {
    RFFW_LTE_RF_INVALID,
    RFFW_LTE_RF_GRFC,
    RFFW_LTE_RF_SBI,
    RFFW_LTE_RF_RFFE,
    RFFW_LTE_RF_TYPE_MAX
  } rffw_lte_rf_trans_type;

/*! @brief SBI transaction data structure
*/
typedef struct
{
  uint8 sbi_bus;              /*!< The SSBI bus number */
  uint8 rd_wr;                /*!< Read or write transaction. */
  uint16 num_trans;            /*!< Number of transaction data */
} rffw_lte_sbi_hdr;

typedef struct
{
  uint8 addr;                 /*!< The 8 bit SSBI address */
  uint8 data;                 /*!< The 8 bit SSBI data. Undefined if transaction is read */
  boolean timed_imm;          /*!< Timed or immediate. If immediate, start_delta will be ignored */
  int16 start_delta;          /*!< Start time in TBD units */
} rffw_lte_sbi_data_type;

typedef struct
{
  uint16 num_trans;            /*!< Number of transaction data */
} rffw_lte_grfc_hdr;

/*! @brief GRFC transaction data structure

*/
typedef struct
{
  /* uint8 sounds better but current shared mem define grfc no using uint16*/
  uint16 grfc_id;              /*!< The GRFC ID */
  boolean start_level;          /*!< The logical level of GRFC. 0 = Low, 1 = High */
  int16 start_delta;          /*!< Start time in TBD units */  
} rffw_lte_grfc_data_type;


/*! @brief RFFE transaction data structure
    Does not support extended write/read for now
*/
typedef struct
{
  uint8 rd_wr;                /*!< Read or write transaction. */
  uint8 channel;              /*!< The RFFE channel (bus) number */
  uint8 slave_id;             /*!< The RFFE slave ID */
  boolean rd_delay;           /*!< The read delay (N/A for writes) */
  boolean half_rate;          /*!< Use half rate reads (N/A for writes) */
  uint16 num_trans;            /*!< Number of transactions to write or read */
} rffw_lte_rffe_hdr;

typedef struct
{
  uint8 num_bytes;            /*!< Number of bytes to write or read */
  uint16 addr;                /*!< The RFFE register address. 8 bits for now */
  uint8 data;   /*!< Data bytes to write or space for holding read data */
  boolean timed_imm;          /*!< Timed or immediate. If immediate, start_delta will be ignored */
  int16 start_delta;          /*!< Start time in TBD units */
  boolean extended_cmd;       /*!< Whether this transaction uses extended command format or not (must be TRUE if addr is 16 bits or num_bytes > 1 */
} rffw_lte_rffe_data_type;

typedef struct
{
  rffw_lte_rf_trans_type script_type;
  uint8 script_buffer_index;
  union
  {
    rffw_lte_sbi_hdr sbi_hdr;
    rffw_lte_grfc_hdr grfc_hdr;
    rffw_lte_rffe_hdr rffe_hdr;
  } script_hdr;
} rffw_lte_rf_script_type;

/*! @brief PAPM look up table 
  To do: add timing information */
typedef struct ALIGN(32)
{
  rffw_lte_rffe_hdr hdr;
  uint16 papm_addr_et[RFFW_LTE_PAPM_ADDR_MAX_NUM];
  uint16 papm_addr_smps[RFFW_LTE_PAPM_ADDR_MAX_NUM]; // Share between EPT and APT
}rffw_lte_papm_txagc_addr_s;

typedef struct ALIGN(32)
{
  rffw_lte_rffe_hdr hdr;  
  boolean pa_icq_in_use;
  uint16 pa_icq_addr[RFFW_LTE_PA_ICQ_ADDR_MAX_NUM]; 
}rffw_lte_pa_icq_txagc_addr_s;

/*ENUMs for scripts*/
typedef enum 
  {
    RFFW_LTE_RF_SCRIPT_PA_0,            /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PA_1,            /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PA_2,            /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PA_3,            /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PA_ON,           /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PA_OFF,          /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PA_BOOST,        /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PA_TRIGGER,      /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_TX_ON_OFF,       /* TDD */
    RFFW_LTE_RF_SCRIPT_TX_EN_OFF,       /* TDD */
    RFFW_LTE_RF_SCRIPT_TDD_ASM_TX,      /* TDD RFFE Tx */
    RFFW_LTE_RF_SCRIPT_TDD_ASM_RX,      /* TDD RFFE Rx */
    RFFW_LTE_RF_SCRIPT_TDD_ASM_TX_TRIGGER, /* TDD RFFE Tx */
    RFFW_LTE_RF_SCRIPT_TDD_ASM_RX_TRIGGER, /* TDD RFFE Tx */
    RFFW_LTE_RF_SCRIPT_TDD_ASM_TX_GRFC, /* TDD GRFC */
    RFFW_LTE_RF_SCRIPT_TDD_ASM_RX_GRFC, /* TDD GRFC */
    RFFW_LTE_RF_SCRIPT_TXPLL_AFC,       /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PAPM_OFF,        /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_PAPM_TRIGGER,    /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX0,       /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX1,       /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_EN_ANT_S,        /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_DIS_ANT_S,       /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX0_GRFC_BUF0, /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX1_GRFC_BUF0, /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX0_RFFE_BUF0, /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX1_RFFE_BUF0, /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX0_GRFC_BUF1, /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX1_GRFC_BUF1, /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX0_RFFE_BUF1, /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_ANT_S_TX1_RFFE_BUF1, /* FDD and TDD */
    RFFW_LTE_RF_SCRIPT_EVENT_MAX
  } rffw_lte_rf_script_id_type;

/* Structure for TDD ASM control with Dynamic Switching */
typedef struct
{
  rffw_lte_rf_script_type  tdd_asm_tx_script;
  rffw_lte_rf_script_type  tdd_asm_tx_trigger_script;
  /* To program multiple ASM settings for Tx */
  rffw_lte_rf_script_type  tdd_aux_asm_tx_script[RFFW_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rffw_lte_rf_script_type  tdd_aux_asm_tx_trigger_script[RFFW_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rffw_lte_rf_script_type  tdd_asm_rx_script;
  rffw_lte_rf_script_type  tdd_asm_rx_trigger_script;
  /* To program multiple ASM settings for Rx */
  rffw_lte_rf_script_type  tdd_aux_asm_rx_script[RFFW_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rffw_lte_rf_script_type  tdd_aux_asm_rx_trigger_script[RFFW_LTE_ADDITIONAL_NUM_FE_DEV_SUPPORTED];
  rffw_lte_rf_script_type  tdd_asm_tx_grfc_script;
  rffw_lte_rf_script_type  tdd_asm_rx_grfc_script;

}rffw_lte_rf_tdd_asm_scripts_type;

/* Structure for PA script control with Dynamic Switching */
typedef struct
{
  /* Linearizer tables 0 - 3*/
  rffw_lte_rf_script_type  state_0;
  rffw_lte_rf_script_type  state_1;
  rffw_lte_rf_script_type  state_2;
  rffw_lte_rf_script_type  state_3;

  /* PA ON script*/
  rffw_lte_rf_script_type  on;

  /* PA OFF script*/
  rffw_lte_rf_script_type  off;

  /* PA BOOST script*/
  rffw_lte_rf_script_type  boost;

  /* PA TRIGGER script*/
  rffw_lte_rf_script_type  trigger;

}rffw_lte_rf_pa_scripts_type;

/* Structure for scripts*/
typedef struct
{
    rffw_lte_rf_script_type  pa_state_0_script;
    rffw_lte_rf_script_type  pa_state_1_script;
    rffw_lte_rf_script_type  pa_state_2_script;
    rffw_lte_rf_script_type  pa_state_3_script;
    rffw_lte_rf_script_type  pa_on_script;
    rffw_lte_rf_script_type  pa_off_script;
    rffw_lte_rf_script_type  pa_boost_script;
    rffw_lte_rf_script_type  pa_trigger_script;

    /* To Support Dynamic Switching we will need to 
       allocate more PA script space */
    rffw_lte_rf_pa_scripts_type pa[RFFW_LTE_MAX_PA_SCRIPTS_SUPPORTED_PER_PATH];

    rffw_lte_rf_script_type  tx_on_off_script;
    rffw_lte_rf_script_type  tx_en_off_script;
    rffw_lte_rf_script_type  txpll_afc_script;
    rffw_lte_rf_script_type  papm_off_script;
    rffw_lte_rf_script_type  papm_trigger_script;

    /* To support Ant swap for TxAGC events in special scenarios (eg:TM8 SRS) */
    rffw_lte_rf_script_type  ant_s_tx0_script;
    rffw_lte_rf_script_type  ant_s_tx1_script;

    /* To support general Ant Swap for features like Ant Switch Diversity */
    rffw_lte_rf_script_type  en_ant_s_script;
    rffw_lte_rf_script_type  dis_ant_s_script;

    /* Antenna switch scripts for TM8 SRS: GRFC - Ant Switch,
       RFFE - QTuner settings */
    rffw_lte_rf_script_type  ant_s_tx0_grfc_buf0_script;
    rffw_lte_rf_script_type  ant_s_tx1_grfc_buf0_script;
    rffw_lte_rf_script_type  ant_s_tx0_rffe_buf0_script;
    rffw_lte_rf_script_type  ant_s_tx1_rffe_buf0_script;
    /* Double buffer to avoid FW reading script while RF
       updates it. When buf0 is used by FW, buf1 is updated
       by RF and vice-versa */
    rffw_lte_rf_script_type  ant_s_tx0_grfc_buf1_script;
    rffw_lte_rf_script_type  ant_s_tx1_grfc_buf1_script;
    rffw_lte_rf_script_type  ant_s_tx0_rffe_buf1_script;
    rffw_lte_rf_script_type  ant_s_tx1_rffe_buf1_script;

  /* We can support RFFW_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH many 
       unique ASM scripts per LTE Band specific path */
    rffw_lte_rf_tdd_asm_scripts_type tdd_asm_script[RFFW_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH];

    /*Variable which will specify which TDD Tx ASM port settings to be used
    We need to make sure the value of this variable is always
    less than RFFW_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH */
    uint8 tdd_tx_asm_script_index;

    /*Variable which will specify which TDD Rx ASM port settings to be used
    We need to make sure the value of this variable is always
    less than RFFW_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH */
    uint8 tdd_rx_asm_script_index;

    /*Variable which will specify which TDD/FDD PA ASM port settings to be used
    We need to make sure the value of this variable is always
    less than RFFW_LTE_MAX_PA_SCRIPTS_SUPPORTED_PER_PATH */
    uint8 tdd_tx_pa_script_index;

} rffw_lte_rf_scripts;

/* Tx Linearizer now needs to hold - 
   RGI, Tx Pwr, Bias, IQ Gain, PA current, QPoet bias, DPD 
   This structure holds the additional XPT information to the 
   Standard linearizer interface 
   Currently, only support one ET and one EPT PA state. */ 

typedef struct ALIGN(32)
{
  rffw_lte_xpt_mode_type xpt_pa_map[RFFW_LTE_MAX_NUM_PA_RANGES];
  rffw_et_mode_enable_s papm_mode;
  rffw_lte_pa_icq_txagc_addr_s pa_icq_addr;
  uint8 pa_icq[RFFW_LTE_MAX_NUM_PA_RANGES]
                                [RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE]
              [RFFW_LTE_PA_ICQ_ADDR_MAX_NUM];              /* PA current */
  rffw_lte_papm_txagc_addr_s papm_addr;
  uint8 papm[RFFW_LTE_MAX_NUM_PA_RANGES]
            [RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE]
            [RFFW_LTE_PAPM_ADDR_MAX_NUM];             /* QPoet Bias */
  rffw_lte_xpt_txagc_s et_ept[RFFW_LTE_MAX_NUM_PA_RANGES];                                              /* EPT DPD tables */                                               /* ET DPD tables */
  int32 et_delay_slot1[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME];
  int32 et_delay_slot2[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME];
  int32 et_delay_srs[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME];
  boolean dpd_in_use; /* This will be removed in next check in */
  boolean xpt_in_use;
  uint32 env_scale_nominal[RFFW_LTE_MAX_NUM_PA_RANGES];
  uint32 iq_gain_nominal[RFFW_LTE_MAX_NUM_PA_RANGES];

  /* First sweep linearizer*/
  boolean first_sweep_tx_lin_in_use; /* This will be removed in next check in */
  rffw_lte_tx_gain_s first_sweep_tx_lin_tbl0[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  rffw_lte_tx_gain_s first_sweep_tx_lin_tbl1[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  rffw_lte_tx_gain_s first_sweep_tx_lin_tbl2[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];
  rffw_lte_tx_gain_s first_sweep_tx_lin_tbl3[RFFW_LTE_MAX_TX_GAIN_SETTING_TBL_SIZE];

  /* P_in and P_out TXAGC compensation for current SF */
  boolean lin_vs_temp_vs_freq_comp_in_use;
  int32 p_in_comp_slot1[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES][RFFW_LTE_MAX_NUM_FREQ_COMP];
  int32 p_in_comp_slot2[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES][RFFW_LTE_MAX_NUM_FREQ_COMP];
  int32 p_in_comp_srs[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES][RFFW_LTE_MAX_NUM_FREQ_COMP];
  int32 p_out_comp_slot1[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES][RFFW_LTE_MAX_NUM_FREQ_COMP];
  int32 p_out_comp_slot2[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES][RFFW_LTE_MAX_NUM_FREQ_COMP];
  int32 p_out_comp_srs[RFFW_INTF_LTE_MAX_NUM_SUBFRAMES_PER_FRAME][RFFW_LTE_MAX_NUM_PA_RANGES][RFFW_LTE_MAX_NUM_FREQ_COMP];

  int8 numb_rb_threshold;
  int8 pwr_adjust_based_on_rb_db10;
  rffw_lte_dpd_cfg_type dpd_cfg;
  
  uint8 rffw_lte_xpt_buffer_2[3];
  uint32 rffw_lte_xpt_buffer[98];
} rffw_lte_xpt_s;

/*!
 * @brief  
 */
typedef struct
{
    uint16 addr; /*only 8bits for now*/
    uint8 value;
} rffw_lte_hw_rd_data_type;

/*!
 * @brief  
 */
typedef struct
{
    rffw_lte_hw_rd_data_type hdet_rd[RFFW_HDET_RD_RESULT_NUM_TRANS];
    uint8 num_trans;
    rffw_lte_rf_trans_type trans_type;
}rffw_lte_hdet_rd_result_data_type;

/*!
 * @brief  
 */
typedef struct
{
    rffw_lte_hw_rd_data_type therm_rd[RFFW_THERM_RD_RESULT_NUM_TRANS];
    uint8 num_trans;
    rffw_lte_rf_trans_type trans_type;
}rffw_lte_therm_rd_result_data_type;

/*!
 * @brief  
 */
typedef struct ALIGN(32)
{
    rffw_lte_hdet_rd_result_data_type hdet_rd_event_result;
    rffw_lte_therm_rd_result_data_type therm_rd_event_result;
}rffw_ccs_rd_events_result_data_type;

/* Structure for Event timings */
typedef struct
{
   /* WTR RGI Write Timing from Start of UL SF for both FDD and TDD */

   int16 tx_on_ul_sf_agc_timing;                    /* DTx/Sleep--UL_SF*/
   int16 tx_on_prach_or_srs_only_agc_timing;        /* Sleep--PRACH or Sleep--SRS */

   int16 tx_cont_ul_sf_agc_timing;                  /* UL_SF--UL_SF */
   int16 tx_cont_pre_ue_srs_agc_timing;             /* UL_SF--SRS */
   int16 tx_cont_post_ue_srs_agc_timing;            /* SRS---UL_SF */

   /* PA Control Timings from Start of UL SF for PA ON and end of UL SF for PA OFF - both FDD and TDD*/

   int16 tx_on_ul_sf_pa_ctl_on_timing;              /* DTx/Sleep--UL_SF*/
   int16 tx_on_prach_or_srs_only_pa_ctl_on_timing;  /* Sleep--PRACH or Sleep--SRS */

   int16 tx_cont_ul_sf_pa_ctl_on_timing;            /* UL_SF--UL_SF */
   int16 tx_cont_pre_ue_srs_pa_ctl_on_timing;       /* UL_SF--SRS */
   int16 tx_cont_post_ue_srs_pa_ctl_on_timing;      /* SRS---UL_SF */

   int16 tx_off_null_sf_pa_ctl_off_timing;          /* UL_SF--Sleep/DTx */
   int16 tx_off_cell_srs_pa_ctl_off_timing;         /* UL_SF--Blank_SRS_Sym */

   /* PA Current Timings from Start of UL SF for PA ON and end of UL SF for PA OFF - both FDD and TDD*/

   int16 tx_on_ul_sf_pa_current_timing;              /* DTx/Sleep--UL_SF*/
   int16 tx_on_prach_or_srs_only_pa_current_timing;  /* Sleep--PRACH or Sleep--SRS */

   int16 tx_cont_ul_sf_pa_current_timing;            /* UL_SF--UL_SF */
   int16 tx_cont_pre_ue_srs_pa_current_timing;       /* UL_SF--SRS */
   int16 tx_cont_post_ue_srs_pa_current_timing;      /* SRS---UL_SF */

   /* ASM, Tx ON OFF and Tx EN OFF Timings for TDD */

   int16 tdd_tx_on_ul_sf_asm_tx_timing;             /* DTx/Sleep--UL_SF or DL_SF--UL_SF */
   int16 tdd_tx_on_prach_or_srs_only_asm_tx_timing; /* Sleep/DL_SF--PRACH or Sleep/DL_SF--SRS */

   int16 tdd_tx_off_null_sf_asm_rx;                 /* UL_SF/PRACH--DL_SF OR UL_SF/PRACH--<Empty>*/
   int16 tdd_tx_off_cell_srs_asm_rx;                /* UL_SF--Blank_SRS_Sym */

   int16 tdd_tx_tx_on_low_timing;                   /* UL_SF--DL_SF OR UL_SF--<Empty> */
   int16 tdd_tx_tx_en_low_timing;                   /* UL_SF--DL_SF OR UL_SF--<Empty> */

   /* TxPLL AFC Timings for FDD and TDD */

   int16 tx_on_ul_sf_txpll_afc_timing;               /* DTx/Sleep--UL_SF */

   int16 tx_cont_ul_sf_txpll_afc_timing;             /* UL_SF--UL_SF */
   int16 tx_cont_post_ue_srs_txpll_afc_timing;       /* SRS---UL_SF */

   /* QPOET config timing for FDD and TDD */

   int16 tx_on_ul_sf_bias_config;
   int16 tx_on_prach_or_srs_only_bias_config;
   
   int16 tx_cont_ul_sf_bias_config;
   int16 tx_cont_pre_ue_srs_bias_config;
   int16 tx_cont_post_ue_srs_bias_config;

   int16 common_tx_bias_off;

   /* Tx ON/Tx EN timings for FDD and TDD */

   int16 fdd_tx_tx_on_high_timing;
   int16 fdd_tx_tx_en_high_timing;

   int16 fdd_tx_tx_on_low_timing;
   int16 fdd_tx_tx_en_low_timing;

   int16 tdd_tx_tx_on_high_timing;

   /* Tx Antenna Switch for TxAGC events for FDD and TDD */

   int16 ant_s_tx0_timing;
   int16 ant_s_tx1_timing;

   /* General Antenna Swap for features like AsDiv for FDD and TDD */

   int16 en_ant_s_timing;
   int16 dis_ant_s_timing;
}rffw_lte_rf_timing;


/*!
 * @brief This structure unifies the item specific structures.
 * This is the final structure that resides in the Shared Memory.
 * The cache aligner is used to devide information where SW writes to FW and
 * FW writes for SW. Otherwise there might be serious complications
 */
typedef struct ALIGN(32)
{  
   rffw_lte_counter_s counter; 
   rffw_lte_grfc_s grfc;
   rffw_lte_rxagc_fw2sw_s rxagc_fw2sw[RFFW_LTE_NUM_RX_PATHS];
   rffw_lte_rxagc_s rxagc[RFFW_LTE_NUM_RX_PATHS];
   rffw_lte_txagc_fw2sw_s txagc_fw2sw;
   rffw_lte_txagc_s txagc;
   rffw_lte_hdet_s hdet;     
   rffw_lte_txagc_pa_bias_s pa_bias;
   /* To be Deprecated going ahead after WTR 1625 BU */
   rffw_lte_rxagc_ncell_s rxagc_ncell[RFFW_LTE_NUM_RX_PATHS];

   rffw_lte_rxagc_ncell_ca_enb_s rxagc_ncell_ca_enb[RFFW_LTE_NUM_RX_PATHS];
   boolean rx_config_done;
   /*! reserved space to make sure the size of the struct is a multiple of 32-bit word */
   int8 reserved_bytes[3];
   rffw_lte_txp2_ctrl_info_s txp2_ctrl_info;
   rffw_lte_css_events_types_s ccs_events;
   rffw_lte_rf_scripts rf_scripts;
   rffw_lte_sbi_data_type sbi_buff[RFFW_LTE_SBI_TRANS_MAX];
   rffw_lte_grfc_data_type grfc_buff[RFFW_LTE_GRFC_TRANS_MAX];
   rffw_lte_rffe_data_type rffe_buff[RFFW_LTE_RFFE_TRANS_MAX];
   rffw_lte_xpt_s xpt;
   rffw_ccs_rd_events_result_data_type ccs_rd_events_result;
   rffw_lte_rf_timing rf_timing;
    
   /* array to specify which TDD ASM port settings to be used
    We need to make sure the value of this variable is always
    less than RFFW_LTE_MAX_ASM_SCRIPTS_SUPPORTED_PER_PATH */
   uint8 rffw_lte_tdd_asm_script_index;
   /* Flag to specify if Intraband CA is active */
   boolean intraband_ca_active;

} rffw_lte_s;


#endif //RFFW_LTE_INTF_H
