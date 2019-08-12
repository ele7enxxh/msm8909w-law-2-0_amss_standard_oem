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
   Uart_clock_open
   Uart_clock_close
   Uart_get_properties
   Uart_tlmm_open
   Uart_tlmm_close
   Uart_busy_wait

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/uart/src/Uart_sbl_os.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
06/24/13   VV      Initial revision.

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
#include "boot_extern_clk_interface.h"

extern UART_DRIVER_PROPERTIES uart_driver_props;
#define UART_LOGGING_FILE_ID 20

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/
typedef struct
{
   uint32                   freq;
   ClockBootPerfLevelType   perf_level;
} UART_FREQ_PERF_LEVEL_SETTINGS;
/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static const UART_FREQ_PERF_LEVEL_SETTINGS freq_table[] =
{
   //Frequency, Clock performance level
   { 3686400,  CLOCK_BOOT_PERF_MIN},
   {0}
};

static const ClockUARTType uart_clocks[] =
{
   CLK_BLSP_UART_NONE,
   CLK_BLSP1_UART1_APPS,
   CLK_BLSP1_UART2_APPS,
   CLK_BLSP1_UART3_APPS,
   CLK_BLSP1_UART4_APPS,
   CLK_BLSP1_UART5_APPS,
   CLK_BLSP1_UART6_APPS,
   CLK_BLSP2_UART1_APPS,
   CLK_BLSP2_UART2_APPS,
   CLK_BLSP2_UART3_APPS,
   CLK_BLSP2_UART4_APPS,
   CLK_BLSP2_UART5_APPS,
   CLK_BLSP2_UART6_APPS
};
/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void      get_dword_property(DALSYSPropertyHandle handle,const char* property_name,
                                    uint32* property_value, uint32 default_value);
static void      get_string_property(DALSYSPropertyHandle handle,const char* property_name,
                                    char** property_value, char* default_value);


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
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
void Uart_busy_wait(uint32 uSecs)
{
   DALSYS_BusyWait(uSecs);
}

/*==================================================================================================

FUNCTION: Uart_clock_close

DESCRIPTION:

==================================================================================================*/
UartResult Uart_clock_close(UART_CONTEXT* h)
{
   ClockUARTType            uart_core_clock;

   UART_LOG_0(INFO,"+Uart_clock_close");

   if (0 == h->properties.clock_id_index)
   {
      UART_LOG_1(ERROR,"Invalid clock id index. Index: %d", h->properties.clock_id_index);
      return UART_ERROR;
   }
   uart_core_clock = uart_clocks[h->properties.clock_id_index];

   
   if(FALSE == boot_clock_disable_UARTClock(uart_core_clock) )
   {
      UART_LOG_0(ERROR, "Clock_DisableUARTClock failed.");
      return UART_ERROR;
   }
   
   UART_LOG_0(INFO,"-Uart_clock_close");
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_clock_open

DESCRIPTION:

==================================================================================================*/
UartResult Uart_clock_open(UART_CONTEXT* h, uint32 input_freq)
{
   ClockUARTType            uart_core_clock;
   const UART_FREQ_PERF_LEVEL_SETTINGS *entry;

   UART_LOG_0(INFO, "+Uart_clock_open");

   for (entry = freq_table; entry->freq; entry++)
   {
      if (entry->freq == input_freq) { break; }
   }
   if (0 == entry->freq)
   {
      UART_LOG_1(ERROR, "Unable to find Perf level. Frequency: %d", entry->freq);
      return UART_ERROR;
   }

   if ((0 == h->properties.clock_id_index) ||
       (h->properties.clock_id_index > ( (sizeof(uart_clocks)/sizeof(ClockUARTType) - 1 ) )))
   {
      UART_LOG_1(ERROR,"Invalid clock id index. Index: %d", h->properties.clock_id_index);
      h->properties.clock_id_index = 0;
      return UART_ERROR;
   }
   uart_core_clock = uart_clocks[h->properties.clock_id_index];
   
   if(FALSE == boot_clock_SetUARTClockFrequency(entry->perf_level, uart_core_clock) )
   {
      UART_LOG_1(ERROR, "Clock_SetUARTClockFrequency failed. Frequency: %d", entry->freq);
      return UART_ERROR;
   }
   
   UART_LOG_0(INFO, "-Uart_clock_open");
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_get_driver_properties

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
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp_device);

   UART_LOG_0(INFO, "+Uart_get_properties");

   if( DAL_SUCCESS != DALSYS_GetDALPropertyHandleStr(h->port_id,hProp_device))
   {
      UART_LOG_0(ERROR,"DALSYS_GetDALPropertyHandleStr for device failed");
      return UART_ERROR;
   }

   get_dword_property (hProp_device, "IsLoopback",   &h->properties.is_loopback,0);
   get_dword_property (hProp_device, "BitRate",      &h->properties.bit_rate,0);
   get_dword_property (hProp_device, "UartBase",     &h->properties.uart_base,0);
   get_dword_property (hProp_device, "GpioTxData",   &h->properties.gpio_tx_config,0);
   get_dword_property (hProp_device, "GpioRxData",   &h->properties.gpio_rx_config,0);
   get_dword_property (hProp_device, "GpioRfrN",     &h->properties.gpio_rfr_config,0);
   get_dword_property (hProp_device, "GpioCtsN",     &h->properties.gpio_cts_config,0);
   get_dword_property (hProp_device, "ClockIdIndex", &h->properties.clock_id_index,0);

   UART_LOG_0(INFO, "-Uart_get_properties");
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_tlmm_close

DESCRIPTION:

==================================================================================================*/
UartResult Uart_tlmm_close(UART_CONTEXT* h)
{
   uint32 gpio_group[4];
   uint32 num_gpios = 0;
   DALResult result;

   UART_LOG_0(INFO, "+Uart_tlmm_close");

   if(h->properties.gpio_rfr_config){gpio_group[num_gpios++] = h->properties.gpio_rfr_config;}
   if(h->properties.gpio_cts_config){gpio_group[num_gpios++] = h->properties.gpio_cts_config;}
   if(h->properties.gpio_tx_config){gpio_group[num_gpios++] = h->properties.gpio_tx_config;}
   if(h->properties.gpio_rx_config){gpio_group[num_gpios++] = h->properties.gpio_rx_config;}


   result = Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_DISABLE, gpio_group, num_gpios);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "Tlmm_ConfigGpioGroup Disable failed.");
      return UART_ERROR;
   }

   UART_LOG_0(INFO, "-Uart_tlmm_close");
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: Uart_tlmm_open

DESCRIPTION:

==================================================================================================*/
UartResult Uart_tlmm_open(UART_CONTEXT* h)
{

   uint32 gpio_group[4];
   uint32 num_gpios = 0;
   DALResult result;

   UART_LOG_0(INFO, "+Uart_tlmm_open");

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

   result = Tlmm_ConfigGpioGroup(DAL_TLMM_GPIO_ENABLE, gpio_group, num_gpios);
   if (result != DAL_SUCCESS)
   {
      UART_LOG_0(ERROR, "Tlmm_ConfigGpioGroup Enable failed.");
      return UART_ERROR;
   }

   UART_LOG_0(INFO, "-Uart_tlmm_open");
   return UART_SUCCESS;
}
