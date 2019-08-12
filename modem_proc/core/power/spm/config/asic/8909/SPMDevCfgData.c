/*===========================================================================

FILE:         BSPspm.c

DESCRIPTION:  
  This is the BSP data for the SPM driver.
  This data is for the 8916 MSS processor.

                Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/config/asic/8909/SPMDevCfgData.c#1 $

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
  QDSP6_RESET,               /* 0  */
  QDSP6_CLAMP,               /* 1  */
  QDSP6_CLOCK,               /* 2  */
  QDSP6_MEM_SLEEP,           /* 3  */
  QDSP6_BHS,                 /* 4  */
  QDSP6_LDO_BYPASS,          /* 5  */
  QDSP6_LDO,                 /* 6  */
  QDSP6_PLL_RESET,           /* 7  */
  QDSP6_PLL_BYPASS,          /* 8  */
  QDSP6_PLL_OUTPUT,          /* 9  */
  QDSP6_XO_CLOCK       = 14, /* 14 */
  QDSP6_LDO_RET_VREF,        /* 15 */
  QDSP6_CLAMP_WL,            /* 16 */
  QDSP6_COMP_MEM_CLAMP,      /* 17 */
  QDSP6_PLL_FREEZE,          /* 18 */
  QDSP6_MEM_SRC_BIAS,        /* 19 */
  QDSP6_RETAIN_SAVE,         /* 20 */
  QDSP6_RETAIN_RESTORE,      /* 21 */
  QDSP6_PMI,                 /* 22 */
  QDSP6_MEM_RET        = 30, /* 30 */
} SPM_SEQ_PWR_CTL_BITS;

/**
 * @brief Set of macros to create SPM command to toggle power control bits
 *        defined in @SPM_SEQ_PWR_CTL_BITS
 */
#define SPM_CMD_BIT7_4(bit)   (((bit) & 0xF) << 4)
#define SPM_CMD_BIT3(bit)     (((bit) & 0x10) >> 1)
#define SPM_CMD_BIT2_1(delay) ((delay) << 1)
#define SPM_SEQ_PWR_CTL(bit,delay) ( SPM_CMD_BIT7_4(bit)   | \
                                     SPM_CMD_BIT3(bit)     | \
                                     SPM_CMD_BIT2_1(delay) ) 

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
  /* 0x3F, */  SPM_SEQ_DLY_CTL   (DLY3,                     0), /* Delay seq by DLY3  */
  /* 0x20, */  SPM_SEQ_PWR_CTL   (QDSP6_CLOCK,              0), /* QDSP6 Clock Off    */
  /* 0x2B, */  SPM_SEQ_EVENT_CTL (EVENT_2,                  0), /* Wait for core clock off event */
  /* 0x10, */  SPM_SEQ_PWR_CTL   (QDSP6_CLAMP,              0), /* QDSP6 Assert Q6 Clamp */
  
  /*   4   */
  /* 0x48, */  SPM_SEQ_PWR_CTL   (QDSP6_RETAIN_SAVE,        0), /* Assert QDSP6 retain save*/
  /* 0x48, */  SPM_SEQ_PWR_CTL   (QDSP6_RETAIN_SAVE,        0), /* De-assert QDSP6 retain save*/
  /* 0x8, */   SPM_SEQ_PWR_CTL   (QDSP6_CLAMP_WL,           0), /* Assert Q6 clamp_wl */
  /* 0x38, */  SPM_SEQ_PWR_CTL   (QDSP6_MEM_SRC_BIAS,       0), /* Enable Q6 memory source biasing */
  
  /*   8   */
  /* 0xE8, */  SPM_SEQ_PWR_CTL   (QDSP6_MEM_RET,            0), /* Assert Q6 memory retention sleep */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (QDSP6_MEM_SLEEP,          1), /* Assert Q6 memory non-retention sleep */
  /* 0x18, */  SPM_SEQ_PWR_CTL   (QDSP6_COMP_MEM_CLAMP,     0), /* Assert Q6 QMC memory clamp */
  /* 0x50, */  SPM_SEQ_PWR_CTL   (QDSP6_LDO_BYPASS,         0), /* Force LDO bypass off  */
  
  /*   12   */
  /* 0x40, */  SPM_SEQ_PWR_CTL   (QDSP6_BHS,                0), /* Force QDSP6 BHS off            */
  /* 0x60, */  SPM_SEQ_PWR_CTL   (QDSP6_LDO,                0), /* Force LDO off            */
  /* 0xE0, */  SPM_SEQ_PWR_CTL   (QDSP6_XO_CLOCK,           0), /* MSS switch clock to XO  */
  /* 0x90, */  SPM_SEQ_PWR_CTL   (QDSP6_PLL_OUTPUT,         0), /* Disable Q6/MSS PLL outputs */
 
  /*   16   */
  /* 0x70, */  SPM_SEQ_PWR_CTL   (QDSP6_PLL_RESET,          0), /* Reset Q6/MSS PLL*/
  /* 0x80, */  SPM_SEQ_PWR_CTL   (QDSP6_PLL_BYPASS,         0), /* Bypass Q6/MSS PLL      */
  /* 0x28, */  SPM_SEQ_PWR_CTL   (QDSP6_PLL_FREEZE,         0), /* Freeze Q6 PLL     */
  /* 0x3,  */  SPM_SEQ_SLEEP_CTL (NO_RPM_HS,                0), /*-SLEEP-NO-RPM-HS----*/
  
  /*   20   */
  /* 0x0, */   SPM_SEQ_PWR_CTL   (QDSP6_RESET,              0), /* Assert Q6 reset     */
  /* 0x28, */  SPM_SEQ_PWR_CTL   (QDSP6_PLL_FREEZE,         0), /* Unfreeze Q6 PLL */  
  /* 0x84, */  SPM_SEQ_PWR_CTL   (QDSP6_PLL_BYPASS,         2), /* Disable Q6/MSS PLL bypass      */
  /* 0x70, */  SPM_SEQ_PWR_CTL   (QDSP6_PLL_RESET,          0), /* Release Q6/MSS PLL from reset */
  
  /*   24   */
  /* 0x90, */  SPM_SEQ_PWR_CTL   (QDSP6_PLL_OUTPUT,         0), /* Enable Q6/MSS PLL outputs    */
  /* 0x60, */  SPM_SEQ_PWR_CTL   (QDSP6_LDO,                0), /* De-assert LDO force off            */
  /* 0x42, */  SPM_SEQ_PWR_CTL   (QDSP6_BHS,                1), /* De-assert BHS force off  */  
  /* 0x50, */  SPM_SEQ_PWR_CTL   (QDSP6_LDO_BYPASS,         0), /* De-assert LDO bypass force off  */
  
  /*   28   */
  /* 0x18, */  SPM_SEQ_PWR_CTL   (QDSP6_COMP_MEM_CLAMP,     0), /* De-assert Q6 QMC memory clamp */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (QDSP6_MEM_SLEEP,          1), /* De-assert Q6 memory non-retention sleep */
  /* 0xEA, */  SPM_SEQ_PWR_CTL   (QDSP6_MEM_RET,            1), /* De-assert Q6 memory retention sleep */  
  /* 0x38, */  SPM_SEQ_PWR_CTL   (QDSP6_MEM_SRC_BIAS,       0), /* Disable Q6 memory source biasing */  
  
  /*   32   */
  /* 0x8, */  SPM_SEQ_PWR_CTL    (QDSP6_CLAMP_WL,           0), /* Assert Q6 clamp_wl */  
  /* 0x58, */  SPM_SEQ_PWR_CTL   (QDSP6_RETAIN_RESTORE,     0), /* Assert Q6 restore */
  /* 0x58, */  SPM_SEQ_PWR_CTL   (QDSP6_RETAIN_RESTORE,     0), /* De-assert Q6 restore */													
  /* 0x10, */  SPM_SEQ_PWR_CTL   (QDSP6_CLAMP,              0), /* De-assert Q6 clamp */ 
															
  /*   36   */
  /* 0x0, */  SPM_SEQ_PWR_CTL    (QDSP6_RESET,              0), /* De-assert Q6 reset   */
  /* 0x68, */  SPM_SEQ_PWR_CTL   (QDSP6_PMI,                0), /* Assert SPM wakeup IRQ     */
  /* 0x68, */  SPM_SEQ_PWR_CTL   (QDSP6_PMI,                0), /* De-assert SPM wakeup IRQ     */
  /* 0x0B, */  SPM_SEQ_EVENT_CTL (EVENT_0,                  0), /* Wait for Q6 PLL lock     */
  
  /*   40   */
  /* 0xE0, */  SPM_SEQ_PWR_CTL   (QDSP6_XO_CLOCK,           0), /* MSS switch back clock from XO */
  /* 0x20, */  SPM_SEQ_PWR_CTL   (QDSP6_CLOCK,              0), /* Q6 Clock on    */ 
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
