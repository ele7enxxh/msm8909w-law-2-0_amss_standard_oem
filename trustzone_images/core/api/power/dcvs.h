#ifndef DCVS_H
#define DCVS_H

/*===========================================================================

FILE:         dcvs.h

DESCRIPTION:
 This module contains the public declarations and definitions for the
 DCVS system.  This interface will be utilized to define a target system and
 provide feedback to the HLOS on how the cores should be operated.


===========================================================================
             Copyright © 2012 Qualcomm Technologies Incorporated.
                 All Rights Reserved.
                 QUALCOMM Confidential and Proprietary.
===========================================================================

                        Edit History
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/power/dcvs.h#1 $

 when       who     what, where, why
--------   ----    -----------------------------------------------------------
04/16/12    sg     Added support for versioning; Added support for indicating
                   to the HLOS what the new QoS timer value should be at
                   IDLE_EXIT.
04/11/12    sg     Updated idle_exit interface to indicate if QoS timer needs
                   to be extended or restarted.
01/10/12    sg     Initial version.
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "IxErrno.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/** Return status values generally match the SCM recommended error codes. */
#define DCVS_STATUS_SUCCESS                 (E_SUCCESS)
#define DCVS_STATUS_OPERATION_NOT_ALLOWED   (-E_NOT_ALLOWED)
#define DCVS_STATUS_ALREADY_COMPLETED       (-E_ALREADY_DONE)
#define DCVS_STATUS_INSUFFICIENT_MEMORY     (-E_NO_MEMORY)
#define DCVS_STATUS_INVALID_ARGS            (-E_OUT_OF_RANGE)
#define DCVS_STATUS_BAD_ADDRESS             (-E_BAD_ADDRESS)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Core/Group Identifier type.
 */
typedef unsigned long DcvsIdType;

/**
 * DCVS System events
 */
typedef enum
{
  DCVS_EVENT_ID_IDLE_ENTER,        //!< Core entered Idle state
  DCVS_EVENT_ID_IDLE_EXIT,         //!< Core exited Idle state
  DCVS_EVENT_ID_QOS_TIMER_EXPIRED, //!< QoS Timer expired implying that CPU usage is very high.
  DCVS_EVENT_ID_CLOCK_FREQ_UPDATE, //!< Clock frequency for the core has changed.
  DCVS_EVENT_ID_ENABLE_CORE,       //!< Change the state (used for enabling/disabling).
  DCVS_EVENT_ID_RESET_CORE,        //!< Reset core stats and return new frequency.

  DCVS_EVENT_ID_DUMMY_VAL_FORCE_TO_32BIT = 0x7FFFFFFF // DO NOT USE!
} DcvsEventIdType;

/**
 * DcvsAlgorithmParams - A structure with all parameters 
 * required by the DCVS algorithm. These parameters are expected 
 * to change at runtime. 
 */
typedef struct
{
  unsigned long slackTime;            // The amount of time the cpu can run 
                                      // without an idle cycle before it must
                                      // jump to max performance.  If set to
                                      // zero, useEm will be disabled.
                                      // Units: us

  int scaleSlackTime;                 // If true, slack time is scaled based 
                                      // on the current and max frequencies.
                                      // Lower current frequencies yield
                                      // shorter scaled times.
                                      // Units: Boolean

  unsigned int scaleSlackTimePct;     // If scaleSlackTime is true, this parameter
                                      // will additionally scale slack time as
                                      // a direct percentage.
                                      // Units: Percent
 
  unsigned int disablePCThreshold;    // Commanded frequencies above this value 
                                      // cause power collapse to be disabled 
                                      // for this core. A value of 0 disables 
                                      // this feature.
                                      // Units: kHz

  unsigned long emWindowSize;         // Window size for the energy minimization algorithm.
                                      // Units: us 

  unsigned int emMaxUtilPct;          // When determining the level with the 
                                      // lowest energy, any level that exceeds
                                      // this busy percentage, for the measured 
                                      // work in the last window, is disqualified
                                      // for performance reasons. 
                                      // Units: Percent

  unsigned long ssWindowSize;         // Window size for the steady state algorithm.
                                      // Units: Percent 

  unsigned int ssUtilPct;             // When determining the steady state level, 
                                      // this value is used to provide headroom
                                      // from the utilized cpu to the selected 
                                      // level.
                                      // Units: Percent

  unsigned int ssIoBusyConversion;    // Used to convert Adjacent Busy Time into
                                      // assumed IO Busy time, which is removed
                                      // from measured elapsed time when computing
                                      // cpu utilization
                                      // Units: Percent

} DcvsAlgorithmParams;

/**
 * Describes the energy properties for a given core clock frequency.
 */
typedef struct
{
  unsigned long freq;               // Units: kHz
  unsigned long idleEnergy;         // Units???
  unsigned long activeEnergy;       // Units??? 
} DcvsFreqEntry;

/** 
 * DcvsCoreParams - A structure with all parameters required to 
 * define a core.  These parameters are not expected to change 
 * at runtime. 
 */
typedef struct
{
  
  unsigned long maxFreqChangeTime;  // Maximum time the algorithm will consider as
                                    // time spent changing the frequency.  Anything
                                    // above this value will be limited before being
                                    // applied to the freqChangeTime and 
                                    // freqChangeMinTime arrays
                                    // Units: us

  int freqEntryCount;               // Total number of entries in the freqTables
                                    // Units: count

} DcvsCoreParams;

extern unsigned long g_DcvsReturnVal0;
extern unsigned long g_DcvsReturnVal1;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * TZ specific initialization. Must be called from tzbsp_chipset_init()
 * prior to any other DCVS API being used.
 * 
 */
void DcvsInitTZ(void);

/**
 @brief Initialize the DCVS algorithm.

 Initialize the DCVS algorithm and pass in global data that is
 independent of the cores.
 Must be called before any other DCVS API is invoked.

 @param bufPhysAddr[in] : Physical address of a physically contiguous buffer
    that will be used internally by the algorithm. The HLOS should never
    directly use this buffer after it has been passed to the algorithm.
    IMPORTANT: Buffer must be word aligned.

 @param size[in] : Size of the buffer in bytes.
 
 @return : 0 if successful - When successful, the first 16-bit word at
             bufPhysAddr will indicate minor version number; The next 16-bit
             word will indicate the major version number. Both should be treated
             as unsigned short int.

          Non-zero value in case of failure:
          DCVS_STATUS_ALREADY_COMPLETED: if already initialized.
          DCVS_STATUS_INVALID_ARGS: Location failed security check.
*/
int DcvsInit (
  unsigned long bufPhysAddr,
  unsigned long size
);

/**
 @brief DcvsCreateGroup - Create a group for linking cores.
 
 Initialize a group for cores. Grouped cores will always run
 symmetrically (same freq), although they have separate 
 algorithms. 
 
 @param groupId[in] : A unique identifier for the group. This id will be used by
    other public APIs to interface with the algorithm, such as adding
    cores to the group. Must be non-zero.
    It is recommended that a alphanumeric ASCII name of size 4 characters be
    passed in. For example "GRP0" would be 0x47525030.
 
 @return : 0 if successful
          Non-zero value in case of failure:
          DCVS_STATUS_OPERATION_NOT_ALLOWED: if DcvsInit() was not previously
            called.
          DCVS_STATUS_INSUFFICIENT_MEMORY: there is insufficient memory.
          DCVS_STATUS_INVALID_ARGS: If group already exists.
 
*/
int DcvsCreateGroup (
  DcvsIdType groupId
);

/**
 @brief DcvsRegisterCore - Register a core with the DCVS system.
 
 Register a core, and provide the required core parameters set. 
  Also indicate if this core is part of a group for symmetric
  frequency behavior.
 
 @param coreId[in] : A name for the core. This name will be used by other
    public APIs to interface with the algorithm. Must be non-zero.
    It is recommended that a alphanumeric ASCII name of size 4 characters be
    passed in. For example "CPU0" would be 0x43505530.
 @param groupId[in]    : The group this core belongs to. Zero if not 
                         part of a group. If a group is used, requires that a
                         valid group with this groupId was previously created.
 @param coreParams[in] : The parameters for the core.
 @param freqEntry[in]  : A 1d array of available frequency entries
 
 @return : 0 if successful
          Non-zero value in case of failure:
          DCVS_STATUS_OPERATION_NOT_ALLOWED: if DcvsInit() was not previously
            called.
          DCVS_STATUS_INSUFFICIENT_MEMORY: There is insufficient memory
          DCVS_STATUS_INVALID_ARGS: Arguments are invalid or out of range.
 
*/
int DcvsRegisterCore (
  DcvsIdType       coreId,
  DcvsIdType       groupId,
  DcvsCoreParams * coreParams, 
  DcvsFreqEntry  * freqEntry
);

/**
 @brief DcvsSetAlgParams - Set the algorithm parameters for a 
        core.
 
 Set the algorithm parameters for a core.  This function is also
 used to adjust the parameters at runtime. 
 
 @param coreId[in]  : The core to adjust.
 @param params[in]  : The new parameters for the algorithm.
 
 @return : 0 if successful
          Non-zero value in case of failure:
          DCVS_STATUS_OPERATION_NOT_ALLOWED: if DcvsInit() was not previously
            called.
          DCVS_STATUS_INVALID_ARGS: Arguments are invalid or out of range.
 
*/
int DcvsSetAlgParams (
  DcvsIdType            coreId,
  DcvsAlgorithmParams * params
);


/**
 @brief DcvsSystemEventHandler - Handles DCVS system events.
 This is a "slow" SCM call. Will context switch into non-secure mode after
 entering TZ.
 
 @param coreId[in]   : The core with which the event is associated.
 @param eventId[in]  : Event ID. Possible values are:
    DCVS_EVENT_ID_IDLE_ENTER  - Core entered Idle state.
      param0: unused
      param1: unused
      return0: unused
      return1: unused
    DCVS_EVENT_ID_IDLE_EXIT  - Core exited Idle state
      param0: If 1, idle period is treated as an I/O wait period. 0 otherwise.
      param1: unused
      return0: New clock frequency to set for the core (kHz).
      return1: New Qos Timer value (Units: us)
    DCVS_EVENT_ID_QOS_TIMER_EXPIRED - QoS Timer expired implying that CPU
         usage is very high.
      param0: unused
      param1: unused
      return0: New clock frequency to set for the core (kHz).
    DCVS_EVENT_ID_CLOCK_FREQ_UPDATE - Clock frequency for the core has changed.
      param0: Active clock frequency for this core.
      param1: Time it took to switch to this clock rate (microsecs).
      return0: New Qos Timer value (Units: us)
    DCVS_EVENT_ID_ENABLE_CORE - Core state is enabled/disabled. The algo stats
          will be reset when enabled.
      param0: enable state: 1 to enable; 0 to disable.
      param1: Active (current) clock frequency for this core.
      return0: New clock frequency to set for the core (microsecs).
    DCVS_EVENT_ID_RESET_CORE - Reset core stats and returns the new frequency.
         This is to used to indicate that the core is back online after being
         taken offline. The algo will clear its windowing history and provide
         the new operating frequency.
      param0: Active (current) clock frequency for this core.
      param1: unused
      return0: New clock frequency to set for the core (microsecs).

 @param param0[in]   : Parameter associated with the event. Param is valid
                     : only for certain events. See above.
 @param param1[in]   : Parameter associated with the event. Param is valid
                     : only for certain events. See above.
 @param return0[out] : Return value0 for the event. May not be valid based on
                     : Event ID.
 
 @return : 0 if successful
          Non-zero value in case of failure:
          DCVS_STATUS_OPERATION_NOT_ALLOWED: if DcvsInit() was not previously
            called.
          DCVS_STATUS_INVALID_ARGS: Arguments are invalid or out of range.
 */
int DcvsSystemEventHandler (
  DcvsIdType           coreId,
  DcvsEventIdType      eventId,
  unsigned long        param0,
  unsigned long        param1
);

#endif // DCVS_H

