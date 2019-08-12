/*!
  @file 
  rf_hal_rffe.h

  @brief
  Header file for RFFE RF HAL Bus functions

*/

/*============================================================================== 
   
  Copyright (c) 2012 - 2013 Qualcomm Technologies Incorporated. All Rights Reserved 
   
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
   
                        EDIT HISTORY FOR MODULE 
   
This section contains comments describing changes made to the module. 
Notice that changes are listed in reverse chronological order. 
 
$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/ccs/rf_hal_rffe.h#1 $ 
   
when       who     what, where, why 
--------   ---     ------------------------------------------------------------- 
05/02/14   yb      Fixed max read size for rfhal_rffe_execute_rf_buf
10/16/13   ra      Support CCS portability
07/10/13   jfc     Update RFFE API documentation
06/19/13   aca     New API to process RFFE buffer/script involving reads, writes and delays
07/02/12   jfc     Remove count from write_byte() and read_byte()
05/21/12   jfc     Add API functions for extended RFFE read and write 
05/17/12   jfc     Fill in function prototypes
01/09/12   jfc     Initial version
==============================================================================*/ 


#ifndef RF_HAL_RFFE_H
#define RF_HAL_RFFE_H

#include "rf_hal_bus_types.h"
#include "rf_hal_buffer.h"
#include "rf_hal_ccs.h"

/*! @brief Write a single RFFE register 
 
  @details Write a single RFFE transaction to a single device. Supports all
  MIPI RFFE command sequences, but only up to 1 byte of payload. Proper
  command sequence type is automatically chosen.
 
  @param channel
  The bus number of RFFE device. Only a single channel is supported
  per API call.
 
  @param slave_id
  The slave ID of the RFFE device. Only a single slave is supported
  per API call.
 
  @param reg
  Address of register to write to. Supports up to 16 bits
 
  @param data
  Data to write to register. Only 7 bits are supported if reg is 0x0
 
  @param settings
  Pointer to struct of misc settings. Only settings.priority is supported
 
  @return
  Error status function call
 
*/
rf_hal_bus_result_type rfhal_rffe_write_byte
(
  uint8 channel,
  uint8 slave_id,
  uint16 reg,
  uint8 data,
  rf_hal_bus_resource_script_settings_type* settings
);



/*! @brief Write multiple RFFE registers (simple)
 
   
  @details Write multiple RFFE registers to a single device. Supports 
  MIPI RFFE command sequences: reg0, write, write ext,
  but only up to 1 byte of payload. Do not pay too much attention to
  the "non_ext"in the function name. Proper command sequence type
  is automatically chosen.
 
  @param channel
  The bus number of RFFE device. Only a single channel is supported
  per API call.
 
  @param slave_id
  The slave ID of the RFFE device. Only a single slave is supported
  per API call.
 
  @param reg
  Pointer to array of register addresses. Supports up to 8 bits only
 
  @param data
  Pointer to array of data bytes. Only 7 bits are supported if reg is 0x0.
 
  @param count
  Number of transactions to write. Number of valid elements in data
  array and reg array are expected to be the same and equal to count
 
  @param settings
  Pointer to struct of misc settings. Only settings.priority is supported
 
  @return
  Error status function call
 
*/
rf_hal_bus_result_type rfhal_rffe_write_bytes_non_ext
(
  uint8 channel,
  uint8 slave_id,
  uint8* reg,
  uint8* data,
  uint32 count,
  rf_hal_bus_resource_script_settings_type* settings
);



/*! @brief Write multiple RFFE registers (advanced)
 
  @details Write multiple RFFE registers to one or more devices on the same
  RFFE bus. Supports all MIPI RFFE write command sequences, but only up to 4 bytes
  of payload (where applicable). Do not pay too much attention to the "ext"
  in the function name. Proper command sequence type is automatically chosen.
 
  @param channel
  The bus number of RFFE device. Only a single channel is supported
  per API call.
 
  @param script_ptr
  Pointer to array of RFFE transactions (each of type rf_hal_bus_rffe_type).
  Fields _ignored_ from each transaction are
  rd_wr       Only writes are supported in this API
  channel     Only the channel parameter passed to this function takes effect
  rd_delay    Not used for write operation
  half_rate   Not used for write operation
  start_delta  All transactions are assumed back-to-back
  extended_cmd The command sequence format is automatically determined
 
  @param count
  Size of script_ptr array
 
  @param settings
  Pointer to struct of misc settings. Only settings.priority, settings.preempt_error,
  settings.force_ext are supported. If force_ext is set, all transactions will use
  RFFE ext write or ext write long
 
  @return
  Error status function call
 
*/
rf_hal_bus_result_type rfhal_rffe_write_bytes_ext
(
  rf_hal_bus_rffe_type* script_ptr,
  uint8 channel,
  uint32 count,
  rf_hal_bus_resource_script_settings_type* settings
);


 
/*! @brief Read a single RFFE register 
 
  @details Read a single RFFE transaction from a single device. Supports all
  MIPI RFFE read command sequences, but only up to 1 byte of payload. Proper
  command sequence type is automatically chosen.
 
  @param channel
  The bus number of RFFE device. Only a single channel is supported
  per API call.
 
  @param slave_id
  The slave ID of the RFFE device. Only a single slave is supported
  per API call.
 
  @param reg
  Address of register to write from. Supports up to 16 bits
 
  @param data
  Pointer to 1 byte of memory to hold read data. Only 7 bits are
  valid if reg is 0x0
 
  @param settings
  Pointer to struct of misc settings. Only settings.priority,
  settings.rd_delay, settings.half_rate are supported
 
  @return
  Error status function call
 
*/
rf_hal_bus_result_type rfhal_rffe_read_byte
(
  uint8 channel,
  uint8 slave_id,
  uint16 reg,
  uint8* data,
  rf_hal_bus_resource_script_settings_type* settings
);



/*! @brief Read multiple RFFE registers (simple)
   
  @details Read multiple RFFE registers from a single device. Supports 
  MIPI RFFE command sequences: read, read ext,
  but only up to 1 byte of payload. Do not pay too much attention to
  the "non_ext"in the function name. Proper command sequence type
  is automatically chosen.
 
  @param channel
  The bus number of RFFE device. Only a single channel is supported
  per API call.
 
  @param slave_id
  The slave ID of the RFFE device. Only a single slave is supported
  per API call.
 
  @param reg
  Pointer to array of register addresses. Supports up to 8 bits only
 
  @param data
  Pointer to array of data bytes. Only 7 bits are supported if reg is 0x0.
  Data is passed back to caller by writing this memory
 
  @param count
  Number of transactions to read. Number of valid elements in data
  array and reg array are expected to be the same and equal to count
 
  @param settings
  Pointer to struct of misc settings. Only settings.priority,
  settings.rd_delay, settings.half_rate are supported
 
  @return
  Error status function call
 
*/
rf_hal_bus_result_type rfhal_rffe_read_bytes_non_ext
(
  uint8 channel,
  uint8 slave_id,
  uint8* reg,
  uint8* data,
  uint32 count,
  rf_hal_bus_resource_script_settings_type* settings
);



/*! @brief Read multiple RFFE registers (advanced)
 
  @details Read multiple RFFE registers from one or more devices on the same
  RFFE bus. Supports all MIPI RFFE read command sequences, but only up to 4 bytes
  of payload (where applicable). Do not pay too much attention to the "ext"
  in the function name. Proper command sequence type is automatically chosen.
 
  @param channel
  The bus number of RFFE device. Only a single channel is supported
  per API call.
 
  @param script_ptr
  Pointer to array of RFFE transactions (each of type rf_hal_bus_rffe_type).
  Fields _ignored_ from each transaction are
  channel      Only the channel parameter passed to this function takes effect
  start_delta  All transactions are assumed back-to-back
  extended_cmd The command sequence format is automatically determined
 
  The rd_wr field should be READ. Transactions set to WRITE will lead to
  undefined behavior
 
  @param count
  Size of script_ptr array
 
  @param settings
  Pointer to struct of misc settings. Only settings.priority, settings.preempt_error,
  settings.force_ext are supported. If force_ext is set, all transactions will use
  RFFE ext write or ext write long
 
  @return
  Error status function call. The read data is placed in script_ptr[i].data[0..3]
  where script_ptr[i].num_bytes of bytes will valid, for i = [0..count-1]
 
*/
rf_hal_bus_result_type rfhal_rffe_read_bytes_ext
(
  rf_hal_bus_rffe_type* script_ptr,
  uint8 channel,
  uint32 count,
  rf_hal_bus_resource_script_settings_type* settings
);



/*! @brief Process an entire script buffer. 
         Any reads will be sent back in the original script
      @param script_ptr script to be processed
      @count number of transactions
      @settings 
*/

rf_hal_bus_result_type rfhal_rffe_execute_buf_script
(
  rf_hal_bus_rffe_type* script_ptr,
  uint32 count,
  rf_hal_bus_resource_script_settings_type* settings
);
/*! @brief Process an buffer object
      @param rf_buff buffer to be processed
      @rffe_return_script return script for reads
      @return_read_size number of registers read back
      @settings 
*/
rf_hal_bus_result_type rfhal_rffe_execute_rf_buf
(
  rf_buffer_intf* rf_buff, 
  rf_hal_bus_rffe_type* rffe_return_script,
  uint8 *return_read_size,
  rf_hal_bus_resource_script_settings_type* settings,
  uint8 max_read_size
);

#endif /* RF_HAL_RFFE_H */


