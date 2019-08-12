#ifndef RFCOMMON_DATA_H
#define RFCOMMON_DATA_H

/*! 
  @file
  rfcommon_data.h
 
  @brief
  This file contains prototypes and definitions to be used by centralized 
  RF COMMON Global Data.
*/

/*==============================================================================

  Copyright (c) 2012 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/inc/rfcommon_data.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
08/06/14   aro     Added FBRx data structure and access interface 
                   in common data
11/11/13   kma     Add pin IQ capture type to support pin cal
01/30/14   jmf     Added XPT IQ capture type to internal state variable  
11/18/13   cd      Added debug flag for RFLM based Tx AGC override
07/17/13   jmf     [xPT] New Delay Table separate from DPD data module
04/15/13   Saul    Common. Consolidated XPT data into one structure.
04/15/13   Saul    Common. DPD APIs use critical section.
04/12/13   Saul    [Common] Use RF dispatch for fw responses.
03/19/13   aro     Added XPT CDMA cal deconfig
03/19/13   aro     Added call to XPT cal config from protocal IQ capture
03/19/20   aro     Added Data module hookup
03/05/13   aro     Enhanced XPT override command and migrated to FTM common
                   dispatch
02/04/13   aro     Migrated the variable to enable/disable slave resource
                   to RF common data
01/10/13   cri     Track allocated DPD tables 
11/19/12   aki     Added rfcommon_data_deinit() function
11/05/12   Saul    Common XPT Sample Capture. Support for capture and proc rsp.
10/19/12   Saul    COMMON XPT. Partitioned dpd data per tx path.
08/08/12   Saul    CDMA. Added code to profile writing dpd tables.
07/23/12   Saul    RF COMMON. MDSP/MSGR framework. Initial Revision.

==============================================================================*/

#include "comdef.h"
#include "rfcommon_mdsp_data.h"
#include "rfcommon_semaphore.h"
#include "rfcommon_fw_response.h"
#include "rfcommon_mdsp_types.h"
#include "rfcommon_core_xpt.h"
#include "rfcommon_core_xpt_buffer.h"
#include "rfcommon_fbrx_data.h"


#ifdef __cplusplus
extern "C" {
#endif

/* Number of bytes that fit */
#define MAX_DPD_BANK_SIZE 1024
/* Number of tables that fit in a bank */
#define MAX_DPD_TBLS_PER_BANK MAX_DPD_BANK_SIZE/(64*2)
/* Number of banks allocated to a tx path */
#define MAX_DPD_BANKS 6 /* WARNING: If this val changes, search for refs in SW for 
                           calculations that depend on this val */

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing xpt dpd data.
*/ 
typedef struct
{
  /*! Mask representing which banks are full of data */
  uint16 bank_full_mask;
  /*! Mask representing which banks are currently allocated */
  uint16 bank_allocation_mask;
  /*! Mask representing which banks have any data */
  uint16 bank_used_mask;
  /*! Contains the amount of used space in the corresponding bank */
  uint16 bank_used_space[MAX_DPD_BANKS];
  /*! Indicates the type of dpd data contained in the bank */
  load_dpd_input_table_type dpd_types_in_bank[MAX_DPD_BANKS][MAX_DPD_TBLS_PER_BANK];
  /*! Contains the actual dpd data */
  uint8 dpd_data[MAX_DPD_BANKS][MAX_DPD_BANK_SIZE] ALIGN(4);
} rfcommon_xpt_dpd_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure contains tx status items.
*/ 
typedef struct
{
  /*! The xpt data */
  rfcommon_xpt_dpd_data_type xpt_dpd_data;
} rfcommon_data_tx_status_type;

/*----------------------------------------------------------------------------*/
/*! Structure to keep track of IQ Capture Data */
typedef struct
{
  rfm_device_enum_type device;
  /*! RFM device  */

  rfm_device_enum_type device_fb;
  /*! RFM feedback device  */

  /*! RF Channel for which the IQ capture was done */
  uint32 channel;

  /*! Flag indicating, if the IQ Capture command is the LAST Trigger */
  boolean is_last_trig;

  /*!< Structure to store current buffer IDs */
  rf_common_xpt_buffer_id_type buffer_id;

  /*!< Type of delay capture - to differentiate LTE BWs 5,10,20 */
  rf_common_xpt_delay_capture_type delay_capture_type;

  /*!< Type of sample capture - to differentiate FBRx type Capture for FW */
  rfcommon_xpt_capture_type capture_type;

  /*!< Store IQ cap samp rate to convert FW processed ET delay */
  uint32 samp_rate;

  /*!< Type of pin capture */
  rf_common_xpt_pin_capture_type pin_capture_type;

}rfcommon_xpt_cal_iq_capture_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by/during XPT
  calibration.
*/
typedef struct
{
  /*! Flag indicating, if the XPT IQ Capture initiated by FW is in process 
  (during RF Calibration). This flag will be set when RF receives the IQ Capture 
  done message from FW.*/
  boolean is_iq_capture_in_process;

  /*! Flag indicating, if the XPT IQ Processing initiated by FW is in process 
  (during RF Calibration). This flag will be set when RF receives the Processing 
  done message from FW.*/
  boolean is_processing_in_process;

  /*! Structure to keep track of IQ Capture Data */
  rfcommon_xpt_cal_iq_capture_data_type iq_cap_data;

} rfcommon_xpt_cal_data_type;

/*----------------------------------------------------------------------------*/
/*! Structure to keepm track of XPT override data */
typedef struct
{
  boolean override_en;
  /*!< Flag indicating if the override is enabled or not */

  xpt_cfg_type xpt_mode;
  /*!< XPT mode to be overridden to */
} rfcommon_xpt_override_data;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common XPT Data used by RF COMMON Driver. 
*/ 
typedef struct
{
  /*! XPT Global Mutex */
  rf_lock_data_type xpt_crit_section;

  /*! DPD Info */
  rfcommon_mdsp_dpd_info_type dpd_info;

  /*! Structure containing XPT cal data */
  rfcommon_xpt_cal_data_type xpt_cal_data;

  /*! Structure to store the XPT override data */
  rfcommon_xpt_override_data xpt_override;

} rfcommon_xpt_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure Containing Common Global Data used by RF COMMON Driver. 
*/ 
typedef struct
{
  /*! Common driver status */
  rfcommon_data_tx_status_type *tx_status[2];

  /*! Data structure to hold all RF COMMON MDSP data */
  rfcommon_mdsp_data_type mdsp;

  /*! Data structure for FW response */
  rfcommon_fw_response_data_type fw_response;
      
  /*! Data structure to hold RF COMMON semaphore information */
  rfcommon_semaphore_data_type semaphore;

  /*! Common XPT data structure */
  rfcommon_xpt_data_type xpt_data;

  /*! Common FBRx data structure */
  rfcommon_fbrx_data_type fbrx_data;

} rfcommon_data_type;

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Structure containing debug flags for RF COMMON driver. 
*/ 
typedef struct
{
  boolean use_new_fwrsp;
  boolean use_rflm_txagc_override;
} rfcommon_debug_flags_t;

/*----------------------------------------------------------------------------*/
extern rfcommon_debug_flags_t rfcommon_debug_flags;

/*----------------------------------------------------------------------------*/
boolean
rfcommon_data_init
(
  void
);

/*----------------------------------------------------------------------------*/
void
rfcommon_data_deinit
(
  void
);

/*----------------------------------------------------------------------------*/
void
rfcommon_data_init_xpt_dpd_data
(
   rfcommon_xpt_dpd_data_type* xpt_dpd_data
);

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_data_type *
rfcommon_get_mdsp_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_mdsp_dpd_info_type *
rfcommon_get_dpd_info
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_semaphore_data_type *
rfcommon_get_semaphore_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_fw_response_data_type *
rfcommon_get_fw_response_data
(
  void
);

/*----------------------------------------------------------------------------*/
rf_lock_data_type *
rfcommon_get_xpt_lock_data
(
  void
);

/*----------------------------------------------------------------------------*/
rfcommon_data_tx_status_type*
rfcommon_data_get_tx_status
(
   rfm_device_enum_type device
);

/*----------------------------------------------------------------------------*/
rfcommon_xpt_cal_data_type*
rfcommon_data_get_xpt_cal_data
(
   void
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_configure_xpt_override
(
  rfm_device_enum_type device,
  boolean override_en,
  xpt_cfg_type xpt_mode
);

/*----------------------------------------------------------------------------*/
boolean
rfcommon_is_xpt_override
(
  rfm_device_enum_type device,
  xpt_cfg_type *xpt_mode
);

/*------------------------------------------------------------------------------------------------*/
rfcommon_fbrx_data_type*
rfcommon_get_fbrx_data
(
  void
);

#ifdef __cplusplus
}
#endif

#endif /* RFCOMMON_DATA_H */
