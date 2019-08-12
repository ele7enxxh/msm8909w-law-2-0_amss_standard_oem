#ifndef DEM_H
#define DEM_H
/*============================================================================
  @file dem.h

  DEM - Dynamic Environment Manager -- Public Interface

  DEM provides API's for managing power save states -- putting a processor in
  to power collapse, waking up other processors, and notification events about
  changes between power saving states.

  DEM also manages a few other enviornment-related things, such as reading NV
  items and temperature notifications.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/dem.h#1 $ */

// When we're included by C++ files, make sure we're identified as 'C' code.
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

 /*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "comdef.h"
#include "smsm.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

// Note: If these masks change, then the smem log perl script also needs to be
//       updated or else the human-readable log will be incorrect.
#define DEM_WAKEUP_REASON_NONE       0x00000000
#define DEM_WAKEUP_REASON_SMD        0x00000001
#define DEM_WAKEUP_REASON_INT        0x00000002
#define DEM_WAKEUP_REASON_GPIO       0x00000004
#define DEM_WAKEUP_REASON_TIMER      0x00000008
#define DEM_WAKEUP_REASON_ALARM      0x00000010
#define DEM_WAKEUP_REASON_RESET      0x00000020
#define DEM_WAKEUP_REASON_OTHER      0x00000040
#define DEM_WAKEUP_REASON_REMOTE     0x00000080

// Backward compatibility if the SMD driver is using the RPC wakeup reason.
#define DEM_WAKEUP_REASON_RPC        DEM_WAKEUP_REASON_SMD

// Length of port name to log in wakeup information.
#define DEM_MAX_PORT_NAME_LEN      20

// These three belong in the chip-specific section below, but until we have
// a chip-specific internal header, we'll have to leave them here.
#define DEM_NUM_SHARED_INTS          32
#define DEMAPPS_NUM_SMD_WAKEUP_INTS   5
#define DEM_NUM_WAKEUP_INTS (DEM_NUM_SHARED_INTS + DEMAPPS_NUM_SMD_WAKEUP_INTS)

// Return value from the notification registration function.
#define DEM_NOTIFICATION_HANDLE uint32

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

typedef struct
{
  // Bit mask of all of the wakeup reasons.
  uint32 wakeup_reasons;

  // These three are only valid if the DEM_WAKEUP_REASON_SMD bit is set.
  uint32 aArm_rpc_prog;
  uint32 aArm_rpc_proc;
  char   aArm_smd_port_name[DEM_MAX_PORT_NAME_LEN];

  // These two are only valid if the DEM_WAKEUP_REASON_INT bit is set.
  // For Apps, the wakeup_src should be cast as tramp_isr_type
  uint32 wakeup_src[DEM_NUM_WAKEUP_INTS];
  uint32 wakeup_src_idx;

  // This is only valid if the DEM_WAKEUP_REASON_GPIO bit is set.
  uint32 which_gpio;
} DEM_WAKEUP_INFO_TYPE;

typedef enum
{
  // Power state related events.
  DEM_NOTIFY_POWERDOWN,
  DEM_NOTIFY_POWER_RESTORED,
  DEM_NOTIFY_RESET,
  DEM_NOTIFY_RESET_COMPLETE,
  DEM_NOTIFY_SUSPEND,

  // Run quiet related events.
  DEM_NOTIFY_RUN_QUIET,
  DEM_NOTIFY_RUN_NORMAL,

  // Temperature related events.
  DEM_NOTIFY_TEMP_OKAY,
  DEM_NOTIFY_TEMP_WARNING,
  DEM_NOTIFY_TEMP_CRITICAL,

  // Internal-only events.
  DEM_NOTIFY_NUM_EVENTS
} DEM_NOTIFY_EVENTS;

typedef enum
{
  // Highest priority is for internal use only.  If you are unsure, you should
  // use DEM_NOTIFICATION_PRIORITY_NORMAL.
  DEM_NOTIFICATION_PRIORITY_HIGHEST = 0,
  DEM_NOTIFICATION_PRIORITY_NORMAL = 10,
  DEM_NOTIFICATION_PRIORITY_LOWEST = 20
} DEM_NOTIFICATION_PRIORITY;

typedef enum
{
  DEM_LCDC_STATE_ON,
  DEM_LCDC_STATE_OFF,
  DEM_LCDC_STATE_DEFAULT
} dem_lcdc_state_type;

// Forward declaration for self-reference reasons, defined below.
struct DEM_NOTIFICATION_STRUCT;

typedef void (*DEM_NOTIFICATION_CALLBACK_TYPE)(struct DEM_NOTIFICATION_STRUCT *notification);

typedef struct DEM_NOTIFICATION_STRUCT
{
  smsm_host_type host;
  DEM_NOTIFY_EVENTS event;
  DEM_NOTIFICATION_PRIORITY priority;
  DEM_NOTIFICATION_CALLBACK_TYPE callback;
  void *userData;
  struct DEM_NOTIFICATION_STRUCT *next;
} DEM_NOTIFICATION_TYPE;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/* Functions that relate to determining system state. */

/**
 @brief Register for notification of a DEM event.

 This function allows registration for a number of different events triggered
 by the DEM code.  You specify both which processor's events you are
 interested in, as well as which event on that processor this callback is for.

 @param host        : The CPU that needs to be monitored.
 @param event       : The event that should trigger the callback.
 @param priority    : Determines the order of callbacks.  SMD code should use
                      DEM_NOTIFICATION_PRIORITY_HIGHEST, and all others should use
                      DEM_NOTIFICATION_PRIORITY_NORMAL.
 @param userData    : Data that should be passed to the callback.
 @param cbFcn       : The function that should be called when the event occurs.
 
 @return : A handle that uniquely identifies the notification.
           0 if the notification failed to register.
 
 */
DEM_NOTIFICATION_HANDLE DEMNotificationRegister (
                              smsm_host_type host,
                              DEM_NOTIFY_EVENTS event,
                              DEM_NOTIFICATION_PRIORITY priority,
                              void * userData,
                              DEM_NOTIFICATION_CALLBACK_TYPE cbFcn);

/**
 @brief Determine if a Processor is Sleeping

 Query to determine if an external processor is sleeping.

 @param  smsmCpuId: The SMSM ID of the target processor.
 @return TRUE if the processor is sleeping, otherwise FALSE.
 */
extern boolean DEMProcSleeping( uint32 smsmCpuId );

/**
 @brief Determine if a Processor is Power Collapsed

 Query to determine if an external processor is in Power
 Collapse.

 @param  smsmCpuId: The SMSM ID of the target processor.
 @return TRUE if the processor is in power collapse, otherwise
         FALSE.
 */
extern boolean DEMProcPowerCollapsed( uint32 smsmCpuId );

/**
 @brief Get information about why this target was woken up.

 This function retrieves information received from the modem during the wakeup
 process regarding why the processor was woken up.

 @param wakeup_info_type : Pointer to the structure the data should be stored
                           into.  Must be allocated by caller.
 */
void DEMTargetGetWakeupSource(DEM_WAKEUP_INFO_TYPE *wakeup_info_type);





/* Functions for forcing a processor to wake up. */

/**
 @brief Cause a processor to be awakened from Apps Sleep if it
        is not awake already.

 If a processor is in Apps Sleep, calling this function will
 cause the processor to be pulled out of sleep.

 @param  smsmCpuId: The SMSM ID of the target processor.
 @param  wakeupReason: The reason we are waking the processor.
 @return TRUE if the processor is asleep or awake.  FALSE if the
         processor is power collapsed.
 */
extern boolean DEMProcWakeup ( uint32 smsmCpuId, uint32 wakeupReason );

/**
 @brief Cause a processor to be awakened from Apps Sleep if it
        is not awake already.

 If a processor is in Apps Sleep, calling this function will
 cause the processor to be pulled out of sleep.  Allows for
 extended wakeup information to be sent.

 @param  smsmCpuId: The SMSM ID of the target processor.
 @param  wakeupReason: The reason we are waking the processor.
 @param  extenededReason: The extended reason we are waking the
                        processor.
 @return TRUE if the processor is asleep or awake.  FALSE if the
         processor is power collapsed.
 */
extern boolean DEMProcWakeupEx (uint32 smsmCpuId, 
                                uint32 wakeupReason, 
                                const void * extendedReason );

/**
 @brief Cause a processor to be powered on if it is in Power
        Collapse.

 If a processor is in Power Collapse, this function will cause
 the processor to be powered up.

 @param  smsmCpuId: The SMSM ID of the target processor.
 @param  wakeupReason: The reason we are powering up the
                     processor.
 @return TRUE if the processor is asleep or awake.  FALSE if the
         processor is power collapsed.
 */
extern boolean DEMProcPowerup (uint32 smsmCpuId, uint32 wakeupReason );





/* Functions for synchronizing time between processors.
   Note: only one of these will work at any given time.  That is, if you are
   not the time master, calling DEMTimeMasterInit() will fail. */

/**
 @brief Intialize the slock clock synchronization service.

 The modem currently serves as the time master, providing a nominal 32.768 kHz
 time base that can be synchronized to the other processors to provide a
 correlated time base for use in things like SMEM logging.

 @see  DEMTimeSlaveSlowClockSync
 */
extern void DEMTimeMasterInit(void);

/**
 @brief Synchronize the slow clock to the time master.

 The modem currently serves as the time master, providing a nominal 32.768 kHz
 time base.  This function is used to synchronize a processor's own slow clock
 counter to the time master's, so that a correlated time base is available for
 use in things like SMEM logging.

 @param  smsmCpuId: The SMSM ID of this processor.
 @param  timeout  : A timeout for the synchronization in milliseconds.  If
                    this amount of time passes without a successful
                    synchronization, the effort will be abanonded an the local
                    slow clock counter will be untouched.
 @see    DEMMasterTimeInit
 @return The number of slock clock ticks that the local time base was corrected
         by.
 */
extern uint32 DEMTimeSlaveSlowClockSync(uint32 smsmCpuId, uint32 timeout);





/* Misc. functions.  These probably aren't what you're looking for. */

/**
 @brief Initialize DEM's internal state.

 This function needs to be called before any other DEM functions can be called.
 On the modem this function is currently called automatically when the DEM task
 is initialized.

 @see    dem_task
 */
extern void DEMTargetInit(void);

/**
 @brief dem_task - The task that initializes and runs DEM.

 Some systems require a task to run DEM.  This task must be named dem_task.  A
 default implementation exists that should be sufficient for most applications.
 
 @param  ignored : A parameter required by the implementation that remains
                 unused.
 
 */
void dem_task(dword ignored);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DEM_H

