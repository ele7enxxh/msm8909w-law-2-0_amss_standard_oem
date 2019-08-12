/*==================================================================================================

FILE: Uart_os.c

DESCRIPTION: This module provides the os based functionalities for the UART.

                           Copyright (c) 2009-2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   Uart_busy_wait
   Uart_clock_open
   Uart_clock_close
   Uart_get_driver_properties
   Uart_get_properties
   Uart_interrupt_close
   Uart_interrupt_done
   Uart_interrupt_open
   Uart_tlmm_open
   Uart_tlmm_close
   Uart_busy_wait

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/uart_lite/src/Uart_dal_os.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   VV      Initial revision.

==================================================================================================*/

/*-------------------------------------------------------------------------
* Include Files
* ----------------------------------------------------------------------*/
#include "DALSys.h"
#include "Uart_os.h"
#include "Uart.h"
#include "Uart_defs.h"
#include "Uart_log.h"
#include "DDITlmm.h"
#include "DDIClock.h"
#include "DDIHWIO.h"
#include "DDIInterruptController.h"


extern UART_DRIVER_PROPERTIES uart_driver_props;
#define UART_LOGGING_FILE_ID 20

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/
typedef struct
{
   DalDeviceHandle* dal_interrupt_handle;
   uint32  irq_num;
   uint32* uart_tcsr_addr;
   uint32  tcsr_mask;
}UART_INTERRUPT_CONTEXT;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static DalDeviceHandle *dal_hwio = NULL;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static UartResult configure_tcsr(UART_CONTEXT* h, boolean configure);
static void       get_dword_property(DALSYSPropertyHandle handle,const char* property_name,
                                    uint32* property_value, uint32 default_value);
static void       get_string_property(DALSYSPropertyHandle handle,const char* property_name,
                                    char** property_value, char* default_value);


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: configure_tcsr

DESCRIPTION:

==================================================================================================*/
static UartResult configure_tcsr(UART_CONTEXT* h, boolean configure)
{
   uint32           uart_tcsr_value;
   volatile uint32* uart_tcsr_addr = 0;
   UART_INTERRUPT_CONTEXT* uart_interrupt_context = (UART_INTERRUPT_CONTEXT*)h->interrupt_context;

   uart_tcsr_addr = uart_interrupt_context->uart_tcsr_addr;
   uart_tcsr_value = *uart_tcsr_addr;

   if (configure)
   {
      uart_tcsr_value |= uart_interrupt_context->tcsr_mask;
   }
   else
   {
      uart_tcsr_value &= ~uart_interrupt_context->tcsr_mask;
   }

   *uart_tcsr_addr = uart_tcsr_value;

   return UART_SUCCESS;

}
/*==================================================================================================

FUNCTION: get_dword_property

DESCRIPTION:

==================================================================================================*/
static void get_dword_property(DALSYSPropertyHandle handle,const char* property_name,
                                    uint32* property_value, uint32 default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.dwVal;
   }
   else
   {
      *property_value = default_value;
   }

}

/*==================================================================================================

FUNCTION: get_string_property

DESCRIPTION:

==================================================================================================*/
static void get_string_property(DALSYSPropertyHandle handle,const char* property_name,
                                    char** property_value, char* default_value)
{
   DALSYSPropertyVar property_variable;
   DALResult result;

   result = DALSYS_GetPropertyValue(handle, property_name, 0, &property_variable);

   if (result == DAL_SUCCESS)
   {
      *property_value = property_variable.Val.pszVal;
   }
   else
   {
      *property_value = default_value;
   }

}
/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================

FUNCTION: Uart_busy_wait

DESCRIPTION:

==================================================================================================*/
void Uart_busy_wait(uint32 usecs)
{
   DALSYSEventHandle timer_event = NULL;
   uint32 triggered_event;
   DALResult result;

   result = DALSYS_EventCreate(DALSYS_EVENT_ATTR_TIMEOUT_EVENT | DALSYS_EVENT_ATTR_NON_DEFERRED,
                               &timer_event, NULL);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR,"Unable to create timer event");
      goto exit;
   }

   result = DALSYS_EventMultipleWait(&timer_event, 1, usecs , &triggered_event);
   if (result != DAL_ERROR_TIMEOUT)
   {
      UART_LOG_0(ERROR,"DALSYS_EventMultipleWait failed.");
      goto exit;
   }

exit:
   if (timer_event) { DALSYS_DestroyObject(timer_event); }
}

/*==================================================================================================

FUNCTION: Uart_clock_close

DESCRIPTION:

==================================================================================================*/
UartResult Uart_clock_close(UART_CONTEXT* h)
{
   DALResult          dal_result;
   DalDeviceHandle*   dal_clock;
   UartResult         result = UART_ERROR;


   UART_LOG_0(INFO,"+Uart_clock_close");

   dal_clock = (DalDeviceHandle*)h->clock_handle;

   // Disable the Uart core clock.
   dal_result = DalClock_DisableClock(dal_clock, (ClockIdType)h->properties.core_clock_id);
   if (dal_result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "DalClock_DisableClock for uart core clock failed.");
      result = UART_ERROR;
   }

   // Disable the AHB clock.
   dal_result = DalClock_DisableClock(dal_clock, (ClockIdType)h->properties.bus_clock_id);
   if (dal_result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "DalClock_DisableClock for AHB clock failed.");
      result = UART_ERROR;
   }

   DAL_DeviceDetach(dal_clock);

   UART_LOG_0(INFO,"-Uart_clock_close");
   return result;
}

/*==================================================================================================

FUNCTION: Uart_clock_open

DESCRIPTION:

==================================================================================================*/
UartResult Uart_clock_open(UART_CONTEXT* h, uint32 input_freq)
{
   DalDeviceHandle *dal_clock = NULL;
   DALResult       result;
   uint32          result_freq = 0;
   char*           clock_name = NULL;

   DALSYS_PROPERTY_HANDLE_DECLARE(hProp_device);

   UART_LOG_0(INFO, "+Uart_clock_open");

   if (DAL_DeviceAttach(DALDEVICEID_CLOCK, &dal_clock) != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR,"DALDEVICEID_CLOCK Attach failed");
      return UART_ERROR;
   }

   h->clock_handle = (void*)dal_clock;

   if( DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(h->port_id, hProp_device))
   {
      UART_LOG_0(ERROR,"DALSYS_GetDALPropertyHandleStr for device failed");
      return UART_ERROR;
   }

   // Read the name of the Uart core clock
   get_string_property(hProp_device, "UartClockName", &clock_name, NULL);
   if (clock_name == NULL)
   {
      UART_LOG_0(ERROR, "UartClockname property not found.");
      return UART_ERROR;
   }

   // Look up the clock ID for the core clock
   result = DalClock_GetClockId(dal_clock, clock_name, (ClockIdType*)&h->properties.core_clock_id);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "Core GetClockId failed.");
      return UART_ERROR;
   }

   // Read the name of the AHB clock
   get_string_property(hProp_device, "PClockName", &clock_name, NULL);
   if (result != DAL_SUCCESS || clock_name == NULL)
   {
      UART_LOG_0(ERROR, "PClockname property not found.");
      return UART_ERROR;
   }

   // Look up the clock ID for the AHB clock
   result = DalClock_GetClockId(dal_clock, clock_name, (ClockIdType*)&h->properties.bus_clock_id);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "AHB GetClockId failed.");
      return UART_ERROR;
   }

   // Set the Frequency for the UART core clock.
   result = DalClock_SetClockFrequency( dal_clock,
                                        (ClockIdType)h->properties.core_clock_id,
                                        input_freq,
                                        CLOCK_FREQUENCY_HZ_EXACT,
                                        &result_freq);

   if (result != DAL_SUCCESS || result_freq != input_freq)
   {
      UART_LOG_0(ERROR, "DalClock_SetClockFrequency failed");
      return UART_ERROR;
   }


   // Enable both the UART and AHB clocks
   result = DalClock_EnableClock(dal_clock, (ClockIdType)h->properties.core_clock_id);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "Core clock - DalClock_EnableClock failed");
      return UART_ERROR;
   }

   result = DalClock_EnableClock(dal_clock, (ClockIdType)h->properties.bus_clock_id);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "Bus clock - DalClock_EnableClock failed");
      return UART_ERROR;
   }

   UART_LOG_0(INFO, "-Uart_clock_open");
   return UART_SUCCESS;

}

/*==================================================================================================

FUNCTION: Uart_get_driver_propertiess

DESCRIPTION:

==================================================================================================*/
UartResult Uart_get_driver_properties( void )
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp_driver);

   UART_LOG_0(INFO, "+Uart_get_driver_properties");

   if( DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr("/core/buses/uart",hProp_driver))
   {
      UART_LOG_0(ERROR,"DALSYS_GetDALPropertyHandleStr for driver failed");
      return UART_ERROR;
   }
   get_string_property(hProp_driver, "UartMainPortPhy",
                                    &uart_driver_props.device_names[UART_MAIN_PORT],NULL);

   UART_LOG_0(INFO, "-Uart_get_driver_properties");
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_get_properties

DESCRIPTION:

==================================================================================================*/
UartResult Uart_get_properties(UART_CONTEXT* h)
{
   uint32 uart_base;
   static DALResult dal_result = DAL_ERROR;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp_device);

   UART_LOG_0(INFO, "+Uart_get_properties");

   if( DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(h->port_id,hProp_device))
   {
      UART_LOG_0(ERROR,"DALSYS_GetDALPropertyHandleStr for device failed");
      return UART_ERROR;
   }

   get_dword_property (hProp_device, "IsLoopback",       &h->properties.is_loopback,0);
   get_dword_property (hProp_device, "BitRate",          &h->properties.bit_rate,0);
   get_dword_property (hProp_device, "UartBase",         &uart_base,0);
   get_dword_property (hProp_device, "GpioTxData",       &h->properties.gpio_tx_config,0);
   get_dword_property (hProp_device, "GpioRxData",       &h->properties.gpio_rx_config,0);
   get_dword_property (hProp_device, "GpioRfrN",         &h->properties.gpio_rfr_config,0);
   get_dword_property (hProp_device, "GpioCtsN",         &h->properties.gpio_cts_config,0);
   get_dword_property (hProp_device, "ClockIdIndex",     &h->properties.clock_id_index,0);

   // Attach to the HWIO device and get the virtual address of the UART base address.
   if(dal_result == DAL_ERROR)
   {
	   if(DAL_HWIODeviceAttach(DALDEVICEID_HWIO, &dal_hwio) == DAL_SUCCESS)
	   {
		  if(DalDevice_Open(dal_hwio, DAL_OPEN_SHARED) == DAL_SUCCESS)
		  {
			 dal_result = DAL_SUCCESS;
		  }
		  else
		  {
			 DAL_DeviceDetach(dal_hwio);
			 dal_hwio = NULL;
		  }
	    }
	}
	if(dal_result == DAL_SUCCESS)
	{
		if(DalHWIO_MapRegionByAddress(dal_hwio,
										   (uint8*)uart_base,
										   (uint8**)&h->properties.uart_base) == DAL_SUCCESS)
		{
			UART_LOG_0(INFO, "-Uart_get_properties.");
			return UART_SUCCESS;
		}
	}

   UART_LOG_0(INFO, "-Uart_get_properties failed.");
   return UART_ERROR;
}
/*==================================================================================================

FUNCTION: Uart_interrupt_close

DESCRIPTION:

==================================================================================================*/
UartResult Uart_interrupt_close(UART_CONTEXT* h)
{
   UART_INTERRUPT_CONTEXT* uart_interrupt_context;
   DALResult result = DAL_ERROR;

   UART_LOG_0(INFO, "+Uart_interrupt_close");
   uart_interrupt_context = (UART_INTERRUPT_CONTEXT*)h->interrupt_context;

   // De-register the interrupt.
   result = DalInterruptController_Unregister(uart_interrupt_context->dal_interrupt_handle,
                                                    (DALInterruptID)uart_interrupt_context->irq_num);

   // Close the DAL interrupt device.
   DalDevice_Close(uart_interrupt_context->dal_interrupt_handle);
   DAL_DeviceDetach(uart_interrupt_context->dal_interrupt_handle);

   UART_LOG_0(INFO, "-Uart_interrupt_close");
   return (result == DAL_SUCCESS) ? (UART_SUCCESS) : (UART_ERROR);
}

/*==================================================================================================

FUNCTION: Uart_interrupt_done

DESCRIPTION:

==================================================================================================*/
UartResult Uart_interrupt_done(UART_CONTEXT* h)
{
   DALResult result = DAL_ERROR;
   UART_INTERRUPT_CONTEXT* uart_interrupt_context;

   UART_LOG_0(INFO, "+Uart_interrupt_done");

   uart_interrupt_context = (UART_INTERRUPT_CONTEXT*)h->interrupt_context;
   result = DalInterruptController_InterruptDone(uart_interrupt_context->dal_interrupt_handle,
                                                       (DALInterruptID)uart_interrupt_context->irq_num);

   UART_LOG_0(INFO, "-Uart_interrupt_done");
   return (result == DAL_SUCCESS) ? (UART_SUCCESS) : (UART_ERROR);
}

/*==================================================================================================

FUNCTION: Uart_interrupt_open

DESCRIPTION:

==================================================================================================*/
UartResult Uart_interrupt_open(UART_CONTEXT* h, void* isr)
{
   uint8* virtual_tcsr_base;
   uint32 tcsr_offset;
   UART_INTERRUPT_CONTEXT* uart_interrupt_context;
   DalDeviceHandle* interrupt_dal = NULL;
   DALResult result;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp_device);

   UART_LOG_0(INFO, "+Uart_interrupt_open");


   result = DALSYS_Malloc(sizeof(UART_INTERRUPT_CONTEXT), (void **)(&uart_interrupt_context));
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR,"DALSYS_Malloc Attach failed");
      return UART_ERROR;
   }
   h->interrupt_context = (void*)uart_interrupt_context;
   result = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &interrupt_dal);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR,"DALDEVICEID_INTERRUPTCONTROLLER Attach failed");
      return UART_ERROR;
   }

   result = DalDevice_Open(interrupt_dal, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR,"DALDEVICEID_INTERRUPTCONTROLLER open failed");
      return UART_ERROR;
   }

   if( DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(h->port_id,hProp_device))
   {
      UART_LOG_0(ERROR,"DALSYS_GetDALPropertyHandleStr for device failed");
      return UART_ERROR;
   }

   // Get the interrupt specific Properties in to the interrupt context.
   get_dword_property (hProp_device, "Irq",              &uart_interrupt_context->irq_num,0);
   get_dword_property (hProp_device, "UartIntSelOffset", &tcsr_offset,0);
   get_dword_property (hProp_device, "UartIntSelVal",    &uart_interrupt_context->tcsr_mask,0);

   result = DalInterruptController_RegisterISR(interrupt_dal,
                                                         (DALInterruptID)uart_interrupt_context->irq_num,
                                                         (DALISR)isr, h,
                                                         DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR,"DalInterruptController_RegisterISR failed");
      return UART_ERROR;
   }													 
   uart_interrupt_context->dal_interrupt_handle =  interrupt_dal;

   // If the processor uses TCSR for interrupts, configure the UART TCSR registers.
   if (tcsr_offset)
   {
      // Map the TCSR region and get the virtual address of the TCSR REGS base.
      if(DalHWIO_MapRegion(dal_hwio,"+TCSR_TCSR_REGS",&virtual_tcsr_base) != DAL_SUCCESS)
      {
         UART_LOG_0(ERROR,"DalHWIO_MapRegion for TCSR failed");
         return UART_ERROR;
      }

      uart_interrupt_context->uart_tcsr_addr = (uint32*)(virtual_tcsr_base + tcsr_offset);

      //Configure the TCSR register.
      configure_tcsr(h,TRUE);
   }

   UART_LOG_0(INFO, "-Uart_interrupt_open");
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_tlmm_close

DESCRIPTION:

==================================================================================================*/
UartResult Uart_tlmm_close(UART_CONTEXT* h)
{
   DalDeviceHandle *dal_tlmm;
   uint32 gpio_group[4];
   uint32 num_gpios = 0;
   DALResult result;

   UART_LOG_0(INFO, "+Uart_tlmm_close");

   dal_tlmm = (DalDeviceHandle *)h->tlmm_handle;

   if(h->properties.gpio_rfr_config){gpio_group[num_gpios++] = h->properties.gpio_rfr_config;}
   if(h->properties.gpio_cts_config){gpio_group[num_gpios++] = h->properties.gpio_cts_config;}
   if(h->properties.gpio_tx_config){gpio_group[num_gpios++] = h->properties.gpio_tx_config;}
   if(h->properties.gpio_rx_config){gpio_group[num_gpios++] = h->properties.gpio_rx_config;}


   result = DalTlmm_ConfigGpioGroup(dal_tlmm, DAL_TLMM_GPIO_DISABLE,gpio_group, num_gpios);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "DalTlmm_ConfigGpioGroup Disable failed.");
      return UART_ERROR;
   }

   DalDevice_Close(dal_tlmm);
   DAL_DeviceDetach(dal_tlmm);

   UART_LOG_0(INFO, "-Uart_tlmm_close");
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_tlmm_open

DESCRIPTION:

==================================================================================================*/
UartResult Uart_tlmm_open(UART_CONTEXT* h)
{
   DalDeviceHandle *dal_tlmm = NULL;
   uint32 gpio_group[4];
   uint32 num_gpios = 0;
   DALResult result;

   UART_LOG_0(INFO, "+Uart_tlmm_open");

   result = DAL_DeviceAttach(DALDEVICEID_TLMM, &dal_tlmm);
   if (result != DAL_SUCCESS)
   {
      return UART_ERROR;
   }

   result = DalDevice_Open(dal_tlmm, DAL_OPEN_SHARED);
   if (result != DAL_SUCCESS)
   {
      return UART_ERROR;
   }

   h->tlmm_handle = (void*)dal_tlmm;

   if(h->properties.gpio_rfr_config){gpio_group[num_gpios++] = h->properties.gpio_rfr_config;}
   if(h->properties.gpio_cts_config){gpio_group[num_gpios++] = h->properties.gpio_cts_config;}
   if(h->properties.gpio_tx_config){gpio_group[num_gpios++] = h->properties.gpio_tx_config;}
   if(h->properties.gpio_rx_config){gpio_group[num_gpios++] = h->properties.gpio_rx_config;}

   if (!(h->properties.gpio_tx_config && h->properties.gpio_rx_config))
   {
      UART_LOG_2(ERROR, "GPIO configurations of RX or TX or both missing. TX: %d, RX: %d",
                                                               h->properties.gpio_tx_config,
                                                               h->properties.gpio_rx_config );
      return UART_ERROR;
   }

   result = DalTlmm_ConfigGpioGroup(dal_tlmm, DAL_TLMM_GPIO_ENABLE, gpio_group, num_gpios);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "DalTlmm_ConfigGpioGroup Enable failed.");
      return UART_ERROR;
   }

   UART_LOG_0(INFO, "-Uart_tlmm_open");
   return UART_SUCCESS;

}
