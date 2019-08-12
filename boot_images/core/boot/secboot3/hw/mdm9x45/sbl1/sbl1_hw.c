/*=============================================================================

                         SBL1 Hardware Initialization

GENERAL DESCRIPTION
  This file does basic hardware initialization at power up.

Copyright 2012-2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/secboot3/hw/mdm9x45/sbl1/sbl1_hw.c#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
10/27/14   jz        Removed sbl1_hw_set_boot_done_gpio (not needed for 9x45)
10/09/14   jz        Pass the new base addr of reconstructed MBA image to modem PBL
09/25/14   jz        Added sbl1_hw_get_usb_speed to check USB_SS_ENABLE
09/16/14   jz        Removed SPMI related init calls, which will be done in PMIC init
08/04/14   jz        Replaced BOOT_PRE_SILICON with FEATURE_RUMI_BOOT for skipping clock/pmic calls on RUMI
07/17/14   jz        Added BOOT_PRE_SILICON to skip clock driver in RUMI
07/16/14   jz        Fixed getting image entry point from qsee interface
07/01/14   tj	     Calling pm_oem_init function from sbl1_init_hw_secondary() 
04/11/14   jz        Updated for boot flows changes
03/21/14   ck        Added logic to save reset_status_register to imem cookies before clearing
02/24/13   jz        Updated for Bear
10/28/13   Kedara    Move boot_dload_clock_init to dload_mem_debug_target_init
10/25/13   Kedara    Update DDR config settings
08/23/13   Kedara    Add correct ddr_type_info
06/19/13   Kedara    Initial revision. Branch from 9x25, make target specific
                     changes.
=============================================================================*/

/*=============================================================================

                            INCLUDE FILES FOR MODULE

=============================================================================*/
#include "boot_comdef.h"
#include "boot_sbl_if.h"
#include "sbl1_hw.h"
#include "DALSys.h"
#include "boot_dload.h"
#include "boot_logger.h"
#include "boot_config_data.h"
#include "boot_util.h"
#include "boot_extern_clk_interface.h"
#include "boot_extern_ddr_interface.h"
#include "boot_extern_pmic_interface.h"
#include "boot_extern_dal_interface.h"
#include "boot_extern_tlmm_interface.h"
#include "string.h"
#include "boot_extern_subsys_interface.h"
#include "boot_rmb.h"
#include "boot_target.h"
#include "boot_extern_bam_interface.h"
#include "boot_cache_mmu.h"
#include "boot_authenticator.h"
#include "boot_extern_bus_interface.h"
#include "boot_extern_crypto_interface.h"
#include "boot_extern_tsensor_interface.h"
#include "boot_thermal_management.h"
#include "boot_shared_imem_cookie.h"
#include "boot_loader.h"
#include "boot_extern_power_interface.h"

/*=============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

=============================================================================*/

/* Boolean to indicate if Q6 was loaded and hw was setup */
static boolean mpss_out_of_reset = FALSE;

/* Override_2 register value */
static uint32 override2_val = 0;

/* Override_4 register value */
static uint32 override4_val = 0;

/* backed up GCC_RESET_STATUS for ramdump */
static uint32 reset_status_register = 0;

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
void sbl1_ddr_init()
{
  ddr_info sbl1_ddr_info;

  boot_log_message("Pre_DDR_clock_init, Start");
  boot_log_start_timer();
  
  /* Get the ddr type by reading the cdt */
  sbl1_ddr_info = boot_ddr_get_info();
  
#if (!defined(FEATURE_RUMI_BOOT))
  /*compile out clock api for rumi */
  /* Initialize the pre ddr clock */ 
  BL_VERIFY((boot_pre_ddr_clock_init(sbl1_ddr_info.ddr_type) == TRUE), BL_ERR_SBL);
#endif
  
  /*ICB config api will configure address range for NOCs */
  BL_VERIFY(boot_ICB_Config_Init("/dev/icbcfg/boot") == ICBCFG_SUCCESS,BL_ERR_SBL);

  boot_log_stop_timer("Pre_DDR_clock_init, Delta");
  
  /* Initialize DDR */
  boot_ddr_initialize_device(boot_clock_get_ddr_speed());
  
  /* Test DDR */
  boot_ddr_test(boot_clock_get_ddr_speed());
  
  /* Configure DDR slave segments address range */
  sbl1_ddr_info = boot_ddr_get_info();
  boot_ICB_Segments_Init( "/dev/icbcfg/boot", &sbl1_ddr_info);
  
} /* sbl1_ddr_init() */

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
void sbl1_hw_init( void )
{
  /* Initialize BAM interface */
  boot_bam_drv_init();

  /* Initialize temperature sensor */
  BL_VERIFY(boot_Tsens_Init() == TSENS_SUCCESS,BL_ERR_SBL);
  
  /* Calling here to ensure eeprom init goes fine for CDT read */
  boot_pre_i2c_clock_init();

#if (!defined(FEATURE_RUMI_BOOT))
  boot_log_message("pm_device_init, Start");
  boot_log_start_timer();

  /* Initialize the pmic */
  BL_VERIFY((boot_pm_device_init() == PM_ERR_FLAG__SUCCESS), BL_ERR_SBL);  
  
  boot_log_stop_timer("pm_device_init, Delta");
  
#endif /*FEATURE_RUMI_BOOT*/
 /* Check the temperature */
  boot_check_device_temp();

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

  boot_log_message("pm_driver_init, Start");
  boot_log_start_timer();

  /*First save gcc reset status then clear it */
  reset_status_register = HWIO_IN(GCC_RESET_STATUS);

  if (boot_shared_imem_cookie_ptr)
  {
    boot_shared_imem_cookie_ptr->reset_status_register = reset_status_register;
  }

  HWIO_OUT(GCC_RESET_STATUS,0);


  /* Initialize PMIC Driver - Allowing API usage */
  BL_VERIFY((boot_pm_driver_init() == PM_ERR_FLAG__SUCCESS), BL_ERR_SBL);    

  /* pm_oem_init is an function for
  OEM desirable PMIC configurations in boot. */
  BL_VERIFY((boot_pm_oem_init() == PM_ERR_FLAG__SUCCESS), BL_ERR_SBL);  

  boot_log_stop_timer("pm_driver_init, Delta"); 
  
  boot_log_message("clock_init, Start");
  boot_log_start_timer();  
  
  boot_rbcpr_set_cx_mx_to_safe_turbo_voltage();
  BL_VERIFY(boot_clock_init() == TRUE, BL_ERR_SBL);
  
  boot_log_stop_timer("clock_init, Delta");
  
#endif /*FEATURE_RUMI_BOOT*/
} /* sbl2_hw_init_secondary() */


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
*   Set DDR configuration parameters.
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
  
  boot_log_stop_timer("sbl1_ddr_set_params, Delta");

} /* sbl1_ddr_set_params() */

/*===========================================================================

**  Function :  sbl1_hw_write_override_register

** ==========================================================================
*/
/*!
* 
* @brief
*   Writes to the one-time write OVERRIDE register
* 
* @param[in]
*  None
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
void sbl1_hw_write_override_register( void )
{

  /* As apps image is loaded at the end, we have accumulated the value in the 
     over-ride register. Program the one-time writable OVERRIDE registers */
  HWIO_OUT(OVERRIDE_2, override2_val);

  return;
}

/*===========================================================================

**  Function :  sbl1_hw_setup_rpm_proc

** ==========================================================================
*/
/*!
* 
* @brief
*   This function does the set-up for the RPM processor 
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
void sbl1_release_rpm_proc( bl_shared_data_type *bl_shared_data )
{
  uint32 entry_point;

  BL_VERIFY((bl_shared_data != NULL), BL_ERR_NULL_PTR_PASSED );
  
  entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_QSEE_SW_TYPE);

 /* Setup the RPM only if it was loaded */
 if (entry_point)
 {
    /* Setup Clocks for the RPM processor */
    boot_clock_setup_processor(CLOCK_PROCESSOR_RPM);

    /* Flush cache before bringing processor out of reset */
    mmu_flush_cache();

    /* Bring RPM out of reset */   
    boot_clock_enable_processor(CLOCK_PROCESSOR_RPM);
 }
} /* sbl2_hw_setup_lpass_proc */

/*===========================================================================

**  Function :  sbl1_hw_jtag_enable_rpm

** ==========================================================================
*/
/*!
* 
* @brief
*   Check the certificate to see if JTAG over-ride register needs to be 
*   programmed to enable access via JTAG.
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
void sbl1_hw_jtag_enable_rpm( bl_shared_data_type *bl_shared_data )
{
  secboot_verified_info_type *sbl_auth_verified_info;
  uint32 entry_point;
  
  BL_VERIFY((bl_shared_data != NULL), BL_ERR_NULL_PTR_PASSED );
  
  entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_RPM_FW_SW_TYPE);
  
  /* If RPM image is loaded then update the value of the RPM over-ride */
  if (entry_point)
  {
    sbl_auth_verified_info = 
      (secboot_verified_info_type *) boot_get_sbl_auth_verified_info
      (bl_shared_data);
	  
    BL_VERIFY((sbl_auth_verified_info != NULL), BL_ERR_NULL_PTR_PASSED );

    if(SECBOOT_DEBUG_ENABLE == sbl_auth_verified_info->enable_debug)
    {
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_RPM_DBGEN_DISABLE));             
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_RPM_DAPEN_DISABLE));
    }
  }
}

/*===========================================================================

**  Function :  sbl1_hw_jtag_enable_apps

** ==========================================================================
*/
/*!
* 
* @brief
*   Check the certificate to see if JTAG over-ride register needs to be 
*   programmed to enable access via JTAG.
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
void sbl1_hw_jtag_enable_apps( bl_shared_data_type *bl_shared_data )
{
  secboot_verified_info_type *sbl_auth_verified_info;
  uint32 entry_point;
  
  BL_VERIFY((bl_shared_data != NULL), BL_ERR_NULL_PTR_PASSED );
  
  /* Check if the image is loaded
    * Note: both APPSBL and APPS images share this function
    * so we have to check if any of them is loaded
    */
  entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_APPSBL_SW_TYPE);

  if (!entry_point)
  entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_APPS_HASH_TABLE_TYPE);
  
  /* If APPS image is loaded then update the value of the APPS over-ride */
  if (entry_point)
  {
    sbl_auth_verified_info = 
      (secboot_verified_info_type *) boot_get_sbl_auth_verified_info
      (bl_shared_data);
	  
    BL_VERIFY((sbl_auth_verified_info != NULL), BL_ERR_NULL_PTR_PASSED );

    if(SECBOOT_DEBUG_ENABLE == sbl_auth_verified_info->enable_debug)
    {
      /* Corresponds to Sparrow fuses */
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_APPS_NIDEN_DISABLE));
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_APPS_DBGEN_DISABLE));
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_DAP_DEVICEEN_DISABLE));
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_DAP_NIDEN_DISABLE));
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_DAP_DBGEN_DISABLE));

      /* Corresponds to OEM Spare1 and Spare0 fuses */
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_SPARE1_DISABLE));
      override2_val |= 1 << (HWIO_SHFT(OVERRIDE_2, OVRID_SPARE0_DISABLE));
    }
  }
}

/*===========================================================================

**  Function :  sbl1_hw_jtag_enable_modem

** ==========================================================================
*/
/*!
* 
* @brief
*   Check the certificate to see if JTAG over-ride register needs to be 
*   programmed to enable access via JTAG.
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
void sbl1_hw_jtag_enable_modem( bl_shared_data_type *bl_shared_data )
{
  secboot_verified_info_type *sbl_auth_verified_info;
  uint32 entry_point;
  
  BL_VERIFY((bl_shared_data != NULL), BL_ERR_NULL_PTR_PASSED );
  
  entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_MBA_SW_TYPE);
  
  /* If MBA image is loaded then update the value of the MPSS over-ride */
  if (entry_point)
  {
    sbl_auth_verified_info = 
      (secboot_verified_info_type *) boot_get_sbl_auth_verified_info
      (bl_shared_data);
	  
    BL_VERIFY((sbl_auth_verified_info != NULL), BL_ERR_NULL_PTR_PASSED );

    if(SECBOOT_DEBUG_ENABLE == sbl_auth_verified_info->enable_debug)
    {
      override4_val = 1 << (HWIO_SHFT(OVERRIDE_4,
                                 OVRID_MSS_DBGEN_DISABLE));
      override4_val |= 1 << (HWIO_SHFT(OVERRIDE_4,
                                 OVRID_MSS_NIDEN_DISABLE));                                 
    }
  }

  /* Write the override value to register */    
  HWIO_OUT(OVERRIDE_4, override4_val);

}

/*===========================================================================

**  Function :  sbl1_release_modem_proc

** ==========================================================================
*/
/*!
* 
* @brief
*  This function brings modem proc out of reset
* 
* @param[in] 
*  bl_shared_data Pointer to shared data
*
* @par Dependencies
*  It is assumed that MBA image is loaded before invoking this api.
* 
* @retval
*  None
* 
* @par Side Effects
*  None
* 
*/
void sbl1_release_modem_proc( bl_shared_data_type *bl_shared_data )
{
  uint32 entry_point;
  
  BL_VERIFY((bl_shared_data != NULL), BL_ERR_NULL_PTR_PASSED );
  
  entry_point = boot_sbl_qsee_interface_get_entry_point(&(bl_shared_data->sbl_qsee_interface),
                                            SECBOOT_MBA_SW_TYPE);

  /* Bring Q6 out of reset only if MBA image was loaded */
  if (entry_point)
  {
    boot_log_message("Modem proc reset, Start");
    boot_log_start_timer();
  
    /* Setup Clocks for the Q6 processor */
    boot_clock_setup_processor(CLOCK_PROCESSOR_MODEM); 
  
    /* Communicate MBA image start address to modem pbl via reg RMB_MBA_IMAGE */
    boot_rmb_set_mba_image_addr(MBA_RAM_BASE);
    
    /* Set modem pbl start address*/
    HWIO_OUT(MSS_QDSP6SS_RST_EVB, (uint32)MODEM_PBL_START_ADDR >> 4);
       
    /*Flush cache before bringing subsystem out of reset */  
    mmu_flush_cache();     

    /* Bring Q6 proc out of reset. Modem pbl executes and then jumps to MBA
       image */
    boot_clock_enable_processor(CLOCK_PROCESSOR_MODEM);
    
    /*Set Flag to indicate MBA loaded and q6 is out of reset */
    mpss_out_of_reset = TRUE;
  
    boot_log_stop_timer("Modem proc reset, Delta");
  }
}

/*===========================================================================

**  Function :  sbl1_check_modem_status

** ==========================================================================
*/
/*!
* 
* @brief
*  This function Checks if modem PBL and MBA images are able to boot 
*  successfuly.
* 
* @param[in] 
*  None
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
void sbl1_check_modem_status( void )
{
  if(mpss_out_of_reset == TRUE)
  {
    boot_log_message("Check Modem boot Status, Start");
    boot_log_start_timer();
  
    /* The RMB registers can only be accessed if mba is loaded and mpss is
      out of reset.
      Check if modem PBL was able to jumb to MBA successfully via rmb regiter 01,
      ie RMB_PBL_STATUS.
      Also Check if MBA image executes successfully via rmb regiter 03,
      ie RMB_MBA_STATUS.
    */
    boot_rmb_check_subsys_status();
  
    boot_log_stop_timer("Check Modem boot Status, Delta");
  }
}


/*===========================================================================

**  Function :  sbl1_hw_get_boot_option

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads the fast boot option register. 
*
* @par Dependencies
*   None
* 
* @retval
*   boot_select_option_enum_type as read from the register
* 
* @par Side Effects
*   None
* 
*/
boot_select_option_enum_type sbl1_hw_get_boot_option( void )
{
  uint32 fast_boot_option = 0;
  
  /* FAST_BOOT register bits takes into account the fuse values, debug
     re-enable settings and GPIO[48..54] input captured during reset
     take lower 3 bits for fast boot option.*/
  fast_boot_option = HWIO_INM( BOOT_CONFIG,
                               HWIO_FMSK( BOOT_CONFIG, FAST_BOOT ) 
                              );
  fast_boot_option >>= HWIO_SHFT( BOOT_CONFIG, FAST_BOOT );
  
  return (boot_select_option_enum_type)(fast_boot_option & 7);
  
} /* sbl1_hw_get_boot_option() */

/*===========================================================================

**  Function :  sbl1_hw_get_usb_speed

** ==========================================================================
*/
/*!
* 
* @brief
*   This function reads the usb speed option register. 
*
* @par Dependencies
*   None
* 
* @retval
*   qusb_max_speed_required_t as read from the register
* 
* @par Side Effects
*   None
* 
*/
qusb_max_speed_required_t sbl1_hw_get_usb_speed(void)
{
  qusb_max_speed_required_t usb_speed = (qusb_max_speed_required_t)HWIO_INF( BOOT_CONFIG, USB_SS_ENABLE );

  if (usb_speed == QUSB_MAX_SPEED_SUPER)
  {
    boot_log_message("Super speed USB mode");
  }
  else
  {
    boot_log_message("High speed USB mode");
  }
  	
  return usb_speed;
} /* sbl1_hw_get_usb_speed() */


/*===========================================================================
**  Function :  sbl1_post_ddr_hw_init
** ==========================================================================
*/
/*!
* 
* @brief
*   This function will initialize hw /modules who have a dependency on 
*   DDR being initialized.
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
void sbl1_post_ddr_hw_init(bl_shared_data_type *bl_shared_data)
{
  /*Ensure bam_init gets called to reset the crypto bam hw*/
  BL_VERIFY((CEML_ERROR_SUCCESS == boot_CeMLInit()),BL_ERR_SBL);    

}
