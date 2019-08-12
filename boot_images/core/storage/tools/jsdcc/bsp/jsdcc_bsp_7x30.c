/**********************************************************************
* jsdcc_bsp_7x30.c
*
* SDCC(Secure Digital Card Controller) driver BSP for TOOL.
*
* This file implements the SDCC driver BSP for the board in use for TOOL.
*
* Copyright (c) 2009-2010
* Qualcomm Technologies Incorporated.
* All Rights Reserved.
* Qualcomm Confidential and Proprietary
*
**********************************************************************/

/*=======================================================================
                        Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/tools/jsdcc/bsp/jsdcc_bsp_7x30.c#1 $
$DateTime: 2015/09/01 00:30:35 $

YYYY-MM-DD   who     what, where, why
----------------------------------------------------------------------
2010-04-21   vj      Use TCXO as clock source for tool
2009-11-17   rh      Adding the stub out code for L4 INTLOCK/FREE
2009-08-18   rh      Adding the BSP support for 7x30
===========================================================================*/

/*==============================================================================
  Includes for this module.
  ----------------------------------------------------------------------------*/

#include "msm.h"
#include "jsdcc_bsp.h"

/*==============================================================================
                               DEFINES
==============================================================================*/

#define BUS_SPEED_IN_MHZ      20
#define OPERATION_TIMEOUT_CNT 1000

/* Static configuration values for the SSBI bus link to PMIC */
#define SSBI_READ_CMD             0x1
#define SSBI_CMD_POS              0x18
#define SSBI_REG_POS              0x10
#define PMIC_BANKSEL_REG          0x4f
#define PMIC_GPIO25_REG           0x168

/* Some MACRO for accessing the SSBI registers */
/* SSBI2 10 bit write routine */
#define PMIC_REG_WRITE10b(addr, val) \
         do {  \
         (void) jsdcc_wait_for_sbi(                                          \
                                BUS_SPEED_IN_MHZ,                            \
                                HWIO_ADDR(PMIC_SSBI2_STATUS),                \
                                HWIO_FMSK(PMIC_SSBI2_STATUS, READY),         \
                                (0x1<<HWIO_SHFT(PMIC_SSBI2_STATUS,READY)) ); \
         HWIO_OUT(PMIC_SSBI2_MODE2, ((addr >> 8) << 4) & 0x0ff0 | (0x05) );  \
         HWIO_OUT(PMIC_SSBI2_CMD, (addr << SSBI_REG_POS) & 0xff0000 | \
                                  (val & 0x0000ff) );  \
         }while(0)

/* SSBI 8 bit write routine */
#define PMIC_REG_WRITE(addr, val) \
         do {  \
         (void) jsdcc_wait_for_sbi(                                          \
                                BUS_SPEED_IN_MHZ,                            \
                                HWIO_ADDR(PMIC_SSBI2_STATUS),                \
                                HWIO_FMSK(PMIC_SSBI2_STATUS, READY),         \
                                (0x1<<HWIO_SHFT(PMIC_SSBI2_STATUS,READY)) ); \
         HWIO_OUT(PMIC_SSBI2_CMD, (addr << SSBI_REG_POS) & 0xff0000 | \
                                  (val & 0x0000ff) );  \
         }while(0)

/*==============================================================================
================================================================================
                         J S D C C    B S P
==============================================================================*/

/******************************************************************************
* Name: jsdcc_wait_for_sbi
*
* Description:
*    This function polls for specific SBI register value.
*
* Arguments:
*    [IN]: clock cycle to be paused
*    [IN]: register address
*    [IN]: mask of the register
*    [IN]: value to be compared
*
* Returns:
*    TRUE on success
*
* Note:
*
******************************************************************************/
static boolean jsdcc_wait_for_sbi
(
   uint32 wait_cycle,   /* clock cycle to be paused */
   uint32 reg_addr,     /* register address         */
   uint8 mask,          /* mask of the register     */
   uint8 val            /* value to be compared     */
)
{
   uint32 count = 0;  /* count as per us */

   /* Poll device status every one micro second
      so we return as soon as possible. Timeout if we exceed count. */
   while ( ((in_byte(reg_addr) & mask) != val)  &&
           (count < OPERATION_TIMEOUT_CNT) )
   {
      HWIO_OUT( MSSAHB_PAUSE_TIMER, wait_cycle );
      count++;
   }

   if ( count < OPERATION_TIMEOUT_CNT )
   {
      return TRUE;
   }
   return FALSE;
} 

/******************************************************************************
* Name: jsdcc_sbi_read
*
* Description:
*    This function reads the requested pmic register
*
* Arguments:
*    [IN]: register address
*    [OUT]: value
*
* Returns:
*    TRUE on success
*
* Note:
*
******************************************************************************/
static boolean jsdcc_sbi_read(uint32 reg, uint32* data)
{
   uint32 ret_value = FALSE;

   *data = 0;

   /* Set the higher bits of the register address - used only for 10 bit mode 
    * To do this, use the following example code to set the high bit:
    * HWIO_OUT(PMIC_SSBI2_MODE2, ((reg >> 8) << 4) & 0x0ff0 | (0x05) );  */

   /* Read value of PMIC register */
   HWIO_OUT(PMIC_SSBI2_CMD, (SSBI_READ_CMD << SSBI_CMD_POS |
            ((reg & 0xff) << SSBI_REG_POS)));

   /* Poll RD_READY to know when read data has returned */
   /* bit gets clear once SSBI_RD is performed          */
   
   ret_value = jsdcc_wait_for_sbi(
                                BUS_SPEED_IN_MHZ,
                                HWIO_ADDR(PMIC_SSBI2_STATUS), 
                                HWIO_FMSK(PMIC_SSBI2_STATUS, RD_READY),
                                (0x1<<HWIO_SHFT(PMIC_SSBI2_STATUS,RD_READY))
                                ); 
   /* Double check the status before proceeding */
   if (FALSE == ret_value) 
   {
      return FALSE;
   }
   else
   {
      *data =  HWIO_INM(PMIC_SSBI2_RD, HWIO_FMSK(PMIC_SSBI2_RD, REG_DATA));
   }
   return TRUE;
} /* sdcc_bsp_sbi_read */


/******************************************************************************
* Name: jsdcc_bsp_tool_config_pll
*
* Description:
*    Setup the PLL for default frequency required so sdcc_boot_config_clock
*    can get the proper clock source.  
*
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_config_pll(void)
{
}  /* sdcc_bsp_boot_config_pll */


/******************************************************************************
* Name: jsdcc_bsp_tool_tune_pll
*
* Description:
*    Change the output frequency of the PLL.  This function is used to
*    fine tune the output frequency of the PLL which is used mainly to
*    detect data corruption on a specific clock frequency.  
*
* Arguments:
*    arg_L - Multiplier settting for the PLL
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_tune_pll(uint32 arg_L)
{
}  /* sdcc_bsp_boot_config_pll */


/******************************************************************************
* Name: jsdcc_bsp_tool_enable_pmic_ssbi_clk
*
* Description:
*    Setup the clock source for PMIC SSBI bus 
*    This function has undergone a slight change in 7x30, which also turn
*    on the power supply after enabling the SSBI clock source
*    
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_tool_enable_pmic_ssbi_clk(void)
{
   boolean err_val;
   uint32 reg_val= 0x0;

   /* Enable SSBI clock */
   if(!HWIO_INM(PMIC_SSBI_NS_REG, HWIO_FMSK(PMIC_SSBI_NS_REG, PMIC_SSBI_ROOT_ENA)))
   {
      HWIO_OUTM( PMIC_SSBI_NS_REG,
              HWIO_FMSK(PMIC_SSBI_NS_REG, PMIC_SSBI_CLK_BRANCH_ENA) |
              HWIO_FMSK(PMIC_SSBI_NS_REG, PMIC_SSBI_ROOT_ENA),
              HWIO_FMSK(PMIC_SSBI_NS_REG, PMIC_SSBI_CLK_BRANCH_ENA) |
              HWIO_FMSK(PMIC_SSBI_NS_REG, PMIC_SSBI_ROOT_ENA)
              );     
   }

   /* Configure the SSBI GPIO to enable bit output */
   HWIO_OUTM(MPM_IO, HWIO_FMSK(MPM_IO, MPM_GPIO2_CFG) |
                     HWIO_FMSK(MPM_IO, MPM_GPIO2_OE), 
                     HWIO_FMSK(MPM_IO, MPM_GPIO2_CFG) |
                     HWIO_FMSK(MPM_IO, MPM_GPIO2_OE)
                     );

   /* Start by resetting the SSBI register */
   HWIO_OUT(PMIC_SSBI2_RESET, 0x0);

   /* Configure the SSBI Mode register to default SSBI mode */
   HWIO_OUT(PMIC_SSBI2_MODE2, 0x00);

   /* Set the default control register */
   HWIO_OUT(PMIC_SSBI2_CTL, 0x8005);

   /* Set the default priority */
   HWIO_OUT(PMIC_SSBI2_PRIORITIES, 0x88);

   /* Set the PMIC MPM mode to SSBI */
   HWIO_OUT(MPMC_ARM9_MPM_PMIC_SBI_SEL, 0x04);


   /* Go ahead and turn on the PMIC power supply */
   /* Need to turn on PMIC_GPIO_25 to get power to the SD card slot */

   /* Select first bank to write to */
   PMIC_REG_WRITE(PMIC_BANKSEL_REG, 2);

   /* Read value of PMIC register 0x12 */
   err_val = jsdcc_sbi_read(PMIC_GPIO25_REG, &reg_val);
   if (err_val == TRUE)
   {
      /* The following sequence is a set of magic sequence obtained from the PMIC team on how
       * to turn GPIO25 on and drive it low.  For reference how it is actually done, please see
       * document 80-VB073-348 */
      /* Enable mode select */
      PMIC_REG_WRITE(PMIC_GPIO25_REG, 0x81);
      err_val = jsdcc_sbi_read(PMIC_GPIO25_REG, &reg_val);

      /* drive strength */
      PMIC_REG_WRITE(PMIC_GPIO25_REG, 0x98);
      err_val = jsdcc_sbi_read(PMIC_GPIO25_REG, &reg_val);

      /* drive strength */
      PMIC_REG_WRITE(PMIC_GPIO25_REG, 0xBC);
      err_val = jsdcc_sbi_read(PMIC_GPIO25_REG, &reg_val);

      /* drive source slect to 0 */
      PMIC_REG_WRITE(PMIC_GPIO25_REG, 0xC0);
      err_val = jsdcc_sbi_read(PMIC_GPIO25_REG, &reg_val);
   }
}  /* jsdcc_bsp_tool_enable_pmic_ssbi_clk */

 
/******************************************************************************
* Name: jsdcc_bsp_kill_watchdog
*
* Description:
*    This function put the watchdog timer in freeze state
*    
* Arguments:
*    None
*
* Returns:
*    None
*
* Note:
*
******************************************************************************/
void jsdcc_bsp_kill_watchdog(void)
{
   HWIO_OUT(WDOG_FREEZE, HWIO_FMSK(WDOG_FREEZE,WDOG_FREEZE));
}


/* Stub out functions - Required to compile the code */
L4_Word_t int_lock_ext( void )
{
   return 0;
}

void int_free_ext( void )
{
}

