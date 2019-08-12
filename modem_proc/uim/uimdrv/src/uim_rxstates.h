#ifndef UIM_RXSTATES_H
#define UIM_RXSTATES_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  UIM RX STATE MACHINE HEADER

GENERAL DESCRIPTION
  This header defines the external interface for the uim_rxstates module.

EXTERNALIZED FUNCTIONS

  uimdrv_rx_event_cb
    Called from uim_rxtx when an rx interrupt happens to execute the RX
    state machine.

Copyright (c) 2001-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/uimdrv/src/uim_rxstates.h#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/20/14   ak      Created

===========================================================================*/

/**
*  This function is the entry point into the RX state machine.
*  It is designed to be called from the uim_rxtx interrupt
*  handler.
*  
*  @param uim_ptr Pointer to the current instance of the UIM
*                 global structure
*  @param interrupt_processed The type of interrupt that fired
*  @param uart_status The UART status
*  @param uart_rx_buf_ptr Contains bytes received if applicable
*  @param no_of_bytes_received Number of bytes received if
*                              applicable
*  
*  @return boolean TRUE if an error occured 
*/
boolean uimdrv_rx_event_cb(uim_instance_global_type *uim_ptr,
                           dword interrupt_processed,
                           dword uart_status,
                           dword* uart_rx_buf_ptr,
                           uint32 no_of_bytes_received);

#endif  /* UIM_RXSTATES_H */
