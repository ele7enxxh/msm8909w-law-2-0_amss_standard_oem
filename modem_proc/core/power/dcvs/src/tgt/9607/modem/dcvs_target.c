/*============================================================================
  FILE:         dcvs_target.c
 
  OVERVIEW:     This file provides the target specific code for DCVS.
 
  DEPENDENCIES: None
 
                Copyright (c) 2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE
  
  $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/tgt/9607/modem/dcvs_target.c#1 $
  $DateTime: 2016/12/13 07:59:23 $

  when       who     what, where, why
  --------   ----    ---------------------------------------------------
  12/11/12   nj      Correction in Bus Driver Fn for the processor name  
  11/14/12   nj      Bus arbiter client changed to type NPA_CLIENT_SUPPRESSIBLE_VECTOR
  03/16/12   nj      Initial version.
========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "dcvs_target.h"
#include "npa_resource.h"
#include "dcvs_nv.h"
#include "dcvs_algorithm.h"
#include "dcvs_npa.h"
#include "CoreVerify.h"
#include "icbarb.h"
#include "icbid.h"

/* Number of Master slave pairs which for the 8660 Modem target */
#define DCVS_MS_COUNT 1 

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/* The DCVS data context for the target */
DCVS_CONTEXT dcvsContext;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * This section contains and defines all target specific DCVS NV items.
 * -------------------------------------------------------------------------*/

static const DCVS_NV_PARAM_ENTRY dcvsTargetAlgParamsStartup[] =
{
  {"Algorithm", DCVS_NV_PARAM_STRING, {"qdsp_classic"}},
  {"LogSize", DCVS_NV_PARAM_UINT32, {(void*)16384}},
  {NULL, DCVS_NV_PARAM_NULL, {(void*)0}},
};

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

const DCVS_NV_PARAM_LIST dcvsTargetNvDefaults[] =
{
  {"Startup", DCVS_NV_CORE_ID_ALL, dcvsTargetAlgParamsStartup},  
  {NULL, 0, NULL}
};

/*----------------------------------------------------------------------------
 * List of available algorithms for this target.
 * -------------------------------------------------------------------------*/

extern DCVS_CORE_ALGO disabledAlgorithm;
extern DCVS_CORE_ALGO fixedAlgorithm;
extern DCVS_CORE_ALGO requestsAlgorithm;
extern DCVS_CORE_ALGO qdspClassicAlgorithm;

const DCVS_CORE_ALGO* dcvsTargetAlgList[] = 
{ 
  &disabledAlgorithm, 
  &fixedAlgorithm, 
  &requestsAlgorithm,
  &qdspClassicAlgorithm,
  NULL
};


/* Structure to maintain the internal state of the DCVS target specific members */
typedef struct 
{
  npa_client_handle clientHandle;
  
  /* Vector of request states for the master/slave pairs */
  ICBArb_RequestType vecReq[DCVS_MS_COUNT];
  
}DCVS_TARGET_STRUCT;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static npa_resource_state bus_driver_fcn ( npa_resource *resource,
                                            npa_client *client,
                                            npa_resource_state state )
{
  uint32 i;
  // Extract the DCVS context ; 
  DCVS_CONTEXT* dcvsContext = (DCVS_CONTEXT*)resource->definition->data;
  npa_resource_state busReqState;

  if(client->type == NPA_CLIENT_INITIALIZE)
  {
    /* We create the initial states for the bus arbiter */
    /* There are 2 master/slave pairs which are affected by the DCVS */  
    ICBArb_MasterSlaveType msArray[] = 
    {
      {ICBID_MASTER_MSS_PROC , ICBID_SLAVE_EBI1}
    };

    /* Create a client for the bus arbiter */
    if ((((DCVS_TARGET_STRUCT*)dcvsContext->targetDataContext)->clientHandle = icbarb_create_suppressible_client_ex("/node/core/cpu/bus", 
                                                                                                    msArray, 
                                                                                                    DCVS_MS_COUNT, NULL))==NULL)
    {
      /* Error Fatal */
      CORE_VERIFY_PTR(NULL);
    }

    /* Set up the vector of request states for the bus arbiter . */
    for(i=0; i<DCVS_MS_COUNT; i++) 
    {
      /* Indicate we are using CPU-style master */
      ((DCVS_TARGET_STRUCT*)dcvsContext->targetDataContext)->vecReq[i].arbType = ICBARB_REQUEST_TYPE_2;
      ((DCVS_TARGET_STRUCT*)dcvsContext->targetDataContext)->vecReq[i].arbData.type2.uUsagePercentage = 0;
    }
    /* Get the maximum frequency of the target in MIPS. */
    busReqState = DCVSCoreTargetMaxFreqGet(dcvsContext);
   
    /* Convert it into MB/s */
    busReqState = DCVSCoreMipsToMBpsConvert(busReqState);

  }

  /* Convert the state(in megabytes/s) to bytes/s. */
  busReqState = state * 1000000;
  /* Set up the vector of request states for the bus arbiter . */
  for(i=0; i<DCVS_MS_COUNT; i++) 
  {
    ((DCVS_TARGET_STRUCT*)dcvsContext->targetDataContext)->vecReq[i].arbData.type2.uThroughPut = busReqState;
  }

  if(((DCVS_TARGET_STRUCT*)dcvsContext->targetDataContext)->clientHandle != NULL)
  {
    // Link to /icb/arbiter with the vector client request.
    icbarb_issue_request(((DCVS_TARGET_STRUCT*)dcvsContext->targetDataContext)->clientHandle, 
                         ((DCVS_TARGET_STRUCT*)dcvsContext->targetDataContext)->vecReq, 
                         DCVS_MS_COUNT);
  }
  /* Return the state to the /core/cpu/bus resource */
  return state;

}


/* Variable list of dependencies of "node/core/cpu/bus" node to other nodes */
static npa_node_dependency bus_deps[] =   
{
  {"/icb/arbiter", NPA_NO_CLIENT}, 
};

static npa_resource_definition bus_res[] =
{
  {"/core/cpu/bus", 
    "MB/s",
    0xffffffff, // Max mips * 4
    &npa_identity_plugin,  // The resource plugin.
    NPA_RESOURCE_DEFAULT,  // The resource attributes.
    &dcvsContext}          // The user data field.
};

/* Definition of the node/core/cpu */
static npa_node_definition node_bus = 
{ 
  "node/core/cpu/bus",  /* name */
  bus_driver_fcn,  /* driver_fcn */
  NPA_NODE_DEFAULT, /* attributes */
  NULL,             /* data */
  NPA_ARRAY(bus_deps),
  NPA_ARRAY(bus_res)
};

/* Array to maintain the target specific dependency resources . */
npa_node_dependency targetNodeDeps[] = 
{
  /* Clock Resource */
  {
    "/clk/cpu", /* Name of resource */
    NPA_CLIENT_REQUIRED, /* Type of client to create */
    NULL /* Client Handle - created by NPA */
  },
  /* Bus resource */
  {
    "/core/cpu/bus", /* Name of resource */
    NPA_CLIENT_REQUIRED, /* Type of client to create */
    NULL /* Client Handle - created by NPA */
  }                                 
};

/* Array to maintain the external system dependencies  */
const char* targetSysDeps[] = {"/init/dcvs/startup"};

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

void DCVSTargetSpecificInit (void)
{
  npa_resource_state initialState[] = 
  {
    0
  };
  /****** Create the memory for the target data**********/
  if(DALSYS_Malloc(sizeof(DCVS_TARGET_STRUCT),  
                   (void**) &dcvsContext.targetDataContext)
                   != DAL_SUCCESS) 
  {
    /* Error fatal */
    CORE_VERIFY(NULL);   
  }
  memset(dcvsContext.targetDataContext, 0, sizeof(DCVS_TARGET_STRUCT)); 

  npa_define_node( &node_bus, initialState, NULL);
  NodeCoreCpuCreate(targetNodeDeps, sizeof(targetNodeDeps)/sizeof(npa_node_dependency), (const char**)&targetSysDeps, sizeof(targetSysDeps)/sizeof(const char*), 0, &dcvsContext);

}

void MCATargetSpecificInit(void)
{
  NodeCoreMcaCreate();
}


