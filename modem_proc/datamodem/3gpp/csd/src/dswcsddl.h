#ifndef DSWCSDDL_H
#define DSWCSDDL_H
/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                D O W N L I N K   H E A D E R   F I L E

DESCRIPTION
  This file contains data declarations and function prototypes for WCDMA 
  Circuit-Switched Data Downlink. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsddl.h_v   1.6   17 May 2002 15:12:12   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsddl.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
04/15/03   dgy     Added support for UMTS CS Data handlers. Removed 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS and
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
05/17/02   dgy     Merged in the Simple Rate Adaptation under 
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
03/15/02   dgy     Added Synchronous Transparent CS Data test application 
                   under FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP.
02/21/02   dgy     Modified comments.
09/20/01   dgy     Removed function prototype for dswcsd_rx_data_from_rlc(). 
08/31/01   dgy     Changed function prototype for dswcsd_tx_data_to_sio().
08/29/01   dgy     Changed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
08/17/01   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODDLE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS
#include "comdef.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSWCSD_DL_CLEANUP

DESCRIPTION
  This function cleans up the DL variables and data structures.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_dl_cleanup( void );


/*===========================================================================

FUNCTION DSWCSD_TX_DATA_TO_SIO

DESCRIPTION
  This function retrieves all the data from the RLC/DS watermark, processes 
  it, and sends the DL data to SIO.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_tx_data_to_sio( void );

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*===========================================================================

FUNCTION DSWCSD_SET_DL_SDU_CNT

DESCRIPTION
  This function sets the value of dswcsd_dl_sdu_cnt.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_dl_sdu_cnt(uint8 value);

/*===========================================================================

FUNCTION DSWCSD_SET_DL_EXPECTED_FIRST_BYTE

DESCRIPTION
  This function sets the value of dswcsd_dl_expected_first_byte.
  
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_dl_expected_first_byte(uint8 value);

#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */


#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


#endif /* DSWCSDDL_H */
