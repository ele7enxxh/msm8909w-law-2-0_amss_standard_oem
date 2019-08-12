#ifndef __VS_VOCEFR_H__
#define __VS_VOCEFR_H__

/**
  @file vs_vocefr.h
  @brief This file contains the definitions of the constants,
    	 data structures, and interfaces for GSM Enhanced Full
    	 Rate (EFR) vocoder
*/

/*
  ============================================================================

   Copyright (C) 2012, 2014 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_vocefr.h#1 $
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

/** @addtogroup efr_datatypes
@{ */

/*----------------------------------------------------------------------------
   Type Definitions
----------------------------------------------------------------------------*/

/*
 * Defined media types.
 */

/** GSM Enhanced Full Rate (EFR) vocoder format. */
#define VS_COMMON_MEDIA_ID_EFR ( 0x00012E1E )

/*----------------------------------------------------------------------------
  Start of Structures
----------------------------------------------------------------------------*/




typedef struct vs_vocefr_frame_info_t vs_vocefr_frame_info_t;

/**
 * GSM Enhanced Full Rate (EFR) vocoder frame information structure.
 */
struct vs_vocefr_frame_info_t
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
      /**< BIT(3): Time Alignment Flag (TAF). The TAF applies to downlink only.
        *  Unused for uplink: \n
        *  - 1 -- Position of the SID frame within the Slow Associated Control
        *         Channel (SACCH) multiframe \n
        *  - 0 -- Frame is not aligned @tablebulletend
        */
  uint8_t reserved_bit_4 : 1; /**< Reserved. */
  uint8_t reserved_bit_5 : 1; /**< Reserved. */
  uint8_t reserved_bit_6 : 1; /**< Reserved. */
  uint8_t reserved_bit_7 : 1; /**< Reserved. */
};

/** @} */

/** @addtogroup set_efr_dtx_mode
@{ */

/**
 * Sets the DTX mode of the GSM EFR vocoder.
 */
#define VS_VOCEFR_CMD_SET_DTX_MODE ( 0x00012E40 )

typedef struct vs_vocefr_cmd_set_dtx_mode_t vs_vocefr_cmd_set_dtx_mode_t;

/** Set DTX Mode command parameters. */
struct vs_vocefr_cmd_set_dtx_mode_t
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

/** @addtogroup get_efr_dtx_mode
@{ */

/**
 * Gets the DTX mode of the GSM EFR vocoder.
 */
#define VS_VOCEFR_CMD_GET_DTX_MODE ( 0x00012E41 )

typedef struct vs_vocefr_cmd_get_dtx_mode_t vs_vocefr_cmd_get_dtx_mode_t;

/** Get DTX Mode command parameters. */
struct vs_vocefr_cmd_get_dtx_mode_t
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

#endif /* __VS_VOCEFR_H__ */


