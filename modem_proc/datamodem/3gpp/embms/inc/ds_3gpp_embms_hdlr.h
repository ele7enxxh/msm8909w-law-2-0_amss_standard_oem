#ifndef DS_3GPP_EMBMS_HDLR_H
#define DS_3GPP_EMBMS_HDLR_H

/*===========================================================================

                        DS 3GPP eMBMS Header File
GENERAL DESCRIPTION
  This file contains the definition of structures to be used in eMBMS mode
  handler. This also has the list of externalized functions.

EXTERNALIZED FUNCTIONS

  DS_3GPP_EMBMS_INIT
    Called during power up initialization of the UE.

  DS_3GPP_EMBMS_PROCESS_CMDS
    Processes commands from PS for eMBMS iface control.

  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  dsumts_mbms_init() must be called at startup.

 Copyright (c) 2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/embms/inc/ds_3gpp_embms_hdlr.h#1 $
-----------------------------------------------------------------------------  
when        who    what, where, why
--------    ---    -------------------------------------------------------
09/14/11    hs     Created file/Initial version.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_EMBMS

#include "dstask.h"
#include "ps_iface_embms.h"
#include "ds_3gpp_bearer_context.h"

/*==============================================================================
                            TYPEDEFS
==============================================================================*/

/*------------------------------------------------------------------------------
  Macros defining the watermark values for DL data
------------------------------------------------------------------------------*/
#define DS_3GPP_EMBMS_DL_WM_DEFAULT_LO   175000 //10ms*100Mbps
#define DS_3GPP_EMBMS_DL_WM_DEFAULT_HI   350000 //20ms*100Mbps
#define DS_3GPP_EMBMS_DL_WM_DEFAULT_DNE  400000 //HI+50k
                                          
#define DS_3GPP_EMBMS_DUMMY_EPS_ID 0
/*------------------------------------------------------------------------------
  This is reuse of the watermark structure defined in DS 3GPP bearer context 
  module
------------------------------------------------------------------------------*/
typedef ds_bearer_context_wm_s ds_3gpp_embms_wm_s;

/*------------------------------------------------------------------------------
  This is a type definition for the eMBMS control block
------------------------------------------------------------------------------*/
typedef struct{
  ps_iface_type               ps_iface;
  acl_type                    acl;
  ps_phys_link_type           phys_link;
  ds_3gpp_embms_wm_s          rx_wm;
  ps_sig_enum_type            rx_sig;
  boolean                     l2_wm_set_up;
  boolean                     is_registered_with_dpm;
  sys_modem_as_id_e_type      subs_id;
  boolean                     in_use;
} ds_3gpp_embms_ctrl_blk_type;

/*------------------------------------------------------------------------------
  This is a type definition for the eMBMS TMGI Activation Info
------------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_embms_tmgi_activation_info_type   ind_info;
  uint8                                      index;
  sys_modem_as_id_e_type                     subs_id;
} ds_3gpp_embms_tmgi_act_ind_info;

/*------------------------------------------------------------------------------
  This is a type definition for the eMBMS TMGI De-activation Info
------------------------------------------------------------------------------*/
typedef struct
{
  ps_iface_embms_tmgi_deactivation_info_type   ind_info;
  sys_modem_as_id_e_type                       subs_id;
} ds_3gpp_embms_tmgi_deact_ind_info;

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                        eMBMS INITIALIZATION
===========================================================================*/

/*===========================================================================

FUNCTION      DS_3GPP_EMBMS_INIT

DESCRIPTION   Called once at mobile power-up. Creates embms ps_iface. Creates
              the phys link. Initializes the data structures to be used with
              eMBMS. Also performs registration with lower layers for message
              router communication.

DEPENDENCIES  eMBMS code runs in DS task.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_3gpp_embms_init(void);

/*===========================================================================

FUNCTION DS_3GPP_EMBMS_PROCESS_CMDS

DESCRIPTION
  This function processes eMBMS commands.

  This function should be called by the top-level task dispatcher whenever
  any embms specific command is received. The task dispatcher is DS task.

DEPENDENCIES
  Should be called only after all eMBMS structures are initialized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  ds_3gpp_embms_process_cmds
(
  const ds_cmd_type  *cmd_ptr
);

/*===========================================================================

FUNCTION      DS_3GPP_EMBMS_TEAR_DOWN

DESCRIPTION
  This function is used to clean up the eMBMS control structures. This puts
  the phys link and iface to down sate.

PARAMETERS
  ctrl_blk_p: Pointer to the EMBMS Control Block

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds_3gpp_embms_tear_down
(
  ds_3gpp_embms_ctrl_blk_type *ctrl_blk_p
);

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
rex_crit_sect_type* ds_3gpp_embms_get_crit_section(void);

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
);

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
);

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
  uint8                                       index,
  sys_modem_as_id_e_type                      subs_id
);

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
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_SET_UP_RLC_TO_PS_WM

DESCRIPTION
  This function is used to set up the Air link receive watermarks.

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
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_BRING_DOWN_RLC_TO_PS_WM

DESCRIPTION
  This function is used to bring down the Air link receive watermarks.
  
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
);

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
);

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
void* ds_3gpp_embms_get_avail_tmgi_list_client(void);

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
void ds_3gpp_embms_reset_avail_list_tmgi_client(void);

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
void* ds_3gpp_embms_get_avail_sai_list_client(void);

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
void ds_3gpp_embms_reset_avail_sai_list_client(void);

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
);

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
boolean ds_3gpp_embms_tlbmc_close_cb
(
  sys_modem_as_id_e_type subs_id
);

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
);

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
);

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
);

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
);

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
);

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
);



#endif /* FEATURE_DATA_EMBMS */
#endif /* DS_3GPP_EMBMS_HDLR_H */
