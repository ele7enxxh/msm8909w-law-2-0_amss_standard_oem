#ifndef DS707_DEVMGR_H
#define DS707_DEVMGR_H

/** 
  @file ds707_devmgr.h
  @brief 
   Handles device manager
*/
/** Contains API's to handle device manager */

/*===========================================================================

                         D S 7 0 7 _ D E V _ M G R
GENERAL DESCRIPTION
  This file contains the device manager APIs to handles the LCP & AUTH 
  incase of eHRPD 

EXTERNALIZED FUNCTIONS

INTERNAL FUNCTIONS 

 Copyright (c) 2011 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_devmgr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/21/14   sc      Added code to perform graceful PPP termination during hard
                   failure and then update eHRPD availability  
01/06/14   vm      Changes to use correct PCMT value during IRAT success and
                   failure
09/06/11   ssh     Test instrumentation
05/18/11   jee     Created module
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_ppp.h"
#include "dstask_v.h"
#include "ds707_data_session_profile.h"
#include "rex.h"

#ifdef FEATURE_EHRPD
/*===========================================================================

                          PUBLIC DATA DECLARATIONS

===========================================================================*/

/** eHRPD partial context EFS config structure
*/
typedef PACKED struct
{ 
  uint32  pcmt_val_ehrpd; /**<EHRPD PCMT value */
  uint32  pcmt_val_irat;  /**<iRAT PCMT value */
} ds707_devmgr_partial_ctxt_cfg_type;

/** Device Manager states
*/
typedef enum
{
  DEVMGR_CLOSED_STATE  = 0,
  DEVMGR_CONFIG_STATE  = 1,
  DEVMGR_UP_STATE      = 2,
  DEVMGR_LINGER_STATE  = 3,
  DEVMGR_CLOSING_STATE = 4,
  DEVMGR_RESTART_STATE = 5    
} ds707_devmgr_state_type;

/** Device manager stop reason
*/
typedef enum
{
  DS707_DEVMGR_STOP_COMPLETE = 0,
  DS707_DEVMGR_STOP_PENDING  = 1,
  DS707_DEVMGR_STOP_FAIL     = 2
} ds707_devmgr_stop_reason;

#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/** PPP failure control block info
*/
typedef struct ds707_devmgr_ppp_failure_counts_type
{
  uint8                   soft_failure_count;  /**< ppp soft failure count */
  uint8                   hard_failure_count;  /**< ppp hard failure count */
  boolean                 max_ppp_failure_reached; /**< indicates max hard/soft
                                                        ppp failure occurred */
}ds707_devmgr_ppp_failure_counts_type;
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */

typedef struct ds707_devmgr_persist_cb_type
{
#ifdef FEATURE_EHRPD_HRPD_FALLBACK
  /** PPP failure control block for storing Hard/Soft failure
   *  counts for eHRPD to HRPD fallback feature. */
  ds707_devmgr_ppp_failure_counts_type ppp_failure_counts;
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */  

  /** Partial Context timer handle */
  rex_timer_type       pcmt_hdl;         
}ds707_devmgr_persist_cb_type;

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION DS707_DEVMGR_POWERUP_INIT
===========================================================================*/
/**
  Device manager powerup init

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_devmgr_powerup_init
(
  void
);

/*===========================================================================
FUNCTION DS707_DEVMGR_START
===========================================================================*/
/**
  Configures the PPP device, registers LCP & AUTH events

  @param device                     [in] PPP device
  @param data_sess_profile_info_ptr [in] Data session profile info pointer

  @return 
  0  -- Success \n
  -1 -- Failure \n
	
  @dependencies 
  This must be called once dssnet wants to bringup the device manager
*/
int	ds707_devmgr_start
(
  ppp_dev_enum_type                     device,
  ds707_data_session_profile_info_type *data_sess_profile_info_ptr
);

/*===========================================================================
FUNCTION DS707_DEVMGR_SET_PCMT_TO_EHRPD_VAL
===========================================================================*/
/**
  Re-configures the PCM timeout value to eHRPD standalone value.

  @param
  None

  @return 
  None
	
  @dependencies 
  None
*/
void ds707_devmgr_set_pcmt_to_ehrpd_val
(
  void
);

/*===========================================================================
FUNCTION      DS707_DEVMGR_PROCESS_CMD
===========================================================================*/
/**
  Process commands posted through DS

  @param cmd_ptr [in] Command pointer

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_process_cmd
(
  const ds_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION      DS707_DEVMGR_ALL_PDN_DOWN_IND
===========================================================================*/
/**
  Notifies device manager when all the PDNs are down

  @param
  None

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_all_pdn_down_ind
(
  void
);

/*===========================================================================
FUNCTION      DS707_DEVMGR_EPC_TO_NONEPC_HANDOFF_IND
===========================================================================*/
/**
  EPC to NONEPC handoff indication

  @param
  None

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_epc_to_nonepc_handoff_ind 
(
  void
);

/*===========================================================================
FUNCTION DS707_DEVMGR_ABORT
===========================================================================*/
/**
  Aborts device manager

  @param
  None

  @return 
  0 - Success
  -1 - Failure
	
  @dependencies 
  None
*/
int32 ds707_devmgr_abort 
(
  void
);

/*===========================================================================
FUNCTION DS707_DEVMGR_STOP
===========================================================================*/
/**
  Cleans up device manager

  @param
  None

  @return 
  ds707_devmgr_stop_reason - Success, Failure or Pending
	
  @dependencies 
  None
*/
ds707_devmgr_stop_reason ds707_devmgr_stop 
(
  void
);

/*===========================================================================
FUNCTION      DS707_DEVMGR_IN_PARTIAL_CTX
===========================================================================*/
/**
  Checks if device manager is in partial context

  @param
  None

  @return 
  TRUE  - devmgr is in partial context
  FALSE - devmgr is not in partial context

  @dependencies 
  None
*/
boolean ds707_devmgr_in_partial_ctx
(
  void
);

/*===========================================================================
FUNCTION      DS707_DEVMGR_UP_OR_LINGERING
===========================================================================*/
/**
  Checks if device manager is in UP or partial context (lingering) state

  @param
  None

  @return 
  TRUE  - devmgr is in UP/Lingering state
  FALSE - devmgr is not up

  @dependencies 
  None
*/
boolean ds707_devmgr_up_or_lingering
(
  void
);

/*===========================================================================
FUNCTION      DS707_DEVMGR_LTE_ATTACH_INIT_IND
===========================================================================*/
/**
  Indicates that the AT is attaching to LTE

  @param
  None

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_lte_attach_init_ind
(
  void
);

/*===========================================================================
FUNCTION      DS707_DEVMGR_DISABLE_PARTIAL_CONTEXT
===========================================================================*/
/**
  Disable partial context for the current call

  @param
  None

  @return 
  None

  @dependencies 
  None
*/
void ds707_devmgr_disable_partial_context
(
  void
);

/*===========================================================================
FUNCTION      DS707_DEVMGR_GET_STATE
===========================================================================*/
/**
  Returns device manager state

  @param
  None

  @return 
  state

  @dependencies 
  None
*/
ds707_devmgr_state_type ds707_devmgr_get_state 
(
  void
);

#ifdef FEATURE_EHRPD_HRPD_FALLBACK
/*===========================================================================
FUNCTION      DS707_DEVMGR_GET_PPP_FAILURE_COUNTS
===========================================================================*/
/**
  Returns the address of device manager ppp failure count structure

  @param
  None

  @return 
  ds707_devmgr_ppp_failure_counts_type *

  @dependencies 
  None
*/
ds707_devmgr_ppp_failure_counts_type* ds707_devmgr_get_ppp_failure_counts 
(
  void
);
#endif /* FEATURE_EHRPD_HRPD_FALLBACK */
#endif /* FEATURE_EHRPD */
#endif /* DS707_DEVMGR_H */
