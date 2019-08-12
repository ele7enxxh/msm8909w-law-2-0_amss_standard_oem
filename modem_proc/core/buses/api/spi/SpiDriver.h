#ifndef SPIDRIVER_H
#define SPIDRIVER_H

/**
@file spi.h
@brief SPI API
*/

/*===========================================================================
  Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when       who      what, where, why
--------   ---      ------------------------------------
07/10/15   ddk      Added APIs for init and deinit.
04/10/13   vmk      Initial version.

===========================================================================*/

#include "comdef.h"
#include "spi_defs.h"
#include "spi_errors.h"

// num of SPI instances that will be used
#define SPIPD_DEVICE_COUNT 2

/** @brief Devices supported by the interface. These device IDs map on to the
 *         BLSP QUP cores supported by the Hardware*/
typedef enum
{
   SPI_DEVICE_1=0,      // BLSP1 QUP1
   SPI_DEVICE_2,      // BLSP1 QUP2
   SPI_DEVICE_3,      // BLSP1 QUP3
   SPI_DEVICE_4,      // BLSP1 QUP4
   SPI_DEVICE_5,      // BLSP1 QUP5
   SPI_DEVICE_6,      // BLSP1 QUP6
   SPI_DEVICE_7,      // BLSP2 QUP1
   SPI_DEVICE_8,      // BLSP2 QUP2
   SPI_DEVICE_9,      // BLSP2 QUP3
   SPI_DEVICE_10,     // BLSP2 QUP4
   SPI_DEVICE_11,     // BLSP2 QUP5
   SPI_DEVICE_12,     // BLSP2 QUP6
   SPI_DEVICE_COUNT,
   SPI_DEVICE_INVALID = 0x7fffffff,
} spi_device_id_t;

typedef struct spi_config_t spi_config_t;
struct spi_config_t
{
   SpiClockPolarityType spi_clk_polarity;
   /**< Clock polarity  type to be used for the SPI core.*/
   SpiShiftModeType spi_shift_mode;
   /**< Shift mode(CPHA) type to be used for SPI core.*/
   SpiCSPolarityType spi_cs_polarity;
   /**< CS polarity type to be used for the SPI core.*/
   SpiCSModeType spi_cs_mode;
   /**< CS Mode to be used for the SPI core.*/
   SpiClockModeType spi_clk_always_on;
   /**< Clock mode type to be used for the SPI core.*/
   uint8 spi_bits_per_word;
   /**< SPI bits per word, any value from 3 to 31*/

   uint8 spi_slave_index;
   /**< Slave index, beginning at 0, if mulitple SPI devices are connected to the same master */
   
   uint32 min_slave_freq_hz;
   uint32 max_slave_freq_hz;
   /**< Host will set the SPI clk frequency to max_slave_freq_hz, or to the nearest 
   supported frequency greater than min_slave_freq_hz */
   
   uint32 deassertion_time_ns;
   /**< The minimum delay between two word(N-bit) transfers */

   boolean loopback_mode;
   /**<If set, SPI controller will enable loopback mode, used primarily for testing */

   /* Add BAM mode specifier? */
   /* Add transaction timeout? */
};

typedef struct
{
   void* buf_phys_addr;
   void* buf_virt_addr;
   /**<buff address for read or write data*/
   uint32 buf_len; 
   /**<size in bytes*/
   uint32 total_bytes;
   /**<total bytes successfully transfered on SPI Bus*/
}spi_transaction_t;

/**
 * Init  the device so that all SPI driver initialisation routine is completed \n
 *
 * @param[in] device_id The SPI device ID to attach to
 *
 * @return 0 on success, negative on failure
 */
int32 spi_init (spi_device_id_t device_id);

/**
 * De-init  the device so that all SPI driver de-initialisation routine is completed \n
 *
 * @param[in] device_id The SPI device ID to attach to
 *
 * @return 0 on success, negative on failure
 */
int32 spi_deinit (spi_device_id_t device_id);

/**
 * Open the device, and perform some HW intialization \n
 *
 * @param[in] device_id The SPI device ID to attach to
 *
 * @return 0 on success, negative on failure
 */
int32 spi_open (spi_device_id_t device_id);

/**
 * Reads data from SPI bus.
 *
 * @param[in] device_id The SPI device ID to attach to
 * @param[in] p_config The desired SPI configuration
 * @param[in] p_read_info Read buffer information
 *
 * @return 0 on success, negative on failure
 */
int32 spi_read (spi_device_id_t device_id,
   const spi_config_t* p_config,
   spi_transaction_t* p_read_info);

/**
 * Writes data on SPI bus.
 *
 * @param[in] device_id The SPI device ID to attach to
 * @param[in] p_config The desired SPI configuration
 * @param[in] p_write_info Write buffer information
 *
 * @return 0 on success, negative on failure
 */
int32 spi_write (spi_device_id_t device_id,
   const spi_config_t* p_config,
   spi_transaction_t* p_write_info);

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
int32 spi_full_duplex (spi_device_id_t device_id,
   const spi_config_t* p_config,
   spi_transaction_t* p_write_info,
   spi_transaction_t* p_read_info);

/**
 * Close the client access to the spi device.
 *
 * @param[in] device_id The SPI device ID 
 *
 * @return 0 on success, negative on failure
 */

int32 spi_close (spi_device_id_t device_id);

#endif //SPIDRIVER_H
