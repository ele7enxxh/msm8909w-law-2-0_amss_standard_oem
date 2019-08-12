#ifndef TM_XTRA_T_H
#define TM_XTRA_T_H

#include "gps_variation.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifdef FEATURE_CGPS_XTRA_T
#include "pdapiclient.h"

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 


  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_xtra_t.h#1 $
  
when       who   what, where, why
--------   ---   ----------------------------------------------------------
01/06/09   atien Initial version
===========================================================================*/

#include "pdapi.h"
#include "msg.h"
#include "sm_api.h"
#include "gm_api.h"
#include "tm_prtl_iface.h"

/*===========================================================================
=============================================================================

   TM XTRA-T data structures and declarations
   
=============================================================================
===========================================================================*/
#define TM_XTRA_T_MSG_LOW( str, a, b, c )       MSG_LOW( "=TM XTRA-T= " str, a, b, c )
#define TM_XTRA_T_MSG_MED( str, a, b, c )       MSG_MED( "=TM XTRA-T= " str, a, b, c )
#define TM_XTRA_T_MSG_HIGH( str, a, b, c )      MSG_HIGH( "=TM XTRA-T= " str, a, b, c )
#define TM_XTRA_T_MSG_ERROR( str, a, b, c )     MSG_ERROR( "=TM XTRA-T= " str, a, b, c )


/*
** TM XTRA-T Module State information.
*/
typedef enum{
  TM_XTRA_T_STATE_IDLE,
  TM_XTRA_T_STATE_BEGIN,   /* start of XTRA-T session */
  TM_XTRA_T_STATE_DATA,    /* data upload to client / download to MC in progress */
  TM_XTRA_T_STATE_ENDING,  /* ending of XTRA-T session */
  TM_XTRA_T_STATE_MAX,   /* For internal use */
} tm_xtra_t_state_e_type;


/*
** TM XTRA-T information
*/
typedef struct 
{
  /*========================================*/
  /* XTRA-T session related                 */
  /*========================================*/ 
  tm_xtra_t_state_e_type                           e_state;        /* Session state */
  pdsm_xtra_t_session_e_type                       e_session_type; /* Session type */

  /*========================================*/
  /* NV Settings cached here after power up */
  /*========================================*/
  /* Enable/Disable XTRA-T feature */
  boolean                                          xtra_t_enabled;


  /*Geofencing Client related info*/  
  sm_gm_client_handle tm_xtra_t_gm_client_handle;
  
  /* A structure that contains pointer to API functions exposed by TmCore to
     handle its registerd clients for callbacks */
  tm_prtl_cb_s_type z_prtlIfaceTbl;

  /*Whether Position is Pending for Geofence*/
  boolean is_gm_pos_pending;
} tm_xtra_t_info_type;

/*===========================================================================
=============================================================================

   Extern Functions
   
=============================================================================
===========================================================================*/

/*===========================================================================
FUNCTION tm_xtra_t_init

DESCRIPTION
Initialize the XTRA-T module

INPUT PARAMS

OUTPUT PARAMS
None

RETURN VALUE 
XTRA-T command status
===========================================================================*/
void tm_xtra_t_init(void);

/*===========================================================================
FUNCTION tm_xtra_t_check_get_data_param

DESCRIPTION
Check the input parameters for XTRA-T get data client request.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA-T command status
===========================================================================*/
extern pdsm_xtra_t_cmd_err_e_type tm_xtra_t_check_get_data_param
(
  const pdsm_xtra_t_cmd_s_type *req_ptr
);


/*===========================================================================
FUNCTION tm_xtra_t_cell_db_report_proc

DESCRIPTION
Process segmented Cell DB report into the Data Transfer Format (DTR)

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA-T command status
===========================================================================*/
extern void tm_xtra_t_cell_db_report_proc(void *cell_db_report_ptr);


/*===========================================================================
FUNCTION tm_xtra_t_trigger_event

DESCRIPTION
Send the specified XTRA-T event to the client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 

===========================================================================*/
extern boolean tm_xtra_t_trigger_event(pdsm_xtra_t_info_s_type  *p_xtra_t_info);


/*===========================================================================
FUNCTION tm_xtra_t_process_get_data_cmd

DESCRIPTION
Process the get data command request from XTRA-T client

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE 
XTRA-T command status
===========================================================================*/
extern boolean tm_xtra_t_process_get_data_cmd(pdsm_xtra_t_cmd_s_type *cmd_ptr);

/*===========================================================================
FUNCTION tm_xtra_t_handle_best_avail_pos

DESCRIPTION
This function handles Best Avail Pos from TM CORE

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE

===========================================================================*/
void tm_xtra_t_handle_best_avail_pos(const sm_GnssFixRptStructType *p_gnss_fix_rpt);

#endif /* FEATURE_CGPS_XTRA_T */

#ifdef __cplusplus
}
#endif

#endif /* TM_XTRA_T_H */

/*===========================================================================

FUNCTION tm_xtm_handle_nv_write_status

DESCRIPTION
This process calls the corresponding TM callback to report the success
of NV write of XTRAT Client ID.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
boolean

===========================================================================*/

boolean tm_xtm_client_token_nv_write_status(void* p_Data);

/*===========================================================================

FUNCTION tm_xtm_session_control_nv_write_status

DESCRIPTION
This process calls the corresponding TM callback to report the success
of NV write of XTRAT Session Control.

INPUT PARAMS
None

OUTPUT PARAMS
None

RETURN VALUE
boolean

===========================================================================*/

boolean tm_xtm_session_control_nv_write_status(void* p_Data);
