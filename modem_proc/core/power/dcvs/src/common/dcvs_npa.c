/**********************************************************************
 * dcvs_npa.c
 *
 * This module implements the NPA layer for DCVS module.
 *
 * EXTERNALIZED FUNCTIONS
 *
 * Copyright (C) 2009 - 2014 by Qualcomm Technologies, Inc.
 * All Rights Reserved.
 *
 **********************************************************************/
/*=======================================================================
                        Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/dcvs_npa.c#1 $ 
$DateTime: 2016/12/13 07:59:23 $

when       who     what, where, why
--------   ----    --------------------------------------------------------------------
08/24/12   sg      Log events on QDSS tracer.
09/25/11   sg      Define /init/dcvs/startup marker node in DCVSInitialize();
                   Moved requestStateVector definition from dcvs_npa.h file.
04/14/11   ss      Changing the initialization sequence to enable the clients to use the 
                   /core/cpu resource as soon as it is constructed rather than waiting 
                   for external system dependencies. 
02/28/11   ss      Using memset to initialiize the allocated memory.
02/18/11   ss      Adding context based processing for DCVS.
01/21/11   ss      Initializing resource handle for each of the cores.  
10/22/10   ss      Using the DCVSCoreClockPlanIndexGet API to obtain index of clock plan table.
10/08/10   ss      Making the cpu frequency and bus frequency requests from the lookup 
                   table of clock plan frequencies of DCVS. 
09/14/10   ss      Removing the "Statement not reachable" and "Last line of the 
                   file ends without a newline" warnings.
08/12/10   ss      Renaming the /core/cpu client from "CPU Dynamics Timer"
                   to "CPU_Dynamics_Mips_Changed_or_Timer". 
07/08/10   ss      Requesting the maximum mips of the target at the
                   core_cpu_driver function . 
07/01/10   ss      Changing the type of pendingRequestMips and 
                   activeRequestMips to be of unsigned ints.
07/01/10   ss      Changing the requests made by the init client of clock 
                   and bus resources during start up.
06/17/10   ss      Removing unused NPA clients from the code.  
06/15/10   ss      Resetting the state of /core/cpu resource on entering
                   TCXO shutdown on 7x30 Modem.
04/08/10   ss      Adding a startup client to issue request for max. mips
                   before the actual DCVS algorithm starts.
03/30/10   ss      Adding the DCVS NPA layer event notification interfaces 
                   used to register / deregister for notifications.
03/30/10   ss      Adding real_dcvs_init to initialize the core cpu node 
                   and the remaining portion of the DCVS separately. 
02/21/10   ss      Invoking CPU monitor when the /clk/cpu resource
                   changes state. 
01/20/10   bjs     Cleanup
08/25/09   ss      Initial check in.
===========================================================================================*/
#include "BuildFeatures.h"

#include "dcvs_os.h"
#include "dcvs_trace.h"
#include "dcvs_npa.h"
#include "dcvs_init.h"
#include "CoreString.h"
#include "CoreVerify.h"
#include "dcvs_target.h"
#include "CoreCpu.h"
#include "qurt.h"
#include "max_stub.h"
#include "ULogFront.h"
#include "CLM.h"

//#include DCVS_OS_HEADER1_H

int32 cpuCount = MAX_THREADS;


/*=======================================================================

                 DCVS_NPA EVENT NOTIFICATION function definitions

========================================================================*/

/**
  @brief dcvs_npa_events_initialize

  The function is used to initialize the event notification mechanism using
  the core event notification apis. 

  @param context: The DCVS Data context.
  
  @return : None.

*/
static void dcvs_npa_events_initialize(DCVS_CONTEXT* context)
{
  /* Obtain the client context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  if(resDataContext->eventHandle == NULL) 
  {
    resDataContext->eventHandle = Core_EventInitialize(&context->resDataContext->notifyEvent, 
                                                                                         DCVS_NPA_EVENT_COUNT);
    CORE_VERIFY_PTR(resDataContext->eventHandle);
  }
  return;
}


/**
  @brief dcvs_npa_events_register

  The function is used to register for the event notification mechanism using
  the core event notification apis. 

  @param  event_id: The id which identifies the type of event .
  @param cb: The callback function which is notified when the registered event occurs.
  @param context: The DCVS Data context.

  @return : None.

*/
void dcvs_npa_events_register(dcvs_npa_event event_id, dcvs_npa_event_cb cb, 
                              DCVS_CONTEXT* context)
{
  /* Obtain the client context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;  
  /* Make sure we are initialized */
  dcvs_npa_events_initialize(context);

  Core_EventRegister(resDataContext->eventHandle, event_id, (Core_EventCallback)cb, 
                     CORE_EVENT_PRIORITY_HIGHEST, context);

  return;
}

/**
  @brief dcvs_npa_events_deregister

  The function is used to deregister for the event notification mechanism using
  the core event notification apis. 

  @param  event_id: The id which identifies the type of event which is registered.

  @param cb: The callback function which is notified when the registered event occurs.
  @param context: The DCVS Data context.

  @return : None.

*/
void dcvs_npa_events_deregister(dcvs_npa_event event_id, dcvs_npa_event_cb cb, 
                                DCVS_CONTEXT* context)
{
  /* Obtain the client context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  /* Make sure we are initialized */
  dcvs_npa_events_initialize(context);
    
  Core_EventDeRegister(resDataContext->eventHandle, event_id, (Core_EventCallback)cb, 
                       CORE_EVENT_PRIORITY_HIGHEST, context);

  return;
}

/**
  @brief dcvs_npa_events_notify

  The function is used to notify the clients for the registered events of type event_id .

  @param client_data: The DCVS Data context.
  
  @return : None.

*/
void dcvs_npa_events_notify(dcvs_npa_event event_id, DCVS_CONTEXT* context)
{
  /* Obtain the client context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  /* Make sure we are initialized */
  dcvs_npa_events_initialize(context);

  Core_EventNotify(resDataContext->eventHandle, event_id, context);

  return;
}



/*=======================================================================

                  LOCAL FUNCTION DEFINITIONS

========================================================================*/


/**
  @brief ResDataMemoryAllocateAndInitialize
  
  This function is used to allocate and initialize memory for the resource data.
  @param context: The DCVS data context.

  @return: None.

*/
static void ResDataMemoryAllocateAndInitialize(DCVS_CONTEXT* context)
{
  
  DCVS_RES_DATA* resDataContext;
  if(context->resDataContext == NULL)
  {
    /* Allocate the memory for storing the client information for the resources */
    if(DALSYS_Malloc(sizeof(DCVS_RES_DATA),  
                     (void**) &resDataContext)
                     != DAL_SUCCESS) 
    {
      /* Error fatal */
      CORE_VERIFY(NULL);   
    }
    /* Initialize the allocated memory. */
    memset(resDataContext, 0, sizeof(DCVS_RES_DATA));
    context->resDataContext = resDataContext;

  }
 
}

/**
  @brief res_core_cpu_update_fcn
  
  This function is invoked by the /core/cpu resource when a client request for new 
  mips is made. It gets the new mips from the algorithm .

  Note on active and pending requests of a client ::

  Each client has an active and pending request ( double buffered ) 

  The clients have 2 entries in the work buffer. One is used for the active request, other for the pending request.
  NPA_ACTIVE_REQUEST always returns the state in the current index of the buffer.
  NPA_PENDING_REQUEST always returns the state in the other index of the buffer.
  The index of the work buffer is shifted in the 
  npa_request_serviced ( from npa_client_complete_request) which is done after issuing request against the resource by the client.
  
  When a request against a resource is issued, then the update function of the node(having the resource) 
  is invoked. Inside the update function the pending state of the client would be the state just requested. 
  The return value of the update function represents the state request ( still pending request ) against the resource. 
  Then the corresponding driver function of the resource is called with the state request from the update function
  and the resource state is updated. 
  
  @param resource: A dynamic system element that work requests can be made against.
  @param client: The handle to the clients registered to the resource.

  @return : None.

*/
static npa_resource_state res_core_cpu_update_fcn( npa_resource *resource,
                                                   npa_client_handle client )
{
  npa_resource_state newMips = 0;

  /* Keep track of the new incoming requests for the client*/
  uint32  pendingRequestMips = 0;

  /* Keep track of the active request for the client*/
  uint32  activeRequestMips = 0;

  /* Obtain the DCSV data context which is stored in the resource. */
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)resource->definition->data;

  /* Obtain the client context from the resource. */
  DCVS_RES_DATA* resDataContext= context->resDataContext;
  
  /* Ensure that the client is not NULL */
  CORE_VERIFY(client);


  switch (client->type)
  {
    case NPA_CLIENT_REQUIRED:
      /* Required client request*/
      if ( npa_request_has_attribute( client, NPA_REQUEST_BEST_EFFORT ) ) 
      {

        uint64 currentTime = qurt_sysclock_get_hw_ticks();
        uint64 lastLPRexitTime = CLM_GetLastLPRExitTime();

        uint32 deltaSysTicks = currentTime - lastLPRexitTime; 
        uint64 elapsedTimeUs = ((uint64)deltaSysTicks * 1000000)
                               / (uint64)TICKS_PER_SECOND;

        if ( elapsedTimeUs < (uint64)BEST_EFFORT_IGNORED_REQUEST_DURATION )
        {
          /* Ignore the client's request. Reset PENDING_REQUEST to active
             request so that a subsequent request for "state" is not
             short-circuited by NPA */
          NPA_PENDING_REQUEST( client ).state = NPA_ACTIVE_REQUEST( client ).state;
          ULogFront_RealTimePrintf( context->coreDataContext->logHandle, 4,
                                    "Ignoring request from BEST_EFFORT client as"
                                    " %dd is within 3ms of %dd",
                                    ULOG64_DATA( currentTime ),
                                    ULOG64_DATA( lastLPRexitTime ) ); 
          break;
        }
      }
      /* Get the pending new request  from the client */
      pendingRequestMips = NPA_PENDING_REQUEST(client).state;

      // ToDo: Use resource max.
      if (pendingRequestMips > resDataContext->maxMips)
      {
        /* The requested mips is more than that supported by the target 
           Set the requested mips to the max mips on the target */
        pendingRequestMips = resDataContext->maxMips;
      }

      /* Get the previous active request of the client */
      activeRequestMips = NPA_ACTIVE_REQUEST(client).state;

      if (activeRequestMips > resDataContext->maxMips)
      {
        /* The previous active mips is more than that supported by the target 
         Set the  mips to the max mips on the target */
        activeRequestMips = resDataContext->maxMips;
      }

      resDataContext->clientDelta = pendingRequestMips - activeRequestMips;

      resDataContext->activeFloor = resDataContext->activeFloor + resDataContext->clientDelta;
      break;

    case NPA_CLIENT_IMPULSE:
      /* Impulse client request */
      resDataContext->activeImpulse = TRUE;
      break;

    case NPA_CLIENT_ISOCHRONOUS:
      /* Isochronous client request */
      if (NPA_PENDING_REQUEST(client).state == 0)
      {
        resDataContext->activeIsochronous--;
      }
      else
      {
        resDataContext->activeIsochronous++;
      }
      break;

    case NPA_CLIENT_INTERNAL:
      /* Internal client request */

      if (resDataContext->clkChangePriorityInit == 0)
      {
        // Only do this once
        resDataContext->clkChangePriorityInit = 1;

        // Read the current thread priority (from which the timer callback was
        // invoked) and store it for use as our clock change event priority.
        // We will use this priority (we're assuming it is fairly high)
        // instead of using the caller's priority when a clock change callback
        // is called - that way we ensure that we'll switch the clock at a
        // high priority even if the caller of clock change callback is of a
        // lower priority (CR 264711).
        resDataContext->clkChangePriority = DALSYS_SetThreadPriority(0);
      }

      /* DCVS Timer has kicked us.  Request new MIPS. */
      resDataContext->algoMipsChangedOrTimerFired = 1;
      break;

    default:
      break;
  } /* End of switch */

  if ( TRUE == resDataContext->dcvsInit )
  {
    /* DCVS subsystem is initialized . Get the new mips */
    newMips = DCVSCoreMipsGet(context);
  }
  else
  {
    /* Return the max mips for now till DCVS is initialized */
    newMips = resDataContext->maxMips*MAX_THREADS;
  }

  return newMips;
}


/**
  @brief node_core_cpu_driver_fcn

  This is the driver function for DCVS node. This function issues state requests
  to the clocks and bus. 

  @param resource: A dynamic system element that work requests can be made against.
  @param client: The handle to the clients registered to the resource.
  @param state: Work request type.

  @return : None.

*/
static npa_resource_state node_core_cpu_driver_fcn ( npa_resource *resource,
                                                     npa_client *client,
                                                     npa_resource_state state )
{
  uint32 khz, mb_s;

  /* Obtain the core context from the resource */
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)resource->definition->data;

  DCVS_RES_DATA* resDataContext = NULL;

  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    /* Allocate the memory for the resource data if not already done so. */
    ResDataMemoryAllocateAndInitialize(context);

    resDataContext = context->resDataContext;

    resDataContext->maxMips = DCVSCoreTargetMaxFreqGet(context);

    /* Obtain the pointer to the clock plan of the target. */
    resDataContext->clockPlan = DCVSCoreClockPlanGet(context);

    /* This is the initialize client to the bus and clock resources */
    /* Initialize the resource max and active max for the /core/cpu reource. */
    resource->active_max = resDataContext->maxMips;
    resource->definition->max = resDataContext->maxMips;
    /* Initially request for max mips */
    state = resDataContext->maxMips;
  }

  if(resDataContext == NULL)
  {
    resDataContext = context->resDataContext;
  }

  if (resource == NPA_RESOURCE( resource, 0))
  {
    int32 index = DCVSCoreClockPlanIndexGet( state, context );
    
    /* If an index < 0 is returned, we issue 0 requests for Q6 clock and bus */
    if ( index < 0 )
    {
      khz = mb_s = 0;
    }
    else
    {
      /* Lookup the CPU frequency in KHz and Bus bandwidth in MBs from the
         clock plan table. */
    khz = resDataContext->clockPlan->clockPlanTable[index].cpuFreq;
    mb_s = resDataContext->clockPlan->clockPlanTable[index].busThroughput;
    }

    // Log this on QDSS
    DcvsTraceCpuDynamics(khz, mb_s);
    
    /* Request into the clock resource */
    npa_issue_required_request( NPA_DEPENDENCY(resource, 0), khz );

    /* Request into the bus resource */
    npa_issue_required_request( NPA_DEPENDENCY(resource, 1), mb_s );
  }

  return state;
}


/**
  @brief clk_cpu_change_cb

  This function is the callback which gets invoked everytime the
  state of /clk/cpu resource changes.

  @param user_data: The optional user data passed in this callback.
  @param cb_event_type:  NPA event type
  @param cb_data: NPA event data structure passed to this callback.
  @param size: size of the NPA event data structure.

  @return : None.

*/

static void *clk_cpu_change_cb( void *user_data, 
                                uint32 cb_event_type,
                                void *cb_data, 
                                uint32 size )
{
  /* Obtain the DCVS data context from the resource */
  DCVS_CONTEXT* context = (DCVS_CONTEXT*)user_data;

  /* Obtain the client context from the resource. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;
  
  if ( TRUE == resDataContext->dcvsInit )
  {
    uint32 previousPriority;

    /* Notify all the registered clients who requested for the callbacks 
	      from this event */
    previousPriority = DALSYS_SetThreadPriority(resDataContext->clkChangePriority);
    dcvs_npa_events_notify(DCVS_NPA_EVENT_CLK_CHANGE, context);
    DALSYS_SetThreadPriority(previousPriority);
  }
  return(NULL);
}



/**
  @brief DCVSInitializationCb

  This function is the callback after the file system and other DCVS system
  dependencies become available. After the /node/core/cpu is created  
  it waits for the system dependencies to be available during  
  system initialization and this call back is invoked.

  When invoked, the callback arguments will be
  (context, 0, *resource_name, strlen( resource_name ) );

  @param  context: The DCVS data context.
  @param  event_type: The type of the event.
  @param  data: The name of the node being created.
  @param  data_size: The length of the node name.

  @return : None.

*/
static void DCVSInitializationCb(void         *context,
                                 unsigned int event_type, 
                                 void         *data,       
                                 unsigned int data_size)
{
  DCVS_CONTEXT *dcvsContext = (DCVS_CONTEXT*)context;
  DCVS_RES_DATA  *resDataContext = dcvsContext->resDataContext;

  /* Initialize the os-specific portion of DCVS */
  DCVSOSInitialize(dcvsContext); 
  /* Initialize the core data portion of the DCVS */
  DCVSCoreInitialize(dcvsContext);
  /* Mark the DCVS subsystem as initialized. */
  resDataContext->dcvsInit = TRUE;

}

/**
  @brief NodeCoreCpuCreationCb

  This function is the callback from the single resource node 
  created while initializing DCVS.  
  The callback will be invoked after the node is fully
  constructed.  The callback can be NULL if creation notification is
  unneeded. If invoked, the callback arguments are (context, 0,
  node->name, sizeof(const char *)).
  
  @param  context: The DCVS data context.
  @param  event_type: The type of the event.
  @param  data: The name of the node being created.
  @param  data_size: The length of the node name.

  @return : None.

*/
static void NodeCoreCpuCreationCb(  void         *context,
                                    unsigned int event_type, 
                                    void         *data,       
                                    unsigned int data_size )
{

  DCVS_CONTEXT *dcvsContext = (DCVS_CONTEXT*)context;
  /* Obtain the resource context */
  npa_resource_definition *resourceContext = (npa_resource_definition*)dcvsContext->resourceContext;
  DCVS_RES_DATA  *resDataContext;

  /* Query handle to NPA resource */
  npa_query_handle queryHandle;
  
  /* Allocate the memory for the resource data if not already done so. */
  ResDataMemoryAllocateAndInitialize(dcvsContext);

  resDataContext = dcvsContext->resDataContext;

  resDataContext->handleClient = npa_create_async_client(resourceContext->name,
                                                         "CPU_Dynamics_Mips_Changed",
                                                         NPA_CLIENT_INTERNAL,
                                                         NULL);
  CORE_VERIFY(resDataContext->handleClient);

  /* Create a startup only client for /core/cpu resource */
  resDataContext->startupClient = npa_create_async_client(resourceContext->name, 
                                                          "DCVS_Startup_Client",
                                                          NPA_CLIENT_REQUIRED,
                                                          NULL);
  CORE_VERIFY(resDataContext->startupClient);

  CORE_VERIFY_PTR( queryHandle = npa_create_query_handle(resourceContext->name) );
  CORE_VERIFY_PTR( resDataContext->resHandle = npa_query_get_resource( queryHandle ) );


  /* Set up the change event for the CPU monitor */
  /* Create a callback event for state change of /clk/cpu resource */
  CORE_DAL_VERIFY( DALSYS_EventCreate( DALSYS_EVENT_ATTR_CALLBACK_EVENT,
                                       &resDataContext->clockChangeEvent,
                                       NULL ) );

  /* Set up a callback function to be notified when coreCpuChangeEvent occurs */
  resDataContext->clkChangePriority = 237;
  CORE_DAL_VERIFY( DALSYS_SetupCallbackEvent( resDataContext->clockChangeEvent, 
                                              clk_cpu_change_cb, context ) );

  CORE_VERIFY( npa_create_change_event( dcvsContext->clockDependency, 
                                        "Clock_Change_Event", 
                                        resDataContext->clockChangeEvent) );

  /* Signal the succesful creation of node and the clients to the node */
  resDataContext->nodeCreateComplete = TRUE;

  /* Issue a request for maximum mips before the actual DCVS algorithm starts */
  npa_issue_required_request(resDataContext->startupClient, resDataContext->maxMips);

  #ifdef WINSIM
  /* Notify the test framework that the /node/core/cpu creation is complete */
  //  dcvs_npa_events_notify(DCVS_NPA_CORE_CPU_CREATE_COMPLETE, NULL);
  #endif
  

  /* Register and wait for the DCVS external system dependencies to be available */
  npa_resources_available_cb(dcvsContext->numSysDeps,
                             dcvsContext->sysDeps,
                             DCVSInitializationCb,
                             dcvsContext);
}


/* The plugin definition for /core/cpu resource */
const npa_resource_plugin res_core_cpu_plugin = 
{
  res_core_cpu_update_fcn,
  NPA_CLIENT_REQUIRED | 
  NPA_CLIENT_ISOCHRONOUS |
  NPA_CLIENT_IMPULSE |
  NPA_CLIENT_INTERNAL,          /* Supported client types */
  NULL,                         /* Create client function */
  NULL                          /* Destroy client function */
};



/*=======================================================================

                 PUBLIC FUNCTION DEFINITIONS

========================================================================*/


void CoreCpuInitialize( void )
{

  /* Initialize each core in the multicore environment of DCVS */
  DCVSTargetSpecificInit();

  MCATargetSpecificInit();

}


void DCVSIssueInternalRequest(DCVS_CONTEXT* context)
{
  /* Obtain the client The DCVS data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  /* Proceed only if the /node/core/cpu creation is complete */
  if ( resDataContext->nodeCreateComplete == TRUE )
  {
    npa_issue_internal_request(resDataContext->handleClient);
  }
}


/**
  @brief DCVSInitialize

  This function is empty for now.

  @param  : None.

  @return : None.

*/
void DCVSInitialize(void)
{
  npa_define_marker("/init/dcvs/startup");
}

void DCVSStartupClientDestroy(DCVS_CONTEXT* context)
{
  /* Obtain the client The DCVS data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  npa_destroy_client(resDataContext->startupClient);
}

void DCVSZeroStateRequestIssue(DALBOOL sleepEntered, DCVS_CONTEXT* context)
{
  /* Obtain the client The DCVS data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  NPA_PENDING_REQUEST(resDataContext->zeroRequestClient).state = sleepEntered;
  npa_issue_internal_request(resDataContext->zeroRequestClient);

}
void CoreCpuResStateChange(uint32 state, DCVS_CONTEXT* context)
{
  /* Obtain the client The DCVS data context. */
  DCVS_RES_DATA* resDataContext = context->resDataContext;

  NPA_PENDING_REQUEST(resDataContext->stateChangeClient).state = state;
  npa_issue_internal_request(resDataContext->stateChangeClient);

}



void NodeCoreCpuCreate (npa_node_dependency* targetNodeDeps, uint32 numNodeDeps, const char** targetSysDeps, uint32 numSysDeps, uint32 coreId, DCVS_CONTEXT* context)
{
  npa_node_definition *nodePtr = NULL; 
  /* Initial state of the resource */
  npa_resource_state initialState[] = 
  {
    0
  };

  /****** Create the resource memory **********/
  if(DALSYS_Malloc(sizeof(npa_resource_definition),  
                   (void**) &context->resourceContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  memset(context->resourceContext, 0, sizeof(npa_resource_definition)); 

  /* Note that the target node dependencies information as well as system dependencies information need to be persistent till the 
     end . We just use pointers to those structures. The structures need to remain valid till the end. */

  /* Fill in the target system dependency information. Note that we are copying the pointer to the memory area where the system 
     dependency information is present . This meory area has to be persistent till the end. */
  context->sysDeps = targetSysDeps;
  context->numSysDeps = numSysDeps;


  /* Fill up the resource information. */  
  ((npa_resource_definition *)context->resourceContext)->name = "/core/cpu";                 /* Name of resource */
  ((npa_resource_definition *)context->resourceContext)->units = "MIPS";                      /* Units for this resource */
  ((npa_resource_definition *)context->resourceContext)->max = 0;                             /* Maximum state */
  ((npa_resource_definition *)context->resourceContext)->plugin = &res_core_cpu_plugin;     /* Plugin */
  ((npa_resource_definition *)context->resourceContext)->attributes = NPA_RESOURCE_DEFAULT;   /* Resource Attributes */
  ((npa_resource_definition *)context->resourceContext)->data = context;          /* User data field  */
  ((npa_resource_definition *)context->resourceContext)->query_fcn = NULL;                    /* Query fcn */

  
  /****** Create the memory for single resource node **********/
  if(DALSYS_Malloc(sizeof(npa_node_definition),  
                   (void**) &nodePtr)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }  
  memset(nodePtr, 0, sizeof(npa_node_definition));

  nodePtr->name = "/node/core/cpu";                /* Node name */
  nodePtr->driver_fcn = node_core_cpu_driver_fcn;  /* Driver function used to actually update resource */
  nodePtr->attributes = NPA_NODE_DEFAULT;  /* Node Attributes */
  nodePtr->data = NULL;                    /* User supplied data */
  nodePtr->resources = ((npa_resource_definition *)context->resourceContext); /* The resources for this node. */
  nodePtr->resource_count = 1;             /* The number of resources */
  nodePtr->dependencies = targetNodeDeps;        /* The information about the dependencies */
  nodePtr->dependency_count = numNodeDeps;       /* The number of dependencies */

  /* The clock dependency name for the particular clock resource */
  context->clockDependency = targetNodeDeps[0].name;
    
  npa_define_node_cb(nodePtr, initialState, NodeCoreCpuCreationCb, context);
  
}



