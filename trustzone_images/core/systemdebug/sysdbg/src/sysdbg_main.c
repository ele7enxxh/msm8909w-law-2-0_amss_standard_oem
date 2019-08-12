/*=============================================================================
                        SDI Main Init Function

GENERAL DESCRIPTION     
		  This module handles the main entry point of the debug functionalities within TZ.
  
	Copyright 2012- 2014 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

01/13/14    davidai     Ported for Bear
07/02/13    AJCheriyan  Updated for Gandalf
01/28/13    AJCheriyan  Added BOOT_PARTITION_SELECT write for v2
01/15/13    AJCheriyan  Moved reset apps core and debug feature enablement here
02/22/12    AJCheriyan  First cut for Aragorn. First ever SYSDBG
===========================================================================*/
#include "sysdbg_target.h"
#include "sysdbg_mem_dump.h"
#include "msmhwio.h"
#include "comdef.h"
#include "tzbsp_sys.h"
#include "sysdbg_hwio.h"
#include "sysdbg_def.h"
#include "tt_sd.h"
#include "tzbsp_ext_os.h"


extern sysdbg_pass_data_type *sysdbg_pass;
extern sysdbg_pass_data_type sysdbg_pass_data;
extern boolean tzbsp_allow_unlock_xpu(void);
#ifdef TZBSP_WITH_TBASE
void sysdbg_enable_flat_mapped(void);
boolean debug_reset_boot_flat_mapped(void);
#endif

/* Create stack of 1KB for system debug as we cant use TZ stack */
uint32 sysdbg_stack[256] = {0};
/*===========================================================================
**  Function :  SYSTEM_DEBUG_MAIN
** ==========================================================================
*/
/*!
* 
* @brief
* 	Main SYSDBG
* 	
* 
* @param[in] 
*       None
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
*
*/


void system_debug_main(void)
{
	if(tzbsp_allow_unlock_xpu()) //Only allow sysdbg execution when debug enabled
	{
	sysdbg_pass=&sysdbg_pass_data;
	sysdbg_execute_pass(sysdbg_pass);
	}
}

/*===========================================================================
**  Function :  SYSDBG_ENABLE
** ==========================================================================
*/
/*!
* 
* @brief
* The function that enables the debug through watchdog feature
* 
* @param[in] 
*   None
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
boolean sysdbg_enable(void)
{
#ifdef TZBSP_WITH_TBASE
	if (tzbsp_sysdbg_flat_mapped())
	{
		sysdbg_enable_flat_mapped();
		return TRUE;
	}
#endif
	// We need to enable PROC_HALT_EN so that MSM pulls up PS_HOLD after reset
	//HWIO_OUTF(GCC_RESET_DEBUG, PROC_HALT_EN, 0x1);

	// Set the debug timer after which controller can reset the system
	// 256 sclk cycles = ~ 7.8ms
	HWIO_OUTF(GCC_RESET_DEBUG, PRE_ARES_DEBUG_TIMER_VAL, 0x100);	

	// Enable / Disable the debug through watchdog feature
	HWIO_OUTF(GCC_RESET_DEBUG, SECURE_WDOG_DEBUG_EN, 0x1);
	HWIO_OUTF(GCC_RESET_DEBUG, MSM_TSENSE_RESET_DEBUG_EN, 0x1);
	HWIO_OUTF(GCC_RESET_DEBUG, PMIC_ABNORMAL_RESET_DEBUG_EN, 0x1);
	
	HWIO_OUTF(GCC_QDSS_DAP_AHB_CBCR, CLK_ENABLE, 0x1);
#ifdef SYSDBG_TARGET_MDM9x07
	/*In lykan, qdss_dap_clk and the qdss_cfg_ahb_clk are on votable branch enable logic*/
	HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, QDSS_CFG_AHB_CLK_ENA, 1);
	HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, QDSS_DAP_CLK_ENA, 1);
	/*Enable this so that debugUI can access the AHB data.*/
	HWIO_OUTF(TCSR_GCC_CLK_MUX_SEL, CLK_MUX_SEL, 0x1);
#else
	HWIO_OUTF(GCC_QDSS_CFG_AHB_CBCR,CLK_ENABLE, 0x1);
	HWIO_OUTF(GCC_QDSS_DAP_CBCR, CLK_ENABLE, 0x1);
#endif

	HWIO_OUT(QDSS_WRAPPER_CS_DEBUG_UI_LOCKACCESS, 0xC5ACCE55);
	HWIO_OUTF(QDSS_WRAPPER_DEBUG_UI_SECURE, SEC_CTL, 0x1);
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL, 0x1);   	   // Clear all status
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL_MASK, 0x0);     // Clear SW_TRIG_MASK
	//Set STOP_CAPTURE_DEBUG_TIMER to 2ms
	HWIO_OUT(GCC_STOP_CAPTURE_DEBUG_TIMER, 0x40);
	// Set the ETR timer to flush data out
	HWIO_OUT(GCC_FLUSH_ETR_DEBUG_TIMER, 0x180);
	// Flag to enable Debug
	HWIO_OUTM(TCSR_RESET_DEBUG_SW_ENTRY, 0x1, 0X1);
	return TRUE;
}

/*===========================================================================
**  Function :  DEBUG_RESET_BOOT
** ==========================================================================
*/
/*!
* 
* @brief
* The function checks for reset status
* 
* @param[in] 
*   None
*
* @par Dependencies
*   None
* 
* @retval
*   TRUE indicates reset due to crash; FALSE indicates cold boot
* 
* @par Side Effects
*   None
* 
*/
boolean debug_reset_boot(void)
{
#ifdef TZBSP_WITH_TBASE
	if (tzbsp_sysdbg_flat_mapped())
	{
		return debug_reset_boot_flat_mapped();
	}
#endif

    if ((HWIO_IN(GCC_RESET_STATUS)==0) &&
    (HWIO_INF(GCC_RESET_DEBUG, SECURE_WDOG_DEBUG_EN) == 0) &&
    (HWIO_INF(GCC_RESET_DEBUG, MSM_TSENSE_RESET_DEBUG_EN) == 0) &&
    (HWIO_INF(GCC_RESET_DEBUG, PMIC_ABNORMAL_RESET_DEBUG_EN)== 0))
        return 0;
    return 1;
}

#ifdef TZBSP_WITH_TBASE
/* Use the C pre-processor to re-define the below macros based on
 * physical addresses. Note that the new defines are valid until
 * the end of the current translation unit */
#undef  GCC_CLK_CTL_REG_REG_BASE
#define GCC_CLK_CTL_REG_REG_BASE	   GCC_CLK_CTL_REG_REG_BASE_PHYS
#undef  QDSS_WRAPPER_DEBUG_UI_REG_BASE
#define QDSS_WRAPPER_DEBUG_UI_REG_BASE QDSS_WRAPPER_DEBUG_UI_REG_BASE_PHYS
#undef  TCSR_TCSR_REGS_REG_BASE
#define TCSR_TCSR_REGS_REG_BASE 	   TCSR_TCSR_REGS_REG_BASE_PHYS

void sysdbg_enable_flat_mapped(void)
{
	// We need to enable PROC_HALT_EN so that MSM pulls up PS_HOLD after reset
	HWIO_OUTF(GCC_RESET_DEBUG, PROC_HALT_EN, 0x1);

	// Set the debug timer after which controller can reset the system
	// 256 sclk cycles = ~ 7.8ms
	HWIO_OUTF(GCC_RESET_DEBUG, PRE_ARES_DEBUG_TIMER_VAL, 0x100);	
	// Enable / Disable the debug through watchdog feature
	HWIO_OUTF(GCC_RESET_DEBUG, SECURE_WDOG_DEBUG_EN, 0x1);
	HWIO_OUTF(GCC_RESET_DEBUG, MSM_TSENSE_RESET_DEBUG_EN, 0x1);
	HWIO_OUTF(GCC_RESET_DEBUG, PMIC_ABNORMAL_RESET_DEBUG_EN, 0x1);
	
	HWIO_OUTF(GCC_QDSS_DAP_AHB_CBCR, CLK_ENABLE, 0x1);
#ifdef SYSDBG_TARGET_MDM9x07
	HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, QDSS_CFG_AHB_CLK_ENA, 1);
	HWIO_OUTF(GCC_APCS_CLOCK_BRANCH_ENA_VOTE, QDSS_DAP_CLK_ENA, 1);
	/*Enable this so that debugUI can access the AHB data.*/
	HWIO_OUTF(TCSR_GCC_CLK_MUX_SEL, CLK_MUX_SEL, 0x1);
#else
	HWIO_OUTF(GCC_QDSS_CFG_AHB_CBCR,CLK_ENABLE, 0x1);
	HWIO_OUTF(GCC_QDSS_DAP_CBCR, CLK_ENABLE, 0x1);
#endif

	HWIO_OUT(QDSS_WRAPPER_CS_DEBUG_UI_LOCKACCESS, 0xC5ACCE55);
	HWIO_OUTF(QDSS_WRAPPER_DEBUG_UI_SECURE, SEC_CTL, 0x1);
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL, 0x1);   	   // Clear all status
	HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL_MASK, 0x0);     // Clear SW_TRIG_MASK

	//Set STOP_CAPTURE_DEBUG_TIMER to 2ms
	HWIO_OUT(GCC_STOP_CAPTURE_DEBUG_TIMER, 0x40);
	// Set the ETR timer to flush data out
	HWIO_OUT(GCC_FLUSH_ETR_DEBUG_TIMER, 0x180);
	// Flag to enable Debug
	HWIO_OUTM(TCSR_RESET_DEBUG_SW_ENTRY, 0x1, 0X1);
}

boolean debug_reset_boot_flat_mapped(void)
{
	if ((HWIO_IN (GCC_RESET_STATUS) 							== 0) && 
		(HWIO_INF(GCC_RESET_DEBUG, SECURE_WDOG_DEBUG_EN)		== 0) &&
		(HWIO_INF(GCC_RESET_DEBUG, MSM_TSENSE_RESET_DEBUG_EN)   == 0) &&
		(HWIO_INF(GCC_RESET_DEBUG, PMIC_ABNORMAL_RESET_DEBUG_EN)== 0))
		return 0;
	return 1;
}
#endif
