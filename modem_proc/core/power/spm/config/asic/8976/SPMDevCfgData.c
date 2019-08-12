/*===========================================================================

FILE:         SPMDevCfgData.c

DESCRIPTION:  This is the BSP data for the SPM driver. This data is for the 
              8952 MSS processor.

              Copyright (c) 2013-2015 QUALCOMM Technologies Inc. (QTI)
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/config/asic/8976/SPMDevCfgData.c#1 $

============================================================================*/


/*==========================================================================
 *                            INCLUDES
 *==========================================================================*/
#include "BSPspm.h"

/*==========================================================================
 *                         INTERNAL TYPES
 *=========================================================================*/

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
  QDSP6_QMC_MEM_CLAMP,       /* 17 */
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

/**
 * @brief Enumeration to indicate bit for different PMIC data in 
 *        SPM command.
 */
typedef enum
{
  DATA_0,
  DATA_1,
  DATA_2,
} SPM_SEQ_PMIC_DATA_BITS;

/**
 * @brief Macro to create SPM command to send PMIC data based on 
 *        @SPM_SEQ_PMIC_DATA_BITS.
 */
#define SPM_SEQ_PMIC_CTL(bit,delay) (bit<<4)|(0x1)

/**
 * @brief Enumeration to indicate bit settings for RPM interaction
 *        during Sleep in SPM command.
 */
typedef enum
{
  NO_RPM_HS,
  RPM_HS
} SPM_SEQ_SLEEP_BITS;

/**
 * @brief Macro to set up SPM command for RPM interaction while waiting for
 *        wake up event.
 */
#define SPM_SEQ_SLEEP_CTL(bit,delay) (bit<<2)|(0x3)
  
/**
 * @brief Event bits for SPM command.
 */
typedef enum
{
  EVENT_0,
  EVENT_1,
  EVENT_2,
  EVENT_3
} SPM_SEQ_EVENT_BITS;

/**
 * @brief Macro to create SPM command to wait for events defined in 
 *        @SPM_SEQ_EVENT_BITS.
 */
#define SPM_SEQ_EVENT_CTL(bit,delay) (bit<<4)|(0xB)

typedef enum
{
  DLY1 = 1,
  DLY2 = 2,
  DLY3 = 3
} SPM_SEQ_DLY_BITS;

#define SPM_SEQ_DLY_CTL(bit,nop) (bit<<4)|(0xF)

#define SPM_SEQ_END_CTL(bit,delay) (0xF)

/**
 * @brief Macro to wait for PMIC to restore modem power rail
 */
 #define SPM_SEQ_WAIT_MSS_PWR_RAIL(bit,delay)  (0x1F)
 
/*==========================================================================
 *                                   NOTE
 *=========================================================================*/

/**
 * Even though different low power modes share some commands, for some 
 * of them actual impact at the HW level will be determined by special control
 * registers values (e.g. SPMCTL) which are programmed as part of enter 
 * functions of low power modes. Below is the list of such known 
 * commands/toggles.
 *
 * QDSP6_RESET
 * -----------
 * Depending on how SPMCTL_EN_ARES register is programed, this command may 
 * reset core differently (partially or full).
 * 
 * QDSP6_RETAIN_SAVE
 * -----------------
 * If SAVE field in SPMCTL_EN_STATERET is programed zero, this command/toggle 
 * may not take impact. (For example in Power collapse - but we removed this 
 * toggle itself from sequence).
 *
 * QDSP6_RETAIN_RESTORE 
 * --------------------
 * Same as QDSP6_RETAIN_SAVE but with RESTORE field.
 *
 * QDSP6_PMI
 * ---------
 * Same as QDSP6_RETAIN_SAVE but with WAKEUP_IRQ field.
 */
 
/*==========================================================================
 *                                   DATA
 *=========================================================================*/

/**
 * @brief Target specific SPM Hardware configuration.
 */
BSP_spm_ConfigDataType devcfgSpmBspData[] =
{
  {
    /* Saw2SecureCfg
     * ------------
     * sawCtl               pwrCtl                vltCtl */
    {  BSP_SPM_SAW_NONSECURE,  BSP_SPM_SAW_NONSECURE,   BSP_SPM_SAW_NONSECURE },
 
    /* Saw2Cfg
     * ------
     * frcRefClkOn  clkDiv   */     
    {  0x0,         0x1F },

    /* Saw2SpmCtlCfg
     * ------------
     * inhibitStartAddrReset wakeupConfig                */
    {  0x1                  , BSP_SPM_SAW_WAKEUP_CONFIG_0 },

    /* Saw2SpmDlyType
     * -------------
     * each field is 10 bits: max value of 1024
     * delay time = (delay_n * clkDiv)/19.2e6
     * delay_1, delay_2, delay_3 */
    {  300    , 275    , 1    },

    /* Saw2SpmPmicData[]
     * ----------
     *  pmicSel                      , pmicData, pmicDly, pmicDataSize, adrIdx */
    {
      { BSP_SPM_SAW_PMIC_DATA_NOMINAL, 0x80    , 0x0    , 0           , 3 },
      { BSP_SPM_SAW_PMIC_DATA_PC     , 0x00    , 0x0    , 0           , 3 },
      { BSP_SPM_SAW_PMIC_DATA_VDD_MIN, 0x00    , 0x0    , 0           , 0 },
    }
  }
}; /* SPM_BSP_DATA */

/**
 * @brief SPM command sequence for MSS APCR with PLL on 
 *  
 *        Corresponds to APCR short sequence from HPG. Table 4-5.
 */
static uint8 spmCmdSeqAPCRPLLOn[] =
{ 
  /* index 0 */
  /* 0x20 */  SPM_SEQ_PWR_CTL( QDSP6_CLOCK,         0 ), /* QDSP6 Clock Off */
  /* 0x10 */  SPM_SEQ_PWR_CTL( QDSP6_CLAMP,         0 ), /* QDSP6 Assert Clamp */
  /* 0x48 */  SPM_SEQ_PWR_CTL( QDSP6_RETAIN_SAVE,   0 ), /* Assert QDSP6 retain save*/
  /* 0x48 */  SPM_SEQ_PWR_CTL( QDSP6_RETAIN_SAVE,   0 ), /* Deassert QDSP6 retain save */

  /* index 4 */
  /* 0x8  */  SPM_SEQ_PWR_CTL( QDSP6_CLAMP_WL,      0 ), /* QDSP6 Assert Clamp WL */
  /* 0x30 */  SPM_SEQ_PWR_CTL( QDSP6_MEM_SLEEP,     0 ), /* QDSP6 Assert Mem Sleep */
  /* 0x18 */  SPM_SEQ_PWR_CTL( QDSP6_QMC_MEM_CLAMP, 0 ), /* QDSP6 QMC Mem Clamp */
  /* 0x50 */  SPM_SEQ_PWR_CTL( QDSP6_LDO_BYPASS,    0 ), /* Force LDO Bypass Off */

  /* index 8 */
  /* 0x42 */  SPM_SEQ_PWR_CTL( QDSP6_BHS,           1 ), /* Force BHS Off */
  /* 0x60 */  SPM_SEQ_PWR_CTL( QDSP6_LDO,           0 ), /* Force LDO Off */
  /* 0x3  */  SPM_SEQ_SLEEP_CTL( NO_RPM_HS,         0 ), /* Init Sleep to NO RPM HS */
  /* 0x0  */  SPM_SEQ_PWR_CTL( QDSP6_RESET,         0 ), /* Assert Reset */

  /* index 12 */
  /* 0x60 */  SPM_SEQ_PWR_CTL( QDSP6_LDO,           0 ), /* Deassert LDO Off */
  /* 0x42 */  SPM_SEQ_PWR_CTL( QDSP6_BHS,           1 ), /* Deassert BHS Off */
  /* 0x50 */  SPM_SEQ_PWR_CTL( QDSP6_LDO_BYPASS,    0 ), /* Deassert LDO Bypass Off */
  /* 0x18 */  SPM_SEQ_PWR_CTL( QDSP6_QMC_MEM_CLAMP, 0 ), /* Deassert QMC Mem Clamp */

  /* index 16 */
  /* 0x32 */  SPM_SEQ_PWR_CTL( QDSP6_MEM_SLEEP,     1 ), /* Deassert QDSP6 Mem Sleep */
  /* 0x8  */  SPM_SEQ_PWR_CTL( QDSP6_CLAMP_WL,      0 ), /* Deassert QDSP6 Clamp WL */
  /* 0x58 */  SPM_SEQ_PWR_CTL( QDSP6_RETAIN_RESTORE,0 ), /* Assert QDSP6 retain restore */
  /* 0x58 */  SPM_SEQ_PWR_CTL( QDSP6_RETAIN_RESTORE,0 ), /* Deassert QDSP6 retain restore */

  /* index 20 */
  /* 0x10 */  SPM_SEQ_PWR_CTL( QDSP6_CLAMP,         0 ), /* Deassert QDSP6 Clamp */
  /* 0x0  */  SPM_SEQ_PWR_CTL( QDSP6_RESET,         0 ), /* Deassert Reset */
  /* 0x68 */  SPM_SEQ_PWR_CTL( QDSP6_PMI,           0 ), /* Assert Power Manager Interrupt (PMI) */
  /* 0x68 */  SPM_SEQ_PWR_CTL( QDSP6_PMI,           0 ), /* Deassert PMI */

  /* index 24 */
  /* 0x20 */  SPM_SEQ_PWR_CTL( QDSP6_CLOCK,         0 ), /* Deassert QDSP6 Clock Off */
  /* 0xF  */  SPM_SEQ_END_CTL( 0,                   0 ), /* End of Program */
};

/**
 * @brief SPM command sequence for MSS APCR PLL in low power 
 *        mode.
 *
 *        Hybrid APCR sequence that include PLL control command,
 *        but does not include any PMIC control commands.
 */
static uint8 spmCmdSeqAPCRPLLLPM[] =
{
  /* index 0 */
  /* 0x20 */ SPM_SEQ_PWR_CTL( QDSP6_CLOCK,         0 ), /* QDSP6 Clock Off */
  /* 0x10 */ SPM_SEQ_PWR_CTL( QDSP6_CLAMP,         0 ), /* QDSP6 Assert Clamp */
  /* 0x48 */ SPM_SEQ_PWR_CTL( QDSP6_RETAIN_SAVE,   0 ), /* Assert QDSP6 retain save*/
  /* 0x48 */ SPM_SEQ_PWR_CTL( QDSP6_RETAIN_SAVE,   0 ), /* Deassert QDSP6 retain save */

  /* index 4 */
  /* 0x8  */ SPM_SEQ_PWR_CTL( QDSP6_CLAMP_WL,      0 ), /* QDSP6 Assert Clamp WL */
  /* 0x30 */ SPM_SEQ_PWR_CTL( QDSP6_MEM_SLEEP,     0 ), /* QDSP6 Assert Mem Sleep */
  /* 0x18 */ SPM_SEQ_PWR_CTL( QDSP6_QMC_MEM_CLAMP, 0 ), /* QDSP6 QMC Mem Clamp */
  /* 0x50 */ SPM_SEQ_PWR_CTL( QDSP6_LDO_BYPASS,    0 ), /* Force LDO Bypass Off */

  /* index 8 */
  /* 0x42 */ SPM_SEQ_PWR_CTL( QDSP6_BHS,           1 ), /* Force BHS Off */
  /* 0x60 */ SPM_SEQ_PWR_CTL( QDSP6_LDO,           0 ), /* Force LDO Off */
  /* 0xE0 */ SPM_SEQ_PWR_CTL( QDSP6_XO_CLOCK,      0 ), /* MSS switch clock to XO */
  /* 0x90 */ SPM_SEQ_PWR_CTL( QDSP6_PLL_OUTPUT,    0 ), /* Disable QDSP6 PLL outputs */

  /* index 12 */
  /* 0x28 */ SPM_SEQ_PWR_CTL( QDSP6_PLL_FREEZE,    0 ), /* Freeze QDSP6 PLL */
  /* 0x3  */ SPM_SEQ_SLEEP_CTL( NO_RPM_HS,         0 ), /* Sleep w/o RPM handshake */
  /* 0x0  */ SPM_SEQ_PWR_CTL( QDSP6_RESET,         0 ), /* Assert QDSP6 reset */
  /* 0x28 */ SPM_SEQ_PWR_CTL( QDSP6_PLL_FREEZE,    4 ), /* Unfreeze QDSP6 PLL */

  /* index 16 */
  /* 0x90 */ SPM_SEQ_PWR_CTL( QDSP6_PLL_OUTPUT,    0 ), /* Enable QDSP6 PLL outputs */
  /* 0xE0 */ SPM_SEQ_PWR_CTL( QDSP6_XO_CLOCK,      0 ), /* MSS switch back clock from XO */
  /* 0x60 */ SPM_SEQ_PWR_CTL( QDSP6_LDO,           0 ), /* Deassert LDO force off */
  /* 0x42 */ SPM_SEQ_PWR_CTL( QDSP6_BHS,           1 ), /* Deassert BHS off, 1 us min delay is req'd */

  /* index 20 */
  /* 0x50 */ SPM_SEQ_PWR_CTL( QDSP6_LDO_BYPASS,    0 ), /* Deassert LDO bypass force off */
  /* 0x18 */ SPM_SEQ_PWR_CTL( QDSP6_QMC_MEM_CLAMP, 0 ), /* Deassert QDSP6 QMC memory clamp */
  /* 0x30 */ SPM_SEQ_PWR_CTL( QDSP6_MEM_SLEEP,     0 ), /* Deassert QDSP6 memory sleep + pipeline delay to assert L2 */
  /* 0x8  */ SPM_SEQ_PWR_CTL( QDSP6_CLAMP_WL,      0 ), /* Assert QDSP6 clamp_wl */  
  
  /* index 24 */
  /* 0x58 */ SPM_SEQ_PWR_CTL( QDSP6_RETAIN_RESTORE,0 ), /* Assert QDSP6 restore */
  /* 0x58 */ SPM_SEQ_PWR_CTL( QDSP6_RETAIN_RESTORE,0 ), /* De-assert QDSP6 restore */
  /* 0x10 */ SPM_SEQ_PWR_CTL( QDSP6_CLAMP,         0 ), /* Deassert QDSP6 clamp */
  /* 0x0  */ SPM_SEQ_PWR_CTL( QDSP6_RESET,         0 ), /* Deassert QDSP6 reset */
          
  /* index 28 */
  /* 0x68 */ SPM_SEQ_PWR_CTL( QDSP6_PMI,           0 ), /* Assert SPM wakeup IRQ */
  /* 0x68 */ SPM_SEQ_PWR_CTL( QDSP6_PMI,           0 ), /* De-assert SPM wakeup IRQ */
  /* 0x20 */ SPM_SEQ_PWR_CTL( QDSP6_CLOCK,         0 ), /* QDSP6 Clock on */
  /* 0xF  */ SPM_SEQ_END_CTL( 0,                   0 )  /* End of command sequence */
};

/**
 * @brief SPM command sequence for MSS Power Collapse.
 *
 *        From Table 4-7 of QDSP6SSv56 HPG. APCR Long Sequence
 */
static uint8 spmCmdSeqPC[] =
{
  /* index 0 */
  /* 0x20 */  SPM_SEQ_PWR_CTL( QDSP6_CLOCK,         0 ), /* QDSP6 Clock Off */
  /* 0x2B */  SPM_SEQ_EVENT_CTL( EVENT_2,           0 ), /* Wait for clock off event */
  /* 0x10 */  SPM_SEQ_PWR_CTL( QDSP6_CLAMP,         0 ), /* Assert QDSP6 clamp */
  /* 0x48 */  SPM_SEQ_PWR_CTL( QDSP6_RETAIN_SAVE,   0 ), /* Assert QDSP6 retain save*/

  /* index 4 */
  /* 0x48 */  SPM_SEQ_PWR_CTL( QDSP6_RETAIN_SAVE,   0 ), /* Deassert QDSP6 retain save */
  /* 0x8  */  SPM_SEQ_PWR_CTL( QDSP6_CLAMP_WL,      0 ), /* Assert QDSP6 Clamp WL */
  /* 0x38 */  SPM_SEQ_PWR_CTL( QDSP6_MEM_SRC_BIAS,  0 ), /* Assert memory biasing */
  /* 0xE8 */  SPM_SEQ_PWR_CTL( QDSP6_MEM_RET,       0 ), /* Assert memory ret sleep */
  
  /* index 8 */
  /* 0x32 */  SPM_SEQ_PWR_CTL( QDSP6_MEM_SLEEP,     1 ), /* Assert memory non-ret sleep */
  /* 0x18 */  SPM_SEQ_PWR_CTL( QDSP6_QMC_MEM_CLAMP, 0 ), /* Assert QDSP6 memory clamp */
  /* 0x50 */  SPM_SEQ_PWR_CTL( QDSP6_LDO_BYPASS,    0 ), /* Assert LDO bypass */
  /* 0x40 */  SPM_SEQ_PWR_CTL( QDSP6_BHS,           0 ), /* Assert BHS off */
  
  /* index 12 */
  /* 0x60 */  SPM_SEQ_PWR_CTL( QDSP6_LDO,           0 ), /* Assert LDO off */
  /* 0xE0 */  SPM_SEQ_PWR_CTL( QDSP6_XO_CLOCK,      0 ), /* MSS switch clock to XO */
  /* 0x90 */  SPM_SEQ_PWR_CTL( QDSP6_PLL_OUTPUT,    0 ), /* Disable QDSP6 PLL outputs */
  /* 0x70 */  SPM_SEQ_PWR_CTL( QDSP6_PLL_RESET,     0 ), /* Reset QDSP6 PLL  */
  
  /* index 16 */
  /* 0x80 */  SPM_SEQ_PWR_CTL( QDSP6_PLL_BYPASS,    0 ), /* Bypass QDSP6 PLL*/
  /* 0x28 */  SPM_SEQ_PWR_CTL( QDSP6_PLL_FREEZE,    0 ), /* Freeze QDSP6 PLL */
  /* 0x1  */  SPM_SEQ_PMIC_CTL( DATA_0,             0 ), /* PMIC to collapse modem power rail */
  /* 0x3  */  SPM_SEQ_SLEEP_CTL( NO_RPM_HS,         0 ), /* Sleep w/o RPM handshake */
  
  /* index 20 */
  /* 0x0  */  SPM_SEQ_PWR_CTL( QDSP6_RESET,         0 ), /* Assert QDSP6 reset */
  /* 0x11 */  SPM_SEQ_PMIC_CTL( DATA_1,             0 ), /* PMIC to restore modem power rail */
  /* 0x1F */  SPM_SEQ_WAIT_MSS_PWR_RAIL( 0,         0 ), /* Wait for PMIC to restore modem power rail */
  /* 0x28 */  SPM_SEQ_PWR_CTL( QDSP6_PLL_FREEZE,    0 ), /* Unfreeze QDSP6 PLL */
  
  /* index 24 */
  /* 0x88 */  SPM_SEQ_PWR_CTL( QDSP6_PLL_BYPASS,    4 ), /* Disable QDSP6 PLL bypass*/
  /* 0x70 */  SPM_SEQ_PWR_CTL( QDSP6_PLL_RESET,     0 ), /* Release QDSP6 PLL from reset */
  /* 0x90 */  SPM_SEQ_PWR_CTL( QDSP6_PLL_OUTPUT,    0 ), /* Enable QDSP6 PLL outputs */
  /* 0x60 */  SPM_SEQ_PWR_CTL( QDSP6_LDO,           0 ), /* Deassert LDO force off */
  
  /* index 28 */
  /* 0x42 */  SPM_SEQ_PWR_CTL( QDSP6_BHS,           1 ), /* Deassert BHS off, 1 us min delay is req'd */  
  /* 0x50 */  SPM_SEQ_PWR_CTL( QDSP6_LDO_BYPASS,    0 ), /* Deassert LDO bypass force off */
  /* 0x18 */  SPM_SEQ_PWR_CTL( QDSP6_QMC_MEM_CLAMP, 0 ), /* Deassert QDSP6 QMC memory clamp */
  /* 0x32 */  SPM_SEQ_PWR_CTL( QDSP6_MEM_SLEEP,     1 ), /* Deassert QDSP6 memory non-ret sleep */

  /* index 32 */
  /* 0xEA */  SPM_SEQ_PWR_CTL( QDSP6_MEM_RET,       1 ), /* Assert memory ret sleep */  
  /* 0x38 */  SPM_SEQ_PWR_CTL( QDSP6_MEM_SRC_BIAS,  0 ), /* Disable QDSP6 memory sorce biasing */
  /* 0x8  */  SPM_SEQ_PWR_CTL( QDSP6_CLAMP_WL,      0 ), /* Assert QDSP6 clamp_wl */  
  /* 0x58 */  SPM_SEQ_PWR_CTL( QDSP6_RETAIN_RESTORE,0 ), /* Assert QDSP6 restore */

  /* index 36 */
  /* 0x58 */  SPM_SEQ_PWR_CTL( QDSP6_RETAIN_RESTORE,0 ), /* De-assert QDSP6 restore */  
  /* 0x10 */  SPM_SEQ_PWR_CTL( QDSP6_CLAMP,         0 ), /* Deassert QDSP6 clamp */
  /* 0x0  */  SPM_SEQ_PWR_CTL( QDSP6_RESET,         0 ), /* Deassert QDSP6 reset */
  /* 0x68 */  SPM_SEQ_PWR_CTL( QDSP6_PMI,           0 ), /* Assert SPM wakeup IRQ */

  /* index 40 */
  /* 0x68 */  SPM_SEQ_PWR_CTL( QDSP6_PMI,           0 ), /* De-assert SPM wakeup IRQ */  
  /* 0xB  */  SPM_SEQ_EVENT_CTL( EVENT_0,           0 ), /* Wait for QDSP6 PLL lock */
  /* 0xE0 */  SPM_SEQ_PWR_CTL( QDSP6_XO_CLOCK,      0 ), /* MSS switch back clock from XO */
  /* 0x20 */  SPM_SEQ_PWR_CTL( QDSP6_CLOCK,         0 ), /* QDSP6 Clock on */
  
  /* index 44 */
 /* 0xF */    SPM_SEQ_END_CTL( 0,                   0 )  /* End of command sequence */
};

/**
 * @brief Array containing SPM command sequences for supported low power
 *        modes and associated information.
 */
BSP_spm_CmdSequenceType devcfgSpmCmdSeqArray[][SPM_NUM_LOW_POWER_MODES] =
{
  /* MSS has only one core */
  {
    { 
      SPM_MODE_APCR_PLL_ON,
      spmCmdSeqAPCRPLLOn,
      sizeof(spmCmdSeqAPCRPLLOn), 
      10, /* Index of the RPM handshake cmd in the command sequence array */
      -1
    },
    {
      SPM_MODE_APCR_PLL_LPM,
      spmCmdSeqAPCRPLLLPM,
      sizeof(spmCmdSeqAPCRPLLLPM),
      13, /* Index of the RPM handshake cmd in the command sequence array */
      -1
    },
    {
      SPM_MODE_PWRC,
      spmCmdSeqPC,
      sizeof(spmCmdSeqPC),
      19, /* Index of the RPM handshake cmd in the command sequence array */
      -1
    }
  }
};
