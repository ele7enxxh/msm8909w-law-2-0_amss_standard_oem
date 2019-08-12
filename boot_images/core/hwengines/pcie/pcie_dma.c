/** 
@file pcie_dma.c
@brief
Implements the interfaces to the PCIe Core HW 

*/
/*
===============================================================================

Edit History

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
4/01/2014   Weijunz     DMA linked list mode support
01/03/14   Weijunz      Created
===============================================================================
Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/
#ifndef PCIE_BOOT
#include "pcie_hwio.h"
#include "pcie_plat.h"
#include "pcie_core.h"
#include "pcieosal.h"
#include "pcie_dma.h"
#include "DALSysTypes.h"
//#include "assert.h"

#define NUM_OF_PCIE_DMA_READ_CHANNELS 1
#define NUM_OF_PCIE_DMA_WRITE_CHANNELS 1
#define PCIE_DMA_IRQ 85

#if 0
typedef struct _pcie_dma_debug_counters_t
{
  uint32 intr_when_write_fifo_count_0;
  uint32 intr_when_read_fifo_count_0;
} pcie_dma_debug_counters_t;

pcie_dma_debug_counters_t pcie_dma_debugC={0};
#endif

static uint8 pcie_dma_rd_channel_num = 0 ;
static uint8 pcie_dma_wr_channel_num = 0 ;

void*  rd_channel_user_data = NULL ;
void*  wr_channel_user_data = NULL ;
static pcie_dma_req_cb  registered_dma_rd_cb = NULL ;
static pcie_dma_req_cb  registered_dma_wr_cb = NULL ;
static pcie_osal_sync_type dma_cb_sync ;
static boolean pcie_dma_sync_init_needed = TRUE ;


extern pcie_devinfo_type *pcie_dev ;
/*Read and write DMA request FIFO data structure*/
#define PCIE_DMA_READ_FIFO_SIZE  128
static pcie_dma_req_type   dma_read_fifo[PCIE_DMA_READ_FIFO_SIZE] ;
static uint16 dma_read_fifo_fetch_index = 0 ;
static uint16 dma_read_fifo_put_index = 0 ;
static uint16 dma_read_fifo_cb_index = 0 ;
static uint16 read_fifo_count = 0 ;
#define PCIE_DMA_WRITE_FIFO_SIZE  128
static pcie_dma_req_type   dma_write_fifo[PCIE_DMA_WRITE_FIFO_SIZE] ;
static uint16 dma_write_fifo_fetch_index = 0 ;
static uint16 dma_write_fifo_put_index = 0 ;
static uint16 dma_write_fifo_cb_index = 0 ;
static uint16 write_fifo_count = 0 ;
static pcie_osal_sync_type fifo_sync ;

/*Read and Write Linked list buffer data structure*/
#define PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE 10
#define PCIE_DMA_READ_WATERMARK_INDEX (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE/2 -1)
#define PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE 10
#define PCIE_DMA_WRITE_WATERMARK_INDEX (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE/2 -1)
static pcie_dma_linked_list_element_type* read_ll_buf ;
static pcie_dma_linked_list_element_type* write_ll_buf ;
static void *read_buf_handle = NULL;
static DALSYSMemInfo read_buf_mem_info;
static void *write_buf_handle = NULL;
static DALSYSMemInfo write_buf_mem_info;
static uint8 read_buf_index = 0 ;
static uint8 write_buf_index = 0 ;
static uint8 read_buf_recycle_index = 0 ;
static uint8 write_buf_recycle_index = 0 ;
static boolean read_buf_pcs = TRUE ;
static boolean write_buf_pcs = TRUE ;
static boolean read_buf_first_tran = TRUE ;
static boolean write_buf_first_tran = TRUE ;

/*Control Word for data element and link element*/
#define DMA_CONTROL_NONINT_CB_1  0X1
#define DMA_CONTROL_NONINT_CB_0  0X0
#define DMA_CONTROL_INT_CB_1  0X9
#define DMA_CONTROL_INT_CB_0  0X8
#define DMA_CONTROL_LE_CB_1   0X7
#define DMA_CONTROL_LE_CB_0   0x6

#define EXIT_L1_PER_FUNCTION

static inline dma_write_reg(uint32 base,uint32  offset, uint32 val)
{
#ifdef EXIT_L1_PER_REGISTER
   pcie_core_req_exit_l1();
#endif
   pcie_write_reg(base, offset, val);

#ifdef EXIT_L1_PER_REGISTER
   pcie_core_req_enter_l1();
#endif

}
static inline dma_write_reg_field(uint32 base,uint32 offset,uint32 mask, uint32 shift, uint32 val)
{
#ifdef EXIT_L1_PER_REGISTER
   pcie_core_req_exit_l1();
#endif
   pcie_write_reg_field(base, offset, mask, shift, val);
#ifdef EXIT_L1_PER_REGISTER
   pcie_core_req_enter_l1();
#endif
}

static inline uint32 dma_read_reg(uint32 base,uint32  offset )
{
   uint32 ret;

#ifdef EXIT_L1_PER_REGISTER
   pcie_core_req_exit_l1();
#endif
   ret = pcie_read_reg(base, offset);

#ifdef EXIT_L1_PER_REGISTER
   pcie_core_req_enter_l1();
#endif
   return ret;

}
static inline uint32 dma_read_reg_field(uint32 base,uint32 offset,uint32 mask, uint32 shift)
{
   uint32 ret;

#ifdef EXIT_L1_PER_REGISTER
   pcie_core_req_exit_l1();
#endif
   ret = pcie_read_reg_field(base, offset, mask, shift);
#ifdef EXIT_L1_PER_REGISTER
   pcie_core_req_enter_l1();
#endif
   return ret;
}

static void pcie_dma_interrupt_handler(void) ;

static void pcie_dma_add_read_req_to_ll_buf(void);
static void pcie_dma_add_write_req_to_ll_buf(void);
static void pcie_dma_recycle_read_req(void);
static void pcie_dma_recycle_write_req(void);

__align(32) uint32 scratch;

void pcie_dma_flush_pending_interrupts(void)
{
   uint8 dma_int_rd_done_status,dma_int_wr_done_status ;
   uint8 dma_int_rd_err_status,dma_int_wr_err_status ;

   dma_int_wr_done_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_STATUS_REG,WR_DONE_INT_STATUS));
   dma_int_rd_done_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_STATUS_REG,RD_DONE_INT_STATUS));
   dma_int_wr_err_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_STATUS_REG,WR_ABORT_INT_STATUS));
   dma_int_rd_err_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_STATUS_REG,RD_ABORT_INT_STATUS));

   if (dma_int_wr_done_status || dma_int_rd_done_status || dma_int_wr_err_status ||
       dma_int_rd_err_status)
   {
      pcie_osal_debug(MSG_HIGH,"pcie_dma_flush_pending_interrupts: Pending interrupts => \
      wr_done = %d, rd_done = %d, wr_err = %d, rd_err = %d",dma_int_wr_done_status,
               dma_int_rd_done_status,dma_int_wr_err_status,dma_int_rd_err_status);
      pcie_dma_interrupt_handler();
   }
}

static void pcie_dma_interrupt_handler(void)
{
   uint8 dma_int_rd_done_status,dma_int_wr_done_status ;
   uint8 dma_int_rd_err_status,dma_int_wr_err_status ;
   uint32 rd_err_low,rd_err_high ;
   pcie_dma_cb_return_type cb_data ;
   pcie_dma_interrupt_result_type err_code = PCIE_DMA_ERROR_UNKOWN;
   int8 index ;
   int8 temp ,i ;
   uint32 remain_size ;

   pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: Enter");

#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_exit_l1();
#endif

   dma_int_wr_done_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_STATUS_REG,WR_DONE_INT_STATUS));
   dma_int_rd_done_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_STATUS_REG,RD_DONE_INT_STATUS));
   dma_int_wr_err_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_STATUS_REG,WR_ABORT_INT_STATUS));
   dma_int_rd_err_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_STATUS_REG,RD_ABORT_INT_STATUS));

   while (dma_int_wr_done_status || dma_int_rd_done_status ||dma_int_wr_err_status || dma_int_rd_err_status ) 
   {
      /*DMA write is done succesfully*/
      if (dma_int_wr_done_status ) 
      {
         pcie_osal_syncenter(&fifo_sync); //arjun

         dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x0);

         /*clear interrupt bit*/ //arjun
         dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_CLEAR_REG,WR_DONE_INT_CLEAR),0x1 ) ;

         index = (dma_read_reg(pcie_dev->dbi_base,DMA_LLP_LOW_REG_WRCH_0) - write_buf_mem_info.PhysicalAddr)/24 ;
         pcie_osal_debug(MSG_HIGH,"DMA write address %x, write_buf_mem_info.PhysicalAdd %x",dma_read_reg(pcie_dev->dbi_base,DMA_LLP_LOW_REG_WRCH_0),write_buf_mem_info.PhysicalAddr);
         if ((index < 0) ||(index > (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1) ))
         {
            /*Something is wrong here*/
            pcie_osal_debug(MSG_FATAL,"Index out of range");
            //ASSERT(0) ;
         }

         index = (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1 == index)?0:index; //arjun: According to Weijun index 9 should never happen, but it does. Check with HW folks.
         
         //index = (index == 0)?(PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1)  : index ;
         /*Call DMA call back for finished data elements*/
          //arjun: Index also should be synced
          //pcie_osal_syncenter(&fifo_sync);
         //for (temp = write_buf_recycle_index; temp < index ; temp++)
         temp = index - write_buf_recycle_index;
         temp = (temp < 0)?PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1 + temp:temp;
         pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: temp %d = index %d - write_buf_recycle_index %d",temp,index,write_buf_recycle_index);
         while(temp--)        
         {
            cb_data.user_data = dma_write_fifo[dma_write_fifo_cb_index].user_data ;
            //pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: Call cb for dma_write_fifo_cb_index %d, write ll buf index %d",dma_write_fifo_cb_index,index);
            pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: Call cb for dma_write_fifo_cb_index %d, equivalent write_ll_buf index %d",dma_write_fifo_cb_index,dma_write_fifo_cb_index%(PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1));
            dma_write_fifo_cb_index = (dma_write_fifo_cb_index +1 )% PCIE_DMA_WRITE_FIFO_SIZE ;
            if (write_fifo_count ==0) 
            {
               pcie_osal_debug(MSG_FATAL,"write_fifo_count is 0");
               //ASSERT(0) ;
            }
            else
            {
               write_fifo_count--;
               pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: write_fifo_count %d",write_fifo_count);

               //arjun: Last request has been serviced, no more interrupts expected, but can be generated due to race condition in reading index
               if (!write_fifo_count)
               {
                 /* Last transaction processed clear interrupt */
                 dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_CLEAR_REG,WR_DONE_INT_CLEAR),0x1 );
                 pcie_osal_debug(MSG_HIGH,"dma_write_fifo_fetch_index %d,dma_write_fifo_put_index %d,write_buf_index %d, write_buf_recycle_index %d",dma_write_fifo_fetch_index,dma_write_fifo_put_index,write_buf_index,write_buf_recycle_index);
               }
            }

           /*Call the DMA write Callback*/
            pcie_osal_syncenter(&dma_cb_sync);
            if (registered_dma_wr_cb != NULL) 
            {
               cb_data.result = PCIE_DMA_DONE_SUCCESS;
               cb_data.remain_size = 0 ;
               registered_dma_wr_cb(cb_data);
            }
            pcie_osal_syncleave(&dma_cb_sync);
         }
       //  pcie_osal_syncleave(&fifo_sync);
         //write_buf_recycle_index = (index == (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -1))?0 : index; //arjun
         write_buf_recycle_index = index;
         /*recycle  DMA Linked list data element*/
       //  pcie_osal_syncenter(&fifo_sync);
         temp = (write_buf_index - index)<0?index - write_buf_index:PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -1 - (write_buf_index - index);
         while(temp--)  
         {
            pcie_dma_recycle_write_req();
         }
         pcie_osal_syncleave(&fifo_sync);
         
         /*clear interrupt bit*/
         //arjun: Clearing interrupt here could end up loosing latest index if dma stops before intr is cleared.
         //dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_CLEAR_REG,WR_DONE_INT_CLEAR),0x1 ) ;

         /*Check if Channel is in stopped status */
         dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x0);
         if (dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS)) == 3) 
         {
            pcie_osal_memorybarrier();
          dma_write_reg(pcie_dev->dbi_base,DMA_WRITE_DOORBELL_REG,0) ;
         }

      }

      /*DMA write cause error*/
      if (dma_int_wr_err_status )
      {
         /*select write channel with channel_id*/
         dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x0);

         /*clear interrupt bit*/ //arjun
         //dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_CLEAR_REG,WR_ABORT_INT_CLEAR),0x1) ;
         
         remain_size = dma_read_reg(pcie_dev->dbi_base,DMA_TRANSFER_SIZE_REG_WRCH_0);
         index = (dma_read_reg(pcie_dev->dbi_base,DMA_LLP_LOW_REG_WRCH_0) - write_buf_mem_info.PhysicalAddr)/24 ;
         pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: write err index %d",index);
         /*Get Error Code*/
         if (dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(TYPE0_DMA_WRITE_ERR_STATUS_REG,APP_READ_ERR_DETECT))) 
         {
            err_code = PCIE_DMA_WRITE_AXI_ERROR_DETECTED;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_WRITE_AXI_ERROR_DETECTED"); 
         }
         if (dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(TYPE0_DMA_WRITE_ERR_STATUS_REG,LINKLIST_ELEMENT_FETCH_ERR_DETECT))) 
         {
            err_code = PCIE_DMA_LINKED_LIST_FETCH_ERROR;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_LINKED_LIST_FETCH_ERROR"); 
         }
         if (dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS)) == 3)
         {
           err_code = PCIE_DMA_STOPPED ;
           pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_STOPPED");
         }

         pcie_osal_debug(MSG_FATAL,"pcie_dma_interrupt_handler: wr err status received");   

         if ( (index < 0) ||(index >= (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1)) )
         {
            /*Something is wrong here*/
            pcie_osal_debug(MSG_FATAL,"Index out of range");
            //ASSERT(0) ;
         }
         /*Call DMA call back for finished data elements*/
         for (temp = write_buf_recycle_index; temp < index ; temp++) 
         {
            /*recycle this DMA Linked list data element*/
            pcie_osal_syncenter(&fifo_sync);
            cb_data.user_data = dma_write_fifo[dma_write_fifo_cb_index].user_data ;
            dma_write_fifo_cb_index = (dma_write_fifo_cb_index +1 )% PCIE_DMA_WRITE_FIFO_SIZE ;
		    if (write_fifo_count ==0) 
            {
               pcie_osal_debug(MSG_FATAL,"write_fifo_count is 0");
               //ASSERT(0) ;
            }
            else
            {
               write_fifo_count--;

               //arjun: Last request has been serviced, no more interrupts expected, but can be generated due to race condition in reading index
               if (!write_fifo_count)
                  dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_CLEAR_REG,WR_ABORT_INT_CLEAR),0x1) ;
            }
            pcie_osal_syncleave(&fifo_sync);
              /*Call the DMA write Callback*/
             pcie_osal_syncenter(&dma_cb_sync);
             if (registered_dma_wr_cb != NULL) 
             {
                if (temp!= index-1) 
                {
                   cb_data.result = PCIE_DMA_DONE_SUCCESS;
                   cb_data.remain_size = 0 ;
                }
                else
                {
                   cb_data.result = err_code ;
                   cb_data.remain_size = remain_size ;
                }
                registered_dma_wr_cb(cb_data);
             }
             pcie_osal_syncleave(&dma_cb_sync);
         }
         write_buf_recycle_index = (index + 1) % (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -2);
         /*recycle  DMA Linked list data element*/
         pcie_osal_syncenter(&fifo_sync);
         for (temp= write_buf_index ; temp< index;temp++) 
         {
            pcie_dma_recycle_write_req();
         }
         pcie_osal_syncleave(&fifo_sync);
         /*clear interrupt bit*/ //arjun
         //dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_CLEAR_REG,WR_ABORT_INT_CLEAR),0x1) ;
         /*Restart the DMA write */
         pcie_osal_memorybarrier();
         dma_write_reg(pcie_dev->dbi_base,DMA_WRITE_DOORBELL_REG,0) ;
      }
      /*DMA read is done succesfully*/
      if (dma_int_rd_done_status ) 
      {
         pcie_osal_syncenter(&fifo_sync); //arjun

         dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x80000000);

         /*clear interrupt bit*/ //arjun
         dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_CLEAR_REG,RD_DONE_INT_CLEAR),0x1) ; 
         
         index = (dma_read_reg(pcie_dev->dbi_base,DMA_LLP_LOW_REG_WRCH_0) - read_buf_mem_info.PhysicalAddr)/24 ;
         pcie_osal_debug(MSG_HIGH,"DMA read address %x, read_buf_mem_info.PhysicalAdd %x",dma_read_reg(pcie_dev->dbi_base,DMA_LLP_LOW_REG_WRCH_0),read_buf_mem_info.PhysicalAddr);
         if ((index < 0) ||(index > (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1) ))
         {
            /*Something is wrong here*/
            pcie_osal_debug(MSG_FATAL,"Index out of range");
            //ASSERT(0) ;
         }

         //index = (index == 0)?(PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1)  : index ;
         /*Call DMA call back for finished data elements*/
         //arjun: index also needs to be synced...  
         //pcie_osal_syncenter(&fifo_sync);
         //for (temp = read_buf_recycle_index; temp < index ; temp++)
         //arjun: change cb invocation logic
         temp = index - read_buf_recycle_index;
         temp = (temp < 0)?PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1 + temp:temp;
         pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: temp %d = index %d - read_buf_recycle_index %d",temp,index,read_buf_recycle_index);
         pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: Outside loop read_fifo_count %d",read_fifo_count);
         while(temp--)
         {
          
            cb_data.user_data = dma_read_fifo[dma_read_fifo_cb_index].user_data ;
            //pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: Call cb for dma_read_fifo_cb_index %d, read ll buf index: %d",dma_read_fifo_cb_index,index);
            pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: Call cb for dma_read_fifo_cb_index %d, eqiuvalent read_ll_buf index %d",dma_read_fifo_cb_index,dma_read_fifo_cb_index%(PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1));
            dma_read_fifo_cb_index = (dma_read_fifo_cb_index +1 )% PCIE_DMA_READ_FIFO_SIZE ;
            if (read_fifo_count == 0) 
            {
               pcie_osal_debug(MSG_FATAL,"read_fifo_count is 0");
               //ASSERT(0) ;
            }
            else
            {
               read_fifo_count--;
               pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: read_fifo_count %d",read_fifo_count);

               if(!read_fifo_count)
               {
                  /* Last transaction processed clear interrupt */
                  dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_CLEAR_REG,RD_DONE_INT_CLEAR),0x1) ; 
                  pcie_osal_debug(MSG_HIGH,"dma_read_fifo_fetch_index %d,dma_read_fifo_put_index %d,read_buf_index %d, read_buf_recycle_index %d",dma_read_fifo_fetch_index,dma_read_fifo_put_index,read_buf_index,read_buf_recycle_index);
               }
            }
           

              /*Call the DMA read Callback*/
             pcie_osal_syncenter(&dma_cb_sync);
             if (registered_dma_rd_cb != NULL) 
             {
                cb_data.result = PCIE_DMA_DONE_SUCCESS;
                cb_data.remain_size = 0 ;
                registered_dma_rd_cb(cb_data);
             }
             pcie_osal_syncleave(&dma_cb_sync);
         }
       //   pcie_osal_syncleave(&fifo_sync);
         //read_buf_recycle_index = (index == (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -1))?0 : index; //arjun
         read_buf_recycle_index = index;
         /*Recylce DMA read buffer data element*/
        // pcie_osal_syncenter(&fifo_sync);
         temp = (read_buf_index - index)<0?index - read_buf_index:PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -1 - (read_buf_index - index);
         while(temp--)  
         {
            pcie_dma_recycle_read_req();
         }
          pcie_osal_syncleave(&fifo_sync);
         /*clear interrupt bit*/ //arjun
         //dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_CLEAR_REG,RD_DONE_INT_CLEAR),0x1) ;
         /*Check if Channel is in stopped status */
         dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x80000000);
         if (dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS)) == 3) 
         {
            pcie_osal_memorybarrier();
            dma_write_reg(pcie_dev->dbi_base,DMA_READ_DOORBELL_REG,0) ;
         }
      }
      /*DMA read cause error*/
      if (dma_int_rd_err_status ) 
      {
         /*DMA Read channel Error information */
         dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x80000000);

         /*clear interrupt bit*/ //arjun
         //dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_CLEAR_REG,RD_ABORT_INT_CLEAR),0x1) ;

         remain_size = dma_read_reg(pcie_dev->dbi_base,DMA_TRANSFER_SIZE_REG_WRCH_0);
         index = (dma_read_reg(pcie_dev->dbi_base,DMA_LLP_LOW_REG_WRCH_0) - read_buf_mem_info.PhysicalAddr)/24 ;
         pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: read err index %d",index);
         rd_err_low = dma_read_reg(pcie_dev->dbi_base,DMA_READ_ERR_STATUS_LOW_REG);
         rd_err_high = dma_read_reg(pcie_dev->dbi_base,DMA_READ_ERR_STATUS_HIGH_REG);
         if (rd_err_low & 0x1 ) 
         {
            err_code = PCIE_DMA_READ_AXI_ERROR_DETECTED ;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_READ_AXI_ERROR_DETECTED");
         }
         if (rd_err_low & 0x10000 ) 
         {
            err_code = PCIE_DMA_LINKED_LIST_FETCH_ERROR ;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_LINKED_LIST_FETCH_ERROR");            
         }
         if (rd_err_high & 0X1 ) 
         {
            err_code = PCIE_DMA_READ_UR ;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_READ_UR");            
         }
         if (rd_err_high & 0x200 ) 
         {
            err_code = PCIE_DMA_READ_CA ;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_READ_CA");            
         }
         if (rd_err_high & 0x10000) 
         {
            err_code = PCIE_DMA_READ_CPL_TIMEOUT ;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_READ_CPL_TIMEOUT");            
         }
         if (rd_err_high & 0x1000000 ) 
         {
            err_code = PCIE_DMA_READ_DATA_POISONING ;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_READ_DATA_POISONING");            
         }

         if ((err_code == PCIE_DMA_ERROR_UNKOWN) &&
             (dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS)) == 3)
             )
         {
            err_code = PCIE_DMA_STOPPED ;
            pcie_osal_debug(MSG_ERR,"pcie_dma_interrupt_handler: PCIE_DMA_ERROR_UNKOWN");            
         }

         pcie_osal_debug(MSG_FATAL,"pcie_dma_interrupt_handler: rd err sts");

         if ((index < 0) ||(index > (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1) ))
         {
            /*Something is wrong here*/
            pcie_osal_debug(MSG_FATAL,"Index out of range");
            //ASSERT(0) ;
         }
         /*Call DMA call back for finished data elements*/
         for (temp = read_buf_recycle_index; temp < index ; temp++) 
         {
            /*recycle this DMA Linked list data element*/
            pcie_osal_syncenter(&fifo_sync);
            cb_data.user_data = dma_read_fifo[dma_read_fifo_cb_index].user_data ;
            dma_read_fifo_cb_index = (dma_read_fifo_cb_index +1 )% PCIE_DMA_READ_FIFO_SIZE ;
            if (read_fifo_count == 0) 
            {
               pcie_osal_debug(MSG_FATAL,"read_fifo_count is 0");
               //ASSERT(0) ;
            }
            else
            {
               read_fifo_count--;

               //arjun: Last request has been serviced, no more interrupts expected, but can be generated due to race condition in reading index
               if(!read_fifo_count)
                  dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_CLEAR_REG,RD_ABORT_INT_CLEAR),0x1) ;
            }
            pcie_osal_syncleave(&fifo_sync);
            /*Debug Purpose only*/

             /*Call the DMA read Callback*/
             pcie_osal_syncenter(&dma_cb_sync);
             if (registered_dma_rd_cb != NULL ) 
             {
                if (temp != index-1) 
                {
                   cb_data.result = PCIE_DMA_DONE_SUCCESS;
                   cb_data.remain_size = 0 ;
                }
                else
                {
                   cb_data.result = err_code;
                   cb_data.remain_size = remain_size ;
                }
                registered_dma_rd_cb(cb_data);
             }
             pcie_osal_syncleave(&dma_cb_sync);
         }
         read_buf_recycle_index = (index + 1) % (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -2);
         /*Recylce DMA read buffer data element*/
         pcie_osal_syncenter(&fifo_sync);
         for (temp= read_buf_index ; temp< index;i++) 
         {
            pcie_dma_recycle_read_req();
         }
         pcie_osal_syncleave(&fifo_sync);
         /*clear interrupt bit*/ //arjun
         //dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_CLEAR_REG,RD_ABORT_INT_CLEAR),0x1) ;
         /*Restart DMA read*/
         pcie_osal_memorybarrier();
         dma_write_reg(pcie_dev->dbi_base,DMA_READ_DOORBELL_REG,0) ;
      }
      dma_int_wr_done_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_STATUS_REG,WR_DONE_INT_STATUS));
      dma_int_rd_done_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_STATUS_REG,RD_DONE_INT_STATUS));
      dma_int_wr_err_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_STATUS_REG,WR_ABORT_INT_STATUS));
      dma_int_rd_err_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_STATUS_REG,RD_ABORT_INT_STATUS));
   }

#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_enter_l1();
#endif
   pcie_osal_debug(MSG_HIGH,"pcie_dma_interrupt_handler: read DBI");
   //Enabling this causes XPU violation in BAM code.
   //pcie_read_va((char*)&scratch, pcie_dev->dbi_base, 4, 0);
}

static void pcie_dma_add_read_req_to_ll_buf(void)
{
   uint8 i ;
   int8 index ;
   uint8 temp ;

   pcie_osal_debug(MSG_HIGH,"pcie_dma_add_read_req_to_ll_buf: Enter");
   
   if (read_buf_first_tran ) 
   {
      pcie_osal_debug(MSG_HIGH,"pcie_dma_add_read_req_to_ll_buf: first tran");
      temp = (read_fifo_count > (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1))?(PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1): read_fifo_count ;
      for (i=0;i < temp;i++)
      {
         (read_ll_buf+i)->transfer_size = dma_read_fifo[dma_read_fifo_fetch_index].size ;
         (read_ll_buf+i)->sar_phy_addr_low = dma_read_fifo[dma_read_fifo_fetch_index].host_phy_addr_low;
         (read_ll_buf+i)->sar_phy_addr_hi = dma_read_fifo[dma_read_fifo_fetch_index].host_phy_addr_hi ;
         (read_ll_buf+i)->dar_phy_addr_low = dma_read_fifo[dma_read_fifo_fetch_index].device_phy_addr_low ;
         (read_ll_buf+i)->dar_phy_addr_hi = dma_read_fifo[dma_read_fifo_fetch_index].device_phy_addr_hi ;
         
         if (i == PCIE_DMA_READ_WATERMARK_INDEX ) 
         {
            (read_ll_buf+i)->control_word = DMA_CONTROL_INT_CB_1 ;
            read_buf_index++ ;
         }
         else if (i == (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 2)) 
         {
            (read_ll_buf+i)->control_word = DMA_CONTROL_INT_CB_1 ;
            (read_ll_buf+PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1)->control_word = DMA_CONTROL_LE_CB_0 ;
            read_buf_index = 0 ;
            read_buf_pcs = !read_buf_pcs ;
         }
         else
         {
            if ( i == (temp-1)) 
            {
               (read_ll_buf+i)->control_word = DMA_CONTROL_INT_CB_1 ;

            }
            else
            {
               (read_ll_buf+i)->control_word = DMA_CONTROL_NONINT_CB_1 ;
            }
            read_buf_index++ ;
         }
         dma_read_fifo_fetch_index = (dma_read_fifo_fetch_index +1 ) % PCIE_DMA_READ_FIFO_SIZE ;
      }
      read_buf_first_tran = FALSE ;
      pcie_osal_memorybarrier();

      dma_write_reg(pcie_dev->dbi_base,DMA_READ_DOORBELL_REG,0) ;
   }
   else if (!read_buf_first_tran) 
   {
      pcie_osal_debug(MSG_HIGH,"pcie_dma_add_read_req_to_ll_buf: !first tran");
    
      /*Check if Channel is in stopped status */
      dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x80000000);
      if (dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS)) == 3) 
      {
         index = (dma_read_reg(pcie_dev->dbi_base,DMA_LLP_LOW_REG_WRCH_0) - read_buf_mem_info.PhysicalAddr)/24 ;
         pcie_osal_debug(MSG_HIGH,"pcie_dma_add_read_req_to_ll_buf: index = %d",index);
         if ((index < 0) ||(index > (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1) ))
         {
            /*Something is wrong here*/
            pcie_osal_debug(MSG_FATAL,"Index out of range");
            //ASSERT(0) ;
         }
         //arjun: read_buf_recycle_index = (index == (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -1))?0 : index;

         index = (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1 == index)?0:index; //arjun
         
         /*Add DMA request from FIFO to linked list buffer*/
         for (i= read_buf_index ; i< (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -1);i++) 
         {
            pcie_dma_recycle_read_req();
         }

         /* Start DMA if interrupt is not pending, else let interrupt handler start it */
         if (!dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_INT_STATUS_REG,RD_DONE_INT_STATUS)))  
         {
            pcie_osal_memorybarrier();
            dma_write_reg(pcie_dev->dbi_base,DMA_READ_DOORBELL_REG,0);
         }
      }
   }

}

static void pcie_dma_add_write_req_to_ll_buf(void)
{
   uint8 i;
   int8 index ;
   uint8 temp ;

   if (write_buf_first_tran  ) 
   {
      pcie_osal_debug(MSG_HIGH,"pcie_dma_add_write_req_to_ll_buf: first trans");

      temp = (write_fifo_count > (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1))?(PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1) : write_fifo_count ;
      for (i=0;i< temp;i++)

      {
         (write_ll_buf+i)->transfer_size = dma_write_fifo[dma_write_fifo_fetch_index].size ;
         (write_ll_buf+i)->sar_phy_addr_low = dma_write_fifo[dma_write_fifo_fetch_index].device_phy_addr_low;
         (write_ll_buf+i)->sar_phy_addr_hi = dma_write_fifo[dma_write_fifo_fetch_index].device_phy_addr_hi ;
         (write_ll_buf+i)->dar_phy_addr_low = dma_write_fifo[dma_write_fifo_fetch_index].host_phy_addr_low ;
         (write_ll_buf+i)->dar_phy_addr_hi = dma_write_fifo[dma_write_fifo_fetch_index].host_phy_addr_hi ;
         if (i == PCIE_DMA_WRITE_WATERMARK_INDEX ) 
         {
            (write_ll_buf+i)->control_word = DMA_CONTROL_INT_CB_1 ;
            write_buf_index++ ;
         }
         else if (i == (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 2)) 
         {
            (write_ll_buf+i)->control_word = DMA_CONTROL_INT_CB_1 ;
            (write_ll_buf+PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1)->control_word = DMA_CONTROL_LE_CB_0 ;
            write_buf_index = 0 ;
            write_buf_pcs = !write_buf_pcs ;
         }
         else
         {
            if ( i == (temp -1)) 
            {
               (write_ll_buf+i)->control_word = DMA_CONTROL_INT_CB_1 ;
            }
            else
            {
               (write_ll_buf+i)->control_word = DMA_CONTROL_NONINT_CB_1 ;
            }
            write_buf_index++ ;
         }
         dma_write_fifo_fetch_index = (dma_write_fifo_fetch_index +1 ) % PCIE_DMA_WRITE_FIFO_SIZE ;
      }
      write_buf_first_tran = FALSE ;
      pcie_osal_memorybarrier();
      dma_write_reg(pcie_dev->dbi_base,DMA_WRITE_DOORBELL_REG,0) ;
   }
   else  if (!write_buf_first_tran) 
   {
      pcie_osal_debug(MSG_HIGH,"pcie_dma_add_write_req_to_ll_buf: !first_trans"); 

      /*Check if Channel is in stopped status */
      dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x0);
      if (dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS)) == 3) 
      {
         pcie_osal_syncenter(&fifo_sync); //arjun

         pcie_osal_debug(MSG_HIGH,"pcie_dma_add_write_req_to_ll_buf: after fifo_sync");
         
         index = (dma_read_reg(pcie_dev->dbi_base,DMA_LLP_LOW_REG_WRCH_0) - write_buf_mem_info.PhysicalAddr)/24 ;
         pcie_osal_debug(MSG_HIGH,"pcie_dma_add_write_req_to_ll_buf: index = %d",index);
         if ( (index < 0) ||(index > (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1)) )
         {
            /*Something is wrong here*/
            pcie_osal_debug(MSG_FATAL,"Index out of range");
            //ASSERT(0) ;
         }
         //arjun: write_buf_recycle_index = (index == (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -1))?0 : index;

         index = (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1 == index)?0:index; //arjun

         for (i=index; i < (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -1);i++) 
         {
            pcie_dma_recycle_write_req();
         }

         pcie_osal_syncleave(&fifo_sync); //arjun
         
         /* Start DMA if interrupt is not pending, else let interrupt handler start it */
         if (!dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_INT_STATUS_REG,WR_DONE_INT_STATUS)))  
         {  
            pcie_osal_memorybarrier();
            dma_write_reg(pcie_dev->dbi_base,DMA_WRITE_DOORBELL_REG,0);
         }
      }
   }

}

static void pcie_dma_recycle_read_req(void)
{
   uint8 i ;

   i = (dma_read_fifo_put_index >= dma_read_fifo_fetch_index )?(dma_read_fifo_put_index - dma_read_fifo_fetch_index ) : (PCIE_DMA_READ_FIFO_SIZE - dma_read_fifo_fetch_index + dma_read_fifo_put_index);
 
   if (i > 0) 
   {
      (read_ll_buf+read_buf_index)->transfer_size = dma_read_fifo[dma_read_fifo_fetch_index].size ;
      (read_ll_buf+read_buf_index)->sar_phy_addr_low = dma_read_fifo[dma_read_fifo_fetch_index].host_phy_addr_low;
      (read_ll_buf+read_buf_index)->sar_phy_addr_hi = dma_read_fifo[dma_read_fifo_fetch_index].host_phy_addr_hi ;
      (read_ll_buf+read_buf_index)->dar_phy_addr_low = dma_read_fifo[dma_read_fifo_fetch_index].device_phy_addr_low ;
      (read_ll_buf+read_buf_index)->dar_phy_addr_hi = dma_read_fifo[dma_read_fifo_fetch_index].device_phy_addr_hi ;
      if (read_buf_index == PCIE_DMA_READ_WATERMARK_INDEX ) 
      {
         (read_ll_buf+read_buf_index)->control_word = (read_buf_pcs)?DMA_CONTROL_INT_CB_1:DMA_CONTROL_INT_CB_0 ;
         read_buf_index++ ;
      }
      else if (read_buf_index == (PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 2)) 
      {
         (read_ll_buf+read_buf_index)->control_word = (read_buf_pcs)?DMA_CONTROL_INT_CB_1:DMA_CONTROL_INT_CB_0 ; 
         (read_ll_buf+PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE - 1)->control_word = (read_buf_pcs)?DMA_CONTROL_LE_CB_0:DMA_CONTROL_LE_CB_1 ;
         read_buf_index = 0 ;
         read_buf_pcs = !read_buf_pcs ;
      }
      else
      {
        if (1 == i) 
        {
            (read_ll_buf+read_buf_index)->control_word = (read_buf_pcs )?DMA_CONTROL_INT_CB_1:DMA_CONTROL_INT_CB_0 ;
         }
         else
         {
            (read_ll_buf+read_buf_index)->control_word = (read_buf_pcs )?DMA_CONTROL_NONINT_CB_1:DMA_CONTROL_NONINT_CB_0 ; 
         }
         read_buf_index++ ;
      }
      dma_read_fifo_fetch_index = (dma_read_fifo_fetch_index +1 ) % PCIE_DMA_READ_FIFO_SIZE ;
   }
}

static void pcie_dma_recycle_write_req(void)
{
   uint8 i ;

   i = (dma_write_fifo_put_index >= dma_write_fifo_fetch_index )?(dma_write_fifo_put_index - dma_write_fifo_fetch_index ) : (PCIE_DMA_WRITE_FIFO_SIZE - dma_write_fifo_fetch_index + dma_write_fifo_put_index);
   
   if (i > 0) 
   {
      (write_ll_buf+write_buf_index)->transfer_size = dma_write_fifo[dma_write_fifo_fetch_index].size ;
      (write_ll_buf+write_buf_index)->sar_phy_addr_low = dma_write_fifo[dma_write_fifo_fetch_index].device_phy_addr_low;
      (write_ll_buf+write_buf_index)->sar_phy_addr_hi = dma_write_fifo[dma_write_fifo_fetch_index].device_phy_addr_hi ;
      (write_ll_buf+write_buf_index)->dar_phy_addr_low = dma_write_fifo[dma_write_fifo_fetch_index].host_phy_addr_low ;
      (write_ll_buf+write_buf_index)->dar_phy_addr_hi = dma_write_fifo[dma_write_fifo_fetch_index].host_phy_addr_hi ;
      if (write_buf_index == PCIE_DMA_WRITE_WATERMARK_INDEX ) 
      {
         (write_ll_buf+write_buf_index)->control_word = (write_buf_pcs)?DMA_CONTROL_INT_CB_1:DMA_CONTROL_INT_CB_0 ;
         write_buf_index++ ;
      }
      else if (write_buf_index == (PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 2)) 
      {
         (write_ll_buf+write_buf_index)->control_word = (write_buf_pcs)?DMA_CONTROL_INT_CB_1:DMA_CONTROL_INT_CB_0 ; 
         (write_ll_buf+PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE - 1)->control_word = (write_buf_pcs)?DMA_CONTROL_LE_CB_0:DMA_CONTROL_LE_CB_1 ;
         write_buf_index = 0 ;
         write_buf_pcs = !write_buf_pcs ;
      }
      else
      {
         if( 1 == i) 
         {
            (write_ll_buf+write_buf_index)->control_word = (write_buf_pcs)?DMA_CONTROL_INT_CB_1:DMA_CONTROL_INT_CB_0 ; 
         }
         else
         {
            (write_ll_buf+write_buf_index)->control_word = (write_buf_pcs)?DMA_CONTROL_NONINT_CB_1:DMA_CONTROL_NONINT_CB_0 ;
         }
         write_buf_index++ ;
      }
      dma_write_fifo_fetch_index = (dma_write_fifo_fetch_index +1 ) % PCIE_DMA_WRITE_FIFO_SIZE ;
   }
}

/* ============================================================================
**  Function : pcie_register_dma_cb
** ============================================================================
*/
/**
Register callback functions for dma requests .


@param[in]     read_cb  call back funcion pointer for read request . 
@param[in]     write_cb  call back funcion pointer for  write request . 


@return
void

*/
void pcie_dma_register_cb(pcie_dma_req_cb read_cb,pcie_dma_req_cb write_cb)
{
   pcie_osal_syncenter(&dma_cb_sync);
   registered_dma_rd_cb = read_cb ;
   registered_dma_wr_cb = write_cb ;
   pcie_osal_syncleave(&dma_cb_sync);
}

/* ============================================================================
**  Function : pcie_dma_init
** ============================================================================
*/
/**
Initialize PCIe DMA HW and SW module.

@param[in] 
void 



@return
void

*/
void pcie_dma_init(void)
{
   uint8 i ;
   uint32 buf_size ;
  
   pcie_osal_debug(MSG_HIGH,"pcie_dma_init: Enter");

   if (pcie_dma_sync_init_needed) 
   {
      pcie_osal_syncinit(&dma_cb_sync);
      pcie_osal_syncinit(&fifo_sync);
      pcie_dma_sync_init_needed = FALSE ;
   }

   /*Read DMA HW configuration*/
   pcie_dma_rd_channel_num = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CTRL_REG,NUM_DMA_RD_CHAN));
   pcie_dma_wr_channel_num = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CTRL_REG,NUM_DMA_WR_CHAN));
   if ((pcie_dma_rd_channel_num == 0) ||
       (pcie_dma_rd_channel_num > NUM_OF_PCIE_DMA_READ_CHANNELS) ||
       (pcie_dma_wr_channel_num == 0) ||
       (pcie_dma_wr_channel_num > NUM_OF_PCIE_DMA_WRITE_CHANNELS) 
       ) 
   {
      pcie_osal_debug(MSG_FATAL,"Invalid config");
      //ASSERT(0) ;
   }

   /*Enable PCIe DMA write and Read engine*/
   dma_write_reg(pcie_dev->dbi_base,DMA_WRITE_ENGINE_EN,0X1);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG,WRITE_CHANNEL0_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG,WRITE_CHANNEL1_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG,WRITE_CHANNEL2_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG,WRITE_CHANNEL3_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG,WRITE_CHANNEL4_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG,WRITE_CHANNEL5_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG,WRITE_CHANNEL6_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG,WRITE_CHANNEL7_WEIGHT),0X0);

   dma_write_reg(pcie_dev->dbi_base,DMA_READ_ENGINE_EN,0X1);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG,READ_CHANNEL0_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG,READ_CHANNEL1_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG,READ_CHANNEL2_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG,READ_CHANNEL3_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG,READ_CHANNEL4_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG,READ_CHANNEL5_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG,READ_CHANNEL6_WEIGHT),0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG,READ_CHANNEL7_WEIGHT),0X0);

      /*Allocate Read and Wirte Linked list buffer*/
   buf_size = PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE*sizeof(pcie_dma_linked_list_element_type) ;
   if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
    DALSYS_MEM_PROPS_PHYS_CONT, DALSYS_MEM_ADDR_NOT_SPECIFIED, 
    DALSYS_MEM_ADDR_NOT_SPECIFIED, buf_size, &read_buf_handle, NULL) )
   {
      read_buf_handle = NULL;
      pcie_osal_debug(MSG_FATAL,"Could not allocate Read linked list buffer");
      //ASSERT(0) ;
      
   }
   DALSYS_MemInfo(read_buf_handle, &read_buf_mem_info);

   read_ll_buf = (pcie_dma_linked_list_element_type*)read_buf_mem_info.VirtualAddr  ;
   

   buf_size = PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE*sizeof(pcie_dma_linked_list_element_type) ;
   if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
      DALSYS_MEM_PROPS_PHYS_CONT, DALSYS_MEM_ADDR_NOT_SPECIFIED, 
      DALSYS_MEM_ADDR_NOT_SPECIFIED, buf_size, &write_buf_handle, NULL))
    {
       write_buf_handle = NULL;
       pcie_osal_debug(MSG_FATAL,"Could not allocate write linked list buffer");
       //ASSERT(0) ;
       
    }

   DALSYS_MemInfo(write_buf_handle, &write_buf_mem_info);
   write_ll_buf = (pcie_dma_linked_list_element_type*)write_buf_mem_info.VirtualAddr ;
 


   for (i=0; i < PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE;i++) 
   {
      (read_ll_buf+i)->control_word = 0 ;
      (read_ll_buf+i)->transfer_size = 0 ;
      (read_ll_buf+i)->sar_phy_addr_low = 0 ;
      (read_ll_buf+i)->sar_phy_addr_hi = 0 ;
      (read_ll_buf+i)->dar_phy_addr_low = 0 ;
      (read_ll_buf+i)->dar_phy_addr_hi = 0 ;
   }
   (read_ll_buf+PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -1)->sar_phy_addr_low = read_buf_mem_info.PhysicalAddr ;
   (read_ll_buf+PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -1)->sar_phy_addr_hi = 0 ;

   for (i=0; i < PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE;i++) 
   {
      (write_ll_buf+i)->control_word = 0 ;
      (write_ll_buf+i)->transfer_size = 0 ;
      (write_ll_buf+i)->sar_phy_addr_low = 0 ;
      (write_ll_buf+i)->sar_phy_addr_hi = 0 ;
      (write_ll_buf+i)->dar_phy_addr_low = 0 ;
      (write_ll_buf+i)->dar_phy_addr_hi = 0 ;
   }
   (write_ll_buf+PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -1)->sar_phy_addr_low = write_buf_mem_info.PhysicalAddr ;
   (write_ll_buf+PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -1)->sar_phy_addr_hi = 0 ;

   /*Enable all PCIe DMA read and write interrupt*/
   dma_write_reg(pcie_dev->dbi_base,DMA_WRITE_INT_MASK_REG,0X0);
   dma_write_reg(pcie_dev->dbi_base,DMA_READ_INT_MASK_REG,0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_LINKED_LIST_ERR_EN_REG,RD_CHANNEL_LLLAIE),0X1);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_LINKED_LIST_ERR_EN_REG,WR_CHANNEL_LLLAIE),0X1);
   /*Program DMA Read and Write Control*/
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0X0);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_CH_CONTROL1_REG_WRCH_0,0X4000300);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_LLP_LOW_REG_WRCH_0,write_buf_mem_info.PhysicalAddr);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_LLP_HIGH_REG_WRCH_0,0X0);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0X80000000);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_CH_CONTROL1_REG_WRCH_0,0X4000300);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_LLP_LOW_REG_WRCH_0,read_buf_mem_info.PhysicalAddr);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_LLP_HIGH_REG_WRCH_0,0X0);
   pcie_osal_isrinstall(&pcie_dev->pcie_dma_irq_h,PCIE_DMA_IRQ,pcie_dma_interrupt_handler,NULL,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);
   pcie_osal_debug(MSG_HIGH,"pcie_dma_init: Exit");
   
}

/* ============================================================================
**  Function : pcie_dma_deinit
** ============================================================================
*/
/**
DeInitialize PCIe DMA HW and SW module.

@param[in] 
void.


@return
void

*/
void pcie_dma_deinit(void)
{
}

/* ============================================================================
**  Function : pcie_dma_reset
** ============================================================================
*/
/**
Reset PCIe DMA HW and SW.Running DMA request will be stopped

@param[in] 
void.


@return
void

*/
void pcie_dma_reset(void)
{
   uint8 i ;


#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_enter_l1();
#endif
   /*Reset DMA read engine*/
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x80000000); 
   while( dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS)) == 1) ;
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_ENGINE_EN,DMA_READ_ENGINE),0X0) ;
   while( dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_ENGINE_EN,DMA_READ_ENGINE)) != 0) ;
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_ENGINE_EN,DMA_READ_ENGINE),0X1) ;

   /*Reset DMA Write engine*/
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x0); 
   while( dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS)) == 1) ;
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_ENGINE_EN,DMA_WRITE_ENGINE),0X0) ;
   while( dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_ENGINE_EN,DMA_WRITE_ENGINE)) != 0) ;
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_ENGINE_EN,DMA_WRITE_ENGINE),0X1) ;
   /*Reset Linked buffer index*/
   pcie_osal_syncenter(&fifo_sync);
   dma_read_fifo_cb_index = 0 ;
   dma_read_fifo_fetch_index = 0 ;
   dma_read_fifo_put_index = 0 ;
   read_fifo_count = 0 ;
   read_buf_index = 0 ;
   read_buf_recycle_index = 0 ;
   pcie_osal_syncleave(&fifo_sync);

   pcie_osal_syncenter(&fifo_sync);
   dma_write_fifo_cb_index = 0 ;
   dma_write_fifo_fetch_index = 0 ;
   dma_write_fifo_put_index = 0 ;
   write_fifo_count = 0 ;
   write_buf_index = 0 ;
   write_buf_recycle_index = 0 ;
   pcie_osal_syncleave(&fifo_sync);

   /*reset Linked list buffer content*/
     read_ll_buf = (pcie_dma_linked_list_element_type*)read_buf_mem_info.VirtualAddr  ;
   for (i=0; i < PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE;i++) 
   {
      (read_ll_buf+i)->control_word = 0 ;
      (read_ll_buf+i)->transfer_size = 0 ;
      (read_ll_buf+i)->sar_phy_addr_low = 0 ;
      (read_ll_buf+i)->sar_phy_addr_hi = 0 ;
      (read_ll_buf+i)->dar_phy_addr_low = 0 ;
      (read_ll_buf+i)->dar_phy_addr_hi = 0 ;
   }
   (read_ll_buf+PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -1)->sar_phy_addr_low = read_buf_mem_info.PhysicalAddr ;
   (read_ll_buf+PCIE_DMA_READ_LINK_LIST_BUFFER_SIZE -1)->sar_phy_addr_hi = 0 ;

   write_ll_buf = (pcie_dma_linked_list_element_type*)write_buf_mem_info.VirtualAddr ;
   for (i=0; i < PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE;i++) 
   {
      (write_ll_buf+i)->control_word = 0 ;
      (write_ll_buf+i)->transfer_size = 0 ;
      (write_ll_buf+i)->sar_phy_addr_low = 0 ;
      (write_ll_buf+i)->sar_phy_addr_hi = 0 ;
      (write_ll_buf+i)->dar_phy_addr_low = 0 ;
      (write_ll_buf+i)->dar_phy_addr_hi = 0 ;
   }
   (write_ll_buf+PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -1)->sar_phy_addr_low = write_buf_mem_info.PhysicalAddr;
   (write_ll_buf+PCIE_DMA_WRITE_LINK_LIST_BUFFER_SIZE -1)->sar_phy_addr_hi = 0 ;

   read_buf_pcs = TRUE ;
   write_buf_pcs = TRUE ;
   /*Enable all PCIe DMA read and write interrupt*/
   dma_write_reg(pcie_dev->dbi_base,DMA_WRITE_INT_MASK_REG,0X0);
   dma_write_reg(pcie_dev->dbi_base,DMA_READ_INT_MASK_REG,0X0);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_READ_LINKED_LIST_ERR_EN_REG,RD_CHANNEL_LLLAIE),0X1);
   dma_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_WRITE_LINKED_LIST_ERR_EN_REG,WR_CHANNEL_LLLAIE),0X1);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0X0);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_CH_CONTROL1_REG_WRCH_0,0X4000300);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_LLP_LOW_REG_WRCH_0,(uint32)write_ll_buf);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_LLP_HIGH_REG_WRCH_0,0X0);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0X80000000);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_CH_CONTROL1_REG_WRCH_0,0X4000300);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_LLP_LOW_REG_WRCH_0,(uint32)read_ll_buf);
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_LLP_HIGH_REG_WRCH_0,0X0);

   read_buf_first_tran = TRUE ;
   write_buf_first_tran = TRUE ;
   

#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_enter_l1();
#endif
}

/* ============================================================================
**  Function : pcie_dma_read
** ============================================================================
*/
/**
Read  data from the host memory through PCIe DMA.

@param[in]     read_req   PCIe  DMA read request. 
@param[in]     size   PCIe  DMA read request 
 
 


@return
pcie_dma_req_return_type
*/
pcie_dma_req_return_type  pcie_dma_read(pcie_dma_req_type* read_req,uint8 size)
{
   uint8 index ;
   
   pcie_osal_debug(MSG_HIGH,"pcie_dma_read: Enter");

   pcie_osal_syncenter(&fifo_sync);
   if ( (read_fifo_count == PCIE_DMA_READ_FIFO_SIZE) ||
        ((read_fifo_count + size ) > PCIE_DMA_READ_FIFO_SIZE)
      ) 
   {
      return PCIE_DMA_CHANNEL_FULL ;
   }


#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_exit_l1();
#endif
   for (index = 0 ; index < size ; index++) 
   {
      dma_read_fifo[dma_read_fifo_put_index].device_phy_addr_hi = ((pcie_dma_req_type*)(read_req + index))->device_phy_addr_hi ;
      dma_read_fifo[dma_read_fifo_put_index].device_phy_addr_low = ((pcie_dma_req_type*)(read_req + index))->device_phy_addr_low ;
      dma_read_fifo[dma_read_fifo_put_index].host_phy_addr_hi = ((pcie_dma_req_type*)(read_req + index))->host_phy_addr_hi ;
      dma_read_fifo[dma_read_fifo_put_index].host_phy_addr_low = ((pcie_dma_req_type*)(read_req + index))->host_phy_addr_low ;
      dma_read_fifo[dma_read_fifo_put_index].size = ((pcie_dma_req_type*)(read_req + index))->size ;
      dma_read_fifo[dma_read_fifo_put_index].user_data = ((pcie_dma_req_type*)(read_req + index))->user_data;
      dma_read_fifo[dma_read_fifo_put_index].EOT = ((pcie_dma_req_type*)(read_req + index))->EOT;
      dma_read_fifo_put_index = (dma_read_fifo_put_index + 1) % PCIE_DMA_READ_FIFO_SIZE ;
   }
   read_fifo_count +=size ;
   pcie_dma_add_read_req_to_ll_buf();
   pcie_osal_syncleave(&fifo_sync);

   dma_read_reg(pcie_dev->dbi_base, 0);
#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_enter_l1();
#endif
   pcie_read_va((char*)&scratch, pcie_dev->dbi_base, 4, 0);

   return PCIE_DMA_ACCEPTED ;

}

/* ============================================================================
**  Function : pcie_dma_write
** ============================================================================
*/
/**
Write  data to the host memory through PCIe DMA.

@param[in]     write_req   PCIe  DMA write request.
@param[in]     size        number  of PCIe  DMA write request. 


@return
pcie_dma_req_return_type
*/
pcie_dma_req_return_type pcie_dma_write(pcie_dma_req_type* write_req,uint8 size)
{
   uint8 index ;
   pcie_osal_syncenter(&fifo_sync);
   if ( (write_fifo_count == PCIE_DMA_WRITE_FIFO_SIZE) ||
        ((write_fifo_count + size ) > PCIE_DMA_WRITE_FIFO_SIZE)
      ) 
   {
      return PCIE_DMA_CHANNEL_FULL ;
   }

   pcie_osal_debug(MSG_HIGH,"pcie_dma_write: Enter");

#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_exit_l1();
#endif
   for (index = 0 ; index < size ; index++) 
   {
      dma_write_fifo[dma_write_fifo_put_index].device_phy_addr_hi = ((pcie_dma_req_type*)(write_req + index))->device_phy_addr_hi ;
      dma_write_fifo[dma_write_fifo_put_index].device_phy_addr_low = ((pcie_dma_req_type*)(write_req + index))->device_phy_addr_low ;
      dma_write_fifo[dma_write_fifo_put_index].host_phy_addr_hi = ((pcie_dma_req_type*)(write_req + index))->host_phy_addr_hi ;
      dma_write_fifo[dma_write_fifo_put_index].host_phy_addr_low = ((pcie_dma_req_type*)(write_req + index))->host_phy_addr_low ;
      dma_write_fifo[dma_write_fifo_put_index].size = ((pcie_dma_req_type*)(write_req + index))->size ;
      dma_write_fifo[dma_write_fifo_put_index].user_data = ((pcie_dma_req_type*)(write_req + index))->user_data;
      dma_write_fifo[dma_write_fifo_put_index].EOT = ((pcie_dma_req_type*)(write_req + index))->EOT;
      dma_write_fifo_put_index = (dma_write_fifo_put_index + 1) % PCIE_DMA_WRITE_FIFO_SIZE ;
   }
   write_fifo_count +=size ;
   pcie_dma_add_write_req_to_ll_buf();
   pcie_osal_syncleave(&fifo_sync);
 
   dma_read_reg(pcie_dev->dbi_base, 0);
#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_enter_l1();
#endif
  //arjun 
  pcie_read_va((char*)&scratch, pcie_dev->dbi_base, 4, 0); 
 
   
   return PCIE_DMA_ACCEPTED ;
}

/* ============================================================================
**  Function : pcie_dma_stop_read
** ============================================================================
*/
/**
Stop running DMA read request.

@param[in]     
void

@return
pcie_dma_req_return_type
*/

pcie_dma_req_return_type pcie_dma_stop_read(void)
{
   uint8 channel_status ;
   pcie_dma_req_return_type ret=PCIE_DMA_STOP_SUCCESS;

#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_exit_l1();
#endif
   pcie_osal_syncenter(&fifo_sync);
   /*select read channel with channel_id*/
    dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x80000000);

   channel_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS));
   if (channel_status != 1) 
   {
       pcie_osal_syncleave(&fifo_sync);
       /*Read Channel isn't running. cannot stop it*/
       ret= PCIE_DMA_STOP_FAILURE ;
       goto end;
   }
   /*Stop READ Channel*/
   dma_write_reg(pcie_dev->dbi_base,DMA_READ_DOORBELL_REG,0X80000000);
   pcie_osal_syncleave(&fifo_sync);

#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_enter_l1();
#endif
end:

   return ret;
}

/* ============================================================================
**  Function : pcie_dma_stop_write
** ============================================================================
*/
/**
Stop running DMA write request.

@param[in]     
void

@return
pcie_dma_req_return_type
*/

pcie_dma_req_return_type pcie_dma_stop_write(void)
{
   uint8 channel_status ;
   pcie_dma_req_return_type ret=PCIE_DMA_STOP_SUCCESS;

   pcie_osal_syncenter(&fifo_sync);

#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_exit_l1();
#endif
   /*select write channel with channel_id*/
   dma_write_reg(pcie_dev->dbi_base,TYPE0_DMA_VIEWPORT_SEL_REG,0x0);

   channel_status = dma_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DMA_CH_CONTROL1_REG_WRCH_0,CS));
   if (channel_status != 1) 
   {
       pcie_osal_syncleave(&fifo_sync);
       /*Write Channel isn't running. cannot stop it*/
       ret= PCIE_DMA_STOP_FAILURE ;
       goto end;
   }
   /*Stop write Channel*/
   dma_write_reg(pcie_dev->dbi_base,DMA_WRITE_DOORBELL_REG,0X80000000);
   pcie_osal_syncleave(&fifo_sync);

#ifdef EXIT_L1_PER_FUNCTION
   pcie_core_req_enter_l1();
#endif
end:
   return ret ;
}
#endif /* PCIE_BOOT */
