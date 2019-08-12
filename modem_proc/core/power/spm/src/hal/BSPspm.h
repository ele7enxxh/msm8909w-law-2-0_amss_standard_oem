#ifndef BSP_SPM_H
#define BSP_SPM_H
/*===========================================================================

FILE:         BSPspm.h

DESCRIPTION:  
  This is the BSP data interface for the SPM(SAW2) driver.

===========================================================================
             Copyright © 2010-2015 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 Qualcomm Technologies Confidential and Proprietary.
===========================================================================

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/src/hal/BSPspm.h#1 $

=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "HALcomdef.h"
#include "spm.h"

/* -----------------------------------------------------------------------
** Constants
** ----------------------------------------------------------------------- */

/* Generic SAW2 Commands that are independent of the processor instance
 * with which it is used */
#define BSP_SPM_CMD_SLEEP_NO_RPM_HANDSHAKE              0x03
#define BSP_SPM_CMD_SLEEP_RPM_HANDSHAKE                 0x07
#define BSP_SPM_CMD_END_OF_PROGRAM                      0x0F

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* This enum is for enabling/disabling non-secure bus transactions  for SPM Hw */
typedef enum
{ 
  BSP_SPM_SAW_SECURE,
  BSP_SPM_SAW_NONSECURE
} BSP_spm_SawSecureType;

/* Type for various registers group of SPM Hw which can be secure or non-secure */
typedef struct
{
  BSP_spm_SawSecureType sawCtl;
  BSP_spm_SawSecureType pwrCtl;
  BSP_spm_SawSecureType vltCtl;
} BSP_spm_SawSecureCfgType;

/* Type which reflects the revision and parameter information for
   the SAW2 core */
typedef struct
{
  uint8  numSpmEntry;
  uint8  numPwrCntl;
  uint8  pmicDataWidth;
  uint8  numPmicData;
  BSP_spm_SawSecureType CfgNSAccess;
  bool32 pmicArbiterIntf;
  bool32 avsPresent;
  bool32 spmPresent;
} BSP_spm_SawIDType;

/* Type which configures the common control between AVS and SPM */
typedef struct
{
  bool32                       frcRefClkOn;
  uint8                        clkDiv;
} BSP_spm_SawCfgType;

/* State of RPM finite state machine */
typedef enum
{
  BSP_SPM_SAW_RPM_STATE_RUN, /* waiting for SPM request */
  BSP_SPM_SAW_RPM_STATE_STDNACK, /* waiting for shutdown ACK from RPM */
  BSP_SPM_SAW_RPM_STATE_WAKEUP, /* waiting for wakeup interrupt */
  BSP_SPM_SAW_RPM_STATE_BGUPACK /* waiting for bringup ACK from RPM */
} BSP_spm_SawRpmStateType;

/* SAW2 SPM Status Register */
typedef struct
{
  bool32                   shtdwnReqSpmToRpm;
  bool32                   shtdwnAckRpmToSpm;
  bool32                   bringUpReqSpmToRpm;
  bool32                   bringUpAckRpmToSpm;
  BSP_spm_SawRpmStateType  rpmState;
  uint8                    spmCmdAddr;
} BSP_spm_SawStsType;

/* State of PMIC finite state machine */
typedef enum
{
  BSP_SPM_SAW_PMIC_STATE_IDLE, /* Ready for new request from AVS or SPM */
  BSP_SPM_SAW_PMIC_STATE_DONE, /* waiting for DONE from PMIC arbiter
                                  before transitions back to IDLE */  
  BSP_SPM_SAW_PMIC_STATE_DELAY /* waiting for delay count termination
                                  before transitions back to IDLE */
} BSP_spm_SawPmicStateType;

/* SAW2 PMIC Status Register */
typedef struct
{
  uint8                    currDelay;   /* VLVL value of the counter used to
                                           calculate the time until next AVS
                                           controller request for a new VLVL */
  uint8                    currPmicSize;
  BSP_spm_SawPmicStateType pmicState;
  uint8                    currPmicData;
  
} BSP_pmic_SawStsType;

/* Size of PMIC data */
typedef enum
{
  BSP_SPM_SAW_PMIC_DATA_SIZE_0,    /* 1 byte */
  BSP_SPM_SAW_PMIC_DATA_SIZE_1     /* 2 byte */
} BSP_spm_SawPmicDataSize;


/* SAW2_VCTL */
typedef struct
{
  BSP_spm_SawPmicDataSize pmicDataSize;
  uint8                   adrIdx;
  uint8                   pmicData;    /* PMIC DATA sent to the PMIC Arbiter */
} BSP_spm_SawVctlCfgType;

/* This enum reflects the wakeup configuration */
/* sys_spm_wakeup         - This signal indicates that CPU wants to initiate
                            a power up sequence. */
/* cpu_spm_wait_req       - This signal indicates that CPU wants to initiate
                            the power down sequence. */
/* sys_saw_dbg_stay_awake - When this signal is asserted the SPM will go to
                            the motion of programmed power mode but will not
                            gate clock, not turn OFF head switch/foot switch,
                            not turn on CLAMP. */
typedef enum
{
  BSP_SPM_SAW_WAKEUP_CONFIG_0, /* sys_spm_wakeup */
  BSP_SPM_SAW_WAKEUP_CONFIG_1, /* sys_spm_wakeup OR !cpu_spm_wait_req */
  BSP_SPM_SAW_WAKEUP_CONFIG_2, /* sys_spm_wakeup OR rising edge of
                                  sys_saw_dbg_stay_awake */
  BSP_SPM_SAW_WAKEUP_CONFIG_3  /* sys_spm_wakeup OR !cpu_spm_wait_req 
                                  OR rising edge of sys_saw_dbg_stay_awake */
} BSP_spm_SawPowerModeType;

/* Type which is used for control of SPM operations */
typedef struct
{
  bool32                   inhibitStartAddrReset;
  BSP_spm_SawPowerModeType wakeupConfig;
} BSP_spm_SawSpmCtlCfgType;

/* Type which is used to provide the delay values for SPM Delay command */
typedef struct
{
  uint16 delay_1; /* SPM Delay. Controls the time between
                     SPM_SLP_SEQ_SPM Delay and the next
                     SPM_SLP_SEQ command */
  uint16 delay_2; /* SPM Delay. Controls the time between
                     SPM_SLP_SEQ_SPM Delay and the next
                     SPM_SLP_SEQ command */
  uint16 delay_3; /* SPM Delay. Controls the time between
                     SPM_SLP_SEQ_SPM Delay and the next
                     SPM_SLP_SEQ command */
} BSP_spm_SawSpmDlyType;


/* Enums that must match any PMIC operation commands within SPM SLP SEQ lists */
typedef enum 
{
  BSP_SPM_SAW_PMIC_DATA_NOMINAL = 0,
  BSP_SPM_SAW_PMIC_DATA_PC = 1,
  BSP_SPM_SAW_PMIC_DATA_VDD_MIN = 2,
  BSP_SPM_SAW_PMIC_DATA_NUM    /* Number of combinations that can be programmed */
} BSP_spm_PmicModeData;

  
/* Type which is used to provide the PMIC data values for SPM FSM PMIC
   transaction. */
typedef struct
{
  BSP_spm_PmicModeData    mode;          /* Specifies PMIC_DATA_VLVL and DLY 
                                            combination that is being programmed */

  uint8                   pmicData;      /* DATA sent to the PMIC Arbiter */

  uint8                   pmicDly;       /* PMIC Delay. Controls the time 
                                            between SPM_SLP_SEQ PMIC command 
                                            with SAW2_SPM_PMIC_DATA_n and 
                                            the next SPM_SLP_SEQ command. */

  BSP_spm_SawPmicDataSize pmicDataSize;  /* Width of SAW -> PMIC interface */

  uint8                   adrIdx;        /* ADR_IDX field for PMIC */

} BSP_spm_SawPmicDataType;

/* Type which reflects the overall SPM (SAW2) configuration values */
typedef struct
{
  BSP_spm_SawSecureCfgType   sawSecureCfg;
  BSP_spm_SawCfgType         sawCfg;
  BSP_spm_SawSpmCtlCfgType   sawSpmCtlCfg;
  BSP_spm_SawSpmDlyType      sawSpmDlyCfg;
  BSP_spm_SawPmicDataType    sawSpmPmicData[BSP_SPM_SAW_PMIC_DATA_NUM];
} BSP_spm_ConfigDataType;

#define HAL_spm_CfgType BSP_spm_ConfigDataType

/* BSP SPM Version of the data file to be used by SW and BSP to access the 
 * data */
#define HAL_SPM_BSP_VERSION  \
                    BSP_ENCODE_VERSION(1, 0, 1)  /* major, minor, build */

/* Container type for specifying the SPM command sequence for the
 * different sleep modes. */
typedef struct 
{
  spm_low_power_mode_type mode;  /**< Low power mode for this cmd sequence */
  uint8           *sequence;     /**< Pointer to an array of SPM commands */
  uint8           len;           /**< Size of the sequence[] array - initial at
                                       compile time using sizeof() */
  int8           rpmModeIndex;   /**< sequence[rpmModeIndex] must contain the
                                       RPM handshake mode command */
  int8           seqStartIndex;  /**< start index of this modes SPM_SLP_SEQ_ENTRY */
  
} BSP_spm_CmdSequenceType;


/* -----------------------------------------------------------------------
**                           GLOBAL DATA
** ----------------------------------------------------------------------- */

/* Pointer to structure that contains initialization data for various SPM
 * registers. */
extern BSP_spm_ConfigDataType *SPM_BSP_DATA;

/* Pointer to array of command sequence type of size SPM_NUM_LOW_POWER_MODES.
 * Array contains the command sequence related information for various modes. */
extern BSP_spm_CmdSequenceType (*gSpmCmdSeqArray)[SPM_NUM_LOW_POWER_MODES];

/**
 * @brief BSP_spm_InitTargetData
 *
 * Performs initialization of target specific values for the SPM like 
 *   SPM_NUM_CORES
 *   SPM_BSP_DATA
 *   gSpmCmdSeqArray
 *
 * The above variables may be initialized using different techniques like
 * querying device config or direct assignment of variables from target 
 * specific file.
 */
void BSP_spm_InitTargetData( void );

/**
 * @brief Returns the number of power sequences that have specified in the 
 *        platform. 
 *
 * @dependency  SPM_NUM_CORES must be initialized. 
 *   gSpmCmdSeqArray must be a valid pointer.
 * 
 * @param coreNum  Which SPM.
 *
 * @return Number of sequences.
 */
uint32 BSP_spm_GetNumSupportedLPMs( uint32 coreNum );

#ifdef __cplusplus
}
#endif

#endif /* BSP_SPM_H */

