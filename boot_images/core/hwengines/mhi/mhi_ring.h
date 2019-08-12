#ifndef MHIRING_H
#define MHIRING_H

/**
  @file mhi_ring.h
  @brief
  This file defines the interface for the MHI Ring Management API's   
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/02/14   nk      Add interruptable field to ring
10/22/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_i.h"
#include "mhi_mmio.h"
#include "mhi_util.h"
#include "mhi_osal.h" 

struct _ring;

/** Event Type */
typedef struct _event
{
  uint8 trigger_msi;             /**< Whether this event should trigger msi or not  */
  mhi_ring_element_t event_el;   /**< Ring element */ 
}event_t; 

/** CB function to be called to process a ring element */ 
typedef uint32 (*rel_process_func)(struct _ring*, mhi_ring_element_t*, event_t*);

/* Ring processing state */
typedef enum
{
 RING_UNINIT=0, /**< Ring has not been initialized           */    
 RING_IDLE,     /**< Ring is idle and has no work to be done  */    
 RING_PENDING,  /**< Ring is pending and has work to be done */ 
 RING_WAITREL,  /**< Ring waiting for ring elements          */
 RING_WAITER,   /**< Ring waiting for event ring elements    */ 
 RING_WAITREQ,  /**< Ring waiting for additional client requests */  
 RING_WAITLINK, /**< Ring halted due to link busy and is waiting for link to become available */ 
 RING_ERROR     /*<< Ring is in error state, requires a reset */ 
}ring_state_t; 

/* Enum of Ring types */
typedef enum _ring_type
{
   RING_TYPE_CR=0,    /**< Command Ring */ 
   RING_TYPE_ER=1,    /**< Event Ring */ 
   RING_TYPE_TR=2     /**< Transfer Ring */
}ring_type_t;

/* Generic ring context that stores ring management information */ 
typedef struct _ring
{
   list_t             list;                /*<< Primary list */ 
   list_t             list2;               /*<< Secondary list. This allows ring to be on two lists at once. For ex: ring processing Q, and completion Q */ 
   mhi_osal_sync_type sync;                /**< Mutex */  
   uint32             id;                  /**< Ring id. Can be CCI, ER number or 0 for command ring */ 
   ring_type_t        type;                /**< Ring type. i.e command, transfer or event ring*/  
   ring_state_t       state;               /**< Ring processing state */ 
   mhi_ring_ctx_t     *ctx;                /**< Pointer to cached ring context */
   uint64             ringBase;            /**< Base ring address in host DRAM */ 
   mhi_ring_element_t *lRingBase;          /**< Pointer to cached ring base */ 
   uint32             priority;            /**< Ring priority */ 

   uint32             rdOffset;            /**< Offset into the ring where we are processing */ 
   uint32             wrOffset;            /**< Current Offset into local ring where last write happened */ 
   uint64             rSize;               /**< Ring size in elements */ 
   uint32             erNum;
   list_t             linkReqQ;            /**< Outstanding link requests issued for Ring */ 

   rel_process_func   rel_process;         /**< Ring element processing function */ 
   boolean            interruptable;       /**< If ring is interruptable or not */
}ring_t;

/** 
 * @brief      mhi_ring_get_priority
 *
 *             Returns a rings priority. Currently command ring is highest. Second comes the control channel,
 *             and then all other rings. In future we can have a table of properties per ring. 
 *              
 *
 *
 * @dependencies
 *              
 * 
 * @return       
 *              Ring priority 
 * 
 * @sideeffect  None 
 */
uint32 mhi_ring_get_priority(ring_t *ring);

/** 
 * @brief      mhi_ring_update_wroffset
 *
 *             Update the rings latest write offset from its write pointer retrieved from the
 *             doorbell. Also cache any new ring elements
 *              
 * 
 * @param[in]  *ring - Ring
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if successful
 *             SYSERR if system error occurred
 *             INVALID_OFFSET if offset is invalid
 *             ERROR if ring cache failed
 *
 * 
 * @sideeffect None 
 */
uint32 mhi_ring_update_wroffset(ring_t *ring);

/** 
 * @brief      mhi_ring_init
 *
 *             Initializes ring. This function should be called once. Should not be called 
 *             across MHI Resets. See mhi_ring_init_ctx 
 *              
 * 
 * @param[in]  id  
 * @param[in]  *ring  
 * @param[in]  type
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SUCCESS 
 * 
 * @sideeffect  None 
 */ 
uint32 mhi_ring_init(uint32 id, ring_t *ring, ring_type_t type, rel_process_func func);

/** 
 * @brief      mhi_ring_init_ctx
 *
 *             Initializes ring ctx. Done after channel reset for TR's, and after MHI reset for all rings. 
 *             Also validates cached contexts to make sure the host programmed them correctly.  
 *              
 * 
 * @param[in]  *ring - Ring 
 * @param[in]  *ctx  - Pointer to MHI Ring context such as event ring context, channel ring context, command ring context
 *
 * @dependencies
 *              
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect  None 
 */
uint32 mhi_ring_init_ctx(ring_t *ring, mhi_ring_ctx_t *ctx);

/** 
 * @brief      mhi_ring_update_ctx
 *
 *             Update rings MHI context with the rings metadata
 *              
 * 
 * @param[in]  *ring - Ring 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_ring_update_ctx(ring_t *ring);

/** 
 * @brief      mhi_ring_process
 *
 *             Generic ring processor. Return SUCCESS if all work is done on a ring. 
 *             It can also update the rings write pointer if db flag is set. The cacheonly flag
 *             is used to only update the ring pointers and not do any processing. At this time 
 *             it is not used 
 *              
 * 
 * @param[in]  *ring      - Ring 
 * @param[in]  db         - Flag indicating if db should be fetched
 * @param[in]  cacheOnly  - Flag indicating if we should only cache the ring and not do processing
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Return codes returned by underlying processing as well ass 
 *             SYSERR if system error occurred
 *             
 *
 * 
 * @sideeffect None 
 */
uint32 mhi_ring_process(ring_t *ring, uint32 db , uint32 cacheOnly );

/** 
 * @brief      mhi_ring_insert_element
 *
 *             Insert element into ring. Technically this implies we are producing elements 
 *             however, host is the producer of all ring elements, this function 
 *             just populates those free elements).
 *              
 * 
 * @param[in]  *ring  - Ring to insert into
 * @param[in]  *el    - Element to insert
 * @param[in]  update_offset - Whether to update the ring write offset or not 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SYSERR - system error 
 *             RING_FULL - Ring is full from producer perspective ( semantically its empty)  
 * 
 * @sideeffect None 
 */
uint32 mhi_ring_insert_element(ring_t *ring, mhi_ring_element_t *el, boolean update_offset );


#endif
