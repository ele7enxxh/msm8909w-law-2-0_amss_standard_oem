#ifndef __UL_I_H__
#define __UL_I_H__
/**
 * @file ul_i.h
 * 
 * This file defines the internal UL interface.
 *
 *  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
 *             All Rights Reserved.
 *          QUALCOMM Proprietary/GTDR
 *
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/icb/src/common/ul_i.h#1 $
 * $DateTime: 2016/12/13 07:59:23 $
 * $Author: mplcsds1 $
 * $Change: 11985146 $
 *
 *                    EDIT HISTORY FOR FILE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * when       who     what, where, why
 * --------   ---     -----------------------------------------------------------
 * 05/18/15   sds     Add support for notification interface.
 * 02/26/15   sds     Add APIs to fetch node aggregation state.
 * 09/09/14   jc      Support for NPA active-only clients and RPM semi-active set.
 * 06/20/14   sds     Extend request types to add latency request in addition to BW.
 * 03/11/14   sds     Extend query function - slave port count.
 * 11/15/12   sds     Remove unused typedef and unnecessary slave ID array
 * 10/03/12   sds     Clock cleanup and master clock aggregation added.
 * 04/25/12   dj      add dal config support
 * 04/11/12   dj      double word align 64 bit heap data
 * 12/08/11   rng     Ported from dev/1.0
 *
 */

#include "icbid.h"
#include "DALStdDef.h"
#include "npa_resource.h"
#include "ul_tree.h"
#include "icb_soc_data.h"
#include "icb_notify.h"

/*============================================================================
                          DEFINES
============================================================================*/

typedef struct ul_request_queue_node
{
  struct ul_request_queue_node  * next;
  npa_client_handle    hClient;          /**< Client to issue request */
  DALBOOL              bQueued;          /**< Has this request been queued */
  void *               pRequest;         /**< Request value or pointer */
  void               (*req_func)( struct ul_request_queue_node *);
} ul_request_type;

/**
 * Masters Created Marker 
 *  
 * Marker to require as a dependency that indicates all Master 
 * Subnodes have been created. 
 */
#define UL_MASTERS_CREATED   "/bus/masters/inited"

/**
 * Master Request Type
 *
 * Structure of Ib and Ab values to pass to a Master node when requesting
 * bandwidth
 */
typedef struct
{
  uint64            uIb;   /**< Instantaneous BW request in Bytes/Sec */
  uint64            uAb;   /**< Arbitrated BW request in Bytes/Sec */
  npa_client_handle hClient;
  npa_client_type   clientType;  /* Retained to support client type change request */
  uint32            uLatencyNs; 
} ul_bw_request_type;

/**
 * BW Request List 
 */
typedef struct
{
  uint32                uEntryCount;
  uint32                uListSize;
  ul_bw_request_type ** aRequestList;
} ul_bw_request_list_type;

/** Node State */
typedef struct
{
  uint64 uTotalBW;           /**< Total BW as computed by compute_bw() */
  uint64 uIb;                /**< Stored Ib value */
  uint64 uSuppIb;            /**< Stored Ib value */
  uint64 uActiveOnlyIb;      /**< Stored Ib value */
  uint64 uAb;                /**< Stored Ab value */
  uint64 uSuppAb;            /**< Stored Ab value */
  uint64 uActiveOnlyAb;      /**< Stored Ab value */
} ul_node_state_type;

/** Clock info */
typedef struct
{
  const char * const name;         /**< NPA name for this clock. */
  npa_client_handle  normal;       /**< Normal, non-suppressible request handle. */
  npa_client_handle  suppressible; /**< Suppressible request handle. */
  npa_client_handle  activeonly;   /**< Active-only request handle. */
} ul_clock_type;

typedef struct icb_notify_type
{
  struct icb_notify_type *     next;
  struct icb_notify_list_type *list;
  icb_notify_definition_type   definition;
  icb_notify_cb_type           callback;
  void *                       callback_data;
  bool                         status_valid;
  icb_notify_status_type       status;
} icb_notify_type;

typedef struct icb_notify_list_type
{
  icb_notify_type *head;
  uint32_t         count;
} icb_notify_list_type;

/** Master Settings */
typedef struct
{
  const char *             clientName;
  ICBId_MasterType         eExtMasterID;        /**< External Master ID */
  uint32                   u32SlaveWidth;       /**< Width of slave bus in bytes */
  uint32                   u32MstrClockCount;   /**< Number of master clock clients */
  ul_clock_type          * aMstrClocks;         /**< Array of master clocks. */
  ul_node_state_type       mstrState;           /**< Stored Ab value */
  ul_request_type          masterRequest;       /**< Master Request Structure */
  icb_master_def_type    * pMasterDef;
  ul_bw_request_list_type  requestList;         /**< List of current requests for this master */
  ul_request_type          latencyRequest;      /**< Master Latency Request Structure */
  icb_notify_list_type     notifications;       /**< List of current notification requests */
} ul_master_data_type;

/** Slave Settings */
typedef struct
{
  const char *             clientName;
  ICBId_SlaveType          eExtSlaveID;        /**< External Slave ID */
  uint32                   u32SlaveWidth;      /**< Width of slave bus in bytes */
  ul_node_state_type       slvState;           /**< Slave State */
  uint32                   u32SlvClockCount;   /**< Number of slave clock clients */
  ul_clock_type          * aSlvClocks;         /**< Array of slave clocks. */
  uint32                   u32MemClockCount;   /**< Number of mem clock clients */
  ul_clock_type          * aMemClocks;         /**< Array of mem clocks. */
  ul_bw_request_list_type  requestList;        /**< List of current requests for this slave */
  ul_request_type          slvRequest;         /**< Slave Request Structure */
  icb_slave_def_type     * pSlaveDef;
  icb_notify_list_type     notifications;       /**< List of current notification requests */
} ul_slave_data_type;    

/** Route Settings */
typedef struct
{
  ul_master_data_type * pMasterData;  /**< Pointer to master data */
  ul_slave_data_type  * pSlaveData;   /**< Pointer to slave data */
} ul_pair_type;

typedef struct
{
  uint32         u32NumPairs;         /**< Number of Master/Slave Pairs in route */
  ul_pair_type * aMasterSlavePairs;   /**< Array of internal master slave pairs */
} ul_route_list_type;

typedef struct
{
  ICBId_MasterType     eExtMasterID;   /**< External Master ID */
  ICBId_SlaveType      eExtSlaveID;    /**< External Slave ID */
  ul_route_list_type * pRouteList;     /**< List of nodes in route */
} ul_route_type;

typedef struct
{
  uint32                 uNumMasters;
  ul_master_data_type ** aMasterList;
} ul_int_master_list_type;

typedef struct
{
  uint32                uNumSlaves;
  ul_slave_data_type ** aSlaveList;
} ul_int_slave_list_type;

/*============================================================================
                       External Functions
============================================================================*/

/** ul_issue_pair_request
 * 
 * @brief Issue request for the given master slave pair 
 * 
 * @author ngibson (11/18/2011)
 * 
 * @param client 
 * @param pBWReq 
 * @param pOldReq 
 * @param pMSPair 
 * @param bFirst 
 * 
 */
void ul_issue_pair_request
(
  npa_client_handle        client,
  ul_bw_request_type     * pBWReq,
  ul_bw_request_type     * pOldReq,
  ul_pair_type           * pMSPair,
  DALBOOL                  bFirst
);

/** ul_add_bw_request
 * 
 * @brief Adds a BW request to the passed request list
 * 
 * @author seansw (09/24/2012)
 * 
 * @param pBWReq 
 * @param pRequestList
 */
void ul_add_bw_request
(
  ul_bw_request_type * pBWReq,
  ul_bw_request_list_type *   pRequestList
);

/** ul_remove_bw_request
 * 
 * @brief Removes a BW request from the passed request list
 * 
 * @author seansw (09/24/2012)
 * 
 * @param pBWReq 
 * @param pRequestList
 */
void ul_remove_bw_request
(
  ul_bw_request_type      * pBWReq,
  ul_bw_request_list_type * pRequestList
);

/**
 * @brief    Initialize and define the bus arbitration subnodes
 *
 */
void ul_internal_init( void );

/**
   @brief ul_get_route - Get a route associated with 
          an external master/slave pair

   This function looks up the route based upon the passed in 
   External Master ID and External Slave ID.  If the route 
   exists it will return a pointer to the route structure. 
   Otherwise it will return NULL if the route is not found. 
   
   @param eMaster - External Master ID
   @param eSlave  - External Slave ID
   
   @return ul_route_list_type* 
*/
ul_route_list_type *ul_get_route
(
  ICBId_MasterType eMaster,
  ICBId_SlaveType  eSlave
);

/**
  @brief
  Add a request to the commit queue
 */ 
void ul_request_enqueue ( ul_request_type * req );

/** ul_int_init
 * 
 * 
 * @author ngibson (12/13/2011)
 */
void ul_int_init( void );

/** ul_target_init
 *
 * 
 * @author ngibson (12/13/2011)
 */
void ul_target_init( void );

/** ul_target_get_route_tree
 * 
 * 
 * @author ngibson (12/13/2011)
 * 
 * @return ul_treeNodeType* 
 */
ul_treeNodeType * ul_target_get_route_tree( void );

/** ul_target_get_master_list
 * 
 * @brief Get the internal master list data.
 */
void ul_target_get_master_list( ul_int_master_list_type *pMasterList );

/** ul_target_get_slave_list
 * 
 * @brief Get the internal slave list data.
 */
void ul_target_get_slave_list( ul_int_slave_list_type *pSlaveList );

/** ul_get_master_state
 *
 * @brief Get the internal node aggregated state
 */
ul_node_state_type *ul_get_master_state( ICBId_MasterType eExtMasterID );

/** ul_get_slave_state
 *
 * @brief Get the internal node aggregated state
 */
ul_node_state_type *ul_get_slave_state( ICBId_SlaveType eExtSlaveID );

/** ul_notify_evaluate
 *
 * @brief Evaluate and update the current notification status for the passed notification.
 *
 */
void ul_notify_evaluate
(
  icb_notify_type *   notify,
  ul_node_state_type *state
);
#endif /* __UL_I_H__ */
