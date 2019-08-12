#ifndef ICB_RPM_SOC_DATA_H
#define ICB_RPM_SOC_DATA_H
/**

@file   icb_rpm_soc_data.h

@brief  This module provides the type definitions for ICB SoC specifc data.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/icb_rpm_soc_data.h#1 $

when         who     what, where, why
----------   ---     --------------------------------------------------------
2014/10/02   sds     Add number of ports for aggregation purposes to masters/slaves.
2014/06/20   sds     Extend request types to add latency request in addition to BW.
2014/05/20   sds     Change the hardware base address to a uint8_t*.
2014/01/15   sds     Remove unused threshold keys, and support threshold percentages.
2013/11/25   sds     Add support for configurable aggregation schemes.
2012/01/06   sds     Created 

===========================================================================
                Copyright (c) 2012-2014 Qualcomm Technologies Incorporated.
                           All Rights Reserved.
                        QUALCOMM Proprietary/GTDR
===========================================================================
*/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "HALbimc.h"
#include "HALnoc.h"
#include "icbid.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/* Slave segment definition structure */
typedef enum
{
  ICB_BUS_BIMC = 0,
  ICB_BUS_NOC,
  ICB_BUS_VIRT,
} icb_bus_type;

typedef enum
{
  /** Default scheme:
   * max(max(IB), sum(AB)/ports)/bus_width */
  ICB_AGG_SCHEME_LEGACY = 0,

  /** Scheme 0:
   * max(sum(AB)*P0, max(IB))/ports/bus_width */
  ICB_AGG_SCHEME_0
} icb_agg_type;

/* Bus definition structure */
typedef struct
{
  char *       pszBaseName;
  uint8_t *    pBaseAddr;
  icb_bus_type busType;
  uint32_t     uDataWidth;
  void *       pInfo; /* Expected to be BIMC or NOC info type. */
  icb_agg_type aggType; /**< Aggregation scheme */
  uint32_t     uP0;     /**< Aggregation scheme parameter 0 */
  uint32_t     uP1;     /**< Aggregation scheme parameter 1 */
} icb_bus_def_type;

typedef struct
{
  uint64_t uBandwidth;        /**< Bandwidth requirement in bytes per second */
  uint32_t uThresholdHighPct; /**< Percentage of requested bandwidth to use as high threshold. */
  uint32_t uThresholdMedPct;  /**< Percentage of requested bandwidth to use as medium threshold. */
  uint32_t uThresholdLowPct;  /**< Percentage of requested bandwidth to use as low threshold. */
} icb_bimc_bw_def_type;

/* Master definition structures */
typedef struct
{
  /* Clock gating */
  HAL_bimc_ClockGatingType bimcClockGate;

  /* Arbitration mode. */
  HAL_bimc_QosModeType bimcMode;

  /* Priority definition (if necessary) */
  HAL_bimc_QosPriorityType bimcPriority;

  /* Bandwidth requirement (if necessary) */
  icb_bimc_bw_def_type bimcBandwidth;
} icb_bimc_master_defaults_type;

typedef struct
{
  /* Arbitration mode. */
  HAL_noc_QosModeType  nocMode;

  /* Priority definition (if necessary) */
  HAL_noc_QosPriorityType  nocPriority;

  /* Bandwidth requirement (if necessary) */
  HAL_noc_QosBandwidthType  nocBandwidth;
} icb_noc_master_defaults_type;

typedef struct
{
  const ICBId_MasterType  masterId;
  const icb_bus_def_type *pBus;
  const uint32_t          uNumPorts;
  const uint32_t *        pPortIds;
  const uint32_t *        pQosPortIds;
  const uint32_t          uWindowSize;
  const void *            pDefaults;
  const uint32_t          uNumAggPorts;
} icb_master_def_type;

/* Slave definition structures */
typedef struct
{
  /* BIMC only. */
  HAL_bimc_ClockGatingType bimcClockGate;
  bool                     bArbEn;
} icb_bimc_slave_defaults_type;

typedef struct
{
  const ICBId_SlaveType   slaveId;
  const icb_bus_def_type *pBus;
  const uint32_t          uNumPorts;
  const uint32_t *        pPortIds;
  const void *            pDefaults;
  const bool              bMonitored;
  const uint32_t          uNumAggPorts;
} icb_slave_def_type;
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ICB_RPM_SOC_DATA_H */
