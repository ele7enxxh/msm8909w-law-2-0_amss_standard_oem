/*===========================================================================
  rpm_set_changer.cpp

  SERVICES:

  DESCRIPTION:

  INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

  Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government. Diversion contrary to U.S. law prohibited.
  ===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include <assert.h>
#include <string.h>
#include <alloca.h>
#include "rpm_set_changer.h"
#include "rpmserver_internal.h"
#include "rpm_spm_handshake.h"
#include "rpm_estimator.h"
#include "rpm_hash.h"
#include "swevent.h"
#include "rpm_config.h"
#include "rpm_settling_timer.h"
#include "rpm_resources_config.h"
#include "gpio_debug.h"

#include "time_service.h"
#include "err.h"
#include "CoreVerify.h"

#include "rpm_messageram.h"
#include <stringl/stringl.h>
#include "Clock.h"


//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

#define MAX_WAKEUP_DEADLINE_EXCEEDANCE  38400 /* Ticks in 19.2 MHz, 38400 ~= 2.0ms */

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
struct EstimateContext
{
  unsigned ee;
  rpm_transition_type transition;

  unsigned *approxWorkRemaining;
  unsigned *approxSettlingRemaining;
};

struct TransitionContext
{
  volatile bool &preempt;
  uint64_t      &stop_time;

  unsigned ee;
  rpm_transition_type transition;

  unsigned *approxWorkRemaining;
  unsigned *approxSettlingRemaining;

  TransitionContext(volatile bool &pre, uint64_t &stop, unsigned ee, rpm_transition_type trans, unsigned *approx, unsigned *settling)
      : preempt(pre), stop_time(stop), ee(ee), transition(trans), approxWorkRemaining(approx), approxSettlingRemaining(settling)
  {
  }
};

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static uint32_t      *preProcessResources = NULL;
static uint32_t      *postProcessResources = NULL;
static ResourceData  *last_transitioning_resource;
static boolean        disable_settling = FALSE;

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------

//===========================================================================
//                             Macro Definitions
//===========================================================================

//===========================================================================
//                           Function Definitions
//===========================================================================

/*===========================================================================
  FUNCTION: rpm_set_changer_dep_init

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
static void rpm_set_changer_dep_init(const rpm_resource_config_t *resource_dep, uint32_t processResources[])
{
  if(!resource_dep) return;
  while (resource_dep->type != 0)
  {
    unsigned resource_idx = (rpmserver_lookup_resource(resource_dep->type, resource_dep->id)) - rpm->resources;
    if (resource_idx < rpm->supported_resources)
      processResources[resource_idx/32] |= (1 << (resource_idx % 32));
    resource_dep++;
  }
}

/*===========================================================================
FUNCTION: rpm_set_changer_common_init

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void rpm_set_changer_common_init(void)
{
  uint32_t resource_bmsk_size = (sizeof(uint32_t) * ((rpm->supported_resources + 31) / 32));
  preProcessResources  = (uint32_t *)malloc(resource_bmsk_size);
  postProcessResources = (uint32_t *)malloc(resource_bmsk_size);
  CORE_VERIFY(preProcessResources && postProcessResources);

  memset(preProcessResources, 0, resource_bmsk_size);
  memset(postProcessResources, 0, resource_bmsk_size);

  rpm_set_changer_dep_init(BIMC_PREPROCESS_DEP, preProcessResources);
  rpm_set_changer_dep_init(BIMC_POSTPROCESS_DEP, postProcessResources);
}

/*===========================================================================
  FUNCTION: SetChanger::isPostprocessed

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
inline bool SetChanger::isPostprocessed(ResourceData *r)
{
  unsigned resource_idx = r - rpm->resources;
  if(resource_idx < rpm->supported_resources)
  {
    return ((postProcessResources[resource_idx/32] & (1 << (resource_idx % 32))) != 0);
  }
  return false;
}

/*===========================================================================
  FUNCTION: SetChanger::SetChanger

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
SetChanger::SetChanger(unsigned ee, Handler *handler, uint8_t priority)
    : Task(priority), state_(IDLE), ee_(ee), handler_(handler), currentSet_(RPM_ACTIVE_SET),
      destinationSet_(RPM_ACTIVE_SET), approxWorkRemaining_(0), approxSettlingRemaining_(0), actionHash_(0),
      started_(false), interrupted_(false), startState_(0), startTime_ (0), settlingTime_(0), tranBypassCount_ (0),
      wakeTime_(0), cacheHit_(0)
{
  for(unsigned i = 0; i < settling_bmsk_size; i++)
    settlingBmsk_[i] = 0;
}

/*===========================================================================
  FUNCTION: SetChanger::get_length_given

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
uint64_t SetChanger::get_length_given(uint32_t systemStateIn, uint32_t &systemStateOut)
{
  bool hit = false;
  uint32_t resultState, timeEstimate;

  // Check the appropriate queue for active->sleep (immediate) or sleep->active.
  if(destinationSet_ != RPM_ACTIVE_SET)
  {
    hit = theEstimateCacheImmediate().lookup(systemStateIn, actionHash_, &resultState, &timeEstimate);
  }
  else
  {
    hit = theEstimateCache().lookup(systemStateIn, actionHash_, &resultState, &timeEstimate);
  }

  if(hit) {
    systemStateOut = resultState;
    setCacheHit(1);
    return timeEstimate;
  }

  setCacheHit(0);
  systemStateOut = 0;
  return get_length();
}

EEClient* resource_lookup_and_alloc_client(ResourceData *r, unsigned ee);

/*===========================================================================
  FUNCTION: SetChanger::estimateResource

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
bool SetChanger::estimateResource(ResourceData *r)
{
  bool has_work = false;
  rpm_transition_type transition = (destinationSet_ == RPM_ACTIVE_SET) ? SLEEP_TO_ACTIVE : ACTIVE_TO_SLEEP;

  if(resource_ee_has_transition_work(r, ee_, transition))
  {
    unsigned estimate = resource_estimate(r, theEstimator().currentOperatingPoint());
    approxWorkRemaining_ += estimate;
    has_work = true;

    ResourceClassData *rc = &rpm->classes[r->class_idx];
    unsigned inrep_size = rc->inrep_size;

    char *substate = (char *)alloca(2 * inrep_size);
    EEClient* ee = resource_lookup_and_alloc_client(r, ee_);
    memscpy(substate, inrep_size,   ee->request[0], inrep_size);
    memscpy(substate + inrep_size, inrep_size,  ee->request[1], inrep_size);
    unsigned rotation = r - rpm->resources;
    actionHash_ ^= __ror(rpm_hash(2 * inrep_size, substate), rotation);
  }
  return has_work;
}

/*===========================================================================
  FUNCTION: SetChanger::requeue

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void SetChanger::requeue_settling(uint64_t deadline)
{
  unsigned is_settling = 0;
  //its possible we got an explicit transition before the timer fired. In this case we
  //will have already processed the settling resource, so check to see if there is anything still settling
  for (unsigned i = 0; i < settling_bmsk_size; i++)
  {
    is_settling |= settlingBmsk_[i];
  }

  if(is_settling)
  {
    if(!deadline)
    {
      SWEVENT(RPM_SETTLING_TRANSITION_REQUEUED, ee_, false);
      schedule_me(IGNORE_DEADLINE);
    }
    else
    {
      SWEVENT(RPM_SETTLING_TRANSITION_REQUEUED, ee_, true, (uint32)deadline, (uint32)(deadline >> 32));
      set_deadline(deadline);
      schedule_me();
    }
    state_ = SETTLING;
  }
}


/*===========================================================================
  FUNCTION: SetChanger::enqueue

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void SetChanger::enqueue(rpm_set_type destination_set, uint64_t deadline)
{
  if(state_ == IDLE)
  {
    rpm_transition_type  transition = (destination_set == RPM_ACTIVE_SET) ? SLEEP_TO_ACTIVE : ACTIVE_TO_SLEEP;
    destinationSet_ = destination_set;
    state_ = CHANGE_SETS;

    // Compute what work we'll need to do (how long it will take).
    approxWorkRemaining_ = 0;
    actionHash_ = 0xb7ee364b;
    for(unsigned i = 0; i < rpm->supported_resources; ++i)
    {
      ResourceData *r = &(rpm->resources[i]);

      if(!(r && r->current_state && resource_ee_has_transition_work(r, ee_, transition)))
      {
        continue; // not an initialized resource or no work to do
      }

      if(estimateResource(r))
      {
          approxSettlingRemaining_ = MAX(approxSettlingRemaining_, resource_settling_estimate(r));
      }
    }

    approxWorkRemaining_ += approxSettlingRemaining_;

    // FIXME: need dynamic estimations here - this number seems high
    // About 500 us of overhead for state transitions
    if (approxWorkRemaining_ == 0)
    {
      approxWorkRemaining_ = 9733;
    }

    started_ = false;
    interrupted_ = false;
  }
  else
  {
    CORE_VERIFY(destinationSet_ == destination_set);
  }

  if(!deadline)
  {
    SWEVENT(RPM_TRANSITION_QUEUED, ee_, false);
    schedule_me(IGNORE_DEADLINE);
  }
  else
  {
    SWEVENT(RPM_TRANSITION_QUEUED, ee_, true, (uint32)deadline, (uint32)(deadline >> 32));
    set_deadline(deadline);
    schedule_me();
  }
}

/*===========================================================================
  FUNCTION: SetChanger::hasImmediateWork

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
bool SetChanger::hasImmediateWork() const
{
  const EEData *ee = &(rpm->ees[ee_]);

  bool rudely_awoken = (ee->subsystem_status == SPM_WAKING_UP)      && (state_ != IDLE);
  bool going_to_bed  = (ee->subsystem_status == SPM_GOING_TO_SLEEP) && (state_ != IDLE);

  return rudely_awoken || going_to_bed;
}

/*===========================================================================
  FUNCTION: SetChanger::transitionResource

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
static bool transitionResource(ResourceData *r, void *context, uint64 *settling_time)
{
  TransitionContext *ctxt = (TransitionContext *)context;

  Estimator &estimator = theEstimator();
  unsigned   runtime   = resource_estimate(r, estimator.currentOperatingPoint());
  uint64     observed_settling = 0;

  *settling_time = 0;

  if(ctxt->preempt || ((time_service_now() + runtime) > ctxt->stop_time))
    return false;

  Observation obs = estimator.startObservation();

  *settling_time = resource_ee_transition(r, ctxt->ee, ctxt->transition);
  if(*(ctxt->approxWorkRemaining) > runtime)
    *(ctxt->approxWorkRemaining) -= runtime;
  else
    *(ctxt->approxWorkRemaining) = 0;

  estimator.completeObservation(obs);

  if(*settling_time)
  {
    uint64 now = time_service_now();
    if (disable_settling)
    {
      while(*settling_time)
        *settling_time = resource_ee_settling_apply(r, ctxt->ee, *settling_time);
    }
    else if(*settling_time <= now)
    {
      *settling_time = resource_ee_settling_apply(r, ctxt->ee, *settling_time);
    }
    else
    {
      observed_settling = (*settling_time) - now;
    }
  }

  resource_observe(r, estimator.currentOperatingPoint(), obs, observed_settling);

  return true;
}

/*===========================================================================
FUNCTION: SetChanger::execute_specific_resource

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
inline bool SetChanger::execute_specific_resource(TransitionContext *context, ResourceData *r)
{
  uint64 settling_time = 0;

  if(!resource_ee_has_transition_work(r, ee_, context->transition))
  {
    return true;
  }

  if(!transitionResource(r, context, &settling_time))
  {
    return false;
  }

  if(settling_time)
  {
    unsigned r_index = r - rpm->resources;
    assert((r_index/32) < settling_bmsk_size);
    settlingBmsk_[r_index/32] |= 1 << (r_index%32);
    SWEVENT(RPM_RESOURCE_SETTLING, ee_, (rpm->classes[r->class_idx]).type, r->id, 0);
    SWEVENT(RPM_RESOURCE_SETTLING_TIME, (uint32_t)settling_time, (uint32_t)(settling_time >> 32));
  }
  settlingTime_ = MAX(settling_time, settlingTime_);
  return true;
}

/*===========================================================================
FUNCTION: SetChanger::execute_resource_list

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
inline bool SetChanger::execute_resource_list(TransitionContext *context, uint32_t *resources_bmsk)
{
  for(unsigned i = 0; i < settling_bmsk_size; ++i)
  {
    uint32_t index_bmsk = resources_bmsk[i];
    while(index_bmsk)
    {
      uint32_t index = 31 - __clz(index_bmsk);
      ResourceData *r = &(rpm->resources[index + (32 * i)] );

      index_bmsk &= (~(1 << index));

      if (!execute_specific_resource(context, r))
      {
        return false;
      }
    }
  }
  return true;
}

/*===========================================================================
  FUNCTION: SetChanger::execute_until

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void SetChanger::execute_until(volatile bool &preempt, uint64_t stop_time)
{
  SWEVENT(RPM_MASTER_SET_TRANSITION, ee_, currentSet_, destinationSet_, getCacheHit());

  gpio_test_low((gpio_debug_application)ee_, (gpio_debug_transition)destinationSet_);
  if(!started_)
  {
    started_ = true;
    interrupted_ = false;
    startState_ = rpm_system_hash;
    startTime_ = time_service_now();
  }

  // Note: this switch statement "resumes" from a previous pre-emption,
  // allowing a mechansim that is sometime referred to as "green threads" or
  // "protothreads." The fall-through (in every case block) is intentional
  // and vital to correct functionality.
  switch(state_)
  {
    case CHANGE_SETS:
      {
        currentSet_ = destinationSet_;

        // If we're going to sleep and at least one processor has woken up
        // already, skip directly to post dispatch without completing the
        // transition--we're just going to wake right up again.
        bool completed = true;
        // Visit all resources and place them in the destination set.
        rpm_transition_type  transition = (destinationSet_ == RPM_ACTIVE_SET) ? SLEEP_TO_ACTIVE : ACTIVE_TO_SLEEP;
        TransitionContext context(preempt, stop_time, ee_, transition, &approxWorkRemaining_, &approxSettlingRemaining_);

        if(!isWaking () && !isWakeupNear (&context))
        {
		
		  ResourceData *state_node = rpmserver_lookup_resource(RPM_SUBSYS_STATE_REQ, 0);
		  
		  if (state_node)
		  {
		    execute_specific_resource(&context, state_node);
		  }
		  
          // Visit all resources and place them in the destination set.
          if(destinationSet_ == RPM_SLEEP_SET)
          {
            //if we are going to sleep, process bimc dependencies first
            completed = execute_resource_list(&context, preProcessResources);
            if(!completed)
            {
              interrupted_ = true;
              break;
            }
          }

          ResourceData *bimc = rpmserver_lookup_resource(RPM_CLOCK_2_REQ, 0);

          if(bimc && bimc->current_state)
          {
            //process BIMC
            completed = execute_specific_resource(&context, bimc);
            if(!completed)
            {
              interrupted_ = true;
              break;
            }
          }
          
		  ResourceData *ldo_10 = rpmserver_lookup_resource(RPM_LDO_A_REQ, 10);

	      if (ldo_10 && completed)
	      {
	        completed = execute_specific_resource(&context, ldo_10);

            if(!completed)
            {
              interrupted_ = true;
              break;
            }
	      }

	  
          // process everything else
          for(unsigned i = 0; i < rpm->supported_resources; ++i)
          {
            ResourceData *r = &(rpm->resources[i]);

            if(!(r && r->current_state))
            {
              continue; // not an initialized resource, doesn't have transition work, or was one of the preProcessed resources
            }

            // Defer MX as the last resource request
            if (r == last_transitioning_resource)
            {
              continue;
            }

            completed = execute_specific_resource(&context, r);

            if(!completed)
            {
              interrupted_ = true;
              break;
            }
          }

          // Process mx if applicable
          if (last_transitioning_resource && completed)
          {
            completed = execute_specific_resource(&context, last_transitioning_resource);
            if(!completed)
            {
              interrupted_ = true;
              break;
            }
          }

          if(bimc && bimc->current_state)
          {
            uint64_t settling_time = resource_ee_settling_query(bimc, ee_);
            if(settling_time)
            {
              unsigned index = bimc - rpm->resources;
              assert((index/32) < settling_bmsk_size);
              settlingBmsk_[index/32] |= 1 << (index%32);
              SWEVENT(RPM_RESOURCE_SETTLING, ee_, (rpm->classes[bimc->class_idx]).type, bimc->id, 0);
              SWEVENT(RPM_RESOURCE_SETTLING_TIME, (uint32)settling_time, (uint32)(settling_time >> 32));
            }
            settlingTime_ = MAX(settling_time, settlingTime_);
          }
        }
        else
        {
          completed = true;
          interrupted_ = true;
          tranBypassCount_ ++;
          SWEVENT (RPM_MASTER_SET_TRANSITION_BYPASS, (ee_ << 16 | rpm->ees[ee_].pending_bringups),
                   (uint32_t)(wakeTime_ >> 32), (uint32_t)wakeTime_, approxWorkRemaining_);
        }

        if(completed)
        {
          state_ = NAS_WORK;
        }
        else
        {
          break;
        }
      }

    case NAS_WORK:
      {
        if((RPM_SLEEP_SET != destinationSet_) || (handler_->processNAS(preempt, stop_time)))
        {
          if(!settlingTime_)
          {
            approxSettlingRemaining_ = 0;
            state_ = CLEANUP;
          }
          else
          {
            state_ = SETTLING;
          }
        }
        else
        {
            break;
        }
      }

    case SETTLING:
      {
        if(state_ == SETTLING)
        {
          uint64 new_time = 0;
          for(unsigned i = 0; i <= (rpm->supported_resources / 32); ++i)
          {
            while (settlingBmsk_[i])
            {
              uint32 index = 31 - __clz(settlingBmsk_[i]);
              ResourceData *r = &(rpm->resources[(i*32) + index]);
              new_time = resource_ee_settling_apply(r, ee_, settlingTime_);

              if(!new_time)
              {
                settlingBmsk_[i] &= (~(1 << index));
                SWEVENT(RPM_RESOURCE_SETTLING_COMPLETE, ee_, (rpm->classes[r->class_idx]).type, r->id);
              }
            }
          }
          approxSettlingRemaining_ = 0;
          settlingTime_ = 0;
          state_ = CLEANUP;
        }
      }

    case CLEANUP:
      {

        bool from_sleep = (RPM_SLEEP_SET != destinationSet_);

        uint64_t now = time_service_now();

        // We're all done, so reset our internal state.
        set_end(now);
        uint64_t old_deadline = 0;
        bool deadline_valid = get_deadline (old_deadline);
        clear_deadline();
        state_ = IDLE;
        settlingTime_ = 0;
        approxSettlingRemaining_ = 0;

        SWEVENT(RPM_MASTER_SET_TRANSITION_COMPLETE, ee_, (uint32)(old_deadline>>32), (uint32)(old_deadline), (old_deadline && (now > old_deadline)));

        gpio_test_high((gpio_debug_application)ee_,(gpio_debug_transition)destinationSet_);
        uint64_t elapsed = now - startTime_;

        if (! interrupted_)
        {
          if (RPM_ACTIVE_SET == destinationSet_)
          {
            rpm->ees[ee_].stats->last_wake_transition_duration = (uint32_t)elapsed;
            theEstimateCache().observe(startState_, actionHash_, rpm_system_hash, elapsed, true);
          }
          else
          {
            rpm->ees[ee_].stats->last_sleep_transition_duration = (uint32_t)elapsed;
            theEstimateCacheImmediate().observe(startState_, actionHash_, rpm_system_hash, elapsed, true);
          }
        }


        if(from_sleep)
        {
          wakeTime_ = 0;
          // Also, send an interrupt ensure the other side wakes up in
          // the "timed wakeup" case.
          if(SPM_SLEEPING == rpm->ees[ee_].subsystem_status)
          {
            // coming from sleep with a scheduled wake up so set wakeup reason to 1 or SCHD_WAKEUP
            (message_ram_master_stats(ee_))->wakeup_reason = 1;
            rpm->ees[ee_].stats->wakeup_ind = now;
            rpm_spm_trigger_wakeup_int(ee_);

            if(deadline_valid && old_deadline && (now > old_deadline) && ((now - old_deadline) > MAX_WAKEUP_DEADLINE_EXCEEDANCE))
            {
              //ERR_FATAL("Deadline latency exceeded", (uint32_t)this, 0, 0);
            }
          }
        }
        else
        {
          // going to sleep so set wakeup reason to be 0, or RUDE_WAKEUP
          (message_ram_master_stats(ee_))->wakeup_reason = 0;
        }

        // We just changed set, so the SPM state machine may need to advance.
        rpm_spm_state_machine(ee_, SPM_TRANSITION_COMPLETE);
      }

    default:
      break;
  }
	
  if(state_ != IDLE)
  {
    interrupted_ = true;

    gpio_test_high((gpio_debug_application)ee_,(gpio_debug_transition)destinationSet_);
  }
}

/*===========================================================================
  FUNCTION: bool SetChanger::isWaking () const

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
inline bool SetChanger::isWaking () const
{
  return ((RPM_SLEEP_SET == destinationSet_) && (rpm->ees[ee_].pending_bringups));
}

/*===========================================================================
  FUNCTION: bool SetChanger::isWakeupNear () const

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
bool SetChanger::isWakeupNear (const TransitionContext *context) const
{
  bool ret = false;

  if ((context->transition == ACTIVE_TO_SLEEP) && (wakeTime_ > 0))
  {
    uint64_t now = time_service_now ();
    uint32_t timeEstimate = (*context->approxWorkRemaining);

    // Check if there's sufficient time to process the work. We use 2 times work estimate for
    // A->S and S->W.

    // If there's a cache hit, use the cache estimate. Else use the worst case (approxWorkRemaining).
    if (cacheHit_)
    {
      uint32_t resultState, timeEstimate = (*context->approxWorkRemaining);
      if(destinationSet_ != RPM_ACTIVE_SET)
      {
        theEstimateCacheImmediate().lookup(startState_, actionHash_, &resultState, &timeEstimate);
      }
      else
      {
        theEstimateCache().lookup(startState_, actionHash_, &resultState, &timeEstimate);
      }
    }
    ret = (now >= wakeTime_) || ((wakeTime_ - now) < (timeEstimate << 1));
  }

  return ret;
}

void SetChanger::set_last_transitioning_resource(ResourceData* resource)
{
  //This function can't be called twice.
  CORE_VERIFY(!last_transitioning_resource);
  last_transitioning_resource = resource;
}

