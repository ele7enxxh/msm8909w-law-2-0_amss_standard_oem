#ifndef __VS_VOCG711MULAW_H__
#define __VS_VOCG711MULAW_H__

/**
  @file vs_vocg711mulaw.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for G711 MU-Law vocoder

  G711 MU-Law : G.711 also known as Pulse Code Modulation is a narrawband
  audio codec that provides audio at 64 kbits/s

  MU-Law : G.711Mu encoding takes a 14-bit signed linear audio sample as 
  input, increases the magnitude by 32 (binary 100000), and converts it 
  to an 8 bit value.

*/

/*
  ============================================================================

   Copyright (C) 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocg711mulaw.h#1 $
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

/** @addtogroup g711mulaw_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** G.711 Mu-Law vocoder format. */
#define VS_COMMON_MEDIA_ID_G711MULAW ( 0x000130D6 )

/*----------------------------------------------------------------------------
   Start of Enumerations
----------------------------------------------------------------------------*/


/**
 * G.711 Mu-Law frame type information. 
 * Refer to Section 3.12.2 in @xhyperref{Q5, [Q5]}.
 */
typedef enum vs_vocg711mulaw_frame_type_e
{
  VS_VOCG711MULAW_FRAME_SPEECH_TYPE = 0,
    /**< Speech or active frame (full rate). */
  VS_VOCG711MULAW_FRAME_SID_TYPE = 1,
    /**< SID frame. */
  VS_VOCG711MULAW_FRAME_NTX_TYPE = 2,
    /**< No transmission frame. */
  VS_VOCG711MULAW_FRAME_ERASURE_TYPE = 3,
    /**< Erasure flag; applicable for downlink only. */
} /** @cond */
  vs_vocg711mulaw_frame_type_t/** @endcond */;

/*  End of Enumerations */

/*----------------------------------------------------------------------------
   Start of Structures
----------------------------------------------------------------------------*/


typedef struct vs_vocg711mulaw_frame_info_t
  vs_vocg711mulaw_frame_info_t;

/**
 * G.711 Mu-Law frame information structure.
 */
struct vs_vocg711mulaw_frame_info_t
{
  vs_vocg711mulaw_frame_type_t frame_type;
      /**< G.711 Mu-Law frame type. */
};

/** @} */

/** @addtogroup set_g711mulaw_dtx_mode
@{ */

/**
 * Sets the DTX mode of the G.711 Mu-Law vocoder.
 */
#define VS_VOCG711MULAW_CMD_SET_DTX_MODE ( 0x000130CB )

typedef struct vs_vocg711mulaw_cmd_set_dtx_mode_t 
  vs_vocg711mulaw_cmd_set_dtx_mode_t;

/** Set DTX mode command parameters. */
struct vs_vocg711mulaw_cmd_set_dtx_mode_t
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

#endif /* __VS_VOCG711MULAW_H__ */

