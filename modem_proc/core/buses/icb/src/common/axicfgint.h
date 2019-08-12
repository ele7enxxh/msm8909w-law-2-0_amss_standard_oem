#ifndef __AXICFGINT_H__
#define __AXICFGINT_H__
/**
 * @file axicfgint.h
 * @note Internal header file for Bus Configuration
 * 
 *                REVISION  HISTORY
 *  
 * This section contains comments describing changes made to this file. Notice
 * that changes are listed in reverse chronological order.
 * 
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/axicfgint.h#1 $ 
 * $DateTime: 2016/12/13 07:59:23 $ 
 * $Author: mplcsds1 $ 
 * 
 * when         who     what, where, why
 * ----------   ---     ---------------------------------------------------
 * 02/26/2015   sds     Add QoS override API
 * 06/18/2014   sds     Add lockless halt/unhalt APIs to be used during error handling.
 * 02/16/2014   sds     Add port halt/unhalt APIs.
 * 12/05/2011   dj      Created file
 *  
 *  
 * Copyright (c) 2011-2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
 */ 

#include "ddiaxicfgint.h"
#include "ddiaxicfg.h"
#include "icb_soc_data.h"
#include "HALtcsr.h"

#ifdef __cplusplus
extern "C" {
#endif

/*--------------------------------- types ---------------------------------*/
struct axicfg_soc_info 
{
  icb_master_def_type ** pMasterList;
  uint32_t uMasterCount;
  icb_slave_def_type ** pSlaveList;
  uint32_t uSlaveCount;
  icb_bus_def_type ** pBusList;
  uint32_t uBusCount;
  HAL_tcsr_InfoType *pTcsrInfo;
};

struct axicfg_master_search_info
{
  ICBId_MasterType   ePort;
  icb_master_def_type * pCurrMaster;
};

struct axicfg_slave_search_info
{
  ICBId_SlaveType eSlave;
  icb_slave_def_type * pCurrSlave;
};

/* Should be kept in sync with ul node state type, as necessary. */
typedef struct
{
  uint64 uTotalBW;           /**< Total BW as computed by UL */
  uint64 uIb;                /**< Stored Ib value */
  uint64 uSuppIb;            /**< Stored Ib value */
  uint64 uActiveOnlyIb;      /**< Stored Ib value */
  uint64 uAb;                /**< Stored Ab value */
  uint64 uSuppAb;            /**< Stored Ab value */
  uint64 uActiveOnlyAb;      /**< Stored Ab value */
} axicfg_node_state_type;

/*--------------------------- public functions ---------------------------*/
/**
  @brief Used to check if an external master id is valid

  @pre   pInfo != NULL

  @param[inout] pInfo  pointer to structure that's used to hold master search info

  @return bool
    @retval true  if valid
    @retval false if not valid
 */
bool AxiCfgInt_SearchPort( struct axicfg_master_search_info *pInfo );

/**
  @brief Used to check if an external slave id is valid

  @pre   pInfo != NULL

  @param[inout] pInfo  pointer to structure that's used to hold slave search info

  @return bool
    @retval true  if valid
    @retval false if not valid
 */
bool AxiCfgInt_SearchSlave( struct axicfg_slave_search_info *pInfo  );

/**
  @brief Attempt to halt the specified master port.

  @param[in] eMaster  The master port identifier
  @param[in] bLock    Whether or not to use the hw mutex
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfgInt_HaltMasterPort( ICBId_MasterType eMaster, bool bLock );

/**
  @brief Attempt to unhalt the specified master port.

  @param[in] eMaster  The master port identifier
  @param[in] bLock    Whether or not to use the hw mutex
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfgInt_UnhaltMasterPort( ICBId_MasterType eMaster, bool bLock );

/**
  @brief Used to check if an external master id is a BIMC master id

  @pre   AxiCfgInt_SearchPort returned true on pInfo
         NULL != puCount
         NULL != puMasterPort

  @param[inout] pInfo         pointer to structure that's used to hold master search info
  @param[out] puMasterPort  pointer to internal master port id array
  @param[out] puCount       pointer to internal master port id count

  @return bool
    @retval true  if pInfo->ePort is valid and puMasterPort, puCount are set
    @retval false if pInfo->ePort is invalid. Both puMasterPort and puCount are not set
 */
bool AxiCfgInt_GetBimcMasterInfo( struct axicfg_master_search_info *pInfo, uint32_t ** puMasterPort, uint32_t * puCount );

/**
  @brief Used to check if an external master id is remoted

  @pre   AxiCfgInt_SearchPort returned true on pInfo

  @param[in] pInfo  pointer to structure that's used to hold master search info
 
  @return bool
    @retval true  if pInfo->ePort is remoted
    @retval false if pInfo->ePort is not remoted or unsupported 
 */
bool AxiCfgInt_IsRemoteMaster( struct axicfg_master_search_info *pInfo );

/**
  @brief Used to check if an external slave id is a BIMC slave id

  @pre   AxiCfgInt_SearchSlave returned true on pInfo
         puSlaveWay != NULL
         puCount != NULL 

  @param[inout] pInfo         pointer to structure that's used to hold slave search info
  @param[out] puSlaveWay    pointer to internal slave port id array
  @param[out] puCount       pointer to internal slave port id count

  @return bool
    @retval true  if pInfo->eSlave is valid and puSlaveWay, puCount are set
    @retval false if pInfo->eSlave is invalid. Both puSlaveWay and puCount are not set
 */
bool AxiCfgInt_GetBimcSlaveInfo( struct axicfg_slave_search_info *pInfo, uint32_t ** puSlaveWay, uint32_t * puCount );

/**
  @brief Used to check if an external slave id is remoted

  @pre   AxiCfgInt_SearchSlave returned true on pInfo

  @param[in] pInfo         pointer to structure that's used to hold slave search info
 
  @return bool
    @retval true  if eSlave is remoted
    @retval false if eSlave is not remoted or unsupported 
 */
bool AxiCfgInt_IsRemoteSlave( struct axicfg_slave_search_info *pInfo );

/**
  @brief Used to get master hal info

  @pre   AxiCfgInt_SearchPort returned true on pInfo
         pSearchInfo != NULL 
         pHalInfo != NULL 
         *pHalInfo != NULL

  @param[in]  pSearchInfo   pointer to structure that's used to hold master search info
  @param[out] pHalInfo      a pointer to master hal info
 */
void AxiCfgInt_GetMasterHalInfo( struct axicfg_master_search_info *pSearchInfo, void **pHalInfo );

/**
  @brief Used to get slave hal info

  @pre   AxiCfgInt_SearchSlave returned true on pInfo
         pSearchInfo != NULL 
         pHalInfo != NULL 
         *pHalInfo != NULL

  @param[in]  pSearchInfo   pointer to structure that's used to hold slave search info
  @param[out] pHalInfo      a pointer to slave hal info
 */
void AxiCfgInt_GetSlaveHalInfo( struct axicfg_slave_search_info *pSearchInfo, void **pHalInfo );

/**
  @brief Set a QoS policy for the given master, overriding default settings.

  @param[in] pInfo       A pointer to the master information
  @param[in] pQosPolicy  A pointer to the qos policy to set (NULL to unset)
 
  @returns AXICFG_SUCCESS if successful
           AXICFG_ERROR_* otherwise
 */
AxiCfg_ReturnCodeType AxiCfgInt_SetQosPolicy( struct axicfg_master_search_info *pInfo, 
                                              AxiCfg_QosPolicyType *            pQosPolicy,
                                              axicfg_node_state_type *          pState );

#ifdef __cplusplus
}
#endif

#endif /* __AXICFGINT_H__ */
