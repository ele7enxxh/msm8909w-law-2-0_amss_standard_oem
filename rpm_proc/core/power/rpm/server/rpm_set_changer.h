#ifndef RPM_SET_CHANGER_H
#define RPM_SET_CHANGER_H
/*===========================================================================
                             rpm_set_changer.h

SERVICES:

DESCRIPTION:
  Verbose description of what is defined by this header file. Typically
  describes the service provided by the classes, macros, etc. defined in
  this file.

PUBLIC CLASSES AND FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#ifdef __cplusplus

#include "rpm.h"
#include "rpm_task.h"
#include "rpm_handler.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define settling_bmsk_size 5

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------
struct TransitionContext;

//===========================================================================
//                             Class Definitions
//===========================================================================

class SetChanger : public Task
{
public:
    SetChanger(unsigned ee, Handler *handler, uint8_t priority = 127);

    // This set change has all the required information, and should be placed in the schedule.
    void enqueue(rpm_set_type destination_set, uint64_t deadline);

    void requeue_settling(uint64_t deadline);

    // Returns the worst-case duration of processing that this set change might
    // consume.  Valid only after calling enqueue and before the changeover
    // completes.
    virtual uint64_t get_length() const { return approxWorkRemaining_; }

    virtual uint64_t get_length_given(uint32_t systemStateIn, uint32_t &systemStateOut);

    virtual bool hasImmediateWork() const;

    bool inTransition() const { return (state_ != IDLE); }

    rpm_set_type currentSet() const { return currentSet_; }

    bool isWaking () const;
    bool isWakeupNear (const TransitionContext *) const;

    void setWakeTime (uint64_t t) {wakeTime_ = t;}
    uint64_t getWakeTime () const {return wakeTime_;}

    void setCacheHit (bool h) {cacheHit_ = h;}
    bool getCacheHit () {return cacheHit_;}

    static void set_last_transitioning_resource(ResourceData* resource);

protected:
    // Actual implementation of the set change logic.
    virtual void execute_until(volatile bool &preempt, uint64_t stop_time);

    bool execute_specific_resource(TransitionContext *context, ResourceData *r);

    bool execute_resource_list(TransitionContext *context, uint32_t *resources_bmsk);

private:
	bool isPostprocessed(ResourceData *r);
    bool estimateResource(ResourceData *r);

    enum ExecutionState
    {
        IDLE,
        CHANGE_SETS,
        NAS_WORK,
        SETTLING,
        CLEANUP,
    };

    // the current state of this request, used to resume after preemption
    ExecutionState            state_;

    unsigned     ee_;
    Handler     *handler_;
    rpm_set_type currentSet_;
    rpm_set_type destinationSet_;
    unsigned     approxWorkRemaining_;
    unsigned     approxSettlingRemaining_;
    uint32_t     actionHash_;

    bool         started_;
    bool         interrupted_;
    uint32_t     startState_;
    uint64_t     startTime_;
    uint64_t     settlingTime_;
    uint32_t     settlingBmsk_[settling_bmsk_size];

    uint32_t     tranBypassCount_;

    // The deadline for a transition from sleep to active,
    // only valid in the context of a set transition from active to sleep
    uint64_t wakeTime_;

    // Note if the current time estimate is from the estimate cache or not
    bool        cacheHit_;
};

#endif // __cplusplus

#endif // RPM_SET_CHANGER_H

