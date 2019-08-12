/*****************************************************************************
***
*** TITLE
***
***  GPRS LAYER 2 RLC TRANSLATION SERVICES
***
***
*** DESCRIPTION
***
***  Translation services between RLC/MAC block format and their internal 
***  representation
***
*** 
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/grlcx.h#1 $   
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 03/16/01    hv     RLC translation functions definition
*** 04/22/04    hv     Added EGPRS utils
*****************************************************************************/

#ifndef INC_GRLCX_H
#define INC_GRLCX_H

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#include "customer.h" /* This include must be first before #ifdef FEATURE_  */

#include "grlcmaccmn.h"
#include "grlci.h"
#include "msg.h"
#include "err.h"
#include "memory.h"
#include "grlcx.h"
#include "gbitutil.h"
#include "grlculfifo.h"



/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/




/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/




/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/



/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/*===========================================================================
===
===  FUNCTION      XLATE_EXT_MAC_HDR
===
===  DESCRIPTION
===
===    Translate MAC header from internal format to external format
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/

extern void xlate_ext_mac_hdr
(
  uint8     src[],        /* MAC header in external format as part of an 
                          ** array of bytes
                          */
  mac_hdr_t *mac_hdr      /* destination internal format */
#if defined (DEBUG_GSM_GPRS_RLC_TEST) || defined (PERLUTF)
  #error code not present
#endif
);

/*===========================================================================
===
===  FUNCTION      XLATE_EXT_DL_DATA
===
===  DESCRIPTION
===
===    Translate downlink RLC data block from External format into internal 
===    format
===
===  PARAMETERS
===
===    pl1_hdr_ptr
===       Points to PL1 Radio Block header
===  
===    dsm_ptr:
===       points to DSM item representing a PL1 radio block payload which
===       contains RLC DL data block (MAC header, RLC header and payload)
===
===    dl_blk_ptr:
===       Internal structure representing rlc dl data block
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
void xlate_ext_dl_data
(
  gas_id_t                       gas_id,
  const pl1_radio_block_header_t *pl1_hdr_ptr,
  dsm_item_type                  *dsm_ptr,
  grlc_dl_data_block_t           *dl_blk_ptr
);


/*===========================================================================
===
===  FUNCTION      TEST_MODE_XLATE_EXT_DL_DATA
===
===  DESCRIPTION
===
===    Translate downlink RLC data block from External format into internal 
===    format  without extracting the LI/M/E octets
===
===  PARAMETERS
===
===    pl1_blk_ptr:
===       points to DSM item representing a PL1 radio block which contains
===       PL1 header info (coding scheme, time-slot and frame number) and
===       RLC DL data block (MAC header, RLC header and payload)
===
===    dl_blk_ptr:
===       Internal structure representing rlc dl data block
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
void test_mode_xlate_ext_dl_data
(
  gas_id_t                       gas_id,
  const pl1_radio_block_header_t *pl1_hdr_ptr,
  dsm_item_type                  *dsm_ptr,
  grlc_dl_data_block_t           *dl_blk_ptr
);

/*===========================================================================
===
===  FUNCTION      E_XLATE_EXT_DL_DATA
===
===  DESCRIPTION
===
===    Translate downlink EGPRS RLC data block from External format into internal 
===    format
===
===  PARAMETERS
===
===    pl1_blk_ptr:
===       points to a PL1 radio block which contains
===       PL1 header info (coding scheme, time-slot and frame number) and
===       RLC DL data blocks (MAC header, RLC header and payload)
===
===    dl_blk1_ptr/dl_blk2_ptr:
===       Internal structure representing rlc dl data blocks
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
void e_xlate_ext_dl_data
(
  gas_id_t              gas_id,
  e_pl1_radio_block_t   *pl1_blk_ptr,
  grlc_dl_data_block_t  *dl_blk1_ptr,
  grlc_dl_data_block_t  *dl_blk2_ptr
);

/*===========================================================================
===
===  FUNCTION      E_TEST_MODE_XLATE_EXT_DL_DATA
===
===  DESCRIPTION
===
===    Translate downlink EGPRS RLC data block from External format into internal 
===    format for ETSI test mode B.
===
===  PARAMETERS
===
===    pl1_blk_ptr:
===       points to a PL1 radio block which contains
===       PL1 header info (coding scheme, time-slot and frame number) and
===       RLC DL data blocks (MAC header, RLC header and payload)
===
===    dl_blk1_ptr/dl_blk2_ptr:
===       Internal structure representing rlc dl data blocks
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
void e_test_mode_xlate_ext_dl_data
(
  gas_id_t              gas_id,
  e_pl1_radio_block_t   *pl1_blk_ptr,
  grlc_dl_data_block_t  *dl_blk1_ptr,
  grlc_dl_data_block_t  *dl_blk2_ptr
);

/*===========================================================================
===
===  FUNCTION      XLATE_INT_UL_DATA.
===
===  DESCRIPTION
===
===    Translate downlink RLC data block from internal format into external format
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    none
===  
===========================================================================*/
void xlate_int_ul_data
(
  grlc_ul_data_block_t *ul_data,   /* structure representing ul RLC data block */
  uint8               *dst        /* array of bytes representing external 
                                  ** RLC data block
                                  */
);

/*===========================================================================
===
===  FUNCTION      E_XLATE_INT_UL_DATA.
===
===  DESCRIPTION
===
===    Translate uplink EGPRS RLC data block from internal format into external format
===    This function also applies RLC filler octets when the remaining octets
===    is not zero.
===
===
===  PARAMS
===    *ul_data1:  structure representing 1st ul RLC data block.
===    *ul_data2:  structure representing 2nd ul RLC data block. 
===                NULL for single payload MCS.
===    *dst:       pointer to egprs PL1 FIFO element
===
===  DEPENDENCIES
===
===    none
===  
===  RETURN VALUE
===
===    none
===
===  SIDE EFFECTS
===
===    the dup_dsm_ptr is NULL after calling gprs_pdu_strip_head()
===  
===========================================================================*/
void e_xlate_int_ul_data
(
  grlc_ul_data_block_t      *ul_data1,
  grlc_ul_data_block_t      *ul_data2,
  e_grlc_ul_fifo_element_t  *dst1,
  e_grlc_ul_fifo_element_t  *dst2
);



#if defined(DEBUG_GSM_GPRS_DS_UNIT_TEST) || defined (DEBUG_GSM_EGPRS_DS_UNIT_TEST) || defined (PERLUTF)
#error code not present
#endif /* DEBUG_GSM_GPRS_DS_UNIT_TEST || DEBUG_GSM_EGPRS_DS_UNIT_TEST / PERLUTF */

#endif /* INC_GRLCX_H  */

/*** EOF: don't remove! ***/
