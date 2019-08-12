/*==============================================================================
  FILE:         HALspmPlatform.c

  OVERVIEW:     This is the generic hardware abstraction layer implementation
                for the SPMv2 (SAW2) Hardware.

  DEPENDENCIES: None

                Copyright (c) 2010-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
==============================================================================

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/src/hal/HALspmPlatform.c#1 $

=============================================================================*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include <stdlib.h>
#include <string.h>
#include "BSPspm.h"
#include "HALspm.h"
#include "HALspmPlatform.h"
#include "CoreVerify.h"
#include "spm.h"
#include "spm_common.h"

/* -----------------------------------------------------------------------
**                            FORWARD DECLARATION
** ----------------------------------------------------------------------- */
void spm_hwio_mmap(void);

/* -----------------------------------------------------------------------
**                           INTERNAL MACROS
** ----------------------------------------------------------------------- */

/* Identifier string for this HAL */
#define HAL_SPM_VERSION_STRING "HAL_SPM_V2"

/* Macro that will be used for memory/resource allocation for programming
 * command sequences. The value is an empirical value. However, it is not 
 * a hard limit. If underlying SPM HW can accommodate more sequences, we 
 * will increment resource allocation by this value. */
#define HAL_SPM_BASE_PRG_CMD_SEQ_LIMIT 8

/* -----------------------------------------------------------------------
**                           INTERNAL VARIABLES
** ----------------------------------------------------------------------- */

/* Consolidated data */
HAL_spm_PlatformType spmPlatform = 
{
  /* Placeholder for BSP config data.*/
  NULL,
  /* Placeholders for some custom functions (unused).*/
  NULL,
  NULL,
  NULL
};

/**
 * @brief Pointer to function for handling rpm handshake/bypass mechanism on 
 *        SPM
 *
 * @param coreNum   : Which SPM
 * @param rpmBypass : Enum to indicate if rpm handshake should be performed or
 *                    skipped.
 */
void (*g_rpmHandshakeFcn)(uint32 coreNum, spm_rpm_bypass_type rpmBypass);

/* Array containing information about already programmed command sequences on
 * each spm. */
static HAL_spm_ProgrammedSeqInfoType *prgCmdSeqInfo = NULL;

/* -----------------------------------------------------------------------
**                        INTERNAL FUNCTIONS
** ----------------------------------------------------------------------- */

/**
 * @brief Configures the PMIC data for a given PMIC mode.
 * 
 * @param coreNum  : Which SPM.
 * @param pmicMode : Mode for which we are configuring PMIC data.
 * @param pmicData : Actual data for above mode.
 */
static void HAL_spm_ConfigPMICData
( 
  uint32 coreNum,
  BSP_spm_PmicModeData pmicMode,
  uint8 pmicData
)
{
  SPM_HWIO_OUTFI_W( coreNum, SPM_PMIC_DATA_n, pmicMode, DATA, pmicData);
}

/**
 * @brief Configures the pmic delay for a given PMIC mode.
 * 
 * @param coreNum  : Which SPM.
 * @param pmicMode : Mode for which we are configuring PMIC delay.
 * @param pmicData : Actual delay for above mode.
 */
static void HAL_spm_ConfigPMICDelay
( 
  uint32 coreNum,
  BSP_spm_PmicModeData pmicMode,
  uint8 pmicDly
)
{
  SPM_HWIO_OUTFI_W( coreNum, SPM_PMIC_DATA_n, pmicMode, DLY, pmicDly);
}

/**
 * @brief This function writes the sequence to SPM HW registers.
 *
 * @note 
 * - This is an internal function and contains simply copying logic. All the
 *   verification must have been done by caller. It cannot fail for SW reasons.
 * - This function is not threadsafe for a given core.
 *
 * @param coreNum     : Which SPM.
 * @param seq         : Command sequence to program
 * @param seqLength   : Length of above command sequence.
 * @param writeOffset : Offset in bytes at which copying will begin in
 *                      sequence registers.
 */
static void HAL_spm_CopyCmdSeq
(
  uint32                  coreNum,
  spm_cmd_t *seq, 
  uint32 seqLength, 
  uint32 writeOffset
)
{
  uint32 i = 0;                  /* Index to the command sequence block */
  uint32 tmask = 0;              /* Register mask for a given command */
  uint32 mask = 0;               /* Mask when we actually write to register. It
                                    is 'OR'd value of multiple command masks */
  uint32 cmdShift;               /* Shift for the last command */
  uint32 val = 0;                /* Value that will be written to register. It
                                    is an 'OR'd value of commands */
  uint8 anyCmdRead = 0;          /* Indicates if we started reading cmd seq */

  /* Start copying command sequence to spm registers */
  for( i = 0; i < seqLength; i++, writeOffset++ )
  {
    switch(writeOffset % 4)
    {
      case 0:
        /* We reached 4-byte register boundary. Write to actual register if we
         * had copied any command */
        if( 0 != anyCmdRead )
        {
          SPM_HWIO_OUTMI_W( coreNum, SPM_SLP_SEQ_ENTRY_n, 
                            ((writeOffset -1) >> 2), mask, val );
          mask = val = 0;
        }

        tmask = SPM_HWIO_BMSK_W( coreNum, SPM_SLP_SEQ_ENTRY_n, CMD0);
        cmdShift = SPM_HWIO_SHFT_W( coreNum, SPM_SLP_SEQ_ENTRY_n, CMD0);
        break;

      case 1:
        tmask = SPM_HWIO_BMSK_W( coreNum, SPM_SLP_SEQ_ENTRY_n, CMD1);
        cmdShift = SPM_HWIO_SHFT_W( coreNum, SPM_SLP_SEQ_ENTRY_n, CMD1);
        break;

      case 2:
        tmask = SPM_HWIO_BMSK_W( coreNum, SPM_SLP_SEQ_ENTRY_n, CMD2);
        cmdShift = SPM_HWIO_SHFT_W( coreNum, SPM_SLP_SEQ_ENTRY_n, CMD2);
        break;

      case 3:
        tmask = SPM_HWIO_BMSK_W( coreNum, SPM_SLP_SEQ_ENTRY_n, CMD3);
        cmdShift = SPM_HWIO_SHFT_W( coreNum, SPM_SLP_SEQ_ENTRY_n, CMD3);
        break;
    }
    
    mask |= tmask;
    val  |= (seq[i] << cmdShift);

    /* We have read at least one command */
    anyCmdRead = 1;
  }

  SPM_HWIO_OUTMI_W( coreNum, SPM_SLP_SEQ_ENTRY_n, 
                    ((writeOffset -1) >> 2), mask, val);

}

/**
 * @brief Updates a command in SPM HW
 *
 * @param coreNum    : Which SPM
 * @param cmdOffset  : Offset of command within register bank. It is sum of
 *                     start address of the sequence and command's position
 *                     within sequence
 * @param updatedCmd : Updated command which will be written to HW.
 */
static void HAL_spm_UpdateCmdInHW
(
  uint32 coreNum,
  uint32 cmdOffset,
  spm_cmd_t updatedCmd
)
{
  uint32 regIndex = cmdOffset / SPM_HAL_COMMANDS_PER_REGISTER;

  switch ( cmdOffset % SPM_HAL_COMMANDS_PER_REGISTER )
  {
    case 0:
      SPM_HWIO_OUTFI_W( coreNum, SPM_SLP_SEQ_ENTRY_n, regIndex, CMD0, updatedCmd );
      break;

    case 1:
      SPM_HWIO_OUTFI_W( coreNum, SPM_SLP_SEQ_ENTRY_n, regIndex, CMD1, updatedCmd );
      break;

    case 2:
      SPM_HWIO_OUTFI_W( coreNum, SPM_SLP_SEQ_ENTRY_n, regIndex, CMD2, updatedCmd );
      break;

    case 3:
      SPM_HWIO_OUTFI_W( coreNum, SPM_SLP_SEQ_ENTRY_n, regIndex, CMD3, updatedCmd );
      break;

    default:
      /* To catch an error in case number of commands per register increases */
      CORE_VERIFY(0);
  }
}

/**
 * @brief Prepares the SPM for performing sequence of the input low power 
 *        mode.
 *
 * @param coreNum : Which SPM.
 * @param seqData : Sequence data for the mode which being prograqmmed.
 */
static void HAL_spm_ConfigLowPowerMode
( 
  uint32 coreNum,
  BSP_spm_CmdSequenceType *seqData
)
{
  /* Set SPM_START_ADR for mode */
  SPM_HWIO_OUTF_W( coreNum, SPM_CTL, SPM_START_ADR, seqData->seqStartIndex );

  switch(seqData->mode)
  {
    case SPM_MODE_CLK_GATE:
    case SPM_MODE_L2_RETENTION:
    case SPM_MODE_L2_GDHS:
    case SPM_MODE_L2_OFF:
      break;

    case SPM_MODE_VDD_MIN:
      /* Update wake-up voltage to current voltage */
      HAL_spm_UpdateActiveVoltageLevel(coreNum);
      break;

    case SPM_MODE_PWRC:
    case SPM_MODE_PWRC_BHS:
    case SPM_MODE_APCR_PLL_ON:
    case SPM_MODE_APCR_PLL_LPM:
    case SPM_MODE_APCR_RAIL_LPM:
      /* Q6 HPG dictates SYS_PC_MODE = 1 */
      SPM_HWIO_OUTF_W( coreNum, SPM_CTL, SPM_SYS_PC_MODE, 0x1 );
      break;

    default:
      ERR_FATAL("HAL_spm_SetLowPowerMode: Low Power Mode %d. Not supported",
                seqData->mode, 0, 0);
      break;
  }    
  
}
  
/**
 * @brief Downloads _all_ the power control sequences necessary for the
 * different sleep modes. 
 * 
 * @param coreNum : Which SPM.
 *
 * @dependencies SPM early initialization must be completed successfully.
 */
static void HAL_spm_DwnldAllSlpSeqEntry( uint32 coreNum )
{
  uint32 i;
  uint32       startAddr = 0;
  spm_result_t result = SPM_ERROR;
  uint32 nmodes = BSP_spm_GetNumSupportedLPMs(coreNum);

  for (i = 0 ; i < nmodes ; i++)
  {
    /* Skip this mode if it's invalid */
    if ( 0 == gSpmCmdSeqArray[coreNum][i].len )
    {
      continue;
    }

    result = HAL_spm_ProgramCmdSeq( coreNum, 
                                    gSpmCmdSeqArray[coreNum][i].sequence,
                                    gSpmCmdSeqArray[coreNum][i].len,
                                    &startAddr );
    
    CORE_VERIFY( SPM_SUCCESS == result );

    /* Update BSP Data with start offset of SLP_SEQ */
    gSpmCmdSeqArray[coreNum][i].seqStartIndex = startAddr;
  }
}

/**
 * @brief Returns the relative offset for the requested mode in the 
 * SPM_SLQ_SEQ_ENTRY_n HW register array.
 * 
 * @param coreNum    : Which SPM.
 * @param mode       : Power mode for which the start address is required.
 * @param pStartAddr : Command start address will be returned here
 *                        (only valid if return value is TRUE).
 * 
 * @return TRUE if a command sequence is supported for the requested mode.
 */
static bool32 HAL_spm_GetLPMSequenceData
(
  uint32       coreNum,
  unsigned int mode,
  BSP_spm_CmdSequenceType** seqData
)
{
  uint32 i;
  bool32 status = FALSE;
  uint32 nmodes = BSP_spm_GetNumSupportedLPMs(coreNum);

  /* Make sure we are looking for a valid mode */
  CORE_VERIFY( mode < SPM_NUM_LOW_POWER_MODES );
  
  /* Search the array of modes for the one passed in.  Keep track of
   * the length of command sequences that are passed along the way, in
   * order to determine the start address of this mode. */

  for ( i = 0; i < nmodes; i++ )
  {
    if ( gSpmCmdSeqArray[coreNum][i].mode == mode )
    {
      *seqData = &gSpmCmdSeqArray[coreNum][i];
      status = TRUE; 
      break;
    }
  }

  return status;
}

/**
 * @brief Updates LPM sequence so that SPM can perform rpm handshake
 *        or skip it during processor idle. 
 *
 * @param coreNum          : Which SPM
 * @param rpmHandshakeMode : Enum indicating if rpm handshake is requied
 *                           or can be bypassed.
 */
static void HALSPM_setRPMHandshakeModeIndex
( 
  uint32 coreNum,
  spm_rpm_bypass_type rpmHandshakeMode
)
{
  BSP_spm_CmdSequenceType * seqData;
  uint32 i;
  uint32 rpmHwRegByteOffset;
  uint32 nmodes = BSP_spm_GetNumSupportedLPMs(coreNum);
  uint8  newRpmMode = (SPM_RPM_BYPASS_HANDSHAKE == rpmHandshakeMode) ?
    BSP_SPM_CMD_SLEEP_NO_RPM_HANDSHAKE : BSP_SPM_CMD_SLEEP_RPM_HANDSHAKE;
  HAL_spm_ProgrammedSeqInfoType *seqInfo = &prgCmdSeqInfo[coreNum];

  for ( i = 0; i < nmodes ; i++ )
  {
    seqData = &gSpmCmdSeqArray[coreNum][i];
      
    /* Sequence includes command with option to RPM HS */
    if ( seqData->rpmModeIndex != -1 )
    {
      /* Set the RPMHS bit in the in-memory copy of sequence */
      *(seqData->sequence + seqData->rpmModeIndex) = newRpmMode;
  
      /* Update RPMHS bit in SLP_SEQ_ENTRY register */
      rpmHwRegByteOffset = seqData->seqStartIndex + seqData->rpmModeIndex; 

      Core_MutexLock( seqInfo->lock );
      HAL_spm_UpdateCmdInHW( coreNum, rpmHwRegByteOffset, newRpmMode );
      Core_MutexUnlock( seqInfo->lock );
    }
  }

  return;
}

/**
 * @brief Updates the SPM control bit that to perform rpm handshake or
 *        skip it during pocessor idle.
 *
 * @param coreNum          : Which SPM
 * @param rpmHandshakeMode : Enum which will determine if rpm handshake
 *                           bit should be set or cleared.
 */
static void HALSPM_setRPMHandshakeModeSawctl
(
  uint32               coreNum, 
  spm_rpm_bypass_type  rpmHandshakeMode
)
{
#if (SPM_HWIO_BMSK(SPM_CTL, SLP_CMD_MODE) != 0)
  uint8 bypassBit = (SPM_RPM_BYPASS_HANDSHAKE == rpmHandshakeMode) ? 0 : 1;
  SPM_HWIO_OUTF(SPM_CTL, SLP_CMD_MODE, bypassBit);
#else
  /* This function should be used only when above macro was defined. Ensure 
   * HWIO header is upto date. */
  CORE_VERIFY(0);
#endif
  return;
}

/**
 * @brief Internal initialization routine.
 *
 * @param coreNum          : Which SPM.
 * @param ppszVersion[out] : Pointer to SPM version string.
 */
static bool32 HAL_spm_Init_internal( uint32 coreNum, char **ppszVersion )
{
  BSP_spm_SawCfgType              *sawCfgPtr              = NULL;
  BSP_spm_SawSpmCtlCfgType        *sawSpmCtlCfgPtr        = NULL;
  BSP_spm_SawSpmDlyType           *sawSpmDlyCfgPtr        = NULL;
  BSP_spm_SawPmicDataType         *sawSpmPmicDataPtr      = NULL;
  uint32                          sawVersion;

  /* Obtaining target specific data */
  BSP_spm_InitTargetData();

  /* Determine whether SPM_SLP_SEQ_ENTRY_n is sufficient in size to
     accomodate all supported sequence commands */
  HAL_spm_PlatformInitEarly( coreNum, &spmPlatform );

  /* copy sequences for all available modes */
  HAL_spm_DwnldAllSlpSeqEntry( coreNum );     

  spmPlatform.spmConfig = &( SPM_BSP_DATA[coreNum] );

  sawCfgPtr           = &((spmPlatform.spmConfig)->sawCfg);
  sawSpmCtlCfgPtr     = &((spmPlatform.spmConfig)->sawSpmCtlCfg);
  sawSpmDlyCfgPtr     = &((spmPlatform.spmConfig)->sawSpmDlyCfg);
  sawSpmPmicDataPtr   = (spmPlatform.spmConfig)->sawSpmPmicData;  

  HAL_spm_ConfigClkDiv      ( coreNum,
                              sawCfgPtr->clkDiv );
  
  HAL_spm_ConfigSpmCtl      ( coreNum,
                              sawSpmCtlCfgPtr->inhibitStartAddrReset,
                              sawSpmCtlCfgPtr->wakeupConfig );

  HAL_spm_ConfigSpmDly      ( coreNum,
                              sawSpmDlyCfgPtr->delay_1,
                              sawSpmDlyCfgPtr->delay_2,
                              sawSpmDlyCfgPtr->delay_3 );
  
  HAL_spm_ConfigPMIC        ( coreNum,
                              sawSpmPmicDataPtr );

  sawVersion = SPM_HWIO_IN_W(SPM_CURR_CORE, VERSION);

  /* Set RPM handshake function based on SAW version. Version 3.0.0 and greater
   * contain a new bit-wise handshake enable, so use that rather than updating
   * the command sequence itself. */
  if(sawVersion >= 0x30000000)
  {
    g_rpmHandshakeFcn = HALSPM_setRPMHandshakeModeSawctl;
  }
  else
  {
    g_rpmHandshakeFcn = HALSPM_setRPMHandshakeModeIndex;
  }

  /* Fill in return value */
  if (ppszVersion != NULL)
  {
    *ppszVersion = (char*)HAL_SPM_VERSION_STRING;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------
**                        EXTERNAL FUNCTIONS
** ----------------------------------------------------------------------- */

/* ===========================================================================
**  HAL_spm_PlatformInitEarly
**
** ======================================================================== */
void HAL_spm_PlatformInitEarly ( uint32 coreNum, HAL_spm_PlatformType *spmPlatform )
{
  uint32 numCmdSeqRegs;
  uint32 bytesRequired;
  HAL_spm_ProgrammedSeqInfoType *seqInfo = NULL;

  /* Allocating memory for array that stores information about programmed
   * command sequences on each spm instance - we need a check as this routine
   * will be called more than once on multicore target */
  if( NULL == prgCmdSeqInfo )
  {
    bytesRequired = sizeof(HAL_spm_ProgrammedSeqInfoType) * SPM_NUM_CORES;
    prgCmdSeqInfo = (HAL_spm_ProgrammedSeqInfoType *)malloc(bytesRequired);
    CORE_VERIFY_PTR( prgCmdSeqInfo );
    memset(prgCmdSeqInfo, 0, bytesRequired);
  }

  /* Initializing object for information about programmed sequnces on this 
   * spm */
  numCmdSeqRegs = SPM_HWIO_INF_W( coreNum, ID, NUM_SPM_ENTRY );
  bytesRequired = (HAL_SPM_BASE_PRG_CMD_SEQ_LIMIT + 1) * sizeof(uint32);
  seqInfo = &prgCmdSeqInfo[coreNum];

  seqInfo->lock = Core_MutexCreate( CORE_MUTEXATTR_DEFAULT );
  CORE_VERIFY_PTR( seqInfo->lock );

  seqInfo->maxAllowedCmds = numCmdSeqRegs * SPM_HAL_COMMANDS_PER_REGISTER;
  seqInfo->usedCmdCount = 0;
  seqInfo->maxAllowedSeqs = HAL_SPM_BASE_PRG_CMD_SEQ_LIMIT;
  seqInfo->numProgrammedSeqs = 0;

  seqInfo->seqList = (uint32 *)malloc(bytesRequired);
  CORE_VERIFY_PTR( seqInfo->seqList );
  memset(seqInfo->seqList, 0, bytesRequired);

} 

/* ===========================================================================
**  HAL_spm_Reset
**
** ======================================================================== */
inline void HAL_spm_Reset( uint32 coreNum )
{
  SPM_HWIO_OUTF_W( coreNum, RST, RST, 0x1 );
}

/* ===========================================================================
**  HAL_spm_Init
**
** ======================================================================== */
bool32 HAL_spm_Init( uint32 coreNum, char **ppszVersion )
{
  /* Map HWIO base address */
  spm_hwio_mmap();

  if ( FALSE == HAL_spm_Init_internal( coreNum, ppszVersion ) )
  {
    return FALSE;
  }

  return TRUE;
}

/* ===========================================================================
**  HAL_spm_UpdateActiveVoltageLevel
**
** Reads the currently set voltage level (set by AVS) and updates the
** PMIC data for the restore voltage level.
** ======================================================================== */
void HAL_spm_UpdateActiveVoltageLevel
( 
  uint32 coreNum
)
{
  uint8 vctl;

  /* get currently set active voltage level */
  HAL_spm_GetVctl( coreNum, &vctl );
  
  CORE_VERIFY( vctl != 0 );

  /* update NOMINAL PMIC config with current voltage level */
  HAL_spm_ConfigPMICData( coreNum, BSP_SPM_SAW_PMIC_DATA_NOMINAL, vctl );
}

/* ===========================================================================
**  HAL_spm_GetSawStatus0
**
** ======================================================================== */
void HAL_spm_GetSawStatus0
( 
  uint32 coreNum,
  uint32 *sawStatus 
)
{
  *sawStatus = SPM_HWIO_IN_W( coreNum, SPM_STS );
}

/* ===========================================================================
**  HAL_spm_GetSawInfo
**
** ======================================================================== */
void HAL_spm_GetSawInfo
( 
  uint32 coreNum,
  uint32 *sawInfo 
)
{
  *sawInfo = SPM_HWIO_IN_W( coreNum, ID );
}

/* ===========================================================================
**  HAL_spm_SetLowPowerMode
**
** ======================================================================== */
void HAL_spm_SetLowPowerMode
(
  uint32 coreNum,
  spm_low_power_mode_type spm_mode
)
{
  BSP_spm_CmdSequenceType *seqData;

  /* Lookup low-power mode data in BSP Data */
  CORE_VERIFY( TRUE == 
               HAL_spm_GetLPMSequenceData( coreNum, spm_mode, &seqData ) );

  /* Perform Mode specific configuration */
  HAL_spm_ConfigLowPowerMode(coreNum, seqData);
  
}

/* ===========================================================================
**  HAL_spm_Set_RPM_HandshakeMode
**
** ======================================================================== */
void HAL_spm_Set_RPM_HandshakeMode
( 
  uint32 coreNum,
  spm_rpm_bypass_type rpmHandshakeMode
)
{
  g_rpmHandshakeFcn(coreNum, rpmHandshakeMode);
  return;
}

/* ===========================================================================
**  HAL_spm_SetSecurity
**
** ======================================================================== */
void HAL_spm_SetSecurity
( 
  uint32 coreNum,
  uint8 sawCtl, 
  uint8 pwrCtl, 
  uint8 vltCtl
)
{
  /* Not supported. */
}

/* ===========================================================================
**  HAL_spm_ConfigClkDiv
**
** ======================================================================== */
void HAL_spm_ConfigClkDiv
( 
  uint32 coreNum,
  uint8 clkDiv
)
{
  SPM_HWIO_OUTF_W( coreNum, CFG, CLK_DIV, clkDiv);
}

/* ===========================================================================
**  HAL_spm_GetVctl
**
** ======================================================================== */
void HAL_spm_GetVctl
( 
  uint32 coreNum,
  uint8 *pmicData
)
{
  *pmicData = (uint8)SPM_HWIO_INF_W( coreNum, VCTL, PMIC_DATA );
}

/* ===========================================================================
**  HAL_spm_ConfigSpmCtl
**
** ======================================================================== */
void HAL_spm_ConfigSpmCtl // internal, init with BSP
( 
  uint32                   coreNum,
  bool32                   inhibitStartAddrReset,
  BSP_spm_SawPowerModeType wakeupConfig
)
{  
  SPM_HWIO_OUTM_W( coreNum, SPM_CTL,
    (SPM_HWIO_FMSK_W( coreNum, SPM_CTL, ISAR )) |
    (SPM_HWIO_FMSK_W( coreNum, SPM_CTL, WAKEUP_CONFIG ))
    ,
    (SPM_HWIO_FVAL_W( coreNum, SPM_CTL, ISAR, inhibitStartAddrReset )) |
    (SPM_HWIO_FVAL_W( coreNum, SPM_CTL, WAKEUP_CONFIG, wakeupConfig ))
  );
}

/* ===========================================================================
**  HAL_spm_ConfigSpmDly
**
** ======================================================================== */
void HAL_spm_ConfigSpmDly // internal, init with BSP
( 
 uint32                   coreNum,
 uint16                   delay_1,
 uint16                   delay_2,
 uint16                   delay_3
)
{  
  SPM_HWIO_OUTM_W( coreNum, SPM_DLY,
                   (SPM_HWIO_FMSK_W( coreNum, SPM_DLY, DLY1 )) |
                   (SPM_HWIO_FMSK_W( coreNum, SPM_DLY, DLY2 )) |
                   (SPM_HWIO_FMSK_W( coreNum, SPM_DLY, DLY3 )) 
                   ,
                   (SPM_HWIO_FVAL_W( coreNum, SPM_DLY, DLY1, delay_1 )) |
                   (SPM_HWIO_FVAL_W( coreNum, SPM_DLY, DLY2, delay_2 )) |
                   (SPM_HWIO_FVAL_W( coreNum, SPM_DLY, DLY3, delay_3 )) 
                   );
}

/* ===========================================================================
**  HAL_spm_ConfigPMIC
** ======================================================================== */
void HAL_spm_ConfigPMIC
( 
  uint32 coreNum,
  BSP_spm_SawPmicDataType pmicCfg[]
)
{
  int i;

  for(i = 0; i < BSP_SPM_SAW_PMIC_DATA_NUM; i++)
  {
    SPM_HWIO_OUTFI_W( coreNum, SPM_PMIC_DATA_n, 
                      pmicCfg[i].mode, SIZE, pmicCfg[i].pmicDataSize );
    SPM_HWIO_OUTFI_W( coreNum, SPM_PMIC_DATA_n, 
                      pmicCfg[i].mode, ADR_IDX, pmicCfg[i].adrIdx );

    HAL_spm_ConfigPMICDelay(coreNum, pmicCfg[i].mode, pmicCfg[i].pmicDly);
    HAL_spm_ConfigPMICData(coreNum, pmicCfg[i].mode, pmicCfg[i].pmicData);
  }
}

/* ===========================================================================
**  HAL_spm_Enable
**
** ======================================================================== */
void HAL_spm_Enable( uint32 coreNum, bool32 spmEnable )
{
  SPM_HWIO_OUTF_W( coreNum, SPM_CTL, SPM_EN, spmEnable );
}

/* ===========================================================================
**  HAL_spm_ProgramCmdSeq
**
** ======================================================================== */
spm_result_t HAL_spm_ProgramCmdSeq
( 
  uint32 coreNum,
  spm_cmd_t *seq, 
  uint32 seqLength,
  uint32 *startAddr
)
{
  uint32 prgSeqCount = 0; 
  uint32 *seqList = NULL;
  HAL_spm_ProgrammedSeqInfoType *seqInfo = NULL; 

  /* Ensure that SPM is initialized enough to program the sequence */
  CORE_VERIFY_PTR( prgCmdSeqInfo );

  /* Basic verification on input command sequence */
  if( (NULL == seq) || (NULL == startAddr) || (0 == seqLength) )
  {
    return SPM_INVALID_PARAM;
  }

  seqInfo = &prgCmdSeqInfo[coreNum];

  Core_MutexLock( seqInfo->lock );

  /* Ensure that we can actually copy this command sequence to HW */
  if( seqLength > (seqInfo->maxAllowedCmds - seqInfo->usedCmdCount) )
  {
    Core_MutexUnlock( seqInfo->lock );
    return SPM_NO_SEQ_MEMORY;
  }

  /* We can copy a new sequence in HW. Make sure that we have memory in 
   * sequence meta data array. If not, increase it by reallocating. */
  if( seqInfo->numProgrammedSeqs == seqInfo->maxAllowedSeqs )
  {
    /* Increase number of allowed sequences */
    seqInfo->maxAllowedSeqs += HAL_SPM_BASE_PRG_CMD_SEQ_LIMIT;

    seqList = seqInfo->seqList;
    seqList = (uint32 *)realloc(seqList, 
                                (seqInfo->maxAllowedSeqs + 1) * sizeof(uint32));
    CORE_VERIFY_PTR( seqList );

    /* Setting additional memory to 0 */
    memset( &seqList[seqInfo->numProgrammedSeqs+1], 0, 
            HAL_SPM_BASE_PRG_CMD_SEQ_LIMIT * sizeof(uint32) );

    seqInfo->seqList = seqList;
  }

  /* Input sequence can be copied - Get the start address */
  *startAddr = seqInfo->usedCmdCount; 

  /* Actually copying sequence to HW */
  HAL_spm_CopyCmdSeq( coreNum, seq, seqLength, seqInfo->usedCmdCount );

  /* Updating information about programmed sequences */
  seqInfo->usedCmdCount += seqLength;
  prgSeqCount = seqInfo->numProgrammedSeqs++;
  seqInfo->seqList[prgSeqCount] = *startAddr;
  seqInfo->seqList[prgSeqCount+1] = seqInfo->usedCmdCount;

  Core_MutexUnlock( seqInfo->lock );

  return SPM_SUCCESS;
}

/* ===========================================================================
**  HAL_spm_UpdateCmdSeq
**
** ======================================================================== */
spm_result_t HAL_spm_UpdateCmdSeq
( 
  uint32 coreNum,
  spm_cmd_t *seq, 
  uint32 seqLen, 
  uint32 startAddr 
)
{
  uint32 i = 0;
  HAL_spm_ProgrammedSeqInfoType *seqInfo = NULL;
  spm_result_t retVal = SPM_ERROR;

  /* Check that SPM is initialized enough to update the sequence */
  CORE_VERIFY_PTR( prgCmdSeqInfo );

  /* Basic verification on input command sequence */
  if( (NULL == seq) || (seqLen == 0) )
  {
    return SPM_INVALID_PARAM;
  }

  seqInfo = &prgCmdSeqInfo[coreNum];

  Core_MutexLock( seqInfo->lock );
  
  /* Check if actually there is an entry for this address and if we
   * can fit the sequence. */
  for( i = 0; i < seqInfo->numProgrammedSeqs; i++ )
  {
    if( (startAddr == seqInfo->seqList[i]) && 
        (seqLen <= (seqInfo->seqList[i+1] - seqInfo->seqList[i])) )
    {
      break;
    }
  }

  if( i < seqInfo->numProgrammedSeqs )
  {
    /* Found sequence: Actually updating the sequence in SPM HW */
    HAL_spm_CopyCmdSeq( coreNum, seq, seqLen, startAddr );
    retVal = SPM_SUCCESS;
  }
  else
  {
    /* No programmed sequence found at a given start address or the new
     * sequence is longer than originally programmed one. */
    retVal = SPM_INVALID_PARAM;
  }

  Core_MutexUnlock( seqInfo->lock );

  return retVal;
}

/* ===========================================================================
**  HAL_spm_UpdateCmdInSeq
**
** ======================================================================== */
spm_result_t HAL_spm_UpdateCmdInSeq
(
  uint32 coreNum,
  uint32 startAddr,
  uint32 cmdOffset,
  spm_cmd_t updatedCmd
)
{
  uint32 i = 0;
  spm_result_t result = SPM_ERROR;
  HAL_spm_ProgrammedSeqInfoType *seqInfo = NULL;

  /* Check that SPM is initialized enough to update the sequence */
  CORE_VERIFY_PTR( prgCmdSeqInfo );

  seqInfo = &prgCmdSeqInfo[coreNum];

  Core_MutexLock( seqInfo->lock );

  /* Check if a sequence exists at a given start address and if offset falls
   * within its range */
  for( i = 0; i < seqInfo->numProgrammedSeqs; i++ )
  {
    if( (startAddr == seqInfo->seqList[i]) &&
        (cmdOffset < (seqInfo->seqList[i+1] - seqInfo->seqList[i])) )
    {
      break;
    }
  }

  if( i < seqInfo->numProgrammedSeqs )
  {
    /* Found an entry - update the command */
    HAL_spm_UpdateCmdInHW( coreNum, startAddr+cmdOffset, updatedCmd );
    result = SPM_SUCCESS;
  }
  else
{
    /* Entry not found */
    result = SPM_INVALID_PARAM;
}

  Core_MutexUnlock( seqInfo->lock );

  return result;
}
