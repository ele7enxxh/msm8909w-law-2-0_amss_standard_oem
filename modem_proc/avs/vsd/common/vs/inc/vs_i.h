#ifndef __VS_I_H__
#define __VS_I_H__

/**
  @file  vs_i.h
  @brief This file contains internal definitions of VS
*/

/*
  ============================================================================

   Copyright (C) 2013-2014 QUALCOMM Technologies Incorporated. All Rights Reserved.
   QUALCOMM Proprietary and Confidential

  ============================================================================

                             Edit History

  $Header: //source/qcom/qct/multimedia2/api/audio/2.x/vs/main/latest/inc/vs_i.h
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  08/31/14  shr   Added support for 1x vooders.
  05/26/14  aim   Re-aligned timing offsets which are close to VFR
	              to account for the jitter.
  02/10/14  aim   Modified thread priority.
  08/09/13  sud   Renamed macro PC_ENV_SET to WINSIM.
  07/30/13  sud   Added a macro VS_REPORT_FATAL_ON_ERROR to report MSG_FATAL
                  when fatal error conditions occurs in client context.
  06/26/13  sud   Moved some of the function declarations from vs.c and
                  vs_voc.c
  06/16/13  sud   Removed some of the function declarations.
  06/13/13  hl    Added support for handling ADSP SSR.
  06/12/13  sud   VS refactored code and fixed bugs.
  04/16/13  sud   VS refactored code and fixed bugs.
  03/14/13  sud   Initial revision

  ============================================================================
*/

/****************************************************************************
  Include files for Module
****************************************************************************/
#include <stddef.h>
#include <string.h>
#include "err.h"
#include "mmdefs.h"
#include "aprv2_packet.h"
#include "apr_lock.h"
#include "apr_memmgr.h"
#include "apr_thread.h"
#include "apr_event.h"
#include "apr_objmgr.h"
#include "apr_memmgr.h"
#include "apr_list.h"
#include "apr_misc.h"
#include "aprv2_api_inline.h"
#include "vs.h"
#include "vss_public_if.h"
#include "vs_voc_private_if.h"

/****************************************************************************
  VS DEFINES
****************************************************************************/

#ifdef WINSIM
#include "testclient_vs.h"
#endif

/* Defined 10K for now. Expected it to go up to 25 K in future  <TBD>*/
#define VS_HEAP_SIZE_V ( 15000 )

/* Size of VS work packet queue. */
#define VS_NUM_WORK_PKTS_V ( 80 )

#define VS_HANDLE_TOTAL_BITS_V ( 16 )
#define VS_HANDLE_INDEX_BITS_V ( 7 ) /**< 7 bits = 128 handles. */
#define VS_MAX_OBJECTS_V ( 1 << VS_HANDLE_INDEX_BITS_V )
#define VS_MAX_GENERIC_OBJECTS_PER_SESSION_V ( 32 )

/* This value is decided considering that 10 session/client per voice
 * system. 10 * 2 voice system = 20
 */
#define VS_MAX_NUM_SESSIONS_V ( 20 )
/* This value is bounded by CVS max session name size (31) */
#define VS_MAX_CVD_SESSION_NAME_SIZE_V ( 31 )

/* Definitions for error checking */
#define VS_COMMON_MEDIA_ID_UNDEFINED_V (0xFFFFFFFF)
#define VS_VSID_UNDEFINED_V (0xFFFFFFFF)

/**
 * TODO: Currenlty the above thread priorities are same, this
 * needs to be revisited as based upon the processing the
 * priorites needs to be decided.
 */

/* Length of EFR/FR/HR packet core speech data in bytes */
#define VS_VOCEFR_MAX_PACKET_DATA_LENGTH ( 31 )
#define VS_VOCFR_MAX_PACKET_DATA_LENGTH ( 33 )
#define VS_VOCHR_MAX_PACKET_DATA_LENGTH ( 14 )

/* Length of EVRC-A packet core speech data in bytes */
#define VS_VOCEVRCA_MAX_PACKET_DATA_LENGTH ( 22 )

/* Length of EVRC-B packet core speech data in bytes */
#define VS_VOCEVRCB_MAX_PACKET_DATA_LENGTH ( 22 )

/* Length of EVRC-WB packet core speech data in bytes */
#define VS_VOCEVRCWB_MAX_PACKET_DATA_LENGTH ( 22 )

/* Length of EVRC-NW packet core speech data in bytes */
#define VS_VOCEVRCNW_MAX_PACKET_DATA_LENGTH ( 22 )

/* Length of EVRC-NW2K packet core speech data in bytes */
#define VS_VOCEVRCNW2K_MAX_PACKET_DATA_LENGTH ( 22 )

/* Length of QCELP-13K packet core speech data in bytes */
#define VS_VOCQCELP13K_MAX_PACKET_DATA_LENGTH ( 34 )

/* Length of G711 A-LAW packet core speech data in bytes */
#define VS_VOCG711ALAW_MAX_PACKET_DATA_LENGTH ( 160 )

/* Length of G711 MU-LAW packet core speech data in bytes */
#define VS_VOCG711MULAW_MAX_PACKET_DATA_LENGTH ( 160 )

/* Length of G729 frame in bytes */
#define VS_VOCG729_MAX_FRAME_DATA_LENGTH ( 10 )

/* Length of G729 packet core speech data in bytes
 * Two frames of 10 bytes in a packet.
 */
#define VS_VOCG729LAW_MAX_PACKET_DATA_LENGTH ( 20 )

/* This should be a GUID. Change it. <TBD> */
#define VS_CMD_PACKET_TYPE_V ( 0xCCCCCCCC )
#define VS_APR_PACKET_TYPE_V ( 0xAAAAAAAA )

/* Length of EFR/FR/HR packet core speech data in bytes */
#define VS_VOCEFR_MAX_PACKET_DATA_LENGTH ( 31 )
#define VS_VOCFR_MAX_PACKET_DATA_LENGTH ( 33 )
#define VS_VOCHR_MAX_PACKET_DATA_LENGTH ( 14 )

/* Maximum vocoder packet length - This number is decided based on the
 * vocoders supported by VS. As of today 4/2013, the biggest of all
 * the vocoder supported is AMR-WB. Size of AMR-WB is 61 + 1 byte header.
 */
#define VS_VOC_MAX_PACKET_LENGTH_V ( 62 )

// Length of voice packet header in bytes
#define VS_VOC_PACKET_HDR_LENGTH_V ( 1 )

/* Definitions for error checking */
#define VS_VOC_CLIENT_UNDEFINED_V (0xFFFFFFFF)
#define VS_VOC_CMD_UNDEFINED_V (0xFFFFFFFF)

#define VS_VOCAMR_CODEC_MODE_UNDEFINED_V  (0xFFFFFFFF)
#define VS_VOCAMR_CODEC_MODE_NONE_V VS_VOCAMR_CODEC_MODE_UNDEFINED_V

#define VS_VOCAMRWB_CODEC_MODE_UNDEFINED_V  (0xFFFFFFFF)
#define VS_VOCAMRWB_CODEC_MODE_NONE_V VS_VOCAMRWB_CODEC_MODE_UNDEFINED_V

/* EVRC-A */
#define VS_VOCEVRCA_MINMAX_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCA_MINMAX_RATE_NONE_V VS_VOCEVRCA_MINMAX_RATE_UNDEFINED_V

#define VS_VOCEVRCA_RATE_MODULATION_UNDEFINED_V (0xFFFFFFFF)
#define VS_VOCEVRCA_RATE_MODULATION_NONE_V VS_VOCEVRCA_RATE_MODULATION_UNDEFINED_V

/* EVRC-B */
#define VS_VOCEVRCB_MINMAX_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCB_MINMAX_RATE_NONE_V VS_VOCEVRCB_MINMAX_RATE_UNDEFINED_V

#define VS_VOCEVRCB_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCB_RATE_NONE_V VS_VOCEVRCB_RATE_UNDEFINED_V

/* EVRC-WB */
#define VS_VOCEVRCWB_MINMAX_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCWB_MINMAX_RATE_NONE_V VS_VOCEVRCWB_MINMAX_RATE_UNDEFINED_V

#define VS_VOCEVRCWB_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCWB_RATE_NONE_V VS_VOCEVRCWB_RATE_UNDEFINED_V

/* EVRC-NW */
#define VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCNW_MINMAX_RATE_NONE_V VS_VOCEVRCNW_MINMAX_RATE_UNDEFINED_V

#define VS_VOCEVRCNW_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCNW_RATE_NONE_V VS_VOCEVRCNW_RATE_UNDEFINED_V

/* EVRC-NW2K */
#define VS_VOCEVRCNW2K_MINMAX_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCNW2K_MINMAX_RATE_NONE_V VS_VOCEVRCNW2K_MINMAX_RATE_UNDEFINED_V

#define VS_VOCEVRCNW2K_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCEVRCNW2K_RATE_NONE_V VS_VOCEVRCNW2K_RATE_UNDEFINED_V

/* QCELP-13K */
#define VS_VOCQCELP13K_MINMAX_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCQCELP13K_MINMAX_RATE_NONE_V VS_VOCQCELP13K_MINMAX_RATE_UNDEFINED_V

#define VS_VOCQCELP13K_RATE_UNDEFINED_V (0xFFFFFFFF) /* Doubt */
#define VS_VOCQCELP13K_RATE_NONE_V VS_VOCQCELP13K_RATE_UNDEFINED_V

#define VS_VOCQCELP13K_RATE_MODULATION_UNDEFINED_V (0xFFFFFFFF)
#define VS_VOCQCELP13K_RATE_MODULATION_NONE_V VS_VOCQCELP13K_RATE_MODULATION_UNDEFINED_V

#define VS_VOC_DTX_MODE_DISABLE_V ( 0x0 )
#define VS_VOC_DTX_MODE_ENABLE_V ( 0x1 )
#define VS_VOC_DTX_MODE_UNDEFINED_V (0x2)

#define VS_VOC_INVALID_RATE ( ( uint16_t ) -1 ) 

/**
 * Default timing parameters. Applicable to any network that
 * doesn't have its own timing parameters.
 */
#define VS_VOC_TIMING_ENC_OFFSET_V ( 8000 )
  /**< DSP sends encoder packet 8 msec afer VFR. */
#define VS_VOC_TIMING_DEC_REQ_OFFSET_V ( 3300 )
  /**< DSP sends decoder request 3.3 msec afer VFR. */
#define VS_VOC_TIMING_DEC_OFFSET_V ( 8300 )
  /**< DSP deadline to receive decoder packet - 8.3 msec afer VFR. */

/* GSM specific timing parameters. */
#define VS_VOC_TIMING_GSM_ENC_OFFSET_V ( 10000 )
  /**< DSP sends encoder packet 10 msec afer VFR. */
#define VS_VOC_TIMING_GSM_DEC_REQ_OFFSET_V ( 1000 )
  /**< DSP sends decoder request 1 msec afer VFR. */
#define VS_VOC_TIMING_GSM_DEC_OFFSET_V ( 4300 )
  /**< DSP deadline to receive decoder packet - 4.3 msec afer VFR. */

/* WCDMA specific timing parameters. */
#define VS_VOC_TIMING_WCDMA_ENC_OFFSET_V ( 19000 )
  /**< DSP sends encoder packet 19 msec afer VFR. */
#define VS_VOC_TIMING_WCDMA_DEC_REQ_OFFSET_V ( 3300 )
  /**< DSP sends decoder request 3.3 msec afer VFR. */
#define VS_VOC_TIMING_WCDMA_DEC_OFFSET_V ( 6300 )
  /**< DSP deadline to receive decoder packet - 6.3 msec afer VFR. */

/* TDSCDMA specific timing parameters. */
#define VS_VOC_TIMING_TDSCDMA_ENC_OFFSET_V ( 19000 )
  /**< DSP sends encoder packet 19 msec afer VFR. */
#define VS_VOC_TIMING_TDSCDMA_DEC_REQ_OFFSET_V ( 3300 )
  /**< DSP sends decoder request 3.3 msec afer VFR. */
#define VS_VOC_TIMING_TDSCDMA_DEC_OFFSET_V ( 6300 )
  /**< DSP deadline to receive decoder packet - 6.3 msec afer VFR. */

/* CDMA specific timing parameters. */
#define VS_VOC_TIMING_CDMA_ENC_OFFSET_V ( 18000 )
  /**< DSP sends encoder packet 18 msec afer VFR. */
#define VS_VOC_TIMING_CDMA_DEC_REQ_OFFSET_V ( 3750 )
  /**< DSP sends decoder request 3.75 msec afer VFR. */
#define VS_VOC_TIMING_CDMA_DEC_OFFSET_V ( 6750 )
  /**< DSP deadline to receive decoder packet - 6.75 msec afer VFR. */

/* Vocode frame synchronization modes */
#define VS_VOC_VFR_MODE_HARD_V (1)  /* Hard VFR (20 ms VFR interrupt) */
#define VS_VOC_VFR_MODE_SOFT_V (0)  /* No VFR */


#define VS_VOC_BUFFER_VALID_SPEECH_FRAME ( 1 ) /* Valid encoder speech frame */
#define VS_VOC_BUFFER_NO_SPEECH_FRAME_V ( 0 ) /* NO encoder speech frame */
#define VS_VOC_BUFFER_MINOR_VERSION_V ( 0x0001 ) /* minor versoion - 0001 */
#define VS_VOC_BUFFER_TIMESTAMP_NOT_SUPPORTED_V ( 0 ) /* timestamp not supported */

/**
 * Maximum Number of VS Clients.
 */
#define VS_MAX_NUM_CLIENTS ( 10 )

/**
 * This is and internal command for reseting the vocoder state
 * of each VS session, and cleaning up all the CVD reference
 * created by VS, after recieving any of below events
 *
 * Audio-PD DOWN, CVD service DOWN and ADSP SSR events.
 */
#define VS_VOC_CMDI_CLEANUP_CVD_REFERENCES ( 0x000130EA )


/**
 * This command ensure that when ADSP/CVD recovers after the timeout
 * we should ensure that CVD session referenced in all VS session should issue
 * STOP_VOICE.
 */
#define VS_VOC_CMDI_DISBALE_CVD_ON_ADSP_RECOVERY ( 0x00013101 )


/**
 * This command ensure that when ADSP/CVD recovers after the timeout
 * we should ensure that CVD (MVM/CVS) session referenced in all VS session should be
 * destroyed.
 *
 * Post cleanup vocoder state of all the VS session will be restored as per the
 * current call state.
 */
#define VS_VOC_CMDI_DESTROY_CVD_ON_ADSP_RECOVERY ( 0x00013100 )


/**
 * This is and internal command for restoring the vocoder state
 * of each VS session when CVD services are available to VS
 * after recieving below events
 *
 * Audio-PD UP and CVD service UP and ADSP SSR events.
 */
#define VS_VOC_CMDI_RESTORE_CVD_STATE ( 0x000130EB )

/* Command to setup voice session state as per the call_state. */
#define VS_VOC_CMDI_ENABLE ( 0x000130E6 )

typedef struct vs_voc_cmdi_enable_t
{
  uint32_t handle;
  /**< The open handle. */
  void* client_context;
    /**< Client-provided data pointer for the client's use. */
  bool_t is_rsp_pending;
    /**< Indicates VS client is awaiting response. */
} vs_voc_cmdi_enable_t;

/* Command to standby voice session state as per the call_state. */
#define VS_VOC_CMDI_STANDBY ( 0x000130E7 )

typedef struct vs_voc_cmdi_standby_t
{
  uint32_t handle;
  /**< The open handle. */
  void* client_context;
    /**< Client-provided data pointer for the client's use. */
  bool_t is_rsp_pending;
    /**< Indicates VS client is awaiting response. */
} vs_voc_cmdi_standby_t;

/* Command to reset/cleanup voice session as per the call_state. */
#define VS_VOC_CMDI_DISABLE (  0x000130E8 )

typedef struct vs_voc_cmdi_disable_t
{
  uint32_t handle;
  /**< The open handle. */
  void* client_context;
    /**< Client-provided data pointer for the client's use. */
  bool_t is_rsp_pending;
    /**< Indicates VS client is awaiting response. */
} vs_voc_cmdi_disable_t;

/* Command to Flush the VOC buffers allocated by client. */
#define VS_VOC_CMDI_FLUSH_BUFFERS ( 0x000130F2 )

typedef struct vs_voc_cmdi_flush_buffers_t
{
  uint32_t handle;
  /**< The open handle. */
  void* client_context;
  /**< Client-provided data pointer for the client's use. */
} vs_voc_cmdi_flush_buffers_t;

/* Command to Destroy the VS session Open by client. */
#define VS_VOC_CMDI_CLOSE ( 0x000130F1 )

typedef struct vs_voc_cmdi_close_t
{
  uint32_t handle;
  /**< The open handle. */
   void* client_context;
  /**< Client-provided data pointer for the client's use. */
} vs_voc_cmdi_close_t;

/* Internal Command to Destroy the VS session. */
#define VS_VOC_CMDI_DESTROY_SESSION ( 0x000130F3 )

typedef struct vs_voc_cmdi_destroy_session_t
{
  uint32_t handle;
  /**< The open handle. */
  uint32_t token;
  /**< To track the response for Internal session destroy. */
  void* client_context;
  /**< Client-provided data pointer for the client's use. */
} vs_voc_cmdi_destroy_session_t;

/* Internal Response to Destroy VS session. */
#define VS_VOC_RSPI_DESTROY_SESSION ( 0x000130F4 )

typedef struct vs_voc_rspi_destroy_session_t
{
  uint32_t token;
  /**< The open handle. */
  uint32_t status;
} vs_voc_rspi_destroy_session_t;

#define VS_PANIC_ON_ERROR( rc ) \
  { if ( rc ) { ERR_FATAL( "Error[0x%08x], vs_state=%d", \
                            rc, vs_is_initialized, 0 ); } }

#define VS_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,\
                      "Error[0x%08x], vs_state=%d", \
                            rc, vs_is_initialized ); } }

#define VS_ACQUIRE_LOCK( lock ) \
  apr_lock_enter( lock );

#define VS_RELEASE_LOCK( lock ) \
  apr_lock_leave( lock );

/* flag that indicates the state of vs task. */
extern bool_t vs_is_initialized;

typedef enum vs_voc_client_type_t
{
  VS_VOC_CLIENT_TYPE_MVS,
  VS_VOC_CLIENT_TYPE_MODEM,
  VS_VOC_CLIENT_TYPE_INVALID,
}
  vs_voc_client_type_t;
/****************************************************************************
                    D A T A   D E F I N I T I O N S
****************************************************************************/
/* The following macros are here for temporary purposes.
 * Once the voicecfg is preset, we do not need this. Instead
 */

typedef enum vs_vsid_enum_t
{
  VS_VSID_CS_VOICE_MULTIMODE = 0x10C01000,
  VS_VSID_CS_VOICE_GSM2 = 0x10DC1000
}
  vs_vsid_enum_t;


/****************************************************************************
  VS_VOC packet status type
****************************************************************************/

typedef enum vs_voc_pkt_status_enum_t
{
  VS_VOC_PKT_STATUS_NORMAL,
  VS_VOC_PKT_STATUS_FAST,    /* packets were supplied too fast */
  VS_VOC_PKT_STATUS_SLOW     /* packets were supplied too slowly */
}
  vs_voc_pkt_status_enum_t;

/****************************************************************************
  VS vocoder properties definitions
****************************************************************************/

/* AMR vocoder property structure. */
typedef struct vs_vocamr_property_t vs_vocamr_property_t;
struct vs_vocamr_property_t
{
  uint32_t dtx_enable;
      /**< Toggle DTX on or off. Possible values:\n
           - VS_VOC_DTX_MODE_DISABLE_V -- Disables DTX \n
           - VS_VOC_DTX_MODE_ENABLE_V -- Enables DTX \n
           - VS_VOC_DTX_MODE_UNDEFINED_V -- Not configured */
  uint32_t codec_mode;
};

/* AMR-WB vocoder property structure. */
typedef struct vs_vocamrwb_property_t vs_vocamrwb_property_t;
struct vs_vocamrwb_property_t
{
  uint32_t dtx_enable;
  /**< Toggle DTX on or off. Possible values:\n
       - VS_VOC_DTX_MODE_DISABLE_V -- Disables DTX \n
       - VS_VOC_DTX_MODE_ENABLE_V -- Enables DTX \n
       - VS_VOC_DTX_MODE_UNDEFINED_V -- Not configured */
  uint32_t codec_mode;
};

/* EFR vocoder property structure. */
typedef struct vs_vocefr_property_t vs_vocefr_property_t;
struct vs_vocefr_property_t
{
  uint32_t dtx_enable;
  /**< Toggle DTX on or off. Possible values:\n
       - VS_VOC_DTX_MODE_DISABLE_V -- Disables DTX \n
       - VS_VOC_DTX_MODE_ENABLE_V -- Enables DTX \n
       - VS_VOC_DTX_MODE_UNDEFINED_V -- Not configured */
};

/* FR vocoder property structure. */
typedef struct vs_vocfr_property_t vs_vocfr_property_t;
struct vs_vocfr_property_t
{
  uint32_t dtx_enable;
  /**< Toggle DTX on or off. Possible values:\n
       - VS_VOC_DTX_MODE_DISABLE_V -- Disables DTX \n
       - VS_VOC_DTX_MODE_ENABLE_V -- Enables DTX \n
       - VS_VOC_DTX_MODE_UNDEFINED_V -- Not configured */
};

/* HR vocoder property structure. */
typedef struct vs_vochr_property_t vs_vochr_property_t;
struct vs_vochr_property_t
{
  uint32_t dtx_enable;
  /**< Toggle DTX on or off. Possible values:\n
       - VS_VOC_DTX_MODE_DISABLE_V -- Disables DTX \n
       - VS_VOC_DTX_MODE_ENABLE_V -- Enables DTX \n
       - VS_VOC_DTX_MODE_UNDEFINED_V -- Not configured */
};

/* EVRC-A vocoder property structure. */
typedef struct vs_vocevrca_property_t vs_vocevrca_property_t;
struct vs_vocevrca_property_t
{
  uint16_t min_rate;
      /**< Min encoder rate. */
  uint16_t max_rate;
      /**< Max encoder rate. */
  uint32_t rate_modulation;
    /**< Rate modulation to the vocoder.*/
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

/* EVRC-B vocoder property structure. */
typedef struct vs_vocevrcb_property_t vs_vocevrcb_property_t;
struct vs_vocevrcb_property_t
{
  uint16_t min_rate;
      /**< Min encoder rate. */
  uint16_t max_rate;
      /**< Max encoder rate. */
  uint32_t rate;
    /**< Average encoder rate.*/
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

/* EVRC-NW vocoder property structure. */
typedef struct vs_vocevrcnw_property_t vs_vocevrcnw_property_t;
struct vs_vocevrcnw_property_t
{
  uint16_t min_rate;
      /**< Min encoder rate. */
  uint16_t max_rate;
      /**< Max encoder rate. */
  uint32_t rate;
    /**< Average encoder rate.*/
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

/* EVRC-NW2K vocoder property structure. */
typedef struct vs_vocevrcnw2k_property_t vs_vocevrcnw2k_property_t;
struct vs_vocevrcnw2k_property_t
{
  uint16_t min_rate;
      /**< Min encoder rate. */
  uint16_t max_rate;
      /**< Max encoder rate. */
  uint32_t rate;
    /**< Average encoder rate.*/
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

/* EVRC-WB vocoder property structure. */
typedef struct vs_vocevrcwb_property_t vs_vocevrcwb_property_t;
struct vs_vocevrcwb_property_t
{
  uint16_t min_rate;
      /**< Min encoder rate. */
  uint16_t max_rate;
      /**< Max encoder rate. */
  uint32_t rate;
    /**< Average encoder rate.*/
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

/* QCELP-13K vocoder property structure. */
typedef struct vs_vocqcelp13k_property_t vs_vocqcelp13k_property_t;
struct vs_vocqcelp13k_property_t
{
  uint16_t min_rate;
      /**< Min encoder rate. */
  uint16_t max_rate;
      /**< Max encoder rate. */
  uint32_t rate;
    /**< Average encoder rate.*/
  uint32_t rate_modulation;
    /**< Rate modulation to the vocoder.*/
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

/* G.729 vocoder property structure. */
typedef struct vs_vocg729_property_t vs_vocg729_property_t;
struct vs_vocg729_property_t
{
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

/* G.711 A-Law vocoder property structure. */
typedef struct vs_vocg711alaw_property_t vs_vocg711alaw_property_t;
struct vs_vocg711alaw_property_t
{
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

/* G.711 MU-Law vocoder property structure. */
typedef struct vs_vocg711mulaw_property_t vs_vocg711mulaw_property_t;
struct vs_vocg711mulaw_property_t
{
  uint32_t dtx_enable;
      /**< DTX Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX
        */
};

typedef struct vs_voc_property_t vs_voc_property_t;
struct vs_voc_property_t
{
 /**
  * TBD: Add missing vocoder support.
  *
  * AI: Shruthi.
  */
  vs_vocamr_property_t    amr;
  vs_vocamrwb_property_t  amrwb ;
  vs_vocefr_property_t    efr;
  vs_vocfr_property_t     fr;
  vs_vochr_property_t     hr;
  vs_vocevrca_property_t evrca;
  vs_vocevrcb_property_t evrcb;
  vs_vocevrcwb_property_t evrcwb;
  vs_vocevrcnw_property_t evrcnw;
  vs_vocevrcnw2k_property_t evrcnw2k;
  vs_vocqcelp13k_property_t qcelp13k;
  vs_vocg729_property_t g729;
  vs_vocg711alaw_property_t g711alaw;
  vs_vocg711mulaw_property_t g711mulaw;
};


/***************************************************************************
  VS VOC BUFFER ( TYPE / ITEM ) DEFINATIONS                                *
****************************************************************************/

/* This identifies the state of a client allocated buffer at an
 * instant of time.
 */
typedef enum vs_voc_buffer_type_t
{
  VS_VOC_BUFFER_TYPE_NONE,
  VS_VOC_BUFFER_TYPE_READ,
  VS_VOC_BUFFER_TYPE_WRITE,
  VS_VOC_BUFFER_TYPE_PRIMED
}
  vs_voc_buffer_type_t;

/* The generic item is used in VS to track read and write buffers and
 * free them at appropriate time.
 */
typedef struct vs_voc_buffer_item_t vs_voc_buffer_item_t;
struct vs_voc_buffer_item_t
{
  apr_list_node_t link;
  vs_voc_buffer_type_t buf_type;
  vs_voc_buffer_t* buf;
};

/****************************************************************************
  VS thread states
****************************************************************************/
typedef enum vs_thread_state_enum_t
{
  VS_THREAD_STATE_ENUM_INIT,
  VS_THREAD_STATE_ENUM_READY,
  VS_THREAD_STATE_ENUM_EXIT
}
  vs_thread_state_enum_t;

 typedef enum vs_thread_priority_enum_t
{
  VS_THREAD_PRIORITY_ENUM_HIGH,
  VS_THREAD_PRIORITY_ENUM_MED,
  VS_THREAD_PRIORITY_ENUM_LOW
}
  vs_thread_priority_enum_t;

/***************************************************************************
  VS WORK REQUEST ( TYPE / ITEM ) DEFINITIONS                              *
****************************************************************************/

typedef struct vs_cmd_packet_t vs_cmd_packet_t;
struct vs_cmd_packet_t
{
  uint32_t cmd_id; /**< Command id issued from client. */
  void* params; /**< Structure associated to each cmd_id. */
};

typedef enum vs_work_item_pkt_type_t
{
  VS_WORK_ITEM_PKT_TYPE_NONE,
  VS_WORK_ITEM_PKT_TYPE_CMD,
  VS_WORK_ITEM_PKT_TYPE_APR
}
  vs_work_item_pkt_type_t;

typedef struct vs_work_item_t vs_work_item_t;
struct vs_work_item_t
{
  apr_list_node_t link;
  vs_work_item_pkt_type_t pkt_type;
  void* packet;
   /**< This is generic request packet, as per the pkt_type it can
    * to vs_cmd_packet_t or aprv2_packet_t. */
};

/****************************************************************************
   VS OBJECT DEFINITIONS
****************************************************************************/

typedef struct vs_object_header_t vs_object_header_t;
typedef struct vs_token_object_t vs_token_object_t;
typedef struct vs_simple_job_object_t vs_simple_job_object_t;
typedef struct vs_sequencer_job_object_t vs_sequencer_job_object_t;
typedef struct vs_session_object_t vs_session_object_t;
typedef union vs_object_t vs_object_t;
typedef struct vs_generic_item_t vs_generic_item_t;

/****************************************************************************
  COMMAND RESPONSE FUNCTION TABLE
****************************************************************************/
typedef void ( *vs_event_handler_fn_t ) ( aprv2_packet_t* packet );

typedef enum vs_response_fn_enum_t
{
  VS_RESPONSE_FN_ENUM_ACCEPTED,
  VS_RESPONSE_FN_ENUM_RESULT,
  VS_RESPONSE_FN_ENUM_INVALID,
  VS_RESPONSE_FN_ENUM_MAX = VS_RESPONSE_FN_ENUM_INVALID
}
  vs_response_fn_enum_t;

/*
 * Pending commands may load different sets of response and event handlers to
 * complete each job. The response function table is equivalent to the state
 * design pattern. The state context is stored in the gating command control.
 * Pending commands can be as simple or as complex as required.
 */
typedef vs_event_handler_fn_t vs_response_fn_table_t[ VS_RESPONSE_FN_ENUM_MAX ];

typedef enum vs_gating_cmd_state_enum_t
{
  VS_GATING_CMD_STATE_ENUM_FETCH,
  VS_GATING_CMD_STATE_ENUM_EXECUTE,
  VS_GATING_CMD_STATE_ENUM_CONTINUE
}
  vs_gating_cmd_state_enum_t;

typedef struct vs_gating_control_t vs_gating_control_t;
struct vs_gating_control_t
{
  apr_list_t cmd_q;
  /**< The gating (vs_work_item_t) command queue. */
  vs_gating_cmd_state_enum_t state;
  /**<
   * The current state of the gating command control.
   *
   * This variable is managed by the gating command processor. The
   * individual gating command controls indicates to the gating command
   * processor to complete or to delay the completion of the current
   * gating command.
   */
  vs_work_item_pkt_type_t pkt_type;
  /**<
   * This is required for Gating commands to identify the packet type
   * below mentioned.
   */
  void* packet;
  /**<
   * The current (command) packet being processed.
   * This is generic reference to command packet, which could be
   * vs_cmd_packet_t or aprv2_packet_t.
   */
  vs_object_t* rootjob_obj;
  /**<
   * The rootjob_obj is a temporary storage for the current gating
   * command.
   */
};

/****************************************************************************
   THE COMMON OBJECT DEFINITIONS
****************************************************************************/
typedef enum vs_object_type_enum_t
{
   VS_OBJECT_TYPE_ENUM_UNINITIALIZED,
   VS_OBJECT_TYPE_ENUM_HANDLE,
   VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
   VS_OBJECT_TYPE_ENUM_SEQUENCER_JOB,
   VS_OBJECT_TYPE_ENUM_SESSION,
   VS_OBJECT_TYPE_ENUM_INVALID
}
  vs_object_type_enum_t;

struct vs_object_header_t
{
  uint32_t handle;
   /**< The handle to the associated apr_objmgr_object_t instance. */
  vs_object_type_enum_t type;
   /**<
    * The object type defines the actual derived object.
    *
    * The derived object can be any custom object type. A session or a
    * command are two such custom object types. A free object entry is set
    * to VS_OBJECT_TYPE_ENUM_FREE.
    */
};

struct vs_generic_item_t
{
  apr_list_node_t link;

  uint32_t handle;
   /**< Any custom handle. */
};

typedef struct vs_voc_voice_timing_t vs_voc_voice_timing_t;
struct vs_voc_voice_timing_t
{
  uint16_t vfr_mode;
  /**< Vocoder frame synchronization mode. Possible values:\n
        - 0 -- No frame synchronization. \n
        - 1 -- Hard Vocoder Frame Reference (VFR). A 20 ms VFR interrupt. */
  uint32_t vsid;
  /**< Voice System ID as defined by DCN 80-NF711-1 */
  uint16_t enc_offset;
  /**< Offset in microseconds from the VFR to deliver a Tx vocoder
      packet. The offset is to be less than 20000 us. */
  uint16_t dec_req_offset;
  /**< The offset in microseconds from the VFR to request for an Rx vocoder
        packet. The offset is to be less than 20000 us. */
  uint16_t dec_offset;
  /**< Offset in microseconds from the VFR to indicate the deadline to
        receive an Rx vocoder packet. The offset is to be less than 20000 us.
        Rx vocoder packets received after this deadline are not guaranteed to
        be processed.  */
};

/****************************************************************************
  THE HANDLE OBJECT
****************************************************************************/
struct vs_token_object_t
{
  vs_object_header_t header;
  vs_response_fn_table_t fn_table;
   /**<
    * This is the response function v-table. The response table can store
    * custom response routines for all possible responses directed to this
    * specific job.
    */
};

/****************************************************************************
  THE SIMPLE JOB OBJECT
****************************************************************************/
struct vs_simple_job_object_t
{
  vs_object_header_t header;
  uint32_t context_handle;
   /**<
    * The parent-job handle indicates this sub-job is part of a batch-job.
    *
    * Set this value to -1 when it is unused or when there is no parent.
    */
  vs_response_fn_table_t fn_table;
   /**<
    * This is the response function v-table. The response table can store
    * custom response routines for all possible responses directed to this
    * specific job.
    */
  bool_t is_accepted;
   /**< The command accepted response flag. 0 is false and 1 is true. */
  bool_t is_completed;
   /**< The command completed response flag. 0 is false and 1 is true. */
  uint32_t status;
   /**< The status returned by the command completion. */
};

/****************************************************************************
  THE SEQUENCER JOB OBJECT
****************************************************************************/
struct vs_sequencer_job_object_t
{
  vs_object_header_t header;

  uint32_t state;
   /**< The generic state variable. */
  union vs_object_t* subjob_obj;
   /**< The current sub-job object. */
  uint32_t status;
   /**< A status value. */
};

/****************************************************************************
 * VS_VOC Call/Client STATE DEFINITIONS                                     *
 ****************************************************************************/

 typedef enum vs_call_state_enum_t
 {
   VS_CALL_STATE_ENUM_RESET,
     /**< Indicate either Call signaling setup is not initiated or
      * Call signaling tear down is done.
      */
   VS_CALL_STATE_ENUM_IDLE,
     /**< Indicates either Call signaling setup is imitated or
      * Call signaling for reconfiguration is done.
      */
   VS_CALL_STATE_ENUM_ENABLED
    /**< Indicates call signaling setup is done. */
 }
  vs_call_state_enum_t;

/****************************************************************************
 * VOICE SESSION SETUP CONTROL DEFINITIONS                                  *
 ****************************************************************************/

typedef enum vs_voc_state_enum_t
{
  VS_VOC_STATE_ENUM_UNINITIALIZED,
    /**< Reserved. */
  VS_VOC_STATE_ENUM_RESET_ENTRY,
    /**< Move into or out of reset. */
  VS_VOC_STATE_ENUM_RESET,
    /**< The voice session resource( CVD ) is not created/acquired. */
  VS_VOC_STATE_ENUM_IDLE_ENTRY,
    /**< Move into or out of idle. */
  VS_VOC_STATE_ENUM_IDLE,
    /**< The voice session resource( CVD ) is acquired and ready to be
      * configure/run.
      */
  VS_VOC_STATE_ENUM_RUN_ENTRY,
    /**< Move into or out of run. */
  VS_VOC_STATE_ENUM_RUN,
    /**< The Voice session resource ( CVD ) is running. */
  VS_VOC_STATE_ENUM_INVALID
    /**< Reserved. */
}
  vs_voc_state_enum_t;

typedef enum vs_voc_goal_enum_t
{
  VS_VOC_GOAL_ENUM_UNINITIALIZED,
  VS_VOC_GOAL_ENUM_NONE,
  VS_VOC_GOAL_ENUM_CREATE,
  VS_VOC_GOAL_ENUM_DESTROY,
  VS_VOC_GOAL_ENUM_CONFIGURE,
  VS_VOC_GOAL_ENUM_ENABLE,
  VS_VOC_GOAL_ENUM_DISABLE,
  VS_VOC_GOAL_ENUM_INVALID,
}
  vs_voc_goal_enum_t;

typedef enum vs_voc_action_enum_t
{
  VS_VOC_ACTION_ENUM_UNINITIALIZED,
    /**< Common actions. */
  VS_VOC_ACTION_ENUM_NONE,
    /**< The first action has not started for a goal from any state. */
  VS_VOC_ACTION_ENUM_COMPLETE,
    /**<
     * Reached the last action for a goal from a state. A multi-action goal
     * that starts from and ends in the same state may require a COMPLETE
     * action to properly differentiate a terminate signal.
     */
  VS_VOC_ACTION_ENUM_CONTINUE,
    /**<
     * For multi-state goals, the last action from each state should set to
     * CONINTUE. This indicates to the next state that a goal is continuing
     * its operation from a previous state. Usually the previous state is
     * known given the current state and the continued goal. New actions can
     * be created to help discriminate the direction from where goals come
     * from as required.
     */
  VS_VOC_ACTION_ENUM_START_VOICE,
  VS_VOC_ACTION_ENUM_STOP_VOICE,
  VS_VOC_ACTION_ENUM_STANDBY_VOICE,
  VS_VOC_ACTION_ENUM_CREATE_MVM,
  VS_VOC_ACTION_ENUM_DESTROY_MVM,
  VS_VOC_ACTION_ENUM_SET_NETWORK_ON_MVM,
  VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_ON_MVM,
  VS_VOC_ACTION_ENUM_SET_VOICE_TIMING,
    /**< Above acion are associated to MVM. */
  VS_VOC_ACTION_ENUM_CREATE_CVS,
  VS_VOC_ACTION_ENUM_DESTROY_CVS,
  VS_VOC_ACTION_ENUM_ATTACH_CVS,
  VS_VOC_ACTION_ENUM_DETACH_CVS,
  VS_VOC_ACTION_ENUM_LISTEN_EAMR,
  VS_VOC_ACTION_ENUM_CANCEL_LISTEN_EAMR,
  VS_VOC_ACTION_ENUM_SET_MEDIA_TYPE_OVER_CVS,
  VS_VOC_ACTION_ENUM_SET_CACHED_STREAM_PROPERTIES,
  VS_VOC_ACTION_ENUM_RESYNC_CTM,
    /**< Above acion are associated to CVS/STREAM. */
  VS_VOC_ACTION_ENUM_INVALID
}
  vs_voc_action_enum_t;

typedef struct vs_control_t vs_control_t;
struct vs_control_t
{
  uint32_t transition_job_handle;
  uint32_t pendjob_handle;

  vs_voc_goal_enum_t goal;
    /**<
     * The goal field is the current goal for the state machine. Goals are
     * defined to complete one at a time.
     *
     * CAUTION: The goal field is intended for the user of the state machine
     *          to indicate the goal. Any attempt by the state machine to
     *          manipulate the goal will lead to live locking and
     *          unmaintainable code.
     */
  vs_voc_state_enum_t state;
    /**<
     * The state field is the current state of the state machine.
     *
     * CAUTION: The state field is intended for the state machine to keep
     *          track of its current state. Any attempt to manipulate the
     *          state field from outside the state machine will lead to
     *          live locking and unmaintainable code.
     */
  vs_voc_action_enum_t action;
    /**<
     * The action field is the next action to perform by the state machine.
     *
     * CAUTION: The action field is intended by the state machine to keep
     *          track of the series of steps to transition out of the current
     *          state or to complete a goal. Any attempt to manipulate the
     *          action field from outside the state machine will lead to
     *          live locking, system panics due to unhandled actions in the
     *          state, and unmaintainable code.
     */
  uint32_t status;
    /**<
     * The status field reports the status of the last action.
     */
};

typedef struct vs_mvs_adapter_t vs_mvs_adapter_t;
struct vs_mvs_adapter_t
{
  uint16_t src_addr;
  uint16_t src_port;
  uint16_t dst_addr;
  uint16_t dst_port;
  uint32_t token;
};

/****************************************************************************
  THE SESSION OBJECT
****************************************************************************/
struct vs_session_object_t
{
  vs_object_header_t header;

  uint32_t vsid;
    /**< System level published/documented Voice System ID. */
  uint32_t client_id;
    /**< Indicates the network type over the call is going on. */

  uint32_t pending_media_id;
    /**< Indicates the vocoder type requested for the call. */
  uint32_t media_id;
    /**< Indicates the active vocoder type for the call. */

  vs_common_event_callback_fn_t event_cb;
    /**< Callback function registered by client to recive command
     * responses and events.
     */
  void* session_context;
    /**< Session context or client data provided during VS_VOC_CMD_OPEN. */

  apr_list_t alloc_buf_q;

  apr_list_t read_buf_q;

  apr_list_t write_buf_q;

  char_t cvd_session_name[ VS_MAX_CVD_SESSION_NAME_SIZE_V ];

  bool_t is_resource_granted;
    /**< Indicates that access to CVD resource is granted. */

  vs_call_state_enum_t call_state;
    /**< Represent the current state of voice call. */

  vs_voc_voice_timing_t voice_timing;

  vs_voc_property_t current_voc;

  vs_voc_property_t pending_voc;

  vs_control_t session_ctrl;
    /**< Maintains Voice session state, and it's state transition,
     * as per the policy management.
     */
  bool_t stream_ready;
    /**< Indicates that stream is running. */
  uint16_t cvs_port;
    /**< CVS session handle. */
  uint16_t mvm_port;
    /**< MVM session handle. */

  apr_lock_t client_state_lock;
    /**< session lock: lock to synchronize client state access. */
  apr_lock_t session_ctrl_lock;
    /**< session lock: lock to synchronize session_ctrl access. */
  apr_lock_t common_lock;
    /**< session lock: lock to synchronize shared sesson members access. */

  bool_t is_close_invoked;
    /**< Indicates that VS_VOC_CMD_CLOSE has been received. */

  vs_mvs_adapter_t mvs_adapter;
};

/****************************************************************************
  VS Client Session Destroy Lock Pool
****************************************************************************/
typedef struct vs_session_lock_pool_t vs_session_lock_pool_t;
struct vs_session_lock_pool_t
{
  uint32_t session_handle;
  apr_lock_t lock;
};

/****************************************************************************
  THE GENERIC VS OBJECT
****************************************************************************/
union vs_object_t
{
  vs_object_header_t header;
  vs_token_object_t handle;
  vs_simple_job_object_t simple_job;
  vs_sequencer_job_object_t sequencer_job;
  vs_session_object_t session;// not required
};

VS_INTERNAL uint32_t vs_queue_incoming_cmd_packet (
  vs_thread_priority_enum_t priority,
  vs_cmd_packet_t* p_packet
);

VS_INTERNAL uint32_t vs_queue_pending_cmd_packet (
  vs_thread_priority_enum_t priority,
  vs_cmd_packet_t* p_packet
);

VS_INTERNAL uint32_t vs_get_typed_object (
  uint32_t handle,
  vs_object_type_enum_t type,
  vs_object_t** ret_obj
);

VS_INTERNAL void vs_int_lock_fn (
  void
);

VS_INTERNAL void vs_int_unlock_fn (
  void
);

VS_INTERNAL void vs_voc_data_buf_lock_fn (
  void
);

VS_INTERNAL void vs_voc_data_buf_unlock_fn (
  void
);

VS_INTERNAL void vs_signal_run (
  vs_thread_priority_enum_t priority
);

VS_INTERNAL vs_call_state_enum_t vs_get_call_state (
  vs_session_object_t* p_session_obj
);

VS_INTERNAL void vs_signal_run_thrd1 (
  void
);

VS_INTERNAL void vs_signal_run_thrd2 (
  void
);

VS_INTERNAL void vs_signal_run_thrd3 (
  void
);

VS_INTERNAL uint32_t vs_common_send_event (
  uint32_t handle,
  uint32_t event_id,
  void* p_rsp,
  uint32_t rsp_size
);

VS_INTERNAL uint32_t vs_get_object (
  uint32_t handle,
  vs_object_t** ret_obj
);


/****************************************************************************
    SYNCHRONOUS BLOCKING API DEFINITIONS
****************************************************************************/
VS_INTERNAL uint32_t vs_voc_cmd_open (
  vs_voc_cmd_open_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_alloc_buffer (
  vs_voc_cmd_alloc_buffer_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_free_buffer (
  vs_voc_cmd_free_buffer_t* params
);

/****************************************************************************
    SYNCHRONOUS NON-BLOCKING API DEFINITIONS
****************************************************************************/
VS_INTERNAL uint32_t vs_voc_cmd_prime_read_buffer (
  vs_voc_cmd_prime_read_buffer_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_read_buffer (
  vs_voc_cmd_read_buffer_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_write_buffer (
  vs_voc_cmd_write_buffer_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_write_buffer_through (
  vs_voc_cmd_write_buffer_t* params
);


/****************************************************************************
    ASYNCHRONOUS NON-BLOCKING API DEFINITIONS
****************************************************************************/
VS_INTERNAL uint32_t vs_voc_cmd_enable (
  vs_voc_cmd_enable_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_disable (
  vs_voc_cmd_disable_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_standby (
  vs_voc_cmd_standby_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_flush_buffers (
  vs_voc_cmd_flush_buffers_t* params
);

VS_INTERNAL uint32_t vs_voc_cmd_close (
  vs_voc_cmd_close_t* params
);


/****************************************************************************
 * VOCODER PROPERTY FUNCTIONS                                      *
 ****************************************************************************/
/* AMR */
VS_INTERNAL uint32_t vs_vocamr_cmd_set_codec_mode (
  vs_vocamr_cmd_set_codec_mode_t* params
);

VS_INTERNAL uint32_t vs_vocamr_cmd_set_codec_mode_proc (
  uint32_t handle,
  vs_vocamr_codec_mode_t codec_mode
);

VS_INTERNAL uint32_t vs_vocamr_cmd_get_codec_mode (
  vs_vocamr_cmd_get_codec_mode_t* params
);

VS_INTERNAL uint32_t vs_vocamr_cmd_set_dtx_mode (
  vs_vocamr_cmd_set_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vocamr_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocamr_cmd_get_dtx_mode (
  vs_vocamr_cmd_get_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vocamr_set_cached_voc_property (
  uint32_t handle
);

/* AMR-WB */
VS_INTERNAL uint32_t vs_vocamrwb_cmd_set_codec_mode (
  vs_vocamrwb_cmd_set_codec_mode_t* params
);

VS_INTERNAL uint32_t vs_vocamrwb_cmd_set_codec_mode_proc (
  uint32_t handle,
  vs_vocamrwb_codec_mode_t codec_mode
);

VS_INTERNAL uint32_t vs_vocamrwb_cmd_set_dtx_mode (
  vs_vocamrwb_cmd_set_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vocamrwb_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocamrwb_cmd_get_codec_mode (
  vs_vocamrwb_cmd_get_codec_mode_t* params
);

VS_INTERNAL uint32_t vs_vocamrwb_cmd_get_dtx_mode (
  vs_vocamrwb_cmd_get_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vocamrwb_set_cached_voc_property (
  uint32_t handle
);

/* EFR */
VS_INTERNAL uint32_t vs_vocefr_cmd_set_dtx_mode (
  vs_vocefr_cmd_set_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vocefr_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocefr_cmd_get_dtx_mode (
  vs_vocefr_cmd_get_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vocefr_set_cached_voc_property (
  uint32_t handle
);

/* FR */
VS_INTERNAL uint32_t vs_vocfr_cmd_set_dtx_mode (
  vs_vocfr_cmd_set_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vocfr_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocfr_cmd_get_dtx_mode (
  vs_vocfr_cmd_get_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vocfr_set_cached_voc_property (
  uint32_t handle
);

/* HR */
VS_INTERNAL uint32_t vs_vochr_cmd_set_dtx_mode (
  vs_vochr_cmd_set_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vochr_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vochr_cmd_get_dtx_mode (
  vs_vochr_cmd_get_dtx_mode_t* params
);

VS_INTERNAL uint32_t vs_vochr_set_cached_voc_property (
  uint32_t handle
);

/* EVRC- A */
VS_INTERNAL uint32_t vs_vocevrca_cmd_set_dtx_mode (
  vs_vocevrca_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocevrca_cmd_set_frame_minmax_rate (
  vs_vocevrca_cmd_set_frame_minmax_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrca_cmd_set_enc_rate_mod (
  vs_vocevrca_cmd_set_enc_rate_mod_t* params
  );

VS_INTERNAL uint32_t vs_vocevrca_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocevrca_cmd_set_enc_rate_mod_proc (
  uint32_t handle,
  uint32_t rate_modulation
  );

VS_INTERNAL uint32_t vs_vocevrca_cmd_set_frame_minmax_rate_proc (
  uint32_t handle,
  vs_vocevrca_frame_rate_t min_rate,
  vs_vocevrca_frame_rate_t max_rate
);

VS_INTERNAL uint32_t vs_vocevrca_set_cached_voc_property (
  uint32_t handle
);

/* EVRC-B */

VS_INTERNAL uint32_t vs_vocevrcb_cmd_set_avg_enc_rate (
  vs_vocevrcb_cmd_set_avg_enc_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcb_cmd_set_frame_minmax_rate (
  vs_vocevrcb_cmd_set_frame_minmax_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcb_cmd_set_dtx_mode (
  vs_vocevrcb_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcb_cmd_set_avg_enc_rate_proc (
  uint32_t handle,
  uint32_t rate
  );

VS_INTERNAL uint32_t vs_vocevrcb_cmd_set_frame_minmax_rate_proc (
  uint32_t handle,
  vs_vocevrcb_frame_rate_t min_rate,
  vs_vocevrcb_frame_rate_t max_rate
);

VS_INTERNAL uint32_t vs_vocevrcb_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocevrcb_set_cached_voc_property (
  uint32_t handle
);

/* EVRC-WB */

VS_INTERNAL uint32_t vs_vocevrcwb_cmd_set_avg_enc_rate (
  vs_vocevrcwb_cmd_set_avg_enc_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcwb_cmd_set_frame_minmax_rate (
  vs_vocevrcwb_cmd_set_frame_minmax_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcwb_cmd_set_dtx_mode (
  vs_vocevrcwb_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcwb_cmd_set_avg_enc_rate_proc (
  uint32_t handle,
  uint32_t rate
  );

VS_INTERNAL uint32_t vs_vocevrcwb_cmd_set_frame_minmax_rate_proc (
  uint32_t handle,
  vs_vocevrcwb_frame_rate_t min_rate,
  vs_vocevrcwb_frame_rate_t max_rate
);

VS_INTERNAL uint32_t vs_vocevrcwb_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocevrcwb_set_cached_voc_property (
  uint32_t handle
);

/* EVRC-NW */

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_avg_enc_rate (
  vs_vocevrcnw_cmd_set_avg_enc_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_frame_minmax_rate (
  vs_vocevrcnw_cmd_set_frame_minmax_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_dtx_mode (
  vs_vocevrcnw_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_avg_enc_rate_proc (
  uint32_t handle,
  uint32_t rate
  );

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_frame_minmax_rate_proc (
  uint32_t handle,
  vs_vocevrcnw_frame_rate_t min_rate,
  vs_vocevrcnw_frame_rate_t max_rate
);

VS_INTERNAL uint32_t vs_vocevrcnw_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocevrcnw_set_cached_voc_property (
  uint32_t handle
);

/* EVRC-NW2K */

VS_INTERNAL uint32_t vs_vocevrcnw2k_cmd_set_avg_enc_rate (
  vs_vocevrcnw2k_cmd_set_avg_enc_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcnw2k_cmd_set_frame_minmax_rate (
  vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcnw2k_cmd_set_dtx_mode (
  vs_vocevrcnw2k_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocevrcnw2k_cmd_set_avg_enc_rate_proc (
  uint32_t handle,
  uint32_t rate
  );

VS_INTERNAL uint32_t vs_vocevrcnw2k_cmd_set_frame_minmax_rate_proc (
  uint32_t handle,
  vs_vocevrcnw2k_frame_rate_t min_rate,
  vs_vocevrcnw2k_frame_rate_t max_rate
);

VS_INTERNAL uint32_t vs_vocevrcnw2k_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocevrcnw2k_set_cached_voc_property (
  uint32_t handle
);

/* QCELP-13K */

VS_INTERNAL uint32_t vs_vocqcelp13k_cmd_set_avg_enc_rate (
  vs_vocqcelp13k_cmd_set_avg_enc_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocqcelp13k_cmd_set_enc_rate_mod (
  vs_vocqcelp13k_cmd_set_enc_rate_mod_t* params
  );

VS_INTERNAL uint32_t vs_vocqcelp13k_cmd_set_frame_minmax_rate (
  vs_vocqcelp13k_cmd_set_frame_minmax_rate_t* params
  );

VS_INTERNAL uint32_t vs_vocqcelp13k_cmd_set_dtx_mode (
  vs_vocqcelp13k_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocqcelp13k_cmd_set_avg_enc_rate_proc (
  uint32_t handle,
  uint32_t rate
  );

VS_INTERNAL uint32_t vs_vocqcelp13k_cmd_set_enc_rate_mod_proc (
  uint32_t handle,
  uint32_t rate_modulation
  );

VS_INTERNAL uint32_t vs_vocqcelp13k_cmd_set_frame_minmax_rate_proc (
  uint32_t handle,
  vs_vocqcelp13k_frame_rate_t min_rate,
  vs_vocqcelp13k_frame_rate_t max_rate
);

VS_INTERNAL uint32_t vs_vocqcelp13k_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocqcelp13k_set_cached_voc_property (
  uint32_t handle
);

/* G.729 */

VS_INTERNAL uint32_t vs_vocg729_cmd_set_dtx_mode (
  vs_vocg729_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocg729_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocg729_set_cached_voc_property (
  uint32_t handle
);

/* G.711 A-LAW */

VS_INTERNAL uint32_t vs_vocg711alaw_cmd_set_dtx_mode (
  vs_vocg711alaw_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocg711alaw_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
  );

VS_INTERNAL uint32_t vs_vocg711alaw_set_cached_voc_property (
  uint32_t handle
  );

/* G.711 MU-LAW */

VS_INTERNAL uint32_t vs_vocg711mulaw_cmd_set_dtx_mode (
  vs_vocg711mulaw_cmd_set_dtx_mode_t* params
  );

VS_INTERNAL uint32_t vs_vocg711mulaw_cmd_set_dtx_mode_proc (
  uint32_t handle,
  bool_t dtx_enable
);

VS_INTERNAL uint32_t vs_vocg711mulaw_set_cached_voc_property (
  uint32_t handle
);

/****************************************************************************
 * GATING COMMAND PROCESSING FUNCTIONS                                      *
 ****************************************************************************/
VS_INTERNAL uint32_t vs_voc_cmd_enable_proc (
  vs_cmd_packet_t* p_vs_cmd_pkt
);

VS_INTERNAL uint32_t vs_voc_cmd_disable_proc (
  vs_cmd_packet_t* p_vs_cmd_pkt
);

VS_INTERNAL uint32_t vs_voc_cmd_standby_proc (
  vs_cmd_packet_t* p_vs_cmd_pkt
);

VS_INTERNAL uint32_t vs_voc_cmd_close_proc (
  vs_cmd_packet_t* p_vs_cmd_pkt
);

VS_INTERNAL uint32_t vs_voc_cmd_flush_buffers_proc (
  vs_cmd_packet_t* p_vs_cmd_pkt
);

VS_INTERNAL uint32_t vs_voc_cmdi_destroy_session_proc (
  vs_cmd_packet_t* p_vs_cmd_pkt
);

VS_INTERNAL uint32_t vs_voc_cmdi_enable_proc (
  vs_gating_control_t* ctrl
);

VS_INTERNAL uint32_t vs_voc_cmdi_standby_proc (
  vs_gating_control_t* ctrl
);

VS_INTERNAL uint32_t vs_voc_cmdi_disable_proc (
  vs_gating_control_t* ctrl
);

VS_INTERNAL uint32_t vs_voc_cmdi_flush_buffers_proc (
  vs_gating_control_t* ctrl
);

VS_INTERNAL uint32_t vs_voc_cmdi_close_proc (
  vs_gating_control_t* ctrl
);

VS_INTERNAL uint32_t vs_voc_cmdi_cleanup_cvd_reference_proc (
  vs_gating_control_t* ctrl
);

VS_INTERNAL uint32_t vs_voc_cmdi_disable_cvd_on_adsp_recovery_proc (
  vs_gating_control_t* ctrl
);

VS_INTERNAL uint32_t vs_voc_cmdi_destroy_cvd_on_adsp_recovery_proc (
  vs_gating_control_t* ctrl
);

VS_INTERNAL uint32_t vs_voc_cmdi_restore_cvd_state_proc (
  vs_gating_control_t* ctrl
);

/****************************************************************************
 * VS STATE MACHINE FUNCTIONS                                               *
 ****************************************************************************/
VS_INTERNAL uint32_t vs_voc_run_state_control (
  void
);

VS_INTERNAL uint32_t vs_voc_state_control (
  vs_session_object_t* session_obj
);

/****************************************************************************
 * VS SSR FUNCTIONS                                                         *
 ****************************************************************************/

VS_INTERNAL uint32_t vs_voc_ssr_init (
  void
);

VS_INTERNAL uint32_t vs_voc_ssr_deinit (
  void
);

/****************************************************************************
 * OTHER FUNCTIONS                                                          *
 ****************************************************************************/

/* This set the cached stream properties. */
VS_INTERNAL uint32_t vs_voc_action_set_cached_stream_properties (
  vs_session_object_t* session_obj
);

/* this function gets decoder packet from modem and adds packet header */
VS_INTERNAL uint32_t vs_voc_send_enc_packet (
  uint32_t session_handle,
  vs_voc_buffer_item_t* p_rbuffer_node,
  aprv2_packet_t* p_packet
);

/* this function gets decoder packet from modem and adds packet header */
VS_INTERNAL uint32_t vs_voc_send_dec_packet (
  uint32_t session_handle,
  vs_voc_buffer_t* p_wbuffer,
  aprv2_packet_t* p_packet
);

/**
 * Maps the EVRC-A vocoder frame type (CVD to VS).
 *
 */
VS_INTERNAL vs_vocevrca_frame_rate_t cvd_to_vs_map_evrca_frame_rate (
  uint8_t rate
);

/**
 * Maps the EVRC-A vocoder frame type (VS to CVD).
 *
 */
VS_INTERNAL uint8_t vs_to_cvd_map_evrca_frame_rate (
  vs_vocevrca_frame_rate_t rate
);

/**
 * Maps the EVRC-B vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrcb_frame_rate_t cvd_to_vs_map_evrcb_frame_rate (
  uint8_t rate
);

/**
 * Maps the EVRC-B vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrcb_frame_rate (
  vs_vocevrcb_frame_rate_t rate
);

/**
 * Maps the EVRC-WB vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrcwb_frame_rate_t cvd_to_vs_map_evrcwb_frame_rate (
  uint8_t rate
);

/**
 * Maps the EVRC-WB vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrcwb_frame_rate (
  vs_vocevrcwb_frame_rate_t rate
);

/**
 * Maps the EVRC-NW vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrcnw_frame_rate_t cvd_to_vs_map_evrcnw_frame_rate (
  uint8_t rate
);

/**
 * Maps the EVRC-NW vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrcnw_frame_rate (
  vs_vocevrcnw_frame_rate_t rate
);

/**
 * Maps the EVRC-NW2k vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocevrcnw2k_frame_rate_t cvd_to_vs_map_evrcnw2k_frame_rate (
  uint8_t rate
);

/**
 * Maps the EVRC-NW2K vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_evrcnw2k_frame_rate (
  vs_vocevrcnw2k_frame_rate_t rate
);

/**
 * Maps the QCELP-13K vocoder frame type (CVD to VS).
 *
 */

VS_INTERNAL vs_vocqcelp13k_frame_rate_t cvd_to_vs_map_qcelp13k_frame_rate (
  uint8_t rate
);

/**
 * Maps the QCELP-13K vocoder frame type (VS to CVD).
 *
 */

VS_INTERNAL uint8_t vs_to_cvd_map_qcelp13k_frame_rate (
  vs_vocqcelp13k_frame_rate_t rate
);


/**
 * This function will determine number of bytes of AMR vocoder frame length
 * based on the frame type and frame rate.
 *
 * returns "number of bytes" when successful.
 */
VS_INTERNAL uint32_t vs_vocamr_get_frame_len (
  vs_vocamr_frame_type_t frame_type,
  vs_vocamr_codec_mode_t amr_mode
);

/**
 * This function will determine number of bytes of AMR-WB vocoder frame length
 * based on the frame type and frame rate.
 *
 * returns "number of bytes" when successful.
 */
VS_INTERNAL uint32_t vs_vocamrwb_get_frame_len (
  vs_vocamrwb_frame_type_t frame_type,
  vs_vocamrwb_codec_mode_t amrwb_mode
);


VS_INTERNAL uint32_t vs_create_autofree_token_object (
  vs_token_object_t** p_ret_obj
);

VS_INTERNAL uint32_t vs_create_simple_job_object (
  uint32_t context_handle,
  vs_simple_job_object_t** p_ret_job_obj
);

VS_INTERNAL uint32_t vs_create_sequencer_job_object (
  vs_sequencer_job_object_t** p_ret_job_obj
);

VS_INTERNAL void vs_simple_result_rsp_fn (
  aprv2_packet_t* p_packet
);

VS_INTERNAL void vs_simple_transition_result_rsp_fn (
  aprv2_packet_t* p_packet
);

VS_INTERNAL uint32_t vs_mem_alloc_object (
  uint32_t size,
  vs_object_t** p_ret_object
);

VS_INTERNAL uint32_t vs_mem_free_object (
  vs_object_t* p_object
);

/* Track the allocated VS objects with type VS_OBJECT_TYPE_ENUM_SIMPLE_JOB and
 * VS_OBJECT_TYPE_ENUM_HANDLE, in order to free them in case of ADSP SSR. VS
 * allocates job object with one of the above types when communicating with the
 * ADSP and free the object once a response is received from the ADSP. If VS
 * has issued commands to the ADSP and ADSP SSR occurs, VS will not get
 * responses from the ADSP and thus the objects will not be freed. Therefore
 * VS need to track these types of allocated objects and free them upon ADSP
 * SSR.
 */
VS_INTERNAL uint32_t vs_voc_ssr_track_object (
  uint32_t obj_handle
);

/* Untrack the VS objects with type VS_OBJECT_TYPE_ENUM_SIMPLE_JOB and
 * VS_OBJECT_TYPE_ENUM_HANDLE.
 */
VS_INTERNAL uint32_t vs_voc_ssr_untrack_object (
  uint32_t obj_handle
);

/* Free all of the tracked VS objects. */
VS_INTERNAL uint32_t vs_voc_ssr_free_all_tracked_objects (
  void
);

VS_INTERNAL uint32_t vs_prepare_and_dispatch_cmd_packet (
  vs_thread_priority_enum_t priority,
  uint32_t cmd_id,
  void* p_params,
  uint32_t params_size
);

VS_INTERNAL uint32_t vs_free_cmd_packet (
  vs_cmd_packet_t* p_cmd_packet
);

void vs_assign_session_destroy_lock (
  uint32_t session_handle
);

void vs_clear_session_destroy_lock (
  uint32_t session_handle
);

apr_lock_t* vs_get_session_destroy_lock (
  uint32_t session_handle
);

VS_INTERNAL uint32_t vs_voc_rspi_destroy_proc (
  vs_cmd_packet_t* p_vs_cmd_pkt
);

VS_INTERNAL vs_voc_client_type_t vs_voc_check_client_type (
  uint32_t client_id
);
#endif  /* __VS_I_H__ */

