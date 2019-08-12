/*!
  @file
  rfgsm_core_data_manager.c

  @brief
  CCS event definitions to define the memory allocation for RF GSM in the 
  RFGSM CORE data-manger
*/

/*===========================================================================

  Copyright (c) 2013 - 2016 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/rf/core/src/rfgsm_core_data_manager.c#1 $
$DateTime: 2016/12/13 07:59:51 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/08/16   bp      Put the QPOET to standby mode after GSM Tx burst ends
01/14/16   ase     Store and dump CCS build script errors
12/07/15   bp      Add API to set TX_STOP CCS event priority to be used in HMSC case
08/19/15   par     Deprecate slot mode control support
07/27/15   par     Swap order for populating RFLM intf idx and data for Rx
02/24/15   piy     Added a check for UMID for both ASD Response and Device Status Read
01/22/15   piy     Fixed bug in Device status read feature for Mon Burst
01/13/15   sml     GL1 -> RF handshake mechanism to handles cases when 
                   GSM ASD response is not being received as scripts 
                   weren't executed
12/18/14   ec      Initialise holdoff flag in JDET data to FALSE
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
11/17/14   sc      Read and calculate delay match DM1-3 in RFLM GSM TxFE config
11/12/14   sw      Rename RX/TX script types to reflect actual devices
10/23/14   sc      Enable event type control from RFSW
10/15/14   sw      Program stop rx tuner and asd scripts for probe burst
10/07/14   sc      Program all delay match settings in RFLM DRIF
10/02/14   sc      Move dBm100 to dBm10 to RFSW to reduce overhead on RFLM
10/01/14   tsr     GSM IDLE Mode RXD support
10/01/14   sc      Reduce overhead on getting table ptr
09/19/14   sb      Add IP2 cal moving averaging + pass/fail status
09/24/14   hoh     Add multislot APT control feature
09/19/14   ch      Introduce RFLM_GSM_PROCESSING_OVERHEAD to fix 144MHz GSM voice call drop
08/28/14   sc      Clean up Tx data initialisation to reduce wakeup timeline
11/08/14   sw      Make ASD subscription aware
08/08/14   ch      Fix KW error NULL ptr check
08/04/14   cj      Added common dm event wrapper
07/24/14   cj      Added changes for Tx sharing
07/30/14   hoh     Add enhanced rx feature support 
07/28/14   ec      Initialise Jammer Detector DM memory
07/25/14   sb      bug fixes for V1 IP2 cal functionality
07/11/14   ec      Use API to get SAWless status
07/09/14   tsr     Remove RXD script buffer from RX CCS event programming 
                   as RXD Script is populated as part of PRX RX script
07/09/14   tws     Add new RX burst type PCH
07/02/14   tsr     DR-DSDS: Add worst case rx alpha for IDLE mode RX
07/01/14   tsr     DR-DSDS: Lower RX CCS event priority when GSM sub is in Idle mode
07/01/14   jr      Enable therm read on sub-2 
06/26/14   ch      added set tx power for CL tuner processing
06/19/14   sb      changes to support cal on c2 for DSDA
05/30/14   ldg     Add slot mode control function
05/29/14   sb      IP2 cal bug fixes, updates
05/23/14   sw      Code optimisation - do not create event if there are no transactions
05/23/14   sc      Log intf data in RFLM trace buffer to tie up RFSW timeline
05/20/14   sw      Fix OFT compiler errors
05/13/14   tsr     Remove rx stop script from CCS eevnt building for RXD( not required)
05/08/14   sc      Add RFLM GSM init once API interface
05/06/14   tsr     Reuse ccs event handles for RX/TX start stop & trans 
04/25/14   tsr     Optimize RX CCS events programming
04/24/14   ch      Enable AOL Tuner support in Rx and Tx burst start
04/22/14   ch      Added Rx Tx Frontend config events
04/16/14   sb      Fix KW
04/16/14   sb      IP2 cal Interface and functionality changes
04/10/14  tsr      Add RFLM RX gain and spectral inv coefficients for DRX 
04/07/14   sc      Clean event API
04/07/14   sc      Fix already allocated RFLM GSM DM check return logic
04/03/13   ggs     Re-fix ASD typo that was backed out from last check in
03/28/14   hoh     Revert back CL5565452 due to boot up failure
03/28/14   sw      Correct ASD typo
03/26/14   sb      Add support for IP2 cal for Dime PM
03/21/04   tsr     Program PA Stop in TX Stop script
03/19/04   tsr     Correct tx_stop_ccs_scripts_taskq_b array size
03/19/14   sc      Add set RFLM DM device function
03/11/04   tsr     Added RXD support to RX Burst
03/13/14   sw      Improve RX timeline by reducing ASD check overhead
03/06/14   tsr     Program device scripts dynamically through CCS
03/06/14   sc      Correct the PA ramp up table for accurate PA ramp adjust
03/05/14   sw      Introduce Antenna Switch Diversity
02/28/14   sc      Pass core data through mc to core layers
02/26/14   sc      Pass core data through cmd processing and mc layer
02/21/14   sc      Off-target atomic_inc_return solution
02/21/14   ggs     Fix Tx cal crash due to wrong assertion 
02/18/14   sc      Ensure thread safety on interface index operations
02/11/14   sw      Therm read event handle stored first before being incremented
02/05/14   sc      Use RFLM DM memory space for RF GSM data
01/30/14   sc      Manage data table independent of device
01/29/14   sc      Handle Rx burst types in more efficient mapping method
01/22/14   sc      Allow therm read to manage data based on rfm_dev
01/22/14   sc      Remove rfm_dev dependency and unrequired APIs - part 1
01/21/14   sw      Thermistor read changes
01/21/14   tsr     WTR TXAGC support for Multislot TX
01/21/14   ggs     Enable Pre-distortion for Bolt
01/21/14   sc      Move delay calculation to RFSW context
01/15/14   sc      Remove unused APIs using FW shared memory
27/12/13   cj      Debug info updated
12/13/13   sc      Add set predistortion flag API
12/10/13   sc      Add set envelope gain API functionality
12/09/13   sc      Remove unused monitor alpha update
11/11/13   ggs     Added call to initialise Tx cal mem pointer by calling
                   rflm_gsm_dm_set_tx_cal_table_ptr 
11/07/13   sc      Remove init once from initialisation of intf data
11/07/13   sc      Rx gain API using Rx burst type
11/05/13   tsr     Updated the TX transition buffers in DM memory to correct CCS taskQs
11/05/13   ggs     Added rfgsm_core_get_tx_cal_dma_data_addr
                   Need to set Tx cal mem ptr to NULL after deallocation
10/28/13   ggs     Temporary F3 to track when AMAM AMPM table is set
10/23/13   tsr     WTR3925 support for Bolt
10/21/13   sc      Modify dynamic event building for scenario-specific wrappers
10/21/13   sc      Mainline dynamic event building
10/15/13   sc      Add RFLM GSM DM table ptr set API for when mem is in RFSW
10/14/13   sc      Mainline core DM and RFLM code
10/10/13   sc      Correct intf data population and PA ramp RFLM band info
10/08/13   sc      Correct intf data pointer
10/03/13   sc      RFLM DM interface data and dynamic event development
09/27/13   sc      Modify RFLM GSM interface data programming functionality
09/12/13   sc      Add dynamic CCS event creation APIs
09/11/13   sc      Move calls to alloc/free DM mem to enter and exit mode
09/11/13   sc      Correct Tx calibration data type name 
08/19/13   sc      Prepare for merge from core DM to RFLM
08/13/13   sc      Rename definitions ready for RFLM GSM merging
08/08/13   sc      Add master PA ramp table retrieval APIs
07/29/13   sc      Modify feature guarding and add debug mem alloc size
01/19/12   sc      Initial version

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "amssassert.h"
#include "rfcom.h"
#include "rfgsm_core_types.h"
#include "rfgsm_core_handle.h"
#include "rfgsm_core.h"
#include "rfgsm_data_manager.h"
#include "rfm_device_types.h"
#include "rfgsm_mdsp.h"
#include "rfgsm_msg.h"
#include "rfgsm_msm.h"
#include "modem_mem.h"
#include "msg.h"

#include "rflm_gsm_dm.h"
#include "rflm_ccs_rf_event_intf.h"
#include "rfcommon_nv_mm.h"

/*===========================================================================

                        EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
static rflm_gsm_tx_cal_data_type *rfgsm_rflm_tx_cal_data_table[RFM_MAX_WAN_DEVICES] = {0};
extern rfgsm_mdsp_ccs_handles_s  *rfgsm_mdsp_ccs_handles[RFM_MAX_WAN_DEVICES];

static rflm_gsm_intf *rflm_gsm_intf_data_ptr = NULL;

/* Mapping RFSW Rx burst type to RFLM GSM Rx type for Rx start */
static rflm_gsm_script_type rflm_gsm_rx_start_script_map[RFGSM_MAX_RX_BURST_TYPES] = {
  RFLM_GSM_DEFAULT_SCRIPT,
  RFLM_GSM_RX_START_SCRIPT,
  RFLM_GSM_RX_ACQ_START_SCRIPT,
  RFLM_GSM_RX_PCH_START_SCRIPT,
  RFLM_GSM_MON_START_SCRIPT,
  RFLM_GSM_MON_START_SCRIPT
};

/* Mapping RFSW Rx burst type to RFLM GSM Rx type for Rx stop */
static rflm_gsm_script_type rflm_gsm_rx_stop_script_map[RFGSM_MAX_RX_BURST_TYPES] = {
  RFLM_GSM_DEFAULT_SCRIPT,
  RFLM_GSM_RX_STOP_SCRIPT,
  RFLM_GSM_RX_ACQ_STOP_SCRIPT,
  RFLM_GSM_RX_PCH_STOP_SCRIPT,
  RFLM_GSM_MON_STOP_SCRIPT,
  RFLM_GSM_MON_STOP_SCRIPT
};

/* Macro to check if ASD response is required 
    Also, Check if the ASD UMID has been 
    populated correctly or not in rfgsm_core_handles. */
#define IS_ASDIV_SET( core_data ) \
  ( ( core_data->rfgsm_core_handle_ptr->set_antenna_position != RF_INVALID_VALUE_8BIT ) && \
    ( core_data->rfgsm_core_handle_ptr->asd_rsp == TRUE ) && \
    ( core_data->rfgsm_core_handle_ptr->asd_fw_rsp_umid != 0 ) )

/* Macro to convert Tx power from dBm100 to dBm10 without use of divide */
/* Max dBm < 35 therefore max dBm100 < 3500                             */
/* (3500*((2^16)/10)) < MAX_INT32 so accuracy of 2^16 is safe           */
#define RFGSM_DM_DBM100_TO_DBM10_ACCURACY_SHFT 16
#define RFGSM_DM_DBM100_TO_DBM10_SCALE 6554 //CEIL(2^RFGSM_DM_DBM100_TO_DBM10_ACCURACY_SHFT)/10)

#define RFGSM_DM_DBM100_TO_DBM10(tx_power_dbm100) \
        ( (tx_power_dbm100 * RFGSM_DM_DBM100_TO_DBM10_SCALE) >> RFGSM_DM_DBM100_TO_DBM10_ACCURACY_SHFT )



/*============================================================================= 
 
                             STATIC FUNCTIONS
 
=============================================================================*/
static boolean rfgsm_core_dm_validate_tx_table_params( uint8 burst_buffer_id,
                                                       rfgsm_core_dm_double_buff ping_pong_id );

static boolean rfgsm_core_dm_validate_rx_params( uint8 burst_buffer_id,
                                                 rfgsm_core_dm_script_type script_type,
                                                 uint8 burst_num );

static boolean rfgsm_core_dm_validate_therm_read_event_hdr( void* event_mem_ptr );

static boolean rfgsm_core_dm_populate_therm_read_event_hdr( rfgsm_core_data_type *core_data, 
                                                 void* event_mem_ptr );

static boolean rfgsm_core_dm_populate_device_status_read_event_hdr( rfgsm_core_data_type *core_data, 
                                                   void* event_mem_ptr, rfgsm_core_dm_script_type script_type,
                                                   rflm_gsm_ccs_task_q task_q, uint8 burst_num);

static uint32 rfgsm_core_dm_get_rflm_intf_data_idx( void );

static rflm_gsm_intf_data* rfgsm_core_dm_get_rflm_intf_data( uint32 intf_idx );

static uint32 rfgsm_core_dm_increment_intf_data_idx( void );

static boolean rfgsm_core_dm_populate_rflm_intf_data( rfgsm_core_data_type *core_data,
                                                      rflm_gsm_script_type rflm_script,
                                                      uint8 rflm_burst_num,
                                                      uint32 intf_idx );

void* rfgsm_core_dm_get_data_table_event_addr( rflm_gsm_data_type *table_ptr,
                                               uint8 burst_buffer_id,
                                               rfgsm_core_dm_script_type script_type,
                                               uint8 burst_num,
                                               rfgsm_core_dm_task_q task_q );

static boolean rfgsm_core_dm_dynamic_event_wrapper( rfgsm_core_data_type* core_data,
                                                    rfgsm_core_dm_script_type script_type,
                                                    rflm_gsm_ccs_task_q task_q,
                                                    rf_buffer_intf** ccs_scripts,
                                                    size_t num_scripts,
                                                    void** event_handle_ptr,
                                                    uint8 burst_num);

static boolean rfgsm_core_dm_device_status_event_wrapper( rfgsm_core_data_type* core_data,
                                                    rfgsm_core_dm_script_type script_type,
                                                    rflm_gsm_ccs_task_q task_q,
                                                    uint8 burst_num,
                                                    rflm_gsm_data_type *table_ptr);


/* Increment with wrap-around and return orginal index */
static inline unsigned int
rfgsm_core_dm_atomic_inc_return(atomic_word_t *target, unsigned int mask)
{
#ifndef TEST_FRAMEWORK
    unsigned int result;
    unsigned int tmp = 0;

    __asm__ __volatile__(
        "1: %0 = memw_locked(%2)\n"
        " %4 = add(%0, #1)\n"
        " %4 = and(%4, %3)\n"
        " memw_locked(%2, p0) = %4\n"
        " if !p0 jump 1b\n"
        : "=&r" (result),"+m" (*target)
        : "r" (target), "r" (mask), "r" (tmp)
        : "p0");

    return result;
#else
    #error code not present
#endif
}



/*============================================================================= 
 
                        RFLM INITIALISATION FUNCTIONS
 
=============================================================================*/
/*! @fn rfgsm_core_dm_rflm_init
 *  @brief This function initialises the RFLM GSM for operation
 *  @details This function initialises mutexes and state machine in RFLM GSM
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_rflm_init()
{
  if( !rflm_gsm_dm_init( rfgsm_mdsp_get_event_map() ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM RFLM init failed!");
    return FALSE;
  }
  
  MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM core DM RFLM init successful!");
  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_rflm_deinit
 *  @brief This function uninitialises the RFLM GSM for operation
 *  @details This function uninitialises mutexes and state machine in RFLM GSM
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_rflm_deinit()
{
  if(!rflm_gsm_dm_deinit())
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM RFLM deinit failed!");
    return FALSE;
  }
  
  MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM core DM RFLM deinit successful!");
  return TRUE;
}



/*============================================================================= 
 
                             VALIDATION FUNCTIONS
 
=============================================================================*/
/*! @fn rfgsm_core_dm_validate_tx_table_params
 *  @brief This function validates the device ID, burst triple buffer, and
 *         ping pong buffer
 *  @param burst_buffer_id - triple buffer ID
 *  @param ping_pong_id    - double buffer ID for ramp and pre-dist tables
 *  @return boolean        - function success status
 */
static boolean rfgsm_core_dm_validate_tx_table_params( uint8 burst_buffer_id,
                                                       rfgsm_core_dm_double_buff ping_pong_id )
{
  /* Check the burst buffer ID is valid */
  if( (burst_buffer_id >= RFLM_GSM_TRIPLE_BUFFER) && 
      (burst_buffer_id != RFLM_GSM_BUFFER_ID_NOT_REQUIRED) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM burst_buffer_id %d invalid", burst_buffer_id);
    return FALSE;
  }

  if( ping_pong_id >= RFGSM_CORE_MAX_DOUBLE_BUF )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM ping pong buffer %d invalid", ping_pong_id);
    return FALSE;
  }

  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_validate_rx_params
 *  @brief This function validates the device ID and burst triple buffer
 *  @param burst_buffer_id - triple buffer ID
 *  @param script_type     - which event type
 *  @param burst_num       - which burst number
 *  @return boolean        - function success status
 */
static boolean rfgsm_core_dm_validate_rx_params( uint8 burst_buffer_id,
                                                 rfgsm_core_dm_script_type script_type,
                                                 uint8 burst_num )
{
  /* Check the burst buffer ID is valid */
  if( (burst_buffer_id >= RFLM_GSM_TRIPLE_BUFFER) && 
      (burst_buffer_id != RFLM_GSM_BUFFER_ID_NOT_REQUIRED) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM burst_buffer_id %d invalid", burst_buffer_id);
    return FALSE;
  }

  /* Check the script type is valid */
  if ( script_type >= RFLM_GSM_MAX_SCRIPT_TYPES )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM script_type %d invalid", script_type);
    return FALSE;
  }

  /* Check the burst num is valid */
  if ( burst_num >= RFLM_GSM_MAX_RSSI_MEASUREMENTS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM burst_num %d invalid", burst_num);
    return FALSE;
  }

  return TRUE;
}

/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_validate_event_hdr
 *  @brief This function validates the event header to ensure that event id
 *   is only in IDLE or COMPLETED state
 *  @param rfm_dev         - RFM device ID
 *  @param event_mem_ptr   - RFLM DM memory ptr
 *  @return boolean        - function success status
 */
static boolean rfgsm_core_dm_validate_therm_read_event_hdr( void* event_mem_ptr )
                                                    
{
  /* Check to see if previous therm read event has been processed */
  /* If not, then do not schedule another one */
  if(((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->event_status != RFLM_RF_CCS_EVENT_IDLE && 
     ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->event_status != RFLM_RF_CCS_EVENT_COMPLETE)
  {
    /* @Todo Introduce an error count here */
    RF_MSG_1(MSG_LEGACY_ERROR,
             "THERM READ SBI Event status not in IDLE or COMPLETE, status = %d", 
             ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->event_status);

    /* This is to ensure that we don't send a second cmd to GFW if they haven't processed the first one */
    return FALSE;
  }

  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_populate_therm_read_event_hdr
 *  @brief This function populates therm read event header
 *  @param core_data       - Core data
 *  @param event_mem_ptr   - RFLM DM memory ptr
 *  @return boolean        - TRUE for success or FALSE if an error occurs
 */
static boolean rfgsm_core_dm_populate_therm_read_event_hdr( rfgsm_core_data_type *core_data, 
                                                 void* event_mem_ptr )
{
  /*Set the response required to TRUE so that CCS can send the response back.Setting the
    response required is done here because if it is passed in transaction settings.The 
    Event builder stores the event handle during event_start and look for a RF driver function 
    call back ptr which it can call upon successful completion of event. We pass in null ptr 
    which would result in crash
  */
  ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->response_required = TRUE;
 
  /* the UMID of ASD RSP*/
  ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[0]=(uint32)core_data->rfgsm_core_handle_ptr->therm_read.fw_therm_rsp_umid;
 
  /* Store device ID and event_handle_tag for retrieval on therm read response */ 
  /* @todo Replace device with sub-id */
  ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1]=(uint32)(((uint32)core_data->rfm_dev<<16)|
                                                                                            ((uint32)core_data->rfgsm_core_handle_ptr->therm_read.event_handle_tag & 0xFF));
  return TRUE;
}

/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_populate_device_status_read_event_hdr
 *  @brief This function populates Device Status read event header
 *  @param core_data       - Core data
 *  @param event_mem_ptr   - RFLM DM memory ptr
 *  @param script_type     - Script Type for which the Event Hdr is being populated
 *  @return boolean        - TRUE for success or FALSE if an error occurs
 */
static boolean rfgsm_core_dm_populate_device_status_read_event_hdr( rfgsm_core_data_type *core_data, 
                                                                    void* event_mem_ptr, rfgsm_core_dm_script_type script_type,
                                                                    rflm_gsm_ccs_task_q task_q, uint8 burst_num)
{
  /*Set the response required to TRUE so that CCS can send the response back.Setting the
    response required is done here because if it is passed in transaction settings.The 
    Event builder stores the event handle during event_start and look for a RF driver function 
    call back ptr which it can call upon successful completion of event. We pass in null ptr 
    which would result in crash
  */
  
  switch(script_type)
  {
    case RFLM_GSM_TX_STOP_SCRIPT:
	  if((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_A))
	  {
        core_data->rfgsm_core_handle_ptr->device_config_to_read.event_tag++;
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->response_required = TRUE;
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[0] = (uint32)core_data->rfgsm_core_handle_ptr->device_config_to_read.device_status_rsp_umid;
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1] = (uint32)(((uint32)core_data->rfm_dev<<28) |
                                                                                                     ((uint32)script_type<<24) |
                                                                                                     ((uint32)core_data->rfgsm_core_handle_ptr->device_config_to_read.event_tag<<16) |
                                                                                                      ((uint32)core_data->triple_buffer_id<<12) | 
                                                                                                      ((uint32)core_data->sub_id<<8) |
                                                                                                      ((uint32)burst_num<<4 | (uint32)RF_TX_BURST));
        MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "Device status read for device %d, script type %d with event tag %d",
                                           core_data->rfm_dev,
                                           script_type,
                                           core_data->rfgsm_core_handle_ptr->device_config_to_read.event_tag);
	  }
	  if((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable_grfc_read == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_B))
	  {
		//This will run only for TQ_B if GRFC Status Read is Enabled 		
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->response_required = FALSE;
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1] = core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable_grfc_read;	    
      }
      break;
    case RFLM_GSM_RX_STOP_SCRIPT:
    case RFLM_GSM_MON_STOP_SCRIPT:
    case RFLM_GSM_RX_ACQ_STOP_SCRIPT:
    case RFLM_GSM_RX_PCH_STOP_SCRIPT:
	  if((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_A))
	  {
        core_data->rfgsm_core_handle_ptr->device_config_to_read.event_tag++;
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->response_required = TRUE;
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[0] = (uint32)core_data->rfgsm_core_handle_ptr->device_config_to_read.device_status_rsp_umid;

        if(script_type == RFLM_GSM_MON_STOP_SCRIPT)
        {
          ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1] = (uint32)(((uint32)core_data->rfm_dev<<28) |
                                                                                                        ((uint32)script_type<<24) |
                                                                                                        ((uint32)core_data->rfgsm_core_handle_ptr->device_config_to_read.event_tag<<16) |
                                                                                                        ((uint32)core_data->triple_buffer_id<<12) |
                                                                                                        ((uint32)core_data->sub_id<<8) |
                                                                                                        ((uint32)burst_num<<4 | (uint32)RF_MON_BURST)); 
        }
        else
        {
          ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1] = (uint32)(((uint32)core_data->rfm_dev<<28) |
                                                                                                      ((uint32)script_type<<24) |
                                                                                                      ((uint32)core_data->rfgsm_core_handle_ptr->device_config_to_read.event_tag<<16) |
                                                                                                        ((uint32)core_data->triple_buffer_id<<12) |
                                                                                                        ((uint32)core_data->sub_id<<8) |
                                                                                                        ((uint32)burst_num<<4 | (uint32)RF_RX_BURST)); 
        }
		
         MSG_3(MSG_SSID_RF, MSG_LEGACY_HIGH, "Device status read for device %d, script type %d with event tag %d",
                                             core_data->rfm_dev, script_type,
                                             core_data->rfgsm_core_handle_ptr->device_config_to_read.event_tag);
	  }
	  if((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable_grfc_read == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_B))
	  {
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->response_required = FALSE;
        ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1] = (uint32)(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable_grfc_read);     	
	  }
	  
      break;
    case RFLM_GSM_TX_START_SCRIPT:
      ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->response_required = FALSE;
      ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1] = core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable_grfc_read;
	  break;
    case RFLM_GSM_RX_START_SCRIPT:
    case RFLM_GSM_MON_START_SCRIPT:
    case RFLM_GSM_RX_ACQ_START_SCRIPT:
    case RFLM_GSM_RX_PCH_START_SCRIPT:
      ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->response_required = FALSE;
      ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1] = (uint32)(core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable_grfc_read); 
      break;
    default:
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "Invalid Script Type for Device Status Read");
	  return FALSE;
  }
   
  return TRUE;
}


#ifdef FEATURE_RF_ASDIV
/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_populate_asd_event_hdr
 *  @brief This function populates asd event header
 *  @param core_data       - core data
 *  @param event_mem_ptr   - RFLM DM memory ptr
 *  @return boolean        - TRUE for success or FALSE if an error occurs
 */
static boolean rfgsm_core_dm_populate_asd_event_hdr( rfgsm_core_data_type *core_data, 
                                                 void* event_mem_ptr )
{
  
  /*Set the response required to TRUE so that CCS can send the response back */
    ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->response_required = TRUE;

  /* the UMID of ASD RSP*/
    ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[0]=(uint32)core_data->rfgsm_core_handle_ptr->asd_fw_rsp_umid;

  core_data->rfgsm_core_handle_ptr->asd_switch_info.rfm_dev = core_data->rfm_dev;
  core_data->rfgsm_core_handle_ptr->asd_switch_info.sub_id = core_data->rfgsm_core_handle_ptr->asd_sub_id;
  core_data->rfgsm_core_handle_ptr->asd_switch_info.l1_cb = core_data->rfgsm_core_handle_ptr->asd_l1_cb_func;
  core_data->rfgsm_core_handle_ptr->asd_switch_info.set_antenna_position = core_data->rfgsm_core_handle_ptr->set_antenna_position;
           
  /* CCS will store the info from scratch memory and return back to GRF in response payload*/
    ((rflm_rf_ccs_event_description_buffer_header_type*)event_mem_ptr)->scratch_mem[1]=(uint32)(&core_data->rfgsm_core_handle_ptr->asd_switch_info);  

  return TRUE;
}
#endif /* FEATURE_RF_ASDIV */

/*============================================================================= 
 
                       RFGSM CORE ALLOCATE DATA-MANAGER MEM
 
=============================================================================*/
/*! @fn rfgsm_core_dm_alloc_data_manager_mem
 *  @brief This function allocates memory for the data manager scripts, LUTs,
 *         and variables
 *  @param rfm_dev - RFM device ID
 *  @return rfgsm_rfc_event_table_type - pointer to the memory holding the information
 */
boolean rfgsm_core_dm_alloc_data_manager_mem( rflm_gsm_subscription_id sub_id )
{
  /* Initialise variables */
  rflm_gsm_dm_err_type rflm_gsm_dm_err = RFLM_GSM_DM_ERR_NONE;

  /* Allocate the data table pointer from DM */
  rflm_gsm_dm_err = rflm_gsm_dm_allocate_tech_buf( sub_id );
  if ( rflm_gsm_dm_err != RFLM_GSM_DM_ERR_NONE )
  {
    if ( rflm_gsm_dm_err == RFLM_GSM_DM_ALREADY_ALLOCATED )
    {
      /* Already have memory allocated for this subscription */
      MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, 
            "RF GSM core DM alloc data table error code %d for sub %d",
            rflm_gsm_dm_err, sub_id);
      return TRUE;
    }
    else
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "RF GSM core DM alloc data table error code %d for sub %d",
            rflm_gsm_dm_err, sub_id);
      return FALSE;
    }
  }

  MSG_2(MSG_SSID_RF, MSG_LEGACY_HIGH, 
        "RF GSM core DM data table %d bytes allocated for sub %d", 
        sizeof(rflm_gsm_data_type), sub_id);
  return TRUE;
}



/*============================================================================= 
 
                RFLM ALLOCATE TX CALIBRATION DATA-MANAGER MEM
 
=============================================================================*/
/*! @fn rfgsm_core_dm_alloc_tx_cal_data_manager_mem
 *  @brief This function allocates memory for the Tx Calibration data manager
 *         scripts, LUTs, and variables
 *  @param rfm_dev - RFM device ID
 *  @return rflm_gsm_tx_cal_table_type - pointer to the memory holding the information
 */
rflm_gsm_tx_cal_data_type* rfgsm_core_dm_alloc_tx_cal_data_manager_mem( rfm_device_enum_type rfm_dev )
{
  if( rfm_dev < RFM_MAX_WAN_DEVICES )
  {
    /* If the events table has not been allocated, allocate the table space */
    if( rfgsm_rflm_tx_cal_data_table[rfm_dev] == NULL )
    {
      rfgsm_rflm_tx_cal_data_table[rfm_dev] = (rflm_gsm_tx_cal_data_type*)modem_mem_alloc(sizeof(rflm_gsm_tx_cal_data_type), MODEM_MEM_CLIENT_RFA);
      if (rfgsm_rflm_tx_cal_data_table[rfm_dev] == NULL) // failed to allocate memory
      {
        RFGC_MSG(MSG_LEGACY_ERROR, "RF GSM core DM failed to create rfgsm_tx_cal_table");
      }
      else
      {
        /* Print message of how much memory was allocated */
        RFGC_MSG_1(MSG_LEGACY_HIGH, "RF GSM core DM rflm_gsm_tx_cal_table_type size is: %d bytes", sizeof(rflm_gsm_tx_cal_data_type));

        /* Initialise Memory */
        memset( rfgsm_rflm_tx_cal_data_table[rfm_dev], 0, sizeof(rflm_gsm_tx_cal_data_type) );

        /* Initialise the RFLM GSM DM ptr */
        if( !rflm_gsm_dm_set_tx_cal_table_ptr(rfm_dev, rfgsm_rflm_tx_cal_data_table[rfm_dev]) )
        {
          RFGC_MSG(MSG_LEGACY_ERROR, "RF GSM core DM failed to set RFLM GSM TX CAL DM table ptr");
        }

      }
    }
    else
    {
      RFGC_MSG(MSG_LEGACY_HIGH, "RF GSM core DM Tx Cal memory already allocated");
    }

    return rfgsm_rflm_tx_cal_data_table[rfm_dev];
  }

  RFGC_MSG(MSG_LEGACY_ERROR, "RF GSM core DM failed to init rflm_gsm_tx_cal_data_type dev ID invalid");
  return NULL;
}



/*============================================================================= 
 
                     RFGSM CORE ALLOCATE INTERFACE DATA MEM
 
=============================================================================*/
/*! @fn rfgsm_core_dm_alloc_intf_data_mem
 *  @brief This function allocates memory for the interface data
 *  @details This function will allocate memory a assign the intf data pointer
 *           with that memory. Further calls to this function will return the 
 *           same pointer and not allocate any more memory. This 
 *  @return rflm_gsm_intf - pointer to the memory holding the information
 */
rflm_gsm_intf* rfgsm_core_dm_alloc_intf_data_mem( void )
{
  /* If first init call and table has not already been allocated */
  if( rflm_gsm_intf_data_ptr == NULL )
  {
    /* Allocate the memory */
    rflm_gsm_intf_data_ptr = (rflm_gsm_intf*)modem_mem_alloc(sizeof(rflm_gsm_intf), MODEM_MEM_CLIENT_RFA);

    /* Check memory was allocated */
    if (rflm_gsm_intf_data_ptr == NULL) // failed to allocate memory
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM failed to create rflm_gsm_intf");
    }
    else //allocated interface data success
    {
      /* Print message of how much memory was allocated */
      MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM core DM rflm_gsm_intf size is: %d bytes", sizeof(rflm_gsm_intf));

      /* Initialise Memory */
      memset( rflm_gsm_intf_data_ptr, 0, sizeof(rflm_gsm_intf) );

      /* Assign the RFLM DM local ptr with intf data ptr */
      if( !rflm_gsm_dm_init_intf_buf( rflm_gsm_intf_data_ptr ) )
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM failed to init rflm_gsm_intf");
      }
    }
  }

  return rflm_gsm_intf_data_ptr;
}



/*============================================================================= 
 
                        RFLM DESTROY DATA-MANAGER MEM
 
=============================================================================*/
/*! @fn rfgsm_core_dm_free_data_manager_mem
 *  @brief This function frees all the memory allocated for RFGSM script, LUTs,
 *         and variables
 *  @param rfm_dev - RFM device ID
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_free_data_manager_mem( rflm_gsm_subscription_id sub_id )
{
  /* Initialise variables */
  rflm_gsm_dm_err_type rflm_gsm_dm_err = RFLM_GSM_DM_ERR_NONE;

  /* Free the DM GSM memory */
  rflm_gsm_dm_err = rflm_gsm_dm_deallocate_tech_buf( sub_id );
  if( rflm_gsm_dm_err != RFLM_GSM_DM_ERR_NONE )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM free data table error code %d for sub %d",
          rflm_gsm_dm_err, sub_id);
    return FALSE;
  }

  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM core DM data table free for sub %d", sub_id);
  return TRUE;
}



/*============================================================================= 
 
                    RFLM DESTROY TX CAL DATA-MANAGER MEM
 
=============================================================================*/
/*! @fn rfgsm_core_dm_free_tx_cal_data_manager_mem
 *  @brief This function frees all the memory allocated for RFGSM Tx Cal
 *         script, LUTs, and variables
 *  @param rfm_dev - RFM device ID
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_free_tx_cal_data_manager_mem( rfm_device_enum_type rfm_dev )
{
  boolean dm_mem_free = FALSE;

  if( rfm_dev < RFM_MAX_WAN_DEVICES )
  {
    /* only destory the table if its NOT already been destroyed */
    if( rfgsm_rflm_tx_cal_data_table[rfm_dev] != NULL )
    {
      /* Free the destroy buffer */
      modem_mem_free( rfgsm_rflm_tx_cal_data_table[rfm_dev], MODEM_MEM_CLIENT_RFA );
      dm_mem_free = TRUE;
      rfgsm_rflm_tx_cal_data_table[rfm_dev] = NULL;
      rflm_gsm_dm_set_tx_cal_table_ptr( rfm_dev, NULL );
    }
  }

  if ( dm_mem_free == FALSE )
  {
    RFGC_MSG(MSG_LEGACY_ERROR, "RF GSM core DM no Tx cal memory destroyed");
    return FALSE;
  }
    RFGC_MSG(MSG_LEGACY_HIGH, "RF GSM core DM Tx cal memory destroyed");

  return TRUE;
}



/*============================================================================= 
 
                     RFLM DESTROY INTERFACE DATA MEM
 
=============================================================================*/
/*! @fn rfgsm_core_dm_free_intf_data_mem
 *  @brief This function frees the memory allocated for interface data
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_free_intf_data_mem( void )
{
  boolean intf_data_mem_free = FALSE;

  /* only destory the table if its NOT already been destroyed */
  if( rflm_gsm_intf_data_ptr != NULL )
  {
    /* Free the destroy buffer */
    modem_mem_free( rflm_gsm_intf_data_ptr, MODEM_MEM_CLIENT_RFA );
    rflm_gsm_intf_data_ptr = NULL;
    intf_data_mem_free = TRUE;
  }

  if ( intf_data_mem_free == FALSE )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM core DM no interface data memory destroyed");
    return FALSE;
  }

  return TRUE;
}



/*============================================================================= 
 
                    RFGSM CORE GET DATA-MANAGER MEM ADDR
 
=============================================================================*/
/*! @fn rfgsm_core_dm_free_data_manager_mem
 *  @brief This function retrieves the memory address for RFGSM script, LUTs,
 *         and variables
 *  @param rfm_dev - RFM device ID
 *  @return rflm_gsm_tx_cal_table_type - pointer to memory
 */
rflm_gsm_tx_cal_data_type* rfgsm_core_dm_get_tx_cal_data_manager_addr( rfm_device_enum_type rfm_dev )
{
  rflm_gsm_tx_cal_data_type *rfgsm_rflm_tx_cal_data_table_ptr = NULL;

  /* Check the rfm device is valid*/
  if (rfm_dev < RFM_MAX_WAN_DEVICES)
  {
    rfgsm_rflm_tx_cal_data_table_ptr = rfgsm_rflm_tx_cal_data_table[rfm_dev];
  }
  else
  {
    RFGC_MSG(MSG_LEGACY_ERROR, "RF GSM core DM failed to get rflm_gsm_data_table_ptr: invalid rfm_dev");
  }

  return rfgsm_rflm_tx_cal_data_table_ptr;
}



/*============================================================================= 
 
                        RFLM GET DATA-MANAGER INTF DATA
 
=============================================================================*/
/*! @fn rfgsm_core_dm_get_rflm_intf_data_idx
 *  @brief This function retieves the rflm data information index
 *  @details This function gets the RFLM GSM interface data index
 *  @retval - uint32 value of the array index
*/
static uint32 rfgsm_core_dm_get_rflm_intf_data_idx()
{
  /* Validate the interface data pointer */
  if ( rflm_gsm_intf_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM get intf data idx NULL!");
    return RF_INVALID_VALUE_32BIT;
  }

  /* return the index value */
  return rfgsm_core_dm_increment_intf_data_idx();
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_get_rflm_intf_data
 *  @brief This function retieves the rflm data information location
 *  @details This function gets the RFLM GSM interface data within array
 *  @retval - pointer to location of interface data to populate
*/
static rflm_gsm_intf_data* rfgsm_core_dm_get_rflm_intf_data( uint32 intf_idx )
{
  /* Initialise variables */
  rflm_gsm_intf_data *rflm_intf_data_ptr = NULL;

  /* Validate the interface data pointer */
  if ( rflm_gsm_intf_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM get intf data pointer NULL!");
    return NULL;
  }

  if( intf_idx >= RFGSM_CORE_DM_MAX_INTF_DATA_ENTRIES )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM get intf data idx invalid!");
    return NULL;
  }

  /* retrieve the interface data location to populate */
  rflm_intf_data_ptr = &rflm_gsm_intf_data_ptr->intf_data[intf_idx];

  /* return the pointer to the interface data to populate */
  return rflm_intf_data_ptr;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_increment_intf_data_idx
 *  @brief This function increments the interface data index
 *  @details This index is used to manage the RFLM GSM interface data array
 *  @retval - void
*/
static uint32 rfgsm_core_dm_increment_intf_data_idx()
{
  /* Store the current idx */
  return rfgsm_core_dm_atomic_inc_return((atomic_word_t*)&rflm_gsm_intf_data_ptr->intf_data_idx, (unsigned int)RFGSM_CORE_DM_MAX_INTF_DATA_ENTRIES-1);
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_populate_rflm_intf_data
 *  @brief This function populates the rflm data information
 *  @details This function gets the RFLM GSM interface data array and populates
 *           the interface data
 *  @param rflm_data_ptr   - pointer to GFW shared mem RFLM data info
 *  @param rflm_script     - enum description of the type of script i.e enter mode
 *  @param rflm_buffer_id  - triple buffer to be used. 0 unless burst event
 *  @param rflm_burst_num  - which Rx/Tx burst is being processed. 0xFF unless burst event
 *  @param rflm_device     - which RFM device processing for
 *  @param rflm_script_ptr - the pointer to the shared memory location for RFLM
 *  @retval - success status boolean
*/
static boolean rfgsm_core_dm_populate_rflm_intf_data( rfgsm_core_data_type *core_data,
                                                      rflm_gsm_script_type rflm_script,
                                                      uint8 rflm_burst_num,
                                                      uint32 intf_idx )
{
  /* Initialise variables */
  rflm_gsm_intf_data *rflm_intf_data_ptr = NULL;

  /* Get the RFLM GSM interface data location to populate */
  rflm_intf_data_ptr = rfgsm_core_dm_get_rflm_intf_data(intf_idx);

  /* Check the RFLM data pointer */
  if( rflm_intf_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM get intf data is NULL!");
    return FALSE;
  }

  /* Populate the RFLM RF data information */
  rflm_intf_data_ptr->intf_data_idx = intf_idx; //for debug
  rflm_intf_data_ptr->script     = rflm_script;
  rflm_intf_data_ptr->buffer_id  = core_data->triple_buffer_id;
  rflm_intf_data_ptr->burst_num  = rflm_burst_num;
  rflm_intf_data_ptr->device     = (uint8)core_data->rfm_dev; //cast because rfm_device_enum_type cannot be included in RFLM
  rflm_intf_data_ptr->sub_id     = (rflm_gsm_subscription_id)core_data->sub_id;

  /* RFLM trace */
  rflm_gsm_log_intf_data(rflm_intf_data_ptr);

  return TRUE;
}



/*============================================================================= 
 
                 COMMAND-SPECIFIC RFLM GSM INTF DATA UPDATE
 
=============================================================================*/
/*! @fn rfgsm_core_dm_populate_enter_mode_intf_data
 *  @brief This function populates the enter mode interface data
 *  @details This function populates the command-specific interface data in
 *           the internally-managed RFLM GSM intf data index location
 *  @retval - boolean success status
*/
boolean rfgsm_core_dm_populate_enter_mode_intf_data( rfgsm_core_data_type *core_data, 
                                                     void *shared_mem_ptr )
{
  /* Get the current intf data index */
  uint32 idx = RF_INVALID_VALUE_32BIT;
  idx = rfgsm_core_dm_get_rflm_intf_data_idx();

  /* Populate the FW shared mem index value */
  if( !rfgsm_mdsp_populate_enter_mode_index( shared_mem_ptr, idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Enter mode FW intf data index update failed");
    return FALSE;
  }
#ifndef FEATURE_TX_SHARING
  /* Populate the interface data */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                              RFLM_GSM_ENTER_MODE_SCRIPT,
                                              RFGSM_CORE_DM_BURST_NUM_NOT_REQUIRED,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Enter mode FW intf data update failed");
    return FALSE;
  }
#else
  /* Populate the interface data */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
											  RFLM_GSM_RX_ENTER_MODE_SCRIPT,
                                              RFGSM_CORE_DM_BURST_NUM_NOT_REQUIRED,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Enter mode FW intf data update failed");
    return FALSE;
  }

#endif
  
  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_populate_wakeup_intf_data
 *  @brief This function populates the wakeup interface data
 *  @details This function populates the command-specific interface data in
 *           the internally-managed RFLM GSM intf data index location
 *  @retval - boolean success status
*/
boolean rfgsm_core_dm_populate_wakeup_intf_data( rfgsm_core_data_type *core_data, 
                                                    void *shared_mem_ptr )
{
  /* Get the current intf data index */
  uint32 idx = RF_INVALID_VALUE_32BIT;
  idx = rfgsm_core_dm_get_rflm_intf_data_idx();

  /* Populate the FW shared mem index value */
  if( !rfgsm_mdsp_populate_wakeup_index( shared_mem_ptr, idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Wakeup FW intf data index update failed");
    return FALSE;
  }

  /* Populate the interface data */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                              RFLM_GSM_WAKEUP_SCRIPT,
                                              RFGSM_CORE_DM_BURST_NUM_NOT_REQUIRED,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Wakeup FW intf data update failed");
    return FALSE;
  }

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_populate_tx_band_intf_data
 *  @brief This function populates the Tx band interface data
 *  @details This function populates the command-specific interface data in
 *           the internally-managed RFLM GSM intf data index location
 *  @retval - boolean success status
*/
boolean rfgsm_core_dm_populate_tx_band_intf_data( rfgsm_core_data_type *core_data, 
                                                  void *shared_mem_ptr )
{
  /* Get the current intf data index */
  uint32 idx = RF_INVALID_VALUE_32BIT;
  idx = rfgsm_core_dm_get_rflm_intf_data_idx();

  /* Populate the FW shared mem index value */
  if( !rfgsm_mdsp_populate_tx_band_index( shared_mem_ptr, idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Tx band FW intf data index update failed");
    return FALSE;
  }

  /* Populate the interface data */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                              RFLM_GSM_TX_BAND_SCRIPT,
                                              RFGSM_CORE_DM_BURST_NUM_NOT_REQUIRED,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Tx band FW intf data update failed");
    return FALSE;
  }

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_populate_therm_read_intf_data
 *  @brief This function populates the Therm read interface data
 *  @details This function populates the command-specific interface data in
 *           the internally-managed RFLM GSM intf data index location
 *  @retval - boolean success status
*/
boolean rfgsm_core_dm_populate_therm_read_intf_data( rfgsm_core_data_type *core_data, 
                                                     void *shared_mem_ptr )
{
  /* Get the current intf data index */
  uint32 idx = RF_INVALID_VALUE_32BIT;
  idx = rfgsm_core_dm_get_rflm_intf_data_idx();

  /* Populate the FW shared mem index value */
  if( !rfgsm_mdsp_populate_therm_read_index( shared_mem_ptr, idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Therm read FW intf data index update failed");
    return FALSE;
  }

  /* Populate the interface data */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                              RFLM_GSM_THERM_READ_SCRIPT,
                                              RFGSM_CORE_DM_BURST_NUM_NOT_REQUIRED,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Therm read FW intf data update failed");
    return FALSE;
  }

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_populate_rx_burst_intf_data
 *  @brief This function populates the Rx burst interface data
 *  @details This function populates the command-specific interface data in
 *           the internally-managed RFLM GSM intf data index location
 *  @retval - boolean success status
*/
boolean rfgsm_core_dm_populate_rx_burst_intf_data( rfgsm_core_data_type *core_data, 
                                                   void *shared_mem_ptr )
{
  /* Initialise variables */
  uint32 idx = RF_INVALID_VALUE_32BIT;

  /* Validate the Rx burst type */
  if ( core_data->rfgsm_core_handle_ptr->rx_burst_type >= RFGSM_MAX_RX_BURST_TYPES || 
       core_data->rfgsm_core_handle_ptr->rx_burst_type <= RFGSM_RX_DEFAULT_RX_BURST )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "RF GSM core DM Rx burst type %d invalid", 
          core_data->rfgsm_core_handle_ptr->rx_burst_type);
    return FALSE;
  }

  /******************************* Rx Start **********************************/
  /* Get the current intf data index */
  idx = rfgsm_core_dm_get_rflm_intf_data_idx();

  /* Populate the Rx start RFLM data info */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                              rflm_gsm_rx_start_script_map[core_data->rfgsm_core_handle_ptr->rx_burst_type],
                                              core_data->rfgsm_core_handle_ptr->rx_burst_num,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Rx start FW intf data update failed");
    return FALSE;
  }

  /* Populate the Rx start FW shared mem index value */
  if( !rfgsm_mdsp_populate_rx_start_index( shared_mem_ptr, idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Rx start FW intf data index update failed");
    return FALSE;
  }

  /******************************* Rx Stop ***********************************/
  /* Get the current intf data index */
  idx = rfgsm_core_dm_get_rflm_intf_data_idx();

  /* Populate the Rx stop RFLM data info */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                              rflm_gsm_rx_stop_script_map[core_data->rfgsm_core_handle_ptr->rx_burst_type],
                                              core_data->rfgsm_core_handle_ptr->rx_burst_num,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Rx stop FW intf data update failed");
    return FALSE;
  }

  /* Populate the Rx stop FW shared mem index value */
  if( !rfgsm_mdsp_populate_rx_stop_index( shared_mem_ptr, idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Rx stop FW intf data index update failed");
    return FALSE;
  }
  return TRUE;
}






/*===========================================================================*/
/*! @fn rfgsm_core_dm_populate_tx_burst_intf_data
 *  @brief This function populates the Tx burst interface data
 *  @details This function populates the command-specific interface data in
 *           the internally-managed RFLM GSM intf data index location
 *  @retval - boolean success status
*/
boolean rfgsm_core_dm_populate_tx_burst_intf_data( rfgsm_core_data_type *core_data, 
                                                   void *shared_mem_ptr, 
                                                   uint8 num_tx_slots )
{
  /* Initialise variables */
  uint32 slot_num = 0;
  uint32 idx = RF_INVALID_VALUE_32BIT;

  /* Check the number of Tx slots to process is not invalid */
  if ( num_tx_slots > RFGSM_MAX_TX_SLOTS_PER_FRAME )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM invalid num of UL slots %d", num_tx_slots);
    return FALSE;
  }

  /****************************** Tx Start ***********************************/
  /* Get the current intf data index */
  idx = rfgsm_core_dm_get_rflm_intf_data_idx();

  /* Populate the Tx start FW shared mem index value */
  if( !rfgsm_mdsp_populate_tx_start_index( shared_mem_ptr, idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Tx start FW intf data index update failed");
    return FALSE;
  }

  /* Populate the Tx start RFLM data info */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                              RFLM_GSM_TX_START_SCRIPT,
                                              0,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Tx start FW intf data update failed");
    return FALSE;
  }


  /****************************** Tx Trans ***********************************/
  for( slot_num = 0; slot_num < num_tx_slots; slot_num++ )
  {
    /* Get the current intf data index */
    idx = rfgsm_core_dm_get_rflm_intf_data_idx();

    /* Populate the Rx start FW shared mem index value */
    if( !rfgsm_mdsp_populate_tx_trans_index( shared_mem_ptr, idx, slot_num ) )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Tx trans FW intf data index update failed for slot %d", slot_num);
      return FALSE;
    }

    /* Populate the Tx trans RFLM data info */
    if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                                RFLM_GSM_TX_TRANS_SCRIPT,
                                                slot_num,
                                                idx ) )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Tx trans FW intf data update failed for slot %d", slot_num);
      return FALSE;
    }
  }


  /****************************** Tx Stop ************************************/
  /* Get the current intf data index */
  idx = rfgsm_core_dm_get_rflm_intf_data_idx();

  /* Populate the Rx start FW shared mem index value */
  if( !rfgsm_mdsp_populate_tx_stop_index( shared_mem_ptr, idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Tx stop FW intf data index update failed");
    return FALSE;
  }

  /* Populate the Tx start RFLM data info */
  if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                              RFLM_GSM_TX_STOP_SCRIPT,
                                              num_tx_slots-1,
                                              idx ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM Tx stop FW intf data update failed");
    return FALSE;
  }

  return TRUE;
}




/*============================================================================= 
 
                     RFGSM CORE EVENT TABLE INTERFACE
 
=============================================================================*/
/*! @fn rfgsm_core_dm_get_data_table_event_addr
 *  @brief This function dereferences into the event table to acquire the
 *         address of a specific event based on arguments passed into func
 *  @param table_ptr - pointer to RFLM data table
 *  @param burst_buffer_id - triple buffer ID
 *  @param burst_num - which burst required
 *  @param script_type - which event type we need
 *  @param task_q - which RFC task q event we want the address for
 *  @return void* - rfc_event_addr address of the event
 */
void* rfgsm_core_dm_get_data_table_event_addr( rflm_gsm_data_type *table_ptr,
                                               uint8 burst_buffer_id,
                                               rfgsm_core_dm_script_type script_type,
                                               uint8 burst_num,
                                               rfgsm_core_dm_task_q task_q )
{
  void *event_addr = NULL;

  /* Validate the table ptr */
  if ( table_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM get event addr table ptr NULL");
    return NULL;
  }

  switch ( script_type )
  {
#ifndef FEATURE_TX_SHARING
    case RFLM_GSM_ENTER_MODE_SCRIPT:
    case RFLM_GSM_WAKEUP_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_init_event.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_init_event.task_q_b;
      }
      break;
#else
  case RFLM_GSM_RX_ENTER_MODE_SCRIPT:
  case RFLM_GSM_RX_WAKEUP_SCRIPT:
    if( task_q == RFLM_GSM_CCS_TASK_Q_A )
    {
	  event_addr = (void*)&table_ptr->rfgsm_rx_init_event.task_q_a;
    }
    else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
    {
	  event_addr = (void*)&table_ptr->rfgsm_rx_init_event.task_q_b;
    }
    break;

  case RFLM_GSM_TX_ENTER_MODE_SCRIPT:
  case RFLM_GSM_TX_WAKEUP_SCRIPT:
    if( task_q == RFLM_GSM_CCS_TASK_Q_A )
    {
      event_addr = (void*)&table_ptr->rfgsm_tx_init_event.task_q_a;
    }
    else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
    {
      event_addr = (void*)&table_ptr->rfgsm_tx_init_event.task_q_b;
    }
    break;
#endif

    case RFLM_GSM_TX_BAND_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_tx_band_event.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_tx_band_event.task_q_b;
      }
      break;

    case RFLM_GSM_THERM_READ_SCRIPT:
       if( task_q == RFLM_GSM_CCS_TASK_Q_A )
       {
         event_addr = (void*)&table_ptr->rfgsm_therm_read_event.task_q_a;
       }
       else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
       {
         event_addr = (void*)&table_ptr->rfgsm_therm_read_event.task_q_b;
       }
      break;

    case RFLM_GSM_RX_FRONTEND_CONFIG_SCRIPT:
      /*RX frontend config event should already been processed in rfgsm_core_dm_create_event*/
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM invalid script type");
      break;

    case RFLM_GSM_TX_FRONTEND_CONFIG_SCRIPT:
       /*MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM core DM get event addr table ptr called for tuner RFLM_GSM_TX_FRONTEND_CONFIG_SCRIPT %d",task_q);*/
       if( task_q == RFLM_GSM_CCS_TASK_Q_A )
       {
         event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_frontend_config.task_q_a;
       }
       else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
       {
         event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_frontend_config.task_q_b;
       }
      break;

    case RFLM_GSM_TX_CAL_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      { 
        event_addr = (void*)NULL;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)NULL;
      }
      break;

    case RFLM_GSM_RX_START_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SYNC_RX_EVENT].rx_start.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SYNC_RX_EVENT].rx_start.task_q_b;
      }
      break;

    case RFLM_GSM_RX_STOP_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SYNC_RX_EVENT].rx_stop.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SYNC_RX_EVENT].rx_stop.task_q_b;
      }
      break;

    case RFLM_GSM_RX_ACQ_START_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SCH_OR_ACQ_RX_EVENT].rx_start.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SCH_OR_ACQ_RX_EVENT].rx_start.task_q_b;
      }
      break;

    case RFLM_GSM_RX_ACQ_STOP_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SCH_OR_ACQ_RX_EVENT].rx_stop.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SCH_OR_ACQ_RX_EVENT].rx_stop.task_q_b;
      }
      break;

    case RFLM_GSM_RX_PCH_START_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_PCH_RX_EVENT].rx_start.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_PCH_RX_EVENT].rx_start.task_q_b;
      }
      break;

    case RFLM_GSM_RX_PCH_STOP_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_PCH_RX_EVENT].rx_stop.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_PCH_RX_EVENT].rx_stop.task_q_b;
      }
      break;

    case RFLM_GSM_MON_START_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      { 
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].monitor[burst_num].rx_start.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].monitor[burst_num].rx_start.task_q_b;
      }
      break;

    case RFLM_GSM_MON_STOP_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].monitor[burst_num].rx_stop.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].monitor[burst_num].rx_stop.task_q_b;
      }
      break;

    case RFLM_GSM_TX_START_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_start.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_start.task_q_b;
      }
      break;

    case RFLM_GSM_TX_TRANS_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_trans[burst_num].task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_trans[burst_num].task_q_b;
      }
      break;

    case RFLM_GSM_TX_STOP_SCRIPT:
      if( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_stop.task_q_a;
      }
      else if( task_q == RFLM_GSM_CCS_TASK_Q_B )
      {
        event_addr = (void*)&table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_stop.task_q_b;
      }
      break;

     case RFLM_GSM_IP2_CAL_SETUP_SCRIPT:
       if( task_q == RFLM_GSM_CCS_TASK_Q_A )
       {
         event_addr = (void*)&table_ptr->rfgsm_ip2_cal_event.ip2_cal_setup.task_q_a;
       }

       break;

     case RFLM_GSM_IP2_CAL_STEP_SCRIPT:
       if( task_q == RFLM_GSM_CCS_TASK_Q_A )
       {
         if ( burst_num < RFLM_MAX_IP2_CAL_STEPS )
         {
           event_addr = (void*)&table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.ip2_cal_step[burst_num].task_q_a;
         }
         else
         {

           MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Get event addres for IP2 cal step :%d failed",burst_num);
         }
       }
       break;

     case RFLM_GSM_IP2_CAL_CLEANUP_SCRIPT:
       if( task_q == RFLM_GSM_CCS_TASK_Q_A )
       {
         event_addr = (void*)&table_ptr->rfgsm_ip2_cal_event.ip2_cal_cleanup.task_q_a;
       }
       break;

    case RFLM_GSM_RX_JDET_READ_SCRIPT:
      if ( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        if ( burst_num < RFLM_MAX_JDET_READS_PER_RX )
        {
            event_addr = (void*)&table_ptr->rfgsm_jdet_read_data.rx_jdet_read_script[burst_num].task_q_a;
        }
        else
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Get event address for Rx JDET read:%d failed",burst_num);
        }
      }
      break;

    case RFLM_GSM_MON_JDET_READ_SCRIPT:
      if ( task_q == RFLM_GSM_CCS_TASK_Q_A )
      {
        if ( burst_num < RFLM_JDET_NUM_MON_EVENTS )
        {
            event_addr = (void*)&table_ptr->rfgsm_jdet_read_data.mon_jdet_read_script[burst_num].task_q_a;
        }
        else
        {
          MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "Get event address for Mon JDET read:%d failed",burst_num);
        }
      }
      break;

    default:
      break;
  }

  return event_addr;
}



/*=============================================================================

                     RFGSM CORE PROGRAMMING RX RFLM DATA

=============================================================================*/
/*! @fn rfgsm_core_dm_get_rx_ptr
 *  @brief This function retrieves the pointer to the Rx information
 *         This function is intended to be used to set/get the Rx data after
 *         retrieving the pointer
 *  @param rfm_dev - RFM device ID
 *  @param burst_buffer_id - triple buffer ID
 *  @param script_type - which event type we need
 *  @param burst_num - which burst required
 *  @return rfgsm_rx_rfc_event - pointer to Rx data
 */
rflm_gsm_rx_data* rfgsm_core_dm_get_rx_ptr( rflm_gsm_data_type *table_ptr,
                                            uint8 burst_buffer_id,
                                            rfgsm_core_dm_script_type script_type,
                                            uint8 burst_num )
{
  rflm_gsm_rx_data *rx_data_ptr = NULL;

  /* Validate the table ptr */
  if ( table_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM get Rx data table addr table ptr NULL");
    return NULL;
  }

  if( !rfgsm_core_dm_validate_rx_params( burst_buffer_id, script_type, burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM Rx param error");
    return NULL;
  }

  switch ( script_type )
  {
    case RFLM_GSM_RX_START_SCRIPT:
    case RFLM_GSM_RX_STOP_SCRIPT:
      rx_data_ptr = &table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SYNC_RX_EVENT];
      break;

    case RFLM_GSM_RX_ACQ_START_SCRIPT:
    case RFLM_GSM_RX_ACQ_STOP_SCRIPT:
      rx_data_ptr = &table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_SCH_OR_ACQ_RX_EVENT];
      break;

    case RFLM_GSM_RX_PCH_START_SCRIPT:
    case RFLM_GSM_RX_PCH_STOP_SCRIPT:
      rx_data_ptr = &table_ptr->rfgsm_burst_event[burst_buffer_id].rx[RFLM_GSM_PCH_RX_EVENT];
      break;

    case RFLM_GSM_MON_START_SCRIPT:
    case RFLM_GSM_MON_STOP_SCRIPT:
      rx_data_ptr = &table_ptr->rfgsm_burst_event[burst_buffer_id].monitor[burst_num];
      break;

    default:
       MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM Rx invalid script type");
      break;
  }

  return rx_data_ptr;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_rx_gain
 *  @brief 
 *  @param sub_id - subscription
 *  @param rx_burst_type - Rx type
 *  @param burst_buffer_id - triple buffer ID
 *  @param burst_num - Rx burst number
 *  @param gain - gain of the Rx
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_set_rx_gain( rfgsm_core_data_type * core_data,
                                   rfgsm_rx_burst_type rx_burst_type,
                                   uint8 burst_num,
                                   uint16 gain)
{
  /* Initialise variables */
  rflm_gsm_rx_data* rx_data_ptr = NULL;
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Rx gain table_ptr NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }

  /* Validate the Rx burst type */
  if ( rx_burst_type >= RFGSM_MAX_RX_BURST_TYPES || 
       rx_burst_type <= RFGSM_RX_DEFAULT_RX_BURST )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM Rx burst type %d invalid", rx_burst_type);
    return FALSE;
  }

  /* Validate the Rx data */
  if( !rfgsm_core_dm_validate_rx_params( core_data->triple_buffer_id, 
                                         rflm_gsm_rx_start_script_map[rx_burst_type], 
                                         burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM Rx param error");
    return FALSE;
  }

  /* Get the Rx core data pointer for a specific device and triple buffer */
  rx_data_ptr = rfgsm_core_dm_get_rx_ptr( table_ptr, 
                                          core_data->triple_buffer_id, 
                                          rflm_gsm_rx_start_script_map[rx_burst_type], 
                                          burst_num );

  /* Check that the Rx rflm data pointer is not NULL */
  if( rx_data_ptr == NULL )
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, 
         "rx_data_ptr is NULL burst_buffer_id %d rflm rx start script %d burst_num %d",
         core_data->triple_buffer_id, rflm_gsm_rx_start_script_map[rx_burst_type], burst_num);
    return FALSE;
  }

  if (rfgsm_core_is_device_prx(core_data->rfm_dev)) 
  {
    /* Set the Rx data */
    rx_data_ptr->rf_gain_prx = gain;
  }
  else
  {
    /* Set the Rx data */
    rx_data_ptr->rf_gain_drx = gain;
  }

  /* Set the RFLM flag to indicate enhanced Rx status and offset to FW */
  rx_data_ptr->rflm_enh_rx_on_flag = core_data->rfgsm_core_handle_ptr->enh_rx_on_flag;
  rx_data_ptr->rflm_enh_rx_offset_freq = core_data->rfgsm_core_handle_ptr->enh_rx_freq_offset;

  return TRUE;
}

/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_rx_alpha
 *  @brief 
 *  @param sub_id - subscription
 *  @param alpha - Rx alpha value
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_set_rx_alpha( rflm_gsm_subscription_id sub_id,
                                    uint16 alpha ,
                                    uint16 idle_mode_rx_alpha)
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Rx alpha table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  /* Set the Rx data */
  table_ptr->rflm_gsm_vars.rx_alpha = alpha;
  table_ptr->rflm_gsm_vars.idle_mode_rx_alpha = idle_mode_rx_alpha;

  return TRUE;
}


/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_hmsc_tx_stop_priority
 *  @brief 
 *  @param sub_id - subscription
 *  @param priority - TX STOP CCS task priority to be used when in HMSC case
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_set_hmsc_tx_stop_priority( rflm_gsm_subscription_id sub_id,
                                                 uint8 priority )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx stop hmsc prio table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  /* Set the priority */
  table_ptr->rflm_gsm_vars.tx_stop_hmsc_prio = priority;

  return TRUE;
}


/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_spectral_inv
 *  @brief 
 *  @param sub_id - subscription
 *  @param rx_burst_type - Rx type
 *  @param burst_buffer_id - triple buffer ID
 *  @param burst_num - Rx burst number
 *  @param rsb_coefA, rsb_coefB - spec inv coefficients
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_set_spectral_inv( rfgsm_core_data_type* core_data,
                                        rfgsm_rx_burst_type rx_burst_type,
                                        uint8 burst_num,
                                        uint32 rsb_coefA,
                                        uint32 rsb_coefB )
{
  /* Initialise variables */
  rflm_gsm_rx_data* rx_data_ptr = NULL;
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Rx spec inv table_ptr NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }

  /* Validate the Rx burst type */
  if ( rx_burst_type >= RFGSM_MAX_RX_BURST_TYPES || 
       rx_burst_type <= RFGSM_RX_DEFAULT_RX_BURST )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,
          "RF GSM core DM set Rx spec inv Rx burst type %d invalid", 
          rx_burst_type);
    return FALSE;
  }

  if( !rfgsm_core_dm_validate_rx_params( core_data->triple_buffer_id, 
                                         rflm_gsm_rx_start_script_map[rx_burst_type], 
                                         burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Rx spec inv param error");
    return FALSE;
  }

  /* Get the Rx core data pointer for a specific device and triple buffer */
  rx_data_ptr = rfgsm_core_dm_get_rx_ptr( table_ptr, 
                                          core_data->triple_buffer_id, 
                                          rflm_gsm_rx_start_script_map[rx_burst_type], 
                                          burst_num );

  /* Check that the Rx rflm data pointer is not NULL */
  if( rx_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Rx spec inv rx_data_ptr NULL");
    return FALSE;
  }
  if (rfgsm_core_is_device_prx(core_data->rfm_dev)) 
  {
    /* Set the Rx data */
    rx_data_ptr->rsb_coefA_prx = rsb_coefA;
    rx_data_ptr->rsb_coefB_prx = rsb_coefB;
  }
  else
  {
    /* Set the Rx data for RXD device*/
    rx_data_ptr->rsb_coefA_drx = rsb_coefA;
    rx_data_ptr->rsb_coefB_drx = rsb_coefB;
  }

  return TRUE;
}



/*=============================================================================

                      RFGSM PROGRAMMING TX RFLM DATA

=============================================================================*/
/*! @fn rfgsm_core_dm_get_amam_table_addr
 *  @brief This function retrieves the amam table pointer
 *  @param sub_id       - subscription
 *  @param ping_pong_id - double buffer ID for ramp and pre-dist tables
 *  @return uint16*     - pointer to the memory holding the information
 */
uint16* rfgsm_core_dm_get_amam_table_addr( rflm_gsm_subscription_id sub_id, 
                                           rfgsm_core_dm_double_buff ping_pong_id )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx amam table_ptr NULL for sub %d",
          sub_id);
    return NULL;
  }

  if( ping_pong_id >= RFGSM_CORE_MAX_DOUBLE_BUF )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx amam table addr ping pong ID invalid %d", 
          ping_pong_id);
    return NULL;
  }

  return (table_ptr->rfgsm_txfe_data.amam_ampm_tables[ping_pong_id].amam_table);
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_get_ampm_table_addr
 *  @brief This function retrieves the 
 *  @param rfm_dev - RFM device ID
 *  @param ping_pong_id - double buffer ID for ramp and pre-dist tables
 *  @return uint16* - pointer to the memory holding the information
 */
uint16* rfgsm_core_dm_get_ampm_table_addr( rflm_gsm_subscription_id sub_id,
                                           rfgsm_core_dm_double_buff ping_pong_id )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx ampm table_ptr NULL for sub %d",
          sub_id);
    return NULL;
  }

  if( ping_pong_id >= RFGSM_CORE_MAX_DOUBLE_BUF )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx ampm table addr ping pong ID %d invalid", 
          ping_pong_id);
    return NULL;
  }

  return (table_ptr->rfgsm_txfe_data.amam_ampm_tables[ping_pong_id].ampm_table);
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_get_pa_ramp_up_table_addr
 *  @brief This function retrieves the pa ramp up table pointer
 *  @param rfm_dev - RFM device ID
 *  @param ping_pong_id - double buffer ID for ramp and pre-dist tables
 *  @param slot - which Tx slot number
 *  @return uint16* - pointer to the memory holding the information
 */
uint16* rfgsm_core_dm_get_pa_ramp_up_table_addr( rflm_gsm_subscription_id sub_id,
                                                 rfgsm_core_dm_double_buff ping_pong_id,
                                                 uint8 slot )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx PA up ramp table_ptr NULL for sub %d",
          sub_id);
    return NULL;
  }

  if( ping_pong_id >= RFGSM_CORE_MAX_DOUBLE_BUF )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM get Tx PA up ramp table addr ping pong ID %d invalid", ping_pong_id);
    return NULL;
  }

  if( slot >= RFLM_GSM_MAX_TX_UL_SLOTS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM get Tx PA up ramp table addr slot num %d invalid", slot);
    return NULL;
  }

  return table_ptr->rfgsm_txfe_data.pa_frame_ramps[ping_pong_id].pa_slot_ramps[slot].pa_ramp_up_table;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_get_pa_ramp_down_table_addr
 *  @brief This function retrieves the pa ramp down table pointer
 *  @param rfm_dev      - RFM device ID
 *  @param ping_pong_id - double buffer ID for ramp and pre-dist tables
 *  @param slot         - which Tx slot number
 *  @return uint16*     - pointer to the memory holding the information
 */
uint16* rfgsm_core_dm_get_pa_ramp_down_table_addr( rflm_gsm_subscription_id sub_id,
                                                   rfgsm_core_dm_double_buff ping_pong_id,
                                                   uint8 slot )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx PA down ramp table_ptr NULL for sub %d",
          sub_id);
    return NULL;
  }

  if( ping_pong_id >= RFGSM_CORE_MAX_DOUBLE_BUF )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx PA down table addr ping pong ID %d invalid", 
          ping_pong_id);
    return NULL;
  }

  if( slot >= RFLM_GSM_MAX_TX_UL_SLOTS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get PA ramp down table addr slot %d invalid", 
          slot);
    return NULL;
  }

  return table_ptr->rfgsm_txfe_data.pa_frame_ramps[ping_pong_id].pa_slot_ramps[slot].pa_ramp_down_table;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_get_master_pa_ramp_up_table_addr
 *  @brief This function retrieves the master pa ramp up table pointer
 *         populated from NV
 *  @param sub_id - subscription
 *  @param tx_band - which Tx band table required
 *  @return uint16* - pointer to the memory holding the information
 */
uint16* rfgsm_core_dm_get_master_pa_ramp_up_table_addr( rflm_gsm_subscription_id sub_id,
                                                        rflm_gsm_band_type tx_band )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx master PA up ramp table_ptr NULL for sub %d",
          sub_id);
    return NULL;
  }

  if ( tx_band >= RFLM_GSM_NUM_GSM_BANDS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM get Tx master PA up ramp invalid Tx band %d", tx_band);
    return NULL;
  }

  return table_ptr->rfgsm_txfe_data.pa_master_ramps[tx_band].pa_ramp_up_table;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_get_master_pa_ramp_down_table_addr
 *  @brief This function retrieves the master pa ramp down table pointer
 *         populated from NV
 *  @param sub_id - subscription
 *  @param tx_band - which Tx band table required
 *  @return uint16* - pointer to the memory holding the information
 */
uint16* rfgsm_core_dm_get_master_pa_ramp_down_table_addr( rflm_gsm_subscription_id sub_id,
                                                          rflm_gsm_band_type tx_band )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx master PA down ramp table_ptr NULL for sub %d",
          sub_id);
    return NULL;
  }

  if ( tx_band >= RFLM_GSM_NUM_GSM_BANDS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx master PA down ramp invalid Tx band %d", 
          tx_band);
    return NULL;
  }

  return table_ptr->rfgsm_txfe_data.pa_master_ramps[tx_band].pa_ramp_down_table;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_get_tx_cal_dma_data_addr
 *  @brief This function retrieves the tx cal dma data pointer which stores
 *  the modulator data to be used during calibration
 *  @param rfm_dev - RFM device ID
 *  @return uint64* - pointer to the memory holding the information
 */
uint64* rfgsm_core_get_tx_cal_dma_data_addr( rfm_device_enum_type rfm_dev )
{
  /* Check the rfm device is valid */
  if ( rfm_dev >= RFM_MAX_WAN_DEVICES )
  {
    RFGC_MSG(MSG_LEGACY_ERROR, 
             "RF GSM core DM get dma data rfm_dev invalid");
    return NULL;
  }

  return &(rfgsm_rflm_tx_cal_data_table[rfm_dev]->dma_data[0]);
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_amam_ampm_tables
 *  @brief 
 *  @param sub_id - subscription
 *  @param burst_buffer_id - triple buffer ID
 *  @param ping_pong_id    - double buffer ID for ramp and pre-dist tables
 *  @return boolean - function success status
 */
boolean rfgsm_core_dm_set_amam_ampm_tables( rflm_gsm_subscription_id sub_id,
                                            uint8 burst_buffer_id,
                                            rfgsm_core_dm_double_buff ping_pong_id )
{
  /* Initialise variables */
  rflm_gsm_data_type * table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx amam table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  if( !rfgsm_core_dm_validate_tx_table_params(burst_buffer_id, ping_pong_id) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx amam/ampm tables param error");
    return FALSE;
  }

  /* Set the Tx data */
  table_ptr->rfgsm_burst_event[burst_buffer_id].tx.amam_table = rfgsm_core_dm_get_amam_table_addr( sub_id, ping_pong_id );
  table_ptr->rfgsm_burst_event[burst_buffer_id].tx.ampm_table = rfgsm_core_dm_get_ampm_table_addr( sub_id, ping_pong_id );

  if ( table_ptr->rfgsm_burst_event[burst_buffer_id].tx.amam_table == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx amam/ampm tables amam table NULL");
    return FALSE;
  }

  if ( table_ptr->rfgsm_burst_event[burst_buffer_id].tx.ampm_table == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx amam/ampm tables ampm table NULL");
    return FALSE;
  }

  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_ramp_tables
 *  @brief 
 *  @param sub_id          - subscription
 *  @param burst_buffer_id - triple buffer ID
 *  @param ping_pong_id    - double buffer ID for ramp and pre-dist tables
 *  @param script_type     - which script to 
 *  @param num_tx_slots    - 
 *  @return boolean        - function success status
 */
boolean rfgsm_core_dm_set_ramp_tables( rflm_gsm_subscription_id sub_id,
                                       uint8 burst_buffer_id,
                                       rfgsm_core_dm_double_buff ping_pong_id,
                                       uint8 num_tx_slots )
{
  /* Initialise variables */
  uint32 index;
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1( MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx amam table_ptr NULL for sub %d",
           sub_id );
    return FALSE;
  }

  if( !rfgsm_core_dm_validate_tx_table_params(burst_buffer_id, ping_pong_id) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx ramp tables param error");
    return FALSE;
  }

  if( num_tx_slots > RFLM_GSM_MAX_TX_UL_SLOTS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx ramp tables num tx slots %d invalid", num_tx_slots);
    return FALSE;
  }

  /* Set the Tx data */
  for( index = 0; index < num_tx_slots; index++ )
  {
    table_ptr->rfgsm_burst_event[burst_buffer_id].tx.pa_ramp_up_profile[index] = rfgsm_core_dm_get_pa_ramp_up_table_addr( sub_id, ping_pong_id, index );
    table_ptr->rfgsm_burst_event[burst_buffer_id].tx.pa_ramp_down_profile[index] = rfgsm_core_dm_get_pa_ramp_down_table_addr( sub_id, ping_pong_id, index );
  }

  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_ramp_deltas
 *  @brief 
 *  @param sub_id             - subscription
 *  @param pa_ramp_up_delta   - qs delay/advance to apply to the up ramp start
 *  @param pa_ramp_down_delta - qs delay/advance to apply to the down ramp start
 *  @return boolean           - function success status
 */
boolean rfgsm_core_dm_set_ramp_deltas( rflm_gsm_subscription_id sub_id,
                                       int16 pa_ramp_up_delta,
                                       int16 pa_ramp_down_delta )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx ramp deltas table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  if ( pa_ramp_up_delta > RFGSM_CORE_DM_MAX_RAMP_UP_DELTA ||
       pa_ramp_up_delta < RFGSM_CORE_DM_MIN_RAMP_UP_DELTA )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM Tx ramp up delta is not within limits between %d and %d",
          RFGSM_CORE_DM_MIN_RAMP_UP_DELTA, RFGSM_CORE_DM_MAX_RAMP_UP_DELTA );
    return FALSE;
  }

  if ( pa_ramp_down_delta > RFGSM_CORE_DM_MAX_RAMP_DOWN_DELTA ||
       pa_ramp_down_delta < RFGSM_CORE_DM_MIN_RAMP_DOWN_DELTA )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM Tx ramp down delta is not within limits between %d and %d",
          RFGSM_CORE_DM_MIN_RAMP_DOWN_DELTA, RFGSM_CORE_DM_MAX_RAMP_DOWN_DELTA );
    return FALSE;
  }

  /* Set the Tx data */
  table_ptr->rfgsm_tx_band_event.pa_up_ramp_delta = pa_ramp_up_delta;
  table_ptr->rfgsm_tx_band_event.pa_down_ramp_delta = pa_ramp_down_delta;
  
  return TRUE;
} 



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_tx_alpha
 *  @brief 
 *  @param sub_id          - subscription
 *  @param burst_buffer_id - triple buffer ID
 *  @param tx_alpha        - Tx alpha value based on KV and RFI writes
 *  @return boolean        - function success status
 */
boolean rfgsm_core_dm_set_tx_alpha( rflm_gsm_subscription_id sub_id,
                                    uint16 tx_alpha )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx alpha table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  table_ptr->rflm_gsm_vars.tx_alpha = tx_alpha;
  table_ptr->rflm_gsm_vars.tx_alpha_rflm_overhead = RFLM_GSM_TX_MAX_PROC_OVERHEAD_QS;
  
  return TRUE;
} 

/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_tx_power
 *  @brief 
 *  @param sub_id          - subscription
 *  @param burst_buffer_id - triple buffer ID
 *  @param txPower         - Tx power level in dBm
 *  @return boolean        - function success status
 */
boolean rfgsm_core_dm_set_tx_power( rflm_gsm_subscription_id sub_id,
                                    uint8 burst_buffer_id,
                                    uint8 slot_num,
                                    int32 txPower)
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx power table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }


  /* Check the burst buffer ID is valid */
  if( (burst_buffer_id >= RFLM_GSM_TRIPLE_BUFFER) && 
      (burst_buffer_id != RFLM_GSM_BUFFER_ID_NOT_REQUIRED) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx power burst_buffer_id %d invalid", burst_buffer_id);
    return FALSE;
  }

  /* Validate the slot number */
  if( slot_num >= RFLM_GSM_MAX_TX_UL_SLOTS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx power tx slot num invalid %d", slot_num);
    return FALSE;
  }

  /* Set RFLM DM variable for Tx power */
  /* Tx power needs to be x10 dBm unit for common CL tuner */
  table_ptr->rfgsm_burst_event[burst_buffer_id].tx.tx_pwr_level_dbm10[slot_num] = RFGSM_DM_DBM100_TO_DBM10(txPower);

  /*For debug only*/
  //MSG_3(MSG_SSID_RF, MSG_LEGACY_MED, "RF GSM core DM set Tx power burst_buffer_id %d tx slot num %d  tx power %d x100dBm",burst_buffer_id, slot_num, txPower);
  return TRUE;  
} 


/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_tx_path_delay
 *  @brief 
 *  @param sub_id          - subscription
 *  @param burst_buffer_id - triple buffer ID
 *  @param tx_path_delay   - the delay on the Tx path from WTR and NV offset
 *  @return boolean        - function success status
 */
boolean rfgsm_core_dm_set_tx_path_delay( rflm_gsm_subscription_id sub_id,
                                         uint8 burst_buffer_id,
                                         int64 path_delay_val )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx path delay table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  /* Check the burst buffer ID is valid */
  if( (burst_buffer_id >= RFLM_GSM_TRIPLE_BUFFER) && 
      (burst_buffer_id != RFLM_GSM_BUFFER_ID_NOT_REQUIRED) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx path delay burst_buffer_id %d invalid", burst_buffer_id);
    return FALSE;
  }

  /* Set the Tx data */
  table_ptr->rfgsm_burst_event[burst_buffer_id].tx.path_delay_val = path_delay_val;

  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_kv_data
 *  @brief 
 *  @param sub_id              - subscription
 *  @param burst_buffer_id     - triple buffer ID
 *  @param num_kv_bits         - number of bits in the KV data sequence
 *  @param tx_kv_data_sequence - the KV data sequence
 *  @return boolean            - function success status
 */
boolean rfgsm_core_dm_set_kv_data( rflm_gsm_subscription_id sub_id,
                                   uint16 num_kv_bits,
                                   uint32 tx_kv_data_sequence[10] )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM get Tx amam table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  /* Set the Tx data */
  table_ptr->rflm_gsm_vars.num_tx_kv_cal_bits = num_kv_bits;


  table_ptr->rflm_gsm_vars.tx_kv_cal_data[0] = ((((uint64)*(tx_kv_data_sequence+0)) << 48) |
                                                (((uint64)*(tx_kv_data_sequence+1)) << 32) |
                                                (((uint64)*(tx_kv_data_sequence+2)) << 16) |
                                                (((uint64)*(tx_kv_data_sequence+3)) <<  0));
  table_ptr->rflm_gsm_vars.tx_kv_cal_data[1] = ((((uint64)*(tx_kv_data_sequence+4)) << 48) |
                                                (((uint64)*(tx_kv_data_sequence+5)) << 32) |
                                                (((uint64)*(tx_kv_data_sequence+6)) << 16) | 
                                                (((uint64)*(tx_kv_data_sequence+7)) <<  0));
  table_ptr->rflm_gsm_vars.tx_kv_cal_data[2] = ((((uint64)*(tx_kv_data_sequence+8)) << 48) |
                                                (((uint64)*(tx_kv_data_sequence+9)) << 32));
  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_env_gain_val
 *  @brief 
 *  @param sub_id          - subscription
 *  @param burst_buffer_id - triple buffer ID
 *  @param slot_num        - Tx slot number
 *  @param env_gain        - envelope gain value
 *  @return boolean        - function success status
 */
boolean rfgsm_core_dm_set_env_gain_val( rflm_gsm_subscription_id sub_id,
                                        uint8 burst_buffer_id,
                                        uint8 slot_num,
                                        uint16 env_gain )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx envelope gain table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  /* Check the burst buffer ID is valid */
  if( (burst_buffer_id >= RFLM_GSM_TRIPLE_BUFFER) && 
      (burst_buffer_id != RFLM_GSM_BUFFER_ID_NOT_REQUIRED) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx envelope gain val burst_buffer_id %d invalid", burst_buffer_id);
    return FALSE;
  }

  /* Validate the slot number */
  if( slot_num >= RFLM_GSM_MAX_TX_UL_SLOTS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM set Tx envelope gain val tx slot num invalid %d", slot_num);
    return FALSE;
  }

  /* Set the Tx data */
  table_ptr->rfgsm_burst_event[burst_buffer_id].tx.env_gain[slot_num] = env_gain;

  return TRUE;
}



/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_predist_flag
 *  @brief 
 *  @param sub_id          - subscription
 *  @param burst_buffer_id - triple buffer ID
 *  @param enabled         - predist enabled/disabled flag
 *  @return boolean        - function success status
 */
boolean rfgsm_core_dm_set_predist_flag( rflm_gsm_subscription_id sub_id,
                                        uint8 burst_buffer_id,
                                        uint8 slot_num,
                                        boolean enabled )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx predist flag table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  /* Check the burst buffer ID is valid */
  if( (burst_buffer_id >= RFLM_GSM_TRIPLE_BUFFER) && 
      (burst_buffer_id != RFLM_GSM_BUFFER_ID_NOT_REQUIRED) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx predist flag burst_buffer_id %d invalid", 
          burst_buffer_id);
    return FALSE;
  }

  /* Validate the slot number */
  if( slot_num >= RFLM_GSM_MAX_TX_UL_SLOTS )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx predist flag Tx param error %d", 
          slot_num);
    return FALSE;
  }

  /* Set the predistortion enabled flag */
  table_ptr->rfgsm_burst_event[burst_buffer_id].tx.predist_enabled[slot_num] = enabled;

  return TRUE;
}

/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_set_program_amam_ampm_tables_flag
 *  @brief 
 *  @param sub_id          - subscription
 *  @param burst_buffer_id - triple buffer ID
 *  @param flag            - Flag to indicate whether or not to program amam and ampm tables
 *  @return boolean        - function success status
 */
boolean rfgsm_core_dm_set_program_amam_ampm_tables_flag( rflm_gsm_subscription_id sub_id,
                                                         uint8 burst_buffer_id,
                                                         boolean flag )
{
  /* Initialise variables */
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx amam/ampm tbl flag table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  /* Check the burst buffer ID is valid */
  if( (burst_buffer_id >= RFLM_GSM_TRIPLE_BUFFER) && 
      (burst_buffer_id != RFLM_GSM_BUFFER_ID_NOT_REQUIRED) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Tx amam/ampm tbl flag burst_buffer_id %d invalid", 
          burst_buffer_id);
    return FALSE;
  }

  /* Set the Tx data */
  table_ptr->rfgsm_burst_event[burst_buffer_id].tx.program_amam_ampm_tables = flag;

  return TRUE;
}



/*=============================================================================
 
                            DYNAMIC mDSP INTERFACE
 
=============================================================================*/
/*! @fn rfgsm_core_dm_create_event                                           */
/*! @brief This function gets DM memory addr and execute a generic mDSP event*/
/*!        creation API                                                      */
boolean rfgsm_core_dm_create_event( rfgsm_core_data_type *core_data,
                                    void **event_handle,
                                    rflm_gsm_data_type *table_ptr,
                                    rfgsm_core_dm_script_type script_type,
                                    uint8 burst_num,
                                    rfgsm_core_dm_task_q task_q_id )
{
  void* event_mem_ptr = NULL;
  rflm_gsm_rx_data *rx_data_ptr = NULL;
  rfgsm_core_dm_script_type rx_script_type = RFLM_GSM_DEFAULT_SCRIPT;

  if (script_type == RFLM_GSM_RX_FRONTEND_CONFIG_SCRIPT)
  { 
    if (core_data->rfgsm_core_handle_ptr)
    {
      switch(core_data->rfgsm_core_handle_ptr->rx_burst_type)
      {
        case RFGSM_RX_DEFAULT_RX_BURST:
        case RFGSM_RX_SYNC_BURST:
          rx_script_type = RFLM_GSM_RX_START_SCRIPT;
          break;

        case RFGSM_RX_SCH_OR_ACQ_BURST:
          rx_script_type = RFLM_GSM_RX_ACQ_START_SCRIPT;
          break;

        case RFGSM_RX_PCH_BURST:
          rx_script_type = RFLM_GSM_RX_PCH_START_SCRIPT;
          break;

        case RFGSM_RX_MON_BURST:
          rx_script_type = RFLM_GSM_MON_START_SCRIPT;
          break;

        default:        
          MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM Rx invalid burst type");
          return FALSE;          
      }      

      rx_data_ptr = rfgsm_core_dm_get_rx_ptr( table_ptr, core_data->triple_buffer_id, rx_script_type, burst_num );  

      /* Check that the Rx rflm data pointer is not NULL */
      if( rx_data_ptr == NULL )
      {
        MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, 
             "rx_data_ptr is NULL burst_buffer_id %d rflm rx start script %d burst_num %d",
             core_data->triple_buffer_id, rflm_gsm_rx_start_script_map[core_data->rfgsm_core_handle_ptr->rx_burst_type], burst_num);
        return FALSE;
      }

      if( task_q_id == RFLM_GSM_CCS_TASK_Q_A )
       {
         event_mem_ptr = (void*)&rx_data_ptr->rx_frontend_config.task_q_a;
       }
       else if( task_q_id == RFLM_GSM_CCS_TASK_Q_B )
       {
         event_mem_ptr = (void*)&rx_data_ptr->rx_frontend_config.task_q_b;
       }
    }
    else 
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM rfgsm_core_handle_ptr uninitalised");
      return FALSE;   
    }
  }
  else
  {
    event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 
                                                             core_data->triple_buffer_id, 
                                                             script_type, 
                                                             burst_num, 
                                                             task_q_id );
  }
  if( event_mem_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM RFLM DM memory pointer NULL");
    return FALSE;
  }

  /* Create the event and get the event info ID for later use */
  if( !rfgsm_mdsp_create_buffer( core_data,
                                 event_handle,
                                 event_mem_ptr,
                                 script_type,
                                 task_q_id ) )
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM create CCS buffer failed for script %d, TQ %d, smem location 0x%x!",
          script_type, task_q_id, event_mem_ptr);
    return FALSE;
  }

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_push_script                                            */
/*! @brief This function pushes SSBIs, or RFFEs, or GRFCs to the event with  */
/*!        idx event_info_idx from the RF script buffer pointed to by script */
boolean rfgsm_core_dm_push_script( void* event_handle,
                                   rf_hal_bus_resource_type bus_resource,
                                   rf_buffer_intf* script,
                                   uint8 script_type,
                                   uint8 task_q_id,
								   boolean lower_rx_ccs_prio,
								   boolean ssbi_protocol_supported)
{
  if( !rfgsm_mdsp_push_script_to_buffer( event_handle,
                                         bus_resource,
                                         script,
                                         script_type,
                                         task_q_id,
										 lower_rx_ccs_prio,
										 ssbi_protocol_supported ) )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM failed to push transactions for script %d, TQ %d", 
          script_type, task_q_id);
    return FALSE;
  }

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_commit_event                                           */
/*! @brief This function commits the event at index event_info_idx ready for */
/*!        programming to CCS                                                */
boolean rfgsm_core_dm_commit_event( void* event_handle )
{
  if( !rfgsm_mdsp_commit_buffer( event_handle ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM failed to commit event");
    return FALSE;
  }

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_cleanup_event                                          */
/*! @brief This function cleans the event at index event_info_idx freeing the*/
/*!        RF CCS HAL memory                                                 */
void rfgsm_core_dm_cleanup_event( void** event_handle )
{
  rfgsm_mdsp_cleanup_buffer( event_handle );
}

/*---------------------------------------------------------------------------*/
/*! @fn rfgsm_core_dm_store_event
 *  @brief This function maintains the event handle related parameters required 
 *         for the therm read event. Event handle tag is used to tally the therm
 *         read request with the CCS response.
 *         The event handle is used to fetch the RFFE read data.
 *  @param rfm_dev         - RFM device ID
 *  @param event handle    - Handle to therm read event 
 */
static void rfgsm_core_dm_store_event( rfgsm_core_handle_type *rfgsm_core_handle_ptr, void* event_handle )
{
  rfgsm_mdsp_therm_read_event_store( rfgsm_core_handle_ptr, event_handle );
}

/*===========================================================================*/
/*! @fn rfgsm_core_dm_dynamic_event_wrapper                                  */
/*! @brief This function acts as a wrapper for the dynamic event method to be*/
/*!        backwards compatible with the old event building method           */
static boolean rfgsm_core_dm_dynamic_event_wrapper( rfgsm_core_data_type* core_data,
                                                    rfgsm_core_dm_script_type script_type,
                                                    rflm_gsm_ccs_task_q task_q,
                                                    rf_buffer_intf** ccs_scripts,
                                                    size_t num_scripts,
                                                    void** event_handle_ptr,
                                                    uint8 burst_num )
{
  /* Initialise variables */
  void *event_handle = NULL;
  boolean reuse_event_handle;
  boolean status = TRUE;
  rflm_gsm_data_type *table_ptr = NULL;
  uint8 index = 0;
  void* event_mem_ptr = NULL;

  /* If script buffer pointer is NULL, nothing to process - return from this function */
  if ( NULL == *ccs_scripts )
  {
    return status;
  }

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM dynamic event table_ptr NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }

  if( event_handle_ptr != NULL )
  {
    reuse_event_handle = TRUE;    
  }
  else
  {
    reuse_event_handle = FALSE;
    event_handle_ptr = &event_handle;
  }

  if( !rfgsm_core_dm_create_event( core_data, event_handle_ptr, table_ptr, script_type, burst_num, task_q) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM dynamic event wrapper create event failed");
    return FALSE;
  }

   /*Loop through the each device scripts to be pushed onto CCS */
  for (index = 0; index < num_scripts ; index++) 
  {
    if (NULL != ccs_scripts[index])
    {
      if ( TRUE == core_data->rfgsm_core_handle_ptr->ssbi_protocol_supported )
      {
        if( !rfgsm_core_dm_push_script( *event_handle_ptr , RF_HAL_BUS_RESOURCE_SSBI, ccs_scripts[index], script_type, task_q,core_data->gl1_lower_rx_ccs_prio, core_data->rfgsm_core_handle_ptr->ssbi_protocol_supported) )
        { 
          MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM dynamic event wrapper push failed");
          status &= FALSE;
        }
      }

      if( !rfgsm_core_dm_push_script( *event_handle_ptr , RF_HAL_BUS_RESOURCE_RFFE, ccs_scripts[index], script_type, task_q, core_data->gl1_lower_rx_ccs_prio, core_data->rfgsm_core_handle_ptr->ssbi_protocol_supported) )
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM dynamic event wrapper push failed");
        status &= FALSE;
      }

      if( !rfgsm_core_dm_push_script( *event_handle_ptr , RF_HAL_BUS_RESOURCE_GRFC, ccs_scripts[index], script_type, task_q, core_data->gl1_lower_rx_ccs_prio, core_data->rfgsm_core_handle_ptr->ssbi_protocol_supported) )
      {
        MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM dynamic event wrapper push failed");
        status &= FALSE;
      }
    }
  }

  if( status )
  {
    if( !rfgsm_core_dm_commit_event(*event_handle_ptr) )
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM dynamic event wrapper commit failed");
      status &= FALSE;
    }
  }

  if(status)
  {
    if( !rfgsm_core_dm_device_status_event_wrapper(core_data,
		                                      script_type,
		                                      task_q,
		                                      burst_num,
		                                      table_ptr) )
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_core_dm_device_status_event_wrapper failed !!!");
      status &= FALSE;
    }
  }

  if (reuse_event_handle != TRUE)
  {
    /* Cleanup */
    rfgsm_core_dm_cleanup_event(event_handle_ptr);
  }

  return status;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_dynamic_enter_event_wrapper                            */
/*! @brief This function acts as a wrapper for the dynamic event method to be*/
/*!        backwards compatible with the old event building method           */
boolean rfgsm_core_dm_dynamic_enter_event_wrapper( rfgsm_core_data_type *core_data )
{
#ifndef FEATURE_TX_SHARING
  rf_buffer_intf* enter_mode_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.enter_mode_script,
                                                      (core_data->prx_drx_combined_mode)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.enter_mode_script:NULL,
                                                       NULL};
  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_ENTER_MODE_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)enter_mode_ccs_scripts_taskq_a,
                                            sizeof(enter_mode_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)enter_mode_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Enter mode RFLM dynamic event prog failed!");
    return FALSE;
  }

#else
  rf_buffer_intf* rx_enter_mode_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_enter_mode_script,
                                                         (core_data->prx_drx_combined_mode)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.rx_enter_mode_script:NULL,
                                                          NULL};
  rf_buffer_intf* tx_enter_mode_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_enter_mode_script,
                                                          NULL};

  /* Program the RX event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_RX_ENTER_MODE_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)rx_enter_mode_ccs_scripts_taskq_a,
                                            sizeof(rx_enter_mode_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)rx_enter_mode_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM RX Enter mode RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the TX event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_ENTER_MODE_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)tx_enter_mode_ccs_scripts_taskq_a,
                                            sizeof(tx_enter_mode_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)tx_enter_mode_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM RX Enter mode RFLM dynamic event prog failed!");
    return FALSE;
  }
#endif

  return TRUE;
}

/*===========================================================================*/
/* IP2 cal setup*/

boolean rfgsm_core_dm_dynamic_ip2_cal_setup_event_wrapper( rfgsm_core_data_type *core_data_ptr )
{
  rf_buffer_intf* ip2_cal_setup_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data_ptr->rfgsm_core_handle_ptr->scripts.ip2_cal_setup_script,NULL};

  /* Validate scripts ptr */
  if( core_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_dynamic_ip2_cal_setup_event_wrapper(), core data ptr is NULL!");
    return FALSE;
  }
  


  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data_ptr,
                                            RFLM_GSM_IP2_CAL_SETUP_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)ip2_cal_setup_ccs_scripts_taskq_a,
                                            sizeof(ip2_cal_setup_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)ip2_cal_setup_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM IP2 cal setup RFLM dynamic event prog failed!");
    return FALSE;
  }

  return TRUE;

}

/* IP2 cal step*/

boolean rfgsm_core_dm_dynamic_ip2_cal_step_event_wrapper( rfgsm_core_data_type *core_data_ptr  )
{
  uint8 index = 0;
  rf_buffer_intf* ip2_cal_step_ccs_scripts_taskq_a[] = {NULL,NULL}; 
  uint8 num_cal_points = 0;
   /* Validate scripts ptr */
  if( core_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_dynamic_ip2_cal_step_event_wrapper(), core data ptr is NULL!");
    return FALSE;
  }
  num_cal_points = core_data_ptr->rfgsm_core_handle_ptr->ip2_cal_params.alg_params.num_cal_points;
  if ( num_cal_points >  RFGSM_DEVICE_MAX_IP2_CAL_STEPS)
  {

    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_dynamic_ip2_cal_step_event_wrapper(), number of cal steps:%d greater than limit :%d",num_cal_points,RFGSM_DEVICE_MAX_IP2_CAL_STEPS);
    return FALSE;
  }

  


  for (index = 0; index < num_cal_points; index++)
  {
    ip2_cal_step_ccs_scripts_taskq_a[0] = (rf_buffer_intf*)core_data_ptr->rfgsm_core_handle_ptr->scripts.ip2_cal_step_script[index];

      /* Program the event to data table */
    if( !rfgsm_core_dm_dynamic_event_wrapper( core_data_ptr,
                                              RFLM_GSM_IP2_CAL_STEP_SCRIPT,
                                              RFLM_GSM_CCS_TASK_Q_A,
                                              (rf_buffer_intf**)ip2_cal_step_ccs_scripts_taskq_a,
                                              sizeof(ip2_cal_step_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)ip2_cal_step_ccs_scripts_taskq_a),
                                              NULL,
                                              index ) )
    {
      MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM IP2 cal step RFLM dynamic event prog failed!, step:%d",index);
      return FALSE;
    }
  }

  return TRUE;

}



boolean rfgsm_core_dm_dynamic_ip2_cal_cleanup_event_wrapper( rfgsm_core_data_type *core_data_ptr)
{


  rf_buffer_intf* ip2_cal_cleanup_ccs_scripts_taskq_a[] = {(rf_buffer_intf*)core_data_ptr->rfgsm_core_handle_ptr->scripts.ip2_cal_cleanup_script,NULL}; 
  /* Validate scripts ptr */
  if( core_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_dynamic_ip2_cal_cleanup_event_wrapper(), core data ptr is NULL!");
    return FALSE;
  }



  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data_ptr,
                                            RFLM_GSM_IP2_CAL_CLEANUP_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)ip2_cal_cleanup_ccs_scripts_taskq_a,
                                            sizeof(ip2_cal_cleanup_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)ip2_cal_cleanup_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM IP2 cal cleanup RFLM dynamic event prog failed!");
    return FALSE;
  }


  return TRUE;

}

/*===========================================================================*/
/* Jammer Detector Read setup*/

boolean rfgsm_core_dm_dynamic_jdet_read_event_wrapper( rfgsm_core_data_type *core_data_ptr, uint8 burst_num, rflm_gsm_script_type jdet_script_type )
{
  rf_buffer_intf* jdet_read_setup_ccs_scripts_taskq_a[] = {NULL,NULL};

  /* Validate scripts ptr */
  if( core_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_dynamic_jdet_read_event_wrapper(), core data ptr is NULL!");
    return FALSE;
  }

  if ( jdet_script_type == RFLM_GSM_MON_JDET_READ_SCRIPT )
  {
    /* Assuming re-tune for each monitor, jdet read will always be stored in index 0 */
    jdet_read_setup_ccs_scripts_taskq_a[0] = (rf_buffer_intf *)core_data_ptr->rfgsm_core_handle_ptr->scripts.jdet_read_script[0]; 
  }
  else if ( jdet_script_type == RFLM_GSM_RX_JDET_READ_SCRIPT )
  {
    jdet_read_setup_ccs_scripts_taskq_a[0] = (rf_buffer_intf *)core_data_ptr->rfgsm_core_handle_ptr->scripts.jdet_read_script[burst_num];
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_dynamic_jdet_read_event_wrapper(), invalid jdet script type: %d!", jdet_script_type );
    return FALSE;
  }

  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data_ptr,
                                            jdet_script_type,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)jdet_read_setup_ccs_scripts_taskq_a,
                                            sizeof(jdet_read_setup_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)jdet_read_setup_ccs_scripts_taskq_a),
                                            NULL,
                                            burst_num ) )
  {
    MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM JDET read setup RFLM dynamic event prog failed for index %d, type: %d!", burst_num, jdet_script_type );
    return FALSE;
  }

  return TRUE;
}
/*===========================================================================*/
/*! @fn rfgsm_core_dm_dynamic_wakeup_event_wrapper                           */
/*! @brief This function acts as a wrapper for the dynamic event method to be*/
/*!        backwards compatible with the old event building method           */
boolean rfgsm_core_dm_dynamic_wakeup_event_wrapper( rfgsm_core_data_type *core_data )
{

#ifndef FEATURE_TX_SHARING
  rf_buffer_intf* enter_mode_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.enter_mode_script,
                                                     (core_data->prx_drx_combined_mode)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.enter_mode_script:NULL,
                                                       NULL};

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_WAKEUP_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)enter_mode_ccs_scripts_taskq_a,
                                            sizeof(enter_mode_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)enter_mode_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Wakeup RFLM dynamic event prog failed!");
    return FALSE;
  }
#else
  rf_buffer_intf* rx_enter_mode_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_enter_mode_script,
                                                         (core_data->prx_drx_combined_mode)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.rx_enter_mode_script:NULL,
                                                          NULL};
  rf_buffer_intf* tx_enter_mode_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_enter_mode_script,
                                                          NULL};


  /* Program the RX event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_RX_WAKEUP_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)rx_enter_mode_ccs_scripts_taskq_a,
                                            sizeof(rx_enter_mode_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)rx_enter_mode_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM RX Wakeup RFLM dynamic event prog failed!");
    return FALSE;
}


  /* Program the TX event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_WAKEUP_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)tx_enter_mode_ccs_scripts_taskq_a,
                                            sizeof(tx_enter_mode_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)tx_enter_mode_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM TX Wakeup RFLM dynamic event prog failed!");
    return FALSE;
  }
#endif

  rfgsm_mdsp_dump_ccs_error_events_ext( core_data->sub_id );

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_dynamic_tx_band_event_wrapper                          */
/*! @brief This function acts as a wrapper for the dynamic event method to be*/
/*!        backwards compatible with the old event building method           */
boolean rfgsm_core_dm_dynamic_tx_band_event_wrapper( rfgsm_core_data_type *core_data )
{
  rf_buffer_intf* tx_band_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_band_script,
                                                    NULL};
  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_BAND_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)tx_band_ccs_scripts_taskq_a,
                                            sizeof(tx_band_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)tx_band_ccs_scripts_taskq_a),
                                            NULL,
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Band RFLM dynamic event prog failed!");
    return FALSE;
  }

  return TRUE;
}

#ifdef FEATURE_RF_ASDIV
/*===========================================================================*/
/*! @fn rfgsm_core_dm_rx_start_event_hdr_asd_processing                      */
/*! @brief This function updates the RX Start Event (TQB) header to include  */
/*!        necessary information for the response callback function          */

static boolean rfgsm_core_dm_rx_start_event_hdr_asd_processing( rfgsm_core_data_type *core_data,
                                                    rfgsm_core_dm_script_type script_type,
                                                    rflm_gsm_ccs_task_q task_q,                                                    
                                                    uint8 burst_num )
{
  /* Initialise Variables */
  rflm_gsm_data_type *table_ptr = NULL;
  void* event_mem_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM dynamic RX event table_ptr NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }

  /* Retrieve DM memory pointer for RX start event */
  if ((event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 
                                                                core_data->triple_buffer_id, 
                                                                script_type, 
                                                                burst_num, 
                                                                task_q )) == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM CORE RX Event Wrapper RFLM DM memory pointer NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }

  /* Populate the event header with appropriate ASD information*/
  if( !rfgsm_core_dm_populate_asd_event_hdr( core_data, event_mem_ptr ) )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM CORE RX Event Wrapper RFLM DM memory pointer NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }

  return TRUE;
}
#endif /* FEATURE_RF_ASDIV */

/*===========================================================================*/
/*! @fn rfgsm_core_dm_dynamic_rx_event_wrapper                               */
/*! @brief This function acts as a wrapper for the dynamic event method to be*/
/*!        backwards compatible with the old event building method           */
boolean rfgsm_core_dm_dynamic_rx_event_wrapper( rfgsm_core_data_type *core_data,
                                                rfgsm_rx_burst_type rx_burst_type,
                                                uint8 rx_burst_num )
{
  rfgsm_mdsp_ccs_handles_s* ccs_handles = rfgsm_mdsp_ccs_handles[core_data->rfm_dev];
  /*Initialise rx scripts for each CCS task*/ 
  rf_buffer_intf* rx_frontend_config_ccs_scripts_taskq_a[] = {NULL};
  /* enable tuner AOL support during RX burst*/
  rf_buffer_intf* rx_frontend_config_ccs_scripts_taskq_b[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_start_script[RX_ATUNER_SCRIPT],
                                                              (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_start_script[RX_XSW_SCRIPT],
                                                              (core_data->enable_rx_diversity)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.rx_start_script[RX_ATUNER_SCRIPT]:NULL,
                                                               NULL};
  rf_buffer_intf* rx_start_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_start_script[RX_WTR_SCRIPT],
												     NULL};

  rf_buffer_intf* rx_start_ccs_scripts_taskq_b[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_start_script[RX_ASM_SCRIPT],
                                                    (core_data->enable_rx_diversity)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.rx_start_script[RX_ASM_SCRIPT]:NULL,
                                                    (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_start_script[RX_GRFC_SCRIPT],
                                                    (core_data->enable_rx_diversity)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.rx_start_script[RX_GRFC_SCRIPT]:NULL,
                                                     NULL};

  rf_buffer_intf* rx_stop_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_WTR_SCRIPT],
                                                    NULL};
  /* @TODO ASD needed tuner stop script? but is not currently populated during rx burst start, removed for now*/
  rf_buffer_intf* rx_stop_ccs_scripts_taskq_b[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_ASM_SCRIPT],
                                                   (core_data->enable_rx_diversity)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.rx_stop_script[RX_ASM_SCRIPT]:NULL,
                                                   (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_GRFC_SCRIPT],
                                                   (core_data->enable_rx_diversity)?(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr_rxd->scripts.rx_stop_script[RX_GRFC_SCRIPT]:NULL,
                                                   (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_XSW_SCRIPT],
                                                   (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.rx_stop_script[RX_ATUNER_SCRIPT],
                                                    NULL};

  /* Validate the Rx burst type */
  if ( rx_burst_type >= RFGSM_MAX_RX_BURST_TYPES || 
	   rx_burst_type <= RFGSM_RX_DEFAULT_RX_BURST )
	 {
	 MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM Rx burst type %d invalid", rx_burst_type);
	 return FALSE;
	 }



  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            rflm_gsm_rx_start_script_map[rx_burst_type],
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)rx_start_ccs_scripts_taskq_a,
                                            sizeof(rx_start_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)rx_start_ccs_scripts_taskq_a),
                                            (void*)&ccs_handles->rx_event[START_TASKQA],
                                            rx_burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Rx Start RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            rflm_gsm_rx_start_script_map[rx_burst_type],
                                            RFLM_GSM_CCS_TASK_Q_B,
                                            (rf_buffer_intf**)rx_start_ccs_scripts_taskq_b,
                                            sizeof(rx_start_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)rx_start_ccs_scripts_taskq_b),
                                            (void *)&ccs_handles->rx_event[START_TASKQB],
                                            rx_burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Rx Start RFLM dynamic event prog failed!");
    return FALSE;
  }
  
  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_RX_FRONTEND_CONFIG_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)rx_frontend_config_ccs_scripts_taskq_a,
                                            sizeof(rx_frontend_config_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)rx_frontend_config_ccs_scripts_taskq_a),
                                            (void *)ccs_handles->rx_event[FE_TASKQA],
                                            rx_burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Rx Frontend Config RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_RX_FRONTEND_CONFIG_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_B,
                                            (rf_buffer_intf**)rx_frontend_config_ccs_scripts_taskq_b,
                                            sizeof(rx_frontend_config_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)rx_frontend_config_ccs_scripts_taskq_b),
                                            (void *)ccs_handles->rx_event[FE_TASKQB],
                                            rx_burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Rx Frontend Config RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            rflm_gsm_rx_stop_script_map[rx_burst_type],
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)rx_stop_ccs_scripts_taskq_a,
                                            sizeof(rx_stop_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)rx_stop_ccs_scripts_taskq_a),
                                            (void *)&ccs_handles->rx_event[STOP_TASKQA],
                                            rx_burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Rx Stop RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            rflm_gsm_rx_stop_script_map[rx_burst_type],
                                            RFLM_GSM_CCS_TASK_Q_B,
                                            (rf_buffer_intf**)rx_stop_ccs_scripts_taskq_b,
                                            sizeof(rx_stop_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)rx_stop_ccs_scripts_taskq_b),
                                            (void *)&ccs_handles->rx_event[STOP_TASKQB],
                                            rx_burst_num ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Rx Stop RFLM dynamic event prog failed!");
    return FALSE;
  }

#ifdef FEATURE_RF_ASDIV
  /* We have got this far => "RX Start" event has programmed correctly */
  /* Therefore if ASDIV enabled and GL1 has requested a switch, populate the event header with ASD response information */
  if ( IS_ASDIV_SET( core_data ) )
  {
      if ( !rfgsm_core_dm_rx_start_event_hdr_asd_processing( core_data,
                                                rflm_gsm_rx_start_script_map[rx_burst_type],
                                                RFLM_GSM_CCS_TASK_Q_B,
                                                rx_burst_num ) )
      {
          MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Rx Start Event Header ASD processing  failed!");
          return FALSE;
      }

      /*ASD response is only for up coming rx burst and not for all Rx burst in a frame */
      core_data->rfgsm_core_handle_ptr->asd_rsp = FALSE;
  }
#endif

  rfgsm_mdsp_dump_ccs_error_events_ext( core_data->sub_id );

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_dynamic_tx_event_wrapper                               */
/*! @brief This function acts as a wrapper for the dynamic event method to be*/
/*!        backwards compatible with the old event building method           */
boolean rfgsm_core_dm_dynamic_tx_event_wrapper( rfgsm_core_data_type *core_data,
                                                uint8 num_tx_slots )
{
  uint32 index = 0;
  rfgsm_mdsp_ccs_handles_s* ccs_handles = rfgsm_mdsp_ccs_handles[core_data->rfm_dev];

  rf_buffer_intf *tx_stop_apt_script_buf = NULL;

  if (( (RF_HW_WTR2965_NON_CA_4373_V2_SAW == (rf_hw_type)rfcommon_nv_get_rf_config())) ||
             (RF_HW_WTR2965_NON_CA_4373_V2 == (rf_hw_type)rfcommon_nv_get_rf_config()) )
  {
    tx_stop_apt_script_buf = core_data->rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_APT_SCRIPT];
  }

   /*Initialise tx scripts for each CCS taskQ*/
  rf_buffer_intf* tx_frontend_config_ccs_scripts_taskq_a[] = {NULL};
  /* enable tuner AOL support during TX burst*/
  rf_buffer_intf* tx_frontend_config_ccs_scripts_taskq_b[] = { (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_start_script[TX_ATUNER_SCRIPT],
                                                              NULL};

  rf_buffer_intf* tx_start_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_start_script[TX_WTR_SCRIPT],
                                                    (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_start_script[TX_APT_SCRIPT],
                                                     NULL};

  rf_buffer_intf* tx_start_ccs_scripts_taskq_b[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_start_script[TX_ASM_SCRIPT],
                                                    (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_start_script[TX_PA_SCRIPT],
                                                    (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_start_script[TX_GRFC_SCRIPT],
                                                    NULL};

  rf_buffer_intf* tx_stop_ccs_scripts_taskq_a[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_WTR_SCRIPT],
                                                   tx_stop_apt_script_buf,
                                                    NULL};

  rf_buffer_intf* tx_stop_ccs_scripts_taskq_b[] = {(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_ASM_SCRIPT],
                                                   (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_PA_SCRIPT],
                                                   (rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_stop_script[TX_GRFC_SCRIPT],
                                                   NULL};

  rf_buffer_intf* tx_transition_ccs_scripts_taskq_a[] = {NULL,NULL}; 
  rf_buffer_intf* tx_transition_ccs_scripts_taskq_b[] = {NULL,NULL,NULL}; 


  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_FRONTEND_CONFIG_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)tx_frontend_config_ccs_scripts_taskq_a,
                                            sizeof(tx_frontend_config_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)tx_frontend_config_ccs_scripts_taskq_a),
                                            (void *)&ccs_handles->tx_event[FE_TASKQA],
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Frontend Config RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_FRONTEND_CONFIG_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_B,
                                            (rf_buffer_intf**)tx_frontend_config_ccs_scripts_taskq_b,
                                            sizeof(tx_frontend_config_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)tx_frontend_config_ccs_scripts_taskq_b),
                                            (void *)&ccs_handles->tx_event[FE_TASKQB],
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Frontend Config RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_START_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)tx_start_ccs_scripts_taskq_a,
                                            sizeof(tx_start_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)tx_start_ccs_scripts_taskq_a),
                                            (void *)&ccs_handles->tx_event[START_TASKQA],
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Start RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_START_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_B,
                                            (rf_buffer_intf**)tx_start_ccs_scripts_taskq_b,
                                            sizeof(tx_start_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)tx_start_ccs_scripts_taskq_b),
                                            (void *)&ccs_handles->tx_event[START_TASKQB],
                                            0 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Start RFLM dynamic event prog failed!");
    return FALSE;
  }

  for( index = 0; index < num_tx_slots; index++ )
  {
    tx_transition_ccs_scripts_taskq_a[0]=(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_WTR_SCRIPT][index];
    tx_transition_ccs_scripts_taskq_b[0]=(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.tx_trans_script[TX_TRANS_FE_SCRIPT][index];
    /* Use apt_script only for interslot in multislot case. For the very first Tx slot or single slot Tx case, above tx_start_script[TX_APT_SCRIPT] is used */    
    if (index > 0)
    {
      tx_transition_ccs_scripts_taskq_b[1]=(rf_buffer_intf *)core_data->rfgsm_core_handle_ptr->scripts.apt_script[index];
    }

    /* Program the event to data table */
    if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                              RFLM_GSM_TX_TRANS_SCRIPT,
                                              RFLM_GSM_CCS_TASK_Q_A,
                                              (rf_buffer_intf**)tx_transition_ccs_scripts_taskq_a,
                                              sizeof(tx_transition_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)tx_transition_ccs_scripts_taskq_a),
                                              (void *)&ccs_handles->tx_trans_event[TRANS_TASKQA][index],
                                              index ) )
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Trans RFLM dynamic event prog failed!");
      return FALSE;
    }

    /* Program the event to data table */
    if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                              RFLM_GSM_TX_TRANS_SCRIPT,
                                              RFLM_GSM_CCS_TASK_Q_B,
                                              (rf_buffer_intf**)tx_transition_ccs_scripts_taskq_b,
                                              sizeof(tx_transition_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)tx_transition_ccs_scripts_taskq_b),
                                              (void *)&ccs_handles->tx_trans_event[TRANS_TASKQB][index],
                                              index ) )
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Trans RFLM dynamic event prog failed!");
      return FALSE;
    }
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_STOP_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_A,
                                            (rf_buffer_intf**)tx_stop_ccs_scripts_taskq_a,
                                            sizeof(tx_stop_ccs_scripts_taskq_a)/sizeof((rf_buffer_intf**)tx_stop_ccs_scripts_taskq_a),
                                            (void *)&ccs_handles->tx_event[STOP_TASKQA],
                                            num_tx_slots-1 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Stop RFLM dynamic event prog failed!");
    return FALSE;
  }

  /* Program the event to data table */
  if( !rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                            RFLM_GSM_TX_STOP_SCRIPT,
                                            RFLM_GSM_CCS_TASK_Q_B,
                                            (rf_buffer_intf**)tx_stop_ccs_scripts_taskq_b,
                                            sizeof(tx_stop_ccs_scripts_taskq_b)/sizeof((rf_buffer_intf**)tx_stop_ccs_scripts_taskq_b),
                                            (void *)&ccs_handles->tx_event[STOP_TASKQB],
                                            num_tx_slots-1 ) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM Tx Stop RFLM dynamic event prog failed!");
    return FALSE;
  }

  rfgsm_mdsp_dump_ccs_error_events_ext( core_data->sub_id );

  return TRUE;
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_dynamic_therm_event_wrapper                            */
/*! @brief This function acts as a wrapper for the dynamic event method to be*/
/*!        backwards compatible with the old event building method           */
boolean rfgsm_core_dm_dynamic_therm_event_wrapper( rfgsm_core_data_type *core_data )
{
  /* Initialise Variables */
  void *event_handle = NULL;
  rflm_gsm_data_type *table_ptr = NULL;
  rf_buffer_intf* script = NULL;
  boolean status = TRUE;
  void* event_mem_ptr = NULL;
  
  script = (rf_buffer_intf*)core_data->rfgsm_core_handle_ptr->scripts.therm_read_script;
  RF_NULL_CHECK_RTN(script, "RF GSM core DM Therm event wrapper therm_read_script is NULL!", FALSE);

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM dynamic therm event table_ptr NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }

  /* Retrieve DM memory pointer for therm read event */
  if ((event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 0, RFLM_GSM_THERM_READ_SCRIPT, 0, RFLM_GSM_CCS_TASK_Q_A )) == NULL)
  {
    RF_MSG(MSG_LEGACY_ERROR, "RF GSM THERM READ RFLM DM memory pointer NULL");
    return FALSE;
  }

  if ( !rfgsm_core_dm_validate_therm_read_event_hdr( event_mem_ptr ) )
  {
    RF_MSG(MSG_LEGACY_ERROR, "THERM READ Event header validation failed");
    return FALSE;
  }

  /* Create the event */
  if( !rfgsm_core_dm_create_event( core_data, &event_handle, table_ptr, RFLM_GSM_THERM_READ_SCRIPT, 0, RFLM_GSM_CCS_TASK_Q_A) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM therm event create failed");
    return FALSE;
  }

  /* Push any SSBIs from the device script to the event */
  if( !rfgsm_core_dm_push_script(event_handle, RF_HAL_BUS_RESOURCE_SSBI, script, RFLM_GSM_THERM_READ_SCRIPT, RFLM_GSM_CCS_TASK_Q_A, FALSE, FALSE) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM therm event push SSBI failed");
    status &= FALSE;
  }

  /* Push any RFFEs from the device script to the event */
  if( !rfgsm_core_dm_push_script(event_handle, RF_HAL_BUS_RESOURCE_RFFE, script, RFLM_GSM_THERM_READ_SCRIPT, RFLM_GSM_CCS_TASK_Q_A, FALSE, FALSE) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM therm event push RFFE failed");
    status &= FALSE;
  }

  /* Push any GRFCs from the device script to the event */
  if( !rfgsm_core_dm_push_script(event_handle, RF_HAL_BUS_RESOURCE_GRFC, script, RFLM_GSM_THERM_READ_SCRIPT, RFLM_GSM_CCS_TASK_Q_A, FALSE, FALSE) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM therm event push GRFC failed");
    status &= FALSE;
  }

  if( status )
  {
    if( !rfgsm_core_dm_commit_event(event_handle) )
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM therm event commit failed");
      status &= FALSE;
    }
  }

  if ( status ) 
  {
    /* Don't cleanup the event_handle if Therm Read is scheduled */
    /* Instead store it away for later use (to fetch the RFFE read data) */
    rfgsm_core_dm_store_event(core_data->rfgsm_core_handle_ptr, event_handle);

    if ( !rfgsm_core_dm_populate_therm_read_event_hdr(core_data, event_mem_ptr))
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM therm populate event hdr failed");
      status &= FALSE;
    }
  }
  else
  {
    rfgsm_core_dm_cleanup_event(&event_handle);
  }

  return status;
}

boolean rfgsm_core_dm_device_status_event_wrapper( rfgsm_core_data_type* core_data,
                                                    rfgsm_core_dm_script_type script_type,
                                                    rflm_gsm_ccs_task_q task_q,
                                                    uint8 burst_num,
                                                    rflm_gsm_data_type *table_ptr)
{

  void* event_mem_ptr = NULL;
  boolean status = TRUE;
  
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM dynamic event table_ptr NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }
  
  //Check if the Device Status/GRFC Status is enabled or Device Status Response UMID has been populated correctly 
  //or not in rfgsm_core_handles.
  if((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable_grfc_read == TRUE) ||
     (core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable_grfc_read == TRUE) ||
     (((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable == TRUE) ||
      (core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable == TRUE)) &&
      (core_data->rfgsm_core_handle_ptr->device_config_to_read.device_status_rsp_umid != 0)))
  {
   switch(script_type)
   {
     case RFLM_GSM_RX_START_SCRIPT:
     case RFLM_GSM_MON_START_SCRIPT:
     case RFLM_GSM_RX_ACQ_START_SCRIPT:
     case RFLM_GSM_RX_PCH_START_SCRIPT:
        if((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable_grfc_read == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_B))
        {
          event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 
                                                                 core_data->triple_buffer_id, 
                                                                 script_type, 
                                                                 burst_num, 
                                                                 task_q );
  
          if(event_mem_ptr!=NULL)
	  {
            if(!rfgsm_core_dm_populate_device_status_read_event_hdr(core_data, event_mem_ptr, script_type,task_q, burst_num))
    	    {
    	      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM GRFC status populate event hdr failed");
    	      status &= FALSE;
    	     }
          }
	  else		 
	  {
            MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "event_mem_ptr is NULL for rfgsm_core_dm_populate_device_status_read_event_hdr");
            status &= FALSE;
          }
        }  
        break;

     case RFLM_GSM_TX_START_SCRIPT:
        if((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable_grfc_read == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_B))
		{
		  event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 
                                                                   core_data->triple_buffer_id, 
                                                                   script_type, 
                                                                   burst_num, 
                                                                   task_q );
		  if(event_mem_ptr!=NULL)
       	  {
            if(!rfgsm_core_dm_populate_device_status_read_event_hdr(core_data, event_mem_ptr, script_type, task_q, burst_num))
            { 
              MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM GRFC status populate event hdr failed");
              status &= FALSE;
            }
          }  
          else
          {
            MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "event_mem_ptr is NULL for rfgsm_core_dm_populate_device_status_read_event_hdr");
            status &= FALSE;
          }
		}
		break;

    case RFLM_GSM_RX_STOP_SCRIPT:
    case RFLM_GSM_MON_STOP_SCRIPT:
    case RFLM_GSM_RX_ACQ_STOP_SCRIPT:
    case RFLM_GSM_RX_PCH_STOP_SCRIPT:
	  	if(((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable_grfc_read == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_B)) ||
			((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_RX_BURST].enable == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_A)))
        {
		  event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 
                                                                   core_data->triple_buffer_id, 
                                                                   script_type, 
                                                                   burst_num, 
                                                                   task_q );
		  if(event_mem_ptr!=NULL)
       	  {
            if(!rfgsm_core_dm_populate_device_status_read_event_hdr(core_data, event_mem_ptr, script_type, task_q, burst_num))
            { 
              MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM GRFC status populate event hdr failed");
              status &= FALSE;
            }
          }  
          else
          {
            MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "event_mem_ptr is NULL for rfgsm_core_dm_populate_device_status_read_event_hdr");
            status &= FALSE;
          }
		}
		break;

   case RFLM_GSM_TX_STOP_SCRIPT:
		if(((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable_grfc_read == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_B)) ||
			((core_data->rfgsm_core_handle_ptr->device_config_to_read.params[RF_TX_BURST].enable == TRUE) && (task_q == RFLM_GSM_CCS_TASK_Q_A)))
		{
		  event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 
                                                                   core_data->triple_buffer_id, 
                                                                   script_type, 
                                                                   burst_num, 
                                                                   task_q );
		  if(event_mem_ptr!=NULL)
       	  {
           if(!rfgsm_core_dm_populate_device_status_read_event_hdr(core_data, event_mem_ptr, script_type, task_q, burst_num))
            { 
              MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core DM GRFC status populate event hdr failed");
              status &= FALSE;
            }
          }  
          else
          {
            MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "event_mem_ptr is NULL for rfgsm_core_dm_populate_device_status_read_event_hdr");
            status &= FALSE;
          }
		}
		break;
		
      default:	  
	  	break;  	
   }
  }

  return status;
}	

/*===========================================================================*/
boolean rfgsm_core_dm_set_ip2_cal_info( uint8 sub_id, 
                                        uint16 step_duration_qs, 
                                        uint16 dc_settling_time_qs, 
                                        uint16 ip2_cal_alpha,
                                        uint8 total_cal_steps )
{
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  /* update dc settling time*/
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.dc_settling_time_qs = dc_settling_time_qs;

  /* Update ip2 cal alpha*/
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_setup.ip2_cal_alpha = ip2_cal_alpha;

  /*Update cal step duration*/
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.step_duration_qs = step_duration_qs;

  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.max_num_cal_steps = total_cal_steps;

  return TRUE;
}



/*! @fn rfgsm_core_dm_dynamic_therm_event_wrapper                            */
/*! @brief This function updates passes pointer to a static memory 
    containing the following information:  
    - a flag to indicate whether averaging IM2 values of overlapping cal
      codes between current and previous iterations is needed or not.
    - pointer to array of cal code indices of overlapping points in
      current iteration grid. The cal code indices are arranged per
      traversal sequence.

    - A pointer to an array of cal code indices of overlapping points in
      the previous iteration.

*/
boolean rfgsm_core_dm_set_update_ip2_cal_grid_info( uint8 sub_id, 
                                                    rfgsm_ip2_cal_grid_info_type* const common_grid_info)
{
  rflm_gsm_data_type *table_ptr = NULL;

  if (common_grid_info == NULL)
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "rfgsm_core_dm_set_update_ip2_cal_grid_info for sub %d failed",sub_id);
    return FALSE;
  }

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }


  /* Update overlap area info*/
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.ip2_cal_grid_info.curr_iter_overlap_arr = common_grid_info->curr_iter_step_idx; 
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.ip2_cal_grid_info.prev_iter_overlap_arr = common_grid_info->prev_iter_step_idx;
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.ip2_cal_grid_info.num_overlap_points = common_grid_info->num_overlap_points;
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.ip2_cal_grid_info.do_average = common_grid_info->do_average;

  /*Reset overlap index*/
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.ip2_cal_grid_info.overlap_index = 0;

  return TRUE;
}


boolean rfgsm_core_dm_get_last_ip2_cal_iter_result(rflm_gsm_subscription_id sub_id, int32* min_im2_ptr, uint8* step_idx_ptr )
{
  rflm_gsm_data_type *table_ptr = NULL;

  if (( min_im2_ptr == NULL) || (step_idx_ptr == NULL))
  {
    return FALSE;
  }

  /* Get the GSM DM memory pointer */
  table_ptr = rflm_gsm_dm_get_table_ptr( sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core table_ptr NULL for sub %d",
          sub_id);
    return FALSE;
  }

  *min_im2_ptr = table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.min_im2_value;
  *step_idx_ptr = table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.min_im2_step_idx; // 0xFF means IP2 cal has never been attempted

  if(( *step_idx_ptr >= RFGSM_DEVICE_MAX_IP2_CAL_STEPS ) && (*step_idx_ptr != 0xFF) )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR, "rfgsm_core_dm_get_ip2_cal_result, index larger than max IP2 cal steps");
    return FALSE;
  }

  return TRUE;
}



boolean rfgsm_core_dm_init_ip2_cal_data(rfgsm_core_data_type *core_data_ptr)
{
  rflm_gsm_data_type *table_ptr = NULL;

  if ( core_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM core data ptr passed is NULL ");

    return FALSE;
  }

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data_ptr->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM table_ptr NULL for sub %d",
          core_data_ptr->sub_id);
    return FALSE;
  }

  if ( core_data_ptr->rfgsm_core_handle_ptr != NULL )
  {
    table_ptr->rfgsm_ip2_cal_event.ip2_cal_supported = rfgsm_core_get_sawless_status(core_data_ptr->rfm_dev);
  }
  else
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, "RF GSM core handle ptr NULL for device %d",core_data_ptr->rfm_dev );
    table_ptr->rfgsm_ip2_cal_event.ip2_cal_supported =  FALSE;
  }

  table_ptr->rfgsm_ip2_cal_event.first_iter =  TRUE;

  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.final_step_done = FALSE;

  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.min_im2_step_idx = 0xFF;

  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.min_im2_value = 0xFFFFFFFF;

  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.max_num_cal_steps = 0;
  
    /* initialize dc settling time*/
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_meas.dc_settling_time_qs = 0xFFFF;

  /* initialize ip2 cal alpha*/
  table_ptr->rfgsm_ip2_cal_event.ip2_cal_setup.ip2_cal_alpha = 0xFFFF;


  return TRUE;

  
}



/*===========================================================================*/
/*! @fn rfgsm_core_dm_clean_event                                            */
/*! @brief This function cleans the event tasks for a given event script type*/
boolean rfgsm_core_dm_clean_event( rfgsm_core_data_type* core_data,
                                   rfgsm_core_dm_script_type script_type )
{
  void* event_mem_ptr = NULL;
  rflm_gsm_data_type *table_ptr = NULL;

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM clean event table_ptr NULL for sub %d",
          core_data->sub_id);
    return FALSE;
  }

  event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 
                                                           core_data->triple_buffer_id,
                                                           script_type, 
                                                           0, 
                                                           RFLM_GSM_CCS_TASK_Q_A );

  if( !rfgsm_mdsp_clean_event(event_mem_ptr) )
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM clean event failed for sub %d, script %d, TQ %d, chain %d",
          core_data->sub_id, script_type, RFLM_GSM_CCS_TASK_Q_A);
    return FALSE;
  }

  event_mem_ptr = rfgsm_core_dm_get_data_table_event_addr( table_ptr, 
                                                           core_data->triple_buffer_id,
                                                           script_type, 
                                                           0, 
                                                           RFLM_GSM_CCS_TASK_Q_B );

  if( !rfgsm_mdsp_clean_event(event_mem_ptr) )
  {
    MSG_3(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM clean event failed for sub %d, script %d, TQ %d, chain %d",
          core_data->sub_id, script_type, RFLM_GSM_CCS_TASK_Q_B);
    return FALSE;
  }

  return TRUE;
}



boolean rfgsm_core_dm_set_ftm_sub_id(rfm_device_enum_type rfm_dev, rflm_gsm_subscription_id sub_id)
{


   /* Initialise variables */
  rflm_gsm_intf_data *rflm_intf_data_ptr = NULL;

  /* Get the RFLM GSM interface data location to populate */
  rflm_intf_data_ptr = rfgsm_core_dm_get_rflm_intf_data(0);


  /* Check the RFLM data pointer */
  if( rflm_intf_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"RF GSM core DM get intf data is NULL!");
    return FALSE;
  }

  /* Populate the RFLM RF data information */
  rflm_intf_data_ptr->intf_data_idx = 0; //for debug
  rflm_intf_data_ptr->script     = RFLM_GSM_TX_CAL_SCRIPT;
  rflm_intf_data_ptr->buffer_id  = 0;
  rflm_intf_data_ptr->burst_num  = 0;
  rflm_intf_data_ptr->device     = rfm_dev; 
  rflm_intf_data_ptr->sub_id     = sub_id;

  /* RFLM trace */
  rflm_gsm_log_intf_data(rflm_intf_data_ptr);


  MSG_1(MSG_SSID_RF, MSG_LEGACY_HIGH, "rfgsm_core_dm_set_ftm_sub_id in intf idx 0:%d",sub_id);

  return TRUE;


}


/*===========================================================================*/
/*! @fn rfgsm_core_dm_populate_ip2_intf_data                                            */
/*! @brief This function is called to populate IP2 cal related 
    interface data to DM. Example of such data is :Spectral iversion data.
    interface index represents the part of DM used to store such data.
    When GFW calls RFLM APIs for execution, the data is queried using the
    same interface index passed by GFW
 
    For IP2 cal, the understanding between GFW and RF is to use interface index 0
    for IP2 cal related data.
    */
boolean rfgsm_core_dm_populate_ip2_intf_data( rfgsm_core_data_type *core_data,
                                              rfgsm_rx_burst_type rx_burst_type,
                                              uint8 burst_num,
                                              uint8 intf_idx )
{

  if ( core_data == NULL )
  {

    MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_populate_ip2_intf_data, RF GSM core data ptr is null");
    return FALSE;
  }


  if ( rx_burst_type < RFGSM_MAX_RX_BURST_TYPES )
  {
    /* Populate the IP2 cal spec inv data start RFLM data info */
    if( !rfgsm_core_dm_populate_rflm_intf_data( core_data,
                                                rflm_gsm_rx_start_script_map[rx_burst_type],
                                                burst_num,
                                                intf_idx ) )
    {
      MSG(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_populate_ip2_intf_data, RF GSM core update IP2 cal interface data in DM failed");
    return FALSE;
    }
  }
  else
  {

    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR,"rfgsm_core_dm_populate_ip2_intf_data, RF GSM core update IP2 cal interface data in DM failed due to invalid burst type :%d",rx_burst_type);
    return FALSE;
  }


  return TRUE;
}

/*===========================================================================*/
/*! @fn rfgsm_core_dm_init_jdet_read_data                                            */
/*! @brief This function initialises the DM memory for the Jammer Detector reads     */
boolean rfgsm_core_dm_init_jdet_read_data(rfgsm_core_data_type *core_data_ptr)
{
  rflm_gsm_data_type *table_ptr = NULL;
  rflm_gsm_jdet_read_script_data *jdet_script_data_ptr = NULL;

  uint8 i = 0;

  if ( core_data_ptr == NULL )
  {
    MSG(MSG_SSID_RF, MSG_LEGACY_HIGH, "RF GSM core data ptr passed is NULL ");

    return FALSE;
  }

  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr( core_data_ptr->sub_id );
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM table_ptr NULL for sub %d",
          core_data_ptr->sub_id);
    return FALSE;
  }

  /* Initialise therm read holdoff so we can read JDET until next therm read */
  table_ptr->rfgsm_jdet_read_data.jdet_therm_read_holdoff = FALSE;

  /* Build Rx JDET reads into DM, initialise callback data */
  jdet_script_data_ptr = table_ptr->rfgsm_jdet_read_data.rx_jdet_read_script;

  // For each reading index, set the active entry to 0 and invalidate the readings
  for ( i = 0 ; i < RFGSM_DEVICE_MAX_JDET_READS_PER_RX ; i++ )
  {
    /* Init callback parameters */
    jdet_script_data_ptr[i].cb_params.event_info_ptr = NULL;
    jdet_script_data_ptr[i].cb_params.fw_callback = NULL;
    jdet_script_data_ptr[i].cb_params.fw_jdet_cb_arg = NULL;
    jdet_script_data_ptr[i].cb_params.payload_ptr = NULL;
    jdet_script_data_ptr[i].cb_params.reading_idx = 0;
    jdet_script_data_ptr[i].cb_params.rflm_intf_idx = 0;
    jdet_script_data_ptr[i].cb_params.sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION;

    if( rfgsm_core_dm_dynamic_jdet_read_event_wrapper(core_data_ptr, i, RFLM_GSM_RX_JDET_READ_SCRIPT) == FALSE)
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "Failed building Rx JDET read into DM! index: %d, sub_id: %d",
            i, core_data_ptr->sub_id);
      return FALSE;
    }
  }

  /* Build Monitor JDET reads into DM, initialise callback data */
  jdet_script_data_ptr = table_ptr->rfgsm_jdet_read_data.mon_jdet_read_script;

  for ( i = 0 ; i < RFLM_JDET_NUM_MON_EVENTS ; i++ )
  {
    jdet_script_data_ptr[i].cb_params.event_info_ptr = NULL;
    jdet_script_data_ptr[i].cb_params.fw_callback = NULL;
    jdet_script_data_ptr[i].cb_params.fw_jdet_cb_arg = NULL;
    jdet_script_data_ptr[i].cb_params.payload_ptr = NULL;
    jdet_script_data_ptr[i].cb_params.reading_idx = 0;
    jdet_script_data_ptr[i].cb_params.rflm_intf_idx = 0;
    jdet_script_data_ptr[i].cb_params.sub_id = RFLM_GSM_DEFAULT_SUBSCRIPTION;

    if( rfgsm_core_dm_dynamic_jdet_read_event_wrapper(core_data_ptr, i, RFLM_GSM_MON_JDET_READ_SCRIPT) == FALSE )
    {
      MSG_2(MSG_SSID_RF, MSG_LEGACY_ERROR, 
            "Failed building Monitor JDET read into DM! index: %d, sub_id: %d",
            i, core_data_ptr->sub_id);
      return FALSE;
    }
  }
  
  return TRUE;
}

#ifdef FEATURE_TX_SHARING
/*===========================================================================*/
/*! @fn rfgsm_core_dm_dynamic_common_event_wrapper                           */
/*! @brief This function acts as a wrapper for the dynamic event method to be*/
/*!        backwards compatible with the old event building method           */
boolean rfgsm_core_dm_dynamic_common_event_wrapper( rfgsm_core_data_type* core_data,
                                                    rfgsm_core_dm_script_type script_type,
                                                    rflm_gsm_ccs_task_q task_q,
                                                    rf_buffer_intf** ccs_scripts,
                                                    size_t num_scripts,
                                                    void** event_handle_ptr,
                                                    uint8 burst_num )
{

  boolean ret_val = FALSE;

  ret_val = rfgsm_core_dm_dynamic_event_wrapper( core_data,
                                       script_type,
                                       task_q,
                                       ccs_scripts,
                                       num_scripts,
                                       event_handle_ptr,
                                       burst_num);

  return TRUE;
}

#endif

/*===========================================================================*/
/*! @fn rfgsm_core_dm_device_status_read                           */
/*! @brief This function inputs burst type and buffer index so that 
                we can print the device registers*/

boolean rfgsm_core_dm_device_status_read( uint8 sub_id, 
                                          uint8 buff_id,
                                          rf_burst_type burst_type, 
                                          uint8 event_type, 
                                          uint8 event_tag,
                                          uint8 burst_num)
{
  rflm_gsm_data_type *table_ptr = NULL; 
  rflm_gsm_ccs_tx_stop_task_q_a script_data_tx;
  rflm_gsm_ccs_rx_stop_task_q_a script_data_rx;
  uint8 num_tasks;
  
  /* Get the data table pointer from DM */
  table_ptr = rflm_gsm_dm_get_table_ptr(sub_id);
  if( table_ptr == NULL )
  {
    MSG_1(MSG_SSID_RF, MSG_LEGACY_ERROR, 
          "RF GSM core DM set Rx/Tx device table_ptr NULL for sub %d", sub_id);
    return FALSE;
  }
    
  if(burst_type == RF_RX_BURST)
  {
    script_data_rx = table_ptr->rfgsm_burst_event[buff_id].rx[event_type].rx_stop.task_q_a;
    if(script_data_rx.header.event_status == RFLM_CCS_RF_EVENT_COMPLETE)
    {
      num_tasks = script_data_rx.header.num_tasks;
      rfgsm_mdsp_device_status_read(script_data_rx.task, num_tasks, event_tag);      
    }
  }
  
  if(burst_type == RF_MON_BURST)
  {
    if(burst_num < RFLM_GSM_MAX_RSSI_MEASUREMENTS)
    {
      script_data_rx = table_ptr->rfgsm_burst_event[buff_id].monitor[burst_num].rx_stop.task_q_a;
      if(script_data_rx.header.event_status == RFLM_CCS_RF_EVENT_COMPLETE)
      {
        num_tasks = script_data_rx.header.num_tasks;
        rfgsm_mdsp_device_status_read(script_data_rx.task, num_tasks, event_tag);      
      }
    }
  }
  
  if(burst_type == RF_TX_BURST)
  {
    script_data_tx = table_ptr->rfgsm_burst_event[buff_id].tx.tx_stop.task_q_a;
    if(script_data_tx.header.event_status == RFLM_CCS_RF_EVENT_COMPLETE)
    {
      num_tasks = script_data_tx.header.num_tasks;
      rfgsm_mdsp_device_status_read(script_data_tx.task, num_tasks, event_tag);      
    }
  } 
  
  return TRUE;
}

