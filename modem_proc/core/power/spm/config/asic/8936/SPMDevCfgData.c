/*===========================================================================

FILE:         BSPspm.c

DESCRIPTION:  
  This is the BSP data for the SPM driver.
  This data is for the 8916 MSS processor.

                Copyright (c) 2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/config/asic/8936/SPMDevCfgData.c#1 $

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
  QDSP6_RESET,      /* 0 */
  QDSP6_CLAMP_IO,
  QDSP6_CLOCK,
  QDSP6_CLAMP_MEM,
  QDSP6_BHS,
  QDSP6_LDO_BYPASS, /* 5 */
  QDSP6_LDO,
  MSS_PLL_RESET,
  MSS_PLL_BYPASS,
  MSS_PLL_OUTPUT,
  MSS_RESET,        /* 10 */
  MSS_SPARE_1,
  MSS_CLAMP_IO,
  MSS_CLAMP_MEM,
  MSS_CLK_XO_SEL,
  QDSP6_LDO_VSEL,   /* 15 */
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
  /* 0x22, */  SPM_SEQ_PWR_CTL   (QDSP6_CLOCK       ,1), /* QDSP6 Clock Off    */
  /* 0xE0, */  SPM_SEQ_PWR_CTL   (MSS_CLK_XO_SEL    ,0), /* MSS Switch to XO */
  /* 0x12, */  SPM_SEQ_PWR_CTL   (QDSP6_CLAMP_IO    ,1), /* QDSP6 Assert Clamp IO */
  
  /*   4   */
  /* 0x32, */  SPM_SEQ_PWR_CTL   (QDSP6_CLAMP_MEM   ,1), /* QDSP6 Memory Sleep */
  /* 0x42, */  SPM_SEQ_PWR_CTL   (QDSP6_BHS         ,1), /* BHS off            */
  /* 0xB0, */  SPM_SEQ_PWR_CTL   (MSS_SPARE_1       ,0), /* Set MSS_MSA[FORCE_Q6_MSA] equal one */
  /* 0x90, */  SPM_SEQ_PWR_CTL   (MSS_PLL_OUTPUT    ,0), /* MSS Disable PLL Out*/
 
  /*   8   */
  /* 0x70, */  SPM_SEQ_PWR_CTL   (MSS_PLL_RESET     ,0), /* MSS Reset PLL      */
  /* 0x80, */  SPM_SEQ_PWR_CTL   (MSS_PLL_BYPASS    ,0), /* MSS Bypass PLL     */
  /* 0x3,  */  SPM_SEQ_SLEEP_CTL (NO_RPM_HS         ,0), /*-SLEEP-NO-RPM-HS----*/
  /* 0x86, */  SPM_SEQ_PWR_CTL   (MSS_PLL_BYPASS    ,3), /* MSS Bypass PLL     */
  
  /*   12   */
  /* 0x70, */  SPM_SEQ_PWR_CTL   (MSS_PLL_RESET     ,0), /* Release MSS PLL frm Reset */  
  /* 0x0B, */  SPM_SEQ_EVENT_CTL (EVENT_0           ,0), /* WAIT for MSS PLL lock     */
  /* 0x90, */  SPM_SEQ_PWR_CTL   (MSS_PLL_OUTPUT    ,0), /* Enable MSS PLL Outuput    */
  /* 0xB0, */  SPM_SEQ_PWR_CTL    (MSS_SPARE_1      ,0), /* Toggle back this control. Later Q6
	                                                    SW will clear MSS_MSA[FORCE_Q6_MSA]*/
															
  /*   16  */													    
  /* 0xE0, */  SPM_SEQ_PWR_CTL   (MSS_CLK_XO_SEL    ,0), /* MSS switch from XO        */
  /* 0x2,  */  SPM_SEQ_PWR_CTL   (QDSP6_RESET       ,1), /* QDSP6 Assert Reset */
  /* 0x42, */  SPM_SEQ_PWR_CTL   (QDSP6_BHS         ,1), /* BHS on */  
  /* 0x32, */  SPM_SEQ_PWR_CTL   (QDSP6_CLAMP_MEM   ,1), /* De-assert QDSP6 Memory Sleep */
  
  /*   20  */
  /* 0x12, */  SPM_SEQ_PWR_CTL   (QDSP6_CLAMP_IO    ,1), /* De-assert QDSP6 IO Clamps    */
  /* 0x02, */  SPM_SEQ_PWR_CTL   (QDSP6_RESET       ,1), /* De-assert QDSp6 Reset        */
  /* 0x22, */  SPM_SEQ_PWR_CTL   (QDSP6_CLOCK       ,1), /* Turn ON QDSP6 Clock          */
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
      10, /* Index of the RPM handshake mode cmd in the command sequence array */
      -1, /* start index of this mode in SPM_SLP_SEQ; only valid if programmed */
    }
  }
};
