/**
 * @file sns_ddf_comm.c
 * @brief Implementation of the Device Driver Framework communication module.
 *
 * @see sns_ddf_comm.h
 *
 * Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 */

/*==============================================================================
  Edit History

  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.


  when       who  what, where, why
  ---------- ---  -----------------------------------------------------------
  2015-08-12 dk   Updated to use new malloc API
  2015-01-21 jms  SPI and BMI160 enabled on SLPI 1.0
  2014-09-02 jms  SPI refactored for uImage
  2014-08-23 vy   Used new DDF apis to allocate/free mem supported in uImage
  2014-08-14 vy   Added support for island mode msg
  2014-06-20 jms  Added SPI support
  2014-05-15 pn   Absorbed sns_ddf_uimg_comm.c
  2014-04-07 DC   Fix return code in SNS_DDF_BUS_SPI when DalDevice_Open fails
  2014-02-26 MW   Fix KW errors
  2014-01-30 lka  Added sns_ddf_read_port_u32_count()
  2014-01-28 dk   Added SPI transfer
  2014-01-27 hw   Add QDSS logging
  2013-07-05 MW	  Added single transfer (simple) I2C read and write APIs.
  2013-05-01 lka  Added IO functions for enhanced DDF interface.
  2013-04-28 vh   DDF handle validation before calling read/write
  2013-03-18 vh   Added appropriate return errors
  2013-02-14 ag   - Added support for 16-bit register addressing
                  - I2C transfer optimizations
                  - support for multiple reads without STOP
  2013-01-09 pn   Updated sns_ddf_write_port() to handle zero-byte write.
  2012-09-26 sc   Handle I2C read/write error condition (call I2C close)
  2012-09-12 sc   Call I2C open/close in each read/write operation
  2012-08-29 sc   Enabled I2C access by default
  2012-08-22 sc   Populated bus_instance[] array (based on Amolika's fix)
  2012-06-08 vh   Added run time switch for I2C code
  2012-05-17 vh   Enabled I2C functionality
  2012-05-01 vh   Modified to support latest I2C driver
  2012-02-16 yk   Added sns_ddf_write_port_byte
  2012-01-05 br   Adjust alignment
  2011-11-27 br   Deleted an unnecessary line in both ddf_read_port() and ddf_write_port()
  2011-07-19 dc   Lowercase inclusion of DALStdErr fails on linux builds.
  2011-07-01 agk  Bypassed DAL1 for i2c communication.
  2011-04-11 yk   Moved ASSERT macro from header file to here.
  2011-03-11 ag   Update path to handle building on Linux machines
  2011-01-21 sc   Modified on PCSIM support.
  2010-11-22 pg   Added raw I2C logs.
  2010-08-23 yk   Added alternate implementation for PCSim (via flag SNS_PCSIM).
  2010-08-20 yk   Added cleanup in case of failure of open_port().
  2010-08-16 yk   Initial revision
==============================================================================*/

#include "sns_ddf_comm.h"
#include "sns_profiling.h"

#if !defined (SNS_PCSIM) //------------------------------------

#include "sns_ddf_comm_priv.h"
#include "sns_profiling.h"
#include "sns_log_types.h"
#include "sns_log_api.h"
#include "sns_ddf_util.h"
#include "sns_memmgr.h"
#include "sns_debug_str.h"
#include "spi_errors.h"
#include "sns_smgr_ddf_priv.h"

/*----------------------------------------------------------------------------
 * Private Function Declarations
 * -------------------------------------------------------------------------*/

//------------------------------- I2C BUS ------------------------------------
static sns_ddf_status_e sns_ddf_comm_bus_i2c_open
(
    sns_ddf_handle_t              handle,
    const sns_ddf_port_config_s*  cfg
);
static sns_ddf_status_e sns_ddf_comm_bus_i2c_close( sns_ddf_handle_t  handle );

//------------------------------- SPI BUS ------------------------------------
static sns_ddf_status_e sns_ddf_comm_bus_spi_open
(
    sns_ddf_handle_t              handle,
    const sns_ddf_port_config_s*  cfg
);
static sns_ddf_status_e sns_ddf_comm_bus_spi_close( sns_ddf_handle_t  handle );

/*----------------------------------------------------------------------------
 * Big Image Static Function Declarations
 * -------------------------------------------------------------------------*/

static sns_ddf_status_e sns_ddf_comm_bus_i2c_set_defaults
(
    sns_ddf_device_access_s*  dev_access,
    uint32_t                  bus_slave_addr
);
static sns_ddf_status_e sns_ddf_comm_bus_spi_set_defaults
(
    sns_ddf_device_access_s*  dev_access,
    uint32_t                  bus_slave_addr
);

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
//                         PRIVATE DDF COMM UTILS
//============================================================================
//============================================================================

//============================================================================
/**
 * @brief   Allocates memory in TCM or DDR
 * @detail  Private ddf_comm function specific for ddf_comm requirements.
 *
 * @param[out] ptr   Double pointer to store allocation address.
 * @param[in]  size  Allocation size in bytes.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_ENOMEM to indicate an error has occurred.
 */
static sns_ddf_status_e sns_ddf_comm_malloc( void** ptr, uint16_t size )
{
  *ptr = SNS_OS_ANY_MALLOC(SNS_DBG_MOD_DSPS_DDF, size);
  return (*ptr != NULL) ? SNS_DDF_SUCCESS : SNS_DDF_ENOMEM;
}

//============================================================================
/**
 * @brief   Releases allocated memory.
 * @detail  Private ddf_comm function.
 *
 * @param[in/out] ptr
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 */
static sns_ddf_status_e sns_ddf_comm_mfree( void* ptr )
{
  if ( ptr == NULL )
  {
      return SNS_DDF_EFAIL;
  }
  SNS_OS_ANY_FREE(ptr);
  return SNS_DDF_SUCCESS;
}


//============================================================================
//============================================================================
//                         PUBLIC  BUS  API
//============================================================================
//============================================================================

/**
 * @brief Initializes and configures a communication port.
 *
 * @param[out] handle  Opaque I/O port handle assigned by this API. This handle
 *                     is given to drivers as part of initialization. Drivers
 *                     will pass this handle as a parameter to read/write API
 *                     calls to the port.
 * @param[in]  config  Configuration data for the port.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_open_port(
    sns_ddf_handle_t*             handle,
    const sns_ddf_port_config_s*  cfg )
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if ( cfg == NULL || handle == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  *handle = NULL;
  status  = sns_ddf_comm_malloc( (void **)handle, sizeof(sns_ddf_sensor_info_s) );
  if ( SNS_DDF_SUCCESS != status )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_DDF, "Malloc fail, size = %d",
                               sizeof(sns_ddf_sensor_info_s) );
    return status;
  }

  switch( cfg->bus )
  {
    case SNS_DDF_BUS_I2C:
      status = sns_ddf_comm_bus_i2c_open( *handle, cfg );
      break;
    case SNS_DDF_BUS_SPI:
      status = sns_ddf_comm_bus_spi_open( *handle, cfg );
      break;
    default:
      status = SNS_DDF_EINVALID_PARAM;
  }

  if ( SNS_DDF_SUCCESS != status )
  {
    SNS_PRINTF_STRING_ERROR_1(SNS_DBG_MOD_DSPS_DDF, "open_port, result = %d", status);
    sns_ddf_comm_mfree( *handle );
    *handle = NULL;
    return SNS_DDF_EBUS;
  }

  return status;
}


//============================================================================
/**
 * @brief   Closes a previously opened port.
 * @detail  Frees handler and bus configuration. The device cannot be open again.
 * @param[out] handle  I/O port handle.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
sns_ddf_status_e sns_ddf_close_port( sns_ddf_handle_t  handle )
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;
  if ( handle == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  switch( ((sns_ddf_sensor_info_s*)handle)->bus )
  {
    case SNS_DDF_BUS_I2C:
      status = sns_ddf_comm_bus_i2c_close( handle );
      break;
    case SNS_DDF_BUS_SPI:
      status = sns_ddf_comm_bus_spi_close( handle );
      break;
    default:
      status = SNS_DDF_EINVALID_PARAM;
  }

  sns_ddf_comm_mfree(handle);
  handle = NULL;
  return status;
}


//============================================================================
/**
 * @brief Get the port identifier from the port configuration
 *
 * @param[in]  port_config_ptr  The pointer to the port configuration
 * @param[out] port_id_ptr      The pointer in which the port id is saved
 *
 * @return none
 */
void sns_ddf_get_port_id(
    const   sns_ddf_port_config_s*  port_config_ptr,
    sns_ddf_port_id_s*      port_id_ptr)
{
  if ( port_config_ptr != NULL && port_id_ptr != NULL )
  {
    port_id_ptr->bus      = port_config_ptr->bus;
    port_id_ptr->bus_inst = port_config_ptr->bus_instance;

    switch( port_id_ptr->bus )
    {
      case SNS_DDF_BUS_I2C:
        if ( port_config_ptr->bus_config.i2c == NULL )
        {
          port_id_ptr->dev_addr = 0;
        } else
        {
          port_id_ptr->dev_addr = port_config_ptr->bus_config.i2c->slave_addr;
        }
        break;
      case SNS_DDF_BUS_SPI:
#if SNS_DDF_COMM_BUS_SPI_ENABLE_DRIVER

        if ( port_config_ptr->bus_config.spi == NULL )
        {
          port_id_ptr->dev_addr = 0;
        } else
        {
          port_id_ptr->dev_addr = port_config_ptr->bus_config.spi->spi_slave_index;
        }
        port_id_ptr->dev_addr = 0;
#endif
        break;
      default:
        port_id_ptr->dev_addr = 0;
    }
  }
}


//============================================================================
/**
 * @brief Determine if the port configuration and the port id are referring to
 *        the same port.
 *
 * @param[in]  port_config_ptr  The pointer to the port configuration
 * @param[in]  port_id_ptr      The pointer in which the port id is saved
 *
 * @return True if the port id of the port configuration is the same as the input port id
 */
bool sns_ddf_is_same_port_id(
    const   sns_ddf_port_config_s*  port_config_ptr,
    const   sns_ddf_port_id_s*      port_id_ptr)
{
  if ( port_config_ptr == NULL || port_id_ptr == NULL  ||
       port_id_ptr->bus      != port_config_ptr->bus   ||
       port_id_ptr->bus_inst != port_config_ptr->bus_instance )
  {
    return false;
  }

  uint32_t slave_addr = 0;
  switch( port_id_ptr->bus )
  {
    case SNS_DDF_BUS_I2C:
      slave_addr = port_config_ptr->bus_config.i2c->slave_addr;
      break;
    case SNS_DDF_BUS_SPI:
#if SNS_DDF_COMM_BUS_SPI_ENABLE_DRIVER
      slave_addr = port_config_ptr->bus_config.spi->spi_slave_index;
#endif
      break;
    default:
      SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_DDF, "UNKNOWN bus_id=%d", port_id_ptr->bus );
      return false;
  }

  if ( port_id_ptr->dev_addr != slave_addr )
  {
    return false;
  }

  return true;
}



//============================================================================
//============================================================================
//                         PRIVATE  BUS  API
//============================================================================
//============================================================================

//============================================================================
/**
* @brief  Sets a communication bus default configuration.
*
* @param[in/out] dev_access      Bus config ptr. This config
*                                is given to drivers as part of initialization.
* @param[in]     bus_instance    Configuration data for the bus address and bus type.
* @param[in]     bus_slave_addr  Configuration data for the bus slave address.
*
* @return SNS_DDF_SUCCESS if the operation was done successfully.
*         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
*         SNS_DDF_EFAIL to indicate an error has occurred.
*/

sns_ddf_status_e sns_ddf_comm_bus_init_cfg
(
    sns_ddf_device_access_s*  dev_access,
    uint16_t                  bus_instance,
    uint32_t                  bus_slave_addr
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  if ( dev_access == NULL )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  dev_access->port_config.bus            = (uint8_t)SNS_DDF_COMM_BUS_TYPE(bus_instance);
  dev_access->port_config.bus_instance   = (uint8_t)SNS_DDF_COMM_BUS_ID(bus_instance);

  switch( dev_access->port_config.bus )
  {
    case SNS_DDF_BUS_I2C:
      status = sns_ddf_comm_bus_i2c_set_defaults( dev_access, bus_slave_addr );
      break;
    case SNS_DDF_BUS_SPI:
      status = sns_ddf_comm_bus_spi_set_defaults( dev_access, bus_slave_addr );
      break;
    default:
      status = SNS_DDF_EINVALID_PARAM;
  }

  return status;
}


//============================================================================
/**
* @brief  Frees a communication bus default configuration.
*
* @param[in/out] dev_access      Bus config ptr. This config
*                                is given to drivers as part of initialization.
*
* @return SNS_DDF_SUCCESS if the operation was done successfully.
*         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
*         SNS_DDF_EFAIL to indicate an error has occurred.
*/
sns_ddf_status_e sns_ddf_comm_bus_free_cfg
(
    sns_ddf_device_access_s*  dev_access
)
{
  sns_ddf_status_e status = SNS_DDF_SUCCESS;

  switch( dev_access->port_config.bus )
  {
    case SNS_DDF_BUS_I2C:
      //i2c cfg used only to configure the port once
      sns_ddf_comm_mfree( dev_access->port_config.bus_config.i2c );
      dev_access->port_config.bus_config.i2c = NULL;
      break;
    case SNS_DDF_BUS_SPI:
      //spi cfg is used later in spi read/write
      break;
    default:
      status = SNS_DDF_EINVALID_PARAM;
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
 * @brief  Sets the default I2C bus configuration.
 *
 * @param[in/out] dev_access    Bus config ptr. This config
 *                              is given to drivers as part of initialization.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
static sns_ddf_status_e sns_ddf_comm_bus_i2c_set_defaults
(
    sns_ddf_device_access_s*  dev_access,
    uint32_t                  bus_slave_addr
)
{
  sns_ddf_i2c_config_s*  i2c_cfg;
  sns_ddf_status_e       ddf_result;

  dev_access->port_config.bus_config.i2c = NULL;

  ddf_result = sns_ddf_comm_malloc( (void **)&i2c_cfg, sizeof(sns_ddf_i2c_config_s) );
  if ( SNS_DDF_SUCCESS != ddf_result )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_DDF, "Malloc fail, size=%d",
                               sizeof(sns_ddf_i2c_config_s) );
    return ddf_result;
  }

  dev_access->port_config.bus_config.i2c = i2c_cfg;
  i2c_cfg->slave_addr       = bus_slave_addr;
  i2c_cfg->addr_type        = SNS_DDF_I2C_ADDR_7BIT;
  i2c_cfg->bus_acq_timeout  = -1;
  i2c_cfg->bus_freq         = SNS_DDF_DEFAULT_I2C_BUS_FREQ;
  i2c_cfg->dev_type         = SNS_DDF_I2C_DEVICE_REGADDR;
  i2c_cfg->read_opt         = SNS_DDF_I2C_START_BEFORE_RD;
  i2c_cfg->xfer_timeout     = -1;
  i2c_cfg->reg_addr_type    = SNS_DDF_I2C_REG_ADDR_8BIT; /* default to 8-bit register
                                                         address. Driver can override
                                                         for 16-bit addressing. */

  SNS_PRINTF_STRING_MEDIUM_2( SNS_DBG_MOD_DSPS_DDF, "bus_instance: %d slave_addr:0x%x",
                              dev_access->port_config.bus_instance,
                              (unsigned)i2c_cfg->slave_addr);
  return SNS_DDF_SUCCESS;
}

//============================================================================
/**
 * @brief Initializes and configures I2C communication bus.
 *
 * @param[out] handle  Opaque bus handle assigned by this API. This handle
 *                     is given to drivers as part of initialization. Drivers
 *                     will pass this handle as a parameter to read/write API
 *                     calls to the I2C bus.
 * @param[in]  cfg     Configuration data for the bus.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
static sns_ddf_status_e sns_ddf_comm_bus_i2c_open
(
    sns_ddf_handle_t              handle,
    const sns_ddf_port_config_s*  cfg
)
{
  //TODO: table declared twice! Check sns_smgr_hw.c I2cDrv_I2cBusId sns_i2c_bus_table[]
  static const I2cDrv_I2cBusId i2c_bus_instances[] =
  {
    0,
    I2CDRV_I2C_1,
    I2CDRV_I2C_2,
    I2CDRV_I2C_3,
    I2CDRV_I2C_4,
    I2CDRV_I2C_5,
    I2CDRV_I2C_6,
    I2CDRV_I2C_7,
    I2CDRV_I2C_8,
    I2CDRV_I2C_9,
    I2CDRV_I2C_10,
    I2CDRV_I2C_11,
    I2CDRV_I2C_12
  };
  int32                   result;
  sns_ddf_sensor_info_s*  sns_info = (sns_ddf_sensor_info_s*)handle;

  if ( cfg->bus_instance >= ARR_SIZE(i2c_bus_instances) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* Initialize member params */
  sns_info->bus = SNS_DDF_BUS_I2C;
  sns_info->i2c_s.reg_addr_type               = cfg->bus_config.i2c->reg_addr_type;
  sns_info->i2c_s.i2c_bus.clntCfg.uSlaveAddr  = cfg->bus_config.i2c->slave_addr;
  sns_info->i2c_s.i2c_bus.clntCfg.uBusFreqKhz = SNS_DDF_DEFAULT_I2C_BUS_FREQ;
  sns_info->i2c_s.i2c_bus.clntCfg.uByteTransferTimeoutUs = SNS_DDF_DEFAULT_BYTE_XFER_TMO;

  if ( EnableI2C == false )
  {
    return SNS_DDF_SUCCESS;
  }

  /* Obtain the handle for the port. */
  result = I2cDrv_Open(i2c_bus_instances[cfg->bus_instance], &sns_info->i2c_s.i2c_bus, 0);
  if ( I2C_RES_SUCCESS != result )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_DDF, "I2cDrv_Open, result = %d", result );
    return SNS_DDF_EBUS;
  }

  return SNS_DDF_SUCCESS;
}

//============================================================================
/**
 * @brief Closes a previously opened port.
 *
 * @param[out] handle  I/O port handle.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
static sns_ddf_status_e sns_ddf_comm_bus_i2c_close( sns_ddf_handle_t  handle )
{
  sns_ddf_sensor_info_s* ddf_handle = (sns_ddf_sensor_info_s *)handle;

  if ( EnableI2C == false )
  {
    return SNS_DDF_SUCCESS;
  }

  I2cDrv_Close(&ddf_handle->i2c_s.i2c_bus);
  return SNS_DDF_SUCCESS;
}


//============================================================================
//============================================================================
//                        PRIVATE  SPI  BUS
//============================================================================
//============================================================================

//============================================================================
/**
 * @brief  Sets the default SPI bus configuration.
 *
 * @param[in/out] dev_access    Bus config ptr. This config
 *                              is given to drivers as part of initialization.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
static sns_ddf_status_e sns_ddf_comm_bus_spi_set_defaults
(
    sns_ddf_device_access_s*  dev_access,
    uint32_t                  bus_slave_addr
)
{
#if SNS_DDF_COMM_BUS_SPI_ENABLE_DRIVER
  sns_ddf_spi_config_s*  spi_cfg;
  sns_ddf_status_e       ddf_result;

  dev_access->port_config.bus_config.spi = NULL;
  ddf_result = sns_ddf_comm_malloc( (void **)&spi_cfg, sizeof(sns_ddf_spi_config_s) );
  if ( SNS_DDF_SUCCESS != ddf_result )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_DDF, "Malloc fail, size=%d",
                               sizeof(sns_ddf_spi_config_s) );
    return ddf_result;
  }

  dev_access->port_config.bus_config.spi = spi_cfg;

  spi_cfg->spi_clk_polarity    = SPI_CLK_IDLE_HIGH;
  spi_cfg->spi_shift_mode      = SPI_OUTPUT_FIRST_MODE;
  spi_cfg->spi_cs_polarity     = SPI_CS_ACTIVE_LOW;
  spi_cfg->spi_cs_mode         = SPI_CS_KEEP_ASSERTED;
  spi_cfg->spi_clk_always_on   = SPI_CLK_NORMAL;

   //SPI bits per word, any value from 3 to 31
  spi_cfg->spi_bits_per_word   = (uint8)SNS_DDF_DEFAULT_SPI_BITS_PER_WORD;

  //Slave index, beginning at 0, if multiple SPI devices are connected to the same master
  spi_cfg->spi_slave_index     = (uint8)bus_slave_addr;
  spi_cfg->min_slave_freq_hz   = (uint32)SNS_DDF_DEFAULT_SPI_MIN_CLK_FRQ_HZ;
  spi_cfg->max_slave_freq_hz   = (uint32)SNS_DDF_DEFAULT_SPI_MAX_CLK_FRQ_HZ;
  spi_cfg->deassertion_time_ns = (uint32)SNS_DDF_DEFAULT_SPI_DEASSERT_TIME;

  // If set, SPI controller will enable loopback mode, used primarily for testing
  spi_cfg->loopback_mode       = (boolean)SNS_DDF_DEFAULT_SPI_LOOPBACK_MODE;


  SNS_PRINTF_STRING_MEDIUM_1( SNS_DBG_MOD_DSPS_DDF, "bus_instance: %d SPI bus",
                              dev_access->port_config.bus_instance );
#endif
  return SNS_DDF_SUCCESS;
}

//============================================================================
/**
 * @brief Initializes and configures SPI communication bus.
 *
 * @param[in/out] handle  Opaque bus handle assigned by this API. This handle
 *                        is given to drivers as part of initialization. Drivers
 *                        will pass this handle as a parameter to read/write API
 *                        calls to the SPI bus.
 * @param[in]     cfg     Configuration data for the bus.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
static sns_ddf_status_e sns_ddf_comm_bus_spi_open
(
    sns_ddf_handle_t              handle,
    const sns_ddf_port_config_s*  cfg
)
{
#if SNS_DDF_COMM_BUS_SPI_ENABLE_DRIVER
  static const spi_device_id_t spi_bus_instances[] =
  {
    0,
    SPI_DEVICE_1,
    SPI_DEVICE_2,
    SPI_DEVICE_3,
    SPI_DEVICE_4,
    SPI_DEVICE_5,
    SPI_DEVICE_6,
    SPI_DEVICE_7,
    SPI_DEVICE_8,
    SPI_DEVICE_9,
    SPI_DEVICE_10,
    SPI_DEVICE_11,
    SPI_DEVICE_12,
  };
  SPI_RESULT              result; //spi_errors.h
  sns_ddf_sensor_info_s*  sns_info = (sns_ddf_sensor_info_s*)handle;

  if ( cfg->bus_instance >= ARR_SIZE(spi_bus_instances) )
  {
    return SNS_DDF_EINVALID_PARAM;
  }

  /* Initialize member params */
  sns_info->bus          = SNS_DDF_BUS_SPI;
  sns_info->spi_s.dev_id = spi_bus_instances[cfg->bus_instance];
  sns_info->spi_s.cfg    = cfg->bus_config.spi;

  if ( EnableSPI == false )
  {
    return SNS_DDF_SUCCESS;
  }

  /* Initialize the SPI */
  spi_init(sns_info->spi_s.dev_id);
  //SNS_PRINTF_STRING_ERROR_0( SNS_DBG_MOD_DSPS_DDF, "spi_init done" );

  /* Open SPI port*/
  result = spi_open(sns_info->spi_s.dev_id);
  if ( result != SPI_OP_SUCCESS )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_DDF, "spi_open fail result=%d", result );
    return SNS_DDF_EBUS;
  }

  //TODO: fake write switching sensor to SPI mode? ------------------------

  /* Close device - this only turns the clocks off */
  result = spi_close(sns_info->spi_s.dev_id);
  if ( result != SPI_OP_SUCCESS )
  {
    SNS_PRINTF_STRING_ERROR_1( SNS_DBG_MOD_DSPS_DDF, "spi_close fail result=%d", result );
    return SNS_DDF_EBUS;
  }
#endif

  return SNS_DDF_SUCCESS;
}

//============================================================================
/**
 * @brief Closes a previously opened port.
 *
 * @param[out] handle  I/O port handle.
 *
 * @return SNS_DDF_SUCCESS if the operation was done successfully.
 *         Otherwise SNS_DDF_EDEVICE, SNS_DDF_EBUS, SNS_DDF_EINVALID_PARAM, or
 *         SNS_DDF_EFAIL to indicate an error has occurred.
 */
static sns_ddf_status_e sns_ddf_comm_bus_spi_close( sns_ddf_handle_t  handle )
{
  sns_ddf_sensor_info_s*  sns_info = (sns_ddf_sensor_info_s*)handle;

  sns_ddf_comm_mfree( sns_info->spi_s.cfg );
  sns_info->spi_s.cfg = NULL;

  return SNS_DDF_SUCCESS;
}


//============================================================================
//============================================================================


#else    //#if !defined (SNS_PCSIM) ------------------------------------
/*------------------------------------------------------------------------------
  Compiling on PCSIM...
------------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include "sns_ddf_comm.h"

sns_ddf_status_e sns_ddf_open_port(
    sns_ddf_handle_t*       handle,
    sns_ddf_port_config_s*  config)
{
   /*
      For now it is stubbed out.
      Later when we add driver-level playback support,it will call
      the playback interface to open files,etc.
      This allows centralization of playback utilities, as well as the
      flexibility to control from user shell.
   */
   return SNS_DDF_SUCCESS;
}


sns_ddf_status_e sns_ddf_close_port(sns_ddf_handle_t  handle)
{
    fclose(handle);
    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e sns_ddf_read_port(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          read_count)
{
   /* future driver-level playback will be done through playback API */
    MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "ddf_read_port");

    *read_count = bytes;
    return SNS_DDF_SUCCESS;
}


sns_ddf_status_e sns_ddf_write_port(
    sns_ddf_handle_t  handle,
    uint16_t          reg_addr,
    uint8_t*          buffer,
    uint8_t           bytes,
    uint8_t*          write_count)
{
    *write_count = bytes;
    return SNS_DDF_SUCCESS;
}


#endif

