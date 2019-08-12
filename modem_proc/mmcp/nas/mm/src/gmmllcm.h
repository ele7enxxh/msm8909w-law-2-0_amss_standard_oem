#ifndef GMMLLCM_H
#define GMMLLCM_H
/*===========================================================================

              GMM<->LLC INTERFACE MODULE HEADER FILE

DESCRIPTION
  This file contains types, function prototypes and data declarations are
  required in order to GMM<->LLC interface module

Copyright (c) 2001,2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/gmmllcm.h_v   1.3   18 Jul 2002 11:43:04   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/gmmllcm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/21/11   gps     FREE FLOAT FEATURE changes 
03/22/06   vdr     Added new event GMM_TRIGGER_LLC_RESUMPTION.
09/21/05   vdr     Added definitions of the new GMM primitives GMM_TRIGGER_LLC_SUSPENSION
                   amd GMM_TRIGGER_LLC_RESUMPTION
08/14/05   hj      Added handling for monitoring CCCH paging channel when detached from 
                   GPRS Services.
08/08/05   vdr     Added GMM_TRIGGER_DETACH_STARTED
06/17/05   vdr     Added GMM_TRIGGER_TLLI_UNASSIGNMENT
02/20/04   vdr     Added processing required by READY timer negotiation procedure
10/13/03   jca     Added external definition for function glif_exit_mm_non_drx_mode().
07/09/03   vdr     Added prototype for glif_ra_changed_rau_completed_ra var
05/23/03   jca     Added missing include file.
04/30/03   vdr     Added external definition for glif_force_local_tlli
04/21/03   vdr     Added new function prototype (glif_grr_send_service_confirm)
03/07/03   vdr     Removed some function and vars prototypes related to RAT change
02/27/03   vdr     Added some prototypes in order to support UMTS to GSM RAT change
01/22/03   vdr     Added prototypes of the sms_radio_priority and cell_notification_ind
                   vars
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/15/02   vdr     Added #include "customer.h"
03/04/02   vdr     Initial version
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "mmcp_variation.h"
#include "customer.h"
#include "mm_v.h"

#ifdef FEATURE_GSM_GPRS

#include "comdef.h"
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*
** GMM event list
*/
                                                          /*  Binary masks     */
#define GMM_NO_EVENT                                  0   /*                 0 */
#define GMM_TRIGGER_GMM_ATTACHED                      1   /*                 1 */
#define GMM_TRIGGER_GMM_DETACHED                      2   /*                10 */
#define GMM_TRIGGER_PTMSI_ASSIGNED                    4   /*               100 */
#define GMM_TRIGGER_FORCE_TO_STANDBY                  8   /*              1000 */
#define GMM_TRIGGER_RAU_STARTED                      16   /*             10000 */
#define GMM_TRIGGER_RAU_FINISHED                     32   /*            100000 */
#define GMM_TRIGGER_NEW_READY_TMR_VALUE              64   /*           1000000 */
#define GMM_TRIGGER_NEW_KC_VALUE                    128   /*          10000000 */
#define GMM_TRIGGER_NEW_READY_TMR_VALUE_WO_UPDATE   256   /*         100000000 */
#define GMM_TRIGGER_NO_READY_TMR_VALUE              512   /*        1000000000 */
#define GMM_TRIGGER_NO_READY_TMR_VALUE_WO_UPDATE   1024   /*       10000000000 */
#define GMM_TRIGGER_GMM_ATTACH_STARTED             2048   /*      100000000000 */
#define GMM_TRIGGER_TLLI_UNASSIGNMENT              4096   /*     1000000000000 */
#define GMM_TRIGGER_DETACH_STARTED                 8192   /*    10000000000000 */
#define GMM_TRIGGER_LLC_SUSPENSION                16384   /*   100000000000000 */
#define GMM_TRIGGER_LLC_RESUMPTION                32768   /*  1000000000000000 */
#define GMM_TRIGGER_IMSI_DETACH_FINISHED          65536   /* 10000000000000000 */
#ifdef FEATURE_GPRS_PS_HANDOVER
#define GMM_TRIGGER_PSHO_STARTED                 131072   /*100000000000000000 */
#endif
#define GMM_TRIGGER_G_RAT_RESUMED               262144   /*1000000000000000000*/
#define GMM_TRIGGER_GMM_STABILIZED              524288  /*100000000000000000000*/
/* GMM event buffer type */
typedef dword gmm_trigger_llc_type ;

/* GRR Camp state */
typedef enum glif_grr_camp_state_tag
{
  GLIF_CAMP_ON_CCCH ,
  GLIF_CAMP_ON_PCCCH_REQUESTED ,
  GLIF_CAMP_ON_PCCCH_CONFIRMED
} glif_grr_camp_state_type ;

/* New  service states indicating the GRR camp state */
typedef enum glif_grr_ser_req_state_tag
{
  GLIF_NO_SERVICE ,
  GLIF_SERVICE_REQUESTED ,
  GLIF_SERVICE_CONFIRMED
} glif_grr_ser_req_state_type ;

/* LLC connection state */
typedef enum glif_llc_connection_state_tag
{
  LLC_RESUMED ,
  LLC_SUSPENSION_REQUESTED ,
  LLC_SUSPENSION_CONFIRMED
} glif_llc_connection_state_type ;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 

/*
** The purpose of the radio priority information element is to specify
** the priority level that the MS shall use at the lower layers for
** transmission of data related to a PDP context or for mobile originated
** SMS transmission.
*/
extern byte    sms_radio_priority_sim[MAX_NAS_STACKS] ;

/* The purpose of the Cell Notification Indicator is to indicate that
** the Cell Notification is supported by the network and shall be
** used by MS
*/
extern boolean cell_notification_ind_sim[MAX_NAS_STACKS] ;

extern boolean glif_force_local_tlli_sim[MAX_NAS_STACKS] ;

extern routing_area_id_T glif_ra_changed_rau_completed_ra_sim[MAX_NAS_STACKS] ;

/* TLLI type definition */
typedef byte glif_tlli_type[ PTMSI_SIZE ] ;

extern glif_tlli_type glif_tlli_sim[MAX_NAS_STACKS] ;

/* GRR Camp state variable */
extern glif_grr_camp_state_type glif_grr_camp_state_sim[MAX_NAS_STACKS] ;


#define  sms_radio_priority sms_radio_priority_sim[mm_as_id]
#define  cell_notification_ind cell_notification_ind_sim[mm_as_id]

#define  glif_force_local_tlli glif_force_local_tlli_sim[mm_as_id]

#define  glif_ra_changed_rau_completed_ra glif_ra_changed_rau_completed_ra_sim[mm_as_id]
//#define  glif_tlli_type glif_tlli_type_sim[mm_as_id]

#define  glif_tlli glif_tlli_sim[mm_as_id]
#define  glif_grr_camp_state glif_grr_camp_state_sim[mm_as_id]

#else

/*
** The purpose of the radio priority information element is to specify
** the priority level that the MS shall use at the lower layers for
** transmission of data related to a PDP context or for mobile originated
** SMS transmission.
*/
extern byte    sms_radio_priority ;

/* The purpose of the Cell Notification Indicator is to indicate that
** the Cell Notification is supported by the network and shall be
** used by MS
*/
extern boolean cell_notification_ind ;

extern boolean glif_force_local_tlli ;

extern routing_area_id_T glif_ra_changed_rau_completed_ra ;

/* TLLI type definition */
typedef byte glif_tlli_type[ PTMSI_SIZE ] ;

extern glif_tlli_type glif_tlli ;

/* GRR Camp state variable */
extern glif_grr_camp_state_type glif_grr_camp_state ;

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE */
/*===========================================================================

                        FUNCTION'S PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION      GMMLLC_GMM_TRIGGER_LLC_EVENT

DESCRIPTION
  This function is a GMM entry point of the GMM<->LLC interface module.
  It parses & extracts incoming GMM events from the event buffer
  and kicks GMM<->LLC Interface module state machine.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmmllc_gmm_trigger_llc_event
(
  /* Incoming GMM event buffer */
  gmm_trigger_llc_type gmm_events,

  /* value of GMM_TRIGGER_FORCE_TO_STANDBY request */
  boolean force_to_standby,

  /* Current READY timer value's been assigned by the network */
  dword ready_timer_value

) ;

extern void glif_execute_pended_gmm_triggers(void);
/*===========================================================================

FUNCTION      GMMLLC_HANDLE_LLC_EVENT

DESCRIPTION
  This is an GMM-LLC module's interface function for processing of the
  incoming LLC events

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmmllc_handle_llc_event
(
  /* Incoming LLC UNIT DATA INDICATION */
  mm_cmd_type     *primitive
) ;

/*===========================================================================

FUNCTION      GLIF_GRR_NOTIFY_MS_RA_CAP

DESCRIPTION
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void glif_grr_notify_ms_ra_cap( void );


/*===========================================================================

FUNCTION      GMMLLC_GMM_SEND_OTA_MESSAGE

DESCRIPTION
  This is GMM-LLC module's interface function which is used in order to send
  any upper layer messages out to the GSM network in GPRS mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmmllc_gmm_send_ota_message
(
  dword message_length,
  byte *message_body
#ifdef FEATURE_GPRS_PS_HANDOVER
  ,boolean purgeQueueFlag
#endif 
) ;

/*===========================================================================

FUNCTION      GMMLLC_HANDLE_GRR_EVENT

DESCRIPTION
  This is a GMM-LLC module's interface function for GRR generated events.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmmllc_handle_grr_event
(
  /* Incoming GRR primitive */
  mm_cmd_type     *primitive
) ;

extern void gmmllc_reset_service_state(void);

extern void gmmllc_reset_llc_conn_state(void);
/*===========================================================================

FUNCTION      GMMLLC_INIT

DESCRIPTION
  This is GMM-LLC module's interface function is to be called upon power up
  of the protocol stack in order to attach the GMM layer to the LLC

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmmllc_init
(
  void
) ;

/*===========================================================================

FUNCTION      GMMLLC_REINIT

DESCRIPTION
  This is GMM-LLC module's interface function is to be called in order to
  re-initialize the interface' state variable into particular state :
  o GMM_IDLE_STATE
  o GMM_STANDBY_STATE
  o GMM_READY_STATE

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes GMM/GPRS interface' state.

===========================================================================*/
extern void gmmllc_reinit
(
  gmm_llc_if_state_type required_state
) ;

/*===========================================================================

FUNCTION      GLIF_GRR_SEND_SERVICE_CONFIRM

DESCRIPTION
  Sends a RR_GMM_GPRS_SERVICE_CNF primitive to GRR in order to allow Tx of
  layer 3 messages

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void glif_grr_send_service_confirm( void ) ;


/*===========================================================================

FUNCTION      GLIF_EXIT_MM_NON_DRX_MODE

DESCRIPTION
  This function builds RR_GMM_STOP_MM_NON_DRX_IND request primitive and
  sends it to the GRR

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void glif_exit_mm_non_drx_mode( void );

/*===========================================================================

FUNCTION      GMMLLC_GMM_TLLI_ASSIGNMENT

DESCRIPTION
  This is GMM-LLC module's interface function is to be called in order to
  assign new TLLI to LLC/GRR entitides

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void gmmllc_gmm_tlli_assignment
(
  void
) ;

/*==========================================================================

FUNCTION      GMMLLC_GET_GLIF_STATE

DESCRIPTION
  This function facilitates the use of gmm_llc_state in files other than gmmllcm.c where this variable
  is defined as a static.

DEPENDENCIES
  None

RETURN VALUE
  gmm_llc_if_state_type

SIDE EFFECTS
  None
  
==========================================================================*/
extern gmm_llc_if_state_type gmmllc_get_glif_state
(
  void
) ;

/*==========================================================================

FUNCTION      GMMLLC_GET_GLIF_GRR_CAMP_STATE

DESCRIPTION
  This function facilitates the use of glif_grr_camp_state in files other
  than gmmllcm.c 

DEPENDENCIES
  None

RETURN VALUE
  glif_grr_camp_state_type

SIDE EFFECTS
  None
  
==========================================================================*/
extern glif_grr_camp_state_type gmmllc_get_glif_grr_camp_state
(
  void
) ;
/*==========================================================================
FUNCTION      GMMLLC_GET_GLIF_GRR_SERVICE_REQ_STATE
     
DESCRIPTION
  This function facilitates the use of glif_grr_service_request_state in files other than gmmllcm.c
     
DEPENDENCIES
  None
     
RETURN VALUE
  glif_grr_ser_req_state_type
     
SIDE EFFECTS
  None
==========================================================================*/
extern glif_grr_ser_req_state_type gmmllc_get_glif_grr_service_req_state
(
  void
);
/*==========================================================================
FUNCTION      GMMLLC_GET_GLIF_LLC_CONN_STATE
DESCRIPTION
  This function facilitates the use of glif_service_conn_state in files other than gmmllcm.c
DEPENDENCIES
  None
RETURN VALUE
  glif_llc_connection_state_type
SIDE EFFECTS
  None
=========================================================================*/
extern glif_llc_connection_state_type gmmllc_get_glif_llc_conn_state
(
  void
);
#ifdef FEATURE_NAS_FREE_FLOAT
/*==========================================================================

FUNCTION      GMMLLC_SEND_SERVICE_REQ_IF_NEEDED

DESCRIPTION
  This function sends service request, if rr plmn select cnf is received, and no service request 
  was sent

DEPENDENCIES
  None

RETURN VALUE
  gmmllc_send_service_req_if_needed

SIDE EFFECTS
  None
  
==========================================================================*/
extern void gmmllc_send_service_req_if_needed(
  void
) ;
#endif
/*===========================================================================

FUNCTION      GLIF_GRR_NOTIFY_MS_RA_CAP

DESCRIPTION
  Generates a coded GRR MS Radio Access Capability IE without any spare bits
  or padding, and provides this to GRR in a RR_GMM_START_MM_NON_DRX_IND
  request primitive for use in GPRS Packet Resource Request messages.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void glif_grr_notify_ms_ra_cap( void );

#endif /* #ifdef FEATURE_GSM_GPRS */

#endif /* #ifndef GMMLLCM_H */
