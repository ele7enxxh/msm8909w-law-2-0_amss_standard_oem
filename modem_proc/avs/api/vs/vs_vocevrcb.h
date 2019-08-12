#ifndef __VS_VOCEVRCB_H__
#define __VS_VOCEVRCB_H__

/**
  @file vs_vocevrcb.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for EVRC-B vocoder

*/

/*
  ============================================================================

   Copyright (C) 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================ 

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocevrcb.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  7/16/14  shr   Initial revision

  ============================================================================
*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "vs_common.h"

/** @addtogroup evrcb_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** EVRC-B vocoder format. */
#define VS_COMMON_MEDIA_ID_EVRCB ( 0x000130CF )

/*---------------------------------------------------------------------------
   Start of Enumerations
---------------------------------------------------------------------------*/


/**
 * 
 * Estimated average encoding rates for active speech (channel encoding rates).
 *
 * Refer to Table 2.4.1.2-1 in @xhyperref{S15, [S15]}.
 */
typedef enum vs_vocevrcb_avg_enc_rate_e
{
  VS_VOCEVRCB_AVG_ENC_RATE_4_8_KBPS = 0,
    /**< Rate -- 4.8 kbps; COP -- 7. */
  VS_VOCEVRCB_AVG_ENC_RATE_5_8_KBPS = 1,
    /**< Rate -- 5.8 kbps; COP -- 6. */
  VS_VOCEVRCB_AVG_ENC_RATE_6_2_KBPS = 2,
    /**< Rate -- 6.2 kbps; COP -- 5. */
  VS_VOCEVRCB_AVG_ENC_RATE_6_6_KBPS = 3,
    /**< Rate -- 6.6 kbps; COP -- 4. */
  VS_VOCEVRCB_AVG_ENC_RATE_7_0_KBPS = 4,
    /**< Rate -- 7.0 kbps; COP -- 3. */
  VS_VOCEVRCB_AVG_ENC_RATE_7_5_KBPS = 5,
    /**< Rate -- 7.5 kbps; COP -- 2. */
  VS_VOCEVRCB_AVG_ENC_RATE_8_5_KBPS = 6,
    /**< Rate -- 8.5 kbps; COP -- 1. */
  VS_VOCEVRCB_AVG_ENC_RATE_9_3_KBPS = 7,
    /**< Rate -- 9.3 kbps; COP -- 0. */
}/** @cond */
  vs_vocevrcb_avg_enc_rate_t/** @endcond */;

/**
 * EVRC-B frame type (rate) information. 
 * Refer to Table 3-5 in @xhyperref{Q5, [Q5]}.
 */
typedef enum vs_vocevrcb_frame_rate_e
{
  VS_VOCEVRCB_FRAME_BLANK_RATE = 0,
    /**< Blank. */
  VS_VOCEVRCB_FRAME_EIGHTH_RATE = 1,
    /**< Rate 1/8. */
  VS_VOCEVRCB_FRAME_QUARTER_RATE = 2,
    /**< Rate 1/4. */
  VS_VOCEVRCB_FRAME_HALF_RATE = 3,
    /**< Rate 1/2. */
  VS_VOCEVRCB_FRAME_FULL_RATE = 4,
    /**< Rate 1. */
  VS_VOCEVRCB_FRAME_ERASURE_RATE = 5,
    /**< Erasure rate; applicable only for downlink. */
} /** @cond */
  vs_vocevrcb_frame_rate_t/** @endcond */;

/*  End of Enumerations */

/*---------------------------------------------------------------------------
   Start of Structures
---------------------------------------------------------------------------*/


typedef struct vs_vocevrcb_frame_info_t
  vs_vocevrcb_frame_info_t;

/**
 * EVRC-B frame information structure.
 */
struct vs_vocevrcb_frame_info_t
{
  vs_vocevrcb_frame_rate_t rate;
      /**< EVRC-B frame rate. */
};

/** @} */

/** @addtogroup set_evrcb_frame_minmax_rate
@{ */

/**
 * Sets the minimum and maximum encoder frame rate of the EVRC-B vocoder.
 */
#define VS_VOCEVRCB_CMD_SET_FRAME_MINMAX_RATE ( 0x000130B6 )

typedef struct vs_vocevrcb_cmd_set_frame_minmax_rate_t 
  vs_vocevrcb_cmd_set_frame_minmax_rate_t;

/** Set the minimum/maximum frame rate command parameters. */
struct vs_vocevrcb_cmd_set_frame_minmax_rate_t 
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocevrcb_frame_rate_t min_rate;
      /**< Minimum frame rate. */
  vs_vocevrcb_frame_rate_t max_rate;
      /**< Maximum frame rate. */
};

/** @} */

/** @addtogroup set_evrcb_avg_enc_rate
@{ */

/**
 * Sets the average encoder rate of the EVRC-B vocoder.
 */
#define VS_VOCEVRCB_CMD_SET_AVG_ENC_RATE ( 0x000130B7 )

typedef struct vs_vocevrcb_cmd_set_avg_enc_rate_t 
  vs_vocevrcb_cmd_set_avg_enc_rate_t;

/** Set average encoder rate command parameters. */
struct vs_vocevrcb_cmd_set_avg_enc_rate_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocevrcb_avg_enc_rate_t rate;
      /**< Average encoder rate. */ 
};

/** @} */

/** @addtogroup set_evrcb_dtx_mode
@{ */

/**
 * Sets the DTX mode of the EVRC-B vocoder.
 */
#define VS_VOCEVRCB_CMD_SET_DTX_MODE ( 0x000130B8 )

typedef struct vs_vocevrcb_cmd_set_dtx_mode_t 
  vs_vocevrcb_cmd_set_dtx_mode_t;

/** Set DTX mode command parameters. */
struct vs_vocevrcb_cmd_set_dtx_mode_t
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

#endif /* __VS_VOCEVRCB_H__ */

