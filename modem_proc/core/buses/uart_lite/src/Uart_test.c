#include "Uart.h"

static UartHandle serial_port;
static void rx_callback(UartEvent ue,uint32);

void uartlite_test(void)
{
   uint32 i = 0;
   char msg[] = "TX works!!!TX works!!!TX works!!!TX works!!!TX works!!!TX works!!!TX works!!!TX works!!!TX works!!!TX works!!!";
   char rx_msg[200] = "RX works";
   Uart_init(&serial_port,UART_MAIN_PORT);
   Uart_register_event_callback(serial_port, rx_callback);
/*   while (rxed < 10)
   {
      rxed += Uart_receive(serial_port, (rx_msg + rxed), 10);

      Uart_transmit(serial_port, rx_msg, rxed);
   }*/
   while (i < 10)
   { i++;	   
     Uart_transmit(serial_port, msg,110);
   }

}

static void rx_callback(UartEvent ue,uint32 bytes_available)
{

  uint32 rxed = 0;
   char rx_msg[200] = "RX works";
   Uart_transmit(serial_port, rx_msg,10);

   if (ue == UART_EVENT_RX)
   {
      rxed = Uart_receive(serial_port, rx_msg, bytes_available);

      Uart_transmit(serial_port, rx_msg,rxed);
   }

}
