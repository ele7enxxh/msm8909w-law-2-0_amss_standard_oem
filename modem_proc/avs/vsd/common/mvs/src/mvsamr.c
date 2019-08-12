/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                    A M R   F R A M I N G   P R O C E S S I N G

DESCRIPTION
  Functions that copy AMR vocoder frames to and from logical channel data.

REFERENCES
  3G TS 26.093 - "AMR Speech Codec; Source Controlled Rate Operation"
  3G TS 26.101 - "AMR Speech Codec; Frame structure"

Copyright (C) 2002-2014 QUALCOMM Technologies Incorporated
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/src/mvsamr.c#1 $
$Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/20/13   sre     Fixed the banned QPSI memcpy APIs.
06/26/13   sud     Updated Copyright.
04/25/10   ayu     Reorganized 8K source code to match up with legacy.
                   Re-added missing AMR frame prediction code.
06/01/09    at     Added AMR_WB support and fixed some warnings.
08/30/08    at     Added if1 versions of functions to determine class number
                   of bits from frame_type_index.
08/16/07   ash     Moved from P4 /asic/msmshared/openmm/com/
03/18/06    ay     Cleaned up lint errors.
                   Updated Copyright.
10/19/05   ph      removed  include of amrsup.h as its done through mvsamr.h
07/22/05   ayu     Added AMR IF1 support.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
01/27/04   ymc     Removed LOCAL declarations from conversion tables.
           ymc     Moved MVS_AMR_TO_AMRSUP_FRAME() and MVS_AMR_TO_AMRSUP_MODE()
                   definitions to mvsamr.h.
           ymc     Updated Copyright.
09/16/03   ymc     Added mvs_amr_is_subset_chan_state.
06/29/03   ymc     Added mvs_amr_frame_len.
04/17/03    sm     Added amrsup support.
            sm     Moved mvs->voc, voc->mvs tables here, added
                   mvs_amr_audfmt_framing(), mvs_amr_audfmt_de_framing().
01/24/03   ymc     Updated copyright.
01/14/03    sm     Moved AMR reorg code & tables to mvsamr_iram.c. Include
                   "mvsamri.h".
12/04/02    sm     Added support for AMR IF2.
11/14/02    sm     If no A data, clear B & C data.
05/30/02   ymc     Ported from wvsamr.c.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include <string.h>
#include "comdef.h"
#include "mvsamr.h"
#include "amrsup.h"
#include "msg.h"
#include "mmstd.h"

/* <EJECT> */

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* Conversion tables & macros from MVS_AMR to AMRSUP & vice versa
*/
mvs_amr_frame_type amrsup_to_mvs_amr_frame_table[AMRSUP_FRAME_TYPE_MAX] =
{
  MVS_AMR_SPEECH_GOOD,     /* AMRSUP_SPEECH_GOOD */
  MVS_AMR_SPEECH_DEGRADED, /* AMRSUP_SPEECH_DEGRADED */
  MVS_AMR_ONSET,           /* AMRSUP_ONSET */
  MVS_AMR_SPEECH_BAD,      /* AMRSUP_SPEECH_BAD */
  MVS_AMR_SID_FIRST,       /* AMRSUP_SID_FIRST */
  MVS_AMR_SID_UPDATE,      /* AMRSUP_SID_UPDATE */
  MVS_AMR_SID_BAD,         /* AMRSUP_SID_BAD */
  MVS_AMR_NO_DATA,         /* AMRSUP_NO_DATA */
  MVS_AMR_SPEECH_LOST      /* AMRSUP_SPEECH_LOST */
};

amrsup_frame_type mvs_amr_to_amrsup_frame_table[MVS_AMR_FRAME_TYPE_MAX] =
{
  AMRSUP_SPEECH_GOOD,     /* MVS_AMR_SPEECH_GOOD */
  AMRSUP_SPEECH_DEGRADED, /* MVS_AMR_SPEECH_DEGRADED */
  AMRSUP_ONSET,           /* MVS_AMR_ONSET */
  AMRSUP_SPEECH_BAD,      /* MVS_AMR_SPEECH_BAD */
  AMRSUP_SID_FIRST,       /* MVS_AMR_SID_FIRST */
  AMRSUP_SID_UPDATE,      /* MVS_AMR_SID_UPDATE */
  AMRSUP_SID_BAD,         /* MVS_AMR_SID_BAD */
  AMRSUP_NO_DATA,         /* MVS_AMR_NO_DATA */
  AMRSUP_SPEECH_LOST      /* MVS_AMR_SPEECH_LOST */
};

mvs_amr_mode_type amrsup_to_mvs_amr_mode_table[AMRSUP_MODE_MAX] =
{
  MVS_AMR_MODE_0475,       /* AMRSUP_MODE_0475 */
  MVS_AMR_MODE_0515,       /* AMRSUP_MODE_0515 */
  MVS_AMR_MODE_0590,       /* AMRSUP_MODE_0590 */
  MVS_AMR_MODE_0670,       /* AMRSUP_MODE_0670 */
  MVS_AMR_MODE_0740,       /* AMRSUP_MODE_0740 */
  MVS_AMR_MODE_0795,       /* AMRSUP_MODE_0795 */
  MVS_AMR_MODE_1020,       /* AMRSUP_MODE_1020 */
  MVS_AMR_MODE_1220,       /* AMRSUP_MODE_1220 */
  MVS_AMR_MODE_0660,       /* AMRSUP_MODE_0660 */
  MVS_AMR_MODE_0885,       /* AMRSUP_MODE_0885 */
  MVS_AMR_MODE_1265,       /* AMRSUP_MODE_1265 */
  MVS_AMR_MODE_1425,       /* AMRSUP_MODE_1425 */
  MVS_AMR_MODE_1585,       /* AMRSUP_MODE_1585 */
  MVS_AMR_MODE_1825,       /* AMRSUP_MODE_1825 */
  MVS_AMR_MODE_1985,       /* AMRSUP_MODE_1985 */
  MVS_AMR_MODE_2305,       /* AMRSUP_MODE_2305 */
  MVS_AMR_MODE_2385        /* AMRSUP_MODE_2385 */
};

amrsup_mode_type mvs_amr_to_amrsup_mode_table[MVS_AMR_MODE_MAX] =
{
  AMRSUP_MODE_0475,        /* MVS_AMR_MODE_0475 */
  AMRSUP_MODE_0515,        /* MVS_AMR_MODE_0515 */
  AMRSUP_MODE_0590,        /* MVS_AMR_MODE_0590 */
  AMRSUP_MODE_0670,        /* MVS_AMR_MODE_0670 */
  AMRSUP_MODE_0740,        /* MVS_AMR_MODE_0740 */
  AMRSUP_MODE_0795,        /* MVS_AMR_MODE_0795 */
  AMRSUP_MODE_1020,        /* MVS_AMR_MODE_1020 */
  AMRSUP_MODE_1220,        /* MVS_AMR_MODE_1220 */
  AMRSUP_MODE_0660,        /* MVS_AMR_MODE_0660 */
  AMRSUP_MODE_0885,        /* MVS_AMR_MODE_0885 */
  AMRSUP_MODE_1265,        /* MVS_AMR_MODE_1265 */
  AMRSUP_MODE_1425,        /* MVS_AMR_MODE_1425 */
  AMRSUP_MODE_1585,        /* MVS_AMR_MODE_1585 */
  AMRSUP_MODE_1825,        /* MVS_AMR_MODE_1825 */
  AMRSUP_MODE_1985,        /* MVS_AMR_MODE_1985 */
  AMRSUP_MODE_2305,        /* MVS_AMR_MODE_2305 */
  AMRSUP_MODE_2385         /* MVS_AMR_MODE_2385 */
};

/* expected good frame w.r.t next state */
mvs_amr_frame_type mvs_amr_state_good_frame_table[MVS_AMR_STATE_MAX] =
{
  MVS_AMR_SPEECH_GOOD,     /* MVS_AMR_STATE_SPEECH  */
  MVS_AMR_SID_FIRST,       /* MVS_AMR_STATE_SID_F   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_F_DTX_1 */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_F_DTX_2 */
  MVS_AMR_SID_UPDATE,      /* MVS_AMR_STATE_SID_U   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_1   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_2   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_3   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_4   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_5   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_6   */
  MVS_AMR_NO_DATA          /* MVS_AMR_STATE_DTX_7   */
};

/* expected bad frame (CRC fails or multiple CRCs pass) w.r.t. next state */
mvs_amr_frame_type mvs_amr_state_bad_frame_table[MVS_AMR_STATE_MAX] =
{
  MVS_AMR_SPEECH_BAD,      /* MVS_AMR_STATE_SPEECH  */
  MVS_AMR_SID_BAD,         /* MVS_AMR_STATE_SID_F   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_F_DTX_1 */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_F_DTX_2 */
  MVS_AMR_SID_BAD,         /* MVS_AMR_STATE_SID_U   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_1   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_2   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_3   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_4   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_5   */
  MVS_AMR_NO_DATA,         /* MVS_AMR_STATE_DTX_6   */
  MVS_AMR_NO_DATA          /* MVS_AMR_STATE_DTX_7   */
};

/* CRC status type w.r.t. AMR frame type */
mvs_amr_crc_status_type mvs_amr_frame_crc_table[MVS_AMR_FRAME_TYPE_MAX] =
{
  MVS_AMR_CRC_SPEECH_ONLY, /* MVS_AMR_SPEECH_GOOD     */
  MVS_AMR_CRC_FAIL,        /* MVS_AMR_SPEECH_DEGRADED */
  MVS_AMR_CRC_FAIL,        /* MVS_AMR_ONSET           */
  MVS_AMR_CRC_FAIL,        /* MVS_AMR_SPEECH_BAD      */
  MVS_AMR_CRC_SID_F_ONLY,  /* MVS_AMR_SID_FIRST       */
  MVS_AMR_CRC_SID_U_ONLY,  /* MVS_AMR_SID_UPDATE      */
  MVS_AMR_CRC_FAIL,        /* MVS_AMR_SID_BAD         */
  MVS_AMR_CRC_DTX_ONLY,    /* MVS_AMR_NO_DATA         */
  MVS_AMR_CRC_SPEECH_ONLY  /* MVS_AMR_SPEECH_LOST     */
};

mvs_amr_frame_type_index_type
  amrsup_to_mvs_amr_frame_type_index_table[AMRSUP_FRAME_TYPE_INDEX_MAX] =
{
  MVS_AMR_FRAME_TYPE_INDEX_0475,
  MVS_AMR_FRAME_TYPE_INDEX_0515,
  MVS_AMR_FRAME_TYPE_INDEX_0590,
  MVS_AMR_FRAME_TYPE_INDEX_0670,
  MVS_AMR_FRAME_TYPE_INDEX_0740,
  MVS_AMR_FRAME_TYPE_INDEX_0795,
  MVS_AMR_FRAME_TYPE_INDEX_1020,
  MVS_AMR_FRAME_TYPE_INDEX_1220,
  MVS_AMR_FRAME_TYPE_INDEX_AMR_SID,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_NO_DATA
};

amrsup_frame_type_index_type
  mvs_amr_to_amrsup_frame_type_index_table[MVS_AMR_FRAME_TYPE_INDEX_MAX] =
{
  AMRSUP_FRAME_TYPE_INDEX_0475,
  AMRSUP_FRAME_TYPE_INDEX_0515,
  AMRSUP_FRAME_TYPE_INDEX_0590,
  AMRSUP_FRAME_TYPE_INDEX_0670,
  AMRSUP_FRAME_TYPE_INDEX_0740,
  AMRSUP_FRAME_TYPE_INDEX_0795,
  AMRSUP_FRAME_TYPE_INDEX_1020,
  AMRSUP_FRAME_TYPE_INDEX_1220,
  AMRSUP_FRAME_TYPE_INDEX_AMR_SID,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_NO_DATA
};

#if defined(FEATURE_VOC_AMR_WB)
mvs_amr_frame_type_index_type
  amrsup_to_mvs_awb_frame_type_index_table[AMRSUP_FRAME_TYPE_INDEX_MAX] =
{
  MVS_AMR_FRAME_TYPE_INDEX_0660,
  MVS_AMR_FRAME_TYPE_INDEX_0885,
  MVS_AMR_FRAME_TYPE_INDEX_1265,
  MVS_AMR_FRAME_TYPE_INDEX_1425,
  MVS_AMR_FRAME_TYPE_INDEX_1585,
  MVS_AMR_FRAME_TYPE_INDEX_1825,
  MVS_AMR_FRAME_TYPE_INDEX_1985,
  MVS_AMR_FRAME_TYPE_INDEX_2305,
  MVS_AMR_FRAME_TYPE_INDEX_2385,
  MVS_AMR_FRAME_TYPE_INDEX_AWB_SID,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_UNDEF,
  MVS_AMR_FRAME_TYPE_INDEX_AWB_SPL,
  MVS_AMR_FRAME_TYPE_INDEX_NO_DATA
};

amrsup_frame_type_index_type
  mvs_awb_to_amrsup_frame_type_index_table[MVS_AMR_FRAME_TYPE_INDEX_MAX] =
{
  AMRSUP_FRAME_TYPE_INDEX_0660,
  AMRSUP_FRAME_TYPE_INDEX_0885,
  AMRSUP_FRAME_TYPE_INDEX_1265,
  AMRSUP_FRAME_TYPE_INDEX_1425,
  AMRSUP_FRAME_TYPE_INDEX_1585,
  AMRSUP_FRAME_TYPE_INDEX_1825,
  AMRSUP_FRAME_TYPE_INDEX_1985,
  AMRSUP_FRAME_TYPE_INDEX_2305,
  AMRSUP_FRAME_TYPE_INDEX_2385,
  AMRSUP_FRAME_TYPE_INDEX_AWB_SID,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_UNDEF,
  AMRSUP_FRAME_TYPE_INDEX_AWB_SPL,
  AMRSUP_FRAME_TYPE_INDEX_NO_DATA
};
#endif /*MSMAUD_VOC_AMR_WB*/
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
boolean mvs_amr_is_subset_chan_state(
  mvs_amr_chan_state_type *chan_state1,
  mvs_amr_chan_state_type *chan_state2
)
{
  if ((chan_state1==NULL) || (chan_state2==NULL))
  {
    return (chan_state1==chan_state2);
  }

  if ((chan_state1->has_chan_a == TRUE) && (chan_state2->has_chan_a == FALSE))
  {
    return FALSE;
  }

  if ((chan_state1->has_chan_b == TRUE) && (chan_state2->has_chan_b == FALSE))
  {
    return FALSE;
  }

  if ((chan_state1->has_chan_c == TRUE) && (chan_state2->has_chan_c == FALSE))
  {
    return FALSE;
  }

  return TRUE;
}


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
boolean mvs_amr_cmp_chan_state(
  mvs_amr_chan_state_type *chan_state1,
  mvs_amr_chan_state_type *chan_state2
)
{
  if ((chan_state1==NULL) || (chan_state2==NULL))
  {
    return (chan_state1==chan_state2);
  }

  if (chan_state1->has_chan_a != chan_state2->has_chan_a)
  {
    return FALSE;
  }

  if (chan_state1->has_chan_b != chan_state2->has_chan_b)
  {
    return FALSE;
  }

  if (chan_state1->has_chan_c != chan_state2->has_chan_c)
  {
    return FALSE;
  }

  return TRUE;
}


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
mvs_amr_chan_state_type mvs_amr_get_chan_state(
  mvs_amr_mode_type amr_mode
)
{
  mvs_amr_chan_state_type chan_state = {FALSE, FALSE, FALSE};


  if (((int8)amr_mode<MVS_AMR_MODE_0475) || (amr_mode>=MVS_AMR_MODE_MAX))
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Invalid AMR mode %d", amr_mode);
    return chan_state;
  }

  chan_state.has_chan_a = amrsup_mode_has_class_data(
                                               MVS_AMR_TO_AMRSUP_MODE(amr_mode),
                                               AMRSUP_CLASS_A);
  chan_state.has_chan_b = amrsup_mode_has_class_data(
                                               MVS_AMR_TO_AMRSUP_MODE(amr_mode),
                                               AMRSUP_CLASS_B);
  chan_state.has_chan_c = amrsup_mode_has_class_data(
                                               MVS_AMR_TO_AMRSUP_MODE(amr_mode),
                                               AMRSUP_CLASS_C);

  return chan_state;
}


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
void mvs_amr_init_core_speech(
  amrsup_core_speech_type     *amr_speech,
  mvs_amr_speech_buffer_type  *speech_buf
)
{
  if ((amr_speech==NULL) || (speech_buf==NULL))
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"MVS AMR speech init failed.");
    return;
  }


  amr_speech->num_bits_a = 0;
  amr_speech->num_bits_b = 0;
  amr_speech->num_bits_c = 0;

  amr_speech->data_a = speech_buf->data_a;
  amr_speech->data_b = speech_buf->data_b;
  amr_speech->data_c = speech_buf->data_c;
}


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
void mvs_amr_copy_core_speech(
  amrsup_core_speech_type *dst_speech,
  amrsup_core_speech_type *src_speech
)
{
  if ((src_speech==NULL) || (dst_speech==NULL))
  {
    return;
  }

  /* Return empty speech if copy fails */
  dst_speech->num_bits_a = 0;
  dst_speech->num_bits_b = 0;
  dst_speech->num_bits_c = 0;


  if (src_speech->num_bits_a > 0)
  {
    if ((src_speech->data_a == NULL) || (dst_speech->data_a == NULL))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "AMR speech copy class A " \
           "failed." );
      return;
    }
    mmstd_memcpy(dst_speech->data_a, AMRSUP_CLASS_A_BYTES, src_speech->data_a, AMRSUP_CLASS_A_BYTES);
  }

  if (src_speech->num_bits_b > 0)
  {
    if ((src_speech->data_b == NULL) || (dst_speech->data_b == NULL))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "AMR speech copy class B " \
           "failed." );
      return;
    }
    mmstd_memcpy(dst_speech->data_b, AMRSUP_CLASS_B_BYTES, src_speech->data_b, AMRSUP_CLASS_B_BYTES);
  }

  if (src_speech->num_bits_c > 0)
  {
    if ((src_speech->data_c == NULL) || (dst_speech->data_c == NULL))
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "AMR speech copy class C " \
           "failed." );
      return;
    }
    mmstd_memcpy(dst_speech->data_c, AMRSUP_CLASS_C_BYTES, src_speech->data_c, AMRSUP_CLASS_C_BYTES);
  }


  dst_speech->num_bits_a = src_speech->num_bits_a;
  dst_speech->num_bits_b = src_speech->num_bits_b;
  dst_speech->num_bits_c = src_speech->num_bits_c;

  return;
}


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
int mvs_amr_move_class_a(
  amrsup_core_speech_type *dst_speech,
  amrsup_core_speech_type *src_speech
)
{
  if ((src_speech==NULL) || (dst_speech==NULL))
  {
    return 0;
  }

  dst_speech->num_bits_a = src_speech->num_bits_a;
  src_speech->num_bits_a = 0;

  if (src_speech->data_a == NULL)
  {
    return 0;
  }

  if (dst_speech->data_a == NULL)
  {
    dst_speech->data_a = src_speech->data_a;
    src_speech->data_a = NULL;
  }
  else
  {
    mmstd_memcpy(dst_speech->data_a, AMRSUP_CLASS_A_BYTES, src_speech->data_a, AMRSUP_CLASS_A_BYTES);
  }

  return (dst_speech->num_bits_a);
}


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
int mvs_amr_move_class_b(
  amrsup_core_speech_type *dst_speech,
  amrsup_core_speech_type *src_speech
)
{
  if ((src_speech==NULL) || (dst_speech==NULL))
  {
    return 0;
  }

  dst_speech->num_bits_b = src_speech->num_bits_b;
  src_speech->num_bits_b = 0;

  if (src_speech->data_b == NULL)
  {
    return 0;
  }

  if (dst_speech->data_b == NULL)
  {
    dst_speech->data_b = src_speech->data_b;
    src_speech->data_b = NULL;
  }
  else
  {
    mmstd_memcpy(dst_speech->data_b, AMRSUP_CLASS_B_BYTES, src_speech->data_b, AMRSUP_CLASS_B_BYTES);
  }

  return (dst_speech->num_bits_b);
}


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
int mvs_amr_move_class_c(
  amrsup_core_speech_type *dst_speech,
  amrsup_core_speech_type *src_speech
)
{
  if ((src_speech==NULL) || (dst_speech==NULL))
  {
    return 0;
  }

  dst_speech->num_bits_c = src_speech->num_bits_c;
  src_speech->num_bits_c = 0;

  if (src_speech->data_c == NULL)
  {
    return 0;
  }

  if (dst_speech->data_c == NULL)
  {
    dst_speech->data_c = src_speech->data_c;
    src_speech->data_c = NULL;
  }
  else
  {
    mmstd_memcpy(dst_speech->data_c, AMRSUP_CLASS_C_BYTES, src_speech->data_c, AMRSUP_CLASS_C_BYTES);
  }

  return (dst_speech->num_bits_c);
}

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
int mvs_amr_class_A_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_class_bits(MVS_AMR_TO_AMRSUP_FRAME(frame_type),
                                 MVS_AMR_TO_AMRSUP_MODE(amr_mode),
                                 AMRSUP_CLASS_A);
}


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
int mvs_amr_class_B_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_class_bits(MVS_AMR_TO_AMRSUP_FRAME(frame_type),
                                 MVS_AMR_TO_AMRSUP_MODE(amr_mode),
                                 AMRSUP_CLASS_B);
}


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
int mvs_amr_class_C_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_class_bits(MVS_AMR_TO_AMRSUP_FRAME(frame_type),
                                 MVS_AMR_TO_AMRSUP_MODE(amr_mode),
                                 AMRSUP_CLASS_C);
}


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
int mvs_amr_frame_len(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_len(MVS_AMR_TO_AMRSUP_FRAME(frame_type),
                          MVS_AMR_TO_AMRSUP_MODE(amr_mode));
}


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
int mvs_amr_frame_len_bits(
  mvs_amr_frame_type frame_type,
  mvs_amr_mode_type amr_mode
)
{
  if ((frame_type >= MVS_AMR_FRAME_TYPE_MAX) || (amr_mode >= MVS_AMR_MODE_MAX))
  {
    return 0;
  }


  return amrsup_frame_len_bits(MVS_AMR_TO_AMRSUP_FRAME(frame_type),
                               MVS_AMR_TO_AMRSUP_MODE(amr_mode));
}


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
mvs_amr_frame_type mvs_amr_state_roll(
  mvs_amr_crc_status_type crc_status,       /* CRC status of next frame */
  boolean update                            /* update state or not */
)
{
  static mvs_amr_state_type state = MVS_AMR_STATE_UNDEF;
  mvs_amr_state_type next_state = MVS_AMR_STATE_UNDEF;
  mvs_amr_frame_type ret_frame = MVS_AMR_FRAME_TYPE_UNDEF;


  if (crc_status==MVS_AMR_CRC_UNDEF)
  {
    /* reset the state machine */
    if (update==TRUE)
    {
      state = MVS_AMR_STATE_UNDEF;
      MSG(MSG_SSID_DFLT, MSG_LEGACY_MED,"Reset AMR state machine");
      return MVS_AMR_FRAME_TYPE_UNDEF;
    }


    /* no state change */
    if (state==MVS_AMR_STATE_UNDEF)
    {
      ret_frame = MVS_AMR_FRAME_TYPE_UNDEF;
    }
    else
    {
      ret_frame = mvs_amr_state_good_frame_table[state];
    }

    MSG_2(MSG_SSID_DFLT, MSG_LEGACY_MED,"Current AMR state %d %d",
                                         state, ret_frame);
    return ret_frame;
  }


  switch (crc_status)
  {
    case MVS_AMR_CRC_SPEECH_ONLY :
      next_state = MVS_AMR_STATE_SPEECH;
      break;

    case MVS_AMR_CRC_SID_F_ONLY :
      next_state = MVS_AMR_STATE_SID_F;

      ret_frame = mvs_amr_state_roll(MVS_AMR_CRC_SID_ONLY, FALSE);
      if (ret_frame!=MVS_AMR_SID_FIRST)
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Incompatible SID_FIRST " \
              "prediction %d",
               ret_frame );
      }

      break;

    case MVS_AMR_CRC_SID_U_ONLY :
      next_state = MVS_AMR_STATE_SID_U;

      ret_frame = mvs_amr_state_roll(MVS_AMR_CRC_SID_ONLY, FALSE);
      if (ret_frame!=MVS_AMR_SID_UPDATE)
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Incompatible SID_UPDATE " \
              "prediction %d",
               ret_frame );
      }

      break;

    case MVS_AMR_CRC_SID_ONLY :
      switch (state)
      {
        case MVS_AMR_STATE_UNDEF :
        case MVS_AMR_STATE_SPEECH :
          next_state = MVS_AMR_STATE_SID_F;
          break;

        case MVS_AMR_STATE_F_DTX_2 :
        case MVS_AMR_STATE_DTX_7 :
          next_state = MVS_AMR_STATE_SID_U;
          break;

        case MVS_AMR_STATE_DTX_2 :
          next_state = MVS_AMR_STATE_SID_U;
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Incompatible old state %d",
                                                state);
          break;

        default :
          next_state = MVS_AMR_STATE_SID_F;
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Incompatible old state %d",
                                                state);
          break;
      }

      break;

    case MVS_AMR_CRC_DTX_ONLY :
      switch (state)
      {
        case MVS_AMR_STATE_F_DTX_2 :
          next_state = MVS_AMR_STATE_DTX_3;
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Incompatible old state %d",
                                                state);
          break;

        case MVS_AMR_STATE_SPEECH :
        case MVS_AMR_STATE_DTX_7 :
          MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Incompatible old state %d",
                                                state);
          /* fall thru */
        case MVS_AMR_STATE_UNDEF :
          next_state = MVS_AMR_STATE_F_DTX_1;
          break;

        default :
          next_state = (mvs_amr_state_type)(state + 1);
          break;
      }

      break;

    case MVS_AMR_CRC_MULTIPLE :
    case MVS_AMR_CRC_FAIL :
      switch (state)
      {
        case MVS_AMR_STATE_UNDEF :
        case MVS_AMR_STATE_SPEECH :
          next_state = MVS_AMR_STATE_SPEECH;
          break;

        case MVS_AMR_STATE_DTX_7 :
          next_state = MVS_AMR_STATE_SID_U;
          break;

        default :
          next_state = (mvs_amr_state_type)(state + 1);
          break;
      }

      break;

    default :
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unknown CRC status %d",
                                             crc_status);
      return MVS_AMR_FRAME_TYPE_UNDEF;
  }


  if ((crc_status==MVS_AMR_CRC_MULTIPLE) || (crc_status==MVS_AMR_CRC_FAIL))
  {
    ret_frame = mvs_amr_state_bad_frame_table[next_state];
  }
  else
  {
    ret_frame = mvs_amr_state_good_frame_table[next_state];
  }


  MSG_3(MSG_SSID_DFLT, MSG_LEGACY_MED,"AMR state old %d new %d crc %d",
                                       state, next_state, crc_status);

  if (update==TRUE)
  {
    state = next_state;
  }


  return ret_frame;
}

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
mvs_amr_crc_status_type mvs_amr_get_crc_status(
  amrsup_core_speech_type *amr_speech
)
{
  amrsup_frame_type amrsup_frame;


  amrsup_frame = amrsup_get_frame_type(amr_speech);
  if (amrsup_frame >= AMRSUP_FRAME_TYPE_MAX)
  {
    return MVS_AMR_CRC_UNDEF;
  }

  return mvs_amr_frame_crc_table[AMRSUP_TO_MVS_AMR_FRAME(amrsup_frame)];
}

