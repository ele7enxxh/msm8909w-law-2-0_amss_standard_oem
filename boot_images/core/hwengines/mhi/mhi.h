#ifndef MHI_H
#define MHI_H
/** 
  @file mhi.h
  @brief
  This header files defines the public interface to the MHI Device Driver 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/21/14   nk      Add read null op 
01/15/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "com_dtypes.h"

/** Return codes used by MHI API's **/
typedef enum _mhi_status
{
   MHI_SUCCESS=0x0,             /**< Operation was a success */
   MHI_ERROR=0x1,               /**< Generic internal error */
   MHI_INVALID_PARAM=0x2,       /**< Invalid Parameters */ 
   MHI_CH_INVALID=0x3,          /**< Invalid Channel Specified */
   MHI_CH_NOT_IDLE=0x4,         /**< Channel not idle  */ 
   MHI_CH_INVALID_READ=0x5,     /**< Channel read operation is invalid */   
   MHI_CH_INVALID_WRITE=0x6,    /**< Channel write operation is invalid */   
   MHI_INVALID_ADDR=0x7,        /**< Invalid Address */ 
   MHI_INVALID_TOKEN=0x8,       /**< Token is invalid */ 
   MHI_INVALID_EVENT=0x9,       /**< Invalid notification event */ 
   MHI_NOTIFY_FULL=0xA,         /**< MHI cannot except any more notification requests */ 
   MHI_PENDING=0xB,             /**< Action is pending, a cb will be returned */
   MHI_ABORTED=0xC,             /**< Operation Aborted */ 
   MHI_OP_FULL=0xD              /**< MHI operation queue full, cannot accept more requests */ 
}mhi_status_t;

/** MHI operations **/
typedef enum _mhi_op
{
   MHI_INVALID=0x0,
   MHI_OPEN=0x1,  
   MHI_CLOSE=0x2,
   MHI_WRITE=0x3,
   MHI_READ=0x4,
   MHI_READ_NULL=0x5,
   MHI_NOTIFY=0x6,
   MHI_READM=0x7,
   MHI_WRITEM=0x8
}mhi_op_t; 

/** MHI Return type used by callback functions. 
 *  It contains user supplied data and status of the operation.  */
typedef struct _mhi_return
{
   mhi_op_t  op;            /**< Operation performed */
   mhi_status_t status;     /**< Status of the operation */
   uint32       chid;       /**< Which channel this operation is for */ 
   void* userData;          /**< User supplied data */ 
   
   union _payload           /**< Additional return payload depending on the operation */ 
   {
      /**< Used for MHI_WRITE/MHI_READ/MHI_READM/MHI_WRITEM operations */
      struct _rw
      {
         uint32 bytesRW;      
         uint32 buff;       /**< Buffer address  */ 
      }rw;
   }payload;

}mhi_return_t;

/** User callback function to be called for async operation */ 
typedef void (*mhi_cb_func_t)(mhi_return_t status); 

/** MHI CB type supplied with some API to allow asynchronous notification
 *  of the completion result  */  
typedef struct _mhi_cb
{
   void* userData;       /**< User supplied data */
   mhi_cb_func_t cbFunc; /**< User supplied callback function */
}mhi_cb_t;


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
boolean mhi_is_enumerated(void);

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
mhi_status_t mhi_init(void);

/** 
 * @brief      mhi_deinit
 *
 *             Deinitialize the MHI SW stack, MMIO registers, internal data structures,
 *             TRB caches. 
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
mhi_status_t mhi_deinit(boolean reset);

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
mhi_status_t mhi_channel_open(uint32 chId, mhi_cb_t *cb);

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
mhi_status_t mhi_channel_close(uint32 chId);

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
mhi_status_t mhi_channel_write(uint32 chId, void* data, uint32 size, uint32 *bytesWritten, mhi_cb_t *cb );

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
mhi_status_t mhi_channel_read(uint32 chId, void* data, uint32 size, uint32 *bytesRead, mhi_cb_t *cb );

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
mhi_status_t mhi_channel_read_null(uint32 chId, uint32 size, uint32 *bytesRead, mhi_cb_t *cb );


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
mhi_status_t mhi_channel_read_multiple(uint32 chId, void** data_array, uint32* size_array, uint32 nElements, uint32 *nSubmitted, mhi_cb_t *cb);

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
mhi_status_t mhi_channel_write_multiple(uint32 chId, void** data_array, uint32* size_array, uint32 nElements, uint32 *nSubmitted, mhi_cb_t *cb);

/** 
 * @brief      mhi_channel_abort
 *
 *             Aborts issued channel operations  
 *              
 * 
 * @param[in]  chid - Channel id
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
mhi_status_t mhi_channel_abort(uint32 chid);


/* Return if mhi is enumerated */
boolean mhi_is_enumerated(void);
#endif
