/*=======================================================================
 * FILE:        flash_extdrv_spi_v2.c
 *
 * DESCRIPTION
 *     SPI driver for use with flash drivers to drive SPI flash.
 *     Its expected that the CMM scripts would setup the Clock.
 *
 * Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=======================================================================*/

/*=======================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *   $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/flash/src/extdrv/config_v2/flash_extdrv_spi_v2.c#1 $
 *   $DateTime: 2015/09/01 00:30:35 $
 *   $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 10/24/12     eo      Corrected GPIO settings for SPI NOR support.
 * 09/07/12     eo      Removed HCLK register references for BLSP
 * 08/21/12     sb      Add BLSP config for SPI NOR tools support on 9x25
 * 08/25/11     sv      Move Clock branch enable to cmm script
 * 07/06/11     sv      Initial version
 *=======================================================================*/

#include "flash_extdrv_spi.h"
#include "string.h"
#include "msmhwiobase.h"

#define TLMM_CSR_REG_BASE                 (TLMM_BASE + 0x00010000)
#define HWIO_GPIO_CFGn_ADDR(c) \
  (TLMM_CSR_REG_BASE + 0x00001000 + 0x10 * (c))

/* GPIO pins (4,5,6 and 7) for SPI NOR */
#define SPI_GPIO_CFG_CLK_ADDR              HWIO_GPIO_CFGn_ADDR(7)
#define SPI_GPIO_CFG_CS_ADDR               HWIO_GPIO_CFGn_ADDR(6)
#define SPI_GPIO_CFG_MISO_ADDR             HWIO_GPIO_CFGn_ADDR(5)
#define SPI_GPIO_CFG_MOSI_ADDR             HWIO_GPIO_CFGn_ADDR(4)

#define SPI_GPIO_CFG_CLK \
  *((volatile uint32 *)SPI_GPIO_CFG_CLK_ADDR)
#define SPI_GPIO_CFG_CS \
  *((volatile uint32 *)SPI_GPIO_CFG_CS_ADDR)
#define SPI_GPIO_CFG_MISO \
  *((volatile uint32 *)SPI_GPIO_CFG_MISO_ADDR)
#define SPI_GPIO_CFG_MOSI \
  *((volatile uint32 *)SPI_GPIO_CFG_MOSI_ADDR)	  

#define SPI_IO_PULL_UP_8mA_OE_MODE                         0x2C5
#define SPI_IO_PULL_UP_8mA_IE_MODE                         0xC5

/* The following configs need to be checked and updated for 9x25.
 * SPI: BLSP1 */
#define BLSP1_QUP0_REG_BASE              (COPSS_ELAN_BASE + 0x123000)
#define BLSP1_QUP1_REG_BASE              (COPSS_ELAN_BASE + 0x124000)
#define BLSP1_QUP2_REG_BASE              (COPSS_ELAN_BASE + 0x125000)
#define BLSP1_QUP3_REG_BASE              (COPSS_ELAN_BASE + 0x126000)
#define BLSP1_QUP4_REG_BASE              (COPSS_ELAN_BASE + 0x127000)
#define BLSP1_QUP5_REG_BASE              (COPSS_ELAN_BASE + 0x128000)

/* BLSP1_QUP0 */
#define HWIO_BLSP1_QUP0_CONFIG_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x00000000)
#define HWIO_BLSP1_QUP0_STATE_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x00000004)
#define HWIO_BLSP1_QUP0_IO_MODES_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x00000008)
#define HWIO_BLSP1_QUP0_SW_RESET_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x0000000c)
#define HWIO_BLSP1_QUP0_OPERATIONAL_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x00000018)
#define HWIO_BLSP1_QUP0_ERROR_FLAGS_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x0000001c)
#define HWIO_BLSP1_QUP0_ERROR_FLAGS_EN_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x00000020)
#define HWIO_BLSP1_QUP0_OUTPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP0_REG_BASE + 0x00000110 + 4 * (c))
#define HWIO_BLSP1_QUP0_INPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP0_REG_BASE + 0x00000218 + 4 * (c))
#define HWIO_BLSP1_QUP0_SPI_CONFIG_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x00000300)
#define HWIO_BLSP1_QUP0_SPI_IO_CONTROL_ADDR \
    (BLSP1_QUP0_REG_BASE + 0x00000304)

/* BLSP1_QUP1 */
#define HWIO_BLSP1_QUP1_CONFIG_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x00000000)
#define HWIO_BLSP1_QUP1_STATE_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x00000004)
#define HWIO_BLSP1_QUP1_IO_MODES_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x00000008)
#define HWIO_BLSP1_QUP1_SW_RESET_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x0000000c)
#define HWIO_BLSP1_QUP1_OPERATIONAL_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x00000018)
#define HWIO_BLSP1_QUP1_ERROR_FLAGS_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x0000001c)
#define HWIO_BLSP1_QUP1_ERROR_FLAGS_EN_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x00000020)
#define HWIO_BLSP1_QUP1_OUTPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP1_REG_BASE + 0x00000110 + 4 * (c))
#define HWIO_BLSP1_QUP1_INPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP1_REG_BASE + 0x00000218 + 4 * (c))
#define HWIO_BLSP1_QUP1_SPI_CONFIG_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x00000300)
#define HWIO_BLSP1_QUP1_SPI_IO_CONTROL_ADDR \
    (BLSP1_QUP1_REG_BASE + 0x00000304)

/* BLSP1_QUP2 */
#define HWIO_BLSP1_QUP2_CONFIG_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x00000000)
#define HWIO_BLSP1_QUP2_STATE_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x00000004)
#define HWIO_BLSP1_QUP2_IO_MODES_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x00000008)
#define HWIO_BLSP1_QUP2_SW_RESET_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x0000000c)
#define HWIO_BLSP1_QUP2_OPERATIONAL_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x00000018)
#define HWIO_BLSP1_QUP2_ERROR_FLAGS_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x0000001c)
#define HWIO_BLSP1_QUP2_ERROR_FLAGS_EN_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x00000020)
#define HWIO_BLSP1_QUP2_OUTPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP2_REG_BASE + 0x00000110 + 4 * (c))
#define HWIO_BLSP1_QUP2_INPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP2_REG_BASE + 0x00000218 + 4 * (c))
#define HWIO_BLSP1_QUP2_SPI_CONFIG_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x00000300)
#define HWIO_BLSP1_QUP2_SPI_IO_CONTROL_ADDR \
    (BLSP1_QUP2_REG_BASE + 0x00000304)

/* BLSP1_QUP3 */
#define HWIO_BLSP1_QUP3_CONFIG_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x00000000)
#define HWIO_BLSP1_QUP3_STATE_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x00000004)
#define HWIO_BLSP1_QUP3_IO_MODES_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x00000008)
#define HWIO_BLSP1_QUP3_SW_RESET_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x0000000c)
#define HWIO_BLSP1_QUP3_OPERATIONAL_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x00000018)
#define HWIO_BLSP1_QUP3_ERROR_FLAGS_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x0000001c)
#define HWIO_BLSP1_QUP3_ERROR_FLAGS_EN_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x00000020)
#define HWIO_BLSP1_QUP3_OUTPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP3_REG_BASE + 0x00000110 + 4 * (c))
#define HWIO_BLSP1_QUP3_INPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP3_REG_BASE + 0x00000218 + 4 * (c))
#define HWIO_BLSP1_QUP3_SPI_CONFIG_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x00000300)
#define HWIO_BLSP1_QUP3_SPI_IO_CONTROL_ADDR \
    (BLSP1_QUP3_REG_BASE + 0x00000304)

/* BLSP1_QUP4 */
#define HWIO_BLSP1_QUP4_CONFIG_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x00000000)
#define HWIO_BLSP1_QUP4_STATE_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x00000004)
#define HWIO_BLSP1_QUP4_IO_MODES_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x00000008)
#define HWIO_BLSP1_QUP4_SW_RESET_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x0000000c)
#define HWIO_BLSP1_QUP4_OPERATIONAL_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x00000018)
#define HWIO_BLSP1_QUP4_ERROR_FLAGS_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x0000001c)
#define HWIO_BLSP1_QUP4_ERROR_FLAGS_EN_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x00000020)
#define HWIO_BLSP1_QUP4_OUTPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP4_REG_BASE + 0x00000110 + 4 * (c))
#define HWIO_BLSP1_QUP4_INPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP4_REG_BASE + 0x00000218 + 4 * (c))
#define HWIO_BLSP1_QUP4_SPI_CONFIG_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x00000300)
#define HWIO_BLSP1_QUP4_SPI_IO_CONTROL_ADDR \
    (BLSP1_QUP4_REG_BASE + 0x00000304)

/* BLSP1_QUP5 */
#define HWIO_BLSP1_QUP5_CONFIG_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x00000000)
#define HWIO_BLSP1_QUP5_STATE_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x00000004)
#define HWIO_BLSP1_QUP5_IO_MODES_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x00000008)
#define HWIO_BLSP1_QUP5_SW_RESET_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x0000000c)
#define HWIO_BLSP1_QUP5_OPERATIONAL_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x00000018)
#define HWIO_BLSP1_QUP5_ERROR_FLAGS_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x0000001c)
#define HWIO_BLSP1_QUP5_ERROR_FLAGS_EN_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x00000020)
#define HWIO_BLSP1_QUP5_OUTPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP5_REG_BASE + 0x00000110 + 4 * (c))
#define HWIO_BLSP1_QUP5_INPUT_FIFOc_ADDR(c) \
    (BLSP1_QUP5_REG_BASE + 0x00000218 + 4 * (c))
#define HWIO_BLSP1_QUP5_SPI_CONFIG_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x00000300)
#define HWIO_BLSP1_QUP5_SPI_IO_CONTROL_ADDR \
    (BLSP1_QUP5_REG_BASE + 0x00000304)
 
struct spi_registers
{
  volatile uint32* QUP_OUTPUT_FIFO;
  volatile uint32* QUP_INPUT_FIFO;
  volatile uint32* QUP_CONFIG;   
  volatile uint32* SPI_CONFIG;
  volatile uint32* SPI_IO_CONTROL;
  volatile uint32* QUP_IO_MODES;
  volatile uint32* QUP_SW_RESET;
  volatile uint32* QUP_OPERATIONAL;
  volatile uint32* QUP_STATE;
  volatile uint32* QUP_ERROR_FLAGS;
  volatile uint32* QUP_ERROR_FLAGS_EN;
};

struct spi_registers gsbi_ports[] = 
{
  {
    (volatile uint32*)HWIO_BLSP1_QUP0_OUTPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP0_INPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP0_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP0_SPI_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP0_SPI_IO_CONTROL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP0_IO_MODES_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP0_SW_RESET_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP0_OPERATIONAL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP0_STATE_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP0_ERROR_FLAGS_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP0_ERROR_FLAGS_EN_ADDR,
  },
  {
    (volatile uint32*)HWIO_BLSP1_QUP1_OUTPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP1_INPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP1_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP1_SPI_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP1_SPI_IO_CONTROL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP1_IO_MODES_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP1_SW_RESET_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP1_OPERATIONAL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP1_STATE_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP1_ERROR_FLAGS_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP1_ERROR_FLAGS_EN_ADDR,
  },
  {
    (volatile uint32*)HWIO_BLSP1_QUP2_OUTPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP2_INPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP2_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP2_SPI_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP2_SPI_IO_CONTROL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP2_IO_MODES_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP2_SW_RESET_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP2_OPERATIONAL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP2_STATE_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP2_ERROR_FLAGS_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP2_ERROR_FLAGS_EN_ADDR,
  },
  {
    (volatile uint32*)HWIO_BLSP1_QUP3_OUTPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP3_INPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP3_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP3_SPI_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP3_SPI_IO_CONTROL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP3_IO_MODES_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP3_SW_RESET_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP3_OPERATIONAL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP3_STATE_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP3_ERROR_FLAGS_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP3_ERROR_FLAGS_EN_ADDR,
  },
  {
    (volatile uint32*)HWIO_BLSP1_QUP4_OUTPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP4_INPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP4_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP4_SPI_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP4_SPI_IO_CONTROL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP4_IO_MODES_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP4_SW_RESET_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP4_OPERATIONAL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP4_STATE_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP4_ERROR_FLAGS_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP4_ERROR_FLAGS_EN_ADDR,
  },
  {
    (volatile uint32*)HWIO_BLSP1_QUP5_OUTPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP5_INPUT_FIFOc_ADDR(0),
    (volatile uint32*)HWIO_BLSP1_QUP5_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP5_SPI_CONFIG_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP5_SPI_IO_CONTROL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP5_IO_MODES_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP5_SW_RESET_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP5_OPERATIONAL_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP5_STATE_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP5_ERROR_FLAGS_ADDR,
    (volatile uint32*)HWIO_BLSP1_QUP5_ERROR_FLAGS_EN_ADDR,
  },
};

#define SPI_IO_CONTROL_CLOCK_IDLE_HIGH     (1 << 10)
#define QUP_STATE_VALID                    (1 << 2)

#define QUP_OUTPUT_FIFO_NOT_EMPTY          (1 << 4)
#define QUP_INPUT_FIFO_NOT_EMPTY           (1 << 5)

#define QUP_OUTPUT_FIFO_FULL               (1 << 6)
#define QUP_INPUT_FIFO_FULL                (1 << 7)

#define QUP_DATA_AVAILABLE_FOR_READ(s)     ((s) & QUP_INPUT_FIFO_NOT_EMPTY)
#define QUP_OUTPUT_FIFO_EMPTY(s)           (!((s) & QUP_OUTPUT_FIFO_NOT_EMPTY))
#define QUP_OUT_FIFO_FULL(s)               ((s) & QUP_OUTPUT_FIFO_FULL)

#define QUP_IO_MODES_INPUT_MODE_FIFO       (0 << 12)
#define QUP_IO_MODES_INPUT_MODE_BLOCK      (1 << 12)
#define QUP_IO_MODES_INPUT_MODE_DM         (2 << 12)

#define QUP_IO_MODES_OUTPUT_MODE_FIFO      (0 << 10)
#define QUP_IO_MODES_OUTPUT_MODE_BLOCK     (1 << 10)
#define QUP_IO_MODES_OUTPUT_MODE_DM        (2 << 10)

#define QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN   (1 << 16)
#define QUP_IO_MODES_PACK_EN               (1 << 15)
#define QUP_IO_MODES_UNPACK_EN             (1 << 14)

#define QUP_CONFIG_INPUT_ONLY_MODE         (1 << 6)
#define QUP_CONFIG_OUTPUT_ONLY_MODE        (2 << 6)
#define QUP_CONFIG_INPUT_OUTPUT_MODE       0
#define QUP_CONFIG_INPUT_OUTPUT_MODE_MASK  (3 << 6)
// check below is needed
#define QUP_CONFIG_MINI_CORE_SPI           (1 << 8)

#define SPI_INPUT_FIRST_MODE               (1 << 9)

#define QUP_ERROR_INPUT_OVER_RUN_ERR       (1 << 2)
#define QUP_ERROR_INPUT_UNDER_RUN_ERR      (1 << 4)
#define QUP_ERROR_OUTPUT_OVER_RUN_ERR      (1 << 5)
#define QUP_ERROR_TIME_OUT_ERR             (1 << 6)
#define QUP_ERROR_ALL_ERRORS               (QUP_ERROR_INPUT_OVER_RUN_ERR   | \
                                            QUP_ERROR_INPUT_UNDER_RUN_ERR  | \
                                            QUP_ERROR_OUTPUT_OVER_RUN_ERR  | \
                                            QUP_ERROR_TIME_OUT_ERR)

#define reverse_endian(x)                  ((((x) & 0xFF) << 24)        | \
                                            (((x) & 0xFF00) << 8)       | \
                                            (((x) & 0xFF0000) >> 8)     | \
                                            (((x) & 0xFF000000) >> 24))

#define QUP_STATE_RUN_STATE_VAL                  0x1
#define QUP_STATE_RESET_STATE_VAL                0x0

#define MAX_BITS_SUPPORTED                 32

enum spi_core_state
{
  SPI_CORE_RESET = 0,
  SPI_CORE_RUNNING,
  SPI_CORE_PAUSED,
  SPI_CORE_STATE_UNKNOWN = 0x7FFFFFFF       
};

#define SPI_SIGNATURE   0xA1B2C3D4

struct spi_handle{
  uint32 signature;
  struct spi_registers gsbi_port;
  enum spi_data_width data_width;
  enum return_data_option data_ret_option;
  enum spi_mode mode;
  enum spi_core_state core_state;
  uint32 last_error_flags;
  int initialized;
};

static struct spi_handle ghandle;

enum in_out
{
  IN_ONLY,
  OUT_ONLY,
  IN_OUT_BOTH,
  IN_OUT_UNKNOWN = 0x7FFFFFFF
};

enum io_mode
{
  FIFO_MODE,
  BLOCK_MODE,
  IO_MODE_UNKNOWN = 0x7FFFFFFF
};

#define INVALID_HANDLE_VALUE         0x0

#define INITIAL_FIRST_VERSION        0x101

#define CURRENT_DRIVER_VERSION       INITIAL_FIRST_VERSION

#if (CURRENT_DRIVER_VERSION!=SPI_INTERFACE_VERSION)
  #error "Interface and implementation versions differ, update header file..."
#endif

/* Enable optimized reads */
//#define ENABLE_32BIT_OPTIMIZED_READ
#undef ENABLE_32BIT_OPTIMIZED_READ
/* Disable optimized writes, since flash device has some problems with this */
#undef  ENABLE_32BIT_OPTIMIZED_WRITE

/******************************************************************
 *
 *                 Internal helper functions
 *
 *****************************************************************/
static uint32
get_fifo_status (spi_handle_t handle)
{
  return *handle->gsbi_port.QUP_OPERATIONAL;
}

static void
enable_errors (spi_handle_t handle, uint32 error_flag_mask)
{
  error_flag_mask &= QUP_ERROR_ALL_ERRORS;

  *handle->gsbi_port.QUP_ERROR_FLAGS_EN |= error_flag_mask;
}

static void
clear_errors (spi_handle_t handle, uint32 error_flag_mask)
{
  error_flag_mask &= QUP_ERROR_ALL_ERRORS;

  *handle->gsbi_port.QUP_ERROR_FLAGS |= error_flag_mask;
}

static uint32
get_error_flag (spi_handle_t handle, uint32 error_flag_mask)
{
  error_flag_mask &= QUP_ERROR_ALL_ERRORS;

  return (*handle->gsbi_port.QUP_ERROR_FLAGS & error_flag_mask);
}


/* This should not be changed when the core is running.
 * Only do it when the core is in RESET state */
static int
config_direction (spi_handle_t handle, enum in_out tx_type)
{
  uint32 qup_conf;

  if (handle->core_state != SPI_CORE_RESET)
    return SPI_DEVICE_FAIL;

  qup_conf = *handle->gsbi_port.QUP_CONFIG;

  qup_conf = qup_conf & (~QUP_CONFIG_INPUT_OUTPUT_MODE_MASK);

  switch (tx_type)
  {
    case IN_ONLY:
      qup_conf = qup_conf | QUP_CONFIG_INPUT_ONLY_MODE;
      break;

    case OUT_ONLY:
      qup_conf = qup_conf | QUP_CONFIG_OUTPUT_ONLY_MODE;
      break;

    case IN_OUT_BOTH:
      qup_conf = qup_conf | QUP_CONFIG_INPUT_OUTPUT_MODE;
      break;

    default:
      return SPI_DEVICE_FAIL;
  }

  *handle->gsbi_port.QUP_CONFIG = qup_conf;

  return SPI_DEVICE_DONE;
}

static void
spi_core_reset (spi_handle_t handle)
{
  // Wait until the state valid flag is set
  while ((*handle->gsbi_port.QUP_STATE & QUP_STATE_VALID) == 0)
    ;

  // Put SPI core in Reset state
  *handle->gsbi_port.QUP_STATE = 
    (*handle->gsbi_port.QUP_STATE & 0xFFFFFFFC) | QUP_STATE_RESET_STATE_VAL;

  handle->core_state = SPI_CORE_RESET;
}

static void
spi_core_run (spi_handle_t handle)
{
  // Wait until the state valid flag is set
  while ((*handle->gsbi_port.QUP_STATE & QUP_STATE_VALID) == 0)
    ;

  // Put SPI core in running state
  *handle->gsbi_port.QUP_STATE = 
    (*handle->gsbi_port.QUP_STATE & 0xFFFFFFFC) | QUP_STATE_RUN_STATE_VAL;

  handle->core_state = SPI_CORE_RUNNING;
}

static void
set_data_width (spi_handle_t handle, uint32 bits)
{
  *handle->gsbi_port.QUP_CONFIG = 
    (*handle->gsbi_port.QUP_CONFIG & ~(MAX_BITS_SUPPORTED - 1)) | (bits - 1);
}

static int
configure_spi_core (spi_handle_t handle)
{
  uint32 clk_idle_state = 1, input_first_mode = 0;

  *handle->gsbi_port.QUP_SW_RESET = 0;

  /* Now initialize rest of the SPI core
   * according to the SPI Software considerations 4.2 in 80-VD507-1 */

  /* OUTPUT_FIRST mode, NO LOOPBACK, NO IN/OUT & FIFO's Empty, SPI Master */
  *handle->gsbi_port.SPI_CONFIG = 0;
  *handle->gsbi_port.QUP_CONFIG = 0;

  *handle->gsbi_port.QUP_CONFIG |= QUP_CONFIG_MINI_CORE_SPI;

  switch (handle->data_width)
  {
    case SPI_DATA_WIDTH_8_BIT:   set_data_width (handle, 8);  break;
    case SPI_DATA_WIDTH_16_BIT:  set_data_width (handle, 16); break;
    case SPI_DATA_WIDTH_24_BIT:  set_data_width (handle, 24); break;
    case SPI_DATA_WIDTH_32_BIT:  set_data_width (handle, 32); break;
  }

  /* Enable output bit shifting and disable pack/unpacking
   * and run the SPI core in FIFO mode */
  *handle->gsbi_port.QUP_IO_MODES = QUP_IO_MODES_OUTPUT_BIT_SHIFT_EN  |
                               QUP_IO_MODES_INPUT_MODE_FIFO      |
                               QUP_IO_MODES_OUTPUT_MODE_FIFO;

  switch (handle->mode)
  {
    case SPI_MODE_0:
      clk_idle_state = 0;
      input_first_mode = SPI_INPUT_FIRST_MODE;
      break;

    case SPI_MODE_1:
      clk_idle_state = 0;
      input_first_mode = 0;
      break;

    case SPI_MODE_2:
      clk_idle_state = 1;
      input_first_mode = SPI_INPUT_FIRST_MODE;
      break;

    case SPI_MODE_3:
      clk_idle_state = 1;
      input_first_mode = 0;
      break;

    default:
      return SPI_DEVICE_FAIL;
  }

  /* Set sample phase or QCOM's INPUT/OUTPUT First mode
   * LOOP_BACK and SLAVE_OPERATION : off, N is already set above */
  *handle->gsbi_port.SPI_CONFIG |= input_first_mode;

  /*  CLK_ALWAYS_ON = 0    : No Clk during IDLE, 
   *  MX_CS_MODE = 1       : Assert CS till last word xfer
   *  CS_N_POLARITY = 0000 : All CS's are active Low
   *  CS_SELECT = 00       : Use SPI_CS_N
   *  TRISTATE_CS = 0      : No Z for any CS
   *  NO_TRI_STATE = 0     : Out pin Z when CS inactive
   */
  *handle->gsbi_port.SPI_IO_CONTROL = 0x100;

  /* set clock polarity  */
  if (clk_idle_state)
    *handle->gsbi_port.SPI_IO_CONTROL |= SPI_IO_CONTROL_CLOCK_IDLE_HIGH;
  else
    *handle->gsbi_port.SPI_IO_CONTROL &= (~SPI_IO_CONTROL_CLOCK_IDLE_HIGH);

  enable_errors (handle, QUP_ERROR_ALL_ERRORS);

  spi_core_reset (handle);

  return SPI_DEVICE_DONE;
}

static int
drain_fifos (spi_handle_t handle)
{
  uint32 fifo_data;

  /* In case if while checking for output FIFO empty, if there is
   * already a DWORD being moved from shift register, then
   * there is a chance that we might miss draining input FIFO
   * since the data is still being received and hence still didn't
   * make it into the FIFO
   * If this scenario ever causes problem (mostly for 32 bit mode), then
   * we can attempt to put the core in paused state, which pauses after the
   * data is moved out of shift register completely, then we can drain both
   * FIFO's when core is back in running state */

  /* First make sure our output FIFO is drained */
  while (1)
  {
    if (QUP_DATA_AVAILABLE_FOR_READ (get_fifo_status (handle)))
      fifo_data = *handle->gsbi_port.QUP_INPUT_FIFO;
    else
        if (QUP_OUTPUT_FIFO_EMPTY (get_fifo_status (handle)))
            if (!QUP_DATA_AVAILABLE_FOR_READ (get_fifo_status (handle)))
              break;
  }

  (void)fifo_data;

  return SPI_DEVICE_DONE;
}

#if defined(ENABLE_32BIT_OPTIMIZED_WRITE)||defined(ENABLE_32BIT_OPTIMIZED_READ)
static int
spi_fifo_write32 (spi_handle_t handle, uint32* cmd_buffer, int cmd_bytes)
{
  uint32 fifo_data, temp;

  while (cmd_bytes > 0)
  {
    /* Write to output FIFO as long as its not full */
    while (!QUP_OUT_FIFO_FULL (get_fifo_status (handle)))
    {
      fifo_data = *cmd_buffer;
      temp = reverse_endian (fifo_data);

      *handle->gsbi_port.QUP_OUTPUT_FIFO = temp;
      ++cmd_buffer;
      cmd_bytes -= 4;

      if (cmd_bytes <= 0)
        break;
    }
  }

  return SPI_DEVICE_DONE;
}
#endif

#ifdef ENABLE_32BIT_OPTIMIZED_WRITE
static int 
spi_write32 (spi_handle_t handle, uint32* cmd_buffer, int cmd_bytes,
             uint32* data_buffer, int data_bytes)
{

  if ((cmd_bytes % 4) || (data_bytes % 4) ||
      ((uint32)cmd_buffer & 0x3) || ((uint32)data_buffer & 0x3))
    return SPI_DEVICE_FAIL;

  config_direction (handle, OUT_ONLY);
  
  /* Set data width to 32 bits */
  set_data_width (handle, 32);

  spi_core_run (handle);

  spi_fifo_write32 (handle, cmd_buffer, cmd_bytes);

  spi_fifo_write32 (handle, data_buffer, data_bytes);

  handle->last_error_flags = get_error_flag (handle, QUP_ERROR_ALL_ERRORS);
  if (handle->last_error_flags)
    clear_errors (handle, handle->last_error_flags);

  drain_fifos (handle);

  /* Put SPI core back into reset state */
  spi_core_reset (handle);

  /* put it back into 8 bit mode */
  set_data_width (handle, 8);

  if (handle->last_error_flags & QUP_ERROR_OUTPUT_OVER_RUN_ERR)
    return SPI_DEVICE_FAIL;

  return SPI_DEVICE_DONE;
}
#endif

static int
spi_fifo_write (spi_handle_t handle, uint8* cmd_buffer, int cmd_bytes)
{
  while (cmd_bytes)
  {
    /* Write to output FIFO as long as its not full */
    while (!QUP_OUT_FIFO_FULL (get_fifo_status (handle)))
    {
      *handle->gsbi_port.QUP_OUTPUT_FIFO = *cmd_buffer;
      ++cmd_buffer;
      --cmd_bytes;

      if (cmd_bytes == 0)
        break;
    }
  }

  return SPI_DEVICE_DONE;
}

#ifdef ENABLE_32BIT_OPTIMIZED_READ
static int
spi_read32 (spi_handle_t handle, uint32* cmd_buffer, int cmd_bytes,
            uint32* data_buffer, int data_bytes)
{
  uint32 fifo_data, ignore_junk, fifo_status;

  /* See if we can operate in 32 bit mode which is very efficient */
  if ((cmd_bytes % 4) ||
      ((uint32)cmd_buffer & 0x3) || ((uint32)data_buffer & 0x3))
    return SPI_DEVICE_FAIL;

  ignore_junk = 1;
  if (handle->data_ret_option == DATA_RETURN_ALL)
    ignore_junk = 0;

  config_direction (handle, IN_OUT_BOTH);

  /* Set data width to 32 bits */
  set_data_width (handle, 32);

  /* If we have a command, send it out */
  spi_fifo_write32 (handle, cmd_buffer, cmd_bytes);

  spi_core_run (handle);

  if (ignore_junk)
  {
    while (cmd_bytes > 0)
    {
      while (!QUP_DATA_AVAILABLE_FOR_READ(get_fifo_status (handle)))
        ;

      fifo_data = *handle->gsbi_port.QUP_INPUT_FIFO;
      cmd_bytes -= 4;
    }
  }

  /* Read the data, also need to output data to clock to get input */
  while (data_bytes > 0)
  {
    *handle->gsbi_port.QUP_OUTPUT_FIFO = 0;
    fifo_status = get_fifo_status (handle);

    while (QUP_DATA_AVAILABLE_FOR_READ(fifo_status))
    {
      fifo_data = *handle->gsbi_port.QUP_INPUT_FIFO;
      fifo_data = reverse_endian (fifo_data);
      *data_buffer = fifo_data;

      ++data_buffer;
      data_bytes -= 4;
      if (data_bytes <= 0)
        break;

      fifo_status = get_fifo_status (handle);

      *handle->gsbi_port.QUP_OUTPUT_FIFO = 0;
    }
    if (QUP_OUTPUT_FIFO_EMPTY(fifo_status) && data_bytes > 8)
      *handle->gsbi_port.QUP_OUTPUT_FIFO = 0;
  }

  handle->last_error_flags = get_error_flag (handle, QUP_ERROR_ALL_ERRORS);
  if (handle->last_error_flags)
    clear_errors (handle, handle->last_error_flags);

  /* drain is needed since we might have some data pushed out into
   * the FIFO in an attempt to keep the bus saturated */
  drain_fifos (handle);

  /* Put SPI core back into reset state */
  spi_core_reset (handle);

  /* put it back into 8 bit mode */
  set_data_width (handle, 8);

  if (handle->last_error_flags & (QUP_ERROR_INPUT_OVER_RUN_ERR |
                             QUP_ERROR_INPUT_UNDER_RUN_ERR))
    return SPI_DEVICE_FAIL;

  return SPI_DEVICE_DONE;
}
#endif


/*
 * Function: Initialize the GPIOs used for SPI clock, data and chipselect
 */
static void spi_io_init()
{
  /* set the GSBI SPI GPIOs to Pull Up, 8mA drive mode */
  
  SPI_GPIO_CFG_CLK  = SPI_IO_PULL_UP_8mA_OE_MODE;
  SPI_GPIO_CFG_CS   = SPI_IO_PULL_UP_8mA_OE_MODE;
  SPI_GPIO_CFG_MISO = SPI_IO_PULL_UP_8mA_IE_MODE;
  SPI_GPIO_CFG_MOSI = SPI_IO_PULL_UP_8mA_OE_MODE;
}



/******************************************************************
 *
 *                     Public functions
 *
 *****************************************************************/
spi_handle_t 
spi_open (int port, enum spi_mode mode)
{
  spi_handle_t handle = &ghandle;

  /* GSBI port numbers start from 1 onwards */
  if ((port <= 0) || (port > sizeof(gsbi_ports)/sizeof(*gsbi_ports)))
    return INVALID_HANDLE_VALUE;

  /* For now allocation of the handle is just check if its in use */
  if ((handle->signature == SPI_SIGNATURE) || (handle->initialized == 1))
    return INVALID_HANDLE_VALUE;

  memset (handle, 0, sizeof(*handle));

  spi_io_init();

  handle->data_width = SPI_DATA_WIDTH_8_BIT;
  handle->data_ret_option = DATA_DISCARD_WRITE_SIZE;
  handle->mode = mode;
  handle->gsbi_port = gsbi_ports[port - 1];

  if (configure_spi_core (handle) < 0)
    return INVALID_HANDLE_VALUE;

  handle->signature = SPI_SIGNATURE;
  handle->initialized = 1;

  return handle;
}

int
spi_configure (spi_handle_t handle, struct spi_config* config_options)
{
  if ((handle->signature != SPI_SIGNATURE) || (handle->initialized != 1))
    return SPI_DEVICE_FAIL;

  /* If no change, then just return success */
  if ((handle->data_width == config_options->data_width) &&
      (handle->data_ret_option == config_options->data_ret_option))
    return SPI_DEVICE_DONE;

  switch (config_options->interface_version){
    case INITIAL_FIRST_VERSION:
      break;

      /* If unknown version number passed means the interface used is
       * of higher version */
    default:
      return SPI_DEVICE_FAIL;
  }

  /* Other data width mode not yet supported */
  if (config_options->data_width != SPI_DATA_WIDTH_8_BIT)
    return SPI_DEVICE_FAIL;

  if (handle->mode > SPI_MODE_3)
    return SPI_DEVICE_FAIL;

  handle->data_width = config_options->data_width;
  handle->data_ret_option = config_options->data_ret_option;

  if (configure_spi_core (handle) < 0)
    return SPI_DEVICE_FAIL;

  return SPI_DEVICE_DONE;
}

int 
spi_write (spi_handle_t handle, uint8* cmd_buffer, int cmd_bytes,
           uint8* data_buffer, int data_bytes)
{
  if ((handle->signature != SPI_SIGNATURE) || (handle->initialized != 1))
    return SPI_DEVICE_FAIL;

#ifdef ENABLE_32BIT_OPTIMIZED_WRITE
  if ((cmd_bytes || data_bytes) &&
      ((cmd_bytes % 4) == 0) && ((data_bytes % 4) == 0) && 
      (((uint32)cmd_buffer & 0x3) == 0) && (((uint32)data_buffer & 0x3) == 0))
  {
    return spi_write32 (handle, (uint32*) cmd_buffer, cmd_bytes,
                        (uint32*) data_buffer, data_bytes);
  }
#endif

  config_direction (handle, OUT_ONLY);

  spi_core_run (handle);

  spi_fifo_write (handle, cmd_buffer, cmd_bytes);

  spi_fifo_write (handle, data_buffer, data_bytes);

  handle->last_error_flags = get_error_flag (handle, QUP_ERROR_ALL_ERRORS);
  if (handle->last_error_flags)
    clear_errors (handle, handle->last_error_flags);

  drain_fifos (handle);

  /* Put SPI core back into reset state */
  spi_core_reset (handle);

  if (handle->last_error_flags & QUP_ERROR_OUTPUT_OVER_RUN_ERR)
    return SPI_DEVICE_FAIL;

  return SPI_DEVICE_DONE;
}

int 
spi_read (spi_handle_t handle, uint8* cmd_buffer, int cmd_bytes,
          uint8* data_buffer, int data_bytes)
{
  uint32 fifo_data;

  if ((handle->signature != SPI_SIGNATURE) || (handle->initialized != 1))
    return SPI_DEVICE_FAIL;

#ifdef ENABLE_32BIT_OPTIMIZED_READ
  if ((cmd_bytes || data_bytes) && ((cmd_bytes % 4) == 0) &&
      (((uint32)cmd_buffer & 0x3) == 0) && ((data_bytes % 4) == 0) &&
      (((uint32)data_buffer & 0x3) == 0))
    return spi_read32 (handle, (uint32*)cmd_buffer, cmd_bytes,
                       (uint32*)data_buffer, data_bytes);
#endif

  config_direction (handle, IN_OUT_BOTH);

  spi_core_run (handle);

  /* If we have a command, send it out */
  spi_fifo_write (handle, cmd_buffer, cmd_bytes);

  /* Needed to get the data reading started after ignoring the junk reads */
//  if (data_bytes > 0)
//    *handle->gsbi_port.QUP_OUTPUT_FIFO = 0;

//  spi_core_run (handle);

  /* Consume garbage data of data sent out */
  while (cmd_bytes)
  {
    while (QUP_DATA_AVAILABLE_FOR_READ (get_fifo_status (handle)))
    {
      fifo_data = *handle->gsbi_port.QUP_INPUT_FIFO; 
      --cmd_bytes;
      if (cmd_bytes == 0)
        break;
    }
  }

  if (data_bytes > 0)
    *handle->gsbi_port.QUP_OUTPUT_FIFO = 0;

  while (data_bytes)
  {
    if (QUP_DATA_AVAILABLE_FOR_READ (get_fifo_status (handle)))
    {	    
      fifo_data = *handle->gsbi_port.QUP_INPUT_FIFO;
      *data_buffer = (fifo_data & 0xFF);
      ++data_buffer;
      --data_bytes;
    }
    else
    {
      if (QUP_OUTPUT_FIFO_EMPTY(get_fifo_status (handle)) &&
        !QUP_DATA_AVAILABLE_FOR_READ (get_fifo_status (handle)))
      {	      
        *handle->gsbi_port.QUP_OUTPUT_FIFO = 0;
      }
    }	    
  }

  handle->last_error_flags = get_error_flag (handle, QUP_ERROR_ALL_ERRORS);
  if (handle->last_error_flags)
    clear_errors (handle, handle->last_error_flags);

  /* We would have sent more data into the output FIFO to speedup
   * So drain both FIFO's */
  drain_fifos (handle);

  /* Put SPI core back into reset state */
  spi_core_reset (handle);

  clear_errors(handle, QUP_ERROR_ALL_ERRORS);

  if (handle->last_error_flags & (QUP_ERROR_INPUT_OVER_RUN_ERR |
                             QUP_ERROR_INPUT_UNDER_RUN_ERR))
    return SPI_DEVICE_FAIL;

  return SPI_DEVICE_DONE;
}

int 
spi_close (spi_handle_t handle)
{
  if ((handle->signature != SPI_SIGNATURE) || (handle->initialized != 1))
    return SPI_DEVICE_FAIL;

  memset (handle, 0, sizeof(*handle));

  return SPI_DEVICE_DONE;
}
