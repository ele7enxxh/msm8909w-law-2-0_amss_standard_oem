/**
 * @file boot_ddr_debug.c
 * @brief
 * Implementation for DDR Debug Mode.
 */
/*==============================================================================
                                EDIT HISTORY

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/boot/ddr/common/ddr_debug_phy_v1.c#1 $
$DateTime: 2015/09/01 00:30:35 $
$Author: pwbldsvc $
================================================================================
when       who     what, where, why
--------   ---     -------------------------------------------------------------
04/15/15   yps    Add DQ training function
12/10/14   yps    Fixed wrong include file name Boot_extern_clk_interface.h
11/25/14   yps    Porting code to 8909 platform
11/05/14   yps    Rename DDI file for diferent PHY.
05/11/14   yps    Porting code to 8916 platform
09/17/13   sl      Added more DDR tuning options.
09/13/13   sl      Fixed pslew/nslew order.
08/29/13   sl      Use USB APIs instead of Boot APIs.
08/20/13   sl      Added DDR tuning.
08/07/13   sr      Added watchdog reset support.
06/18/13   sl      Initial version.
================================================================================
                     Copyright 2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "stdio.h"
#include "string.h"
#include "qhsusb_al_bulk.h"
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_hwio.h"
#include "ddr_test.h"
#include "DDR_PHY.h"
#include "HAL_DDR.h"
#include "mpm_hwio.h"
#include "boot_extern_clk_interface.h"
#include "HALhwio.h"
#include "boot_extern_pmic_interface.h"
#include "ddr_debug_common.h"
#include "HALhwio.h"
//#include "msmhwioreg.h"
#include "pm_pon.h"
#include "ddr_target.h"
/*==============================================================================
                                  MACROS
==============================================================================*/
//#define SEC_WDOG_ENABLE
#ifdef SEC_WDOG_ENABLE
#define bite_timeout 0x0000ffff
#endif
#define PHY_DQ_ADDR(ddr_reg)  DIM_D00_DIM_DQ_##ddr_reg

/*==============================================================================
                                  DATA
==============================================================================*/
/* Echo and message buffers: must be addressable by USB and 4-byte aligned */
__attribute__((aligned(4)))

__attribute__((section("DDR_DEBUG_TUNING_COOKIE")))
volatile uint32 dq_rout, dq_pslew, dq_nslew, read_cdc_delay, write_cdc_delay,dq_wr_scan_byte;
int32  dq_rd_cdc_value = 0;
int32  vref = 0;
int32  dq_wr_cdc_value = 0;
uint32 failures[4];

uint32 each_byte_cdc_delay[PHY_DQ_MAX_NUM];

extern int32 scan_start ;
extern int32 scan_end ;
extern int32 scan_step;
extern int32 set_rout ;
extern uint32 DDR_TEST_SIZE;
#ifdef SEC_WDOG_ENABLE
extern void pm_ddi_wdg_init (void);
#endif

/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
pm_err_flag_type switch_cx_mx_to_turbo_mode(void)
{
  pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
  
  
  err_flag = pm_smps_volt_level(0,PM_SMPS_1,1287500);
  if(err_flag != PM_ERR_FLAG__SUCCESS)
  	return err_flag;
  
  
  err_flag = pm_ldo_volt_level(0,PM_LDO_3,1287500);
  return err_flag;
}
#ifdef SEC_WDOG_ENABLE
void sysdbg_enable(void)
{
  pm_ddi_wdg_init();
  pm_pon_ps_hold_cfg(0, PM_PON_RESET_CFG_WARM_RESET);
  // Set the debug timer after which controller can reset the system
  // 256 sclk cycles = ~ 7.8ms
  HWIO_OUTF(GCC_RESET_DEBUG, PRE_ARES_DEBUG_TIMER_VAL, 0x100);	
  // Enable / Disable the debug through watchdog feature
  HWIO_OUTF(GCC_RESET_DEBUG, SECURE_WDOG_DEBUG_EN, 0x1);
  HWIO_OUTF(GCC_RESET_DEBUG, MSM_TSENSE_RESET_DEBUG_EN, 0x1);
  HWIO_OUTF(GCC_RESET_DEBUG, PMIC_ABNORMAL_RESET_DEBUG_EN, 0x1);
  
  HWIO_OUTF(GCC_QDSS_DAP_AHB_CBCR, CLK_ENABLE, 0x1);
  HWIO_OUTF(GCC_QDSS_CFG_AHB_CBCR,CLK_ENABLE, 0x1);
  
  HWIO_OUTF(GCC_QDSS_DAP_CBCR, CLK_ENABLE, 0x1);
  HWIO_OUT(QDSS_WRAPPER_CS_DEBUG_UI_LOCKACCESS, 0xC5ACCE55);
  HWIO_OUTF(QDSS_WRAPPER_DEBUG_UI_SECURE, SEC_CTL, 0x1);
  HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL, 0x1);	   // Clear all status
  HWIO_OUT(QDSS_WRAPPER_DEBUG_UI_CTL_MASK, 0x0);	   // Clear SW_TRIG_MASK
  //HWIO_OUTF(GCC_RESET_DEBUG, BLOCK_RESIN, 0x1);
  
  //Set STOP_CAPTURE_DEBUG_TIMER to 2ms
  HWIO_OUT(GCC_STOP_CAPTURE_DEBUG_TIMER, 0x40);
  // Set the ETR timer to flush data out
  HWIO_OUT(GCC_FLUSH_ETR_DEBUG_TIMER, 0x180);
  // Flag to enable Debug
  //HWIO_OUTM(TCSR_RESET_DEBUG_SW_ENTRY, 0x1, 0X1);
  //   HWIO_OUTF(GCC_RESET_DEBUG,RESET_ACCESS_FIRST_PASS,0);
  //    HWIO_OUTF(GCC_RESET_DEBUG,RESET_DEBUG_FIRST_PASS,0);
  out_dword(0x01814000,(0x00600000&(~(1<<21))));
  out_dword(0x01814000,(0x00600000&(~(11<<20))));
  out_dword(0x01940000,(SCL_SYSTEM_DEBUG_BASE|RESET_DEBUG_SW_ENTRY_ENABLE));
}

	// We need to enable PROC_HALT_EN so that MSM pulls up PS_HOLD after reset
void ddr_debug_dog_init(boolean normal)
{
  sysdbg_enable();
}

void ddr_debug_dog_enable(boolean enable)
{
  /* Make sure the watchdog is disabled */
  //HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_EN, 0);
  out_dword(0x004AA004,0);
  /* Program bark and bite timeouts. */
  //HWIO_OUTF(MPM2_WDOG_BARK_VAL_REG, WDOG_BARK_VAL, bark_timeout);
  out_dword(0x004AA00c,bite_timeout);
  //while(HWIO_INF(MPM2_WDOG_BARK_VAL_REG, SYNC_STATUS) == 0);
  while((in_dword(0x004AA00c)&0x80000000)== 0);
  //HWIO_OUTF(MPM2_WDOG_BITE_VAL_REG, WDOG_BITE_VAL, bite_timeout);
  out_dword(0x004AA010,(bite_timeout-0xff));
  while((in_dword(0x004AA010)&0x80000000)== 0);
  
  //HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_CLK_EN, 1);
  out_dword(0x004AA004,0x80000001);
  /* Configure the WDOG sleep to be handled by HW and enable
     auto_petting */
  //HWIO_OUTF(MPM2_WDOG_CTL_REG, CHIP_AUTOPET_EN, 1);
  //out_dword(0x004AA004,0x80000004);
  //HWIO_OUTF(MPM2_WDOG_CTL_REG, HW_SLEEP_WAKEUP_EN, 1);
  //out_dword(0x004AA004,0x80000006);
  //HWIO_OUTF(MPM2_WDOG_CTL_REG, WDOG_EN, 1);
  //out_dword(0x004AA004,0x80000007);
  
  /* Reset to make sure the count starts at 0. */
  //HWIO_OUT(MPM2_WDOG_RESET_REG, 1);
  out_dword(0x004AA000,1); 
    
}

void ddr_debug_dog_kick( void )
{
}

#endif
// LPDDR2 VDDQ ---> LDO2
pm_err_flag_type pm_comm_write_byte(uint32 slave_id, uint16 addr, uint8 data, uint8 priority);

pm_err_flag_type sbl1_ddr_debug_set_vddq(uint32 vol_uv)
{
	pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
	uint8 val = 0x42; // default value

	if(vol_uv > 1537500 || vol_uv < 375000)
	{
		err_flag =	PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
	}
	else
	{
		val = (vol_uv - 375000) / 12500;
	   err_flag = pm_comm_write_byte(1,0x4141, val, 1);
	}

	return err_flag;
}

void ddr_debug_tuning_vddq(void)
{
	const char *cmd;
	uint8 VDDQ = 0;
	pm_err_flag_type ret_status;

	ddr_debug_output("Please input select (e.g, 1 for 1=1.2V  2=1.2125V 3=1.225V 4=1.2375V ... 9 =1.3V,step=0x0125V, maxset:1.3V ): ");

	cmd = ddr_debug_input();
	VDDQ = cmd[0] - '0';
	 
	if (( strlen(cmd) == 1)&& (VDDQ>0)&& (VDDQ<10))
	{
     ret_status = sbl1_ddr_debug_set_vddq(1200000+(VDDQ-1)*12500);
     if(PM_ERR_FLAG__SUCCESS!=ret_status)
     {
      ddr_debug_output("Set VDDQ failed\n");
     }
     else
     {
      ddr_debug_output("Set VDDQ ok\n");
     }
	}
	else
	{
	 ddr_debug_output("input failed \n");
	}
}
void ddr_tuning_init(uint16 *CDC_DELAY_START,uint16 *READ_CDC_DELAY_MAX,uint16 *WRITE_CDC_DELAY_MAX,uint8 * CDC_DELAY_INC)
{
   ddr_info ddi_ddr_info = ddr_get_info();
   
   memset(str,0,BUFFER_LENGTH);
   switch(ddi_ddr_info.ddr_type)
   	{
	case DDR_TYPE_LPDDR2:
		snprintf(str, BUFFER_LENGTH, "\r\n DDR type:LPDDR2\r\n");
		break;
    case DDR_TYPE_LPDDR3:
		snprintf(str, BUFFER_LENGTH, "\r\n DDR type:LPDDR3\r\n");
	    break;
	default:
		snprintf(str, BUFFER_LENGTH, "\r\n DDR type:unknown\r\n");
	    break;
   	}
   ddr_debug_output(str);
   memset(str,0,BUFFER_LENGTH);
    snprintf(str, BUFFER_LENGTH, "\r\n DDR Size:\r\n CS0 ADDR:0x%x; SIZE:%dMB\r\n CS1 ADDR:0x%x; SIZE:%dMB\r\n",
			(uint32)ddi_ddr_info.ddr_size.sdram0_cs0_addr,ddi_ddr_info.ddr_size.sdram0_cs0,
			(uint32)ddi_ddr_info.ddr_size.sdram0_cs1_addr,ddi_ddr_info.ddr_size.sdram0_cs1);
   ddr_debug_output(str);
  switch(boot_clock_get_ddr_speed())
 	{
 	case 600000:
 	case 533000:
	case 534000:
	case 400000:
	   *CDC_DELAY_START = scan_start;
	   *READ_CDC_DELAY_MAX = scan_end;
       *WRITE_CDC_DELAY_MAX = scan_end;
       *CDC_DELAY_INC = scan_step;
	    dq_pslew = dq_nslew = 0;
		memset(str,0,BUFFER_LENGTH);
		snprintf(str, BUFFER_LENGTH, "\r\n DDR test frequency on %dMHz. Test size 0x%xKB\r\n",boot_clock_get_ddr_speed()/1000,DDR_TEST_SIZE);
		ddr_debug_output(str);
      if(vref!=0)
      	{
		memset(str,0,BUFFER_LENGTH);
		snprintf(str, BUFFER_LENGTH, "\r\n Vref(%d,0,%d)\r\n",vref,vref);
		ddr_debug_output(str);
		vref = 0;
      	}

		memset(str,0,BUFFER_LENGTH);
				snprintf(str, BUFFER_LENGTH, "\r\n RCDA(%d,0,%d)\r\n",HWIO_ADDR(PHY_DQ_ADDR(RD_CDC_DELAY_CFG)),HWIO_ADDR(PHY_DQ_ADDR(RD_CDC_DELAY_CFG)));
		ddr_debug_output(str);
		memset(str,0,BUFFER_LENGTH);
				snprintf(str, BUFFER_LENGTH, "\r\n RCDM(4095,0,4095)\r\n");
		ddr_debug_output(str);
		memset(str,0,BUFFER_LENGTH);
				snprintf(str, BUFFER_LENGTH, "\r\n WCDA(%d,0,%d)\r\n",HWIO_ADDR(PHY_DQ_ADDR(WR_CDC_DELAY_CFG)),HWIO_ADDR(PHY_DQ_ADDR(WR_CDC_DELAY_CFG)));
		ddr_debug_output(str);
		memset(str,0,BUFFER_LENGTH);
				snprintf(str, BUFFER_LENGTH, "\r\n WCDM(4095,0,4095)\r\n");
		ddr_debug_output(str);
		
		memset(str,0,BUFFER_LENGTH);
		snprintf(str, BUFFER_LENGTH, "\r\n Scan range[%d,%d,%d]\r\n",scan_start,scan_step,scan_end);
		ddr_debug_output(str);
		ddr_debug_output("\r\n Pass range[259,679]\r\n");
		break;
		
 	case 333333:
		*CDC_DELAY_START = 210;
   	    *READ_CDC_DELAY_MAX = 1400;
        *WRITE_CDC_DELAY_MAX = 1400 ;
        *CDC_DELAY_INC = 5;
		ddr_debug_output("\r\n DDR test frequency on 333MHz\r\n");
		ddr_debug_output("\r\n Scan range[210,5,1400]\r\n");
		ddr_debug_output("\r\n Pass range[400,1100]\r\n");
	    break;

	default:
		break;
 	}
			
}
void ddr_tuning_log_out(uint32 failures[4],uint32 tempresult[4],uint16 CDC_DELAY_START,uint16 WRITE_CDC_DELAY_MAX)
{
	if(((failures[0]!=tempresult[0]) 
	  || (failures[1]!=tempresult[1]) 
	  || (failures[2]!=tempresult[2]) 
	  || (failures[3]!=tempresult[3]))
	  || failures[0]
	  || failures[1]
	  || failures[2]
	  || failures[3]
	  || (write_cdc_delay == CDC_DELAY_START)
	  || (write_cdc_delay == WRITE_CDC_DELAY_MAX))
	{
	  memset(str,0,BUFFER_LENGTH);
	  if( failures[0]==0
	   && failures[1]==0
	   && failures[2]==0
	   && failures[3]==0
		 )
	  {
		 snprintf(str, BUFFER_LENGTH, "%4u, %4u, %4u,"
								"%3u,%3u,%3u,%3u.[log]%s\r\n",
								dq_rout, read_cdc_delay, write_cdc_delay,
								failures[0], failures[1], failures[2], failures[3],"[stress test]PASS");
	  }
	  else
	  {
		 snprintf(str, BUFFER_LENGTH, "%4u, %4u, %4u,"
								"%3u,%3u,%3u,%3u.[log]%s\r\n",
								dq_rout, read_cdc_delay, write_cdc_delay,
								failures[0], failures[1], failures[2], failures[3],str_error_log);
	  }
	 
	  tempresult[0]= failures[0];
	  tempresult[1]= failures[1];
	  tempresult[2]= failures[2];
	  tempresult[3]= failures[3];
	  ddr_debug_output(str);
	  memset(str_error_log,0,BUFFER_LENGTH);
	}
	else
	{
	 // ddr_debug_output("* \n");
	}
}
void ddr_debug_do_tuning(boolean in_progress, boolean is_all, boolean is_training)
{

  ddr_size_info ddr_info;
  uint32 count;
  static uint32 tempresult[4]={0xFF};
  uint16 CDC_DELAY_START = 210;
  uint16 READ_CDC_DELAY_MAX = 1400;
  uint16 WRITE_CDC_DELAY_MAX = 1400 ;
  uint8 CDC_DELAY_INC = 5;
  boolean result = TRUE;
  /* CDC_DELAY_START is begin CDC delay configuration, we will scan CDC delay from CDC_DELAY_START to CDC_DELAY_MAX. 
	   CDC_DELAY_INC is CDC delay scan step, the stepping rate depend on DDR PHY. 
	  CDC delay units is ps.
   */
  ddr_tuning_init(&CDC_DELAY_START,&READ_CDC_DELAY_MAX,&WRITE_CDC_DELAY_MAX,&CDC_DELAY_INC);

  if (!in_progress)
  {
	ddr_debug_output("\r\nScan DDR: [rout, rdcdc, wrcdc]...\r\n");

    if (is_all||is_training) 
	{ 
	 dq_rout = dq_pslew = dq_nslew = 0;
	}
    read_cdc_delay = write_cdc_delay = CDC_DELAY_START;
  }
  else
  {
    memset(str,0,BUFFER_LENGTH);
    snprintf(str, BUFFER_LENGTH, "[%X, %X, %X]: "
                                 "X|X|X|X\r\n",
                                 dq_rout, read_cdc_delay, write_cdc_delay);
    ddr_debug_output(str);

    if ( (write_cdc_delay = (write_cdc_delay + CDC_DELAY_INC) & WRITE_CDC_DELAY_MAX) == 0 &&
         (read_cdc_delay = (read_cdc_delay + CDC_DELAY_INC) & READ_CDC_DELAY_MAX) == 0 &&
         ( !is_all || (dq_rout = (dq_rout + 1) & DQ_ROUT_MAX) == 0) )
    {
      return;
    }
  }

  ddr_info = ddr_get_size();

#ifdef SEC_WDOG_ENABLE
  /* Enable watchdog */
  ddr_debug_dog_enable(TRUE);
#endif
  do /* dq_rout */
  {
    if (dq_rd_cdc_value == 0)
    	{
		if( read_cdc_delay > READ_CDC_DELAY_MAX)
		   read_cdc_delay = CDC_DELAY_START ;
    	}
	else
		{
		 read_cdc_delay = dq_rd_cdc_value;
		}

	/* Update drive strength: rout/pslew/nslew */
	dq_pslew=dq_nslew=0;
	memset(str,0,BUFFER_LENGTH);
    snprintf(str, BUFFER_LENGTH, "ROUT:%d,PSLEW:%d.NSLEW:%d \r\n",
                                 dq_rout, dq_pslew, dq_pslew);
    ddr_debug_output(str);
	if(!is_training)
       HAL_SDRAM_PHY_Update_Drive_Strength(0,SDRAM_INTERFACE_0, dq_rout, dq_pslew, dq_nslew);

    do /* read_cdc_delay */
    {
      /* Update read CDC delay */
	  
      HAL_SDRAM_PHY_Update_CDC_Delay(0,SDRAM_INTERFACE_0, TRUE, read_cdc_delay);
	  if( dq_wr_cdc_value == 0)
	  	{
	  if( write_cdc_delay > WRITE_CDC_DELAY_MAX)
	      write_cdc_delay = CDC_DELAY_START;
	  	}
	  else
	  	{
	  	  write_cdc_delay = dq_wr_cdc_value;
	  	}

      do /* write_cdc_delay */
      {
        memset(str_error_log,0,BUFFER_LENGTH);
        /* Update write CDC delay */
		if(dq_wr_scan_byte == 0xFFFFFFFF)
		{
          HAL_SDRAM_PHY_Update_CDC_Delay(0,SDRAM_INTERFACE_0, FALSE, write_cdc_delay);
		}
		else
		{

		  each_byte_cdc_delay[0]= (dq_wr_scan_byte==0?write_cdc_delay:625);
		  each_byte_cdc_delay[1]= (dq_wr_scan_byte==1?write_cdc_delay:300);
		  each_byte_cdc_delay[2]= (dq_wr_scan_byte==2?write_cdc_delay:300);
		  each_byte_cdc_delay[3]= (dq_wr_scan_byte==3?write_cdc_delay:625);
		  HAL_SDRAM_PHY_Update_Per_Byte_CDC_Delay(0,SDRAM_INTERFACE_0, FALSE,each_byte_cdc_delay);

		}

        /* Issue manual IOCAL and CDCCAL to make new settings take effect */
        //HAL_SDRAM_PHY_Manual_IO_Cal(SDRAM_INTERFACE_0);
		/* Issue manual IOCAL */
        HAL_SDRAM_PHY_Manual_IO_Cal(0, SDRAM_INTERFACE_0, HAL_SDRAM_DDR_Wait);
		
        //HAL_SDRAM_PHY_Manual_CDC_Cal(SDRAM_INTERFACE_0);
		/* Issue manual CDCCAL */
		HAL_SDRAM_PHY_Manual_CDC_Cal(0, SDRAM_INTERFACE_0);

        failures[0] = failures[1] = failures[2] = failures[3] = 0;

        for (count = 0; count < DDR_TUNING_LOOP_COUNT; count++)
        {
          if (ddr_info.sdram0_cs0 != 0)
          {
            result=ddr_debug_test((uint32 *)(uint32)ddr_info.sdram0_cs0_addr, failures);
          }
          if (ddr_info.sdram0_cs1 != 0)
          {
            result=ddr_debug_test((uint32 *)(uint32)ddr_info.sdram0_cs1_addr, failures);
          }
        }
		//ddr_function_defect_test_bit_flip_tuning((uint32)ddr_info.sdram0_cs0_addr, BITFLIP_TEST_SIZE , failures);
		if(result)
		{
		ddr_defect_interval_test(0,SDRAM_INTERFACE_0, SDRAM_BOTH);
		}
	    ddr_tuning_log_out(failures,tempresult,CDC_DELAY_START,WRITE_CDC_DELAY_MAX);
#ifdef SEC_WDOG_ENABLE        
        /* Kick watchdog */
        ddr_debug_dog_kick();
#endif
      } while (((write_cdc_delay = (write_cdc_delay + CDC_DELAY_INC))<=WRITE_CDC_DELAY_MAX)&&!dq_wr_cdc_value);

    }while (((read_cdc_delay = (read_cdc_delay + CDC_DELAY_INC))<= READ_CDC_DELAY_MAX)&& !dq_rd_cdc_value);

	dq_rout = (dq_rout + 1) & DQ_ROUT_MAX;
  } while(is_all && dq_rout&& !is_training);

  dq_wr_cdc_value = 0;
  dq_rd_cdc_value = 0;
  ddr_debug_output("DDR tuning done.\r\n");
  ddr_debug_output("Please press ENTER to continue: ");

} /* ddr_debug_do_tuning */

void ddr_debug_ddr_training(void)
{
  pm_err_flag_type ret_status;	
  uint32 VDDQ_mv = 1200000;
  const char *cmd;
  
  ddr_debug_output("\r\n Please input ENV select (eg [0]:HT Training [1]:NT Training [2]:LT Training):");
  cmd = ddr_debug_input();
  memset(str,0,BUFFER_LENGTH);
  
  if ( strlen(cmd) == 1 &&cmd[0]>= '0' &&cmd[0]<= '2')
  {
	switch(cmd[0])
	{
	 case '0': //DQ training under HIGH temperature
	 
		ddr_debug_output("Start DQ training under HIGH temperture\r\n");
		VDDQ_mv = 1140000;
		break;
		
	 case '1': //DQ training under normal temperature
		ddr_debug_output("Start DQ training under Normal temperture\r\n");
		VDDQ_mv = 1200000;
		break;
		
	 case '2': //DQ training under LOW temperature
		ddr_debug_output("Start DQ training under LOW temperture\r\n");
		VDDQ_mv = 1300000;
		break;
	
	  default:
		break;
	}	
	
  	ret_status = sbl1_ddr_debug_set_vddq(VDDQ_mv);
	
	if(PM_ERR_FLAG__SUCCESS!=ret_status)
	{
	  snprintf(str, BUFFER_LENGTH, "Start DQ training VDDQ = %d\r\n",VDDQ_mv);
	  ddr_debug_output(str);
	}
	else
	{
	  ddr_debug_output("Set VDDQ ok\n");
	}
	        
    ddr_debug_dq_training(FALSE);
  }
  else
  {
	ddr_debug_output("input failed \n");
  }
}

void ddr_debug_cdc_tuning_manually(void)
{
  const char *cmd;
  ddr_debug_output("\r\n");
  while (1)
  {
	ddr_debug_output("Please input rout(e.g, 2): ");
	cmd = ddr_debug_input();

	if ( strlen(cmd) == 3 &&
		 cmd[0] >= '0' && cmd[0] <= '0' + DQ_ROUT_MAX &&
		 cmd[1] >= '0' && cmd[1] <= '0' + DQ_PSLEW_MAX &&
		 cmd[2] >= '0' && cmd[2] <= '0' + DQ_NSLEW_MAX )
	{
	  dq_rout = cmd[0] - '0';
	  dq_pslew = cmd[1] - '0';
	  dq_nslew = cmd[2] - '0';
	  ddr_debug_tune_cdc(FALSE);
	  break;
	}
	
    if ( strlen(cmd) == 1 &&
		 cmd[0] >= '0' && cmd[0] <= '0' + DQ_ROUT_MAX)
	{
	  dq_rout = cmd[0] - '0';
	  ddr_debug_tune_cdc(FALSE);
	  break;
	}
  }
}

