/*!
  @file 
  ftm_lte_ino.h

  @brief
  header file for all lte ino typedef and functions that are made available to
  external mododule

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfmodem_jolokia/hal/lte/inc/rflte_msm_iq_capture.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/15   bar     Added embedded support for Sensitivity/CNR calculation
12/29/14   bar     Added FTM interface for FTM_RF_IQ_CAPTURE_ANALYZE_SAMPLES() Command.
09/25/14   bar     Added support for FTM IQ capture
10/18/13   bsh     PCFL to 16SC conversion and enable IQ capture using QRCT only
09/26/13   kab     Fix for using stg_is_bad_fuse value from actual STG involved in RSB Cal
08/09/13   yzw     Added FTM LTE IQ capture support
07/24/13   gvn     RSB Cal updates on online mode, iqmc update and IRAT
04/18/13   gvn     DIME IQ Capture support
05/24/12   aca     16SC IQ capture CW fix
05/11/12   aca     16SC IQ capture
03/16/11   tnt     Initial Check-in
===========================================================================*/
#ifndef RFLTE_MSM_IQ_CAPTURE_H
#define RFLTE_MSM_IQ_CAPTURE_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ftm_common_ino.h"
#include "intf_dl.h"
#include "fw_rf_common_intf.h" /* FW interface */
#include "ftm_hwtc_iqcapture.h"

#include "intf_sys.h"
#include "intf_sys_msg.h"
#include "ftm_lte_log.h"



/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
/*----------------------------------------------------------------------------*/
/*!
  @brief 
  lte hwtc command enumeration.

*/
typedef enum
{
	LTE_RXF0_Y1Y2_SAMPLES		            = 0x1,
	LTE_RXF0_BBF_IQ_SAMPLES		            = 0x2,
	LTE_RXF0_WIDEBAND_IQ_FLOATING_SAMPLES	= 0x3,
	LTE_RXF0_NARROWBAND_IQ_FLOATING_SAMPLES = 0x4,
  LTE_RXF1_Y1Y2_SAMPLES		            = 0x5,
	LTE_RXF1_BBF_IQ_SAMPLES		            = 0x6,
	LTE_RXF1_WIDEBAND_IQ_FLOATING_SAMPLES	= 0x7,
	LTE_RXF1_NARROWBAND_IQ_FLOATING_SAMPLES = 0x8,
#ifdef FEATURE_FTM_HWTC 
  LTE_RX0_WIDEBAND_IQ_16_SC_SAMPLES = 0xA,
  LTE_RX1_WIDEBAND_IQ_16_SC_SAMPLES = 0xB,
  LTE_RX0RX1_WIDEBAND_IQ_16_SC_SAMPLES = 0xC,

#endif /* #ifdef FEATURE_FTM_HWTC */
        LTE_SCELL_RXF0_WIDEBAND_IQ_FLOATING_SAMPLES=0xD,
        LTE_SCELL_RXF1_WIDEBAND_IQ_FLOATING_SAMPLES=0xE,
        LTE_PCELL_RX0_WIDEBAND_IQ_PCFL_SAMPLES,
        LTE_PCELL_RX1_WIDEBAND_IQ_PCFL_SAMPLES,
        LTE_PCELL_RX0RX1_WIDEBAND_IQ_PCFL_SAMPLES,
        LTE_SCELL_RX0_WIDEBAND_IQ_PCFL_SAMPLES,
        LTE_SCELL_RX1_WIDEBAND_IQ_PCFL_SAMPLES,
        LTE_SCELL_RX0RX1_WIDEBAND_IQ_PCFL_SAMPLES,
#ifdef FEATURE_FTM_HWTC 

		LTE_SCELL_RX0_WIDEBAND_IQ_16_SC_SAMPLES=0x15,
		LTE_SCELL_RX1_WIDEBAND_IQ_16_SC_SAMPLES,
		LTE_SCELL_RX0RX1_WIDEBAND_IQ_16_SC_SAMPLES,

#endif /* #ifdef FEATURE_FTM_HWTC */
        
}rflte_iq_sel_type; 


typedef struct
{
  /*! @brief  
   *  Bitmask of the chains being requested.
   *  0x1 = chain 0        
   *  0x2 = chain 1
   *  0x3 = chain 0 and chain 1
   */  
  uint8 chain_bitmap;

  /*! @brief  
   *  The buffer XO manager (or another entity) needs details of the 
   *  mempool buffers and can be the narrowband searcher
   *  or the wideband demod buffer.        
   */
  lte_LL1_sys_mp_buf_type_e buffer_requested;

  boolean ship_logs_immediate;

  uint8 ship_counter;

  uint32* prx_data_pointer;
  uint32* drx_data_pointer;

  uint16 data_size;
/*
1 - 16SC format
0 - PCF format */
   lte_LL1_iq_sample_capture_qsize_enum_t iq_format;

/* Flag to track the FW response for the request .Before request , RFSW will set this flag to flase . After response from FW , the flag will be set to TRUE. */

   boolean fw_response_flag;
   uint32 iq_capture_output_delay;

   boolean memory_alloc_used;

}rflte_msm_iq_capture_state_type;


#define RFLTE_IQ_SAMPLE_WIDTH FTM_IQ_16_BIT_FORMAT

// Maximum number of  samples that can be captured in LTE
#define RFLTE_IQ_MAX_SAMPLES 4096

/*! This is the maximum number of GSM IQ samples that can be sent through the diag. */
/*! If diag requires more, it has to get it through multiple iterations */
/*! Through experimentation it is found to be 512 bytes: 128 samples * 4 bytes. Each sample:  I (2 byte) + Q(2 byte)  */
#define RFLTE_IQ_DIAG_MAX_SAMPLES (FTM_IQ_MAX_NUMBER_OF_BYTES_DIAG/4)

/*! LTE sampling frequency = 7.68Mhz for BW <= 5Mhz */
#define RFLTE_IQ_SAMPLE_FREQ_5Mhz (7.68e6)


/*! LTE sampling frequency = 15.36 MHz for BW = 10Mhz*/
#define RFLTE_IQ_SAMPLE_FREQ_10Mhz (15.36e6)


/*! LTE sampling frequency = 30.72Mhz for BW <= 20Mhz */
#define RFLTE_IQ_SAMPLE_FREQ_20Mhz (30.72e6)


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void rflte_msm_iq_capture_process_samples(lte_LL1_dl_iq_sample_capture_cnf_msg_struct* rf_lte_msg_ptr);
//void rflte_msm_iq_capture_process_samples(void);

// originalboolean rflte_msm_rsb_calibration
// original(
  // original lte_LL1_carrier_type_enum carrier, 
  // originaluint8 chain_mask, uint16 size, uint32 time
// original);
boolean rflte_msm_rsb_calibration
(
  lte_LL1_carrier_type_enum carrier, 
  uint8 chain_mask, 
  uint16 size, 
  uint32 time,
  rfm_device_enum_type device,
  rfm_device_enum_type div_device,
  uint8 stg_is_bad_fuse
);



void rflte_msm_iq_capture (void *msg_ptr);

#ifdef FEATURE_FTM_HWTC 
void rflte_msm_iq_capture_16sc (void *msg_ptr);
#endif /* #ifdef FEATURE_FTM_HWTC */

void rflte_msm_iq_capture_config(byte carrier,uint8 antenna,uint32 size);

ftm_iq_capture_error_code_type rflte_msm_iq_capture_extract_data(uint32 size,uint32 offset,int16* iq_buf_ptr);

ftm_iq_capture_error_code_type rflte_msm_iq_capture_extract_data_pointer(uint32 size, uint16 **iq_buf_ptr);

#endif /* RFLTE_MSM_IQ_CAPTURE_H */

