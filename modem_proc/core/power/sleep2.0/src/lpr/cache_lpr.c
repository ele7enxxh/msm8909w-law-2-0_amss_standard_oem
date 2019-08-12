/*==============================================================================
  FILE:         cache_lpr.c

  OVERVIEW:     This file provides the sleep LPR cache definition for the
                QDSP6V4 ALL WAIT low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2013-2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/lpr/cache_lpr.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "DALStdDef.h"
#include "sleep_log.h"
#include "sleep_target.h"
#include "HALhwio.h"
#include "HALsleep_hwio.h"

/*==============================================================================
                              INTERNAL TYPES
 =============================================================================*/
/** 
 * mem_state
 * 
 * @brief Used to enumerate lprm configuration for cacheLPR_l2ConfigureSPM
 */
typedef enum{
  MEM_RET,       /* Config processor SS & MEM for ret           */
  MEM_NORET,     /* Config processor SS & MEM for non-ret       */
} mem_state;

/*==============================================================================
                           EXTERNAL VARIABLES
 =============================================================================*/
/* g_sleepTargetConfig stores target specific config */
extern sleep_target_config g_sleepTargetConfig;

/*===========================================================================
                              GLOBAL FUNCTIONS
 ==========================================================================*/
/* SPMCTL_EN_MEM BIT configuration */
/* QDSP6SS_SPMCTL_EN_MEM, L2TAG,          RET = 0x0,  NO_RET = 0xF   */
/* QDSP6SS_SPMCTL_EN_MEM, L2DATA,         RET = 0x00, NO_RET = 0x3F */

/* MSS QDSP6v5  L2DATA (768K) = 256KB L2$ + 512KB TCM (Lower bits are for TCM) */
/* L2_TCM_RET   L2DATA = 0x00 0b0000 0000  (each bit corr. to 128KB)    */
/* L2_TCM_NORET L2DATA = 0x3F 0b0011 1111                              */
/* L2_NORET     L2DATA = 0x03 0b0011 0000                              */

/** 
 * cacheLPR_TCMConfigureSPM 
 *  
 * @brief Perform TCM specific configuration for Power Collapse
 *
 * @param tcm_state: enum that specifies whether memory is retained or not
 */
static void cacheLPR_TCMConfigureSPM(mem_state tcm_state)
{
  uint32 l2data = HWIO_INF( QDSP6SS_SPMCTL_EN_MEM, L2DATA );

  /* TCM is controlled by target specific config */
  if(tcm_state == MEM_RET)
  {
    /* Clear bit */
    l2data &= ~g_sleepTargetConfig.tcm_mask;
  }
  else
  {
    /* Set bit */
    l2data |= g_sleepTargetConfig.tcm_mask;
  }

  HWIO_OUTF(QDSP6SS_SPMCTL_EN_MEM, L2DATA, l2data);
  return;
}

/** 
 * cacheLPR_L2ConfigureSPM 
 *  
 * @brief Perform L2 specific configuration for Power Collapse
 *
 * @param l2_state: enum that specifies whether memory is retained or not
 */
static void cacheLPR_L2ConfigureSPM(mem_state l2_state)
{
  uint32 l2tag;
  uint32 l2data = HWIO_INF( QDSP6SS_SPMCTL_EN_MEM, L2DATA);

  /* L2 is controlled by target specific config and L2TAG */
  if(l2_state == MEM_RET)
  {
    l2data &= ~g_sleepTargetConfig.l2_mask;
    l2tag = 0x0;
  }
  else
  {
    l2data |= g_sleepTargetConfig.l2_mask;
    l2tag = 0xF;
  }

  HWIO_OUTF(QDSP6SS_SPMCTL_EN_MEM, L2DATA, l2data);
  HWIO_OUTF(QDSP6SS_SPMCTL_EN_MEM, L2TAG,  l2tag);
  return;
}

/** 
 * cacheLPR_l2RetentionEnter 
 *  
 * @brief Configure L2 for Retention
 */
void cacheLPR_L2RetentionEnter( uint64 wakeupTick )
{
  cacheLPR_L2ConfigureSPM(MEM_RET);
  return;
}

/** 
 * cacheLPR_l2NonRetentionEnter 
 *  
 * @brief Configure L2 for Non-Retention
 */
void cacheLPR_L2NonRetentionEnter( uint64 wakeupTick )
{
  cacheLPR_L2ConfigureSPM(MEM_NORET);
  return;
}

/** 
 * cacheLPR_TCMRetentionEnter 
 *  
 * @brief Configure TCM for Retention
 */
void cacheLPR_TCMRetentionEnter( uint64 wakeupTick )
{
  cacheLPR_TCMConfigureSPM(MEM_RET);
  return;
}

/** 
 * cacheLPR_TCMNonRetentionEnter 
 *  
 * @brief Configure TCM for Non-Retention
 */
void cacheLPR_TCMNonRetentionEnter( uint64 wakeupTick )
{
  cacheLPR_TCMConfigureSPM(MEM_NORET);
  return;
}

/** 
 * cacheLPR_commonExit 
 *  
 * @brief Common exit function
 */
void cacheLPR_commonExit( void )
{
  return;
}

/** 
 * cacheLPR_TCMNonRetentionExit
 *  
 * @brief Configure TCM for retention on exit
 */
void cacheLPR_TCMNonRetentionExit( void )
{
  cacheLPR_TCMConfigureSPM(MEM_RET);
}

/** 
 * cacheLPR_L2NonRetentionExit
 *  
 * @brief Configure L2 for retention on exit
 */
void cacheLPR_L2NonRetentionExit( void )
{
  cacheLPR_L2ConfigureSPM(MEM_RET);
}

