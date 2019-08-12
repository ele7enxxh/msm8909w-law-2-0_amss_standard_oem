/*==================================================================================================

FILE: Uart.c

DESCRIPTION: This module provides the driver Software for the UART.

                           Copyright (c) 2014 Qualcomm Technologies Incorporated
                                        All Rights Reserved
                                     QUALCOMM Proprietary/GTDR

==================================================================================================*/
/*==================================================================================================
                                            DESCRIPTION
====================================================================================================

GLOBAL FUNCTIONS:
   Uart_Deinit
   Uart_Init
   Uart_Receive
   Uart_Transmit

==================================================================================================*/
/*==================================================================================================
Edit History

$Header: //components/rel/core.mpss/3.5.c12.3/buses/uart_lite/src/Uart.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
09/25/13   VV      Included interrupt support.
06/24/13   VV      Initial revision.

==================================================================================================*/

/*==================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "Uart_hwio.h"
#include "Uart.h"
#include "Uart_defs.h"
#include "Uart_os.h"
#include "Uart_hal.h"
#include "Uart_log.h"

/*==================================================================================================
                                              TYPEDEFS
==================================================================================================*/
typedef struct
{
   uint32 bit_rate;
   uint32 input_freq;
   uint32 divider;
} CLOCK_SETTINGS;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
#define UART_LOGGING_FILE_ID 10

#define TX_LEV_MARGIN 0
#define RX_LEV_MARGIN 16


/* Initializing the variables to a non-zero values so that they don't get cleared during Ram init*/
UART_DRIVER_PROPERTIES uart_driver_props = { {"/core/buses/uart/0"} };

UART_CONTEXT static_uart_context[UART_MAX_PORTS] =
{
   {NULL, FALSE, {0, 115200, 0, 0, 0, 0, 0, 0}, NULL, 0, 0, 0},
};

static const CLOCK_SETTINGS baud_table[] =
{
   // bit_rate, input_freq, divider
   {     115200,    3686400,      0xEE },
   {0}
};

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static void         check_rx_error(UART_CONTEXT* uart_ctxt);
static UartResult   clock_enable(UART_CONTEXT* uart_ctxt);
static void         flush_tx(UART_CONTEXT* uart_ctxt);
static uint32       process_tx_data(UART_CONTEXT* uart_ctxt,char* buf, uint32 bytes_to_tx);
static uint32       receive_data(UART_CONTEXT* uart_ctxt, char* buf,uint32 bytes_to_read);
static void         register_init(UART_CONTEXT* uart_ctxt);
static void         rx_transfer_start(UART_CONTEXT* uart_ctxt);
static void*        uart_isr(void *h);

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*==================================================================================================

FUNCTION: check_rx_error

DESCRIPTION:

==================================================================================================*/
static void check_rx_error(UART_CONTEXT* uart_ctxt)
{
   uint32 sr;

   sr = HALuart_GetStatus(uart_ctxt->properties.uart_base);

   if (sr & UARTDM_SR_OVR_ERR)
   {
      UART_LOG_0(ERROR,"UART overrun detected.");
   }

   if (sr & UARTDM_SR_PF_ERR)
   {
      UART_LOG_0(ERROR,"UART parity framing error detected.");
   }
}

/*==================================================================================================

FUNCTION: clock_enable

DESCRIPTION:

==================================================================================================*/
static UartResult clock_enable(UART_CONTEXT* uart_ctxt)
{
   uint32 input_freq;
   uint32 divider;
   uint32 bit_rate;
   const CLOCK_SETTINGS *entry;

   bit_rate = uart_ctxt->properties.bit_rate;
   for (entry = baud_table; entry->bit_rate; entry++)
   {
      if ( entry->bit_rate == bit_rate ) { break; }
   }

   if (entry->bit_rate == 0)
   {
      UART_LOG_1(ERROR,"Invalid baud rate : %d",bit_rate);
      return UART_ERROR;
   }
   input_freq = entry->input_freq;
   divider    = entry->divider;

   if(UART_SUCCESS != Uart_clock_open(uart_ctxt, input_freq))
   {
      UART_LOG_1(ERROR,"Uart_clock_open failed. Requested Frequency : %d", input_freq);
      return UART_ERROR;
   }

   HALuart_SetDivider(uart_ctxt->properties.uart_base, divider);
   return UART_SUCCESS;
}

/*==================================================================================================

FUNCTION: flush_tx

DESCRIPTION:

==================================================================================================*/
static void flush_tx(UART_CONTEXT* uart_ctxt)
{
   uint32 base = uart_ctxt->properties.uart_base;
   uint32 total_wait_time;
   uint32 char_time;
   uint32 wait_per_iter;

   // Calculate character time. Assuming 8 bits per character with 1 stop bit and no parity.
   char_time = ((1000000/uart_ctxt->properties.bit_rate) + 1) * 10;

   total_wait_time = HALuart_GetTxFifoSize(base) * char_time * 2;
   wait_per_iter = 10 * char_time;

   while(((HALuart_GetStatus(base) & UARTDM_SR_TXEMT) == 0))
   {
      Uart_busy_wait(wait_per_iter);
      total_wait_time -= (total_wait_time > wait_per_iter) ? wait_per_iter : total_wait_time;
      if (total_wait_time == 0)
      {
         UART_LOG_0(ERROR,"Flush timeout. FIFO still has data left.");
         break;
      }
   }
}
/*==================================================================================================

FUNCTION: process_tx_data

DESCRIPTION: This function calls the HAL layer to transmit data and also does polling for TXLEV
             and calculates the wait time.

==================================================================================================*/

static uint32 process_tx_data(UART_CONTEXT* uart_ctxt,char* buf, uint32 num_bytes)
{
   uint32 base = uart_ctxt->properties.uart_base;
   uint32 bytes_to_tx = num_bytes;
   uint32 bytes_txed = 0;
   uint32 char_time;
   uint32 wait_time;
   uint32 total_time_to_wait;
   uint32 tx_fifo_size;

   tx_fifo_size = HALuart_GetTxFifoSize(base);

   // Calculate character time. Used to calculate the wait time for required empty FIFO slots.
   // Assuming 8 bits per character with 1 stop bit and no parity.
   char_time = ((1000000/uart_ctxt->properties.bit_rate) + 1) * 10;

   // Calculate total time to wait. 2 * (character time *  msg length)
   // Return if wait time exceeds this value with bytes trasnmitted
   // Prevents infinite looping.
   total_time_to_wait = 2 * (bytes_to_tx * char_time);

   while (bytes_to_tx > 0)
   {
      bytes_txed += HALuart_WriteTxFifo(base, (buf + bytes_txed), bytes_to_tx);

      bytes_to_tx = num_bytes - bytes_txed;

      if (bytes_to_tx == 0)
      {
         return bytes_txed;
      }
      // If the time waited is equal to total time allowed, return with number of bytes transmitted.
      // Log the error message.
      if (total_time_to_wait == 0)
      {
         UART_LOG_1(ERROR,"Unable to send all characters. Characters remaining: %d", bytes_to_tx);
         return bytes_txed;
      }

      // If there is pending bytes to be transmitted wait for the TXLEV interrupt.
      // TX watermark will be set appropriately in the HALuart_WriteTxFifo.
      while(total_time_to_wait && ((HALuart_GetInterruptStatus(base) & UARTDM_ISR_TXLEV) == 0))
      {
         wait_time = ((bytes_to_tx > tx_fifo_size)? (tx_fifo_size) : (bytes_to_tx)) * char_time;
         Uart_busy_wait(wait_time);
         total_time_to_wait -=(total_time_to_wait > wait_time) ? (wait_time) : (total_time_to_wait);
      }

   }
   return bytes_txed;
}

/*==================================================================================================

FUNCTION: receive_data

DESCRIPTION:

==================================================================================================*/
static uint32 receive_data(UART_CONTEXT* uart_ctxt, char* buf, uint32 bytes_to_rx)
{
   uint32 base = uart_ctxt->properties.uart_base;
   uint32 bytes_read;
   boolean is_stale_intr = FALSE;

   // read_buf being NULL indicates we are in no interrupt and no memory image.
   // So use the buffer provided by the client.
   //
   // If read_buf exists, then we will copy the data from the read_buf in to the client buffer.
   // The read_buf will be filled whenever we get a RXLEV or RXSTALE interrupts.
   check_rx_error(uart_ctxt);
   if (uart_ctxt->read_buf == NULL)
   {
      if (bytes_to_rx > uart_ctxt->rx_bytes_left)
      {
         bytes_to_rx = uart_ctxt->rx_bytes_left;
      }

      // If it is stale event. Reset it.
      if (HALuart_GetInterruptStatus(base) & UARTDM_ISR_RXSTALE)
      {
         HALuart_ResetStaleInterrupt(base); // Clear the stale interrupt.
         bytes_to_rx = HALuart_GetDmaRxSnapChars(base);
         is_stale_intr = TRUE;
      }
      bytes_read = HALuart_ReadRxFifo(base, buf, bytes_to_rx);
      uart_ctxt->rx_bytes_left -= bytes_read;

      if (is_stale_intr)
      {
         rx_transfer_start(uart_ctxt);
      }
      return bytes_read;
   }
   else
   {
      return 0;
   }
}

/*==================================================================================================

FUNCTION: reginit

DESCRIPTION:

==================================================================================================*/
static void register_init(UART_CONTEXT* uart_ctxt)
{
   uint32 base = uart_ctxt->properties.uart_base;
   uint32 fifo_size;

   fifo_size = HALuart_GetFifoSize(base);

   HALuart_SetCharFormat        (base, UARTDM_8_BITS_PER_CHAR, UARTDM_1_STOP_BIT, UARTDM_NO_PARITY);
   HALuart_SetCmdSynchronization(base, TRUE);
   HALuart_SetTxFifoSize        (base, fifo_size * TX_FIFO_RATIO);
   HALuart_SetDmaEnable         (base, FALSE);
   HALuart_SetTxSCEnable        (base, TRUE); // Enable Single Character mode only for Trasmit path.
   HALuart_SetRxSCEnable        (base, FALSE);
   HALuart_SetAutoInboundFlowCtl(base, FALSE);
   HALuart_SetOutboundFlowCtl   (base, FALSE);
   HALuart_SetLoopback          (base, uart_ctxt->properties.is_loopback);
   HALuart_DisableInterrupts    (base, 0xFFFFFFFF);
   HALuart_SetStaleTimeout      (base, 0x0F); // 15 character time.
   HALuart_SetIrdaEnable        (base, FALSE);
   HALuart_SetTxWatermark       (base, TX_LEV_MARGIN);  // Must be a multiple of 4
   HALuart_SetRxWatermark       (base, RX_LEV_MARGIN);  // Must be a multiple of 4

   HALuart_ResetReceiver        (base);
   HALuart_ResetTransmitter     (base);
   HALuart_ResetErrorStatus     (base);
   HALuart_SetRxEnable          (base, TRUE);
   HALuart_SetTxEnable          (base, TRUE);

   rx_transfer_start(uart_ctxt);
}


/*==================================================================================================

FUNCTION: rx_transfer_start

DESCRIPTION:
   This function starts a new RX transfer.

==================================================================================================*/
static void rx_transfer_start(UART_CONTEXT* uart_ctxt)
{
   uint32 base = uart_ctxt->properties.uart_base;
   // We must write DMRX (start a new rx transfer) and then CR (enable stale
   // event), in that order.  If we reverse this order it is possible to get
   // a stale interrupt without a corresponding update to the SNAP register.

   HALuart_SetupDmaRx(base, RX_XFER_SIZE);
   HALuart_SetStaleEvent(base, TRUE);

   // Enable the RX Level and Stale interrupts.
   HALuart_EnableInterrupts(base, UARTDM_ISR_RXLEV | UARTDM_ISR_RXSTALE);
   uart_ctxt->rx_bytes_left = RX_XFER_SIZE;
}



/*==================================================================================================

FUNCTION: Uart_deinit

DESCRIPTION:

==================================================================================================*/
UartResult Uart_deinit(UartHandle h)
{
   UART_CONTEXT* uart_ctxt;
   UartResult    result = UART_SUCCESS;

   UART_LOG_0(INFO,"+Uart_deinit");

   if (NULL == h)
   {
      UART_LOG_0(ERROR,"Calling Uart_deinit with a NULL handle.");
      return UART_ERROR;
   }

   uart_ctxt = (UART_CONTEXT*)h;
   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG_0(ERROR,"Calling Uart_deinit on a closed port.");
      return UART_ERROR;
   }

   flush_tx(uart_ctxt);

   uart_ctxt->is_port_open = FALSE;

   if(UART_SUCCESS != Uart_clock_close(uart_ctxt))
   {
      UART_LOG_0(ERROR, "Uart_clock_close failed.");
      result = UART_ERROR;
   }
   if(UART_SUCCESS != Uart_tlmm_close(uart_ctxt))
   {
      UART_LOG_0(ERROR, "Uart_tlmm_close failed.");
      result = UART_ERROR;
   }
   if(UART_SUCCESS != Uart_interrupt_close(uart_ctxt))
   {
      UART_LOG_0(ERROR, "Uart_interrupt_close failed.");
      result = UART_ERROR;
   }

   UART_LOG_0(INFO,"-Uart_deinit");
   return result;
}


/*==================================================================================================

FUNCTION: Uart_init

DESCRIPTION:

==================================================================================================*/
UartResult Uart_init(UartHandle* h,UartPortID id)
{
   UART_CONTEXT* uart_ctxt;

   UART_LOG_0(INFO,"+Uart_init");

   if (NULL == h)
   {
      UART_LOG_0(ERROR, "Calling Uart_init with NULL handle pointer.");
      return UART_ERROR;
   }

   *h = NULL;

   if(UART_ERROR == Uart_get_driver_properties())
   {
      UART_LOG_0(ERROR, "Uart_get_driver_properties failed");
      return UART_ERROR;
   }

   if (id < UART_MAX_PORTS)
   {
      uart_ctxt = &static_uart_context[id];

      // In SBL, all the drivers, that needs to log using UART, will directly call the UART APIs
      // They need to call the Uart_init function before using any of the APIs.
      // So initialize the UART only on the first call. From the second call just return the handle
      // and log it.
      if (uart_ctxt->is_port_open == TRUE)
      {
         *h = (UartHandle)uart_ctxt;
         UART_LOG_1(INFO, "Port is already open. Port: %d", id);
         return UART_SUCCESS;
      }

      if (NULL == uart_driver_props.device_names[id])
      {
         UART_LOG_1(ERROR, "Physical device is not defined in UART properties XML. Port: %d", id);
         return UART_ERROR;
      }
      uart_ctxt->port_id = uart_driver_props.device_names[id];
      uart_ctxt->read_buf = NULL;
   }
   else
   {
      UART_LOG_1(ERROR, "Invalid Port ID. Port: %d", id);
      return UART_ERROR;
   }

   if(UART_SUCCESS != Uart_get_properties(uart_ctxt))
   {
      UART_LOG_0(ERROR, "Uart_get_properties failed.");
      return UART_ERROR;
   }
   if(UART_SUCCESS != clock_enable(uart_ctxt))
   {
      UART_LOG_0(ERROR, "clock_enable failed.");
      return UART_ERROR;
   }
   if(UART_SUCCESS != Uart_tlmm_open(uart_ctxt))
   {
      UART_LOG_0(ERROR, "Uart_tlmm_open failed.");
      Uart_deinit((UartHandle)uart_ctxt);
      return UART_ERROR;
   }
   if(UART_SUCCESS != Uart_interrupt_open(uart_ctxt,(void*)uart_isr))
   {
      UART_LOG_0(ERROR, "Uart_interrupt_open failed.");
      Uart_deinit((UartHandle)uart_ctxt);
      return UART_ERROR;
   }

   register_init(uart_ctxt);

   *h = (UartHandle)uart_ctxt;
   uart_ctxt->is_port_open = TRUE;

   UART_LOG_0(INFO,"-Uart_init");
   return UART_SUCCESS;
}


/*==================================================================================================

FUNCTION: uart_isr

DESCRIPTION:

==================================================================================================*/
static void *uart_isr(void *h)
{
   UART_CONTEXT *uart_ctxt = (UART_CONTEXT *)h;
   uint32 isr;
   uint32 misr;
   uint32 base = uart_ctxt->properties.uart_base;

   UART_LOG_0(INFO,"+uart_isr");

   isr         = HALuart_GetInterruptStatus(base);
   misr        = HALuart_GetMaskedInterruptStatus(base);

   UART_LOG_2(INFO,"MISR = %d. ISR = %d", misr, isr);

   if (misr)
   {
      if (uart_ctxt->client_callback)
      {
         if ((misr & UARTDM_ISR_RXSTALE) || (misr & UARTDM_ISR_RXLEV))
         {
            uint32 num_bytes_available = 0;

            HALuart_DisableInterrupts(base,UARTDM_ISR_RXSTALE | UARTDM_ISR_RXLEV);

            if (misr & UARTDM_ISR_RXSTALE)
            {
               num_bytes_available = HALuart_GetDmaRxSnapChars(base);
            }
            else
            {
               num_bytes_available = RX_LEV_MARGIN;
            }

            // The client callback can call the Uart_receive within the ISR context.
            UART_LOG_0(INFO, "Calling client callback.");
            uart_ctxt->client_callback(UART_EVENT_RX, num_bytes_available);

            HALuart_EnableInterrupts(base, UARTDM_ISR_RXLEV | UARTDM_ISR_RXSTALE);
         }
      }
      else
      {
         UART_LOG_0(INFO,"Client Callback not registered. Ignoring the ISR");
      }
   }

   Uart_interrupt_done(uart_ctxt);

   return NULL;
}

/*==================================================================================================

FUNCTION: Uart_receive

DESCRIPTION:

==================================================================================================*/
uint32 Uart_receive(UartHandle h, char* buf,uint32 bytes_to_rx)
{
   UART_CONTEXT* uart_ctxt;
   uint32 bytes_read;

   UART_LOG_0(INFO,"+Uart_receive");

   if (NULL == h)
   {
      UART_LOG_0(ERROR,"Calling Uart_receive with a NULL handle.");
      return 0;
   }

   if (NULL == buf)
   {
      UART_LOG_0(ERROR,"Calling Uart_receive with a NULL buffer.");
      return 0;
   }

   if (0 == bytes_to_rx)          { return 0; }

   uart_ctxt = (UART_CONTEXT*)h;

   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG_0(ERROR,"Calling Uart_receive on a closed port.");
      return 0;
   }

   bytes_read = receive_data(uart_ctxt,buf,bytes_to_rx);
   UART_LOG_0(INFO,"-Uart_receive");

   return bytes_read;
}


/*==================================================================================================

FUNCTION: Uart_register_callback

DESCRIPTION:

==================================================================================================*/
UartResult Uart_register_event_callback(UartHandle h, UART_CALLBACK client_callback)
{
   if (NULL == h)
   {
      UART_LOG_0(ERROR,"Calling Uart_register_callback with a NULL handle");
      return UART_ERROR;
   }

   if (NULL == client_callback)
   {
      UART_LOG_0(ERROR,"Calling Uart_register_callback with a NULL callback function");
      return UART_ERROR;
   }

   ((UART_CONTEXT*)h)->client_callback = client_callback;

   return UART_SUCCESS;
}


/*==================================================================================================

FUNCTION: Uart_transmit

DESCRIPTION:

==================================================================================================*/
uint32 Uart_transmit(UartHandle h, char* buf, uint32 bytes_to_tx)
{
   UART_CONTEXT* uart_ctxt;
   uint32 bytes_sent;

   if (NULL == h)
   {
      UART_LOG_0(ERROR,"Calling Uart_transmit with a NULL handle.");
      return 0;
   }

   if (NULL == buf)
   {
      UART_LOG_0(ERROR,"Calling Uart_transmit with a NULL buffer.");
      return 0;
   }

   if (0 == bytes_to_tx)            { return 0; }

   uart_ctxt = (UART_CONTEXT*)h;

   if (FALSE == uart_ctxt->is_port_open)
   {
      UART_LOG_0(ERROR,"Calling Uart_transmit on a closed port.");
      return 0;
   }

   UART_LOG_0(INFO,"+Uart_transmit");
   bytes_sent = process_tx_data(uart_ctxt,buf,bytes_to_tx);
   UART_LOG_0(INFO,"-Uart_transmit");

   return bytes_sent;
}
