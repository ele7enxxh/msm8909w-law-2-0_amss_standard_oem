/* 
* dcvs_core.c
*
* This module implements the Dynamic Clock and Voltage Scaling(DCVS)
* for ARM11.
*
* EXTERNALIZED FUNCTIONS
*
* Copyright (C) 2009 - 2011, 2012 by Qualcomm Technologies, Inc.
* All Rights Reserved.
*
**********************************************************************/
/*============================================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/dcvs_core.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----   ---------------------------------------------------------------------------- 
10/08/13   pk     Added temp fix for 8926 1.0 parts
03/27/12   nj     Replaced DALSYS_SyncEnter with npa_resource_lock and DALSYS_SyncExit with 
                  npa_resource_unlock. These APIs use the correct/required argument signatures
03/23/12   nj     Changed include file & macros to be compatible with 8974
10/25/11   sg     Limit algo name to DCVS_NV_STRING_LEN_LIMIT.
09/27/11   ss     Changing the default log size for the algorithm logs. 
09/25/11   sg     Removed unused inclusion of CoreString.h.
04/26/11   ss     Fixing Q6 compiler warnings.
02/28/11   ss     Using memset to initialiize the allocated memory.
02/18/11   ss     Adding context based processing for DCVS.
01/24/11   ss     Removing the OAL layer of the DCVS Module and moving CPU monitor module to 
                  a separate file.
10/22/10   ss     Adding DCVSCoreClockPlanIndexGet API.
10/14/10   ss     Using OS specific locks while logging from the particular algorithm.
                  Logging the mips with and without idle override from the CPU monitor.
10/08/10   ss     Using the new DCVS clock plan data structure. Initialization api for 
                  the clock plan and default frequency conversions are also added.
10/08/10   ss     Adding the default conversion frequency functions  in the core.
07/28/10   ss     Using the clock plan from the query interface of the
                  "/clk/cpu" resource if available.
07/28/10   ss     Limiting the maximum length of the algorithm name to 
                  31 bytes.
07/15/10   ss     Adding code to enable/disable AVS from the
                  DCVSCoreInitialize() function.
06/17/10   ss     Changing the logging format to be compatible with
                  visualization tools.
06/16/10   ss     Removing unused interfaces DCVSCoreMonitorEnable, 
                  DCVSCoreMipsSet, DCVSCoreDisable, DCVSCoreEnable,
                  DCVSCoreMonitorDisable.  
4/12/10    ss     Using the "/clk/cpu" resource locks for locking by
                  algorithm and CPU monitor.
04/08/10   ss     Fixing the value of idle percentage in the CPU monitor 
                  when the time difference between 2 samples is 0. 
04/08/10   ss     Destroying the DCVS startup client and its requests
                  before the actual DCVS algorithm starts.
03/30/10   ss     Introducing OS specific delay before the starting the 
                  DCVS algorithm.
03/30/10   ss     Using DCVSCoreCpuMonitor as a callback from event 
                  notification APIs
02/21/10   ss     Implementing CPU monitor with periodic logging which 
                  is separate from DCVS algorithm specific logging.
01/20/10   bjs    Cleanup
08/25/09   ss     Initial check in.
==============================================================================================*/

/*=======================================================================

                          INCLUDE FILES

========================================================================*/

#include "dcvs_core.h"
#include "dcvs_os.h"
#include "dcvs_algorithm.h"
#include "dcvs_npa.h"
#include "ClockDefs.h"
#include "npa.h"
#include "CoreVerify.h"
#include "dcvs_nv.h"
#include "ULogFront.h"
#include "npa_resource.h"
#include "ULog.h"
#include "dcvs_target.h"
#include "DDIChipInfo.h"

#if MCA_ENABLE_DIAG_HOOKS
#include "mca_diag_hooks.h"
#endif

#ifdef WINSIM
#include <stdio.h>
#include "string.h"

#define strlprintf sprintf_s 
#define strlcpy(d, s, c) strcpy_s(d, c, s)
#define strlen strlen

#else
#include "stringl/stringl.h"
#endif

DalChipInfoVersionType dcvsTargetChipVersion = 0;
DalChipInfoFamilyType dcvsTargetFamily = 0;

/* Data type to maintain the common/generic states of this layer of DCVS. These
   states are used by all the cores. */
typedef struct
{
  /* Flag to indicate that the NV items are initialized. */
  DALBOOL nvDataInit;

}DCVS_CORE_GENERIC_DATA;

static DCVS_CORE_GENERIC_DATA dcvsCoreGeneric;

typedef enum
{
  DCVS_PARAMS_STARTUP_ALGORITHM = 0,
  DCVS_PARAMS_STARTUP_LOG_SIZE,
  DCVS_PARAMS_STARTUP_AVS_ENABLE,
} DCVS_PARAMS_STARTUP;

// Default Startup NV Parameter set.
// Disable AVS by default.(Target specific entry may override this)

static const DCVS_NV_PARAM_ENTRY dcvsParamsStartup[] =
{
  {"Algorithm", DCVS_NV_PARAM_STRING, {"Disabled"}},
  {"LogSize", DCVS_NV_PARAM_UINT32, {(void*)16384}},
  {"AVSEnable", DCVS_NV_PARAM_UINT32, {(void*)0}},
  DCVS_NV_PARAM_LIST_END
};



/**
  @brief DCVSCoreMipsToMBpsConvert
  This function is the default conversion from Mips to MBps ( Mega Bytes per second ).
  This function is used when the target doesnt define the conversion function.

  @param mips  : The input mips which needs to be converted.
  @return      : The value in MBps.
	
*/
uint32 DCVSCoreMipsToMBpsConvert(uint32 mips)
{
  return (mips*4);
}


/**
  @brief DCVSCoreMipsToKhzConvert
  This function is the default conversion from Mips to Khz ( Kilo Hz ).
  This function is used when the target doesnt define the conversion function.

  @param mips  : The input mips which needs to be converted.
  @return      : The value in Khz.
	
*/
uint32 DCVSCoreMipsToKhzConvert(uint32 mips)
{
  return (mips*1000);
}

/**
  @brief DCVSCoreKhzToMipsConvert
  This function is the default conversion from Khz( Kilo Hz ) to mips.
  This function is used when the target doesnt define the conversion function.

  @param mips  : The input Khz which needs to be converted.
  @return      : The value in mips.
	
*/
uint32 DCVSCoreKhzToMipsConvert(uint32 freqKhz)
{
  return (freqKhz/1000);
}

/**
  @brief ClockPlanDataMemoryAllocateAndInitialize
  This function is used to allocate memory dynamically for the clock plan table. 
  The function error fatals if the memory allocation fails.

	 @param context: The The DCVS data context in question.

  @return None.
*/
static void ClockPlanDataMemoryAllocateAndInitialize(DCVS_CONTEXT* context)
{
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;
  DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE* clockPlanTable;

  /* Allocate the memory for the clockplan table for each of the performance levels. */
  if(DALSYS_Malloc(sizeof(DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE)*(coreDataContext->clockPlan->numLevels),  
                   (void**) &clockPlanTable)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  /* Initialize the memory allocated for the CPU monitor data */
  memset(clockPlanTable, 0, sizeof(DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE)*(coreDataContext->clockPlan->numLevels));
  context->coreDataContext->clockPlan->clockPlanTable = clockPlanTable;

}

/**
  @brief ClockPlanDataFill
  This function fills out the clock plan data of type 
	 DCVS_PRIMARY_CLOCK_PLAN_DATA_TYPE

	 @param context: The The DCVS data context.
  @param index: The index into the clock plan table.
  @param freqKhz: The frequency in khz.

  @return None.
	
*/
static void ClockPlanDataFill(DCVS_CONTEXT* context, uint32 index, uint32 freqKhz)
{
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext; 

  /* Fill in the requested mips values. */
  coreDataContext->clockPlan->clockPlanTable[index].reqMips = coreDataContext->clockPlan->KhzToMipsConvert(freqKhz);

  /* Fill in the cpu frequency value in khz. */
  coreDataContext->clockPlan->clockPlanTable[index].cpuFreq = freqKhz;
  
  /* Fill in the bus frequency in MBps. */
  coreDataContext->clockPlan->clockPlanTable[index].busThroughput = coreDataContext->clockPlan->MipsToMBpsConvert(coreDataContext->clockPlan->clockPlanTable[index].reqMips);

}

/**
  @brief DCVSCoreClockPlanFromResourceGet
  This function fills out the clock plan data for each core by querying the clock resource.
  This function is invoked :
  *) if the lookup table of mips and frequencies for the target doesnt exist.
	
	 @param context: The DCVS data context in question.

  @return TRUE if the clock plan data could be fetched by querying from the resource.
          FALSE if the local table of clock plan frequencies do not exist 
          or there is an internal error in the function.
	
*/
static DALBOOL DCVSCoreClockPlanFromResourceGet(DCVS_CONTEXT* context)
{
  uint32 freqKhz, i;
  npa_query_type queryResult;
  /* Query handle to NPA resource */
  npa_query_handle localHandle;
  
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext; 

  /* Create a query handle to the clock resource . */ 
  localHandle = npa_create_query_handle(context->clockDependency);
  
  /* Check if the query interface for obtaining the clockplan directly from the resource 
     is available */
  if( (localHandle == NULL)||
      (npa_query(localHandle, CLOCK_NPA_QUERY_NUM_PERF_LEVELS, &queryResult)!= NPA_QUERY_SUCCESS)
    )
  {
    /* destroy the query handle */
    npa_destroy_query_handle(localHandle);
    return FALSE;
  }
  else
  {
    /* Fetch the number of levels from the from clkrgm */
    coreDataContext->clockPlan->numLevels = queryResult.data.value;

    /* Allocate the memory for the clock plan table */
    ClockPlanDataMemoryAllocateAndInitialize(context);

    /* Fill our clock plan array. */
    for ( i = 0; 
          i < coreDataContext->clockPlan->numLevels; 
          i++ )
    { 
      /* Error fatal if the clock plan cannot be fetched. */
      CORE_VERIFY(!npa_query(localHandle, 
                             CLOCK_NPA_QUERY_PERF_LEVEL_KHZ+i, 
                             &queryResult));
      
      /* The value returned is in KHz. */
      freqKhz = queryResult.data.value;
      /* Fill in the clock plan data for the particular entry. */
      ClockPlanDataFill(context, i, freqKhz);
      
    }
    /* destroy the query handle */
    npa_destroy_query_handle(localHandle);
    return TRUE;
  }

}

/**
  @brief DCVSCoreClockPlanLocalGet
  This function fills out the clock plan data from the local clock plan data 
  of the target.

  This function is invoked :
  *) if the lookup table of mips and frequencies for the target doesnt exist.
  *) if the clock plan data cannot be queried from the resource.
	
	 @param context: The The DCVS data context.

  @return TRUE if the clock plan data could be fetched by querying from the local clock plan.
          FALSE if the local table of clock plan frequencies do not exist 
          or there is an internal error in the function.
	
*/
DALBOOL DCVSCoreClockPlanLocalGet(DCVS_CONTEXT* context)
{
  uint32 i, freqKhz;
  DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE *tempClockPlan;
  
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext; 

  /* Get the number of levels in the local clock plan of the target */
  /* ToDo:: Figure out a way to have a local clock plan . */
  coreDataContext->clockPlan->numLevels = DCVSTargetClockPlanLocalNumLevelsGet();

  if((tempClockPlan=DCVSTargetClockPlanLocalGet())==NULL)
  {
    return FALSE;
  }

  /* Allocate the memory for the clock plan table */
  ClockPlanDataMemoryAllocateAndInitialize(context);

  /* Fill the passed in clock plan array. */
  for ( i = 0; 
        i < coreDataContext->clockPlan->numLevels; 
        i++ )
  {
    freqKhz = tempClockPlan[i].freqKhz;

    /* Fill in the clock plan data for the particular entry. */
    ClockPlanDataFill(context, i, freqKhz);
  }
  return TRUE;
}

/**
  @brief This function initializes the clock plan for the particular target.  
  This function gets the clock plan in the following priority ::
  *) lookup table of mips and frequencies for the target.
  *) querying the clock plan data from the clock  resource.
  *) local clock plan table of frequencies of the target.
	
	 @param context: The The DCVS data context in question.

  @return : The clock plan type of the target.
	
*/
DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* DCVSCoreClockPlanInitialize(DCVS_CONTEXT* context)
{
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext; 

  if(coreDataContext->clockPlanDataInit==FALSE)
  {
    coreDataContext->clockPlan = DCVSTargetClockPlanInfoGet();

    /* Check if the target specific frequency conversion functions exist. If not then use the
       default frequency conversion functions. */
    if(coreDataContext->clockPlan->KhzToMipsConvert == NULL)
    {
      coreDataContext->clockPlan->KhzToMipsConvert = DCVSCoreKhzToMipsConvert;
    }

    if(coreDataContext->clockPlan->MipsToKhzConvert == NULL)
    {
      coreDataContext->clockPlan->MipsToKhzConvert = DCVSCoreMipsToKhzConvert;
    }

    if(coreDataContext->clockPlan->MipsToMBpsConvert == NULL)
    {
      coreDataContext->clockPlan->MipsToMBpsConvert = DCVSCoreMipsToMBpsConvert;
    }

    /* Check if the target specific mips freq. map table exists  */
    if(coreDataContext->clockPlan->clockPlanTable == NULL)
    {
      /* Check if the clock plan can be obtained through a query into the clock resource */
      if(DCVSCoreClockPlanFromResourceGet(context) == FALSE)
      {
        /* Fill in the clock plan from the target's local clock plan table of frequencies. */
        CORE_VERIFY(DCVSCoreClockPlanLocalGet(context));
      }
    }
    
    /* Mark the clock plan data initialization as complete */
    coreDataContext->clockPlanDataInit = TRUE;
  }
  return coreDataContext->clockPlan;
}


const DCVS_CORE_ALGO * DCVSCoreAlgoGet(const char * algName)
{
  int count = 0;

  while (dcvsTargetAlgList[count]!= NULL)
  {
    /* Ensure that we'll properly handle truncated names passed in
       from NV, for targets that only support efs_get/put(). */
    if (strncmp(algName, dcvsTargetAlgList[count]->DCVSAlgoName,
                (DCVS_NV_STRING_LEN_LIMIT-1)) == 0)
    {
      break;
    }
    count++;
  }
  return dcvsTargetAlgList[count];
}

/**
  @brief DCVSCoreGetNumAlgos
  
  This function returns the number of algorithms currently supported.

  @param void

  @return The number of algorithms supported currently.

*/
// ToDo: Could this be an init time calculation that is just
// compared after.
uint32 DCVSCoreGetNumAlgos(void)
{
  int count = 0;

  while (dcvsTargetAlgList[count]!= NULL)
  {
    count++;
  }
  return count;
}

/**
  @brief CoreDataMemoryAllocateAndInitialize
  
  This function is used to allocate memory for the initialization data. 

  @param context: The DCVS data context.

  @return: None.

*/
static void CoreDataMemoryAllocateAndInitialize(DCVS_CONTEXT* context)
{
  
  DCVS_CORE_DATA* coreDataContext;
  /* Allocate memory for the core data context if not already done so. */
  if(context->coreDataContext == NULL)
  {
    if(DALSYS_Malloc(sizeof(DCVS_CORE_DATA),  
                     (void**) &coreDataContext)
                     != DAL_SUCCESS) 
    {
      /* Error fatal */
      CORE_VERIFY(NULL);   
    }
    /* Initialize the memory allocated for the initialization data */
    memset(coreDataContext, 0, sizeof(DCVS_CORE_DATA));
    context->coreDataContext = coreDataContext ;
  }
}

uint32 DCVSCoreTargetMaxFreqGet(DCVS_CONTEXT* context)
{
  uint32 maxMips;
  
  DCVS_CORE_DATA* coreDataContext;

  /* Allocate and initialize the memory for the initialization data . */
  CoreDataMemoryAllocateAndInitialize(context);
  
  coreDataContext = context->coreDataContext;
  
  /* Initialize the clock plan for the target if not already done so. */
  coreDataContext->clockPlan = DCVSCoreClockPlanInitialize(context);
  maxMips = coreDataContext->clockPlan->clockPlanTable[coreDataContext->clockPlan->numLevels-1].reqMips;

  return maxMips;
}



void DCVSCoreInitialize(DCVS_CONTEXT* context)
{
  /* The size of the cpu dynamics log in bytes. */ 
  uint32 cpuDynLogBufSize = 0;

  /* The name of the log */
  char logName[128], coreIdString[2];
  /* The start up algorithm name */
  char startupAlg[DCVS_NV_STRING_LEN_LIMIT];

  ULogResult createResult;

  DCVS_CORE_DATA* coreDataContext;
  
  /* Allocate the memory for the initialization data if not already done so. */
  CoreDataMemoryAllocateAndInitialize(context);

  coreDataContext = context->coreDataContext;

  if(dcvsCoreGeneric.nvDataInit == FALSE)
  {
    // Initialize the NV parameters only once.
    DcvsNvInitialize();

    // Write the Default Startup Parameters
    DCVSNvDefaultParamsInit ("Startup", 
                             dcvsParamsStartup);

    /* Mark the NV data as initialized. */
    dcvsCoreGeneric.nvDataInit = TRUE;
  }

  // Determine the proper size for our DCVS Log
  DcvsNvParamUint32Read(context->coreId, 
                        "Startup", 
                        &dcvsParamsStartup[DCVS_PARAMS_STARTUP_LOG_SIZE], 
                        &cpuDynLogBufSize);

  /* Convert the core ID to ASCII character. */
  coreIdString[0] = context->coreId+48;
  coreIdString[1] = '\0';


  /* Create the name for the log. */  
  strlcpy(logName, "CPUDyn", 128);
  strlcat(logName, coreIdString, 128);

  /* Create the log for CPU Dynamics . */
  createResult = ULogFront_RealTimeInit(&coreDataContext->logHandle, 
                                        logName,
                                        cpuDynLogBufSize,  // Initial Buffer
                         		ULOG_MEMORY_LOCAL,
  					ULOG_LOCK_OS);
  if ((createResult != DAL_SUCCESS) &&
      (createResult != ULOG_ERR_INITINCOMPLETE))
  {
    // error fatal
    CORE_VERIFY(0);
  }

  /* Create a query handle to the clock resource . */ 
  CORE_VERIFY_PTR(coreDataContext->clockQueryHandle = npa_create_query_handle(context->clockDependency));
  CORE_VERIFY_PTR(coreDataContext->clockResourceHandle = npa_query_get_resource(coreDataContext->clockQueryHandle));

  /* Write the header to allow the visualization tool to know which parser to apply  */
  ULogCore_HeaderSet(coreDataContext->logHandle, "Content-Type: text/tagged-log-1.0; title=CPU Dynamics running history");

  
  /* Initialize the clock plan information for this target if it is not already done. */
  coreDataContext->clockPlan = DCVSCoreClockPlanInitialize(context);
  
  /* Read the startup algorithm . */
  DcvsNvParamStringRead(context->coreId, 
                        "Startup", 
                        &dcvsParamsStartup[DCVS_PARAMS_STARTUP_ALGORITHM], 
                        startupAlg, 
                        DCVS_NV_STRING_LEN_LIMIT);
  coreDataContext->currentAlgorithm = DCVSCoreAlgoGet(startupAlg);
 
  if (coreDataContext->currentAlgorithm == NULL)
  {
    // Disabled must exist on every target.
    coreDataContext->currentAlgorithm = DCVSCoreAlgoGet("Disabled");

    if (coreDataContext->currentAlgorithm == NULL)
    {
      // Error fatal!
      CORE_VERIFY(0);
    }
  } /* end of coreDataContext->clockPlan != NULL*/


  #ifdef MCA_ENABLE_DIAG_HOOKS
  //coreDataContext->currentAlgorithm = DCVSCoreAlgoGet("Requests");
  #endif
  
  // **** Temp fix for 8926 1.0 HW ****
  // We want to have Disabled running on 8926 targets that have the chip version as 0x00010000
  // dcvsTargetChipVersion & dcvsTargetFamily gets initialized in the function: DCVSCoreClockPlanInitialize
  // that will unconditionally get called before this check happens
  if (dcvsTargetFamily == DALCHIPINFO_FAMILY_MSM8926 && dcvsTargetChipVersion == 0x00010000)
  {
    // Set the DCVS algorithm to Disabled
    coreDataContext->currentAlgorithm = DCVSCoreAlgoGet("Disabled");
  }


  /* Initialize the algorithm specific members*/
  coreDataContext->mips = coreDataContext->currentAlgorithm->DCVSCoreAlgoInitialize(context);

  /* Build the parameters of the required algorithm */
  coreDataContext->currentAlgorithm->DCVSCoreAlgoBuildParams(context);

  /* Indicate that the algorithm is enabled */
  coreDataContext->enabled = TRUE;

  /* If desired, enable hooks to fix MCA output via ULogDiag Plugin */
  #ifdef MCA_ENABLE_DIAG_HOOKS
    ULogDiagAddPlugin( mca_ulog_diag_plugin, (uint32) ULOG_DIAG_PLUGIN_ID_MCA );
  #endif

  /* Introduce a delay before starting the DCVS algorithm through a callback. */
  DCVSOSCallbackCreate(DCVSCoreStart, DCVSOSStartUpDelay(), context);

}/* DCVSCoreInitialize */



void DCVSCoreStart(DCVS_CONTEXT* context)
{
  
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;

  /* Remove the DCVS startup client and its requests */
  DCVSStartupClientDestroy(context);

  /* Lock to ensure the algorithm timers are protected */
  npa_resource_lock(coreDataContext->clockResourceHandle);

  /* Start the DCVS algorithm */
  coreDataContext->mips = coreDataContext->currentAlgorithm->DCVSCoreAlgoEnable(context);
  npa_resource_unlock(coreDataContext->clockResourceHandle);

} /* DCVSCoreStart */

uint32 DCVSCoreMipsGet(DCVS_CONTEXT* context)
{
  
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;

  if (!coreDataContext->enabled)
  {
    /* Get the max mips  */
    coreDataContext->mips = coreDataContext->clockPlan->clockPlanTable[coreDataContext->clockPlan->numLevels-1].reqMips;
  }
  else
  {
    /* Act only if DCVS algorithm is enabled */
    coreDataContext->mips = coreDataContext->currentAlgorithm->DCVSCoreAlgoDecideMips(context);
  }
  return(coreDataContext->mips);
}

uint32 DCVSCoreCheckEnabled(DCVS_CONTEXT* context)
{
  
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;

  return(coreDataContext->enabled);
}


/**
  @brief DCVSCoreClockPlanGet
  
  This function returns the pointer to clock plan information for the target.

  @return pointer to clock plan information for the target.

*/
DCVS_TARGET_CLOCK_PLAN_INFO_TYPE* DCVSCoreClockPlanGet(DCVS_CONTEXT* context)
{
  
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext;

  /* Allocate memory for the core data context if not already done so. */
  CoreDataMemoryAllocateAndInitialize(context);

  coreDataContext = context->coreDataContext;

  /* Initialize the clock plan data if it is not already done */
  coreDataContext->clockPlan = DCVSCoreClockPlanInitialize(context);

  return(coreDataContext->clockPlan);
}



ULogHandle DCVSCoreLogHandleGet(DCVS_CONTEXT* context)
{
  
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;

  return(coreDataContext->logHandle);
}


/**
  @brief DCVSCoreClockPlanIndexGet

  This function is used to lookup the mips from the clock plan of the target.
  The Mips searched for would be equal to or the next nearest mips value from the clock plan table.

  @param mips: mips value to be looked up
  @param context: The DCVS data context

  @return : index where the mips value is found in the table. If the index 
            returned is negative, the aggregated MIPS is zero and DCVS will relinquish its
            votes for Q6 clk and BW

*/
int32 DCVSCoreClockPlanIndexGet(uint32 mips, DCVS_CONTEXT* context)
{
  uint32 i;
  
  /* Obtain the core data context. */
  DCVS_CORE_DATA* coreDataContext = context->coreDataContext;

  // return a negative index if zero MIPS
  if ( mips == 0 )
  {
    return -1;
  }

  // start with highest clk index and increament down to optimal level
  uint32 maxClockIndex = coreDataContext->clockPlan->numLevels-1;

  /* Iterate the clock plan table. 
     The zeroth index of the array corresponds to the minimum mips */
  for ( i = 0; i <= maxClockIndex; i++)
  {
    if (coreDataContext->clockPlan->clockPlanTable[i].reqMips >= mips)
    {
      return i;
    }
  }

  /* Return the maximum index of clock plan table */
  return maxClockIndex;
}

