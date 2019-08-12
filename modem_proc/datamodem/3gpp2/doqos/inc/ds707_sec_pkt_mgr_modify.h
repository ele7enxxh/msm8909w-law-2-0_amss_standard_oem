#ifndef DS707_SEC_PKT_MGR_MODIFY_H
#define DS707_SEC_PKT_MGR_MODIFY_H
/*===========================================================================

                      D S 7 0 7 _ S E C _ P K T _ M G R _ M O D I F Y
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2004 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.h_v   1.4   27 Nov 2002 11:02:18   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/doqos/inc/ds707_sec_pkt_mgr_modify.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

-----------------------------------------------------------------------------
when        who    what, where, why
--------    ---    -------------------------------------------------------
07/17/07    sk     Created module

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_DATA_IS707

/*===========================================================================

            EXTERNAL DEFINITIONS AND DECLARATIONS

===========================================================================*/
/*-----------------------------------------------------------------
  The events for the QoS modify state machine
------------------------------------------------------------------*/
typedef enum
{
  DS707_SEC_PKT_MGR_MODIFY_OPEN       =0,/*Post by the caller if flow modify is initiated  */
  DS707_SEC_PKT_MGR_MODIFY_SUCCESS    =1,/* Post by the caller if flow modify is succeeded */
  DS707_SEC_PKT_MGR_MODIFY_TIMEOUT    =2,/* Post by the caller if flow modify is timedout  */
  DS707_SEC_PKT_MGR_MODIFY_REJECT     =3,/* Post by the caller if flow modify is rejected  */
  DS707_SEC_PKT_MGR_MODIFY_CLOSE      =4,/* Post by the caller if flow is being released   */
  DS707_SEC_PKT_MGR_MODIFY_ERROR      =5 /* Post by the caller if flow modify is error     */
}ds707_sec_pkt_mgr_modify_type;

/*===========================================================================

FUNCTION      DS707_SEC_PKT_MGR_MODIFY_SM_INIT

DESCRIPTION   This function is called during power up. This is start of
              QoS modify state machine.

DEPENDENCIES  This function must be called only after ds flows are initialized

RETURN VALUE  None

SIDE EFFECTS  The initial state of the modify state is set
===========================================================================*/
void ds707_sec_pkt_mgr_modify_sm_init(void);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_MGR_SET_MODIFY_IN_PROGRESS

DESCRIPTION   This is utility function and is called to set the modify flag.

DEPENDENCIES  This function is called in application's context and DS context only. 
              This gives priority to IOCTL over DS task modifying this variable.

RETURN VALUE  None

SIDE EFFECTS  The modify flag is set to TRUE or FALSE
===========================================================================*/
void ds707_sec_pkt_mgr_set_modify_in_progress(boolean modify_flag, 
                                              ds707_flow_type *ds_flow_ptr);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_IS_MODIFY_IN_PROGRESS

DESCRIPTION   This is utility function and is called to get the modify flag.

DEPENDENCIES  None

RETURN VALUE  The Flag value

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_sec_pkt_mgr_is_modify_in_progress(ds707_flow_type *ds_flow_ptr);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_MGR_IS_DQOS_MODIFY_REQ

DESCRIPTION   Utility function used only by this file. This utility function 
              is called to find if the DQOS is being modified

DEPENDENCIES  None

RETURN VALUE  :TRUE   - if DQOS is being modified
              :FALSE  - if DQOS is being modified

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sec_pkt_mgr_is_dqos_modify_req(ps_flow_type  * ps_flow_ptr);

/*===========================================================================

FUNCTION      DS707_SEC_PKT_MGR_IS_AQOS_MODIFY_REQ

DESCRIPTION   Utility function used only by this file. This utility function 
              is called to find if the AQOS is being modified

DEPENDENCIES  None

RETURN VALUE  :TRUE   - if AQOS is being modified
              :FALSE  - if AQOS is being modified

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sec_pkt_mgr_is_aqos_modify_req(ps_flow_type  * ps_flow_ptr);

/*===========================================================================
FUNCTION      DS707_SEC_PKT_MGR_MODIFY_SM_EVENT

DESCRIPTION   This is the interface function to notify the modify state machine
              The caller is allowed to generate the defined events. Based on 
              the events and the current modify state, the state machine will
              execute the defined action.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  If the event is valid, then Modify state machine will start 
              executing
===========================================================================*/
void ds707_sec_pkt_mgr_modify_sm_event(ds707_flow_type *ds_flow_ptr[], 
                                    uint8 num_flows,
                                    ds707_sec_pkt_mgr_modify_type modify_type);


#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_HDR_QOS */
#endif /* DS707_SEC_PKT_MGR_MODIFY_H */

