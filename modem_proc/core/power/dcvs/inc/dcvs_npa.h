#ifndef DCVS_NPA_H
#define DCVS_NPA_H
/**********************************************************************
 * dcvs_npa.h
 *
 * This is the header file of NPA layer for DCVS module.
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009-2011 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/inc/dcvs_npa.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    ---------------------------------------------------

09/25/11   sg     Moved requestStateVector definition to dcvs_npa*.c file.
02/18/11   ss     Adding context based processing for DCVS.
09/14/10   ss     Removing the "Last line of the file ends without a newline" 
                  warning.
06/13/10   ss     Adding an interface to issue zero state requests to 
                  bus and clock nodes .Another interface to change the  
                  state of core/cpu resource.
04/08/10   ss     Adding an interface to destroy the DCVS startup client
                  and its requests.
03/30/10   ss     Adding a new interface to register and deregister for 
                  DCVS NPA events.
08/25/09   ss     Initial check in.
========================================================================*/
#include "npa_resource.h"
#include "dcvs_core.h"
#include "dcvs_target.h"

/**
   Enumeration of the list of valid events which are sent out 
   by the DCVS NPA layer to its registered clients.
*/
typedef enum 
{
  DCVS_NPA_CORE_CPU_CREATE_COMPLETE,
  DCVS_NPA_EVENT_CLK_CHANGE,   /* Event to signal the state of the clock has changed */
  DCVS_NPA_EVENT_COUNT,        /**< Number of valid dcvs npa events. */
}dcvs_npa_event;


//#define MCA_ENABLE_DIAG_HOOKS


/*=======================================================================

                  PUBLIC FUNCTION DECLARATIONS

========================================================================*/


/**
   @brief Callback function prototype for DCVS NPA events

   @param event_id: Event which caused callback to be fired.
   @param pserver_data: optional pointer to data supplied by NPA.
   @param pclient_data: optional pointer to data passed by client at time 
   of registration.

*/
typedef void (*dcvs_npa_event_cb)(dcvs_npa_event event_id,
                                  void* pserver_data,
                                  void* pclient_data);

/**
   @brief dcvs_npa_events_register
   Function to register for notifications from DCVS NPA layer.

   This function should only be called from the client side of the API.
   The clients use this function to register for the events listed in
   dcvs_npa_event enum. They would be notified using the callback 
   which is passed into this function . 
   
   @param event_id: Event for which notification is required.
   @param cb: Callback function to be invoked when event_id is fired.
   @param pclient_data: optional client data which is relayed to cb on
   event trigger.

*/
void dcvs_npa_events_register(dcvs_npa_event event_id,
                              dcvs_npa_event_cb cb,
                              DCVS_CONTEXT* pclient_data);

/**
   @brief dcvs_npa_events_deregister

   Function to deregister for notifications from DCVS NPA layer.
   
   This function should only be called from the client side of the API.
   The clients use this function to deregister for the events listed in
   dcvs_npa_event enum. 
   
   @param event_id: Event for which notification is required to stop.
   @param cb: Callback function which was being invoked on event_id fire.
   @param pclient_data: optional client data which is used to when
   registering the callback using the dcvs_npa_events_register().

*/
void dcvs_npa_events_deregister(dcvs_npa_event event_id,
                                dcvs_npa_event_cb cb,
                                DCVS_CONTEXT* pclient_data);




/**
  @brief DCVSIssueInternalRequest

  This function is called when the mips calculated by the DCVS algorithm is changed.
  The new mips is then requested to the bus and clocks.

  @param context: The DCVS data context.

  @return : None.

*/
void DCVSIssueInternalRequest(DCVS_CONTEXT* context);


/**
  @brief DCVSStartupClientDestroy

  This function is used to destroy the startup client thereby removing its
  requests.

  @param context: Context for each core.

  @return : None.

*/
void DCVSStartupClientDestroy(DCVS_CONTEXT* context);

/**
  @brief DCVSZeroStateRequestIssue

  This function is used to issue zero state requests to bus and clock nodes for a 
  particular core.

  @param sleepEntered : Indicated whether the event was a sleep event entry or exit.
  TRUE : Indicates a sleep event entry.
  FALSE : Indicates a sleep event exit.

  @param context: The DCVS data context.
  @return : None.

*/
void DCVSZeroStateRequestIssue(DALBOOL sleepEntered, DCVS_CONTEXT* context);

/**
  @brief CoreCpuResStateChange

  This function is used to change the state of /core/cpu resource.
  Here "*" refers to the  coreId .

  @param state : The new state to which the /core/cpu resource needs to be assigned
  @param context: The DCVS data context.
  @return : None.

*/
void CoreCpuResStateChange(uint32 state, DCVS_CONTEXT* context);


/**
  @brief NodeCoreCpuCreate

  This function is used to create the /node/core/cpu for DCVS algorithm.

  @param targetNodeDeps  : The dependency resources for this target.
  @param numNodeDeps     : The number of dependency resources for this target.
  @param targetSysDeps   : The external system dependencies for this target.
  @param numSysDeps      : The number of external system dependencies for this target.
  @param coreId          : The id of the core. 
  @param context         : The DCVS data context.
         
  @return : None.
*/

void NodeCoreCpuCreate (npa_node_dependency* targetNodeDeps, uint32 numNodeDeps, const char** targetSysDeps, uint32 numSysDeps, uint32 coreId, DCVS_CONTEXT* context);


void NodeCoreMcaCreate(void);

#endif /* DCVS_NPA_H */

