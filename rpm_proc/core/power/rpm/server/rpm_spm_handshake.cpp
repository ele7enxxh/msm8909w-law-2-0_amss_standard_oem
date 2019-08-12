/*==============================================================================

FILE:      DalRPMFWSpmHandshake.c

DESCRIPTION: This file implements SPM Handshakes and state changes

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

      Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential


$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/server/rpm_spm_handshake.cpp#1 $
$Date: 2015/02/25 $

==============================================================================*/

#define __STDC_LIMIT_MACROS

#include <stdlib.h>
#include <assert.h>
#include <limits.h>

extern "C"
{
    #include "DALSys.h"
    #include "cortex-m3.h"
}
#include "swevent.h"
#include "rpm.h"
#include "rpmserver.h"
#include "rpmserver_internal.h"
#include "CoreVerify.h"

#include "msmhwiobase.h"
#include "HALhwio.h"
#include "rpm_hwio.h"

#include "time_service.h"
#include "rpm_config.h"
#include "rpm_sched.h"
#include "rpm_spm_handshake.h"
#include "rpm_messageram.h"
#include "cortex-m3.h"

typedef struct
{
  uint8_t ee;
  uint8_t core;
} rpm_spm_isr_context_t;

void rpm_spm_shutdown_low_isr(void) __irq;
void rpm_spm_shutdown_high_isr(void) __irq;


static rpm_spm_isr_context_t   *spm_isr_context;

// Table of callbacks for SPM state changes.
// FIXME: in future revision, get the size of this table from target specific
//        code, since it's really target-dependent

#define SPM_CHANGE_STATE(new_state) \
{ \
    changed_state = TRUE; \
    rpm->ees[ee].subsystem_status = new_state; \
}

inline void rpm_acknowledge_spm_handshakes(unsigned ee)
{
    unsigned pending_bringups = __rbit(rpm->ees[ee].pending_bringups);

    unsigned core;
    while((core = __clz(pending_bringups)) != 32)
    {
        SWEVENT(RPM_BRINGUP_ACK, ee, core, rpm->ees[ee].is_soft_transition);

        if (rpm->ees[ee].is_soft_transition)
        {
            //send bringup ACK message
            rpm->ees[ee].is_soft_transition = false;
            rpm->ees[ee].handler->writeResponse(SPM_BRINGUP_ACK, 0, RPM_COMMAND_SERVICE);
        }
        else
        {
            HWIO_OUT(RPM_GPO_WDSET, (1 << (rpm->ees[ee].spm.bringupAcks[core]))); // set bringup_ack
        }
        pending_bringups &= ~(0x80000000 >> core);
    }

    // Handled the known pending bringups.
    if (rpm->ees[ee].pending_bringups)
    {
      rpm->ees[ee].stats->bringup_ack = time_service_now ();
    }
    
    rpm->ees[ee].pending_bringups = 0;
}

bool rpm_get_wakeup_deadline(unsigned ee, uint64_t &deadline)
{
  vmpm_data_t *vmpm = message_ram_vmpm((vmpm_masters)ee);
  deadline = vmpm->wakeup_time;

  return (deadline < UINT64_MAX);
}

void rpm_spm_state_machine(unsigned ee, rpm_spm_entry_reason reason)
{
    INTLOCK();

    bool        changed_state = false;
    EEData     *ee_state      = &(rpm->ees[ee]);
    SetChanger *changer       = ee_state->changer;

    do
    {
        switch(ee_state->subsystem_status)
        {
            case SPM_AWAKE:
                changed_state = FALSE;

                if(0 == ee_state->num_active_cores)
                {
                    SPM_CHANGE_STATE(SPM_GOING_TO_SLEEP);
                }
                else
                {
                    // We're awake, so make sure we keep up with any incoming bringup reqs.
                    rpm_acknowledge_spm_handshakes(ee);
                }
                break;

            case SPM_GOING_TO_SLEEP:
                if(changed_state)
                {
                    // check for scheduled wakeup
                    uint64_t deadline = 0;

                    if(! rpm_get_wakeup_deadline(ee, deadline))
                    {
                      deadline = 0;
                    }
                    changer->setWakeTime (deadline);
                    // enqueue immediate set transition to sleep
                    changer->enqueue(RPM_SLEEP_SET, 0);
                }
                changed_state = FALSE;

                // When we've finished selecting the sleep set, we're officially asleep.
                if((SPM_TRANSITION_COMPLETE == reason) && (RPM_SLEEP_SET == changer->currentSet()))
                {
                  SPM_CHANGE_STATE(SPM_SLEEPING);
                }
                // However, we might get a wakeup request before we've made it all the way to sleep.
                if(SPM_BRINGUP_REQ == reason)
                {
                    // Set the preempt flag; this will force the set change to recycle if
                    // it's currently running.  It will notice the processor has woken up
                    // and stop performing its work.
                    theSchedule().preempt();
                }
                break;

            case SPM_SLEEPING:
                if(changed_state)
                {
                    // check for scheduled wakeup
                    uint64_t deadline = changer->getWakeTime ();
                    
                    // enqueue scheduled wakeup request
                    changer->enqueue(RPM_ACTIVE_SET, deadline);
                }
                changed_state = FALSE;

                if(ee_state->num_active_cores > 0)
                {
                    SPM_CHANGE_STATE(SPM_WAKING_UP);
                }
                break;

            case SPM_WAKING_UP:
                if(changed_state)
                {
                    // work our way back to the active set
                    if(RPM_SLEEP_SET == changer->currentSet() || changer->inTransition())
                    {
                        changer->enqueue(RPM_ACTIVE_SET, 0);
                    }
                }
                changed_state = FALSE;

                // check for completion
                if(RPM_ACTIVE_SET == changer->currentSet() && !changer->inTransition())
                {
                  SPM_CHANGE_STATE(SPM_AWAKE);
                }
                break;
        }
    } while(changed_state);

    INTFREE();
}

static void rpm_spm_bringup_isr(void) __irq
{
    INTLOCK();

    unsigned isrNum = interrupt_current_isr();
    unsigned ee     = spm_isr_context[isrNum].ee;
    unsigned core   = spm_isr_context[isrNum].core;

    EEData               *ee_state = &(rpm->ees[ee]);
    const SPMDescription *spm      = &(ee_state->spm);

    CORE_VERIFY(ee < 0xFF);

    SWEVENT(RPM_BRINGUP_REQ, ee, core, ee_state->is_soft_transition);

    // Record this core as being active now.
    ee_state->num_active_cores++;
    ee_state->stats->active_cores |= (1 << core);
    ee_state->stats->bringup_req = time_service_now ();
    
    // We can't actually acknowledge the bringup here, because we haven't brought
    // the master out of its sleep set.
    ee_state->pending_bringups |= (1 << core);

    // Disable bringup_req as an interrupt, as the signal will stay high.
    interrupt_disable(spm->bringupInts[core]);
    
    // Kick off whatever other action needs to happen due to this event.
    rpm_spm_state_machine(ee, SPM_BRINGUP_REQ);

    INTFREE();
}

static void rpm_spm_shutdown_high_isr(void) __irq
{
    INTLOCK();

    unsigned isrNum = interrupt_current_isr();
    unsigned ee     = spm_isr_context[isrNum].ee;
    unsigned core   = spm_isr_context[isrNum].core;
    unsigned active_cores;
    
    EEData               *ee_state = &(rpm->ees[ee]);
    const SPMDescription *spm      = &(ee_state->spm);

    CORE_VERIFY(ee < 0xFF);

    // A core turned off, so record that.
    ee_state->num_active_cores--;
    active_cores = ee_state->stats->active_cores;
    active_cores &= ~(1 << core);
    ee_state->stats->active_cores = active_cores;
    if (ee_state->num_active_cores == 0)
    {
      ee_state->stats->num_shutdowns ++;
      ee_state->stats->shutdown_req = time_service_now ();
    }
    SWEVENT(RPM_SHUTDOWN_REQ, ee, core, ee_state->is_soft_transition);

    // Acknowledge the shutdown by clearing bringup ack and setting shutdown ack.
    if(!ee_state->is_soft_transition)
    {
        HWIO_OUT(RPM_GPO_WDCLR, (1 << (spm->bringupAcks[core])));  // clear bringup_ack
        HWIO_OUT(RPM_GPO_WDSET, (1 << (spm->shutdownAcks[core]))); // set shutdown_ack
    }
    else
    {
        //send soft ACK
        rpm->ees[ee].handler->writeResponse(SPM_SHUTDOWN_ACK, 0, RPM_COMMAND_SERVICE);
    }

    // Wait for the shutdown to go through.
    interrupt_set_isr(spm->shutdownInts[core], rpm_spm_shutdown_low_isr);
    interrupt_configure(spm->shutdownInts[core], LEVEL_LOW);
    
    INTFREE();
}

static void rpm_spm_shutdown_low_isr(void) __irq
{
    INTLOCK();

    unsigned isrNum = interrupt_current_isr();
    unsigned ee     = spm_isr_context[isrNum].ee;
    unsigned core   = spm_isr_context[isrNum].core;

    EEData               *ee_state = &(rpm->ees[ee]);
    const SPMDescription *spm      = &(ee_state->spm);

    CORE_VERIFY(ee < 0xFF);

    SWEVENT(RPM_SHUTDOWN_ACK, ee, core, ee_state->is_soft_transition);

    // We're shutdown, switch over to sleep mode.
    if(!ee_state->is_soft_transition)
    {
        HWIO_OUT(RPM_GPO_WDCLR, (1 << (spm->shutdownAcks[core]))); // clear shutdown_ack
        interrupt_enable(spm->bringupInts[core]); // enable bringup_req ISR
    }

    // Reset shutdown_req monitoring for after the wakeup.
    interrupt_configure(spm->shutdownInts[core], LEVEL_HIGH);
    interrupt_set_isr(spm->shutdownInts[core], rpm_spm_shutdown_high_isr);

    // Kick off whatever other action needs to happen due to this event.
    rpm_spm_state_machine(ee, SPM_SHUTDOWN_REQ);

    INTFREE();
}

void rpm_spm_soft_transition( unsigned ee, rpm_spm_entry_reason reason)
{
  EEData *ee_state              = &(rpm->ees[ee]);
  const SPMDescription *spm     = &(ee_state->spm);
  
  //set flag, soft trigger interrupt
  ee_state->is_soft_transition = true;
  switch(reason)
  {
      case SPM_SHUTDOWN_REQ:
          interrupt_soft_trigger(spm->shutdownInts[0]);
          break;
      case SPM_BRINGUP_REQ:
          if((ee_state->subsystem_status != SPM_AWAKE) && (ee_state->subsystem_status != SPM_WAKING_UP))
          {		  
            interrupt_soft_trigger(spm->bringupInts[0]);
            interrupt_enable(spm->bringupInts[0]); // enable bringup_req ISR
          }	    
          break;

      default:
          abort();
  }    
}

void rpm_spm_init(void)
{
    unsigned num_ees = rpm->num_ees;

    unsigned highestISR = 0;
    for(unsigned i = 0; i < num_ees; ++i)
    {
        EEData               *ee       = &(rpm->ees[i]);
        const SPMDescription *spm      = &(ee->spm);
        unsigned              numCores = spm->numCores;
        unsigned active_cores = 0;
        
        // All cores start "awake."
        ee->num_active_cores = numCores;
        ee->pending_bringups = 0;
        ee->subsystem_status = SPM_AWAKE;
        ee->is_soft_transition = false;
        
        for(unsigned core = 0; core < numCores; ++core)
        {
            unsigned bringupISR = spm->bringupInts[core];
            if(bringupISR > highestISR) highestISR = bringupISR;
            interrupt_configure(bringupISR, LEVEL_HIGH);
            interrupt_set_isr(bringupISR, rpm_spm_bringup_isr);

            unsigned shutdownISR = spm->shutdownInts[core];
            if(shutdownISR > highestISR) highestISR = shutdownISR;
            interrupt_configure(shutdownISR, LEVEL_HIGH);
            interrupt_set_isr(shutdownISR, rpm_spm_shutdown_high_isr);
            interrupt_enable(shutdownISR);

            active_cores |= (1 << core);
            ee->stats->active_cores = active_cores;
        }
    }

    // Allocate context blocks for each ISR.
    ++highestISR;
    spm_isr_context = new rpm_spm_isr_context_t[highestISR];
    memset(spm_isr_context, 0xFF, highestISR * sizeof(rpm_spm_isr_context_t));
    for(unsigned i = 0; i < num_ees; ++i)
    {
        EEData               *ee       = &(rpm->ees[i]);
        const SPMDescription *spm      = &(ee->spm);
        unsigned              numCores = spm->numCores;
        
        for(unsigned core = 0; core < numCores; ++core)
        {
            spm_isr_context[spm->shutdownInts[core]].ee = i;
            spm_isr_context[spm->shutdownInts[core]].core = core;

            spm_isr_context[spm->bringupInts[core]].ee = i;
            spm_isr_context[spm->bringupInts[core]].core = core;
        }
    }
}

void rpm_spm_trigger_wakeup_int(unsigned ee)
{
    if(!rpm->ees[ee].is_soft_transition)
    {
        unsigned wakeupInt = rpm->ees[ee].wakeupInt;
        HWIO_OUT(RPM_IPC, wakeupInt);
    }
    else
    {
        rpm_spm_soft_transition(ee, SPM_BRINGUP_REQ);
    }
}

