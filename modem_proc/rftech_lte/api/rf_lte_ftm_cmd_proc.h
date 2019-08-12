/*!
  @file rf_lte_ftm_cmd_proc.h

  @brief
  This file contains the interface to the RF LTE FTM Calibration Command Processor

*/

/*============================================================================
                Copyright (c) 2008 - 2012 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_lte/api/rf_lte_ftm_cmd_proc.h#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 07:59:25 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/12/12   pl      Moved FTM command processing to FTM task
                   removed obsolete RFA_RF_LTE_FTM messages
05/22/12   aca     Added FTM command to extract RX AGC data
04/04/12   pl       Remove obsolete SMPS interface
04/04/12   pl      Remove obsolete SMPS interfaces
03/18/12   jfc     Remove tramp.h references. Use void * func pointer
04/20/11   pl      Remove unused functions
03/08/11   aca	    Added functions used for NPA voting for max CPU
02/22/11 whc/can    Initial APT support.
01/13/11   mkv      Adding rflte_ftm_exit_mode_cmd declaration.
15/10/10   mkv      Support LTE Composite Calibration  
08/09/10   qma      Updated LTE IM2 FTM cmd interface to match legacy
07/01/10   mkv      Adding support LTE LPM HDET Mode.   
06/18/10   jyu      Fix warning 
06/03/10   pl       Fix Warning
02/18/10   pl       Added set Secondary Chain function 
02/18/10   pl       Added get LNA offset function
11/22/09   mkv      Added LTE Calibration Diag Response dispatcher.
11/06/09   mkv      Initial Check-in
===========================================================================*/

#ifndef RF_LTE_FTM_CMD_PROC_H
#define RF_LTE_FTM_CMD_PROC_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "ftm.h"
#include "ftmdiag.h"
#include "msgr_types.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/
/*===========================================================================

  FUNCTION:  rf_lte_ftm_cal_msgr_register

===========================================================================*/
/*!
  @brief
  This function is used by RF Task to register for commands sent by the
  RF LTE FTM Calibration Dispatcher in the FTM task via the message router.


  @return
  TRUE if all registration were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rf_lte_ftm_cal_msgr_register( msgr_client_t *client_id, msgr_id_t id );

/*===========================================================================

  FUNCTION:  rf_lte_ftm_msgr_deregister

===========================================================================*/
/*!
  @brief
  This function is used by RF Task to de-register the messages registered earlier 
  for handling the messages sent by the RF LTE FTM Calibration processor to the message router


  @return
  TRUE if all de-registration were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rf_lte_ftm_cal_msgr_deregister( msgr_client_t *client_id );

/*===========================================================================

  FUNCTION:  rf_lte_ftm_calibration_response_processor

===========================================================================*/
/*!
  @brief
  This function is used by the FTM task to dispatch the MSGR responses from
  RF Task to the appropriate build diag response packet function . 


  @return
  void
*/
/*=========================================================================*/
void rf_lte_ftm_calibration_response_processor( ftm_msgr_u *cmd_data_ptr, ftm_rsp_pkt_type *rsp_pkt_ptr );


/*===========================================================================*/
/*!
  @brief This function builds the DIAG response packet for 
		 RFA_RF_LTE_FTM_GET_HDET_FROM_TX_SWEEP_RSP

  @param rsp_pkt_ptr pointer to the DIAG response packet
  @param index  HDET index

  @return None
*/
/*=========================================================================*/
void rf_lte_ftm_get_hdet_from_tx_sweep_rsp_diag_pkt(ftm_rsp_pkt_type *rsp_pkt_ptr, uint16 index );

/*===========================================================================*/
/*!
  @brief This function builds the DIAG response packet for 
		 RFA_RF_LTE_FTM_GET_ALL_HDET_FROM_TX_SWEEP_RSP

  @param rsp_pkt_ptr pointer to the DIAG response packet

  @return None
*/
/*=========================================================================*/
void rf_lte_ftm_get_all_hdet_from_tx_sweep_rsp_diag_pkt(ftm_rsp_pkt_type *rsp_pkt_ptr);

/*===========================================================================*/
/*!
  @brief This function builds the DIAG response packet for 
		 RFA_RF_LTE_FTM_GET_LPM_HDET_FROM_TX_SWEEP_RSP

  @param rsp_pkt_ptr pointer to the DIAG response packet
  @param index  HDET index

  @return None
*/
/*=========================================================================*/
void rf_lte_ftm_get_lpm_hdet_from_tx_sweep_rsp_diag_pkt(ftm_rsp_pkt_type *rsp_pkt_ptr, uint16 index );

/*===========================================================================*/
/*!
  @brief This function builds the DIAG response packet for 
		 RFA_RF_LTE_FTM_GET_ALL_LPM_HDET_FROM_TX_SWEEP_RSP

  @param rsp_pkt_ptr pointer to the DIAG response packet

  @return None
*/
/*=========================================================================*/
void rf_lte_ftm_get_all_lpm_hdet_from_tx_sweep_rsp_diag_pkt(ftm_rsp_pkt_type *rsp_pkt_ptr);
/*===========================================================================*/
/*!
  @brief This function performs the Tx Rx Sweep for LTE and returns a response packet

  @param rx_chain                Defines PRx Vs DRx
  @param num_freqs               Number of Frequecies to perform the sweep
  @param num_steps_per_freq      Number of segments per frequency
  @param step_length             Duration of each step
  @param freq_list_ptr           Pointer to the list of frequencies
  @param pdm_list_ptr            List of PDM values
  @param pri_exp_agc_val_ptr     List of Rx Power Values
  @param pa_range_list_ptr       List of PA Range values
  @param read_hdet_ptr           List of HDET reads
  @param read_hdet_ptr           Rx Action list to determine Gain range
  @param read_hdet_ptr           Rx LPM list

  @return uint16                 Indicates status of LTE Tx Rx Sweep
*/
/*=========================================================================*/
uint16 ftm_lte_do_tx_rx_freq_cal_sweep(uint16 rx_chain,
                                   uint8  num_freqs,
                                   uint8  num_steps_per_freq,
                                   uint16 step_length,
                                   uint16 *freq_list_ptr,
                                   uint16 *pdm_list_ptr,
                                   int16  *pri_exp_agc_val_ptr,
                                   uint8  *pa_range_list_ptr,
                                   uint8  *read_hdet_ptr,
                                   uint8  *rx_action_ptr,
                                   uint8  *rx_lpm_list_ptr);

/*!
  @brief This function creates the timers used by the ISR


  @return
  void
*/
/*=========================================================================*/
void ftm_lte_install_rfcal_timer(uint16 interval, void (*cb_func) (void) );

/*!
  @brief This function uninstalls the timers used by the ISR


  @return
  void
*/
/*=========================================================================*/
void ftm_lte_uninstall_rfcal_timer(void);

#endif /*  RF_LTE_FTM_CMD_PROC_H */

