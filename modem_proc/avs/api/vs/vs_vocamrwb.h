#ifndef __VS_VOCAMRWB_H__
#define __VS_VOCAMRWB_H__

/**
  @file vs_vocamrwb.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for AMR-WB vocoder
*/

/*
  ============================================================================

   Copyright (C) 2012, 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocamrwb.h#1 $
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

/** @addtogroup amrwb_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** Adaptive Multirate Wideband (AMR-WB) vocoder format. */
#define VS_COMMON_MEDIA_ID_AMRWB ( 0x00012E20 )

/*----------------------------------------------------------------------------
   Start of Enumerations
----------------------------------------------------------------------------*/




/**
 * AMR wideband vocoder frame types, used to specify the contents of
 * the AMR-WB vocoder frame. These frame types are applicable to both
 * uplink and downlink unless explicitly stated otherwise.
 */
typedef enum vs_vocamrwb_frame_type_t
{
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD = 0,
      /**< Good speech frame. */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_PROBABLY_DEGRADED = 1,
      /**< Speech frame with a good CRC, but there may
        * be errors in the class2 bits.
        */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_LOST = 2,
      /**< No frame recieved. */
  VS_VOCAMRWB_FRAME_TYPE_SPEECH_BAD = 3,
      /**< Corrupt speech frame (bad CRC). */
  VS_VOCAMRWB_FRAME_TYPE_SID_FIRST = 4,
  /**< Marks the beginning of a comfort noise period (silence).
    * This does not contain information bits.
    */
  VS_VOCAMRWB_FRAME_TYPE_SID_UPDATE = 5,
  /**< Comfort noise frame.
    *  This contains inforamtion bits.
    */
  VS_VOCAMRWB_FRAME_TYPE_SID_BAD = 6,
      /**< Comfort noise frame (bad CRC). */
  VS_VOCAMRWB_FRAME_TYPE_NO_DATA = 7
  /**< No useful information (nothing to transmit).
    */
} /** @cond */
  vs_vocamrwb_frame_type_t/** @endcond */;
 

/**
 * AMR-WB codec mode. This denotes all the AMR-WB codec
 * bitrates.
 */
typedef enum vs_vocamrwb_codec_mode_t
{
  VS_VOCAMRWB_CODEC_MODE_0660 = 0, /**< 6.60 kbps. */
  VS_VOCAMRWB_CODEC_MODE_0885 = 1, /**< 8.85 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1265 = 2, /**< 12.65 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1425 = 3, /**< 14.25 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1585 = 4, /**< 15.85 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1825 = 5, /**< 18.25 kbps. */
  VS_VOCAMRWB_CODEC_MODE_1985 = 6, /**< 19.85 kbps. */
  VS_VOCAMRWB_CODEC_MODE_2305 = 7, /**< 23.05 kbps. */
  VS_VOCAMRWB_CODEC_MODE_2385 = 8  /**< 23.85 kbps. */
} /** @cond */
  vs_vocamrwb_codec_mode_t/** @endcond */;

/*  End of Enumerations */

/*----------------------------------------------------------------------------
   Start of Structures
----------------------------------------------------------------------------*/


typedef struct vs_vocamrwb_frame_info_t vs_vocamrwb_frame_info_t;

/**
 *  AMR-WB frame information structure.
 */
struct vs_vocamrwb_frame_info_t
{
  vs_vocamrwb_frame_type_t frame_type;
      /**< AMR-WB frame type. */
  vs_vocamrwb_codec_mode_t codec_mode;
      /**< AMR-WB codec mode. */
};

/** @} */

/** @addtogroup set_amrwb_codec_mode
@{ */

/**
 * Sets the codec mode of the AMR-WB vocoder.
 */
#define VS_VOCAMRWB_CMD_SET_CODEC_MODE ( 0x00012E38 )

typedef struct vs_vocamrwb_cmd_set_codec_mode_t vs_vocamrwb_cmd_set_codec_mode_t;

/** Set Codec Mode command parameters. */
struct vs_vocamrwb_cmd_set_codec_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocamrwb_codec_mode_t codec_mode;
      /**< AMR-WB codec mode. */
};

/** @} */

/** @addtogroup get_amrwb_codec_mode
@{ */

/**
 * Gets the codec mode of the AMR-WB vocoder.
 */
#define VS_VOCAMRWB_CMD_GET_CODEC_MODE ( 0x00012E39 )

typedef struct vs_vocamrwb_cmd_get_codec_mode_t vs_vocamrwb_cmd_get_codec_mode_t;

/** Get Codec Mode command parameters. */
struct vs_vocamrwb_cmd_get_codec_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocamrwb_codec_mode_t* ret_codec_mode;
      /**< AMR-WB codec mode returned to the client. */
};

/** @} */

/** @addtogroup set_amrwb_dtx_mode
@{ */

/**
 * Sets the DTX mode of the AMR-WB vocoder.
 */
#define VS_VOCAMRWB_CMD_SET_DTX_MODE ( 0x00012E3A )

typedef struct vs_vocamrwb_cmd_set_dtx_mode_t vs_vocamrwb_cmd_set_dtx_mode_t;

/** Set DTX Mode command parameters. */
struct vs_vocamrwb_cmd_set_dtx_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  bool_t enable_flag;
      /**< Enable flag: \n
       *   - TRUE -- Enable DTX \n
       *   - FALSE -- Disable DTX @tablebulletend
       */
};

/** @} */

/** @addtogroup get_amrwb_dtx_mode
@{ */

/**
 * Gets the DTX mode of the AMR-WB Vocoder.
 */
#define VS_VOCAMRWB_CMD_GET_DTX_MODE ( 0x00012E3B )

typedef struct vs_vocamrwb_cmd_get_dtx_mode_t vs_vocamrwb_cmd_get_dtx_mode_t;

/** Get DTX Mode command parameters. */
struct vs_vocamrwb_cmd_get_dtx_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  bool_t* ret_enable_flag;
      /**< Returned enable flag value: \n
       *   - TRUE -- DTX is enabled \n
       *   - FALSE -- DTX is disabled @tablebulletend
       */
};

/*  End of Structures */

/** @} */

#endif /* __VS_VOCAMRWB_H__ */

