#ifndef __VS_VOCAMR_H__
#define __VS_VOCAMR_H__

/**
  @file vs_vocamr.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for AMR vocoder
*/

/*
  ============================================================================

   Copyright (C) 2012, 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocamr.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  12/06/12  sud   Initial revision

  ============================================================================
*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "mmdefs.h"
#include "vs_errcodes.h"
#include "vs_common.h" 

/** @addtogroup amr_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** Adaptive Multirate (AMR) vocoder format. */
#define VS_COMMON_MEDIA_ID_AMR ( 0x00012E1F )

/*----------------------------------------------------------------------------
   Start of Enumerations
----------------------------------------------------------------------------*/




/**
 * AMR vocoder frame types. This is used to specify
 * the contents of the AMR vocoder frame.
 *
 * These frame types are applicable to both uplink and downlink unless
 * explicitly stated otherwise.
 */
typedef enum vs_vocamr_frame_type_t
{
  VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD = 0,
      /**< Good speech frame. */
  VS_VOCAMR_FRAME_TYPE_SPEECH_DEGRADED = 1,
      /**< Degraded speech frame. */
  VS_VOCAMR_FRAME_TYPE_ONSET = 2,
      /**< Onset -- Announces the beginning of a speech burst. This
        * does not contain information bits.
        */
  VS_VOCAMR_FRAME_TYPE_SPEECH_BAD = 3,
      /**< Corrupt speech frame (bad CRC). */
  VS_VOCAMR_FRAME_TYPE_SID_FIRST = 4,
      /**< Marks the beginning of a comfort noise period (silence).
        * This does not contain information bits.
        */
  VS_VOCAMR_FRAME_TYPE_SID_UPDATE = 5,
      /**< Comfort noise frame. Contains information bits.
        */
  VS_VOCAMR_FRAME_TYPE_SID_BAD = 6,
      /**< Comfort noise frame (bad CRC).    */
  VS_VOCAMR_FRAME_TYPE_NO_DATA = 7,
      /**< No useful information (nothing to transmit).
        */
} /** @cond */
  vs_vocamr_frame_type_t/** @endcond */;

/**
 * AMR codec mode. This denotes all the AMR codec bitrates.
 */
typedef enum vs_vocamr_codec_mode_t
{
  VS_VOCAMR_CODEC_MODE_0475 = 0, /**< 4.75 kbps. */
  VS_VOCAMR_CODEC_MODE_0515 = 1, /**< 5.15 kbps. */
  VS_VOCAMR_CODEC_MODE_0590 = 2, /**< 5.90 kbps. */
  VS_VOCAMR_CODEC_MODE_0670 = 3, /**< 6.70 kbps. */
  VS_VOCAMR_CODEC_MODE_0740 = 4, /**< 7.40 kbps. */
  VS_VOCAMR_CODEC_MODE_0795 = 5, /**< 7.95 kbps. */
  VS_VOCAMR_CODEC_MODE_1020 = 6, /**< 10.2 kbps. */
  VS_VOCAMR_CODEC_MODE_1220 = 7, /**< 12.2 kbps. */
} /** @cond */
  vs_vocamr_codec_mode_t/** @endcond */;

/*  End of Enumerations */

/*----------------------------------------------------------------------------
   Start of Structures
----------------------------------------------------------------------------*/


typedef struct vs_vocamr_frame_info_t vs_vocamr_frame_info_t;

/**
 * AMR frame information structure.
 */
struct vs_vocamr_frame_info_t
{
  vs_vocamr_frame_type_t frame_type;
      /**< AMR frame type. */
  vs_vocamr_codec_mode_t codec_mode;
      /**< AMR codec mode. */
};

/** @} */

/** @addtogroup set_amr_codec_mode
@{ */

/**
 * Sets the codec mode of the AMR vocoder.
 */
#define VS_VOCAMR_CMD_SET_CODEC_MODE ( 0x00012E34 )

typedef struct vs_vocamr_cmd_set_codec_mode_t vs_vocamr_cmd_set_codec_mode_t;

/** Set Codec Mode command parameters. */
struct vs_vocamr_cmd_set_codec_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocamr_codec_mode_t codec_mode;
      /**< AMR codec mode. */
};

/** @} */

/** @addtogroup get_amr_codec_mode
@{ */

/**
 * Gets the codec mode of the AMR vocoder.
 */
#define VS_VOCAMR_CMD_GET_CODEC_MODE ( 0x00012E35 )

typedef struct vs_vocamr_cmd_get_codec_mode_t vs_vocamr_cmd_get_codec_mode_t;

/** Get Codec Mode command parameters. */
struct vs_vocamr_cmd_get_codec_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocamr_codec_mode_t* ret_codec_mode;
      /**< AMR codec mode returned to the client. */
};

/** @} */

/** @addtogroup set_amr_dtx_mode
@{ */

/**
 * Sets the DTX mode of the AMR vocoder.
 */
#define VS_VOCAMR_CMD_SET_DTX_MODE ( 0x00012E36 )

typedef struct vs_vocamr_cmd_set_dtx_mode_t vs_vocamr_cmd_set_dtx_mode_t;

/** Set DTX mode command parameters. */
struct vs_vocamr_cmd_set_dtx_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  bool_t enable_flag;
      /**< Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX @tablebulletend
        */
};

/** @} */

/** @addtogroup get_amr_dtx_mode
@{ */

/**
 * Gets the DTX mode of the AMR vocoder.
 */
#define VS_VOCAMR_CMD_GET_DTX_MODE ( 0x00012E37 )

typedef struct vs_vocamr_cmd_get_dtx_mode_t vs_vocamr_cmd_get_dtx_mode_t;

/** Get DTX Mode command parameters. */
struct vs_vocamr_cmd_get_dtx_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  bool_t* ret_enable_flag;
      /**< Returned enable flag value: \n
        *  - TRUE -- DTX is enabled \n
        *  - FALSE -- DTX is disabled @tablebulletend
        */
};

/*  End of Structures */

/** @} */

#endif /* __VS_VOCAMR_H__ */

