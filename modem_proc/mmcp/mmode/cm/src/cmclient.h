#ifndef CMCLIENT_H
#define CMCLIENT_H
/*===========================================================================

       C A L L   M A N A G E R   C L I E N T   H E A D E R   F I L E

DESCRIPTION
  This header file contains definitions necessary to internally interface
  with CMCLIENT.C.

  Note that definitions that are necessary to externally interface with
  CMCLIENT.C are provided in CM.H


Copyright (c) 1991 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmclient.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/19/13   jvo     Remove all MDM/MSM Fusion features
06/18/13   gm      FR 2617 - OMADM IMS and ADMIN PDN Disable Requirement
11/22/12   xs      Added PS supplemetary service support
01/06/12   gm      Data synchronization changes for freefloating platform
12/23/10   sv      Integrate DSDS feature
11/24/10   xs      Fusion featurization clean up
10/01/10   ak      Merged the Fusion code from Dev branch.
05/21/10   ak      Mainlining memory optimization changes.
10/22/09   rm      Adding Modem Statistics changes
03/01/09   sv      Lint cleanup.
07/01/08   rm      Mainlining of MBMS changes
05/27/08   sv      Add new power collapse bitmask, to buffer any events
                   during Power Collapse State
03/31/08   ks      Add new CM client critical section
04/03/07   sk      Added cmclient_free_client_buf() and cmclient_list_del()
                   needed for asynchronously freeing the client object.
03/07/07   jqi     Added support for FEATURE_SMS_OVER_IP
02/18/07   rp      Added support for WMS<->CM<->NAS interface.
07/17/06   ic      For consistency, moved setting of requesting_client_id from
                   cmclient_list_call_event_ntfy() to
                   cm_client_call_event_ntfy().
03/15/06   sk      Added cmclient_map_init()
12/10/05   ic      Lint cleanup.
                   Added cmclient_get_client_id_for_client_type()
01/07/05   ic      Removed all code under and referring to FEATURE_UASMS
12/05/04   ic      Added cmclient_get_client_ptr_by_type()
12/04/04   ws      Support for Remote WMS
08/12/04   ws      Remove cm20 support.
06/24/04   ws      Add client id support in event generation.
06/08/04   ank     Mainlined FEATURE_GPSONE_DBM.
03/02/04   ic      Added BCMCS support (FEATURE_BCMCS)
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/21/04   ka      Added function header for cmclient_call_info_list_ntfy ()
04/11/03   prk     Removed references to FEATURE_SUPS
01/21/03   ws      Updated copyright information for 2003
09/13/92   kar     Fixed compile error
09/10/02   ws      Added initial cm20 backward compatibility.
08/06/02   HQ      Added FEATURE_CDSMS.
05/07/02   pk      Fixed indentation.
05/05/02   vt      Fixed compile problem with new mm cm.h
02/13/02   ic      Removed const modifier from
                   cm_inband_cmd_s_type  *inband_cmd_ptr
                   function parameter (ripple effect of removing call_id from
                   cm_inband_cmd_s_type structure)
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the
                   changes between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32
                   (CM VU in N*2165) that we needed to merge with and changes
                   between those two VUs were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
08/21/01   ic      Main-lined FEATURE_NEWCM
02/28/01   HQ      Fixed compiler warnings.
01/29/01   HQ      Properly feature-ized UASMS, GPSONE_DBM and NEWCM.
01/23/01   ych     Merged JCDMA features.
10/26/00   PJ      Added prototype for cmclient_list_pd_event_ntfy, to fix
                   compiler warning.
09/15/00   PJ      Added support for PD commands/events. (FEATURE_GPSONE_DBM).
08/23/00   vt      Added new CM interface changes.
08/31/99   SH      Modified definitions to featurize UASMS
08/06/99   SH      Added SMS support
07/07/99   SH      Added prototype for cmclient_free_q_init
06/21/99   SH      Fixed incorrects argument names for
                   cmclient_list_ss_cmd_err_ntfy
05/31/99   SH      Added CM 2.0 features
01/07/99   RC      Changes per ARM compiler.
11/24/98   RC      Initial release.

===========================================================================*/


#include "mmcp_variation.h"
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */


/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
                               CLIENT
---------------------------------------------------------------------------*/
/*===========================================================================

FUNCTION cmclient_free_client_buf

DESCRIPTION
  Free a client object.

  This function must be called after cmclient_free_q_init is called.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_free_client_buf( cm_client_s_type *client_ptr );

/*===========================================================================

FUNCTION cm_client_ph_event_ntfy

DESCRIPTION
  Notify a client of a specified phone event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_client_ph_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_ph_event_e_type         ph_event,
        /* notify client of this phone event */

    cm_ph_info_s_type          *ph_info_ptr
        /* pointer to phone state information struct */
);

/*===========================================================================

FUNCTION cm_client_ss_event_ntfy

DESCRIPTION
  Notify a client of a specified serving system event if the client is
  registered for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_ss_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_ss_event_e_type         ss_event,
        /* notify client of this serving system event */

    cm_mm_ss_info_s_type       *ss_info_ptr
        /* pointer to serving system state information struct */
);

/*===========================================================================

FUNCTION cm_client_ss_event_ntfy_new

DESCRIPTION
  Notify a client of a specified serving system event if the client is
  registered for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_ss_event_ntfy_new(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_ss_event_e_type         ss_event,
        /* notify client of this serving system event */

    cm_mm_msim_ss_info_s_type       *ss_info_ptr
        /* pointer to serving system information struct */

);

#ifdef CM_GW_SUPPORTED


/*===========================================================================

FUNCTION cm_client_sups_event_ntfy

DESCRIPTION
  Notify a client of a specified sups event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_client_sups_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_sups_event_e_type       sups_event,
        /* notify client of this sups event */

    cm_sups_info_s_type        *sups_info_ptr
        /* pointer to sups event information struct */
);

#endif

/*===========================================================================

FUNCTION cm_client_stats_event_ntfy

DESCRIPTION
  Notify a client of a specified Statistics event iff the client 
  is registered for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_client_stats_event_ntfy ( 

  cm_stats_event_e_type        stats_event,
   /* pointer to a client struct */

  sys_modem_stats_info_s_type  *stats_info_ptr
  /* pointer to STATS information struct */

);

/*===========================================================================

FUNCTION cmclient_call_info_list_ntfy

DESCRIPTION
  Notify a client of call info list iff the client is registered
  for this.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_call_info_list_ntfy (

    const cm_client_s_type      *client_ptr,

    const cm_call_state_info_list_s_type    *call_info_list_ptr
        /* pointer to call state information struct */
);


/*===========================================================================

FUNCTION cmclient_list_call_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified call event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_call_event_ntfy(

    cm_call_event_e_type            call_event,
        /* notify clients of this call event */

    cm_mm_call_info_s_type    *call_info_ptr
        /* pointer to call state information struct */
);




/*===========================================================================

FUNCTION cmclient_list_call_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified call command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_call_cmd_err_ntfy(

    cm_call_cmd_err_e_type      call_cmd_err,
        /* call command error code */

    const cm_call_cmd_s_type    *call_cmd_ptr
        /* pointer to call command struct */

);




/*===========================================================================

FUNCTION cmclient_list_ph_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified phone event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_ph_event_ntfy(

    cm_ph_event_e_type         ph_event,
        /* notify clients of this phone event */

    cm_ph_info_s_type    *ph_info_ptr
        /* pointer to phone state information struct */
);

/*===========================================================================

FUNCTION cmclient_list_subs_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified phone event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_subs_event_ntfy(

    const cm_subs_info_s_type    *subs_info_ptr
        /* pointer to phone state information struct */
);


#if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))
/*===========================================================================

FUNCTION cmclient_list_bcmcs_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified BCMCS event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_bcmcs_event_ntfy(

    cm_bcmcs_event_e_type          bcmcs_event,
        /* notify clients of this BCMCS event */

    cm_bcmcs_info_s_type          *bcmcs_info_ptr
        /* pointer to BCMCS information struct */
);
#endif /* FEATURE_BCMCS || MBMS */

#ifdef CM_GW_SUPPORTED
/*===========================================================================

FUNCTION cmclient_list_mbms_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified NAS event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_mbms_event_ntfy(

    cm_mbms_event_e_type         mbms_event,
        /* notify clients of this NAS event */

    cm_mbms_info_s_type         *mbms_info_ptr
        /* pointer to NAS information struct */
);

#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif /* FEATURE_WMS_CM_NAS_INTERFACES */
#endif /* CM_GW_SUPPORTED */

/*===========================================================================

FUNCTION cmclient_list_ss_event_ntfy_new

DESCRIPTION
  Notify all the clients in the client list of a specified serving system event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmclient_list_ss_event_ntfy_new(

    cm_ss_event_e_type            ss_event,
        /* notify clients of this serving system event */

    cm_mm_msim_ss_info_s_type    *ss_info_ptr
        /* pointer to serving system state information struct */
);


/*===========================================================================

FUNCTION cmclient_list_ss_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified serving system event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_ss_event_ntfy(

    cm_ss_event_e_type            ss_event,
        /* notify clients of this serving system event */

    cm_mm_ss_info_s_type    *ss_info_ptr
        /* pointer to serving system state information struct */
);

/*===========================================================================

FUNCTION cmclient_list_power_collapse_buffered_events_ntfy

DESCRIPTION
  Notify all the clients in the client list of the buffered events during 
  power collapse.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_power_collapse_buffered_events_ntfy(void);

/*===========================================================================

FUNCTION cmclient_list_stats_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified stats command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_stats_cmd_err_ntfy(

    cm_stats_cmd_err_e_type      stats_cmd_err,
        /* phone command error code */

    const cm_stats_cmd_s_type    *stats_cmd_ptr
        /* pointer to ph command struct */

);

/*===========================================================================

FUNCTION cmclient_list_ph_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified phone command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_ph_cmd_err_ntfy(

    cm_ph_cmd_err_e_type      ph_cmd_err,
        /* phone command error code */

    const cm_ph_cmd_s_type    *ph_cmd_ptr
        /* pointer to ph command struct */

);

/*===========================================================================

FUNCTION cmss_list_ss_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified serving system command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ss_cmd_err_ntfy(

    cm_ss_cmd_err_e_type      ss_cmd_err,
        /* call command error code */

    const cm_ss_cmd_s_type    *ss_cmd_ptr
        /* pointer to ss command struct */

);

/*===========================================================================

FUNCTION cmclient_list_inband_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified inband command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_inband_cmd_err_ntfy(

    cm_inband_cmd_err_e_type      inband_cmd_err,
        /* phone command error code */

    const cm_inband_cmd_s_type    *inband_cmd_ptr
        /* pointer to inband command struct */

);

/*===========================================================================

FUNCTION cmclient_list_sms_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified SMS command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sms_cmd_err_ntfy(

    cm_sms_cmd_err_e_type      sms_cmd_err,
        /* phone command error code */

    const cm_sms_cmd_s_type    *sms_cmd_ptr
        /* pointer to sms command struct */

);

#ifdef FEATURE_GPSONE
/*===========================================================================

FUNCTION cmclient_list_pd_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified PD command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmclient_list_pd_cmd_err_ntfy(
    cm_pd_cmd_err_e_type      pd_cmd_err,
        /* phone command error code */

    const cm_pd_cmd_s_type    *pd_cmd_ptr
        /* pointer to pd command struct */
);

/*===========================================================================

FUNCTION cmclient_list_pd_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified pd event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_pd_event_ntfy(
    cm_pd_event_e_type         pd_event,
        /* notify clients of this pd event */

    cm_pd_info_s_type    *pd_info_ptr
        /* pointer to pd information struct */
);
#endif /* FEATURE_GPSONE */


/*===========================================================================

FUNCTION cmclient_list_dbm_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified DBM command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/

void cmclient_list_dbm_cmd_err_ntfy(
    cm_dbm_cmd_err_e_type      dbm_cmd_err,
        /* phone command error code */

    const cm_dbm_cmd_s_type    *dbm_cmd_ptr
        /* pointer to sms command struct */
);


/*===========================================================================

FUNCTION cmclient_list_dbm_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified dbm event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_dbm_event_ntfy(
    cm_dbm_event_e_type         dbm_event,
        /* notify clients of this dbm event */

    cm_dbm_info_s_type         *dbm_info_ptr
        /* pointer to dbm information struct */
);

#ifdef CM_GW_SUPPORTED


/*===========================================================================

FUNCTION cmclient_list_sups_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified sups command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmclient_list_sups_cmd_err_ntfy(

    cm_sups_cmd_err_e_type      sups_cmd_err,
        /* call command error code */

    const cm_sups_cmd_s_type    *sups_cmd_ptr
        /* pointer to call command struct */

);

#endif

/*===========================================================================

FUNCTION cmclient_list_ac_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified AC command
  error.

===========================================================================*/
void cmclient_list_ac_cmd_err_ntfy(

    cm_ac_cmd_err_e_type      cmd_err,
        /* AC command error code */

    const cm_ac_cmd_s_type    *cmd_ptr
        /* pointer to AC command struct */

);

/*===========================================================================

FUNCTION cm_client_call_event_ntfy

DESCRIPTION
  Notify a client of a specified call event iff the client is registered
  for this event.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cm_client_call_event_ntfy(

    cm_client_s_type           *client_ptr,
        /* pointer to a client struct */

    cm_call_event_e_type       call_event,
        /* notify client of this call event */

    cm_mm_call_info_s_type     *call_info_ptr
        /* pointer to call state information struct */
);



/*===========================================================================

FUNCTION cmclient_list_inband_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified inband event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_inband_event_ntfy(

    cm_inband_event_e_type         inband_event,
        /* notify clients of this inband event */

    cm_inband_info_s_type    *inband_info_ptr
        /* pointer to phone state information struct */
);


/*===========================================================================

FUNCTION cmclient_list_sms_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified sms event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sms_event_ntfy(

    cm_sms_event_e_type         sms_event,
        /* notify clients of this sms event */

    cm_sms_info_s_type    *sms_info_ptr
        /* pointer to sms information struct */
);



#if defined CM_GW_SUPPORTED || defined FEATURE_IP_CALL


/*===========================================================================

FUNCTION cmclient_list_sups_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified sups event.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_sups_event_ntfy(

    cm_sups_event_e_type         sups_event,
        /* notify clients of this sups event */

    cm_sups_info_s_type          *sups_info_ptr
        /* pointer to sups information struct */
);
#endif

/*===========================================================================

FUNCTION cmclient_list_ac_event_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified access control
  events
  
DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_list_ac_event_ntfy(

    cm_ac_event_e_type event,
        /* notify clients of this access control event */

    cm_ac_info_s_type *info_ptr
        /* pointer to access control state information struct */
);

/*===========================================================================

FUNCTION cmclient_map_init

DESCRIPTION
  Initialize the client map array (cmclient_map_arr).

  This function must be called before the cmclient_map_arr can be used.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_map_init( void );

/*===========================================================================

FUNCTION cmclient_get_client_ptr

DESCRIPTION
  Returns a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object

SIDE EFFECTS
  None

===========================================================================*/
extern cm_client_s_type* cmclient_get_client_ptr(

    cm_client_id_type client_id
    /* Client Id */
);

/*===========================================================================

FUNCTION cmclient_get_client_ptr_by_type

DESCRIPTION

  Given client type, return a pointer to client object

DEPENDENCIES
  None

RETURN VALUE
  A pointer to client object or NULL if not found

SIDE EFFECTS
  None

===========================================================================*/
extern cm_client_s_type* cmclient_get_client_ptr_by_type(

    cm_client_type_e_type client_type
    /* Client type */
);


/*===========================================================================

FUNCTION cmclient_get_client_id

DESCRIPTION
  Returns client id that corresponds to a client object

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
extern cm_client_id_type cmclient_get_client_id(

    const cm_client_s_type *client_ptr
    /* Client object pointer */
);

cm_client_cmd_s_type *cm_cmd_alloc_client_init( void );

/*===========================================================================

FUNCTION cmclient_client_cmd_proc

DESCRIPTION
  Process clients' phone commands.

DEPENDENCIES
  Client must have already been initialized with cm_client_init().

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_client_cmd_proc(

    cm_client_cmd_s_type  *client_cmd
        /* pointer to a CM command */
);

#ifdef CM_DEBUG
#error code not present
#endif /* CM_DEBUG */


#if ((defined FEATURE_BCMCS) || (defined FEATURE_MBMS))
/*===========================================================================

FUNCTION cmclient_list_bcmcs_cmd_err_ntfy

DESCRIPTION
  Notify all the clients in the client list of a specified BCMCS command
  error.

DEPENDENCIES
  Initialized with cmclient_map_init().

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void cmclient_list_bcmcs_cmd_err_ntfy(

    cm_bcmcs_cmd_err_e_type      bcmcs_cmd_err,
        /* BCMCS command error code */

    const cm_bcmcs_cmd_s_type    *bcmcs_cmd_ptr
        /* pointer to BCMCS command struct */

);
#endif /* FEATURE_BCMCS || FEATURE_MBMS */

#if (defined FEATURE_MBMS)
#error code not present
#endif /* FEATURE_MBMS */

/*===========================================================================

FUNCTION cmclient_get_client_id_for_client_type

DESCRIPTION
  Returns client id that corresponds to a client type

DEPENDENCIES
  None

RETURN VALUE
  client id

SIDE EFFECTS
  None

===========================================================================*/
extern cm_client_id_type cmclient_get_client_id_for_client_type
(

    cm_client_type_e_type client_type
      /* Client type */
);

/*===========================================================================

FUNCTION 
  cmclient_crit_sect_ptr

DESCRIPTION
  Return pointer to CM client critical section. 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern rex_crit_sect_type* cmclient_crit_sect_ptr(void);

/*===========================================================================

FUNCTION 
  cmclient_init_crit_sect

DESCRIPTION
  Calling rex_init_crit_sect 

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cmclient_init_crit_sect(void);


#endif /* CMCLIENT_H */

