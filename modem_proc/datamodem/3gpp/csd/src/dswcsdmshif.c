/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A
                             
                M O D E - S P E C I F I C   H A N D L E R S
                 
                           I N T E R F A C E

DESCRIPTION
  This file contains data and definitions of the WCDMA Circuit-Switched Data 
  Services APIs used by the UMTS Circuit Switched Mode-Specific Handlers. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2003 - 2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsdmshif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/11   SS      Q6 free floating support.
03/06/09   MS      CMI Integration fixes
03/04/09   sa      AU level CMI modifications.
26/02/07   da      Fixed high lint errors
09/01/06   ar      Added support for multiprocessor build.
06/04/04   ar      Synch test app shoudl be disabled by default.
05/28/04   ar      Add logging of synch CSD packets.
03/28/03   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS
#include "comdef.h"
#include "dsm.h"
#include "dswcsdmshif.h"
#include "dsucsdlog.h"
#include "dswcsdi.h"
#include "dswcsddltask_v.h"
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
#include "nv.h"      
#include "dstaski.h"
#include "ds3gcfgmgr.h"
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */
#include "data_msg.h"
#include "err.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Call-related information
---------------------------------------------------------------------------*/  
static dswcsd_call_info_T     dswcsd_call_info = {NULL};

/*---------------------------------------------------------------------------
  Pointers to the Uplink and Downlink RLC watermarks
---------------------------------------------------------------------------*/  
static dsm_watermark_type *dswcsd_to_rlc_wm_ptr = NULL;   /* Pointer to the UL RLC WM */
static dsm_watermark_type *dswcsd_from_rlc_wm_ptr = NULL; /* Pointer to the DL RLC WM */

#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
/*---------------------------------------------------------------------------
  Flag to know if the WCDMACS Synchronous Test App has been enabled. This
  boolean is set after reading NV_WCDMACS_SYNC_TEST_APP_ENABLED_I from NV.
---------------------------------------------------------------------------*/
static boolean dswcsd_sync_test_app_enabled = FALSE;
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */


/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                          FUNCTION DEFINITIONS

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

boolean dswcsd_config_to_sio_wm
( 
  dsm_watermark_type *to_sio_wm_ptr      /* Pointer to the Downlink SIO WM */
)
{

  if(to_sio_wm_ptr == NULL)
  {
    DATA_MSG0_ERROR("to_sio_wm_ptr is NULL");
    return FALSE;
  }
  /*-------------------------------------------------------------------------
    Initialize the Downlink SIO watermark.
  -------------------------------------------------------------------------*/  

  to_sio_wm_ptr->lo_watermark        = 0;
  to_sio_wm_ptr->hi_watermark        = SDU_SIZE_FOR_64K * 198;
  to_sio_wm_ptr->dont_exceed_cnt     = SDU_SIZE_FOR_64K * 200;
  to_sio_wm_ptr->gone_empty_func_ptr = NULL;
  to_sio_wm_ptr->lowater_func_ptr    = NULL;
  to_sio_wm_ptr->non_empty_func_ptr  = NULL;
  to_sio_wm_ptr->hiwater_func_ptr    = NULL;

  return TRUE;
} /* dswcsd_config_to_sio_wm() */


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

void dswcsd_init_call_info
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
)
{
  /*-------------------------------------------------------------------------
    Initialize call-related information.
  -------------------------------------------------------------------------*/  
  dswcsd_call_info.bytes_per_sdu = bytes_per_sdu;
  dswcsd_call_info.sdus_per_tti  = sdus_per_tti;
  dswcsd_call_info.cm_call_id = cm_call_id;
  dswcsd_call_info.set_data_counters_cb = set_data_counters_cb;
  
  MSG_MED("dswcsd_init_call_info(): bytes_per_sdu %d  sdus_per_tti %d  CM callID=%d",
	  dswcsd_call_info.bytes_per_sdu, dswcsd_call_info.sdus_per_tti, cm_call_id); 

  switch (fnur)
  {
  case FNUR_14400:
    dswcsd_call_info.fnur = DS_UCSD_RATE_14400;
    break;
    
  case FNUR_28800:
    dswcsd_call_info.fnur = DS_UCSD_RATE_28800;
    break;
    
  case FNUR_56K:
    dswcsd_call_info.fnur = DS_UCSD_RATE_57600;
    break;

  case FNUR_64K:
  default:
    /* Assume 64K is default */
    dswcsd_call_info.fnur = DS_UCSD_RATE_64000;
    break;
  }

  /*-------------------------------------------------------------------------
    Initialize CS packet logging
  -------------------------------------------------------------------------*/  
  ds_ucsd_log_init(num_data_bits,
                   num_stop_bits,
                   parity,
                   TRUE,
                   dswcsd_call_info.fnur,
                   mode,
                   0);

} /* dswcsd_init_call_info() */


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

void dswcsd_register_rlc_watermarks
( 
  dsm_watermark_type *to_rlc_wm_ptr,     /* Pointer to the Uplink RLC WM   */
  dsm_watermark_type *from_rlc_wm_ptr    /* Pointer to the Downlink RLC WM */
)
{
/*-------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Set the pointers to the RLC watermarks.
  -------------------------------------------------------------------------*/  
  dswcsd_to_rlc_wm_ptr   = to_rlc_wm_ptr;
  dswcsd_from_rlc_wm_ptr = from_rlc_wm_ptr;
} /* dswcsd_register_rlc_watermarks() */


#if defined(FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP)
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

boolean dswcsd_get_sync_test_app_enable_status
(
  sys_modem_as_id_e_type subs_id
)
{
  nv_stat_enum_type nv_status;
    /* status var for reading from NV */

  nv_item_type      nv_item;                  
    /* NV interface data */
  
/*-------------------------------------------------------------------------*/
  
  /*-------------------------------------------------------------------------
    See if this is to be a Sync test app call. If we remove the below
    work around for flow controlling the TE, this pulling of
    NV_WCDMACS_SYNC_TEST_APP_ENABLED_I from NV should move into the
    call_connected_hdlr. The NV item is only gotten here (versus 
    call_connected) because we don't flow control the TE now if it's a
    sync test app call, we only due for a V.80 call.

    Note, if a V.80 call is enabled (+ES=6,,8), and the Sync Test App
    is enabled, the call will be a Sync Test App Call.
  -------------------------------------------------------------------------*/

  nv_status =
     ds3gcfgmgr_read_legacy_nv_ex( NV_WCDMACS_SYNC_TEST_APP_ENABLED_I,
                      (nv_item_type *) &dswcsd_sync_test_app_enabled,subs_id );

  if(nv_status == NV_DONE_S)
  {
    DATA_MSG1_HIGH("SYNC_TEST_APP_ENABLED_I is %d",
                             dswcsd_sync_test_app_enabled);
  }
  else
  {
    /*-----------------------------------------------------------------------
      If problem reading from NV, default to FALSE. It will be a sync test
      app call only if explicitly set.
    -----------------------------------------------------------------------*/
    DATA_MSG0_ERROR("SYNC_TEST_APP_ENABLED_I not read");

    dswcsd_sync_test_app_enabled = FALSE;

    /* NV item is not initialized. Set it for future calls.
    */
    if( nv_status == NV_NOTACTIVE_S )
    {
      nv_item.wcdmacs_sync_test_app_enabled = dswcsd_sync_test_app_enabled;
      
     (void)ds3gcfgmgr_write_legacy_nv_ex( NV_WCDMACS_SYNC_TEST_APP_ENABLED_I,
                                         &nv_item, 
                                         subs_id );
    }
  }

  return ( dswcsd_sync_test_app_enabled ); 
} /* dswcsd_get_sync_test_app_enable_status() */

#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP &&
          !(FEATURE_DATA_ON_APPS && IMAGE_MODEM_PROC) */

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
dswcsd_call_info_T * dswcsd_get_call_info_ptr(void)
{
  return &dswcsd_call_info;
}

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
dsm_watermark_type * dswcsd_get_to_rlc_wm_ptr(void)
{
  return dswcsd_to_rlc_wm_ptr;
}

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
dsm_watermark_type * dswcsd_get_from_rlc_wm_ptr(void)
{
  return dswcsd_from_rlc_wm_ptr;
}

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
boolean dswcsd_is_sync_test_app_enabled(void)
{
  return dswcsd_sync_test_app_enabled;
}
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */



