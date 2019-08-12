#ifndef __VS_VOCEVRCA_H__
#define __VS_VOCEVRCA_H__

/**
  @file vs_vocevrca.h
  @brief This file contains the definitions of the constants,
         data structures, and interfaces for EVRC-A vocoder

*/

/*
  ============================================================================

   Copyright (C) 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocevrca.h#1 $
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

/** @addtogroup evrca_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** EVRC-A vocoder format. */
#define VS_COMMON_MEDIA_ID_EVRCA ( 0x000130CE )

/*----------------------------------------------------------------------------
   Start of Enumerations
----------------------------------------------------------------------------*/


/**
 * EVRC-A frame type (rate) information. 
 * Refer to Table 3-3 in @xhyperref{Q5,[Q5]}.
 */
typedef enum vs_vocevrca_frame_rate_e
{
  VS_VOCEVRCA_FRAME_BLANK_RATE = 0,
    /**< Blank. */
  VS_VOCEVRCA_FRAME_EIGHTH_RATE = 1,
    /**< Rate 1/8. */
  VS_VOCEVRCA_FRAME_HALF_RATE = 2,
    /**< Rate 1/2. */
  VS_VOCEVRCA_FRAME_FULL_RATE = 3,
    /**< Rate 1. */
  VS_VOCEVRCA_FRAME_ERASURE_RATE = 4,
    /**< Erasure rate, applicable only for downlink. */
} /** @cond */
  vs_vocevrca_frame_rate_t/** @endcond */;

/*  End of Enumerations */

/*----------------------------------------------------------------------------
   Start of Structures
----------------------------------------------------------------------------*/


typedef struct vs_vocevrca_frame_info_t
  vs_vocevrca_frame_info_t;

/**
 * EVRC-A frame information structure.
 */
struct vs_vocevrca_frame_info_t
{
  vs_vocevrca_frame_rate_t rate;
      /**< EVRC-A frame rate. */
};

/** @} */

/** @addtogroup set_evrca_frame_minmax_rate
@{ */

/**
 * Sets the minimum and maximum encoder frame rate of the EVRC-A vocoder.
 */
#define VS_VOCEVRCA_CMD_SET_FRAME_MINMAX_RATE ( 0x000130C6 )

typedef struct vs_vocevrca_cmd_set_frame_minmax_rate_t 
  vs_vocevrca_cmd_set_frame_minmax_rate_t;

/** Set the minimum and maximum frame rate.
*/
struct vs_vocevrca_cmd_set_frame_minmax_rate_t 
{
  uint32_t handle;
      /**< Handle from the Open command. */
  vs_vocevrca_frame_rate_t min_rate;
      /**< Minimum frame rate. */
  vs_vocevrca_frame_rate_t max_rate;
      /**< Maximum frame rate. */
};

/** @} */

/** @addtogroup set_evrca_rate_modulation
@{ */

/**
 * Sets the encoder rate modulation of the EVRC-A vocoder.
 */
#define VS_VOCEVRCA_CMD_SET_ENC_RATE_MODULATION ( 0x000130C7 )

typedef struct vs_vocevrca_cmd_set_enc_rate_mod_t 
  vs_vocevrca_cmd_set_enc_rate_mod_t;

/** Set the enocder rate modulation.
*/
struct vs_vocevrca_cmd_set_enc_rate_mod_t
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

     * where X = S when in select S mode and X=1/S when in select 1/S mode. S is
     * the rate limit factor. S is an integer that, when multiplied by
     * 14.4 kbps (full rate) causes the ratio to be the desired average
     * bitrate.
     *
     * The mode is the rateModulationCmd. The mode's bit structure is:
     *
     *        - b0 = Vocoder rate modulation; enabled when 1 and disabled when 0 \n
     *        - b1 = Select S when 1 and select 1/S when 0 \n
     *        - b9..b2 = Rate limit factor is the value of S @tablebulletend
     */
};

/** @} */

/** @addtogroup set_evrca_dtx_mode
@{ */

/**
 * Sets the Discontinuous Transmission (DTX) mode of the EVRC-A vocoder.
 */
#define VS_VOCEVRCA_CMD_SET_DTX_MODE ( 0x000130C8 )

typedef struct vs_vocevrca_cmd_set_dtx_mode_t
  vs_vocevrca_cmd_set_dtx_mode_t;

/** Set the DTX mode command parameters.
*/
struct vs_vocevrca_cmd_set_dtx_mode_t
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

#endif /* __VS_VOCEVRCA_H__ */


