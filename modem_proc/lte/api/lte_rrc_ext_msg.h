/*!
  @file
  lte_rrc_ext_msg.h

  @brief
  This file describes the interfaces to LTE Radio resource Controller (RRC)

   <!-- Doxygen chokes on the @mainpage vs. @page inside a conditional, but the
       following seems to work.  Basically, make this the mainpage if it
       is the only component being built, otherwise, just make it a page and
       let the upper level component/s @ref it. -->
  @if rrc 
  @mainpage LTE Radio Resource Controller (RRC)
  @endif
  @ifnot rrc
  @page LTE Radio Resource Controller (RRC)
  @endif

  RRC module is the heart of the configuration and control for the LTE access 
  stratum. This layer intercepts all the over the air messages and configures 
  all the lower layer modules. This module is also in charge if 
  tracking system information change, connection set up , security and keeping 
  the connection up through mobility control.  This module also acts as the 
  access stratum's interface to the NAS layer. 

  RRC is a single task architecture and consists of a set of co-operating state 
  machines which communicate with each other, and with external modules, via 
  messages with unique message ids (UMIDs). All the messages posted by RRC to 
  modules outside of RRC is always through the message router. Internally 
  almost all of the major modules and procedures of RRC are modeled as state 
  machines. State machines inside of RRC can communicate either synchronously 
  ( i.e without invoking a message router or a CS call) or asynchronously using 
  the message router /CS call. Either way all the received external and internal
   UMIDs of RRC go through a common RRC dispatcher which has the knowledge of 
   the recipient state machines for the various UMIds. 

  <h2>High level architecture</h2>
  <img src='../pics/rrc_arch.jpg'>
  
  <h2>API</h2>
 <h3><i>Message Interfaces</i></h3> 
   -- External API to RRC is defined in @ref lte_rrc_ext_msg.h
  
 <h3><i>Function Call Interfaces</i></h3> 
  This module does not have any function call API.

  <b><i> QXDM Log Parser </i></b>
  - @ref QXDM_Parser_DLL

  @ref umids
  @ref all_umids

*/


/*===========================================================================

  Copyright (c) 2008 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/api/lte_rrc_ext_msg.h#1 $

when       who       what, where, why
--------   ---       ---------------------------------------------------------- 
10/01/13   kp        Added LTE Load Balancing for VoLTE Call interface changes NAS-RRC. 
08/07/13   kp        Added DSDS interface changes for NAS-RRC. 
03/13/12   kp        Added code to perform ac-barring for AC-11 to 15 from param provided 
                     by NAS when SIB2 says no AC barring for AC-11 to 15.  
03/12/12   kp        Added code to provide ac-barring info from SIB2 to NAS in srv_ind  
11/14/11   kp        removed nas_msg_container from lte_rrc_conn_est_req_s structure 
                     as we have moved to use DSM ptrs.
10/03/11   sureshs   Added SSAC params to camped cell info
09/30/11   gpotnuru  Added new plmn search status LTE_RRC_PLMN_SEARCH_ABORTED_ON_RLF
08/25/11   zz        Added RRC-DS and RRC-QMI_NAS interfaces for eMBMS
05/27/11   vatsac    Added support using for ims emergency from Rel9 SIB1
05/25/11   np        Define interface for LTE key gen for W2L PSHO
05/13/11   vatsac    Added forbidden_manual_ta_list to lte_rrc_service_req_s and
                     lte_rrc_system_update_s
09/10/10   np        Switch to DSM items for NAS DT messages
02/01/11   mm        Added definitions for passing CMAS notification to upper layers
01/21/11   vatsac    Added service req type field
12/08/10   np        Add corresponding abort reason after aborting and releasing conn.
12/06/10   np        Add abort cause to lte_rrc_conn_abort_req_s for L2W PSHO
11/08/10   np        Define interface for UMTS key gen for L2W PSHO
09/10/10   np        e1xCSFB Interfaces for HO to 1xCP
08/25/10   np        Add Redirection Information to Connection Release for CSFB
08/05/10   amit      Added LTE_RRC_ETWS_MSG_ID_LIST_REQ and corresponding structures
06/28/10   amit      Added PLMN to etws_prim_ind and etws_sec_ind 
06/23/10   np        Consolidate IRAT Tunnel into unified message
06/21/10   np        Add payload to lte_rrc_irat_ul_msg_req_s
06/17/10   np        IRAT Tunnel Support for 1xCSFB Pre-Reg
05/10/10   da        Added Msgs for NMR
04/21/10   sureshs   Added use_oos field to Service Indication
02/22/10   amit      Added indications for ETWS primary and secondary notifications
02/08/10   sureshs   Added Network Select Mode Reset Request
02/02/10   vatsac    Seperating out IRAT msgs from lte_rrc_ext_msg.h
02/02/10   mm        Added a SIB_READ_ERROR cause for Connection Release
02/02/10   vatsac    Removed support for irat_info in service req
01/26/10   np        W IRAT Capabilities placeholder
01/26/10   np        Generic definition of PLMN for IRAT
01/24/10   sk        Added support for G to L Reselection
01/15/10   np        W -> L IRAT Resel/Redir UMIDs
01/14/10   sk        Added dummy data types for LTE  -> G reselection and 
                     LTE --> G redirection
01/13/10   sk        Removed placeholders for LTE --> DO reselection/redirection
01/07/10   np        Minor update to lte_rrc_lte_to_w_resel_req_s
01/07/10   np        Add stub payload for  L -> W Redir
12/22/09   np        Added stub types for WCDMA RRC Redir Interface
12/18/09   np        Added stub types for WCDMA RRC Resel Interface
12/15/09   np        Removed the temp LTE -> 1x IRAT placeholders
12/06/09   sk        Added dummy types for LTE to DO reselection
10/19/09   sureshs   Added RAT priority list to Service Request
09/21/09   np        Added dummy data types for LTE to 1X Redirection support.
09/18/09   sureshs   Added CDMA support fields to IRAT info
09/17/09   sureshs   Added no_svc_cause field to Service Indication
09/10/09   sureshs   Added quality field to found PLMN list element
09/02/09   sk        Added dummy data types for LTE to DO Redirection support.
08/26/09   sureshs   Added t311_rem_time field to service indication
08/20/09   vatsac    Removed redirection related info from NAS service ind 
08/05/09   sureshs   Changed dl_arfcn in redir info to uint32 for March spec
07/29/09   vatsac    Added cell_identity in lte_rrc_conn_mode_cell_change_ind_s
07/28/09   vatsac    Added cell_identity in lte_rrc_service_ind_s to NAS
07/01/09   sureshs   Added some comments
05/22/09   sureshs   Changed LTE_RRC_MAX_NUM_TRACKING_AREAS to 20
05/04/09   vatsac    Updated lte_rrc_conn_rel_reason_e, lte_rrc_service_ind_s
                     and lte_rrc_deactivate_req_s to support inter-RAT redir
04/20/09   dd        Modified nas_count field to uint32 in security_info
04/07/09   sureshs   Added search_done field to service indication
01/19/09   sureshs   Added support for multiple access classes
01/14/09   sureshs   Added found plmn type and list to include rssi
12/18/08   sureshs   Added new fields in Cell Change Ind
12/18/08   dd        Added ConnEstTimerUpdateInd. Added is_high_priority boolean
                     flag to ConnEstReq
12/10/08   vatsac    Added a new value to enum lte_rrc_ul_data_cnf_status_e
                     and increased MAX_NAS_MSG_CONTAINER_SIZE to 4K
12/09/08   vatsac    Increased size of trans_id for msgs to/from NAS
12/09/08   sureshs   Conn Abort and Svc indication have phy_cell_id fields,
                     trans_id in svc req and ind increased to 32 bits
12/08/08   vatsac    Increased MAX_NAS_MSG_CONTAINER_SIZE to 2K
12/08/08   sureshs   Added EPLMN list to Service Request, removed ehpln valid
                     flag
12/05/08   sureshs   Removed Conn Abort Cnf
11/26/08   sureshs   Removed cause field in Conn Abort Req
11/24/08   sureshs   Added PLMN to TA struct
11/18/08   sureshs   Made EHPLMN into a list
11/17/08   sureshs   Clarified size of EPLMN list
11/13/08   sureshs   Enhanced field in UL DATA CNF
11/05/08   ask       Ported RRC to POSIX APIs
11/03/08   sureshs   Renamed new_scan to scan_is_new
11/03/08   sureshs   Made list into single entry for selected PLMN in svc ind
10/29/08   sureshs   Added EHPLMN to service request, and new payload for
                     System Update message
10/16/08   sureshs   Incorporated changes to NAS interface after meeting
10/13/08   dd        Updated ConnEstCnf and expanded the conn establishment 
                     status codes
10/10/08   dd        Added access class to SIMUpdate
10/06/08   sureshs   Added selected band field
10/06/08   sureshs   Removed num_mcc_digits
10/04/08   sureshs   Included sys.h, added band preference bit mask
10/03/08   sureshs   Added num_mcc_digits field to lte_rrc_plmn_s and made the
                     max number of digits as defines
10/01/08   vr        Changed PLMN to PLMN List in Service Ind to NAS
09/11/08   sureshs   Added cell barring reqd field to Conn Abort Req
09/09/08   dd        Added est_failure to conn est status enum 
09/09/08   sureshs   Added load balancing required flag to conn released ind
08/27/08   vr        Added typedefs for cell id and tracking area code
07/03/08   sureshs   Initial version
===========================================================================*/

#ifndef LTE_RRC_EXT_MSG_H
#define LTE_RRC_EXT_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include <msgr.h>
#include <msgr_lte.h>
#include <lte_app.h>
#include <sys.h>
#include <sys_plmn_selection.h>
#include <lte_cphy_msg.h>
#include <lte_as.h>
#include <lte.h>
#include <lte_irat_types.h>
#include <cm.h>
#include <lte_misc.h>
#include <policyman_msg.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief External defines
*/
#define LTE_RRC_MAX_PLMN_LIST_SIZE 16 /*!< Maximum PLMN list size TBD */
#define LTE_RRC_MAX_NUM_TRACKING_AREAS 40 /*!< Maximum number of tracking areas 
                                               - like 
                                             SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH 
                                               in sys.h */
#define LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE 2048
                              /*!< Maximum size of NAS message container TBD */
#define LTE_RRC_MAX_UE_IDENTITY_TYPES 5 
                                /*!< Maximum number of UE identity types TBD */
#define LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS 11 
                               /*!< Maximum number of active EPS bearers TBD */

#define LTE_RRC_MAX_EPS_BEARER_ID 15 
                     /*!< Maximum value for EPS bearer ID as defined in ASN.1 */
#define LTE_RRC_ASYNC_SRV_IND_TRANS_ID    0xFFFFFFFF  
                          /*!< Reserved Trans id for Async Serv Ind from RRC */ 
#define LTE_RRC_MAX_MCC_DIGITS 3 /*!< Maximum number of digits in MCC */
#define LTE_RRC_MAX_MNC_DIGITS 3 /*!< Maximum number of digits in MNC */

#define LTE_RRC_MAX_NUM_CSGS 2*SYS_CSG_LIST_MAX_LENGTH /*!< Maximimum CSG list size */

#define LTE_RRC_INDEFINITE_BARRING_TIME 0xffffffff
/*!<This means that the barring is indefinitely valid  */

#define LTE_RRC_SERVICE_REQ_TIMEOUT_MAX 0xffffffff/*!<This means that service request should not be timed out  */

#define LTE_RRC_INVALID_ACCESS_CLASS 0xffff
/*!< Invalid Access Class*/


#define LTE_RRC_ETWS_WARN_TYPE_LEN 2

#define LTE_RRC_ETWS_WARN_SEC_INFO_LEN 50

/*!< Defines the max message size for measurement report included in NMR_INFO_CNF */
#define LTE_RRC_NMR_INFO_MAX_MSG_SIZE 256

/*!< Defines the max size of the list in MSG_ID_LIST_REQ */
/* TO DO: Remove after WMS removes reference to the old macro */
#define LTE_RRC_ETWS_MAX_MSG_ID_LIST_SIZE 20
#define LTE_RRC_MAX_MSG_ID_LIST_SIZE 50

#define LTE_RRC_NAS_SEC_PARAM_TO_EUTRA_LEN 6

#define LTE_RRC_ACCESS_BARRING_PROB_p100 100

#define LTE_RRC_ACCESS_BARRING_PROB_p0 0

#define LTE_RRC_ACCESS_BARRING_PROB_p1 1

#define LTE_RRC_ACCESS_BARRING_TIME_0 0

#define LTE_RRC_ACCESS_BARRING_TIME_20 20

/*!< Defines access barring probability table size*/
#define LTE_RRC_AC_BARRING_PROB_TABLE_SIZE 16

/* Max number of SAIs LTE Modem will search to activate a TMGI */
#define LTE_RRC_EMBMS_MAX_NUM_SAI_TO_SRCH 64

/* Max number of Freq LTE Modem will search to activate a TMGI , 
  currently modem will search atmost 8 freq, based on the max inter-freq
  neighbors specified in SIB5 */
#define LTE_RRC_EMBMS_MAX_NUM_FREQ_TO_SRCH 64

/*! Maximum number of PMCH DATAMCS INFO */
#define LTE_MAX_NUM_PMCH_DATAMCS_INFO LTE_MAX_ACTIVE_MRB

/*! Maximum number of Cells provided */
#define LTE_RRC_MAX_NUM_CELL_LOCK 20

/* Type for ETWS and CMAS msg id */
typedef uint16 lte_rrc_msg_id_t;
/* TO DO: Remove after WMS removes reference to the old type */
typedef lte_rrc_msg_id_t lte_rrc_etws_msg_id_t;

/* Type for Global Cell id */
typedef uint32 lte_rrc_global_cell_id_t;

/*! @brief External enumerations
*/

/*! @brief Enumeration for service status
*/
typedef enum
{
  LTE_RRC_SVC_STATUS_NOT_AVAILABLE, /*!< 0 */
  LTE_RRC_SVC_STATUS_AVAILABLE, /*!< 1 */
  LTE_RRC_SVC_STATUS_MAX 
} lte_rrc_svc_status_e;

/*! @brief Enumeration for cell access status
*/
typedef enum
{
  LTE_RRC_CELL_ACCESS_NORMAL_ONLY, /*!< 0 */
  LTE_RRC_CELL_ACCESS_EMERGENCY_ONLY, /*!< 1 */
  LTE_RRC_CELL_ACCESS_NO_CALLS, /*!< 2 */
  LTE_RRC_CELL_ACCESS_ALL_CALLS, /*!< 3 */
  LTE_RRC_CELL_ACCESS_MAX 
} lte_rrc_cell_access_status_e;

/*! @brief Enumeration for establishment cause
*/
typedef enum
{
  LTE_RRC_EST_CAUSE_EMERGENCY, /*!< 0 */
  LTE_RRC_EST_CAUSE_HIGH_PRIORITY, /*!< 1 */
  LTE_RRC_EST_CAUSE_MT_ACCESS, /*!< 2 */
  LTE_RRC_EST_CAUSE_MO_SIGNALING, /*!< 3 */
  LTE_RRC_EST_CAUSE_MO_DATA, /*!< 4 */
  LTE_RRC_EST_CAUSE_MO_CSFB, /*!< 5 */
  LTE_RRC_EST_CAUSE_DELAY_TOLERANT_ACCESS, /*!< 6 */
  LTE_RRC_EST_CAUSE_MAX
} lte_rrc_est_cause_e;

/*! @brief Enumeration for UL data request priority
*/
typedef enum
{
  LTE_RRC_UL_NAS_MSG_PRIORITY_HIGH, /*!< 0 */
  LTE_RRC_UL_NAS_MSG_PRIORITY_LOW, /*!< 1 */
  LTE_RRC_UL_NAS_MSG_PRIORITY_MAX
} lte_rrc_ul_nas_msg_priority_e;

/*! @brief Enumeration for connection establishment status
*/
typedef enum
{
  LTE_RRC_CONN_EST_SUCCESS, /*!< 0 */
  LTE_RRC_CONN_EST_FAILURE, /*!< 1*/
  LTE_RRC_CONN_EST_FAILURE_ABORTED, /*!< 2 */
  /*!< Failure as a result of an Abort*/
  LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED, /*!< 3*/
  /*!< Failure as a result of Access barring*/
  LTE_RRC_CONN_EST_FAILURE_CELL_RESEL, /*!< 4*/
  /*!< Failure as a result of cell reselection*/
  LTE_RRC_CONN_EST_FAILURE_CONFIG_FAILURE,/*!< 5*/
  /*!< SRB1 Config fails */
  LTE_RRC_CONN_EST_FAILURE_TIMER_EXPIRED, /*!< 6*/
  /*!< T300 expiry for max number of retries*/
  LTE_RRC_CONN_EST_FAILURE_LINK_FAILURE,/*!< 7*/
  /*!< If ConnSetupCompleteTx fails or any other link error */
  LTE_RRC_CONN_EST_FAILURE_NOT_CAMPED, /*!< 8*/
  /*!<Failure as a result of not being camped */ 
  LTE_RRC_CONN_EST_FAILURE_SI_FAILURE, /*!< 9*/
  /*Failure as a result of invalid System Information*/
  LTE_RRC_CONN_EST_FAILURE_CONN_REJECT, /*!< 10*/
  /*Failure as a result of rxing ConnectionReject from eNB*/
  LTE_RRC_CONN_EST_FAILURE_REQ_ERROR, /*!< 11*/
  /*Failure as a result of bad ConnEstReq from NAS*/
  LTE_RRC_CONN_EST_FAILURE_TAI_CHANGE, /*!< 12*/
  /*Failure as a result of TA or PLMN change */
  LTE_RRC_CONN_EST_FAILURE_RF_UNAVAILABLE, /*!< 13*/
  /*Failure as a result of RF unavailable*/
   LTE_RRC_CONN_EST_FAILURE_ACCESS_BARRED_EAB, /*!< 14*/
  /*Failure as a result of Access Barring due to EAB*/
  LTE_RRC_CONN_EST_MAX
} lte_rrc_conn_est_status_e;

/*! @brief Enumeration for redirection rat type
*/
typedef enum
{
  LTE_RRC_REDIR_RAT_LTE,            /*!< 0 */
  LTE_RRC_REDIR_RAT_GERAN,          /*!< 1 */
  LTE_RRC_REDIR_RAT_UTRA_FDD,       /*!< 2 */
  LTE_RRC_REDIR_RAT_UTRA_TDD,       /*!< 3 */
  LTE_RRC_REDIR_RAT_CDMA2000_HRPD,  /*!< 4 */
  LTE_RRC_REDIR_RAT_CDMA2000_1xRTT, /*!< 5 */
  LTE_RRC_REDIR_RAT_MAX
} lte_rrc_redir_rat_type_e;

/*! @brief Enumeration for connection release cause
*/
typedef enum
{
  LTE_RRC_CONN_REL_NORMAL, /*!< 0 */
  LTE_RRC_CONN_REL_RLF, /*!< 1 */
  LTE_RRC_CONN_REL_CRE_FAILURE, /*!< 2 */
  LTE_RRC_CONN_REL_OOS_DURING_CRE, /*!< 3 */
  LTE_RRC_CONN_REL_ABORTED, /*!< 4 */
  LTE_RRC_CONN_REL_SIB_READ_ERROR,  /*!< 5 */ 
  LTE_RRC_CONN_REL_ABORTED_IRAT_SUCCESS, /*!< 6 */
  LTE_RRC_CONN_REL_RLF_SEC_NOT_ACTIVE, /*!< 7 */
  LTE_RRC_CONN_REL_IRAT_TO_LTE_ABORTED, /*!< 8 */
  LTE_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_SUCCESS, /*!< 9 */
  LTE_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_CCO_ABORTED, /*!< 10 */
  LTE_RRC_CONN_REL_IRAT_FROM_LTE_TO_G_SGLTE_SRVCC_SUCCESS, /*!< 11 */
  LTE_RRC_CONN_REL_RLF_NONDDS_SPV_TMR_EXPIRY, /*!< 12*/
  LTE_RRC_CONN_REL_MAX
} lte_rrc_conn_rel_reason_e;

/*! @brief Enumeration for activation reason
*/
typedef enum
{
  LTE_RRC_ACT_IRAT_CELL_RESEL, /*!< 0 */
  LTE_RRC_ACT_IRAT_CCO_FAILURE, /*!< 1 */
  LTE_RRC_ACT_IRAT_HO, /*!< 2 */
  LTE_RRC_ACT_IRAT_REDIR, /*!< 3 */
  LTE_RRC_ACT_MAX
} lte_rrc_act_reason_e;

/*! @brief Enumeration for UL DATA CNF status
*/
typedef enum
{
  LTE_RRC_UL_DATA_CNF_SUCCESS, /*!< 0 */
  LTE_RRC_UL_DATA_CNF_FAILURE_TXN, /*!< 1 */
  LTE_RRC_UL_DATA_CNF_FAILURE_HO, /*!< 2 */
  LTE_RRC_UL_DATA_CNF_FAILURE_CONN_REL, /*!< 3 */
  LTE_RRC_UL_DATA_CNF_FAILURE_RLF, /*!< 4 */
  LTE_RRC_UL_DATA_CNF_FAILURE_CTRL_NOT_CONN, /*!< 5 */

} lte_rrc_ul_data_cnf_status_e;

/*! @brief Enumeration for IRAT Tunnel UL Msg CNF status
*/
typedef enum
{
  LTE_RRC_TUNNEL_UL_MSG_CNF_SUCCESS, /*!< 0 */
  LTE_RRC_TUNNEL_UL_MSG_CNF_FAILURE_TXN, /*!< 1 */
  LTE_RRC_TUNNEL_UL_MSG_CNF_FAILURE_HO, /*!< 2 */
  LTE_RRC_TUNNEL_UL_MSG_CNF_FAILURE_CONN_REL, /*!< 3 */
  LTE_RRC_TUNNEL_UL_MSG_CNF_FAILURE_RLF, /*!< 4 */
  LTE_RRC_TUNNEL_UL_MSG_CNF_FAILURE_CTRL_NOT_CONN, /*!< 5 */
  LTE_RRC_TUNNEL_UL_MSG_CNF_FAILURE_SRB2_NOT_UP, /*!< 6 */
  LTE_RRC_TUNNEL_UL_MSG_CNF_FAILURE_REQ_ERROR, /*!< 7 */

} lte_rrc_tunnel_ul_msg_cnf_status_e;

/*! @brief Enumeration for PLMN Search Status
 */
typedef enum
{
  LTE_RRC_PLMN_SEARCH_COMPLETED, /*!< 0 */
  LTE_RRC_PLMN_SEARCH_TIMED_OUT, /*!< 1 */
  LTE_RRC_PLMN_SEARCH_ABORT_CNF, /*!< 2 */
  LTE_RRC_PLMN_SEARCH_ABORTED, /*!< 3 */
  LTE_RRC_PLMN_SEARCH_ABORTED_ON_RLF, /*!< 4 */
  LTE_RRC_PLMN_SEARCH_PARTIAL, /*!<5*/
} lte_rrc_plmn_search_status_e;

/*! @brief Enumeration for Service Req Type
*/
typedef enum
{
  LTE_RRC_SERVICE_REQ_TYPE_NORMAL, /*!< 0 */
  LTE_RRC_SERVICE_REQ_TYPE_BSR, /*!< 1 */
  LTE_RRC_SERVICE_REQ_TYPE_1xCSFB_END,/*!< 2*/
} lte_rrc_service_req_type_e;

/*! @brief Enumeration for eMBMS deactivation reason
*/
typedef enum
{
  LTE_RRC_EMBMS_DEACT_LTE_STOPPED, /*!< 0 - LTE is not available, 
                                            e.g. UE camps on other RATs */
  LTE_RRC_EMBMS_DEACT_SESSION_END, /*!< 1 - When an eMBMS session end, 
                                            e.g. file download session ends */
  LTE_RRC_EMBMS_DEACT_OUT_OF_COVERAGE, /*!< 2 - When UE is out of eMBMS coverage */
  LTE_RRC_EMBMS_DEACT_SESSION_NOT_FOUND, /*!< 3 - eMBMS session was not found on any SAI */
  LTE_RRC_EMBMS_DEACT_REASON_MAX
} lte_rrc_embms_deact_reason_e;

/*! @brief Enumeration for eMBMS OOS reason
*/
typedef enum
{
  LTE_RRC_EMBMS_WARN_REASON_UCAST_OOS, /*!< 0 - Warning for Unicast OOS */

  LTE_RRC_EMBMS_WARN_REASON_MCAST_OOS, /*!< 1 - Warning for eMBMS OOS */

  LTE_RRC_EMBMS_WARN_REASON_OOS_CLEARED, /*!< 2 - Warning cleared for OOS */

} lte_rrc_embms_warn_reason_e;

/*! @brief External data structures
*/


#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

/*! @brief OTA message log structure
*/
typedef PACK(struct)
{
  uint8 log_packet_ver;  /*!< Log packet version */
  uint8 rrc_rel;  /*!< RRC release number */
  uint8 rrc_ver;  /*!< RRC version number; if 8.x.y, left 4 bits are for x, 
                       right 4 bits are for y */
  uint8 rb_id;  /*!< Radio bearer id */
  uint16 phy_cell_id;  /*!< Physical cell ID */
  uint32 freq;  /*!< Frequency of the cell */
  lte_sfn_s sfn;  /*!< SFN on which message was sent/received - 0 if N/A */
  uint8 pdu_num;  /*!< Identifies the message type
                       2 - BCCH_DL_SCH Message
                       3 - PCCH Message
                       4 - DL_CCCH Message
                       5 - DL_DCCH Message
                       6 - UL_CCCH Message
                       7 - UL_DCCH Message */
  uint32 sib_mask_in_si;/*!< Bitmask of available sibs in BCCH_DL_SCH
                                    Will be 0 if no SIBS are availabe in BCCH_DL_SCH
                                    or if the msg is not of type BCCH_DL_SCH*/                        
  uint16 encoded_msg_len;  /*!< Length of ASN.1 encoded message */
  uint8 encoded_msg_first_byte;  /*!< First byte of the ASN.1 encoded message 
                                      sent/received (present only when message 
                                      length <= LTE_RRC_LOG_MAX_ENCODED_MSG_LEN) */
} lte_rrc_log_ota_msg_s;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif


/*! @brief Data structure for PLMN type
*/
typedef struct
{
  uint8 mcc[LTE_RRC_MAX_MCC_DIGITS]; /*!< Mobile country code */
  uint8 num_mnc_digits; /*!< Number of digits in MNC */
  uint8 mnc[LTE_RRC_MAX_MNC_DIGITS]; /*!< Mobile network code */
} lte_rrc_plmn_s;

/*! @brief Data structure for PLMN list type
*/
typedef struct
{
  uint32 num_plmns; /*!< Number of PLMNs */
  lte_rrc_plmn_s plmn[LTE_RRC_MAX_PLMN_LIST_SIZE]; /*!< List of PLMNs */
} lte_rrc_plmn_list_s;

/*! @brief Data structure for EHPLMN list type
*/
typedef struct
{
  uint32 num_plmns; /*!< Number of PLMNs */
  lte_rrc_plmn_s plmn[SYS_EHPLMN_LIST_MAX_LENGTH]; /*!< List of PLMNs */
} lte_rrc_ehplmn_list_s;

/*! @brief Data structure for found PLMN type
*/
typedef struct
{
  lte_rrc_plmn_s plmn; /*!< PLMN ID */
  int16 rsrp; /*!< RSRP, in dBm, if available, otherwise 0x8000 */
  /*! Quality, based on RSRP above, SYS_SIGNAL_QUALITY_LOW is not available  */

  int16 rssi; /*!< RSSI, in dBm, if available, otherwise 0x8000 */
  /*! Quality, based on rssi above, SYS_SIGNAL_QUALITY_LOW is not available  */
  sys_signal_quality_e_type quality; 
} lte_rrc_found_plmn_s;

/*! @brief Data structure for found PLMN list type
*/
typedef struct
{
  uint32 num_found_plmns; /*!< Number of found PLMNs */
  lte_rrc_found_plmn_s found_plmn[LTE_RRC_MAX_PLMN_LIST_SIZE]; 
                    /*!< List of found PLMNs */
} lte_rrc_found_plmn_list_s;

/*! @brief Data structure for TA type
*/
typedef struct
{
  uint16 tac;
  lte_rrc_plmn_s plmn;
} lte_rrc_tracking_area_s;

/*! @brief Data structure for TA list type
*/
typedef struct
{
  uint8 num_entries; /*!< Number of entries */
  /*! List of TAs */
  lte_rrc_tracking_area_s tracking_area[LTE_RRC_MAX_NUM_TRACKING_AREAS]; 
} lte_rrc_tracking_area_list_s;

/*! @brief Enumeration for CSG List Type
*/
typedef enum
{
  LTE_RRC_CSG_LIST_UNINITIALIZED, /*!< 0 */
  LTE_RRC_CSG_LIST_INITIALIZED, /*!< 1 */
} lte_rrc_csg_list_type_e;

/*! @brief Data structure for CSG Info
*/
typedef struct
{
  sys_csg_id_type csg_id; /*!< CSG Id */
  lte_rrc_plmn_s plmn; /*!< PLMN Id */
} lte_rrc_csg_list_info_s;

/*! @brief Data structure for CSG list type
*/
typedef struct
{
  uint8 num_entries; /*!< Number of entries */
  /*! List of CSGs */
  lte_rrc_csg_list_info_s csg_info[LTE_RRC_MAX_NUM_CSGS]; 
} lte_rrc_csg_list_s;

/*! @brief Data structure for Multi Mode info
*/
typedef struct
{
  boolean wcdma_is_supported; /*!< Is WCDMA supported? */
  boolean gsm_is_supported; /*!< Is GSM supported? */
  boolean cdma_1x_is_supported; /*!< Is CDMA 1X supported? */
  boolean cdma_do_is_supported; /*!< Is DO supported? */
  sys_band_mask_type band_preference; /*!< 3G band bit mask */
} lte_rrc_irat_info_s;

/*! @brief Data structure for NAS message container
*/
typedef struct
{
  uint16 msg_size; /*!< Size of message in bytes */
  uint8 msg_payload[LTE_RRC_MAX_NAS_MSG_CONTAINER_SIZE]; /*!< Message data */
} lte_rrc_nas_msg_container_s;

/*! @brief Data structure for security info
*/
typedef struct
{
  uint8 eksi; /*!< Key Set Index */
  uint8 k_asme[LTE_GENERATOR_KEY_LEN]; /*!< K_ASME*/
  uint32 nas_count; /*!< NAS uplink count*/
  boolean flush_nas_security_context;/*Should all NAS security 
                                      contexts be flushed ? If this field is set
                                      to TRUE, the rest of the NAS context info is
                                      irrelevant*/
} lte_rrc_security_info_s;


/*! @brief Enumeration of types of UE Ids
*/
typedef enum
{
  LTE_RRC_UE_ID_STMSI   = 0,      /*!< 0 */
  LTE_RRC_UE_ID_IMSI    = 1,      /*!< 1 */
  LTE_RRC_UE_ID_CRNTI   = 2,      /*!< 2 */

  LTE_RRC_UE_ID_MAX     = 3 

} lte_rrc_ue_id_type_e;


/*! @brief Defines members of the S-TMSI UE Id
*/
typedef struct
{
  uint32 m_tmsi;  /*!< Brief description of this member */
  uint8  mmec;

} lte_rrc_s_tmsi_s;

#define LTE_RRC_MIN_IMSI_DIGITS 6
#define LTE_RRC_MAX_IMSI_DIGITS 21

/*! @brief Defines members of the IMSI UE Id
*/
typedef struct
{
  uint8 num_digits;                            /*!< Number of digits in the 
                                                    imsi_digits field */ 
  uint8 imsi_digits[LTE_RRC_MAX_IMSI_DIGITS];  /*!< 6..21 digits of IMSI */

} lte_rrc_imsi_s;


/*! @brief Union of different UE Id values
*/
typedef union
{
  uint16            c_rnti;
  lte_rrc_s_tmsi_s  s_tmsi;
  lte_rrc_imsi_s    imsi;

} lte_rrc_ue_id_val_u;


/*! @brief Data structure for UE identity
*/
typedef struct
{
  lte_rrc_ue_id_type_e  type;    /*!< Type of UE Id */
  lte_rrc_ue_id_val_u   value;   /*!< UE Id value */

} lte_rrc_ue_id_s;

/*! @brief Data structure for UE identity list
*/
typedef struct
{
  uint8 num_ue_identities;
  lte_rrc_ue_id_s ue_id[LTE_RRC_MAX_UE_IDENTITY_TYPES]; /* UE identity 
                                                                   list */
} lte_rrc_ue_id_list_s;

/*! @brief Data structure for SSAC params
*/
typedef struct
{
  boolean ssac_param_voice_valid; /*!< SSAC mmtel voice param valid (SSAC parameters present in SIB)*/
  uint8 barring_factor_mmtel_voice;/*!< Access Barring factor for MMTEL voice ptr*/
  uint16 barring_time_mmtel_voice; /*!< Access Barring time for MMTEL voice ptr*/
  uint8 sib2_barring_factor_mmtel_voice;/*!< Access Barring factor for MMTEL voice ptr*/
  uint16 sib2_barring_time_mmtel_voice; /*!< Access Barring time for MMTEL voice ptr*/
  boolean ssac_param_video_valid; /*!< SSAC mmtel video param valid (SSAC parameters present in SIB)*/
  uint8 barring_factor_mmtel_video; /*!< Access barring factor for MMTEL video ptr*/
  uint16 barring_time_mmtel_video; /*!< Access Barring time for MMTEL video ptr*/
  uint8 sib2_barring_factor_mmtel_video; /*!< Access barring factor for MMTEL video ptr*/
  uint16 sib2_barring_time_mmtel_video; /*!< Access Barring time for MMTEL video ptr*/
} lte_rrc_ssac_params_s;

/*
ac barring factor, NAS when decide AC is barred then it has to use to use BarringFactor in the new algo
*/

typedef enum
{
  LTE_RRC_AC_BARRING_FACTOR_P00,  /*!< 0 */
  LTE_RRC_AC_BARRING_FACTOR_P05,  /*!< 5 */
  LTE_RRC_AC_BARRING_FACTOR_P10,  /*!<10 */
  LTE_RRC_AC_BARRING_FACTOR_P15,  /*!< 15 */
  LTE_RRC_AC_BARRING_FACTOR_P20,  /*!< 20 */
  LTE_RRC_AC_BARRING_FACTOR_P25,  /*!< 25 */
  LTE_RRC_AC_BARRING_FACTOR_P30,  /*!< 30 */
  LTE_RRC_AC_BARRING_FACTOR_P40,  /*!< 40 */
  LTE_RRC_AC_BARRING_FACTOR_P50,  /*!< 50 */
  LTE_RRC_AC_BARRING_FACTOR_P60,  /*!< 60 */
  LTE_RRC_AC_BARRING_FACTOR_P70,  /*!< 70 */
  LTE_RRC_AC_BARRING_FACTOR_P75,  /*!< 75 */
  LTE_RRC_AC_BARRING_FACTOR_P80,  /*!< 80 */
  LTE_RRC_AC_BARRING_FACTOR_P85,  /*!< 85 */
  LTE_RRC_AC_BARRING_FACTOR_P90,  /*!< 90 */
  LTE_RRC_AC_BARRING_FACTOR_P95,  /*!< 95 */
  LTE_RRC_AC_BARRING_FACTOR_INVALID ,  /*!< Invalid */
} lte_rrc_ac_barring_factor_e;


/*! @brief Data structure for AC barring params
*/
typedef struct
{
  lte_rrc_ac_barring_factor_e ac_barring_factor;/*!< Access Barring factor*/
  uint16 barring_time; /*!< Access Barring time value in seconds :-
     valid values s4 ,    s8 ,    s16 ,    s32 ,    s64 ,    s128 ,    s256 ,    s512*/
  /*boolean which say ac_barring_for_special_AC is present */
  boolean ac_barring_for_special_AC_present;
  uint8 ac_barring_for_special_AC; /*!< Access barring factor for special AC*/
} lte_rrc_ac_barring_params_s;


/*! @brief Data structure for AC barring params
*/
typedef struct
{
  /*!< Indicates ac_barring_for_emergency present*/
  boolean ac_barring_for_emergency_present;
  boolean ac_barring_for_emergency;/*!< Access Barring for emergency*/

  /*!< Indicates Access Barring info for MO signalling present*/
  boolean ac_barring_MO_signalling_present; 
  lte_rrc_ac_barring_params_s ac_barring_MO_signalling; /*!< Access Barring time for MO signalling*/

  /*!< Indicates Access Barring info for MO Data present*/
  boolean ac_barring_MO_data_present; 
  lte_rrc_ac_barring_params_s ac_barring_MO_data; /*!< Access Barring time for MO data*/

  /*Rel 12 Access barring Per PLMN list Info present in SIB-2*/
  boolean ac_Barring_r12_present;

  /*Skip Acess barring check for MMTELVoice*/
  boolean ac_BarringSkipForMMTELVoice_r12;

  /*Skip Acess barring check for MMTELVideo*/
  boolean ac_BarringSkipForMMTELVideo_r12;

  /*Skip Acess barring check for SMS*/
  boolean ac_BarringSkipForSMS_r12;

} lte_rrc_ac_barring_info_s;


/*! @brief Data structure for Service info
*/
typedef struct
{
  lte_rrc_plmn_s selected_plmn; /*!< Selected PLMN - req PLMN or its equivalent
                                     or first entry in SIB1 list for limited
                                     service */
  sys_sband_lte_e_type selected_band; /*!< Selected band */
  lte_rrc_global_cell_id_t cell_identity; /*!< Global Cell Id */
  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id */
  lte_earfcn_t earfcn;  /*!< DL Frequency */
  lte_bandwidth_e bandwidth;/*! System Bandwidth */
  uint16 tac;
  boolean ims_emergency_support_enabled; /*!< Support for IMS emergency calls*/
  lte_rrc_cell_access_status_e cell_access_status; /*!< access possible on camped cell */
  sys_csg_info_s_type csg_info; /*!< CSG Info of the camped cell */
  boolean limited_mode_only;/*  flag to indicate if the cell reported is suitable for registration */
} lte_rrc_camped_svc_info_s;

/*! @gummei-type to indicate whether the GUMMEI included is native (assigned by EPC) or 
    mapped (from 2G/3G identifiers).
*/
typedef enum
{
  LTE_RRC_GUMMEI_TYPE_NATIVE,  /*!< 0 */
  LTE_RRC_GUMMEI_TYPE_MAPPED,  /*!< 1 */
} lte_rrc_gummei_type_s;

/*! @brief Data structure for RegisteredMME information element
*/
typedef struct
{
  lte_rrc_plmn_s plmn_identity; /*!<PLMN Identity*/
  boolean plmn_identity_is_present; /*!< Is the optional 
                                      PLMN Identity present ?*/
  uint16 mmegi; /*!< Group Identity of the registered MME within the PLMN*/
  uint8 mmec; /*!< MME identity within the MME group.*/
  lte_rrc_gummei_type_s gummei_type; /*!< Globally unique MME identity*/
} lte_rrc_registered_mme_s;

/*! @brief Data structure for eMBMS TMGI info (PLMN ID and Service ID)
 */
typedef struct
{
  sys_plmn_id_s_type plmn_id; /*!< PLMN ID */
  byte service_id[3]; /*!< Service ID */

} lte_rrc_embms_tmgi_info_s;

/*! @brief Data structure for eMBMS session info (TMGI and session ID)
 */
typedef struct
{
  lte_rrc_embms_tmgi_info_s tmgi; /*!< TMGI */
  boolean session_id_present; /*!< Flag of session ID presenting or not */
  uint8 session_id; /*!< Session ID */

} lte_rrc_embms_session_info_s;

/*! @brief Data structure for eMBMS bearer info (session info and MRB id). 
  This is only used for lte_rrc_embms_act_tmgi_cnf_s, lte_rrc_embms_deact_tmgi_cnf_s
  and lte_rrc_embms_deact_tmgi_ind_s.
 */
typedef struct
{
  lte_rrc_embms_session_info_s session_info; /*!< Includes TMGI and session IDs */
  lte_rb_id_t mrb_id; /*!< MRB ID */

} lte_rrc_embms_bearer_info_s;

/*! @brief Request cphy sends to query if any ongoing RRC procedure is in progress
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */
} lte_rrc_procedure_status_req_s;

/*! @brief Enumeration for FGI change req
 */
typedef enum
{
  LTE_RRC_FGI_REL_8, /*!< 0*/
  LTE_RRC_FGI_REL_9, /*!< 1*/
  LTE_RRC_FGI_REL_10 /*!< 2*/
} lte_fgi_rel_type_e;

/*! @brief Enumeration for division duplexing type
*/
typedef enum
{
  /*!< None */
  LTE_RRC_NONE_DIV_DUPLEX, /*!< 0 */

  /*!< FDD - Frequency Division Duplex */
  LTE_RRC_FREQ_DIV_DUPLEX, /*!< 1 */

  /*!< TDD - Time Division Duplex */
  LTE_RRC_TIME_DIV_DUPLEX /*!< 2 */

} lte_rrc_div_duplex_e;

/*! @brief Data Structure for FGI change in CAP module
 */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;

  lte_fgi_rel_type_e fgi_rel_type; /*!<Indicates which FGI we need to change Rel-8/9/10*/

  uint32 fgi_word; /*!< 32 bit LTE Rel-8/9/10 FGI word, , MSB(Bit -31) is FGI-1 
  or 33 or 101 &  LSB(Bit -0) is FGI-32 or 64 or 132 */
  lte_rrc_div_duplex_e eutra_mode;  /*!Indicates whether to update FDD FGI or TDD FGI*/
} lte_rrc_cap_fgi_change_req_s;

/*! @brief Data Structure for FGI change in CAP module
 */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;

  /*! 
    Bit position
(starting from right most)	EFS file intended to be reread by LTE RRC
    1	                         /nv/item_files/modem/lte/common/lte_category
    2	                         /nv/item_files/modem/lte/common/ca_disable 
    3	                         /nv/item_files/modem/lte/rrc/cap/fgi
    4	                         /nv/item_files/modem/lte/rrc/cap/fgi_tdd
    5	                         /nv/item_files/modem/lte/rrc/cap/fgi_rel9
    6	                         /nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel9
    7	                         /nv/item_files/modem/lte/rrc/cap/fgi_r10
    8	                         /nv/item_files/modem/lte/rrc/cap/fgi_tdd_rel10
    */
  uint64 efs_bitmask;

} lte_rrc_cap_efs_reread_req_s;

/*! @brief Data Structure for eMBMS coverage status req
 */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;

} lte_rrc_cmapi_embms_cov_status_req_s;

/*! @brief Data Structure for eMBMS DATAMCS req
 */
typedef struct
{
  msgr_hdr_struct_type msg_hdr;
} lte_rrc_cmapi_embms_datamcs_req_s;

/*! @brief Message for Band priority change request
 */
typedef struct
{
  msgr_hdr_s msg_hdr;
  uint32 num_prioritized_bands; /*! Num prioritized bands */
  sys_sband_lte_e_type band_priority_list[SYS_SBAND_LTE_EUTRAN_BAND_MAX]; /*! Prioritized band list */
} lte_rrc_band_pri_change_req_s;

/*! @brief Message for Get Band priority list request
 */
typedef struct
{
  msgr_hdr_s msg_hdr;
  sys_lte_band_mask_e_type lte_band_cap;
} lte_rrc_get_band_pri_list_req_s;

/*! @brief Message for Set Pci Lock request
 */
typedef struct
{
  msgr_hdr_s msg_hdr;
  /*! 0..65535 (allowing for expansion) */
  lte_earfcn_t  dl_earfcn;
  /*! PCI */
  lte_phy_cell_id_t pci;
} lte_rrc_csp_set_pci_lock_req_s;

/*! @brief Message for Set Earfcn Lock request
 */
typedef struct
{
  msgr_hdr_s msg_hdr;
  /*! 0..65535 (allowing for expansion) */
  lte_earfcn_t  dl_earfcn1;
  /*! 0..65535 (allowing for expansion) */
  lte_earfcn_t  dl_earfcn2;
} lte_rrc_csp_set_earfcn_lock_req_s;



/*! @brief Enumeration of Source Tech
*/
typedef enum
{
  LTE_TRM_PRIORITY_REQ_FROM_NONE, /*!< 0 */
  LTE_TRM_PRIORITY_REQ_FROM_NAS, /*!< 1 */
  LTE_TRM_PRIORITY_REQ_FROM_DS, /*!< 2 */
  LTE_TRM_PRIORITY_REQ_FROM_CM /*!< 3 */
} lte_trm_client_id_e;


/*! @brief Message to indicate trm priority in DSDS operation.
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_trm_priority_e trm_priority; /*Indicates TRM priority(HIGH/LOW) for RRC connection establishment*/
  lte_trm_client_id_e client_id;
} lte_dsds_trm_priority_req_s;

/*! @brief Message to if UE is operating in Single Standby or dual standby.
dual_standby_mode-> Tells current standby pref: 
  SINGLE / DUAL / TRIPLE, typically based on number of active subscription available.
device_mode-> EFS setting of device via. 
  SINGLE SIM / DUAL STANDBY / DUAL ACTIVE / TRIPLE
active_subs-> which subs are active for given standby preference.

*/
typedef struct 
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  boolean    dual_standby_mode;
  sys_modem_dual_standby_pref_e_type  standby_mode;
  sys_modem_device_mode_e_type   device_mode;
}lte_rrc_ds_status_change_req_s;

/*! @brif Message indicating irat measurement capability*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /* This will be masked per rat, enum sys_radio_access_tech_e_type values can be used to index rats directly */
  sys_as_irat_status_e_type  irat_status[SYS_RAT_MAX]; 

}lte_rrc_irat_capability_req_s;

/*! @brief External requests that RRC receives
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CONN_EST,      0x00, 
                                                      lte_rrc_conn_est_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, UL_DATA,       0x01, lte_rrc_ul_data_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, DEACTIVATE,    0x02, 
                                                    lte_rrc_deactivate_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CONN_ABORT,    0x03, 
                                                    lte_rrc_conn_abort_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, SERVICE,       0x04, lte_rrc_service_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, SYSTEM_UPDATE, 0x05, 
                                                 lte_rrc_system_update_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, SIM_UPDATE,    0x06, 
                                                    lte_rrc_sim_update_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, DRX_INFO,      0x07, lte_rrc_drx_info_req_s),

  MSGR_DEFINE_UMID(LTE, RRC, REQ, PLMN_SEARCH,   0x08,
                                                    lte_rrc_plmn_search_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, PLMN_SEARCH_ABORT, 0x09,
                                              lte_rrc_plmn_search_abort_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, NW_SEL_MODE_RESET, 0x0A,
                                             lte_rrc_nw_sel_mode_reset_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, NMR_INFO,      0x0B, lte_rrc_nmr_info_req_s),

  MSGR_DEFINE_UMID(LTE, RRC, REQ, IRAT_TUNNEL_UL_MSG,   0x0C, lte_rrc_irat_tunnel_ul_msg_req_s),
  /* TO DO: Remove after WMS removes reference to the old UMID */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, ETWS_MSG_ID_LIST,   0x0D, lte_rrc_etws_msg_id_list_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, MSG_ID_LIST,   0x0E, lte_rrc_msg_id_list_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, IM3_BACKOFF_APPLIED,   0x0F, lte_rrc_im3_backoff_applied_req_s),
  /* NAS to LTE Tunnel HDR UL Msg Request */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, IRAT_HDR_UL_TUNNEL_MSG, 0x10, lte_irat_cdma_tunnel_ul_msg_req_s),  //LTE_RRC_IRAT_HDR_UL_TUNNEL_MSG_REQ

  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_ACT_TMGI, 0x11, lte_rrc_embms_act_tmgi_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_DEACT_TMGI, 0x12, lte_rrc_embms_deact_tmgi_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_AVAIL_TMGI_LIST, 0x13, lte_rrc_embms_avail_tmgi_list_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_ACT_TMGI_LIST, 0x14, lte_rrc_embms_act_tmgi_list_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_ENABLE, 0x15, lte_rrc_embms_enable_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_DISABLE, 0x16, lte_rrc_embms_disable_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_COVERAGE_STATE, 0x17, lte_rrc_embms_coverage_state_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_ACT_DEACT_TMGI, 0x18, lte_rrc_embms_act_deact_tmgi_req_s),

  /* CPHY to resquest for rrc procedure status */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, PROCEDURE_STATUS,0x19, lte_rrc_procedure_status_req_s),

  /* CPHY to resquest for rrc procedure status */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CELL_BAR,0x1A, lte_rrc_cell_bar_req_s),

  /* Reset forbidden TA list */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, FORBIDDEN_TA_LIST_RESET, 0x1B, lte_rrc_forbidden_ta_list_reset_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CAP_FGI_CHANGE, 0x1C, lte_rrc_cap_fgi_change_req_s), 
  
  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_SIGNAL_STRENGTH, 0x1D, lte_rrc_embms_signal_strength_report_req_s),

  MSGR_DEFINE_UMID(LTE, RRC, REQ, BAND_PRI_CHANGE, 0x1E, lte_rrc_band_pri_change_req_s), 

  MSGR_DEFINE_UMID(LTE, RRC, REQ, EMBMS_AVAIL_SAI_LIST, 0x1F, lte_rrc_embms_avail_sai_list_req_s), 
  MSGR_DEFINE_UMID(LTE, RRC, REQ, GET_BAND_PRI_LIST, 0x20, lte_rrc_get_band_pri_list_req_s),
  /* UMID to req TRM priority during DSDS
  */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, DSDS_TRM_PRIORITY, 0x21, lte_dsds_trm_priority_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CSP_SET_PCI_LOCK, 0x22, lte_rrc_csp_set_pci_lock_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CSP_SET_EARFCN_LOCK, 0x23, lte_rrc_csp_set_earfcn_lock_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, DS_STATUS_CHANGE, 0x24, 
                                                 lte_rrc_ds_status_change_req_s),
  /*LTE_RRC_REQ_MAX, */ /* defined in lte_rrc_irat_msg.h */
  MSGR_DEFINE_UMID(LTE, RRC, REQ, AVOIDANCE,       0x25, lte_rrc_avoidance_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CMAPI_EMBMS_COV_STATUS, 0x26, lte_rrc_cmapi_embms_cov_status_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CMAPI_EMBMS_DATAMCS, 0x27, lte_rrc_cmapi_embms_datamcs_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, GET_SERV_CELL_SIB, 0x28, lte_rrc_get_serv_cell_sib_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, COMMON_MSIM_CRAT_FEATURE, 0x29, lte_common_msim_crat_feature_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, MTC_CFG, 0x2A, lte_rrc_mtc_cfg_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, IRAT_CAPABILITY, 0x2B, lte_rrc_irat_capability_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CAP_EFS_REREAD, 0x2C, lte_rrc_cap_efs_reread_req_s),
  MSGR_DEFINE_UMID(LTE, RRC, REQ, CELL_LOCK, 0x2D, lte_rrc_cell_lock_req_s),
};

/*! @brief External requests that RRC receives which has AC barring 
    info for special AC's i.e 10..15
*/
typedef struct
{
  /*Access Barring probability which RRC need to apply for AC-11..15 when SIB2 says 
  no AC barring for AC-11..15*/
  uint8 access_barring_prob[LTE_RRC_AC_BARRING_PROB_TABLE_SIZE];

  /*Access Barring Timer value in msec*/
  uint16 access_barring_time;

} lte_rrc_ac_barring_info_for_special_s;


/*! @brief Message for Connection Establishment Confirm 
  In case RRC is sending est_cnf with failure due to TAC\PLMN change during est_req,
  RRC needs to ensure that est_cnf with failure is sent to NAS before service ind.
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  msgr_attach_s dsm_attach; /*!< dsm attachment
                                 containing the NAS Message Container */
  uint32 transaction_id; /*!< Transaction ID for the ConnEstReq..
                              Used to track the NAS message contained in
                              ConnSetupComplete*/
  lte_rrc_est_cause_e est_cause; /*!< Establishment cause */
  boolean is_high_priority; /*Is the call a high priority call ?*/
  lte_rrc_registered_mme_s registered_mme; /*!< RegisteredMME Information
                                                Element*/
  boolean registered_mme_is_present; /*Is RegisteredMME IE present ?*/

  boolean include_s_tmsi; /*Should S-TMSi be included in ConnRequest ?*/

  boolean ac_barring_info_for_special_valid; /*Only when ac_barring_info_for_special_valid  = TRUE, RRC to look at ac_barring_info_for_special */
  lte_rrc_ac_barring_info_for_special_s ac_barring_info_for_special; /*AC barring info for special AC's i.e. 10 to 15*/

  lte_trm_priority_e trm_priority; /*Indicates TRM priority(HIGH/LOW) for RRC connection establishment*/

  sys_lte_reestab_calltype_e est_calltype; /*!< Establishment call type VoLte or normal call*/

  boolean barring_restr_for_special_ac_enabled; /*!When set to TRUE, if lower AC is not barred in SIB2, then DO NOT bar special AC too*/

  boolean mo_data_enabled_dt; /*!Indicates whether MO data is enabled or not for Conn est type = DT
                                 If set to FALSE and if Conn est type = DT, then DT is getting enabled for MO signalling*/
  boolean eab_cat_c; /* Boolean to indicate whether EAB CAT C check needs to be performed by RRC or not
                                     Applicable only for EAB call types*/
} lte_rrc_conn_est_req_s;

/*! @brief Message for UL Data Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  msgr_attach_s dsm_attach; /*!< dsm attachment
                                 containing the NAS Message Container */

  uint32 trans_id; /*!< Transaction ID */
  lte_rrc_ul_nas_msg_priority_e priority; /*!< Priority of the message */
  boolean cnf_is_reqd; /*!< Is confirmation required? */
  boolean cnf_is_reqd_during_conn_rel; /*!< Is confirmation required 
                                 during RRC connection release */
} lte_rrc_ul_data_req_s;

/*! @brief Message for IRAT UL Message Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint32 trans_id; /*!< Transaction ID */
  boolean cnf_is_reqd; /*!< Is confirmation required? */

  /*! Union of IRAT Tunnel UL Message types */
  lte_irat_tunnel_ul_msg_req_s msg;

} lte_rrc_irat_tunnel_ul_msg_req_s;

/*! @brief Message for Deactivate Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  sys_stop_mode_reason_e_type deactivate_reason; /* Deactivate reason */
} lte_rrc_deactivate_req_s;

/*! @brief Message for Service Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint32 trans_id; /*!< Transaction ID */

  /* network select mode */
  sys_network_selection_mode_e_type network_select_mode;

  boolean req_plmn_info_is_valid; /*!< Is requested PLMN valid */

  /* valid for svc_req_type of LTE_RRC_SVC_REQ_NORMAL only */
  lte_rrc_plmn_s req_plmn; /*!< Requested PLMN */

  boolean rplmn_info_is_valid; /*!< Is registered PLMN info available?  */

  /* valid only if rplmn_info_is_valid is TRUE */
  lte_rrc_plmn_s rplmn; /*!< Registered PLMN */

  boolean hplmn_info_is_valid; /*!< Is HPLMN info available?  */

  /* valid only if hplmn_info_is_valid is TRUE */
  lte_rrc_plmn_s hplmn; /*!< HPLMN */

  /* If no list, set num_plmns to 0 */
  lte_rrc_ehplmn_list_s ehplmn_list; /*!< EHPLMN list */

  /* if no forbidden TAs, num_plmns = 0 */
  lte_rrc_tracking_area_list_s forbidden_ta_list; /*!< Forbidden TA list */

  /* if no manual forbidden TAs, num_plmns = 0 */
  lte_rrc_tracking_area_list_s forbidden_manual_ta_list; /*!< Forbidden TA list in Manual mode */

  sys_rat_pri_list_info_s_type rat_pri_list; /*!< RAT priority list */

  /* the list of PLMNS equivalent to RPLMN, valid only if rplmn_info_is_valid 
     is TRUE; If it is TRUE, list must at least be initialized with 0 entries
     if it is empty */
  lte_rrc_plmn_list_s eplmn_list; /*!< EPLMN list */

  boolean scan_is_new; /*!< Is service request new or not */

  boolean use_timer; /*!< True = use timer (UOOS), False = don't use timer 
                          (Non-UOOS) */

  /* valid only if use_timer is TRUE */
  uint32 lte_scan_time; /*!< UOOS search timer for LTE in ms */

  /* Service req type to indicate if it due to BSR */
  lte_rrc_service_req_type_e req_type;

  sys_csg_id_type csg_id; /*!< CSG Id */

  /* Boolean to indicate if a E911 call is pending */
  boolean  emc_srv_pending;

  /*! scan type*/
  sys_scan_scope_e_type scan_scope;

  /*! timeout duration*/
  uint32 t_timeout;  

  /*!Boolean to give priority to EHPLMN/HPLMN to RPLMN*/
  boolean ehplmn_camping_allowed;

  /* service search type TBD */
} lte_rrc_service_req_s;

/*! @brief Enumeration for connection abort cause
*/
typedef enum
{
  /*!< LTE RRC Connection being aborted for a 
    generic reason by NAS.  If IRAT is in progress, it will be aborted because
    NAS wants to do something on LTE */
  LTE_RRC_CONN_ABORT_CAUSE_NORMAL, /*!< 0 */

  /*!< LTE RRC Connection being aborted because of a successful IRAT 
       and LTE RRC needs to be deactivated.  NAS is first trying to abort the
       existing RRC connection before it can deactivate LTE RRC */
  LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS, /*!< 1 */

  /*!< LTE RRC Connection being aborted because of a successful IRAT -
           L2G SRVCC in an SGLTE mode,
       LTE RRC shall not abort IRAT just needs to be resumed. */
  LTE_RRC_CONN_ABORT_CAUSE_IRAT_SUCCESS_SGLTE_SRVCC_TO_G, /*!< 2 */

  LTE_RRC_CONN_ABORT_CAUSE_MAX
} lte_rrc_conn_abort_cause_e;

/*! @brief Message for Connection Abort Request. Must be sent before deactivate
    if UE is connected, and NAS needs to wait for Connection Released indication
    before sending deactivate request.
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_conn_abort_cause_e cause;
  boolean cell_barring_is_reqd; /*!< Is cell barring required */
  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id */
  lte_earfcn_t earfcn;  /*!< DL Frequency */
} lte_rrc_conn_abort_req_s;

/*! @brief Message for Cell bar Request. L-NAS may request cell to be barred for AUTH
      related failures.
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id */
  lte_earfcn_t earfcn;  /*!< DL Frequency */
} lte_rrc_cell_bar_req_s;


/*! @brief Message for System Update Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean rplmn_info_is_valid; /*!< Is registered PLMN info available?  */

  /* valid only if rplmn_info_is_valid is TRUE */
  lte_rrc_plmn_s rplmn; /*!< Registered PLMN */

  boolean hplmn_info_is_valid; /*!< Is HPLMN info available?  */

  /* valid only if hplmn_info_is_valid is TRUE */
  lte_rrc_plmn_s hplmn; /*!< HPLMN */

  /* If no list, set num_plmns to 0 */
  lte_rrc_ehplmn_list_s ehplmn_list; /*!< EHPLMN list */

  /* if no forbidden TAs, num_plmns = 0 */
  lte_rrc_tracking_area_list_s forbidden_ta_list; /*!< Forbidden TA list */

  /* if no manual forbidden TAs, num_plmns = 0 */
  lte_rrc_tracking_area_list_s forbidden_manual_ta_list; /*!< Forbidden TA list in Manual mode */

  /* the list of PLMNS equivalent to RPLMN, valid only if rplmn_info_is_valid 
     is TRUE. If it is TRUE, list must at least be initialized with 0 entries
     if it is empty */
  lte_rrc_plmn_list_s eplmn_list; /*!< EPLMN list */

  lte_rrc_csg_list_type_e csg_whitelist_type; /*!< CSG Whitelist Type */

  lte_rrc_csg_list_s csg_whitelist; /*!< CSG whitelist */

} lte_rrc_system_update_req_s;

/*! @brief Message for Sim Update Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean security_info_is_valid; /*!< Is security info valid */
  lte_rrc_security_info_s security_info; /*!< Security info */
  boolean ue_id_list_is_valid; /*!< Is UE ID list valid */
  lte_rrc_ue_id_list_s ue_id_list; /*!< UE identity list */
  /* interpret as ((byte1 << 8) | byte2) in SIM spec */
  uint16 access_class; /*!< Access classes that the UE belongs to*/
  boolean eab_enabled; /*!< Boolean to indicate whether EAB is enabled or not*/
} lte_rrc_sim_update_req_s;

/*! @brief Message for Network Select Mode Reset Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*! network select mode */
  sys_network_selection_mode_e_type network_select_mode;

  /*! boolean to indicate if RAT priority list has to be updated */
  boolean                                   update_rat_pri_list;

  /*! updated RAT priority list */
  sys_rat_pri_list_info_s_type   rat_pri_list;
} lte_rrc_nw_sel_mode_reset_req_s;

/*! @brief Message for Forbidden TA List Reset Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_forbidden_ta_list_reset_req_s;

/*! @brief Enumeration of allowed paging cycles
*/
typedef enum
{
  LTE_RRC_DRX_CN_COEFFICIENT_S1_T32 = 6,    /*!< T = 32 */
  LTE_RRC_DRX_CN_COEFFICIENT_S1_T64 = 7,    /*!< T = 64 */
  LTE_RRC_DRX_CN_COEFFICIENT_S1_T128 = 8,   /*!< T = 128 */
  LTE_RRC_DRX_CN_COEFFICIENT_S1_T256 = 9,    /*!< T = 256 */
  LTE_RRC_DRX_CN_NOT_SPECIFIED /*!< 0xFF Always the last one */

} lte_rrc_paging_cycle_e;


/*! @brief Message for DRX Info Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_paging_cycle_e paging_cycle;

} lte_rrc_drx_info_req_s;


/*! @brief External confirmations that RRC sends
*/

/*! @brief Message for PLMN Search Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint32 trans_id; /*!< Transaction ID */

  /*! network select mode */
  sys_network_selection_mode_e_type network_select_mode;  

  sys_service_search_e_type search_type; /*!< service search */

  sys_rat_pri_list_info_s_type rat_pri_list; /*!< RAT priority list */

  sys_plmn_rat_list_s_type plmn_rat_list; /*!< PLMN RAT List*/

  boolean rplmn_info_is_valid; /*!< Is registered PLMN info available? */

  /* valid only if rplmn_info_is_valid is TRUE */
  lte_rrc_plmn_s rplmn; /*!< Registered PLMN */

  boolean hplmn_info_is_valid; /*!< Is HPLMN info available?  */

  /* valid only if hplmn_info_is_valid is TRUE */
  lte_rrc_plmn_s hplmn; /*!< HPLMN */

  /* if no ehplmn list, ehplmn_list.num_plmns = 0 */
  lte_rrc_ehplmn_list_s ehplmn_list; /*!< EHPLMN list */

  /* if no eplmn list, eplmn_list.num_plmns = 0 */
  lte_rrc_plmn_list_s eplmn_list; /*!< EPLMN list  */

  /* if no forbidden TAs, forbidden_ta_list.num_entries = 0 */
  lte_rrc_tracking_area_list_s forbidden_ta_list; /*!< Forbidden TA list */

  sys_scan_scope_e_type   scan_scope;

  /* Partial PLMN search timer values */
  sys_manual_search_timer_value_type   search_timer_value;         

  /* Following rat pri list to be used as a bst filter 
  ** in bplmn search by lower layers*/
  sys_rat_pri_list_info_s_type    mcc_mode_band_filter;

} lte_rrc_plmn_search_req_s;

/*! @brief Message for PLMN Search Abort Request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_plmn_search_abort_req_s;

/*! @brief Enumeration for NMR report types
*/
typedef enum
{

  /* Initial value - should be used just for initialization  */
  LTE_RRC_NMR_REPORT_NONE = 0x00,
  /* Used for requesting report info containing LTE intra frequency neighbors */
  LTE_RRC_NMR_REPORT_INTRA_FREQ = 0x01,
  /* Used for requesting report info containing LTE inter frequency neighbors */
  LTE_RRC_NMR_REPORT_INTER_FREQ = 0x02,
  /* Used for requesting report info containing WCDMA neighbors */
  LTE_RRC_NMR_REPORT_IRAT_WCDMA = 0x03,
  /* Used for requesting report info containing GERAN neighbors */
  LTE_RRC_NMR_REPORT_IRAT_GERAN = 0x04,
  /* Used for requesting report info containing C2K neighbors */
  LTE_RRC_NMR_REPORT_IRAT_CDMA2K = 0x05            

}lte_rrc_nmr_report_type_e;


/*! @brief Message for PLMN Search Abort Request
 */
typedef struct
{

  msgr_hdr_s msg_hdr; /*!< Message router header */
  lte_rrc_nmr_report_type_e report_type; /*!< Report type being requested for */

}lte_rrc_nmr_info_req_s;

/*! @brief Message for Serving Cell Get SIB Req
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;
  
  /*! Indicate the SIB number requested */
  uint8 sib_num;

} lte_rrc_get_serv_cell_sib_req_s;

/*! @brief Structure representing a message id range. Its a single message id 
  if min_msg_id = max_msg_id
 */
typedef struct
{
  lte_rrc_msg_id_t min_msg_id;
  lte_rrc_msg_id_t max_msg_id;
} lte_rrc_msg_id_range_s;

/*! @brief Message for RRC ETWS Message id List request
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /* Msg id list length */
  uint8 msg_id_range_list_size;

  /* List of requested msg_id_ranges */
  lte_rrc_msg_id_range_s msg_id_range_list[LTE_RRC_MAX_MSG_ID_LIST_SIZE];

} lte_rrc_msg_id_list_req_s;

/* TO DO: Remove after WMS removes reference to the old structure */
typedef lte_rrc_msg_id_list_req_s lte_rrc_etws_msg_id_list_req_s;

typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  boolean on_off;/*!< Set to TRUE if backoff applied*/
}lte_rrc_im3_backoff_applied_req_s;

/* eMBMS related commands */
/*! @brief Message for eMBMS TMGI activate request sent from upper layer 
  to RRC for activating an eMBMS service.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

  uint8 preempt_priority; /*!< Preemption priority of the TMGI to be activated, range 0 - 5(highest),
                               If value > 5, modem will treat it as 5 (highest priority) */

  uint8 num_sai_to_search; /*!< Number of SAIs in the list below */

  uint16 sai_list[LTE_RRC_EMBMS_MAX_NUM_SAI_TO_SRCH]; 
                          /*! List of SAIs where TMGI can be found */

  uint8 num_earfcn_to_search; /*!< Number of EARFCNs in the list below */

  lte_earfcn_t earfcn_list[LTE_RRC_EMBMS_MAX_NUM_FREQ_TO_SRCH]; 
                          /*! List of EARFCNs where TMGI can be found */

  lte_rrc_embms_session_info_s session_info; /*!< Includes TMGI and session IDs */

} lte_rrc_embms_act_tmgi_req_s;

/*! @brief Message for eMBMS TMGI deactivate request sent from upper layer 
  to RRC for deactivating an eMBMS service.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

  lte_rrc_embms_session_info_s session_info; /*!< Includes TMGI and session IDs */

} lte_rrc_embms_deact_tmgi_req_s;


/* eMBMS related commands */
/*! @brief Message for eMBMS TMGI activate request sent from upper layer 
  to RRC for activating an eMBMS service.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

  uint8 preempt_priority; /*!< Preemption priority of the TMGI to be activated, range 0 - 5(highest),
                               If value > 5, modem will treat it as 5 (highest priority) */

  uint8 num_sai_to_search; /*!< Number of SAIs in the list below */

  uint16 act_sai_list[LTE_RRC_EMBMS_MAX_NUM_SAI_TO_SRCH]; 
                          /*! List of SAIs where TMGI can be found */

  uint8 num_earfcn_to_search; /*!< Number of EARFCNs in the list below */

  lte_earfcn_t earfcn_list[LTE_RRC_EMBMS_MAX_NUM_FREQ_TO_SRCH]; 
                          /*! List of EARFCNs where Activation TMGI can be found */

  lte_rrc_embms_session_info_s act_session_info; /*!< Includes TMGI and session ID that needs to
                                                      be activated */

  lte_rrc_embms_session_info_s deact_session_info; /*!< Includes TMGI and session ID that needs to
                                                      be deactivated */

} lte_rrc_embms_act_deact_tmgi_req_s;

/*! @brief Message for eMBMS available TMGI list sent from upper layer 
  to RRC for acquiring all available eMBMS services across all MCCHs.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

} lte_rrc_embms_avail_tmgi_list_req_s;

/*! @brief Message for eMBMS available SAI list sent from upper layer 
  to RRC for determine all available SAIs in SIB15 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

  boolean mdm_initiated; /*!< Boolean to indicate if Modem is 
                              initiating this req on its own */

} lte_rrc_embms_avail_sai_list_req_s;

/*! @brief Message for eMBMS active TMGI list sent from upper layer 
  to RRC for acquiring all active eMBMS services.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

} lte_rrc_embms_act_tmgi_list_req_s;

/*! @brief Message for eMBMS service enable sent from upper layer 
  to RRC to enable eMBMS service.
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                              -1 when not used  */

} lte_rrc_embms_enable_req_s;

/*! @brief Message for eMBMS service disable sent from upper layer 
  to RRC to disable eMBMS service.
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                               -1 when not used  */

} lte_rrc_embms_disable_req_s;

typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /* the following is the staus code */
  /*
      FALSE: Didn't update FGI
      TRUE: updated FGI 
   */
  boolean                             status;
} lte_rrc_cap_fgi_change_cnf_s;

typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /* the following is the staus code */
  /*
      FALSE: EFS reread NOT DONE successfully
      TRUE: All requested EFS read successfully
   */
  boolean                             status;
} lte_rrc_cap_efs_reread_cnf_s;

/*! Structure for PMCH data-MCS for all configured PMCHs in an MBSFN area */
typedef struct
{
  /*! MBSFN area id (0..255) from SIB13 */
  uint8 mbsfn_area_id;

  /*! PMCH id (1..15), First PMCH config is treated as PMCH id 1 */
  uint8 pmch_id;

  /*! Data MCS for the PMCH */
  uint8 datamcs;

} lte_rrc_pmch_datamcs_info_s;

/*! Structure for PMCH data-MCS info for all PMCHs actively being monitored */
typedef struct
{
  /*! Number of PMCHs for which dataMCS info has been included */
  uint8 num_pmch_datamcs_info;

  /*! PMCH data-MCS of all PMCHs actively being monitored */
  lte_rrc_pmch_datamcs_info_s pmch_datamcs_info[LTE_MAX_NUM_PMCH_DATAMCS_INFO];
  
} lte_rrc_datamcs_info_s;

/*! 
  @brief 
  This message indicates to upper layers the current signal quality at L1 for each MBSFN Area.
*/

typedef struct
{  
  /*! Number of MBSFN areas whose SNR values are populated in the array below */
  uint8                                    num_mbsfn_areas;
  
 /*! SNR info per MBSFN Area */
  lte_cphy_mbsfn_area_signal_strength_s    mbsfn_area_signal_strength[LTE_EMBMS_MAX_MBSFN_AREA];

} lte_rrc_mbsfn_snr_info_s;

typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /* the following is the cov_status code */
  /*
    0 - implies both SIB13 and SIB15 are not scheduled in SIB1 of current cell,
    1 - implies SIB13 and optionally SIB15 scheduled in SIB1 of current cell,
    2 - implies SIB15 alone scheduled in SIB1 of current cell. 
   */
  uint8                             cov_status;

} lte_rrc_cmapi_embms_cov_status_cnf_s;

typedef struct
{
  /*! Message router header */
  msgr_hdr_struct_type              msgr_hdr;

  /* MTCH data MCS for all PMCHs that are actively being monitored */
  lte_rrc_datamcs_info_s            datamcs_info;

} lte_rrc_cmapi_embms_datamcs_cnf_s;

/*! @brief Message for band priority change cnf
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_s              msgr_hdr;

  /* the following is the staus code TRUE for SUCCESS and FALSE for failure */
  boolean                             status;
} lte_rrc_band_pri_change_cnf_s;

/*! @brief Message for get band priority list cnf
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_s              msgr_hdr;
  uint32                  num_configured_bands; /*! Num configured bands */
  sys_sband_lte_e_type    configured_band_pri_list[SYS_SBAND_LTE_EUTRAN_BAND_MAX]; /*! configured band pri list */
  uint32                  num_supported_bands; /*! Num supported bands*/
  sys_sband_lte_e_type    supported_band_pri_list[SYS_SBAND_LTE_EUTRAN_BAND_MAX]; /*! supported band pri list*/
} lte_rrc_get_band_pri_list_cnf_s;

/*! @brief Message for set pci lock cnf
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_s              msgr_hdr;

  /* the following is the staus code TRUE for SUCCESS and FALSE for failure */
  boolean                             status;
} lte_rrc_csp_set_pci_lock_cnf_s;

/*! @brief Message for set earfcn lock cnf
*/
typedef struct
{
  /*! Message router header */
  msgr_hdr_s              msgr_hdr;

  /* the following is the staus code TRUE for SUCCESS and FALSE for failure */
  boolean                             status;
} lte_rrc_csp_set_earfcn_lock_cnf_s;

typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

}lte_rrc_irat_capability_cnf_s;


enum
{
  MSGR_DEFINE_UMID(LTE, RRC, CNF, CONN_EST,      0x00, lte_rrc_conn_est_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, UL_DATA,       0x01, lte_rrc_ul_data_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, DEACTIVATE,    0x02, lte_rrc_deactivate_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, PLMN_SEARCH,   0x03, lte_rrc_plmn_search_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, NMR_INFO,      0x04, lte_rrc_nmr_info_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, IRAT_TUNNEL_UL_MSG,   0x05, lte_rrc_irat_tunnel_ul_msg_cnf_s),
  /* HDR Tunnel UL Msg Cnf to NAS from LTE RRC */
  MSGR_DEFINE_UMID(LTE, RRC, CNF, IRAT_HDR_TUNNEL_UL_MSG, 0x06, lte_rrc_irat_tunnel_ul_msg_cnf_s),

  MSGR_DEFINE_UMID(LTE, RRC, CNF, EMBMS_ACT_TMGI, 0x07, lte_rrc_embms_act_tmgi_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, EMBMS_DEACT_TMGI, 0x08, lte_rrc_embms_deact_tmgi_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, EMBMS_ACT_DEACT_TMGI, 0x09, lte_rrc_embms_act_deact_tmgi_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, CAP_FGI_CHANGE, 0x0A, lte_rrc_cap_fgi_change_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, EMBMS_SIGNAL_STRENGTH, 0x0B, lte_rrc_embms_signal_strength_report_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, BAND_PRI_CHANGE, 0x0C, lte_rrc_band_pri_change_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, GET_BAND_PRI_LIST, 0x0D, lte_rrc_get_band_pri_list_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, CSP_SET_PCI_LOCK, 0x0E, lte_rrc_csp_set_pci_lock_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, CSP_SET_EARFCN_LOCK, 0x0F, lte_rrc_csp_set_earfcn_lock_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, CMAPI_EMBMS_COV_STATUS, 0x10, lte_rrc_cmapi_embms_cov_status_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, CMAPI_EMBMS_DATAMCS, 0x11, lte_rrc_cmapi_embms_datamcs_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, GET_SERV_CELL_SIB, 0x12, lte_rrc_get_serv_cell_sib_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, MTC_CFG, 0x13, lte_rrc_mtc_cfg_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, COMMON_MSIM_CRAT_FEATURE, 0x15, lte_common_msim_crat_feature_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, IRAT_CAPABILITY, 0x14, lte_rrc_irat_capability_cnf_s),
  MSGR_DEFINE_UMID(LTE, RRC, CNF, CAP_EFS_REREAD, 0x16, lte_rrc_cap_efs_reread_cnf_s),
};

/*! @brief Message for Connection Establishment Confirm
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_conn_est_status_e est_status; /*!< Status */
  boolean barring_info_is_present; /*!< Is the barring timer info present ?*/
  /* !< Bit mask is the mask indicating the calls for which the access barring 
        information is present below in the conn_est_cnf. 
        Bitmask = (0x1<<est_cause_1) | (0x1<<est_cause_2) | ...
        where est_cause_1, est_cause_2, etc. are the call types whose access barring
        information is indicated below. 
        i.e. for example,
        Bit mask is 0x1<<LTE_RRC_EST_CAUSE_EMERGENCY for Emergency calls.
        If the access barring information includes info for emergency and MT calls,
        the bit mask is (0x1 << LTE_RRC_EST_CAUSE_EMERGENCY) | (0x1< LTE_RRC_EST_CAUSE_MT_ACCESS)
  */
  uint8 call_type_bitmask;/*!< Bit mask indicating for which calls the barring 
                               info is valid */
  uint64 barring_timer_value; /*!< Value of the barring timer in ms.
                                LTE_RRC_INDEFINITE_BARRING_TIME (0xffffffff)
                                means barred indefinitely. 
                                Used for Emergency calls*/
  uint16 extended_wait_time;  /*!< Extended Wait time in seconds for Delay Tolerant Access requests;  Valid Range (1 to 1800); 0 indicates value not applicable */
} lte_rrc_conn_est_cnf_s;

/*! @brief Message for EAB update Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean eab_valid; /*!< Boolean to indicate whether EAB barring is valid or not
                                        Will be sent whenever EAB timer is running and NW removes SIB14
                                        i.e. EAB should be stopped by the UE*/
} lte_rrc_eab_update_ind_s;


#define LTE_RRC_MAX_TRANS_IDS 256

/*! @brief Message for UL Data Confirm
*/
typedef struct
{
  /*!< UL_DATA_CNF will not sent if controller is in CLOSING state. 
       NAS will cleanup pending UL_DATA_REQs on receiving CONN_REL_IND eventually.
  */
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint8 num_trans_ids; /*!< Number of transaction ids. Transaction ids are
                            bundled if status = LTE_RRC_UL_DATA_CNF_FAILURE_HO
                            or LTE_RRC_UL_DATA_CNF_FAILURE_RLF */
  uint32 trans_id[LTE_RRC_MAX_TRANS_IDS]; /*!< Transaction IDs */
  lte_rrc_ul_data_cnf_status_e status; /*!< Transmission status */
  lte_rrc_tracking_area_s ta; /*!< Tracking area code, valid only if 
                                    status = LTE_RRC_UL_DATA_CNF_FAILURE_HO */
} lte_rrc_ul_data_cnf_s;


/*! @brief Message for IRAT Tunnel UL Msg Confirm
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint8 num_trans_ids; /*!< Number of transaction ids. Transaction ids are
                            bundled if status = LTE_RRC_UL_DATA_CNF_FAILURE_HO
                            or LTE_RRC_UL_DATA_CNF_FAILURE_RLF */
  uint32 trans_id[LTE_RRC_MAX_TRANS_IDS]; /*!< Transaction IDs */
  lte_rrc_tunnel_ul_msg_cnf_status_e status; /*!< Transmission status */

  lte_rrc_tracking_area_s ta; /*!< Tracking area code, valid only if 
                                    status = LTE_RRC_UL_DATA_CNF_FAILURE_HO */
} lte_rrc_irat_tunnel_ul_msg_cnf_s;

/*! @brief Message for Deactivate Confirm
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  boolean status; /*!< True = success, False = failure */
} lte_rrc_deactivate_cnf_s;

/*! @brief Message for PLMN Search Confirmation
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint32 trans_id; /*!< Transaction ID */

  /*!< network select mode */
  sys_network_selection_mode_e_type network_select_mode;

  sys_service_search_e_type search_type; /*!< service search */

  lte_rrc_plmn_search_status_e status; /*!< Status of search */

  sys_found_plmn_list_u_type found_plmn_list; /*!< Union of RAT PLMN list and PCI PLMN list */

} lte_rrc_plmn_search_cnf_s;


typedef enum
{ 
  /*!< RRC successfully reported NMR info back to GSTK */
  LTE_RRC_NMR_INFO_SUCCESS,
  /*!< FAILURE: LTE stack is not the active stack */
  LTE_RRC_NMR_INFO_FAIL_LTE_INACTIVE,
  /*!< FAILURE: UE is on LTE but not camped on a cell */
  LTE_RRC_NMR_INFO_FAIL_UE_NOT_CAMPED,
  /*!< FAILURE: Any other failure */
  LTE_RRC_NMR_INFO_FAIL_OTHER
} lte_rrc_nmr_info_statue_e;

/*! @brief Message for NMR info Confirmation
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  /*!< dsm attachment containing the n freq meas report */
  /*!< Format represented below. Fields not applicable for
       certain report types can be ignored
       -------------------------------------------------------------------------------------------
       |num_report|  size   | freq    |  meas_report   | ... |  size   | freq    |  meas_report   |
       | 1 byte   | 2 bytes | 4 bytes | (size-2 bytes) |     | 2 bytes | 4 bytes | (size-2 bytes) |
       -------------------------------------------------------------------------------------------
  */
  msgr_attach_s dsm_attach;

  lte_rrc_nmr_info_statue_e status; /*< status of NMR info req */
  
  lte_rrc_nmr_report_type_e report_type; /*!< Report type being requested for */
  
} lte_rrc_nmr_info_cnf_s;

/*! @brief Enumeration for GET SIB CNF status
*/
typedef enum
{
  LTE_RRC_GET_SIB_SUCCESS = 0, /*SIB string successfully retrieved and stored in DSM*/
  LTE_RRC_GET_SIB_FAIL_NOT_CAMPED, /*When LTE is not camped*/
  LTE_RRC_GET_SIB_FAIL_NOT_AVAILABLE, /*When the requested SIB is not read/not scheduled*/
  LTE_RRC_GET_SIB_FAIL_NOT_SUPPORTED, /*When the requested SIB is not supported*/
  LTE_RRC_GET_SIB_FAIL_OTHER, /*For all other failures*/
  LTE_RRC_GET_SIB_FAIL_MAX = 255
} lte_rrc_get_serv_cell_sib_status_e;

/*! @brief Message for Get Serving Cell SIB Confirmation
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  /*!< Encoded Measurement Message */
  msgr_attach_s dsm_attach; /*!< dsm attachment
                                 sib string if status is success */
  
  /*! Indicate the status of Get SIB Req */
  lte_rrc_get_serv_cell_sib_status_e status;
    
} lte_rrc_get_serv_cell_sib_cnf_s; 

/*! @brief Enumeration for service status
*/
typedef enum
{
  LTE_EMBMS_SUCCESS, /*!< Success */
  LTE_EMBMS_SUCCESS_ALREADY_DONE,  /*!< Success, TMGI is already activated/deactivated */
  LTE_EMBMS_SUCCESS_IDLE_RADIO_TUNE, /*!< TMGI is requested on a different freq while UE is idle.
                                          Priority of interested freq bumped up */
  LTE_EMBMS_SUCCESS_CONN_RADIO_TUNE, /*!< TMGI is requested on a different freq while UE is connected. 
                                         eMBMS interest has been sent for the target freq */
  LTE_EMBMS_ALREADY_DONE,  /*!< Deprecated, replaced with LTE_EMBMS_SUCCESS_ALREADY_DONE */
  LTE_EMBMS_FAILURE_UNKNOWN,  /*!< Deprecated, replaced with LTE_EMBMS_FAIL_UNKNOWN */
  LTE_EMBMS_NOT_ALLOWED,  /*!< Deprecated, replaced with LTE_EMBMS_NOT_ALLOWED */
  LTE_EMBMS_MISSING_CTRL_INFO,  /*!< Deprecated, replaced with LTE_EMBMS_FAIL_MISSING_CTRL_INFO */
  LTE_EMBMS_TMGI_NOT_FOUND,  /*!< Deprecated, replaced with LTE_EMBMS_FAIL_TMGI_NOT_FOUND */
  LTE_EMBMS_MCAST_OOC,  /*!< Deprecated, replaced with LTE_EMBMS_FAIL_MCAST_OOC */
  LTE_EMBMS_UCAST_OOS,  /*!< Deprecated, replaced with LTE_EMBMS_FAIL_UCAST_OOS */
  LTE_EMBMS_CAMPED_ON_OTHER_FREQ,  /*!< Deprecated, replaced with LTE_EMBMS_FAIL_FREQ_CONFLICT */

  /* Add new Success codes here */

  LTE_EMBMS_FAIL_UNKNOWN = 100,  /*!< Failure cause is unknown */
  LTE_EMBMS_FAIL_NOT_ALLOWED,  /*!< Failure, eMBMS is not enabled */
  LTE_EMBMS_FAIL_MISSING_CTRL_INFO,  /*!< Failure, Atleast one MCCH message was not received */
  LTE_EMBMS_FAIL_TMGI_NOT_FOUND,  /*!< Failure, TMGI not found in MCCH */
  LTE_EMBMS_FAIL_MCAST_OOC,  /*!< Failure, sib13 not present in the cell */
  LTE_EMBMS_FAIL_UCAST_OOS,  /*!< Failure, currently under Unicast OOS */
  LTE_EMBMS_FAIL_FREQ_CONFLICT,  /*!< Failure, TMGI is requested on a different freq 
                                   while actively receiving a TMGI on current freq */
  LTE_EMBMS_FAIL_SAI_MISMATCH,  /*!< Failure, none of the SAIs in Act TMGI req are available */
  LTE_EMBMS_FAIL_MAX_TMGI_ALREADY_ACTIVE, /*!< Failure, max supported number of TMGIs are already active */

  /* Add new Failure codes here */
  
} lte_embms_status_e;

/* eMBMS related commands */
/*! @brief Message for eMBMS TMGI activate confirm sent from RRC 
  upper layer. The status field will indicate success or failure.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

  lte_errno_e status; /*!< DEPRECATED IN PHASE 3, Status of activation */

  lte_embms_status_e act_status; /*!< Status code for activation */

  lte_earfcn_t tmgi_earfcn; /*! EARFCN where TMGI was activated, will be
                               set only if activation status is Success */

  lte_rrc_embms_bearer_info_s bearer_info; /*!< Bearer info */

} lte_rrc_embms_act_tmgi_cnf_s;

/*! @brief Message for eMBMS TMGI deactivate confirm sent from 
  RRC to upper layer. This confirm is sent as the response to
  lte_rrc_embms_deact_tmgi_req. The status field will indicate
  success or failure. 
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

  lte_errno_e status; /*!< DEPRECATED IN PHASE 3, Status of deactivation */

  lte_embms_status_e deact_status; /*!< Status of deactivation */

  lte_rrc_embms_bearer_info_s bearer_info; /*!< Bearer info */

} lte_rrc_embms_deact_tmgi_cnf_s;

/* eMBMS related commands */
/*! @brief Message for eMBMS TMGI activate confirm sent from RRC upper layer. 
  The act_status field will indicate success or failure for activation.
  The deact_status will indicate success or failure for deactivation

 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767,
                            -1 when not used  */

  lte_embms_status_e act_status; /*!< Status of activation */

  lte_earfcn_t act_tmgi_earfcn; /*! EARFCN where TMGI was activated, will be
                               set only if activation status is Success */

  lte_rrc_embms_bearer_info_s act_bearer_info; /*!< Activated Bearer info */

  lte_embms_status_e deact_status; /*!< Status of deactivation */

  lte_rrc_embms_bearer_info_s deact_bearer_info; /*!< Deactivated Bearer info */


} lte_rrc_embms_act_deact_tmgi_cnf_s;

/*! @brief enum for RRC procedure status indication
    LTE_RRC_PROCEDURE_IN_PROGRESS: is sent when RRC procedure starts or when CPHY requests
    	for procedure status and procedure is in progress
    LTE_RRC_PROCEDURE_NOT_IN_PROGRESS: is sent when ALL RRC procedures are done processing
    	or when CPHY requests for procedure status and no procedure is in progress
 */
typedef enum
{
  LTE_RRC_PROCEDURE_IN_PROGRESS = 0,   
  LTE_RRC_PROCEDURE_NOT_IN_PROGRESS
 
} lte_rrc_procedure_status_ind_e;

/*! @brief Indication RRC sends indicating the Status of RRC procedure
	Ind is sent when
	1. CPHY requests for RRC Procedure Status
	2. When any RRC procedure starts
	3. When ALL RRC procedures are done processing
*/
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */
    lte_rrc_procedure_status_ind_e rrc_procedure_status;         /*!< Status */
} lte_rrc_procedure_status_ind_s;
 
/*! @brief Indication NAS sends indicating the SGLTE mode to RRC
	Ind is sent when mode is changed due to various triggers.
*/
typedef struct 
{
  msgr_hdr_s                     msg_hdr;
  sys_ue_mode_e_type             ue_mode;
  boolean                        is_ue_mode_substate_srlte;
}lte_rrc_ue_mode_ind_s;
 
typedef enum
{
   LTE_RRC_CSFB_CALL_STARTED = 0,
   LTE_RRC_CSFB_CALL_ENDED   = 1
}  lte_rrc_csfb_call_status_type_e;

typedef struct 
{
   msgr_hdr_s msg_hdr; /*!< Message router header */
   lte_rrc_csfb_call_status_type_e call_status_e;         /*!< Status */
}lte_rrc_csfb_call_status_ind_s;

/*! @brief ENUM decribing the START of E911 Emergency call 
  and end of the callback time for the emergency call
*/
typedef enum
{
   LTE_RRC_E911_CALL_STARTED     = 0,
   LTE_RRC_E911_CALLBACK_ENDED   = 1

}  lte_rrc_e911_call_status_type_e;

/* POLICYMAN_CFG_FREQ_LIST_UPDATE_IND */
/*! @@brief Indication PM sends frequency list update indicating to LTE RRC
    Ind is sent when UE location(Home/Roaming) is changed due to various triggers.
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
}lte_rrc_freq_list_update_ind_s;

/*! @brief Indication that CM sends to RRC Notifying the START 
  and STOP of an emergency E911 call.
*/
typedef struct 
{
  msgr_hdr_s                           msg_hdr;
  lte_rrc_e911_call_status_type_e      e911_call_status;

}lte_rrc_e911_call_status_ind_s;

typedef struct 
{
   msgr_hdr_s msg_hdr; /*!< Message router header */
   boolean enable_srvcc_cap;  /*!< If set to TRUE, enable SRVCC cap,
                                  else disable SRVCC cap */
}lte_rrc_ue_cap_change_ind_s;

/*! Below indication should be sent by W/G/TDS to pass requested PLMN to LTE. 
  This indication is useful only when LTE deprioritization list is valid.
  L-RRC would ignore this UMID otherwise. */
typedef struct 
{
   msgr_hdr_s msg_hdr; /*!< Message router header */

   /*! @brief Requested PLMN in NAS service request */
   sys_plmn_id_s_type requested_plmn;

}lte_rrc_depri_requested_plmn_ind_s;

/*! @brief Message for service domain indication
IND will be sent to L-RRC to identify whether the current LTE subscription is DDS SUB using service domain of other sub (SUB2)
*/
 
typedef struct
{
  msgr_hdr_s          msg_hdr;               /*!< Message router header */
 
  /* Requested Service domain, CS Only, PS Only or CS_PS */
  sys_srv_domain_e_type              service_domain;
 
} lte_rrc_nas_domain_pref_ind_s;

/* Structure of lte_rrc_pseudo_resel_low_priority_ind_s */
/*! @brief External indications that RRC throws to NAS layer
    Note: Once current serving cell priority is lower than all wcdma neighbors
    cell priority, LRRC will sent this indicator to NAS 
*/
typedef struct
{
  msgr_hdr_s     msg_hdr;               /* !< Message router header */
  lte_earfcn_t   earfcn;                /*!< DL Frequency */
}lte_rrc_pseudo_resel_low_priority_ind_s;

/*! @brief External indications that RRC throws
    Note: To calculate umid id consider the Max umid id of
    this enum and following enum containing 
    External indications that RRC receives

*/

enum
{
  MSGR_DEFINE_UMID(LTE, RRC, IND, ACTIVATION,               0x00, 
                                                    lte_rrc_act_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, SERVICE,                  0x01, 
                                                    lte_rrc_service_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, CONN_REL,                 0x03, 
                                                    lte_rrc_conn_rel_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, DL_DATA,                  0x04, 
                                                    lte_rrc_dl_data_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, PAGE,                     0x05, 
                                                    lte_rrc_page_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, ACTIVE_EPS_BEARER_UPDATE, 0x06, 
                                       lte_rrc_active_eps_bearer_update_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, CONN_EST_TIMER_UPDATE, 0x07,
                                        lte_rrc_conn_est_timer_update_ind_s),
  /* The definition for LTE_RRC_HANDOVER_COMPLETED_IND with IDNUM 0x08 and 
    LTE_RRC_CONFIG_COMPLETED_IND with IDNUM 0x09 are present 
    in lte_ind_msg.h; Next one should start with 0x0A  */
  /* ETWS indications */
  MSGR_DEFINE_UMID(LTE, RRC, IND, ETWS_PRIM, 0x0b, lte_rrc_etws_prim_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, ETWS_SEC, 0x0c, lte_rrc_etws_sec_ind_s),
  
  MSGR_DEFINE_UMID(LTE, RRC, IND, IRAT_TUNNEL_DL_MSG,    0x0d, lte_rrc_irat_tunnel_dl_msg_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, 1XCSFB_HO_STARTED, 0x0e, lte_rrc_1xcsfb_ho_started_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, 1XCSFB_HO_FAILED,  0x0f, lte_rrc_1xcsfb_ho_failed_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, NAS_UMTS_KEY,  0x10, lte_rrc_nas_umts_key_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, CMAS, 0x11, lte_rrc_cmas_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, NAS_LTE_KEY,  0x12, lte_rrc_nas_lte_key_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, IRAT_HDR_TUNNEL_DL_MSG,   0x13, lte_irat_cdma_tunnel_dl_msg_ind_s),

  MSGR_DEFINE_UMID(LTE, RRC, IND, EMBMS_DEACT_TMGI, 0x14, lte_rrc_embms_deact_tmgi_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, EMBMS_AVAIL_TMGI_LIST, 0x15, lte_rrc_embms_avail_tmgi_list_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, EMBMS_ACT_TMGI_LIST, 0x16, lte_rrc_embms_act_tmgi_list_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, EMBMS_COVERAGE_STATE, 0x17, lte_rrc_embms_coverage_state_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, EMBMS_STATUS_CHANGE, 0x18, lte_rrc_embms_status_change_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, EMBMS_OOS_WARN, 0x19, lte_rrc_embms_oos_warn_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, NAS_GSM_KEY,  0x1A, lte_rrc_nas_umts_key_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, PROCEDURE_STATUS,0x1B, lte_rrc_procedure_status_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, UTC_TIME_UPDATE, 0x1C, lte_rrc_utc_time_update_ind_s),

  MSGR_DEFINE_UMID(LTE, RRC, IND, CSG_INFO_UPDATE, 0x1D, lte_rrc_csg_info_update_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, EMBMS_AVAIL_SAI_LIST, 0x1F, lte_rrc_embms_avail_sai_list_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND,  BARRING_UPDATE, 0x21, lte_rrc_barring_update_ind_s), 
  MSGR_DEFINE_UMID(LTE, RRC, IND, 1XSRVCC_HO_FAILED,  0x22, lte_rrc_1xsrvcc_ho_failed_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, 1XSRVCC_HO_STARTED, 0x24, lte_rrc_1xsrvcc_ho_started_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, UE_CAP_INFO, 0x26, lte_ue_cap_info_s),  
  MSGR_DEFINE_UMID(LTE, RRC, IND, NAS_DOMAIN_PREF, 0x27, lte_rrc_nas_domain_pref_ind_s),  
  MSGR_DEFINE_UMID(LTE, RRC, IND, EAB_UPDATE, 0x29, lte_rrc_eab_update_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, PESUDO_RESEL_LOW_PRIORITY, 0x2F, lte_rrc_pseudo_resel_low_priority_ind_s),
};
/*! @brief External indications that RRC receives
    Note: To calculate umid id consider the Max umid id of
    this enum and above enum containing 
    External indications that RRC sends
 */
enum
{
  /* CSFB call status indication from NAS to LTE used by TO_W_MGR 
    to decide internal G redirection on W redirection failure
  */
  MSGR_DEFINE_UMID(LTE, RRC, IND, CSFB_CALL_STATUS, 0x1E, lte_rrc_csfb_call_status_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, UE_MODE,          0x20, lte_rrc_ue_mode_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, E911_CALL_STATUS, 0x23, lte_rrc_e911_call_status_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, UE_CAP_CHANGE, 0x25, lte_rrc_ue_cap_change_ind_s),
  MSGR_DEFINE_UMID(LTE, RRC, IND, DEPRI_REQUESTED_PLMN, 0x28, lte_rrc_depri_requested_plmn_ind_s),
};

/*! @brief Message for Activation Indication sent from RRC to NAS for
    procedures within 3GPP
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_act_reason_e act_reason; /*!< Activation reason */
} lte_rrc_act_ind_s;

/*! @brief Message for Service Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint32 trans_id; /*!< Transaction ID, reflects one in svc request or 0xFFFF 
                        for asynchronous indication */

  lte_rrc_svc_status_e svc_status; /*!< Service status */

  /* valid only when svc_ind_type is LTE_RRC_SVC_IND_NORMAL and svc_status
     = LTE_RRC_SVC_STATUS_AVAILABLE */
  lte_rrc_camped_svc_info_s camped_svc_info; /*!< Service info */

  /* when svc_ind_type is LTE_RRC_SVC_IND_PLMN_LIST_CNF, it is the found list,
     when it is LTE_RRC_SVC_IND_NORMAL, it is the list of acceptable cells
     found if svc_ind_status is LTE_RRC_SVC_STATUS_NOT_AVAILABLE */
  lte_rrc_found_plmn_list_s found_plmn_list; /*!< Found PLMN list */

  boolean uoos_search_done; /*!< TRUE if at least one scan completed before 
                                 OOS timer expires */

  uint32 uoos_rem_awake_time; /*!< Time in ms that remains for UE to search
                                   - currently unused and just set to 0 */

  uint32 uoos_rem_sleep_time; /*!< Time in ms that remains for UE to sleep 
                                   - currently unused and just set to 0 */

  uint64 t311_rem_time; /*!< Time in ms remaining for T311 when running, 0
                             otherwise */

  sys_no_svc_cause_e_type no_svc_cause; /*!< If svc_status == 
          LTE_RRC_SVC_STATUS_NOT_AVAILABLE and this is first indication after 
          RLF, value is SYS_NO_SVC_CAUSE_RLF, otherwise SYS_NO_SVC_CAUSE_NORMAL 
          */

  /*! If svc_status is LTE_RRC_SVC_STATUS_AVAILABLE, it is FALSE. If Limited
      Service was requested, it is FALSE.

      If svc_status is LTE_RRC_SVC_STATUS_NOT_AVAILABLE and Full Service 
      (Automatic or Manual) was requested, and the found PLMN list is not empty,
      or if it is empty because a PLMN was not put in it due to failure of 
      forbidden TA check, it is FALSE. Else it is TRUE. */
  boolean use_oos;

  /*! Boolean value indicating if SIB-8 is available on the network.
      The presence of SIB-8 indicates potential for 1xCSFB service by the network.
      This information is used by Domain Selection for Voice Centric LTE Devices.  */
  boolean sib8_available;

  /* Division duplex type - None, FDD or TDD  */
  lte_rrc_div_duplex_e div_duplex;

  /* boolean flag to indicate whether force detach is required*/
  boolean detach_reqd;

  /*indicates whether acquisition succeeded or failed due to TRM failure*/
  sys_acq_status_e_type  acq_status;

} lte_rrc_service_ind_s;

/*! @brief Message for Barring Update Indication
1. IND will be sent to QMI-NAS/CM to update them about 
SSAC / ACB / emergency parameters whenever these values 
change OTA. 
2. IND would also be sent when LTE enters CONNECTED 
state */ 
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*!< SSAC params. Valid flag within struct indicates 
    validity of field*/
  lte_rrc_ssac_params_s ssac_params; 

  /*! ACB params. Boolean present within struct for 
    each kind of barring indicates validity of the params*/ 
  lte_rrc_ac_barring_info_s  ac_barring_info; 

  /* NAS to allow VoLTE calls even though AC barring 
  timer are running due to any previous data call that 
  fail AC barring check
  RRC will set this Boolean in below case
  1.  SSAC is enabled in SIB2 by NW.
  2.  Camped PLMN belong to one of EFS listed PLMN where 
  we need to enable this special case of allowing VoLTE 
  call without ACB check at RRC   
  */
  boolean skip_acb_for_volte_call;
  /*!< emergency access barred based on 36.331 5.3.3.2. 
    Needed for CM*/
  boolean emergency_access_barred; 
  /* !<Camped PLMN */
  lte_rrc_plmn_s camped_plmn;
} lte_rrc_barring_update_ind_s;

/*! @brief Message for CSG Info Update Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_plmn_s selected_plmn; /*!< Selected PLMN - req PLMN or its equivalent
                                     or first entry in SIB1 list for limited
                                     service */
  sys_csg_info_s_type csg_info; /*!< CSG Info of the camped cell */
} lte_rrc_csg_info_update_ind_s;

/*! @brief enumeration to identify the dayLightSavingTime type 
    SPEC: TS 24.301 [35] and TS 24.008 [49]
    It is a type 4 information element with a length of 3 octets
*/
typedef enum
{
  LTE_UTC_DAYLIGHT_NONE,     /*!< NO adjustment needed*/
  LTE_UTC_DAYLIGHT_PLUS_ONE, /*!< +1 hour adjustment*/
  LTE_UTC_DAYLIGHT_PLUS_TWO, /*!< +2 hour adjustment*/
  LTE_UTC_DAYLIGHT_RESERVED  /*!< reserved */

}lte_rrc_utc_dayLightSaving_type_e;

/*! @brief Message for UTC Time Update Indication sent from 
  RRC to QMI-NAS to deliver SIB 16 UTC time information
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  sys_modem_as_id_e_type as_subs_id; /*!< subscription ID */

  /*! If UTC SIB 16 Time is Acquired - TRUE
  ELSE - FALSE*/
  boolean utc_sib16_acquired;

  /*! If UTC dayLightSavingTime is enabled - TRUE
  ELSE - FALSE*/
  boolean utc_dayLightSavingTime_enabled;

  /*! UTC dayLightSavingTime*/
  lte_rrc_utc_dayLightSaving_type_e dayLightSavingTime;

  /*! If UTC leapSeconds is enabled - TRUE
  ELSE - FALSE*/
  boolean utc_leapSeconds_enabled;

  /*! UTC leapSeconds in units of seconds */
  int8 leapSeconds;

  /*! If UTC localTimeOffset is enabled - TRUE
  ELSE - FALSE*/
  boolean utc_localTimeOffset_enabled;

  /*! UTC localTimeOffset in units of 15 min*/
  int8 localTimeOffset;

} lte_rrc_utc_time_update_ind_s;

/*! @brief Info regarding Redir if Connection Release is due
 *         to redirection.
*/
typedef struct
{
  boolean                     is_redir;   /*!< Conn Rel due to Redir? */
  lte_rrc_redir_rat_type_e    redir_rat;  /*!< Redirected RAT if TRUE */
  /* Release 10 field */
  boolean cs_fallback_highPriority;      /*!< CS Fallback High Priority applicable only for UTRA TDD and FDD*/
} lte_rrc_conn_rel_redir_info_s;

/*! @brief Message for Connection Release Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_conn_rel_reason_e rel_reason; /*!< Release reason */
  boolean load_balancing_tau_is_reqd; /*!< Is load balancing TAU needed */
  lte_rrc_conn_rel_redir_info_s redir_info;  /*!< Redir Info if Con Rel is due to redir. Valid for LTE_RRC_CONN_REL_NORMAL */
  uint16 extended_wait_time;  /*!< Extended Wait time in seconds for Delay Tolerant Access requests; Valid Range (1 to 1800); 0 indicates value not applicable */
} lte_rrc_conn_rel_ind_s;

/*! @brief Message for DL Data Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  msgr_attach_s dsm_attach; /*!< dsm attachment
                                 containing the NAS Message Container */

} lte_rrc_dl_data_ind_s;

/*! @brief Message for IRAT Tunnel DL Message Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*! Union of IRAT Tunnel DL Message types */
  lte_irat_tunnel_dl_msg_ind_s msg;

} lte_rrc_irat_tunnel_dl_msg_ind_s;

/*! @brief Message for (eCSFB) HO started Indication.
 *         This indication acts a confirmation to stop T3417 for the ESR procedure.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_1xcsfb_ho_started_ind_s;

/*! @brief Message for (SRVCC) HO started Indication.
 *         This indication helps NAS to initiate TAU on SRVCC success/failure.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

} lte_rrc_1xsrvcc_ho_started_ind_s;

/*! @brief Message for HO failed Indication.
 *         This indication informs NAS that the HO to 1x and thus
 *         the 1xCSFB call has failed. (along with the error code.)
 *
 *         The failure cause is propagated to CM to decide on next steps
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_irat_1xcsfb_ho_failure_cause_e  failure_cause;
} lte_rrc_1xcsfb_ho_failed_ind_s;

/*! @brief Message for HO failed Indication.
 *         This indication informs NAS that the SRVCC-HO to 1x has failed.
 *         (along with the error code.)
 *
 *         NAS needs to perform TAU once connection is re-established
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_irat_1xsrvcc_ho_failure_cause_e  failure_cause;
} lte_rrc_1xsrvcc_ho_failed_ind_s;

/*! @brief Message for NAS to do UMTS Key derivation 
 *         (DL NAS Count obtained from MobilityFromEUTRACommand parameter
 *         SecurityParamsFromEUTRA IE.)
 *         
 *         NAS derives UMTS mapped security context from this info.
 *
 *         This is used for LTE -> W PS HO.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint8   dl_nas_count;
} lte_rrc_nas_umts_key_ind_s;

/*! @brief Message for NAS to do LTE Key derivation 
 *         (Contents defined in TS 24.301, Section 9.9.2.7)
 *         
 *         NAS derives LTE mapped security context from this info.
 *         and returns results back to RRC using the existing
 *         LTE_RRC_SIM_UPDATE_REQ
 *
 *         This is used for W -> LTE PS HO.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  unsigned char nas_sec_param_to_eutra[LTE_RRC_NAS_SEC_PARAM_TO_EUTRA_LEN]; /*!< NAS Security param to EUTRAN */
  
} lte_rrc_nas_lte_key_ind_s;

/*! @brief Message for ETWS primary notification
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_plmn_s plmn; /*!< PLMN id for the cell */

  lte_phy_cell_id_t phy_cell_id; /*!< Physical cell id */

  lte_earfcn_t earfcn; /*!< DL Frequency */

  lte_rrc_msg_id_t msg_id; /*!< Message identifier from SIB10 */

  uint16 sr_num; /*!< Serial number from SIB10 */

  unsigned char warning_type[LTE_RRC_ETWS_WARN_TYPE_LEN]; /*!< Warning type */

  boolean warning_sec_info_present; /*!< Boolean indicating if warning_sec_info 
                                         is present */

  unsigned char warning_sec_info[LTE_RRC_ETWS_WARN_SEC_INFO_LEN]; /*!< Warning 
                                         security info */
} lte_rrc_etws_prim_ind_s;

/*! @brief Message for ETWS secondary notification
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  msgr_attach_s dsm_attach; /*!< dsm attachment containing the warning message */

  lte_rrc_plmn_s plmn; /*!< PLMN id for the cell */

  lte_phy_cell_id_t phy_cell_id; /*!< Physical cell id */

  lte_earfcn_t earfcn; /*!< DL Frequency */

  lte_rrc_msg_id_t msg_id; /*!< Message identifier from SIB11 */

  uint16 sr_num; /*!< Serial number from SIB11 */

  boolean data_coding_scheme_present; /*!< Boolean indicating if data_coding_scheme 
                                           is present */

  unsigned char data_coding_scheme; /*!< Data coding scheme from SIB11 */
} lte_rrc_etws_sec_ind_s;

/*! @brief Message for CMAS notification
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  msgr_attach_s dsm_attach; /*!< dsm attachment containing the warning message */

  lte_rrc_plmn_s plmn; /*!< PLMN id for the cell */

  lte_phy_cell_id_t phy_cell_id; /*!< Physical cell id */

  lte_earfcn_t earfcn; /*!< DL Frequency */

  lte_rrc_msg_id_t msg_id; /*!< Message identifier from SIB12 */

  uint16 sr_num; /*!< Serial number from SIB12 */

  unsigned char data_coding_scheme; /*!< Data coding scheme from SIB12, 
                                         present only for first segment */

} lte_rrc_cmas_ind_s;


/*! @brief REQUIRED brief description of this enumeration typedef
*/
typedef enum
{
  LTE_RRC_PAGING_CAUSE_FFS = 0,   /*!< Cause values are FFS currently in the 
                                       spec */

  LTE_RRC_PAGING_CAUSE_MAX

} lte_rrc_paging_cause_e;


/*! @brief REQUIRED brief description of this enumeration typedef
*/
typedef enum
{
  LTE_RRC_PAGING_CN_DOMAIN_PS = 0,   /*!< Circuit-switched */
  LTE_RRC_PAGING_CN_DOMAIN_CS = 1,   /*!< Packet-switched */

  LTE_RRC_PAGING_CN_DOMAIN_MAX

} lte_rrc_paging_cn_domain_e;


/*! @brief Message for Page Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  lte_rrc_ue_id_s ue_id;                /*!< UE id used in the paging message */
  lte_rrc_paging_cause_e cause;         /*!< Paging cause */
  lte_rrc_paging_cn_domain_e cn_domain; /*!< Paging origin (cs/ps) */

} lte_rrc_page_ind_s;

/*! @brief Message for Active EPS Bearer Update Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint8 num_active_bearers; /*!< Number of active EPS bearers */
  /*!< All EPS bearers that are currently active, this includes bearers that 
  have been newly added as well as those that are already existing */
  lte_rb_eps_bearer_map_info_s active_eps_bearers[LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS]; 
  uint8 num_added_bearers; /*!< Number of added EPS bearers */
  /*!< EPS bearers that have been newly added */
  lte_rb_eps_bearer_map_info_s added_eps_bearers[LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS]; 
  uint8 num_removed_bearers; /*!< Number of removed EPS bearers */
  /*!< Removed EPS bearers */
  lte_rb_eps_bearer_map_info_s removed_eps_bearers[LTE_RRC_MAX_NUM_ACTIVE_EPS_BEARERS]; 

} lte_rrc_active_eps_bearer_update_ind_s;


/*! @brief Message for Connection Establishment Timer Update Indication
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  uint8 call_type_bitmask; /*!< Bit mask indicating for which calls the barring 
                                info is valid. Bit mask is 0x1<<est_cause
                                i.e. for example,
                                Bit mask is 
                                0x1<<LTE_RRC_EST_CAUSE_EMERGENCY 
                                for Emergency calls*/

  uint64 barring_timer_value; /*!< Value of the barring timer in ms.
                                LTE_RRC_INDEFINITE_BARRING_TIME (0xffffffff)
                                means barred indefinitelyfor Emergency calls.
                                If the access barring for Emergency calls is
                                lifted, barring_timer_value will be set to 0*/
} lte_rrc_conn_est_timer_update_ind_s;

/* eMBMS related commands */
/*! @brief Message for eMBMS TMGI deactivate indication sent from RRC 
  to upper layer with deactivation reason for all the eMBMS services.
  This is only used for modem-initiate eMBMS deactivation.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id, range 0 to 32767,
                            -1 when not used  */

  lte_rrc_embms_deact_reason_e reason; /*!< eMBMS deactivation reason */
  uint8 num_tmgi_deactivated; /*!< Number of deactivated tmgi in deact_info_list */
  lte_rrc_embms_bearer_info_s deact_info_list[LTE_EMBMS_MAX_ACTIVE_SESSIONS]; /*!< List of deact_info */

} lte_rrc_embms_deact_tmgi_ind_s;

/*! @brief Message for eMBMS available TMGI list indication sent as a 
  response to lte_rrc_embms_avail_tmgi_list_req. Currently this list
  will return the available TMGIs across all MCCHs.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  msgr_attach_s dsm_attach; /*!< dsm attachment containing the List of available TMGIs,
                                 containing maximum of LTE_EMBMS_MAX_AVAIL_SESSIONS */

  int16 debug_trace_id; /*!< Debug trace id for the req, if sent in response to
                            available TMGI list req, range 0 to 32767, -1 if not used or 
                            For each modem initiated indication, the value is incremented
                            from last time. */

  uint8 num_available_embms_sessions; /*!< Number of available sessions in avail_tmgi_list */
  
  lte_rrc_embms_session_info_s session_info[LTE_EMBMS_MAX_ACTIVE_SESSIONS]; 
                                                    /*!< DEPRECATED IN PHASE 3 with move to DSM item */

} lte_rrc_embms_avail_tmgi_list_ind_s;


/*! @brief Message for eMBMS available SAI list indication sent as a 
  response to lte_rrc_embms_avail_sai_list_req. This list will return
  the available SAIs in SIB15 (SAIs in intra-freq as well as inter-freq).
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /*!< dsm attachment containing the m freq sai info */
  /*!< Format represented below. 
       -------------------------------------------------------------------------------------------
       |num_freq_sai_info|  freq 1  | serving_freq |  num_sai | sai 1   | ... | sai n   | ... | freq m  | serving_freq |  num_sai | sai 1   | ... | sai n   |
               m                        (boolean)         n                                                  (boolean)       n        
       |    1 byte       |  2 bytes | 1 byte       |  1 byte  | 2 bytes | ... | 2 bytes | ... | 2 bytes |    1 byte    |  1 byte  | 2 bytes | ... | 2 bytes | 
       -------------------------------------------------------------------------------------------*/

  msgr_attach_s dsm_attach; /*!< dsm attachment containing the List of available SAIs,
                             grouped by freq containing maximum of LTE_EMBMS_MAX_FREQ_SAI_INFO */

  int16 debug_trace_id; /*!< Debug trace id for the req, if sent in response to
                            available SAI list req, range 0 to 32767, -1 if not used or 
                            For each modem initiated indication, the value is incremented
                            from last time. */

} lte_rrc_embms_avail_sai_list_ind_s;

/*! @brief Message for eMBMS active TMGI list indication sent as a 
  response to lte_rrc_embms_act_tmgi_list_req or whenever the active
  TMGIs change. This will return all the active TMGIs to upper layer.
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    int16 debug_trace_id; /*!< Debug trace id, range 0 to 32767, -1 if not used,
                             Incremented each time modem sends Activate TMGI 
                             list indication */

    uint8 num_active_embms_sessions; /*!< Number of active sessions in act_tmgi_list */

    lte_rrc_embms_bearer_info_s session_info[LTE_EMBMS_MAX_ACTIVE_SESSIONS]; /*!< Includes TMGIs and session IDs */

} lte_rrc_embms_act_tmgi_list_ind_s;

/*! @brief enumeration to identify embms coverage status
*/
typedef enum
{
  LTE_RRC_EMBMS_COVERAGE_STATUS_NOT_AVAILABLE, /*!< 0 - when UE is not in eMBMS coverage - generic*/
  LTE_RRC_EMBMS_COVERAGE_STATUS_AVAILABLE, /*!< 1 - when UE is in eMBMS coverage */
  LTE_RRC_EMBMS_COVERAGE_STATUS_UNKNOWN, /*!< 2 - when coverage is unknown(like when LTE is being stopped)*/
  LTE_RRC_EMBMS_COVERAGE_STATUS_NOT_AVAIL_DUE_TO_UEMODE, /*!< 3 - when UE is not in embms coverage 
                                                                  for modes like DSDS/DSDA/1xSRLTE*/
  LTE_RRC_EMBMS_COVERAGE_STATUS_NOT_AVAIL_DUE_TO_E911, /*!< 4 - when UE is not in embms coverage 
                                                                due to on-going E911 emergency*/
  LTE_RRC_EMBMS_COVERAGE_STATUS_MAX = 255 /*!< 5-255 Reserved for future use */
} lte_rrc_embms_coverage_status_e;

/*! @brief Message for eMBMS coverage indication sent as a response to 
  lte_rrc_embms_coverage_state_req from RRC to upper layer notifying
  if UE is in eMBMS coverage.
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    int16 debug_trace_id; /*!< Debug trace id, range 0 to 32767, -1 if not used,
                             Incremented each time modem sends coverage status 
                             indication*/

    lte_rrc_embms_coverage_status_e coverage_status; /*!< 0-No Coverage; 1-In Coverage; 2-Unknown */


} lte_rrc_embms_coverage_state_ind_s;

/*! @brief Message for eMBMS coverage status change notification sent from 
  RRC to upper layer whenever eMBMS service is enabled/disabled.
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767, 
                               -1 when not used  */

    boolean enabled_status; /*!< TRUE if eMBMS is enabled */

} lte_rrc_embms_status_change_ind_s;

/*! @brief Data Structure for OOS warning indication. Contains the reason for
  the warning and the list of TMGIs impacted.
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id, range 0 to 32767, -1 if not used,
                           Incremented each time modem sends OOS warning 
                           indication */

  lte_rrc_embms_warn_reason_e reason; /*!< eMBMS warning reason */

  uint8 num_sessions_affected; /*!< Number of eMBMS sessions affected by this warning ind */

  lte_rrc_embms_bearer_info_s affected_session_info[LTE_EMBMS_MAX_ACTIVE_SESSIONS]; 
                                                /*! List of affected eMBMS sessions */

} lte_rrc_embms_oos_warn_ind_s;

/*! 
  @brief Data Structure to indicate upper layers the current signal quality and
  Number of active sessions for each MBSFN Area
*/
typedef struct
{
   
  lte_mbsfn_area_id_t  area_id; /*! MBSFN area id: 0 - LTE_EMBMS_MAX_MBSFN_AREA */

  int16 snr; /*! The average SNR of the serving cell over last measurement 
                 period in decibels Q3[2 ^3] format result SNR in 
                 cnf = raw SNR value * 2^3. Range -10..30 */

  int16 excess_snr; /*! The excess SNR of the serving cell over last 
                        measurement period in decibels Q3[2 ^3] format.
                        Range -10..30 */

  uint8 num_active_embms_sessions; /*!< Number of active sessions that are active */

  lte_rrc_embms_bearer_info_s session_info[LTE_EMBMS_MAX_ACTIVE_SESSIONS]; 
                              /*!< Includes TMGI IDs and session IDs of active TMGIs */
 
} lte_rrc_embms_mbsfn_area_info;

/*! @brief Message for eMBMS Signal level reporting sent as a 
  CNF to NAS. Currently this list will return:
  ->the active TMGIs per MBSFN AREA across all MCCHs
  ->mbsfn area_id
  ->SNR
  ->excessSNR
 */
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767, -1 if not used or 
                            For each modem initiated indication, the value is incremented
                            from last time. */

  uint8  num_mbsfn_areas; /*! Number of MBSFN areas */

  lte_rrc_embms_mbsfn_area_info   mbsfn_area_info[LTE_EMBMS_MAX_MBSFN_AREA];
                                                  /*! Signal level Information per MBSFN Area */

} lte_rrc_embms_signal_strength_report_cnf_s;

/*! @brief Message for eMBMS Signal level reporting sent as a 
  Request from NAS. 
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  int16 debug_trace_id; /*!< Debug trace id for the req, range 0 to 32767, -1 if not used or 
                            For each modem initiated indication, the value is incremented
                            from last time. */
} lte_rrc_embms_signal_strength_report_req_s;

/*! @brief External responses that RRC receives
 */
enum
{
    MSGR_DEFINE_UMID(LTE, RRC, RSP, ACTIVATION, 0x80, lte_rrc_act_rsp_s),
    MSGR_DEFINE_UMID(LTE, RRC, RSP, NAS_UMTS_KEY, 0x01, lte_rrc_nas_umts_key_rsp_s),
    MSGR_DEFINE_UMID(LTE, RRC, RSP, NAS_LTE_KEY, 0x02, lte_rrc_nas_lte_key_rsp_s),
    MSGR_DEFINE_UMID(LTE, RRC, RSP, NAS_GSM_KEY, 0x03, lte_rrc_nas_umts_key_rsp_s),
    MSGR_DEFINE_UMID(LTE, RRC, RSP, CELL_LOCK, 0x04, lte_rrc_cell_lock_rsp_s),
};


/*! @brief Message for Activation Response from NAS for
    procedures within 3GPP
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    boolean status; /*!< Has NAS accepted activation indication? */
    lte_rrc_plmn_s plmn; /*!< PLMN identity */
} lte_rrc_act_rsp_s;

/*! @brief Message for Mapped UMTS Key Context for L2W PS HO
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    lte_irat_umts_sec_info_s sec_info;
} lte_rrc_nas_umts_key_rsp_s;

/*! @brief Message for Mapped LTE Key Context for W2L PS HO
 */
typedef struct
{
    msgr_hdr_s msg_hdr; /*!< Message router header */

    lte_rrc_security_info_s security_info; /*!< Security info */
} lte_rrc_nas_lte_key_rsp_s;

/*! @brief Supervisory messages that RRC receives or throws
*/
enum
{
  MSGR_DEFINE_UMID(LTE, RRC, SPR, LOOPBACK, MSGR_ID_LOOPBACK, 
                   msgr_spr_loopback_s),
  MSGR_DEFINE_UMID(LTE, RRC, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY, 
                   msgr_spr_loopback_reply_s),
  MSGR_DEFINE_UMID(LTE, RRC, SPR, THREAD_READY, LTE_APP_ID_THREAD_READY, 
                   none),
  MSGR_DEFINE_UMID(LTE, RRC, SPR, THREAD_KILL, LTE_APP_ID_THREAD_KILL, 
                   none),

  MSGR_DEFINE_UMID(LTE, RRC, SPR, INT_MSG, 5, none),
  MSGR_DEFINE_UMID(LTE, RRC, SPR, EXT_MSG, 6, none),
  MSGR_DEFINE_UMID(LTE, RRC, SPR, STIMULI, 7, none),
};

/*! @brief Structure for Cell Identity
*/
typedef struct
{
  lte_phy_cell_id_t phy_cell_id;  /*!< Physical cell id */
  lte_earfcn_t freq;  /*!< Frequency */
} lte_rrc_cell_id_s;

/*! @brief The message structure for Cell Lock Req
*/
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  uint8 num_cell_id;

  lte_rrc_cell_id_s cell_id_list[LTE_RRC_MAX_NUM_CELL_LOCK];
  
} lte_rrc_cell_lock_req_s;

/*! @brief Enumeration for Cell Lock Status
*/
typedef enum
{
  LTE_RRC_CELL_LOCK_STATUS_SUCCESS,
  LTE_RRC_CELL_LOCK_STATUS_FAIL_LTE_ACTIVE,
  LTE_RRC_CELL_LOCK_STATUS_FAIL_OTHER
} lte_rrc_cell_lock_status_e;

/*! @brief The message structure for Cell Lock Rsp
 */
typedef struct
{
  /*! Message header */
  msgr_hdr_struct_type msgr_hdr;

  lte_rrc_cell_lock_status_e status;
} lte_rrc_cell_lock_rsp_s;

/*! @brief Cell information */
typedef struct
{
  sys_sys_mode_e_type  rat_type;/*RAT type can be 1x/DO*/
  sys_channel_type      channel;
  uint64                avoid_time;/*! barred time in seconds*/
} lte_rrc_avoided_channel_info;

/*! @brief Message for providing 1x/DO avoidance list
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  uint32     num_avoid_channels;
  lte_rrc_avoided_channel_info avoided_channel_list[LTE_RRC_MAX_1X_DO_AVOID_CHANNELS];
} lte_rrc_avoidance_req_s;

/*! @brief Message for scaling Idle DRX/Search/Meas Request
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */

  /* To be set to TRUE when scaling factor is configured */
  boolean drx_scaling_factor_valid;

  /*!< LTE_RRC_DRX_CN_COEFFICIENT_S1_T32 for default configuration */ 
  uint8 drx_scaling_factor;

  /* To be set to TRUE when scaling factor is configured */
  boolean skip_idle_srch_meas_valid;
  
  /* To be set to TRUE when idle srch/meas have to be skipped.,
  else default FALSE must be set */
  boolean skip_idle_srch_meas;

} lte_rrc_mtc_cfg_req_s;

/*! @brief Message for scaling Idle DRX/Search/Meas confirmation
*/
typedef struct
{
  msgr_hdr_s msg_hdr; /*!< Message router header */
  errno_enum_type status; /*!< E_SUCCESS if cfg req passed, E_FAILURE otherwise */
} lte_rrc_mtc_cfg_cnf_s;

#endif /* LTE_RRC_EXT_MSG_H */

