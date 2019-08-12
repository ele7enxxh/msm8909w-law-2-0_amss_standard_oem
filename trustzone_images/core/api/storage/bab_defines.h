/***********************************************************************
 * FEATURE_BABYLON
 * bab_defines.h
 *
 * Common declarations for Babylon.
 *
 * NOTE: This file is included both C and Assembly code (.s), so it
 * must contain preprocessor instructions ONLY, and no C constructs!
 * 
 * Copyright (C)2006, 2008-2010 by Qualcomm Technologies Incorporated.
 *
 ***********************************************************************/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/bab_defines.h#1 $

YYYY-MM-DD       who     what, where, why
-------------    ---     ----------------------------------------------------------
2010-03-10       rh      Reduced message type to 4-bits - Expand stream ID to 9 bits
2009-01-15       sh      Reduced message type to 5-bits
2008-10-30       sh      Added discrete start/end marker types
2006-01-06       sh      Created module from Alan C Wong's project.

===========================================================================*/

#ifndef __BAB_DEFINES_H__
#define __BAB_DEFINES_H__

/*
 * This sets the frequency of the sync packets -- one is sent out
 * after every BAB_SYNC_PACKET_PERIOD packets.  The value is somewhat
 * arbitrary.
 *
 * If set too high, then too few sync packets may be present in the
 * data stream to facilitate synching up to a garbled, edited, or
 * "midstream" capture.  The decoder has to wait until the next sync
 * appears to begin.
 *
 * If set too low, then too many unnecessary sync packets will be
 * sent, adding overhead traffic and execution time.
 *
 * For very reliable links, this number can be increased.  For noisy
 * links or frequent interrupted messages, a decreased period would be
 * appropriate.
 */
#define BAB_SYNC_PACKET_PERIOD               0x100

/*
 * This is the magic sync word, sent FOUR times as a sync packet
 * Do not change.
 */
#define BAB_SYNCHRONIZATION_PAYLOAD          0xCCCC


/* Packet structure as bits:
   First word: pp0Stttt ssssssss
   Second word: eeeeeeee iiiiiiii
   
   p: Payload size (2 bits)
   0: Reserved.  Must be 0
   t: Message type (4 bits)
   S: Upper bit of Stream ID (1 bit)
   s: Stream ID (8 bits)
   e: Extended ID (8 bits)
   i: ID (8 bits)
   
   Third word: Optional <16-bit or low word of 32-bit payload>
   Fourth word: Optional <top word of 32-bit payload>
*/

/* Defined packet types */
#define BAB_TYPE_MASK                        0x0F /* to mask out the payload flags */
#define BAB_TYPE_VALUE                       0x01
/* Value 0x02 reserved - Reducing types down to 3 bit field in the future, do not add any
 * new type value */
/* Value 0x03 reserved */
#define BAB_TYPE_SINGLE_EVENT                0x04
/* Value 0x05 reserved */
#define BAB_TYPE_TEXT_MESSAGE                0x06
#define BAB_TYPE_RETURN_VALUE                0x09
#define BAB_TYPE_MARKER_START                0x0A
#define BAB_TYPE_MARKER_END                  0x0B
#define BAB_TYPE_INVALID                     0x0C /* Reserved for sync field */
/* Value 0x0D reserved */
/* Value 0x0E reserved */
#define BAB_TYPE_RESERVED                    0x0F

/* Defined payload sizes */
#define BAB_PAYLOAD_MASK     0xC000 /* to isolate the top 2 bits */
#define BAB_PAYLOAD_NONE     0x0000 /* 00 2-word message */
#define BAB_PAYLOAD_16BIT    0x4000 /* 01 3-word message for EBI bus; 4-word for SD bus */
#define BAB_PAYLOAD_32BIT    0x8000 /* 10 4-word message */
#define BAB_PAYLOAD_INVALID  0xC000 /* 11 Not valid / Sync */

/* Specifics flags pertaining to the value display */
#define BAB_VALUE_FORMAT_DECIMAL 0x01 /* Decimal (base 10); zero for Hex */
#define BAB_VALUE_FORMAT_SIGNED  0x02 /* Signed; zero for unsigned */
#define BAB_VALUE_FORMAT_32BIT   0x04 /* 32-bit; zero for 16-bit */

/* Extended IDs for use with the deprecated MARKER type */
#define BAB_MARKER_BEGIN          0x01
#define BAB_MARKER_END            0x00

#endif /* __BAB_DEFINES_H__ */
