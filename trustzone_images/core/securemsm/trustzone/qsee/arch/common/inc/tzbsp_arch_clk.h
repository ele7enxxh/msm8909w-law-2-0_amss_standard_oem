#ifndef TZBSP_ARCH_CLK_H 
#define TZBSP_ARCH_CLK_H
/**
@file tzbsp_cp.h
@brief Header file for Content Protection related functions.
*/
/*===========================================================================
   Copyright (c) 2012-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/arch/common/inc/tzbsp_arch_clk.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
10/08/2015 prathi   using clockdriver API for clock enabled/disable

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
 
 #include <comdef.h>
 
 /* -----------------------------------------------------------------------
 * Typedefs
 * ----------------------------------------------------------------------- */
 /* Different Clock vote which are supported by the CLOCKdriver for TZ*/
typedef enum 
{
   TZ_CLK_CRYPTO            = 0x0,
   TZ_CLK_CRYPTO_AHB        = 0x1,
   TZ_CLK_CRYPTO_AXI        = 0x2,
   TZ_CLK_BLSP1_AHB         = 0x3,
   TZ_CLK_BLSP2_AHB         = 0x4,
   TZ_CLK_PRNG              = 0x5,
   TZ_CLK_MSG_RAM_AHB       = 0x6,
   TZ_CLK_APCS_AHB          = 0x7,
   TZ_CLK_MPM_AHB           = 0x8,
   /* NOTE: increase the value of MAX if adding more clk support */
   TZ_CLK_MAX               = 0x9,
} tzbsp_clk_id_t;
   
typedef struct
{
  tzbsp_clk_id_t  tz_clk_id;
  char            *clk_name;
} tzbsp_clk_data_t;
 
   
typedef enum
{
  CLK_HALT_OFF = 0,
  CLK_HALT_ON
} HAL_clk_ClockHaltMode;
  
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
 * Enables/disables different CLOCK vote based on the input paramater.
 * input: 
 * enabled     0 to disable/ 1 to enable
 * vote        clock vote which need to enable/disable
 *
 * return NULL;
 */
void tzbsp_clk_vote_set_enabled(boolean enabled, tzbsp_clk_vote_et vote);

/**
 * Enables/disables QDSS clock.
 */
void tzbsp_enable_qdss_clocks();

/**
 * Enables/disables some XPU clock to read the data needed for XPU voilated register.
 */
void tzbsp_enable_xpu_isr_clocks(boolean enable, uint32 xpu_err_bitmask);

/**
 * Enables/disables VMID clock, needed for the VMID configuration.
 */
void tzbsp_enable_vmid_clocks(boolean enable);

/**
 * Enables/disables XPU clock, needed for the XPU static configuration.
 */
void tzbsp_enable_xpu_config_clocks_boot(boolean enable);

/** Clock to make system out od reset.
  Before clocks are accessible, subsystems must be taken out of reset **/
void tzbsp_subsystem_clk_preamble(void);

/**
 * Enables/disables SMMU vote, needed for the SMMU configuration.
 */
void tzbsp_enable_smmu_clocks(boolean enable);

/**
 * Enables/disables xpu vote, needed for the XPU configuration.
 */
void tzbsp_vote_xpu_clocks(boolean enable);

/**
 * Enables/disables bam vote.
 */
void tzbsp_enable_bam_clocks(boolean enable);

/**
 * Enables/disables all crypto clock related TZ-branch register.
 */
void tzbsp_enable_ce1_clocks(boolean enable);

/**
 * Enables/disables APCS HCLK vote.
 */
void tzbsp_hclk_vote_set_enabled(boolean val);


/**
 * To set the bandwidth of the crypto clock is HLOS is not controlling the Clock 
 */
uint32 tzbsp_set_bandwidth(void *reqClient, uint32 reqClientlen,
                           uint32 res_req, uint32 level, 
                           uint32 flags);
                           
                           
#endif
