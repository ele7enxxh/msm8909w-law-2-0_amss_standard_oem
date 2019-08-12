/**
  @file mhi_link.c
  @brief
  This file contains the implementation of the MHI Link Abstraction component 

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/01/13   am      Fixes based on D3cold testing
11/21/13   nk      Single ATU map call
11/07/13   nk      Add async reads and writes
08/15/13   am      Added power management notifications.
05/30/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "mhi_link.h"
#include "mhi_pm.h"
#include "mhi_mmio.h"
#include "mhi_core.h"
#include "mhi_hwacc.h"
#include "pcie.h" 

/* DMA minimum request size. Any request below this will not use the DMA  */
#define DMA_REQ_MIN_SIZE 128 

/* Translates host address to local address */
#define HOST_TO_LOCAL(index,addr) ((addr)-(address_map[index].host)+(address_map[index].local.va))

#define VALIDATE_TRANS(index, addr,len) ( (addr)>=address_map[index].host && (((addr)-address_map[index].host+(len)) <= address_map[index].size) )

#ifdef MHI_DMA
/* Local Prototypes */
void mhi_link_rw_cb_wrapper(dma_read_write_req_type result);
#endif

/* Static address map used to store host to local address mappings */
static addr_map_t address_map[2];

#ifndef MHI_BOOT 

/* PCIE to MHI Notification map */ 
static uint32 pcie_to_mhi_notification_map[PCIE_TO_MHI_PM_MAX] = {0};


/** 
 * @brief      mhi_link_map_pcie_notification_to_mhi
 *
 *             Map the notifications enumerated in PCIE driver to equivalent MHI notifications
 *              
 * 
 * @param[in]  pcie_notif - pcie notification type
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
void mhi_link_map_pcie_notification_to_mhi(pcie_to_mhi_pm_n_type pcie_notif)
{
   switch (pcie_notif)
   {
      case PCIE_PM_PERST_DEASSERTED:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_HOST_WAKEUP_MHI_REQ_SHIFT;
         break;
      case PCIE_PM_DEVICE_STATE_IN_D3_COLD:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_D_STATE_CHANGE_SHIFT;
         break;
      case PCIE_PM_DEVICE_STATE_IN_D3_HOT:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_D_STATE_CHANGE_SHIFT;
         break;
      case PCIE_PM_DEVICE_STATE_IN_D0:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_D_STATE_CHANGE_SHIFT;
         break;
      case PCIE_LINK_UP:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_LINK_UP_SHIFT;
         break;
      case PCIE_LINK_DOWN:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_LINK_DOWN_SHIFT;
         break;
      case PCIE_PM_L1_INACT_TIMEOUT:
         pcie_to_mhi_notification_map[pcie_notif] = MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT;
         break;
      default:
         mhi_osal_debug(MSG_FATAL,"mhi_link_map_pcie_notification_to_mhi: Unknown notification received %x",pcie_notif);
   }
}

/** 
 * @brief      mhi_link_notifications
 *
 *             MHI Link Notifications. Called by pcie driver
 *              
 * 
 * @param[in]  pcie_notif  - Notification 
 * @param[in]  *data - User supplied data 
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
void mhi_link_notifications(pcie_to_mhi_pm_n_type pcie_notif, void *data)
{
   /* Multiple interrupts of varying priorities may invoke this callback */
   mhi_osal_syncenter(&mhiCtx.mhi_link_info.mhi_link_notifSync);

   mhi_osal_debug(MSG_HIGH,"mhi_link_notifications: Received pcie_notif %d \
                            MHI SM state %d",pcie_notif,mhiCtx.mhi_sm_info.mhiState); 
   
   switch (pcie_notif)
   {
      case PCIE_PM_PERST_DEASSERTED:
         /* PCIe EP driver is expected to enable hw blocks before this notification.
          * Also, PCIe EP driver has mutex protection for PCIE_PM_PERST_DEASSERTED and
          * PCIE_PM_DEVICE_STATE_IN_D3_COLD, so mhi_link_hw_enabled should be 
          * free of possible races between the two notifications */
          
         if (mhiCtx.mhi_pm_info.current_d_state != MHI_LINK_D3cold)
         {
           mhi_osal_syncleave(&mhiCtx.mhi_link_info.mhi_link_notifSync);
           mhi_osal_debug(MSG_ERR,"PERST deasserted in device state: %d",mhiCtx.mhi_pm_info.current_d_state);
           return;
         }

         mhi_osal_syncenter(&mhiCtx.mhiSync);
         mhiCtx.mhi_link_info.mhi_link_hw_enabled = TRUE;
         mhi_osal_syncleave(&mhiCtx.mhiSync);

         mhi_mmio_restore(mhiCtx.mhi_link_info.mhiConfig);

         break;
      /* In the current implementation PCIe driver reports D3cold whenever PERST is asserted */   
      case PCIE_PM_DEVICE_STATE_IN_D3_COLD:
         /* PCIe EP driver is expected to disable hw blocks */
         mhi_osal_syncenter(&mhiCtx.mhiSync); 
         mhiCtx.mhi_link_info.mhi_link_hw_enabled = FALSE;
         mhi_osal_syncleave(&mhiCtx.mhiSync);
       
         mhi_pm_set_d_state(MHI_LINK_D3cold);
         break;
      case PCIE_PM_DEVICE_STATE_IN_D3_HOT:
         /* Backup MMIO to prepare for clocks to go off*/
         mhi_mmio_backup(mhiCtx.mhi_link_info.mhiConfig);
         mhi_pm_set_d_state(MHI_LINK_D3hot);
         break;
      case PCIE_PM_DEVICE_STATE_IN_D0:
         mhi_pm_set_d_state(MHI_LINK_D0);
         break;
      case PCIE_LINK_UP:
         /* Nothing to do */
         break;
      case PCIE_LINK_DOWN:
         /* Nothing to do */
         break; 
      case PCIE_PM_L1_INACT_TIMEOUT:
         /* No special handling needed */
         break;
      default:
         mhi_osal_debug(MSG_FATAL,"mhi_link_notifications: Unknown notification received %x",pcie_notif);
   }

   /* If notification was not mapped, map it, so when we set the pending trigger we can use the MHI 
    * LINK notification map */
   if(!pcie_to_mhi_notification_map[pcie_notif])
   {
      mhi_link_map_pcie_notification_to_mhi(pcie_notif);
   }

   mhi_osal_syncleave(&mhiCtx.mhi_link_info.mhi_link_notifSync);

   /* Set the state machine trigger as pending and signal the MHI CORE thread */
   mhi_sm_set_trig_pending(pcie_to_mhi_notification_map[pcie_notif],TRUE); 

}
#endif

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
mhi_status_t mhi_link_init(mhi_link_info_t *link_info)
{
   mhi_osal_debug(MSG_MEDIUM,"mhi_link_init: Enter");

   if(FALSE == pcie_is_pcie_boot_enabled())
   {
      return MHI_ERROR; 
   }

   /* This mutex is exclusively for link notifications. Should not be used for
    * anything else since it will be used in isr context.
    */
   mhi_osal_syncinit(&mhiCtx.mhi_link_info.mhi_link_notifSync);

#ifdef MHI_BOOT
   pcie_init();
#else
   pcie_mhi_register_pm_cb(mhi_link_notifications,NULL);   
#endif

   /* Register DMA callbacks with PCIE driver */
#ifdef MHI_DMA 
   pcie_register_dma_cb(mhi_link_rw_cb_wrapper, mhi_link_rw_cb_wrapper);
#endif 

   /* Link hardware must be enabled at init */
   mhiCtx.mhi_link_info.mhi_link_hw_enabled = TRUE;

   return MHI_SUCCESS;
}

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
mhi_status_t mhi_link_triggerinterrupt(uint32 vec)
{
   if( 0 != pcie_sendmsi(vec) )
      return MHI_ERROR; 

   return MHI_SUCCESS; 
}

#ifdef MHI_DMA
/** 
 * @brief      mhi_link_rw_cb_wrapper
 *
 *             Link CB wrapper that receives callbacks from underlying transport 
 *             driver
 *              
 * 
 * @param[in]  result
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
void mhi_link_rw_cb_wrapper(dma_read_write_req_type result)
{
   mhi_link_ret_t ret; 

   /* Translate the return structure from pcie to mhi and issue the CB */
   ret.userdata=result.user_data; 
   ret.addr=result.src_addr; 
   ret.size=result.size;

   mhiCtx.mhi_link_info.cbFunc(ret);  
}
#endif 

/** 
 * @brief      mhi_link_rw
 *
 *             Perform a read/write operation over the link
 *              
 * 
 * @param[in]  index - Address index
 * @param[in]  destAddr  - Destination address
 * @param[in]  *srcAddr  - Source address
 * @param[in]  size      - Size of write in bytes
 * @param[in]  *userdata - User data to associate with write 
 * @param[in]  *op       - Operation (READ/WRITE)
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
mhi_status_t mhi_link_rw(uint32 index, uint64 link_addr, void* local_addr, uint32 size, void* userdata, mhi_op_t op)
{
   uint32 ret; 
   uint32 (*pcie_func)(void *, uint32, uint32, void *) = (MHI_READ == op?pcie_read_va:pcie_write_va);

   (void)ret;

   if(!VALIDATE_TRANS(index,link_addr,size))
   {
      return MHI_ERROR; 
   }

#ifdef MHI_DMA
   /* Check size, if less than threshold use synchronous pcie write (memcpy). 
    * Otherwise use DMA based (async) */
   if(size < DMA_REQ_MIN_SIZE || NULL == userdata)
#endif
   {
      pcie_func((char*)local_addr,HOST_TO_LOCAL(index,link_addr),size, NULL);  
      return MHI_SUCCESS;
   }
#ifdef MHI_DMA
   else
   {
      ret = pcie_func((char*)local_addr,HOST_TO_LOCAL(index,link_addr),size, userdata );  

      if(0 == ret) 
         return MHI_PENDING; 
      else if(0xFFFFFFFFF == ret) 
         return MHI_OP_FULL; 
      else
         return MHI_ERROR; 

   }
#endif

}

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
mhi_status_t mhi_link_write(uint32 index,uint64 destAddr, void* srcAddr, uint32 size, void* userdata )
{
#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_link_write: Attempted access when hardware is off");
   }
#endif

   return mhi_link_rw(index, destAddr, srcAddr, size, userdata, MHI_WRITE);  
}

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
mhi_status_t mhi_link_read(uint32 index, uint64 srcAddr, void* destAddr, uint32 size, void *userdata)
{
#ifdef MHI_CHECK_HW_ENABLED
   if (!mhiCtx.mhi_link_info.mhi_link_hw_enabled)
   {
      mhi_osal_debug(MSG_FATAL,"mhi_link_read: Attempted access when hardware is off");
   }
#endif   
   
   return mhi_link_rw(index, srcAddr, destAddr, size, userdata, MHI_READ);  
}

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
mhi_status_t mhi_link_reset(void)
{
   /* Remove the atu mappings */
   pcie_remove_index_obatu(1);
   pcie_remove_index_obatu(2);

   mhi_osal_memset(address_map, 0, sizeof(address_map));

   mhiCtx.mhi_link_info.mhi_link_hw_enabled = TRUE;

   return MHI_SUCCESS; 
}

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
mhi_status_t mhi_link_maphostaddrspace(uint64 *base, uint64 *limit, mhi_addr_t **local, uint32 nMap)
{
   pcie_iatu_entry_type iatu_map[5]; 
   pcie_address_type local_addr[5];

   uint32 i; 

   /* Pcie driver does not support more than 2 */
   if(nMap > 2)
      return MHI_ERROR; 

   /* For each map populate the iatu_map to describe the addresses */
   for(i =0; i< nMap; i++)
   {
      local[i]->va=0; 
      local[i]->pa=0; 

      if(limit[i] < base[i] || base[i]&0xFFFULL)
         return MHI_ERROR;

      iatu_map[i].host_addr.pa_low=*(uint32*)&base[i]; 
      iatu_map[i].host_addr.pa_hi=*(((uint32*)&base[i])+1); 
      iatu_map[i].size=limit[i]-base[i]+1;
      iatu_map[i].local_addr=&local_addr[i];
   } 

   /* Request addresses to be mapped */   
   if(SUCCESS != pcie_set_host2dev_mmap(iatu_map,nMap))
      return MHI_ERROR; 

   for (i=0; i< nMap; i++)
   {
      if(0 == local_addr[i].va_low)
         return MHI_ERROR; 

      local[i]->va = local_addr[i].va_low; 
      local[i]->pa = local_addr[i].pa_low; 

      address_map[i].local=*local[i]; 
      address_map[i].host=base[i]; 
      address_map[i].size=iatu_map[i].size; 
   }

   return MHI_SUCCESS;
}

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
mhi_status_t mhi_link_wakeup_req(void)
{
#ifndef MHI_BOOT  
   /* Decision to assert Wake GPIO or send PME is left to PCIe EP driver */
   pcie_wakeup_host();
#endif
   return MHI_SUCCESS;
}

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
mhi_status_t mhi_link_enable(void)
{
#ifndef MHI_BOOT
   /* Enable ipa db routing  */
   mhi_link_setup_hwacc_db_routing(IPA_UC_MAILBOX_CDB_TARGET,IPA_UC_MAILBOX_EDB_TARGET);

   pcie_enable_endpoint();
   
   mhi_osal_debug(MSG_HIGH, "mhi_link_enable: Restore Control and Data ATUs"); 

   if (MHI_SUCCESS != mhi_core_map_ctrl_and_base_regions(mhiCtx.mhiaddr,&mhiCtx.mhiLocalAddr))
   {
      mhi_osal_debug(MSG_FATAL, "mhi_link_enable: FATAL - Could not allocate local address space."); 
      return MHI_ERROR;
   }
#endif

   return MHI_SUCCESS;
}

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
mhi_status_t mhi_link_disable(void)
{
   /* Not supported at this time from MHI */
   return MHI_ERROR;
}

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
boolean mhi_link_is_active(void)
{
#ifndef MHI_BOOT /* Enable if supported by boot */  
   return (pcie_get_link_state() == PCIE_LINK_STATE_L0);
#else
   return TRUE;
#endif
}

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
boolean mhi_link_is_hw_enabled(void)
{
   return mhiCtx.mhi_link_info.mhi_link_hw_enabled;
}

#ifndef MHI_BOOT
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
void mhi_link_enable_inactivity_intr(void)
{
   pcie_control_l1ss_inact_interrupt(TRUE);
}

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
void mhi_link_disable_inactivity_intr(void)
{
   pcie_control_l1ss_inact_interrupt(FALSE);
}

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
void mhi_link_release_hw_resources(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_link_release_hw_resources: Enter");
   pcie_pm_enter_lpm();
   mhiCtx.mhi_link_info.mhi_link_hw_enabled = FALSE;
}
   
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
void mhi_link_acquire_hw_resources(void)
{
   mhi_osal_debug(MSG_HIGH,"mhi_link_acquire_hw_resources: Enter"); 
   pcie_pm_exit_lpm();
   mhiCtx.mhi_link_info.mhi_link_hw_enabled = TRUE;
}
#endif /* #ifndef MHI_BOOT */

/**
 * @brief       mhi_link_setup_hwacc_db_routing
 *
 *              Setup the doorbell routing for the hardware accelerator.
 *
 * @param[in]   chdb_mailbox_addr - Target address for routing channel doorbells to
 *              the hardware accelerator.
 * @param[in]   erdb_mailbox_addr - Target address for routing event ring doorbells
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
void mhi_link_setup_hwacc_db_routing(uint32 chdb_mailbox_addr, uint32 erdb_mailbox_addr)
{
#ifdef MHIIPA
   pcie_set_mhi_ipa_dbs(HWCHANNELBASE, HWCHANNELBASEEND, HWCHANNELBASE, HWCHANNELBASEEND); 
   pcie_set_mhi_ipa_cdb_target(chdb_mailbox_addr); 
   pcie_set_mhi_ipa_edb_target(erdb_mailbox_addr);
#endif
}


