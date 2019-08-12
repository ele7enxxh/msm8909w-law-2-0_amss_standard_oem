/*==================================================================================================

FILE: tal_tlmm.c

DESCRIPTION: This module defines the "stock" implementation of the TLMM APIs for the
             Target Abstraction Layer.  This implementation simply forwards requests
             to the DAL TLMM.

                            Copyright (c) 2012-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================

$Header: //components/rel/core.mpss/3.5.c12.3/wiredconnectivity/uart/tal/src/tal_tlmm.c#1 $

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   tal_tlmm_close
   tal_tlmm_get_rx_gpio_num
   tal_tlmm_gpio_disable
   tal_tlmm_gpio_enable
   tal_tlmm_open

==================================================================================================*/
/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

#if ( defined(_WIN32) && !defined(_WIN32_WCE) )
#include "dal.h"
#else
#include "comdef.h"
#include "DALDeviceId.h"
#include "DALSys.h"
#include "DDITlmm.h"
#endif

#include "tal.h"

/*==================================================================================================
                                               MACROS
==================================================================================================*/

#define TAL_GPIO_NUMBER(config)    (((config) & 0x003FF0) >>  4)

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/

typedef enum
{
   TX,
   RX,
   CTS,
   RFR,
   NUM_GPIOS
}GPIOS;

typedef struct tal_tlmm_context
{
   DALDEVICEID       client_id;
   DalDeviceHandle  *tlmm_dal;
   uint32            gpios[NUM_GPIOS];
   uint32            num_gpios;
} TAL_TLMM_CONTEXT;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static boolean get_gpio_config        (TAL_TLMM_CONTEXT *handle);
static void    get_property_dword     (DALDEVICEID id, DALSYSPropertyHandle handle,
                                       const char *property_name, uint32 *property_value,
                                       uint32 default_value);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: get_gpio_config

DESCRIPTION:
   
==================================================================================================*/
static boolean get_gpio_config(TAL_TLMM_CONTEXT *tlmm_ctxt)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(handle);
   DALResult        result;
   DALDEVICEID      id = tlmm_ctxt->client_id;
   uint32          *gpios = tlmm_ctxt->gpios;

   tlmm_ctxt->num_gpios = 0;

   result = DALSYS_GetDALPropertyHandle(id, handle);
   if (result != DAL_SUCCESS) { goto error; }
   
   get_property_dword(id, handle, "GpioTxData",  &gpios[ TX], 0);
   get_property_dword(id, handle, "GpioRxData",  &gpios[ RX], 0);
   get_property_dword(id, handle, "GpioCtsN",    &gpios[CTS], 0);
   get_property_dword(id, handle, "GpioRfrN",    &gpios[RFR], 0);

   if (!gpios[ TX])
   {
      DALSYS_LogEvent(id, 
                         DALSYS_LOGEVENT_ERROR, 
                         "get_gpio_config: Invalid TX GPIO configuration",
                         gpios[TX]);
         goto error;
   }
   
   if (!gpios[ RX])
   {
      DALSYS_LogEvent(id, 
                         DALSYS_LOGEVENT_ERROR, 
                         "get_gpio_config: Invalid RX GPIO configuration",
                         gpios[RX]);
         goto error;
   }
   tlmm_ctxt->num_gpios = 2;
   
   if (gpios[CTS]) {(tlmm_ctxt->num_gpios)++;}
   if (gpios[RFR]) {(tlmm_ctxt->num_gpios)++;}

   return TRUE;
error:
   DALSYS_LogEvent(id, DALSYS_LOGEVENT_ERROR, "get_gpio_config: failed");
   return FALSE;
}

/*==================================================================================================

FUNCTION: get_property_dword

DESCRIPTION:
   Retrieve a dword (32-bit integer) property.

==================================================================================================*/
static void get_property_dword(DALDEVICEID id, DALSYSPropertyHandle handle,
                               const char *property_name, uint32 *property_value,
                               uint32 default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.dwVal;
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "get_property_dword: %s = 0x%08X",
                                                property_name, *property_value);
   }
   else
   {
      *property_value = default_value;
      DALSYS_LogEvent(id, DALSYS_LOGEVENT_INFO, "get_property_dword: %s = 0x%08X (defaulted)",
                                                property_name, *property_value);
   }
}

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: tal_tlmm_close

DESCRIPTION:

==================================================================================================*/
void tal_tlmm_close(TAL_TLMM_HANDLE handle)
{
   DalDevice_Close(handle->tlmm_dal);
   DAL_DeviceDetach(handle->tlmm_dal);
   DALSYS_Free(handle);
}

/*==================================================================================================

FUNCTION: tal_tlmm_get_rx_gpio_num

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_tlmm_get_rx_gpio_num(TAL_TLMM_HANDLE handle, uint32 *gpio_num)
{
   if (!handle->gpios[RX])
   {
      return TAL_ERROR;
   }

   *gpio_num = TAL_GPIO_NUMBER(handle->gpios[RX]);
   
   return TAL_SUCCESS;
}

/*==================================================================================================

FUNCTION: tal_tlmm_gpio_disable

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_tlmm_gpio_disable(TAL_TLMM_HANDLE handle)
{
   DALResult result = DAL_SUCCESS;

   if (handle->num_gpios)
   {
      result = DalTlmm_ConfigGpioGroup(handle->tlmm_dal, DAL_TLMM_GPIO_DISABLE,
                                       handle->gpios, handle->num_gpios);
   }

   return (result == DAL_SUCCESS) ? (TAL_SUCCESS) : (TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_tlmm_gpio_enable

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_tlmm_gpio_enable(TAL_TLMM_HANDLE handle)
{
   DALResult result = DAL_SUCCESS;

   if (handle->num_gpios)
   {
      result = DalTlmm_ConfigGpioGroup(handle->tlmm_dal, DAL_TLMM_GPIO_ENABLE,
                                       handle->gpios, handle->num_gpios);
   }

   return (result == DAL_SUCCESS) ? (TAL_SUCCESS) : (TAL_ERROR);
}

/*==================================================================================================

FUNCTION: tal_tlmm_open

DESCRIPTION:

==================================================================================================*/
TAL_RESULT tal_tlmm_open(TAL_TLMM_HANDLE *phandle, uint32 client_id)
{
   TAL_TLMM_CONTEXT *tlmm_ctxt = NULL;
   DalDeviceHandle  *tlmm_dal = NULL;
   DALResult         result;

   result = DALSYS_Malloc(sizeof(TAL_TLMM_CONTEXT), (void **)&tlmm_ctxt);
   if (result != DAL_SUCCESS) { goto error; }

   result = DAL_DeviceAttach(DALDEVICEID_TLMM, &tlmm_dal);
   if (result != DAL_SUCCESS) { goto error; }

   result = DalDevice_Open(tlmm_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS) { goto error; }

   tlmm_ctxt->client_id = client_id;
   tlmm_ctxt->tlmm_dal = tlmm_dal;

   if(!get_gpio_config(tlmm_ctxt)) { goto error;}

   *phandle = tlmm_ctxt;
   return TAL_SUCCESS;
error:
   if (tlmm_ctxt) { DALSYS_Free(tlmm_ctxt); }
   if (tlmm_dal)  { DAL_DeviceDetach(tlmm_dal); }
   *phandle = NULL;
   return TAL_ERROR;
}
