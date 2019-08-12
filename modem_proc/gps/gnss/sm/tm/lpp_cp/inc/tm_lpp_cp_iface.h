#ifndef _TM_LPP_CP_IFACE_H
#define _TM_LPP_CP_IFACE_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         TM LPP Control Plane Interface

GENERAL DESCRIPTION
  This file contains TM's LPP Control Plane Protocol Interface. 

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
#include "tm_lpp_cp.h"

/*===========================================================================

FUNCTION tm_lpp_cp_prtl_iface_init

DESCRIPTION
  This function is used by TmCore which calls it when TM task starts, 
  initiating the LPP Control Plane Protocol submodule.
  
DEPENDENCIES: none.

RETURN VALUE: none.

SIDE EFFECTS: none.

===========================================================================*/
void tm_lpp_cp_prtl_iface_init(void);

/*===========================================================================

FUNCTION tm_lpp_cp_callflow_kind_set

DESCRIPTION
  Sets the nature of the call flow for the current session.

DEPENDENCIES: none

RETURN VALUE: none

SIDE EFFECTS: none

===========================================================================*/
void tm_lpp_cp_callflow_kind_set(tm_lpp_cp_callflow_kind_enum_type e_callflow_kind);

/********************************************************************
*
* tm_lpp_cp_send_start_sess_req
*
* Function description:
*   Sends a START Session request to TM-Core. 
*
* Parameters: 
*   e_agps_mode : Send START Session in MSB or MSA mode.
*
* Return:
*   TRUE if Successful, else FALSE
*  
*********************************************************************
*/
boolean tm_lpp_cp_send_start_sess_req(tm_lpp_cp_session_agps_mode_enum_type   e_agps_mode);
#endif /* _TM_LPP_CP_IFACE_H */
