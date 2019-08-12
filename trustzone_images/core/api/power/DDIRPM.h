#ifndef __DDIRPM_H__
#define __DDIRPM_H__

#include "DalDevice.h"
#include "RPMTypes.h"

/***********************************
 *         DAL boilerplate         *
 ***********************************/

#define DALRPM_INTERFACE_VERSION DALINTERFACE_VERSION(0,0)

typedef struct DalRPM DalRPM;
struct DalRPM
{
   DalDevice DalDevice;
   DALResult (*SendRequest)(DalDeviceHandle * _h, DAL_rpm_ResourceType  Resource, uint32  DataLen, void * Data);
   DALResult (*CreateMessage)(DalDeviceHandle * _h, DAL_rpm_MessageType * MessageHandle);
   DALResult (*DestroyMessage)(DalDeviceHandle * _h, DAL_rpm_MessageType  MessageHandle);
   DALResult (*AppendRequest)(DalDeviceHandle * _h, DAL_rpm_MessageType  MessageHandle, DAL_rpm_ResourceType  Resource, uint32  DataLen, void * Data);
   DALResult (*SendMessage)(DalDeviceHandle * _h, DAL_rpm_MessageType  MessageHandle);
   DALResult (*ConfigureSet)(DalDeviceHandle * _h, DAL_rpm_ConfigSetType  ConfigSet);
   DALResult (*SetAsyncMode)(DalDeviceHandle * _h, DALSYSEventHandle  CompletedEvent, DALBOOL  Polling);
   DALResult (*RegisterForNotification)(DalDeviceHandle * _h, DAL_rpm_ResourceType  Resource, DALSYSEventHandle  UpdatedEvent);
   DALResult (*GetStatus)(DalDeviceHandle * _h, DAL_rpm_ResourceType  Resource, uint32  DataLen, void * Data);
   DALResult (*SetTrigger)(DalDeviceHandle * _h, DAL_rpm_ConfigSetType  FromSet, DAL_rpm_ConfigSetType  ToSet, DAL_rpm_InterruptType  Interrupts);
   DALResult (*ClearTrigger)(DalDeviceHandle * _h, DAL_rpm_ConfigSetType  FromSet, DAL_rpm_ConfigSetType  ToSet, DAL_rpm_InterruptType  Interrupts);
   DALResult (*SetTimedTrigger)(DalDeviceHandle * _h, DAL_rpm_ConfigSetType  FromSet, DAL_rpm_ConfigSetType  ToSet, uint32  DurationSclks);
   DALResult (*GetEnumByName)(DalDeviceHandle * _h, const char *Name, DAL_rpm_ResourceType *Resource);
   DALResult (*GetResourceSize)(DalDeviceHandle * _h, DAL_rpm_ResourceType Resource, uint32 *ResourceSize);
};

typedef struct DalRPMHandle DalRPMHandle; 
struct DalRPMHandle 
{
   uint32 dwDalHandleId;
   const DalRPM * pVtbl;
   void * pClientCtxt;
};

#define DAL_RPMDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALRPM_INTERFACE_VERSION,hDalDevice)


/***********************************
 *            Requests             *
 ***********************************/

/* General blanket statement for requests: acknowledgements are returned in the
 * very same structure you pass the request in.  This works on the assumption
 * that the ack mirrors the request on an accepted request, and resembles a
 * valid request on a rejected request. */

/* Sends request to RPM. Which set the request is applied to and whether this
 * call blocks is determined by the client state (see below for the functions
 * used to manipulate your driver's state, however the default configuration
 * is a synchronous call).  The request is sent immediately. */
static __inline DALResult
DalRPM_SendRequest(DalDeviceHandle * _h, DAL_rpm_ResourceType  Resource, uint32  DataLen, void * Data)
{
   return ((DalRPMHandle *)_h)->pVtbl->SendRequest( _h, Resource, DataLen, Data);
}


/***********************************
 *       Multi-part Requests       *
 ***********************************/

/* Allocate a workspace where a multiple-request message can be built. */
static __inline DALResult
DalRPM_CreateMessage(DalDeviceHandle * _h, DAL_rpm_MessageType * MessageHandle)
{
   return ((DalRPMHandle *)_h)->pVtbl->CreateMessage( _h, MessageHandle);
}

/* Destroy a workspace previously allocated with CreateMessage.  The handle
 * 'MessageHandle' should not be used after this call. */
static __inline DALResult
DalRPM_DestroyMessage(DalDeviceHandle * _h, DAL_rpm_MessageType  MessageHandle)
{
   return ((DalRPMHandle *)_h)->pVtbl->DestroyMessage( _h, MessageHandle);
}

/* Appends a request to the message represented by MessageHandle (previously
 * allocated by CreateMessage).  This function will overwrite any previously
 * appended requests for the same resource.  The request is not sent to the
 * RPM until SendMessage is called. */
static __inline DALResult
DalRPM_AppendRequest(DalDeviceHandle * _h, DAL_rpm_MessageType  MessageHandle, DAL_rpm_ResourceType  Resource, uint32  DataLen, void * Data)
{
   return ((DalRPMHandle *)_h)->pVtbl->AppendRequest( _h, MessageHandle, Resource, DataLen, Data);
}

/* Send the message represented by MessageHandle (previously allocated by
 * CreateMessage) to the RPM.  Which set the request is applied to and
 * whether this call blocks until acknowledged is determined by the client
 * state, set using the functions below.
 *
 * Note: sending a message virtually 'clears' a message's contents.  If you
 * need to send the same request a second time, you will have to re-append it
 * into the message to get it flagged as 'dirty' before sending again.  A
 * client configuration might be available to modify this behavior at a later
 * date, please contact the author if you need it. */
static __inline DALResult
DalRPM_SendMessage(DalDeviceHandle * _h, DAL_rpm_MessageType  MessageHandle)
{
   return ((DalRPMHandle *)_h)->pVtbl->SendMessage( _h, MessageHandle);
}


/***********************************
 *    Client (per-driver) state    *
 ***********************************/

/* First, a word about configuration sets.
 *
 * There can be 'n' configuration sets in a given system (where 'n' varies by
 * target), and a set can be configured to be entered or left upon a certain
 * trigger (e.g. an interrupt or timer).  These sets can be pre-filled with
 * requests for servicing a specific use case or mode.
 *
 * However, certain sets serve special purposes above and beyond merely being
 * a set of cached requests.  The first active set, referred to as
 * DAL_RPM_CONFIG_SET_ACTIVE_0, is considered the "primary" set.  When a driver
 * on the RPM is evaluating whether enough resources are available to satisfy
 * a request, the resources are available only if the primary requests from all
 * other masters allow for it.  Thus, the primary request for a given resource
 * should always be the "most intensive" request, to assure that requests to
 * other sets can be satisfied.
 *
 * Additionally, if a set other than the primary set becomes active, but no
 * request has been set up for a resource in that set, the set inherits the
 * current request configured for that resource in the primary set. */

/* Change which configuration set requests from this client are directed to.
 * Most drivers will not need this function, as intially the configuration
 * is always to configure the primary set. */
static __inline DALResult
DalRPM_ConfigureSet(DalDeviceHandle * _h, DAL_rpm_ConfigSetType  ConfigSet)
{
   return ((DalRPMHandle *)_h)->pVtbl->ConfigureSet( _h, ConfigSet);
}

/* Configure requests from this client to be either blocking or asynchronous.
 * If a valid event handle is passed for CompletedEvent, all future requests
 * (SendRequest, SendMessage) will be completed asynchronously and the event
 * triggered when the acknowledgement has been received.
 *
 * If CompletedEvent == NULL, future calls will be blocking (synchronous).
 *
 * If Polling == TRUE, CompletedEvent is ignored, the call is blocking, and
 * the RPM response interrupt is polled for in-function.  This mode should
 * only be used if interrupts are locked at the time of request. */
static __inline DALResult
DalRPM_SetAsyncMode(DalDeviceHandle * _h, DALSYSEventHandle  CompletedEvent, DALBOOL  Polling)
{
   return ((DalRPMHandle *)_h)->pVtbl->SetAsyncMode( _h, CompletedEvent, Polling);
}


/***********************************
 *          Notification           *
 ***********************************/

/* Client passes a valid event UpdatedEvent which will be triggered whenever
 * a status change in the requested Resource is received. */
static __inline DALResult
DalRPM_RegisterForNotification(DalDeviceHandle * _h, DAL_rpm_ResourceType  Resource, DALSYSEventHandle  UpdatedEvent)
{
   return ((DalRPMHandle *)_h)->pVtbl->RegisterForNotification( _h, Resource, UpdatedEvent);
}


/***********************************
 *         Status Lookup           *
 ***********************************/

/* Retrieves the status of a resource and copies it into the user provided
 * buffer.  If the length of the buffer is insufficient, error is returned. */
static __inline DALResult
DalRPM_GetStatus(DalDeviceHandle * _h, DAL_rpm_ResourceType  Resource, uint32  DataLen, void * Data)
{
   return ((DalRPMHandle *)_h)->pVtbl->GetStatus( _h, Resource, DataLen, Data);
}


/***********************************
 *         Trigger Config          *
 ***********************************/
/* Triggers are the mechanism used to switch between configuration sets.  There
 * are two basic kinds of triggers: interrupt-based triggers, and timer-based
 * triggers.
 *
 * 
 * == Interrupt-Based Triggers ==
 *
 * Interrupt-based triggers are set and cleared through the SetTrigger and
 * ClearTrigger functions below.  Their last parameter is a
 * DAL_rpm_InterruptType which represents the mask of all interrupts that
 * can potentially be supported by the RPM for triggering.
 *
 * Issuing one of these requests will basically send a request to the RPM
 * to set up the trigger.  The RPM can deny the trigger request for 3 possible
 * reasons:
 *   1) The interrupt being requested is an interrupt that the requesting
 *      master does not have permission to monitor.  Even though all possible
 *      interrupts are represented by the InterruptType, the RPM will filter
 *      the requested trigger interrupts so that security is maintained.
 *
 *   2) The interrupt being requested cannot be used to service the transition
 *      between the requested sets.  For example, an interrupt that is mapped
 *      to the MPM can only be used for a transition from the sleep set to an
 *      active set--since the RPM doesn't have the interrupt mapped directly,
 *      it cannot be used for an active set to active set transition.
 *
 *   3) The interrupt is not supported by this version of the RPM.
 *
 *
 * == Timer-Based Triggers ==
 *
 * Timer-based triggers are set and cleared through the SetTimedTrigger
 * function.  The last parameter is a uint32 representing the duration of the
 * timer, in units of slow clock ticks (32768 Hz).  Thus, the minimum time is
 * 1 sclk (about 30.5 us) and the maximum time is 0xFFFFFFFF sclks (about
 * 131,072 s or 36.4 hours).  To clear a TimedTrigger, 'set' the trigger with
 * a duration of '0'.
 *
 * When something causes the RPM to transition to the set represented by the
 * FromSet of the timed trigger, the timer will start.  If no other event
 * causes a transition away from FromSet before the timer duration expires,
 * then the RPM will transition this master to ToSet.  If the master
 * transitions away from FromSet before the timer expires, the timer is
 * cancelled and restarted the next time FromSet is reached.
 *
 *
 * == Config Set Wildcards ==
 *
 * Interrupt-based trigger functions accept DAL_RPM_CONFIG_SET_INVALID as a
 * wildcard to either the FromSet or ToSet parameters (but not both at the
 * same time).
 *
 * If the invalid set is passed as the FromSet parameter and your trigger is
 * accepted, then any time that interrupt is received it will transition your
 * master to ToSet, regardless of which set it was currently in.
 *
 * If the invalid set is passed as the ToSet parameter and your trigger is
 * accepted, then whenever your trigger fires it will cause a transition to
 * the previous *active* (not sleep) set that your master had selected.  That
 * language is hard to parse, so here's an example:
 *
 * [Master's state] <-- [Set a wildcarded ToSet would transition to]:
 *   - Active Set 0 <-- Active Set 0 (since there's no "history")
 *   - Active Set 2 <-- Active Set 0
 *   - Active Set 1 <-- Active Set 2
 *   - Active Set 4 <-- Active Set 1
 *   - Sleep Set    <-- Active Set 4
 *   - Active Set 0 <-- Active Set 4 (sleep set is ignored by wildcarded ToSet)
 *   - Active Set 1 <-- Active Set 0
 *
 *
 * == Wakeup Interrupts ==
 *
 * Wildcarding the ToSet parameter can be very useful for simply flagging an
 * interrupt as a "wakeup" interrupt.  For example, if you wanted to have
 * the USB interrupt wake your master from sleep you could do
 *
 *   DAL_rpm_InterruptType triggers = {0};
 *   triggers.u.bits.USB = 1;
 *   SetTrigger(<handle>, SLEEP_SET, INVALID_SET, triggers);
 *
 * That way, if the USB interrupt occurs while your master is sleeping, your
 * master will automatically be transitioned back to its previously active set.
 */

 /* sets the trigger */
static __inline DALResult
DalRPM_SetTrigger(DalDeviceHandle * _h, DAL_rpm_ConfigSetType  FromSet, DAL_rpm_ConfigSetType  ToSet, DAL_rpm_InterruptType  Interrupts)
{
   return ((DalRPMHandle *)_h)->pVtbl->SetTrigger( _h, FromSet, ToSet, Interrupts);
}

/* Clears Interrupts from triggering a transition from FromSet to ToSet.
 * Note: Does not affect wildcarded triggers, which could still cause the
 *       transition being cleared here if the master happened to be in FromSet
 *       when the wildcarded trigger to ToSet came in. */
static __inline DALResult
DalRPM_ClearTrigger(DalDeviceHandle * _h, DAL_rpm_ConfigSetType  FromSet, DAL_rpm_ConfigSetType  ToSet, DAL_rpm_InterruptType  Interrupts)
{
   return ((DalRPMHandle *)_h)->pVtbl->ClearTrigger( _h, FromSet, ToSet, Interrupts);
}

/* sets or clears a timed trigger--use duration '0' to clear */
static __inline DALResult
DalRPM_SetTimedTrigger(DalDeviceHandle * _h, DAL_rpm_ConfigSetType  FromSet, DAL_rpm_ConfigSetType  ToSet, uint32  DurationSclks)
{
   return ((DalRPMHandle *)_h)->pVtbl->SetTimedTrigger( _h, FromSet, ToSet, DurationSclks);
}


/***********************************
 *         Misc Functions          *
 ***********************************/
static __inline DALResult
DalRPM_GetEnumByName(DalDeviceHandle * _h, const char *Name, DAL_rpm_ResourceType *Resource)
{
   return ((DalRPMHandle *)_h)->pVtbl->GetEnumByName( _h, Name, Resource);
}

static __inline DALResult
DalRPM_GetResourceSize(DalDeviceHandle * _h, DAL_rpm_ResourceType Resource, uint32 *ResourceSize)
{
   return ((DalRPMHandle *)_h)->pVtbl->GetResourceSize( _h, Resource, ResourceSize);
}


#endif

