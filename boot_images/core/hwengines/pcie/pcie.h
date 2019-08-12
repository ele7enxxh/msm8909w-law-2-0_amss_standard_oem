/** 
@file pcie.h
@brief
This header files defines the public interface to the PCIe Device Driver 

*/
/*
===============================================================================

Edit History

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/30/14   Dilip        Added pcie_is_pcie_fuse_disabled function
05/23/14   Dilip        Added timer value as a parameter in pcie_control_l1ss_inact_interrupt
05/20/14   Dilip        Removed pcie_set_l1_dot2_thresh
05/02/14   nk           pcie_flush added
04/08/14   Dilip        Removed pcie_req_exit_l1 and  pcie_req_enter_l1
03/19/14   Dilip        Created pcie_disable_endpoint
08/05/13   Weijunz      Created
===============================================================================
Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================
*/

#ifndef __PCIE_H__
#define __PCIE_H__

#include "comdef.h"

#define PCIE_L1SUB_AHB_CLK_MAX_TIMER_ZERO_VAL  0x0

/** @addtogroup pcie_enum_api
@{ */

/** Various status codes returned by PCIe APIs */
typedef enum _pcie_status_type
{
   PCIE_STATUS_SUCCESS               = 0x0, /**< -- Operation succeeded */
   PCIE_STATUS_ERROR                 = 0x1, /**< -- Operation met an error */
   PCIE_STATUS_INVALIDPARAMS         = 0x2, /**< -- Invalid paramaters were provided */
   PCIE_STATUS_MSIDISABLED           = 0x3, /**< -- MSI is not enabled by the host   */
   PCIE_STATUS_LINKTIMEOUT           = 0x4, /**< -- Link Training timed out          */
   PCIE_STATUS_UNEXPLINKDOWN         = 0x5, /**< -- Unexpected link down */ 
   PCIE_BOOT_DISABLED                = 0x6, /**< -- PCie boot disabled */ 
   PCIE_FUSE_DISABLED                = 0x7, /**< -- PCie fuse disabled */
   PCIE_TRANSFER_REQUESTS_FULL       = 0x8, /**< -- PCIe cannot accomodate more transfer requests */
   /* PHY Error Codes */
   PCIE_STATUS_PHYINITFAILED         = 0x700, /**< -- PHY initialization failed */
   /* Misc Error Codes */
   PCIE_STATUS_CLKCFGFAILED          = 0x800   /**< -- Clock Configuration failed */
}pcie_status_type;

/** Different Device types supported by a PCIe Core */
typedef enum _pcie_devmode_type
{
   PCIE_DEV_EP = 0, /**< -- Device is an Endpoint */
   PCIE_DEV_RC = 1  /**< -- Device is a Root Complex */
}pcie_devmode_type;

/** Different states of a PCIe link */
typedef enum _pcie_linkstatus_type
{
   PCIE_LINKSTATUS_DOWN       = 0, /**< -- Link is down */
   PCIE_LINKSTATUS_UP         = 1, /**< -- Link is Up */
   PCIE_LINKSTATUS_ENUMERATED = 2 /**< -- Link is enumerated and allocated an 
                                  MMIO space by the RC. Implies that 
                                  the link is up */
}pcie_linkstatus_type;

/** Direction of the traffic for Address Translation Unit (ATU( */
typedef enum _pcie_atudir_type
{
   PCIE_ATUDIR_OUTBOUND = 0, /**< -- Outbound ATU  */
   PCIE_ATUDIR_INBOUND  = 1  /**< -- Inbound ATU  */
}pcie_atudir_type;

/** PERST pin value */
typedef enum _pcie_perst_pin_value_type
{
   PCIE_PERST_PIN_LOW  = 0,    /**< -- PERST Pin value is low */
   PCIE_PERST_PIN_HIGH = 1     /**< -- PERST Pin value is high */
}pcie_perst_pin_value_type;

/** ATU Modes */
typedef enum _pcie_atumode_type
{
   PCIE_ATUMODE_ADDRESSMATCHED = 0, /**< -- Performs matches based on address range */
   PCIE_ATUMODE_BARMATCHED = 1     /**< -- Performs matches based on bar range */
}pcie_atumode_type;

/** PCIe Link Speeds */
typedef enum
{
   PCIE_CORE_TARGETSPEED_DEFAULT=0,/**< Core's default speed */
   PCIE_CORE_TARGETSPEED_GEN1 = 1, /**< Gen1 Speed - 2.5GT/s */
   PCIE_CORE_TARGETSPEED_GEN2 = 2, /**< Gen2 Speed - 5.0GT/s */
   PCIE_CORE_TARGETSPEED_GEN3 = 4 /**< Gen3 Speed - 8.0GT/s */
} pcie_targetspeed_type;

/** @}  - pcie_enum_api */

/** @addtogroup pcie_struct_api
@{ */

/** PCIe Address */
typedef struct _pcie_address_type
{
   uint32 va_hi;    /**< -- Upper 32 bits of the Virtual Address */
   uint32 va_low;   /**< -- Lower 32 bits of the Virtual Address */
   uint32 pa_hi;    /**< -- Upper 32 bits of the Physical Address */ 
   uint32 pa_low;   /**< -- Lower 32 bits of the Physical Address */ 
}pcie_address_type;
/** @}  - pcie_struct_api */

/** MSI Config structure */
typedef struct _pcie_msiconfig_type
{
    uint32 enabled;        /**< -- Flag to indicate whether MSI is enabled or not */
    uint32 addr_low;       /**< -- Lower 32 bits of the MSI Address */
    uint32 addr_hi;        /**< -- Upper 32 bits of the MSI Address */
    uint32 data;           /**< -- Data Pattern to use when generating the MSI */ 
    pcie_address_type local_addr; /**< -- Local address to access the MSI address */
    uint32 num_multi_msgs; /**<- Number of messages assigned by the host */
}pcie_msiconfig_type;

typedef struct
{
   uint32 src_addr ;
   uint32 dest_addr;
   uint16 size ;
   void *user_data;
   boolean result ;

}dma_read_write_req_type ;

typedef struct pcie_gpio_perst_cb_param
{
   uint8 expected_perst ;

}pcie_gpio_perst_cb_param_type;

typedef enum pcie_to_mhi_pm_n
{
   PCIE_PM_DEVICE_STATE_IN_D0 ,
   PCIE_PM_DEVICE_STATE_IN_D3_HOT ,
   PCIE_PM_DEVICE_STATE_IN_D3_COLD ,
   PCIE_PM_L1_INACT_TIMEOUT ,
   PCIE_PM_PERST_DEASSERTED ,
   PCIE_LINK_UP,
   PCIE_LINK_DOWN,
   PCIE_TO_MHI_PM_MAX
}pcie_to_mhi_pm_n_type;

typedef enum pcie_link_state
{
   PCIE_LINK_STATE_LINK_DOWN,
   PCIE_LINK_STATE_L0,
   PCIE_LINK_STATE_L0S,
   PCIE_LINK_STATE_L1,
   PCIE_LINK_STATE_L1SS,
   PCIE_LINK_STATE_L2,
   PCIE_LINK_STATE_OTHERS

}pcie_link_state_type;

typedef struct
{
   pcie_address_type host_ctrl ;
   pcie_address_type* local_ctrl ;
   uint32 ctrl_size ;
   pcie_address_type host_data ;
   pcie_address_type* local_data ;
   uint32 data_size ;

}pcie_iatu_debug_type ;

typedef struct
{
   pcie_address_type host_addr ;
   pcie_address_type* local_addr ;
   uint32 size ;
   
}pcie_iatu_entry_type ;

typedef void (*pcie_pm_cb_type)(pcie_to_mhi_pm_n_type pm_notify,void *ctx) ;

typedef void (*dma_req_cb)(dma_read_write_req_type dma_req) ;


/** @addtogroup pcie_func_api
@{ */

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

void pcie_reg_dump(void *dump_addr, uint32 max_size);

/* ============================================================================
**  Function : pcie_init
** ============================================================================
*/
/**
  Initializes the PCIe Link.
  
  Initializes the PCIe MAC and the PHY layer and starts link enumeration.
  
  
  @return
  returns the Link Status after initialization.
*/
pcie_status_type pcie_init( void );

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
pcie_linkstatus_type pcie_getlinkstate( void );



/* MSI */
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
pcie_status_type pcie_sendmsi(uint32 msi_vector);

/* ============================================================================
**  Function : pcie_add_index_obatu
** ============================================================================
*/
/**
Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.

Configures the iATU in the PCIe for outbound (EP <-> Host Memory) traffic.
 
@param[in]      index     Index to be added in IATU. 
@param[in]      host_addr     Address of the host memory to be access enabled
@param[in]  local_addr    Address relevant in the endpoint memory space
@param[in]      size          size of the region

@return
Status of the operation
*/
int16  pcie_add_index_obatu(uint8 index ,pcie_address_type host_addr, pcie_address_type *local_addr, uint32 size);

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
uint8  pcie_remove_index_obatu(uint8 index );

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

uint32 pcie_read_pa(void* buf, uint32 pcie_axi_addr, uint32 size, void* user_data);

/* ============================================================================
**  Function : pcie_read_va
** ============================================================================
*/
/**
Reads data from the host memory.

Reads data from the host memory via PCIe link. 


@param[in]  buf             Buffer to read into 
@param[in]     pcie_axi_addr    PCIe AXI address in modem address space
@param[in]      size        Size of the data to be read in bytes 
@param[in]      user data         user data for this transaction


@return
PCIE_STATUS_SUCCESS - Successfully .
!PCIE_STATUS_SUCCESS - An error code .

*/

uint32 pcie_read_va(void* buf, uint32 pcie_axi_addr, uint32 size, void* user_data);


/* ============================================================================
**  Function : pcie_write_pa
** ============================================================================
*/
/**
Writes data to the host memory using a physical buffer

Writes data to the host memory via PCIe link. 


@param[in]  buf             Buffer that contains data to be written  into host
@param[in]     pcie_axi_addr    PCIe AXI virtual address in modem address space
@param[in]      size        Size of the data to be written in bytes
@param[in]      user data         user data for this tranaction

@return
PCIE_STATUS_SUCCESS - Successfully .
!PCIE_STATUS_SUCCESS - An error code .

*/

uint32 pcie_write_pa(void* buf, uint32 pcie_axi_addr, uint32 size,void* user_data);

/* ============================================================================
**  Function : pcie_write_va
** ============================================================================
*/
/**
Writes data to the host memory.

Writes data to the host memory via PCIe link. 


@param[in]  buf             Buffer that contains data to be written  into host
@param[in]     pcie_axi_addr    PCIe AXI virtual address in modem address space
@param[in]      size        Size of the data to be written in bytes
@param[in]      user data         user data for this transaction

@return
PCIE_STATUS_SUCCESS - Successfully .
!PCIE_STATUS_SUCCESS - An error code .

*/

uint32 pcie_write_va(void* buf, uint32 pcie_axi_addr, uint32 size,void* user_data);

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
void pcie_get_msi_config(pcie_msiconfig_type* pcie_msi_info);

/* ============================================================================
**  Function : pcie_is_pcie_boot_enabled
** ============================================================================
*/
/**
return True if PCIe boot is enabled. 
   


@return
 Boolean

*/

boolean  pcie_is_pcie_boot_enabled(void);

/* ============================================================================
**  Function : pcie_is_pcie_fuse_disabled
** ============================================================================
*/
/**
return True if PCIe fuse is disabled. 
     


@return
 Boolean - True if fuse is disabled else returns false

*/

boolean  pcie_is_pcie_fuse_disabled(void);

/* ============================================================================
**  Function : pcie_mhi_register_pm_cb
** ============================================================================
*/
/**
  MHI register power management call back with PCIe driver.
  
  
  @param[in]      host_addr     Address of the host memory to be access enabled.
  @param[in]      param         pointer to parameters provided by client. 
  
  @return
  void
*/
void pcie_mhi_register_pm_cb(pcie_pm_cb_type mhi_pm_cb,void *param);

/* ============================================================================
**  Function : pcie_wakeup_host
** ============================================================================
*/
/**
  Assert GPIO siganl WAKE# to wakeup host.
 
  
  
  @param[in]      void
  
  
  @return
  void
*/
void pcie_wakeup_host(void);

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
pcie_perst_pin_value_type pcie_read_perst_pin_value(void);

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
void pcie_enable_endpoint(void);

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
void pcie_disable_endpoint(void);

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
pcie_link_state_type    pcie_get_link_state(void) ;


/* ============================================================================
**  Function : pcie_gpio_trigger_perst_int
** ============================================================================
*/
/**
  Trigger PERST GPIO interrupt.. Used for internal testing purpose only.
 
  
  
  @param[in]      None.
  
  
  @return
  None
*/
void pcie_gpio_trigger_perst_int(void);

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
void pcie_change_device_state(uint8 d_state);

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
void pcie_send_ltr(uint32 ltr_us);

/* ============================================================================
**  Function : pcie_control_l1ss_inact_interrupt
** ============================================================================
*/
/**
  API to enable or disable L1SS inactivity timer interrupt.
 
  
  
  @param[in]      ctrl :       True to enable interrut,False to disable interrupt.
                  maxtimer:    Max time out value
 
 
  
  
  @return
  None
*/
void pcie_control_l1ss_inact_interrupt(boolean ctrl,uint32 maxtimer);

/* ============================================================================
**  Function : pcie_set_mhi_ipa_dbs
** ============================================================================
*/
/**
Set register PCIE20_PARF_MHI_IPA_DBS register.



@param[in]     cdb_base    channel doorbell base. 
@param[in]     cdb_end     channel doorbell end. 
@param[in]     edb_base    event doorbell base. 
@param[in]     edb_end     event doorbell end.  



@return
None

*/
void pcie_set_mhi_ipa_dbs(uint8 cdb_base,uint8 cdb_end,uint8 edb_base,uint8 edb_end) ;

/* ============================================================================
**  Function : pcie_set_mhi_ipa_cdb_target
** ============================================================================
*/
/**
Set register PCIE20_PARF_MHI_IPA_CDB_TARGET_LOWER register.



@param[in]     mhi_ipa_cdb_addr    MHI IPA channel Doorbell Target address



@return
None

*/
void pcie_set_mhi_ipa_cdb_target(uint32 mhi_ipa_cdb_addr) ;

/* ============================================================================
**  Function : pcie_set_mhi_ipa_edb_target
** ============================================================================
*/
/**
Set register PCIE20_PARF_MHI_IPA_EDB_TARGET_LOWER register.



@param[in]     mhi_ipa_edb_addr   MHI IPA event Doorbell Target address



@return
None

*/

void pcie_set_mhi_ipa_edb_target(uint32 mhi_ipa_edb_addr) ;

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

int16 pcie_set_host2dev_mmap(pcie_iatu_entry_type atu_array[],uint8 entry_count);

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
void pcie_pm_enter_lpm(void);

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
void pcie_pm_exit_lpm(void);

/* ============================================================================
**  Function : pcie_enable_ltssm
** ============================================================================
*/
/**
  API to enable LTSSSM.
 
  
  
  @param[in]      void


  @return
  void
*/
void pcie_enable_ltssm(void);

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
void pcie_flush(void);

/* ============================================================================
**  Function : pcie_register_cb_for_dma
** ============================================================================
*/
/**
  Register callbacks for DMA 
  
  
  @param[in]      read_cb      Read callback
  @param[in]      write_cb     Write callback
  
  
  @return
  None.  
*/
void pcie_register_dma_cb(void *read_cb, void *write_cb);

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
int pcie_dma_poll(void);

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

int pcie_read_dma(dma_read_write_req_type dma_req);

/* ============================================================================
**  Function : pcie_write_dma
** ============================================================================
*/
/**
Write data to the host memory through  DMA.




@param[in]     dma_req    PCIe  DMA request .



@return
0 - Successfully .
-1 - An error code .

*/

int pcie_write_dma(dma_read_write_req_type dma_req);

void sbl_read_test_func(void);


/** @}  - pcie_func_api */

#endif /* __PCIE_H__ */

