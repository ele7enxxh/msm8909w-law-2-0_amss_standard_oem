#ifndef HDRUTIL_H
#define HDRUTIL_H

/*===========================================================================

                          H D R  U T I L I T I E S
                           D E F I N I T I O N S

DESCRIPTION
  This module contains declarations for generic utilites used by many 
  different HDR modules.

Copyright (c) 2000 - 2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrutil.h_v   1.11   05 Jun 2002 13:12:56   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrutil.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/12/15   sat     Removing occurences of chipset name tags
05/09/14   vko     Support to handle bad NV read gracefully
08/27/13   cnx     Remove unsupported channel in hash before session negotiation.
08/01/13   smd     Used new secapi to get random number.
04/17/12   cnx     Support rfm_is_band_chan_supported_v2().
09/16/11   wsh     Reduce heap mem usage by moving memory back to static
07/27/11   cnx     Supported byte stream to dsm and vice vesa.
04/15/11   cnx     Removed hdrutil_band_lists if RF_HAS_RFM_BAND_CHAN_V2_API defined.
10/21/10   wsh     Disabled MEM_CHK_LOG_TASK_SWITCH to avoid warning msg
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
11/10/08   smd     Replaced errno.h with IxErrno.h  
05/09/08   sju     Added support to remove avoided channels from NSET
07/30/07   hal     Added hdrutil_pushdown()
07/13/07   sju     Removed hdrutil_mode_num_to_enum()
06/18/07   hal     Added hdrutil_chan_is_in_band_with_all_subclasses()
03/22/07   hal     Added F3 messages to trace DSM calls (FEATURE_HDRUTIL_DEBUG)
01/08/07   etv     Added hdrutil_bit_count32().
11/30/06   hal     Added hdrutil_pretty_print_byte_array for F3 debugging
09/08/06   hal     Added hdrutil_new_dsm_buffer() for DSM buffers without offsets
07/31/06   hal     Moved HDR message item pool to UTIL
05/31/06   hal     Added hdrutil_chan_is_in_band()
                   Added hdrutil_get_band_all_sc_mask()
                   Added hdrutil_band_has_subclasses()
04/28/06   etv     Added hdrutil_write_nv_for_hdrmc.
04/07/06   hal     Added hdrutil_new_msg_buffer()
                   Added hdrutil_pushdown_head()
                   Added hdrutil_pushdown_tail()
11/21/05   hal     Added hdrutil_unpack_channel_record()
10/31/05   hal     Added hdrutil_bit_count16()
07/07/05   dna     Added support for Enhanced Idle State protocol
12/07/04   hal     Added hdrutil_new_buffer wrapper function for dsm_offset_new_buffer.
11/29/04   grl     Added time duration calcing func to support logging.
11/08/04   pba     Added hdrutil_remove_invalid_neighbors()
11/03/04   mpa     Removed dependency on std.h (merged with sys.h)
06/05/02   dna     Queue NV writes and write the items one at a time.
05/22/01   nmn     added send_empty_config_rsp
03/19/01   vas     Removed hash function & band num to enum conversion.
01/18/01   nmn     Added NV read/write functions
06/27/00   vas     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "nv.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrhai.h"
#include "dsm.h"
#include "hdrsrchtypes.h"
#include "time_svc.h"
#include "hdrerrno.h"
#include "rex.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/* <EJECT> */ 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define hdrutil_new_msg_buffer() \
  hdrutil_new_msg_buffer_ext( (void *) __FILENAME__, (void *) __LINE__ )

#define hdrutil_new_dsm_buffer() \
  hdrutil_new_dsm_buffer_ext( (void *) __FILENAME__, (void *) __LINE__ )

#define hdrutil_pushdown( item_ptr, data_ptr, data_size ) \
  hdrutil_pushdown_ext( (item_ptr), (data_ptr), (data_size), \
                        (void *) __FILENAME__, (void *) __LINE__ )

#define hdrutil_pushdown_head( item_ptr, data_ptr, data_size ) \
  hdrutil_pushdown_head_ext( (item_ptr), (data_ptr), (data_size), \
                             (void *) __FILENAME__, (void *) __LINE__ )

#define hdrutil_pushdown_tail( item_ptr, data_ptr, data_size ) \
  hdrutil_pushdown_tail_ext( (item_ptr), (data_ptr), (data_size), \
                             (void *) __FILENAME__, (void *) __LINE__ )

/*=============================================================================
FUNCTION HDRUTIL_MSG_SEQ_NUM_IS_GREATER

DESCRIPTION
  This function validates a received message sequence number according to 
  the rules specified in the HAI.

DEPENDENCIES
  None.

PARAMETERS
  new_seq_num - New Sequence munber
  old_seq_num - Old (previously received) Sequence number
  seq_num_bits - Number of bits in the Sequence number

RETURN VALUE
  TRUE  - If the new message sequence number is valid.
  FALSE - Otherwise

SIDE EFFECTS
  None.
=============================================================================*/
boolean hdrutil_msg_seq_num_is_greater
( 
  uint8 new_seq_num, 
  uint8 old_seq_num, 
  uint8 seq_num_bits 
);

/*===========================================================================

FUNCTION hdrutil_hdr_x4chan_to_cdma

DESCRIPTION
  Map HDR Rev. X4 channel to CDMA band-class and channel.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if HDR Rev. X4 is mapped to a supported CDMA band-class and channel.
  FALSE otherwise.

  The CDMA band class and channel are returned via the rtrn_cdma_band_ptr and
  rtrn_cdma_chan_ptr pointers, respectively.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                   hdrutil_hdr_x4chan_to_cdma(

        uint32                    hdr_chan,
            /* HDR Rev. X4 channel which to convert.
            */
        sys_band_class_e_type            *rtrn_cdma_band_ptr,
            /* Pointer to buffer where to store returned CDMA band-calss.
            */

        sys_channel_num_type            *rtrn_cdma_chan_ptr
            /* Pointer to buffer where to store returned CDMA channel.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION hdrutil_cdma_to_hdr_x4chan

DESCRIPTION
  Map CDMA band-class and channel to HDR Rev. X4 channel.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if CDMA band-class and channel are mapped to a supported HDR Rev. X4
  channel. FALSE otherwise.
  
  The HDR X4 channel is returned via the rtrn_hdr_chan.

SIDE EFFECTS
  None.

===========================================================================*/
boolean                   hdrutil_cdma_to_hdr_x4chan(

        sys_band_class_e_type            cdma_band,
            /* CDMA band class which to convert.
            */

        sys_channel_num_type              cdma_chan,
            /* CDMA channel which to convert.
            */
        
        uint32                    *rtrn_hdr_chan_ptr
            /* Pointer to buffer where to store returned HDR X4 channel.
            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_WRITE_NV_ITEM

DESCRIPTION
  This function writes an item to NV

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  ERR_FATAL if the command return is not DONE

===========================================================================*/
void hdrutil_write_nv_item
(
  nv_items_enum_type item_code, /* item to write */
  nv_item_type * item_ptr /* pointer to where to get the item */
);


/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_WRITE_NV_FOR_HDRMC

DESCRIPTION
  This function could be used to write an NV item in hdrmc task context.

  Note: This function call would block hdrmc task until the NV write is 
  complete. 
  
DEPENDENCIES
  None.

ASSUMPTIONS
  1. This function assumes that the calling function would fill-in the NV item
     name and the data_ptr in the NV command. 
  2. This function would be called only from HDRMC task context.
  
RETURN VALUE
  None

SIDE EFFECTS
  The call to this function blocks until the NV write is complete.
  This function would result in a FATAL error if the NV write fails.

===========================================================================*/
LOCAL void hdrutil_write_nv_for_hdrmc
(
  nv_cmd_type *cmd
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_READ_NV_ITEM

DESCRIPTION
  This function reads an item from NV

DEPENDENCIES
  None.

RETURN VALUE
  NV command return value

SIDE EFFECTS
  ERR_FATAL if the command return is not DONE or NOTACTIVE

===========================================================================*/
nv_stat_enum_type hdrutil_read_nv_item
(
  nv_items_enum_type item_code, /* item to read */
  nv_item_type * item_ptr /* pointer to where to put the item */
);

/* <EJECT> */
/*===========================================================================

FUNCTION HDRUTIL_SEND_EMPTY_CONFIG_RSP

DESCRIPTION
  This function delivers an empty Configuration Response Message
    on behalf of the caller.

DEPENDENCIES
  None.

PARAMETERS
  caller - name of the protocol sending the empty config response message
  transaction_id - transaction_id to use for response

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrutil_send_empty_config_rsp
(
  hdrhai_protocol_name_enum_type caller,
  uint8 transaction_id
);

/*===========================================================================

FUNCTION HDRUTIL_POWERUP_INIT

DESCRIPTION
  This function performs powerup initialization for the hdrutil module.

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void hdrutil_powerup_init( void );

/*===========================================================================

FUNCTION HDRUTIL_NV_Q_SRV

DESCRIPTION
  This function services the NV queue to write the next item.

DEPENDENCIES
  None.

RETURN VALUE
  True if an NV write is in progress as a result of this call
  
SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrutil_nv_q_srv ( void );

/*===========================================================================

FUNCTION HDRUTIL_IS_BAND_SUPPORTED

DESCRIPTION
  This function checks if a band is supported based on hardware configuration
  and user preference.
  
DEPENDENCIES
  None.

PARAMETERS
  channel - the band class to be checked.

RETURN VALUE
  TRUE if the band is supported.
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrutil_is_band_supported
(
  sys_band_class_e_type band
);

/*===========================================================================

FUNCTION HDRUTIL_REMOVE_INVALID_NEIGHBORS

DESCRIPTION
This function scans through the neighbor list and removes all entries
corresponding to band class not supported by AT

DEPENDENCIES
None

PARAMETERS
  nset_ptr - record of neighbor set pilots, from which invalid band class
             neighbors are to be removed
  ncnt - count of number of neighbors
  avoided_chan_ptr - Pointer to avoided channel list 
  num_avoided_channels - Number of channels currently avoided by SD

RETURN VALUE
TRUE if any (invalid) neighbor entry is removed, FALSE otherwise

SIDE EFFECTS
None

===========================================================================*/
boolean hdrutil_remove_invalid_neighbors
( 
  hdrsrch_nset_pilot_type *nset_ptr,
  uint8 *ncnt,
  const sys_avoided_channel_record_type *avoided_chan_ptr,       
  const uint8 num_avoided_channels
);

/*=========================================================================

FUNCTION HDRUTIL_CALC_DURATION

DESCRIPTION  
  Calculate the duration between two time stamps.
               
DEPENDENCIES 
  None

RETURN VALUE 
  The duration in ms between the end and start time.

SIDE EFFECTS 
  None

=========================================================================*/

uint64 hdrutil_calc_duration 
(
  PACKED time_type start_time,
  PACKED time_type end_time
);

/*=========================================================================

FUNCTION HDRUTIL_NEW_BUFFER

DESCRIPTION  
  Wrapper for the dsm_offset_new_buffer function to exit gracefully in
  the event of a NULL item being returned.
               
DEPENDENCIES 
  None

RETURN VALUE 
  Either the buffer from dsm_offset_new_buffer or NULL if there are no
  buffers remaining.

SIDE EFFECTS 
  None

=========================================================================*/
dsm_item_type *hdrutil_new_buffer
(
  dsm_mempool_id_enum_type pool_id,
  uint16 offset
);

/*===========================================================================

FUNCTION HDRUTIL_NEW_MSG_BUFFER

DESCRIPTION
  This function is a wrapper for dsm_offset_new_buffer. The function
  takes no parameters and passes the default DSM_HDR_MSG_ITEM_POOL for the
  pool id and HDRHMP_HEADER_SIZE for the offset parameters.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  A pointer to the buffer item or nothing if the pool is empty.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type *hdrutil_new_msg_buffer_ext
(
  void *file,
  void *line
);

/*===========================================================================

FUNCTION HDRUTIL_NEW_DSM_BUFFER

DESCRIPTION
  This function allocates a new DSM item from the HDR messaging pool.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  A pointer to the buffer item. If the pool is empty, the software will
  halt and throw an ERR_FATAL.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type *hdrutil_new_dsm_buffer_ext
(
  void *file,
  void *line
);

/*===========================================================================

FUNCTION HDRUTIL_PUSHDOWN_HEAD

DESCRIPTION
  <enter description here>

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrutil_pushdown_head_ext
(
  dsm_item_type **dsm_item_ptr,
  void *data_ptr,
  uint16 data_size,
  void *file,
  void *line
);

/*===========================================================================

FUNCTION HDRUTIL_PUSHDOWN_EXT

DESCRIPTION
  <enter description here>

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrutil_pushdown_ext
(
  dsm_item_type **dsm_item_ptr,
  void *data_ptr,
  uint16 data_size,
  void *file,
  void *line
);

/*===========================================================================

FUNCTION HDRUTIL_PUSHDOWN_TAIL

DESCRIPTION
  <enter description here>

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
uint16 hdrutil_pushdown_tail_ext
(
  dsm_item_type **dsm_item_ptr,
  void *data_ptr,
  uint16 data_size,
  void *file,
  void *line
);

/*===========================================================================

FUNCTION HDRUTIL_BYTE_TO_DSM

DESCRIPTION
  This function converts a byte stream to a packet (dsm item).
  
DEPENDENCIES
  None.

PARAMETERS
  buf - pointer to input buffer
  buf_len - length of input buffer

RETURN VALUE 
  pointer to the packet.

SIDE EFFECTS
  None.

===========================================================================*/
dsm_item_type * hdrutil_byte_to_dsm
( 
  uint8 *buf,
  uint32 buf_len 
);

/*===========================================================================

FUNCTION HDRUTIL_DSM_TO_BYTE

DESCRIPTION
  This function converts a packet (dsm item) to byte stream.
  
DEPENDENCIES
  None.

PARAMETERS
  dsm_item_ptr - pointer to pointer of the input packet to convert
  buf - pointer to output buffer
  max_buf_len - maximum length of the buffer 
  buf_len - pointer to the size of output buffer

RETURN VALUE 
  E_SUCCESS - convert successfully
  E_NO_MEMORY - maximum allowed buffer size is smaller than the packet size

SIDE EFFECTS
  None.

===========================================================================*/
errno_enum_type hdrutil_dsm_to_byte
( 
  dsm_item_type **dsm_item_ptr,
  uint8 *buf,
  uint32 max_buf_len, 
  uint32 *buf_len
);

/*============================================================================

FUNCTION HDRUTIL_SCI_TO_NS

DESCRIPTION
  This function converts a Slot Cycle Index to a sleep duration in 
  nanoseconds.

DEPENDENCIES
  None

PARAMETERS
  sci - The Slot Cycle Index to convert to nanoseconds (range of 0 - 31)

RETURN VALUE
  Number of nanoseconds for the given slot cycle index, or zero if the given
  SCI is out of range.

SIDE EFFECTS
  None

============================================================================*/
uint64 hdrutil_sci_to_ns
(
  uint8 sci
);

/*===========================================================================

FUNCTION HDRUTIL_BIT_COUNT16

DESCRIPTION
  This function counts the number of 1 bits in a 16 bit field.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  The number of 1 bits.

SIDE EFFECTS
  None.

===========================================================================*/
extern int hdrutil_bit_count16
(
  uint16 bitfield
);

/*===========================================================================

FUNCTION HDRUTIL_UNPACK_CHANNEL_RECORD

DESCRIPTION
  This function unpacks a channel record from a given message.

DEPENDENCIES
  None.

PARAMETERS
  item_ptr         - The message containing the channel record.
  offset_ptr       - The offset to begin unpacking the channel record.
  sys_type_ptr     - A pointer to hold the system type of the unpacked
                     channel record.
  channel_list_ptr - A pointer to the structure that holds the extracted 
                     channel record.

RETURN VALUE
  E_SUCCESS if the extraction is successful; non-zero if the extraction
  failed.

SIDE EFFECTS
  The offset_ptr will be updated to the end of the channel record block.

===========================================================================*/
extern errno_enum_type hdrutil_unpack_channel_record
(
  dsm_item_type    *const item_ptr,
  uint16           *const offset_ptr,
  uint8            *const sys_type_ptr,
  sys_channel_type *const channel_list_ptr
);

/*===========================================================================

FUNCTION HDRUTIL_PRETTY_PRINT_BYTE_ARRAY

DESCRIPTION
  This function pretty prints a byte array as an F3 message.

DEPENDENCIES
  None.

PARAMETERS
  byte_array - The array holding the bytes to be printed.
  size - The size of the array holding the bytes to be printed.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrutil_pretty_print_byte_array
(
  void *msg_ptr,
  uint8 *byte_array,
  uint32 size
);

/*===========================================================================

FUNCTION HDRUTIL_BIT_COUNT32

DESCRIPTION
  This function counts the number of bits set in an unsigned int.

DEPENDENCIES
  None.

PARAMETERS
  bitfield - The unsigned int

RETURN VALUE
  The number of set bits in the 32 bit field.

SIDE EFFECTS
  None.

===========================================================================*/
extern uint32 hdrutil_bit_count32
(
  uint32 bitfield
);
/*===========================================================================

FUNCTION HDRUTIL_GET_RANDOM()

DESCRIPTION
  This function returns a random number. This is a wrap up function for
  secapi_get_random.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
dword  hdrutil_get_random ( void );

/*===========================================================================

FUNCTION HDRUTIL_RESET_NV_ERROR

DESCRIPTION
  This function rests the NV error flag. Used for tracking if there is 
  any NV error during a specific operation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrutil_reset_nv_error(void);

/*===========================================================================

FUNCTION HDRUTIL_GET_NV_ERROR

DESCRIPTION
  This function retrieves the NV error flag. Used for tracking if there is 
  any NV error during a specific operation.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - if there is any NV error from the last time it is reset.
  FALSE - otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrutil_get_nv_error(void);

#endif /* HDRUTIL_H */
