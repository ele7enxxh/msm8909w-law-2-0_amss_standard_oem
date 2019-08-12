/*!
  @file
  fw_autopin_intf.h

  @brief
  Public definitions for auto-Pin capture and sample processing.
*/

/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly
  reserved. By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in trust
  and will not be used, copied, reproduced in whole or in part, nor its
  contents revealed in any manner to others without the express written
  permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/07/15   nbb      Initial version.

===========================================================================*/

#ifndef _FW_AUTOPIN_INTF_H_
#define _FW_AUTOPIN_INTF_H_

#ifdef __cplusplus
extern "C" {
#endif

#define FW_AUTOPIN_MIN_CAP_LEAD_TIME_USTMR  (768)


/*! @Structure holding Sample capture related params for TX */
typedef struct {
  /*! @brief TXC rate in nanoseconds (timeperiod in which TXC rolls over) */
  uint32 txc_rate_ns;
  /*! @brief TXC sampling rate */
  uint32 txc_input_freq_hz;
  /*! @brief TX rotator clock rate */
  uint32 txrot_clk_hz;
  /*! @brief TXC IQ down sampling rate */
  uint32 txc_down_sampling_rate;  
  /*! @brief Capture time in USTMR units ( the time at which first TXC samples leave DAC in the target slot) */
  uint32 tx_capture_time;
  /*! @brief Capture time delay base on tx_capture_time, IQ streaming happens after the delay */
  uint32 tx_capture_delay;
  /*! @brief Capture length is number of TXC samples need to capture, depend on 
      RFSW's down sampling rate, TXC sample rate and capture time duration */
  uint32 capture_length;
  /*! @brief TX chain index where IQ sample is captured */
  uint32 tx_chain;
  /*! @brief RFSW buffer pointer for saving TXC IQ samples for debuging purpose */
  uint32* tx_iq_sample_ptr;
  /*! @brief boolean for FTM calibration mode */
  boolean ftm_mode;
  /*! @brief boolean for immediate set TXC reflog register. Should only be used for TDS online case. 
      For TDS online mode, need to program TXC reflog register before TDS set OFFL_EN */
  boolean imme_trig;

} fw_autopin_capture_input_t;

/*! @brief Structure holding Sample processing related params for TX PAR processing */
typedef struct {
  /*! @brief Dummy params */
  uint32 dummy;
} fw_autopin_psamp_input_t;

/*! @brief Debug parameters from processing */
typedef struct {
  /*! @brief Dummy debug params */
  uint32 dummy;
} fw_autopin_psamp_debug_out_t;

/*! @brief Result parameters from processing */
typedef struct {
  /*! @brief PAR output in some Q format  */
  /* it is in dB100 format, Q16 accuracy 
   e.g. 0.1234dB in dB100 format is 12.34, in Q16 accuracy 
  fix point value is 0xc570a */
  uint32 par;
  /*! @brief mean power is the average power of all samples in Q0 format*/
  uint16 mean_pwr;
   /*! @brief peak_avg_pwr is the average power of at most 16 peaks in Q0 format*/
  uint16 peak_avg_pwr;
  /*! @brief indicate this PAR result is valid or not */
  boolean valid_result;
} fw_autopin_psamp_result_out_t;

/*! @brief Output parameters as a part of PSAMP processing */
typedef struct {
  /*! @brief Debug output params */
  fw_autopin_psamp_debug_out_t debug;
  /*! @brief PAR results for Autopin */
  fw_autopin_psamp_result_out_t result;
} fw_autopin_output_params_t;


/*! @brief Autopin processing function callback type */
typedef void (*fw_autopin_proc)(fw_autopin_capture_input_t *, 
                                fw_autopin_psamp_input_t *,
                                fw_autopin_output_params_t *,
                                void (*result_funcptr)(fw_autopin_output_params_t *pout));


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _FW_AUTOPIN_INTF_H_
