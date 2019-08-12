#ifndef _MVS_VOC_DEF_H_
#define _MVS_VOC_DEF_H_

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S

DESCRIPTION
  This header file defines function interfaces and corresponding data
  structures for invoking multi-mode voice services.

REFERENCES
  None.

Copyright (C) 2009-2011, 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/inc/mvs_voc_def.h#1 $
$Author: mplcsds1 $

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include <stddef.h>
#include "comdef.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

// Vocoder packet length (maximum) ( g711 pcm 322)
#define VOICE_PACKET_LENGTH 322

// Length of voice packet header in bytes
#define VOICE_PACKET_HDR_LENGTH 1

// Length of CDMA voice packet header in bytes
#define CDMA_VOICE_PACKET_HDR_LENGTH 2

// Length (bytes) of EFR/FR/HR packet core speech data
#define EFR_MAX_PACKET_DATA_LENGTH 31
#define FR_MAX_PACKET_DATA_LENGTH 33
#define HR_MAX_PACKET_DATA_LENGTH 14

//Logging
#define LEGACY_GSM_PACKET_LENGTH  160  /* GSM packet length in bytes*/
#define LEGACY_GSM_PACKET_LENGTH_WORD 80  /* GSM packet length in words*/

#define LEGACY_CDMA_PACKET_LENGTH  36  /* CDMA packet length in bytes */

// Length (bytes) of EVRC packet core speech data
#define EVRC_MAX_PACKET_DATA_LENGTH 22

// Length (bytes) of 4GV packet core speech data
#define FOURGV_MAX_PACKET_DATA_LENGTH 22

// Length (bytes) of 13k packet core speech data
#define IS733_MAX_PACKET_DATA_LENGTH 34

#define LINEAR_PCM_MAX_PACKET_DATA_LENGTH 320  /* 320 bytes per PCM packet */

#define G711_MAX_PACKET_DATA_LENGTH 160        /* 160 bytes per G711 packet */

#define G711_MAX_SG_DATA_LENGTH 80        /* 80 bytes g711 speed good packet*/

#define G729A_MAX_FRAME_DATA_LENGTH 10    /*10 bytes per G729 frame */

#define G729A_MAX_PACKET_DATA_LENGTH 20  /*20 bytes per G729 packet (2 frames)*/

#define G722_MAX_PACKET_DATA_LENGTH 160        /* 160 bytes per G722 packet */

#define TIMEWARP_PCM_FRAME_LENGTH_NB 160 /* used for timewarping for narrowband vocoders. */

#define TIMEWARP_PCM_FRAME_LENGTH_WB 320 /* used for timewarping for wideband vocoders.*/

/* Internal AMRNB frame type definitions as per the 3GPP, also referenced
 * in DCN 80-N8828-1.
 *
 * These are use to map the frame type between  modem and ADSP, both
 * over Uplink and Downlink.
 */
typedef enum {
  CVD_AMR_SPEECH_GOOD,      /* Good speech frame. */
  CVD_AMR_SPEECH_DEGRADED,  /* Speech degraded. */
  CVD_AMR_ONSET,            /* onset */
  CVD_AMR_SPEECH_BAD,       /* Corrupt speech frame (bad CRC) */
  CVD_AMR_SID_FIRST,        /* First silence descriptor */
  CVD_AMR_SID_UPDATE,       /* Comfort noise frame */
  CVD_AMR_SID_BAD,          /* Corrupt SID frame (bad CRC) */
  CVD_AMR_NO_DATA,          /* Nothing to transmit */
  CVD_AMR_FRAME_TYPE_MAX
} cvd_amr_frame_type;

/* Internal AMRWB frame type definitions as per the 3GPP, also referenced
 * in DCN 80-N8828-1.
 *
 * These are use to map the frame type between  modem and ADSP, both
 * over Uplink and Downlink.
 */
typedef enum {
  CVD_AMRWB_SPEECH_GOOD,              /* Good speech frame. */
  CVD_AMRWB_SPEECH_PROBABLY_DEGRADED, /* Speech degraded. */
  CVD_AMRWB_SPEECH_LOST,              /* Speech lost */
  CVD_AMRWB_SPEECH_BAD,               /* Corrupt speech frame (bad CRC) */
  CVD_AMRWB_SID_FIRST,                /* First silence descriptor */
  CVD_AMRWB_SID_UPDATE,               /* Comfort noise frame */
  CVD_AMRWB_SID_BAD,                  /* Corrupt SID frame (bad CRC) */
  CVD_AMRWB_NO_DATA,                  /* Nothing to transmit */
  CVD_AMRWB_FRAME_TYPE_MAX
} cvd_amrwb_frame_type;

#endif /* _MVS_VOC_DEF_H_ */

