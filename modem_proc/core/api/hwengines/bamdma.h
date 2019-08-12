#ifndef __BAM_DMA_H__
#define __BAM_DMA_H__
/**
  @file bamdma.h
  @brief Public interface include file for the BAM DMA driver.
   
  This file contains definitions of constants, data structures, and
  interfaces that provide operational control to the BAM DMA device.

*/
/*
===============================================================================

                             Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/api/hwengines/bamdma.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
4/12/12    rl      Changed 'bamdma_get_freebytes'. 'bamdma_get_max_descriptors' returns
                   the max descriptors supported by the bamdma bam.
11/11/11   rl      add async api,replaced generic types with standard typedefs.
09/01/11   rl      Created

===============================================================================
                   Copyright (c) 2011 Qualcomm Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "comdef.h"
#define BAMDMA_API_VERSION_MAJOR  2
#define BAMDMA_API_VERSION_MINOR  0
#define BAMDMA_API_VERSION        BAMDMA_API_VERSION_MAJOR##.##BAMDMA_VERSION_MINOR

#define BAMDMA_INVALID_TOKEN      0xFFFFFFFF

/** Return type of api calls */
typedef enum 
{
    BAMDMA_SUCCESS,               /**< operation successful */
    BAMDMA_FAILED,                /**< operation failed */
    BAMDMA_TRANSFER_SUCCESS,      /**< transfer was successful */
    BAMDMA_TRANSFER_FAILED,       /**< transfer was not successful */
    BAMDMA_TRANSFER_PENDING,      /**< transfer still in progress*/
    BAMDMA_INVALID_SIZE,          /**< descriptor size invalid */
    BAMDMA_CHAN_NOT_ACCESSIBLE,   /**< channel number not accessible from this execution environment */
    BAMDMA_CHAN_IN_USE,           /**< channel in use while trying to change configuration */
    BAMDMA_CHAN_NUM_INVALID,      /**< invalid channel number */
    BAMDMA_CHAN_INVALID_HANDLE,   /**< channel handle passed is invalid */
    BAMDMA_CHAN_INVALID_TYPE,     /**< trying to use a channel of incorrect type */
    BAMDMA_SG_DESC_INVALID,       /**< SG descriptor is invalid */
    BAMDMA_RESULT_INVALID,        /**< transfer result invalid */
    BAMDMA_INSUFFICIENT_RESOURCES,/**< not enough resources to satisfy this request */
    BAMDMA_CHAN_INVALID_TOKEN,    /**< token invalid */
    BAMDMA_CHAN_NO_ACTIVE_TRANSFER/**< no active transfer for the channel */
} bamdma_ret_t;

/** channel operation mode */
typedef enum
{
    BAMDMA_CHAN_MODE_POLLING=0,
    BAMDMA_CHAN_MODE_INTR
} bamdma_chan_mode_t;

/* 
    token type used in dma calls 
 
    when a channel is operating in polling mode, a transfer request
    submission through memcpy api or ipc api, returns a token which
    identifies the transfer request. 

    When the channel is polled for a result, this token should be used
    to let the polling function know, which transfer's result is
    being requested.
 */
typedef uint32 bamdma_token_t;

/** callback object for async api calls 
   
   The asynchronous APIs take bamdma_cb_t object as an argument
   This argument contains the callback function and a user context. The user
   context and the transfer result are combined into a bamdma_result_t 
   object and passed as an argument to the user given callback function
 */
/** object representing the result of a bamdma operation */
typedef struct _bamdma_result_t
{
    void* user_data;                   /**< pointer to the user data that was passed through bamdma_cb_t variable */
    bamdma_ret_t  result;              /**< result of the bamdma operation */
}bamdma_result_t;

typedef struct _bamdma_cb_t
{
    void* user_data;                      /**< user data */
    void (*cb)(bamdma_result_t*);         /**< callback function */
} bamdma_cb_t;

/** i/o vector type for dma transfers */
typedef struct _bamdma_iov_t
{
    uint32 phy_addr;           /**< starting address. this should be a physical address */
    uint32 len;            /**< length of the src/destination buffer  in bytes*/  
} bamdma_iov_t;

/** scatter/gather type transfer descriptor. Contains a list of i/o vectors */
typedef struct _bam_dma_sg_descr_t
{
    bamdma_iov_t* iov;           /**< list of i/o vectors */
    uint32 count;                /**< io vector count */
} bamdma_sg_descr_t;

/** opaque bam_dma channel handle */
typedef void* bamdma_chan_handle_t; 

/**
  Open a bam dma channel. 

  Opens the channel before using it. This returns a channel handle
  which should be used in the subsequent api calls.
  
  @param[in]  chan_no   -  channel number to open
  @param[out] chan_handle  - channel handle
 
  @return 
    BAMDMA_SUCCESS - successfully opened the channel, and ready to use
    BAMDMA_FAILED  - open failed.
    BAMDMA_CHAN_NOT_ACCESSIBLE - the channel cannot be accessed from this EE

*/
bamdma_ret_t bamdma_chan_open(uint32 chan_id, bamdma_chan_handle_t* chan_handle);

/**
  Close a bam dma channel. 
  
  @param[in] chan_handle  - channel handle
 
  @return 
    BAMDMA_SUCCESS - successfully closed the channel.
    BAMDMA_FAILED  - close failed.
    BAMDMA_CHAN_INVALID_HANDLE - channel handle used is invalid. The channel could be
                                 already closed.
*/
bamdma_ret_t bamdma_chan_close(bamdma_chan_handle_t chan_handle); 

/**
  Change the channel operation mode.
  
  Change the channel operation mode to interrupt or polling. 
  
  @param[in] chan_handle  - channel handle
  @param[in] mode         - operation mode to change to
 
  @return 
    BAMDMA_SUCCESS - successfully changed the channel operation mode.
    BAMDMA_FAILED  - operation failed.
    BAMDMA_CHAN_INVALID_HANDLE - channel handle passed is invalid. 
  @dependencies
    Channel should be opened
*/
bamdma_ret_t bamdma_chan_set_mode(bamdma_chan_handle_t chan_handle, bamdma_chan_mode_t mode);

/**
 Returns the maximum number of descriptors that can be accepted by bamdma's bam

 @param[in] chan_handle    - channel handle
 @param[out] bytes_free    - maximum number of descriptors supported by bamdma

 @return                   
    BAMDMA_SUCCESS             - operation successfull.
    BAMDMA_FAILED              - operation failed.
    BAMDMA_CHAN_INVALID_HANDLE - channel handle passed is invalid. 
*/
bamdma_ret_t bamdma_get_max_descriptors(bamdma_chan_handle_t chan_handle,uint32* max_descriptors);

/**
  Perform a DMA transfer from a single buffer

  Transfer 'size' amount of data from src buffer to dst buffer.

  The source and destination buffers has to be 'DMA' ready, the bamdma driver
  won't do any cache management for these buffers.

  If the channel is operating in polling mode, then the token that's returned
  by the driver should be used to poll the channel ( \ref bamdma_get_poll_result ).

  If the channel is operating in interrupt mode, the callback provided will get called
  once a result is ready. 

  This api is suitable for transferring data that is contiguous

  @param[in] chan_handle  - channel handle
  @param[in] dst          - pointer to destination buffer ( physical address)
  @param[in] src          - pointer to source buffer      ( physical address)
  @param[in] size         - number of bytes to transfer
  @param[in] cb           - callback function to use to notify of results 
  @param[out] token       - token returned by the driver for polling.

  @return 
    BAMDMA_SUCCESS                - Transfer successfully queued
    BAMDMA_FAILED                 - Transfer request queuing failed
    BAMDMA_CHAN_INVALID_HANDLE    - channel handle passed is invalid. 
    BAMDMA_INSUFFICIENT_RESOURCES - not enough resouces to accept this transfer request

  @dependencies
    Channel should be opened.
*/
bamdma_ret_t bamdma_memcpy(bamdma_chan_handle_t chan_handle, uint8* dst_phy, uint8* src_phy, uint32 size,bamdma_cb_t* cb,bamdma_token_t* token);

/**
  Perform a DMA transfer using using scattered buffers.

  Move data from buffers pointed to by source descriptor list to buffers pointed
  to by destination descriptor list. The corresponding source and destination buffers
  don't require to be of same size. 
  
  The source and destination buffers has to be 'DMA' ready, the bamdma driver
  won't do any cache management for these buffers. All the buffer addresses in i/o vector
  list has to be physical addresses.

  If the channel is operating in polling mode, then the token that's returned
  by the driver should be used to poll the channel ( \ref bamdma_get_poll_result ).

  If the channel is operating in interrupt mode, the callback provided will get called
  once a result is ready. 

  This api is most suitable to perform data transfers on buffers which are not
  contiguous.

  @param[in] chan_handle  - channel handle
  @param[in] dst          - i/o vector list describing dst buffers
  @param[in] src          - i/o vector list describing src buffers
  @param[in] cb           - callback function to use to notify of results 
  @param[out] token       - token returned by the driver to polling.

  @return 
    BAMDMA_SUCCESS                - Transfer successfully queued
    BAMDMA_FAILED                 - Transfer request queuing failed
    BAMDMA_CHAN_INVALID_HANDLE    - channel handle passed is invalid. 
    BAMDMA_INSUFFICIENT_RESOURCES - not enough resouces to accept this transfer request


  @dependencies
    Channel should be opened.
*/
bamdma_ret_t bamdma_sg_memcpy(bamdma_chan_handle_t chn_handle, bamdma_sg_descr_t* dst, bamdma_sg_descr_t* src, bamdma_cb_t* cb,bamdma_token_t* token);

/**
  Query bamdma driver for transfer result for a given channel.

  Once a transfer is sumbitted through bamdma_sg_memcpy or bamdma_memcpy,
  the transfer result can be fetched through bamdma_get_poll_result.

  The call returns with the current status of the transfer.

  @param[in] chan_handle  - channel handle
  @param[in]  token         - token returned by memcpy calls

  @return
    BAMDMA_TRANSFER_SUCCESS  - transfer was successfull
    BAMDMA_TRANSFER_FAILED   - transfer failed 

*/
bamdma_ret_t bamdma_get_poll_result(bamdma_chan_handle_t chan_handle, bamdma_token_t token);

/**
  Send a given data packet to an agent in a different execution environment

  Send a given data packet to a receiver. For IPC operations, the producer and consumer
  pipes of a given channel are used. IPC sender sends data through consumer pipe, and
  the IPC receiver receives data through the producer pipe.

  The buffers have to be 'DMA' ready, the bamdma driver
  won't do any cache management for these buffers.

  If the channel is operating in polling mode, then the token that's returned
  by the driver should be used to poll the channel ( \ref bamdma_get_poll_result ).

  If the channel is operating in interrupt mode, the callback provided will get called
  once a result is ready. 


  @param[in] chan_handle  - channel handle
  @param[in] data         - pointer to data to be sent ( physical address)
  @param[in] size         - size of data to send
  @param[in] cb           - callback function to use to notify of results (interrupt mode )
  @param[out] token       - token returned by the driver to polling.
 
  @return 
    BAMDMA_SUCCESS                - Transfer successfully queued
    BAMDMA_FAILED                 - Transfer request queuing failed
    BAMDMA_CHAN_INVALID_HANDLE    - channel handle passed is invalid. 
    BAMDMA_INSUFFICIENT_RESOURCES - not enough resources to accept this transfer request


  @dependencies
    Channel should be opened.
*/
bamdma_ret_t bamdma_ipc_send(bamdma_chan_handle_t chan_handle, uint8* src_phy, uint32 size,bamdma_cb_t* cb,bamdma_token_t* token);

/**
  Receive a data packet from an agent in a different execution environment

  Receive a data packet sent by an IPC sender. For IPC operations, the producer and consumer
  pipes of a given channel are used. IPC sender sends data through consumer pipe, and
  the IPC receiver receives data through the producer pipe.

  The buffers have to be 'DMA' ready, the bamdma driver
  won't do any cache management for these buffers.

  If the channel is operating in polling mode, then the token that's returned
  by the driver should be used to poll the channel ( \ref bamdma_get_poll_result ).

  If the channel is operating in interrupt mode, the callback provided will get called
  once a result is ready. 

  @param[in] chan_handle  - channel handle
  @param[in] data         - pointer to data buffer to put received data ( physical address)
  @param[in] size         - size of the expected data.
  @param[in] cb           - callback function to use to notify of results (interrupt mode )
  @param[out] token       - token returned by the driver to polling.
 
  @return 
    BAMDMA_SUCCESS                - Transfer successfully queued
    BAMDMA_FAILED                 - Transfer request queuing failed
    BAMDMA_CHAN_INVALID_HANDLE    - channel handle passed is invalid. 
    BAMDMA_INSUFFICIENT_RESOURCES - not enough resources to accept this transfer request


  @dependencies
    Channel should be opened.
*/
bamdma_ret_t bamdma_ipc_rcv(bamdma_chan_handle_t chan_handle, uint8* data, uint32 size,bamdma_cb_t* cb, bamdma_token_t* token);

#endif
