/**
 * @file bimc_ddrss_wrapper.c
 * @brief ddrss (DDR sub-system) level wrapping bimc APIs 
 * 
 */
/*==============================================================================
                                EDIT HISTORY

$Header $ $DateTime $
$Author: pwbldsvc $
================================================================================
when         who         what, where, why
----------   --------     -------------------------------------------------------------
01/08/15   sk         Resolved compilation error.
12/23/14   aus        Updated divide to an optimizaed divide routine
11/21/14   sk         changes for creating two elfs for RUMI and ASIC
11/06/14   sk         Removed BRIC register settings and API prototypes.
10/28/14   sk         Changes for 8952 Chipset
05/30/14   rjangili   Programmed BRIC global2 segment0 registers properly
05/12/14   nevedeth   Added APIs for enable,disable and manual IOCAL
05/09/14   nevedeth	  Added API to get/set HW_Self_Refresh on self refresh entry/exit respectively
02/05/14   cchou      Replace hard-code bimc_base/phy_base with defined values
01/08/14   cchou      Code optimization. Add API description.
09/01/13   cchou      Initial version.
================================================================================
                   Copyright 2013-2014 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                                             INCLUDES
==============================================================================*/
#include "ddr_params.h"
#include "ddr_target.h"
#include "ddr_config.h"
#include "bimc_scale.h"
#include "ddr_phy_ddrss.h"
#include "ddrss_rcw.h"
#include "ebi.h"

#ifndef SEQ_DDRSS_EBI1_PHY_CH1_CA_DDRPHY_CA_OFFSET
#define SEQ_DDRSS_EBI1_PHY_CH1_CA_DDRPHY_CA_OFFSET SEQ_DDRSS_EBI1_PHY_CH0_CA_DDRPHY_CA_OFFSET
#endif

#define REG_BASE_DDRPHY_CA(INTERFACE) ((INTERFACE == SDRAM_INTERFACE_0) ? \
    SEQ_DDRSS_EBI1_PHY_CH0_CA_DDRPHY_CA_OFFSET: \
    SEQ_DDRSS_EBI1_PHY_CH1_CA_DDRPHY_CA_OFFSET)
	

/*==============================================================================
                                                             FUNCTIONS
==============================================================================*/    
#pragma arm
uint32 ddr_divide_func(int32 a, uint32 b)
{ 
  uint32 quotient = 0;
  uint32 temp;
  
  do 
  {
    if (b == 0) break;
    if (a < 0) a = -a;

    __asm 
    { 
        MOV     temp, b                              // Put divisor in temp
        CMP     temp, a, LSR #1                      // double it until 2* temp is greater than dividend
        loop1:
            MOVLS   temp, temp, LSL #1         
            CMP     temp, a, LSR #1
            BLS     loop1                            
        loop2:
            CMP     a, temp                          // Can we subtract temp?
            SUBCS   a, a, temp                       // If we can, do so
            ADC     quotient, quotient, quotient     // Double quotient
            MOV     temp, temp, LSR #1               // Halve temp,
            CMP     temp, b                          // and loop until
            BHS     loop2                            // less than divisor
    } 
  } while (0);
  
  return quotient;
}
#pragma thumb

#if 0
/* needed for compilation purpose */
uint32 Get_BIMC_Base_Address()
{
	return((0));
}
#endif

/* ============================================================================
 **  Function : HAL_SDRAM_PHY_Disable_IO_Cal
 ** ============================================================================
 */
/*!
 *   @brief
 *   Disable auto IOCAL
 **/
void HAL_SDRAM_PHY_Disable_IO_Cal(SDRAM_INTERFACE interface)
{
  DDRPHY_CA_disable_auto_io_cal(Get_BIMC_Base_Address() + REG_BASE_DDRPHY_CA(interface));
} /* HAL_SDRAM_PHY_Disable_IO_Cal */

/* ============================================================================
 **  Function : HAL_SDRAM_PHY_Manual_IO_Cal
 ** ============================================================================
 */
/*!
 *   @brief
 *   Manual IOCAL
 **/
void HAL_SDRAM_PHY_Manual_IO_Cal(SDRAM_INTERFACE interface)
{
  DDRPHY_CA_manual_io_cal(Get_BIMC_Base_Address() + REG_BASE_DDRPHY_CA(interface));
} /* HAL_SDRAM_PHY_Manual_IO_Cal */

/* ============================================================================
 **  Function : HAL_SDRAM_PHY_Enable_IO_Cal
 ** ============================================================================
 */
/*!
 *   @brief
 *   enable IOCAL
 **/
void HAL_SDRAM_PHY_Enable_IO_Cal(SDRAM_INTERFACE interface)
{
  DDRPHY_CA_auto_io_cal(Get_BIMC_Base_Address() + REG_BASE_DDRPHY_CA(interface));
} /* HAL_SDRAM_PHY_Enable_IO_Cal */

/* =============================================================================
 **  Function : HAL_SDRAM_PHY_CA_Manual_IO_Cal
 ** =============================================================================
 */
/*!
 *   @brief
 *   manual IO calibration
 **/
 void HAL_SDRAM_PHY_CA_Manual_IO_Cal(SDRAM_INTERFACE interface)
 {
    DDRPHY_CA_manual_io_cal(REG_BASE_DDRPHY_CA(interface)); 
 } /* HAL_SDRAM_PHY_CA_Manual_IO_Cal */
 
/* =============================================================================
 **  Function : HAL_SDRAM_Init
 ** =============================================================================
 */
/**
 *   @brief
 *   Initialize DDR controller, as well as DDR device.
 **/
void HAL_SDRAM_Init(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_freq_in_khz)
{

  uint32 chipSel;  
  uint32 concurrent_sr = 0;
  uint32 clk_period_in_ps;
  #ifndef FEATURE_RUMI_BOOT
  uint32 dqs_pad_pull_status = 0;  
  struct ddr_device_params_common *ddr_params;
  #endif
  
  clk_period_in_ps = ddr_divide_func(1000000000, clk_freq_in_khz);
  #ifndef FEATURE_RUMI_BOOT
  /* Calculate DDR size based on parameters */
  ddr_params = &(ddr_get_params(interface)->common);
  
  /* PHY Initialization */
  /* clk_mode = 1, odt_en = 0 */
  if( ENABLE_LPDDR3 && (ddr_params->device_type == SDRAM_LPDDR3))
    EBI1_PHY_CFG_phy_init(SEQ_DDRSS_EBI1_PHY_OFFSET, 0, interface, dynamic_legacy, 0 );
  else if ( ENABLE_PCDDR3 && (ddr_params->device_type == SDRAM_PCDDR3)) {
    EBI1_PHY_CFG_phy_init(SEQ_DDRSS_EBI1_PHY_OFFSET, 1, interface, dynamic_legacy, 0 );
  }
    
  EBI1_PHY_CFG_DDR_CC_Pre_Init_Setup(SEQ_DDRSS_EBI1_PHY_OFFSET,interface);
  #endif 
  
  BIMC_Pre_Init_Setup (SEQ_DDRSS_BIMC_OFFSET,interface, clk_freq_in_khz, clk_period_in_ps);
  
  #ifndef FEATURE_RUMI_BOOT
  HAL_SDRAM_PHY_CA_Manual_IO_Cal(interface);
  #endif 
  
  BIMC_Device_Init(SEQ_DDRSS_BIMC_OFFSET,interface,chip_select,clk_freq_in_khz);

  #ifndef FEATURE_RUMI_BOOT
  /* Rank detection */
  // for PCDDR3, detect the dqs pull status and set it to Pull_Down status
  if(ddr_params->device_type == SDRAM_PCDDR3) {
    dqs_pad_pull_status = EBI1_PHY_CFG_dqs_pad_pull_status(SEQ_DDRSS_EBI1_PHY_OFFSET, interface);
    EBI1_PHY_CFG_set_dqs_pad_pull( SEQ_DDRSS_EBI1_PHY_OFFSET, interface, 1);
  }  
  #endif
  
  chipSel = BIMC_Rank_Detection(SEQ_DDRSS_BIMC_OFFSET,interface);
  
  #ifndef FEATURE_RUMI_BOOT
  //for PCDDR3, after rank detection, restore the status for dqs pull signal
  if(ddr_params->device_type == SDRAM_PCDDR3)
    EBI1_PHY_CFG_set_dqs_pad_pull( SEQ_DDRSS_EBI1_PHY_OFFSET, interface, dqs_pad_pull_status);
  #endif
  
  /* Size detection */
  BIMC_Ram_Size_Detection(SEQ_DDRSS_BIMC_OFFSET, interface);

  BIMC_Post_Init_Setup (SEQ_DDRSS_BIMC_OFFSET,interface,chipSel,concurrent_sr);  

} /* HAL_SDRAM_Init */

/* ============================================================================
 **  Function : HAL_SDRAM_Ram_Size_Detection
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will probe various address of ddr to find out the exact
 *   configuration of row and column size
 *   
 *   @details
 *   This function will probe various address of ddr to find out the exact
 *   configuration of row and column size. Special care is needed with device
 *   configured as 2x 16 bit or 4x 8 bit devices that combines into 1 32 bit 
 *   device as the MR8 reading will return just 1 of the device. We know however
 *   that HSDDRx does not support 16 or 8 bit devices, thus it has to be a a 
 *   multiple of the detected device that makes up to 32 bit.
 **/ 
boolean HAL_SDRAM_Ram_Size_Detection(SDRAM_INTERFACE interface)
{

  return BIMC_Ram_Size_Detection(SEQ_DDRSS_BIMC_OFFSET,interface);

} /* HAL_SDRAM_Ram_Size_Detection */

/* ============================================================================
 **  Function : HAL_SDRAM_Enter_Self_Refresh
 ** ============================================================================
 */
/*!
 *   @brief
 *   Puts the ddr into self refresh mode using idle self refresh
 *
 *   @details
 *   Puts the ddr into self refresh mode using idle self refresh.
 *   This means the controller will wait for the device to become idle
 *   before putting the ddr into self refresh.
 **/
void HAL_SDRAM_Enter_Self_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{

  /* Disable ranks */
  BIMC_Disable_Rank((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, chip_select); 
#if 0  
  /* Get HW_Self_Refresh Enable register value for the channels and store it */
  if (interface == SDRAM_INTERFACE_0) {
    if ((Get_BIMC_Base_Address() +SEQ_DDRSS_BIMC_OFFSET) != EBI1_BIMC_BASE)
    {
      ch_addr_0 -= RPM_BASE_ADDR;
	  ch_addr_ptr_0 = (uint32 *)ch_addr_0;
    }
    else
    {
	  ch_addr_ptr_0 =  (uint32 *)ch_addr_0;
    }  
    *(ch_addr_ptr_0) = BIMC_Get_HW_Self_Refresh_Reg((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface);
  }
  else {
    if ((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET) != EBI1_BIMC_BASE)
    {
      ch_addr_1 -= RPM_BASE_ADDR;
	  ch_addr_ptr_1 = (uint32 *)ch_addr_1;
    }
    else
    {
	  ch_addr_ptr_1 =  (uint32 *)ch_addr_1;
    } 
    *(ch_addr_ptr_1) = BIMC_Get_HW_Self_Refresh_Reg((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface);
  } 
#endif

  /* Disable HW based self refresh */
  BIMC_Disable_HW_Self_Refresh((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, chip_select);

  /* Disable auto IOCAL */
  HAL_SDRAM_PHY_Disable_IO_Cal(interface);

  /* Enter self refresh */
  BIMC_Enter_Self_Refresh((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, chip_select);
} /* HAL_SDRAM_Enter_Self_Refresh */

/* ============================================================================
 **  Function : HAL_SDRAM_Exit_Self_Refresh
 ** ============================================================================
 */
/*!
 *   @brief
 *   Takes the ddr out of self refresh mode using idle self refresh
 *
 *   @details
 *   Takes the ddr out of self refresh mode.
 *   After the ddr has been taken out of self refresh memory access can resume
 **/
void HAL_SDRAM_Exit_Self_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  uint32 concurrent_sr=0;
  
  /* Enable ranks */
  BIMC_Enable_Rank((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, chip_select);

  /* Issue manual IOCAL */
  HAL_SDRAM_PHY_Manual_IO_Cal(interface);

  /* Exit self refresh */ 
  BIMC_Exit_Self_Refresh((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, chip_select);

  /* Enable auto IOCAL */
  HAL_SDRAM_PHY_Enable_IO_Cal(interface);
#if 0
  /* Restore HW based self refresh value */
  if (interface == SDRAM_INTERFACE_0) {
    if ((Get_BIMC_Base_Address() +SEQ_DDRSS_BIMC_OFFSET) != EBI1_BIMC_BASE)
    {
      ch_addr_0 -= RPM_BASE_ADDR;
	  ch_addr_ptr_0 = (uint32 *)ch_addr_0;
    }
    else
    {
	  ch_addr_ptr_0 =  (uint32 *)ch_addr_0;
    }  
	BIMC_Set_HW_Self_Refresh_Reg((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, *(ch_addr_ptr_0));
  }
  else {
    if ((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET) != EBI1_BIMC_BASE)
    {
      ch_addr_1 -= RPM_BASE_ADDR;
	  ch_addr_ptr_1 = (uint32 *)ch_addr_1;
    }
    else
    {
	  ch_addr_ptr_1 =  (uint32 *)ch_addr_1;
    } 
	BIMC_Set_HW_Self_Refresh_Reg((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, *(ch_addr_ptr_1));
  } 
#endif
  if ((Get_BIMC_Base_Address() +SEQ_DDRSS_BIMC_OFFSET) != EBI1_BIMC_BASE)
  {
	ABHN_SHKE_Enable_HW_Self_Refresh(0x6044d000, chip_select, concurrent_sr);
  }
  else
  {
    ABHN_SHKE_Enable_HW_Self_Refresh(0x0044d000, chip_select, concurrent_sr);
  }
} /* HAL_SDRAM_Exit_Self_Refresh */



/* =============================================================================
 **  Function : HAL_SDRAM_BIMC_Rank_Detection
 ** =============================================================================
 */
/**
 *   @brief
 *   Detect present ranks by reading a mode register on that rank. A rank is
 *   present if and only if the rank is initialized and the mode register read
 *   does not time out.
 **/

SDRAM_CHIPSELECT HAL_SDRAM_BIMC_Rank_Detection(SDRAM_INTERFACE interface)
{  
  return (SDRAM_CHIPSELECT)BIMC_Rank_Detection(SEQ_DDRSS_BIMC_OFFSET,interface);
} /* HAL_SDRAM_BIMC_Rank_Detection */

/* =============================================================================
 **  Function : HAL_SDRAM_RL
 ** =============================================================================
 */
/**
 *   @brief
 *   Return Read Latency according to DDR type and frequency.
 **/
uint32 HAL_SDRAM_RL(SDRAM_INTERFACE interface, uint32 clk_freq_in_khz)
{
  return BIMC_Get_RL((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, clk_freq_in_khz);
} /* HAL_SDRAM_RL */

/* =============================================================================
 **  Function : HAL_SDRAM_WL
 ** =============================================================================
 */
/**
 *   @brief
 *   Return Write Latency according to DDR type and frequency.
 **/
uint32 HAL_SDRAM_WL(SDRAM_INTERFACE interface, uint32 clk_freq_in_khz)
{
  return BIMC_Get_WL((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, clk_freq_in_khz);
} /* HAL_SDRAM_WL */

/* =============================================================================
 **  Function : HAL_SDRAM_BIMC_Update_Clock_Period
 ** =============================================================================
 */
/**
 *   @brief
 *   Updates the bimc clock period for clock switching
 **/
void HAL_SDRAM_BIMC_Update_Clock_Period(SDRAM_INTERFACE interface, uint32 clk_period_in_ps)
{
  BIMC_BIMC_MISC_GLOBAL_CSR_Update_Clock_Period(SEQ_DDRSS_BIMC_OFFSET, interface, CLK_PERIOD_RESOLUTION_1PS, clk_period_in_ps);                                              
}

/* ============================================================================
 **  Function : HAL_SDRAM_Enter_Deep_Power_Down
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will put the ddr into DPD mode
 *
 *   @details
 *   This function will trigger the ddr to be put into dee power down mode.
 *   In the case where we only have 1 device on the interface (CS0 only), or both
 *   chip select are chosen, we will also turn off ddr clock.
 **/
__attribute__((section("ddr_boot_funcs")))
void HAL_SDRAM_Enter_Deep_Power_Down(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  //Disable periodic IO calibration via PHY CSRs. all phy IOCAl Disable 
  HAL_SDRAM_PHY_Disable_IO_Cal(interface);


  /* Put ddr into deep power down */  
  BIMC_Enter_DPD(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);

} /* HAL_SDRAM_Enter_Deep_Power_Down */

/* ============================================================================
 **  Function : HAL_SDRAM_Exit_Deep_Power_Down
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will take ddr out of deep power down
 *
 *   @details
 *   This function will take ddr out of deep power down
 **/
__attribute__((section("ddr_boot_funcs")))
void HAL_SDRAM_Exit_Deep_Power_Down(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 clk_freq_in_khz)
{
  uint32 chipSel;
  uint32 concurrent_sr = 0;
  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);

  /* Take ddr out of deep power down */
  BIMC_Exit_DPD(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select, clk_freq_in_khz);

  BIMC_Device_Init(SEQ_DDRSS_BIMC_OFFSET,interface,chip_select, clk_freq_in_khz);

  /* Rank detection */
  chipSel = BIMC_Rank_Detection(SEQ_DDRSS_BIMC_OFFSET,interface);

  if(ddr_params->common.device_type == SDRAM_LPDDR3) {
    /* Size detection */
    BIMC_Ram_Size_Detection(SEQ_DDRSS_BIMC_OFFSET, interface);
  }

  BIMC_Post_Init_Setup (SEQ_DDRSS_BIMC_OFFSET,interface,chipSel,concurrent_sr);  
  BIMC_Enable_All_Periodic(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select, concurrent_sr);

  //Enable periodic IO calibration via PHY CSRs.
  HAL_SDRAM_PHY_Enable_IO_Cal(interface);

} /* HAL_SDRAM_Exit_Deep_Power_Down */

/* ============================================================================
 **  Function : HAL_SDRAM_Get_Base_Addr
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function return the base address of the specific interface and chip select
 *   
 *   @details
 *   This function will return pre determined base address for the particular interface\chipselect requested
 **/
uint32 HAL_SDRAM_Get_Base_Addr(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  uint32 base_addr = 0xFFFFFFFF;
  uint32 sdram0_cs0_size = 0;
  uint32 sdram0_cs1_size = 0;
  uint32 sdram1_cs0_size = 0; 
  uint32 DDR_BASE = 0;
  uint32 ddr_interleaved = ddr_param_interleaved();
  struct ddr_device_params_common *ddr_params;
  

  /* Calculate DDR size based on parameters */
  ddr_params = &(ddr_get_params(SDRAM_INTERFACE_0)->common);

  if(BIMC_NUM_BYTES == 8){
    sdram0_cs0_size = ddr_params->num_banks_cs0
      << (ddr_params->num_rows_cs0 + ddr_params->num_cols_cs0 + 3);
    sdram0_cs1_size = ddr_params->num_banks_cs1
      << (ddr_params->num_rows_cs1 + ddr_params->num_cols_cs1 + 3);
  }
  else if(BIMC_NUM_BYTES == 4) {
    sdram0_cs0_size = ddr_params->num_banks_cs0
      << (ddr_params->num_rows_cs0 + ddr_params->num_cols_cs0 + 2);
    sdram0_cs1_size = ddr_params->num_banks_cs1
      << (ddr_params->num_rows_cs1 + ddr_params->num_cols_cs1 + 2);
  }
  ddr_params = &(ddr_get_params(SDRAM_INTERFACE_1)->common);

  if(BIMC_NUM_BYTES == 8){
    sdram1_cs0_size = ddr_params->num_banks_cs0
      << (ddr_params->num_rows_cs0 + ddr_params->num_cols_cs0 + 3);
  }                    
  else if(BIMC_NUM_BYTES == 4) {
    sdram1_cs0_size = ddr_params->num_banks_cs0
      << (ddr_params->num_rows_cs0 + ddr_params->num_cols_cs0 + 2); 
  }

  if ((sdram0_cs0_size + sdram0_cs1_size) <= 0x80000000) {
  	DDR_BASE = DDR_BASE_1;
  }
  else {
  	DDR_BASE = DDR_BASE_2;
  }
  
  if(chip_select == SDRAM_CS0)
  {
    if(interface_name == SDRAM_INTERFACE_0)
    {
      /* Regardless of interleave vs non-interleave
       * we always populate CH0 CS0 at the base,
       * no calculation needed.
       */
       if (DDR_BASE && sdram0_cs1_size && sdram0_cs0_size && (sdram0_cs0_size > sdram0_cs1_size)) {
         base_addr = DDR_BASE + sdram0_cs1_size;
      }
	  else {
         base_addr = DDR_BASE;
      }
    }
    else /* if (interface_name == SDRAM_INTERFACE_1) */
    {
      /* CH1 CS0 base address depends on interleave vs non-interleave
       * interleaved base will be [base of CH0 CS0]
       * non-interleaved base will be [base of CH0 CS0] +
       *                              [ramsize of CH0 CS0] +
       *                              [ramsize of CH0 CS1]
       */
      if(ddr_interleaved & SDRAM_CS0_INTERLEAVE)
      {
        base_addr = DDR_BASE;
      }
      else
      {
        /* Non-interleaved memory follows logical base address increments
         * CH0 CS1 follows CH0 CS0,
         * CH1 CS0 follows CH0 CS1,
         * etc.
         */
        base_addr = DDR_BASE + (sdram0_cs0_size + sdram0_cs1_size);
      }
    }
  }
  else /* if(chip_select = SDRAM_CS1) */
  {
    /* CH0 CS1 base address depends on interleave vs non-interleave
     * interleaved base will be [base of CH0 CS0] + [ramsize of CH0 CS0] * 2
     * non-interleaved base will be [base of CH0 CS0] + [ramsize of CH0 CS0]
     */
    if(ddr_interleaved & SDRAM_CS1_INTERLEAVE)
    {
      base_addr = DDR_BASE + (sdram0_cs0_size * 2);
    }
    else
    {
      /* Non-interleaved memory follows logical base address increments
       * CH0 CS1 follows CH0 CS0,
       * CH1 CS0 follows CH0 CS1,
       * etc.
       */
      if(interface_name == SDRAM_INTERFACE_0)
      {
        if (DDR_BASE && sdram0_cs1_size && sdram0_cs0_size && (sdram0_cs0_size > sdram0_cs1_size)) {
          base_addr = DDR_BASE;
        }
        else {
          base_addr = DDR_BASE + sdram0_cs0_size ;
	    }
      }
      else
      {
        base_addr = DDR_BASE + (sdram0_cs0_size + sdram0_cs1_size + sdram1_cs0_size);
      }
    }
  }

  return base_addr;

} /* HAL_SDRAM_Get_Base_Addr */


/* ============================================================================
 **  Function : HAL_SDRAM_SCMO_Rank_Disable
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will initialize the disable the rank given in the parameter
 *
 *   @details
 *   This function will initialize the disable the rank given in the parameter
 **/
void HAL_SDRAM_SCMO_Rank_Disable(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  SCMO_CFG_Disable_Rank( SEQ_DDRSS_BIMC_OFFSET, chip_select );
}

/* ============================================================================
 **  Function : HAL_SDRAM_SCMO_Rank_Enable
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will initialize the enable the rank given in the parameter
 *
 *   @details
 *   This function will initialize the enable the rank given in the parameter
 **/
void HAL_SDRAM_SCMO_Rank_Enable(SDRAM_INTERFACE interface_name, SDRAM_CHIPSELECT chip_select)
{
  SCMO_CFG_Enable_Rank( SEQ_DDRSS_BIMC_OFFSET, chip_select );
}


/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Enter_Self_Refresh
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will put the requested ddr into self refresh
 *
 *   @details
 *   This function will put the requested ddr into self refresh.
 *   Note in dual rank configuration both rank must go into self refresh
 *   at the same time.
 **/
void HAL_SDRAM_SHKE_Enter_Self_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Enter_Self_Refresh(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Enter_Self_Refresh */

/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Exit_Self_Refresh
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will take the requested ddr out of self refresh
 *
 *   @details
 *   This function will take the requested ddr out of self refresh.
 *   Note in dual rank configuration both rank must go and out of self refresh
 *   at the same time.
 **/
void HAL_SDRAM_SHKE_Exit_Self_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Exit_Self_Refresh(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Exit_Self_Refresh */

/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Enable_Auto_Refresh
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will turn on periodic auto refresh
 *
 *   @details
 *   This function will turn on periodic auto refresh
 **/
void HAL_SDRAM_SHKE_Enable_Auto_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Enable_Auto_Refresh(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Enable_Auto_Refresh */

/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Disable_Auto_Refresh
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will turn off periodic auto refresh
 *
 *   @details
 *   This function will turn off periodic auto refresh
 **/
void HAL_SDRAM_SHKE_Disable_Auto_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Disable_Auto_Refresh(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Disable_Auto_Refresh */

/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Enable_HW_Self_Refresh
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will take turn on the hw based self refresh
 *
 *   @details
 *   This function will take turn on the hardware based self refresh.
 *   This will put the ddr into self refresh after a predetermined idle duration.
 **/
void HAL_SDRAM_SHKE_Enable_HW_Self_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Enable_HW_Self_Refresh(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select,0);
} /* HAL_SDRAM_SHKE_Enable_HW_Self_Refresh */

/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Disable_HW_Self_Refresh
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will take turn off the hw based self refresh
 *
 *   @details
 *   This function will take turn off the hardware based self refresh.
 **/
void HAL_SDRAM_SHKE_Disable_HW_Self_Refresh(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Disable_HW_Self_Refresh(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Disable_HW_Self_Refresh */


/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Enable_ZQCAL
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will turn on period ZQCAL
 *
 *   @details
 *   This function will turn on period ZQCAL
 **/
void HAL_SDRAM_SHKE_Enable_ZQCAL(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Enable_Periodic_ZQCAL(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Enable_ZQCAL */

/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Disable_ZQCAL
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will turn off period ZQCAL
 *
 *   @details
 *   This function will turn off period ZQCAL
 **/
void HAL_SDRAM_SHKE_Disable_ZQCAL(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Disable_Periodic_ZQCAL(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Disable_ZQCAL */

/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Enable_Periodic_SRR
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will turn on period SRR
 *
 *   @details
 *   This function will turn on period SRR to enable temperature compensated
 *   auto refresh.
 **/
void HAL_SDRAM_SHKE_Enable_Periodic_SRR(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Enable_Periodic_SRR(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Enable_Periodic_SRR */

/* ============================================================================
 **  Function : HAL_SDRAM_SHKE_Disable_Periodic_SRR
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function will turn off period SRR
 *
 *   @details
 *   This function will turn off period SRR to enable temperature compensated
 *   auto refresh.
 **/
void HAL_SDRAM_SHKE_Disable_Periodic_SRR(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select)
{
  BIMC_Disable_Periodic_SRR(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select);
} /* HAL_SDRAM_SHKE_Disable_Periodic_SRR */



/* ============================================================================
 **  Function : HAL_SDRAM_Pre_Clock_Switch
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function is used for changing  the core frequency of operating for hte memory controller
 *
 *   @details
 *   This function execute the hardware driven clock frequency switch which is triggered by the memory
 *  controller.
 **/
void HAL_SDRAM_Pre_Clock_Switch (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz)
{
  uint32 clk_period_in_ps=0;

  union ddr_device_params *ddr_params;
  ddr_params = ddr_get_params(interface);
  

  switch(clk_freq_in_khz)
  {  case 19200: clk_period_in_ps=0xCB73;break;
	 case 100000: clk_period_in_ps=0x2710; break;
	 case 200000: clk_period_in_ps=0x1399; break;
	 case 259200: clk_period_in_ps=0x0F12; break;
	 case 333600: clk_period_in_ps=0x0BB5; break;
	 case 403200: clk_period_in_ps=0x09B0;break;
	 case 518400: clk_period_in_ps=0x0789; break;
	 case 667200: clk_period_in_ps=0x05DA; break;
	 case 800000: clk_period_in_ps=0x04E2; break;
         case 796800: clk_period_in_ps=0x04E7; break;
         case 331200: clk_period_in_ps=0x0BCB; break;
         case 662400: clk_period_in_ps=0x05E5; break;
         default: clk_period_in_ps = ddr_divide_func(1000000000, clk_freq_in_khz); break;	 
	}
  if ( ENABLE_LPDDR3 && (ddr_params->common.device_type == SDRAM_LPDDR3))
    BIMC_Clock_Switch_LPDDR3((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, clk_freq_in_khz, clk_period_in_ps);
  else if ( ENABLE_PCDDR3 && (ddr_params->common.device_type == SDRAM_PCDDR3))
    BIMC_Clock_Switch_PCDDR3((Get_BIMC_Base_Address() + SEQ_DDRSS_BIMC_OFFSET), interface, clk_freq_in_khz, clk_period_in_ps);
} /* HAL_SDRAM_Pre_Clock_Switch */

/* ============================================================================
 **  Function : HAL_SDRAM_Post_Clock_Switch
 ** ============================================================================
 */
/*!
 *   @brief
 *   This function is used for changing  the core frequency of operating for hte memory controller
 *
 *   @details
 *   This function execute the hardware driven clock frequency switch which is triggered by the memory
 *  controller.
 **/
void HAL_SDRAM_Post_Clock_Switch (SDRAM_INTERFACE interface, uint32 clk_freq_in_khz)
{

} /* HAL_SDRAM_Post_Clock_Switch */

/* =============================================================================
 **  Function : HAL_SDRAM_Write_MR
 ** =============================================================================
 */
/**
 *   @brief
 *   Write DDR mode register.
 **/
void HAL_SDRAM_Write_MR(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr, uint32 mr_data)
{
  BIMC_Write_MR( SEQ_DDRSS_BIMC_OFFSET , interface, chip_select, mr_addr, mr_data );
}

/* =============================================================================
 **  Function : HAL_SDRAM_Read_MR
 ** =============================================================================
 */
/**
 *   @brief
 *   Read DDR mode register Lower 32-bit data.
 **/
uint32 HAL_SDRAM_Read_MR(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr)
{
  return((uint32)(BIMC_Read_MR(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select, mr_addr)));

}

/* =============================================================================
 **  Function : HAL_SDRAM_Read_MR_Upper
 ** =============================================================================
 */
/**
 *   @brief
 *   Read DDR mode register higher 32-bit data.
 **/
uint32 HAL_SDRAM_Read_MR_Upper(SDRAM_INTERFACE interface, SDRAM_CHIPSELECT chip_select, uint32 mr_addr)
{
  return((uint32)(BIMC_Read_MR(SEQ_DDRSS_BIMC_OFFSET, interface, chip_select, mr_addr) >> 32));
}

