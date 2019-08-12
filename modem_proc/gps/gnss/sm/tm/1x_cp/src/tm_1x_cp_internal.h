#ifndef  TM_1X_CP_INTERNAL_H
#define  TM_1X_CP_INTERNAL_H
#include "gps_variation.h"
#include "tm_1x_cp.h"
#include "tm_common.h"
#include "tm_is801.h"



/*===========================================================================

FUNCTION tm_1x_cp_start_sess_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_cp_start_sess_req(void);


/*===========================================================================

FUNCTION tm_1x_cp_terminate_sess_req

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_cp_terminate_sess_req(tm_sess_stop_reason_e_type reason);


/*===========================================================================

FUNCTION tm_1x_cp_start_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_cp_start_ppm(tm_1x_cp_sess_req_e_type req_type);

/*===========================================================================

FUNCTION tm_1x_cp_get_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_cp_get_ppm(void);


/*===========================================================================

FUNCTION tm_1x_cp_end_ppm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_cp_end_ppm(void);


/*===========================================================================

FUNCTION tm_1x_cp_start_prm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_cp_start_prm(void);

          
/*===========================================================================

FUNCTION tm_1x_cp_end_prm

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern boolean tm_1x_cp_end_prm(void);

/*===========================================================================

FUNCTION tm_1x_cp_terminate_session

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
extern void tm_1x_cp_terminate_session
(
  tm_1x_cp_term_src_e_type    term_src, 
  tm_sess_stop_reason_e_type  reason
);

/*===========================================================================


FUNCTION tm_1x_cp_timer_cb_handler


DESCRIPTION

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

  None.
===========================================================================*/

extern void tm_1x_cp_timer_cb_handler
(
  void *timer_data
);

/*===========================================================================

FUNCTION tm_1x_cp_send_is801_1_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_send_is801_1_rlink_pddm(byte sess_end);

/*===========================================================================

FUNCTION xlate_fl_resp_mask_is801_1_to_1x_cp

DESCRIPTION: Translate from Is801-1 fwd link response type to generic 1x CP
             response type.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_resp_type xlate_fl_resp_mask_is801_1_to_1x_cp
(  
  is801_fwd_link_resp_mask_type resp_mask
);

/*===========================================================================

FUNCTION xlate_fl_req_mask_is801_1_to_1x_cp

DESCRIPTION: Translate from Is801-1 fwd link request type to generic 1x CP
             request type.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_req_type xlate_fl_req_mask_is801_1_to_1x_cp
(  
  is801_fwd_link_req_mask_type *req_mask
);

/*===========================================================================

FUNCTION i_tm_1x_cp_is801_1_post_gps_aa

DESCRIPTION: post 801-1 gps aa to mgp.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void i_tm_1x_cp_is801_1_post_gps_aa
(
  tm_post_data_payload_type   *aiding_data_ptr,
  is801_fwd_link_resp_s_type  *resp_ptr,
  gps_PreFixPpmSructType     **cp_prefix_ppm_buff_ptr
);

/*===========================================================================

FUNCTION i_tm_1x_cp_create_is801_1_ppm_resp

DESCRIPTION: Create a ppm response using the given ppms and the rlink buffer.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS
  Will terminate active session if ppm response can not be created

===========================================================================*/
boolean i_tm_1x_cp_create_is801_1_ppm_resp
(
  is801_rev_link_resp_s_type *resp_ptr,
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited 
);

/*===========================================================================

FUNCTION i_tm_1x_cp_create_is801_b_ppm_resp

DESCRIPTION: Create a ppm response using the given ppms and the rlink buffer.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS
  Will terminate active session if ppm response can not be created

===========================================================================*/
boolean i_tm_1x_cp_create_is801_b_ppm_resp
(
  u_new_rev_link_resp_params *is801b_resp_parm_ptr,
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited 
);


/*===========================================================================

FUNCTION i_tm_1x_cp_create_and_tx_is801_1_tom_resp

DESCRIPTION: create an send to pddm an 801 time offset measurement response

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean i_tm_1x_cp_create_and_tx_is801_1_tom_resp
(
  is801_rev_link_resp_s_type *resp_ptr,
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited 
);

/*===========================================================================

FUNCTION i_tm_1x_cp_create_and_tx_is801_b_tom_resp

DESCRIPTION: create an send to pddm an 801 time offset measurement response

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
boolean i_tm_1x_cp_create_and_tx_is801_b_tom_resp
(
  u_new_rev_link_resp_params *is801b_resp_parm_ptr,
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited 
);

/*===========================================================================

FUNCTION i_tm_1x_cp_create_is801_1_prm_resp

DESCRIPTION: Create and transmit a prm response using the given prms and 
             the rlink buffer.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS
  Will terminate active session if prm response can not be created

===========================================================================*/
boolean i_tm_1x_cp_create_is801_1_prm_resp
(
  is801_rev_link_resp_s_type *resp_ptr,
  srch_ppm_rpt_type          *ppm_ptr,
  uint8                       unsolicited, 
  sm_GpsMeasRptStructType    *cp_prm_buff_ptr
);

#ifdef FEATURE_GNSS_SM_TM_1X_CP_IS801B_ENABLE
#error code not present
#endif

/*===========================================================================
FUNCTION i_tm_1x_cp_create_msi_resp

DESCRIPTION: Create an 801-1 msi response.

DEPENDENCIES
  
RETURN VALUE  
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS
===========================================================================*/
boolean i_tm_1x_cp_create_msi_resp
(
  is801_rev_link_resp_s_type *resp_ptr,
  tm_1x_cp_fl_req_type        fl_req,
  byte                        pde_version
);

/*===========================================================================
FUNCTION i_tm_1x_cp_reject_is801_1_requests

DESCRIPTION
  This function is used to queue IS801-1 reject responses for PDE. The return
  value is true if any IS801 response needs to be sent to the PDE.

  An MSI req is not rejected, and an MSI response is prepared instead.
  
DEPENDENCIES
  
RETURN VALUE  
TRUE:  Some response needs to be sent to PDE
FALSE: No response needs to be sent to PDE


SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_reject_is801_1_requests
(
  is801_fwd_link_req_mask_type     is801_1_v1_rej_req,
  is801_rev_link_resp_s_type      *resp_ptr
);

/*===========================================================================
FUNCTION i_tm_1x_cp_reject_is801b_requests

DESCRIPTION
  This function is used to queue IS801B reject responses for PDE. The return
  value is true if any IS801b Req needs to be rejected. An MSI req however,
  is not rejected
  
ARGUMENTS
  req_mask   : Requests to be rejected
  resp_mask  : Response mask to be used (Reject/Adv Reject) when sending the
               reject response
DEPENDENCIES
  
RETURN VALUE  
TRUE:  Some response needs to be sent to PDE
FALSE: No response needs to be sent to PDE
SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_reject_is801b_requests
(
  is801B_fwd_link_req_mask_type    req_mask,
  is801B_rev_link_resp_mask_type   resp_mask
);

/*===========================================================================

FUNCTION i_tm_1x_cp_create_is801_1_cack_resp

DESCRIPTION: Create an IS801-1 cancellation acknowledgement response

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_create_is801_1_cack_resp
(
  uint8    cancel_no_out_req,
  byte     cancel_type,
  is801_rev_link_resp_s_type *resp_ptr
);

/*===========================================================================

FUNCTION i_tm_1x_cp_create_is801_b_cack_resp

DESCRIPTION: Create an IS801-B cancellation acknowledgement response

DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If successful
  FALSE:  If unsuccessful

SIDE EFFECTS

===========================================================================*/
boolean i_tm_1x_cp_create_is801_b_cack_resp
(
  uint8                            cancel_no_out_req,
  is801B_rev_link_resp_mask_type   cack_resp_type,
  is801B_fwd_link_req_mask_type    cancelled_req_type,
  u_new_rev_link_resp_params      *is801b_resp_parm_ptr
);

/*===========================================================================

FUNCTION tm_1x_cp_session_active

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean tm_1x_cp_session_active(void);

/*===========================================================================

FUNCTION tm_1x_cp_xlate_is801_1_rlink_req_to_1x

DESCRIPTION: Translate IS801-1 reverse link req reject type, to the 1xCP rlink
             request that it corresponded to.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
tm_1x_cp_rlink_req_type tm_1x_cp_xlate_is801_1_rlink_req_to_1x
(
  ms_to_pde_req_type rejected_req_type
);

/*===========================================================================

FUNCTION tm_1x_cp_xlate_is801_b_rlink_req_to_1x

DESCRIPTION: Translate IS801-B fwd link resp reject type, to the 1xCP rlink
             request that it corresponded to.

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
tm_1x_cp_rlink_req_type tm_1x_cp_xlate_is801_b_rlink_req_to_1x
(
  is801B_fwd_link_resp_mask_type reject_resp
);


is801_rev_link_req_mask_type  xlate_1x_cp_rl_req_to_is801_1
(
  tm_1x_cp_rlink_req_type req
);

is801B_rev_link_req_mask_type xlate_1x_cp_rl_req_to_is801_B
(
  tm_1x_cp_rlink_req_type
);

/*===========================================================================

FUNCTION xlate_fl_req_mask_is801_b_to_1x_cp

DESCRIPTION: Translate from Is801-B fwd link request type to generic 1x CP
             request type. Only those req that are used by the 1xCP state 
             machine are translated, rest are not.

             The reject bit mask is returned in the pointers passed as input.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_req_type xlate_fl_req_mask_is801_b_to_1x_cp
(  
  is801B_fwd_link_req_mask_type *req_mask_v1,
  is801B_fwd_link_req_mask_type *req_mask_v3
);

/*===========================================================================

FUNCTION xlate_fl_req_mask_is801_1_to_1x_cp

DESCRIPTION: Translate from Is801-1 fwd link request type to generic 1x CP
             request type. Only those req that are used by the 1xCP state 
             machine are translated, rest are not.

             The reject bit mask is returned in the pointers passed as input.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_req_type xlate_fl_req_mask_is801_1_to_1x_cp
(  
  is801_fwd_link_req_mask_type *req_mask
);

/*===========================================================================

FUNCTION xlate_fl_req_mask_is801_1_to_1x_cp

DESCRIPTION: Translate from Is801-1 cancellation message request type to 
             generic 1x CP request type. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_req_type xlate_cancel_req_is801_1_to_1x_cp
(
  pde_to_ms_req_type            cancel_type
);

/*===========================================================================

FUNCTION xlate_cancel_req_is801_b_to_1x_cp

DESCRIPTION: Translate from Is801-B cancellation message request type to 
             generic 1x CP request type. 

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_req_type xlate_cancel_req_is801_b_to_1x_cp
(
  is801B_fwd_link_req_mask_type  cancel_type
);

/*===========================================================================

FUNCTION i_tm_1x_cp_post_aa

DESCRIPTION: post 801-1 gps aa to mgp.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void i_tm_1x_cp_post_aa
(
  tm_post_data_payload_type   *post_data_ptr,
  is801_fwd_link_resp_s_type  *is801_1_aa,
  is801b_flink_ie_gnss_aa     *is801_b_aa,
  gps_PreFixPpmSructType      *cp_prefix_ppm_buff_ptr_ptr
);

/*===========================================================================

FUNCTION xlate_fl_resp_mask_is801_b_to_1x_cp

DESCRIPTION: Translate from Is801-B fwd link response type to generic 1x CP
             response type.

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
tm_1x_cp_fl_resp_type xlate_fl_resp_mask_is801_b_to_1x_cp
(  
  is801B_fwd_link_resp_mask_type resp_mask
);

/*===========================================================================

FUNCTION i_tm_1x_cp_send_rlink_pddm

DESCRIPTION

RETURN VALUE
  boolean

DEPENDENCIES

===========================================================================*/
boolean i_tm_1x_cp_send_rlink_pddm
(
  byte sess_end,
  byte pde_version
);

/********************************************************************
*
* tm_1x_cp_send_pd_comm_event
*
* Function description:
*   Send pdapi comm events through Tmcore
*
* Parameters: 
*
*   comm_event: part of pdsm_pd_event_type that are relevant to
*               comm.
*
* Return:
*  None
*  
*********************************************************************
*/

void tm_1x_cp_send_pd_comm_event 
(  
   pdsm_pd_event_type      comm_event
);
#endif /* TM_1X_CP_INTERNAL_H */
