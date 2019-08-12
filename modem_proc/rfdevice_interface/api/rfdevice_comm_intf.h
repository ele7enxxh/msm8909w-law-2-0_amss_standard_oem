#ifndef _RFDEVICE_COMM_INTF_H_
#define _RFDEVICE_COMM_INTF_H_
/*!
  @file
  rfdevice_comm_intf.h
 
  @brief
  This file contains the interface class definition for all RF communication
  classes.
  
  @addtogroup RFDEVICE_COMM_INTF
  @{
*/
/*==============================================================================

  Copyright (c) 2013 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfdevice_interface/api/rfdevice_comm_intf.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/07/16   apm     Added rffe multi-byte read interface to fix an issue(CCS Task queue overloading due to inefficient 
                   single byte read)
10/13/14   shb     Added overloaded write_to_script API that populates rf script
                   buffer from arrays of addr and data
10/09/14   lm      Made rfdevice_comm_intf derive from rfa to make sure new operator is using
                   modem heap
05/19/14   zhw     Support for reading to RFFE script
10/15/13   jfc     Add stub for rfdevice_common_modify_reg_info
09/27/13   rp      Converted delay prameter to int16 in write_to_script/write_buffered API.
09/23/13   rp      new_time_ref parameter added in write to script api.
09/10/13   rp      Added flush_to_script method.
05/30/13   dps     Initial Revision

==============================================================================*/

#include "comdef.h"
#include "rf_buffer_intf.h"
#include "rfcom.h"
#include "rfm_mode_types.h"

#define RFDEVICE_COMM_FAILURE 255

/* Used as a handle to a class pointer */
typedef struct rfdevice_comm_type rfdevice_comm_type;

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus

/*!
  @brief
  Interface communication class that contains all APIs that every RF
  communication class needs to implement.

  @details
  Should not contain any data definitions. Just virtual interface methods that
  each RF communication class needs to implement. All data should be defined
  in derived classes as the type may be different for various protocol types.
 
  Contains pure virtual functions that must be implemented by classes extending
  this communication class.
*/
class rfdevice_comm_intf: public rfa
{
public:

  /*! Empty virtual destructor - base, interface class has no data so nothing 
  to delete */
  virtual ~rfdevice_comm_intf( void )
  {
  }


  /*! Derived class implementation should force anything in the buffer to be 
  executed immediately -- this only applies to writes done through 
  write_buffered, not write_script. */
  virtual void 
  flush( void ) = 0;

  /*! Derived class should execute an immediate write. */
  virtual boolean 
  write_immediate
  (
    uint16 addr, 
    uint8 data
  ) = 0;

  /*! Derived class should execute a delayed write from a script. */
  virtual boolean 
  write_buffered
  (
    uint16 addr, 
    uint8 data, 
    int16 delay
  ) = 0;

  /*! API to push to script buffer 1 transaction at a time */
  virtual boolean 
  write_to_script
  (
    rf_buffer_intf* buf,
    uint16 addr,
    uint8 data,
    rf_buffer_transaction_type transaction_type,
    int16 delay,
    boolean new_time_ref
  )
  {
    return FALSE;
  }

  /*! API to push data to script buffer from an array of address and data */
  virtual boolean
  write_to_script
  (
    rf_buffer_intf* script_buf,
    uint16* addr,
    uint8* data,
    int16* delay,
    uint16 buf_size,
    rf_buffer_transaction_type transaction_type,
    boolean new_time_ref
  )
  {
    return FALSE;
  }

  /*! Derived class should execute an immediate read. */
  virtual boolean 
  read_immediate
  (
    uint16 addr, 
    uint8 *data
  ) = 0;

  /*! Derived class should execute an immediate multi read. */
  virtual boolean 
  multi_read_immediate
  (
    uint16 *addr, 
    uint8 *data,
    uint16 count
  ) 
  {
    return FALSE;
  }

  /*! Derived class should use this method to flush the data to script. */
  virtual boolean
  flush_to_script(void)
  { 
    return TRUE;
  }  

  /*! Derived class should use this method to read the data to script. */
  virtual boolean
  read_to_script(   rf_buffer_intf* rf_buff, void* return_script, uint8 *read_size, uint8 max_read_size )
  {
    return TRUE;
  }
};

extern "C"
{

#else

typedef struct rfdevice_comm_intf_type rfdevice_comm_intf;

#endif /* __cplusplus */

/* ************************************************************************ */
/*                         C-compatible Interface                           */
/* ************************************************************************ */

/*! Forces anything in the buffer to be executed immediately -- this only 
  applies to writes done through write_buffered, not write_script. */
void
rfdevice_comm_flush
(
  rfdevice_comm_type *rfdevice_comm
);

/*! Executes an immediate write. */
boolean 
rfdevice_comm_write_immediate
(
  rfdevice_comm_type *rfdevice_comm,
  uint16 addr,
  uint8 data
);

/*! Executes a delayed write from a script. */
boolean 
rfdevice_comm_write_buffered
(
  rfdevice_comm_type *rfdevice_comm,
  uint16 addr,
  uint8 data,
  int16 delay
);

/*! Adds the transaction to a buffer to be written. Provides better 
  performance for immediate writes than write_immediate. */
boolean 
rfdevice_comm_write_to_script
(
  rfdevice_comm_type *rfdevice_comm,
  rf_buffer_intf* buf,
  uint16 addr,
  uint8 data,
  rf_buffer_transaction_type transaction_type,
  int16 delay,
  boolean new_time_ref
);

/*! Executes an immediate read. */
boolean 
rfdevice_comm_read_immediate
(
  rfdevice_comm_type *rfdevice_comm,
  uint16 addr,
  uint8 *data
);

/*! Common device point point of entry to modify AG register tables */
boolean
rfdevice_common_modify_reg_info
(
  rfcom_mode_enum_type mode,
  rfcom_band_type_u band,
  void* new_data
);

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */
#endif /*_RFDEVICE_COMM_INTF_H_*/
