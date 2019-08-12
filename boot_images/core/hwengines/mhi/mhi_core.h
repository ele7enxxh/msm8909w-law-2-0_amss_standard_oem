#ifndef MHI_CORE_H
#define MHI_CORE_H
/**
  @file mhi_core.h
  @brief
  This file contains the mhi core functionality interface
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2013-2015 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "mhi_osal.h"
#include "mhi_i.h"
#include "mhi_ring.h"
#include "mhi_util.h"
#include "mhi_sm.h"
#include "mhi_pm.h"
#include "mhi_hwacc.h"

/** MHI Address **/
typedef struct _mhi_local_addr
{
   mhi_addr_t mhictrl; /**< MHI Control address info */
   mhi_addr_t mhidata; /**< MHI Data address info */
}mhi_local_addr_t;

/** Enumeration of channel client states **/
typedef enum
{ 
  CH_UNINIT=0,        /**< Channel has not been opened by client */  
  CH_PENDING_OPEN=1,  /**> Channel has been opened by client and is pending notification */
  CH_OPENED=2,        /**< Channel has been successfully opened by client */
  CH_PENDING_CLOSE=3, /**< Channel is pending to be closed */ 
  CH_CLOSED=4         /**< Channel is closed */ 

}ch_client_state; 


/** Internal Channel Context Structure **/ 
typedef struct _ch
{
   mhi_osal_sync_type chSync;       /**< Channel Synchronization structure */ 
   ch_client_state ch_client_state; /**< Channel Client State */ 
  
   mhi_osal_event_handle hNotify;   /**< Notification event to block on, used for sync calls */  

   /* TRB processing meta data */ 
   uint8   skip_to_td;              /**< Set when channel transfer, ring needs to move to next TD boundary */                       
   uint32  trb_bytes_processed;     /**< Number of bytes processed at current read offset */     

   /* Request Q */
   void      *requests;             /**< Pool of allocated requests used for queueing operations */
   list_t request_free_q;           /**< Queue of free requests */
   list_t request_q;                /**< Queue of outstanding requests */ 
   list_t request_q_issued;         /**< Queue of issued requests to link layer */ 

   mhi_cb_t  close_cb;              /**< CB to be used for close completion */ 
}ch_t;

/* Logical Event ring context */
typedef struct _er
{
   boolean pending_msi;             /**< Whether an msi is pending due to moderation */
   mhi_timer_t intmod_timer;        /**< Interrupt moderation timer */  
}er_t; 

/* MHI Request Status */ 
typedef enum _mhi_request_status
{
   MHI_REQ_UNINIT=0,               /**< Request uninitialized */ 
   MHI_REQ_PENDING,                /**< Request pending */ 
   MHI_REQ_BUSY,                   /**< Request busy */ 
   MHI_REQ_COMPLETED               /**< Request completed */ 
}mhi_request_status_t; 
 

/** MHI Request object representing client requests */ 
typedef struct _mhi_request
{
   list_t list;              /**< List object to facilitate placing request on queues. Must be first */
   mhi_cb_t cb;              /**< Supplied cb */ 
   mhi_op_t op;              /**< MHI operation */ 

   /** Parameter union */
   union _params
   {
      /** Read write parameters **/
      struct _rw_params
      {
         uint32 data;    /**< Data address of client buffer */ 
         uint32 size;    /**< Size of buffer */ 
         int8 last_op;   /**< If the operation is the last one (EOT) */ 
      }rw; 
   }params; 

   /* Internal request data */
   mhi_return_t ret;                  /**< Return object passed via CB */ 
}mhi_request_t; 
 

/** Request structure describing async link requests **/ 
typedef struct _mhi_link_async_req
{
  list_t list;            /**< List object. Facilitates insertion of object onto queues */ 
  uint32 local_addr;      /**< Local device address */ 
  uint64 link_addr;       /**< Link Address */ 
  uint32 size;            /**< Size of link operation in bytes */ 
  uint32 status;          /**< Status of operation */ 
 
  /* Ring processing related info */
  ring_t *ring;           /**< Ring that this link request is tied to */ 
  event_t event;          /**< MHI event associated with this link request */ 
  mhi_request_t *req;     /**< Client request associated with this link request */ 
}mhi_link_async_req_t; 

/** MHI context structure **/
typedef struct _mhi_ctx
{
  mhi_osal_sync_type mhiSync;        /**< Main MHI Synchronization structure */ 
  boolean            enumerated;     /**< TRUE if MHI successfully enumerated */ 
  mhi_config_t       mhiConfig;      /**< MHI configuration structure. Stores SW/HW configuration/capabilities of MHI */  
 
  mhi_osal_irq_type  irqhandle;      /**< IRQ handle for global MHI interrupt */   

  /* Interrupts */ 
  mhi_interrupts_t   int_ctrl;       /**< MHI CTRL pending interrupt  */ 
  mhi_interrupts_t   int_chdb;       /**< MHI CH doorbell pending interrupt status */ 
  mhi_interrupts_t   int_erdb;       /**< MHI ER interrupt pending status */ 
  mhi_interrupts_t   int_erdb_mask;  /**< MHI ER interrupt mask */ 


  /* Worker thread context  */
  mhi_osal_thread_t      worker;     /**< Main worker thread  */ 
  mhi_osal_event_handle  hNotify;    /**< Notify event used to signal worker thread */ 

  /* MHI ADDR configuration */
  mhi_host_addr_t         mhiaddr;   /**< MHI Address configuration by host*/ 
  mhi_local_addr_t   mhiLocalAddr;   /**< Local MHI address configuration */ 

  /* Transfer rings */
  uint64             ccabap;         /**< Host channel context base address */ 
  mhi_ch_ctx_t       *lChCtxBase;    /**< Local MHI channel context base pointer */ 
  
  /* Event rings */
  uint64             ecabap;         /**< Host event context array base address pointer */ 
  mhi_ev_ctx_t       *lErCtxBase;    /**< Local MHI event context base pointer */ 
  er_t               *er;            /**< Logical ER processing context */
 
  /* Command Ring */  
  uint64             crcbap;         /**< Host Command Ring Context Base Address Pointer */ 
  mhi_cr_ctx_t       lCrCtxBase;     /**< Local Command Ring Context */ 

  /* Generic Ring data */
  ring_t             *ring;         /**< Generic Ring Context structure used to abstract the ring involved. There is one per ring type*/
  list_t pendingRingQ;              /**< Pending Ring queue used to queue rings which have work to be done */ 
  list_t completeRingQ;             /**< Completion Ring queue used to queue rings which have async completions ready to be processed */  
  list_t waitErRingQ;               /**< Queue of rings waiting on event rings */  
  list_t waitLinkRingQ;             /**< Queue of rings waiting on link request  */  

  /* Channels */
  ch_t               *ch;           /**< Logical channel context for storing client context, channel opened/closed state, etc. */  
  uint32 pendingChannels[4];        /**< Bit array of pending channels. Each bit represents a channel */ 

  /* MHI Link */
  void                  *linkReq;                /**< Link requests */   
  list_t                freeLinkReqQ;            /**< Free link request Q */
  volatile uint32       num_completions_pending; /**< Number of issued link requests pending completions */
  mhi_osal_event_handle hNoMoreCompletions;      /**< Notify event used to signal when no more link request completions are pending */
  mhi_timer_t           link_completion_timer;   /**< Timer for monitoring link completions before MHI suspend */
  boolean               is_mhi_work_pending;     /**< Track whether MHI is okay to sleep. Should be cleared if MHI has work pending */ 
  
  mhi_sm_info_t mhi_sm_info;        /**< MHI state machine status information */  

  mhi_pm_info_t mhi_pm_info;        /**< MHI power management information */

  mhi_link_info_t mhi_link_info;    /**< MHI link information */
 
  mhi_hw_acc_info_t mhi_hw_acc_info; /**< MHI hardware accelerator informations */

}mhi_ctx_t; 

/* MHI Context */
extern mhi_ctx_t mhiCtx; 

#ifndef MHI_BOOT
/** 
 * @brief      mhi_core_suspend
 *
 *             Change the state of valid channels to suspended
 *              
 *
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
void mhi_core_suspend(void);

/** 
 * @brief      mhi_core_resume
 *
 *             Change the state of suspended channels to running
 *              
 *
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
void mhi_core_resume(void);
#endif /* #ifndef MHI_BOOT */

/** 
 * @brief      mhi_core_sys_error
 *
 *             Set MHI to system error state
 *              
 *
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
void mhi_core_sys_error(void);

/** 
 * @brief      mhi_core_init
 *
 *             Initialize the MHI Core
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_init(void);

/** 
 * @brief      mhi_core_deinit
 *
 *             De-initializes the MHI Core. This is primarily used during boot to clean up. 
 *             Clean up includes backing up the command ring and event rings to host memory. 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_deinit(void);

/** 
 * @brief      mhi_core_link_rw_cb
 *
 *             Main callback function used for async link r/w request completions 
 *              
 * 
 * @param[in]  ret - MHI Link return type describing the operation and status
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
void mhi_core_link_rw_cb(mhi_link_ret_t ret);

/** 
 * @brief      mhi_core_ch_pendingset
 *
 *             Set the pending channel flag bit
 *              
 * 
 * @param[in]  chId - Channel id
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
void mhi_core_ch_pendingset(uint32 chId);

/** 
 * @brief      mhi_core_ch_pendingclear
 *
 *             Clear the pending channel bits
 *              
 * 
 * @param[in]  chId - Channel id
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
void mhi_core_ch_pendingclear(uint32 chId);

/** 
 * @brief      mhi_core_send_event
 *
 *             Populate an event in event ring and notify the HOST 
 *              
 * 
 * @param[in]  erNum  - Event ring number
 * @param[in]  *el    - Event element to send
 * @param[in]  trigger_msi - Whether to trigger an msi or not 
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SUCCESS if successful 
 *              SYSERR if system error occurred
 * 
 * @sideeffect  None 
 */
uint32 mhi_core_send_event(uint32 erNum, mhi_ring_element_t *el, uint32 trigger_msi);

/**
 * @brief       mhi_core_signal_worker_thread
 *
 *              Signal MHI worker thread                
 *
 *
 * @dependencies
 *              Can only be called after MHI init
 * 
 * @return       
 *              None
 * 
 * @sideeffect  None
 */
void mhi_core_signal_worker_thread(void);

/** 
 * @brief      mhi_core_cache_hostconfig
 *
 *             Cache host configuration from host memory
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_cache_hostconfig(void);

/** 
 * @brief      mhi_core_process_ch_requestopen
 *
 *             Process any channel open requests from clients 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
uint32 mhi_core_process_ch_requestopen(void);

/**
 * @brief       mhi_core_send_exec_env_change_event
 *
 *              Send execution environment change event to the host
 *
 *
 * @param[in]   excenv - Current execution environment (SBL or AMSS)
 *
 * @dependencies
 *              None
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_core_send_exec_env_change_event(uint32 excenv);

/** 
 * @brief      mhi_core_reset
 *
 *             Resets the MHI Core layer. This is used to put MHI in reset state. 
 *              
 *
 *
 * @dependencies
 *             mhi_core_init must be first called
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_core_reset(void); 

/** 
 * @brief      mhi_core_scheduler
 *
 *             This is the heart of MHI Core. This function is called by the MHI Worker thread whenever it is awakened. 
 *             This function will check for any new interrupts or SW triggers and run the appropriate processing 
 *             routines.  
 *              
 *
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
void mhi_core_scheduler(void);

/** 
 * @brief      mhi_core_send_event
 *
 *             Populate an event in event ring and notify the HOST 
 *              
 * 
 * @param[in]  erNum  - Event ring number
 * @param[in]  *el    - Event element to send
 * @param[in]  trigger_msi - Whether to trigger an msi or not 
 *
 * @dependencies
 *              
 * 
 * @return       
 *              SUCCESS if successful 
 *              SYSERR if system error occurred
 * 
 * @sideeffect  None 
 */
uint32 mhi_core_send_event(uint32 erNum, mhi_ring_element_t *el, uint32 trigger_msi );

/** 
 * @brief      mhi_core_set_channel_state
 *
 *             Set the channel state 
 *              
 * 
 * @param[in]  chId - Channel ID 
 * @param[in]  state - State of channel to set
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, ERROR otherwise
 * 
 * @sideeffect None 
 */
uint32 mhi_core_set_channel_state(uint32 chId, mhi_ch_state_t state );

/** 
 * @brief      mhi_core_ack_interrupts
 *
 *             Acknowledge all the interrupts by reading the interrupts status registers. This will clear them 
 *             form HW and store the pending interrupts to our context. Also acknowledge any SW interrupts/triggers
 *             such as pendingChannel bit array and etc. 
 *              
 *
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
void mhi_core_ack_interrupts(void);

/*
* @brief      mhi_core_map_ctrl_and_base_regions
*
*             Map the host control and data regions to the local address space.
*             
* 
* @paran[in]  hostAddr - MHI host address configuration
* @paran[in]  *localAddr - Pointer to local MHI address configuration
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
mhi_status_t mhi_core_map_ctrl_and_base_regions(mhi_host_addr_t hostAddr, mhi_local_addr_t *localAddr); 

/*
* @brief      mhi_core_flush_link_completions
*
*             Flush all completions from pending link requests 
*             
* 
* @param[in]  ring - Ring for which completions need to be flushed. If ring is NULL, 
*                    completions for all channels will be flushed.
*
* @dependencies
*             None
*              
* 
* @return       
*             SUCCESS - Flush succeeded 
*             SYSERR - Flush failed
* 
* @sideeffect 
*             None 
*/
uint32 mhi_core_flush_link_completions(ring_t *ring);


#endif
