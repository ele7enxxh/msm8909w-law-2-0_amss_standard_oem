/*===========================================================================

FILE:         BSPspm.c

DESCRIPTION:  
  This is the BSP data for the SPM driver.
  This data is for the 8916 MSS processor.

                Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/config/asic/9609/SPMDevCfgData.c#1 $

============================================================================*/


/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "BSPspm.h"

/* -----------------------------------------------------------------------
**                           Constants
** ----------------------------------------------------------------------- */
    
/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**             START - QDSP6 SAW2 POWER OUTPUT CONTROL BITS
** The bits are toggled using the bit index (not bitmask). So the initial
** value of each bit at boot time is crucial to the command sequence.
** ----------------------------------------------------------------------- */
typedef enum 
{
  SPM_CRPC_Q6_RESET,                        /* 0  */
  SPM_CRPC_Q6_CUST_MEM_CLAMP,               /* 1  */
  SPM_CRPC_Q6_CLAMP_IO,                     /* 2  */
  SPM_CRPC_Q6_COMP_MEM_CLAMP,               /* 3  */
  SPM_CRPC_Q6_CLOCK_DIS,                    /* 4  */
  SPM_CRPC_Q6_PLL_FREEZE,                   /* 5  */
  SPM_CRPC_Q6_HSF,                          /* 6  */
  SPM_CRPC_Q6_MEM_SOURCE_BIAS_EN,           /* 7  */
  SPM_CRPC_Q6_BHS,                          /* 8  */
  SPM_CRPC_Q6_RETAIN_IO,
  SPM_CRPC_Q6_LDO_BYPASS_OFF,
  SPM_CRPC_Q6_RESTORE_IO,
  SPM_CRPC_Q6_LDO_OFF,
  SPM_WAKEUP_IRQ                 = 13,      /* 13 */
  SPM_MSS_PLL_RESET,                        /* 14 */
  SPM_MSS_RES_15,                           /* 15 */
  SPM_MSS_PLL_BYPASS,                       /* 16 */
  SPM_MSS_RES_17,                           /* 17 */
  SPM_MSS_PLL_OUT_DIS,                      /* 18 */
  SPM_MSS_RES_19,                           /* 19 */
  SPM_CRPC_Q6_LDO_ON,
  SPM_MSS_RES_21,                           /* 21 */
  SPM_MSS_SPARE_1,
  SPM_MSS_RES_23,                           /* 23 */
  SPM_MSS_CLAMP_IO,
  SPM_MSS_RES_25,                           /* 25 */
  SPM_MSS_CLAMP_MEM,
  SPM_MSS_RES_27,                           /* 27 */
  SPM_MSS_XO_EN                  = 28,      /* 28 */
  SPM_CRPC_Q6_MEM_RET_SLEEP,                /* 29 */
  SPM_CRPC_Q6_LDO_VOLT_SEL,
  SPM_MSS_RES_31                            /* 31 */
} SPM_SEQ_PWR_CTL_BITS;

#define SPM_SEQ_PWR_CTL(bit,delay) (bit<<4)|(delay<<1)|(0x0)

typedef enum
{
  NO_RPM_HS,
  RPM_HS
} SPM_SEQ_SLEEP_BITS;

#define SPM_SEQ_SLEEP_CTL(bit,delay) (bit<<2)|(0x3)
  
typedef enum
{
  EVENT_0,
  EVENT_1,
  EVENT_2,
  EVENT_3
} SPM_SEQ_EVENT_BITS;

#define SPM_SEQ_EVENT_CTL(bit,delay) (bit<<4)|(0xB)

typedef enum
{
  DLY1 = 1,
  DLY2 = 2,
  DLY3 = 3
} SPM_SEQ_DLY_BITS;

#define SPM_SEQ_DLY_CTL(bit,nop) (bit<<4)|(0xF)

#define SPM_SEQ_END_CTL(bit,delay) (0xF)

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */

/*
 * Target specific SPM Hardware configuration.
 */
BSP_spm_ConfigDataType devcfgSpmBspData[] =
{
  {
    /* SawSecureCfg
     * ------------
     * sawCtl               pwrCtl                vltCtl */
    {  BSP_SPM_SAW_NONSECURE,  BSP_SPM_SAW_NONSECURE,   BSP_SPM_SAW_NONSECURE },
 
    /* SawCfg
     * ------
     * frcRefClkOn  clkDiv   */     
    {  0x0,         0x1F },

    /* SawSpmCtlCfg
     * ------------
     * inhibitStartAddrReset, wakeupConfig                */
    {  0x0                  , BSP_SPM_SAW_WAKEUP_CONFIG_0 },

    /* SawSpmDlyCfg
     * ------------
     * each field is 10 bits: max value of 1024
     * delay time = (delay_n * clkDiv)/19.2e6
     * delay_1, delay_2, delay_3 */
    {  300    , 275    , 1    },

    /* SawSpmPmicData[]
     * ----------
     *  pmicSel                      , pmicData, pmicDly, pmicDataSize, adrIdx */
    {
      { BSP_SPM_SAW_PMIC_DATA_NOMINAL, 0x80    , 0x0    , 0           , 3 },
      { BSP_SPM_SAW_PMIC_DATA_PC     , 0x00    , 0x0    , 0           , 3 },
      { BSP_SPM_SAW_PMIC_DATA_VDD_MIN, 0x00    , 0x0    , 0           , 0 },
    }
  }
}; /* SPM_BSP_DATA */


/* SPM command sequence for Modem QDSP6 PC without RPM Handshake */
static uint8 spmCmdSeqBHS[] =
{
  /* 0x3F, */  SPM_SEQ_DLY_CTL   (DLY3              ,0), /* Delay seq by DLY3  */
  /* 0x22, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_CLOCK_DIS            ,1), /* QDSP6 Clock Off    */
  /* 0x0B, */  SPM_SEQ_EVENT_CTL (EVENT_2                          ,0), /* WAIT for MSS PLL lock     */
  /* 0x12, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_CLAMP_IO             ,1), /* QDSP6 Assert Clamp IO */
  
  /*   4   */
  /* 0x12, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_RETAIN_IO            ,1), /* Assert Retain IO signal*/
  /* 0x12, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_RETAIN_IO            ,1), /* Toggle Retain IO signal*/
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_CUST_MEM_CLAMP       ,0), /* QDSP6 Memory Sleep */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_MEM_SOURCE_BIAS_EN   ,0), /* QDSP6 Memory Sleep */
  
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_MEM_RET_SLEEP        ,1), /* QDSP6 Memory Sleep */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_HSF                  ,1), /* QDSP6 Memory Sleep */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_COMP_MEM_CLAMP       ,0), /* QDSP6 Memory Sleep */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_LDO_BYPASS_OFF       ,0), /* QDSP6 Memory Sleep */
  
  /* 0x42, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_BHS                  ,0), /* QDSP6 BHS off            */
  /* 0x42, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_LDO_OFF              ,0), /* QDSP6BHS off            */
  /* 0x42, */  SPM_SEQ_PWR_CTL   (SPM_MSS_XO_EN                    ,0), /* QDSP6 LDO off  */
  /* 0xB0, */  SPM_SEQ_PWR_CTL   (SPM_MSS_PLL_OUT_DIS              ,0), /*  */
 
  /*   8   */
  /* 0x90, */  SPM_SEQ_PWR_CTL   (SPM_MSS_PLL_RESET                ,0), /* MSS Disable PLL Out*/
  /* 0x70, */  SPM_SEQ_PWR_CTL   (SPM_MSS_PLL_BYPASS                   ,0), /* MSS Reset PLL      */
  /* 0x80, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_PLL_FREEZE           ,0), /* MSS Bypass PLL     */
  /* 0x3,  */  SPM_SEQ_SLEEP_CTL (NO_RPM_HS         ,0), /*-SLEEP-NO-RPM-HS----*/
  
  /*   12   */
  /* 0x86, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_RESET                ,3), /* MSS Bypass PLL     */
  /* 0x70, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_PLL_FREEZE           ,0), /* Release MSS PLL frm Reset */  
  /* 0x70, */  SPM_SEQ_PWR_CTL   (SPM_MSS_PLL_BYPASS               ,0), /* MSS Reset PLL      */
  /* 0x90, */  SPM_SEQ_PWR_CTL   (SPM_MSS_PLL_RESET                ,0), /* MSS Disable PLL Out*/
  
  /* 0x0B, */  SPM_SEQ_EVENT_CTL (EVENT_0           ,0), /* WAIT for MSS PLL lock     */
  /* 0x90, */  SPM_SEQ_PWR_CTL   (SPM_MSS_PLL_OUT_DIS              ,0), /* Enable MSS PLL Outuput    */
  /* 0x42, */  SPM_SEQ_PWR_CTL   (SPM_MSS_XO_EN                    ,0), /* BHS off  */
  /* 0x42, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_LDO_OFF              ,0), /* QDSP6BHS off            */
  
  /* 0x42, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_BHS                   ,0), /* BHS off  */  
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_LDO_BYPASS_OFF       ,0), /* QDSP6 Memory Sleep */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_COMP_MEM_CLAMP       ,0), /* QDSP6 Memory Sleep */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_HSF                  ,1), /* QDSP6 Memory Sleep */
  
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_MEM_RET_SLEEP        ,1), /* QDSP6 Memory Sleep */  
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_MEM_SOURCE_BIAS_EN   ,0), /* QDSP6 Memory Sleep */  
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_CUST_MEM_CLAMP       ,0), /* QDSP6 Memory Sleep */  
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_RESTORE_IO           ,0), /* QDSP6 Memory Sleep */
															
  /* 0x32, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_CLAMP_IO             ,0), /* QDSP6 Memory Sleep */ 
  /* 0x86, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_RESET                ,3), /* MSS Bypass PLL     */
  /* 0x86, */  SPM_SEQ_PWR_CTL   (SPM_WAKEUP_IRQ                   ,3), /* MSS Bypass PLL     */
  /* 0x86, */  SPM_SEQ_PWR_CTL   (SPM_WAKEUP_IRQ                   ,3), /* MSS Bypass PLL     */
  
  /* 0x0B, */  SPM_SEQ_EVENT_CTL (EVENT_0                          ,0), /* WAIT for MSS PLL lock     */
  /* 0x22, */  SPM_SEQ_PWR_CTL   (SPM_CRPC_Q6_CLOCK_DIS            ,1), /* QDSP6 Clock Off    */ 
  /* 0x0F, */  SPM_SEQ_END_CTL   (0                 ,0), /* End of Program               */
};


/*                                                              
 * Array of SPM command sequences.
 */
BSP_spm_CmdSequenceType devcfgSpmCmdSeqArray[][SPM_NUM_LOW_POWER_MODES] =
{
  /* q6 has only one core */
  {
    { SPM_MODE_PWRC,
      spmCmdSeqBHS,
      sizeof(spmCmdSeqBHS),
      19, /* Index of the RPM handshake mode cmd in the command sequence array */
      -1, /* start index of this mode in SPM_SLP_SEQ; only valid if programmed */
    }
  }
};
