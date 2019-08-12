#ifndef __VS_VOCG711ALAW_H__
#define __VS_VOCG711ALAW_H__

/**
  @file vs_vocg711alaw.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for G711 A-Law vocoder

  G711 A-Law : G.711 also known as Pulse Code Modulation is a narrawband
  audio codec that provides audio at 64 kbits/s

  A-Law : A-Law encoding takes a 13-bit signed linear audio sample as input
  and converts it into 8bit value.
*/

/*
  ============================================================================

   Copyright (C) 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocg711alaw.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  07/22/14  shr   Initial revision

  ============================================================================
*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "vs_common.h"

/** @addtogroup g711alaw_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** G.711 A-Law vocoder format. */
#define VS_COMMON_MEDIA_ID_G711ALAW ( 0x000130D5 )

/*----------------------------------------------------------------------------
   Start of Enumerations
----------------------------------------------------------------------------*/

/**
 * G.711 A-Law frame type information. 
 * Refer to Table 3-22 in @xhyperref{Q5, [Q5]}.
 */
typedef enum vs_vocg711alaw_frame_type_e
{
  VS_VOCG711ALAW_FRAME_SPEECH_TYPE = 0,
    /**< Speech or active frame (full rate). */
  VS_VOCG711ALAW_FRAME_SID_TYPE = 1,
    /**< SID frame. */
  VS_VOCG711ALAW_FRAME_NTX_TYPE = 2,
    /**< No transmission frame. */
  VS_VOCG711ALAW_FRAME_ERASURE_TYPE = 3,
    /**< Erasure flag; applicable for downlink only. */
} /** @cond */
  vs_vocg711alaw_frame_type_t/** @endcond */;

/*  End of Enumerations */

/*----------------------------------------------------------------------------
   Start of Structures
----------------------------------------------------------------------------*/


typedef struct vs_vocg711alaw_frame_info_t
  vs_vocg711alaw_frame_info_t;

/**
 * G.711 A-Law frame information structure.
 */
struct vs_vocg711alaw_frame_info_t
{
  vs_vocg711alaw_frame_type_t frame_type;
      /**< G.711 A-Law frame type. */
};

/** @} */

/** @addtogroup set_g711alaw_dtx_mode
@{ */

/**
 * Sets the DTX mode of the G.711 A-Law vocoder.
 */
#define VS_VOCG711ALAW_CMD_SET_DTX_MODE ( 0x000130CA )

typedef struct vs_vocg711alaw_cmd_set_dtx_mode_t 
  vs_vocg711alaw_cmd_set_dtx_mode_t;

/** Set DTX Mode command parameters. */
struct vs_vocg711alaw_cmd_set_dtx_mode_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  bool_t enable_flag;
      /**< Enable flag: \n
        *  - TRUE -- Enable DTX \n
        *  - FALSE -- Disable DTX @tablebulletend
        */
};

/*  End of Structures */

/** @} */

#endif /* __VS_VOCG711ALAW_H__ */
