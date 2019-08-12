#ifndef DS707_PKT_MGR_HDLR_H
#define DS707_PKT_MGR_HDLR_H
/*===========================================================================

                   D S 7 0 7 _ P K T _ M G R _ H D L R. H
                   
GENERAL DESCRIPTION
  This file contains the handlers for DS3GMGR functionality.
  This file also contains handlers for OptHo Tunnel Call functionality.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ds707_pkt_mgr_init() must be called at startup.

  Copyright (c) 2004 - 2014  by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pkt_mgr.c_v   1.45   25 Feb 2003 14:25:02   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_pkt_mgr_hdlr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/05/14    ms     DSDA Phase 2 changes.
06/26/14    ms     Data+MMS - DSDA changes Phase 1.
03/15/11    mg     Global variable cleanup
06/17/10    as     Fusion related additions
12/07/08    ms     Fixed off-target lint errors and compiler warnings
11/10/08    yz     Fixed RX/TX byte count issue with wmk approach,
                   reverted previous fix with rlp.
01/08/04    ak     First version of file.
===========================================================================*/



/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "enc.h"
#include "hdrl1_api.h"
#include "txc.h"
#include "rlgc.h"
#include "dsutil.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_sys_chg_hdlr.h"
#ifdef FEATURE_DATA_OPTHO
#include "ds707_pkt_mgri.h"
#endif /* FEATURE_DATA_OPTHO */
/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/
typedef boolean (*ds707_call_allowed_hdlr_f_type)
(
  uint32 pf_id,
  ps_sys_subscription_enum_type subs_id
);

/*===========================================================================
                          EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_PKT_MGR_HDLR_INIT

DESCRIPTION   Called at initialization.  Registers callbacks with 3G DSMGR.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkt_mgr_hdlr_init(void);

/*===========================================================================
FUNCTION      DS707_PKTI_CLEANUP_WMKS

DESCRIPTION   Called when phys link goes down.  Cleans up watermarks and
              queues.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_pkti_cleanup_wmks
(
  void
);
/*===========================================================================
FUNCTION      DS707_PKT_GET_DATA_COUNT

DESCRIPTION   Returns the count of data sent and received in the current data
              call


DEPENDENCIES  None.

RETURN VALUE  TRUE: If the values are correct
              FALSE: If the values are incorrect

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkt_get_data_count
(
  uint32                    *rxed_bytes,
  uint32                    *txed_bytes
);

#ifdef FEATURE_EPC_HANDOFF
/*===========================================================================
FUNCTION DS707_REG_CALL_ALLOWED_HDLR

DESCRIPTION
  This function is used to register the call allowed handler with the 3GPP2
  Mode Handler. This API is called by EPC Mode Handler. 

PARAMETERS
  call_allowed_hdlr_f - Pointer to the call allowed handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_reg_call_allowed_hdlr
(
  ds707_call_allowed_hdlr_f_type call_allowed_hdlr_f
);

/*===========================================================================
FUNCTION DS707_GET_CALL_ALLOWED_HDLR

DESCRIPTION
  This function is used to get the call allowed handler provided by EPC
  mode handler

PARAMETERS
  call_allowed_hdlr_f - Pointer to the call allowed handler provided by EPC MH

DEPENDENCIES
  None.

RETURN VALUE  
  None.

SIDE EFFECTS
  None.

===========================================================================*/
ds707_call_allowed_hdlr_f_type ds707_get_call_allowed_hdlr
(
  void
);
#endif /* FEATURE_EPC_HANDOFF */

/*===========================================================================
FUNCTION      DS707_PKT_MGR_SET_FAILED_ALN_CTA_RESTORE_VAL

DESCRIPTION   
  This function sets the value of failed_aln_cta_restore_val to the passed
  value

PARAMETERS   
  dword  failed_aln_cta_restore_val
  
RETURN VALUE  
  None

SIDE EFFECTS  
  None.
===========================================================================*/
void ds707_pkt_mgr_set_failed_aln_cta_restore_val(
  dword  failed_aln_cta_restore_val
);

#ifdef FEATURE_HDR
/*===========================================================================
FUNCTION     DS707_SET_ALN_CMD_PENDING

DESCRIPTION  This function sets the value of aln_cmd_pending flag.

PARAMETERS   boolean
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_set_aln_cmd_pending(boolean val);
#endif /* FEATURE_HDR */

/*===========================================================================
FUNCTION      DS707_PKTI_IS_SN_IFACES_ROUTABLE_OR_UP

DESCRIPTION   Util function to check if atleast one of the ifaces is up or 
              routeable 

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_pkti_is_sn_ifaces_routable_or_up
(
  void
);

#ifdef FEATURE_DATA_OPTHO
/*===========================================================================
FUNCTION     DS707_PKTI_TUNNEL_CALL_CONN_HDLR

DESCRIPTION  This handler is called by hdrpac when a 
             HDRIND_IDLE_CONNECTION_OPENED is received while in tunnel mode
             operation

PARAMETERS   None.
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_pkti_tunnel_phys_link_cleanup
(
  uint32 end_reason,
  ds707_tc_state_type *tc_state_ptr,
  ps_phys_link_type   *phys_link_ptr
);
/*===========================================================================
FUNCTION     DS707_PKTI_TUNNEL_CALL_CONN_HDLR

DESCRIPTION  This handler is called by hdrpac when a 
             HDRIND_IDLE_CONNECTION_OPENED is received while in tunnel mode
             operation

PARAMETERS   None.
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_pkti_tunnel_call_conn_hdlr
(
  void
);

/*===========================================================================
FUNCTION     DS707_PKTI_TUNNEL_CALL_ENDED_HDLR

DESCRIPTION  This handler is called by hdrpac when
             HDRIND_CONNECTED_CONNECTION_CLOSED
             is received while in tunnel mode operation

PARAMETERS   None.
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_pkti_tunnel_call_ended_hdlr
(
  uint32 end_reason
);

/*===========================================================================
FUNCTION     DS707_PKTI_TUNNEL_CALL_FAILED_HDLR

DESCRIPTION  This handler is called by hdrpac when
             HDRIND_IDLE_CONNECTION_FAILED
             is received while in tunnel mode operation

PARAMETERS   None.
  
RETURN VALUE None.

SIDE EFFECTS None.
===========================================================================*/
void ds707_pkti_tunnel_call_failed_hdlr
(
  uint32 end_reason,
  uint64 backoff_time
);
#endif /* FEATURE_DATA_OPTHO */

/*===========================================================================
FUNCTION     DS707_GET_LAST_GOOD_SYS

DESCRIPTION
  Gets the last known good sytem

PARAMETERS
  None

RETURN VALUE
  ds707_sys_info_enum_type - last known good system
===========================================================================*/
void ds707_set_last_good_sys
(
  ds707_sys_info_enum_type last_good_sys
);

/*===========================================================================
FUNCTION     DS707_GET_LAST_GOOD_SYSTEM

DESCRIPTION
  Gets the last known good sytem

PARAMETERS
  None

RETURN VALUE
  ds707_sys_info_enum_type - last known good system
===========================================================================*/
ds707_sys_info_enum_type ds707_get_last_good_sys
(
  void
);

/*===========================================================================
FUNCTION     ds707_clear_1x_rateset

DESCRIPTION
  Clears the 1x throughput state variables

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
void ds707_clear_1x_rateset
(
  void
);


/*===========================================================================
FUNCTION      DS707_FREE_THROUGHPUT_REPORT

DESCRIPTION   This function frees the memory  allocated for the global 
              throughput Information list if already allocated 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_free_throughput_report
(
   void
);

/*===========================================================================
FUNCTION      DS707_ALLOCATE_THROUGHPUT_INFO

DESCRIPTION   This function invokes the mode handlers to Fill the throughput 
              Info and also sends 'Throughput Info Indication' to PS
              
DEPENDENCIES  None

RETURN VALUE  Error on return

SIDE EFFECTS  None
===========================================================================*/
int16 ds707_allocate_throughput_info
(
   uint16 num_ifaces_in_use
);


/*===========================================================================
FUNCTION  DS_707_REPORT_ALL_IFACE_THROUGHPUT_INFO

DESCRIPTION 
 This Utility function reports the the throughput Information for all
 active DS 3GPP2 ifaces

PARAMETERS 
  iface_info_list - Location to fill in iface throughput related stats
  num_max_ifaces  - The throughput info for Max num of ifaces,
                    the info list can hold
                    
DEPENDENCIES 
  This utility function will be used only when
  * there is atleast one active DS 3GPP2 Iface
        &
  * Throughput Info Indication timer is running
RETURN VALUE
  TRUE   - If the throughput Info for the iface is reported successfully
 
  FALSE  - Otherwise
            
SIDE EFFECTS None

===========================================================================*/
boolean ds707_report_all_iface_throughput_info
(
   ps_sys_throughput_info_list_type *report_p,
   uint16 num_max_ifaces
);
/*===========================================================================
FUNCTION ds707_get_throughput_from_rpc

DESCRIPTION
  Depending on RPC rating, this function returns the throughput in Kbps

PARAMETERS
  scrm_sch_rate_type.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - Success
  FALSE - Failure
===========================================================================*/
uint32 ds707_get_throughput_from_rpc
(
  enc_sch_rate_type rpc_rating
);

/*===========================================================================
FUNCTION DS707_GET_THROUGHPUT_INFO

DESCRIPTION
  Depending on the current network mode, this function gets throughput
  information from the lower layers and generates THROUGHPUT_INFO_EVENT to
  PS framework

PARAMETERS
  Subscriber id.

DEPENDENCIES
  Call should be up.

RETURN VALUE
  TRUE - Success
  FALSE - Failure
===========================================================================*/
void ds707_get_throughput_info
(
  ds3gsubsmgr_subs_id_e_type subs_id /*Use ds3gsubs_type*/
);
/*===========================================================================
FUNCTION ds_3gpp2_calculate_actual_throughput

DESCRIPTION
  Depending on the current network mode, this function calculates throughput
  information from the lower layers and caches the value

PARAMETERS
  Sampling Interval - Unit in ms.

DEPENDENCIES
  Call should be up.

RETURN VALUE
  TRUE - Success
  FALSE - Failure
===========================================================================*/
void ds707_calculate_actual_throughput
(
  ds3gsubsmgr_subs_id_e_type cm_subs_id, 
  uint16 timer_expiry_count
);

/*===========================================================================
FUNCTION ds707_cost_of_modem_power

DESCRIPTION
  Depending on the current network mode, this function gets throughput
  information from the lower layers and generates THROUGHPUT_INFO_EVENT to
  PS framework

PARAMETERS
  Sampling Interval - Unit in ms.

DEPENDENCIES
  Call should be up.

RETURN VALUE
  TRUE - Success
  FALSE - Failure
===========================================================================*/
int16 ds707_cost_of_modem_power
(
  void               *argval_ptr,
  int16              *ps_errno
);

/*===========================================================================
FUNCTION     DS707PKTI__SET_IS_BSR_COMPLETED

DESCRIPTION
  Sets the bsr completed flag to TRUE, it blocks all further originations to CM.

PARAMETERS
  None

RETURN VALUE
  None
===========================================================================*/
void ds707_pkti_set_is_bsr_completed
(
  boolean value
);

/*===========================================================================
FUNCTION     DS707_PKTI_GET_IS_BSR_COMPLETED

DESCRIPTION
  Gets the bsr completed flag value, it blocks all further originations to CM.

PARAMETERS
  None

RETURN VALUE
  bsr_completed flag status
===========================================================================*/
boolean ds707_pkti_get_is_bsr_completed
(
  void
);

/*===========================================================================
FUNCTION  DS707_COEX_DATA_FLOW_ENABLE_HDLR

DESCRIPTION
  This function enable PS flows for a subscription with COEX mask.

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_coex_data_flow_enable_hdlr
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS707_COEX_DATA_FLOW_DISABLE_HDLR

DESCRIPTION
  This function disable PS flows for a subscription with COEX mask.

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_coex_data_flow_disable_hdlr
(
  sys_modem_as_id_e_type  subs_id
);

/*===========================================================================
FUNCTION  DS707_CONFIG_MODEM_POWER_STATS_HDLR

DESCRIPTION
  This function disable or enabled MODEM power stats by calling lower layer level APIs

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_config_modem_power_stats_hdlr
(
  sys_modem_as_id_e_type  subs_id,
  boolean enable_flag
);


/*===========================================================================
FUNCTION  DS707_GET_MODEM_POWER_STATS_HDLR

DESCRIPTION
  This function queries the lower layers and fills in modem power stats back to DS3G framework

PARAMETERS
  subs_id: subscription id

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds707_get_modem_power_stats_hdlr
(
  sys_modem_as_id_e_type  subs_id,
  uint32 tx_power_bin[]
);



#endif /* FEATURE_DATA_IS707   */
#endif /* DS707_PKT_MGR_HDLR_H */

