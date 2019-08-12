#ifndef __VS_VOCEVRCNW2K_H__
#define __VS_VOCEVRCNW2K_H__

/**
  @file vs_vocevrcnw2k.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for EVRC-NW2K vocoder

*/

/*
  ============================================================================

   Copyright (C) 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocevrcnw2k.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  07/18/14  shr   Initial revision

  ============================================================================
*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "vs_common.h"

/** @addtogroup evrcnw2k_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** EVRC-NW2K vocoder format. */
#define VS_COMMON_MEDIA_ID_EVRCNW2K ( 0x000130D2 )

/*----------------------------------------------------------------------------
   Start of Enumerations
----------------------------------------------------------------------------*/


/**
 *
 * Estimated average encoding rates for active speech (channel encoding rates).
 *
 * Refer to Table 2.7.1.2-1 in @xhyperref{S15, [S15]}.
 */
typedef enum vs_vocevrcnw2k_avg_enc_rate_e
{
  VS_VOCEVRCNW2K_AVG_ENC_RATE_2_4_KBPS = 0,
    /**< Rate -- 2.4 kbps; COP -- 3. */
  VS_VOCEVRCNW2K_AVG_ENC_RATE_4_8_KBPS = 1,
    /**< Rate -- 4.8 kbps; COP -- 7. */
  VS_VOCEVRCNW2K_AVG_ENC_RATE_5_8_KBPS = 2,
    /**< Rate -- 5.8 kbps; COP -- 6. */
  VS_VOCEVRCNW2K_AVG_ENC_RATE_6_2_KBPS = 3,
    /**< Rate -- 6.2 kbps; COP -- 5. */
  VS_VOCEVRCNW2K_AVG_ENC_RATE_6_6_KBPS = 4,
    /**< Rate -- 6.6 kbps; COP -- 4. */
  VS_VOCEVRCNW2K_AVG_ENC_RATE_7_5_KBPS = 5,
    /**< Rate -- 7.5 kbps; COP -- 2. */
  VS_VOCEVRCNW2K_AVG_ENC_RATE_8_5_KBPS = 6,
    /**< Rate -- 8.5 kbps; COP -- 0. */
  VS_VOCEVRCNW2K_AVG_ENC_RATE_9_3_KBPS = 7,
    /**< Rate -- 9.3 kbps; COP -- 1. */
}/** @cond */
  vs_vocevrcnw2k_avg_enc_rate_t/** @endcond */;

/**
 * EVRC-NW2K frame type (rate) information. 
 * Refer to Table 3-9 in @xhyperref{Q5, [Q5]}.
 */
typedef enum vs_vocevrcnw2k_frame_rate_e
{
  VS_VOCEVRCNW2K_FRAME_BLANK_RATE = 0,
      /**< Blank. */
  VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE = 1,
      /**< Rate 1/8. */
  VS_VOCEVRCNW2K_FRAME_QUARTER_RATE = 2,
      /**< Rate 1/4. */
  VS_VOCEVRCNW2K_FRAME_HALF_RATE = 3,
      /**< Rate 1/2. */
  VS_VOCEVRCNW2K_FRAME_FULL_RATE = 4,
      /**< Rate 1. */
  VS_VOCEVRCNW2K_FRAME_ERASURE_RATE = 5,
      /**< Erasure rate; applicable only for downlink. */
  VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE_NONCRITICAL = 6,
      /**< Noncritical 1/8 rate. */
} /** @cond */
  vs_vocevrcnw2k_frame_rate_t/** @endcond */;

/*  End of Enumerations */

/*----------------------------------------------------------------------------
   Start of Structures
----------------------------------------------------------------------------*/


typedef struct vs_vocevrcnw2k_frame_info_t
  vs_vocevrcnw2k_frame_info_t;

/**
 * EVRC-NW2K frame information structure.
 */
struct vs_vocevrcnw2k_frame_info_t
{
  vs_vocevrcnw2k_frame_rate_t rate;
      /**< EVRC-NW2K frame rate. */
};

/** @} */

/** @addtogroup set_evrcnw2k_frame_minmax_rate
@{ */

/**
 * Sets the minimum and maximum encoder rate of the EVRC-NW2K vocoder.
 */
#define VS_VOCEVRCNW2K_CMD_SET_FRAME_MINMAX_RATE ( 0x000130BF )

typedef struct vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t 
  vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t;

/** Set minimum/maximum frame rate command parameters. */
struct vs_vocevrcnw2k_cmd_set_frame_minmax_rate_t 
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocevrcnw2k_frame_rate_t min_rate;
      /**< Minimum frame rate. */
  vs_vocevrcnw2k_frame_rate_t max_rate;
      /**< Maximum frame rate. */
};

/** @} */

/** @addtogroup set_evrcnw2k_avg_enc_rate
@{ */

/**
 * Sets the average encoder rate of the EVRC-NW2K vocoder.
 */
#define VS_VOCEVRCNW2K_CMD_SET_AVG_ENC_RATE ( 0x000130C0 )

typedef struct vs_vocevrcnw2k_cmd_set_avg_enc_rate_t 
  vs_vocevrcnw2k_cmd_set_avg_enc_rate_t;

/** Set average encoder rate command parameters. */
struct vs_vocevrcnw2k_cmd_set_avg_enc_rate_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocevrcnw2k_avg_enc_rate_t rate;
      /**< Average encoder rate. */
};

/** @} */

/** @addtogroup set_evrcnw2k_dtx_mode
@{ */

/**
 * Sets the DTX mode of the EVRC-NW2K vocoder.
 */
#define VS_VOCEVRCNW2K_CMD_SET_DTX_MODE ( 0x000130C1 )

typedef struct vs_vocevrcnw2k_cmd_set_dtx_mode_t 
  vs_vocevrcnw2k_cmd_set_dtx_mode_t;

/** Set DTX mode command parameters. */
struct vs_vocevrcnw2k_cmd_set_dtx_mode_t
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

#endif /* __VS_VOCEVRCNW2K_H__ */


