/*==============================================================================

FILE:      rpm_log.h

DESCRIPTION: Function and data structure declarations

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //components/rel/rpm.bf/2.1.1/core/bsp/rpm/inc/rpm_log.h#1 $
$Date: 2015/02/25 $

==============================================================================*/

#ifndef RPM_LOG_H
#define RPM_LOG_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "comdef.h"

#define EXTEND_ARGS(...) \
         EXTEND_ARGS_(0, ##__VA_ARGS__, FILLER_ARGS())
#define EXTEND_ARGS_(...) \
         SELECT_ARGS(__VA_ARGS__)
#define SELECT_ARGS(_ignore, _1, _2, _3, _4, ...) (uint32)_1, (uint32)_2, (uint32)_3, (uint32)_4
#define FILLER_ARGS() 0, 0, 0, 0

// Macro to call to log an event to the externally visible RPM log.
//
// Usage is RPM_LOG_EVENT(<id>[, arg1, arg2, ..., argn]) where <id> is from
// the list below, and the number of arguments (if any) is determined by
// the ID's expected format (listed on the line above ID).
//
// Maximum number of arguments is 4, all arguments must be transparently
// castable into a uint32.
//#define RPM_LOG_EVENT(id, ...) rpm_log_event_id4((id), ##EXTEND_ARGS(__VA_ARGS__))
#define RPM_LOG_EVENT(id, ...)

// Log message IDs.

// Indicates that the RPM has started its bootup sequence.
// Arguments: none
#define RPM_BOOT_STARTED 0x0

// Indicates that the RPM has completed its bootup sequence.
// Arguments: 1: non-zero if bootup failed
#define RPM_BOOT_FINISHED 0x1

// Indicates that a driver has claimed ownership of an RPM resource ID.
// Arguments: 1: the RPM resource ID, 2: '1' if an NPA driver was registered
#define RPM_DRIVER_REGISTERED 0x2

// Indicates that a driver has relinquished ownership of an RPM resource ID.
// Arguments: 1: the RPM resource ID
#define RPM_DRIVER_DEREGISTERED 0x3

// Indicates that a driver is updating a status field.
// Arguments: 1: the RPM resource ID, 2: '1' when status starts update, '0' when finished
#define RPM_STATUS_UPDATING 0x4

// Indicates that a master's message interrupt has been received.
// Arguments: 1: the ID of the master
#define RPM_MESSAGE_INT_RECEIVED 0x5

// Indicates that a master's request is being serviced.
// Arguments: 1: the ID of the master
#define RPM_SERVICING_MASTER 0x6

// Indicates that a master's request was not processed
// Arguments: 1: a reason from the set listed below
#define RPM_REQUEST_ABORTED 0x7
  #define RPM_VERSION_MISMATCH 0x0
  #define RPM_INVALID_PARAMETERS 0x1
  #define RPM_MASTER_NOT_READY 0x2

// Indicates a reserved field was populated with data.
// Arguments: none
#define RPM_RESERVED_FIELD_WARNING 0x8

// Indicates a resource request is being dispatched to a driver.
// Arguments: 1: the resource being dispatched
#define RPM_DRIVER_DISPATCH 0x9

// Indicates a resource request is being dispatched to a driver.
// Arguments: 1: '1' if rejected by driver
#define RPM_DRIVER_COMPLETE 0xa

// Indicates there was a request to a resource ID that is not managed.
// Arguments: none
#define RPM_NO_DRIVER 0xb

// Indicates the RPM is sending an acknowledge interrupt to a master.
// Arguments: 1: the master's ID
#define RPM_SENDING_MESSAGE_INT 0xc

// Indicates a master is being nonconformant and that it will be serviced
// on a polled timetable.
// Arguments: 1: the master's ID, 2: actions being deferred, 3: number of deferrals so far
#define RPM_MASTER_DEFERRED 0xd

// Indicates a master has been deemed to be in conformance once again.
// Arguments: 1: the master's ID
#define RPM_MASTER_DEFERRAL_COMPLETE 0xe

// Indicates a trigger interrupt has fired
// Arguments: 1: the interrupt id
#define RPM_TRIGGER_FIRED 0xf

// Indicates a trigger timer expired
// Arguments: 1: the master who's timer expired
#define RPM_TIMED_TRIGGER_FIRED 0x10

// Indicates a master has signalled bringup_req
// Arguments: 1: the master waking up
#define RPM_BRINGUP_REQ 0x11

// Indicates a master has signalled shutdown_req
// Arguments: 1: the master shutting down
#define RPM_SHUTDOWN_REQ 0x12

// Indicates the RPM has signalled bringup_ack
// Arguments: 1: the master being acknowledged
#define RPM_BRINGUP_ACK 0x13

// Indicates the RPM has signalled shutdown_ack
// Arguments: 1: the master being acknowledged
#define RPM_SHUTDOWN_ACK 0x14

// Indicates the RPM is moving a master between sets.
// Arguments: 1: the master being transitioned, 2: the set the master is leaving, 3: the set the master is moving to
#define RPM_MASTER_SET_TRANSITION 0x15

// Indicates a driver rejection occured during set transition.  I don't think this should ever happen, but it's a big deal if it does.
// Arguments: 1: the resource that failed
#define RPM_SET_TRANSITION_FAILURE 0x16

// Indicates the RPM has encountered a fatal error.
// Arguments: 1: pc of caller, 2: param a, 3: param b, 4: param c
#define RPM_ERROR_FATAL 0x17

// Indicates a resource request invalidation left no valid request for the resource.
// Arguments: none
#define RPM_INVALIDATE_TO_NOTHING 0x18

// Indicates a resource request invalidation forced a new request to be dispatched to a driver.
// Arguments: 1: the resource being invalidated
#define RPM_INVALIDATED_DRIVER_DISPATCH 0x19

// Indicates a resource request invalidation driver dispatch returned.
// Arguments: 1: '1' if rejected by driver
#define RPM_INVALIDATED_DRIVER_COMPLETE 0x1a

// Indicates a resource changed notification is going out to a master.
// Arguments: 1: the master being notified
#define RPM_OUTGOING_NOTIFICATION 0x1b

// Indicates that a request is being "faked" for a master-resource pair.
// Arguments: 1: the master being soft-triggered
// Arguments: 2: the resource being soft-triggered
#define RPM_SOFT_REQUEST_INITIATED 0x1c

// Indicates the RPM is entering or leaving an XO shutdown state.
// Arguments: 1: '0' for entering, '1' for leaving
#define RPM_XO_SHUTDOWN 0x1d

// Indicates the RPM is entering or leaving a Vdd Minimzation state.
// Arguments: 1: '0' for entering, '1' for leaving
#define RPM_VDD_MIN 0x1e

// Pending MPM wakeup interrupts dump upon exiting an MPM sleep
// Arguments: 1: MPM_INT_STATUS_1, MPM_INT_STATUS_2
#define RPM_MPM_WAKEUP_INTS 0x1f

// A master was queued for request processing
// Arguments: 1: master_id, 2: insertion position
#define RPM_REQUEST_QUEUED 0x20

//RPM has determined that we will not sleep
// Arguments: 1: sleep path designation 
// Arguments for path designation 0 -> 2: pending interrupts
// Arguments for path designation 1 -> 2: hard duration 3: latency 
#define RPM_NO_SLEEP 0x21

// A master's set transition was enqueued for processing
// Arguments: 1: master_id, 2: scheduled?
// Additional if scheduled -> 3: deadline
#define RPM_TRANSITION_QUEUED 0x22

// A master's set transition has completed
// Arguments: 1: master_id
#define RPM_MASTER_SET_TRANSITION_COMPLETE 0x23

// A resource has been observed with a new maximum worst case
// Argument 1: estimate type
//      estimate type 0 -> Argument 2: resource number
//      estimate type 1 -> Argument 2: quantity number
// Argument 3: worst case measurement
#define RPM_NEW_WORST_CASE 0x24

// A set transition has been planned for a particular time.
// Argument 1: the master that planned the transition
// Argument 2: the set the transition is from
// Argument 3: the set the transition is to
// Argument 4: the deadline by which the transition shall be completed
#define RPM_TIMED_TRANSITION 0x25

// Indicates the RPM is going into / coming out of simple halt
// Argument 1: 0 for entering, 1 for exiting
// Argument 2: (only if entering) planned duration
#define RPM_HALT 0x26

#define RPM_TIMETICK_ROLLOVER 0x27
#define RPM_BAD_ROLLOVER 0x28

#define RPM_SVS_FAST 0x29
#define RPM_SVS_SLOW 0x2a

// Big Block o' NPA Log "Request" Messages
// These messages equate to other 'standard' NPA log messages.
// Note: we do not track creation-time messages at this time.
#define RPM_NPA_REQUEST_COMPLETE           0x1000
#define RPM_NPA_ASSIGN_RESOURCE_STATE      0x1001
#define RPM_NPA_ISSUE_LIMIT_MAX_REQUEST    0x1002
#define RPM_NPA_ISSUE_REQUIRED_REQUEST     0x1003
#define RPM_NPA_MODIFY_REQUEST             0x1004
#define RPM_NPA_ISSUE_IMPULSE_REQUEST      0x1005
#define RPM_NPA_ISSUE_VECTOR_REQUEST       0x1006
#define RPM_NPA_ISSUE_INTERNAL_REQUEST     0x1007
#define RPM_NPA_COMPLETE_REQUEST           0x1008
#define RPM_NPA_CANCEL_REQUEST             0x1009
#define RPM_NPA_QUEUE_PREEMPTIVE_FLUSH     0x100a
#define RPM_NPA_ISSUE_SUPPRESSIBLE_REQUEST 0x100b

extern int rpm_log_init(void);
extern void rpm_log_event_id4(uint32 id, uint32 d0, uint32 d1, uint32 d2, uint32 d3);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif

