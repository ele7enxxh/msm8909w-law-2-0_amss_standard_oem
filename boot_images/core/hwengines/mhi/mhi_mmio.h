#ifndef MHI_MMIO_H
#define MHI_MMIO_H
/**
  @file mhi_mmio.h
  @brief
  This file contains the MMIO register space initialization, configuration, and 
  access header API's. 
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/11/nk   nk      Add Interrupt Moderation support
10/02/14   am      Added changes for MHI spec Rev D
06/25/13   nk      Added changes for latest MHI spec
02/20/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

 /* *****************************************************
 * MHI SPEC DATA Structres                            *
 * ****************************************************/

#ifndef PACKED
#define PACKED __packed  
#endif 

#include "mhi.h" 
#include "mhi_i.h"

/** MHI Channel States */ 
typedef enum _mhi_ch_state
{
  MHI_CH_STATE_DISABLED=0x0,        /**< Channel is disabled. This is the reset state */
  MHI_CH_STATE_ENABLED=0x1,         /**< Channel is enabled. Doorbells can be processed */ 
  MHI_CH_STATE_RUNNING=0x2,         /**< Channel is running. */ 
  MHI_CH_STATE_SUSPENDED=0x3,       /**< Channel is suspended */ 
  MHI_CH_STATE_STOPPED=0x4,         /**< Channel is stopped using channel stop command */
  MHI_CH_STATE_ERROR=0x5,           /**< Channel error has occurred */ 
  MHI_CH_STATE_IS32=0x7FFFFFFF      /**< Force enum size to 32-bit */ 
}mhi_ch_state_t; 

/** MHI Channel Type */ 
typedef enum _mhi_ch_type
{
   MHI_CH_TYPE_INVALID=0x0,         /**< Channel is not valid */ 
   MHI_CH_TYPE_OUTBOUND=0x1,        /**< Outbound channel (host to device) */
   MHI_CH_TYPE_INBOUND=0x2,         /**< Inbound channel (device to host) */
   MHI_CH_TYPE_IS32=0x7FFFFFFF      /**< Force enum size to 32-bit */ 
}mhi_ch_type_t; 

/** Channel Context */ 
typedef PACKED struct _mhi_ch_ctx 
{
   mhi_ch_state_t chstate;  /**< Channel state. See mhi_ch_state_t type */ 
   mhi_ch_type_t  chtype;   /**< Channel type. see mhi_ch_type_t type */ 
   uint32         erindex;  /**< Index number of event ring associated with channel */ 
   uint64         rbase;    /**< Ring address pointer of the ring associated with channel */ 
   uint64         rlen;     /**< Ring length in bytes. Must be a multiple of ring element size */ 
   uint64         rp;       /**< Read pointer to 64-bit address in host space */ 
   uint64         wp;       /**< Write pointer to 64-bit address in host space */ 
}mhi_ch_ctx_t;

/* Event Context */
typedef PACKED struct _mhi_ev_ctx
{
   uint32 res:16;          /**< Reserved */ 
   uint32 intmodt:16;      /**< Interrupt Moderation Timer */ 
   uint32 ertype;          /**< Event ring type. Identifies if the event ring context is valid */ 
   uint32 msivec;          /**< MSI Vector associated with the event ring */ 
   uint64 rbase;           /**< Base address pointer of the vent ring */ 
   uint64 rlen;            /**< Size of the ring (in bytes). Total size must be multiple of event ring element size */ 
   uint64 rp;              /**< Ring read pointer */ 
   uint64 wp;              /**< Ring write pointer */ 
}mhi_ev_ctx_t;

/** Command Ring Context */ 
typedef PACKED struct _mhi_cr_ctx
{
   uint32 res;     /**< Reserved */ 
   uint32 res1;    /**< Reserved */ 
   uint32 res2;    /**< Reserved */ 
   uint64 rbase;   /**< Base address of command ring */ 
   uint64 rlen;    /**< Size of the ring (in bytes). Total size must be multiple of command ring element size */ 
   uint64 rp;      /*<< Command ring read pointer */
   uint64 wp;      /*<< Command ring write pointer */
}mhi_cr_ctx_t; 

/* Generic Context */
typedef PACKED struct _mhi_generic_ctx
{
   uint32 res;             /**< Reserved */ 
   uint32 res1;            /**< Reserved */ 
   uint32 res2;            /**< Reserved */ 
   uint64 rbase;           /**< Base address pointer of the event ring */ 
   uint64 rlen;            /**< Size of the ring (in bytes). Total size must be multiple of event ring element size */ 
   uint64 rp;              /**< Ring read pointer */ 
   uint64 wp;              /**< Ring write pointer */
}mhi_generic_ctx_t; 

/* Union of possible contexts (CR, ER, and CH)  */
typedef PACKED union _mhi_ring_ctx
{
   mhi_cr_ctx_t        cmd;    /**< Command context */  
   mhi_ev_ctx_t         ev;    /**< Event Context */   
   mhi_ch_ctx_t         ch;    /**< Channel Context */ 
   mhi_generic_ctx_t   gen;    /**< Generic context */
}mhi_ring_ctx_t;

/** Ring Element Types */ 
typedef enum  _mhi_r_el_type
{
   MHI_RING_EL_TYPE_RESERVED=0,                /**< Reserved type */       
   MHI_RING_EL_TYPE_NOOP=1,                    /**< No op */ 
   MHI_RING_EL_TYPE_TRANSFER=2,                /**< Transfer */ 
   MHI_RING_EL_TYPE_RESET_CH_CMD=16,           /**< Reset Channel command */ 
   MHI_RING_EL_TYPE_STOP_CH_CMD=17,            /**< Stop channel command */ 
   MHI_RING_EL_TYPE_START_CH_CMD=18,           /**< Start channel command */ 
   MHI_RING_EL_TYPE_START_ALL_CHS_CMD=20,      /**< Start all channels command */ 
   MHI_RING_EL_TYPE_MHI_STATE_CHG=32,          /**< MHI state change notification */  
   MHI_RING_EL_TYPE_MHI_CMD_COMPLETE_EVENT=33, /**< Command completion event */ 
   MHI_RING_EL_TYPE_TRANSFER_EVENT=34,         /**< Transfer event */ 
   MHI_RING_EL_TYPE_EXEC_ENV_CHG=64,           /**< Execution environment state change notification */
}mhi_ring_el_t; 

/** Completion codes */
typedef enum _mhi_cmd_complete_code
{
   MHI_CMD_CODE_INVALID=0,                  /**< Undefined completion code */ 
   MHI_CMD_CODE_SUCCESS=1,                  /**< Successful completion of the operation */ 
   MHI_CMD_CODE_EOT=2,                      /**< Successful completion of the transfer operation, but less bytes were 
                                                   transferred from device to host than requested by TD */  
   MHI_CMD_CODE_OVERFLOW=3,                 /**< Overflow completion code indicates transfer unit could not be contained 
                                                 in a single TD. Applies only to inbound channels */ 
   MHI_CMD_CODE_EOB=4,                      /**< End of Block. Successful completion of operation and EOB condition encountered */
   MHI_CMD_CODE_UNDEFINED_ERR=16,           /**< Generic error code used by the device when other code does not apply */
   MHI_CMD_CODE_RING_EL_ERROR=17,           /**< Ring element parameter error condition */ 
}mhi_cmd_complete_code_t;

/** Ring elements **************************************/

/** Transfer Ring Element structure */
typedef PACKED struct _mhi_tr_element_t
{
    uint64 ptr;                            /**< Pointer to TRB */ 
    uint32 len:16;                         /**< Length of buffer */ 
    uint32 res:16;                         /**< RESERVED */ 
    uint32 chain:1;                        /**< Chain bit */  
    uint32 res1:7;                         /**< RESERVED */ 
    uint32 ieob:1;                         /**< Interrupt on end of block */ 
    uint32 ieot:1;                         /**< Interrupt on end of transfer */ 
    uint32 bei:1;                          /**< Block event interrupt */ 
    uint32 res2:5;                         /**< RESERVED */ 
    mhi_ring_el_t type:8;                  /**< Ring element type */ 
    uint32 res3:8;                         /**< RESERVED */ 
}mhi_tr_element_t; 

/** Noop command */
typedef PACKED struct _mhi_noop_cmd_element
{
    uint64 res;                            /**< RESERVED */ 
    uint32 res1;                           /**< RESERVED */ 
    uint32 res2:16;                        /**< RESERVED */ 
    mhi_ring_el_t type:8;                  /**< Ring element type */
    uint32 res3:8;                         /**< RESERVED */ 
}mhi_noop_cmd_element_t; 

/** Reset channel command */
typedef PACKED struct _mhi_reset_cmd_element
{
    uint64 res;                            /**< RESERVED */ 
    uint32 res1;                           /**< RESERVED */ 
    uint32 res2:16;                        /**< RESERVED */ 
    mhi_ring_el_t type:8;                  /**< Ring element type */
    uint32 chid:8;                         /**< Channel ID */ 
}mhi_reset_cmd_element_t;

/** Stop channel command */
typedef PACKED struct _mhi_stop_cmd_element
{
    uint64 res;                            /**< RESERVED */ 
    uint32 res1;                           /**< RESERVED */ 
    uint32 res2:16;                        /**< RESERVED */   
    mhi_ring_el_t type:8;                  /**< Ring element type */
    uint32 chid:8;                         /**< Channel ID */ 
}mhi_stop_cmd_element_t;

/** Transfer Completion Event */
typedef PACKED struct _mhi_transfer_event_element
{
   uint64 ptr;                             /**< Pointer to TRE (Transfer Ring Element ) */ 
   uint32 len:16;                          /**< Length of transfer */ 
   uint32 res:8;                           /**< RESERVED */ 
   mhi_cmd_complete_code_t code:8;         /**< Event code */ 
   uint32 res1:16;                         /**< RESERVED */ 
   mhi_ring_el_t type:8;                   /**< Ring element type */
   uint32 chid:8;                          /**< Channel ID */ 
}mhi_transfer_event_element_t; 

/** Command Completion Event  */
typedef PACKED struct _mhi_cmd_complete_event
{
   uint64 ptr;                            /**< Pointer to CRE (Command Ring Element ) */ 
   uint32 res:24;                         /**< RESERVED */ 
   mhi_cmd_complete_code_t code:8;        /**< Event code */  
   uint32 res1:16;                        /**< RESERVED */ 
   mhi_ring_el_t type:8;                  /**< Ring element type */
   uint32 res2:8;                         /**< RESERVED */ 
}mhi_cmd_complete_event_t; 

/** MHI State Change Event  */
typedef PACKED struct _mhi_state_chg_event
{
   uint64 res;                            /**< RESERVED */ 
   uint32 res1:24;                        /**< RESERVED */ 
   uint32 mhistate:8;                     /**< MHI state */
   uint32 res2:16;                        /**< RESERVED */ 
   mhi_ring_el_t type:8;                  /**< Ring element type */
   uint32 res3:8;                         /**< RESERVED */ 
}mhi_state_chg_event_t;

/** Execution Environment (EE) Change Event  */
typedef PACKED struct _mhi_exec_env_chg_event
{
   uint64 res;                            /**< RESERVED */ 
   uint32 res1:24;                        /**< RESERVED */ 
   uint32 exec_env:8;                     /**< Execution environment */
   uint32 res2:16;                        /**< RESERVED */ 
   mhi_ring_el_t type:8;                  /**< Ring element type */
   uint32 res3:8;                         /**< RESERVED */ 
}mhi_exec_env_chg_event_t;

/** Generic Ring Element structure. Used to determine ring type */
typedef PACKED struct _mhi_generic_element
{
    uint64 res;                           /**< RESERVED */ 
    uint32 res1;                          /**< RESERVED */ 
    uint32 res2:16;                       /**< RESERVED */ 
    mhi_ring_el_t type:8;                 /**< Ring element type */
    uint32 res3:8;                        /**< RESERVED */ 
}mhi_generic_element_t;

/** Union of possible ring elements */ 
typedef union _mhi_ring_element
{
    mhi_tr_element_t               tr;                /**< Transfer Ring element */  
    mhi_noop_cmd_element_t         cmdNoop;           /**< No-op command element */   
    mhi_reset_cmd_element_t        cmdReset;          /**< Reset command element */ 
    mhi_stop_cmd_element_t         cmdStop;           /**< Stop command element */ 
    mhi_transfer_event_element_t   tEvent;            /**< Transfer Completion event   */ 
    mhi_cmd_complete_event_t       cmdCompleteEvent;  /**< Command completion event */ 
    mhi_state_chg_event_t          stateChgEvent;     /**< State change event */
    mhi_exec_env_chg_event_t       exec_env_ChgEvent; /**< Execution Environment state change event */
    mhi_generic_element_t          rel;               /**< Generic ring element */ 
}mhi_ring_element_t; 

/** MHI Memory allocations */ 
typedef PACKED struct _mhi_host_addr
{
   uint64 mhictrlbase;  /**< Base address of memory region in host address space where control data structures reside */ 
   uint64 mhictrllimit; /**< Upper limit of ctrl address space */ 
   uint64 mhidatabase;  /**< Base address of region in host address space where MHI data buffers reside */ 
   uint64 mhidatalimit; /**< Upper limit of data region */ 
}mhi_host_addr_t; 

typedef enum  _mhictrl_int_status
{
   MHI_CTRL_CLEAR=0x0,                              /**< MHI Control Clear */
   MHI_CTRL_INT=0x1,                                /**< MHI Control Interrupt set */ 
   MHI_CRDB_INT=0x2                                 /**< MHI Command Ring Doorbell Interrupt Set */
}mhictrl_int_status;

typedef union _mhi_interrupts
{
  mhictrl_int_status ctrl;                          /**< Control interrupts */ 
  uint32 chdb[4];                                   /**< Channel doorbell interrupt mask */ 
  uint32 erdb[4];                                   /**< Event doorbell interrupt mask */ 
}mhi_interrupts_t;

/** Externalized gloabls */
/** MHI Base, initialized on init */
extern uint32 mmio_base;

/* MHI Spec register base */
extern uint32 mmio_mhi_reg_base;


/** 
 * @brief      mhi_mmio_enablechdbinterrupts
 *
 *             Enable specified channel doorbell interrupts 
 *              
 * 
 * @param[in]  *mask - mask representing interrupts to enable
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
void mhi_mmio_enablechdbinterrupts(mhi_interrupts_t *mask);

/** 
 * @brief      mhi_mmio_disablechdbinterrupts
 *
 *             Disable specified channel doorbell interrupts 
 *              
 * 
 * @param[in]  *mask - Mask representing interrupts to disable
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
void mhi_mmio_disablechdbinterrupts(mhi_interrupts_t *mask);

/** 
 * @brief      mhi_mmio_disablectrlinterrupt
 *
 *             Disable control interrupts
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
void mhi_mmio_disablectrlinterrupt(void);

/** 
 * @brief      mhi_mmio_enablectrlinterrupts
 *
 *             Enable control interrupts
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
void mhi_mmio_enablectrlinterrupts(void);

/** 
 * @brief      mhi_mmio_enableerdbinterrupt
 *
 *             Enable doorbell interrupt of specified event ring
 *              
 * 
 * @param[in]  ernum - Event ring number
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
void mhi_mmio_enableerdbinterrupt(uint32 ernum);

/** 
 * @brief      mhi_mmio_enableerdbinterrupts
 *
 *             Enable specified event ring doorbell interrupts
 *              
 * 
 * @param[in]  *mask - Mask representing interrupts to enable
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
void mhi_mmio_enableerdbinterrupts(mhi_interrupts_t *mask);

/** 
 * @brief      mhi_mmio_disableerdbinterrupts
 *
 *             Disable specified event ring doorbell interrupts
 *              
 * 
 * @param[in]  *mask
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
void mhi_mmio_disableerdbinterrupts(mhi_interrupts_t *mask);

/** 
 * @brief      mhi_mmio_disableerdbinterrupt
 *
 *             Disable event ring doorbell interrupt of specified event ring
 *              
 * 
 * @param[in]  ernum - Event ring number
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
void mhi_mmio_disableerdbinterrupt(uint32 ernum);

/** 
 * @brief      mhi_mmio_unmaskinterrupts
 *
 *             Unmask all interrupts 
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
void mhi_mmio_unmaskinterrupts(void);

/** 
 * @brief      mhi_mmio_maskinterrupts
 *
 *             Mask all interrupts
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
void mhi_mmio_maskinterrupts(void);

/** 
 * @brief      mhi_mmio_clearinterrupts
 *
 *             Clear all interrupts
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
void mhi_mmio_clearinterrupts(void);

/** 
 * @brief      mhi_mmio_reset
 *
 *             Reset mmio 
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
void mhi_mmio_reset(void);

/**
 * @brief      mhi_mmio_init
 *
 *             Performs any initialization necessary for mmio register operation 
 *
 * @param[in]  base - Address of MHI register base
 * @param[in]  mhiConfig - MHI HW/SW Device configuration 
 *              
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_ERROR if operation failed 
 * 
 * @sideeffect  None 
*/
mhi_status_t mhi_mmio_init(uint32 base, mhi_config_t *mhiConfig);

/**
 * @brief      mhi_mmio_deinit
 *
 *             Cleanup mhi_mmio_init operation 
 *
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              MHI_SUCCESS if operation succeeded  
 *              MHI_ERROR if operation failed 
 * 
 * @sa
 * mhi_mmio_init 
*/
mhi_status_t mhi_mmio_deinit(void);

/**
 * @brief      mhi_mmio_get_mhistate
 *
 *             Get MMIO MHI state from MHICTRL  
 *
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *             MHI state.  
 * 
 * @sa
 * mhi_mmio_init 
*/
mhi_state_t mhi_mmio_get_mhistate(void);

/** 
 * @brief      mhi_mmio_is_reset
 *
 *             Return the reset bit of MMIO
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             MMIO reset bit
 * 
 * @sideeffect None 
 */
uint32 mhi_mmio_is_reset(void);

/** 
 * @brief      mhi_mmio_clear_reset
 *
 *             Clear the reset bit
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
void mhi_mmio_clear_reset(void);

/** 
 * @brief      mhi_mmio_get_chbase
 *
 *             Get the channel context base address
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Channel context base address
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_chbase(void);

/** 
 * @brief      mhi_mmio_get_erbase
 *
 *             Get the event context base address
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Event context base address
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_erbase(void);

/** 
 * @brief      mhi_mmio_get_crbase
 *
 *             Get the command ring context base address
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Return the command ring context base address
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_crbase(void);

/** 
 * @brief      mhi_mmio_get_mhiaddr
 *
 *             Get the MHI address information
 *              
 * 
 * @param[out]  *mhiaddr - Received MHI address info configured by host
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
void mhi_mmio_get_mhiaddr(mhi_host_addr_t *mhiaddr);

/** 
 * @brief      mhi_mmio_get_crdb
 *
 *             Get the command ring doorbell value
 *              
 *
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Command ring doorbell value
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_crdb(void);

/** 
 * @brief      mhi_mmio_get_chdb
 *
 *             #desc 
 *              
 * 
 * @param[in]  cci
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
uint64 mhi_mmio_get_chdb(uint32 cci);

/** 
 * @brief      mhi_mmio_get_erdb
 *
 *             Get the event ring doorbell of specified event ring
 *              
 * 
 * @param[in]  eci - Event ring index
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             Event ring doorbell
 * 
 * @sideeffect None 
 */
uint64 mhi_mmio_get_erdb(uint32 eci);

/** 
 * @brief      mhi_mmio_get_mhictrlinterrupts
 *
 *             Get MHI Control interrupts. The first parameter is or-red with new interrupts
 *             so as to not clear any existing ones. The second parameter changed is set to 
 *             new interrupts
 *              
 * 
 * @param[out]  *interrupts - Interrupt object to receive interrupts
 * @param[out]  *changed    - Interrupt object to receive new interrupts
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Clears control interrupts 
 */
void mhi_mmio_get_mhictrlinterrupts(mhi_interrupts_t *interrupts, mhi_interrupts_t *changed );

/** 
 * @brief      mhi_mmio_get_chdbinterrupts
 *
 *             Get MHI channel doorbell interrupts. The first parameter is or-red with new interrupts
 *             so as to not clear any existing ones. The second parameter changed is set to 
 *             new interrupts
 *              
 * 
 * @param[out]  *interrupts - Interrupt object to receive interrupts
 * @param[out]  *changed    - Interrupt object to receive new interrupts
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Clears only enabled interrupts 
 */
void mhi_mmio_get_chdbinterrupts(mhi_interrupts_t *interrupts, mhi_interrupts_t *changed);

/** 
 * @brief      mhi_mmio_get_erdbinterrupt
 *
 *             Return single event ring doorbell interrupt. 
 *              
 * 
 * @param[in]  erdb - Event ring 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Clears interrupt
 */
boolean mhi_mmio_get_erdbinterrupt(uint32 ernum);

/** 
 * @brief      mhi_mmio_get_erdbinterrupts
 *
 *             Get MHI event ring doorbell interrupts. The first parameter is or-red with new interrupts
 *             so as to not clear any existing ones. The second parameter changed is set to 
 *             new interrupts
 *              
 * 
 * @param[out]  *interrupts - Interrupt object to receive interrupts
 * @param[out]  *changed    - Interrupt object to receive new interrupts
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect Clears enabled event ring interrupts 
 */
void mhi_mmio_get_erdbinterrupts(mhi_interrupts_t *interrupts, mhi_interrupts_t *changed );

/**
 * @brief      mhi_mmio_set_mhistate
 *
 *             Set MMIO registers to reflect corresponding state 
 *
 * @param[in]  state - MHI state to set 
 *
 * @dependencies
 *             None 
 * 
 * @return       
 *             MHI_SUCCESS if operation succeeded  
 *             MHI_ERROR if operation failed 
 * 
 * @sideeffect None
*/
mhi_status_t mhi_mmio_set_mhistate(mhi_state_t state);

/** 
 * @brief      mhi_mmio_set_execenv
 *
 *             Set the execution environment
 *              
 * 
 * @param[in]  env - Environment
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
void mhi_mmio_set_execenv(uint32 env);

/**
 * @brief       mhi_mmio_backup
 *
 *              Restore MMIO from RAM.
 *
 *@param[in]    *mhiConfig - MHI HW/SW Device configuration 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None
 *
 * 
 * @sideeffect  None 
 */
void mhi_mmio_backup(mhi_config_t *mhiConfig);

/**
 * @brief       mhi_mmio_restore
 *
 *              Restore MMIO from RAM.
 *
 *@param[in]    *mhiConfig - MHI HW/SW Device configuration 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None
 *
 * 
 * @sideeffect  None 
 */
void mhi_mmio_restore(mhi_config_t *mhiConfig);

/**
 * @brief       mhi_mmio_set_mhictrl_reg
 *
 *              Set the MHI state in the MHICTRL register. Used for test ONLY.
 *
 *@param[in]    state - MHI state to set.
 *
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None
 *
 * 
 * @sideeffect  MHI control interrupt will be generated. 
 */
void mhi_mmio_set_mhictrl_mhistate(mhi_state_t state);

/**
 * @brief       mhi_mmio_set_reset
 *
 *              Set the reset bit in the MHICTRL register. Used for test ONLY.
 *
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None
 *
 * 
 * @sideeffect  MHI control interrupt will be generated. 
 */
void mhi_mmio_set_reset(void);

/**
 * @brief       mhi_mmio_get_mhistate_from_status_reg
 *
 *              Reads MHI status only from the MHISTATUS register. Used only in test
 *              for now.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              MHI State
 *
 * 
 * @sideeffect  None 
 */
mhi_state_t mhi_mmio_get_mhistate_from_status_reg(void);

#endif
