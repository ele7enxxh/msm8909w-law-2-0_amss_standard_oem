/**
 * @file smdl_echo.c
 *
 * Echo/loopback service for SMD Lite
 */

/*==============================================================================
     Copyright (c) 2009-2011 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smd/src/smdl_echo.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/12   bt      Remove DSPS featurizations for B-family.
05/04/11   bm      Added support for DSPS as a special case of loopback server
05/02/11   tl      Keep the loopback port open when the client closes the port
03/25/11   bt      Added signal-forwarding echo callbacks for CD and RI events.
02/15/11   rs      Included missing header file 
01/25/11   bt      Updated to be able to forward DSR and CTS signals as well.
01/04/11   bt      Fix truncation of packets to be 24 bytes less than fifo.
01/04/11   tl      Truncate transmitted packets at 21 bytes less than fifo 
09/08/10   tl      Use DALSYS_Malloc().
04/21/10   tl      Updated to SMD Lite v2
11/17/09   tl      Moved SMD Lite echo driver to SMD tree
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "DALSys.h"
#include "smd_lite.h"
#include "smd_os.h"
#include "smd_internal.h"
#include "smdl_echo.h"
#include "smdl_debug.h"
#include <stdlib.h>
#include <string.h>

#include "CoreVerify.h"

/*===========================================================================
                      GLOBAL STATICS
===========================================================================*/

/*===========================================================================
                      LOCAL TYPE DECLARATIONS
===========================================================================*/

/**
 * The control data for each echo data port.
 *
 * This is allocated from the heap by smdl_echo_open_local() and freed
 * by smdl_echo_data_cb().
 */
typedef struct smdl_echo_struct
{
  /** The size of the echo buffer. */
  uint32                buffer_len;

  /** The amount of data waiting to be written from the echo buffer. */
  uint32                data_len;

  /**
   * The buffer used for reading data from the echo port.
   *
   * This is allocated from the heap by smdl_echo_open_local() and freed
   * by smdl_echo_data_cb().
   */
  uint8                *buffer;

  smdl_handle_type      port;
  smdl_handle_type      rx_port;

  struct smdl_echo_struct *rx;
} smdl_echo_type;

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Allocate an SMD Lite echo handle and buffer for received data
 *
 * @param[in]  buffer_len   Length of the buffer to use for data received by
 *                          the echo port. Should be equal to the FIFO size.
 *
 * @return Pointer to an SMD Lite echo handle, or NULL if the memory could not
 *         be allocated.
 */
static smdl_echo_type * smdl_echo_alloc_handle
(
  uint32 buffer_len
)
{
  smdl_echo_type *handle;

  if(DALSYS_Malloc(sizeof(smdl_echo_type), (void **)&handle) != DAL_SUCCESS)
  {
    return NULL;
  }
  
  CORE_VERIFY_PTR(handle);

  memset(handle, 0, sizeof(smdl_echo_type));
  handle->buffer_len = buffer_len;

  if(DALSYS_Malloc(handle->buffer_len, (void **)&handle->buffer) != DAL_SUCCESS)
  {
    DALSYS_Free(handle);
    return NULL;
  }

  return handle;
}

/**
 * Frees an SMD Lite echo handle
 *
 * @param[in]  handle  The SMD Lite echo handle. Must have been allocated by
 *                     smdl_echo_alloc_handle() and must not be NULL.
 *
 * @return None
 */
static void smdl_echo_free_handle
(
  smdl_echo_type *handle
)
{
  DALSYS_Free(handle->buffer);

  DALSYS_Free(handle);
}

/*
 * Reads data "from" and writes data "to"
 *
 * Uses the "from" buffer to store data until it can be written "to"
 */
void smdl_echo_read_write
(
  smdl_echo_type *from,
  smdl_echo_type *to
)
{
  SMDL_DEBUG("smdl_echo_read_write: from=%p to=%p from->data_len=%d",
      from, to, from->data_len);

  /* Try to write data in the return buffer */
  if(from->data_len > 0)
  {
    int32 fifo_free_space = smdl_tx_peek(to->port);
    int32 wlen = smdl_write(to->port, from->data_len, from->buffer, 0);
    SMDL_DEBUG("SMDL echo(%p): Wrote %d (free %d)",
        to->port, wlen, fifo_free_space);
    if(wlen == 0)
    {
      /* Outgoing port full; data could not be written; wait until the
       * next event_write. */
    }
    else if(wlen != from->data_len)
    {
      ERR_FATAL("Echo port: smdl_write() failed", 0, 0, 0);
    }
    else
    {
      from->data_len = 0;
    }
  }

  /* Try to read more data from the incoming FIFO and write it back to the
   * outgoing FIFO */
  if(from->data_len == 0)
  {
    int32 len;
    do {
      len = smdl_read(from->port, from->buffer_len, from->buffer, 0);
      SMDL_DEBUG("smdl_echo_read_write: from=%p from->port=%p read=%d",
          from, from->port, len);
      if(len > 0)
      {
        /* The maximum packet size that can be written is the FIFO size minus
         * the packet header (20 bytes) minus the must-be-empty padding
         * (currently 4 bytes). We can *read* messages larger than, if the
         * transmitter is using SMD, that because SMD Lite can read the packet
         * header first, leaving space for a packet up to the full capacity of
         * the FIFO. */
        int32 wlen;
        if((uint32)(len + SMD_PACKET_HEADER_BYTE_SIZE + 4) > from->buffer_len)
        {
          /* Truncate at maximum packet length */
          SMDL_DEBUG("SMDL echo(%p): Truncating %d to %d",
              from->port, len, from->buffer_len - 
                               (SMD_PACKET_HEADER_BYTE_SIZE + 4));
          len = from->buffer_len - (SMD_PACKET_HEADER_BYTE_SIZE + 4);
        }
        wlen = smdl_write(to->port, len, from->buffer, 0);
        SMDL_DEBUG("SMDL echo(%p): Read %d, wrote %d",
            from->port, len, wlen);
        if(wlen == 0)
        {
          /* Outgoing port full; data could not be written */
          from->data_len = len;
          len = 0;
        }
        else if(wlen != len)
        {
          ERR_FATAL("Echo port: smdl_write() failed", 0, 0, 0);
        }
      }
    } while(len > 0);
  }
}

/**
 * Callback for the echo data ports
 *
 * When data is waiting to be read (event == SMDL_EVENT_READ), this function
 * reads the first packet from the port and writes it to the return FIFO. If
 * space is not immediately available in the return FIFO, the data will be
 * stored until it can be transmitted (event == SMDL_EVENT_WRITE).
 * 
 * When the remote processor sets an RS232 signal (event == SMDL_EVENT_DSR or
 * SMDL_EVENT_RTS or SMDL_EVENT_CD or SMDL_EVENT_RI), the local processor sets 
 * its own corresponding signal.
 *
 * @param[in]  port     The echo data port.
 * @param[in]  event    The event signaled by SMD Lite.
 * @param[in]  data     Pointer to the echo data port control structure,
 *                      of type smdl_echo_type.
 *
 * @return     None
 */
static void smdl_echo_data_cb
(
  smdl_handle_type port,
  smdl_event_type  event,
  void            *data
)
{
  smdl_echo_type *handle = (smdl_echo_type *)data;
  int32 sig;

  SMDL_DEBUG("smdl_echo_data_cb: port=%p event=%d data=%p",
      port, event, data);

  switch(event)
  {
    case SMDL_EVENT_OPEN:
    case SMDL_EVENT_READ:
      smdl_echo_read_write(handle, handle->rx);
      break;

    case SMDL_EVENT_CLOSE:
      /* Ignore this event. The loopback port remains open indefinately, and
       * clients can close and reopen their end of the port as they see fit.
       */
      break;

    case SMDL_EVENT_WRITE:
      smdl_echo_read_write(handle->rx, handle);
      break;

    case SMDL_EVENT_DSR:
      sig = smdl_sig_get(handle->rx_port, SMDL_SIG_DTR_DSR);
      if (sig == 0 || sig == 1)
      {
        if (smdl_sig_set(handle->port, SMDL_SIG_DTR_DSR, (boolean)sig) < 0){
          ERR_FATAL("Echo Event: DSR sig set: The port is invalid or another "
              "error occurred. port=%p  DSR sig=%d", (uint32)port, sig, 0);
        }
      }
      break;

    case SMDL_EVENT_CTS:
      sig = smdl_sig_get(handle->rx_port, SMDL_SIG_RTS_CTS);
      if (sig == 0 || sig == 1)
      {
        if (smdl_sig_set(handle->port, SMDL_SIG_RTS_CTS, (boolean)sig) < 0){
          ERR_FATAL("Echo Event: CTS sig set: The port is invalid or another "
              "error occurred. port=%p  CTS sig=%d", (uint32)port, sig, 0);
        }
      }
      break;

    case SMDL_EVENT_CD:
      sig = smdl_sig_get(handle->rx_port, SMDL_SIG_CD);
      if (sig == 0 || sig == 1)
      {
        if (smdl_sig_set(handle->port, SMDL_SIG_CD, (boolean)sig) < 0){
          ERR_FATAL("Echo Event: CD sig set: The port is invalid or another "
              "error occurred. port=%p  CD sig=%d", (uint32)port, sig, 0);
        }
      }
      break;

    case SMDL_EVENT_RI:
      sig = smdl_sig_get(handle->rx_port, SMDL_SIG_RI);
      if (sig == 0 || sig == 1)
      {
        if (smdl_sig_set(handle->port, SMDL_SIG_RI, (boolean)sig) < 0){
          ERR_FATAL("Echo Event: RI sig set: The port is invalid or another "
              "error occurred. port=%p  RI sig=%d", (uint32)port, sig, 0);
        }
      }
      break;

    default:
      /* Ignore the other events */
      break;
  }
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Opens a echo data port on the local processor.
 *
 * @param[in]  name          The name of the port to open.
 * @param[in]  channel_type  The edge on which to open the echo port.
 *
 * @return       @li 0 
 *               @li A negative error code if an error occurred.
 *
 * @sideeffects  Opens an SMD Lite port on the local system.
 */
void smdl_setup_echo
(
  const char       *echo_a,
  const char       *echo_b,
  smd_channel_type  channel_type
)
{
  smdl_echo_type *handle_a;
  smdl_echo_type *handle_b;
  uint32 fifo_size = 8192;

  SMDL_INFO("smdl_echo_open(): Opening echo data port", 0, 0, 0);

  handle_a = smdl_echo_alloc_handle(fifo_size);
  if(handle_a == NULL)
  {
    return;
  }

  handle_b = smdl_echo_alloc_handle(fifo_size);
  if(handle_b == NULL)
  {
    smdl_echo_free_handle(handle_a);
    return;
  }

  handle_a->rx = handle_b;
  handle_b->rx = handle_a;

  handle_a->port = smdl_open(echo_a, channel_type, 0, fifo_size,
      smdl_echo_data_cb, handle_a);
  handle_b->port = smdl_open(echo_b, channel_type, 0, fifo_size,
      smdl_echo_data_cb, handle_b);

  handle_a->rx_port = handle_b->port;
  handle_b->rx_port = handle_a->port;

  SMDL_DEBUG("smdl_setup_echo(): handle_a=%p handle_a->port=%p handle_a->rx=%p",
      handle_a, handle_a->port, handle_a->rx);
  SMDL_DEBUG("smdl_setup_echo(): handle_b=%p handle_b->port=%p handle_b->rx=%p",
      handle_b, handle_b->port, handle_b->rx);
}

/**
 * Opens a loopback data port on the local processor.
 *
 * @param[in]  name          The name of the port to open.
 * @param[in]  channel_type  The edge on which to open the loopback port.
 * @param[in]  fifo_size     The FIFO size to use
 *
 * @return       none
 *
 * @sideeffects  Opens an SMD Lite port on the local system.
 */
void smdl_open_loopback
(
  const char       *name,
  smd_channel_type  channel_type,
  uint32            fifo_size
)
{
  smdl_echo_type *handle;

  SMDL_INFO("smdl_echo_open(): Opening loopback data port", 0, 0, 0);

  handle = smdl_echo_alloc_handle(fifo_size);
  if(handle == NULL)
  {
    return;
  }

  handle->rx = handle;

  handle->port = smdl_open(name, channel_type, 0, fifo_size,
      smdl_echo_data_cb, handle);
  handle->rx_port = handle->port;

  SMDL_DEBUG("smdl_open_loopback(): handle=%p",
      handle, 0, 0);
}

/**
 * Opens a loopback data port on the local processor.
 *
 * @param[in]  name          The name of the port to open.
 * @param[in]  channel_type  The edge on which to open the loopback port.
 *
 * @return       none
 *
 * @sideeffects  Opens an SMD Lite port on the local system.
 */
void smd_handle_loopback
(
  const char       *name,
  smd_channel_type  channel_type
)
{
  smdl_open_loopback(name, channel_type, 8192);
}
