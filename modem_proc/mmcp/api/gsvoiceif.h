#ifndef _GSVOICEIF_H_
#define _GSVOICEIF_H_
/*==============================================================================

      GS Voice library interface header file

DESCRIPTION

  This header file contains the API function declarations of the functions that the gsvoice library provides.
  All the modem modules which require the services of MVS (multi vocoder services) have to use the 
  appropriate APIs provided by the gsvoice library.

         COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
==============================================================================*/

/*==============================================================================
         EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_plmn.h_v   1.1   28 May 2002 11:11:20   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/gsvoiceif.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when         who     what, where, why
--------   ---     -------------------------------------------------------------
12/01/11   vsm     Initial version.

==============================================================================*/

/*==============================================================================

                          INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mvs.h"


/*==============================================================================

                             DATA DECLARATIONS

==============================================================================*/



/*==============================================================================

                           FUNCTION DECLARATIONS

==============================================================================*/

void ghdi_gsm_set_dtx_mode(mvs_dtx_mode_type dtx_mode);


/*#ifdef FEATURE_CS_VOICE_OVER_HSPA */
/*void ghdi_mvs_enable(mvs_client_type client, mvs_mode_type mode, umts_wcdma_cs_voice_bearer_enum_T rab_type); */
/*#else */
void ghdi_mvs_enable(mvs_client_type client, mvs_mode_type mode);
/*#endif */


void ghdi_mvs_release(mvs_client_type client);
void ghdi_mvs_acquire(mvs_client_type client);
void ghdi_mvs_standby(mvs_client_type client, mvs_mode_type mode);

void ghdi_mvs_wcdma_set_ul_channel(rlc_lc_id_type lc_class_a,
                                   rlc_lc_id_type lc_class_b,
                                   rlc_lc_id_type lc_class_c,
                                   mvssup_wcdma_chan_class_type chan_class
                                  );

void ghdi_mvs_wcdma_set_dl_channel(rlc_lc_id_type lc_class_a,
                                   rlc_lc_id_type lc_class_b,
                                   rlc_lc_id_type lc_class_c,
                                   mvssup_wcdma_chan_class_type chan_class
                                  );

void ghdi_mvs_amr_set_awb_mode(mvs_amr_mode_type amr_ul_mode);
void ghdi_mvs_amr_set_amr_mode(mvs_amr_mode_type amr_ul_mode);
void ghdi_mvs_amr_set_scr_mode(mvs_scr_mode_type scr_mode);

mvs_amr_mode_type ghdi_mvs_get_ul_awb_mode(void);
mvs_amr_mode_type ghdi_mvs_get_ul_amr_mode(void);


boolean ghdi_is_acquisition_in_progress(void);

/*ghdi_status_T ghdi_control_tch_loopback(loopback_type_T  loopback_type,
                                         tch_T            tch );
*/

void ghdi_amr_dl_cb(uint8               *vocoder_packet,
                    mvs_frame_info_type *frame_info,
                    mvs_pkt_status_type *status);

void ghdi_amr_ul_cb(uint8               *vocoder_packet,
                    mvs_frame_info_type *frame_info,
                    uint16              packet_length,
                    mvs_pkt_status_type *status);

/* FEATURE_TDSCDMA */
void ghdi_umts_amr_ul_cb(uint8               *vocoder_packet,
                    mvs_frame_info_type *frame_info,
                    uint16              packet_length,
                    mvs_pkt_status_type *status);

/* FEATURE_TDSCDMA */
void ghdi_umts_amr_dl_cb(uint8               *vocoder_packet,
                    mvs_frame_info_type *frame_info,
                    mvs_pkt_status_type *status);


/* GSM client */
void ghdi_mvs_gsm_ul_proc(uint8                *ul_pkt,
                          mvs_frame_info_type  *frame_info,
                          uint16               pkt_len,
                          mvs_pkt_status_type  *status);

/* GSM client */
void ghdi_mvs_gsm_dl_proc(uint8                *dl_pkt,
                          mvs_frame_info_type  *frame_info,
                          mvs_pkt_status_type  *status);



void ghdi_mvs_cb_func(mvs_event_type *event);







#endif /* #ifndef _GSVOICEIF_H_ */
