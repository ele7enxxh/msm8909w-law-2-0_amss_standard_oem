/*!
  @file
  ds_eps_tlb.c

  @brief
  Test loopback mode handler

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/src/ds_eps_tlb.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/02/12   sa      Fixed issue with V4V6 data path.
11/17/11   sa      Added support for V6 Loopback Mode B.
05/10/11   sa      Added support for TLB OPEN followed by CLOSE IND for Mode B.
03/07/11   sa      Fixed multiple LTE_TLB_DS_CTL_READY_IND being sent. 
11/17/10   vk      Added flag in policy to support bypassing the preferred 
                   system check
11/18/10   sa      Added support for close TLB loopback indication message 
                   to LTE TLB.
08/10/10   sa      Added support of LTE_TLB_LB_OPEN_IND.
04/21/10   sa      Added support for Loopback Mode B.
04/03/10   vk      Added support for MSG2.0 based logging
08/31/09   ml      Initial revision
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA_LTE

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST 
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

#include "modem_mem.h"
#include "ds_eps_tlb.h"
#include "dsmsgr.h"
#include "msgr_ds.h"
#include "dsm_item.h"
#include "ds_3gpp_pdn_context.h"
#include "ds_3gpp_bearer_context.h"
#include "ds_eps_pdn_context.h"
#include "ds_dsd_apm_ext_i.h"
#include "lte_pdcp_ext_api.h"
#include "ds3gsubsmgr.h"

#ifdef FEATURE_DATA_EMBMS
#include "ds_3gpp_embms_hdlr.h"
#endif /* FEATURE_DATA_EMBMS */

/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef struct
{
  int                           event;             /* Event type  */
  ds_eps_tlb_cmd_type           event_info;        /* Event info  */
  ds_eps_tlb_per_subs_info_type *hndl;

}ds_3gpp_tlb_cmd_info_type;

/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

/* Cookie to Ensure that that the TLB Information Block is valid */
#define DS_EPS_TLB_PER_SUBS_INFO_COOKIE 0xbcdebcde

/*-----------------------------------------------------------------------------
  Macro for Low, High and Do not exceed count
  for Mode B Rm iface watermark 
-----------------------------------------------------------------------------*/
#define DS_MODE_B_UL_WM_LO 15200
#define DS_MODE_B_UL_WM_HI 45600
#define DS_MODE_B_UL_WM_DNE 68400

/*-----------------------------------------------------------------------------
  Macro to start ModeB timer with a given value. It also sets the timer_running
  flag
-----------------------------------------------------------------------------*/
#define DSEPSTLB_SET_MODEB_TIMER(timer,time_value, hndl)\
  {\
    if(time_value >0){\
    (void)rex_set_timer(timer, time_value);\
    hndl->tlb_state.tmr_delay_modeB_running = TRUE;}\
  }

/*-----------------------------------------------------------------------------
  Macro to stop ModeB timer. It also resets the timer_running flag
-----------------------------------------------------------------------------*/
#define DSEPSTLB_CLR_MODEB_TIMER(timer, hndl)\
  {\
    if(hndl->tlb_state.tmr_delay_modeB_running == TRUE)\
    {\
      (void)rex_clr_timer(timer);\
      hndl->tlb_state.tmr_delay_modeB_running = FALSE;\
    }\
  }

#define IS_DSEPSTLB_VALID_INSTANCE(instance)\
    (DS_EPS_MAX_IFACE_PER_TLB > instance)

#define DSEPSTLB_GET_INST_FROM_UM_IFACE(iface_ptr,instance, hndl) \
  { \
   if( (TRUE == ps_iface_addr_family_is_v6(iface_ptr)) && \
       (iface_ptr ==  hndl->rmsm_state.um_iface_ptr[DS_EPS_TLB_IPV6_IFACE]))\
   {\
     instance = DS_EPS_TLB_IPV6_IFACE;\
   }else if (iface_ptr == hndl->rmsm_state.um_iface_ptr[DS_EPS_TLB_IPV4_IFACE])\
   {\
     instance = DS_EPS_TLB_IPV4_IFACE;\
   }\
   else \
   {\
     instance = DS_EPS_MAX_IFACE_PER_TLB;\
   }\
  }

#define DSEPSTLB_GET_INST_FROM_RM_IFACE(rm_iface_ptr,instance, hndl) \
  { \
   if(rm_iface_ptr == &(hndl->rmsm_state.rm_lo_b_iface[DS_EPS_TLB_IPV6_IFACE]))\
   {\
     instance = DS_EPS_TLB_IPV6_IFACE;\
   }else if(rm_iface_ptr == &(hndl->rmsm_state.rm_lo_b_iface[DS_EPS_TLB_IPV4_IFACE]))\
   {\
     instance = DS_EPS_TLB_IPV4_IFACE;\
   }\
   else \
   {\
     instance = DS_EPS_MAX_IFACE_PER_TLB;\
   }\
  }

#define DSEPSTLB_GET_INST_FROM_RM_PHYS_LINK_PTR(rm_phys_link_ptr,instance, hndl) \
  { \
   if(rm_phys_link_ptr == \
            &(hndl->rmsm_state.rm_lo_phys_link[DS_EPS_TLB_IPV6_IFACE]))\
   {\
     instance = DS_EPS_TLB_IPV6_IFACE;\
   }else if(rm_phys_link_ptr == \
            &(hndl->rmsm_state.rm_lo_phys_link[DS_EPS_TLB_IPV4_IFACE]))\
   {\
     instance = DS_EPS_TLB_IPV4_IFACE;\
   }\
   else \
   {\
     instance = DS_EPS_MAX_IFACE_PER_TLB;\
   }\
  }

/*------------------------------------------------------------------------------
  Defines the maximum number of EMBMS Control Block Instances that can be 
  allocated.
------------------------------------------------------------------------------*/

#define DS_EPS_TLB_MAX_INSTANCES 1

/*------------------------------------------------------------------------------
  Defines the value of an invalid EMBMS control block instance
------------------------------------------------------------------------------*/
#define DS_EPS_TLB_INVALID_INSTANCE DS_EPS_TLB_MAX_INSTANCES

/*----------------------------------------------------------------------------- 
  Array of pointers to instances of Test Loopback Mode Info Block.
------------------------------------------------------------------------------*/
static ds_eps_tlb_per_subs_info_type 
         *ds_eps_tlb_info_p[DS_EPS_TLB_MAX_INSTANCES] = {NULL};

/*------------------------------------------------------------------------------
  This is a map between the Subs Id and TLB Information Block Instance
------------------------------------------------------------------------------*/

typedef struct
{
  boolean init;
  int     index;
} ds_eps_tlb_subs_info_type;

static ds_eps_tlb_subs_info_type
         ds_eps_tlb_subs_id_map[DS3GSUBSMGR_SUBS_ID_MAX] = {{0}};

/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*==============================================================================
FUNCTION      DS_EPS_TLB_FIND_OR_ALLOCATE_CTRL_BLK

DESCRIPTION
  This function is used to find the Control Block corresponding to passed
  subs_id. If no control block is found, allocate a new Control Block and
  return a pointer to that.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES  
  None

RETURN VALUE  
  Pointer to the TLB Info Block.

SIDE EFFECTS
  None
==============================================================================*/
static ds_eps_tlb_per_subs_info_type* ds_eps_tlb_find_or_allocate_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================
FUNCTION DS_3GPP_GET_TLB_INFO_BLK_INDEX

DESCRIPTION
  This functions fetches the TLB Info Blk index corresponding to the
  given Subs Id.
 
PARAMETERS 
  Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Index to the TLB Info Blk

SIDE EFFECTS
  None.
==============================================================================*/
static int ds_eps_tlb_get_tlb_info_blk_index
(
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================
FUNCTION DS_3GPP_SET_TLB_INFO_BLK_INDEX

DESCRIPTION
  This functions maps the Subs Id and TLB Info Blk Index
 
PARAMETERS 
  Index to the TLB Info Blk
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
static void ds_eps_tlb_set_tlb_info_blk_index
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================
FUNCTION DS_EPS_TLB_VALIDATE_TLB_INFO_BLK_INDEX

DESCRIPTION
  This functions checks whether the TLB Information Block Index is Valid
 
PARAMETERS 
  Index 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
static boolean ds_eps_tlb_validate_tlb_info_blk_index
(
  uint8 index
);

/*==========================================================================
FUNCTION ds_eps_tlb_set_per_subs_info_hndl

DESCRIPTION
  This function is used to set the TLB information pointer
  corresponding to the given Subs Id
 
PARAMETERS 
  hndl    - Pointer to the TLB Information  
  subs_id - Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  TLB Information Block Instance
  
SIDE EFFECTS
  None.
===========================================================================*/
static int ds_eps_tlb_set_per_subs_info_hndl
(
  ds_eps_tlb_per_subs_info_type*  hndl
);

/*==========================================================================
FUNCTION ds_eps_tlb_get_hndl_from_rm_iface

DESCRIPTION
  This function is used to fetch the TLB Info handle from the Rm Iface
 
PARAMETERS 
  rm_iface_ptr - Pointer to the Rm Iface 
 
DEPENDENCIES
  None.

RETURN VALUE
  Handle to the TLB Information
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_eps_tlb_per_subs_info_type* ds_eps_tlb_get_hndl_from_rm_iface
(
  ps_iface_type* rm_iface_ptr
);

/*===========================================================================
FUNCTION DSEPSTLB_HDLR_REG_UM_EVENTS

DESCRIPTION
  This function registers callbacks for UM events
  
DEPENDENCIES 
  None.

RETURN VALUE 
  TRUE -- SUCCESS
  FALSE -- FAILURE
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean dsEpsTlb_hdlr_reg_um_events 
(
  ds_eps_rmsm_tlb_b_state_type *rmsm_state_ptr
)
{
  int ps_iface_result = 0;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_HIGH("dsEpsTlb_hdlr_reg_um_events()");

  ps_iface_result=ps_iface_event_cback_reg
                               (
                                 NULL,
                                 IFACE_DOWN_EV,
                                 rmsm_state_ptr->um_iface_down_buf_ptr
                               );

  if( ps_iface_result < 0 )
  {
    return FALSE;
  }

  ps_iface_result = ps_iface_event_cback_reg
                           (
                             NULL,
                             IFACE_UP_EV,
                             rmsm_state_ptr->um_iface_up_buf_ptr
                           );

  if( ps_iface_result < 0)
  {
    DS_LTE_MSG0_ERROR("IFACE_UP_EV Registration Failed");
    ps_iface_event_cback_dereg(
                            NULL,
                            IFACE_DOWN_EV,
                            rmsm_state_ptr->um_iface_up_buf_ptr
                          );
    return FALSE;
  }
  ps_iface_result = ps_iface_event_cback_reg(
                      NULL,
                      IFACE_BEARER_TECH_CHANGED_EV,
                      rmsm_state_ptr->um_iface_bearer_tech_change_buf_ptr);
  if( ps_iface_result < 0)
  {
    DS_LTE_MSG0_ERROR("Registration for bearer tech change event failed!");
    /* De-register for UP and down events */
    ps_iface_event_cback_dereg(
                NULL,
                IFACE_UP_EV,
                rmsm_state_ptr->um_iface_up_buf_ptr
              );
    ps_iface_event_cback_dereg(
                NULL,
                IFACE_DOWN_EV,
                rmsm_state_ptr->um_iface_down_buf_ptr
              );
    return FALSE;
  }

  return TRUE;

}/* dsEpsTlb_hdlr_reg_um_events */

/*===========================================================================
FUNCTION DSEPSTLB_HDLR_DEREG_UM_EVENTS

DESCRIPTION
  This function registers callbacks for UM events
  
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsEpsTlb_hdlr_dereg_um_events 
(
  ds_eps_rmsm_tlb_b_state_type *rmsm_state_ptr
)
{
  DS_LTE_MSG0_HIGH("dsEpsTlb_hdlr_dereg_um_events()");
  
  /* De-register for UP and down events 
     if both UM ifaces are down.*/
  ps_iface_event_cback_dereg(
              NULL,
              IFACE_UP_EV,
              rmsm_state_ptr->um_iface_up_buf_ptr
            );
  ps_iface_event_cback_dereg(
              NULL,
              IFACE_DOWN_EV,
              rmsm_state_ptr->um_iface_down_buf_ptr
            );
  ps_iface_event_cback_dereg(
        NULL,
        IFACE_BEARER_TECH_CHANGED_EV,
        rmsm_state_ptr->um_iface_bearer_tech_change_buf_ptr
      );
}/* dsEpsTlb_hdlr_dereg_um_events */

/*===========================================================================
FUNCTION DS_LOOPBACK_HDLR_TX_DATA_CB

DESCRIPTION
  This function modifies and loops ping and iperf pkts received back to
  RX WM
  This function is registered with ps_iface for a PDP-IP call 
  
PARAMETERS  
  *iface_ptr       - Interface ptr for the call.
  **item_ptr       - Payload
  *meta_info_ptr   - Not used.
  *pkt_instance    - Call instance.
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0 - Success
  
SIDE EFFECTS 
  None.

===========================================================================*/
/*ARGSUSED*/
int dsEpsTlb_hdlr_mode_b_tx_data_cb
(
  ps_iface_type     *iface_ptr,
  dsm_item_type     **item_ptr,
  ps_meta_info_type *meta_info_ptr,
  void              *pkt_instance
)
{
  ds_eps_rmsm_tlb_b_state_type    *rmsm_tlb_state_ptr;
  uint8                            instance = DS_EPS_MAX_IFACE_PER_TLB;
  ds_eps_tlb_per_subs_info_type   *hndl = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hndl = (ds_eps_tlb_per_subs_info_type*)iface_ptr->iface_private.tx_cmd_info;

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return 0;
  }

  rmsm_tlb_state_ptr = &(hndl->rmsm_state);

  ASSERT(rmsm_tlb_state_ptr != NULL);

  /* if MODE B loop is not closed , free the packet 
     and not to enqueue to rx water mark */
  if(hndl->tlb_state.ds_eps_bearer_tlb_state != DS_EPS_BEARER_TLB_B_CLOSED)
  {
    dsm_free_packet(item_ptr);
    DS_LTE_MSG0_HIGH("Mode TLB mode,discarding data, MODE B not closed");
    return 0;
  }
  /*-------------------------------------------------------------------------
    Enqueue the packet on RX watermark and set a signal to PS to pick it up
    or send it to a registered handler
  -------------------------------------------------------------------------*/
  DSEPSTLB_GET_INST_FROM_RM_IFACE(iface_ptr,instance,hndl);

  /* Check if this is a valid instance */
  if(!(IS_DSEPSTLB_VALID_INSTANCE(instance)))
  {
    DS_LTE_MSG0_HIGH("Not a loopback RM iface ,return without processing");
    return 0;
  }
  dsm_enqueue(&(rmsm_tlb_state_ptr->rm_lo_rx_wmk[instance]),item_ptr);

  PS_META_INFO_FREE(&meta_info_ptr); 

  return 0;
}/* dsEpsTlb_hdlr_mode_b_tx_data_cb */

/*===========================================================================
FUNCTION DSEPSTLB_RM_RX_DATA_CB

DESCRIPTION
  This function handles rx data on the RM iface which were established in 
  TLB mode. 
  In TLB is closed in mode B, data will be looped back. Otherwise, data will
  be dropped
  
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
/*ARGSUSED*/
LOCAL boolean dsEpsTlb_rm_rx_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
)
{
  dsm_item_type                 *item_ptr;
  ps_iface_type                 *rm_iface_ptr;
  ds_eps_rmsm_tlb_b_state_type  *rmsm_tlb_state_ptr ;
  uint8                          instance = DS_EPS_MAX_IFACE_PER_TLB;
  ds_eps_tlb_per_subs_info_type *hndl = NULL; 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rm_iface_ptr = (ps_iface_type *) user_data_p;
  ASSERT(rm_iface_ptr != NULL);

  hndl = ds_eps_tlb_get_hndl_from_rm_iface(rm_iface_ptr);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return FALSE;
  }

  rmsm_tlb_state_ptr = &(hndl->rmsm_state);

  DSEPSTLB_GET_INST_FROM_RM_IFACE(rm_iface_ptr,instance,hndl);
  /* Check if this is a valid instance */
  if(!(IS_DSEPSTLB_VALID_INSTANCE(instance)))
  {
    DS_LTE_MSG0_HIGH("Not a loopback RM iface ,return without processing");
    return TRUE;
  }
  /* if MODE B loop is not closed , return TRUE */
  if(hndl->tlb_state.ds_eps_bearer_tlb_state != DS_EPS_BEARER_TLB_B_CLOSED)
  {
    item_ptr = (dsm_item_type *)dsm_dequeue(
                     &(rmsm_tlb_state_ptr->rm_lo_rx_wmk[instance]));
    if (item_ptr != NULL)
    {
      /*  We need to free DSM item., and no action taken */
      dsm_free_packet(&item_ptr);
      DS_LTE_MSG0_HIGH("Mode TLB mode,discarding data, MODE B not closed");
    }
    return TRUE;
  }
  /* 3GPP TS 36.509,as per section Section 5.4.4.2;5.4.4.4;5.4.4.6
     In case of 3GPP mode,
     1. If delay timer is running, buffer the data
     2. if timer is not running and if buffer_ip_pdu == TRUE,
          buffer the data and start delay timer.
     3. if above conditions are false, submit the data to Uplink
  */
  if (rmsm_tlb_state_ptr->current_network == PS_IFACE_NETWORK_3GPP)
  {
    if ( hndl->tlb_state.tmr_delay_modeB_running == TRUE)
    {
      /* Buffer the data in rm water mark, don't raise signal */
      DS_LTE_MSG0_MED("Mode TLB mode, buffering data in Rm WM");
      return TRUE;
    }
    else
    { 
      /* check if buffer IP_PDU is set and timer val is non zero */
      if( (hndl->tlb_state.buffer_ip_pdu == TRUE ) &&
          (hndl->tlb_state.ip_pdu_delay_time != 0) && 
           !dsm_is_wm_empty(&(rmsm_tlb_state_ptr->rm_lo_rx_wmk[instance] )))
      {
        DS_LTE_MSG1_HIGH("Start mode B timer for %d ",
                         hndl->tlb_state.ip_pdu_delay_time);
        DSEPSTLB_SET_MODEB_TIMER(&(hndl->tlb_state.tmr_delay_modeB),
                                 hndl->tlb_state.ip_pdu_delay_time,
                                 hndl)
        return TRUE;
      }
    }
  }
  else
  {

    /* 3GPP TS 36.509,as per section Section 5.4.4.8
     In case of C2K mode,
     1. if CDMA_2K_INITED is FALSE, and if delay timer is running, 
         restart the timer else start the timer and set 
         CDMA_2K_INITED set to TRUE
     2. if not above, and timer is running deliver the packets to
        upper layers
     3. if above conditions are false, submit the data to Uplink
    */
    /* Mode is CDMA */
    if(hndl->tlb_state.cdma_2000_inited == FALSE)
    {
      hndl->tlb_state.cdma_2000_inited = TRUE;

      DSEPSTLB_CLR_MODEB_TIMER(&(hndl->tlb_state.tmr_delay_modeB), hndl);
      DS_LTE_MSG1_HIGH("Start mode B timer for %d ",
                       hndl->tlb_state.ip_pdu_delay_time);
      DSEPSTLB_SET_MODEB_TIMER(&(hndl->tlb_state.tmr_delay_modeB),
                               hndl->tlb_state.ip_pdu_delay_time,
                               hndl);
    }
#if 0
     /* In case of C2K mode, then UE shall not take any action
        3GPP TS 36.509,as per section Section 5.4.4.9. 
        Clarification required, does this mean
           to discard the ip packets recieved 
     */
    else
    {
      if ( ds_eps_tlb_state.tmr_delay_modeB_running == FALSE)
      {
        item_ptr = (dsm_item_type *)dsm_dequeue(
            &(rmsm_tlb_state_ptr->rm_lo_rx_wmk));
        if (item_ptr != NULL)
        {
          /*  We need to free DSM item., and no action taken */
          dsm_free_packet(&item_ptr);
          DS_LTE_MSG0_MED("Mode TLB mode,discarding uplink data");
        }
        return TRUE;
      }
    }
#endif
  }

  if(rmsm_tlb_state_ptr->rm_flow_enabled[instance] == TRUE)
  {
    item_ptr = (dsm_item_type *)dsm_dequeue(
            &(rmsm_tlb_state_ptr->rm_lo_rx_wmk[instance]));
    if (item_ptr != NULL)
    {
      DS_LTE_MSG1_HIGH("ip rm rx data cb is called for instance[%d]", instance);

      (void)ps_phys_link_input(&
                    (rmsm_tlb_state_ptr->rm_lo_phys_link[instance]), 
                     &item_ptr,
                     NULL);
      return FALSE;
    }
  }
  return TRUE;
}/* dsEpsTlb_rm_rx_data_cb */

/*===========================================================================
  FUNCTION DSEPSTLB_POST_CLOSE_READY_IND

  DESCRIPTION
    This function post close ready indication to lower layers.

  PARAMETERS
    Subscription Id

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsEpsTlb_post_close_ready_ind
(
  sys_modem_as_id_e_type subs_id
)
{
  lte_tlb_ds_close_tl_ready_ind_s    tlb_close_ready_ind;
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  eps_tlb_hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return;
  }

  eps_tlb_hndl->tlb_state.sent_close_loopback_ind= TRUE;

  /* Inform TLB that DS is ready for incoming packets */
  if( FALSE == dsmsgrsnd_per_subs_msg_send_ext
         ( LTE_TLB_DS_CTL_READY_IND,
           MSGR_DS_LTE, 
           (msgr_hdr_struct_type*)(&tlb_close_ready_ind),
           sizeof(tlb_close_ready_ind),
           SYS_AS_ID_TO_INST_ID(subs_id)
         ) )
  {
    DS_LTE_MSG0_LOW("Posting of CLOSE READY ind failed");
  }
  else
  {
    DS_LTE_MSG0_HIGH("Posted CLOSE READY ind ");
  }

}/* dsEpsTlb_post_close_ready_ind */

/*===========================================================================
FUNCTION DSEPSTLB_MODEB_TIMER_CB()

DESCRIPTION
  This function is registered as the timer call back. When the timer 
  for loopback mode B expires, this function is called. This posts
  a command to DS.  
      
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void dsEpsTlb_modeB_timer_cb
(
  void*              callback_data      /* unused */
)
{
  ds_cmd_type                   *cmd_ptr = NULL;
  ds_3gpp_tlb_cmd_info_type     *data_ptr = NULL;
  ds_eps_tlb_per_subs_info_type *hndl = 
                                   (ds_eps_tlb_per_subs_info_type*)callback_data;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_LTE_MSG0_HIGH("dsEpsTlb_modeB_timer_cb");

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  /*
   * This function is executed in the timer call back.
   * So all we do here is to put a command to DS and exit
   */
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_tlb_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return;
  }
  else
  {
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_TLB_MODE_B_HANDLER;
    data_ptr = (ds_3gpp_tlb_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->event = (int)DS_EPS_MODE_B_TMR_EXP;
    data_ptr->hndl = hndl;
    ds_put_cmd_ext(cmd_ptr);
  }
} /* dsEpsTlb_lmodeB_timer_cb */

/*===========================================================================
FUNCTION DSEPSTLB_RM_FLOW_LOW_WM_CB

DESCRIPTION
  This function puts a ds cmd for low water mark event
  
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int dsEpsTlb_rm_flow_low_wm_cb 
(
  dsm_watermark_type *wm,  
  void*              callback_data
)
{
  ds_eps_tlb_cmd_type                event_info;
  ds_cmd_type                       *cmd_ptr = NULL;    /* DS Task msg      */
  ps_iface_type                     *iface_ptr = NULL;  /* RM Iface pointer */
  ds_3gpp_tlb_cmd_info_type         *data_ptr = NULL;
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  iface_ptr = (ps_iface_type*)callback_data;
  ASSERT(iface_ptr != NULL);
 
  eps_tlb_hndl = ds_eps_tlb_get_hndl_from_rm_iface(iface_ptr);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return -1;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_tlb_cmd_info_type));
  if((cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return -1;
  }
  else
  {
    DS_LTE_MSG1_HIGH("Posting flow enable for rm iface 0x%x",iface_ptr);
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_TLB_MODE_B_HANDLER;
    data_ptr = (ds_3gpp_tlb_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->event = (int)DS_EPS_RM_FLOW_EVT;
    data_ptr->hndl = eps_tlb_hndl;
    event_info.rm_flow_evt_info.rm_flow = DS_EPS_RM_FLOW_ENABLE;
    event_info.rm_flow_evt_info.rm_iface_ptr = iface_ptr;
    data_ptr->event_info = event_info;
    ds_put_cmd(cmd_ptr);
  }
  return 0;
}/* dsEpsTlb_rm_flow_low_wm_cb */


/*===========================================================================
FUNCTION DSEPSTLB_RM_FLOW_HIGH_WM_CB

DESCRIPTION
  This function puts a ds cmd for high water mark event
  
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL int dsEpsTlb_rm_flow_high_wm_cb 
(
  dsm_watermark_type *wm,  
  void*              callback_data
)
{
  ds_eps_tlb_cmd_type                event_info;
  ds_cmd_type                       *cmd_ptr = NULL;    /* DS Task msg        */
  ps_iface_type                     *iface_ptr = NULL;  /* RM Iface pointer   */
  ds_3gpp_tlb_cmd_info_type         *data_ptr = NULL;
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  iface_ptr = (ps_iface_type*)callback_data;
  ASSERT(iface_ptr != NULL);

  eps_tlb_hndl = ds_eps_tlb_get_hndl_from_rm_iface(iface_ptr);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return -1;
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_tlb_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    return -1;
  }
  else
  {
    DS_LTE_MSG1_HIGH("Posting flow disable for rm iface 0x%x",iface_ptr);
    cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_TLB_MODE_B_HANDLER;
    data_ptr = (ds_3gpp_tlb_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
    data_ptr->event = (int)DS_EPS_RM_FLOW_EVT;
    data_ptr->hndl = eps_tlb_hndl;
    event_info.rm_flow_evt_info.rm_flow = DS_EPS_RM_FLOW_DISABLE;
    event_info.rm_flow_evt_info.rm_iface_ptr = iface_ptr;
    data_ptr->event_info = event_info;
    ds_put_cmd(cmd_ptr);
  }
  return 0;
}/* dsEpsTlb_rm_flow_high_wm_cb */


/*===========================================================================
FUNCTION DSEPSTLB_HDLR_REG_RM_EVENTS

DESCRIPTION
  This function registers callbacks for RM phys link flow enable and disable events
  
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsEpsTlb_hdlr_reg_rm_events 
(
  ps_phys_link_type* rm_phys_link_p,
  ds_eps_rmsm_tlb_b_state_type *rmsm_state_ptr,
  uint8  inst
)
{
  ASSERT(rmsm_state_ptr != NULL);
  ASSERT(rm_phys_link_p != NULL);

  DS_LTE_MSG1_HIGH("dsEpsTlb_hdlr_reg_rm_events(),inst(%d)",inst);

  /*-------------------------------------------------------------------------
    Register for PHYS link Flow enabled and Flow disabled events
  -------------------------------------------------------------------------*/
  if(0 != ps_phys_link_event_cback_reg(rm_phys_link_p,\
            PHYS_LINK_FLOW_ENABLED_EV,
            rmsm_state_ptr->rm_flow_enable_buf_ptr[inst]))
  {
    DS_LTE_MSG0_ERROR("Could not register for RM - PHYS_LINK_FLOW_ENABLED_EV");
    return;
  }

  if(0 != ps_phys_link_event_cback_reg(rm_phys_link_p,\
            PHYS_LINK_FLOW_DISABLED_EV,\
            rmsm_state_ptr->rm_flow_disable_buf_ptr[inst]))
  {
    DS_LTE_MSG0_ERROR("Could not register for RM - PHYS_LINK_FLOW_DISABLED_EV");
    ps_phys_link_event_cback_dereg(rm_phys_link_p,
                                 PHYS_LINK_FLOW_ENABLED_EV, 
                                 rmsm_state_ptr->rm_flow_enable_buf_ptr[inst]);
    return ;
  }
}/* dsEpsTlb_hdlr_reg_rm_events */

/*===========================================================================
FUNCTION DSEPSTLB_HDLR_DEREG_RM_EVENTS

DESCRIPTION
  This function de-registers callbacks for RM phys link flow enable and disable events
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsEpsTlb_hdlr_dereg_rm_events 
(
  ps_phys_link_type* rm_phys_link_p,
  ds_eps_rmsm_tlb_b_state_type *rmsm_state_ptr,
  uint8  inst
)
{  
  ASSERT(rmsm_state_ptr != NULL);

  DS_LTE_MSG1_HIGH("dsEpsTlb_hdlr_dereg_rm_events(),inst(%d)",inst);

  /*-------------------------------------------------------------------------
    Register for PHYS link Flow enabled and Flow disabled events
  -------------------------------------------------------------------------*/
  ps_phys_link_event_cback_dereg(rm_phys_link_p,
                                 PHYS_LINK_FLOW_ENABLED_EV, 
                                 rmsm_state_ptr->rm_flow_enable_buf_ptr[inst]);
  
  ps_phys_link_event_cback_dereg(rm_phys_link_p,
                                 PHYS_LINK_FLOW_DISABLED_EV,
                                 rmsm_state_ptr->rm_flow_disable_buf_ptr[inst]); 
  return;
}/* dsEpsTlb_hdlr_dereg_rm_events*/

/*===========================================================================
FUNCTION DSEPSTLB_RESET_RM_LOOPBACK_MODEB

DESCRIPTION
  This function process the loopback mode B related 
  at the reception of de-activate msg.

PARAMTERS
  eps_tlb_hndl - Handle to the TLB Information

DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsEpsTlb_reset_rm_loopback_modeB 
(
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl
)
{
  uint8                          i = 0;
  ds_eps_rmsm_tlb_b_state_type*  rmsm_state_ptr = NULL;
  ps_iface_down_mh_event_info_type   down_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/ 

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  DS_LTE_MSG0_HIGH("dsEpsTlb_reset_rm_loopback_modeB()");

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return;
  }

  rmsm_state_ptr = &(eps_tlb_hndl->rmsm_state);

  /* 1. Remove the bridged iface
     2. De-reg all Iface and Physlink events
     3. Empty the RX Queue
     4. Send Physlink Down indication
     5. Send RM Iface down indication
  */
    DSEPSTLB_CLR_MODEB_TIMER(&(eps_tlb_hndl->tlb_state.tmr_delay_modeB), 
                               eps_tlb_hndl);

    for( i = DS_EPS_TLB_IPV4_IFACE;
         (i<DS_EPS_MAX_IFACE_PER_TLB);
         i++)
    {
      if (NULL != rmsm_state_ptr->um_iface_ptr[i])
      {
        ps_iface_set_bridge( &(rmsm_state_ptr->rm_lo_b_iface[i]),
                             NULL );
        dsEpsTlb_hdlr_dereg_rm_events(
                                 &(rmsm_state_ptr->rm_lo_phys_link[i]),
                                   rmsm_state_ptr,
                                   i);
        dsm_empty_queue(&(rmsm_state_ptr->rm_lo_rx_wmk[i]));
        ps_iface_phys_link_down_ind(&(rmsm_state_ptr->rm_lo_b_iface[i]));

        down_info.down_reason = PS_NET_DOWN_REASON_REGULAR_DEACTIVATION;
        down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
        down_info.bearer_tech.rat_value =
          ds_3gpp_map_sys_mode_to_ds_sys_rat
            (ds3gpp_get_current_network_mode(eps_tlb_hndl->subs_id));
        down_info.bearer_tech.so_mask = ds_3gpp_bearer_cntxt_get_so_mask(eps_tlb_hndl->subs_id);

        ps_iface_down_ind( &(rmsm_state_ptr->rm_lo_b_iface[i]), &down_info);
        rmsm_state_ptr->um_iface_ptr[i]= NULL;
      }
    }/* for .. */

    /* De-register for UM Iface UP and down events */
    dsEpsTlb_hdlr_dereg_um_events(rmsm_state_ptr);

}/* dsEpsTlb_reset_rm_loopback_modeB */

/*===========================================================================
  FUNCTION DSEPSTLB_UM_IFACE_DOWN_EVT_HDLR()

  DESCRIPTION
    This function process IFACE_DOWN event

  PARAMTERS
    um_iface_ptr - Um iface Ptr
    eps_tlb_hndl - Handle to the TLB Information

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsEpsTlb_um_iface_down_evt_hdlr
(
  ps_iface_type                  *um_iface_ptr,
  ds_eps_tlb_per_subs_info_type  *hndl

)
{
  uint8                                  instance = DS_EPS_TLB_IPV4_IFACE;
  ps_iface_state_enum_type               iface_state = IFACE_STATE_INVALID;
  ds_eps_rmsm_tlb_b_state_type         * rmsm_state_ptr = NULL;
  ps_iface_type                        * other_iface_ptr = NULL;
  ps_iface_type                        * bridge_iface_ptr = NULL;
  ps_iface_down_mh_event_info_type       down_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  down_info.down_reason = PS_NET_DOWN_REASON_REGULAR_DEACTIVATION;

  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
  down_info.bearer_tech.rat_value =
    ds_3gpp_map_sys_mode_to_ds_sys_rat
      (ds3gpp_get_current_network_mode(hndl->subs_id));
  down_info.bearer_tech.so_mask = 
    ds_3gpp_bearer_cntxt_get_so_mask(hndl->subs_id);

  /* Find the instance form um iface pointer */
  DSEPSTLB_GET_INST_FROM_UM_IFACE(um_iface_ptr,instance, hndl);

  /* Check if this is a valid instance */
  if(!(IS_DSEPSTLB_VALID_INSTANCE(instance)))
  {
    DS_LTE_MSG0_HIGH("Not a loopback UM iface ,return without processing");
    return;
  }

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  DS_LTE_MSG1_HIGH("Mode TLB mode, Processing Um iface DOWN event for 0x%x",
                    um_iface_ptr);

  if(instance == DS_EPS_TLB_IPV4_IFACE)
  {
    other_iface_ptr = hndl->rmsm_state.um_iface_ptr[DS_EPS_TLB_IPV6_IFACE];
  }
  else 
  {
    other_iface_ptr = hndl->rmsm_state.um_iface_ptr[DS_EPS_TLB_IPV4_IFACE];
  }

  if(NULL != other_iface_ptr)
  {
    iface_state = ps_iface_state(other_iface_ptr);
  }

  rmsm_state_ptr = &(hndl->rmsm_state);

  /* 1. Remove the bridged iface
     2. De-reg all Iface and Physlink events
     3. Empty the RX Queue
     4. Send Physlink Down indication
     5. Send RM Iface down indication
  */
  /* As per 3GPP TS36.509, section 5.4.4.11:
     if (Loopback mode is active) &&
      (BUFFER_IP_PDU == TRUE || T_delay_modeB_tmr running == TRUE)
        then keep UE Test loop mode B active,
     otherwise UE behavour is unspecified. keeping both as 
     similar. ignoring loopback mode A case.
   */
  if ( hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_B_CLOSED )
  {

    /* Check if RM iface pointer is already bridged, 
       if so we have handled the UM iface up event as part 
       of IFACE bring up. No further action needed.
    */
    bridge_iface_ptr = ps_iface_bridge_iface(
                                &(rmsm_state_ptr->rm_lo_b_iface[instance]));
    if( NULL != bridge_iface_ptr)
    {
      ps_iface_set_bridge( &(rmsm_state_ptr->rm_lo_b_iface[instance]),
                             NULL );
      dsEpsTlb_hdlr_dereg_rm_events(
                           &(rmsm_state_ptr->rm_lo_phys_link[instance]),
                             rmsm_state_ptr,
                             instance);
      dsm_empty_queue(&(rmsm_state_ptr->rm_lo_rx_wmk[instance]));
      ps_iface_phys_link_down_ind(&(rmsm_state_ptr->rm_lo_b_iface[instance]));
      ps_iface_down_ind( &(rmsm_state_ptr->rm_lo_b_iface[instance]),
                            &down_info );
      /* Clear UM iface pointer so that reset loopback mode B Will not be 
         called for this already cleaned UM Iface.
      */
      rmsm_state_ptr->um_iface_ptr[instance] = NULL;
   }
  }
  /* Send close loopback Indication only if 
      1. if it is not already sent or
      2. other iface is valid and its state is in UP state
  */
  if( (hndl->tlb_state.sent_close_loopback_ind == FALSE ) &&
      (other_iface_ptr != NULL) && (IFACE_UP == iface_state) )
  {
    dsEpsTlb_post_close_ready_ind(hndl->subs_id);
  }
  if( NULL == other_iface_ptr)
  {
    /* If both Iface's are down, consider it as equivalent to  
       LTE_TLB_LB_DEACT_IND and clean up the Loopback state.
    */
    DS_LTE_MSG2_HIGH("Both IFACE's down EPS state(%d)and mode (%d))",
        (int)hndl->tlb_state.ds_eps_bearer_tlb_state,
                     hndl->tlb_state.mode);
    
    if( hndl->tlb_state.mode == LTE_TLB_MODE_B )
    {
      dsEpsTlb_reset_rm_loopback_modeB(hndl);
    }
    hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_INACTIVE;
    ds3gpp_enter_global_crit_section();
    hndl->tlb_state.mode = LTE_TLB_MODE_INVALID;
    ds3gpp_leave_global_crit_section();
    hndl->tlb_state.buffer_ip_pdu = FALSE;
    hndl->tlb_state.ip_pdu_delay_time=0;
    hndl->tlb_state.sent_close_loopback_ind = FALSE;
    hndl->tlb_state.cdma_2000_inited=FALSE;
  }

  /* Update the UM iface pointer as NULL only after RM de-registration 
  */  
  rmsm_state_ptr->um_iface_ptr[instance] = NULL;

  
}/* dsEpsTlb_um_iface_down_evt_hdlr */

/*===========================================================================
  FUNCTION DSEPSTLB_UM_IFACE_UP_EVT_HDLR()

  DESCRIPTION
    This function process IFACE_UP event

  PARAMTERS
    um_iface_ptr - Um iface Ptr
    eps_tlb_hndl - Handle to the TLB Information

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsEpsTlb_um_iface_up_evt_hdlr
(
  ps_iface_type                 *um_iface_ptr,
  ds_eps_tlb_per_subs_info_type *hndl
)
{
  ps_phys_link_link_protocol_handle_type  protocol_handle;
  ps_iface_type                         * bridge_iface_ptr = NULL;
  uint8                                   instance = DS_EPS_TLB_IPV4_IFACE;
  uint8                                   other_instance = DS_EPS_TLB_IPV4_IFACE;
  ps_iface_type                         * other_iface_ptr = NULL;
  ps_iface_state_enum_type                iface_state = IFACE_DOWN;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_LTE_MSG0_HIGH("Mode TLB mode, Processing Um iface up event");
  memset (&protocol_handle, 0,sizeof(ps_phys_link_link_protocol_handle_type));

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return;
  }
  
  /* Find the instance form um iface pointer */
  DSEPSTLB_GET_INST_FROM_UM_IFACE(um_iface_ptr,instance, hndl);

  /* Check if this is a valid instance */
  if(!(IS_DSEPSTLB_VALID_INSTANCE(instance)))
  {
    DS_LTE_MSG0_HIGH("Not a loopback UM iface ,return without processing");
    return;
  }
  /* Check if UM iface pointer is already bridged, 
     if so we have handled the UM iface up event as part 
     of IFACE bring up. No further action needed.
  */
  bridge_iface_ptr = ps_iface_bridge_iface(um_iface_ptr);
  if(bridge_iface_ptr != NULL)
  {
    DS_LTE_MSG0_HIGH("Processed Um iface up event already, return immediately");
    return;
  }
  bridge_iface_ptr = &(hndl->rmsm_state.rm_lo_b_iface[instance]);
  /*---------------------------------------------------------------------
    Setup IP fwd link and rev link bridging between RM_LO_B and UM 
    interface. Also send IFACE_UP and PHYS_LINK_UP indication
    for RM Iface.
  ---------------------------------------------------------------------*/
  ps_iface_set_bridge( bridge_iface_ptr,
                       um_iface_ptr );
  ps_iface_set_bridge( um_iface_ptr, 
                       bridge_iface_ptr );
  
  /*-------------------------------------------------------------------------
   Set the handler for the downlink signal for the call.
   -------------------------------------------------------------------------*/
  (void)ps_set_sig_handler(
                 hndl->rmsm_state.rx_sig[instance],
                 dsEpsTlb_rm_rx_data_cb,
                 (void*) bridge_iface_ptr);

  /*-------------------------------------------------------------------------  
     Enable it.
  ------------------------------------------------------------------------*/
  ps_enable_sig(hndl->rmsm_state.rx_sig[instance]);

  dsm_empty_queue(&(hndl->rmsm_state.rm_lo_rx_wmk[instance]));

  hndl->rmsm_state.rm_lo_rx_wmk[instance].total_rcvd_cnt = 0;
  hndl->rmsm_state.rm_lo_rx_wmk[instance].lowater_func_ptr = 
                          (wm_cb_type) dsEpsTlb_rm_flow_low_wm_cb;

  hndl->rmsm_state.rm_lo_rx_wmk[instance].lowater_func_data = 
                          (void *)&(hndl->rmsm_state.rm_lo_b_iface[instance]);
  
  hndl->rmsm_state.rm_lo_rx_wmk[instance].hiwater_func_ptr = 
                        (wm_cb_type) dsEpsTlb_rm_flow_high_wm_cb;
  
  hndl->rmsm_state.rm_lo_rx_wmk[instance].hiwater_func_data = 
                          (void *)&(hndl->rmsm_state.rm_lo_b_iface[instance]);

  /* Set High Watermark before low */
  dsm_set_hi_wm(&(hndl->rmsm_state.rm_lo_rx_wmk[instance]), DS_MODE_B_UL_WM_HI);
  dsm_set_low_wm(&(hndl->rmsm_state.rm_lo_rx_wmk[instance]), DS_MODE_B_UL_WM_LO);
  if(instance == DS_EPS_TLB_IPV4_IFACE)
  {
    protocol_handle.none_handle.handle.ip_proto_handle.v4_iface_ptr = 
                      &(hndl->rmsm_state.rm_lo_b_iface[instance]);
  }
  else
  {
    protocol_handle.none_handle.handle.ip_proto_handle.v6_iface_ptr = 
                      &(hndl->rmsm_state.rm_lo_b_iface[instance]);
  }

  protocol_handle.none_handle.high_protocol = PS_PHYS_LINK_HIGHER_LAYER_PROTOCOL_IP;

  /*---------------------------------------------------------------------------
    Set the link layer protocol to NONE and pass the protocol handle into the
    function that will do this. The protocol handle has just been populated
    with the higher layer protocol and the pointers to the ifaces that might
    communicate over this physlink.
  ---------------------------------------------------------------------------*/
 (void) ps_phys_link_set_link_protocol(
                    &(hndl->rmsm_state.rm_lo_phys_link[instance]),
                    PS_PHYS_LINK_LINK_PROTOCOL_NONE,
                    protocol_handle);

  ps_iface_up_ind( &(hndl->rmsm_state.rm_lo_b_iface[instance]) );
  ps_iface_phys_link_up_ind(&(hndl->rmsm_state.rm_lo_b_iface[instance]));

  dsEpsTlb_hdlr_reg_rm_events( 
                &(hndl->rmsm_state.rm_lo_phys_link[instance]),
                &(hndl->rmsm_state),
                instance);

  ps_phys_link_enable_flow (
               &(hndl->rmsm_state.rm_lo_phys_link[instance]), 
               DS_FLOW_UMTS_RMSM_MASK);

  /* Check for the other um instance, if it is in UP/DOWN state
     send the CLOSE ready indication */

  if(instance == DS_EPS_TLB_IPV4_IFACE)
  {
    other_instance = DS_EPS_TLB_IPV6_IFACE;
    other_iface_ptr = hndl->rmsm_state.um_iface_ptr[other_instance];
  }
  iface_state = ps_iface_state(other_iface_ptr);

  /* Send close loopback only if 
      1. if not already sent or
      2. other iface state is either down or UP
  */
  if( (hndl->tlb_state.sent_close_loopback_ind == FALSE ) &&
      ((other_iface_ptr == NULL) ||
       (IFACE_UP == iface_state) || (IFACE_DOWN == iface_state)) )
  {
    /* Inform TLB that DS is ready for incoming packets */
    dsEpsTlb_post_close_ready_ind(hndl->subs_id);
  }

}/* dsEpsTlb_um_iface_up_evt_hdlr */

/*===========================================================================
FUNCTION DSEPSTLB_MODEB_TIMER_EXP_CMD()

DESCRIPTION
  This function is called when we process the command posted to DS as a result
  of loopback ModeB timer expiry. This simply calls the function to set the
  bridge and enable the flow.

PARAMTERS
  hndl - Handle to the TLB Information

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void dsEpsTlb_modeB_timer_exp_cmd
(
  ds_eps_tlb_per_subs_info_type *hndl
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_HIGH("Loopback mode B timer expiry command handler");

  /* 3GPP TS 36.509,as per section Section 5.4.4.3;5.4.4.5;
     5.4.4.7.      
     In case of 3GPP mode, On timer expiry,
       1. Submit the buffered IP PDU's as the same orderas recieved
       2. Set BUFFER_IP_PDU = FALSE
     In case of C2K mode, then UE shall not take any action
       3GPP TS 36.509,as per section Section 5.4.4.9
  */

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  hndl->tlb_state.tmr_delay_modeB_running = FALSE;
  hndl->tlb_state.buffer_ip_pdu = FALSE;

  /* set signal to rm Ifaces for buffered data transfer */
  if(hndl->rmsm_state.um_iface_ptr[DS_EPS_TLB_IPV4_IFACE] != NULL)
  {
    PS_SET_SIGNAL( hndl->rmsm_state.rx_sig[DS_EPS_TLB_IPV4_IFACE]);
  }
  if(hndl->rmsm_state.um_iface_ptr[DS_EPS_TLB_IPV6_IFACE] != NULL)
  {
    PS_SET_SIGNAL( hndl->rmsm_state.rx_sig[DS_EPS_TLB_IPV6_IFACE]);
  }
} /* dsEpsTlb_modeB_timer_exp_cmd */


/*===========================================================================
FUNCTION DSEPSTLB_HDLR_RX_WM_EACH_ENQUE_CB

DESCRIPTION
  Each enque function for rx_wm of Mode B RM iface  

DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
/*ARGSUSED*/
void dsEpsTlb_hdlr_rx_wm_each_enqueue_cb 
(
  dsm_watermark_type *wm,
  void* callback_data
)
{
  ps_iface_type                     *rm_iface_ptr = 
                                       (ps_iface_type *)callback_data;
  uint8                              instance;
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(rm_iface_ptr == NULL)
  {
    DS_LTE_MSG0_HIGH("Invalid call back");
    return;
  }

  eps_tlb_hndl = ds_eps_tlb_get_hndl_from_rm_iface(rm_iface_ptr);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return;
  }

  DSEPSTLB_GET_INST_FROM_RM_IFACE(rm_iface_ptr,instance, eps_tlb_hndl);
  /* Check if this is a valid instance */
  if(!(IS_DSEPSTLB_VALID_INSTANCE(instance)))
  {
    DS_LTE_MSG0_HIGH("Not a loopback RM iface ,return without processing");
    return;
  }
  DS_LTE_MSG0_HIGH("set signal for rm iface");
  PS_SET_SIGNAL( eps_tlb_hndl->rmsm_state.rx_sig[instance]);
}/* dsEpsTlb_hdlr_rx_wm_each_enqueue_cb */

/*===========================================================================
  FUNCTION DSEPSTLB_UM_IFACE_EVT_HDLR()

  DESCRIPTION
    This function process IFACE_UP, IFACE_DOWN
    events

  PARAMTERS
    iface_event -  Iface Event
    eps_tlb_hndl - Handle to the TLB Information

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsEpsTlb_um_iface_evt_hdlr
(
  ds_eps_tlb_iface_event_type   iface_event,
  ds_eps_tlb_per_subs_info_type *hndl
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DS_LTE_MSG1_HIGH( "Recd Iface event %d ",iface_event.event);

  switch(iface_event.event)
  {
    case  IFACE_DOWN_EV:
      dsEpsTlb_um_iface_down_evt_hdlr(iface_event.um_iface_ptr, hndl);
      break;
    case  IFACE_UP_EV:
      dsEpsTlb_um_iface_up_evt_hdlr(iface_event.um_iface_ptr, hndl);
      break;

    default:
      DS_LTE_MSG1_ERROR("Invalid Event type :%d",iface_event.event);
  }

}/* dsEpsTlb_um_iface_evt_hdlr */

/*===========================================================================
  FUNCTION DSEPSTLB_RM_FLOW_EVT_HDLR()

  DESCRIPTION
    This function process RM FLOW Enable or Disable commands.

  PARAMTERS
    iface_event -  Flow Event
    eps_tlb_hndl - Handle to the TLB Information

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
LOCAL void dsEpsTlb_rm_flow_evt_hdlr
(
  ds_eps_tlb_rm_flow_evt_type   flow_event,
  ds_eps_tlb_per_subs_info_type *hndl
)
{
  uint8              instance;
  ps_iface_type     *rm_iface_ptr;
  ps_phys_link_type *rm_phys_link_ptr;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(flow_event.rm_iface_ptr != NULL);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  rm_iface_ptr= flow_event.rm_iface_ptr;

  DSEPSTLB_GET_INST_FROM_RM_IFACE(rm_iface_ptr,instance,hndl);
 
  /* Check if this is a valid instance */
  if(!(IS_DSEPSTLB_VALID_INSTANCE(instance)))
  {
    DS_LTE_MSG0_HIGH("Not a loopback RM iface ,return without processing");
    return;
  }
  rm_phys_link_ptr = &(hndl->rmsm_state.rm_lo_phys_link[instance]);

  DS_LTE_MSG1_HIGH("Mode TLB mode, rm_flow is %d", flow_event.rm_flow);

  if(flow_event.rm_flow == DS_EPS_RM_FLOW_ENABLE)
  {
    DS_LTE_MSG0_LOW("Mode TLB mode,enable RM flow");
    ps_phys_link_enable_flow (rm_phys_link_ptr, 
                               DS_FLOW_UMTS_RMSM_MASK);
  }
  else if (flow_event.rm_flow == DS_EPS_RM_FLOW_DISABLE)
  {
    DS_LTE_MSG0_LOW("Mode TLB mode,disable RM flow");
    ps_phys_link_disable_flow (rm_phys_link_ptr, 
                               DS_FLOW_UMTS_RMSM_MASK);
  }
  else
  {
    DS_LTE_MSG0_LOW("Incorrect flow command");
  }
}/* dsEpsTlb_rm_flow_evt_hdlr */

/*===========================================================================
FUNCTION DSEPSTLB_RM_EVENT_HANDLER_CB

DESCRIPTION

PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
  None
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void dsEpsTlb_rm_event_handler_cb
(
  ps_phys_link_type           *rm_phys_link_ptr,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                        *user_data_ptr
)
{
  ds_eps_rmsm_tlb_b_state_type    *rmsm_state_ptr;
  uint8                            instance;
  ds_eps_tlb_per_subs_info_type   *eps_tlb_hndl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  eps_tlb_hndl = (ds_eps_tlb_per_subs_info_type*)user_data_ptr;

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return;
  }

  rmsm_state_ptr = &(eps_tlb_hndl->rmsm_state);

  ASSERT(rmsm_state_ptr != NULL);

  /*---------------------------------------------------------------------------
    Take action based on the event. Set the Rm_flow_controlled flag in the 
    RMSTATE according to the event received. 
  ---------------------------------------------------------------------------*/
  DSEPSTLB_GET_INST_FROM_RM_PHYS_LINK_PTR(rm_phys_link_ptr,
                                          instance, 
                                          eps_tlb_hndl);
  
  if( instance >= DS_EPS_MAX_IFACE_PER_TLB )
  {
    DS_LTE_MSG1_LOW("Unknown instance: %d. Ignoring",instance);
    return;
  }

  DS_LTE_MSG1_HIGH("Event %d for rm iface", event);

  switch(event)
  {
    case PHYS_LINK_FLOW_ENABLED_EV:
      rmsm_state_ptr->rm_flow_enabled[instance] = TRUE;
      DS_LTE_MSG0_LOW("PHYS_LINK_FLOW_ENABLED_EV for rm iface");
      PS_SET_SIGNAL(rmsm_state_ptr->rx_sig[instance]);
      break;

    case PHYS_LINK_FLOW_DISABLED_EV:
      rmsm_state_ptr->rm_flow_enabled[instance] = FALSE;
      DS_LTE_MSG0_LOW("PHYS_LINK_FLOW_DISABLED_EV for Rm iface ");
      break;
      
    default:
      {
        DS_LTE_MSG1_LOW("Unknown event: %d. Ignoring",event);
      }
  }
}/* dsEpsTlb_rm_event_handler_cb */

/*===========================================================================
FUNCTION   DSEPSTLB_UM_EVENT_HANDLER_CB

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on the UM_PKT_IFACE. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/*ARGSUSED*/
void dsEpsTlb_um_event_handler_cb
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
)
{
  ds_cmd_type                      *cmd_ptr = NULL;                   /* DS Task cmd buf        */
  ds_eps_tlb_cmd_type               tlb_event_info;
  ps_iface_bearer_technology_type  *new_bearer_tech_ptr = NULL;
  uint8                             instance;
  ds_3gpp_tlb_cmd_info_type        *data_ptr = NULL;
  ds_eps_tlb_per_subs_info_type    *hndl = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_LTE_MSG2_HIGH( "Recd event %d for um iface 0x%x", event, this_iface_ptr);

  hndl = (ds_eps_tlb_per_subs_info_type*)user_data_ptr;

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return;
  }

  switch( event )
  {
    case IFACE_DOWN_EV:
      if( event_info.iface_down_info.state != IFACE_DOWN )
      {
        DS_LTE_MSG0_HIGH( "UM iface down, posting DOWN_EV");
      }
      else
      {
        DS_LTE_MSG1_MED( "Ignoring event %d", event);
        return;
      }
      break;

    case IFACE_UP_EV:
      DS_LTE_MSG0_HIGH( "Um Iface up, posting IFACE_UP_EV" );
      break;

    case IFACE_BEARER_TECH_CHANGED_EV:
    {
      /* Find the instance form um iface pointer */
      DSEPSTLB_GET_INST_FROM_UM_IFACE(this_iface_ptr,instance, hndl);
      /* Check if this is a valid instance */
      if(!(IS_DSEPSTLB_VALID_INSTANCE(instance)))
      {
        DS_LTE_MSG0_MED("Not processing IFACE_BEARER_TECH_CHANGED_EV");
        return;
      }
      new_bearer_tech_ptr = 
          &(event_info.bearer_tech_changed_info.new_bearer_tech);
      if( ((new_bearer_tech_ptr->current_network == PS_IFACE_NETWORK_3GPP) &&
           (new_bearer_tech_ptr->data.umts_type.rat_mask != 
                        PS_IFACE_3GPP_NULL_BEARER) )  ||
          ((new_bearer_tech_ptr->current_network == PS_IFACE_NETWORK_CDMA)&&
           (new_bearer_tech_ptr->data.cdma_type.rat_mask != 
                        PS_IFACE_CDMA_NULL_BEARER) )
          )
      {
        DS_LTE_MSG1_HIGH("Bearer tech changed ev, updating mode %d", 
                         (int)new_bearer_tech_ptr->current_network);
        hndl->rmsm_state.current_network = new_bearer_tech_ptr->current_network;
      }
      return;
    }
    default:
      DS_LTE_MSG1_ERROR( "Unregistered event %d recvd, ignoring", event);
      ASSERT(0);
  }

  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_tlb_cmd_info_type));
  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return;
  }
  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_TLB_MODE_B_HANDLER;
  data_ptr = (ds_3gpp_tlb_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
  data_ptr->event = (int)DS_EPS_UM_IFACE_EVT;
  data_ptr->hndl = hndl;
  tlb_event_info.iface_evt_info.event = event;
  tlb_event_info.iface_evt_info.um_iface_ptr = this_iface_ptr;
  data_ptr->event_info = tlb_event_info;
  ds_put_cmd(cmd_ptr);

}/* dsEpsTlb_um_event_handler_cb */

/*===========================================================================
FUNCTION DSEPSTLB_HDLR_IFACE_SETUP_BRIDGE_INTF_EVENT

DESCRIPTION
  This function calls the registration function to respond to rmnet's flow disable/enable events
  
PARAMETERS  
    
DEPENDENCIES 
  None.

RETURN VALUE 
 0 on SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
int dsEpsTlb_hdlr_rmiface_setup_bridge_intf_event 
( 
  ps_iface_type *this_iface_ptr,
  ps_iface_type *target_iface_ptr,
  void          *client_data
)
{
  ps_iface_type                  *iface_ptr = NULL; /* bridged iface pointer */
  ps_phys_link_type              *rm_phys_link_p = NULL;
  uint8                           instance ;
  uint32                          subs_id;
  ds_eps_tlb_per_subs_info_type  *eps_tlb_hndl = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_LTE_MSG1_HIGH("dsEpsTlb_hdlr_rmiface_setup_bridge_intf_event()0x%x",
                    this_iface_ptr);

  iface_ptr = ps_iface_bridge_iface(this_iface_ptr); 

  if(iface_ptr == NULL)
  {
    DS_LTE_MSG0_HIGH("Setting bridge Iface Ptr to NULL");
    return 0;
  }

  subs_id = PS_IFACE_GET_SUBS_ID(iface_ptr);

  eps_tlb_hndl = ds_eps_tlb_get_per_subs_info_hndl
                   (ds3gsubsmgr_subs_id_ds_to_cm(subs_id));
  
  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return 0;
  }

  rm_phys_link_p = PS_IFACE_GET_PHYS_LINK(this_iface_ptr);
  
  if (rm_phys_link_p == NULL) 
  {
    DATA_ERR_FATAL("rm_phys_link_p = NULL");
    return 0;
  }

  DSEPSTLB_GET_INST_FROM_UM_IFACE(iface_ptr,instance, eps_tlb_hndl);

  /* Check if this is a valid instance */
  if(!(IS_DSEPSTLB_VALID_INSTANCE(instance)))
  {
    DS_LTE_MSG0_HIGH("Not a loopback UM iface ,return without processing");
    return 0;
  }
  /*-------------------------------------------------------------------------
   Set the handler for the downlink signal for the call.
   -------------------------------------------------------------------------*/
  (void)ps_set_sig_handler(
                  eps_tlb_hndl->rmsm_state.rx_sig[instance],
                  dsEpsTlb_rm_rx_data_cb, 
                  (void *)this_iface_ptr);
  
  /*-------------------------------------------------------------------------  
    Enable it.
    -------------------------------------------------------------------------*/
  ps_enable_sig(eps_tlb_hndl->rmsm_state.rx_sig[instance]);  
  return 0;
}/* dsEpsTlb_hdlr_iface_setup_bridge_intf_event */

/*===========================================================================
FUNCTION DSEPSTLB_PDP_IP_TX_UM_DATA_CB

DESCRIPTION
  This function handles uplink data for bearers established in TLB mode. 
  For TLB mode A, all data will be simply discarded. TLB mode B is currently 
  not supported
  
PARAMETERS  
  call_id:     CM call id
  mode_info_p: mode specific info pointer
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
/*ARGSUSED*/
void dsEpsTlb_pdp_ip_tx_um_data_cb
(
  ps_phys_link_type *phys_link_ptr,
  dsm_item_type     **item_ptr,
  ps_meta_info_type *meta_info_ptr,
  void              *tx_info_ptr
)
{
  // We need to free DSM item...
  dsm_free_packet(item_ptr);
  /* In TLB active mode  all uplink data must be discarded */
  DS_LTE_MSG0_LOW("This bearer is established in TLB mode,discarding uplink data");
  PS_META_INFO_FREE(&meta_info_ptr);
  return;
}/* dsEpsTlb_pdp_ip_tx_um_data_cb */


/*===========================================================================
FUNCTION DSEPSTLB_PDP_IP_RX_UM_DROP_DATA_CB

DESCRIPTION
  This funciton dequeues RX watermark and drops data
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean dsEpsTlb_pdp_ip_rx_um_drop_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
)
{
  dsm_item_type           *item_ptr;          /* Pointer to DSM item         */
  ds_bearer_context_wm_s  *rx_watermark_ptr;
  ds_bearer_context_s     *bearer_context_p;  /* Pointer to bearer context   */

  /*-------------------------------------------------------------------------
    Get the bearer context from the call back data
  -------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)user_data_p;

   
  if ((bearer_context_p == NULL) ||
    (!ds_bearer_cntx_validate_bearer_context(bearer_context_p)))
  {
    return FALSE;
  }

  /*-----------------------------------------------------------------------
    Get PDCP state information for the call.
  -----------------------------------------------------------------------*/
  rx_watermark_ptr = &(bearer_context_p->lte_rx_wm);

  /*-----------------------------------------------------------------------
    The PDCP wm item has IP packets queued up. De-queue 
  -----------------------------------------------------------------------*/
  item_ptr = (dsm_item_type *)dsm_dequeue(&(rx_watermark_ptr->wm_item));

  if(item_ptr != NULL)
  {
    dsm_free_packet(&item_ptr);
    DS_LTE_MSG0_HIGH("TLB is not closed, dropping data");
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}/* dsEpsTlb_pdp_ip_rx_um_drop_data_cb */

/*===========================================================================
FUNCTION DSEPSTLB_PDP_IP_RX_UM_DATA_CB

DESCRIPTION
  This function handles rx data on the bearers which were established in 
  TLB mode. 
  In TLB is closed in mode A, data will be looped back. Otherwise, data will
  be dropped
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE if success, FALSE otherwise.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL boolean dsEpsTlb_pdp_ip_rx_um_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
)
{
  dsm_item_type                   *item_ptr;          /* Pointer to DSM item  */
  ds_bearer_context_wm_s          *rx_watermark_ptr, *tx_watermark_ptr;
  ds_bearer_context_s             *bearer_context_p;  /* Bearer context */
  ds_eps_bearer_context_s         *eps_bearer_context_p = NULL;
  boolean                          result = TRUE;
  sys_modem_as_id_e_type           subs_id;
  ds_eps_tlb_per_subs_info_type   *eps_tlb_hndl = NULL;

  /*-------------------------------------------------------------------------
    Get the bearer context from the call back data
  -------------------------------------------------------------------------*/
  bearer_context_p = (ds_bearer_context_s*)user_data_p;

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_context_p, &subs_id))
  {
    return FALSE;
  }

  eps_tlb_hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return FALSE;
  }

  eps_bearer_context_p = (ds_eps_bearer_context_s *)\
    (bearer_context_p->ds_bearer_context_dyn_p->
     vobj_data_ptr_tbl[DS_3GPP_EPS_VOBJ]);


  if((eps_bearer_context_p == NULL) || 
     (!ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_context_p)))
  {
    return FALSE;
  }

  if (PS_PHYS_LINK_FLOW_ENABLED(&(bearer_context_p->phys_link)) == FALSE) {
    DS_LTE_MSG0_MED("flow is disabled");
    return TRUE;
  }

  /*-----------------------------------------------------------------------
    Get PDCP state information for the call.
  -----------------------------------------------------------------------*/
  rx_watermark_ptr = &(bearer_context_p->tlb_rx_wm);
  tx_watermark_ptr = &(bearer_context_p->tx_wm);

  /*-----------------------------------------------------------------------
    The PDCP wm item has IP packets queued up. De-queue 
  -----------------------------------------------------------------------*/
  item_ptr = (dsm_item_type *)dsm_dequeue(&(rx_watermark_ptr->wm_item));

  if (item_ptr != NULL) {
    /*-----------------------------------------------------------------------
      If TLB is closed in mode A, convert the item and send it back. 
      Otherwise, simply free it
    -----------------------------------------------------------------------*/
    result = lte_tlb_mode_a_scaling_pdcp_packet(&item_ptr,
                     eps_bearer_context_p->ds_eps_bearer_context_dyn_p->rb_id);

    if ( (eps_tlb_hndl->tlb_state.ds_eps_bearer_tlb_state != 
          DS_EPS_BEARER_TLB_A_CLOSED) || (result != TRUE) ) 
    {
      DS_LTE_MSG0_HIGH("Dropping data on TLB bearer");
      dsm_free_packet(&item_ptr);
      return FALSE;
    }

    lte_pdcpul_tag_each_pkt(item_ptr);
    dsm_enqueue(&tx_watermark_ptr->wm_item, &item_ptr);
    return FALSE;
  }

  return TRUE;
}/* dsEpsTlb_pdp_ip_rx_um_data_cb */

/*===========================================================================
FUNCTION DSEPSTLB_BRING_UP_V4_IFACE

DESCRIPTION
  This function process the loopback mode 
  bringing up the V4 UM iface.
  
DEPENDENCIES 
  None.

RETURN VALUE 
 TRUE - SUCCESS
 FALSE - FAILURE
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean dsEpsTlb_bring_up_iface 
(
  ds_umts_pdp_type_enum_type  pdp_type,
  uint16                      prof_num,
  sys_modem_as_id_e_type      subs_id
)
{
  acl_policy_info_type              *acl_policy_info = NULL;
  ip_pkt_info_type                   ip_info;
  boolean                            result = TRUE;
  ps_iface_type                     *um_iface_ptr;
  int16                              ps_iface_errno = DS_EWOULDBLOCK;
  uint8                              instance = DS_EPS_TLB_IPV4_IFACE;
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  acl_policy_info = (acl_policy_info_type *)modem_mem_alloc(
                                   sizeof(acl_policy_info_type),
                                   MODEM_MEM_CLIENT_DATA_CRIT );

  if (acl_policy_info == NULL ) 
  {
    DS_LTE_MSG0_ERROR("FAIL modem_mem_alloc for dsEpsTlb_bring_up_iface");
    return  FALSE;
  }

  eps_tlb_hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return FALSE;
  }

  /* ACL info population */
  acl_init_policy_info(acl_policy_info);
  memset( acl_policy_info, 0, sizeof(acl_policy_info_type) );
  acl_policy_info->iface.kind = DSS_IFACE_NAME; 
  acl_policy_info->iface.info.name = DSS_IFACE_EPC_ANY;
  acl_policy_info->policy_flag = DSS_IFACE_POLICY_SPECIFIED;
  acl_policy_info->pdp_info = (acl_pdp_ctx_cfg_type) ((uint32)(prof_num));
  acl_policy_info->subs_id = ds3gsubsmgr_subs_id_cm_to_ds(subs_id);
  acl_policy_info->tlb_mode = TRUE;
  /* ip_info population */
  memset( &ip_info, 0, sizeof(ip_info) );
  if( DS_UMTS_PDP_IPV4 == pdp_type)
  {
    ip_info.ip_vsn = IP_V4;
    ip_info.ip_hdr.v4.dest.ps_s_addr = ROUTE_DEFAULT_DST;
    acl_policy_info->ip_family = IPV4_ADDR;
    instance = DS_EPS_TLB_IPV4_IFACE;
  }
  else if( DS_UMTS_PDP_IPV6 == pdp_type)
  {
    ip_info.ip_vsn = IP_V6;
    acl_policy_info->ip_family = IPV6_ADDR;
    instance = DS_EPS_TLB_IPV6_IFACE;
  }

  /*-------------------------------------------------------------------------
    Bring_up has to be set to TRUE here so that the in_use flag is to TRUE
    when the IFACE gets assigned.
  -------------------------------------------------------------------------*/
  acl_policy_info->bring_up = TRUE;

  acl_policy_info->allow_less_pref_sys = TRUE;

  if ( route_get( &ip_info, 
                  acl_policy_info, 
                  FALSE, 
                  NULL, 
                  &um_iface_ptr ) < 0 )
  {
    DS_LTE_MSG0_ERROR("route_get: no iface available");
    modem_mem_free((void *)acl_policy_info, MODEM_MEM_CLIENT_DATA_CRIT );
    return  FALSE;
  }

  eps_tlb_hndl->rmsm_state.um_iface_ptr[instance] = um_iface_ptr;

  DS_LTE_MSG1_HIGH("Activate Um iface 0x%1x",um_iface_ptr);

  if( result != FALSE )
  {
    /*---------------------------------------------------------------------
      Bring up the Um IFACE interface. If there is an error, 
      delete the allocated instance and return FALSE.
    ---------------------------------------------------------------------*/
    if ( ps_iface_bring_up_cmd( um_iface_ptr, 
                                &ps_iface_errno,
                                NULL ) == 0 )
    {
      /* we need to make sure before that the context is already up or not */
      /* if already up , Take action as if we have got IFACE_IP Event */
      DS_LTE_MSG1_HIGH("UM i/f 0x%lx already up", um_iface_ptr);
      result = TRUE;
    }
    else if ( ps_iface_errno != DS_EWOULDBLOCK )
    {
      DS_LTE_MSG2_ERROR("UM i/f 0x%lx error %d in bring up", 
                (uint32)um_iface_ptr, ps_iface_errno);
      /* Return ERROR */
      result =  FALSE;
      /* clear the um iface pointer from rmsm table */
      eps_tlb_hndl->rmsm_state.um_iface_ptr[instance] = NULL;
    }
    else
    {
      DS_LTE_MSG1_HIGH("um_iface 0x%lx is being brought up ",
          (uint32)um_iface_ptr);
      result = TRUE;
    }
  }
  modem_mem_free((void *)acl_policy_info, MODEM_MEM_CLIENT_DATA_CRIT );
  return result;
}/* dsEpsTlb_bring_up_v4_iface */

/*===========================================================================
FUNCTION DSEPSTLB_CLOSE_LOOPBACK_MODEB

DESCRIPTION
  This function process the loopback mode B related 
  processing of updating the state variable and 
  bringing up the UM iface.
  
DEPENDENCIES 
  None.

RETURN VALUE 
 0 - SUCCESS
 
SIDE EFFECTS 
  None.
  
===========================================================================*/
boolean dsEpsTlb_close_loopback_modeB 
(
  const lte_tlb_lb_close_tl_ind_s*  close_tl_ind
)
{
  uint16                         prof_num = 0;
  ds_umts_pdp_context_type      *prof_cntx_info_ptr;
  boolean                        result = TRUE;
  boolean                        v4_result= TRUE;
  boolean                        v6_result = TRUE;
  ps_iface_type                 *um_iface_ptr;
  uint8                          i;
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  ds_eps_tlb_per_subs_info_type* hndl = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (close_tl_ind == NULL)
  {
    DS_LTE_MSG0_ERROR("dsmsg is NULL");
    return FALSE;
  }

  subs_id = INST_ID_TO_SYS_AS_ID(close_tl_ind->hdr.inst_id);
  hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
  {
    return FALSE;
  }

  if( (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_ACTIVE )||
      (hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_B_OPEN) )
  {
    /* Update the state variables for TLB */
    /* 3GPP 36.509, section: 5.4.2.3 */
    hndl->tlb_state.cdma_2000_inited = FALSE;
    /* convert the data in sec to ms and store */
    hndl->tlb_state.ip_pdu_delay_time =  (dword)
                                (close_tl_ind->ip_ipu_delay) * 1000;
    if( hndl->tlb_state.ip_pdu_delay_time!= 0)
    {
      hndl->tlb_state.buffer_ip_pdu = TRUE;
    }
    else
    {
      hndl->tlb_state.buffer_ip_pdu = FALSE;
    }
  }
  else 
  {
    DS_LTE_MSG0_ERROR("Recieved CLOSE TL_IND before ACT_IND,ignore");
    return FALSE;
  }
  
  if(hndl->tlb_state.ds_eps_bearer_tlb_state == DS_EPS_BEARER_TLB_B_OPEN)
  {
    hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_B_CLOSED;
    /* Inform TLB that DS is ready for incoming packets */
     dsEpsTlb_post_close_ready_ind(hndl->subs_id);
    return result;
  }
  hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_B_CLOSED;
  /* Get the default attached profile and get the 
  ** um iface pointer associated with that
  */
  subs_id = ds3g_get_ps_subs_id();
  prof_num = ds_eps_get_attach_prof(subs_id);

  prof_cntx_info_ptr = (ds_umts_pdp_context_type *)modem_mem_alloc(
                       sizeof(ds_umts_pdp_context_type),
                       MODEM_MEM_CLIENT_DATA );
  if( (prof_cntx_info_ptr == NULL ) ||
      ( DS_UMTS_PDP_SUCCESS !=  ds_umts_get_pdp_profile_context_info_per_subs(
                                                         prof_num, 
                                                         dsumts_subs_mgr_get_subs_id(subs_id), 
                                                         prof_cntx_info_ptr)))
  {
    DS_LTE_MSG1_ERROR("PDP context data retrieval for prof %d failed",
                       prof_num);
    result =  FALSE;
  }
  if(result != FALSE )
  {
    result = dsEpsTlb_hdlr_reg_um_events(&(hndl->rmsm_state));
  }
  /*---------------------------------------------------------------------
    Register IFACE_UP and DOWN events with UM packet interface
  ---------------------------------------------------------------------*/
  if( (result != FALSE ) && 
      ( (prof_cntx_info_ptr->pdp_type == DS_UMTS_PDP_IPV4 )||
        (prof_cntx_info_ptr->pdp_type == DS_UMTS_PDP_IPV4V6) ) )
  {
    DS_LTE_MSG0_HIGH("Bring up V4 Um iface ");
    v4_result = dsEpsTlb_bring_up_iface(DS_UMTS_PDP_IPV4,prof_num, subs_id);
  }
  if( (result != FALSE ) && 
      ( (prof_cntx_info_ptr->pdp_type == DS_UMTS_PDP_IPV6 )||
        (prof_cntx_info_ptr->pdp_type == DS_UMTS_PDP_IPV4V6) ) )
  {
    DS_LTE_MSG0_HIGH("Bring up V6 Um iface ");
    v6_result = dsEpsTlb_bring_up_iface(DS_UMTS_PDP_IPV6,prof_num, subs_id);
  }

  if( (result != FALSE ) &&
      (( TRUE == v4_result) || ( TRUE == v6_result)) )
  {
    for(i =0;i< DS_EPS_MAX_IFACE_PER_TLB;i++)
    {
      um_iface_ptr = hndl->rmsm_state.um_iface_ptr[i];
      if( (NULL != um_iface_ptr) && 
          (IFACE_UP == ps_iface_state(um_iface_ptr)))
      {
        /* Send and IFACE_UP_EVENT, if the IFACE is already UP */
        dsEpsTlb_um_iface_up_evt_hdlr(um_iface_ptr, hndl);
      }
    }
  }

  if ( ( FALSE == result ) ||
       ( ( FALSE == v4_result) && ( FALSE == v6_result)) )
  {
    /* de-allocate the memory and clear the close loopback mode */
    hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_INACTIVE;
    ds3gpp_enter_global_crit_section();
    hndl->tlb_state.mode = LTE_TLB_MODE_INVALID;
    ds3gpp_leave_global_crit_section();
    hndl->tlb_state.buffer_ip_pdu = FALSE;
    hndl->tlb_state.ip_pdu_delay_time=0;
    hndl->tlb_state.sent_close_loopback_ind = FALSE;
    hndl->tlb_state.cdma_2000_inited=FALSE;
  }

  modem_mem_free((void *)prof_cntx_info_ptr, 
                 MODEM_MEM_CLIENT_DATA );
  return result;
}/* dsEpsTlb_close_loopback_modeB */

/*===========================================================================
FUNCTION  DSEPSBEARERCNTX_LTE_TLB_MSG_HDLR

DESCRIPTION
  This function handles messages coming from Test loopback module
  
PARAMETERS
  None.

DEPENDENCIES
  None.

RETURN VALUE
  boolean.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsEpsTlb_msg_hdlr 
(
  msgr_umid_type        msgrtype,
  const msgr_hdr_struct_type *dsmsg
)
{
  boolean                        bRet = FALSE;
  int                            i = 0; 
  sys_modem_as_id_e_type         subs_id = SYS_MODEM_AS_ID_NONE;
  ds_dsd_apm_ind_pay_load        tlb_payload;
  ds_eps_tlb_per_subs_info_type* hndl = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (dsmsg == NULL)
  {
    DS_LTE_MSG0_LOW("dsmsg is NULL");
    return FALSE;
  }
  
  switch (msgrtype) 
  {
    case LTE_TLB_LB_ACT_IND:
      subs_id = INST_ID_TO_SYS_AS_ID
              (((lte_tlb_lb_activation_ind_s*)dsmsg)->hdr.inst_id);

      hndl = ds_eps_tlb_find_or_allocate_subs_info_hndl(subs_id);

      if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
      {
        bRet = FALSE;
      }
      else
      {
        /* TLB activation recieved */
        if(((lte_tlb_lb_activation_ind_s*)dsmsg)->mode != LTE_TLB_MODE_INVALID)
        {
          hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_ACTIVE;
          ds3gpp_enter_global_crit_section();
          hndl->tlb_state.mode = 
               ((lte_tlb_lb_activation_ind_s*)dsmsg)->mode;
          ds3gpp_leave_global_crit_section();

          if(hndl->tlb_state.mode == LTE_TLB_MODE_A)
          {
            memset(&tlb_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));
            tlb_payload.subs_id = hndl->subs_id;
            tlb_payload.tlb_mode_active = TRUE;

            if(ds_dsd_apm_ind(DS_DSD_APM_TLB_MODE_ACT_IND,&tlb_payload)
                                                        != DS_DSD_APM_SUCCESS)
            {
              DS_LTE_MSG0_ERROR("Failed to send TLB Mode Active Ind to APM");
            }
          }
          

  #ifdef FEATURE_DATA_EMBMS
          if(hndl->tlb_state.mode==LTE_TLB_MODE_C)
          {
            (void)ds_3gpp_embms_tlbmc_activate_cb(hndl->subs_id);
          }
  #endif /* FEATURE_DATA_EMBMS */
          bRet = TRUE;
        }
        else
        {
          /* loopback mdoe will not be effective */
          DS_LTE_MSG0_ERROR("Recieved ACTIVATE TL_IND incorrect mode,failing loopback");
          bRet = FALSE;
        }
        DS_LTE_MSG3_HIGH("LTE_TLB_LB_ACT_IND  EPS state(%d)and mode(%d),bRet(%d)",
                         (int)hndl->tlb_state.ds_eps_bearer_tlb_state,
                         hndl->tlb_state.mode,(int)bRet);
      }
      break;

    case LTE_TLB_LB_CLOSE_TL_IND:

      subs_id = INST_ID_TO_SYS_AS_ID
              (((lte_tlb_lb_close_tl_ind_s*)dsmsg)->hdr.inst_id);

      hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

      if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
      {
        bRet = FALSE;
      }
      else
      {
        if (((lte_tlb_lb_close_tl_ind_s*)dsmsg)->mode == LTE_TLB_MODE_A) 
        {
          hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_A_CLOSED;
          for (i = 0; i < DS_3GPP_MAX_BEARER_CONTEXT; i++) 
          {
            dsEpsTlb_set_tlb_mode_A_params(i);  
          }
          bRet = TRUE;
          /* Inform TLB that DS is ready for incoming packets */
          dsEpsTlb_post_close_ready_ind(subs_id);
        }
        else if(((lte_tlb_lb_close_tl_ind_s*)dsmsg)->mode == LTE_TLB_MODE_B)
        {
          bRet = dsEpsTlb_close_loopback_modeB(((lte_tlb_lb_close_tl_ind_s*)dsmsg));
        }
  #ifdef FEATURE_DATA_EMBMS
        else if(((lte_tlb_lb_close_tl_ind_s*)dsmsg)->mode == LTE_TLB_MODE_C)
        {
          hndl->tlb_state.ds_eps_bearer_tlb_state=DS_EPS_BEARER_TLB_C_CLOSED;

          if(ds_3gpp_embms_tlbmc_close_cb(subs_id) == TRUE)
          {
            DS_LTE_MSG0_HIGH("state TLB_C_CLOSED. Post DS_CTL_READY_IND");
            /* Inform TLB that DS is ready for incoming packets */
            dsEpsTlb_post_close_ready_ind(subs_id);
          }
          bRet= TRUE;
        }
  #endif /* FEATURE_DATA_EMBMS */
        else {
          DS_LTE_MSG0_HIGH("Invalid TLB mode");
          bRet = FALSE;
        }
        DS_LTE_MSG3_HIGH("LTE_TLB_LB_CLOSE_TL_IND EPS state(%d)and mode (%d),bRet(%d)",
                          (int)hndl->tlb_state.ds_eps_bearer_tlb_state,
                          hndl->tlb_state.mode,(int)bRet);
      }
      break;

    case LTE_TLB_LB_DEACT_IND:

      subs_id = INST_ID_TO_SYS_AS_ID
              (((msgr_hdr_struct_type*)dsmsg)->inst_id);

      hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

      if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
      {
        bRet = FALSE;
      }
      else
      {
        DS_LTE_MSG2_HIGH("LTE_TLB_LB_DEACT_IND EPS state(%d)and mode (%d))",
            (int)hndl->tlb_state.ds_eps_bearer_tlb_state,
                         hndl->tlb_state.mode);
        
        if(hndl->tlb_state.mode == LTE_TLB_MODE_B )
        {
          dsEpsTlb_reset_rm_loopback_modeB(hndl);
        }

  #ifdef FEATURE_DATA_EMBMS
        if(hndl->tlb_state.mode == LTE_TLB_MODE_C)
        {
          ds_3gpp_embms_tlbmc_deactivate_cb(subs_id);
          /* Do not move deactivate after the state has changed
             to LTE_TLB_MODE_INVALID */
        }

        if(hndl->tlb_state.mode == LTE_TLB_MODE_A)
        {
          memset(&tlb_payload, 0, sizeof(ds_dsd_apm_ind_pay_load));
          tlb_payload.subs_id = subs_id;
          tlb_payload.tlb_mode_active = FALSE;

          if(ds_dsd_apm_ind(DS_DSD_APM_TLB_MODE_ACT_IND,&tlb_payload)
                                                      != DS_DSD_APM_SUCCESS)
          {
            DS_LTE_MSG0_ERROR("Failed to send TLB Mode De-Activate Ind to APM");
          }
        }
  #endif /* FEATURE_DATA_EMBMS */

        hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_INACTIVE;
        ds3gpp_enter_global_crit_section();
        hndl->tlb_state.mode = LTE_TLB_MODE_INVALID;
        ds3gpp_leave_global_crit_section();
        hndl->tlb_state.buffer_ip_pdu = FALSE;
        hndl->tlb_state.ip_pdu_delay_time=0;
        hndl->tlb_state.sent_close_loopback_ind = FALSE;
        hndl->tlb_state.cdma_2000_inited=FALSE;

        ds_eps_tlb_free_subs_info_hndl(subs_id);

        bRet = TRUE;
      }
      break;

    case LTE_TLB_LB_OPEN_IND:
      subs_id = INST_ID_TO_SYS_AS_ID
              (((lte_tlb_lb_open_ind_s*)dsmsg)->hdr.inst_id);

      hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

      if (!ds_eps_tlb_validate_per_subs_info_hndl(hndl))
      {
        bRet = FALSE;
      }
      else
      {
        if( (((lte_tlb_lb_open_ind_s*)dsmsg)->mode == LTE_TLB_MODE_B) &&
            (hndl->tlb_state.ds_eps_bearer_tlb_state ==
                              DS_EPS_BEARER_TLB_B_CLOSED )) 
        {
          /* if we set the state to open,all 
             recieving data will be discarded by the rmsm module */
          hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_B_OPEN;
          hndl->tlb_state.sent_close_loopback_ind= FALSE;
          bRet = TRUE;
        }
        else if (((lte_tlb_lb_open_ind_s*)dsmsg)->mode == LTE_TLB_MODE_A) 
        {
          hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_ACTIVE;
          bRet = TRUE;
        }

  #ifdef FEATURE_DATA_EMBMS
        else if(((lte_tlb_lb_open_ind_s*)dsmsg)->mode == LTE_TLB_MODE_C)
        {
          hndl->tlb_state.ds_eps_bearer_tlb_state = DS_EPS_BEARER_TLB_C_OPEN;
        }
  #endif /* FEATURE_DATA_EMBMS */

        DS_LTE_MSG3_HIGH("LTE_TLB_LB_OPEN_IND EPS state(%d)and mode (%d) bRet (%d)",
            (int)hndl->tlb_state.ds_eps_bearer_tlb_state,
            hndl->tlb_state.mode,(int)bRet);
      }
      break;

    default:
      break;
  }

  return bRet;
}/* dsEpsTlb_msg_hdlr */
/*===========================================================================
FUNCTION dsEpsTlb_set_tlb_mode_A_params()

DESCRIPTION
  This function is used to set the tlb rx watermark nonempty call back 
  function and set the signal handler for TLB Mode A.

PARAMETERS
 Bearer Index Pointer

DEPENDENCIES
  None.

RETURN VALUE
  None
  
SIDE EFFECTS
  None.
  
===========================================================================*/

void dsEpsTlb_set_tlb_mode_A_params
( 
  int index
)
{
  ds_bearer_context_s     *bearer_context_p;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((ds_eps_bearer_cntxt_validate_bearer_cntxt(&eps_bearer_context_tbl[index])) 
          && (eps_bearer_context_tbl[index].ds_eps_bearer_context_dyn_p->is_in_tlb 
          == TRUE))
  {
    bearer_context_p = eps_bearer_context_tbl[index].
           ds_eps_bearer_context_dyn_p->bearer_context_p;
    if( FALSE == ds_bearer_cntx_validate_bearer_context(bearer_context_p))
    {
      ASSERT(0);
      return;
    }

   (void)ps_set_sig_handler(
         bearer_context_p->rx_sig,
         dsEpsTlb_pdp_ip_rx_um_data_cb, 
         (void *)bearer_context_p);

    bearer_context_p->tlb_rx_wm.wm_item.non_empty_func_ptr = 
        (wm_cb_type)ds_3gpp_bearer_cntxt_rx_data_at_u_pdp_cb;
    bearer_context_p->tlb_rx_wm.wm_item.non_empty_func_data = 
        (void *) bearer_context_p;
  
  }
} /* dsEpsTlb_set_tlb_mode_A_params */

/*==============================================================================

FUNCTION DS_EPS_TLB_PER_SUBS_INFO_INIT

DESCRIPTION
  This function allocates and initializes TLB information for the given Subs Id.

PARAMETERS
  subs_id - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_eps_tlb_per_subs_info_init
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_eps_tlb_per_subs_info_type* hndl = NULL;
  int                            i = 0;
  int                            iface_instance =0;
  ps_iface_type*                 iface_ptr = NULL;
  int                            index = 0;

#ifdef FEATURE_DATA_PS_DATA_LOGGING
  /*-------------------------------------------------------------------------
    DPL iface ID which uniquely identifies each rm_iface
  -------------------------------------------------------------------------*/
  dpl_iid_ifname_enum_type       ifname[DS_EPS_MAX_IFACE_PER_TLB *
                                        DS_EPS_TLB_MAX_INSTANCES] =
  {
    DPL_IID_IFNAME_TLB_RM_IFACE_V4,    
    DPL_IID_IFNAME_TLB_RM_IFACE_V6
  };
#endif /* FEATURE_DATA_PS_DATA_LOGGING */

  ps_sig_enum_type              ps_sigs[DS_EPS_MAX_IFACE_PER_TLB*
                                        DS_EPS_TLB_MAX_INSTANCES] =
  {
    PS_3GPP_LO_B_UM_RX_Q_SIGNAL,
    PS_3GPP_LO_B_UM_RX_Q_SIGNAL_1
  };

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

  do
  {
    if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      break;
    }
    
    if (ds_eps_tlb_subs_id_map[subs_id].init == TRUE)
    {
      DS_LTE_MSG1_ERROR("Subs id(CM) %d already inited", subs_id);
      break;
    }

    hndl = (ds_eps_tlb_per_subs_info_type*)
             modem_mem_alloc (sizeof(ds_eps_tlb_per_subs_info_type),
                              MODEM_MEM_CLIENT_DATA);
    if (hndl == NULL)
    {
      DS_3GPP_MSG1_ERROR ("Memory allocation failed for Subs Id(CM): %d", 
                          subs_id );
      break;
    }

    index = ds_eps_tlb_set_per_subs_info_hndl(hndl);

    if (!ds_eps_tlb_validate_tlb_info_blk_index(index))
    {
      DS_3GPP_MSG1_ERROR ("invalid Index %d returned", index );
      modem_mem_free(hndl, MODEM_MEM_CLIENT_DATA);
      break;
    }

    hndl->cookie = DS_EPS_TLB_PER_SUBS_INFO_COOKIE;

    hndl->subs_id = SYS_MODEM_AS_ID_NONE;

    /* ---------------------------------------------------------------------
      In Use Flag will be set to FALSE by default. This flag will be set to TRUE
      when TLB Information Block is allocated. The flag will be reset when 
      the TLB Information Block is freed.
    ------------------------------------------------------------------------*/      
    hndl->in_use = FALSE;

    /* ---------------------------------------------------------------------
      Initialize TLB state and RMSM stae
    ------------------------------------------------------------------------*/
    memset((void*)&(hndl->tlb_state) ,0x0,sizeof(ds_eps_tlb_state_type));
    ds3gpp_enter_global_crit_section();
    hndl->tlb_state.mode = LTE_TLB_MODE_INVALID;
    ds3gpp_leave_global_crit_section();
   
    memset((void*)&(hndl->rmsm_state),0x0,sizeof(ds_eps_rmsm_tlb_b_state_type));
    hndl->rmsm_state.current_network= PS_IFACE_NETWORK_3GPP;      

    /*-----------------------------------------------------------------------
      Initialize the ps_iface event cback buffers for Um interface
    -----------------------------------------------------------------------*/
   
    hndl->rmsm_state.um_iface_up_buf_ptr = 
                                        ps_iface_alloc_event_cback_buf  
                                        ( 
                                          dsEpsTlb_um_event_handler_cb, 
                                          hndl 
                                        ); 
   
    hndl->rmsm_state.um_iface_down_buf_ptr = 
                                        ps_iface_alloc_event_cback_buf
                                        ( 
                                          dsEpsTlb_um_event_handler_cb, 
                                          hndl 
                                        );
    hndl->rmsm_state.um_iface_bearer_tech_change_buf_ptr =
                                        ps_iface_alloc_event_cback_buf
                                        ( 
                                          dsEpsTlb_um_event_handler_cb, 
                                          hndl 
                                        );
    for(i=0; i<DS_EPS_MAX_IFACE_PER_TLB; i++)
    {
      iface_ptr = &(hndl->rmsm_state.rm_lo_b_iface[i]);
      /* ---------------------------------------------------------------------
        Create LOOP BACK_MODE_B_IFACE. 
      ------------------------------------------------------------------------*/
      iface_instance = ps_iface_create(iface_ptr, 
                                       LO_MODE_B_IFACE, 
                                       NULL,
                                       NULL,
                                       &(hndl->rmsm_state.\
                                         rm_lo_phys_link[i]),
                                       1);
      DS_LTE_MSG1_HIGH("LO_MODE_B Iface created :inst:%d",iface_instance);
      ASSERT(iface_instance >= 0);
      
      hndl->rmsm_state.rx_sig[i] = (ps_sig_enum_type)
                                     (ps_sigs[(index * 2) + i]);

      /*-----------------------------------------------------------------------
        Bridge processing function cmd callback.
      -----------------------------------------------------------------------*/
      iface_ptr->bridge_proc_f_ptr = dsEpsTlb_hdlr_rmiface_setup_bridge_intf_event;
      
      /*-----------------------------------------------------------------------
        Set Rm iface tx cmd as well
      -----------------------------------------------------------------------*/

      ps_iface_set_tx_function(iface_ptr, 
                               dsEpsTlb_hdlr_mode_b_tx_data_cb, 
                               (void*)hndl);
    
      hndl->rmsm_state.rm_flow_enable_buf_ptr[i] =
              ps_phys_link_alloc_event_cback_buf(dsEpsTlb_rm_event_handler_cb,
                                                 hndl);
      
      hndl->rmsm_state.rm_flow_disable_buf_ptr[i] =
                ps_phys_link_alloc_event_cback_buf(dsEpsTlb_rm_event_handler_cb,
                                                   hndl);
    
      ps_iface_enable_ind(iface_ptr);
    
      /*-----------------------------------------------------------------------
        Enable flow on the interface.
      -----------------------------------------------------------------------*/    
      ps_iface_enable_flow(iface_ptr, DS_FLOW_UMTS_RMSM_MASK);
    
      /*-----------------------------------------------------------------------
        Initialize the watermarks and queues.
      -----------------------------------------------------------------------*/
      dsm_queue_init ( &(hndl->rmsm_state.rm_lo_rx_wmk[i]),
                         DS_MODE_B_UL_WM_DNE,
                       &(hndl->rmsm_state.rm_lo_rx_wmk_q[i]));

      dsm_queue_init (&(hndl->tlb_state.tlb_rx_wm.wm_item),
                       DS_MODE_B_UL_WM_DNE,
                      &(hndl->tlb_state.tlb_rx_wm.wm_queue));
     
      hndl->rmsm_state.rm_lo_rx_wmk[i].each_enqueue_func_ptr = 
                        (wm_cb_type)dsEpsTlb_hdlr_rx_wm_each_enqueue_cb;
                         /* RMSM instance */
      hndl->rmsm_state.rm_lo_rx_wmk[i].each_enqueue_func_data = 
                         (void*)(iface_ptr);
      /*-----------------------------------------------------------------------
        Enable Logging on RM interfaces.        
      -----------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_DATA_LOGGING
     (void) ps_iface_dpl_support_network_logging(iface_ptr, 
                                                 ifname[(index * 2) + i]);
     (void) ps_iface_dpl_set_iface_desc(iface_ptr, NULL);
#endif /* FEATURE_DATA_PS_DATA_LOGGING */
   
    }/* for .. */

    /*-------------------------------------------------------------------------
      Define the timer for Loopback mode B handling
    -------------------------------------------------------------------------*/
    rex_def_timer_ex(&(hndl->tlb_state.tmr_delay_modeB),
                     (rex_timer_cb_type) dsEpsTlb_modeB_timer_cb,
                     (unsigned long)hndl);

    ds_eps_tlb_subs_id_map[subs_id].init = TRUE;
  } while (0); 

  return;
} /* ds_eps_tlb_per_subs_info_init */

/*===========================================================================
FUNCTION DSEPSTLB_INIT

DESCRIPTION
  Initializes test loopback specific handler
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
void dsEpsTlb_init (void)
{
  uint8 subs_index = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------
    Register the TLB MSGRTR for all TLB related indications
  ------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(
    LTE_TLB_LB_ACT_IND,
            (dsmsgrcv_msg_hdlr_f)dsEpsTlb_msg_hdlr
  ); 
  dsmsgrrcv_reg_hdlr(
    LTE_TLB_LB_CLOSE_TL_IND,
            (dsmsgrcv_msg_hdlr_f)dsEpsTlb_msg_hdlr
  );
  dsmsgrrcv_reg_hdlr(
    LTE_TLB_LB_DEACT_IND,
            (dsmsgrcv_msg_hdlr_f)dsEpsTlb_msg_hdlr
  );
  dsmsgrrcv_reg_hdlr(
    LTE_TLB_LB_OPEN_IND,
            (dsmsgrcv_msg_hdlr_f)dsEpsTlb_msg_hdlr
  ); 
 
  for (subs_index = 0; subs_index < DS3GSUBSMGR_SUBS_ID_MAX; subs_index++)
  {
    if (ds_eps_tlb_subs_id_map[subs_index].init == FALSE)
    {
      ds_eps_tlb_subs_id_map[subs_index].index = DS_EPS_TLB_INVALID_INSTANCE;
    }
  }

  /*======================================================================
     During powerup, we allocate TLB Information only for
     DDS.
   
     If Dual SIM / Triple SIM Device mode event is received at
     a later stage, we will allocate the information for other
     subscriptions. 
  ======================================================================*/
  ds_eps_tlb_per_subs_info_init(ds3g_get_ps_subs_id());

}/* dsEpsTlb_init */

/*===========================================================================
FUNCTION DSEPSTLB_MODEB_HDLR_CMD()

DESCRIPTION
  This function is called when we process the command posted to DS as a result
  of loopback ModeB .

DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.
  
===========================================================================*/
void dsEpsTlb_modeB_hdlr_cmd
(
  const ds_cmd_type *cmd_ptr                         /* Pointer to Command */
)
{
  ds_eps_tlb_cmd_type            tlb_cmd;
  ds_eps_tlb_cmd_e_type          cmd_type;
  ds_3gpp_tlb_cmd_info_type     *data_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(cmd_ptr != NULL)
  ASSERT(cmd_ptr->cmd_payload_ptr != NULL);

  data_ptr = (ds_3gpp_tlb_cmd_info_type*)cmd_ptr->cmd_payload_ptr;
  cmd_type = (ds_eps_tlb_cmd_e_type) data_ptr->event;
  tlb_cmd = data_ptr->event_info;

  switch(cmd_type)
  {
    case DS_EPS_MODE_B_TMR_EXP:
      dsEpsTlb_modeB_timer_exp_cmd(data_ptr->hndl);
      break;

    case DS_EPS_UM_IFACE_EVT:
      dsEpsTlb_um_iface_evt_hdlr(tlb_cmd.iface_evt_info, data_ptr->hndl);
      break;

    case DS_EPS_RM_FLOW_EVT:
      dsEpsTlb_rm_flow_evt_hdlr(tlb_cmd.rm_flow_evt_info, data_ptr->hndl);
      break;

    default:
      DS_LTE_MSG1_LOW("Invalid command posted %d",(int)cmd_type);
      break;
  }

}/* dsEpsTlb_modeB_hdlr_cmd */



/*===========================================================================
FUNCTION dsEpsTlb_is_tlb_in_mode_C()

DESCRIPTION
  This function is used to check if the UE is in Test loop mode Mode C.

PARAMSTERS 
  subs_id: Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Boolean 1 = True  ( Test mode in Mode C)
          0 = False ( Not in Test Mode C)
  
SIDE EFFECTS
  None.
  
===========================================================================*/
boolean dsEpsTlb_is_tlb_in_mode_C
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl = NULL; 
  /*----------------------------------------------------------------------*/

  eps_tlb_hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return FALSE;
  }

  DS_LTE_MSG1_HIGH("TLB Mode is in mode %d",
                    eps_tlb_hndl->tlb_state.mode);

  if(eps_tlb_hndl->tlb_state.mode==LTE_TLB_MODE_C)
  {
    return TRUE;
  }
  return FALSE;

}/* dsEpsTlb_is_tlb_in_mode_C  */


/*===========================================================================
  FUNCTION DSEPSTLB_POST_IFACE_UP_IND_MODE_C

  DESCRIPTION
    This function post IFACE UP indication. If close ready 
    indication is not already send to lower layers, it will be 
    send for Mode C in this function. 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsEpsTlb_post_iface_up_ind_mode_C
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl = NULL; 
  /*-----------------------------------------------------------------------*/

  eps_tlb_hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return;
  }

  if( (eps_tlb_hndl->tlb_state.ds_eps_bearer_tlb_state ==
                                  DS_EPS_BEARER_TLB_C_CLOSED ) &&
      (eps_tlb_hndl->tlb_state.sent_close_loopback_ind == FALSE))
  {
#ifdef FEATURE_DATA_EMBMS
    (void) ds_3gpp_embms_tlbmc_close_cb(eps_tlb_hndl->subs_id);
#endif
     /* Inform TLB that DS is ready for incoming packets */
     dsEpsTlb_post_close_ready_ind(eps_tlb_hndl->subs_id);
  }
}/* dsEpsTlb_post_iface_up_ind_mode_C */

/*===========================================================================
  FUNCTION DSEPSTLB_BEARER_PHYS_LINK_ENABLE_FLOW_CB

  DESCRIPTION
    This function is the callback function for bearer phys link enable flow.
    It sets the rx signal to start dequeuing from the tlb_wm.

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void dsEpsTlb_bearer_phys_link_enable_flow_cb
(
  ps_phys_link_type           *rm_phys_link_ptr,
  ps_iface_event_enum_type     event,
  ps_iface_event_info_u_type   event_info,
  void                        *user_data_ptr
)
{
  ds_bearer_context_s               *bearer_cntx_p;
  ds_eps_bearer_context_s           *eps_bearer_cntx_p;
  sys_modem_as_id_e_type             subs_id;
  ds_eps_tlb_per_subs_info_type     *eps_tlb_hndl = NULL;
  /*------------------------------------------------------------------------*/

  bearer_cntx_p = (ds_bearer_context_s *)user_data_ptr;

  if (!ds_bearer_cntxt_get_subs_id_from_bearer(bearer_cntx_p, &subs_id))
  {
    return;
  }

  eps_tlb_hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id);

  if (!ds_eps_tlb_validate_per_subs_info_hndl(eps_tlb_hndl))
  {
    return;
  }

  if (event == PHYS_LINK_FLOW_ENABLED_EV)
  {
      eps_bearer_cntx_p = (ds_eps_bearer_context_s *)(DS_BEARER_CTXT_VOBJ(
                             bearer_cntx_p, SYS_SYS_MODE_LTE));
      if (ds_eps_bearer_cntxt_validate_bearer_cntxt(eps_bearer_cntx_p) &&  
          eps_bearer_cntx_p->ds_eps_bearer_context_dyn_p->is_in_tlb == TRUE
          && eps_tlb_hndl->tlb_state.mode == LTE_TLB_MODE_A)
      {
        PS_SET_SIGNAL(bearer_cntx_p->rx_sig);
      }
  }
} /* dsEpsTlb_bearer_phys_link_enable_flow_cb */

/*===========================================================================
FUNCTION ds_eps_tlb_get_per_subs_info_hndl

DESCRIPTION
  This function is used to fetch the TLB information corresponding to
  the given Subs Id
 
PARAMETERS 
  Subs Id - Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the TLB Info
  
SIDE EFFECTS
  None.
  
===========================================================================*/
ds_eps_tlb_per_subs_info_type* ds_eps_tlb_get_per_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_eps_tlb_per_subs_info_type *hndl = NULL;
  int                            i = 0;
  /*--------------------------------------------------------------------------*/

  do
  {
    i = ds_eps_tlb_get_tlb_info_blk_index(subs_id);

    if (!ds_eps_tlb_validate_tlb_info_blk_index(i))
    {
      DS_LTE_MSG1_ERROR("Index %d is invalid", i);
      break;
    }

    if (ds_eps_tlb_info_p[i] != NULL &&
        ds_eps_tlb_info_p[i]->subs_id == subs_id)
    {
      hndl = ds_eps_tlb_info_p[i];
    }

  } while (0);

  return hndl;
} /* ds_eps_tlb_get_per_subs_info_hndl */

/*==========================================================================
FUNCTION ds_eps_tlb_set_per_subs_info_hndl

DESCRIPTION
  This function is used to set the TLB information pointer
  corresponding to the given Subs Id
 
PARAMETERS 
  hndl    - Pointer to the TLB Information  
  subs_id - Subscription Id
 
DEPENDENCIES
  None.

RETURN VALUE
  TLB Information Block Instance
  
SIDE EFFECTS
  None.
===========================================================================*/
static int ds_eps_tlb_set_per_subs_info_hndl
(
  ds_eps_tlb_per_subs_info_type*  hndl
)
{
  int  index  = 0;
  /*--------------------------------------------------------------------------*/

  do
  {
    for (index=0;index<DS_EPS_TLB_MAX_INSTANCES;index++)
    {
      if (ds_eps_tlb_info_p[index] == NULL)
      {
        ds_eps_tlb_info_p[index] = hndl;
        break;
      }    
    }

  } while (0); 

  DS_LTE_MSG2_HIGH("Set Ctrl Blk 0x%x on index %", hndl, index);
  return index;

} /* ds_eps_tlb_set_per_subs_info_hndl */

/*==========================================================================
FUNCTION ds_eps_tlb_validate_per_subs_info_hndl

DESCRIPTION
  This function is used to validate the TLB information handle.
 
PARAMETERS 
  hndl    - Pointer to the TLB Information  
 
DEPENDENCIES
  None.

RETURN VALUE
  TRUE, if the handle is valid
  FALSE, otherwise
  
SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_tlb_validate_per_subs_info_hndl
(
  ds_eps_tlb_per_subs_info_type*  hndl
)
{
  boolean ret_val = FALSE;
  /*----------------------------------------------------------*/

  if (hndl != NULL && hndl->cookie == DS_EPS_TLB_PER_SUBS_INFO_COOKIE)
  {
    ret_val = TRUE;
  }
  else
  {
    DS_LTE_MSG1_ERROR("EPS TLB info hndl:0x%x is INVALID",hndl);
  }
  
  return ret_val;
} /* ds_eps_tlb_validate_per_subs_info_hndl */

/*==============================================================================
FUNCTION      DS_EPS_TLB_FIND_OR_ALLOCATE_CTRL_BLK

DESCRIPTION
  This function is used to find the TLB Information Block corresponding to 
  passed subs_id. If no control block is found, allocate a new Control Block and
  return a pointer to that.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES  
  None

RETURN VALUE  
  Pointer to the TLB Info Block.

SIDE EFFECTS
  None
==============================================================================*/
static ds_eps_tlb_per_subs_info_type* ds_eps_tlb_find_or_allocate_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  int                             i = 0;
  ds_eps_tlb_per_subs_info_type  *hndl = NULL;
  /*--------------------------------------------------------------------------*/

  do
  { 
    if ((hndl = ds_eps_tlb_get_per_subs_info_hndl(subs_id)) == NULL)
    {
      for (i=0; i<DS_EPS_TLB_MAX_INSTANCES; i++)
      {
        if (ds_eps_tlb_info_p[i] != NULL &&
            ds_eps_tlb_info_p[i]->in_use == FALSE)
        {
          ds_eps_tlb_info_p[i]->in_use = TRUE;
          ds_eps_tlb_info_p[i]->subs_id = subs_id;

          ds_eps_tlb_set_tlb_info_blk_index(i, subs_id);

          DS_LTE_MSG2_HIGH("Allocated TLB Info block at index %d for subs id %d",
                            i, subs_id);
          hndl = ds_eps_tlb_info_p[i];
          break;
        }
      }
    }
  } while (0); 

  return hndl;

} /* ds_eps_tlb_find_or_allocate_subs_info_hndl */

/*==============================================================================
FUNCTION      DS_EPS_TLB_FREE_SUBS_INFO_HNDL

DESCRIPTION
  This function is used to free the TLB information block corresponding to the
  given subs_id.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES  
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_eps_tlb_free_subs_info_hndl
(
  sys_modem_as_id_e_type subs_id
)
{
  int index =0;
  /*--------------------------------------------------------------------------*/

  do
  {
    index = ds_eps_tlb_get_tlb_info_blk_index(subs_id);

    if (!ds_eps_tlb_validate_tlb_info_blk_index(index))
    {
      DS_LTE_MSG1_ERROR("Ctrl Blk Index %d is invalid", index);
      break;
    }

    if (ds_eps_tlb_info_p[index] != NULL &&
        ds_eps_tlb_info_p[index]->subs_id == subs_id)
    {
      DS_LTE_MSG2_HIGH("Releasing Cntrl block at index %d for subs id %d",
                         index, ds_eps_tlb_info_p[index]->subs_id);

      ds_eps_tlb_info_p[index]->in_use = FALSE;
      ds_eps_tlb_info_p[index]->subs_id = SYS_MODEM_AS_ID_NONE;
      ds_eps_tlb_set_tlb_info_blk_index(DS_EPS_TLB_INVALID_INSTANCE, subs_id);
      break;
    }
  } while (0); 
 
  return;
} /* ds_eps_tlb_free_subs_info_hndl */

/*==============================================================================
FUNCTION DS_3GPP_GET_TLB_INFO_BLK_INDEX

DESCRIPTION
  This functions fetches the TLB Info Blk index corresponding to the
  given Subs Id.
 
PARAMETERS 
  Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Index to the TLB Info Blk

SIDE EFFECTS
  None.
==============================================================================*/
static int ds_eps_tlb_get_tlb_info_blk_index
(
  sys_modem_as_id_e_type subs_id
)
{
  int index = -1;
  /*------------------------------------------------------------------*/

  do
  {
    if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      break;
    }
    
    index = ds_eps_tlb_subs_id_map[subs_id].index;

    DS_LTE_MSG2_LOW("Subs Id: %d, index: %d", subs_id, index);

  } while (0); 

  return index;
} /* ds_eps_tlb_get_index */

/*==============================================================================
FUNCTION DS_3GPP_SET_TLB_INFO_BLK_INDEX

DESCRIPTION
  This functions maps the Subs Id and TLB Info Blk Index
 
PARAMETERS 
  Index to the TLB Info Blk
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
static void ds_eps_tlb_set_tlb_info_blk_index
(
  int                    index,
  sys_modem_as_id_e_type subs_id
)
{
  do
  {
    if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      break;
    }

    DS_LTE_MSG2_MED("Subs Id: %d, index: %d", 
                      subs_id, index);

    ds_eps_tlb_subs_id_map[subs_id].index = index;
  } while (0); 

  return;
} /* ds_eps_tlb_set_tlb_info_blk_index */

/*==============================================================================
FUNCTION DS_EPS_TLB_VALIDATE_TLB_INFO_BLK_INDEX

DESCRIPTION
  This functions checks whether the TLB Information Block Index is Valid
 
PARAMETERS 
  Index 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
static boolean ds_eps_tlb_validate_tlb_info_blk_index
(
  uint8 index
)
{
  boolean ret_val = FALSE;
  /*--------------------------------------------------------------------------*/

  if (index < DS_EPS_TLB_MAX_INSTANCES)
  {
    ret_val = TRUE;
  }

  return ret_val;
} /* ds_eps_tlb_validate_tlb_info_blk_index */

/*==========================================================================
FUNCTION ds_eps_tlb_get_hndl_from_rm_iface

DESCRIPTION
  This function is used to fetch the TLB Info handle from the Rm Iface
 
PARAMETERS 
  rm_iface_ptr - Pointer to the Rm Iface 
 
DEPENDENCIES
  None.

RETURN VALUE
  Handle to the TLB Information
  
SIDE EFFECTS
  None.
===========================================================================*/
ds_eps_tlb_per_subs_info_type* ds_eps_tlb_get_hndl_from_rm_iface
(
  ps_iface_type* rm_iface_ptr
)
{
  int                            i = 0, j = 0;
  ds_eps_tlb_per_subs_info_type *hndl = NULL;
  /*---------------------------------------------------------------------*/

  for (i=0; i< DS3GSUBSMGR_SUBS_ID_MAX;i++)
  {
    hndl = ds_eps_tlb_get_per_subs_info_hndl(i);

    if (ds_eps_tlb_validate_per_subs_info_hndl(hndl))
    {
      for (j=0; j< DS_EPS_MAX_IFACE_PER_TLB; j++)
      {
        if (&(hndl->rmsm_state.rm_lo_b_iface[j]) == rm_iface_ptr)
        {
          return hndl;
        }
      }
    }
  }
  return NULL;
} /* ds_eps_tlb_get_subs_id_from_rm_iface */

#endif /* FEATURE_DATA_LTE */
