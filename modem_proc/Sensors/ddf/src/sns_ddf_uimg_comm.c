/**
 * @file sns_ddf_comm.c
 * @brief Implementation of the Device Driver Framework communication module.
 *
 * @see sns_ddf_comm.h
 *
 * Copyright (c) 2014 - 2015 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.


  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-09-24 hw   Explicitly declare qup related APIs to fix compiler warning
  2015-08-14 tc   Added automatic QUP clock management when performing reads/writes
  2015-02-24 pn   Updated QDSS events
  2015-01-21 BD   Moved F3 constant string from uImage to bigImage
  2014-09-05 jms  Added SPI support
  2014-08-05 ps   Fix bus type for simple read/write port functions
  2014-09-03 vy   Added QDSS logs
  2014-06-05 vy   Refactored for uImage
==============================================================================*/

#include "sns_profiling.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_ddf_util.h"
#include "sns_memmgr.h"
#include "sns_debug_str.h"

#include "sns_ddf_comm_priv.h"
#include "sns_smgr_ddf_priv.h"

/*----------------------------------------------------------------------------
 * Private Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Extern Variables and Function Declarations
 * -------------------------------------------------------------------------*/
extern void sns_hw_set_qup_clk(bool enable_flag);

extern bool sns_hw_qup_clck_status(void);

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Private Structure Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Public Global Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Local Variables
 * -------------------------------------------------------------------------*/

//============================================================================
//============================================================================
//                         PUBLIC  BUS  API
//============================================================================
//============================================================================

/**
 * @brief Performs a bus read operation for up to 2^32 - 1 bytes 
 *        at a time.
 *
 * @param[in]  handle      The I/O port handle that identifies a device. This is
 *                         given to drivers as part of initialization.
 * @param[in]  reg_addr    Address of the register to read on the device.
 * @param[out] buffer      Location where result will be stored. Note size of
 *                         this buffer must >= @a bytes.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
 SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_read_port_u32_count(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    uint8_t*                buffer,
    uint32_t                bytes,
    uint32_t*               read_count)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  bool qup_clck_state;

  if ( handle == NULL || buffer == NULL || read_count == NULL )
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "fun arg is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }

//  static uint32_t xcntr=0;
//  DDF_COMM_MSG_4( HIGH, "#%d READ bus=%d addr=0x%X bytes=%d",
//                  xcntr++, ((sns_ddf_sensor_info_s*)handle)->bus,
//                  reg_addr, bytes );

  sns_profiling_log_qdss(
    SNS_DDF_BUS_READ_ENTER, 1, ((sns_ddf_sensor_info_s*)handle)->bus);

  /* PEND: The QUP function calls violate the separation between the
           SMGR and DDF subsystems. This needs to be revisited. */
  // Save the previous QUP clock status before attempting to turn it on
  qup_clck_state = sns_hw_qup_clck_status();

  // Try to turn on the QUP clock if it is not currently turned on
  sns_hw_set_qup_clk(true);

  switch( ((sns_ddf_sensor_info_s*)handle)->bus )
  {
    case SNS_DDF_BUS_I2C:
      status = sns_ddf_comm_bus_i2c_read( handle, reg_addr, buffer, bytes, read_count );
      break;
    case SNS_DDF_BUS_SPI:
#if SNS_DDF_COMM_BUS_SPI_ENABLE_DRIVER
      status = sns_ddf_comm_bus_spi_read( handle, reg_addr, buffer, bytes, read_count );
      if ( status == SNS_DDF_SUCCESS )
      {
        *read_count = bytes; //not implemented in SPI driver
      } else
      {
        *read_count = 0;
      }
#endif
      break;

    default:
      status = SNS_DDF_EINVALID_PARAM;
  }

  // Return the QUP clock to its previous state
  sns_hw_set_qup_clk(qup_clck_state);

  sns_profiling_log_qdss( SNS_DDF_BUS_READ_EXIT, 2, status, *read_count);
  return status;
}


/**
 * @brief Performs a bus read operation.
 *
 * @param[in]  handle      The I/O port handle that identifies a device. This is
 *                         given to drivers as part of initialization.
 * @param[in]  reg_addr    Address of the register to read on the device.
 * @param[out] buffer      Location where result will be stored. Note size of
 *                         this buffer must >= @a bytes.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
 SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_read_port(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    uint8_t*                buffer,
    uint8_t                 bytes,
    uint8_t*                read_count)
{
   uint32 bytes_read       = 0;
   sns_ddf_status_e status = SNS_DDF_SUCCESS;

   if ( read_count == NULL )
   {
     SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "fun arg is NULL");
     return SNS_DDF_EINVALID_PARAM;
   }

   status = sns_ddf_read_port_u32_count(handle, reg_addr, buffer, bytes, &bytes_read);

   *read_count = (uint8_t)bytes_read;

   return status;
}


/**
 * @brief Performs a bus write operation.
 *
 * @param[in]  handle       The I/O port handle that identifies a device. This
 *                          is given to drivers as part of initialization.
 * @param[in]  reg_addr     Address of the register to write on the device.
 * @param[in]  buffer       Location where value to be written is stored. Note
 *                          that size of this buffer must >= @a bytes.
 * @param[in]  bytes        Number of bytes to write.
 * @param[out] write_count  Number of bytes actually written from the buffer.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
SNS_DDF_UIMAGE_CODE sns_ddf_status_e sns_ddf_write_port(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    const uint8_t*          buffer,
    uint8_t                 bytes,
    uint8_t*                write_count)
{
  uint32 bytes_written = 0;
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  bool qup_clck_state;

  if ( handle == NULL || write_count == NULL )
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "fun arg is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }

//  static uint32_t xcntw=0;
//  DDF_COMM_MSG_4( HIGH, "#%d WRITE bus=%d addr=0x%X bytes=%d",
//                  xcntw++, ((sns_ddf_sensor_info_s*)handle)->bus,
//                  reg_addr, bytes );

  /* PEND: The QUP function calls violate the separation between the
           SMGR and DDF subsystems. This needs to be revisited. */
  // Save the previous QUP clock status before attempting to turn it on
  qup_clck_state = sns_hw_qup_clck_status();

  // Try to turn on the QUP clock if it is not currently turned on
  sns_hw_set_qup_clk(true);

  switch( ((sns_ddf_sensor_info_s*)handle)->bus )
  {
    case SNS_DDF_BUS_I2C:
      status = sns_ddf_comm_bus_i2c_write( handle, reg_addr, buffer, bytes, &bytes_written );
      break;
    case SNS_DDF_BUS_SPI:
#if SNS_DDF_COMM_BUS_SPI_ENABLE_DRIVER
      status = sns_ddf_comm_bus_spi_write( handle, reg_addr, buffer, bytes, &bytes_written );
      if ( status == SNS_DDF_SUCCESS )
      {
        bytes_written = bytes;
      } else
      {
        bytes_written = 0;
      }
#endif
      break;

    default:
      status = SNS_DDF_EINVALID_PARAM;
  }

  // Return the QUP clock to its previous state
  sns_hw_set_qup_clk(qup_clck_state);

  *write_count = (uint8_t)bytes_written;
  return status;
}


/**
 * @brief Performs a bus write operation.
 *
 * @param[in]  handle       The I/O port handle that identifies a device. This
 *                          is given to drivers as part of initialization.
 * @param[in]  reg_addr     Address of the register to write on the device.
 * @param[in]  buffer       Value to be written.
 * @param[out] status       SNS_DDF_SUCCESS if the write
 *  	   operation was done successfully. Otherwise
 *  	   SNS_DDF_EDEVICE, SNS_DDF_EBUS,
 *  	   SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate
 *  	   an error has occurred
 *
 * @return none
 */
SNS_DDF_UIMAGE_CODE void sns_ddf_write_port_byte(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    uint8_t                 buffer,
    sns_ddf_status_e*       status)
{
  uint8_t write_count;
  uint8_t buff = buffer;

  if( status == NULL )
  {
    return;
  }
  *status = sns_ddf_write_port(handle, reg_addr, &buff, 1, &write_count);

  if(*status != SNS_DDF_SUCCESS)
    return;

  if(write_count != 1)
    *status = SNS_DDF_EBUS;
}




