#ifndef __DDIAXICFGINT_H__
#define __DDIAXICFGINT_H__
/**
 * @file ddiaxicfgint.h
 * @note Internal header file. API for Bus Configuration
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/ddiaxicfgint.h#1 $ 
 * $DateTime: 2016/12/13 07:59:23 $ 
 * $Author: mplcsds1 $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 09/09/2014   jc      Support for NPA active-only clients and RPM semi-active set.
 * 06/20/2014   sds     Extend request types to add latency request in addition to BW.
 * 07/12/2013   sds     Add support for BKE thresholding and static configuration.
 * 11/15/2012   sds     Fix handling of RPM msg ids, while consolidating all RPM
 *                      communications in this layer.
 *                      Add support for NAS and proper sleep set usage.
 * 12/05/2011   dj      Created file
 *  
 *  
 * Copyright (c) 2011-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 */ 
#include <stdint.h>
#include <stdbool.h>
#include "icbid.h"
#include "axicfgos.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
  Enums for different function return codes 
 */
typedef enum {
 
  AXICFG_INT_SUCCESS, 
 
  AXICFG_INT_UNSUPPORTED,
  AXICFG_INT_UNINITIALIZED,

  AXICFGINTRETURNCODE_PLACEHOLDER= 0x7FFFFFFF  /**< Forced to extend enum to int32_t size */
} AxiCfgIntReturnCode;

/**
 * Axicfg Initialization Done Marker 
 *  
 * Marker to require as a dependency that indicates that all
 * Axicfg initialization is complete.
 * Axicfg waits on NPA nodes to be present and will create this marker when that is complete.
 */
#define AXICFG_INIT_DONE   "/bus/axicfg/inited"

/**
  @brief  The first method called to initialize AxiCfg driver. 
          Will set up all the required resources including locks, memory maps

  @return bool
      @retval true  if successful
      @retval false if failed or not yet complete

  @post Wait on AXICFG_INIT_DONE marker to know when initialization is complete.
 */
bool AxiCfgInt_Init( void );

/**
  @brief  The method called to reprogram the hardware to default/reset values.

  @return bool
      @retval true  if successful
      @retval false if failed
 */
bool AxiCfgInt_Reset( void );

/**
  @brief Configures the QoS Generator bandwidth for a master.
         Valid only when master QOS mode is either Limited, Regulator, or remote.

  @param[in]  ePort       Master port id
  @param[in]  u64TotalBW  Total aggregated bandwidth value for potential thresholding
  @param[in]  u64ActiveBW QOS generator bandwidth in bytes per second( BpS ) for Active/Next Active
  @param[in]  u64SemiBW   QOS generator bandwidth in bytes per second( BpS ) for Semi-Active
  @param[in]  u64SleepBW  QOS generator bandwidth in bytes per second( BpS ) for Sleep
  @param[in]  bNextActive Boolean indicating that the active request is for next active set

  @return int32_t
        @retval AXICFG_INT_SUCCESS        When successfully configured 
        @retval AXICFG_INT_UNSUPPORTED    When the port is unsupported
        @retval AXICFG_INT_UNINITIALIZED  When the initialization of driver failed
 */
int32_t AxiCfgInt_QoSSetMasterBW
( 
  ICBId_MasterType ePort,
  uint64_t         u64TotalBW,
  uint64_t         u64ActiveBW,
  uint64_t         u64SemiBW,
  uint64_t         u64SleepBW,
  bool             bNextActive
);


/**
  @brief Configures the bandwidth for a slave (non-clock).
         Valid only when the slave is remote.

  @param[in]  eSlave      Slave port id
  @param[in]  u64ActiveBW Bandwidth in bytes per second( BpS ) for Active/Next Active
  @param[in]  u64SemiBW   Bandwidth in bytes per second( BpS ) for Semi-Active
  @param[in]  u64SleepBW  Bandwidth in bytes per second( BpS ) for Sleep
  @param[in]  bNextActive Boolean indicating that the active request is for next active set

  @return int32_t
        @retval AXICFG_INT_SUCCESS        When successfully configured 
        @retval AXICFG_INT_UNSUPPORTED    When the port is unsupported
        @retval AXICFG_INT_UNINITIALIZED  When the initialization of driver failed
 */
int32_t AxiCfgInt_QoSSetSlaveBW
( 
  ICBId_SlaveType eSlave,
  uint64_t        u64ActiveBW,
  uint64_t        u64SemiBW,
  uint64_t        u64SleepBW,
  bool            bNextActive
);

/**
  @brief Return whether or not the master supports latency requests.

  @param[in] eMaster Master port id

  @return bool
        @retval true  When the master supports a latency request
        @retval false Otherwise
 */
bool AxiCfgInt_IsMasterLatencySupported( ICBId_MasterType eMaster );

/**
  @brief Configures the latency request for a master.

  @param[in]  eMaster          Master port id
  @param[in]  u32ActiveLatency Latency in nanoseconds for Active/Next Active
  @param[in]  u32SemiLatency   Latency in nanoseconds for Semi-Active
  @param[in]  u32SleepLatency  Latency in nanoseconds for Sleep
  @param[in]  bNextActive Boolean indicating that the active request is for next active set

  @return int32_t
        @retval AXICFG_INT_SUCCESS        When successfully configured 
        @retval AXICFG_INT_UNSUPPORTED    When the port is unsupported
        @retval AXICFG_INT_UNINITIALIZED  When the initialization of driver failed
 */
int32_t AxiCfgInt_QoSSetMasterLatency
(
  ICBId_MasterType eMaster,
  uint32_t         u32ActiveLatency,
  uint32_t         u32SemiLatency,
  uint32_t         u32SleepLatency,
  bool             bNextActive
);

/**
  @brief  Used to fetch a value from xml property file in target config lib
          Can only be called after AxiCfgOs_Init

  @param  pString   Name of the property string

  @return uint32_t    Value of the property string
  */
#define AxiCfgInt_GetProperty( pString )  AxiCfgOs_GetProperty( pString )


/**
 @brief Commit all QoS changes to the RPM.

 @param[in] bWait  Indicate to the commit function whether or not
                   to wait for the RPM response.
 */
void AxiCfgInt_Commit( bool bWait );

#ifdef __cplusplus
}
#endif

#endif /* __DDIAXICFGINT_H__ */
