#ifndef FLASH_HAL_H
#define FLASH_HAL_H

/**********************************************************************
 * flash_hal.h
 *
 * Public interface declaration for the FLASH HAL.
 *
 * This file describes the Flash HAL external interface, common to all
 * MSMs.
 *
 * Copyright (c) 2008-2012,2013,2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 **********************************************************************/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/core.mpss/3.5.c12.3/storage/flash/src/hal/flash_hal.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 *
 *
 * when         who     what, where, why
 *  --------     ---     ---------------------------------------------------
 * 11/12/15      sb      ECC bit flip check support
 * 10/18/13      sb      Add error codes for WP and CW mismatch
 * 09/05/12      sv      cleanup and optimization
 * 07/27/12      sv      BAM based NAND driver support changes
 * 02/15/12      sb      Add support for memutils in assembly
 * 12/14/11      eo      Add optimized transfer steps values
 * 05/18/11      bb      flash hal optimization changes
 * 10/21/10      sv      Add flash hal property id for wait cfg
 * 07/30/10      jz      Added 2X support
 * 07/30/10      eo      Fix FLASH_HAL_OP_TIMEOUT naming conflict
 * 07/08/10      jz      Cleanup dual nand controller mode
 * 06/23/10      bb      Fixing all enum sizes to be 4 bytes
 * 02/05/10      mcg     Support to get DMOV configs from DAL
 * 01/16/10      mm      Support for 2x Error codes
 * 01/10/10      mm      Support for real two plane program
 * 09/26/09      mh      Link hal ctlr data for case fo multiple hal_ids
 * 08/11/09      bb      Support for HAL optimized copy-Multi r/w APIs for NAND
 * 06/26/09      rk      512B - device enum types updated for 512B
 * 07/29/09      mh      Add API to set/get ctlr_info
 * 07/21/09      sv      Add option to set properties to HAL
 * 06/14/09      eo      Support for OneNAND HAL optimized APIs
 * 03/20/09      bb      Support for HAL optimized APIs
 * 03/15/08      bb      Initial version
 *=======================================================================*/

#include <HALcomdef.h>

/**  \mainpage Flash HAL Functional Overview
 *
 * The Flash driver module consists of three main components :
 * - Flash hardware abstraction layer (Flash HAL)
 * - Flash device driver common to all OSs
 * - Board Support Packages (BSP) for Qualcomm platforms.\n
 *
 * Flash driver provides a high-level interface for clients such
 * as bootloaders, file systems to interact with the flash devices.
 * And the flash HAL layer contains the HAL interface and
 * its implementation. The HAL provides a standard API to the driver,
 * independent of the underlying MSM ASIC hardware and Flash device.
 *
 * The flash HAL abstracts the flash controllers keeping the flash
 * device specific information with the driver. The driver configures HAL
 * according to the underlying flash device. The driver gets the underlying
 * device specific information from driver BSP. All hardware access is
 * performed in the HAL. The flash HAL is specific to a particular or
 * group of MSM ASICs.
 *
 * The BSP module defines board-level behavior for the flash HAL.
 *
 * Parameters and data passed to the Flash HAL are represented in
 * MSM-agnostic values. \n
 * \n
 *
 * <b> Note : Flash HAL has external dependency to
 *  clk_busy_wait, memcpy, memset functions.  </b>
 *
 */

/**
 * @section flash_hal Flash Hardware Abstraction Layer
 *
 * The flash hal software provides data transfer and control methods
 * for the flash controller.
 *
 * The Hardware Abstraction Layer serves to present a common calling
 * interface for all MSM designs that implement a flash Controller
 * block.  The specific attributes of the hardware, such as register
 * addresses, register names, and status bits are all contained within
 * the HAL, and referenced by generic terms between the driver and
 * HAL. The Flash HAL abstracts only the Flash controllers, leaving the
 * device specifics to the driver. The driver configure the HAL with
 * all the device specifics through HAL configuration APIs.
 *
 * @section error_handling HAL Error Handling
 *
 * None
 *
 *
 * @section return_values HAL Return Values
 *
 * Flash HAL returns operation status for critical functions. But most
 * functions have a "void" return type and no return value.
 *
 *
 * @section side_effects HAL "Side Effects"
 *
 * All flash HAL functions have "side effects" because they interact
 * with hardware.  That is, there are no "pure functional" HAL calls.
 * The documented purpose of each API call is one and the same with
 * the side-effect.  For this reason, no specific Side Effect, or lack
 * thereof, is stated in the documentation for each function.
 *
 *
 * @section dev_id Flash Device ID
 *
 * Most flash HAL functions take a \c dev_id parameter as the first
 * argument.  This value represents the unique logical device ID assigned
 * to a flash client by the flash HAL. A flash client gets the unique
 * device ID through the flash_hal_init() API call.
 *
 * This value is ASSUMED to be valid by all HAL API calls!  Because
 * these calls are not a public API, and the upper code must already
 * validate the controller number, the HAL will never perform a range
 * or validation check on the device ID.  The burden is on the
 * Driver layer to call HAL only with valid device ID values.  Use of
 * values that are not supported by the HAL will likely result in
 * system failure.  Values must never be passed from user/client code
 * to the HAL without careful inspection.
 */

/**
 * @defgroup flash_hal_api Flash HAL APIs
 * This section explains the APIs provided by flash HAL.
 *
 * @section flash_hal_dev_id Flash Device ID
 *
 * Most flash HAL functions take a \c dev_id parameter as the first
 * argument.  This value represents the unique logical device ID assigned
 * to a flash client by the flash HAL. A flash client gets the unique
 * device ID through the flash_hal_dev_init () API call.
 *
 * This value is ASSUMED to be valid for all HAL API calls!
 * The HAL will never perform a range or validation check on the device ID.
 * The burden is on the driver layer to call HAL only with valid device ID
 * values.  Use of values that are not supported by the HAL will likely
 * result in system failure.  Values must never be passed from
 * user/client code to the HAL without careful inspection.
 *
 * @section flash_hal_buffer HAL API Buffers
 *
 * Some HAL APIs take buffer pointers for data read from/write to device.
 * These buffer pointers can be either vitual or physical depending on
 * the context when using data mover (DM).Driver should make sure
 * that this buffer can be accessed by flash HAL when using in the
 * non-DM mode.
 */

/**
 * @defgroup flash_hal_mandatory Flash HAL Mandatory APIs
 * @ingroup flash_hal_api
 *
 * HAL Mandatory APIs.
 *
 * The HAL specification mandates certain common API calls to be
 * provided by all modules.
 *
 */

/*@{*/

#define FLASH_HAL_INVALID_DEVICE_ID     0xFFFFFFFF /* Invalid HAL Device ID */
#define FLASH_HAL_DEFAULT_DEVICE_ID     0x0        /* Default HAL Device ID */


/** HAL operation status */
enum flash_hal_status
{
  FLASH_HAL_SUCCESS,                   /**< Operation completed successfully */
  FLASH_HAL_FAIL,                      /**< Operation failed */
  FLASH_HAL_FAIL_MPU_ERR,              /**< Operation failed - MPU error*/
  FLASH_HAL_OP_NOT_SUPPORTED,          /**< Operation not supported */
  FLASH_HAL_OP_TIMEOUT_ERR,            /**< Timeout during operation */
  FLASH_HAL_BSP_DATA_NOT_FOUND,        /**< HAL BSP Data unavailable
                                               for the given device */
  FLASH_HAL_DEVICE_NOT_ATTACHED_TO_CS, /**< No flash device attached to
                                               this chip select  */
  FLASH_HAL_INVALID_DEVICE,            /**< Invalid device number */
  FLASH_HAL_ERASED_PAGE,               /**< Erased Page */
  FLASH_HAL_WP_ENABLED,                /**< Write protected */
  FLASH_HAL_CW_MISMATCH_ERR,           /**< CW mismatch error */
  FLASH_HAL_IGNORE_ERROR,              /**< Ignorable Error */
  FLASH_HAL_NO_MEM_ERROR,              /**< Insufficient memory */
  FLASH_HAL_PRGM_FAIL_PLANE1,          /**< Program failed on plane 1 */
  FLASH_HAL_PRGM_FAIL_PLANE2,          /**< Program failed on plane 2 */
  FLASH_HAL_PRGM_FAIL_BOTH_PLANES,     /**< Program failed on both planes */
  FLASH_HAL_CHECK_ECC,                 /**< Check ECC bytes to see if erased 
                                               page */
  FLASH_HAL_STATUS_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/**
 * Initializes flash HAL .
 *
 * Does initialization for a flash HAL and returns the
 * flash HAL version.
 *
 * Must be called prior to any other HAL APIs
 *
 * @param version [OUT]
 *   -- Flash HAL version
 *
 */
void flash_hal_init(char **version);

/**
 * Reset the flash HAL to  default state
 *
 * In flash HAL, this function has no effect.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 */
void flash_hal_reset(uint32 dev_id);

/**
 * Save the hardware status of flash HAL
 *
 * This mandatory function, combined with flash_hal_restore(),
 * provides a way to save and restore the state of HAL hardware. \n
 *
 * In flash HAL, this function does nothing.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @sa flash_hal_restore
 *
 */
void flash_hal_save(uint32 dev_id);

/**
 * Restore the hardware status of flash HAL
 *
 * This mandatory function, combined with flash_hal_save(),
 * provides a way to save and restore the state of HAL hardware. \n
 *
 * In flash HAL, this function does nothing.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @sa flash_hal_save
 */
void flash_hal_restore(uint32 dev_id);

/*@}*/

/**
 * @defgroup flash_hal_cfg Flash HAL Configuration APIs
 * @ingroup flash_hal_api
 *
 * APIs to configure the flash HAL according to the underlying device
 * specifications.
 *
 */

/*@{*/

/** Chip select that a flash device can be attached */
enum flash_hal_chip_sel
{
  FLASH_HAL_CS0,         /**< Chip select 0 */
  FLASH_HAL_CS1,         /**< Chip select 1 */
  FLASH_HAL_CS2,         /**< Chip select 2 */
  FLASH_HAL_CS3,         /**< Chip select 3 */
  FLASH_HAL_MAX_CS,      /**< Max Chip select supported */
  FLASH_HAL_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Types of flash devices */
enum flash_hal_dev_type
{
  FLASH_HAL_DEVICE_NONE,      /**< No device */
  FLASH_HAL_DEVICE_NOR,       /**< NOR device */
  FLASH_HAL_DEVICE_NAND,      /**< Regular NAND device */
  FLASH_HAL_DEVICE_ONENAND,   /**< OneNAND device */
  FLASH_HAL_DEVICE_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Device width */
enum flash_hal_dev_width
{
  FLASH_HAL_DEV_WIDTH_UNKNOWN = 0, /**< unknown device width */
  FLASH_HAL_8_BIT_WIDTH = 8,       /**< 8 Bit */
  FLASH_HAL_16_BIT_WIDTH = 16,     /**< 16 bit */
  FLASH_HAL_DEV_WIDTH_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Device page/sector unit size in bytes*/
enum flash_hal_page_size
{
  FLASH_HAL_PAGE_SIZE_UNKNOWN = 0, /**< unknown page size */
  FLASH_HAL_512B_PAGE  = 512,      /**< 512 bytes */
  FLASH_HAL_2048B_PAGE = 2048,     /**< 2048 bytes */
  FLASH_HAL_4096B_PAGE = 4096,     /**< 4096 bytes */
  FLASH_HAL_PAGE_SIZE_MAX = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Device spare unit size in bytes*/
enum flash_hal_spare_size
{
  FLASH_HAL_SPARE_SIZE_UNKNOWN = 0,     /**< unknown spare size */
  FLASH_HAL_16B_SPARE = 16,             /**< 16 bytes */
  FLASH_HAL_64B_SPARE = 64,             /**< 64 bytes */
  FLASH_HAL_SPARE_SIZE_MAX = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash HW ECC coverage/state */
enum flash_hal_ecc_state
{
  FLASH_HAL_HW_ECC_NONE,              /**< HW ECC Not supported */
  FLASH_HAL_HW_ECC_DISABLE,           /**< HW ECC Disable */
  FLASH_HAL_HW_ECC_ON_MAIN_ONLY,      /**< HW ECC on main only */
  FLASH_HAL_HW_ECC_ON_MAIN_AND_SPARE, /**< HW ECC on main and spare */
  FLASH_HAL_HW_ECC_STATE_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash HW ECC length */
enum flash_hal_ecc_length
{
  FLASH_HAL_HW_ECC_1BIT = 1,             /**< 1 Bit correction */
  FLASH_HAL_HW_ECC_4BIT = 4,             /**< 4 Bit correction */
  FLASH_HAL_HW_ECC_8BIT = 8,             /**< 8 Bit correction */
  FLASH_HAL_HW_ECC_LEN_ERROR,            /**< ECC length error */
  FLASH_HAL_HW_ECC_LEN_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/** DM state */
enum flash_hal_dma_state
{
  FLASH_HAL_DMA_DISABLE,             /**< Disables data mover usage */
  FLASH_HAL_DMA_ENABLE,              /**< Enables data mover usage  */
  FLASH_HAL_DMA_STATE_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Device AC Characteristics
 *
 * Some Parameters are not defined, in that case use 0 or
 * some educated guess. In any case all the information in
 * the data structure below is required */
struct flash_hal_ac_params
{
  uint32 cle_setup_ns;           /**< Min CLE setup Time    */
  uint32 cle_hold_ns;            /**< Min CLE Hold Time     */
  uint32 ce_setup_ns;            /**< Min CE setup Time     */
  uint32 ce_hold_ns;             /**< Min CE Hold Time      */
  uint32 we_wr_pulse_width_ns;   /**< Min WE Pulse Width    */
  uint32 ale_setup_ns;           /**< Min ALE setup Time    */
  uint32 ale_hold_ns;            /**< Min ALE Hold Time     */
  uint32 data_setup_ns;          /**< Min Data setup Time   */
  uint32 data_hold_ns;           /**< Min Data Hold Time    */
  uint32 write_cycle_time_ns;    /**< Min Write Cycle Time  */
  uint32 wen_high_hold_ns;       /**< Min WE High Hold Time */
  uint32 rdy_to_re_fall_edge_ns; /**< Min Ready to RE Low   */
  uint32 rp_read_pulse_width_ns; /**< Min RE Pulse Width    */
  uint32 rc_read_cycle_ns;       /**< Min Read Cycle Time   */
  uint32 re_access_ns;           /**< Max RE Access Time    */
  uint32 ce_high_to_output_ns;   /**< Max CE High to Output */
  uint32 re_high_hold_ns;        /**< Min RE High Hold Time */
  uint32 we_high_to_ren_low_ns;  /**< Min WE High to RE Low */
  uint32 wen_high_to_busy_ns;    /**< Max WE High to Busy   */
  uint32 ale_low_to_ren_low_ns;  /**< Min ALE to RE Delay   */
  uint32 cle_to_re_low_ns;       /**< Min CLE to RE Delay   */
};

/** Page Unit layout.
 *  A flash page is made up of multiple page units. This structure defines
 *  how the main and spare user data are organized in a page unit.
 */
struct flash_hal_page_unit_layout
{
  uint32 num_code_word;                /**< Number of codewords */
  uint32 main_offset;                  /**< Main data offset */
  uint32 main_len_bytes;               /**< Main data length in bytes */
  uint32 udata_offset;                 /**< Spare user data offset */
  uint32 udata_len_bytes;              /**< Spare user data length in bytes */
};

/** Page layout for a flash device.
 */
struct flash_hal_page_layout
{
  uint32 page_unit_count;                  /**< Number of page units */
  const struct flash_hal_page_unit_layout *unit_layout; /**< Page unit layout */
};

/** Flash device configuration information.
 */
struct flash_hal_dev_config
{
  enum flash_hal_dev_type dev_type;    /**< Device Type */
  enum flash_hal_chip_sel chip_sel;    /**< Chip select */
};

/**
 * Initializes flash device client in HAL and assigns a unique logical
 * device ID to it.
 *
 * Does initialization for a flash client and returns
 * a unique logical device ID to the caller.
 *
 * Must be called prior to any other operation related APIs
 *
 * @param dev_cfg [IN]
 *   -- Basic device configuration to be used in device init.
 *
 * @param dev_id [OUT]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param status [OUT]
 *   -- Flash HAL function status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail
 *
 * @sa flash_hal_dev_deinit
 */
void flash_hal_dev_init(const struct flash_hal_dev_config *dev_cfg,
  uint32 *dev_id, enum flash_hal_status *status);

/**
 * Does the clean up associated with a flash client.
 *
 * This is a mandatory API to call when a client no longer
 * needed any service from flash HAL
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @sa flash_hal_dev_init
 */
void flash_hal_dev_deinit(uint32 dev_id);

/**
 * Sets hardware ECC coverage state in flash HAL.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param ecc_state [IN]
 *   -- ECC State.
 */
void flash_hal_set_ecc_state(uint32 dev_id,
  enum flash_hal_ecc_state ecc_state);

/**
 * Sets hardware ECC length to be used with the flash device.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param ecc_len_bits [IN]
 *   -- ECC length.
 *
 */
void flash_hal_set_ecc_length(uint32 dev_id,
  enum flash_hal_ecc_length ecc_len_bits);

/**
 * Sets page size of the underlying flash device in flash HAL.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param page_size_bytes [IN]
 *   -- Device page size
 *
 */
void flash_hal_set_page_size(uint32 dev_id,
  enum flash_hal_page_size page_size_bytes);

/**
 * Sets spare size (if applicable) of the underlying flash device in flash HAL.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param spare_size_bytes [IN]
 *   -- Device spare size
 */
void flash_hal_set_spare_size(uint32 dev_id,
  enum flash_hal_spare_size spare_size_bytes);

/**
 *
 * Sets device access width in flash HAL. It can be one of
 *
 *  1) FLASH_HAL_8_BIT_WIDTH
 *  2) FLASH_HAL_16_BIT_WIDTH
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param width [IN]
 *   -- Device access width
 */
void flash_hal_set_dev_width(uint32 dev_id, enum flash_hal_dev_width width);

/**
 *
 * Sets the clock cycle gap for the device in flash HAL.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param clk_rate_mhz [IN]
 *   -- EBI2 clock rate in MHz
 *
 * @param wb_ns [IN]
 *   -- tWB in ns (from device's datasheet)
 *
 */
void flash_hal_set_clk_cycle_gap(uint32 dev_id, uint32 clk_rate_mhz, uint32 wb_ns);

/**
 *
 * Sets the number of address cycles for the device in flash HAL.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param row_count [IN]
 *   -- Number of row address
 *
 * @param col_count [IN]
 *   -- Number of column address
 */
void flash_hal_set_address_cycles(uint32 dev_id,
  uint32 row_count, uint32 col_count);

/*@}*/

/**
 * @defgroup flash_hal_capabilities Flash HAL Capability APIs
 * @ingroup flash_hal_api
 *
 * APIs to report hardware capabilities of the flash controller.
 *
 */

/*@{*/

/** Controller Hardware ECC Support information.
 */
struct flash_hal_hw_ecc_info
{
  boolean hw_ecc_is_supported;          /**< H/W ECC Support flag */
  boolean hw_ecc_on_spare_is_supported; /**< H/W ECC on Main and spare
                                             support flag */
  uint32  num_hw_ecc_len_supported;     /**< Number of H/W ECC length
                                             corrections supported */
  uint32  hw_ecc_len_supported[5];      /**< Supported ECC Lengths */
  uint32  config_hw_ecc_length;     /**< Currently configured ECC Length */
};

/**
 * Returns page layout configured by flash HAL.
 *
 * Main data and spare data are programmed in a flash device based
 * on a layout, which is determined by the page size and
 * ECC state configured. The flash driver can get the page layout
 * using this API to determine how the main and spare bytes
 * are organized in the flash device.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param layout [OUT]
 *   Page Layout pointer
 *
 * @sa flash_hal_page_layout_id
 */
void flash_hal_get_page_layout(uint32 dev_id,
 struct flash_hal_page_layout **layout);

/**
 * Returns memory required by the predefined APIs
 *
 * This is to optimize the memory usage by the flash driver.
 * This API helps to determine the memory required by the flash HAL.
 *
 * Note: This API returns only the memory required by the predefined API.
 *  It doesn't include the memory required for non-predefiend path in HAL.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param no_of_pages [IN]
 *   -- Number of pages. The memory usage will vary based on the multi
 *      page support with predefined APIs.
 *
 * @param mem_use [OUT]
 *   -- Memory required.
 *
 */
void flash_hal_get_predef_mem_usage (uint32 dev_id, uint32 no_of_pages,
  uint32 *mem_use);

/**
 * Returns base address of a given chip select. This API is applicable
 *  only for NOR interface devices.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param addr_is_phys [IN]
 *   -- Physical or virtual address flag
 *
 * @return 32bit address
 *   -- Physical address if addr_is_phys is TRUE /
 *      Virtual address if addr_is_phys is FALSE
 */
uint32 flash_hal_get_cs_base_addr(uint32 dev_id, boolean addr_is_phys);


/*@}*/

/**
 * @defgroup flash_hal_ops Flash HAL Operation APIs
 * @ingroup flash_hal_api
 *
 * APIs to complete device operations. The driver creates an
 * operation sequence and it can be completed by executing the
 * created sequence.
 *
 * Execution of an operation sequence can be done in two ways,              \n
 *
 * <b>- Without Data Mover </b>
 *
 * The flash HAL directly interfaces with the flash controller to
 * execute the operation and returns the results to the driver
 *
 * <b>- With Data Mover </b>
 *
 * The flash HAL indirectly interfaces with the flash controller
 * via the Data Mover, as follows,                                           \n
 *  1) Flash HAL generates a set of flash controller operations to
 *     execute the flash driver operations                                   \n
 *  2) Flash HAL returns this set to the driver.                             \n
 *  3) Flash driver generates a corresponding DM command set and
 *     forwards it to DM driver                                              \n
 *  4) Flash driver get the response from DM driver                          \n
 *  5) Flash driver forwards the DM response to flash HAL for post processing\n
 *  6) Flash HAL does the post processing and returns results to the driver  \n
 *
 */

/**
 * @defgroup flash_hal_common_cfg Flash HAL Operation Common APIs
 * @ingroup flash_hal_ops
 *
 * APIs common across both NAND and NOR interface devices.
 */

/*@{*/
struct flash_hal_op_cmd
{
  uint32 reg_addr : 24;  /**< Register physical address */
  uint32 command  : 8;   /**< Command  */
  uint32 data ;          /**< Data */
  uint32 mask;           /**< Mask */
  uint32 reserved;       /**< Reserved */ 
};

struct flash_hal_op_desc
{
  uint32 buf_pa;         /**< Buffer physical address */
  uint32 buf_size : 16;  /**< Buffer size in bytes */
  uint32 flags : 16;     /**< Flag bitmask (see bam_iovec_options_type) */
};

/**
   @brief Data structure for flash HAL post processing DM response
*/
struct flash_hal_post_process_cmd
{
  uint32 cmd;         /**< Flash HAL command options */
  uint32 addr;        /**< Address to verify. */
  uint32 op_len;      /**< Length of operation */
  uint32 mask;        /**< Mask value to use(if needed) */
  uint32 value;       /**< Value to use(if needed) */
};

/**
   @brief Data structure for executing flash operation with data mover
*/
struct flash_hal_op_list
{
  uint32 op_list_count;                  /**< Operation List count */
  struct flash_hal_op_cmd *op_list;      /**< Operation List */
  uint32 pp_list_count;                  /**< Post processing list count */
  struct flash_hal_post_process_cmd *pp_list; /**< Operation post
                                                   processing list */
};

/**
   @brief Operation command types
*/
enum flash_hal_op_cmd_option
{
  FLASH_HAL_CMD_OPTION_NONE = 0x00000000,    /**< No options */
  FLASH_HAL_CMD_OPTION_CHANGED = 0x00000001, /**< Data source SYNC */
  FLASH_HAL_SRC_SYNC        = 0x00000002,    /**< Data source SYNC */
  FLASH_HAL_DST_SYNC        = 0x00000004,    /**< Data destination SYNC */
  FLASH_HAL_SRC_VIRTUAL     = 0x00000008,    /**< Source virtual address */
  FLASH_HAL_DEST_VIRTUAL    = 0x00000010,    /**< Dest. virtual address */
  FLASH_HAL_SUBCMD_LAST     = 0x00000020,    /**< Dest. virtual address */
  FLASH_HAL_CMDPTR_LAST     = 0x00000040,    /**< Dest. virtual address */
  FLASH_HAL_DST_VERIFY      = 0x00010000,    /**< Post process verify */
  FLASH_HAL_ERASED_CHECK    = 0x00020000     /**< Post process verify */
};

/**
 * Indicates beginning of a flash HAL operation.
 *
 * Any operation (read, write etc) with flash device is done
 * through a set of flash HAL operation APIs. This function
 * should be called prior to any other operation related APIs.
 *
 * Flash HAL uses a memory area for storing and executing operation
 * sequences. Driver must provide sufficient memory to the flash
 * HAL through the ws_buffer. If there is not enough memory, HAL will
 * return failure during flash_hal_exec_op.
 *
 * This must be called prior to any other NAND/NOR interface APIs
 *
 * @note Flash HAL doesn't provide any concurrency. It should be
 *       handled in the upper layers (driver). It is drivers responsibity
 *       to ensure that no two clients tries to execute flash operations
 *       simultaneously.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param ws_buffer [IN]
 *   -- HAL Workspace buffer pointer
 *
 * @param ws_buf_len_bytes [IN]
 *   -- Workspace buffer length in bytes
 *
 */
void flash_hal_begin_op(uint32 dev_id, void *ws_buffer,
  uint32 ws_buf_len_bytes);

/**
 *
 * Indicates end of a flash HAL operation.
 * This API should be called prior to executing the operation.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 */
void flash_hal_end_op(uint32 dev_id);

/**
 * Returns hardware read/write list corresponding to the operation
 *
 * This API returns the list of hardware read/write operations to the
 * caller (driver), according to the driver requested operation.
 * The returned set may be used to generate DM sequence to perform the
 * flash operation.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param op_list [OUT]
 *   -- Operation List
 *
 * @param status [OUT]
 *   -- API status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail
 *
 * @sa flash_hal_ops
 */
void flash_hal_get_op_list(uint32 dev_id, struct flash_hal_op_list **op_list,
  enum flash_hal_status *status);

/**
 *
 * Post processes operation list after data mover processing.
 *
 *
 * Since the flash driver is completely unaware of the flash controller
 * details, all the controller specific status needs to be checked in the
 * flash HAL. So after getting the operation done (from
 * flash HAL generated list - flash_hal_get_op_list) with the data mover, the
 * driver forwards the response to the flash HAL for error checking.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param op_list [IN]
 *   -- Operation List
 *
 * @param status [OUT]
 *   -- API status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail \n
 *      FLASH_HAL_MPU_ERROR - On MPU error during the operation\n
 *      FLASH_HAL_ERASED_PAGE - When erased page detected during a read
 *
 * @sa flash_hal_ops
 */
void flash_hal_post_process_op_list(uint32 dev_id,
  struct flash_hal_op_list *op_list, enum flash_hal_status *status);

/**
 *
 * Execute the flash operation when not using data mover.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param status [OUT]
 *   -- API status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail
 *
 * @sa flash_hal_ops
 */
void flash_hal_exec_op(uint32 dev_id, enum flash_hal_status *status);

/*@}*/

/**
  * @defgroup flash_hal_nand_cfg Flash HAL NAND Interface APIs
  * @ingroup flash_hal_ops
  *
  * APIs specific to NAND interface based devices.
  *
  * Any NAND interface operation can be performed by creating an
  * operation sequence and then by executing it. Syntax for any
  * NAND interface operation can be shown as below,
  *
  * Eg: Read Operation
  *
  * - Lock Mechanism, see \ref flash_hal_begin_op \n
  * - flash_hal_begin_op()               \n
  * - flash_hal_set_cmd()                \n
  * - flash_hal_set_addr()               \n
  * - flash_hal_set_sub_cmd()            \n
  * - flash_hal_wait()                   \n
  * - flash_hal_read_data()              \n
  * - flash_hal_set_cmd()                \n
  * - flash_hal_set_addr()               \n
  * - flash_hal_wait()                   \n
  * - flash_hal_read_data()              \n
  * - flash_hal_end_op()                 \n
  * if nonDM                             \n
  * -- flash_hal_exec_op()               \n
  * else DM                              \n
  * -- flash_hal_get_op_list()           \n
  * -- flash_hal_post_process_op_list()  \n
  * end if                               \n
  * - Unlock Mechanism
  */

/*@{*/

/** Flash page operation buffer details */
struct flash_hal_page_data
{
  void   *main_data;       /**< Pointer to main data buffer */
  void   *spare_data;      /**< Pointer to spare data buffer */
  uint32 main_size_bytes;  /**< Length of main data in bytes */
  uint32 spare_size_bytes; /**< Length of spare data in bytes */
};

/** Wait mechanism to be followed during an operation */
enum flash_hal_wait_t
{
  FLASH_HAL_WAIT_NONE,             /**< No wait */
  FLASH_HAL_WAIT_ON_DEVICE_READY,  /**< Device is Ready */
  FLASH_HAL_WAIT_ON_DEVICE_BUSY,   /**< Device is Busy */
  FLASH_HAL_WAIT_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/**
 *
 * Issues a one byte device command to the NAND device.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param cmd [IN]
 *   -- Device command.
 *
 */
void flash_hal_set_cmd(uint32 dev_id, uint8 cmd);

/**
 *
 * Issues a one byte device sub-command to the NAND device.
 * If a NAND operation consists of multiple commands, then all the
 * commands except the first termed as sub-commands by flash HAL.
 *
 * An flash_hal_set_cmd () should be invoked prior
 * to this function.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param sub_cmd [IN]
 *   -- Device sub command.
 *
 */
void flash_hal_set_sub_cmd(uint32 dev_id, uint8 sub_cmd);

/**
 *
 * Issues address bytes for the NAND operation.
 *
 * An flash_hal_set_cmd () should be invoked prior
 * to this function.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param num_bytes [IN]
 *   -- Number of Address bytes
 *
 * @param addr_bytes [IN]
 *   -- Address bytes
 *
 */
void flash_hal_set_addr(uint32 dev_id, uint32 num_bytes,
  const uint8 *addr_bytes);

/**
 *
 * Wait on device status
 *
 * This function introduces wait till the wait mechanism is
 * satisfied or until timeout occurs.
 *
 * flash_hal_set_cmd () should be invoked prior
 * to this function.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param wait [IN]
 *   -- Wait mechanism to be used.
 *
 * @param timeout [IN]
 *   -- Timeout in micro seconds
 *
 */
void flash_hal_wait(uint32 dev_id, enum flash_hal_wait_t wait,
  uint32 timeout);

/**
 *
 * Reads data_len_bytes bytes from the device to buffer.
 *
 * An flash_hal_set_cmd () should be invoked prior
 * to this function.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param data_len_bytes [IN]
 *   -- Data length in bytes.
 *
 * @param buffer [OUT]
 *   -- Destination Buffer pointer. See \ref flash_hal_buffer
 *
 */
void flash_hal_read_data(uint32 dev_id, uint32 data_len_bytes,
  uint8 *buffer);

/**
 *
 * Writes data_len_bytes bytes from buffer to the device.
 *
 * An flash_hal_set_cmd () should be invoked prior
 * to this function.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param data_len_bytes [IN]
 *   -- Data length in bytes.
 *
 * @param buffer [IN]
 *   -- Source Buffer pointer. See \ref flash_hal_buffer
 *
 */
void flash_hal_write_data(uint32 dev_id, uint32 data_len_bytes,
  const uint8 *buffer);

/**
 *
 * Read data bytes from main and/or spare areas of the device.
 *
 * An flash_hal_set_cmd () should be invoked prior
 * to this function.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param page_data [IN]
 *   -- Page buffer details
 *
 */
void flash_hal_read_main_spare(uint32 dev_id,
  struct flash_hal_page_data *page_data);

/**
 *
 * Write data bytes to main and/or spare areas of the device.
 *
 * An flash_hal_set_cmd () should be invoked prior
 * to this function.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param page_data [IN]
 *   -- Page buffer details
 *
 */
void flash_hal_write_main_spare(uint32 dev_id,
  struct flash_hal_page_data *page_data);

/**
 *
 * Read data_len_bytes bytes from source buffer at given offset.
 *
 * An flash_hal_set_cmd () should be invoked prior
 * to this function.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param offset [IN]
 *   -- Offset in source buffer.
 *
 * @param data_len_bytes [IN]
 *   -- Data length bytes.
 *
 * @param buffer [OUT]
 *   -- Destination buffer pointer for data bytes read.
 *
 */
void flash_hal_read_data_with_offset(uint32 dev_id, uint32 offset,
  uint32 data_len_bytes, uint8 *buffer);
/*@}*/

/**
  * @defgroup flash_hal_nor_cfg Flash HAL NOR Interface APIs
  * @ingroup flash_hal_ops
  *
  * APIs specific to NOR interface based devices.
  *
  * Any NOR interface operation can be performed by creating an
  * operation sequence and then by executing it. Syntax for any
  * NOR interface operation can be shown as below,
  *
  * Eg: Read Operation
  *
  * - Lock Mechanism, see \ref flash_hal_begin_op \n
  * - flash_hal_begin_op()          \n
  * - flash_hal_write_dev_reg_16()  \n
  * - flash_hal_write_dev_reg_16()  \n
  * - flash_hal_write_dev_reg_16()  \n
  * - flash_hal_wait_on_dev_16()         \n
  * - flash_hal_read_dev_buffer()        \n
  * - flash_hal_read_dev_reg_16()        \n
  * - flash_hal_end_op()                 \n
  * if nonDM                             \n
  * -- flash_hal_exec_op()               \n
  * else DM                              \n
  * -- flash_hal_get_op_list()           \n
  * -- flash_hal_post_process_op_list()  \n
  * end if                               \n
  * - Unlock Mechanism
  *
  */

/*@{*/

/** Wait mask mechanism to be followed during an operation */
struct flash_hal_wait_mask_info_16
{
   uint32 reg_addr;           /**< Wait Mask Register Address */
   uint16 mask;               /**< Mask Value */
   uint16 value;              /**< Value to check */
};

/**
 *
 * Reads data_len_bytes bytes from src_addr address to buffer.
 * This API is used to read data from device data buffer.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param src_addr [IN]
 *   -- Source Address
 *
 * @param data_len_bytes [IN]
 *   -- Length in bytes
 *
 * @param buffer [OUT]
 *   -- Destination buffer pointer.. See \ref flash_hal_buffer
 *
 */
void flash_hal_read_dev_buffer(uint32 dev_id, uint32 src_addr,
  uint32 data_len_bytes, uint8 *buffer);

/**
 *
 * Writes data_len_bytes bytes from buffer to dst_addr address.
 * This API is used to write data to device data buffer.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param dst_addr [IN]
 *   -- Destination Address
 *
 * @param data_len_bytes [IN]
 *   -- Length in bytes
 *
 * @param buffer [IN]
 *   -- Source buffer pointer. See \ref flash_hal_buffer
 *
 */
void flash_hal_write_dev_buffer(uint32 dev_id, uint32 dst_addr,
  uint32 data_len_bytes, const uint8 *buffer);


/*@}*/

/**
  * @defgroup flash_hal_predef Flash HAL Predefined sequences
  * @ingroup flash_hal_ops
  *
  * APIs specific to predefined sequences for DMA and Non-DMA modes.
  * The idea behind the below APIs to get the maximum throughput
  * from the HAL based flash driver. This module creates and initializes
  * predefined data structures once during init and use it when needed.
  * This avoids the overhead of creating the list everytime from the
  * scratch.
  *
  */

/*@{*/

/** Pre-allocated flash HAL sequence IDs for NAND devices.*/
enum flash_hal_predef_nand_ids
{
  FLASH_HAL_00_x1_READ,           /**<  00 single read */
  FLASH_HAL_00_30_x1_READ,        /**<  00-30 single read */
  FLASH_HAL_80_10_x1_WRITE,       /**<  10-80 single write */
  FLASH_HAL_60_D0_ERASE,          /**<  60-D0 erase */
  FLASH_HAL_FF_90_PROBE,          /**<  FF-90 read ID */
  FLASH_HAL_90_ONFI_ID,           /**<  90-ONFI ID read */
  FLASH_HAL_00_EC_READ,           /**<  00-EC parameter page read */

  FLASH_HAL_PREDEF_NAND_ID_MAX,   /**<  Number of perdef NAND IDs */
  FLASH_HAL_PREDEF_NAND_ID_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash HAL Buffer descriptor data. */
struct flash_hal_buffer_desc
{
  uint32 buff_vaddr;  /**< Virtual address of the buffer */
  uint32 buff_paddr;  /**< Physical address of the buffer */
  uint32 buff_size;   /**< Buffer size */
  uint32 num_pages;   /**< Number of pages to read/write with this buffer */
};

/** Flash HAL Buffer data */
struct flash_hal_mem_data
{
  uint32 buff_vaddr;  /**< Virtual address of the buffer */
  uint32 buff_paddr;  /**< Physical address of the buffer */
  uint32 buff_size;   /**< Buffer size */
};

/** Pre-alloc operation initialization data */
struct flash_hal_predef_init_data
{
  uint32 num_cs;             /**< Number of chip selects */
  uint32 max_support;        /**< Maximum pages/blocks supported */
  struct flash_hal_mem_data buff_data; /**< Buffer used for predefined
                                               sequence */
  struct flash_hal_mem_data raw_buff_data; /**< raw Buffer used raw page 
                                               read */											 
};

/** Buffer information for cache maintanence */
struct flash_hal_cache_info
{
  uint32 read_buffer_count;            /**< Read buffer count */
  struct flash_hal_mem_data *read_mem; /**< Read buffers */
  uint32 write_buffer_count;           /**< Write buffer count */
  struct flash_hal_mem_data *write_mem;/**< Write buffers */
};

/** Operation list for predefined sequences. This list
    is ready for DMA transfer and doesn't need any translation.
    But perform cache maintanence if required. */
struct flash_hal_predef_op_list
{
  uint32 *op_list;                      /**< Operation List */
  uint32 op_list_count;                 /**< Operation List count */
  uint32 seq_id;                        /**< Sequence ID */
  uint32 prev_op_count;                 /**< Previous operation list count */
  struct flash_hal_cache_info mem_info; /**< Cache maintanence information */
};

/** Details for probe with predefined HAL APIs */
struct flash_hal_predef_onfi_data
{
  uint32 page;                            /**< Page number */
  uint32 col_offset;                      /**< Address */
  uint32 addr_cycles;                     /**< Number of Address Cycles */
  struct flash_hal_buffer_desc buff_desc; /**< Buffer descriptors */
};

/** Details for page read/write with predefined HAL APIs */
struct flash_hal_predef_page_data
{
  uint32 start_page;                    /**< Start page */
  uint32 total_pages;                   /**< Total pages */
  uint32 main_size_bytes;               /**< Main data size */
  uint32 spare_size_bytes;              /**< Spare data size */
  uint32 buff_desc_count;               /**< Buffer descriptor count */
  struct flash_hal_buffer_desc *buff_desc; /**< -- Buffer descriptors */
};

/** Details for Flash spare operation with predefined HAL APIs*/
struct flash_hal_predef_spare_data
{
  uint32 page;                           /**< Page number */
  uint32 col_offset;                     /**< Coulmn offset */
  uint32 spare_size_bytes;               /**< Spare data size */
  struct flash_hal_buffer_desc buff_desc;/**< Buffer descriptor */
};

/** Flash block operation details */
struct flash_hal_predef_block_data
{
  uint32 start_addr;       /**< Starting address */
  uint32 num_blocks;       /**< Number of blocks to operate */
  uint32 pages_per_block;  /**< Number of pages to operate */
};


/** Flash HAL Property types */
enum flash_hal_property
{
  FLASH_HAL_PROPERTY_USE_STD_MEMUTIL,     /**< Use standard memutil APIs */
  FLASH_HAL_PROPERTY_USE_INT_MEMUTIL,     /**< Use internal memutil APIs */
  FLASH_HAL_PROPERTY_USE_INT_ASM_MEMUTIL, /**< Use internal optimized memutil APIs */
  FLASH_HAL_PROPERTY_DM_CFG,              /**< Data mover properties */
  FLASH_HAL_PROPERTY_WAIT_CFG,            /**< Wait properties */
  FLASH_HAL_PROPERTY_CACHE_INVAL_CFG,     /**< Cache invalidate function */
  FLASH_HAL_PROPERTY_CACHE_CLEAN_CFG,     /**< Cache clean function */
  FLASH_HAL_PROPERTY_MAX,                 /**< HAL max properties */
  FLASH_HAL_PROPERTY_MAX_SIZE = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash transfer steps configuration */
struct flash_hal_xfer_cfg
{
  uint32 xfer_reg_count;         /**< Number of transfer step registers */
  uint32 *xfer_reg_data;         /**< Transfer step register data buffer*/
};
  
/**
 *
 * Initializes flash HAL predefined API support.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param init_data [IN]
 *   -- Init data for predefined support
 *
 * @param used_buff_size [OUT]
 *   -- Used buffer size
 *
 *   -- API status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail \n
 *      FLASH_HAL_NO_BUFFER_ERROR - When erased page detected during a read
 *
 */
void flash_hal_predef_init(uint32 dev_id,
  struct flash_hal_predef_init_data *init_data, uint32 *used_buff_size,
  enum flash_hal_status *status);


/**
 *
 * Reads a flash page with predefined command sequence. In DMA mode
 * this function returns list for DMA transfer. For non-DMA, this
 * this function actually does the read operation.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param page_data [IN/OUT]
 *   -- Predefined read data
 *
 * @param op_list [OUT]
 *   -- Operation list. This is NULL for non-DMA mode. In the case of
 *      DMA, do the DMA transfer with the list and perform post-processing
 *      with this list.
 *
 * @param status [OUT]
 *   -- API status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail \n
 *      FLASH_HAL_MPU_ERROR - On MPU error during the operation in non-DM mode\n
 *      FLASH_HAL_ERASED_PAGE - When erased page detected during a read
 *                              in non-DM mode
 */
void flash_hal_read_page(uint32 dev_id, struct flash_hal_predef_page_data
  *page_data, struct flash_hal_predef_op_list **op_list, enum flash_hal_status
  *status);

/**
 *
 * Reads bad block marker with predefined command sequence.In DMA mode
 * this function returns list for DMA transfer. For non-DMA, this
 * this function actually does the bad block marker read.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param page_data [IN/OUT]
 *   -- Predefined spare read data
 *
 * @param op_list [OUT]
 *   -- Operation list. This is NULL for non-DMA mode. In the case of
 *      DMA, do the DMA transfer with the list and perform post-processing
 *      with this list.
 *
 * @param status [OUT]
 *   -- API status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail \n
 *      FLASH_HAL_MPU_ERROR - On MPU error during the operation in non-DM mode\n
 */
void flash_hal_read_bb_marker(uint32 dev_id, struct
  flash_hal_predef_spare_data *page_data, struct flash_hal_predef_op_list
  **op_list, enum flash_hal_status *status);

/**
 *
 * Writes a flash page with predefined command sequence.In DMA mode
 * this function returns list for DMA transfer. For non-DMA, this
 * this function actually does the page write operation.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param page_data [IN]
 *   -- Predefined read data
 *
 * @param op_list [OUT]
 *   -- Operation list. This is NULL for non-DMA mode. In the case of
 *      DMA, do the DMA transfer with the list and perform post-processing
 *      with this list.
 *
 * @param status [OUT]
 *   -- API status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail \n
 *      FLASH_HAL_MPU_ERROR - On MPU error during the operation in non-DM mode\n
 */
void flash_hal_write_page(uint32 dev_id, struct flash_hal_predef_page_data
  *page_data, struct flash_hal_predef_op_list **op_list, enum flash_hal_status
  *status);

/**
 *
 * Erase a flash block with predefined command sequence.In DMA mode
 * this function returns list for DMA transfer. For non-DMA, this
 * this actually does the block erase operation.
 *
 * @param dev_id [IN]
 *   -- Logical device ID, as described in \ref flash_hal_dev_id
 *
 * @param block_data [IN]
 *   -- Predefined block data
 *
 * @param op_list [OUT]
 *   -- Operation list. This is NULL for non-DMA mode. In the case of
 *      DMA, do the DMA transfer with the list and perform post-processing
 *      with this list.
 *
 * @param status [OUT]
 *   -- API status. Returns, \n
 *      FLASH_HAL_SUCCESS - On success \n
 *      FLASH_HAL_FAIL - On fail \n
 *      FLASH_HAL_MPU_ERROR - On MPU error during the operation in non-DM mode\n
 */
 void flash_hal_erase_block(uint32 dev_id, struct flash_hal_predef_block_data
  *block_data, struct flash_hal_predef_op_list **op_list, enum flash_hal_status
  *status);

/**
 *
 * Set properties to flash HAL
 *
 * @param id [IN]
 *   -- ID of the property that is set to HAL
 *
 * @param value [IN]
 *   -- Value of the property
 */
void flash_hal_set_property(enum flash_hal_property id, void *value);


/*
 * Optimized flash HAL read ID
 */
void flash_hal_read_id(uint32 dev_id, struct flash_hal_buffer_desc *buff_desc,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status *status);

/*
 * Optimized flash read ONFI ID
 */
void flash_hal_read_onfi_id(uint32 dev_id,
  struct flash_hal_predef_onfi_data *probe_data,
  struct flash_hal_predef_op_list **op_list, enum flash_hal_status *status);

/*
 * Optimized flash HAL read page
 */
void flash_hal_read_param_page(uint32 dev_id, struct flash_hal_predef_onfi_data
  *page_data, struct flash_hal_predef_op_list **op_list, enum flash_hal_status
  *status);

/*
 * Optimized flash HAL set xfer steps configuration data
 */
void flash_hal_set_xfer_cfg(uint32 dev_id, struct flash_hal_xfer_cfg *cfg_data, 
  enum flash_hal_status *status);

/*@}*/

#endif  /*ifndef FLASH_HAL_H */
