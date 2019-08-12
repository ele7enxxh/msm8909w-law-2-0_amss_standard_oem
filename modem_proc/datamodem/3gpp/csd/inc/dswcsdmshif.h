#ifndef DSWCSDMSHIF_H
#define DSWCSDMSHIF_H
/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                M O D E - S P E C I F I C   H A N D L E R S
                 
                I N T E R F A C E   H E A D E R   F I L E

DESCRIPTION
  This file contains the prototypes of the WCDMA Circuit-Switched Data 
  Services APIs used by the UMTS Circuit Switched Mode Specific Handlers. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 - 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dswcsdmshif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
03/28/03   dgy     Initial version.
26/02/07   DA      Fixed high lint errors

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS
#include "dsm.h"
#include "sys.h"
#include "dsucsdi.h"
#include "dswcsdi.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSWCSD_CONFIG_TO_SIO_WM

DESCRIPTION
  This function initializes the watermark for Downlink data from CS Data to
  SIO. It is called by the UCSD mode-specific handlers when a call is 
  connected.
   
DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern boolean dswcsd_config_to_sio_wm
( 
  dsm_watermark_type *to_sio_wm
);


/*===========================================================================

FUNCTION DSWCSD_INIT_CALL_INFO

DESCRIPTION
  This function initializes call-related information. It is called by the 
  UCSD mode-specific handlers when a call is connected.
   
DEPENDENCIES
  rrc_return_cs_data_call_parms() should be called before this function is
  called.  

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_init_call_info
( 
  uint16       bytes_per_sdu,                 /* Number of bytes in an SDU */
  uint32       sdus_per_tti,                  /* Number of SDUs per TTI    */
  uint8        num_data_bits,                 /* Number of data bits       */
  uint8        num_stop_bits,                 /* Number of stop bits       */
  uint8        parity,                        /* Parity indicator          */
  uint8        fnur,                          /* Fixed Network User Rate   */
  sys_sys_mode_e_type  mode,                  /* Network mode              */
  cm_call_id_type     cm_call_id,             /* Call Manager call ID      */
  ds_ucsd_set_data_counters_cb_T   set_data_counters_cb
                                              /* Traffic counter update cb */
);


/*===========================================================================

FUNCTION DSWCSD_REGISTER_RLC_WATERMARKS

DESCRIPTION
  This function registers the RLC watermarks with WCDMA CS Data. It is called
  upon powerup.
   
DEPENDENCIES
  RLC watermarks should be initialized before this function is called.

RETURN VALUE
  None
  
SIDE EFFECTS
  None

===========================================================================*/

extern void dswcsd_register_rlc_watermarks
( 
  dsm_watermark_type *to_rlc_wm_ptr,     /* Pointer to the Uplink RLC WM   */
  dsm_watermark_type *from_rlc_wm_ptr    /* Pointer to the Downlink RLC WM */
);


#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*===========================================================================

FUNCTION DSWCSD_GET_SYNC_TEST_APP_ENABLE_STATUS

DESCRIPTION
  This function reads from NV whether or not the Synchronous Test Application
  is enabled.
   
DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating whether or not the Sync test app is enabled.
  
SIDE EFFECTS
  None

===========================================================================*/

extern boolean dswcsd_get_sync_test_app_enable_status( sys_modem_as_id_e_type        subs_id );
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

/*===========================================================================

FUNCTION DSWCSD_GET_CALL_INFO_PTR

DESCRIPTION
  This function returns a pointer to dswcsd_call_info
   
DEPENDENCIES
  None

RETURN VALUE
  dswcsd_call_info_T *.
  
SIDE EFFECTS
  None

===========================================================================*/
dswcsd_call_info_T * dswcsd_get_call_info_ptr(void);

/*===========================================================================

FUNCTION DSWCSD_GET_TO_RLC_WM_PTR

DESCRIPTION
  This function returns dswcsd_to_rlc_wm_ptr
   
DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type *.
  
SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * dswcsd_get_to_rlc_wm_ptr(void);

/*===========================================================================

FUNCTION DSWCSD_GET_TO_RLC_WM_PTR

DESCRIPTION
  This function returns dswcsd_from_rlc_wm_ptr
   
DEPENDENCIES
  None

RETURN VALUE
  dsm_watermark_type *.
  
SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * dswcsd_get_from_rlc_wm_ptr(void);

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*===========================================================================

FUNCTION DSWCSD_IS_SYNC_TEST_APP_ENABLED

DESCRIPTION
  This function returns the value of dswcsd_sync_test_app_enabled flag
   
DEPENDENCIES
  None

RETURN VALUE
  boolean.
  
SIDE EFFECTS
  None

===========================================================================*/
boolean dswcsd_is_sync_test_app_enabled(void);

#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


#endif /* DSWCSDMSHIF_H */
