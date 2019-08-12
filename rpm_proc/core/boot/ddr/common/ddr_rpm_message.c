/*
===========================================================================

FILE:         ddr_rpm_message.c

DESCRIPTION:  
  This is the ddr rpm message driver that will handle the rpm message 
  requests. There is an init function to register with the rpmfw with the
  callback function, and the callback function that will handle the message
  requests.

Copyright 2009-2011 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/boot/ddr/common/ddr_rpm_message.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
11/2/12    tw      Update status during init to report all banks active
06/28/11   tw      Initial version

===========================================================================

*/

/* -----------------------------------------------------------------------
**                           INCLUDES
** ----------------------------------------------------------------------- */
#include "ddr_common.h"
#include "ddr_drivers.h"
#include "ddr_sync.h"
#include "DALDeviceId.h"
#include "DDIRPMFW.h"
#include "err.h"

/* -----------------------------------------------------------------------
**                           TYPES
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           MACROS
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
**                           DATA
** ----------------------------------------------------------------------- */
static DALSYSEventHandle gRPMFWEvent;

static DAL_rpm_ResourceType ddrResources[] =
    { DAL_RPM_RESOURCE_DDR_DMM };

/* handle for the RPMFW driver */
static DALDEVICEHANDLE ddr_dmm_handle = NULL;     

/* -----------------------------------------------------------------------      
**                           FUNCTIONS                                          
** ----------------------------------------------------------------------- */   
       
/* ============================================================================
**  Function : ddr_rpmfw_process_request
** ============================================================================
*/
/*!
*   @brief
*   This function is used to process RPM message requests
*   
*   @details
*   This function is used to process RPM message requests
*      
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/


void *ddr_rpmfw_process_request(void *pCtxt, uint32 EffectiveImmediately, void *payload, uint32 payloadLen)
{
  DAL_rpmfw_RequestType *request = (DAL_rpmfw_RequestType *)payload;
  uint32 * request_data;

  DDR_SYNC_LOCK();

  /* verify params */
  do
  {
    /* if its not effective, don't process */
    if (!EffectiveImmediately)
      break;

    /* make sure the request is valid and has a valid master */
    if(!request || !RPM_IS_VALID_MASTER(request->Master))
      break;

    /* make sure its a DDR DMM */
    if(request->Resource != DAL_RPM_RESOURCE_DDR_DMM)
      break;
    
    /* check to make sure size is 8 bytes */
    if(request->DataLen != 8)
      break;

    /* do actual update */
    request_data = (uint32 *)(request->Data);
    ddr_dmm_partition(request_data[0], request_data[1]);

    // Update the DDR DMM Status
    DalRPMFW_SetStatus(ddr_dmm_handle, request->Resource, sizeof(request_data[0]) * 2, request_data, FALSE);

  } while(FALSE); 

  DDR_SYNC_UNLOCK();

  return NULL;

} /* DDR_RPMFW_Process_Request */

/* ============================================================================
**  Function : DDR_RPMFW_init
** ============================================================================
*/
/*!
*   @brief
*   This function is used to initialize the callback function for 
*   RPMFW to DDR_RPMFW_Process_Request when it receives a DDR DMM 
*   Resource message
*   
*   @details
*   This function is used to initialize the callback function for 
*   RPMFW to DDR_RPMFW_Process_Request when it receives a DDR DMM 
*   Resource message
*      
*   @par Dependencies
*   None
*   
*   @par Side Effects
*   None
*   
*   @retval  None
*   
*   @sa None
*/

void ddr_rpmfw_init( void )
{
  uint32 default_status[]  = {0,0};
  do
  {
    /* Attach to RPMFW */
    if(DAL_SUCCESS != DAL_RPMFWDeviceAttach(DALDEVICEID_RPMFW,&ddr_dmm_handle))
    {
      ERR_FATAL("Unable to attach to RPMFW", 0, 0, 0);
    }
    else
    {
      DALResult RetVal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CALLBACK_EVENT,
                                 &gRPMFWEvent, NULL);
      if(DAL_SUCCESS != RetVal) break;

      RetVal = DALSYS_SetupCallbackEvent(gRPMFWEvent,
                                         ddr_rpmfw_process_request, NULL);
      if(DAL_SUCCESS != RetVal) break;

      /* register internal driver.  internal drivers have no client context */
      RetVal = DalRPMFW_RegisterDriver(ddr_dmm_handle, /* context from RPMFW attach */
                                       sizeof(ddrResources)/sizeof(ddrResources[0]),
                                       ddrResources,
                                       gRPMFWEvent);
      if(DAL_SUCCESS != RetVal) break;
      
      /* Update DDR DMM Status to default all 0 */
      DalRPMFW_SetStatus(ddr_dmm_handle, DAL_RPM_RESOURCE_DDR_DMM, sizeof(default_status[0]) * 2, default_status, FALSE);
    }
  }
  while(FALSE);
  
}/* DDR_RPMFW_init */
