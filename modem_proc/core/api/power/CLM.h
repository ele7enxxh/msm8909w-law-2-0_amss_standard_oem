/*===========================================================================

FILE:         CLM.h

DESCRIPTION:  Defines the API for the CPU Load Monitor (CLM). The CLM provides 
clients with CPU utilization information on periodic basis or threshold 
triggers. 
 
=========================================================================== 
Copyright © 2013-2014 QUALCOMM Technologies, Incorporated. All Rights Reserved. 
QUALCOMM Confidential and Proprietary. 
=========================================================================== 
 
Edit History 
$Header: //components/rel/core.mpss/3.5.c12.3/api/power/CLM.h#1 $ 

=============================================================================*/
#ifndef _CLM_H_
#define _CLM_H_

#include "DALStdDef.h"

/* CLM Client names are restricted to this length (including the '\0') */
#define CLM_MAX_NAME_LENGTH 16

/* The max possible number of HW threads on the system */
#define MAX_HW_THREADS 4

/**
 * CLM Client Attributes
 */
#define CLM_ATTRIBUTE_DEFAULT                     (0)
//#define CLM_ATTRIBUTE_RESET_MEAS_ON_WAKEUP        (1 << 1)
//#define CLM_ATTRIBUTE_REDUCED_PERIOD_ON_PC_EXIT   (1 << 2)

/**
 * CLM Client handle
 */
typedef struct CLM_Client* CLM_HandleType;

/**
 * List of reasons why the client is being provided a util update
 */
typedef enum
{
  /* Periodic update at the end of the client specified measurement window */
  CLM_UPDATE_REASON_PERIODIC,

  /* CPU utilization is below client specified threshold */
  CLM_UPDATE_REASON_LOW_THRESHOLD,

  /* CPU utilization is at or above client specified threshold */
  CLM_UPDATE_REASON_HIGH_THRESHOLD,

  /* INTERNAL: Forces enum to 32-bits */
  CLM_UPDATE_REASON_DUMMY_FORCE_INT32 = 0x7FFFffff

} CLM_UpdateReasonEnumType;

/**
 * List of client types supported by CLM
 */
typedef enum
{
  /* Use this client to access information from PMU registers */
  CLM_CLIENT_PMU_INFO = 0,

  /* Use this client if basic CPU load information is desired.
     See CLM_LoadInfoBasicStruct for the list of fields returned */
  CLM_CLIENT_BASIC_CPUUTIL,

  /* Use this client if extended CPU utilization info is desired.
     See CLM_LoadInfoExtendedStruct for the list of fields returned */
  CLM_CLIENT_EXTENDED_CPUUTIL

} CLM_ClientOutputType;


/**
 * Container for Basic CPU load data
 */
typedef struct CLM_LoadInfoBasicStruct
{
  /* Reason why the update is being provided */
  CLM_UpdateReasonEnumType updateReason;

  /* Number of usecs over which this information was computed */
  uint32 timeElapsedUs;

  /* The current clock rate of the CPU in kHz */
  uint32 currentClkKhz;

  /* CPU utilization percentage at the clock rate when the window ended */
  uint32 utilPctAtCurrClk;

  /* CPU utilization percentage scaled to the maximum clock rate supported by 
     the CPU */
  uint32 utilPctAtMaxClk;

} CLM_LoadInfoBasicStructType;


/**
 * Container for PMU Data
 */
typedef struct CLM_LoadInfoPMUInfoStructType
{
  /* Reason why the update is being provided */
  CLM_UpdateReasonEnumType updateReason;

  /* Number of usecs over which this information was computed */
  uint32 timeElapsedUs;
  
  /* True if the PMU was disabled when this update is provided */
  uint32 pmuDisabled;

  /* Array with the number of cycles during which 1-nHWThreads were busy.
   * [0] is the number of cycles where only 1 HWT was busy, [1] enumerates
   * cycles where 2 HWTs were busy etc.
   */
  uint32 cyclesNThreadsBusy[MAX_HW_THREADS];

} CLM_LoadInfoPMUInfoStructType;

/**
 * Container for extended CPU utilization data
 */
typedef struct CLM_LoadInfoExtendedStructType
{
  /* Reason why the update is being provided */
  CLM_UpdateReasonEnumType updateReason;

  /* Number of usecs over which this information was computed */
  uint32 timeElapsedUs;

  /* Measurement period of the client in usecs */
  uint32 measPeriodUs;

  /* The current clock rate of the CPU in kHz */
  uint32 currentClkKhz;

  /* CPU utilization percentage at the clock rate when the window ended */
  uint32 utilPctAtCurrClk;

  /* CPU utilization percentage scaled to the maximum clock rate supported by 
     the CPU */
  uint32 utilPctAtMaxClk;

  /* Number of processor cycles that were available for use during the 
     measurement window; includes cycles we may've spent power-collapsed */
  uint64 budgetCycles;

  /* Number of processor cycles that were used to execute instructions 
     during the measurement window */
  uint64 busyCycles;

  /* Array with the number of cycles during which 1-nHWThreads were busy.
   * [0] is the number of cycles where only 1 HWT was busy, [1] enumerates
   * cycles where 2 HWTs were busy etc.
   */
  uint32 cyclesNThreadsBusy[MAX_HW_THREADS];

} CLM_LoadInfoExtendedStructType;


/**
 * Template of callback function (provided by client) that is invoked when
 * the measurement window for the client elapses or when CPU Utilization at 
 * MAX Clock exceeds/drops below the specified high/low bus thresholds. 
 *
 * @param loadInfo : pointer to CPU Load info. This data structure is 
 * allocated and owned by CLM and is only valid in context of this callback.
 * Clients must make their own copy if data is needed outside or after this
 * context. loadInfo should be typecast to one of the CLM_LoadInfoXXX
 * types, based on the client type.
 *  
 * @param cbData : Data (opaque to CLM) that was passed in by the client
 * during registration.
 */
typedef void (*CLM_CallbackFuncPtr)( void *loadInfo, void *cbData );


/**
 * Configuration data provided by Threshold clients during registration
 */
typedef struct CLM_ThresholdClientRegStruct
{
  /* If non-zero, the client will be notified when _scaled_ CPU utilization at
     MAX clock is >= this value. Must be >= lowThresholdPct */
  uint32 highThresholdPct;

  /* If non-zero, the client will be notified when _scaled_ CPU utilization at
     MAX clock has fallen below this value. Must be <= highThresholdPct */
  uint32 lowThresholdPct;

  /* Num of threads used in computing load to compare against highThreshold */
  uint32 nThreadsForHigh;

  /* Num of threads used in computing load to compare against lowThreshold */  
  uint32 nThreadsForLow;

} CLM_ThresholdClientRegStruct;


/**
 * <!-- CLM_RegisterThresholdClient -->
 * 
 * Clients that need CPU Load information when a specific threshold is crossed
 * must register using this API.
 *
 * Note that all threshold values will be compared against CPU utilization
 * scaled to MAX clock for the given target.
 *  
 * Clients that have specified a low/high threshold will only receive callbacks 
 * with reason code CLM_UPDATE_REASON_BELOW_LOW_THRESHOLD or 
 * CLM_UPDATE_REASON_BELOW_HIGH_THRESHOLD. 
 *  
 * It is legal to set no thresholds, only a low or a high threshold or to set 
 * both thresholds. If no thresholds are specified, the client is treated as 
 * PERIODIC.
 *
 * @param clientName : Name of client; should be limited to CLM_MAX_NAME_LENGTH
 * characters (including the terminating NULL)
 * 
 * @param clientType : Type of client; one of CLM_ClientOutputType.
 *
 * @param measPeriodUs : Measurement period of client 
 *
 * @param params : Pointer to a CLM_ThresholdClientRegStruct.
 * Will be copied over by CLM during registration
 *
 * @param attributes : Attributes to modify CLM behavior for this client
 *   
 * @param cbFunc : Client callback function of type CLM_CallbackFuncPtr 
 * 
 * @param cbData : Data (opaque to CLM) that was passed in by the client
 * during registration.
 * 
 * @return A non-NULL client handle is returned on successful registration.
 */
CLM_HandleType 
CLM_RegisterThresholdClient( const char            *clientName,
                             CLM_ClientOutputType   clientType,
                             uint32                 measPeriodUs,
                             CLM_ThresholdClientRegStruct *params,
                             uint32                 attributes,
                             CLM_CallbackFuncPtr    cbFunc,
                             void                  *cbData );

/** 
 * <!-- CLM_RegisterPeriodicClient -->
 *  
 * Clients that need CPU load information periodically must register with
 * this API. 
 *  
 * Periodic clients will only receive callbacks with reason code 
 * CLM_UPDATE_REASON_PERIODIC. 
 *  
 * @param clientName : Name of client; should be limited to CLM_MAX_NAME_LENGTH
 * characters (including the terminating NULL)
 * 
 * @param clientType : Type of client; one of CLM_ClientOutputType.
 *
 * @param measPeriodUs : Measurement period of client 
 *
 * @param cbFunc : Client callback function of type CLM_CallbackFuncPtr 
 * 
 * @param cbData : Data (opaque to CLM) that was passed in by the client
 * during registration.
 * 
 * @return A non-NULL client handle is returned on successful registration.
 */
CLM_HandleType
CLM_RegisterPeriodicClient( const char            *clientName,
                            CLM_ClientOutputType   clientType, 
                            uint32                 measPeriodUs,
			    uint32                  attributes,
                            CLM_CallbackFuncPtr		 cbFunc,
                            void                  *cbData );


/** 
 * <!-- CLM_RegisterPeriodicClient_Ex -->
 *  
 * A version of the RegisterPeriodicClient API that takes an additional
 * attributes parameter.
 *  
 * Periodic clients will only receive callbacks with reason code 
 * CLM_UPDATE_REASON_PERIODIC. 
 *  
 * @param clientName : Name of client; should be limited to CLM_MAX_NAME_LENGTH
 * characters (including the terminating NULL)
 * 
 * @param clientType : Type of client; one of CLM_ClientOutputType.
 *
 * @param measPeriodUs : Measurement period of client 
 *
 * @param attributes : Attributes to modify CLM behavior for this client
 *
 * @param cbFunc : Client callback function of type CLM_CallbackFuncPtr 
 * 
 * @param cbData : Data (opaque to CLM) that was passed in by the client
 * during registration.
 * 
 * @return A non-NULL client handle is returned on successful registration.
 */
CLM_HandleType 
CLM_RegisterPeriodicClient_Ex( const char            *clientName,
                               CLM_ClientOutputType   clientType, 
                               uint32                 measPeriodUs,
                               uint32                 attributes,
                               CLM_CallbackFuncPtr		cbFunc,
                               void                  *cbData );

/**
 * <!-- CLM_UnregisterClient -->
 * 
 * @brief Unregister a client from CLM
 * 
 * @param handle : Handle to client
 */
void CLM_UnregisterClient( CLM_HandleType handle );


/**
 * <!-- CLM_GetLastLPRExitTime -->
 * 
 * @brief Returns the timetick recorded at the last CLM LPR exit
 * @return Timetick of last CLM LPR exit
 */
uint64 CLM_GetLastLPRExitTime( void );


/**
 * <!-- CLM_SetClientMeasurementPeriod -->
 * 
 * @brief Modify the measurement period of the client. Must be invoked
 * from within the client' callback, not outside.
 *  
 * @param client : Client whose measPeriod is to be modified
 * 
 * @param measPeriodUs : New measurement period in usec
 */
void CLM_SetClientMeasurementPeriod( CLM_HandleType client, 
                                     uint32 measPeriodUs );

#endif /* _CLM_H_ */
