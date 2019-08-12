/** 
@file pcie_dma.h
@brief
This header files defines the public interface to the PCIe Device Driver 

*/
/*
===============================================================================

Edit History

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
12/25/13   Weijunz      Created
===============================================================================
Copyright (c) 2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#ifndef __PCIE_DMA_H__
#define __PCIE_DMA_H__
#ifndef PCIE_BOOT
#include "comdef.h"
//#include "pcie_core.h"

/*PCIe DMA request result*/
typedef enum
{
   /*DMA Done Successfully*/
   PCIE_DMA_DONE_SUCCESS = 0 ,   
   /*FataL Error,DMA write channel has received an error response from the AXI bus */  
   PCIE_DMA_WRITE_AXI_ERROR_DETECTED = 1 , 
   /*FataL Error,DMA Read channel has received an error response from the AXI bus */ 
   PCIE_DMA_READ_AXI_ERROR_DETECTED =2 ,
   /*DMA read channel has received a PCIe UR CPL Status*/
   PCIE_DMA_READ_UR  = 3 ,
   /*DMA read channel has received a PCIe CA CPL status*/
   PCIE_DMA_READ_CA = 4 ,
   /*DMA read channel has timed-out while waiting for the remote device to respond to the MRd request*/
   PCIE_DMA_READ_CPL_TIMEOUT = 5 ,
   /*DMA read channel has detected data poisoning in the CPL*/
   PCIE_DMA_READ_DATA_POISONING = 6 ,
   /*DMA read channel has detected data poisoning in the CPL*/
   PCIE_DMA_STOPPED  = 7 ,
   /*Linked List Element Fetch Error Detected*/
   PCIE_DMA_LINKED_LIST_FETCH_ERROR= 8 ,
   /*DMA READ UNKONWN Error*/
   PCIE_DMA_ERROR_UNKOWN = 9
   
}pcie_dma_interrupt_result_type;

/*PCIe DMA read or write request return type */
typedef enum 
{
   /*DMA request is accpeted and assigned channel to handle it.*/
   PCIE_DMA_ACCEPTED = 0 ,
   /*DMA Channel Full. Cannot accept more request*/  
   PCIE_DMA_CHANNEL_FULL  = 1 ,
   /*DMA STOP success. */  
   PCIE_DMA_STOP_SUCCESS  = 2 ,
   /*DMA STOP failure. */  
   PCIE_DMA_STOP_FAILURE  = 3
   
}pcie_dma_req_return_type;

/*PCIe DMA request parameters structure*/
typedef struct
{
   uint32 host_phy_addr_low ;
   uint32 host_phy_addr_hi ;
   uint32 device_phy_addr_low;
   uint32 device_phy_addr_hi;
   uint32 size ;
   void *user_data;
   boolean EOT;
}pcie_dma_req_type ;

typedef struct {
   uint32 control_word;
   uint32 transfer_size ; 
   uint32 sar_phy_addr_low ; 
   uint32 sar_phy_addr_hi ; 
   uint32 dar_phy_addr_low;
   uint32 dar_phy_addr_hi;
}pcie_dma_linked_list_element_type ;


typedef struct
{
   pcie_dma_interrupt_result_type result ;
   void* user_data ;
   uint32 remain_size;

}pcie_dma_cb_return_type ;

/*
PCIe DMA request callback function prototype 
result   : DMA request result code. 
user_data : user_data passed in dma request. 
remain_size : 0 if DMA done successfully, otherwise non zero if there is error. 
*/
typedef void (*pcie_dma_req_cb)(pcie_dma_cb_return_type cb_data)  ; 

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
void pcie_dma_register_cb(pcie_dma_req_cb read_cb,pcie_dma_req_cb write_cb);

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
void pcie_dma_init(void);

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
void pcie_dma_deinit(void);

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
void pcie_dma_reset(void);

/* ============================================================================
**  Function : pcie_dma_read
** ============================================================================
*/
/**
Read  data from the host memory through PCIe DMA.

@param[in]     read_req   start address of  an array of PCIe  DMA read request. 
@param[in]     size   PCIe  DMA read request 
 
 


@return
pcie_dma_req_return_type
*/

pcie_dma_req_return_type  pcie_dma_read(pcie_dma_req_type* read_req,uint8 size);

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

pcie_dma_req_return_type pcie_dma_stop_read(void);

/* ============================================================================
**  Function : pcie_dma_write
** ============================================================================
*/
/**
Write  data to the host memory through PCIe DMA.

@param[in]     write_req   start address of PCIe  DMA write request. 
@param[in]     size        number  of PCIe  DMA write request. 


@return
pcie_dma_req_return_type
*/
pcie_dma_req_return_type pcie_dma_write(pcie_dma_req_type* write_req,uint8 size);

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

pcie_dma_req_return_type pcie_dma_stop_write(void);

void pcie_dma_flush_pending_interrupts(void);

#endif /* PCIE_BOOT */
#endif  /*__PCIE_DMA_H__*/
