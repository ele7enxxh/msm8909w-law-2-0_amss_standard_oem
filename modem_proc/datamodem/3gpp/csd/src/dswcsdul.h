#ifndef DSWCSDUL_H
#define DSWCSDUL_H
/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                     U P L I N K   H E A D E R   F I L E

DESCRIPTION
  This file contains data declarations and function prototypes for WCDMA 
  Circuit-Switched Data Uplink. 

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

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsdul.h_v   1.8   16 Jul 2002 14:41:16   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsdul.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
04/15/03   dgy     Added support for UMTS CS Data handlers. Removed 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS and
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
03/07/03   dwp     Make sync test app runtime configurable.
02/17/03   dwp     Extern dswcsd_from_sio_q and dswcsd_from_sio_q_cnt for
                   V.80 support. Update copyright.
07/15/02   dgy     Added dswcsd_at_esc_seq_ul_cleanup().
06/14/02   dgy     Mainlined the Simple Rate Adaption under 
                   FEATURE_DATA_WCDMA_CS_SIMPLE_RA.
04/29/02   dgy     Replaced static variables with global variables.
03/15/02   dgy     Added Synchronous Transparent CS Data test application 
                   under FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP.
03/12/02   dgy     Featurized Asynchronous Transparent CS Data under 
                   FEATURE_DATA_WCDMA_ASYNC_T_CS.
02/21/02   dgy     Added Synchronous CS Data and a run-time switch to switch 
                   between Asynchronous and Synchronous CS Data.
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
#include "dsm.h"
#include "sys.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
#define UL_DATA_BUF_SIZ  80         /* Size of the UL data buffer in bytes */


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSWCSD_INIT_FROM_SIO_Q

DESCRIPTION
  This function initializes the queue for the incoming UL SIO data.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_init_from_sio_q( void );


/*===========================================================================

FUNCTION DSWCSD_AT_ESC_SEQ_UL_CLEANUP

DESCRIPTION
  This function is invoked to clean up the relevant UL variables and data 
  structures and enable inbound SIO data flow if necessary when the AT escape 
  sequence is detected.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_at_esc_seq_ul_cleanup( void );


/*===========================================================================

FUNCTION DSWCSD_UL_CLEANUP

DESCRIPTION
  This function cleans up the UL variables and data structures and enables
  inbound SIO data flow if necessary.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_ul_cleanup( void );


/*===========================================================================

FUNCTION DSWCSD_TX_DATA_TO_RLC

DESCRIPTION
  This function builds and sends UL SDUs to RLC.
    
DEPENDENCIES
  dswcsd_to_rlc_wm_ptr must have been previously initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_tx_data_to_rlc( void );


/*===========================================================================

FUNCTION DSWCSD_RX_DATA_FROM_SIO

DESCRIPTION
  This function receives UL data from SIO. It queues the data on a local queue,
  dswcsd_from_sio_q.
  
DEPENDENCIES
  dswcsd_from_sio_q must have been previously initialized.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_rx_data_from_sio
( 
  dsm_item_type **dsm_item_ptr       /* Address of a pointer to a DSM item */
); /* dswcsd_rx_data_from_sio() */

/*===========================================================================

FUNCTION DSWCSD_GET_SIO_DSM_ITEM_PTR

DESCRIPTION
  This function returns the dswcsd_sio_dsm_item_ptr.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  dsm_item_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_item_type * dswcsd_get_sio_dsm_item_ptr(void);

/*===========================================================================

FUNCTION DSWCSD_SET_SIO_DSM_ITEM_PTR

DESCRIPTION
  This function sets the dswcsd_sio_dsm_item_ptr.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_sio_dsm_item_ptr(dsm_item_type * item_ptr);

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*===========================================================================

FUNCTION DSWCSD_SET_KNOWN_PATTERN

DESCRIPTION
  This function sets the value corresponding to the index in 
  dswcsd_known_pattern array.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  int

SIDE EFFECTS
  None

===========================================================================*/
int dswcsd_set_known_pattern
(
  uint8 index, 
  uint8 value
);

/*===========================================================================

FUNCTION DSWCSD_SET_UL_SDU_CNT

DESCRIPTION
  This function sets the value of dswcsd_ul_sdu_cnt
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_ul_sdu_cnt(uint8 value);

/*===========================================================================

FUNCTION DSWCSD_SET_UL_FIRST_BYTE

DESCRIPTION
  This function sets the value of dswcsd_ul_first_byte
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_ul_first_byte(uint8 value);

#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

/*===========================================================================

FUNCTION DSWCSD_GET_DSM_PAYLOAD_SIZE

DESCRIPTION
  This function returns the value of dswcsd_dsm_payload_size
  
DEPENDENCIES
  None.
  
RETURN VALUE
  uint16.

SIDE EFFECTS
  None

===========================================================================*/
uint16 dswcsd_get_dsm_payload_size(void);

/*===========================================================================

FUNCTION DSWCSD_SET_DSM_PAYLOAD_SIZE

DESCRIPTION
  This function sets the value of dswcsd_dsm_payload_size
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_set_dsm_payload_size(uint16 value);

/*===========================================================================

FUNCTION DSWCSD_GET_FROM_SIO_Q_PTR

DESCRIPTION
  This function returns the pointer to dswcsd_from_sio_q
  
DEPENDENCIES
  None.
  
RETURN VALUE
  q_type *.

SIDE EFFECTS
  None

===========================================================================*/
q_type * dswcsd_get_from_sio_q_ptr(void);

/*===========================================================================

FUNCTION DSWCSD_GET_FROM_SIO_Q_CNT

DESCRIPTION
  This function returns the queue count for dswcsd_from_sio_q
  
DEPENDENCIES
  None.
  
RETURN VALUE
  uint16.

SIDE EFFECTS
  None

===========================================================================*/
uint16 dswcsd_get_from_sio_q_cnt(void);

/*===========================================================================

FUNCTION DSWCSD_UPDATE_FROM_SIO_Q_CNT

DESCRIPTION
  This function updates the queue count for dswcsd_from_sio_q
  
DEPENDENCIES
  None.
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void dswcsd_update_from_sio_q_cnt(uint16 value);

/*===========================================================================

FUNCTION DS_WCSD_GET_CSVTI_NV_QMSS_ENABLED

DESCRIPTION
  This function reads from NV whether or not CSVT NV for QRD device is set or not.
  It must be called in the DSGCSD task.
   
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

void ds_wcsd_get_csvti_nv_qmss_enabled( sys_modem_as_id_e_type subs_id );

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


#endif /* DSWCSDUL_H */
