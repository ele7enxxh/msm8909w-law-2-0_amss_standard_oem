/** 
  @file mhi_boot.c
  @brief
  This header file contains the boot interface implementation layer
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/02/14   nk      Add skip data support  
10/22/13   nk      Temporary Fix to decrease packet size  
07/22/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi.h" 
#include "boot_sahara.h" 

/* MHI Channel numbers for RX and TX  */
#define SAHARA_RX 2
#define SAHARA_TX 3

/* This cannot be bigger as SAHARA packets cannot be bigger than 512 */
#define MHI_MAX_PACKET_SIZE 512

/* MAX Data size. This can be unlimited, for now 1 MB */
#define MHI_MAX_RAW_DATA_SIZE 0x100000

#define MHI_BLACK_HOLE 0xFFFFDEAD

/* Indicates if SAHARA channel is successfully opened */ 
static boolean mhi_up=FALSE;

/* Indicates if a fatal error occurred */
static uint32 fatal_error=0; 

/* Mhi main processing loop which we will trigger via the poll */
extern void mhi_core_scheduler(void); 
 
/* Triggers pcie DMA completion polling */
extern int pcie_dma_poll(void); 

/** 
 * @brief      mhi_booti_rw_cb
 *
 *             Callback function used to receive channel request completions from MHI Core
 *              
 * 
 * @param[in]  status - Indicates status of operation
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
void mhi_booti_rw_cb(mhi_return_t status)
{
   uint32 code;  

   code = BULK_SUCCESS;

   /* Check if a fatal error occurred that we couldn't handle. Return the error code as part of the cb */
   if(fatal_error)
      code = BULK_ERR_UNKNOWN;

   switch(status.op)
   {
      case MHI_READ:
      case MHI_READ_NULL:
         if(status.status != MHI_SUCCESS)
            code=BULK_ERR_READ;

         ((sahara_rx_cb_type)status.userData)(status.payload.rw.bytesRW, code); 
         break;
      case MHI_WRITE:
         if(status.status != MHI_SUCCESS)
            code=BULK_ERR_WRITE;

         ((sahara_rx_cb_type)status.userData)(status.payload.rw.bytesRW, code); 
         break;
      default:
         /* Should not get an unknown operation. Mark the global fatal error flag */
         fatal_error=1; 
   }

   return; 
}

/** 
 * @brief      mhi_boot_init
 *
 *             Initialize the boot wrapper
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             BULK_SUCCESS if successful
 *             BULK_ERR_ENUMERATION if channel could not be opened 
 *             BULK_ERR_UNKNOWN for all other errors
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_init(void)
{
   /* Call MHI Init */
   if (MHI_SUCCESS != mhi_init())
      return BULK_ERR_UNKNOWN ; 

   /* Open the channels. This could block if host did not enumerate MHI yet */
   if( MHI_SUCCESS != mhi_channel_open(SAHARA_RX, NULL))
   {
      return BULK_ERR_ENUMERATION;
   }

   /* Open the channels. This could block if host did not enumerate MHI yet */
   if( MHI_SUCCESS != mhi_channel_open(SAHARA_TX, NULL) )
   {
      return BULK_ERR_ENUMERATION; 
   }

   mhi_up=TRUE; 
   return BULK_SUCCESS ; 
}

/** 
 * @brief      mhi_boot_shutdown
 *
 *             Shutdown MHI boot. This involves any clean-up such as saving 
 *             event ring/command rings to HOST memory to allow hand-over to 
 *             AMSS
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
void mhi_boot_shutdown(void)
{
   mhi_channel_close(SAHARA_RX);
   mhi_channel_close(SAHARA_TX);

   mhi_deinit(FALSE);
   mhi_up=FALSE; 
   return; 
}

/** 
 * @brief      mhi_boot_poll
 *
 *             Poll for any new data
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
void mhi_boot_poll(void)
{
   if (mhi_up != TRUE)
      return; 

#ifdef MHI_DMA 
   /* Trigger PCIE driver polling */
   (void)pcie_dma_poll();
#endif
   /* Trigger MHI Core processing */
   mhi_core_scheduler(); 

   return; 
}

/** 
 * @brief      mhi_boot_rx_bulk
 *
 *             Read bulk data over SAHARA RX channel
 *              
 * 
 * @param[in]  *rx_buf    - Pointer to RX buffer
 * @param[in]  len        - Length of buffer in bytes
 * @param[in]  rx_cb      - CB to be called when operation is completed 
 * @param[out] *err_code - Set if any error occurred
 * @param[in]  type       - Specifies the packet type
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Bytes read if no CB is specified
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_rx_bulk(uint8 *rx_buf,
      uint32 len,
      sahara_rx_cb_type rx_cb,
      uint32 * err_code,
      enum boot_sahara_packet_type type)
{
   uint32 bytesWritten=0; 
   mhi_cb_t cb; 
   mhi_status_t status;  

   if(err_code == NULL) 
   {
      return 0; 
   }

   /* Check that MHI is enumerated  */
   if (mhi_up != TRUE)
   {
      *err_code=BULK_ERR_ENUMERATION;
      return 0;
   }


   /* Check if a fatal error occured if so return error so it can get caught */
   if(fatal_error)
   {
      *err_code=BULK_ERR_UNKNOWN;
      return 0;
   }

   /* Prepare the cb data if we are doing an async request */
   if(rx_cb != NULL)
   {
      cb.cbFunc=mhi_booti_rw_cb; 
      cb.userData=(void*)rx_cb;
   } 

   if( MHI_BLACK_HOLE == (uint32)rx_buf)
   {
      status = mhi_channel_read_null(SAHARA_RX, len, &bytesWritten, rx_cb!=NULL?&cb:NULL);
   }
   else
   {
      status = mhi_channel_read(SAHARA_RX, (void*)rx_buf, len, &bytesWritten, rx_cb!=NULL?&cb:NULL);
   }

   if (MHI_SUCCESS != status && MHI_PENDING != status )
   {
      *err_code=BULK_ERR_WRITE;
      return 0;
   }

   return bytesWritten; 
}

/** 
 * @brief      mhi_boot_tx_bulk
 *
 *             Transmits bulk data over SAHARA TX channel
 *              
 * 
 * @param[in]  *tx_buf   - Pointer to tx buffer
 * @param[in]  len       - Length of data in bytes
 * @param[in]  tx_cb     - CB to be called when operation is completed
 * @param[out] *err_code - Set if any error occurred 
 *
 * @dependencies
 *             mhi_boot_init must be called first
 *              
 * 
 * @return       
 *             Number of bytes written if CB is not specified
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_tx_bulk(uint8 *tx_buf,
      uint32 len,
      sahara_tx_cb_type tx_cb,
      uint32 * err_code)
{
   mhi_cb_t cb;
   uint32 bytesWritten=0;
   mhi_status_t status; 

   if(err_code == NULL) 
   {
      return 0;
   }

   if (mhi_up != TRUE)
   {
      *err_code=BULK_ERR_ENUMERATION;
      return 0; 
   }


   /* Check if a fatal error occurred if so return error so it can get caught */
   if(fatal_error)
   {
      *err_code=BULK_ERR_UNKNOWN;
      return 0;
   }

   /* Prepare the cb data if we are doing an async request */
   if(tx_cb != NULL)
   {
      cb.cbFunc=mhi_booti_rw_cb; 
      cb.userData=(void*)tx_cb;
   } 

   status = mhi_channel_write(SAHARA_TX, (void*)tx_buf, len, &bytesWritten, tx_cb!=NULL?&cb:NULL);
   if (MHI_SUCCESS != status && MHI_PENDING != status )
   {
      *err_code=BULK_ERR_WRITE;
      return 0;
   }

   return bytesWritten; 
}

/** 
 * @brief      mhi_boot_get_max_packet_size
 *
 *             Return MAX packet size in bytes 
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Max packet size
 * 
 * @sideeffect None 
 */
uint32 mhi_boot_get_max_packet_size(void)
{
   return MHI_MAX_PACKET_SIZE; 
}

/** 
 * @brief      mhi_boot_get_max_raw_data_size
 *
 *             Return max raw data size. This is used to determine max raw binary 
 *             data size that can be read at one time 
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
uint32 mhi_boot_get_max_raw_data_size(void)
{
   return MHI_MAX_RAW_DATA_SIZE;
}

