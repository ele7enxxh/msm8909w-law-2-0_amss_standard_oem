/*==============================================================================

          e M B M S D A T A - S P E C I F I C   H A N D L E R 


                        DS_3GPP_EMBMS_HDLR . C
GENERAL DESCRIPTION
  This file is the controller for eMBMS Data functionality. 

  This modules  creates and initializes broadcast ps_iface and the associated
  initialization like registering the function handlers for eMBMS commands.
  And contains code for eMBMS specific funciton handlers. 

  This module does not know who the clients are. The initial implementation
  only deals with delivering packets to the SIO interface. Packet filtering
  has not been considered in the design although the design is open for 
  allowing socket clients to install filters to get packets up the stack.

EXTERNALIZED FUNCTIONS

  DS_3GPP_EMBMS_INIT
    Called during power up initialization of the UE.

  DS_3GPP_EMBMS_PROCESS_CMDS
    Processes commands from PS for eMBMS iface control.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  dsumts_mbms_init() must be called at startup.

  Copyright (c) 2011 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/embms/src/ds_3gpp_embms_hdlr.c#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/15/11    hs     Created file/Initial version.
==============================================================================*/

/*==============================================================================
                          INCLUDE FILES FOR MODULE
==============================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_EMBMS

#include "ds_3gpp_embms_hdlr.h"
#include "ds_3gpp_embms_rrcif.h"
#include "ds_3gpp_embms_rt_acl.h"
#include "ds_3gpp_embms_tmgi_tbl_mgr.h"

#include "ds_3gppi_utils.h"
#include "modem_mem.h"
#include "dstask.h"
#include "ps_iface.h"
#include "ps_iface_embms.h"
#include "ps_iface_ioctl.h"
#include "ps_iface_flow.h"
#include "ps_meta_info.h"
#include "ps_acl.h"
#include "list.h"
#include "ds_eps_tlb.h"
#include "lte_rlc_msg.h"
#include "dsmsgr.h"
#include "ps_dpm.h"

#include "ps_iface_logging.h"
#include "ps_logging.h"
#include "ps_logging_defs.h"


#include "ds_flow_control.h"
#include "ds3gsubsmgr.h"
#include "ds3gflowmgr.h"

#include <string.h>

/*==============================================================================
                       INTERNAL VARIABLE DEFINITIONS AND TYPES
==============================================================================*/

#define DS_3GPP_EMBMS_MAX_PHYS_LINKS 1

/*------------------------------------------------------------------------------
  Defines the maximum number of EMBMS Control Block Instances that can be 
  allocated.
------------------------------------------------------------------------------*/

#define DS_3GPP_EMBMS_MAX_CTRL_BLK_INSTANCES 1

/*------------------------------------------------------------------------------
  Defines the value of an invalid EMBMS control block instance
------------------------------------------------------------------------------*/
#define DS_3GPP_EMBMS_INVALID_CTRL_BLK_INSTANCE DS_3GPP_EMBMS_MAX_CTRL_BLK_INSTANCES

/*------------------------------------------------------------------------------
  Array of pointers to the eMBMS Control blocks. 
  An EMBMS Control Blk will contain the Iface, the physlink, watermarks
  and other associated control structures that are needed for eMBMS
------------------------------------------------------------------------------*/
static ds_3gpp_embms_ctrl_blk_type 
         *ds_3gpp_embms_ctrl_block_p[DS_3GPP_EMBMS_MAX_CTRL_BLK_INSTANCES] = 
            {NULL};

/*------------------------------------------------------------------------------
  This is a map between the Subs Id and EMBMS Control Block Index.
------------------------------------------------------------------------------*/

typedef struct
{
  boolean init;
  int     ctrl_blk_index;
} ds_3gpp_embms_subs_info_type;

static ds_3gpp_embms_subs_info_type
         ds_3gpp_embms_subs_id_map[DS3GSUBSMGR_SUBS_ID_MAX] = {{0}};

/*-----------------------------------------------------------------------------
  Critical section to protect the TMGI management table
-----------------------------------------------------------------------------*/
static rex_crit_sect_type ds_3gpp_embms_crit_sect;

/*-----------------------------------------------------------------------------
  The client that has requested available TMGI list. Currently this operation
  is supported for only one client at a time. If there is a need, this will
  expand into a list of clients and when the response from RRC reaches DS,
  Iface indications will be sent out to all clients in this list.
-----------------------------------------------------------------------------*/
static void* ds_3gpp_embms_avail_list_client = NULL;

/*-----------------------------------------------------------------------------
  The client that has requested SAI list. Currently this operation
  is supported for only one client at a time. If there is a need, this will
  expand into a list of clients and when the response from RRC reaches DS,
  Iface indications will be sent out to all clients in this list.
-----------------------------------------------------------------------------*/
static void* ds_3gpp_embms_sai_list_client = NULL;

/*==============================================================================
                       FUNCTION PROTOTYPES
==============================================================================*/

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_IFACE_BRING_UP

DESCRIPTION   Called when eMBMS is enabled. This posts a command to DS task
              to bring up the interface on the modem.

DEPENDENCIES  None

RETURN VALUE  Integer.

SIDE EFFECTS  None.
==============================================================================*/
LOCAL int ds_3gpp_embms_iface_bring_up
( 
 ps_iface_type    *iface_ptr,
 void             *not_used
);

/*==============================================================================
FUNCTION      ds_3gpp_embms_modeC_rx_um_data_cb

DESCRIPTION   This function is used to close TLB mode C and return an boolean.

DEPENDENCIES  None

RETURN VALUE  Integer.

SIDE EFFECTS  None.
==============================================================================*/

boolean ds_3gpp_embms_modeC_rx_um_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
);

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_IFACE_BRING_UP_CMD_HDLR

DESCRIPTION   cmd posted when DS_3GPP_EMBMS_IFACE_BRING_UP() function 
              corresponding to embms ps_iface is called. This is typically
              called when eMBMS is enabled. This function brings up the iface
              and the phys link. Since no OTA signaling is involved, it moves
              the state of these call objects. 

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  If the iface is already UP, iface_up indication is posted. Note
              that its important to post iface_up indication, even though the
              state is already UP as the application clients must be blocked
              on the IFACE_UP event.
==============================================================================*/
LOCAL void ds_3gpp_embms_iface_bring_up_cmd_hdlr
(
  sys_modem_as_id_e_type *subs_id_p
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_CLEAN_UP_RX_WM

DESCRIPTION
  This function is used to flush out any data that may be present in the WM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
LOCAL void ds_3gpp_embms_clean_up_rx_wm
(
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p
);

/*==============================================================================
FUNCTION DS_3GPP_EMBMS_TX_UM_DATA_CB

DESCRIPTION
  This function handles any packet to be transmitted over the air in the UL
  direction. There shoud be no uplink traffic. If any reaches the modem in 
  this direction, it will be dropped at this point.
  
PARAMETERS  
  phys_link_ptr - pointer to the phys link on which this is to be sent
  item_ptr      - pointer to the packet to be transmitted
  meta_info_ptr - pointer to the meta info blob
  tx_info_ptr   - user data
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
   
SIDE EFFECTS 
  None.
  
==============================================================================*/
LOCAL void ds_3gpp_embms_tx_um_data_cb
(
  ps_phys_link_type *phys_link_ptr,
  dsm_item_type     **item_ptr,
  ps_meta_info_type *meta_info_ptr,
  void              *tx_info_ptr
);

/*==============================================================================
FUNCTION DS_3GPP_EMBMS_RX_UM_DATA_CB

DESCRIPTION
  This function handles any packet that has been received over the air in the DL
  direction. There shoud be no uplink traffic. If any reaches the modem in 
  this direction, it will be dropped at this point.
  
PARAMETERS  
  sig             - Signal to process
  user_data_ptr   - user data
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  FALSE if there is more data in the WM
  TRUE if there is no more data
   
SIDE EFFECTS 
  None.
  
==============================================================================*/
LOCAL boolean ds_3gpp_embms_rx_um_data_cb
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void             *user_data_ptr          /* Parameter carrying user data */
);

/*==============================================================================
FUNCTION DS_3GPP_EMBMS_IOCTL_HDLR()

DESCRIPTION
  This function is called to handle the IOCTL requests
  PS_IFACE_IOCTL_EMBMS_ACTIVATE_TMGI
  PS_IFACE_IOCTL_EMBMS_DEACTIVATE_TMGI
  PS_IFACE_IOCTL_EMBMS_GET_ACTIVE_TMGI_LIST
  PS_IFACE_IOCTL_EMBMS_GET_AVAIL_TMGI_LIST

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL int ds_3gpp_embms_ioctl_hdlr
( 
  ps_iface_type       *iface_ptr,       /* PS IFACE for the eMBMS iface */
  ps_iface_ioctl_type ioctl_name,       /* IOCTL Name                   */
  void                *argval_ptr,      /* IOCTL Params                 */  
  sint15              *ps_errno         /* Errno Value                  */  
);

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_ACTIVATE_TMGI_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to activate a TMGI. Based on the TMGI
  state machine a request is sent to LTE RRC. It is possible that we need not go
  to RRC to service the request. DS may also reject the IOCTL in cases where it
  is not possible to activate a TMGI

  Note: 
  1. Currently the UE supports only 8 concurrent TMGI activation. DS does not
  block requests after 8 TMGIs are active. It will be passed on to LTE RRC which
  may choose to bring down a TMGI and activate the new one. There is also no
  dependency in the order in which the deactivation of an existing TMGI and 
  activation of a new TMGI happens.

  2. There is a limit to the number of TMGI state machines that can be created
  This is a limit in DS and is set to 16. Please look at the declaration of 
  ds_3gpp_embms_context_table to understand this rationale


DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_activate_tmgi_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_DEACTIVATE_TMGI_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to deactivate a TMGI. Based on the TMGI
  state machine a request is sent to LTE RRC. It is possible that we need not go
  to RRC to service the request. DS may also reject the IOCTL in cases where it
  is not possible to deactivate a TMGI


DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_deactivate_tmgi_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GET_ACTIVE_TMGI_LIST_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to retrieve the list of TMGIs that have
  been activated. This function simply loops through the DS TMGI table and
  returns the TMGIs that are in active state.
 
  In this function we will get the critical section that protects access
  to the TMGI table. As a result, only one client's IOCTL invocation can
  be processed at a time.

DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_get_active_tmgi_list_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GET_AVAIL_TMGI_LIST_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to retrieve the list of TMGIs that are avail
  to the user. To get this information, we always go to RRC module.
 
  There is always a possibility that another client may request the available
  list while we are waiting for RRC to respond. We simply add client info to a
  list and respond to the client when RRC responds. We do not go to RRC with a
  second request while the first one is pending.

DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_get_avail_tmgi_list_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GET_SAI_LIST_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to retrieve the list of SAIs.
  To get this information, we always go to RRC module.
 
  Currently this operation is supported for only one client at a time.
  If there is a need, this will need to be expanded for multiple clients 
  and when the response from RRC reaches DS, Iface indications will be 
  sent out to all clients in this list.

DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_get_sai_list_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GENERATE_EVENTS_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to retrieve the list of all available events
  from RRC for which QMI will to broadcast to all the clients
  This IOCTL Handler is invoked soon after EMBMS iface is brought up

DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_generate_events_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_ACT_DEACT_TMGI_IOCTL_HDLR

DESCRIPTION
  This function is used to simultaneously activate/deactivate TMGIs. TMGI to be
  activated must not have been activated before. TMGI to be deactivated must be
  activated before. 
  A message router packet is populated and sent to LTE RRC.

PARAMETERS
  tmgi_req_ptr - Pointer to the TMGI 

DEPENDENCIES  
  None

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL int ds_3gpp_embms_act_deact_tmgi_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_CONTENT_DESC_UPDATE_IOCTL_HDLR

DESCRIPTION
  This function is used to pass the content descovery update information from
  AP to LTE RLC

DEPENDENCIES  
  None

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL int ds_3gpp_embms_content_desc_update_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND_CMD_HDLR

DESCRIPTION
  This function is used to send the indication to the specified client that a 
  TMGI has been activated.

  This function is solely used in the processing of 
  DS_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND_CMD which is posted when we decide to 
  send an indication to the client requesting activation since the TMGI is
  already active.

PARAMETERS
  ind_info_ptr  - information that will be sent in the Iface indication.
  user_data_ptr - call back information (used to get the index into the TMGI
                  context table) 

DEPENDENCIES  
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
==============================================================================*/
LOCAL void ds_3gpp_embms_send_tmgi_activated_ind_cmd_hdlr
(
  ps_iface_embms_tmgi_activation_info_type   *ind_info_ptr,
  uint8                                      incoming_index,
  sys_modem_as_id_e_type                     subs_id
);

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND

DESCRIPTION
  This function is used to send the indication to the specified client that a 
  TMGI has been deactivated.

  This function is solely used in the processing of 
  DS_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND_CMD which is posted when we decide to 
  send an indication to the client requesting deactivation since the TMGI is
  currently active with more than one client. The command is needed to ensure
  that we post indication after we return from the IOCTL.

PARAMETERS
  ind_info_ptr  - information that will be sent in the Iface indication.

DEPENDENCIES  
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
==============================================================================*/
LOCAL void ds_3gpp_embms_send_tmgi_deactivated_ind
(
  ps_iface_embms_tmgi_deactivation_info_type *ind_info_ptr,
  sys_modem_as_id_e_type                      subs_id
);

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_FIND_OR_ALLOCATE_CTRL_BLK

DESCRIPTION
  This function is used to find the Control Block corresponding to passed
  subs_id. If no control block is found, allocate a new Control Block and
  return a pointer to that.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES  
  None

RETURN VALUE  
  Pointer to the EMBMS Control Block.

SIDE EFFECTS
  None
==============================================================================*/
static ds_3gpp_embms_ctrl_blk_type* ds_3gpp_embms_find_or_allocate_ctrl_blk
(
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================
FUNCTION DS_3GPP_GET_CTRL_BLK_INDEX

DESCRIPTION
  This functions fetches the Ctrl Blk index corresponding to the given Subs Id.
 
PARAMETERS 
  Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Ctrl Block Index

SIDE EFFECTS
  None.
==============================================================================*/
static int ds_3gpp_embms_get_ctrl_blk_index
(
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================
FUNCTION DS_3GPP_SET_CTRL_BLK_INDEX

DESCRIPTION
  This functions maps the Subs Id and Ctrl Blk Index
 
PARAMETERS 
  EMBMS Ctrl Block Index
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
static void ds_3gpp_embms_set_ctrl_blk_index
(
  int                    index,
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================
FUNCTION DS_3GPP_VALIDATE_CTRL_BLK_INDEX

DESCRIPTION
  This functions checks whether the given index into the ds_3gpp_embms_subs_id_map
  is valid.
 
PARAMETERS 
  Index 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
static boolean ds_3gpp_embms_validate_ctrl_blk_index
(
  uint8 index
);

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_SET_CTRL_BLK

DESCRIPTION
  This function is used to set the EMBMS Control Block.

PARAMETERS 
  ctrl_blk_p - Pointer to EMBMS Ctrl Blk
  
DEPENDENCIES  
  None

RETURN VALUE
  Index to Ctrl Blk

SIDE EFFECTS  
  None
==============================================================================*/
static int ds_3gpp_embms_set_ctrl_blk
(
  ds_3gpp_embms_ctrl_blk_type  *ctrl_blk_p
);

/*==============================================================================
                       FUNCTION DEFINITIONS
==============================================================================*/

/*==============================================================================
FUNCTION DS_3GPP_EMBMS_RLC_TO_PS_DATA_CB()

DESCRIPTION
  This is the Watermark non-empty call back funtion. This function should
  only set a signal to PS task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL void ds_3gpp_embms_rlc_to_ps_data_cb
(
  dsm_watermark_type *wm,            /* Registered watermark                  */
  void               *callback_data  /* Callback data registered with WM      */
)
{
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = 
    (ds_3gpp_embms_ctrl_blk_type*)callback_data;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Set Rx Signal. As a part of processing this, the eMBMS signal handler 
    will be called.
  ----------------------------------------------------------------------------*/
  PS_SET_SIGNAL(ctrl_blk_p->rx_sig);

  return;
} /* ds_3gpp_embms_rlc_to_ps_data_cb() */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_INITIALIZE_RLC_TO_PS_WM

DESCRIPTION
  This function is used to set up the Air link receive watermarks. It sets the
  Hi, Lo and DNE values on the WM and also registers a non-empty call back that
  will be called when data is enqueued into this WM.

PARAMETERS 
  ctrl_blk_p - Pointer to the EMBMS Control Block

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_3gpp_embms_initialize_rlc_to_ps_wm
(
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p
)
{
  dsm_watermark_type* wm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Get the watermark pointer for the DL data
  ----------------------------------------------------------------------------*/
  wm_ptr = &(ctrl_blk_p->rx_wm.wm_item);

  dsm_set_low_wm(wm_ptr, DS_3GPP_EMBMS_DL_WM_DEFAULT_LO);
  dsm_set_hi_wm(wm_ptr, DS_3GPP_EMBMS_DL_WM_DEFAULT_HI);
  dsm_set_dne(wm_ptr, DS_3GPP_EMBMS_DL_WM_DEFAULT_DNE);


  /*-----------------------------------------------------------------------
    Register a non empty function pointer call back so that we can set the
    PS signal when data is enqueued into this watermark.
  -----------------------------------------------------------------------*/ 

  wm_ptr->non_empty_func_ptr = (wm_cb_type)ds_3gpp_embms_rlc_to_ps_data_cb;

  wm_ptr->non_empty_func_data = (void*)ctrl_blk_p;

} /* ds_3gpp_embms_initialize_rlc_to_ps_wm() */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_REG_WM_WITH_DPM

DESCRIPTION   This function is called to register the WM with DPM

PARAMETERS    ctrl_blk_p - Pointer to the EMBMS Control Block            

RETURN VALUE  TRUE: Processing Successful 
              FALSE: Processing Error 

SIDE EFFECTS  None
==============================================================================*/
boolean ds_3gpp_embms_reg_wm_with_dpm
(
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p
)
{
  ps_dpm_um_reg_bearer_info_type          *reg_info = NULL;
  int32                                    ps_dpm_reg_ret = -1;
  boolean                                  ret_val = FALSE;
/*---------------------------------------------------------------------------*/

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return FALSE;
  }

  if(ctrl_blk_p->is_registered_with_dpm == FALSE)
  {
    reg_info = (ps_dpm_um_reg_bearer_info_type *)modem_mem_alloc 
                (sizeof(ps_dpm_um_reg_bearer_info_type), MODEM_MEM_CLIENT_DATA);

    /*---------------------------------------------------------------------- 
      In case of Memory allocation/DPM Registration failure we take the SW path
      instead of HW path
    -----------------------------------------------------------------------*/

    if (reg_info == NULL)
    {
      DS_EMBMS_MSG0_ERROR ("Memory allocation failure while registering with"
                           " DPM, Taking SW Path instead");
      ds_3gpp_embms_initialize_rlc_to_ps_wm(ctrl_blk_p);
      return ret_val;
    }
    /*---------------------------------------------------------------------- 
      We provide a dummy bearer ID for EMBMS, bearer id field removed on
      DPM re-arch
    -----------------------------------------------------------------------*/

    memset (reg_info, 0, sizeof(ps_dpm_um_reg_bearer_info_type));
    reg_info->ps_phys_link_ptr = &(ctrl_blk_p->phys_link);
    reg_info->l2_to_ps_wm_ptr  = &(ctrl_blk_p->rx_wm.wm_item);
    reg_info->v4_iface_ptr = &(ctrl_blk_p->ps_iface);
    reg_info->v6_iface_ptr = &(ctrl_blk_p->ps_iface);
    reg_info->rat = PS_SYS_RAT_EX_3GPP_LTE;

    ps_dpm_reg_ret = ps_dpm_um_reg_bearer(reg_info);
    if (ps_dpm_reg_ret == DSS_SUCCESS)
    {
      DS_EMBMS_MSG0_MED("Dpm Registration Succesful for EMBMS IFACE");
      ctrl_blk_p->is_registered_with_dpm = TRUE;
      ret_val = TRUE;
    }
    else
    {
      DS_EMBMS_MSG0_ERROR("Dpm Registration failed for EMBMS IFACE, taking"
                          " the SW Path");
      ds_3gpp_embms_initialize_rlc_to_ps_wm(ctrl_blk_p);
    }
    modem_mem_free ((void *)reg_info, MODEM_MEM_CLIENT_DATA);
  }
  else
  {
    DS_EMBMS_MSG0_HIGH ("WM is already registered with DPM");
  }
  return ret_val;
} /* ds_3gpp_embms_reg_wm_with_dpm */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_WM_REG_CNF_HDLR

DESCRIPTION   This function is called when EMBMS WM registration is successful 
              at RLC Layer.
              Once the registration is successful we register the WM with DPM

PARAMETERS    MSG Type: Confirmation Message 
              DSMSG: Structure containing confirmation status and its relevant
              info              

RETURN VALUE  TRUE: Processing Successful 
              FALSE: Processing Error 

SIDE EFFECTS  None
==============================================================================*/
boolean ds_3gpp_embms_wm_reg_cnf_hdlr
(
  msgr_umid_type                           msgrtype,
  const msgr_hdr_struct_type              *dsmsg
)
{
  boolean                                  ret_val = FALSE;
  sys_modem_as_id_e_type                   subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_embms_ctrl_blk_type             *ctrl_blk_p = NULL;
/*----------------------------------------------------------------------------*/

  if (msgrtype != LTE_RLCDL_EMBMS_REG_CNF || dsmsg == NULL)
  {
    DS_EMBMS_MSG0_ERROR ("Invalid Input Param");
    return ret_val;
  }

  /*---------------------------------------------------------------------- 
    If the WM Reg with RLC succeeds then conf status will be E_SUCCESS
   
    Now if we are TLB Mode C we dont register with DPM
        Else we register with DPM to complete the bridge
  -----------------------------------------------------------------------*/
  if (((lte_rlcdl_embms_wm_register_cnf_msg_s *)dsmsg)->cnf_status == E_SUCCESS)
  {
    subs_id = INST_ID_TO_SYS_AS_ID
              (((lte_rlcdl_embms_wm_register_cnf_msg_s*)dsmsg)->hdr.inst_id);

    ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

    if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
    {
      return ret_val;
    }

    ctrl_blk_p->l2_wm_set_up = TRUE;
    if (dsEpsTlb_is_tlb_in_mode_C(subs_id) == TRUE)
    {
      DS_EMBMS_MSG0_HIGH ("EMBMS in Test Loopback Mode, No Reg performed with "
                          "DPM use SW path");
      ds_3gpp_embms_initialize_rlc_to_ps_wm(ctrl_blk_p);
      ret_val = TRUE;
    }
    else
    {
      ret_val = ds_3gpp_embms_reg_wm_with_dpm(ctrl_blk_p);
    }
  }
  else
  {
    DS_EMBMS_MSG0_ERROR ("Watermark Reg with RLC failed, Not Registering with "
                         "DPM");
  }

  return ret_val;
} /* ds_3gpp_embms_wm_reg_cnf_hdlr */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_DEREG_WM_WITH_DPM

DESCRIPTION   This function is called to de-register the WM with DPM

PARAMETERS    ctrl_blk_p - Pointer to the EMBMS Control Block              

RETURN VALUE  TRUE: Processing Successful 
              FALSE: Processing Error 

SIDE EFFECTS  None
==============================================================================*/
boolean ds_3gpp_embms_dereg_wm_with_dpm
(
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p
)
{
  int32                                    ps_dpm_dereg_ret = FALSE;
  ps_dpm_um_dereg_bearer_info_type        *dereg_info = NULL;
  boolean                                  ret_val = FALSE;
/*-----------------------------------------------------------------------------*/

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return FALSE;
  }

  if(ctrl_blk_p->is_registered_with_dpm == TRUE)
  {
    dereg_info = (ps_dpm_um_dereg_bearer_info_type *)modem_mem_alloc 
               (sizeof(ps_dpm_um_dereg_bearer_info_type), MODEM_MEM_CLIENT_DATA);
 
    /*------------------------------------------------------------------------
    In case of De-reg failure instead of an ASSERT, MH cleans
    it state so that during next attempt Registration with DPM does not fail
    -------------------------------------------------------------------------*/
     
    if (dereg_info == NULL)
    {
      DATA_ERR_FATAL("Memory allocation failure while de-registering with DPM");
      return ret_val;
    }

    memset (dereg_info, 0, sizeof(ps_dpm_um_dereg_bearer_info_type));
    
    dereg_info->v4_iface_ptr = &(ctrl_blk_p->ps_iface);
    dereg_info->v6_iface_ptr = &(ctrl_blk_p->ps_iface);
    dereg_info->ps_phys_link_ptr = &(ctrl_blk_p->phys_link);

    ps_dpm_dereg_ret = ps_dpm_um_dereg_bearer (dereg_info);

    ctrl_blk_p->is_registered_with_dpm = FALSE;
    if (ps_dpm_dereg_ret == DSS_SUCCESS)
    {
      DS_EMBMS_MSG0_MED("Dpm De-Registration Succesful for EMBMS IFACE");

      ret_val = TRUE;
    }
    else
    {
      DS_EMBMS_MSG0_ERROR("Dpm De-Registration failed for EMBMS IFACE");
    }
    modem_mem_free ((void *)dereg_info, MODEM_MEM_CLIENT_DATA);
  }
  else
  {
    DS_EMBMS_MSG0_HIGH ("No Registration was performed with DPM, thereby no"
                        " De-Reg required");
  }
  return ret_val;
} /* ds_3gpp_embms_dereg_wm_with_dpm */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_WM_DEREG_CNF_HDLR

DESCRIPTION   This function is called when EMBMS WM de-registration is successful 
              at RLC Layer.
              Once the de-registration is successful we de-register the WM with
              DPM

PARAMETERS    MSG Type: Confirmation Message 
              DSMSG: Structure containing confirmation status and its relevant
              info              

RETURN VALUE  TRUE: Processing Successful 
              FALSE: Processing Error 

SIDE EFFECTS  None
==============================================================================*/

boolean ds_3gpp_embms_wm_dereg_cnf_hdlr
(
  msgr_umid_type                           msgrtype,
  const msgr_hdr_struct_type              *dsmsg
)
{
  boolean                                  ret_val = FALSE;
  sys_modem_as_id_e_type                   subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_embms_ctrl_blk_type             *ctrl_blk_p = NULL;
/*----------------------------------------------------------------------------*/

  if (msgrtype !=  LTE_RLCDL_EMBMS_DEREG_CNF || dsmsg == NULL)
  {
    DS_EMBMS_MSG0_ERROR ("Invalid Input Param");
    return ret_val;
  }
  /*------------------------------------------------------------------------
   After we get a de-reg confirmation we de-register the WM with DPM
  -------------------------------------------------------------------------*/
  if(((lte_rlcdl_embms_wm_deregister_cnf_msg_s *)dsmsg)->cnf_status == E_SUCCESS)
  {
    DS_EMBMS_MSG0_MED ("Watermark De-reg with RLC successful, De-Registering "
                       "with DPM");

    subs_id = INST_ID_TO_SYS_AS_ID
               (((lte_rlcdl_embms_wm_deregister_cnf_msg_s *)dsmsg)->hdr.inst_id);

    ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

    if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
    {
      return ret_val;
    }

    ctrl_blk_p->l2_wm_set_up = FALSE; 
    ret_val = TRUE;   
  }
  else
  {
    DS_EMBMS_MSG0_ERROR ("Watermark De-reg with RLC failed, did not de-register"
                         " with DPM");
  }
  return ret_val;

} /* ds_3gpp_embms_wm_dereg_cnf_hdlr */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_PER_SUBS_INFO_INIT

DESCRIPTION
  This function allocates and initializes one instance of the EMBMS Control BLock.

PARAMETERS
  subs_id - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_per_subs_info_init
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_embms_ctrl_blk_type     *ctrl_blk_p = NULL;
  ps_iface_type                   *iface_ptr = NULL; 
  acl_type                        *my_acl_ptr = NULL;
  int                              ret_val = 0;
  ps_phys_link_type               *phys_link_ptr = NULL;
  ps_sig_enum_type                ps_sigs
                                    [DS_3GPP_EMBMS_MAX_CTRL_BLK_INSTANCES] = 
                                  {
                                    PS_3GPP_EMBMS_RX_Q_SIGNAL
                                  };

  dpl_iid_ifname_enum_type        dpl_iface
                                    [DS_3GPP_EMBMS_MAX_CTRL_BLK_INSTANCES] = 
                                  {
                                    DPL_IID_IFNAME_EMBMS_IFACE
                                  };
  int                             ctrl_blk_index = 0;
  /*---------------------------------------------------------------------------*/

  do
  {
    if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      break;
    }

    if (ds_3gpp_embms_subs_id_map[subs_id].init == TRUE)
    {
      DS_EMBMS_MSG1_ERROR("Subs id(CM) %d already inited", subs_id);
      break;
    }
    ctrl_blk_p = (ds_3gpp_embms_ctrl_blk_type*)
                    modem_mem_alloc (sizeof(ds_3gpp_embms_ctrl_blk_type),
                      MODEM_MEM_CLIENT_DATA);
    if (ctrl_blk_p == NULL)
    {
      DS_3GPP_MSG1_ERROR ("Memory allocation failed for Subs Id (CM): %d", 
                          subs_id );
      break;
    }

    ctrl_blk_index = ds_3gpp_embms_set_ctrl_blk(ctrl_blk_p);

    if (!ds_3gpp_embms_validate_ctrl_blk_index(ctrl_blk_index))
    {
      DS_3GPP_MSG1_ERROR ("invalid Ctrl Blk Index %d returned", 
                          ctrl_blk_index );
      modem_mem_free(ctrl_blk_p, MODEM_MEM_CLIENT_DATA);
      break;
    }

    memset((void*)ctrl_blk_p, 0, sizeof(ds_3gpp_embms_ctrl_blk_type));          

    ctrl_blk_p->subs_id = SYS_MODEM_AS_ID_NONE;
    /*-----------------------------------------------------------------------
      Get the acl_ptr.
    -----------------------------------------------------------------------*/
    my_acl_ptr        = &(ctrl_blk_p->acl);
    my_acl_ptr->acl_fptr = ds_3gpp_embms_rt_acl; 
    my_acl_ptr->post_proc_fptr =  NULL;

    iface_ptr         = &(ctrl_blk_p->ps_iface);
    
    /*-----------------------------------------------------------------------
      Initialize the iface client data pointer to point to the eMBMS ctrl
      block.
    -----------------------------------------------------------------------*/
    iface_ptr->client_data_ptr = (void*)(ctrl_blk_p);

    /*-----------------------------------------------------------------------
      Initialize the PS_IFACE.
    -----------------------------------------------------------------------*/
    ret_val = ps_iface_create(iface_ptr,
                              EMBMS_IFACE,
                              my_acl_ptr, 
                              NULL,
                              &(ctrl_blk_p->phys_link),
                              DS_3GPP_EMBMS_MAX_PHYS_LINKS);

    if (ret_val == -1)
    {
      DATA_ERR_FATAL("DS 3GPP EMBMS Init: Could not create ps_iface");
    }

    /*-------------------------------------------------------------------------
       Set the Iface group mask and set addr family to any addr family
    -------------------------------------------------------------------------*/
    //Hari - is this needed?
    iface_ptr->group_mask = BCAST_MCAST_GROUP;  
  
    /*----------------------------------------------------------------------------
      Unable to set the IP ADD family to UNSPEC - Follow up with PS
    ----------------------------------------------------------------------------*/
    ret_val = ps_iface_set_addr_family(iface_ptr, IFACE_ANY_ADDR_FAMILY);

    if (ret_val == -1)
    {
      DATA_ERR_FATAL("DS 3GPP eMBMS INIT: Could not set ps iface family");
    }

    DS_EMBMS_MSG0_HIGH("DS 3GPP eMBMS Iface Created ");
   
    /*----------------------------------------------------------------------------
      Enable Logging on UMTS interfaces.
    ----------------------------------------------------------------------------*/
   
    ps_iface_dpl_support_network_logging(iface_ptr, dpl_iface[ctrl_blk_index]);
    

    (void) ps_iface_dpl_set_iface_desc(iface_ptr, "EMBMS Iface");


    /*----------------------------------------------------------------------------
      These function pointers are set so that eMBMS Iface related functions can
      happen. 
   
      We do not have a teardown cmd function pointer set because we bring down 
      the interface when LTE RRC declares eMBMS is disabled. This is to ensure 
      that we avoid dropping messages that LTE RRC may give us after we post 
      down on the iface.
    ----------------------------------------------------------------------------*/
    iface_ptr->bring_up_cmd_f_ptr   = ds_3gpp_embms_iface_bring_up;
    iface_ptr->tear_down_cmd_f_ptr  = NULL;
   
    /*----------------------------------------------------------------------------
      Register the IOCTL handler. This will handle all requests coming from the
      higher layers (clients)
    ----------------------------------------------------------------------------*/
    iface_ptr->iface_ioctl_f_ptr = ds_3gpp_embms_ioctl_hdlr; 
 
    /*----------------------------------------------------------------------------
      Enable flow on the interface.
    ----------------------------------------------------------------------------*/
    ps_iface_enable_ind(iface_ptr);
   
    /*----------------------------------------------------------------------------
      Set the signal to be used for eMBMS in the control block. This is mainly 
      used for debugging purposes. Since there is only one signal, we can do away
      with this field in the control block and use the signal name directly.
    ----------------------------------------------------------------------------*/

    ctrl_blk_p->rx_sig = ps_sigs[ctrl_blk_index];
 

    /*----------------------------------------------------------------------------
      Since there is no need to implement the phys link functions, we set the fn
      pointers to NULL
    ----------------------------------------------------------------------------*/
    phys_link_ptr = &(ctrl_blk_p->phys_link);
  
    phys_link_ptr->phys_link_down_cmd_f_ptr    = NULL;
    phys_link_ptr->phys_link_go_null_cmd_f_ptr = NULL;
    phys_link_ptr->phys_link_up_cmd_f_ptr      = NULL;
    phys_link_ptr->ioctl_f_ptr                 = NULL;
  
    /*----------------------------------------------------------------------------
      As of now there is only one phys link. Hence the client data pointer
      of the phys link can be made to point to the eMBMS control block.
      This is the same as the iface's client data pointer.
    ----------------------------------------------------------------------------*/
    phys_link_ptr->client_data_ptr = (void*)(ctrl_blk_p);
  
    /*----------------------------------------------------------------------------
      Initialize boolean signifying the establishment of RLC bridge to False.
      When the first TMGI is activated, this will be set to True if RLC bridge is
      setup. Reset of this field will happen when the bridge is being torn down.
  
      Same logic is used for the DPM registration flag.
    ----------------------------------------------------------------------------*/
    ctrl_blk_p->l2_wm_set_up = FALSE;
  
    ctrl_blk_p->is_registered_with_dpm = FALSE;
  
    /*----------------------------------------------------------------------------
      Initialize the Watermark queue used to hold Rx data
    ----------------------------------------------------------------------------*/
    dsm_queue_init(&(ctrl_blk_p->rx_wm.wm_item),
                   (int) 0xFFFFFFFF,  /* No MAX queue size */
                   &(ctrl_blk_p->rx_wm.wm_queue));

    ds_3gpp_embms_subs_id_map[subs_id].init = TRUE;
  } while (0); 

  return;
} /* ds_3gpp_embms_per_subs_info_init */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_INIT

DESCRIPTION   Called once at mobile power-up. Creates embms ps_iface. Creates
              the phys link. Initializes the data structures to be used with
              eMBMS. Also performs registration with lower layers for message
              router communication.

DEPENDENCIES  eMBMS code runs in DS task.

RETURN VALUE  None

SIDE EFFECTS  None
==============================================================================*/
void ds_3gpp_embms_init(void)
{
  int subs_index = 0;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - -*/

  DS_EMBMS_MSG0_HIGH("DS 3GPP eMBMS Initialization");

  /*-----------------------------------------------------------------------
    Initialize the critical section that will be used to protect the table
    used for TMGI management. 
  -----------------------------------------------------------------------*/		
  DS_3GPP_INIT_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  
  /*======================================================================
     During powerup, we allocate EMBMS Structures only for
     DDS.
   
     If Dual SIM / Triple SIM Device mode event is received at
     a later stage, we will allocate the structure for other
     subscriptions. 
  ======================================================================*/

  for (subs_index = 0; subs_index < DS3GSUBSMGR_SUBS_ID_MAX; subs_index++)
  {
    if (ds_3gpp_embms_subs_id_map[subs_index].init == FALSE)
    {
      ds_3gpp_embms_subs_id_map[subs_index].ctrl_blk_index = 
        DS_3GPP_EMBMS_INVALID_CTRL_BLK_INSTANCE;
    }
  }

  ds_3gpp_embms_per_subs_info_init(ds3g_get_ps_subs_id());
  /*----------------------------------------------------------------------------
    Initialize the context table. The context table is a table of pointers. Set
    all the pointers in the array to NULL
  ----------------------------------------------------------------------------*/
  ds_3gpp_embms_tmgi_tbl_mgr_init_context_table(ds3g_get_ps_subs_id());

  /*------------------------------------------------------------------------
    Register the RLC MSGRTR for WM REG CNF
  ------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(LTE_RLCDL_EMBMS_REG_CNF,
                    (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_wm_reg_cnf_hdlr);

  /*------------------------------------------------------------------------
    Register the RLC MSGRTR for WM DE-REG CNF
  ------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(LTE_RLCDL_EMBMS_DEREG_CNF,
                    (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_wm_dereg_cnf_hdlr);

  /*----------------------------------------------------------------------------
    Initialize the LTE RRC interface module.
    Register with LTE RRC. Registration should happen for all the messages
    that we are interested in.
  ----------------------------------------------------------------------------*/
  ds_3gpp_embms_rrcif_init();

  return;
} /* ds_3gpp_embms_init */

/*===========================================================================

FUNCTION DS_3GPP_EMBMS_PROCESS_CMDS

DESCRIPTION
  This function processes eMBMS commands. 
  This function should be called by the top-level task dispatcher whenever
  any eMBMS command is received. Below commands are processed.
  DS_3GPP_EMBMS_IFACE_BRING_UP_CMD
  DS_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND_CMD
  DS_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND_CMD

DEPENDENCIES
  eMBMS modules should have been initialized prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  ds_3gpp_embms_process_cmds
(
  const ds_cmd_type  *cmd_ptr
)
{
  ds_3gpp_embms_tmgi_act_ind_info            *act_ind_info_ptr = NULL;
  ds_3gpp_embms_tmgi_deact_ind_info          *deact_ind_info_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(cmd_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL cmd_ptr passed, return");
    return;
  }

  DS_EMBMS_MSG1_HIGH("DS 3GPP eMBMS CMD processing. Cmd Id: %d", 
                     cmd_ptr->hdr.cmd_id);

  switch(cmd_ptr->hdr.cmd_id)
  {
    case DS_CMD_3GPP_EMBMS_IFACE_BRING_UP:
      ds_3gpp_embms_iface_bring_up_cmd_hdlr(cmd_ptr->cmd_payload_ptr);
      break;

    case DS_CMD_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        act_ind_info_ptr = 
          (ds_3gpp_embms_tmgi_act_ind_info*)cmd_ptr->cmd_payload_ptr;

        if (act_ind_info_ptr == NULL)
        {
          DATA_ERR_FATAL("act_ind_info_ptr is NULL");
          break;
        }

        ds_3gpp_embms_send_tmgi_activated_ind_cmd_hdlr(
           &(act_ind_info_ptr->ind_info),
           act_ind_info_ptr->index,
           act_ind_info_ptr->subs_id);
      }
      break;

    case DS_CMD_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND:
      if (cmd_ptr->cmd_payload_ptr != NULL)
      {
        deact_ind_info_ptr = 
          (ds_3gpp_embms_tmgi_deact_ind_info*)cmd_ptr->cmd_payload_ptr;

        if (deact_ind_info_ptr == NULL)
        {
          DATA_ERR_FATAL("deact_ind_info_ptr is NULL");
          break;
        }

        ds_3gpp_embms_send_tmgi_deactivated_ind
          (&(deact_ind_info_ptr->ind_info), deact_ind_info_ptr->subs_id );
      }
      break;

    default:
      DS_EMBMS_MSG1_ERROR("Unhandled command id: %d",cmd_ptr->hdr.cmd_id);
      break;
  } /* switch */

  return;
} /* ds_3gpp_embms_process_cmds */


/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_IFACE_BRING_UP

DESCRIPTION   Called when eMBMS is enabled. This posts a command to DS task
              to bring up the interface on the modem.

DEPENDENCIES  None

RETURN VALUE  Integer.

SIDE EFFECTS  None.
==============================================================================*/
LOCAL int ds_3gpp_embms_iface_bring_up
( 
 ps_iface_type    *iface_ptr,
 void             *not_used
)
{
  ds_cmd_type               *cmd_ptr;                     /* DS Task msg */
  sys_modem_as_id_e_type    *subs_id_p = NULL;
  sys_modem_as_id_e_type     subs_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  subs_id = ds3gsubsmgr_subs_id_ds_to_cm
              (PS_IFACE_GET_SUBS_ID(iface_ptr));

  DS_EMBMS_MSG0_HIGH("DS 3GPP eMBMS Bring up Iface");

  if (ds_3gpp_embms_find_or_allocate_ctrl_blk(subs_id) == NULL)
  {
    DS_EMBMS_MSG0_HIGH("Could not allocate EMBMS Cntrl Block");
    return (-1);
  }

  if( (cmd_ptr = ds_allocate_cmd_buf(sizeof(sys_modem_as_id_e_type)) ) == NULL || 
       cmd_ptr->cmd_payload_ptr == NULL)
  {
    ASSERT(0);
    return(-1);
  }                                 

  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EMBMS_IFACE_BRING_UP;
  subs_id_p = (sys_modem_as_id_e_type*)cmd_ptr->cmd_payload_ptr;

  *subs_id_p = subs_id;

  ds_put_cmd(cmd_ptr);

  return(0);
} /* ds_3gpp_embms_iface_bring_up() */

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND

DESCRIPTION
  This function is used to send the indication to the specified client that a 
  TMGI has been deactivated.

  This function is solely used in the processing of 
  DS_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND_CMD which is posted when we decide to 
  send an indication to the client requesting deactivation since the TMGI is
  currently active with more than one client. The command is needed to ensure
  that we post indication after we return from the IOCTL.

PARAMETERS
  *cmd_ptr - ds cmd ptr
   subs_id - Subscription Id

DEPENDENCIES  
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
==============================================================================*/
LOCAL void ds_3gpp_embms_send_tmgi_deactivated_ind
(
  ps_iface_embms_tmgi_deactivation_info_type *ind_info_ptr,
  sys_modem_as_id_e_type                      subs_id
)
{
  ps_iface_type                              *iface_ptr = NULL;  
  ds_3gpp_embms_ctrl_blk_type                *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Validate the incoming params
  ----------------------------------------------------------------------------*/
  if(ind_info_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Print out the request. Call the function to print out the info in a manner
      that is desirable. Print the client id & num TMGI passed in the command.
    --------------------------------------------------------------------------*/
    DS_EMBMS_MSG1_HIGH("Send DEACT ind cmd hdlr: Num TMGI: %d", 
                       ind_info_ptr->tmgi_list.num_tmgi);

    ds_3gpp_embms_print_tmgi_session_info(
                         ind_info_ptr->tmgi_list.list);

    DS_EMBMS_MSG3_HIGH("Send DEACT ind cmd hdlr: Client id: 0x%x, "
                       "debug_trace_id: 0x%x, Deact cause: %d", 
                       ind_info_ptr->client_id, ind_info_ptr->debug_trace_id, 
                       ind_info_ptr->info_code);

    /*--------------------------------------------------------------------------
      Get the iface pointer from the eMBMS control block.
    --------------------------------------------------------------------------*/
    iface_ptr = &(ctrl_blk_p->ps_iface);

    ps_iface_generic_ind(iface_ptr, 
                         IFACE_EMBMS_TMGI_DEACTIVATED_EV, 
                         (void*)ind_info_ptr);

    /*--------------------------------------------------------------------------
      Free the memory that was allocated to hold the TMGI info in the deact ind
      payload
    --------------------------------------------------------------------------*/
    modem_mem_free((void*)ind_info_ptr->tmgi_list.list,
                   MODEM_MEM_CLIENT_DATA);
  } /* Valid input params */
  else
  {
    DS_EMBMS_MSG0_ERROR("Invalid input params. Ind_info_ptr: is NULL");
  } /* Invalid input params */

  return;

} /* ds_3gpp_embms_send_tmgi_deactivated_ind_cmd */

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND_CMD_HDLR

DESCRIPTION
  This function is used to send the indication to the specified client that a 
  TMGI has been activated.

  This function is solely used in the processing of 
  DS_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND_CMD which is posted when we decide to 
  send an indication to the client requesting activation since the TMGI is
  already active.

PARAMETERS
  ind_info_ptr  - information that will be sent in the Iface indication.
  user_data_ptr - call back information (used to get the index into the TMGI
                  context table) 
   subs_id      - Subscription Id

DEPENDENCIES  
  None

RETURN VALUE  
  None

SIDE EFFECTS
  None
==============================================================================*/
LOCAL void ds_3gpp_embms_send_tmgi_activated_ind_cmd_hdlr
(
  ps_iface_embms_tmgi_activation_info_type   *ind_info_ptr,
  uint8                                      incoming_index,
  sys_modem_as_id_e_type                     subs_id
)
{
  ps_iface_type                              *iface_ptr = NULL;
  uint8                                      tbl_index = 
                                                DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE;
  ds_3gpp_embms_tmgi_state_e                 tmgi_state = 
                                               DS_3GPP_EMBMS_TMGI_STATE_INVALID;
  ds_3gpp_embms_ctrl_blk_type                *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  /*----------------------------------------------------------------------------
    Validate the incoming params
  ----------------------------------------------------------------------------*/
  if( (ind_info_ptr != NULL) && 
      (DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(incoming_index)) )
  {
    /*--------------------------------------------------------------------------
      Print out the request. Call the function to print out the info in a manner
      that is desirable. Print the client id & tbl index passed in the command.
    --------------------------------------------------------------------------*/
    ds_3gpp_embms_print_tmgi_session_info(&(ind_info_ptr->tmgi_session_info));

    DS_EMBMS_MSG3_HIGH("Send ACT ind cmd hdlr: Client id: 0x%x,"
                       " debug_trace_id: 0x%x, Table index: %d", 
                       ind_info_ptr->client_id, ind_info_ptr->debug_trace_id,
                       incoming_index);

    /*----------------------------------------------------------------------------
      Before sending the indication, find the index in the TMGI context table 
      where the TMGI_Session info resides. The index returned by the find function
      should match the index that we get from the command pointer. If this is not
      true, we cannot guarantee that this indication is "not stale". In other 
      words, if the TMGI context info has changed in the index that we get in the
      command, we could be sending a stale indication. We should ignore the 
      processing of the command in such cases.
    ----------------------------------------------------------------------------*/
    if(ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
         (&(ind_info_ptr->tmgi_session_info), FALSE, &tbl_index, subs_id) == 
           TRUE)
    {
      ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(tbl_index));

      if(tbl_index == incoming_index)
      {
        /*----------------------------------------------------------------------
          Get the state of the TMGI. If the TMGI is in ACTIVE state post the
          indication. If the TMGI is in any other state ignore the command.
          Print error message
        ----------------------------------------------------------------------*/
        tmgi_state = ds_3gpp_embms_tmgi_tbl_mgr_get_state(tbl_index, subs_id);
        if(tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_ACTIVE)
        {
          /*--------------------------------------------------------------------
            Get the PS iface and post iface indication
          --------------------------------------------------------------------*/
          iface_ptr = &(ctrl_blk_p->ps_iface);

          ps_iface_generic_ind(iface_ptr, 
                               IFACE_EMBMS_TMGI_ACTIVATED_EV, 
                               (void*)ind_info_ptr);
        } /* if TMGI is in active state */
        else
        {
          DS_EMBMS_MSG1_ERROR("TMGI not in ACTIVE state. State: %d. Ignore cmd",
                              tmgi_state);
        } /* TMGI not in active state */
      } /* Command index is same as table index */
      else
      {
        DS_EMBMS_MSG2_ERROR("TMGI_Session info found in diff index. Tbl index:"
                            " %d, index in cmd: %d",tbl_index,incoming_index);
      } /* Command index is different from table index */

    } /* TMGI is found in the table */
    else
    {
      DS_EMBMS_MSG0_ERROR("TMGI_Session info not found in TMGI cntx tbl");
    } /* TMGI not found in table */
  } /* Valid input params */
  else
  {
    DS_EMBMS_MSG2_ERROR("Invalid input params. Ind_info_ptr: 0x%x, Index: %d",
                        ind_info_ptr,incoming_index);
  } /* Invalid input params */

  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  return;

} /* ds_3gpp_embms_send_tmgi_activated_ind_cmd */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_ACT_SUCCESS_GEN_IND

DESCRIPTION
  This function is used to generate TMGI activated indication when a TMGI act
  cnf is received from LTE RRC. It gets the list from the TMGI TBL MGR and sends
  indication for all the clients present in the list.

PARAMETERS

  ind_info_ptr - pointer to the payload. (this function overwrites the client
                 id in this location and generates as many indications as the
                 clients)
  index        - index into the TMGI table. Needed to get the list of clients
  subs_id      - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_act_success_gen_ind
(
  ps_iface_embms_tmgi_activation_info_type   *ind_info_ptr,
  uint8                                      index,
  sys_modem_as_id_e_type                     subs_id
)
{
  list_type                            *list_ptr = NULL;
  ds_3gpp_embms_client_info_type       *client_ptr = NULL;
  ps_iface_type                        *iface_ptr = NULL;
  ds_3gpp_embms_ctrl_blk_type          *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));
  ASSERT(ind_info_ptr != NULL);

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Get the eMBMS iface
  ----------------------------------------------------------------------------*/
  iface_ptr = &(ctrl_blk_p->ps_iface);

  /*----------------------------------------------------------------------------
    Get the pointer to the list of clients.
  ----------------------------------------------------------------------------*/
  list_ptr = ds_3gpp_embms_tmgi_tbl_mgr_get_list(index, subs_id);

  ASSERT(list_ptr != NULL);

  /*----------------------------------------------------------------------------
    Get the pointer to the head node in the list.
  ----------------------------------------------------------------------------*/
  client_ptr = (ds_3gpp_embms_client_info_type*) list_peek_front(list_ptr);

  while(client_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Print the client id and send an iface indication
    --------------------------------------------------------------------------*/
    DS_EMBMS_MSG1_MED("Sending TMGI ACT Success indication for Client id: 0x%x",
                      client_ptr->user_data_ptr);

    /*--------------------------------------------------------------------------
      Copy the client id into the iface indication payload
    --------------------------------------------------------------------------*/
    ind_info_ptr->client_id = client_ptr->user_data_ptr;

    /*--------------------------------------------------------------------------
      Send the iface indication
    --------------------------------------------------------------------------*/
    ps_iface_generic_ind(iface_ptr, 
                         IFACE_EMBMS_TMGI_ACTIVATED_EV, 
                         (void*)ind_info_ptr);

    /*--------------------------------------------------------------------------
      Get the next client in the list
    --------------------------------------------------------------------------*/
    client_ptr = list_peek_next(list_ptr, &(client_ptr->link));
  } /* while client_ptr != NULL */

}/* ds_3gpp_embms_tmgi_act_success_gen_ind */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_ACT_DEACT_RESPONSE_GEN_IND

DESCRIPTION
  This function is used to generate TMGI activate deactive response indication.
 
  Client ID cannot have the reserved value. ACT_DEACT response can only result
  from an explicit request from a client.

PARAMETERS

  ind_info_ptr - pointer to the payload. (this function overwrites the client
                 id in this location and generates as many indications as the
                 clients)
  index        - index into the TMGI table. Needed to get the list of clients
                 This field is used only when the indication is sent on a per
                 client basis.
  subs_id      - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_act_deact_response_gen_ind
(
  ps_iface_embms_tmgi_activation_deactivation_info_type *ind_info_ptr,
  uint8                                                  index,
  sys_modem_as_id_e_type                                 subs_id
)
{
  list_type                            *list_ptr = NULL;
  ds_3gpp_embms_client_info_type       *client_ptr = NULL;
  ps_iface_type                        *iface_ptr = NULL;
  ds_3gpp_embms_ctrl_blk_type          *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ind_info_ptr == NULL)
  {
    DS_EMBMS_MSG0_ERROR("ds_3gpp_embms_tmgi_act_deact_resp_gen_ind: ind_info_ptr "
                        "NULL. Returning...");
    return;
  }

  if(ind_info_ptr->client_id == NULL
	 && !DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index))
  {
     DS_EMBMS_MSG1_ERROR("ds_3gpp_embms_tmgi_act_deact_response_gen_ind: invalid"
			" index: %d. Returning...", index);
     return;
  }

  if(ind_info_ptr->client_id == (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID)
  {
    DS_EMBMS_MSG0_ERROR("ds_3gpp_embms_tmgi_act_deact_response_gen_ind: Reserved"
		        " client ID invalid here. Returning...");
    return;
  } 

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Get the eMBMS iface
  ----------------------------------------------------------------------------*/
  iface_ptr = &(ctrl_blk_p->ps_iface);

  /*----------------------------------------------------------------------------
	Get the pointer to the list of clients.
  ----------------------------------------------------------------------------*/
  list_ptr = ds_3gpp_embms_tmgi_tbl_mgr_get_list(index, subs_id);

  ASSERT(list_ptr != NULL);

  /*----------------------------------------------------------------------------
	Get the pointer to the head node in the list.
  ----------------------------------------------------------------------------*/
  client_ptr = (ds_3gpp_embms_client_info_type*) list_peek_front(list_ptr);

  while(client_ptr != NULL)
  {
	/*--------------------------------------------------------------------------
	  Print the client id and send an iface indication
	--------------------------------------------------------------------------*/
	DS_EMBMS_MSG1_MED("Sending TMGI ACT DEACT ind for Client id: 0x%x",
			  client_ptr->user_data_ptr);

	/*--------------------------------------------------------------------------
	  Copy the client id into the iface indication payload
	--------------------------------------------------------------------------*/
	ind_info_ptr->client_id = client_ptr->user_data_ptr;

	/*--------------------------------------------------------------------------
	  Send the iface indication
	--------------------------------------------------------------------------*/
	ps_iface_generic_ind(iface_ptr, 
						 IFACE_EMBMS_TMGI_ACTIVATE_DEACTIVATE_RESPONSE_EV, 
						 (void*)ind_info_ptr);

	/*--------------------------------------------------------------------------
	  Get the next client in the list
	--------------------------------------------------------------------------*/
	client_ptr = list_peek_next(list_ptr, &(client_ptr->link));
  } /* while client_ptr != NULL */
    
}/* ds_3gpp_embms_tmgi_deactivated_gen_ind */


/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_DEACTIVATED_GEN_IND

DESCRIPTION
  This function is used to generate TMGI activated indication when a TMGI is
  deactivated. This could be because of TMGI activation failure, TMGI deactivate
  success or due to modem initiated deactivation.
  It gets the list from the TMGI TBL MGR and sends indication for all the 
  clients present in the list.

  To distinguish between the case where the indication is to be sent per client
  and cases where indication is sent to all clients, the caller of this function
  should set the client id to a reserved value.

PARAMETERS

  ind_info_ptr - pointer to the payload. (this function overwrites the client
                 id in this location and generates as many indications as the
                 clients)
  index        - index into the TMGI table. Needed to get the list of clients
                 This field is used only when the indication is sent on a per
                 client basis.
  subs_id      - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_deactivated_gen_ind
(
  ps_iface_embms_tmgi_deactivation_info_type *ind_info_ptr,
  uint8                                      index,
  sys_modem_as_id_e_type                     subs_id
)
{
  list_type                            *list_ptr = NULL;
  ds_3gpp_embms_client_info_type       *client_ptr = NULL;
  ps_iface_type                        *iface_ptr = NULL;
  ds_3gpp_embms_ctrl_blk_type          *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(ind_info_ptr != NULL);

  if(ind_info_ptr->client_id == NULL)
  {
    /*--------------------------------------------------------------------------
      If the caller of this fn has set the client id to NULL, the indication has
      to be generated for all clients. Also the deactivation is for a particular
      TMGI. We need the index into the TMGI table to handle this
    --------------------------------------------------------------------------*/
    ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));
  }

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Get the eMBMS iface
  ----------------------------------------------------------------------------*/
  iface_ptr = &(ctrl_blk_p->ps_iface);

  if(ind_info_ptr->client_id == (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID)
  {
    DS_EMBMS_MSG0_MED("Deactivated Indication to all clients");
    /*--------------------------------------------------------------------------
      Send the iface indication
    --------------------------------------------------------------------------*/
    ps_iface_generic_ind(iface_ptr, 
                         IFACE_EMBMS_TMGI_DEACTIVATED_EV, 
                         (void*)ind_info_ptr);
  } /* deactivated indication to all clients */
  else
  {
    DS_EMBMS_MSG0_MED("Client specific ind for TMGI deactivation");

    /*----------------------------------------------------------------------------
      Get the pointer to the list of clients.
    ----------------------------------------------------------------------------*/
    list_ptr = ds_3gpp_embms_tmgi_tbl_mgr_get_list(index, subs_id);

    ASSERT(list_ptr != NULL);

    /*----------------------------------------------------------------------------
      Get the pointer to the head node in the list.
    ----------------------------------------------------------------------------*/
    client_ptr = (ds_3gpp_embms_client_info_type*) list_peek_front(list_ptr);

    while(client_ptr != NULL)
    {
      /*--------------------------------------------------------------------------
        Print the client id and send an iface indication
      --------------------------------------------------------------------------*/
      DS_EMBMS_MSG1_MED("Sending TMGI DEACT ind for Client id: 0x%x",
                        client_ptr->user_data_ptr);

      /*--------------------------------------------------------------------------
        Copy the client id into the iface indication payload
      --------------------------------------------------------------------------*/
      ind_info_ptr->client_id = client_ptr->user_data_ptr;

      /*--------------------------------------------------------------------------
        Send the iface indication
      --------------------------------------------------------------------------*/
      ps_iface_generic_ind(iface_ptr, 
                           IFACE_EMBMS_TMGI_DEACTIVATED_EV, 
                           (void*)ind_info_ptr);

      /*--------------------------------------------------------------------------
        Get the next client in the list
      --------------------------------------------------------------------------*/
      client_ptr = list_peek_next(list_ptr, &(client_ptr->link));
    } /* while client_ptr != NULL */
  } /* indication to be sent for a specific TMGI and per client basis */
    

}/* ds_3gpp_embms_tmgi_deactivated_gen_ind */

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_IFACE_BRING_UP_CMD_HDLR

DESCRIPTION   cmd posted when DS_3GPP_EMBMS_IFACE_BRING_UP() function 
              corresponding to embms ps_iface is called. This is typically
              called when eMBMS is enabled. This function brings up the iface
              and the phys link. Since no OTA signaling is involved, it moves
              the state of these call objects. 

PARAMETERS    Pointer to the subscription id

DEPENDENCIES  None

RETURN VALUE  None.

SIDE EFFECTS  If the iface is already UP, iface_up indication is posted. Note
              that its important to post iface_up indication, even though the
              state is already UP as the application clients must be blocked
              on the IFACE_UP event.
==============================================================================*/
LOCAL void ds_3gpp_embms_iface_bring_up_cmd_hdlr
( 
  sys_modem_as_id_e_type *subs_id_p
)
{
  ps_iface_type               *iface_ptr = NULL;
  ps_phys_link_type           *phys_link_ptr = NULL;
  boolean                      tlb_in_mode_C = FALSE;
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = NULL;
  ps_iface_down_mh_event_info_type        down_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  DS_EMBMS_MSG0_HIGH("DS 3GPP eMBMS Bring up Iface Cmd Hdlr");

  if (subs_id_p == NULL)
  {
    ASSERT(0);
    return;
  }

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(*subs_id_p);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*--------------------------------------------------------------------------
    Get the iface pointer from the eMBMS control block
  --------------------------------------------------------------------------*/
  iface_ptr = &(ctrl_blk_p->ps_iface);

  /*----------------------------------------------------------------------------
    It is ideal to allow interface bring up only when LTE RRC is enabled for 
    eMBMS service.

    We will deny iface bring up if LTE RRC is not enabled. If there are race
    conditions that were not considered in the initial design, we will have to
    revisit this strict check
  ----------------------------------------------------------------------------*/
 
  tlb_in_mode_C=dsEpsTlb_is_tlb_in_mode_C(*subs_id_p);
  if((ds_3gpp_embms_rrcif_is_iface_bring_up_allowed() == TRUE) || 
     (tlb_in_mode_C))
  {
    /*--------------------------------------------------------------------------
      Get the phys link pointer from the eMBMS control block
    --------------------------------------------------------------------------*/
    phys_link_ptr = &(ctrl_blk_p->phys_link);

    /*--------------------------------------------------------------------------
      Tx Cmd info is not used. It can be set to NULL. 
    --------------------------------------------------------------------------*/
    ps_phys_link_set_tx_function(phys_link_ptr,
                                 ds_3gpp_embms_tx_um_data_cb, ctrl_blk_p);

    /*--------------------------------------------------------------------------
      Register the Rx Signal handler function. There is no need to pass the user
      data here. But for the ease of implementation in the signal handler and to
      allow future enhancements, the Wm from which data is to be drained is 
      passed as user_data
    --------------------------------------------------------------------------*/
    (void)ps_set_sig_handler(ctrl_blk_p->rx_sig,
                             ds_3gpp_embms_rx_um_data_cb, 
                            (void*)ctrl_blk_p);

    /*-------------------------------------------------------------------------  
      Enable the signal
    -------------------------------------------------------------------------*/
    ps_enable_sig(ctrl_blk_p->rx_sig);

    /*--------------------------------------------------------------------------
      Activate Default Flow
      Post phys link up and enable flow on the phys link
    --------------------------------------------------------------------------*/

    ps_flow_activate_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
                         PS_EIC_NOT_SPECIFIED);

    ps_phys_link_up_ind(phys_link_ptr);

    /*--------------------------------------------------------------------------
      Enable flow on the primary phys link TODO: Hari - is this needed?
    --------------------------------------------------------------------------*/
    ps_phys_link_enable_flow(phys_link_ptr, DS_FLOW_ALL_MASK);

    if(!ds3gflowmgr_get_coex_data_flow_status(*subs_id_p))
    {
      ps_phys_link_disable_flow(phys_link_ptr, DS_FLOW_COEX_MASK);
    }

    /*--------------------------------------------------------------------------
      Move the Iface state to UP.
    --------------------------------------------------------------------------*/
    ps_iface_up_ind(iface_ptr);

    /*--------------------------------------------------------------------------
      If we are in TLB mode C. Set up the RLC - PS watermark during Iface up
      as there will not TMGI act/act-deact message
    --------------------------------------------------------------------------*/
    if (tlb_in_mode_C)
    {
      ds_3gpp_embms_set_up_rlc_to_ps_wm(ctrl_blk_p);
      dsEpsTlb_post_iface_up_ind_mode_C(*subs_id_p);      
    }
    else
    {
      /*---------------------------------------------------------------------
        Register iface with DPM in case of iface bring up
        ---------------------------------------------------------------------*/
      if (FALSE == ds_3gpp_embms_reg_wm_with_dpm(ctrl_blk_p))
      {
        DS_EMBMS_MSG0_ERROR("EMBMS iface up: Reg with DPM failed");
      }
    }

  } /* If LTE RRC is enabled - allow iface bring up */
  else
  {
    DS_EMBMS_MSG0_HIGH("Cannot allow iface Bring up. eMBMS not enabled at RRC");

    down_info.down_reason = PS_NET_DOWN_REASON_REGULAR_DEACTIVATION;

    down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
    down_info.bearer_tech.rat_value =
      ds_3gpp_map_sys_mode_to_ds_sys_rat
        (ds3gpp_get_current_network_mode(ctrl_blk_p->subs_id));
    down_info.bearer_tech.so_mask = 
      ds_3gpp_bearer_cntxt_get_so_mask(ctrl_blk_p->subs_id);

    ps_iface_down_ind(iface_ptr, &down_info);
    ds_3gpp_embms_free_ctrl_blk(ctrl_blk_p->subs_id);
  }
  return;
} /* ds_3gpp_embms_iface_bring_up_cmd_hdlr() */

/*===========================================================================
FUNCTION DS_3GPP_EMBMS_TX_UM_DATA_CB

DESCRIPTION
  This function handles any packet to be transmitted over the air in the UL
  direction. There shoud be no uplink traffic. If any reaches the modem in 
  this direction, it will be dropped at this point.
  
PARAMETERS  
  phys_link_ptr - pointer to the phys link on which this is to be sent
  item_ptr      - pointer to the packet to be transmitted
  meta_info_ptr - pointer to the meta info blob
  tx_info_ptr   - user data
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None
   
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL void ds_3gpp_embms_tx_um_data_cb
(
  ps_phys_link_type *phys_link_ptr,
  dsm_item_type     **item_ptr,
  ps_meta_info_type *meta_info_ptr,
  void              *tx_info_ptr
)
{
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ctrl_blk_p = (ds_3gpp_embms_ctrl_blk_type*)tx_info_ptr;

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  if(phys_link_ptr == &(ctrl_blk_p->phys_link))
  {
    DS_EMBMS_MSG0_HIGH("Dropping UL data on the interface - Benign");

    dsm_free_packet(item_ptr);

    if(meta_info_ptr != NULL)
    {
      PS_META_INFO_FREE(&meta_info_ptr);
    }
  }/* If the phys link pointer matches the one used for eMBMS */
  else
  {
    DS_EMBMS_MSG0_ERROR("eMBMS: Phys link Tx fn invoked incorrectly");
  }

} /* ds_3gpp_embms_tx_um_data_cb() */

/*==============================================================================
FUNCTION DS_3GPP_EMBMS_RX_UM_DATA_CB

DESCRIPTION
  This function handles any packet that has been received over the air in the DL
  direction. There shoud be no uplink traffic. If any reaches the modem in 
  this direction, it will be dropped at this point.
  
PARAMETERS  
  sig             - Signal to process
  user_data_ptr   - user data
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  FALSE if there is more data in the WM
  TRUE if there is no more data
   
SIDE EFFECTS 
  None.
  
==============================================================================*/
LOCAL boolean ds_3gpp_embms_rx_um_data_cb
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void             *user_data_ptr          /* Parameter carrying user data */
)
{
  dsm_item_type               *item_ptr = NULL;   /* Pointer to DSM item         */
  dsm_watermark_type          *wm_ptr = NULL;     /* Watermark to dequeue from   */
  boolean                      ret_val = TRUE;     /* Return Value                */
  ps_iface_type               *iface_ptr = NULL;  /* Iface to send packets on    */
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ctrl_blk_p = (ds_3gpp_embms_ctrl_blk_type*)user_data_ptr;

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return FALSE;
  }

  ASSERT(sig == ctrl_blk_p->rx_sig);

  /*----------------------------------------------------------------------------
    Get the Watermark from the call back data and ensure that it is the same as
    the WM pointer as in our control block.
  ----------------------------------------------------------------------------*/
  wm_ptr = &(ctrl_blk_p->rx_wm.wm_item);

  ASSERT(wm_ptr == &(ctrl_blk_p->rx_wm.wm_item));

  /*----------------------------------------------------------------------------
    Get the PS iface
  ----------------------------------------------------------------------------*/
  iface_ptr = &(ctrl_blk_p->ps_iface);

  item_ptr = dsm_dequeue(wm_ptr);

  if(item_ptr != NULL)
  {
    /*--------------------------------------------------------------------------
      Call PS Iface input with the appropriate parameters. We set the meta info
      pointer explicitly to NULL.
    --------------------------------------------------------------------------*/
    (void)ps_iface_input(iface_ptr, &item_ptr, NULL);

    /*--------------------------------------------------------------------------
      Set the retval to false so that PS task sets the signal again.
      Todo: Hari - Should the draining happen in this fashion? Or should we 
      drain in one shot and return True?
    --------------------------------------------------------------------------*/
    ret_val = FALSE;
  }

  return ret_val;
  
} /* ds_3gpp_embms_rx_um_data_cb */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_CLEAN_UP_RX_WM

DESCRIPTION
  This function is used to flush out any data that may be present in the WM

PARAMETERS
  Pointer to the EMBMS Control Block

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
LOCAL void ds_3gpp_embms_clean_up_rx_wm
(
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p
)
{
  dsm_watermark_type* wm_ptr = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Get the watermark pointer for the DL data and flush any data in it.
  ----------------------------------------------------------------------------*/
  wm_ptr = &(ctrl_blk_p->rx_wm.wm_item);

  /*-------------------------------------------------------------------------
    Empty the WM and set the received count to zero
  -------------------------------------------------------------------------*/
  dsm_empty_queue(wm_ptr);
  
  /*-----------------------------------------------------------------------
    Reset the total received count. Should this be done differently?
  -----------------------------------------------------------------------*/
  wm_ptr->total_rcvd_cnt = 0;
} /* ds_3gpp_embms_clean_up_rx_wm */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_SET_UP_RLC_TO_PS_WM

DESCRIPTION
  This function is used to set up the Air link receive watermarks. We pass a
  message to RLC with WM and Bearer ID and wait for its confirmation
 
  On its confirmation we register the WM with DPM
 
PARAMETERS
  ctrl_blk_p - Pointer to the EMBMS Control Block
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_3gpp_embms_set_up_rlc_to_ps_wm
(
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p
)
{
  lte_rlcdl_embms_wm_register_req_msg_s   rlc_wm_reg_msg;
  boolean                                 rtval = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*------------------------------------------------------------------------
    We check whether Reg was done with RLC or not, it does not make to perform
    Registration twice
  -------------------------------------------------------------------------*/
  if(ctrl_blk_p->l2_wm_set_up == FALSE)
  {
    memset (&rlc_wm_reg_msg, 0, sizeof (lte_rlcdl_embms_wm_register_req_msg_s));
    rlc_wm_reg_msg.embms_wm_ptr = &(ctrl_blk_p->rx_wm.wm_item);
    rlc_wm_reg_msg.id = DS_3GPP_EMBMS_DUMMY_EPS_ID;

    rtval = dsmsgrsnd_per_subs_msg_send
              (LTE_RLCDL_EMBMS_REG_REQ, 
               (msgr_hdr_struct_type*)(&rlc_wm_reg_msg),
               sizeof(lte_rlcdl_embms_wm_register_req_msg_s),
               SYS_AS_ID_TO_INST_ID(ctrl_blk_p->subs_id));

    if(rtval == FALSE) 
    {
      DS_EMBMS_MSG0_ERROR(" Unable to send message to DSMSGR");
    }  
  }
  else
  {
    DS_EMBMS_MSG0_MED ("EMBMS Watermark is already setup with RLC");
  }

} /* ds_3gpp_embms_setup_rlc_to_ps_wm() */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_BRING_DOWN_RLC_TO_PS_WM

DESCRIPTION
  This function is used to bring down the Air link receive watermarks.We pass a
  message to RLC with WM and Bearer ID and wait for its confirmation.
 
  On its confirmation we deregister the WM with DPM
 
PARAMETERS
  subs_id      - Subscription Id
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_3gpp_embms_bring_down_rlc_to_ps_wm
(
  sys_modem_as_id_e_type subs_id
)
{
  lte_rlcdl_embms_wm_deregister_req_msg_s   rlc_wm_dereg_msg;
  boolean                                   rtval = FALSE;
  ds_3gpp_embms_ctrl_blk_type               *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*------------------------------------------------------------------------
    We check whether Reg was done with RLC or not, it does not make to perform
    de-registration on an already deregistered WM
   
    We also need to make sure that no TMGI is active before breaking the data
    path
  -------------------------------------------------------------------------*/
  if((ds_3gpp_embms_tmgi_tbl_mgr_is_any_tmgi_active(subs_id) == FALSE ))
  {
    if(ctrl_blk_p->l2_wm_set_up == TRUE)
    {
      memset (&rlc_wm_dereg_msg, 0, 
              sizeof(lte_rlcdl_embms_wm_deregister_req_msg_s));

      rlc_wm_dereg_msg.id = DS_3GPP_EMBMS_DUMMY_EPS_ID;

      rtval = dsmsgrsnd_per_subs_msg_send
               (LTE_RLCDL_EMBMS_DEREG_REQ, 
                (msgr_hdr_struct_type*)(&rlc_wm_dereg_msg),
                 sizeof(lte_rlcdl_embms_wm_deregister_req_msg_s),
                 SYS_AS_ID_TO_INST_ID(ctrl_blk_p->subs_id));

      if(rtval == FALSE) 
      {
        DS_EMBMS_MSG0_ERROR(" Unable to send message to DSMSGR");
      }  
    }
    else
    {
      DS_EMBMS_MSG0_MED ("EMBMS Watermark is not setup with RLC: Cannot De-reg");
    }
  }
  else
  {
    DS_EMBMS_MSG0_HIGH("Other TMGIs active. Don't tear data path");
  }

} /* ds_3gpp_embms_bring_down_rlc_to_ps_wm() */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TEAR_DOWN

DESCRIPTION
  This function is used to clean up the eMBMS control structures. This puts
  the phys link and iface to down sate.

PARAMETERS
  Pointer to the EMBMS Control Block

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
==============================================================================*/
void ds_3gpp_embms_tear_down
(
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p
)
{
  ps_iface_type     *iface_ptr = NULL;
  ps_phys_link_type *phys_link_ptr = NULL;
  ps_iface_down_mh_event_info_type   down_info;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(&down_info, 0, sizeof(ps_iface_down_mh_event_info_type));

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  down_info.down_reason = PS_NET_DOWN_REASON_REGULAR_DEACTIVATION;

  down_info.bearer_tech.technology = DS_SYS_NETWORK_3GPP;
  down_info.bearer_tech.rat_value =
    ds_3gpp_map_sys_mode_to_ds_sys_rat
      (ds3gpp_get_current_network_mode(ctrl_blk_p->subs_id));
  down_info.bearer_tech.so_mask = 
    ds_3gpp_bearer_cntxt_get_so_mask(ctrl_blk_p->subs_id);

  iface_ptr = &(ctrl_blk_p->ps_iface);
  phys_link_ptr = &(ctrl_blk_p->phys_link);

  /*-------------------------------------------------------------------------- 
    De Register Um Iface with DPM before posting Iface down Ind
    --------------------------------------------------------------------------*/
  if (FALSE == ds_3gpp_embms_dereg_wm_with_dpm(ctrl_blk_p))
  {
    DS_EMBMS_MSG0_ERROR("embms iface tear_down. De-Reg with DPM Failed");
  }

  /*----------------------------------------------------------------------------
    Clean up the receive watermarks
  ----------------------------------------------------------------------------*/
  ds_3gpp_embms_clean_up_rx_wm(ctrl_blk_p);

  /*----------------------------------------------------------------------------
    Unbind the default flow and the phys link
  ----------------------------------------------------------------------------*/
  ps_flow_go_null_ind(PS_IFACE_GET_DEFAULT_FLOW(iface_ptr),
                                                          PS_EIC_NOT_SPECIFIED);

  /*----------------------------------------------------------------------------
    Post phys link gone indication
  ----------------------------------------------------------------------------*/
  ps_phys_link_gone_ind(phys_link_ptr);

  /*----------------------------------------------------------------------------
    Post Iface Down indication
  ----------------------------------------------------------------------------*/
  ps_iface_down_ind(iface_ptr, &down_info);

  ds_3gpp_embms_free_ctrl_blk(ctrl_blk_p->subs_id);

} /* ds_3gpp_embms_tear_down */

/*===========================================================================
FUNCTION      DS_3GPP_EMBMS_GET_CRIT_SECTION

DESCRIPTION
  This function is used to get the critical section to be used by any sub
  module in eMBMS MH

DEPENDENCIES
  None

RETURN VALUE
  rex_crit_sect_type*

SIDE EFFECTS
  None
===========================================================================*/
rex_crit_sect_type* ds_3gpp_embms_get_crit_section(void)
{
  return &(ds_3gpp_embms_crit_sect);
} /* ds_3gpp_embms_get_crit_section */

/*==============================================================================
FUNCTION DS_3GPP_EMBMS_IOCTL_HDLR()

DESCRIPTION
  This function is called to handle the IOCTL requests
  PS_IFACE_IOCTL_EMBMS_ACTIVATE_TMGI
  PS_IFACE_IOCTL_EMBMS_DEACTIVATE_TMGI
  PS_IFACE_IOCTL_EMBMS_GET_ACTIVE_TMGI_LIST
  PS_IFACE_IOCTL_EMBMS_GET_AVAIL_TMGI_LIST

DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL int ds_3gpp_embms_ioctl_hdlr
( 
  ps_iface_type       *iface_ptr,       /* PS IFACE for the eMBMS iface */
  ps_iface_ioctl_type ioctl_name,       /* IOCTL Name                   */
  void                *argval_ptr,      /* IOCTL Params                 */  
  sint15              *ps_errno         /* Errno Value                  */  
)
{
  int                          ret_val = -1;
  sys_modem_as_id_e_type       subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DS_EMBMS_MSG1_HIGH("DS 3GPP eMBMS Ioctl: %d", ioctl_name);

  if (ps_errno == NULL)
  {
    DS_EMBMS_MSG0_ERROR("DS 3GPP eMBMS Ioctl: ps_errno is NULL");
    return ret_val;
  }

  if (iface_ptr == NULL)
  {
    DS_EMBMS_MSG0_ERROR("iface ptr is NULL");
    *ps_errno = DS_EINVAL;
    return ret_val;
  }

  subs_id = ds3gsubsmgr_subs_id_ds_to_cm(PS_IFACE_GET_SUBS_ID(iface_ptr));

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    *ps_errno = DS_EINVAL;
    return ret_val;
  }

  if( (argval_ptr == NULL) ||
      (iface_ptr != &(ctrl_blk_p->ps_iface)) )
  {
    DS_EMBMS_MSG3_ERROR("DS 3GPP eMBMS IOCTL: Bad input. iface_ptr: 0x%x, "
                        "argval_ptr: 0x%x, embms_iface_ptr: 0x%x",
                        iface_ptr,argval_ptr,
                        &(ctrl_blk_p->ps_iface));
    *ps_errno = DS_EINVAL;
    return ret_val;
  }

  if( (ps_iface_state(iface_ptr) != IFACE_UP) && 
      (ps_iface_state(iface_ptr) != IFACE_ROUTEABLE) )
  {
    DS_EMBMS_MSG1_ERROR("DS 3GPP eMBMS IOCTL: Iface not up. State: %d",
                        iface_ptr->iface_private.state);

    *ps_errno = DS_ENETNONET;
    return ret_val;
  }
  
  /*-------------------------------------------------------------------------
    Based on the ioctl name, perform the appropriate actions.
  -------------------------------------------------------------------------*/

  switch (ioctl_name)
  {
  case PS_IFACE_IOCTL_EMBMS_ACTIVATE_TMGI:
    ret_val = ds_3gpp_embms_activate_tmgi_ioctl_hdlr
                (argval_ptr, ps_errno, subs_id);
    break;
  case PS_IFACE_IOCTL_EMBMS_DEACTIVATE_TMGI:
    ret_val = ds_3gpp_embms_deactivate_tmgi_ioctl_hdlr
                (argval_ptr, ps_errno, subs_id);
    break;
  case PS_IFACE_IOCTL_EMBMS_GET_ACTIVE_TMGI_LIST:
    ret_val = ds_3gpp_embms_get_active_tmgi_list_ioctl_hdlr
                (argval_ptr, ps_errno, subs_id);
    break;
  case PS_IFACE_IOCTL_EMBMS_GET_AVAIL_TMGI_LIST:
    ret_val = ds_3gpp_embms_get_avail_tmgi_list_ioctl_hdlr
               (argval_ptr, ps_errno, subs_id);
    break;
  case PS_IFACE_IOCTL_EMBMS_ACT_DEACT_TMGI:
   ret_val = ds_3gpp_embms_act_deact_tmgi_ioctl_hdlr
               (argval_ptr, ps_errno, subs_id);
   break;
  case PS_IFACE_IOCTL_EMBMS_GET_SAI_LIST:
    ret_val = ds_3gpp_embms_get_sai_list_ioctl_hdlr
                (argval_ptr, ps_errno, subs_id);
    break;
  case PS_IFACE_IOCTL_EMBMS_GENERATE_EVENTS:
    ret_val = ds_3gpp_embms_generate_events_ioctl_hdlr
                (argval_ptr,ps_errno, subs_id);
    break;
  case PS_IFACE_IOCTL_EMBMS_CONTENT_DESC_UPDATE:
    ret_val = ds_3gpp_embms_content_desc_update_ioctl_hdlr
                (argval_ptr, ps_errno, subs_id);
    break;
  default:
    DS_EMBMS_MSG1_ERROR("DS 3GPP eMBMS IOCTL: Invalid IOCTL on eMBMS Iface: %d",
                        ioctl_name);
    break;
  } /* switch ioctl_name*/

  return ret_val;
} /* ds_3gpp_embms_ioctl_hdlr */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_DEACTIVATE_TMGI_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to deactivate a TMGI. Based on the TMGI
  state machine a request is sent to LTE RRC. It is possible that we need not go
  to RRC to service the request. DS may also reject the IOCTL in cases where it
  is not possible to deactivate a TMGI


DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_deactivate_tmgi_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
)
{
  ps_iface_embms_tmgi_deact_req_type         *ioctl_arg_ptr = NULL;
  int                                        ret_val = -1;
  boolean                                    tmgi_entry_present = FALSE;
  uint8                                      index = 
                                                DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE;
  uint8                                      ref_count = 0;
  ds_3gpp_embms_tmgi_state_e                 tmgi_state = 
                                               DS_3GPP_EMBMS_TMGI_STATE_INVALID;
  ds_cmd_type                                *cmd_ptr = NULL;
  ps_iface_embms_tmgi_type                   *tmgi_info_ptr = NULL;
  ds_3gpp_embms_tmgi_deact_ind_info          *deact_ind_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(argval_ptr != NULL && ps_errno!= NULL);

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  /*----------------------------------------------------------------------------
    Typecast the incoming argval_ptr to the appropriate type
  ----------------------------------------------------------------------------*/
  ioctl_arg_ptr = (ps_iface_embms_tmgi_deact_req_type*)argval_ptr;

  /*----------------------------------------------------------------------------
    Print out the request. Call the function to print out the info in a manner
    that is desirable. Print the client id passed in the request.
  ----------------------------------------------------------------------------*/
  ds_3gpp_embms_print_tmgi_session_info(&(ioctl_arg_ptr->tmgi_session_info));

  DS_EMBMS_MSG2_HIGH("Deact TMGI IOCTL HDLR: Client id: 0x%x, "
                     "debug_trace_id: %d", ioctl_arg_ptr->client_id ,
                     ioctl_arg_ptr->debug_trace_id);

    /*----------------------------------------------------------------------------
      Logic as follows:
        Check state of TMGI
          If TMGI is not present
            Fail IOCTL

          TMGI entry present
            Check state

              ACTIVE, ACTIVATING
                Check if client is present in list, if not Fail IOCTL
                If yes and if this is last client, send deact req to RRC
                Else remove client from list, decrement ref count, fake an ind
                to the client

              If other states - fail IOCTL.

       Any failure - fail IOCTL if recoverable. If not assert.
  ----------------------------------------------------------------------------*/
  /*----------------------------------------------------------------------------
    Check if there is an entry in the eMBMS TMGI context table that corresponds
    to this TMGI-Session Id Pair.
  ----------------------------------------------------------------------------*/
  tmgi_entry_present = 
       ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
         (&(ioctl_arg_ptr->tmgi_session_info), FALSE, &index, subs_id);

  if(tmgi_entry_present)
  {
    ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));
    /*--------------------------------------------------------------------------
      TMGI entry is present in the eMBMS TMGI context table. Now get the state
      of the TMGI. If the TMGI is in Activating or Active State, we can allow 
      the requested operation.
    --------------------------------------------------------------------------*/

    tmgi_state = ds_3gpp_embms_tmgi_tbl_mgr_get_state(index, subs_id);

    if( (tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_ACTIVE) ||
        (tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_ACTIVATING) )
    {
      /*------------------------------------------------------------------------
        Find out if the client specified is present in the list. If not, we 
        cannot allow the deactivation operation.
      ------------------------------------------------------------------------*/
      
      if(FALSE == ds_3gpp_embms_tmgi_tbl_mgr_is_client_in_list
                    (index, ioctl_arg_ptr->client_id, subs_id))
      {
        /*----------------------------------------------------------------------
          Client Id is not present in the list of clients that have requested
          activation of this TMGI. Fail IOCTL
        ----------------------------------------------------------------------*/
        DS_EMBMS_MSG0_ERROR("Deact TMGI IOCTL: client: 0x%x not present. Fail");
        *ps_errno = DS_EOPNOTSUPP;
      } /* Client id not present in client list */
      else
      {
        /*----------------------------------------------------------------------
         Client id is present in the list. Set the retval and PS_errno as we 
         are going to let the IOCTL succeed.
        ----------------------------------------------------------------------*/
        ret_val = 0;
        *ps_errno = DS_ENOERR;

        /*----------------------------------------------------------------------
          Get the ref count. This is the number of clients that have requested
          activation of the TMGI. We need to go to RRC for deactivation only if
          the ref_count is 1
          Note: Do not alter ref_count now! we need the last client info so
          that we can send deactivate indication.
        ----------------------------------------------------------------------*/
        ref_count = ds_3gpp_embms_tmgi_tbl_mgr_get_ref_count(index, subs_id);

        if(ref_count > 1)
        {
          /*--------------------------------------------------------------------
            Remove the client from the list
          --------------------------------------------------------------------*/
          ds_3gpp_embms_tmgi_tbl_mgr_remove_client_from_list
            (index, ioctl_arg_ptr->client_id, subs_id);

          tmgi_info_ptr = (ps_iface_embms_tmgi_type*) 
                           modem_mem_alloc(sizeof(ps_iface_embms_tmgi_type),
                           MODEM_MEM_CLIENT_DATA);

          if(tmgi_info_ptr != NULL)
          {
            /*----------------------------------------------------------------------
              Post a command to generate Iface indication. 
              This is just to ensure we do not generate the iface indication before 
              we respond to the IOCTL. 
              Post the command irrespective of the state of the TMGI.
            ----------------------------------------------------------------------*/
            cmd_ptr = 
              ds_allocate_cmd_buf(sizeof(ds_3gpp_embms_tmgi_deact_ind_info));

            if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
            {
              ASSERT(0);
              return (-1);
            }

            /*----------------------------------------------------------------
              Fill in the command parameters
            ----------------------------------------------------------------*/
            cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EMBMS_SEND_TMGI_DEACTIVATED_IND;
            deact_ind_info_ptr = 
              (ds_3gpp_embms_tmgi_deact_ind_info*)cmd_ptr->cmd_payload_ptr;

            deact_ind_info_ptr->subs_id = subs_id;
            deact_ind_info_ptr->ind_info.client_id = ioctl_arg_ptr->client_id;
            deact_ind_info_ptr->ind_info.debug_trace_id = 
              ioctl_arg_ptr->debug_trace_id;
            deact_ind_info_ptr->ind_info.info_code = 
              IFACE_EMBMS_TMGI_DEACTIVATION_CLIENT_INITIATED;
            deact_ind_info_ptr->ind_info.tmgi_list.list = tmgi_info_ptr;
            deact_ind_info_ptr->ind_info.tmgi_list.num_tmgi = 1;

            memscpy((void*)tmgi_info_ptr,
	  	    sizeof(ps_iface_embms_tmgi_type),
                    (void*)&(ioctl_arg_ptr->tmgi_session_info),
                    sizeof(ps_iface_embms_tmgi_type));

            ds_put_cmd_ext(cmd_ptr);
          } /* mem allocation for TMGI info success */
          else
          {
            DS_EMBMS_MSG0_ERROR("Deact TMGI IOCTL:Can't get mem - deact ind");
          } /* mem allocation for Tmgi info failure */
    
        }/* If ref count is more than 1 */
        else if(ref_count == 1)
        {
          /*--------------------------------------------------------------------
            Ref count is one. There is only one client that is left for this 
            TMGI. Set the TMGI state to deactivating and send request to LTE RRC
            to deactivate the TMGI
          --------------------------------------------------------------------*/
          if(ds_3gpp_embms_rrcif_deactivate_tmgi(ioctl_arg_ptr, subs_id) != TRUE)
          {
            DS_EMBMS_MSG0_ERROR("Deact TMGI IOCTL Fail - can't send msg to RRC");

            ret_val = -1;
            *ps_errno = DS_EINVAL;
          }
          else
          {
            ds_3gpp_embms_tmgi_tbl_mgr_set_state
              (index, DS_3GPP_EMBMS_TMGI_STATE_DEACTIVATING, subs_id);
          }
        } /* ref count is one */
        else
        {
          DS_EMBMS_MSG1_ERROR("Invalid ref_count value: %d", ref_count);

          ret_val = -1;
          *ps_errno = DS_EINVAL;
        }

      }/* Client present in the TMGI table */ 

    } /* State allows operation */
    else
    {
      /*------------------------------------------------------------------------
        State is such that we cannot allow the operation. The IOCTL will be
        rejected in both cases. If the TMGI state is deactivating, the ps_errno
        will have CLOSEINPROGRESS.
      ------------------------------------------------------------------------*/
      DS_EMBMS_MSG1_ERROR("Cannot service IOCTL at this time. TMGI state: %d",
                          tmgi_state);
      if(tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_DEACTIVATING)
      {
        *ps_errno = DS_ENETCLOSEINPROGRESS;
      }
      else
      {
        *ps_errno = DS_EINVAL;
      }

    } /* TMGI state does not allow deactivation of the TMGI */

  } /* TMGI entry present */
  else
  {
    DS_EMBMS_MSG0_ERROR("TMGI not present in the table. Fail DEACT IOCTL");
    *ps_errno = DS_EINVAL;
  } /* TMGI entry not present */
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  return ret_val;
} /* ds_3gpp_embms_deactivate_tmgi_ioctl_hdlr */
/*==============================================================================

FUNCTION DS_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND_CMD

DESCRIPTION
    This function posts a command to DS task to send TMGI activate IND.
    Currently the command will be posted during either of the following two cases
    (1)  When the same Client attempts to activate the TMGI list, which has been
         already activated
    (2) When a new client attempts to activate the TMGI list that was already
        activated

DEPENDENCIES
  None.

RETURN VALUE
  TRUE      If the command has been posted successfully
  FALSE     Otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_send_tmgi_activated_ind_cmd
(
  ps_iface_embms_tmgi_act_req_type  *ioctl_arg_ptr,
  uint8                              index,
  sys_modem_as_id_e_type             subs_id
)
{
  ds_cmd_type                     *cmd_ptr = NULL;
  ds_3gpp_embms_tmgi_act_ind_info *act_ind_info_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*------------------------------------------------------------------------- 
    Validate arguments
  -------------------------------------------------------------------------*/
  if (ioctl_arg_ptr == NULL) 
  {
    DS_EMBMS_MSG0_ERROR("NULL ioctl arg ptr.Can't post TMGI_activated_Ind cmd");
    return FALSE;
  }

  if (!DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index))
  {
    DS_EMBMS_MSG0_ERROR("TMGI TBL Idx invalid.Can't post TMGI_activated_Ind cmd");
    return FALSE;
  }
  /*------------------------------------------------------------------------- 
    Request Command buffer
  -------------------------------------------------------------------------*/
  cmd_ptr = ds_allocate_cmd_buf(sizeof(ds_3gpp_embms_tmgi_act_ind_info));

  if( (cmd_ptr == NULL) || (cmd_ptr->cmd_payload_ptr == NULL) )
  {
    ASSERT(0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Fill in the command parameters
  ---------------------------------------------------------------------------*/
  cmd_ptr->hdr.cmd_id = DS_CMD_3GPP_EMBMS_SEND_TMGI_ACTIVATED_IND;
  act_ind_info_ptr = 
    (ds_3gpp_embms_tmgi_act_ind_info*)cmd_ptr->cmd_payload_ptr;

  act_ind_info_ptr->subs_id = subs_id;
  act_ind_info_ptr->ind_info.client_id = ioctl_arg_ptr->client_id;
  act_ind_info_ptr->ind_info.act_info_code = 
    IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS_INFO_DUPLICATE;
  act_ind_info_ptr->ind_info.debug_trace_id = 
                                        ioctl_arg_ptr->debug_trace_id;

  memscpy((void*)&(act_ind_info_ptr->ind_info.tmgi_session_info),
          sizeof(ps_iface_embms_tmgi_type),
          (void*)&(ioctl_arg_ptr->tmgi_session_info),
          sizeof(ps_iface_embms_tmgi_type));

  /*-------------------------------------------------------------------------
    Store the index as obtained here. This is needed to perform state
    check before sending the indication. There is always a chance that
    the TMGI gets deactivated before we process this command.

    The processing of this command should ensure that the TMGI is in
    the right state before sending activated indication. If not, just
    ignore the command
  ---------------------------------------------------------------------------*/
  act_ind_info_ptr->index = index;

  ds_put_cmd_ext(cmd_ptr);
  
  return TRUE;
  
}
/*==============================================================================

FUNCTION DS_3GPP_EMBMS_ACTIVATE_TMGI_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to activate a TMGI. Based on the TMGI
  state machine a request is sent to LTE RRC. It is possible that we need not go
  to RRC to service the request. DS may also reject the IOCTL in cases where it
  is not possible to activate a TMGI

  Note: 
  1. Currently the UE supports only 8 concurrent TMGI activation. DS does not
  block requests after 8 TMGIs are active. It will be passed on to LTE RRC which
  may choose to bring down a TMGI and activate the new one. There is also no
  dependency in the order in which the deactivation of an existing TMGI and 
  activation of a new TMGI happens.

  2. There is a limit to the number of TMGI state machines that can be created
  This is a limit in DS and is set to 16. Please look at the declaration of 
  ds_3gpp_embms_context_table to understand this rationale


DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL int ds_3gpp_embms_activate_tmgi_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type subs_id
)
{
  ps_iface_embms_tmgi_act_req_type *ioctl_arg_ptr = NULL;
  int                                    ret_val = -1;
  boolean                                tmgi_entry_present = FALSE;
  uint8                                  index;
  uint8                                  loop_index;
  ds_3gpp_embms_tmgi_state_e             tmgi_state = 
                                               DS_3GPP_EMBMS_TMGI_STATE_INVALID;  
  ds_3gpp_embms_tmgi_tbl_client_add_enum_type result_code = 
                                          DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_MAX;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(argval_ptr != NULL && ps_errno!= NULL);

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  /*----------------------------------------------------------------------------
    Typecast the incoming argval_ptr to the appropriate type
  ----------------------------------------------------------------------------*/
  ioctl_arg_ptr = (ps_iface_embms_tmgi_act_req_type*)argval_ptr;

  /*----------------------------------------------------------------------------
    Print out the request. Call the function to print out the info in a manner
    that is desirable. Print the client id passed in the request.
  ----------------------------------------------------------------------------*/
  DS_EMBMS_MSG3_HIGH("Act TMGI IOCTL HDLR: Client id: 0x%x, debug_trace_id: %d,"
                     " preempt_priority: %d", ioctl_arg_ptr->client_id, 
                                              ioctl_arg_ptr->debug_trace_id, 
                                              ioctl_arg_ptr->preempt_priority);

  DS_EMBMS_MSG0_HIGH("Act TMGI IOCTL HDLR: frequency list:");

  for(loop_index = 0; loop_index < ioctl_arg_ptr->num_earfcn_to_search; 
                                                                  loop_index ++)
  {
    DS_EMBMS_MSG1_HIGH(" %d", ioctl_arg_ptr->earfcn_list[loop_index]);
  }

  ds_3gpp_embms_print_tmgi_session_info(&(ioctl_arg_ptr->tmgi_session_info));

  /*----------------------------------------------------------------------------
    Logic as follows:
       Check state of TMGI
         If TMGI is not present
           Allocate entry - This stores TMGI and client info, increments ref cnt
           Send request to RRC
           Set TMGI state as activating if the rerequest was successful

         TMGI Already present
           Check state
             If ACTIVE or ACTIVATING - add client to list, send indication if
             TMGI is already active (post a command so that we send indication
             only after returning from the IOCTL).

             If other states - fail IOCTL.

       Any failure - fail IOCTL if recoverable. If not assert.
  ----------------------------------------------------------------------------*/
  if( (ioctl_arg_ptr->client_id == NULL) || 
      (ioctl_arg_ptr->client_id == (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID) )
  {
    DS_EMBMS_MSG0_ERROR("Invalid client id. Rejecting ACT TMGI IOCTL");
    *ps_errno =  DS_EFAULT;
    goto bail_out;
  } /* Validate Client id before proceeding */

  if (ioctl_arg_ptr->num_earfcn_to_search != 0 
      && ioctl_arg_ptr->num_sai_to_search != 0)
  {    
	/*--------------------------------------------------------------------
		When both SAI list and EARFCN list are specified by the Client.
		Pass through both the lists to RRC. RRC may decide to choose the 
		SAI (or) EARFCN list and report activation status
	  --------------------------------------------------------------------*/
	  DS_EMBMS_MSG1_HIGH("Client 0x%x has specified both EARFCN and SAI list",
						ioctl_arg_ptr->client_id);
  }

  /*----------------------------------------------------------------------------
    Check if there is an entry in the eMBMS TMGI context table that corresponds
      to this TMGI-Session Id Pair.
  ----------------------------------------------------------------------------*/
  tmgi_entry_present = 
    ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
      (&(ioctl_arg_ptr->tmgi_session_info), FALSE, &index, subs_id);

  if(tmgi_entry_present)
  {
    ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));
    /*--------------------------------------------------------------------------
      TMGI entry is present in the eMBMS TMGI context table. Now get the state
      of the TMGI. If the TMGI is in Activating or Active State, we can add the
      client to the list.
    --------------------------------------------------------------------------*/

    tmgi_state = ds_3gpp_embms_tmgi_tbl_mgr_get_state(index, subs_id);

    if( (tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_ACTIVE) ||
        (tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_ACTIVATING) )
    {
      /*------------------------------------------------------------------------
        Update the table entry with the client info (if client already exists 
        it's not added again)
      ------------------------------------------------------------------------*/

      result_code = ds_3gpp_embms_tmgi_tbl_mgr_add_client_to_list
                      (index, ioctl_arg_ptr->client_id, subs_id);

      if(DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_SUCCESS != result_code)
      {
        /*------------------------------------------------------------------------
          Set the ps_errno based on the result code. If the result code indicates
          duplicate activation of same TMGI by same client, fail ioctl.
         
          If failure is other than duplicate activation, set ps_errno as DS_EINVAL
        ------------------------------------------------------------------------*/
        
        if(result_code == DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_DUP)
        {
          /*------------------------------------------------------------------- 
            Same Client doing a duplicate activate TMGI for the TMGI list
            which already got activated. Send TMGI activate Indication
            with SUCCESS_INFO_DUPLICATE status
            -------------------------------------------------------------------*/
          if (!ds_3gpp_embms_send_tmgi_activated_ind_cmd
                (ioctl_arg_ptr,index, subs_id))
          {
            DS_EMBMS_MSG0_HIGH("Act TMGI  IOCTL: Sending TMGI Activated Ind "
                              "failed");
          }
          else
          {
            DS_EMBMS_MSG0_HIGH("Act TMGI  IOCTL: Sending TMGI "
                                 "Activated 'Dup Act' for same TMGI Client");

          }
	  ret_val = 0;
          *ps_errno = DS_ENOERR;
        }
        else
        {
          DS_EMBMS_MSG0_ERROR("Act TMGI  IOCTL: Could not update client info");
          *ps_errno = DS_EINVAL;
        }

      } /* Addition to the client list did not happen */
      else
      {
        if(tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_ACTIVE)
        {
          /*----------------------------------------------------------------------
            Post a command to generate Iface indication. This is just to ensure 
            we do not generate the iface indication before we respond to the IOCTL
            Also the command to generate the indication has to be posted only if 
            TMGI is already in Active state. If the TMGI is in activating state, 
            we will post indications to each client that is present in the list
          ----------------------------------------------------------------------*/
          if (!ds_3gpp_embms_send_tmgi_activated_ind_cmd
                (ioctl_arg_ptr,index, subs_id))
          {
            DS_EMBMS_MSG0_HIGH("Act TMGI  IOCTL: Sending TMGI Activated Ind "
                               "failed");            
            *ps_errno = DS_EINVAL;
            if(ds_3gpp_embms_tmgi_tbl_mgr_remove_client_from_list
                    (index,  ioctl_arg_ptr->client_id, subs_id) == FALSE)
            {
              ASSERT(0);
              goto bail_out;
            }
          }
          else
          {
            DS_EMBMS_MSG0_HIGH("Act TMGI  IOCTL: Sending TMGI "
                               "Activated 'Dup Act' for new TMGI Client");
            ret_val = 0;
            *ps_errno = DS_ENOERR;
          }

        }/* If TMGI is Active already*/

      }/* Successful update of the TMGI table */ 

    } /* State allows update */
    else
    {
      /*------------------------------------------------------------------------
        State is such that we cannot update the client list. The IOCTL will be
        rejected in both cases. If the TMGI state is deactivating, the ps_errno
        will have CLOSEINPROGRESS.
      ------------------------------------------------------------------------*/
      DS_EMBMS_MSG1_ERROR("Cannot service IOCTL at this time. TMGI state: %d",
                          tmgi_state);
      if(tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_DEACTIVATING)
      {
        *ps_errno = DS_ENETCLOSEINPROGRESS;
      }
      else
      {
        *ps_errno = DS_EINVAL;
      }

    } /* TMGI state does not allow activation of the TMGI */

  } /* TMGI entry present */
  else
  {
    DS_EMBMS_MSG0_HIGH("New request to activate TMGI");
    /*--------------------------------------------------------------------------
      There is no entry corresponding to this TMGI. Create one, update the table
      entry and then send a request to LTE RRC.
    --------------------------------------------------------------------------*/
    if(
       ds_3gpp_embms_tmgi_tbl_mgr_allocate_entry
         ( &(ioctl_arg_ptr->tmgi_session_info), ioctl_arg_ptr->client_id, 
           &index, subs_id) == TRUE)
    {
      ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));

      DS_EMBMS_MSG1_MED("Allocated table entry for the TMGI. Index: %d",
                        index);

      /*------------------------------------------------------------------------
        Send message to LTE RRC - Call the function into RRCIF module
      ------------------------------------------------------------------------*/
      if(ds_3gpp_embms_rrcif_activate_tmgi(ioctl_arg_ptr, subs_id) == TRUE)
      {
        DS_EMBMS_MSG0_HIGH("Msg to activate TMGI sent to RRC");
        /*----------------------------------------------------------------------
          Message sent successfully to LTE RRC. Change the state of the TMGI to
          Activating
        ----------------------------------------------------------------------*/
        if(ds_3gpp_embms_tmgi_tbl_mgr_set_state
             (index, DS_3GPP_EMBMS_TMGI_STATE_ACTIVATING, subs_id) != TRUE)
        {
          DS_EMBMS_MSG1_ERROR("Act TMGI IOCTL: Couldn't set state - index: %d",
                              index);

          /*--------------------------------------------------------------------
            This is not expected to happen. Also if we free the table entry and
            later drop the response from RRC, there will be a disconnect between
            DS and RRC. Hence ASSERT.
          --------------------------------------------------------------------*/
          ASSERT(0);
          *ps_errno = DS_EINVAL;
        } /* Could not set TMGI state in the TMGI table*/
        else
        {
          ret_val = 0;
          *ps_errno = DS_ENOERR;
        }
        
      } /* Message sent successfully to LTE RRC*/
      else
      {
        /*----------------------------------------------------------------------
          Remove the entry added in the table as the activate request was not
          sent to LTE RRC.
        ----------------------------------------------------------------------*/
        ds_3gpp_embms_tmgi_tbl_mgr_remove_entry(index, subs_id);
        *ps_errno = DS_NOMEMORY;
      }
    } /* Successful allocation in table */
    else
    {
      DS_EMBMS_MSG0_ERROR("DS 3GPP eMBMS IOCTL: Couldn't alloc TMGI tbl entry");
      *ps_errno = DS_EINVAL;
    }/* Allocation failed */
  } /* no existing entry in TMGI table*/

bail_out:
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  return ret_val;

} /* ds_3gpp_embms_activate_tmgi_ioctl_hdlr */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_ACT_DEACT_TMGI_IOCTL_HDLR

DESCRIPTION
  This function is used to simultaneously activate/deactivate TMGIs. TMGI to be
  activated must not have been activated before. TMGI to be deactivated must be
  activated before. 
  A message router packet is populated and sent to LTE RRC.

PARAMETERS
  tmgi_req_ptr - Pointer to the TMGI 
  subs_id      - Subscription Id

DEPENDENCIES  
  None

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL int ds_3gpp_embms_act_deact_tmgi_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
)
{
  ps_iface_embms_tmgi_act_deact_req_type *ioctl_arg_ptr = NULL;
  int                                    ret_val = -1;
  boolean                                tmgi_entry_present = FALSE;
  uint8                                  act_index;
  uint8                                  deact_index;
  uint8                                  loop_index;
  ds_3gpp_embms_client_info_type         *client_ptr = NULL;
  ds_3gpp_embms_tmgi_state_e             tmgi_state = 
                                               DS_3GPP_EMBMS_TMGI_STATE_INVALID;
  list_type                              *client_list;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ASSERT(argval_ptr != NULL && ps_errno!= NULL);

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  /*----------------------------------------------------------------------------
    Typecast the incoming argval_ptr to the appropriate type
  ----------------------------------------------------------------------------*/
  ioctl_arg_ptr = (ps_iface_embms_tmgi_act_deact_req_type*)argval_ptr;

  /*----------------------------------------------------------------------------
    Print out the request. Call the function to print out the info in a manner
    that is desirable. Print the client id passed in the request.
  ----------------------------------------------------------------------------*/
  DS_EMBMS_MSG3_HIGH("Act/Deact TMGI IOCTL HDLR: Client id: 0x%x, debug_trace_id: %d,"
                     " preempt_priority: %d", ioctl_arg_ptr->client_id, 
                                              ioctl_arg_ptr->debug_trace_id, 
                                              ioctl_arg_ptr->preempt_priority);
  
  DS_EMBMS_MSG0_MED("Act/Deact TMGI IOCTL HDLR: frequency list:");

  for(loop_index = 0; loop_index < ioctl_arg_ptr->num_earfcn_to_search; 
                                                                  loop_index ++)
  {
    DS_EMBMS_MSG1_MED(" %d", ioctl_arg_ptr->earfcn_list[loop_index]);
  }
  DS_EMBMS_MSG0_HIGH("Act/Deact TMGI IOCTL HDLR: Act TMGI: ");
  ds_3gpp_embms_print_tmgi_session_info(&(ioctl_arg_ptr->act_tmgi_session_info));
  DS_EMBMS_MSG0_HIGH("Act/Deact TMGI IOCTL HDLR: Deact TMGI: ");
  ds_3gpp_embms_print_tmgi_session_info(&(ioctl_arg_ptr->act_tmgi_session_info));

  if( (ioctl_arg_ptr->client_id == NULL) || 
      (ioctl_arg_ptr->client_id == (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID) )
  {
    DS_EMBMS_MSG0_ERROR("Invalid client id. Rejecting ACT TMGI IOCTL");
    *ps_errno = DS_EFAULT;
    goto bail_out;
  } /* Validate Client id before proceeding */

  if(memcmp(&(ioctl_arg_ptr->act_tmgi_session_info),
      &(ioctl_arg_ptr->deact_tmgi_session_info), sizeof(ps_iface_embms_tmgi_type))
                                                                            == 0)
  {
    DS_EMBMS_MSG0_ERROR("Activation/Deactivating same TMGI. "
                        "Rejecting IOCTL");
    *ps_errno = DS_EINVAL;
    goto bail_out;
  }

  /*---------------------------------------------------------------------------- 
   	Check that the TMGI to be activated is not already active for the client. If
   	so, fail the IOCTL.
  ----------------------------------------------------------------------------*/
  tmgi_entry_present = 
    ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
      (&(ioctl_arg_ptr->act_tmgi_session_info), FALSE, &act_index, subs_id);
  
  if(tmgi_entry_present)
  {
    tmgi_state = ds_3gpp_embms_tmgi_tbl_mgr_get_state(act_index, subs_id);

    if(tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_DEACTIVATING)
    {
      DS_EMBMS_MSG1_ERROR("Cannot service IOCTL at this time. TMGI state: %d",
                          tmgi_state);
      *ps_errno = DS_ENETCLOSEINPROGRESS;
      goto bail_out;
    }
            
	/* Match client IDs */
    client_list = ds_3gpp_embms_tmgi_tbl_mgr_get_list(act_index, subs_id);
    if (NULL != client_list)
    {
      client_ptr = (ds_3gpp_embms_client_info_type*) list_peek_front(
                               client_list);
    }
    while(client_ptr != NULL)
    {
      if(ioctl_arg_ptr->client_id == client_ptr->user_data_ptr)
	    {
        DS_EMBMS_MSG1_ERROR("Act/Deact TMGI IOCTL HDLR: Act TMGI already active"
		            " for client %d", client_ptr->user_data_ptr);
         *ps_errno = DS_EINPROGRESS;
         goto bail_out;
	    }
      client_ptr = list_peek_next(client_list, &(client_ptr->link));
    } /* while client_ptr != NULL */
  }

  /*---------------------------------------------------------------------------- 
    rraghave: The following logic works for single client only.
    1) Check if TMGI to be deactivated is already active/activating. If not, we
   	fail the call.
   	2) Ref count for the TMFI to be deactivated is 1. We do not check if client_id
	matches with the client that activated the TMGI (this check needs to be added)
  ----------------------------------------------------------------------------*/
  tmgi_entry_present = 
       ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
         (&(ioctl_arg_ptr->deact_tmgi_session_info), FALSE, 
          &deact_index, subs_id);

  if (tmgi_entry_present)
  {
    tmgi_state = ds_3gpp_embms_tmgi_tbl_mgr_get_state(deact_index, subs_id);
  }  

  if( !tmgi_entry_present 
     || ((tmgi_state != DS_3GPP_EMBMS_TMGI_STATE_ACTIVE) 
         && (tmgi_state != DS_3GPP_EMBMS_TMGI_STATE_ACTIVATING)))	 
  {
    DS_EMBMS_MSG0_ERROR("Act/Deact TMGI IOCTL HDLR: Deact TMGI state is not "
                        "active/activating"); 
    if(tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_DEACTIVATING)
    {
      *ps_errno = DS_ENETCLOSEINPROGRESS;
    }
    else
    {
      *ps_errno = DS_EINVAL;
    }    
    goto bail_out;
  }

  if(ds_3gpp_embms_tmgi_tbl_mgr_get_ref_count(deact_index, subs_id) != 1)
  {
    DS_EMBMS_MSG0_ERROR("Act/Deact TMGI IOCTL HDLR: Deact TMGI ref count != 1"); 
    *ps_errno = DS_EINVAL;   
    goto bail_out;
  }

  if(! ds_3gpp_embms_tmgi_tbl_mgr_is_client_in_list
         (deact_index, ioctl_arg_ptr->client_id, subs_id))
  {
    DS_EMBMS_MSG0_ERROR("Act Deact TMGI IOCTL: client: 0x%x not present. Fail");
    *ps_errno = DS_EOPNOTSUPP;     
    goto bail_out; 
  }/* Client id not present in client list */

  /*------------------------------------------------------------------------
   Allocate entry for TMGI Act request, if not already activated. Otherwise
   increment the ref count for the TMGI.
  ------------------------------------------------------------------------*/
  if(act_index == DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE)
  {
  	if( ds_3gpp_embms_tmgi_tbl_mgr_allocate_entry(
              &(ioctl_arg_ptr->act_tmgi_session_info),
               ioctl_arg_ptr->client_id,
               &act_index, subs_id ) == FALSE)
  	{
  	  DS_EMBMS_MSG0_ERROR("Act/Deact TMGI IOCTL HDLR: TMGI table entry "
  			      "allocation failed");
  	  *ps_errno = DS_ENOMEM;
  	  goto bail_out;
  	}    
    
  }
  else
  {
    ds_3gpp_embms_tmgi_tbl_mgr_add_client_to_list
      (act_index, ioctl_arg_ptr->client_id, subs_id);
  }
    
  DS_EMBMS_MSG1_MED("Allocated table entry for the TMGI. Index: %d",
                    act_index);

  /*------------------------------------------------------------------------
    Send message to LTE RRC - Call the function into RRCIF module
  ------------------------------------------------------------------------*/
  if(ds_3gpp_embms_rrcif_act_deact_tmgi(ioctl_arg_ptr, subs_id) == FALSE)
  {
    DS_EMBMS_MSG0_ERROR("Could not send activate/deactive TMGI msg to RRC");
    goto cleanup_bail_out;
  }
    
  /*------------------------------------------------------------------------
    Update state for TMGIs
  ------------------------------------------------------------------------*/
  ds_3gpp_embms_tmgi_tbl_mgr_set_state
    (deact_index, DS_3GPP_EMBMS_TMGI_STATE_DEACTIVATING, subs_id);

  if(ds_3gpp_embms_tmgi_tbl_mgr_get_state(act_index, subs_id)
                                     != DS_3GPP_EMBMS_TMGI_STATE_ACTIVE)
  {
    ds_3gpp_embms_tmgi_tbl_mgr_set_state
      (act_index, DS_3GPP_EMBMS_TMGI_STATE_ACTIVATING, subs_id);
  }
  
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  *ps_errno = DS_ENOERR;
  return 0; /*success */

cleanup_bail_out:
  ds_3gpp_embms_tmgi_tbl_mgr_remove_entry(act_index, subs_id);
bail_out:
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  return ret_val;

} /* ds_3gpp_embms_act_deact_tmgi_ioctl_hdlr */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_CONTENT_DESC_UPDATE_IOCTL_HDLR

DESCRIPTION
  This function is used to pass the content descovery update information from
  AP to LTE RLC

DEPENDENCIES  
  None

RETURN VALUE
  0 on success
  -1 on failure

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL int ds_3gpp_embms_content_desc_update_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
)
{
  int                                      ret_val = -1; 
  ps_iface_embms_content_desc_update_type *ioctl_arg_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(argval_ptr == NULL || ps_errno == NULL)
  {
    DS_EMBMS_MSG0_ERROR("ds_3gpp_embms_content_desc_update_ioctl_hdlr "
                        "NULL ptr passed");
    if(ps_errno != NULL)
    {
      *ps_errno = DS_EFAULT;
    }
    return ret_val;
  }

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  ioctl_arg_ptr = (ps_iface_embms_content_desc_update_type*)argval_ptr;

  /* content of argval_ptr cannot be 0 */
  if (ioctl_arg_ptr->size <= 0)
  {
    DS_EMBMS_MSG0_ERROR("embms content desc update. Invalid size passed");
    *ps_errno = DS_EFAULT;
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
    return ret_val;
  }

  if (ds_3gpp_embms_rrcif_content_desc_update(ioctl_arg_ptr, subs_id) == TRUE)
  {
    DS_EMBMS_MSG0_HIGH("Send DS_LTE_EMBMS_CONTENT_DESC_UPDATE_REQ to RLC");
    *ps_errno = DS_ENOERR;
    ret_val = 0;
  }
  else
  {
    DS_EMBMS_MSG0_ERROR("Failed to send DS_LTE_EMBMS_CONTENT_DESC_UPDATE_REQ");
    *ps_errno = DS_EFAULT;
    ret_val = -1;
  }

  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  return ret_val;
} /* ds_3gpp_embms_content_desc_update_ioctl_hdlr */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_PRINT_TMGI_SESSION_INFO

DESCRIPTION
  This function is used to print the TMGI_session info that is present in a 
  request or a response. We need to convert the TMGI string into uint64 so that
  we can display the hex value that is passed in.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void ds_3gpp_embms_print_tmgi_session_info
(
  ps_iface_embms_tmgi_type *tmgi_session_info_ptr
)
{
  uint32 plmn_id = 0;
  uint32 service_id = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  memscpy((void*)&service_id, sizeof(service_id), (void*)tmgi_session_info_ptr->tmgi, 
           DS_3GPP_EMBMS_RRCIF_SVC_ID_SIZE);

  memscpy((void*)&plmn_id, sizeof(plmn_id),
         (void*)(tmgi_session_info_ptr->tmgi+DS_3GPP_EMBMS_RRCIF_PLMN_OFFSET), 
         DS_3GPP_EMBMS_RRCIF_PLMN_ID_SIZE);

  DS_EMBMS_MSG3_HIGH("TMGI info - Service Id: 0x%x, PLMN Id: 0x%x Session: %d",
                     service_id, plmn_id, 
                     (tmgi_session_info_ptr->session_id_present == TRUE)?
                     tmgi_session_info_ptr->session_id:0xFF);

  return;
} /* ds_3gpp_embms_print_tmgi_session_info*/


/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_SEND_IND

DESCRIPTION
  This function is used to send the TMGI/SAI list indications. This is used to send
  active  TMGI lists, available TMGI lists and available SAI lists

PARAMETERS
  event        - type of the event
  ind_info_ptr - Pointer to the indication
  subs_id      - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_send_ind
(
  ps_iface_event_enum_type  event, 
  void                      *ind_info_ptr,
  sys_modem_as_id_e_type    subs_id
)
{
  ps_iface_type                *iface_ptr = NULL;
  ds_3gpp_embms_ctrl_blk_type  *ctrl_blk_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ASSERT(ind_info_ptr != NULL);

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  /*----------------------------------------------------------------------------
    Get the iface pointer
  ----------------------------------------------------------------------------*/
  iface_ptr = &(ctrl_blk_p->ps_iface);

  /*----------------------------------------------------------------------------
    Post the indication
  ----------------------------------------------------------------------------*/
  ps_iface_generic_ind(iface_ptr, event, (void*)ind_info_ptr);

} /* ds_3gpp_embms_send_tmgi_list_ind */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GET_ACTIVE_TMGI_LIST_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to retrieve the list of TMGIs that have
  been activated. This function simply loops through the DS TMGI table and
  returns the TMGIs that are in active state.
 
  In this function we will get the critical section that protects access
  to the TMGI table. As a result, only one client's IOCTL invocation can
  be processed at a time.

DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_get_active_tmgi_list_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
)
{
  int                                    ret_val = -1;
  
  boolean                                send_indication_flag = TRUE;
  ps_iface_embms_tmgi_related_query_type *ioctl_arg_ptr = NULL;
  ps_iface_embms_tmgi_list_info_type     ind_info;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(argval_ptr != NULL && ps_errno!= NULL)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  
    /*----------------------------------------------------------------------------
      Typecast the incoming argval_ptr to the appropriate type
    ----------------------------------------------------------------------------*/
    ioctl_arg_ptr = (ps_iface_embms_tmgi_related_query_type*)argval_ptr;

    /*--------------------------------------------------------------------------
      Memset the indication
    --------------------------------------------------------------------------*/
    memset(&ind_info, 0, sizeof(ind_info));

    ind_info.client_id = ioctl_arg_ptr->client_id;
    ind_info.debug_trace_id = ioctl_arg_ptr->debug_trace_id;    
    
     DS_EMBMS_MSG2_HIGH("ds_3gpp_embms_get_active_tmgi_list_ioctl_hdlr:"
                        " Client id: 0x%x, debug_trace_id: 0x%x",
                        ioctl_arg_ptr->client_id, ioctl_arg_ptr->debug_trace_id);


    if(ds_3gpp_embms_tmgi_tbl_mgr_fill_act_tmgi_list
         (&ind_info, subs_id) == FALSE)
    {
      DS_EMBMS_MSG0_ERROR("Modem mem alloc failed. Not sending ind");
      send_indication_flag = FALSE;
      *ps_errno = DS_ENOMEM;
    } /* memory allocation successful */
    
    if(send_indication_flag == TRUE)
    {
      /*----------------------------------------------------------------------
        Send the indication
      ----------------------------------------------------------------------*/
      ds_3gpp_embms_send_ind(IFACE_EMBMS_ACTIVE_TMGI_LIST_EV,
                             (void*)&ind_info, subs_id);

      if(ind_info.tmgi_list.list != NULL)
      {
        /*----------------------------------------------------------------------
          Free the memory that was use for the ACT TMGI list
        ----------------------------------------------------------------------*/
        modem_mem_free((void*)ind_info.tmgi_list.list,MODEM_MEM_CLIENT_DATA);
      }
    } //send indication flag is true

    /*--------------------------------------------------------------------------
      No other errors are possible. Set retval and ps_errno to No Error
    --------------------------------------------------------------------------*/
    ret_val = 0;
    *ps_errno = DS_ENOERR;
  
    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  } // valid arguments
  else
  {
    DS_EMBMS_MSG2_ERROR("Invalid IOCTL param- argval_ptr: 0x%x, ps_errno: 0x%x",
                        argval_ptr,ps_errno);    
  }

  return ret_val;
} /* ds_3gpp_embms_get_active_tmgi_list_ioctl_hdlr */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GET_SAI_LIST_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to retrieve the list of SAIs.
  To get this information, we always go to RRC module.
 
  Currently this operation is supported for only one client at a time.
  If there is a need, this will need to be expanded for multiple clients
  and when the response from RRC reaches DS, Iface indications will be 
  sent out to all clients in this list.

DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_get_sai_list_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
)
{
  int                               ret_val         = -1;
  ps_iface_embms_sai_list_req_type* ioctl_arg_ptr = NULL;
  boolean                           is_mdm_orig     = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(argval_ptr == NULL || ps_errno== NULL)
  {
    DS_EMBMS_MSG2_ERROR("Invalid IOCTL param- argval_ptr: 0x%x, ps_errno: 0x%x",
                        argval_ptr,ps_errno);
    return ret_val;    
  }

  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  if(ds_3gpp_embms_sai_list_client != NULL)
  {
    DS_EMBMS_MSG1_HIGH("IOCTL unserviceable. Awaiting RRC msg for clnt:0x%x",
                        ds_3gpp_embms_sai_list_client);
    *ps_errno = DS_EOPNOTSUPP;
  }
  else
  {
    /*------------------------------------------------------------------------
      Typecast the incoming argval_ptr to the appropriate type
    ------------------------------------------------------------------------*/
    ioctl_arg_ptr = (ps_iface_embms_sai_list_req_type *)argval_ptr;

    DS_EMBMS_MSG2_HIGH("Get SAI List IOCTL. Client: 0x%x, debug_trace_id: %d", 
                       ioctl_arg_ptr->client_id, ioctl_arg_ptr->debug_trace_id);
    /*------------------------------------------------------------------------
      Cache the client id from the request indicating that we are sending a
      request to RRC
    ------------------------------------------------------------------------*/
  
    ds_3gpp_embms_sai_list_client = ioctl_arg_ptr->client_id;
    
    /*------------------------------------------------------------------------
      Send the message to RRC.
    ------------------------------------------------------------------------*/
    if(ds_3gpp_embms_rrcif_get_sai_list(ioctl_arg_ptr,is_mdm_orig, subs_id) 
         ==TRUE)
    {
      DS_EMBMS_MSG0_HIGH("Message sent to RRC to get avail SAI list");
      ret_val = 0;
      *ps_errno = DS_ENOERR;
    }
    else
    {
      DS_EMBMS_MSG0_ERROR("Unable to send msg to LTE RRC to get avail SAI list");
      ds_3gpp_embms_sai_list_client = NULL;
      *ps_errno = DS_EINVAL;
    }
  }
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  return ret_val;
} /* ds_3gpp_embms_get_sai_list_ioctl_hdlr */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GENERATE_EVENTS_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to retrieve the list of all available events
  from RRC for which QMI will to broadcast to all the clients
  This IOCTL Handler is invoked soon after EMBMS iface is brought up
 
DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
int ds_3gpp_embms_generate_events_ioctl_hdlr
(           
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
)
{
  int                               ret_val               = -1;
  ps_iface_embms_generate_events_info_type *ioctl_arg_ptr = NULL;
  ps_iface_embms_sai_list_req_type  sai_list_query;    
  boolean                           is_mdm_orig           = TRUE;
  const int16                       DEBUG_TRACE_ID        = -1;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /*-------------------------------------------------------------------------
    Query for all the supported available events to RRC 
    -------------------------------------------------------------------------*/

  if((argval_ptr == NULL) ||( ps_errno == NULL))
  {
    DS_EMBMS_MSG2_ERROR("Invalid IOCTL param- argval_ptr: 0x%x, ps_errno: 0x%x",
                        argval_ptr,ps_errno);
    return ret_val;
  }
  /*============================================= 
    GET AVAILABLE SAI LIST
    =============================================*/
  ioctl_arg_ptr     = (ps_iface_embms_generate_events_info_type*)argval_ptr;
  switch(ioctl_arg_ptr->embms_event)
  {
    case PS_IFACE_EMBMS_GENERATE_SAI_LIST_EV:
    /*-----------------------------------------------------------------------
      send SAI_LIST_REQ
      -----------------------------------------------------------------------*/
      sai_list_query.client_id      = NULL;
      sai_list_query.debug_trace_id = DEBUG_TRACE_ID;

      if (ds_3gpp_embms_rrcif_get_sai_list(&sai_list_query,is_mdm_orig, subs_id)
            == TRUE)
      {
        DS_EMBMS_MSG0_HIGH("Message sent to RRC to get avail SAI list");
        ret_val = 0;
        *ps_errno = DS_ENOERR;
      }
      else
      {
        DS_EMBMS_MSG0_ERROR("Unable to send msg to LTE RRC.Query SAI list failed");
        *ps_errno = DS_ENOMEM;      
      }
      ret_val = 0;
      break;
    default:
      *ps_errno = DS_EOPNOTSUPP;
      DS_EMBMS_MSG1_ERROR("Event %d not supported",ioctl_arg_ptr->embms_event);
      break;
  }
  return ret_val;
}

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GET_AVAIL_TMGI_LIST_IOCTL_HDLR

DESCRIPTION
  This function handles the ioctl to retrieve the list of TMGIs that are avail
  to the user. To get this information, we always go to RRC module.
 
  There is always a possibility that another client may request the available
  list while we are waiting for RRC to respond. We simply add client info to a
  list and respond to the client when RRC responds. We do not go to RRC with a
  second request while the first one is pending.

DEPENDENCIES
  None.

RETURN VALUE
  0  - on success
  -1 - on failure

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL int ds_3gpp_embms_get_avail_tmgi_list_ioctl_hdlr
(
  void                   *argval_ptr,
  sint15                 *ps_errno,
  sys_modem_as_id_e_type  subs_id
)
{
  int                                     ret_val = -1;
  ps_iface_embms_tmgi_related_query_type* ioctl_arg_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(argval_ptr != NULL && ps_errno!= NULL)
  {
    DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

    if(ds_3gpp_embms_avail_list_client != NULL)
    {
      DS_EMBMS_MSG1_HIGH("IOCTL unserviceable. Awaiting RRC msg for clnt:0x%x",
                          ds_3gpp_embms_avail_list_client);
      *ps_errno = DS_EOPNOTSUPP;
    }
    else
    {
      /*------------------------------------------------------------------------
        Typecast the incoming argval_ptr to the appropriate type
      ------------------------------------------------------------------------*/
      ioctl_arg_ptr = (ps_iface_embms_tmgi_related_query_type*)argval_ptr;

      DS_EMBMS_MSG2_HIGH("Available TMGI List IOCTL. Client: 0x%x, debug_trace_id: %d", 
                         ioctl_arg_ptr->client_id, ioctl_arg_ptr->debug_trace_id);
    
      /*------------------------------------------------------------------------
        Cache the client id from the request indicating that we are sending a
        request to RRC
      ------------------------------------------------------------------------*/
      ds_3gpp_embms_avail_list_client = ioctl_arg_ptr->client_id;

      /*------------------------------------------------------------------------
        Send the message to RRC.
      ------------------------------------------------------------------------*/
      if(ds_3gpp_embms_rrcif_get_avail_tmgi_list(ioctl_arg_ptr, subs_id) == TRUE)
      {
        DS_EMBMS_MSG0_HIGH("Message sent to RRC to get avail tmgi list");
        ret_val = 0;
        *ps_errno = DS_ENOERR;
      }
      else
      {
        DS_EMBMS_MSG0_ERROR("Unable to send msg to LTE RRC to get avail list");
        *ps_errno = DS_EINVAL;
      }
      
    } //can service the IOCTL

    DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);
  } //valid incoming arguments
  else
  {
    DS_EMBMS_MSG2_ERROR("Invalid IOCTL param- argval_ptr: 0x%x, ps_errno: 0x%x",
                        argval_ptr,ps_errno);
  }

  return ret_val;
} /* ds_3gpp_embms_get_avail_tmgi_list_ioctl_hdlr */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_GET_AVAIL_TMGI_LIST_CLIENT

DESCRIPTION
  This function is used to get the client id that is waiting for the available
  TMGI list indication

PARAMETERS
  NONE
  
DEPENDENCIES  
  None

RETURN VALUE
  void* - the client id that is waiting for the Available list

SIDE EFFECTS  
  None
==============================================================================*/
void* ds_3gpp_embms_get_avail_tmgi_list_client(void)
{
  return ds_3gpp_embms_avail_list_client;
} /* ds_3gpp_embms_get_avail_tmgi_list_client */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_GET_AVAIL_SAI_LIST_CLIENT

DESCRIPTION
  This function is used to get the client id that is waiting for the available
  SAI list indication

PARAMETERS
  NONE
  
DEPENDENCIES  
  None

RETURN VALUE
  void* - the client id that is waiting for the Available list

SIDE EFFECTS  
  None
==============================================================================*/
void* ds_3gpp_embms_get_avail_sai_list_client(void)
{
  return ds_3gpp_embms_sai_list_client;
} /* ds_3gpp_embms_get_avail_sai_list_client */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RESET_AVAIL_TMGI_LIST_CLIENT

DESCRIPTION
  This function is used to clear the client id that is waiting for the available
  TMGI list indication

PARAMETERS
  NONE
  
DEPENDENCIES  
  None

RETURN VALUE
  NONE

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_reset_avail_list_tmgi_client(void)
{
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  ds_3gpp_embms_avail_list_client = NULL;

  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  return;
} /* ds_3gpp_embms_reset_avail_list_tmgi_client */



/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RESET_AVAIL_SAI_LIST_CLIENT

DESCRIPTION
  This function is used to clear the client id that is waiting for the available
  SAI list indication

PARAMETERS
  NONE
  
DEPENDENCIES  
  None

RETURN VALUE
  NONE

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_reset_avail_sai_list_client(void)
{
  DS_3GPP_ENTER_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  ds_3gpp_embms_sai_list_client = NULL;
  
  DS_3GPP_LEAVE_CRIT_SECTION(&ds_3gpp_embms_crit_sect);

  return;
} /* ds_3gpp_embms_reset_avail_sai_list_client */

/*==============================================================================

FUNCTION      ds_3gpp_embms_tlbmc_activate_cb

DESCRIPTION
  This function is used to activate TLB mode C and return an int

PARAMETERS
  subs_id      - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  int:0 success
      -1 fail

SIDE EFFECTS  
  None
==============================================================================*/
int ds_3gpp_embms_tlbmc_activate_cb
(
  sys_modem_as_id_e_type subs_id
)
{
  
  ps_iface_type               *this_iface_ptr = NULL;
  int16                        ps_errno = 0;
  int                          result=0;
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((ctrl_blk_p = ds_3gpp_embms_find_or_allocate_ctrl_blk(subs_id)) == NULL)
  {
    DS_EMBMS_MSG0_HIGH("Could not allocate EMBMS Cntrl Block");
    return (-1);
  }

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return -1;
  }

  this_iface_ptr = &(ctrl_blk_p->ps_iface);

  PS_IFACE_SET_SUBS_ID(this_iface_ptr, ds3gsubsmgr_subs_id_cm_to_ds(subs_id));

  result= ps_iface_bring_up_cmd(this_iface_ptr,&ps_errno,NULL) ;

  if(0 == result )
  {
    /*------------------------------------------------------------------------
    If we are in TLB Mode C we dont need to setup the bridge with DPM, so 
    we break the bridge because it a loopback mode 
    -------------------------------------------------------------------------*/
    if (ctrl_blk_p->is_registered_with_dpm == TRUE)
    {
      ds_3gpp_embms_dereg_wm_with_dpm(ctrl_blk_p);
    }
    ds_3gpp_embms_set_up_rlc_to_ps_wm(ctrl_blk_p);
  }
  else
  {
    if(ps_errno == DS_EWOULDBLOCK)
    {
      DS_EMBMS_MSG0_HIGH("EMBMS Iface bring up in progress");
    }
    else
    {
      DS_EMBMS_MSG0_HIGH("Could not bring up Embms Iface");
    }
  }
  return result; 

} /* ds_3gpp_embms_tlbmc_activate_cb */

/*==============================================================================
FUNCTION      ds_3gpp_embms_tlbmc_close_cb

DESCRIPTION
  This function is used to close TLB mode C and return an int

PARAMETERS
  subs_id      - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
 int:0 success
      -1 fail

SIDE EFFECTS  
  None
==============================================================================*/
boolean  ds_3gpp_embms_tlbmc_close_cb
( 
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = NULL;
  ps_iface_state_enum_type  iface_state = IFACE_DOWN;
  /*--------------------------------------------------------------------------*/

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return FALSE;
  }

  /*------------------------------------------------------------------------
     Change to make sure the TLB close ready indication is posted only after 
     EMBMS IFACE moved to UP state. This is to avoid the signal handler to be
     over-written later by IFACE UP handler command to drop the packets.
  -------------------------------------------------------------------------*/

  iface_state =  ps_iface_state(&(ctrl_blk_p->ps_iface));
  if( ( IFACE_UP ==  iface_state) || ( IFACE_ROUTEABLE == iface_state) )
  {
    /*Setting signal Handler*/
    DS_EMBMS_MSG0_HIGH("enabled signal handler and set modeC cb, return TRUE");
   (void)ps_set_sig_handler(
                          ctrl_blk_p->rx_sig,
                          ds_3gpp_embms_modeC_rx_um_data_cb, 
                          (void*)ctrl_blk_p);
    /*-------------------------------------------------------------------------  
       Enable the signal
    -------------------------------------------------------------------------*/
    ps_enable_sig(ctrl_blk_p->rx_sig);
    return TRUE;
  }
  else
  {
    /* As part of IFACE UP Indication this function will be called
       again */
    DS_EMBMS_MSG0_HIGH("IFACE is not in UP/routable state, return FALSE");
    return FALSE;
  }

}/* ds_3gpp_embms_tlbmc_close_cb */

/*=============================================================================

FUNCTION      ds_3gpp_embms_modeC_rx_um_data_cb

DESCRIPTION
  This function is used to close TLB mode C and return an boolean

PARAMETERS
  
  
DEPENDENCIES  
  None

RETURN VALUE
 boolean :TRUE Success
          FALSE Failure

SIDE EFFECTS  
  None
=============================================================================*/
boolean ds_3gpp_embms_modeC_rx_um_data_cb
(
  ps_sig_enum_type sig,
  void             *user_data_p
)
{
  dsm_item_type               *item_ptr;          /* Pointer to DSM item         */
  ds_3gpp_embms_wm_s          *rx_watermark_ptr;
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = NULL;
  boolean                      result = FALSE;
  /*-----------------------------------------------------------------------------*/

  ctrl_blk_p = (ds_3gpp_embms_ctrl_blk_type*)user_data_p;

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return TRUE;
  }

  /*-----------------------------------------------------------------
     Below check is to find sceanrio where WM count is zero, 
     then no signal handling is required, return TRUE to PS 
     signal handler 
  -------------------------------------------------------------------*/
  if(dsm_queue_cnt(&(ctrl_blk_p->rx_wm.wm_item)) == 0)
  {
    return TRUE;
  }

  /*---------------------------------------------------------------------------
    Get the rx watermark used in eMBMS
  ---------------------------------------------------------------------------*/
  rx_watermark_ptr = &(ctrl_blk_p->rx_wm);

  /* Return for invalid scenarios */
  if (ctrl_blk_p->l2_wm_set_up == FALSE)
  {
    DS_EMBMS_MSG0_ERROR("Not registered with RLC");
    return TRUE;
  }

  do
  {
    /*-------------------------------------------------------------------------
      Dequeue data from the Rx WM
    -------------------------------------------------------------------------*/
    item_ptr = (dsm_item_type *)dsm_dequeue(&(rx_watermark_ptr->wm_item));

    if(item_ptr != NULL)
    {
      /*-----------------------------------------------------------------------
        If TLB is closed in mode C, call the API to do packet counting. 
        Otherwise, in success or failure case,LTE TLB layer will free packet.
      -----------------------------------------------------------------------*/
      result = lte_tlb_mode_c_packet_count(&item_ptr); 
      if(result == FALSE )
      {
        DS_EMBMS_MSG0_ERROR("lte_tlb_mode_c_packet_count(), failed to increment");
      }
    }
    
  } while (item_ptr != NULL); //for all DSM items in the WM
  /*-----------------------------------------------------------------------
     Loop one more time to check if any data is present, by returning FALSE
     to PS signal handler. This is to avoid signal clearing in case of 
     race condition between PS and LTE task. 
   ----------------------------------------------------------------------*/

  return FALSE;
}/* ds_3gpp_embms_modeC_rx_um_data_cb */

/*==============================================================================

FUNCTION      ds_3gpp_embms_tlbmc_deactivate_cb

DESCRIPTION
  This function is used to close TLB mode C and return an boolean

PARAMETERS
  subs_id      - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tlbmc_deactivate_cb
( 
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p = NULL;
  /*--------------------------------------------------------------------------*/

  DS_EMBMS_MSG0_HIGH("ds_3gpp_embms_tlbmc_deactivate_cb()");

  ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

  if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
  {
    return;
  }

  if(ctrl_blk_p->is_registered_with_dpm)
  {
    ds_3gpp_embms_dereg_wm_with_dpm(ctrl_blk_p);
  }
  else
  {
    ds_3gpp_embms_tear_down(ctrl_blk_p);
  }
}/*ds_3gpp_embms_tlbmc_deactivate_cb*/

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_GET_CTRL_BLK

DESCRIPTION
  This function is used to get the EMBMS Control Block.

PARAMETERS
  subs_id - Subscription Id
  
DEPENDENCIES  
  None

RETURN VALUE
  Pointer to the EMBMS Ctrl Block

SIDE EFFECTS  
  None
==============================================================================*/
ds_3gpp_embms_ctrl_blk_type* ds_3gpp_embms_get_ctrl_blk
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_3gpp_embms_ctrl_blk_type *hndl = NULL;
  int                          i = 0;
  /*--------------------------------------------------------------------------*/

  do
  {
    i = ds_3gpp_embms_get_ctrl_blk_index(subs_id);

    if (!ds_3gpp_embms_validate_ctrl_blk_index(i))
    {
      DS_EMBMS_MSG1_ERROR("Ctrl Blk Index %d is invalid", i);
      break;
    }

    if (ds_3gpp_embms_ctrl_block_p[i] != NULL &&
        ds_3gpp_embms_ctrl_block_p[i]->subs_id == subs_id)
    {
      hndl = ds_3gpp_embms_ctrl_block_p[i];
    }

  } while (0); 

  DS_EMBMS_MSG2_HIGH("Fetched Ctrl Blk 0x%x for subs id %d",hndl, subs_id);
  return hndl;
} /* ds_3gpp_embms_get_ctrl_blk */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_SET_CTRL_BLK

DESCRIPTION
  This function is used to set the EMBMS Control Block.

PARAMETERS 
  ctrl_blk_p - Pointer to EMBMS Ctrl Blk
  
DEPENDENCIES  
  None

RETURN VALUE
  Index to Ctrl Blk

SIDE EFFECTS  
  None
==============================================================================*/
static int ds_3gpp_embms_set_ctrl_blk
(
  ds_3gpp_embms_ctrl_blk_type  *ctrl_blk_p
)
{
  int  index  = DS_3GPP_EMBMS_INVALID_CTRL_BLK_INSTANCE;
  /*--------------------------------------------------------------------------*/

  do
  {
    for (index=0;index<DS_3GPP_EMBMS_MAX_CTRL_BLK_INSTANCES;index++)
    {
      if (ds_3gpp_embms_ctrl_block_p[index] == NULL)
      {
        ds_3gpp_embms_ctrl_block_p[index] = ctrl_blk_p;
        break;
      }    
    }

  } while (0); 

  DS_EMBMS_MSG1_HIGH("Set Ctrl Blk 0x%x on index %", ctrl_blk_p);
  return index;
} /* ds_3gpp_embms_set_ctrl_blk */

/*==============================================================================

FUNCTION DS_3GPP_EMBMS_VALIDATE_CTRL_BLK

DESCRIPTION
  This function is used to validate the handle to EMBMS Control Block.

PARAMETERS
  Pointer to the EMBMS Control Block
  
DEPENDENCIES  
  None

RETURN VALUE
  TRUE, if valid
  FALSE, otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_validate_ctrl_blk
(
  ds_3gpp_embms_ctrl_blk_type *hndl
)
{
  boolean ret_val = FALSE;
  /*--------------------------------------------------------------------------*/

  if (hndl != NULL)
  {
    ret_val = TRUE;
  }
  else
  {
    DS_EMBMS_MSG0_HIGH("Invalid Cntrl block");
  }

  return ret_val;
} /* ds_3gpp_embms_validate_ctrl_blk */

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_FIND_OR_ALLOCATE_CTRL_BLK

DESCRIPTION
  This function is used to find the Control Block corresponding to passed
  subs_id. If no control block is found, allocate a new Control Block and
  return a pointer to that.

PARAMETERS
  subs_id: Subscription Id

DEPENDENCIES  
  None

RETURN VALUE  
  Pointer to the EMBMS Control Block.

SIDE EFFECTS
  None
==============================================================================*/
static ds_3gpp_embms_ctrl_blk_type* ds_3gpp_embms_find_or_allocate_ctrl_blk
(
  sys_modem_as_id_e_type subs_id
)
{
  int                          i = 0;
  ds_3gpp_embms_ctrl_blk_type* ctrl_blk_p = NULL;
  /*--------------------------------------------------------------------------*/

  do
  { 
    if ((ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id)) == NULL)
    {
      for (i=0; i<DS_3GPP_EMBMS_MAX_CTRL_BLK_INSTANCES; i++)
      {
        if (ds_3gpp_embms_ctrl_block_p[i] != NULL &&
            ds_3gpp_embms_ctrl_block_p[i]->in_use == FALSE)
        {
          ds_3gpp_embms_ctrl_block_p[i]->in_use = TRUE;
          ds_3gpp_embms_ctrl_block_p[i]->subs_id = subs_id;

          ds_3gpp_embms_set_ctrl_blk_index(i, subs_id);

          DS_EMBMS_MSG2_HIGH("Allocated Cntrl block at index %d for subs id %d",
                             i, subs_id);
          ctrl_blk_p = ds_3gpp_embms_ctrl_block_p[i];
          break;
        }
      }
    }
  } while (0); 

  return ctrl_blk_p;

} /* ds_3gpp_embms_allocate_ctrl_blk */

/*==============================================================================
FUNCTION      DS_3GPP_EMBMS_FREE_CTRL_BLK

DESCRIPTION
  This function is used to free the EMBMS control block corresponding to the
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
void ds_3gpp_embms_free_ctrl_blk
(
  sys_modem_as_id_e_type subs_id
)
{
  int i =0;
  /*--------------------------------------------------------------------------*/

  do
  {
    i = ds_3gpp_embms_get_ctrl_blk_index(subs_id);

    if (!ds_3gpp_embms_validate_ctrl_blk_index(i))
    {
      DS_EMBMS_MSG1_ERROR("Ctrl Blk Index %d is invalid", i);
      break;
    }

    if (ds_3gpp_embms_ctrl_block_p[i] != NULL &&
        ds_3gpp_embms_ctrl_block_p[i]->subs_id == subs_id)
    {
      DS_EMBMS_MSG2_HIGH("Releasing Cntrl block at index %d for subs id %d",
                         i, ds_3gpp_embms_ctrl_block_p[i]->subs_id);

      ds_3gpp_embms_ctrl_block_p[i]->in_use = FALSE;
      ds_3gpp_embms_ctrl_block_p[i]->subs_id = SYS_MODEM_AS_ID_NONE;
      ds_3gpp_embms_ctrl_block_p[i]->l2_wm_set_up = FALSE;
      ds_3gpp_embms_set_ctrl_blk_index(DS_3GPP_EMBMS_INVALID_CTRL_BLK_INSTANCE, 
                                       subs_id);
      break;
    }
  } while (0); 
 
  return;
} /* ds_3gpp_embms_free_ctrl_blk */

/*==============================================================================
FUNCTION DS_3GPP_GET_CTRL_BLK_INDEX

DESCRIPTION
  This functions fetches the Ctrl Blk index corresponding to the given Subs Id.
 
PARAMETERS 
  Subscription id

DEPENDENCIES
  None.

RETURN VALUE
  Ctrl Block Index

SIDE EFFECTS
  None.
==============================================================================*/
static int ds_3gpp_embms_get_ctrl_blk_index
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
    
    index = ds_3gpp_embms_subs_id_map[subs_id].ctrl_blk_index;

    DS_EMBMS_MSG2_MED("Subs Id: %d, Ctrl Blk index: %d", subs_id, index);

  } while (0); 

  return index;
} /* ds_3gpp_embms_get_ctrl_blk_index */

/*==============================================================================
FUNCTION DS_3GPP_SET_CTRL_BLK_INDEX

DESCRIPTION
  This functions maps the Subs Id and Ctrl Blk Index
 
PARAMETERS 
  EMBMS Ctrl Block Index
  Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
static void ds_3gpp_embms_set_ctrl_blk_index
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

    DS_EMBMS_MSG2_MED("Subs Id: %d, Ctrl Blk index: %d", 
                      subs_id, index);

    ds_3gpp_embms_subs_id_map[subs_id].ctrl_blk_index = index;
  } while (0); 

  return;
} /* ds_3gpp_embms_set_ctrl_blk_index */

/*==============================================================================
FUNCTION DS_3GPP_VALIDATE_CTRL_BLK_INDEX

DESCRIPTION
  This functions checks whether the given index into the ds_3gpp_embms_subs_id_map
  is valid.
 
PARAMETERS 
  Index 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
==============================================================================*/
static boolean ds_3gpp_embms_validate_ctrl_blk_index
(
  uint8 index
)
{
  boolean ret_val = FALSE;
  /*--------------------------------------------------------------------------*/

  if (index < DS_3GPP_EMBMS_MAX_CTRL_BLK_INSTANCES)
  {
    ret_val = TRUE;
  }

  return ret_val;
} /* ds_3gpp_embms_validate_ctrl_blk_index */

#endif /* FEATURE_DATA_EMBMS */
