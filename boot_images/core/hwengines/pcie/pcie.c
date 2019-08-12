/** 
@file pcie.c
@brief
Implements the interfaces to the PCIe Core HW 

*/
/*
===============================================================================

Edit History

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/23/14   Dilip   Removed bam_setirqmode from pcie_dma_init
09/21/14   Dilip   Added comments 
06/24/14   Dilip   LTR_EN bit check and enable L1.2 in M0
06/20/14   Dilip   Deassert wake after LTSSM
06/18/14   Dilip   Call pcie_phy_config_update and req_l1 enter in pcie_init for boot over pcie
06/11/14   Dilip   Vote for CNOC SVS frequency and changes for core dump
06/09/14   am      Added BAM DMA for async transfers
05/30/14   Dilip   Return from pcie_init if the pcie fuse is disabled
05/23/14   Dilip   Added timer value as a parameter in pcie_control_l1ss_inact_interrupt
05/21/14   Dilip   Deassert wake after perst deassertion
05/21/14   Dilip   Enabled BAMDMA
05/20/14   Dilip   Removed pcie_set_l1_dot2_thresh
05/13/14   Dilip   Move pmic client registration and svs voting before enabling power domain in pcie_init and before it returns from init.
05/06/14   Dilip   Removed call to pcie_phy_config_update from pcie_init
05/02/14   nk      pcie_flush added
04/30/14   Dilip   Call pcie_phy_config_update in pcie_init and Vote for PCIE_OFF in M2
04/25/14   Dilip   pcie_osal_init done before other initializations
04/23/14   Dilip   Fix KW warnings
04/08/14   Dilip   Changes in LTR and removed pcie_req_exit_l1 and  pcie_req_enter_l1
03/19/14   Dilip   Created pcie_disable_endpoint and moved calls from perst and link down ISR to pcie_disable_endpoint and pcie_enable_endpoint
03/17/14   MK      Added support for CLKREQ# routing
03/17/14   Dilip   Replaced assert with pcie_osal_debug
03/17/14   Dilip   Changes for Restoring TZ system call
08/05/13   Weijunz Created
===============================================================================
Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/
#include "pcie_hwio.h"
#include "pcie.h"
#include "pcie_core.h"
#include "pcie_plat.h"
#include "pcieosal.h"
#include "pcie_phy.h"
#include "DDIPlatformInfo.h"
#include "pcie_dma.h"
#include "pcie_fuse.h"
#ifndef  PCIE_BOOT
#include "kernel.h"
/** For VMIDT restoration */
#include "tz_syscalls.h"
#include "syscalls.h"

#include "err.h"
#endif
#ifdef PCIE_BOOT
#include "bam.h"
#endif

#define PCIE_DEFAULT_LTR 16000

/** Link Status indicator structure */
typedef struct _pcie_link_info_type
{
    pcie_core_linkstate_type link_state;    /** State of the PHY link */
    uint32                    enum_status;  /** State of the enumeration */
}pcie_linkinfo_type;

static boolean pcie_bam_dump_alloc_success = TRUE;
/** handle to the PCIe device from plat */
pcie_devinfo_type *pcie_dev     = NULL;

/** handle to the target configuration data from plat */
static pcie_tgtcfg_type  *pcie_tgt_cfg = NULL;

/** Link status indicator */
static pcie_linkinfo_type pcie_link_info;

#ifndef  PCIE_BOOT
/** Flag indicating whether the PCIe error-fatal callback has been registered. */
static boolean pcie_err_fatal_callback_registered = FALSE;
static pcie_gpio_perst_cb_param_type  perst_in_expect ;
/* For PCIe core dump */
static pcie_coredump_type pcie_coredump;
static boolean pcie_sync_init_needed = TRUE ;
static DalDeviceHandle    *h_pcie_clkreq_tlmm;
static DALGpioSignalType  pcie_clkreq_pin_config;
/*Debug counters */
static uint32 pcie_link_down_counter = 0 ;
static uint32 pcie_link_up_counter = 0 ;
/*Power management callback function and parameter*/
static pcie_pm_cb_type  mhi_registered_pm_cb = NULL ;
static void *cb_passed_param ;
#endif

#ifndef PCIE_TRADITIONAL_SENDMSI
static pcie_msiconfig_type pcie_msi_config = {0};
#endif /* #ifdef PCIE_TRADITIONAL_SENDMSI */

/* Outbound index 0 : used only for the region mapped for MSIs */
#define PCIE_OATU_INDEX_MSI 0  
/*OUTBOUND INDEX 1: used for address mapping for MHI control */    
#define PCIE_OATU_INDEX_MHICTRL 1  
/*OUTBOUND INDEX 2: used for address mapping for MHI data*/
#define PCIE_OATU_INDEX_MHIDATA 2 
 
/*Offset for MSI address */
#define PCIE_AXI_MEMOFFSET_FOR_MSI 0X800000
/*Offset for MHI control  */
#define PCIE_AXI_MEMOFFSET_FOR_MHICTRL  0X802000
/*Upper limit for PCIe memory space */
#define PCIE_AXI_MEMSPACE_LIMIT 0X3FFFD000
/*Current PCIe AXI memory space used for MHI control region. reserved at least 4KB initially */
static uint32 mhictrl_upper_offset = PCIE_AXI_MEMOFFSET_FOR_MHICTRL + 0XFFF ;
/*Current PCIe AXI memory space used for MHI data region. reserved at least 4KB initially */
static uint32 mhidata_lower_offset = PCIE_AXI_MEMSPACE_LIMIT - 0X1000;
 
/* define IRQ for PCIe interrupts*/
#define PCIE_LINK_DOWN_INT_IRQ  83
#define PCIE_LINK_UP_INT_IRQ  82
#define PCIE_DEVICE_STATE_CHANGE_IRQ 78
#define PCIE_L1_INACT_TIMEOUT_IRQ  79
#define PCIE_BRIDGE_FLUSH_IRQ 84
static __align(32) uint32 scratch_read_buf;


#ifdef PCIE_BOOT_ENABLED
boolean pcie_init_needed = TRUE ;
#else
boolean pcie_init_needed = FALSE ;
#endif
#ifndef PCIE_BOOT
static pcie_osal_sync_type mhi_pm_cb_sync ;
static pcie_osal_sync_type pcie_pmic_npa_sync ;
#endif

static DalDeviceHandle    *h_pcie_wake_tlmm;
static DALGpioSignalType  pcie_wake_pin_config;

#ifdef PCIE_BOOT
static bam_config_type pcie_ipadma_bam_cfg;
static bam_handle pcie_ipadma_bam_handle;

static bam_handle pcie_rxp_bam_handle;
static bam_handle pcie_rxa_bam_handle;
static bam_handle pcie_txp_bam_handle;
static bam_handle pcie_txa_bam_handle;

static bam_pipe_config_type ipadma_bam_rxp_pipe_cfg;
static bam_pipe_config_type ipadma_bam_txp_pipe_cfg;
static bam_pipe_config_type ipadma_bam_rxa_pipe_cfg;
static bam_pipe_config_type ipadma_bam_txa_pipe_cfg;


static void *desc_buf_handle = NULL;

static uint32 desc_fifo_size ;  
#define RXP_DESC_FIFO_SIZE   (120*8)
#define TXA_DESC_FIFO_SIZE   (120*8)
#define TXP_DESC_FIFO_SIZE   (120*8)
#define RXA_DESC_FIFO_SIZE   (120*8)
static DALSYSMemInfo desc_mem_info;

#define PCIE_BAM_PHY_ADDR    0x7904000    
#define PCIE_BAM_RXP_PIPE     1  
#define PCIE_BAM_TXP_PIPE     11  
#define PCIE_BAM_RXA_PIPE     0  
#define PCIE_BAM_TXA_PIPE     12
#define PCIE_BAM_NUM_PIPES    4
#define PCIE_BAM_NUM_OF_BAMS  1
#define PCIE_BAM_DUMP_MASK    (0x00000000 | (1<<PCIE_BAM_RXA_PIPE) | (1<<PCIE_BAM_RXP_PIPE) | (1<<PCIE_BAM_TXP_PIPE) | (1<<PCIE_BAM_TXA_PIPE) )


dma_req_cb  dma_read_req_cb_func = NULL ;
dma_req_cb  dma_write_req_cb_func = NULL ;


static void pcie_rxa_pipe_cb(bam_result_type bam_result);
static void pcie_txp_pipe_cb(bam_result_type bam_result);


bam_callback_type rxa_pipe_callback ;
bam_callback_type txp_pipe_callback ;

static void pcie_ipa_core_hw_init(void) ;
static bam_handle pcie_bam_pipe_init(uint32 pipe_num,bam_pipe_config_type *bam_pipe_cfg,bam_callback_type *pipe_cb);
static int pcie_dma_init(void);


/*Added for Unit test*/
uint32 sbl_ipadma_src[8] = {
   0X1A2B3C4D,
   0XA1B2C3D4,
   0X5A6B7C8D,
   0XA5B6C7D8,
   0X1A2B3C4D,
   0XA1B2C3D4,
   0X5A6B7C8D,
   0XA5B6C7D8
};


uint32 sbl_ipadma_dest[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0} ;

boolean test_stop_poll = FALSE ;
#endif
pcie_iatu_debug_type pcie_atu_debug_info ;
 



#define SCRATCH_POOL_SIZE 128
static pcie_osal_meminfo scratch_pool; 
static pcie_osal_sync_type scratch_sync;

/* Generic callback for asynchronous read/write requests */
dma_req_cb async_rw_req_cb=NULL;



#ifndef  PCIE_BOOT
static void pcie_install_isrs(void) ;
static void pcie_config_core(void) ;
static void pcie_uninstall_isrs(void) ;
static void* pcie_gpio_perst_isr(GPIOINTISRCtx param);
static void pcie_gpio_perst_register_isr(void) ;
static void pcie_gpio_perst_deregister_isr(void) ;
static void pcie_err_fatal(void);
/*Local function prototype*/
static void pcie_link_down_interrupt_handler(void *pcie_dev_r);
static void pcie_link_up_interrupt_handler(void *pcie_dev_r);
static void pcie_device_state_change_interrupt_handler(void *pcie_dev_r) ;
static void pcie_l1_inact_timeout_interrupt_handler(void *pcie_dev_r) ;
#endif

static void pcie_gpio_assert_wake(void) ;

static void pcie_gpio_deassert_wake(void) ;



#ifndef  PCIE_BOOT
static void pcie_gpio_wake_init(void) ;
static void pcie_gpio_clkreq_init(void) ;
void KxCache_DCacheCleanBytes (  addr_t aVirtualAddress, unsigned int dwBytes);
#endif


#ifdef PCIE_BOOT
void sbl_read_test_func(void)
{
   uint32 wait = 0xFFF;
   dma_read_write_req_type read_req ;

   //pcie_read_va((char *)sbl_ipadma_dest,(uint32)sbl_ipadma_src,32,sbl_ipadma_src);

   read_req.src_addr = (uint32)sbl_ipadma_src ;
   read_req.dest_addr = (uint32)sbl_ipadma_dest ;
   read_req.size = 0X20 ;
   read_req.user_data = sbl_ipadma_src ;
   pcie_read_dma(read_req);

   while (wait-- > 0);
   pcie_dma_poll();
   while (!test_stop_poll) 
   {
      pcie_dma_poll();
   }

}
/*end of unit test*/


static void pcie_ipa_core_hw_init(void)
{
   uint32 wait = 0XFFF ;
  
  /*----------------------------------------------------------------------------
    Clock on the IPA hardware. This needs to be done only if we are not in
    simulation mode.
  ----------------------------------------------------------------------------*/
 
  /*----------------------------------------------------------------------------
    Set the SW_RESET field in the IPA_COMP_SW_RESET register to 0x1
    After 1 ms, set this value to 0. This will reset IPA HW.
    Note: Do not use the REG_WRITE_FIELD API in this case since this register is
    write only and cannot be read. The REG_WRITE_FIELD_API needs to read the reg
  ----------------------------------------------------------------------------*/
  HWIO_IPA_COMP_SW_RESET_OUT(1) ;

  /*----------------------------------------------------------------------------
    sleep for 1 ms before removing the reset from high
  ----------------------------------------------------------------------------*/
  while (wait > 0) 
  {
     wait-- ;
  }

  HWIO_IPA_COMP_SW_RESET_OUT(0) ;

  /*----------------------------------------------------------------------------
    Enable IPA hardware by writing to register IPA_COMP_CFG
    Note that the programming guide specifies this to be done after BAM init.
    But here this has been reversed. Should any problem arise, this function has
    to be split into two "init" and "enable" with pipe_mgmt_start being called
    in between
  ----------------------------------------------------------------------------*/
  HWIO_IPA_COMP_CFG_OUT(0X1);

} /* ipa_core_power_up_hw */



static bam_handle pcie_bam_pipe_init(uint32 pipe_num, 
  bam_pipe_config_type *bam_pipe_cfg,
  bam_callback_type *pipe_cb)
{
  bam_status_type bam_status;
  bam_handle bam_pipe_handle;

  /* Initialize the BAM PIPE */  
  bam_pipe_handle = bam_pipe_init(pcie_ipadma_bam_handle, pipe_num, bam_pipe_cfg,pipe_cb);
  
  if(bam_pipe_handle == NULL)
  {
    pcie_osal_debug(MSG_FATAL, "bam_pipe_init failed");
    return NULL;
  }

  /* use polling mode for this pipe */
  bam_status = bam_pipe_setirqmode(bam_pipe_handle, 0, bam_pipe_cfg->options);
  if(bam_status != BAM_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "bam_pipe_setirqmode failed");
    return NULL;
  }

  /* Enable the pipe  */
  bam_status = bam_pipe_enable(bam_pipe_handle);
  if(bam_status != BAM_SUCCESS)
  {
    pcie_osal_debug(MSG_FATAL, "bam_pipe_enable failed");
    return NULL;
  }

  return bam_pipe_handle;
}

static void pcie_rxa_pipe_cb(bam_result_type bam_result)
{
    bam_status_type bam_status ;
    bam_result_type rxp_poll_result ;
    dma_read_write_req_type dma_req ;

    /*Added for unit test*/
    test_stop_poll = TRUE ;
    /*end of unit test code*/

    if ( (bam_result.event == BAM_EVENT_ERROR) || 
        (bam_result.event == BAM_EVENT_OUT_OF_DESC)
        )
    {
      // error fatal here
      pcie_osal_debug(MSG_FATAL, "bam_result error");
      return ;
    }

    bam_status = bam_pipe_poll(pcie_rxp_bam_handle,&rxp_poll_result);

   if ((bam_status != BAM_SUCCESS) || (rxp_poll_result.event == BAM_EVENT_ERROR) || (
      rxp_poll_result.event == BAM_EVENT_OUT_OF_DESC))
    {
      // error fatal here
      pcie_osal_debug(MSG_FATAL, "bam_result error");
      return ;
    }
    if( ((rxp_poll_result.event == BAM_EVENT_DESC_DONE) || 
        (rxp_poll_result.event == BAM_EVENT_EOT)) && 
        ( (bam_result.event == BAM_EVENT_DESC_DONE) || 
        (bam_result.event == BAM_EVENT_EOT))
        )
    {
       if( (bam_result.data.xfer.xfer_cb_data == rxp_poll_result.data.xfer.xfer_cb_data) &&
           (bam_result.data.xfer.iovec.buf_size == rxp_poll_result.data.xfer.iovec.buf_size)
         )
       {
          if (dma_read_req_cb_func != NULL) 
          {
             dma_req.src_addr = rxp_poll_result.data.xfer.iovec.buf_pa ;
             dma_req.dest_addr = bam_result.data.xfer.iovec.buf_pa ;
             dma_req.size = bam_result.data.xfer.iovec.buf_size ;
             dma_req.user_data = (void *)bam_result.data.xfer.xfer_cb_data;
             dma_req.result = TRUE ;
             dma_read_req_cb_func(dma_req);
          }

       }
       else
       {
          if (dma_read_req_cb_func != NULL) 
          {
             dma_req.src_addr = rxp_poll_result.data.xfer.iovec.buf_pa ;
             dma_req.dest_addr = bam_result.data.xfer.iovec.buf_pa ;
             dma_req.size = bam_result.data.xfer.iovec.buf_size ;
             dma_req.user_data = (void *)bam_result.data.xfer.xfer_cb_data;
             dma_req.result = FALSE ;
             dma_read_req_cb_func(dma_req);
          }
          //Error fatal here
          pcie_osal_debug(MSG_FATAL, "bam error");
          return ;
       }
      
    }    
    
   

}

static void pcie_txp_pipe_cb(bam_result_type bam_result)
{
    bam_status_type bam_status ;
    bam_result_type txa_poll_result ;
    dma_read_write_req_type dma_req ;

    if ( (bam_result.event == BAM_EVENT_ERROR) || 
        (bam_result.event == BAM_EVENT_OUT_OF_DESC)
        )
    {
      // error fatal here
      pcie_osal_debug(MSG_FATAL, "bam_result error");
      return ;
    }

    bam_status = bam_pipe_poll(pcie_txa_bam_handle,&txa_poll_result);

   if ((bam_status != BAM_SUCCESS) || (txa_poll_result.event == BAM_EVENT_ERROR) || (
      txa_poll_result.event == BAM_EVENT_OUT_OF_DESC))
    {
      // error fatal here
      pcie_osal_debug(MSG_FATAL, "bam_result error");
      return ;
    }
    if( ((txa_poll_result.event == BAM_EVENT_DESC_DONE) || 
        (txa_poll_result.event == BAM_EVENT_EOT)) && 
        ( (bam_result.event == BAM_EVENT_DESC_DONE) || 
        (bam_result.event == BAM_EVENT_EOT))
        )
    {
       if( (bam_result.data.xfer.xfer_cb_data == txa_poll_result.data.xfer.xfer_cb_data) &&
           (bam_result.data.xfer.iovec.buf_size == txa_poll_result.data.xfer.iovec.buf_size)
         )
       {
          if (dma_write_req_cb_func != NULL) 
          {
             dma_req.src_addr = txa_poll_result.data.xfer.iovec.buf_pa ;
             dma_req.dest_addr = bam_result.data.xfer.iovec.buf_pa ;
             dma_req.size = bam_result.data.xfer.iovec.buf_size ;
             dma_req.user_data = (void *)bam_result.data.xfer.xfer_cb_data;
             dma_req.result = TRUE ;
             dma_write_req_cb_func(dma_req);
          }

       }
       else
       {
          if (dma_write_req_cb_func != NULL) 
          {
             dma_req.src_addr = txa_poll_result.data.xfer.iovec.buf_pa ;
             dma_req.dest_addr = bam_result.data.xfer.iovec.buf_pa ;
             dma_req.size = bam_result.data.xfer.iovec.buf_size ;
             dma_req.user_data = (void *)bam_result.data.xfer.xfer_cb_data;
             dma_req.result = FALSE ;
             dma_write_req_cb_func(dma_req);
          }
          //Error fatal here
          pcie_osal_debug(MSG_FATAL, "bam error");
          return ;
       }
      
    }    
    
   

}

/* ============================================================================
**  Function : pcie_dma_init
** ============================================================================
*/
/**
Initialize for data tranfer through PCIe link and DMA .




@return
None

*/
static int pcie_dma_init(void)
{
   int result = 0 ;
   uint32 reg_val ;

   
   

  // pcie_plat_init(&pcie_dev, &pcie_tgt_cfg);

   pcie_ipa_core_hw_init();

   /* Allocate a physically contiguous buffer for Descriptor FIFO - check 
     for 8 byte alignment */
  desc_fifo_size = RXP_DESC_FIFO_SIZE + RXA_DESC_FIFO_SIZE + TXA_DESC_FIFO_SIZE + TXP_DESC_FIFO_SIZE ;

  if ( DAL_SUCCESS != DALSYS_MemRegionAlloc(DALSYS_MEM_PROPS_UNCACHED |
    DALSYS_MEM_PROPS_PHYS_CONT, DALSYS_MEM_ADDR_NOT_SPECIFIED, 
    DALSYS_MEM_ADDR_NOT_SPECIFIED, desc_fifo_size, &desc_buf_handle, NULL))
  {
    desc_buf_handle = NULL;
    pcie_osal_debug(MSG_FATAL, "DALSYS_MemRegionAlloc failed");
    return -1;
  }

  DALSYS_MemInfo(desc_buf_handle, &desc_mem_info);

  /* Mandatory fields to be supplied by the client */
  pcie_ipadma_bam_cfg.bam_pa =  PCIE_BAM_PHY_ADDR; 
  pcie_ipadma_bam_cfg.bam_va = PCIE_BAM_PHY_ADDR;   
  pcie_ipadma_bam_cfg.bam_irq = 0;  /* Set this to 0 for polling mode */
  pcie_ipadma_bam_cfg.bam_irq_mask = 0; 
  pcie_ipadma_bam_cfg.sum_thresh = 0x2000; /* Desc Summing threshold - Not used */
  pcie_ipadma_bam_cfg.bam_mti_irq_pa = 0;  

  /* Initialize BAM module */
  pcie_ipadma_bam_handle =  bam_init(&pcie_ipadma_bam_cfg, NULL);

  if(pcie_ipadma_bam_handle == NULL)
  {
    pcie_osal_debug(MSG_FATAL, "bam_init failed");
    return -1;
  }

  if(BAM_SUCCESS != bamdump_init(pcie_ipadma_bam_handle, PCIE_BAM_NUM_PIPES , PCIE_BAM_NUM_OF_BAMS))
  {
       pcie_bam_dump_alloc_success = FALSE;
  }
  /* Initialize common Pipe configuration structure values */
  ipadma_bam_rxp_pipe_cfg.options = BAM_O_DESC_DONE | BAM_O_OUT_OF_DESC | BAM_O_ERROR
    | BAM_O_EOT | BAM_O_AUTO_ENABLE | BAM_O_POLL; 

  ipadma_bam_rxp_pipe_cfg.mode = BAM_MODE_SYSTEM;
  ipadma_bam_rxp_pipe_cfg.evt_thresh = 0x200; /* Not used - setting to 0x200 as per VI code */

  /* Security features */
  ipadma_bam_rxp_pipe_cfg.lock_group = 0;

 /* RXP Pipe Initialization */ 
  ipadma_bam_rxp_pipe_cfg.desc_base_va = desc_mem_info.VirtualAddr;
  ipadma_bam_rxp_pipe_cfg.desc_base_pa = desc_mem_info.PhysicalAddr;
  ipadma_bam_rxp_pipe_cfg.desc_size = RXP_DESC_FIFO_SIZE;
  ipadma_bam_rxp_pipe_cfg.dir = BAM_DIR_CONSUMER;

  
    pcie_rxp_bam_handle = pcie_bam_pipe_init(PCIE_BAM_RXP_PIPE, &ipadma_bam_rxp_pipe_cfg,NULL);
    if(pcie_rxp_bam_handle == NULL)
    {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "pcie_bam_pipe_init failed");
      return -1;
    }
  
   /* Initialize common Pipe configuration structure values */
  ipadma_bam_rxa_pipe_cfg.options = BAM_O_DESC_DONE | BAM_O_OUT_OF_DESC | BAM_O_ERROR
    | BAM_O_EOT | BAM_O_AUTO_ENABLE | BAM_O_POLL; 

  ipadma_bam_rxa_pipe_cfg.mode = BAM_MODE_SYSTEM;
  ipadma_bam_rxa_pipe_cfg.evt_thresh = 0x200; /* Not used - setting to 0x200 as per VI code */

  /* Security features */
  ipadma_bam_rxa_pipe_cfg.lock_group = 0;

 /* RXP Pipe Initialization */ 
  ipadma_bam_rxa_pipe_cfg.desc_base_va = desc_mem_info.VirtualAddr + RXP_DESC_FIFO_SIZE ;
  ipadma_bam_rxa_pipe_cfg.desc_base_pa = desc_mem_info.PhysicalAddr + RXP_DESC_FIFO_SIZE ;
  ipadma_bam_rxa_pipe_cfg.desc_size = RXA_DESC_FIFO_SIZE;
  ipadma_bam_rxa_pipe_cfg.dir = BAM_DIR_PRODUCER;

  rxa_pipe_callback.func = pcie_rxa_pipe_cb ;
  rxa_pipe_callback.data = NULL ;
  
    pcie_rxa_bam_handle = pcie_bam_pipe_init(PCIE_BAM_RXA_PIPE, &ipadma_bam_rxa_pipe_cfg,&rxa_pipe_callback);
    if(pcie_rxa_bam_handle == NULL)
    {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "pcie_bam_pipe_init failed");
      return -1;
    }
  
     /*Set IPA DMA mode*/

    //result = ipa_tf_set_pipe_in_dma_mode(pcie_rxp_bam_handle,pcie_rxa_bam_handle);
    reg_val = (0xFFFF << HWIO_IPA_ENDP_INIT_MODE_n_BYTE_THRESHOLD_SHFT) |
               (PCIE_BAM_RXA_PIPE << HWIO_IPA_ENDP_INIT_MODE_n_DEST_PIPE_INDEX_SHFT) |
               (0X3 << HWIO_IPA_ENDP_INIT_MODE_n_MODE_SHFT) ;

    HWIO_IPA_ENDP_INIT_MODE_n_OUTI(PCIE_BAM_RXP_PIPE,reg_val) ;

   

    /* Initialize common Pipe configuration structure values */
  ipadma_bam_txa_pipe_cfg.options = BAM_O_DESC_DONE | BAM_O_OUT_OF_DESC | BAM_O_ERROR
    | BAM_O_EOT | BAM_O_AUTO_ENABLE | BAM_O_POLL; 

  ipadma_bam_txa_pipe_cfg.mode = BAM_MODE_SYSTEM;
  ipadma_bam_txa_pipe_cfg.evt_thresh = 0x200; /* Not used - setting to 0x200 as per VI code */

  /* Security features */
  ipadma_bam_txa_pipe_cfg.lock_group = 0;

 /* TXA Pipe Initialization */ 
  ipadma_bam_txa_pipe_cfg.desc_base_va = desc_mem_info.VirtualAddr + RXP_DESC_FIFO_SIZE + RXA_DESC_FIFO_SIZE;
  ipadma_bam_txa_pipe_cfg.desc_base_pa = desc_mem_info.PhysicalAddr+ RXP_DESC_FIFO_SIZE + RXA_DESC_FIFO_SIZE;
  ipadma_bam_txa_pipe_cfg.desc_size = TXA_DESC_FIFO_SIZE;
  ipadma_bam_txa_pipe_cfg.dir = BAM_DIR_CONSUMER;

  
    pcie_txa_bam_handle = pcie_bam_pipe_init(PCIE_BAM_TXA_PIPE, &ipadma_bam_txa_pipe_cfg,NULL);
    if(pcie_txa_bam_handle == NULL)
    {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "pcie_bam_pipe_init failed");
      return -1;
    }

   

  
   
    /* Initialize common Pipe configuration structure values */
  ipadma_bam_txp_pipe_cfg.options = BAM_O_DESC_DONE | BAM_O_OUT_OF_DESC | BAM_O_ERROR
    | BAM_O_EOT | BAM_O_AUTO_ENABLE | BAM_O_POLL; 

 ipadma_bam_txp_pipe_cfg.mode = BAM_MODE_SYSTEM;
  ipadma_bam_txp_pipe_cfg.evt_thresh = 0x200; /* Not used - setting to 0x200 as per VI code */

  /* Security features */
  ipadma_bam_txp_pipe_cfg.lock_group = 0;

 /* TXP Pipe Initialization */ 
  ipadma_bam_txp_pipe_cfg.desc_base_va = desc_mem_info.VirtualAddr + RXP_DESC_FIFO_SIZE + RXA_DESC_FIFO_SIZE + TXA_DESC_FIFO_SIZE;
  ipadma_bam_txp_pipe_cfg.desc_base_pa = desc_mem_info.PhysicalAddr+ RXP_DESC_FIFO_SIZE + RXA_DESC_FIFO_SIZE + TXA_DESC_FIFO_SIZE;
  ipadma_bam_txp_pipe_cfg.desc_size = TXP_DESC_FIFO_SIZE;
  ipadma_bam_txp_pipe_cfg.dir = BAM_DIR_PRODUCER;
   
  txp_pipe_callback.func = pcie_txp_pipe_cb ;
  txp_pipe_callback.data = NULL ;
  
    pcie_txp_bam_handle = pcie_bam_pipe_init(PCIE_BAM_TXP_PIPE, &ipadma_bam_txp_pipe_cfg,&txp_pipe_callback);
    if(pcie_txp_bam_handle == NULL)
    {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "pcie_bam_pipe_init failed");
      return -1;
    }

    /*Set IPA DMA mode*/

    //result = ipa_tf_set_pipe_in_dma_mode(pcie_txa_bam_handle,pcie_txp_bam_handle);
    reg_val = (0xFFFF << HWIO_IPA_ENDP_INIT_MODE_n_BYTE_THRESHOLD_SHFT) |
               (PCIE_BAM_TXP_PIPE << HWIO_IPA_ENDP_INIT_MODE_n_DEST_PIPE_INDEX_SHFT) |
               (0X3 << HWIO_IPA_ENDP_INIT_MODE_n_MODE_SHFT) ;

    HWIO_IPA_ENDP_INIT_MODE_n_OUTI(PCIE_BAM_TXA_PIPE,reg_val) ;

    
    return result ;

}

#endif

#ifndef PCIE_BOOT
/**
 * @brief  pcie_gpio_wake_init 
 *  
 *       PCIe GPIO WAKE# initialization.
  
   
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void pcie_gpio_wake_init(void)
{
   DALResult dRes;

   dRes = DAL_DeviceAttach(DALDEVICEID_TLMM, &h_pcie_wake_tlmm);

    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DAL_DeviceAttach with DALDEVICEID_TLMM failed");
    }

    if(NULL == h_pcie_wake_tlmm)
    {
        pcie_osal_debug(MSG_FATAL, "h_pcie_wake_tlmm is NULL");
        return;
    }
   

   dRes = DalDevice_Open(h_pcie_wake_tlmm, DAL_OPEN_SHARED);

    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalDevice_Open with h_pcie_wake_tlmm failed");
    }


   dRes = DalTlmm_SetInactiveConfig(
         h_pcie_wake_tlmm, 
         (uint32)PCIE_GPIO_WAKE_PINOUT,
         DAL_GPIO_CFG_OUT((uint32)PCIE_GPIO_WAKE_PINOUT, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA, 1)
         );

    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalTlmm_SetInactiveConfig failed");
    }
    
    pcie_wake_pin_config = DAL_GPIO_CFG(
       (uint32)PCIE_GPIO_WAKE_PINOUT, 
        0, 
        DAL_GPIO_OUTPUT, 
       DAL_GPIO_NO_PULL, 
       DAL_GPIO_2MA);
    
    dRes = DalTlmm_ConfigGpio(h_pcie_wake_tlmm,pcie_wake_pin_config, DAL_TLMM_GPIO_ENABLE);

    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalTlmm_ConfigGpio failed");
    }
    

    dRes = DalTlmm_GpioOut(h_pcie_wake_tlmm,pcie_wake_pin_config, DAL_GPIO_HIGH_VALUE);

    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioOut failed");
    }
}

/**
 * @brief  pcie_gpio_clkqreq_init 
 *  
 *       PCIe GPIO CLKREQ# initialization.
  
   
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void pcie_gpio_clkreq_init(void)
{
   DALResult dRes;

   dRes = DAL_DeviceAttach(DALDEVICEID_TLMM, &h_pcie_clkreq_tlmm);

    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DAL_DeviceAttach with DALDEVICEID_TLMM failed");
    }

    if(NULL == h_pcie_clkreq_tlmm)
    {
        pcie_osal_debug(MSG_FATAL, "h_pcie_clkreq_tlmm is NULL");
        return;
    }


   pcie_clkreq_pin_config = DAL_GPIO_CFG(
       (uint32)PCIE_GPIO_CLKREQ_PINOUT, 
        1, 
        DAL_GPIO_INPUT, 
       DAL_GPIO_NO_PULL, 
       DAL_GPIO_2MA);
    
    dRes = DalTlmm_ConfigGpio(h_pcie_clkreq_tlmm,pcie_clkreq_pin_config, DAL_TLMM_GPIO_ENABLE);
    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalTlmm_ConfigGpio failed");
    }
}
#endif /* PCIE_BOOT */



/**
   * @brief pcie_gpio_assert_wake 
   *  
   *Assert PCIe GPIO WAKE# to wake up Host. 
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/

static void pcie_gpio_assert_wake(void)
{
   DALResult dRes;

  if(NULL != h_pcie_wake_tlmm)
  {
    dRes = DalTlmm_GpioOut(h_pcie_wake_tlmm,pcie_wake_pin_config, DAL_GPIO_LOW_VALUE);

    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioOut failed");
    }

    pcie_osal_debug(MSG_HIGH,"Endpoint assert WAKE#",0,0,0);
    
  }

}


/* ============================================================================
**  Function : pcie_gpio_deassert_wake
** ============================================================================
*/
/**
  de-assert PCIe GPIO WAKE#.
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void pcie_gpio_deassert_wake(void)
{
   DALResult dRes;

  if(NULL != h_pcie_wake_tlmm)
  {
    dRes = DalTlmm_GpioOut(h_pcie_wake_tlmm,pcie_wake_pin_config, DAL_GPIO_HIGH_VALUE);

    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioOut failed");
    }

    pcie_osal_debug(MSG_HIGH,"Endpoint deassert WAKE#",0,0,0);
    
  }

}


#ifndef  PCIE_BOOT
/* ============================================================================
**  Function : pcie_gpio_perst_isr
** ============================================================================
*/
/**
  PCIe GPIO PERST# interrupt handler.
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void* pcie_gpio_perst_isr(GPIOINTISRCtx param)
{
   DALResult           dal_result;
   pcie_gpio_perst_cb_param_type *perst_exp = (pcie_gpio_perst_cb_param_type *)param ;
   DALGpioValueType perst_pin; 
   GPIOIntTriggerType trigger; 

   dal_result = DalTlmm_GpioIn(pcie_gpio_perst.h_pcie_tlmm,pcie_gpio_perst.pcie_gpio_pin,&perst_pin);

   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "Could not read gpio");
   }
 
   /* Determine next trigger based on gpio value */ 
   if(DAL_GPIO_HIGH_VALUE == perst_pin) 
   {
      pcie_osal_debug(MSG_ERR, "GPIO line is HIGH");  
      trigger=GPIOINT_TRIGGER_LOW; 
   }
   else
   {
      pcie_osal_debug(MSG_ERR, "GPIO line is LOW");  
      trigger=GPIOINT_TRIGGER_HIGH; 
   }

   /* Set trigger based on gpio value */ 
   dal_result = GPIOInt_SetTrigger(
         pcie_gpio_perst.h_pcie_gpio_int,
         PCIE_GPIO_PERST_PINOUT, 
         trigger);

   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_SetTrigger failed");
   }

   /* If perst pin is not whats expected, nothing to do */
   if(perst_pin != perst_exp->expected_perst)
   {
      pcie_osal_debug(MSG_ERR, "Perst doesnt match expected nothing to do, possible"
            "spurious interrupt or line toggled too fast");
      return 0; 
   }

   if (DAL_GPIO_LOW_VALUE == perst_pin) 
    {
       pcie_osal_debug(MSG_HIGH,"GPIO PERST# is asserted");
       
       /*Call MHI registered PM Callback function*/
       pcie_osal_syncenter(&mhi_pm_cb_sync) ;
       if (NULL != mhi_registered_pm_cb)
       {
          mhi_registered_pm_cb(PCIE_PM_DEVICE_STATE_IN_D3_COLD,cb_passed_param);
       }

       /* Disable msi so it will get re-enabled and ATU will be configured appropriately */
       pcie_msi_config.enabled=FALSE; 

       pcie_osal_syncleave(&mhi_pm_cb_sync) ;

      /*set PERST GPIO trigger as level HIGH: PERST# is asserted*/
      perst_in_expect.expected_perst = 1 ;
    }
   else if (DAL_GPIO_HIGH_VALUE == perst_pin) 
    {
       pcie_osal_debug(MSG_HIGH,"GPIO PERST# is deasserted");

       /*Call MHI registered PM Callback function*/
       pcie_osal_syncenter(&mhi_pm_cb_sync) ;
       if (NULL != mhi_registered_pm_cb)
       {
          mhi_registered_pm_cb(PCIE_PM_PERST_DEASSERTED,cb_passed_param);
       }
	   
       pcie_osal_syncleave(&mhi_pm_cb_sync) ;

      /*set PERST GPIO trigger as level LOW: PERST# is deasserted*/
      perst_in_expect.expected_perst = 0 ;
   }


   return 0 ;
}

/* ============================================================================
**  Function : pcie_gpio_perst_register_isr
** ============================================================================
*/
/**
  Register PCIe GPIO PERST# interrupt handler.
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void pcie_gpio_perst_register_isr(void)
{
   DALResult           dal_result;
  
   dal_result = GPIOInt_RegisterIsr(
      pcie_gpio_perst.h_pcie_gpio_int,
      PCIE_GPIO_PERST_PINOUT,
      GPIOINT_TRIGGER_LOW,
      pcie_gpio_perst_isr,
      (uint32)&perst_in_expect) ;

   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_RegisterIsr failed");
   }

   /*set PERST GPIO trigger as level LOW: PERST# is deasserted*/
   perst_in_expect.expected_perst = 0 ;
   dal_result = GPIOInt_SetTrigger(
    pcie_gpio_perst.h_pcie_gpio_int,
    PCIE_GPIO_PERST_PINOUT, 
    GPIOINT_TRIGGER_LOW);

   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_SetTrigger failed");
   }
}

/* ============================================================================
**  Function : pcie_gpio_perst_deregister_isr
** ============================================================================
*/
/**
  De-Regsiter PCIe GPIO PERST# interrupt handler.
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void pcie_gpio_perst_deregister_isr(void)
{
   DALResult           dal_result; 

   /* Deregister a GPIO interrupt handler.  This also disables the interrupt */
   dal_result = GPIOInt_DeregisterIsr(
      pcie_gpio_perst.h_pcie_gpio_int,
      PCIE_GPIO_PERST_PINOUT,
      pcie_gpio_perst_isr);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_DeregisterIsr failed");
   }
}

/* ============================================================================
**  Function : pcie_gpio_trigger_perst_int
** ============================================================================
*/
/**
  Trigger PERST GPIO interrupt.. Used for internal testing purpose only..
 
  
  
  @param[in]      None.
  
  
  @return
  None
*/
void pcie_gpio_trigger_perst_int(void)
{
   DALResult           dal_result;
   dal_result = GPIOInt_TriggerInterrupt(pcie_gpio_perst.h_pcie_gpio_int,PCIE_GPIO_PERST_PINOUT);
   if(dal_result != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "GPIOInt_TriggerInterrupt failed");
   }
   
}
#endif

#ifndef PCIE_BOOT
/* ============================================================================
**  Function : pcie_install_isrs
** ============================================================================
*/
/**
  Install PCIe interrupt ISR.
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void pcie_install_isrs(void)
{
   pcie_osal_isrinstall(&pcie_dev->link_up_irq_h,PCIE_LINK_UP_INT_IRQ,pcie_link_up_interrupt_handler,(void *)pcie_dev,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);

   pcie_osal_isrinstall(&pcie_dev->link_down_irq_h,PCIE_LINK_DOWN_INT_IRQ,pcie_link_down_interrupt_handler,(void *)pcie_dev,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);

   pcie_osal_isrinstall(&pcie_dev->d_state_irq_h,PCIE_DEVICE_STATE_CHANGE_IRQ,pcie_device_state_change_interrupt_handler,(void *)pcie_dev,DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER);
   pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_DEBUG_INT_EN,PM_DSTATE_CHANGE_BIT),0X1);

   pcie_osal_isrinstall(&pcie_dev->l1_inact_timeout_irq_h,PCIE_L1_INACT_TIMEOUT_IRQ,pcie_l1_inact_timeout_interrupt_handler,(void *)pcie_dev,DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);
   /*Disable L1_INACT_TIMEOUT interrupt by default*/
   pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_DEBUG_INT_EN,L1SUB_TIMEOUT_BIT),0X0);
   pcie_osal_ctrl_interrupt(PCIE_L1_INACT_TIMEOUT_IRQ,FALSE);
  
}

/* ============================================================================
**  Function : pcie_uninstall_isrs
** ============================================================================
*/
/**
  Uninstall PCIe interrupt ISR.
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void pcie_uninstall_isrs(void)
{
    pcie_osal_isruninstall(&pcie_dev->link_down_irq_h);
    pcie_osal_isruninstall(&pcie_dev->link_up_irq_h);
    pcie_osal_isruninstall(&pcie_dev->d_state_irq_h);
    pcie_osal_isruninstall(&pcie_dev->l1_inact_timeout_irq_h);
    pcie_osal_isruninstall(&pcie_dev->bridge_flush_irq_h);
}

/* ============================================================================
**  Function : pcie_config_core
** ============================================================================
*/
/**
  Configure PCIe core.
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/
static void pcie_config_core(void)
{
   pcie_config_type pcie_config;
   pcie_status_type status = PCIE_STATUS_SUCCESS;
   uint8 bar, atu_index;
   pcie_atuconfig_type atu_config = {0};

   pcie_osal_debug(MSG_HIGH,"Begin");
   
   /* Set the default parameters */
   pcie_config.dev_type  = PCIE_DEV_EP;
   pcie_config.tgt_speed = PCIE_CORE_TARGETSPEED_GEN1; 
   pcie_config.device_id = pcie_tgt_cfg->device_id;
   pcie_config.vendor_id = pcie_tgt_cfg->vendor_id;

   status = pcie_core_init(pcie_dev, &pcie_config);
   if(status != PCIE_STATUS_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "pcie_core_init failed");
   }
   
   atu_index = 0x0;
   for(bar = 0; bar<PCIE_MAX_BARS; bar++) 
   {
      pcie_core_setbarconfig(pcie_dev,&pcie_tgt_cfg->bar_config[bar]);
      /* Program the inbound ATU if the BAR is enabled */
      if( 0x0 != pcie_tgt_cfg->bar_config[bar].mask )
      {
         /* Configure the inbound iATU regions */
         atu_config.index           = atu_index; 
         atu_config.dir             = PCIE_ATUDIR_INBOUND;
         atu_config.mode            = PCIE_ATUMODE_BARMATCHED;
         atu_config.bar             = bar;            
         atu_config.target_addr_low = pcie_tgt_cfg->bar_config[bar].local_addr_low; 
         atu_config.target_addr_hi  = pcie_tgt_cfg->bar_config[bar].local_addr_hi;
         pcie_core_setatuconfig(pcie_dev, TRUE, &atu_config);
         atu_index++;
       }
   }
   pcie_osal_debug(MSG_HIGH,"Exit");
}
#endif

/* ============================================================================
**  Function : pcie_reg_dump
** ============================================================================
*/
/**
  Calls the core dump.
  
  @param[in,out]     dump_addr  Address where the registers are dumped . 
  @param[in]         max_size  Maximum allowed size for the dump . 

  @return
  void.
  
  @dependencies
  None.
  
  
*/

void pcie_reg_dump(void *dump_addr, uint32 max_size)
{
  //if(max_size >= sizeof(pcie_coredump_type))
  //{
    //pcie_core_dump(pcie_dev,(pcie_coredump_type *)dump_addr);
    //pcie_coredump = (pcie_coredump_type *)dump_addr;
  //}
}

/* ============================================================================
**  Function : pcie_err_fatal
** ============================================================================
*/
/**
  Calls the core dump in case of error fatal.
  
  
  @return
  void.
  
  @dependencies
  None.
  
  
*/

#ifndef  PCIE_BOOT
static void pcie_err_fatal(void)
{
    pcie_coredump_type *pcie_coredump_ptr = &pcie_coredump;
    pcie_core_enable_clk();
    pcie_osal_debug(MSG_HIGH, "Trying to collect PCIe core dump");
    pcie_core_dump(pcie_dev,pcie_coredump_ptr,PCIE_ERROR_FATAL_DUMP_TYPE);
}
#endif


/* ============================================================================
**  Function : pcie_init
** ============================================================================
*/
/**
  Initializes the PCIe Link.
  
  Initializes the PCIe MAC and the PHY layer and starts link enumeration.
  
  
  @return
  returns the Link Status after initialization.
  
  @dependencies
  None.
  
  @sa
  pcie_link_restart.
*/
pcie_status_type pcie_init( void )
{
#ifndef  PCIE_BOOT
    boolean  err_crash_register_status;
    DALGpioValueType  perst_pin ;
    DALResult dRes;
#else
    pcie_config_type pcie_config;
    int bar, atu_index;
    pcie_atuconfig_type atu_config = {0};
#endif
    pcie_status_type status = PCIE_STATUS_SUCCESS;
    
    if (pcie_is_pcie_fuse_disabled()) 
    {
       return PCIE_FUSE_DISABLED ;
    }
    
    if ( !pcie_is_pcie_boot_enabled()) 
    {
       return PCIE_STATUS_SUCCESS ;
    }

    /* Doing osal_init before pcie_plat_init so that we can error fatal with logs if chip_info versions are not captured */
    pcie_osal_init();

    pcie_osal_mallocmemregion(&scratch_pool, SCRATCH_POOL_SIZE);
    pcie_osal_syncinit(&scratch_sync);
    
    if( NULL == scratch_pool.handle)
    {
       pcie_osal_debug(MSG_FATAL,"Couldn't malloc memregion");
    }
    

    /* Perform platform and target sw-specific initialization */
    status = pcie_plat_init(&pcie_dev, &pcie_tgt_cfg);
    if(PCIE_STATUS_SUCCESS != status)
    {
        return status;
    }

#ifndef  PCIE_BOOT
    if (pcie_sync_init_needed) 
    {
       pcie_osal_syncinit(&mhi_pm_cb_sync) ;
       pcie_sync_init_needed = FALSE ;
    }

    pcie_core_init_pwr_domain();

    pcie_osal_syncinit(&pcie_pmic_npa_sync) ;
    /* Initialize the PMIC NPA client*/
    pcie_core_pmic_npa_client_init();

    pcie_osal_syncenter(&pcie_pmic_npa_sync) ;

    /* Vote for PCIE_ACTIVE_SVS mode */
    pcie_core_pmic_npa_mode_request(PCIE_ACTIVE_SVS);

    pcie_osal_syncleave(&pcie_pmic_npa_sync) ;

    pcie_core_enable_pwr_domain() ;
    pcie_core_init_clk();
    pcie_core_enable_clk(); 
    pcie_osal_memcpy_enable();
    /* Initialize the CLK NPA client*/
    pcie_core_clk_npa_client_init();
    /* Vote for CNOC SVS frequency */
	pcie_core_clk_npa_mode_request();


    /* Init errata */
    pcie_core_initerrata(pcie_dev); 
	
    pcie_install_isrs();
    pcie_gpio_wake_init();
    pcie_gpio_clkreq_init();

    if ( pcie_read_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_PM_STTS,XMLH_LINK_UP)) == 0x1 )
    {
       pcie_gpio_perst_register_isr();
       pcie_osal_select_extern_source(&pcie_pipe_clk_ctl,62500000,0, 2, 0, 0, 0);

       pcie_link_down_counter = 0;
       pcie_link_up_counter = 0 ;
       pcie_dma_init();
       pcie_osal_debug(MSG_HIGH, "Saving inital PCIe core dump");
       pcie_core_dump(pcie_dev,&pcie_coredump,PCIE_FULL_DUMP);
       pcie_send_ltr(PCIE_DEFAULT_LTR);
       pcie_phy_config_update(pcie_dev);
       pcie_core_req_enter_l1();

       pcie_osal_debug(MSG_HIGH,"Exit: XMLH_LINK_UP");
       return PCIE_STATUS_SUCCESS;
    }  

    pcie_gpio_assert_wake();

    dRes = DalTlmm_GpioIn(pcie_gpio_perst.h_pcie_tlmm,pcie_gpio_perst.pcie_gpio_pin,&perst_pin);
    if(dRes != DAL_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioIn failed");
    }
    
    while(perst_pin != DAL_GPIO_HIGH_VALUE) 
    {
       dRes = DalTlmm_GpioIn(pcie_gpio_perst.h_pcie_tlmm,pcie_gpio_perst.pcie_gpio_pin,&perst_pin);
       if(dRes != DAL_SUCCESS)
       {
           pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioIn failed");
       }
    }   
    
    pcie_osal_debug(MSG_HIGH,"perst_pin value high");		

	pcie_gpio_deassert_wake();
	
    /* register callback during error fatal */
    if(pcie_err_fatal_callback_registered == FALSE)
    {
        /* Make sure this callback is only registered once */
        err_crash_register_status = err_crash_cb_register(pcie_err_fatal);
		if(!err_crash_register_status)
        {
            pcie_osal_debug(MSG_HIGH, "Could not Register callback for Error Fatal");
        }
        else
        {
            pcie_err_fatal_callback_registered = TRUE;
            pcie_osal_debug(MSG_HIGH, "pRegistered callback for Error Fatal");
        }
    }

    /* Initialize the PHY */
    status = pcie_phy_init(pcie_dev); 
    
    if(status != PCIE_STATUS_SUCCESS)
    {
        pcie_osal_debug(MSG_FATAL, "pcie_phy_init failed");
    }
    pcie_osal_select_extern_source(&pcie_pipe_clk_ctl,62500000,0, 2, 0, 0, 0);   
    pcie_config_core();
    /* The first BAR is to be routed to MHI */
    pcie_core_configmhigate(pcie_dev, &pcie_tgt_cfg->bar_config[0]);
   
    /* Finally enable the link training and enumeration */
    pcie_core_enableltssm(pcie_dev );

    pcie_osal_debug(MSG_HIGH,"Starting enumeration");
    while ( PCIE_LINKSTATUS_ENUMERATED != pcie_getlinkstate() ) ;
    pcie_osal_debug(MSG_HIGH,"PCIE_LINKSTATUS_ENUMERATED");
    pcie_gpio_perst_register_isr();
    pcie_link_down_counter = 0;
    pcie_link_up_counter = 0 ;

    pcie_dma_init();

    pcie_osal_debug(MSG_HIGH, "Saving inital PCIe core dump");
    pcie_core_dump(pcie_dev,&pcie_coredump,PCIE_FULL_DUMP);
    pcie_send_ltr(PCIE_DEFAULT_LTR);

    pcie_osal_debug(MSG_HIGH,"Exit");

#else  /* PCIE_BOOT */


  if(!pcie_core_isenumerated(pcie_dev))
  {

        /* Set the default parameters */
        pcie_config.dev_type  = PCIE_DEV_EP;
        pcie_config.tgt_speed = PCIE_CORE_TARGETSPEED_GEN1; 
        pcie_config.device_id = pcie_tgt_cfg->device_id;
        pcie_config.vendor_id = pcie_tgt_cfg->vendor_id;

        status = pcie_core_init(pcie_dev, &pcie_config);
        if(PCIE_STATUS_SUCCESS != status)
        {
            return status;
        }

        atu_index = 0x0;
        for(bar = 0; bar<PCIE_MAX_BARS; bar++) 
        {
            pcie_core_setbarconfig(pcie_dev,&pcie_tgt_cfg->bar_config[bar]);

            /* Program the ATU if the BAR is enabled */
            if( 0x0 != pcie_tgt_cfg->bar_config[bar].mask )
            {
                /* Configure the inbound iATU regions */
                atu_config.index           = atu_index; 
                atu_config.dir             = PCIE_ATUDIR_INBOUND;
                atu_config.mode            = PCIE_ATUMODE_BARMATCHED;
                atu_config.bar             = bar;            
                atu_config.target_addr_low = pcie_tgt_cfg->bar_config[bar].local_addr_low; 
                atu_config.target_addr_hi  = pcie_tgt_cfg->bar_config[bar].local_addr_hi;
                pcie_core_setatuconfig(pcie_dev, TRUE, &atu_config);
                atu_index++;
            }
        }

        /* The first BAR is to be routed to MHI */
        pcie_core_configmhigate(pcie_dev, &pcie_tgt_cfg->bar_config[0]);

      
        /* Finally enable the link training and enumeration */
        pcie_core_enableltssm(pcie_dev );

        #ifndef PCIE_VIRTIO
         while ( PCIE_LINKSTATUS_ENUMERATED != pcie_getlinkstate() ) ;
        
        #endif
       
       
    }
    pcie_dma_init();
#endif    
     
    return PCIE_STATUS_SUCCESS;
}

/* ============================================================================
**  Function : pcie_deinit
** ============================================================================
*/
/**
  release resource allocated to PCIe module.
  
  
  
  @return
  returns the Link Status after deinitialization.
*/
void pcie_deinit( void )
{
#ifndef  PCIE_BOOT
   pcie_uninstall_isrs();
   pcie_gpio_perst_deregister_isr();
   if (!pcie_sync_init_needed) 
   {
      pcie_osal_syncdestroy(&mhi_pm_cb_sync);
      pcie_sync_init_needed = TRUE ;
   }
   pcie_osal_syncdestroy(&pcie_pmic_npa_sync) ;
#else
   pcie_osal_isruninstall(&pcie_dev->link_down_irq_h);
   pcie_osal_isruninstall(&pcie_dev->link_up_irq_h);
#endif
   pcie_osal_deinit();
}

/* ============================================================================
**  Function : pcie_getlinkstate
** ============================================================================
*/
/**
  Returns the status of the link.
  
  Returns the status of the link. The link can be in one of the three possible
  states as defined in pcie_linkstate_type.
  
  @return
  Returns the status of the link.

*/
pcie_linkstatus_type pcie_getlinkstate(void)
{
    pcie_linkstatus_type link_status = pcie_core_getlinkstate(pcie_dev, &pcie_link_info.link_state);

    if(PCIE_LINKSTATUS_UP != link_status)
    {
       return link_status;
    }
    pcie_link_info.enum_status = pcie_core_isenumerated(pcie_dev);
    return ((TRUE == pcie_link_info.enum_status)?PCIE_LINKSTATUS_ENUMERATED:PCIE_LINKSTATUS_UP);
}

/* ============================================================================
**  Function : pcie_sendmsi
** ============================================================================
*/
/**
  Send an MSI with the <c>msi_vector</c> payload to the host.
  
  Send an MSI with the <c>msi_vector</c> payload to the host.
  
  @param[in]  msi_vector    Payload for the msi
  
  @return
  Status of the operation.
  
  @note
  MSIs are optional and this functional can fail under normal circumstances as
  well. So the failure should not be considered fatal, unless the EP was
  instructed to use MSIs.

*/
pcie_status_type pcie_sendmsi(uint32 msi_vector)
{
   /* First check if the msi config we have matches the one in hardware */
   pcie_msiconfig_type curr_msi_config = {0};

   #ifndef PCIE_TRADITIONAL_SENDMSI
      pcie_atuconfig_type atu_config = {0};
      uint32 mask, value;
   #endif
   /* Check if the MSI info has changed */

#ifndef PCIE_BOOT
   if(0x0 == pcie_msi_config.enabled)
   {
#endif
    pcie_core_getmsiconfig(pcie_dev,&curr_msi_config);

    #ifndef PCIE_TRADITIONAL_SENDMSI
       /* Old Method of generating MSIs */
       if( (curr_msi_config.enabled!=pcie_msi_config.enabled) ||
           (curr_msi_config.addr_hi!=pcie_msi_config.addr_hi) || 
           (curr_msi_config.addr_low!=pcie_msi_config.addr_low) ||
           (curr_msi_config.data!=pcie_msi_config.data) ||
           (curr_msi_config.num_multi_msgs!=pcie_msi_config.num_multi_msgs)
           )
       {
           /* MSI info has changed. Setup the ATU to redirect to this new region */
           pcie_msi_config = curr_msi_config;
           atu_config.index = PCIE_OATU_INDEX_MSI;
           atu_config.dir = PCIE_ATUDIR_OUTBOUND;
           atu_config.mode = PCIE_ATUMODE_ADDRESSMATCHED;
           atu_config.base_addr_hi = 0x0;
           atu_config.base_addr_low = ((pcie_dev->memmap_base_phys+PCIE_AXI_MEMOFFSET_FOR_MSI)& ~0xFFF);
           atu_config.target_addr_hi = pcie_msi_config.addr_hi;
           atu_config.target_addr_low = pcie_msi_config.addr_low;
           atu_config.size = 0x1000;
           pcie_core_setatuconfig(pcie_dev, TRUE, &atu_config);
           pcie_msi_config.local_addr.pa_hi = pcie_msi_config.local_addr.va_hi = 0x0;
           pcie_msi_config.local_addr.pa_low = (((pcie_dev->memmap_base_phys+PCIE_AXI_MEMOFFSET_FOR_MSI)& ~0xFFF) | (pcie_msi_config.addr_low & 0xFFF));
           pcie_msi_config.local_addr.va_low = (((pcie_dev->memmap_base+PCIE_AXI_MEMOFFSET_FOR_MSI)& ~0xFFF) | (pcie_msi_config.addr_low & 0xFFF));
       }
    #endif
#ifndef PCIE_BOOT
   }
#endif
   
#ifndef PCIE_BOOT
   if(0x1 == pcie_msi_config.enabled)
   {
#else
   if(0x1 == curr_msi_config.enabled)
   {
#endif
      /* Send the msi */
      #ifndef PCIE_TRADITIONAL_SENDMSI
         mask = (1 << pcie_msi_config.num_multi_msgs) - 1;
         #ifndef PCIE_BOOT
           value = (pcie_msi_config.data & ~mask) | (msi_vector & mask);
           pcie_write_va((char*)&value, pcie_msi_config.local_addr.va_low, 4, NULL);
         #else
           value = pcie_msi_config.data  | (msi_vector & mask);

           *((uint32 *)pcie_msi_config.local_addr.va_low)= value;
         #endif
      #else
         pcie_core_sendparfmsi(pcie_dev, msi_vector);
      #endif /* #ifdef PCIE_TRADITIONAL_SENDMSI */

      pcie_flush();

      return PCIE_STATUS_SUCCESS;
   }
   else
   {
      return PCIE_STATUS_MSIDISABLED;
   }
}


/* ============================================================================
**  Function : pcie_add_index_obatu
** ============================================================================
*/
/**
Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.

Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.
 
@param[in]      index     Index to be added in IATU. 
@param[in]      host_addr     Address of the host memory to be access enabled
@param[in,out]  local_addr    Address relevant in the endpoint memory space
@param[in]      size          size of the region

@return
Status of the operation
*/
int16  pcie_add_index_obatu(uint8 index ,pcie_address_type host_addr, pcie_address_type *local_addr, uint32 size)
{
   pcie_atuconfig_type atu_config;
    
   if((1 != index) && (2 != index))
   {
      return -1 ;
   }

   atu_config.index = index ;
   atu_config.dir = PCIE_ATUDIR_OUTBOUND;
   atu_config.mode = PCIE_ATUMODE_ADDRESSMATCHED;
   atu_config.base_addr_hi = 0x0;

   
   if( 1 == index)
   {
      if( (size >= PCIE_AXI_MEMSPACE_LIMIT ) || (((PCIE_AXI_MEMOFFSET_FOR_MHICTRL + size -1) | 0XFFF) > mhidata_lower_offset) ) 
      {
         /*MHI data region overlap with MHI control region*/
         pcie_osal_debug(MSG_FATAL, "MHI data region overlap with MHI control region");
         return -1 ;
      }
       
      mhictrl_upper_offset = PCIE_AXI_MEMOFFSET_FOR_MHICTRL + size - 1 ;
      mhictrl_upper_offset = mhictrl_upper_offset | 0XFFF ;
      atu_config.base_addr_low = pcie_dev->memmap_base_phys + PCIE_AXI_MEMOFFSET_FOR_MHICTRL ;
      local_addr->pa_hi = 0 ;
      local_addr->pa_low = pcie_dev->memmap_base_phys+ PCIE_AXI_MEMOFFSET_FOR_MHICTRL ;
      local_addr->va_hi = 0 ;
      local_addr->va_low = pcie_dev->memmap_base + PCIE_AXI_MEMOFFSET_FOR_MHICTRL ;
      
   }
   else if( 2 == index)
   {
      if( (size >= PCIE_AXI_MEMSPACE_LIMIT) ||(((PCIE_AXI_MEMSPACE_LIMIT - size ) & 0XFFFFF000 ) < mhictrl_upper_offset) )
      {
         /*MHI data region overlap with MHI control region*/
         pcie_osal_debug(MSG_FATAL, "MHI data region overlap with MHI control region");
         return -1 ;
      }
      mhidata_lower_offset = (PCIE_AXI_MEMSPACE_LIMIT - size) & 0XFFFFF000 ;
      atu_config.base_addr_low = pcie_dev->memmap_base_phys + mhidata_lower_offset ;
      local_addr->pa_hi = 0 ;
      local_addr->pa_low = pcie_dev->memmap_base_phys+ mhidata_lower_offset  ;
      local_addr->va_hi = 0 ;
      local_addr->va_low = pcie_dev->memmap_base + mhidata_lower_offset  ;
   }

   atu_config.target_addr_hi = host_addr.pa_hi;
   atu_config.target_addr_low = host_addr.pa_low;
   atu_config.size = size;
   pcie_core_setatuconfig(pcie_dev, TRUE, &atu_config);
       
   return PCIE_STATUS_SUCCESS;
}

/* ============================================================================
**  Function : pcie_remove_index_obatu
** ============================================================================
*/
/**
Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.

Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.
 
@param[in]      index     Index to be removed from IATU. 


@return
Status of the operation
*/
uint8  pcie_remove_index_obatu(uint8 index )
{
   /* 1. Setup the Index Register */
   pcie_write_reg(pcie_dev->dbi_base,IATU_VIEWPORT_REG, index);
   pcie_osal_memorybarrier();
       
   /*Disabled the index in IATU */
   pcie_write_reg(pcie_dev->dbi_base,PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0, 0X0) ;
   if (index == 1) 
   {
      mhictrl_upper_offset = PCIE_AXI_MEMOFFSET_FOR_MHICTRL + 0XFFF ;
   }
   else if (index == 2) 
   {
      mhidata_lower_offset = PCIE_AXI_MEMSPACE_LIMIT - 0X1000 ;
   }

   return index ;
}

/* ============================================================================
**  Function : pcie_read_pa
** ============================================================================
*/
/**
Reads data from the host memory to a physical address

Reads data from the host memory via PCIe link. 


@param[in]  buf             Physical Buffer to read into 
@param[in]     pcie_axi_addr    PCIe AXI address in modem address space
@param[in]      size        Size of the data to be read in bytes 
@param[in]      user data         user data for this tranaction


@return
PCIE_STATUS_SUCCESS - Successfully .
!PCIE_STATUS_SUCCESS - An error code .

*/

uint32 pcie_read_pa(void* buf, uint32 pcie_axi_addr, uint32 size, void* user_data)
{
    #ifdef PCIE_AHB_AXI_CLK_CHECK
    if( pcie_core_is_ahb_or_axi_clk_off() )
    {
        /* Error fatal */
        pcie_osal_debug(MSG_FATAL, "pcie_core_is_ahb_or_axi_clk_off is true");
        return -1 ;
    }
    #endif

   if (NULL == user_data) 
   {      
      pcie_osal_memcpy(buf,(void *)pcie_axi_addr,size);     
   }
   
   return PCIE_STATUS_SUCCESS;


}

/* ============================================================================
**  Function : pcie_read_va
** ============================================================================
*/
/**
Reads data from the host memory.

Reads data from the host memory via PCIe link. 


@param[in]     buf             Buffer to read into 
@param[in]     pcie_axi_addr   PCIe AXI address in modem address space
@param[in]     size            Size of the data to be read in bytes 
@param[in]     user data       user data for this tranaction


@return
PCIE_STATUS_SUCCESS - Successfully .
!PCIE_STATUS_SUCCESS - An error code .

*/

uint32 pcie_read_va(void* buf, uint32 pcie_axi_addr, uint32 size, void* user_data)
{
#ifndef  PCIE_BOOT
    uint32 dst;
#else
    dma_read_write_req_type read_req ;
#endif
    uint32 ret=PCIE_STATUS_SUCCESS;
    #ifdef PCIE_AHB_AXI_CLK_CHECK
    if( pcie_core_is_ahb_or_axi_clk_off() )
    {
        /* Error fatal */
        pcie_osal_debug(MSG_FATAL, "pcie_core_is_ahb_or_axi_clk_off is true");
        return -1 ;
    }
    #endif

#ifndef  PCIE_BOOT
   if (NULL == user_data) 
   {      
      /* Use local un-cached scratch pool if size is less than limit */
      if (size < SCRATCH_POOL_SIZE)
      {
         pcie_osal_syncenter(&scratch_sync);
         pcie_osal_memcpy((void*)scratch_pool.pa,(void *)pcie_axi_addr,size );     
         memcpy(buf, (void*)scratch_pool.va, size);
         pcie_osal_syncleave(&scratch_sync);
      }
      else
      {
         dst=(uint32)memsection_virt_to_phys((uintptr_t)buf, NULL );
         KxCache_DCacheInvalidateBytes((addr_t)buf, size);
         pcie_osal_memcpy((void*)dst,(void *)pcie_axi_addr,size);     
         KxCache_DCacheInvalidateBytes((addr_t)buf, size);
      }
   }
   #ifdef PCIE_USE_BAMDMA_ASYNC_MODE   
   else
   {
      KxCache_DCacheInvalidateBytes((addr_t)buf, size);      
      ret=pcie_osal_async_memcpy((void *)memsection_virt_to_phys((uintptr_t)buf,NULL),(void *)pcie_axi_addr,size,user_data); 

      if (BAMDMA_INSUFFICIENT_RESOURCES == ret)
         ret=PCIE_TRANSFER_REQUESTS_FULL;
      else if (BAMDMA_TRANSFER_SUCCESS != ret && BAMDMA_SUCCESS != ret)
         ret=PCIE_STATUS_ERROR;
      else
         ret=PCIE_STATUS_SUCCESS;
   }
   #endif   
#else
    if (NULL == user_data) 
    {
         pcie_osal_memcpy(buf,(void *)pcie_axi_addr,size);     
    }
    else
    {
       read_req.src_addr = pcie_axi_addr ;
       read_req.dest_addr = (uint32)buf ;
       read_req.size = size ;
       read_req.user_data = user_data ;
       pcie_read_dma(read_req);
    }
#endif
   
   return ret;
}

/* ============================================================================
**  Function : pcie_write_pa
** ============================================================================
*/
/**
Writes data to the host memory using a physical buffer

Writes data to the host memory via PCIe link. 


@param[in]     buf             Buffer that contains data to be written  into host
@param[in]     pcie_axi_addr   PCIe AXI virtual address in modem address space
@param[in]     size            Size of the data to be written in bytes
@param[in]     user data       user data for this tranaction

@return
PCIE_STATUS_SUCCESS - Successfully .
!PCIE_STATUS_SUCCESS - An error code .

*/

uint32 pcie_write_pa(void* buf, uint32 pcie_axi_addr, uint32 size,void* user_data)
{
 #ifdef PCIE_AHB_AXI_CLK_CHECK
    if( pcie_core_is_ahb_or_axi_clk_off() )
    {
       /* Error fatal */
       pcie_osal_debug(MSG_FATAL, "pcie_core_is_ahb_or_axi_clk_off is true");
       return -1 ;
    }
    #endif

   if (NULL == user_data) 
   {
      pcie_osal_memcpy((void *)pcie_axi_addr,buf, size);
   }


   return PCIE_STATUS_SUCCESS;
}

/* ============================================================================
**  Function : pcie_write_va
** ============================================================================
*/
/**
Writes data to the host memory.

Writes data to the host memory via PCIe link. 


@param[in]     buf             Buffer that contains data to be written  into host
@param[in]     pcie_axi_addr   PCIe AXI virtual address in modem address space
@param[in]     size            Size of the data to be written in bytes
@param[in]     user data       user data for this tranaction

@return
PCIE_STATUS_SUCCESS - Successfully .
!PCIE_STATUS_SUCCESS - An error code .

*/

uint32 pcie_write_va(void* buf, uint32 pcie_axi_addr, uint32 size,void* user_data)
{
#ifndef  PCIE_BOOT
    void *src;
#else
    dma_read_write_req_type write_req ;
#endif
    uint32 ret=PCIE_STATUS_SUCCESS;
    #ifdef PCIE_AHB_AXI_CLK_CHECK
    if( pcie_core_is_ahb_or_axi_clk_off() )
    {
       /* Error fatal */
       pcie_osal_debug(MSG_FATAL, "pcie_core_is_ahb_or_axi_clk_off is true");
       return -1 ;
    }
    #endif

#ifndef  PCIE_BOOT
   if (NULL == user_data) 
   {
      /* Use local un-cached scratch pool if size is less than limit */
      if (size < SCRATCH_POOL_SIZE)
      {
         pcie_osal_syncenter(&scratch_sync);
         src=(void*)scratch_pool.pa;
         memcpy((void*)scratch_pool.va, (void*)buf, size);
         pcie_osal_memcpy((void *)pcie_axi_addr,src, size);
         pcie_osal_syncleave(&scratch_sync);
      }
      else
      {
         KxCache_DCacheFlushBytes((addr_t)buf, size);
         src=(void*)memsection_virt_to_phys((uintptr_t)buf, NULL );
         pcie_osal_memcpy((void *)pcie_axi_addr,src, size);
      }
   }
   #ifdef PCIE_USE_BAMDMA_ASYNC_MODE   
   else
   {  
      KxCache_DCacheFlushBytes((addr_t)buf, size);
      ret=pcie_osal_async_memcpy((void *)pcie_axi_addr,(void *)memsection_virt_to_phys((uintptr_t)buf,NULL),size,user_data);

      if (BAMDMA_INSUFFICIENT_RESOURCES == ret)
         ret=PCIE_TRANSFER_REQUESTS_FULL;
      else if (BAMDMA_TRANSFER_SUCCESS != ret && BAMDMA_SUCCESS != ret)
         ret=PCIE_STATUS_ERROR;
      else
         ret=PCIE_STATUS_SUCCESS;
   }
   #endif  
#else
    if (NULL == user_data) 
    {
        pcie_osal_memcpy((void *)pcie_axi_addr,buf, size);
    }
    else
    {
       write_req.src_addr = (uint32)buf ;
       write_req.dest_addr = pcie_axi_addr ;
       write_req.size = size ;
       write_req.user_data = user_data ;
       pcie_write_dma(write_req);
    }
#endif

   return ret;
}

#ifndef  PCIE_BOOT
/* ============================================================================
**  Function : pcie_core_link_down_interrupt_handler
** ============================================================================
*/
/**
  Handle PCIe link down.reset the PCIe PHY and Core and try to bring PCIe link back to L0 state.
    
  @param[in,out]  None

  @return
  None.
  
*/
static void pcie_link_down_interrupt_handler(void *pcie_dev_r)
{
  
   pcie_link_down_counter++ ;
   pcie_osal_debug(MSG_HIGH,"PCIE link down interrupt is firing",0,0,0);
   pcie_osal_syncenter(&mhi_pm_cb_sync) ;
   if (NULL != mhi_registered_pm_cb) 
   {
      mhi_registered_pm_cb(PCIE_LINK_DOWN,cb_passed_param);
   }
   pcie_osal_syncleave(&mhi_pm_cb_sync) ;
   
}

/* ============================================================================
**  Function : pcie_core_link_up_interrupt_handler
** ============================================================================
*/
/**
  Handle PCIe link up .
    
  @param[in,out]  None

  @return
  None.
  
*/
static void pcie_link_up_interrupt_handler(void *pcie_dev_r)
{
   pcie_link_up_counter++ ;
   pcie_osal_syncenter(&mhi_pm_cb_sync) ;
   if (NULL != mhi_registered_pm_cb) 
   {
      mhi_registered_pm_cb(PCIE_LINK_UP,cb_passed_param);
   }
   pcie_osal_syncleave(&mhi_pm_cb_sync) ;
   pcie_osal_debug(MSG_HIGH,"PCIE link up interrupt is firing",0,0,0);
}
#endif

/* ============================================================================
**  Function : pcie_set_mhi_ipa_dbs
** ============================================================================
*/
/**
Set register PCIE20_PARF_MHI_IPA_DBS.



@param[in]     cdb_base    channel doorbell base. 
@param[in]     cdb_end     channel doorbell end. 
@param[in]     edb_base    event doorbell base. 
@param[in]     edb_end     event doorbell end.  



@return
None

*/
void pcie_set_mhi_ipa_dbs(uint8 cdb_base,uint8 cdb_end,uint8 edb_base,uint8 edb_end)
{
   uint32 reg_val ;
   reg_val = ((uint32)cdb_base) | ((uint32)cdb_end<<8) | ((uint32)edb_base <<16) | ((uint32)edb_end <<24) ;
   pcie_write_reg(pcie_dev->parf_base,PCIE20_PARF_MHI_IPA_DBS,reg_val);
}

/* ============================================================================
**  Function : pcie_set_mhi_ipa_cdb_target
** ============================================================================
*/
/**
Set register PCIE20_PARF_MHI_IPA_CDB_TARGET_LOWER .



@param[in]     mhi_ipa_cdb_addr    MHI IPA channel Doorbell Target address



@return
None

*/
void pcie_set_mhi_ipa_cdb_target(uint32 mhi_ipa_cdb_addr)
{
   pcie_write_reg(pcie_dev->parf_base,PCIE20_PARF_MHI_IPA_CDB_TARGET_LOWER,mhi_ipa_cdb_addr);
}

/* ============================================================================
**  Function : pcie_set_mhi_ipa_edb_target
** ============================================================================
*/
/**
Set register PCIE20_PARF_MHI_IPA_EDB_TARGET_LOWER .



@param[in]     mhi_ipa_edb_addr   MHI IPA event Doorbell Target address



@return
None

*/
void pcie_set_mhi_ipa_edb_target(uint32 mhi_ipa_edb_addr)
{
   pcie_write_reg(pcie_dev->parf_base,PCIE20_PARF_MHI_IPA_EDB_TARGET_LOWER,mhi_ipa_edb_addr);
}

/* ============================================================================
**  Function : pcie_get_msi_config
** ============================================================================
*/
/**
return MSI configuration information. 



@param[in]      pcie_msi_info PCIE MSI  information.


@return
 Void

*/
void pcie_get_msi_config(pcie_msiconfig_type* pcie_msi_info)
{
   pcie_msiconfig_type curr_msi_config = {0};
   pcie_atuconfig_type atu_config = {0};

   if(0x0 == pcie_msi_config.enabled)
   {

   /* Check if the MSI info has changed */
   pcie_core_getmsiconfig(pcie_dev,&curr_msi_config);

   if( (curr_msi_config.enabled!=pcie_msi_config.enabled) ||
        (curr_msi_config.addr_hi!=pcie_msi_config.addr_hi) || 
        (curr_msi_config.addr_low!=pcie_msi_config.addr_low) ||
        (curr_msi_config.data!=pcie_msi_config.data) ||
        (curr_msi_config.num_multi_msgs!=pcie_msi_config.num_multi_msgs)
        )
   {
      /* MSI info has changed. Setup the ATU to redirect to this new region */
      pcie_msi_config = curr_msi_config;
      atu_config.index = PCIE_OATU_INDEX_MSI;
      atu_config.dir = PCIE_ATUDIR_OUTBOUND;
      atu_config.mode = PCIE_ATUMODE_ADDRESSMATCHED;
      atu_config.base_addr_hi = 0x0;
      atu_config.base_addr_low = ((pcie_dev->memmap_base_phys+PCIE_AXI_MEMOFFSET_FOR_MSI)& ~0xFFF);
      atu_config.target_addr_hi = pcie_msi_config.addr_hi;
      atu_config.target_addr_low = pcie_msi_config.addr_low;
      atu_config.size = 0x1000;

      pcie_core_setatuconfig(pcie_dev, TRUE, &atu_config);
      pcie_msi_config.local_addr.pa_hi = pcie_msi_config.local_addr.va_hi = 0x0;
      pcie_msi_config.local_addr.pa_low = (((pcie_dev->memmap_base_phys+PCIE_AXI_MEMOFFSET_FOR_MSI)& ~0xFFF) | (pcie_msi_config.addr_low & 0xFFF));
      pcie_msi_config.local_addr.va_low = (((pcie_dev->memmap_base+PCIE_AXI_MEMOFFSET_FOR_MSI)& ~0xFFF) | (pcie_msi_config.addr_low & 0xFFF));
   }
       
   }

   pcie_msi_info->addr_low = pcie_msi_config.addr_low;
   pcie_msi_info->addr_hi = pcie_msi_config.addr_hi;
   pcie_msi_info->data = pcie_msi_config.data;
   pcie_msi_info->enabled = pcie_msi_config.enabled;
   pcie_msi_info->num_multi_msgs = pcie_msi_config.num_multi_msgs;

   pcie_msi_info->local_addr.pa_hi = pcie_msi_info->local_addr.va_hi = 0x0;
   pcie_msi_info->local_addr.pa_low = (((pcie_dev->memmap_base_phys+PCIE_AXI_MEMOFFSET_FOR_MSI)& ~0xFFF) | (pcie_msi_config.addr_low & 0xFFF));
   pcie_msi_info->local_addr.va_low = (((pcie_dev->memmap_base+PCIE_AXI_MEMOFFSET_FOR_MSI)& ~0xFFF) | (pcie_msi_config.addr_low & 0xFFF));
}

/* ============================================================================
**  Function : pcie_is_pcie_boot_enabled
** ============================================================================
*/
/**
return True if PCIe boot is enabled. 
     


@return
 Boolean

*/

boolean  pcie_is_pcie_boot_enabled(void)
{
#ifndef PCIE_BOOT
   uint32 read_fuse;

   #if 0
    read_fuse = HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK) ;
    read_fuse = read_fuse >> HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT ;
   #endif
   
   read_fuse = HWIO_BOOT_CONFIG_INM(HWIO_BOOT_CONFIG_FAST_BOOT_BMSK) ;
   read_fuse >>= HWIO_BOOT_CONFIG_FAST_BOOT_SHFT ;
   
   if ((0x1 == read_fuse) || pcie_init_needed || DalPlatformInfo_IsFusion() ) 
   {
      return TRUE ;
   }

   return FALSE ;
#else
   return TRUE;
#endif
}

/* ============================================================================
**  Function : pcie_is_pcie_fuse_disabled
** ============================================================================
*/
/**
return True if PCIe fuse is disabled. 
     


@return
 Boolean - True if fuse is disabled else returns false

*/

boolean  pcie_is_pcie_fuse_disabled(void)
{
#ifndef PCIE_BOOT
   uint32 read_fuse;

   read_fuse = HWIO_INF(FEATURE_CONFIG1, PCIE_DISABLE);
   
   if (read_fuse) 
   {
      return TRUE ;
   }

   return FALSE ;
#else
   return FALSE ;
#endif
}

/* ============================================================================
**  Function : pcie_mhi_register_pm_cb
** ============================================================================
*/
/**
  MHI register power management call back with PCIe driver.
  
  
  
  @param[in]      host_addr     Address of the host memory to be access enabled
  @param[in]      param         pointer to parameters provided by client.
  
  @return
  void
*/
void pcie_mhi_register_pm_cb(pcie_pm_cb_type mhi_pm_cb,void *param)
{
#ifndef PCIE_BOOT
   pcie_osal_debug(MSG_HIGH,"MHI register Power management callback",0,0,0);
   pcie_osal_syncenter(&mhi_pm_cb_sync) ;
   mhi_registered_pm_cb = mhi_pm_cb ;
   cb_passed_param = param ;
   pcie_osal_syncleave(&mhi_pm_cb_sync);
#endif
}

#ifndef  PCIE_BOOT
/* ============================================================================
**  Function : pcie_device_state_change_interrupt_handler
** ============================================================================
*/
/**
  PCIe device state change interrupt handler.
  
  
  
  @param[in]      pcie_dev_r    pointer to pcie_dev info.
  
  
  @return
  void
*/
static void pcie_device_state_change_interrupt_handler(void *pcie_dev_r)
{
   uint8 cur_d_state ;
   pcie_devinfo_type *pcie_dev_info = (pcie_devinfo_type *)pcie_dev_r ;

   cur_d_state = pcie_read_reg_field(pcie_dev_info->dbi_base,PCIE_FLD(TYPE0_CON_STATUS_REG_1,POWER_STATE)) ;
   pcie_osal_debug(MSG_HIGH,"Device State change interrupt is firing, Device state=%d",cur_d_state,0,0);
   pcie_osal_syncenter(&mhi_pm_cb_sync) ;
   if (NULL != mhi_registered_pm_cb) 
   {
      if (0x0 == cur_d_state)
      {
         /*indicate to core that link  isn't ready to enter L23*/
         pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_PM_CTRL,READY_ENTR_L23),0);
         mhi_registered_pm_cb(PCIE_PM_DEVICE_STATE_IN_D0,cb_passed_param);
      }
      else if (0x3 == cur_d_state) 
      {
         mhi_registered_pm_cb(PCIE_PM_DEVICE_STATE_IN_D3_HOT,cb_passed_param);
         /* By Spec when we enter a non-D0 state, we must clear the
            requirements for both the LTRs */
         pcie_send_ltr(0);
         /*indicate to core that link  is ready to enter L23*/
         pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_PM_CTRL,READY_ENTR_L23),1);
      }
   }
   pcie_osal_syncleave(&mhi_pm_cb_sync) ;
}

/* ============================================================================
**  Function : pcie_l1_inact_timeout_interrupt_handler
** ============================================================================
*/
/**
  PCIe L1SS inactivity timer interrupt handler.
  
  
  
  @param[in]      pcie_dev_r    pointer to pcie_dev info.
  
  
  @return
  void
*/
static void pcie_l1_inact_timeout_interrupt_handler(void *pcie_dev_r)
{
   uint32 l1sub_cnt_val;
   l1sub_cnt_val = pcie_read_reg(pcie_dev->mhi_base,PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1SUB);
   pcie_osal_debug(MSG_HIGH,"L1 inactive time out interrupt is firing with l1subcnt=0x%x", l1sub_cnt_val);

   pcie_osal_syncenter(&mhi_pm_cb_sync) ;
   pcie_control_l1ss_inact_interrupt(FALSE,PCIE_L1SUB_AHB_CLK_MAX_TIMER_ZERO_VAL);

   if (NULL != mhi_registered_pm_cb)
   {
      mhi_registered_pm_cb(PCIE_PM_L1_INACT_TIMEOUT,cb_passed_param);
   }  
    
   pcie_osal_syncleave(&mhi_pm_cb_sync) ;
}
#endif

/* ============================================================================
**  Function : pcie_assert_gpio_wake
** ============================================================================
*/
/**
  Assert GPIO siganl WAKE# to wakeup host.
 
  
  
  @param[in]      void
  
  
  @return
  void
*/
void pcie_wakeup_host(void)
{
   pcie_gpio_assert_wake();
}

/* ============================================================================
**  Function : pcie_enable_ltssm
** ============================================================================
*/
/**
  API to enable LTSSSM.
 
  
  
  @param[in]      void
  
  
  @return
  void
  
  @SideEffect
  wake gpio is deasserted
  
*/
void pcie_enable_ltssm(void)
{
   pcie_core_enableltssm(pcie_dev);

   /* Wait till enumeration completes */
   while ( PCIE_LINKSTATUS_ENUMERATED != pcie_getlinkstate() ) ;
 
   /* deassert wake only after the enumeration is complete*/
   pcie_gpio_deassert_wake();	   

   pcie_osal_debug(MSG_HIGH,"Reestablish PCIe link");

}

#ifndef  PCIE_BOOT
/* ============================================================================
**  Function : pcie_read_perst_pin_value
** ============================================================================
*/
/**
  Reads and returns the current value on the perst pin input.
    
  @param[in]  None
  
  @return     pcie_perst_pin_value_type
  
  PCIE_PERST_PIN_HIGH - if perst pin is high
  PCIE_PERST_PIN_LOW  - if perst pin is low


  @return
  None.
  
*/
pcie_perst_pin_value_type pcie_read_perst_pin_value(void)
{
   DALGpioValueType  perst_pin ;
   DALResult dRes;
   
   pcie_osal_debug(MSG_HIGH,"Begin");

   dRes = DalTlmm_GpioIn(pcie_gpio_perst.h_pcie_tlmm,pcie_gpio_perst.pcie_gpio_pin,&perst_pin);
   if(dRes != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioIn failed");
   }

   if (perst_pin == DAL_GPIO_HIGH_VALUE) 
   {
      pcie_osal_debug(MSG_HIGH,"PERST pin high");
      return PCIE_PERST_PIN_HIGH;
   }
   else 
   {
      pcie_osal_debug(MSG_HIGH,"PERST pin low");
      return PCIE_PERST_PIN_LOW;
   }
}
#endif

/* ============================================================================
**  Function : pcie_enable_endpoint
** ============================================================================
*/
/**
  Handle PCIe link reestablishment.
    
  @param[in,out]  None

  @return
  None.
  
*/
void pcie_enable_endpoint(void)
{
#ifndef  PCIE_BOOT
   DALGpioValueType  perst_pin ;
   DALResult dRes;
   
   pcie_osal_debug(MSG_HIGH,"Begin");

   dRes = DalTlmm_GpioIn(pcie_gpio_perst.h_pcie_tlmm,pcie_gpio_perst.pcie_gpio_pin,&perst_pin);
   if(dRes != DAL_SUCCESS)
   {
      pcie_osal_debug(MSG_FATAL, "DalTlmm_GpioIn failed");
   }

   if (perst_pin != DAL_GPIO_HIGH_VALUE) 
   {
      pcie_osal_debug(MSG_HIGH,"PERST pin LOW. Exit without enabling EP");
      return ;
   }

   pcie_osal_syncenter(&pcie_pmic_npa_sync) ;

   /* Vote for PCIE_ACTIVE_SVS mode */
   pcie_core_pmic_npa_mode_request(PCIE_ACTIVE_SVS);

   pcie_osal_syncleave(&pcie_pmic_npa_sync) ;
   
   /*This is D3 cold exit . Turn on PCIe HW clock*/
   pcie_core_enable_pwr_domain();

   /* Vote for CNOC SVS frequency */
   pcie_core_clk_npa_mode_request();
   pcie_core_enable_clk();

   pcie_osal_debug(MSG_HIGH,"Restoring TZ system calls");
   if (0 != scm_sec_restore_syscall(TZ_DEVICE_PCIE_20))
   {
      pcie_osal_debug(MSG_FATAL, "TZ security restore failed for TZ_DEVICE_PCIE_20");
   }   
   pcie_osal_debug(MSG_HIGH,"Restored TZ system calls");

   pcie_install_isrs();
      
   /* Initialize the PHY */
   pcie_phy_init(pcie_dev);       
   pcie_osal_select_extern_source(&pcie_pipe_clk_ctl,62500000,0, 2, 0, 0, 0); 
   pcie_config_core();


   /* The first BAR is to be routed to MHI */
   pcie_core_configmhigate(pcie_dev, &pcie_tgt_cfg->bar_config[0]);
   pcie_osal_memcpy_enable();
   pcie_send_ltr(PCIE_DEFAULT_LTR);
   pcie_osal_debug(MSG_HIGH,"Exit");
#endif
}

/* ============================================================================
**  Function : pcie_disable_endpoint
** ============================================================================
*/
/**
  Handle PCIe link down.
    
  @param[in,out]  None

  @return
  None.
  
*/
void pcie_disable_endpoint(void)
{
#ifndef PCIE_BOOT
  
   pcie_osal_debug(MSG_HIGH,"Begin");

   /*Disable PCIe EP HW*/
   pcie_core_disable_clk();
   pcie_uninstall_isrs();
   pcie_core_disable_pwr_domain();
   
   pcie_osal_syncenter(&pcie_pmic_npa_sync) ;

   /* Vote for PCIE_OFF mode */
   pcie_core_pmic_npa_mode_request(PCIE_OFF);
   pcie_osal_memcpy_disable();
   pcie_osal_syncleave(&pcie_pmic_npa_sync) ;
   
   /* Remove vote for CNOC frequency */
   pcie_core_clk_npa_mode_release();
   pcie_osal_debug(MSG_HIGH,"Exit");
#endif
}

/* ============================================================================
**  Function : pcie_get_link_state
** ============================================================================
*/
/**
  Return current PCIe link state.
 
  
  
  @param[in]      void
  
  
  @return
  pcie_link_state_type
*/
pcie_link_state_type    pcie_get_link_state(void) 
{
   uint8 reg_fld_val ;
   uint32 reg_val ;

   reg_fld_val = pcie_read_reg_field(pcie_dev->elbi_base,PCIE_FLD(PCIE20_ELBI_SYS_STTS,XMLH_LINK_UP));
   if (0x0 == reg_fld_val) 
   {
      return PCIE_LINK_STATE_LINK_DOWN ;
   }

   reg_fld_val = pcie_read_reg_field(pcie_dev->elbi_base,PCIE_FLD(PCIE20_ELBI_SYS_STTS,XMLH_LTSSM_STATE)); 
   if (0x11 == reg_fld_val) 
   {
      return PCIE_LINK_STATE_L0 ;
   }
   else if (0x12 == reg_fld_val) 
   {
      return PCIE_LINK_STATE_L0S ;
   }

   reg_val = pcie_read_reg(pcie_dev->parf_base,PCIE20_PARF_PM_STTS);
   if (0x0 != (reg_val & PCIE20_PARF_PM_STTS_PM_LINKST_IN_L0S_MASK) ) 
   {
      return PCIE_LINK_STATE_L0S ;
   }

   if (0x0 != (reg_val & PCIE20_PARF_PM_STTS_PM_LINKST_IN_L1SUB_MASK) ) 
   {
      return PCIE_LINK_STATE_L1SS ;
   }

   if (0x0 != (reg_val & PCIE20_PARF_PM_STTS_PM_LINKST_IN_L1_MASK) ) 
   {
      return PCIE_LINK_STATE_L1 ;
   }
 
   if (0x0 != (reg_val & PCIE20_PARF_PM_STTS_PM_LINKST_IN_L2_MASK) ) 
   {
      return PCIE_LINK_STATE_L2 ;
   }

   return PCIE_LINK_STATE_OTHERS ;
}



/* ============================================================================
**  Function : pcie_change_device_state
** ============================================================================
*/
/**
  Change device state from EP driver side. Used only for internal testing purpose.
 
  
  
  @param[in]      device state( o for D0 state, 3 for D3 state)
  
  
  @return
  None
*/
void pcie_change_device_state(uint8 d_state)
{
   pcie_write_reg_field(pcie_dev->dbi_base,PCIE_FLD(TYPE0_CON_STATUS_REG_1,POWER_STATE),d_state);
   pcie_osal_debug(MSG_HIGH,"Change Device state to : %d",d_state,0,0);
}

/* ============================================================================
**  Function : pcie_send_ltr
** ============================================================================
*/
/**
  Request to exit L1 link state.
 
  
  @param[in]      ltr_us : LTR value in usecs (max value is ~ 30 seconds)
  
  @return
  None
*/
void pcie_send_ltr(uint32 ltr_us)
{
    uint32 ltr_value,ltr_msg_val;
    uint32 nosnoop_ltr, snoop_ltr;
	uint32 ltr_enabled;

    pcie_osal_debug(MSG_HIGH,"Begin");
	ltr_enabled =  pcie_read_reg_field(pcie_dev->dbi_base,PCIE_FLD(DEVICE_CONTROL2_DEVICE_STATUS2_REG,PCIE_CAP_LTR_EN));
    if(!ltr_enabled)
    {
        pcie_osal_debug(MSG_HIGH,"LTR bit not enabled");
        return;
    }
    if(ltr_us)
    {

        ltr_value = ((ltr_us * 1000) >> 15); /* Need the value to be a unit of 32768 ns */
        if((ltr_value & ~0x3FF) != 0)
        {
            pcie_osal_debug(MSG_HIGH,"LTR usec was too high to fit in a message : %d",ltr_us);
            return;
        }
                                   /* Req    | Scale (3) | Value  */
        nosnoop_ltr = snoop_ltr = ((1 << 15) | (3 << 10) | (ltr_value & 0x3FF));
        ltr_msg_val = ((nosnoop_ltr << 16)|snoop_ltr);
    }
    else
    {
        /* Disable LTR message */
        ltr_msg_val = 0x0;
    }

    /* Make sure we are out of L1ss */
    pcie_core_req_exit_l1();
    pcie_write_reg(pcie_dev->parf_base,PCIE20_PARF_LTR_MSG_GEN,ltr_msg_val);
    pcie_core_req_enter_l1();
    pcie_osal_debug(MSG_HIGH,"LTR Msg = 0x%X ; LTR usec = %d usecs",ltr_msg_val, ltr_us);
}

/* ============================================================================
**  Function : pcie_control_l1ss_inact_int
** ============================================================================
*/
/**
  API to enable or disable L1SS inactivity timer interrupt.
 
  
  
  @param[in]      ctrl :       True to enable interrut,False to disable interrupt.
                  maxtimer:    Max time out value
  
 
  
  
  @return
  None
*/
void pcie_control_l1ss_inact_interrupt(boolean ctrl,uint32 maxtimer)
{
   if (ctrl) 
   {
      pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_DEBUG_INT_EN,L1SUB_TIMEOUT_BIT),0X1);
      pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER,CNT_MAX),maxtimer);
   }
   else
   {
      pcie_write_reg_field(pcie_dev->parf_base,PCIE_FLD(PCIE20_PARF_DEBUG_INT_EN,L1SUB_TIMEOUT_BIT),0X0);
   }
   pcie_osal_ctrl_interrupt(PCIE_L1_INACT_TIMEOUT_IRQ,ctrl) ;
}

/* ============================================================================
**  Function : pcie_set_host2dev_mmap
** ============================================================================
*/
/**
Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.

 
 
@param[in]      atu_array      Array of ATU configuration 
@param[in]      entry_count     atu_array size 
 


@return
Status of the operation
*/
int16 pcie_set_host2dev_mmap(pcie_iatu_entry_type atu_array[],uint8 entry_count)
{
   uint32 temp;
   pcie_address_type host_ctrl_addr;
   pcie_address_type *local_ctrl_addr;
   uint32 ctrl_size;
   pcie_address_type host_data_addr ;
   pcie_address_type *local_data_addr;
   uint32 data_size;

   if ((NULL == atu_array) || (entry_count > 2)) 
   {
      pcie_osal_debug(MSG_FATAL, "atu_array is NULL or entry_count = %d > 2",entry_count);
      return -1;
   }

   host_ctrl_addr = atu_array[0].host_addr ;
   local_ctrl_addr = atu_array[0].local_addr ;
   ctrl_size = atu_array[0].size ;

   host_data_addr = atu_array[1].host_addr ;
   local_data_addr = atu_array[1].local_addr;
   data_size = atu_array[1].size ;

   pcie_atu_debug_info.host_ctrl = host_ctrl_addr ;
   pcie_atu_debug_info.ctrl_size = ctrl_size ;
   pcie_atu_debug_info.host_data = host_data_addr ;
   pcie_atu_debug_info.data_size = data_size ;

   pcie_atu_debug_info.local_ctrl = local_ctrl_addr ;
   pcie_atu_debug_info.local_data = local_data_addr ;
   /*Disable ATU index 1 and 2 before configure ATU to make sure previous configuration is cleaned up*/
   pcie_remove_index_obatu(1);
   pcie_remove_index_obatu(2);

   if (host_ctrl_addr.pa_hi == host_data_addr.pa_hi)
   {
      /*There is no overlapping between Control and data region. */
      if ( ((host_ctrl_addr.pa_low < host_data_addr.pa_low) && (ctrl_size <= (host_data_addr.pa_low - host_ctrl_addr.pa_low))) ||
           ((host_ctrl_addr.pa_low > host_data_addr.pa_low) && (data_size <= (host_ctrl_addr.pa_low - host_data_addr.pa_low)))
           ) 
      {
         pcie_add_index_obatu(1,host_ctrl_addr,local_ctrl_addr,ctrl_size);
         pcie_add_index_obatu(2,host_data_addr,local_data_addr,data_size);
         return 0 ;
      }
      /*Control region is inclusive in Data region. only ATU index 2 is needed.*/
      else if ( (host_ctrl_addr.pa_low >= host_data_addr.pa_low)  &&
           ((host_ctrl_addr.pa_low + ctrl_size) <= (host_data_addr.pa_low + data_size))
           ) 
      {
         pcie_add_index_obatu(2,host_data_addr,local_data_addr,data_size);
         local_ctrl_addr->pa_hi = 0 ;
         local_ctrl_addr->va_hi = 0 ;
         local_ctrl_addr->pa_low = local_data_addr->pa_low + host_ctrl_addr.pa_low - host_data_addr.pa_low ;
         local_ctrl_addr->va_low = local_data_addr->va_low + host_ctrl_addr.pa_low - host_data_addr.pa_low ;
         return 0 ;
      }
      /*Data region start address is within Control region . Consildate the overlapped region */
      else if ((host_data_addr.pa_low > host_ctrl_addr.pa_low) &&
          (host_data_addr.pa_low < (host_ctrl_addr.pa_low + ctrl_size))
          ) 
      {
         pcie_add_index_obatu(1,host_ctrl_addr,local_ctrl_addr,(host_data_addr.pa_low - host_ctrl_addr.pa_low));
         pcie_add_index_obatu(2,host_data_addr,local_data_addr,data_size);
         return 0 ;
      }
      /*Control region start address is within data region . Consildate the overlapped region */
      else if ( (host_ctrl_addr.pa_low > host_data_addr.pa_low) &&
           (host_ctrl_addr.pa_low < (host_data_addr.pa_low + data_size))
           ) 
      {
         pcie_add_index_obatu(1,host_ctrl_addr,local_ctrl_addr,ctrl_size);
         pcie_add_index_obatu(2,host_data_addr,local_data_addr,(host_ctrl_addr.pa_low - host_data_addr.pa_low ));
         return 0 ;
      }
      else
      {
         /*Conditions not handled Error fatal*/
         pcie_osal_debug(MSG_FATAL, "Unhandled conditions  host_ctrl_addr.pa_low=0x%x  host_ctrl_addr.pa_hi=0x%x  host_data_addr.pa_low=0x%x  host_data_addr.pa_hi=0x%x", host_ctrl_addr.pa_low, host_ctrl_addr.pa_hi, host_data_addr.pa_low, host_data_addr.pa_hi);
         return -1 ;
      }
  
   }
   else if( (host_ctrl_addr.pa_hi == 0) && (host_data_addr.pa_hi != 0) ) 
   {
      /*There is no overlapping between Control and data region. */
      if ( ((0xFFFFFFFF - host_ctrl_addr.pa_low + 1) >= ctrl_size) ||
           (host_data_addr.pa_low >= (ctrl_size - (0XFFFFFFFF - host_ctrl_addr.pa_low +1))) 
         )
      {
         pcie_add_index_obatu(1,host_ctrl_addr,local_ctrl_addr,ctrl_size);
         pcie_add_index_obatu(2,host_data_addr,local_data_addr,data_size);
         return 0 ;
      }
      /*Data region start address is within Control region . Consildate the overlapped region */
      else 
      {
         temp = 0XFFFFFFFF - host_ctrl_addr.pa_low +1 + host_data_addr.pa_low ;
         pcie_add_index_obatu(1,host_ctrl_addr,local_ctrl_addr,temp);
         pcie_add_index_obatu(2,host_data_addr,local_data_addr,data_size);
         return 0 ;
      }

   }
   else if( (host_ctrl_addr.pa_hi != 0) && (host_data_addr.pa_hi == 0) ) 
   {
      /*There is no overlapping between Control and data region. */
      if ( ((0xFFFFFFFF - host_data_addr.pa_low + 1) >= data_size) ||
           (host_ctrl_addr.pa_low >= (data_size - (0XFFFFFFFF - host_data_addr.pa_low +1))) 
         )
      {
         pcie_add_index_obatu(1,host_ctrl_addr,local_ctrl_addr,ctrl_size);
         pcie_add_index_obatu(2,host_data_addr,local_data_addr,data_size);
         return 0 ;
      }
      /*Data region start address is within Control region . Consildate the overlapped region */
      else 
      {
         temp = 0XFFFFFFFF - host_data_addr.pa_low +1 + host_ctrl_addr.pa_low ;
         pcie_add_index_obatu(1,host_ctrl_addr,local_ctrl_addr,ctrl_size);
         pcie_add_index_obatu(2,host_data_addr,local_data_addr,temp);
         return 0 ;
      }

   }
   else
   {
         /*Conditions not handled Error fatal*/
         pcie_osal_debug(MSG_FATAL, "Unhandled conditions host_ctrl_addr.pa_low=0x%x  host_ctrl_addr.pa_hi=0x%x  host_data_addr.pa_low=0x%x  host_data_addr.pa_hi=0x%x", host_ctrl_addr.pa_low, host_ctrl_addr.pa_hi, host_data_addr.pa_low, host_data_addr.pa_hi);
         return -1 ;
   }

}

/* ============================================================================
**  Function : pcie_pm_enter_lpm
** ============================================================================
*/
/**
  request PCIe  to enter low power mode
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_pm_enter_lpm(void)
{
#ifndef PCIE_BOOT
   uint32 pm_stts_1;
   uint32 pm_stts;

   pm_stts_1 = pcie_read_reg(pcie_dev->parf_base, PCIE20_PARF_PM_STTS_1);

   /* Assert if we are not in L1ss */
   if(0 == (pm_stts_1&(PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK)) )
   {
	  pm_stts = pcie_read_reg(pcie_dev->parf_base, PCIE20_PARF_PM_STTS); 
	  
	  pcie_osal_debug(MSG_FATAL, "Link is in l0 with pm_stts = 0x%x pm_stts_1= 0x%x",pm_stts, pm_stts_1); 
   }

   pcie_core_enter_lpm();

   pcie_osal_memcpy_disable();
   pcie_osal_debug(MSG_HIGH,"Disabled BAMDMA");

   pcie_osal_syncenter(&pcie_pmic_npa_sync) ;

   /* Vote for PCIE_OFF mode */
   pcie_core_pmic_npa_mode_request(PCIE_OFF);

   pcie_osal_syncleave(&pcie_pmic_npa_sync) ;

   /* Remove vote for CNOC frequency */
   pcie_core_clk_npa_mode_release();
#endif

}

/* ============================================================================
**  Function : pcie_pm_exit_lpm
** ============================================================================
*/
/**
  request PCIe  to exit low power mode
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_pm_exit_lpm(void)
{
#ifndef PCIE_BOOT
   pcie_osal_syncenter(&pcie_pmic_npa_sync) ;

   /* Vote for PCIE_ACTIVE_SVS mode */
   pcie_core_pmic_npa_mode_request(PCIE_ACTIVE_SVS);

   pcie_osal_syncleave(&pcie_pmic_npa_sync) ;
   
   /* Vote for CNOC SVS frequency */
   pcie_core_clk_npa_mode_request();
   
   pcie_core_exit_lpm();
   pcie_osal_memcpy_enable();
   pcie_osal_debug(MSG_HIGH,"Enabled BAMDMA");
#endif
}

/* ============================================================================
**  Function : pcie_flush
** ============================================================================
*/
/**
  Flush any posted writes 
  
  
  @param[in]  void
  
  
  @return
  None.  
*/
void pcie_flush(void)
{
  pcie_read_va((char*)&scratch_read_buf, pcie_dev->dbi_base, 4, NULL);

  return;
}

/* ============================================================================
**  Function : pcie_register_dma_cb
** ============================================================================
*/
/**
  Register callbacks for DMA 
  
  
  @param[in]      read_cb      Read callback
  @param[in]      write_cb     Write callback
  
  
  @return
  None.  
*/
void pcie_register_dma_cb(void *read_cb, void *write_cb)
{
#ifndef PCIE_BOOT
  async_rw_req_cb=(dma_req_cb)read_cb;
#else
  dma_read_req_cb_func = (dma_req_cb)read_cb ;
  dma_write_req_cb_func = (dma_req_cb)write_cb ;
#endif
}

#ifdef PCIE_BOOT
/* ============================================================================
**  Function : pcie_read_dma
** ============================================================================
*/
/**
Read  data from the host memory through IPA DMA.


@param[in]     dma_req   PCIe  DMA request .


@return
0 - Successfully .
-1 - An error code .

*/

int pcie_read_dma(dma_read_write_req_type dma_req)
{
   bam_status_type bam_status;

   if ((NULL == pcie_rxp_bam_handle) || (NULL == pcie_rxa_bam_handle) ) 
   {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "pcie_rxp_bam_handle or pcie_rxa_bam_handle is null");
      return -1 ;
   }

   bam_status = bam_pipe_bulktransfer(pcie_rxp_bam_handle,dma_req.src_addr,dma_req.size,BAM_IOVEC_FLAG_EOT,(void *)dma_req.user_data);
   
   if (bam_status != BAM_SUCCESS)
   {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "bam_pipe_bulktransfer failed");
      return -1;
    }

    bam_status = bam_pipe_bulktransfer(pcie_rxa_bam_handle,dma_req.dest_addr,dma_req.size,BAM_IOVEC_FLAG_EOT,(void *)dma_req.user_data);
    if (bam_status != BAM_SUCCESS)
    {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
       pcie_osal_debug(MSG_FATAL, "bam_pipe_bulktransfer failed");
       return -1;
     }

    return 0 ;
}

/* ============================================================================
**  Function : pcie_dma_poll
** ============================================================================
*/
/**
Poll for dma request .


@param[in]     Void.


@return
0 - Successfully .
-1 - An error code .

*/
int pcie_dma_poll(void)
{
   bam_status_type bam_status;

   if ((NULL == pcie_rxp_bam_handle) || (NULL == pcie_rxa_bam_handle) ) 
   {
      //Error fatal here
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "pcie_rxp_bam_handle or pcie_rxa_bam_handle is null");
      return -1 ;
   }

   bam_status = bam_pipe_poll(pcie_rxa_bam_handle,NULL) ;

   if (bam_status != BAM_SUCCESS)
   {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
       pcie_osal_debug(MSG_FATAL, "bam_pipe_poll failed");
       return -1;
   }

   
   if ((NULL == pcie_txp_bam_handle) || (NULL == pcie_txa_bam_handle) ) 
   {
      //Error fatal here
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "pcie_txp_bam_handle or pcie_txa_bam_handle is null");
      return -1 ;
   }

   bam_status = bam_pipe_poll(pcie_txp_bam_handle,NULL) ;

   if (bam_status != BAM_SUCCESS)
   {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
       pcie_osal_debug(MSG_FATAL, "bam_pipe_poll failed");
       return -1;
   }

   return 0 ;
}


/* ============================================================================
**  Function : pcie_write_dma
** ============================================================================
*/
/**
Write data to the host memory through DMA.




@param[in]     dma_req   PCIe  DMA request .



@return
0 - Successfully .
-1 - An error code .

*/

int pcie_write_dma(dma_read_write_req_type dma_req)
{
   bam_status_type bam_status;

   if ((NULL == pcie_txp_bam_handle) || (NULL == pcie_txa_bam_handle) ) 
   {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "pcie_txp_bam_handle or pcie_txa_bam_handle is null");
      return -1 ;
   }

   bam_status = bam_pipe_bulktransfer(pcie_txa_bam_handle,dma_req.src_addr,dma_req.size,BAM_IOVEC_FLAG_EOT,(void *)dma_req.user_data);
   if (bam_status != BAM_SUCCESS)
   {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
      pcie_osal_debug(MSG_FATAL, "bam_pipe_bulktransfer failed");
      return -1;
    }

    bam_status = bam_pipe_bulktransfer(pcie_txp_bam_handle,dma_req.dest_addr,dma_req.size,BAM_IOVEC_FLAG_EOT,(void *)dma_req.user_data);
    if (bam_status != BAM_SUCCESS)
    {
      if(TRUE == pcie_bam_dump_alloc_success)
	  {
          bam_get_coredump( pcie_ipadma_bam_handle,  PCIE_BAM_DUMP_MASK , NULL); 
      }
       pcie_osal_debug(MSG_FATAL, "bam_pipe_bulktransfer failed");
       return -1;
     }

    return 0 ;


}
#endif
