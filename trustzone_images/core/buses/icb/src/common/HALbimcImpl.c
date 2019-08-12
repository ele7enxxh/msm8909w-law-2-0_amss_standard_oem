/**

@file   HALbimcImpl.c

@brief  Implementation file for HAL BIMC, initial core rev.
        
===========================================================================

                             Edit History

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/common/HALbimcImpl.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2013/10/30   sds     Fix Remap/Slave segment size in getter function when
                     maximum sized region is used.
2013/10/10   sds     Add API to check if remapper is actually supported.
2013/09/27   sds     BKE Grant Count is in bytes, not beats (SWI incorrect).
2013/09/23   sds     Fixes for BIMC remapper support
2013/07/23   sds     Add BIMC remapper support, new data APIs
2013/04/22   sds     Correct BKE implementation
2013/02/28   sds     Added LPAE support
2011/08/15   sds     Created 

===========================================================================
                Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALbimci.h"
#include "HALbimcHwio.h"
#include "HALbimcHwioGeneric.h"
#include <string.h>

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define BIMC_HW_REV_LESS(major,minor,step) \
  (((int64_t)BIMC_IN(pInfo->pBaseAddr,MISC_HW_VERSION) - \
    (int64_t)(BIMC_OUTFC((major),MISC_HW_VERSION,MAJOR) | \
              BIMC_OUTFC((minor),MISC_HW_VERSION,MINOR) | \
              BIMC_OUTFC((step), MISC_HW_VERSION,STEP))) < 0LL)
#define BIMC_VERIFY_HW_REV_GREATER(major,minor,step) \
  if(BIMC_HW_REV_LESS((major),(minor),(step))) return

#define BIMC_VERIFY_SEGMENT_IDX(idx) if(idx >= 3) return
#define BIMC_VERIFY_REMAP_IDX(idx)   if(idx >= 4) return
#define BIMC_VERIFY_REMAP_OFFS(val) \
  if( ((val) >> 28) > 127 || ((val) >> 28) < -128) return

#define UPPER(val) (uint32_t)((val) >> 32)
#define LOWER(val) (uint32_t)((val) & 0xFFFFFFFF)

#define GRANULARITY_1KB 10
#define REMAP_256MB_DIV(val) ((int8_t)(((val) >> 28) & 0xFF))

#define SEGMENT_ADDR_MASK_UPPER \
  ((uint64_t)(BIMC_FMSK(SEGMENTn_ADDR_BASEm_UPPER, BASE_35_32)) << 32)

#define SEGMENT_ADDR_MASK_LOWER \
  ((uint64_t)BIMC_FMSK(SEGMENTn_ADDR_BASEm_LOWER, BASE_31_20))

#define SEGMENT_ADDR_MASK (SEGMENT_ADDR_MASK_UPPER | SEGMENT_ADDR_MASK_LOWER)

#define SEGMENT_SIZE_MASK_UPPER \
  ((uint64_t)(BIMC_FMSK(SEGMENTn_ADDR_BASEm_UPPER, BASE_35_32)) << 32)

#define SEGMENT_SIZE_MASK_LOWER \
  ((uint64_t)0xFFFFFFFFULL)

#define BKE_HEALTH_MASK \
  (BIMC_FMSK(M_BKE_HEALTH_0_CONFIG, LIMIT_COMMANDS) | \
   BIMC_FMSK(M_BKE_HEALTH_0_CONFIG, AREQPRIORITY) | \
   BIMC_FMSK(M_BKE_HEALTH_0_CONFIG, PRIORITYLVL))

#define BKE_HEALTH_VAL(limit,areq,plvl) \
  (BIMC_OUTFC( (limit), M_BKE_HEALTH_0_CONFIG, LIMIT_COMMANDS ) | \
   BIMC_OUTFC( (areq),  M_BKE_HEALTH_0_CONFIG, AREQPRIORITY ) | \
   BIMC_OUTFC( (plvl),  M_BKE_HEALTH_0_CONFIG, PRIORITYLVL ))

#define ENABLE(val) ((val)==true?1:0)

/* Grant period definitions. */
#define MAX_GRANT_PERIOD         (BIMC_FMSK(M_BKE_GRANT_PERIOD, GRANT_PERIOD) >> BIMC_SHFT(M_BKE_GRANT_PERIOD, GRANT_PERIOD))

/* Grant count definitions. */
#define MAX_GRANT_COUNT          (BIMC_FMSK(M_BKE_GRANT_COUNT, GRANT_COUNT) >> BIMC_SHFT(M_BKE_GRANT_COUNT, GRANT_COUNT))

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** 
@copydoc HAL_bimc_Init
*/
void bimc_Init( HAL_bimc_InfoType *pInfo )
{
  uint32_t uReg = BIMC_IN(pInfo->pBaseAddr,BRIC_CONFIGURATION_INFO_0);
  pInfo->devParams.uBusId      = BIMC_INFC(uReg,BRIC_CONFIGURATION_INFO_0,BUSID);
  pInfo->devParams.uNumMasters = BIMC_INFC(uReg,BRIC_CONFIGURATION_INFO_0,NUM_MPORTS);
  pInfo->devParams.uNumSlaves  = BIMC_INFC(uReg,BRIC_CONFIGURATION_INFO_0,NUM_SWAYS);

  /* Width in hardware is in bits. */
  pInfo->devParams.uAddrWidth  = BIMC_INFC(uReg,BRIC_CONFIGURATION_INFO_0,ADDR_WIDTH);
  pInfo->devParams.uDataWidth  = BIMC_INF(pInfo->pBaseAddr,BRIC_CONFIGURATION_INFO_1,DATA_WIDTH);
}

/**
@copydoc HAL_bimc_Reset
*/
void bimc_Reset( HAL_bimc_InfoType *pInfo )
{
  (void)pInfo;
}

/**
@copydoc HAL_bimc_Save
*/
void bimc_Save( HAL_bimc_InfoType *pInfo )
{
  (void)pInfo;
}

/**
@copydoc HAL_bimc_Restore
*/
void bimc_Restore( HAL_bimc_InfoType *pInfo )
{
  (void)pInfo;
}

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_bimc_SetSlaveSegment
*/
void bimc_SetSlaveSegment
(
  HAL_bimc_InfoType *        pInfo,
  uint32_t                   uSlaveIdx,
  uint32_t                   uSegmentIdx,
  HAL_bimc_SlaveSegmentType *pSegment
)
{
  uint64_t uAddrVal;
  uint64_t uMaskVal;
  bool bLPAE;

  /* Verify the segment is in range for this core. */
  BIMC_VERIFY_SEGMENT_IDX(uSegmentIdx);

  /* Get the address width so we know if this configuration supports LPAE.
   * At this point, LPAE means ADDR_WIDTH > 32 bits. */
  bLPAE = BIMC_INF(pInfo->pBaseAddr, BRIC_CONFIGURATION_INFO_0, ADDR_WIDTH) > 32;

  /* Check to see if the segment should be enabled, or cleared and disabled. */
  if( pSegment->bEnable )
  {
    uAddrVal = (((((uint64_t)BIMC_RMSK(SEGMENTn_ADDR_BASEm_UPPER)) << 32) | 
                   (uint64_t)BIMC_FMSK(SEGMENTn_ADDR_BASEm_LOWER, BASE_31_20)) &
                pSegment->uStartAddr) | BIMC_FMSK(SEGMENTn_ADDR_BASEm_LOWER, ENABLE);
    uMaskVal = ~(pSegment->uSegmentSize - 1);
    uMaskVal &= ((((uint64_t)BIMC_RMSK(SEGMENTn_ADDR_MASKm_UPPER)) << 32) | 
                   (uint64_t)BIMC_FMSK(SEGMENTn_ADDR_MASKm_LOWER, MASK_31_20));

    /* Modify based on segment type. */
    switch( pSegment->type )
    {
      case BIMC_SEGMENT_TYPE_ADDITIVE:
        switch( pSegment->interleave )
        {
          case BIMC_INTERLEAVE_NONE:
            /* Do nothing */
            break;
          case BIMC_INTERLEAVE_ODD:
            uAddrVal |= BIMC_FMSK(SEGMENTn_ADDR_BASEm_LOWER, BASE_10);
            /* Intentional fall through */
          case BIMC_INTERLEAVE_EVEN:
            uMaskVal |= BIMC_FMSK(SEGMENTn_ADDR_MASKm_LOWER, MASK_10);
            break;
          default:
            /* Invalid interleave type */
            return;
        }
        break;
      case BIMC_SEGMENT_TYPE_SUBTRACTIVE:
        uAddrVal |= BIMC_FMSK(SEGMENTn_ADDR_BASEm_LOWER, TYPE);
        break;
    default:
      /* Invalid type */
      return;
    }
  }
  /* Disable the segment. */
  else
  {
    uAddrVal = 0;
    uMaskVal = 0;
  }

  /* Write the upper bits first, if supported. */
  if( bLPAE )
  {
    BIMC_OUTI2(pInfo->pBaseAddr, SEGMENTn_ADDR_MASKm_UPPER, uSlaveIdx, uSegmentIdx, UPPER(uMaskVal));
    BIMC_OUTI2(pInfo->pBaseAddr, SEGMENTn_ADDR_BASEm_UPPER, uSlaveIdx, uSegmentIdx, UPPER(uAddrVal));
  }
  BIMC_OUTI2(pInfo->pBaseAddr, SEGMENTn_ADDR_MASKm_LOWER, uSlaveIdx, uSegmentIdx, LOWER(uMaskVal));
  BIMC_OUTI2(pInfo->pBaseAddr, SEGMENTn_ADDR_BASEm_LOWER, uSlaveIdx, uSegmentIdx, LOWER(uAddrVal));
}

/**
@copydoc HAL_bimc_SetRemapSegment
*/
void bimc_SetRemapSegment
(
  HAL_bimc_InfoType *        pInfo,
  uint32_t                   uSegmentIdx,
  HAL_bimc_RemapSegmentType *pSegment
)
{
  uint64_t uAddrVal;
  uint64_t uMaskVal;
  uint32_t uOp0Val;
  uint32_t uOp1Val;
  bool bLPAE;

  /* Verify that we are an acceptable core revision. */
  BIMC_VERIFY_HW_REV_GREATER(2,0,0);

  /* Verify the segment is in range for this core. */
  BIMC_VERIFY_REMAP_IDX(uSegmentIdx);
  BIMC_VERIFY_REMAP_OFFS(pSegment->nSegmentOffset);

  /* Get the address width so we know if this configuration supports LPAE.
   * At this point, LPAE means ADDR_WIDTH > 32 bits. */
  bLPAE = BIMC_INF(pInfo->pBaseAddr, BRIC_CONFIGURATION_INFO_0, ADDR_WIDTH) > 32;

  /* Check to see if the segment should be enabled, or cleared and disabled. */
  if( pSegment->bEnable )
  {
    uAddrVal = (((((uint64_t)BIMC_RMSK(REMAPn_ADDR_BASE_UPPER)) << 32) | 
                   (uint64_t)BIMC_FMSK(REMAPn_ADDR_BASE_LOWER, BASE_31_28)) &
                pSegment->uStartAddr) | BIMC_FMSK(REMAPn_ADDR_BASE_LOWER, ENABLE);
    uMaskVal = ~(pSegment->uSegmentSize - 1);
    uMaskVal &= ((((uint64_t)BIMC_RMSK(REMAPn_ADDR_MASK_UPPER)) << 32) | 
                   (uint64_t)BIMC_FMSK(REMAPn_ADDR_MASK_LOWER, MASK_31_28));

    /* Update based on interleave type. */
    switch( pSegment->interleave )
    {
      case BIMC_INTERLEAVE_NONE:
        /* Do nothing */
        break;
      case BIMC_INTERLEAVE_ODD:
        uAddrVal |= BIMC_FMSK(REMAPn_ADDR_BASE_LOWER, BASE_10);
        /* Intentional fall through */
      case BIMC_INTERLEAVE_EVEN:
        uMaskVal |= BIMC_FMSK(REMAPn_ADDR_MASK_LOWER, MASK_10);
        break;
      default:
        /* Invalid interleave type */
        return;
    }

    /* Set Opcodes. */
    if( pSegment->bDeinterleave )
    {
      uOp0Val = BIMC_OUTFC(GRANULARITY_1KB, REMAPn_OP0, OPERAND) | BIMC_FMSK(REMAPn_OP0, OPCODE_ENABLE);
    }
    else
    {
      uOp0Val = 0;
    }
    if( pSegment->nSegmentOffset )
    {
      int8_t offs = REMAP_256MB_DIV(pSegment->nSegmentOffset);
      uOp1Val = BIMC_OUTFC(offs, REMAPn_OP1, OPERAND) | BIMC_FMSK(REMAPn_OP1, OPCODE_ENABLE);
    }
    else
    {
      uOp1Val = 0;
    }
  }
  /* Disable the segment. */
  else
  {
    uAddrVal = 0;
    uMaskVal = 0;
    uOp0Val  = 0;
    uOp1Val  = 0;
  }

  /* Set options first. */
  BIMC_OUTI(pInfo->pBaseAddr, REMAPn_OP0, uSegmentIdx, uOp0Val);
  BIMC_OUTI(pInfo->pBaseAddr, REMAPn_OP1, uSegmentIdx, uOp1Val);

  /* Write the upper bits first, if supported. */
  if( bLPAE )
  {
    BIMC_OUTI(pInfo->pBaseAddr, REMAPn_ADDR_BASE_UPPER, uSegmentIdx, UPPER(uAddrVal));
    BIMC_OUTI(pInfo->pBaseAddr, REMAPn_ADDR_MASK_UPPER, uSegmentIdx, UPPER(uMaskVal));
  }
  BIMC_OUTI(pInfo->pBaseAddr, REMAPn_ADDR_MASK_LOWER, uSegmentIdx, LOWER(uMaskVal));
  BIMC_OUTI(pInfo->pBaseAddr, REMAPn_ADDR_BASE_LOWER, uSegmentIdx, LOWER(uAddrVal));
}


/**
@copydoc HAL_bimc_SetSlaveClockGating
*/
void bimc_SetSlaveClockGating
(
  HAL_bimc_InfoType *       pInfo,
  uint32_t                  uSlaveIdx,
  HAL_bimc_ClockGatingType *pGating
)
{
  /* Make sure that the first block for this slave port is actually a slaveway.
   * Other types don't have this register. */
  if( BIMC_SLAVE_BLOCK_TYPE_SLAVE_WAY ==
        BIMC_INFI( pInfo->pBaseAddr, S_COMPONENT_INFO, uSlaveIdx, SUB_TYPE) )
  {
    BIMC_OUTMI( pInfo->pBaseAddr, S_SWAY_CLOCK_CTRL, uSlaveIdx,
                BIMC_FMSK(S_SWAY_CLOCK_CTRL, SLAVE_CLOCK_GATING_EN) |
                BIMC_FMSK(S_SWAY_CLOCK_CTRL, CORE_CLOCK_GATING_EN),
                (ENABLE(pGating->bCoreClockGateEn) << BIMC_SHFT(S_SWAY_CLOCK_CTRL, SLAVE_CLOCK_GATING_EN)) |
                (ENABLE(pGating->bPortClockGateEn) << BIMC_SHFT(S_SWAY_CLOCK_CTRL, CORE_CLOCK_GATING_EN)));
  }

  /* Arbiter is always present. */
  BIMC_OUTFI( pInfo->pBaseAddr, S_ARB_CLOCK_CTRL, uSlaveIdx,
              CORE_CLOCK_GATING_EN, ENABLE(pGating->bArbClockGateEn) );
}

/**
@copydoc HAL_bimc_SetMasterClockGating
*/
void bimc_SetMasterClockGating
(
  HAL_bimc_InfoType *       pInfo,
  uint32_t                  uMasterIdx,
  HAL_bimc_ClockGatingType *pGating
)
{
  BIMC_OUTMI( pInfo->pBaseAddr, M_CLOCK_CTRL, uMasterIdx,
              BIMC_FMSK(M_CLOCK_CTRL, MASTER_CLOCK_GATING_EN) |
              BIMC_FMSK(M_CLOCK_CTRL, CORE_CLOCK_GATING_EN),
              (ENABLE(pGating->bCoreClockGateEn) << BIMC_SHFT(M_CLOCK_CTRL, MASTER_CLOCK_GATING_EN)) |
              (ENABLE(pGating->bPortClockGateEn) << BIMC_SHFT(M_CLOCK_CTRL, CORE_CLOCK_GATING_EN)));
}

/**
@copydoc HAL_bimc_ArbitrationEnable
*/
void bimc_ArbitrationEnable
(
  HAL_bimc_InfoType *pInfo,
  uint32_t           uSlaveIdx,
  bool               bEnable
)
{
  if( BIMC_ARB_MODE_PRIORITY_RR == 
      BIMC_INFI( pInfo->pBaseAddr, S_ARB_CONFIGURATION_INFO_0, uSlaveIdx, FUNCTIONALITY) )
  {
    BIMC_OUTFI( pInfo->pBaseAddr, S_ARB_MODE, uSlaveIdx, PRIORITY_RR_EN, ENABLE(bEnable) );
  }
}

/**
@copydoc HAL_bimc_SetQosMode
*/
void bimc_SetQosMode
(
  HAL_bimc_InfoType *  pInfo,
  uint32_t             uMasterIdx,
  HAL_bimc_QosModeType eMode
)
{
  switch(eMode)
  {
    case BIMC_QOS_MODE_FIXED:
      BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_ENABLE,             uMasterIdx, ENABLE,                0 );
      BIMC_OUTFI( pInfo->pBaseAddr, M_PRIORITYLVL_OVERRIDE,   uMasterIdx, OVERRIDE_PRIORITYLVL,  1 ); 
      BIMC_OUTFI( pInfo->pBaseAddr, M_READ_COMMAND_OVERRIDE,  uMasterIdx, OVERRIDE_AREQPRIORITY, 1 );
      BIMC_OUTFI( pInfo->pBaseAddr, M_WRITE_COMMAND_OVERRIDE, uMasterIdx, OVERRIDE_AREQPRIORITY, 1 );
      break;
    case BIMC_QOS_MODE_BYPASS:
      BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_ENABLE,             uMasterIdx, ENABLE,                0 ); 
      BIMC_OUTFI( pInfo->pBaseAddr, M_PRIORITYLVL_OVERRIDE,   uMasterIdx, OVERRIDE_PRIORITYLVL,  0 ); 
      BIMC_OUTFI( pInfo->pBaseAddr, M_READ_COMMAND_OVERRIDE,  uMasterIdx, OVERRIDE_AREQPRIORITY, 0 );
      BIMC_OUTFI( pInfo->pBaseAddr, M_WRITE_COMMAND_OVERRIDE, uMasterIdx, OVERRIDE_AREQPRIORITY, 0 );
      break;
    case BIMC_QOS_MODE_REGULATOR:
    case BIMC_QOS_MODE_LIMITER:
      BIMC_OUTFI( pInfo->pBaseAddr, M_PRIORITYLVL_OVERRIDE,   uMasterIdx, OVERRIDE_PRIORITYLVL,  0 ); 
      BIMC_OUTFI( pInfo->pBaseAddr, M_READ_COMMAND_OVERRIDE,  uMasterIdx, OVERRIDE_AREQPRIORITY, 0 );
      BIMC_OUTFI( pInfo->pBaseAddr, M_WRITE_COMMAND_OVERRIDE, uMasterIdx, OVERRIDE_AREQPRIORITY, 0 );
      BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_ENABLE,             uMasterIdx, ENABLE,                1 );
      break;
    default:
      /* Unknown mode, do nothing */
      break;
  }
}

/**
@copydoc HAL_bimc_SetQosPriority
*/
void bimc_SetQosPriority
(
  HAL_bimc_InfoType *       pInfo,
  uint32_t                  uMasterIdx,
  HAL_bimc_QosModeType      eMode,
  HAL_bimc_QosPriorityType *pPriority
)
{
  switch(eMode)
  {
    case BIMC_QOS_MODE_FIXED:
      BIMC_OUTFI( pInfo->pBaseAddr, M_PRIORITYLVL_OVERRIDE,   uMasterIdx, PRIORITYLVL,  pPriority->fixed.uPriorityLvl );
      BIMC_OUTFI( pInfo->pBaseAddr, M_READ_COMMAND_OVERRIDE,  uMasterIdx, AREQPRIORITY, pPriority->fixed.uAReqPriorityRead );
      BIMC_OUTFI( pInfo->pBaseAddr, M_WRITE_COMMAND_OVERRIDE, uMasterIdx, AREQPRIORITY, pPriority->fixed.uAReqPriorityWrite );
      break;
    case BIMC_QOS_MODE_REGULATOR:
    case BIMC_QOS_MODE_LIMITER:
      BIMC_OUTMI( pInfo->pBaseAddr, M_BKE_HEALTH_3_CONFIG,  uMasterIdx,
                  BKE_HEALTH_MASK,
                  BKE_HEALTH_VAL( pPriority->regulator.health3.bLimitCommands,
                                  pPriority->regulator.health3.uAReqPriority,
                                  pPriority->regulator.health3.uPriorityLvl ) );
      BIMC_OUTMI( pInfo->pBaseAddr, M_BKE_HEALTH_2_CONFIG, uMasterIdx,
                  BKE_HEALTH_MASK,
                  BKE_HEALTH_VAL( pPriority->regulator.health2.bLimitCommands,
                                  pPriority->regulator.health2.uAReqPriority,
                                  pPriority->regulator.health2.uPriorityLvl ) );
      BIMC_OUTMI( pInfo->pBaseAddr, M_BKE_HEALTH_1_CONFIG, uMasterIdx,
                  BKE_HEALTH_MASK,
                  BKE_HEALTH_VAL( pPriority->regulator.health1.bLimitCommands,
                                  pPriority->regulator.health1.uAReqPriority,
                                  pPriority->regulator.health1.uPriorityLvl ) );
      BIMC_OUTMI( pInfo->pBaseAddr, M_BKE_HEALTH_0_CONFIG, uMasterIdx,
                  BKE_HEALTH_MASK,
                  BKE_HEALTH_VAL( pPriority->regulator.health0.bLimitCommands,
                                  pPriority->regulator.health0.uAReqPriority,
                                  pPriority->regulator.health0.uPriorityLvl ) );
      break;
    case BIMC_QOS_MODE_BYPASS:
      /* Nothing to do. */
    default:
      /* Unknown mode, do nothing */
      break;
  }
}

/**
@copydoc HAL_bimc_SetQosBandwidth
*/
void bimc_SetQosBandwidth
(
  HAL_bimc_InfoType *        pInfo,
  uint32_t                   uMasterIdx,
  HAL_bimc_QosBandwidthType *pBandwidth
)
{
  uint32_t uEnabled;

  /* Validate Qos freq to prevent divide by zero. */
  if( !pInfo->uQosFreq )
  {
    /* Bad config data! Do nothing. */
    return;
  }

  /* Get the enable bit for the BKE before we program the period. */
  uEnabled = BIMC_INFI( pInfo->pBaseAddr, M_BKE_ENABLE, uMasterIdx, ENABLE );

  /* Only perform calculations if there's a requested bandwidth and window. */
  if( pBandwidth->uBandwidth && pBandwidth->uWindowSize )
  {
    /* First, calculate the nominal grant period as defined by the
    ** requested window size.
    ** It's important to remember the input units (hence the 1e6 division).
    ** Ceiling this value to max grant period.
    */
    uint64_t uGrantPeriodNominal = MIN(MAX_GRANT_PERIOD, 
                                      (pBandwidth->uWindowSize * pInfo->uQosFreq) / 1000000);

    /* Now, calculate our maximum window size as defined by our bandwidth request,
    ** since we need to make sure we don't overflow the grant counter.
    ** Again, note the unit differences (KHz, MB/s).
    */
    uint64_t uGrantPeriodRequired  = (MAX_GRANT_COUNT * pInfo->uQosFreq * 1000ULL) / (pBandwidth->uBandwidth);

    /* Use the smaller of the two grant period caluculations for the next step.
    ** Note that it is safe to cast down to 32-bit, we know the result of the
    ** MIN operation will fit.
    ** If the nominal grant period is zero, use the required grant period.
    */
    uint32_t uGrantPeriod = (uint32_t)MIN(uGrantPeriodNominal, uGrantPeriodRequired);

    /* Calculate bandwidth in grants and ceiling. */
    uint32_t uGrantCount = (uint32_t) MIN(MAX_GRANT_COUNT, 
                              CEILING_DIV(pBandwidth->uBandwidth * uGrantPeriod,
                                          pInfo->uQosFreq * 1000));

    /* Calculate thresholds. */
    int64_t uThresholdHighOffs   = pBandwidth->uThresholdHigh - pBandwidth->uBandwidth;
    int64_t uThresholdMediumOffs = pBandwidth->uThresholdMedium - pBandwidth->uBandwidth;
    int64_t uThresholdLowOffs    = pBandwidth->uThresholdLow - pBandwidth->uBandwidth;
    int16_t uThresholdHigh   = (uThresholdHighOffs * uGrantPeriod)/(pInfo->uQosFreq * 1000);
    int16_t uThresholdMedium = (uThresholdMediumOffs * uGrantPeriod)/(pInfo->uQosFreq * 1000);
    int16_t uThresholdLow    = (uThresholdLowOffs * uGrantPeriod)/(pInfo->uQosFreq * 1000);

    /* HPG states that the BKE *must* be disabled when programming the period. */
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_ENABLE, uMasterIdx, ENABLE, 0 );

    /* Write all the registers we just calculated. */
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_GRANT_PERIOD,     uMasterIdx, GRANT_PERIOD, uGrantPeriod );
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_GRANT_COUNT,      uMasterIdx, GRANT_COUNT,  uGrantCount );
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_THRESHOLD_HIGH,   uMasterIdx, THRESHOLD,    uThresholdHigh );
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_THRESHOLD_MEDIUM, uMasterIdx, THRESHOLD,    uThresholdMedium );
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_THRESHOLD_LOW,    uMasterIdx, THRESHOLD,    uThresholdLow );
  }
  else
  {
    /* Clear the bandwidth registers. */
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_GRANT_PERIOD,     uMasterIdx, GRANT_PERIOD, 0 );
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_GRANT_COUNT,      uMasterIdx, GRANT_COUNT,  0 );
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_THRESHOLD_HIGH,   uMasterIdx, THRESHOLD,    0 );
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_THRESHOLD_MEDIUM, uMasterIdx, THRESHOLD,    0 );
    BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_THRESHOLD_LOW,    uMasterIdx, THRESHOLD,    0 );
  }

  /* Set enable bit back to what it was before we forcibly disabled it */
  BIMC_OUTFI( pInfo->pBaseAddr, M_BKE_ENABLE, uMasterIdx, ENABLE, uEnabled );
}

/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
/**
@copydoc HAL_bimc_GetDeviceParameters
*/
void bimc_GetDeviceParameters
(
  HAL_bimc_InfoType *        pInfo,
  HAL_bimc_DeviceParamsType *pDevParams
)
{
  *pDevParams = pInfo->devParams;
}

/**
@copydoc HAL_bimc_GetMasterParameters
*/
void bimc_GetMasterParameters
(
  HAL_bimc_InfoType *            pInfo,
  uint32_t                       uMasterIdx, 
  HAL_bimc_MasterPortParamsType *pPortParams
)
{
  pPortParams->uConnMask = BIMC_INI( pInfo->pBaseAddr, M_CONFIGURATION_INFO_1, uMasterIdx );
  pPortParams->uDataWidth = BIMC_INFI( pInfo->pBaseAddr, M_CONFIGURATION_INFO_2, uMasterIdx, M_DATA_WIDTH );
}

/**
@copydoc HAL_bimc_GetSlaveParameters
*/
void bimc_GetSlaveParameters
(
  HAL_bimc_InfoType *           pInfo,
  uint32_t                      uSlaveIdx,
  HAL_bimc_SlavePortParamsType *pPortParams
)
{
  /* Determine which type of block exists at the top of the slave registers */
  if( BIMC_SLAVE_BLOCK_TYPE_SCMO == BIMC_INFI( pInfo->pBaseAddr, S_COMPONENT_INFO, uSlaveIdx, SUB_TYPE ) )
  {
    pPortParams->uDataWidth = BIMC_INFI( pInfo->pBaseAddr, S_SCMO_CONFIGURATION_INFO_0, uSlaveIdx, DATA_WIDTH );
  }
  else
  {
    pPortParams->uDataWidth = BIMC_INFI( pInfo->pBaseAddr, S_SWAY_CONFIGURATION_INFO_2, uSlaveIdx, S_DATA_WIDTH );
  }
  pPortParams->eArbMode = (HAL_bimc_ArbModeType)BIMC_INFI( pInfo->pBaseAddr, S_ARB_CONFIGURATION_INFO_0, uSlaveIdx, FUNCTIONALITY );
  pPortParams->uConnMask = BIMC_INI( pInfo->pBaseAddr, S_ARB_CONFIGURATION_INFO_1, uSlaveIdx );
}

/**
@copydoc HAL_bimc_GetArbitrationEnable
*/
bool bimc_GetArbitrationEnable
(
  HAL_bimc_InfoType *pInfo,
  uint32_t           uSlaveIdx
)
{
  bool enable;

  if( BIMC_ARB_MODE_PRIORITY_RR == 
      BIMC_INFI( pInfo->pBaseAddr, S_ARB_CONFIGURATION_INFO_0, uSlaveIdx, FUNCTIONALITY) )
  {
    enable = (bool)BIMC_INFI( pInfo->pBaseAddr, S_ARB_MODE, uSlaveIdx, PRIORITY_RR_EN );
  } 
  else
  {
    enable = false;
  }

  return enable;
}

/**
@copydoc HAL_bimc_GetSlaveSegment
*/
void bimc_GetSlaveSegment
(
  HAL_bimc_InfoType *        pInfo,
  uint32_t                   uSlaveIdx,
  uint32_t                   uSegmentIdx,
  HAL_bimc_SlaveSegmentType *pSegment
)
{
  /* Verify the segment is in range for this core. */
  BIMC_VERIFY_SEGMENT_IDX(uSegmentIdx);

  /* Check to see if the segment is even enabled. */
  if( BIMC_INFI2(pInfo->pBaseAddr, SEGMENTn_ADDR_BASEm_LOWER, uSlaveIdx, uSegmentIdx, ENABLE) )
  {
    /* Get the address width so we know if this configuration supports LPAE.
    * At this point, LPAE means ADDR_WIDTH > 32 bits. */
    bool bLPAE         = BIMC_INF(pInfo->pBaseAddr, BRIC_CONFIGURATION_INFO_0, ADDR_WIDTH) > 32;
    uint64_t uAddrVal  = BIMC_INI2(pInfo->pBaseAddr, SEGMENTn_ADDR_BASEm_LOWER, uSlaveIdx, uSegmentIdx);
    uint64_t uMaskVal  = BIMC_INI2(pInfo->pBaseAddr, SEGMENTn_ADDR_MASKm_LOWER, uSlaveIdx, uSegmentIdx);
    uint32_t uAddrBase = BIMC_INFC(uAddrVal, SEGMENTn_ADDR_BASEm_LOWER, BASE_10);
    uint32_t uMaskBase = BIMC_INFC(uMaskVal, SEGMENTn_ADDR_MASKm_LOWER, MASK_10);
    uint64_t uSegMask  = SEGMENT_ADDR_MASK_LOWER;
    uint64_t uSizeMask = SEGMENT_SIZE_MASK_LOWER;
    uMaskVal &= BIMC_FMSK(SEGMENTn_ADDR_MASKm_LOWER, MASK_31_20);

    /* Read the top bits, if LPAE is supported. */
    if( bLPAE )
    {
      uAddrVal |= (uint64_t)BIMC_INI2(pInfo->pBaseAddr, SEGMENTn_ADDR_BASEm_UPPER, uSlaveIdx, uSegmentIdx) << 32;
      uMaskVal |= (uint64_t)BIMC_INI2(pInfo->pBaseAddr, SEGMENTn_ADDR_MASKm_UPPER, uSlaveIdx, uSegmentIdx) << 32;
      uSegMask |= SEGMENT_ADDR_MASK_UPPER;
      uSizeMask |= SEGMENT_SIZE_MASK_UPPER;
    }

    pSegment->bEnable      = true;
    pSegment->uStartAddr   = uAddrVal & uSegMask;
    pSegment->uSegmentSize = (~(uMaskVal) & uSizeMask) + 1;
    pSegment->type         = (HAL_bimc_SegmentType)BIMC_INFC(uAddrVal, SEGMENTn_ADDR_BASEm_LOWER, TYPE);

    /* Determine interleaving mode. */
    if( uAddrBase & uMaskBase )
    {
      pSegment->interleave = BIMC_INTERLEAVE_ODD;
    }
    else if( uMaskBase )
    {
      pSegment->interleave = BIMC_INTERLEAVE_EVEN;
    }
    else
    {
      pSegment->interleave = BIMC_INTERLEAVE_NONE;
    }
  }
  else
  {
    pSegment->bEnable      = false;
    pSegment->uStartAddr   = 0;
    pSegment->uSegmentSize = 0;
    pSegment->type         = BIMC_SEGMENT_TYPE_ADDITIVE;
    pSegment->interleave   = BIMC_INTERLEAVE_NONE;
  }
}

/**
@copydoc HAL_bimc_GetRemapSegment
*/
void bimc_GetRemapSegment
(
  HAL_bimc_InfoType *        pInfo,
  uint32_t                   uSegmentIdx,
  HAL_bimc_RemapSegmentType *pSegment
)
{
  /* Verify that we are an acceptable core revision. */
  BIMC_VERIFY_HW_REV_GREATER(2,0,0);

  /* Verify the segment is in range for this core. */
  BIMC_VERIFY_REMAP_IDX(uSegmentIdx);

  /* Check to see if the segment is even enabled. */
  if( BIMC_INFI(pInfo->pBaseAddr, REMAPn_ADDR_BASE_LOWER, uSegmentIdx, ENABLE) )
  {
    /* Get the address width so we know if this configuration supports LPAE.
    * At this point, LPAE means ADDR_WIDTH > 32 bits. */
    bool bLPAE = BIMC_INF(pInfo->pBaseAddr, BRIC_CONFIGURATION_INFO_0, ADDR_WIDTH) > 32;
    uint64_t uAddrVal  = BIMC_INI(pInfo->pBaseAddr, REMAPn_ADDR_BASE_LOWER, uSegmentIdx);
    uint64_t uMaskVal  = BIMC_INI(pInfo->pBaseAddr, REMAPn_ADDR_MASK_LOWER, uSegmentIdx);
    uint32_t uOp0Val   = BIMC_INI(pInfo->pBaseAddr, REMAPn_OP0, uSegmentIdx);
    uint32_t uOp1Val   = BIMC_INI(pInfo->pBaseAddr, REMAPn_OP1, uSegmentIdx);
    uint32_t uAddrBase = BIMC_INFC(uAddrVal, REMAPn_ADDR_BASE_LOWER, BASE_10);
    uint32_t uMaskBase = BIMC_INFC(uMaskVal, REMAPn_ADDR_MASK_LOWER, MASK_10);
    uint64_t uSegMask = SEGMENT_ADDR_MASK_LOWER;
    uint64_t uSizeMask = SEGMENT_SIZE_MASK_LOWER;
    uMaskVal &= BIMC_FMSK(REMAPn_ADDR_MASK_LOWER, MASK_31_28);

    /* Read the top bits, if LPAE is supported. */
    if( bLPAE )
    {
      uAddrVal |= (uint64_t)BIMC_INI(pInfo->pBaseAddr, REMAPn_ADDR_BASE_UPPER, uSegmentIdx) << 32;
      uMaskVal |= (uint64_t)BIMC_INI(pInfo->pBaseAddr, REMAPn_ADDR_MASK_UPPER, uSegmentIdx) << 32;
      uSegMask |= SEGMENT_ADDR_MASK_UPPER;
      uSizeMask |= SEGMENT_SIZE_MASK_UPPER;
    }

    pSegment->bEnable      = true;
    pSegment->uStartAddr   = uAddrVal & uSegMask;
    pSegment->uSegmentSize = (~(uMaskVal) & uSizeMask) + 1;

    /* Determine interleaving mode. */
    if( uAddrBase & uMaskBase )
    {
      pSegment->interleave = BIMC_INTERLEAVE_ODD;
    }
    else if( uMaskBase )
    {
      pSegment->interleave = BIMC_INTERLEAVE_EVEN;
    }
    else
    {
      pSegment->interleave = BIMC_INTERLEAVE_NONE;
    }

    /* Get the segment offset and deinterleave setting. */
    if( uOp1Val & BIMC_FMSK(REMAPn_OP1, OPCODE_ENABLE) )
    {
      /* Double cast this, or we won't sign extend properly. */
      pSegment->nSegmentOffset = (int64_t)((int8_t)BIMC_INFC(uOp1Val, REMAPn_OP1, OPERAND)) << 28;
    }
    else
    {
      pSegment->nSegmentOffset = 0;
    }
    pSegment->bDeinterleave  = (uOp0Val & BIMC_FMSK(REMAPn_OP0, OPCODE_ENABLE)) != 0;
  }
  else
  {
    pSegment->bEnable        = false;
    pSegment->uStartAddr     = 0;
    pSegment->uSegmentSize   = 0;
    pSegment->interleave     = BIMC_INTERLEAVE_NONE;
    pSegment->nSegmentOffset = 0;
    pSegment->bDeinterleave  = false;
  }
}

/*
@copydoc HAL_bimc_IsRemapSupported
*/
bool bimc_IsRemapSupported
(
  HAL_bimc_InfoType *pInfo
)
{
  /* Remap is supported on HW revs 2.0.0 and above */
  if( BIMC_HW_REV_LESS(2,0,0) )
  {
    return false;
  }
  else
  {
    return true;
  }
}
