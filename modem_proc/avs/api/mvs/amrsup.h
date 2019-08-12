#ifndef __AMRSUP_H__
#define __AMRSUP_H__

/*===========================================================================

                    A M R   F R A M I N G   P R O C E S S I N G
                    S U P P L E M E N T A L   F U N C T I O N S

DESCRIPTION
  This header file contains definitions used by the AMR framing functions.

REFERENCES
  3G TS 26.093 - "AMR Speech Codec; Source Controlled Rate Operation"
  3G TS 26.101 - "AMR Speech Codec; Frame structure"

Copyright (C) 2003-2009, 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/mvs/amrsup.h#1 $
$Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sud     Updated Copyright.
08/18/09    sr     Adaptation to CVD.
06/16/09    at     Added amrsup_set_default_dl_amr_mode().
06/01/09    at     Added AMR_WB support.
05/11/09    sr     Changed amrsup_audfmt_de_framing() return type .
06/02/08    at     Added amrsup_get_frame_type() and amrsup_get_frame_mode()
08/30/08    at     Added some new functions and data types for AMR IF1 support
08/09/07   ash     Moved from P4 /asic/msmshared/openmm/com/
02/26/07   kan     Added AMR-WB changes
07/22/05   ymc     Added FEATURE_AMR_IF1 support.
           ymc     Updated Copyright.
08/18/04   ymc     Added crc_err in amrsup_de_framing.
           ymc     Updated Copyright.
           ymc     Added amrsup_move_valid_bits, amrsup_get_frame_type,
                   amrsup_frame_class_bits, amrsup_frame_len_bits.
06/29/03   ymc     Added amrsup_frame_len.
           ymc     Added frame length return for amrsup_if2_framing and
                   amrsup_if2_de_framing.
04/17/03    sm     Initial revision taken from mvsamr.h.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* AMR frame type definitions */
typedef enum {
  AMRSUP_SPEECH_GOOD = 0,      /* Good speech frame              */
  AMRSUP_SPEECH_DEGRADED = 1,  /* Speech degraded                */
  AMRSUP_ONSET = 2,            /* onset                          */
  AMRSUP_SPEECH_BAD = 3,       /* Corrupt speech frame (bad CRC) */
  AMRSUP_SID_FIRST = 4,        /* First silence descriptor       */
  AMRSUP_SID_UPDATE = 5,       /* Comfort noise frame            */
  AMRSUP_SID_BAD = 6,          /* Corrupt SID frame (bad CRC)    */
  AMRSUP_NO_DATA = 7,          /* Nothing to transmit            */
  AMRSUP_SPEECH_LOST = 8,      /* Lost speech in downlink        */
  AMRSUP_FRAME_TYPE_MAX
} amrsup_frame_type;

/* AMR frame mode (frame rate) definitions */
typedef enum {
  AMRSUP_MODE_0475 = 0,   /* 4.75 kbit /s */
  AMRSUP_MODE_0515 = 1,   /* 5.15 kbit /s */
  AMRSUP_MODE_0590 = 2,   /* 5.90 kbit /s */
  AMRSUP_MODE_0670 = 3,   /* 6.70 kbit /s */
  AMRSUP_MODE_0740 = 4,   /* 7.40 kbit /s */
  AMRSUP_MODE_0795 = 5,   /* 7.95 kbit /s */
  AMRSUP_MODE_1020 = 6,   /* 10.2 kbit /s */
  AMRSUP_MODE_1220 = 7,   /* 12.2 kbit /s */
  AMRSUP_MODE_0660 = 8,   /* AMR-WB 6.60 kbps */
  AMRSUP_MODE_0885 = 9,   /* AMR-WB 8.85 kbps */
  AMRSUP_MODE_1265 = 10,  /* AMR-WB 12.65 kbps */
  AMRSUP_MODE_1425 = 11,  /* AMR-WB 14.25 kbps */
  AMRSUP_MODE_1585 = 12,  /* AMR-WB 15.85 kbps */
  AMRSUP_MODE_1825 = 13,  /* AMR-WB 18.25 kbps */
  AMRSUP_MODE_1985 = 14,  /* AMR-WB 19.85 kbps */
  AMRSUP_MODE_2305 = 15,  /* AMR-WB 23.05 kbps */
  AMRSUP_MODE_2385 = 16,  /* AMR-WB 23.85 kbps */
  AMRSUP_MODE_MAX
} amrsup_mode_type;

/* Frame types for 4-bit frame type as in 3GPP TS 26.101 v3.2.0, Sec.4.1.1 */
typedef enum {
  AMRSUP_FRAME_TYPE_INDEX_0475    = 0,    /* 4.75 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0515    = 1,    /* 5.15 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0590    = 2,    /* 5.90 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0670    = 3,    /* 6.70 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0740    = 4,    /* 7.40 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0795    = 5,    /* 7.95 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1020    = 6,    /* 10.2 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1220    = 7,    /* 12.2 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_AMR_SID = 8,    /* SID frame       */
  /* Frame types 9-11 are not supported */
  /* Frame types 12-14 are reserved for future use */
  AMRSUP_FRAME_TYPE_INDEX_0660    = 0,    /* 6.60  kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_0885    = 1,    /* 8.85  kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1265    = 2,    /* 12.65 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1425    = 3,    /* 14.25 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1585    = 4,    /* 15.85 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1825    = 5,    /* 18.25 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_1985    = 6,    /* 19.85 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_2305    = 7,    /* 23.05 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_2385    = 8,    /* 23.85 kbit /s    */
  AMRSUP_FRAME_TYPE_INDEX_AWB_SID = 9,    /* AWB SID frame   */
  AMRSUP_FRAME_TYPE_INDEX_AWB_SPL = 14,   /* AWB Speech Lost frame */
  AMRSUP_FRAME_TYPE_INDEX_NO_DATA = 15,   /* No data         */
  AMRSUP_FRAME_TYPE_INDEX_MAX,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF = AMRSUP_FRAME_TYPE_INDEX_MAX
} amrsup_frame_type_index_type;

typedef enum {
  AMRSUP_CODEC_AMR_NB = 0,
  AMRSUP_CODEC_AMR_WB = 1,
  AMRSUP_CODEC_MAX
} amrsup_codec_type;

/* IF1-encoded frame info */
typedef struct {
  amrsup_frame_type_index_type frame_type_index;
  boolean fqi;    /* frame quality indicator: TRUE: good frame, FALSE: bad */
  amrsup_codec_type amr_type;   /* AMR-NB or AMR-WB */
} amrsup_if1_frame_info_type;

/* The maximum number of bits in each class */
#define AMRSUP_CLASS_A_MAX 81
#define AMRSUP_CLASS_B_MAX 405
#define AMRSUP_CLASS_C_MAX 60

/* The size of the buffer required to hold the vocoder frame */
#define AMRSUP_VOC_FRAME_BYTES  \
  ((AMRSUP_CLASS_A_MAX + AMRSUP_CLASS_B_MAX + AMRSUP_CLASS_C_MAX + 7) / 8)

/* Size of each AMR class to hold one frame of AMR data */
#define AMRSUP_CLASS_A_BYTES ((AMRSUP_CLASS_A_MAX + 7) / 8)
#define AMRSUP_CLASS_B_BYTES ((AMRSUP_CLASS_B_MAX + 7) / 8)
#define AMRSUP_CLASS_C_BYTES ((AMRSUP_CLASS_C_MAX + 7) / 8)

/* Valid/invalid frame types */
#define AMRSUP_CLASS_DATA_INVALID 0
#define AMRSUP_CLASS_DATA_VALID_A 1
#define AMRSUP_CLASS_DATA_VALID_B 2
#define AMRSUP_CLASS_DATA_VALID_C 4
#define AMRSUP_CLASS_DATA_CORRUPT 8

#define AMRSUP_CLASS_DATA_VALID (AMRSUP_CLASS_DATA_VALID_A | \
                                 AMRSUP_CLASS_DATA_VALID_B | \
                                 AMRSUP_CLASS_DATA_VALID_C)

/* Type to contain class-divided AMR Core speech data
*/
typedef struct {
  int   num_bits_a;     /* The number of bits in the class a buffer      */
  int   num_bits_b;     /* The number of bits in the class b buffer      */
  int   num_bits_c;     /* The number of bits in the class c buffer      */
  uint8 *data_a;        /* Pointer to buffer containing the class a data */
  uint8 *data_b;        /* Pointer to buffer containing the class b data */
  uint8 *data_c;        /* Pointer to buffer containing the class c data */
} amrsup_core_speech_type;

typedef enum {
  AMRSUP_CLASS_A = 0,
  AMRSUP_CLASS_B = 1,
  AMRSUP_CLASS_C = 2
} amrsup_class_type;

/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION amrsup_frame_class_bits

DESCRIPTION
This function will determine number of bits of certain class
based on the frame type and frame rate.

DEPENDENCIES
None.

RETURN VALUE
number of bits of AMR frame in certain class

SIDE EFFECTS
None.

===========================================================================*/
extern int amrsup_frame_class_bits (
  amrsup_frame_type frame_type,
  amrsup_mode_type amr_mode,
  amrsup_class_type amr_class
);

/*===========================================================================

FUNCTION amrsup_if1_frame_class_bits

DESCRIPTION
This function will determine number of bits of certain class
based on the frame type index.

DEPENDENCIES
None.

RETURN VALUE
number of bits of AMR frame in certain class

SIDE EFFECTS
None.

===========================================================================*/
extern int amrsup_if1_frame_class_bits (
  amrsup_frame_type_index_type frame_type_index,
  amrsup_class_type amr_class
);

/*===========================================================================

FUNCTION amrsup_frame_len_bits

DESCRIPTION
This function will determine number of bits of AMR vocoder frame length
based on the frame type and frame rate.

DEPENDENCIES
None.

RETURN VALUE
number of bits of AMR frame

SIDE EFFECTS
None.

===========================================================================*/
extern int amrsup_frame_len_bits (
  amrsup_frame_type frame_type,
  amrsup_mode_type amr_mode
);

/*===========================================================================

FUNCTION amrsup_mode_has_class_data

DESCRIPTION
Determines whether of not the AMR mode contains data of the selected class.

DEPENDENCIES
None.

RETURN VALUE
Returns TRUE if the selected AMR mode contains data of the selected class.

SIDE EFFECTS
None.

===========================================================================*/
extern boolean amrsup_mode_has_class_data (
  amrsup_mode_type   amr_mode,
  amrsup_class_type  amr_class
);

/*===========================================================================

FUNCTION amrsup_move_valid_bits

DESCRIPTION
  This function is used to move dedicated bit mask values from one
  'valid bits' number into another.

  A valid bits number is a byte containing 3 bit masks:
  AMRSUP_CLASS_DATA_VALID_A (bit 0),
  AMRSUP_CLASS_DATA_VALID_B (bit 1),
  AMRSUP_CLASS_DATA_VALID_C (bit 2)

  A valid bits number could be used for general purposes, such as
  indicating CRC status bits of each class A, B and C data.

  In the above instance, when all bits are set, the byte informs data validity
  of all 3 classes.  When all bits are unset, none of the data is valid and
  the number will be equal to AMRSUP_CLASS_DATA_INVALID.

  A valid bits number could also contain a corrupt bit mask:
  AMRSUP_CLASS_DATA_CORRUPT (bit 3).

  Usage of corrupt bit mask is mostly dependent on the application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  *src bit mask values will be copied over to *dst.
  *src bit masks will be set.

===========================================================================*/
extern void amrsup_move_valid_bits (
  uint8 *dst,
  uint8 *src,
  uint8 mask
);

/* <EJECT> */
/*===========================================================================

FUNCTION amrsup_validate_data

DESCRIPTION
  Attempts to determine if the a,b, and c data belong to the same packet.
  If not, it determines if it is possible to move data to create a valid packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_validate_data (
  amrsup_core_speech_type *speech_data
);

/*===========================================================================

FUNCTION amrsup_audfmt_framing

DESCRIPTION
  Performs the transmit side framing function. Generates class-divided and
  ordered data from the vocoder packet and frame type input data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void amrsup_audfmt_framing (
  uint8             *vocoder_packet,
  amrsup_frame_type  frame_type,
  amrsup_mode_type   amr_mode,
  uint8             *amr_data
);

/* <EJECT> */
/*===========================================================================

FUNCTION amrsup_audfmt_de_framing

DESCRIPTION
  Performs the receive side de-framing function. Generates a vocoder packet
  and frame type information from the class-divided and ordered input data.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_audfmt_de_framing (
  uint8             *vocoder_packet,
  amrsup_frame_type  frame_type,
  amrsup_mode_type   amr_mode,
  uint8             *amr_data
);

/*===========================================================================

FUNCTION amrsup_if1_de_framing

DESCRIPTION
  Performs the receive side de-framing function. Generates a vocoder packet
  and frame type information from the AMR IF1 input data.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of decoded frame.
  vocoder_packet : decoded packet
  frame_type     : AMR frame type of decoded packet
  amr_mode       : AMR frame rate of decoded packet

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_if1_de_framing (
  uint8                      *vocoder_packet,
  amrsup_frame_type          *frame_type,
  amrsup_mode_type           *amr_mode,
  uint8                      *if1_frame,
  amrsup_if1_frame_info_type *if1_frame_info
);

/*===========================================================================

FUNCTION amrsup_if1_framing

DESCRIPTION
  Performs the transmit side framing function.  Generates AMR IF1 ordered data
  from the vocoder packet and frame type.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of encoded frame.
  if1_frame : IF1-encoded frame.
  if1_frame_info : holds frame information of IF1-encoded frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_if1_framing (
  uint8                      *vocoder_packet,
  amrsup_frame_type           frame_type,
  amrsup_mode_type            amr_mode,
  uint8                      *if1_frame,
  amrsup_if1_frame_info_type *if1_frame_info
);

/*==========================================================================
FUNCTION amrsup_frame_len

DESCRIPTION
  This function will determine number of bytes of AMR vocoder frame length
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of AMR frame

SIDE EFFECTS
  None.

===========================================================================*/
extern int amrsup_frame_len (
  amrsup_frame_type frame_type,
  amrsup_mode_type amr_mode
);

/*===========================================================================

FUNCTION amrsup_get_frame_type

DESCRIPTION
  This function will return frame type information of a packet.

DEPENDENCIES
  None.

RETURN VALUE
  amrsup_frame_type

SIDE EFFECTS
  None.

===========================================================================*/
extern amrsup_frame_type amrsup_get_frame_type (
  amrsup_core_speech_type *speech_data
);

/*===========================================================================

FUNCTION amrsup_get_frame_mode

DESCRIPTION
  This function will return frame mode information of a packet.

DEPENDENCIES
  None.

RETURN VALUE
  amrsup_mode_type

SIDE EFFECTS
  None.

===========================================================================*/
extern amrsup_mode_type amrsup_get_frame_mode (
  amrsup_core_speech_type *speech_data,
  amrsup_frame_type frame_type
);

/*===========================================================================

FUNCTION amrsup_rx_order

DESCRIPTION
  Use a bit ordering table to re-order bits to their original sequence.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void amrsup_rx_order (
  uint8        *dst,
  uint8        *src_frame,
  int          *src_bit_index,
  int           num_bits,
  const uint16 *order
);

/*===========================================================================

FUNCTION amrsup_set_default_dl_amr_mode

DESCRIPTION
  Set the default downlink amr mode to be used by amrsup utility functions
  (like amrsup_get_frame_mode() if the the mode is not known yet (because,
  for example, only NO_DATA frames have been received thus far).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void amrsup_set_default_dl_amr_mode (
  amrsup_mode_type amr_mode
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* !__AMRSUP_H__ */

