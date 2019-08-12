/*!
  @file
  wci2_frame.c

  @brief
  Service for framing/unframing messages in SLIP or HDLC-like format for 
  sending over uart as a WCI-2 type 2 message

*/

/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*==========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/wci2/src/wci2_frame.c#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
07/21/14   btl     Adding SLIP
06/26/14   btl     Initial version

==========================================================================*/

/*=============================================================================

                      INCLUDE FILES

=============================================================================*/
#include <comdef.h>
#include <crc.h>

#include "wci2_frame.h"
#include "wci2_utils.h"
#include "wci2_core.h"

/*=============================================================================

                      DEFINES, CONSTANTS, GLOBALS

=============================================================================*/
#define HDLC_START_FLAG      0x7e
#define HDLC_ESCAPE_FLAG     0x7d
#define HDLC_ESCAPE_XOR_MASK 0x20

#define SLIP_END_FLAG 0xc0
#define SLIP_ESC_FLAG 0xdb
#define SLIP_ESC_END  0xdc
#define SLIP_ESC_ESC  0xdd
#define SLIP_FRAME_OVERHEAD 2

#define HDLC_SHOULD_ESCAPE_BYTE(byte) \
    ( HDLC_START_FLAG == (byte) || HDLC_ESCAPE_FLAG == (byte) )

typedef struct
{
  uint8 start_stop;
  uint8 esc;
  uint8 esc_xor_mask;
} wci2_frame_flags_s;

typedef struct
{
  wci2_frame_mode_e  mode;
  wci2_frame_flags_s flags;
} wci2_frame_data_s;

wci2_frame_data_s wci2_frame_data = 
{
  WCI2_FRAME_MODE_HDLC,
  {
    HDLC_START_FLAG, 
    HDLC_ESCAPE_FLAG, 
    HDLC_ESCAPE_XOR_MASK 
  }
};

/*=============================================================================

  FUNCTION:  wci2_frame_set_mode

=============================================================================*/
/*!
    @brief
    Set type2 framing mode into HDLC or SLIP protocol. Only one can be
    active at a time.

    @return
    void
*/
/*===========================================================================*/
void wci2_frame_set_mode( wci2_frame_mode_e mode )
{
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( mode == WCI2_FRAME_MODE_HDLC ||
               mode == WCI2_FRAME_MODE_SLIP );
  wci2_frame_data.mode = mode;
  if( mode == WCI2_FRAME_MODE_HDLC )
  {
    wci2_frame_data.flags.start_stop = HDLC_START_FLAG;
    wci2_frame_data.flags.esc = HDLC_ESCAPE_FLAG;
    wci2_frame_data.flags.esc_xor_mask = HDLC_ESCAPE_XOR_MASK;
  }
  else
  {
    wci2_frame_data.flags.start_stop = SLIP_END_FLAG;
    wci2_frame_data.flags.esc = SLIP_ESC_FLAG;
  }

  WCI2_MSG_1( HIGH, "TYPE2 Frame mode set to %d", mode );

  return;
}


/*=============================================================================

  FUNCTION:  wci2_frame_buffer_rx

=============================================================================*/
/*!
    @brief
    Buffer until start/stop flag is encountered. When this happens, return
    a pointer to the complete buffer (must be freed when done)

    @return
    wci2_t2_dyn_msg ptr
*/
/*===========================================================================*/
wci2_t2_dyn_msg* wci2_frame_buffer_rx( uint8 byte )
{
  /* copy into global buffer, when frame complete, return it */
  static wci2_t2_dyn_msg *frame_ptr = NULL; /* tracks frame in progress */
  static boolean          escaped_character = FALSE;
  wci2_t2_dyn_msg        *ret_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  if( frame_ptr == NULL )
  {
    /* wait for a frame start, drop until then */
    if( byte == wci2_frame_data.flags.start_stop )
    {
      /* new frame. get memory for buffer and initialize */
      frame_ptr = WCI2_MEM_ALLOC(sizeof(wci2_t2_dyn_msg));
      WCI2_ASSERT( NULL != frame_ptr );
      frame_ptr->ref_count = 0;
      frame_ptr->len = 0;
      escaped_character = FALSE;
    }
  }
  else
  {
    /* frame in progress -- keep building the frame */
    if( byte == wci2_frame_data.flags.start_stop )
    {
      if( frame_ptr->len != 0 )
      {
        /* found a frame! end of this/start of next. return this. */
        ret_ptr = frame_ptr;

        /* also reset in case that was a start flag */
        frame_ptr = WCI2_MEM_ALLOC(sizeof(wci2_t2_dyn_msg));
        WCI2_ASSERT( NULL != frame_ptr );
        frame_ptr->ref_count = 0;
        frame_ptr->len = 0;
      }
      escaped_character = FALSE;
    }
    else if( byte == wci2_frame_data.flags.esc )
    {
      /* character was escaped. un-escape it: */
      /* skip the escape char and un-mask the following one */
      escaped_character = TRUE;
    }
    else
    {
      if( frame_ptr->len >= WCI2_TYPE2_MAX_FRAME_SIZE )
      {
        /* if buffer overflow imminent, reset */
        WCI2_MSG_1( ERROR, "TYPE2 Frame RX buffer full, resetting. size=%d", 
                    frame_ptr->len );
        frame_ptr->len = 0;
      }
      else if( escaped_character )
      {
        /* previous character was escape character. Need to uncode this one */
        if( wci2_frame_data.mode == WCI2_FRAME_MODE_HDLC )
        {
          frame_ptr->buff[frame_ptr->len++] = byte ^ wci2_frame_data.flags.esc_xor_mask;
        }
        /* SLIP framing uses special characters instead of an escape mask */
        else if( byte == SLIP_ESC_END )
        {
          frame_ptr->buff[frame_ptr->len++] = SLIP_END_FLAG;
        }
        else
        {
          frame_ptr->buff[frame_ptr->len++] = SLIP_ESC_FLAG;
        }
        escaped_character = FALSE;
      }
      else
      {
        /* end not found yet and character normal; store byte in buffer */
        frame_ptr->buff[frame_ptr->len++] = byte;
      }
    }
  }

  return ret_ptr;
}

/*=============================================================================

  FUNCTION:  wci2_frame_unframe

=============================================================================*/
/*!
    @brief
    Given a pointer to a frame, unframe and retreive the message
    inside. Also verify CRC.

    NOTE: this will return a pointer into the original buffer. DO NOT
          modify it and do not free it.

    @detail
    Start/Stop flags and escape characters should already be stripped out


    @return
    void
*/
/*===========================================================================*/
wci2_error_e wci2_frame_unframe( 
  uint8  *in_buff,  /**< Input - buffer containing framed msg */
  uint16  in_size,  /**< Input - size of framed msg */
  uint8 **out_buff, /**< Output- buffer containing unframed msg */
  uint16 *out_size, /**< Output- size of unframed msg */
  uint16 *out_addr  /**< Output- address from frame, IFF HDLC, else ret 0 */
)
{
  uint8        seq_n;
  uint8        chcksum_tst;
  wci2_error_e retval;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( NULL != in_buff  && NULL != out_buff && 
               NULL != out_size && NULL != out_addr );

  if( wci2_frame_data.mode == WCI2_FRAME_MODE_HDLC )
  {
    /* HDLC unframe */
    retval = wci2_hdlc_unframe( in_buff, in_size, 
                                out_buff, out_size, out_addr );
  }
  else
  {
    /* SLIP unframe */
    *out_addr = 0;
    retval = wci2_slip_unframe( in_buff, in_size, out_buff, out_size, &seq_n, 
                                &chcksum_tst );
  }

  WCI2_MSG_4( MED, "Unframe mode=%d in_size=%d out_size=%d retval=%d", 
              wci2_frame_data.mode, in_size, *out_size, retval );

  return retval;
}

/*=============================================================================

                      FUNCTION DECLARATIONS: HDLC

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_hdlc_frame_and_escape

=============================================================================*/
/*!
    @brief
    Frame the given raw byte buffer using HDLC-like framing. To be used for
    transporting messages over UART.

    @detail
    * The maximum message size is defined by WCI2_TYPE2_MAX_MSG_SIZE
    Frame Structure:
      +----------+----------+----------+----------+----------+
      |   Flag   | Address  | Payload  |   CRC    |   Flag   |
      |   0x7e   |    *     |    *     | 16 bits  |   0x7e   |
      +----------+----------+----------+----------+----------+
       - only one flag sequence needed btw two frames
       - CRC transmitted least-significant octet first (highest term coefficients)
       - CRC does not include Flags or CRC fields. i.e. escape after CRC calculation
       - closing flag is used to locate CRC field
       - recommended that transmitters send a new open flag sequence when an 
         appreciable time has elapsed (1sec?)

    @return
    size of frame in bytes
*/
/*===========================================================================*/
wci2_error_e wci2_hdlc_frame_and_escape ( 
  const void *msg,             /**< In - frame payload */
  uint16      msg_size,        /**< In - size of msg payload in bytes */
  uint8       address,         /**< In - address field of frame */
  uint8      *frame_buff,      /**< In/Out - buffer to write frame to */
  uint16      frame_buff_size, /**< In - size of frame buffer in bytes */
  uint16     *frame_size       /**< Out - size of frame after framing */
)
{
  uint8       *in_buff;
  uint16       crc = (uint16) CRC_16_STEP_SEED;
  uint8        out_pos = 0;
  uint8        in_pos = 0;
  uint8        byte_to_write;
  /*-----------------------------------------------------------------------*/
  /* Validate inputs */
  WCI2_ASSERT( NULL != msg && NULL != frame_buff && NULL != frame_size );

  /* treat msg as a byte stream */
  in_buff = (uint8*) msg;

  /* fill in frame header - start flag & address */
  frame_buff[out_pos++] = HDLC_START_FLAG;

  /* compute the CRC (address + msg, before escaping) */
  crc = crc_16_step( crc, &address, 1 );
  crc = crc_16_step( crc, in_buff, msg_size );

  /* write (escaped) address */
  if( HDLC_SHOULD_ESCAPE_BYTE( address ) )
  {
    frame_buff[out_pos++] = HDLC_ESCAPE_FLAG;
    frame_buff[out_pos++] = address ^ HDLC_ESCAPE_XOR_MASK;
  }
  else
  {
    frame_buff[out_pos++] = address;
  }

  /* Copy over message from msg buffer passed in to new buffer. Also
   * escape any reserved bytes if encountered in msg buffer. */
  for( in_pos = 0; in_pos < msg_size; in_pos++ )
  {
    if( out_pos + 7 < frame_buff_size )
    {
      /* test if character needs to be escaped */
      if( HDLC_SHOULD_ESCAPE_BYTE( in_buff[in_pos] ) )
      {
        frame_buff[out_pos++] = HDLC_ESCAPE_FLAG;
        frame_buff[out_pos++] = in_buff[in_pos] ^ HDLC_ESCAPE_XOR_MASK;
      }
      else
      {
        frame_buff[out_pos++] = in_buff[in_pos];
      }
    }
    else
    {
      /* out of space. should not happen if we sized eveything correctly. */
      *frame_size = 0;
      return WCI2_E_OUT_OF_MEM;
    }
  }

  /* write (escaped) LSB of CRC first */
  byte_to_write = (uint8) crc & 0x00ff;
  if( HDLC_SHOULD_ESCAPE_BYTE( byte_to_write ) )
  {
    frame_buff[out_pos++] = HDLC_ESCAPE_FLAG;
    frame_buff[out_pos++] = byte_to_write ^ HDLC_ESCAPE_XOR_MASK;
  }
  else
  {
    frame_buff[out_pos++] = byte_to_write;
  }

  /* now write (escaped) MSB of CRC */
  byte_to_write = (uint8) ((crc & 0xff00) >> 8);
  if( HDLC_SHOULD_ESCAPE_BYTE( byte_to_write ) )
  {
    frame_buff[out_pos++] = HDLC_ESCAPE_FLAG;
    frame_buff[out_pos++] = byte_to_write ^ HDLC_ESCAPE_XOR_MASK;
  }
  else
  {
    frame_buff[out_pos++] = byte_to_write;
  }

  /* append ending flag */
  frame_buff[out_pos++] = HDLC_START_FLAG;
  *frame_size = out_pos;

  return WCI2_E_SUCCESS;
}

/*=============================================================================

  FUNCTION:  wci2_hdlc_unframe

=============================================================================*/
/*!
    @brief
    Take the WCI-2 HDLC-like frame and strip out all the frame info (while
    validating the frame), returning a buffer containing the original raw
    bytestream. If frame is invalid, will silently drop. 
    NOTE: this will return a pointer into the original buffer. DO NOT
          modify it and do not free it.

    @detail
    Start/Stop flags and escape characters should already be stripped out

    @return
    boolean frame_valid
*/
/*===========================================================================*/
wci2_error_e wci2_hdlc_unframe( 
  uint8  *in_buff,  /**< Input - buffer containing framed msg */
  uint16  in_size,  /**< Input - size of framed msg */
  uint8 **out_buff, /**< Output- buffer containing unframed msg */
  uint16 *out_size, /**< Output- size of unframed msg */
  uint16 *out_addr  /**< Output- address from frame */
)
{
  uint16       crc_calcd = CRC_16_SEED;
  uint16       crc_recvd = 0;
  wci2_error_e retval = WCI2_E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  /* validate frame size first. must be at least 3 bytes (address + CRC) */
  WCI2_ASSERT( NULL != in_buff  && NULL != out_buff && 
               NULL != out_size && NULL != out_addr );

  /* initialize return values */
  *out_buff = NULL;
  *out_size = 0;
  *out_addr = 0;

  if( in_size < 3 )
  {
    retval = WCI2_E_FRAME_INVALID;
  }
  else
  {
    /* assume first byte is address and last 2 bytes are the CRC. 
     * The rest are the message. Loop through and make sure the frame
     * is not corrupted (check CRC-16) */
    crc_calcd = crc_16_calc( in_buff, (in_size-2)<<3 );

    /* Compare calculated CRC with the transmitted one */
    crc_recvd = (in_buff[in_size - 1] << 8) | (in_buff[in_size - 2]);
    if( crc_recvd == crc_calcd )
    {
      /* frame error-free! process it */
      /* now that the frame is validated, strip off the frame overhead
       * and pass along the message */
      *out_addr = in_buff[0];
      *out_buff = &in_buff[1];
      *out_size = in_size - 3;
    }
    else
    {
      /* there was an error - CRC didn't check out */
      retval = WCI2_E_FRAME_CRC_MISMATCH;
    }
  }

  return retval;
}

/*=============================================================================

                      FUNCTION DECLARATIONS: SLIP

=============================================================================*/

/*=============================================================================

  FUNCTION:  wci2_slip_frame_and_escape

=============================================================================*/
/*!
    @brief
    Frame the given raw byte buffer using SLIP-like framing. This
    is a standard RFC1055 SLIP, except that a sequence number
    and checksum are first applied. To be used for transporting 
    messages over UART.

    @detail
    * The maximum message size is defined by WCI2_TYPE2_MAX_MSG_SIZE
    Frame Structure:
      +----------+----------+----------+----------+----------+
      |   Flag   | Payload  | Seq_n    | Parity   |   Flag   |
      |   0xc0   |    *     | 8 bits   | 8 bits   |   0xc0   |
      +----------+----------+----------+----------+----------+
       - seq_n is a modulo 256 counter
       - parity checksum: calculated as a cumulative XOR of the packet bytes
       - 0xc0 (end) and 0xdb (esc) are reserved, and will be replaced by, 
         respectively:
          - 0xdb 0xdc
          - 0xdb 0xdd

    @return
    size of frame in bytes
*/
/*===========================================================================*/
wci2_error_e wci2_slip_frame_and_escape ( 
  const void *msg,             /**< In - frame payload */
  uint16      msg_size,        /**< In - size of msg payload in bytes */
  uint8      *frame_buff,      /**< In/Out - buffer to write frame to */
  uint16      frame_buff_size, /**< In - size of frame buffer in bytes */
  uint16     *frame_size       /**< Out - size of frame after framing */
)
{
  static uint8 seq_n = 0;
  uint8       *in_buff;
  uint8        chksum = 0;
  uint8        out_pos = 0;
  uint8        in_pos = 0;
  /*-----------------------------------------------------------------------*/
  /* Validate inputs */
  WCI2_ASSERT( NULL != msg && NULL != frame_buff && NULL != frame_size );

  /* treat msg as a byte stream */
  in_buff = (uint8*) msg;

  /* fill in frame header (just start flag) */
  frame_buff[out_pos++] = SLIP_END_FLAG;

  /* Copy over message from msg buffer passed in to new buffer. Also
   * escape any reserved bytes if encountered in msg buffer. */
  for( in_pos = 0; in_pos < msg_size; in_pos++ )
  {
    /* leave enough room for SLIP overhead = 5:
     *   1) 1 byte for seq_n + 1 in case of esc
     *   2) 1 byte for checksum parity + 1 in case of esc
     *   3) end flag */
    if( out_pos + 5 < frame_buff_size )
    {
      /* test if character needs to be escaped */
      switch( in_buff[in_pos] )
      {
        case SLIP_END_FLAG:
          frame_buff[out_pos++] = SLIP_ESC_FLAG;
          frame_buff[out_pos++] = SLIP_ESC_END;
          break;

        case SLIP_ESC_FLAG:
          frame_buff[out_pos++] = SLIP_ESC_FLAG;
          frame_buff[out_pos++] = SLIP_ESC_ESC;
          break;

        default:
          frame_buff[out_pos++] = in_buff[in_pos];
          break;
      }

      /* update running checksum parity */
      chksum ^= in_buff[in_pos];
    }
    else
    {
      /* out of space. should not happen if we sized eveything correctly. */
      *frame_size = 0;
      return WCI2_E_OUT_OF_MEM;
    }
  }

  /* write (escaped) sequence number and checksum, then increment seq_n 
   * for next message */
  switch( seq_n )
  {
    case SLIP_END_FLAG:
      frame_buff[out_pos++] = SLIP_ESC_FLAG;
      frame_buff[out_pos++] = SLIP_ESC_END;
      break;

    case SLIP_ESC_FLAG:
      frame_buff[out_pos++] = SLIP_ESC_FLAG;
      frame_buff[out_pos++] = SLIP_ESC_ESC;
      break;

    default:
      frame_buff[out_pos++] = seq_n;
      break;
  }

  /* update running checksum parity */
  chksum ^= seq_n++;

  switch( chksum )
  {
    case SLIP_END_FLAG:
      frame_buff[out_pos++] = SLIP_ESC_FLAG;
      frame_buff[out_pos++] = SLIP_ESC_END;
      break;

    case SLIP_ESC_FLAG:
      frame_buff[out_pos++] = SLIP_ESC_FLAG;
      frame_buff[out_pos++] = SLIP_ESC_ESC;
      break;

    default:
      frame_buff[out_pos++] = chksum;
      break;
  }

  /* append ending flag */
  frame_buff[out_pos++] = SLIP_END_FLAG;
  *frame_size = out_pos;

  return WCI2_E_SUCCESS;
}

/*=============================================================================

  FUNCTION:  wci2_slip_unframe

=============================================================================*/
/*!
    @brief
    Take the SLIP-like frame and strip out all the frame overhead (while
    validating the frame), returning a buffer containing the original raw
    bytestream. If frame is invalid, will return a non-zero checksum_tst
    NOTE: this will return a pointer into the original buffer. DO NOT
          modify it and do not free it.

    @detail
    Start/Stop flags and escape characters should already be stripped out

    @return
    boolean frame_valid
*/
/*===========================================================================*/
wci2_error_e wci2_slip_unframe( 
  uint8  *in_buff,   /**< Input - buffer containing framed msg */
  uint16  in_size,   /**< Input - size of framed msg */
  uint8 **out_buff,  /**< Output- buffer containing unframed msg */
  uint16 *out_size,  /**< Output- size of unframed msg */
  uint8  *seq_n,     /**< Output- sequence number */
  uint8  *chksum_tst /**< Output- checksum_tst */
)
{
  uint16       i;
  wci2_error_e retval = WCI2_E_SUCCESS;
  /*-----------------------------------------------------------------------*/
  WCI2_ASSERT( NULL != in_buff && NULL != out_buff && NULL != out_size && 
               NULL != seq_n   && NULL != chksum_tst );
  if( in_size < SLIP_FRAME_OVERHEAD || in_size > WCI2_TYPE2_MAX_FRAME_SIZE )
  {
    WCI2_MSG_1( ERROR, "SLIP unframe failed, size=%d", in_size );
    retval = WCI2_E_FRAME_INVALID;
  }
  else
  {
    /* remove framing overhead. nothing in front, just 2B in back */
    *out_buff = in_buff;
    *out_size = in_size - SLIP_FRAME_OVERHEAD;

    /* sequence number is second to last in frame */
    *seq_n = in_buff[in_size - SLIP_FRAME_OVERHEAD];

    /* run checksum computation over entire frame, should be 0 if valid */
    *chksum_tst = 0;
    for( i = 0; i < in_size; i++ )
    {
      *chksum_tst ^= in_buff[i];
    }
  }

  return retval;
}


