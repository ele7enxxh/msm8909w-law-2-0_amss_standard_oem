/***********************************************************************
 * FEATURE_BABYLON
 *
 * DO NOT INCLUDE THIS HEADER FILE
 *
 * Babylon internal functions.  Both the functions and this header
 * file are NOT INTENDED for direct consumption.
 *
 * This header file is a bit unusual.  It either defines prototypes
 * for these functions, OR defines inline void declarations that get
 * compiled away.
 * 
 * Copyright (C) 2006-2009 Qualcomm Technologies Incorporated. All Rights Reserved.
 * Qualcomm Confidential and Proprietary.
 *
 ***********************************************************************/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/bab_internal.h#1 $

YYYY-MM-DD    who     what, where, why
----------    ---     ----------------------------------------------------------
2009-05-19    sh      Adapted for gcc __inline__
2009-04-27    jm      Use __inline instead of inline
2008-01-23    sh      Preparing for release.
2008-10-27    rh      Adding support for Windows Mobile Targets.
2006-01-06    sh      Created module from Alan C Wong's project.

===========================================================================*/

/**
 * Writes one packet out to the Babylon port.
 *
 * This is called from each of the public packet-sending functions to
 * do the grunt work of putting a message on the wire.
 * 
 * After every 'BAB_SYNC_PACKET_PERIOD' packets, a sync packet is
 * automatically sent out by this function as well.
 *
 * @param type_stream
 * Packed combination of the Message Type and Stream ID
 *
 * @param eid_id
 * Packed combination of the ID and Extended ID
 *
 * @param payload
 * Payload word (16-bits or 32-bits)
 * 
 * @param paload_size
 * Indicator of payload size
 */
#ifdef FEATURE_BABYLON
void _bab_write_packet (uint16 type_stream, uint16 eid_id,
                        uint32 payload, uint16 payload_size);
#else
static _INLINE_
void _bab_write_packet (uint16 type_stream, uint16 eid_id,
                        uint32 payload, uint16 payload_size)
{
  (void) type_stream;
  (void) eid_id;
  (void) payload;
  (void) payload_size;
}
#endif

/**
 * This is the private function that gets called by bab_port_test.
 * This function sends out INVALID packets for the purpose
 *  of testing physical connectivity to the Bab port.
 *  These are not valid BABYLON words, just recognizeable
 * patterns to ensure the bits are connected.
 */
#ifdef FEATURE_BABYLON
void _bab_port_test (void);
#else
static _INLINE_
void _bab_port_test (void) {}
#endif

/**
 * This function is used to initialize memory mapping or address
 * pointers for the hardware DEBUG_LED/FPGA_LED port.
 * 
 * Must be called (once) prior to any Babylon use.
 */
#ifdef FEATURE_BABYLON
void _bab_port_init (void);
#else
static _INLINE_
void _bab_port_init (void) {}
#endif
