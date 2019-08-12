#ifndef __VS_VOCFR_H__
#define __VS_VOCFR_H__

/**
  @file vs_vocfr.h
  @brief This file contains the definitions of the constants,
    	 data structures, and interfaces for GSM Full Rate (FR)
    	 vocoder
*/

/*
  ============================================================================

   Copyright (C) 2012, 2014 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocfr.h#1 $
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

/** @addtogroup fr_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** GSM Full Rate (FR) vocoder format. */
#define VS_COMMON_MEDIA_ID_FR ( 0x00012E1C )

/*----------------------------------------------------------------------------
  Start of Structures
----------------------------------------------------------------------------*/



typedef struct vs_vocfr_frame_info_t vs_vocfr_frame_info_t;

/**
 * GSM FR vocoder frame information structure.
 */
struct vs_vocfr_frame_info_t
{
  uint8_t bfi : 1;
      /**< BIT(0): Bad Frame Indication (BFI); used for error concealment.
        * The BFI applies to downlink only. Unused for uplink.
        */
  uint8_t sid : 2;
      /**< BIT(1-2): Silence Insertion Descriptor (SID);
        * Applies to both uplink and downlink: \n
        *    - 3 -- Reserved \n
        *    - 2 -- Valid SID frame \n
        *    - 1 -- Invalid SID frame \n
        *    - 0 -- Speech frame @tablebulletend
        */
  uint8_t taf : 1;
      /**< BIT(3): Time alignment flag.
        * The TAF applies to downlink only. Unused for uplink: \n
        * - 1 -- Position of the SID frame within the Slow Associated Control
        *         Channel (SACCH) multiframe \n
        * - 0 -- Frame is not aligned @tablebulletend
        */
  uint8_t reserved_bit_4 : 1; /**< Reserved. */
  uint8_t reserved_bit_5 : 1; /**< Reserved. */
  uint8_t reserved_bit_6 : 1; /**< Reserved. */
  uint8_t reserved_bit_7 : 1; /**< Reserved. @newpagetable */
};

/** @} */

/** @addtogroup set_fr_dtx_mode
@{ */

/**
 * Sets the DTX mode of the GSM FR vocoder.
 */ 
#define VS_VOCFR_CMD_SET_DTX_MODE ( 0x00012E3E )

typedef struct vs_vocfr_cmd_set_dtx_mode_t vs_vocfr_cmd_set_dtx_mode_t;

/** Set DTX Mode command parameters. */
struct vs_vocfr_cmd_set_dtx_mode_t
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

/** @addtogroup get_fr_dtx_mode
@{ */

/**
 * Gets the DTX mode of the GSM FR Vocoder.
 */
#define VS_VOCFR_CMD_GET_DTX_MODE ( 0x00012E3F )

typedef struct vs_vocfr_cmd_get_dtx_mode_t vs_vocfr_cmd_get_dtx_mode_t;

/** Get DTX Mode command parameters. */
struct vs_vocfr_cmd_get_dtx_mode_t
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

#endif /* __VS_VOCFR_H__ */

