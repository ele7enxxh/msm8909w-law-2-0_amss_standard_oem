/*===========================================================================

             U M T S   C I R C U I T - S W I T C H E D   D A T A

                  M U L T I P R O C E S S O R   S H I M


DESCRIPTION
  This file contains the implementation of the interfaces for UMTS
  Circuit-Switched Data multiprocessor (MP) shim.  Routines which are
  specific to the MP environment are found below.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2014 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdmpshim.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/12   ND      RCINIT and CLK API changes.
04/29/11   SA      Added support for TD-SCDMA.
04/05/11   SS      Q6 free floating support.
01/06/11   SA      Fix to set the correct SIO packet size for GSM async call.
03/06/09   MS      CMI Integration fixes
03/04/09   SA      AU level CMI modifications.
12/23/08   SA      Replaced q_init with dsm_queue_init for watermark initialization.
06/07/07   AR      Remove redundant ds_ucsd_mpshim_deregister_wcdma_rlc() call
03/14/06   AR      Initial version.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "ds_3gppi_utils.h"


#ifdef FEATURE_DATA_WCDMA_CS
#include "rex.h"
#include "task.h"
#include "wcdmamvsif.h"
#include "rrcdata.h"
#include "dswcsd.h"
#include "dswcsdul.h"
#include "dswcsdmshif.h"
#include "dswcsdntrlcif.h"
#include "rcinit_rex.h"
#endif /* FEATURE_DATA_WCDMA_CS */
#ifdef FEATURE_DATA_GCSD_FAX
#include "dsfps.h"

#if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
    defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)
#endif
#endif /* FEATURE_DATA_GCSD_FAX */

#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
#include "comdef.h"
#include "amssassert.h"

#include <stringl/stringl.h>
#include "nv.h"
#include "ds_cap.h"
#include "cm.h"
#include "dsm.h"
#include "dstask.h"
#include "dstaski.h"
#include "dsat_v.h"       /* ATCOP values */
#include "ds3gsiolib.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "dsgcsdi.h"
#include "dsgcsd_task_v.h"
#include "dsgcsdl1if.h"
#include "dsucsdi.h"
#include "dsucsdnt.h"
#include "dsucsdappif.h"
#include "dsucsdbc.h"
#include "dsucsdmpshim.h"

#ifdef FEATURE_TDSCDMA
#ifndef FEATURE_SEGMENT_LOADING
#include "tdsl2uldatamodem.h"
#include "tdsl2dldatamodem.h"
//#include "tdsrlcdsapif.h"
#include "tdsl2external.h" 
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_TDSCDMA */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#ifdef FEATURE_SEGMENT_LOADING
/*---------------------------------------------------------------------------
TDSCDMA and WCDMA interface tables
---------------------------------------------------------------------------*/
extern interface_t *ds_3gpp_csd_tds_tbl;
extern interface_t *ds_3gpp_csd_wcdma_tbl;
#endif /* FEATURE_SEGMENT_LOADING */

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/



/*---------------------------------------------------------------------------
  UCSD Call Data Path elements
---------------------------------------------------------------------------*/
   
#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
/* Uplink and Downlink RLC deregistration parameters */
LOCAL rlc_ul_deregister_srvc_type  ds_ucsd_rlc_ul_dereg_params = {0};
LOCAL rlc_dl_deregister_srvc_type  ds_ucsd_rlc_dl_dereg_params = {0};

/* Queues for the Uplink and Downlink RLC watermarks */  
LOCAL q_type ds_ucsd_to_rlc_q = {{NULL}};
LOCAL q_type ds_ucsd_from_rlc_q = {{NULL}};

/* Uplink and Downlink RLC watermarks */                
LOCAL dsm_watermark_type ds_ucsd_to_rlc_wm = {NULL};             /* Uplink RLC WM   */
LOCAL dsm_watermark_type ds_ucsd_from_rlc_wm = {NULL};           /* Downlink RLC WM */
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_TDSCDMA */

#ifdef FEATURE_TDSCDMA
tdsl2_ul_datamodem_api_type  ds_ucsd_to_tdrlc_ul_cmd = {NULL};
tdsl2_dl_datamodem_api_type  ds_ucsd_from_tdrlc_dl_cmd = {NULL};
#endif /* FEATURE_TDSCDMA */

/* Queues for Uplink and Downlink Fax watermarks */
LOCAL q_type ds_ucsd_fps_from_sio_q = {{NULL}};
LOCAL q_type ds_ucsd_fps_to_sio_q = {{NULL}};

/* Queues for Uplink and Downlink SIO/Fax watermarks */
LOCAL q_type ds_ucsd_tps_from_sio_or_fps_q = {{NULL}};
LOCAL q_type ds_ucsd_tps_to_sio_or_fps_q = {{NULL}};

/* These watermarks will only be used during a fax call 
** by the fax layer to trasmit to/from SIO
*/                         
LOCAL dsm_watermark_type         ds_ucsd_fps_from_sio_wm = {NULL};             
LOCAL dsm_watermark_type         ds_ucsd_fps_to_sio_wm = {NULL};
   
/* These watermarks will be used by the GCSD transport
** protocol stack for data transfer with the FAX layer
** in the case of fax calls, or with SIO in the case
** of pure data calls.
*/
LOCAL dsm_watermark_type         ds_ucsd_tps_from_sio_or_fps_wm = {NULL};
LOCAL dsm_watermark_type         ds_ucsd_tps_to_sio_or_fps_wm = {NULL};

LOCAL void 
ds_ucsd_mpshim_init_sio_watermarks( void );

#ifdef FEATURE_DATA_WCDMA_CS
/*---------------------------------------------------------------------------
  TCB pointer for GCSD task
---------------------------------------------------------------------------*/
static rex_tcb_type* gcsd_tcb_ptr = NULL; 

LOCAL void 
ds_ucsd_mpshim_init_rlc_watermarks( void );

#endif /* FEATURE_DATA_WCDMA_CS */



/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================


FUNCTION ds_ucsd_mpshim_init_sio_watermarks

DESCRIPTION
  This function initializes the Uplink and Downlink GCSD watermarks
  used with SIO.
  There are two distinct watermark sets:
  * SIO <-> GCSD_FPS                 : for GSM fax calls only
  * SIO or GCSD_FPS <-> GCSD_TPS     : for all GCSD calls 
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_mpshim_init_sio_watermarks( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------
    Initialize the Uplink and Downlink GCSD - SIO watermarks
  ------------------------------------------------------------------*/

  memset(&ds_ucsd_fps_from_sio_wm, 0, sizeof(dsm_watermark_type));
  memset(&ds_ucsd_fps_from_sio_q, 0, sizeof(q_type));
  dsm_queue_init(&ds_ucsd_fps_from_sio_wm, 0, &ds_ucsd_fps_from_sio_q);
  memset(&ds_ucsd_fps_to_sio_wm, 0, sizeof(dsm_watermark_type));
  memset(&ds_ucsd_fps_to_sio_q, 0, sizeof(q_type));
  dsm_queue_init(&ds_ucsd_fps_to_sio_wm, 0, &ds_ucsd_fps_to_sio_q);
  memset(&ds_ucsd_tps_from_sio_or_fps_wm, 0, sizeof(dsm_watermark_type));
  memset(&ds_ucsd_tps_from_sio_or_fps_q, 0, sizeof(q_type));
  dsm_queue_init(&ds_ucsd_tps_from_sio_or_fps_wm, 0, &ds_ucsd_tps_from_sio_or_fps_q);
  memset(&ds_ucsd_tps_to_sio_or_fps_wm, 0, sizeof(dsm_watermark_type));
  memset(&ds_ucsd_tps_to_sio_or_fps_q, 0, sizeof(q_type));
  dsm_queue_init(&ds_ucsd_tps_to_sio_or_fps_wm, 0, &ds_ucsd_tps_to_sio_or_fps_q);

  /* Clear out the watermarks */
  dsm_empty_queue( &ds_ucsd_fps_from_sio_wm );
  dsm_empty_queue( &ds_ucsd_fps_to_sio_wm );
  dsm_empty_queue( &ds_ucsd_tps_from_sio_or_fps_wm );
  dsm_empty_queue( &ds_ucsd_tps_to_sio_or_fps_wm );

} /* ds_ucsd_mpshim_init_sio_watermarks() */


#ifdef FEATURE_DATA_WCDMA_CS

/*===========================================================================

FUNCTION ds_ucsd_mpshim_init_rlc_watermarks

DESCRIPTION
  This function initializes the Uplink and Downlink GCSD watermarks
  used with RLC.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_mpshim_init_rlc_watermarks( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------
    Initialize the Uplink and Downlink GCSD - RLC watermarks
  ------------------------------------------------------------------*/
  memset(&ds_ucsd_to_rlc_wm, 0, sizeof(dsm_watermark_type));
  memset(&ds_ucsd_to_rlc_q, 0, sizeof(q_type));
  dsm_queue_init(&ds_ucsd_to_rlc_wm, (SDU_SIZE_FOR_64K * 20),&ds_ucsd_to_rlc_q);
  
  memset(&ds_ucsd_from_rlc_wm, 0, sizeof(dsm_watermark_type));
  memset(&ds_ucsd_from_rlc_q, 0, sizeof(q_type));
  dsm_queue_init(&ds_ucsd_from_rlc_wm, (SDU_SIZE_FOR_64K * 20),&ds_ucsd_from_rlc_q);

  /* Clear out the watermarks */
  dsm_empty_queue( &ds_ucsd_to_rlc_wm );
  dsm_empty_queue( &ds_ucsd_from_rlc_wm );
  
  ds_ucsd_to_rlc_wm.lo_watermark          = 0;
  ds_ucsd_to_rlc_wm.hi_watermark          = SDU_SIZE_FOR_64K * 18;
  ds_ucsd_to_rlc_wm.gone_empty_func_ptr   = NULL;
  ds_ucsd_to_rlc_wm.lowater_func_ptr      = NULL;
  ds_ucsd_to_rlc_wm.non_empty_func_ptr    = NULL;
  ds_ucsd_to_rlc_wm.hiwater_func_ptr      = NULL;

  ds_ucsd_from_rlc_wm.lo_watermark        = SDU_SIZE_FOR_64K;
  ds_ucsd_from_rlc_wm.hi_watermark        = SDU_SIZE_FOR_64K * 18;
  ds_ucsd_from_rlc_wm.gone_empty_func_ptr = NULL; 
  ds_ucsd_from_rlc_wm.lowater_func_ptr    = NULL;
  ds_ucsd_from_rlc_wm.non_empty_func_ptr  = NULL;
  ds_ucsd_from_rlc_wm.hiwater_func_ptr    = NULL;

} /* ds_ucsd_mpshim_init_rlc_watermarks() */
#endif /* FEATURE_DATA_WCDMA_CS */


/*===========================================================================

FUNCTION ds_ucsd_mpshim_configure_l2_watermarks

DESCRIPTION
  This function configure watermarks for raw data calls.  The
  watermarks are configured based on the type of call.  Watermarks for
  other call types (e.g. VT) are configured by the external client.

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void 
ds_ucsd_mpshim_configure_l2_watermarks
(
  ds_ucsd_call_tbl_entry_type    *call_info_ptr          /* Call instance  */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /* Setup watermarks used between SIOLIB and GCSD task */
  if( IS_CALL_TYPE( call_info_ptr->call_type, DS_UCSD_FAX_CALL ) ) /*lint !e655 */ 
  {
    /* Fax call */
    call_info_ptr->dpath_params.ul_wm_ptr = &ds_ucsd_fps_from_sio_wm;
    call_info_ptr->dpath_params.dl_wm_ptr = &ds_ucsd_fps_to_sio_wm;
    call_info_ptr->dpath_params.sio_ul_rx_cb = NULL;
  }
  else
  {
    /* Other call types */
    call_info_ptr->dpath_params.ul_wm_ptr = &ds_ucsd_tps_from_sio_or_fps_wm;
    call_info_ptr->dpath_params.dl_wm_ptr = &ds_ucsd_tps_to_sio_or_fps_wm;
    call_info_ptr->dpath_params.sio_ul_rx_cb = NULL;
  }
  
} /* ds_ucsd_mpshim_configure_l2_watermarks() */


/*===========================================================================

FUNCTION ds_ucsd_mpshim_send_gcsd_msg

DESCRIPTION
  This function sends a command message to the GCSD task.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Command is enqueued to GCSD task.

===========================================================================*/
void
ds_ucsd_mpshim_send_gcsd_msg
(
  ds_gcsd_cmd_enum_type   cmd_id,
  cm_call_id_type         call_id
)
{
  ds_gcsd_cmd_type * gcsd_cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Get a comamnd buffer for GCSD task */
  if ( NULL == (gcsd_cmd_ptr = ds_gcsd_get_cmd_buf()) )
  {
    DS_3GPP_MSG0_ERROR ("Unable to allocate GCSD command buffer");
  }
  else
  {
    gcsd_cmd_ptr->hdr.cmd_id = cmd_id;
    gcsd_cmd_ptr->cmd.call_id = call_id;
    
    /* Send the message */
    ds_gcsd_put_cmd (gcsd_cmd_ptr);
  }
} /* ds_ucsd_mpshim_send_gcsd_msg() */

#ifdef FEATURE_TDSCDMA
/*===========================================================================
FUNCTION ds_ucsd_mpshim_register_tdscdma_rlc

DESCRIPTION
  This function registers the watermarks with 
  TD-SCDMA RLC for uplink & downlink data
  
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE -- Sucess
  FALSE -- Failure
    
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ucsd_mpshim_register_tdscdma_rlc
(
  const tdsrrc_user_plane_lc_info_type  *lc_info_ptr,
  const ds_ucsd_dpath_info_type         *dpath_ptr,
  sys_modem_as_id_e_type                 subs_id
)
{
 
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
   DATA_MSG0_MED("In ds_ucsd_mpshim_register_tdscdma_rlc");
#ifdef FEATURE_SEGMENT_LOADING
   if(ds_3gpp_csd_tds_tbl == NULL)
   {
     DATA_MSG0_ERROR("In ds_ucsd_mpshim_register_tdscdma_rlc TDS not active");
     return ( DS3G_FAILURE );
   }
#endif /* FEATURE_SEGMENT_LOADING */
  /*-------------------------------------------------------------------------
   Set up the TD RLC uplink registration parameters 
  -------------------------------------------------------------------------*/

  ds_ucsd_to_tdrlc_ul_cmd.api_id = TDSRLC_UL_MODEMDATA_REG_DEREG_SRVC_REQ;
  /*-----------------------------------------------------------------------
    Uplink Parameters :
    Number of uplink logical channels to register 
    Uplink RLC logical channel ID 
    Uplink Watermark
  -----------------------------------------------------------------------*/
  ds_ucsd_to_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.service = TRUE;//register
  ds_ucsd_to_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.nchan   = 
                          ds_ucsd_rlc_ul_dereg_params.nchan =
                                   lc_info_ptr->num_ul_lc_ids_for_rab;
  ds_ucsd_to_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.rlc_id[ 0 ] =
                          ds_ucsd_rlc_ul_dereg_params.rlc_id[ 0 ] =
                                   lc_info_ptr->ul_lc_id[ 0 ];
  ds_ucsd_to_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.ul_wm_ptr[ 0 ] = 
                                   dpath_ptr->ul_wm_ptr;
  /*-----------------------------------------------------------------------
   Register the watermarks with TD RLC 
  -----------------------------------------------------------------------*/  
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg_sub(ds_3gpp_csd_tds_tbl, 
                                            subs_id, &ds_ucsd_to_tdrlc_ul_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg(ds_3gpp_csd_tds_tbl, &ds_ucsd_to_tdrlc_ul_cmd) != TRUE)
#endif
  
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_ul_submit_datamodem_msg_sub(subs_id, &ds_ucsd_to_tdrlc_ul_cmd) != TRUE)
#else
  if(tdsl2_ul_submit_datamodem_msg(&ds_ucsd_to_tdrlc_ul_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DS_3GPP_MSG1_ERROR("UL WM registration failed lc_id:%d", 
              lc_info_ptr->ul_lc_id[ 0 ]);
  }

  /*-------------------------------------------------------------------------
   Set up the TD RLC uplink registration parameters 
  -------------------------------------------------------------------------*/
  ds_ucsd_from_tdrlc_dl_cmd.api_id = TDSRLC_DL_MODEMDATA_REG_DEREG_SRVC_REQ;

  /*-----------------------------------------------------------------------
    Downlink Parameters :
    Number of downlink logical channels to register 
    Downlink RLC logical channel ID 
    Downlink Watermark
  -----------------------------------------------------------------------*/
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.service = TRUE;//register
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.nchan = 
                            ds_ucsd_rlc_dl_dereg_params.nchan =
                                     lc_info_ptr->num_dl_lc_ids_for_rab;
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.rlc_id[ 0 ] = 
                            ds_ucsd_rlc_dl_dereg_params.rlc_id[ 0 ] =
                                     lc_info_ptr->dl_lc_id[ 0 ];
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.dl_wm_ptr[ 0 ] = 
                                     dpath_ptr->dl_wm_ptr;
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.\
                            rlc_post_rx_proc_func_ptr[ 0 ] = 

                                     dpath_ptr->rlc_dl_rx_cb;
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.\
                            rlc_post_rx_func_ptr_para[ 0 ] = 
NULL;
                            
  /* Task context */
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.context[ 0 ] = FALSE;

  /*-----------------------------------------------------------------------
   Register the watermarks with RLC 
  -----------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
  if((ds_3gpp_csd_tds_tbl != NULL) &&
#ifdef FEATURE_DUAL_WCDMA
     (ITDSCDMA_tdsl2_dl_submit_datamodem_msg_sub(ds_3gpp_csd_tds_tbl,
                     subs_id,
                     &ds_ucsd_from_tdrlc_dl_cmd) != TRUE) 
#else
     (ITDSCDMA_tdsl2_dl_submit_datamodem_msg(ds_3gpp_csd_tds_tbl,
                     &ds_ucsd_from_tdrlc_dl_cmd) != TRUE) 
#endif
     )
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_dl_submit_datamodem_msg_sub(subs_id, &ds_ucsd_from_tdrlc_dl_cmd) != TRUE)
#else
  if(tdsl2_dl_submit_datamodem_msg(&ds_ucsd_from_tdrlc_dl_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DS_3GPP_MSG1_ERROR("DL WM registration failed lc_id:%d", 
              lc_info_ptr->dl_lc_id[ 0 ]);
  } 

  return ( DS3G_SUCCESS );
} /* ds_ucsd_mpshim_register_tdscdma_rlc() */

/*===========================================================================
FUNCTION ds_ucsd_mpshim_deregister_tdscdma_rlc

DESCRIPTION
  This function deregisters the watermarks with 
  TDRLC for UL & DL data

PARAMETERS
  None

DEPENDENCIES
  None.

RETURN VALUE
  TRUE -- Success
  FALSE -- Failure

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ucsd_mpshim_deregister_tdscdma_rlc
(
  sys_modem_as_id_e_type subs_id
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG0_MED("In ds_ucsd_mpshim_deregister_tdscdma_rlc");
#ifdef FEATURE_SEGMENT_LOADING
  if(ds_3gpp_csd_tds_tbl == NULL)
  {
    DATA_MSG0_ERROR("In ds_ucsd_mpshim_register_tdscdma_rlc TDS not active");
    return ( DS3G_FAILURE );
  }
#endif /* FEATURE_SEGMENT_LOADING */
  /*-------------------------------------------------------------------------
   Set up the TD RLC uplink registration parameters 
  -------------------------------------------------------------------------*/
  ds_ucsd_to_tdrlc_ul_cmd.api_id = TDSRLC_UL_MODEMDATA_REG_DEREG_SRVC_REQ;
  ds_ucsd_from_tdrlc_dl_cmd.api_id = TDSRLC_DL_MODEMDATA_REG_DEREG_SRVC_REQ;

  /*-------------------------------------------------------------------------
    Deregister the RLC UL & DL watermarks
  -------------------------------------------------------------------------*/
  ds_ucsd_to_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.service = FALSE;//deregister
  ds_ucsd_to_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.nchan = 
                              ds_ucsd_rlc_ul_dereg_params.nchan;
  ds_ucsd_to_tdrlc_ul_cmd.api_data.ul_reg_dereg_cmd.rlc_id[ 0 ] = 
                              ds_ucsd_rlc_ul_dereg_params.rlc_id[ 0 ];

  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.service = FALSE;//deregister
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.nchan = 
                                ds_ucsd_rlc_dl_dereg_params.nchan;
  ds_ucsd_from_tdrlc_dl_cmd.api_data.dl_reg_dereg_cmd.rlc_id[ 0 ] = 
                                ds_ucsd_rlc_dl_dereg_params.rlc_id[ 0 ];

  /*-------------------------------------------------------------------------
    Post L2 UL command 
  -------------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
  
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg_sub(ds_3gpp_csd_tds_tbl, 
                                                subs_id, &ds_ucsd_to_tdrlc_ul_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_ul_submit_datamodem_msg(ds_3gpp_csd_tds_tbl, &ds_ucsd_to_tdrlc_ul_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_ul_submit_datamodem_msg_sub(subs_id, &ds_ucsd_to_tdrlc_ul_cmd) != TRUE)
#else
  if(tdsl2_ul_submit_datamodem_msg(&ds_ucsd_to_tdrlc_ul_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DS_3GPP_MSG1_ERROR("UL WM registration failed lc_id:%d", 
              ds_ucsd_rlc_ul_dereg_params.rlc_id[ 0 ]);
  }

  /*-------------------------------------------------------------------------
    Post L2 DL command 
  -------------------------------------------------------------------------*/
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
  if(ITDSCDMA_tdsl2_dl_submit_datamodem_msg_sub(ds_3gpp_csd_tds_tbl, subs_id, 
                                            &ds_ucsd_from_tdrlc_dl_cmd) != TRUE)
#else
  if(ITDSCDMA_tdsl2_dl_submit_datamodem_msg(ds_3gpp_csd_tds_tbl, &ds_ucsd_from_tdrlc_dl_cmd) != TRUE)
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
  if(tdsl2_dl_submit_datamodem_msg_sub(subs_id, &ds_ucsd_from_tdrlc_dl_cmd) != TRUE)
#else
  if(tdsl2_dl_submit_datamodem_msg(&ds_ucsd_from_tdrlc_dl_cmd) != TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    DS_3GPP_MSG1_ERROR("DL WM deregistration failed lc_id:%d", 
              ds_ucsd_rlc_dl_dereg_params.rlc_id[ 0 ]);
  }
  return TRUE ;
} /* ds_ucsd_mpshim_deregister_tdscdma_rlc */
#endif /* FEATURE_TDSCDMA */


#ifdef FEATURE_DATA_WCDMA_CS
/*===========================================================================

FUNCTION ds_ucsd_mpshim_register_wcdma_rlc

DESCRIPTION
  This function registers the interface with WCDMA Layer 2 RLC.
        
DEPENDENCIES
   None
  
RETURN VALUE
   TRUE for successful processing; FALSE otherwise.
    
SIDE EFFECTS
   Commands queued to L2 UL/DL tasks.

===========================================================================*/
boolean
ds_ucsd_mpshim_register_wcdma_rlc
(
  const rrc_user_plane_lc_info_type  *lc_info_ptr,
  const ds_ucsd_dpath_info_type      *dpath_ptr,
  sys_modem_as_id_e_type              subs_id
)
{
  /* Pointer to L2 UL command */
  l2_ul_cmd_type              *l2_ul_cmd_ptr;

  /* Pointer to L2 DL command */
  l2_dl_cmd_type              *l2_dl_cmd_ptr;

  /* RLC Uplink registration parameters */
  rlc_ul_register_srvc_type    rlc_ul_reg_params;

  /* RLC Downlink registration parameters */
  rlc_dl_register_srvc_type    rlc_dl_reg_params;

/*-------------------------------------------------------------------------*/

   ASSERT( NULL != lc_info_ptr );
  
   DATA_MSG0_MED("In ds_ucsd_mpshim_register_wcdma_rlc");
   
#ifdef FEATURE_SEGMENT_LOADING
   if(ds_3gpp_csd_wcdma_tbl == NULL)
   {
     DATA_MSG0_ERROR("In ds_ucsd_mpshim_register_wcdma_rlc WCDMA not active");
     return ( DS3G_FAILURE );
   }
#endif /* FEATURE_SEGMENT_LOADING */
   
   /*-----------------------------------------------------------------
     Set the RLC uplink registration and deregistration parameters
   ------------------------------------------------------------------*/
   
   /* Task context */
   rlc_dl_reg_params.rlc_data[ 0 ].context = FALSE;

   /* Number of uplink logical channels to register/deregister */
   ds_ucsd_rlc_ul_dereg_params.nchan = 
     rlc_ul_reg_params.nchan = 
     lc_info_ptr->num_ul_lc_ids_for_rab;

   /* Uplink RLC logical channel ID */
   ds_ucsd_rlc_ul_dereg_params.rlc_id[ 0 ] = 
     rlc_ul_reg_params.rlc_data[ 0 ].lc_id = 
     lc_info_ptr->ul_lc_id[ 0 ];

   /*-----------------------------------------------------------------
     Set the RLC downlink registration and deregistration parameters
   ------------------------------------------------------------------*/

   /* Number of downlink logical channels to register/deregister */
   ds_ucsd_rlc_dl_dereg_params.nchan = 
     rlc_dl_reg_params.nchan = 
     lc_info_ptr->num_dl_lc_ids_for_rab;

   /* Downlink RLC logical channel ID */
   ds_ucsd_rlc_dl_dereg_params.rlc_id[ 0 ] = 
     rlc_dl_reg_params.rlc_data[ 0 ].lc_id = 
     lc_info_ptr->dl_lc_id[ 0 ];

   /*-----------------------------------------------------------------
     Set the RLC datapath parameters
   ------------------------------------------------------------------*/
         
   /* Set the pointers to Uplink and Downlink RLC WMs */
   rlc_ul_reg_params.rlc_data[ 0 ].ul_wm_ptr = dpath_ptr->ul_wm_ptr;
   rlc_dl_reg_params.rlc_data[ 0 ].dl_wm_ptr = dpath_ptr->dl_wm_ptr;
         
   /* Set the RLC callback function */
   rlc_dl_reg_params.rlc_data[ 0 ].rlc_post_rx_proc_func_ptr = 
     dpath_ptr->rlc_dl_rx_cb;
   

   /*-----------------------------------------------------------------
     Register the Uplink and Downlink watermarks with RLC
   ------------------------------------------------------------------*/

   /* Obtain an L2 UL command buffer */
#ifdef FEATURE_SEGMENT_LOADING
   if((l2_ul_cmd_ptr = IWCDMA_l2_ul_get_cmd_buf(ds_3gpp_csd_wcdma_tbl))!= NULL)
#else
   if ( ( l2_ul_cmd_ptr = l2_ul_get_cmd_buf() ) != NULL )
#endif /* FEATURE_SEGMENT_LOADING */  	
   {
     l2_ul_cmd_ptr->cmd_hdr.cmd_id = RLC_UL_REGISTER_SRVC_REQ;
     l2_ul_cmd_ptr->cmd_data.ul_reg = rlc_ul_reg_params;
#ifdef FEATURE_DUAL_WCDMA
     l2_ul_cmd_ptr->cmd_hdr.as_id = subs_id;
#endif /* FEATURE_DUAL_WCDMA */

#ifdef FEATURE_SEGMENT_LOADING
     IWCDMA_l2_ul_put_cmd(ds_3gpp_csd_wcdma_tbl, l2_ul_cmd_ptr);
#else
     l2_ul_put_cmd( l2_ul_cmd_ptr );
#endif /* FEATURE_SEGMENT_LOADING */
   }
   else
   {
     DS_3GPP_MSG0_ERROR( "Cannot get L2 UL cmd buffer");
   }

   /* Obtain an L2 DL command buffer */
#ifdef FEATURE_SEGMENT_LOADING
   if ( ( l2_dl_cmd_ptr = IWCDMA_l2_dl_get_cmd_buf(ds_3gpp_csd_wcdma_tbl))!= NULL )
#else
   if ( ( l2_dl_cmd_ptr = l2_dl_get_cmd_buf() ) != NULL )
#endif /* FEATURE_SEGMENT_LOADING */
   {
     l2_dl_cmd_ptr->cmd_hdr.cmd_id = RLC_DL_REGISTER_SRVC_REQ;
     l2_dl_cmd_ptr->cmd_data.dl_reg = rlc_dl_reg_params;
#ifdef FEATURE_DUAL_WCDMA
     l2_ul_cmd_ptr->cmd_hdr.as_id = subs_id;
#endif /* FEATURE_DUAL_WCDMA */

#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_dl_put_cmd(ds_3gpp_csd_wcdma_tbl, l2_dl_cmd_ptr );
#else
     l2_dl_put_cmd( l2_dl_cmd_ptr );
#endif /* FEATURE_SEGMENT_LOADING */
   }
   else
   {
     DS_3GPP_MSG0_ERROR( "Cannot get L2 DL cmd buffer");
   }

   return ( DS3G_SUCCESS );
} /* ds_ucsd_mpshim_register_wcdma_rlc() */



/*===========================================================================

FUNCTION ds_ucsd_mpshim_deregister_wcdma_rlc

DESCRIPTION
   This function deregisters the interface with WCDMA RLC.
        
DEPENDENCIES
   None
  
RETURN VALUE
   TRUE on successful processing; FALSE otherwise.
    
SIDE EFFECTS
   Command sent to L2 UL & DL tasks

===========================================================================*/
boolean
ds_ucsd_mpshim_deregister_wcdma_rlc 
(
  sys_modem_as_id_e_type subs_id 
)
{
   /* Pointer to L2 UL command */
   l2_ul_cmd_type   *l2_ul_cmd_ptr;

   /* Pointer to L2 DL command */
   l2_dl_cmd_type   *l2_dl_cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_MSG0_MED("In ds_ucsd_mpshim_deregister_wcdma_rlc");
   
#ifdef FEATURE_SEGMENT_LOADING
   if(ds_3gpp_csd_wcdma_tbl == NULL)
   {
     DATA_MSG0_ERROR("In ds_ucsd_mpshim_deregister_wcdma_rlc WCDMA not active");
     return ( FALSE );
   }

  if ( ( l2_ul_cmd_ptr = IWCDMA_l2_ul_get_cmd_buf(ds_3gpp_csd_wcdma_tbl) ) != NULL )
#else   
  if ( ( l2_ul_cmd_ptr = l2_ul_get_cmd_buf() ) != NULL )
#endif /* FEATURE_SEGMENT_LOADING */
  {
    l2_ul_cmd_ptr->cmd_hdr.cmd_id = RLC_UL_DEREGISTER_SRVC_REQ;
    l2_ul_cmd_ptr->cmd_data.ul_dereg = ds_ucsd_rlc_ul_dereg_params;
#ifdef FEATURE_DUAL_WCDMA
    l2_ul_cmd_ptr->cmd_hdr.as_id = subs_id;
#endif /* FEATURE_DUAL_WCDMA */

#ifdef FEATURE_SEGMENT_LOADING
   IWCDMA_l2_ul_put_cmd(ds_3gpp_csd_wcdma_tbl, l2_ul_cmd_ptr);
#else
    l2_ul_put_cmd( l2_ul_cmd_ptr );
#endif /* FEATURE_SEGMENT_LOADING */
  }
  else
  {
    DS_3GPP_MSG0_ERROR( "Cannot get L2 UL cmd buffer");
  }

#ifdef FEATURE_SEGMENT_LOADING
  if ( ( l2_dl_cmd_ptr = IWCDMA_l2_dl_get_cmd_buf(ds_3gpp_csd_wcdma_tbl) ) != NULL )
#else
  if ( ( l2_dl_cmd_ptr = l2_dl_get_cmd_buf() ) != NULL )
#endif /* FEATURE_SEGMENT_LOADING */
  {
    l2_dl_cmd_ptr->cmd_hdr.cmd_id = RLC_DL_DEREGISTER_SRVC_REQ;
    l2_dl_cmd_ptr->cmd_data.dl_dereg = ds_ucsd_rlc_dl_dereg_params;
#ifdef FEATURE_DUAL_WCDMA
    l2_ul_cmd_ptr->cmd_hdr.as_id = subs_id;
#endif /* FEATURE_DUAL_WCDMA */

#ifdef FEATURE_SEGMENT_LOADING
    IWCDMA_l2_dl_put_cmd(ds_3gpp_csd_wcdma_tbl, l2_dl_cmd_ptr );
#else
    l2_dl_put_cmd( l2_dl_cmd_ptr );
#endif /* FEATURE_SEGMENT_LOADING */
  }
  else
  {
    DS_3GPP_MSG0_ERROR( "Cannot get L2 DL cmd buffer");
  }

  return ( TRUE );
} /* ds_ucsd_mpshim_deregister_wcdma_rlc() */

#endif /* FEATURE_DATA_WCDMA_CS */


#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_rlc_datapath

DESCRIPTION
  This function returns the elements for the datapath to WCDMA RLC
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void
ds_ucsd_mpshim_get_rlc_datapath
(
  ds_ucsd_dpath_info_type * datapath_ptr
)
{
/*-------------------------------------------------------------------------*/

  ASSERT( NULL != datapath_ptr );

  /* Set the RLC watermarks */
  datapath_ptr->ul_wm_ptr = &ds_ucsd_to_rlc_wm;
  datapath_ptr->dl_wm_ptr = &ds_ucsd_from_rlc_wm;
  datapath_ptr->rlc_dl_rx_cb = NULL;
  datapath_ptr->sio_ul_rx_cb = NULL;
}/* ds_ucsd_mpshim_get_rlc_datapath */

#endif /* (FEATURE_DATA_WCDMA_CS) || (FEATURE_TDSCDMA) */

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_rlp_dc_state

DESCRIPTION
  This function returns the RLP data compression state.
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
byte
ds_ucsd_mpshim_get_rlp_dc_state( void )
{
/*-------------------------------------------------------------------------*/

  /* Return the RLP data compression state */
  return ds_ucsd_nt_get_rlp_P0();
} /* ds_ucsd_mpshim_get_rlp_dc_state */



/*===========================================================================

FUNCTION ds_ucsd_mpshim_connect_data_path

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE
  DS3G_SUCCESS if operations complete successfully, DS3G_FAILURE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_mpshim_connect_data_path
(
  const dsucsd_call_descriptor_type  *call_desc_ptr,   /* Call descriptor */
  const ds_ucsd_ota_channel_params_T *chan_parms_ptr   /* RLC OTA info   */
)
{
   ds_ucsd_frame_size_T nt_bytes_per_sdu = DS_UMTS_CSD_FRAME_SIZE_INVALID;
   ds_ucsd_data_rate_T  data_rate = DS_UCSD_RATE_NONE;
   uint16               sdus_per_tti = 0;
   boolean              call_originated;
   
/*-------------------------------------------------------------------------*/

   ASSERT( NULL != call_desc_ptr );
   ASSERT( NULL != chan_parms_ptr );

   DATA_MSG0_MED( "Connecting CSD data path (mpshim)");

   /* Configure the GCSD module */
   if( call_desc_ptr->network_mode == SYS_SYS_MODE_GSM )
   {
     /* GSM CS Data */
#ifdef FEATURE_DATA_GCSD  
     /* Data_rate is mapped to the FNUR */
     if ( call_desc_ptr->bearer_cap.fixed_network_user_rate == FNUR_14400 )
     {
       /* 14.4k call */
       data_rate = DS_UCSD_RATE_14400;
       nt_bytes_per_sdu = DS_UMTS_CSD_FRAME_SIZE_288_BIT;
     }
     else
     {
       /* 9.6k call */
       data_rate = DS_UCSD_RATE_9600;
       nt_bytes_per_sdu = DS_UMTS_CSD_FRAME_SIZE_240_BIT;
     }

     /* 1 SDU every TTI */
     sdus_per_tti = 1;
     
     /* Register callback functions for transferring data between the 
     ** UCSD NT Protocol Stack and Layer 1.
     */
     ds_ucsd_nt_register_ota_funcs( ds_gcsd_read_from_channel_decoder,
                                    ds_gcsd_write_to_channel_coder,
                                    ds_gcsd_get_tx_buffer,
                                    ds_gcsd_l1_init );
 #endif /* FEATURE_DATA_GCSD */        
   }
#if defined ( FEATURE_DATA_WCDMA_CS ) || defined (FEATURE_TDSCDMA)
   else if( (DS_UCSD_IS_WCDMA_TDS_MODE(call_desc_ptr->network_mode)) &&
            (call_desc_ptr->bearer_cap.sync_async == ASYNCHRONOUS) )
   {
     /* WCDMA Async CS Data */
     
     sdus_per_tti = chan_parms_ptr->sdus_per_tti;
     nt_bytes_per_sdu = (ds_ucsd_frame_size_T)chan_parms_ptr->sdu_size;
     
     /* Register callback functions for transferring data between the 
     ** UCSD NT Protocol Stack and RLC.
     */
     ds_ucsd_nt_register_ota_funcs( dswcsd_nt_read_dl_ota_data_cb,
                                    dswcsd_nt_write_ul_ota_data_cb,
                                    dswcsd_nt_get_ul_ota_buf_cb,
                                    dswcsd_nt_init_ota_cb );

     /* Set L2 WM callback functions to signal GCSD task */
     ds_ucsd_to_rlc_wm.gone_empty_func_ptr  = dswcsd_nt_set_proc_ul_sig_cb;
     ds_ucsd_from_rlc_wm.non_empty_func_ptr = dswcsd_nt_set_proc_dl_sig_cb;

   }
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_TDSCDMA */

   /* Initialize the GCSD task per-call data */
   call_originated = (DS_UCSD_CALL_DIR_ORIGINATED == call_desc_ptr->call_dir);
   
   /* Single processor targets */
   ds_gcsd_init_call_data( call_desc_ptr->cm_call_id,
                           call_originated,
                           call_desc_ptr->network_mode,
                           data_rate,
                           nt_bytes_per_sdu,
                           (uint32)sdus_per_tti,
                           &call_desc_ptr->bearer_cap,
                           &call_desc_ptr->rlp_parameters,
                           ds_ucsd_set_data_counters );

   

#if defined ( FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
   /* If Async data, clear out the DL watermark again to make sure that there was 
   ** no data queued between clearing the watermarks and initializing the NT stack
   ** If there was, DL processing will never start because the downlink 
   ** processing indication would have been ignored by the NT stack and
   ** the frame would have never been read.  
   ** At this point if there is data in the WM, it is garbage anyway.
   */
   if( (DS_UCSD_IS_WCDMA_TDS_MODE(call_desc_ptr->network_mode)) &&
       (call_desc_ptr->bearer_cap.sync_async == ASYNCHRONOUS) )
   {
     dsm_empty_queue( &ds_ucsd_from_rlc_wm );
   }
#endif /* FEATURE_DATA_WCDMA_CS || TDSCDMA*/

   /* Send connect message to protocol stack task */
   /* Set the command ID based on MO or MT call */
   if( DS_UCSD_CALL_DIR_ORIGINATED == call_desc_ptr->call_dir )
   {
     ds_ucsd_mpshim_send_gcsd_msg( DS_GCSD_CONNECT_MO_REQ,
                                   call_desc_ptr->cm_call_id );
   }
   else
   {
     ds_ucsd_mpshim_send_gcsd_msg( DS_GCSD_CONNECT_MT_REQ,
                                   call_desc_ptr->cm_call_id );
   }

#ifdef FEATURE_DATA_WCDMA_CS
   if( (DS_UCSD_IS_WCDMA_TDS_MODE(call_desc_ptr->network_mode) ) &&
       (call_desc_ptr->bearer_cap.sync_async == ASYNCHRONOUS) )
   {
     /* Kickstart the Uplink data transfer for WCDMA Async CS Data */
#ifdef FEATURE_MODEM_RCINIT_PHASE2
     gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd"); 
     if ( RCINIT_NULL == gcsd_tcb_ptr)
     {
        DATA_ERR_FATAL("RCINIT Lookup for gcsd tcb returned NULL");
     }
#else
     gcsd_tcb_ptr = &Gcsd_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */
     (void)rex_set_sigs( gcsd_tcb_ptr, DS_GCSD_PROC_UL_SIG );
   }
#endif /* FEATURE_DATA_WCDMA_CS */

   return DS3G_SUCCESS;
} /* ds_ucsd_mpshim_connect_data_path() */



/*===========================================================================

FUNCTION ds_ucsd_mpshim_disconnect_data_path

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE
  DS3G_SUCCESS if operations complete successfully, DS3G_FAILURE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_mpshim_disconnect_data_path
(
  const sys_sys_mode_e_type          network_mode
)
{
/*-------------------------------------------------------------------------*/
  /*lint -e506 -e774 */
  
#ifdef FEATURE_DATA_WCDMA_CS
  if( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode))
  {
    /* Clear L2 watermarks */
    ds_ucsd_to_rlc_wm.gone_empty_func_ptr  = NULL;
    ds_ucsd_from_rlc_wm.non_empty_func_ptr = NULL;

    dsm_empty_queue( &ds_ucsd_from_rlc_wm ); 
    dsm_empty_queue( &ds_ucsd_to_rlc_wm ); 
  }
#endif /* FEATURE_DATA_WCDMA_CS */
     
  /* Clear out all of the CSD watermarks to free all DSM items */
  dsm_empty_queue( &ds_ucsd_fps_from_sio_wm ); 
  dsm_empty_queue( &ds_ucsd_fps_to_sio_wm ); 
  dsm_empty_queue( &ds_ucsd_tps_from_sio_or_fps_wm ); 
  dsm_empty_queue( &ds_ucsd_tps_to_sio_or_fps_wm );

  /*lint +e506 +e774 */
  return DS3G_SUCCESS;
} /* ds_ucsd_mpshim_disconnect_data_path */


/*===========================================================================

FUNCTION ds_ucsd_mpshim_call_cleanup

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE
  DS3G_SUCCESS if operations complete successfully, DS3G_FAILURE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_mpshim_call_cleanup
(
  const dsucsd_call_descriptor_type *call_desc_ptr,     /* Call descriptor */
  const boolean                      call_was_connected  /* State flag     */
)
{
  boolean result = DS3G_SUCCESS;
  
/*-------------------------------------------------------------------------*/

  MSG_MED("In ds_ucsd_mpshim_call_cleanup: call ID=%d",
          call_desc_ptr->cm_call_id,0,0);
  
  /* Check if call was connected. */
  if( TRUE == call_was_connected )
  {
    if ( ( call_desc_ptr->network_mode == SYS_SYS_MODE_GSM ) ||
         ( ( DS_UCSD_IS_WCDMA_TDS_MODE(call_desc_ptr->network_mode) ) &&
           ( call_desc_ptr->bearer_cap.sync_async == ASYNCHRONOUS ) ) )
    {
      /* GSM or WCDMA Async CS Data */

      /* Only send message to GCSD task if we have a network originated
      ** termination.  Mobile originated terminations will be sent to 
      ** GCSD in ds_ucsd_user_end_handler()
      */
      if( !call_desc_ptr->user_initiated_termination )
      {
        /* Send disconnect message to protocol stack task */
        ds_ucsd_mpshim_send_gcsd_msg( DS_GCSD_DISCONNECT_MT_REQ,
                                      call_desc_ptr->cm_call_id );
      }
    }

    /* Disconnect datapath  */
    if( DS3G_SUCCESS !=
        ds_ucsd_mpshim_disconnect_data_path( call_desc_ptr->network_mode ) )
    {
      DATA_MSG0_ERROR("Problem disconnecting data path");
      result = DS3G_FAILURE;
    }
  }

  return result;
} /* ds_ucsd_mpshim_call_cleanup */


#if defined(FEATURE_DATA_WCDMA_CS) && defined(FEATURE_DATA_UCSD_HANDOVER)

/*===========================================================================

FUNCTION ds_ucsd_mpshim_nt_handover_change_config

DESCRIPTION
  This function changes the NT call configuration for WCDMA or GSM
  target mode.  The OTA callback functions for interfacing with lower
  layers are set based on handover direction/reason.  

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if processing successful; FALSE otherwise
    
SIDE EFFECTS
  None

===========================================================================*/
boolean
ds_ucsd_mpshim_nt_handover_change_config
(
  const dsucsd_call_descriptor_type   *call_desc_ptr,   /* Call descriptor */
  const sync_enum_T                    direction,       /* HO direction    */
  const ds_ucsd_ota_channel_params_T  *chan_params_ptr  /* RLC OTA info    */
)
{
  boolean  result = TRUE;
  static ds_ucsd_data_rate_T  data_rate = DS_UCSD_RATE_NONE;
  ds_ucsd_frame_size_T   nt_bytes_per_sdu = DS_UMTS_CSD_FRAME_SIZE_INVALID; 
  sys_sys_mode_e_type new_nw_mode = SYS_SYS_MODE_NONE;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != call_desc_ptr );
  ASSERT( NULL != chan_params_ptr );
  
  MSG_MED ("Handover change config: dir=%d datarate=%d SDUsize=%d",
           direction,
           chan_params_ptr->data_rate,
           chan_params_ptr->sdu_size);

  switch ( direction )
  {
  case GSM_GSM_HANDOVER:
    /* Check for null change in GSM/GSM handover */
    if (data_rate == chan_params_ptr->data_rate)
    {
      return result; 
    }
    /* Intentional fallthrough to next case */
    /*lint -fallthrough */
  case WCDMA_GSM_HANDOVER:
    new_nw_mode = SYS_SYS_MODE_GSM;

    /* Register GSM callback functions for transferring data between the 
    ** UCSD NT Protocol Stack and Layer 1.
    */
    ds_ucsd_nt_register_ota_funcs( ds_gcsd_read_from_channel_decoder,
                                   ds_gcsd_write_to_channel_coder,
                                   ds_gcsd_get_tx_buffer,
                                   ds_gcsd_l1_init );
         
    /* Set the bytes_per_sdu to the DS_UMTS_CSD_FRAME_SIZE_288_BIT value for
    ** 14.4 Async calls in GSM mode. 
    */
    data_rate = (ds_ucsd_data_rate_T)chan_params_ptr->data_rate;
    if ( data_rate == DS_UCSD_RATE_14400 )
    {
      /* Frame size for 14.4k */
      nt_bytes_per_sdu = DS_UMTS_CSD_FRAME_SIZE_288_BIT;
    }
    else
    {
      /* Frame size for 9.6k */
      nt_bytes_per_sdu = DS_UMTS_CSD_FRAME_SIZE_240_BIT;
    }
    break;

  case GSM_WCDMA_HANDOVER:
    new_nw_mode = SYS_SYS_MODE_WCDMA;
    nt_bytes_per_sdu = (ds_ucsd_frame_size_T)chan_params_ptr->sdu_size;

    /* Register WCDMA callback functions for transferring data between the 
    ** UCSD NT Protocol Stack and RLC.
    */
    ds_ucsd_nt_register_ota_funcs( dswcsd_nt_read_dl_ota_data_cb,
                                   dswcsd_nt_write_ul_ota_data_cb,
                                   dswcsd_nt_get_ul_ota_buf_cb,
                                   dswcsd_nt_init_ota_cb );
    break;

  default:
    MSG_ERROR( "Unsupported handover direction: %d",
               direction,0,0 );
    return FALSE;
  }

  /* Initialize the NT call data */
  ds_ucsd_nt_init_call_data( call_desc_ptr->cm_call_id,
                             new_nw_mode,
                             nt_bytes_per_sdu,                           
                             chan_params_ptr->sdus_per_tti,
                             &call_desc_ptr->rlp_parameters,
                             data_rate,
                             call_desc_ptr->bearer_cap.compression,
                             TRUE,
                             NULL );

  return result;
} /* ds_ucsd_mpshim_nt_handover_change_config() */



/*===========================================================================

FUNCTION ds_ucsd_mpshim_nt_handover_wrapup

DESCRIPTION
  This function performs wrapup for interRAT handover processing.  If
  there was an error indicated, call teardown is triggered.  It
  changes the NT call configuration if neceaasy to ensure the lower
  layers will clean up correctly.  GCSD task is signaled to kickstart
  RLP is new mode is WCDMA.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Command enqueued to GCSD task.

===========================================================================*/
void
ds_ucsd_mpshim_nt_handover_wrapup
(
  const dsucsd_call_descriptor_type   *call_desc_ptr,   /* Call descriptor */
  const sync_enum_T                    direction,       /* HO direction    */
  const boolean                        success          /* Status flag     */ 
)
{
  /* Problem during HO processing so terminate call. */
  if( FALSE == success )
  {
    /* If HO was to WCDMA, configure RLP stack to WCDMA mode for DTX timer. */
    if( ( GSM_WCDMA_HANDOVER == direction ) &&
        ( ASYNCHRONOUS == call_desc_ptr->bearer_cap.sync_async ) )
    {
      ds_ucsd_nt_init_call_data( call_desc_ptr->cm_call_id,
                                 SYS_SYS_MODE_WCDMA,
                                 DS_UMTS_CSD_FRAME_SIZE_INVALID,
                                 0,
                                 &call_desc_ptr->rlp_parameters,
                                 DS_UCSD_RATE_NONE,
                                 FALSE,
                                 TRUE,
                                 NULL );
    }

    /* Teardown call without data flush */
    ds_ucsd_mpshim_send_gcsd_msg ( DS_GCSD_DISCONNECT_MT_REQ,
                                   call_desc_ptr->cm_call_id );
  }

  /* Kickstart the Uplink data transfer for WCDMA Async CS Data.
   * Even if call teardaown is required, need to trigger RLP
   * processing this way. */
  if( ( GSM_WCDMA_HANDOVER == direction ) &&
      ( ASYNCHRONOUS == call_desc_ptr->bearer_cap.sync_async ) )
  {
    DATA_MSG0_HIGH("Kickstarting RLP processing after G->W HO.");
#ifdef FEATURE_MODEM_RCINIT_PHASE2
  gcsd_tcb_ptr = rcinit_lookup_rextask("ds_gcsd"); 
  if ( RCINIT_NULL == gcsd_tcb_ptr)
  {
    DATA_ERR_FATAL("RCINIT Lookup for gcsd tcb returned NULL");
  }
#else
  gcsd_tcb_ptr = &Gcsd_tcb;
#endif /*FEATURE_MODEM_RCINIT_PHASE2 */
    (void)rex_set_sigs( gcsd_tcb_ptr, DS_GCSD_PROC_UL_SIG );
  }
  
} /* ds_ucsd_mpshim_nt_handover_wrapup() */

#endif /* defined(FEATURE_DATA_WCDMA_CS) && defined(FEATURE_DATA_UCSD_HANDOVER) */


/*===========================================================================

FUNCTION ds_ucsd_mpshim_init

DESCRIPTION
  This funciton initializes the module at power up.

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void
ds_ucsd_mpshim_init( void )
{
/*-------------------------------------------------------------------------*/

   /* Initialize the Uplink and Downlink GCSD watermarks */
   ds_ucsd_mpshim_init_sio_watermarks();
#ifdef FEATURE_DATA_WCDMA_CS
   ds_ucsd_mpshim_init_rlc_watermarks();
#endif /* FEATURE_DATA_WCDMA_CS */

#if defined(FEATURE_DATA_GCSD) && defined(FEATURE_DATA_WCDMA_CS) && \
    defined(FEATURE_DATA_UCSD_HANDOVER)
   /* Register inter-RAT handover callback with NAS/MN */
   if (TRUE !=
       MN_register_cb_services( MN_CLIENT_DS,
                                MN_SYNC_NOTIFICATION,
                                (void*)ds_ucsd_handover_notify
       ) ) /*lint !e611 */
   {
     DATA_MSG0_ERROR("Failed to register for NAS handover notification");
   }
   else
   {
     DATA_MSG0_MED("Registered with NAS/MN for handover notification");
   }
#endif /* FEATURE_DATA_UCSD_HANDOVER */
   
} /* ds_ucsd_mpshim_init() */

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_tps_from_sio_or_fps_wm

DESCRIPTION
  This funciton returns a pointer to the tps_from_sio_or_fps_wm

DEPENDENCIES
  None
  
RETURN VALUE
  dsm_watermark_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * ds_ucsd_mpshim_get_tps_from_sio_or_fps_wm(void)
{
  return &ds_ucsd_tps_from_sio_or_fps_wm;
}

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_tps_to_sio_or_fps_wm

DESCRIPTION
  This funciton returns a pointer to the tps_to_sio_or_fps_wm

DEPENDENCIES
  None
  
RETURN VALUE
  dsm_watermark_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * ds_ucsd_mpshim_get_tps_to_sio_or_fps_wm(void)
{
  return &ds_ucsd_tps_to_sio_or_fps_wm;
}

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_fps_from_sio_wm

DESCRIPTION
  This funciton returns a pointer to the fps_from_sio_wm

DEPENDENCIES
  None
  
RETURN VALUE
  dsm_watermark_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * ds_ucsd_mpshim_get_fps_from_sio_wm(void)
{
  return &ds_ucsd_fps_from_sio_wm;
}

/*===========================================================================

FUNCTION ds_ucsd_mpshim_get_fps_to_sio_wm

DESCRIPTION
  This funciton returns a pointer to the fps_to_sio_wm

DEPENDENCIES
  None
  
RETURN VALUE
  dsm_watermark_type *

SIDE EFFECTS
  None

===========================================================================*/
dsm_watermark_type * ds_ucsd_mpshim_get_fps_to_sio_wm(void)
{
  return &ds_ucsd_fps_to_sio_wm;
}

#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

