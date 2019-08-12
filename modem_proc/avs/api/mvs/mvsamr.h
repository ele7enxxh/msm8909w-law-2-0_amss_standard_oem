#ifndef __MVSAMR_H__
#define __MVSAMR_H__

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                    A M R   F R A M I N G   P R O C E S S I N G

DESCRIPTION
  This header file contains definitions used by the AMR framing functions.

REFERENCES
  3G TS 26.093 - "AMR Speech Codec; Source Controlled Rate Operation"
  3G TS 26.101 - "AMR Speech Codec; Frame structure"

Copyright (C) 2002-2010, 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/mvs/mvsamr.h#1 $
$Author: mplcsds1 $
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sud     Updated Copyright.
04/25/10   ayu     Reorganized 8K source code to match up with legacy.
08/18/09    sr     Adapting to CVD.
08/30/08    at     Added if1 versions of functions to determine class number
                   of bits from frame_type_index.
08/16/07   ash     Moved from P4 /asic/msmshared/openmm/com/
03/18/06    ay     Cleaned up lint errors.
                   Updated Copyright.
07/22/05   ayu     Added AMR IF1 support.
01/28/05   ymc     Removed MSMAUD dependencies.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
01/27/04   ymc     Added MVS_AMR_TO_AMRSUP_FRAME() and MVS_AMR_TO_AMRSUP_MODE().
           ymc     Updated Copyright.
09/16/03   ymc     Added mvs_amr_is_subset_chan_state.
06/29/03   ymc     Added mvs_amr_frame_len.
04/17/03    sm     Added amrsup support.
            sm     Remove mvssup.h, add voc.h. Moved frame_type, mode_type
                   from mvssup.h to here.  Added protos for
                   mvs_amr_audfmt_framing(), mvs_amr_audfmt_de_framing().
12/04/02    sm     Added support for AMR IF2.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "amrsup.h"

/* <EJECT> */
/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* AMR frame type definitions */
typedef enum {
  MVS_AMR_SPEECH_GOOD,          /* Good speech frame              */
  MVS_AMR_SPEECH_DEGRADED,      /* Speech degraded                */
  MVS_AMR_ONSET,                /* onset                          */
  MVS_AMR_SPEECH_BAD,           /* Corrupt speech frame (bad CRC) */
  MVS_AMR_SID_FIRST,            /* First silence descriptor       */
  MVS_AMR_SID_UPDATE,           /* Comfort noise frame            */
  MVS_AMR_SID_BAD,              /* Corrupt SID frame (bad CRC)    */
  MVS_AMR_NO_DATA,              /* Nothing to transmit            */
  MVS_AMR_SPEECH_LOST,          /* downlink speech lost           */
  MVS_AMR_FRAME_TYPE_MAX,
  MVS_AMR_FRAME_TYPE_UNDEF = MVS_AMR_FRAME_TYPE_MAX  /* undefined */
} mvs_amr_frame_type;

/* AMR frame mode (frame rate) definitions */
typedef enum {
  MVS_AMR_MODE_0475,    /* 4.75 kbit /s */
  MVS_AMR_MODE_0515,    /* 5.15 kbit /s */
  MVS_AMR_MODE_0590,    /* 5.90 kbit /s */
  MVS_AMR_MODE_0670,    /* 6.70 kbit /s */
  MVS_AMR_MODE_0740,    /* 7.40 kbit /s */
  MVS_AMR_MODE_0795,    /* 7.95 kbit /s */
  MVS_AMR_MODE_1020,    /* 10.2 kbit /s */
  MVS_AMR_MODE_1220,    /* 12.2 kbit /s */
  MVS_AMR_MODE_0660,    /* AMR-WB 6.60 kbps */
  MVS_AMR_MODE_0885,    /* AMR-WB 8.85 kbps */
  MVS_AMR_MODE_1265,    /* AMR-WB 12.65 kbps */
  MVS_AMR_MODE_1425,    /* AMR-WB 14.25 kbps */
  MVS_AMR_MODE_1585,    /* AMR-WB 15.85 kbps */
  MVS_AMR_MODE_1825,    /* AMR-WB 18.25 kbps */
  MVS_AMR_MODE_1985,    /* AMR-WB 19.85 kbps */
  MVS_AMR_MODE_2305,    /* AMR-WB 23.05 kbps */
  MVS_AMR_MODE_2385,    /* AMR-WB 23.85 kbps */
  MVS_AMR_MODE_MAX,
  MVS_AMR_MODE_UNDEF = MVS_AMR_MODE_MAX  /* undefined */
} mvs_amr_mode_type;

/* AMR Frame Type Index (4-bit); 3GPP TS 26.101 V6.0.0, Table 1a & 1c */
typedef enum {
  MVS_AMR_FRAME_TYPE_INDEX_0475 = 0,      /* AMR 4,75 kbit/s            */
  MVS_AMR_FRAME_TYPE_INDEX_0515 = 1,      /* AMR 5,15 kbit/s            */
  MVS_AMR_FRAME_TYPE_INDEX_0590 = 2,      /* AMR 5,90 kbit/s            */
  MVS_AMR_FRAME_TYPE_INDEX_0670 = 3,      /* AMR 6,70 kbit/s (PDC-EFR)  */
  MVS_AMR_FRAME_TYPE_INDEX_0740 = 4,      /* AMR 7,40 kbit/s (TDMA-EFR) */
  MVS_AMR_FRAME_TYPE_INDEX_0795 = 5,      /* AMR 7,95 kbit/s            */
  MVS_AMR_FRAME_TYPE_INDEX_1020 = 6,      /* AMR 10,2 kbit/s            */
  MVS_AMR_FRAME_TYPE_INDEX_1220 = 7,      /* AMR 12,2 kbit/s (GSM-EFR)  */
  MVS_AMR_FRAME_TYPE_INDEX_AMR_SID = 8,   /* AMR SID                    */
  MVS_AMR_FRAME_TYPE_INDEX_0660    = 0,    /* 6.60  kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_0885    = 1,    /* 8.85  kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_1265    = 2,    /* 12.65 kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_1425    = 3,    /* 14.25 kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_1585    = 4,    /* 15.85 kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_1825    = 5,    /* 18.25 kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_1985    = 6,    /* 19.85 kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_2305    = 7,    /* 23.05 kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_2385    = 8,    /* 23.85 kbit /s    */
  MVS_AMR_FRAME_TYPE_INDEX_AWB_SID = 9,    /* AWB SID frame   */
  MVS_AMR_FRAME_TYPE_INDEX_AWB_SPL = 14,   /* AWB Speech Lost frame */
  MVS_AMR_FRAME_TYPE_INDEX_NO_DATA = 15,  /* No Data                    */
  MVS_AMR_FRAME_TYPE_INDEX_MAX,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF = MVS_AMR_FRAME_TYPE_INDEX_MAX
} mvs_amr_frame_type_index_type;

/* External conversion tables
*/
extern mvs_amr_frame_type amrsup_to_mvs_amr_frame_table[AMRSUP_FRAME_TYPE_MAX];
#define AMRSUP_TO_MVS_AMR_FRAME(x) amrsup_to_mvs_amr_frame_table[(x)]
extern amrsup_frame_type mvs_amr_to_amrsup_frame_table[MVS_AMR_FRAME_TYPE_MAX];
#define MVS_AMR_TO_AMRSUP_FRAME(x) mvs_amr_to_amrsup_frame_table[(x)]
extern mvs_amr_mode_type amrsup_to_mvs_amr_mode_table[AMRSUP_MODE_MAX];
#define AMRSUP_TO_MVS_AMR_MODE(x) amrsup_to_mvs_amr_mode_table[(x)]
extern amrsup_mode_type mvs_amr_to_amrsup_mode_table[MVS_AMR_MODE_MAX];
#define MVS_AMR_TO_AMRSUP_MODE(x) mvs_amr_to_amrsup_mode_table[(x)]

extern mvs_amr_frame_type_index_type
  amrsup_to_mvs_amr_frame_type_index_table[AMRSUP_FRAME_TYPE_INDEX_MAX];
#define AMRSUP_TO_MVS_AMR_FRAME_TYPE_INDEX(x) \
  amrsup_to_mvs_amr_frame_type_index_table[(x)];
extern amrsup_frame_type_index_type
  mvs_amr_to_amrsup_frame_type_index_table[MVS_AMR_FRAME_TYPE_INDEX_MAX];
#define MVS_AMR_TO_AMRSUP_FRAME_TYPE_INDEX(x) \
  mvs_amr_to_amrsup_frame_type_index_table[(x)];


extern mvs_amr_frame_type_index_type
  amrsup_to_mvs_awb_frame_type_index_table[AMRSUP_FRAME_TYPE_INDEX_MAX];
#define AMRSUP_TO_MVS_AWB_FRAME_TYPE_INDEX(x) \
  amrsup_to_mvs_awb_frame_type_index_table[(x)];
extern amrsup_frame_type_index_type
  mvs_awb_to_amrsup_frame_type_index_table[MVS_AMR_FRAME_TYPE_INDEX_MAX];
#define MVS_AWB_TO_AMRSUP_FRAME_TYPE_INDEX(x) \
  mvs_awb_to_amrsup_frame_type_index_table[(x)];

/* status of vocoder packet CRCs */
typedef enum {
  MVS_AMR_CRC_SPEECH_ONLY,    /* only SPEECH TF CRC passed */
  MVS_AMR_CRC_SID_F_ONLY,     /* only SID FIRST TF CRC passed */
  MVS_AMR_CRC_SID_U_ONLY,     /* only SID UPDATE TF CRC passed */
  MVS_AMR_CRC_SID_ONLY,       /* only SID TF CRC passed */
  MVS_AMR_CRC_DTX_ONLY,       /* only DTX TF CRC passed */
  MVS_AMR_CRC_MULTIPLE,       /* CRCs passed for multiple TF */
  MVS_AMR_CRC_FAIL,           /* all CRCs failed */
  MVS_AMR_CRC_MAX,
  MVS_AMR_CRC_UNDEF = MVS_AMR_CRC_MAX   /* undefined */
} mvs_amr_crc_status_type;

/* current AMR frame state */
typedef enum {
  MVS_AMR_STATE_SPEECH,    /* SPEECH state                   */
  MVS_AMR_STATE_SID_F,     /* SID FIRST                      */
  MVS_AMR_STATE_F_DTX_1,   /* NO DATA cnt 1 after SID FIRST  */
  MVS_AMR_STATE_F_DTX_2,   /* NO DATA cnt 2 after SID FIRST  */
  MVS_AMR_STATE_SID_U,     /* SID UPDATE                     */
  MVS_AMR_STATE_DTX_1,     /* NO DATA cnt 1 after SID UPDATE */
  MVS_AMR_STATE_DTX_2,     /* NO DATA cnt 2 after SID UPDATE */
  MVS_AMR_STATE_DTX_3,     /* NO DATA cnt 3 after SID UPDATE */
  MVS_AMR_STATE_DTX_4,     /* NO DATA cnt 4 after SID UPDATE */
  MVS_AMR_STATE_DTX_5,     /* NO DATA cnt 5 after SID UPDATE */
  MVS_AMR_STATE_DTX_6,     /* NO DATA cnt 6 after SID UPDATE */
  MVS_AMR_STATE_DTX_7,     /* NO DATA cnt 7 after SID UPDATE */
  MVS_AMR_STATE_MAX,
  MVS_AMR_STATE_UNDEF = MVS_AMR_STATE_MAX  /* undefined */
} mvs_amr_state_type;

/* frame prediction result */
typedef struct {
  mvs_amr_frame_type predicted_frame;
  boolean is_bad_frame;
} mvs_amr_predict_type;


/* Type for buffer allocation of AMR core speech data
*/
typedef struct {
  uint8 data_a[AMRSUP_CLASS_A_BYTES];
  uint8 data_b[AMRSUP_CLASS_B_BYTES];
  uint8 data_c[AMRSUP_CLASS_C_BYTES];
} mvs_amr_speech_buffer_type;

/* Type for defining channel setup state */
typedef struct {
  boolean has_chan_a;   /* channel A exists */
  boolean has_chan_b;   /* channel B exists */
  boolean has_chan_c;   /* channel C exists */
} mvs_amr_chan_state_type;

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

/*===========================================================================

FUNCTION mvs_amr_is_subset_chan_state

DESCRIPTION
  Compare two channel setup states to see if chan_state1 is a subset of
chan_state2.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE : chan_state1 C chan_state2
  FALSE : not a subset

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mvs_amr_is_subset_chan_state(
  mvs_amr_chan_state_type *chan_state1,
  mvs_amr_chan_state_type *chan_state2
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_cmp_chan_state

DESCRIPTION
  Compare two channel setup states to see if they are identical.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE : identical
  FALSE : different

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean mvs_amr_cmp_chan_state(
  mvs_amr_chan_state_type *chan_state1,
  mvs_amr_chan_state_type *chan_state2
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_get_chan_state

DESCRIPTION
  Determine channel A, B, C setup state based on passed in AMR mode.

DEPENDENCIES
  None.

RETURN VALUE
  channel setup state.

SIDE EFFECTS
  None.

===========================================================================*/
extern mvs_amr_chan_state_type mvs_amr_get_chan_state(
  mvs_amr_mode_type amr_mode
);


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_init_core_speech

DESCRIPTION
  Initialize AMR core speech buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mvs_amr_init_core_speech(
  amrsup_core_speech_type     *amr_speech,
  mvs_amr_speech_buffer_type  *speech_buf
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_copy_core_speech

DESCRIPTION
  Copy one AMR core speech to another.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void mvs_amr_copy_core_speech(
  amrsup_core_speech_type *dst_speech,
  amrsup_core_speech_type *src_speech
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_move_class_a

DESCRIPTION
  Move class A information from one speech to another.

DEPENDENCIES
  None.

RETURN VALUE
  Number of actual data bytes moved.

SIDE EFFECTS
  None.

===========================================================================*/
extern int mvs_amr_move_class_a(
  amrsup_core_speech_type *dst_speech,
  amrsup_core_speech_type *src_speech
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_move_class_b

DESCRIPTION
  Move class B information from one speech to another.

DEPENDENCIES
  None.

RETURN VALUE
  Number of actual data bytes moved.

SIDE EFFECTS
  None.

===========================================================================*/
extern int mvs_amr_move_class_b(
  amrsup_core_speech_type *dst_speech,
  amrsup_core_speech_type *src_speech
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_move_class_c

DESCRIPTION
  Move class C information from one speech to another.

DEPENDENCIES
  None.

RETURN VALUE
  Number of actual data bytes moved.

SIDE EFFECTS
  None.

===========================================================================*/
extern int mvs_amr_move_class_c(
  amrsup_core_speech_type *dst_speech,
  amrsup_core_speech_type *src_speech
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_class_A_bits

DESCRIPTION
  This function will determine number of bits of AMR class A data
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of class A bits of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int mvs_amr_class_A_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_class_B_bits

DESCRIPTION
  This function will determine number of bits of AMR class B data
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of class B bits of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int mvs_amr_class_B_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_class_C_bits

DESCRIPTION
  This function will determine number of bits of AMR class C data
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of class C bits of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int mvs_amr_class_C_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_frame_len

DESCRIPTION
  This function will determine number of bytes of AMR vocoder frame length
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of bytes of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int mvs_amr_frame_len(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_frame_len_bits

DESCRIPTION
  This function will determine number of bits of AMR vocoder frame length
based on the frame type and frame rate.

DEPENDENCIES
  None.

RETURN VALUE
  number of bits of AMR frame.

SIDE EFFECTS
  None.

===========================================================================*/
extern int mvs_amr_frame_len_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_state_roll

DESCRIPTION
  This function runs a state machine to predict the next possible AMR frame
based on the previous frames.  Next state could be peeked without update.
Undefined CRC status with update option is used to reset the state machine.
Undefined CRC status without update option indicates no state changes,
predicted frame will be based on good crc status in this case.

DEPENDENCIES
  None.

RETURN VALUE
  Next AMR frame type predicted.

SIDE EFFECTS
  None.

===========================================================================*/
extern mvs_amr_frame_type mvs_amr_state_roll(
  mvs_amr_crc_status_type crc_status,       /* CRC status of next frame */
  boolean update                            /* update state or not */
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_amr_get_crc_status

DESCRIPTION
  This function will return a CRC status type based on AMRSUP core speech.

DEPENDENCIES
  None.

RETURN VALUE
  mvs_amr_crc_status_type

SIDE EFFECTS
  None.

===========================================================================*/
extern mvs_amr_crc_status_type mvs_amr_get_crc_status(
  amrsup_core_speech_type *amr_speech
);

#endif  /* !__MVSAMR_H__ */

