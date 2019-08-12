/** 
  @file mhi_sm.c
  @brief
  This file implements the mhi state machine. 
    

*/
/*
===============================================================================
 
                             Edit History
 
 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/10/14   am      Added MHI Spec Rev D Changes    
02/03/14   am      Major clean up of state machine
08/15/13   am      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi.h"
#include "mhi_sm.h"
#include "mhi_hwacc.h"
#include "mhi_core.h"

/* Variables used by the MHI SM during run time */
struct _mhi_sm_runtime
{
   mhi_state_t mhi_oldState;             /**< Old MHI state */ 
   mhi_state_t mhi_newState;             /**< New MHI state */ 
   mhi_link_d_state_t d_state;           /**< Device state */ 
   boolean mhi_client_requested_wakeup;  /**< MHI client requested wakeup */ 
   boolean link_inact_intr_received;     /**< Link inactivity interrupt was received */ 
   boolean device_wake_oldState;         /**< Old state of device wake */
   boolean device_wake_newState;         /**< New state of device wake */ 
   boolean wakeup_received_from_host;    /**< Wakeup received from host */ 
};

/* State machine run time */
static mhi_sm_runtime_t sm_rtime;

/* Static function prototypes */
#ifndef MHI_BOOT 
static uint32 mhi_sm_wakeup_from_M2(mhi_sm_runtime_t *sm_Rtime);
static uint32 mhi_sm_wakeup_from_M3(mhi_sm_runtime_t *sm_Rtime);
static void mhi_sm_process_sm_trigs_link_disabled(mhi_sm_runtime_t sm_Rtime);
#endif /* #ifndef MHI_BOOT  */

/*Initialize debug counters */
static void mhi_sm_init_debug_counters(void)
{
#ifndef MHI_BOOT 
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_entry = 0;
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M1_entry = 0;
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M2_entry = 0;
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_M3_entry = 0; 
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M1_M3_entry = 0;
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.device_wake_toggles = 0;
   memset(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.core_wakeup,0,sizeof(uint32)*MHI_STATE_MAX);
   memset(&mhiCtx.mhi_sm_info.mhi_sm_debug_counters.hw_acc_wakeup,0,sizeof(uint32)*MHI_STATE_MAX);
#endif /* #ifndef MHI_BOOT */ 
}

/**
 * @brief       mhi_sm_init
 *
 *              Initialize the MHI state machine.
 *              
 * @param[in]   *mhi_sm_info - pointer to the MHI state machine context.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
void mhi_sm_init(mhi_sm_info_t *sm_info)
{
   mhi_osal_debug(MSG_HIGH,"mhi_sm_init: Enter");

   mhiCtx.mhi_sm_info.mhiState=MHI_STATE_RESET;

   mhiCtx.mhi_sm_info.wakeup_pending = FALSE;
   mhiCtx.mhi_sm_info.mhi_sm_trigs_pending = 0;
   mhiCtx.mhi_sm_info.sm_rtime_info = &sm_rtime;
   mhiCtx.mhi_sm_info.device_wake = mhi_pm_get_device_wake_gpio_state();

   mhi_sm_init_debug_counters();


   /* If current state is RESET set to READY otherwise leave the state as RESET, 
    * once mhi_run_state_machine executes it will get set to correct state */
   if(MHI_STATE_RESET == mhi_mmio_get_mhistate())
   {
      /* Set MHI state to ready, so host can start initializing MMIO */ 
      mhi_sm_set_mhiState(MHI_STATE_READY);
   }
   else
   {
      mhi_sm_set_trig_pending(MHI_SM_TRIG_MANUAL_SHIFT, FALSE);
   }
}

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
void mhi_sm_reset(void)
{
   /* Reset MHI state machine */
   mhi_sm_set_mhiState(MHI_STATE_RESET);
   mhiCtx.mhi_sm_info.wakeup_pending = FALSE;
   mhiCtx.mhi_sm_info.mhi_sm_trigs_pending = 0;
   mhiCtx.mhi_sm_info.device_wake = mhi_pm_get_device_wake_gpio_state();
   
   mhi_sm_init_debug_counters();
}

/**
 * @brief       mhi_sm_send_state_change_event
 *
 *              Send MHI state change event to the host
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
void mhi_sm_send_state_change_event(mhi_state_t state)
{
  mhi_ring_element_t event;

  event.stateChgEvent.mhistate = state; 
  event.stateChgEvent.type=MHI_RING_EL_TYPE_MHI_STATE_CHG;
  if (SUCCESS != mhi_core_send_event(PRIMARY_ER,&event,1))
  {
    mhi_core_sys_error();
  }
}

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
void mhi_sm_set_mhiState(mhi_state_t state)
{
   mhiCtx.mhi_sm_info.mhiState=state; 
   mhi_mmio_set_mhistate(state);
}

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
static __inline void mhi_sm_init_runtime_variables(mhi_sm_runtime_t *sm_Rtime)
{
   sm_Rtime->device_wake_oldState = mhiCtx.mhi_sm_info.device_wake;
   sm_Rtime->device_wake_newState = sm_Rtime->device_wake_oldState;
   sm_Rtime->mhi_oldState = mhiCtx.mhi_sm_info.mhiState;
   sm_Rtime->mhi_newState = sm_Rtime->mhi_oldState;
   sm_Rtime->d_state = MHI_LINK_INVALID_D_STATE;
   sm_Rtime->mhi_client_requested_wakeup = FALSE;
   sm_Rtime->link_inact_intr_received = FALSE;
   sm_Rtime->wakeup_received_from_host = FALSE;
}

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
static uint32 mhi_sm_set_runtime_variables(mhi_sm_runtime_t *sm_Rtime)
{
   mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: MHI SM trigs pending bitmask: %x",
                                            mhiCtx.mhi_sm_info.mhi_sm_trigs_pending);

   /* Manual trigger */
   if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_MANUAL_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_MANUAL_SHIFT);
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received manual state machine trigger in M State = %d",sm_Rtime->mhi_oldState);
   }
   /* Check for M state change */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_CTRL_INTR_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_CTRL_INTR_SHIFT);
      sm_Rtime->mhi_newState=mhi_mmio_get_mhistate();
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received M state change event. New M State = %d",sm_Rtime->mhi_newState);
   }
#ifndef MHI_BOOT   
   /* Check for d state change */
   else if(mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_D_STATE_CHANGE_SHIFT))
   {
      if (MHI_STATE_M3 != sm_Rtime->mhi_oldState)
      {
         mhi_osal_debug(MSG_FATAL,"mhi_sm_set_runtime_variables: Unexpected D state change in MHI state: %d",sm_Rtime->mhi_oldState);
      }
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_D_STATE_CHANGE_SHIFT);
      sm_Rtime->d_state = mhi_pm_get_d_state();
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received D state change event. New D State = %d",sm_Rtime->d_state);
   }   
  /* Check if mhi core originated a wakeup request */
  else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_CORE_WAKEUP_SHIFT)) 
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_CORE_WAKEUP_SHIFT);
      sm_Rtime->mhi_client_requested_wakeup = TRUE;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received wake up request from MHI CORE");
      mhiCtx.mhi_sm_info.mhi_sm_debug_counters.core_wakeup[mhiCtx.mhi_sm_info.mhiState]++;
   }
   /* Check if hardware accelerator originated wakeup request */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_HW_ACC_WAKEUP_SHIFT)) 
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_HW_ACC_WAKEUP_SHIFT);
      sm_Rtime->mhi_client_requested_wakeup = TRUE;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received wake up request from HW Acc");
      mhiCtx.mhi_sm_info.mhi_sm_debug_counters.hw_acc_wakeup[mhiCtx.mhi_sm_info.mhiState]++;
   }
   /* Check if the state of device wake changed */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_DEVICE_WAKE_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_DEVICE_WAKE_SHIFT);
      sm_Rtime->device_wake_newState = mhi_pm_get_device_wake_gpio_state();
      mhiCtx.mhi_sm_info.device_wake = sm_Rtime->device_wake_newState;
      mhiCtx.mhi_sm_info.mhi_sm_debug_counters.device_wake_toggles++;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received device_wake state change"); 
   }
   /* Check if the link inactivity interrupt fired */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_LINK_INACT_TIMER_SHIFT);
      sm_Rtime->link_inact_intr_received = TRUE;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received link inactivity interrupt");    
   }
   /* Host sent wakeup request */
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_HOST_WAKEUP_MHI_REQ_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_HOST_WAKEUP_MHI_REQ_SHIFT);
      sm_Rtime->wakeup_received_from_host = TRUE;
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received wakeup from host PERST De-asserted");    
   }
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_LINK_UP_SHIFT))
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_LINK_UP_SHIFT);
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received link up event");    
   } 
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending & GEN_MASK(MHI_SM_TRIG_LINK_DOWN_SHIFT)) 
   {
      mhi_sm_clear_trig_pending(MHI_SM_TRIG_LINK_DOWN_SHIFT);
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Received link down event");    
   } 
#endif /* #ifndef MHI_BOOT */   
   else if (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending)
   {
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: sm_trig = %d not handled",mhiCtx.mhi_sm_info.mhi_sm_trigs_pending);
   }
   else
   {
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: No sm_trigs_pending");
   }

   return SUCCESS;
}

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
void mhi_sm_run_state_machine(void)
{
   mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: Enter");

   /* Acquire mutex */
   mhi_osal_syncenter(mhiCtx.mhi_sm_info.mhiSync);

   /* Check for reset. All states transition to reset, when RESET bit is set in MHICTRL */
   if(mhi_link_is_hw_enabled() && mhi_mmio_is_reset())
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: mhi_reset");
      mhi_core_reset();
   }

   /* Keep running the state machine until there's nothing left to do */
   while (mhiCtx.mhi_sm_info.mhi_sm_trigs_pending)
   {
      /* Initialize the sm runtime variables */
      mhi_sm_init_runtime_variables(&sm_rtime);

      /* Set the stimulus for the state machine */
      mhi_sm_set_runtime_variables(&sm_rtime);
 
      switch(sm_rtime.mhi_oldState)
      {
          case MHI_STATE_RESET:
            /* Get the MHI state. If not RESET move to correct state. Required for
             * PCIe bootchain, where MHI is already in M0 by the time AMSS is launched */ 
            if(MHI_STATE_RESET != mhi_mmio_get_mhistate())
            {
               /* Set internal state to current state */
               mhiCtx.mhi_sm_info.mhiState = mhi_mmio_get_mhistate(); 

               /* Start the hardware accelerator */
               mhi_hw_acc_start();
            }
            break;
         case MHI_STATE_READY:
            if(MHI_STATE_M0 == sm_rtime.mhi_newState)
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition Ready -> M0"); 
               /* Cache channel contexts, event contexts etc. */

               if (SUCCESS != mhi_core_cache_hostconfig())
               {
                  mhi_core_sys_error();
               }

#ifndef MHI_BOOT
               /* Start the hardware accelerator */
               mhi_hw_acc_start();
#endif               

               mhi_sm_set_mhiState(MHI_STATE_M0); 
               
#ifndef MHI_BOOT
               mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_entry++;
#endif /* #ifndef MHI_BOOT */

               /* Release the mhiSync mutex and reacquire after signalling waiting
                * channels to avoid deadlock */
               mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);    
               /* Signal any waiting channels */
               mhi_core_process_ch_requestopen();
               mhi_osal_syncenter(mhiCtx.mhi_sm_info.mhiSync);  

               /* Send MHI M0 state set event */
               mhi_sm_send_state_change_event(MHI_STATE_M0);

               /* Send exec env state change notification to host */   
               mhi_core_send_exec_env_change_event(MHI_ENV);
            }
            else
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in READY"); 
            }
            break;
#ifndef MHI_BOOT            
         case MHI_STATE_M0:
            if(MHI_STATE_M3 == sm_rtime.mhi_newState)
            {
               /* Device needs to set all valid channels to suspend in the host context. */
               mhi_core_suspend();

               /* IPA suspend */ 
               mhi_hw_acc_suspend();

               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M0 -> M3");
               mhi_sm_set_mhiState(MHI_STATE_M3); 
               mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_M3_entry++;
               /* Send MHI M3 state event */
               mhi_sm_send_state_change_event(MHI_STATE_M3);

               /* Disable link inactivity intr, not relevant in M3 */
               mhi_link_disable_inactivity_intr(); 

               /* Check if MHI has pending work to be done */ 
               mhi_core_ack_interrupts();

               if (mhiCtx.is_mhi_work_pending)
               {
                  mhi_sm_wakeup_from_M3(&sm_rtime);
               }
            }
            else if(sm_rtime.device_wake_newState != sm_rtime.device_wake_oldState)
            {
               /* Device wake toggled, enabl/disable link inactivity intr accordingly */
               if (LOW == sm_rtime.device_wake_newState)
               {
                  mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Arm the link inactivity timer interrupt");
                  mhi_link_enable_inactivity_intr(); 
               }
               else 
               {
                  mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Disarm the link inactivity timer interrupt");
                  mhi_link_disable_inactivity_intr();
               }
            }
            else if(sm_rtime.link_inact_intr_received)  
            {
               if(LOW != sm_rtime.device_wake_newState) 
               {
                  mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: link inactivity intr fired but device_wake asserted");
                  continue;
               }

               /* Check if MHI has pending work to be done */
               mhi_core_ack_interrupts();

               if (mhiCtx.is_mhi_work_pending)
               {
                  mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: Work pending, don't suspend MHI"); 
                  /* Do not suspend if there is work pending */
                  continue;
               }

               /* Device needs to set all valid channels to suspend in the host context. */
               mhi_core_suspend();

               /* IPA suspend */ 
               mhi_hw_acc_suspend();

               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M0 -> M1");  
               mhi_sm_set_mhiState(MHI_STATE_M1);
               mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M1_entry++;

               /* Send MHI M1 state event */
               mhi_sm_send_state_change_event(MHI_STATE_M1);
            }
            else
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in M0");
            }
            break;
         case MHI_STATE_M1:
            if (MHI_STATE_M2 == sm_rtime.mhi_newState)
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M1 -> M2"); 
               mhi_sm_set_mhiState(MHI_STATE_M2);  
               mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M2_entry++;
            
               /* Disable link inactivity intr, not relevant in M2 and don't want it going off if clocks are turned off.
                * Will be armed/disarmed again on entry in M0 */
               mhi_link_disable_inactivity_intr(); 

               /* Check if any interrupts are pending */
               mhi_core_ack_interrupts();

               /* Exit M2 immediately under the following circumstances. New client requests will trigger a wakeup */
               if (TRUE == sm_rtime.device_wake_newState ||
                   mhiCtx.mhi_sm_info.wakeup_pending ||
                   mhiCtx.int_ctrl.ctrl & MHI_CRDB_INT ||
                   mhiCtx.is_mhi_work_pending)
               {
                   mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Wake up condition detected exit M2 immediately");
                   mhi_sm_wakeup_from_M2(&sm_rtime);
               }
               else
               {
                  /* Invoke hw accelerator power management  */ 
                  mhi_hw_acc_enable_pm();

                  /* Link is not active, vote for sleep. */
                  mhi_pm_vote_for_sleep();

                  /* Link can now release hardware resources to save power */
                  mhi_link_release_hw_resources();
               }
            }
            else if(MHI_STATE_M3 == sm_rtime.mhi_newState)
            {
               /* No need to suspend as we are in M1  */
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M1 -> M3");
               mhi_sm_set_mhiState(MHI_STATE_M3);  
               mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M1_M3_entry++;
               /* Send MHI M3 event */
               mhi_sm_send_state_change_event(MHI_STATE_M3);
               
               /* Disable link inactivity intr, not relevant in M3 */
               mhi_link_disable_inactivity_intr();

               /* Check if MHI has pending work to be done */ 
               mhi_core_ack_interrupts();

               if (mhiCtx.is_mhi_work_pending || mhiCtx.mhi_sm_info.wakeup_pending)
               {
                  mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Wake up condition detected in M3");
                  mhi_sm_wakeup_from_M3(&sm_rtime);
               }
            }
            /* Device wake will be checked when SM attempts to go to M2, otherwise unnecessary wake up may
             * occur due to device wake if SM goes from M1 to M3 instead of M2 */
            else if (sm_rtime.mhi_client_requested_wakeup)
            { 
               /* Wakeups are acted upon only in M2, D3hot or D3cold states */
               mhiCtx.mhi_sm_info.wakeup_pending = TRUE;
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Wake up received in M1");
            }
            else
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in M1");
            }
            break; 
         case MHI_STATE_M2:
           if (sm_rtime.device_wake_newState || sm_rtime.mhi_client_requested_wakeup)
            {             
               mhi_sm_wakeup_from_M2(&sm_rtime);
            } 
            else
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in M2");
            }
            break;         
         case MHI_STATE_M3: 
            if(MHI_STATE_M0 == sm_rtime.mhi_newState)
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: State transition M3 -> M0"); 
               mhi_sm_set_mhiState(MHI_STATE_M0); 
               mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_entry++;

               /* Send MHI M0 state change event */
               mhi_sm_send_state_change_event(MHI_STATE_M0);
               
               /* resume mhi */
               mhi_core_resume();
              
               /* resume ipa */
               mhi_hw_acc_resume();

               /* Enable link inactivity timer if required */
               if (LOW == sm_rtime.device_wake_newState)
               {
                  mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Arm the link inactivity timer interrupt");
                  mhi_link_enable_inactivity_intr(); 
               }

               /* Resume procedure complete, clear wake up pending flag */
               mhiCtx.mhi_sm_info.wakeup_pending = FALSE;
            }
            else if(MHI_LINK_INVALID_D_STATE != sm_rtime.d_state)
            {
               mhi_pm_process_d_state_change(sm_rtime.d_state);
            }
            else if (sm_rtime.mhi_client_requested_wakeup)
            {
               mhi_sm_wakeup_from_M3(&sm_rtime);
            }
            else if(sm_rtime.wakeup_received_from_host)
            {
               mhi_pm_wakeup_device_req_fromHost();
            } 
            else
            {
               mhi_osal_debug(MSG_HIGH,"mhi_sm_run_state_machine: Nothing to do in M3");
            }
            break;
#endif /* #ifndef MHI_BOOT */           
         default:
            mhi_osal_debug(MSG_FATAL,"mhi_sm_run_state_machine: Unexpected state");
            return; 
      }

      /* Check for reset. All states transition to reset, when RESET bit is set in MHICTRL */
      if(mhi_link_is_hw_enabled() && mhi_mmio_is_reset())
      {
         mhi_osal_debug(MSG_HIGH, "mhi_sm_run_state_machine: mhi_reset");
         mhi_core_reset();
      }
   }

   /* Release mutex */
   mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);
}

#ifndef MHI_BOOT 
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
static uint32 mhi_sm_wakeup_from_M2(mhi_sm_runtime_t *sm_Rtime)
{
   /* Vote against sleep/HW ACC power management only if it has been voted for */
   if (mhi_pm_is_sleep_voted())
   {
      /* According to the current state machine design, there is no circumstance 
       * where MHI votes for sleep, but does not vote for HW ACC power management */
      mhi_pm_vote_against_sleep();

      /* Request IPA resources */
      mhi_hw_acc_disable_pm();

      /* Link must acquire hardware resources to recover from power saving state */
      mhi_link_acquire_hw_resources();
   }

   /* M1 is set just for the record, since it's just a transient state. */
   mhi_osal_debug(MSG_HIGH,"mhi_sm_wakeup_from_M2: State transition M2 -> M1");
   mhi_sm_set_mhiState(MHI_STATE_M1);

   mhi_osal_debug(MSG_HIGH,"mhi_sm_wakeup_from_M2: State transition M1 -> M0");
   mhi_sm_set_mhiState(MHI_STATE_M0); 
   mhiCtx.mhi_sm_info.mhi_sm_debug_counters.M0_entry++;

   /* Send MHI M0 state set event */
   mhi_sm_send_state_change_event(MHI_STATE_M0);

   /* resume mhi */
   mhi_core_resume();
            
   /* resume ipa */
   mhi_hw_acc_resume();

   /* Enable link inactivity timer if required */
   if (LOW == sm_rtime.device_wake_newState)
   {
      mhi_osal_debug(MSG_HIGH,"mhi_sm_set_runtime_variables: Arm the link inactivity timer interrupt");
      mhi_link_enable_inactivity_intr(); 
   }

   /* Resume procedure complete, clear wake up pending flag */
   mhiCtx.mhi_sm_info.wakeup_pending = FALSE;
  
   return SUCCESS;
}

/**
 * @brief       mhi_sm_wakeup_from_M3
 *
 *              Handles the wake up procedure from M3 if a wake up condition
 *              exists.
 *
 * @dependencies
 *              None 
 * 
 * @return       
 *              None  
 * 
 * @sideeffect  None 
 */
static uint32 mhi_sm_wakeup_from_M3(mhi_sm_runtime_t *sm_Rtime)
{  
   /* Set flag to register pending wake up */
   if (FALSE == mhiCtx.mhi_sm_info.wakeup_pending)
   {
     mhiCtx.mhi_sm_info.wakeup_pending = TRUE;
   }

   return MHI_SUCCESS;
}
#endif /* #ifndef MHI_BOOT */

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
void mhi_sm_set_trig_pending(uint32 trigger, boolean signal_worker_thread)
{
   mhi_osal_debug(MSG_HIGH,"mhi_sm_set_trig_pending: %d",trigger);

   mhi_osal_atomic_set_bit(&mhiCtx.mhi_sm_info.mhi_sm_trigs_pending,trigger);
   /* External triggers usually require some processing on the MHI CORE thread.
    * But state machine triggers originating from the MHI worker thread wouldn't
    * want to unnecessarily signal the MHI CORE thread. */
   if (signal_worker_thread)
   {
      mhi_core_signal_worker_thread();
   }
}

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
void mhi_sm_clear_trig_pending(uint32 trigger)
{
   mhi_osal_debug(MSG_HIGH,"mhi_sm_clear_trig_pending: %d",trigger);

   mhi_osal_atomic_clear_bit(&mhiCtx.mhi_sm_info.mhi_sm_trigs_pending,trigger);
}

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
mhi_sm_trigs_pending_t mhi_sm_get_trigs_status(void)
{
   return mhiCtx.mhi_sm_info.mhi_sm_trigs_pending;
}

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
mhi_state_t mhi_sm_get_mhistate(void)
{
   mhi_state_t state;

   /* Synchronize with the MHI state machine */
   mhi_osal_syncenter(mhiCtx.mhi_sm_info.mhiSync);
   state = mhiCtx.mhi_sm_info.mhiState;
   mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);

   return state;
}

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
void mhi_sm_set_syserr_mhistate(void)
{
   /* Synchronize with the MHI state machine */
   mhi_osal_syncenter(mhiCtx.mhi_sm_info.mhiSync);
   mhiCtx.mhi_sm_info.mhiState = MHI_STATE_SYSERR;
   mhi_mmio_set_mhistate(mhiCtx.mhi_sm_info.mhiState);
   mhi_osal_syncleave(mhiCtx.mhi_sm_info.mhiSync);
}





