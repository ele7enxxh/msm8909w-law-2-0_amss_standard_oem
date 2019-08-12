#ifndef HAL_SPM_PLATFORM_H
#define HAL_SPM_PLATFORM_H

/*=============================================================================
   FILE:           HALspmPlatform.h

   OVERVIEW:       This is the hardware abstraction layer internal interface
                   for the SPM v2 (SAW2) block.


                 Copyright © 2010-2014 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/spm/src/hal/HALspmPlatform.h#1 $

=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "BSPspm.h"
#include "HALhwio.h"
#include "HALspm_hwio.h"

/* -----------------------------------------------------------------------
**                   PLATFORM SPECIFIC MACROS
** ----------------------------------------------------------------------- */

/* 
 * The following three macros are not supposed to change on a given SPM
 * HW version. If that happens and we still need to maintain binary 
 * compatibility, we may need to switch to variables instead of macros.
 */

/*
 * Size of each command sequence register in bytes
 */
#define SPM_HAL_COMMAND_SEQUENCE_REGISTER_SIZE 4

/*
 * Size of each command in bytes
 */
#define SPM_HAL_COMMAND_SIZE 1

/*
 * Number of commands per register
 */
#define SPM_HAL_COMMANDS_PER_REGISTER \
  (SPM_HAL_COMMAND_SEQUENCE_REGISTER_SIZE / SPM_HAL_COMMAND_SIZE)

/* -----------------------------------------------------------------------
**                       HWIO Macros
** ----------------------------------------------------------------------- */

/*
 * Macros to handle differing register names.  Build script should send
 * SPM_HWIO_PREFIX via command line during compilation, based on target.
 */
#define SPM_HWIO_CONCAT_NAME(prefix, name)                  prefix##_##name
#define SPM_HWIO_CONCAT_BMSK(prefix, reg, field)            HWIO_##prefix##_##reg##_##field##_BMSK
#define SPM_HWIO_EXPAND_NAME(prefix, name)                  SPM_HWIO_CONCAT_NAME(prefix, name)
#define SPM_HWIO_EXPAND_BMSK(prefix, reg, field)            SPM_HWIO_CONCAT_BMSK(prefix, reg, field)

#define SPM_HWIO_ADDR( name )                               HWIO_ADDR(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name))
#define SPM_HWIO_BMSK( reg, field )                         SPM_HWIO_EXPAND_BMSK(SPM_HWIO_PREFIX, reg, field)
#define SPM_HWIO_FMSK( reg, field )                         HWIO_FMSK(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field)
#define SPM_HWIO_SHFT( reg, field )                         HWIO_SHFT(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field)
#define SPM_HWIO_FVAL( reg, field, val )                    HWIO_FVAL(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field, val)
#define SPM_HWIO_IN( name )                                 HWIO_IN(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name))
#define SPM_HWIO_INI( name, index )                         HWIO_INI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index)
#define SPM_HWIO_INF( reg, field )                          HWIO_INF(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field)
#define SPM_HWIO_OUT( name, val )                           HWIO_OUT(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), val)
#define SPM_HWIO_OUTI( reg, index, val )                    HWIO_OUTI(SPM_HWIO_EXPAND_REG(SPM_HWIO_PREFIX, reg), index, val)
#define SPM_HWIO_OUTM( name, bmsk, val )                    HWIO_OUTM(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), bmsk, val)
#define SPM_HWIO_OUTMI( name, index, bmsk, val )            HWIO_OUTMI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index, bmsk, val)
#define SPM_HWIO_OUTF( reg, field, val )                    HWIO_OUTF(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field, val)
#define SPM_HWIO_OUTFI( name, index, field, val )           HWIO_OUTFI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index, field, val)

  /* currently unused: in-case we need to explicityl specify prefix _PX */
#define SPM_HWIO_ADDR_PX( prefix, name )                    HWIO_ADDR(SPM_HWIO_EXPAND_NAME(prefix, name))
#define SPM_HWIO_BMSK_PX( prefix, reg, field )              SPM_HWIO_EXPAND_BMSK(prefix, reg, field)
#define SPM_HWIO_FMSK_PX( prefix, reg, field )              HWIO_FMSK(SPM_HWIO_EXPAND_NAME(prefix, reg), field)
#define SPM_HWIO_SHFT_PX( prefix, reg, field )              HWIO_SHFT(SPM_HWIO_EXPAND_NAME(prefix, reg), field)
#define SPM_HWIO_FVAL_PX( prefix, reg, field, val )         HWIO_FVAL(SPM_HWIO_EXPAND_NAME(prefix, reg), field, val)
#define SPM_HWIO_IN_PX( prefix, name )                      HWIO_IN(SPM_HWIO_EXPAND_NAME(prefix, name))
#define SPM_HWIO_INI_PX( prefix, name, index )              HWIO_INI(SPM_HWIO_EXPAND_NAME(prefix, name), index)
#define SPM_HWIO_INF_PX( prefix, reg, field )               HWIO_INF(SPM_HWIO_EXPAND_NAME(prefix, reg), field)
#define SPM_HWIO_OUT_PX( prefix, name, val )                HWIO_OUT(SPM_HWIO_EXPAND_NAME(prefix, name), val)
#define SPM_HWIO_OUTI_PX( prefix, reg, index, val )         HWIO_OUTI(SPM_HWIO_EXPAND_REG(prefix, reg), index, val)
#define SPM_HWIO_OUTM_PX( prefix, name, bmsk, val )         HWIO_OUTM(SPM_HWIO_EXPAND_NAME(prefix, name), bmsk, val)
#define SPM_HWIO_OUTMI_PX( prefix, name, index, bmsk, val ) HWIO_OUTMI(SPM_HWIO_EXPAND_NAME(prefix, name), index, bmsk, val)
#define SPM_HWIO_OUTF_PX( prefix, reg, field, val )         HWIO_OUTF(SPM_HWIO_EXPAND_NAME(prefix, reg), field, val)
#define SPM_HWIO_OUTFI_PX( prefix, name, index, field, val )HWIO_OUTFI(SPM_HWIO_EXPAND_NAME(prefix, name), index, field, val)

#if !defined(SPM_HWIO_PREFIX_L2)

#define SPM_HWIO_ADDR_W( core, name )                       HWIO_ADDR(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name))
#define SPM_HWIO_BMSK_W( core, reg, field )                 SPM_HWIO_EXPAND_BMSK(SPM_HWIO_PREFIX, reg, field)
#define SPM_HWIO_FMSK_W( core, reg, field )                 HWIO_FMSK(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field)
#define SPM_HWIO_SHFT_W( core, reg, field )                 HWIO_SHFT(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field)
#define SPM_HWIO_FVAL_W( core, reg, field, val )            HWIO_FVAL(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field, val)
#define SPM_HWIO_IN_W( core, name )                         HWIO_IN(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name))
#define SPM_HWIO_INI_W( core, name, index )                 HWIO_INI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index)
#define SPM_HWIO_INF_W( core, reg, field )                  HWIO_INF(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field)
#define SPM_HWIO_OUT_W( core, name, val )                   HWIO_OUT(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), val)
#define SPM_HWIO_OUTI_W( core, reg, index, val )            HWIO_OUTI(SPM_HWIO_EXPAND_REG(SPM_HWIO_PREFIX, reg), index, val)
#define SPM_HWIO_OUTM_W( core, name, bmsk, val )            HWIO_OUTM(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), bmsk, val)
#define SPM_HWIO_OUTMI_W( core, name, index, bmsk, val )    HWIO_OUTMI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index, bmsk, val)
#define SPM_HWIO_OUTF_W( core, reg, field, val )            HWIO_OUTF(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field, val)
#define SPM_HWIO_OUTFI_W( core, name, index, field, val )   HWIO_OUTFI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index, field, val)

#else

#define SPM_CORE_NDX_L2 2

#define SPM_HWIO_ADDR_W( core, name )                       ((core != SPM_CORE_NDX_L2) ? (HWIO_ADDR(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name)))                     : (HWIO_ADDR(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 name))))
#define SPM_HWIO_BMSK_W( core, reg, field )                 ((core != SPM_CORE_NDX_L2) ? (SPM_HWIO_EXPAND_BMSK(SPM_HWIO_PREFIX, reg, field))                          : (SPM_HWIO_EXPAND_BMSK(SPM_HWIO_PREFIX_L2 reg, field)))
#define SPM_HWIO_FMSK_W( core, reg, field )                 ((core != SPM_CORE_NDX_L2) ? (HWIO_FMSK(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field))               : (HWIO_FMSK(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 reg), field)))
#define SPM_HWIO_SHFT_W( core, reg, field )                 ((core != SPM_CORE_NDX_L2) ? (HWIO_SHFT(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field))               : (HWIO_SHFT(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 reg), field)))
#define SPM_HWIO_FVAL_W( core, reg, field, val )            ((core != SPM_CORE_NDX_L2) ? (HWIO_FVAL(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field, val))          : (HWIO_FVAL(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 reg), field, val)))
#define SPM_HWIO_IN_W( core, name )                         ((core != SPM_CORE_NDX_L2) ? (HWIO_IN(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name)))                       : (HWIO_IN(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 name))))
#define SPM_HWIO_INI_W( core, name, index )                 ((core != SPM_CORE_NDX_L2) ? (HWIO_INI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index))               : (HWIO_INI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 name), index)))
#define SPM_HWIO_INF_W( core, reg, field )                  ((core != SPM_CORE_NDX_L2) ? (HWIO_INF(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field))                : (HWIO_INF(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 reg), field)))
#define SPM_HWIO_OUT_W( core, name, val )                   ((core != SPM_CORE_NDX_L2) ? (HWIO_OUT(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), val))                 : (HWIO_OUT(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 name), val)))
#define SPM_HWIO_OUTI_W( core, reg, index, val )            ((core != SPM_CORE_NDX_L2) ? (HWIO_OUTI(SPM_HWIO_EXPAND_REG(SPM_HWIO_PREFIX, reg), index, val))           : (HWIO_OUTI(SPM_HWIO_EXPAND_REG(SPM_HWIO_PREFIX_L2 reg), index, val)))
#define SPM_HWIO_OUTM_W( core, name, bmsk, val )            ((core != SPM_CORE_NDX_L2) ? (HWIO_OUTM(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), bmsk, val))          : (HWIO_OUTM(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 name), bmsk, val)))
#define SPM_HWIO_OUTMI_W( core, name, index, bmsk, val )    ((core != SPM_CORE_NDX_L2) ? (HWIO_OUTMI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index, bmsk, val))  : (HWIO_OUTMI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 name), index, bmsk, val)))
#define SPM_HWIO_OUTF_W( core, reg, field, val )            ((core != SPM_CORE_NDX_L2) ? (HWIO_OUTF(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, reg), field, val))          : (HWIO_OUTF(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 reg), field, val)))
#define SPM_HWIO_OUTFI_W( core, name, index, field, val )   ((core != SPM_CORE_NDX_L2) ? (HWIO_OUTFI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX, name), index, field, val)) : (HWIO_OUTFI(SPM_HWIO_EXPAND_NAME(SPM_HWIO_PREFIX_L2 name), index, field, val)))

#undef SPM_CORE_NDX_L2

#endif /* !defined(SPM_HWIO_PREFIX_L2) */


/* -----------------------------------------------------------------------
**                           Constants
** ----------------------------------------------------------------------- */

/* Technically not a constant but it used for memory mapping with DAL. Once
 * it is assigned a value, it is never changed */
extern uint8 *SPM_BASE_PTR;

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/**
 @brief Platform (asic/processor) specific information filled in by the
        platform code in HAL_spm_PlatformEarlyInit.
 */
typedef struct
{
  HAL_spm_CfgType *spmConfig;

  void   (*InitLate)(void); /**< Optional function executed at the end
                                 of HAL_pic_Init.*/
  void   (*Save)(void);     /**< Optional function executed at the end
                                 of HAL_pic_Save. */
  void   (*Restore)(void);  /**< HAL_pic_Restore */

} HAL_spm_PlatformType;

/* -----------------------------------------------------------------------
**                           FUNCTIONS
** ----------------------------------------------------------------------- */

/**
 * This function returns the contents of the SAW2_ID register.  The caller
 * must know how to interpret the data returned.
 *
 * @param coreNum[in]   - Which SPM.
 * @param[out] *sawInfo - pointer to the location to put the saw information in.
 *
 * @return None.
 *
 * @dependencies None.
 */
void HAL_spm_GetSawInfo
(
  uint32 coreNum,
  uint32 *sawInfo
);

/**
 * This function programs the security settings of the SAW2 block.
 * These settings should only be programmed by the SROT.  The SROT
 * reads these values from the BSP at init time and programs them.
 *
 * @param[in] coreNum Which SPM.
 * @param[in] sawCtl  Controls security treatment for SAW2 registers.
 * @param[in] pwrCtl  Controls security treatment for SPMv2 registers.
 * @param[in] vltCtl  Controls security treatment for AVS registers.
 *
 * @return None.
 *
 * @dependencies None.
 *
 * @sa HAL_spm_Init
 */
void HAL_spm_SetSecurity // internal, proc-specific, init with BSP
(
  uint32 coreNum,
  uint8 sawCtl,
  uint8 pwrCtl,
  uint8 vltCtl
);

/**
 * This function is used to configure the PMIC arbiter address
 * index and PMIC handshake. These values are read from the BSP
 * data at init time and programmed.
 *
 * @param[in] coreNum   Which SPM.
 * @param[in] pmicData  Pointer to array of DATA_VLVL_DLY combinations.
 *
 * @return None
 *
 * @dependencies BSP Programmer must cross-check that SPM SLP SEQ refers
 * to one of the voltage and delay combinations that are programmed here.
 *
 * @sa HAL_spm_Init
 */
void HAL_spm_ConfigPMIC // internal, init with BSP
(
  uint32 coreNum,
  BSP_spm_SawPmicDataType* pmicData
);

/**
 * This function is used to configure the divider ratio for the clock.
 * This data is read from the BSP at init time and programmed.
 *
 * @param[in] coreNum Which SPM.
 * @param[in] clkDiv  Divider ratio for the clock.
 *
 * @return None
 *
 * @dependencies None.
 *
 * @sa HAL_spm_Init
 */
void HAL_spm_ConfigClkDiv // internal, init with BSP
(
  uint32 coreNum,
  uint8 clkDiv
);

/**
 * This function is used to configure the SPM control
 * parameters. This data is read from the BSP at init time and
 * programmed.
 *
 * @param[in] coreNum                Which SPM.
 * @param[in] inhibitStartAddrReset  Inhibit Start Address Reset.
 *                                   0:End of program reset the
 *                                     SPM_START_ADR to zero.
 *                                   1:Inhibit End of program to
 *                                     reset SPM_START_ADR.
 * @param[in] wakeupConfig           Wakeup Configuration.
 *
 * @return None
 *
 * @dependencies None.
 *
 * @sa HAL_spm_Init
 */
void HAL_spm_ConfigSpmCtl // internal, init with BSP
(
  uint32                   coreNum,
  bool32                   inhibitStartAddrReset,
  BSP_spm_SawPowerModeType wakeupConfig
);

/**
 * This function is used to configure the SPM control
 * parameters. This data is read from the BSP at init time and
 * programmed.
 *
 * @param[in] coreNum        Which SPM.
 * @param[in] delay_1        Delay programmed in DLY1 field of SPM_DLY.
 * @param[in] delay_2        Delay programmed in DLY2 field of SPM_DLY.
 * @param[in] delay_3        Delay programmed in DLY3 field of SPM_DLY.
 *
 * @return None
 *
 * @dependencies None.
 *
 * @sa HAL_spm_Init
 */
void HAL_spm_ConfigSpmDly // internal, init with BSP
(
  uint32                   coreNum,
  uint16                   delay_1,
  uint16                   delay_2,
  uint16                   delay_3
);

/**
 * This function returns the SAW2_VCTL register value.
 *
 * @param[in] coreNum         Which SPM.
 * @param[out] pmicData       PMIC_DATA register field value
 *
 * @return None
 *
 * @dependencies None.
 *
 * @sa HAL_spm_Init
 */
void HAL_spm_GetVctl
(
  uint32 coreNum,
  uint8 *pmicData
);

/**
 * This function can be used for any platform wide, pre-BSP config
 *
 * @param spmPlatform: BSP Platform info
 *
 * @return None
 *
 * @dependencies None.
 *
 * @sa HAL_spm_Init
 */
void HAL_spm_PlatformInitEarly
(
  uint32 coreNum,
  HAL_spm_PlatformType *spmPlatform
);

#ifdef __cplusplus
}
#endif

#endif /* HAL_SPM_PLATFORM_H */
