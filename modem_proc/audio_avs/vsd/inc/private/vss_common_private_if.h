#ifndef __VSS_COMMON_PRIVATE_IF_H__
#define __VSS_COMMON_PRIVATE_IF_H__

/*
  Copyright (C) 2010-2014 QUALCOMM Technologies, Inc.
  All Rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/private/vss_common_private_if.h#1 $
   $Author: mplcsds1 $
*/

#include "apr_comdef.h"
#include "vss_common_public_if.h"


/****************************************************************************
 * VOICE SYSTEM CONFIG APIS                                                 *
 ****************************************************************************/

/**
  Command issued by the MVM to attached streams and vocprocs, to propagate
  the current system configuration.

  @par Payload
  #vss_icommon_cmd_set_system_config_t

  @return
  #VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG -- In case of success.
  #APRV2_IBASIC_RSP_RESULT -- In case of failure.
 
  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG ( 0x00012A0F )

/** @brief Type definition for #vss_icommon_cmd_set_system_config_t.
*/
typedef struct vss_icommon_cmd_set_system_config_t vss_icommon_cmd_set_system_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_SYSTEM_COPNFIG command.
*/
struct vss_icommon_cmd_set_system_config_t
{
  uint32_t network_id;
    /** Network ID. Supported values: \n
        see @latexonly \hyperref[cal_network_ID]{Calibration Network IDs}
        @endlatexonly. */
  uint32_t media_id;
    /** Cal media ID requested by the client. Supported values: \n
        see @latexonly \hyperref[media_ID]{Media Type IDs}@endlatexonly. */
  uint32_t rx_voc_op_mode;
    /** Rx vocoder operating mode. Supported values: \n
        see @latexonly \hyperref[cal_voc_op_mode]{Calibration Vocoder Operating
        Modes}@endlatexonly. */
  uint32_t tx_voc_op_mode;
    /** Tx vocoder operating mode. Supported values: \n
        see @latexonly \hyperref[cal_voc_op_mode]{Calibration Vocoder Operating
        Modes}@endlatexonly. */
  uint32_t dec_sr;
    /** Decoder sampling rate. */
  uint32_t enc_sr;
    /** Encoder sampling rate. */
  uint32_t rx_pp_sr;
    /** RX post-processing blocks sampling rate. */
  uint32_t tx_pp_sr;
    /** TX pre-processing blocks sampling rate. */
  uint32_t feature;
    /** Calibration feature. Supported values: \n
        see @latexonly \hyperref[cal_feature]{Calibration Features}
        @endlatexonly). */
  uint32_t vsid;
    /**< Voice System ID as defined by DCN 80-NF711-1. */
  uint16_t vfr_mode;
    /**< Vocoder frame synchronization mode. Possible values:\n
         - #VSS_ICOMMON_VFR_MODE_SOFT -- No frame synchronization. \n
         - #VSS_ICOMMON_VFR_MODE_HARD -- Hard Vocoder Frame Reference (VFR). 
           A 20 ms VFR interrupt. */
  uint16_t timewarp_mode;
    /**< Timewarping mode. Possible values:\n
         - 0x0001 -- Timewarping enabled.
         - 0x0000 -- Timewarping disabled. */
  uint32_t call_num;
    /**< CVD Call Number. */
}
#include "apr_pack_end.h"
;


/**
  Response to the #VSS_ICOMMON_CMD_SET_SYSTEM_CONFIG command.

  @par Payload
  #vss_icommon_rsp_set_system_config_t

  @return
  None.

  @dependencies
  None.
*/

#define VSS_ICOMMON_RSP_SET_SYSTEM_CONFIG ( 0x00012E69 )

/** @brief Type definition for #vss_icommon_rsp_set_system_config_t.
*/
typedef struct vss_icommon_rsp_set_system_config_t vss_icommon_rsp_set_system_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_RSP_SET_SYSTEM_COPNFIG command.
*/
struct vss_icommon_rsp_set_system_config_t
{
  uint32_t dec_sr;
    /** Decoder sampling rate. */
  uint32_t enc_sr;
    /** Encoder sampling rate. */
  uint32_t rx_pp_sr;
    /** RX post-processing blocks sampling rate. */
  uint32_t tx_pp_sr;
    /** TX pre-processing blocks sampling rate. */
  uint32_t vocproc_tx_topology_id;
    /**< Vocproc Tx path topology ID. */
  uint32_t vocproc_rx_topology_id;
    /**< Vocproc Rx path topology ID. */
  uint32_t enc_kpps;
    /**< Encoder KPPS requirements. */
  uint32_t dec_kpps;
    /**< Decoder KPPS requirements. */
  uint32_t dec_pp_kpps;
    /**< Stream RX post-processing block KPPS requirements. */
  uint32_t vp_rx_kpps;
    /**< Vocproc RX KPPS requirements. */
  uint32_t vp_tx_kpps;
    /**< Vocproc TX KPPS requirements. */
  uint16_t timewarp_mode;
    /**< Timewarping mode. Possible values:\n
         - 0x0001 -- Timewarping enabled.
         - 0x0000 -- Timewarping disabled. */
}
#include "apr_pack_end.h"
;


/**
  Command issued by the MVM to attached vocprocs, to propagate the dynamic
  system configuration changes during run-time.

  @par Payload
  #vss_icommon_cmd_set_dynamic_system_config_t

  @return
  #APRV2_IBASIC_RSP_RESULT
 
  @dependencies
  None.
*/
#define VSS_ICOMMON_CMD_SET_DYNAMIC_SYSTEM_CONFIG ( 0x000130B2 )

/** @brief Type definition for #vss_icommon_cmd_set_dynamic_system_config_t.
*/
typedef struct vss_icommon_cmd_set_dynamic_system_config_t vss_icommon_cmd_set_dynamic_system_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_ICOMMON_CMD_SET_DYNAMIC_SYSTEM_CONFIG command.
*/
struct vss_icommon_cmd_set_dynamic_system_config_t
{
  uint32_t rx_voc_op_mode;
    /** Rx vocoder operating mode. Supported values: \n
        see @latexonly \hyperref[cal_voc_op_mode]{Calibration Vocoder Operating
        Modes}@endlatexonly. */
  uint32_t tx_voc_op_mode;
    /** Tx vocoder operating mode. Supported values: \n
        see @latexonly \hyperref[cal_voc_op_mode]{Calibration Vocoder Operating
        Modes}@endlatexonly. */
  uint32_t feature_id;
    /** Calibration feature. Supported values: \n
        see @latexonly \hyperref[cal_feature]{Calibration Features}
        @endlatexonly). */
  uint32_t rx_pp_sr;
    /** RX post-processing blocks sampling rate. */
}
#include "apr_pack_end.h"
;

#endif /* __VSS_COMMON_PRIVATE_IF_H__ */

