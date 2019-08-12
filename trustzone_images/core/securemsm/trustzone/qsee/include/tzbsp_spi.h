#ifndef TZBSP_SPI_H
#define TZBSP_SPI_H

/**
@file tzbsp_spi.h
@brief Provide SPI API wrappers
*/

/*===========================================================================
  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
04/25/16   dpk      Updated SPI config structure.
07/14/14   ms       Added support for all QUP devices
05/12/13   ag       Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
 #define TZBSP_SPIPD_DEVICE_COUNT 12

/** @brief Devices supported by the interface. These device IDs map on to the
 *         BLSP QUP cores supported by the Hardware*/
typedef enum
{
  TZBSP_SPI_DEVICE_1,      /* BLSP1 QUP 1*/
  TZBSP_SPI_DEVICE_2,      /* BLSP1 QUP 2*/
  TZBSP_SPI_DEVICE_3,      /* BLSP1 QUP 3*/
  TZBSP_SPI_DEVICE_4,      /* BLSP1 QUP 4*/
  TZBSP_SPI_DEVICE_5,      /* BLSP1 QUP 5*/
  TZBSP_SPI_DEVICE_6,      /* BLSP1 QUP 6*/
  TZBSP_SPI_DEVICE_7,      /* BLSP1 QUP 1*/
  TZBSP_SPI_DEVICE_8,      /* BLSP1 QUP 2*/
  TZBSP_SPI_DEVICE_9,      /* BLSP1 QUP 3*/
  TZBSP_SPI_DEVICE_10,      /* BLSP1 QUP 4*/
  TZBSP_SPI_DEVICE_11,      /* BLSP1 QUP 5*/
  TZBSP_SPI_DEVICE_12,      /* BLSP1 QUP 6*/  
  TZBSP_SPI_DEVICE_COUNT
  /**< Used to check the bounds of this enumeration. */

} tzbsp_spi_device_id_t;


/**
 In the idle state whether the SPI clk is high or low. 
*/ 
typedef enum 
{
   TZBSP_SPI_CLK_IDLE_LOW,/**< CLK signal is low when idle.*/
   TZBSP_SPI_CLK_IDLE_HIGH/**< CLK signal is high when idle.*/
}tzbsp_spi_clock_polarity_t;

/**
 In idle state whether the Chip Select is high or low.
*/ 
typedef enum 
{
   TZBSP_SPI_CS_ACTIVE_LOW,/**< During idle state CS  line is held low*/
   TZBSP_SPI_CS_ACTIVE_HIGH/**< During idle state CS line is held high*/
}tzbsp_spi_cs_polarity_t;

/**
Shift mode(CPHA), detemines which signal (input or output) is sampled first.
*/ 
typedef enum 
{
   TZBSP_SPI_INPUT_FIRST_MODE,/**< In both Master and slave input Bit is shifted in first.*/
   TZBSP_SPI_OUTPUT_FIRST_MODE/**< In both Master and slave  output Bit is shifted in first*/
}tzbsp_spi_shift_mode_t;

/**
 List of values that inidicates, whether the Chip select is always active or if it's brought back to its idle state
 after N clock cycles.
*/ 
typedef enum 
{
   TZBSP_SPI_CS_DEASSERT,/**< CS is deasserted after transferring data for N clock cycles*/
   TZBSP_SPI_CS_KEEP_ASSERTED/**< CS is asserted as long as the core is in Run state*/
}tzbsp_spi_cs_mode_t;

/**
 SPI Clock is always on or turned off after every transaction, this enum is deprecated.
 The driver ignores the value passed.
*/ 
typedef enum 
{

   TZBSP_SPI_CLK_NORMAL, /**< Turns off SPI CLK during idle state.*/
   TZBSP_SPI_CLK_ALWAYS_ON/**< Runs SPI CLK during idle state.*/
}tzbsp_spi_clock_mode_t;
/** @brief Structure that is used for SPI configuration. */

typedef enum
{
   TZBSP_SPI_BLSP_1 = 0,
   TZBSP_SPI_BLSP_2 = 1,
   
   TZBSP_SPI_BLSP_MAX
   
}tzbsp_spi_blsp_t;

typedef struct
{
  /**< SPI clock frequency */
  uint32 max_freq;

  /**< Clock polarity  type to be used for the SPI core.*/
  tzbsp_spi_clock_polarity_t spi_clk_polarity;

  /**< Shift mode(CPHA) type to be used for SPI core.*/
  tzbsp_spi_shift_mode_t spi_shift_mode;

  /**< CS polarity type to be used for the SPI core.*/
  tzbsp_spi_cs_polarity_t spi_cs_polarity;

  /**< CS Mode to be used for the SPI core.*/
  tzbsp_spi_cs_mode_t spi_cs_mode;

  /**< Clock mode type to be used for the SPI core.*/
  tzbsp_spi_clock_mode_t spi_clk_always_on;

  /**< SPI bits per word, any value from 3 to 31*/
  uint8 spi_bits_per_word;

  /**< Put the Device in High Performance */
  uint8 hs_mode;

  /**< Put the Device in loop back test*/
  uint8 loopback;
   
  /**< Slave index from 0 to 3, if mulitple SPI devices are connected to the same master */
  uint8 spi_slave_index;
  
  /**< The minimum delay between two word(N-bit) transfers */
  uint32 deassertion_time_ns;
}tzbsp_spi_config_t;

typedef struct
{
 void* buf_addr;
 /**<buff address for read or write data*/
 uint32 buf_len; 
 /**<size in bytes*/
 uint32 total_bytes;
 /**<total bytes successfully transfered on SPI Bus*/
}tzbsp_spi_transaction_info_t;

/**
 * Open the device, and perform some HW intialization \n
 *
 * @param[in] device_id The SPI device ID to attach to
 *
 * @return 0 on success, negative on failure
 */
int tzbsp_spi_open(tzbsp_spi_device_id_t device_id);

/**
 * Reads data from SPI bus.
 *
 * @param[in] device_id The SPI device ID to attach to
 * @param[in] p_config The desired SPI configuration
 * @param[in] p_read_info Read buffer information
 *
 * @return 0 on success, negative on failure
 */
int tzbsp_spi_read(
	tzbsp_spi_device_id_t device_id,
	const tzbsp_spi_config_t* p_config,
	tzbsp_spi_transaction_info_t* p_read_info
);	

/**
 * Writes data on SPI bus.
 *
 * @param[in] device_id The SPI device ID to attach to
 * @param[in] p_config The desired SPI configuration
 * @param[in] p_write_info Write buffer information
 *
 * @return 0 on success, negative on failure
 */
int tzbsp_spi_write(
	tzbsp_spi_device_id_t device_id,
	const tzbsp_spi_config_t* p_config,
	tzbsp_spi_transaction_info_t* p_write_info
);

/**
 * Writes data on SPI bus.
 *
 * @param[in] device_id The SPI device ID to attach to
 * @param[in] p_config The desired SPI configuration
 * @param[in] p_write_info Write buffer information
 * @param[in] p_read_info Read buffer information
 *
 * @return 0 on success, negative on failure
 */
int tzbsp_spi_full_duplex(
	tzbsp_spi_device_id_t device_id,
	const tzbsp_spi_config_t* p_config,
	tzbsp_spi_transaction_info_t* p_write_info,
	tzbsp_spi_transaction_info_t* p_read_info
);

/**
 * Close the client access to the spi device.
 *
 *
 * @param[in] device_id The SPI device ID 
 *
 * @return 0 on success, negative on failure
 */

int tzbsp_spi_close(tzbsp_spi_device_id_t device_id);

#endif
