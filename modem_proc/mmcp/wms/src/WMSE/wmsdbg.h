#ifndef WMSDBG_H
#define WMSDBG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
           W I R E L E S S    M E S S A G I N G   S E R V I C E S
           wmsdbg.h -- Debug Group Definitions

  The WMS module which implements the User API for SMS. This source file
  defines the data types for the Debug group.

  -------------

    Copyright (c) 2001-2008,2010-2012 QUALCOMM Technologies Incorporated.
    All Rights Reserved.
    Qualcomm Confidential and Proprietary

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/wms/src/WMSE/wmsdbg.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#include "mmcp_variation.h"

#if (defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS))

/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "wms.h"
#include "wmsi.h"



/*===========================================================================

                         DATA TYPE DECLARATIONS

===========================================================================*/

/* Debug data
*/
typedef struct wms_dbg_struct
{
  boolean                       init_complete;

} wms_dbg_s_type;



/*===========================================================================

                       FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION wms_dbg_s_ptr

DESCRIPTION
  Return the pointer to the Debug data.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to the internal data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_dbg_s_type* wms_dbg_s_ptr
(
  void
);

/*===========================================================================
FUNCTION wms_dbg_process_cmd

DESCRIPTION
  Dispatch the Debug command processing.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void wms_dbg_process_cmd
(
  wms_cmd_type  *cmd_ptr
);



/*===========================================================================
FUNCTION wms_dbg_init

DESCRIPTION
  Do Debug-related initialization.

DEPENDENCIES
  none

RETURN VALUE
  None.

SIDE EFFECTS
  Debug data initialized.
===========================================================================*/
void wms_dbg_init
(
  void
);

#endif /* FEATURE_CDSMS */

#if (defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS))

/*===========================================================================
FUNCTION wms_dbg_get_cmd_string

DESCRIPTION
  This function returns the Command given the Command ID

DEPENDENCIES
  Command Enumeration (wms_cmd_id_e_type)

RETURN VALUE
  Command String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cmd_string
(
  wms_cmd_id_e_type cmd_id
);

/*===========================================================================
FUNCTION wms_dbg_get_cmd_err_string

DESCRIPTION
  This function returns the Command Error given the Command Error ID

DEPENDENCIES
  Command Error Enumeration (wms_cmd_err_e_type)

RETURN VALUE
  Command String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cmd_err_string
(
  wms_cmd_err_e_type cmd_err
);

/*===========================================================================
FUNCTION wms_dbg_get_cfg_event_string

DESCRIPTION
  This function returns the Configuration event String given the cfg_event

DEPENDENCIES
  Configuration Event Enumeration (wms_cfg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_cfg_event_string
(
  uint16 cfg_event
);

/*===========================================================================
FUNCTION wms_dbg_get_msg_event_string

DESCRIPTION
  This function returns the Message event String given the msg_event

DEPENDENCIES
  Message Event Enumeration (wms_msg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_msg_event_string
(
  uint16 msg_event
);

/*===========================================================================
FUNCTION wms_dbg_get_dc_event_string

DESCRIPTION
  This function returns the DC event String given the _event

DEPENDENCIES
  DC Event Enumeration (wms_dc_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_dc_event_string
(
  uint16 dc_event
);

/*===========================================================================
FUNCTION wms_dbg_get_bc_event_string

DESCRIPTION
  This function returns the Broadcast event String given the bc_event

DEPENDENCIES
  Broadcast Event Enumeration (wms_bc_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_bc_event_string
(
  uint16 bc_event
);

/*===========================================================================
FUNCTION wms_dbg_get_event_bc_mm_string

DESCRIPTION
  This function returns the Multimode Broadcast event String given the bc_mm_event

DEPENDENCIES
  Multimode Broadcast Event Enumeration (wms_bc_mm_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_bc_mm_event_string
(
  uint16 bc_mm_event
);

/*===========================================================================
FUNCTION wms_dbg_get_dbg_event_string

DESCRIPTION
  This function returns the Debug event String given the dbg_event

DEPENDENCIES
  Debug Event Enumeration (wms_dbg_event_e_type)

RETURN VALUE
  Event String

SIDE EFFECTS
  None
===========================================================================*/
char *wms_dbg_get_dbg_event_string
(
  uint16 dbg_event
);

#endif /* defined(FEATURE_GWSMS) || defined(FEATURE_CDSMS) */

#ifdef FEATURE_FUZZ_TEST
#define WMS_CMD_DBG_SEND_TRANSPORT_RPT_IND WMS_CMD_MAX
#define WMS_DBG_EVENT_SEND_TRANSPORT_RPT_IND WMS_DBG_EVENT_MAX
#define WMS_DBG_CONNECTION_ID   255
/* Transaction ID used for diagnostic. */
#define WMS_DBG_TRANSACTION_ID  0xFFFFFFFE

/*=========================================================================
FUNCTION  wms_dbg_send_transport_rpt_ind
===========================================================================*/
/**
  Send the transport report indication for debug purpose.

  @param client_id    [IN]  Client ID.
  @param cmd_cb       [IN]  Command callback for reporting the command status.
  @param *user_data   [IN]  Pointer provided by the client to uniquely identify
                            this transaction of sending a message. The same
                            pointer is passed to the client's callback function.
                            A NULL pointer is acceptable.
  @param rpt_ind_ptr [IN]  Pointer to the transprot_rpt_ind sturct.

  @return
  Status of the request.
   - WMS_OK_S
   - WMS_OUT_OF_RESOURCES_S
   - WMS_UNSUPPORTED_S

  @dependencies
  Client must have been initialized.

  @commandstatus
  WMS_CMD_ERR_NONE\n
  WMS_CMD_ERR_DBG_BAD_PARAM

  @events
  WMS_DBG_EVENT_RETRY_INTERVAL

  @sideeffects
  A command is sent to the WMS task.
*/

wms_status_e_type wms_dbg_send_transport_rpt_ind
(
  wms_client_id_type                  client_id,
  wms_cmd_cb_type                     cmd_cb,
  const void                        * user_data,
  wms_msg_transport_rpt_ind_s_type  * rpt_ind_ptr
);

#endif

#endif /* WMSDBG_H */
