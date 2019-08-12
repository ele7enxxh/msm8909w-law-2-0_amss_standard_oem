#ifndef __VS_VOCQCELP13K_H__
#define __VS_VOCQCELP13K_H__
/**
  @file vs_vocqcelp13k.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for QCELP-13k vocoder

*/

/*
  ============================================================================

   Copyright (C) 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocqcelp13k.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  07/18/2014  shr   Initial revision

  ============================================================================
*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "vs_common.h"

/** @addtogroup qcelp13k_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** QCELP-13K vocoder format. */
#define VS_COMMON_MEDIA_ID_QCELP13K ( 0x000130D3 )

/*----------------------------------------------------------------------------
   Start of Enumerations
----------------------------------------------------------------------------*/


/**
 *
 * Average encoding rates for active speech.
 *
 * Refer to Table 2.3.5.2-2 in @xhyperref{S15, [S15]}.
 */
typedef enum vs_vocqcelp13k_avg_enc_rate_e
{
  VS_VOCQCELP13K_AVG_ENC_RATE_7_2_KBPS = 0,
    /**< Rate -- 7.2 kbps; Reduced Rate mode level -- 4. */
  VS_VOCQCELP13K_AVG_ENC_RATE_9_0_KBPS = 1,
    /**< Rate -- 9.0 kbps; Reduced Rate mode level -- 3. */
  VS_VOCQCELP13K_AVG_ENC_RATE_11_2_KBPS = 2,
    /**< Rate -- 11.2 kbps; Reduced Rate mode level -- 2. */
  VS_VOCQCELP13K_AVG_ENC_RATE_12_2_KBPS = 3,
    /**< Rate -- 12.2 kbps; Reduced Rate mode level -- 1. */
  VS_VOCQCELP13K_AVG_ENC_RATE_14_4_KBPS = 4,
    /**< Rate -- 14.4 kbps; Reduced Rate mode level -- 0. */
}/** @cond */
  vs_vocqcelp13k_avg_enc_rate_t/** @endcond */;

/**
 * QCELP-13K frame type (rate) information. 
 * Refer to Table 3-1 in @xhyperref{Q5, [Q5]}.
 */
typedef enum vs_vocqcelp13k_frame_rate_e
{
  VS_VOCQCELP13K_FRAME_BLANK_RATE = 0,
    /**< Blank. */
  VS_VOCQCELP13K_FRAME_EIGHTH_RATE = 1,
    /**< Rate 1/8. */
  VS_VOCQCELP13K_FRAME_QUARTER_RATE = 2,
    /**< Rate 1/4. */
  VS_VOCQCELP13K_FRAME_HALF_RATE = 3,
    /**< Rate 1/2. */
  VS_VOCQCELP13K_FRAME_FULL_RATE = 4,
    /**< Rate 1. */
  VS_VOCQCELP13K_FRAME_ERASURE_RATE = 5,
    /**< Erasure rate; applicable only for downlink. */
} /** @cond */
  vs_vocqcelp13k_frame_rate_t/** @endcond */;

/*  End of Enumerations */

/*----------------------------------------------------------------------------
   Start of Structures
----------------------------------------------------------------------------*/


typedef struct vs_vocqcelp13k_frame_info_t
  vs_vocqcelp13k_frame_info_t;

/**
 * QCELP-13K frame information structure.
 */
struct vs_vocqcelp13k_frame_info_t
{
  vs_vocqcelp13k_frame_rate_t rate;
      /**< QCELP-13K frame rate. */
};

/** @} */

/** @addtogroup set_qcelp13k_frame_minmax_rate
@{ */

/**
 * Sets the minimum and maximum encoder frame rate of the QCELP-13K vocoder.
 */
#define VS_VOCQCELP13K_CMD_SET_FRAME_MINMAX_RATE ( 0x000130C2 )

typedef struct vs_vocqcelp13k_cmd_set_frame_minmax_rate_t 
  vs_vocqcelp13k_cmd_set_frame_minmax_rate_t;

/** Set minimum/maximum frame rate command parameters. */
struct vs_vocqcelp13k_cmd_set_frame_minmax_rate_t 
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocqcelp13k_frame_rate_t min_rate;
      /**< Minimum encoder rate. */
  vs_vocqcelp13k_frame_rate_t max_rate;
      /**< Maximum encoder rate. */
};

/** @} */

/** @addtogroup set_qcelp13k_avg_enc_rate
@{ */

/**
 * Sets the average encoder rate of the QCELP-13K vocoder.
 */
#define VS_VOCQCELP13K_CMD_SET_AVG_ENC_RATE ( 0x000130C3 )

typedef struct vs_vocqcelp13k_cmd_set_avg_enc_rate_t 
  vs_vocqcelp13k_cmd_set_avg_enc_rate_t;

struct vs_vocqcelp13k_cmd_set_avg_enc_rate_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocqcelp13k_avg_enc_rate_t rate;
      /**< Average encoder rate. */ 
};

/** @} */

/** @addtogroup set_qcelp13k_rate_modulation
@{ */

/**
 * Sets the encoder rate modulation of the QCELP-13K vocoder.
 */
#define VS_VOCQCELP13K_CMD_SET_ENC_RATE_MODULATION ( 0x000130C4 )

typedef struct vs_vocqcelp13k_cmd_set_enc_rate_mod_t 
  vs_vocqcelp13k_cmd_set_enc_rate_mod_t;

struct vs_vocqcelp13k_cmd_set_enc_rate_mod_t
{
  uint32_t handle;
      /**< Handle from the Open command. */
  uint32_t rate_modulation;
    /**< Rate modulation to the vocoder.
     *
     * The vocoder rate modulation equation is:
     @verbatim
               2X+1
     ratio = --------
              2(X+1)
     
     @endverbatim
     * where X = S when in select S mode and X=1/S when in select 1/S mode.
     * S is the rate limit factor, which is an integer that, when
     * multiplied by the 14.4 kbps (full rate), causes the ratio to be the
     * desired average bitrate.
     *
     * The mode is rateModulationCmd. The mode's bit structure is:
     *
     *        - b0 = Vocoder rate modulation enabled when 1 and disabled when 0 \n
     *        - b1 = Select S when 1 and select 1/S when 0 \n
     *        - b9..b2 = Rate limit factor is the value of S @tablebulletend
     */
};

/** @} */

/** @addtogroup set_qcelp13k_dtx_mode
@{ */

/**
 * Sets the DTX mode of the QCELP-13k vocoder.
 */
#define VS_VOCQCELP13K_CMD_SET_DTX_MODE ( 0x000130C5 )

typedef struct vs_vocqcelp13k_cmd_set_dtx_mode_t 
  vs_vocqcelp13k_cmd_set_dtx_mode_t;

/** Set DTX mode command parameters. */
struct vs_vocqcelp13k_cmd_set_dtx_mode_t
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

#endif /* __VS_VOCQCELP13K_H__ */

