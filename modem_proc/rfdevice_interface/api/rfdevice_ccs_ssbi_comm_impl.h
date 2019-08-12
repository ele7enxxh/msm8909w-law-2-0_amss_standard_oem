#ifndef _RFDEVICE_CCS_SSBI_COMM_IMPL_H_
#define _RFDEVICE_CCS_SSBI_COMM_IMPL_H_
/*!
  @file
  rfdevice_ccs_ssbi_comm_impl.h
 
  @brief
  This file contains the class used for all RF on-target CCS SSBI
  communication classes, by calling the appropriate CCS functions.
  
  @addtogroup RFDEVICE_COMM_INTF
  @{
*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_ccs_ssbi_comm_impl.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
10/25/13   bmg     Removed unnecessary dependecy on modem hardware specifics
09/27/13   rp      Converted delay prameter to int16 in write_to_script/write_buffered API.
09/23/13   rp      new_time_ref parameter added in write to script API.
06/05/13   dps     Initial Revision

==============================================================================*/

#include "rfdevice_comm_intf.h"
#include "rfdevice_cmn_type_defs.h"

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  The amount of space that is reserved in the buffer for pending
  transactions.
*/
#define RFDEVICE_CCS_SSBI_COMM_BUFFER_SCRIPT_SIZE 255

/*----------------------------------------------------------------------------*/
/*! 
  @brief
  Buffer type for CCS SSBI optimizations.
*/
typedef struct
{
  /*! Current number of buffer elements */
  uint16 size;

  /*! Bus number for all current buffered writes */
  uint8 bus;

  uint8 address [RFDEVICE_CCS_SSBI_COMM_BUFFER_SCRIPT_SIZE];
  uint8 data [RFDEVICE_CCS_SSBI_COMM_BUFFER_SCRIPT_SIZE];
} rfdevice_ccs_ssbi_comm_buffer_type;

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus

/*!
  @brief
  Communication class implementation for on-target CCS SSBI devices.
 
  @details
  Contains optimizations for SSBI communication in on-target CCS devices.
*/
class rfdevice_ccs_ssbi_comm_impl : public rfdevice_comm_intf
{
public:
  /*! 
    @brief
    Constructs a CCS SSBI communication object.
   
    @param bus_num
    SSBI bus number for the device that this object is associated with.
   
    @param buffer
    Pointer to a buffer that is shared with every other communication object
    on this device.
  */
  rfdevice_ccs_ssbi_comm_impl
  (
    uint8 bus_num, 
    rfdevice_ccs_ssbi_comm_buffer_type *buffer
  );

  /*! 
    @brief
    Flushes anything in the buffer.
  */
  ~rfdevice_ccs_ssbi_comm_impl();

  /*! 
    @brief
    Forces anything in the buffer to be executed immediately. This only applies
    to writes done through write_buffered, not write_script.
  */
  void flush();

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
    Returns a pointer to the buffer that is shared by all communication
    objects associated with this device.
  */
  rfdevice_ccs_ssbi_comm_buffer_type* 
  get_buffer(void);

private:
  /*! Bus number for the device associated with this communication object. */
  uint8 bus;

  /*! Pointer to a buffer that is shared between all communication objects 
    associated with this device.
  */
  rfdevice_ccs_ssbi_comm_buffer_type *buffer;
};
#endif /* __cplusplus */
#endif /* _RFDEVICE_CCS_SSBI_COMM_IMPL_H_ */
