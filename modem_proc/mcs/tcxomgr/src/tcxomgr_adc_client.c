/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

              T C X O    M A N A G E R   A D C   C L I E N T

                        TCXO Manager Source File

GENERAL DESCRIPTION

  This module maintains the client interface into the ADC DAL. It replaces
  the now *OBSOLETE* legacy ADC interface which consisted of:

   - adc_read_channel()
   - adc_read()
   - therm_read()

  These APIs are now deprecated and clients should use the new ADC DAL client
  interface provided by this module.

  The new DAL API exposes the following interfaces to the client:

   - DAL_AdcDeviceAttach()
   - DalAdc_GetInputProperties()
   - DalAdc_RequestConversion()
   - DalAdc_RequestRecalibration()

  These DAL API are used inside this module and made available through the
  client interface.

EXTERNALIZED FUNCTIONS

  tcxomgr_adc_client_init()
  tcxomgr_adc_client_adc_read()
  

REGIONAL FUNCTIONS

  tcxomgr_adc_client_blocking_read_cb()
  tcxomgr_adc_client_blocking_read_init()
  tcxomgr_adc_client_blocking_read()
  tcxomgr_adc_client_buffered_read_cb()
  tcxomgr_adc_client_buffered_read_init()
  tcxomgr_adc_client_buffered_read()

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/src/tcxomgr_adc_client.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     --------------------------------------------------------- 
08/13/15     tl      Release ADC handle when shutting down
07/09/15     tl      Clean up unused code and interfaces
01/30/15     tl      Remove obsolete featurization
11/12/14     tl      Send temperature update failures to Tcxomgr task
06/30/14     tl      Added MOB support
06/27/13     mn      Changed DALSYS_memcpy usage to memscpy.
11/28/11     mn      Added invalid temp checks in tcxomgr_report_new_temp
11/01/11     ns      Add support for Tcxo mode
06/27/11     ag      Reports new temp readings to tcxomgr task via a cmd.
05/31/11     ag      Resolve KW errors. 
04/22/11     ag      Featurize freq est calls under XO feature. 
04/06/11     sm      Use latest temperature for freq estimation
02/03/10     ns      Store ADC read time in milliseconds 
11/19/10     ns      Featurize stubs for off-target under XO 
11/02/10     ns      Store Timestamp of temp read
08/20/10     sg      Fixed KW errors
08/17/10     sg      Added NULL pointer checks in adc call backs
08/12/10     sg      Added code to support a stub for off-target test env
07/14/10     sg      Added code to perform recalibration for high temp change
11/25/09     ag      Wrapping ADC DAL with FEATURE_TCXOMGR_ADCDAL
07/29/09     ns      Featurize PA therm
07/29/09     ag      Changes made during integration with tcxomgr code. 
06/03/09     adw     Initial revision.

============================================================================*/


/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "mcs_variation.h"
#include "modem_mcs_defs.h"
#include "err.h"
#include "tcxomgr_adc_client.h"
#include "timetick.h"
#include "tcxomgr_i.h"
#include "stringl.h"


/*---------------------------------------------------------------------------
  Local structure definitions
---------------------------------------------------------------------------*/

/* Blocking read request client object */
typedef struct
{
  DalDeviceHandle  *dev;
  AdcResultType  adc_result;
  DALSYSEventHandle  event_cb;
  DALSYS_EVENT_OBJECT( event_object_cb );
  DALSYSEventHandle  event_signal;
  DALSYS_EVENT_OBJECT( event_object_signal );
  DALSYSSyncHandle  sync;
  DALSYS_SYNC_OBJECT( sync_object );
  DALBOOL  request_pending;
}
tcxomgr_adc_client_blocking_read_obj_type;

/* Buffered read request client object */
typedef struct
{
  DalDeviceHandle  *dev;
  AdcResultType  adc_result;
  DALSYSEventHandle  event_cb;
  DALSYS_EVENT_OBJECT( event_object_cb );
  DALSYSSyncHandle  sync;
  DALSYS_SYNC_OBJECT( sync_object );
  DALBOOL  request_pending;
  uint32   call_back_time;
}
tcxomgr_adc_client_buffered_read_obj_type;

/* Client objects used for temp reads */
typedef struct 
{
  tcxomgr_adc_client_blocking_read_obj_type blocking_client;
  tcxomgr_adc_client_buffered_read_obj_type buffered_client;
  /* added to store the call backs and information for adc cal*/
  tcxomgr_adc_client_buffered_read_obj_type buffered_adc_cal; 
}
tcxomgr_adc_client_obj_type;

/* ADC Device object type  */
typedef struct
{
  DalDeviceHandle  *dev;
  AdcInputPropertiesType vbatt_properties[NUM_ADC_DEVICES];
}
tcxomgr_adc_device_type;


/*---------------------------------------------------------------------------
  Local variables 
---------------------------------------------------------------------------*/

/* Tcxomgr ADC client local variables */
static tcxomgr_adc_device_type tcxomgr_adc_device;
static tcxomgr_adc_client_obj_type tcxomgr_adc_client[NUM_ADC_DEVICES];


#ifdef TEST_FRAMEWORK
#error code not present
#endif // TEST_FRAMEWORK


/*============================================================================

                         LOCAL FUNCTION DECLARATIONS

============================================================================*/

/*===========================================================================

FUNCTION TCOMGR_REPORT_NEW_TEMP

DESCRIPTION
  This function is used to report XO or PMIC temperatures. Once both
  temperatures are available, cmd is sent to tcxomgr task for futher
  processing. 

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

static void tcxomgr_report_new_temp
(
  tcxomgr_adc_client_buffered_read_obj_type *obj
)
{
   tcxomgr_temp_set_type      temp = { TCXOMGR_INVALID_TEMP,
                                      TCXOMGR_INVALID_TEMP,
                                      TCXOMGR_INVALID_TEMP }; 
                                       
   tcxomgr_cmd_type * cmd_ptr; 
   
   if ( obj==NULL )
   {
     return;
   }

   if(obj->adc_result.eStatus == ADC_RESULT_INVALID)
   {
     /* We still need to send the temp update so the temperature monitor knows
      * we got a result back */
     MSG(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, "ADC conversion failed");
   }

   /* Get a command buffer */
   cmd_ptr = tcxomgr_get_cmd_buf();
   if (cmd_ptr == NULL)
   {
     ERR( "Out of TCXOMGR command buffers", 0, 0, 0);
     return;
   }

   if((uint32)obj == 
      (uint32)(&tcxomgr_adc_client[TCXO_PMIC_THERM].buffered_client))
   {
     cmd_ptr->cmd.temp_read.channel = TCXO_PMIC_THERM;
     temp.pmic = obj->adc_result.nPhysical;

     /* discard the temp if it is not in the valid range */
     if ( (obj->adc_result.eStatus == ADC_RESULT_INVALID) ||
         TCXOMGR_TEMP_RANGE_PMIC_INVALID(temp) )
     {
       MSG(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
           "Range check failed. Channel: TCXO_PMIC_THERM. Setting pmic temp to invalid");
       temp.pmic = TCXOMGR_INVALID_TEMP;
     }
   }
   else if((uint32)obj == 
      (uint32)(&tcxomgr_adc_client[TCXO_XO_THERM_ACCURATE].buffered_client))
   {
     cmd_ptr->cmd.temp_read.channel = TCXO_XO_THERM_ACCURATE;
     temp.xo = obj->adc_result.nPhysical;

     /* discard the temp if it is not in the valid range */
     if ( (obj->adc_result.eStatus == ADC_RESULT_INVALID) ||
         TCXOMGR_TEMP_RANGE_XO_INVALID(temp) )
     {
       MSG(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
           "Range check failed. Channel: TCXO_XO_THERM_ACCURATE. Setting xo temp to invalid");
       temp.xo = TCXOMGR_INVALID_TEMP;
     }
   }
   else if((uint32)obj == 
      (uint32)(&tcxomgr_adc_client[TCXO_XO_THERM_COARSE].buffered_client))
   {
     cmd_ptr->cmd.temp_read.channel = TCXO_XO_THERM_COARSE;
     temp.xo = obj->adc_result.nPhysical;
    
     /* discard the temp if it is not in the valid range */
     if ( (obj->adc_result.eStatus == ADC_RESULT_INVALID) ||
         TCXOMGR_TEMP_RANGE_XO_INVALID(temp) )
     {
       MSG(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
           "Range check failed. Channel: TCXO_XO_THERM_COARSE. Setting xo temp to invalid");
       temp.xo = TCXOMGR_INVALID_TEMP;
     }
   }
   else
   {
     /* Invalid channel, free the cmd pointer and return */
     tcxomgr_free_cmd_buf(cmd_ptr);

     return;
   }

   cmd_ptr->cmd_id                           = TCXOMGR_CMD_TEMP;
   cmd_ptr->cmd.temp_read.temp_set           = temp;
   cmd_ptr->cmd.temp_read.read_time          = obj->call_back_time;
      
   tcxomgr_send_cmd( cmd_ptr );        
}

/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_BLOCKING_READ_CB

DESCRIPTION    This function handles the conversion completion event which
               occurs after the DAL completes an ADC blocking read.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_blocking_read_cb
(
  tcxomgr_adc_client_blocking_read_obj_type *obj,  /* request object      */
  uint32 param,                                    
  void   *payload,                                 /* conatins the result */
  uint32 payload_size                              /* size of result      */
)
{
  AdcResultType *adc_result;

  /* Status buffer */
  DALResult result = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/

  adc_result = (AdcResultType*) payload;
  if(adc_result)
  {
    memscpy( &obj->adc_result, sizeof(AdcResultType), adc_result, sizeof(AdcResultType) );
  }
  obj->request_pending = FALSE;
  result = DALSYS_EventCtrl( obj->event_signal, DALSYS_EVENT_CTRL_TRIGGER );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("tcxomgr_adc_client_blocking_read_cb failed setting event %d",
              result, 0, 0);
  }


} /* tcxomgr_adc_client_blocking_read_cb */




/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_BLOCKING_READ

DESCRIPTION    This function initiates an ADC blocking read. It waits on a
               signal set in the client conversion request callback function
               and will not return until the ADC read is complete.

DEPENDENCIES   None.

RETURN VALUE   Result of ADC read.

SIDE EFFECTS   None.

===========================================================================*/
DALResult tcxomgr_adc_client_blocking_read
(
  tcxomgr_adc_client_blocking_read_obj_type *obj,  /* request object     */
  AdcInputPropertiesType *adc_input_properties     /* channel properties */
)
{
  AdcRequestParametersType adc_params;
  DALResult result = DAL_ERROR;

  /*-----------------------------------------------------------------------*/
 
  if ( obj->request_pending == FALSE )
  {
    obj->request_pending = TRUE;
    adc_params.hEvent = obj->event_cb;
    adc_params.nDeviceIdx  = adc_input_properties->nDeviceIdx;
    adc_params.nChannelIdx = adc_input_properties->nChannelIdx;
    
    result = DalAdc_RequestConversion( obj->dev, &adc_params, NULL );

    (void) DALSYS_EventWait( obj->event_signal );
 
  }
 
  return result;

} /* tcxomgr_adc_client_blocking_read */


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_BUFFERED_READ_CB

DESCRIPTION    This function handles the conversion completion event which
               occurs after the DAL completes an ADC buffered read.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_buffered_read_cb
(
  tcxomgr_adc_client_buffered_read_obj_type *obj,  /* request object */
  uint32 param,                                    /*                */
  void   *payload,                                 /* result         */
  uint32 payload_size                              /* result size    */
)
{
  AdcResultType *adc_result;

  /*-----------------------------------------------------------------------*/

  adc_result = (AdcResultType*) payload;
  /* Use payload_size to store time */
  payload_size = timetick_get_ms();

  DALSYS_SyncEnter( obj->sync );
  if(adc_result)
  {
    memscpy( &obj->adc_result, sizeof(AdcResultType), adc_result, sizeof(AdcResultType) );
  }
  obj->request_pending = FALSE;
  obj->call_back_time =  payload_size ;
  if (tcxomgr_get_crystal_type()!= TCXO_CRYSTAL ) 
  {
    /* Report the tempertaure for XO targets */
    tcxomgr_report_new_temp(obj);
  }
  DALSYS_SyncLeave( obj->sync );

} /* tcxomgr_adc_client_buffered_read_cb */


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_BUFFERED_READ

DESCRIPTION    This function initiates an ADC buffered read and will return
               immediatley. The value returned is the result which was
               initiated by a previous call to this function. The first
               call to this function will return an invalid result since
               no prior reading will have taken place.

DEPENDENCIES   None.

RETURN VALUE   Result initiated by the previous call to this function.
               An invalid result will always be returned the first time
               this function is called.

SIDE EFFECTS   None.

===========================================================================*/
DALResult tcxomgr_adc_client_buffered_read
(
  tcxomgr_adc_client_buffered_read_obj_type *obj,  /* request object     */
  AdcInputPropertiesType *adc_input_properties,    /* channel properties */
  AdcResultType *adc_result                        /* result storage     */
)
{
  AdcRequestParametersType adc_params;
  DALResult result = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/

  adc_params.hEvent = obj->event_cb;
  adc_params.nDeviceIdx  = adc_input_properties->nDeviceIdx;
  adc_params.nChannelIdx = adc_input_properties->nChannelIdx;

  memscpy( adc_result, sizeof( AdcResultType ), &obj->adc_result, sizeof( AdcResultType ) );

  if ( obj->request_pending == FALSE )
  {
    obj->request_pending = TRUE;
    result = DalAdc_RequestConversion( obj->dev, &adc_params, NULL );
  }

  return result;

} /* tcxomgr_adc_client_buffered_read */



/*---------------------------------------------------------------------------
  Initialization
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION       TCXOMGR_ADC_GET_PROPERTIES

DESCRIPTION    This function initializes a particular ADC for the client.

DEPENDENCIES   None.

RETURN VALUE   DAL_SUCCESS if the intitialization goes through sussessfully.

SIDE EFFECTS   None.

===========================================================================*/
DALResult tcxomgr_adc_get_properties 
(
  /* Which channel needs to be initialized */
  tcxomgr_adc_client_therm_type   which_therm,

  const char                     *therm_name
)
{
  DALResult result = 0;

  /*-----------------------------------------------------------------------*/
  if ( (int)which_therm >= NUM_ADC_DEVICES )
  {
     ERR_FATAL("ADC GetInputProps Failed: Incorrect therm %d",
              (int)(which_therm), 0, 0);
  }

  DalAdc_GetAdcInputProperties(tcxomgr_adc_device.dev,
                               therm_name,
                               strlen(therm_name),
                               &tcxomgr_adc_device.vbatt_properties[(int)(which_therm)]);

  return result; 

}


/*===========================================================================

FUNCTION       TCXOMGR_ADC_DEVICE_INIT

DESCRIPTION    This function initializes the Tcxomgr ADC device. It first
               reads the DAL input properties and then attches to the ADC
               DAL device driver. It initializes the driver for all PMIC,
               XO and PA therm.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_device_init( void )
{
  /* To store the result of every DAL driver function call */
  DALResult result;

  /*-----------------------------------------------------------------------*/

  DALSYS_InitMod( NULL );


  /* Attach the ADC device */
  result = DAL_AdcDeviceAttach(
            (DALDEVICEID) DALDEVICEID_ADC,
            &tcxomgr_adc_device.dev );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("ADC Device Attach Failed with result %d ",result, 0, 0);
  }

  if (tcxomgr_get_crystal_type()!= TCXO_CRYSTAL ) 
  {
    /* Get the PMIC channel information */
    result = tcxomgr_adc_get_properties(TCXO_PMIC_THERM, ADC_INPUT_PMIC_THERM);
        
    if ( result != DAL_SUCCESS )
    {
      ERR_FATAL("ADC GetInputProps Failed with result %d for therm %d",
                result, (int)(TCXO_PMIC_THERM), 0);
    }

    /* Get the XO GPS channel information */
    result = tcxomgr_adc_get_properties(TCXO_XO_THERM_ACCURATE,
        ADC_INPUT_XO_THERM_GPS);
    if ( result != DAL_SUCCESS )
    {
      ERR_FATAL("ADC GetInputProps Failed with result %d for therm %d",
                result, (int)(TCXO_XO_THERM_ACCURATE), 0);
    }

    /* Get the XO Coarse channel information */
    result = tcxomgr_adc_get_properties(TCXO_XO_THERM_COARSE,
        ADC_INPUT_XO_THERM);
    if ( result != DAL_SUCCESS )
    {
      ERR_FATAL("ADC GetInputProps Failed with result %d for therm %d",
                result, (int)(TCXO_XO_THERM_COARSE), 0);
    }

    /* There is no vctcxo therm, so set it to max */
    tcxomgr.vctcxo_therm = TCXO_MAX_THERM;
  }
  else
  {
    /* Operate in VCTXCO mode */

    /* intialize the vctcxo therm to MAX THERM */
    tcxomgr.vctcxo_therm = TCXO_MAX_THERM;

    /* use the Therm present on the order of preference */
    if ( tcxomgr_adc_get_properties(TCXO_XO_THERM_COARSE, ADC_INPUT_XO_THERM)
         == DAL_SUCCESS )
    {
      tcxomgr.vctcxo_therm = TCXO_XO_THERM_COARSE;
    }
    else if ( tcxomgr_adc_get_properties(TCXO_PA_THERM, ADC_INPUT_PA_THERM)
              == DAL_SUCCESS ) 
    {
      tcxomgr.vctcxo_therm = TCXO_PA_THERM;
    }
    else if ( tcxomgr_adc_get_properties(TCXO_MSM_THERM, ADC_INPUT_MSM_THERM)
              == DAL_SUCCESS)
    {
      tcxomgr.vctcxo_therm = TCXO_MSM_THERM;
    }
  }
}


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_BLOCKING_READ_INIT

DESCRIPTION    This function initializes the client read request object
               for blocking reads.

DEPENDENCIES   None.

RETURN VALUE   Initialization success or failure.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_blocking_read_init
(
  tcxomgr_adc_client_blocking_read_obj_type *obj   /* */
)
{
  DALResult result = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/

  DALSYS_memset( obj, 0, sizeof(tcxomgr_adc_client_blocking_read_obj_type) );

  obj->dev = tcxomgr_adc_device.dev;

  result = DALSYS_EventCreate(
              DALSYS_EVENT_ATTR_CLIENT_DEFAULT |
              DALSYS_EVENT_ATTR_CALLBACK_EVENT,
              &obj->event_cb,
              &obj->event_object_cb );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_EventCreate failed in blocking read init - %d",
              (int)result, 0, 0);
  }

  result = DALSYS_SetupCallbackEvent(
              obj->event_cb,
              (DALSYSCallbackFunc) tcxomgr_adc_client_blocking_read_cb,
              obj );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_SetupCallbackEvent failed in blocking read init - %d",
              (int)result, 0, 0);
  }

  result = DALSYS_EventCreate(
              DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
              &obj->event_signal,
              &obj->event_object_signal );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_EventCreate failed for signal in blocking read init-%d",
              (int)result, 0, 0);
  }

  result = DALSYS_SyncCreate(
              DALSYS_SYNC_ATTR_RESOURCE,
              &obj->sync,
              &obj->sync_object);

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_SyncCreate failed for in blocking read init-%d",
              (int)result, 0, 0);
  }

} /* tcxomgr_adc_client_blocking_read_init */

/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_RECALIBRATE_CB

DESCRIPTION    This function handles the recalibration completion event which
               occurs after the DAL completes an ADC recalibration.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_recalibrate_cb
(
  tcxomgr_adc_client_buffered_read_obj_type *obj,  /* request object */
  uint32 param,                                    /*                */
  void   *payload,                                 /* result         */
  uint32 payload_size                              /* result size    */
)
{
  AdcResultType *adc_result;

  /*-----------------------------------------------------------------------*/
  
  adc_result = (AdcResultType*) payload;

  obj->request_pending = FALSE;
  
  /* adc_result could be NULL for recalibration event 
     This check is to make sure that NULL pointer is not de-referenced */
  if(adc_result && (ADC_RESULT_VALID == adc_result->eStatus))
  {
    MSG(MSG_SSID_TCXOMGR, MSG_LEGACY_MED, "ADC recalibration Successful");
  }

} /* tcxomgr_adc_client_recalibrate_cb */


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_RECALIBRATE

DESCRIPTION    This function re-calibrates ADC.

DEPENDENCIES   None.

RETURN VALUE   Result of ADC re-calibration.

SIDE EFFECTS   None.

===========================================================================*/
DALResult tcxomgr_adc_client_recalibrate
(
  tcxomgr_adc_client_buffered_read_obj_type *obj,  /* request object     */
  AdcInputPropertiesType *adc_input_properties     /* channel properties */
)
{
  AdcRequestParametersType adc_params;
  DALResult result = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/

  adc_params.hEvent = obj->event_cb;
  adc_params.nDeviceIdx  = adc_input_properties->nDeviceIdx;
  adc_params.nChannelIdx = adc_input_properties->nChannelIdx;

  if ( obj->request_pending == FALSE )
  {
    obj->request_pending = TRUE;
    result = DalAdc_RequestRecalibration( obj->dev, &adc_params );
  }

  return result;

} /* tcxomgr_adc_client_blocking_read */



/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_RECALIBRATION_INIT

DESCRIPTION    This function initializes the client recalibration object
               for ADC recalibration request.

DEPENDENCIES   None.

RETURN VALUE   Initialization success or failure.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_recalibration_init
(
  tcxomgr_adc_client_buffered_read_obj_type *obj  
)
{
  DALResult result = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/

  DALSYS_memset( obj, 0, sizeof(tcxomgr_adc_client_buffered_read_obj_type) );

  obj->dev = tcxomgr_adc_device.dev;

  result = DALSYS_EventCreate(
              DALSYS_EVENT_ATTR_CLIENT_DEFAULT |
              DALSYS_EVENT_ATTR_CALLBACK_EVENT,
              &obj->event_cb,
              &obj->event_object_cb );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_EventCreate failed in buffered read init - %d",
              (int)result, 0, 0);
  }

  result = DALSYS_SetupCallbackEvent(
              obj->event_cb,
              (DALSYSCallbackFunc) tcxomgr_adc_client_recalibrate_cb,
              obj );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_SetupCallbackEvent failed in buffered read init - %d",
              (int)result, 0, 0);
  }

  result = DALSYS_SyncCreate(
              DALSYS_SYNC_ATTR_RESOURCE,
              &obj->sync,
              &obj->sync_object );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_SyncCreate failed in buffered read init - %d",
              (int)result, 0, 0);
  }

} /* tcxomgr_adc_client_recalibration_init */


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_BUFFERED_READ_INIT

DESCRIPTION    This function initializes the client read request object
               for buffered reads.

DEPENDENCIES   None.

RETURN VALUE   Initialization success or failure.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_buffered_read_init
(
  tcxomgr_adc_client_buffered_read_obj_type *obj   /* */
)
{
  DALResult result = DAL_SUCCESS;

  /*-----------------------------------------------------------------------*/

  DALSYS_memset( obj, 0, sizeof(tcxomgr_adc_client_buffered_read_obj_type) );

  obj->dev = tcxomgr_adc_device.dev;

  result = DALSYS_EventCreate(
              DALSYS_EVENT_ATTR_CLIENT_DEFAULT |
              DALSYS_EVENT_ATTR_CALLBACK_EVENT,
              &obj->event_cb,
              &obj->event_object_cb );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_EventCreate failed in buffered read init - %d",
              (int)result, 0, 0);
  }

  result = DALSYS_SetupCallbackEvent(
              obj->event_cb,
              (DALSYSCallbackFunc) tcxomgr_adc_client_buffered_read_cb,
              obj );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_SetupCallbackEvent failed in buffered read init - %d",
              (int)result, 0, 0);
  }

  result = DALSYS_SyncCreate(
              DALSYS_SYNC_ATTR_RESOURCE,
              &obj->sync,
              &obj->sync_object );

  if ( result != DAL_SUCCESS )
  {
    ERR_FATAL("DALSYS_SyncCreate failed in buffered read init - %d",
              (int)result, 0, 0);
  }

} /* tcxomgr_adc_client_buffered_read_init */


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_INIT

DESCRIPTION    This function initializes the Tcxomgr ADC client objects. 

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_init( void )
{

  /* Initilaize the ADC device */
  tcxomgr_adc_device_init();
  
  /* Initialize blocking read objects */

  if (tcxomgr_get_crystal_type()!= TCXO_CRYSTAL )
  {
    tcxomgr_adc_client_blocking_read_init
             (&tcxomgr_adc_client[(int)TCXO_PMIC_THERM].blocking_client);
    tcxomgr_adc_client_blocking_read_init
             (&tcxomgr_adc_client[(int)TCXO_XO_THERM_ACCURATE].blocking_client);
    tcxomgr_adc_client_blocking_read_init
             (&tcxomgr_adc_client[(int)TCXO_XO_THERM_COARSE].blocking_client);
  }
  else
  {
    tcxomgr_adc_client_blocking_read_init
             (&tcxomgr_adc_client[(int)TCXO_XO_THERM_COARSE].blocking_client);
    tcxomgr_adc_client_blocking_read_init
             (&tcxomgr_adc_client[(int)TCXO_PA_THERM].blocking_client);
    tcxomgr_adc_client_blocking_read_init
             (&tcxomgr_adc_client[(int)TCXO_MSM_THERM].blocking_client);
  }

  /* Initialize buffered read objects */
  if (tcxomgr_get_crystal_type()!= TCXO_CRYSTAL )
  {
    tcxomgr_adc_client_buffered_read_init
      (&tcxomgr_adc_client[(int)TCXO_PMIC_THERM].buffered_client);
    tcxomgr_adc_client_recalibration_init
      (&tcxomgr_adc_client[(int)TCXO_PMIC_THERM].buffered_adc_cal);
    tcxomgr_adc_client_buffered_read_init
      (&tcxomgr_adc_client[(int)TCXO_XO_THERM_ACCURATE].buffered_client);
    tcxomgr_adc_client_recalibration_init
      (&tcxomgr_adc_client[(int)TCXO_XO_THERM_ACCURATE].buffered_adc_cal);
    tcxomgr_adc_client_buffered_read_init
      (&tcxomgr_adc_client[(int)TCXO_XO_THERM_COARSE].buffered_client);
    tcxomgr_adc_client_recalibration_init
      (&tcxomgr_adc_client[(int)TCXO_XO_THERM_COARSE].buffered_adc_cal);
  }
  else
  {
    tcxomgr_adc_client_buffered_read_init
      (&tcxomgr_adc_client[(int)TCXO_XO_THERM_COARSE].buffered_client);
    tcxomgr_adc_client_buffered_read_init
      (&tcxomgr_adc_client[(int)TCXO_PA_THERM].buffered_client);
    tcxomgr_adc_client_buffered_read_init
      (&tcxomgr_adc_client[(int)TCXO_MSM_THERM].buffered_client);
  }

} /* tcxomgr_adc_client_init */


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_TERM

DESCRIPTION    This function releases all resources acquired while
               initializing and operating the ADC client.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_term(void)
{
  DALResult result;

  if(tcxomgr_adc_device.dev)
  {
    result = DAL_DeviceDetach(tcxomgr_adc_device.dev);

    if(result != DAL_SUCCESS)
    {
      MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
          "ADC DAL_DeviceDetach() failed: %d", result);
    }
    tcxomgr_adc_device.dev = NULL;
  }
}


/*---------------------------------------------------------------------------
  ADC Read
---------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_ADC_READ

DESCRIPTION    This function initiates an ADC read.

DEPENDENCIES   The tcxomgr_adc_client_init() must have been called previously.

RETURN VALUE   Result of ADC read.

SIDE EFFECTS   None.

===========================================================================*/
tcxomgr_adc_temp_type tcxomgr_adc_client_adc_read
(
  /* Which ADC do you need to read? */
  tcxomgr_adc_client_therm_type   which_therm, 
  
  /* What is the type of read - buffered, blocking? */
  tcxomgr_adc_client_read_type    read_type,
  
  /* Return read time, only for buffered read */
  uint32 *read_time        
)
{
  /* Init  return value to invalid value */
  tcxomgr_adc_temp_type adc_reading = TCXOMGR_ADC_INVALID_TEMP;

  /* Copy buffered result */
  AdcResultType buffered_result;
  
  /* Status buffer */
  DALResult result;


  /* Get index */
  int index = (int)which_therm;

  /*-----------------------------------------------------------------------*/
  if(index >= TCXO_MAX_THERM || index >= NUM_ADC_DEVICES)
  {
    MSG_1( MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR, 
           "ADC Buffered Read failed. Unknown therm: %d",index);
    return adc_reading;
  }

  switch ( read_type )
  {
    case BUFFERED_READ:
      {
  
        DALSYS_SyncEnter( tcxomgr_adc_client[index].buffered_client.sync );

        /* Do a buffered read */
        result = tcxomgr_adc_client_buffered_read( 
                       &tcxomgr_adc_client[index].buffered_client, 
                       &tcxomgr_adc_device.vbatt_properties[index],
                       &buffered_result );

        /* Is the read successful ? */
        if ( result != DAL_SUCCESS )
        {
          ERR_FATAL("ADC Buffered Read returned with result %d for therm %d",
                     result, index, 0);
        }

        /* Is the result invalid */
        if ( buffered_result.eStatus == ADC_RESULT_INVALID )
        {
          MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
                "ADC Buffered Read returned status INVALID for therm %d", index);
        }
        else
        {
          /* Read is successful and result is valid so we got an adc value */
          adc_reading = buffered_result.nPhysical;
          if (read_time) 
          {
            /* return callback time in ms */
            *read_time
               = tcxomgr_adc_client[index].buffered_client.call_back_time;
          }
        }

      DALSYS_SyncLeave( tcxomgr_adc_client[index].buffered_client.sync );

      break;
      }
    case BLOCKING_READ:
      {
        
        DALSYS_SyncEnter( tcxomgr_adc_client[index].blocking_client.sync );
        result = DALSYS_EventCtrl( 
                 tcxomgr_adc_client[index].blocking_client.event_signal, 
                 DALSYS_EVENT_CTRL_RESET );

        if ( result != DAL_SUCCESS )
        {
          ERR_FATAL("tcxomgr_adc_client_adc_read failed setting event %d",
                    result, 0, 0);
        }

        result = tcxomgr_adc_client_blocking_read(
                       &tcxomgr_adc_client[index].blocking_client,
                       &tcxomgr_adc_device.vbatt_properties[index] );

        /* Is the read successful ? */
        if ( result != DAL_SUCCESS )
        {
          ERR_FATAL("ADC Blocking Read returned with result %d for therm %d",
                     result, index, 0);
        }

        /* Is the result invalid ? */
        if ( tcxomgr_adc_client[index].blocking_client.adc_result.eStatus
              == ADC_RESULT_INVALID )
        {
          ERR_FATAL("ADC Blocking Read returned status INVALID for therm %d",
                     index, 0, 0);
        }
        else
        {  
          /* Read is successful and result is valid so we got an adc value */
          adc_reading = 
              tcxomgr_adc_client[index].blocking_client.adc_result.nPhysical;

        }
        
        DALSYS_SyncLeave( tcxomgr_adc_client[index].blocking_client.sync );       
        break;
      }
    
    default:
      ERR_FATAL("Invalid argument read_type %d", (int) read_type, 0, 0);
  }

  return adc_reading;
    
} /* tcxomgr_adc_client_adc_read */


/*===========================================================================

FUNCTION       TCXOMGR_ADC_CLIENT_ADC_RECALIBRATE

DESCRIPTION    This function initiates ADC recalibration

DEPENDENCIES   The tcxomgr_adc_client_init() must have been called previously.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_adc_client_adc_recalibrate
(
  /* Which ADC do you need to read? */
  tcxomgr_adc_client_therm_type   which_therm      
)
{
  /* Status buffer */
  DALResult result;

  /* Get index */
  int index = (int)which_therm;

  /*-----------------------------------------------------------------------*/
  if(index >= TCXO_MAX_THERM)
  {
    MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
          "ADC Recalibration failied. Unknown therm: %d", index);
  }
  else
  {
  
    DALSYS_SyncEnter( tcxomgr_adc_client[index].buffered_client.sync );

    result = tcxomgr_adc_client_recalibrate( 
               &tcxomgr_adc_client[index].buffered_adc_cal, 
               &tcxomgr_adc_device.vbatt_properties[index] );
  
    /* Is the recalibration query successful ? */
    if(result != DAL_SUCCESS)
    {
      MSG_1(MSG_SSID_TCXOMGR, MSG_LEGACY_ERROR,
            "ADC Recalibration failied. Returned: %d", result);
    }
      
    DALSYS_SyncLeave( tcxomgr_adc_client[index].buffered_client.sync );
  }

} /* tcxomgr_adc_client_adc_read */

