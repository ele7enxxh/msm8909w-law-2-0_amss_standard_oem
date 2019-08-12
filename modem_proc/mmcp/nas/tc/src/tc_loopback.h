#ifndef LOOPBACK_H
#define LOOPBACK_H
/*===========================================================================

              L O O P B A C K    D A T A    H E A D E R   F I L E

DESCRIPTION
  This file contains data declarations and function prototypes for Loopback.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/loopback.h_v   1.3   04 Mar 2002 19:57:36   madiraju  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/tc/src/tc_loopback.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
08/31/04   mks    Added support for handling multiple radio bearers during
                  loopback call setup
12/05/03   mks    Fixed DL WM overflow issues when network requests larger
                  SDU size while closing the test loop during RLC testing.
03/04/02   ks     Added macro for MAX_SDU_SIZE_FOR_384K
02/06/02   ks     Register for UL/DL water marks with RLC.
09/28/01   ks     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODDLE

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "dsm.h"
#include "tc.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum SDU size for various data rates, in bytes.
---------------------------------------------------------------------------*/

#define MAX_SDU_SIZE_FOR_14400   72                /* 72 bytes or 576 bits */
#define MAX_SDU_SIZE_FOR_28800   72                /* 72 bytes or 576 bits */
#define MAX_SDU_SIZE_FOR_32K     80                /* 80 bytes or 640 bits */
#define MAX_SDU_SIZE_FOR_57600   72                /* 72 bytes or 576 bits */
#define MAX_SDU_SIZE_FOR_64K     80                /* 80 bytes or 640 bits */
#define MAX_SDU_SIZE_FOR_384K    480               /* 480 bytes or 3840 bits */

#define NUM_BITS_PER_BYTE        8             /* Number of bits in a byte */

/*---------------------------------------------------------------------------
  Watermark for DL RLC data
---------------------------------------------------------------------------*/  

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

#define MAX_LB_SDU_SIZE     900
#define LB_DL_DATA_BUF_SIZ  MAX_LB_SDU_SIZE  /* Size of the DL data buffer in bytes */
#define LB_UL_DATA_BUF_SIZ  MAX_LB_SDU_SIZE  /* Size of the DL data buffer in bytes */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================
FUNCTION LB_INIT_DLRLC_WM

DESCRIPTION
  This function initializes the DL RLC/DS watermark.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void lb_init_dlrlc_wm( byte rab_id );

/*===========================================================================
FUNCTION lb_INIT_ULRLC_WM

DESCRIPTION
  This function initializes the UL RLC/DS watermark.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void lb_init_ulrlc_wm( byte rab_id );

/*===========================================================================
FUNCTION LB_REGISTER_RLC_WM

DESCRIPTION
  This function registers the UL/UL watermarks with the RLC for data 
  transfer.
  
DEPENDENCIES
  This function should be called only after a successful RAB establishment.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void lb_register_rlc_wm( byte rab_id );
extern void lb_register_dl_lc_rlc_wm(byte rab_id, uint8 dl_lc_id);

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION LB_REGISTER_TDSRLC_WM

DESCRIPTION
  This function registers the UL/UL watermarks with the TD-SCDMA RLC for data 
  transfer.
  
DEPENDENCIES
  This function should be called only after a successful RAB establishment.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void lb_register_tdsrlc_wm( byte rab_id );
void lb_register_tdsdl_lc_rlc_wm(byte rab_id, uint8 dl_lc_id);
#endif /* FEATURE_TDSCDMA */



/*===========================================================================

FUNCTION LB_CLEANUP

DESCRIPTION
  This function cleans up the DL/UL variables and data structures.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void lb_dl_cleanup( void );


/*===========================================================================
FUNCTION LB_RX_DATA_FROM_RLC

DESCRIPTION
  This function receives DL data from RLC. When it receives data, it sends 
  a command to the TC task along with the data.
  
DEPENDENCIES
  lb_from_rlc_wm must have been previously initialized.
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

extern void lb_rx_data_from_rlc(
  struct dsm_watermark_type_s *wm,
  /*lint -esym(715,wm) */
  void* callback_data
);

/*===========================================================================

FUNCTION LB_TX_DATA_TO_RLC

DESCRIPTION
  This function sends UL data to RLC. It converts UL data into a chained
  DSM item.
  
DEPENDENCIES
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
#ifdef FEATURE_TC_HSPA_OPTIMIZATION
void lb_tx_data_to_rlc(uint32 wm_key, dsm_item_type *dsm_item_ptr);
#else
void lb_tx_data_to_rlc( uint32 wm_id, uint8 * tx_item_ptr, uint16 tx_item_size
#ifdef FEATURE_TC_CONSTANT_DELAY_ENH
, uint32 app_ptr
#endif
);
#endif


#endif /* LOOPBACK_H */


