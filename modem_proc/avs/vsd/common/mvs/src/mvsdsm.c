/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                       D S M   P O O L   P R O C E S S I N G

DESCRIPTION
  This file contains definitions for processing packets in MVS DSM pools.

REFERENCES
  None.

Copyright (C) 2002-2014 QUALCOMM Technologies Incorporated
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/src/mvsdsm.c#1 $
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
11/04/11    at     Compiler warning fix in mvsdsm.c due to unannounced DSM API
                   change in watermark non_empty_func callback parameter lis.
10/08/10    jk     Added dsm queue init.
04/25/10   ayu     Reorganized 8K source code to match up with legacy.
                   Re-added missing AMR frame prediction code.
06/05/09    at     Fixed a compiler warning in mvs_dsm_amr_set_loop_mode().
07/10/08    at     Removed #include "msmaud.h" (this file will be gone).
05/07/08    at     Removed FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING code.
04/28/05   ymc     Modified mvs_dsm_amr_predict_dl so that MVS will always
                   use frame type information from L1 instead of running
                   prediction automatically when CRC status is bad.
                   Featurised automatic prediction under MVS internal feature
                   MVSF_WCDMA_AMR_AUTO_PREDICT_CRC_FRAME.
           ymc     Modified mvs_dsm_amr_predict_dl so that MVS will drop the
                   first good frame if CRC error rate is too high for a moment.
                   Featurised under MVS internal feature
                   MVSF_WCDMA_AMR_CRC_FRAME_WINDOW.
           ymc     Updated Copyright.
08/18/04   ymc     Added support for FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING.
06/10/04   ymc     If there are too many downlink packets in DSM queues,
                   dump the rest instead of calling ERR_FATAL.
04/17/03    sm     Added amrsup support.
            sm     Refeaturized code.
03/03/03   ymc     Cleaned up all lint errors and warnings.
01/24/03   ymc     Updated copyright.
01/16/03    sm     Removed unused variable in mvs_dsm_amr_dl_proc().
12/16/02   ymc     Changed the uplink packet loss message level from ERROR to
                   HIGH.
05/30/02   ymc     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include <string.h>

#ifndef WINSIM

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "queue.h"

#include "mvs.h"
#include "mvsdsm.h"
#include "mvsamr.h"
#include "amrsup.h"

/* <EJECT> */

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

LOCAL mvs_dsm_amr_queue_type mvs_dsm_amr_ul_q;
LOCAL mvs_dsm_amr_queue_type mvs_dsm_amr_dl_q;
LOCAL mvs_dsm_amr_queue_type mvs_dsm_amr_ul2dl_q;

#define MVS_AMR_UL_QUEUE_LEN 2
#define MVS_AMR_DL_QUEUE_LEN 2

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
LOCAL mvs_amr_predict_type mvs_amr_dl_predict[MVS_AMR_DL_QUEUE_LEN];

#ifdef MVSF_WCDMA_AMR_CRC_FRAME_WINDOW
#define MVS_AMR_CRC_WINDOW_SIZE 20
LOCAL int mvs_amr_consec_crc_cnt = 0;  /* consecutive CRC frames */
#endif /* MVSF_WCDMA_AMR_CRC_FRAME_WINDOW */
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

LOCAL amrsup_core_speech_type mvs_amr_dl_speech[MVS_AMR_DL_QUEUE_LEN];
LOCAL int mvs_amr_dl_spi = 0;    /* downlink speech index */
LOCAL int mvs_amr_dl_cnt = 0;    /* downlink speech count */
LOCAL int mvs_amr_dl_lost = 0;   /* count of DL speech lost */
LOCAL int mvs_amr_dl_bad = 0;    /* count of DL bad speech */

LOCAL int mvs_amr_ul_lost = 0;   /* count of UL speech lost */

/* buffers for storing loopback speech packets */
LOCAL amrsup_core_speech_type mvs_amr_ul2dl_speech[MVS_AMR_DL_QUEUE_LEN];
LOCAL int mvs_amr_ul2dl_spi = 0;   /* UL2DL_VOC speech index */
LOCAL int mvs_amr_ul2dl_cnt = 0;   /* UL2DL_VOC speech count */
LOCAL int mvs_amr_ul2dl_lost = 0;  /* count of UL2DL_VOC speech lost */

LOCAL amrsup_core_speech_type mvs_amr_dl2ul_speech[MVS_AMR_UL_QUEUE_LEN];
LOCAL int mvs_amr_dl2ul_spi = 0;   /* DL2UL speech index */
LOCAL int mvs_amr_dl2ul_cnt = 0;   /* DL2UL speech count */
LOCAL int mvs_amr_dl2ul_lost = 0;  /* count of DL2UL speech lost */

LOCAL mvs_dsm_amr_state_type mvs_dsm_amr_state = MVS_DSM_AMR_STATE_RESET;

LOCAL mvs_dsm_loop_type mvs_dsm_loop = MVS_DSM_LOOP_OFF;

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
/*===========================================================================

FUNCTION mvs_dsm_amr_predict_dl

DESCRIPTION
  To perform prediction on AMR downlink speeches.  AMR state will be updated
according to new downlink speeches received.

DEPENDENCIES
  mvs_dsm_amr_reorganize_data should have been called first to put
mvs_amr_dl_speech in proper order.

RETURN VALUE
  None

SIDE EFFECTS
  mvs_amr_dl_predict[] will be updated.

===========================================================================*/
void mvs_dsm_amr_predict_dl(
  uint8 *crc_valid_bits,      /* CRC status bits table */
  int head,                   /* index of the first speech */
  int count                   /* number of speeches */
)
{
  mvs_amr_crc_status_type crc_status;
  int spi;
  int i;


  if ((crc_valid_bits==NULL) || (count<=0) || (count>MVS_AMR_DL_QUEUE_LEN)
      || (head<0) || (head>=MVS_AMR_DL_QUEUE_LEN))
  {
    return;
  }


  for (i=0;i<count;i++)
  {
    spi = (head+i) % MVS_AMR_DL_QUEUE_LEN;

    mvs_amr_dl_predict[spi].is_bad_frame
    = (crc_valid_bits[spi] == AMRSUP_CLASS_DATA_VALID)? FALSE : TRUE;

#ifdef MVSF_WCDMA_AMR_CRC_FRAME_WINDOW
    if (mvs_amr_dl_predict[spi].is_bad_frame == FALSE)
    {
      if (mvs_amr_consec_crc_cnt >= MVS_AMR_CRC_WINDOW_SIZE)
      {
        mvs_amr_dl_predict[spi].is_bad_frame = TRUE;
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Drop good frame: consecutive " \
              "CRCs %d", mvs_amr_consec_crc_cnt );
      }
      mvs_amr_consec_crc_cnt = 0;
    }
    else
    {
      if (mvs_amr_consec_crc_cnt < MVS_AMR_CRC_WINDOW_SIZE)
      {
        mvs_amr_consec_crc_cnt++;
      }
    }
#endif  /* MVSF_WCDMA_AMR_CRC_FRAME_WINDOW */

#ifdef MVSF_WCDMA_AMR_AUTO_PREDICT_CRC_FRAME
    if (mvs_amr_dl_predict[spi].is_bad_frame == TRUE)
    {
      crc_status = MVS_AMR_CRC_FAIL;
    }
    else
#endif  /* MVSF_WCDMA_AMR_AUTO_PREDICT_CRC_FRAME */
    {
      crc_status = mvs_amr_get_crc_status(mvs_amr_dl_speech+spi);
      if (crc_status >= MVS_AMR_CRC_MULTIPLE)
      {
        /* maybe reordering problem */
        MSG_3(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Unexpected speech %d %d %d",
                                               mvs_amr_dl_speech[spi].num_bits_a,
                                               mvs_amr_dl_speech[spi].num_bits_b,
                                               mvs_amr_dl_speech[spi].num_bits_c);

        /* force to NO_DATA frame due to program error */
        mvs_amr_dl_speech[spi].num_bits_a = 0;
        mvs_amr_dl_speech[spi].num_bits_b = 0;
        mvs_amr_dl_speech[spi].num_bits_c = 0;
        crc_status = MVS_AMR_CRC_DTX_ONLY;
      }
    }

    /* update AMR frame state based on newly received frames */
    mvs_amr_dl_predict[spi].predicted_frame
    = mvs_amr_state_roll(crc_status, TRUE);

    /* If no auto prediction, state machine totally driven by L1 frames. */
    if (mvs_amr_dl_predict[spi].is_bad_frame == TRUE)
    {
      switch (mvs_amr_dl_predict[spi].predicted_frame)
      {
        case MVS_AMR_SPEECH_GOOD :
          mvs_amr_dl_predict[spi].predicted_frame = MVS_AMR_SPEECH_BAD;
          break;

        case MVS_AMR_SID_FIRST :
        case MVS_AMR_SID_UPDATE :
          mvs_amr_dl_predict[spi].predicted_frame = MVS_AMR_SID_BAD;
          break;

        default :
          break;
      }
    }
  }  /* for () */
}
#endif  /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_reorganize_data

DESCRIPTION
  To reorder class A, B, C data in an array of speeches for producing valid
speeches.

DEPENDENCIES
  None

RETURN VALUE
  Number of bad speeches encountered.

SIDE EFFECTS
  None

===========================================================================*/
int mvs_dsm_amr_reorganize_data(
  amrsup_core_speech_type *speech_tbl,   /* pointer to a rotary array */
  int head,           /* index of the first speech */
  int count,          /* number of speeches in the table */
  int size,           /* table size */
  uint8 *valid_tbl    /* rotary array containing valid bits of each packet */
)
{
  int validity = AMRSUP_CLASS_DATA_INVALID;  /* speech validity check bits */
  int bad_cnt=0;                   /* number of bad speeches */
  int reorda=count;                /* index of last class A reordered */
  int reordb=count;                /* index of last class B reordered */
  int reordc=count;                /* index of last class C reordered */
  int spi, spj;                    /* speech table index */
  int i;


  if ((speech_tbl==NULL) || (count<=0) || (size<=0) || (count>size)
      || (head<0) || (head>=size))
  {
    return 0;
  }


  for (i=count-1; i>=0; i--)
  {
    spi = (head+i) % size;

    validity = amrsup_validate_data(speech_tbl + spi);
    if (validity == AMRSUP_CLASS_DATA_VALID)
    {
      continue;
    }

    /* invalid or corrupted packet which does not require reordering */
    if ((validity == AMRSUP_CLASS_DATA_INVALID)
        || ((validity & AMRSUP_CLASS_DATA_VALID) == AMRSUP_CLASS_DATA_VALID)
       )
    {
      speech_tbl[spi].num_bits_a = 0;
      speech_tbl[spi].num_bits_b = 0;
      speech_tbl[spi].num_bits_c = 0;
      bad_cnt++;
      continue;
    }

    /* Reordering class A data if necessary */
    if ((validity & AMRSUP_CLASS_DATA_VALID_A) == 0)
    {
      if (speech_tbl[spi].num_bits_a > 0)
      {
        /* Reordering attempt failed */
        validity |= AMRSUP_CLASS_DATA_CORRUPT;
      }
      else
      {
        reorda = (reorda<i)? (reorda-1) : (i-1);

        for (; reorda>=0; reorda--)
        {
          spj = (head+reorda) % size;

          if (speech_tbl[spj].num_bits_a > 0)
          {
            /* Found substitution: reorder class A data */
            if (mvs_amr_move_class_a(speech_tbl+spi, speech_tbl+spj) == 0)
            {
              validity |= AMRSUP_CLASS_DATA_CORRUPT;
            }

            /* reorder relating data structures */
            if (valid_tbl!=NULL)
            {
              amrsup_move_valid_bits(valid_tbl+spi, valid_tbl+spj,
                                     AMRSUP_CLASS_DATA_VALID_A);
            }

            break;
          }
        }

        /* Reordering attempt failed */
        if (reorda<0)
        {
          reorda = -1;
          validity |= AMRSUP_CLASS_DATA_CORRUPT;
        }
      }
    }

    /* Reordering class B data if necessary */
    if ((validity & AMRSUP_CLASS_DATA_VALID_B) == 0)
    {
      if (speech_tbl[spi].num_bits_b > 0)
      {
        /* Reordering attempt failed */
        validity |= AMRSUP_CLASS_DATA_CORRUPT;
      }
      else
      {
        reordb = (reordb<i)? (reordb-1) : (i-1);

        for (; reordb>=0; reordb--)
        {
          spj = (head+reordb) % size;

          if (speech_tbl[spj].num_bits_b > 0)
          {
            /* Found substitution: reorder class B data */
            if (mvs_amr_move_class_b(speech_tbl+spi, speech_tbl+spj) == 0)
            {
              validity |= AMRSUP_CLASS_DATA_CORRUPT;
            }

            /* reorder relating data structures */
            if (valid_tbl!=NULL)
            {
              amrsup_move_valid_bits(valid_tbl+spi, valid_tbl+spj,
                                     AMRSUP_CLASS_DATA_VALID_B);
            }

            break;
          }
        }

        /* Reordering attempt failed */
        if (reordb<0)
        {
          reordb = -1;
          validity |= AMRSUP_CLASS_DATA_CORRUPT;
        }
      }
    }

    /* Reordering class C data if necessary */
    if ((validity & AMRSUP_CLASS_DATA_VALID_C) == 0)
    {
      if (speech_tbl[spi].num_bits_c > 0)
      {
        /* Reordering attempt failed */
        validity |= AMRSUP_CLASS_DATA_CORRUPT;
      }
      else
      {
        reordc = (reordc<i)? (reordc-1) : (i-1);

        for (; reordc>=0; reordc--)
        {
          spj = (head+reordc) % size;

          if (speech_tbl[spj].num_bits_c > 0)
          {
            /* Found substitution: reorder class C data */
            if (mvs_amr_move_class_c(speech_tbl+spi, speech_tbl+spj) == 0)
            {
              validity |= AMRSUP_CLASS_DATA_CORRUPT;
            }

            /* reorder relating data structures */
            if (valid_tbl!=NULL)
            {
              amrsup_move_valid_bits(valid_tbl+spi, valid_tbl+spj,
                                     AMRSUP_CLASS_DATA_VALID_C);
            }

            break;
          }
        }

        /* Reordering attempt failed */
        if (reordc<0)
        {
          reordc = -1;
          validity |= AMRSUP_CLASS_DATA_CORRUPT;
        }
      }
    }

    if ((validity & AMRSUP_CLASS_DATA_CORRUPT) != 0)
    {
      speech_tbl[spi].num_bits_a = 0;
      speech_tbl[spi].num_bits_b = 0;
      speech_tbl[spi].num_bits_c = 0;
      bad_cnt++;
      continue;
    }

    /* recheck the validity after reorganizing the packet */
    if (amrsup_validate_data(speech_tbl+spi) != AMRSUP_CLASS_DATA_VALID)
    {
      speech_tbl[spi].num_bits_a = 0;
      speech_tbl[spi].num_bits_b = 0;
      speech_tbl[spi].num_bits_c = 0;
      bad_cnt++;
    }
  }  /* end for loop */


  if (bad_cnt>0)
  {
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Bits mismatch: bad pkts %d",
                                           bad_cnt);
  }

  return bad_cnt;
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_dl_dequeue_a

DESCRIPTION
  Causes downlink queued class A packet to immediately be dequeued.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_dl_dequeue_a(
  dsm_watermark_type *dsm_watermark,
  void *param
)
{
  dsm_empty_queue(&(mvs_dsm_amr_dl_q.wm_a));
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_dl_dequeue_b

DESCRIPTION
  Causes downlink queued class B packet to immediately be dequeued.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_dl_dequeue_b(
  dsm_watermark_type *dsm_watermark,
  void *param
)
{
  dsm_empty_queue(&(mvs_dsm_amr_dl_q.wm_b));
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_dl_dequeue_c

DESCRIPTION
  Causes downlink queued class C packet to immediately be dequeued.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_dl_dequeue_c(
  dsm_watermark_type *dsm_watermark,
  void *param
)
{
  dsm_empty_queue(&(mvs_dsm_amr_dl_q.wm_c));
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_enable_dldsm

DESCRIPTION
  Allow downlink DSM buffer to accept any packets or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_enable_dldsm(
  boolean enable_flag
)
{
  if (enable_flag==TRUE)
  {
    INTLOCK();
    mvs_dsm_amr_dl_q.wm_a.non_empty_func_ptr = NULL;
    mvs_dsm_amr_dl_q.wm_b.non_empty_func_ptr = NULL;
    mvs_dsm_amr_dl_q.wm_c.non_empty_func_ptr = NULL;
    INTFREE();
  }
  else
  {
    INTLOCK();
    mvs_dsm_amr_dl_q.wm_a.non_empty_func_ptr = mvs_dsm_amr_dl_dequeue_a;
    mvs_dsm_amr_dl_q.wm_b.non_empty_func_ptr = mvs_dsm_amr_dl_dequeue_b;
    mvs_dsm_amr_dl_q.wm_c.non_empty_func_ptr = mvs_dsm_amr_dl_dequeue_c;
    INTFREE();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_q_empty

DESCRIPTION
  Emptify AMR DSM queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_q_empty(
  mvs_dsm_amr_queue_type *amr_queues
)
{
  if (amr_queues==NULL)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Empty AMR DSM queue failed.");
    return;
  }

  INTLOCK();
  dsm_empty_queue(&(amr_queues->wm_a));
  dsm_empty_queue(&(amr_queues->wm_b));
  dsm_empty_queue(&(amr_queues->wm_c));
  INTFREE();
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_q_init

DESCRIPTION
  Initialize AMR DSM watermarks.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_q_init(
  mvs_dsm_amr_queue_type *amr_dsm_wms,
  q_type queues[3],
  uint32 queue_len
)
{
  if (amr_dsm_wms==NULL)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"AMR DSM queue init failed.");
    return;
  }

  /* Initialize class A watermark */
  dsm_queue_init(&amr_dsm_wms->wm_a,
                 ( AMRSUP_CLASS_A_BYTES * queue_len + 1 ),
                 queues );

  amr_dsm_wms->wm_a.hi_watermark = AMRSUP_CLASS_A_BYTES;
  amr_dsm_wms->wm_a.lo_watermark = 0;

  /* Initialize class B watermark */
  dsm_queue_init(&amr_dsm_wms->wm_b,
                 ( AMRSUP_CLASS_B_BYTES * queue_len + 1 ),
                 queues+1 );

  amr_dsm_wms->wm_b.hi_watermark = AMRSUP_CLASS_B_BYTES;
  amr_dsm_wms->wm_b.lo_watermark = 0;

  /* Initialize class C watermark */
  dsm_queue_init(&amr_dsm_wms->wm_c,
                 ( AMRSUP_CLASS_C_BYTES * queue_len + 1 ),
                 queues+2 );

  amr_dsm_wms->wm_c.hi_watermark = AMRSUP_CLASS_C_BYTES;
  amr_dsm_wms->wm_c.lo_watermark = 0;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_init

DESCRIPTION
  Initialize AMR DSM buffers.

DEPENDENCIES
  To be called once only.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_init(void)
{
  static q_type ul_queues[3];
  static q_type dl_queues[3];
  static q_type ul2dl_queues[3];


  if (mvs_dsm_amr_state != MVS_DSM_AMR_STATE_RESET)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "AMR DSM buffers have already been " \
        "initialized." );
    return;
  }

  /* ======== Initialize uplink DSM watermarks ======== */
  mvs_dsm_amr_q_init(&mvs_dsm_amr_ul_q, ul_queues, MVS_AMR_UL_QUEUE_LEN);

  /* ======== Initialize downlink DSM watermarks ======== */
  mvs_dsm_amr_q_init(&mvs_dsm_amr_dl_q, dl_queues, MVS_AMR_DL_QUEUE_LEN);

  mvs_dsm_amr_enable_dldsm(FALSE);

  /* ======== Initialize UL2DL loopback DSM watermarks ======== */
  mvs_dsm_amr_q_init(&mvs_dsm_amr_ul2dl_q, ul2dl_queues, MVS_AMR_DL_QUEUE_LEN);


  mvs_dsm_loop = MVS_DSM_LOOP_OFF;
  mvs_dsm_amr_state = MVS_DSM_AMR_STATE_INIT;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_activate

DESCRIPTION
  Activate or de-activate AMR DSM buffer management.
  When activated,
  uplink: queuing passed-in or DL2UL loopback AMR packet to uplink DSM buffer.
  downlink: extracting AMR packet from downlink DSM buffer or returning back
            UL2DL loopback AMR packet.

DEPENDENCIES
  mvs_dsm_amr_init has to be called first for initialization.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_activate(
  boolean active_flag
)
{
  static mvs_amr_speech_buffer_type dl_speech_buffers[MVS_AMR_DL_QUEUE_LEN];
  static mvs_amr_speech_buffer_type ul2dl_speech_buffers[MVS_AMR_DL_QUEUE_LEN];
  static mvs_amr_speech_buffer_type dl2ul_speech_buffers[MVS_AMR_UL_QUEUE_LEN];
  int i;


  if (mvs_dsm_amr_state == MVS_DSM_AMR_STATE_RESET)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "AMR DSM buffers have not been " \
         "initialized." );
    return;
  }


  if (active_flag==TRUE)
  {
    if (mvs_dsm_amr_state != MVS_DSM_AMR_STATE_INIT)
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "MVS AMR DSMs are being active, " \
            "state %d",
             mvs_dsm_amr_state );
      return;
    }

    /* Initialize all AMR speech buffers */
    for (i=0; i<MVS_AMR_DL_QUEUE_LEN; i++)
    {
      mvs_amr_init_core_speech((mvs_amr_dl_speech+i), (dl_speech_buffers+i));

      mvs_amr_init_core_speech((mvs_amr_ul2dl_speech+i),
                               (ul2dl_speech_buffers+i));
    }

    for (i=0; i<MVS_AMR_UL_QUEUE_LEN; i++)
    {
      mvs_amr_init_core_speech((mvs_amr_dl2ul_speech+i),
                               (dl2ul_speech_buffers+i));
    }

    mvs_amr_dl_spi = 0;      mvs_amr_dl_cnt = 0;
    mvs_amr_ul2dl_spi = 0;   mvs_amr_ul2dl_cnt = 0;
    mvs_amr_dl2ul_spi = 0;   mvs_amr_dl2ul_cnt = 0;

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
    /* Reset MVS AMR state machine for frame prediction */
    mvs_amr_state_roll(MVS_AMR_CRC_UNDEF, TRUE);

#ifdef MVSF_WCDMA_AMR_CRC_FRAME_WINDOW
    mvs_amr_consec_crc_cnt = 0;
#endif /* MVSF_WCDMA_AMR_CRC_FRAME_WINDOW */
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

    /* Activate downlink DSM buffers if not in UL2DL loopback modes:
       stop automatic queue emptifying process
    */
    if ((mvs_dsm_loop != MVS_DSM_LOOP_UL2DL_VOC)
        && (mvs_dsm_loop != MVS_DSM_LOOP_UL2DL_DSM))
    {
      mvs_dsm_amr_enable_dldsm(TRUE);
    }
    else
    {
      mvs_dsm_amr_enable_dldsm(FALSE);
    }

    mvs_dsm_amr_state = MVS_DSM_AMR_STATE_ACTIVE;
  }
  else  /* de-activate */
  {
    mvs_dsm_amr_enable_dldsm(FALSE);

    /* Clear out all DSM queues */
    mvs_dsm_amr_q_empty(&mvs_dsm_amr_ul_q);
    mvs_dsm_amr_q_empty(&mvs_dsm_amr_dl_q);
    mvs_dsm_amr_q_empty(&mvs_dsm_amr_ul2dl_q);

    mvs_dsm_loop = MVS_DSM_LOOP_OFF;

    mvs_dsm_amr_state = MVS_DSM_AMR_STATE_INIT;
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_set_loop_mode

DESCRIPTION
  Setup AMR voice packet loop mode:

  UL2DL_VOC: route uplink vocoder packets into downlink
             (loopback without DSM queues),
             continue queuing uplink DSM packets.

  UL2DL_DSM: route uplink DSM packets into downlink
             (loopback through DSM queues),
             continue queuing uplink DSM packets.

  DL2UL: route downlink vocoder packets into uplink,
         continue decoding downlink vocoder packets.

DEPENDENCIES
  mvs_dsm_amr_init has to be called first for initialization.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_set_loop_mode(
  mvs_dsm_loop_type loop_mode
)
{
  int i;


  if (mvs_dsm_amr_state == MVS_DSM_AMR_STATE_RESET)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "AMR DSM buffers have not been " \
         "initialized." );
    return;
  }

  if (loop_mode >= MVS_DSM_LOOP_MAX)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Invalid MVS DSM loop mode " \
           "setting %d",
            loop_mode );
    return;
  }

  if (loop_mode == mvs_dsm_loop)
  {
    return;
  }


  /* Set MVS DSM up in the corresponding loop mode */
  if (mvs_dsm_loop == MVS_DSM_LOOP_OFF)
  {
    switch (loop_mode)
    {
      case MVS_DSM_LOOP_UL2DL_VOC:
      case MVS_DSM_LOOP_UL2DL_DSM:
        mvs_dsm_amr_enable_dldsm(FALSE);
        mvs_dsm_amr_q_empty(&mvs_dsm_amr_dl_q);

        if (loop_mode == MVS_DSM_LOOP_UL2DL_VOC)
        {
          for (i=0; i<MVS_AMR_DL_QUEUE_LEN; i++)
          {
            mvs_amr_dl_speech[i].num_bits_a = 0;
            mvs_amr_dl_speech[i].num_bits_b = 0;
            mvs_amr_dl_speech[i].num_bits_c = 0;
          }

          mvs_amr_dl_spi = 0;
          mvs_amr_dl_cnt = 0;
        }

        /* fall through */

      case MVS_DSM_LOOP_DL2UL:
        mvs_dsm_loop = loop_mode;
        MSG_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"MVS is in loop mode %d",
                                              mvs_dsm_loop);
        break;

      default:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Invalid MVS DSM loop mode " \
               "setting %d",
                loop_mode );
        return;
    }

    return;
  }


  /* Stop the current loop mode, set back to normal mode */
  switch (mvs_dsm_loop)
  {
    case MVS_DSM_LOOP_UL2DL_VOC:
      mvs_dsm_loop = MVS_DSM_LOOP_OFF;

      for (i=0; i<MVS_AMR_DL_QUEUE_LEN; i++)
      {
        mvs_amr_ul2dl_speech[i].num_bits_a = 0;
        mvs_amr_ul2dl_speech[i].num_bits_b = 0;
        mvs_amr_ul2dl_speech[i].num_bits_c = 0;
      }
      mvs_amr_ul2dl_spi = 0;
      mvs_amr_ul2dl_cnt = 0;

      /* packet paths are active. */
      if (mvs_dsm_amr_state != MVS_DSM_AMR_STATE_INIT)
      {
        mvs_dsm_amr_enable_dldsm(TRUE);
      }

      break;

    case MVS_DSM_LOOP_UL2DL_DSM:
      mvs_dsm_loop = MVS_DSM_LOOP_OFF;
      mvs_dsm_amr_q_empty(&mvs_dsm_amr_ul2dl_q);

      /* packet paths are active. */
      if (mvs_dsm_amr_state != MVS_DSM_AMR_STATE_INIT)
      {
        mvs_dsm_amr_enable_dldsm(TRUE);
      }

      break;

    case MVS_DSM_LOOP_DL2UL:
      mvs_dsm_loop = MVS_DSM_LOOP_OFF;

      for (i=0; i<MVS_AMR_UL_QUEUE_LEN; i++)
      {
        mvs_amr_dl2ul_speech[i].num_bits_a = 0;
        mvs_amr_dl2ul_speech[i].num_bits_b = 0;
        mvs_amr_dl2ul_speech[i].num_bits_c = 0;
      }
      mvs_amr_dl2ul_spi = 0;
      mvs_amr_dl2ul_cnt = 0;

      break;

    default:
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Invalid mvs_dsm_loop %d",
                                             mvs_dsm_loop);

      return;
  }


  if (loop_mode == MVS_DSM_LOOP_OFF)
  {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"MVS DSM loop stopped.");
    return;
  }


  /* Change to other loop mode if necessary */
  mvs_dsm_amr_set_loop_mode(loop_mode);
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_ul_proc

DESCRIPTION
  Put AMR packets into uplink DSM queues.  Put packets into loopback buffers
if necessary.

DEPENDENCIES
  mvs_dsm_amr_activate has to be called first for getting into ACTIVE state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_ul_proc(
  amrsup_core_speech_type *ul_speech,
  mvs_amr_chan_state_type chan_state  /* current channel A, B, C setup state */
)
{
  /* to check changes of channels setup */
  static mvs_amr_chan_state_type prev_chan_state;

  mvs_dsm_amr_queue_type *amr_q;
  dsm_item_type* dsm_item_a[2] = {NULL, NULL};
  dsm_item_type* dsm_item_b[2] = {NULL, NULL};
  dsm_item_type* dsm_item_c[2] = {NULL, NULL};
  dword i_sav = 0;
  int qitems_num;
  int spi;
  int i;


  if (mvs_dsm_amr_state < MVS_DSM_AMR_STATE_ACTIVE)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "MVS DSM AMR state %d is not " \
           "active for uplink processing.",
            mvs_dsm_amr_state );
    return;
  }


  /* ============ Dealing with DL2UL loopback ============= */
  if (mvs_dsm_loop == MVS_DSM_LOOP_DL2UL)
  {
    if (mvs_amr_dl2ul_cnt == 0)
    {
      return;
    }

    ul_speech = mvs_amr_dl2ul_speech + mvs_amr_dl2ul_spi;
  }
  else
  if (ul_speech == NULL)
  {
    return;
  }

  /* check if class A data would be lost due to channel A setup */
  if ((chan_state.has_chan_a != TRUE) && (ul_speech->num_bits_a > 0))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Warning loss of uplink class A " \
        "data" );
  }

  /* check if class B data would be lost due to channel B setup */
  if ((chan_state.has_chan_b != TRUE) && (ul_speech->num_bits_b > 0))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Warning loss of uplink class B " \
        "data" );
  }

  /* check if class C data would be lost due to channel C setup */
  if ((chan_state.has_chan_c != TRUE) && (ul_speech->num_bits_c > 0))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Warning loss of uplink class C " \
        "data" );
  }


  /* =========== Uplink waiting state control ============= */
  INTLOCK_SAV(i_sav);

  qitems_num = q_cnt(mvs_dsm_amr_ul_q.wm_a.q_ptr);

  if (mvs_dsm_amr_state == MVS_DSM_AMR_STATE_ACTIVE)
  {
    mvs_dsm_amr_state = MVS_DSM_AMR_STATE_UL_WAIT;

    prev_chan_state = chan_state;   /* initialize for every call */
  }
  else
  if ((mvs_dsm_amr_state == MVS_DSM_AMR_STATE_UL_WAIT)
      && (qitems_num==0))
  {
    mvs_dsm_amr_state = MVS_DSM_AMR_STATE_START;
  }

  if ((mvs_dsm_amr_state == MVS_DSM_AMR_STATE_UL_WAIT) && (qitems_num==1))
  {
    mvs_dsm_amr_q_empty(&mvs_dsm_amr_ul_q);
    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "Lost uplink packet: channels not " \
        "ready." );
  }

  if ((mvs_dsm_amr_state == MVS_DSM_AMR_STATE_START)
      && (qitems_num == MVS_AMR_UL_QUEUE_LEN))
  {
    mvs_dsm_amr_q_empty(&mvs_dsm_amr_ul_q);
    mvs_amr_ul_lost += MVS_AMR_UL_QUEUE_LEN;
    MSG(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"MVS prev. UL pkt. not taken.");
  }

  /* clean up DSM queues if channels setup has been changed
     to avoid packets being out of synchronization between channels
  */
  if (mvs_amr_cmp_chan_state(&chan_state, &prev_chan_state) != TRUE)
  {
    mvs_dsm_amr_q_empty(&mvs_dsm_amr_ul_q);
    mvs_dsm_amr_q_empty(&mvs_dsm_amr_ul2dl_q);
  }


  /* =========== Preparing and queuing DSM packets ============= */
  /* Dealing with UL2DL DSM loopback */
  qitems_num = (mvs_dsm_loop == MVS_DSM_LOOP_UL2DL_DSM)? 2 : 1;

  for (i=0; i<qitems_num; i++)
  {
    /* ----- Preparing class C DSM items if channel C is active ----- */
    if (chan_state.has_chan_c==TRUE)
    {
      if (ul_speech->num_bits_c > 0)
      {
        dsm_item_c[i] = dsm_sdu_to_pkt_chain_bit(ul_speech->data_c,
                                                 (uint16)ul_speech->num_bits_c);
      }
      else
      {
        dsm_item_c[i] = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
      }

      if (dsm_item_c[i] == NULL)
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Can't get DSM item for class C " \
             "AMR data." );
        continue;
      }
    }

    /* ----- Preparing class B DSM items if channel B is active ----- */
    if (chan_state.has_chan_b==TRUE)
    {
      if (ul_speech->num_bits_b > 0)
      {
        dsm_item_b[i] = dsm_sdu_to_pkt_chain_bit(ul_speech->data_b,
                                                 (uint16)ul_speech->num_bits_b);
      }
      else
      {
        dsm_item_b[i] = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
      }

      if (dsm_item_b[i] == NULL)
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Can't get DSM item for class B " \
             "AMR data." );
        if (dsm_item_c[i] != NULL)
        {
          dsm_free_packet(dsm_item_c+i);
          dsm_item_c[i] = NULL;
        }
        continue;
      }
    }

    /* ----- Preparing class A DSM items if channel A is active ----- */
    if (chan_state.has_chan_a==TRUE)
    {
      if (ul_speech->num_bits_a > 0)
      {
        dsm_item_a[i] = dsm_sdu_to_pkt_chain_bit(ul_speech->data_a,
                                                 (uint16)ul_speech->num_bits_a);
      }
      else
      {
        dsm_item_a[i] = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
      }

      if (dsm_item_a[i] == NULL)
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Can't get DSM item for class A " \
             "AMR data." );
        if (dsm_item_b[i] != NULL)
        {
          dsm_free_packet(dsm_item_b+i);
          dsm_item_b[i] = NULL;
        }
        if (dsm_item_c[i] != NULL)
        {
          dsm_free_packet(dsm_item_c+i);
          dsm_item_c[i] = NULL;
        }
        continue;
      }
    }


    /* ---------- Queuing DSM items ---------- */
    /* Dealing with UL2DL DSM loopback */
    amr_q = (i==0)? (&mvs_dsm_amr_ul_q) : (&mvs_dsm_amr_ul2dl_q);

    if (dsm_item_c[i] != NULL)
    {
      dsm_simple_enqueue_isr(&(amr_q->wm_c), (dsm_item_c+i));
    }
    if (dsm_item_b[i] != NULL)
    {
      dsm_simple_enqueue_isr(&(amr_q->wm_b), (dsm_item_b+i));
    }
    if (dsm_item_a[i] != NULL)
    {
      dsm_simple_enqueue_isr(&(amr_q->wm_a), (dsm_item_a+i));
    }
  }  /* end for */

  INTFREE_SAV(i_sav);

  prev_chan_state = chan_state;


  /* =========== Dealing with UL2DL VOC loopback ========== */
  if (mvs_dsm_loop == MVS_DSM_LOOP_UL2DL_VOC)
  {
    if (mvs_amr_ul2dl_cnt > MVS_AMR_DL_QUEUE_LEN)
    {
      ERR_FATAL("UL2DL_VOC count overflow %d", mvs_amr_ul2dl_cnt, 0, 0);
    }

    if (mvs_amr_ul2dl_cnt == MVS_AMR_DL_QUEUE_LEN)
    {
      spi = mvs_amr_ul2dl_spi;
      mvs_amr_ul2dl_spi = (mvs_amr_ul2dl_spi+1) % MVS_AMR_DL_QUEUE_LEN;
      mvs_amr_ul2dl_lost++;
    }
    else
    if (mvs_amr_ul2dl_cnt == 0)
    {
      spi = 0;
      mvs_amr_ul2dl_spi = 0;
      mvs_amr_ul2dl_cnt++;
    }
    else
    {
      spi = (mvs_amr_ul2dl_spi + mvs_amr_ul2dl_cnt)
            % MVS_AMR_DL_QUEUE_LEN;
      mvs_amr_ul2dl_cnt++;
    }

    mvs_amr_copy_core_speech(mvs_amr_ul2dl_speech + spi, ul_speech);
  }


  /* =========== Cleaning up DL2UL loopback buffer ========== */
  if ((mvs_dsm_loop == MVS_DSM_LOOP_DL2UL) && (mvs_amr_dl2ul_cnt > 0))
  {
    mvs_amr_dl2ul_speech[mvs_amr_dl2ul_spi].num_bits_a = 0;
    mvs_amr_dl2ul_speech[mvs_amr_dl2ul_spi].num_bits_b = 0;
    mvs_amr_dl2ul_speech[mvs_amr_dl2ul_spi].num_bits_c = 0;

    mvs_amr_dl2ul_cnt--;

    mvs_amr_dl2ul_spi = (mvs_amr_dl2ul_cnt==0)? 0
                          : ((mvs_amr_dl2ul_spi+1) % MVS_AMR_UL_QUEUE_LEN);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_dl_proc

DESCRIPTION
  Extract and decode AMR packets from downlink DSM queues in normal mode.
  Extract and decode AMR packets from UL2DL DSM queues in UL2DL DSM loopback.
  Decode AMR packets from UL2DL VOC buffer in UL2DL VOC loopback.
  Put AMR packets to be decoded into DL2UL buffer in DL2UL loopback.

DEPENDENCIES
  mvs_dsm_amr_activate has to be called first for getting into ACTIVE state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvs_dsm_amr_dl_proc(
  amrsup_core_speech_type *dl_speech,
  mvs_amr_frame_type *predicted_frame,
  boolean *is_bad_frame
)
{
  mvs_dsm_amr_queue_type *amr_q;
  amrsup_core_speech_type *amr_speech = NULL;
  dsm_item_type *dsm_item;
  dword i_sav = 0;
  int pkt_cnt;
  int tail;
  int spi;

  /* stores bad status bits of new DSM packets. 2:0 -- class C, B, A */
  uint8 crc_valid_bits[MVS_AMR_DL_QUEUE_LEN];
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
  mvs_amr_frame_type predicted_frame_type;
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */


  if (mvs_dsm_amr_state < MVS_DSM_AMR_STATE_ACTIVE)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "MVS DSM AMR state %d is not " \
           "active for downlink processing.",
            mvs_dsm_amr_state );
    return;
  }

  if (dl_speech==NULL)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "NULL error: could not return " \
         "downlink packets." );
    return;
  }

  /* return empty speech if no downlink packet is available */
  dl_speech->num_bits_a = 0;
  dl_speech->num_bits_b = 0;
  dl_speech->num_bits_c = 0;

  /* Undefined frame type indicates no prediction operation.
     Prediction is done only when DSM watermarks are utilised.
  */
  if (predicted_frame!=NULL)
  {
    *predicted_frame = MVS_AMR_FRAME_TYPE_UNDEF;
  }

  if (is_bad_frame!=NULL)
  {
    *is_bad_frame = FALSE;
  }


  /* Dealing with UL2DL VOC loopback */
  if (mvs_dsm_loop == MVS_DSM_LOOP_UL2DL_VOC)
  {
    if (mvs_amr_ul2dl_cnt == 0)
    {
      return;
    }

    mvs_amr_copy_core_speech(dl_speech, mvs_amr_ul2dl_speech
                                        + mvs_amr_ul2dl_spi);
    amr_speech = dl_speech;

    mvs_amr_ul2dl_speech[mvs_amr_ul2dl_spi].num_bits_a = 0;
    mvs_amr_ul2dl_speech[mvs_amr_ul2dl_spi].num_bits_b = 0;
    mvs_amr_ul2dl_speech[mvs_amr_ul2dl_spi].num_bits_c = 0;

    mvs_amr_ul2dl_cnt--;

    mvs_amr_ul2dl_spi = (mvs_amr_ul2dl_cnt==0)? 0
                          : ((mvs_amr_ul2dl_spi+1) % MVS_AMR_DL_QUEUE_LEN);
  }
  else
  {
    /* Dealing with UL2DL DSM loopback */
    amr_q = (mvs_dsm_loop == MVS_DSM_LOOP_UL2DL_DSM)? (&mvs_dsm_amr_ul2dl_q)
                                                    : (&mvs_dsm_amr_dl_q);

    if ((amr_speech==NULL) && (mvs_amr_dl_cnt>0))
    {
      mvs_amr_copy_core_speech(dl_speech, mvs_amr_dl_speech
                                          + mvs_amr_dl_spi);
      amr_speech = dl_speech;

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
      if (predicted_frame!=NULL)
      {
        *predicted_frame = mvs_amr_dl_predict[mvs_amr_dl_spi].predicted_frame;
      }

      if (is_bad_frame!=NULL)
      {
        *is_bad_frame = mvs_amr_dl_predict[mvs_amr_dl_spi].is_bad_frame;
      }
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

      mvs_amr_dl_speech[mvs_amr_dl_spi].num_bits_a = 0;
      mvs_amr_dl_speech[mvs_amr_dl_spi].num_bits_b = 0;
      mvs_amr_dl_speech[mvs_amr_dl_spi].num_bits_c = 0;

      mvs_amr_dl_cnt--;

      mvs_amr_dl_spi = (mvs_amr_dl_cnt==0)? 0
                         : ((mvs_amr_dl_spi+1) % MVS_AMR_DL_QUEUE_LEN);
    }


    /* ========== Extract downlink DSM packets ========== */
    /* The current tail index of DL buffer */
    tail = (mvs_amr_dl_cnt==0)? 0
             : ((mvs_amr_dl_spi + mvs_amr_dl_cnt) % MVS_AMR_DL_QUEUE_LEN);
    pkt_cnt = 0;


    INTLOCK_SAV(i_sav);

    while ((q_cnt(amr_q->wm_a.q_ptr) > 0)
           || (q_cnt(amr_q->wm_b.q_ptr) > 0)
           || (q_cnt(amr_q->wm_c.q_ptr) > 0)
          )
    {
      spi = (tail+pkt_cnt) % MVS_AMR_DL_QUEUE_LEN;

      /* indicate program bug if occurs */
      if (mvs_amr_dl_cnt > MVS_AMR_DL_QUEUE_LEN)
      {
        ERR_FATAL("MVS DSM AMR DL count error %d.", mvs_amr_dl_cnt, 0, 0);
      }

      /* DSM queues overflow: indicate out of synchronization with L1 */
      if (pkt_cnt >= MVS_AMR_DL_QUEUE_LEN)
      {
        MSG_3( MSG_SSID_DFLT,MSG_LEGACY_FATAL,"DL DSM queue overflow %d %d %d", \
                  q_cnt(amr_q->wm_a.q_ptr), \
                  q_cnt(amr_q->wm_b.q_ptr), \
                  q_cnt(amr_q->wm_c.q_ptr) );

        /* discard the remaining PDUs, incomplete reordering will cause
           bad voice quality.
        */
        dsm_empty_queue(&(amr_q->wm_a));
        dsm_empty_queue(&(amr_q->wm_b));
        dsm_empty_queue(&(amr_q->wm_c));

        break;
      }

      /* downlink buffer full, overwrite old packets */
      if ((mvs_amr_dl_cnt + pkt_cnt) == MVS_AMR_DL_QUEUE_LEN)
      {
        mvs_amr_dl_cnt--;
        mvs_amr_dl_spi = (mvs_amr_dl_spi+1) % MVS_AMR_DL_QUEUE_LEN;
        mvs_amr_dl_lost++;

        MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"MVS DSM AMR DL buffer full.");
      }


      /* initialize crc valid bits of new packet */
      crc_valid_bits[spi] = AMRSUP_CLASS_DATA_VALID;

      /* extract class A data */
      dsm_item = dsm_dequeue(&(amr_q->wm_a));
      if (dsm_item==NULL)
      {
        mvs_amr_dl_speech[spi].num_bits_a = 0;
      }
      else
      {
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
        if (dsm_read_crc_status_bit(&dsm_item) != 0)
        {
          crc_valid_bits[spi] &= ~(uint8)AMRSUP_CLASS_DATA_VALID_A;
        }
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

        mvs_amr_dl_speech[spi].num_bits_a
        = (int)dsm_pkt_to_sdu_chain_bit(&dsm_item,
                                        mvs_amr_dl_speech[spi].data_a);
      }

      /* extract class B data */
      dsm_item = dsm_dequeue(&(amr_q->wm_b));
      if (dsm_item==NULL)
      {
        mvs_amr_dl_speech[spi].num_bits_b = 0;
      }
      else
      {
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
        if (dsm_read_crc_status_bit(&dsm_item) != 0)
        {
          crc_valid_bits[spi] &= ~(uint8)AMRSUP_CLASS_DATA_VALID_B;
        }
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

        mvs_amr_dl_speech[spi].num_bits_b
        = (int)dsm_pkt_to_sdu_chain_bit(&dsm_item,
                                        mvs_amr_dl_speech[spi].data_b);
      }

      /* extract class C data */
      dsm_item = dsm_dequeue(&(amr_q->wm_c));
      if (dsm_item==NULL)
      {
        mvs_amr_dl_speech[spi].num_bits_c = 0;
      }
      else
      {
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
        if (dsm_read_crc_status_bit(&dsm_item) != 0)
        {
          crc_valid_bits[spi] &= ~(uint8)AMRSUP_CLASS_DATA_VALID_C;
        }
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

        mvs_amr_dl_speech[spi].num_bits_c
        = (int)dsm_pkt_to_sdu_chain_bit(&dsm_item,
                                        mvs_amr_dl_speech[spi].data_c);
      }

      if (crc_valid_bits[spi] != AMRSUP_CLASS_DATA_VALID)
      {
        MSG_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Bad frame %d %d %d",
                                              mvs_amr_dl_speech[spi].num_bits_a,
                                              mvs_amr_dl_speech[spi].num_bits_b,
                                              mvs_amr_dl_speech[spi].num_bits_c);
      }

      pkt_cnt++;
    }  /* end while loop */


    /* Re-organise extracted downlink packets to be in order */
    if (pkt_cnt > 0)
    {
      mvs_amr_dl_bad
      += mvs_dsm_amr_reorganize_data(mvs_amr_dl_speech, tail, pkt_cnt,
                                     MVS_AMR_DL_QUEUE_LEN, crc_valid_bits);

      if (mvs_amr_dl_cnt==0)
      {
        mvs_amr_dl_spi = tail;
      }

      mvs_amr_dl_cnt += pkt_cnt;

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
      mvs_dsm_amr_predict_dl(crc_valid_bits, tail, pkt_cnt);
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */
    }


    /* need to make sure MVS AMR frame state is fully-updated */
    INTFREE_SAV(i_sav);


    if ( mvs_amr_dl_cnt>0 )
    {
      mvs_amr_copy_core_speech(dl_speech, mvs_amr_dl_speech
                                          + mvs_amr_dl_spi);
      amr_speech = dl_speech;

#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
      if (predicted_frame!=NULL)
      {
        *predicted_frame = mvs_amr_dl_predict[mvs_amr_dl_spi].predicted_frame;
      }

      if (is_bad_frame!=NULL)
      {
        *is_bad_frame = mvs_amr_dl_predict[mvs_amr_dl_spi].is_bad_frame;
      }
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

      mvs_amr_dl_speech[mvs_amr_dl_spi].num_bits_a = 0;
      mvs_amr_dl_speech[mvs_amr_dl_spi].num_bits_b = 0;
      mvs_amr_dl_speech[mvs_amr_dl_spi].num_bits_c = 0;

      mvs_amr_dl_cnt--;

      mvs_amr_dl_spi = (mvs_amr_dl_cnt==0)? 0
                         : ((mvs_amr_dl_spi+1) % MVS_AMR_DL_QUEUE_LEN);
    }


#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
    /* empty buffer and DSM watermarks */
    if (amr_speech==NULL)
    {
      /* run state machine on no data */
      predicted_frame_type = mvs_amr_state_roll(MVS_AMR_CRC_DTX_ONLY, TRUE);

      if (predicted_frame!=NULL)
      {
        *predicted_frame = predicted_frame_type;
      }

      if (is_bad_frame!=NULL)
      {
        *is_bad_frame = FALSE;
      }
    }
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */
  }  /* end else */


  /* Dealing with DL2UL loopback */
  if ((mvs_dsm_loop==MVS_DSM_LOOP_DL2UL) && (amr_speech!=NULL))
  {
    if (mvs_amr_dl2ul_cnt > MVS_AMR_UL_QUEUE_LEN)
    {
      ERR_FATAL("DL2UL count overflow %d", mvs_amr_dl2ul_cnt, 0, 0);
    }

    if (mvs_amr_dl2ul_cnt == MVS_AMR_UL_QUEUE_LEN)
    {
      spi = mvs_amr_dl2ul_spi;
      mvs_amr_dl2ul_spi = (mvs_amr_dl2ul_spi+1) % MVS_AMR_UL_QUEUE_LEN;
      mvs_amr_dl2ul_lost++;
    }
    else if (mvs_amr_dl2ul_cnt == 0)
         {
           spi = 0;
           mvs_amr_dl2ul_spi = 0;
           mvs_amr_dl2ul_cnt++;
         }
         else
         {
           spi = (mvs_amr_dl2ul_spi + mvs_amr_dl2ul_cnt)
                 % MVS_AMR_UL_QUEUE_LEN;
           mvs_amr_dl2ul_cnt++;
         }

    mvs_amr_copy_core_speech(mvs_amr_dl2ul_speech + spi, amr_speech);
  }
}


/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_get_state

DESCRIPTION
  Return the current state of AMR DSM management.

DEPENDENCIES
  None

RETURN VALUE
  mvs_dsm_amr_state

SIDE EFFECTS
  None

===========================================================================*/
mvs_dsm_amr_state_type mvs_dsm_amr_get_state(void)
{
  return mvs_dsm_amr_state;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_get_ul_q

DESCRIPTION
  Return AMR uplink DSM queues.

DEPENDENCIES
  mvs_dsm_amr_init has to be called first.

RETURN VALUE
  &mvs_dsm_amr_ul_q

SIDE EFFECTS
  None

===========================================================================*/
mvs_dsm_amr_queue_type* mvs_dsm_amr_get_ul_q(void)
{
  if (mvs_dsm_amr_state == MVS_DSM_AMR_STATE_RESET)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "AMR DSM buffers have not been " \
         "initialized." );
    return NULL;
  }

  return (&mvs_dsm_amr_ul_q);
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_dsm_amr_get_dl_q

DESCRIPTION
  Return AMR downlink DSM queues.

DEPENDENCIES
  mvs_dsm_amr_init has to be called first.

RETURN VALUE
  &mvs_dsm_amr_dl_q

SIDE EFFECTS
  None

===========================================================================*/
mvs_dsm_amr_queue_type* mvs_dsm_amr_get_dl_q(void)
{
  if (mvs_dsm_amr_state == MVS_DSM_AMR_STATE_RESET)
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "AMR DSM buffers have not been " \
         "initialized." );
    return NULL;
  }

  return (&mvs_dsm_amr_dl_q);
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvs_clear_ul_dsm_amr_queue

DESCRIPTION
  Clears Ulink DSM queue. It prevents increase in RTD, by clearing UL DSM 
  Queue. It should be called before queuing first vocoder packet after silence frames. 

DEPENDENCIES
  mvs_dsm_amr_init has to be called first.

RETURN VALUE
  void

SIDE EFFECTS
  It has to be carefully called, ONLY when silence frame is/are
  present in UL DSM Queue.   
===========================================================================*/
void mvs_clear_ul_dsm_amr_queue( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "MAKING AMR DSM UL QUEUE EMPTY" );
  mvs_dsm_amr_q_empty( &mvs_dsm_amr_ul_q );
}

#endif /* !WINSIM */

