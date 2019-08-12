#ifndef WL1ULEDRX_H
#define WL1ULEDRX_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            L 1 UL EDRX FUNCTIONALITY                   

GENERAL DESCRIPTION
Contains implemenation of EDRX related to UL module. 
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2000-2012 by Qualcomm Technologies, Inc. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/inc/wl1uledrx.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who    what, where, why
--------   ---    ----------------------------------------------------------
07/16/14   sr     EDRX code cleanup
03/12/13   sr     Changes to execute RF TX after set mod timing for EDRX FS.
06/14/12   hk     E-DRX support for HS-RACH
04/04/12   hk     Added support to freeze/unfreeze UL entity
03/27/12   hk     Added support for FEATURE_WCDMA_HS_FACH_DRX
11/22/11   nd     externed wl1_drx_ul_stop_drx for fixing comp warning. 
11/09/11   nd     Added support for the EDRX log packets.
09/15/11   gnk    Changes for the review comments
09/12/11   gnk    Initial Version of UL EDRX implementation
===========================================================================*/

#include "wcdma_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_WCDMA_HS_FACH_DRX
#include "ulstates.h"
/*===========================================================================

                                 TYPES

===========================================================================*/

typedef enum
{
  WL1_UL_ENTITY_RACH,
  WL1_UL_ENTITY_AICH,
  WL1_UL_ENTITY_HS_RACH,
  WL1_UL_ENTITY_MAX,
}wl1_ul_entity_enum_type;


typedef enum
{
  /* If no channels are added */
  UL_E_DRX_INIT_STATE,
  /* Channels are up, but EDRX is not active */
  UL_E_DRX_INACTIVE_STATE,
  /* Channels are up and EDRX is active */
  UL_E_DRX_AWAKE_STATE,
  /* channels are up but need to relinquish the channels before transmitting */
  UL_E_DRX_SLEEP_STATE
}wl1_ul_edrx_state_enum_type;


typedef enum
{
  /* Before setting up the channels */
  WL1_EDRX_UL_ENTITY_ACTION_PRE_ADD,
  /* Before dropping the channels */
  WL1_EDRX_UL_ENTITY_ACTION_PRE_DROP,   
  /* After setting up the channels, it means the adding of channels is done */
  WL1_EDRX_UL_ENTITY_ACTION_POST_ADD,   
  /* After dropping the channels, it means the dropping of channels is done */
  WL1_EDRX_UL_ENTITY_ACTION_POST_DROP,   
  /* Indication to EDRX module saying that there is no activity going from UL module */
  WL1_EDRX_UL_ENTITY_ACTION_NO_ACTIVITY,   
  /* Indication to EDRX module saying that there is data to transmit and hence some activity from UL module */
  WL1_EDRX_UL_ENTITY_ACTION_START_ACTIVITY,
  WL1_EDRX_UL_ENTITY_NUM_ACTIONS
}wl1_edrx_ul_entity_action_enum_type;

typedef enum
{
  /* Sleep indefinitely tearing down the channels */
  WL1_EDRX_UL_SLEEP_INDEFINITELY,
  /* Wake up indefinietly when we have data to transmit */
  WL1_EDRX_UL_WAKEUP_INDEFINITELY,
  WL1_EDRX_UL_NUM_ACTIVITY
}wl1_edrx_ul_activity_enum_type;

typedef struct
{
  /** is scheduling needed */
  uint8 scheduling_needed;
  /** entity id provided by drx ctrlr   */
  uint8 drx_ctrlr_id;
  /** entity type RACH/AICH */
  wl1_ul_entity_enum_type entity;
  /** entity state */
  wl1_ul_edrx_state_enum_type entity_state;
}wl1_edrx_ul_entity_struct_type;

/* Variable to keep track of the UL EDRX entity state */
extern wl1_edrx_ul_entity_struct_type wl1_ul_edrx_entity;

extern boolean wl1_ul_edrx_init_tx_needed;

/*===========================================================================

                                 FUNCTIONS

===========================================================================*/
 /*=========================================================================
 FUNCTION        WL1_UL_EDRX_INIT
 
 DESCRIPTION    This function is used to initialize all the global variables required for EDRX UL module. This function
                         will be called during WL1 init and E-DRX stop.
 
 DEPENDENCIES    None
 
 RETURN VALUE    None
 
 SIDE EFFECTS    None
 =========================================================================*/
extern void wl1_ul_edrx_init(void);

/*=======================================================================
FUNCTION        WL1_EDRX_UL_ENTITY_STATE_UPDATE

DESCRIPTION    This function is called before start of adding the RACH physical channel and again 
               called after the phychan add is done. This is used to indicate the UL DRX entity when 
               the Phychan add is started and when it is completed. The same function is used to 
               indicate the UL DRX entity if we have data/No data through RACH in L1.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void
wl1_edrx_ul_entity_state_update 
(
  wl1_ul_entity_enum_type entity,
  wl1_edrx_ul_entity_action_enum_type action
);

/*=========================================================================
FUNCTION        WL1_DRX_UL_START_DRX

DESCRIPTION    This function is used to indicate the UL DRX entity that the EDRX is activated so 
               that the UL DRX entity registers the call back functions with the EDRX controller

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_drx_ul_start_drx (void);

/*=========================================================================
FUNCTION        WL1_DRX_UL_START_DRX

DESCRIPTION    This function is used to indicate the UL DRX entity that the EDRX is activated so 
                         that the UL DRX entity registers the call back functions with the EDRX controller

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_drx_ul_stop_drx (void);

/*=========================================================================
FUNCTION        WL1_UL_DRXCTRL_REGISTER_WAKE_SLEEP_CB

DESCRIPTION    This function is used to register the sleep call back functions and the wakeup call 
               back functions with the DRX controller. The registered call back functions are called 
               when DRX wants the UL DRX entity to wakeup/sleep.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_drxctrl_register_wake_sleep_cb(void);

/*=========================================================================
FUNCTION        WL1_UL_DRXCTRL_ENTITY_SCHEDULE_UPDATE

DESCRIPTION    This function indicates the DRX controller that UL DRX entity wants to either 
               sleep indefinitely or wakeup indefinitely

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_drxctrl_entity_schedule_update(wl1_ul_entity_enum_type entity, wl1_edrx_ul_activity_enum_type action);

/*=========================================================================
FUNCTION        WL1_UL_RACH_SLEEP_MODE_CB

DESCRIPTION    This function is a call back function by the DRX controller whenever the DRX controller 
               informs UL entity to sleep
               All the power optimizations, NPA clock down requests will be handled as a part of this call back.
               But since the RACH design is already optimized nothing will be done as part of this call back

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_rach_sleep_mode_cb(void);

/*=========================================================================
FUNCTION        WL1_UL_RACH_WAKEUP_MODE_CB

DESCRIPTION    This function is a call back function by the DRX controller whenever the DRX controller 
               informs UL entity to wakeup
               All the power , NPA clock relinquishments to original values will be handled as a part of this call back.
               But since the RACH design is already optimized and nothing is done as a part of this sleep call back
               we will not do much as a part of this call back

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_rach_wakeup_mode_cb(void);

/*=========================================================================
FUNCTION        WL1_UL_RACH_CHANNEL_SUSPEND_CB

DESCRIPTION    This function is a call back function by the DRX controller and will be called whenever the DRX controller 
               would like to suspend the UL channels
               Since there is no actual physical channel for RACH, there is no need to tear down the RACH channel and 
               so there is nothing that is done as a part of this call back function
                         
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_rach_channel_suspend_cb(void);

/*=========================================================================
FUNCTION        WL1_UL_RACH_CHANNEL_RESUME_CB

DESCRIPTION    This function is a call back function by the DRX controller and will be called whenever the DRX controller 
               would like to resume the UL channels
               We just need to call the set mod timing command as a part of this call back function.

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_rach_channel_resume_cb(void);
#ifdef FEATURE_WCDMA_HS_RACH
/*=========================================================================
FUNCTION        WL1_UL_HSPRACH_SLEEP_MODE_CB

DESCRIPTION    

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_hsprach_sleep_mode_cb(void);


/*=========================================================================
FUNCTION        WL1_UL_HSPRACH_WAKEUP_MODE_CB

DESCRIPTION    
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_hsprach_wakeup_mode_cb(void);

/*=========================================================================
FUNCTION        WL1_UL_HSPRACH_CHANNEL_SUSPEND_CB

DESCRIPTION    
                         
DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_hsprach_channel_suspend_cb(void);

/*=========================================================================
FUNCTION        WL1_UL_HSPRACH_CHANNEL_RESUME_CB

DESCRIPTION    

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_hsprach_channel_resume_cb(void);

#endif /*FEATURE_WCDMA_HS_RACH*/

/*=========================================================================
FUNCTION        wl1_edrx_ul_entity_log_state_info

DESCRIPTION    This function is used to transfer the UL state to logging function

DEPENDENCIES  None

RETURN VALUE  uint16;
                      UL activity 0 - nothing, 1 - R99 UL, 2 - PRE, 3 - ENH UL

SIDE EFFECTS   None
=========================================================================*/
extern uint16 wl1_edrx_ul_entity_log_state_info(void);

/*=========================================================================
FUNCTION        WL1_UL_EDRX_SCHEDULING_CTRL

DESCRIPTION    This function is used to indicate the START & STOP of EDRX Scheduling

DEPENDENCIES    None

RETURN VALUE    None

SIDE EFFECTS    None
=========================================================================*/
extern void wl1_ul_edrx_scheduling_ctrl(boolean start_scheduling);

#ifdef FEATURE_WCDMA_HS_RACH
/*=========================================================================
FUNCTION        wl1_edrx_ul_is_hs_prach_entity_awake

DESCRIPTION    This function retruns the state of the HS_PRACH entity

DEPENDENCIES  None

RETURN VALUE  boolean;
                      TRUE: If HS_PRACH entity is in active state
                      FALSE: otherwise

SIDE EFFECTS   None
=========================================================================*/
extern boolean wl1_edrx_ul_is_hs_prach_entity_awake(void);
#endif /* FEATURE_WCDMA_HS_RACH */

#endif /* FEATURE_WCDMA_HS_FACH_DRX */

#endif /* WL1ULEDRX_H */

