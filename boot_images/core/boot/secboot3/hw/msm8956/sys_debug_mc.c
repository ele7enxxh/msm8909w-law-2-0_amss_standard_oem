/*=============================================================================
                        System Debug Main Control Function

GENERAL DESCRIPTION     
   This module defines objects for the main control loop 
   of the system debug image.
  
	Copyright 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who   what, where, why
--------   ---   --------------------------------------------------
07/22/14   lm    Aligned with V1.1 cpu dump table format
07/14/14   lm    Added changes to support for 8936
06/13/14   lm    Added a logic to find the cpu number instead of hard coding
				 the cpu number
06/07/14   lm    Enable QDSS clock by writing GCC reg
05/29/14   lm    Changed the core number due to performance cluster for 8936
05/23/14   ck    Added additional QDSS clocks and initialization
05/21/14   lm    Added flatfile change updates
03/13/14   ck    Added Debug UI clocks to sys_debug_enable_clocks
03/06/14   ck    Added sys_debug_enable_clocks function
02/26/14   ck    Added QDSS unlock logic
02/18/14   ck    Added call to boot_ddr_restore_from_wdog_reset
02/18/14   ck    Removed duplicate copy of 32 to 64 bit switch functions.
                 Single copy is stored in SDI code space and shared.
02/06/14   ck    Added sys_debug_switch_to_aarch64
01/02/14   ck    Initial release for Bear family 

===========================================================================*/
#include "sys_debug.h"
#include "boot_msm.h"
#include "msmhwioreg.h"
#include "boot_elf_header.h"
#include "boothw_target.h"
#include "boot_extern_ddr_interface.h"
#include "boot_target.h"


/* SYSTEM_DEBUG_QSEE_INFO is memory specified in the scatter file for SBL
   and system debug to share information about where QSEE was loaded and
   if it's a 32 bit or 64 bit image. */
extern uint32 Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;
extern uint32 get_cur_cluster_num(void);

/*===========================================================================
**  Function :  sys_debug_enable_clocks
** ==========================================================================
*/
/*!
* 
* @brief
*   During abnormal reset some clocks are turned off that need to be reenabled
*   for proper operation.  Since full clock driver cannot be placed into
*   RPM Code RAM this function will enable the minimum set of clocks needed.
*
* @param[in] 
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
* 
*/
void sys_debug_enable_clocks(void)
{
  /* Needed by Debug UI */
  HWIO_GCC_QDSS_DAP_AHB_CBCR_OUTM(HWIO_GCC_QDSS_DAP_AHB_CBCR_CLK_ENABLE_BMSK,
                                  1);

  HWIO_GCC_QDSS_CFG_AHB_CBCR_OUTM(HWIO_GCC_QDSS_CFG_AHB_CBCR_CLK_ENABLE_BMSK,
                                  1);

  HWIO_GCC_QDSS_DAP_CBCR_OUTM(HWIO_GCC_QDSS_DAP_CBCR_CLK_ENABLE_BMSK,
                              1);


  /* Needed by DDR */
  HWIO_GCC_PCNOC_DDR_CFG_CBCR_OUT(HWIO_GCC_PCNOC_DDR_CFG_CBCR_CLK_ENABLE_BMSK);

  HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_OUT(HWIO_GCC_BIMC_DDR_XO_CMD_RCGR_ROOT_EN_BMSK);

  HWIO_GCC_BIMC_XO_CBCR_OUT(HWIO_GCC_BIMC_XO_CBCR_CLK_ENABLE_BMSK);

  HWIO_GCC_BIMC_CFG_AHB_CBCR_OUT(HWIO_GCC_BIMC_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK |
                                 HWIO_GCC_BIMC_CFG_AHB_CBCR_CLK_ENABLE_BMSK);

  HWIO_GCC_DDR_DIM_CFG_CBCR_OUT(HWIO_GCC_DDR_DIM_CFG_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK |
                                HWIO_GCC_DDR_DIM_CFG_CBCR_CLK_ENABLE_BMSK);

  HWIO_GCC_BIMC_SLEEP_CBCR_OUT(HWIO_GCC_BIMC_SLEEP_CBCR_CLK_ENABLE_BMSK);

  HWIO_GCC_DDR_DIM_SLEEP_CBCR_OUT(HWIO_GCC_DDR_DIM_SLEEP_CBCR_CLK_ENABLE_BMSK);

  HWIO_GCC_BIMC_DDR_CMD_RCGR_OUT(HWIO_GCC_BIMC_DDR_CMD_RCGR_UPDATE_BMSK);

  HWIO_GCC_BIMC_DDR_CPLL0_CBCR_OUT(HWIO_GCC_PCNOC_DDR_CFG_CBCR_CLK_ENABLE_BMSK);  
  HWIO_GCC_BIMC_DDR_CPLL1_CBCR_OUT(HWIO_GCC_PCNOC_DDR_CFG_CBCR_CLK_ENABLE_BMSK);
  HWIO_GCC_BIMC_DDR_CH0_CBCR_OUT(HWIO_GCC_PCNOC_DDR_CFG_CBCR_CLK_ENABLE_BMSK);
}


/*===========================================================================
**  Function :  sys_debug_main_ctl
** ==========================================================================
*/
/*!
* 
* @brief
* 
* @param[in] 
*  
* @par Dependencies
*   None
* 
* @retval
*   None
* 
* @par Side Effects
* 
*/
void sys_debug_main_ctl(sys_debug_cpu_ctxt_regs_type * ctxt_regs)
{
   
  sys_debug_qsee_info_type * qsee_info =
    (sys_debug_qsee_info_type *)&Image$$SYSTEM_DEBUG_QSEE_INFO$$Base;


  /* Enable clocks that were disabled during abnormal reset
     that are needed.  This must be done first. */
  sys_debug_enable_clocks();


  /* Unlock and initialize QDSS Coresight component so the PC and Reset Mode
     can be accessed.  No reason to verify if unlock was successful or not as
     code must continue. */
  HWIO_QDSS_WRAPPER_CS_DEBUG_UI_LOCKACCESS_OUT(DEBUG_UI_UNLOCK_CODE);

  HWIO_QDSS_WRAPPER_DEBUG_UI_SECURE_OUTM(
    HWIO_QDSS_WRAPPER_DEBUG_UI_SECURE_SEC_CTL_BMSK,
    1);

  HWIO_QDSS_WRAPPER_DEBUG_UI_CTL_OUT(
    HWIO_QDSS_WRAPPER_DEBUG_UI_CTL_CLEAR_CTL_BMSK);

  HWIO_QDSS_WRAPPER_DEBUG_UI_CTL_MASK_OUT(0);


  HWIO_APCS_BANKED_GCC_DBG_CLK_ON_REQ_OUT(1);
  
  
  /* Save Reset PC */
  /* For 8936 quadcore support,only cluster and cpu numbers have changed but 
     the register's remain same */
  ctxt_regs->pc = HWIO_QDSS_WRAPPER_DEBUG_UI_DATA_n_INI(4);
 

  /* Bring DDR out of self refresh */
  ddr_restore_from_wdog_reset(DDR_XO_SPEED_IN_KHZ);
  

  /* Jump to QSEE passing the address of the internal dump structure.
     The entry point and elf class have already been validated during cold
     boot.  Even if they were bad there is nowhere to go from here.
     Entry point must be 32 bits as the boot remapper is used.  Again this
     was already validated during cold boot.*/
  if (qsee_info->elf_class == ELF_CLASS_64)
  {
    boot_switch_to_aarch64((uint32)qsee_info->entry_point,
                           (uint32)ctxt_regs);
  }
  else
  {
    ((void (*)())(uint32)(qsee_info->entry_point))(ctxt_regs); 
  }

} /* sys_debug_main_ctl() */


void sys_debug_undefined_c_handler(void)
{
}


void sys_debug_swi_c_handler(void)
{
}


void sys_debug_prefetch_abort_c_handler(void)
{
}


void sys_debug_data_abort_c_handler(void)
{
}


void sys_debug_reserved_c_handler(void)
{
}


void sys_debug_irq_c_handler(void)
{
}


void sys_debug_fiq_c_handler(void)
{
}
