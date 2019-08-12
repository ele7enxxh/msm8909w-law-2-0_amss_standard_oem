#ifndef DS_3GPP_EMBMS_TMGI_TBL_MGR_H
#define DS_3GPP_EMBMS_TMGI_TBL_MGR_H

/*==============================================================================

               DS 3GPP eMBMS TMGI Table Manager Header File
GENERAL DESCRIPTION
  This file contains the definition of structures to be used in the TMGI
  table management module of eMBMS mode handler. 

EXTERNALIZED FUNCTIONS
  
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  

 Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/embms/inc/ds_3gpp_embms_tmgi_tbl_mgr.h#1 $
--------------------------------------------------------------------------------  
when        who    what, where, why
--------    ---    -------------------------------------------------------------
09/27/11    hs     Created file/Initial version.

==============================================================================*/

/*==============================================================================
                     INCLUDE FILES FOR MODULE
==============================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_EMBMS

#include "lte.h"
#include "ps_iface_embms.h"
#include "list.h"
#include "ds_3gpp_embms_rrcif.h"
#include "ds3gsubsmgr.h"

/*==============================================================================
                            TYPEDEFS
==============================================================================*/

#define DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE (2*DS_3GPP_EMBMS_MAX_ACTIVE_SESSIONS)

#define DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index) \
        (index < DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE)

/*------------------------------------------------------------------------------
  This is a type definition to hold the client information.
------------------------------------------------------------------------------*/
typedef struct{
  list_link_type link;          /* Pointer to the next item in list           */
  void           *user_data_ptr;/* Client info passed from upper layer        */
} ds_3gpp_embms_client_info_type;

/*------------------------------------------------------------------------------
  This is the enum used to define the various states for a TMGI
------------------------------------------------------------------------------*/
typedef enum{
  DS_3GPP_EMBMS_TMGI_STATE_INACTIVE     = 0,
  DS_3GPP_EMBMS_TMGI_STATE_ACTIVATING   = 1,
  DS_3GPP_EMBMS_TMGI_STATE_ACTIVE       = 2,
  DS_3GPP_EMBMS_TMGI_STATE_DEACTIVATING = 3,
  DS_3GPP_EMBMS_TMGI_STATE_INVALID      = 4
} ds_3gpp_embms_tmgi_state_e;

/*------------------------------------------------------------------------------
  This is the data type that will be used to define the table containing
  information about the management of TMGIs and the clients associated with
  them
------------------------------------------------------------------------------*/
typedef struct{
  ps_iface_embms_tmgi_type   tmgi_session_info; 
                                     /* TMGI that we are working with         */
  uint8                      rb_id;  /* Radio bearer id associated with TMGI  */
  uint8                      ref_count;  /* Number of clients for the TMGI    */
  list_type                  client_list;/* List of clients                   */
  ds_3gpp_embms_tmgi_state_e state;  /* State of the TMGI                     */
} ds_3gpp_embms_context_info_type;

typedef enum{
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_FAIL = 0,
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_SUCCESS,
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_DUP,
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_MAX
} ds_3gpp_embms_tmgi_tbl_client_add_enum_type;
/*==============================================================================
                            VARIABLES
==============================================================================*/

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
==============================================================================*/

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_FIND_TMGI

DESCRIPTION
  This function is used to find if the TMGI table already has an entry for
  the TMGI.

  If there is a TMGI match a valid index (between 0 and 15) is returned

PARAMETERS
  IN
    TMGI - to check against all valid entries in table
    is_response_processing - is function called while processing response
    subs_id                - Subscription Id 

  OUT
    index - the entry in the table where the TMGI match occured

DEPENDENCIES
  The matching logic has to behave differently between trying to process a req
  from the app and trying to process a response from LTE RRC.

  While processing the request, the TMGI table is believed to have more updated
  info and while processing the response from LTE RRC, the response is expected
  to have more updated session info.

  It is for this reason the caller has to pass info about whether this function
  is being called while processing a request or a response.

RETURN VALUE
  True - if entry is present
  False otherwise

SIDE EFFECTS  
  None
===========================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
(
  ps_iface_embms_tmgi_type *tmgi_session_info,
  boolean                  is_response_processing,
  uint8                    *index,
  sys_modem_as_id_e_type   subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_ADD_CLIENT_TO_LIST

DESCRIPTION
  This function is used to add a client to the list of clients that have 
  requested the activation of the same TMGI

PARAMETERS
  IN
    index                 - the index into the ds_3gpp_embms_context_table
    client_id             - This is the client id to be added to the list.
    subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_FAIL    - if client cannot be added
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_SUCCESS - if client was added
  DS_3GPP_EMBMS_TMGI_TBL_CLIENT_ADD_DUP     - if client in list

SIDE EFFECTS  
  None
==============================================================================*/
ds_3gpp_embms_tmgi_tbl_client_add_enum_type 
                                   ds_3gpp_embms_tmgi_tbl_mgr_add_client_to_list
(
  uint8                     index,
  void                      *client_id,
  sys_modem_as_id_e_type    subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_ALLOCATE_ENTRY

DESCRIPTION
  This function is used to locate and unused entry in the TMGI context table and
  allocate memory needed for storing the TMGI context information.

  Note: List init will happen for the list of clients. The list should be 
        destroyed before freeing the memory allocated for this context. Freeing
        this context will happen when the last client deactivates TMGI.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if we were able to find free index and allocate memory
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_allocate_entry
(
  ps_iface_embms_tmgi_type *tmgi_session_info,
  void                     *client_id,
  uint8                    *index_ptr,
  sys_modem_as_id_e_type    subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_SET_STATE

DESCRIPTION
  This function is used to set a state for a TMGI

PARAMETERS
  index      - The index in the TMGI table where we have to set state
  tmgi_state - The state to set to.
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if we were able to set state
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_set_state
(
  uint8                      index,
  ds_3gpp_embms_tmgi_state_e tmgi_state,
  sys_modem_as_id_e_type     subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_STATE

DESCRIPTION
  This function is used to get state for a TMGI

PARAMETERS
  index      - The index in the TMGI table where we have to get state from
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  ds_3gpp_embms_tmgi_state_e - state of the TMGI

SIDE EFFECTS  
  None
==============================================================================*/
ds_3gpp_embms_tmgi_state_e ds_3gpp_embms_tmgi_tbl_mgr_get_state
(
  uint8                      index,
  sys_modem_as_id_e_type     subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_REF_COUNT

DESCRIPTION
  This function is used to get ref_count for a TMGI

PARAMETERS
  index      - The index in the TMGI table where we have to get ref_count from
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  uint8 - the number of clients that have activated the TMGI

SIDE EFFECTS  
  None
==============================================================================*/
uint8 ds_3gpp_embms_tmgi_tbl_mgr_get_ref_count
(
  uint8                      index,
  sys_modem_as_id_e_type     subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_REMOVE_CLIENT_FROM_LIST

DESCRIPTION
  This function is used to remove a client from the list of clients that have 
  requested the activation of the same TMGI

  Note: This function decrements the ref_count in the TMGI context info.

PARAMETERS
  IN
    index                 - the index into the ds_3gpp_embms_context_table
    client_id             - This is the client id to be removed from the list.
    subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if we were able to remove the entry.
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_remove_client_from_list
(
  uint8                   index,
  void                   *client_id,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_IS_CLIENT_IN_LIST

DESCRIPTION
  This function is used to find if a client id is present in the list of clients
  that have requested the activation of a TMGI

  This function does not do any changes to the client list

PARAMETERS
  IN
    index                 - the index into the ds_3gpp_embms_context_table
    client_id             - This is the client id to be found in the list.
    subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if we were able to find the entry.
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_is_client_in_list
(
  uint8                   index,
  void                   *client_id,
  sys_modem_as_id_e_type  subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_REMOVE_ENTRY

DESCRIPTION
  This function is used to remove an entry from the ds_3gpp_embms_context_table.
  This function deallocates client list memory, destroys the client list and
  frees up the memory that was allocated for the entry at the index specified.

PARAMETERS
  IN
    index                 - the index into the ds_3gpp_embms_context_table
    subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_tbl_mgr_remove_entry
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_STORE_MRB_ID

DESCRIPTION
  This function is used to store the Multicast Radio Bearer (m-rb) id in the 
  TMGI table. Currently this m-RB is not used for any purpose. But if the WM
  at lower layers are going to be different for different radio bearers, we will
  need this for registration with lower layers.

PARAMETERS
    index   - the index into the ds_3gpp_embms_context_table
    mrb_id  - the radio bearer id for this TMGI
    subs_id - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_tbl_mgr_store_mrb_id
(
  uint8                  index,
  uint8                  mrb_id,
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_STORE_SESSION_ID

DESCRIPTION
  This function is used to store the session id returned by LTE RRC. The update
  to our table will happen only if the table currently has no session id info
  in it. Otherwise the update will be ignored. On a similar note, we will not
  set the session id to be 0xFF (invalid session id).

PARAMETERS
    index      - the index into the ds_3gpp_embms_context_table
    session_id - the session id that was activated
    subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_tbl_mgr_store_session_id
(
  uint8                  index,
  uint8                  session_id,
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_IS_ANY_TMGI_ACTIVE

DESCRIPTION
  This function is used to figure out if there are any TMGIs present in the
  TMGI table.

PARAMETERS
  subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if there are TMGIs in the table
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_is_any_tmgi_active
(
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_IS_ANY_TMGI_ACTIVE

DESCRIPTION
  This function is used to figure out if there are any TMGIs present in the
  TMGI table.

PARAMETERS
  subs_id               - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - if there are TMGIs in the table
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_tmgi_tbl_mgr_init_context_table
(
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_GET_LIST

DESCRIPTION
  This function is used to fetch the list of clients.

PARAMETERS
  index      - index into the table
  subs_id    - Subscription Id

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the list if there are no errors
  NULL otherwise

SIDE EFFECTS  
  None
==============================================================================*/
list_type* ds_3gpp_embms_tmgi_tbl_mgr_get_list
(
  uint8                  index,
  sys_modem_as_id_e_type subs_id
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_TMGI_TBL_MGR_FILL_ACT_TMGI_LIST

DESCRIPTION
  This function is used to populate the active TMGI list into a container sent
  into this function.

PARAMETERS 
  IN
    subs_id               - Subscription Id
  OUT
    act_tmgi_list_ind_ptr - pointer to the container

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if everything was successful
  FALSE otherwise

SIDE EFFECTS  
  Allocates memory for the TMGI list to be sent. Caller of this function
  has to free the memory after sending the indication.
==============================================================================*/
boolean ds_3gpp_embms_tmgi_tbl_mgr_fill_act_tmgi_list
(
  ps_iface_embms_tmgi_list_info_type* act_tmgi_list_ind_ptr,
  sys_modem_as_id_e_type              subs_id
);

#endif /* FEATURE_DATA_EMBMS */
#endif /* DS_3GPP_EMBMS_TMGI_TBL_MGR_H */


