#ifndef BOOT_EXTERN_DDR_INTERFACE_H
#define BOOT_EXTERN_DDR_INTERFACE_H
/*===========================================================================

                    BOOT EXTERN DDR DRIVER DEFINITIONS

DESCRIPTION
  Contains wrapper definition for external ddr drivers

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
 
    
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/03/14   xl      Add boot_ddr_set_params_enhance, enhance set
02/24/14   ck      Added boot_ddr_restore_from_wdog_reset
02/07/14   ck      Readjusted DDR info to match 512MB 8916 spec
01/07/14   ck      Updated DDR info to match 8916 spec
08/15/13   sl      Change feature flag for boot_ddr_debug
06/18/13   sl      Add boot_ddr_debug
05/23/13   tw      removed unused ddr sdram2 in ddr structure
10/16/12   sl      Add boot_ddr_test
10/09/12   dh      Add boot_ddr_get_info
09/14/12   jz      Remove unused boot_ddr_param_interleaved
07/23/12   dh      Add DDR training API
10/18/11   dh      Initial Creation.
===========================================================================*/

/*==========================================================================

                               INCLUDE FILES

===========================================================================*/

#include "ddr_drivers.h"
#include "ddr_common.h"


/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

/* ============================================================================
**  Function : boot_ddr_set_params
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR driver's device table to an external device table.
*   It will also update the interleaving mode information according to the parameter
*   header that's passed in.
*   User should append ddr_device_params_header_v1 header in front of their own DDR device 
*   table and pass  a pointer points to the beginning of this header via the this API to 
*   force DDR driver to uiltize the user defined parameters.
*   The user defined ddr device table must contain a minimal number of entries to 
*   match the system's ddr devices.
*   For example if a system has two ddr devices attached then the external table must 
*   define at least two entries to match those two devices.
*   However, more entries can be added to the end of table to support other ddr devices
*   with different parameters.
*
*   @param[in]  ddr_params_ptr   Pointer to the external ddr parameters. It should
*                                point to the user defined ddr device table with ddr
*                                parameters header appended in front. This pointer must
*                                points to a 4 bytes aligned address.
*                         
*
*   @return
*   TRUE if DDR device table is updated, FALSE if an error happens and this operation
*   is aborted
*
*   @dependencies
*   Must be called prior to ddr_initialize_device. 
*   This API will take no effect after ddr_initialize_device
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  boolean boot_ddr_set_params(void *ddr_params_ptr, uint32 ddr_params_size);
#else
  static inline boolean boot_ddr_set_params(void *ddr_params_ptr, uint32 ddr_params_size)
  {
    return TRUE;
  }
#endif

/* ============================================================================
**  Function : boot_ddr_set_params_enhance
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR DC timing parameters in CDB2 and CDB3.
*   For CDB2,this function will directly set the registers of DDR driver .
*   For CDB3,this function will put the data in IMEM for RPM setting.
*   @param[in]  ddr_params_ptr   pointer to the base address of CDB2 or CDB3 in memory
*                
*
*   @return
*   TRUE if the parameters was successfully set.
*
*   @dependencies
*  None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_ENHANCE_SET_ENABLED
  boolean boot_ddr_set_params_enhance(void *ddr_params_ptr, uint32 ddr_params_size);
#else
  static inline boolean boot_ddr_set_params_enhance(void *ddr_params_ptr, uint32 ddr_params_size)
  {
    return TRUE;
  }
#endif
/*=============================================================================
    Function : boot_ddr_initialize_device
=============================================================================*/

/**
  Sets up the DDR parameters in the HAL and initializes the DDR device.

  This function passes the currently used DDR parameters to the HAL and
  then initializes the DDR device per the specified clock speed.

  @param[in] clk_speed    Clock speed.

  @return
  None.

  @dependencies
  None.
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  void boot_ddr_initialize_device(uint32 clk_speed);
#else
  static inline void boot_ddr_initialize_device(uint32 clk_speed)
  {
  }
#endif


/*=============================================================================
    Function : boot_ddr_get_size
=============================================================================*/

/**
  Gets the DDR size and base address.

  @return
  DDR size and base address.

  @dependencies
  None.
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  ddr_size_info boot_ddr_get_size( void );
#else
  static inline ddr_size_info boot_ddr_get_size()
  {
    //TODO: remove hard coded size once DDR driver is delivered
    ddr_size_info ddr_size =
    {
      256,         /**< DDR size of Interface0 and chip select 0. */
      0,           /**< DDR size of Interface0 and chip select 1. */
      256,         /**< DDR size of Interface1 and chip select 0. */
      0,           /**< DDR size of Interface1 and chip select 1. */
  
      0x80000000,  /**< DDR start address of Interface0 and chip select 0. */
      0x0,         /**< DDR start address of Interface0 and chip select 1. */
      0x90000000,  /**< DDR start address of Interface1 and chip select 0. */
      0x0,         /**< DDR start address of Interface1 and chip select 1. */
    };
    return ddr_size;
  }
#endif


/* ============================================================================
**  Function : boot_ddr_get_partition
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr partitions and base address
*   
*   @details
*
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval ddr_size_partitioning contains the ddr partitions and base address
*   
*   @sa None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  ddr_size_partition boot_ddr_get_partition(void);
#else
  static inline ddr_size_partition boot_ddr_get_partition()
  {
    //TODO: remove hard coded size once DDR driver is delivered
    ddr_size_partition ddr_partition =
    {
      {4,0x80000000},{0,0},{4,0x90000000},{0,0},
    };
    return ddr_partition;
  }
#endif


/* ============================================================================
**  Function : boot_ddr_params_is_training_required
** ============================================================================
*/
/*!
*   @brief
*   Returns true of ddr training is required, false if not
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval Returns true of ddr training is required, false if not
*   
*   @sa None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  boolean boot_ddr_params_is_training_required(void);
#else
  static inline boolean boot_ddr_params_is_training_required(void)
  {
    return FALSE;
  }
#endif


/* ============================================================================
**  Function : boot_ddr_params_get_training_data
** ============================================================================
*/
/*!
*   @brief
*   Returns a pointer to ddr training data
*   
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval pointer to the ddr training data 
*   
*   @sa None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  void* boot_ddr_params_get_training_data(uint32 *size);
#else
  static inline void* boot_ddr_params_get_training_data(uint32 *size)
  {
    return NULL;
  }
#endif


/* ============================================================================
**  Function : boot_ddr_params_set_training_data
** ============================================================================
*/
/*!
*   @brief
*   pass ddr training data buffer to ddr driver
*   
*  @param  ddr_training_data pointer to ddr training data
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval None
*   
*   @sa None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  boolean boot_ddr_params_set_training_data(void *ddr_training_data, uint32 size);
#else
  static inline boolean boot_ddr_params_set_training_data(void *ddr_training_data, uint32 size)
  {
    return TRUE;
  }
#endif


/* ============================================================================
**  Function : boot_ddr_post_init
** ============================================================================
*/
/*!
*   @brief
*   This function does actual DDR training if training is required
*
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval True if ddr training data is updated, false if not
*   
*   @sa None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  boolean boot_ddr_post_init(void);
#else
  static inline boolean boot_ddr_post_init(void)
  {
    return FALSE;
  }
#endif


/* ============================================================================
**  Function : boot_ddr_get_info
** ============================================================================
*/
/*!
*   @brief
*   Returns the physical configuration of ddr on target
*
*   @details
*   This function will return the physical configuration of the ddr on target
*
*   @param  None.
*
*   @par Dependencies
*   None
*
*   @par Side Effects
*   None
*
*   @retval ddr_size_info contains the ddr size and base address
*
*   @sa None
*/

#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  ddr_info boot_ddr_get_info(void);
#else
  static inline ddr_info boot_ddr_get_info(void)
  {
    /*simulating 512MB interleaved lpddr3 */
    ddr_info sbl_ddr_info  =
      {
        {
          256,        /**< DDR size of Interface0 and chip select 0. */
          0,          /**< DDR size of Interface0 and chip select 1. */
          256,        /**< DDR size of Interface1 and chip select 0. */
          0,          /**< DDR size of Interface1 and chip select 1. */
          
          0x80000000, /**< DDR start address of Interface0 and chip select 0. */
          0x0,        /**< DDR start address of Interface0 and chip select 1. */
          0x90000000, /**< DDR start address of Interface1 and chip select 0. */
          0x0,        /**< DDR start address of Interface1 and chip select 1. */
        },
        SDRAM_CS0_CS1_INTERLEAVE,
        DDR_TYPE_LPDDR3
      };
	
    return sbl_ddr_info;
  }
#endif

/* =============================================================================
**  Function : boot_ddr_test
** =============================================================================
*/
/**
*   @brief
*   This function invokes DDR Test Framework.
*
*   @param[in]  clk_speed  Clock speed (in KHz) DDR runs at
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
  void boot_ddr_test(uint32 clk_speed);
#else
  static inline void boot_ddr_test(uint32 clk_speed)
  {}
#endif

/* =============================================================================
**  Function : boot_ddr_debug
** =============================================================================
*/
/**
*   @brief
*   DDR Debug Mode engine: receive and echo user's inputs, and execute commands
*   if ready.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   Transportation layer (e.g., USB) must be set up, and all data must be moved
*   out of DDR (otherwise would be corrupted).
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
void boot_ddr_debug( void );
#else
static inline void boot_ddr_debug( void )
{}
#endif

/* ============================================================================
**  Function : boot_ddr_params_is_training_on_RPM_required
** ============================================================================
*/
/*!
*   @brief
*   Returns true of ddr training in RPM is required, false if not
*   
*   @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval Returns true of ddr training on RPM is required, false if not
*   
*   @sa None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
boolean boot_ddr_params_is_training_on_rpm_required(void);
#else
static inline boolean boot_ddr_params_is_training_on_rpm_required(void)
{
  return FALSE;
}
#endif

/* =============================================================================
**  Function : boot_ddr_do_ddr_training
** =============================================================================
*/
/**
*   @brief
*   do DDR PHY training.
*
*   @param  None
*
*   @retval  None
*
*   @dependencies
*   Needs to initialize railway and CPR.
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
void boot_ddr_do_ddr_training(void);
#else
static inline void boot_ddr_do_ddr_training(void)
{}
#endif

/* =============================================================================
**  Function : boot_ddr_restore_from_wdog_reset
** =============================================================================
*/
/**
*   @brief
*   Used by SDI to bring DDR out of self refresh
*
*   @param [in]
*   clk_speed_khz  Clock speed of DDR in KHz
*
*   @retval  None
*
*   @dependencies
*   None
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
#ifdef FEATURE_BOOT_EXTERN_DDR_COMPLETED
void boot_ddr_restore_from_wdog_reset(uint32 clk_speed_khz);
#else
static inline void boot_ddr_restore_from_wdog_reset(uint32 clk_speed_khz)
{}
#endif

#endif /* BOOT_EXTERN_DDR_INTERFACE_H */
