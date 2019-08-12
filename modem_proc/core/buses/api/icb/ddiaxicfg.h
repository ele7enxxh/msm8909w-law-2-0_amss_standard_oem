#ifndef __DDIAXICFG_H__
#define __DDIAXICFG_H__
/**
 * @file ddiaxicfg.h
 * @note External header file. API 2.0 for Bus Configuration. Not compatible with API 1.0
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/api/icb/ddiaxicfg.h#1 $ 
 * $DateTime: 2016/12/13 07:59:23 $ 
 * $Author: mplcsds1 $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 02/26/2015   sds     Add QoS override API
 * 06/18/2014   sds     Add lockless halt/unhalt APIs to be used during error handling.
 * 02/16/2014   sds     Add port halt/unhalt APIs.
 * 11/28/2011   dj      Branched from rev 1.0 and updated to rev 2.0 
 *  
 *  
 * Copyright (c) 2011-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 */ 
#include "icbid.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** AxiCfg return types. */
typedef enum
{
  AXICFG_SUCCESS           = 0,
  AXICFG_ERROR             = 1, /**< Any error not described below */
  AXICFG_ERROR_UNSUPPORTED = 2, /**< Operation unsupported on this target */
  AXICFG_ERROR_INVALID     = 3, /**< Invalid parameter */
  AXICFG_ERROR_TIMEOUT     = 4, /**< Timeout encountered during operation. */
  AXICFG_RETURNCODE_SIZE   = 0x7FFFFFFF,
} AxiCfg_ReturnCodeType;

/** Clock gating type.
 *  Note: Not all fields apply to all ports.
 */
typedef struct
{
  bool bCoreClockGateEn; /**< Gating of the registers on the clock of the core behind the port. */
  bool bArbClockGateEn;  /**< Gating of the arbiter on the port. */ 
  bool bPortClockGateEn; /**< Gating of the registers on the bus core clock. */
} AxiCfg_ClockGatingType;

/* Rule structures. */
typedef enum
{
  AXICFG_BW_REQ_TYPE_DEFAULT   = 0,        /**< Dynamic, non-thresholded request. */
  AXICFG_BW_REQ_TYPE_STATIC    = (1 << 0), /**< Static request. */
  AXICFG_BW_REQ_TYPE_THRESHOLD = (1 << 1), /**< Thresholded request. */
  AXICFG_BW_REQ_TYPE_COUNT,
  AXICFG_BW_REQ_TYPE_WIDTH     = 0x7FFFFFFF,
} AxiCfg_BWReqType;

typedef struct
{
  uint64_t uBandwidth;        /**< Bandwidth requirement in bytes per second */
  uint32_t uWindowSize;       /**< Size of the qos window, in ns */
  uint32_t uThresholdHighPct; /**< Percentage of requested bandwidth to use as high threshold. */
  uint32_t uThresholdMedPct;  /**< Percentage of requested bandwidth to use as medium threshold. */
  uint32_t uThresholdLowPct;  /**< Percentage of requested bandwidth to use as low threshold. */
} AxiCfg_BWType;

typedef enum
{
  AXICFG_QOS_MODE_FIXED     = 0,
  AXICFG_QOS_MODE_LIMITER   = 1,
  AXICFG_QOS_MODE_BYPASS    = 2,
  AXICFG_QOS_MODE_REGULATOR = 3,
  AXICFG_QOS_MODE_COUNT,
  AXICFG_QOS_MODE_WIDTH     = 0x7FFFFFFF,
} AxiCfg_QosModeType;

typedef struct
{
  bool     bLimitCommands;
  uint32_t uAReqPriority;
  uint32_t uPriorityLvl;
} AxiCfg_QosHealthType;

typedef struct
{
  /** NoC priority modes */
  struct
  {
    uint32_t p1; /* High/Read priority, depending on the mode */
    uint32_t p0; /* Low/Write priority, depending on the mode */
  } noc;

  /** BIMC fixed priority mode */
  struct
  {
    uint32_t uPriorityLvl;
    uint32_t uAReqPriorityRead;
    uint32_t uAReqPriorityWrite;
  } bimc_fixed;

  /** BIMC Regulator/Limiter modes */
  struct
  {
    AxiCfg_QosHealthType health3;
    AxiCfg_QosHealthType health2;
    AxiCfg_QosHealthType health1;
    AxiCfg_QosHealthType health0;
  } bimc_regulator;
} AxiCfg_PriorityType;

/* QOS policy definition */
typedef struct
{
  AxiCfg_BWReqType    reqType;
  AxiCfg_QosModeType  mode[2];
  AxiCfg_PriorityType priority;
  uint64_t            bwThreshold;
  AxiCfg_BWType       bandwidth;
} AxiCfg_QosPolicyType;

/**
  @brief Configure the clock gating for the indicated master.

  @param[in] eMaster    The master port identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetMasterClockGating( ICBId_MasterType eMaster, AxiCfg_ClockGatingType * pGating);

/**
  @brief Configure the clock gating for the indicated slave.

  @param[in] eSlave     The slave way identifier
  @param[in] pGating    A pointer to the clock gating configuration
 */
void AxiCfg_SetSlaveClockGating( ICBId_SlaveType eSlave, AxiCfg_ClockGatingType * pGating);

/**
  @brief Attempt to halt the specified master port.

  @param[in] eMaster  The master port identifier
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_HaltMasterPort( ICBId_MasterType eMaster );

/**
  @brief Attempt to halt the specified master port.
         MUST ONLY BE USED IN SINGLE THREADED ERROR CONTEXT.

  @param[in] eMaster  The master port identifier
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_HaltMasterPort_OnError( ICBId_MasterType eMaster );

/**
  @brief Attempt to unhalt the specified master port.

  @param[in] eMaster  The master port identifier
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_UnhaltMasterPort( ICBId_MasterType eMaster );

/**
  @brief Attempt to unhalt the specified master port.
         MUST ONLY BE USED IN SINGLE THREADED ERROR CONTEXT.

  @param[in] eMaster  The master port identifier
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_UnhaltMasterPort_OnError( ICBId_MasterType eMaster );

/**
  @brief Set a QoS policy for the given master, overriding default settings.

  @param[in] eMaster     The master port identifier
  @param[in] pQosPolicy  A pointer to the qos policy to set (NULL to unset)
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfg_SetQosPolicy( ICBId_MasterType eMaster, AxiCfg_QosPolicyType *pQosPolicy );

#ifdef __cplusplus
}
#endif

#endif /* __DDIAXICFG_H__ */
