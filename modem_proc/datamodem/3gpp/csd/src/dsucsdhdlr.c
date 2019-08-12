/*===========================================================================

             U M T S   C I R C U I T - S W I T C H E D   D A T A

                M O D E - S P E C I F I C   H A N D L E R S


DESCRIPTION
  This file contains the implementation of the UMTS Circuit Switched
  mode-specific handlers.

  Copyright (c) 2003 - 2014 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdhdlr.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   ss      3GPP MH LTE-Off compilation fixes.
07/19/12   nd      Release the call when channel characteristic is not
                   supported by network.
05/09/12   nd      Memory reduction effort in 3gpp.
03/22/12   nd      Compilation error due to CLK API changes.
02/08/12   sk      Feature cleanup.
01/13/12   SA      Added support for CSVT for CSFB targets.
11/11/11   ND      Feature cleanup.
09/29/11   SA      Added support for TD-SCDMA.
09/23/11   TTV     Fixed an issue of leaving critical section correctly.
08/20/11   TTV     GSDI to MMGSDI_SESSION api migration changes.
07/11/11   TTV     Fixed klocwork errors.
05/26/11   DVK     Global Variables Cleanup
04/05/11   SS      Q6 free floating support.
02/28/11   TTV     Made changes to init watermarks before changing the sio 
                   mode to rawdata.
02/24/11   TTV     Fixed compilation warnings.
01/06/11   TTV     Changes made to start GCSD general purpose timer after call
                   origination.
01/06/11   SA      Fix to set the correct SIO packet size for GSM async call.
11/18/10   TTV     Added support for DSDS.
05/28/10   SA      Removing feature wrap FEATURE_CM_DISABLE_CALL_TYPE.
02/15/10   SA      Added ds_ucsd_is_atcop_call().
10/29/09   SA      Assigning L2 watermark counts equivalent to QVP for Video call.
10/09/09   SA      Fixed CBST values checking.
09/29/09   SA      Fixed KW errors.
09/11/09   SA      Updated GERAN header file inclusion.
07/30/09   SA      Fixed UL data transfer issue after OOS for a VT call.
07/10/09   SA      Fixed MT side Voice to VT rejection scenario of SCUDIF call.
05/25/09   SA      Added support to reject VT calls based on NV_DISABLE_CM_CALL_TYPE_I.
07/09/09   BS      Fixed offtarget lint medium and low's.
05/25/09   SA      Added support to reject VT calls based on NV_DISABLE_CM_CALL_TYPE_I.
07/01/09   SA      Fixed compilation issues for 1x+GSM only builds.
03/07/09   MS      CMI Integration fixes.
03/04/09   SA      AU level CMI modifications.
01/06/09   SA      Fixed VT call ending in GSM mode if SCUDIF feature is enabled.
12/23/08   SA      Replaced q_init with dsm_queue_init for watermark initialization.
12/15/08   SA      Fixed allowing of MT CSD call with no BC present.
10/31/08   SA      Fixed re-configuring of SMD port during FAX call.
09/01/08   SA      Added modifications for CMI defeaturization.
08/25/08   SA      Updated the speech preferences in set-up response 
                   for voice bearer in a SCUDIF call.
04/01/08   SA      Moved ds_ucsd_find_call_instance() to dsucsdhdlr.h
03/15/08   AR      Add ATCOP WAIUR value to ds_ucsd_waiur_mapping_tbl.
03/13/08   SA      Fixing Klocwork issues.
02/06/08   SA      Fixing PDP drop when incoming CSD call is terminated by remote user.
11/20/07   AR      Permit multimedia call in GSM per 3GPP TS24.008 CR962.
11/19/07   AR      Use SIO_IOCTL_SET_PACKET_SIZE for HSUSB driver on fax calls.
10/22/07   AR      Use macro for API interface configuration; partition modify handler.
10/09/07   AR      Replace call end with call confirmed notification on MO fallback.
                   Use SMD_DATA11 for VT calls with SIO datapath interface.
09/28/07   AR      Accomodate ATCOP changes on dsat_report_rate_on_connect().
09/27/07   AR      Move datapath teardown in call ended handler for all modes.
09/12/07   AR      Add ds_ucsd_perform_atcop_registration() for ATCOP interface.
08/22/07   AR      Add support for multiple UCSD API clients & call types.
08/08/07   DA      Reinit gcsd_s30_timer_val on returning to online data mode
07/27/07   AR      Use alerting state within DTR sig hndlr if no port assigned. 
06/06/07   AR      Use correct CM call ID for USER_USER_DATA events.
14/05/07   DA      Added support when FEATURE_DATA_GCSD_FAX is not defined
04/05/07   DA      Incoming CSD call is rejected when unsupported ITC in BCIE
03/29/07   AR      Increase ds_ucsd_to_l2_wm.dont_exceed_cnt for HS-USB.
19/03/06   DA      NV_DS_UCSD_RLP_ADM_IDLE_I is now unused when gcf flag(nv 947) 
                   is set.
02/26/07   AR      Replace banned APIs with safer routines.
02/26/07   DA      Changed return value of dswcsd_config_to_sio_wm() to boolean
02/16/07   AR      Specify call direction on SIOLIB port allocation.
02/06/07   AR      Migrate from SIO to SMD interface.
01/31/07   AR      Prevent SCUDIF call with swapped bearers until fallback supported.
12/14/06   AR      Reject incoming call if no client registered to handle it.
11/20/06   AR      Decouple the datapath from API call control. 
11/15/06   AR      Ensure call connected before resetting dpath on call end.
10/17/06   AR      Make progress event handler consistent with modify handler
                   Suppress RLC deregistration on VT->voice service change.
10/10/06   AR      Set SIO buffer limit for fax calls to reduce latency.
09/13/06   AR      Fix CSD API for SCUDIF integration with QVP.
09/05/06   AR      Enhance incoming call repeat indicator validation.
08/22/06   AR      Fix API interface for SCUDIF enabled build. 
08/21/06   AR      Changes to support multi-processor architectures.
07/26/06   AR      Move MT call SIOLIB registration from incoming to
                   connected handler for multi-port target constraint.
07/22/06   AR      Signal lower task earlier on network call release.
06/21/06   AR      Fix outgoing setup BC reporting to use negotiated BC.
06/12/06   AR      Directly map instance ID to call table array element.
06/02/06   AR      Deregister ATCOP handlers in end handler only if call not 
                   in NULL state; cleanup SCUDIF code.
05/23/06   AR      Create call instance at SETUP event rather than INCOMING.
03/22/06   AR      Ensure call switch does not cleanup lower-layers twice.
03/20/06   AR      Conditionally reset SIOLIB on call end based on state.
03/09/06   AR      Add CM call ID to GCSD commands.
02/27/06   AR      Use FNUR_UNDEFINED for autobaud default.
02/22/06   AR      Reject call setup with alternate mode if not supported.
02/15/06   AR      Merge in fixes for SCUDIF from patch branch.
02/09/06   AR      Ensure stack cleaned up if DSMgr origination fails.
10/26/05   AR      Add support for multiple call instances; Lint corrections.
10/21/05   AR      Replace TASKLOCK/TASKFREE with critical section.
09/27/05   AR      Fixed async G->W HO processing to kickstart RLP.
09/12/05   AR      Remove unnecessary code with bad pointer dereferences.
09/07/05   AR      Set ATCOP call response mode handling for fax calls.
08/26/05   AR      Added support for CS Data incall modification & fallback.
08/01/05   AR      Add check for valid call instance on call end handling.
06/16/05   AR      Add support for asynchronous call setup response. 
05/20/05   AR      Added ds_ucsd_call_progress_handler() routine to pass 
                   event to client interface.
04/28/05   AR      Add support for exported ATCOP +CV120 parameters.
04/26/05   AR      Add rlp_connect_state NV item for CONNECT result timing.
12/17/04   AR      Add explicit flag for use of LLC in SETUP message.
11/12/04   dwp     Add support for VT two stage dialing via User to User IE.
11/11/04   AR      Ensure FNUR and WAIUR set for V.34 9.6kpbs configuration.
11/03/04   dwp     Add support for sending reject cause for incoming call.
10/25/04   AR      Add support for RDI and FTM on asynch NT calls. Remove
                   improper V.110 56kbps case.  Generalize +CHSN setting.
10/19/04   AR      Add rlp_adm_idle timer parameter stored in NVRAM.
10/14/04   AR      Ensure SIO port allocated for external data call.
10/08/04   AR      Avoid duplicate system mode change processing.
09/20/04   AR      Use call_details bearer capability structure for call
                   confirmation handler.
09/08/04   AR      Send TE result code after SIO WM switch on call end.
                   Fix RLP compression state reporting.
08/10/04   AR      Include LLC user rate in +CBST translation table.
08/05/04   AR      Added generation of BUSY/NOANSWER call end return codes.
07/16/04   AR      Added supplementary services dial modifer to CSD API.
07/02/04   TMR     Make sure we always do ds_ucsd_get_rlp_params() call in
                   ds_ucsd_setup_handler()
06/07/04   AR      Query RLP recovery mode from NV item.
06/03/04   AR      Prevent VT app autoanswer when sync test app enabled.
05/28/04   AR      Accomodate dswcsd_init_call_info interface change.
05/24/04   AR      Added sys_change_handler functionality to handle Inter-RAT
                   handoff notifications
05/03/04   AR      Set GSM autobaud FNUR to more pervasive 9600bps support.
04/19/04   AR      Add WAIUR indication to CONNECT message on TE.
04/06/04   AR      Set default RLP version based on NV lookup.
04/06/04   TMR     Added clock speed changing for FAX calls
03/24/04   AR      Relocate ds_ucsd_call_tbl to common header file.
03/15/04   srk     Added reject reason and cc_cause container 
                   structure pointer as a parameter to the End 
                   call back.
12/14/03   TMR     Fixed ds_ucsd_map_fnur_waiur_to_ota_rate() to default to 
                   9.6 for FNUR_UNDEFINED and WAIUR_UNDEFINED
12/02/03   TMR     Fixing +CBST speed value of 17 to be FNUR_33600, not 
                   FNUR_38400.
11/18/03   TMR     Added dsm_empty_queue() function and +CBST speed value 17
11/18/03   ar      Add support for inter-RAT handover for async NT calls.
                   Lint corrections.
08/21/03   TMR     Added in changes for +CHSN and multiple +CBST parameters
08/13/03   dwp     Added support for call confirmation call back for VT.
07/22/03   TMR     Added fixes from Nortel WCDMA Async IOT
06/20/03   dgy     Added support for UMTS CS Data API and call table.
05/23/03   TMR     Added parameter to ds_gcsd_init_call_data() and
                   calls to dsfps_call_connect_init() and 
                   dsfps_powerup_wm_init() to fix FAX problems
05/22/03   dgy     Fixed a compiler warning.
05/21/03   dwp     Add read of ds_ucsd_sync_test_app_enabled in incoming hdlr
04/28/03   dgy     Corrected F3 messages.
04/25/03   TMR     Added dsgcsdl1if.h back in
04/25/03   dgy     Removed dsgcsdl1if.h.
04/25/03   dgy     Merged changes from dswcsdhdlr.c#16.
03/25/03   dgy     Initial version. Merged GSM and WCDMA CS Data mode-
                   specific handlers. Added support for WCDMA Asynchronous
                   CS Data.

===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "ds_3gppi_utils.h"


#include <stringl/stringl.h>
#include "ds_system_heap.h"

#ifdef FEATURE_TDSCDMA
#ifndef FEATURE_SEGMENT_LOADING
//#include "tdsrrcdata.h"
#include "tdsrrcdataext.h"
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_TDSCDMA */

#ifdef FEATURE_DATA_WCDMA_CS
#include "rex.h"
#include "task.h"
#include "wcdmamvsif.h"
#include "rrcdata.h"
#include "dswcsd.h"
#include "dswcsdul.h"
#include "dswcsdultask_v.h"
#include "dswcsddltask_v.h"
#include "dswcsdmshif.h"
#include "dswcsdntrlcif.h"
#include "ds3gcfgmgr.h"
#endif /* FEATURE_DATA_WCDMA_CS */

#ifdef FEATURE_DATA_GCSD_FAX
#include "dsfps.h"
#include "dst31i.h"

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
#include "cause.h"
#include "cm.h"
#include "dsm.h"
#include "dstaski.h"
#include "dsat_v.h"       /* ATCOP values */
#include "dsati.h"
#include "ds3gsiolib.h"
#include "ds3gsiolib_ex.h"
#include "ds3gmgr.h"
#include "ds3gmshif.h"
#include "dsgcsdi.h"
#include "dsgcsd_task_v.h"
#include "dsgcsdl1if.h"
#include "dsucsdi.h"
#include "dsucsdnt.h"
#include "dsucsdappifi.h"
#include "dsucsdbc.h"
#include "dsucsdmpshim.h"
#include "dsucsdhdlr.h"
#include "l1_ds.h"
#include "l1_ds_v.h"
#include "mn_cm_exp.h"

#ifdef FEATURE_DUAL_SIM
#include "sys.h"
#endif /*FEATURE_DUAL_SIM*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* DS_UCSD_HANDOVER_CMD comamnd type */
typedef struct
{
  sync_enum_T    reason;                  /* Handover direction */
  uint32         rab_id;                  /* RAB identifier */
} ds_ucsd_handover_cmd_type;

#ifdef FEATURE_SEGMENT_LOADING
/*---------------------------------------------------------------------------
TDSCDMA and WCDMA interface tables
---------------------------------------------------------------------------*/
interface_t *ds_3gpp_csd_tds_tbl   = NULL;
interface_t *ds_3gpp_csd_wcdma_tbl = NULL;
#endif /* FEATURE_SEGMENT_LOADING  */

uint32 ds_ucsd_to_l2_wm_lo_wm  = SDU_SIZE_FOR_64K * 10;
uint32 ds_ucsd_to_l2_wm_hi_wm  = SDU_SIZE_FOR_64K * 20;
uint32 ds_ucsd_to_l2_wm_dne_wm = SDU_SIZE_FOR_64K * 700;

/*===========================================================================

                      LOCAL DATA DECLARATIONS

===========================================================================*/

#define INVALID_INST_ID          MAX_UCSD_CALL_TBL_ENTRIES

#ifdef FEATURE_DATA_V80
/*---------------------------------------------------------------------------
  Indices into AT+ES information
---------------------------------------------------------------------------*/
#define ES_ORIG_RQST_INDEX      0
/* #define ES_ORIG_FBK_INDEX       1 */
#define ES_ANS_FBK_INDEX        2

#endif /* FEATURE_DATA_V80 */

/*---------------------------------------------------------------------------
  UCSD Call Instance elements
---------------------------------------------------------------------------*/

/* UMTS CS Data call table. Each entry contains information 
** about a call.
*/
static ds_ucsd_call_tbl_entry_type  *ds_ucsd_call_tbl_ptr[MAX_UCSD_CALL_TBL_ENTRIES] = {NULL};

/* Call Manager callID to UMTS CS Data call instance table.  Provides
** direct lookup for CM event handlers.
*/
static cm_call_id_type  ds_ucsd_cm_callid_to_inst_tbl[ CM_CALL_ID_MAX ] = {0};

/* Concurrent CS Data call limit table.  Note the access stratum
** layers must support multiple CS RABs in order to truely support
** concurrent calls.  Also constrained by orthogonal datapaths for
** active calls. Internal video telephony application requires
** multiple call instances to permit prompting user to select one when
** second call is incoming; user picks one to continue.
*/
static uint8 ds_ucsd_call_limit_info[DS_UCSD_CALL_INDEX_MAX] =
{
  1,  /* Asynchronous call type    */
  1,  /* Synchronous call type     */
  1,  /* Fax call type                 */
  2,  /* Video Telephony call type */
  0,  /* Voice call type (placeholder) */
};

/* Declare a task critical section for serialized code execution. Used
 * in place of TASKLOCK()/TASKFREE(). */
static rex_crit_sect_type ds_ucsd_instance_crit_sect = {{NULL}};

/* Marco to initialize UCSD call instance */
#define DSUCSD_INIT_CALL_INSTANCE_PTR( call_ptr )                           \
    {                                                        \
      memset( (void*)call_ptr, 0x0, sizeof(ds_ucsd_call_tbl_entry_type) );  \
      call_ptr->inst_id     = INVALID_INST_ID;                              \
      call_ptr->cm_call_id  = CM_CALL_ID_INVALID;                           \
      call_ptr->client_id   = DSUCSD_INVALID_CLIENT_ID;                     \
      call_ptr->call_type   = DS_UCSD_UNKNOWN_CALL;                         \
      call_ptr->sio_port_id = DS3G_SIOLIB_PORT_NONE;                      \
    }

#define DSUCSD_INIT_CALL_INSTANCE( index ) \
    DSUCSD_INIT_CALL_INSTANCE_PTR( (ds_ucsd_call_tbl_ptr[ index ]) ); 

/* Marco to register OTA data rate with ATCOP */
#define REPORT_RATE_ON_CONNECT(ota_rate,speed,sync,ce)  \
   {                                                    \
     dsat_connect_rate_s_type  connect_rate = {0};            \
     connect_rate.call_type = DSAT_CALL_TYPE_CS;        \
     connect_rate.rate.cs_call.waiur = ota_rate;        \
     connect_rate.rate.cs_call.fnur = speed;            \
     connect_rate.rate.cs_call.name = sync;             \
     connect_rate.rate.cs_call.conn_elem = ce;          \
     dsat_report_rate_on_connect( connect_rate );       \
   }

/* Marco to assign API control and datapath interfaces. */
#if defined(FEATURE_MULTIPROCESSOR) && defined(IMAGE_MODEM_PROC)
/* Assign SIO datapath routing.*/
#define SET_API_INTERFACES(info_ptr)               \
    info_ptr->call_ctrl_iface = DS_UCSD_IFACE_API; \
    info_ptr->datapath_iface  = DS_UCSD_IFACE_SIO; \
    MSG_MED("UCSD Routing: call_ctrl=API, datapath=SIO",0,0,0);
#else
/* Assign API datapath routing.*/
#define SET_API_INTERFACES(info_ptr)               \
    info_ptr->call_ctrl_iface = DS_UCSD_IFACE_API; \
    info_ptr->datapath_iface  = DS_UCSD_IFACE_API; \
    MSG_MED("UCSD Routing: call_ctrl=API, datapath=API",0,0,0);
#endif  /* FEATURE_MULTIPROCESSOR && IMAGE_MODEM_PROC */


LOCAL boolean ds_ucsd_release_call_instance
(
  uint8    inst_id                               /* CSData call instance ID */
);

LOCAL uint8 ds_ucsd_find_call_instance_by_port
(
  ds3g_siolib_port_e_type   port_id                    /* SIOLIB port ID */
);

LOCAL uint8 ds_ucsd_find_call_instance_by_type
(
  ds_ucsd_call_type  call_type                            /* CSD call type */
);

LOCAL boolean ds_ucsd_force_call_down_handler
(
   cm_call_id_type         exist_call_id,
   cm_end_params_s_type   *end_params,
   sys_modem_as_id_e_type  subs_id
);

LOCAL ds3g_incom_e_type 
ds_ucsd_switch_call_handler
(
   ds_cmd_type            *cmd_ptr,
   cm_call_id_type         inc_call_id,
   cm_call_id_type         exist_call_id,
   cm_ans_params_s_type   *ans_params,
   cm_end_params_s_type   *end_params_orig,
   cm_end_params_s_type   *end_params_inc,
   sys_modem_as_id_e_type  subs_id
);

/* TODO: Remove?? */
#if 0
LOCAL boolean ds_ucsd_get_call_connection_info_handler
(
   cm_call_id_type        call_id,
   uint8                 *system_information, 
   uint8                  max_system_information_length
);
#endif

LOCAL boolean ds_ucsd_get_data_counters_handler
(
   cm_call_id_type        call_id,
   uint32                *rx_from_network_byte_cnt, 
   uint32                *tx_to_network_byte_cnt
);

/* TODO: Remove?? */
#if 0
LOCAL boolean ds_ucsd_get_last_session_data_counters_handler
(
   uint32                *rx_from_network_byte_cnt,
   uint32                *tx_to_network_byte_cnt
);
#endif

LOCAL uint8 ds_ucsd_find_call_instance_by_state
(
   uint8                  call_state             /* CSD call state */
);

LOCAL void ds_ucsd_decode_v120_parameters
(
  const uint32            bit_array,             /* Parameter values */
  ds_ucsd_v120_param_T   *v120_info_ptr          /* Output structure */
);

LOCAL void ds_ucsd_store_bearer_service_type
(
  ds_ucsd_bearer_service_type *service_ptr,      /* Bearer service configuration  */  
   uint8                  speed,                 /* Speed value                   */ 
   uint8                  name,                  /* Synchronous or Asynchronous   */ 
   uint8                  ce,                    /* Transparent or Non-Transparent*/
   uint8                  waiur                  /* Wanted air interface rate     */
);

LOCAL void ds_ucsd_store_call_info
(
   uint8                  tbl_idx,               /* Call table index              */
   sys_sys_mode_e_type    network_mode,          /* WCDMA or GSM                  */
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type subs_id,               /* Subscription id               */
#endif /* FEATURE_DUAL_SIM */
   ds_ucsd_call_type      call_type,             /* Type of call                  */
   const ds_ucsd_bearer_service_type *service_ptr,
                                                 /* Pointer to bearer service info*/
   dsm_watermark_type    *ul_wm_ptr,             /* Pointer to UL L2 WM           */
   dsm_watermark_type    *dl_wm_ptr,             /* Pointer to DL L2 WM           */
   ds_ucsd_rlc_dl_rx_cb_type rlc_dl_rx_cb        /* Function called by RLC        **
                                                 ** when RLC receives DL data.    **
                                                 ** This callback can be NULL     */
);

LOCAL ds_ucsd_data_rate_T ds_ucsd_map_fnur_waiur_to_ota_rate
( 
   uint8                   ce,                   /* Transparent or Non-Transparent */
   sys_sys_mode_e_type     network_mode,         /* GSM or WCDMA                   */
   uint8                   fnur,                 /* Fixed Network User Rate        */
   uint8                   waiur                 /* Wanted Air User Interface Rate */
);

LOCAL boolean ds_ucsd_get_incoming_internal_csd_call_enable_status
( 
  sys_modem_as_id_e_type        subs_id
);

LOCAL void ds_ucsd_get_rlp_params
(
   ds_ucsd_nt_rlp_parameters_T *rlp_params,
   sys_modem_as_id_e_type        subs_id
);

LOCAL void ds_ucsd_get_rlp_params_from_nv
(
  ds_ucsd_nt_rlp_parameters_T *rlp_params,
  sys_modem_as_id_e_type        subs_id
);

LOCAL boolean ds_ucsd_map_chsn_to_bcie
(
  const uint8              chsn_val,
  uint8                   *waiur_ptr  
);

LOCAL void
ds_ucsd_hdlr_init_orig_dial_mod_params
(
  const ds_ucsd_call_tbl_entry_type    *call_info_ptr,
  cm_gw_cs_orig_params_s_type          *gw_cs_orig_params
);

#ifdef FEATURE_UUS
LOCAL void
ds_ucsd_hdlr_init_orig_uus_info
(
  const ds_ucsd_call_tbl_entry_type    *call_info_ptr,
  cm_gw_cs_orig_params_s_type          *gw_cs_orig_params
);
#endif /* FEATURE_UUS */

LOCAL void ds_ucsd_init_sio_watermarks( void );

LOCAL void ds_ucsd_init_l2_watermarks ( void );

LOCAL void ds_ucsd_configure_l2_watermarks
(
  ds_ucsd_call_tbl_entry_type    *call_info_ptr          /* Call instance  */
);

#ifdef FEATURE_DATA_WCDMA_CS
LOCAL boolean ds_ucsd_connect_l2_interface
(
  ds_ucsd_call_tbl_entry_type        *call_info_ptr,
  const cm_bearer_capability_s_type  *data_bc_ptr,
  const uint32                        rab_id,
  const boolean                       is_handover
);

LOCAL boolean ds_ucsd_deregister_l2_interface 
(
  sys_modem_as_id_e_type subs_id 
);

#endif /* FEATURE_DATA_WCDMA_CS*/

LOCAL boolean ds_ucsd_check_concurrent_calls
(
  ds_ucsd_call_type             new_call_type,   /* Call type for new call */
  ds_ucsd_call_dir_e_type       direction,       /* Call direction         */
  ds_ucsd_call_tbl_entry_type **switch_inst_pptr /* Instance to switch     */
);

LOCAL boolean ds_ucsd_allocate_call_instance
(
  const cm_call_id_type              call_id,          /* CM call ID       */
  const cm_bearer_capability_s_type *bc_ptr,
                            /* Bearer Capability for preferred mode of call */
  const cm_gw_cs_call_info_s_type   *mode_info_ptr,    /* New mode info    */
  ds_ucsd_call_tbl_entry_type ** call_info_pptr
                                       /* Returned pointer to call instance */
);

LOCAL void ds_ucsd_sys_change_handler
(
 sys_modem_as_id_e_type              subs_id,
 sys_sys_mode_e_type                 new_mode,
 sys_sys_mode_e_type                 old_mode
);

LOCAL boolean ds_ucsd_get_GSM_channel_info
(
    ds_ucsd_call_tbl_entry_type        *call_info_ptr
);

#ifdef FEATURE_DATA_UCSD_HANDOVER
LOCAL boolean ds_ucsd_nt_handover_update_stack_interface
(
  ds_ucsd_call_tbl_entry_type        *call_info_ptr,
  const ds_cmd_type                  *cmd_ptr,
  const cm_bearer_capability_s_type  *data_bc_ptr
);

LOCAL boolean ds_ucsd_process_handover_cmd
( 
   const ds_cmd_type  *cmd_ptr
);

#endif /* FEATURE_DATA_UCSD_HANDOVER */

LOCAL void ds_ucsd_process_app_originate_call_cmd
( 
   const ds_cmd_type  *cmd_ptr
);

LOCAL void ds_ucsd_process_app_confirm_call_cmd
( 
   ds_cmd_type  *cmd_ptr
);

LOCAL void ds_ucsd_process_app_answer_call_cmd
( 
   const ds_cmd_type  *cmd_ptr
);

#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */


#if defined (FEATURE_3GPP_CSFB) && defined (FEATURE_DATA_LTE)
LOCAL void ds_ucsd_cntxt_transfer_ind_handler
(
  sys_sys_mode_e_type      sys_mode,
  sys_modem_as_id_e_type   subs_id
);
#endif /* FEATURE_3GPP_CSFB && FEATURE_DATA_LTE */

LOCAL void ds_ucsd_process_app_end_call_cmd
( 
   const ds_cmd_type  *cmd_ptr
);

LOCAL void ds_ucsd_process_uus_data_cmd
( 
   const ds_cmd_type  *cmd_ptr
);

LOCAL void ds_ucsd_process_uus_data_cong_cmd
( 
   const ds_cmd_type  *cmd_ptr
);

LOCAL boolean ds_ucsd_call_end_common_cleanup
(
  uint8 inst_id
);

LOCAL void ds_ucsd_call_hangup_cb (void);

LOCAL dsat_call_abort_cb_type ds_ucsd_call_answer_cb (void);

LOCAL boolean ds_ucsd_connect_data_path
(
  const uint32                        rab_id,          /* RAB Identifier */
  const cm_bearer_capability_s_type  *data_bc_ptr,     /* Bearer info    */
  ds_ucsd_call_tbl_entry_type        *call_info_ptr    /* Call instance  */
);

LOCAL boolean ds_ucsd_disconnect_data_path
(
  const cm_bearer_capability_s_type  *data_bc_ptr,     /* Bearer info    */
  ds_ucsd_call_tbl_entry_type        *call_info_ptr,   /* Call instance  */
  const boolean                       do_rlc_dereg     /* Dereg from RLC?*/
);

LOCAL boolean ds_ucsd_perform_siolib_registration
(
  ds_ucsd_call_tbl_entry_type              *call_info_ptr
);

LOCAL boolean ds_ucsd_perform_atcop_registration
(
  const ds_ucsd_call_tbl_entry_type *call_info_ptr,
  cm_end_params_s_type              *end_params_ptr
);

#ifdef FEATURE_DATA_GCSD_FAX
LOCAL boolean ds_ucsd_perform_siodriver_config
(
  boolean  is_call_connecting
);
#endif /* FEATURE_DATA_GCSD_FAX */

LOCAL void ds_ucsd_call_id_handler
(
   cm_call_id_type          call_id,
   void                    *mode_spec_info_ptr,
   sys_modem_as_id_e_type   subs_id
);

LOCAL boolean ds_ucsd_call_conf_handler
(
   cm_call_id_type            call_id,
   cm_call_mode_info_s_type   *mode_info_ptr,
   cm_end_params_s_type       *end_params,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   sys_modem_as_id_e_type      subs_id
);

LOCAL boolean ds_ucsd_call_progress_handler
(
   cm_call_id_type             call_id,
   cm_call_mode_info_s_type   *mode_info_ptr,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   sys_modem_as_id_e_type      subs_id
);

LOCAL boolean ds_ucsd_call_connected_handler
(
   cm_call_id_type           call_id,
   cm_call_mode_info_s_type  *mode_info_ptr,
   boolean                   *delay_connect,
   cm_end_params_s_type      *end_params,
   sys_modem_as_id_e_type     subs_id
);

LOCAL boolean ds_ucsd_complete_ll_connect_handler
(
   cm_call_id_type         call_id,
   cm_end_params_s_type    *end_params,
   sys_modem_as_id_e_type  subs_id
);

LOCAL boolean ds_ucsd_setup_handler
(
   cm_call_id_type               call_id,
   cm_call_mode_info_s_type     *mode_info_ptr,
   cm_setup_res_params_s_type   *setup_res_params,
   sys_sys_mode_e_type           call_mode,
   boolean                      *delay_rsp,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   sys_modem_as_id_e_type        subs_id
);

LOCAL void ds_ucsd_user_end_handler
( 
   cm_call_id_type             call_id,
   cm_end_params_s_type       *end_params,
   boolean                    *delay_disconnect,
   void                       *user_info_ptr,
   sys_modem_as_id_e_type      subs_id 
);

LOCAL void ds_ucsd_complete_ll_disconnect_handler
(
   cm_call_id_type        call_id,
   cm_end_params_s_type  *end_params,
   sys_modem_as_id_e_type subs_id
);

LOCAL void ds_ucsd_call_ended_handler
( 
   ds_cmd_type               *cmd_ptr,
   boolean                    call_was_connected,
   sys_modem_as_id_e_type     subs_id
);

LOCAL void ds_ucsd_user_answer_handler
(
   cm_call_id_type         call_id,
   cm_ans_params_s_type   *ans_params,
   ie_cm_cc_cause_s_type  *cause_ptr,
   sys_modem_as_id_e_type  subs_id
);

LOCAL boolean ds_ucsd_prepare_call_origination
(
  ds_ucsd_call_tbl_entry_type       *call_info_ptr,
  const cm_bearer_capability_s_type *data_bc_ptr
);

LOCAL boolean ds_ucsd_originate_call_handler
(
   const uint8                    *dial_string,
   uint8                           dial_string_len,
   cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
   boolean                        *cdma_params_changed,
   cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
   boolean                        *gw_cs_params_changed,
   cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
   boolean                        *gw_ps_params_changed,
   cm_srv_type_e_type             *cm_srv_type,
   void                           *mode_spec_info_ptr,
   sys_modem_as_id_e_type          subs_id
);

LOCAL boolean ds_ucsd_decode_cbst_values
(
  const cm_bearer_capability_s_type  *data_bc_ptr,
  uint8 *speed_ptr,
  uint8 *name_ptr,
  uint8 *ce_ptr
);

LOCAL ds3g_incom_e_type ds_ucsd_incoming_call_handler
(
  ds_cmd_type            *cmd_ptr,
  cm_call_id_type         call_id,
  cm_ans_params_s_type   *ans_params,
  cm_end_params_s_type   *end_params,
  sys_modem_as_id_e_type  subs_id
);

/* Mode-specific handler function table */
LOCAL ds3g_hdlr_func_tbl_type       ds_ucsd_hdlr_func_tbl;

/* Current network mode */
static sys_sys_mode_e_type                 ds_ucsd_network_mode;

#ifdef FEATURE_3GPP_CSFB
/* Internal Mode if, used when current mode is LTE */
sys_sys_mode_e_type      ds_ucsd_internal_mode = SYS_SYS_MODE_WCDMA;
#endif /* FEATURE_3GPP_CSFB */

#ifdef FEATURE_DUAL_SIM
/* Current Subs id  */
sys_modem_as_id_e_type      ds_ucsd_subs_id;
#endif /* FEATURE_DUAL_SIM */

/* Data counters for last call */
static ds_ucsd_last_call_info_type ds_ucsd_last_call_info;

/* Flag for reading from NV whether the Sync Test App is enabled */
static boolean      ds_ucsd_sync_test_app_enabled = FALSE;

/* Flag for reading from NV whether the incoming call will be routed
 * to internal app via CSD API */
static boolean      ds_ucsd_incoming_internal_call_enabled = FALSE;

/*---------------------------------------------------------------------------
  DS3G SIOLIB interface elements
---------------------------------------------------------------------------*/

/* Forward function declarations for ds_ucsd_siolib_func_tbl */
dsat_mode_enum_type 
ds_ucsd_at_escape_sequence_handler( void );

void ds_ucsd_dtr_changed_sig_handler
( 
   boolean dtr_status 
);

void 
ds_ucsd_at_return_to_online_data_handler
(
  dsm_watermark_type     **from_sio_wm_ptr,
  dsm_watermark_type     **to_sio_wm_ptr,
  sio_rx_func_ptr_type    *mode_hdlr_sio_rx_func_ptr
);

void ds_ucsd_at_return_to_online_data_complete_handler ( void );

void ds_ucsd_return_to_online_cmd_mode_complete_handler( void );

void ds_ucsd_auto_answer_handler ( void );


LOCAL ds3g_siolib_func_tbl_type ds_ucsd_siolib_func_tbl =
{
   // at_escape_sequence_handler
   ds_ucsd_at_escape_sequence_handler,
   
   // dtr_changed_sig_handler 
   ds_ucsd_dtr_changed_sig_handler,
   
   // dtr_changed_sig_handler_ampd0 
   NULL,
   
   // at_return_to_online_data_handler 
   ds_ucsd_at_return_to_online_data_handler,
   
   // at_return_to_online_data_complete_handler 
   ds_ucsd_at_return_to_online_data_complete_handler,
   
   // return_to_online_cmd_mode_complete_handler                                        
   ds_ucsd_return_to_online_cmd_mode_complete_handler,
   
   // auto_answer_handler
   ds_ucsd_auto_answer_handler
};
   
/*===========================================================================
The following #defines and structure/table definitions are used by the
ds_ucsd_get_chosen_cap() function for translating ATCOP CBST values
to the chosen_capability structure.
===========================================================================*/

/* Positions within the ds_atcop_cbst_export_val[] array for the various
** CBST parameters 
*/
#define ATCOP_CBST_SPEED_POSITION  0
#define ATCOP_CBST_NAME_POSITION   1
#define ATCOP_CBST_CE_POSITION     2

/* Positions within the  dsat_crlp_val[][] array for the various
** CRLP parameters 
*/
#define ATCOP_CRLP_IWS_POSITION  0
#define ATCOP_CRLP_MWS_POSITION  1
#define ATCOP_CRLP_T1_POSITION   2
#define ATCOP_CRLP_N2_POSITION   3

/* Positions within dsat_crlp_val[][] array for different RLP versions */
#define ATCOP_CRLP_V0            0
#define ATCOP_CRLP_V1            1
#define ATCOP_CRLP_V2            2

/* Positions within the dsat_ds_val[] array for the various 
** +DS parameters
*/
#define ATCOP_DS_P0_POSITION        0
#define ATCOP_DS_COMP_NEG_POSITION  1
#define ATCOP_DS_P1_POSITION        2
#define ATCOP_DS_P2_POSITION        3

/* Positions within the dsat_chsn_val[] array for the various 
** +DS parameters
*/
#define ATCOP_CHSN_WAIUR_POSITION   0
/*
#define ATCOP_CHSN_WRX_POSITION     1
#define ATCOP_CHSN_TOPRX_POSITION   2
#define ATCOP_CHSN_CODINGS_POSITION 3
*/

/* CHSN WAIUR values */
#define ATCOP_CHSN_WAIUR_DEFAULT    0
#define ATCOP_CHSN_WAIUR_9600       1
#define ATCOP_CHSN_WAIUR_14400      2
#define ATCOP_CHSN_WAIUR_28800      4
/* #define ATCOP_CHSN_WAIUR_43200      6 */
#define ATCOP_CHSN_WAIUR_57600      7

/* ATCOP WAIUR Reporting on CONNECT */
#define ATCOP_WAIUR_REPORT_9600     9600
#define ATCOP_WAIUR_REPORT_14400    14400
#define ATCOP_WAIUR_REPORT_28800    28800
#define ATCOP_WAIUR_REPORT_57600    57600
#define ATCOP_WAIUR_REPORT_64000    64000

/* ATCOP +CBST name values */
#define ATCOP_ASYNC_UDI_DATA            0  /* UDI or 3.1kHz */
#define ATCOP_SYNC_UDI_DATA             1  /* UDI or 3.1kHz */
#define ATCOP_ASYNC_RDI_DATA            4
#define ATCOP_SYNC_RDI_DATA             5

/* ATCOP +CBST Connection Element values */
#define ATCOP_TRANSPARENT_DATA      0
#define ATCOP_NON_TRANSPARENT_DATA  1


/* ATCOP +CV120 parameter value masks */
#define ATCOP_V120_RAH_MASK      ( 1 << (int)DSAT_CV120_RAH_POS )
#define ATCOP_V120_MFM_MASK      ( 1 << (int)DSAT_CV120_MFM_POS )
#define ATCOP_V120_MODE_MASK     ( 1 << (int)DSAT_CV120_MODE_POS )
#define ATCOP_V120_LLINEG_MASK   ( 1 << (int)DSAT_CV120_LLINEG_POS )
#define ATCOP_V120_ASSIGN_MASK   ( 1 << (int)DSAT_CV120_ASSIGN_POS )
#define ATCOP_V120_NEGTYPE_MASK  ( 1 << (int)DSAT_CV120_NEGTYPE_POS )


/* Modes that the +CBST Speed parameters apply to */
/* Note: All modes applicable to WCDMA are 
   applicable to TD_SCDMA */
#define MODE_GSM        0x01
#define MODE_WCDMA      0x02
#define MODE_GSM_WCDMA  (MODE_GSM | MODE_WCDMA)

#define MODE_SYNC       0x04
#define MODE_ASYNC      0x08
/* #define MODE_SYNC_ASYNC (MODE_SYNC | MODE_ASYNC) */

#define MODE_T_DATA     0x10
#define MODE_NT_DATA    0x20
#define MODE_T_NT_DATA  (MODE_T_DATA | MODE_NT_DATA)

/* Default in seconds to remain in ADM idle state following RLP error */
#define RLP_ADM_IDLE_DEFAULT  0 

/* Default NT connection state at which to send CONNECT response */
#define RLP_CONNECT_STATE_DEFAULT  NT_CONNECT_STATE_L2R 

/* Define bit field values for supported ITC values */
#define ITC_BF_UDI    (1 << ITC_UDI)
#define ITC_BF_OTHER  (1 << ITC_OTHER)
#define ITC_BF_3_1K   (1 << ITC_3_1K)

/* When gcf flag (nv item 947) is set, rlp_adm_idle_timer is not set to 
   NV_DS_UCSD_RLP_ADM_IDLE_I. Instead, rlp_adm_idle_timer is set to 
   DS_UCSD_GCF_RLP_ADM_IDLE_TIMER which is 2 hours. This replacement is
   to support reducing the use of nv item for gcf purposes. */
#define DS_UCSD_GCF_RLP_ADM_IDLE_TIMER 7200

/* Speed table element. The order of the fields matters. 
** See AT_speed_table.
*/
typedef struct
{
   dsat_num_item_type  speed;
   uint8               mode;
   uint8               bc_fnur;         /* Fixed network user rate         */
   uint8               bc_waiur;        /* Wanted air interface user rate  */
   uint8               bc_modem_type;
   uint8               bc_other_modem_type;
   uint8               bc_itc;          /* Information transfer capability */
   uint8               bc_ra;           /* Rate adaption                   */
   uint8               bc_other_ra;     /* Other rate adaption             */
   uint8               llc_user_rate;   /* LLC user rate                   */
   boolean             llc_present;     /* LLC seeded into OTA msg?        */
} AT_speed_element_T;


/* Table used to convert AT CBST speed parameter to BC and LLC data
** Note that the speed values in this table must remain in ascending 
** order. WAIUR applies to Non-Transparent or "both T and NT" only. 
** The WAIUR column is applicable only when the +CHSN WAIUR parameter is 0.
*/ 
LOCAL const 
AT_speed_element_T AT_speed_table[] =
{

   /* Note that for AUTOBAUDING case, we will modify the FNUR and
   ** WAIUR values depending on what mode we are in, so ignore the
   ** values in this structure.
   */
   /* GSM 9600 have FNUR set to not applicable/undefined as some deployed
   ** networks had problem when FNUR_9600 specified.
   */
   {0,   MODE_GSM_WCDMA | MODE_ASYNC | MODE_NT_DATA,    
         FNUR_NOT_APPLICABLE, WAIUR_UNDEFINED,     AUTO_TYPE_1,  
         OMT_NONE,            ITC_BF_3_1K,         NO_RATE_ADAPTION,    
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_9600,      FALSE },
#ifdef FEATURE_DATA_GCSD 
   {7,   MODE_GSM       | MODE_ASYNC |  MODE_T_NT_DATA,  
         FNUR_UNDEFINED,      WAIUR_UNDEFINED,     V32,              
         OMT_NONE,            ITC_BF_3_1K,         NO_RATE_ADAPTION, 
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_9600,      FALSE },

   {12,  MODE_GSM       | MODE_ASYNC | MODE_NT_DATA,
         FNUR_9600,           WAIUR_9600,          V32,              
         OMT_V34,             ITC_BF_3_1K,         NO_RATE_ADAPTION, 
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_9600,      FALSE },
#endif /* FEATURE_DATA_GCSD */
   {14,  MODE_GSM_WCDMA | MODE_ASYNC | MODE_NT_DATA,
         FNUR_14400,          WAIUR_14400,         V32, 
         OMT_V34,             ITC_BF_3_1K,         NO_RATE_ADAPTION,    
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_14400,     FALSE },

   {16,  MODE_WCDMA     | MODE_ASYNC | MODE_NT_DATA,      
         FNUR_28800,          WAIUR_28800,         V32,  
         OMT_V34,             ITC_BF_3_1K,         NO_RATE_ADAPTION,    
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_28800,     FALSE }, 
    
   {17,  MODE_WCDMA     | MODE_ASYNC | MODE_NT_DATA,      
         FNUR_33600,          WAIUR_57600,         V32,  
         OMT_V34,             ITC_BF_3_1K,         NO_RATE_ADAPTION,    
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_32000,     FALSE }, 
#ifdef FEATURE_DATA_GCSD 
   {39,  MODE_GSM | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_UNDEFINED,      WAIUR_UNDEFINED,     NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          OTHER_RATE_ADAPTION,    
         V120,                                     LLC_9600,      TRUE }, 
#endif /* FEATURE_DATA_GCSD */
   {43,  MODE_GSM_WCDMA | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_14400,          WAIUR_14400,         NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          OTHER_RATE_ADAPTION,    
         V120,                                     LLC_14400,     TRUE }, 

   {48,  MODE_WCDMA     | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_28800,          WAIUR_28800,         NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          OTHER_RATE_ADAPTION,    
         V120,                                     LLC_28800,     TRUE }, 

   {51,  MODE_WCDMA     | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_56K ,           WAIUR_57600,         NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          OTHER_RATE_ADAPTION,    
         V120,                                     LLC_56000,     TRUE }, 
#ifdef FEATURE_DATA_GCSD 
   {71,  MODE_GSM       | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_UNDEFINED,      WAIUR_UNDEFINED,     NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          V110_X30,    
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_9600,      TRUE },
#endif /* FEATURE_DATA_GCSD */
   {75,  MODE_GSM_WCDMA | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_14400,          WAIUR_14400,         NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          V110_X30,   
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_14400,     TRUE },
   
   {80,  MODE_GSM_WCDMA | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_28800,          WAIUR_28800,         NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          V110_X30,   
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_28800,     TRUE },
   
   {81,  MODE_GSM_WCDMA | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_38400,          WAIUR_57600,         NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          V110_X30,   
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_38400,     TRUE },
   
   {83,  MODE_GSM_WCDMA | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_56K,            WAIUR_57600,         NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI|ITC_BF_OTHER,   X31_FLAGSTUFFING,   
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_56000,     TRUE }, 
   
   {84,  MODE_WCDMA | MODE_ASYNC | MODE_NT_DATA,  
         FNUR_64K,            WAIUR_57600,         NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          X31_FLAGSTUFFING,   
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_64000,     TRUE }, 

   /* Do not specify LLC for speed 116 configuration as field issues have 
    * been reported. */
   {116, MODE_WCDMA     | MODE_SYNC  | MODE_T_DATA,      
         FNUR_64K,            WAIUR_UNDEFINED,     NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          NO_RATE_ADAPTION,   
         OTHER_RATE_ADAPTION_NOT_APPLICABLE,       LLC_64000,     FALSE }, 
   
   {134, MODE_WCDMA     | MODE_SYNC  | MODE_T_DATA,      
         FNUR_64K,            WAIUR_UNDEFINED,     NO_MODEM,    
         OMT_NONE,            ITC_BF_UDI,          OTHER_RATE_ADAPTION,   
         H223_H245,                                LLC_64000,     TRUE } 
};

LOCAL const int AT_speed_table_size = sizeof (AT_speed_table) /
                                      sizeof (AT_speed_element_T); 

/* Name AT command table element */
typedef struct
{
   dsat_num_item_type name;
   uint8              bc_sync_async;
   uint8              llc_sync_async;
   uint8              bc_itc;
} AT_name_element_T;

LOCAL const AT_name_element_T AT_name_table[] =
{
   /* 
   name                    bc_sync_async     llc_sync_async       bc_itc
   */
  {ATCOP_ASYNC_UDI_DATA,   ASYNCHRONOUS,     LLC_ASYNCHRONOUS,    ITC_BF_UDI|ITC_BF_3_1K },
  {ATCOP_SYNC_UDI_DATA,    SYNCHRONOUS,      LLC_SYNCHRONOUS,     ITC_BF_UDI|ITC_BF_3_1K },
  {ATCOP_ASYNC_RDI_DATA,   ASYNCHRONOUS,     LLC_ASYNCHRONOUS,    ITC_BF_OTHER },
  {ATCOP_SYNC_RDI_DATA,    SYNCHRONOUS,      LLC_SYNCHRONOUS,     ITC_BF_OTHER }   
};

LOCAL const int AT_name_table_size = sizeof (AT_name_table) /
                                     sizeof (AT_name_element_T);
                                     
                                     
/* Connection Element AT command table element */
typedef struct
{
   dsat_num_item_type ce;
   uint8              connection_element;   
   
} AT_ce_element_T;

LOCAL const AT_ce_element_T AT_ce_table[] =
{
   /* 
    ce          connection_element 
   */
   {ATCOP_TRANSPARENT_DATA,              TRANSPARENT_DATA},
   {ATCOP_NON_TRANSPARENT_DATA,          NON_TRANSPARENT_DATA}
};

LOCAL const int AT_ce_table_size = sizeof (AT_ce_table) /
                                     sizeof (AT_ce_element_T);
           
/* Entry of the table that maps FNUR to ds_ucsd_data_rate_T */
typedef struct
{
   uint8                    fnur;               /* Fixed Network User Rate */
   ds_ucsd_data_rate_T      data_rate;          /* CS data rate            */
} ds_ucsd_fnur_mapping_tbl_entry_type;

/* Table that maps FNUR to ds_ucsd_data_rate_T */
LOCAL const ds_ucsd_fnur_mapping_tbl_entry_type ds_ucsd_fnur_mapping_tbl[] =
{
   /* 
    FNUR          Data Rate
   */ 
   {FNUR_9600,    DS_UCSD_RATE_9600},
   {FNUR_14400,   DS_UCSD_RATE_14400},
   {FNUR_28800,   DS_UCSD_RATE_28800},
   {FNUR_38400,   DS_UCSD_RATE_38400},
   {FNUR_48K,     DS_UCSD_RATE_48000},
   {FNUR_56K,     DS_UCSD_RATE_57600},
   {FNUR_64K,     DS_UCSD_RATE_64000}
};

LOCAL const int ds_ucsd_fnur_mapping_tbl_size = 
   sizeof(ds_ucsd_fnur_mapping_tbl) / 
   sizeof(ds_ucsd_fnur_mapping_tbl_entry_type);
           
/* Entry of the table that maps WAIUR to ds_ucsd_data_rate_T */
typedef struct
{
   uint8                    waiur;       /* Wanted Air Interface User Rate */
   uint8                    at_waiur;    /* ATCOP WAIUR value              */
   ds_ucsd_data_rate_T      data_rate;   /* CS data rate                   */
   uint8                    mode;        /* Network service supported      */
} ds_ucsd_waiur_mapping_tbl_entry_type;

/* Table that maps WAIUR to ds_ucsd_data_rate_T */
LOCAL const ds_ucsd_waiur_mapping_tbl_entry_type ds_ucsd_waiur_mapping_tbl[] =
{
   /* 
    WAIUR         ATCOP Value              Data Rate            Network Mode
   */
   {WAIUR_9600,   ATCOP_CHSN_WAIUR_9600,   DS_UCSD_RATE_9600,   MODE_GSM },
   {WAIUR_14400,  ATCOP_CHSN_WAIUR_14400,  DS_UCSD_RATE_14400,  MODE_GSM_WCDMA },
   {WAIUR_28800,  ATCOP_CHSN_WAIUR_28800,  DS_UCSD_RATE_28800,  MODE_WCDMA },
 //{WAIUR_43200,  ATCOP_CHSN_WAIUR_43200,  DS_UCSD_RATE_43200,  MODE_WCDMA },
   {WAIUR_57600,  ATCOP_CHSN_WAIUR_57600,  DS_UCSD_RATE_57600,  MODE_WCDMA }
};

LOCAL const int ds_ucsd_waiur_mapping_tbl_size = 
   sizeof(ds_ucsd_waiur_mapping_tbl) / 
   sizeof(ds_ucsd_waiur_mapping_tbl_entry_type);
           

/*---------------------------------------------------------------------------
  UCSD Call Data Path elements
---------------------------------------------------------------------------*/
           
/* Downlink SIO watermark */  
LOCAL q_type ds_ucsd_to_sio_q;
LOCAL dsm_watermark_type ds_ucsd_to_sio_wm;            /* Downlink SIO WM */

/* Uplink and Downlink L2 watermarks */  
LOCAL q_type ds_ucsd_from_l2_q;
LOCAL q_type ds_ucsd_to_l2_q;

LOCAL dsm_watermark_type ds_ucsd_from_l2_wm;           /* Downlink L2 WM  */
LOCAL dsm_watermark_type ds_ucsd_to_l2_wm;             /* Uplink L2 WM    */

LOCAL boolean ds_ucsd_perform_call_switch
(
  ds_ucsd_call_tbl_entry_type        *call_info_ptr,    /* Existing call   */
  const cm_gw_cs_call_info_s_type    *mode_info_ptr,    /* New mode info   */
  const cm_bearer_capability_s_type  *data_bc_ptr,      /* Data bearer     */
  const ds_ucsd_call_type             curr_call_type    /* Current type    */
);



/*===========================================================================

                          FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================


FUNCTION ds_ucsd_validate_cbst_values

DESCRIPTION
   This function validates the +CBST values against the speed, name,
   and connection element list supported within the CS protocol stack.

DEPENDENCIES
   None

RETURN VALUE
   TRUE if the parameter set is valid.  FALSE otherwise.

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_validate_cbst_values
(
   uint32 speed,
   uint32 name,
   uint32 ce
)
{
   register boolean rc    = TRUE;
   register uint32  i;

   MSG_HIGH ("CBST validate: SPEED=%d,NAME=%d,CE=%d",speed,name,ce);
   for( i = 0; i < (uint32)AT_speed_table_size; i++ )
   {
      if( (uint32)AT_speed_table[i].speed == speed )
      {
       /* Check for ITC RDI name against CBST name parameter */
       if ( ((ATCOP_ASYNC_RDI_DATA == name) ||
             (ATCOP_SYNC_RDI_DATA == name)) &&
            (ITC_BF_OTHER != (ITC_BF_OTHER & AT_speed_table[i].bc_itc)) )
       {
         DATA_MSG1_ERROR("+CBST: speed %d won't support RDI ITC",speed);
         rc = FALSE;
       }
      
         /* Check the sync/async value to see if it is supported
         ** for the current speed value
         */
       if ( (name == ATCOP_ASYNC_UDI_DATA) ||
            (name == ATCOP_ASYNC_RDI_DATA) )
       {
         if (!(AT_speed_table[i].mode & MODE_ASYNC))
         {
           DATA_MSG1_ERROR("+CBST: speed %d won't support ASYNC data",speed);
           rc = FALSE;
         }
       }
       else if ( (name == ATCOP_SYNC_UDI_DATA) ||
                 (name == ATCOP_SYNC_RDI_DATA) )
       {
         if (!(AT_speed_table[i].mode & MODE_SYNC))
         {
           DATA_MSG1_ERROR("+CBST: speed %d won't support SYNC data",speed);
           rc = FALSE;
         }
       }
       else
       {
         DATA_MSG1_ERROR("+CBST: unknown name (sync/async) setting",name);
         rc = FALSE;
       }

       /* Check the connection element (T/NT) value to see if it is supported
       ** for the current speed value
       */
       if (ce == ATCOP_NON_TRANSPARENT_DATA)
       {
         if (!(AT_speed_table[i].mode & MODE_NT_DATA))
         {
           DATA_MSG1_ERROR("+CBST: speed %d won't support NT data",speed);
           rc = FALSE;
         }
       }
       else if (ce == ATCOP_TRANSPARENT_DATA)
       {
         if (!(AT_speed_table[i].mode & MODE_T_DATA))
         {
          DATA_MSG1_ERROR("+CBST: speed %d won't support T data",speed);
           rc = FALSE;
         }
       }
       else
       {
         DATA_MSG1_ERROR("+CBST: unknown CE (T/NT) setting",ce);
         rc = FALSE;
       }

       /* Speed table entry found, exit loop */
       break;

      } /* AT_speed_table[i].speed == speed */
   } /* for loop */

   /* Make sure we found a setting in the table */
   if (i >= (uint32)AT_speed_table_size)
   {
      DATA_MSG1_ERROR("+CBST: speed value %d not supported",speed);
      rc = FALSE;
   }

   if ( (rc != FALSE) &&
       ( ((ce == ATCOP_TRANSPARENT_DATA) && (name == ATCOP_ASYNC_UDI_DATA))||
         ((ce == ATCOP_NON_TRANSPARENT_DATA) && (name == ATCOP_SYNC_UDI_DATA)) ) )
   {
     DATA_MSG2_ERROR("+CBST: name %d and ce %d is not supported",name ,ce);
     rc = FALSE;
   }

   return rc;
} /* ds_ucsd_validate_cbst_values() */   


/*===========================================================================


FUNCTION ds_ucsd_decode_cbst_values

DESCRIPTION
  This function decodes from the bearer service configuraiton the
  +CBST values for speed, name, and connection element list supported
  within the CS protocol stack.

DEPENDENCIES
   None

RETURN VALUE
   TRUE if the parameter set is valid.  FALSE otherwise.

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_decode_cbst_values
(
  const cm_bearer_capability_s_type  *data_bc_ptr,
  uint8 *speed_ptr,
  uint8 *name_ptr,
  uint8 *ce_ptr
)
{
  uint8 i;
  
  ASSERT( NULL != data_bc_ptr );
  ASSERT( NULL != speed_ptr );
  ASSERT( NULL != name_ptr );
  ASSERT( NULL != ce_ptr );

  /* Decode the connection element */
  *ce_ptr = (NON_TRANSPARENT_DATA == data_bc_ptr->connection_element)?
            ATCOP_NON_TRANSPARENT_DATA : ATCOP_TRANSPARENT_DATA;
  
  /* Decode the name (synch/asynch) */
  if( ASYNCHRONOUS == data_bc_ptr->sync_async )
  {
    *name_ptr = ((ITC_OTHER == data_bc_ptr->information_transfer_capability) &&
                 (OITC_RDI == data_bc_ptr->other_ITC))?
                ATCOP_ASYNC_RDI_DATA : ATCOP_ASYNC_UDI_DATA;
  }
  else
  {
    *name_ptr = ((ITC_OTHER == data_bc_ptr->information_transfer_capability) &&
                 (OITC_RDI == data_bc_ptr->other_ITC))?
                ATCOP_SYNC_RDI_DATA : ATCOP_SYNC_UDI_DATA;
  }

  /* Decode the speed */
  *speed_ptr = 0;
  for( i = 0; i < AT_speed_table_size; i++ )
  {
    uint8 bc_itc = (1 << data_bc_ptr->information_transfer_capability);
    uint8 bc_other_ra = (AT_speed_table[i].bc_other_ra ==
                         OTHER_RATE_ADAPTION_NOT_APPLICABLE)?
                        0 : AT_speed_table[i].bc_other_ra;

    if( ((AT_speed_table[i].bc_itc & bc_itc) != 0) &&
         (AT_speed_table[i].bc_fnur ==  data_bc_ptr->fixed_network_user_rate) &&
         (AT_speed_table[i].bc_ra == data_bc_ptr->rate_adaption) &&
         (bc_other_ra == data_bc_ptr->other_rate_adaption) )
    {
      *speed_ptr = AT_speed_table[i].speed;
      break;
    }
  }

  MSG_HIGH ("CBST decode: SPEED=%d,NAME=%d,CE=%d",*speed_ptr,*name_ptr,*ce_ptr);

  /* Check for no SPEED match */
  return (i < AT_speed_table_size)? TRUE : FALSE;
} /* ds_ucsd_decode_cbst_values() */


/*===========================================================================


FUNCTION ds_ucsd_validate_chsn_values

DESCRIPTION
  This function validates the +CHSN values against the WAIUR list
  supported within the CS protocol stack.

DEPENDENCIES
   None

RETURN VALUE
   TRUE if the parameter set is valid.  FALSE otherwise.

SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_validate_chsn_values
(
  uint8 connection_element,
  uint8 waiur
)
{
  boolean result = FALSE;
  uint8 i;
  sys_sys_mode_e_type curr_sys_mode = SYS_SYS_MODE_NO_SRV;
  uint8 mode;
#ifdef FEATURE_DUAL_SIM
    if( ds_ucsd_set_call_mode_and_subs_id(DS_UCSD_VIDEO_TELEPHONY_CALL,
                                          &ds_ucsd_network_mode,
                                          &ds_ucsd_subs_id) )
    {
      MSG_HIGH("VT call on subs id %d", (int)ds_ucsd_subs_id, 0, 0);
      /* set the current subs_id in ds3g for CS call origination */
      ds3g_set_cs_subs_id(ds_ucsd_subs_id);
      curr_sys_mode = ds_ucsd_network_mode;
    }
    else
    {
      DATA_MSG1_ERROR("Invalid subs id = %d", ds_ucsd_subs_id);
      return FALSE;
    }
#else
  curr_sys_mode = ds3g_get_current_cs_domain_mode();
  if( INVALID_3GPP_CS_SYS_MODE(curr_sys_mode) )
  {
#ifdef FEATURE_3GPP_CSFB
      DATA_MSG1_HIGH("CSFB: setting mode to internal mode %d",
                      ds_ucsd_internal_mode);
    /* Set the mode currently to WCDMA/TDS as per internal mode */
      curr_sys_mode = ds_ucsd_internal_mode;
#endif /* FEATURE_3GPP_CSFB */
    }
#endif /* FEATURE_DUAL_SIM */

  if( DS_UCSD_IS_WCDMA_TDS_MODE(curr_sys_mode) )
  {
    mode = MODE_WCDMA;
  }
  else
  {
    mode = MODE_GSM;
  }

  /* +CHSN applies only to non-transparent calls, early return
   * otherwise. */
  if( ATCOP_NON_TRANSPARENT_DATA != connection_element )
  {
    return TRUE;
  }

  /* Process Wanted Air Interface Rate (WAIUR) */
  /* For non-transparent calls, WAIUR is the OTA data rate. */
  /* If default setting specified, skip actual validation.  */
  if( ATCOP_CHSN_WAIUR_DEFAULT != waiur )
  {
    for( i = 0; i < ds_ucsd_waiur_mapping_tbl_size; i++ )
    {
      /* Match on WAIUR value and current network mode */
      if( (ds_ucsd_waiur_mapping_tbl[i].at_waiur == waiur) &&
          ((ds_ucsd_waiur_mapping_tbl[i].mode & mode)!=0 ))
      {
        /* Found matching WAIUR */
        result = TRUE;
        break;
      }
    }
  }
  else
  {
    result = TRUE;
  }

  return result;
} /* ds_ucsd_validate_chsn_values() */



/*===========================================================================

FUNCTION DS_UCSD_GET_INSTID

DESCRIPTION
  This function is used to allocate and return a CS Data internal call
  instance ID. It returns NO_CALL_ID if all the entries in the call
  table, ds_ucsd_call_tbl_ptr[], are being used.  The table index is
  returned by parameter as a conveninece, which may be NULL.
        
DEPENDENCIES
   None
  
RETURN VALUE
   CS Data internal call instance ID or NO_CALL_ID.
    
SIDE EFFECTS
   Mark the call table entry used.  Set the call type in the entry to
   UNKNOWN_CALL_TYPE and CM call ID to NO_CALL_ID.
   
   Critical section used to serialize task execution.
   
===========================================================================*/
uint8 ds_ucsd_get_instid
(
  uint8  *index_ptr
)
{
  uint8 index;                                   /* Call table index      */
  uint8 inst_id = NO_CALL_ID;                    /* Instance ID to return */

/*-------------------------------------------------------------------------*/

  /* Enter critical section to ensure serialized exection. */
  rex_enter_crit_sect( &ds_ucsd_instance_crit_sect );

  /* Find an unused entry in the call table */
  for ( index = 0; 
        index < MAX_UCSD_CALL_TBL_ENTRIES; 
        index++ ) 
  {
    if ( ds_ucsd_call_tbl_ptr[ index ]== NULL )
    {
      /* Found an unused entry in the call table */
      break;
    }
  }

  if ( index >= MAX_UCSD_CALL_TBL_ENTRIES ) 
  {
    /* There is no unused entry in the call table. Return NO_CALL_ID */

   DATA_MSG0_ERROR("All entries in call table are being used. Cannot allocate inst ID");
  }
  else
  {
    /*Allocating memory for call table entry */
    ds_ucsd_call_tbl_ptr[ index ] = 
           (ds_ucsd_call_tbl_entry_type *)
            ds_system_heap_mem_alloc_crit(sizeof(ds_ucsd_call_tbl_entry_type));
    if(ds_ucsd_call_tbl_ptr[ index ] == NULL)
    {
      index = MAX_UCSD_CALL_TBL_ENTRIES;
      DATA_MSG2_ERROR( "Memory allocation Failure for client:%d, size:%d",
           MODEM_MEM_CLIENT_DATA_CRIT, sizeof(ds_ucsd_call_tbl_entry_type));
    }
    else
    {
      /* Initialize the call table entry */
      DSUCSD_INIT_CALL_INSTANCE( index );
      /* Remember the CS Data instance ID in the call table */
      ds_ucsd_call_tbl_ptr[ index ]->inst_id = inst_id = index;
      if( NULL != index_ptr )
      {
        *index_ptr = index;
      }
    }
  }

  /* Allow waiting task to proceed. */
  rex_leave_crit_sect( &ds_ucsd_instance_crit_sect );

  if( index < MAX_UCSD_CALL_TBL_ENTRIES )
  {
    DATA_MSG1_HIGH("Allocated CS Data call instance ID = %d", 
              ds_ucsd_call_tbl_ptr[ index ]->inst_id);
  }

  return ( inst_id );
} /* ds_ucsd_get_instid() */


/*===========================================================================

FUNCTION ds_ucsd_find_call_instance

DESCRIPTION
  This function finds a previously allocated record in the call
  instance table based on internal CS Data call instance ID.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Index of the call table array element if found,
  MAX_UCSD_CALL_TBL_ENTRIES otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
uint8 ds_ucsd_find_call_instance
(
  uint8    inst_id                              /* CSData call instance ID */
)
{
  uint8 rc;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Check for invalid instance ID specified */
  if( INVALID_INST_ID <= inst_id )
  {
    /* Immediately return the 'not found' control value */
    return MAX_UCSD_CALL_TBL_ENTRIES;
  }

  /* Verify call instance record is consistent. */
  /* Instance ID is actually just array offset now. */
  if( (ds_ucsd_call_tbl_ptr[ inst_id ] != NULL)            &&
      (inst_id == ds_ucsd_call_tbl_ptr[ inst_id ]->inst_id) )
  {
    rc = inst_id;
  }
  else
  {
    DATA_MSG1_ERROR("Invalid instance ID: %d",inst_id);
    rc = MAX_UCSD_CALL_TBL_ENTRIES;
  }

  return rc;
} /* ds_ucsd_find_call_instance() */



/*===========================================================================

FUNCTION ds_ucsd_find_call_instance_by_port

DESCRIPTION
  This function finds a previously allocated record in the call
  instance table based on assigned SIOLIB port ID.  If port ID is
  PORTID_MAX, returns the first instance with an assigned port (if any).
  
DEPENDENCIES
  None
  
RETURN VALUE
  Index of the call table array element if found,
  MAX_UCSD_CALL_TBL_ENTRIES otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL uint8
ds_ucsd_find_call_instance_by_port
(
  ds3g_siolib_port_e_type   port_id                    /* SIOLIB port ID */
)
{
  uint8 index;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find used entry in the call table */
  for ( index = 0; 
        index < MAX_UCSD_CALL_TBL_ENTRIES; 
        index++ ) 
  {
    /* Match SIOLIB port identifier */
    if( (ds_ucsd_call_tbl_ptr[ index ] != NULL) &&
        ( (ds_ucsd_call_tbl_ptr[ index ]->sio_port_id == port_id) ||
        ( (DS3G_SIOLIB_PORT_MAX == port_id) &&
          (DS3G_SIOLIB_PORT_NONE != ds_ucsd_call_tbl_ptr[ index ]->sio_port_id) )))
    {
      /* Return the current index */
      break;
    }
  }

  return index;
} /* ds_ucsd_find_call_instance_by_port() */


/*===========================================================================

FUNCTION ds_ucsd_find_call_instance_by_type

DESCRIPTION
  This function finds a previously allocated record in the call
  instance table based on CSD call type.  The index of the first
  record with specified call type (if any) is returned.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Index of the call table array element if found,
  MAX_UCSD_CALL_TBL_ENTRIES otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL uint8
ds_ucsd_find_call_instance_by_type
(
  ds_ucsd_call_type  call_type                            /* CSD call type */
)
{
  uint8 index;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find used entry in the call table */
  for ( index = 0; 
        index < MAX_UCSD_CALL_TBL_ENTRIES; 
        index++ ) 
  {
    /* Match SIOLIB port identifier */
    if( (ds_ucsd_call_tbl_ptr[ index ] != NULL) &&
        (ds_ucsd_call_tbl_ptr[ index ]->call_type == call_type ) )
    {
      /* Return the current index */
      break;
    }
  }

  return index;
} /* ds_ucsd_find_call_instance_by_type() */


/*===========================================================================

FUNCTION ds_ucsd_find_call_instance_by_state

DESCRIPTION

  This function finds a previously allocated record in the call
  instance table based on CSD call state.  The index of the first
  record with specified call state (if any) is returned.

  The state parameter may be set of OR'd state values, in which case
  the first matching instance is returned.
  
DEPENDENCIES
  None
  
RETURN VALUE
  Index of the call table array element if found,
  MAX_UCSD_CALL_TBL_ENTRIES otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL uint8
ds_ucsd_find_call_instance_by_state
(
  uint8  call_state                                      /* CSD call state */
)
{
  uint8 index;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Find used entry in the call table */
  for ( index = 0; 
        index < MAX_UCSD_CALL_TBL_ENTRIES; 
        index++ ) 
  {
    /* Match SIOLIB port identifier */
    if( (ds_ucsd_call_tbl_ptr[ index ] != NULL) &&
        (0 != ((uint8)ds_ucsd_call_tbl_ptr[ index ]->state & (call_state))) )
    {
      /* Return the current index */
      break;
    }
  }

  return index;
} /* ds_ucsd_find_call_instance_by_type() */


/*===========================================================================

FUNCTION ds_ucsd_release_call_instance

DESCRIPTION
  This function releases a previously allocated record in the call
  instance table.  Parameter is the CS Data call instance ID.
  
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if record found and cleared; FALSE otherwise.
    
SIDE EFFECTS
  Critical section used to serilize task execution.

===========================================================================*/
LOCAL boolean
ds_ucsd_release_call_instance
(
  uint8    inst_id                              /* CSData call instance ID */
)
{
  boolean result = TRUE;
  uint8 index;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Enter critical section to ensure serialized exection. */
  rex_enter_crit_sect( &ds_ucsd_instance_crit_sect );
  
  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( inst_id )) )
  {
    DATA_MSG1_ERROR("Could not find call instance in table: %d",
               inst_id);
    result = FALSE;
  }
  else
  {
    /* Remove link between instance and CM call ID */
    if( (ds_ucsd_call_tbl_ptr[ index ] != NULL ) &&
        (CM_CALL_ID_INVALID != ds_ucsd_call_tbl_ptr[ index ]->cm_call_id ) )
    {
      result = ds_ucsd_set_call_inst(ds_ucsd_call_tbl_ptr[ index ]->cm_call_id, 
                                      INVALID_INST_ID);
    }

    /* Freeing call table entry */
    ds_system_heap_mem_free_crit(ds_ucsd_call_tbl_ptr[ index ]);
    ds_ucsd_call_tbl_ptr[ index ] = NULL;
  }

  /* Allow waiting task to proceed. */
  rex_leave_crit_sect( &ds_ucsd_instance_crit_sect );

  return result;
} /* ds_ucsd_release_call_instance() */


/*===========================================================================

FUNCTION ds_ucsd_decode_v120_parameters

DESCRIPTION
  This function parses the V.120 parameter array from ATCOP into the
  constituent values for bearer capability.
   
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/ 
LOCAL void ds_ucsd_decode_v120_parameters
(
  const uint32 bit_array,                        /* Parameter values */
  ds_ucsd_v120_param_T * v120_info_ptr           /* Output structure */
)
{
  memset((void*)v120_info_ptr, 0x0, sizeof(ds_ucsd_v120_param_T));
  
  /* Decode the ATCOP bit array */
  v120_info_ptr->ra_header       = (bit_array & ATCOP_V120_RAH_MASK)? 1 : 0;
  v120_info_ptr->multi_frame_est = (bit_array & ATCOP_V120_MFM_MASK)? 1 : 0;
  v120_info_ptr->oper_mode       = (bit_array & ATCOP_V120_MODE_MASK)? 1 : 0;
  v120_info_ptr->llink_id_neg    = (bit_array & ATCOP_V120_LLINEG_MASK)? 1 : 0;
  v120_info_ptr->assignor        = (bit_array & ATCOP_V120_ASSIGN_MASK)? 1 : 0;
  v120_info_ptr->band_neg        = (bit_array & ATCOP_V120_NEGTYPE_MASK)? 1 : 0;
}



/*===========================================================================

FUNCTION ds_ucsd_store_bearer_service_type

DESCRIPTION
   This function stores bearer service type parameters. 
   It should be called before ds_ucsd_get_chosen_cap() is called.
   
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/ 
LOCAL void ds_ucsd_store_bearer_service_type
(
  ds_ucsd_bearer_service_type *service_ptr,/* Bearer service configuration */  
   uint8 speed,                           /* Speed value                   */ 
   uint8 name,                            /* Synchronous or Asynchronous   */ 
  uint8 ce,                               /* Transparent or Non-Transparent*/
  uint8 waiur                             /* Wanted air interface rate     */
)
{
  dsat_num_item_type tmp_dsat_cv120_export_val = dsat_get_cv120_export_val();
  ASSERT( NULL != service_ptr);
    
  /* Store bearer service type in ds_ucsd_bearer_service */
  service_ptr->cbst_info.speed              = speed;
  service_ptr->cbst_info.name               = name;
  service_ptr->cbst_info.connection_element = ce;
  service_ptr->chsn_info.waiur              = waiur;
  
  /* Decode the V.120 parameters from ATCOP */
  ds_ucsd_decode_v120_parameters( tmp_dsat_cv120_export_val,
                                  &service_ptr->v120_info );

}


/*===========================================================================

FUNCTION ds_ucsd_store_call_info

DESCRIPTION
   This function stores call-related information in the call table, 
   ds_ucsd_call_tbl_ptr[].
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/ 
LOCAL void ds_ucsd_store_call_info
(
   uint8                tbl_idx,          /* Call table index              */
   sys_sys_mode_e_type  network_mode,     /* WCDMA or GSM                  */
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type subs_id,        /* Subscription id               */
#endif /* FEATURE_DUAL_SIM */
   ds_ucsd_call_type    call_type,        /* Type of call                  */
   const ds_ucsd_bearer_service_type *service_ptr,
                                          /* Pointer to bearer service info*/
   dsm_watermark_type   *ul_wm_ptr,       /* Pointer to UL L2 WM           */
   dsm_watermark_type   *dl_wm_ptr,       /* Pointer to DL L2 WM           */
   ds_ucsd_rlc_dl_rx_cb_type rlc_dl_rx_cb /* Function called by RLC        **
                                          ** when RLC receives DL data.    **
                                          ** This callback can be NULL     */
)
{
  ASSERT( NULL != service_ptr );
  if( tbl_idx >= MAX_UCSD_CALL_TBL_ENTRIES ||
      (ds_ucsd_call_tbl_ptr[tbl_idx] == NULL) )
  {
    DATA_ERR_FATAL("Invalid Table Index");
    return;
  }
  /* Store call information in the call table */
  ds_ucsd_call_tbl_ptr[ tbl_idx ]->network_mode              = network_mode;
#ifdef FEATURE_DUAL_SIM
  ds_ucsd_call_tbl_ptr[ tbl_idx ]->subs_id                   = subs_id;
#endif /* FEATURE_DUAL_SIM */
  ds_ucsd_call_tbl_ptr[ tbl_idx ]->call_type                 = call_type;
  ds_ucsd_call_tbl_ptr[ tbl_idx ]->bearer_params             = *service_ptr;
  ds_ucsd_call_tbl_ptr[ tbl_idx ]->dpath_params.ul_wm_ptr    = ul_wm_ptr;
  ds_ucsd_call_tbl_ptr[ tbl_idx ]->dpath_params.dl_wm_ptr    = dl_wm_ptr;
  ds_ucsd_call_tbl_ptr[ tbl_idx ]->dpath_params.rlc_dl_rx_cb = rlc_dl_rx_cb;
} /* ds_ucsd_store_call_info() */


/*===========================================================================

FUNCTION DS_UCSD_MAP_FNUR_WAIUR_TO_OTA_RATE

DESCRIPTION
   This function maps FNUR/WAIUR to over-the-air data rate and return the
   data rate. It returns a default value if the FNUR/WAIUR is not supported.
        
DEPENDENCIES
   None
  
RETURN VALUE
   Over-the-air data rate.
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL ds_ucsd_data_rate_T ds_ucsd_map_fnur_waiur_to_ota_rate
( 
   uint8 ce,                             /* Transparent or Non-Transparent */
   sys_sys_mode_e_type network_mode,     /* GSM or WCDMA                   */
   uint8 fnur,                           /* Fixed Network User Rate        */
   uint8 waiur                           /* Wanted Air User Interface Rate */
)
{
   uint8    i;                                  /* Index                   */
   ds_ucsd_data_rate_T ota_rate = DS_UCSD_RATE_NONE; /* OTA data rate      */

/*-------------------------------------------------------------------------*/

   if ( ce == TRANSPARENT_DATA )
   {
      /* For Transparent calls, FNUR is the OTA data rate */
      for( i = 0; i < ds_ucsd_fnur_mapping_tbl_size; i++ )
      {
         if( ds_ucsd_fnur_mapping_tbl[i].fnur == fnur )
         {
            /* Found the FNUR */
            ota_rate = ds_ucsd_fnur_mapping_tbl[i].data_rate;
            break;
         }
      }
      
      if ( i == ds_ucsd_fnur_mapping_tbl_size ) 
      {
         /* The FNUR is not supported. Use the default data rate */
         if ( DS_UCSD_IS_WCDMA_TDS_MODE(network_mode) ) 
         {
            DATA_MSG0_HIGH("Cannot map FNUR to OTA rate. Default OTA rate to 64k");

            ota_rate = DS_UCSD_RATE_64000;   
         }
         else  /* Mode is GSM */
         {
            /* In GSM, UNDEFINED equates to 9600 */
            if (fnur == FNUR_UNDEFINED)
            {
               DATA_MSG0_HIGH("Map T-GSM FNUR UNDEFINED to 9.6k");
               ota_rate = DS_UCSD_RATE_9600;
            }
            else
            {
               DATA_MSG0_HIGH("Cannot map FNUR to OTA rate. Default OTA rate to 14.4k");
               ota_rate = DS_UCSD_RATE_14400;
            }
         }
      }
   }
   else
   {
      /* For Non-Transparent calls, WAIUR is the OTA data rate */
      for( i = 0; i < ds_ucsd_waiur_mapping_tbl_size; i++ )
      {
         if( ds_ucsd_waiur_mapping_tbl[i].waiur == waiur )
         {
            /* Found the WAIUR */
            ota_rate = ds_ucsd_waiur_mapping_tbl[i].data_rate;
            break;
         }
      }
      
      /* Check for no WAIUR lookup match */
      if ( i == ds_ucsd_waiur_mapping_tbl_size ) 
      {
         /* In GSM, WAIUR = WAIUR_UNDEFINED => 9.6 */
         if ( (network_mode == SYS_SYS_MODE_GSM) &&
              (waiur == WAIUR_UNDEFINED) )
         {
            DATA_MSG0_HIGH( "Map NT-GSM WAIUR UNDEFINED to 9.6k");
            ota_rate = DS_UCSD_RATE_9600;   
         }
         else
         {
            /* The WAIUR is not supported. Use the default data rate */
            DATA_MSG0_HIGH( "Cannot map WAIUR to OTA rate. Default OTA rate to 14.4k");

            ota_rate = DS_UCSD_RATE_14400;
         }
      }
   }

   return ( ota_rate );
} /* ds_ucsd_map_fnur_waiur_to_ota_rate() */


/*===========================================================================

FUNCTION DS_UCSD_GET_INCOMING_INTERNAL_CSD_CALL_ENABLE_STATUS

DESCRIPTION
  This function reads from NV whether or not incoming CS Data call is 
  internal. It must be called in the DS task.
   
DEPENDENCIES
  None

RETURN VALUE
  Boolean indicating whether or not incoming CS Data call is internal.
  
SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean ds_ucsd_get_incoming_internal_csd_call_enable_status
( 
  sys_modem_as_id_e_type        subs_id
)
{
  nv_stat_enum_type nv_status;
    /* status var for reading from NV */

  nv_item_type      nv_item;                  
    /* NV interface data */

  boolean           incoming_internal_csd_call_enabled;
    /* Flag indicating whether incoming internal CSD call is enabled in NV */

/*-------------------------------------------------------------------------*/
  
  /* Read from NV whether or not incoming CS Data call is internal */

  nv_status =
     ds3gcfgmgr_read_legacy_nv_ex( NV_DATA_INCOMING_CSD_CALL_IS_INTERNAL_I,
                      &nv_item,
                      subs_id );

  if(nv_status == NV_DONE_S)
  {
    incoming_internal_csd_call_enabled = 
                          nv_item.data_incoming_csd_call_is_internal;
    DATA_MSG1_HIGH("NV_DATA_INCOMING_CSD_CALL_IS_INTERNAL_I is %d",
             incoming_internal_csd_call_enabled);
  }
  else
  {
    /* If problem reading from NV, default to TRUE: incoming CS Data call
    ** is internal.
    */
    DATA_MSG0_ERROR("Default incoming CS Data call to internal call");

    incoming_internal_csd_call_enabled = TRUE;
    
    /* NV item is not initialized. Set it to TRUE: incoming CS Data call is
    ** internal.
    */
    if( nv_status == NV_NOTACTIVE_S )
    {
      nv_item.data_incoming_csd_call_is_internal = TRUE;
      (void) ds3gcfgmgr_read_legacy_nv_ex( NV_DATA_INCOMING_CSD_CALL_IS_INTERNAL_I, 
                              &nv_item,subs_id);
    }
  }

  return ( incoming_internal_csd_call_enabled ); 
} /* ds_ucsd_get_incoming_internal_csd_call_enable_status() */


/*===========================================================================

FUNCTION DS_UCSD_GET_RLP_PARAMS_FROM_NV

DESCRIPTION
  This function reads from NV parameters for non-transparent RLP.
  The list of parameters:
    - Default RLP version for each supported mode.  This ensures the
      RLP XID negotiation is started with the appropriate version for
      the service provider.
    - Selective reject recovery mode.  This toggles between REJ and SREJ
      revovery.  SREJ is best suited to UMTS systems for performance.
    
  It must be called in the DS task context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void ds_ucsd_get_rlp_params_from_nv
(
  ds_ucsd_nt_rlp_parameters_T *rlp_params,
  sys_modem_as_id_e_type        subs_id
)
{
  nv_stat_enum_type nv_status;
    /* status var for reading from NV */

  nv_item_type      nv_item;                  
  /* NV interface data */
  
  boolean anite_gcf_flag = FALSE;

/*-------------------------------------------------------------------------*/

  ASSERT( NULL != rlp_params );

  /* Assume defaults */
  rlp_params->rlp_ver_gsm = XIDDEFAULT_VERSION_0_1;
  rlp_params->rlp_ver_wcdma = XIDDEFAULT_VERSION_2;
  rlp_params->rlp_uses_srej = FALSE;
  rlp_params->rlp_adm_idle_timer = RLP_ADM_IDLE_DEFAULT;
  rlp_params->rlp_connect_state = RLP_CONNECT_STATE_DEFAULT;
    
  /* Read from NV default RLP versions for each mode */

  /* GSM mode */
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_UCSD_RLP_VERSION_GSM_I, &nv_item, subs_id );
  if (nv_status == NV_DONE_S)
  {
    /* Check integrity of returned value */
    if (XIDMAX_VERSION >= nv_item.ds_ucsd_rlp_version_gsm)
    {
      rlp_params->rlp_ver_gsm = nv_item.ds_ucsd_rlp_version_gsm;
      DATA_MSG1_MED("NV_DS_UCSD_RLP_VERSION_GSM_I: %d", rlp_params->rlp_ver_gsm);
    }
    else
    {
      MSG_ERROR("NV_DS_UCSD_RLP_VERSION_GSM_I out of range: %d [%d,%d]",
                nv_item.ds_ucsd_rlp_version_gsm,XIDMIN_VERSION,XIDMAX_VERSION);
    }
  }
  else if ( NV_NOTACTIVE_S == nv_status)
  {
    /*-----------------------------------------------------------------------
      Parameter was initialized; store initial value
      -----------------------------------------------------------------------*/
    nv_item.ds_ucsd_rlp_version_gsm =  rlp_params->rlp_ver_gsm;
    
    /* Post to NV */
    (void)ds3gcfgmgr_write_legacy_nv_ex( NV_DS_UCSD_RLP_VERSION_GSM_I,
                                         &nv_item, 
                                         subs_id );
    DATA_MSG1_MED("NV_DS_UCSD_RLP_VERSION_GSM_I: %d", rlp_params->rlp_ver_gsm);
  }
  else
  {
    /* If problem reading from NV, default to FALSE */
    DATA_MSG0_ERROR("Error reading NV_DS_UCSD_RLP_VERSION_GSM_I from NV");
  }

  
  /* WCDMA mode */
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_UCSD_RLP_VERSION_WCDMA_I, &nv_item,subs_id );
  if (nv_status == NV_DONE_S)
  {
    /* Check integrity of returned value */
    if (XIDMAX_VERSION >= nv_item.ds_ucsd_rlp_version_wcdma)
    {
      rlp_params->rlp_ver_wcdma = nv_item.ds_ucsd_rlp_version_wcdma;
      DATA_MSG1_MED("NV_DS_UCSD_RLP_VERSION_WCDMA_I: %d", rlp_params->rlp_ver_wcdma);
    }
    else
    {
      MSG_ERROR("NV_DS_UCSD_RLP_VERSION_WCDMA_I out of range: %d [%d,%d]",
                nv_item.ds_ucsd_rlp_version_wcdma,
                XIDMIN_VERSION,XIDMAX_VERSION);
    }
  }
  else if ( NV_NOTACTIVE_S == nv_status)
  {
    /*-----------------------------------------------------------------------
      Parameter was initialized; store initial value
      -----------------------------------------------------------------------*/
    nv_item.ds_ucsd_rlp_version_wcdma =  rlp_params->rlp_ver_wcdma;
    
    /* Post to NV */
    (void)ds3gcfgmgr_write_legacy_nv_ex( NV_DS_UCSD_RLP_VERSION_WCDMA_I,
                                         &nv_item ,
                                         subs_id );
    DATA_MSG1_MED("NV_DS_UCSD_RLP_VERSION_WCDMA_I: %d", rlp_params->rlp_ver_wcdma);
  }
  else
  {
    /* If problem reading from NV, default to FALSE */
    DATA_MSG0_ERROR("Error reading NV_DS_UCSD_RLP_VERSION_WCDMA_I from NV");
  }


  /* Read from NV selective reject recovery status */
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_UCSD_RECOVERY_MODE_SREJ_I, &nv_item,subs_id );
  if (nv_status == NV_DONE_S)
  {
    rlp_params->rlp_uses_srej = nv_item.ds_ucsd_recovery_mode_srej;
    DATA_MSG1_MED("NV_DS_UCSD_RECOVERY_MODE_SREJ_I: %d",
            rlp_params->rlp_uses_srej);
  }
  else if ( NV_NOTACTIVE_S == nv_status)
  {
    /*-----------------------------------------------------------------------
      Parameter was initialized; store initial value
      -----------------------------------------------------------------------*/
    nv_item.ds_ucsd_recovery_mode_srej = rlp_params->rlp_uses_srej;
    
    /* Post to NV */
    (void)ds3gcfgmgr_write_legacy_nv_ex( NV_DS_UCSD_RECOVERY_MODE_SREJ_I,  
                                         &nv_item,
                                          subs_id );
    DATA_MSG1_MED("NV_DS_UCSD_RECOVERY_MODE_SREJ_I: %d",
            rlp_params->rlp_uses_srej);
  }
  else
  {
    /* If problem reading from NV, default to FALSE */
    DATA_MSG0_ERROR("Error reading NV_DS_UCSD_RECOVERY_MODE_SREJ_I from NV");
  }

  /* Read NV_GPRS_ANITE_GCF_I from NV. Purpose of the use of gcf flag here is
     to reduce the dependency on nv items for gcf purposes and to consolidate
     gcf nv dependencies on a single nv item NV_GPRS_ANITE_GCF_I.

     When gcf flag is set timers TIMERID_MO_FLUSH and TIMERID_ADM_IDLE are
     set to DS_UCSD_GCF_RLP_ADM_IDLE_TIME which is 2 hours. This means CSD 
     call stays on call for 2 hours before flushing UL buffers. Also, when in 
     ADMAttached state, it takes 2 hours before disconnecting call. */

  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_GPRS_ANITE_GCF_I, &nv_item,subs_id );

  if(NV_DONE_S == nv_status)
  {
    anite_gcf_flag = nv_item.gprs_anite_gcf;
  }
  else
  {
    /* If problem reading NV_GPRS_ANITE_GCF_I from NV */
    DATA_MSG0_ERROR("Error reading NV_GPRS_ANITE_GCF_I from NV");
  }

  /* If Anite flag is set, set timer to DS_UCSD_RLP_ADM_IDLE_TIMER */ 
  if(anite_gcf_flag)
  {
    rlp_params->rlp_adm_idle_timer = DS_UCSD_GCF_RLP_ADM_IDLE_TIMER;
    DATA_MSG1_HIGH("Anite gcf flag is set, adm_idle_timer value %d:",
              rlp_params->rlp_adm_idle_timer);
  }
  else
  {
    /* Read from NV RLP ADM idle timer for disconnect on RLP error */
    nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_UCSD_RLP_ADM_IDLE_I, &nv_item,subs_id );
    if (nv_status == NV_DONE_S)
    {
      rlp_params->rlp_adm_idle_timer = nv_item.ds_ucsd_rlp_adm_idle;
      DATA_MSG1_MED("NV_DS_UCSD_RLP_ADM_IDLE_I: %d",rlp_params->rlp_adm_idle_timer);
    }
    else if ( NV_NOTACTIVE_S == nv_status)
    {
      /*-----------------------------------------------------------------------
        Parameter was initialized; store initial value
      -----------------------------------------------------------------------*/
      nv_item.ds_ucsd_rlp_adm_idle = rlp_params->rlp_adm_idle_timer;
    
      /* Post to NV */
      (void)ds3gcfgmgr_write_legacy_nv_ex( NV_DS_UCSD_RLP_ADM_IDLE_I, 
                                           &nv_item,
                                           subs_id );
      DATA_MSG1_MED("NV_DS_UCSD_RLP_ADM_IDLE_I: %d",
              rlp_params->rlp_adm_idle_timer);
    }
    else
    {
      /* If problem reading from NV, default to zero */
      DATA_MSG0_ERROR("Error reading NV_DS_UCSD_RLP_ADM_IDLE_I from NV");
    }
  }

  /* Read from NV connection state when to send CONNECT response */
  nv_status = ds3gcfgmgr_read_legacy_nv_ex( NV_DS_UCSD_NT_CONNECT_STATE_I, &nv_item,subs_id );
  if (nv_status == NV_DONE_S)
  {
    if( NT_CONNECT_STATE_MAX < nv_item.ds_ucsd_nt_connect_state )
    {
      DATA_MSG2_ERROR("NV_DS_UCSD_NT_CONNECT_STATE_I %d > %d, using default",
                nv_item.ds_ucsd_nt_connect_state, NT_CONNECT_STATE_MAX);
      rlp_params->rlp_connect_state = RLP_CONNECT_STATE_DEFAULT;
    }
    else
    {
      rlp_params->rlp_connect_state = nv_item.ds_ucsd_nt_connect_state;
    }
    DATA_MSG1_MED("NV_DS_UCSD_NT_CONNECT_STATE_I: %d",
            rlp_params->rlp_connect_state);
  }
  else if ( NV_NOTACTIVE_S == nv_status)
  {
    /*-----------------------------------------------------------------------
      Parameter was initialized; store initial value
      -----------------------------------------------------------------------*/
    nv_item.ds_ucsd_nt_connect_state = rlp_params->rlp_connect_state;
    
    /* Post to NV */
    (void)ds3gcfgmgr_write_legacy_nv_ex( NV_DS_UCSD_NT_CONNECT_STATE_I, 
                                         &nv_item,
                                         subs_id );
    DATA_MSG1_MED("NV_DS_UCSD_NT_CONNECT_STATE_I: %d",
            rlp_params->rlp_connect_state);
  }
  else
  {
    /* If problem reading from NV, use default */
    DATA_MSG0_ERROR("Error reading NV_DS_UCSD_NT_CONNECT_STATE_I from NV");
  }

  return; 
} /* ds_ucsd_get_rlp_params_from_nv() */



/*===========================================================================

FUNCTION ds_ucsd_get_rlp_params

DESCRIPTION
  This function fills in the passed in RLP parameter
  data pointer with data from ATCOP and NV 
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void
ds_ucsd_get_rlp_params
(
   ds_ucsd_nt_rlp_parameters_T *rlp_params,
   sys_modem_as_id_e_type        subs_id
)
{ 
   dsat_num_item_type idx;
   dsat_num_item_type tmp_val;

   ASSERT( NULL != rlp_params );
   
   /* Query NV for default RLP parameters */
   ds_ucsd_get_rlp_params_from_nv (rlp_params,subs_id);
  
   /* Get +CRLP values from ATCOP */
   CRLP_SIZE(ATCOP_CRLP_V0, ATCOP_CRLP_IWS_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_iws = (uint16)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V0, ATCOP_CRLP_MWS_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_mws = (uint16)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V0, ATCOP_CRLP_T1_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_t1 = (uint8)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V0, ATCOP_CRLP_N2_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_n2 = (uint8)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V1, ATCOP_CRLP_IWS_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_iws_V1 = (uint16)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V1, ATCOP_CRLP_MWS_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_mws_V1 = (uint16)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V1, ATCOP_CRLP_T1_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_t1_V1= (uint8) tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V1, ATCOP_CRLP_N2_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_n2_V1 = (uint8)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V2, ATCOP_CRLP_IWS_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_iws_V2 = (uint16)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V2, ATCOP_CRLP_MWS_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_mws_V2 = (uint16)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V2, ATCOP_CRLP_T1_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);
   rlp_params->rlp_t1_V2 = (uint8)tmp_val;
   CRLP_SIZE(ATCOP_CRLP_V2, ATCOP_CRLP_N2_POSITION, idx)\
   tmp_val = (dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_CRLP_IDX,0, idx, NUM_TYPE);	
   rlp_params->rlp_n2_V2 = (uint8)tmp_val;

   /* Get +DS values from ATCOP */
   tmp_val                  =  (dsat_num_item_type)dsatutil_get_val(
                               DSAT_EXT_DS_IDX,
                                       0,ATCOP_DS_P0_POSITION,NUM_TYPE);
   rlp_params->rlp_P0       = (uint8)tmp_val;
   tmp_val                  =  (dsat_num_item_type)dsatutil_get_val(
                               DSAT_EXT_DS_IDX,
                                       0,ATCOP_DS_COMP_NEG_POSITION,NUM_TYPE);
   rlp_params->rlp_comp_neg = (uint8)tmp_val;
   tmp_val                  = (dsat_num_item_type) dsatutil_get_val(
                               DSAT_EXT_DS_IDX,
                                       0,ATCOP_DS_P1_POSITION,NUM_TYPE);
   rlp_params->rlp_P1       = (uint16)tmp_val;
   tmp_val                  =  (dsat_num_item_type)dsatutil_get_val(
                               DSAT_EXT_DS_IDX,
                                       0,ATCOP_DS_P2_POSITION,NUM_TYPE);
   rlp_params->rlp_P2       = (uint8)tmp_val;

   /* Set default values for rest of parameters */
   rlp_params->rlp_Pt       = DEF_RLP_Pt; 
   rlp_params->rlp_t4       = DEF_RLP_T4; 
} /* ds_ucsd_get_rlp_params() */


/*===========================================================================

FUNCTION ds_ucsd_map_chsn_to_bcie

DESCRIPTION
  This function maps the ATCOP +CHSN parameters to bearer capability
  values.
         
DEPENDENCIES
  None

RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean ds_ucsd_map_chsn_to_bcie
(
  const uint8  chsn_val,
  uint8       *waiur_ptr  
)
{
  boolean result = TRUE;

  ASSERT( NULL != waiur_ptr );
  
  /* Map +CHSN value based on network mode */
  if ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode))
  {
    switch ( chsn_val )
    {
    case ATCOP_CHSN_WAIUR_DEFAULT:
      /* Do no change previously derived WAIUR */
      break;
      
    /* User specified WAIUR 's */
    case ATCOP_CHSN_WAIUR_14400:
      *waiur_ptr = WAIUR_14400;
      break;
                   
    case ATCOP_CHSN_WAIUR_28800:
      *waiur_ptr = WAIUR_28800;
      break;
                  
    case ATCOP_CHSN_WAIUR_57600:
      *waiur_ptr = WAIUR_57600;
      break;
                  
    default:
      DATA_MSG1_ERROR("Unsupported WCDMA WAIUR value from ATCOP=%d",chsn_val);
      result = FALSE;
      break;
    } /* End switch */
  }
  else
  {
    switch( chsn_val )
    {
    case ATCOP_CHSN_WAIUR_DEFAULT:
      /* Do no change previously derived WAIUR */
      break;
               
    /* User specified WAIUR 's */
    case ATCOP_CHSN_WAIUR_9600:
      *waiur_ptr = WAIUR_9600;
      break;

    case ATCOP_CHSN_WAIUR_14400:
      *waiur_ptr = WAIUR_14400;
      break;
                   
    default:
      DATA_MSG1_ERROR("Unsupported GSM WAIUR value from ATCOP=%d",chsn_val);
      result = FALSE;
      break;
    }
  }
  
  return result;
} /* ds_ucsd_map_chsn_to_bcie() */


/*===========================================================================

FUNCTION ds_ucsd_get_chosen_cap

DESCRIPTION
  This function fills in the passed in chosenCapability parameter
  with bearer capability and low-level compatibility information 
  which is derived from the bearer service type sent to the mobile. 
         
DEPENDENCIES
  ds_ucsd_store_bearer_service_type() must be called before this function 
  is called.
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
boolean
ds_ucsd_get_chosen_cap 
(
  const ds_ucsd_bearer_service_type  *service_ptr,
  ds_ucsd_chosen_capability_T        *chosen_Capability
)
{
   int i;
   ds_ucsd_chosen_capability_T tempChosen_capability;
   boolean valuesOK;
   uint8 atCmdVal;
   dsat_num_item_type tmp_val;

   ASSERT( NULL != service_ptr );
   ASSERT( NULL != chosen_Capability );
   
   memset ((void*)&tempChosen_capability, 0x0,
           sizeof(ds_ucsd_chosen_capability_T));
   
   /* Check the speed parameter */
   
   /* Get the Speed value */
   atCmdVal = service_ptr->cbst_info.speed;
   
   for( i = 0; i < AT_speed_table_size; i++ )
   {
     if( AT_speed_table[i].speed == atCmdVal )
     {
       tempChosen_capability.chosen_bc.mode =
         AT_speed_table[i].mode;
       tempChosen_capability.chosen_bc.fnur =
         AT_speed_table[i].bc_fnur;
       tempChosen_capability.chosen_bc.waiur =
         AT_speed_table[i].bc_waiur;
       tempChosen_capability.chosen_bc.modem_type =
         AT_speed_table[i].bc_modem_type;
       tempChosen_capability.chosen_bc.other_modem_type =
         AT_speed_table[i].bc_other_modem_type;
       tempChosen_capability.chosen_bc.itc =
         AT_speed_table[i].bc_itc;
       tempChosen_capability.chosen_bc.rate_adaption =
         AT_speed_table[i].bc_ra;
       tempChosen_capability.chosen_bc.other_rate_adaption =
         AT_speed_table[i].bc_other_ra;

       tempChosen_capability.chosen_llc.present =
         AT_speed_table[i].llc_present;
       tempChosen_capability.chosen_llc.user_rate =
         AT_speed_table[i].llc_user_rate;

       /* user_rate is BCIE is always 9600... we don't support any
       ** speeds below this.
       */
       tempChosen_capability.chosen_bc.user_rate = USER_RATE_9600;

       /* Get out of loop */
       break;

     }
   }
   
   valuesOK = (i < AT_speed_table_size) ? TRUE : FALSE;
   
   if ( valuesOK == FALSE ) 
   {
      DATA_MSG1_ERROR( "Invalid Speed value: %d", atCmdVal);
   }

   /* Now check the name parameter */
   
   if (valuesOK)
   {
      /* Get the Name value */
      atCmdVal = service_ptr->cbst_info.name;
      
      for( i = 0; i < AT_name_table_size; i++ )
      {
         if( AT_name_table[i].name == atCmdVal )
         {
            tempChosen_capability.chosen_bc.sync_async =
                              AT_name_table[i].bc_sync_async;
            tempChosen_capability.chosen_llc.sync_async =
                              AT_name_table[i].llc_sync_async;

            /* Resolve ITC 3.1k,UDI,RDI */
            if (ITC_BF_3_1K == (ITC_BF_3_1K & tempChosen_capability.chosen_bc.itc))
            {
              tempChosen_capability.chosen_bc.itc = ITC_3_1K;
            }
            else if ( (ITC_BF_UDI ==
                  (ITC_BF_UDI & tempChosen_capability.chosen_bc.itc)) &&
                 (ITC_BF_UDI ==
                  (ITC_BF_UDI & AT_name_table[i].bc_itc)) )
            {
              tempChosen_capability.chosen_bc.itc = ITC_UDI;
            }
            else if ( (ITC_BF_OTHER ==
                       (ITC_BF_OTHER & tempChosen_capability.chosen_bc.itc)) &&
                      (ITC_BF_OTHER ==
                       (ITC_BF_OTHER & AT_name_table[i].bc_itc)) )
            {
              tempChosen_capability.chosen_bc.itc = ITC_OTHER;
            }
            else
            {
              DATA_MSG1_ERROR("Cannot map ITC: %d",
                         tempChosen_capability.chosen_bc.itc);
            }
            break;
         }
      }
      
      valuesOK = (i < AT_name_table_size) ? TRUE : FALSE;
      
      if ( valuesOK == FALSE ) 
      {
         DATA_MSG1_ERROR("Invalid Name value: %d", atCmdVal);
      }
   }
   
    /* Now check the connection element parameter */
   if (valuesOK)
   {
      /* Get the Connection Element value */
      atCmdVal = service_ptr->cbst_info.connection_element;
      
      for( i = 0; i < AT_ce_table_size; i++ )
      {
         if( AT_ce_table[i].ce == atCmdVal )
         {
            tempChosen_capability.chosen_bc.connection_element =
                                 AT_ce_table[i].connection_element;
            break;
         }
      }
      
      valuesOK = (i < AT_ce_table_size) ? TRUE : FALSE;

      if ( valuesOK == FALSE ) 
      {
         DATA_MSG1_ERROR("Invalid CE value: %d", atCmdVal);
      }
   }
   
   /* Check if the current network mode is compatible with the currently
   ** selected +CBST setting. 
   */
   if (valuesOK)
   {
      /* If in WCDMA/TD_SCDMA mode, make sure it is supported by current
      ** speed setting
      */
      if ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) )
      {
         if (!(tempChosen_capability.chosen_bc.mode & MODE_WCDMA)) 
         {
            valuesOK = FALSE;
            DATA_MSG1_ERROR("Speed setting %d not valid in WCDMA mode",
                       service_ptr->cbst_info.speed);
         }
      }
      /* If in GSM mode, make sure it is supported by current
      ** speed setting
            */
      else if (ds_ucsd_network_mode == SYS_SYS_MODE_GSM)
      {
#ifdef FEATURE_MULTIMEDIA_FALLBACK
        #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
        {       
          if (!(tempChosen_capability.chosen_bc.mode & MODE_GSM)) 
          {
            valuesOK = FALSE;
            DATA_MSG1_ERROR("Speed setting %d not valid in GSM mode",
                       service_ptr->cbst_info.speed);
          }
        }
      }
      /* We should never even be in this code if we aren't in 
      ** GSM or WCDMA mode, but just in case.... 
      */
      else
      {
        valuesOK = FALSE;
        DATA_MSG1_ERROR("Mode %d invalid for UMTS CSD call",
                   ds_ucsd_network_mode);
      }
   }
      
   /* Now check to make sure that we don't support Transparent data in 
   ** WCDMA mode for 3.1KHz calls (we could have the 14.4 CBST setting
   ** which supports both T & NT).  This is the only special case checking
   ** that needs to be done.
            */
   if ((valuesOK) &&
       ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) )&&
       (tempChosen_capability.chosen_bc.itc == ITC_3_1K) &&
       (tempChosen_capability.chosen_bc.connection_element == TRANSPARENT_DATA))
   {
     valuesOK = FALSE;
   }

   /* At this point, all validation has been completed.  Now take care
   ** of autobauding case, and changing WAIUR values based on +CHSN
   ** command.
   */
   if (valuesOK)
   {
     /* Set for autobauding case */
     if (tempChosen_capability.chosen_bc.modem_type == AUTO_TYPE_1)
     {
       /* In WCDMA mode, we will try to bring up a 57.6 call per
        * 3GPP TS22.002 sect 3.1.1 */
       if ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) )
       {
         tempChosen_capability.chosen_bc.fnur = FNUR_NOT_APPLICABLE;
         tempChosen_capability.chosen_bc.waiur = WAIUR_57600;
       }
       /* In GSM mode, we will try to bring up a 9.6 call */
       else
       {
         tempChosen_capability.chosen_bc.fnur = FNUR_UNDEFINED;
         tempChosen_capability.chosen_bc.waiur = WAIUR_UNDEFINED;
       }
     }

     /* As per spec, do +CHSN processing only for NT calls. */
     if ( NON_TRANSPARENT_DATA ==
          tempChosen_capability.chosen_bc.connection_element )
     {
       /* Now set the WAIUR values according to the users +CHSN setting. */ 
       tmp_val = (dsat_num_item_type)dsatutil_get_val(
                  DSATETSI_EXT_CHSN_IDX,0,ATCOP_CHSN_WAIUR_POSITION,NUM_TYPE);
       valuesOK = ds_ucsd_map_chsn_to_bcie(
                        (uint8)tmp_val,
                        &tempChosen_capability.chosen_bc.waiur );
         
       MSG_HIGH ("CS ASYNC +CHSN=%d, FNUR=%d, WAIUR=%d",
                 (dsat_num_item_type)dsatutil_get_val(
                        DSATETSI_EXT_CHSN_IDX,0,ATCOP_CHSN_WAIUR_POSITION,NUM_TYPE),
                 tempChosen_capability.chosen_bc.fnur,
                 tempChosen_capability.chosen_bc.waiur );
     }  
   }

   /* Use V.120 parameters decoded previously */
   tempChosen_capability.chosen_bc.v120_info = service_ptr->v120_info;
   
   /* If everything checked out, copy the chosen capability data to
   ** the output parameter of the function
   */
   if (valuesOK)
   {
      *chosen_Capability = tempChosen_capability;
   }

   return valuesOK;   
} /* ds_ucsd_get_chosen_cap() */



/*===========================================================================

FUNCTION ds_ucsd_hdlr_init_orig_dial_mod_params

DESCRIPTION
  This function fills in the CLIR and CUG call origination settings
  based on dial string modifiers from ATCOP. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void
ds_ucsd_hdlr_init_orig_dial_mod_params
(
  const ds_ucsd_call_tbl_entry_type  *call_info_ptr,
  cm_gw_cs_orig_params_s_type          *gw_cs_orig_params
)
{
  ASSERT( NULL != call_info_ptr );
  ASSERT( NULL != gw_cs_orig_params );
  
  /* Initialize CLIR and CUG paramaters */
  gw_cs_orig_params->cm_clir = CM_CLIR_DEFAULT;
  memset((void*)&gw_cs_orig_params->forward_cug_info,
         0, sizeof(cm_forward_cug_info_s_type));

  /* See if ATCOP provided modifier info */
  if ( TRUE == call_info_ptr->dial_string_modifiers.modifier_present )
  {
    /* Populate CLIR flag if set */
    if (( DSAT_CLI_DIAL_MODIFIER == 
          (dsat_dial_modifier_e_type)call_info_ptr->dial_string_modifiers.modifier ) ||
        ( DSAT_CLI_AND_CUG_MODIFIER  == 
          (dsat_dial_modifier_e_type)call_info_ptr->dial_string_modifiers.modifier ))
    {
      gw_cs_orig_params->cm_clir = 
        (call_info_ptr->dial_string_modifiers.modifier_info.cli.cli_enabled)?
          CM_CLIR_INVOCATION : CM_CLIR_SUPPRESSION;
    }

    /* Populate CUG info if set */
    if (( DSAT_CUG_DIAL_MODIFIER == 
          (dsat_dial_modifier_e_type)call_info_ptr->dial_string_modifiers.modifier ) ||
        ( DSAT_CLI_AND_CUG_MODIFIER  == 
          (dsat_dial_modifier_e_type)call_info_ptr->dial_string_modifiers.modifier ))
    {
      gw_cs_orig_params->forward_cug_info.present = 
        call_info_ptr->dial_string_modifiers.modifier_info.cug.cug_enabled;

      gw_cs_orig_params->forward_cug_info.cm_suppress_pref_cug =
        call_info_ptr->dial_string_modifiers.modifier_info.cug.suppress_pref_cug;
      
      gw_cs_orig_params->forward_cug_info.cm_suppress_outgoing_access =
        call_info_ptr->dial_string_modifiers.modifier_info.cug.suppress_outgoing_access;

      /* Populate CUG index if set */
      if ( TRUE == call_info_ptr->dial_string_modifiers.modifier_info.cug.cug_index_enabled )
      {
        /* ATCOP range for CUG index is [0..9] */
        gw_cs_orig_params->forward_cug_info.cm_cug_index.present = TRUE;
        gw_cs_orig_params->forward_cug_info.cm_cug_index.msb = 0;
        gw_cs_orig_params->forward_cug_info.cm_cug_index.lsb = 
          call_info_ptr->dial_string_modifiers.modifier_info.cug.cug_index_val;
      }
      else
      {
        gw_cs_orig_params->forward_cug_info.cm_cug_index.present = FALSE;
      }
    }
    else
    {
      gw_cs_orig_params->forward_cug_info.present = FALSE;
    }
  }
  return;    
} /* ds_ucsd_hdlr_init_orig_dial_mod_params() */


/*===========================================================================

FUNCTION DS_UCSD_HDLR_INIT_UUS_INFO

DESCRIPTION
  This function fills in the UUS info and does any formating required. For
  now, UUS info comes in from VT/QVP for support of 2 stage dialing.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/

#ifdef FEATURE_UUS
LOCAL void
ds_ucsd_hdlr_init_orig_uus_info
(
  const ds_ucsd_call_tbl_entry_type  *call_info_ptr,
  cm_gw_cs_orig_params_s_type          *gw_cs_orig_params
)
{
  ASSERT( NULL != call_info_ptr );
  ASSERT( NULL != gw_cs_orig_params );
  
  /*-------------------------------------------------------------------------
    Initialize UUS info, defaulting to not present. These IEs are common to 
    all CSD calls, but only used by VT for now. They are updated in 
    ds_ucsd_originate_call_handler().
  -------------------------------------------------------------------------*/
  memset((void*)&gw_cs_orig_params->mo_user_data,
         0, sizeof(cm_user_data_s_type));

  /*-------------------------------------------------------------------------
    Check if UUS info is present.
  -------------------------------------------------------------------------*/
  if ( call_info_ptr->uus_info.present == TRUE )
  {
    /*-----------------------------------------------------------------------
      Check for valid H.323 address id.If ok, format the UUS payload. For
      now, this is based on doc from E///!! See ds_ucsd_uus_id_e_type in 
      dsucsd.h
    -----------------------------------------------------------------------*/
    if( call_info_ptr->uus_info.uus_id >= DS_UMTS_CSD_UUS_ID_FIRST_VALID &&
        call_info_ptr->uus_info.uus_id < DS_UMTS_CSD_UUS_ID_ENUM_MAX)
    {
      call_info_ptr->uus_info.uus_data[0] = (byte)call_info_ptr->uus_info.uus_id;
    }
    else
    {
      DATA_MSG1_HIGH("Invalid H.323 addr id = %d",call_info_ptr->uus_info.uus_id);
      return;
    }

    /*-----------------------------------------------------------------------
      Mark as UUS data being present.
    -----------------------------------------------------------------------*/
    gw_cs_orig_params->mo_user_data.present                 = TRUE;

    /*-----------------------------------------------------------------------
      Set all the fields. Note, this is UUS1 specific for now.
    -----------------------------------------------------------------------*/
    gw_cs_orig_params->mo_user_data.user_user_type          = UUS1_IMPLICIT;

    /*-----------------------------------------------------------------------
      TS 24.008, sect. 10.5.4.25 User-user IE; octet 2 length of UU contents
    -----------------------------------------------------------------------*/
    gw_cs_orig_params->mo_user_data.user_user_data_length   = 
                                           call_info_ptr->uus_info.data_len;

    /*-----------------------------------------------------------------------
      TS 24.008, sect. 10.5.4.25 User-user IE; octet 3 protocol discriminator
      Zero is User Specific Protocol.
    -----------------------------------------------------------------------*/
    gw_cs_orig_params->mo_user_data.user_user_protocol_disc = 0;

    /*-----------------------------------------------------------------------
      TS 24.008, sect. 10.5.4.25 User-user IE; octet 4 begins user data.
      Since we're using a user specific protocol, octet 4 will actualy have
      the H.323 address ID encoded in it. 
    -----------------------------------------------------------------------*/
    (void)memscpy((void*)gw_cs_orig_params->mo_user_data.user_user_data, 
                   MAX_USER_USER_DATA_LENGTH, 
                  (const void*)call_info_ptr->uus_info.uus_data,
                   sizeof(call_info_ptr->uus_info.data_len));

    /*-----------------------------------------------------------------------
      Place holder for use w/UUS3. Just set to False.
    -----------------------------------------------------------------------*/
    gw_cs_orig_params->mo_user_data.more_data_indicator     = FALSE;

  }

} /* ds_ucsd_hdlr_init_orig_uus_info() */

#endif /* FEATURE_UUS */


/*===========================================================================

FUNCTION dsucsd_init_sio_watermarks

DESCRIPTION
  This function initializes the Uplink and Downlink SIO watermarks.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void 
ds_ucsd_init_sio_watermarks( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Initialize the Downlink SIO watermark */
   memset(&ds_ucsd_to_sio_wm, 0, sizeof(dsm_watermark_type));
   memset(&ds_ucsd_to_sio_q, 0, sizeof(q_type));
   dsm_queue_init(&ds_ucsd_to_sio_wm,
                  (SDU_SIZE_FOR_64K * 40),
                  &ds_ucsd_to_sio_q);

   ds_ucsd_to_sio_wm.lo_watermark        = SDU_SIZE_FOR_64K;
   ds_ucsd_to_sio_wm.hi_watermark        = SDU_SIZE_FOR_64K * 18;
   ds_ucsd_to_sio_wm.gone_empty_func_ptr = NULL; 
   ds_ucsd_to_sio_wm.lowater_func_ptr    = NULL;
   ds_ucsd_to_sio_wm.non_empty_func_ptr  = NULL;
   ds_ucsd_to_sio_wm.hiwater_func_ptr    = NULL;

} /* ds_ucsd_init_sio_watermarks() */


/*===========================================================================

FUNCTION ds_ucsd_init_l2_watermarks

DESCRIPTION
  This function initializes the Uplink and Downlink L2 watermarks.
  Note the  flow control levels are set during call connection by
  seperate routine. 
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_init_l2_watermarks ( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

   /* Initialize the Uplink L2 watermark */
   
   memset(&ds_ucsd_to_l2_wm, 0, sizeof(dsm_watermark_type));
   memset(&ds_ucsd_to_l2_q, 0, sizeof(q_type));
   dsm_queue_init(&ds_ucsd_to_l2_wm, 0, &ds_ucsd_to_l2_q);
   ds_ucsd_to_l2_wm.lo_watermark          = 0;
   ds_ucsd_to_l2_wm.hi_watermark          = 0;
   ds_ucsd_to_l2_wm.gone_empty_func_ptr   = NULL;
   ds_ucsd_to_l2_wm.lowater_func_ptr      = NULL;
   ds_ucsd_to_l2_wm.non_empty_func_ptr    = NULL;
   ds_ucsd_to_l2_wm.hiwater_func_ptr      = NULL;

   /* Initialize the Downlink L2 watermark */  

   memset(&ds_ucsd_from_l2_wm, 0, sizeof(dsm_watermark_type));
   memset(&ds_ucsd_from_l2_q, 0, sizeof(q_type));
   dsm_queue_init(&ds_ucsd_from_l2_wm, 0, &ds_ucsd_from_l2_q);
   ds_ucsd_from_l2_wm.lo_watermark        = 0;
   ds_ucsd_from_l2_wm.hi_watermark        = 0;
   ds_ucsd_from_l2_wm.gone_empty_func_ptr = NULL;
   ds_ucsd_from_l2_wm.lowater_func_ptr    = NULL;
   ds_ucsd_from_l2_wm.non_empty_func_ptr  = NULL;
   ds_ucsd_from_l2_wm.hiwater_func_ptr    = NULL;
   
} /* ds_ucsd_init_l2_watermarks() */


/*===========================================================================

FUNCTION ds_ucsd_configure_l2_watermarks

DESCRIPTION
  This function configure watermarks for CS data calls.  The
  watermarks are configured based on the type of call.  

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_configure_l2_watermarks
(
  ds_ucsd_call_tbl_entry_type    *call_info_ptr          /* Call instance  */
)
{
  DATA_MSG0_MED("In ds_ucsd_configure_l2_watermarks");

  /* Local watermarks used for connection to L2. */
  call_info_ptr->dpath_params.ul_wm_ptr = &ds_ucsd_to_l2_wm;
  call_info_ptr->dpath_params.dl_wm_ptr = &ds_ucsd_from_l2_wm;
  call_info_ptr->dpath_params.rlc_dl_rx_cb = NULL;
  call_info_ptr->dpath_params.sio_ul_rx_cb = NULL;

  /* Set flow control levels. */
#ifdef FEATURE_DATA_GCSD_FAX
  if( IS_CALL_TYPE( call_info_ptr->call_type, DS_UCSD_FAX_CALL ) ) /*lint !e655 */ 
  {
    /* Need to minimize data accumulation given slow drain on UL chain */
    ds_ucsd_to_l2_wm.lo_watermark          = DST31_SIO_UL_LO_WM_THRESHOLD;
    ds_ucsd_to_l2_wm.hi_watermark          = DST31_SIO_UL_HI_WM_THRESHOLD;
    ds_ucsd_to_l2_wm.dont_exceed_cnt       = DST31_SIO_UL_DONT_EXCEED_WM_THRESHOLD;
    ds_ucsd_to_l2_wm.gone_empty_func_ptr   = NULL;
    ds_ucsd_to_l2_wm.lowater_func_ptr      = NULL;
    ds_ucsd_to_l2_wm.non_empty_func_ptr    = NULL;
    ds_ucsd_to_l2_wm.hiwater_func_ptr      = NULL;


    ds_ucsd_from_l2_wm.lo_watermark        = DST31_SIO_DL_LO_WM_THRESHOLD;
    ds_ucsd_from_l2_wm.hi_watermark        = DST31_SIO_DL_HI_WM_THRESHOLD;
    ds_ucsd_from_l2_wm.dont_exceed_cnt     = DST31_SIO_DL_DONT_EXCEED_WM_THRESHOLD;
    ds_ucsd_from_l2_wm.gone_empty_func_ptr = NULL;
    ds_ucsd_from_l2_wm.lowater_func_ptr    = NULL;
    ds_ucsd_from_l2_wm.non_empty_func_ptr  = NULL;
    ds_ucsd_from_l2_wm.hiwater_func_ptr    = NULL;

  }
  else
#endif /* FEATURE_DATA_GCSD_FAX */
  {
    ds_ucsd_to_l2_wm.lo_watermark          = ds_ucsd_to_l2_wm_lo_wm   /*SDU_SIZE_FOR_64K * 10*/;
    ds_ucsd_to_l2_wm.hi_watermark          = ds_ucsd_to_l2_wm_hi_wm   /*SDU_SIZE_FOR_64K * 20*/;
    ds_ucsd_to_l2_wm.dont_exceed_cnt       = ds_ucsd_to_l2_wm_dne_wm  /*SDU_SIZE_FOR_64K * 700*/;
    ds_ucsd_to_l2_wm.gone_empty_func_ptr   = NULL;
    ds_ucsd_to_l2_wm.lowater_func_ptr      = NULL;
    ds_ucsd_to_l2_wm.non_empty_func_ptr    = NULL;
    ds_ucsd_to_l2_wm.hiwater_func_ptr      = NULL;

    ds_ucsd_from_l2_wm.lo_watermark        = SDU_SIZE_FOR_64K;
    ds_ucsd_from_l2_wm.hi_watermark        = SDU_SIZE_FOR_64K * 18;
    ds_ucsd_from_l2_wm.dont_exceed_cnt     = SDU_SIZE_FOR_64K * 20;
    ds_ucsd_from_l2_wm.gone_empty_func_ptr = NULL; 
    ds_ucsd_from_l2_wm.lowater_func_ptr    = NULL;
    ds_ucsd_from_l2_wm.non_empty_func_ptr  = NULL;
    ds_ucsd_from_l2_wm.hiwater_func_ptr    = NULL;

    if( ( IS_VIDEO_CALL( call_info_ptr->call_type ) && 
        (DS_UCSD_IFACE_API == call_info_ptr->datapath_iface) )) 
    {
      /*  Assigning the default values equivalent to QVP.
      */
      ds_ucsd_to_l2_wm.lo_watermark          = 0;
      ds_ucsd_to_l2_wm.hi_watermark          = SDU_SIZE_FOR_64K * 4;
      ds_ucsd_to_l2_wm.dont_exceed_cnt       = SDU_SIZE_FOR_64K * 8;
      ds_ucsd_from_l2_wm.dont_exceed_cnt     = SDU_SIZE_FOR_64K * 200;
    }
  }

  /* Clean up the registered watermarks */
  dsm_empty_queue( &ds_ucsd_from_l2_wm );
  dsm_empty_queue( &ds_ucsd_to_l2_wm );

  /* Configure watermarks for external data call */
  if ( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface ) 
  {
#ifdef FEATURE_DATA_WCDMA_CS
    /* Tweak for synchronous calls (raw data or external VT) */
    if( ( DS_UCSD_IS_WCDMA_TDS_MODE(call_info_ptr->network_mode) ) &&
        ( IS_CALL_TYPE( call_info_ptr->call_type, DS_UCSD_SYNC_CSD_CALL ) ||         /*lint !e655 */
          IS_CALL_TYPE( call_info_ptr->call_type, DS_UCSD_VIDEO_TELEPHONY_CALL ) ) ) /*lint !e655 */ 
    {
      /* WCDMA Sync CS Data */

      /* Local watermarks used for connection to L2.  Used by SIOLIB
       * online-command mode switch handler */
      call_info_ptr->dpath_params.sio_ul_rx_cb = dswcsd_rx_data_from_sio;

      /* Configure the Downlink SIO watermark */
      if(!dswcsd_config_to_sio_wm( &ds_ucsd_from_l2_wm ))
      {
        DATA_MSG0_ERROR("dswcsd_config_to_sio_wm() failed");
        return;              
      }
      /* Set L2 WM callback functions */
      ds_ucsd_to_l2_wm.gone_empty_func_ptr   = dswcsd_set_ul_tx_data_sig;
      ds_ucsd_to_l2_wm.lowater_func_ptr      = dswcsd_set_ul_tx_data_sig;
      ds_ucsd_from_l2_wm.non_empty_func_ptr  = dswcsd_set_dl_tx_data_sig;
    }
    else
#endif /* FEATURE_DATA_WCDMA_CS */
    {
      /* Single processor targets */
      
      /* Setup watermarks used between SIOLIB and GCSD task */
      ds_ucsd_mpshim_configure_l2_watermarks( call_info_ptr );
    }

  } /* DS_UCSD_IFACE_SIO */
  
} /* ds_ucsd_configure_l2_watermarks() */


/*===========================================================================

FUNCTION ds_ucsd_process_signals

DESCRIPTION
   This function will be called from the main DS task dispather to send
   signals to the UCSD mode-specific subtask.  Currently there are no
   UCSD mode specific signals. 
   
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/ 
void
ds_ucsd_process_signals
(
   rex_sigs_type set_sigs 
)
{
   DATA_MSG1_ERROR("UCSD signal handler incorrectly called, sigs=%lx",set_sigs);
}


#ifdef FEATURE_DATA_WCDMA_CS

/*===========================================================================

FUNCTION ds_ucsd_connect_l2_interface

DESCRIPTION
  This function connects the CS stack with the interface with layer 2 protocol.
  For synchronous calls only, the CONNECT result code is sent before
  interface change to preserve TE notification.
        
DEPENDENCIES
   None
  
RETURN VALUE
   TRUE for successful processing; FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean
ds_ucsd_connect_l2_interface
(
  ds_ucsd_call_tbl_entry_type        *call_info_ptr,
  const cm_bearer_capability_s_type  *data_bc_ptr,
  const uint32                        rab_id,
  const boolean                       is_handover
)
{
  /* Logical channel information */
  rrc_user_plane_lc_info_type  lc_info;
  
  /* CS RAB information */
  rrc_cs_data_parms_type       cs_data_parms;

  /* Number of bytes in an SDU */
  uint16                       bytes_per_sdu = 0; 

  /* Number of SDUs per TTI */
  uint16                       sdus_per_tti = 0;

  /* Watermark information */
  ds_ucsd_dpath_info_type      datapath;

#ifdef FEATURE_TDSCDMA
  /* TDS Logical channel information */
  tdsrrc_user_plane_lc_info_type  tds_lc_info;
  
  /* TDS CS RAB information */
  tdsrrc_cs_data_parms_type       tds_cs_data_parms;
#endif /* FEATURE_TDSCDMA */
  sys_modem_as_id_e_type subs_id = ds3g_get_cs_subs_id();
/*-------------------------------------------------------------------------*/

   ASSERT( NULL != call_info_ptr );
  
#ifdef FEATURE_DUAL_SIM
   subs_id = call_info_ptr->subs_id;
#endif /* FEATURE_DUAL_SIM */
  
   DATA_MSG2_MED("In ds_ucsd_connect_l2_interface: RABId %d; isHO %d",
           rab_id, is_handover);
   
   /* Get the corresponding RLC ids for the RAB id */
   memset( (void*)&datapath, 0, sizeof( datapath ) );

#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
   if ( (ds_3gpp_csd_tds_tbl != NULL) && (SYS_SYS_MODE_TDS ==  ds_ucsd_network_mode) )
#else
   if ( SYS_SYS_MODE_TDS ==  ds_ucsd_network_mode)
#endif /* FEATURE_SEGMENT_LOADING */
   {
     memset( (void*)&tds_cs_data_parms,0,sizeof(tdsrrc_cs_data_parms_type) );
     memset( (void*)&tds_lc_info, 0, sizeof( tdsrrc_user_plane_lc_info_type ) );

     tds_lc_info.rab_id = rab_id;

#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
     if ( ITDSCDMA_tdsrrc_return_lc_info_for_rab_sub(ds_3gpp_csd_tds_tbl, subs_id, 
                                                 &tds_lc_info ) == TDSRRC_RAB_NOT_FOUND )
#else
     if ( ITDSCDMA_tdsrrc_return_lc_info_for_rab(ds_3gpp_csd_tds_tbl, &tds_lc_info ) == TDSRRC_RAB_NOT_FOUND )
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
     if ( tdsrrc_return_lc_info_for_rab_sub(subs_id, &tds_lc_info ) == TDSRRC_RAB_NOT_FOUND )
#else
     if ( tdsrrc_return_lc_info_for_rab( &tds_lc_info ) == TDSRRC_RAB_NOT_FOUND )
#endif
#endif /* FEATURE_SEGMENT_LOADING */
     {
       DATA_MSG1_ERROR("Invalid RAB id %d", rab_id);
       return ( DS3G_FAILURE );
     }
     if ( ( tds_lc_info.num_ul_lc_ids_for_rab > 1 ) ||
          ( tds_lc_info.num_dl_lc_ids_for_rab > 1 ) )
     {
       MSG_ERROR( "More than 1 LC id: RABid %d; UL %d; DL %d",
                  rab_id,
                  tds_lc_info.num_ul_lc_ids_for_rab,
                  tds_lc_info.num_dl_lc_ids_for_rab );

       return ( DS3G_FAILURE );
     }
     /* Get the SDU size (in bits) and number of SDUs to send per TTI. */
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
     if ( ITDSCDMA_tdsrrc_return_cs_data_call_parms_sub( ds_3gpp_csd_tds_tbl,
	    subs_id, tds_lc_info.rab_id, &tds_cs_data_parms ) == TDSRRC_STATUS_FAILURE )
#else
     if ( ITDSCDMA_tdsrrc_return_cs_data_call_parms( ds_3gpp_csd_tds_tbl,
	    tds_lc_info.rab_id, &tds_cs_data_parms ) == TDSRRC_STATUS_FAILURE )
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
     if ( tdsrrc_return_cs_data_call_parms_sub( subs_id, tds_lc_info.rab_id, 
                                            &tds_cs_data_parms ) == 
          TDSRRC_STATUS_FAILURE )
     
#else
     if ( tdsrrc_return_cs_data_call_parms( tds_lc_info.rab_id, 
                                            &tds_cs_data_parms ) == 
          TDSRRC_STATUS_FAILURE )
#endif
#endif /* FEATURE_SEGMENT_LOADING */
     {
       DATA_MSG1_ERROR("Failed to get RAB info for RAB ID %d",
                  tds_lc_info.rab_id);

       return ( DS3G_FAILURE );
     }
     /* Check if the SDU size is valid. */
     if ( ( tds_cs_data_parms.sdu_size % NUM_BITS_PER_BYTE ) != 0 )
     {
       DATA_MSG1_ERROR("Invalid SDU size: %d bits", tds_cs_data_parms.sdu_size);

       return ( DS3G_FAILURE );
     }

     bytes_per_sdu = call_info_ptr->chan_info.sdu_size
                   = (tds_cs_data_parms.sdu_size / NUM_BITS_PER_BYTE);
     sdus_per_tti  = call_info_ptr->chan_info.sdus_per_tti
                   = tds_cs_data_parms.num_sdu_per_tti;
   }
   else
#endif /* FEATURE_TDSCDMA */
   {
#ifdef FEATURE_SEGMENT_LOADING
   if(ds_3gpp_csd_wcdma_tbl == NULL)
   {
     DATA_MSG0_ERROR("In ds_ucsd_connect_l2_interface WCDMA not active");
     return ( DS3G_FAILURE );
   }
#endif /* FEATURE_SEGMENT_LOADING */

      memset( (void*)&cs_data_parms,0,sizeof(rrc_cs_data_parms_type) );
      memset( (void*)&lc_info, 0, sizeof( rrc_user_plane_lc_info_type ) );

     lc_info.rab_id = rab_id;

#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
     if ( IWCDMA_rrc_return_lc_info_for_rab_sub(ds_3gpp_csd_wcdma_tbl, subs_id, 
                           &lc_info ) ==  RRC_RAB_NOT_FOUND )
#else
     if ( IWCDMA_rrc_return_lc_info_for_rab(ds_3gpp_csd_wcdma_tbl, 
                           &lc_info ) ==  RRC_RAB_NOT_FOUND )
#endif /* FEATURE_DUAL_WCDMA */
#else
#ifdef FEATURE_DUAL_WCDMA
     if ( rrc_return_lc_info_for_rab_sub(subs_id, &lc_info ) == RRC_RAB_NOT_FOUND )
#else
     if ( rrc_return_lc_info_for_rab( &lc_info ) == RRC_RAB_NOT_FOUND )
#endif /* FEATURE_DUAL_WCDMA */
#endif /* FEATURE_SEGMENT_LOADING */
     {
       DATA_MSG1_ERROR( "Invalid RAB id %d", rab_id);
       return ( DS3G_FAILURE );
     }

     if ( ( lc_info.num_ul_lc_ids_for_rab > 1 ) ||
          ( lc_info.num_dl_lc_ids_for_rab > 1 ) )
     {
       MSG_ERROR( "More than 1 LC id: RABid %d; UL %d; DL %d",
                  rab_id,
                  lc_info.num_ul_lc_ids_for_rab,
                  lc_info.num_dl_lc_ids_for_rab );

       return ( DS3G_FAILURE );
     }

     /* Get the SDU size (in bits) and number of SDUs to send per TTI. */
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
     if ( IWCDMA_rrc_return_cs_data_call_parms_sub(ds_3gpp_csd_wcdma_tbl, subs_id, 
          lc_info.rab_id, &cs_data_parms ) == RRC_STATUS_FAILURE )
#else
     if ( IWCDMA_rrc_return_cs_data_call_parms(ds_3gpp_csd_wcdma_tbl, 
          lc_info.rab_id, &cs_data_parms ) == RRC_STATUS_FAILURE )
#endif /* FEATURE_DUAL_WCDMA */

#else

#ifdef FEATURE_DUAL_WCDMA
     if ( rrc_return_cs_data_call_parms_sub(subs_id, lc_info.rab_id, 
                  &cs_data_parms ) ==  RRC_STATUS_FAILURE )
#else
     if ( rrc_return_cs_data_call_parms( lc_info.rab_id, 
                  &cs_data_parms ) ==  RRC_STATUS_FAILURE )
#endif /* FEATURE_DUAL_WCDMA */
     
#endif /* FEATURE_SEGMENT_LOADING */
     {
       DATA_MSG1_ERROR( "Failed to get RAB info for RAB ID %d",
                  lc_info.rab_id);

       return ( DS3G_FAILURE );
     }

     /* Check if the SDU size is valid. */
     if ( ( cs_data_parms.sdu_size % NUM_BITS_PER_BYTE ) != 0 )
     {
       DATA_MSG1_ERROR( "Invalid SDU size: %d bits", cs_data_parms.sdu_size);

       return ( DS3G_FAILURE );
     }

     bytes_per_sdu = call_info_ptr->chan_info.sdu_size
                   = (cs_data_parms.sdu_size / NUM_BITS_PER_BYTE);
     sdus_per_tti  = call_info_ptr->chan_info.sdus_per_tti
                   = cs_data_parms.num_sdu_per_tti;
   }

   /* Set L2 datapath based on call type */
   if( IS_VIDEO_CALL( call_info_ptr->call_type ) || /*lint !e655 */
       IS_CALL_TYPE( call_info_ptr->call_type, DS_UCSD_SYNC_CSD_CALL ) ) /*lint !e655 */
   {
     /*  Number of SDUs per TTI, and bytes per sdu is set by the network.
     **  bytes per sdu is always 80, but there are 20 and 40 ms networks.
     **  resulting in 2 or 4 sdu per tti calls, respectively.
     **  Modem needs to push down correct number of sdus for a tti,
     **  otherwise MAC throws the data away. QVP needs the buffer 
     **  to move not more than two TTI's worth of data. So high count and 
     **  DNE count is set thrice of high.
     */
     ds_ucsd_to_l2_wm.lo_watermark          = 
             sdus_per_tti * call_info_ptr->chan_info.sdu_size;
     ds_ucsd_to_l2_wm.hi_watermark          = 
             2 * sdus_per_tti * call_info_ptr->chan_info.sdu_size;
     ds_ucsd_to_l2_wm.dont_exceed_cnt       = 
             3 * sdus_per_tti * call_info_ptr->chan_info.sdu_size;

     /* For VT use datapath specified by client */
     datapath = call_info_ptr->dpath_params;
   }
   else
   {
     /* Asynchrous calls use shim watermarks */
     ds_ucsd_mpshim_get_rlc_datapath( &datapath );
   }
    
   /* Register the watermarks with WCDMA interface */
   dswcsd_register_rlc_watermarks( datapath.ul_wm_ptr,
                                   datapath.dl_wm_ptr );

   /* Single processor targets */
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
   if ( (ds_3gpp_csd_tds_tbl != NULL) && (SYS_SYS_MODE_TDS ==  ds_ucsd_network_mode))
   {
#ifdef FEATURE_DUAL_WCDMA
     if( ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_csd_tds_tbl, subs_id) == TRUE )
#else
     if( ITDSCDMA_rrc_is_tds_active(ds_3gpp_csd_tds_tbl) == TRUE )
#endif
#else
   if ( SYS_SYS_MODE_TDS ==  ds_ucsd_network_mode)
   {
#ifdef FEATURE_DUAL_WCDMA
     if(rrc_is_tds_active_sub(subs_id) == TRUE)
#else
     if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
     {
       /* Configure interface with Layer 2 - TDCDMA RLC*/
       if( TRUE != ds_ucsd_mpshim_register_tdscdma_rlc( &tds_lc_info,
                                                      &datapath, subs_id ) )
       {
         DATA_MSG0_ERROR("Problem establishing TD-SCDMA RLC interface");
         return FALSE;
       }
     }
     else
     {
       DATA_MSG0_ERROR("UCSD mode is TDS, but RRC_is_tds_active returned FALSE");
       return FALSE;
     }
   }
   else
#endif /* FEATURE_TDSCDMA */
   {
     /* Configure interface with Layer 2 - WCDMA RLC*/
     if( TRUE != ds_ucsd_mpshim_register_wcdma_rlc( &lc_info,
                                                    &datapath,
			                            subs_id ) )
     {
       DATA_MSG0_ERROR("Problem establishing WCDMA RLC interface");
       return FALSE;
     }
   }
         
   /* Derive data rate from sdus_per_tti */
   switch (sdus_per_tti)
   {
   case 1:
    call_info_ptr->chan_info.data_rate = DS_UCSD_RATE_14400;
    REPORT_RATE_ON_CONNECT( ATCOP_WAIUR_REPORT_14400,
                            call_info_ptr->bearer_params.cbst_info.speed,
                            call_info_ptr->bearer_params.cbst_info.name,
                            call_info_ptr->bearer_params.cbst_info.connection_element );
     break;

   case 2:
     if ( DS_UCSD_ASYNC_CSD_CALL == call_info_ptr->call_type )
     {
       /* Asynchronous call (assumes 72 byte frames @ 40ms TTI) */
       call_info_ptr->chan_info.data_rate = DS_UCSD_RATE_28800;
       REPORT_RATE_ON_CONNECT( ATCOP_WAIUR_REPORT_28800,
                               call_info_ptr->bearer_params.cbst_info.speed,
                               call_info_ptr->bearer_params.cbst_info.name,
                               call_info_ptr->bearer_params.cbst_info.connection_element );
     }
     else
     {
       /* Synchronous call (assumes 80 byte frames @ 20ms TTI) */
       call_info_ptr->chan_info.data_rate = DS_UCSD_RATE_64000;
       if(bytes_per_sdu > UL_DATA_BUF_SIZ)
       {
         DATA_MSG2_ERROR("No. of bytes_per_sdu:%d for this call info is invalid for SDUS_PER_TTI=%d", 
                          bytes_per_sdu, sdus_per_tti);
         return ( DS3G_FAILURE );
       }
       REPORT_RATE_ON_CONNECT( ATCOP_WAIUR_REPORT_64000,
                               call_info_ptr->bearer_params.cbst_info.speed,
                               call_info_ptr->bearer_params.cbst_info.name,
                               call_info_ptr->bearer_params.cbst_info.connection_element );
     }
     break;

   case 4:
     call_info_ptr->chan_info.data_rate = DS_UCSD_RATE_57600;
     REPORT_RATE_ON_CONNECT( ATCOP_WAIUR_REPORT_57600,
                             call_info_ptr->bearer_params.cbst_info.speed,
                             call_info_ptr->bearer_params.cbst_info.name,
                             call_info_ptr->bearer_params.cbst_info.connection_element );
     break;

   default:
     DATA_MSG1_ERROR("Bad WCDMA Async SDUS_PER_TTI=%d",sdus_per_tti);
     return ( DS3G_FAILURE );
   }
   
   /* Synchronous call specific processing (Async call handled after
    * lower-layers setup) */
   if ( IS_CALL_TYPE( call_info_ptr->call_type, DS_UCSD_SYNC_CSD_CALL ) ||       /*lint !e655 */
        IS_CALL_TYPE( call_info_ptr->call_type, DS_UCSD_VIDEO_TELEPHONY_CALL ) ) /*lint !e655 */
   {
     /* Check for external datapath */
     if ( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface ) 
     {
       /* Initialize call-related information for WCDMA UL & DL tasks */
       dswcsd_init_call_info( bytes_per_sdu, 
                              (uint32)sdus_per_tti,
                              data_bc_ptr->number_of_data_bits,
                              data_bc_ptr->number_of_stop_bits,
                              data_bc_ptr->parity,
                              data_bc_ptr->fixed_network_user_rate,
                              call_info_ptr->network_mode,
                              call_info_ptr->cm_call_id,
                              ds_ucsd_set_data_counters );
     }

     /* Check for external call control */
     if ( DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface ) 
     {
       /* Notify TE only if not handover scenario */
       if( !is_handover )
       {
         /*------------------------------------------------------------------
           Send CONNECT result code to the TE before registering the Uplink 
           and Downlink watermarks with RLC. If the RLC watermarks are 
           registered before CONNECT is sent, data may be sent to the TE 
           before CONNECT is sent. 
           ------------------------------------------------------------------*/
         dsat_send_result_code( DSAT_CONNECT );
         
         /* Set Carrier Detect signal to tell TE the call is up */
         (void)ds3g_siolib_ex_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_ON,
                                            call_info_ptr->sio_port_id);
       }
     }
   } /* Non-VT call */

   MSG_HIGH( "Channel config: %d bytes per SDU, %d SDU per TTI, data rate %d",
             call_info_ptr->chan_info.sdu_size,
             call_info_ptr->chan_info.sdus_per_tti,
             call_info_ptr->chan_info.data_rate );
   
   return ( DS3G_SUCCESS );
} /* ds_ucsd_connect_l2_interface() */



/*===========================================================================

FUNCTION ds_ucsd_deregister_l2_interface

DESCRIPTION
   This function deregisters the interface with WCDMA RLC.
        
DEPENDENCIES
   None
  
RETURN VALUE
   TRUE on successful processing; FALSE otherwise.
    
SIDE EFFECTS
   Command sent to L2 UL & DL tasks

===========================================================================*/
LOCAL boolean
ds_ucsd_deregister_l2_interface 
(
  sys_modem_as_id_e_type subs_id
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Deregister interface with Layer 2 - TD_SCDMA RLC */
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_DUAL_WCDMA
   if( (ds_3gpp_csd_tds_tbl != NULL) && 
       (ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_csd_tds_tbl, subs_id) == TRUE) )
#else
  if( (ds_3gpp_csd_tds_tbl != NULL) && (ITDSCDMA_rrc_is_tds_active(ds_3gpp_csd_tds_tbl) == TRUE) )
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
   if(rrc_is_tds_active_sub(subs_id) == TRUE)
#else
  if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
  {
    if( TRUE != ds_ucsd_mpshim_deregister_tdscdma_rlc(subs_id) )
    {
      DATA_MSG0_ERROR("Problem releasing TD_SCDMA RLC interface");
      return FALSE;
    }
  }
  else 
#endif /* FEATURE_TDSCDMA */
  {
    /* Deregister interface with Layer 2 - WCDMA RLC */
    if( TRUE != ds_ucsd_mpshim_deregister_wcdma_rlc(subs_id) )
    {
      DATA_MSG0_ERROR("Problem releasing WCDMA RLC interface");
      return FALSE;
    }
  }
  return ( TRUE );
} /* ds_ucsd_deregister_l2_interface() */

#endif /* FEATURE_DATA_WCDMA_CS */



/*===========================================================================

FUNCTION ds_ucsd_check_concurrent_calls

DESCRIPTION
  This function compares the existing calls in the call instance table
  against the proposed new call type.  If the new call would exceed
  the limit of concurrent calls, the new call should be rejected.
  Supporting concurrent CS Data calls requires multiple CS RAB support
  in access stratum and orthogonal data paths.

  Call instance switch pointer is set if a call instance exists but has
  not connected.  In this case, the call being established can be
  ended to enable the new incoming call.
  
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if concurrent call limits are not exceeded; FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean
ds_ucsd_check_concurrent_calls
(
  ds_ucsd_call_type             new_call_type,   /* Call type for new call */
  ds_ucsd_call_dir_e_type       direction,       /* Call direction         */
  ds_ucsd_call_tbl_entry_type **switch_inst_pptr /* Instance to switch     */
)
{
  boolean result = TRUE;
  uint8 index;
  int32 call_type_count[DS_UCSD_CALL_INDEX_MAX];
  int8 call_total;
  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Verify call type known */
  if( DS_UCSD_UNKNOWN_CALL == new_call_type)
  {
    return FALSE;
  }

  /* Mobile originated calls have instance allocated at start of
   * processing so table scan will see itself. Terminated calls have
   * instance allocated only after this validation. To account for
   * difference, initialize counters with one offset. */
  call_total = ( DS_UCSD_CALL_DIR_ORIGINATED == direction )? -1 : 0;
  memset((void*)call_type_count, call_total, sizeof(call_type_count));
  
  /* Loop over call instance table */
  for ( index = 0; 
        index < MAX_UCSD_CALL_TBL_ENTRIES; 
        index++ ) 
  {
    /* Count all allocated instances. */
    if( ds_ucsd_call_tbl_ptr[ index ] != NULL )
    {
      uint8 type;

      /* Increment total call counter */
      call_total++;
      
      /* Loop over each call type */
      /*lint -e641 */
      for( type  = DS_UCSD_CALL_INDEX_ASYNC;
           (type <= DS_UCSD_CALL_INDEX_VT) && result;
           type++ )
      {
        /* Check for matching call type */
        if( 0 != (ds_ucsd_call_tbl_ptr[ index ]->call_type & MAKE_CALL_TYPE( type ) ) )
        {
          /* Increment call type on match */
          call_type_count[ type ]++;

          /* Compare against call limit table */
          if( ( new_call_type == MAKE_CALL_TYPE( type ) ) && 
              ( ds_ucsd_call_limit_info[ type ] <= call_type_count[ type ] ) )
          {
            /* Limit would be exeeced is new call allowed, return faulure. */
            MSG_ERROR("Concurrent limits exceeded: call ID %d inst ID %d type %d",
                      ds_ucsd_call_tbl_ptr[ index ]->cm_call_id,
                      ds_ucsd_call_tbl_ptr[ index ]->inst_id,
                      ds_ucsd_call_tbl_ptr[ index ]->call_type );
            result = FALSE;
            break;
          }
        }
      }
      /*lint +e641 */

      /* Check to see if call switch may be possible. */
      /* Existing call instance can be switched to another incoming
       * call ID only if it has not reached connected state. */
      if( (NULL != switch_inst_pptr) &&
          (DS_UCSD_CALL_DIR_TERMINATED == direction) &&
          (DS_UCSD_CALL_STATE_CONNECT_LL != ds_ucsd_call_tbl_ptr[ index ]->state) &&
          (DS_UCSD_CALL_STATE_CONNECT    != ds_ucsd_call_tbl_ptr[ index ]->state) )
      {
        *switch_inst_pptr = ds_ucsd_call_tbl_ptr[ index ];
      }
    } /* if ds_ucsd_call_tbl_ptr[ index ].used */
  }

  /* Finally check that the total number of calls is less than maximum
   * supported. */
  if( TRUE == result )
  {
    uint8 call_limit;

    /* Need to reserve one slot for incoming (VT|Voice) call if
     * another VT call is active to permit propogating events to CSD
     * API client. */
    call_limit =
      ( (DS_UCSD_CALL_DIR_TERMINATED == direction) &&
        (0 < call_type_count[ DS_UCSD_CALL_INDEX_VT ]) &&
        ( (DS_UCSD_VIDEO_TELEPHONY_CALL|DS_UCSD_VOICE_CALL) & new_call_type ) )? /*lint !e655 */
      SYS_MAX_UCSD_CALLS : (SYS_MAX_UCSD_CALLS-1);
  
    if( call_total >= call_limit )
    {
      DATA_MSG0_ERROR("Total number of supported call instances exceeded");
      result = FALSE;
    }
  }

  return result;
} /* ds_ucsd_check_concurrent_calls() */



/*===========================================================================

FUNCTION ds_ucsd_allocate_call_instance

DESCRIPTION
  This function allocates a record in the call instance table for the
  current call.  If there is no free record, a error condintion is
  returned.  The bearer capability for the preferred mode of the call
  is passed to initialze the call instance.

  Note there is a possible race conditions for call collisions when we
  have an outgoing call and an incoming call setup at the same time,
  assuming there is no more than one incoming or outgoing call at the
  same time.  Call collision may happen here if the outgoing call is
  made via the API by a CS Data client that has priority over the DS
  task and it may have obtained a call table entry after DSMGR decides
  to call ds_ucsd_setup_call_handler(). We need to detect the call
  collision (mismatch in CS Data call instance ID) in
  ds_ucsd_process_app_originate_call_cmd() and end the outgoing call
  there.  Here is the call flow:
  
    CSD client           CSD API                  Mode-Specific Handler                       DSMGR            CM
    |                      |                                |                                   |              |
    |                      |                                |                                   | EVENT_SETUP  |
    |                      |                                |                                   |<-------------|
    |                      |                                | setup_call_handler()              |              |
    |                      |                                |<----------------------------------|              |
    <<context switch>>     |                                |                                   |              |
    | app_originate_call() |                                |                                   |              |
    |--------------------->|                                |                                   |              |
    |                      | get_instid()                   |                                   |              |
    |                      |------------------------------->|                                   |              |
    |                      | (call_tbl[0].used = TRUE)      |                                   |              |
    |                      | (call_tbl[0].inst_id = 1)      |                                   |              |
    |                      |                                |                                   |              |
    |                      | APP_ORIG_CALL_CMD              |                                   |              |
    |                      |------------------------------->|                                   |              |
    |                      | (orig_params.inst_id = 1)      |                                   |              |
    <<context switch>>     |                                |                                   |              |
    |                      |                                | setup_call_handler()              |              |
    |                      |                                | continues execution:              |              |
    |                      |                                |  call_tbl[0].used = FALSE;        |              |
    |                      |                                |  call get_instid()                |              |
    |                      |                                |<---------                         |              |
    |                      |                                | (call_tbl[0].used = TRUE)         |              |
    |                      |                                | (call_tbl[0].inst_id = 2)         |              |
    |                      | setup_call_cb(inst_id=2)       |                                   |              |
    |<---------------------|--------------------------------|                                   |              |
    |                      |                                | process_app_originate_call_cmd(): |              |
    |                      |                                |  orig_params.inst_id !=           |              |
    |                      |                                |  call_tbl[0].inst_id              |              |
    |                      |                                |----------                         |              |
    |                      |                                |                                   |              |
    |                      | call_end_cb(inst_id = 1)       |                                   |              |
    |<---------------------|--------------------------------|                                   |              |
  
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if allocation successful; FALSE otherwise.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean
ds_ucsd_allocate_call_instance
(
  const cm_call_id_type              call_id,          /* CM call ID       */
  const cm_bearer_capability_s_type *bc_ptr,
                            /* Bearer Capability for preferred mode of call */
  const cm_gw_cs_call_info_s_type   *mode_info_ptr,    /* New mode info    */
  ds_ucsd_call_tbl_entry_type ** call_info_pptr
                                       /* Returned pointer to call instance */
)
{
  boolean result = TRUE;
  uint8   index;
  const cm_bearer_capability_s_type  *data_bc_ptr;
  ds_ucsd_call_type call_type = DS_UCSD_UNKNOWN_CALL;
  ds_ucsd_call_tbl_entry_type  *switch_ptr = NULL;
  dsat_num_item_type tmp_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != bc_ptr );
  ASSERT( NULL != mode_info_ptr );
  ASSERT( NULL != call_info_pptr );
  
  /* Determine the internal CS Data call type from bearer capability info. */
  call_type =
    ds_ucsd_determine_call_type( bc_ptr, NULL, NULL, &data_bc_ptr, NULL );
  
  /* Check that the new call will not exceed concurrent call limits. */
  if( TRUE != ds_ucsd_check_concurrent_calls( call_type,
                                              DS_UCSD_CALL_DIR_TERMINATED,
                                              &switch_ptr ) )
  {
    /* Check for possibility to switch existing call instance to incoming call. */
    if( NULL != switch_ptr )
    {
      cm_call_id_type prev_call_id = switch_ptr->cm_call_id;
      MSG_HIGH( "Switching incoming call ID %d to prev call ID: %d inst ID: %d",
                call_id, prev_call_id, switch_ptr->inst_id );
      
      /* Cleanup existing call. This is analogous to end call
       * processing but we do it here to free up the resources for
       * incoming call rather than wait for call end event. As CM call
       * ID will be cleared from the instance table, call end handler
       * will not do anything. */
      result = ds_ucsd_perform_call_switch( switch_ptr,
                                            mode_info_ptr,
                                            data_bc_ptr,
                                            call_type );

      /* Indicate stack is triggering call teardown.  This will ensure
       * no delay in call end during user end handler given switch
       * handler triggered low-layer cleanup.*/
      switch_ptr->protocol_stack_termination = TRUE;
      
      /* Tell 3G DSMGR to end the existing call */
      if ( DS3G_FAILURE == ds3g_msh_hangup_call_ex( prev_call_id, NULL,
#ifdef FEATURE_DUAL_SIM
                                                   ds_ucsd_subs_id
#else
                                                 SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
         ))
      {
        DATA_MSG1_ERROR( "Failed to end the call on switch: %d", prev_call_id);
      }

      /* Free the call instance */
      (void)ds_ucsd_release_call_instance( switch_ptr->inst_id );
    }
    else
    {
      /* New call of this type cannot be supported. */
      result = FALSE;
    }
  }
  
  /* Proceed with new call instance */
  if( TRUE == result )
  {
    /* Get a CS Data call instance ID and a call table entry.  */
    /* Instance ID is seeded into call record. */
    if ( NO_CALL_ID == ds_ucsd_get_instid( &index ) )
    {
      /* Cannot get a CS Data call ID */
      result = FALSE;
      call_info_pptr = NULL;
    }
    else
    {
      ds_ucsd_bearer_service_type  bearer_service;
      tmp_val = (dsat_num_item_type) dsatutil_get_val( DSATETSI_EXT_CHSN_IDX,
                                       0,ATCOP_CHSN_WAIUR_POSITION,NUM_TYPE);

      /* Setup bearer configuration defaults from ATCOP */
      ds_ucsd_store_bearer_service_type(
        &bearer_service,
        (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_SPEED_POSITION),
        (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_NAME_POSITION),
        (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_CE_POSITION),
        (uint8)tmp_val );
      
      /* Initialize the call instance with setup parameters */
      *call_info_pptr = ds_ucsd_call_tbl_ptr[ index ];

      /* Store the CM call ID */
      (*call_info_pptr)->cm_call_id = call_id;
      (*call_info_pptr)->call_dir = DS_UCSD_CALL_DIR_TERMINATED;
  
      /* Store call information */
      ds_ucsd_store_call_info( index,
                               ds_ucsd_network_mode,
#ifdef FEATURE_DUAL_SIM
                               ds_ucsd_subs_id,
#endif /* FEATURE_DUAL_SIM */
                               call_type,
                               &bearer_service,
                               NULL,
                               NULL,
                               NULL );
      
      (*call_info_pptr)->ota_rate =
        ds_ucsd_map_fnur_waiur_to_ota_rate( bc_ptr->connection_element,
                                            ds_ucsd_network_mode,
                                            bc_ptr->fixed_network_user_rate,
                                            bc_ptr->wanted_air_interface_user_rate );
      (*call_info_pptr)->modify_allowed = mode_info_ptr->is_call_mod_possible;

      /* Add cross-reference for instance to CM call ID */
      result = ds_ucsd_set_call_inst(call_id, (*call_info_pptr)->inst_id);
    }
  }
  
  return result;
} /* ds_ucsd_allocate_call_instance() */


/*===========================================================================
FUNCTION  DS_UCSD_SYS_CHANGE_HANDLER

DESCRIPTION
  This function handles the notification of an Inter RAT change
  For all calls that are active, inform DS manager to change the mode
  of the call so that it changes it in its internal call table entries
  
  Called by 3G DSMGR when the serving system changes. 
  
  For all calls that are active/pending,
  If the mode changes between WCDMA <-> GSM/GPRS, we notify DS manager
  to change the mode of the new mode.
  
  If the new mode is not WCDMA / GSM-GPRS, then do nothing : we will wait
  for call end to arrive from CM.              
  
PARAMETERS  
  None.
      
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL void ds_ucsd_sys_change_handler
(
 sys_modem_as_id_e_type        subs_id,
 sys_sys_mode_e_type           new_mode,
 sys_sys_mode_e_type           old_mode
)
{
  unsigned  call_instance;
  static sys_sys_mode_e_type prev_new_mode = SYS_SYS_MODE_NONE;
  
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the new mode has not changed from last report, do nothing
  -------------------------------------------------------------------------*/
  if( new_mode == prev_new_mode )
  {
    return;
  }
  prev_new_mode = new_mode;

  DATA_MSG2_HIGH("CS Sys change handler new mode:%d old mode:%d",new_mode,old_mode);

  /*-------------------------------------------------------------------------
    If the new mode is not WCDMA or GSM, do nothing
  -------------------------------------------------------------------------*/
  if( (!(DS_UCSD_IS_WCDMA_TDS_MODE(new_mode))) &&
      ( new_mode != SYS_SYS_MODE_GSM) )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    For all the calls that have a valid callid, request DS to change the mode.
  -------------------------------------------------------------------------*/
  for( call_instance = 0;
       call_instance < MAX_UCSD_CALL_TBL_ENTRIES ;
       call_instance++)
  {
    if( (ds_ucsd_call_tbl_ptr[call_instance] != NULL) &&
        (ds_ucsd_call_tbl_ptr[call_instance]->cm_call_id != CM_CALL_ID_INVALID) )
    {
#ifdef FEATURE_DUAL_SIM
      if ( ds_ucsd_call_tbl_ptr[call_instance]->subs_id == subs_id)
#endif /* FEATURE_DUAL_SIM */
      {
#ifndef FEATURE_DATA_GCSD
        if (new_mode == SYS_SYS_MODE_GSM )
        {
          DATA_MSG1_HIGH("No CSD Support in GSM, Cleanup Call_id:%d",
                                 ds_ucsd_call_tbl_ptr[call_instance]->cm_call_id);
           /* Tell 3G DSMGR to end the call */
          if ( DS3G_FAILURE == ds3g_msh_hangup_call_ex( 
                                  ds_ucsd_call_tbl_ptr[call_instance]->cm_call_id,
                                  NULL,
#ifdef FEATURE_DUAL_SIM
                                  ds_ucsd_call_tbl_ptr[call_instance]->subs_id
#else
                                  SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                              ) )
         {
            DATA_MSG2_ERROR("Failed to end the call: "
                 "CM call ID = %d; CS Data call instance ID = %d", 
                 ds_ucsd_call_tbl_ptr[call_instance]->cm_call_id, 
                 ds_ucsd_call_tbl_ptr[call_instance]->inst_id);
         }
         continue;           
       }           
 #endif /* FEATURE_DATA_GCSD */       
   
        MSG_HIGH("Changing mode for call :inst:%d :call id:%d :mode:%d",
                   call_instance,
                   ds_ucsd_call_tbl_ptr[call_instance]->cm_call_id, new_mode);

        if( !ds3g_msh_set_new_call_mode(ds_ucsd_call_tbl_ptr[call_instance]->cm_call_id,
                                        new_mode) )
        {
          DATA_MSG1_ERROR("Mode set failed for inst :%d", call_instance);
        }
      }
    }
  }
} /* ds_ucsd_sys_change_handler */



/*===========================================================================
FUNCTION  DS_UCSD_GET_GSM_CHANNEL_INFO

DESCRIPTION
  This function queries the GSM L1 dor channel characteristices.
  The passed structure is populated based on the results.

PARAMETERS  
  None.
      
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE on successful operation, FALSE otherwise.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
LOCAL boolean ds_ucsd_get_GSM_channel_info
(
  ds_ucsd_call_tbl_entry_type        *call_info_ptr
)
{
  boolean result = TRUE;
  channel_mode_T  chmode;
  ds_ucsd_ota_channel_params_T* chan_params_ptr = &(call_info_ptr->chan_info);
  
  ASSERT( NULL != call_info_ptr );

/* Query GSM L1 for channel characteristics */
#ifdef FEATURE_DUAL_SIM
  chmode = gl1_get_current_ded_chan_type(call_info_ptr->subs_id);
#else
  chmode = gl1_get_current_ded_chan_type();
#endif /* FEATURE_DUAL_SIM */

  /* Convert channel mode to data rate */
  switch (chmode)
  {
  case DATA_9600:
    chan_params_ptr->data_rate = DS_UCSD_RATE_9600;
    break;
      
  case DATA_14400:
    chan_params_ptr->data_rate = DS_UCSD_RATE_14400;
    break;
      
  default:
    DATA_MSG1_ERROR("Unsupported GSM L1 mode: %d", chmode);
    result = FALSE;
  }

  /* Constant 1 SDU every TTI */
  chan_params_ptr->sdus_per_tti = 1;

  return result;
} /* ds_ucsd_get_GSM_channel_info() */


#ifdef FEATURE_DATA_UCSD_HANDOVER

/*===========================================================================

FUNCTION ds_ucsd_handover_notify

DESCRIPTION
  This function is a callback routine for NAS/MN to notify DS of RAT
  handover and RAB change events.  The handover direction (reason) and
  new channel RAB identifier are passed from MN.

DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  Command is enqueued to DS task.

===========================================================================*/
void
ds_ucsd_handover_notify
(
  sync_enum_T reason,
  uint8       rab_id
)
{
  ds_cmd_type                *cmd_buf = NULL;
  ds_ucsd_handover_cmd_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_MED ("NAS(MN)->UCSD notification: reason=%d RAB=%d",
           reason, rab_id, 0);

  /* Filter events of interest */
  switch (reason)
  {
  case WCDMA_GSM_HANDOVER:
  case GSM_WCDMA_HANDOVER:
  case GSM_GSM_HANDOVER:
      /* Get a command buffer for DS task */
    cmd_buf = ds_allocate_cmd_buf(sizeof(ds_ucsd_handover_cmd_type));
    if( (NULL == cmd_buf) || (NULL == cmd_buf->cmd_payload_ptr) )
    {
      return;
    }
    else
    {
      /* Prepare message parameters */
      cmd_buf->hdr.cmd_id = DS_CMD_3GPP_UCSD_HANDOVER;
      data_ptr = (ds_ucsd_handover_cmd_type*)cmd_buf->cmd_payload_ptr;
      data_ptr->reason = reason;
      data_ptr->rab_id = rab_id;
      /* Post message to DS task */
      ds_put_cmd(cmd_buf);
    }
    break;

  default:
      /* Ignore event */
      break;
  }

} /* ds_ucsd_handover_notify() */



/*===========================================================================

FUNCTION ds_ucsd_nt_handover_update_stack_interface

DESCRIPTION
  This function updates the OTA stack interface based on handover direction.
  The WCDMA interface is registered or deregistered as required.  The GSM
  channel parameters are determined by query to L1.

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE if processing successful; FALSE otherwise
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean
ds_ucsd_nt_handover_update_stack_interface
(
  ds_ucsd_call_tbl_entry_type        *call_info_ptr,
  const ds_cmd_type                  *cmd_ptr,
  const cm_bearer_capability_s_type  *data_bc_ptr
)
{
  boolean                     result = TRUE;
  ds_ucsd_handover_cmd_type  *data_ptr = NULL;
  sys_modem_as_id_e_type subs_id = ds3g_get_cs_subs_id();  
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( NULL != call_info_ptr );
  ASSERT( NULL != cmd_ptr );
  ASSERT( NULL != cmd_ptr->cmd_payload_ptr );
  ASSERT( NULL != data_bc_ptr );

#ifdef FEATURE_DUAL_SIM
  subs_id = call_info_ptr->subs_id;
#endif /* FEATURE_DUAL_SIM */

  data_ptr = (ds_ucsd_handover_cmd_type*)cmd_ptr->cmd_payload_ptr;
  
  DATA_MSG1_MED("Handover update stack interface: reason=%d",
           data_ptr->reason);

  switch ( data_ptr->reason )
  {
  case WCDMA_GSM_HANDOVER:
    /* Deregister WCDMA RLC interface */
    result = ds_ucsd_deregister_l2_interface(subs_id);
    /* Intentional fall through to next case */
    /*lint -fallthrough */
    
  case GSM_GSM_HANDOVER:
    if( TRUE !=
        ds_ucsd_get_GSM_channel_info(call_info_ptr) )
    {
      DATA_MSG0_ERROR("Failed to get channel characteristics");
      result = FALSE;
    }
    else
    {
      uint32 report_rate = (DS_UCSD_RATE_9600 ==
                            call_info_ptr->chan_info.data_rate)?
                           ATCOP_WAIUR_REPORT_9600 :
                           ATCOP_WAIUR_REPORT_14400;
      REPORT_RATE_ON_CONNECT( report_rate,
                              call_info_ptr->bearer_params.cbst_info.speed,
                              call_info_ptr->bearer_params.cbst_info.name,
                              call_info_ptr->bearer_params.cbst_info.connection_element );
    }
    break;

  case GSM_WCDMA_HANDOVER:
#if defined(FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
    /* Register interface with WCDMA/TD-SCDMA RLC */
    result = ds_ucsd_connect_l2_interface( call_info_ptr,
                                           data_bc_ptr,
                                           data_ptr->rab_id,
                                           TRUE );
#endif /* defined(FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA) */
    break;
    
  default:
    DATA_MSG1_ERROR("Unsupported handover reason: %d",
               data_ptr->reason);
    result = FALSE;
  }

  return result;
} /* ds_ucsd_nt_handover_update_stack_interface() */



/*===========================================================================

FUNCTION DS_UCSD_PROCESS_HANDOVER_CMD

DESCRIPTION
  This function is used to process the DS_UCSD_HANDOVER_CMD command.
  It first suspends data transfer by the GCSD task.  Then the new channel
  charateristics are queired and the data stack is reconfigured.  Data
  transfer is then resumed.
        
DEPENDENCIES
   None
  
RETURN VALUE
   TRUE on successful handover processing; FALSE otherwise.
    
SIDE EFFECTS
  Command enqueued to GCSD task.

===========================================================================*/
LOCAL boolean
ds_ucsd_process_handover_cmd
( 
   const ds_cmd_type  *cmd_ptr
)
{ 
  boolean                     result = TRUE;
  uint8                       index;
  ds_ucsd_handover_cmd_type  *data_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT (NULL != cmd_ptr);
  ASSERT (NULL != cmd_ptr->cmd_payload_ptr);

  data_ptr = (ds_ucsd_handover_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /* Find call instance in the call table.  HO processing applies only
   * to asynch/NT calls at this time. There is only one call of this type. */
  if( MAX_UCSD_CALL_TBL_ENTRIES ==
      (index = ds_ucsd_find_call_instance_by_type( DS_UCSD_ASYNC_CSD_CALL )) )
  {
    DATA_MSG0_ERROR("No call instance of ASYNC type for HO processing");
    result = FALSE;
  }
  else
  {
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
    sync_enum_T reason = data_ptr->reason;

    DATA_MSG1_MED("Process handover cmd: reason=%d",reason);
  
    memset ((void*)&call_info_ptr->chan_info, 0x0,
            sizeof(ds_ucsd_ota_channel_params_T));

    /* Check for connected data path */
    if( (call_info_ptr->state == DS_UCSD_CALL_STATE_CONNECT) ||
        (call_info_ptr->state == DS_UCSD_CALL_STATE_CONNECT_LL) )
    {
      const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
      dsucsd_call_descriptor_type  call_desc;

      /* Find the data bearer for possibly dual-mode call */
      (void)ds_ucsd_determine_call_type(
             &call_info_ptr->neg_cap.bearer_capability_1,
             &call_info_ptr->neg_cap.bearer_capability_2,
             &call_info_ptr->neg_cap.bc_repeat_indicator,
             &data_bc_ptr,
             NULL );
    
      if( NULL == data_bc_ptr )
      {
        MSG_ERROR("Cannot determine data bearer configuration: inst %d",
                  call_info_ptr->inst_id,0,0);
        return FALSE;
      }
     
      DATA_MSG0_MED("Switching CSD LL data path on HO");
    
      SET_CALL_DESCRIPTOR( (&call_desc), call_info_ptr, data_bc_ptr );
    
      /* Suspend data transfer in the GCSD task */
      ds_ucsd_mpshim_send_gcsd_msg( DS_GCSD_SUSPEND_DATA_REQ,
                             call_info_ptr->cm_call_id ); 

      /* Query/modify stack configuration  */
      if (TRUE !=
          ds_ucsd_nt_handover_update_stack_interface( call_info_ptr,
                                                      cmd_ptr,
                                                      data_bc_ptr ))
      {
        DATA_MSG0_ERROR("Failed to modify stack during handover");
        result = FALSE;
      }
      else
      {
        
        /* Change NT call parameters  */
        if (TRUE !=
            ds_ucsd_mpshim_nt_handover_change_config(
                   &call_desc,
                                                      reason, 
                   &call_info_ptr->chan_info ))
        {
          DATA_MSG0_ERROR("Failed to change handover configuration");
          result = FALSE;
        }
      }
    
      if( TRUE == result )
      {
        /* Resume data transfer in the GCSD task */
        ds_ucsd_mpshim_send_gcsd_msg( DS_GCSD_RESUME_DATA_REQ,
                                call_info_ptr->cm_call_id );
      }

      /* Finish HO processing */
      ds_ucsd_mpshim_nt_handover_wrapup( &call_desc, reason, result );
    }
    else
    {
      /* Preserve network mode */
      switch ( reason )
      {
      case GSM_GSM_HANDOVER:
      case WCDMA_GSM_HANDOVER:
        ds_ucsd_network_mode = SYS_SYS_MODE_GSM;
        break;
      
      case GSM_WCDMA_HANDOVER:
        ds_ucsd_network_mode = SYS_SYS_MODE_WCDMA;
        break;
      
      default:
        DATA_MSG1_ERROR("Unsupported handover reason: %d",reason);
        return FALSE;
      }
    }

    DATA_MSG0_MED("Handover command processing done.");
  }
  
  return result;
} /* ds_ucsd_process_handover_cmd() */

#endif /*  FEATURE_DATA_UCSD_HANDOVER */



/*===========================================================================

FUNCTION DS_UCSD_PROCESS_APP_ORIGINATE_CALL_CMD

DESCRIPTION
   This function is to process the DS_UCSD_APP_ORIG_CALL_CMD command from a
   CS Data application. It tries to originate a CS Data call. If the call  
   origination fails, the function will notify the application of the failure.
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void ds_ucsd_process_app_originate_call_cmd
( 
   const ds_cmd_type  *cmd_ptr
)
{ 
   ds_ucsd_call_end_param_type end_param; /* the end reason parameters
                                           * passed in the app call
                                           * back
                                           */
   /* Whether or not to end the call */
   boolean end_call = FALSE;

   /* Reason for ending the call */
   cm_call_end_e_type call_end_cause = CM_CALL_END_INTERNAL_ERROR;

   uint8 index;
   ds_ucsd_app_orig_call_cmd_type *data_ptr = NULL;
   sys_modem_as_id_e_type        subs_id = SYS_MODEM_AS_ID_1;
/*-------------------------------------------------------------------------*/
   
   ASSERT( NULL != cmd_ptr );
   ASSERT( NULL != cmd_ptr->cmd_payload_ptr );

   data_ptr = (ds_ucsd_app_orig_call_cmd_type*)cmd_ptr->cmd_payload_ptr;

  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index =
       ds_ucsd_find_call_instance( data_ptr->inst_id )) )
  {
    /* Mismatch in CS Data call instance ID. End the call. This may
    ** be a call collision. See comments and call flow in
    ** ds_ucsd_incoming_call_handler().
    */
    DATA_MSG1_ERROR( "Invalid CS Data call instance ID %d in APP_ORIG_CALL cmd."
                     " End the call",
                     data_ptr->inst_id);

    end_call = TRUE;
  }
  else
  {
    ds_ucsd_bearer_service_type bearer_service;
    ds_ucsd_call_type           call_type = DS_UCSD_UNKNOWN_CALL;
    
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );

#ifdef FEATURE_DUAL_SIM
    if( ds_ucsd_set_call_mode_and_subs_id(
                data_ptr->call_type,
                &ds_ucsd_network_mode,
                &ds_ucsd_subs_id) )
    {
      DATA_MSG1_HIGH("VT call on subs id %d", (int)ds_ucsd_subs_id);
      /* set the current subs_id in ds3g for CS call origination */
      ds3g_set_cs_subs_id(ds_ucsd_subs_id);
    }
    else
    {
      DATA_MSG1_ERROR("Invalid subs id = %d", ds_ucsd_subs_id);
      end_call = TRUE;
    }
#else
    /* Get the current network mode */
    ds_ucsd_network_mode = ds3g_get_current_cs_domain_mode();
#endif /* FEATURE_DUAL_SIM */

    if( INVALID_3GPP_CS_SYS_MODE(ds_ucsd_network_mode) )
    {
#ifdef FEATURE_3GPP_CSFB
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_TDSCDMA
      if(ds_3gpp_csd_tds_tbl != NULL)
      {
        /* Set the mode currently to TD-SCDMA */
        ds_ucsd_network_mode = SYS_SYS_MODE_TDS;
      }
      else
#endif /* FEATURE_TDSCDMA */
      {
        /* Set the mode currently to WCDMA */
        ds_ucsd_network_mode = SYS_SYS_MODE_WCDMA;
      }
#elif defined(FEATURE_TDSCDMA)
      /* Set the mode currently to TD-SCDMA */
      ds_ucsd_network_mode = SYS_SYS_MODE_TDS;
#else
      /* Set the mode currently to WCDMA */
      ds_ucsd_network_mode = SYS_SYS_MODE_WCDMA;
#endif /* FEATURE_SEGMENT_LOADING */
      DATA_MSG1_HIGH("CSFB: setting mode to %d",(int)ds_ucsd_network_mode);
#endif /* FEATURE_3GPP_CSFB*/
    }

    if ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) 
    {
      DATA_MSG0_HIGH("Initiating GSM CS data call");
    }
    else if ( ds_ucsd_network_mode == SYS_SYS_MODE_WCDMA ) 
    {
      DATA_MSG0_HIGH("Initiating WCDMA CS data call");
    }
#ifdef FEATURE_TDSCDMA
    else if ( ds_ucsd_network_mode == SYS_SYS_MODE_TDS )
    {
      DATA_MSG0_HIGH("Initiating TD_SCDMA CS data call");
    }
#endif /* FEATURE_TDSCDMA */
    else
    {
      DATA_MSG1_ERROR("Invalid network mode %d", ds_ucsd_network_mode);
    }
    /* Determine composite call type based on call mode */
    call_info_ptr->call_mode = data_ptr->call_mode;

#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
    /* TESTCASE: dual-mode MO call has Voice as less-preferred mode */
    if( 1 == dsat_get_qcscftest_exp_val(DSAT_QCSCFTEST_MO_INDEX) )
    {
      /* Add voice to call type */
      call_type |= DS_UCSD_VOICE_CALL;
    }
#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */
  
#ifdef FEATURE_DUAL_SIM
  subs_id = call_info_ptr->subs_id;
#endif      
    /* Get status of incoming internal data call routing, for QVP support */
    ds_ucsd_incoming_internal_call_enabled =
      ds_ucsd_get_incoming_internal_csd_call_enable_status(subs_id);

    /* Set the API call control and datapath interfaces. */
    SET_API_INTERFACES( call_info_ptr );
    
    /* Preserve the SupSrv dial string modifiers */
    call_info_ptr->dial_string_modifiers = data_ptr->modifiers;
      
    /*---------------------------------------------------------------------
      Preserve the User to User info. 
    ---------------------------------------------------------------------*/
#ifdef FEATURE_UUS
    call_info_ptr->uus_info = data_ptr->uus_info;
#endif
      
    /* Preserve the call bearer config settings */
    ds_ucsd_store_bearer_service_type(
         &bearer_service,
         data_ptr->speed,
         data_ptr->name,
         data_ptr->ce,
         data_ptr->waiur );

    
    /* Store call information. NULL RLC WM pointer has been checked in the call
    ** origination API, ds_ucsd_app_originate_call().
    */
    ds_ucsd_store_call_info(
         index,
         ds_ucsd_network_mode,
#ifdef FEATURE_DUAL_SIM
         ds_ucsd_subs_id,
#endif /* FEATURE_DUAL_SIM */
         call_type,
         &bearer_service,
         data_ptr->ul_rlc_wm_ptr,
         data_ptr->dl_rlc_wm_ptr,
         data_ptr->rlc_dl_rx_cb );

    call_info_ptr->client_id = data_ptr->client_id;
    
    
    /* Originate a call */
    if ( DS3G_FAILURE ==
         ds3g_msh_initiate_call( DS_CALL_TYPE_CKT,
                                 ds_ucsd_network_mode,
                                 data_ptr->dial_string,
                                 0,
                                 call_info_ptr ) )
    {
      DATA_MSG1_HIGH("Failed to initiate the call: CS Data call instance ID = %d", 
                data_ptr->inst_id);

      end_call = TRUE;

      /* Free the call table entry */
      ds_system_heap_mem_free_crit(ds_ucsd_call_tbl_ptr[index]);
      ds_ucsd_call_tbl_ptr[index] = NULL;
    }
  }

  /* Check for call termination */
  if ( end_call == TRUE ) 
  {
    ds_ucsd_call_event_type event;
    
    /* Setup call end event information */
    end_param.cc_param.cc_cause_present = FALSE;
    end_param.rej_param.present = FALSE;
    end_param.end_cause = call_end_cause;

    event.call_event = CM_CALL_EVENT_END;
    event.event_info.end.inst_id = data_ptr->inst_id;
    event.event_info.end.end_param = &end_param;

    /* Notify API client of event */
    if( FALSE ==
        ds_ucsd_app_notify_client(
          data_ptr->client_id, &event) )
    {
      DATA_MSG0_LOW("No client notified for call end");
    }
  }
} /* ds_ucsd_process_app_originate_call_cmd() */


/*===========================================================================

FUNCTION DS_UCSD_PROCESS_APP_CONFIRM_CALL_CMD

DESCRIPTION
   This function is to process the DS_UCSD_APP_CONFIRM_CALL_CMD
   command from a CS Data application. It initiates response to call
   setup for incoming CS Data call.  Call setup may be rejected by the
   client so must support seeding call control cause value.
        
   For dual mode calls, the client may opt to request the preferred
   mode be swapped by requesting the alternate call type.  The setup
   parameters are swapped in this case before invoking the DSMgr API.
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void ds_ucsd_process_app_confirm_call_cmd
( 
   ds_cmd_type  *cmd_ptr
)
{ 
  uint8  index;
  boolean end_call = FALSE;
#ifdef FEATURE_MULTIMEDIA_FALLBACK
  #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
  ds_ucsd_app_confirm_call_cmd_type  *data_ptr = NULL;

  ASSERT( NULL != cmd_ptr );
  ASSERT( NULL != cmd_ptr->cmd_payload_ptr );

  data_ptr = (ds_ucsd_app_confirm_call_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /* Ensure the call instance is available. */
  index = ds_ucsd_find_call_instance( data_ptr->inst_id );
  if( MAX_UCSD_CALL_TBL_ENTRIES > index )
  {
    ds_ucsd_call_tbl_entry_type * call_info_ptr = DSUCSD_GET_CALL_INFO( index );
    cm_setup_res_params_s_type  setup_params;

    /* Check for call instance in SETUP state */
    if( DS_UCSD_CALL_STATE_SETUP != call_info_ptr->state )
    {
      DATA_MSG0_ERROR("Call instance not in SETUP state, confirm rejected");
      /* Send end notification only if call state has not progressed. */
      end_call = (DS_UCSD_CALL_STATE_SETUP < call_info_ptr->state)? FALSE : TRUE;
    }
    else
    {
      /* Assign client ID for call end event processing */
      call_info_ptr->client_id = data_ptr->client_id;

    /* Check if client rejected the call setup */
    if( TRUE == data_ptr->result_params.reject)
    {
      ds_ucsd_cc_cause_param_type *cause_ptr
        = &data_ptr->result_params.cause_info;
      
      /* Populate the cause value */
      if( TRUE == cause_ptr->cc_cause_present )
      {
        /* According to spec 3GPP TS24.008 section 5.2.2.3.1, only
         * user busy and call rejected are allowable causes for call
         * setup rejection if configuration is otherwise fine. */
        switch( cause_ptr->cc_cause )
        {
        case USER_BUSY:
        case CALL_REJECTED:
          REJECT_CALL( (&setup_params), cause_ptr->cc_cause );
          break;

        default:
          DATA_MSG0_ERROR("Unsupported setup cause, using CALL_REJECTED" );
          REJECT_CALL( (&setup_params), CALL_REJECTED );
          break;
        }
      }
    }
    else
    {
#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#else
      /* Populate the Call Manager structure */
      setup_params.accept = TRUE;
      setup_params.cc_cause.present = FALSE;
      COPY_BEARER_CONFIG( &setup_params, &call_info_ptr->neg_cap );
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
    }
    
    /* Confirm the incoming call.  Setup parameters stored locally. If
     * call setup rejected, call end event will be posted to trigger cleanup. */
    if( DS3G_FAILURE == ds3g_msh_confirm_call( call_info_ptr->cm_call_id,
                                               &setup_params ) )
    {
      /* If CM fails to answer the call, DSMGR will end the call */
      DATA_MSG1_ERROR( "Failed to confirm call setup: CM call ID = %d", 
                 call_info_ptr->cm_call_id);
    }
    } /* DS_UCSD_CALL_STATE_SETUP */
  }
  else
  {
    DATA_MSG1_ERROR( "Invalid CM call instance ID %d in APP_CONFIRM_CALL cmd. End the call",
                     data_ptr->inst_id);
    end_call = TRUE;
  }

  /* Check for error condition and post message to client. */
  if( end_call )
  {
    ds_ucsd_call_end_param_type  end_param;
    ds_ucsd_call_event_type event;
    
    /* Setup call end end information */
    end_param.cc_param.cc_cause_present = FALSE;
    end_param.rej_param.present = FALSE;
    end_param.end_cause = CM_CALL_END_CONF_FAILED;

    event.call_event = CM_CALL_EVENT_END;
    event.event_info.end.inst_id = data_ptr->inst_id;
    event.event_info.end.end_param = &end_param;

    /* Notify API client of event */
    if( FALSE == ds_ucsd_app_notify_client(data_ptr->client_id, &event) )
    {
      DATA_MSG0_LOW("No client notified for call end");
    }
  }

} /* ds_ucsd_process_app_confirm_call_cmd() */



/*===========================================================================

FUNCTION DS_UCSD_PROCESS_APP_ANSWER_CALL_CMD

DESCRIPTION
   This function is to process the DS_UCSD_APP_ANSWER_CALL_CMD command from a
   CS Data application. It answers an incoming CS Data call.
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void ds_ucsd_process_app_answer_call_cmd
( 
   const ds_cmd_type  *cmd_ptr
)
{ 
  uint8                              index;
  boolean                            end_call = FALSE;
  ds_ucsd_call_end_param_type        end_param; /* the end reason parameters
                                                 * passed in the app call
                                                 * back
                                                 */
  ds_ucsd_app_answer_call_cmd_type  *data_ptr = NULL;
  /*-------------------------------------------------------------------------*/

  ASSERT( NULL != cmd_ptr );
  ASSERT( NULL != cmd_ptr->cmd_payload_ptr);

  data_ptr = (ds_ucsd_app_answer_call_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES >
      (index =
      ds_ucsd_find_call_instance( data_ptr->inst_id )) )
  {
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
    
    /* Check for call instance in ALERTING state */
    if( DS_UCSD_CALL_STATE_ALERTING != call_info_ptr->state )
    {
      DATA_MSG0_ERROR("Attempt to answer call not in ALERTING state");
      /* Send client end event */
      end_call = TRUE;
    }
    else
    {
      call_info_ptr->state = DS_UCSD_CALL_STATE_ANSWERED;

      /* Update call instance to indicate call owned by internal client */
      call_info_ptr->client_id = data_ptr->client_id;
      call_info_ptr->answer_params = data_ptr->answer_params;
           
      /* Set the API call control and datapath interfaces. */
      SET_API_INTERFACES( call_info_ptr );
    
      /* Answer the call, answer parameters read in later MSH callback */
      if ( ds3g_msh_answer_call_ex( call_info_ptr->cm_call_id,
#ifdef FEATURE_DUAL_SIM
                                    call_info_ptr->subs_id
#else
                                    SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                  ) == DS3G_FAILURE )
      {
        /* If CM fails to answer the call, DSMGR will end the call */
        DATA_MSG2_ERROR( "Failed to answer the call: CM call ID = %d; "
                   "CS Data call instance ID = %d", 
                   call_info_ptr->cm_call_id, 
                   call_info_ptr->inst_id);
      }
    } /* DS_UCSD_CALL_STATE_ALERTING */
  }

  /* Notify client of error by end event. */
  if( end_call )
  {
    ds_ucsd_call_event_type event;
    
    /* Setup call end event information */
    end_param.cc_param.cc_cause_present = FALSE;
    end_param.rej_param.present = FALSE;
    end_param.end_cause = CM_CALL_END_INTERNAL_ERROR;

    event.call_event = CM_CALL_EVENT_END;
    event.event_info.end.inst_id = data_ptr->inst_id;
    event.event_info.end.end_param = &end_param;

    /* Notify API client of event */
    if( FALSE == ds_ucsd_app_notify_client(data_ptr->client_id, &event) )
    {
      DATA_MSG0_LOW("No client notified for call end");
    }

    DATA_MSG1_ERROR("Invalid CS Data call instance ID %d in APP_ANSWER_CALL cmd. "
               "End the call", 
               data_ptr->inst_id );

  }

} /* ds_ucsd_process_app_answer_call_cmd() */

#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */



/*===========================================================================

FUNCTION DS_UCSD_PROCESS_APP_END_CALL_CMD

DESCRIPTION
   This function is to process the DS_UCSD_APP_END_CALL_CMD command from a
   CS Data application. It ends a CS Data call.
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void ds_ucsd_process_app_end_call_cmd
( 
   const ds_cmd_type  *cmd_ptr
)
{ 
   ds_ucsd_call_end_param_type     end_param;
   boolean                         end_call = FALSE; 
                                      /* Whether or not to end the call */
   cm_call_end_e_type              call_end_cause = CM_CALL_END_INTERNAL_ERROR;
                                      /* Reason for ending the call */
   uint8                           index;
   ds_ucsd_app_end_call_cmd_type  *data_ptr = NULL;
/*-------------------------------------------------------------------------*/

  ASSERT( NULL != cmd_ptr );
  ASSERT( NULL != cmd_ptr->cmd_payload_ptr );

  data_ptr = (ds_ucsd_app_end_call_cmd_type*)cmd_ptr->cmd_payload_ptr;
  
  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES >
      (index =
      ds_ucsd_find_call_instance( data_ptr->inst_id )) )
  {
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );

      /* Tell 3G DSMGR to end the call */
    if ( ds3g_msh_hangup_call_ex( call_info_ptr->cm_call_id, NULL,
#ifdef FEATURE_DUAL_SIM 
                                  call_info_ptr->subs_id
#else
                                  SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                ) == DS3G_FAILURE )
      {
        DATA_MSG2_ERROR( "Failed to end the call: CM call ID = %d; CS Data call instance ID = %d", 
                   call_info_ptr->cm_call_id, 
                   call_info_ptr->inst_id );

        end_call = TRUE;
      }
  }
  else
  {
    DATA_MSG1_ERROR( "Invalid CS Data call instance ID %d in APP_END_CALL cmd. End the call",
                     data_ptr->inst_id);

    end_call = TRUE;

    call_end_cause = CM_CALL_END_INTERNAL_ERROR;
  }

  /* Check for call termination */
  if ( end_call == TRUE ) 
  {
    ds_ucsd_call_event_type event;
    
    /* Setup call end event information */
    end_param.cc_param.cc_cause_present = FALSE;
    end_param.rej_param.present = FALSE;
    end_param.end_cause = call_end_cause;

    event.call_event = CM_CALL_EVENT_END;
    event.event_info.end.inst_id = data_ptr->inst_id;
    event.event_info.end.end_param = &end_param;

    /* Notify API client of event */
    if( FALSE == ds_ucsd_app_notify_client(data_ptr->client_id, &event) )
    {
      DATA_MSG0_LOW("No client notified for call end");
    }
  }
} /* ds_ucsd_process_app_end_call_cmd() */



/*===========================================================================

FUNCTION DS_UCSD_PROCESS_UUS_DATA_CMD

DESCRIPTION

   This function is to process the DS_UCSD_USER_USER_DATA_IND_CMD command 
   from CM. It's being used for MT User-User data, UUS1 in particular.

   The user to user info will be checked in the incoming call handler. If
   present, it will get passed up to QVP. QVP is the only CSD App using 
   UUS (UUIE) at present.
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void ds_ucsd_process_uus_data_cmd
( 
   const ds_cmd_type  *cmd_ptr
)
{ 
#ifdef FEATURE_UUS
  ds_ucsd_call_tbl_entry_type *call_info_ptr;
  uint8 index;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;

  ASSERT( NULL != cmd_ptr );
  ASSERT( NULL != cmd_ptr->cmd_payload_ptr );

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID(
                     call_info_cmd_ptr->call_id ))) )
  {
    DATA_MSG1_ERROR("Cannot find call instance for CM call ID: %d",
               call_info_cmd_ptr->call_id );
    return;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );
  
  /*-------------------------------------------------------------------------
    Recover the UUS info.
  -------------------------------------------------------------------------*/
  call_info_ptr->uus1_data = call_info_cmd_ptr->uus_info;

  if( call_info_ptr->uus1_data.mt_user_data.present == TRUE )
  {
    DATA_MSG0_HIGH("Received UUS1 data");
  }
  else
  {
    DATA_MSG0_HIGH("No UUS1 data present");
  }
  /*-------------------------------------------------------------------------
    Set ds_ucsd_uus1_data.mt_user_data.present back to FALSE in 
    ds_ucsd_call_ended_handler()
  -------------------------------------------------------------------------*/
#endif /* FEATURE_UUS */
}


/*===========================================================================

FUNCTION DS_UCSD_PROCESS_UUS_DATA_CONG_CMD

   This function is to process the DS_UCSD_USER_USER_DATA_CONG_IND_CMD 
   command from CM. It's being used for MO User-User data, UUS1 in 
   particular. It really is a stub for UUS3, where you can send/recv data
   continuously through UUIE. 

   For UUS1, it's really an internal (UE) error indication. If MN (NAS) sends 
   this to us after we orig, MN does not have enough command buffers to 
   initiate the UUS call. In this case, the call should probably be ended.

DESCRIPTION
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void ds_ucsd_process_uus_data_cong_cmd
( 
   const ds_cmd_type  *cmd_ptr
)
{ 
#ifdef FEATURE_UUS
  ds_ucsd_call_tbl_entry_type *call_info_ptr;
  uint8 index;
  ds3g_call_info_cmd_type  *call_info_cmd_ptr = NULL;

  ASSERT( NULL != cmd_ptr );
  ASSERT( NULL != cmd_ptr->cmd_payload_ptr );

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID(
                     call_info_cmd_ptr->call_id ))) )
  {
    DATA_MSG1_ERROR("Cannot find call instance for CM call ID: %d",
                    call_info_cmd_ptr->call_id );
    return;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );
    
  call_info_ptr->uus1_data = call_info_cmd_ptr->uus_info;

  if( call_info_ptr->uus1_data.mt_user_data.present == TRUE )
  {
    DATA_MSG0_HIGH("UUS1 data congestion ind has data");
  }
  else
  {
    DATA_MSG0_HIGH("UUS1 data congestion ind has no data");
  }

  DATA_MSG0_HIGH("Should end CSD call");
#endif /* FEATURE_UUS */
}


/*===========================================================================

FUNCTION ds_ucsd_post_client_event

DESCRIPTION
  This function posts an event notification to the UCSD API.  If the
  call instance client_ID has been previously assigned, only that
  client is notified.  Otherwise, all clients registered for the call
  type will be notified.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void ds_ucsd_post_client_event
(
  ds_ucsd_call_tbl_entry_type    *call_info_ptr,
  ds_ucsd_call_event_type        *event_ptr
)
{
  ASSERT( NULL != call_info_ptr );
  ASSERT( NULL != event_ptr );

  /* Check for uninitialized client ID. */
  if( DSUCSD_INVALID_CLIENT_ID != call_info_ptr->client_id )
  {
    /* Notify specific API client of event */
    if( FALSE ==
        ds_ucsd_app_notify_client( call_info_ptr->client_id, event_ptr ) )
    {
      DATA_MSG2_ERROR("No client notified for call event: clientID=%d CM event=%d",
                 call_info_ptr->client_id, event_ptr->call_event);
    }
  }
  else
  {
    /* Notify all API clients registered for VT calls of event */
    if( FALSE ==
        ds_ucsd_app_notify_clients_by_type( (uint8)call_info_ptr->call_type,
                                            event_ptr ) )
    {
      DATA_MSG1_LOW("No client notified for call event: CM event=%d",
              event_ptr->call_event);
    }
  }
} /* ds_ucsd_post_client_event() */



/*===========================================================================

FUNCTION ds_ucsd_call_end_common_cleanup

DESCRIPTION
   This function performs common operations to cleanup the call
   state. This routine is invoked by the call end handler on routine
   call end, and by the dial-string handler on error condition to undo
   the call origination handler operations.

DEPENDENCIES
   None
  
RETURN VALUE
   TRUE if operations successful, FALSE otherwise.

SIDE EFFECTS
   None

===========================================================================*/
LOCAL boolean 
ds_ucsd_call_end_common_cleanup
(
  uint8 inst_id
)
{
  boolean result = TRUE;
  ds_ucsd_call_tbl_entry_type *call_info_ptr;
  uint8 index;
  ds3g_siolib_portid_e_type port_index = DS3G_SIOLIB_PORTID_NONE;

/*-------------------------------------------------------------------------*/

  DATA_MSG1_MED("ds_ucsd_call_end_common_cleanup: inst ID=%d", inst_id);

  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( inst_id )) )
  {
    DATA_MSG1_ERROR("Cannot find call instance for inst ID: %d", inst_id);
    return FALSE;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );

  /* Check for allocated instance */
  if( (call_info_ptr != NULL) &&
      (call_info_ptr->inst_id == inst_id) )
  {
    /* Check for external datapath */
    if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
    {
      /* External datapath call */

#if defined(FEATURE_DATA_WCDMA_CS) || defined(FEATURE_TDSCDMA)
#ifdef FEATURE_DATA_V80
      /*----------------------------------------------------------------------
        If it was a V.80 call, make sure we re-enable flow from the TE. Flow
        is temporarily disabled in the orig_handler. It's re-enabled in the
        connected_handler, but if the call didn't connect, clean up here.
        ----------------------------------------------------------------------*/
      if( dsat_get_es_export_val(ES_ORIG_RQST_INDEX) == 6 &&
          dsat_get_es_export_val(ES_ANS_FBK_INDEX) == 8 )
      {
        ds3g_siolib_ex_set_inbound_flow( DS_FLOW_WCDMACS_MASK, 
                                         DS_FLOW_ENABLE,
                                         call_info_ptr->sio_port_id);
      }
#endif /* FEATURE_DATA_V80 */
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_TDSCDMA */
      
      /* Deregister from SIOLIB */
      if( DS3G_SIOLIB_PORT_NONE != call_info_ptr->sio_port_id )
      {
        /* Deregister callbacks for external call control */
        if( DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface )
        {
          if( FALSE ==
              ds3g_siolib_ex_deregister_callback_func (
                                  &ds_ucsd_siolib_func_tbl,
                                  call_info_ptr->sio_port_id) )
          {
            DATA_MSG0_ERROR("Error in SIOLIB deregister");
          }
        }
	port_index = ds3g_siolib_get_index_from_allocated_port(call_info_ptr->sio_port_id);
        ds3g_siolib_release_port ( port_index );
		
        call_info_ptr->sio_port_id = DS3G_SIOLIB_PORT_NONE;
      }
      
    } /* DS_UCSD_IFACE_SIO == datapath_iface */

    /* Check for external call control */
    if( DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface )
    {
      if( call_info_ptr->state != DS_UCSD_CALL_STATE_NONE )
      {
        /* De-register from ATCOP */
        dsat_deregister_handlers();
      }
    }

    /* Clear call instance record */
    (void)ds_ucsd_release_call_instance( inst_id );
  }
  else
  {
    DATA_MSG0_MED("Specified instance ID not in use");
  }

  return result;
} /* ds_ucsd_call_end_common_cleanup() */



/*===========================================================================

FUNCTION ds_ucsd_process_cmds

DESCRIPTION
   This function will be called from the main DS task dispather to send
   commands to the UCSD mode-specific subtask.   
   
   Note that the command buffer is not put back on the free queue here...
   that is done by the caller of this function.
        
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void
ds_ucsd_process_cmds
(
  ds_cmd_type  *cmd_ptr
)
{
  ds_ucsd_call_tbl_entry_type *ucsd_call_info_ptr = NULL;
  uint8                       call_id;
  
  if(cmd_ptr == NULL)
  {
    DATA_MSG0_ERROR("NULL ptr passed, return");
    return;
  }

  DATA_MSG1_HIGH("Got %d",cmd_ptr->hdr.cmd_id);

  switch (cmd_ptr->hdr.cmd_id)
  {
    /* If we get a disconnect indication from the protocol stack and
    ** we have a currently active call (call_info_ptr->cm_call_id is
    ** valid), then pass the indication up to 3GDSMGR
    */
  case DS_CMD_3GPP_GCSD_DISCONNECT:
    if (cmd_ptr->cmd_payload_ptr != NULL)
    {
      call_id = (uint8)(*((uint8*)(cmd_ptr->cmd_payload_ptr)));
             
      if( call_id != CM_CALL_ID_INVALID ) 
      {
        uint8 index;
      
        /* Find call instance in the call table */
        /* The same call_id field is used for both internal and external events. */
        if( MAX_UCSD_CALL_TBL_ENTRIES <=
            (index = ds_ucsd_find_call_instance(
              DSUCSD_XLATE_CALLID( call_id ))) )
        {
          DATA_MSG1_ERROR("Cannot find call instance for CM call ID: %d",
                          call_id);
          return;
        }

        ucsd_call_info_ptr = DSUCSD_GET_CALL_INFO( index );
        ucsd_call_info_ptr->protocol_stack_termination = TRUE;
              
        /* Tell 3G DSMGR to end the call */
      if ( ds3g_msh_hangup_call_ex( ucsd_call_info_ptr->cm_call_id, NULL,
#ifdef FEATURE_DUAL_SIM 
                                    ucsd_call_info_ptr->subs_id
#else
                                    SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                  ) == DS3G_FAILURE )
        {
          DATA_MSG2_ERROR("Failed to end the call: CM call ID = %d"
                          "CS Data call instance ID = %d", 
                          ucsd_call_info_ptr->cm_call_id, 
                          ucsd_call_info_ptr->inst_id);
        }
      }
    }
    break;
      
    /* If we finish RLP negotiation, pass along COMPLETE_CONNECT
    ** message to 3GDSMGR
    */
  case DS_CMD_3GPP_GCSD_RX_DCD_ON:
    if (cmd_ptr->cmd_payload_ptr != NULL)
    {
      call_id = (uint8)(*((uint8*)(cmd_ptr->cmd_payload_ptr)));
           
      if( call_id != CM_CALL_ID_INVALID )
      {
      (void)ds3g_msh_complete_ll_connect( call_id,
                                          ds3g_get_cs_subs_id());
      }
      else
      {
        DATA_MSG1_ERROR("Aborting connect, CM Call ID invalid!: %d",
                        call_id);
      }
    }
    break;  
      
  case DS_CMD_3GPP_GCSD_TX_DATA_FLUSHED:
    if (cmd_ptr->cmd_payload_ptr != NULL)
    {
      call_id = (uint8)(*((uint8*)(cmd_ptr->cmd_payload_ptr)));
           
      if( call_id != CM_CALL_ID_INVALID )
      {
      (void)ds3g_msh_complete_ll_disconnect( call_id,
                                             ds3g_get_cs_subs_id());   
      }
      /* Do not report anything here as call instance is removed on
       * switch/force-down so invalid call ID is expected. */
    }
    break;
      
    /* CS Data application originates a call */
  case DS_CMD_3GPP_UCSD_APP_ORIG_CALL:
    ds_ucsd_process_app_originate_call_cmd( cmd_ptr );
    break;

    /* CS Data application answers a call */
  case DS_CMD_3GPP_UCSD_APP_CONFIRM_CALL:
    ds_ucsd_process_app_confirm_call_cmd( cmd_ptr );
    break;

    /* CS Data application answers a call */
  case DS_CMD_3GPP_UCSD_APP_ANSWER_CALL:
    ds_ucsd_process_app_answer_call_cmd( cmd_ptr );
    break;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
    #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

    /* CS Data application ends a call */
  case DS_CMD_3GPP_UCSD_APP_END_CALL:
    ds_ucsd_process_app_end_call_cmd( cmd_ptr );
    break;

  /* UUS data indication came in */
  case DS_CMD_CM_CALL_EV_USER_DATA_IND:
    ds_ucsd_process_uus_data_cmd( cmd_ptr );
    break;

  /* UUS data congestion indication came in */
  case DS_CMD_CM_CALL_EV_USER_DATA_CONG_IND:
    ds_ucsd_process_uus_data_cong_cmd( cmd_ptr );
    break;

#ifdef FEATURE_DATA_UCSD_HANDOVER
    /* MN inter-RAT handover occurred */
  case DS_CMD_3GPP_UCSD_HANDOVER:
    (void)ds_ucsd_process_handover_cmd( cmd_ptr );
    break;
#endif /* FEATURE_DATA_UCSD_HANDOVER */
    
#ifdef FEATURE_MMGSDI_SESSION_LIB
  case DS_CMD_3GPP_UCSD_MMGSDI_PROC:
    ds_ucsd_process_mmgsdi_cmd(cmd_ptr);
    break;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

    /* If we get anything else, record it as an error */
  default:
    DATA_MSG1_ERROR("Unexpected command to UCSD handlers = %d",cmd_ptr->hdr.cmd_id);
    break;
  }
} /* ds_ucsd_process_cmds() */


/*===========================================================================

FUNCTION ds_ucsd_hangup_call_cb

DESCRIPTION
   This function is called from ATCOP to abort a call either between
   ATDxxx and CONNECT, or between ATA and CONNECT.  To this end, the
   3G DSMGR ds3g_hangup_call() function is called.

   No call ID is passed from caller so the call instance must be found
   indirectly.  First external calls with assigned ports are tried.
   Second calls which are in ORIG or ALERTING state are tried (abort
   scenario).  This routine assumes only one call instance will be in
   ORIGINATION or ALERTING state at any time.
   
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Ends current call

===========================================================================*/
void
ds_ucsd_call_hangup_cb (void)
{
  uint8 index;
  
  /* Check for external call instance with assigned port */
  if( MAX_UCSD_CALL_TBL_ENTRIES ==
      (index = ds_ucsd_find_call_instance_by_port( DS3G_SIOLIB_PORT_MAX )) )
  {
    /* Check for pending call establishment */
    index =
      ds_ucsd_find_call_instance_by_state( ((uint8)DS_UCSD_CALL_STATE_ANSWERED |
                                            (uint8)DS_UCSD_CALL_STATE_ALERTING |
                                            (uint8)DS_UCSD_CALL_STATE_ORIG) );
  }

  if( MAX_UCSD_CALL_TBL_ENTRIES != index )
  {
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
  
    DATA_MSG2_HIGH("HANGUP cb called, ending call: CM call ID %d  inst ID %d",
              call_info_ptr->cm_call_id, call_info_ptr->inst_id);
   
    /* Tell 3G DSMGR to end the call */
    if ( DS3G_FAILURE == ds3g_msh_hangup_call_ex( call_info_ptr->cm_call_id,
                                                  NULL,
#ifdef FEATURE_DUAL_SIM
                                                  call_info_ptr->subs_id
#else
                                                  SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                                ) )
    {
      DATA_MSG2_ERROR("Failed to end the call: "
                 "CM call ID = %d; CS Data call instance ID = %d", 
                 call_info_ptr->cm_call_id, 
                 call_info_ptr->inst_id);
    }
  }
  else
  {
    DATA_MSG0_ERROR("HANGUP cb cannot find external call instance");
  }

} /* ds_ucsd_call_hangup_cb() */


/*===========================================================================

FUNCTION ds_ucsd_call_answer_cb

DESCRIPTION
   This function is called from ATCOP to indicate that an incoming call
   is to be answered.  
   
   This routine assumes only one call instance will be in ALERTING
   state at any time.
   
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
dsat_call_abort_cb_type
ds_ucsd_call_answer_cb (void)
{
  uint8 index =
    ds_ucsd_find_call_instance_by_state( DS_UCSD_CALL_STATE_ALERTING );/*lint !e641 !e655 */
  
  /* Stop alerting the user */
  ds3g_siolib_stop_alerting();

  /* Verify external call instance found */
  if( MAX_UCSD_CALL_TBL_ENTRIES == index )
  {
    /* Call instance in bad state, reply to external host */
    DATA_MSG0_ERROR( "Call instance not in ALERTING state, rejecting answer");
    dsat_send_result_code( DSAT_NO_CARRIER );
  }
  else
  {
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
  
    DATA_MSG0_HIGH("ANSWER cb called");
    call_info_ptr->state = DS_UCSD_CALL_STATE_ANSWERED;

    /* Update call instance to indicate call owned by external host */
    call_info_ptr->client_id = DSUCSD_EXTERNAL_CLIENT_ID;
    call_info_ptr->datapath_iface =  DS_UCSD_IFACE_SIO;
    call_info_ptr->call_ctrl_iface = DS_UCSD_IFACE_SIO;

    /* Answer the call */
    if ( ds3g_msh_answer_call_ex( call_info_ptr->cm_call_id,
#ifdef FEATURE_DUAL_SIM
                                  call_info_ptr->subs_id
#else
                                  SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                ) == DS3G_FAILURE )
    {
      DATA_MSG2_ERROR("Failed to answer the call: CM call ID = %d; CS Data call instance ID = %d", 
                 call_info_ptr->cm_call_id, 
                 call_info_ptr->inst_id);

      /* Send NO_CARRIER result code to TE */  
      dsat_send_result_code( DSAT_NO_CARRIER );
    }
  }

  return ds_ucsd_call_hangup_cb;
} /* ds_ucsd_call_answer_cb() */



/*===========================================================================

FUNCTION ds_ucsd_connect_data_path

DESCRIPTION
  This function connects the UCSD data path with external subsystems.
  Watermarks are registered with RLC for WCDMA calls.  These may Data
  Services watermarks for raw data calls or watermarks provided by an
  external client (e.g QVP) for internal applications.  For external
  calls, watermarks are registered with SIOLIB.

DEPENDENCIES
  External client must have registered watermarks before invoking.
  
RETURN VALUE
  DS3G_SUCCESS if operations complete successfully, DS3G_FAILURE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL boolean 
ds_ucsd_connect_data_path
(
  const uint32                        rab_id,          /* RAB Identifier */
  const cm_bearer_capability_s_type  *data_bc_ptr,     /* Bearer info    */
  ds_ucsd_call_tbl_entry_type        *call_info_ptr    /* Call instance  */
)
{
#ifdef FEATURE_DATA_WCDMA_CS
   /* Pointer to WCSD command, used for WCDMA Sync CS Data */
   dswcsd_cmd_type             *wcsd_cmd_ptr;                   

   /* Flag for V.80 mode call */
   boolean                      v80_call = FALSE;
#endif /* FEATURE_DATA_WCDMA_CS */
   ds3g_siolib_port_e_type      port_id = ds3g_siolib_ex_get_csd_port();
   sys_modem_as_id_e_type subs_id = ds3g_get_cs_subs_id();   
/*-------------------------------------------------------------------------*/

   ASSERT( NULL != data_bc_ptr );
   ASSERT( NULL != call_info_ptr );
   
#ifdef FEATURE_DUAL_SIM
  subs_id = call_info_ptr->subs_id;
#endif /* FEATURE_DUAL_SIM */
   
   DATA_MSG1_HIGH("Connecting CSD data path: RABid=%d", rab_id );
   
#ifdef FEATURE_DUAL_SIM
   subs_id = call_info_ptr->subs_id;
#endif /* FEATURE_DUAL_SIM */

   memset ((void*)&call_info_ptr->chan_info, 0x0,
           sizeof(ds_ucsd_ota_channel_params_T));

   /* Configure watermarks with L2 for raw data call */
   ds_ucsd_configure_l2_watermarks( call_info_ptr );

#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
   if ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) )
   {
     boolean rlc_result;
     
     /* Setup interface with Layer 2 */
     rlc_result = ds_ucsd_connect_l2_interface( call_info_ptr,
                                                data_bc_ptr,
                                                rab_id,
                                                FALSE );

     if( DS3G_FAILURE == rlc_result )
     {
       DATA_MSG0_ERROR("Problem registering with RLC, connection aborted");
       return rlc_result;
     }


     /* External port datapath processing */
     if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface ) 
     {
       /* Notify the WCSD Uplink and Downlink tasks of call connection */
       if ( data_bc_ptr->sync_async == SYNCHRONOUS )
       {
         /* WCDMA Sync CS Data */

#ifdef FEATURE_DATA_V80
         /*------------------------------------------------------------------
           Determine if this is a V.80 mode call. The magic numbers 6 and 8
           can be found in section 8.1 of ITU-T V.80. When these are passed
           to the UE vi +ES=6,,8, the DTE is putting the UE into Synchronous
           Access Mode of V.80.
         ------------------------------------------------------------------*/
         if( dsat_get_es_export_val(ES_ORIG_RQST_INDEX) == 6 &&
             dsat_get_es_export_val(ES_ANS_FBK_INDEX) == 8 )
         {
           /*----------------------------------------------------------------
             Set local flag used for notifying the UL & DL tasks that
             a V.80 mode call or regular CS data call is connected.
           ----------------------------------------------------------------*/
           v80_call = TRUE;
         }
#endif /* FEATURE_DATA_V80 */

         
         if ( ( wcsd_cmd_ptr = dswcsd_ul_get_cmd_buf() ) != NULL ) 
         {
           if(v80_call == TRUE
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
              && ( ds_ucsd_sync_test_app_enabled == FALSE )
#endif
             )
           {
             wcsd_cmd_ptr->hdr.cmd_id = DSWCSD_V80_CONNECT_CALL_CMD;
           }
           else
           {
             wcsd_cmd_ptr->hdr.cmd_id = DSWCSD_CONNECT_CALL_CMD;
           }
             wcsd_cmd_ptr->hdr.subs_id = subs_id;

           dswcsd_ul_put_cmd( wcsd_cmd_ptr );
         }
         else
         {
           DS_3GPP_MSG0_ERROR( "Cannot get WCSD UL cmd buffer");
         }

         if ( ( wcsd_cmd_ptr = dswcsd_dl_get_cmd_buf() ) != NULL ) 
         {
           if(v80_call == TRUE
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
              && ( ds_ucsd_sync_test_app_enabled == FALSE )
#endif
             )
           {
             wcsd_cmd_ptr->hdr.cmd_id = DSWCSD_V80_CONNECT_CALL_CMD;
           }
           else
           {
             wcsd_cmd_ptr->hdr.cmd_id = DSWCSD_CONNECT_CALL_CMD;
           }

           dswcsd_dl_put_cmd( wcsd_cmd_ptr );
         }
         else
         {
           DS_3GPP_MSG0_ERROR( "Cannot get WCSD DL cmd buffer");
         }

#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
         /* Configure SIOLIB for USB port */
         port_id = DS3G_SIOLIB_USB_CDC_PORT;
         ds3g_siolib_set_ds3g_siolib_modem_port( DS3G_SIOLIB_USB_CDC_PORT );
	 ds3g_siolib_set_ds3g_siolib_active_port( DS3G_SIOLIB_USB_CDC_PORT );
#endif

         /*------------------------------------------------------------------
           Change SIO mode to RAWDATA and set up SIO watermark for raw data 
           processing. 
         ------------------------------------------------------------------*/
         if ( ds3g_siolib_ex_change_mode( 
                SIO_DS_RAWDATA_MODE,           /* Change SIO to   **
                                               ** RAWDATA mode    */
                NULL,                          /* Watermark for   **
                                               ** data from SIO   */
                &ds_ucsd_to_sio_wm,            /* Watermark for   **
                                               ** data to SIO     */
                dswcsd_rx_data_from_sio,        /* Function to be  **
                                               ** called when SIO **
                                               ** receives data   **
                                               ** from TE         */
                port_id) != DS3G_SIOLIB_OK )
         {
           DATA_MSG0_ERROR( "Failed to set SIO to RAWDATA mode" );
           return ( DS3G_FAILURE );
         }


#ifdef FEATURE_DATA_V80
         /*---------------------------------------------------------------------
           If the call is in V.80 mode, send the rate command to the DTE.
         ---------------------------------------------------------------------*/
         if(v80_call == TRUE
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
            && ( ds_ucsd_sync_test_app_enabled == FALSE )
#endif
           )
         {
           if ( ( wcsd_cmd_ptr = dswcsd_dl_get_cmd_buf() ) != NULL )
           {
             wcsd_cmd_ptr->hdr.cmd_id  = DSWCSD_V80_SEND_DTE_CMD;
             wcsd_cmd_ptr->cmd.v80_cmd = DSWCSDV80_RATE_CMD;
             dswcsd_dl_put_cmd( wcsd_cmd_ptr );
           }
           else
           {
             DS_3GPP_MSG0_ERROR( "Cannot get WCSD DL cmd buffer");
           }

           /*---------------------------------------------------------------------
             The following is temporary. The one H.324 app we have used on the
             TE was sending data prior to receiving the CONNECT. This was to
             get around that. The corresponding flow disable is in the call_orig 
             handler.
           ---------------------------------------------------------------------*/
           ds3g_siolib_ex_set_inbound_flow( DS_FLOW_WCDMACS_MASK,
                                            DS_FLOW_ENABLE,
                                            port_id);
         }
#endif /* FEATURE_DATA_V80 */
       } /* WCDMA Sync CS Data */
     } /* DS_UCSD_IFACE_SIO */
   } /* WCDMA */
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_TDSCDMA */

#if defined (FEATURE_DATA_GCSD)  ||   defined (FEATURE_DATA_WCDMA_CS)  
   if ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM )
   {
     /* Query for channel characteristics */
     if( TRUE !=
         ds_ucsd_get_GSM_channel_info(call_info_ptr) ) 
     {
       DATA_MSG0_ERROR("Failed to get valid channel characteristics: Releasing the call");
       return ( DS3G_FAILURE );
     }
   }
   
   if ( ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) ||
        ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) )&&
          ( data_bc_ptr->sync_async == ASYNCHRONOUS ) ) )
   {
     dsucsd_call_descriptor_type  call_desc;
   
            /* Register watermarks and initialize stack for GCSD call */
     ds_gcsd_init_watermarks( call_info_ptr->cm_call_id,
                              call_info_ptr->call_type,
                              ds_ucsd_mpshim_get_tps_to_sio_or_fps_wm(),
                              ds_ucsd_mpshim_get_tps_from_sio_or_fps_wm(),
                              ds_ucsd_mpshim_get_fps_to_sio_wm(),
                              ds_ucsd_mpshim_get_fps_from_sio_wm() );
     

     /* GSM Async CS Data */
     if ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) 
     {

       /* Data_rate is mapped to the FNUR */
       if ( data_bc_ptr->fixed_network_user_rate == FNUR_14400 )
       {
         /* 14.4k call */
         REPORT_RATE_ON_CONNECT( ATCOP_WAIUR_REPORT_14400,
                                 call_info_ptr->bearer_params.cbst_info.speed,
                                 call_info_ptr->bearer_params.cbst_info.name,
                                 call_info_ptr->bearer_params.cbst_info.connection_element );
       }
       else
       {
         /* 9.6k call */
         REPORT_RATE_ON_CONNECT( ATCOP_WAIUR_REPORT_9600,
                                 call_info_ptr->bearer_params.cbst_info.speed,
                                 call_info_ptr->bearer_params.cbst_info.name,
                                 call_info_ptr->bearer_params.cbst_info.connection_element );
       }
     } /* GSM CS Data */

     /* External port datapath processing */
     if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface ) 
     {
       /*------------------------------------------------------------------
         Change SIO mode to RAWDATA and set up SIO watermark for raw data 
         processing. 
       ------------------------------------------------------------------*/
       if ( ds3g_siolib_ex_change_mode( 
              SIO_DS_RAWDATA_MODE,                     /* Change SIO to   **
                                                       ** RAWDATA mode    */
              call_info_ptr->dpath_params.ul_wm_ptr,   /* Watermark for   **
                                                       ** data from SIO   */
              call_info_ptr->dpath_params.dl_wm_ptr,   /* Watermark for   **
                                                       ** data to SIO     */
              call_info_ptr->dpath_params.sio_ul_rx_cb, /* Function to be  **
                                                       ** called when SIO **
                                                       ** receives data   **
                                                       ** from TE         */
              port_id) != DS3G_SIOLIB_OK )
       {
         DATA_MSG0_ERROR("Failed to set SIO to RAWDATA mode");
   
         return ( DS3G_FAILURE );
       }
     }
#ifdef FEATURE_DATA_GCSD_FAX
     /* GSM FAX specific configuration */
     if ( ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) &&
          ( data_bc_ptr->information_transfer_capability == ITC_FAX_G3) )
     {
#ifdef FEATURE_SMD
       sio_ioctl_param_type  ioctl_param; 
       
       /* Restrict SMD buffering to reduce latency */
       ioctl_param.buffer_size = DSUCSD_MAX_SIO_FAX_BUFFER;
       ds3g_siolib_ex_ioctl( SIO_IOCTL_SET_BUFFER_SIZE, &ioctl_param, port_id );
#endif /* FEATURE_SMD */

       /* Notify ATCOP fax call mode in effect */
       DSAT_SET_CALL_MODE( DSAT_CALL_MODE_FAX ); /*lint !e655 */
     }
#endif /* FEATURE_DATA_GCSD_FAX */
      
     /*------------------------------------------------------------------
       Configure the GCSD task and start processing.
     ------------------------------------------------------------------*/
     SET_CALL_DESCRIPTOR( (&call_desc), call_info_ptr, data_bc_ptr );
     if( DS3G_SUCCESS !=
         ds_ucsd_mpshim_connect_data_path( &call_desc,
                                           &call_info_ptr->chan_info ))
     {
       DATA_MSG0_ERROR("Problem establishing data path");
     }
      
   } /* GSM or WCDMA Async CS Data */

#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS  */

   return DS3G_SUCCESS;
} /* ds_ucsd_connect_data_path() */




/*===========================================================================

FUNCTION ds_ucsd_disconnect_data_path

DESCRIPTION
  This function disconnects the UCSD data path with external subsystems.
  Watermarks are registered with RLC for WCDMA calls.  These may Data
  Services watermarks for raw data calls or watermarks provided by an
  external client (e.g QVP) for internal applications.

  Note for in-call service change, transition data->voice mode does
  not require RLC deregistration.  For voice calls, the speech
  datapath is connected by NAS when it processes the RAB
  reconfiguration.  It is not necessary to disconnect the data call
  datapath in this case as previous RLC registration of watermarks is
  overwritten.  Still need to cleanup watermarks locally however.
  
DEPENDENCIES
  External client must have register watermarks before invoking.
  
RETURN VALUE
  DS3G_SUCCESS if operations complete successfully, DS3G_FAILURE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL boolean 
ds_ucsd_disconnect_data_path
(
  const cm_bearer_capability_s_type  *data_bc_ptr,     /* Bearer info    */
  ds_ucsd_call_tbl_entry_type        *call_info_ptr,   /* Call instance  */
  const boolean                       do_rlc_dereg     /* Dereg from RLC?*/
)
{
#ifdef FEATURE_DATA_WCDMA_CS
   /* Pointer to WCSD command, used for WCDMA Sync CS Data */
   dswcsd_cmd_type             *wcsd_cmd_ptr;                   
#endif  

   boolean                      result = DS3G_SUCCESS;

   sys_modem_as_id_e_type subs_id = ds3g_get_cs_subs_id();
/*-------------------------------------------------------------------------*/
   ASSERT( NULL != data_bc_ptr );
   ASSERT( NULL != call_info_ptr );

#ifdef FEATURE_DUAL_SIM
   subs_id = call_info_ptr->subs_id;
#endif /* FEATURE_DUAL_SIM */

#if defined FEATURE_DATA_WCDMA_CS || defined (FEATURE_TDSCDMA)
   if ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode))
   {
     /* WCDMA/TD-SCDMA CS Data */

     /* Deregister the RLC UL and DL watermarks */
     if( do_rlc_dereg )
     {
       (void)ds_ucsd_deregister_l2_interface(subs_id);
     }
     
     /* Check for external datapath */
     if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
     {
       /* Tell WCSD UL/DL tasks to stop transmitting/receiving data and 
       ** dequeue the remaining data on the watermarks
       */
       if ( data_bc_ptr->sync_async == SYNCHRONOUS )
       {
         /* WCDMA Sync CS Data */

         if ( ( wcsd_cmd_ptr = dswcsd_ul_get_cmd_buf() ) != NULL ) 
         {
           wcsd_cmd_ptr->hdr.cmd_id = DSWCSD_END_CALL_CMD;

           dswcsd_ul_put_cmd( wcsd_cmd_ptr );
         }
         else
         {
           DS_3GPP_MSG0_ERROR( "Cannot get WCSD UL cmd buffer");
         }

         if ( ( wcsd_cmd_ptr = dswcsd_dl_get_cmd_buf() ) != NULL ) 
         {
           wcsd_cmd_ptr->hdr.cmd_id = DSWCSD_END_CALL_CMD;

           dswcsd_dl_put_cmd( wcsd_cmd_ptr );
         }
         else
         {
           DS_3GPP_MSG0_ERROR( "Cannot get WCSD DL cmd buffer" );
         }

       } /* WCDMA Sync CS Data */
     } /* Non-VT call */

   } /* WCDMA || TD_SCDMA */
#endif /* FEATURE_DATA_WCDMA_CS */
    
   /* Perform disconnect in shim layer */
   result = ds_ucsd_mpshim_disconnect_data_path( call_info_ptr->network_mode );
   if( DS3G_SUCCESS != result )
   {
     DATA_MSG0_ERROR("Problem disconnecting datapath (mpshim)");
   }
   return result;
} /* ds_ucsd_disconnect_data_path() */




/*===========================================================================

FUNCTION ds_ucsd_perform_siolib_registration

DESCRIPTION
  This function registers UCSD as a client of SIOLIB for external
  datapath.  An attempt to allocate an external modem port is made.  
  If successful, the callback table is registered for SIOLIB
  notificaitons.

  In case where call control is done via UCSD API client, the modem
  port is manaully selected.  This is to comply with multi-processor
  builds where another entity (e.g. WM RIL) will connect to same SMD
  port on remote processor.
   
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful registration, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL boolean 
ds_ucsd_perform_siolib_registration
(
  ds_ucsd_call_tbl_entry_type              *call_info_ptr
)
{
  boolean result = TRUE;
  ds3g_siolib_port_alloc_info_s_type  alloc_info;

  ASSERT( NULL != call_info_ptr );
  
  /* Setup SIOLIB port allocation mode as follows:
   *  - For external control path, use automatic mode
   *  - For API client control path, use manual mode   */
  alloc_info.mode = 
    (DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface)?
    DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC :
    DS3G_SIOLIB_PORT_ALLOC_MODE_MANUAL;

  alloc_info.call_dir =
    (DS_UCSD_CALL_DIR_ORIGINATED == call_info_ptr->call_dir)?
    DS3G_SIOLIB_PORT_CALL_DIR_ORIGINATED:
    DS3G_SIOLIB_PORT_CALL_DIR_TERMINATED;
      
  /* Setup external port allocation.  For manual allocations,
   * use a fixed SMD data port (convention for WM RIL). */
#if defined(FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD)
  alloc_info.manual_port =
    (DS3G_SIOLIB_PORT_ALLOC_MODE_MANUAL == alloc_info.mode)?
    DS3G_SIOLIB_SMD_DATA11_PORT : /* maps to SIO_PORT_SMD_WINMOB_QVP_DATA */
    DS3G_SIOLIB_PORT_NONE;
#else
  alloc_info.manual_port = DS3G_SIOLIB_PORT_NONE;
#endif /* (FEATURE_MULTIPROCESSOR) && defined(FEATURE_SMD) */
        
  /* Try to allocate SIO port for external data call */
  call_info_ptr->sio_port_id = ds3g_siolib_ex_allocate_port(
                                              &alloc_info, DS_CALL_TYPE_CKT );
  if( DS3G_SIOLIB_PORT_NONE != call_info_ptr->sio_port_id )
  {
    /* Register callbacks for external call control */
    if( DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface )
    {
      if (FALSE ==
          ds3g_siolib_ex_register_callback_func (&ds_ucsd_siolib_func_tbl,
                                                 call_info_ptr->sio_port_id))
      {
        DATA_MSG0_ERROR("Unable to reg SIOLIB funcs");
        result = FALSE;
      }
      else
      {
        DATA_MSG0_MED("Registered SIOLIB call control callbacks");
      }
    }
  }
  else
  {
    DATA_MSG0_ERROR("Unable to allocate SIOLIB port");
    result = FALSE;
  }

  return result;
} /* ds_ucsd_perform_siolib_registration() */



/*===========================================================================

FUNCTION ds_ucsd_perform_atcop_registration

DESCRIPTION
  This function registers UCSD as a client of ATCOP for answer and
  hangup command handlers.  If ATCOP cannot support the registration,
  the registration return code will indicate the reason.  Call end
  parameters are set base on the return code.

DEPENDENCIES
  ATCOP callback functions must be registered before SIOLIB
  callback functions are registered to avoid a race condition in
  ATCOP.
  
RETURN VALUE
  TRUE on successful registration, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean 
ds_ucsd_perform_atcop_registration
(
  const ds_ucsd_call_tbl_entry_type *call_info_ptr,
  cm_end_params_s_type              *end_params_ptr
)
{
  dsat_result_enum_type reg_rc;
  boolean result = FALSE;

  /* Attempt to register handlers with ATCOP */
  reg_rc = dsat_register_handlers( 
              (DS_UCSD_CALL_DIR_TERMINATED == call_info_ptr->call_dir)?
              ds_ucsd_call_answer_cb : NULL, /* Answer call back */
              ds_ucsd_call_hangup_cb );      /* Hangup call back */

  MSG_MED("Registering command handlers with ATCOP: inst=%d dir=%d reg_rc=%d",
          call_info_ptr->inst_id, call_info_ptr->call_dir, reg_rc);

  /* Check registration result */
  switch( reg_rc )
  {
  case  DSAT_OK:
    result = TRUE;
    break;
    
  case  DSAT_ERROR:
    DATA_MSG0_ERROR("ATCOP rejected registration: +CGAUTO=2");
    break;
  case  DSAT_BUSY:
    DATA_MSG0_ERROR("ATCOP rejected registration: MT PDP registered");
    break;

  default:
    DATA_MSG0_ERROR("Unsupported ATCOP return code");
    break;
  }

  /* Set call end information if unsuccessful registration */
  if( !result && (NULL != end_params_ptr) )
  {
    end_params_ptr->call_id   = call_info_ptr->cm_call_id;
    end_params_ptr->info_type = CM_CALL_MODE_INFO_GW_CS;
  }
  
  return result;
} /* ds_ucsd_perform_atcop_registration() */

#ifdef FEATURE_DATA_GCSD_FAX

/*===========================================================================

FUNCTION ds_ucsd_perform_siodriver_config

DESCRIPTION
  This function configures the SIO driver for CSD call.  The passed
  flag indicates whether call is in setup or teardown state.
   
DEPENDENCIES
  None
  
RETURN VALUE
  TRUE on successful operation, FALSE otherwise.

SIDE EFFECTS
  SIO driver configuration is modified.

===========================================================================*/
LOCAL boolean 
ds_ucsd_perform_siodriver_config
(
  boolean  is_call_connecting
)
{
  sio_ioctl_param_type sio_ioctl_param;
  static uint32 orig_packet_size = 0;

  /* Check for call connecting */
  if( is_call_connecting )
  {
    memset(&sio_ioctl_param, 0x0, sizeof(sio_ioctl_param));
    
    /* Query SIO driver for current DSM packet size. */
    ds3g_siolib_ex_ioctl( SIO_IOCTL_GET_PACKET_SIZE, 
                          &sio_ioctl_param,
                          ds3g_siolib_ex_get_csd_port());
    if( 0 < sio_ioctl_param.packet_size )
    {
      DATA_MSG1_MED("Getting SIO_IOCTL_SET_PACKET_SIZE = %d",
               sio_ioctl_param.packet_size);
      /* Cache in static variable for eventual call teardown */
      orig_packet_size = sio_ioctl_param.packet_size;
    }
    else
    {
      DATA_MSG1_ERROR("Error getting SIO_IOCTL_SET_PACKET_SIZE: %d",
                 sio_ioctl_param.packet_size );
    }

    /* Set max DSM packet size from SIO driver. Required for case
     * where HS-USB delivers huge packets, leading to watermark
     * overflow and fax protocol latency problems. */
    sio_ioctl_param.packet_size = DST31_MAX_SIO_TX_DATA_SZ;
  }
  else
  {
    /* Restore original packet size. */
    sio_ioctl_param.packet_size = orig_packet_size;
    orig_packet_size = 0;
  }

  DATA_MSG1_MED("Setting SIO_IOCTL_SET_PACKET_SIZE = %d",
           sio_ioctl_param.packet_size);
  ds3g_siolib_ioctl( SIO_IOCTL_SET_PACKET_SIZE, &sio_ioctl_param );

  return TRUE;
}
#endif /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */



/*===========================================================================

FUNCTION ds_ucsd_call_id_handler

DESCRIPTION
   From the 3GDSMGR header file....
  
   This handler is invoked after the origination, in order to pass the call
   id allocated by Call Mgr to the mode-specific handler.  
   
   In this case, we simply store the Call id in our global.
       
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
void 
ds_ucsd_call_id_handler
(
   cm_call_id_type          call_id,
   void                    *mode_spec_info_ptr,
   sys_modem_as_id_e_type   subs_id
)
{
   ds_ucsd_call_tbl_entry_type  *call_info_ptr = NULL;

   ASSERT( NULL != mode_spec_info_ptr );

   /* Call instance pointer was passed into the DS3GMGR call initation
    *  routine so we will have it in mode_spec_info_ptr. */
   call_info_ptr = (ds_ucsd_call_tbl_entry_type*)mode_spec_info_ptr;

   /* Make sure we are using a valid entry in the call table */
   if ( call_info_ptr != NULL )
   {
      MSG_HIGH( "Call ID handler: CM call ID=%d; CS inst ID=%d type=%d", 
                call_id, 
              call_info_ptr->inst_id, 
              call_info_ptr->call_type );

      /* Store the CM call ID */
    call_info_ptr->cm_call_id = call_id;

    /* Add cross-reference for instance to CM call ID */
    if (!ds_ucsd_set_call_inst(call_id, call_info_ptr->inst_id))
    {
      DATA_MSG1_ERROR("Unable to set call inst for cm_call_id : %d",
                 call_id);
      return;
    }
   }
   else
   {
      /* Invalid call table entry */
    DATA_MSG2_ERROR("Invalid call table entry [0x%p]. End the call: CM call ID = %d", 
         call_info_ptr, call_id);
      
      /* Tell 3G DSMGR to end the call */
      if ( ds3g_msh_hangup_call_ex( call_id,
                                    NULL,
#ifdef FEATURE_DUAL_SIM
                                    call_info_ptr->subs_id
#else
                                    SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                  ) == DS3G_FAILURE )
      {
        DATA_MSG0_ERROR("Failed to end the call" );
      }
   }
} /* ds_ucsd_call_id_handler() */



/*===========================================================================

FUNCTION ds_ucsd_dtr_changed_sig_handler

DESCRIPTION
   From the 3GDSMGR header file....
  
   This handler is invoked when the DTR signal gets asserted or de-asserted. 
   
   In our case, we will check the current setting the AT&D.  If it is 1, we
   will go to online command mode.  If it is 2, we will terminate the call. 
         
DEPENDENCIES
  None
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_dtr_changed_sig_handler
( 
   boolean dtr_status 
)
{
   const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
  uint8 index = MAX_UCSD_CALL_TBL_ENTRIES;

  /* Look for call instance with external SIO port assigned */
  if( MAX_UCSD_CALL_TBL_ENTRIES ==
      (index = ds_ucsd_find_call_instance_by_port( DS3G_SIOLIB_PORT_MAX )) )
  {
    /* Check for alerting state if no port has been assigned yet. */
    if( MAX_UCSD_CALL_TBL_ENTRIES ==
        (index = ds_ucsd_find_call_instance_by_state((uint8)DS_UCSD_CALL_STATE_ALERTING)) )
    {
      DATA_MSG0_HIGH("Could not find external call port "
               "for DTR signal hndlr");
    }
  }

  /* Check that call instance was found */
  if( MAX_UCSD_CALL_TBL_ENTRIES != index )  
  {
    /* Get the call info record */
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );

    DATA_MSG2_HIGH("DTR changed signal handler: status=%d index=%d",
             dtr_status, index);

   /* Find the data bearer for possibly dual-mode call */
   (void)ds_ucsd_determine_call_type(
           &call_info_ptr->neg_cap.bearer_capability_1,
           &call_info_ptr->neg_cap.bearer_capability_2,
           &call_info_ptr->neg_cap.bc_repeat_indicator,
           &data_bc_ptr,
           NULL );

   /* Check for data call instance available */
   if( NULL == data_bc_ptr )
   {
      DATA_MSG0_HIGH("Ignoring DTR drop-No data call instance");
      return;
   }
     
   /* If in a FAX call, and call is not yet connected, ignore the DTR
   ** transition... for some reason the MS FAX application flashes DTR
   ** and without this code, the call will be terminated.
   */
   if ( (data_bc_ptr->information_transfer_capability == ITC_FAX_G3) &&
        (ds3g_msh_is_call_connected( call_info_ptr->cm_call_id ) == 
         FALSE)
      )
   {
      DATA_MSG0_HIGH("Ignoring DTR drop-FAX not connected");
      return;
   }
      
   
   /* If DTR goes low, hang up call... note that this function is only
   ** called when the AT&D value is 2 (which means terminate the call
   ** when DTR goes low)
   */
   if ( dtr_status == FALSE )
   {
      DATA_MSG0_HIGH("DTR dropped, ending call");
      
      /* Tell 3G DSMGR to end the call */
      if ( DS3G_FAILURE == ds3g_msh_hangup_call_ex( call_info_ptr->cm_call_id, 
                                                    call_info_ptr,
#ifdef FEATURE_DUAL_SIM
                                                    call_info_ptr->subs_id
#else
                                                    SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                                  ) )
      {
        DATA_MSG2_ERROR( "Failed to end the call: CM call ID = %d; "
                   "CS Data call instance ID = %d", 
                   call_info_ptr->cm_call_id, 
                   call_info_ptr->inst_id);
      }
   }
  }
} /* ds_ucsd_dtr_changed_sig_handler() */


/*===========================================================================

FUNCTION ds_ucsd_call_conf_handler

DESCRIPTION
   From the 3GDSMGR header file....
  
   This handler is invoked when the CM_CALL_EVENT_CALL_CONF event is rx'ed
   from Call Mgr. The mode_info_ptr points to mode-specific information
   provided by Call Mgr. The mode-specific handler should validate any
   parameters that may have been proposed by the network. 
   
   This function is called after a MO origination with the (possibly 
   modified) set of bearer capabilities returned by the network.
         
DEPENDENCIES
  None
  
RETURN VALUE
   A return value of
   SUCCESS indicates that the parameters are acceptable, FAILURE indicates
   that the parameters are not acceptable i.e. the call should be released.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean 
ds_ucsd_call_conf_handler
(
   cm_call_id_type            call_id,
   cm_call_mode_info_s_type   *mode_info_ptr,
   cm_end_params_s_type       *end_params,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   sys_modem_as_id_e_type      subs_id
)
{
   boolean bc_ok;
   ds_ucsd_call_type curr_call_type;
   const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
   ds_ucsd_call_tbl_entry_type * call_info_ptr = NULL;
   uint8 index;

  if( (NULL == mode_info_ptr) ||
#ifdef FEATURE_MULTIMEDIA_FALLBACK
      #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
      (NULL == end_params) )
  {
    DATA_MSG0_ERROR( "NULL pointer parameter specified");
    return DS3G_FAILURE;
  }

  /* Determine current call type (for dual mode calls) */
  curr_call_type =
     ds_ucsd_determine_call_type( &mode_info_ptr->info.gw_cs_call.bearer_capability_1,
                                  &mode_info_ptr->info.gw_cs_call.bearer_capability_2,
                                  &mode_info_ptr->info.gw_cs_call.bc_repeat_indicator,
                                  &data_bc_ptr,
                                  NULL );

  DATA_MSG2_HIGH("Call confirm handler: CM call ID=%d type=%d",
            call_id,
            curr_call_type);
   
  /* Set the call end parameters */
  end_params->call_id   = call_id;
  end_params->info_type = CM_CALL_MODE_INFO_GW_CS;
   
  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
       (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
  {
#ifdef FEATURE_MULTIMEDIA_FALLBACK
    #error code not present
#else
    DATA_MSG1_ERROR("Cannot find call instance for CM call ID: %d",call_id );
    return DS3G_FAILURE;
#endif
  }
  else
  {
    call_info_ptr = DSUCSD_GET_CALL_INFO( index );
	if(call_info_ptr == NULL)
	{
	  DATA_MSG0_ERROR("NULL Pointer Parameter was about to be accesses");
	  return DS3G_FAILURE;
	}
	
	if(call_info_ptr->cm_call_id != call_id)
	{
	  DATA_MSG2_ERROR("Call Id values not matching, cm_call_id: %d call_id: %d",
	  call_info_ptr->cm_call_id, call_id);
	  return DS3G_FAILURE;
	}
  }
   
#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
  {
    ds_ucsd_negotiated_capability_T  dummy_cap;

    /* Dump to data mode BCIE */
    DATA_MSG0_HIGH("Conf Data BC structure:");
    ds_ucsd_print_bc_struct( data_bc_ptr );

    /* Check the bearer capability parameters... */ 
    bc_ok = ds_ucsd_bc_check_bc (mode_info_ptr, 
                                 data_bc_ptr,
                                 ds_ucsd_network_mode,
                                 &dummy_cap );
   
    if ( bc_ok ) 
    {
       /* For call in data mode */
       if(! IS_VOICE_CALL( curr_call_type ) ) /*lint !e655 */
       {
         /* Check for existing call instance.  Dual-mode voice calls may
          * have switched to data preferred-mode on call confirmation. */
         if( call_info_ptr != NULL &&
             (call_id == call_info_ptr->cm_call_id) )
         {
           DATA_MSG0_HIGH("CS Data call confirmed");
         }
         else
         {
#ifdef FEATURE_MULTIMEDIA_FALLBACK
           #error code not present
#else
           DATA_MSG0_ERROR( "Missing CS Data call instance");
           return (DS3G_FAILURE);
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
         }

         /* Capture the confirmed configuration */
         COPY_BEARER_CONFIG( &call_info_ptr->neg_cap, &mode_info_ptr->info.gw_cs_call );
         call_info_ptr->modify_allowed =
           mode_info_ptr->info.gw_cs_call.is_call_mod_possible;

#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
         /* Send intermediate result code */
         if ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) &&
              ( data_bc_ptr->sync_async == SYNCHRONOUS ) )
         {
           /* WCDMA Synchronous CS Data call */
           if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
           {
             dsat_send_cr_result( DSAT_CR_SERVICE_SYNC );
           }
         }
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_TDSCDMA*/
       }
#ifdef FEATURE_MULTIMEDIA_FALLBACK
       #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   
       /*------------------------------------------------------------------------
         Notify API client of call confirmation.  This is done whether
         the BC's were ok or not.
       -------------------------------------------------------------------------*/
       if( DS_UCSD_IFACE_API == call_info_ptr->call_ctrl_iface  ) 
       {
          ds_ucsd_call_event_type event;
    
          /* Setup event information */
          event.call_event = CM_CALL_EVENT_CALL_CONF;
          event.event_info.confirm.inst_id = call_info_ptr->inst_id;
          event.event_info.confirm.call_type = curr_call_type;
          event.event_info.confirm.modify_allowed =
            call_info_ptr->modify_allowed;

          /* Post notification to client(s) */
          ds_ucsd_post_client_event( call_info_ptr, &event);
       }

#ifdef FEATURE_MULTIMEDIA_FALLBACK
       #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
     } /* bc_ok */
     else
     {
        DATA_MSG0_ERROR("CS Data bearer validation failed");
     }
   }

   return (bc_ok);

} /* ds_ucsd_call_conf_handler() */



/*===========================================================================

FUNCTION ds_ucsd_call_progress_handler

DESCRIPTION
   From the 3GDSMGR header file....
  
   This handler is invoked when the CM_CALL_EVENT_CALL_PROGRESS_IND
   event is rx'ed from Call Mgr. The mode_info_ptr points to
   mode-specific information provided by Call Mgr. A return value of
   SUCCESS indicates that the message was processed successfully,
   FAILURE indicates some problem occurred.
         
   For dual-mode calls, the progress event indicates a network
   initiated service change has been completed.  If the call was
   originally in voice mode, need to transition for video mode and
   establish data path.
         
DEPENDENCIES
  None
  
RETURN VALUE
  A return value of SUCCESS indicates that the message was processed
  successfully, FAILURE indicates some problem occurred.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean 
ds_ucsd_call_progress_handler
(
   cm_call_id_type             call_id,
   cm_call_mode_info_s_type   *mode_info_ptr,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   sys_modem_as_id_e_type      subs_id
)
{
   ds_ucsd_call_type curr_call_type;
   const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
   ds_ucsd_call_tbl_entry_type  *call_info_ptr = NULL;
   uint8 index;
   ds_ucsd_call_tbl_entry_type  local_call_info;

   if( (NULL == mode_info_ptr)
#ifdef FEATURE_MULTIMEDIA_FALLBACK
        #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
     )
   {
     DATA_MSG0_ERROR("NULL pointer parameter specified");
     return DS3G_FAILURE;
   }
   memset((void*)&local_call_info,0x0,sizeof(local_call_info));
   
   /* Find call instance in the call table */
   if( MAX_UCSD_CALL_TBL_ENTRIES <=
       (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
   {
     DATA_MSG1_MED("Cannot find call instance for CM call ID: %d",call_id);
     local_call_info.inst_id = NO_CALL_ID;
   }
   else
   {
     call_info_ptr = DSUCSD_GET_CALL_INFO( index );
     /* Cache call_info locally in case instance is removed */
     local_call_info = *call_info_ptr;
   }
   
   /* Determine current call type (for dual mode calls) */
   curr_call_type =
     ds_ucsd_determine_call_type( &mode_info_ptr->info.gw_cs_call.bearer_capability_1,
                                  &mode_info_ptr->info.gw_cs_call.bearer_capability_2,
                                  &mode_info_ptr->info.gw_cs_call.bc_repeat_indicator,
                                  &data_bc_ptr,
                                  NULL );

   if( NULL == data_bc_ptr )
   {
     DATA_MSG0_ERROR("Cannot determine data bearer configuration");
     return DS3G_FAILURE;
   }

   MSG_HIGH( "Call progress handler: CM call ID=%d type=%d progress=%d",
             call_id, curr_call_type,
             mode_info_ptr->info.gw_cs_call.call_progress );
   
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   
   
   /*------------------------------------------------------------------------
     Notify API client of call progress event.
   ------------------------------------------------------------------------*/
   if( DS_UCSD_IFACE_API == local_call_info.call_ctrl_iface )
   {
     ds_ucsd_call_event_type event;
     ds_ucsd_datapath_type datapath;
    
     /* Setup event information */
     ds_ucsd_app_init_datapath( &local_call_info, &datapath );
     event.call_event = CM_CALL_EVENT_PROGRESS_INFO_IND;
     event.event_info.progress.inst_id = local_call_info.inst_id;
     event.event_info.progress.call_type = curr_call_type;
     event.event_info.progress.datapath = &datapath;
     event.event_info.progress.progress_info =
       mode_info_ptr->info.gw_cs_call.call_progress;

     /* Post notification to client(s) */
     ds_ucsd_post_client_event( &local_call_info, &event);
   }

   return (DS3G_SUCCESS);

} /* ds_ucsd_call_progress_handler() */


/*===========================================================================

FUNCTION ds_ucsd_call_connected_handler

DESCRIPTION
   From the 3GDSMGR header file....
  
   This handler is invoked when the CM_CALL_EVENT_CONNECT event is rx'ed
   from Call Mgr. The mode_info_ptr points to mode-specific information
   provided by Call Mgr. The mode-specific handler should set up the lower
   layers for data transfer, and optionally, set change_serial_mode to TRUE
   (and specify the additional serial parameters) if the serial mode needs
   to be changed. A return value of SUCCESS indicates that the data path is
   established, FAILURE indicates that the lower layers could not be set up
   i.e. the call should be released. 
   
   In our case, we will set up the watermarks to be used by
   the GCSD PS or WCSD Uplink and Downlink task, set up the serial 
   watermarks to be returned by the function, and pass the connection 
   indication to the GCSD PS or WCSD Uplink and Downlink task.
         
DEPENDENCIES
  None
  
RETURN VALUE
   A return value of SUCCESS indicates that the parameters are
   acceptable, FAILURE indicates that the parameters are not
   acceptable i.e. the call should be released.
    
SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL boolean 
ds_ucsd_call_connected_handler
(
   cm_call_id_type           call_id,
   cm_call_mode_info_s_type  *mode_info_ptr,
   boolean                   *delay_connect,
   cm_end_params_s_type      *end_params,
   sys_modem_as_id_e_type     subs_id
)
{
   /* current call type (for dual mode calls) */
   ds_ucsd_call_type curr_call_type;

   /* Data bearer capability (for dual mode calls) */
   const cm_bearer_capability_s_type  *data_bc_ptr = NULL;

   /* Call instance record */
   ds_ucsd_call_tbl_entry_type * call_info_ptr;
   uint8 index;

/*-------------------------------------------------------------------------*/

  if( (NULL == mode_info_ptr) ||
      (NULL == delay_connect) ||
      (NULL == end_params) )
  {
    DATA_MSG0_ERROR( "NULL pointer parameter specified" );
    return DS3G_FAILURE;
  }

  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
  {
    DATA_MSG1_ERROR( "Cannot find call instance for CM call ID: %d",
               call_id);
    return DS3G_FAILURE;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );

   ASSERT( call_info_ptr->cm_call_id == call_id );

   /* Determine current call type (for dual mode calls) */
   curr_call_type =
     ds_ucsd_determine_call_type( &mode_info_ptr->info.gw_cs_call.bearer_capability_1,
                                  &mode_info_ptr->info.gw_cs_call.bearer_capability_2,
                                  &mode_info_ptr->info.gw_cs_call.bc_repeat_indicator,
                                  &data_bc_ptr,
                                  NULL );
   
   if( NULL == data_bc_ptr )
   {
     DATA_MSG1_ERROR("Cannot determine data bearer configuration: inst %d",
               call_info_ptr->inst_id);
     return ( DS3G_FAILURE );
   }
   
   DATA_MSG2_HIGH( "Call connected handler: CM call ID=%d type=%d",
             call_id,
             curr_call_type);

   /* Check for external datapath but also always alert external 
    * ports for non-VT calls. */
   if( (DS_UCSD_CALL_STATE_ALERTING == call_info_ptr->state) &&
       ((DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface) ||
        (!IS_VIDEO_CALL( call_info_ptr->call_type ))) )
   {
     /* Stop alerting the user */
     ds3g_siolib_stop_alerting();
   }

#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   {
     /* Set the call end parameters */
     end_params->call_id   = call_id;
     end_params->info_type = CM_CALL_MODE_INFO_GW_CS;

#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
     /* Do not delay the connection for WCDMA Synchronous calls. Delay the 
     ** connection otherwise.
     */
     if ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) &&
          ( data_bc_ptr->sync_async == SYNCHRONOUS ) )
     {
       /* Connect the call immediately */
       *delay_connect = FALSE;
     }
     else
#endif /* WCDMA || TDSCDMA */
     {
       *delay_connect = TRUE;
     }

#ifdef FEATURE_DATA_WCDMA_CS
        /* clean up of W-UL stale data if any pending from prior call in SIO */
        dswcsd_ul_cleanup();
#endif /* FEATURE_DATA_WCDMA_CS */

     /* Allocate SIO port for external MT data call. */
     /* This applies only to MT calls as the SIOLIB MODEM port does
      * not get assigned until call is answered in multi-port builds,
      * so cannot be done on INCOMING event. MO calls register in
      * origination handler to get semaphore as early as possible. */
     if ( (DS_UCSD_CALL_DIR_TERMINATED == call_info_ptr->call_dir) &&
         (DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface) )
     {
       /* Register with SIOLIB for external data port */
       if( TRUE !=
           ds_ucsd_perform_siolib_registration( call_info_ptr ) )
       {
         /* Registration failed. */  
         DATA_MSG0_ERROR("Unable to allocate SIOLIB port");
         return ( DS3G_FAILURE );
       }
     }
   
     /* Connect the data path for external subsystem interfaces */
     if( TRUE != ds_ucsd_connect_data_path( mode_info_ptr->info.gw_cs_call.rab_id,
                                            data_bc_ptr,
                                            call_info_ptr ) )
     {
       DATA_MSG0_ERROR("Problem connecting external interface data path" );
       return DS3G_FAILURE;
     }
     else
     {
       /* API client notification (for non-RLP calls) */
       if( (DS_UCSD_IFACE_API == call_info_ptr->call_ctrl_iface) &&
           (FALSE == *delay_connect) ) 
       {
         ds_ucsd_call_event_type event;
         ds_ucsd_datapath_type datapath;

         /* Setup event information */
         ds_ucsd_app_init_datapath( call_info_ptr, &datapath );
       
         event.call_event = CM_CALL_EVENT_CONNECT;
         event.event_info.connect.inst_id = call_info_ptr->inst_id;
         event.event_info.connect.call_type = curr_call_type;
         event.event_info.connect.datapath = &datapath;
         event.event_info.connect.modify_allowed = call_info_ptr->modify_allowed;
       
         /* Post notification to client(s) */
         ds_ucsd_post_client_event( call_info_ptr, &event);

       } /* DS_UCSD_IFACE_API == call_ctrl_iface */

       DATA_MSG1_HIGH("CS Data call connected, delay=%d", *delay_connect);
     }
   } /* IS_VOICE_CALL() */

   /* Set call instance state for HO processing */
   call_info_ptr->state = (*delay_connect == TRUE)?
                          DS_UCSD_CALL_STATE_CONNECT_LL :
                          DS_UCSD_CALL_STATE_CONNECT;

   return DS3G_SUCCESS;
} /* ds_ucsd_call_connected_handler() */



/*===========================================================================

FUNCTION ds_ucsd_complete_ll_connect_handler

DESCRIPTION
   From the 3GDSMGR header file....
  
   This handler is invoked when the DS_COMPLETE_LL_CONNECT_CMD event is 
   rx'ed from mode-specific handlers if delay_connect was set to TRUE when
   the call_connected_handler() was called. The mode-specific handler should 
   set up the lower layers for data transfer, and optionally, set 
   change_serial_mode to TRUE
   (and specify the additional serial parameters) if the serial mode needs
   to be changed. A return value of SUCCESS indicates that the data path is
   established, FAILURE indicates that the lower layers could not be set up
   i.e. the call should be released. 
   
   In our case (UCSD subtask), we will set up the watermarks to be used by
   the GCSD task, set up the serial watermarks to be returned by the 
   function, and call the ATCOP +DR command handler
         
DEPENDENCIES
  None
  
RETURN VALUE
   A return value of
   SUCCESS indicates that the parameters are acceptable, FAILURE indicates
   that the parameters are not acceptable i.e. the call should be released.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean 
ds_ucsd_complete_ll_connect_handler
(
   cm_call_id_type         call_id,
   cm_end_params_s_type    *end_params,
   sys_modem_as_id_e_type  subs_id
)
{  
  const cm_bearer_capability_s_type  *data_bc_ptr;
  ds_ucsd_call_tbl_entry_type * call_info_ptr;
  uint8 index;
  ds_ucsd_call_type             curr_call_type;

  if( NULL == end_params )
  {
    DATA_MSG0_ERROR( "NULL pointer parameter specified");
    return DS3G_FAILURE;
  }
  
  DATA_MSG0_HIGH("Complete LL connect handler");

  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
  {
    DATA_MSG1_ERROR("Cannot find call instance for CM call ID: %d",
               call_id );
    return DS3G_FAILURE;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );
#ifdef FEATURE_DUAL_SIM
  if(subs_id != call_info_ptr->subs_id)
  {
    DATA_MSG0_ERROR("Subs_id Mismatch");
    return DS3G_FAILURE;
  }
#endif /* FEATURE_DUAL_SIM */

   /* Find the data bearer for possibly dual-mode call */
  curr_call_type = ds_ucsd_determine_call_type(
           &call_info_ptr->neg_cap.bearer_capability_1,
           &call_info_ptr->neg_cap.bearer_capability_2,
           &call_info_ptr->neg_cap.bc_repeat_indicator,
           &data_bc_ptr,
           NULL );
   ASSERT( NULL != data_bc_ptr );

   if ( ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) ||
        ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) &&
         ( data_bc_ptr->sync_async == ASYNCHRONOUS ) ) )
   {
      /* GSM or WCDMA Async CS Data */
      
      /* Used to report to ATCOP */
      dsat_cr_service_e_type  cr_service;
      dsat_dr_mode_e_type     dr_mode = DSAT_DR_MODE_NONE;   

      /* Set the call end parameters */
      end_params->call_id   = call_id;
      end_params->info_type = CM_CALL_MODE_INFO_GW_CS;


#ifdef FEATURE_DATA_GCSD_FAX
    /* Connect GSM fax prototcol layer */
    if( data_bc_ptr->information_transfer_capability == ITC_FAX_G3)
    {
      /* Configure SIO driver for connecting call */
      (void)ds_ucsd_perform_siodriver_config( TRUE );
      dsfps_call_connect_init();
    }
#endif /* FEATURE_DATA_GCSD_FAX */

    /* Check for external call control */
    if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
    {
      /* Tell ATCOP about CR and DR data...  
      */

      /* If transparent call... */
      if( data_bc_ptr->connection_element == TRANSPARENT_DATA )
      {  
         /* Set CR result for Transparent call */
         if( data_bc_ptr->sync_async == ASYNCHRONOUS )
         {
            cr_service = DSAT_CR_SERVICE_ASYNC;     /* asynchronous transparent */
         }
         else
         {
            cr_service = DSAT_CR_SERVICE_SYNC;      /* synchronous transparent */
         }
      }
      else  /* Non-transparent */
      {
         if( data_bc_ptr->sync_async == ASYNCHRONOUS )
         {
            cr_service = DSAT_CR_SERVICE_REL_ASYNC; /* asynchronous non-transparent */
         }
         else
         {
            cr_service = DSAT_CR_SERVICE_REL_SYNC;  /* synchronous non-transparent */
         }

         /* Translate compression type to ATCOP parameter */   
         switch ( ds_ucsd_mpshim_get_rlp_dc_state() )
         {
            case V42_BOTH_DIRECTIONS: dr_mode = DSAT_DR_MODE_BOTH;  break;
            case     V42_DECODE_ONLY: dr_mode = DSAT_DR_MODE_RD;    break;
            case     V42_ENCODE_ONLY: dr_mode = DSAT_DR_MODE_TD;    break;
            case  V42_NO_COMPRESSION: dr_mode = DSAT_DR_MODE_NONE;  break;

            default:
              DATA_MSG1_ERROR("Bad compression mode=%d",
                        call_info_ptr->rlp_parameters.compression);
              dr_mode = DSAT_DR_MODE_NONE;
         }
      }

      /* Tell ATCOP about CR and DR results */
      dsat_send_cr_result_ex(cr_service,call_info_ptr->sio_port_id);
      dsat_send_dr_result_ex(dr_mode,call_info_ptr->sio_port_id);

      /* Send the CONNECT result code to ATCOP */
      dsat_send_result_code_ex (DSAT_CONNECT, call_info_ptr->sio_port_id);

      /* Set Carrier Detect signal to tell TE the call is up */
      (void)ds3g_siolib_ex_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_ON,
                                         call_info_ptr->sio_port_id);
    }

    /* API client notification */
    if( DS_UCSD_IFACE_API == call_info_ptr->call_ctrl_iface ) 
    {
      ds_ucsd_call_event_type event;
      ds_ucsd_datapath_type datapath;

      /* Setup event information */
      ds_ucsd_app_init_datapath( call_info_ptr, &datapath );
       
      event.call_event = CM_CALL_EVENT_CONNECT;
      event.event_info.connect.inst_id = call_info_ptr->inst_id;
      event.event_info.connect.call_type = curr_call_type;
      event.event_info.connect.datapath = &datapath;
      event.event_info.connect.modify_allowed = call_info_ptr->modify_allowed;
       
      /* Post notification to client(s) */
      ds_ucsd_post_client_event( call_info_ptr, &event);
    }

    /* Set call instance state for HO processing */
    call_info_ptr->state = DS_UCSD_CALL_STATE_CONNECT;
   }

   return DS3G_SUCCESS;
} /* ds_ucsd_complete_ll_connect_handler() */


/*===========================================================================

FUNCTION ds_ucsd_setup_handler

DESCRIPTION
   From the 3GDSMGR header file....
   
   This handler is invoked when the CM_CALL_EVENT_SETUP_IND event is rx'ed
   from Call Mgr. The mode_info_ptr points to mode-specific information
   provided by Call Mgr. The mode-specific handler should validate the setup
   parameters proposed by the network, and specify the setup_res_params. 
   
   In our case (UCSD subtask), we will collect the ds_ucsd_chosen_capability 
   data and then validate/negotiate the bearer capability information. If the
   received parameters are valid, we will return our negotiated data and
   return SUCCESS, otherwise we will return FAILURE.
         
   For dual mode calls, if the data mode cannot be supported, the
   call will still be rejected.  
         
DEPENDENCIES
  None
  
RETURN VALUE
  A return value of SUCCESS indicates that the parameters are
  acceptable, FAILURE indicates that the parameters are not acceptable
  i.e. the call should be released.
    
SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL boolean 
ds_ucsd_setup_handler
(
   cm_call_id_type               call_id,
   cm_call_mode_info_s_type     *mode_info_ptr,
   cm_setup_res_params_s_type   *setup_res_params,
   sys_sys_mode_e_type           call_mode,
   boolean                      *delay_rsp,
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
   sys_modem_as_id_e_type        subs_id
)
{
   boolean chosen_capability_valid, bc_ok, dummy;
   ds_ucsd_call_type curr_call_type = DS_UCSD_UNKNOWN_CALL;
   const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
   const cm_bearer_capability_s_type  *voice_bc_ptr = NULL;
   ds_ucsd_call_tbl_entry_type  *call_info_ptr = NULL;
   ds_ucsd_nt_rlp_parameters_T   rlp_parameters;
   ds_ucsd_chosen_capability_T   chosen_capability;
   ds_ucsd_negotiated_capability_T neg_cap;
   ds_ucsd_bearer_service_type  bearer_service;
   nv_item_type      nv_item;    /* NV interface data */
   nv_stat_enum_type nv_status;  /* status var for reading from NV */
   ds_ucsd_call_event_type event;
   dsat_num_item_type tmp_val;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

   ASSERT( NULL != mode_info_ptr );
   ASSERT( NULL != setup_res_params );
   ASSERT( NULL != delay_rsp );

   DATA_MSG1_HIGH("Call setup handler: CM call ID=%d", call_id);

#ifdef FEATURE_DUAL_SIM
   /* Get the network mode in set up event*/
   ds_ucsd_network_mode = call_mode; /* replace with ds3g fuction*///???
   ds_ucsd_subs_id      = subs_id;
#else
   /* Get the current network mode */
   ds_ucsd_network_mode = ds3g_get_current_cs_domain_mode();
   if( INVALID_3GPP_CS_SYS_MODE(ds_ucsd_network_mode) )
   {
#ifdef FEATURE_3GPP_CSFB
      DATA_MSG1_HIGH("CSFB: setting mode to internal mode %d",
               ds_ucsd_internal_mode);
      ds_ucsd_network_mode = ds_ucsd_internal_mode;
#endif /* FEATURE_3GPP_CSFB */
    }
#endif /* FEATURE_DUAL_SIM */

   /* Query call handling parameters from NVRAM */
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
   /*-------------------------------------------------------------------------
     See if this is to be a Sync test app call. If the work around for flow
     controlling the TE in the orig handler is removed, this pulling of
     NV_WCDMACS_SYNC_TEST_APP_ENABLED_I from NV should move into the
     call_connected_hdlr. Doing this NV read in the connecteed hdlr will
     cover the case for both MO & MT calls.
   ------------------------------------------------------------------------*/
   ds_ucsd_sync_test_app_enabled =
      dswcsd_get_sync_test_app_enable_status(subs_id);
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

   /* Get status of incoming internal data call routing */
   ds_ucsd_incoming_internal_call_enabled =
      ds_ucsd_get_incoming_internal_csd_call_enable_status(subs_id);

   if (mode_info_ptr->info.gw_cs_call.bearer_capability_1.present == TRUE)
   {
#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
     /* TESTCASE2: voice added as preferred mode before call setup */
     if( 2 == dsat_get_qcscftest_exp_val(DSAT_QCSCFTEST_MT_INDEX) )
     {
       cm_call_comp_cap_info_s_type  bc_llc_info;
       
       DATA_MSG0_HIGH("TESTCASE: add voice config as preferred before setup");
       mode_info_ptr->info.gw_cs_call.bearer_capability_2 =
         mode_info_ptr->info.gw_cs_call.bearer_capability_1;
       mode_info_ptr->info.gw_cs_call.low_layer_compatibility_2 =
         mode_info_ptr->info.gw_cs_call.low_layer_compatibility_1;
       
       /* Query voice BC/LLC */
       (void)ds3g_msh_get_call_config(CM_CALL_TYPE_VOICE, &bc_llc_info);
       
       mode_info_ptr->info.gw_cs_call.bearer_capability_1 =
         bc_llc_info.bearer_capability;
       mode_info_ptr->info.gw_cs_call.bc_repeat_indicator.present = TRUE;
       mode_info_ptr->info.gw_cs_call.bc_repeat_indicator.repeat_indication =
         REPEAT_SERVICE_CHANGE_FALLBACK;
       
       mode_info_ptr->info.gw_cs_call.low_layer_compatibility_1 =
         bc_llc_info.low_layer_compatibility;
       mode_info_ptr->info.gw_cs_call.llc_repeat_indicator.present = TRUE;
       mode_info_ptr->info.gw_cs_call.llc_repeat_indicator.repeat_indication =
         REPEAT_SERVICE_CHANGE_FALLBACK;
     }
#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */

     /* Determine current call type (for dual mode calls) */
     curr_call_type =
       ds_ucsd_determine_call_type( &mode_info_ptr->info.gw_cs_call.bearer_capability_1,
                                    &mode_info_ptr->info.gw_cs_call.bearer_capability_2,
                                    &mode_info_ptr->info.gw_cs_call.bc_repeat_indicator,
                                    &data_bc_ptr,
                                    &voice_bc_ptr );

     /* Return if data_bc_ptr is NULL. This condition occurs if ITC = 7 is sent 
        by network in setup message. */
     if(data_bc_ptr == NULL)
     {
       DATA_MSG0_ERROR("data_bc_ptr is NULL, CSD call rejected" );
       REJECT_CALL( setup_res_params, INCOMPATIBLE_DESTINATION );
       return DS3G_FAILURE; 
     }

     DATA_MSG2_HIGH("Current call_type = %d, subs_id = %d, Incoming Setup BC struct:",
                     curr_call_type, subs_id);
     ds3g_set_cs_subs_id(subs_id);     
     ds_ucsd_print_bc_struct( data_bc_ptr );
     
     /* Validate the repeat indicator */
     if( FALSE == ds_ucsd_validate_repeat_indicator( mode_info_ptr,
                                                     data_bc_ptr,
                                                     setup_res_params) )
     {
       return (DS3G_FAILURE);
     }

#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
     /* TESTCASE5: dual-mode MT call; data bearer not supported */
     if( 5 == dsat_get_qcscftest_exp_val(DSAT_QCSCFTEST_MT_INDEX) )
     {
       cm_bearer_capability_s_type *bc_ptr = 
          (cm_bearer_capability_s_type*)data_bc_ptr;
       DATA_MSG0_HIGH("TESTCASE: invalidate bearer (bad FNUR 9600)");
       bc_ptr->fixed_network_user_rate = FNUR_9600;
     }
#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */
   }/* bearer_capability_1.present == TRUE */
   /* Do not delay the call setup response or clear the call by default */
   *delay_rsp = FALSE;
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

   memset((void*)&neg_cap, 0x0, sizeof(neg_cap));
   COPY_BEARER_CONFIG( &neg_cap, &mode_info_ptr->info.gw_cs_call );

   /* Store the +CBST & +CHSN values in bearer_service, 
   ** before ds_ucsd_get_chosen_cap() is called.
   */ 
   tmp_val = (dsat_num_item_type)dsatutil_get_val( DSATETSI_EXT_CHSN_IDX,
                                      0,ATCOP_CHSN_WAIUR_POSITION,NUM_TYPE);
   ds_ucsd_store_bearer_service_type( 
     &bearer_service,
      (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_SPEED_POSITION),
      (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_NAME_POSITION),
      (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_CE_POSITION),
      (uint8)tmp_val );

   /* Fill in the chosen_capability structure based on AT command
   ** inputs.  This data will be gathered at the start of every
   ** MO and MT call.  It is possible that the capability data
   ** will not be valid 
   */
   chosen_capability_valid = 
     ds_ucsd_get_chosen_cap( &bearer_service,
                             &chosen_capability );
   
   /* Get RLP parameters in case requested call is ASYNC */
   ds_ucsd_get_rlp_params (&rlp_parameters,subs_id);
   
   /* Now check and negotiate the bearer capability data */
   bc_ok = ds_ucsd_bc_check_and_negotiate_bc (mode_info_ptr,
                                              data_bc_ptr,
                                              &chosen_capability,
                                              &neg_cap,
                                              &rlp_parameters,
                                              &bearer_service,
                                              &dummy,
                                              chosen_capability_valid,
                                              ds_ucsd_network_mode,
                                              subs_id );
                                              
   if (bc_ok)
   {
     /* Call setup has been negotiated successfully */
     
     /* Update data bearer pointer & call type after negotiation */
     ds_ucsd_call_type data_call_type = 
        ds_ucsd_determine_call_type( &neg_cap.bearer_capability_1,
                                     &neg_cap.bearer_capability_2,
                                     &neg_cap.bc_repeat_indicator,
                                     &data_bc_ptr,
                                     &voice_bc_ptr );

#ifdef FEATURE_MULTIMEDIA_FALLBACK
     #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
     {
       nv_status = ds3gcfgmgr_read_legacy_nv( NV_DISABLE_CM_CALL_TYPE_I, &nv_item );
       if ( NV_DONE_S == nv_status )
       {
         if( (nv_item.disable_cm_call_type & CM_CALL_TYPE_VT_MASK) &&
              IS_VIDEO_CALL( data_call_type ) ) 
         {
           /* Reject the incoming VT data call */
           MSG_ERROR("Incoming VT CSD call rejected, NV_DISABLE_CM_CALL_TYPE_I",
                     0, 0, 0 );
           REJECT_CALL( setup_res_params, INCOMPATIBLE_DESTINATION );
           return DS3G_FAILURE; 
         }
       }
       else
       {
         DATA_MSG1_ERROR("NV read failure NV status = %d",nv_status);
       }
       /* Allocate a CSD call instance record; referece to call info is
        * returned */
       if( TRUE !=
           ds_ucsd_allocate_call_instance( call_id,
                                           data_bc_ptr,
                                           &mode_info_ptr->info.gw_cs_call,
                                           &call_info_ptr ) )
       {
         /* Cannot get a CS Data call instance ID */
         DATA_MSG1_ERROR("CS Data call instance unavailable for CM call ID: %d ",
                    call_id);

         /* Reject the call with cause USER BUSY */
         /* See 3GPP TS24.008 section 5.2.2.3.1  */
         REJECT_CALL( setup_res_params, USER_BUSY );
       
         return DS3G_FAILURE;
       }
       
       /* Proceed with call setup */
       setup_res_params->accept = TRUE;
       COPY_BEARER_CONFIG( setup_res_params, &neg_cap );

       /* No reject cause */
       setup_res_params->cc_cause.present = FALSE;

       /* Preserve the setup paramaters for asynchronous response */
       call_info_ptr->neg_cap = neg_cap;
       call_info_ptr->neg_cap.present = TRUE;
       call_info_ptr->rlp_parameters = rlp_parameters;
       call_info_ptr->bearer_params = bearer_service;
       call_info_ptr->state = DS_UCSD_CALL_STATE_SETUP;
         
       DATA_MSG0_HIGH("Outgoing Setup BC struct:");
       ds_ucsd_print_bc_struct( data_bc_ptr );
      
       /* Remember calling party's number */
       memscpy( &call_info_ptr->calling_party_number, 
                sizeof( call_info_ptr->calling_party_number ),
                &mode_info_ptr->info.gw_cs_call.calling_party_bcd_number,
                sizeof( call_info_ptr->calling_party_number ) );

#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
       /* TESTCASE3: dual-mode MT call setup response for dual mode
        * call has swapped modes. */
       if( 3 == dsat_get_qcscftest_exp_val(DSAT_QCSCFTEST_MT_INDEX) )
       {
         DATA_MSG0_HIGH("TESTCASE: swap bearers");
         ds_ucsd_bc_change_bearers( &call_info_ptr->neg_cap,
                                    TRUE, TRUE, FALSE );
           
         /* TESTCASE4: dual-mode MT call; clear BC2/LLC2 */
         if( 4 == dsat_get_qcscftest_exp_val(DSAT_QCSCFTEST_MO_INDEX) )
         {
           DATA_MSG0_HIGH("TESTCASE: clear bearer #2");
           ds_ucsd_bc_change_bearers( &call_info_ptr->neg_cap,
                                      FALSE, FALSE, TRUE );
         }
           
         COPY_BEARER_CONFIG( setup_res_params, &call_info_ptr->neg_cap );
       }
       /* TESTCASE4: dual-mode MT call; clear BC2/LLC2 */
       else if( 4 == dsat_get_qcscftest_exp_val(DSAT_QCSCFTEST_MT_INDEX) )
       {
         DATA_MSG0_HIGH("TESTCASE: clear bearer #2");
         ds_ucsd_bc_change_bearers( &call_info_ptr->neg_cap,
                                    FALSE, FALSE, TRUE );
         COPY_BEARER_CONFIG( setup_res_params, &call_info_ptr->neg_cap );
       }
#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */

       /* Check for voice bearer stripped condition */
       if( call_info_ptr->modify_allowed && !voice_bc_ptr )
       {
         call_info_ptr->modify_allowed = FALSE;
       }
       
       /* Setup event information */
       event.call_event = CM_CALL_EVENT_SETUP_IND;
       event.event_info.setup.inst_id = call_info_ptr->inst_id;
       event.event_info.setup.call_type = data_call_type;
       event.event_info.setup.modify_allowed = call_info_ptr->modify_allowed;
         
       /* Notify API client of event */
       if( FALSE ==
           ds_ucsd_app_notify_clients_by_type( (uint8)data_call_type, &event ) )
       {
         /* If no client registered for call type, automatically
          * acknowledge setup indication */
         DATA_MSG0_LOW("No client notified for call setup");
         *delay_rsp = FALSE;

         /* Assume external call control and datapath */
         call_info_ptr->call_ctrl_iface = DS_UCSD_IFACE_SIO;
         call_info_ptr->datapath_iface  = DS_UCSD_IFACE_SIO;
         DATA_MSG0_MED("UCSD Routing: call_ctrl=SIO, datapath=SIO");

         /*  Check for expected internally routed Video call */
         if( ds_ucsd_incoming_internal_call_enabled &&
             IS_VIDEO_CALL( data_call_type ) ) /*lint !e655 */
         {
           /* Reject the call with cause CALL_REJECTED as there is no
            * client registered to process call. 
            * See 3GPP TS24.008 section 5.2.2.3.1 */
           REJECT_CALL( setup_res_params, CALL_REJECTED );
           DATA_MSG0_ERROR("CS Data VT call setup failed, "
                      "no client registered - call rejected");
         }
         else
         {
           /* Not internally routed VT call */
           ASSERT(NULL != data_bc_ptr);
           /* Check for WCDMA/TD-SCDMA synchronous call */
           if( (data_bc_ptr->sync_async == SYNCHRONOUS) &&
               ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode)) ) 
           {
             /* Send intermediate result code for external call; for
              * other call types this is done on CONNECT */
             dsat_send_cr_result( DSAT_CR_SERVICE_SYNC );
           }
         }
       }
       else
       {
         /* Client notified; wait for acknowledgement */
         *delay_rsp = TRUE;
         
         /* Assume use client call control and datapath interfaces.
          * Note these values may change depending on answer processing. */
         SET_API_INTERFACES( call_info_ptr );
       }
       
#ifdef FEATURE_MULTIMEDIA_FALLBACK
       #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
     }
   }
   else
   {
     /* Reject the call with cause INCOMPATIBLE_DESTINATION */
     /* See 3GPP TS24.008 Annex B.3.2                       */
     REJECT_CALL( setup_res_params, INCOMPATIBLE_DESTINATION );
     DATA_MSG0_ERROR("CS Data call setup failed - incompatible destination" );
   }  
#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

  ds_gcsd_start_general_purpose_timer();
  return bc_ok;
} /* ds_ucsd_setup_handler() */



/*===========================================================================

FUNCTION ds_ucsd_user_end_handler

DESCRIPTION
   From the 3GDSMGR header file....
   
   This handler is invoked when the user or TE ends the call. The mode-
   specific handler may attempt to shut down lower layer protocols.

   In our case (UCSD subtask), this handler will just be used to set
   the flag indicating that the termination is mobile originated.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void 
ds_ucsd_user_end_handler
( 
   cm_call_id_type             call_id,
   cm_end_params_s_type       *end_params,
   boolean                    *delay_disconnect,
   void                       *user_info_ptr,
   sys_modem_as_id_e_type      subs_id 
)
{
  ds_ucsd_call_type curr_call_type;
  const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
  ds_ucsd_call_tbl_entry_type * call_info_ptr;
  uint8 index;

  ASSERT( NULL != end_params );
  ASSERT( NULL != delay_disconnect );
  
  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
  {
    DATA_MSG1_ERROR( "Cannot find call instance for CM call ID: %d",
               call_id);
    return;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );

   /* Determine current call type (for dual mode calls) */
   curr_call_type =
     ds_ucsd_determine_call_type( &call_info_ptr->neg_cap.bearer_capability_1,
                                  &call_info_ptr->neg_cap.bearer_capability_2,
                                  &call_info_ptr->neg_cap.bc_repeat_indicator,
                                  &data_bc_ptr,
                                  NULL );
   
   DATA_MSG2_ERROR( "User end handler: CM call ID=%d type=%d",
             call_id,
             curr_call_type );
   
   /* Check the current call ID */
   ASSERT( call_info_ptr->cm_call_id == call_id );
   
   /* Set the call end parameters */
   end_params->call_id   = call_id;
   end_params->info_type = CM_CALL_MODE_INFO_GW_CS;
   
   /* If we get a UCSD protocol stack initiated termination, treat
   ** it like a Network originated termination... this will prevent
   ** us from trying to flush the TX data and will also result in a
   ** NO CARRIER being sent to the TE
   */
   if( call_info_ptr->protocol_stack_termination )
   {
      *delay_disconnect = FALSE;
   }
#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA) 
   else if ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode)) &&
             ( data_bc_ptr != NULL ) &&
             ( data_bc_ptr->sync_async == SYNCHRONOUS ) )
   {
      *delay_disconnect = FALSE;
      
      /* Set the flag indicating that the termination is mobile originated */
      call_info_ptr->user_initiated_termination = TRUE;
   }
#endif /* (FEATURE_DATA_WCDMA_CS)  || FEATURE_TDSCDMA */
   else
   {
     /* Set the flag indicating that the termination is mobile originated */
     call_info_ptr->user_initiated_termination = TRUE;

     if( DS_UCSD_CALL_STATE_PRE_CONNECT > call_info_ptr->state ) {
       /* Immediate disconnect if no lower-layer setup done. */
       *delay_disconnect = FALSE;
     }
     else
     {
       /* Indicate that we want to delay the disconnect */
       *delay_disconnect = TRUE;
            
       /* Send disconnect message to protocol stack task */
       ds_ucsd_mpshim_send_gcsd_msg( DS_GCSD_DISCONNECT_MO_REQ,
                                     call_info_ptr->cm_call_id );
     }
   }
   
} /* ds_ucsd_user_end_handler() */


/*===========================================================================

FUNCTION ds_ucsd_complete_ll_disconnect_handler

DESCRIPTION
   This handler is called when the low-level disconnect is complete.
   It simply sets the call end parameters.         
DEPENDENCIES
  None
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/
LOCAL void 
ds_ucsd_complete_ll_disconnect_handler
(
   cm_call_id_type       call_id,
   cm_end_params_s_type  *end_params,
   sys_modem_as_id_e_type subs_id
)
{
  ASSERT( NULL != end_params );

   DATA_MSG0_HIGH("Complete LL disconnect handler");

      /* Set the end params */
      end_params->call_id   = call_id;
      end_params->info_type = CM_CALL_MODE_INFO_GW_CS;
   
} /* ds_ucsd_complete_ll_disconnect_handler() */
   
   

/*===========================================================================

FUNCTION ds_ucsd_call_ended_handler

DESCRIPTION
   From the 3GDSMGR header file....
   
   This handler is invoked when the CM_CALL_EVENT_END event is rx'ed from
   Call Mgr. The mode-specific handler should clean up all call related
   state and shut down protocol layers.

   In our case (UCSD subtask), we will simply forward this event to the UCSD
   protocol stack, and mark the current call table entry unused to 
   indicate that we no longer have an active call.
         
DEPENDENCIES
  None
  
RETURN VALUE
  None.
    
SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
LOCAL void 
ds_ucsd_call_ended_handler
(
   ds_cmd_type               *cmd_ptr,
   boolean                    call_was_connected,
   sys_modem_as_id_e_type     subs_id
)
{

  /* end parameter structure */
  ds_ucsd_call_end_param_type end_param;
  
  /* Call instance record */
  ds_ucsd_call_tbl_entry_type *call_info_ptr = NULL;
  const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
  dsucsd_call_descriptor_type  call_desc;
  ds_ucsd_call_type curr_call_type;
  uint8 index;
#ifdef FEATURE_DATA_WCDMA_CS
  uint16 sio_q_cnt=0;
#endif /* FEATURE_DATA_WCDMA_CS */
  ds3g_call_info_cmd_type *call_info_cmd_ptr = NULL;
/*-------------------------------------------------------------------------*/
   
  ASSERT( NULL != cmd_ptr );
  ASSERT( NULL != cmd_ptr->cmd_payload_ptr );

  call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

  /* Find call instance in the call table */
   /* Make sure we are using a valid entry in the call table.  If MT
   * call setup failed, switched or forced down, no instance is
   * available so call end processing is unnecessary. */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance(
                 DSUCSD_XLATE_CALLID( call_info_cmd_ptr->call_id ))) )
  {
    DATA_MSG1_HIGH("Cannot find call instance for CM call ID: %d",
                   call_info_cmd_ptr->call_id);
    return;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );
  
  if( call_info_ptr == NULL )
  {
    DATA_MSG1_HIGH( "No CSD call instance, skipping call end: CM call ID=%d",
                    call_info_cmd_ptr->call_id );
    return;
  }

  /* Determine current call type (for dual mode calls) */
  curr_call_type =
    ds_ucsd_determine_call_type( &call_info_ptr->neg_cap.bearer_capability_1,
                                 &call_info_ptr->neg_cap.bearer_capability_2,
                                 &call_info_ptr->neg_cap.bc_repeat_indicator,
                                 &data_bc_ptr,
                                 NULL );

  DATA_MSG2_HIGH( "Call ended handler: CM call ID=%d type=%d",
            call_info_ptr->cm_call_id,
            curr_call_type );

  /*---------------------------------------------------------------------
     Signal call end to lower layer protocols and upper layer client.
  ---------------------------------------------------------------------*/
      
  /* Cleanup call in shim layer */
  ASSERT(NULL != data_bc_ptr);
  SET_CALL_DESCRIPTOR( (&call_desc), call_info_ptr, data_bc_ptr );
  if( DS3G_SUCCESS != ds_ucsd_mpshim_call_cleanup( &call_desc,
                                                   call_was_connected ) )
  {
    DATA_MSG0_ERROR("Problem in shim layer cleanup");
  }

#ifdef FEATURE_UUS
  /*---------------------------------------------------------------------
    Mark the UUS data as no longer valid.
    ---------------------------------------------------------------------*/
  call_info_ptr->uus1_data.mt_user_data.present = FALSE;
#endif /* FEATURE_UUS */

  /* Check for API client call control */
  if( DS_UCSD_IFACE_API == call_info_ptr->call_ctrl_iface )
  {
    ds_ucsd_call_event_type event;
    cm_gw_cs_call_info_s_type *mode_info_ptr =
      &call_info_cmd_ptr->mode_info.info.gw_cs_call;
    
    /* Setup call end event information */
    end_param.end_cause = call_info_cmd_ptr->end_status;
    end_param.cc_param.cc_cause_present = mode_info_ptr->cc_cause.present;
    end_param.cc_param.cc_cause = mode_info_ptr->cc_cause.cause_value;
    end_param.rej_param.rej_type = mode_info_ptr->cc_reject.rej_type;
    end_param.rej_param.rej_reason = mode_info_ptr->cc_reject.rej_value;
    
    event.call_event = CM_CALL_EVENT_END;
    event.event_info.end.inst_id = call_info_ptr->inst_id;
    event.event_info.end.end_param = &end_param;

    /* Post notification to client(s) */
    ds_ucsd_post_client_event( call_info_ptr, &event);

  } /* DS_UCSD_IFACE_API == call_ctrl_iface */

  /* Ensure call was connected before resetting datapath */
  if( (DS_UCSD_CALL_STATE_CONNECT_LL == call_info_ptr->state) ||
      (DS_UCSD_CALL_STATE_CONNECT    == call_info_ptr->state) )
  {
    DATA_MSG0_MED("Disconnecting datapath");
    if( TRUE != ds_ucsd_disconnect_data_path( data_bc_ptr,
                                              call_info_ptr,
                                              TRUE ) )
    {
      DATA_MSG0_ERROR("Problem disconnecting external interface datapath");
    }
  }
    
  /*---------------------------------------------------------------------
     Call end processing for local layer.
  ---------------------------------------------------------------------*/

#ifdef FEATURE_DATA_GCSD_FAX
  /* If we are in a FAX call, turn the clock speed back down
   */
  if ( ( call_info_ptr->network_mode == SYS_SYS_MODE_GSM ) &&
       ( data_bc_ptr->information_transfer_capability == ITC_FAX_G3) )
  {
    /* To keep from getting warnings when below features 
    ** aren't defined
    */
    DATA_MSG0_HIGH("FAX call"); 
    /* Ensure call was connected before re-configuring SIO */
    if (DS_UCSD_CALL_STATE_CONNECT == call_info_ptr->state)
    {
      /* Restore SIO driver for disconnecting call */
      (void)ds_ucsd_perform_siodriver_config( FALSE );
    }
    /* Notify ATCOP fax call mode cleared */
    DSAT_CLEAR_CALL_MODE( DSAT_CALL_MODE_FAX ); /*lint !e641 !e64 */

  }
#endif /* FEATURE_DATA_GCSD_FAX */


  /* Check for external datapath */
  if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
  {
    /* Ensure SIOLIB port was successfully allocated */
    if( DS3G_SIOLIB_PORT_NONE != call_info_ptr->sio_port_id )
    {
      /* Ensure call was connected before resetting SIOLIB mode */
      if( (DS_UCSD_CALL_STATE_CONNECT_LL == call_info_ptr->state) ||
          (DS_UCSD_CALL_STATE_CONNECT    == call_info_ptr->state) )
      {
        /* Change SIO mode back to AUTODECTECT */
        if( DS3G_SIOLIB_OK !=
            ds3g_siolib_ex_change_mode( SIO_DS_AUTODETECT_MODE,
                                     NULL,
                                     NULL,
                                     NULL,
                                     call_info_ptr->sio_port_id) )
        {
          DATA_MSG0_ERROR("Failed to set SIO to AUTODETECT mode");
        }
      }
    }
  } /* DS_UCSD_IFACE_SIO == datapath_iface */

  /* Check for external datapath but also always alert external 
   * ports for non-VT calls. */
  if( (DS_UCSD_CALL_STATE_ALERTING == call_info_ptr->state) &&
      ((DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface) ||
       (!IS_VIDEO_CALL( call_info_ptr->call_type ))) )
  {
    /* Stop alerting the user */
    ds3g_siolib_stop_alerting();
  }
  
  /* Check for external call control */
  if( DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface )
  {
    /* Ensure SIOLIB port was successfully allocated */
    if( DS3G_SIOLIB_PORT_NONE != call_info_ptr->sio_port_id )
    {
      /* Set Carrier Detect signal to tell TE the call is down 
         only if the port was successfully allocated previously*/
      (void)ds3g_siolib_ex_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF,
                                         call_info_ptr->sio_port_id);
    }
    if (call_info_ptr->user_initiated_termination)
    {
      /* Send OK result code to ATCOP */
      dsat_send_result_code_ex (DSAT_OK,call_info_ptr->sio_port_id);
    }
    /* Only put out TE result code if call was ever connected or it
     * was a mobile originated call (i.e. network originated call,
     * network originated termination and never connected, print
     * nothing) Also, if it was user answered, need to respond to
     * ATA */
    else if( (DS_UCSD_CALL_DIR_ORIGINATED == call_info_ptr->call_dir) || 
             call_was_connected || 
             call_info_ptr->was_user_answered )
    {
      /* Examine the end event cause indication if present to
       * determine proper result code to send to TE.  Assume NO
       * CARRIER if no indication available. */
      dsat_result_enum_type return_code = DSAT_NO_CARRIER;
        
      if ( call_info_cmd_ptr->mode_info. \
           info.gw_cs_call.cc_cause.present )
      {
        switch ( call_info_cmd_ptr->mode_info. \
                 info.gw_cs_call.cc_cause.cause_value )
        {
        case USER_BUSY:
          return_code = DSAT_BUSY;
          break;

        case USER_ALERTING_NO_ANSWER:
          return_code = DSAT_NO_ANSWER;
          break;
            
        default:
          break;
        }
      }
        
      /* Generate TE return code */
      dsat_send_result_code_ex ( return_code, call_info_ptr->sio_port_id);
    }
  } /* DS_UCSD_IFACE_SIO == call_ctrl_iface */
   
  /* Reset the flags that indicate who terminated the call */
  call_info_ptr->user_initiated_termination = FALSE;
  call_info_ptr->protocol_stack_termination = FALSE;
   
  /* Update the last call information */
  ds_ucsd_last_call_info.present = TRUE;
  ds_ucsd_last_call_info.inst_id = call_info_ptr->inst_id;
  ds_ucsd_last_call_info.cm_call_id = call_info_ptr->cm_call_id;
  ds_ucsd_last_call_info.rx_data_counter = call_info_ptr->rx_data_counter;
  ds_ucsd_last_call_info.tx_data_counter = call_info_ptr->tx_data_counter;
  ds_ucsd_last_call_info.sync_async =
    call_info_ptr->neg_cap.bearer_capability_1.sync_async;

  /* Clear watermarks to free all DSM items */
  dsm_empty_queue( &ds_ucsd_from_l2_wm );
  dsm_empty_queue( &ds_ucsd_to_l2_wm );
  dsm_empty_queue( &ds_ucsd_to_sio_wm );

#ifdef FEATURE_DATA_WCDMA_CS
  sio_q_cnt = dswcsd_get_from_sio_q_cnt();
  if ( sio_q_cnt > 0 )
  {
    DATA_MSG1_HIGH("Cleaning up UL dswcsd_from_sio_q_cnt(%d)",
              sio_q_cnt);
    dswcsd_ul_cleanup();
  }  
#endif /* FEATURE_DATA_WCDMA_CS */

  /* Perform common call end cleanup. */
  if( TRUE != ds_ucsd_call_end_common_cleanup( call_info_ptr->inst_id ) )
  {
    DATA_MSG0_ERROR("Call ended handler: problem in common cleanup");
  }
#ifdef FEATURE_DUAL_SIM
  ds3g_reset_cs_subs_id();
#endif /* FEATURE_DUAL_SIM */

  ds_gcsd_stop_general_purpose_timer();
} /* ds_ucsd_call_ended_handler() */

   

/*===========================================================================

FUNCTION ds_ucsd_at_escape_sequence_handler

DESCRIPTION
   From the 3GDSMGR header file....
   
   This handler is invoked when the AT escape sequence is detected, in
   online data mode. The return value indicates whether online command mode
   should be entered (in this case, data transfer over the air should be
   suspended), or if we should stay in online data mode. 
   
   In our case (UCSD subtask), we simply forward this indication on to the
   UCSD protocol stack task.
         
DEPENDENCIES
   None
  
RETURN VALUE
   An indication of whether or not the MT should transition to online
   command mode or not.  In our case we will always return the indication
   that the MT should go to on-line command mode. 
   
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL dsat_mode_enum_type 
ds_ucsd_at_escape_sequence_handler( void )
{
  uint8 index;

/*-------------------------------------------------------------------------*/
  
   DATA_MSG0_HIGH("AT escape sequence handler");

  if( MAX_UCSD_CALL_TBL_ENTRIES ==
      (index = ds_ucsd_find_call_instance_by_port( DS3G_SIOLIB_PORT_MAX )) )
  {
    DATA_MSG0_HIGH("Could not find external call port");
  }
  else
  {
    /* Get the call info record */
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
    const cm_bearer_capability_s_type *data_bc_ptr = NULL;

   /* Find the data bearer for possibly dual-mode call */
   (void)ds_ucsd_determine_call_type(
           &call_info_ptr->neg_cap.bearer_capability_1,
           &call_info_ptr->neg_cap.bearer_capability_2,
           &call_info_ptr->neg_cap.bc_repeat_indicator,
           &data_bc_ptr,
           NULL );
   ASSERT( NULL != data_bc_ptr );

#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
   if ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) )&&
        ( data_bc_ptr->sync_async == SYNCHRONOUS ) )
   {
      /* WCDMA Sync CS Data */

      /* Flush dswcsd_from_sio_q and reset relevant UL variables */
      dswcsd_at_esc_seq_ul_cleanup();
   }
#endif /* (FEATURE_DATA_WCDMA_CS) || FEATURE_TDSCDMA */

   if ( ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) ||
        ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) &&
          ( data_bc_ptr->sync_async == ASYNCHRONOUS ) ) )
   {
     /* GSM or WCDMA Async CS Data */

     /* Send command mode message to protocol stack task */
      ds_ucsd_mpshim_send_gcsd_msg ( DS_GCSD_CMD_MODE_REQ,
                             call_info_ptr->cm_call_id );
   }
  }
   
  /* Tell 3GDSMGR that we want to go to on-line command mode */
  return (DSAT_ONLINE_CMD);
} /* ds_ucsd_at_escape_sequence_handler() */




/*===========================================================================

FUNCTION ds_ucsd_at_return_to_online_data_handler

DESCRIPTION
   This handler is invoked when the ATO command (return to online data mode)
   is rx'ed in online command mode. It is invoked before the transition to
   return the SIO watermarks and callback function.
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void 
ds_ucsd_at_return_to_online_data_handler
(
  /* Pointer to the watermark for data from SIO */
  dsm_watermark_type     **from_sio_wm_ptr,   

  /* Pointer to the watermark for data to SIO. */
  dsm_watermark_type     **to_sio_wm_ptr,
  
  /* Pointer to SIO RX mode-specific handler function */
  sio_rx_func_ptr_type    *mode_hdlr_sio_rx_func_ptr
)
{
  uint8 index;

/*-------------------------------------------------------------------------*/
  
  ASSERT( NULL != from_sio_wm_ptr );
  ASSERT( NULL != to_sio_wm_ptr );
  ASSERT( NULL != mode_hdlr_sio_rx_func_ptr );
  
   DATA_MSG0_HIGH("Return to online data handler");

  if( MAX_UCSD_CALL_TBL_ENTRIES ==
      (index = ds_ucsd_find_call_instance_by_port( DS3G_SIOLIB_PORT_MAX )) )
  {
    DATA_MSG0_HIGH("Could not find external call port");
  }
  else
  {
    /* Get the call info record */
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );

    /* Switch statapath to that in call instance */
    *from_sio_wm_ptr            = call_info_ptr->dpath_params.ul_wm_ptr;
    *to_sio_wm_ptr              = call_info_ptr->dpath_params.dl_wm_ptr;
    *mode_hdlr_sio_rx_func_ptr  = call_info_ptr->dpath_params.sio_ul_rx_cb;

  }
} /* ds_ucsd_at_return_to_online_data_handler() */



/*===========================================================================

FUNCTION ds_ucsd_at_return_to_online_data_complete_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
   This handler is invoked when the ATO command (return to online data mode)
   is rx'ed in online command mode. The mode-specific handler should resume
   data transfer over the air.
   
   In our case, this simply involves sending an event notification down to
   the protocol stack.. 
   
         
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void 
ds_ucsd_at_return_to_online_data_complete_handler ( void )
{
  uint8 index;

/*-------------------------------------------------------------------------*/

  DATA_MSG0_HIGH("Return to online data complete handler");
   
  if( MAX_UCSD_CALL_TBL_ENTRIES ==
      (index = ds_ucsd_find_call_instance_by_port( DS3G_SIOLIB_PORT_MAX )) )
  {
    DATA_MSG0_HIGH("Could not find external call port");
  }
  else
  {
    /* Get the call info record */
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
    const cm_bearer_capability_s_type *data_bc_ptr = NULL;

    /* Find the data bearer for possibly dual-mode call */
    (void)ds_ucsd_determine_call_type(
      &call_info_ptr->neg_cap.bearer_capability_1,
      &call_info_ptr->neg_cap.bearer_capability_2,
      &call_info_ptr->neg_cap.bc_repeat_indicator,
      &data_bc_ptr,
      NULL );
    ASSERT( NULL != data_bc_ptr );

   
    if ( ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) ||
         ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) &&
           ( data_bc_ptr->sync_async == ASYNCHRONOUS ) ) )
    {
      /* GSM or WCDMA Async CS Data */

      /* Send command mode message to protocol stack task */
       ds_ucsd_mpshim_send_gcsd_msg( DS_GCSD_RECONNECT_REQ,
                                     call_info_ptr->cm_call_id );
    }
  }
} /* ds_ucsd_at_return_to_online_data_complete_handler() */



/*===========================================================================

FUNCTION ds_ucsd_user_answer_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
     This handler is invoked when the user or TE answers the call. The mode-
    specific handler should specify the ans_params, and optionally, set
    change_serial_mode to TRUE (and specify the additional serial parameters)
    if the serial mode needs to be changed.
    
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
LOCAL void
ds_ucsd_user_answer_handler
(
   cm_call_id_type        call_id,
   cm_ans_params_s_type   *ans_params,
   ie_cm_cc_cause_s_type  *cause_ptr,
   sys_modem_as_id_e_type  subs_id
)
{
  ds_ucsd_call_tbl_entry_type *call_info_ptr;
  uint8 index;
  
   /* Find call instance in the call table */
   if( MAX_UCSD_CALL_TBL_ENTRIES ==
       (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
   {
     DATA_MSG1_ERROR( "Cannot find call instance for CM call ID: %d",call_id );
     return;
   }
   call_info_ptr = DSUCSD_GET_CALL_INFO( index );
  
   MSG_HIGH( "User answer handler: CM call ID=%d",
             call_id,
             call_info_ptr->call_type,
             0 );

   ASSERT( NULL != ans_params );
   ASSERT( NULL != cause_ptr );
   ASSERT( call_id == call_info_ptr->cm_call_id );
#ifdef FEATURE_DUAL_SIM
   ASSERT( subs_id == call_info_ptr->subs_id);
#endif /* FEATURE_DUAL_SIM */

   memset( (void*)cause_ptr, 0x0, sizeof(ie_cm_cc_cause_s_type) );
   
   /*------------------------------------------------------------------------
     Set the common answer parameters
   ------------------------------------------------------------------------*/
   ans_params->info_type = CM_CALL_MODE_INFO_GW_CS;
   ans_params->ans_params.gw_cs_ans.call_type = CM_CALL_TYPE_CS_DATA;

   /*------------------------------------------------------------------------
     VT is the first CSD App to want to know if the call was rejected. If 
     we don't accept the incoming VT call, this will now get NAS to send a
     DISCONNECT w/cause_val = 21 (call rejected) 

     answer_params.reject is set in ds_ucsd_process_app_answer_call_cmd().
     Everywhere else, it's hardcoded.
   ------------------------------------------------------------------------*/
   if( TRUE == call_info_ptr->answer_params.reject )
   {
     /*----------------------------------------------------------------------
       Reject the incoming VT call. Only VT will potentially set this to 
       FALSE.
     ----------------------------------------------------------------------*/
     call_info_ptr->was_user_answered = FALSE;
     
     ans_params->info_type = CM_CALL_MODE_INFO_GW_CS;
     ans_params->ans_params.gw_cs_ans.accept = FALSE;

     /* Set the cause value specified earlier by client */
     cause_ptr->present = TRUE;
     cause_ptr->cause_value = call_info_ptr->answer_params.cause_info.cc_cause;
   }
   else
   {
     /*----------------------------------------------------------------------
       Accept incoming call
     ----------------------------------------------------------------------*/
     call_info_ptr->was_user_answered = TRUE;
     ans_params->ans_params.gw_cs_ans.accept = TRUE;
   }
} /* ds_ucsd_user_answer_handler() */



/*===========================================================================

FUNCTION ds_ucsd_auto_answer_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
   This handler is invoked when the user or TE answers the call. The
   mode-specific handler should specify the ans_params, and
   optionally, set change_serial_mode to TRUE (and specify the
   additional serial parameters) if the serial mode needs to be
   changed.
    
   This routine assumes only one call instance will be in ALERTING
   state at any time.
    
DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void
ds_ucsd_auto_answer_handler ( void )
{
  uint8 index =
    ds_ucsd_find_call_instance_by_state( DS_UCSD_CALL_STATE_ALERTING );/*lint !e641 !e655 */

  /* Stop alerting the user */
  ds3g_siolib_stop_alerting();

  /* Verify external call instance found */
  if( MAX_UCSD_CALL_TBL_ENTRIES == index )
  {
    /* Call instance in bad state, reply to external host */
    DATA_MSG0_ERROR("Call instance not in ALERTING state, rejecting auto answer");
    dsat_send_result_code( DSAT_NO_CARRIER );
  }
  else
  {
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
  
    DATA_MSG2_HIGH( "Auto answer handler: CM call ID %d  inst ID %d",
              call_info_ptr->cm_call_id, call_info_ptr->inst_id);
    call_info_ptr->state = DS_UCSD_CALL_STATE_ANSWERED;

    /* Update call instance to indicate call owned by external host */
    call_info_ptr->client_id = DSUCSD_EXTERNAL_CLIENT_ID;
    call_info_ptr->datapath_iface =  DS_UCSD_IFACE_SIO;
    call_info_ptr->call_ctrl_iface = DS_UCSD_IFACE_SIO;

    /* Answer the call */
    if ( ds3g_msh_answer_call_ex( call_info_ptr->cm_call_id,
#ifdef FEATURE_DUAL_SIM
                                  call_info_ptr->subs_id
#else
                                  SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                ) ==  DS3G_FAILURE )
    {
      DATA_MSG2_ERROR("Failed to answer the call: CM call ID = %d; CSD inst ID = %d",
                 call_info_ptr->cm_call_id, 
                 call_info_ptr->inst_id );

      /* Send NO_CARRIER result code to TE */  
      dsat_send_result_code( DSAT_NO_CARRIER );
    }
  }
} /* ds_ucsd_auto_answer_handler() */


/*===========================================================================

FUNCTION ds_ucsd_return_to_online_cmd_mode_complete_handler

DESCRIPTION
   From the 3GDSMGR header file....
   
   This handler is invoked when the DS_ENTER_ONLINE_CMD_* mode transition
   completes (i.e. SIO has been placed in online-command mode).  The mode
   specific handler can be used to notify the initiating task that the
   mode transition has completed.
   
   In our case (UCSD subtask), we simply forward this indication on to the
   UCSD protocol stack task.
         
DEPENDENCIES
   None
  
RETURN VALUE
   None   
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL void 
ds_ucsd_return_to_online_cmd_mode_complete_handler( void )
{
  uint8 index;
  
/*-------------------------------------------------------------------------*/

   DATA_MSG0_HIGH("Return to online cmd mode complete handler");
   
  if( MAX_UCSD_CALL_TBL_ENTRIES ==
      (index = ds_ucsd_find_call_instance_by_port( DS3G_SIOLIB_PORT_MAX )) )
  {
    DATA_MSG0_HIGH("Could not find external call port");
  }
  else
  {
    /* Get the call info record */
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
    const cm_bearer_capability_s_type  *data_bc_ptr = NULL;

   /* Find the data bearer for possibly dual-mode call */
   (void)ds_ucsd_determine_call_type(
           &call_info_ptr->neg_cap.bearer_capability_1,
           &call_info_ptr->neg_cap.bearer_capability_2,
           &call_info_ptr->neg_cap.bc_repeat_indicator,
           &data_bc_ptr,
           NULL );
   ASSERT( NULL != data_bc_ptr );
   
   if ( ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) ||
        ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) &&
          ( data_bc_ptr->sync_async == ASYNCHRONOUS ) ) )
   {
      /* GSM or WCDMA Async CS Data */

      /* Send command mode message to protocol stack task */
      ds_ucsd_mpshim_send_gcsd_msg ( DS_GCSD_CMD_MODE_REQ,
                              call_info_ptr->cm_call_id );
    }
   }
} /* ds_ucsd_return_to_online_cmd_mode_complete_handler() */



/*===========================================================================

FUNCTION ds_ucsd_prepare_call_origination

DESCRIPTION
  This function does the nesessary preparation for call origination.
  It is invoked by multiple parents to adddress originations within
  Data Services and by external subsystems.

DEPENDENCIES
   None
  
RETURN VALUE
   A return value of SUCCESS indicates that it is OK to originate, FAILURE 
   indicates that the call should not be originated.
    
SIDE EFFECTS
   None

===========================================================================*/
LOCAL boolean
ds_ucsd_prepare_call_origination
(
  ds_ucsd_call_tbl_entry_type       *call_info_ptr,
  const cm_bearer_capability_s_type *data_bc_ptr
)
{
   boolean  rc = TRUE;  /* Return code */

   ASSERT( NULL != call_info_ptr );
   ASSERT( NULL != data_bc_ptr );

   /* Check for bearer configuraiton initialized */
   if( TRUE == call_info_ptr->neg_cap.present )
   {
     /* Set the ds_ucsd_mobile_originated_call to TRUE */
      call_info_ptr->call_dir = DS_UCSD_CALL_DIR_ORIGINATED;
   
     /* Set the ds_ucsd_call_was_user_answered to FALSE */
     call_info_ptr->was_user_answered = FALSE;

     DATA_MSG0_HIGH("Orig BC structure:");
     ds_ucsd_print_bc_struct( data_bc_ptr );

     /* Setup for external call control interface */
     if( DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface )
     {
       /* Register command handlers with ATCOP.  Must be done before
        * SIOLIB registration. */
       rc = ds_ucsd_perform_atcop_registration( call_info_ptr, NULL );
     } 

     if( TRUE == rc )
     {
       /* Allocate external datapath */
       if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
       {
         /* Register with SIOLIB for external data port */
         if( TRUE !=
             ds_ucsd_perform_siolib_registration( call_info_ptr ) )
         {
           /* Registration failed. Deregister callback functions with ATCOP. */  
           if( DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface )
           {
             /* Call failed. Deregister callback functions with ATCOP. */  
             dsat_deregister_handlers();
           }
           rc = FALSE;
         }

       } /* DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface */
     
       /* Set the OTA data rate based on bearer configuration */
       call_info_ptr->ota_rate =
         ds_ucsd_map_fnur_waiur_to_ota_rate( data_bc_ptr->connection_element,
                                             ds_ucsd_network_mode,
                                             data_bc_ptr->fixed_network_user_rate,
                                             data_bc_ptr->wanted_air_interface_user_rate );
     }
   }
   else
   {
     DATA_MSG0_ERROR("Bearer capability not initialized, cannot originate");
     rc = FALSE;
   }
   
   if ( rc == TRUE ) 
   {
     if ( data_bc_ptr->sync_async == SYNCHRONOUS )
     {
       DATA_MSG1_HIGH( "Sync CS Data call originated: FNUR=%d", 
                 data_bc_ptr->fixed_network_user_rate);
         
#if defined ( FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA) 
       if ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) 
       {
         /* WCDMA/TD -SCDMA Sync CS Data */

#ifdef FEATURE_DATA_V80
           /*---------------------------------------------------------------
             The following is temporary. The one H.324 app we have used on 
             the TE was sending data prior to receiving the CONNECT. This 
             was to get around that. The corresponding flow enable is in the 
             call_connected handler.
           ---------------------------------------------------------------*/
           /*---------------------------------------------------------------
             Determine if this is a V.80 mode call. The magic numbers 6 and 8
             can be found in section 8.1 of ITU-T V.80. When these are passed
             to the UE vi +ES=6,,8, the DTE is putting the UE into Synchronous
             Access Mode of V.80.
           ---------------------------------------------------------------*/
         if( (DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface) &&
             (dsat_get_es_export_val(ES_ORIG_RQST_INDEX) == 6 &&
              dsat_get_es_export_val(ES_ANS_FBK_INDEX) == 8 ) )
           {
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
             if( ds_ucsd_sync_test_app_enabled == FALSE )
#endif
             {
               ds3g_siolib_ex_set_inbound_flow( DS_FLOW_WCDMACS_MASK, 
                                                DS_FLOW_DISABLE,
                                                call_info_ptr->sio_port_id);
             }
           }
#endif /* FEATURE_DATA_V80 */
         
       } /* WCDMA */
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_TDSCDMA*/
       
     } /* Synchronous CS Data */
     else
     {
       DATA_MSG1_HIGH("Async CS Data call originated: FNUR=%d", 
                 data_bc_ptr->fixed_network_user_rate);
     }
   } /* rc == TRUE */

   return rc;
} /* ds_ucsd_prepare_call_origination() */



/*===========================================================================

FUNCTION ds_ucsd_originate_call_handler

DESCRIPTION
   From the 3GDSMGR header file....
  
   This handler is invoked when the user initiates a data call. The
   mode-specific handler should specify the orig_params for the
   technology it will use.  The *_params_changed for the technology
   should also be set to TRUE.  The mode_spec_orig_info_ptr contains
   data that is passed to CM by the entity that wants to originate the
   call.  Current call models have the mode-specific handler (or some
   mode-specific entity) making that call.  The information within the
   struct is defined by the mode-spec modules.
    
   User must also fill out the SRV type of the call.
       
DEPENDENCIES
   None
  
RETURN VALUE
   A return value of SUCCESS indicates that it is OK to originate, FAILURE 
   indicates that the call should not be originated.
    
SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
LOCAL boolean
ds_ucsd_originate_call_handler
(
   const uint8                    *dial_string,
   uint8                           dial_string_len,
   cm_cdma_orig_params_s_type     *cdma_orig_params_ptr,
   boolean                        *cdma_params_changed,
   cm_gw_cs_orig_params_s_type    *gw_cs_orig_params_ptr,
   boolean                        *gw_cs_params_changed,
   cm_gw_ps_orig_params_s_type    *gw_ps_orig_params_ptr,
   boolean                        *gw_ps_params_changed,
   cm_srv_type_e_type             *cm_srv_type,
   void                           *mode_spec_info_ptr,
   sys_modem_as_id_e_type          subs_id
) 
{
   boolean  rc = DS3G_SUCCESS;  /* Return code */
   ds_ucsd_call_tbl_entry_type *call_info_ptr = NULL;
   ds_ucsd_chosen_capability_T   chosen_capability;
   cm_bearer_capability_s_type  *data_bc_ptr = NULL;
   nv_item_type      nv_item;    /* NV interface data */
   nv_stat_enum_type nv_status;  /* status var for reading from NV */

   ASSERT( NULL != dial_string );
   ASSERT( NULL != mode_spec_info_ptr );
   ASSERT( NULL != cdma_orig_params_ptr );
   ASSERT( NULL != cdma_params_changed );
   ASSERT( NULL != gw_cs_orig_params_ptr );
   ASSERT( NULL != gw_cs_params_changed );
   ASSERT( NULL != gw_ps_orig_params_ptr );
   ASSERT( NULL != gw_ps_params_changed );
   ASSERT( NULL != cm_srv_type );
   
   DATA_MSG0_HIGH("Originate call handler");
   
   /* Call instance pointer was passed into the DS3GMGR call initation
    * routine so we will have it in mode_spec_info_ptr. */
   call_info_ptr = (ds_ucsd_call_tbl_entry_type*)mode_spec_info_ptr;

   /* Set the ds_ucsd_mobile_originated_call to TRUE */
   call_info_ptr->call_dir = DS_UCSD_CALL_DIR_ORIGINATED;
   call_info_ptr->state = DS_UCSD_CALL_STATE_ORIG;
   
   /* Set the ds_ucsd_call_was_user_answered to FALSE */
   call_info_ptr->was_user_answered = FALSE;

   /* Set up the numerous params changed flags */
   *gw_cs_params_changed = TRUE;
   *gw_ps_params_changed = FALSE;
   *cdma_params_changed  = FALSE;
   
   /* Set the CM service type */
   *cm_srv_type = CM_SRV_TYPE_AUTOMATIC;
      
   /* Fill in the chosen_capability structure based on AT command
   ** or application inputs. This data will be gathered at the start of every
   ** MO and MT call.
   */
   {
     ASSERT( NULL != call_info_ptr );

     if ( ds_ucsd_get_chosen_cap( &call_info_ptr->bearer_params,
                                  &chosen_capability ) )
     {
       ds_ucsd_negotiated_capability_T negotiated_capability;
       memset((void*)&negotiated_capability,0x0,sizeof(negotiated_capability));
        
       /* Get RLP parameters for GSM or WCDMA Async CS Data */
       if ( ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) ||
            ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) &&
              ( chosen_capability.chosen_bc.sync_async == 
                ASYNCHRONOUS ) ) )
       {
          ds_ucsd_get_rlp_params (&call_info_ptr->rlp_parameters,subs_id);
       }

       if ( ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) ) &&
            ( chosen_capability.chosen_bc.sync_async == 
              SYNCHRONOUS ) )
       {
         /* Query call handling parameters from NVRAM */
#ifdef FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP
         /*-------------------------------------------------------------
           See if this is to be a Sync test app call. If the work
           around for flow controlling the TE in the orig handler is
           removed, this pulling of
           NV_WCDMACS_SYNC_TEST_APP_ENABLED_I from NV should move
           into the call_connected_hdlr. Doing this NV read in the
           connecteed hdlr will cover the case for both MO & MT
           calls.
           --------------------------------------------------------------*/
         ds_ucsd_sync_test_app_enabled =
           dswcsd_get_sync_test_app_enable_status(subs_id);
#endif /* FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP */

       }

       /* Now call the bearer capability function that will
       ** fill in the orig_params to be passed back to 3GDSMGR
       ** and also fill in the negotiated_capability structure 
       ** for us to keep track of what capabilities are active
       ** for this call.
       ** Call type determined and set in instance record.
       */
       ds_ucsd_bc_init_orig_params( call_info_ptr,
                                    dial_string,
                                    dial_string_len,             
                                    ds_ucsd_network_mode,
                                    &call_info_ptr->rlp_parameters,
                                    gw_cs_orig_params_ptr,          
                                    &chosen_capability,
                                    &negotiated_capability );

       nv_status = ds3gcfgmgr_read_legacy_nv( NV_DISABLE_CM_CALL_TYPE_I, &nv_item );
       if ( NV_DONE_S == nv_status )
       {
         if( (nv_item.disable_cm_call_type &CM_CALL_TYPE_VT_MASK) &&
             IS_VIDEO_CALL( call_info_ptr->call_type ) )
         {
           /* Reject the incoming VT data call */
           DATA_MSG0_HIGH("Originating VT CSD call rejected, NV_DISABLE_CM_CALL_TYPE_I");
           /* New call of this type cannot be supported. */
           (void)ds_ucsd_release_call_instance( call_info_ptr->inst_id );
           return DS3G_FAILURE;
         }
       }
       else
       {
         DATA_MSG1_ERROR("NV read failure NV status = %d",nv_status);
       }

       /* Check that the new call will not exceed concurrent call limits. */
       if( TRUE != ds_ucsd_check_concurrent_calls( call_info_ptr->call_type,
                                                   call_info_ptr->call_dir,
                                                   NULL ) )
       {
         /* New call of this type cannot be supported. */
         (void)ds_ucsd_release_call_instance( call_info_ptr->inst_id );
         rc = DS3G_FAILURE;
       }
       else
       {
         call_info_ptr->neg_cap = negotiated_capability;
         call_info_ptr->neg_cap.present = TRUE;
         data_bc_ptr = &call_info_ptr->neg_cap.bearer_capability_1;
         
         /* Populate the dial string modifiers */
         ds_ucsd_hdlr_init_orig_dial_mod_params( call_info_ptr,
                                                 gw_cs_orig_params_ptr );

         /*------------------------------------------------------------------
           Initialize and populate UUS info. By default, it's not present.
         ------------------------------------------------------------------*/
#ifdef FEATURE_UUS
         ds_ucsd_hdlr_init_orig_uus_info( call_info_ptr,
                                          gw_cs_orig_params_ptr );
#endif /* FEATURE_UUS */

         /* Finish preparation for call origination in common code. */
         rc = ds_ucsd_prepare_call_origination( call_info_ptr,
                                                data_bc_ptr );
         if( FALSE == rc )
         {
           /* Clear call instance on error */
           (void)ds_ucsd_release_call_instance( call_info_ptr->inst_id );
         }
       }
     }
     else
     {
       rc = DS3G_FAILURE;
       DATA_MSG0_ERROR("Can't originate call: invalid bearer service type settings");
     } /* ds_ucsd_get_chosen_cap */
   }

   if( rc != DS3G_FAILURE )
   {
     ds_gcsd_start_general_purpose_timer();
   }
   /* Indicate to 3GDSMGR whether or not it is OK to originate the call */
   return rc;
} /* ds_ucsd_originate_call_handler() */


#ifdef FEATURE_MULTIMEDIA_FALLBACK
#error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

#if defined (FEATURE_3GPP_CSFB) && defined (FEATURE_DATA_LTE)
/*===========================================================================
FUNCTION DS_UCSD_CNTXT_TRANSFER_IND_HANDLER

DESCRIPTION  This function processes CM_CALL_CNTXT_TRANSFER_IND from Call 
             Manager. This ind signifies that L<->GW inter RAT transition is
             in progress.

PARAMETERS
  sys_mode;          - target RAT    

DEPENDENCIES
  None.

RETURN VALUE
  None
   
SIDE EFFECTS

===========================================================================*/
LOCAL void ds_ucsd_cntxt_transfer_ind_handler
(
  sys_sys_mode_e_type      sys_mode,
  sys_modem_as_id_e_type   subs_id
)
{

  DATA_MSG1_HIGH("CNTXT_TRANSFER_IND in sys mode:%d",
             sys_mode);
  /*--------------------------------------------------------------------------
     Call the mode specific release and for each bearer for context
     transfer across LTE<->GW. Release hdlr will be called in source RAT,
     Setup handler will be called in Target RAT.
  --------------------------------------------------------------------------*/
  switch(sys_mode)
  {
    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
    case SYS_SYS_MODE_TDS:
      ds_ucsd_internal_mode = sys_mode;
      break;

    default:
      break;
  }

  DATA_MSG1_HIGH("Internal mode set to :%d", ds_ucsd_internal_mode);

}/* ds_ucsd_cntxt_transfer_ind_handler */

#endif /* FEATURE_3GPP_CSFB && FEATURE_DATA_LTE */


/*===========================================================================

FUNCTION ds_ucsd_incoming_call_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
    This handler is invoked when the CM_CALL_EVENT_INCOM event is rx'ed from
    Call Mgr. The mode-specific handler should specify the ans_params and
    ring type, and optionally, set change_serial_mode to TRUE (and specify
    the additional serial parameters) if the serial mode needs to be changed.
    The return value indicates the action to be taken: answer, notify user or
    reject call. The serial mode is changed only if the return value
    indicates answer.
         
DEPENDENCIES
   None
  
RETURN VALUE
   ds3g_incom_e_type value
    
SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
LOCAL ds3g_incom_e_type 
ds_ucsd_incoming_call_handler
(
  ds_cmd_type            *cmd_ptr,
  cm_call_id_type         call_id,
  cm_ans_params_s_type   *ans_params,
  cm_end_params_s_type   *end_params,
  sys_modem_as_id_e_type  subs_id
)
{
   ds_ucsd_call_type curr_call_type;
   const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
   dsat_result_enum_type   ring_type = DSAT_CRC_CODE_MIN; /* Ring type */
   cm_gw_cs_call_info_s_type *mode_info_ptr = NULL;
   ds_ucsd_call_tbl_entry_type *call_info_ptr = NULL;
   uint8 index;
   ds3g_call_info_cmd_type *call_info_cmd_ptr = NULL;

   ASSERT( NULL != cmd_ptr );
   ASSERT( NULL != cmd_ptr->cmd_payload_ptr );
   ASSERT( NULL != ans_params );
   ASSERT( NULL != end_params );

   call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;
   mode_info_ptr = &(call_info_cmd_ptr->mode_info.info.gw_cs_call);

   /* Find call instance in the call table */
   if( MAX_UCSD_CALL_TBL_ENTRIES <=
       (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
   {
     DATA_MSG1_ERROR( "Cannot find call instance for CM call ID: %d",call_id );
     return DS3G_REJECT_CALL;
   }
   call_info_ptr = DSUCSD_GET_CALL_INFO( index );
     
   /* Determine current call type (for dual mode calls) */
   curr_call_type =
     ds_ucsd_determine_call_type( &mode_info_ptr->bearer_capability_1,
                                  &mode_info_ptr->bearer_capability_2,
                                  &mode_info_ptr->bc_repeat_indicator,
                                  &data_bc_ptr,
                                  NULL );
   ASSERT( NULL != data_bc_ptr );
   
   DATA_MSG2_HIGH( "Incoming call handler: CM call ID=%d type=%d",
             call_id,
             curr_call_type );

   /* Set the ds_ucsd_call_was_user_answered to FALSE */
   call_info_ptr->was_user_answered = FALSE;

   /* Set the call end parameters */  
   end_params->call_id   = call_id;
   end_params->info_type = CM_CALL_MODE_INFO_GW_CS;
   
   /* We want to accept the call */
   ans_params->info_type = CM_CALL_MODE_INFO_GW_CS;
   ans_params->ans_params.gw_cs_ans.accept = TRUE;
   ans_params->ans_params.gw_cs_ans.call_type = CM_CALL_TYPE_CS_DATA;
   
   /* Check for accepting call */   
   if ( ans_params->ans_params.gw_cs_ans.accept == TRUE )
   {
     ds_ucsd_call_event_type event;

     if( TRUE !=
         ds_ucsd_decode_cbst_values( data_bc_ptr,
                                     &call_info_ptr->bearer_params.cbst_info.speed,
                                     &call_info_ptr->bearer_params.cbst_info.name,
                                     &call_info_ptr->bearer_params.cbst_info.connection_element ))
     {
       DATA_MSG0_ERROR("Failed to decode +CBST values"); 
     }
     
     /* Setup event information */
     event.call_event = CM_CALL_EVENT_INCOM;
     event.event_info.incoming.inst_id = call_info_ptr->inst_id;
     event.event_info.incoming.network_mode = ds_ucsd_network_mode;
     event.event_info.incoming.waiur =
       call_info_ptr->bearer_params.chsn_info.waiur;
     event.event_info.incoming.speed =
       call_info_ptr->bearer_params.cbst_info.speed;
     event.event_info.incoming.name =
       call_info_ptr->bearer_params.cbst_info.name;
     event.event_info.incoming.connection_element =
       call_info_ptr->bearer_params.cbst_info.connection_element;
     event.event_info.incoming.caller_number =
       ( call_info_ptr->calling_party_number.present == TRUE ) ?
       call_info_ptr->calling_party_number.data : NULL;
     event.event_info.incoming.caller_number_len =
       ( call_info_ptr->calling_party_number.present == TRUE ) ?
       call_info_ptr->calling_party_number.length : 0;
#ifdef FEATURE_UUS
     event.event_info.incoming.uus_data =
       ( call_info_ptr->uus1_data.mt_user_data.present == TRUE) ?
       call_info_ptr->uus1_data.mt_user_data.user_user_data : NULL;
     event.event_info.incoming.uus_data_len =
       ( call_info_ptr->uus1_data.mt_user_data.present == TRUE) ?
       call_info_ptr->uus1_data.mt_user_data.user_user_data_length : 0;
#endif /* FEATURE_UUS */
     
     /* Post notification to client(s) */
     ds_ucsd_post_client_event( call_info_ptr, &event);

     /* Check for SIO call control.  Also notify external host of
      * non-VT calls using API in case it wants to handle call. */
     if( (DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface) ||
         (!IS_VIDEO_CALL( call_info_ptr->call_type )) )
     {
       /* External call control */

#ifdef FEATURE_MULTIMEDIA_FALLBACK
       #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
       { 
         /* Set the ring type depending on call type */

#ifdef FEATURE_DATA_GCSD_FAX
         /* Fax call */
         if (data_bc_ptr->information_transfer_capability == ITC_FAX_G3)
         {
           ring_type = DSAT_CRC_CODE_FAX;
         }
         else 
#endif
         { 
           /* Asynchronous non-fax */
           if (data_bc_ptr->sync_async == ASYNCHRONOUS)
           {
             if (data_bc_ptr->connection_element == TRANSPARENT_DATA)
             { 
               ring_type = DSAT_CRC_CODE_ASYNC;     /* asynchronous transparent */
             }
             else
             {
               ring_type = DSAT_CRC_CODE_REL_ASYNC; /* asynchronous non-transparent */
             }
           }
           /* Synchronous non-fax */
           else
           {
             if (data_bc_ptr->connection_element == TRANSPARENT_DATA)
             { 
               ring_type = DSAT_CRC_CODE_SYNC;     /* synchronous transparent */
             }
             else
             {
               ring_type = DSAT_CRC_CODE_REL_SYNC; /* synchronous non-transparent */
             }

           }
         } /* information_transfer_capability == ITC_FAX_G3 */

         /* Register command handlers with ATCOP.  Must be done before
          * SIOLIB registration. */
         if( TRUE !=
             ds_ucsd_perform_atcop_registration( call_info_ptr, end_params ))
         {
           /* Reject the incoming call */
           ans_params->ans_params.gw_cs_ans.accept = FALSE;
           return ( DS3G_ANSWER_CALL );
         }

         /* Check for external datapath but also always alert external 
          * ports for non-VT calls. */
         if( (DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface) ||
             (!IS_VIDEO_CALL( call_info_ptr->call_type )))
         {
           /* Test for SIO port for external data call */
           if( TRUE == ds3g_siolib_is_external_port_free() )
           {
             ds3g_siolib_set_answer_callbacks( &ds_ucsd_siolib_func_tbl );
           }
         } /* DS_UCSD_IFACE_SIO == datapath_iface */
       } /* !IS_VOICE_CALL */
     } /* DS_UCSD_IFACE_API == call_ctrl_iface */


     if( TRUE == ans_params->ans_params.gw_cs_ans.accept )
     {
       /* Now alert the user */
       DATA_MSG0_HIGH("Incoming CS Data call accepted" );

       /* Check for external datapath but also always alert external 
        * ports for non-VT calls. */
       if( (DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface) ||
           (!IS_VIDEO_CALL( call_info_ptr->call_type )) )
       {
         boolean auto_answer = IS_VOICE_CALL( curr_call_type )? FALSE : TRUE; /*lint !e655 */
       
         /* Tell ATCOP to start alerting the user. Auto-answer the call
         ** if auto-answer is enabled by the S0 s-register and call is
         ** not in voice mode.
         */
         ds3g_siolib_alert_user( ring_type, auto_answer );
       }

       /* Promote call instance state */
       call_info_ptr->state = DS_UCSD_CALL_STATE_ALERTING;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
       #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */
       {
         /* Tell 3GDSMGR that we are alerting the user */
         return DS3G_ALERT_USER;
       }
     }
     else
     {
       /* In case of call handling error (e.g. no SIOLIB port
        * available), send user busy response.  Need to explicitly
        * answer call in this case to get USER_BUSY call control
        * cause code; CALL_REJECT cause sent otherwise. */
       DATA_MSG0_ERROR("Incoming CS Data call rejected with USER_BUSY cause" );
        
       call_info_ptr->answer_params.reject = TRUE;
       call_info_ptr->answer_params.cause_info.cc_cause = USER_BUSY;

       /* Answer the call, answer parameters read in later MSH callback */
       if ( ds3g_msh_answer_call_ex( call_info_ptr->cm_call_id,
#ifdef FEATURE_DUAL_SIM
                                     call_info_ptr->subs_id
#else
                                     SYS_MODEM_AS_ID_1
#endif /* FEATURE_DUAL_SIM */
                                   ) == DS3G_FAILURE )
       {
         /* If CM fails to answer the call, DSMGR will end the call */
         DATA_MSG2_ERROR("Failed to answer the call: "
                    "CM call ID = %d; CS Data call instance ID = %d", 
                    call_info_ptr->cm_call_id, 
                    call_info_ptr->inst_id);
         
         /* Perform common call end cleanup. */
         if( TRUE != ds_ucsd_call_end_common_cleanup( call_info_ptr->inst_id ) )
         {
           DATA_MSG0_ERROR("Incoming call handler: problem in common cleanup");
         }
         
         return ( DS3G_REJECT_CALL );
       }
       else
       {
         return ( DS3G_DO_NOTHING );
       }
     }
   }
   else
   {
     DATA_MSG0_ERROR( "Incoming CS Data call rejected");
     return ( DS3G_REJECT_CALL );
   } /* accept == TRUE */
} /* ds_ucsd_incoming_call_handler() */



/*===========================================================================

FUNCTION ds_ucsd_perform_call_switch

DESCRIPTION
  This function performs the processing for a call instance switch.
  It is very similair to call end handling.
         
DEPENDENCIES
   None
  
RETURN VALUE
   TBD
    
SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
LOCAL boolean
ds_ucsd_perform_call_switch
(
  ds_ucsd_call_tbl_entry_type        *call_info_ptr,    /* Existing call   */
  const cm_gw_cs_call_info_s_type    *mode_info_ptr,    /* New mode info   */
  const cm_bearer_capability_s_type  *data_bc_ptr,      /* Data bearer     */
  const ds_ucsd_call_type             curr_call_type    /* Current type    */
)
{
#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
  /* Pointer to WCDMA CS Data command */
  dswcsd_cmd_type  *wcsd_cmd_ptr;
#endif /*  (FEATURE_DATA_WCDMA_CS) || (FEATURE_TDSCDMA) */

  ds_ucsd_call_end_param_type end_param; /* the end reason parameters
                                          * passed in the app call
                                          * back
                                          */
  sys_modem_as_id_e_type            subs_id = ds3g_get_cs_subs_id();  
/*-------------------------------------------------------------------------*/
   
  ASSERT( NULL != call_info_ptr );
  ASSERT( NULL != data_bc_ptr);
  
#ifdef FEATURE_DUAL_SIM
  subs_id = call_info_ptr->subs_id;
#endif
  
  /* Call collision: we have an outgoing CS Data call and an incoming CS 
  ** Data call at the same time, assuming there is no more than one incoming
  ** or outgoing call at the same time. End the outgoing call. 
  */

  /* Check for external call control */
  if( DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface )
  {
    /* Ensure SIOLIB port was successfully allocated */
     if( DS3G_SIOLIB_PORT_NONE != call_info_ptr->sio_port_id )
    {
      /* Set Carrier Detect signal to tell TE the call is down */
       (void)ds3g_siolib_ex_set_cd_state( DS3G_SIOLIB_REMOTE_CARRIER_OFF,
                                          call_info_ptr->sio_port_id );
    }

    /* Put out NO CARRIER to TE */
     dsat_send_result_code_ex (DSAT_NO_CARRIER,call_info_ptr->sio_port_id);
  } /* DS_UCSD_IFACE_SIO == call_ctrl_iface */

#if defined (FEATURE_DATA_WCDMA_CS) || defined (FEATURE_TDSCDMA)
  if ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) )
  {
    /* TD_SCDMA CS Data */
    /* Deregister interface with Layer 2 - TD_SCDMA RLC */
#ifdef FEATURE_TDSCDMA
#ifdef FEATURE_SEGMENT_LOADING 
#ifdef FEATURE_DUAL_WCDMA
   if( (ds_3gpp_csd_tds_tbl != NULL) && 
       (ITDSCDMA_rrc_is_tds_active_sub(ds_3gpp_csd_tds_tbl, subs_id) == TRUE) )
#else
    if( (ds_3gpp_csd_tds_tbl != NULL) && (ITDSCDMA_rrc_is_tds_active(ds_3gpp_csd_tds_tbl) == TRUE) )
#endif
#else
#ifdef FEATURE_DUAL_WCDMA
     if(rrc_is_tds_active_sub(subs_id) == TRUE)
#else
    if(rrc_is_tds_active() == TRUE)
#endif
#endif /* FEATURE_SEGMENT_LOADING */
    {
      if( TRUE != ds_ucsd_mpshim_deregister_tdscdma_rlc(subs_id) )
      {
        DATA_MSG0_ERROR("Problem releasing TD_SCDMA RLC interface");
        return FALSE;
      }
    }
    else 
#endif /* FEATURE_TDSCDMA */
    /* WCDMA CS Data */
    {
      /* Deregister the RLC UL and DL watermarks */
      if( TRUE != ds_ucsd_mpshim_deregister_wcdma_rlc(subs_id) )
      {
        DATA_MSG0_ERROR("Problem deregistering from L2");
      }
    }
   /* Check for external datapath */
   if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
   {
     /* Tell WCSD UL/DL tasks to stop transmitting/receiving data and dequeue
     ** the remaining data on the watermarks
     */
     if ( data_bc_ptr->sync_async == SYNCHRONOUS )
     {
       /* WCDMA Sync CS Data */

       if ( ( wcsd_cmd_ptr = dswcsd_ul_get_cmd_buf() ) != NULL ) 
       {
         wcsd_cmd_ptr->hdr.cmd_id = DSWCSD_END_CALL_CMD;

         dswcsd_ul_put_cmd( wcsd_cmd_ptr );
       }
       else
       {
         DS_3GPP_MSG0_ERROR( "Cannot get WCSD UL cmd buffer");
       }

       if ( ( wcsd_cmd_ptr = dswcsd_dl_get_cmd_buf() ) != NULL ) 
       {
         wcsd_cmd_ptr->hdr.cmd_id = DSWCSD_END_CALL_CMD;

         dswcsd_dl_put_cmd( wcsd_cmd_ptr );
       }
       else
       {
         DS_3GPP_MSG0_ERROR( "Cannot get WCSD DL cmd buffer");
       }

       /* Clear the queue for the Downlink SIO watermark and return buffers 
       ** to the DSM item pool.
       */
       EMPTY_WATERMARK( call_info_ptr->dpath_params.dl_wm_ptr );

     } /* WCDMA Sync CS Data */
   } /* DS_UCSD_IFACE_SIO == datapath_iface */
   } /*  ds_ucsd_network_mode == SYS_SYS_MODE_WCDMA */
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_TDSCDMA */

  if ( ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) ||
       ( DS_UCSD_IS_WCDMA_TDS_MODE(ds_ucsd_network_mode) &&
         ( data_bc_ptr->sync_async == ASYNCHRONOUS ) ) )
  {
    /* GSM or WCDMA Async CS Data */

    /* Send a MT disconnect to protocol stack... this type of disconnect will
    ** disconnect and reset the protocol stack immediately
    */
    ds_ucsd_mpshim_send_gcsd_msg ( DS_GCSD_DISCONNECT_MT_REQ,
                                   call_info_ptr->cm_call_id );
  }
  
  /* Check for external datapath */
  if( DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface )
  {
    /* Deregister from SIOLIB */
     if( DS3G_SIOLIB_PORT_NONE != call_info_ptr->sio_port_id )
    {
      /* Ensure call was connected before resetting SIOLIB mode */
      if( (DS_UCSD_CALL_STATE_CONNECT_LL == call_info_ptr->state) ||
          (DS_UCSD_CALL_STATE_CONNECT    == call_info_ptr->state) )
      {
        /* Change SIO mode back to AUTODECTECT */
         if (ds3g_siolib_ex_change_mode (SIO_DS_AUTODETECT_MODE,
                                     NULL,
                                     NULL,
                                     NULL,
                                     call_info_ptr->sio_port_id) 
                                   != DS3G_SIOLIB_OK)
        {
          DATA_MSG0_ERROR("Failed to set SIO to AUTODETECT mode");
        }
      }

       if ( ds3g_siolib_ex_deregister_callback_func (&ds_ucsd_siolib_func_tbl,
                                                     call_info_ptr->sio_port_id) == 
           FALSE )
      {
        DATA_MSG0_ERROR("Error in SIOLIB deregister");
      }
       ds3g_siolib_ex_release_port ( call_info_ptr->sio_port_id );
       call_info_ptr->sio_port_id = DS3G_SIOLIB_PORT_NONE;
    }

#if defined(FEATURE_DATA_WCDMA_CS) || defined(FEATURE_TDSCDMA)
#ifdef FEATURE_DATA_V80
    /*----------------------------------------------------------------------
      If it was a V.80 call, make sure we re-enable flow from the TE. Flow
      is temporarily disbaled in the orig_handler. It's re-enabled in the
      connected_handler, but if the call didn't connect, clean up here.
    ----------------------------------------------------------------------*/
    if( dsat_get_es_export_val(ES_ORIG_RQST_INDEX) == 6 &&
        dsat_get_es_export_val(ES_ANS_FBK_INDEX) == 8 )
    {
       ds3g_siolib_ex_set_inbound_flow( DS_FLOW_WCDMACS_MASK, 
                                        DS_FLOW_ENABLE ,
                                        call_info_ptr->sio_port_id);
    }
#endif /* FEATURE_DATA_V80 */
#endif /* FEATURE_DATA_WCDMA_CS || FEATURE_TDSCDMA */
  } /* DS_UCSD_IFACE_SIO == datapath_iface */ 

  /* Check for API client call control */
  if( DS_UCSD_IFACE_API == call_info_ptr->call_ctrl_iface )
  {
    ds_ucsd_call_event_type event;
    
    /* Setup call end event information */
    end_param.end_cause = CM_CALL_END_INTERNAL_ERROR;
    if( NULL != mode_info_ptr )
    {
      end_param.cc_param.cc_cause_present = mode_info_ptr->cc_cause.present;
      end_param.cc_param.cc_cause = mode_info_ptr->cc_cause.cause_value;
      end_param.rej_param.present = mode_info_ptr->cc_reject.present;
      end_param.rej_param.rej_type = mode_info_ptr->cc_reject.rej_type;
      end_param.rej_param.rej_reason = mode_info_ptr->cc_reject.rej_value;
    }
    else
    {
      /* We do not know/have the cc_cause/reject cause for this
       * call termination. Flag them accoringly. */
      end_param.cc_param.cc_cause_present = FALSE;
      end_param.rej_param.present = FALSE;
    }
    
    event.call_event = CM_CALL_EVENT_END;
    event.event_info.end.inst_id = call_info_ptr->inst_id;
    event.event_info.end.end_param = &end_param;

    /* Post notification to client(s) */
    ds_ucsd_post_client_event( call_info_ptr, &event);
  }
  else
  {
    /* External call control */
    
    /* De-register from ATCOP */
    dsat_deregister_handlers();
  }

  return TRUE;
} /* ds_ucsd_perform_call_switch() */


/*===========================================================================

FUNCTION ds_ucsd_switch_call_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
    This handler is used when a mode-specific handler has an origination 
    in progress when an incoming page of the same type appears.  Mode-spec
    handler must end originating call.  It can accept or reject or do 
    nothing with incoming call (normal incoming call processing).
    
    There are two end_params.  The first is for the originating call, which
    is always ended.  The second is for the incoming call.  That call may
    also be ended (but is not required to be ended).
         
DEPENDENCIES
   None
  
RETURN VALUE
   ds3g_incom_e_type value
    
SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
ds3g_incom_e_type 
ds_ucsd_switch_call_handler
(
   ds_cmd_type            *cmd_ptr,
   cm_call_id_type         inc_call_id,
   cm_call_id_type         exist_call_id,
   cm_ans_params_s_type   *ans_params,
   cm_end_params_s_type   *end_params_orig,
   cm_end_params_s_type   *end_params_inc,
   sys_modem_as_id_e_type  subs_id
)
{
   cm_gw_cs_call_info_s_type * gw_cs_call_ptr = NULL;
   ds_ucsd_call_tbl_entry_type *call_info_ptr;
   uint8 index;
   ds_ucsd_call_type curr_call_type;
   const cm_bearer_capability_s_type  *data_bc_ptr = NULL;
   ds3g_call_info_cmd_type    *call_info_cmd_ptr = NULL;
/*-------------------------------------------------------------------------*/

   ASSERT( NULL != cmd_ptr );
   ASSERT( NULL != cmd_ptr->cmd_payload_ptr );
   ASSERT( NULL != ans_params );
   ASSERT( NULL != end_params_orig );
   ASSERT( NULL != end_params_inc );

   call_info_cmd_ptr = (ds3g_call_info_cmd_type*)cmd_ptr->cmd_payload_ptr;

   gw_cs_call_ptr = &(call_info_cmd_ptr->mode_info.info.gw_cs_call);
   
   /* Find call instance in the call table */
   if( MAX_UCSD_CALL_TBL_ENTRIES <=
       (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( exist_call_id ))) )
   {
     MSG_ERROR( "Cannot find call instance for CM call ID: %d",exist_call_id,0,0 );
     return DS3G_REJECT_CALL;
   }
   call_info_ptr = DSUCSD_GET_CALL_INFO( index );
   
   curr_call_type =
     ds_ucsd_determine_call_type( &gw_cs_call_ptr->bearer_capability_1,
                                  &gw_cs_call_ptr->bearer_capability_2,
                                  &gw_cs_call_ptr->bc_repeat_indicator,
                                  &data_bc_ptr,
                                  NULL );
   ASSERT( NULL != data_bc_ptr);
   
   MSG_HIGH( "Switch call handler: incomID=%d existID=%d type=%d",
             inc_call_id,
             exist_call_id,
             curr_call_type );
   
   if( call_info_ptr->cm_call_id == exist_call_id )
   {
     /* Set end_params for old call  */
     end_params_orig->call_id   = exist_call_id;
     end_params_orig->info_type = CM_CALL_MODE_INFO_GW_CS;

     /* Clear the existing call for switch */
     (void)ds_ucsd_perform_call_switch( call_info_ptr,
                                        gw_cs_call_ptr,
                                        data_bc_ptr,
                                        curr_call_type );

        
     /* Free the call instance */
     (void)ds_ucsd_release_call_instance( call_info_ptr->inst_id );

   }
   
   return DS3G_ANSWER_CALL;
} /* ds_ucsd_switch_call_handler() */



/*===========================================================================

FUNCTION ds_ucsd_force_call_down_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
    This handler is used when there is an incoming call which will override
    an originating call.  This tells the mode-speci handler that their
    call should be ended immediately.
    
    NOTE:  the mode-specific handler will not get a call_ended() after this.
    This is the only indication the mode-specific handler will get that
    their call has been torn down.
    
             
DEPENDENCIES
   None
  
RETURN VALUE
    A return of TRUE indicates that there was a call to be torn down.
    FALSE indicates that the mode-spec handler had no active/originating
    calls.
    
SIDE EFFECTS
   None

===========================================================================*/
boolean
ds_ucsd_force_call_down_handler
(
   cm_call_id_type         exist_call_id,
   cm_end_params_s_type   *end_params,
   sys_modem_as_id_e_type  subs_id
)
{
   /* Return code */
   boolean rc = FALSE;
   
   ds_ucsd_call_tbl_entry_type *call_info_ptr;
   uint8 index;
   ds_ucsd_call_type curr_call_type;
   const cm_bearer_capability_s_type  *data_bc_ptr = NULL;

/*-------------------------------------------------------------------------*/

   if( NULL == end_params )
   {
     DATA_MSG0_ERROR("NULL pointer parameter specified");
     return DS3G_FAILURE;
   }

   /* Find call instance in the call table */
   if( MAX_UCSD_CALL_TBL_ENTRIES <=
       (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( exist_call_id ))) )
   {
     DATA_MSG1_ERROR( "Cannot find call instance for CM call ID: %d",exist_call_id );
     return DS3G_FAILURE;
   }
   call_info_ptr = DSUCSD_GET_CALL_INFO( index );
   
   curr_call_type =
     ds_ucsd_determine_call_type( &call_info_ptr->neg_cap.bearer_capability_1,
                                  &call_info_ptr->neg_cap.bearer_capability_2,
                                  &call_info_ptr->neg_cap.bc_repeat_indicator,
                                  &data_bc_ptr,
                                  NULL );
   ASSERT( NULL != data_bc_ptr );
   
   MSG_HIGH( "Force call down handler: callID=%d type=%d",
             exist_call_id,
             curr_call_type,
             0 );

   if( call_info_ptr->cm_call_id == exist_call_id )
   {
     /* Set end_params for old call  */
      end_params->call_id   = exist_call_id;
      end_params->info_type = CM_CALL_MODE_INFO_GW_CS;

     /* Clear the existing call for force down, whihc is the same as a
      * switch for UMTS CSD calls. */
     (void)ds_ucsd_perform_call_switch( call_info_ptr,
                                        NULL,
                                        data_bc_ptr,
                                        curr_call_type );

     /* Free the call instance */
     (void)ds_ucsd_release_call_instance( call_info_ptr->inst_id );
   

     rc = TRUE;
   }
   return rc;
} /* ds_ucsd_force_call_down_handler() */

/* TODO: Remove?? */
#if 0
/*===========================================================================

FUNCTION ds_ucsd_get_call_connection_info_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
    This handler is invoked from the function ds_get_call_information(),
    which is invoked by UI when a call is established. The mode-specific 
    handler should return by reference the call connection information, which
    is currently a NULL-terminated string indicating the system or service 
    option, e.g., "WCDMA", for the call. The handler checks the size of the 
    buffer for *system_information before it populates the buffer. 

         
DEPENDENCIES
   None
  
RETURN VALUE
   boolean: The return value indicates if the call connection information 
   is valid.
    
SIDE EFFECTS
   None

===========================================================================*/
/*ARGSUSED*/
boolean 
ds_ucsd_get_call_connection_info_handler
(
   cm_call_id_type     call_id,
   uint8              *system_information, 
   uint8               max_system_information_length
)
{
   char    sys_info[6];
   boolean rc;
   ds_ucsd_call_tbl_entry_type *call_info_ptr;
   uint8 index;

   /* Find call instance in the call table */
   if( MAX_UCSD_CALL_TBL_ENTRIES ==
       (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
   {
     DATA_MSG1_ERROR("Cannot find call instance for CM call ID: %d",call_id );
     return DS3G_FAILURE;
   }
   call_info_ptr = DSUCSD_GET_CALL_INFO( index );
   
   ASSERT (call_id == call_info_ptr->cm_call_id);
   
   sys_info[0] = '\0';

   /* Set the system info string based on the network mode */
   if ( ds_ucsd_network_mode == SYS_SYS_MODE_WCDMA ) 
   {
     (void)strlcpy( (char *)sys_info, "WCDMA", sizeof(sys_info) );
   }
#ifdef FEATURE_TDSCDMA
   else if( ds_ucsd_network_mode == SYS_SYS_MODE_TDS )
   {
     (void)strlcpy( (char *)sys_info, "TD-SCDMA", sizeof(sys_info) );
   }
#endif /* FEATURE_TDSCDMA */
   else if ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM )
   {
     (void)strlcpy( (char *)sys_info, "GSM", sizeof(sys_info) );
   }

   if (max_system_information_length > strlen (sys_info))
   { 
     (void)strlcpy ((char *)system_information,
                        sys_info,
                        max_system_information_length);
      rc = TRUE;
   }
   else
   {
      rc = FALSE;
   }
  
   return rc;   
   
} /* ds_ucsd_get_call_connection_info_handler() */
#endif

/*===========================================================================

FUNCTION ds_ucsd_set_data_counters

DESCRIPTION
  This function sets the Tx/Rx data couters for a specified call ID.
  If incremental flag is set, current call counters are incremented.
  Otherwise, specified absolute values overwrite previous values.

DEPENDENCIES
  None
  
RETURN VALUE
  boolean:  The return value indicates if data call call updated.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_set_data_counters
(
   cm_call_id_type    call_id,
  boolean            incremental,
   uint32             rx_from_network_byte_cnt, 
   uint32             tx_to_network_byte_cnt
)
{
  ds_ucsd_call_tbl_entry_type *call_info_ptr;
  uint8 index;
  
  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES ==
      (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( call_id ))) )
  {
    DATA_MSG1_ERROR( "Cannot find call instance for CM call ID: %d",call_id );
    return DS3G_FAILURE;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );

  if( incremental )
  {
    call_info_ptr->rx_data_counter += rx_from_network_byte_cnt;
    call_info_ptr->tx_data_counter += tx_to_network_byte_cnt;
  }
  else
  {
    call_info_ptr->rx_data_counter = rx_from_network_byte_cnt;
    call_info_ptr->tx_data_counter = tx_to_network_byte_cnt;
  }

  return TRUE;
} /* ds_ucsd_set_data_counters() */



/*===========================================================================

FUNCTION ds_ucsd_get_data_counters

DESCRIPTION
  This function returns the Tx/Rx data counters for a specified call.

DEPENDENCIES
  None
  
RETURN VALUE
  boolean:  The return value indicates if metrics are valid.
    
SIDE EFFECTS
  None

===========================================================================*/
boolean 
ds_ucsd_get_data_counters
(
  uint8   inst_id,
  uint32  *rx_from_network_byte_cnt, 
  uint32  *tx_to_network_byte_cnt
)
{
  uint8 index;
  ds_ucsd_call_tbl_entry_type *call_info_ptr;

  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( inst_id )) )
  {
    DATA_MSG1_MED("Cannot find call instance: %d",inst_id);
    return FALSE;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );
  
  *rx_from_network_byte_cnt = call_info_ptr->rx_data_counter;
  *tx_to_network_byte_cnt   = call_info_ptr->tx_data_counter;
  DATA_MSG2_MED("ds_ucsd_get_data_counters: tx=%d rx=%d",
          *tx_to_network_byte_cnt, *rx_from_network_byte_cnt);

  return TRUE;
} /* ds_ucsd_get_data_counters() */

#ifdef FEATURE_TDSCDMA
/*===========================================================================

FUNCTION ds_ucsd_update_network_mode

DESCRIPTION
  This function updates the ds_ucsd_network_mode mode for a call when
  we handle cm call connected command.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None
    
SIDE EFFECTS
  None

===========================================================================*/
void ds_ucsd_update_network_mode
(
  sys_sys_mode_e_type call_mode                        /* network mode */
)
{
  ds_ucsd_network_mode = call_mode;
}/* ds_ucsd_update_network_mode */
#endif /* FEATURE_TDSCDMA */

/*===========================================================================

FUNCTION ds_ucsd_get_data_counters_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
    This handler is invoked from the function 
    ds_get_current_session_data_counters(), which is periodically invoked by
    UI during a data call. The mode-specific handler should return by 
    reference the total count of bytes transmitted and received at RLC for 
    the current call. The total byte count is accumulative for the call. If 
    RAB is re-established after PDP context is preserved during the call, the
    total byte count includes bytes transmitted and received after RAB is 
    re-established.
             
DEPENDENCIES
   None
  
RETURN VALUE
   boolean:  The return value indicates if the byte count is valid.
    
SIDE EFFECTS
   None

===========================================================================*/
boolean 
ds_ucsd_get_data_counters_handler
(
   cm_call_id_type    call_id,
   uint32            *rx_from_network_byte_cnt, 
   uint32            *tx_to_network_byte_cnt
)
{  
  if( (NULL == rx_from_network_byte_cnt) ||
      (NULL == tx_to_network_byte_cnt) )
  {
    DATA_MSG0_ERROR("NULL pointer parameter specified");
    return FALSE;
  }

  return ds_ucsd_get_data_counters( DSUCSD_XLATE_CALLID( call_id ),
                                    rx_from_network_byte_cnt,
                                    tx_to_network_byte_cnt );
  
} /* ds_ucsd_get_data_counters_handler() */

/* TODO: Remove?? */
#if 0
/*===========================================================================

FUNCTION ds_ucsd_get_last_session_data_counters_handler

DESCRIPTION
   From the 3GDSMGR header file...
   
      This handler is invoked from the function
      ds_get_last_session_data_counters(), which is invoked by
      UI at the end of a data call. The mode-specific handler should return by
      reference the total count of bytes transmitted and received at Link Layer
      for the previous call.
      DS just returns the counter values for the last call which terminated
      or the current call (which could be in the process of being released)

      The total byte count is accumulative for the call. If
      RAB is re-established after PDP context is preserved during the call, the
      total byte count includes bytes transmitted and received after RAB is
      re-established. The return value indicates if the byte count is valid.
                   
DEPENDENCIES
   None
  
RETURN VALUE
   boolean:  The return value indicates if the byte count is valid.
    
SIDE EFFECTS
   None

===========================================================================*/
boolean
ds_ucsd_get_last_session_data_counters_handler
(
   uint32 *rx_from_network_byte_cnt,
   uint32 *tx_to_network_byte_cnt
)
{
   if( (NULL == rx_from_network_byte_cnt) ||
       (NULL == tx_to_network_byte_cnt) )
   {
     DATA_MSG0_ERROR( "NULL pointer parameter specified");
     return FALSE;
   }

   if( TRUE == ds_ucsd_last_call_info.present )
   {
     *rx_from_network_byte_cnt = ds_ucsd_last_call_info.rx_data_counter;
     *tx_to_network_byte_cnt   = ds_ucsd_last_call_info.tx_data_counter;
     MSG_MED("ds_ucsd_get_last_session_data_counters_handler: tx=%d rx=%d",
             *tx_to_network_byte_cnt, *rx_from_network_byte_cnt, 0);
   }
   else
   {
      return FALSE;
   }

   return TRUE;
} /* ds_ucsd_get_last_session_data_counters_handler() */
#endif

/*===========================================================================

FUNCTION ds_ucsd_dial_string_handler

DESCRIPTION
   This function is the UMTS CS dial string handler that is called
   by ATCOP when a UMTS CS call is to be initiated. 
   
        
DEPENDENCIES
   None
  
RETURN VALUE
   An abort function callback
    
SIDE EFFECTS
   None

===========================================================================*/
dsat_call_abort_cb_type
ds_ucsd_dial_string_handler
(
   const uint8                   *dial_string,    /* Dial string           */
   boolean                        digit_mode,     /* Non-digit or digits   */
  dsat_dial_modifier_info_type  *modifiers,       /* Dial string modifiers */
  sys_modem_as_id_e_type        subs_id
)
{
  dsat_call_abort_cb_type rc = NULL;
  uint8 index;
  uint8 inst_id;
  ds_ucsd_bearer_service_type   bearer_service;
  dsat_num_item_type tmp_val;

/*-------------------------------------------------------------------------*/

  ASSERT( NULL != dial_string );
  ASSERT( NULL != modifiers );
  
#ifdef FEATURE_DUAL_SIM
  if ( (SYS_MODEM_AS_ID_NONE == subs_id) || (SYS_MODEM_AS_ID_MAX == subs_id) )
  {
    DATA_MSG1_ERROR("Invalid subs_id provided: %d", subs_id);
    return NULL;
  }
#endif /* FEATURE_DUAL_SIM */
  
  /*-------------------------------------------------------------------------
    Get a CS Data call instance ID.
  -------------------------------------------------------------------------*/
  inst_id = ds_ucsd_get_instid( &index );
  if ( inst_id != NO_CALL_ID ) 
  {
    /* Get the call info record */
    ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO( index );
    
#ifdef FEATURE_DUAL_SIM
    /* Get the current subs_id */
    ds_ucsd_subs_id = subs_id;
    /* Get the current network mode */
    ds_ucsd_network_mode = 
            ds3g_get_current_network_mode_ex(ds_ucsd_subs_id);
    /* set the current subs_id in ds3g for CS call origination */
    ds3g_set_cs_subs_id(ds_ucsd_subs_id);
#else
    /* Get the current network mode */
    ds_ucsd_network_mode = ds3g_get_current_cs_domain_mode();
#endif /* FEATURE_DUAL_SIM */

    if( INVALID_3GPP_CS_SYS_MODE(ds_ucsd_network_mode) )
    {
#ifdef FEATURE_3GPP_CSFB
#ifdef FEATURE_SEGMENT_LOADING
#ifdef FEATURE_TDSCDMA
      if(ds_3gpp_csd_tds_tbl != NULL)
      {
        /* Set the mode currently to TD-SCDMA */
        ds_ucsd_network_mode = SYS_SYS_MODE_TDS;
      }
      else
#endif
      {
        /* Set the mode currently to WCDMA */
        ds_ucsd_network_mode = SYS_SYS_MODE_WCDMA;
      }
#elif defined(FEATURE_TDSCDMA)
      /* Set the mode currently to TD-SCDMA */
      ds_ucsd_network_mode = SYS_SYS_MODE_TDS;
#else
      /* Set the mode currently to WCDMA */
      ds_ucsd_network_mode = SYS_SYS_MODE_WCDMA;
#endif /* FEATURE_SEGMENT_LOADING */
#endif /* FEATURE_3GPP_CSFB */
    }


    if ( ds_ucsd_network_mode == SYS_SYS_MODE_GSM ) 
    {
      DATA_MSG0_HIGH( "Initiating GSM CS data call");
    }
    else if ( ds_ucsd_network_mode == SYS_SYS_MODE_WCDMA ) 
    {
      DATA_MSG0_HIGH( "Initiating WCDMA CS data call");
    }
#ifdef FEATURE_TDSCDMA
    else if ( ds_ucsd_network_mode == SYS_SYS_MODE_TDS ) 
    {
      DATA_MSG0_HIGH( "Initiating TD_SCDMA CS data call");
    }
#endif /* FEATURE_TDSCDMA */
    /* Preserve the call bearer configuraiton settings */
    tmp_val = (dsat_num_item_type) dsatutil_get_val(DSATETSI_EXT_CHSN_IDX,
                                     0,ATCOP_CHSN_WAIUR_POSITION,NUM_TYPE);
    ds_ucsd_store_bearer_service_type(
      &bearer_service,
      (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_SPEED_POSITION),
      (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_NAME_POSITION),
      (uint8)*dsat_get_cbst_export_val(ATCOP_CBST_CE_POSITION),
      (uint8)tmp_val );	

   
    /* Update instance object with call info.  */
    ds_ucsd_store_call_info(
      index,
      ds_ucsd_network_mode,
#ifdef FEATURE_DUAL_SIM
      ds_ucsd_subs_id,
#endif /* FEATURE_DUAL_SIM */
      DS_UCSD_UNKNOWN_CALL, /* Determined in originate_call_handler */
      &bearer_service,
      NULL,
      NULL,
      NULL );

    /* Set the call control and datapath interfaces */
    call_info_ptr->call_ctrl_iface = DS_UCSD_IFACE_SIO;
    call_info_ptr->datapath_iface  = DS_UCSD_IFACE_SIO;

    call_info_ptr->client_id = DSUCSD_EXTERNAL_CLIENT_ID;
    call_info_ptr->call_mode = DSUCSD_CALL_MODE_DATA;
    
    /* Preserve the SupSrv dial string modifiers */   
    call_info_ptr->dial_string_modifiers.modifier_present   =
      modifiers->modifier_present;
    call_info_ptr->dial_string_modifiers.modifier           =
     (ds_ucsd_dial_modifier_e_type)modifiers->modifier;
    call_info_ptr->dial_string_modifiers.modifier_info.cli.cli_enabled  =
      modifiers->modifier_info.cli.cli_enabled;

    call_info_ptr->dial_string_modifiers.modifier_info.cug.cug_enabled  =
      modifiers->modifier_info.cug.cug_enabled;
    call_info_ptr->dial_string_modifiers.modifier_info.cug.cug_index_enabled =
      modifiers->modifier_info.cug.cug_index_enabled;
    call_info_ptr->dial_string_modifiers.modifier_info.cug.cug_index_val =
      modifiers->modifier_info.cug.cug_index_val;
    call_info_ptr->dial_string_modifiers.modifier_info.cug.suppress_pref_cug =
      modifiers->modifier_info.cug.suppress_pref_cug;
    call_info_ptr->dial_string_modifiers.modifier_info.cug.suppress_outgoing_access =
      modifiers->modifier_info.cug.suppress_outgoing_access;

   
    /* If we successfully initiate a call, return the hangup callback
       otherwise, put out NO CARRIER and return NULL pointer */
    if( DS3G_SUCCESS ==
        ds3g_msh_initiate_call (DS_CALL_TYPE_CKT,
                                ds_ucsd_network_mode,
                                dial_string,
                                digit_mode,
                                call_info_ptr ))
    {
      rc = ds_ucsd_call_hangup_cb;
    }
    else
    {
      dsat_send_result_code_ex (DSAT_NO_CARRIER,ds3g_siolib_ex_get_csd_port());
      
      /* Perform common call end cleanup. */
      if( TRUE != ds_ucsd_call_end_common_cleanup( inst_id ) )
      {
        DATA_MSG0_ERROR("Dial string handler: problem in common cleanup");
      }
    }
  }
  else
  {
    dsat_send_result_code_ex (DSAT_NO_CARRIER,ds3g_siolib_ex_get_csd_port());
  }
  return rc;
} /* ds_ucsd_dial_string_handler() */


#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST

void ds3gi_cm_call_event_cb
(
  cm_call_event_e_type          call_event, /* The call event that occurred*/
  const cm_mm_call_info_s_type *call_info_ptr          /* Call information */
);



/*===========================================================================
FUNCTION ds_ucsd_trigger_incall_modification

DESCRIPTION
  This function allows an external subsystem to trigger an in-call
  service change for the current CSData call.
  
  !!!THIS IS ONLY FOR TESTING!!! 
         
DEPENDENCIES
   None
  
RETURN VALUE
   None

SIDE EFFECTS
   None

===========================================================================*/
void
ds_ucsd_trigger_incall_modification ( uint8 dir, uint8 test_id )
{
  ds_ucsd_call_tbl_entry_type *call_info_ptr = DSUCSD_GET_CALL_INFO(0);
  const cm_bearer_capability_s_type *data_bc_ptr = NULL;
  ds_ucsd_call_type curr_call_type;

  if( 6 == test_id )
  {
    if( 0 == dir ) /* MO */
    {
      /* Determine current call type (for dual mode calls) */
      curr_call_type =
        ds_ucsd_determine_call_type( &call_info_ptr->neg_cap.bearer_capability_1,
                                     &call_info_ptr->neg_cap.bearer_capability_2,
                                     &call_info_ptr->neg_cap.bc_repeat_indicator,
                                     &data_bc_ptr,
                                     NULL );

      if( DS_UCSD_VOICE_CALL == curr_call_type )
      {
        DATA_MSG0_HIGH("TESTCASE: Modify to VT mode");
        ds_ucsd_app_modify_call( call_info_ptr->inst_id,
                                 DS_UCSD_MSG_COMMAND,
                                 DS_UCSD_VIDEO_TELEPHONY_CALL,
                                 TRUE );
      }
      else
      {
        DATA_MSG0_HIGH("TESTCASE: Modify to Voice mode");
        ds_ucsd_app_modify_call( call_info_ptr->inst_id,
                                 DS_UCSD_MSG_COMMAND,
                                 DS_UCSD_VOICE_CALL,
                                 TRUE );
      }
    }
    else if( 1 == dir ) /* MT */
    {
      cm_mm_call_info_s_type call_info;

      DATA_MSG0_HIGH("TESTCASE: network initiates incall service change");

      /* Setup voice bearer */
      call_info.call_type = CM_CALL_TYPE_VOICE;
      call_info.sys_mode = SYS_SYS_MODE_WCDMA;
      call_info.mode_info.info_type = CM_CALL_MODE_INFO_GW_CS;
      call_info.mode_info.info.gw_cs_call.bearer_capability_1 =
        call_info_ptr->neg_cap.bearer_capability_2;
      call_info.mode_info.info.gw_cs_call.bearer_capability_2 =
        call_info_ptr->neg_cap.bearer_capability_1;
      call_info.mode_info.info.gw_cs_call.bc_repeat_indicator =
        call_info_ptr->neg_cap.bc_repeat_indicator;
      
    
      ds3gi_cm_call_event_cb( CM_CALL_EVENT_MODIFY_IND,
                              &call_info );
    }
  }
}
#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */

   

/*===========================================================================

FUNCTION ds_ucsd_hdlr_init

DESCRIPTION
   This function will be called on power up initialization to initialize
   the UCSD subtask.  A call to this function will be placed in  
   dsi_task_init().  In addition to local initializations, this function
   will register the UCSD mode specific handler functions with 3G DSMGR. 
   
         
DEPENDENCIES
   None
  
RETURN VALUE
   Signal mask of signals that the UCSD mode-specific subtask will expect/
   handle.
    
SIDE EFFECTS
   None

===========================================================================*/
rex_sigs_type
ds_ucsd_hdlr_init (void)
{
   unsigned  call_instance;

   /*----------------------------------------------------------------------*/

  DATA_MSG0_HIGH("Initialize UMTS CS Data handlers");
#ifdef FEATURE_SEGMENT_LOADING
   ds_3gpp_csd_tds_tbl   = NULL;
   ds_3gpp_csd_wcdma_tbl = NULL;
   /*-------------------------------------------------------------------------
     Initialize TDSCDMA and WCDMA tables.
   -------------------------------------------------------------------------*/
   create_instance(CLSID_TDSCDMA, &ds_3gpp_csd_tds_tbl);
   create_instance(CLSID_WCDMA, &ds_3gpp_csd_wcdma_tbl);

   if( (ds_3gpp_csd_tds_tbl == NULL) && (ds_3gpp_csd_wcdma_tbl == NULL) )
   {
     DATA_3GMGR_MSG0(MSG_LEGACY_ERROR, "Neither TDS nor WCDMA active");
   }
#endif /* FEATURE_SEGMENT_LOADING  */
   /* Initialize task critical section */
   memset(&ds_ucsd_instance_crit_sect, 0, sizeof(rex_crit_sect_type));
   rex_init_crit_sect( &ds_ucsd_instance_crit_sect );

   /* Initialize call instance table */
   for( call_instance = 0;
        call_instance < MAX_UCSD_CALL_TBL_ENTRIES ;
        call_instance++)
   {
     ds_ucsd_call_tbl_ptr[ call_instance ] = NULL;
   }
   memset( &ds_ucsd_last_call_info, 0x0, sizeof(ds_ucsd_last_call_info));
   
   /* Initialize the call ID cross-reference table */
   memset( ds_ucsd_cm_callid_to_inst_tbl,
           INVALID_INST_ID,
           sizeof(ds_ucsd_cm_callid_to_inst_tbl) );
   
   /* Zero out the handler table */
   memset( &ds_ucsd_hdlr_func_tbl, NULL, sizeof(ds3g_hdlr_func_tbl_type) );

   /* Initialize the handler function table */

   /* originate_call_handler */
   ds_ucsd_hdlr_func_tbl.originate_call_handler =
      ds_ucsd_originate_call_handler;

   /* call_id_handler */
   ds_ucsd_hdlr_func_tbl.call_id_handler = ds_ucsd_call_id_handler;

   /* call_conf_handler */    
   ds_ucsd_hdlr_func_tbl.call_conf_handler = ds_ucsd_call_conf_handler;

   /* call_progress_handler */
   ds_ucsd_hdlr_func_tbl.call_progress_handler = ds_ucsd_call_progress_handler;
   
   /* call_connected_handler */
   ds_ucsd_hdlr_func_tbl.call_connected_handler = 
      ds_ucsd_call_connected_handler;

   /* complete_ll_connect_handler */
   ds_ucsd_hdlr_func_tbl.complete_ll_connect_handler = 
      ds_ucsd_complete_ll_connect_handler;

   /* setup_handler */
   ds_ucsd_hdlr_func_tbl.setup_handler = ds_ucsd_setup_handler;

   /* incoming_call_handler */
   ds_ucsd_hdlr_func_tbl.incoming_call_handler = ds_ucsd_incoming_call_handler;

   /* user_answer_handler */
   ds_ucsd_hdlr_func_tbl.user_answer_handler = ds_ucsd_user_answer_handler;

   /* user_end_handler */
   ds_ucsd_hdlr_func_tbl.user_end_handler = ds_ucsd_user_end_handler;

   /* complete_ll_disconnect_handler */
   ds_ucsd_hdlr_func_tbl.complete_ll_disconnect_handler =
      ds_ucsd_complete_ll_disconnect_handler;

   /* call_ended_handler */
   ds_ucsd_hdlr_func_tbl.call_ended_handler = ds_ucsd_call_ended_handler;

   /* switch_call_handler */
   ds_ucsd_hdlr_func_tbl.switch_call_handler = ds_ucsd_switch_call_handler;

   /* force_call_down_handler */
   ds_ucsd_hdlr_func_tbl.force_call_down_handler =
       ds_ucsd_force_call_down_handler;

/* TODO: Remove?? */
#if 0
   /* get_call_connection_info_handler */
   ds_ucsd_hdlr_func_tbl.get_call_connection_info_handler =
      ds_ucsd_get_call_connection_info_handler;
#endif

   /* get_data_counters_handler */
   ds_ucsd_hdlr_func_tbl.get_data_counters_handler =
      ds_ucsd_get_data_counters_handler;

/* TODO: Remove?? */
#if 0
   /* get_last_session_data_counters_handler */
   ds_ucsd_hdlr_func_tbl.get_last_session_data_counters_handler =
      ds_ucsd_get_last_session_data_counters_handler;
#endif

   /* sys_change_handler */
   ds_ucsd_hdlr_func_tbl.sys_change_handler = ds_ucsd_sys_change_handler;

#ifdef FEATURE_MULTIMEDIA_FALLBACK
   #error code not present
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

#if defined (FEATURE_3GPP_CSFB) && defined (FEATURE_DATA_LTE)
   /* sys_change_handler */
   ds_ucsd_hdlr_func_tbl.cntxt_transfer_ind_hdlr =
      ds_ucsd_cntxt_transfer_ind_handler;
#endif /* FEATURE_3GPP_CSFB && FEATURE_DATA_LTE */

   /* Initialize the bearer capability module */
   ds_ucsd_bc_init();
   
   /* Initialize the multiprocessor shim module */
   ds_ucsd_mpshim_init();
     
   /* Initialize the client API module */
   ds_ucsd_app_init();
   
   /* Initialize the Uplink and Downlink watermarks */
   ds_ucsd_init_sio_watermarks();
   ds_ucsd_init_l2_watermarks();
                                  
#ifdef FEATURE_DATA_WCDMA_CS
   /* Register the WCDMA CS Data mode-specific handlers with 3G dsmgr */
   ds3g_register_mode_sp_handler( SYS_SYS_MODE_WCDMA,
                                  DS_CALL_TYPE_CKT,
                                  (ds3g_hdlr_func_tbl_type*)
                                    &ds_ucsd_hdlr_func_tbl);
#endif /* FEATURE_DATA_WCDMA_CS */

#ifdef FEATURE_DATA_GCSD
   /* Register the GSM CS Data mode-specific handlers with 3G dsmgr */
   ds3g_register_mode_sp_handler (SYS_SYS_MODE_GSM, 
                                  DS_CALL_TYPE_CKT,
                                  (ds3g_hdlr_func_tbl_type*) 
                                  &ds_ucsd_hdlr_func_tbl);
#endif /* FEATURE_DATA_GCSD */

#ifdef FEATURE_3GPP_CSFB
   /* Register the GSM CS Data mode-specific handlers with 3G dsmgr */
   ds3g_register_mode_sp_handler (SYS_SYS_MODE_LTE, 
                                  DS_CALL_TYPE_CKT,
                                  (ds3g_hdlr_func_tbl_type*) 
                                  &ds_ucsd_hdlr_func_tbl);
#endif /* FEATURE_3GPP_CSFB */

#ifdef FEATURE_TDSCDMA
   /* Register the GSM CS Data mode-specific handlers with 3G dsmgr */
   ds3g_register_mode_sp_handler (SYS_SYS_MODE_TDS, 
                                  DS_CALL_TYPE_CKT,
                                  (ds3g_hdlr_func_tbl_type*) 
                                  &ds_ucsd_hdlr_func_tbl);
#endif /* FEATURE_TDSCDMA */

   /* No message signals used by UCSD call control */
   return (rex_sigs_type) 0;
} /* ds_ucsd_hdlr_init() */

/*===========================================================================
FUNCTION ds_ucsd_is_atcop_call

DESCRIPTION
   This function is called by Ds3g to notify whether the call 
   is originated or answered by ATCoP.

DEPENDENCIES
   None
  
RETURN VALUE
   TRUE -- if it is an ATCoP call.
   FALSE -- if it is a non-ATCoP call.
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_atcop_call
(
  cm_call_id_type                 cm_call_id        /* call id of call    */
)
{
  ds_ucsd_call_tbl_entry_type *call_info_ptr = NULL;
  uint8 index;

  /* Find call instance in the call table */
  if( MAX_UCSD_CALL_TBL_ENTRIES <=
      (index = ds_ucsd_find_call_instance( DSUCSD_XLATE_CALLID( cm_call_id ))) )
  {
    DATA_MSG1_ERROR("Cannot find call instance for CM call ID: %d",cm_call_id );
    return FALSE;
  }
  call_info_ptr = DSUCSD_GET_CALL_INFO( index );
  /* if both call control and data path is SIO, then 
     call is an external call made using ATCoP 
  */
  
  if ( (DS_UCSD_IFACE_SIO == call_info_ptr->call_ctrl_iface) &&
       (DS_UCSD_IFACE_SIO == call_info_ptr->datapath_iface) )
  {
    DATA_MSG0_MED("Call is an ATCoP call");
    return TRUE;
  }
  else
  {
    DATA_MSG0_MED("call is initiated by API");
    return FALSE;
  }
}/* ds_ucsd_is_atcop_call */


/*===========================================================================
FUNCTION ds_ucsd_get_call_tbl

DESCRIPTION
   This function is used to access ds_ucsd_call_tbl_ptr.

DEPENDENCIES
   None
  
RETURN VALUE
   ds_ucsd_call_tbl_entry_type *
    
SIDE EFFECTS
   None

===========================================================================*/
ds_ucsd_call_tbl_entry_type * ds_ucsd_get_call_tbl(uint8 index)
{
  return ds_ucsd_call_tbl_ptr[index];
}

/*===========================================================================
FUNCTION ds_ucsd_get_call_inst_from_call_id

DESCRIPTION
   This function returns the UCSD call_isnt corresponding to the 
   cm call_id.

DEPENDENCIES
   None
  
RETURN VALUE
   int
    
SIDE EFFECTS
   None

===========================================================================*/
uint8 ds_ucsd_get_call_inst_from_call_id(cm_call_id_type cm_call_id)
{
  if (cm_call_id < CM_CALL_ID_MAX)
  {
    return ds_ucsd_cm_callid_to_inst_tbl[cm_call_id];
  }
  else
  {
    DATA_MSG1_ERROR("Invlaid CM Call_id passed: %d", cm_call_id);
    return INVALID_INST_ID;
  }
}

/*===========================================================================
FUNCTION ds_ucsd_set_call_inst

DESCRIPTION
   This function sets the UCSD call_isnt corresponding to the 
   cm call_id.

DEPENDENCIES
   None
  
RETURN VALUE
   boolean
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_set_call_inst
(
  cm_call_id_type cm_call_id,
  uint8           call_inst
)
{
  if (cm_call_id < CM_CALL_ID_MAX)
  {
    ds_ucsd_cm_callid_to_inst_tbl[cm_call_id] = call_inst;
    return TRUE;
  }
  else
  {
    DATA_MSG1_ERROR("Invlaid CM Call_id passed: %d", cm_call_id);
    return FALSE;
  }
}

/*===========================================================================
FUNCTION ds_ucsd_get_network_mode

DESCRIPTION
   This function returns the network mode.

DEPENDENCIES
   None
  
RETURN VALUE
   sys_sys_mode_e_type
    
SIDE EFFECTS
   None

===========================================================================*/
sys_sys_mode_e_type ds_ucsd_get_network_mode(void)
{
  return ds_ucsd_network_mode;
}

/*===========================================================================
FUNCTION ds_ucsd_set_network_mode

DESCRIPTION
   This function sets the UCSD network mode.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
void ds_ucsd_set_network_mode(sys_sys_mode_e_type network_mode)
{
  ds_ucsd_network_mode = network_mode;
}

/*===========================================================================
FUNCTION ds_ucsd_get_last_call_info

DESCRIPTION
   This function accesses the UCSD last_call_info struct.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
ds_ucsd_last_call_info_type * ds_ucsd_get_last_call_info(void)
{
  return &ds_ucsd_last_call_info;
}

/*===========================================================================
FUNCTION ds_ucsd_is_sync_test_app_enabled

DESCRIPTION
   This function returns the value of ds_ucsd_get_sync_test_app_enabled flag.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_sync_test_app_enabled(void)
{
  return ds_ucsd_sync_test_app_enabled;
}

/*===========================================================================
FUNCTION ds_ucsd_is_incoming_internal_call_enabled

DESCRIPTION
   This function returns the value of 
   ds_ucsd_is_incoming_internal_call_enabled flag.

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
boolean ds_ucsd_is_incoming_internal_call_enabled(void)
{
  return ds_ucsd_incoming_internal_call_enabled;
}

#ifdef FEATURE_DUAL_SIM
/*===========================================================================
FUNCTION ds_ucsd_get_ucsd_subs_id

DESCRIPTION
   This function returns the value of ds_ucsd_subs_id

DEPENDENCIES
   None
  
RETURN VALUE
   None
    
SIDE EFFECTS
   None

===========================================================================*/
sys_modem_as_id_e_type ds_ucsd_get_ucsd_subs_id(void)
{
  return ds_ucsd_subs_id;
}

#endif /* FEATURE_DUAL_SIM */

#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

