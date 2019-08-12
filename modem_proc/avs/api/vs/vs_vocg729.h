#ifndef __VS_VOCG729_H__
#define __VS_VOCG729_H__

/**
  @file vs_vocg729.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for G729 vocoder

  G.729 : Coding of speech at 8 kbits/s using conjugate-structure
  algebraic-code-excited linear prediction (CS-ACELP)

  This supports G.729 Annex A and G.729 Annex B

*/

/*
  ============================================================================

   Copyright (C) 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocg729.h#1 $
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

/** @addtogroup g729_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** G.729 vocoder format. */
#define VS_COMMON_MEDIA_ID_G729 ( 0x000130D4 )

/*----------------------------------------------------------------------------
   Start of Enumerations
----------------------------------------------------------------------------*/


/**
 * G.729 frame type information. 
 * Refer to Table 3-24 in @xhyperref{Q5, [Q5]}.
 */
typedef enum vs_vocg729_frame_type_e
{
  VS_VOCG729_FRAME_BLANK_TYPE = 0,
    /**< Blank -- No data. */
  VS_VOCG729_FRAME_SPEECH_TYPE = 1,
    /**< Speech frame. */
  VS_VOCG729_FRAME_SID_TYPE = 2,
    /**< SID frame. */
} /** @cond */
  vs_vocg729_frame_type_t/** @endcond */;

/*  End of Enumerations */

/*----------------------------------------------------------------------------
   Start of Structures
----------------------------------------------------------------------------*/


typedef struct vs_vocg729_frame_info_t
  vs_vocg729_frame_info_t;

/**
 * G.729 frame information structure.
 */
struct vs_vocg729_frame_info_t
{
  vs_vocg729_frame_type_t frame_type;
      /**< G.729 frame type. */
};

/** @} */

/** @addtogroup set_g729_dtx_mode
@{ */

/**
 * Sets the DTX mode of the G.729 vocoder.
 */
#define VS_VOCG729_CMD_SET_DTX_MODE ( 0x000130C9 )

typedef struct vs_vocg729_cmd_set_dtx_mode_t 
  vs_vocg729_cmd_set_dtx_mode_t;

/** Set DTX mode command parameters. */
struct vs_vocg729_cmd_set_dtx_mode_t
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

#endif /* __VS_G729_H__ */
