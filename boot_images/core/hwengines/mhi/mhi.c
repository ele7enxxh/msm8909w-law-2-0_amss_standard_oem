/**
  @file mhi.c
  @brief
  This file contains the implementation of the public MHI device driver interface
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/21/14   nk      Add read null data API 
10/22/13   nk      New algorithm for ring processing, improvements, fixes, DMA support
08/15/13   am      Moved MHI state machine
06/25/13   nk      IPA initalization integration 
01/15/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi.h"
#include "mhi_i.h"
#include "mhi_osal.h" 
#include "mhi_mmio.h"
#include "mhi_link.h"
#include "mhi_ring.h"
#include "mhi_core.h"
#include "mhi_util.h"
#include "mhi_hwacc.h"
#include "mhi_sm.h"
#include "mhi_pm.h"


/** Max requests per channel. Size of this affects how many requests are queued in MHI layer */ 
#define MAX_REQUESTS_PER_CH 40


/******************************** Driver structures *********************************************/

/* Notification type used in synchronous API when blocking 
 * on an async request to MHI CORE */
typedef struct _notify
{
   mhi_osal_event_handle hEvent; /**< Handle to notification event **/
   mhi_osal_event event;         /**< Event object **/ 
   mhi_return_t   *ret;          /**< Pointer to return object */
}notify_t; 

/* Static MHI Context, used by various components of MHI driver  */
mhi_ctx_t mhiCtx; 

/** 
 * @brief      mhi_is_enumerated
 *
 *             Returns true if MHI is enumerated by host 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             TRUE if MHI is enumerated, FALSE otherwise 
 * 
 * @sideeffect  None 
 */
boolean mhi_is_enumerated(void)
{
   return mhiCtx.enumerated; 
}

/**
 * @brief      mhi_init
 *
 *             Initialize the MHI SW stack. Among other things, this includes initializing 
 *             MMIO registers, internal data structures, MHI link layer, core layer, power management and state machine. 
 *             Once finished set MHI status to ready to indicate to Host that we are initialized.
 *              
 *
 * @dependencies
 *             None 
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_init(void)
{
   /* Initialize the OSAL */
   mhi_osal_init(); 
   mhi_osal_debug(MSG_HIGH, "mhi_init: Initializing"); 

   /* Zero out the MHI context */
   mhi_osal_memset(&mhiCtx, 0, sizeof(mhiCtx));
   mhiCtx.enumerated=FALSE; 

   /* Initialize the MHI Sync object and set the MHI version we support */  
   mhiCtx.mhiConfig.version = MHIVERSION;
   mhi_osal_syncinit(&mhiCtx.mhiSync); 

   /* Initialize the link */
   mhiCtx.mhi_link_info.mhiConfig = &mhiCtx.mhiConfig;
   mhiCtx.mhi_link_info.cbFunc=mhi_core_link_rw_cb; 
   if(MHI_SUCCESS != mhi_link_init(&mhiCtx.mhi_link_info))
   {
      return MHI_ERROR; 
   }

   /* Initialize MMIO */
   if(MHI_SUCCESS != mhi_mmio_init(MMIO_BASEPHYS, &mhiCtx.mhiConfig))
   {
      mhi_osal_debug(MSG_ERR, "mhi_init: Failed to initialize MMIO");
      goto err; 
   }

   /* Initialize core MHI functionality */ 
   if (0 != mhi_core_init())
   {
      mhi_osal_debug(MSG_ERR, "mhi_init: MHI Core initialization failed"); 
      goto err; 
   }

   mhi_osal_debug(MSG_HIGH, "mhi_init: Initialization complete"); 

   /* Set enumerated flag in MHI Ctx to true */
   mhiCtx.enumerated=TRUE; 
   return MHI_SUCCESS;

err:
   mhi_osal_debug(MSG_FATAL,"mhi_init: Fatal error occurred");
   return MHI_ERROR; 
}

/** 
 * @brief      mhi_deinit
 *
 *             De-Initialize the MHI driver 
 *              
 * 
 * @param[in]  reset - Whether to reset the MMIO or leave it as is 
 *
 * @dependencies
 *             mhi_init must be first called
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_deinit(boolean reset)
{
   /* Currently support de-init only in BOOT */
#ifdef MHI_BOOT

   /* De-init the MHI Core */
   mhi_core_deinit(); 

   /* Have no current use case for resetting MMIO */
   if(TRUE == reset)
   {
      mhi_osal_debug(MSG_FATAL, "mhi_deinit: Reset of MMIO not supported on de-init"); 
   }
#endif

   return MHI_SUCCESS;
}

/** 
 * @brief      mhi_channel_async_cb
 *
 *             Asynchronous CB used by various function in MHI.c 
 *              
 * 
 * @param[in]  ret - MHI return indicating status and type of operation
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
void mhi_channel_async_cb(mhi_return_t ret)
{
   notify_t *notify=(notify_t*)ret.userData; 

   *notify->ret=ret; 

   /* Signal the caller */
   mhi_osal_eventsignal(notify->hEvent);
}

/**
 * @brief      mhi_channel_open
 *
 *             Opens an MHI channel. This must be called before doing any read/write operations 
 *             on the channel. A return, with one of the below values  is generated immediately if 
 *             MHI STATE != MHI_STATE_READY. If state is READY, this function will wait for the HOST to finish
 *             configuring channels before it returns it's result. If however a CB is specified, then it
 *             will do an async notification once MHI has started.               
 *
 * @param[in]  chId - ID of channel to open. Refer to latest MHI SPEC for list of channels
 * @param[in]  cb   - Callback type that specifies user supplied data and function to be called for 
 *                    async notification. 
 *
 * @dependencies
 *              mhi_init must be called before any other API 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_CH_INVAlID if channel is invalid or not enabled
 *              MHI_PENDING a result will be notified asynchronously
 * 
 * @sideeffect  None 
 */
mhi_status_t mhi_channel_open(uint32 chId, mhi_cb_t *cb)
{
   uint32 i; 
   uint32 count=5000; 
   mhi_request_t *request; 
   ch_t *ch; 
   mhi_return_t req_result; 
   notify_t notify; 
   mhi_cb_t lCb; 
   mhi_status_t ret; 

   if(chId > mhiCtx.mhiConfig.nCh)
   {
      mhi_osal_debug(MSG_ERR,"mhi_channel_open: Invalid channel passed to channel open");
      return MHI_CH_INVALID;
   }

   /* if no CB is specified, use an internal one */  
   if(NULL == cb)
   {
      /* Use a local result structure to get data */
      req_result.status=MHI_PENDING; 
      notify.ret=&req_result;
      mhi_osal_eventcreate(&notify.hEvent, &notify.event); 
      lCb.userData=&notify; 
      lCb.cbFunc=mhi_channel_async_cb;
   }
#ifdef NOINTERRUPTS
   else 
   {
      mhi_osal_debug(MSG_ERR,"mhi_channel_open: Cannot do async without interrupts");
      return MHI_ERROR;
   }
#endif

   /* Grab the main sync object to prevent two opens at same time */
   mhi_osal_syncenter(&mhiCtx.mhiSync);

   /* Get the channel pointer */
   ch=&mhiCtx.ch[chId];

   if(CH_PENDING_OPEN == ch->ch_client_state)
   {
      mhi_osal_syncleave(&mhiCtx.mhiSync);
      return MHI_PENDING; 

   }
   else
   {
      /* Mark channel as pending open */ 
      ch->ch_client_state = CH_PENDING_OPEN; 
   } 

   /* Initialize channel sync */
   mhi_osal_syncinit(&ch->chSync);

   /* Allocate request queue for aync calls and initialize queues */
   if(NULL == (ch->requests = mhi_osal_malloc(sizeof(mhi_request_t)*MAX_REQUESTS_PER_CH)))
   {
      mhi_osal_debug(MSG_FATAL,"mhi_channel_open: Unable to allocate channel request pool" );
      mhi_osal_syncleave(&mhiCtx.mhiSync);
      return MHI_ERROR; 
   }
   request = (mhi_request_t*)ch->requests;

   /* Initialize queues */
   mhi_q_init(&ch->request_q); 
   mhi_q_init(&ch->request_free_q); 
   mhi_q_init(&ch->request_q_issued); 

   /* Build the request queue */
   for(i=0; i < MAX_REQUESTS_PER_CH; i++)
   {
      mhi_q_queue(&ch->request_free_q,(list_t*)&request[i]);
   } 

   /* Initialize channel parameters to initial state */
   ch->trb_bytes_processed = 0; 
   ch->skip_to_td = 0;

   /* Check MHI state, if not running we will have to block here on a signal,  or do a cb */ 
   if(MHI_STATE_READY == mhi_sm_get_mhistate())
   {
      /* enqueue request to a channel and to the task queue  */
      mhi_q_dequeue(&mhiCtx.ch[chId].request_free_q,(list_t**)&request);
      if (NULL == request)
      {
         mhi_osal_debug(MSG_FATAL,"mhi_channel_open: dequeued NULL request from free q");
         return MHI_ERROR;
      }
      request->ret.status=MHI_PENDING; 
      request->ret.chid=chId; 
      request->cb = (cb == NULL?lCb:*cb); 
      request->op=MHI_OPEN; 
      mhi_q_queue(&mhiCtx.ch[chId].request_q, (list_t*)request); 

      /* Leave sync before waiting */
      mhi_osal_syncleave(&mhiCtx.mhiSync);

      /* if operation is async return */ 
      if(NULL != cb)
      {
         ret=MHI_PENDING; 
         goto end; 
      }

      for(;;)
      {
#ifndef NOINTERRUPTS
         mhi_osal_eventwait(notify.hEvent);
         NOT_USED(count);
#else
         mhi_core_scheduler(); 
         {
            mhi_osal_wait(1);
#ifdef MHI_CH_OPEN_TIMEOUT            
            count--;
#endif
            /* Abort the transaction in one second in polling environment */
            if(0 == count)
            {
               req_result.status=MHI_ABORTED;
               break; 
            }
         }
#endif

         if(MHI_PENDING != req_result.status)
            break; 
      }

   }    
   else
   {
      /* Make sure channel is enabled */
      if(MHI_CH_STATE_DISABLED != mhiCtx.lChCtxBase[chId].chstate)
      {
         /* Initialize channel */
         req_result.status = MHI_SUCCESS;
         ch->ch_client_state=CH_OPENED; 
      }
      else
      {
         req_result.status = MHI_CH_INVALID; 
      }

      mhi_osal_syncleave(&mhiCtx.mhiSync);
   }

   ret = req_result.status; 

end:
   /* Destroy the local event if it was used */
   if(NULL != notify.hEvent)
      mhi_osal_eventdestroy(&notify.hEvent);

   return ret; 
} 

/** 
 * @brief      mhi_channel_abort
 *
 *             Aborts issued channel operations  
 *              
 * 
 * @param[in]  chId - Channel id
 *
 * @dependencies
 *             mhi_channel_open must be called first
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_channel_abort(uint32 chId)
{
   ch_t *ch;
   mhi_request_t *req; 

   /* Validate channel id */ 
   if(chId > mhiCtx.mhiConfig.nCh)
   {
      mhi_osal_debug(MSG_ERR,"mhi_channel_abort: Invalid channel passed in");
      return MHI_CH_INVALID; 
   }

   ch=&mhiCtx.ch[chId];

   /* Grab the channel mutex  */
   mhi_osal_syncenter(&ch->chSync);

   /* Make sure channel was opened before */
   if(CH_OPENED != ch->ch_client_state)
   {
      mhi_osal_debug(MSG_ERR, "mhi_channel_abort: Client attempting to close a channel that wasn't opened"); 
      mhi_osal_syncleave(&ch->chSync); 
      return MHI_CH_INVALID; 
   }
   
   /* Cancel all queued requests */
   while(!mhi_q_is_queue_empty(&ch->request_q))
   {
      mhi_q_dequeue(&ch->request_q, (list_t**)&req); 

      if(NULL == req)
      {
         mhi_osal_debug(MSG_FATAL, "mhi_channel_abort: Fatal error, Request is null");
         return MHI_ERROR;
      }

      /* Make a copy of cb function and return value before we release the request */
      req->ret.userData=req->cb.userData;
      req->ret.op=req->op;
      req->ret.payload.rw.buff=req->params.rw.data; 
      req->ret.status = MHI_ABORTED; 

      /* Clear the op */
      req->op=MHI_INVALID;

      /* Issue a CB if last_op is set */
      if(req->params.rw.last_op)
      {
         req->cb.cbFunc(req->ret); 
      }
   }

   mhi_osal_syncleave(&ch->chSync); 

   return MHI_SUCCESS;
}

/**
 * @brief      mhi_channel_close
 *
 *             Closes an MHI channel that was previously opened. The channel must be in DISABLED/ENABLED 
 *             If it is not the channel will be set to ERROR state and an error condition will be generated
 *             to the host. This function is not expected to be called unless we are doing a total cleanup.  
 *
 * @param[in]  chId - ID of channel.  Refer to <MHISPEC> for list of channels
 * @param[in]  cb   - Callback type that specifies user supplied data and function to be called for 
 *                    async notification. If null, a sync operation is performed
 *
 * @dependencies
 *             Channel must be first opened. 
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 *             MHI_CH_INVALID if channel was never opened 
 *             MHI_CH_NOT_IDLE if channel is not in DISABLED/ENABLED state
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_channel_close(uint32 chId)
{
   mhi_status_t ret = MHI_SUCCESS; 
   ch_t *ch;
   mhi_cb_t lCb; 
   mhi_return_t result; 
   notify_t notify; 

   /* Initialize to NULL */
   notify.hEvent=NULL; 

   /* Validate channel id */ 
   if(chId > mhiCtx.mhiConfig.nCh)
   {
      mhi_osal_debug(MSG_ERR,"mhi_channel_close: Invalid channel passed in");
      return MHI_CH_INVALID; 
   }

#ifdef NOINTERRUPTS
   /* Update MHI state */
   mhi_core_scheduler(); 
#endif

   ch=&mhiCtx.ch[chId];

   /* Grab the channel mutex  */
   mhi_osal_syncenter(&ch->chSync);

   if(CH_PENDING_CLOSE == ch->ch_client_state)
   {
      mhi_osal_syncleave(&ch->chSync);
      return MHI_PENDING; 
   }

   /* Make sure channel was opened before */
   if(CH_OPENED != ch->ch_client_state)
   {
      mhi_osal_debug(MSG_ERR, "mhi_channel_close: Client attempting to close a channel that wasn't opened"); 
      ret=MHI_CH_INVALID; 
      mhi_osal_syncleave(&ch->chSync); 
      goto end; 
   }

   /* Mark channel as pending close */ 
   ch->ch_client_state = CH_PENDING_CLOSE; 
   
   /* Check if the channel has any outstanding requests. If it does we need to wait for DMA completion since 
    * it's pending in the HW. Otherwise we can immediately close */
   if(!mhi_q_is_queue_empty(&ch->request_q_issued))
   {
      notify.ret = &result; 
      lCb.userData=&notify; 
      lCb.cbFunc=mhi_channel_async_cb;
      result.status = MHI_PENDING;
      mhi_osal_eventcreate(&notify.hEvent, &notify.event); 

      ch->close_cb=lCb;
      mhi_osal_syncleave(&ch->chSync); 

      for(;;)
      {

#ifndef NOINTERRUPTS
         mhi_osal_eventwait(notify.hEvent); 
         break;
#else
         mhi_core_scheduler(); 
         if(notify.ret->status == SUCCESS)
         {
            break;
         }
#endif
      }

   }

   mhi_osal_syncenter(&ch->chSync);

   /* Clean up the channel */
   mhi_osal_free(ch->requests); 

   mhi_osal_syncleave(&ch->chSync);
   mhi_osal_syncdestroy(&ch->chSync); 

   /* Mark the channel as closed. After this all ring ring completion CB have stopped */
   ch->ch_client_state = CH_CLOSED; 

   return MHI_SUCCESS; 

end:
   return ret; 

} 

/** 
 * @brief      mhi_channel_rw_multiple
 *
 *             Internal function that issues multiple reads/writes on a particular channel. This function can only be used in  
 *             asynchronous mode. In asynchronous mode tt will return immediately, and the cb supplied 
 *             will be called with the status of the operation
 *             
 *              
 * 
 * @param[in]  chId  - Channel to read/write on 
 * @param[in]  *data_array - Pointer to local data buffer array to read in to , or write from
 * @param[in]  *size_array  - Pointer to size array describing the sizes of the buffers 
 * @param[in]  nElements - Number of buffers in data and size array  
 * @param[out] *nSubmitted - Number of buffers that were actually submitted  
 * @param[in]  bytesrw  - Number of bytes read/written. Applies only to MHI_WRITE or MHI_READ   
 * @param[in]  *cb      - CB to be used for asynchronous operation 
 * @param[in]  op -  MHI operation read or write
 *
 * @dependencies
 *             mhi_channel_open must be called first
 *              
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 * 
 * @sideeffect None 
 */
mhi_status_t mhi_channel_rw_multiple(uint32 chId, void** data_array, uint32* size_array, uint32 nElements, uint32 *nSubmitted, uint32* bytesrw, mhi_cb_t *cb, mhi_op_t op) 
{
   mhi_status_t ret = MHI_SUCCESS; 
   ch_t *ch;
   mhi_request_t *request; 
   uint32 i; 

   /* Notification structure used for sync events */
   notify_t notify; 

   mhi_cb_t lCb; 
   mhi_return_t req_result; 

   /* Initialize to NULL */
   notify.hEvent=NULL; 

   if(chId > mhiCtx.mhiConfig.nCh)
   {
      mhi_osal_debug(MSG_ERR,"mhi_channel_rw_multiple: Invalid channel passed in");
      return MHI_CH_INVALID; 
   }

   if(NULL == data_array)
   {
      mhi_osal_debug(MSG_ERR, "mhi_channel_rw_multiple: Invalid data array passed in"); 
      return MHI_INVALID_ADDR; 
   }

   if(NULL == nSubmitted || NULL == size_array)
   {
      mhi_osal_debug(MSG_ERR, "mhi_channel_rw_multiple: Bad parameters received"); 
      return MHI_INVALID_PARAM; 
   }

#ifdef NOINTERRUPTS
   /* Update MHI state */
   mhi_core_scheduler(); 
#endif

   ch=&mhiCtx.ch[chId];

   /* Grab the channel mutex  */
   mhi_osal_syncenter(&ch->chSync);

   /* Make sure channel was opened before */
   if(CH_OPENED != ch->ch_client_state)
   {
      mhi_osal_debug(MSG_ERR, "mhi_channel_rw_multiple: Client attempting to rw on channel that wasn't opened"); 
      ret=MHI_CH_INVALID; 
      mhi_osal_syncleave(&ch->chSync); 
      goto end; 
   }

   /* Validate channel direction */
   if((MHI_WRITEM==op||MHI_WRITE==op?MHI_CH_TYPE_INBOUND:MHI_CH_TYPE_OUTBOUND) != mhiCtx.lChCtxBase[chId].chtype)
   {
      mhi_osal_debug(MSG_ERR,"mhi_channel_rw_multiple: Channel direction not compatible with operation ");
      ret = (MHI_WRITEM == op || MHI_WRITE == op?MHI_CH_INVALID_WRITE:MHI_CH_INVALID_READ);
      mhi_osal_syncleave(&ch->chSync); 
      goto end;
   }

   /* if no CB is specified, use an internal one. */  
   if(NULL == cb)
   {
      /* Synchronous mode only supported for single read/write so bytesrw needs to be supplied */ 
      if(NULL == bytesrw) 
      {
         ret = MHI_INVALID_PARAM; 
         mhi_osal_syncleave(&ch->chSync); 
         goto end;
      }
      /* Use a local result structure to get data */
      req_result.status = MHI_PENDING; 
      notify.ret=&req_result; 
      mhi_osal_eventcreate(&notify.hEvent, &notify.event); 
      lCb.userData=&notify; 
      lCb.cbFunc=mhi_channel_async_cb;

   }

   *nSubmitted=0;  

   /* For each buffer queue request. Mark the last request with last_op flag which will generate an EOT to host */
   for(i = 0; i < nElements; i++)
   { 
      /* Stop enqueueing requests if we run out */
      if(0 != mhi_q_dequeue(&mhiCtx.ch[chId].request_free_q,(list_t**)&request))
      {
         ret = MHI_OP_FULL;
         mhi_osal_syncleave(&ch->chSync);

         /* Set the channel pending flag if we inserted any requests. If requests were inserted i won't be 0 */
         if(0 != i)
         {
            mhi_core_ch_pendingset(chId); 

            /*  Notify the workloop*/
            mhi_osal_eventsignal(mhiCtx.hNotify);
         }

         goto end; 
      }

#ifndef MHI_BOOT
      /* enqueue request to a channel */
      if(NULL == data_array[i])
      {
         mhi_osal_debug(MSG_FATAL, "mhi_channel_rw_multiple: Buffer is NULL"); 
      }
#endif

      request->params.rw.data=(uint32)*(data_array+i); 
      request->params.rw.size=size_array[i];  
      request->params.rw.last_op=FALSE;
      request->cb = (NULL == cb?lCb:*cb); 
      request->op=op; 
      request->ret.payload.rw.bytesRW=0;
      request->ret.chid=chId; 
      request->ret.status=MHI_PENDING; 
      mhi_q_queue(&mhiCtx.ch[chId].request_q, (list_t*)request); 
      (*nSubmitted)++;
   }

   /* for last request mark the last op to indicate its end of a chain */ 
   request->params.rw.last_op=TRUE; 

   /* Set the channel pending flag  */
   mhi_core_ch_pendingset(chId); 

   mhi_osal_syncleave(&ch->chSync);

   /*  Notify the workloop */
   mhi_osal_eventsignal(mhiCtx.hNotify);

   ret=MHI_PENDING; 

   /* if operation is async return */ 
   if(NULL != cb)
   {
      ret=MHI_PENDING; 
      goto end; 
   }

   for(;;)
   {
      if(MHI_PENDING != req_result.status)
         break;
#ifndef NOINTERRUPTS
      mhi_osal_eventwait(notify.hEvent); 
#else
      mhi_core_scheduler(); 
#endif
   }

   /* Destroy the local event if it was used */
   if(NULL != notify.hEvent)
      mhi_osal_eventdestroy(&notify.hEvent);

   ret = req_result.status;

   /* Set the bytesrw, this is only used for single reads/writes in synchronous mode. For multiple reads/writes 
    * more information is needed which is passed via a cb */
   *bytesrw=req_result.payload.rw.bytesRW;

end: 
   return ret;
}

/**
 * @brief      mhi_channel_write
 *
 *             Writes data over a channel. The channel direction must be IN. If CB is specified 
 *             the function will return with MHI_PENDING and perform an async notification
 *             once the write is completed. Otherwise this function will block until completion
 *             of the write. payload.bytesWritten will be set to actual amount of bytes written. 
 *             Likewise in synchronous mode, bytesWritten will be set to actual size written.   
 *
 * @param[in]  chId - ID of channel.  Refer to <MHISPEC> for list of channels
 * @param[in]  data - physical address of data to send 
 * @param[in]  size - number of bytes to write 
 * @param[out] bytesWritten - actual amount of bytes written
 * @param[in]  cb   - Callback type that specifies user supplied data and function to be called for
 *                    async notification 
 *
 * @dependencies
 *              Channel must be first opened. 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_CH_INVALID if channel is invalid 
 *              MHI_CH_INVALID_WRITE channel direction does not permit a write 
 *              MHI_INVALID_ADDR data buffer address is invalid
 *              MHI_PENDING a result will be notified asynchronously
 *              MHI_INCOMPLETE - bytes were written but not all of them 
 *
 * 
 * @sideeffect  None 
 */

mhi_status_t mhi_channel_write(uint32 chId, void* data, uint32 size, uint32 *bytesWritten, mhi_cb_t *cb )
{
   uint32 nSubmitted; 

   return mhi_channel_rw_multiple(chId, &data, &size, 1, &nSubmitted, bytesWritten, cb, MHI_WRITE);
}

/**
 * @brief      mhi_channel_read
 *
 *             Reads data over a channel. The channel direction must be OUT. If CB is specified
 *             the function will return with MHI_PENDING and perform an async notification 
 *             once the read is completed . Otherwise the function will block until completion  
 *             of the read. payload.bytesRead of mhi_return_t will be set to actual number of bytes read. 
 *             Clients should always check bytesRead, as a read can be terminated by
 *             EOT (end of transaction) 
 *
 * @param[in]  chId - ID of channel.  Refer to <MHISPEC> for list of channels
 * @param[out] data - physical address of data buffer to receive the read data 
 * @param[in]  size - size of data buffer in bytes
 * @param[out] bytesRead - bytes actually read
 * @param[in]  cb   - Callback type that specifies user supplied data and function to be called for
 *                    async notification 
 *
 * @dependencies
 *              Channel must be first opened. 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_CH_INVALID if channel is invalid 
 *              MHI_CH_INVALID_READ channel direction does not permit a read 
 *              MHI_INVALID_ADDR if data address is invalid
 *              MHI_PENDING a result will be notified asynchronously
 *
 * 
 * @sideeffect  None 
 */
mhi_status_t mhi_channel_read(uint32 chId, void* data, uint32 size, uint32 *bytesRead, mhi_cb_t *cb )
{
   uint32 nSubmitted; 

   return mhi_channel_rw_multiple(chId, &data, &size, 1, &nSubmitted, bytesRead, cb, MHI_READ);
}

/**
 * @brief      mhi_channel_read_null
 *
 *             Reads null data. As optimization no actual read is performed, rather we only consume the ring TRB by the size 
 *             specified
 *              
 * 
 * @param[in]  chId - Channel id 
 * @param[in]  size - Size to consume
 * @param[in]  *bytesRead - Set to actual size consumed if api is used synchronously
 * @param[in]  *cb - CB type that specifies user supplied data and function to be called for async notification
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
mhi_status_t mhi_channel_read_null(uint32 chId, uint32 size, uint32 *bytesRead, mhi_cb_t *cb )
{
   uint32 nSubmitted; 
   void *buff=NULL;

   return mhi_channel_rw_multiple(chId, &buff, &size, 1, &nSubmitted, bytesRead, cb, MHI_READ_NULL);
}

/**
 * @brief      mhi_channel_read_multiple
 *
 *             Reads multiple data requests over a channel. The channel direction must be OUT. 
 *             The operation of this request will be asynchronous.  
 *             payload.bytesRead of mhi_return_t will be set to actual number of bytes read. 
 *             Client should always check bytesRead, as a read can be terminated by
 *             EOT (end of transaction) 
 *
 * @param[in]  chId - ID of channel.  Refer to <MHISPEC> for list of channels
 * @param[in]  *data_array - physical address of data buffer to receive the read data 
 * @param[in]  *size_array - size of data buffer in bytes
 * @param[in]  nElements - Number of buffers in data and size array  
 * @param[out] *nSubmitted - Number of buffers that were actually submitted  
 * @param[in]  cb   - Callback type that specifies user supplied data and function to be called for
 *                    async notification 
 *
 * @dependencies
 *              Channel must be first opened. 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_CH_INVALID if channel is invalid 
 *              MHI_CH_INVALID_READ channel direction does not permit a read 
 *              MHI_INVALID_ADDR if data address is invalid
 *              MHI_PENDING a result will be notified asynchronously all buffers accepted
 *              MHI_OP_FULL - operation queue full, not all buffers accepted. Check nSubmitted
 *
 * 
 * @sideeffect  None 
 */

mhi_status_t mhi_channel_read_multiple(uint32 chId, void** data_array, uint32* size_array, uint32 nElements, uint32 *nSubmitted, mhi_cb_t *cb) 
{
   return mhi_channel_rw_multiple(chId, data_array, size_array, nElements, nSubmitted, NULL,  cb, MHI_READM);    
} 

/**
 * @brief      mhi_channel_write
 *
 *             Writes multiple data requests over a channel. The channel direction must be IN. 
 *             The operation of this request will be asynchronous.  
 *             payload.bytesRead of mhi_return_t will be set to actual number of bytes written. 
 *
 * @param[in]  chId - ID of channel.  Refer to <MHISPEC> for list of channels
 * @param[in]  *data_array - physical address of data buffer to receive the read data 
 * @param[in]  *size_array - size of data buffer in bytes
 * @param[in]  nElements - Number of buffers in data and size array  
 * @param[out] *nSubmitted - Number of buffers that were actually submitted  
 * @param[in]  cb   - Callback type that specifies user supplied data and function to be called for
 *                    async notification 
 *
 * @dependencies
 *              Channel must be first opened. 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_CH_INVALID if channel is invalid 
 *              MHI_CH_INVALID_READ channel direction does not permit a read 
 *              MHI_INVALID_ADDR if data address is invalid
 *              MHI_PENDING a result will be notified asynchronously all buffers accepted
 *              MHI_OP_FULL - operation queue full, not all buffers accepted. Check nSubmitted
 *
 * 
 * @sideeffect  None 
 */
mhi_status_t mhi_channel_write_multiple(uint32 chId, void** data_array, uint32* size_array, uint32 nElements, uint32 *nSubmitted, mhi_cb_t *cb) 
{
   return mhi_channel_rw_multiple(chId, data_array, size_array, nElements, nSubmitted, NULL, cb, MHI_WRITEM);    
} 


