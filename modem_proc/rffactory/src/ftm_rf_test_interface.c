/*!
  @file
  ftm_rf_test_interface.c

  @brief
  Common framework to perform radio test in FTM mode
*/

/*======================================================================================================================

  Copyright (c) 2015 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document are confidential 
  and proprietary information of Qualcomm Technologies Incorporated and all rights therein are 
  expressly reserved. By accepting this material the recipient agrees that this material and the 
  information contained therein are held in confidence and in trust and will not be used, copied, 
  reproduced in whole or in part, nor its contents revealed in any manner to others without the 
  express written permission of Qualcomm Technologies Incorporated.

======================================================================================================================*/

/*======================================================================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rffactory/src/ftm_rf_test_interface.c#1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------------------------------
06/24/16   jfc     Rename rx_mode continuous_mode and add burst_pattern property
06/21/16   jfc     Add wait_trigger subcommand to FTM RF Test
06/02/16   jps     Use DEBUG_DATA property for blob data
05/25/16   jps     TX Measure - Remove Sig Path and Antenna Path properties
05/16/16   chm     Added new property type for rx measure cmd
05/06/16   jfc     Added support for freq adj in radio config
04/28/16   chm     Return the IQ samples used to compute the IQ reports
04/04/16   zhw     Addeds support for New FTM irat Framework
04/01/15   zhw     Initial Release

======================================================================================================================*/

#include "comdef.h"
#include "rflm_defs.h"
#include "ftm_rf_test_interface.h"
/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Radio Config Property Name Display Info
*/
const char *radio_config_property_names[] = 
{
  "UNASSIGNED",        /* FTM_RF_TEST_RADIO_CFG_PROP_UNASSIGNED */
  "RX_CARRIER",        /* FTM_RF_TEST_RADIO_CFG_PROP_RX_CARRIER */
  "TX_CARRIER",        /* FTM_RF_TEST_RADIO_CFG_PROP_TX_CARRIER */
  "DEVICE_PRI",        /* FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE_PRI */
  "DEVICE_DIV",        /* FTM_RF_TEST_RADIO_CFG_PROP_RFM_DEVICE_DIV */
  "BAND",              /* FTM_RF_TEST_RADIO_CFG_PROP_BAND */
  "CHANNEL",           /* FTM_RF_TEST_RADIO_CFG_PROP_CHANNEL */
  "BANDWIDTH",         /* FTM_RF_TEST_RADIO_CFG_PROP_BANDWIDTH */
  "CONT_MODE",         /* FTM_RF_TEST_RADIO_CFG_PROP_CONTTINUOUS_MODE */
  "SIG_PATH",          /* FTM_RF_TEST_RADIO_CFG_PROP_SIG_PATH */
  "ANT_PATH",          /* FTM_RF_TEST_RADIO_CFG_PROP_ANT_PATH */
  "USER_ADJ",          /* FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_USER_ADJUST */
  "CENTER_FREQ",       /* FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_CENTER_FREQ */
  "ENABLE_XO",         /* FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_ENABLE_XO */
  "TOTAL_ADJ",         /* FTM_RF_TEST_RADIO_CFG_PROP_FREQ_ADJ_TOTAL_ADJUST */
  "BURST_PATTERN",     /* FTM_RF_TEST_RADIO_CFG_BURST_PATTERN */
};

COMPILE_ASSERT(sizeof(radio_config_property_names)/sizeof(radio_config_property_names[0]) == FTM_RF_TEST_RADIO_CFG_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Rx Measure Property Name Display Info
*/
const char *rx_measure_property_names[] = 
{
  "UNASSIGNED",           /* FTM_RF_MEASURE_PROP_UNASSIGNED */
  "RX_CARRIER",           /* FTM_RX_MEASURE_PROP_RX_CARRIER */
  "RFM_DEVICE",           /* FTM_RX_MEASURE_PROP_RFM_DEVICE */
  "EXPECTED_AGC",         /* FTM_RX_MEASURE_PROP_EXPECTED_AGC */
  "RX_AGC",               /* FTM_RX_MEASURE_PROP_AGC */
  "LNA_GS",               /* FTM_RX_MEASURE_PROP_LNA_GAIN_STATE */
  "SIG_PATH",             /* FTM_RX_MEASURE_PROP_SIG_PATH */
  "RX_MODE",              /* FTM_RX_MEASURE_PROP_RX_MODE */
  "RX_SLOT",              /* FTM_RX_MEASURE_PROP_RX_SLOT */
  "NUM_OF_BURST",         /* FTM_RX_MEASURE_PROP_NUM_OF_BURST */
  "SENSITIVITY",          /* FTM_RX_MEASURE_PROP_SENSITIVITY */
  "CTON",                 /* FTM_RX_MEASURE_PROP_CTON */
  "PEAK FREQ",            /* FTM_RX_MEASURE_PROP_PEAK_FREQ */
  "FETCH_IQ",             /* FTM_RX_MEASURE_PROP_FETCH_RAW_IQ_SAMPLES */
  "NUM_OF_SAMPLES",       /* FTM_RX_MEASURE_PROP_NUM_OF_SAMPLES */
  "IQ_DATA_FORMAT",       /* FTM_RX_MEASURE_PROP_IQ_DATA_FORMAT */
  "SAMP_FREQ",            /* FTM_RX_MEASURE_PROP_SAMP_FREQ */
};

COMPILE_ASSERT(sizeof(rx_measure_property_names)/sizeof(rx_measure_property_names[0]) == FTM_RX_MEASURE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  Tx Control Property Name Display Info
*/
const char *tx_control_property_names[] = 
{
  "UNASSIGNED",           /* FTM_TX_CONTROL_PROP_UNASSIGNED */
  "TX_CARRIER",           /* FTM_TX_CONTROL_PROP_TX_CARRIER */
  "RFM_DEVICE",           /* FTM_TX_CONTROL_PROP_RFM_DEVICE */
  "TX_ACTION",            /* FTM_TX_CONTROL_PROP_TX_ACTION */
  "TX_POWER",             /* FTM_TX_CONTROL_PROP_TX_POWER */    
  "RB_CONFIG",            /* FTM_TX_CONTROL_PROP_RB_CONFIG */    
  "SIG_PATH",             /* FTM_TX_CONTROL_PROP_SIG_PATH */      
  "NUM_OF_RB",            /* FTM_TX_CONTROL_PROP_NUM_OF_RB */   
  "TX_WAVEFORM",          /* FTM_TX_CONTROL_PROP_TX_WAVEFORM */ 
  "NETWORK_SIGNAL",       /* FTM_TX_CONTROL_PROP_NETWORK_SIGNAL */
  "TX_SLOT",              /* FTM_TX_CONTROL_PROP_TX_SLOT */
  "MODULATION_TYPE",      /* FTM_TX_CONTROL_PROP_MODULATION_TYPE */
};

COMPILE_ASSERT(sizeof(tx_control_property_names)/sizeof(tx_control_property_names[0]) == FTM_TX_CONTROL_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  MSIM Config Property Name Display Info
*/
const char *msim_config_property_names[] = 
{
  "UNASSIGNED",           /* FTM_MSIM_CONFIG_PROP_UNASSIGNED */
  "SUB",                  /* FTM_MSIM_CONFIG_PROP_SUBSCRIBER */
  "TECH",                 /* FTM_MSIM_CONFIG_PROP_TECH */
  "SCENARIO",             /* FTM_MSIM_CONFIG_PROP_SCENARIO */
};

COMPILE_ASSERT(sizeof(msim_config_property_names)/sizeof(msim_config_property_names[0]) == FTM_MSIM_CONFIG_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  IQ Capture Property Name Display Info
*/
const char *iq_capture_property_names[] = 
{
  "UNASSIGNED",           /* FTM_IQ_CAPTURE_PROP_UNASSIGNED */          
  "SUBSCRIBER",           /* FTM_IQ_CAPTURE_PROP_SUBSCRIBER */          
  "TECHNOLOGY",           /* FTM_IQ_CAPTURE_PROP_TECHNOLOGY */          
  "TX_CARRIER",           /* FTM_IQ_CAPTURE_PROP_TX_CARRIER */          
  "RX_CARRIER",           /* FTM_IQ_CAPTURE_PROP_RX_CARRIER */          
  "RFM_DEVICE",           /* FTM_IQ_CAPTURE_PROP_RFM_DEVICE */          
  "SIG_PATH",             /* FTM_IQ_CAPTURE_PROP_SIG_PATH */            
  "ACTION_GETCFG",        /* FTM_IQ_CAPTURE_PROP_ACTION_GET_CFG */   
  "ACTION_ACQUIRE",       /* FTM_IQ_CAPTURE_PROP_ACTION_ACQUIRE */   
  "ACTION_FETCH",         /* FTM_IQ_CAPTURE_PROP_ACTION_FETCH */
  "ACTION_EST_SENS",      /* FTM_IQ_CAPTURE_PROP_ACTION_EST_SENS */
  "ACTION_CTON",          /* FTM_IQ_CAPTURE_PROP_ACTION_CTON */
  "ACTION_PEAK_FREQ",     /* FTM_IQ_CAPTURE_PROP_ACTION_PEAK_FREQ */
  "IQ_SOURCE",            /* FTM_IQ_CAPTURE_PROP_IQ_CAPTURE_SOURCE */   
  "SAMP_SIZE",            /* FTM_IQ_CAPTURE_PROP_SAMPLE_SIZE */         
  "DATA_FORMAT",          /* FTM_IQ_CAPTURE_PROP_IQ_DATA_FORMAT */  
  "SAMP_FREQ",            /* FTM_IQ_CAPTURE_PROP_SAMP_FREQ */           
  "MAX_DIAG_SIZE",        /* FTM_IQ_CAPTURE_PROP_MAX_DIAG_SIZE */       
  "SAMP_OFFSET",          /* FTM_IQ_CAPTURE_PROP_SAMPLE_OFFSET */       
  "NUM_SAMP_BYTES",       /* FTM_IQ_CAPTURE_PROP_NUM_SAMPLE_BYTES */    
  "EST_SENS",             /* FTM_IQ_CAPTURE_PROP_EST_SENSITIVITY */     
  "CTON",                 /* FTM_IQ_CAPTURE_PROP_CTON */                
  "PEAK_FREQ",            /* FTM_IQ_CAPTURE_PROP_CW_PEAK_FREQ */        
};

COMPILE_ASSERT(sizeof(iq_capture_property_names)/sizeof(iq_capture_property_names[0]) == FTM_IQ_CAPTURE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  TX Measure Property Name Display Info
*/
const char *tx_measure_property_names[] = 
{
  "UNASSIGNED",           /* FTM_IQ_CAPTURE_PROP_UNASSIGNED */          
  "NUM_AVERAGES",           /* FTM_IQ_CAPTURE_PROP_SUBSCRIBER */          
  "TX_CARRIER",           /* FTM_IQ_CAPTURE_PROP_TECHNOLOGY */          
  "TX_DEVICE",           /* FTM_IQ_CAPTURE_PROP_TX_CARRIER */          
  "ACTION_TX_POWER",           /* FTM_IQ_CAPTURE_PROP_RX_CARRIER */          
  "ACTION_EVM",           /* FTM_IQ_CAPTURE_PROP_RFM_DEVICE */          
  "ACTION_ACLR",             /* FTM_IQ_CAPTURE_PROP_SIG_PATH */            
  "ACTION_VSWR",        /* FTM_IQ_CAPTURE_PROP_ACTION_GET_CFG */   
  "ACTION_DEBUG_DATA",       /* FTM_IQ_CAPTURE_PROP_ACTION_ACQUIRE */   
  "ACTION_PHASE_ERROR",         /* FTM_IQ_CAPTURE_PROP_ACTION_FETCH */
  "ACTION_SPECTRUM_FLATNESS",      /* FTM_IQ_CAPTURE_PROP_ACTION_EST_SENS */
  "TX_POWER",          /* FTM_IQ_CAPTURE_PROP_ACTION_CTON */
  "EVM",     /* FTM_IQ_CAPTURE_PROP_ACTION_PEAK_FREQ */
  "ACLR_UTRA1_POS",            /* FTM_IQ_CAPTURE_PROP_IQ_CAPTURE_SOURCE */   
  "ACLR_UTRA1_NEG",            /* FTM_IQ_CAPTURE_PROP_SAMPLE_SIZE */         
  "ACLR_UTRA2_POS",          /* FTM_IQ_CAPTURE_PROP_IQ_DATA_FORMAT */  
  "ACLR_UTRA2_NEG",            /* FTM_IQ_CAPTURE_PROP_SAMP_FREQ */           
  "ACLR_EUTRA_POS",        /* FTM_IQ_CAPTURE_PROP_MAX_DIAG_SIZE */       
  "ACLR_EUTRA_NEG",          /* FTM_IQ_CAPTURE_PROP_SAMPLE_OFFSET */       
  "VSWR_RATIO",       /* FTM_IQ_CAPTURE_PROP_NUM_SAMPLE_BYTES */    
  "VSWR_RETURN_LOSS",             /* FTM_IQ_CAPTURE_PROP_EST_SENSITIVITY */     
  "VSWR_S11_MAG",                 /* FTM_IQ_CAPTURE_PROP_CTON */                
  "VSWR_S11_PHASE",            /* FTM_IQ_CAPTURE_PROP_CW_PEAK_FREQ */        
  "VSWR_FWD_POWER",
  "VSWR_REV_POWER",
  "SPECTRUM_FLATNESS_R1_RIPPLE",
  "SPECTRUM_FLATNESS_R2_RIPPLE",
  "SPECTRUM_FLATNESS_R12_RIPPLE",
  "SPECTRUM_FLATNESS_R21_RIPPLE",
  "SPECTRUM_FLATNESS_R1_RIPPLE_EXTREME",
  "SPECTRUM_FLATNESS_R2_RIPPLE_EXTREME",
  "SPECTRUM_FLATNESS_R12_RIPPLE_EXTREME",
  "SPECTRUM_FLATNESS_R21_RIPPLE_EXTREME",
  "PHASE_ERROR",
  "ACTION_TX_TOTAL",
  "TOTAL_TX_POWER",
  "DEBUG_DATA",
  "ACTION_ORFS_MODULATION",
  "ACTION_ORFS_SWITCHING",
  "RESERVED6",
  "RESERVED1",
  "RESERVED2",
  "TECH",
  "SUBSCRIBER",
  "ORFS_MODULATION_400KHZ_POS",
  "ORFS_MODULATION_400KHZ_NEG",
  "ORFS_MODULATION_600KHZ_POS",
  "ORFS_MODULATION_600KHZ_NEG",
  "ORFS_MODULATION_1200KHZ_POS",
  "ORFS_MODULATION_1200KHZ_NEG",
  "ORFS_MODULATION_1800KHZ_POS",
  "ORFS_MODULATION_1800KHZ_NEG",
  "ORFS_SWITCHING_100KHZ_POS",
  "ORFS_SWITCHING_100KHZ_NEG",
  "ORFS_SWITCHING_200KHZ_POS",
  "ORFS_SWITCHING_200KHZ_NEG",
  "ORFS_SWITCHING_250KHZ_POS",
  "ORFS_SWITCHING_250KHZ_NEG",
  "ORFS_SWITCHING_400KHZ_POS",
  "ORFS_SWITCHING_400KHZ_NEG",
  "ORFS_SWITCHING_600KHZ_POS",
  "ORFS_SWITCHING_600KHZ_NEG",
  "ORFS_SWITCHING_1800KHZ_POS",
  "ORFS_SWITCHING_1800KHZ_NEG",
};

COMPILE_ASSERT(sizeof(tx_measure_property_names)/sizeof(tx_measure_property_names[0]) == FTM_TX_MEASURE_PROP_NUM);

/*--------------------------------------------------------------------------------------------------------------------*/
/*!
  @brief
  FTM TRM RRA Property Name Display Info
*/
const char *ftm_trm_rra_property_names[] = 
{
  "UNASSIGNED",
  "SUB_IDX",    
  "TECH",
  "BAND",
  "CHAN",
  "BANDWIDTH",
  "RXTX",
  "CHAIN",
  "CARRIER_IDX",
  "RESOURCE",
  "REASON",
  "PATH_FUNCTIONALITY",
  "DEVICE",
  "SIGNAL_PATH",
  "ANTENNA_PATH"
};

COMPILE_ASSERT( ARR_SIZE(ftm_trm_rra_property_names) == FTM_TRM_RRA_PROP_NUM);


const char *irat_config_property_names[] = 
{
  "UNASSIGNED",           /* FTM_IRAT_CONFIG_PROP_UNASSIGNED */
  "ACTION",               /* FTM_IRAT_CONFIG_PROP_TGT_ACTION */
  "SUB",                  /* FTM_IRAT_CONFIG_PROP_TGT_SUB */
  "TECH",                 /* FTM_IRAT_CONFIG_PROP_TGT_TECH */
  "CARRIER",              /* FTM_IRAT_CONFIG_PROP_TGT_CARRIER */
  "DEVICE",               /* FTM_IRAT_CONFIG_PROP_TGT_DEVICE */
  "SIG_PATH",             /* FTM_IRAT_CONFIG_PROP_TGT_SIG_PATH */
  "ANT_PATH",             /* FTM_IRAT_CONFIG_PROP_TGT_ANT_PATH */
  "BAND",                 /* FTM_IRAT_CONFIG_PROP_TGT_BAND */
  "CHANNEL",              /* FTM_IRAT_CONFIG_PROP_TGT_CHANNEL */
  "BANDWIDTH",            /* FTM_IRAT_CONFIG_PROP_TGT_BANDWIDTH */

};                 

COMPILE_ASSERT(sizeof(irat_config_property_names)/sizeof(irat_config_property_names[0]) == FTM_IRAT_CONFIG_PROP_NUM);


const char *wait_trigger_property_names[] = 
{
  "UNASSIGNED",           /* FTM_WAIT_TRIGGER_PROP_UNASSIGNED */
  "SUB",                  /* FTM_WAIT_TRIGGER_PROP_SUBSCRIBER */
  "TECH",                 /* FTM_WAIT_TRIGGER_PROP_TECHNOLOGY */
  "TX_CARRIER",           /* FTM_WAIT_TRIGGER_PROP_TX_CARRIER */
  "RX_CARRIER",           /* FTM_WAIT_TRIGGER_PROP_RX_CARRIER */
  "DEVICE",               /* FTM_WAIT_TRIGGER_PROP_RFM_DEVICE */
  "THRESHOLD",            /* FTM_WAIT_TRIGGER_PROP_RXWAITDBM_THRESHOLD */
  "DIRECTION",            /* FTM_WAIT_TRIGGER_PROP_RXWAITDBM_DIRECTION */
  "TIMEOUT",              /* FTM_WAIT_TRIGGER_PROP_RXWAITDBM_TIMEOUT */
  "RXAGC",                /* FTM_WAIT_TRIGGER_PROP_RXWAITDBM_RXAGC */
};                 

COMPILE_ASSERT(sizeof(wait_trigger_property_names)/sizeof(wait_trigger_property_names[0]) == FTM_WAIT_TRIGGER_PROP_NUM);

