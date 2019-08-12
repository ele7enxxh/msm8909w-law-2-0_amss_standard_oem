#ifndef MHI_HWACC_H
#define MHI_HWACC_H

/** 
  @file mhi.h
  @brief
  This header files defines the interface to the MHI hardware accelerator. Note 
  that hardware accelerator has been abbreviated to hw acc.
    

*/
/*
===============================================================================
  
                             Edit History
 
 $Header: 
 
when       who     what, where, why
--------   ---     ------------------------------------------------------------
08/15/13   am      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/


#include "mhi.h"
#include "mhi_i.h"

#ifdef MHIIPA
#include "ipa_per_mhi.h"
#include "ipa_per_common.h"
#endif

#define IPA_UC_MAILBOX_CDB_TARGET  0x7962000
#define IPA_UC_MAILBOX_EDB_TARGET  0x7962080 

/** Structure containing information relevant to the hardware accelerator. **/
typedef struct _mhi_hw_acc_info
{
  uint32 *p_mhictrl_pa; /**< Pointer to phyical address of locally stored MHI control datastructures */ 
  
  uint32 *p_mhidata_pa; /**< Pointer to phyical address of locally stored MHI data datastructures */
} mhi_hw_acc_info_t;

/**
 * @brief        mhi_hw_acc_cb
 *
 *               Callback invoked by hardware accelerator when certain events are
 *               detected and need the awareness of the MHI driver. 
 *
 * @param[in]    ind_type - Type of indication                
 * @param[in]    *info    - Pointer to a data structure associated with the indication
 * @param[in]    cb_data  - Callback data passed at the time of callback registration
 *
 * @dependencies
 *               ipa_per_reg_cb must be called to register this cb. 
 * 
 * @return       
 *               None   
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_cb(uint32 ind_type, void* info, void* cb_data);

/**
 * @brief        mhi_hw_acc_init
 *
 *               Initialize the MHI hardware accelerator (hw acc).
 *
 *@param[in]     *hw_acc_info - pointer to the hw acc context.                
 *
 * @dependencies
 *               None 
 * 
 * @return       
 *               SUCCESS if successful.   
 *               ERROR  if error occurred.   
 * 
 * @sideeffect   None 
 */
uint32 mhi_hw_acc_init(mhi_hw_acc_info_t *hw_acc_info);

/**
 * @brief        mhi_hw_acc_reset
 *
 *               Reset the hw acc.
 *
 * @dependencies
 *               None 
 * 
 * @return       
 *               None
 *
 * @sideeffect   None 
 */
void mhi_hw_acc_reset(void);

/**
 * @brief        mhi_hw_acc_start
 *
 *               Start the hw acc.
 *
 *
 * @dependencies
 *               None 
 * 
 * @return       
 *               None 
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_start(void);

/**
 * @brief        mhi_hw_acc_enable_pm
 *
 *               Enable hw acc power management.
 *
 *
 * @dependencies
 *               None  
 * 
 * @return       
 *               None
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_enable_pm(void);

/**
 * @brief        mhi_hw_acc_disable_pm
 *
 *               Disable hw acc power management.
 *
 *
 * @dependencies
 *               mhi_hw_acc_start must be called first.
 * 
 * @return       
 *               None
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_disable_pm(void);

/**
 * @brief        mhi_hw_acc_suspend
 *
 *               Suspend the MHI channels dedicated to the hw acc.
 *
 *
 * @dependencies
 *               mhi_hw_acc_start must be called first. 
 * 
 * @return       
 *               None
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_suspend(void);

/**
 * @brief        mhi_hw_acc_resume
 *
 *               Resume the MHI channels dedicated to the hw acc.
 *
 *
 * @dependencies
 *               mhi_hw_acc_start must be called first.
 *               Ideally should be called only if mhi_hw_acc_suspend was called prior to this. 
 * 
 * @return       
 *               None
 * 
 * @sideeffect   None 
 */
void mhi_hw_acc_resume(void);

/** 
 * @brief      mhi_hw_acc_ch_cmd
 *
 *             Relay a channel command to the hardware accelerator 
 *              
 * 
 * @param[in]  chid - channel id 
 * @param[in]  cmd  - command
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
uint32 mhi_hw_acc_ch_cmd(uint32 chid , ch_cmd_t cmd);

#endif /* #ifndef MHI_HWACC_H */ 
