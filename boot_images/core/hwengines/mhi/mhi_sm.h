#ifndef MHI_SM_H
#define MHI_SM_H

/** 
  @file mhi_sm.h
  @brief
  This header file defines data structures and public apis associated with the mhi state machine. 
    

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


#include "mhi_i.h"
#include "mhi_osal.h" 
#include "mhi_mmio.h"
#include "mhi_pm.h"


/** Triggers that affected the MHI state machine */
#define MHI_SM_TRIG_DEVICE_WAKE_SHIFT         0
#define MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT    1
#define MHI_SM_TRIG_HW_ACC_WAKEUP_SHIFT       2
#define MHI_SM_TRIG_CORE_WAKEUP_SHIFT         3
#define MHI_SM_TRIG_D_STATE_CHANGE_SHIFT      4
#define MHI_SM_TRIG_HOST_WAKEUP_MHI_REQ_SHIFT 5
#define MHI_SM_TRIG_LINK_UP_SHIFT             6
#define MHI_SM_TRIG_LINK_DOWN_SHIFT           7
#define MHI_SM_TRIG_CTRL_INTR_SHIFT           8
#define MHI_SM_TRIG_MANUAL_SHIFT              9


/** Generate the mask from the shift, i.e., from the bit position value */
#define GEN_MASK(shift) (1<<(shift))

/** Pending notifications of state machine triggers */
typedef uint32 mhi_sm_trigs_pending_t;

typedef struct _mhi_sm_runtime mhi_sm_runtime_t;

typedef struct _mhi_sm_debug_counters
{
   uint32                  M0_entry; /**< # of times M0 entered */

   uint32                  M1_entry; /**< # of times M1 entered */

   uint32                  M2_entry; /**< # of times M2 entered */

   uint32                  M0_M3_entry; /**< # of times M3 entered from M0*/

   uint32                  M1_M3_entry; /**< # of times M3 entered from M1*/

   uint32                  device_wake_toggles; /**< # of times the device wake state change event was received */

   uint32                  hw_acc_wakeup[MHI_STATE_MAX]; /**< # of hw acc wakeups received in a given M state*/ 

   uint32                  core_wakeup[MHI_STATE_MAX]; /**< # of core wakeups received in a given M state */
} mhi_sm_debug_counters_t;

typedef struct _mhi_sm_info
{
   mhi_osal_sync_type     *mhiSync;  /**< Pointer to main MHI Synchornization structure */ 
   
   mhi_state_t             mhiState;  /**< Current MHI state accordind to the state machine */       

   mhi_sm_trigs_pending_t  mhi_sm_trigs_pending; /**< Variable whose bits keep track of pending triggers for the state machine */

   boolean                 wakeup_pending; /**< Register wake up pending*/

   boolean                 device_wake; /**< State of device wake last cached by the state machine */
  
   mhi_sm_runtime_t       *sm_rtime_info;  /**< Pointer to state machine runtime variables. This is mainly for debug */

#ifndef MHI_BOOT
   mhi_sm_debug_counters_t mhi_sm_debug_counters; /**< Struct containing state machine debug counters */
#endif /* #ifndef MHI_BOOT */   
} mhi_sm_info_t;


/**
 * @brief       mhi_sm_init
 *
 *              Initialize the MHI state machine.
 *              
 * @param[in]   *sm_info - pointer to the MHI state machine context.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_init(mhi_sm_info_t *sm_info);

/**
 * @brief       mhi_sm_reset
 *
 *              Reset the MHI state machine. 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */ 
void mhi_sm_reset(void);

/**
 * @brief       mhi_sm_set_mhiState
 *
 *              Set new mhi state in state machine context and update MHISTATUS reg.
 *              Must be called directly or indirectly from mhi_sm_run_state_machine().
 *              mhi_sm_init() being the only exception.
 *
 *
 * @param[in]   state - MHI state to set
 *
 * @dependencies
 *              None
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */ 
void mhi_sm_set_mhiState(mhi_state_t state);

/**
 * @brief       mhi_sm_run_state_machine
 *
 *              Initialize the state machine runtime variables. 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static __inline void mhi_sm_init_runtime_variables(mhi_sm_runtime_t *sm_Rtime);

/**
 * @brief       mhi_sm_set_runtime_variables
 *
 *              Sets the appropriate sm runtime variable which prompts action
 *              from the state machine. Also clears notifications which are pending. 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static uint32 mhi_sm_set_runtime_variables(mhi_sm_runtime_t *sm_Rtime);

/**
 * @brief       mhi_sm_run_state_machine
 *
 *              Run the MHI state machine. 
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_run_state_machine(void);

/**
 * @brief       mhi_sm_wakeup_from_M2
 *
 *              Handles the wakeup procedure from M2 MHI state.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static uint32 mhi_sm_wakeup_from_M2(mhi_sm_runtime_t *sm_Rtime);

/**
 * @brief       mhi_sm_wakeup_from_M3
 *
 *              Handles the wakeup request received in M3 MHI state.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static uint32 mhi_sm_wakeup_from_M3(mhi_sm_runtime_t *sm_Rtime);

/**
 * @brief       mhi_sm_process_sm_trigs_link_disabled
 *
 *              Handle triggers received for the state machine when the
 *              link hardware is disabled.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static void mhi_sm_process_sm_trigs_link_disabled(mhi_sm_runtime_t sm_Rtime);

/**
 * @brief       mhi_sm_set_trig_pending
 *
 *              Set a pending MHI state machine trigger notification 
 *
 * @param[in]   trigger - Event notification that needs to be registered with pending
 *                        notifications for the state machine. 
 * @param[in]   signal_worker_thread - TRUE if mhi_core_scheduler needs to be signaled. 
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_set_trig_pending(uint32 trigger, boolean signal_worker_thread);

/**
 * @brief       mhi_sm_clear_trig_pending
 *
 *              Clear a pending MHI state machine trigger notification 
 *
 * @param[in]   trigger - Event notification that needs to be cleared from the
 *                        memory location containing pending notifications.  
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_clear_trig_pending(uint32 trigger);

/**
 * @brief       mhi_sm_clear_trig_pending
 *
 *              Get status of which state machine event notifications or triggers
 *              are currently pending. 
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
mhi_sm_trigs_pending_t mhi_sm_get_trigs_status(void);

/**
 * @brief       mhi_sm_get_mhistate
 *
 *              Get the current MHI state according to the state machine. This
 *              is for use by external modules.
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              MHI state  
 * 
 * @sideeffect  None 
 */
mhi_state_t mhi_sm_get_mhistate(void);

/**
 * @brief       mhi_sm_set_syserr_mhistate
 *
 *              Set the MHI state to SYSERR. All other MHI states should be 
 *              handled by run_state_machine.
 *
 * @dependencies
 *              mhi_sm_init
 * 
 * @return       
 *              MHI state  
 * 
 * @sideeffect  None 
 */
void mhi_sm_set_syserr_mhistate(void);

#endif
