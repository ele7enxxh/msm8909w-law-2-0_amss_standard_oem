#ifndef MHILINK_H
#define MHILINK_H

/**
  @file mhi_link.h
  @brief
  This file defines the interface for the MHI LInk Abstraction component   
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/21/13   nk      Single ATU map call
08/15/13   am      Added link APIs related to power management.
02/30/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "mhi.h"
#include "mhi_i.h"
#include "mhi_osal.h"

/** Address map structure used to map local to host addresses */ 
typedef struct _addr_map
{
   mhi_addr_t   local;   /**< Local address info */ 
   uint64   host;        /**< Host address */ 
   uint32   size;        /**< Size of address region */ 
}addr_map_t;

/* MHI Link return type used for cb completion */ 
typedef struct _mhi_link_ret
{
   void* userdata;         /**< Userdata */ 
   uint32 addr;            /**> Address */ 
   uint32 size;            /**> Size of operation in bytes */ 
   mhi_status_t status; 
}mhi_link_ret_t;

/* Callback function used for async mhi link read/writes */ 
typedef void(*mhi_link_cb_func)(mhi_link_ret_t ret);

/** MHI link related information */
typedef struct _mhi_link_info
{
  boolean mhi_link_hw_enabled;           /**< Track whether it is safe to access MMIO */
  mhi_config_t *mhiConfig;               /**< Pointer to the MHI configuration structure */
  mhi_osal_sync_type mhi_link_notifSync; /**< Mutex to protect MHI link notifications */
  mhi_link_cb_func cbFunc;               /**< Callback function over which to send link r/w completions */ 
} mhi_link_info_t;

/** 
 * @brief      mhi_link_init
 *
 *             Initialize the MHI Link layer
 *              
 * 
 * @param[in]  *link_info - Pointer to link information context
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_link_init(mhi_link_info_t *link_info);

/** 
 * @brief      mhi_link_triggerinterrupt
 *
 *             Trigger an interrupt over the link
 *              
 * 
 * @param[in]  vec - Vector to trigger
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_link_triggerinterrupt(uint32 vec);

/** 
 * @brief      mhi_link_write
 *
 *             Perform a write operation over the link
 *              
 * 
 * @param[in]  index - Address index
 * @param[in]  destAddr  - Destination address
 * @param[in]  *srcAddr  - Source address
 * @param[in]  size      - Size of write in bytes
 * @param[in]  *userdata - User data to associate with write 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_link_write(uint32 index,uint64 destAddr, void* srcAddr, uint32 size, void* userdata );

/** 
 * @brief      mhi_link_read
 *
 *             Perform a read operation over the link 
 *              
 * 
 * @param[in]  index      - Address index
 * @param[in]  srcAddr    - Source address
 * @param[in]  *destAddr  - Destination address
 * @param[in]  size       - Size of operation in bytes
 * @param[in]  *userdata  - User supplied data to associate with operation
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_link_read(uint32 index, uint64 srcAddr, void* destAddr, uint32 size, void *userdata);

/** 
 * @brief      mhi_link_reset
 *
 *             Reset the link layer
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_link_reset(void);

/** 
 * @brief      mhi_link_maphostaddrspace
 *
 *             Map multiple host base to limit addresses
 *              
 * 
 * @param[in]  *base   - Host base address array
 * @param[in]  *limit  - Limit address array
 * @param[out] **local - Local Address information obtained from underlying transport  
 * @param[in]  nMap    - Number of mappings
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_link_maphostaddrspace(uint64 *base, uint64 *limit, mhi_addr_t **local, uint32 nMap);

/**
 * @brief       mhi_link_wakeup_req
 *
 *              Abstract link API that actually does the wake up procedure.
 *
 *
 * @dependencies
 *              mhi_link_init() must be called first. 
 * 
 * @return       
 *              MHI_STATUS if successful.   
 *              MHI_ERROR  if error occurred.
 *
 * @sideeffect  None 
 */
mhi_status_t mhi_link_wakeup_req(void);

/**
 * @brief       mhi_link_enable
 *
 *              Abstract link API that re-enables link hardware after a power collapse.
 *
 *
 * @dependencies
 *              Should be called only after PERST de-assertion notification is received. 
 * 
 * @return       
 *              MHI_STATUS if successful.   
 *              MHI_ERROR  if error occurred.
 * 
 * @sideeffect  None 
 */
mhi_status_t mhi_link_enable(void);

/**
 * @brief       mhi_link_disable
 *
 *              Abstract link API that disables link hardware. Not used for now.
 *
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              MHI_STATUS if successful.   
 *              MHI_ERROR  if error occurred. 
 * 
 * @sideeffect  None 
 */
mhi_status_t mhi_link_disable(void);

/**
 * @brief       mhi_link_is_active
 *
 *              Determine whether the link can actively push traffic or is in some
 *              power saving state.
 *
 *
 * @dependencies
 *              mhi_link_init() should be called first.  
 * 
 * @return       
 *              TRUE if link is active, False is not. 
 * 
 * @sideeffect  None 
 */
boolean mhi_link_is_active(void);

/**
 * @brief       mhi_link_is_hw_enabled
 *
 *              Report whether link hw is enabled or not, i.e. that link has not
 *              turned off hardware blocks to save power. Otherwise will not be
 *              able to access MMIO.            
 *
 *
 * @dependencies
 *              mhi_link_init must be called. 
 * 
 * @return       
 *              TRUE if link hardware is enabled, False if not. 
 * 
 * @sideeffect  None 
 */
boolean mhi_link_is_hw_enabled(void);

/**
 * @brief       mhi_link_enable_inactivity_intr
 *
 *              Enable the L1ss inactivity timer interrupt.            
 *
 *
 * @dependencies
 *              mhi_link_init must be called. 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None
 */
void mhi_link_enable_inactivity_intr(void);

/**
 * @brief       mhi_link_disable_inactivity_intr
 *
 *              Disable the L1ss inactivity timer interrupt.            
 *
 *
 * @dependencies
 *              mhi_link_init must be called. 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  None
 */
void mhi_link_disable_inactivity_intr(void);

/**
 * @brief       mhi_link_release_hw_resources
 *
 *              Release link hardware resources. This may involve
 *              the link driver software turning off clocks and voting against
 *              hardware resources that are not required if the link will not 
 *              be used. Note that when entering into D3cold, this task is handled
 *              implicitly by the link driver.
 *
 * @dependencies
 *              mhi_link_init must be called. 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  Link hardware is no longer accessible after this function is called.
 */
void mhi_link_release_hw_resources(void);

/**
 * @brief       mhi_link_acquire_hw_resources
 *
 *              Acquire link hardware resources. This may involve
 *              the link driver software turning on clocks and voting for
 *              hardware resources that are required if the link will be used.
 *              Note that when exiting out of D3cold, this task is handled
 *              implicitly by the link driver.
 *
 * @dependencies
 *              mhi_link_init must be called. 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  Link hardware is now accessible.
 */
void mhi_link_acquire_hw_resources(void);

/**
 * @brief       mhi_link_setup_hwacc_db_routing
 *
 *              Setup the doorbell routing for the hardware accelerator.
 *
 *@param[in]    chdb_mailbox_addr - Target address for routing channel doorbells to
 *              the hardware accelerator.
 *@param[in]    erdb_mailbox_addr - Target address for routing event ring doorbells
 *              to the hardware accelerator.
 *
 * @dependencies
 *              mhi_link_init must be called. 
 * 
 * @return       
 *              None 
 * 
 * @sideeffect  Writes to the channel and event ring doorbell registers dedicated to the 
 *              hardware accelerator in MMIO generate interrupts to the hardware accelerator.
 */
void mhi_link_setup_hwacc_db_routing(uint32 chdb_mailbox_addr, uint32 erdb_mailbox_addr);

#endif
