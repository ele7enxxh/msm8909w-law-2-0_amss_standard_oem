/**
 * @file sns_ddf_comm.c
 * @brief Implementation of the Device Driver Framework communication module.
 *
 * @see sns_ddf_comm.h
 *
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.


  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2014-09-02 jms  I2C and SPI refactored for uImage
==============================================================================*/

#include "sns_profiling.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_ddf_util.h"
#include "sns_memmgr.h"
#include "sns_debug_str.h"

#include "sns_ddf_comm_priv.h"

/*----------------------------------------------------------------------------
 * Extern Variables
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Private Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Big Image Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Private Structure Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Public Global Variables
 * -------------------------------------------------------------------------*/

#ifdef VIRTIO_BRINGUP
volatile bool EnableI2C = false;
#else
volatile bool EnableI2C = true;
#endif

/*----------------------------------------------------------------------------
 * Local Variables
 * -------------------------------------------------------------------------*/

//============================================================================
//============================================================================
//                         PUBLIC  I2C  BUS
//============================================================================
//============================================================================

/**
 * @brief Performs a bus read operation of multiple bytes using
 *        repeated start
 *
 * @param[in]  handle      The I/O port handle that identifies a device. This is
 *                         given to drivers as part of initialization.
 * @param[in]  reg_addr    Address of the register to read on the device.
 * @param[out] buffer      Location where result will be stored. Note size of
 *                         this buffer must >= "bytes" argument.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 *
 * @return SNS_DDF_SUCCESS if the open operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_read_port_sr(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    uint8_t*                buffer,
    uint8_t                 bytes,
    uint8_t*                read_count)
{
  if (handle == NULL)
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "handle is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }

  if ((EnableI2C == true) &&
      (((sns_ddf_sensor_info_s *)handle)->bus == SNS_DDF_BUS_I2C))
  {
    /* Need a driver to test batch transfers, until then fail the return */
    return(SNS_DDF_EFAIL);
  }
  else
  {
    *read_count = bytes;
  }

  return SNS_DDF_SUCCESS;
}


/**
* @brief Set address type of device
*
* @param[in/out]  port_config_ptr  The pointer to the port configuration
* @param[in]  addr_type      The address type to be set
*
* @param[out] status       SNS_DDF_SUCCESS if the setting bus address type
*  	   operation was done successfully. Otherwise
*  	   SNS_DDF_EINVALID_PARAM, or SNS_DDF_EFAIL to indicate
*  	   an error has occurred
*/
sns_ddf_status_e sns_ddf_set_i2c_port_addr_type (
    sns_ddf_port_config_s*     port_config_ptr,
    sns_ddf_port_addr_type_e   addr_type  )
{
  if(port_config_ptr != NULL)
  {
    port_config_ptr->bus_config.i2c->addr_type = addr_type;
    return SNS_DDF_SUCCESS;
  }

  return SNS_DDF_EFAIL;
}


/**
* @brief Performs a bus read operation with data sequence:
*	[S] [Addr+R] [ACK] [Device_data_MSB] [ACK] .... [Device_data_LSB] [NACK] [P]
*
* @param[in]  handle      The I/O port handle that identifies a device. This is
*                         given to drivers as part of initialization.
* @param[out] buffer      Location where result will be stored. Note size of
*                         this buffer must >= @a bytes argument.
* @param[in]  bytes       Number of bytes to read.
* @param[out] read_count  Number of bytes actually read into the buffer.
*
* @return SNS_DDF_SUCCESS if the open operation was done successfully.
*         Otherwise
*			SNS_DDF_EBUS - Failure in DAL device open or read
*			SNS_DDF_EINVALID_PARAM - NULL input params
**/
sns_ddf_status_e sns_ddf_simple_read_port(
    sns_ddf_handle_t  handle,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          read_count)
{
  int32                   result;
  I2cSimpleTrObj          tr_obj;
  sns_ddf_status_e        status     = SNS_DDF_SUCCESS;
  uint32_t                data_bytes = 0;
  sns_ddf_sensor_info_s*  ddf_handle = (sns_ddf_sensor_info_s *)handle;

  if( NULL == ddf_handle || NULL == buffer || NULL == read_count )
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "Input param is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( true == EnableI2C )
  {
    /* Init read count */
    *read_count = 0;

    I2c_TrObj_Init(&tr_obj.aTr[0],&tr_obj.io_vec[0], I2cTranDirIn, I2cTrCtxNotASequence);
    I2c_TrObj_AddBuffer(&tr_obj.aTr[0], 0, buffer, bytes);

    /* Start a I2C read transfer */
    result = I2cDrv_Read(&ddf_handle->i2c_s.i2c_bus, &tr_obj.aTr[0], &data_bytes);
    if ( I2C_RES_SUCCESS != result )
    {
      SNS_PRINTF_STRING_ERROR_2( SNS_DBG_MOD_DSPS_DDF,
                                 "I2C_Read fail, result = %d, read_count = %d",
                                 result, data_bytes );
      status = SNS_DDF_EBUS;
    }

    *read_count = (uint8_t)data_bytes;
  }
  else
  {
    *read_count = bytes;
  }

  return status;
}


/**
* @brief Performs a bus write operation with data sequence:
*  [S] [Addr+W] [ACK] [Master_data_MSB] [ACK] .... [Master_data_LSB] [ACK] [P]
*
* @param[in]  handle       The I/O port handle that identifies a device. This
*                          is given to drivers as part of initialization.
* @param[out] buffer       Location where value to be written is stored. Note
*                          that size of this buffer must >= @a bytes argument.
* @param[in]  bytes        Number of bytes to write.
* @param[out] write_count  Number of bytes actually written from the buffer.
*
* @return SNS_DDF_SUCCESS if the write operation was done
*  	   successfully. Otherwise
*				SNS_DDF_EINVALID_PARAM - Input param is NULL
*				SNS_DDF_EBUS - Failure in DAL device open or write
**/
sns_ddf_status_e sns_ddf_simple_write_port(
    sns_ddf_handle_t  handle,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          write_count)
{
  int32                   result;
  I2cSimpleTrObj          tr_obj;
  sns_ddf_status_e        status     = SNS_DDF_SUCCESS;
  uint32_t                data_bytes = 0;
  sns_ddf_sensor_info_s*  ddf_handle = (sns_ddf_sensor_info_s *)handle;

  if( (NULL == ddf_handle) || (NULL == buffer) || (NULL == write_count) )
  {
    SNS_PRINTF_STRING_ERROR_0(SNS_DBG_MOD_DSPS_DDF, "Input param is NULL");
    return SNS_DDF_EINVALID_PARAM;
  }

  if ( true == EnableI2C )
  {
    /* Init write count */
    *write_count = 0;

    I2c_TrObj_Init(&tr_obj.aTr[0],&tr_obj.io_vec[0], I2cTranDirOut, I2cTrCtxNotASequence);
    I2c_TrObj_AddBuffer(&tr_obj.aTr[0], 0, buffer, bytes);


    /* Start a I2C read transfer */
    result = I2cDrv_Write(&ddf_handle->i2c_s.i2c_bus, &tr_obj.aTr[0], &data_bytes);
    if ( I2C_RES_SUCCESS != result )
    {
      SNS_PRINTF_STRING_ERROR_2( SNS_DBG_MOD_DSPS_DDF,
                                 "I2C_Write fail, result = %d, read_count = %d",
                                 result, data_bytes );

      status = SNS_DDF_EBUS;
    }

    *write_count = data_bytes;
  }
  else
  {
    *write_count = bytes;
  }

  return status;
}


//============================================================================
//============================================================================
//                         PRIVATE  I2C  BUS
//============================================================================
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
 *                         this buffer must >= @a bytes argument.
 * @param[in]  bytes       Number of bytes to read.
 * @param[out] read_count  Number of bytes actually read into the buffer.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_comm_bus_i2c_read
(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    uint8_t*                buffer,
    uint32_t                bytes,
    uint32_t*               read_count
)
{
  int32                   result;
  I2cIoResult             ioRes;
  I2cRegSeqObj            i2c_reg_obj;
  sns_ddf_sensor_info_s*  ddf_handle = (sns_ddf_sensor_info_s *)handle;

  if ( EnableI2C == false )
  {
    *read_count = bytes;
    return SNS_DDF_SUCCESS;
  }

  I2c_RegObj_InitRead(&i2c_reg_obj);

  if ( ddf_handle->i2c_s.reg_addr_type == SNS_DDF_I2C_REG_ADDR_16BIT )
  {
    I2c_RegObj_SetOffset(&i2c_reg_obj, reg_addr, I2cRegOffset_16Bit);
  }
  else
  {
    I2c_RegObj_SetOffset(&i2c_reg_obj, reg_addr, I2cRegOffset_8Bit);
  }

  I2c_RegObj_SetReadIoVector(&i2c_reg_obj, buffer, bytes);

  /* Issue Batch Transfer */
  result = I2cDrv_BatchTransfer(&ddf_handle->i2c_s.i2c_bus, &i2c_reg_obj.seq, &ioRes);

  if ( (result != I2C_RES_SUCCESS) && (ioRes.uInByteCnt != bytes) )
  {
    SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                              "I2C_BatchTransfer fail, result = %d",
                              result);

    return SNS_DDF_EBUS;
  }

  *read_count = bytes;

  return SNS_DDF_SUCCESS;
}

//============================================================================
/**
 * @brief Performs a bus write operation.
 *
 * @param[in]  handle       The I/O port handle that identifies a device. This
 *                          is given to drivers as part of initialization.
 * @param[in]  reg_addr     Address of the register to write on the device.
 * @param[in]  buffer       Location where value to be written is stored. Note
 *                          that size of this buffer must >= @a bytes argument.
 * @param[in]  bytes        Number of bytes to write.
 * @param[out] write_count  Number of bytes actually written from the buffer.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_comm_bus_i2c_write
(
    const sns_ddf_handle_t  handle,
    uint16_t                reg_addr,
    const uint8_t*          buffer,
    uint32_t                bytes,
    uint32_t*               write_count
)
{
  int32                   result;
  I2cIoResult             ioRes;
  I2cRegSeqObj            i2c_reg_obj;
  sns_ddf_sensor_info_s*  ddf_handle = (sns_ddf_sensor_info_s *) handle;

  if ( EnableI2C == false )
  {
    *write_count = bytes;
    return SNS_DDF_SUCCESS;
  }

  I2c_RegObj_InitWrite(&i2c_reg_obj);

  if (ddf_handle->i2c_s.reg_addr_type == SNS_DDF_I2C_REG_ADDR_16BIT)
  {
    I2c_RegObj_SetOffset(&i2c_reg_obj, reg_addr, I2cRegOffset_16Bit);
  }
  else
  {
    I2c_RegObj_SetOffset(&i2c_reg_obj, reg_addr, I2cRegOffset_8Bit);
  }

  I2c_RegObj_SetWriteIoVector(&i2c_reg_obj, (uint8_t*)buffer, bytes);

  /* issue I2C write command */
  result = I2cDrv_BatchTransfer(&ddf_handle->i2c_s.i2c_bus, &i2c_reg_obj.seq, &ioRes);
  if ( result != I2C_RES_SUCCESS )
  {
    SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF,
                              "I2C_BatchTransfer fail, result = %d",
                              result);
    return SNS_DDF_EBUS;
  }

  *write_count = bytes;

  return SNS_DDF_SUCCESS;
}




