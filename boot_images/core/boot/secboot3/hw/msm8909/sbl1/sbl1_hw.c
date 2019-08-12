/*=============================================================================

                         SBL1 Hardware Initialization

GENERAL DESCRIPTION
  This file does basic hardware initialization at power up.

Copyright 2014-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/msm8909/sbl1/sbl1_hw.c#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
04/01/15   aus       Initialize DDR heap before device programmer execution
02/25/15   tj        Disable PMIC init for Device Programmer DDR
12/03/14   xl        Add sbl1_ddr_set_params_enhance, enhance set
08/05/14   sk        8909 SBL NAND changes
07/22/14   sk        Changes for 8909 EMMC
06/20/14   lm        Fixed dead/weak battery charging by moving boot_pm_driver_init()
					 after ddr_init and cleaned the code.
06/18/14   SK        Moved boot_ddr_test func out of condition check.
06/06/14   lm        Moved boot_pm_init_driver func before calling boot_pre_ddr_clock_init call
05/20/14   lm        Call boot_railway_init() for DDR training
03/21/14   ck        Added logic to save reset_status_register to imem cookies before clearing
03/18/14   ck        Updated boot_hw_reset calls as they now take a reset type parameter
03/14/14   ck        Moved boot_busywait_init to sbl1_main_ctl as it needs to be done before boot logger init
03/03/14   ck        Removing SpmiInit and SpmiCfg from sbl1_hw_init per SPMI team as PMIC driver does this now
02/18/14   ck        Added logic to sbl1_hw_init to clear PMIC_ABNORMAL_RESIN_STATUS
10/17/13   ck        Temp zeroing of ddr training memory location until ddr team delivers
08/07/13   sr        Removed boot_ddr_debug call .
07/26/13   rj        Calling i2C init to fix eeprom init issue
07/11/13   rj        Calling boot_pre_ddr_clock_init after CDT configuration is 
                     done to dynamically detect ddr and setup the clock plan
06/18/13   sl        Call boot_ddr_debug()
06/06/13   dh        Backup gcc reset status then clear it
06/14/13   dh        Only load ddr training data if sbl is not in dload mode
05/21/13   yp        Turn on vibrator when device power up in normal mode.
04/04/13   dh        Move boot_DALSYS_InitMod to early sbl1_main_ctl
04/02/13   dh        Use boot_dload_is_dload_mode_set instead of uefi dump cookie
                     to determin if device is in dload mode
04/11/13   sl        Relocate boot_ddr_test() call
04/03/13   sl        Call Clock API to get DDR speed
03/21/13   dh        Add memory barrier in boot_ddr_post_init
03/07/12   jz        Cleanup logging
12/13/12   jz        Change clock frequency back to MAX after DDR training is done
12/04/12   dh        Move boot_Tsens_Init to sbl1_hw_init
11/26/12   dh        Skip DDR training if UEFI dload cookie is set
11/12/12   sl        Enable DDR Test Framework
11/05/12   dh        Move thermal management code to common file
10/09/12   dh        Add boot_ICB_Segments_Init right after ddr init to configure
                     bimc slave address range
                     Add boot_SpmiCfg_Init before boot_SpmiBus_Init
10/09/12   dh        Only enable fast debug feature if FEATURE_BOOT_FAST_DEBUG 
                     is defined
09/26/12   jz        Compile out boot_debug_mode_enter for Virtio testing
09/25/12   dh        Added pm_driver_init to allow PMIC API usage and pm_oem_init
	  	  	         to allow customer to call PMIC API in boot for
	  	             their desirable PMIC configurations.
08/30/12   dh        Add boot log for clock and pmic functions 
08/16/12   AJC       Added boot_debug_mode_enter for FASTDEBUG feature
08/01/12   dh        Add sbl1_hw_deinit
07/23/12   dh        Add sbl1_set_ddr_training_data and sbl1_wait_for_ddr_training
07/16/12   dh        Move spmi and pmic init to sbl1_hw_init
06/18/12   dh        Switch to boot external driver api for tsensor and bus API
06/11/12   dh        Add sbl1_check_device_temp, check temp at the beginning of
                     sbl1_hw_init
06/08/12   dh        Add ICB_Config_Init in sbl1_hw_init
05/29/12   dh        Move boot_clock_init to sbl1_hw_init_secondary
                     Add boot_busywait_init in sbl1_hw_init
                     Remove gpio init from sbl1_hw_init_secondary since it must be 
                     called after smem is available.
                     Rename sbl1_clk_regime_ram_dump_init to sbl1_hw_dload_init
05/08/12   dh        Add clock init api
10/18/11   dh        Initial revision
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_if.h"
#include "sbl1_hw.h"
#include "sbl1_mc.h"
#include "boothw_target.h"
#include "boot_dload.h"
#include "boot_logger.h"
#include "boot_config_data.h"
#include "boot_util.h"
#include "boot_thermal_management.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_extern_pmic_interface.h"
#include "boot_extern_dal_interface.h"
#include "boot_extern_tlmm_interface.h"
#include "boot_extern_bus_interface.h"
#include "boot_extern_tsensor_interface.h"
#include "boot_extern_power_interface.h"
#include "boot_shared_imem_cookie.h"
#include "string.h"
#include "boot_cache_mmu.h"
#include "msmhwioreg.h"

#include "ddr_drivers.h"
#include "boot_extern_bam_interface.h"
#include "AdcBoot.h"

#include "boot_authenticator.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* backed up GCC_RESET_STATUS for ramdump */
static uint32 reset_status_register = 0;

/* For initializing the heap before jump to device programmer */
extern byte boot_external_heap[BOOT_EXTERNAL_HEAP_SIZE];


/*=============================================================================

                              FUNCTION DEFINITIONS

=============================================================================*/


/*===========================================================================

**  Function :  sbl1_ddr_init

** ==========================================================================
*/
/*!
* 
* @brief
*   Initialize DDR device.
* * 
* @par Dependencies 
*  Following Api's need to be called first:
*    sbl1_hw_init : To have ddr clocks setup.
*    sbl1_ddr_set_params : To have DDR config parameters setup.
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_init(bl_shared_data_type *bl_shared_data)
{
  ddr_info sbl1_ddr_info;
  /* Initialize DDR */
  boot_ddr_initialize_device(boot_clock_get_ddr_speed());
  
  /* Configure DDR slave segments address range */
  sbl1_ddr_info = boot_ddr_get_info();
  boot_ICB_Segments_Init( "/dev/icbcfg/boot", &sbl1_ddr_info);

  /* PMIC driver must be initialized after DDR init for dead/weak battery charging
     and before ddr training as its uses PMIC API's */
   /*compile out pmic and clock api for rumi */
#if (!defined(FEATURE_RUMI_BOOT))
  /* PMIC image not loaded for Device Programmer */

  if (bl_shared_data->build_type != SBL_BUILD_DEVICEPROGRAMMER_DDR) {
    /* Initialize PMIC Driver - Allowing API usage */
  
  boot_log_message("pm_driver_init, Start");
  boot_log_start_timer(); 
  
  /* Initialize PMIC Driver - Allowing API usage */
  BL_VERIFY((boot_pm_driver_init() == PM_ERR_FLAG__SUCCESS), BL_ERR_SBL);
  
  boot_log_stop_timer("pm_driver_init, Delta"); 

  }  
#endif /* FEATURE_RUMI_BOOT*/
  if (boot_ddr_params_is_training_required() == TRUE &&
      boot_dload_is_dload_mode_set() == FALSE)
  {
    boot_ddr_do_ddr_training();
  }
  boot_ddr_test(boot_clock_get_ddr_speed());

  if (bl_shared_data->build_type == SBL_BUILD_DEVICEPROGRAMMER_DDR) {
  /* Add the DDR heap to DAL */
  boot_DALSYS_HeapInit(boot_external_heap, BOOT_EXTERNAL_HEAP_SIZE, TRUE);
  }
 
} /* sbl1_ddr_init() */

#ifndef BOOT_PRE_SILICON
#ifdef  FEATURE_BOOT_FAST_DEBUG
/*===========================================================================

**  Function :  sbl1_enter_fastdebug

** ==========================================================================
*/
/*!
* 
* @brief
*   Enters debug mode if PMIC GPIO key is pressed while booting up
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
static volatile boolean enter_fastdebug = FALSE;
static void  boot_debug_mode_enter (void)
{
  
	uint8 data;
	uint32 addr, readlen;
	
	// On 8974, GPIO4 is used for the focus key
	// Use a pull up value of 30uA and a voltage source of VIN0

    data = 0x2;
	addr = 0xC341;
	BL_VERIFY((SPMI_BUS_SUCCESS == boot_SpmiBus_WriteLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1)), BL_ERR_SBL);

  	data = 0x0;
    addr = 0xC340;
	BL_VERIFY((SPMI_BUS_SUCCESS == boot_SpmiBus_WriteLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1)), BL_ERR_SBL);


	// Slave & Priority  = 0
	data = 0x0;
	addr = 0xC342;
	BL_VERIFY((SPMI_BUS_SUCCESS == boot_SpmiBus_WriteLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1)), BL_ERR_SBL);

	// Then enable the GPIO
	data = 0x80;
	addr = 0xC346;
	BL_VERIFY((SPMI_BUS_SUCCESS == boot_SpmiBus_WriteLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1)), BL_ERR_SBL);

	// Now poll the value
	addr = 0xC308;
	if (SPMI_BUS_SUCCESS == boot_SpmiBus_ReadLong(0, (SpmiBus_AccessPriorityType)0, addr, &data, 1, &readlen))
	{
		// If GPIO is pressed, bit 0 will be 0 (active low)
		if (!(data & 0x1))
		{
			enter_fastdebug = TRUE;
			while(enter_fastdebug);
		}
	}
}
#endif
#endif

	
/*===========================================================================

**  Function :  sbl1_hw_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This function performs hardware initialization.
* 
*   Only common hardware that is needed for flash
*   initialization is initialized here.
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_init(bl_shared_data_type *bl_shared_data)
{  
#ifndef BOOT_PRE_SILICON
#ifdef FEATURE_BOOT_FAST_DEBUG
  /* Check if we are going to enter debug mode */
  boot_debug_mode_enter();
#endif
#endif
  /* Initialize BAM interface */
  boot_bam_drv_init();

  /* Initialize temperature sensor */
  BL_VERIFY(boot_Tsens_Init() == TSENS_SUCCESS,BL_ERR_SBL);
  
   
#if (!defined(FEATURE_RUMI_BOOT))
  /* Calling here to ensure eeprom init goes fine for CDT read */
    /* PMIC image not loaded for Device Programmer */
  boot_pre_i2c_clock_init();
  
  if (bl_shared_data->build_type != SBL_BUILD_DEVICEPROGRAMMER_DDR) {
  boot_log_message("pm_device_init, Start");
  boot_log_start_timer();
  
  /* Initialize the pmic */
  BL_VERIFY((boot_pm_device_init() == PM_ERR_FLAG__SUCCESS), BL_ERR_SBL);  
  
  boot_log_stop_timer("pm_device_init, Delta");
  }
#endif /*FEATURE_RUMI_BOOT*/
 /* Check the temperature */
  boot_check_device_temp();

  /* There is a HW bug in 8916 where PMIC_ABNORMAL_RESIN_STATUS is set accidently.
     Just clear the bit regardless. */
  HWIO_GCC_RESET_STATUS_OUTM(HWIO_GCC_RESET_STATUS_PMIC_ABNORMAL_RESIN_STATUS_BMSK,
                             0);

} /* sbl1_hw_init() */


/*===========================================================================

**  Function :  sbl1_hw_deinit

** ==========================================================================
*/
/*!
* 
* @brief
*   This function deinit the hardware that's not needed beyond sbl1
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_deinit()
{

  /* Call clock exit boot to disable unneeded clock*/
  BL_VERIFY(boot_clock_exit_boot() == TRUE,BL_ERR_SBL);
  
} /* sbl1_hw_deinit() */


/*===========================================================================

**  Function :  sbl1_hw_dload_init

** ==========================================================================
*/
/*!
* 
* @brief
*   This function initializes the necessary clocks for dload
*   We only do so when auth is disabled 
* 
* @param[in] bl_shared_data Pointer to shared data
*
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_dload_init(bl_shared_data_type *bl_shared_data)
{
  boot_dload_clock_init();
}


/*===========================================================================

**  Function :  sbl1_hw_init_secondary

** ==========================================================================
*/
/*!
* 
* @brief
*   This function performs secondary hardware initialization.
* 
*   DDR is initialized here. This needs to be done after trustzone is loaded.
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_hw_init_secondary( bl_shared_data_type *bl_shared_data )
{ 
  /*compile out pmic and clock api for rumi */
#if (!defined(FEATURE_RUMI_BOOT))
  
  /* First save gcc reset status to shared IMEM, then clear it */
  reset_status_register = HWIO_IN(GCC_RESET_STATUS);

  if (boot_shared_imem_cookie_ptr)
  {
    boot_shared_imem_cookie_ptr->reset_status_register = reset_status_register;
  }

  HWIO_OUT(GCC_RESET_STATUS,0);
  
  if (bl_shared_data->build_type != SBL_BUILD_DEVICEPROGRAMMER_DDR) {
  /* pm_oem_init is an function for customer to access and call PMIC APIs
     for OEM desirable PMIC configurations in boot. */
  BL_VERIFY((boot_pm_oem_init() == PM_ERR_FLAG__SUCCESS), BL_ERR_SBL);
  }
  
  /* Init ADC as we need to store FAB id so that apps image can read it to apply
     temperature compensation for vbatt */
  AdcBoot_Init();

  if(!boot_dload_is_dload_mode_set())
  {
  	/*turn on vibrator*/
 	 BL_VERIFY(boot_pm_vib_on()== PM_ERR_FLAG__SUCCESS,BL_ERR_SBL);
  }

  boot_log_message("clock_init, Start");
  boot_log_start_timer();  
  
  BL_VERIFY((boot_clock_init() == TRUE), BL_ERR_SBL);
  
  boot_log_stop_timer("clock_init, Delta");


  
#endif /*FEATURE_RUMI_BOOT*/
   
} /* sbl1_hw_init_secondary() */


/*===========================================================================

**  Function :  sbl1_hw_get_reset_status

** ==========================================================================
*/
/*!
* 
* @brief
*   This function returns the value of reset status register saved from this boot
* 
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
uint32 sbl1_hw_get_reset_status( void )
{
   return reset_status_register;
}  /* sbl1_hw_get_reset_status */

/*===========================================================================

**  Function :  sbl1_ddr_set_params

** ==========================================================================
*/
/*!
* 
* @brief
*   Set DDR configuration parameters
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
*  	This Api needs to be called :
*  Before: sbl1_ddr_init
*  After: boot_config_data_table_init
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_set_params(  bl_shared_data_type *bl_shared_data  )
{
  uint32 ddr_cdt_params_size = 0;
  struct cdt_info *config_data_table_info;
  uint8 *ddr_cdt_params_ptr;
  ddr_info ddr_type_info;
  
  boot_log_message("sbl1_ddr_set_params, Start");
  boot_log_start_timer();  
  
  /* Extract ddr data block from configuration data table(CDT) */
  config_data_table_info = 
                    bl_shared_data->sbl_shared_data->config_data_table_info;
                    
  ddr_cdt_params_ptr = 
                    boot_get_config_data_block(config_data_table_info->cdt_ptr,
                                               CONFIG_DATA_BLOCK_INDEX_V1_DDR,
                                               &ddr_cdt_params_size);
  
  /* Make sure cdt param size is valid */
  BL_VERIFY((ddr_cdt_params_size <= CONFIG_DATA_TABLE_MAX_SIZE) &&
            (ddr_cdt_params_ptr != NULL),  BL_ERR_SBL );

  /* Copy CDT DDR paramters to shared IMEM */
  qmemcpy((void*)SHARED_IMEM_BOOT_CDT_BASE,
          ddr_cdt_params_ptr,
          ddr_cdt_params_size);
  
  /* Pass cdt parameter to DDR driver */
  BL_VERIFY(boot_ddr_set_params((void*)SHARED_IMEM_BOOT_CDT_BASE, ddr_cdt_params_size) == TRUE, BL_ERR_SBL);
  
  /* Pass ddr training data to DDR driver */
  sbl1_load_ddr_training_data();

  /* Get the ddr type by reading the cdt */
  ddr_type_info = boot_ddr_get_info();

  /* Initialize the railway driver */
  /* NOTE: this driver API is wrapped in boot_extern_power_interface.h, 
      which needs to be enabled by turning on BOOT_EXTERN_POWER_COMPLETED in target.builds */
  boot_railway_init();
 
  boot_log_message("cpr_init, Start");
  boot_log_start_timer();

  /* Call CPR init to settle the voltages to the recommended levels*/
  boot_cpr_init();
 
  boot_log_stop_timer("cpr_init, Delta");

  boot_log_message("Pre_DDR_clock_init, Start");
  boot_log_start_timer();
  /* Initialize the pre ddr clock */ 
  BL_VERIFY((boot_pre_ddr_clock_init(ddr_type_info.ddr_type) == TRUE), BL_ERR_SBL);
  
  boot_log_stop_timer("Pre_DDR_clock_init, Delta");

  /*ICB config api will configure address range for NOCs */
  BL_VERIFY(boot_ICB_Config_Init("/dev/icbcfg/boot") == ICBCFG_SUCCESS,BL_ERR_SBL);

  boot_log_stop_timer("sbl1_ddr_set_params, Delta");
  
} /* sbl1_ddr_set_params() */

/*===========================================================================

**  Function :  sbl1_ddr_set_params_enhance

** ==========================================================================
*/
/*!
* 
* @brief
*   Set DDR DC Timing parameters which are stored in CDB2 and CDB3, enhance set
* 
* @param[in] bl_shared_data Pointer to shared data
* 
* @par Dependencies
* 
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_ddr_set_params_enhance(  bl_shared_data_type *bl_shared_data  )
{

  #if (defined(FEATURE_BOOT_EXTERN_DDR_ENHANCE_SET_ENABLED))
  struct cdt_info *config_data_table_info;

  uint32 cdt_block_ddr_boot_set_size=0;
  uint8 *cdt_block_ddr_boot_set_ptr;
  uint32 cdt_block_ddr_post_boot_set_size=0;
  uint8 *cdt_block_ddr_post_boot_set_ptr;
  boot_boolean is_auth_enabled = FALSE;
  uint32 status=FALSE;
  
  /* Extract ddr data block from configuration data table(CDT) */
  config_data_table_info = 
                    bl_shared_data->sbl_shared_data->config_data_table_info;

  do
  {	  
 	  boot_is_auth_enabled(&is_auth_enabled);
	  if(is_auth_enabled==TRUE)
	  {
		break;
	  }
  
	  cdt_block_ddr_boot_set_ptr=
	  			boot_get_config_data_block(config_data_table_info->cdt_ptr,
	                                               CONFIG_DATA_BLOCK_INDEX_V1_BOOT_SETTINGS,&cdt_block_ddr_boot_set_size);
	  if(cdt_block_ddr_boot_set_ptr==NULL)
	  {
		break;
	  }	   
	  cdt_block_ddr_post_boot_set_ptr=
	  			boot_get_config_data_block(config_data_table_info->cdt_ptr,
	                                               CONFIG_DATA_BLOCK_INDEX_V1_POST_BOOT_SETTINGS,&cdt_block_ddr_post_boot_set_size);
	  if(cdt_block_ddr_post_boot_set_ptr==NULL)
	  {
		break;
	  }  
	  /*Set DDR DC parameters stored in CDB2 */
	  status=boot_ddr_set_params_enhance((void*)cdt_block_ddr_boot_set_ptr,cdt_block_ddr_boot_set_size);

	   /*Set DDR DC parameters stored in CDB3 */
	  status=boot_ddr_set_params_enhance((void*)cdt_block_ddr_post_boot_set_ptr,cdt_block_ddr_post_boot_set_size);

  }while(0);
  if(status==TRUE)
  {
 	boot_log_message("sbl1_ddr_set_params_enhance, CDB2&CDB3 only for debugging");
  }
  #endif /*FEATURE_BOOT_EXTERN_DDR_ENHANCE_SET_ENABLED*/
  
}
/*===========================================================================

**  Function :  sbl1_load_ddr_training_data

** ==========================================================================
*/
/*!
* 
* @brief
*   If ddr training is required, read the ddr training data from partition
*   and pass it to ddr driver.
* 
* @param[in] 
*   None 
*     
* @par Dependencies
*   Must be called before sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_load_ddr_training_data()
{

  uint32 ddr_params_training_data_size = 0;
  boot_boolean success = FALSE;
  uint8 *ddr_training_data_ptr = (uint8 *)SCL_DDR_TRAINING_DATA_BUF_BASE;
//  uint8 *ddr_training_data_ptr = (uint8 *)0;
  
  /*Read the ddr partition if ddr training is required and not in dload mode */
  if((boot_ddr_params_is_training_required() == TRUE) &&
     (!boot_dload_is_dload_mode_set()))
  {
    /* Get the size of training data */
    boot_ddr_params_get_training_data(&ddr_params_training_data_size);
   
   /* Read the ddr training data out, we use
     SCL_DDR_TRAINING_DATA_BUF_BASE which is tz base as a temporary buffer */
    sbl1_load_ddr_training_data_from_partition(ddr_training_data_ptr,
                                               ddr_params_training_data_size);
    
    BL_VERIFY(ddr_training_data_ptr != NULL, BL_ERR_NULL_PTR);
    
    /* Pass the training data to DDR driver */
    success = boot_ddr_params_set_training_data((void *)ddr_training_data_ptr,
                                               ddr_params_training_data_size);

    BL_VERIFY(success, BL_ERR_SBL);
  }
}


/*===========================================================================

**  Function :  sbl1_wait_for_ddr_training

** ==========================================================================
*/
/*!
* 
* @brief
*   Wait for RPM to execute DDR training sequence. 
*   If ddr training parameter data is updated, save it to storage device.
* 
* @param[in] 
*   None 
*     
* @par Dependencies
*   Must be called after sbl1_ddr_init
*   
* @retval
*   None
* 
* @par Side Effects
*   None
* 
*/
void sbl1_wait_for_ddr_training(void)
{
  uint32 ddr_params_training_data_size = 0;
  void* ddr_training_data_ptr = NULL;
  
  /* Check if DDR parameter training is required */
  if (boot_ddr_params_is_training_required() == TRUE)
  {
    /* Only perform DDR training if SBL is not in dload mode */
    if(!boot_dload_is_dload_mode_set())
    { 
      boot_log_message("sbl1_wait_for_ddr_training, Start");
      boot_log_start_timer();
      
      memory_barrier();

      /* If boot_ddr_post_init returns TRUE then ddr training data is updated, 
        we need to save it to storage device */
      if(boot_ddr_post_init() == TRUE)
      {
        memory_barrier();
        
        ddr_training_data_ptr = 
            boot_ddr_params_get_training_data(&ddr_params_training_data_size);
        
        BL_VERIFY((ddr_training_data_ptr != NULL) &&
                  (ddr_params_training_data_size != 0),
                  BL_ERR_NULL_PTR);
        
        /*Save the updated training data to storage device */
        sbl1_save_ddr_training_data_to_partition(ddr_training_data_ptr, 
                                                ddr_params_training_data_size);
                                                
        /* Do a reset after training data is saved */
        boot_hw_reset(BOOT_HARD_RESET_TYPE);
      }

      /* Speed back up to max frequency */
      boot_clock_set_cpu_perf_level(CLOCK_BOOT_PERF_MAX);
      boot_clock_set_L2_perf_level(CLOCK_BOOT_PERF_MAX);
    
      boot_ddr_test(0);

      boot_log_stop_timer("sbl1_wait_for_ddr_training, Delta");
    }
  }
}
