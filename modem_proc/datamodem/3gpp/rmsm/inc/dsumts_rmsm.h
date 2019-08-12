#ifndef DSUMTS_RMSM_H
#define DSUMTS_RMSM_H
/*===========================================================================

                         D S U M T S _ R M S M . H

DESCRIPTION
  The Data Services RM State Machine for pdp-ip/pdp-ppp header file. 
  This state machine manages the Um and Rm interfaces for 
  pkt data calls initiated from laptop or conneted terminals on UMTS
  to support multimode data architecture.
  
  This is the public interface for RM state machine for UMTS.
  
EXTERNALIZED FUNCTIONS

Copyright (c) 2002 - 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR FILE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/rmsm/inc/dsumts_rmsm.h#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/19/12    nd     MT PDP pkt call.
02/08/12    vs     Added support for 3GPP IPv6 over PPP
01/19/12    ss     Fixed issue in initializing entitlement cb function.
06/20/11    ttv    Merged silent redial support changes for UMTS tethered calls.
07/09/10    asn    Support for Arbitration
02/15/10    sa     Added dsumts_rmsm_is_dun_call().
12/12/09    sa     Added support for ATCoP - Mode handler interface.
03/04/09    sa     AU level CMI modifications.
02/14/08    vk     Graceful termination of PDP-PPP calls.
09/06/06    rsl    Added 2 new commands PHYS_LINK_FLOW_ENABLE and IFACE_FLOW_ENABLE.
04/22/03    vsk    Code review cleanup : removed UMTS_RMSM_UM_IFACE_PDP_PPP_UP_EV
04/18/03    vsk    Created module
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "comdef.h"
#include "sys.h"
#include "dstask_v.h"
#include "ps_iface.h"
#include "ps_svc.h"
#include "dsat_v.h"
#ifdef FEATURE_DATA_TE_MT_PDP
#include "smix.h"
#endif /*FEATURE_DATA_TE_MT_PDP*/

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined (FEATURE_DATA_LTE)

/*===========================================================================

                         REGIONAL DATA DECLARATIONS

===========================================================================*/
   
/*---------------------------------------------------------------------------
  Declaration of the events of the UMTS RM state machine.
---------------------------------------------------------------------------*/              
typedef enum
{
  UMTS_RMSM_MIN_EV = -1,
  UMTS_RMSM_RM_WANTS_PKT_CALL_EV = 0,           /* Rm wants PKT call        */
  UMTS_RMSM_RM_PPP_CONFIGURED_EV =1,          /* PPP is configured with 
                                                 IP addr / IID info           */
  UMTS_RMSM_RM_PPP_UP_EV = 2,                 /* Rm ppp is up                 */
  UMTS_RMSM_RM_PPP_DOWN_EV = 3,               /* Rm ppp is down               */
  UMTS_RMSM_UM_IFACE_UP_EV = 4,               /* Um i/f is up                 */
  UMTS_RMSM_UM_IFACE_DOWN_EV = 5,             /* Um i/f is down               */
  UMTS_RMSM_RM_BRING_UP_UM_IFACE_EV = 6,      /* Rm i/f recv IPCP/IPv6CP creq */
#ifdef FEATURE_DATA_TE_MT_PDP                      
  UMTS_RMSM_UM_WANTS_MT_PKT_CALL_EV = 7,      /* Um received MT call          */
  UMTS_RMSM_RM_REJECT_MT_PKT_CALL_EV = 8,     /* Cannot terminate MT call     */
#endif /*FEATURE_DATA_TE_MT_PDP*/
  UMTS_RMSM_UM_PPP_DOWN_EV = 9,               /* PPP down on UM IFACE         */
  UMTS_RMSM_UM_IFACE_DOWN_ARB_EV = 10,
  UMTS_RMSM_NO_REDIALS_REMAINING_EV = 11,     /* All redial attempts used     */
  UMTS_RMSM_ADDR_CFG_COMPLETE_EV  = 12,       /* Addr cfg completed           */
  UMTS_RMSM_ADDR_CFG_END_EV  = 13,            /* Addr cfg ended               */
  UMTS_RMSM_ADDR_CFG_FAILURE_EV  = 14,        /* Addr cfg failed              */
  UMTS_RMSM_ADDR_CFG_CHANGED_EV  = 15,        /* Addr cfg changed             */
#ifdef FEATURE_DATA_TE_MT_PDP
  UMTS_RMSM_UM_LOCKED_EV  = 16,
#endif /*FEATURE_DATA_TE_MT_PDP*/
  UMTS_RMSM_ENTITLEMENT_CHECK_FAILED_EV = 17, /* Entitlement check error      */
  UMTS_RMSM_MAX_EV
} dsumts_rmsm_event_type;
           
           
   
/*---------------------------------------------------------------------------
  SM Instance identifies the instance of sm_info_ptr in dsumts_rmsmi_info.
  Need to revisit during Dual-IP implementation.
---------------------------------------------------------------------------*/             
typedef enum
{
  DSUMTS_RMSM_INVALID_SM_INSTANCE = -1,
  DSUMTS_RMSM_ANY_SM_INSTANCE = -1,
  DSUMTS_RMSM_V4_SM_INSTANCE = 0,          
  DSUMTS_RMSM_V6_SM_INSTANCE,
  DSUMTS_RMSM_MAX_SM_INSTANCES
} dsumts_rmsm_sm_instance_type;

/*---------------------------------------------------------------------------
  Rm Dev instance identifies the instance of dsumts_rmsmi_info. 
---------------------------------------------------------------------------*/
typedef enum
{
  DSUMTS_RMSM_INVALID_RM_DEV_INSTANCE = -1,
  DSUMTS_RMSM_RM_DEV_INSTANCE_0 = 0,
  DSUMTS_RMSM_RM_DEV_INSTANCE_1 = 1,
  DSUMTS_RMSM_MAX_RM_DEV_INSTANCES
} dsumts_rmsm_rm_dev_instance_type;

/*---------------------------------------------------------------------------
  Declaration of the structure containing Rm Dev instance and Sm Instance
  information. 
---------------------------------------------------------------------------*/
typedef struct
{
  dsumts_rmsm_rm_dev_instance_type rm_dev_instance;
  dsumts_rmsm_sm_instance_type     sm_instance;
  void *data_ptr;
} dsumts_rmsm_instance_type; 

typedef struct
    {
      int rm_dev_instance;
      int sm_instance;
      ps_iface_event_enum_type   event;
      ps_iface_event_info_u_type event_info;    
    }dsumts_rmsm_um_event_info;

/*---------------------------------------------------------------------------
  Maximum possible number of state machine instances  
---------------------------------------------------------------------------*/
#define DSUMTS_RMSM_MAX_INSTANCES (DSUMTS_RMSM_MAX_SM_INSTANCES *    \
                                   DSUMTS_RMSM_MAX_RM_DEV_INSTANCES)

#ifdef FEATURE_DATA_TE_MT_PDP

/* This data-structure is used in API exposed by RMSM */
typedef struct
{
  uint8                             pdp_type;     /* pdp type              */
  byte                             *apn_name;
                                                  /* Stores the apn        */
  word                              apn_len;      /* Length of the APN str */
  pdp_address_T                     address;      /* offered PDP address in 
                                                     the incoming page     */
  uint8                             mt_index;     /* index to MT TMR table */
  dsumts_rmsm_rm_dev_instance_type rmsm_inst;    /* linked RMSM instance  */
}dsumts_rmsm_mt_info_type;
#endif /*FEATURE_DATA_TE_MT_PDP*/

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                        PUBLIC FUNCTION DECLARATIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
  FUNCTION DSUMTS_RMSM_INIT()

  DESCRIPTION
    This function initializes the UMTS RM state machine. Once initialized, the 
    state machine manages SIO iface for packet data calls on an appropriate 
    Um iface. As part of initialization it creates SIO iface, init 
    queues/wms, allocates event buffers and resets state variables.
   
    It also calls the pdp-ip and pdp-ppp specific sub state machine
    initializations.
    
  PARAMETERS
    None
    
  RETURN VALUE
    TRUE on success
    FALSE on failure

  DEPENDENCIES
    Should only be called once at powerup

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsumts_rmsm_init( void );


/*===========================================================================
  FUNCTION DSUMTS_RMSM_POST_EVENT()

  DESCRIPTION
    This function receives an event posted to DSUMTS_RMSM and enqueues the
    corresponding command to be processed in the DS task by the appropriate
    sub state machine (pdp-ip or pdp-ppp)

  PARAMETERS
    inst_ptr: RmSm instance to which the event is being posted.
    event:    Event that is being posted to the state machine

  RETURN VALUE
    None

  DEPENDENCIES
    dsumts_rmsm_init() should have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_post_event
(
  dsumts_rmsm_instance_type*   inst_ptr,
  dsumts_rmsm_event_type    event
);



/*===========================================================================
  FUNCTION DSUMTS_RMSM_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DSUMTS_RMSM in DS task and passes it 
    to the appropriate sub state machine to handle it (pdp-ip or pdp-ppp).

  PARAMETERS
    event: The event that is being posted to the state machine.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    dsumts_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_process_cmd
(
  ds_cmd_type *ds_cmd_ptr
);


/*===========================================================================
  FUNCTION DS_3GPP_RMSM_AT_PROCESS_CMD()

  DESCRIPTION
    This function receives a cmd posted to DSUMTS_RMSM AT interface in DS task 
    and passes it to the appropriate handling.

  RETURN VALUE
    ds_cmd_ptr: Cmd buffer passed with the cmd

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void ds_3gpp_rmsm_at_process_cmd
(
  ds_cmd_type *ds_cmd_ptr
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_DIAL_STR_CB()

  DESCRIPTION
    This function handles the dial stings for pkt data calls and initiates 
    the data call over the appropriate pkt iface.  This function is 
    registered with ATCOP ATD handler table.
    
    It queries the pdp profile registry to get the profile associated with
    the context number specified in the dial strig. Based on that it sets
    the "sub-state" to either "pdp-ip" or "pdp-ppp" and kick starts the 
    state machine

  PARAMETERS
    dial_string: The dial string to be processed
    digit_mode: Flag to tell whether dial string contains digits only

  RETURN VALUE
    Function pointer to abort handler

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
dsat_call_abort_cb_type dsumts_rmsm_dial_str_cb
(
  const byte *dial_string,                        /* dial string           */
  boolean    digit_mode,                          /* non-digit or digits   */
  dsat_dial_modifier_info_type  *modifiers, /* Dial sting modifiers  */
  sys_modem_as_id_e_type        subs_id
);

#ifdef FEATURE_DATA_TE_MT_PDP

/*===========================================================================
  FUNCTION DSUMTS_RMSM_MT_RING_TE()

  DESCRIPTION
    This routine calls the RMSM INT routine to handle RING IND to TE.

  PARAMETERS
    int mt_index: The index of call in MT TIMER TBL    

  RETURN VALUE
    Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
void dsumts_rmsm_mt_ring_te
(
  const dsumts_rmsm_mt_info_type *mt_info_ptr
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_MT_HANGUP_CB

  DESCRIPTION
    This routine serves as the callback to ATH from TE

  PARAMETERS
    none    

  RETURN VALUE
    none

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    Should be run in DS Task context
===========================================================================*/
void dsumts_rmsm_mt_hangup_cb 
(
  void
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_MT_ANS_CB()

  DESCRIPTION
    Called by DSAT in DS task context. In this function we need to find the profile 
    to be used to bringup the call (if specified by user +CGANS or ATA) then compare
    PDP address and type of MT call with profile and either perform MT REJECT or
    call dsumts_rmsmi_mt_handle_ans_on_rm() to bringup call

  PARAMETERS
    dsat_pdp_answer_data_type

  RETURN VALUE
    dsat_call_abort_cb_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    Should be run in DS Task context
===========================================================================*/
dsat_call_abort_cb_type dsumts_rmsm_mt_ans_cb
(
  dsat_pdp_answer_data_type * answer_cb_data
);


/*===========================================================================
  FUNCTION dsumts_rmsm_mt_auto_ans_cb()

  DESCRIPTION
    Called in case of auto-answer is set in ATCOP

  PARAMETERS
    dsat_pdp_answer_data_type

  RETURN VALUE
    dsat_call_abort_cb_type

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    Should be run in DS Task context
===========================================================================*/
void dsumts_rmsm_mt_auto_ans_cb
(
  void
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_LOCK_RM()

  DESCRIPTION
    Checks whether Rm is available to terminate a MT-PDP call. This function
    currenly assumes that only 1 Rm Device can be active.

  PARAMETERS
    void

  RETURN VALUE
    TRUE:   successful in locking Rm
    FALSE:  unsuccessful

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
boolean dsumts_rmsm_lock_rm
(
  void
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_UNLOCK_RM()

  DESCRIPTION
    Checks whether Rm is available to terminate a MT-PDP call. This function
    currenly assumes that only 1 RMSM can be active.

  PARAMETERS
    ds_umts_pdp_type_enum_type pdp_type

  RETURN VALUE
    Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
void dsumts_rmsm_unlock_rm
(
  void
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_SET_MT()

  DESCRIPTION
    Used to set the is_mt flag in RMSM state table

  PARAMETERS
    int mt_index: The index of call in MT TIMER TBL    

  RETURN VALUE
    Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
boolean dsumts_rmsm_set_mt
(
  uint8 mt_index
);


/*===========================================================================
  FUNCTION DSUMTS_RMSM_RESET_MT()

  DESCRIPTION
    Used to reset the is_mt flag in RMSM state table

  PARAMETERS
    int mt_index: The index of call in MT TIMER TBL    

  RETURN VALUE
    Boolean

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
    
  RESTRICTION
    This function currenly assumes that only 1 RMSM can be active.
    Should be run in DS Task context
===========================================================================*/
boolean dsumts_rmsm_reset_mt
(
  uint8 mt_index
);

#endif /*FEATURE_DATA_TE_MT_PDP*/

/*===========================================================================
  FUNCTION DSUMTS_RMSMI_IFACE_FLOW_ENABLE_CB()

  DESCRIPTION
    This function handles the IFACE_FLOW_ENABLE_CMD posted
  by RMSM to DS task.

  PARAMETERS
    DS cmd that has information relating to IFACE_FLOW_ENABLE_CB.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsmi_iface_flow_cmd_hdlr
( 
  const ds_cmd_type            *cmd_ptr
);

/*===========================================================================
FUNCTION DSUMTS_RMSM_IS_DUN_CALL

DESCRIPTION
   This function is called by Ds3g to notify whether the call 
   is originated or answered by ATCoP.

DEPENDENCIES
   None
  
RETURN VALUE
   TRUE -- if it is SUCCESS
   FALSE -- if it is FAILURE
    
SIDE EFFECTS
   None

===========================================================================*/
boolean dsumts_rmsm_is_dun_call
(
  cm_call_id_type                 cm_call_id        /* call id of call    */
);

/*===========================================================================
  FUNCTION DSUMTS_RMSM_DUN_CTRL_RESP()

  DESCRIPTION
    This function processes the result for entitlement check and silent
    redial.

  PARAMETERS
    arg_val_ptr - cb_data 
    ps_errno    - errno    

  RETURN VALUE
    int16

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int16 dsumts_rmsm_dun_ctrl_resp
(
  void                    *arg_val_ptr,
  int16                   *ps_errno
);

/*===========================================================================
  FUNCTION dsumts_rmsm_process_sig()

  DESCRIPTION
    This function dequeues the next FC command in RMSM FC Q.

  PARAMETERS

  RETURN VALUE

  DEPENDENCIES
    dsumts_rmsm_init() has to have been called first.

  SIDE EFFECTS
    None
===========================================================================*/
void dsumts_rmsm_process_sig ( void );

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /*defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)|| defined (FEATURE_DATA_LTE)*/
#endif /* FEATURE_DATA_3GPP */
#endif /* DSUMTS_RMSM_H */
