#ifndef __MVSSUP_H__
#define __MVSSUP_H__

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S
                 S U P P L E M E N T A R Y   D E F I N I T I O N S

DESCRIPTION
  This header file contains supplementary functions and data definitions for
multimode voice services.

REFERENCES
  None.

Copyright (C) 2002-2011, 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/mvs/mvssup.h#1 $
$Author: mplcsds1 $
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sud     Updated Copyright.
21/03/11    at     Replaced uecomdef.h with wcdmamvsif.h due to changes in 
                   modem API structure.
11/02/11    at     Added TD-SCDMA support
03/31/11    hl     Added CS SO73 support
01/27/10    jk     Added g722 support
11/05/09    jk     Added g711 and g711A support
08/18/09    sr     Adapting to CVD.
06/01/09    at     Added AMR_WB support.
05/15/09   ymc     Added MVS_MODE_PCM enum to avoid compilation in
                   backward compatible mode.
05/13/09   ymc     Added file description and history section.
           ymc     Moved mvs_mode_type from mvs.h to mvssup.h.
           ymc     Updated remote MVS API.

===========================================================================*/

#include "comdef.h"
#include "wcdmamvsif.h"
#include "mvsamr.h"

/*===========================================================================

                    D A T A   D E F I N I T I O N S

===========================================================================*/

/* Definitions of QCELP-13k and EVRC-8k voice frame information */
typedef enum {
  MVS_VOC_0_RATE,             /* blank frame   */
  MVS_VOC_8_RATE,             /* 1/8 rate      */
  MVS_VOC_4_RATE,             /* 1/4 rate      */
  MVS_VOC_2_RATE,             /* 1/2 rate      */
  MVS_VOC_1_RATE,             /* full rate     */
  MVS_VOC_ERASURE,            /* erasure frame */
  MVS_VOC_8_RATE_NONCRITICAL, /* non critical 1/8 rate */
  MVS_VOC_RATE_MAX,
  MVS_VOC_RATE_UNDEF = MVS_VOC_RATE_MAX
} mvs_voc_rate_type;

/* WCDMA AMR transport channels class type - DEPRECATED: Use mvssup_umts_chan_class_type */
typedef enum {
  MVSSUP_WCDMA_CHAN_CLASS_NONE,    /* no channels, used for de-registration */
  MVSSUP_WCDMA_CHAN_CLASS_A,       /* only class A channel          */
  MVSSUP_WCDMA_CHAN_CLASS_AB,      /* only class A and B channels   */
  MVSSUP_WCDMA_CHAN_CLASS_ABC      /* all class A, B and C channels */
} mvssup_wcdma_chan_class_type;

/* UMTS AMR transport channels class type */
typedef enum {
  MVSSUP_UMTS_CHAN_CLASS_NONE,    /* no channels, used for de-registration */
  MVSSUP_UMTS_CHAN_CLASS_A,       /* only class A channel          */
  MVSSUP_UMTS_CHAN_CLASS_AB,      /* only class A and B channels   */
  MVSSUP_UMTS_CHAN_CLASS_ABC      /* all class A, B and C channels */
} mvssup_umts_chan_class_type;

/* WCDMA AMR transport channels state type - DEPRECATED: Use mvssup_umts_chan_state_type */
typedef struct {
  rlc_lc_id_type lca;                    /* class A logical channel no. */
  rlc_lc_id_type lcb;                    /* class B logical channel no. */
  rlc_lc_id_type lcc;                    /* class C logical channel no. */
  mvs_amr_chan_state_type chan_state;    /* existence of logical channels */
} mvssup_wcdma_chan_state_type;

/* UMTS AMR transport channels state type */
typedef struct {
  rlc_lc_id_type lca;                    /* class A logical channel no. */
  rlc_lc_id_type lcb;                    /* class B logical channel no. */
  rlc_lc_id_type lcc;                    /* class C logical channel no. */
  mvs_amr_chan_state_type chan_state;    /* existence of logical channels */
} mvssup_umts_chan_state_type;

/* Callback functions to determine block sizes of class A, B, C - DEPRECATED: use mvssup_umts_blk_size_cb_type*/
typedef uint16 (*mvssup_wcdma_blk_size_cb_type)(void);

typedef struct {
  mvssup_wcdma_blk_size_cb_type chan_a_size_cb;
  mvssup_wcdma_blk_size_cb_type chan_b_size_cb;
  mvssup_wcdma_blk_size_cb_type chan_c_size_cb;
} mvssup_wcdma_blk_size_cb_set_type;

/* Callback functions to determine block sizes of class A, B, C */
typedef uint16 (*mvssup_umts_blk_size_cb_type)(void);

typedef struct {
  mvssup_umts_blk_size_cb_type chan_a_size_cb;
  mvssup_umts_blk_size_cb_type chan_b_size_cb;
  mvssup_umts_blk_size_cb_type chan_c_size_cb;
} mvssup_umts_blk_size_cb_set_type;

/* Definitions of EFR and FR voice frame information */
/* GSM EFR and FR frame type definitions */
typedef enum {
  MVS_GSM_SID,          /* GSM FR or EFR : silence descriptor  */
  MVS_GSM_SPEECH_GOOD,  /* GSM FR or EFR : good speech frame   */
  MVS_GSM_BFI,          /* GSM FR or EFR : bad frame indicator */
  MVS_GSM_FRAME_MAX,
  MVS_GSM_FRAME_UNDEF = MVS_GSM_FRAME_MAX  /* undefined */
} mvs_gsm_frame_type;

/* GSM frame rate definitions */
typedef enum {
  MVS_GSM_RATE_EFR,     /* GSM EFR mode */
  MVS_GSM_RATE_FR,      /* GSM FR mode */
  MVS_GSM_RATE_MAX,
  MVS_GSM_RATE_UNDEF = MVS_GSM_RATE_MAX  /* undefined */
} mvs_gsm_rate_type;

/* Definitions of HR voice frame information */
typedef enum {
  MVS_HR_SID,           /* silence descriptor         */
  MVS_HR_SPEECH_GOOD,   /* good speech frame          */
  MVS_HR_SPEECH_BAD,    /* bad speech frame           */
  MVS_HR_INVALID_SID,   /* invalid silence descriptor */
  MVS_HR_FRAME_MAX,
  MVS_HR_FRAME_UNDEF = MVS_HR_FRAME_MAX  /* undefined */
} mvs_hr_frame_type;

/* SCR mode type and DTX mode type definitions */
/* SCR mode type definitions */
typedef enum {
  MVS_SCR_DISABLE,      /* Disable TX SCR operation */
  MVS_SCR_ENABLE        /* Enable TX SCR operation  */
} mvs_scr_mode_type;

/* GSM DTX mode type definitions */
typedef enum {
  MVS_DTX_OFF,          /* disable encoder DTX mode */
  MVS_DTX_ON            /* enable encoder DTX mode  */
} mvs_dtx_mode_type;

/* G.711 vocoder data format */
typedef enum {
  MVS_G711_MODE_MULAW,  /* G.711, mulaw format */
  MVS_G711_MODE_ALAW,   /* G.711, alaw format  */
  MVS_G711_MODE_MAX
} mvs_g711_mode_type;

/* G.711 A1/A2 vocoder data format */
typedef enum {
  MVS_G711A_MODE_MULAW = 0,  /* G.711A, mulaw format */
  MVS_G711A_MODE_ALAW,       /* G.711A, alaw format  */
  MVS_G711A_MODE_MAX
} mvs_g711a_mode_type;

/* Definitions of G.711 A1/A2 voice frame information */
typedef enum {
  MVS_G711A_SPEECH_GOOD = 0, /* good speech frame */
  MVS_G711A_SID,             /* silence descriptor */
  MVS_G711A_NO_DATA,         /* No data to transmit */
  MVS_G711A_ERASURE,
  MVS_G711A_FRAME_MAX,
  MVS_G711A_FRAME_UNDEF = MVS_G711A_FRAME_MAX  /* undefined         */
} mvs_g711a_frame_type;


/* G.723 vocoder data format */
typedef enum {
  MVS_G723_MODE_HIGH,   /* G.723.1, 6.3kbits    */
  MVS_G723_MODE_LOW,    /* G.723.1, 5.3kbits    */
  MVS_G723_MODE_SID,    /* G.723.1, SID frame   */
  MVS_G723_MODE_BLANK,  /* G.723.1, blank frame */
  MVS_G723_MODE_ERROR,  /* Error, no data recd  */
  MVS_G723_MODE_MAX
} mvs_g723_mode_type;

/* Definitions of G729A voice frame information */
typedef enum {
  MVS_G729A_NO_DATA = 0,                       /* No data to transmit */
  MVS_G729A_SPEECH_GOOD,                       /* good speech frame */
  MVS_G729A_SID,                               /* silence descriptor */
  MVS_G729A_ERASURE,                           /* no data descriptor*/
  MVS_G729A_FRAME_MAX,
  MVS_G729A_FRAME_UNDEF = MVS_G729A_FRAME_MAX  /* undefined         */
} mvs_g729a_frame_type;


/* G722 frame mode definitions */
typedef enum {
  MVS_G722_MODE_01,    /* 64 kbps */
  MVS_G722_MODE_02,    /* 56 kbps */
  MVS_G722_MODE_03,    /* 48 kbps */
  MVS_G722_MODE_MAX,
  MVS_G722_MODE_UNDEF = MVS_G722_MODE_MAX  /* undefined */
} mvs_g722_mode_type;

/* G722 voice frame definitions */
typedef enum {
  MVS_G722_SPEECH_GOOD,        /* good speech frame */
  MVS_G722_FRAME_MAX,
  MVS_G722_FRAME_UNDEF = MVS_G722_FRAME_MAX  /* undefined         */
} mvs_g722_frame_type;

/* Definitions of MVS mode type */
typedef enum {
  MVS_MODE_NONE,
  MVS_MODE_IS733,       /* QCELP-13k                    */
  MVS_MODE_IS127,       /* EVRC-8k                      */
  MVS_MODE_4GV_NB,      /* 4GV narrowband               */
  MVS_MODE_4GV_WB,      /* 4GV wideband                 */
  MVS_MODE_AMR,         /* Adaptive multi-rate          */
  MVS_MODE_EFR,         /* Enhanced full rate           */
  MVS_MODE_FR,          /* Full rate                    */
  MVS_MODE_HR,          /* Half rate                    */
  MVS_MODE_LINEAR_PCM,  /* enable/disable PCM interface */
  MVS_MODE_G711,        /* G.711 vocoder                */
  MVS_MODE_G723,        /* G.723 vocoder                */
  MVS_MODE_PCM,         /* PCM vocoder                  */
  MVS_MODE_AMR_WB,      /* AMR-WB vocoder               */
  MVS_MODE_G729A,       /* G729A vocoder                */
  MVS_MODE_G711A,       /* G711A vocoder                */
  MVS_MODE_G722,        /* G722 vocoder                 */
  MVS_MODE_4GV_NW,      /* 4GV narrowband-wideband      */
  MVS_MODE_MAX,
  MVS_MODE_32BIT_DUMMY = 0x7FFFFFFF
} mvs_mode_type;

/* ==========================================================================
** Definitions of MVS frame information (frame rate and frame mode)
** ==========================================================================
*/
typedef enum {
  MVS_FRAME_MODE_NONE,
  MVS_FRAME_MODE_VOC_TX,
  MVS_FRAME_MODE_VOC_RX,
  MVS_FRAME_MODE_AMR_UL,
  MVS_FRAME_MODE_AMR_DL,
  MVS_FRAME_MODE_GSM_UL,
  MVS_FRAME_MODE_GSM_DL,
  MVS_FRAME_MODE_HR_UL,
  MVS_FRAME_MODE_HR_DL,
  MVS_FRAME_MODE_G711_UL,
  MVS_FRAME_MODE_G711_DL,
  MVS_FRAME_MODE_G723_UL,
  MVS_FRAME_MODE_G723_DL,
  MVS_FRAME_MODE_PCM_UL,
  MVS_FRAME_MODE_PCM_DL,
  MVS_FRAME_MODE_AMR_IF1_UL,
  MVS_FRAME_MODE_AMR_IF1_DL,
  MVS_FRAME_MODE_G729A_UL,
  MVS_FRAME_MODE_G729A_DL,
  MVS_FRAME_MODE_G711A_UL,
  MVS_FRAME_MODE_G711A_DL,
  MVS_FRAME_MODE_G722_UL,
  MVS_FRAME_MODE_G722_DL,
  MVS_FRAME_MODE_MAX,
  MVS_FRAME_MODE_32BIT_DUMMY = 0x7FFFFFFF
} mvs_frame_mode_type;

typedef struct {
  mvs_frame_mode_type frame_mode;
  mvs_mode_type       mode;
  uint16              buf_free_cnt;
} mvs_frame_info_hdr_type;

/* QCELP-13k and EVRC-8k and 4GV frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_voc_rate_type       rate;
} mvs_voc_tx_frame_info_type;

typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_voc_rate_type rate;
  struct {
    boolean enable;
    uint16 factor;
    boolean phase_enable;
    uint16 run_length;
    uint16 phase_offset;
  } timewarp;
} mvs_voc_rx_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type    hdr;
  mvs_voc_tx_frame_info_type tx_info;
  mvs_voc_rx_frame_info_type rx_info;
} mvs_voc_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_voc_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_VOC_TX mvs_voc_frame_info_type.tx_info */
/*~ CASE MVS_FRAME_MODE_VOC_RX mvs_voc_frame_info_type.rx_info */
/*~ DEFAULT mvs_voc_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* AMR frame info type */
/* uplink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_amr_frame_type      frame;
  mvs_amr_mode_type       mode;
} mvs_amr_ul_frame_info_type;

/* downlink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_amr_frame_type      frame;
  mvs_amr_mode_type       mode;
} mvs_amr_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type    hdr;
  mvs_amr_ul_frame_info_type ul_info;
  mvs_amr_dl_frame_info_type dl_info;
} mvs_amr_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_amr_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_AMR_UL mvs_amr_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_AMR_DL mvs_amr_frame_info_type.dl_info */
/*~ DEFAULT mvs_amr_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* GSM EFR and FR frame info type */
/* uplink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_gsm_frame_type      frame;
  mvs_gsm_rate_type       rate;
} mvs_gsm_ul_frame_info_type;

/* downlink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_gsm_frame_type      frame;
  boolean                 taf;                    /* time allignment flag */
} mvs_gsm_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type   hdr;
  mvs_gsm_ul_frame_info_type ul_info;
  mvs_gsm_dl_frame_info_type dl_info;
} mvs_gsm_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_gsm_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_GSM_UL mvs_gsm_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_GSM_DL mvs_gsm_frame_info_type.dl_info */
/*~ DEFAULT mvs_gsm_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* HR frame info type */
/* uplink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_hr_frame_type       frame;
  boolean                 enc_err_flag;
} mvs_hr_ul_frame_info_type;

/* downlink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_hr_frame_type       frame;
  boolean                 taf;
  boolean                 bfi;
  boolean                 ufi;
} mvs_hr_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type   hdr;
  mvs_hr_ul_frame_info_type ul_info;
  mvs_hr_dl_frame_info_type dl_info;
} mvs_hr_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_hr_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_HR_UL mvs_hr_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_HR_DL mvs_hr_frame_info_type.dl_info */
/*~ DEFAULT mvs_hr_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* G.711 frame info structure */
/* Uplink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g711_mode_type      g711_mode;
} mvs_g711_ul_frame_info_type;

/* Downlink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g711_mode_type      g711_mode;
} mvs_g711_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type      hdr;
  mvs_g711_ul_frame_info_type  ul_info;
  mvs_g711_dl_frame_info_type  dl_info;
} mvs_g711_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_g711_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_G711_UL mvs_g711_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_G711_DL mvs_g711_frame_info_type.dl_info */
/*~ DEFAULT mvs_g711_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */


/* G.711 A1/A2 frame info structure */
/* Uplink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g711a_mode_type       g711a_mode;
  mvs_g711a_frame_type      frame;
} mvs_g711a_ul_frame_info_type;

/* Downlink frame info type */
typedef struct {
  mvs_frame_info_hdr_type   hdr;
  mvs_g711a_mode_type       g711a_mode;
  mvs_g711a_frame_type      frame;
} mvs_g711a_dl_frame_info_type;

/* G.711A frame info structure */
typedef union {
  mvs_frame_info_hdr_type       hdr;
  mvs_g711a_ul_frame_info_type  ul_info;
  mvs_g711a_dl_frame_info_type  dl_info;
} mvs_g711a_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_g711a_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_G711A_UL mvs_g711a_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_G711A_DL mvs_g711a_frame_info_type.dl_info */
/*~ DEFAULT mvs_g711a_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* G.723 frame info structure */
/* Uplink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g723_mode_type      g723_mode;
} mvs_g723_ul_frame_info_type;

/* Downlink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g723_mode_type      g723_mode;
} mvs_g723_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type      hdr;
  mvs_g723_ul_frame_info_type  ul_info;
  mvs_g723_dl_frame_info_type  dl_info;
} mvs_g723_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_g723_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_G723_UL mvs_g723_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_G723_DL mvs_g723_frame_info_type.dl_info */
/*~ DEFAULT mvs_g723_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* G.729A frame info structure */
/* Uplink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g729a_frame_type    frame;
} mvs_g729a_ul_frame_info_type;

/* Downlink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g729a_frame_type    frame;
} mvs_g729a_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type       hdr;
  mvs_g729a_ul_frame_info_type  ul_info;
  mvs_g729a_dl_frame_info_type  dl_info;
} mvs_g729a_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_g729a_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_G729A_UL mvs_g729a_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_G729A_DL mvs_g729a_frame_info_type.dl_info */
/*~ DEFAULT mvs_g729a_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* G.722 frame info structure */
/* Uplink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g722_frame_type frame;
  mvs_g722_mode_type g722_mode;
} mvs_g722_ul_frame_info_type;

/* G.722 frame info structure */
/* Downlink frame info type */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  mvs_g722_frame_type frame;
  mvs_g722_mode_type g722_mode;
} mvs_g722_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type hdr;
  mvs_g722_ul_frame_info_type ul_info;
  mvs_g722_dl_frame_info_type dl_info;
} mvs_g722_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_g722_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_G722_UL mvs_g722_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_G722_DL mvs_g722_frame_info_type.dl_info */
/*~ DEFAULT mvs_g722_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* PCM frame info type */
/* uplink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  void                    *info;            /* unused for now */
} mvs_pcm_ul_frame_info_type;

/* downlink frame info structure */
typedef struct {
  mvs_frame_info_hdr_type hdr;
  void                    *info;            /* unused for now */
} mvs_pcm_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type   hdr;
  mvs_pcm_ul_frame_info_type ul_info;
  mvs_pcm_dl_frame_info_type dl_info;
} mvs_pcm_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_pcm_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_PCM_UL mvs_pcm_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_PCM_DL mvs_pcm_frame_info_type.dl_info */
/*~ DEFAULT mvs_pcm_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* AMR-IF1 frame info structure */
typedef struct {
  mvs_frame_info_hdr_type       hdr;
  mvs_amr_frame_type_index_type frame_type_index;
  boolean                       fqi;
              /* Frame Quality Indicator: TRUE: good frame, FALSE: bad frame */
} mvs_amr_if1_ul_frame_info_type;

typedef struct {
  mvs_frame_info_hdr_type       hdr;
  mvs_amr_frame_type_index_type frame_type_index;
  boolean                       fqi;
              /* Frame Quality Indicator: TRUE: good frame, FALSE: bad frame */
} mvs_amr_if1_dl_frame_info_type;

typedef union {
  mvs_frame_info_hdr_type        hdr;
  mvs_amr_if1_ul_frame_info_type ul_info;
  mvs_amr_if1_dl_frame_info_type dl_info;
} mvs_amr_if1_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_amr_if1_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ CASE MVS_FRAME_MODE_AMR_IF1_UL mvs_amr_if1_frame_info_type.ul_info */
/*~ CASE MVS_FRAME_MODE_AMR_IF1_DL mvs_amr_if1_frame_info_type.dl_info */
/*~ DEFAULT mvs_amr_if1_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Generic MVS frame info type definitions */
typedef union {
  mvs_frame_info_hdr_type     hdr;
  mvs_voc_frame_info_type     voc_rate;
  mvs_amr_frame_info_type     amr_rate;
  mvs_gsm_frame_info_type     gsm_rate;
  mvs_hr_frame_info_type      hr_rate;
  mvs_g711_frame_info_type    g711_rate;
  mvs_g723_frame_info_type    g723_rate;
  mvs_pcm_frame_info_type     pcm_rate;
  mvs_amr_if1_frame_info_type amr_if1_info;
  mvs_g729a_frame_info_type   g729a_rate;
  mvs_g711a_frame_info_type   g711a_rate;
  mvs_g722_frame_info_type    g722_rate;
} mvs_frame_info_type;
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ TYPE mvs_frame_info_type DISC (_OBJ_.hdr.frame_mode) */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_VOC_TX ||
         _DISC_ == MVS_FRAME_MODE_VOC_RX ) mvs_frame_info_type.voc_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_AMR_UL ||
         _DISC_ == MVS_FRAME_MODE_AMR_DL ) mvs_frame_info_type.amr_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_GSM_UL ||
         _DISC_ == MVS_FRAME_MODE_GSM_DL ) mvs_frame_info_type.gsm_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_HR_UL ||
         _DISC_ == MVS_FRAME_MODE_HR_DL ) mvs_frame_info_type.hr_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_G711_UL ||
         _DISC_ == MVS_FRAME_MODE_G711_DL ) mvs_frame_info_type.g711_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_G723_UL ||
         _DISC_ == MVS_FRAME_MODE_G723_DL ) mvs_frame_info_type.g723_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_G722_UL ||
         _DISC_ == MVS_FRAME_MODE_G722_DL ) mvs_frame_info_type.g722_rate */         
/*~ IF ( _DISC_ == MVS_FRAME_MODE_PCM_UL ||
         _DISC_ == MVS_FRAME_MODE_PCM_DL ) mvs_frame_info_type.pcm_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_AMR_IF1_UL ||
         _DISC_ == MVS_FRAME_MODE_AMR_IF1_DL ) mvs_frame_info_type.amr_if1_info */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_G729A_UL ||
         _DISC_ == MVS_FRAME_MODE_G729A_DL ) mvs_frame_info_type.g729a_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_G711A_UL ||
         _DISC_ == MVS_FRAME_MODE_G711A_DL ) mvs_frame_info_type.g711a_rate */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_NONE ) mvs_frame_info_type.hdr */
/*~ IF ( _DISC_ == MVS_FRAME_MODE_MAX ) mvs_frame_info_type.hdr */
/*~ DEFAULT mvs_frame_info_type.hdr */
#endif /* FEATURE_HTORPC_METACOMMENTS */

/* Define MVS general voice frame queue interface */
#define MVS_VOC_PKT_SIZE ( 34 )          /* 34 bytes per vocoder packet */
#define MVS_PCM_PKT_SIZE ( 320 )         /* 320 bytes per PCM packet */
#define MVS_VOC_G711_PKT_SIZE ( 160 )    /* 160 bytes per G711 packet */
#define MVS_VOC_G723_PKT_SIZE ( 24 )     /* 24  bytes per G723 packet */
#define MVS_VOC_AWB_PKT_SIZE ( 60 )      /* 60 bytes per AMR-WB packet */

#define MVS_VOC_G729A_PKT_SIZE ( 10 )    /* 10 bytes per G729A packet */
#define MVS_VOC_G711A_PKT_SIZE ( 80 )    /* 80 bytes per G711 A1 A2 packet */

#define MVS_MAX_PKT_SIZE ( MVS_PCM_PKT_SIZE )

typedef struct {
  uint8 frame_data[MVS_PCM_PKT_SIZE];    /* Should be largest of all lb pkts */
  mvs_frame_info_type frame_info;
  uint16 frame_len;                      /* no. of bytes of frame_data */
} mvs_frame_type;

typedef struct {
  mvs_frame_type *frame_q;
  int q_size;
  int frame_cnt;
  int curr_frame;
  int miss_cnt;
  int lost_cnt;
} mvs_frame_q_type;

/* Definitions of MVS packet status type */
typedef enum {
  MVS_PKT_STATUS_NORMAL,
  MVS_PKT_STATUS_FAST,    /* packets were supplied too fast */
  MVS_PKT_STATUS_SLOW     /* packets were supplied too slowly */
} mvs_pkt_status_type;

/* <EJECT> */
/*===========================================================================

                   F U N C T I O N   D E F I N I T I O N S

===========================================================================*/

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
extern void mvssup_frame_q_init(
  mvs_frame_q_type *frame_q_ptr,
  mvs_frame_type *frames_buf,
  int size
);

/* <EJECT> */
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
extern void mvssup_frame_q_empty(
  mvs_frame_q_type *frame_q_ptr
);

/* <EJECT> */
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
extern boolean mvssup_frame_check_q_full(
  mvs_frame_q_type *frame_q_ptr
);

/* <EJECT> */
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
extern int mvssup_frame_get_q_size(
  mvs_frame_q_type *frame_q_ptr
);

/* <EJECT> */
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
extern int mvssup_frame_get_q_frame_cnt(
  mvs_frame_q_type *frame_q_ptr
);

/* <EJECT> */
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
extern boolean mvssup_frame_check_q_empty(
  mvs_frame_q_type *frame_q_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_frame_q_put

DESCRIPTION
  This function will put a frame into the queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_pkt_status_type mvssup_frame_q_put(
  mvs_frame_q_type *frame_q_ptr,
  uint8 *frame_data,
  mvs_frame_info_type *frame_info,
  uint16 frame_len
);

/* <EJECT> */
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
extern mvs_pkt_status_type mvssup_frame_q_get(
  mvs_frame_q_type *frame_q_ptr,
  uint8 *frame_data,
  mvs_frame_info_type *frame_info,
  uint16 *frame_len
);

/*===========================================================================

FUNCTION mvssup_tdscdma_set_ul_channel

DESCRIPTION
  This function allows the TD-SCDMA client to set the uplink channels used
to transport vocoder data from MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_tdscdma_set_ul_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_tdscdma_set_dl_channel

DESCRIPTION
  This function allows the TD-SCDMA client to set the downlink channels used
to transport vocoder data to MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
extern void mvssup_tdscdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class,
  mvssup_umts_blk_size_cb_set_type *blk_size_cb_set
);
#else
extern void mvssup_tdscdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_umts_chan_class_type chan_class
);
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_wcdma_set_ul_channel

DESCRIPTION
  This function allows the WCDMA client to set the uplink channels used to
transport vocoder data from MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_wcdma_set_ul_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_wcdma_set_dl_channel

DESCRIPTION
  This function allows the WCDMA client to set the downlink channels used to
transport vocoder data to MVS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING
extern void mvssup_wcdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class,
  mvssup_wcdma_blk_size_cb_set_type *blk_size_cb_set
);
#else
extern void mvssup_wcdma_set_dl_channel(
  rlc_lc_id_type lc_class_a,
  rlc_lc_id_type lc_class_b,
  rlc_lc_id_type lc_class_c,
  mvssup_wcdma_chan_class_type chan_class
);
#endif /* FEATURE_AS_MVS_ENHANCED_AMR_PROCESSING */

/* <EJECT> */
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
extern void mvssup_wcdma_ul_processing(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
);

/* <EJECT> */
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
extern void mvssup_wcdma_dl_processing(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
);

/*===========================================================================

FUNCTION mvssup_umts_ul_processing

DESCRIPTION
  This function processes uplink data and transports it over the DSM queue
initialized in the mvssup_wcdma_set_ul_channel or 
mvssup_tdscdma_set_ul_channel.

DEPENDENCIES
  mvssup_wcdma_set_ul_channel or mvssup_tdscdma_set_ul_channel must be
called first to initialize uplink channels.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_umts_ul_processing(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
);

/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_umts_dl_processing

DESCRIPTION
  This function retrieves downlink data from the DSM queue initialized in the
mvssup_wcdma_set_dl_channel or mvssup_tdscdma_set_dl_channel and processes it 
for vocoder use.

DEPENDENCIES
  mvssup_wcdma_set_dl_channel or mvssup_tdscdma_set_dl_channel must be
called first to initialize downlink channels.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_umts_dl_processing(
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
);

/* <EJECT> */
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
extern int mvssup_wcdma_encode_amrif2(
  uint8                *vocoder_packet,
  mvs_frame_info_type  *frame_info,
  uint8                *if2_frame
);

/* <EJECT> */
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
extern int mvssup_wcdma_decode_amrif2(
  uint8 *if2_frame,
  uint8 *vocoder_packet,
  mvs_frame_info_type *frame_info
);


/* <EJECT> */
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
extern void mvssup_amr_set_awb_mode(
  mvs_amr_mode_type amr_ul_mode
);

/* <EJECT> */
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
extern mvs_amr_mode_type mvssup_get_ul_awb_mode(void);

/* <EJECT> */
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
extern void mvssup_amr_set_amr_mode(
  mvs_amr_mode_type amr_ul_mode
);

/* <EJECT> */
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
extern mvs_amr_mode_type mvssup_get_ul_amr_mode(void);


/* <EJECT> */
/*===========================================================================

FUNCTION mvssup_amr_set_scr_mode

DESCRIPTION
  This function sets the SCR mode of the AMR vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_amr_set_scr_mode(
  mvs_scr_mode_type scr_mode
);

/* <EJECT> */
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
extern void mvssup_gsm_set_dtx_mode(
  mvs_dtx_mode_type dtx_mode
);

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
extern void mvssup_set_dtx_mode(
  mvs_dtx_mode_type dtx_mode
);
#ifdef FEATURE_HTORPC_METACOMMENTS
/*~ FUNCTION mvssup_set_dtx_mode */
#endif /* FEATURE_HTORPC_METACOMMENTS */
/*===========================================================================

FUNCTION mvssup_g711a_set_dtx_mode

DESCRIPTION
This function sets the DTX mode of a g711a vocoder session.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
extern void mvssup_g711a_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
);

/*===========================================================================

FUNCTION mvssup_g729a_set_dtx_mode

DESCRIPTION
  This function sets the DTX mode of a G.729A vocoder session.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_g729a_set_dtx_mode (
  mvs_dtx_mode_type dtx_mode
);

/*===========================================================================

FUNCTION mvssup_get_g711_mode

DESCRIPTION
  This function gets the current G.711 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_g711_mode_type mvssup_get_g711_mode ( void );

/*===========================================================================

FUNCTION mvssup_set_g711_mode

DESCRIPTION
  This function sets the G.711 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_set_g711_mode (
  mvs_g711_mode_type mvs_g711_mode
);

/*===========================================================================

FUNCTION mvssup_get_g711a_mode

DESCRIPTION
  This function gets the current G.711 A1/A2 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_g711a_mode_type mvssup_get_g711a_mode ( void );

/*===========================================================================

FUNCTION mvssup_set_g711a_mode

DESCRIPTION
  This function sets the G.711 A1/A2 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_set_g711a_mode (
  mvs_g711a_mode_type mvs_g711a_mode
);

/*===========================================================================

FUNCTION mvssup_set_g722_mode

DESCRIPTION
  This function sets the G.722 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_set_g722_mode (
  mvs_g722_mode_type mvs_g722_mode
);

/*===========================================================================

FUNCTION mvssup_get_g722_mode

DESCRIPTION
  This function gets the current G.722 encoder frame mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern mvs_g722_mode_type mvssup_get_g722_mode ( void );

/*===========================================================================

FUNCTION mvssup_voc_set_frame_rate

DESCRIPTION
  This function sets the max. and min. transfer rates for IS733, IS127 modes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_voc_set_frame_rate (
  mvs_voc_rate_type max_rate,
  mvs_voc_rate_type min_rate
);

/*===========================================================================

FUNCTION mvssup_voc_tx_rate_limit

DESCRIPTION
  This function limits the vocoder transmit rate to ( 2s+1 ) / ( 2s+2 ).

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mvssup_voc_tx_rate_limit (
  uint16 rate_factor
);
/*===========================================================================

Function  mvssup_get_voc_max_rate

DESCRIPTION  
   Gets the current maximum vocoder frame rate setting.
    
DEPENDENCIES
 None
     
RETURN VALUE
    returns the maximum trasfer rate for 1X Vocoders

SIDE EFFECTS
  None
===========================================================================*/
extern mvs_voc_rate_type mvssup_get_voc_max_rate( void );

/*===========================================================================

Function  mvssup_get_voc_min_rate

DESCRIPTION  
   Gets the current minimum vocoder frame rate setting.
    
DEPENDENCIES
 None
     
RETURN VALUE
    returns the minimum trasfer rate for 1X Vocoders

SIDE EFFECTS
  None
===========================================================================*/
extern mvs_voc_rate_type mvssup_get_voc_min_rate( void );


#endif /* !__MVSSUP_H__ */

