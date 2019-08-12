/**
 * @file ddr_target.c
 * @brief
 * Target specific DDR drivers.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/hw/msm8952/ddr_target.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
11/21/14   sk      Changes for creating two elfs for RUMI and ASIC
11/10/14   sk      Added a check for checksum over training data
11/06/14   sk      Disable one time training by return TRUE from ddr_params_is_training_on_rpm_required function
10/09/14   aus     Modify the DDR frequencies
08/26/14   rj      Disabled one time training
08/22/14   sk      KlockWork FIX:Checking NULL for return value of ddr_get_trained_params
07/24/14   sk      Added ddr_training_partition condition check at ddr_params_is_training_required function
05/21/14   aus     Updated to support shared DDR library
03/27/14   rp	   Added a voltage checksum for railway and rbcpr hash
10/25/13   tw      Added support for LPAE
09/09/13   sr      Removed unused extern variables .
09/06/13   sr      Skeleton Code for PHY training in SBL1
09/05/13   sr      initial changes to support training in boot.
08/27/13   sr      is_training_required moved from params.c to target.c
06/21/13   tw      Initial Port for 8084
05/13/13   sl      Updated delta table calls.
03/01/13   tw      Added calling delta table
11/02/12   tw      re-enabled 800mhz, added site initialization
11/02/12   tw      enabled flag for training required
11/02/12   sy      Back out 800Mhz
09/21/12   sl      Combined LPDDR device parameters.
09/17/12   sl      Updated DPE clock switch function calls.
09/12/12   sl      Use macro-style DDR logging.
09/07/12   sl      Moved ddr_get_partition() to ddr_drivers.c.
09/06/12   sl      Added parameter tweaking functions.
08/17/12   sl      Removed ddr_register_fill().
08/08/12   dh      change CORE_CLK_PERIOD to 50 for bring up
07/24/12   tw      Removed bus configuration from one time settings
11/30/11   tw      Initial version.
================================================================================
                   Copyright 2011-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_internal.h"
#include "ddr_sync.h"
#include "ddr_log.h"
#include "ddr_params.h"
#include "ddr_hwio.h"
#include "HALhwio.h"
#include "ddr_target.h"
#include "HAL_SEQ_DDR.h"
#include "BIMC.h"
#include "ddr_config.h"
#include "boot_extern_clk_interface.h"
#include "icbcfg.h"
#include "ddrss_rcw.h"
#include "bimc_scale.h"
#include "msmhwiobase.h"
#include "msmhwioreg.h"
#if 0
#include "railway.h"
#include "rbcpr_api.h"
#endif
#include "ddr_func_tbl.h"
#include "mpm_hwio.h"
#include "ddr_phy_ddrss.h"

/*==============================================================================
                                  MACROS
==============================================================================*/
#define ONE_TIME_TRAINING TRUE

/*==============================================================================
                                  DATA
==============================================================================*/
/* DDR logging level */
enum ddr_log_level ddr_log_level = DDR_ERROR;

extern ddr_func ddr_func_tbl;

extern uint32 ddr_bus_width;

/* DDR Partition information */
ddr_size_partition ddr_size_partition_info;

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/

/* =============================================================================
**  Function : ddr_target_init
** =============================================================================
*/
/**
*   @brief
*   Function called at the end of ddr_init() to initialize target specific
*   configurations.
*
*   @param  None
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
void ddr_target_init()
{
  struct ddr_shared_params *header;
  
  /* initialization of function tables for shared ddr library 
   * taking into account RPM view of shared library is different 
   */
  ddr_func_tbl.enter_self_refresh = (void (*)(SDRAM_INTERFACE, SDRAM_CHIPSELECT))
                                    ((uint8 *)&HAL_SDRAM_Enter_Self_Refresh - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.exit_self_refresh = (void (*)(SDRAM_INTERFACE, SDRAM_CHIPSELECT))
                                   ((uint8 *)&HAL_SDRAM_Exit_Self_Refresh - SCL_RPM_CODE_RAM_BASE);
  
  ddr_func_tbl.get_read_latency = (uint32 (*) (SDRAM_INTERFACE , uint32 )) 
                                  ((uint8 *)&HAL_SDRAM_RL - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.get_write_latency = (uint32 (*) (SDRAM_INTERFACE , uint32 )) 
                                   ((uint8 *)&HAL_SDRAM_WL - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.pre_clock_switch = (void (*)(SDRAM_INTERFACE, uint32 )) 
                                   ((uint8 *)&HAL_SDRAM_Pre_Clock_Switch - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.post_clock_switch = (void (*)(SDRAM_INTERFACE, uint32 )) 
                                           ((uint8 *)&HAL_SDRAM_Post_Clock_Switch - SCL_RPM_CODE_RAM_BASE);

  ddr_func_tbl.ebi_phy_cfg_disable_csd_rx = (void (*)(uint32, uint32, int)) 
                                           ((uint8 *)&EBI1_PHY_CFG_disable_csd_rx - SCL_RPM_CODE_RAM_BASE);
  ddr_func_tbl.ebi_phy_cfg_enable_csd_rx = (void (*)(uint32, uint32, phy_clk_type)) 
                                           ((uint8 *)&EBI1_PHY_CFG_enable_csd_rx - SCL_RPM_CODE_RAM_BASE);

  /* clear out shared header and fill in with updated information */
  header = ddr_get_shared_header();
  memset(header, 0, sizeof(struct ddr_shared_params));
  
  header->ddr_func_tbl_ptr = (void *) ((uint8 *)&ddr_func_tbl - SCL_RPM_CODE_RAM_BASE);
  header->size = sizeof(ddr_func);

  ddr_bus_width = 32;
} /* ddr_target_init */

/* ============================================================================
**  Function : ddr_post_init
** ============================================================================
*/
/*!
*   @brief
*   This function is called after ddr is initialized. It will take care of any
*   post initialization activities such as ddr training.
*
*   @details
*   This function is called after ddr is initialized. It will take care of any
*   post initialization activities such as ddr training.
*
*   @param
*   boolean -
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
boolean ddr_post_init()
{
volatile uint32 * ddr_training_cookie = (uint32 *)SHARED_IMEM_DDR_TRAINING_COOKIE;

  if(*ddr_training_cookie == DDR_TRAINING_NOT_REQUIRED)
  {
    return FALSE;
  }
  else
  {
    return ((*ddr_training_cookie) == DDR_TRAINING_DATA_UPDATED);
  }
}

/* ============================================================================
**  Function : ddr_pre_clock_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right before clock switching occures.
*   The function will configure the ddr such that no data loss will occur
*
*   @details
*   DDR will be stalled and new timing parameter is loaded into shadow.
*   Depending on bumping up or stepping down clock speed, we will load the
*   shadow register value to actual registers before or after clock switch
*   has occurred.
*
*   @param curr_clk   -   [IN] the current clock speed
*   @param new_clk    -  [IN] the clock speed we are switching to
*   @param new_clk    -  [IN] interface to switch clock for
*
*   @par Dependencies
*
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void ddr_pre_clock_switch(uint32 curr_clk, uint32 new_clk, SDRAM_INTERFACE interface_name)
{
    HAL_SDRAM_Pre_Clock_Switch(interface_name,new_clk);
} /* ddr_pre_clock_switch */

/* ============================================================================
**  Function : ddr_post_clock_switch
** ============================================================================
*/
/*!
*   @brief
*   This function is called right after clock switching occurs.
*   The function will configure the ddr such that no data loss will occur
*
*   @details
*   DDR will be unstalled.
*   Depending on bumping up or stepping down clock speed, we will load the
*   shadow register value to actual registers before or after clock switch
*   has occurred.
*
*   @param curr_clk          -  [IN] the current clock speed
*   @param new_clk           -  [IN] the clock speed we are switching to
*   @param interface_name    -  [IN] interface to switch clock for
*
*   @par Dependencies
*   This code has to be on IRAM because ddr is unavailable during clock switching
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/

void ddr_post_clock_switch(uint32 curr_clk, uint32 new_clk, SDRAM_INTERFACE interface_name)
{
    HAL_SDRAM_Post_Clock_Switch(interface_name,new_clk);
} /* ddr_post_clock_switch */


/* =============================================================================
**  Function : ddr_params_is_training_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. LPDDR3 will always require training syncronization
*   to be done between rpm and sbl
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_params_is_training_required( void )
{
  struct ddr_params_partition * ddr_training_partition; 
  uint32 size;
  
  ddr_training_partition = ddr_params_get_training_data(&size);
  
  if(ddr_training_partition == NULL)
	  return TRUE;
  
  if(ddr_training_partition->version != DDR_TRAINING_PARAM_VERSION)
  {
    return TRUE;
    /* parameter version not matched, force retrain */
  }
  
  return FALSE;
} /* ddr_params_is_training_required */

/* =============================================================================
**  Function : ddr_params_is_training_on_rpm_required
** =============================================================================
*/
/**
*   @brief
*   Indicate whether DDR parameter training is required or not in RPM. Training is
*   required if and only if DDR itself (e.g. PCDDR3) requires parameter training
*   and DDR parameter partition is invalid. 
*
*   @param  None
*
*   @retval  TRUE   Training required
*   @retval  FALSE  Training not required
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
boolean ddr_params_is_training_on_rpm_required( void )
{
  return TRUE;
} /* ddr_params_is_training_on_rpm_required */

/* =============================================================================
**  Function : ddr_pre_setup_for_training
** =============================================================================
*/
/**
*   @brief
*   Disable SCMO ranks, SHKE house-keeping , DPE CSPD & enable SCMO interrupts .
*
*   @param  None
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
void ddr_pre_setup_for_training(struct ddr_device_params_common *ddr_params_ch0,
		                   struct ddr_device_params_common *ddr_params_ch1)
{
  SDRAM_CHIPSELECT chip_select_ch0, chip_select_ch1;

  chip_select_ch0 = SDRAM_CS_NONE;
  if (ddr_params_ch0->num_banks_cs0 != 0)
  {
    chip_select_ch0 |= SDRAM_CS0;
  }
  if (ddr_params_ch0->num_banks_cs1 != 0)
  {
    chip_select_ch0 |= SDRAM_CS1;
  }

  chip_select_ch1 = SDRAM_CS_NONE;
  if (ddr_params_ch1->num_banks_cs0 != 0)
  {
    chip_select_ch1 |= SDRAM_CS0;
  }
  if (ddr_params_ch1->num_banks_cs1 != 0)
  {
    chip_select_ch1 |= SDRAM_CS1;
  }

  if (chip_select_ch0 != SDRAM_CS_NONE)
  {
    /* Disable SHKE house-keeping */
   HAL_SDRAM_SHKE_Disable_Auto_Refresh(0, SDRAM_INTERFACE_0, chip_select_ch0);
   HAL_SDRAM_SHKE_Disable_Periodic_SRR(0, SDRAM_INTERFACE_0, chip_select_ch0);
   HAL_SDRAM_SHKE_Disable_ZQCAL(0, SDRAM_INTERFACE_0, chip_select_ch0);
   HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(0, SDRAM_INTERFACE_0, chip_select_ch0);

  }
  if (chip_select_ch1 != SDRAM_CS_NONE)
  {
    /* Disable SHKE house-keeping */
   HAL_SDRAM_SHKE_Disable_Auto_Refresh(0, SDRAM_INTERFACE_1, chip_select_ch1);
   HAL_SDRAM_SHKE_Disable_Periodic_SRR(0, SDRAM_INTERFACE_1, chip_select_ch1);
   HAL_SDRAM_SHKE_Disable_ZQCAL(0, SDRAM_INTERFACE_1, chip_select_ch1);
   HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(0, SDRAM_INTERFACE_1, chip_select_ch1);

  }
}


/* ============================================================================
**  Function : ddr_phy_training
** ============================================================================
*/
/*!
*   @brief
*   This function will train the ddr phy at the highest frequency supported.
*
*   @details
*   This function will train the ddr phy at the highest frequency supported.
*   the rest of frequencies will be interpolated.
*
*   @param interface_name     -  [IN] the interface to train for
*   @param chip_select        -  [IN] the chip select to train for
*   @param training_params    -  [IN/OUT] pointer to training parameter to
*                                         populate trained results.
*
*   @par Dependencies
*
*
*   @par Side Effects
*   None
*
*   @retval  None
*
*   @sa None
*/
void ddr_phy_training(SDRAM_INTERFACE interface_name,
    SDRAM_CHIPSELECT chip_select,
    union ddr_trained_params *training_params,
    uint32 nNumLevel,
    ClockPlanType *pClockPlan)
{
  
  uint16 railway_checksum_vddcx = 0;
  uint16 railway_checksum_vddmx = 0;
  uint16 rbcpr_checksum = 0;
  
  #if 0
  int rail_id_vddcx = rail_id("vddcx");
  int rail_id_vddmx = rail_id("vddmx");  
  uint16 railway_checksum_vddcx = railway_setting_checksum (rail_id_vddcx);
  uint16 railway_checksum_vddmx = railway_setting_checksum (rail_id_vddmx);
  uint16 rbcpr_checksum = rbcpr_cx_mx_settings_checksum();
  uint32 clk_plan[7] = {0};  
  uint32 pc_fmax = 931200;  
  #endif 
  
  uint32 bimc_base = 0x00400000;
  struct ddr_device_params_common *ddr_params_ch0;
  int32 i = nNumLevel-2;
  uint32 lp_fmax = 398400;
 uint32 lp_ddr_fmax = 796800;
// uint16 ca_training_full_cdc[2][2][4];
 //uint32 ca_training_perbit[2][2][2];

  while(i>=0)
  {
#if 0  
     clk_plan[i] = pClockPlan[i+1].nFreqKHz;
#endif
     i--;
  }
  
  ddr_params_ch0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);

  if(ddr_params_ch0->device_type == DDR_TYPE_LPDDR3) {
    ddr_pre_clock_switch(0, lp_ddr_fmax, SDRAM_INTERFACE_0);
         
    boot_clock_set_bimcspeed(lp_fmax /*pClockPlan[nNumLevel-2].nFreqKHz*/);
	ddr_post_clock_switch(0, lp_ddr_fmax, SDRAM_INTERFACE_0);  
    BIMC_Disable_All_Periodic(bimc_base, interface_name, chip_select);
       
	DDRSS_dcc_cal( 0,interface_name, training_params->lpddr3_training.ca_dcc_training, training_params->lpddr3_training.dq_dcc_training);

	DDRSS_ddr_phy_ca_training( 0, /* sequence is expecting driver to send zero for phy_base, they calculate the right offsets */
		             interface_name,
			     chip_select,
			     1,                                   /*fine_delay_training*/
			     0,                                   /*per_bit_deskew_training*/
			     training_params->lpddr3_training.phy_cdc_params.ca_training_full_cdc,
			     training_params->lpddr3_training.phy_cdc_params.ca_training_perbit,
			     2,                                   /*max_loop_count*/
			     0,                                   /*coarse_init_val*/
			     0,			                  /*fine_init_val*/
			     0,
			     lp_ddr_fmax/*pClockPlan[nNumLevel-2].nFreqKHz*/ /*0xD*/);                                  /*per_bit_deskew_init_val*/
				 
  }

     if(ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){
       DDRSS_ddr_phy_dq_rd_training_ddrss( 0,
                      interface_name,
                      chip_select,
                      1,/* fine = 1, coarse = 0*/
                      0,
                      2,
                      0,
                      0,
                      0,
                      training_params->lpddr3_training.phy_cdc_params.dq_read_training,
                      training_params->lpddr3_training.phy_cdc_params.dq_read_perbit_training,
                      training_params->lpddr3_training.phy_cdc_params.dq_read_perbit_dm_training, 
		      lp_ddr_fmax/*pClockPlan[nNumLevel-2].nFreqKHz*/ );
     }

     if(ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){

          DDRSS_ddr_phy_dq_wr_training_ddrss( 0,
                      interface_name,
		      chip_select,
		      1,/* fine = 1, coarse = 0*/
		      0,
		      2,
		      0,
		      0,
		      0,
		      training_params->lpddr3_training.phy_cdc_params.dq_write_training,
	              training_params->lpddr3_training.phy_cdc_params.dq_write_perbit_training,
		      training_params->lpddr3_training.phy_cdc_params.dq_write_perbit_dm_training, 
		      lp_ddr_fmax /*pClockPlan[nNumLevel-2].nFreqKHz*/ );

     }
	 
	 if(ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){
		training_params->lpddr3_training.voltage_checksum = railway_checksum_vddcx ^ railway_checksum_vddmx ^ rbcpr_checksum;
	 }

     BIMC_Enable_All_Periodic(bimc_base, interface_name, chip_select, 0);
     DDRPHY_CA_auto_io_cal(SEQ_DDRSS_EBI1_PHY_CH0_CA_DDRPHY_CA_OFFSET);
}

/* ============================================================================
**  Function : ddr_training_is_required
** ============================================================================
*/
/*!
*   @brief
*   This function will parse the magic cookie and clock plan to determine
*   if the runtime configuration has changed since the last time we trained
*   and if it has return TRUE for training required.
*   Otherwise return FALSE for no training required.
*
*   @details
*   This function will parse the magic cookie and clock plan to determine
*   if the runtime configuration has changed since the last time we trained
*   and if it has return TRUE for training required.
*   Otherwise return FALSE for no training required.
*
*   @param
*   None
*
*   @par Dependencies
*
*   @par Side Effects
*   None
*
*   @retval  boolean - Training required/Not required
*
*   @sa None
*/
boolean ddr_training_is_required(void)
{
  uint32 * ddr_shared_imem_ddr_training_cookie = (uint32 *)SHARED_IMEM_DDR_TRAINING_COOKIE;
  union ddr_trained_params *training_params;
  struct ddr_params_partition * ddr_training_partition;
  struct ddr_device_params_common *ddr_params_ch0;
  uint32 size;
  uint32 serial_number;
  
  uint16 railway_checksum_vddcx = 0;
  uint16 railway_checksum_vddmx = 0;
  uint16 rbcpr_checksum = 0;  
#if 0  
  int rail_id_vddcx = rail_id("vddcx");
  int rail_id_vddmx = rail_id("vddmx");
  uint16 railway_checksum_vddcx = railway_setting_checksum (rail_id_vddcx);
  uint16 railway_checksum_vddmx = railway_setting_checksum (rail_id_vddmx);
  uint16 rbcpr_checksum = rbcpr_cx_mx_settings_checksum();
#endif   
  uint16 recalculated_checksum;
  
#if 0
  uint32          i, j;
  uint32          nNumLevel;
  ClockPlanType   *pClockPlan;
#endif

  /* Get DDR Training parameters */
  training_params = (ddr_get_trained_params(SDRAM_INTERFACE_0));
  
  ddr_params_ch0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);
  
  if(training_params == NULL)
  {
    /* training parameters not found. */
    return TRUE;
  }

  /* Cookie indicates training is required, check training partition to see if
   * magic cookie is set
   */
  
  if(ddr_get_training_cookie() != DDR_PARAMS_MAGIC_NUM)
  {
    /* cookie does not match training parameter cookie */
    /* Training is required */
    return TRUE;
  }
  
  ddr_training_partition = ddr_params_get_training_data(&size);
  
  if(ddr_training_partition->version != DDR_TRAINING_PARAM_VERSION)
  {
    return TRUE;
    /* parameter version not matched, force retrain */
  }
 
  /* Check checksum for training data to see if training is required */
  serial_number = BOOT_HWIO_IN(QFPROM_RAW_SERIAL_NUM , 0);
  if(ddr_training_partition->training_checksum != ddr_get_training_checksum(serial_number))
  {
	return TRUE;
  }
  
  /* Check cookie to see if training is required */
  if(*ddr_shared_imem_ddr_training_cookie != DDR_TRAINING_REQUIRED)
  {
    return FALSE;
  }
#if 0
  if((nNumLevel-i) != training_params_ch0->nlevels)
  {
    /* if total number of frequencies needed to be trained has changed
     * retraining is required.
     */
    DALSYS_Free(pClockPlan);
    return TRUE;
  }

  for(j = 0 ;i < nNumLevel; i++, j++)
  {
    if( (pClockPlan[i].nFreqKHz != training_params_ch0->freq[j]))
    {
      /* if frequency does not match what we have trained,
       * retraining is required.
       */
      DALSYS_Free(pClockPlan);
      return TRUE;
    }
  }
  
  DALSYS_Free(pClockPlan);
#endif

/* Recalculate cx/mx checksum to determine if training is required */
  recalculated_checksum = railway_checksum_vddcx ^ railway_checksum_vddmx  ^ rbcpr_checksum;
  if(ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){
	if(recalculated_checksum != training_params->lpddr3_training.voltage_checksum){
		return TRUE;
	}
  }

  return FALSE;
}


/* =============================================================================
**  Function : ddr_do_phy_training
** =============================================================================
*/
/**
*   @brief
*   function called to do the DDR PHY training 
*
*   @param  None
*
*   @retval  NONE
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
void ddr_do_phy_training( void )
{
  SDRAM_CHIPSELECT chip_select_ch0;
  struct ddr_device_params_common *ddr_params_ch0;
  uint32          nNumLevel=0x0;
  uint32 serial_number;
  ClockPlanType   *pClockPlan;
  union ddr_trained_params *ddr_params;
//#if 0
#if ONE_TIME_TRAINING
  volatile uint32 * ddr_shared_imem_ddr_training_cookie = (uint32 *)SHARED_IMEM_DDR_TRAINING_COOKIE;
#endif

  struct ddr_params_partition * ddr_training_partition;
  uint32 size = 0;
//#if ONE_TIME_TRAINING
#if 0
  uint32 clk_plan[7] = {0};
#endif
  int32 i ;
  uint32 bimc_base = 0x00400000;
//#endif
  ddr_training_partition = ddr_params_get_training_data(&size);
  
  
  /* Get DDR device parameters */
  ddr_params_ch0 = &(ddr_get_params(SDRAM_INTERFACE_0)->common);

  if(ddr_params_ch0 == NULL)
  {
    return;
  }

  /* Call BIMCQuery to get the Num Perf Levels */
  boot_query_bimc_clock(CLOCK_RESOURCE_QUERY_NUM_PERF_LEVELS,&nNumLevel);

  DALSYS_Malloc(nNumLevel * sizeof(ClockPlanType), (void **)&pClockPlan);

  /* Call BIMCQuery to get the Clock Plan */
  boot_query_bimc_clock(CLOCK_RESOURCE_QUERY_ALL_FREQ_KHZ,pClockPlan);

  if ( pClockPlan == NULL ) {
    return;
  }
#if ONE_TIME_TRAINING
  i = nNumLevel-2;
  while(i>=0)
  {
#if 0  
     clk_plan[i] = pClockPlan[i+1].nFreqKHz;
#endif
     i--;
  }
#endif

  chip_select_ch0 = SDRAM_CS_NONE;
  if (ddr_params_ch0->num_banks_cs0 != 0)
  {
    chip_select_ch0 |= SDRAM_CS0;
  }
  if (ddr_params_ch0->num_banks_cs1 != 0)
  {
    chip_select_ch0 |= SDRAM_CS1;
  }

#if ONE_TIME_TRAINING
	 if(!ddr_training_is_required()) {
     if (ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){
	   ddr_pre_clock_switch(0, 796800, SDRAM_INTERFACE_0);
       boot_clock_set_bimcspeed(398400);
       ddr_post_clock_switch(0, 796800, SDRAM_INTERFACE_0);   
     }
     DDRSS_EBI1_PHY_Set_Training_Data(SDRAM_INTERFACE_0, ddr_training_partition->trained_params[0]);
     if (ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){
	  ddr_pre_clock_switch(0, 796800, SDRAM_INTERFACE_0);
       boot_clock_set_bimcspeed(398400);
       ddr_post_clock_switch(0, 796800, SDRAM_INTERFACE_0);
     }
	 
     BIMC_Enable_All_Periodic(bimc_base, SDRAM_INTERFACE_0, chip_select_ch0, 0);
     DDRPHY_CA_auto_io_cal(SEQ_DDRSS_EBI1_PHY_CH0_CA_DDRPHY_CA_OFFSET);	 
     DALSYS_Free(pClockPlan);
     return;
  }
#endif

  ddr_params = ddr_get_trained_params(SDRAM_INTERFACE_0);
  
  if((chip_select_ch0 != SDRAM_CS_NONE) && (ddr_params != NULL))
  {
    ddr_phy_training(SDRAM_INTERFACE_0, chip_select_ch0, ddr_params, nNumLevel,pClockPlan);
  }
  
  /* update training cookie */
  ddr_set_training_cookie(DDR_PARAMS_MAGIC_NUM);
  
  serial_number = BOOT_HWIO_IN(QFPROM_RAW_SERIAL_NUM , 0);
  /* update training data checksum */
  ddr_set_training_checksum(serial_number);

//#if 1
#if ONE_TIME_TRAINING
  #ifdef FEATURE_DDR_PARAM_SAVE_TO_EMMC
        *ddr_shared_imem_ddr_training_cookie = DDR_TRAINING_DATA_UPDATED;
  #else
        *ddr_shared_imem_ddr_training_cookie = DDR_TRAINING_DATA_NOT_UPDATED;      
  #endif
#endif

  ddr_training_partition->version = DDR_TRAINING_PARAM_VERSION;

  if (ddr_params_ch0->device_type == DDR_TYPE_LPDDR3){
    /* clock api to restore to high speed */
    ddr_pre_clock_switch(0, 796800, SDRAM_INTERFACE_0);
	boot_clock_set_bimcspeed(398400);
	ddr_post_clock_switch(0, 796800, SDRAM_INTERFACE_0);
  }
  
  DALSYS_Free(pClockPlan);

  return;

} /* ddr_do_phy_training */



/* =============================================================================
**  Function : ddr_configure_lpae
** =============================================================================
*/
/**
*   @brief
*   configures ddr for LPAE by statically splitting the memory in half.
*
*   @param high_mem_enabled     -  [IN] partition guid for high mem is enabled
*
*   @retval  void
*
*   @dependencies
*   ddr initialization has already finished
*
*   @sideeffects
*   None
*
*   @sa
*   None
*/
void ddr_configure_lpae(boolean high_mem_enabled)
{

} /* ddr_configure_lpae */


/* ============================================================================
**  Function : ddr_set_params_enhance,enhance set
** ============================================================================
*/
/**
*   @brief
*   This function sets the DDR DC timing parameters stored in CDB2 and CDB3.enhance set.
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
boolean ddr_set_params_enhance(void *ddr_params_ptr, uint32 ddr_params_size)
{
  return TRUE;
} /*ddr_set_params_enhance*/

