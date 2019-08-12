/***********************************************************************
 * FEATURE_BABYLON
 * bab_streams.h
 *
 * Babylon stream definitions.  Public.
 * 
 * Copyright (C) 2006, 2008-2010 by Qualcomm Technologies, Incorporated.
 *
 ***********************************************************************/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/storage/bab_streams.h#1 $

YYYY-MM-DD       who     what, where, why
--------         ---     ----------------------------------------------------------
2010-04-22       rh      Add stream ID for IODEVICES
2010-03-10       rh      Expand stream ID to 9 bits
2010-01-14       rh      Adding stream IDs for DALTF and HW ENGINES
2008-11-06       sh      Added a stream ID for the MTP client.
2006-01-06       sh      Created module from Alan C Wong's project.

===========================================================================*/

#ifndef __BAB_STREAMS_H__
#define __BAB_STREAMS_H__

/*************************************************************************
 * Global definitions of common STREAM_ID's to use with the Babylon
 * protocol functions.
 *
 * The use of a STREAM_ID allows a developer to filter messages as
 * needed, and to locally assign meaning to the different sub-IDs.
 * The intent is that a STREAM_ID belongs to a technology or team.
 * The STREAM_IDs are a global and limited resource, so some
 * cooperation is required in assigning and preserving them.
 * 
 * Note that the first 255 IDs (from 0x00 to 0xEF, inclusive) are
 * reserved for global assignment and will be allocated to QCOM
 * technology groups for system-wide use.  Please do not add new
 * values to this range without coordination.
 *
 * For now, the values from 0xF0 to 0xFE can be assigned on an OEM,
 * scratch or temporary basis without interfering.
 *
 * The values from 0x1000 to 0x10FE can be used for future
 * expension - Modification to API is required to make this work
 * 
 * The last value, 0xFF (i.e. 0b1111 1111) is pre-allocated for
 * protocol expansion use.
 *
 *************************************************************************/

#define BAB_STREAM_REX_INFO                0x00 /* hardcoded in rexarm.s */ 
#define BAB_STREAM_EFS                     0x01 /* EFS API and filesystem */
#define BAB_STREAM_FAT                     0x02 /* FAT filesystem */
#define BAB_STREAM_SDCC                    0x03 /* SecureDigital Card Ctrl */
#define BAB_STREAM_USB                     0x04 /* USB driver */
#define BAB_STREAM_HDR                     0x05 /* High Data Rate modem */
#define BAB_STREAM_FLASH                   0x06 /* Flash driver */
#define BAB_STREAM_MTP                     0x07 /* Music Transfer Protocol */
#define BAB_STREAM_BOOT                    0x08 /* Boot-time events */
#define BAB_STREAM_HWENGINES               0x09 /* Hardware Engine events */
#define BAB_STREAM_ISR                     0x0A /* Interrupts */
#define BAB_STREAM_BREW                    0x0B /* BREW */
#define BAB_STREAM_CS                      0x0C /* Component Services */
#define BAB_STREAM_DALTF                   0x0D /* Dal Test Framework events */
#define BAB_STREAM_WMWC                    0x0E /* Windows Mobile Wired Connectivity */ 
                                                /*  USB, UART, SIO, SIOUSB, SIOUSBCONN */
#define BAB_STREAM_IODEVICES					 0x0F /* Keypad, Touchscreen and Headset */

/* Values through 0xFE are available to QCOM */

#define BAB_STREAM_RESERVED                0xFF /* Do not use */

/* Values through 0x3E are available */

#define BAB_STREAM_EXTENDED                0x10FF /* (Backdoor for growth) */

#endif /* __BAB_STREAMS_H__ */
