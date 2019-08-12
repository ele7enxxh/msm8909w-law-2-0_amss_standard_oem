/*==============================================================================

FILE:      DCVSApiTest.c

DESCRIPTION: This file implements functions to test the DCVS algorithm.

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2009 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/test/DCVSApiTest.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ---------------------------------------------------
11/10/10   ss      Remove the usage of the thread to change the clocks
                   periodically.Removing the unused test code.
10/08/10   ss      Using the new DCVS clock plan structure for PC targets.
07/30/10   ss      Removing usage of DCVSEnable_Private().
03/31/10   ss      Implementing a thread to change the state of the 
                   /clk/cpu resource periodically. 
03/25/10   ss      Removing the logging from the testing modules.
========================================================================*/

#include "npa.h"
#include "clkregim.h" 
#include "dcvs_init.h"
#include "DCVSApiTest.h"
#include "dcvs_target.h"
#include "CoreEventNotification.h"
#include "CoreVerify.h"
#include "dcvs_npa.h"
#include "npa_resource.h"

#ifdef WINSIM
#include "Windows.h"
#else
#define Sleep(x)
#endif /* WINSIM */


/* Query handle to NPA resource */
static npa_query_handle clkCpuQueryHandle;

DCVS_LOCAL_CLOCK_PLAN_DATA_TYPE* dcvsClockPlan;




/**
  @brief DCVSTestCoreCPUCreateCompleteCB
  
  This API is notified from DCVS NPA layer after the creation of \node\core\cpu 
  is complete.

   @param event_id: Event which caused callback to be fired.
   @param pserver_data: optional pointer to data supplied by sleep task.
   @param pclient_data: optional pointer to data passed by client at time 
   of registration.

*/

void DCVSTestCoreCPUCreateCompleteCB( dcvs_npa_event event_id,
									  void* pserver_data,
									  void* pclient_data )
{
  /* Create a query handle to a resource /clk/cpu resource */ 
  CORE_VERIFY( ( clkCpuQueryHandle = npa_create_query_handle("/clk/cpu") ) );

}

void DCVSTestApiInitialize( void )
{
  uint32 numLevels;

  /* Register for an event after the creation of \node\core\cpu */
  //dcvs_npa_events_register(DCVS_NPA_CORE_CPU_CREATE_COMPLETE, DCVSTestCoreCPUCreateCompleteCB, NULL);

  DCVSInitialize();

  CoreCpuInitialize();

  /* Sleep for 2 seconds */
  Sleep(2000);

  npa_define_marker("/init/DCVS/Start");

  /* Get the target PC clock plan */
  dcvsClockPlan = DCVSTargetClockPlanLocalGet();
  numLevels = DCVSTargetClockPlanLocalNumLevelsGet();
  dcvsTest.maxFreq = dcvsClockPlan[numLevels-1].freqKhz;
  dcvsTest.maxMips = dcvsTest.maxFreq/1000;   
}





