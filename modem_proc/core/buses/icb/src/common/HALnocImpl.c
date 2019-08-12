/**

@file   HALnocImpl.c.c

@brief  Implementation file for HAL NOC.
        
===========================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/HALnocImpl.c#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2015/07/23   sds     Mark save in the right place.
2015/06/13   sds     Add ExtControl support
2014/04/15   sds     Change the hardware base address to a uint8_t*.
2014/04/08   sds     Fix klockwork error in noc_Init().
2014/03/10   jc      Add new register layouts
2013/10/30   sds     Add support for additional NoC register layouts.
2013/09/17   sds     Window size can be zero.
2013/08/05   sds     Make c89 compliant for builds with non-c99 compilers
2013/04/03   sds     Add support for save/restore
2012/12/21   jc      Do not access QOS_MODEn for FIXED mode only masters.
2012/03/01   sds     Make sure to flush accumulator when updating bandwidth
                     and saturation values.
2011/08/15   sds     Created 

===========================================================================
        Copyright (c) 2011-2015 QUALCOMM Technologies, Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include "HALnoci.h"
#include "HALnocHwio.h"
#include "HALnocHwioGeneric.h"
#include "CoreIntrinsics.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
/* Bandwidth definitions */
#define BW_SCALE                    256 /* 1/256th byte per cycle unit */
#define MAX_BW_FIELD                (NOC_FMSK(QOS_BANDWIDTHn, BANDWIDTH) >> NOC_SHFT(QOS_BANDWIDTHn, BANDWIDTH))
#define MAX_BW(timebase)            noc_calc_bw_ceil(MAX_BW_FIELD,(timebase))

/* Saturation definitions. */
#define SAT_SCALE                   16 /* 16 bytes minimum unit */
#define MAX_SAT_FIELD               (NOC_FMSK(QOS_SATURATIONn, SATURATION) >> NOC_SHFT(QOS_SATURATIONn, SATURATION))
#define MAX_WINDOW(bw,timebase)     noc_calc_window((bw),MAX_SAT_FIELD,(timebase))

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
typedef struct
{
  uint32_t uQosOffset;
  uint32_t uQosBlockSize;
} HAL_noc_QosOffsetType;

/* -----------------------------------------------------------------------
** Data
** ----------------------------------------------------------------------- */
static HAL_noc_QosOffsetType noc_qos_offset[NOC_QOS_OFFSET_COUNT] =
{
  /** NOC_QOS_OFFSET_LEGACY */
  {
    0x3000,
    0x80
  },
  /** NOC_QOS_OFFSET_0x7_0x1 */
  {
    0x7000,
    0x1000
  },
  /** NOC_QOS_OFFSET_0x5000_0x80 */
  {
    0x5000,
    0x80
  },
};

/* -----------------------------------------------------------------------
** Functions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Helper Functions
** ----------------------------------------------------------------------- */
/**
@brief Calculate the bandwidth the hardware is using from the register
       values given.

@param[in] uBwField -  The value of the bandwidth field to use
@param[in] uQosFreq -  The frequency of the Qos clock

@returns The bandwidth in bytes/sec
*/
static uint64_t noc_calc_bw
(
  uint32_t uBwField,
  uint32_t uQosFreq
)
{
  return (2ULL * uQosFreq * uBwField)/(1000 * BW_SCALE) * 1000000ULL;
}

static uint64_t noc_calc_bw_ceil
(
  uint32_t uBwField,
  uint32_t uQosFreq
)
{
  return CEILING_DIV(2ULL * uQosFreq * uBwField, 1000 * BW_SCALE) * 1000000ULL;
}

/**
@brief Calculate the window size the hardware is using from the register
       values given.

@pre Expects uQosFreq to be non-zero.

@param[in] uBwVal -    Bandwidth in bytes/sec 
@param[in] uSatField - Value of the saturation field
@param[in] uQosFreq -  The frequency of the Qos clock

@returns The window size in nanoseconds.

*/
uint32_t noc_calc_window
(
  uint64_t uBwVal,
  uint32_t uSatField,
  uint32_t uQosFreq
)
{
  /* Prevent divide by zero. */
  if( uBwVal && uQosFreq )
  {
	  return (uint32_t) ((1000000000000ULL * BW_SCALE * SAT_SCALE * uSatField) / (uBwVal * uQosFreq));
  }
  {
    return 0;
  }
}

/**
@brief Calculate the bandwidth field value required for the requested
       bandwidth.

@pre Expects uQosFreq to be non-zero.

@param[in] pBandwidth - The bandwidth structure
@param[in] uQosFreq   - The frequency of the Qos clock

@returns The appropriate bandwidth field value, will ceiling at max allowed
         field size.
*/
static uint32_t noc_calc_bw_field
(
  uint64_t uBandwidth,
  uint32_t uQosFreq
)
{
  uint32_t uBwField = 0;

  /* Shortcut if necessary. */ 
  if( uBandwidth )
  {
    /* Calculate field, and cap to maximum. */
    uint64_t uBandwidthCapped = MIN(uBandwidth, MAX_BW(uQosFreq));
    uBwField = (uint32_t)MIN(CEILING_DIV(BW_SCALE * uBandwidthCapped, 2 * uQosFreq * 1000), MAX_BW_FIELD);
  }

  return uBwField;
}

/**
@brief Calculate the saturation field value required for the requested
       bandwidth and window size.

@param[in] uBandwidth - Bandwidth in bytes/sec
@param[in] uWindowSize - Window size in nanoseconds
@param[in] uQosFreq   - The frequency of the Qos clock

@returns The appropriate saturation field value, will ceiling at max allowed
         field size.
*/
static uint32_t noc_calc_sat_field
(
  uint64_t uBandwidth,
  uint32_t uWindowSize,
  uint32_t uQosFreq
)
{
  uint32_t uSatField = 0;

  /* Shortcut if necessary. */ 
  if( uBandwidth )
  {
	/* Limit to max bandwidth and scale the bandwidth down to 100KB increments.
    * We run the risk of blowing out of 64-bits without it. */ 
    uint64_t uBandwidthScaled = MIN(uBandwidth, MAX_BW(uQosFreq));
    uBandwidthScaled = CEILING_DIV(uBandwidthScaled, 100000);

    /* Calculate the saturation from the window size.
    ** Window size *can* be zero.
    ** Saturation must not exceed the maximum allowed field size.
    ** Note the additional scaling factor in the division, due to units:
    ** 10 for the bandwidth being in 100KB increments (instead of MB)
    ** 1000 for the window size being in ns (instead of us)
    ** 1000 for qos freq being in KHz (instead of MHz)
    ** Total: 10000000ULL
    */
    uSatField = (uint32_t)MIN(CEILING_DIV(uBandwidthScaled * uWindowSize * uQosFreq, 10000000ULL * BW_SCALE * SAT_SCALE), MAX_SAT_FIELD);
  }

  return uSatField;
}

/* -----------------------------------------------------------------------
** Mandatory Interface
** ----------------------------------------------------------------------- */
/** 
@copydoc noc_Init
*/
void noc_Init
(
  HAL_noc_InfoType *pInfo
)
{
  /* Range check the layout offset. Make it legacy if invalid. */
  if( pInfo->eOffsetType >= NOC_QOS_OFFSET_COUNT  )
  {
    pInfo->eOffsetType = NOC_QOS_OFFSET_LEGACY;
  } 

  if( NOC_QOS_OFFSET_INVALID != pInfo->eOffsetType )
  {
    pInfo->uQosOffset = noc_qos_offset[pInfo->eOffsetType].uQosOffset;
    pInfo->uQosSize = noc_qos_offset[pInfo->eOffsetType].uQosBlockSize;
  }
  else if( 0 == pInfo->uQosSize )
  {
    pInfo->uQosOffset = noc_qos_offset[NOC_QOS_OFFSET_LEGACY].uQosOffset;
    pInfo->uQosSize = noc_qos_offset[NOC_QOS_OFFSET_LEGACY].uQosBlockSize;
  }
}

/**
@copydoc noc_Reset
*/
void noc_Reset
(
  HAL_noc_InfoType *pInfo
)
{
  (void)pInfo;
}

/**
@copydoc noc_Save
*/
void noc_Save
(
  HAL_noc_InfoType *pInfo
)
{
  /* Only try to save if we have a buffer to do so. */
  if( pInfo->pSaveBuf )
  {
    uint32_t *pSave = pInfo->pSaveBuf, uSaveIdx = 0, i;

    /* Save all QoS registers we know about. */
    for( i = 0; i < pInfo->uNumQosMasters; i++ )
    {
      /* Skip any ports we we have no allowed modes. */
      if( !pInfo->aMasters[i].uAllowedModes )
        continue;

      /* Always save priority. */
      pSave[uSaveIdx++] = NOC_INI(pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_PRIORITYn, i);

      /* If we support LIMITER/REGULATOR, save the BANDWIDTH/SATURATION registers. */
      if( pInfo->aMasters[i].uAllowedModes &
          (NOC_QOS_ALLOWED_MODE_REGULATOR | NOC_QOS_ALLOWED_MODE_LIMITER) )
      {
        pSave[uSaveIdx++] = NOC_INI(pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_BANDWIDTHn,  i);
        pSave[uSaveIdx++] = NOC_INI(pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_SATURATIONn, i);
      }

      /* If we're not FIXED mode only, save the MODE register. */
      if( NOC_QOS_ALLOWED_MODE_FIXED != pInfo->aMasters[i].uAllowedModes )
      {
        pSave[uSaveIdx++] = NOC_INI(pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_MODEn, i);
      }
    }

    /* Save any additional registers. */
    if( pInfo->uNumAddrs && pInfo->paAddrList )
    {
      for( i = 0; i < pInfo->uNumAddrs; i++ )
      {
        uint32_t *pAddr = pInfo->paAddrList[i];
        if( pAddr )
        {
          pSave[uSaveIdx++] = *pAddr;
        }
      }
    }

    /* Mark the saved buffer as containing valid information. */
    pInfo->bSaveValid = true;
  }
}

/**
@copydoc noc_Restore
*/
void noc_Restore
(
  HAL_noc_InfoType *pInfo
)
{
  /* Only try to restore if we have a buffer to do so, and it has valid data. */
  if( pInfo->pSaveBuf && pInfo->bSaveValid )
  {
    uint32_t *pSave = pInfo->pSaveBuf, uSaveIdx = 0, i;

    /* Restore all QoS registers we know about. */
    for( i = 0; i < pInfo->uNumQosMasters; i++ )
    {
      /* Skip any ports we we have no allowed modes. */
      if( !pInfo->aMasters[i].uAllowedModes )
        continue;

      /* Always restore priority. */
      NOC_OUTI(pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_PRIORITYn, i, pSave[uSaveIdx++]);

      /* If we support LIMITER/REGULATOR, restore the BANDWIDTH/SATURATION registers. */
      if( pInfo->aMasters[i].uAllowedModes &
          (NOC_QOS_ALLOWED_MODE_REGULATOR | NOC_QOS_ALLOWED_MODE_LIMITER) )
      {
        NOC_OUTI(pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_BANDWIDTHn,  i, pSave[uSaveIdx++]);
        NOC_OUTI(pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_SATURATIONn, i, pSave[uSaveIdx++]);
      }

      /* If we're not FIXED mode only, restore the MODE registers. */
      if( NOC_QOS_ALLOWED_MODE_FIXED != pInfo->aMasters[i].uAllowedModes )
      {
        NOC_OUTI(pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_MODEn, i, pSave[uSaveIdx++]);
      }
    }

    /* Restore any additional registers. */
    if( pInfo->uNumAddrs && pInfo->paAddrList )
    {
      for( i = 0; i < pInfo->uNumAddrs; i++ )
      {
        uint32_t *pAddr = pInfo->paAddrList[i];
        if( pAddr )
        {
          *pAddr = pSave[uSaveIdx++];
        }
      }
    }

    /* We've restored, clear valid flag */
    pInfo->bSaveValid = false;
  }
}

/* -----------------------------------------------------------------------
** Configuration Interface
** ----------------------------------------------------------------------- */
void noc_SetQosMode
(
  HAL_noc_InfoType *  pInfo,
  uint32_t            uMasterPort,
  HAL_noc_QosModeType mode
)
{
  /* Check to see:
    * That we're not a fixed mode only port and
    * This is a valid mode for this port. */
  if( NOC_QOS_ALLOWED_MODE_FIXED != pInfo->aMasters[uMasterPort].uAllowedModes &&
      mode < NOC_QOS_MODE_COUNT &&
      ((1 << mode) & pInfo->aMasters[uMasterPort].uAllowedModes) )
  {
    NOC_OUTFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_MODEn, uMasterPort, MODE, mode );
  }
}

void noc_SetQosPriority
(
  HAL_noc_InfoType *       pInfo,
  uint32_t                 uMasterPort,
  HAL_noc_QosModeType      eMode,
  HAL_noc_QosPriorityType *pPriority
)
{
  /* Unused currently, added to keep it similar to BIMC HAL */
  (void)eMode;

  NOC_OUTFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_PRIORITYn, uMasterPort, P1, pPriority->p1 );
  NOC_OUTFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_PRIORITYn, uMasterPort, P0, pPriority->p0 );
}

void noc_SetQosBandwidth
(
  HAL_noc_InfoType *        pInfo,
  uint32_t                  uMasterPort,
  HAL_noc_QosBandwidthType *pBandwidth
)
{
  /* Validate Qos freq to prevent divide by zero. */
  if( !pInfo->uQosFreq )
  {
    /* Bad config data! Do nothing. */
    return;
  }

  /* Bandwidth values not available if we don't support LIMITER/REGULATOR */
  if( pInfo->aMasters[uMasterPort].uAllowedModes &
      (NOC_QOS_ALLOWED_MODE_LIMITER | NOC_QOS_ALLOWED_MODE_REGULATOR))
  {
    uint32_t uBandwidthVal = noc_calc_bw_field( pBandwidth->uBandwidth, pInfo->uQosFreq);
    uint32_t uSaturationVal = noc_calc_sat_field( pBandwidth->uBandwidth,
                                                  pBandwidth->uWindowSize,
                                                  pInfo->uQosFreq);

    /* If we're in LIMITER/REGULATOR mode, first go to FIXED mode.
     * We'll need to clear the QoS accumulator. */
    uint32_t mode = NOC_INFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_MODEn, uMasterPort, MODE );
    if( NOC_QOS_MODE_REGULATOR == mode || NOC_QOS_MODE_LIMITER == mode )
    {
      NOC_OUTFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_MODEn, uMasterPort, MODE, NOC_QOS_MODE_FIXED );
    }

    /* Write new bandwidth and saturation values. */
    NOC_OUTFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_BANDWIDTHn,  uMasterPort, BANDWIDTH, uBandwidthVal );
    NOC_OUTFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_SATURATIONn, uMasterPort, SATURATION, uSaturationVal );

    /* Set mode back to what it was. */
    NOC_OUTFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_MODEn, uMasterPort, MODE, mode );
  }
}

void noc_SetQosExtControl
(
  HAL_noc_InfoType *         pInfo,
  uint32_t                   uMasterPort,
  HAL_noc_QosExtControlType *pExtControl
)
{
  uint32_t uMask = pExtControl->uMask & NOC_RMSK(QOS_EXTCONTROLn);
  uint32_t uVal = pExtControl->uFlags & NOC_RMSK(QOS_EXTCONTROLn);

  NOC_OUTMI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_EXTCONTROLn, uMasterPort, uMask, uVal );
}


/* -----------------------------------------------------------------------
** Data Interface
** ----------------------------------------------------------------------- */
void noc_GetQosMode
(
  HAL_noc_InfoType *   pInfo,
  uint32_t             uMasterPort,
  HAL_noc_QosModeType *pMode
)
{
  /* For the time being, we'll assume that QOS ports are either:
  ** 1.) All modes allowed (query hardware)
  ** 2.) Single mode allowed (return mode allowed)
  */
  if( ALL_NOC_QOS_MODES_ALLOWED == pInfo->aMasters[uMasterPort].uAllowedModes ) 
  {
    *pMode = (HAL_noc_QosModeType)NOC_INFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_MODEn, uMasterPort, MODE);
  }
  else
  {
    *pMode = (HAL_noc_QosModeType)(31 - CoreCountLeadingZeros32( pInfo->aMasters[uMasterPort].uAllowedModes & ALL_NOC_QOS_MODES_ALLOWED));
  }
}

void noc_GetQosPriority
(
  HAL_noc_InfoType *       pInfo,
  uint32_t                 uMasterPort,
  HAL_noc_QosPriorityType *pPriority
)
{
  pPriority->p1 = NOC_INFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_PRIORITYn, uMasterPort, P1);
  pPriority->p0 = NOC_INFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_PRIORITYn, uMasterPort, P0);
}

void noc_GetQosBandwidth
(
  HAL_noc_InfoType *        pInfo,
  uint32_t                  uMasterPort,
  HAL_noc_QosBandwidthType *pBandwidth
)
{
  /* Bandwidth values not available if we don't support LIMITER/REGULATOR */
  if( pInfo->aMasters[uMasterPort].uAllowedModes &
      (NOC_QOS_ALLOWED_MODE_LIMITER | NOC_QOS_ALLOWED_MODE_REGULATOR))
  {
    uint32_t uBwVal = NOC_INFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_BANDWIDTHn, uMasterPort, BANDWIDTH);
    uint32_t uSaturation = NOC_INFI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_SATURATIONn, uMasterPort, SATURATION);

    pBandwidth->uBandwidth = noc_calc_bw( uBwVal, pInfo->uQosFreq );
    pBandwidth->uWindowSize = noc_calc_window( pBandwidth->uBandwidth, uSaturation, pInfo->uQosFreq );
  }
  else
  {
    pBandwidth->uBandwidth = 0;
    pBandwidth->uWindowSize = 0;
  }
}

void noc_GetQosExtControl
(
  HAL_noc_InfoType *         pInfo,
  uint32_t                   uMasterPort,
  HAL_noc_QosExtControlType *pExtControl
)
{
  pExtControl->uMask  = NOC_RMSK(QOS_EXTCONTROLn);
  pExtControl->uFlags = NOC_INI( pInfo->pBaseAddr, pInfo->uQosOffset, pInfo->uQosSize, QOS_EXTCONTROLn, uMasterPort );
}

