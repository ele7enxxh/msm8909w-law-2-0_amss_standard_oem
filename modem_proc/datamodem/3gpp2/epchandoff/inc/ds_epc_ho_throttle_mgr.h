#ifndef DS_EPC_HO_THROTTLE_MGR_H
#define DS_EPC_HO_THROTTLE_MGR_H
/*===========================================================================

                      DS_EPC_HO_THROTTLE_MGR. H
                   
DESCRIPTION
  This module handles PDN Throttling info. When a PDN is throttled on the trat,
  no handoff for this PDN.
 
  PDN throttle info is received from MH via PS events. This module maintains
  the PDN throttle info and provide service for handoff.
 
EXTERNALIZED FUNCTIONS

 Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: 
  $Author: 

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/27/14    mz    Created Module
===========================================================================*/
#include "datamodem_variation.h"
#ifdef  FEATURE_EPC_HANDOFF
#include "comdef.h"
#include "customer.h"
#include "ds_profile.h"
#include "secerrno.h"
#include "secapi.h"
#include "ds_epc_timer.h"
#include "ps_sys.h"
#ifdef FEATURE_DATA_WLAN_MAPCON

/*===========================================================================

                          LOCAL DATA DECLARATIONS

===========================================================================*/
/*enum definition */
#define DS_EPC_THROTTLE_MAX_TIMER_CNT 6
#define DS_EPC_THROTTLE_ADD_RANDOM_TIMER_CNT 3
/*-------------------------------------------------------------------------
Throttle tech type handled in HO throttle mgr. 
Current don't support iwlan tech, since iwlan MH will handle iwlan thrtl 
-------------------------------------------------------------------------*/
typedef enum
{
  DS_EPC_HO_THRTL_MGR_TECH_MIN    = 0, 
  DS_EPC_HO_THRTL_MGR_LTE  = DS_EPC_HO_THRTL_MGR_TECH_MIN,
  DS_EPC_HO_THRTL_MGR_EHRPD,
  DS_EPC_HO_THRTL_MGR_UMTS,
  DS_EPC_HO_THRTL_MGR_TECH_MAX    = DS_EPC_HO_THRTL_MGR_UMTS
} ds_epc_ho_thrtl_mgr_tech_type;

/*-------------------------------------------------------------------------
Throttle ip type handled in HO throttle mgr
-------------------------------------------------------------------------*/
typedef enum 
{
  DS_EPC_HO_THRTL_MGR_IP_MIN = 0,
  DS_EPC_HO_THRTL_MGR_IPV4   = DS_EPC_HO_THRTL_MGR_IP_MIN,
  DS_EPC_HO_THRTL_MGR_IPV6,
  DS_EPC_HO_THRTL_MGR_IP_MAX = DS_EPC_HO_THRTL_MGR_IPV6
} ds_epc_ho_thrtl_mgr_ip_type;

/*-------------------------------------------------------------------------
Throttle timer CB info type
-------------------------------------------------------------------------*/
typedef struct  
{
  ds_epc_throttle_timer_type     * throttle_timer_ptr; /**< pointer to the throttle timer*/
  ds_epc_ho_thrtl_mgr_tech_type    tech_type; /**< tech type of throttle timer*/
}ds_epc_ho_thrtl_mgr_thrtl_timer_cb_info;

/*-------------------------------------------------------------------------
Throttle queue item data block definition
-------------------------------------------------------------------------*/
typedef struct thrtl_queue_s ds_epc_ho_thrtl_mgr_q_item; /**< queue element*/
struct thrtl_queue_s
{
  ds_profile_info_type                      apn_name; /** < APN name */
  ds_epc_ho_thrtl_mgr_thrtl_timer_cb_info   thrtl_timer_cb[DS_EPC_HO_THRTL_MGR_TECH_MAX+1];
  /** < throttle timer cb buffer */
  ds_epc_throttle_timer_type                throttle_timer[DS_EPC_HO_THRTL_MGR_TECH_MAX+1]; 
  /** < throttle timer */ 
};

/*-------------------------------------------------------------------------
Throttle item definition
-------------------------------------------------------------------------*/
typedef struct  
{
  q_link_type                  link; 
  ds_epc_ho_thrtl_mgr_q_item * throttle_info_block;
}ds_epc_ho_thrtl_mgr_q_type;

/*-------------------------------------------------------------------------
Throttle info definition. 
-------------------------------------------------------------------------*/
typedef struct  
{
  boolean                               comp_exist; 
  ds_profile_info_type                * apn_name;
  ds_epc_ho_thrtl_mgr_tech_type         tech_type;
}ds_epc_ho_thrtl_mgr_thrtl_info_type; 

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_MGR_READ_TIMER_VAL

DESCRIPTION   Read throttle timer value from NV item.

DEPENDENCIES  None. 
 
PARAMETERS    None

RETURN VALUE  NV read success 
              NV read failed, use default value 

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ho_throttle_mgr_read_timer_val(void);

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_PROCESS_THROTTLE_TIMER_EXPIRE_CMD

DESCRIPTION:  Process DS_EPC_HO_THROTTLE_TIMER_EXP_PROC_CMD from ds_task
 
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ho_throttle_mgr_process_throttle_timer_expire_cmd
( 
  ds_epc_ho_thrtl_mgr_thrtl_timer_cb_info   * timer_cb_info
);

/*queue related operations*/

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_MGR_Q_INIT

DESCRIPTION   Initialize queue head of HO throttle mgr's head 

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ho_throttle_mgr_q_init ( void );

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_MGR_ENQUEUE

DESCRIPTION   Push an item into HO throttle queue.

DEPENDENCIES  None.
 
PARAMETERS    APN name which need to put into the throttle queue.
 
RETURN VALUE  Queue item pointer pointer to the current APN name specified

SIDE EFFECTS  None.
===========================================================================*/
ds_epc_ho_thrtl_mgr_q_type* ds_epc_ho_throttle_mgr_enqueue
(
  ds_profile_info_type           * apn_name
);

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_MGR_DEQUEUE

DESCRIPTION   Pop an item out of HO throttle queue.

DEPENDENCIES  None.
 
PARAMETERS    Pointer to the throttle queue item.
 
RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ho_throttle_mgr_dequeue
(  
  ds_epc_ho_thrtl_mgr_q_type     *queue_item_ptr
);

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_MGR_SET_THROTTLE

DESCRIPTION   handle event sending from client. Please note that this throttle 
              should only be called for iwlan related HO throttling. Please note
              this fucntion don't handle recieve set throttle from same iface
              twice.
 
PARAMETERS    instance_ptr: current instance ptr 
              trat_iface_ptr: trat iface pointer
              tech_type: rat type that the throttle is on

DEPENDENCIES  None.

RETURN VALUE  0: if THROTTLE MGR handle throttle set successfully 
              NON-0: THROTTLE MGR fail to handle throttle, error code returned. 

SIDE EFFECTS  None
===========================================================================*/
int16 ds_epc_ho_throttle_mgr_set_throttle 
(
  ds_epc_cb_type *instance_ptr,
  ps_iface_type                        *trat_iface_ptr,
  ds_epc_ho_thrtl_mgr_tech_type         tech_type       
);

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_MGR_SET_THROTTLE_CLEAR

DESCRIPTION   Send THROTTLE CLEAR EVENT to HO throttle mgr for all technology. 
              Function only called when both EPC insts go down for the dualIP
              case.Tech type doesn't need in this case.
 
PARAMETERS    current instance ptr

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds_epc_ho_throttle_mgr_set_throttle_clear
(
  ds_epc_cb_type *instance_ptr 
);

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_MGR_IS_HO_THROTTLED
 
DESCRIPTION   Query HO throttle mgr if handoff is throttled on current tech 
              for this APN. If throttle timer is running, it means it's
              throttled. Otherwise, it's not throttled. Currently used to check
              before triggering the HO.
 
PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  TRUE: PDN is throttle on current tech for this APN
              FALSE: PDN is not throttle on current tech for this APN

SIDE EFFECTS  None
===========================================================================*/
boolean ds_epc_ho_throttle_mgr_is_ho_throttled
( 
  ds_epc_cb_type                       *instance_ptr,
  ds_epc_pdn_pref_sys_type              pref_sys
);

/*===========================================================================
FUNCTION      DS_EPC_HO_THROTTLE_CONVERT_DS_SYS_TO_THROTTLE_TECH
 
DESCRIPTION   Convert ds_sys_rat_ex_enum_type rat info to ds_epc_ho_thrtl_mgr 
              _tech_type. 
 
PARAMETERS    None.

DEPENDENCIES  None.

RETURN VALUE  

SIDE EFFECTS  None
===========================================================================*/
ds_epc_ho_thrtl_mgr_tech_type ds_epc_ho_throttle_convert_ds_sys_to_throttle_tech
(
  ds_sys_rat_ex_enum_type     rat_info
);
#endif /*FEATURE_DATA_WLAN_MAPCON*/
#endif /*FEATURE_EPC_HANDOFF*/
#endif /* DS_EPC_HO_THROTTLE_MGR_H */
