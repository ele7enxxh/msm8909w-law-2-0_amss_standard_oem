/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                 S U P P L E M E N T A R Y   D E F I N I T I O N S

DESCRIPTION
  This file contains supplementary functions and data definitions for
multimode voice services.

REFERENCES
  None.

Copyright (C) 2002-2014 QUALCOMM Technologies Incorporated
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/src/mvssup.c#1 $
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
03/13/12    at     Merged in TD-SCDMA support.
07/22/09    at     Added more debug messages.
06/05/09    at     Added AMR-WB support and fixed a compiler warning in
                   mvssup_get_ul_amr_mode().
05/15/09   ymc     Added history section.

===========================================================================*/

#include <string.h>

#include "msg.h"

#include "mvs.h"
#include "mvs_module.h"
#include "mvssup.h"
#include "mvs_umts_chan_setup.h"
#include "mmstd.h"

#define USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK
#ifdef USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK
  #include "queue.h"
#endif /* USE_WCDMAMVSIF_CMI_NONCOMPLIANCE_HACK */
#include "wcdmamvsif.h"

/*===========================================================================

FUNCTION mvssup_frame_q_init

DESCRIPTION
  This function will initialize MVS voice frame queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_frame_q_init(
  mvs_frame_q_type *frame_q_ptr,
  mvs_frame_type *frames_buf,
  int size
)
{
  if ((frame_q_ptr==NULL) || (frames_buf==NULL) || (size<=0))
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "MVS frame q init failed %d",
           size );
    return;
  }


  frame_q_ptr->frame_q = frames_buf;
  frame_q_ptr->q_size = size;
  frame_q_ptr->frame_cnt = 0;
  frame_q_ptr->curr_frame = 0;
  frame_q_ptr->miss_cnt = 0;
  frame_q_ptr->lost_cnt = 0;
}

/*===========================================================================

FUNCTION mvssup_frame_q_empty

DESCRIPTION
  This function will emptify MVS voice frame queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_frame_q_empty(
  mvs_frame_q_type *frame_q_ptr
)
{
  if (frame_q_ptr!=NULL)
  {
    frame_q_ptr->frame_cnt = 0;
    frame_q_ptr->curr_frame = 0;
  }
}

/*===========================================================================

FUNCTION mvssup_frame_check_q_full

DESCRIPTION
  This function will check if MVS voice frame queue is full or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mvssup_frame_check_q_full(
  mvs_frame_q_type *frame_q_ptr
)
{
  if ((frame_q_ptr==NULL) || (frame_q_ptr->frame_q==NULL)
      || (frame_q_ptr->q_size <= 0))
  {
    return TRUE;
  }

  if (frame_q_ptr->frame_cnt >= frame_q_ptr->q_size)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION mvssup_frame_get_q_size

DESCRIPTION
  This function will return the size of the queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_frame_get_q_size(
  mvs_frame_q_type *frame_q_ptr
)
{
  if ((frame_q_ptr==NULL) || (frame_q_ptr->frame_q==NULL)
      || (frame_q_ptr->q_size <= 0))
  {
    return 0;
  }

  return frame_q_ptr->q_size;

}

/*===========================================================================

FUNCTION mvssup_frame_get_q_frame_cnt

DESCRIPTION
  This function will return the no of frame in the queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_frame_get_q_frame_cnt(
  mvs_frame_q_type *frame_q_ptr
)
{
  if ((frame_q_ptr==NULL) || (frame_q_ptr->frame_q==NULL)
      || (frame_q_ptr->q_size <= 0))
  {
    return 0;
  }

  return frame_q_ptr->frame_cnt;

}

/*===========================================================================

FUNCTION mvssup_frame_check_q_empty

DESCRIPTION
  This function will check if MVS voice frame queue is empty or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean mvssup_frame_check_q_empty(
  mvs_frame_q_type *frame_q_ptr
)
{
  if ((frame_q_ptr==NULL) || (frame_q_ptr->frame_q==NULL)
      || (frame_q_ptr->q_size <= 0))
  {
    return TRUE;
  }

  if (frame_q_ptr->frame_cnt <= 0)
  {
    return TRUE;
  }

  return FALSE;
}

/*===========================================================================

FUNCTION mvssup_frame_q_put

DESCRIPTION
  This function will put a frame into the queue.

DEPENDENCIES
  mvssup_frame_q_init must be called first to initialize the queue.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mvs_pkt_status_type mvssup_frame_q_put(
  mvs_frame_q_type *frame_q_ptr,
  uint8 *frame_data,
  mvs_frame_info_type *frame_info,
  uint16 frame_len
)
{
  mvs_pkt_status_type status = MVS_PKT_STATUS_NORMAL;
  int tail;


  if ( frame_len > MVS_MAX_PKT_SIZE )
  {
    frame_len = MVS_MAX_PKT_SIZE;
  }


  if ((frame_q_ptr==NULL) || (frame_data==NULL) || (frame_info==NULL))
  {
    return MVS_PKT_STATUS_SLOW;    /* slow status indicates program error */
  }

  if ((frame_q_ptr->frame_q == NULL) || (frame_q_ptr->q_size <= 0))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "MVS frame queue needs to be " \
         "initialised." );
    return MVS_PKT_STATUS_SLOW;    /* slow status indicates program error */
  }

  if (frame_q_ptr->frame_cnt > frame_q_ptr->q_size)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, \
           "Unexpected MVS frame queue error %d %d !!", \
            frame_q_ptr->frame_cnt, frame_q_ptr->q_size );
    return MVS_PKT_STATUS_SLOW;
  }


  if (frame_q_ptr->frame_cnt == frame_q_ptr->q_size)
  {
    frame_q_ptr->frame_cnt--;

    if (frame_q_ptr->frame_cnt == 0)
    {
      frame_q_ptr->curr_frame = 0;
    }
    else
    {
      frame_q_ptr->curr_frame = (frame_q_ptr->curr_frame + 1)
                                % (frame_q_ptr->q_size);
    }

    frame_q_ptr->lost_cnt++;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "MVS frame queue overflow %x %d " \
          "%d",(uint32)(frame_q_ptr->frame_q), \
          frame_q_ptr->frame_cnt, frame_q_ptr->lost_cnt );

    status = MVS_PKT_STATUS_FAST;
  }


  tail = (frame_q_ptr->curr_frame + frame_q_ptr->frame_cnt)
         % (frame_q_ptr->q_size);

  mmstd_memcpy((frame_q_ptr->frame_q + tail)->frame_data,
                MVS_MAX_PKT_SIZE,
                frame_data,
                frame_len);
  (frame_q_ptr->frame_q + tail)->frame_info = *frame_info;
  (frame_q_ptr->frame_q + tail)->frame_len = frame_len;

  frame_q_ptr->frame_cnt++;

  return status;
}

/*===========================================================================

FUNCTION mvssup_frame_q_get

DESCRIPTION
  This function will get a frame from the queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mvs_pkt_status_type mvssup_frame_q_get(
  mvs_frame_q_type *frame_q_ptr,
  uint8 *frame_data,
  mvs_frame_info_type *frame_info,
  uint16 *frame_len
)
{
  if ((frame_q_ptr==NULL) || (frame_data==NULL) || (frame_info==NULL))
  {
    return MVS_PKT_STATUS_FAST;   /* fast status indicates program error */
  }

  if ((frame_q_ptr->frame_q == NULL) || (frame_q_ptr->q_size <= 0))
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "MVS frame queue needs to be " \
         "initialised." );
    return MVS_PKT_STATUS_FAST;   /* fast status indicates program error */
  }

  if (frame_q_ptr->frame_cnt < 0)
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL, \
           "Unexpected MVS frame queue error %d %d !!", \
           frame_q_ptr->frame_cnt, frame_q_ptr->q_size );
    return MVS_PKT_STATUS_FAST;
  }


  if (frame_q_ptr->frame_cnt == 0)
  {
    frame_q_ptr->miss_cnt++;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "MVS frame queue underflow %x %d " \
          "%d",(uint32)(frame_q_ptr->frame_q), \
          frame_q_ptr->frame_cnt, frame_q_ptr->miss_cnt );

    return MVS_PKT_STATUS_SLOW;
  }


  mmstd_memcpy( frame_data,
                MVS_MAX_PKT_SIZE,
                (frame_q_ptr->frame_q + frame_q_ptr->curr_frame)->frame_data,
                (frame_q_ptr->frame_q + frame_q_ptr->curr_frame)->frame_len);
  *frame_info = (frame_q_ptr->frame_q + frame_q_ptr->curr_frame)->frame_info;
  if (frame_len != NULL)
  {
    *frame_len = (frame_q_ptr->frame_q + frame_q_ptr->curr_frame)->frame_len;
  }

  frame_q_ptr->frame_cnt--;

  if (frame_q_ptr->frame_cnt == 0)
  {
    frame_q_ptr->curr_frame = 0;
  }
  else
  {
    frame_q_ptr->curr_frame = (frame_q_ptr->curr_frame + 1)
                              % (frame_q_ptr->q_size);
  }

  return MVS_PKT_STATUS_NORMAL;
}

/*===========================================================================

FUNCTION mvssup_wcdma_set_ul_channel

DESCRIPTION
  This function sets the uplink channels used to transport vocoder data from
MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mvs_ul_wcdma_chan_state will be updated.

===========================================================================*/
void mvssup_wcdma_set_ul_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
)
{
#ifndef WINSIM
  (void) mvs_wcdma_set_ul_channel(lc_class_a, lc_class_b, lc_class_c, (mvssup_umts_chan_class_type)chan_class);
#endif /* WINSIM */
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_wcdma_set_ul_channel(): " \
         "lc_class_a %d, lc_class_b %d, lc_class_c %d",
         lc_class_a, lc_class_b, lc_class_c );
  /*@todo use legacy code here*/
}

/*===========================================================================

FUNCTION mvssup_wcdma_set_dl_channel

DESCRIPTION
  This function sets the downlink channels used to transport vocoder data to
MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mvs_dl_wcdma_chan_state will be updated.

===========================================================================*/
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
void mvssup_wcdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class,
  mvssup_wcdma_blk_size_cb_set_type *blk_size_cb_set
)
{
#ifndef WINSIM
 (void) mvs_wcdma_set_dl_channel(lc_class_a, lc_class_b, lc_class_c, (mvssup_umts_chan_class_type)chan_class, (mvssup_wcdma_blk_size_cb_set_type*)blk_size_cb_set);
#endif /* WINSIM */
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_wcdma_set_dl_channel(): " \
         "lc_class_a %d, lc_class_b %d, lc_class_c %d",
         lc_class_a, lc_class_b, lc_class_c );
  /*@todo use legacy code here*/
}
#else
void mvssup_wcdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
)
{
#ifndef WINSIM
 (void) mvs_wcdma_set_dl_channel(lc_class_a, lc_class_b, lc_class_c, (mvssup_umts_chan_class_type)chan_class, NULL);
#endif /* WINSIM */
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_wcdma_set_dl_channel(): " \
         "lc_class_a %d, lc_class_b %d, lc_class_c %d",
         lc_class_a, lc_class_b, lc_class_c );
  /*@todo use legacy code here*/
}
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

/*===========================================================================

FUNCTION mvssup_tdscdma_set_ul_channel

DESCRIPTION
  This function sets the uplink channels used to transport vocoder data from
MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mvs_ul_umts_chan_state will be updated.

===========================================================================*/
void mvssup_tdscdma_set_ul_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
)
{
#ifndef WINSIM
  (void) mvs_tdscdma_set_ul_channel(lc_class_a,lc_class_b,lc_class_c,chan_class);
#endif /* WINSIM */
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_tdscdma_set_ul_channel(): " \
         "lc_class_a %d, lc_class_b %d, lc_class_c %d",
         lc_class_a, lc_class_b, lc_class_c );
  /*@todo use legacy code here*/
}

/*===========================================================================

FUNCTION mvssup_tdscdma_set_dl_channel

DESCRIPTION
  This function sets the downlink channels used to transport vocoder data to
MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mvs_dl_umts_chan_state will be updated.

===========================================================================*/
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
void mvssup_tdscdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class,
  mvssup_umts_blk_size_cb_set_type *blk_size_cb_set
)
{
#ifndef WINSIM
 (void) mvs_tdscdma_set_dl_channel(lc_class_a, lc_class_b, lc_class_c, chan_class, blk_size_cb_set);
#endif /* WINSIM */
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_tdscdma_set_dl_channel(): " \
         "lc_class_a %d, lc_class_b %d, lc_class_c %d",
         lc_class_a, lc_class_b, lc_class_c );
  /*@todo use legacy code here*/
}
#else
void mvssup_tdscdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
)
{
#ifndef WINSIM
 (void) mvs_tdscdma_set_dl_channel(lc_class_a, lc_class_b, lc_class_c, chan_class, NULL);
#endif /* WINSIM */
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_tdscdma_set_dl_channel(): " \
         "lc_class_a %d, lc_class_b %d, lc_class_c %d",
         lc_class_a, lc_class_b, lc_class_c );
  /*@todo use legacy code here*/
}
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

/*===========================================================================

FUNCTION mvssup_wcdma_ul_processing

DESCRIPTION
  This function processes uplink data and transports it over the DSM queue
initialized in the mvssup_wcdma_set_ul_channel call.

DEPENDENCIES
  mvssup_wcdma_set_ul_channel has to be called first to initialize uplink
channels.

RETURN VALUE
  None

SIDE EFFECTS
  None

COMMENTS
  This function is DEPRECATED. Use mvssup_umts_ul_processing.

===========================================================================*/
void mvssup_wcdma_ul_processing(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
#ifndef WINSIM
   (void) mvs_umts_ul_processing(vocoder_packet, frame_info);
#endif /* WINSIM */
}

/*===========================================================================

FUNCTION mvssup_wcdma_dl_processing

DESCRIPTION
  This function retrieves downlink data from the DSM queue initialized in the
mvssup_wcdma_set_dl_channel call and processes it for vocoder use.

DEPENDENCIES
  mvssup_wcdma_set_dl_channel has to be called first to initialize downlink
channels.

RETURN VALUE
  None

SIDE EFFECTS
  None

COMMENTS
  This function is DEPRECATED. Use mvssup_umts_dl_processing.

===========================================================================*/
void mvssup_wcdma_dl_processing(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
#ifndef WINSIM
  (void) mvs_umts_dl_processing(vocoder_packet, frame_info);
#endif /* WINSIM */
}

/*===========================================================================

FUNCTION mvssup_umts_ul_processing

DESCRIPTION
  This function processes uplink data and transports it over the DSM queue
initialized in the mvssup_wcdma_set_ul_channel or
mvssup_tdscdma_set_ul_channel.

DEPENDENCIES
  mvssup_wcdma_set_ul_channel or mvssup_tdscdma_set_ul_channel has to be
called first to initialize uplink channels.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_umts_ul_processing(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
#ifndef WINSIM
   (void) mvs_umts_ul_processing(vocoder_packet, frame_info);
#endif /* WINSIM */
}

/*===========================================================================

FUNCTION mvssup_umts_dl_processing

DESCRIPTION
  This function retrieves downlink data from the DSM queue initialized in the
mvssup_wcdma_set_dl_channel or mvssup_tdscdma_set_dl_channel and processes it
for vocoder use.

DEPENDENCIES
  mvssup_wcdma_set_dl_channel or mvssup_tdscdma_set_dl_channel has to be
called first to initialize downlink channels.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_umts_dl_processing(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
#ifndef WINSIM
  (void) mvs_umts_dl_processing(vocoder_packet, frame_info);
#endif /* WINSIM */
}

/*===========================================================================

FUNCTION mvssup_wcdma_encode_amrif2

DESCRIPTION
  This function processes a vocoder packet and builds an AMR IF2 packet.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes of encoded frame.

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_wcdma_encode_amrif2(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info,
  uint8 *if2_frame
)
{
  return -1;
}

/*===========================================================================

FUNCTION mvssup_wcdma_decode_amrif2

DESCRIPTION
  This function processes an AMR IF2 packet and builds a vocoder packet.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes of decoded frame.

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_wcdma_decode_amrif2(
  uint8 *if2_frame,
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
  return -1;
}

/*===========================================================================

FUNCTION mvssup_voc_set_frame_rate

DESCRIPTION
  This function sets the 1x Max rate and Min rate

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_voc_set_frame_rate(
  mvs_voc_rate_type max_rate,
  mvs_voc_rate_type min_rate
)
{
  (void) mvs_mod_voc_set_frame_rate(max_rate, min_rate);
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_voc_set_frame_rate(): " \
         "max_rate %d, min_rate %d",
         max_rate, min_rate );
}

/*===========================================================================

FUNCTION mvssup_amr_set_scr_mode

DESCRIPTION
  This function sets  DTX mode for WCDMA

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_amr_set_scr_mode(
  mvs_scr_mode_type scr_mode
)
{
   (void) mvs_mod_amr_set_scr_mode(scr_mode);
   MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_amr_set_scr_mode(): scr_mode " \
          "%d",  scr_mode );
}

/*===========================================================================

FUNCTION mvssup_gsm_set_dtx_mode

DESCRIPTION
  This function sets the DTX mode of a GSM vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_gsm_set_dtx_mode(
  mvs_dtx_mode_type dtx_mode
)
{
  (void) mvs_mod_set_dtx_mode(dtx_mode) ;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_gsm_set_dtx_mode(): dtx_mode " \
         "%d", dtx_mode );
}

/*===========================================================================

FUNCTION mvssup_set_dtx_mode

DESCRIPTION
  This function sets the DTX mode of a vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_set_dtx_mode(
  mvs_dtx_mode_type dtx_mode
)
{
  (void) mvs_mod_set_dtx_mode(dtx_mode) ;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,"mvssup_set_dtx_mode(): dtx_mode %d",
         dtx_mode );
}

/*===========================================================================

FUNCTION mvssup_amr_set_amr_mode

DESCRIPTION
  This function sets the AMR transfer mode of an AMR vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_amr_set_amr_mode(
  mvs_amr_mode_type amr_ul_mode
)
{
  if (amr_ul_mode > MVS_AMR_MODE_1220)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvssup_amr_set_amr_mode(): " \
           "Invalid AMR mode: %d",
            amr_ul_mode );
    return;
  }

  (void) mvs_mod_amr_set_amr_mode(amr_ul_mode);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_amr_set_amr_mode(): " \
         "amr_ul_mode %d",
         amr_ul_mode );
}

/*===========================================================================

FUNCTION mvssup_amr_set_awb_mode

DESCRIPTION
  This function sets the AMR-WB transfer mode of an AMR-WB vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mvssup_amr_set_awb_mode(
  mvs_amr_mode_type amr_ul_mode
)
{
  if (amr_ul_mode < MVS_AMR_MODE_0660 || amr_ul_mode > MVS_AMR_MODE_2385)
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "mvssup_amr_set_awb_mode(): " \
           "Invalid AMR-WB mode: %d",
           amr_ul_mode );
    return;
  }

  (void) mvs_mod_amr_set_awb_mode(amr_ul_mode);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_amr_set_awb_mode(): " \
         "amr_ul_mode %d",
         amr_ul_mode );
}

/*===========================================================================

FUNCTION mvssup_get_ul_amr_mode

DESCRIPTION
  This function returns currently configured uplink AMR mode.

DEPENDENCIES
  None

RETURN VALUE
  mvs_ul_amr_mode

SIDE EFFECTS
  None

===========================================================================*/
mvs_amr_mode_type mvssup_get_ul_amr_mode(void)
{
   MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"mvssup_get_ul_amr_mode()" );
   return mvs_get_ul_amr_mode();
}

/*===========================================================================

FUNCTION mvssup_get_ul_awb_mode

DESCRIPTION
  This function returns currently configured uplink AMR-WB mode.

DEPENDENCIES
  None

RETURN VALUE
  mvs_ul_amr_mode

SIDE EFFECTS
  None

===========================================================================*/
mvs_amr_mode_type mvssup_get_ul_awb_mode(void)
{
   MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"mvssup_get_ul_awb_mode()" );
   return mvs_get_ul_awb_mode();
}

void mvssup_g729a_set_dtx_mode(
  mvs_dtx_mode_type dtx_mode
)
{
  mvs_mod_set_dtx_mode( dtx_mode );
}

void mvssup_g711a_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
)
{
  mvs_mod_set_dtx_mode( dtx_mode );
}

mvs_g711_mode_type mvssup_get_g711_mode ( void )
{
  return mvs_mod_get_g711_mode( );
}

void mvssup_set_g711_mode (
  mvs_g711_mode_type mvs_g711_mode
)
{
  mvs_mod_set_g711_mode( mvs_g711_mode );
}

mvs_g711a_mode_type mvssup_get_g711a_mode ( void )
{
  return map_g711_mode_to_g711a( mvs_mod_get_g711_mode( ) );
}

void mvssup_set_g711a_mode (
  mvs_g711a_mode_type mvs_g711a_mode
)
{
  mvs_mod_set_g711_mode( map_g711a_mode_to_g711( mvs_g711a_mode ) );
}

mvs_g722_mode_type mvssup_get_g722_mode ( void )
{
  return ( mvs_mod_get_g722_mode() );
}

void mvssup_set_g722_mode (
  mvs_g722_mode_type mvs_g722_mode
)
{
  mvs_mod_set_g722_mode(mvs_g722_mode);
}


void mvssup_voc_tx_rate_limit (
  uint16 rate_factor
)
{
  ( void ) mvs_mod_voc_tx_rate_limit( rate_factor );
}

mvs_voc_rate_type mvssup_get_voc_max_rate( void )
{
  return mvs_mod_get_voc_max_rate();
}

mvs_voc_rate_type mvssup_get_voc_min_rate( void )
{
  return mvs_mod_get_voc_min_rate();
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_umts_encode_amrif1

DESCRIPTION
  This function processes a vocoder packet and builds an AMR IF1 packet.

PARAMETERS
  [in/xxx] vocoder_packet : the pointer to the vocoder packet to encode.
  [in/xxx] frame_info     : the pointer to an amr_rate uplink info that
                            describes the vocoder_packet.
  [xx/out] if1_frame      : the pointer to the encoded AMR IF1 frame.
  [xx/out] if1_frame_info : the pointer to an amr_if1_info uplink info that
                            describes the if1_frame.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes of encoded frame.
  if1_frame : IF1-encoded frame.
  if1_frame_info : holds frame information of IF1-encoded frame.

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_umts_encode_amrif1(
  uint8               *vocoder_packet,
  mvs_frame_info_type *frame_info,
  uint8               *if1_frame,
  mvs_frame_info_type *if1_frame_info
)
{

/* function is hacked up to work for 8k may need to featurize per target in the future */

#if defined(FEATURE_AMR_VOCODER)
  mvs_amr_frame_type          frame_type_in;
  mvs_amr_mode_type           frame_rate_in;
  amrsup_if1_frame_info_type  frame_info_out;
  int                         bytes_encoded;


  /* abort if any in/out pointer is NULL */
  if ((vocoder_packet == NULL) || (frame_info == NULL) ||
      (if1_frame == NULL) || (if1_frame_info == NULL)) {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Invalid NULL parameters.");
    return 0;
  }

  if ((frame_info->hdr.mode != MVS_MODE_AMR)
      && (frame_info->hdr.mode != MVS_MODE_AMR_WB))
  {
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,   "Invalid MVS mode %d for AMR " \
            "IF1 encoding.",
            frame_info->hdr.mode );
    return 0;
  }

  /* abort if the requested TX_TYPE is invalid */
  if (MVS_AMR_FRAME_TYPE_MAX <= frame_info->amr_rate.ul_info.frame) {
	MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"Invalid Frame Type (TX_TYPE) value.");
    return 0;
  }

  /* abort if the requested Mode Indication is invalid */
  if ( MVS_AMR_MODE_MAX <= frame_info->amr_rate.ul_info.mode ) {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Invalid Frame Rate (Mode Indication) " \
         "value." );
    return 0;
  }

  /* encode the voice packet into the AMR IF1 Class A, B, & C format */
  frame_type_in = (mvs_amr_frame_type)MVS_AMR_TO_AMRSUP_FRAME(frame_info->amr_rate.ul_info.frame);
  frame_rate_in = (mvs_amr_mode_type)MVS_AMR_TO_AMRSUP_MODE(frame_info->amr_rate.ul_info.mode);

  bytes_encoded =
    amrsup_if1_framing(vocoder_packet,
                       (amrsup_frame_type)frame_type_in,
                       (amrsup_mode_type)frame_rate_in,
                       if1_frame,
                       &frame_info_out);
  if (frame_info_out.amr_type == AMRSUP_CODEC_AMR_NB)
  {
    if1_frame_info->amr_if1_info.ul_info.frame_type_index =
      AMRSUP_TO_MVS_AMR_FRAME_TYPE_INDEX(frame_info_out.frame_type_index);
    if1_frame_info->hdr.mode       = MVS_MODE_AMR;
    if1_frame_info->hdr.frame_mode = MVS_FRAME_MODE_AMR_IF1_UL;
  }
  else
  {
#if defined(FEATURE_VOC_AMR_WB)
    if1_frame_info->amr_if1_info.ul_info.frame_type_index =
      AMRSUP_TO_MVS_AWB_FRAME_TYPE_INDEX(frame_info_out.frame_type_index);
    if1_frame_info->hdr.mode       = MVS_MODE_AMR_WB;
    if1_frame_info->hdr.frame_mode = MVS_FRAME_MODE_AMR_IF1_UL;
#endif /*#defined(FEATURE_VOC_AMR_WB)*/
  }
  if1_frame_info->amr_if1_info.ul_info.fqi = frame_info_out.fqi;

  /*copy vocoder in if1 format to if1_frame */
   mmstd_memcpy(if1_frame, bytes_encoded, vocoder_packet, bytes_encoded);

  return bytes_encoded;
#else
  MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"AMR IF1 mode not supported.");
  return 0;
#endif /* FEATURE_AMR_VOCODER */
}

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_umts_decode_amrif1

DESCRIPTION
  This function processes an AMR IF1 packet and builds a vocoder packet.

PARAMETERS
  [in/xxx] if1_frame      : the pointer to the AMR IF1 frame to decode.
  [in/xxx] if1_frame_info : the pointer to an amr_if1_info downlink info that
                            describes the if1_frame.
  [xx/out] vocoder_packet : the pointer to the decoded vocoder packet.
  [xx/out] frame_info     : the pointer to an amr_rate downlink info that
                            describes the vocoder_packet.

DEPENDENCIES
  None

RETURN VALUE
  number of bytes of decoded frame.
  vocoder_packet : decoded packet
  frame_info : holds frame information of decoded frame

SIDE EFFECTS
  None

===========================================================================*/
int mvssup_umts_decode_amrif1(
  uint8               *if1_frame,
  mvs_frame_info_type *if1_frame_info,
  uint8               *vocoder_packet,
  mvs_frame_info_type *frame_info
)
{
/* function is hacked up to work for 8k may need to featurize per target in the future */

#if defined(FEATURE_AMR_VOCODER)
  amrsup_if1_frame_info_type  frame_info_in;
  amrsup_frame_type           frame_type_out;
  amrsup_mode_type            frame_rate_out;
  int                         bytes_decoded;


  /* abort if any in/out pointer is NULL */
  if ((if1_frame == NULL) || (if1_frame_info == NULL) ||
      (vocoder_packet == NULL) || (frame_info == NULL)) {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Invalid NULL parameters.");
    return 0;
  }

  /* abort if the requested TX_TYPE is invalid */
  if (MVS_AMR_FRAME_TYPE_INDEX_MAX <=
      if1_frame_info->amr_if1_info.dl_info.frame_type_index) {
    MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Invalid Frame Type Index value.");
    return 0;
  }

  /* decode the AMR IF1 Class A, B, & C format into the voice packet */
  if (if1_frame_info->hdr.mode == MVS_MODE_AMR)
  {
    frame_info_in.amr_type = AMRSUP_CODEC_AMR_NB;
    frame_info_in.frame_type_index =
    MVS_AMR_TO_AMRSUP_FRAME_TYPE_INDEX( \
      if1_frame_info->amr_if1_info.dl_info.frame_type_index);

    frame_info->hdr.mode       = MVS_MODE_AMR;
    frame_info->hdr.frame_mode = MVS_FRAME_MODE_AMR_IF1_DL;
  }
#if defined(FEATURE_VOC_AMR_WB)
  else if (if1_frame_info->hdr.mode == MVS_MODE_AMR_WB)
  {
    frame_info_in.amr_type = AMRSUP_CODEC_AMR_WB;

    frame_info_in.frame_type_index =
      MVS_AWB_TO_AMRSUP_FRAME_TYPE_INDEX( \
        if1_frame_info->amr_if1_info.dl_info.frame_type_index);

    frame_info->hdr.mode       = MVS_MODE_AMR_WB;
    frame_info->hdr.frame_mode = MVS_FRAME_MODE_AMR_IF1_DL;
  }
#endif /* FEATURE_VOC_AMR_WB */
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Invalid MVS mode %d for AMR IF1 " \
           "decoding.",
           if1_frame_info->hdr.mode);
    return 0;
  }

  frame_info_in.fqi = if1_frame_info->amr_if1_info.dl_info.fqi;

  bytes_decoded =
    amrsup_if1_de_framing(vocoder_packet,
                          &frame_type_out,
                          &frame_rate_out,
                          if1_frame,
                          &frame_info_in);
  if ( frame_type_out < AMRSUP_FRAME_TYPE_MAX )
  {
    frame_info->amr_rate.dl_info.frame = AMRSUP_TO_MVS_AMR_FRAME(frame_type_out);
  }
  else
  {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_umts_decode_amrif1 :- DL " \
             "frame type = %d",
             frame_type_out );
  }

  if ( frame_rate_out < AMRSUP_MODE_MAX )
  {
    frame_info->amr_rate.dl_info.mode = AMRSUP_TO_MVS_AMR_MODE(frame_rate_out);
  }
  else
  {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "mvssup_umts_decode_amrif1 :- DL " \
             "frame mode= %d",
             frame_rate_out );
  }

  /* vocoder_packet in little-endian format, length must be even. */
  if (bytes_decoded >= 0) {
    bytes_decoded = (int) (((unsigned int) bytes_decoded+1) >> 1) << 1;
  }

   /*copy if1_frame to vocoder packet*/
   mmstd_memcpy(vocoder_packet, bytes_decoded, if1_frame, bytes_decoded);

  return bytes_decoded;
#else
  MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR,"AMR IF1 mode not supported.");
  return 0;
#endif /* FEATURE_AMR_VOCODER */
}


