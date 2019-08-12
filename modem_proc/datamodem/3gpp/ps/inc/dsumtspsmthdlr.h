#ifndef DSUMTSPSMTHDLR_H
#define DSUMTSPSMTHDLR_H
/*===========================================================================

   U M T S (W C D M A / G P R S )   P A C K E T - S W I T C H E D  
    
                    M O B I L E    T E R M I N A T E D 

                  M O D E - S P E C I F I C   H A N D L E R

                           H E A D E R   F I L E


DESCRIPTION
  This file contains data declarations and function prototypes for UMTS
  (WCDMA/GPRS) Packet-Switched Mobile Terminated Data Mode-Specific Handler 
  which are common for wcdma and gprs.


Copyright (c) 2004 - 2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/inc/dsumtspsmthdlr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/09/09    sa     AU level CMI modifications.
06/09/06    vk     A round of Lint fixes.
10/21/04    mv     Created the file.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_3GPP
#include "dsumts_rmsmi.h"
#include "dss_iface_ioctl.h"
#include "ps_acl.h"
#include "cm.h"
#include "dstask_v.h"
#include "dsumts_rmsm.h"
#include "dsumts_rmsm.h"
#include "ds_3gpp_pdn_context.h"

#define DSUMTSPS_MT_MAX_CB 32       /* Maximum no. of callbacks that can be
                                        store in the MT table              */

#define DSUMTSPS_MT_INVALID_TBL_INDEX   DSUMTSPS_MT_MAX_CB
                                    /* Constant to identify invalid MT INDEX */

#define DS_UMTS_MT_PDP_TYPE_INVALID  0xFF /* Const to indicate invalid 
                                             PDP type                      */

/*===========================================================================

                      PUBLIC DATA/FUNC DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DATA_TE_MT_PDP

typedef enum
{
  MT_UMSM_INVALID_EV           = 0,  /* invalid event */
  MT_UMSM_CALL_RING_EV         = 1,  /* event to ring Rm */
  MT_UMSM_CALL_REJECT_EV       = 2,  /* event to reject MT */
  MT_UMSM_CALL_ANS_TIMEOUT_EV  = 3,  /* event posted on MT ans time-out */
  MT_UMSM_MAX_EV               = 4   /* max event */
}dsumts_mt_umsm_ev_type;


/*===========================================================================
FUNCTION dsumtsps_mt_um_table_store_info

DESCRIPTION
  This is used to post event to MT-UMSM 

PARAMETERS
  mt_index: index of MT TIMER TBL
 *cmd_ptr: CM provided CMD ptr
  cm_call_id: CM call id

 
DEPENDENCIES
  None.

RETURN VALUE
  none

RESTRICTION
  Must be executed in DS Task context

SIDE EFFECTS

===========================================================================*/
void dsumtsps_mt_umsm_post_ev
(
  uint8 mt_index,
  dsumts_mt_umsm_ev_type event,
  void *event_data
);


boolean dsumtsps_mt_um_get_info
(
  uint8 mt_index,
  dsumts_rmsm_mt_info_type *mt_info_ptr
);

void dsumtsps_mt_umsm_init_state
(
  void
);

void dsumtsps_mt_handle_te_ans_timeout
(
  uint32 cb_param
);

/*===========================================================================

FUNCTION dsumtsps_mt_umsm_clean_up_state

DESCRIPTION
  Cleanup state information from MT TIMER TABLE

PARAMETERS
  mt_index - index to MT TIMER TABLE

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

COMMENTS
  This routine is internal to DSUMTSPS MT HDLR

===========================================================================*/
void dsumtsps_mt_umsm_clean_up_state
(
  uint32 mt_index
);

#endif /*FEATURE_DATA_TE_MT_PDP*/

/*===========================================================================

FUNCTION DSUMTSPS_MT_REGISTER_CALLBACK

DESCRIPTION
  Called from application context when an app registers for a MT request.

PARAMETERS
  * callback_info - This contains the callback name, profile number
                    and a pointer to the handle for the callback.The value
                    of the handle is assigned by this function and returned  
                    to the app. 
                    The app uses the handle to de-register the callback.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS


===========================================================================*/
int dsumtsps_mt_register_callback
(
  ps_iface_ioctl_mt_reg_cb_type   *callback_info,
  ps_iface_type                   *iface_ptr,
  sint15                          *ps_errno
);

/*===========================================================================
FUNCTION DSUMTSPS_MT_DEREGISTER_CALLBACK

DESCRIPTION
  Called from application context when an app de-registers for a MT request.

PARAMETERS
  * callback_info - This contains the callback name and the handle returned
                    to the app when the callback was registered. The callback
                    is removed from the list of stored callbacks for 
                    MT request.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS


===========================================================================*/
int dsumtsps_mt_deregister_callback
(
  const ps_iface_ioctl_mt_dereg_cb_type *callback_info,
  sint15                                *ps_errno
);

/*===========================================================================
FUNCTION DSUMTSPS_MT_REG_TABLE_MATCH_PROFILE

DESCRIPTION

  Called from DS context when a MT request is received. This function
  matches the incoming profile info such as APN and PDP type to that
  stored in the callback table. If a match occurs, a callback is sent
  to the app and a TRUE is returned else a FALSE is returned and a 
  reject is sent.

PARAMETERS
  * cmd_ptr -    This contains the info such as apn name, pdp_type
    call_id -    This contains the call_id of the incoming call. This
                 is stored in the iface table along with iface timer.

DEPENDENCIES
  None.

RETURN VALUE
  True or False

SIDE EFFECTS


===========================================================================*/
boolean dsumtsps_mt_reg_table_match_emb_profile
(
  const ds_cmd_type *cmd_ptr,
  cm_call_id_type    call_id
);


/*===========================================================================
FUNCTION dsumtsps_mt_um_table_store_info

DESCRIPTION
  This is used to post event to MT-UMSM 

PARAMETERS
  mt_index: index of MT TIMER TBL
 *cmd_ptr: CM provided CMD ptr
  cm_call_id: CM call id

 
DEPENDENCIES
  None.

RETURN VALUE
  none

RESTRICTION
  Must be executed in DS Task context

SIDE EFFECTS

===========================================================================*/
boolean dsumtsps_mt_um_table_store_info
(
  const ds_cmd_type *cmd_ptr,
  cm_call_id_type    call_id,
  uint8 *mt_index
);

/*===========================================================================
FUNCTION DSUMTSPS_MT_CHECK_CALL_ORIG

DESCRIPTION
 This function checks whether a call origination is due to a MT request that
 was received. It matches the APN name and PDP type of the originating call
 with that of any MT request that was received and fills in the call id
 if a match occurs.

PARAMETERS
    call_id -    This contains the call_id of the incoming call that
                 was stored in the MT table.

DEPENDENCIES
  None.

RETURN VALUE
 
  Call ID of the MT request.

SIDE EFFECTS


===========================================================================*/
cm_call_id_type dsumtsps_mt_check_call_orig 
(
  ds_pdn_context_s    *pdn_cntx_p
);

/*===========================================================================
FUNCTION DSUMTSPS_MT_SEND_REJECT

DESCRIPTION
  This function sends a reject for a MT call when the MT timer expires but
  the app has not originated a call.
  
PARAMETERS
  index -    This is the index to the MT table that stores the timer and 
             other info like APN, PDP type, call_id etc.
                 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void dsumtsps_mt_send_reject
(
  uint8 index
);


/*===========================================================================

FUNCTION DSUMTSPS_MT_HANDLE_TIMER_CB

DESCRIPTION
  This posts a command to DS task to send a MT reject response
  to dsmgr.

PARAMETERS
  cb_param - This contains the index to the mt table that the timer 
             running for that callback.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void dsumtsps_mt_handle_timer_cb
(
  uint32 cb_param
);

/*===========================================================================

FUNCTION DSUMTSPS_MT_CVT_NAS_PDP_TYPE_TO_DS_FMT

DESCRIPTION
  Converts NAS PDP TYPE value to DS PDP TYPE.

PARAMETERS
  nas_pdp_type - This is NAS PDP TYPE.

DEPENDENCIES
  None.

RETURN VALUE
  ds_umts_pdp_type_enum_type - PDP type in DS format.

SIDE EFFECTS
  None.

===========================================================================*/
ds_umts_pdp_type_enum_type dsumtsps_mt_cvt_nas_pdp_type_to_ds_fmt
(
    uint8 nas_pdp_type
);

#endif
#endif /* FEATURE_DATA_3GPP */
