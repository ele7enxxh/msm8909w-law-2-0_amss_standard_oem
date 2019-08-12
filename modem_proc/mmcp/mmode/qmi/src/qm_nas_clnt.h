#ifndef _QM_NAS_CLNT_H
#define _QM_NAS_CLNT_H

/*===========================================================================

                         QM_NAS_CLNT.H

DESCRIPTION

  QMI_MMODE header file for nas client information module.

Copyright (c) 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:
  $DateTime:
  $Author:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/25/13    sm     Created file
===========================================================================*/

#include "comdef.h"
#include "qm_comdef.h"
#include "qmi_nas_internal.h"
#include "network_access_service_v01.h"

/*--------------------------------------------------------------------------- 
  data types
---------------------------------------------------------------------------*/
#define QM_NAS_MAX_NUM_CLIDS       NASI_MAX_CLIDS
#define QM_NAS_INVALID_CLIENT      (-1)
// Macro to determine if client id is valid, should be between invalid and max
#define QM_NAS_CLNT_ID_VALID(clid) ((clid)>0 && (clid)<=QM_NAS_MAX_NUM_CLIDS)

/*===========================================================================
  FUNCTION QM_NAS_CLNT_INIT()

  DESCRIPTION
    This function initializes the nas client information

  PARAMETERS

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_init ( void );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_CONFIG_ALLOC()

  DESCRIPTION
    This function allocates memory for the nas client config information for a single client.

  PARAMETERS
    clid: client to allocate memory for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_config_alloc ( int32 clid );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_RESET_INFO()

  DESCRIPTION
    This function clears out client specific information for the specified clid

  PARAMETERS
    clid: client to reset information for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_reset_info ( int32 clid );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_CONFIG_DEALLOC()

  DESCRIPTION
    This function deallocates memory for the nas client config information for a single client.

  PARAMETERS
    clid: client to deallocate memory for

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_config_dealloc ( int32 clid );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_PROCESS_CONFIG_SIG_INFO()

  DESCRIPTION
    This function sets the config_sig_info information for the specific client

  PARAMETERS
    clid: client to store config_sig_info information for
    config: information to store for client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_process_config_sig_info( int32 clid, nas_config_sig_info_req_msg_v01 *config_sig_info );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_PROCESS_CONFIG_SIG_INFO2()

  DESCRIPTION
    This function sets the config_sig_info information for the specific client

  PARAMETERS
    clid: client to store config_sig_info information for
    config: information to store for client 

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_process_config_sig_info2( int32 clid, nas_config_sig_info2_req_msg_v01 *config );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_GET_NEXT_CLIENT_FOR_SIG_INFO_IND()

  DESCRIPTION
    This function retrieves the next client registered for sig_info_ind, based
    on provided previous.

  PARAMETERS
    cl_id: previous client id that was returned. QM_NAS_INVALID_CLIENT if first call.

  RETURN VALUE
    uint32: next client, QM_NAS_INVALID_CLIENT if no more.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 qm_nas_clnt_get_next_client_for_sig_info_ind( int32 clid, enum qm_subs_e sub_id );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_GET_NEXT_CLIENT_FOR_ERR_RATE_IND()

  DESCRIPTION
    This function retrieves the next client registered for err_rate_ind, based
    on provided previous.

  PARAMETERS
    cl_id: previous client id that was returned. QM_NAS_INVALID_CLIENT if first call.

  RETURN VALUE
    uint32: next client, QM_NAS_INVALID_CLIENT if no more.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 qm_nas_clnt_get_next_client_for_err_rate_ind( int32 clid, enum qm_subs_e sub_id );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_GET_BINDING()

  DESCRIPTION
    This function retrieves the subscription for the given client.

  PARAMETERS
    cl_id: client to query bound subscription for.

  RETURN VALUE
    enum qm_subs_e: bound subscription for a client, QM_SUBS_MIN if error.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
enum qm_subs_e qm_nas_clnt_get_binding( int32 clid );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_SEND_IND()

  DESCRIPTION
    This function calls the QMI Framework API to send out the NAS service
    indication to client.

  PARAMETERS
    clid: client ID to send ind to
    cmd: Which indication is being sent
    ind: pointer to dsm item to send

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_send_ind( int32 clid, int32 cmd, dsm_item_type* ind );

/*===========================================================================
  FUNCTION QM_NAS_CLNT_CROSSED_THRESHOLD()

  DESCRIPTION
    This function determines if provided new value has crossed threshold or delta
    for specified client.

  PARAMETERS
    clid: client ID to check
    subscription: subscription this request is for
    rat: RAT to check thresholds for
    type: the measurement type to check threshold for
    new_value: the new value
    old_value: the previous value

  RETURN VALUE
    boolean: whether new value crossed a threshold from old value

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_nas_clnt_crossed_threshold( int32 clid, enum qm_subs_e subscription,
                                       enum qm_rat_e rat, enum qm_sig_type_e type,
                                       float new_value, float old_value );

/*===========================================================================
  FUNCTION QMI_NAS_CLNT_SEND_RESPONSE()

  DESCRIPTION
    Send the response to framework

  PARAMETERS
    cmd_buf_p: pointer to command buffer for client
    msg_ptr: pointer to dsm item to send

  RETURN VALUE
    TRUE if sent successfully, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_nas_clnt_send_response
(
  qmi_cmd_buf_type *      cmd_buf_p,
  dsm_item_type *         msg_ptr
);

/*===========================================================================
  FUNCTION QM_NAS_CLNT_VALIDATE_CMD_BUF_P()

  DESCRIPTION
    Validates cmd_buf_p pointer

  PARAMETERS
    cmd_buf_p: pointer to command buffer for client

  RETURN VALUE
    TRUE if valid, FALSE otherwise

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qm_nas_clnt_validate_cmd_buf_p
( 
  qmi_cmd_buf_type *cmd_buf_p 
);

/*===========================================================================
  FUNCTION QM_NAS_CLNT_SEND_IND_ALL()

  DESCRIPTION
    This function calls the QMI Framework API to send out the NAS service
    indication to all client that are registered for the IND.

  PARAMETERS
    clid: client ID to send ind to
    cmd: Which indication is being sent
    ind: pointer to dsm item to send

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qm_nas_clnt_send_ind_all( uint16 cmd, void *cstruct, enum qm_subs_e subs );


#endif // !_QM_NAS_CLNT_H
