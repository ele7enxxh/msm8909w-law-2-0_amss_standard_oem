#ifndef _RFDEVICE_CCS_RFFE_COMM_IMPL_H_
#define _RFDEVICE_CCS_RFFE_COMM_IMPL_H_
/*!
  @file
  rfdevice_ccs_rffe_comm_impl.h
 
  @brief
  This file contains the class used for all RF on-target CCS RFFE
  communication classes, by calling the appropriate CCS functions.
  
  @addtogroup RFDEVICE_COMM_INTF
  @{
*/

/*==============================================================================

  Copyright (c) 2013-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

===============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_ccs_rffe_comm_impl.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/07/16   apm     Added rffe multi-byte read interface to fix an issue(CCS Task queue overloading due to inefficient 
                   single byte read)
10/13/14   shb     Added overloaded write_to_script API that populates rf script
                   buffer from arrays of addr and data
05/19/14   zhw     Support for reading to RFFE script
04/09/14   ndb     Added a flag for Enable/Disable the Multi-byte write in Commclass
02/27/14   yb      Half Rate support for RFFE reads/writes
11/05/13   rp      Code changes to update the extended_cmd flag according to the CCS 
                   implementation.
10/25/13   bmg     Removed unnecessary dependecy on modem hardware specifics
09/27/13   rp      Converted delay prameter to int16 in write_to_script/write_buffered API.
09/23/13   rp      new_time_ref parameter added in script entry holder as well as in
                   write to script API.
08/28/13   rp      Added support for mutibyte write in write to script and 
                   buffered write api.
06/05/13   dps     Initial Revision

==============================================================================*/

#include "rfdevice_comm_intf.h"
#include "rfdevice_cmn_type_defs.h"
#include "rf_hal_bus_types.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  The amount of space that is reserved in the buffer for pending
  transactions.
*/
#define RFDEVICE_CCS_RFFE_COMM_BUFFER_SCRIPT_SIZE 255

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Buffer type for CCS RFFE optimizations.
*/
typedef struct
{
  /*! Current number of buffer elements */
  uint32 size;

  /*! Channel for all current buffered writes */
  uint8 channel;

  rf_hal_bus_resource_script_settings_type settings;

  rf_hal_bus_rffe_type script[RFDEVICE_CCS_RFFE_COMM_BUFFER_SCRIPT_SIZE];
} rfdevice_ccs_rffe_comm_buffer_type;


  /*! 
  @brief
  Script data holder type to accumulate the contiguous data bytes for extended script writes.
  */
typedef struct
{

  /*! Poniter to the buffer where script has to be written */

  rf_buffer_intf* buf;

  /*! Channel for all current stored bytes */

  uint8 channel;

  /*! Usid for all current stored bytes */

  uint8 usid;

  /*starting address for current multibyte write.*/

  uint16 start_addr;

  /*! Buffer to hold the data for extended writes. */

  uint8 data[RF_HAL_BUS_RFFE_MAX_BYTES];

  /*! Number of data bytes present in script entry holder. */

  uint8  num_script_bytes;

  /*! Transaction type for current entry present in the entry holder */

  rf_buffer_transaction_type transaction_type;

  /*! start delta for this script entry */

  uint16 delay;

  /*! flag indicating the presence of new time reference */

  boolean new_time_ref;

} rfdevice_ccs_rffe_comm_script_entry_holder_type;




/*----------------------------------------------------------------------------*/
#ifdef __cplusplus

/*!
  @brief
  Communication class implementation for on-target CCS RFFE devices.
 
  @details
  Contains optimizations for RFFE communication in on-target CCS devices.
*/
class rfdevice_ccs_rffe_comm_impl : public rfdevice_comm_intf
{
public:
  /*! 
    @brief
    Constructs a CCS RFFE communication object.
   
    @param slave_id
    USID number for the device that this object is associated with.
   
    @param chan
    Channel number on the USID for this device.
   
    @param buffer
    Pointer to a buffer that is shared with every other communication object
    on this device.

    @script entry pointer 
    Pointer to script entry holder for appending extended entries into the script
  */
  rfdevice_ccs_rffe_comm_impl
  (
    uint8 chan,
    uint8 slave_id, 
    rfdevice_ccs_rffe_comm_buffer_type *&buffer,
    rfdevice_ccs_rffe_comm_script_entry_holder_type *script_entry_pointer  ,
    boolean half_rate = FALSE,
    boolean ext_cmd = TRUE
  );

  /*! 
    @brief
    Flushes anything in the buffer.
  */
  ~rfdevice_ccs_rffe_comm_impl();

  /*! 
    @brief
    Forces anything in the buffer to be executed immediately. This only applies
    to writes done through write_buffered, not write_script.
  */
  void flush();

  /*! 
    @brief
  Forces anything in the script byte holder to be appended immediately. 
  This only applies to writes done through write_to_script, not to write_to_script_immediate.

  @retval TRUE
  flush to script was successful.

  @retval FALSE
  flush to script was unsuccessful.
  */
  boolean flush_to_script();

  /*! 
    @brief
    Executes an immediate write.
   
    @param addr
    Address to write to.
   
    @param data
    Data to write.
   
    @retval TRUE
    write_immediate was successful.
   
    @retval FALSE
    write_immediate was unsuccessful.
  */
  boolean 
  write_immediate
  (
    uint16 addr, 
    uint8 data
  );

  /*! 
    @brief
    Executes a delayed write from a buffer. Provides better performance for
    immediate writes than write_immediate.
   
    @param addr
    Address to write to.
   
    @param data
    Data to write.
   
    @param delay
    Delay to be applied before this write.
   
    @retval TRUE
    write_buffered was successful.
   
    @retval FALSE
    write_buffered was unsuccessful.
  */
  virtual boolean 
  write_buffered
  (
    uint16 addr, 
    uint8 data, 
    int16 delay
  );

  /*! 
    @brief
    Adds the transaction to a script to be written.
   
    @param buf
    Pointer to the buffer/script to add this transaction to.
   
    @param addr
    Address to write to.
   
    @param data
    Data to write.
   
    @param transaction_type
    Read or write.
   
    @param delay
    Delay to be applied before this write.
   
    @retval TRUE
    write_to_script was successful.
   
    @retval FALSE
    write_to_script was unsuccessful.
  */
  boolean 
  write_to_script
  (
    rf_buffer_intf* buf,
    uint16 addr,
    uint8 data,
    rf_buffer_transaction_type transaction_type,
    int16 delay,
    boolean new_time_ref = FALSE
  );

  /*----------------------------------------------------------------------------*/
  /*!
    @brief
    Populates rf_buffer_intf* from an array of addr, data, delay
  
    @details
    Optimized implementation of write_to_script that takes an entire array of
    address, data and delay and pushes it to script_buf. Also looks for contiguous
    addresses to minimize number of transactions with multi-byte writes.
   
    Does NOT use rfdevice_ccs_rffe_comm_script_entry_holder_type
    *script_entry_holder hence is re-entrant.
   
    @param script_buf
    RF script buffer to which RFFE transactions need to be appended
   
    @param addr
    Pointer to start of array of addresses that needs to be pushed to script_buf
   
    @param data
    Pointer to first element of array of data corresponding to the array of addr
   
    @param delay
    Pointer to first element of array of delay corresponding to the array of addr.
    Delay is wrt to previous transaction. For example delay = 10 indicates, wait
    10 units after executing previous transaction before executing current one
   
    @param buf_size
    Size of array of addresses that needs to be processed
   
    @param transaction_type
    Indicates whether a read or write script is being created
   
    @param new_time_ref
    Indicates if the first transaction is wrt to the previous entries (if any) in
    the script_buf. After the first transaction, new_time_ref is not used, it will
    be forced FALSE
   
    @return
    Success/Failure of the API
  */
  boolean
  write_to_script
  (
    rf_buffer_intf* script_buf,
    uint16* addr,
    uint8* data,
    int16* delay,
    uint16 buf_size,
    rf_buffer_transaction_type transaction_type,
    boolean new_time_ref
  );

  /*! 
    @brief
    Executes an immediate read.
   
    @param addr
    Address to read from.
   
    @param data
    Pointer to the data to be read from the register.
   
    @retval TRUE
    read_immediate was successful.
   
    @retval FALSE
    read_immediate was unsuccessful.
  */
  boolean 
  read_immediate
  (
    uint16 addr, 
    uint8 *data
  );
  /*! 
     @brief
     Executes multiple RFFE read operations.Number of reads depends on the input 
     argument 'count' 
    
     @param addr
     pointer to the Addresses to read from.
    
     @param data
     Pointer to the data to be read from the registers.

     @param count
     count of registers
    
     @retval TRUE
     multi_read_immediate was successful.
    
     @retval FALSE
     multi_read_immediate was unsuccessful.
   */

  boolean
  multi_read_immediate
  (
    uint16 *addr, 
    uint8 *data,
    uint16 count
  );

  /*! 
    @brief
    Executes an script read.
   
    @param rf_buff
    Input script that contains read control info (channel,slave ID,addr,etc)
   
    @param rffe_return_script
    Output script that contains read results.
   
    @param read_size
    Number of reads returned by CCS, this should match the input exactly
   
    @param max_read_size
    Maximun number of reads for CCS
   
    @return boolean
    TRUE if successful, FALSE if not.
  */
  boolean
  read_to_script
  ( 
     rf_buffer_intf* rf_buff, 
     void* rffe_return_script,
     uint8 *read_size,
     uint8 max_read_size
  );

  /*!
    @brief
    Returns a pointer to the buffer that is shared by all communication
    objects associated with this device.
  */
  rfdevice_ccs_rffe_comm_buffer_type* 
  get_buffer(void);

private:
  /*! Channel number for this device. */
  uint8 channel;

  rf_hal_bus_resource_script_settings_type ccs_rffe_comm_settings;

  /*! Slave ID of the channel for this device. */
  uint8 usid;

  /*! Enable/disable Extended Task creation(needed for Multi-byte write) */
  boolean extended_cmd;

  /*! Pointer to a buffer that is shared between all communication objects 
    associated with this device.
  */
  rfdevice_ccs_rffe_comm_buffer_type *buffer;

  /*! Pointer to a buffer that is shared between all communication objects 
  associated with this device.
  */
  rfdevice_ccs_rffe_comm_script_entry_holder_type *script_entry_holder;  

  /*!
   @brief
   Validates if current RFFE trasnaction is of extended type
  */
  boolean 
  is_rffe_transaction_ext_type(uint16 address, uint8 num_bytes);
  
};
#endif /* __cplusplus */
#endif /* _RFDEVICE_CCS_RFFE_COMM_IMPL_H_ */
