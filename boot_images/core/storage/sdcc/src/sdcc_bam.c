/******************************************************************************
 * sdcc_bam.c
 *
 * This file provides function to support data transport through BAM
 *
 * Copyright (c) 2011 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *
 *****************************************************************************/
/*======================================================================
                        Edit History
$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/storage/sdcc/src/sdcc_bam.c#1 $
$DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $

when         who     what, where, why
----------   ---     ---------------------------------------------------
2011-12-14   rh      Initial Revision
=======================================================================*/

/*======================================================================
                    INCLUDE FILES FOR MODULE
=======================================================================*/
#include "sdcc_priv.h"
#include "sdcc_bam.h"
#include "sdcc_bsp.h"

/*======================================================================
                    FUNCTION DEFINITIONS
=======================================================================*/

#define BAM_COMPLETE_CHECK_COUNT  200
#define BAM_DESCRIPTOR_BUF_SIZE   (64)
#define BAM_MAX_TRANSFER_SIZE     (BAM_DESCRIPTOR_BUF_SIZE / 8) * (48 * 1024)
#define mbr() sdcc_bsp_cache_operation (SDCC_BSP_MEM_BARRIER, NULL, 0)
                                             /* Xfer descriptor buffer */
unsigned char sdcc_bam_desc_buf[BAM_DESCRIPTOR_BUF_SIZE];  
bam_pdata_t sdcc_bam_priv_data;

/******************************************************************************
*   Generate and update the BAM descriptor table
*
* Arguments:
*    pdesc : Pointer to the BAM descriptor table
*    phy_buffer : Physical address of the buffer that contains data 
*    count : Number of bytes to transfer
*
* Returns:
*    Number of descriptor created
******************************************************************************/
static uint32 _sdcc4_write_bam_descriptor (
       uint32 *pdesc, 
       uint32 phy_buffer, 
       uint32 count )
{
   uint32 total;
   uint32 len;
   const uint32 max_bytes_per_packet = 1024 * 48;  // 48K bytes per descriptor
   uint32 rtn = 0;
   
   total = count;
   do {
      if (total > max_bytes_per_packet) {
         len = max_bytes_per_packet;
         total -= max_bytes_per_packet;
      }
      else
      {
         len = total;
         total = 0;
      }
      // Build the transaction descriptor
      *pdesc++ = phy_buffer;       // Physical address of where the data will go
      *pdesc++ = (len & BAM_DESC_LEN_BMSK) | (total == 0 ? BAM_DESC_EOT_BIT : 0);
      phy_buffer += len;           // Move to the next location
      rtn += 8;                    // Size of each packet, 2 DWORD
   } while (total);
   mbr();
   return rtn;
}

/******************************************************************************
*   Setting up an BAM based DMA transfer
*
* Arguments:
*    pdesc : Pointer to the BAM descriptor table
*    phy_buffer : Physical address of the buffer that contains data 
*    count : Number of bytes to transfer
*
* Returns:
*    Number of descriptor created
******************************************************************************/
static int _setup_bam_xfer(sdcc_slot_type *pslot, uint32 paddr, uint32 len, uint32 flag)
{
   bam_pdata_t *bamdata = (bam_pdata_t *)(pslot->dma_data.hdma);
   uint32      dmlbase;
   uint32      bambase;
   uint32      desc_off;
   
   // Get the base adress to the hardware registers
   dmlbase  = bamdata->dml_reg_base;
   bambase  = bamdata->bam_reg_base;

   // Check if the transfer size is too big
   if (len >= BAM_MAX_TRANSFER_SIZE)
      return SDCC_ERR_DMA;
   
   // Setting up the DML
   HWIO_REG_OUT (DML_SW_RESET (dmlbase), 0);          // Start by resetting the core
   // Set the producer/consumer pipe to 0 depending on direction of transfer
   HWIO_REG_OUT (DML_PIPE_ID (dmlbase), (flag & SDCC_DIR_IN) ? (DML_PIPE_ID_HFNC (0, 1)) :  
                                                               (DML_PIPE_ID_HFNC (1, 0)));   
   HWIO_REG_OUT (DML_PRODUCER_PIPE_LOGICAL_SIZE (dmlbase), 4096); // Use the recommanded size
   HWIO_REG_OUT (DML_CONSUMER_PIPE_LOGICAL_SIZE (dmlbase), 4096);
   HWIO_REG_OUT (DML_PRODUCER_BAM_BLOCK_SIZE (dmlbase), pslot->dev.mem.mem_info.block_len);
   HWIO_REG_OUT (DML_PRODUCER_BAM_TRANS_SIZE (dmlbase), len);  
   
   if (flag & SDCC_DIR_IN){
      // Setup for producer mode + PRO_BLOCK_END_EN
      HWIO_REG_OUT (DML_CONFIG (dmlbase), DML_CONFIG_CRCI (1, 0) | DML_PRODUCER_TRANS_END_EN);
   }
   else {
      // Setup for consumer mode + PRO_BLOCK_END_EN
      HWIO_REG_OUT (DML_CONFIG (dmlbase), DML_CONFIG_CRCI (0, 1) | DML_PRODUCER_TRANS_END_EN);
   }
   
   // Setting up the BAM components
   HWIO_REG_OUT (BAM_CTRL (bambase), BAM_CTRL_RESET);  // Reset the BAM hardware
   HWIO_REG_OUT (BAM_CTRL (bambase), 0);               // Finishing resetting the hardware
   
   HWIO_REG_OUT (BAM_CTRL (bambase), BAM_CTRL_EN);                        // Enable BAM
   HWIO_REG_OUT (BAM_CNFG_BITS (bambase), BAM_CHICKEN_BIT);               // Chicken bits
   HWIO_REG_OUT (BAM_DESC_CNT_TRSHLD (bambase), BAM_DESC_TRSHLD_DEFAULT); // 0x400
   
   mbr();
   
   // The important registers are programmed after the debugging registers as the first write after
   // BAM P RESET occurs sometimes can be lost
   HWIO_REG_OUT (BAM_P_FIFO_SIZESn (bambase, 0), BAM_P_FIFO_SIZE_DEFAULT);
   HWIO_REG_OUT (BAM_P_EVNT_GEN_TRSHLDn (bambase, 0), BAM_P_EVNT_GEN_TRSHLD_D);
   HWIO_REG_OUT (BAM_P_DESC_FIFO_ADDRn (bambase, 0), bamdata->desc_buf_phy);
   
   // Build the transaction descriptor
   desc_off = _sdcc4_write_bam_descriptor (bamdata->desc_buffer, paddr, len);
   
   // Flush cache for BAM descriptor and the actual data buffer
   sdcc_bsp_cache_operation (SDCC_BSP_CACHE_CLEAN, (void *)bamdata->desc_buffer, desc_off);
   sdcc_bsp_cache_operation (SDCC_BSP_CACHE_CLEAN, (void *)paddr, len);
   mbr();
   
   // Start BAM
   HWIO_REG_OUT (BAM_P_CTRLn (bambase, 0), (flag == SDCC_DIR_IN) ? 
                                           BAM_P_PRODUCER_START : BAM_P_CONSUMER_START);
   
   // Start the DML transfer by a dummy write
   if (flag == SDCC_DIR_IN){
      HWIO_REG_OUT (DML_PRODUCER_START (dmlbase), 0);          // Dummy write to start the DML state machine
      HWIO_REG_OUT (BAM_P_EVNT_REGn (bambase, 0), desc_off);   // Update EVENT reg, start the BAM transfer
   } else {
      // Flush cache for the data to be transferred
      HWIO_REG_OUT (DML_CONSUMER_START (dmlbase), 0);         
      HWIO_REG_OUT (BAM_P_EVNT_REGn (bambase, 0), desc_off);   // Update EVENT reg, start the BAM transfer
   }

   mbr();
 
   //bamdata->last_op_addr = paddr; 
   //bamdata->last_op_len = len; 

   return SDCC_NO_ERROR;
}

/******************************************************************************
*   Complete an BAM based DMA transfer
*
* Arguments:
*    pdesc : Pointer to the BAM descriptor table
*    phy_buffer : Physical address of the buffer that contains data 
*    count : Number of bytes to transfer
*
* Returns:
*    Number of descriptor created
******************************************************************************/
static int _done_bam_xfer(sdcc_slot_type *pslot, uint32 flag)
{
   bam_pdata_t *bamdata = (bam_pdata_t *)(pslot->dma_data.hdma);
   uint32    bambase;
   uint32    sts;
   uint32    i = 0;

   // Get the base adress to the hardware registers
   bambase  = bamdata->bam_reg_base;
   // Wait until BAM finishes data transfer
   while (1) {
     sts = HWIO_REG_IN (BAM_P_IRQ_STTSn (bambase, 0));
     if (sts & BAM_P_IRQ_STTS_TRANSFER_END) break;
     if (i++ > BAM_COMPLETE_CHECK_COUNT) {
        // Timeout while waiting for BAM complete bit
        return SDCC_ERR_DMA;
     }
   }

   // Turning off BAM hardware
   HWIO_REG_OUT (BAM_CTRL (bambase), 0);   

   // INVAL cache -- Remove this after confirm operation
   //sdcc_bsp_cache_operation (SDCC_BSP_CACHE_INVAL, 
   //                   (void *)bamdata->last_op_addr, bamdata->last_op_len);
   return SDCC_NO_ERROR;
}

int sdcc_init_dma(sdcc_slot_type *pslot)
{
   bam_pdata_t *bamdata = NULL;

   /* Initialize the basic data structures */
   pslot->dma_data.setup_dma = _setup_bam_xfer;
   pslot->dma_data.done_dma = _done_bam_xfer;

   /* Allocate the use of BAM for the first slot only */
   switch (pslot->driveno) 
   {
      case SDCC_DRIVENO_0:
         pslot->dma_data.hdma = (void *)&sdcc_bam_priv_data;
         bamdata = &sdcc_bam_priv_data;
         bamdata->bam_reg_base = SDC1_BAM_REG_BASE;
         bamdata->dml_reg_base = SDC1_DML_REG_BASE;
         break;

      default:
         pslot->dma_data.hdma = NULL;
         return SDCC_ERR_INVALID_PARAM;
   }

   if (bamdata != NULL) 
   {
      /* Initialize the private data structure */
      bamdata->desc_buffer = (uint32 *)sdcc_bam_desc_buf;
      /* In boot loader, virtual addree = physical address */
      bamdata->desc_buf_phy = (uint32)bamdata->desc_buffer;
   }

   return SDCC_NO_ERROR;
}
