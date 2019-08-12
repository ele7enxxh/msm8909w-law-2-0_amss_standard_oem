#ifndef MCALWCDMA_COMMON_H
#define MCALWCDMA_COMMON_H

/*============================================================================*/
/** @file  
 * This module has definition and declaration that are common to MCAL WCDMA
 * modules
 */
/*============================================================================*/

/*============================================================================
Copyright (c) 2006 - 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/mcalwcdma_common.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     -------------------------------------------------------- 
05/06/14    geg     Use safer versions of memcpy() and memmove()
02/12/12    hk      Nikel feature cleanup
01/30/12    vs      Feature cleanup.
01/19/12    zr      Mainlined FEATURE_MCAL_WCDMA
05/31/11    geg     Changes for RRC and L2 api files clean-up wcdma dependencies
01/24/11    stk     Replaced my_memcpy with memcpy for NikeL
07/16/09    rgn     Enhanced existing s-cpich support
04/24/09    mr      Fixed Compiler Warnings
10/03/08    hk      Bringing in 7k Mailine fixes
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
01/23/08    mc      Introduced in mainline from MBMS dev branch.
03/03/07    mc      Sync up for MBMS phase2A integration
12/12/06    mc      Synced up for MBMS demo - 3042 baseline
11/29/06    mc      Initial version for MBMS demo integration.
===========================================================================*/

/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "comdef.h"
#include "customer.h"
#include "qw.h"
#include "mcalwcdma_const.h"
#include "wfw_demod_intf.h"

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/** Indicate success status */
#define MCAL_STATUS_SUCCESS 0

/** Indicate generic failure status */
#define MCAL_STATUS_FAILURE -1

/* Conversion from bits to W32 aligned count */
#define MCALWCDMA_BITS_TO_W32_COUNT(num_bits) \
  (num_bits) & 0x1F ? ((num_bits) >> 5) + 1 : (num_bits) >> 5

/* Macro to set a value in buffer, offset by 'reg', shifted by
   'shift', and masked by 'mask'  
   This macro masks the value and then shifts as the shist is variable */
#define MCALWCDMA_SET_BUF_FIELD_VAR_SHFT(buffer, reg, shift, mask, value) \
            buffer[reg] &= (~((uint32)(mask << shift))); \
            buffer[reg] |= ((value & mask) << shift)

/* Macro to set a value in buffer, offset by 'reg', shifted by
   'shift', and masked by 'mask'  
   This macro shifts the value and then masks */
#define MCALWCDMA_SET_BUF_VAL(buffer, offset, shift, mask, val) \
  buffer[offset] &= (~((uint32)(mask))); \
  buffer[offset] |= (((val) << (shift)) & (mask))

/* Macro to extract the value */
#define MCALWCDMA_GET_BUF_VAL(buffer, offset, shift, mask) \
  ((buffer[offset] & mask) >> shift)

/** Accessing W32 at byte pointer location. It is assumed that byte pointer
 * is W32 aligned */
#define MCALWCDMA_W32_AT_BYTE_PTR(byte_ptr) *((uint32*) (byte_ptr)) /*lint!e123*/

/* Conversion to various chip count */

/** @name Various Chip counts and their conversions
 * @{ */
#define MCALWCDMA_SEQ_CHIPX8_TO_CHIP(cx8) ((cx8) >> 3)
#define MCALWCDMA_CX8_TO_256CHIPS(cx8) ((cx8) >> 11)
#define MCALWCDMA_256CHIPS_TO_CX8(cx8) ((cx8) << 11)
#define MCALWCDMA_CX8_TO_TICKS(cx8) MCALWCDMA_CX8_TO_256CHIPS(cx8)
#define MCALWCDMA_CX8_TO_BPGS(cx8)  MCALWCDMA_CX8_TO_256CHIPS(cx8)
#define MCALWCDMA_BPGS_TO_CX8(bpg)  MCALWCDMA_256CHIPS_TO_CX8(bpg)
#define MCALWCDMA_CX8_TO_SLOT(cx8) \
  (MCALWCDMA_CX8_TO_TICKS(cx8)/MCALWCDMA_NUM_TICKS_PER_SLOT)
/** @} */

/** Maximum number of scrambling code */
#define MCALWCDMA_MAX_SEC_SCR_CODE                16
/** Invalid scrambling code */
#define MCALWCDMA_NO_SEC_SCRAMBLING_CODE        0xFF
/** Spreading factor */
#define MCALWCDMA_L1_SF_512_VAL                  512
/** Chips per frame */
#define MCALWCDMA_NUM_256_CHIP_UNITS_PER_FRAME   150

#define MCALWCDMA_MAX_TPC_INDEX 6
#define MCALWCDMA_MAX_BITS_PER_IND (MCALWCDMA_PICH_PI_BITS_PER_FRAME/MCALWCDMA_PICH_PI_PER_FRAME_18)

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/** MCAL return status type. Use it instead of boolean value */
typedef int32 mcal_status_type;

/* Call back function type declaration */
/* ----------------------------------- */

/*! @brief Physical channel diversity modes */
typedef enum
{
  MCALWCDMA_TX_DIV_NONE = WFW_DEMOD_PHCH_DIV_MODE_NONE,      /*!< No diversity mode */
  MCALWCDMA_TX_DIV_STTD = WFW_DEMOD_PHCH_DIV_MODE_STTD,      /*!< Div mode STTD */
  MCALWCDMA_TX_DIV_CLOSED_LOOP_1_ADJ_1 = WFW_DEMOD_PHCH_DIV_MODE_CLTD_TAM0, /*!< CLTD with timing adjustment mode 0 */
  MCALWCDMA_TX_DIV_CLOSED_LOOP_1_ADJ_2 = WFW_DEMOD_PHCH_DIV_MODE_CLTD_TAM1, /*!< CLTD with timing adjustment mode 1 */
  MCALWCDMA_TX_DIV_NUM = WFW_DEMOD_PHCH_NUM_DIV_MODE
} mcalwcdma_demod_tx_div_mode_enum_type;

/*! @brief Alternate scrambling code types */
typedef enum
{
  MCALWCDMA_DL_SCR_CODE_NO_CHANGE = WFW_DEMOD_DPCH_PHCH_ALT_SCR_CODE_NA,    /*!< Alternate scrambling not applicable */
  MCALWCDMA_DL_SCR_CODE_ALT_RIGHT = WFW_DEMOD_DPCH_PHCH_ALT_SCR_CODE_RIGHT, /*!< Alternate scrambling code right */
  MCALWCDMA_DL_SCR_CODE_ALT_LEFT = WFW_DEMOD_DPCH_PHCH_ALT_SCR_CODE_LEFT,  /*!< Alternate scrambling code left */
  MCALWCDMA_DL_SCR_CODE_ALT_NUM = WFW_DEMOD_DPCH_NUM_PHCH_ALT_SCR_CODE
} mcalwcdma_demod_scr_code_change_type_enum_type;


/* Structure type declaration */
/* -------------------------- */

/* Header definition since the header definition is defined in log.c */
typedef PACKED struct PACKED_POST
{
  uint16 len;  /* Specifies the length, in bytes of 
                 the entry, including this header. */

  uint16 code; /* Specifies the log code for the 
                  entry as enumerated above.       
                  Note: This is specified as word 
                  to guarantee size.               */

  qword ts;    /* The system timestamp for the log entry. The upper 48 bits
                  represent elapsed time since 6 Jan 1980 00:00:00 
                  in 1.25 ms units. The low order 16 bits represent elapsed 
                  time since the last 1.25 ms tick in 1/32 chip units 
                  (this 16 bit counter wraps at the value 49152).          */
} mcalwcdma_log_hdr_struct_type;

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*============================================================================*/
/**
 *
 */
/*============================================================================*/

#endif /* MCALWCDMA_COMMON_H */
