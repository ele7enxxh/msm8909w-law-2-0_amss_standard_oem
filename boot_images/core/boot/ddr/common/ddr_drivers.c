/*
===========================================================================

FILE:         ddr_drivers.c

DESCRIPTION:  
  This is the ddr drivers implementation for Qualcomm memory controller.

Copyright 2009-2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/common/ddr_drivers.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/28/14   aus     Added 6Gb support
02/09/14   tw      added support for common ddr library
11/12/12   sl      Cleaned up ddr_get_size().
09/07/12   sl      Updated SR/DPD enter/exit sequence;
                   cleaned up function calls for HAL DDR status.
08/24/12   sl      Update DDR status after rank detection.
08/17/12   sl      Removed HAL_SDRAM_ptr_init() call etc. for code cleanup.
05/15/12   tw      Fixed api name for deep power down to ensure consistancy
02/02/12   sl      Common LPDDR1/LPDDR2/PCDDR3 device parameters.
12/06/11   tw      Added api for getting ddr configuration and for target init
10/26/11   tw      Initial port for 8064
06/29/11   tw      Added support for DDR DMM
06/21/11   tw      Updated to support XO shutdown, frequency scaling, 
                   voltage scaling
05/23/11   tw      Fixed issues with ddr_get_partition passing back 
                   uninitialized data.
05/06/11   tw      Added support for extended ram partition table.
04/04/11   tw      Ported to 8960
02/03/11   sl      Enable/disable pad low power mode during clock switch.
01/31/11   sl      Cleaned up pre/post clock switch.
01/19/11   sl      Updated pre/post clock switch for deep power down.
01/12/11   sl      Updated pre/post clock switch, and pre/post XO shutdown.
01/06/11   tw      Changed to using HW clock switch for RPM, disabled low power 
                   pad settings due to instability
12/15/10   sl      Updated ddr_pre/post_xo_shutdown().
12/09/10   sl      Added ddr_do_forced_cdc() to ddr_pre_voltage_switch().
11/24/10   tw      Fixed compilation issue
11/24/10   tw      Fixed issue with tWL and tRL difference when auto detecting 
                   DDR Devices.
11/22/10   sl      Updated refresh counter after runtime device detection.
11/17/10   sl      Added I/O calibration in pre/post XO shutdown.
10/26/10   tw      Fixed incorrect tab spacing.
10/15/10   tw      Updated ddr driver to call HAL function for 8660  
                   implementation
09/20/10   tw      Forced timing parameter update after detection of ddr device
06/23/10   sl      Moved ddr_get_device() and ddr_device_detection() to ddr_devices.c
04/21/10   tw      runtime size detection
03/19/10   tw      Ported to 8660
03/04/10   tw      Moved ddr status definition into the drivers code.
01/19/10   tw      Updated clock switching flow to match vi recommendation
01/07/10   tw      Updated to support PCDDR2
06/18/09   tw      Initial version

===========================================================================
*/


/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_internal.h"
#include "ddr_sync.h"
#include "ddr_params.h"
#include "HAL_DDR.h"


/*==============================================================================
                                  DATA
==============================================================================*/
ddr_interface_state ddr_status;

/*Indicates ddr initialization is done or not, FALSE by default*/
boolean ddr_init_done = FALSE;

/* Chip selects for selected ranks by ddr_enter/exit_self_refresh_all() */
static SDRAM_CHIPSELECT ddr_csel_ch0, ddr_csel_ch1;


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
/* ============================================================================
**  Function : ddr_init
** ============================================================================
*/
/*!
*   @brief
*   Initialize ddr HAL with the correct ddr parameters
*   This must be done before calling any of the drivers functions
*   
*   @details
*   Initialize ddr HAL with the correct ddr parameters
*   This must be done before calling any of the drivers functions
*      
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_init(void)
{
  struct ddr_device_params_common *ddr_param_interface_0, *ddr_param_interface_1;

  /* Initialize the ddr driver Mutex */
  DDR_SYNC_INIT();

  /* Get DDR parameters */
  ddr_param_interface_0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
  ddr_param_interface_1 = &(ddr_get_params(SDRAM_INTERFACE_1)->common);

  /* First initialize all the ddr devices to be unavailable */
  ddr_status.sdram0_cs0 = DDR_UNAVAILABLE;
  ddr_status.sdram0_cs1 = DDR_UNAVAILABLE;
  ddr_status.sdram1_cs0 = DDR_UNAVAILABLE;
  ddr_status.sdram1_cs1 = DDR_UNAVAILABLE;

  /* based on ddr parameter selected, set the ddr status for active ddr */
  if(ddr_param_interface_0->num_rows_cs0 != 0)
  {
    ddr_status.sdram0_cs0 = DDR_ACTIVE;
  }
  if(ddr_param_interface_0->num_rows_cs1 != 0)
  {
    ddr_status.sdram0_cs1 = DDR_ACTIVE;
  }
  if(ddr_param_interface_1->num_rows_cs0 != 0)
  {
    ddr_status.sdram1_cs0 = DDR_ACTIVE;
  }
  if(ddr_param_interface_1->num_rows_cs1 != 0)
  {
    ddr_status.sdram1_cs1 = DDR_ACTIVE;
  }

  /* initialize target specific functions */
  ddr_target_init();

} /* ddr_init */

/* ============================================================================
**  Function : ddr_initialize_device
** ============================================================================
*/
/*!
*   @brief
*   This function will setup the ddr parameter in HAl and initialize the ddr
*   
*   @details
*   This function will first pass the currently used ddr device parameter
*   to the HAL. Then it will initialize the ddr as per specified speed
*   
*   @param clkspeed   -  [IN] clk speed to initialize for
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_initialize_device(uint32 clk_speed)
{
  struct ddr_device_params_common *ddr_param_interface_0, *ddr_param_interface_1;

  if(!ddr_init_done)
  {    
    /* Pass the ddr parameters to HAL */
    ddr_init();

    /* Update the ddr clock speed variable to the initialization clock */
    ddr_status.clk_speed = clk_speed;
    
    if(ddr_status.sdram0_cs0 != DDR_UNAVAILABLE)
    {
      if(ddr_status.sdram0_cs1 != DDR_UNAVAILABLE)
      {
        HAL_SDRAM_Init(SDRAM_INTERFACE_0, SDRAM_BOTH, clk_speed);
      }
      else
      {
        HAL_SDRAM_Init(SDRAM_INTERFACE_0, SDRAM_CS0, clk_speed);
      }
    }

    if(ddr_status.sdram1_cs0 != DDR_UNAVAILABLE)
    {
      if(ddr_status.sdram1_cs1 != DDR_UNAVAILABLE)
      {
        HAL_SDRAM_Init(SDRAM_INTERFACE_1, SDRAM_BOTH, clk_speed);
      }
      else
      {
        HAL_SDRAM_Init(SDRAM_INTERFACE_1, SDRAM_CS0, clk_speed);
      }
    }

    /* Get DDR device parameters after detection during initialization */
    ddr_param_interface_0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
    ddr_param_interface_1 = &(ddr_get_params(SDRAM_INTERFACE_1)->common);

    /* Update DDR status */
    ddr_status.sdram0_cs0 = (ddr_param_interface_0->num_rows_cs0 != 0) ?
                            DDR_ACTIVE : DDR_UNAVAILABLE;
    ddr_status.sdram0_cs1 = (ddr_param_interface_0->num_rows_cs1 != 0) ?
                            DDR_ACTIVE : DDR_UNAVAILABLE;
    ddr_status.sdram1_cs0 = (ddr_param_interface_1->num_rows_cs0 != 0) ?
                            DDR_ACTIVE : DDR_UNAVAILABLE;
    ddr_status.sdram1_cs1 = (ddr_param_interface_1->num_rows_cs1 != 0) ?
                            DDR_ACTIVE : DDR_UNAVAILABLE;

    /* Copy DDR device parameters to shared memory */
    ddr_params_set_shared_memory();

    ddr_init_done = TRUE;
  }
  /* Else, already initialized ddr, do nothing */
} /* ddr_initialize_device */

/* =============================================================================
**  Function : ddr_enter_self_refresh_all
** =============================================================================
*/
/**
*   @brief
*   Put DDR into self refresh mode for all active ranks.
*
*   @param[in]  clk_speed  Current clock speed (in KHz)
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
void ddr_enter_self_refresh_all(uint32 clk_speed)
{
  DDR_SYNC_LOCK();

  /* Set chip select and update DDR status */
  ddr_csel_ch0 = SDRAM_CS_NONE;
  if (ddr_status.sdram0_cs0 == DDR_ACTIVE)
  {
    ddr_status.sdram0_cs0 = DDR_SELF_REFRESH;
    ddr_csel_ch0 |= SDRAM_CS0;
  }
  if (ddr_status.sdram0_cs1 == DDR_ACTIVE)
  {
    ddr_status.sdram0_cs1 = DDR_SELF_REFRESH;
    ddr_csel_ch0 |= SDRAM_CS1;
  }
  /* Enter self refresh for selected ranks */
  if (ddr_csel_ch0 != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Enter_Self_Refresh(0, SDRAM_INTERFACE_0, ddr_csel_ch0, HAL_SDRAM_DDR_Wait);
  }

  /* Set chip select and update DDR status */
  ddr_csel_ch1 = SDRAM_CS_NONE;
  if (ddr_status.sdram1_cs0 == DDR_ACTIVE)
  {
    ddr_status.sdram1_cs0 = DDR_SELF_REFRESH;
    ddr_csel_ch1 |= SDRAM_CS0;
  }
  if (ddr_status.sdram1_cs1 == DDR_ACTIVE)
  {
    ddr_status.sdram1_cs1 = DDR_SELF_REFRESH;
    ddr_csel_ch1 |= SDRAM_CS1;
  }
  /* Enter self refresh for selected ranks */
  if (ddr_csel_ch1 != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Enter_Self_Refresh(0, SDRAM_INTERFACE_1, ddr_csel_ch1, HAL_SDRAM_DDR_Wait);
  }

  DDR_SYNC_UNLOCK();

} /* ddr_enter_self_refresh_all */

/* =============================================================================
**  Function : ddr_exit_self_refresh_all
** =============================================================================
*/
/**
*   @brief
*   Take DDR out of self refresh mode for ranks that are active before
*   ddr_enter_self_refresh_all() is called.
*
*   @param[in]  clk_speed  Current clock speed (in KHz)
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
void ddr_exit_self_refresh_all(uint32 clk_speed)
{
  DDR_SYNC_LOCK();

  /* Update DDR status */
  if (ddr_csel_ch0 & SDRAM_CS0)
  {
    ddr_status.sdram0_cs0 = DDR_ACTIVE;
  }
  if (ddr_csel_ch0 & SDRAM_CS1)
  {
    ddr_status.sdram0_cs1 = DDR_ACTIVE;
  }
  /* Exit self refresh for selected ranks */
  if (ddr_csel_ch0 != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Exit_Self_Refresh(0, SDRAM_INTERFACE_0, ddr_csel_ch0, HAL_SDRAM_DDR_Wait);
  }

  /* Update DDR status */
  if (ddr_csel_ch1 & SDRAM_CS0)
  {
    ddr_status.sdram1_cs0 = DDR_ACTIVE;
  }
  if (ddr_csel_ch1 & SDRAM_CS1)
  {
    ddr_status.sdram1_cs1 = DDR_ACTIVE;
  }
  /* Exit self refresh for selected ranks */
  if (ddr_csel_ch1 != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Exit_Self_Refresh(0, SDRAM_INTERFACE_1, ddr_csel_ch1, HAL_SDRAM_DDR_Wait);
  }

  DDR_SYNC_UNLOCK();

} /* ddr_exit_self_refresh_all */

/* ============================================================================
**  Function : ddr_enter_self_refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will put the a particular ddr into self refresh mode, provided that it is currently active.
*   
*   @details
*   This function will first check to see if the ddr specified by the caller is available and active, then put it into self
*   refresh mode. If it is not active or not available, simply do nothing.
*   
*   @param interface_name   - [IN] The interface to put ddr into self refresh
*   @param chip_select      -     [IN] The chip select to put ddr into self refresh
*   @param clkspeed      -        [IN] The current clock speed

*
*   @par Dependencies
*   This code has to reside in IRAM
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_enter_self_refresh(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  SDRAM_CHIPSELECT csel = SDRAM_CS_NONE;

  /*
    Issue a lock to prevent anyone else from updating ddr
    status while we are working here
  */
  DDR_SYNC_LOCK();

  /* Mark device as self refresh after entering self refresh */
  if (interface_name == SDRAM_INTERFACE_0)
  {
    if ( (chip_select & SDRAM_CS0) && (ddr_status.sdram0_cs0 == DDR_ACTIVE) )
    {
      csel |= SDRAM_CS0;
      ddr_status.sdram0_cs0 = DDR_SELF_REFRESH;
    }
    if ( (chip_select & SDRAM_CS1) && (ddr_status.sdram0_cs1 == DDR_ACTIVE) )
    {
      csel |= SDRAM_CS1;
      ddr_status.sdram0_cs1 = DDR_SELF_REFRESH;
    }
  }
  else
  {
    if ( (chip_select & SDRAM_CS0) && (ddr_status.sdram1_cs0 == DDR_ACTIVE) )
    {
      csel |= SDRAM_CS0;
      ddr_status.sdram1_cs0 = DDR_SELF_REFRESH;
    }
    if ( (chip_select & SDRAM_CS1) && (ddr_status.sdram1_cs1 == DDR_ACTIVE) )
    {
      csel |= SDRAM_CS1;
      ddr_status.sdram1_cs1 = DDR_SELF_REFRESH;
    }
  }

  if (csel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Enter_Self_Refresh(0, interface_name, csel, HAL_SDRAM_DDR_Wait);
  }

  /* Unlock now that we are done */
  DDR_SYNC_UNLOCK();

} /* ddr_enter_self_refresh */

/* ============================================================================
**  Function : ddr_exit_self_refresh
** ============================================================================
*/
/*!
*   @brief
*   This function will put the a particular ddr into active mode, provided that it is currently in self refresh.
*   
*   @details
*   This function will first check to see if the ddr specified by the caller is available and in self refresh, then put it into active
*   mode. If it is not in self refresh, simply do nothing.
*   
*   @param interface_name   -  [IN] The interface to put ddr into self refresh
*   @param chip_select      -  [IN] The chip select to put ddr into self refresh
*   @param clkspeed      -        [IN] The current clock speed
*
*   @par Dependencies
*   This code has to reside in IRAM
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_exit_self_refresh(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  SDRAM_CHIPSELECT csel = SDRAM_CS_NONE;

  /*
    Issue a lock to prevent anyone else from updating ddr
    status while we are working here
  */
  DDR_SYNC_LOCK();

  /* Mark device as active after exiting from self refresh */
  if (interface_name == SDRAM_INTERFACE_0)
  {
    if ( (chip_select & SDRAM_CS0) && (ddr_status.sdram0_cs0 == DDR_SELF_REFRESH) )
    {
      csel |= SDRAM_CS0;
      ddr_status.sdram0_cs0 = DDR_ACTIVE;
    }
    if ( (chip_select & SDRAM_CS1) && (ddr_status.sdram0_cs1 == DDR_SELF_REFRESH) )
    {
      csel |= SDRAM_CS1;
      ddr_status.sdram0_cs1 = DDR_ACTIVE;
    }
  }
  else
  {
    if ( (chip_select & SDRAM_CS0) && (ddr_status.sdram1_cs0 == DDR_SELF_REFRESH) )
    {
      csel |= SDRAM_CS0;
      ddr_status.sdram1_cs0 = DDR_ACTIVE;
    }
    if ( (chip_select & SDRAM_CS1) && (ddr_status.sdram1_cs1 == DDR_SELF_REFRESH) )
    {
      csel |= SDRAM_CS1;
      ddr_status.sdram1_cs1 = DDR_ACTIVE;
    }
  }

  if (csel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Exit_Self_Refresh(0, interface_name, csel, HAL_SDRAM_DDR_Wait);
  }

  /* Unlock now that we are done */
  DDR_SYNC_UNLOCK();

} /* ddr_exit_self_refresh */

/* ============================================================================
**  Function : ddr_enter_deep_power_down
** ============================================================================
*/
/*!
*   @brief
*   This function will put ddr into deep power down mode.
*   
*   @details
*   This function will put ddr into deep power down mode.
*   
*   @param interface_name   -  [IN] The interface to put ddr into DPD
*   @param chip_select      -  [IN] The chip select to put ddr into DPD
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   After a device has entered Deep Power Down, it will be unavailable for use.
*   When it is necessary to bring device out of DPD, a full HW initialization must be
*   executed.
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_enter_deep_power_down(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  SDRAM_CHIPSELECT csel = SDRAM_CS_NONE;

  /*
    Issue a lock to prevent anyone else from updating ddr
    status while we are working here
  */
  DDR_SYNC_LOCK();

  /* Mark device as DPD after entering from deep power down */
  if (interface_name == SDRAM_INTERFACE_0)
  {
    if ( (chip_select & SDRAM_CS0) && (ddr_status.sdram0_cs0 == DDR_ACTIVE) )
    {
      csel |= SDRAM_CS0;
      ddr_status.sdram0_cs0 = DDR_DPD;
    }
    if ( (chip_select & SDRAM_CS1) && (ddr_status.sdram0_cs1 == DDR_ACTIVE) )
    {
      csel |= SDRAM_CS1;
      ddr_status.sdram0_cs1 = DDR_DPD;
    }
  }
  else
  {
    if ( (chip_select & SDRAM_CS0) && (ddr_status.sdram1_cs0 == DDR_ACTIVE) )
    {
      csel |= SDRAM_CS0;
      ddr_status.sdram1_cs0 = DDR_DPD;
    }
    if ( (chip_select & SDRAM_CS1) && (ddr_status.sdram1_cs1 == DDR_ACTIVE) )
    {
      csel |= SDRAM_CS1;
      ddr_status.sdram1_cs1 = DDR_DPD;
    }
  }

  if (csel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Enter_Deep_Power_Down(0, interface_name, csel);
  }

  /* Unlock now that we are done */
  DDR_SYNC_UNLOCK();

} /* ddr_enter_deep_power_down */

/* ============================================================================
**  Function : ddr_exit_deep_power_down
** ============================================================================
*/
/*!
*   @brief
*   This function will take the ddr out of deep power down mode.
*   
*   @details
*   This function will take the ddr out of deep power down mode.
*   
*   @param interface_name  -  [IN] The interface to take ddr out of DPD
*   @param chip_select         -  [IN] The chip select to take ddr out of DPD
*   @param clk_speed          -  [IN] The ddr speed to reinitialize for
*
*   @par Dependencies
*   ddr clock must be returned to an applicable initialization frequency
*   LPDDR1 - no restrictions
*   LPDDR2 - < 50Mhz
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/
void ddr_exit_deep_power_down(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select, uint32 clk_speed)
{
  SDRAM_CHIPSELECT csel = SDRAM_CS_NONE;

  /*
    Issue a lock to prevent anyone else from updating ddr
    status while we are working here
  */
  DDR_SYNC_LOCK();

  /* Mark device as active after exiting from deep power down */
  if (interface_name == SDRAM_INTERFACE_0)
  {
    if ( (chip_select & SDRAM_CS0) && (ddr_status.sdram0_cs0 == DDR_DPD) )
    {
      csel |= SDRAM_CS0;
      ddr_status.sdram0_cs0 = DDR_ACTIVE;
    }
    if ( (chip_select & SDRAM_CS1) && (ddr_status.sdram0_cs1 == DDR_DPD) )
    {
      csel |= SDRAM_CS1;
      ddr_status.sdram0_cs1 = DDR_ACTIVE;
    }
  }
  else
  {
    if ( (chip_select & SDRAM_CS0) && (ddr_status.sdram1_cs0 == DDR_DPD) )
    {
      csel |= SDRAM_CS0;
      ddr_status.sdram1_cs0 = DDR_ACTIVE;
    }
    if ( (chip_select & SDRAM_CS1) && (ddr_status.sdram1_cs1 == DDR_DPD) )
    {
      csel |= SDRAM_CS1;
      ddr_status.sdram1_cs1 = DDR_ACTIVE;
    }
  }

  if (csel != SDRAM_CS_NONE)
  {
    HAL_SDRAM_Exit_Deep_Power_Down(0, interface_name, csel, clk_speed);
  }

  /* Unlock now that we are done */
  DDR_SYNC_UNLOCK();

} /* ddr_exit_deep_power_down */

/* ============================================================================
**  Function : ddr_get_status
** ============================================================================
*/
/*!
*   @brief
*   Returns the status of DDR.
*   
*   @details
*   Returns the ddr_status internal structure so that caller of this API can
*   see which devices are available and what state they are in.
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval returns the status of the devices on all available interface
*   and chip select
*   
*   @sa None
*/

ddr_interface_state ddr_get_status(void)
{
  return ddr_status;
} /* ddr_get_status */

/* ============================================================================
**  Function : ddr_get_info
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
*   @retval ddr_size_infor contains the ddr size and base address
*
*   @sa None
*/

ddr_info ddr_get_info(void)
{

  ddr_info ddr_config_info = {0};
  ddr_size_info *ddr_size = &(ddr_config_info.ddr_size);
  struct ddr_device_params_common *ddr_param_interface_0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
  struct ddr_device_params_common *ddr_param_interface_1 = &(ddr_get_params(SDRAM_INTERFACE_1)->common);

  /* first calculate intermediate size in terms of megabits */
  ddr_size->sdram0_cs0 =  (1ul<<(ddr_param_interface_0->num_rows_cs0 +
                          ddr_param_interface_0->num_cols_cs0 - 20));
  /* Calculate final size in terms of MegaBytes */
  ddr_size->sdram0_cs0 *= ddr_param_interface_0->num_banks_cs0 * 32 >> 3;

  /* first calculate intermediate size in terms of megabits */
  ddr_size->sdram0_cs1 =  (1ul<<(ddr_param_interface_0->num_rows_cs1 +
                          ddr_param_interface_0->num_cols_cs1 - 20));
  /* Calculate final size in terms of MegaBytes */
  ddr_size->sdram0_cs1 *= ddr_param_interface_0->num_banks_cs1 * 32 >> 3;

  /* Calculate for Interface 1 */
  /* first calculate intermediate size in terms of megabits */
  ddr_size->sdram1_cs0 =  (1ul<<(ddr_param_interface_1->num_rows_cs0 +
                          ddr_param_interface_1->num_cols_cs0 - 20));
  /* Calculate final size in terms of MegaBytes */
  ddr_size->sdram1_cs0 *= ddr_param_interface_1->num_banks_cs0 * 32 >> 3;

  /* first calculate intermediate size in terms of megabits */
  ddr_size->sdram1_cs1 =  (1ul<<(ddr_param_interface_1->num_rows_cs1 +
                          ddr_param_interface_1->num_cols_cs1 - 20));
  /* Calculate final size in terms of MegaBytes */
  ddr_size->sdram1_cs1 *= ddr_param_interface_1->num_banks_cs1 * 32 >> 3;

  /* workaround for 6Gbit devices as they do not calculate correctly with normal
   * calculation based on row\col\bank size
   */
  if(((DDR_TYPE)(ddr_param_interface_0->device_type) == DDR_TYPE_LPDDR3) && (ddr_init_done))
  {
    if(ddr_size->sdram0_cs0 != 0)
    {
      if(HAL_SDRAM_Read_MR(0, SDRAM_INTERFACE_0, SDRAM_CS0, 0x8) == 0x3B)
      {
        ddr_size->sdram0_cs0 = 768;
      }
    }
    if(ddr_size->sdram0_cs1 != 0)
    {
      if(HAL_SDRAM_Read_MR(0, SDRAM_INTERFACE_0, SDRAM_CS1, 0x8) == 0x3B)
      {
        ddr_size->sdram0_cs1 = 768;
      }
    }
    if(ddr_size->sdram1_cs0 != 0)
    {
      if(HAL_SDRAM_Read_MR(0, SDRAM_INTERFACE_1, SDRAM_CS0, 0x8) == 0x3B)
      {
        ddr_size->sdram1_cs0 = 768;
      }
    }
    if(ddr_size->sdram1_cs1 != 0)
    {
      if(HAL_SDRAM_Read_MR(0, SDRAM_INTERFACE_0, SDRAM_CS1, 0x8) == 0x3B)
      {
        ddr_size->sdram1_cs1 = 768;
      }
    }
  }
  ddr_size->sdram0_cs0_addr = HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_0, SDRAM_CS0);
  ddr_size->sdram0_cs1_addr = HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_0, SDRAM_CS1);
  ddr_size->sdram1_cs0_addr = HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_1, SDRAM_CS0);
  ddr_size->sdram1_cs1_addr = HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_1, SDRAM_CS1);

  /* Check to see if interleave is turned on */
  ddr_config_info.interleaved_memory = ddr_param_interleaved();

  /*
    Parse for ddr type from ddr parameters
    it is assumed that interface 0 and interface 1 has to have
    the same ddr type
  */
  ddr_config_info.ddr_type = (DDR_TYPE)(ddr_param_interface_0->device_type);

  return ddr_config_info;
}/* ddr_get_info */

/* ============================================================================
**  Function : ddr_get_size
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr size and base address
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
*   @retval ddr_size_infor contains the ddr size and base address
*   
*   @sa None
*/

ddr_size_info ddr_get_size(void)
{
  ddr_info ddr_physical_info = ddr_get_info();

  /* Check to see if interleave is turned on for CS0 */
  if(ddr_physical_info.interleaved_memory & SDRAM_CS0)
  {
    /* Interleave mode is on for CS0 , double the size of CS0, and do not populate interface 1 */
    ddr_physical_info.ddr_size.sdram0_cs0 *= 2;
    ddr_physical_info.ddr_size.sdram1_cs0 = 0;
  }

  if(ddr_physical_info.interleaved_memory & SDRAM_CS1)
  {
    /* Interleave mode is on for CS1 , double the size of CS1, and do not populate interface 1 */
    ddr_physical_info.ddr_size.sdram0_cs1 *= 2;
    ddr_physical_info.ddr_size.sdram1_cs1 = 0;
  }

  return ddr_physical_info.ddr_size;

} /* ddr_get_size */

/* ============================================================================
**  Function : ddr_get_partition
** ============================================================================
*/
/*!
*   @brief
*   Returns the ddr partitions and base address
*   
*   @details
*	This API will return the number of partitions on each interface along with the 
*	base address
*   
*  @param  None
*   
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval ddr_size_partition contains the ddr partitions and base address
*   
*   @sa None
*/

ddr_size_partition ddr_get_partition(void)
{
  ddr_size_partition ddr_partitions = {0};
  uint8 ddr_interleave_status = ddr_param_interleaved();
  struct ddr_device_params_common *ddr_param_interface_0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
  struct ddr_device_params_common *ddr_param_interface_1 = &(ddr_get_params(SDRAM_INTERFACE_1)->common);
  
  /* 
    because we only support segment masking due to bank interleaving,
    we always have 8 partitions per device
  */  
  if(ddr_param_interface_0->num_rows_cs0)
  {
    ddr_partitions.sdram0_cs0.sdram_addr = HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_0, SDRAM_CS0);
    ddr_partitions.sdram0_cs0.num_partitions = 8;
  }
  
  if(ddr_param_interface_0->num_rows_cs1)
  {  
    ddr_partitions.sdram0_cs1.sdram_addr = HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_0, SDRAM_CS1);  
    ddr_partitions.sdram0_cs1.num_partitions = 8;
  }

  if((ddr_interleave_status & SDRAM_CS0) == 0)
  {
    /* Non interleave mode for CS0 */

    /* non interleaved configuration has interface 1 */  
    if(ddr_param_interface_1->num_rows_cs0)
    {    
      ddr_partitions.sdram1_cs0.sdram_addr = HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_1, SDRAM_CS0);
      ddr_partitions.sdram1_cs0.num_partitions = 8;
    }
  }

  if((ddr_interleave_status & SDRAM_CS1) == 0)
  {
    /* Non interleave mode for CS1 */

    /* non interleaved configuration has interface 1 */
    if(ddr_param_interface_1->num_rows_cs1)
    {
      ddr_partitions.sdram1_cs1.sdram_addr = HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE_1, SDRAM_CS1);
      ddr_partitions.sdram1_cs1.num_partitions = 8;
    }
  }

  return ddr_partitions;
}
