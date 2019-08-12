#ifndef _MM_TASK_INT_H
#define _MM_TASK_INT_H
/*===========================================================================
                          MM Interface

DESCRIPTION

 This module defines contains the defines, structures and function
 prototypes required to send a command to the MM task.


EXTERNALIZED FUNCTIONS

 mm_put_cmd           Put a MM message on the MM command queue
 mm_get_cmd_buf       Get a MM command buffer

 mm_rrc_get_cmd_buf   RRC specific get - fills in msg set and length

 mm_timer_expiry      Callback function when a MM timer expires

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mmtask.h_v   1.20   18 Jul 2002 11:41:56   vdrapkin  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/mmtask_int.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/00   TS      Initial Revision.
12/18/00   SK      Added RRC commands, enum types and rrcmm_get_cmd_buf
04/12/01   jca     Added more RRC commands.
05/03/01   jca     Added ref to rrc_act_cnf_type.
05/11/01   jca     Added RRC_DEACT_CNF.
05/23/01   jca     Added ref to rrc_close_session_cnf_type.
05/25/01   jca     Added SM/SMS/RABM interface definitions.
06/13/01   jca     Removed RRC_UNIT_DATA_IND and RRC_OPEN_SESSION_CNF.
08/15/01   ab      Renamed CM to CNM.
10/19/01   sbk     New function prototypes for plmn matching to be used by RRC
02/20/02   sbk     Added TC types to the mm command type
06/05/02   ks      Added new primitives and commands for supporting dual mode operation.
07/01/02   jca     Removed timer_enquiry from mm_cmd_type.
07/08/02   vdr     Added LLC_UNITDATA_IND primitive
07/18/02   vdr     Renamed FEATURE_GPRS to be FEATURE_GSM_GPRS compilation flag
07/31/02   jca     Externed mm_main() for use by TMC task.
09/10/02   vdr     Added prototype of the mm_gllc_get_cmd_buf()
09/23/02   mks     Added mmr_act_req as a member in union mm_int_cmd_type_T
10/10/02   mks     Added RRC_ACTIVATION_IND primitive to support GSM to WCDMA
                   Inter-RAT procedures
10/11/02   cd      Added rrc_stop_wcdma_mode_cnf to the command union
10/17/02   cd      enclosed rrc_stop_wcdma_mode_cnf within #ifdef FEATURE_INTERRAT_HANDOVER_WTOG
11/12/02   jca     Added RRC_CLOSE_SESSION_CNF.
12/06/02   mks     Added rr_gmm_gprs_suspension to the command union
01/10/03   jca     Added rr_activation_ind to the command union.
01/16/03   ks      Added gmmsm_session_close_req primitive between GMM-SM
02/12/03   vdr     Added sms_bearer_selection_req primitive between MM-WMS
02/12/03   vdr     Added CM->MM interface definitions to support PS Data Continuity feature
05/13/03   npr     Added gllcmm_llgmm_new_tlli_ind_type to mm_int_cmd_type_T.
06/04/03   ks      Added RRC_MODE_CHANGE_CNF for support MODE_CHANGE in UMTS
07/16/03   jca     Added rr_gmm_gprs_resumption_ind_T to the command union.
11/17/03   vdr     Added rrc_deact_cnf_type to the command union.
04/06/04   cd      LCS support
06/08/04   cd      LCS support. Removed rr_lcs_active_ind and added lcs_active_ind
08/12/04   cd      Replaced FEATURE_LCS by FEATURE_MMGPS
11/16/04   sv      Addition of a primitive - RRC_SERVICE_IND_FROM_OTA_MSG
06/06/05   abe     Added code for CCBS support
07/21/05   up      Added mmr_eq_plmn_change_ind in mm_cmd_type.
07/22/05   abe     Added support for RRC_WTOG_CCO_COMPLETE_IND.
10/08/05   abe     Added support for FEATURE_INTERRAT_PCCO_GTOW
12/14/05   abe     Supported changes for FEATURE_GSM_DTM
01/02/06   up      Added mmr_plmn_search_abort_reg in mm_cmd_type.
04/06/06   sn      Added RRC_HSDPA_HSUPA_CELL_IND in rrc_to_mm_sink_e_type.
                   Added rrc_3g_availability_cell_ind in mm_int_cmd_type_T
03/06/07   vsm   Added a primitive from RABM to GMM gmmrabm_rab_rel_ind_T to indicate RAB release by
                   AS for an active PDP.
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "rex.h"
#include "queue.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "timers_v.h"
#include "rrcmmif.h"
#include "timers_v.h"
#include "ms_timer_v.h"
#include "ms.h"      /* for the message set id MS_MM_RRC */
#include "mm_rr.h"
#include "reg_mm_v.h"
#include "cc_iei_v.h"
#include "cc_cc.h"
#include "ms_cc_mm_v.h"
#include "sm_gmm.h"
#include "rabm_gmm.h"
#include "sms_gmm.h"
#ifdef FEATURE_GSM_GPRS
#include "gllcmmif.h"
#include "gllcsmsif.h"
#endif /* FEATURE_GSM_GPRS */
#ifdef FEATURE_TC
#include "tc_mm.h"
#include "tc_gmm.h"
#endif

#if defined ( FEATURE_MMGPS ) || defined ( FEATURE_CGPS_UMTS_CP )
#include "mm_lsm.h"
#endif
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif
#include "mmtask_v.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern nas_mmgsdi_info_type mm_sim_mmgsdi_info_sim[MAX_NAS_STACKS];
#define mm_sim_mmgsdi_info mm_sim_mmgsdi_info_sim[mm_as_id]

extern mmgsdi_app_enum_type mm_sim_card_mode_sim[MAX_NAS_STACKS];
#define  mm_sim_card_mode mm_sim_card_mode_sim[mm_as_id]
#else
extern nas_mmgsdi_info_type mm_sim_mmgsdi_info;
extern mmgsdi_app_enum_type mm_sim_card_mode;
#endif

/* MMGSDI to MM primitives */
#define SIM_MM_SUBS_READY_IND  0x00
#define SIM_MM_USIM_AUTH_CNF  0x01
#define SIM_MM_GSM_ALGO_CNF  0x02
#define SIM_MM_GET_FILE_ATTR_CNF   0x03
#define SIM_MM_READ_FILE_CNF   0x04
#define SIM_MM_WRITE_FILE_CNF  0x05

/* Message posted from MCFG callback to MM Queue */
#define MM_MCFG_REFRESH_IND 0x00

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/





extern void mm_timer_expiry(unsigned long param);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern void mm_timer_expiry_sub1(unsigned long param);
#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
extern void mm_timer_expiry_sub2(unsigned long param);
#endif
#endif

/* Is this PLMN requested by MM, a HPLMN? */
boolean Is_nas_req_plmn_an_hplmn(rrc_plmn_identity_type plmn);

#endif /* _MM_TASK_H */
