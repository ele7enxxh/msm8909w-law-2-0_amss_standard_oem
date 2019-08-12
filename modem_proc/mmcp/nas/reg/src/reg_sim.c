/*==============================================================================
                             COPYRIGHT INFORMATION

Copyright (c) 2002-2011 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_sim.c_v   1.5   22 May 2002 15:12:30   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_sim.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
05/07/12   gps     Checking each plmn in EHPLMN list for out of range values. 
02/07/02   kwa     Initial version.
05/20/02   kwa     Replaced calls to ERR with MSG_ERROR.
05/21/02   kwa     Added more diagnostic F3 messages.
05/21/02   kwa     Pet the watchdog when reading or writing from the SIM.
05/22/02   kwa     Removed dependency on sys_detailed_plmn_list_s_type and
                   sys_plmn_list_s_type.
08/12/02   kwa     Added calls to gsdi_get_ef_length rather than
                   gsdi_get_file_attributes in order to quickly retrieve the
                   length of the cached EFs.
08/23/02   kwa     Cleaned up diagnostic messages.
08/26/02   kwa     Added function reg_sim_write_rplmn_rat to write RPLMNACT EF.
08/28/02   kwa     Removed calls to gsdi_get_file_attributes.
09/16/02   kwa     Updated calls to gsdi_sim_read and gsdi_sim_write due to
                   interface changes introduced by mm_gsdi.
10/01/02   kwa     Added functions to manage the state of the SIM.
10/07/02   kwa     Modified reg_sim_state_set and added
                   reg_sim_print_gprs_forbidden_plmn_list.
12/30/02   kwa     Updated reg_sim_print_gprs_forbidden_plmn_list, 
                   reg_sim_read_hplmn, reg_sim_read_preferred_plmn_list, and
                   reg_sim_read_fplmn_list to properly display F3 messages due
                   to changes in DIAG.
04/21/03   kwa     Modified the logic for reading the lists of preferred PLMNs
                   with access technology.  Added code to add a PLMN to the
                   preferred list if neither access technology is marked as
                   supported.
05/22/03   kwa     Modified reg_sim_find_plmn_in_list to pass the plmn_list_p
                   parameter as a const pointer to eliminate a lint warning.
                   Removed unused element length from the
                   reg_sim_plmn_w_act_list_s_type structure to eliminate
                   a lint warning.
08/12/03   kwa     Added code to properly account for forbidden location areas
                   and routing areas.
08/26/03   kwa     Modified reg_sim_read_cs_rplmn and reg_sim_read_ps_rplmn to
                   properly use the status indicatior in LOCI and PSLOCI.
10/15/03   kwa     Added code to read and write RPLMNACT to NVRAM.
12/03/03   kwa     Added code to read PSLOCI from NVRAM if not available on
                   the SIM/USIM.
03/17/04   kwa     Added function reg_sim_read_imsi.
04/04/04   kwa     Fixed a bug in reg_sim_read_imsi where the length was
                   included in the IMSI.
04/04/04   kwa     Added FEATURE_DISABLE_RPLMNACT.
04/15/04   kwa     Added function reg_sim_state_print.
04/19/04   kwa     Modifications required to support API changes in MMGSDI.
                   Now calling gsdi_nas_get_card_mode rather than
                   gsdi_get_card_mode and modified file_length variables to
                   be of type int32.
04/22/04   kwa     Included declaration for reg_sim_rplmn_rat_search_order
                   within FEATURE_DISABLE_RPLMNACT to eliminate a compiler
                   warning.
04/26/04   kwa     Added error checking in reg_sim_read_pplmn_w_act_list and
                   reg_sim_read_pplmn_list to prevent the functions from
                   writing beyond the length of the info array in
                   reg_sim_preferred_plmn_list_s_type.
04/27/04   kwa     Removed unnecessary -1 from error checking added in
                   reg_sim_read_pplmn_w_act_list and reg_sim_read_pplmn_list.
12/21/04   up      Updated functions reg_sim_add_plmn_to_fplmn_list and 
                   reg_sim_remove_plmn_from_fplmn_list for Logging (create a
                   LOG PACKET) of updation of FPLMN List (add/remove plmn).
02/22/05   up      Updated for Cingular ENS feature.
                   Added variables reg_sim_refresh_file_list, reg_sim_hplmn,
                   reg_sim_ens_flag. Added functions reg_sim_read_ens_flag,
                   reg_sim_force_read_act_hplmn, reg_sim_refresh_complete,
                   reg_sim_force_read_hplmn_search_period, reg_sim_hplmn_get
                   reg_sim_gsdi_generic_cb, reg_sim_refresh_fcn_ind_is_valid,
                   reg_sim_refresh_file_change_notification_cb,
                   reg_sim_force_read, reg_sim_wait_for_async_op_complete_cb.
                   Updated functions reg_sim_load_card_mode, reg_sim_read_hplmn
                   reg_sim_read_hplmn_search_period.
03/01/05   up      ENS Fix - supported flags are replaced with hash defines.
                   Updated reg_sim_refresh_file_change_notification_cb to post
                   SIM_REFRESH_IND. Added func reg_sim_process_refresh_ind.
07/07/05   up      GPRS gcf flag is read in reg_sim_load_card_mode. HPLMN will
                   be added to GPRS forbidden list if the flag is TRUE.
07/15/05   up      Updated to restrict multiple SIM access for AHPLMN and TST.
07/18/05   up      ENS feature/functionality is enabled if ens_enabled flag
                   is set or enabled in NV.
07/18/05   up      Updated reg_sim_read_hplmn to consider AHPLMN of dofferent 
                   MCC than the MCC of True HPLMN.
07/18/05   up      Supporting for two new ENS EFs UIM_USIM_ACT_HPLMN and 
                   UIM_USIM_SPT_TABLE under USIM directory.
08/04/05   up      Updated reg_sim_read_hplmn to set the MNC digit 3 to 'F'
                   if 2 digits MNC is indicated in EF-AD.
01/31/06   up      Accommodate OPLMNs in place of unused OPLMN's space.
02/20/06   sn      EHPLMN Feature
03/01/06   up      Read/Write of SIM RPLMNAcT are kept under feature flag.
03/27/06   sn      Updated read_hplmn_rat_search_order
10/12/06   sn      WCDMA_1X mode preference implementation.
03/24/09   abhi    Sending refresh complete directly if card mode is unknown
08/22/11   rajesh  Adding a message print to print backoff forbidden plmn in function
                   "reg_sim_plmn_backoff_forbidden"
08/25/11   gps     Truncating Number of files and Plmn list length if they are larger
                   than allowed in reg_sim_refresh_event_cb function. 
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include <string.h>
#include <stringl/stringl.h>
#include "msg.h"
#include "reg_nv.h"
#include "reg_sim_v.h"
#include "reg_timers.h"
#include "sys.h"
#include "sys_v.h"
#include "sys_cnst_v.h"
#include "uim.h"
#include "reg_log.h"
#include "ms.h"
#include "reg_task.h"
#include "reg_task_int.h"
#include "reg_mode.h"
#include "reg_send.h"
#include "mmgsdilib_common.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#else
#include "gs_v.h"
#endif
#include "cm.h"
#ifdef FEATURE_REL7_STEERING_OF_ROAMING
#include "mmgsdilib_v.h"
#endif

#ifdef FEATURE_EQUIVALENT_HPLMN
#include "sys_eplmn_list_v.h"
#endif
#include "err.h"
#include "fs_lib.h"
#include "reg_send.h"
#include "sys.h"
#include "policyman.h"
#include "reg_timers.h"


/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

#define REG_SIM_PLMN_W_ACT_LIST_LENGTH  (REG_SIM_PREFERRED_PLMN_LIST_LENGTH)

#ifndef FEATURE_RAT_PRIORITY_LIST
#define REG_SIM_HPLMN_W_ACT_LIST_LENGTH (2)
#else
#define REG_SIM_HPLMN_W_ACT_LIST_LENGTH (10)
#endif
#define REG_SIM_PLMN_LENGTH       (3)

#define REG_SIM_PLMN_W_ACT_LENGTH (REG_SIM_PLMN_LENGTH + REG_SIM_ACT_LENGTH)

#define REG_SIM_LAI_OFFSET        MMGSDI_LAI_OFFSET /*  4 */
#define REG_SIM_LUS_OFFSET        MMGSDI_LUS_OFFSET /* 10 */

#define REG_SIM_RAI_OFFSET        ( 7)
#define REG_SIM_RUS_OFFSET        (13)

#define REG_SIM_LUS_UPDATED        (0)
#define REG_SIM_LUS_NOT_UPDATED    (1)
#define REG_SIM_LUS_LA_NOT_ALLOWED (3)
#define REG_SIM_LUS_RA_NOT_ALLOWED (3)

#define REG_SIM_ADMIN_LENGTH       (4)

#ifdef FEATURE_LTE  
  #define REG_SIM_EPS_UPDATED        (0)
  #define REG_SIM_EPS_NOT_UPDATED    (1)
  #define REG_SIM_EPS_ROAMING_NOT_ALLOWED (2)

  #define REG_SIM_TAI_OFFSET 12
  #define REG_SIM_EPS_OFFSET 17 
#endif /*FEATURE_LTE*/


typedef struct reg_sim_plmn_w_act_list_s
{
  reg_sim_plmn_w_act_s_type  plmn_w_act[REG_SIM_PLMN_W_ACT_LIST_LENGTH];
} reg_sim_plmn_w_act_list_s_type;

/*
**  Default value for HPLMN Search Period is 60 minutes (10 deci-hours)
**  as specified in TS 23.122 Section 4.4.3.3.
*/
#define REG_SIM_DEFAULT_HPLMN_SP  10

typedef enum reg_sim_mode_pref_e
{
  REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED,
  REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED,
  REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED,
  REG_SIM_MODE_PREF_TDSCDMA_RADIO_ACCESS_PREFERRED,
  REG_SIM_MODE_PREF_UNDEFINED
} reg_sim_mode_pref_e_type;


extern nas_crit_sect_type reg_hplmn_crit_sect;
extern nas_crit_sect_type reg_ehplmn_crit_sect;

sys_plmn_id_s_type reg_sim_read_sim_rplmn
(
  sys_srv_domain_e_type req_service_domain
);

/* NASCONFIG structure */
typedef struct reg_sim_nasconfig_s
{
  boolean  nas_sig_low_priority;
  byte     min_periodic_srch_timer;
  boolean  eab_cfg;
  boolean  t3245_behavior;
} reg_sim_nasconfig_s_type;


/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/
#ifdef FEATURE_TDSCDMA
/* This function searches for a specified RAT in the provided list and returns
** its index in the array.
*/
static boolean reg_sim_is_rat_present_3GPP_capability_list(const reg_mode_3gpp_capability_s_type *req_rat_list,
                                                           sys_radio_access_tech_e_type rat, 
                                                           int *position_rat)
{
     int i = 0;
     for(i = 0; i < req_rat_list->num_items; ++i)
      {
          if(req_rat_list->rat_list[i] == rat)
          {              
            if (position_rat != NULL)
            {
               *position_rat = i;
            }
            return TRUE;
          }
      } 
   return FALSE;
}
#endif
/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

#if defined(FEATURE_TRIPLE_SIM) ||defined(FEATURE_SGLTE_DUAL_SIM)/* FEATURE_TRIPLE_SIM */
/*
**  State of the SIM.
*/
static sys_sim_state_e_type reg_sim_state_sim[MAX_NAS_STACKS] = { SYS_SIM_STATE_NOT_AVAILABLE, SYS_SIM_STATE_NOT_AVAILABLE,SYS_SIM_STATE_NOT_AVAILABLE };
#define reg_sim_state reg_sim_state_sim[reg_as_id]

/*
**  Stored value for RPLMNACT for use with a SIM card.
*/
static reg_sim_mode_pref_e_type reg_sim_rplmn_rat_search_order_sim[MAX_NAS_STACKS] = { REG_SIM_MODE_PREF_UNDEFINED, REG_SIM_MODE_PREF_UNDEFINED,REG_SIM_MODE_PREF_UNDEFINED};
#define reg_sim_rplmn_rat_search_order reg_sim_rplmn_rat_search_order_sim[reg_as_id]

/*
**  GPRS Forbidden PLMN List.
*/
reg_sim_plmn_list_s_type reg_sim_gprs_fplmn_list_sim[MAX_NAS_STACKS];
#define reg_sim_gprs_fplmn_list reg_sim_gprs_fplmn_list_sim[reg_sub_id]

/*
** List of EFs for which REG required REFRESH from GSDI when they changed.
*/
mmgsdi_file_enum_type reg_sim_refresh_file_list_sim[MAX_NAS_STACKS][REG_SIM_NUM_REFRESH_FILE] = {
                     {
                        MMGSDI_USIM_HPLMN
#ifdef FEATURE_ENHANCED_NW_SELECTION
                       ,MMGSDI_SIM_7F66_PROP1_ACT_HPLMN
#endif
                       ,MMGSDI_USIM_FPLMN
                       ,MMGSDI_USIM_HPLMNWACT
                       ,MMGSDI_USIM_RPLMNACT
                       ,MMGSDI_USIM_LOCI
                       ,MMGSDI_USIM_PSLOCI
                       ,MMGSDI_USIM_OPLMNWACT                    
                     },
                     {
                        MMGSDI_USIM_HPLMN
#ifdef FEATURE_ENHANCED_NW_SELECTION
                       ,MMGSDI_SIM_7F66_PROP1_ACT_HPLMN
#endif
                       ,MMGSDI_USIM_FPLMN
                       ,MMGSDI_USIM_HPLMNWACT
                       ,MMGSDI_USIM_RPLMNACT
                       ,MMGSDI_USIM_LOCI
                       ,MMGSDI_USIM_PSLOCI
                       ,MMGSDI_USIM_OPLMNWACT
                     },
                     {
                        MMGSDI_USIM_HPLMN
#ifdef FEATURE_ENHANCED_NW_SELECTION
                       ,MMGSDI_SIM_7F66_PROP1_ACT_HPLMN
#endif
                       ,MMGSDI_USIM_FPLMN
                       ,MMGSDI_USIM_HPLMNWACT
                       ,MMGSDI_USIM_RPLMNACT
                       ,MMGSDI_USIM_LOCI
                       ,MMGSDI_USIM_PSLOCI
                       ,MMGSDI_USIM_OPLMNWACT
                     }};
mmgsdi_file_enum_type reg_sim_refresh_fcn_list_sim[MAX_NAS_STACKS][REG_SIM_NUM_REFRESH_FILE];

#define reg_sim_refresh_file_list reg_sim_refresh_file_list_sim[reg_as_id]
/*
** List of files for which REG received REFRESH.
*/

#define reg_sim_refresh_fcn_list reg_sim_refresh_fcn_list_sim[reg_as_id]

uint8               reg_sim_num_fcn_files_sim[MAX_NAS_STACKS] = { 0, 0, 0};
#define reg_sim_num_fcn_files reg_sim_num_fcn_files_sim[reg_as_id]

#ifdef FEATURE_ENHANCED_NW_SELECTION 
/*
** Flag for ENS supporting features, manahed by REG, derived from EF-TST.
** Feature is supported if the bit is set to '1'.
** B1 - Scan Control Application, B2 - Menu Control Application.
** B3 - Acting HPLMN, Others are RFU and set to 0.
*/
byte reg_sim_ens_flag_sim[MAX_NAS_STACKS] = { 0x00, 0x00, 0x00 };
#define reg_sim_ens_flag reg_sim_ens_flag_sim[reg_as_id]
static boolean reg_sim_is_ens_read_sim[MAX_NAS_STACKS] = { FALSE, FALSE, FALSE};
#define reg_sim_is_ens_read reg_sim_is_ens_read_sim[reg_as_id]
static boolean reg_sim_is_ahplmn_read_sim[MAX_NAS_STACKS] = { FALSE, FALSE,FALSE};
#define reg_sim_is_ahplmn_read reg_sim_is_ahplmn_read_sim[reg_as_id]
#endif

/*
** HPLMN is managed by REG, used by MM/RR/RRC.
** REG derived HPLMN from IMSI (True HPLMN) or Acting HPLMN.
*/
sys_plmn_id_s_type reg_sim_hplmn_sim[MAX_NAS_STACKS] ={ {{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}}};
#define reg_sim_hplmn reg_sim_hplmn_sim[reg_as_id]

#ifdef FEATURE_EQUIVALENT_HPLMN
/*
** EHPLMN List
*/
reg_sim_plmn_list_s_type reg_sim_ehplmn_list_sim[MAX_NAS_STACKS];
#define reg_sim_ehplmn_list reg_sim_ehplmn_list_sim[reg_as_id]
#endif

#ifdef FEATURE_BUILTIN_PLMN 
reg_sim_builtin_plmn_list_type *reg_sim_ota_plmn_list_sim[MAX_NAS_STACKS]={NULL}; 
#define reg_sim_ota_plmn_list (reg_sim_ota_plmn_list_sim[reg_as_id])
#endif

reg_sim_preferred_plmn_list_s_type reg_sim_low_priority_plmn_list_sim[MAX_NAS_STACKS];
#define reg_sim_low_priority_plmn_list reg_sim_low_priority_plmn_list_sim[reg_as_id]

#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)reg_as_id)
#endif


static sys_plmn_id_s_type reg_sim_cs_rplmn_sim[MAX_NAS_STACKS] = {{{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}}};
#define reg_sim_cs_rplmn reg_sim_cs_rplmn_sim[reg_as_id]

#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a, (sys_modem_as_id_e_type)reg_sub_id, (sys_modem_as_id_e_type)reg_as_id) 

void  reg_sim_per_subs_refresh_complete
(
  boolean status,
  sys_modem_as_id_e_type as_id
);

#ifdef FEATURE_NAS_ECALL
boolean reg_ecall_profile_sim[MAX_NAS_STACKS];
#define reg_ecall_profile reg_ecall_profile_sim[reg_as_id]
#endif

#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
 /* FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
 */

/*
**  State of the SIM.
*/
static sys_sim_state_e_type reg_sim_state_sim[MAX_NAS_STACKS] = { SYS_SIM_STATE_NOT_AVAILABLE, SYS_SIM_STATE_NOT_AVAILABLE };
#define reg_sim_state reg_sim_state_sim[reg_as_id]

/*
**  Stored value for RPLMNACT for use with a SIM card.
*/
static reg_sim_mode_pref_e_type reg_sim_rplmn_rat_search_order_sim[MAX_NAS_STACKS] = { REG_SIM_MODE_PREF_UNDEFINED, REG_SIM_MODE_PREF_UNDEFINED};
#define reg_sim_rplmn_rat_search_order reg_sim_rplmn_rat_search_order_sim[reg_as_id]

/*
** List of EFs for which REG required REFRESH from GSDI when they changed.
*/
mmgsdi_file_enum_type reg_sim_refresh_file_list_sim[MAX_NAS_STACKS][REG_SIM_NUM_REFRESH_FILE] = {
                     {
                        MMGSDI_USIM_HPLMN
#ifdef FEATURE_ENHANCED_NW_SELECTION
                       ,MMGSDI_SIM_7F66_PROP1_ACT_HPLMN
#endif
                       ,MMGSDI_USIM_FPLMN
                       ,MMGSDI_USIM_HPLMNWACT
                       ,MMGSDI_USIM_RPLMNACT
                       ,MMGSDI_USIM_LOCI
                       ,MMGSDI_USIM_PSLOCI
					   ,MMGSDI_USIM_OPLMNWACT 
                     },
                     {
                        MMGSDI_USIM_HPLMN
#ifdef FEATURE_ENHANCED_NW_SELECTION
                       ,MMGSDI_SIM_7F66_PROP1_ACT_HPLMN
#endif
                       ,MMGSDI_USIM_FPLMN
                       ,MMGSDI_USIM_HPLMNWACT
                       ,MMGSDI_USIM_RPLMNACT
                       ,MMGSDI_USIM_LOCI
                       ,MMGSDI_USIM_PSLOCI
					   ,MMGSDI_USIM_OPLMNWACT
                     }};
mmgsdi_file_enum_type reg_sim_refresh_fcn_list_sim[MAX_NAS_STACKS][REG_SIM_NUM_REFRESH_FILE];

#define reg_sim_refresh_file_list reg_sim_refresh_file_list_sim[reg_as_id]
/*
** List of files for which REG received REFRESH.
*/

#define reg_sim_refresh_fcn_list reg_sim_refresh_fcn_list_sim[reg_as_id]

uint8               reg_sim_num_fcn_files_sim[MAX_NAS_STACKS] = { 0, 0};
#define reg_sim_num_fcn_files reg_sim_num_fcn_files_sim[reg_as_id]

#ifdef FEATURE_ENHANCED_NW_SELECTION 
/*
** Flag for ENS supporting features, manahed by REG, derived from EF-TST.
** Feature is supported if the bit is set to '1'.
** B1 - Scan Control Application, B2 - Menu Control Application.
** B3 - Acting HPLMN, Others are RFU and set to 0.
*/
byte reg_sim_ens_flag_sim[MAX_NAS_STACKS] = { 0x00, 0x00 };
#define reg_sim_ens_flag reg_sim_ens_flag_sim[reg_as_id]
static boolean reg_sim_is_ens_read_sim[MAX_NAS_STACKS] = { FALSE, FALSE};
#define reg_sim_is_ens_read reg_sim_is_ens_read_sim[reg_as_id]
static boolean reg_sim_is_ahplmn_read_sim[MAX_NAS_STACKS] = { FALSE, FALSE};
#define reg_sim_is_ahplmn_read reg_sim_is_ahplmn_read_sim[reg_as_id]
#endif

/*
** HPLMN is managed by REG, used by MM/RR/RRC.
** REG derived HPLMN from IMSI (True HPLMN) or Acting HPLMN.
*/
sys_plmn_id_s_type reg_sim_hplmn_sim[MAX_NAS_STACKS] ={ {{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}}};
#define reg_sim_hplmn reg_sim_hplmn_sim[reg_as_id]

#ifdef FEATURE_EQUIVALENT_HPLMN
/*
** EHPLMN List
*/
reg_sim_plmn_list_s_type reg_sim_ehplmn_list_sim[MAX_NAS_STACKS];
#define reg_sim_ehplmn_list reg_sim_ehplmn_list_sim[reg_as_id]
#endif

reg_sim_preferred_plmn_list_s_type reg_sim_low_priority_plmn_list_sim[MAX_NAS_STACKS];
#define reg_sim_low_priority_plmn_list reg_sim_low_priority_plmn_list_sim[reg_as_id]

/*
**  GPRS Forbidden PLMN List.
*/
reg_sim_plmn_list_s_type reg_sim_gprs_fplmn_list_sim[MAX_NAS_STACKS];
#define reg_sim_gprs_fplmn_list reg_sim_gprs_fplmn_list_sim[reg_sub_id]

static sys_plmn_id_s_type reg_sim_cs_rplmn_sim[MAX_NAS_STACKS] = {{{0xFF, 0xFF, 0xFF}},{{0xFF, 0xFF, 0xFF}}};
#define reg_sim_cs_rplmn reg_sim_cs_rplmn_sim[reg_as_id]

#define sys_eplmn_list_equivalent_plmn(a) sys_eplmn_per_nas_stacks_list_equivalent_plmn(a,(sys_modem_as_id_e_type)reg_sub_id,(sys_modem_as_id_e_type)reg_as_id) 

void  reg_sim_per_subs_refresh_complete
(
  boolean status,
  sys_modem_as_id_e_type as_id
);

#ifdef FEATURE_NAS_ECALL
boolean reg_ecall_profile_sim[MAX_NAS_STACKS];
#define reg_ecall_profile reg_ecall_profile_sim[reg_as_id]
#endif

#else /* FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
*/


/*
**  State of the SIM.
*/
static sys_sim_state_e_type reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;


/*
**  Stored value for RPLMNACT for use with a SIM card.
*/
static reg_sim_mode_pref_e_type reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_UNDEFINED;

/*
**  GPRS Forbidden PLMN List.
*/
reg_sim_plmn_list_s_type reg_sim_gprs_fplmn_list;



/*
** List of EFs for which REG required REFRESH from GSDI when they changed.
*/

mmgsdi_file_enum_type reg_sim_refresh_file_list[REG_SIM_NUM_REFRESH_FILE] = {
                        MMGSDI_USIM_HPLMN
#ifdef FEATURE_ENHANCED_NW_SELECTION
                       ,MMGSDI_SIM_7F66_PROP1_ACT_HPLMN
#endif
                       ,MMGSDI_USIM_FPLMN
                       ,MMGSDI_USIM_HPLMNWACT
                       ,MMGSDI_USIM_RPLMNACT
                       ,MMGSDI_USIM_LOCI
                       ,MMGSDI_USIM_PSLOCI
#ifdef FEATURE_FEMTO_CSG
                       ,MMGSDI_USIM_OCSGL
#endif 
                        ,MMGSDI_USIM_OPLMNWACT
                     } ;
mmgsdi_file_enum_type reg_sim_refresh_fcn_list[REG_SIM_NUM_REFRESH_FILE];


uint8               reg_sim_num_fcn_files = 0;

#ifdef FEATURE_ENHANCED_NW_SELECTION 
/*
** Flag for ENS supporting features, manahed by REG, derived from EF-TST.
** Feature is supported if the bit is set to '1'.
** B1 - Scan Control Application, B2 - Menu Control Application.
** B3 - Acting HPLMN, Others are RFU and set to 0.
*/
byte reg_sim_ens_flag = 0x00;

static boolean reg_sim_is_ens_read    = FALSE;
static boolean reg_sim_is_ahplmn_read = FALSE;
#endif

/*
** HPLMN is managed by REG, used by MM/RR/RRC.
** REG derived HPLMN from IMSI (True HPLMN) or Acting HPLMN.
*/
sys_plmn_id_s_type reg_sim_hplmn = {{0xFF, 0xFF, 0xFF}};

#ifdef FEATURE_EQUIVALENT_HPLMN
/*
** EHPLMN List
*/
reg_sim_plmn_list_s_type reg_sim_ehplmn_list;
#endif

reg_sim_preferred_plmn_list_s_type reg_sim_low_priority_plmn_list;

static sys_plmn_id_s_type reg_sim_cs_rplmn = {{0xFF, 0xFF, 0xFF}};

#ifdef FEATURE_NAS_ECALL
boolean reg_ecall_profile = FALSE;
#endif

#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE 
*/

#ifdef FEATURE_DUAL_SIM

static reg_sim_nasconfig_s_type reg_sim_nasconfig_sim[MAX_NAS_STACKS];
#define reg_sim_nasconfig reg_sim_nasconfig_sim[reg_sub_id]
#else
static reg_sim_nasconfig_s_type reg_sim_nasconfig;
#endif

#ifdef FEATURE_LTE
/*
**  Backoff Forbidden PLMN List.
*/
reg_sim_backoff_fplmn_list_info_s_type reg_sim_backoff_fplmn_list;

// Define the REG BACKOFF PLMN REX critical section variable
rex_crit_sect_type  reg_backoff_fplmn_crit_sect;
#endif


#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_ENHANCED_NW_SELECTION
#define reg_sim_read_ens_flag() reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)reg_as_id)
#endif
#endif

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
#if defined(FEATURE_TRIPLE_SIM)
nas_mmgsdi_info_type reg_sim_mmgsdi_info_sim[MAX_NAS_STACKS] = {{0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                            {0x00,0x00,MMGSDI_GW_PROV_SEC_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                            {0x00,0x00,MMGSDI_GW_PROV_TER_SESSION,MMGSDI_MAX_SLOT_ID_ENUM}};
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
boolean reg_sim_is_sim_busy_sim[MAX_AS_IDS] = {FALSE,FALSE,FALSE};
#define reg_sim_is_sim_busy reg_sim_is_sim_busy_sim[reg_as_id]
#endif
mmgsdi_app_enum_type reg_sim_card_mode_sim[MAX_NAS_STACKS] = {MMGSDI_APP_UNKNOWN,MMGSDI_APP_UNKNOWN, MMGSDI_APP_UNKNOWN};
#elif defined(FEATURE_SGLTE_DUAL_SIM)
nas_mmgsdi_info_type reg_sim_mmgsdi_info_sim[MAX_NAS_STACKS] = {{0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                            {0x00,0x00,MMGSDI_GW_PROV_SEC_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                            {0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM}};
mmgsdi_app_enum_type reg_sim_card_mode_sim[MAX_NAS_STACKS] = {MMGSDI_APP_UNKNOWN,MMGSDI_APP_UNKNOWN, MMGSDI_APP_UNKNOWN};
#else
nas_mmgsdi_info_type reg_sim_mmgsdi_info_sim[MAX_NAS_STACKS] = {{0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM},
                                                            {0x00,0x00,MMGSDI_GW_PROV_SEC_SESSION,MMGSDI_MAX_SLOT_ID_ENUM}};
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
boolean reg_sim_is_sim_busy_sim[MAX_AS_IDS] = {FALSE,FALSE};
#define reg_sim_is_sim_busy reg_sim_is_sim_busy_sim[reg_as_id]
#endif
mmgsdi_app_enum_type reg_sim_card_mode_sim[MAX_NAS_STACKS] = {MMGSDI_APP_UNKNOWN,MMGSDI_APP_UNKNOWN};
#endif
/*
**  SIM card mode (SIM or USIM).
*/

#define reg_sim_mmgsdi_info reg_sim_mmgsdi_info_sim[reg_as_id]
#define reg_sim_card_mode reg_sim_card_mode_sim[reg_as_id]

extern boolean reg_waiting_for_open_session_cnf_sim[MAX_NAS_STACKS];
#define reg_waiting_for_open_session_cnf reg_waiting_for_open_session_cnf_sim[reg_as_id]

#else
nas_mmgsdi_info_type reg_sim_mmgsdi_info = {0x00,0x00,MMGSDI_GW_PROV_PRI_SESSION,MMGSDI_MAX_SLOT_ID_ENUM};
mmgsdi_app_enum_type reg_sim_card_mode = MMGSDI_APP_UNKNOWN;

extern boolean reg_waiting_for_open_session_cnf;
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
boolean reg_sim_is_sim_busy = FALSE;
#endif
#endif

#if defined(FEATURE_FEMTO_CSG) || defined (FEATURE_NAS_ECALL)
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
boolean reg_sim_rplmn_read_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_sim_rplmn_read  reg_sim_rplmn_read_sim[reg_as_id]

#ifdef FEATURE_FEMTO_CSG
boolean reg_sim_mm_csg_list_read_complete_sim[MAX_NAS_STACKS] = {FALSE, FALSE};
#define reg_sim_mm_csg_list_read_complete reg_sim_mm_csg_list_read_complete_sim[reg_as_id]
#endif

#else
/*
** RPLMN will be read from SIM only once after SIM avaialble
** This flag is set on receiving SIM avaialble and reset after
** reading RPLMN from SIM
*/
boolean reg_sim_rplmn_read = FALSE;
boolean reg_sim_mm_csg_list_read_complete = FALSE;
#endif 
#endif 

#if defined(FEATURE_SGLTE) || defined (FEATURE_DUAL_SIM)
uint8           reg_sim_max_subscription = 1;
#endif 

static boolean reg_nv_extended_fplmn_icc = FALSE;

boolean mm_sim_open_session
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_client_id_type client_id
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
);

#ifdef FEATURE_DUAL_SIM
byte reg_sim_per_nas_stacks_read_ens_flag
(
  sys_modem_as_id_e_type stack_id_nas
);
#endif

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
reg_as_id_e_type reg_sim_get_as_id
(
  mmgsdi_session_id_type session_id
)
{
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_1].session_id == session_id)
  {
    as_id = (sys_modem_as_id_e_type)REG_AS_ID_1;
  }
  else if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_2].session_id == session_id)
  {
    as_id = (sys_modem_as_id_e_type)REG_AS_ID_2;
  }
#ifdef FEATURE_TRIPLE_SIM
  else if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_3].session_id == session_id)
  {
    as_id = (sys_modem_as_id_e_type)REG_AS_ID_3;
  }
#endif  
  else
  {
    MSG_FATAL_DS(REG_SUB, "=REG= Error: session ID not mapped to any AS ID", 0, 0, 0);
  }
  return as_id;
}
#endif


/*==============================================================================

FUNCTION NAME

  reg_sim_get_as_id_from_slot

DESCRIPTION

  Function maps the as_id from the slot id passed as parameter

RETURN VALUE

  sys_modem_as_id_e_type - return as_id.

==============================================================================*/
sys_modem_as_id_e_type reg_sim_get_as_id_from_slot
(
  mmgsdi_slot_id_enum_type  slot_id
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_NONE;
  if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_1].slot_id == slot_id)
  {
    as_id = SYS_MODEM_AS_ID_1;
  }
  else if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_2].slot_id == slot_id)
  {
    as_id = SYS_MODEM_AS_ID_2;
  }
#ifdef FEATURE_TRIPLE_SIM
  else if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_3].slot_id == slot_id)
  {
    as_id = SYS_MODEM_AS_ID_3;
  }
#endif  
  else
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Slot ID not mapped to any AS ID");
  }
  return as_id;
#else
  return SYS_MODEM_AS_ID_1;
#endif
}


/*==============================================================================

FUNCTION NAME

  reg_sim_populate_sim_refresh_file_list

DESCRIPTION

  Function that populates EFs into reg_sim_refresh_file_list

RETURN VALUE

  unsigned char  Returns the number of EFs populated.

==============================================================================*/

void reg_sim_populate_sim_refresh_file_list
(
  mmgsdi_refresh_file_list_type *refresh_files
)
{
  uint32 num_files = 0;
  
  switch(reg_sim_card_mode)
  {
    case MMGSDI_APP_SIM:
      refresh_files->file_list_ptr[num_files++] = MMGSDI_GSM_HPLMN;
#ifdef FEATURE_ENHANCED_NW_SELECTION
      /*
      ** ENS functionality shall be enabled if the NV item is set.
      ** If ENS is not set in NV, make the ens_flag to 0x00 so that all
      ** ENS functionality shall be disabled.
      */
      if (reg_nv_read_ens_enabled_flag() != FALSE)
      {
        refresh_files->file_list_ptr[num_files++] = MMGSDI_SIM_7F66_PROP1_ACT_HPLMN;
      }
      else
      {
        reg_sim_is_ens_read = TRUE;
        reg_sim_ens_flag    = 0x00;
      }
#endif
      refresh_files->file_list_ptr[num_files++] = MMGSDI_GSM_FPLMN;
      refresh_files->file_list_ptr[num_files++] = MMGSDI_GSM_HPLMNACT;  
      refresh_files->file_list_ptr[num_files++] = MMGSDI_GSM_OPLMNWACT;   
      break;

    case MMGSDI_APP_USIM:
      refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_HPLMN;
#ifdef FEATURE_ENHANCED_NW_SELECTION
      /*
      ** ENS functionality shall be enabled if the NV item is set.
      ** If ENS is not set in NV, make the ens_flag to 0x00 so that all
      ** ENS functionality shall be disabled.
      */
      if (reg_nv_read_ens_enabled_flag() != FALSE)
      {
        refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_7F66_PROP1_ACT_HPLMN;
      }
      else
      {
        reg_sim_is_ens_read = TRUE;
        reg_sim_ens_flag    = 0x00;
      }
#endif
      refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_FPLMN;
      refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_HPLMNWACT;
      refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_RPLMNACT;
      refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_LOCI;
      refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_PSLOCI;
#ifdef FEATURE_FEMTO_CSG
      if(reg_nv_read_csg_support() > SYS_CSG_SUPPORT_DISABLED)
      {
        refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_OCSGL;
      }
#endif 
      refresh_files->file_list_ptr[num_files++] = MMGSDI_USIM_OPLMNWACT;      
      break;
      
    default:
      MSG_HIGH_DS_0(REG_SUB, "=REG= Unknown SIM card mode, populating no files for FCN.");
  }

  refresh_files->num_files = num_files;
}

/*==============================================================================

FUNCTION NAME

  reg_sim_read

DESCRIPTION

  Function that reads a specified Elementary File (EF) from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the specified EF was read from the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

==============================================================================*/

static boolean reg_sim_read
(
  mmgsdi_file_enum_type sim_filename,
  byte*                 data_buffer_p,
  int32                 data_length
)
{
  mmgsdi_return_enum_type status = MMGSDI_ERROR;
  mmgsdi_data_type read_data;

  read_data.data_ptr = data_buffer_p;
  read_data.data_len = data_length;

  status = mmgsdi_session_read_cache
  (
    reg_sim_mmgsdi_info.session_id,
    sim_filename,
    read_data
  );

  reg_timers_pet_watchdog();

  return (boolean)(status == MMGSDI_SUCCESS);
}

#ifdef FEATURE_DUAL_SIM
static boolean reg_sim_per_subs_read
(
  mmgsdi_file_enum_type sim_filename,
  byte*                 data_buffer_p,
  int32                 data_length,
  sys_modem_as_id_e_type as_id
)
{
  mmgsdi_return_enum_type status = MMGSDI_ERROR;
  mmgsdi_data_type read_data;

  read_data.data_ptr = data_buffer_p;
  read_data.data_len = data_length;

  status = mmgsdi_session_read_cache
  (
    reg_sim_mmgsdi_info_sim[as_id].session_id,
    sim_filename,
    read_data
  );

  reg_timers_pet_watchdog();

  return (boolean)(status == MMGSDI_SUCCESS);
}
#endif


/*==============================================================================

FUNCTION NAME

  reg_sim_wait_for_async_op_complete_cb

==============================================================================*/

static void reg_sim_wait_for_async_op_complete_cb
(
    mmgsdi_return_enum_type  status,
    mmgsdi_cnf_enum_type       cnf,
    const mmgsdi_cnf_type      *cnf_ptr
)
{
  reg_cmd_type* reg_cmd_p;

#if defined(FEATURE_TRIPLE_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
  static uint8  num_file_read[MAX_NAS_STACKS] = {0,0,0};
  static boolean read_status[MAX_NAS_STACKS] = {TRUE,TRUE,TRUE};
#elif defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  static uint8  num_file_read[MAX_NAS_STACKS] = {0,0};
  static boolean read_status[MAX_NAS_STACKS] = {TRUE,TRUE};
#else
  static uint8  num_file_read = 0;
  static boolean read_status = TRUE;
#endif
#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM)
  reg_as_id_e_type as_id = REG_AS_ID_1;
  reg_as_id_e_type sub_id = REG_AS_ID_1;

  reg_as_id_e_type local_as_id;
  byte no_of_stacks = 1;
  byte index = 0;
#endif
  
  if (cnf_ptr != NULL)
  {
#if defined (FEATURE_DUAL_SIM ) || defined(FEATURE_SGLTE)
#ifdef FEATURE_DUAL_SIM
    as_id = reg_sim_get_as_id(cnf_ptr->response_header.session_id);
    sub_id = reg_sub_id_stack[as_id];
#else
    as_id = reg_ps_stack_id;
#endif 
     local_as_id = as_id;
#ifdef FEATURE_SGLTE
    if(REG_SUB_IS_SGLTE_SUB(sub_id))
    {
      no_of_stacks = REG_SGLTE_DUAL_STACK_SUB(sub_id) + 1;
    }
#endif 
    if(reg_sim_num_fcn_files_sim[as_id])
    {
      num_file_read[as_id]++;
      if (status != MMGSDI_SUCCESS)
      {
        MSG_ERROR_DS_1( REG_LOCAL_SUB, "=REG= ASYNC File read failed", cnf_ptr->response_header.client_data);
        read_status[as_id] = FALSE;
      }
#else
    if(reg_sim_num_fcn_files)
    {
      num_file_read++;
      if (status != MMGSDI_SUCCESS)
      {
        MSG_ERROR_DS_1( REG_LOCAL_SUB, "=REG= ASYNC File read failed", cnf_ptr->response_header.client_data);
        read_status = FALSE;
      }
#endif /* FEATURE_DUAL_SIM  || FEATURE_SGLTE */
      else
      {
        MSG_HIGH_DS_1( REG_LOCAL_SUB, "=REG= ASYNC file %d read success", cnf_ptr->response_header.client_data);
        /*
        ** Get a command buffer for the REG task.
        */
#ifdef FEATURE_SGLTE
       do
#endif 
       {
        if ((reg_cmd_p = reg_get_cmd_buf()) != NULL)
        {
          /*
          ** Construct the REFRESH FCN IND command.
          */
          reg_cmd_p->cmd.sim_refresh_fcn_ind.message_header.message_set = MS_GSDI;
          reg_cmd_p->cmd.sim_refresh_fcn_ind.message_header.message_id  = SIM_REFRESH_FCN_IND;
          reg_cmd_p->cmd.sim_refresh_fcn_ind.message_header.message_len_lsb =
                     (sizeof(sim_refresh_fcn_ind_s_type) - sizeof(IMH_T)) % 256;
          reg_cmd_p->cmd.sim_refresh_fcn_ind.message_header.message_len_msb =
                     (sizeof(sim_refresh_fcn_ind_s_type) - sizeof(IMH_T)) / 256;
          reg_cmd_p->cmd.sim_refresh_fcn_ind.file_id  = ((mmgsdi_file_enum_type)cnf_ptr->response_header.client_data);

#if defined(FEATURE_SGLTE) || defined(FEATURE_DUAL_SIM)
            reg_cmd_p->cmd.sim_refresh_fcn_ind.as_id = (sys_modem_as_id_e_type)local_as_id;
#endif
          /*
          ** Put on REG command queue
          */
          reg_put_cmd(reg_cmd_p);  
        }
        else
        {
          MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate REG Buffer", 0, 0, 0);
        }
#if defined(FEATURE_SGLTE)
        index ++;
        local_as_id = REG_SGLTE_STACK_2;
#endif 
       }
#ifdef FEATURE_SGLTE
       while(index < no_of_stacks);
#endif 
      }
#if defined ( FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)

      if (reg_sim_num_fcn_files_sim[as_id] == num_file_read[as_id])
      {
        reg_sim_per_subs_refresh_complete
        (
          read_status[as_id],
          (sys_modem_as_id_e_type)as_id
        );
        reg_sim_num_fcn_files_sim[as_id] = 0;
        num_file_read[as_id] = 0;
        read_status[as_id] = TRUE;
      }
#else
      if (reg_sim_num_fcn_files == num_file_read)
      {
        reg_sim_refresh_complete
        (
          read_status
        );
        reg_sim_num_fcn_files = 0;
        num_file_read = 0;
        read_status = TRUE;
      }
#endif
    }
    else
    {
      MSG_ERROR_DS_0( REG_LOCAL_SUB, "=REG= Refresh list NULL");
    }
  }
  else
  {
    MSG_ERROR_DS_0( REG_LOCAL_SUB, "=REG= Invalid ASYNC OP COMPLETE CB");
  }
}

/*==============================================================================

FUNCTION NAME

  reg_sim_force_read

==============================================================================*/
static boolean reg_sim_force_read
(
  mmgsdi_file_enum_type sim_filename,
  int32               data_length
)
{
  mmgsdi_return_enum_type status = MMGSDI_ERROR;
  mmgsdi_client_data_type  client_ref = (mmgsdi_client_data_type) sim_filename;

  mmgsdi_access_type      file_access;
  /*
  ** Verify the SIM card mode before attempting to read data.
  */
  if ( ( reg_sim_card_mode != MMGSDI_APP_SIM ) &&
       ( reg_sim_card_mode != MMGSDI_APP_USIM ) )
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return FALSE;
  }

  file_access.access_method = MMGSDI_EF_ENUM_ACCESS;
  file_access.file.file_enum = sim_filename;
#ifdef FEATURE_SGLTE
if((!REG_SUB_IS_SGLTE)||
   (reg_as_id  == reg_ps_stack_id ))
#endif
{
  status = mmgsdi_session_read_transparent 
  (
    reg_sim_mmgsdi_info.session_id,    /*session id*/
    file_access,     /* file_access         */
    (mmgsdi_offset_type)0,                /* offset          */    
    (mmgsdi_len_type)data_length,      /* req_len    */
    reg_sim_wait_for_async_op_complete_cb,  /* callback */
    client_ref               /* client ref           */
  );

  reg_timers_pet_watchdog();
}
#ifdef FEATURE_SGLTE
if((REG_SUB_IS_SGLTE) &&
   (reg_as_id  != reg_ps_stack_id))
{
  status = MMGSDI_SUCCESS;
}
#endif
  return (boolean)(status == MMGSDI_SUCCESS);
}

/*==============================================================================

FUNCTION NAME

  reg_sim_write

DESCRIPTION

  Function that writes a specified Elementary File (EF) to the SIM.

RETURN VALUE

  boolean  Flag indicating whether the specified EF was written to the SIM
           successfully (TRUE) or unsuccessfully (FALSE).

==============================================================================*/

static boolean reg_sim_write
(
  mmgsdi_file_enum_type sim_filename,
  byte*                 data_buffer_p,
  int32                 data_length
)
{
  mmgsdi_return_enum_type status = MMGSDI_ERROR;
  mmgsdi_data_type write_data;

  write_data.data_ptr = data_buffer_p;
  write_data.data_len = data_length;

  status = mmgsdi_session_write_cache
  (
    reg_sim_mmgsdi_info.session_id,
    sim_filename,
    write_data
  );

  reg_timers_pet_watchdog();

  return (boolean)(status == MMGSDI_SUCCESS);
}

#ifdef FEATURE_DUAL_SIM
static boolean reg_sim_per_subs_write
(
  mmgsdi_file_enum_type sim_filename,
  byte*                 data_buffer_p,
  int32                 data_length,
  sys_modem_as_id_e_type as_id
)
{
  mmgsdi_return_enum_type status = MMGSDI_ERROR;
  mmgsdi_data_type write_data;

  write_data.data_ptr = data_buffer_p;
  write_data.data_len = data_length;

  status = mmgsdi_session_write_cache
  (
    reg_sim_mmgsdi_info_sim[as_id].session_id,
    sim_filename,
    write_data
  );

  reg_timers_pet_watchdog();

  return (boolean)(status == MMGSDI_SUCCESS);
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_sim_read_efnas_config

DESCRIPTION

  Function that reads nas configuration file from sim

RETURN VALUE
  Returns TRUE if successful, FALSE otherwise

==============================================================================*/

boolean reg_sim_read_efnas_config(void)
{
  int32 file_size = 0;
  byte  *ef_nasconfig_p = NULL;
  boolean result = FALSE;

  if (reg_sim_card_mode == MMGSDI_APP_USIM )
  {
    /*
    ** Read the attributes for the EFNASCONFIG.
    */
    if(mmgsdi_session_read_cache_file_size 
        ( reg_sim_mmgsdi_info.session_id,
          MMGSDI_USIM_NASCONFIG,&file_size) == MMGSDI_SUCCESS)
    {
       //Make code changes to limit the file size to some max defined value.
#ifdef FEATURE_MODEM_HEAP
      ef_nasconfig_p = (byte *)modem_mem_alloc((unsigned int)file_size, MODEM_MEM_CLIENT_NAS);
#else
      ef_nasconfig_p = (byte *)gs_alloc(file_size);
#endif
      if(ef_nasconfig_p == NULL)
      {
        MSG_ERROR_DS_0(REG_SUB, "=REG= Could not allocate the memory");    
        return FALSE;
      }
      /*
      ** Read the efnasconfig file
      */
      if(reg_sim_read( MMGSDI_USIM_NASCONFIG,
                      (byte *)ef_nasconfig_p,
                      file_size) == TRUE)
      {
        reg_mode_parse_nas_config(ef_nasconfig_p, file_size);
        result = TRUE;
      }
#ifdef FEATURE_MODEM_HEAP
          modem_mem_free(ef_nasconfig_p, MODEM_MEM_CLIENT_NAS);
#else
          gs_free(ef_nasconfig_p);
#endif
          ef_nasconfig_p = NULL;
    }
    else
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= MMGSDI_USIM_NASCONFIG file size read failed");
      result = FALSE;
    }
  }
  else
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= sim is not usim");
    result = FALSE;
  }
  return result;
}


/*==============================================================================
FUNCTION NAME
  reg_sim_set_low_pri_cfg

DESCRIPTION
  Function sets NAS signalling low priority config read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
void reg_sim_set_low_pri_cfg
(
  boolean nas_sig_low_priority
)
{
  reg_sim_nasconfig.nas_sig_low_priority = nas_sig_low_priority;
}


/*==============================================================================
FUNCTION NAME
  reg_sim_per_subs_chk_low_pri_cfg

DESCRIPTION
  Function returns NAS signalling low priority config status read from NAS configuration file

RETURN VALUE
  TRUE or FALSE
==============================================================================*/
boolean reg_sim_per_subs_chk_low_pri_cfg
(
  sys_modem_as_id_e_type sub_id
)
{
#ifdef FEATURE_DUAL_SIM
  return reg_sim_nasconfig_sim[sub_id].nas_sig_low_priority;
#else
  (void)sub_id;
  return reg_sim_nasconfig.nas_sig_low_priority;
#endif
}


/*==============================================================================
FUNCTION NAME
  reg_sim_set_min_periodic_search_timer_value

DESCRIPTION
  Function sets Minimum periodic search timer value read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
void reg_sim_set_min_periodic_search_timer_value
(
  byte min_periodic_search_timer
)
{
  reg_sim_nasconfig.min_periodic_srch_timer = min_periodic_search_timer;
  }

/*==============================================================================
FUNCTION NAME
  reg_sim_per_subs_get_min_periodic_search_timer_value

DESCRIPTION
  Function gets Minimum periodic search timer value read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
byte reg_sim_per_subs_get_min_periodic_search_timer_value
(
  sys_modem_as_id_e_type sub_id
)
{
#ifdef FEATURE_DUAL_SIM
  return reg_sim_nasconfig_sim[sub_id].min_periodic_srch_timer;
#else
  (void)sub_id;
  return reg_sim_nasconfig.min_periodic_srch_timer;
#endif
}

/*==============================================================================
FUNCTION NAME
  reg_sim_set_eab_cfg

DESCRIPTION
  Function sets Extended access barring value read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
void reg_sim_set_eab_cfg
(
  boolean eab_cfg
)
{
  reg_sim_nasconfig.eab_cfg = eab_cfg;
}


/*==============================================================================
FUNCTION NAME
  reg_sim_per_subs_chk_eab_cfg

DESCRIPTION
  Function returns Extended access barring config status read from NAS configuration file

RETURN VALUE
  TRUE or FALSE
==============================================================================*/
boolean reg_sim_per_subs_chk_eab_cfg
(
  sys_modem_as_id_e_type sub_id
)
{
#ifdef FEATURE_DUAL_SIM
  return reg_sim_nasconfig_sim[sub_id].eab_cfg;
#else
  (void)sub_id;
  return reg_sim_nasconfig.eab_cfg;
#endif
}

/*==============================================================================
FUNCTION NAME
  reg_sim_set_t3245_behavior_config

DESCRIPTION
  Function sets T3245 behavior config read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
void reg_sim_set_t3245_behavior_config
(
  boolean t3245_behavior
)
{
  reg_sim_nasconfig.t3245_behavior = t3245_behavior;
}

/*==============================================================================

FUNCTION NAME

  reg_sim_check_if_t3245_behavior_enabled.

DESCRIPTION

  Function checks if T3245 behavior is enabled in NAS configuration file

RETURN VALUE


==============================================================================*/

boolean reg_sim_check_if_t3245_behavior_enabled
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#endif
)
{
#ifdef FEATURE_DUAL_SIM
  return reg_sim_nasconfig_sim[as_id].t3245_behavior;
#else
  return reg_sim_nasconfig.t3245_behavior;
#endif
}


/*==============================================================================

FUNCTION NAME

  reg_sim_read_pplmn_list

DESCRIPTION

  Function that reads the Preferred PLMN List from the SIM and appends it to
  the output Preferred PLMN List.

RETURN VALUE

  boolean  Flag indicating whether the Preferred PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

==============================================================================*/

static boolean reg_sim_read_pplmn_list
(
  reg_sim_plmn_w_act_s_type* preferred_plmn_list_p,
  int32 preferred_plmn_length
)
{
  sys_plmn_id_s_type pplmn_list[REG_SIM_PREFERRED_PLMN_LIST_LENGTH];
  boolean status = FALSE;
  int32 index;

  if(preferred_plmn_list_p != NULL)
  {
    memset(pplmn_list,0,sizeof(sys_plmn_id_s_type)*REG_SIM_PREFERRED_PLMN_LIST_LENGTH);
  
    /*
    ** Read the Preferred PLMN List.
    */
    if(preferred_plmn_length != 0)
    {
      status = reg_sim_read
      (
        MMGSDI_GSM_PLMN,
        (byte *)pplmn_list,
        preferred_plmn_length*REG_SIM_PLMN_LENGTH
      );
    }
  
    if(status == TRUE)
    {
      for (index = 0; index < preferred_plmn_length; index++)
      {
        preferred_plmn_list_p[index].plmn = pplmn_list[index];
        preferred_plmn_list_p[index].act[0] = 0;
        preferred_plmn_list_p[index].act[1] = 0;
      }
    }
  }

  return status;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_write_fplmn_list

DESCRIPTION

  Function that writes the forbidden list to the SIM. 

RETURN VALUE

  boolean  Flag indicating whether the Forbidden PLMN List was written to the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

==============================================================================*/

static boolean reg_sim_write_fplmn_list
(
  reg_sim_plmn_list_s_type* fplmn_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type   as_id
#endif
)
{
  boolean status;

  /*
  ** Verify the SIM card mode before attempting to write data.
  */
#ifdef FEATURE_DUAL_SIM
  if ( ( reg_sim_card_mode_sim[as_id] != MMGSDI_APP_SIM ) &&
       ( reg_sim_card_mode_sim[as_id] != MMGSDI_APP_USIM ) )
#else
  if ( ( reg_sim_card_mode != MMGSDI_APP_SIM ) &&
       ( reg_sim_card_mode != MMGSDI_APP_USIM ) )
#endif
  {
    MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return FALSE;
  }

  /*
  ** Write the Forbidden PLMN List.
  */
#ifdef FEATURE_DUAL_SIM
  status = reg_sim_per_subs_write
  (
    ( reg_sim_card_mode_sim[as_id] == MMGSDI_APP_SIM ? MMGSDI_GSM_FPLMN : MMGSDI_USIM_FPLMN),
    (byte*)fplmn_list_p->plmn, 
    (int32)(fplmn_list_p->length * REG_SIM_PLMN_LENGTH),
    as_id
  );
#else
  status = reg_sim_write
  (
    ( reg_sim_card_mode == MMGSDI_APP_SIM ? MMGSDI_GSM_FPLMN : MMGSDI_USIM_FPLMN),
    (byte*)fplmn_list_p->plmn, 
    (int32)(fplmn_list_p->length * REG_SIM_PLMN_LENGTH)
  );
#endif

  if ( !status )
  {
    MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= Write FPLMN list failed");
  }

  return ( status );
}


/*==============================================================================

FUNCTION NAME

  reg_sim_combined_rat_search_order

DESCRIPTION

  Function that returns the RAT search order based on the RAT search order
  stored on the SIM (RPLMNAcT or HPLMNWAcT) and the user requested mode
  preference.

RETURN VALUE

  sys_mode_pref_e_type  The combined RAT search order.

==============================================================================*/
#ifndef FEATURE_RAT_PRIORITY_LIST
sys_mode_pref_e_type reg_sim_combined_rat_search_order
(
  reg_sim_mode_pref_e_type sim_rat_search_order,
  sys_mode_pref_e_type     req_mode_pref
)
{
  sys_mode_pref_e_type combined_rat_search_order = req_mode_pref;

  switch (req_mode_pref)
  {
    case SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
    case SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
      switch (sim_rat_search_order)
      {
        case REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
          combined_rat_search_order = SYS_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
          break;

        case REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
          combined_rat_search_order = SYS_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED;
          break;

        case REG_SIM_MODE_PREF_UNDEFINED:
        default:
          combined_rat_search_order = req_mode_pref;
          break;
      }
      break;

    case SYS_MODE_PREF_GSM_RADIO_ACCESS_ONLY:
    case SYS_MODE_PREF_UMTS_RADIO_ACCESS_ONLY:
#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
    #error code not present
#endif
    default:
      combined_rat_search_order = req_mode_pref;
      break;
  }

  return combined_rat_search_order;
}
#endif /*ifndef FEATURE_RAT_PRIORITY_LIST*/

#ifdef FEATURE_RAT_PRIORITY_LIST
void reg_sim_combined_rat_search_order
(
  reg_sim_mode_pref_e_type sim_rat_search_order,
  reg_mode_3gpp_capability_s_type *req_rat_list
)
{
  uint8 i,j;
#ifdef FEATURE_TDSCDMA
  int position_TDS = -1;
  int position_WCDMA = -1;
  nv_rplmnact_type rplmnact;
#endif
  sys_radio_access_tech_e_type pref_rat = SYS_RAT_NONE;
  switch(sim_rat_search_order)
  {
     case REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
        pref_rat = SYS_RAT_GSM_RADIO_ACCESS;
        break;
  
     case REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
       pref_rat = SYS_RAT_UMTS_RADIO_ACCESS;
       break;

     case REG_SIM_MODE_PREF_TDSCDMA_RADIO_ACCESS_PREFERRED:
       pref_rat = SYS_RAT_TDS_RADIO_ACCESS;
       break;
#ifdef FEATURE_LTE 
     case REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED:
        pref_rat = SYS_RAT_LTE_RADIO_ACCESS;
        break;
#endif /* FEATURE_LTE */  
     default:
       return;
  }
   
  for(i = 0; i < req_rat_list->num_items; i++)
  {
     if(pref_rat == req_rat_list->rat_list[i])
     {
        for(j = i; j > 0; j--)
        {
           req_rat_list->rat_list[j] = req_rat_list->rat_list[j-1];
        }
        req_rat_list->rat_list[0] = pref_rat;
        break;
     }
  
  }
#ifdef FEATURE_TDSCDMA
  /*
  ** As an optimization, for the cases where the preferred rat is other
  ** than WCDMA or TDSCDMA, reshuffle the WCDMA and TDSCDMA in the rat list 
  ** based on the value of the TDSCDMA bit in the NV_RPLMNAcT.  
  ** That bit, if set, indicates that there is a TDSCDMA coverage, in which 
  ** case TDSCDMA rat should be given a higher priority than WCDMA.
  */

  if ((pref_rat != SYS_RAT_TDS_RADIO_ACCESS) && (pref_rat != SYS_RAT_UMTS_RADIO_ACCESS))
  {
      /* Determine if both WCDMA and TDSCDMA are present in the rat list */
      reg_sim_is_rat_present_3GPP_capability_list(req_rat_list, 
                SYS_RAT_TDS_RADIO_ACCESS, &position_TDS);


      reg_sim_is_rat_present_3GPP_capability_list(req_rat_list, 
                SYS_RAT_UMTS_RADIO_ACCESS, &position_WCDMA);

      if ((position_TDS != -1) && (position_WCDMA != -1))
      {
          /* Both TDSCDMA and WCDMA are present in the capability list.
          ** Read the value of the TDSCDMA bit in the NV_RPLMNAcT 
          */
          reg_nv_read_rplmnact(&rplmnact);

          if ( rplmnact.act[0] & TDSCDMA_ACT_BIT )
          {
             /* There is a TDSCDMA coverage, give TD priority over WCDMA
             ** if necessary
             */
             if( position_TDS > position_WCDMA)
             {
                req_rat_list->rat_list[position_WCDMA] = SYS_RAT_TDS_RADIO_ACCESS;
                req_rat_list->rat_list[position_TDS] = SYS_RAT_UMTS_RADIO_ACCESS; 
             }
          }
          else
          {
             /* No TDSCDMA coverage, give WCDMA priority over TDS
             ** if necessary
             */
             if( position_TDS < position_WCDMA)
             {
                req_rat_list->rat_list[position_TDS] = SYS_RAT_UMTS_RADIO_ACCESS; 
                req_rat_list->rat_list[position_WCDMA] = SYS_RAT_TDS_RADIO_ACCESS;
             }
          }   
      }
  }
#endif    
}
#endif /*FEATURE_RAT_PRIORITY_LIST*/
/*==============================================================================

FUNCTION NAME

  reg_sim_print_gprs_forbidden_plmn_list

DESCRIPTION

  Function that prints the GPRS forbidden list for debugging purposes.

RETURN VALUE

  None

==============================================================================*/

static void reg_sim_print_gprs_forbidden_plmn_list
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
)
{
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  uint32  i;

#ifdef FEATURE_DUAL_SIM
  MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= GPRS Forbidden PLMN list (length = %d)",
           reg_sim_gprs_fplmn_list_sim[as_id].length);

  if (reg_sim_gprs_fplmn_list_sim[as_id].length > 0)
  {
    MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG=  # MCC-MNC");

    for (i = 0; i < reg_sim_gprs_fplmn_list_sim[as_id].length; i++)
    {
      sys_plmn_get_mcc_mnc
      (
        reg_sim_gprs_fplmn_list_sim[as_id].plmn[i],
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );

      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(REG_LOCAL_SUB, "=REG= %2lu %03lu-%03lu", i, mcc, mnc);
      }
      else
      {
        MSG_HIGH_DS_3(REG_LOCAL_SUB, "=REG= %2lu %03lu- %02lu", i, mcc, mnc);
      }
    }
  }

#else
  MSG_HIGH_DS_1(REG_SUB, "=REG= GPRS Forbidden PLMN list (length = %d)", reg_sim_gprs_fplmn_list.length);

  if (reg_sim_gprs_fplmn_list.length > 0)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG=  # MCC-MNC");

    for (i = 0; i < reg_sim_gprs_fplmn_list.length; i++)
    {
      sys_plmn_get_mcc_mnc
      (
        reg_sim_gprs_fplmn_list.plmn[i],
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu", i, mcc, mnc);
      }
      else
      {
        MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu", i, mcc, mnc);
      }

    }
  }
#endif
}

#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_sim_print_backoff_fplmn_list

DESCRIPTION

  Function that prints the GPRS forbidden list for debugging purposes.

RETURN VALUE

  None

==============================================================================*/

static void reg_sim_print_backoff_fplmn_list
(
  void
)
{
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  uint32  i;

  MSG_HIGH_DS_1(REG_SUB, "=REG= BACKOFF PLMN LIST (length = %d)", reg_sim_backoff_fplmn_list.length);

  if (reg_sim_backoff_fplmn_list.length > 0)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG=  MCC-MNC     REMAINING TIME   CAUSE OF ADDITION");

    for (i = 0; i < reg_sim_backoff_fplmn_list.length; i++)
    {
      sys_plmn_get_mcc_mnc
      (
        reg_sim_backoff_fplmn_list.plmn_timer_list[i].plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
      if(mnc_includes_pcs_digit)
      {
        if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count == 
                                                 REG_TIMERS_MAX_TIMER_VALUE)
        {
          if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                          SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE)
          {
            MSG_HIGH_DS_2(REG_SUB, "=REG=  %03lu-%03lu     Permanently   Added for Max registration failure", mcc, mnc);
          }
          else if (reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                          SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE)
          {
            MSG_HIGH_DS_2(REG_SUB, "=REG=  %03lu-%03lu     Permanently   Added as voice is not available", mcc, mnc);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                          SYS_BLOCK_PLMN_CAUSE_OTHERS)
          {
            MSG_HIGH_DS_2(REG_SUB, "=REG=  %03lu-%03lu     Permanently   Added for other causes", mcc, mnc);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                          SYS_BLOCK_PLMN_CAUSE_PRIORITY )
          {
            MSG_HIGH_DS_2(REG_SUB, "=REG=  %03lu-%03lu     Permanently   Added for DS blocking Priority causes", mcc, mnc);
          }
        }
        else
        {
          if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                            SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE)
          {
            MSG_HIGH_DS_3(REG_SUB, "=REG=  %03lu-%03lu     %10lu       Added for Max registration failure", mcc, mnc,
                 reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                            SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE)
          {
            MSG_HIGH_DS_3(REG_SUB, "=REG=  %03lu-%03lu     %10lu       Added as voice is not available", mcc, mnc,
                 reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                          SYS_BLOCK_PLMN_CAUSE_OTHERS)
          {
            MSG_HIGH_DS_3(REG_SUB, "=REG=  %03lu-%03lu     %10lu       Added for other causes", mcc, mnc,
                 reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                          SYS_BLOCK_PLMN_CAUSE_PRIORITY )
          {
            MSG_HIGH_DS_3(REG_SUB, "=REG=  %03lu-%03lu     %10lu    Added for DS Blocking Priority causes", mcc, mnc,
                 reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count);
          }
        }
       
      }
      else
      {
        if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count == 
                                                 REG_TIMERS_MAX_TIMER_VALUE)
        {
          if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                          SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE)
          {
            MSG_HIGH_DS_2(REG_SUB, "=REG=  %03lu- %02lu     Permanently   Added for Max registration failure", mcc, mnc);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                          SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE)
          {
            MSG_HIGH_DS_2(REG_SUB, "=REG=  %03lu- %02lu     Permanently   Added as voice is not available", mcc, mnc);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                       SYS_BLOCK_PLMN_CAUSE_OTHERS)
          {
            MSG_HIGH_DS_2(REG_SUB, "=REG=  %03lu- %02lu     Permanently   Added for other causes", mcc, mnc);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                       SYS_BLOCK_PLMN_CAUSE_PRIORITY )
          {
            MSG_HIGH_DS_2(REG_SUB, "=REG=  %03lu-%02lu     Permanently   Added for DS blocking priority causes", mcc, mnc);
          }
        }
        else
        {
          if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                            SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE)
          {
            MSG_HIGH_DS_3(REG_SUB, "=REG=  %03lu- %02lu     %10lu       Added for Max registration failure", mcc, mnc,
                 reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                            SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE)
          {
            MSG_HIGH_DS_3(REG_SUB, "=REG=  %03lu- %02lu     %10lu       Added as voice is not available", mcc, mnc,
                 reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                            SYS_BLOCK_PLMN_CAUSE_OTHERS)
          {
            MSG_HIGH_DS_3(REG_SUB, "=REG=  %03lu- %02lu     %10lu       Added for other causes", mcc, mnc,
                 reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count);
          }
          else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == 
                                            SYS_BLOCK_PLMN_CAUSE_PRIORITY )
          {
            MSG_HIGH_DS_3(REG_SUB, "=REG=  %03lu- %02lu     %10lu       Added for DS Blocking priority", mcc, mnc,
                 reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count);
          }
        }
      }
    }
  }
}
#endif

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_sim_load_card_mode

==============================================================================*/

boolean reg_sim_load_card_mode
(
  void
)
{
  mmgsdi_file_enum_type refresh_file_list[REG_SIM_NUM_REFRESH_FILE];  
  mmgsdi_refresh_file_list_type refresh_files;
  mmgsdi_return_enum_type        mmgsdi_status = MMGSDI_SUCCESS;  
  mmgsdi_session_info_query_type    session_info_query;
  mmgsdi_session_get_info_type      session_get_info_ptr;
  sys_plmn_id_s_type                sim_rplmn;

  mmgsdi_cached_nv_item_data nv_item;
 
  nv_rplmnact_type rplmnact;

#ifdef FEATURE_ENHANCED_NW_SELECTION
  reg_sim_is_ens_read    = FALSE;
  reg_sim_is_ahplmn_read = FALSE;
#endif
  memset(&refresh_files, 0x00, sizeof(mmgsdi_refresh_file_list_type));
  refresh_files.file_list_ptr = refresh_file_list;

  /* get SIM CARD MODE */
  session_info_query.query_type = MMGSDI_SESSION_INFO_QUERY_BY_TYPE;
  session_info_query.query_key.session_type = reg_sim_mmgsdi_info.session_type;
  if (MMGSDI_SUCCESS != mmgsdi_session_get_info_sync(session_info_query, &session_get_info_ptr))
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card mode get failed!!!");
  }
  reg_sim_card_mode = session_get_info_ptr.app_data.app_type;

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
  reg_sim_mmgsdi_info.slot_id = session_get_info_ptr.slot_id;
#endif
  MSG_HIGH_DS_1(REG_SUB,"slot id received is", session_get_info_ptr.slot_id);

  if ((reg_sim_card_mode == MMGSDI_APP_UNKNOWN) || 
      (reg_sim_card_mode == MMGSDI_APP_NONE) ||
      ((session_get_info_ptr.app_state != MMGSDI_APP_STATE_READY)&&
      (session_get_info_ptr.app_state != MMGSDI_APP_STATE_ILLEGAL)&&
      (session_get_info_ptr.app_state != MMGSDI_APP_STATE_WAIT_FOR_SUB_OK)&&
      (session_get_info_ptr.app_state != MMGSDI_APP_STATE_EXT_PERSO_FAILED)&&
      (session_get_info_ptr.app_state != MMGSDI_APP_STATE_READY_FOR_PERSO)))
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM either not initialized or invalid");
    reg_sim_card_mode = MMGSDI_APP_UNKNOWN;
    reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
    return (boolean)(reg_sim_state == SYS_SIM_STATE_AVAILABLE);
  }

  switch (reg_sim_card_mode)
  {
    case MMGSDI_APP_SIM:
      if (reg_sim_state == SYS_SIM_STATE_NOT_AVAILABLE)
      {
        reg_sim_state = SYS_SIM_STATE_AVAILABLE;
      }

      /* Populate reg_sim_refresh_file_list */

      reg_sim_populate_sim_refresh_file_list(&refresh_files); 
 
      mmgsdi_status = mmgsdi_session_register_for_refresh(
                                  reg_sim_mmgsdi_info.session_id,
                                  refresh_files,
                                  FALSE,
                                  reg_sim_mmgsdi_generic_cb,
                                  0
                                );

      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
#ifdef FEATURE_ENHANCED_NW_SELECTION
        reg_sim_ens_flag = 0x00;
#endif
        MSG_ERROR_DS_0( REG_SUB, "=REG= Failed to Register for FCN - GSM Card");
      }
      else
      {
#ifdef FEATURE_ENHANCED_NW_SELECTION
        reg_sim_ens_flag = reg_sim_read_ens_flag();
#endif
        MSG_HIGH_DS_0(REG_SUB, "=REG= Success FCN Registration - GSM Card");
      }

      /*
      ** To update the global reg_sim_hplmn.
      */
      (void)reg_sim_read_hplmn();

#ifdef FEATURE_EQUIVALENT_HPLMN
#ifdef FEATURE_TDSCDMA
      reg_nv_read_tdscdma_plmn_file();      
      reg_mode_set_usim_compliancy();
#endif
      /*
      ** Read EHPLMN list and update global reg_sim_ehplmn_list.
      ** The reg_sim_ehplmn_list will be populated either from the card
      ** or from the NV.
      */
      (void)reg_sim_read_ehplmn_list();
#endif
      MSG_HIGH_DS_0(REG_SUB, "=REG= SIM card mode (GSM)");
      break;

    case MMGSDI_APP_USIM:
      if (reg_sim_state == SYS_SIM_STATE_NOT_AVAILABLE)
      {
        reg_sim_state = SYS_SIM_STATE_AVAILABLE;
      }

      /* Populate reg_sim_refresh_file_list */

      reg_sim_populate_sim_refresh_file_list(&refresh_files);  
 
#ifdef FEATURE_REL7_STEERING_OF_ROAMING
      mmgsdi_status = mmgsdi_session_register_for_refresh_with_steering (
                                   reg_sim_mmgsdi_info.session_id,
                                   refresh_files,
                                   FALSE,
                                   reg_sim_mmgsdi_generic_cb,
                                   0
                                 );
#else
      
      mmgsdi_status = mmgsdi_session_register_for_refresh(
                                  reg_sim_mmgsdi_info.session_id,
                                  refresh_files,
                                  FALSE, /* vote_for_init */
                                  reg_sim_mmgsdi_generic_cb,
                                  0
                                );
#endif /*FEATURE_REL7_STEERING_OF_ROAMING*/


      if (mmgsdi_status != MMGSDI_SUCCESS)
      {
#ifdef FEATURE_ENHANCED_NW_SELECTION
        reg_sim_ens_flag = 0x00;
#endif
        MSG_ERROR_DS_0( REG_SUB, "=REG= Failed to Register for FCN - USIM Card");
      }
      else
      {
#ifdef FEATURE_ENHANCED_NW_SELECTION
        reg_sim_ens_flag = reg_sim_read_ens_flag();
#endif
        MSG_HIGH_DS_0(REG_SUB, "=REG= Success FCN Registration - USIM Card");
      }

      /*
      ** To update the global reg_sim_hplmn.
      */
      (void)reg_sim_read_hplmn();
      
#ifdef FEATURE_EQUIVALENT_HPLMN      
#ifdef FEATURE_TDSCDMA
      reg_nv_read_tdscdma_plmn_file();
      reg_mode_set_usim_compliancy();
#endif

      /*
      ** Read EHPLMN list and update global reg_sim_ehplmn_list.
      ** The reg_sim_ehplmn_list will be populated either from the card
      ** or from the NV.
      */
      (void)reg_sim_read_ehplmn_list();
#endif
      MSG_HIGH_DS_0(REG_SUB, "=REG= SIM card mode (USIM)");
      break;

    default:
      reg_sim_card_mode = MMGSDI_APP_UNKNOWN;
      reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
      MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card mode (UNKNOWN)");
      break;
  }

  reg_sim_gprs_fplmn_list.length = 0;


  /* If RPLMN has invalid digits, invalidate the RPLMN RAT Search Order too */
  sim_rplmn = reg_sim_read_sim_rplmn(SYS_SRV_DOMAIN_PS_ONLY);
  if(sys_plmn_id_is_valid(sim_rplmn) == TRUE) 
  {
     sim_rplmn = reg_sim_read_sim_rplmn(SYS_SRV_DOMAIN_CS_ONLY);
  }
  if(sys_plmn_id_is_valid(sim_rplmn) == FALSE)
  {
     reg_sim_write_rplmn_rat(SYS_RAT_NONE);
  }
  

   /*
   ** In case MMGSDI NV "MMGSDI_NV_SUPPORT_EXTENDED_FPLMN_ICC_I" is enabled, 
   ** REG removes holes(invalid FPLMN) from the FPLMN list before writing FPLMN list to card
   */
  memset(&nv_item, 0x00, sizeof(mmgsdi_cached_nv_item_data));
  nv_item.nv_item_enum = MMGSDI_NV_SUPPORT_EXTENDED_FPLMN_ICC_I ;
  mmgsdi_status = mmgsdi_get_cached_nv_item(&nv_item);

  MSG_HIGH_DS_2(REG_SUB, "=REG= NV Read status = %d NV support_extended_fplmn_icc = %d", mmgsdi_status, nv_item.nv_item_data.support_extended_fplmn_icc);

  if(mmgsdi_status == MMGSDI_SUCCESS &&
     nv_item.nv_item_data.support_extended_fplmn_icc == TRUE)
  {
    reg_nv_extended_fplmn_icc = TRUE;
  }
  else
  {
    reg_nv_extended_fplmn_icc = FALSE;
  }

  /*
  ** Initialize the value of rplmn_rat_search_order using the value stored
  ** in NVRAM.
  */
  reg_nv_read_rplmnact(&rplmnact);

  if ( rplmnact.act[1] & 0x80 )
  {
    reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
  }
  else if ( rplmnact.act[0] & 0x80 )
  {
    reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED;
  }
#ifdef FEATURE_LTE
  else if ( rplmnact.act[0] & 0x40 )
  {
    reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED;
  }
#endif/*FEATURE_LTE*/
#if defined(FEATURE_FEMTO_CSG) || defined(FEATURE_NAS_ECALL)
  reg_sim_rplmn_read = FALSE;
#endif 

  if(reg_sim_read_efnas_config() == FALSE)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Read NASCONFIG from NV");
    reg_nv_read_efnas_config();
  }
#ifdef FEATURE_FEMTO_CSG
  reg_sim_mm_csg_list_read_complete = FALSE; 
#endif
  /* Read prefererd plmn list from NV*/
  reg_nv_read_preferred_plmn_list();
  
  return (boolean)(reg_sim_state == SYS_SIM_STATE_AVAILABLE);
}


/*==============================================================================

FUNCTION NAME

  reg_sim_reset_card

==============================================================================*/

void reg_sim_reset_card
(
  void
)
{
  reg_sim_state_set(SYS_SIM_STATE_NOT_AVAILABLE);
  reg_sim_card_mode = MMGSDI_APP_UNKNOWN;

#ifdef FEATURE_ENHANCED_NW_SELECTION
  reg_sim_is_ens_read    = FALSE;
  reg_sim_is_ahplmn_read = FALSE;
#endif

  reg_sim_gprs_fplmn_list.length = 0;

#ifdef FEATURE_EQUIVALENT_HPLMN
  reg_sim_ehplmn_list.length = 0;
#endif
}


/*==============================================================================

FUNCTION NAME

  reg_sim_state_set

==============================================================================*/

void reg_sim_state_set
(
  sys_sim_state_e_type sim_state
)
{
  switch (reg_sim_state)
  {
    case SYS_SIM_STATE_AVAILABLE:
      switch (sim_state)
      {
        case SYS_SIM_STATE_AVAILABLE:
          reg_sim_state = SYS_SIM_STATE_AVAILABLE;
          break;

        case SYS_SIM_STATE_CS_INVALID:
          reg_sim_state = SYS_SIM_STATE_CS_INVALID;
          break;

        case SYS_SIM_STATE_PS_INVALID:
          reg_sim_state = SYS_SIM_STATE_PS_INVALID;
          break;

        case SYS_SIM_STATE_CS_PS_INVALID:
          reg_sim_state = SYS_SIM_STATE_CS_PS_INVALID;
          break;

        case SYS_SIM_STATE_NOT_AVAILABLE:
        default:
          reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
          break;
      }
      break;

    case SYS_SIM_STATE_CS_INVALID:
      switch (sim_state)
      {
        case SYS_SIM_STATE_AVAILABLE:
        case SYS_SIM_STATE_CS_INVALID:
          reg_sim_state = SYS_SIM_STATE_CS_INVALID;
          break;

        case SYS_SIM_STATE_PS_INVALID:
        case SYS_SIM_STATE_CS_PS_INVALID:
          reg_sim_state = SYS_SIM_STATE_CS_PS_INVALID;
          break;

        case SYS_SIM_STATE_NOT_AVAILABLE:
        default:
          reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
          break;
      }
      break;

    case SYS_SIM_STATE_PS_INVALID:
      switch (sim_state)
      {
        case SYS_SIM_STATE_AVAILABLE:
          reg_sim_state = SYS_SIM_STATE_PS_INVALID;
          break;

        case SYS_SIM_STATE_CS_INVALID:
        case SYS_SIM_STATE_CS_PS_INVALID:
          reg_sim_state = SYS_SIM_STATE_CS_PS_INVALID;
          break;

        case SYS_SIM_STATE_PS_INVALID:
          reg_sim_state = SYS_SIM_STATE_PS_INVALID;
          break;

        case SYS_SIM_STATE_NOT_AVAILABLE:
        default:
          reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
          break;
      }
      break;

    case SYS_SIM_STATE_CS_PS_INVALID:
      switch (sim_state)
      {
        case SYS_SIM_STATE_AVAILABLE:
        case SYS_SIM_STATE_CS_INVALID:
        case SYS_SIM_STATE_PS_INVALID:
        case SYS_SIM_STATE_CS_PS_INVALID:
          reg_sim_state = SYS_SIM_STATE_CS_PS_INVALID;
          break;

        case SYS_SIM_STATE_NOT_AVAILABLE:
        default:
          reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
          break;
      }
      break;

    case SYS_SIM_STATE_NOT_AVAILABLE:
    default:
      reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
      break;
  }
}


/*==============================================================================

FUNCTION NAME

  reg_sim_reset_sim_state

==============================================================================*/

void reg_sim_reset_sim_state(void)
{
  reg_sim_state = SYS_SIM_STATE_AVAILABLE;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_state_get

==============================================================================*/

sys_sim_state_e_type reg_sim_state_get
(
  void
)
{
  return reg_sim_state;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_state_print

==============================================================================*/

void reg_sim_state_print
(
  void
)
{
  switch (reg_sim_state)
  {
    case SYS_SIM_STATE_NOT_AVAILABLE:
      MSG_HIGH_DS_0(REG_SUB, "=REG= SYS_SIM_STATE_NOT_AVAILABLE");
      break;
    case SYS_SIM_STATE_AVAILABLE:
      MSG_HIGH_DS_0(REG_SUB, "=REG= SYS_SIM_STATE_AVAILABLE");
      break;
    case SYS_SIM_STATE_CS_INVALID:
      MSG_HIGH_DS_0(REG_SUB, "=REG= SYS_SIM_STATE_CS_INVALID");
      break;
    case SYS_SIM_STATE_PS_INVALID:
      MSG_HIGH_DS_0(REG_SUB, "=REG= SYS_SIM_STATE_PS_INVALID");
      break;
    case SYS_SIM_STATE_CS_PS_INVALID:
      MSG_HIGH_DS_0(REG_SUB, "=REG= SYS_SIM_STATE_CS_PS_INVALID");
      break;
    default:
      MSG_HIGH_DS_0(REG_SUB, "=REG= SYS_SIM_STATE_UNKNOWN");
      break;
  }
}


/*==============================================================================

FUNCTION NAME

  reg_sim_allowed_service_domain

==============================================================================*/

sys_srv_domain_e_type reg_sim_allowed_service_domain
(
  sys_srv_domain_e_type req_service_domain
)
{
  sys_srv_domain_e_type allowed_service_domain = req_service_domain;

  /*
  ** Determine the allowed service domain based upon the
  ** current state of the SIM/USIM.
  */
  switch (req_service_domain)
  {
    case SYS_SRV_DOMAIN_CS_ONLY:
      if (reg_sim_state_get() == SYS_SIM_STATE_CS_PS_INVALID)
      {
        allowed_service_domain = SYS_SRV_DOMAIN_NO_SRV;
      }
      break;

    case SYS_SRV_DOMAIN_PS_ONLY:
      if (reg_sim_state_get() == SYS_SIM_STATE_CS_PS_INVALID)
      {
        allowed_service_domain = SYS_SRV_DOMAIN_NO_SRV;
      }
      break;

    case SYS_SRV_DOMAIN_CS_PS:
      if (reg_sim_state_get() == SYS_SIM_STATE_CS_INVALID)
      {
        allowed_service_domain = SYS_SRV_DOMAIN_PS_ONLY;
      }
      else if (reg_sim_state_get() == SYS_SIM_STATE_PS_INVALID)
      {
        allowed_service_domain = SYS_SRV_DOMAIN_CS_ONLY;
      }
      else if (reg_sim_state_get() == SYS_SIM_STATE_CS_PS_INVALID)
      {
        allowed_service_domain = SYS_SRV_DOMAIN_NO_SRV;
      }
      break;

    case SYS_SRV_DOMAIN_NO_SRV:
    default:
      break;
  }

  return allowed_service_domain;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_read_imsi

==============================================================================*/

boolean reg_sim_read_imsi
(
  reg_sim_imsi_s_type* imsi_p
)
{
  byte    imsi[MMGSDI_IMSI_LEN];
  boolean status;
  byte   i;

  /*
  ** Read the IMSI from the SIM.
  */
  if ( reg_sim_card_mode == MMGSDI_APP_SIM )
  {
    status = reg_sim_read( MMGSDI_GSM_IMSI,  imsi, MMGSDI_IMSI_LEN);
  }
  /*
  ** Read the IMSI from the USIM.
  */
  else if ( reg_sim_card_mode == MMGSDI_APP_USIM )
  {
    status = reg_sim_read( MMGSDI_USIM_IMSI, imsi, MMGSDI_IMSI_LEN);
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return FALSE;
  }

  /*
  ** Copy the IMSI to the output parameter.
  */
  if ( status )
  {

    for (i = 0; i < REG_SIM_IMSI_MAX_LENGTH; i++)
    {
      imsi_p->digits[i] = imsi[i+1];
    }
    MSG_HIGH_DS_8(REG_SUB, "=REG= IMSI[0] = 0x%02x IMSI[1] = 0x%02x IMSI[2] = 0x%02x  IMSI[3] = 0x%02x \
                  IMSI[4] = 0x%02x IMSI[5] = 0x%02x IMSI[6] = 0x%02x IMSI[7] = 0x%02x ",imsi_p->digits[0],
                  imsi_p->digits[1],imsi_p->digits[2],imsi_p->digits[3],imsi_p->digits[4],imsi_p->digits[5],
                  imsi_p->digits[6],imsi_p->digits[7]);	
  }
  else
  {
    for (i = 0; i < REG_SIM_IMSI_MAX_LENGTH; i++)
    {
      imsi_p->digits[i] = 0;
    }
  }

  return status;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_read_hplmn

==============================================================================*/

sys_plmn_id_s_type reg_sim_read_hplmn
(
  void
)
{
  sys_plmn_id_s_type hplmn;
#ifdef FEATURE_ENHANCED_NW_SELECTION
  sys_plmn_id_s_type acting_hplmn = {{0xFF, 0xFF, 0xFF}};
  uint32             acting_mcc;
  uint32             acting_mnc;
#endif

  byte               imsi[MMGSDI_IMSI_LEN];

  boolean            status;
  boolean            plmn_id_is_undefined;
  boolean            mnc_includes_pcs_digit;
  uint32             mcc;
  uint32             mnc;
  byte               admin_data[REG_SIM_ADMIN_LENGTH];

  sys_plmn_undefine_plmn_id (&hplmn);

  /*
  ** Read the IMSI from the SIM.
  */
  if ( reg_sim_card_mode == MMGSDI_APP_SIM )
  {
    status = reg_sim_read( MMGSDI_GSM_IMSI,  imsi, MMGSDI_IMSI_LEN);
  }
  /*
  ** Read the IMSI from the USIM.
  */
  else if ( reg_sim_card_mode == MMGSDI_APP_USIM )
  {
    status = reg_sim_read( MMGSDI_USIM_IMSI, imsi, MMGSDI_IMSI_LEN);
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return hplmn;
  }

  /*
  ** Extract the HPLMN from the IMSI.
  */
  if ( status )
  {
    status = reg_sim_read
    (
      (reg_sim_card_mode == MMGSDI_APP_SIM ? MMGSDI_GSM_AD : MMGSDI_USIM_AD),
      admin_data, 
      REG_SIM_ADMIN_LENGTH
    );
    
    /*
    ** Fill MCC Digit 1 and MCC Digit 2
    */
    hplmn.identity[0] = (byte)(( ( imsi[1] & 0xF0 ) / 0x10 ) +
                               ( ( imsi[2] & 0x0F ) * 0x10 ));

    /*
    ** Fill MCC Digit 3 and MNC Digit 3
    */
    hplmn.identity[1] = (byte)(( ( imsi[2] & 0xF0 ) / 0x10 ) +
                               ( ( imsi[4] & 0x0F ) * 0x10 ));

    /*
    ** If 2 digits MNC is indicated in EF-AD then set MNC digit 3 to 'F'.
    */
    if (status && ((admin_data[3] & 0x0F) == 0x02))
    {
      hplmn.identity[1] |= 0xF0;
    }
    
    /*
    ** Fill MNC Digit 1 and MNC Digit 2
    */
    hplmn.identity[2] = imsi[3];
  }

  sys_plmn_get_mcc_mnc
  (
    hplmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );

#ifdef FEATURE_ENHANCED_NW_SELECTION
  /*
  ** If Acting HPLMN feature is supported.
  */
  if (reg_sim_read_ens_flag() & GSDI_SUPPORT_ACTING_HPLMN)
  {
    if (reg_sim_is_ahplmn_read)
    {
      hplmn = reg_sim_hplmn;
      sys_plmn_get_mcc_mnc
      (
        hplmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
    }
    else
    {
      status = reg_sim_read
      (
        (reg_sim_card_mode == MMGSDI_APP_SIM ? MMGSDI_SIM_7F66_PROP1_ACT_HPLMN : MMGSDI_USIM_7F66_PROP1_ACT_HPLMN),
        (byte *)&acting_hplmn,
        GSDI_CINGULAR_ACT_HPLMN_LEN
      );
      reg_sim_is_ahplmn_read = TRUE;
      if (status == TRUE)
      {
        sys_plmn_get_mcc_mnc
        (
          acting_hplmn,
          &plmn_id_is_undefined,
          &mnc_includes_pcs_digit,
          &acting_mcc,
          &acting_mnc
        );
        /*
        ** Valid Acting HPLMN should not be undefined
        ** (and mcc should match with TrueHPLMN.)
        ** MCC can be different from TrueHPLMN (Phase-II).
        */
        if (!sys_plmn_id_is_undefined(acting_hplmn))
        {
          MSG_HIGH_DS_0(REG_SUB, "=REG= Acting HPLMN is considered as HPLMN");
          hplmn = acting_hplmn;
          mcc = acting_mcc;
          mnc = acting_mnc;
        }
      }
    }
  }
#endif

  NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
  reg_sim_hplmn = hplmn;
  NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);

  return hplmn;
}

#ifdef FEATURE_DUAL_SIM
sys_plmn_id_s_type reg_sim_per_subs_read_hplmn
(
  sys_modem_as_id_e_type as_id
)
{
  sys_plmn_id_s_type hplmn;
#ifdef FEATURE_ENHANCED_NW_SELECTION
  sys_plmn_id_s_type acting_hplmn = {{0xFF, 0xFF, 0xFF}};
  uint32             acting_mcc;
  uint32             acting_mnc;
#endif
  byte               imsi[MMGSDI_IMSI_LEN];
  boolean            status;
  boolean            plmn_id_is_undefined;
  boolean            mnc_includes_pcs_digit;
  uint32             mcc;
  uint32             mnc;
  byte               admin_data[REG_SIM_ADMIN_LENGTH];

  sys_plmn_undefine_plmn_id (&hplmn);

  if (IS_NOT_VALID_SUB_ID(as_id))
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= reg_sim_per_subs_read_hplmn- Invalid SUB ID: %d", as_id+1);
    return hplmn;
  }

  /*
  ** Read the IMSI from the SIM.
  */
  if ( reg_sim_card_mode_sim[as_id] == MMGSDI_APP_SIM )
  {
    status = reg_sim_per_subs_read( MMGSDI_GSM_IMSI,  imsi, MMGSDI_IMSI_LEN, as_id);
  }
  /*
  ** Read the IMSI from the USIM.
  */
  else if ( reg_sim_card_mode_sim[as_id] == MMGSDI_APP_USIM )
  {
    status = reg_sim_per_subs_read( MMGSDI_USIM_IMSI, imsi, MMGSDI_IMSI_LEN, as_id);
  }
  else
  {
    MSG_ERROR_DS_1(REG_LOCAL_SUB, "=REG= AS_ID_%d= SIM card unknown mode (GSM or USIM)", as_id+1);
    return hplmn;
  }

  /*
  ** Extract the HPLMN from the IMSI.
  */
  if ( status )
  {
    status = reg_sim_per_subs_read
    (
      (reg_sim_card_mode_sim[as_id] == MMGSDI_APP_SIM ? MMGSDI_GSM_AD : MMGSDI_USIM_AD),
      admin_data, 
      REG_SIM_ADMIN_LENGTH,
      as_id
    );
    
    /*
    ** Fill MCC Digit 1 and MCC Digit 2
    */
    hplmn.identity[0] = (byte)(( ( imsi[1] & 0xF0 ) / 0x10 ) +
                               ( ( imsi[2] & 0x0F ) * 0x10 ));

    /*
    ** Fill MCC Digit 3 and MNC Digit 3
    */
    hplmn.identity[1] = (byte)(( ( imsi[2] & 0xF0 ) / 0x10 ) +
                               ( ( imsi[4] & 0x0F ) * 0x10 ));

    /*
    ** If 2 digits MNC is indicated in EF-AD then set MNC digit 3 to 'F'.
    */
    if (status && ((admin_data[3] & 0x0F) == 0x02))
    {
      hplmn.identity[1] |= 0xF0;
    }
    
    /*
    ** Fill MNC Digit 1 and MNC Digit 2
    */
    hplmn.identity[2] = imsi[3];
  }

  sys_plmn_get_mcc_mnc
  (
    hplmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );

#ifdef FEATURE_ENHANCED_NW_SELECTION
  /*
  ** If Acting HPLMN feature is supported.
  */
  if (reg_sim_read_ens_flag() & GSDI_SUPPORT_ACTING_HPLMN)
  {
    if (reg_sim_is_ahplmn_read_sim[as_id])
    {
      hplmn = reg_sim_hplmn_sim[as_id];
      sys_plmn_get_mcc_mnc
      (
        hplmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
    }
    else
    {
      status = reg_sim_per_subs_read
      (
        (reg_sim_card_mode_sim[as_id] == MMGSDI_APP_SIM ? MMGSDI_SIM_7F66_PROP1_ACT_HPLMN : MMGSDI_USIM_7F66_PROP1_ACT_HPLMN),
        (byte *)&acting_hplmn,
        GSDI_CINGULAR_ACT_HPLMN_LEN,
        as_id
      );
      reg_sim_is_ahplmn_read_sim[as_id] = TRUE;
      if (status == TRUE)
      {
        sys_plmn_get_mcc_mnc
        (
          acting_hplmn,
          &plmn_id_is_undefined,
          &mnc_includes_pcs_digit,
          &acting_mcc,
          &acting_mnc
        );
        /*
        ** Valid Acting HPLMN should not be undefined
        ** (and mcc should match with TrueHPLMN.)
        ** MCC can be different from TrueHPLMN (Phase-II).
        */
        if (!sys_plmn_id_is_undefined(acting_hplmn))
        {
          MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= Acting HPLMN is considered as HPLMN");
          hplmn = acting_hplmn;
          mcc = acting_mcc;
          mnc = acting_mnc;
        }
      }
    }
  }
#endif
  reg_sim_hplmn_sim[as_id] = hplmn;

  return hplmn;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_sim_read_hplmn_rat_search_order

==============================================================================*/
#ifndef FEATURE_RAT_PRIORITY_LIST
sys_mode_pref_e_type reg_sim_read_hplmn_rat_search_order
(
  sys_mode_pref_e_type req_mode_pref
)
{
  reg_sim_mode_pref_e_type        hplmn_rat_search_order;
  reg_sim_plmn_w_act_list_s_type  *hplmn_w_act_list = NULL;
  mmgsdi_file_enum_type           sim_filename = MMGSDI_NO_FILE_ENUM;
  int32                           file_length = 0;

  boolean                         status = FALSE;
  int32                           file_size;
  int32                           length;

  /*
  ** Initialize the HPLMN mode preference to GSM preferred to handle
  ** all of the error and off nominal conditions as specified in
  ** 3G TS 23.122 Section 4.4.3.1.1.
  */
  hplmn_rat_search_order = REG_SIM_MODE_PREF_UNDEFINED;

  file_size = REG_SIM_HPLMN_W_ACT_LIST_LENGTH * REG_SIM_PLMN_W_ACT_LENGTH;

  /*
  ** Read the HPLMNwAcT EF from the SIM.
  */
  if (reg_sim_card_mode == MMGSDI_APP_SIM)
  {
    sim_filename = MMGSDI_GSM_HPLMNACT;
  }
  /*
  ** Read the HPLMNwAcT EF from the USIM.
  */
  else if (reg_sim_card_mode == MMGSDI_APP_USIM)
  {
    sim_filename = MMGSDI_USIM_HPLMNWACT;
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return reg_sim_combined_rat_search_order
    (
      hplmn_rat_search_order,
      req_mode_pref
    );
  }
#ifdef FEATURE_MODEM_HEAP
  hplmn_w_act_list = (reg_sim_plmn_w_act_list_s_type *)modem_mem_alloc(sizeof(reg_sim_plmn_w_act_list_s), MODEM_MEM_CLIENT_NAS);
#else
  hplmn_w_act_list = (reg_sim_plmn_w_act_list_s_type *)gs_alloc(sizeof(reg_sim_plmn_w_act_list_s));
#endif
   reg_check_for_null_ptr((void*)hplmn_w_act_list );
  /*
  ** Read the attributes for the Home PLMN With Access Technology List.
  */
  if(mmgsdi_session_read_cache_file_size (reg_sim_mmgsdi_info.session_id,
                                          sim_filename,
                                          &file_length) == MMGSDI_SUCCESS)
  {
    /*
    ** Set the maximum file size for the Home PLMN With Access Technology
    ** List.
    */
    file_size = MIN(file_length, file_size);

    /*
    ** Read the Home PLMN With Access Technology List.
    */
    status = reg_sim_read
    (
      sim_filename,
      (byte *) hplmn_w_act_list->plmn_w_act,
      file_size
    );

    if ( status )
    {
      length = file_size/REG_SIM_PLMN_W_ACT_LENGTH;

      /*
      ** Convert the raw HPLMNwAcT data into a mode preference.
      */
      if (length > 0)
      {
        if ( hplmn_w_act_list->plmn_w_act[0].act[1] & 0x80 &&
             hplmn_w_act_list->plmn_w_act[0].act[0] & 0x80 )
        {
          hplmn_rat_search_order = REG_SIM_MODE_PREF_UNDEFINED;
        }
        else if ( hplmn_w_act_list->plmn_w_act[0].act[1] & 0x80 )
        {
          hplmn_rat_search_order = REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
        }
        else if ( hplmn_w_act_list->plmn_w_act[0].act[0] & 0x80 )
        {
          hplmn_rat_search_order = REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED;
        }
        else
        {
          hplmn_rat_search_order = REG_SIM_MODE_PREF_UNDEFINED;
        }
      }
    }
  }

  switch (hplmn_rat_search_order)
  {
    case REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
      MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN RAT Search Order (GSM-UMTS/LTE)");
      break;

    case REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
      MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN RAT Search Order (UMTS-GSM/LTE)");
      break;

    case REG_SIM_MODE_PREF_UNDEFINED:
    default:
      MSG_HIGH_DS_0(REG_SUB, "=REG= HPLMN RAT Search Order (UNDEFINED)");
      break;
  }

#ifndef FEATURE_MODEM_HEAP
   gs_free(hplmn_w_act_list);
#else
   modem_mem_free(hplmn_w_act_list, MODEM_MEM_CLIENT_NAS);
#endif
  hplmn_w_act_list = NULL;
  return reg_sim_combined_rat_search_order
  (
    hplmn_rat_search_order,
    req_mode_pref
  );
}
#else // FEATURE_RAT_PRIORITY_LIST
void reg_sim_read_hplmn_rat_search_order
(
  reg_mode_3gpp_capability_s_type *rat_order
)
{
  reg_sim_plmn_w_act_list_s_type  *hplmn_w_act_list = NULL;
  mmgsdi_file_enum_type           sim_filename = MMGSDI_NO_FILE_ENUM;
  int32                           file_length = 0;
  boolean                         status = FALSE;
  int32                           file_size;
  int32                           length;
  int32                           i, j, k;
  uint8                          num_of_hplmn_rat_found;
  sys_radio_access_tech_e_type                     rat_found;
  /*
   This flag is used to indicate if a match between any UMTS RAT (WCDMA or TDSCDMA) 
   in the rat order and a bit in the AcT has been found. In which case, ignore 
   the next match. 
  */
  boolean                        flag_umts_found = FALSE;
  /*
  ** Initialize the HPLMN mode preference to GSM preferred to handle
  ** all of the error and off nominal conditions as specified in
  ** 3G TS 23.122 Section 4.4.3.1.1.
  */
  sys_specialization_t  specialization = SYS_SPECIAL_NONE;
  policyman_status_t  status_pm = POLICYMAN_STATUS_ERR;
  policyman_item_t    *pItem;

  if(rat_order->num_items == 1)
  {
    return; 
  }

  {
    policyman_item_id_t ids[] = {POLICYMAN_ITEM_DEVICE_CONFIGURATION};    
    status_pm = policyman_get_items(ids, ARR_SIZE(ids), (policyman_item_t const **) &pItem);    
  }
  if (status_pm == POLICYMAN_STATUS_SUCCESS)
  {
     policyman_device_config_get_specialization(pItem, &specialization);
     policyman_item_release(pItem);
  }
  
  if(status_pm == POLICYMAN_STATUS_SUCCESS)
  {
      if (specialization == SYS_SPECIAL_SBM_RAT_SEARCH_ORDER)
      {           
          /* Ignore HPLMN RAT search order in USIM when to give priority to LTE over UMTS  */
          if (reg_mode_is_rat_present_in_rat_pri_list(SYS_RAT_LTE_RADIO_ACCESS,   
              reg_mode_rat_pri_list_get(), 
              FALSE))
          {
              return;
          }
      }
  } 
  file_size = REG_SIM_HPLMN_W_ACT_LIST_LENGTH * REG_SIM_PLMN_W_ACT_LENGTH;

  /*
  ** Read the HPLMNwAcT EF from the SIM.
  */
  if (reg_sim_card_mode == MMGSDI_APP_SIM)
  {
    sim_filename = MMGSDI_GSM_HPLMNACT;
  }
  /*
  ** Read the HPLMNwAcT EF from the USIM.
  */
  else if (reg_sim_card_mode == MMGSDI_APP_USIM)
  {
    sim_filename = MMGSDI_USIM_HPLMNWACT;
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return;
  }

#ifdef FEATURE_MODEM_HEAP
  hplmn_w_act_list = (reg_sim_plmn_w_act_list_s_type *)modem_mem_alloc(sizeof(reg_sim_plmn_w_act_list_s_type), MODEM_MEM_CLIENT_NAS);
#else
  hplmn_w_act_list = (reg_sim_plmn_w_act_list_s_type *)gs_alloc(sizeof(reg_sim_plmn_w_act_list_s_type));
#endif
   reg_check_for_null_ptr((void*)hplmn_w_act_list );
  /*
  ** Read the attributes for the Home PLMN With Access Technology List.
  */
  if(mmgsdi_session_read_cache_file_size (reg_sim_mmgsdi_info.session_id,
                                          sim_filename,
                                          &file_length) == MMGSDI_SUCCESS)
  {
    /*
    ** Set the maximum file size for the Home PLMN With Access Technology
    ** List.
    */
    file_size = MIN(file_length, file_size);

    /*
    ** Read the Home PLMN With Access Technology List.
    */
    status = reg_sim_read
    (
      sim_filename,
      (byte *) hplmn_w_act_list->plmn_w_act,
      file_size
    );

    if ( status )
    {
      length = file_size/REG_SIM_PLMN_W_ACT_LENGTH;

      num_of_hplmn_rat_found = 0;

      for(i = 0; i < length; i++)
      {
        for(j = num_of_hplmn_rat_found; j < rat_order->num_items; j++)
        {
          rat_found = SYS_RAT_NONE;
          switch(rat_order->rat_list[j])
          {
            case SYS_RAT_GSM_RADIO_ACCESS:
                if(hplmn_w_act_list->plmn_w_act[i].act[1] & 0x80)
                {
                  rat_found = SYS_RAT_GSM_RADIO_ACCESS;
                }
                break;

            case SYS_RAT_UMTS_RADIO_ACCESS:
                if ((hplmn_w_act_list->plmn_w_act[i].act[0] & 0x80)&&(!flag_umts_found))
                {
                  rat_found = SYS_RAT_UMTS_RADIO_ACCESS;
              flag_umts_found = TRUE;
                }
                break;

            case SYS_RAT_TDS_RADIO_ACCESS:
                // Note that FDD and TDD are sharing the bit
                if ((hplmn_w_act_list->plmn_w_act[i].act[0] & 0x80) && (!flag_umts_found))
                {
                  rat_found = SYS_RAT_TDS_RADIO_ACCESS;
          flag_umts_found = TRUE;
                }
                break;


            case SYS_RAT_LTE_RADIO_ACCESS:
                if(hplmn_w_act_list->plmn_w_act[i].act[0] & 0x40)
                {
                  rat_found = SYS_RAT_LTE_RADIO_ACCESS;
                }
                break;
           
            default:         
                break;
          }
          if(rat_found != SYS_RAT_NONE)
          {
            //RAT found in HPLMNwACT. Move it to last of HPLMN ACT found so far
            for(k = j; k > num_of_hplmn_rat_found; k--)
            {
              rat_order->rat_list[k] = rat_order->rat_list[k-1];  
            }
            rat_order->rat_list[num_of_hplmn_rat_found] = rat_found;
            num_of_hplmn_rat_found++;

            if(num_of_hplmn_rat_found == rat_order->num_items -1)
            {
              break;  
            }
          }
        }
        if(num_of_hplmn_rat_found >= rat_order->num_items -1)
        {
          break;  
        } 
      } 
    }
    MSG_HIGH_DS_1(REG_SUB, "=REG= HPLMN RAT Search Order is num_of_rats: %d", rat_order->num_items);
    for(i = 0; i < rat_order->num_items; i++)
    {
      switch(rat_order->rat_list[i])
      {
        case SYS_RAT_GSM_RADIO_ACCESS:
          MSG_HIGH_DS_1(REG_SUB, "=REG= RAT  %d:  GSM", i);   
          break;
        case SYS_RAT_UMTS_RADIO_ACCESS:
          MSG_HIGH_DS_1(REG_SUB, "=REG= RAT  %d:  UMTS", i);
          break;

        case SYS_RAT_TDS_RADIO_ACCESS:
          MSG_HIGH_DS_1(REG_SUB, "=REG= RAT  %d:  TDS", i);
          break;

        case SYS_RAT_LTE_RADIO_ACCESS:
          MSG_HIGH_DS_1(REG_SUB, "=REG= RAT  %d:  LTE", i); 
          break;   
        default:
          break;
      } 
    }
  }
#ifndef FEATURE_MODEM_HEAP
   gs_free(hplmn_w_act_list);
#else
   modem_mem_free(hplmn_w_act_list, MODEM_MEM_CLIENT_NAS);
#endif
  hplmn_w_act_list = NULL;
}
#endif /*ifndef FEATURE_RAT_PRIORITY_LIST*/

/*==============================================================================

FUNCTION NAME

  reg_sim_read_hplmn_search_period

==============================================================================*/

byte reg_sim_read_hplmn_search_period
(
  void
)
{
  byte hplmn_search_period = REG_SIM_DEFAULT_HPLMN_SP;
  boolean success = FALSE;
#ifdef FEATURE_ENHANCED_NW_SELECTION
  byte ens_flag;
#endif
  /*
  ** Read the HPLMN Search Period from the SIM.
  */
  if ( reg_sim_card_mode == MMGSDI_APP_SIM )
  {
    success = reg_sim_read
    (
      MMGSDI_GSM_HPLMN,
      &hplmn_search_period,
      MMGSDI_HPLMN_SP_LEN
    );
  }
  /*
  ** Read the HPLMN Search Period from the USIM.
  */
  else if ( reg_sim_card_mode == MMGSDI_APP_USIM )
  {
    success = reg_sim_read
    (
      MMGSDI_USIM_HPLMN,
      &hplmn_search_period,
      MMGSDI_HPLMN_SP_LEN
    );
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
  }

#ifdef FEATURE_ENHANCED_NW_SELECTION
  /*
  ** Use default Timer value if the value read is 'FF' or value > 80
  */
  ens_flag = reg_sim_read_ens_flag();
  if (!success || ((ens_flag & GSDI_SUPPORT_HPLMN_TIMER) && (hplmn_search_period == 0xFF)) || (hplmn_search_period > 80))
#else
  if (!success || (hplmn_search_period > 80))
#endif
  {
    hplmn_search_period = REG_SIM_DEFAULT_HPLMN_SP;

    MSG_HIGH_DS_1(REG_SUB, "=REG= HPLMN Search Period (%d - Default)", hplmn_search_period);
  }
  else
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= HPLMN Search Period (%d)", hplmn_search_period);
  }

  return hplmn_search_period;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_read_sim_rplmn

==============================================================================*/
sys_plmn_id_s_type reg_sim_read_sim_rplmn
(
  sys_srv_domain_e_type req_service_domain
)
{
  sys_plmn_id_s_type rplmn;
  boolean            plmn_id_is_undefined;
  boolean            mnc_includes_pcs_digit;
  uint32             mcc;
  uint32             mnc;  
#ifdef FEATURE_LTE 
  reg_sim_mode_pref_e_type   rplmn_rat_search_order = reg_sim_rplmn_rat_search_order;
#endif /*FEATURE_LTE*/

  sys_plmn_undefine_plmn_id(&rplmn);
#ifdef FEATURE_LTE 

#ifdef FEATURE_DISABLE_RPLMNACT
  /* Initial RAT search order is UNDEFINED at power on, get the RAT from rat_priority list*/
  if (rplmn_rat_search_order == REG_SIM_MODE_PREF_UNDEFINED)
  {
     reg_mode_3gpp_capability_s_type rat_search_order;

     rplmn_rat_search_order = REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
     reg_mode_get_3gpp_rat_capabilty(&rat_search_order, reg_mode_rat_pri_list_get()
#ifdef FEATURE_SGLTE
      ,TRUE
#endif                                  
     );
     if (rat_search_order.rat_list[0] == SYS_RAT_LTE_RADIO_ACCESS)
     {
       rplmn_rat_search_order = REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED;
     }
     else if (rat_search_order.rat_list[0] == SYS_RAT_UMTS_RADIO_ACCESS)
     {
       rplmn_rat_search_order = REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED;
     }
     else if (rat_search_order.rat_list[0] == SYS_RAT_TDS_RADIO_ACCESS)
     {
       rplmn_rat_search_order = REG_SIM_MODE_PREF_TDSCDMA_RADIO_ACCESS_PREFERRED;
     }
  }
#endif

  if (rplmn_rat_search_order == REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED)
  {
    rplmn = reg_sim_read_eps_rplmn();

    sys_plmn_get_mcc_mnc
    (
      rplmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
    
    MSG_HIGH_DS_2(REG_SUB, "=REG= EPS RPLMN(%d-%d)", mcc, mnc);
  }
  else
#endif /*FEATURE_LTE*/
  {
    if (req_service_domain == SYS_SRV_DOMAIN_PS_ONLY)
    {
      rplmn = reg_sim_read_ps_rplmn();
  
      sys_plmn_get_mcc_mnc
      (
        rplmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
  
      MSG_HIGH_DS_2(REG_SUB, "=REG= PS RPLMN(%d-%d)", mcc, mnc);
    }
    else
    {
      rplmn = reg_sim_read_cs_rplmn();
  
      sys_plmn_get_mcc_mnc
      (
        rplmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
  
      MSG_HIGH_DS_2(REG_SUB, "=REG= CS RPLMN(%d-%d)", mcc, mnc);
    }
  }
  return rplmn;
}



/*==============================================================================

FUNCTION NAME

  reg_sim_read_rplmn

==============================================================================*/
sys_plmn_id_s_type reg_sim_read_rplmn
(
  sys_srv_domain_e_type req_service_domain
)
{
  sys_plmn_id_s_type rplmn;
#ifdef FEATURE_FEMTO_CSG
  boolean            plmn_id_is_undefined;
  boolean            mnc_includes_pcs_digit;
  uint32             mcc;
  uint32             mnc;
#endif 

#if !(defined FEATURE_FEMTO_CSG) && !(defined FEATURE_NAS_ECALL) 
  rplmn = reg_sim_read_sim_rplmn(req_service_domain);

  /* If RPLMN read from SIM has invalid digits, read RPLMN as FF FF FF */
  if(sys_plmn_id_is_valid(rplmn) == FALSE)
  {
      MSG_HIGH_DS_3(REG_SUB, "=REG= Read invalid RPLMN from SIM, id[0] = %d, id[1] = %d, id[2] = %d", \
                        rplmn.identity[0],rplmn.identity[1],rplmn.identity[2]);
      sys_plmn_undefine_plmn_id(&rplmn); 
  }

#else
  rplmn = reg_nv_read_rplmn();

  /*If RPLMN NV value is undefined PLMN and 
    ** RPLMN being read from first time after SIM avaialable is received 
    ** then read RPLMN from SIM files
*/
  if((sys_plmn_id_is_undefined(rplmn) == TRUE) && (reg_sim_rplmn_read == FALSE))
  {
    rplmn = reg_sim_read_sim_rplmn(req_service_domain);
    /* If RPLMN read from SIM has invalid digits, read RPLMN as FF FF FF */
    if(sys_plmn_id_is_valid(rplmn) == FALSE)
    {
      MSG_HIGH_DS_3(REG_SUB, "=REG= Read invalid RPLMN from SIM, id[0] = %d, id[1] = %d, id[2] = %d", \
                          rplmn.identity[0],rplmn.identity[1],rplmn.identity[2]);
      sys_plmn_undefine_plmn_id(&rplmn); 
    }
    reg_nv_write_rplmn(rplmn);
    reg_nv_write_rplmn_dup(FALSE);
  }

  reg_sim_rplmn_read = TRUE;

  sys_plmn_get_mcc_mnc
  (
    rplmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
  );
  
  MSG_HIGH_DS_2(REG_SUB, "=REG= Stored RPLMN(%d-%d)", mcc, mnc);
#endif 
  return rplmn;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_read_cs_rplmn

==============================================================================*/

sys_plmn_id_s_type reg_sim_read_cs_rplmn
(
  void
)
{
  sys_plmn_id_s_type rplmn;
  byte               loci[MMGSDI_LOCI_LEN];
  boolean            status;

  sys_plmn_undefine_plmn_id (&rplmn);

  /*
  ** Read the LOCI from the SIM
  */
  if (reg_sim_card_mode == MMGSDI_APP_SIM)
  {
    status = reg_sim_read( MMGSDI_GSM_LOCI, loci, MMGSDI_LOCI_LEN);
  }
  else if (reg_sim_card_mode == MMGSDI_APP_USIM)
  {
    status = reg_sim_read( MMGSDI_USIM_LOCI, loci, MMGSDI_LOCI_LEN);
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return rplmn;
  }

  if ( status )
  {
    /*
    ** The RPLMN is only set if the update status is UPDATED, NOT_UPDATED,
    ** or LOCATION AREA NOT ALLOWED.
    */
    if (( loci[REG_SIM_LUS_OFFSET] == REG_SIM_LUS_UPDATED) ||
        ( loci[REG_SIM_LUS_OFFSET] == REG_SIM_LUS_NOT_UPDATED) ||
        ( loci[REG_SIM_LUS_OFFSET] == REG_SIM_LUS_LA_NOT_ALLOWED) )
    {
      memscpy( rplmn.identity, sizeof( rplmn.identity),
              loci + REG_SIM_LAI_OFFSET,
              REG_SIM_PLMN_LENGTH );
    }
  }
  else
  {
    memscpy(rplmn.identity, sizeof(rplmn.identity),
           reg_sim_cs_rplmn.identity,
           REG_SIM_PLMN_LENGTH );
  }
  return rplmn;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_read_ps_rplmn

==============================================================================*/

sys_plmn_id_s_type reg_sim_read_ps_rplmn
(
  void
)
{
  sys_plmn_id_s_type rplmn;
  byte               psloci[MMGSDI_LOCIGPRS_LEN];
  boolean            status;

  sys_plmn_undefine_plmn_id (&rplmn);

  /*
  ** Read the LOCI from the SIM
  */
  if (reg_sim_card_mode == MMGSDI_APP_SIM)
  {
    status = reg_sim_read( MMGSDI_GSM_LOCIGPRS, psloci, MMGSDI_LOCIGPRS_LEN);
  }
  else if (reg_sim_card_mode == MMGSDI_APP_USIM)
  {
    status = reg_sim_read( MMGSDI_USIM_PSLOCI, psloci, MMGSDI_LOCIGPRS_LEN);
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return rplmn;
  }

  /*
  ** Attempt to read PSLOCI from NVRAM if it is not available on the SIM.
  */
  if ( !status )
  {
    status = reg_nv_read_psloci(psloci);
  }

  if ( status )
  {
    /*
    ** The RPLMN is only set if the update status is UPDATED, NOT_UPDATED,
    ** or LOCATION AREA NOT ALLOWED.
    */
    if (( psloci[REG_SIM_RUS_OFFSET] == REG_SIM_LUS_UPDATED) ||
        ( psloci[REG_SIM_RUS_OFFSET] == REG_SIM_LUS_NOT_UPDATED) ||
        ( psloci[REG_SIM_RUS_OFFSET] == REG_SIM_LUS_RA_NOT_ALLOWED) )
    {
      memscpy( rplmn.identity, sizeof( rplmn.identity),
              psloci + REG_SIM_RAI_OFFSET,
              REG_SIM_PLMN_LENGTH );
    }
  }

  return rplmn;
}

/*==============================================================================

FUNCTION NAME

  reg_sim_is_rplmn

==============================================================================*/
boolean reg_sim_is_rplmn
(
  sys_plmn_service_state_s_type  *current_service_state
)
{
  if(
#ifdef FEATURE_LTE
      ((current_service_state->active_rat == SYS_RAT_LTE_RADIO_ACCESS)&&
         sys_plmn_match(reg_sim_read_eps_rplmn(),current_service_state->plmn)) ||
#endif
       (
#ifdef FEATURE_LTE
        (current_service_state->active_rat != SYS_RAT_LTE_RADIO_ACCESS) && 
#endif
         (((reg_state_reg_service_domain_get() == SYS_SRV_DOMAIN_PS_ONLY) && 
             sys_plmn_match(reg_sim_read_ps_rplmn(),current_service_state->plmn)) ||
           ((reg_state_reg_service_domain_get() != SYS_SRV_DOMAIN_PS_ONLY) && 
            sys_plmn_match(reg_sim_read_cs_rplmn(),current_service_state->plmn)))
     ))
   {
     return TRUE;
   }
   else
   {
     return FALSE;
   }

}


#ifdef FEATURE_LTE

/*==============================================================================

FUNCTION NAME

  reg_sim_read_eps_rplmn

==============================================================================*/

sys_plmn_id_s_type reg_sim_read_eps_rplmn
(
  void
)
{
  sys_plmn_id_s_type rplmn;
  byte                           epsloci[REG_EPSLOCI_LENGTH];
  boolean                     status = FALSE;
  
  sys_plmn_undefine_plmn_id (&rplmn);
  
  /*
  ** Read the EPSLOCI from the SIM
  */
  if (reg_sim_card_mode == MMGSDI_APP_USIM)
  {
    status = reg_sim_read( MMGSDI_USIM_EPSLOCI, epsloci, REG_EPSLOCI_LENGTH); 
  }
  else if (reg_sim_card_mode != MMGSDI_APP_SIM)
  {
    MSG_ERROR_DS_0(REG_SUB,"=REG= SIM card unknown mode (GSM or USIM)");
    return rplmn;
  }
  
  /*
  ** Attempt to read EPSLOCI from EFS if it is not available on the SIM.
  */
  if ( !status )
  {
    status = reg_nv_read_epsloci(epsloci);
  }
  
  if ( status )
  {
    /*
    ** The RPLMN is only set if the update status is UPDATED, NOT_UPDATED,
    ** or ROAMING NOT ALLOWED.
    */
    if (( epsloci[REG_SIM_EPS_OFFSET] == REG_SIM_EPS_UPDATED) ||
       ( epsloci[REG_SIM_EPS_OFFSET] == REG_SIM_EPS_NOT_UPDATED) ||
       ( epsloci[REG_SIM_EPS_OFFSET] == REG_SIM_EPS_ROAMING_NOT_ALLOWED) )
    {
      memscpy( rplmn.identity, sizeof( rplmn.identity),
                      epsloci + REG_SIM_TAI_OFFSET,
                      REG_SIM_PLMN_LENGTH );
    }
  }
  
  return rplmn;
}
#endif /*FEATURE_LTE*/

/*==============================================================================

FUNCTION NAME

  reg_sim_read_rplmn_rat

==============================================================================*/
sys_radio_access_tech_e_type reg_sim_read_rplmn_rat
(
  void
)
{
  sys_radio_access_tech_e_type rat = SYS_RAT_NONE;
  if ( reg_sim_rplmn_rat_search_order == REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED)
  {
    rat = SYS_RAT_GSM_RADIO_ACCESS;
  }
  else if ( reg_sim_rplmn_rat_search_order == REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED)
  {
    rat = SYS_RAT_UMTS_RADIO_ACCESS;
  }
#ifdef FEATURE_TDSCDMA
  else if ( reg_sim_rplmn_rat_search_order == REG_SIM_MODE_PREF_TDSCDMA_RADIO_ACCESS_PREFERRED)
  {
    rat = SYS_RAT_TDS_RADIO_ACCESS;
  }
#endif
#ifdef FEATURE_LTE
  else if ( reg_sim_rplmn_rat_search_order == REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED)
  {
    rat = SYS_RAT_LTE_RADIO_ACCESS;
  }
#endif
   return rat;
}
/*==============================================================================

FUNCTION NAME

  reg_sim_ignore_rplmn_act_order

==============================================================================*/
boolean reg_sim_ignore_rplmn_act_order(void)
{
  boolean ignore_rplmn_act = FALSE;
#ifdef FEATURE_TDSCDMA
  sys_plmn_id_s_type rplmn;  
  reg_sim_plmn_list_s_type *oem_list_efs = NULL;
  uint32 i;
#endif
#ifdef FEATURE_DISABLE_RPLMNACT
  ignore_rplmn_act = TRUE;
#else
  if(reg_state_ignore_rplmnact_get())
  {
    ignore_rplmn_act = TRUE;
  }
#ifdef FEATURE_TDSCDMA
  /*
   ** Disable RPLMN based system acquisition if RPLMN matches the 
   ** PLMNs residing in the EFS 
     */
  else if((reg_state_service_status_get() != SYS_SRV_STATUS_SRV) ||
          (!reg_mode_rat_pri_list_match(reg_send_last_rat_pri_list_get(),reg_mode_rat_pri_list_get())))
  {
    oem_list_efs = reg_nv_tdscdma_plmn_list_get();
    ASSERT(oem_list_efs);
    rplmn = reg_sim_read_rplmn(reg_mode_req_service_domain_get());
    for (i = 0; i < oem_list_efs->length; ++i)
    {
      if ( sys_plmn_match( rplmn, oem_list_efs->plmn[i]))       
      {
        /* Ignore the RPLMN */
        MSG_HIGH_DS_0(REG_SUB, "=REG= Match between RPLMN and NV PLMN");
        ignore_rplmn_act = TRUE;
        break;
      }
    }
  }
#endif //FEATURE_TDSCDMA
#endif
  return ignore_rplmn_act;
}
/*==============================================================================

FUNCTION NAME

  reg_sim_read_rplmn_rat_search_order

==============================================================================*/
#ifndef FEATURE_RAT_PRIORITY_LIST
sys_mode_pref_e_type reg_sim_read_rplmn_rat_search_order
(
  sys_mode_pref_e_type req_mode_pref
)
#else
void reg_sim_read_rplmn_rat_search_order
(
  reg_mode_3gpp_capability_s_type *req_rat_list
)
#endif /*ifndef FEATURE_RAT_PRIORITY_LIST*/
{
#ifndef FEATURE_DISABLE_RPLMNACT
  boolean                    status;
  reg_sim_plmn_w_act_s_type  rplmn_w_act;
#endif
#ifdef FEATURE_TDSCDMA
  nv_rplmnact_type rplmnact;
#endif
  reg_sim_mode_pref_e_type   rplmn_rat_search_order = REG_SIM_MODE_PREF_UNDEFINED;

  if(reg_sim_ignore_rplmn_act_order() == FALSE)
  {
    /*
    ** RPLMNAcT EF is not defined for a SIM so use the value stored in RAM.
    */
    if ( reg_sim_card_mode == MMGSDI_APP_SIM )
    {
      rplmn_rat_search_order = reg_sim_rplmn_rat_search_order;
    }
    /*
    ** Read the RPLMNAcT EF from the USIM.
    */
    else if ( reg_sim_card_mode == MMGSDI_APP_USIM )
    {
#ifndef  FEATURE_DISABLE_SIM_RPLMNACT
      status = reg_sim_read
      (
        MMGSDI_USIM_RPLMNACT,
        (byte *) rplmn_w_act.act,
        REG_SIM_ACT_LENGTH
      );
      if (status)
      {
        if ( rplmn_w_act.act[1] & 0x80 )
        {
          rplmn_rat_search_order = REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
        }
        else if ( rplmn_w_act.act[0] & 0x80 )
        {
          rplmn_rat_search_order = REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED;
        }
#ifdef FEATURE_LTE
        else if ( rplmn_w_act.act[0] & 0x40 )
        {
          rplmn_rat_search_order = REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED;
        }
#endif
      }
      else
#endif
      {
        rplmn_rat_search_order = reg_sim_rplmn_rat_search_order;
      }
    }
    else
    {
      MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    }
#ifdef FEATURE_TDSCDMA
     /* Since the same bit in rplmn_w_act[0] is used for WCDMA and TDSCDMA, 
     ** set the initial RAT based on the NV item.
     */

     /* First check if the shared UMTS AcT bit has been set at all*/
    if(rplmn_rat_search_order == REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED)
    {  
      
      /* Check if TDSCDMA is present in the list of RATs */
      if (reg_sim_is_rat_present_3GPP_capability_list(req_rat_list, 
                SYS_RAT_TDS_RADIO_ACCESS, NULL))
      {
        /* Check if WCDMA is in the list as well */
        if (reg_sim_is_rat_present_3GPP_capability_list(req_rat_list, 
                SYS_RAT_UMTS_RADIO_ACCESS, NULL))
        {
          /* Both TDSCDMA and WCDMA are present in the rat priority list. */
          /* Determine the initial rat based on the based on the TDSCDMA 
          ** bit in the NV_RPLMNAcT 
          */
          reg_nv_read_rplmnact(&rplmnact);

          if ( rplmnact.act[0] & TDSCDMA_ACT_BIT )
          {
            /* TDSCDMA is the last registered RAT*/
            rplmn_rat_search_order = REG_SIM_MODE_PREF_TDSCDMA_RADIO_ACCESS_PREFERRED;
          }
        }
        else
        {   /* Only TDSCDMA is present in the list, set the search order to TD*/
            rplmn_rat_search_order = REG_SIM_MODE_PREF_TDSCDMA_RADIO_ACCESS_PREFERRED;
        }
      }      
    }
#endif
  }

  switch (rplmn_rat_search_order)
  {
    case REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED:
      MSG_HIGH_DS_0(REG_SUB, "=REG= LAST RPLMN RAT GSM ");
      break;

    case REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED:
      MSG_HIGH_DS_0(REG_SUB, "=REG= LAST RPLMN RAT UMTS ");
      break;

case REG_SIM_MODE_PREF_TDSCDMA_RADIO_ACCESS_PREFERRED:
      MSG_HIGH_DS_0(REG_SUB, "=REG= LAST RPLMN RAT TDSCDMA ");
      break;

#ifdef FEATURE_LTE
    case REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED:
      MSG_HIGH_DS_0(REG_SUB,"=REG= LAST RPLMN RAT LTE ");
      break;
#endif

    case REG_SIM_MODE_PREF_UNDEFINED:
    default:
      MSG_HIGH_DS_0(REG_SUB, "=REG= LAST RPLMN RAT UNDEFINED");
      break;
  }
#ifndef FEATURE_RAT_PRIORITY_LIST
  return reg_sim_combined_rat_search_order
  (
    rplmn_rat_search_order,
    req_mode_pref
  );
#else
  reg_sim_combined_rat_search_order
  (
    rplmn_rat_search_order,
    req_rat_list
  );
#endif 
}
/*==============================================================================

FUNCTION NAME

  reg_sim_is_preferred_rat

==============================================================================*/
boolean reg_sim_is_preferred_rat(reg_sim_plmn_w_act_s_type plmn_act_w,sys_radio_access_tech_e_type  rat)
{

    sys_radio_access_tech_e_type pplmn_supported_rat_order[SYS_RAT_MAX];
    boolean flag = FALSE;
    uint8 number_of_pplmn_supported_rats = 0;
    uint8 i=0;
    number_of_pplmn_supported_rats = reg_mode_get_rat_order(
                                                           #ifndef FEATURE_RAT_PRIORITY_LIST        
                                                              reg_mode_mode_pref_get(),
                                                           #else
                                                              reg_mode_rat_pri_list_get(),
                                                           #endif
                                                             pplmn_supported_rat_order,
                                                             plmn_act_w,
                                                             NULL
                                                           );

   for(i=0;i< number_of_pplmn_supported_rats; i++)
   {
     if( pplmn_supported_rat_order[i] == rat)
     {
       flag = TRUE;
       break;
     }                
   }
   return flag;

}
/*==============================================================================

FUNCTION NAME

  reg_sim_is_plmn_not_preferred_plmn

==============================================================================*/

boolean reg_sim_is_plmn_not_preferred_plmn
(
  sys_plmn_id_s_type plmn,
  sys_radio_access_tech_e_type rat
)
{
  reg_sim_plmn_w_act_s_type *preferred_plmn_list_p = NULL;
  int32 preferred_plmn_list_length = 0;
  int32 operator_preferred_plmn_list_length = 0;
  int32 user_preferred_plmn_list_length = 0;
  int32 total_pplmn_length=0;
  int32 i=0;
  boolean flag = TRUE;
  if(
#ifdef FEATURE_EQUIVALENT_HPLMN
	  reg_sim_is_ehplmn(plmn) ||
#endif
	  sys_plmn_match(plmn,reg_sim_hplmn_get())
	)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= PLMN is either HPLMN or EHPLMN");
    return FALSE;
  }
    /*
  ** Sort the Preferred PLMNs.
  */
  preferred_plmn_list_p = reg_sim_read_preferred_plmn_list
                                       (
                                         &preferred_plmn_list_length,
                                         &operator_preferred_plmn_list_length,
                                         &user_preferred_plmn_list_length,
                                         TRUE
                                       );
  if ( preferred_plmn_list_p != NULL)
  {
    /* 
    ** If operator and user preferred plmn lists lengths are zero then
    ** we must have read the preffered plmn list from the SIM.
    */
    if((operator_preferred_plmn_list_length + user_preferred_plmn_list_length) > 0)
    {
      total_pplmn_length = operator_preferred_plmn_list_length + user_preferred_plmn_list_length;
    }
    else
    {
      total_pplmn_length = preferred_plmn_list_length;
    }
    for (i = 0; i < total_pplmn_length; i++)
    {
      //SKIP the undefined plmn
      if(sys_plmn_id_is_undefined(preferred_plmn_list_p[i].plmn))
      {
        continue;
      }
      if ( sys_plmn_match(preferred_plmn_list_p[i].plmn, plmn) &&
           reg_sim_is_preferred_rat(preferred_plmn_list_p[i],rat) == TRUE)
      {
        MSG_HIGH_DS_0(REG_SUB, "=REG= PLMN is either OPLMN or UPLMN");
        flag = FALSE;
        break;
      }
    }
  }

#ifdef FEATURE_MODEM_HEAP
    modem_mem_free( preferred_plmn_list_p , MODEM_MEM_CLIENT_NAS);
#else
    gs_free(preferred_plmn_list_p);
#endif
    preferred_plmn_list_p = NULL;

  return flag;

}
/*==============================================================================

FUNCTION NAME

  reg_sim_write_rplmn_rat

==============================================================================*/

void reg_sim_write_rplmn_rat
(
  sys_radio_access_tech_e_type rat
)
{
  reg_sim_plmn_w_act_s_type rplmn_w_act;
  nv_rplmnact_type          rplmnact = {{0, 0}};
#ifdef FEATURE_TDSCDMA
  nv_rplmnact_type          rplmnact_cache;
#endif
  memset(&rplmn_w_act,0x00,sizeof(reg_sim_plmn_w_act_s_type));

#ifdef FEATURE_SGLTE
  /* Do not update RPLMN NV ACT when in dual stack and curretn stack is CS stack */
  if ((rat != SYS_RAT_NONE) && (REG_SGLTE_DUAL_STACK && (reg_as_id == reg_cs_stack_id)))
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= Not writing RPLMN ACT in NV for CS Stack");
    return;
  }
#endif
  /*
  ** Update the value stored in RAM for use if necessary.
  */
  if (rat == SYS_RAT_GSM_RADIO_ACCESS)
  {
    reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_GSM_RADIO_ACCESS_PREFERRED;
  }
  else if (rat == SYS_RAT_UMTS_RADIO_ACCESS)
  {
    reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_UMTS_RADIO_ACCESS_PREFERRED;
  }
  else if (rat == SYS_RAT_TDS_RADIO_ACCESS)
  {
    reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_TDSCDMA_RADIO_ACCESS_PREFERRED;
  }
#ifdef FEATURE_LTE
  else if (rat == SYS_RAT_LTE_RADIO_ACCESS)
  {
    reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_LTE_RADIO_ACCESS_PREFERRED;
  }
#endif
  else
  {
    reg_sim_rplmn_rat_search_order = REG_SIM_MODE_PREF_UNDEFINED;
  }

  /*
  ** Store the value of RPLMNACT in NVRAM.
  */
  if (rat == SYS_RAT_GSM_RADIO_ACCESS)
  {
    rplmnact.act[1] = 0x80;
    rplmnact.act[0] = 0x00;
  }
  else if (rat == SYS_RAT_UMTS_RADIO_ACCESS)
  {
    rplmnact.act[0] = 0x80;
    rplmnact.act[1] = 0x00;
  }
  else if (rat == SYS_RAT_TDS_RADIO_ACCESS)
  {
    rplmnact.act[0] = 0x80;
    /* Set the TDSCMA bit as well */
    rplmnact.act[0] |= TDSCDMA_ACT_BIT;
    rplmnact.act[1] = 0x00;
  }
#ifdef FEATURE_LTE  
  else if (rat == SYS_RAT_LTE_RADIO_ACCESS)
  {
    rplmnact.act[0] = 0x40;
    rplmnact.act[1] = 0x00;
  }
#endif

#ifdef FEATURE_TDSCDMA
  /* As an optimization, if the RPLMN is GSM or LTE, preserve
  ** the existing value carried by the TDSCDMA ACT bit in the NV to indicate 
  ** if the coverage of UMTS is TDSCDMA or WCDMA
  */
  if ((rat == SYS_RAT_GSM_RADIO_ACCESS) || (rat == SYS_RAT_LTE_RADIO_ACCESS))
  {
     reg_nv_read_rplmnact(&rplmnact_cache);
     rplmnact.act[0] |= rplmnact_cache.act[0] & TDSCDMA_ACT_BIT;
  }
#endif

  reg_nv_write_rplmnact(rplmnact);

  /*
  ** RPLMNAcT EF is not defined for a SIM.
  */
  if ( reg_sim_card_mode == MMGSDI_APP_SIM )
  {
  }
  /*
  ** Write the RPLMNAcT EF to the USIM.
  */
  else if ( reg_sim_card_mode == MMGSDI_APP_USIM )
  {
    if (rat == SYS_RAT_GSM_RADIO_ACCESS)
    {
      rplmn_w_act.act[1] = 0x80;
      rplmn_w_act.act[0] = 0x00;
    }
    else if (rat == SYS_RAT_UMTS_RADIO_ACCESS)
    {
      rplmn_w_act.act[0] = 0x80;
      rplmn_w_act.act[1] = 0x00;
    }
    else if (rat == SYS_RAT_TDS_RADIO_ACCESS)
    {
      rplmn_w_act.act[0] = 0x80;
      rplmn_w_act.act[1] = 0x00;
    }
#ifdef FEATURE_LTE    
    else if (rat == SYS_RAT_LTE_RADIO_ACCESS)
    {
      rplmn_w_act.act[0] = 0x40;
      rplmn_w_act.act[1] = 0x00;
    }  
#endif    
#ifndef FEATURE_DISABLE_SIM_RPLMNACT
    (void)reg_sim_write
    (
      MMGSDI_USIM_RPLMNACT,
      (byte *) rplmn_w_act.act,
      REG_SIM_ACT_LENGTH
    );
#endif
  }
  else
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
  }
}


/*==============================================================================

FUNCTION NAME

  reg_sim_read_preferred_plmn_list

DESCRIPTION

  Function that reads either the USER and OPERATOR Preferred PLMN List or PREFFERED PLMN list from the SIM.

RETURN VALUE

  reg_sim_plmn_w_act_s_type* pointer to the Preferred PLMN List 
  Length of USER and OPERATOR Preferred PLMN List or PREFFERED PLMN list.

==============================================================================*/

reg_sim_plmn_w_act_s_type* reg_sim_read_preferred_plmn_list
(
  int32 *preferred_plmn_length,
  int32 *operator_pplmn_length,
  int32 *user_pplmn_length,
  boolean read_upplmn
)
{
  reg_sim_plmn_w_act_s_type* preferred_plmn_list_p = NULL;
  mmgsdi_file_enum_type                  operator_pplmn_sim_filename;
  mmgsdi_file_enum_type                  user_pplmn_sim_filename;
  int32 file_size = 0;

  *preferred_plmn_length = 0;
  *operator_pplmn_length = 0;
  *user_pplmn_length = 0;
  /*
  ** Verify the SIM card mode before attempting to read data.
  */
  if ( ( reg_sim_card_mode != MMGSDI_APP_SIM ) &&
       ( reg_sim_card_mode != MMGSDI_APP_USIM ) )
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return NULL;
  }

  /*
  ** Get the SIM file name and read the lengths of operator and user 
  ** preffered plmn list
  */
  if ( reg_sim_card_mode == MMGSDI_APP_SIM )
  {
    operator_pplmn_sim_filename = MMGSDI_GSM_OPLMNWACT;
    user_pplmn_sim_filename = MMGSDI_GSM_PLMNWACT;
  }
  else
  {
    operator_pplmn_sim_filename = MMGSDI_USIM_OPLMNWACT;
    user_pplmn_sim_filename = MMGSDI_USIM_PLMNWACT;
  }
  if(mmgsdi_session_read_cache_file_size (reg_sim_mmgsdi_info.session_id,
                                          operator_pplmn_sim_filename,
                                          &file_size) != MMGSDI_SUCCESS)
  {
    MSG_HIGH_DS_0(REG_SUB, "=REG= could not read OPLMN list from SIM");    
    *operator_pplmn_length = 0;
  }
  else if(file_size > 0)
  {
    *operator_pplmn_length = (file_size/REG_SIM_PLMN_W_ACT_LENGTH);
  }

#if defined(FEATURE_LTE) && defined(FEATURE_MMSS_3_1)
  /*If UE is multimode capable and ignore uplmn is TRUE then do not
      read UPLMN list */
  if ((read_upplmn == TRUE) && ((reg_nv_gcf_flag_get() == TRUE) || 
          !(reg_nv_read_ignore_uplmn() == TRUE
          && reg_mode_is_multimode_enabled(reg_mode_rat_pri_list_get()))))
#endif
  {
    if(mmgsdi_session_read_cache_file_size (reg_sim_mmgsdi_info.session_id,
                                            user_pplmn_sim_filename,
                                            &file_size) != MMGSDI_SUCCESS)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= could not read UPLMN list from SIM");    
      *user_pplmn_length = 0;
    }
    else if((read_upplmn == TRUE) && (file_size > 0))
    {
      *user_pplmn_length = (file_size/REG_SIM_PLMN_W_ACT_LENGTH);
    }
  }

  /*
  ** Allocate the memory to read the user and operator preffered plmn list.
  */
  if((*operator_pplmn_length + *user_pplmn_length) > 0)
  {
    if((*operator_pplmn_length + *user_pplmn_length) > 
                                REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH)
    {
      if(*user_pplmn_length >= REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH)
      {
        *user_pplmn_length = REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH;
        *operator_pplmn_length = 0;
      }
      else
      {
        *operator_pplmn_length = 
                   REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH - *user_pplmn_length;
      }
    }
    file_size = *operator_pplmn_length + *user_pplmn_length;
    file_size = file_size * REG_SIM_PLMN_W_ACT_LENGTH;
#ifdef FEATURE_MODEM_HEAP
    preferred_plmn_list_p = (reg_sim_plmn_w_act_s_type*)modem_mem_alloc((unsigned int)file_size, MODEM_MEM_CLIENT_NAS);
#else
    preferred_plmn_list_p = (reg_sim_plmn_w_act_s_type*) gs_alloc(file_size);
#endif
    if(preferred_plmn_list_p == NULL)
    {
      MSG_ERROR_DS_0(REG_SUB, "=REG= Could not allocate the memory");    
      return NULL;
    }

    /*
    ** Attempt to read the User Preferred PLMN List.
    */
    if(*user_pplmn_length > 0)
    {
      if(reg_sim_read (user_pplmn_sim_filename, (byte*)preferred_plmn_list_p,
                     (*user_pplmn_length) * REG_SIM_PLMN_W_ACT_LENGTH) != TRUE)
      {
        *user_pplmn_length = 0;
      }
    }

    /*
    ** Attempt to read the Operator Preferred PLMN List.
    */
    if(*operator_pplmn_length > 0)
    {
      if(reg_sim_read (operator_pplmn_sim_filename, 
                           (byte*)(preferred_plmn_list_p + *user_pplmn_length), 
                           ((*operator_pplmn_length) * REG_SIM_PLMN_W_ACT_LENGTH)) != TRUE)
      {
        *operator_pplmn_length = 0;
      }
    }
  }


  if((*operator_pplmn_length <= 0) &&
     (*user_pplmn_length <= 0) &&
     ( reg_sim_card_mode == MMGSDI_APP_SIM )
    )
  {
    /*
    ** The Preferred PLMN With Access Technology Lists were not read
    ** successfully so attempt to read the Preferred PLMN List.
    */
    if(mmgsdi_session_read_cache_file_size (reg_sim_mmgsdi_info.session_id,
                                            MMGSDI_GSM_PLMN,
                                            &file_size) != MMGSDI_SUCCESS)
    {
      MSG_HIGH_DS_0(REG_SUB, "=REG= could not read preferred PLMN list from SIM");
      *preferred_plmn_length = 0;
    }
    else
    {
      *preferred_plmn_length = (file_size/REG_SIM_PLMN_LENGTH);
      if(*preferred_plmn_length > REG_SIM_PREFERRED_PLMN_LIST_LENGTH)
      {
        *preferred_plmn_length = REG_SIM_PREFERRED_PLMN_LIST_LENGTH;
      }
#ifdef FEATURE_MODEM_HEAP
      preferred_plmn_list_p = (reg_sim_plmn_w_act_s_type*)modem_mem_alloc((unsigned int)((*preferred_plmn_length) * REG_SIM_PLMN_W_ACT_LENGTH),
                                                                        MODEM_MEM_CLIENT_NAS);
#else
      preferred_plmn_list_p = (reg_sim_plmn_w_act_s_type*) gs_alloc(file_size);
#endif
      if(preferred_plmn_list_p == NULL)
      {
        MSG_ERROR_DS_0(REG_SUB, "=REG= Could not allocate the memory");    
        return NULL;
      }
      /*
      ** Attempt to read the Preferred PLMN List.
      */
      if(reg_sim_read_pplmn_list( 
                  (reg_sim_plmn_w_act_s_type *)preferred_plmn_list_p,
                 *preferred_plmn_length) != TRUE)
      {
        *preferred_plmn_length = 0;
      }
    }
  }

  /*
  ** Output Preferred PLMN List log packet.
  */

  if(preferred_plmn_list_p != NULL)
  {
	reg_log_umts_nas_pplmn_list(preferred_plmn_list_p,
								(uint16)(*operator_pplmn_length),
								(uint16)(*user_pplmn_length),
								(uint16)(*preferred_plmn_length),
								(sys_modem_as_id_e_type)reg_sub_id);
  }
  return preferred_plmn_list_p;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_read_fplmn_list

==============================================================================*/

boolean reg_sim_read_fplmn_list
(
  reg_sim_plmn_list_s_type* fplmn_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type   as_id
#endif
)
{
  reg_sim_plmn_list_s_type fplmn_list;
  int32                    file_length = 0;
  boolean                  status = FALSE;
  int32                    file_size;
  uint32                   i,j = 0;

  memset(fplmn_list_p, 0xFF, sizeof(reg_sim_plmn_list_s_type));
  
  /*
  ** Initialize the length of the Forbidden PLMN List.
  */
  fplmn_list.length = 0;
  fplmn_list_p->length = 0;

#ifdef FEATURE_DUAL_SIM

  /*
  ** Verify the SIM card mode before attempting to read data.
  */
  if ( ( reg_sim_card_mode_sim[as_id]!= MMGSDI_APP_SIM ) &&
       ( reg_sim_card_mode_sim[as_id] != MMGSDI_APP_USIM ) )
  {
    return FALSE;
  }

  /*
  ** Read the attributes for the Forbidden PLMN List.
  */
  if(mmgsdi_session_read_cache_file_size 
      (
        reg_sim_mmgsdi_info_sim[as_id].session_id,
        ( reg_sim_card_mode_sim[as_id] == MMGSDI_APP_SIM ? MMGSDI_GSM_FPLMN : MMGSDI_USIM_FPLMN),
        &file_length
      ) == MMGSDI_SUCCESS)

#else

  /*
  ** Verify the SIM card mode before attempting to read data.
  */
  if ( ( reg_sim_card_mode != MMGSDI_APP_SIM ) &&
       ( reg_sim_card_mode != MMGSDI_APP_USIM ) )
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= SIM card unknown mode (GSM or USIM)");
    return FALSE;
  }

  /*
  ** Read the attributes for the Forbidden PLMN List.
  */
  if(mmgsdi_session_read_cache_file_size 
      (
        reg_sim_mmgsdi_info.session_id,
        ( reg_sim_card_mode == MMGSDI_APP_SIM ? MMGSDI_GSM_FPLMN : MMGSDI_USIM_FPLMN),
        &file_length
      ) == MMGSDI_SUCCESS)

#endif
  {
    /*
    ** Set the maximum file size for the Forbidden PLMN List.
    */
    file_size = MIN(file_length,
                    SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH *
                    REG_SIM_PLMN_LENGTH);

#ifdef FEATURE_DUAL_SIM
    /*
    ** Read the Forbidden PLMN List.
    */
    status = reg_sim_per_subs_read
    (
      (reg_sim_card_mode_sim[as_id] == MMGSDI_APP_SIM ? MMGSDI_GSM_FPLMN : MMGSDI_USIM_FPLMN),
      (byte *)fplmn_list.plmn,
      file_size,
      as_id
    );
    
#else

    /*
    ** Read the Forbidden PLMN List.
    */
    status = reg_sim_read
    (
      (reg_sim_card_mode == MMGSDI_APP_SIM ? MMGSDI_GSM_FPLMN : MMGSDI_USIM_FPLMN),
      (byte *)fplmn_list.plmn,
      file_size
    );
#endif

    if ( status )
    {
      fplmn_list.length = (uint32)(file_size / REG_SIM_PLMN_LENGTH);
    }
    else
    {
      MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= Read FPLMN list failed");
      fplmn_list.length = 0;
    }
  }

  /*
    ** In case MMGSDI NV "MMGSDI_NV_SUPPORT_EXTENDED_FPLMN_ICC_I" is enabled, 
    ** REG removes holes(invalid FPLMN) from the FPLMN list read from SIM card
  */
  
  fplmn_list_p->length = fplmn_list.length;

  if(reg_nv_extended_fplmn_icc &&
#ifdef FEATURE_DUAL_SIM
     (reg_sim_card_mode_sim[as_id] == MMGSDI_APP_SIM)
#else
     (reg_sim_card_mode == MMGSDI_APP_SIM)
#endif
    )
  {
    for (i = 0; i < fplmn_list.length; i++)
    {
      if(!sys_plmn_id_is_undefined(fplmn_list.plmn[i]))
      {
        fplmn_list_p->plmn[j] = fplmn_list.plmn[i];
        j++;
      }
    }
  }
  else
  {
     /*
     ** Copy the local list to the output list.
     */
    for (i = 0; i < fplmn_list.length; i++)
    {
      fplmn_list_p->plmn[i] = fplmn_list.plmn[i];
    }
  }

  return ( status );
}


/*==============================================================================

FUNCTION NAME

  reg_sim_remove_plmn_from_fplmn_list

==============================================================================*/

boolean reg_sim_remove_plmn_from_fplmn_list
(
  sys_plmn_id_s_type rplmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  reg_sim_plmn_list_s_type fplmn_list;

  boolean status = FALSE;
  uint32  position = 0;
  boolean list_modified = FALSE;
  /*
  ** Verify that the RPLMN is defined before removing it from the
  ** Forbidden List.
  */
  if ( sys_plmn_id_is_undefined(rplmn) )
  {
    MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= Remove undefined PLMN from FPLMN list");
    return  list_modified;
  }

  /*
  ** Read the Forbidden PLMN List from the SIM.
  */
#ifdef FEATURE_DUAL_SIM
  status = reg_sim_read_fplmn_list(&fplmn_list,as_id);
#else
  status = reg_sim_read_fplmn_list(&fplmn_list);
#endif

  if ( status )
  {
    /*
    ** Search for the RPLMN in the Forbidden PLMN List.
    */
    if ( reg_sim_find_plmn_in_list( rplmn, &fplmn_list, &position ) )
    {
      /*
      ** If the RPLMN is found in the Forbidden PLMN List then undefine
      ** the PLMN ID at the position where the RPLMN was stored.
      */
      sys_plmn_undefine_plmn_id(&fplmn_list.plmn[position]);

      /*
     ** In case MMGSDI NV "MMGSDI_NV_SUPPORT_EXTENDED_FPLMN_ICC_I" is enabled, 
     ** REG removes holes(invalid FPLMN) from the FPLMN list before writing FPLMN list to card
     */

    if(reg_nv_extended_fplmn_icc &&
#ifdef FEATURE_DUAL_SIM
       (reg_sim_card_mode_sim[as_id] == MMGSDI_APP_SIM)
#else
       (reg_sim_card_mode == MMGSDI_APP_SIM)
#endif
      )
     {
        for (; (position < fplmn_list.length-1) && (position < REG_SIM_PLMN_LIST_LENGTH-1); position++)
        {
          fplmn_list.plmn[position] = fplmn_list.plmn[position+1];
        }
		
		sys_plmn_undefine_plmn_id(&fplmn_list.plmn[position]);
     }

      /*
      ** Write the updated Forbidden PLMN List to the SIM.
      */
#ifdef FEATURE_DUAL_SIM
      status = reg_sim_write_fplmn_list(&fplmn_list,as_id);
#else
      status = reg_sim_write_fplmn_list(&fplmn_list);
#endif

      /*
      ** Send a Log packet that FPLMN is removed from FPLMN List
      ** FALSE means this FPLMN is removed from FPLMN List.
      */  
#ifdef FEATURE_DUAL_SIM
      reg_log_umts_nas_fplmn_list_update(rplmn, FALSE, as_id);
#else
      reg_log_umts_nas_fplmn_list_update(rplmn, FALSE);
#endif
      list_modified = TRUE;
    }
#ifdef FEATURE_DUAL_SIM
    reg_log_umts_nas_fplmn_list(&fplmn_list,as_id);
#else
    reg_log_umts_nas_fplmn_list(&fplmn_list);
#endif
  }
  return list_modified;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_add_plmn_to_fplmn_list

==============================================================================*/

boolean reg_sim_add_plmn_to_fplmn_list
(
  sys_plmn_id_s_type plmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  reg_sim_plmn_list_s_type fplmn_list;

  sys_plmn_id_s_type undefined_plmn;
  boolean            status = FALSE;
  uint32             position = 0;
  int32              i;
  boolean list_modified = FALSE;

  sys_plmn_undefine_plmn_id(&undefined_plmn);

  /*
  ** Verify that the RPLMN is defined before adding it to the
  ** Forbidden PLMN List.
  */
  if ( sys_plmn_id_is_undefined(plmn) )
  {
    MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= Add undefined PLMN to FPLMN list");
    return list_modified;;
  }

  /*
  ** If the PLMN matches the HPLMN then do not add it to the
  ** Forbidden PLMN List.
  */
#ifdef FEATURE_DUAL_SIM
#ifdef FEATURE_EQUIVALENT_HPLMN
  if( reg_sim_per_subs_is_ehplmn(plmn,as_id) )
#else
  if ( sys_plmn_match(reg_sim_per_subs_read_hplmn(as_id), plmn) )
#endif

#else

#ifdef FEATURE_EQUIVALENT_HPLMN
  if( reg_sim_is_ehplmn(plmn) )
#else
  if ( sys_plmn_match(reg_sim_read_hplmn(), plmn) )
#endif
#endif /*FEATURE_DUAL_SIM*/
  {
    MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= Add HPLMN to FPLMN list");
    return list_modified;;
  }
#ifdef FEATURE_DUAL_SIM
  if(reg_sim_check_if_t3245_behavior_enabled(as_id) &&
     (reg_is_t3245_timer_active(as_id) == FALSE))
  {
    reg_timers_start_t3245_timer(REG_T3245_DEFUALT_VALUE,as_id);
  }
#else
  if(reg_sim_check_if_t3245_behavior_enabled() &&
     (reg_is_t3245_timer_active() == FALSE))
  {
    reg_timers_start_t3245_timer(REG_T3245_DEFUALT_VALUE);
  }
#endif

  /*
  ** Read the Forbidden PLMN List from the SIM.
  */
#ifdef FEATURE_DUAL_SIM
  status = reg_sim_read_fplmn_list(&fplmn_list,as_id);
#else
  status = reg_sim_read_fplmn_list(&fplmn_list);
#endif

  if ( status && fplmn_list.length > 0)
  {
    /*
    ** Check to see if the PLMN is already contained in the
    ** Forbidden PLMN List.
    */
    if ( !reg_sim_find_plmn_in_list( plmn, &fplmn_list, &position ) )
    {
      /*
      ** The PLMN is not already contained in the Forbidden PLMN
      ** List so search for the first empty position in the list.
      */
      if (!reg_sim_find_plmn_in_list( undefined_plmn,
                                      &fplmn_list,
                                      &position))
      {
        /*
        ** The Forbidden PLMN List is full so the first (oldest) PLMN
        ** is removed from the top of the list and the new PLMN is added
        ** to the bottom of the list.
        */
        for (i = 0; i < (int32)(fplmn_list.length - 1); i++)
        {
          fplmn_list.plmn[i] = fplmn_list.plmn[i + 1];
        }

        position = (fplmn_list.length - 1);
      }

      /*
      ** Copy the PLMN into the empty position.
      */
      fplmn_list.plmn[position] = plmn;

      /*
      ** Write the updated Forbidden PLMN List to the SIM.
      */
#ifdef FEATURE_DUAL_SIM
      status = reg_sim_write_fplmn_list(&fplmn_list,as_id);
#else
      status = reg_sim_write_fplmn_list(&fplmn_list);
#endif

      /*
      ** Send a log packet that PLMN is added in FPLMN List.
      ** TRUE means this PLMN is added in FPLMN List.
      */
      if (status)
      {
#ifdef FEATURE_DUAL_SIM
        reg_log_umts_nas_fplmn_list_update(plmn, TRUE, as_id);
#else
        reg_log_umts_nas_fplmn_list_update(plmn, TRUE);
#endif
      }
      list_modified = TRUE;
    }
#ifdef FEATURE_DUAL_SIM
    reg_log_umts_nas_fplmn_list(&fplmn_list,as_id);
#else
    reg_log_umts_nas_fplmn_list(&fplmn_list);
#endif
  }
  return list_modified;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_plmn_forbidden

==============================================================================*/
boolean reg_sim_plmn_forbidden
(
  sys_plmn_id_s_type      plmn
)
{
  boolean                  status = FALSE;
 #ifdef FEATURE_DUAL_SIM
  MSG_ERROR_DS_0(REG_SUB, "=REG= DUAL SIM: Wrong API called - reg_sim_plmn_forbidden() ");
 #else
  reg_sim_plmn_list_s_type fplmn_list;
  uint32                   position = 0;
  boolean                  plmn_id_is_undefined = FALSE;
  boolean                  mnc_includes_pcs_digit = FALSE;
  uint32                   mcc = 0;
  uint32                   mnc = 0;
  uint32  home_mcc;
  uint32  home_mnc;
  boolean home_mnc_includes_pcs_digit;
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32 i;
#endif

  boolean voice_roaming_disabled = TRUE; 
  
  /*
  ** Verify that the PLMN is defined before checking to see if it is
  ** included in the Forbidden PLMN List.
  */
  if ( sys_plmn_id_is_undefined(plmn) )
  {
    return FALSE;
  }
if(reg_nv_read_voice_roaming() == FALSE)
  {
    sys_plmn_get_mcc_mnc
    (
      plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
#ifdef FEATURE_EQUIVALENT_HPLMN
    NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
    for(i =0; i < reg_sim_ehplmn_list.length; i++)
    {
      sys_plmn_get_mcc_mnc
      (
        reg_sim_ehplmn_list.plmn[i],
        &plmn_id_is_undefined,
        &home_mnc_includes_pcs_digit,
        &home_mcc,
        &home_mnc
      );
      if(sys_mcc_same_country(mcc, home_mcc))
      {
        voice_roaming_disabled = FALSE;
        break;
      }
    }    
    NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
#else
    NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
    sys_plmn_get_mcc_mnc
    (
      reg_sim_hplmn,
      &plmn_id_is_undefined,
      &home_mnc_includes_pcs_digit,
      &home_mcc,
      &home_mnc
    );
    if(sys_mcc_same_country(mcc, home_mcc))
    {
      voice_roaming_disabled = FALSE;
    }
    NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);
#endif
    if(voice_roaming_disabled)
    {
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_2(REG_SUB, "=REG= International roaming disabled, CS+PS not allowed on PLMN %03lu-%03lu", mcc,mnc);
      }
      else
      {
        MSG_HIGH_DS_2(REG_SUB, "=REG= International roaming disabled, CS+PS not allowed on PLMN %03lu-%02lu", mcc,mnc);
      }
      return TRUE;
    }
  }

  /*
  ** Read the Forbidden PLMN List from the SIM.
  */
  status = reg_sim_read_fplmn_list(&fplmn_list);

  if ( status )
  {
    status = reg_sim_find_plmn_in_list( plmn, &fplmn_list, &position );
    if(status)
    {
      sys_plmn_get_mcc_mnc
      (
        plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );

      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_2(REG_SUB,"=REG= PLMN %03lu-%03lu is Forbidden", mcc, mnc);
      }
      else
      {
       MSG_HIGH_DS_2(REG_SUB,"=REG= PLMN %03lu- %02lu is Forbidden", mcc, mnc);
      }
    }
  }
#endif
 return status;
}

/*==============================================================================

FUNCTION NAME

  reg_sim_int_voice_forbidden

==============================================================================*/
boolean reg_sim_int_voice_forbidden
(
  sys_plmn_id_s_type      plmn
)
{
  boolean                  status = FALSE;
 #ifdef FEATURE_DUAL_SIM
  MSG_ERROR_DS_0(REG_SUB, "=REG= DUAL SIM: Wrong API called - reg_sim_int_voice_forbidden() ");
 #else
  boolean                  plmn_id_is_undefined = FALSE;
  boolean                  mnc_includes_pcs_digit = FALSE;
  uint32                   mcc = 0;
  uint32                   mnc = 0;
  uint32  home_mcc;
  uint32  home_mnc;
  boolean home_mnc_includes_pcs_digit;
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32 i;
#endif

  boolean voice_roaming_disabled = TRUE; 
  
  /*
  ** Verify that the PLMN is defined before checking to see if it is
  ** included in the Forbidden PLMN List.
  */
  if ( sys_plmn_id_is_undefined(plmn) )
  {
    return FALSE;
  }
  if(reg_nv_read_voice_roaming() == FALSE)
  {
    sys_plmn_get_mcc_mnc
    (
      plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
#ifdef FEATURE_EQUIVALENT_HPLMN
    NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
    for(i =0; i < reg_sim_ehplmn_list.length; i++)
    {
      sys_plmn_get_mcc_mnc
      (
        reg_sim_ehplmn_list.plmn[i],
        &plmn_id_is_undefined,
        &home_mnc_includes_pcs_digit,
        &home_mcc,
        &home_mnc
      );
      if(sys_mcc_same_country(mcc, home_mcc))
      {
        voice_roaming_disabled = FALSE;
        break;
      }
    }    
    NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
#else
    NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
    sys_plmn_get_mcc_mnc
    (
      reg_sim_hplmn,
      &plmn_id_is_undefined,
      &home_mnc_includes_pcs_digit,
      &home_mcc,
      &home_mnc
    );
    if(sys_mcc_same_country(mcc, home_mcc))
    {
      voice_roaming_disabled = FALSE;
    }
    NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);
#endif
    if(voice_roaming_disabled)
    {
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_2(REG_SUB, "=REG= International roaming disabled, CS+PS not allowed on PLMN %03lu-%03lu", mcc,mnc);
      }
      else
      {
        MSG_HIGH_DS_2(REG_SUB, "=REG= International roaming disabled, CS+PS not allowed on PLMN %03lu-%02lu", mcc,mnc);
      }
      return TRUE;
    }
  }
#endif
 return status;
}

#ifdef FEATURE_DUAL_SIM
boolean reg_sim_per_subs_plmn_forbidden
(
  sys_plmn_id_s_type      plmn,
  sys_modem_as_id_e_type sub_id
)
{

  reg_sim_plmn_list_s_type fplmn_list;
  boolean                  status = FALSE;
  uint32                   position = 0;
  boolean                  plmn_id_is_undefined = FALSE;
  boolean                  mnc_includes_pcs_digit = FALSE;
  uint32                   mcc = 0;
  uint32                   mnc = 0;
  uint32  home_mcc;
  uint32  home_mnc;
  boolean home_mnc_includes_pcs_digit;
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32 i;
#endif

  boolean voice_roaming_disabled = TRUE; 

  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= reg_sim_per_subs_plmn_forbidden - Invalid SUB ID: %d", sub_id+1);
    return FALSE;
  }

  /*
  ** Verify that the PLMN is defined before checking to see if it is
  ** included in the Forbidden PLMN List.
  */
  if ( sys_plmn_id_is_undefined(plmn) )
  {
    return FALSE;
  }

  if(reg_nv_read_voice_roaming(sub_id) == FALSE)
  {
    sys_plmn_get_mcc_mnc
    (
      plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
#ifdef FEATURE_EQUIVALENT_HPLMN
    NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
    for(i =0; i < reg_sim_ehplmn_list_sim[sub_id].length; i++)
    {
      sys_plmn_get_mcc_mnc
      (
        reg_sim_ehplmn_list_sim[sub_id].plmn[i],
        &plmn_id_is_undefined,
        &home_mnc_includes_pcs_digit,
        &home_mcc,
        &home_mnc
      );
      if(sys_mcc_same_country(mcc, home_mcc))
      {
        voice_roaming_disabled = FALSE;
        break;
      }
    }    
    NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
#else
    NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
    sys_plmn_get_mcc_mnc
    (
      reg_sim_hplmn_sim[sub_id],
      &plmn_id_is_undefined,
      &home_mnc_includes_pcs_digit,
      &home_mcc,
      &home_mnc
    );
    if(sys_mcc_same_country(mcc, home_mcc))
    {
      voice_roaming_disabled = FALSE;
    }
    NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);
#endif
    if(voice_roaming_disabled)
    {
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_2(sub_id+1, "=REG= International roaming disabled, CS+PS not allowed on PLMN %03lu-%03lu", mcc,mnc);
      }
      else
      {
        MSG_HIGH_DS_2(sub_id+1, "=REG= International roaming disabled, CS+PS not allowed on PLMN %03lu-%02lu", mcc,mnc);
      }
      return TRUE;
    }
  }
  /*
  ** Read the Forbidden PLMN List from the SIM.
  */
  status = reg_sim_read_fplmn_list(&fplmn_list,sub_id);

  if ( status )
  {
    status = reg_sim_find_plmn_in_list( plmn, &fplmn_list, &position );
    if(status)
    {
      sys_plmn_get_mcc_mnc
      (
        plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );

      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_2(REG_SUB,"=REG= PLMN %03lu-%03lu is Forbidden", mcc, mnc);
      }
      else
      {
       MSG_HIGH_DS_2(REG_SUB,"=REG= PLMN %03lu- %02lu is Forbidden", mcc, mnc);
      }
    }
  }

  return status;
}

/*==============================================================================

FUNCTION NAME

  reg_sim_per_subs_int_voice_forbidden

==============================================================================*/
boolean reg_sim_per_subs_int_voice_forbidden
(
  sys_plmn_id_s_type      plmn,
  sys_modem_as_id_e_type sub_id
)
{
  boolean                  status = FALSE;
  boolean                  plmn_id_is_undefined = FALSE;
  boolean                  mnc_includes_pcs_digit = FALSE;
  uint32                   mcc = 0;
  uint32                   mnc = 0;
  uint32  home_mcc;
  uint32  home_mnc;
  boolean home_mnc_includes_pcs_digit;
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32 i;
#endif

  boolean voice_roaming_disabled = TRUE; 

  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= reg_sim_per_subs_int_voice_forbidden - Invalid SUB ID: %d", sub_id+1);
    return FALSE;
  }

  /*
  ** Verify that the PLMN is defined before checking to see if it is
  ** included in the Forbidden PLMN List.
  */
  if ( sys_plmn_id_is_undefined(plmn) )
  {
    return FALSE;
  }

  if(reg_nv_read_voice_roaming(sub_id) == FALSE)
  {
    sys_plmn_get_mcc_mnc
    (
      plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
#ifdef FEATURE_EQUIVALENT_HPLMN
    NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
    for(i =0; i < reg_sim_ehplmn_list_sim[sub_id].length; i++)
    {
      sys_plmn_get_mcc_mnc
      (
        reg_sim_ehplmn_list_sim[sub_id].plmn[i],
        &plmn_id_is_undefined,
        &home_mnc_includes_pcs_digit,
        &home_mcc,
        &home_mnc
      );
      if(sys_mcc_same_country(mcc, home_mcc))
      {
        voice_roaming_disabled = FALSE;
        break;
      }
    }    
    NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
#else
    NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
    sys_plmn_get_mcc_mnc
    (
      reg_sim_hplmn_sim[sub_id],
      &plmn_id_is_undefined,
      &home_mnc_includes_pcs_digit,
      &home_mcc,
      &home_mnc
    );
    if(sys_mcc_same_country(mcc, home_mcc))
    {
      voice_roaming_disabled = FALSE;
    }
    NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);
#endif
    if(voice_roaming_disabled)
    {
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_2(sub_id+1, "=REG= International roaming disabled, CS+PS not allowed on PLMN %03lu-%03lu", mcc,mnc);
      }
      else
      {
        MSG_HIGH_DS_2(sub_id+1, "=REG= International roaming disabled, CS+PS not allowed on PLMN %03lu-%02lu", mcc,mnc);
      }
      return TRUE;
    }
  }
  return status;
}

#endif
#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_sim_handle_backoff_fplmn_search_timer_expiry

==============================================================================*/
void reg_sim_handle_backoff_fplmn_search_timer_expiry()
{
  uint32 i = 0;
  uint32 index = 0;
  rex_timer_cnt_type min_timer_count = 0;
  /* enter the critical section here */ 
  rex_enter_crit_sect(&reg_backoff_fplmn_crit_sect);
  while(i < reg_sim_backoff_fplmn_list.length)
  {
    if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count == 0)
    {
      index = i;
      reg_sim_backoff_fplmn_list.length--;

      for (; index < reg_sim_backoff_fplmn_list.length; index++)
      {
        reg_sim_backoff_fplmn_list.plmn_timer_list[index] = reg_sim_backoff_fplmn_list.plmn_timer_list[index + 1];
      }
    }
    else
    {
      i++;
    }
  }

  for(i=0; i < reg_sim_backoff_fplmn_list.length; i++)
  {
    if(((min_timer_count == 0) ||
       (min_timer_count > reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count)) &&
       (reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count != REG_TIMERS_MAX_TIMER_VALUE))
    {
       min_timer_count = reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count;
    }
  }

  if(reg_sim_backoff_fplmn_list.length != 0 && min_timer_count != 0)
  {
     for(i=0; i < reg_sim_backoff_fplmn_list.length; i++)
     {
       if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count != REG_TIMERS_MAX_TIMER_VALUE)
       {
         reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count = 
            (reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count - min_timer_count);
       }
     }
     reg_timers_restart_backoff_fplmn_search_period_timer(min_timer_count,reg_sub_id);
  }
  /* leave the critical section here */
  rex_leave_crit_sect(&reg_backoff_fplmn_crit_sect);
  reg_sim_print_backoff_fplmn_list();
  reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_LTE_BACKOFF_FPLMN);
}

/*==============================================================================

FUNCTION NAME

  reg_sim_clear_backoff_fplmn_list

==============================================================================*/

void reg_sim_clear_backoff_fplmn_list()
{
  MSG_HIGH_DS_0(REG_SUB, "=REG= Clearing backoff plmn list");
  /* enter the critical section here */ 
  rex_enter_crit_sect(&reg_backoff_fplmn_crit_sect);

  reg_sim_backoff_fplmn_list.length = 0;
  reg_timers_stop_backoff_fplmn_search_period_timer(reg_sub_id);

  /* leave the critical section here */
  rex_leave_crit_sect(&reg_backoff_fplmn_crit_sect);
}

/*==============================================================================

FUNCTION NAME

  reg_sim_remove_plmn_from_backoff_fplmn_list

==============================================================================*/

boolean reg_sim_remove_plmn_from_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  sys_block_plmn_cause_e_type cause,
  sys_modem_as_id_e_type as_id
)
{
  uint32 index;
  boolean found = FALSE;

  /* enter the critical section here */ 
  rex_enter_crit_sect(&reg_backoff_fplmn_crit_sect);

  for(index = 0; index < reg_sim_backoff_fplmn_list.length; index++)
  {
    if(sys_plmn_match(reg_sim_backoff_fplmn_list.plmn_timer_list[index].plmn,plmn))
    {
      if((cause == SYS_BLOCK_PLMN_CAUSE_NONE) ||
         (reg_sim_backoff_fplmn_list.plmn_timer_list[index].cause == cause))
      {
        found = TRUE;
      }
      break;
    }
  }

  if(found == TRUE)
  {
    reg_sim_backoff_fplmn_list.length--;

    for (; index < reg_sim_backoff_fplmn_list.length; index++)
    {
      reg_sim_backoff_fplmn_list.plmn_timer_list[index] = reg_sim_backoff_fplmn_list.plmn_timer_list[index + 1];
    }

    if(reg_sim_backoff_fplmn_list.length == 0)
    {
      reg_timers_stop_backoff_fplmn_search_period_timer((reg_as_id_e_type)as_id);
    }
  }
  /* leave the critical section here */
  rex_leave_crit_sect(&reg_backoff_fplmn_crit_sect);

  reg_sim_print_backoff_fplmn_list();
  return found;
}

/*==============================================================================

FUNCTION NAME

  reg_sim_add_eplmn_to_backoff_fplmn_list

Description:
  This function adds EPLMN passed as the parameter to the backoff list. 
  The timer value passed is not the actual timer value but the current running timer plus
  the timer value passed.
  This function simply adds the PLMN to backoff list with the timer value of the PLMN which
  is already present in the list.

==============================================================================*/

boolean reg_sim_add_eplmn_to_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  rex_timer_cnt_type  timer,
  sys_block_plmn_cause_e_type cause,
  sys_modem_as_id_e_type  as_id
)
{
  int32  i;
  boolean list_updated = FALSE;
  if ( sys_plmn_id_is_undefined(plmn) )
  {
    MSG_ERROR_DS_0(as_id, "=REG= Add undefined PLMN to BACKOFF PLMN list");
    return list_updated;
  }

  MSG_HIGH_DS_0(as_id, "=REG= Adding PLMN to temp FPLMN list rem time after curr backoff timer expiry %d (ms)");
  /*
  ** Check to see if the PLMN is already contained in the
  ** backoff Forbidden PLMN List.
  */
  if ( !reg_sim_plmn_backoff_forbidden(plmn,as_id) )
  {
    /* enter the critical section here */ 
    rex_enter_crit_sect(&reg_backoff_fplmn_crit_sect);
    /*
    ** The PLMN is not already contained in the backoff Forbidden PLMN
    ** list so simply add it to the bottom of the list.
    */
    if ( reg_sim_backoff_fplmn_list.length < REG_SIM_PLMN_LIST_LENGTH )
    {
      reg_sim_backoff_fplmn_list.length++;
    }
    /*
    ** The backoff Forbidden PLMN List is full so the first (oldest) PLMN
    ** is removed from the top of the list and the new PLMN is added
    ** to the bottom of the list.
    */
    else
    {
      reg_sim_backoff_fplmn_list.length = REG_SIM_PLMN_LIST_LENGTH;

      for (i = 0; i < (int32)(reg_sim_backoff_fplmn_list.length - 1); i++)
      {
        reg_sim_backoff_fplmn_list.plmn_timer_list[i]= reg_sim_backoff_fplmn_list.plmn_timer_list[i + 1];
      }
    }

    /*
    ** Copy the PLMN into the empty position.
    */
    reg_sim_backoff_fplmn_list.plmn_timer_list[reg_sim_backoff_fplmn_list.length - 1].plmn = plmn;
    reg_sim_backoff_fplmn_list.plmn_timer_list[reg_sim_backoff_fplmn_list.length - 1].cause = cause;
    reg_sim_backoff_fplmn_list.plmn_timer_list[reg_sim_backoff_fplmn_list.length - 1].timer_count = timer;
    /* leave the critical section here */
    rex_leave_crit_sect(&reg_backoff_fplmn_crit_sect);
    list_updated = TRUE;
  }
  reg_sim_print_backoff_fplmn_list();
  return list_updated;
}



/*==============================================================================

FUNCTION NAME

  reg_sim_add_plmn_to_backoff_fplmn_list

==============================================================================*/

boolean reg_sim_add_plmn_to_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  rex_timer_cnt_type  timer,
  sys_block_plmn_cause_e_type   cause,
  sys_modem_as_id_e_type as_id
)
{
    int32  i;
    uint32 index;
    boolean list_updated = FALSE;
    boolean found = FALSE;
    sys_block_plmn_cause_e_type   cause_local;

  if(cause == SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE)
  {
    MSG_HIGH_DS_1(as_id, "=REG= PLMN added to temp FPLMN list for Max registration failure backoff time %d (ms)",
                    timer);
  }
  else if(cause == SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE)
  {
    MSG_HIGH_DS_1(as_id, "=REG= PLMN added to temp FPLMN list as voice is not available backoff time %d (ms)",
                    timer);
  }
  else if(cause == SYS_BLOCK_PLMN_CAUSE_PRIORITY )
  {
    MSG_HIGH_DS_1(as_id, "=REG= PLMN added to temp FPLMN list as DS blocking priority backoff time %d (ms)",
                    timer);
  }
  else 
  {
    MSG_HIGH_DS_1(as_id, "=REG= PLMN added to temp FPLMN list for other causes backoff time %d (ms)",
                    timer);
  }

  for(index = 0; index < reg_sim_backoff_fplmn_list.length; index++)
  {
    if(sys_plmn_match(reg_sim_backoff_fplmn_list.plmn_timer_list[index].plmn,plmn))
    {
      found = TRUE;
      cause_local = reg_sim_backoff_fplmn_list.plmn_timer_list[index].cause;
      MSG_HIGH_DS_0(as_id, "=REG= PLMN already present in the backoff list");
      break;
    }
  }
  if( cause == SYS_BLOCK_PLMN_CAUSE_PRIORITY  && found == TRUE && timer == REG_TIMERS_MAX_TIMER_VALUE)
  {

    (void)reg_sim_remove_plmn_from_backoff_fplmn_list(plmn,cause_local,as_id);
     MSG_HIGH_DS_0(as_id,"=REG= removing the PLMN to give DS blocking preference");
     found = FALSE;
  }
  /*
  ** Check to see if the PLMN is already contained in the
  ** backoff Forbidden PLMN List.
  */
  if ( found == FALSE )
  {
    if ( sys_plmn_id_is_undefined(plmn) )
    {
      MSG_HIGH_DS_0(as_id, "=REG= Adding wild card entry to BACKOFF PLMN list");
    }
    /* enter the critical section here */ 
    rex_enter_crit_sect(&reg_backoff_fplmn_crit_sect);
    /*
    ** The PLMN is not already contained in the backoff Forbidden PLMN
    ** list so simply add it to the bottom of the list.
    */
    if ( reg_sim_backoff_fplmn_list.length < REG_SIM_PLMN_LIST_LENGTH )
    {
      reg_sim_backoff_fplmn_list.length++;
    }
    /*
    ** The backoff Forbidden PLMN List is full so the first (oldest) PLMN
    ** is removed from the top of the list and the new PLMN is added
    ** to the bottom of the list.
    */
    else
    {
      reg_sim_backoff_fplmn_list.length = REG_SIM_PLMN_LIST_LENGTH;

      for (i = 0; i < (int32)(reg_sim_backoff_fplmn_list.length - 1); i++)
      {
        reg_sim_backoff_fplmn_list.plmn_timer_list[i]= reg_sim_backoff_fplmn_list.plmn_timer_list[i + 1];
      }
    }

    /*
    ** Copy the PLMN into the empty position.
    */
    reg_sim_backoff_fplmn_list.plmn_timer_list[reg_sim_backoff_fplmn_list.length - 1].plmn = plmn;
    /*
    ** Add cause module 
    */
    reg_sim_backoff_fplmn_list.plmn_timer_list[reg_sim_backoff_fplmn_list.length - 1].cause = cause;
    /*
    ** If the timer count is zero then it means the PLMN is backoff forbidden
    ** permanently. Set the timer count to max value to identify this plmn
    */
    if(timer != 0 && timer != REG_TIMERS_MAX_TIMER_VALUE)
    {
      reg_timers_start_backoff_fplmn_search_period_timer(
              &reg_sim_backoff_fplmn_list,
              timer,(reg_as_id_e_type)as_id);
    }
    else
    {
      reg_sim_backoff_fplmn_list.plmn_timer_list[reg_sim_backoff_fplmn_list.length - 1].timer_count = 
               REG_TIMERS_MAX_TIMER_VALUE;
    }
    list_updated = TRUE;
    /* leave the critical section here */
    rex_leave_crit_sect(&reg_backoff_fplmn_crit_sect);
  }
  
  reg_sim_print_backoff_fplmn_list();

  return list_updated;
}



/*==============================================================================

FUNCTION NAME

  reg_sim_eplmn_list_backoff_forbidden

==============================================================================*/
boolean reg_sim_eplmn_list_backoff_forbidden
(
  const sys_plmn_list_s_type* eplmn_list_p,
  rex_timer_cnt_type *max_timer_count,
  sys_block_plmn_cause_e_type *cause
)
{
  uint32 i,j, length;
  boolean eplmn_backoff_forbidden = FALSE;

  length = MIN(eplmn_list_p->length, SYS_PLMN_LIST_MAX_LENGTH);
  rex_enter_crit_sect(&reg_backoff_fplmn_crit_sect);

  reg_sim_backoff_fplmn_list.length = MIN(reg_sim_backoff_fplmn_list.length,
                                                    REG_SIM_PLMN_LIST_LENGTH);

  for(i = 0; i < length; i++)
  {
    for(j = 0; j < reg_sim_backoff_fplmn_list.length; j++)
    {
      if(sys_plmn_match(reg_sim_backoff_fplmn_list.plmn_timer_list[j].plmn, eplmn_list_p->plmn[i]))
      {
        eplmn_backoff_forbidden = TRUE;
        *cause = reg_sim_backoff_fplmn_list.plmn_timer_list[j].cause;
        if(*max_timer_count < reg_sim_backoff_fplmn_list.plmn_timer_list[j].timer_count)
        {
          *max_timer_count = reg_sim_backoff_fplmn_list.plmn_timer_list[j].timer_count;
        }
      }
    }
  }

  rex_leave_crit_sect(&reg_backoff_fplmn_crit_sect);

  return eplmn_backoff_forbidden;
}

#endif

/*==============================================================================

FUNCTION NAME

  reg_sim_remove_plmn_from_gprs_fplmn_list

==============================================================================*/

boolean reg_sim_remove_plmn_from_gprs_fplmn_list
(
  sys_plmn_id_s_type rplmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  uint32 position = 0;
  uint32 i;
  boolean list_updated = FALSE;
  /*
  ** Verify that the RPLMN is defined before removing it from the
  ** GPRS Forbidden PLMN List.
  */
  if ( sys_plmn_id_is_undefined(rplmn) )
  {
    MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= Remove undefined PLMN from GPRS FPLMN list");
    return list_updated;
  }

#ifdef FEATURE_DUAL_SIM
  /*
  ** Search for the RPLMN in the GPRS Forbidden PLMN List.
  */
  if ( reg_sim_find_plmn_in_list(rplmn, &reg_sim_gprs_fplmn_list_sim[as_id], &position) )
  {
    /*
    ** If the RPLMN is found in the GPRS Forbidden PLMN List then
    ** remove the RPLMN from the list.
    */
    reg_sim_gprs_fplmn_list_sim[as_id].length--;

    for (i = position; i < reg_sim_gprs_fplmn_list_sim[as_id].length; i++)
    {
      reg_sim_gprs_fplmn_list_sim[as_id].plmn[i] = reg_sim_gprs_fplmn_list_sim[as_id].plmn[i + 1];
    }
    list_updated = TRUE;
    reg_sim_print_gprs_forbidden_plmn_list(as_id);
  }

#else

  /*
  ** Search for the RPLMN in the GPRS Forbidden PLMN List.
  */
  if ( reg_sim_find_plmn_in_list(rplmn, &reg_sim_gprs_fplmn_list, &position) )
  {
    /*
    ** If the RPLMN is found in the GPRS Forbidden PLMN List then
    ** remove the RPLMN from the list.
    */
    reg_sim_gprs_fplmn_list.length--;

    for (i = position; i < reg_sim_gprs_fplmn_list.length; i++)
    {
      reg_sim_gprs_fplmn_list.plmn[i] = reg_sim_gprs_fplmn_list.plmn[i + 1];
    }
    list_updated = TRUE;
    reg_sim_print_gprs_forbidden_plmn_list();
  }
#endif
  return list_updated;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_add_plmn_to_gprs_fplmn_list

==============================================================================*/

boolean reg_sim_add_plmn_to_gprs_fplmn_list
(
  sys_plmn_id_s_type plmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  uint32 position = 0;
  int32  i;
 
  boolean   list_updated = FALSE;

  /*
  ** Verify that the RPLMN is defined before adding it to the
  ** GPRS Forbidden PLMN List.
  */
  if ( sys_plmn_id_is_undefined(plmn) )
  {
    MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= Add undefined PLMN to GPRS FPLMN list");
    return list_updated;
  }

#ifdef FEATURE_DUAL_SIM
  /*
  ** If the PLMN matches the HPLMN then do not add it to the
  ** GPRS Forbidden PLMN List.
  */
#ifdef FEATURE_EQUIVALENT_HPLMN
  if (reg_sim_per_subs_is_ehplmn(plmn,as_id) )
#else
  if (sys_plmn_match(reg_sim_per_subs_read_hplmn(as_id), plmn) )
#endif
  {
    MSG_ERROR_DS_0(REG_LOCAL_SUB, "=REG= Add HPLMN to GPRS FPLMN list");
    return list_updated;
  }

  /* Start T3245 timer if it's not running */
  if(reg_sim_check_if_t3245_behavior_enabled(as_id) &&
     (reg_is_t3245_timer_active(as_id) == FALSE))
  {
    reg_timers_start_t3245_timer(REG_T3245_DEFUALT_VALUE,as_id);
  }

  /*
  ** Check to see if the PLMN is already contained in the
  ** GPRS Forbidden PLMN List.
  */
  if ( !reg_sim_find_plmn_in_list(plmn, &reg_sim_gprs_fplmn_list_sim[as_id], &position) )
  {
    /*
    ** The PLMN is not already contained in the GPRS Forbidden PLMN
    ** List so simply add it to the bottom of the list.
    */
    if ( reg_sim_gprs_fplmn_list_sim[as_id].length < REG_SIM_PLMN_LIST_LENGTH )
    {
      reg_sim_gprs_fplmn_list_sim[as_id].length++;
    }
    /*
    ** The GPRS Forbidden PLMN List is full so the first (oldest) PLMN
    ** is removed from the top of the list and the new PLMN is added
    ** to the bottom of the list.
    */
    else
    {
      reg_sim_gprs_fplmn_list_sim[as_id].length = REG_SIM_PLMN_LIST_LENGTH;

      for (i = 0; i < (int32)(reg_sim_gprs_fplmn_list_sim[as_id].length - 1); i++)
      {
        reg_sim_gprs_fplmn_list_sim[as_id].plmn[i] = reg_sim_gprs_fplmn_list_sim[as_id].plmn[i + 1];
      }
    }

    /*
    ** Copy the PLMN into the empty position.
    */
    reg_sim_gprs_fplmn_list_sim[as_id].plmn[reg_sim_gprs_fplmn_list_sim[as_id].length - 1] = plmn;

    list_updated = TRUE;

  }

  reg_sim_print_gprs_forbidden_plmn_list(as_id);

#else

  /*
  ** If the PLMN matches the HPLMN then do not add it to the
  ** GPRS Forbidden PLMN List.
  */
#ifdef FEATURE_EQUIVALENT_HPLMN
  if (reg_sim_is_ehplmn(plmn) )
#else
  if (sys_plmn_match(reg_sim_read_hplmn(), plmn) )
#endif
  {
    MSG_ERROR_DS_0(REG_SUB, "=REG= Add HPLMN to GPRS FPLMN list");
    return list_updated;
  }

  /* Start T3245 timer if it's not running */
  if((reg_is_t3245_timer_active() == FALSE) &&
     reg_sim_check_if_t3245_behavior_enabled())
  {
    reg_timers_start_t3245_timer(REG_T3245_DEFUALT_VALUE);
  }

  /*
  ** Check to see if the PLMN is already contained in the
  ** GPRS Forbidden PLMN List.
  */
  if ( !reg_sim_find_plmn_in_list(plmn, &reg_sim_gprs_fplmn_list, &position) )
  {
    /*
    ** The PLMN is not already contained in the GPRS Forbidden PLMN
    ** List so simply add it to the bottom of the list.
    */
    if ( reg_sim_gprs_fplmn_list.length < REG_SIM_PLMN_LIST_LENGTH )
    {
      reg_sim_gprs_fplmn_list.length++;
    }
    /*
    ** The GPRS Forbidden PLMN List is full so the first (oldest) PLMN
    ** is removed from the top of the list and the new PLMN is added
    ** to the bottom of the list.
    */
    else
    {
      reg_sim_gprs_fplmn_list.length = REG_SIM_PLMN_LIST_LENGTH;

      for (i = 0; i < (int32)(reg_sim_gprs_fplmn_list.length - 1); i++)
      {
        reg_sim_gprs_fplmn_list.plmn[i] = reg_sim_gprs_fplmn_list.plmn[i + 1];
      }
    }

    /*
    ** Copy the PLMN into the empty position.
    */
    reg_sim_gprs_fplmn_list.plmn[reg_sim_gprs_fplmn_list.length - 1] = plmn;
    list_updated = TRUE;
  }

  reg_sim_print_gprs_forbidden_plmn_list();
#endif
  return list_updated;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_delete_fplmn_and_gprs_fplmn_list

==============================================================================*/

void reg_sim_delete_fplmn_and_gprs_fplmn_list
(
  void
)
{
  reg_sim_plmn_list_s_type fplmn_list;
 
  boolean status = FALSE;
  uint32  i=0;
  int32  file_length = 0;

  /*
  ** Read the Forbidden PLMN List length from the SIM.
  */
  if ( mmgsdi_session_read_cache_file_size
        (reg_sim_mmgsdi_info.session_id,
         MMGSDI_USIM_FPLMN,
         &file_length) == MMGSDI_SUCCESS)
  {
    /*
      ** Set the maximum file size for the Forbidden PLMN List.
      */
    fplmn_list.length = MIN(file_length,
                  SYS_FORBIDDEN_PLMN_LIST_MAX_LENGTH *
                  REG_SIM_PLMN_LENGTH);
    fplmn_list.length = (uint32)(fplmn_list.length / REG_SIM_PLMN_LENGTH);
    for(i=0; i<fplmn_list.length; i++)
    {
      /*Removed PLMN from FPLMN list*/
      sys_plmn_undefine_plmn_id(&fplmn_list.plmn[i]);
    }
    /*
    ** Write the updated Forbidden PLMN List to the SIM.
    */
#ifdef FEATURE_DUAL_SIM
    status = reg_sim_per_subs_write
    (
      MMGSDI_USIM_FPLMN,
      (byte*)fplmn_list.plmn, 
      (int32)(fplmn_list.length * REG_SIM_PLMN_LENGTH),
      (sys_modem_as_id_e_type)reg_sub_id
    );
#else
    status = reg_sim_write
    (
      MMGSDI_USIM_FPLMN,
      (byte*)fplmn_list.plmn, 
      (int32)(fplmn_list.length * REG_SIM_PLMN_LENGTH)
    );
#endif
    if (status == TRUE)
    {
#ifdef FEATURE_DUAL_SIM
      reg_log_umts_nas_fplmn_list(&fplmn_list, (sys_modem_as_id_e_type)reg_sub_id);
#else
      reg_log_umts_nas_fplmn_list(&fplmn_list);
#endif

#ifdef FEATURE_LTE 
      reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_FPLMN);
#endif
    }
    else
    {
      MSG_ERROR_DS_0(REG_SUB, "=REG= Write FPLMN list failed");
    }

  }

  /*Remove PLMN from GPRS forbiden PLMN list*/
  if (reg_sim_gprs_fplmn_list.length != 0)
  {
    reg_sim_gprs_fplmn_list.length = 0;
#ifdef FEATURE_LTE 
    reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
 #endif
  }
  reg_sim_print_gprs_forbidden_plmn_list
  (
#ifdef FEATURE_DUAL_SIM
    (sys_modem_as_id_e_type)reg_sub_id
#endif
  );

}

/*==============================================================================

FUNCTION NAME

  reg_sim_check_mcc_same_country

==============================================================================*/
boolean reg_sim_check_mcc_same_country
(
  sys_plmn_id_s_type plmn
)
{
  boolean mcc_match = FALSE;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  boolean plmn_id_is_undefined;
  uint32  home_mcc;
  uint32  home_mnc;
  boolean home_mnc_includes_pcs_digit;
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32 i;
#endif
  sys_plmn_get_mcc_mnc
    (
      plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
#ifdef FEATURE_EQUIVALENT_HPLMN
  NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
  for(i =0; i < reg_sim_ehplmn_list.length; i++)
  {
    sys_plmn_get_mcc_mnc
    (
      reg_sim_ehplmn_list.plmn[i],
      &plmn_id_is_undefined,
      &home_mnc_includes_pcs_digit,
      &home_mcc,
      &home_mnc
    );
    if(sys_mcc_same_country(mcc, home_mcc))
    {
      mcc_match = TRUE;
      break;
    }
  }    
  NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
#else
  NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
  sys_plmn_get_mcc_mnc
  (
    reg_sim_hplmn,
    &plmn_id_is_undefined,
    &home_mnc_includes_pcs_digit,
    &home_mcc,
    &home_mnc
  );
  if(sys_mcc_same_country(mcc, home_mcc))
  {
    mcc_match = TRUE;
  }
  NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);
#endif
  return mcc_match;

}


/*==============================================================================

FUNCTION NAME

  reg_sim_plmn_gprs_forbidden

==============================================================================*/

boolean reg_sim_plmn_gprs_forbidden
(
  sys_plmn_id_s_type plmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  uint32  position = 0;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  boolean plmn_id_is_undefined;
  uint32  home_mcc;
  uint32  home_mnc;
  boolean home_mnc_includes_pcs_digit;
#ifdef FEATURE_EQUIVALENT_HPLMN
  uint32 i;
#endif

  boolean data_roaming_disabled = TRUE; 
  /*
  ** Verify that the PLMN is defined before checking to see if it is
  ** included in the Forbidden PLMN List.
  */
  if ( sys_plmn_id_is_undefined(plmn) )
  {
    return FALSE;
  }

  if(reg_nv_read_data_roaming(
#ifdef FEATURE_DUAL_SIM
         as_id
#endif         
	     ) == SYS_DATA_ROAMING_INTERNATIONAL_OFF)
  {
    sys_plmn_get_mcc_mnc
    (
      plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );
#ifdef FEATURE_EQUIVALENT_HPLMN
    NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
#ifdef FEATURE_DUAL_SIM
        for(i =0; i < reg_sim_ehplmn_list_sim[as_id].length; i++)
		{
		  sys_plmn_get_mcc_mnc
		  (
			reg_sim_ehplmn_list_sim[as_id].plmn[i],
			&plmn_id_is_undefined,
			&home_mnc_includes_pcs_digit,
			&home_mcc,
			&home_mnc
		  );
		  if(sys_mcc_same_country(mcc, home_mcc))
		  {
			data_roaming_disabled = FALSE;
			break;
		  }
		}
#else
		for(i =0; i < reg_sim_ehplmn_list.length; i++)
		{
		  sys_plmn_get_mcc_mnc
		  (
			reg_sim_ehplmn_list.plmn[i],
			&plmn_id_is_undefined,
			&home_mnc_includes_pcs_digit,
			&home_mcc,
			&home_mnc
		  );
		  if(sys_mcc_same_country(mcc, home_mcc))
		  {
			data_roaming_disabled = FALSE;
			break;
		  }
		}
#endif    
    NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
#else
    NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
#ifdef FEATURE_DUAL_SIM
        sys_plmn_get_mcc_mnc
		(
		  reg_sim_hplmn_sim[as_id],
		  &plmn_id_is_undefined,
		  &home_mnc_includes_pcs_digit,
		  &home_mcc,
		  &home_mnc
		);
#else
		sys_plmn_get_mcc_mnc
		(
		  reg_sim_hplmn,
		  &plmn_id_is_undefined,
		  &home_mnc_includes_pcs_digit,
		  &home_mcc,
		  &home_mnc
		);
#endif
    if(sys_mcc_same_country(mcc, home_mcc))
    {
      data_roaming_disabled = FALSE;
    }
    NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);
#endif
    if(data_roaming_disabled)
    {
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_2(REG_LOCAL_SUB, "=REG= International roaming disabled, PS not allowed on PLMN %03lu-%03lu", mcc,mnc);
      }
      else
      {
        MSG_HIGH_DS_2(REG_LOCAL_SUB, "=REG= International roaming disabled, PS not allowed on PLMN %03lu-%02lu", mcc,mnc);
      }
      return TRUE;
    }
  }

#ifdef FEATURE_DUAL_SIM
  return reg_sim_find_plmn_in_list( plmn, &reg_sim_gprs_fplmn_list_sim[as_id], &position );
#else
  return reg_sim_find_plmn_in_list( plmn, &reg_sim_gprs_fplmn_list, &position );
#endif
}


/*==============================================================================

FUNCTION NAME

  reg_sim_find_plmn_in_list

==============================================================================*/

boolean reg_sim_find_plmn_in_list
(
        sys_plmn_id_s_type        plmn,
  const reg_sim_plmn_list_s_type* plmn_list_p,
        uint32*                   position_p
)
{
  boolean found = FALSE;
  uint32  i;

  for (i = 0; i < plmn_list_p->length; i++)
  {
    if ( sys_plmn_match(plmn_list_p->plmn[i], plmn) )
    {
      found = TRUE;
      *position_p = i;
      break;
    }
  }

  return found;
}

#ifdef FEATURE_LTE

/*==============================================================================

FUNCTION NAME

  reg_sim_plmn_backoff_forbidden

==============================================================================*/

boolean reg_sim_plmn_backoff_forbidden
(
  sys_plmn_id_s_type        plmn,
  sys_modem_as_id_e_type as_id
)
{
  boolean found = FALSE;
  uint32  i;
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  rex_timer_cnt_type curr_timer_count = 0;
  rex_timer_cnt_type rem_time_value = 0;
  /* enter the critical section here */ 
  rex_enter_crit_sect(&reg_backoff_fplmn_crit_sect);
  /* Get the remainning time for reg_timers_backoff_fplmn_search_timer */
  curr_timer_count = reg_timers_get_rem_backoff_fplmn_search_timer_value((reg_as_id_e_type)as_id);
  for (i = 0; i < reg_sim_backoff_fplmn_list.length; i++)
  {
    if ( sys_plmn_match(reg_sim_backoff_fplmn_list.plmn_timer_list[i].plmn, plmn) ||
         sys_plmn_id_is_undefined(reg_sim_backoff_fplmn_list.plmn_timer_list[i].plmn))
    {
      found = TRUE;
      if (reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count != REG_TIMERS_MAX_TIMER_VALUE)
      {
        rem_time_value = reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count+curr_timer_count;
      }
      else
      {
        rem_time_value = reg_sim_backoff_fplmn_list.plmn_timer_list[i].timer_count;
      }
      sys_plmn_get_mcc_mnc
      (
        reg_sim_backoff_fplmn_list.plmn_timer_list[i].plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
      if(mnc_includes_pcs_digit)
      {
        if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE)
        {
          MSG_HIGH_DS_3(as_id, "=REG= PLMN (%03lu-%03lu) is backoff forbidden added for Max registration failure, timer to unblock %u(ms)", 
                 mcc, mnc,rem_time_value);
        }
        else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE)
        {
          MSG_HIGH_DS_3(as_id, "=REG= PLMN (%03lu-%03lu) is backoff forbidden added as voice is not available, timer to unblock %u(ms)", 
                 mcc, mnc,rem_time_value);
        }
        else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == SYS_BLOCK_PLMN_CAUSE_PRIORITY )
        {
          MSG_HIGH_DS_3(as_id, "=REG= PLMN (%03lu-%03lu) is backoff forbidden added DS blocking Priority, timer to unblock %u(ms)", 
                 mcc, mnc,rem_time_value);
        }
        else
        {
          MSG_HIGH_DS_3(as_id, "=REG= PLMN (%03lu-%03lu) is backoff forbidden added for other causes, timer to unblock %u(ms)",
                  mcc, mnc,rem_time_value);
        }
      }
      else
      {
        if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == SYS_BLOCK_PLMN_CAUSE_MAX_REGISTRATION_FAILURE)
        {
          MSG_HIGH_DS_3(as_id, "=REG= PLMN (%03lu-%02lu) is backoff forbidden added for Max registration failure, timer to unblock %u(ms)", 
                        mcc, mnc, rem_time_value);
        }
        else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == SYS_BLOCK_PLMN_CAUSE_VOICE_NOT_AVAILABLE)
        {
          MSG_HIGH_DS_3(as_id, "=REG= PLMN (%03lu-%02lu) is backoff forbidden added as voice is not available, timer to unblock %u(ms)", 
                        mcc, mnc, rem_time_value);
        }
        else if(reg_sim_backoff_fplmn_list.plmn_timer_list[i].cause == SYS_BLOCK_PLMN_CAUSE_PRIORITY )
        {
          MSG_HIGH_DS_3(as_id, "=REG= PLMN (%03lu-%02lu) is backoff forbidden added DS blocking priority, timer to unblock %u(ms)", 
                 mcc, mnc,rem_time_value);
        }
        else
        {
          MSG_HIGH_DS_3(as_id, "=REG= PLMN (%03lu-%02lu) is backoff forbidden added for other causes, timer to unblock %u(ms)", 
                        mcc, mnc, rem_time_value);
        }
      }
      break;
    }
  }
  /* leave the critical section here */
  rex_leave_crit_sect(&reg_backoff_fplmn_crit_sect);
  return found;
}

/*==============================================================================

FUNCTION NAME

  reg_sim_plmn_rat_forbidden

==============================================================================*/

boolean reg_sim_plmn_rat_forbidden
(
  sys_plmn_id_s_type        plmn,
  sys_radio_access_tech_e_type   rat,
  sys_modem_as_id_e_type as_id
)
{
  if((rat == SYS_RAT_LTE_RADIO_ACCESS) &&
     (reg_sim_plmn_backoff_forbidden(plmn,as_id) ||
#ifdef FEATURE_DUAL_SIM
      reg_sim_plmn_gprs_forbidden(plmn,as_id))
#else
      reg_sim_plmn_gprs_forbidden(plmn))
#endif
     )
  {
    return TRUE;
  }
  return FALSE;
}


#endif 

/*==============================================================================

FUNCTION NAME

  reg_sim_hplmn_get

  <Warning>: Uses critical section

==============================================================================*/

sys_plmn_id_s_type reg_sim_hplmn_get
(
  void
)
{
  sys_plmn_id_s_type plmn;
  NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
  plmn = reg_sim_hplmn;
  NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);
  return plmn;
}

sys_plmn_id_s_type reg_sim_per_subs_hplmn_get
(
  sys_modem_as_id_e_type sub_id
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_plmn_id_s_type plmn = {{0x00, 0x00, 0x00}};
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= reg_sim_per_subs_hplmn_get - Invalid SUB ID: %d", sub_id+1);
  }
  else
  {
    NAS_ENTER_CRIT_SECT(reg_hplmn_crit_sect);
    plmn = reg_sim_hplmn_sim[sub_id];
    NAS_EXIT_CRIT_SECT(reg_hplmn_crit_sect);
  }
  return plmn;
#else
  (void) sub_id;
  return reg_sim_hplmn_get();
#endif

}


void reg_sim_imp_variable_prints()
{
  boolean                  plmn_id_is_undefined = FALSE;
  boolean                  mnc_includes_pcs_digit = FALSE;
  uint32                   mcc = 0;
  uint32                   mnc = 0;
  boolean                  status = FALSE;
  reg_sim_plmn_list_s_type fplmn_list;

#ifdef FEATURE_ENHANCED_NW_SELECTION 
  MSG_HIGH_DS_2(REG_SUB, "=REG= ENS Supported Application Flag reg_sim_ens_flag - %d, reg_sim_is_ens_read - %d",
                reg_sim_ens_flag, reg_sim_is_ens_read);
#endif

  /*
   ** Print HPLMN.
   */
  sys_plmn_get_mcc_mnc
  (
    reg_sim_hplmn,
    &plmn_id_is_undefined,
    &mnc_includes_pcs_digit,
    &mcc,
    &mnc
   );

   if(mnc_includes_pcs_digit)
   {
     MSG_HIGH_DS_2(REG_SUB,"=REG= HPLMN - %03lu-%03lu", mcc, mnc);
   }
   else
   {
     MSG_HIGH_DS_2(REG_SUB,"=REG= HPLMN %03lu- %02lu", mcc, mnc);
   }

    /*
     ** Output Forbidden PLMN List log packet.
     */
#ifdef FEATURE_DUAL_SIM
    status = reg_sim_read_fplmn_list(&fplmn_list, (sys_modem_as_id_e_type)reg_sub_id);
#else
    status = reg_sim_read_fplmn_list(&fplmn_list);
#endif
    MSG_HIGH_DS_1(reg_sub_id, "=REG= Forbidden PLMN list (length = %d)", fplmn_list.length);
    if((status) && (fplmn_list.length > 0))
    {
#ifdef FEATURE_DUAL_SIM
      reg_log_umts_nas_fplmn_list(&fplmn_list,(sys_modem_as_id_e_type)reg_sub_id);
#else
      reg_log_umts_nas_fplmn_list(&fplmn_list);
#endif
    }     
}


#ifdef FEATURE_ENHANCED_NW_SELECTION

/*==============================================================================

FUNCTION NAME

  reg_sim_force_read_act_hplmn

==============================================================================*/

boolean reg_sim_force_read_act_hplmn
(
  void
)
{
  reg_sim_is_ahplmn_read = FALSE;
  return reg_sim_force_read((reg_sim_card_mode == MMGSDI_APP_SIM ? MMGSDI_SIM_7F66_PROP1_ACT_HPLMN :  MMGSDI_USIM_7F66_PROP1_ACT_HPLMN), GSDI_CINGULAR_ACT_HPLMN_LEN);
}

/*==============================================================================

FUNCTION NAME

  reg_sim_per_nas_stacks_read_ens_flag

==============================================================================*/
#ifdef FEATURE_DUAL_SIM
byte reg_sim_per_nas_stacks_read_ens_flag
(
  sys_modem_as_id_e_type stack_id_nas
)
{
  uint8 file_tst[GSDI_CINGULAR_TST_LEN];
  byte ens_flag = 0x00;

  if(IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    ERR("Invalid STACK ID:%d", stack_id_nas+1,0,0);
    return FALSE;
  }

  if (reg_sim_is_ens_read_sim[stack_id_nas])
  {
    return reg_sim_ens_flag_sim[stack_id_nas];
  }

  if ( ( reg_sim_card_mode_sim[stack_id_nas] != MMGSDI_APP_SIM ) &&
       ( reg_sim_card_mode_sim[stack_id_nas] != MMGSDI_APP_USIM ) )
  {
    return reg_sim_ens_flag_sim[stack_id_nas];
  }

  if ( reg_sim_per_subs_read(
         (reg_sim_card_mode_sim[stack_id_nas] == MMGSDI_APP_SIM ? MMGSDI_SIM_7F66_PROP1_SPT_TABLE : MMGSDI_USIM_7F66_PROP1_SPT_TABLE),
         file_tst,
         GSDI_CINGULAR_TST_LEN,
         stack_id_nas) )
  {
    if (file_tst[0] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_HPLMN_TIMER;
    }
    if (file_tst[1] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_MENU_CONTROL;
    }
    if (file_tst[2] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_ACTING_HPLMN;
    }
    if (file_tst[3] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_3G_FLAG;
    }
    if (file_tst[4] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_RAT_BALANCE;
    }
    if (file_tst[5] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_BIP_FEATURE;
    }
    if (file_tst[6] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_LTE_FLAG;
    }
    if (file_tst[7] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_CSG_FLAG;
    }
  }
  reg_sim_is_ens_read_sim[stack_id_nas] = TRUE;
  reg_sim_ens_flag_sim[stack_id_nas] = ens_flag;
  MSG_MED_DS_1(stack_id_nas, "=REG= Reading ENS Supported Application Flag - %d", reg_sim_ens_flag_sim[stack_id_nas]);

  return ens_flag;
}


/*==============================================================================

FUNCTION NAME

  reg_sim_per_subs_read_ens_flag

    This funciton is called by WRRC with sub_id. If the SUB configuration says it is an SGLTE
    device then use ps_stack_id

==============================================================================*/

byte reg_sim_per_subs_read_ens_flag
(
  sys_modem_as_id_e_type sub_id
)
{
  reg_as_id_e_type stack_id_nas = (reg_as_id_e_type)sub_id;

#if defined(FEATURE_SGLTE)
  if(!IS_NOT_VALID_SUB_ID(sub_id))
  {
    if(REG_SUB_IS_SGLTE_SUB((reg_as_id_e_type)sub_id))  
    {
      stack_id_nas = reg_ps_stack_id;
    }
  }
#endif
  return reg_sim_per_nas_stacks_read_ens_flag((sys_modem_as_id_e_type)stack_id_nas);
}


#else

byte reg_sim_read_ens_flag
(
  void
)
{
  uint8 file_tst[GSDI_CINGULAR_TST_LEN];
  byte ens_flag = 0x00;

#ifdef FEATURE_DUAL_SIM

  MSG_FATAL_DS( 0,"=REG= DUAL SIM: Wrong API called reg_sim_read_ens_flag()", 0, 0, 0);

#else

  if (reg_sim_is_ens_read)
  {
    MSG_MED_DS_1(REG_SUB, "=REG= ENS Supported Application Flag - %d", reg_sim_ens_flag);
    return reg_sim_ens_flag;
  }

  if ( ( reg_sim_card_mode != MMGSDI_APP_SIM ) &&
       ( reg_sim_card_mode != MMGSDI_APP_USIM ) )
  {
    MSG_HIGH_DS_1(REG_SUB, "=REG= SIM not available yet - returning reg_sim_ens_flag as %d", reg_sim_ens_flag);
    return reg_sim_ens_flag;
  }

  if ( reg_sim_read(
         (reg_sim_card_mode == MMGSDI_APP_SIM ? MMGSDI_SIM_7F66_PROP1_SPT_TABLE : MMGSDI_USIM_7F66_PROP1_SPT_TABLE),
         file_tst,
         GSDI_CINGULAR_TST_LEN) == TRUE)
  {
    if (file_tst[0] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_HPLMN_TIMER;
    }
    if (file_tst[1] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_MENU_CONTROL;
    }
    if (file_tst[2] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_ACTING_HPLMN;
    }
    if (file_tst[3] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_3G_FLAG;
    }
    if (file_tst[4] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_RAT_BALANCE;
    }
    if (file_tst[5] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_BIP_FEATURE;
    }
    if (file_tst[6] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_LTE_FLAG;
    }
    if (file_tst[7] != 0x00)
    {
      ens_flag |= GSDI_SUPPORT_CSG_FLAG;
    }
  }
  reg_sim_is_ens_read = TRUE;
  reg_sim_ens_flag = ens_flag;
  MSG_MED_DS_1(REG_SUB, "=REG= Reading ENS Supported Application Flag - %d", reg_sim_ens_flag);

#endif

  return ens_flag;
}
#endif

#endif /* FEATURE_ENHANCED_NW_SELECTION */

/*==============================================================================

FUNCTION NAME

  reg_sim_refresh_complete

==============================================================================*/
void  reg_sim_refresh_complete
(
  boolean status
)
{

  MSG_HIGH_DS_1(REG_SUB,"=REG= REFRESH is Completed - %d", status);
#ifdef FEATURE_SGLTE
  if(!REG_SUB_IS_SGLTE || (reg_as_id == reg_ps_stack_id))
#endif
  {

  (void) mmgsdi_session_refresh_complete 
  (
    reg_sim_mmgsdi_info.session_id,
    status, /* pass_fail */
    reg_sim_mmgsdi_generic_cb,
    0      /* client_ref */
  );

}
}

#if defined (FEATURE_DUAL_SIM ) || defined(FEATURE_SGLTE)
void  reg_sim_per_subs_refresh_complete
(
  boolean status,
  sys_modem_as_id_e_type as_id
)
{
  reg_as_id_e_type sub_id;
  MSG_HIGH_DS_1(REG_LOCAL_SUB,"=REG= REFRESH is Completed - %d", status);

  sub_id = reg_sub_id_stack[as_id];
#ifdef FEATURE_SGLTE
  if(!REG_SUB_IS_SGLTE_SUB(sub_id)|| (as_id == (sys_modem_as_id_e_type)reg_ps_stack_id))
#endif  
  {

    (void) mmgsdi_session_refresh_complete 
  (
    reg_sim_mmgsdi_info_sim[as_id].session_id,
    status, /* pass_fail */
    reg_sim_mmgsdi_generic_cb,
    0      /* client_ref */
  );

  }
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_sim_mmgsdi_generic_cb

==============================================================================*/

void reg_sim_mmgsdi_generic_cb
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type        *cnf_ptr
)
{
  reg_cmd_type *reg_cmd_p;
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
#endif


  if (cnf_ptr == NULL)
  {
     MSG_ERROR_DS_0(REG_SUB, "=REG= MMGSDI returned cnf_ptr as NULL");
     return;
  }
  else if(status != MMGSDI_SUCCESS)
  {
     MSG_ERROR_DS_2(REG_SUB, "=REG= MMGSDI CNF %d returned error %d", cnf, status);
     return;
  }

  switch (cnf)
  {
    case MMGSDI_CLIENT_ID_AND_EVT_REG_CNF:
      {
        mmgsdi_client_id_type  nas_cid = cnf_ptr->client_id_and_evt_reg_cnf.response_header.client_id;
        MSG_HIGH_DS_3(REG_SUB, "=REG= MMGSDI_CLIENT_ID_AND_EVT_REG_CNF Status %d H:0x%x L:0x%x", 
                      status, (dword)(nas_cid >> 32), (dword)(nas_cid & 0xFFFFFFFF));

        if ((reg_cmd_p = reg_get_cmd_buf()) != NULL)
        {
          /*
          ** Construct the SIM_REG_CLIENT_ID_IND command.
          */
          reg_cmd_p->cmd.sim_client_id_ind.message_header.message_set = MS_GSDI;
          reg_cmd_p->cmd.sim_client_id_ind.message_header.message_id  = SIM_REG_CLIENT_ID_IND;
          reg_cmd_p->cmd.sim_client_id_ind.message_header.message_len_lsb =
                     (sizeof(sim_client_id_ind_s_type) - sizeof(IMH_T)) % 256;
          reg_cmd_p->cmd.sim_client_id_ind.message_header.message_len_msb =
                     (sizeof(sim_client_id_ind_s_type) - sizeof(IMH_T)) / 256;
          reg_cmd_p->cmd.sim_client_id_ind.cid = nas_cid;

          reg_put_cmd(reg_cmd_p);
        }
        else
        {
          MSG_FATAL_DS(REG_SUB,"=REG= Error: Not able to Allocate REG Buffer", 0, 0, 0);
        }

      }
      break;
    case MMGSDI_REFRESH_CNF:
      switch (cnf_ptr->refresh_cnf.orig_refresh_req)
      {
        case MMGSDI_REFRESH_ORIG_REG_REQ:
          MSG_HIGH_DS_1(REG_SUB, "=REG= MMGSDI REG registration for Refresh status %d", status);
          break;

        case MMGSDI_REFRESH_ORIG_OK_TO_REFRESH_REQ:
          MSG_HIGH_DS_1(REG_SUB, "=REG= MMGSDI REG OK to FCN status %d", status);
          break;

        case MMGSDI_REFRESH_ORIG_COMPLETE_REQ:
          MSG_HIGH_DS_1(REG_SUB, "=REG= MMGSDI REG refresh complete status %d", status);
          break;
        default:
          break;
      }
      break;

    case MMGSDI_SESSION_OPEN_CNF:
#ifdef FEATURE_DUAL_SIM
      rex_enter_crit_sect(&reg_sim_crit_sect);
      if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_1].session_type 
           == (mmgsdi_session_type_enum_type) cnf_ptr->response_header.client_data)
      { 
        as_id = SYS_MODEM_AS_ID_1;
      }
      else if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_2].session_type 
               == (mmgsdi_session_type_enum_type) cnf_ptr->response_header.client_data)
      {
        as_id = SYS_MODEM_AS_ID_2;
      }
#ifdef FEATURE_TRIPLE_SIM
      else if(reg_sim_mmgsdi_info_sim[SYS_MODEM_AS_ID_3].session_type 
          == (mmgsdi_session_type_enum_type) cnf_ptr->response_header.client_data)
      {
        as_id = SYS_MODEM_AS_ID_3;
      }
#endif
      else
      {
        MSG_FATAL_DS(REG_LOCAL_SUB, "=REG= Invalid session type: %d", cnf_ptr->response_header.client_data, 0, 0);
      }
      memscpy(&reg_sim_mmgsdi_info_sim[as_id].session_id, sizeof(mmgsdi_session_id_type) , &(cnf_ptr->session_open_cnf.session_id),
                                        sizeof(mmgsdi_session_id_type));
      MSG_HIGH_DS_3(REG_LOCAL_SUB, "=REG= MMGSDI session open cnf Status %d Session ID: 0x%x %x", 
                    status, 
                    (dword)(reg_sim_mmgsdi_info_sim[as_id].session_id >> 32), 
                    (dword)(reg_sim_mmgsdi_info_sim[as_id].session_id & 0xFFFFFFFF));
      rex_leave_crit_sect(&reg_sim_crit_sect);
#else
#ifdef FEATURE_SGLTE
      memscpy(&reg_sim_mmgsdi_info_sim[REG_AS_ID_1].session_id, sizeof(nas_mmgsdi_info_type), &(cnf_ptr->session_open_cnf.session_id),
             sizeof(mmgsdi_session_id_type));
      MSG_HIGH_DS_3(REG_SUB, "=REG= MMGSDI session open cnf Status %d Session ID: 0x%x %x", 
                    status, 
                    (dword)(reg_sim_mmgsdi_info_sim[REG_AS_ID_1].session_id >> 32), 
                    (dword)(reg_sim_mmgsdi_info_sim[REG_AS_ID_1].session_id & 0xFFFFFFFF));
#else
      memscpy(&reg_sim_mmgsdi_info.session_id, sizeof(nas_mmgsdi_info_type), &(cnf_ptr->session_open_cnf.session_id),
              sizeof(mmgsdi_session_id_type));
      MSG_HIGH_DS_3(REG_SUB, "=REG= MMGSDI session open cnf Status %d Session ID: 0x%x %x", 
                    status, 
                    (dword)(reg_sim_mmgsdi_info.session_id >> 32), 
                    (dword)(reg_sim_mmgsdi_info.session_id & 0xFFFFFFFF));

#endif
#endif


      if ((reg_cmd_p = reg_get_cmd_buf()) != NULL)
      {
        /*
        ** Construct the SIM_REG_SUBS_READY_IND command.
        */
        reg_cmd_p->cmd.sim_subs_ready_ind.message_header.message_set = MS_GSDI;
        reg_cmd_p->cmd.sim_subs_ready_ind.message_header.message_id  = SIM_REG_SUBS_READY_IND;
        reg_cmd_p->cmd.sim_subs_ready_ind.message_header.message_len_lsb =
                   (sizeof(sim_subs_ready_ind_s_type) - sizeof(IMH_T)) % 256;
        reg_cmd_p->cmd.sim_subs_ready_ind.message_header.message_len_msb =
                   (sizeof(sim_subs_ready_ind_s_type) - sizeof(IMH_T)) / 256;

#ifdef FEATURE_DUAL_SIM
        reg_cmd_p->cmd.sim_subs_ready_ind.as_id = as_id;
#endif
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_SGLTE)
        reg_cmd_p->cmd.sim_subs_ready_ind.slot_id = cnf_ptr->session_open_cnf.slot_id;
#endif
        reg_put_cmd(reg_cmd_p);
      }
      else
      {
        MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate REG Buffer", 0, 0, 0);
      }
      break;
    default:
      MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= MMGSDI event %d not handled at REG", cnf);
      break;
  }
  
}



/*==============================================================================

FUNCTION NAME

  reg_sim_process_refresh_ind

==============================================================================*/
void reg_sim_process_refresh_ind(const sim_refresh_ind_s_type* msg_p)
{

  mmgsdi_refresh_stage_enum_type refresh_stage;
  mmgsdi_refresh_mode_enum_type  refresh_mode;  
  uint8          i, num_force_read=0;

#ifdef FEATURE_REL7_STEERING_OF_ROAMING
  reg_sim_plmn_list_s_type  plmn_list;
#endif

#ifdef FEATURE_ENHANCED_NW_SELECTION
  uint8            ens_flag;
  ens_flag = reg_sim_read_ens_flag();
#endif

  refresh_stage = msg_p->refresh.stage;
  refresh_mode = msg_p->refresh.mode;

  MSG_HIGH_DS_2(REG_SUB, "=REG= Reg received MMGSDI refresh event stage = %d, mode = %d", refresh_stage, refresh_mode);

    switch(refresh_stage)
    {
      case  MMGSDI_REFRESH_STAGE_WAIT_FOR_OK_TO_FCN:
        MSG_HIGH_DS_0(REG_SUB, "=REG= Reg received sim refresh ok to FCN");
#ifdef FEATURE_SGLTE
        if(!REG_SUB_IS_SGLTE || (reg_as_id == reg_ps_stack_id))
#endif
        {
          if( mmgsdi_session_ok_to_refresh(
              reg_sim_mmgsdi_info.session_id,
              TRUE, /*ok_to_refresh*/
              reg_sim_mmgsdi_generic_cb,
              0
              ) != MMGSDI_SUCCESS)
        {
           MSG_ERROR_DS_0(REG_SUB, "=REG= Failed to put OK to refresh in MMGSDI queue");
          }
        }
        break;

      case MMGSDI_REFRESH_STAGE_START:

        if (refresh_mode == MMGSDI_REFRESH_NAA_INIT_FCN)
        {
           /* no need to read files in INIT_FCN mode as they will be read 
           ** when sim_available is received from CM **/
          reg_sim_refresh_complete
          (
            TRUE
          );      
        }
        else if (refresh_mode == MMGSDI_REFRESH_NAA_FCN)
        {
          if (reg_sim_state  == SYS_SIM_STATE_NOT_AVAILABLE)
          {
            /*when FCN refresh received in SIM NOT AVAILABLE state means 
            *INIT_FCN has just happend before this. So no need to read files 
            *as they will be read after SIM_AVAIL is received from CM*/

            reg_sim_refresh_complete
            (
              TRUE
            );     
          }
          else
          {
            reg_sim_num_fcn_files = (uint8)MIN(msg_p->refresh.refresh_files.num_files,REG_SIM_NUM_REFRESH_FILE);

            MSG_HIGH_DS_1(REG_SUB, "=REG= Number of files received in FCN = %d", reg_sim_num_fcn_files);

            for (i=0; i<reg_sim_num_fcn_files && num_force_read < REG_SIM_NUM_REFRESH_FILE; i++)
            {
              reg_sim_refresh_fcn_list[i] = msg_p->refresh.refresh_files.file_list_ptr[i];
              switch (reg_sim_refresh_fcn_list[i])
              {
  #ifdef FEATURE_ENHANCED_NW_SELECTION
                case MMGSDI_SIM_7F66_PROP1_ACT_HPLMN:
                case MMGSDI_USIM_7F66_PROP1_ACT_HPLMN:
                  MSG_HIGH_DS_1(REG_SUB, "=REG= Received FCN for file %d\n", reg_sim_refresh_fcn_list[i]);
                  if (ens_flag & GSDI_SUPPORT_ACTING_HPLMN)
                  {
                    if (reg_sim_force_read_act_hplmn())
                    {
                      reg_sim_refresh_fcn_list[num_force_read++] = reg_sim_refresh_fcn_list[i];
                    }
                  }
                  break;
  #endif
                case MMGSDI_USIM_FPLMN:
                case MMGSDI_GSM_FPLMN:
                case MMGSDI_USIM_HPLMNWACT:
                case MMGSDI_GSM_HPLMNACT:
                case MMGSDI_USIM_RPLMNACT:
                case MMGSDI_USIM_HPLMN:
                case MMGSDI_GSM_HPLMN:
                case MMGSDI_USIM_LOCI:
                case MMGSDI_USIM_PSLOCI:
                  MSG_HIGH_DS_1(REG_SUB, "=REG= Received FCN for file %d\n", reg_sim_refresh_fcn_list[i]);
                      
                  if(reg_sim_force_read(reg_sim_refresh_fcn_list[i],0))
                  {
                    reg_sim_refresh_fcn_list[num_force_read++] = reg_sim_refresh_fcn_list[i];
                  }
                  break;
#ifdef FEATURE_FEMTO_CSG
                case MMGSDI_USIM_OCSGL:
                  MSG_HIGH_DS_1(REG_SUB, "=REG= Received FCN for file %d\n", reg_sim_refresh_fcn_list[i]);
                  reg_send_mmr_file_refresh_ind(MMGSDI_USIM_OCSGL);
                  break;
#endif 
                case MMGSDI_GSM_OPLMNWACT:         
                case MMGSDI_USIM_OPLMNWACT:
                  MSG_HIGH_DS_1(REG_SUB, "=REG= Received FCN for file %d\n", reg_sim_refresh_fcn_list[i]);
                  if( msg_p->refresh.source == MMGSDI_REFRESH_SOURCE_BUILTIN_PLMN)
                  {
                    reg_state_process_steering_of_roaming_refresh(msg_p->refresh.source);
                  }
                  break;
                default:
                  MSG_HIGH_DS_1(REG_SUB, "=REG= Wrong FCN Received file - %d", reg_sim_refresh_fcn_list[i]);
                  break;
              }
            }
            reg_sim_num_fcn_files = num_force_read;


            if(
#ifdef FEATURE_FEMTO_CSG				
               reg_sim_num_fcn_files == 0 ||
#endif 				
              ( reg_sim_num_fcn_files == 0 && 
               (reg_sim_refresh_fcn_list[0]== MMGSDI_GSM_OPLMNWACT ||
                reg_sim_refresh_fcn_list[0]== MMGSDI_USIM_OPLMNWACT)))
            {
              reg_sim_refresh_complete
              (
                TRUE
              );                    ;
            }

          }
#ifdef FEATURE_MODEM_HEAP
          modem_mem_free(msg_p->refresh.refresh_files.file_list_ptr, MODEM_MEM_CLIENT_NAS);
#else
          gs_free(msg_p->refresh.refresh_files.file_list_ptr);
#endif
        }
#ifdef FEATURE_REL7_STEERING_OF_ROAMING
        else if (refresh_mode == MMGSDI_REFRESH_STEERING_OF_ROAMING)
        {
          plmn_list.length = (uint32)(msg_p->refresh.plmn_list.data_len/REG_SIM_PLMN_W_ACT_LENGTH);

          for(i=0; i<msg_p->refresh.plmn_list.data_len/REG_SIM_PLMN_W_ACT_LENGTH; i++)
          {
            plmn_list.plmn[i].identity[0] = msg_p->refresh.plmn_list.data_ptr[i*REG_SIM_PLMN_W_ACT_LENGTH];
            plmn_list.plmn[i].identity[1] = msg_p->refresh.plmn_list.data_ptr[i*REG_SIM_PLMN_W_ACT_LENGTH+1];
            plmn_list.plmn[i].identity[2] = msg_p->refresh.plmn_list.data_ptr[i*REG_SIM_PLMN_W_ACT_LENGTH+2];

          }

#ifdef FEATURE_SGLTE
          if(!REG_SUB_IS_SGLTE || (reg_as_id == reg_ps_stack_id))
#endif
          {
            reg_sim_remove_plmns_from_csps_fplmn_list(&plmn_list);
 
          }

          reg_state_process_steering_of_roaming_refresh(MMGSDI_REFRESH_SOURCE_SIM);


#ifdef FEATURE_MODEM_HEAP
          modem_mem_free(msg_p->refresh.plmn_list.data_ptr, MODEM_MEM_CLIENT_NAS);
#else
          gs_free(msg_p->refresh.plmn_list.data_ptr);
#endif
        }

#endif
        break;
  
      case MMGSDI_REFRESH_STAGE_END_SUCCESS:
        MSG_HIGH_DS_0(REG_SUB, "=REG= Refresh FCN completed");
        break;
#ifdef FEATURE_REL7_STEERING_OF_ROAMING
       case MMGSDI_REFRESH_STAGE_END_FAILED:
        MSG_HIGH_DS_0(REG_SUB, "=REG= Refresh FCN Afiled");
        break;
#endif          
      default:
         MSG_ERROR_DS_1(REG_SUB, "=REG= Reg does not support refresh stage: %d", refresh_stage);
    }
  }

/*==============================================================================

FUNCTION NAME

  reg_sim_refresh_file_change_notification_cb

==============================================================================*/

void reg_sim_refresh_file_change_notification_cb
(
  uim_items_enum_type  * file_list,
  uint8                   num_files
)
{
  reg_cmd_type *reg_cmd_p;
  
  reg_sim_num_fcn_files = MIN(num_files,REG_SIM_NUM_REFRESH_FILE);
  memscpy(reg_sim_refresh_fcn_list, sizeof(reg_sim_refresh_fcn_list) , file_list, reg_sim_num_fcn_files * sizeof(uim_items_enum_type));

  /*
  ** Get a command buffer for the REG task.
  */
  if ((reg_cmd_p = reg_get_cmd_buf()) != NULL)
  {
    /*
    ** Construct the REFRESH FCN IND command.
    */
    reg_cmd_p->cmd.sim_refresh_ind.message_header.message_set = MS_GSDI;
    reg_cmd_p->cmd.sim_refresh_ind.message_header.message_id  = SIM_REFRESH_IND;
    /*lint -e778 */
    reg_cmd_p->cmd.sim_refresh_ind.message_header.message_len_lsb =
               (sizeof(sim_refresh_ind_s_type) - sizeof(IMH_T)) % 256;
    reg_cmd_p->cmd.sim_refresh_ind.message_header.message_len_msb =
               (sizeof(sim_refresh_ind_s_type) - sizeof(IMH_T)) / 256;
    /*lint +e778 */
    /*
    ** Put on REG command queue
    */

    reg_put_cmd(reg_cmd_p);
    }
    else
    {
      MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate REG Buffer", 0, 0, 0);
    }
}
/*==============================================================================

FUNCTION NAME

  reg_sim_refresh_fcn_ind_is_valid

==============================================================================*/
boolean reg_sim_refresh_fcn_ind_is_valid
(
  const sim_refresh_fcn_ind_s_type * msg_p
)
{
  boolean valid = FALSE;

  if (msg_p)
  {
    if ((msg_p->file_id == MMGSDI_USIM_HPLMN) ||
#ifdef FEATURE_ENHANCED_NW_SELECTION
        (msg_p->file_id == MMGSDI_USIM_7F66_PROP1_ACT_HPLMN) ||
        (msg_p->file_id == MMGSDI_SIM_7F66_PROP1_ACT_HPLMN) ||
#endif
        (msg_p->file_id == MMGSDI_USIM_FPLMN) ||
        (msg_p->file_id == MMGSDI_GSM_FPLMN) ||
        (msg_p->file_id == MMGSDI_USIM_HPLMNWACT) ||
        (msg_p->file_id == MMGSDI_GSM_HPLMNACT) ||
        (msg_p->file_id == MMGSDI_USIM_RPLMNACT) ||
        (msg_p->file_id == MMGSDI_GSM_HPLMN) ||
        (msg_p->file_id == MMGSDI_USIM_LOCI)||
        (msg_p->file_id == MMGSDI_USIM_PSLOCI))
    {
      valid = TRUE;
    }
  }
  return valid;
}
#ifdef FEATURE_EQUIVALENT_HPLMN
/*==============================================================================

FUNCTION NAME

  reg_sim_is_ehplmn_in_eplmn

DESCRIPTION

  This function will search for EHPLMN in EPLMN list and returns TRUE if any one of the 
  EHPLMN present in the EPLMN list

  <Warning>: Uses critical section

RETURN VALUE

  TRUE/FALSE
==============================================================================*/

boolean reg_sim_is_ehplmn_in_eplmn 
(
  void
)
{
  uint32 i;
  boolean return_value = FALSE;

  NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
  for (i=0;i<reg_sim_ehplmn_list.length;i++)
  {
    if(sys_eplmn_list_equivalent_plmn(reg_sim_ehplmn_list.plmn[i]))
    {
      return_value = TRUE;
      break;
    }
  }
  NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);

  return return_value;
}
/*==============================================================================

FUNCTION NAME

  reg_sim_read_ehplmn_list

==============================================================================*/
boolean reg_sim_read_ehplmn_list
(
  void
)
{
  reg_sim_plmn_list_s_type ehplmn_list;
  sys_plmn_id_s_type       hplmn;
  int32                    file_length = 0;
  boolean                  status = FALSE;
  int32                    file_size;
  boolean                  plmn_id_is_undefined;
  boolean                  mnc_includes_pcs_digit;
  uint32                   mcc;
  uint32                   mnc;
  uint32                   i;
  uint32                   j;
  /*
  ** Initialize the EHPLMN List.
  */
  memset(&ehplmn_list, 0x00, sizeof(reg_sim_plmn_list_s_type));

   /*
  ** Verify the SIM card mode before attempting to read data.
  */
  if (reg_sim_card_mode != MMGSDI_APP_USIM) 
  {
    /* Fill the ehplmn_list from EFS */
   status = reg_nv_read_efs_ehplmn_file(&ehplmn_list);
   if (!status)
   {
       MSG_HIGH_DS_0(REG_SUB, "=REG= SIM card. Adding HPLMN IMSI");
       hplmn = reg_sim_read_hplmn();
       NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
       reg_sim_ehplmn_list.length = 1;
       reg_sim_ehplmn_list.plmn[0] = hplmn;
       NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
       return FALSE;
   }
  }

  if (reg_sim_card_mode == MMGSDI_APP_USIM) 
  {
     /*
     ** Read the attributes for the EHPLMN List.
     */
     if(mmgsdi_session_read_cache_file_size (reg_sim_mmgsdi_info.session_id,
                                             MMGSDI_USIM_EHPLMN,
                                             &file_length) == MMGSDI_SUCCESS)
     {
       /*
       ** Set the maximum file size for the EHPLMN List.
       */
       file_size = MIN(file_length,
                       (REG_SIM_PLMN_LIST_LENGTH - 1) *
                       REG_SIM_PLMN_LENGTH);

       /*
       ** Read the EHPLMN List.
       */
       status = reg_sim_read
       (
         MMGSDI_USIM_EHPLMN,
         (byte *)ehplmn_list.plmn,
         file_size
       );

       if ( status )
       {
         ehplmn_list.length = (uint32)(file_size / REG_SIM_PLMN_LENGTH);
       }
       else
       {
          MSG_ERROR_DS_0(REG_SUB, "=REG= Read EF EHPLMN list failed");    
          MSG_HIGH_DS_0(REG_SUB, "=REG= OEM EHPLMN undefined or USIM compliant, use NV");
          /*Read the EHPLMN list from EFS_NV file*/
          status = reg_nv_read_efs_ehplmn_file(&ehplmn_list);
       }
     }
     else/*if EHPLMN list reading from SIM failed then read from EFS NV*/
     {
          MSG_ERROR_DS_0(REG_SUB, "=REG= Read EF EHPLMN attributes list failed");
          MSG_HIGH_DS_0(REG_SUB, "=REG= OEM EHPLMN undefined or USIM compliant, use EFS");
          /*Read the EHPLMN list from EFS_NV file*/
          status = reg_nv_read_efs_ehplmn_file(&ehplmn_list);
     }
  } /* reg_sim_card_mode == MMGSDI_APP_USIM */

  if(ehplmn_list.length != 0)
  {
    i=0;
    j=0;
    while (i<ehplmn_list.length)
    {
      if(!sys_plmn_id_is_undefined(ehplmn_list.plmn[i]))
      {
        if(sys_plmn_id_is_valid(ehplmn_list.plmn[i]))
        {
          ehplmn_list.plmn[j] = ehplmn_list.plmn[i];
          j++;
        }
      }
      i++;
    }
    ehplmn_list.length = j;
  }

  if(ehplmn_list.length == 0)
  {
      hplmn = reg_sim_read_hplmn();
      ehplmn_list.length = 1;
      ehplmn_list.plmn[0] = hplmn;
  }
  reg_send_cm_plmn_list_change_ind(SYS_PLMN_LIST_TYPE_EHPLMN);
  /*
  ** Copy the local list to the output list and print EHPLMN list.
  */
  MSG_HIGH_DS_1(REG_SUB, "=REG= EHPLMN list (length = %d)", ehplmn_list.length);
  MSG_HIGH_DS_0(REG_SUB, "=REG=  # MCC-MNC");
  for (i = 0; i < ehplmn_list.length; i++)
  {
    NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
    reg_sim_ehplmn_list.plmn[i] = ehplmn_list.plmn[i];
    NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);

    sys_plmn_get_mcc_mnc
      (
        ehplmn_list.plmn[i],
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &mcc,
        &mnc
      );
    if(mnc_includes_pcs_digit)
    {
      MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu", i, mcc, mnc);
    }
    else
    {
      MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu", i, mcc, mnc);
    }
  }
  reg_sim_ehplmn_list.length = ehplmn_list.length;
  
  return ( status );
}

/*==============================================================================

FUNCTION NAME

  reg_sim_is_ehplmn

  <Warning>: Uses critical section

==============================================================================*/
boolean reg_sim_is_ehplmn
(
  sys_plmn_id_s_type plmn
)
{
  uint32                        i;
  boolean return_value = FALSE;

  NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
  for(i = 0; i<reg_sim_ehplmn_list.length; i++)
  {
    if(sys_plmn_match(reg_sim_ehplmn_list.plmn[i], plmn))
    {
      return_value = TRUE;
      break;
    }
  }
  NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);

  return return_value;
}

#ifdef FEATURE_DUAL_SIM
boolean reg_sim_per_subs_is_ehplmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type as_id
)
{
  uint32                        i;

  if (IS_NOT_VALID_SUB_ID(as_id))
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= reg_sim_per_subs_is_ehplmn - Invalid AS ID: %d", as_id+1);
    return FALSE;
  }

  for(i = 0; i<reg_sim_ehplmn_list_sim[as_id].length; i++)
  {
    if(sys_plmn_match(reg_sim_ehplmn_list_sim[as_id].plmn[i], plmn))
        return TRUE;
  }
  return FALSE;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_sim_ehplmn_list_get

DESCRIPTION

  Function to get the ehplmn_list. 

RETURN VALUE

  TRUE/FALSE

==============================================================================*/
reg_sim_plmn_list_s_type reg_sim_ehplmn_list_get
(
  void
)
{
  reg_sim_plmn_list_s_type plmn;

  NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
  plmn = reg_sim_ehplmn_list;
  NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);

  return plmn;
}

/*==============================================================================

FUNCTION NAME

  reg_sim_per_subs_ehplmn_list_get_ptr

DESCRIPTION

  Function to get the ehplmn_list. 

RETURN VALUE

  TRUE/FALSE

==============================================================================*/
void reg_sim_per_subs_ehplmn_list_get_ptr
(
  reg_sim_plmn_list_s_type *plmn_list,
  sys_modem_as_id_e_type sub_id
)
{
  reg_as_id_e_type stack_id_nas = (reg_as_id_e_type)sub_id;
#ifdef FEATURE_DUAL_SIM
  if (IS_NOT_VALID_SUB_ID(sub_id))
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= reg_sim_per_subs_ehplmn_list_get - Invalid AS ID: %d", sub_id+1);
    plmn_list->length = 0;
    return;
  }
#if defined(FEATURE_SGLTE)
  if(REG_SUB_IS_SGLTE_SUB((reg_as_id_e_type)sub_id))  
  {
    stack_id_nas = reg_ps_stack_id;
  }
#endif
#elif defined(FEATURE_SGLTE)
  (void) sub_id;
  stack_id_nas = reg_ps_stack_id;
#endif
 reg_sim_per_nas_stacks_ehplmn_list_get_ptr(plmn_list,(sys_modem_as_id_e_type)stack_id_nas);
}


/*==============================================================================

FUNCTION NAME

  reg_sim_ehplmn_list_get_ptr

DESCRIPTION

  Function to get the ehplmn_list. 

RETURN VALUE

  void

==============================================================================*/
void reg_sim_ehplmn_list_get_ptr
(
  reg_sim_plmn_list_s_type *plmn
)
{
  NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
  memscpy(plmn,sizeof(reg_sim_plmn_list_s_type),&reg_sim_ehplmn_list,sizeof(reg_sim_plmn_list_s_type));
  NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
}
/*==============================================================================

FUNCTION NAME

  reg_sim_per_nas_stacks_ehplmn_list_get_ptr

DESCRIPTION

  Function to get the ehplmn_list in a pointer. 

RETURN VALUE

  TRUE/FALSE

==============================================================================*/
void reg_sim_per_nas_stacks_ehplmn_list_get_ptr
(
  reg_sim_plmn_list_s_type *plmn_src,
  sys_modem_as_id_e_type stack_id_nas
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    ERR("Invalid STACK ID:%d", stack_id_nas+1,0,0);
    plmn_src->length = 0;
    return ;
  }
  NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
  (void)memscpy(plmn_src,sizeof(reg_sim_plmn_list_s_type), 
               &reg_sim_ehplmn_list_sim[stack_id_nas],sizeof(reg_sim_plmn_list_s_type));
  NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
#else
  (void) stack_id_nas;
  NAS_ENTER_CRIT_SECT(reg_ehplmn_crit_sect);
  (void)memscpy(plmn_src,sizeof(reg_sim_plmn_list_s_type), 
			  &reg_sim_ehplmn_list,sizeof(reg_sim_plmn_list_s_type));
  NAS_EXIT_CRIT_SECT(reg_ehplmn_crit_sect);
#endif
}


#endif

void reg_sim_add_to_low_priority_plmn_list
(
  sys_plmn_id_s_type plmn,  
  sys_radio_access_tech_e_type           rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  uint32 i,j;
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  mcc;
  uint32  mnc;
  /* Index Length-1 is the top entry of the array */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE

  /* If entry already present in the list, pull in to top */
  for(i=reg_sim_low_priority_plmn_list_sim[as_id].length;i>0; i--)
  {
    if(sys_plmn_match(plmn,reg_sim_low_priority_plmn_list_sim[as_id].info[i-1].plmn) &&
       reg_sim_low_priority_plmn_list_sim[as_id].info[i-1].rat == rat)
    {
      for(j = i; j<reg_sim_low_priority_plmn_list_sim[as_id].length;j++)
      {
        reg_sim_low_priority_plmn_list_sim[as_id].info[j-1].plmn  = reg_sim_low_priority_plmn_list_sim[as_id].info[j].plmn;
        reg_sim_low_priority_plmn_list_sim[as_id].info[j-1].rat  = reg_sim_low_priority_plmn_list_sim[as_id].info[j].rat;
      }
      /* Reduce length by 1 */
      reg_sim_low_priority_plmn_list_sim[as_id].length--;
      break;
    }
  }

  if(reg_sim_low_priority_plmn_list_sim[as_id].length == 2*REG_SIM_PREFERRED_PLMN_LIST_LENGTH) 
  {
    /* If list is full, remove the oldest (index 0) entry. */
    for(i=1;i<reg_sim_low_priority_plmn_list_sim[as_id].length;i++)  
    {
      reg_sim_low_priority_plmn_list_sim[as_id].info[i-1].plmn  = reg_sim_low_priority_plmn_list_sim[as_id].info[i].plmn;
      reg_sim_low_priority_plmn_list_sim[as_id].info[i-1].rat  = reg_sim_low_priority_plmn_list_sim[as_id].info[i].rat;  
    }
  }
  else
  {
    reg_sim_low_priority_plmn_list_sim[as_id].length++;
  }

  /*
  ** Length of non-zero tells the location of the most recent PLMN rejected with cause #17
  ** Array index is (length - 1)
  */
  reg_sim_low_priority_plmn_list_sim[as_id].info[reg_sim_low_priority_plmn_list_sim[as_id].length-1].plmn = plmn;
  reg_sim_low_priority_plmn_list_sim[as_id].info[reg_sim_low_priority_plmn_list_sim[as_id].length-1].rat = rat;

  /* Print Low priority plmn list */
  MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= Low Priority PLMN list:");
  MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= PLMN    RAT");
  for(i=0;i<reg_sim_low_priority_plmn_list_sim[as_id].length;i++)
  {
    sys_plmn_get_mcc_mnc
    (
      reg_sim_low_priority_plmn_list_sim[as_id].info[i].plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );

    if(reg_sim_low_priority_plmn_list_sim[as_id].info[i].rat == SYS_RAT_GSM_RADIO_ACCESS)
    {
      MSG_HIGH_DS_3(REG_LOCAL_SUB, "=REG= %2lu %03lu-%03lu    GSM", i, mcc, mnc);
    }
    else
    {
      MSG_HIGH_DS_3(REG_LOCAL_SUB, "=REG= %2lu %03lu-%03lu    UMTS", i, mcc, mnc);
  }
  }

#else

  /* If entry already present in the list, pull in to top */
  for(i=reg_sim_low_priority_plmn_list.length;i>0; i--)
  {
    if(sys_plmn_match(plmn,reg_sim_low_priority_plmn_list.info[i-1].plmn) &&
       reg_sim_low_priority_plmn_list.info[i-1].rat == rat)
    {
      for(j = i; j<reg_sim_low_priority_plmn_list.length;j++)
      {
        reg_sim_low_priority_plmn_list.info[j-1].plmn  = reg_sim_low_priority_plmn_list.info[j].plmn;
        reg_sim_low_priority_plmn_list.info[j-1].rat  = reg_sim_low_priority_plmn_list.info[j].rat;
      }
      /* Reduce length by 1 */
      reg_sim_low_priority_plmn_list.length--;
      break;
    }
  }

  if(reg_sim_low_priority_plmn_list.length == 2*REG_SIM_PREFERRED_PLMN_LIST_LENGTH) 
  {
    /* If list is full, remove the oldest (index 0) entry. */
    for(i=1;i<reg_sim_low_priority_plmn_list.length;i++)  
    {
      reg_sim_low_priority_plmn_list.info[i-1].plmn  = reg_sim_low_priority_plmn_list.info[i].plmn;
      reg_sim_low_priority_plmn_list.info[i-1].rat  = reg_sim_low_priority_plmn_list.info[i].rat;  
    }
  }
  else
  {
    reg_sim_low_priority_plmn_list.length++;
  }

  /*
  ** Length of non-zero tells the location of the most recent PLMN rejected with cause #17
  ** Array index is (length - 1)
  */
  reg_sim_low_priority_plmn_list.info[reg_sim_low_priority_plmn_list.length-1].plmn = plmn;
  reg_sim_low_priority_plmn_list.info[reg_sim_low_priority_plmn_list.length-1].rat = rat;

  /* Print Low priority plmn list */
  MSG_HIGH_DS_0(REG_SUB, "=REG= Low Priority PLMN list:");
  MSG_HIGH_DS_0(REG_SUB, "=REG= PLMN    RAT");
  for(i=0;i<reg_sim_low_priority_plmn_list.length;i++)
  {
    sys_plmn_get_mcc_mnc
    (
      reg_sim_low_priority_plmn_list.info[i].plmn,
      &plmn_id_is_undefined,
      &mnc_includes_pcs_digit,
      &mcc,
      &mnc
    );

    if(reg_sim_low_priority_plmn_list.info[i].rat == SYS_RAT_GSM_RADIO_ACCESS)
    {
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu       GSM", i, mcc, mnc);
      }
      else
      {
        MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu       GSM", i, mcc, mnc);
      }
    }
    else
    {
      if(mnc_includes_pcs_digit)
      {
        MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu-%03lu       UMTS", i, mcc, mnc);
      }
      else
      {
        MSG_HIGH_DS_3(REG_SUB, "=REG= %2lu %03lu- %02lu       UMTS", i, mcc, mnc);
      }
    }
  }

#endif

}

void reg_sim_clear_low_priority_plmn_list
(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_modem_as_id_e_type stack_id_nas
#else
  void
#endif
)
{
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  if(IS_NOT_VALID_STACK_ID(stack_id_nas))
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= Invalid STACK ID:%d", stack_id_nas+1);
    return;
  }
#endif
  /* Length of zero means that list is empty */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  reg_sim_low_priority_plmn_list_sim[stack_id_nas].length = 0;
  reg_per_nas_stacks_mode_managed_roaming_plmn_reset(stack_id_nas);
  MSG_HIGH_DS_0(stack_id_nas, "=REG= Low Priority PLMN list cleared!!");
#else
  reg_sim_low_priority_plmn_list.length = 0;
  reg_mode_managed_roaming_plmn_reset();
  MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= Low Priority PLMN list cleared!!");
#endif
}

reg_sim_preferred_plmn_list_s_type* reg_sim_low_priority_plmn_list_get
(
   void
)
{
  return &reg_sim_low_priority_plmn_list;
}

boolean reg_sim_plmn_in_low_priority_plmn_list
(
  sys_plmn_id_s_type plmn,  
  sys_radio_access_tech_e_type rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
)
{
  uint32 i;  
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  for(i=0;i<reg_sim_low_priority_plmn_list_sim[as_id].length;i++)
  {
    if(sys_plmn_match(plmn,reg_sim_low_priority_plmn_list_sim[as_id].info[i].plmn) &&
       reg_sim_low_priority_plmn_list_sim[as_id].info[i].rat == rat)
    {
      return TRUE;
    }
  }
#else
  for(i=0;i<reg_sim_low_priority_plmn_list.length;i++)
  {
    if(sys_plmn_match(plmn,reg_sim_low_priority_plmn_list.info[i].plmn) &&
       reg_sim_low_priority_plmn_list.info[i].rat == rat)
    {
      return TRUE;
    }
  }
#endif
  return FALSE;
}


/*===========================================================================
FUNCTION NAME 
         REG_SIM_REFRESH_EVENT_CB

DESCRIPTION
  Callback function from MMGSDI generated events.

RETURN VALUE
   NONE

===========================================================================*/
void reg_sim_refresh_event_cb
(
  const mmgsdi_event_data_type *event
)
{  
  reg_cmd_type *reg_cmd_p;
  uint32  i;
  uint8 index, num_iterations;
#if defined FEATURE_DUAL_SIM ||  defined FEATURE_SGLTE
  reg_as_id_e_type as_id = REG_AS_ID_1;
  reg_as_id_e_type sub_id = REG_AS_ID_1;

#ifdef FEATURE_DUAL_SIM
  as_id = reg_sim_get_as_id(event->session_id);
  sub_id = reg_sub_id_stack[as_id];
#endif 
#endif 

  index = 0;
  num_iterations = 1;
#ifdef FEATURE_SGLTE
  if(REG_SGLTE_DUAL_STACK_SUB(sub_id))
  {
    num_iterations = 2;
  }
#endif

  if ((event->evt == MMGSDI_REFRESH_EVT) && 
       (event->data.refresh.mode == MMGSDI_REFRESH_NAA_FCN ||
        event->data.refresh.mode == MMGSDI_REFRESH_NAA_INIT_FCN
#ifdef FEATURE_REL7_STEERING_OF_ROAMING
        ||
        event->data.refresh.mode == MMGSDI_REFRESH_STEERING_OF_ROAMING
#endif 
        ||
        event->data.refresh.mode == MMGSDI_REFRESH_3G_SESSION_RESET
        )  
      )
  {
    do
    {
      /* Post the message to Reg task */
      if ((reg_cmd_p = reg_get_cmd_buf()) != NULL)
      {
        
        /*
        ** Construct the REFRESH FCN IND command.
        */
        reg_cmd_p->cmd.sim_refresh_ind.message_header.message_set = MS_GSDI;
        reg_cmd_p->cmd.sim_refresh_ind.message_header.message_id  = SIM_REFRESH_IND;
     
        reg_cmd_p->cmd.sim_refresh_ind.message_header.message_len_lsb =
               (sizeof(sim_refresh_ind_s_type) - sizeof(IMH_T)) % 256;
        reg_cmd_p->cmd.sim_refresh_ind.message_header.message_len_msb =
               (sizeof(sim_refresh_ind_s_type) - sizeof(IMH_T)) / 256;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
        reg_cmd_p->cmd.sim_refresh_ind.as_id = (sys_modem_as_id_e_type)as_id;
#endif
        memscpy(&reg_cmd_p->cmd.sim_refresh_ind.refresh, sizeof(reg_cmd_p->cmd.sim_refresh_ind.refresh),
                &event->data.refresh, sizeof(mmgsdi_refresh_evt_info_type));

        if ( event->data.refresh.stage == MMGSDI_REFRESH_STAGE_START)

        {
          if (event->data.refresh.mode == MMGSDI_REFRESH_NAA_FCN)
          {
        reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.num_files= MIN(event->data.refresh.refresh_files.num_files , REG_SIM_NUM_REFRESH_FILE);
#ifdef FEATURE_MODEM_HEAP
            reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.file_list_ptr = 
                    (mmgsdi_file_enum_type*) modem_mem_calloc(1,sizeof(mmgsdi_file_enum_type)*(reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.num_files), MODEM_MEM_CLIENT_NAS);
#else
            reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.file_list_ptr = 
                    (mmgsdi_file_enum_type*) gs_alloc(sizeof(mmgsdi_file_enum_type)*(reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.num_files));
#endif
  
            if (reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.file_list_ptr == NULL)
            {
              MSG_ERROR_DS_0(REG_SUB, "=REG= Mem Alloc failed");
#ifdef FEATURE_MODEM_HEAP
              modem_mem_free(reg_cmd_p, MODEM_MEM_CLIENT_NAS);
#else
              gs_free(reg_cmd_p);
#endif
              return;
            }
  
           memscpy(reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.file_list_ptr, sizeof(mmgsdi_file_enum_type) * (reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.num_files),
                   event->data.refresh.refresh_files.file_list_ptr, sizeof(mmgsdi_file_enum_type) * (reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.num_files));
  
           reg_cmd_p->cmd.sim_refresh_ind.refresh.source = event->data.refresh.source;   
  
            for (i = 0; i < (reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.num_files); i++)
            {
                MSG_HIGH_DS_3(REG_LOCAL_SUB, "=REG= Received refresh file list  %d  : %d %d", 
                              i, 
                              event->data.refresh.refresh_files.file_list_ptr[i],
                              reg_cmd_p->cmd.sim_refresh_ind.refresh.refresh_files.file_list_ptr[i]);
            }
          }
#ifdef FEATURE_REL7_STEERING_OF_ROAMING
          else if(event->data.refresh.mode == MMGSDI_REFRESH_STEERING_OF_ROAMING)
          {
        reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_len = MIN( event->data.refresh.plmn_list.data_len  , (REG_SIM_PLMN_LIST_LENGTH * REG_SIM_PLMN_W_ACT_LENGTH));
#ifdef FEATURE_MODEM_HEAP
            reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_ptr = 
                     (uint8*) modem_mem_calloc(1,sizeof(uint8)*(uint32)(reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_len), MODEM_MEM_CLIENT_NAS);
#else
            reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_ptr = 
                    (uint8*) gs_alloc(sizeof(uint8)*(reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_len));
#endif
            if (reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_ptr == NULL)
            { 
              MSG_ERROR_DS_0(REG_SUB, "=REG= Mem Alloc failed");

#ifdef FEATURE_MODEM_HEAP
              modem_mem_free(reg_cmd_p, MODEM_MEM_CLIENT_NAS);
#else
              gs_free(reg_cmd_p);
#endif
#if defined (FEATURE_DUAL_SIM ) || defined (FEATURE_SGLTE)
              reg_sim_per_subs_refresh_complete(FALSE,(sys_modem_as_id_e_type)as_id);
#else
              reg_sim_refresh_complete(FALSE);
#endif

              return;
            }
            memscpy(reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_ptr,(unsigned int)(reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_len) ,
                     event->data.refresh.plmn_list.data_ptr,(unsigned int)(event->data.refresh.plmn_list.data_len));
    
            MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= Received OPLMN list  Length : %d  ", 
                          (reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_len/REG_SIM_PLMN_W_ACT_LENGTH));
  
            for (i = 0; i <( uint32)(reg_cmd_p->cmd.sim_refresh_ind.refresh.plmn_list.data_len); i=i+REG_SIM_PLMN_W_ACT_LENGTH)
            {
               MSG_HIGH_DS_3(REG_SUB, "=REG= OPLMN list  %X %X %X",
                             event->data.refresh.plmn_list.data_ptr[i],
                             event->data.refresh.plmn_list.data_ptr[i+1],
                             event->data.refresh.plmn_list.data_ptr[i+2]);
            }
      
          }
#endif
          else
          {
             MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= REG is not supporting the refresh mode %d", event->data.refresh.mode);
          }
  
        }
        /*
        ** Put on REG command queue
        */
        reg_put_cmd(reg_cmd_p);
      }    
      else
      {
        MSG_FATAL_DS(REG_SUB, "=REG= Error: Not able to Allocate REG Buffer", 0, 0, 0);
      }
      index++;
#ifdef FEATURE_SGLTE
      if (REG_SGLTE_DUAL_STACK_SUB(sub_id))
      {
        as_id = REG_SGLTE_STACK_2;
      }
#endif
    }
    //Run the while loop only if in SGLTE DUAL STACK 
    while(index < num_iterations);
         
  }
  else
  {
    MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= Reg does not support MMGSDI event %d", event->evt);
  }
  
}


/*===========================================================================
FUNCTION NAME 
         REG_SIM_POST_SIM_CARD_UNAVAILABLE_IND

DESCRIPTION
  This funciton posts "REG_SIM_CARD_UNAVAILABLE_IND" to REG

RETURN VALUE
   NONE

===========================================================================*/

void reg_sim_post_sim_card_unavailable_ind
(
  sys_modem_as_id_e_type as_id
)
{
  reg_cmd_type *reg_cmd_p;
  if ((reg_cmd_p = reg_get_cmd_buf()) != NULL)
  {
    reg_cmd_p->cmd.sim_card_unavailable_ind.message_header.message_set = MS_GSDI;
    reg_cmd_p->cmd.sim_card_unavailable_ind.message_header.message_id  = REG_SIM_CARD_UNAVAILABLE_IND;
  
    reg_cmd_p->cmd.sim_card_unavailable_ind.message_header.message_len_lsb =
           (sizeof(reg_sim_card_unavailable_ind_s_type) - sizeof(IMH_T)) % 256;
    reg_cmd_p->cmd.sim_card_unavailable_ind.message_header.message_len_msb =
           (sizeof(reg_sim_card_unavailable_ind_s_type) - sizeof(IMH_T)) / 256;
    reg_cmd_p->cmd.sim_card_unavailable_ind.as_id = as_id;
    reg_put_cmd(reg_cmd_p);
  }
  else
  {
    ERR_FATAL("=REG= Error: Not able to Allocate REG Buffer",0,0,0);
  }
}
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
void reg_sim_post_sim_busy_ind
(
  sys_modem_as_id_e_type as_id, boolean sim_busy
)
{
  reg_cmd_type *reg_cmd_p;
  if ((reg_cmd_p = reg_get_cmd_buf()) != NULL)
  {
    reg_cmd_p->cmd.sim_busy_ind.message_header.message_set = MS_GSDI;
    reg_cmd_p->cmd.sim_busy_ind.message_header.message_id  = REG_SIM_BUSY_IND;
    reg_cmd_p->cmd.sim_busy_ind.is_sim_busy = sim_busy;
    reg_cmd_p->cmd.sim_busy_ind.message_header.message_len_lsb =
           (sizeof(reg_sim_card_busy_ind_s_type) - sizeof(IMH_T)) % 256;
    reg_cmd_p->cmd.sim_busy_ind.message_header.message_len_msb =
           (sizeof(reg_sim_card_busy_ind_s_type) - sizeof(IMH_T)) / 256;
    reg_cmd_p->cmd.sim_busy_ind.as_id = as_id;
    reg_put_cmd(reg_cmd_p);
  }
  else
  {
    ERR_FATAL("=REG= Error: Not able to Allocate REG Buffer");
  }
}
#endif
/*===========================================================================
FUNCTION NAME 
         REG_SIM_REFRESH_EVENT_CB

DESCRIPTION
  Callback function from MMGSDI generated session events.

RETURN VALUE
   NONE

===========================================================================*/

void reg_sim_session_generic_event_cb
(
  const mmgsdi_event_data_type *event
)
{
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
#if defined (FEATURE_DUAL_SIM) || defined (FEATURE_SGLTE)
  as_id = (sys_modem_as_id_e_type)reg_sim_get_as_id(event->session_id);
#endif

  if (event->evt == MMGSDI_SESSION_CHANGED_EVT)
  {
    if (event->data.session_changed.activated == FALSE)
    {
      /* Stop SIM read/writes till SIM AVAILABLE REQ */
      MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= MMGSDI Session Deactivated - SIM card marked unknown");
#ifdef FEATURE_SGLTE
      if(REG_SUB_IS_SGLTE_SUB(as_id))
      {
        reg_sim_card_mode_sim[as_id] = MMGSDI_APP_UNKNOWN;
        reg_sim_card_mode_sim[REG_SGLTE_STACK_2] = MMGSDI_APP_UNKNOWN;
      }
      else
#endif
      {
#ifdef FEATURE_DUAL_SIM
        reg_sim_card_mode_sim[as_id] = MMGSDI_APP_UNKNOWN;
#else
        reg_sim_card_mode = MMGSDI_APP_UNKNOWN;
#endif
      }
      reg_sim_post_sim_card_unavailable_ind(as_id);
    }
  }
  else
  {
    reg_sim_refresh_event_cb(event);
  }

}


/*==============================================================================

FUNCTION NAME

  reg_sim_mmgsdi_evt_cb

DESCRIPTION

  This call back is used for receiving events from UIM.

ARGUMENTS

  mmgsdi_event_data_type - Event from UIM.

RETURN VALUE

  None.

==============================================================================*/
void reg_sim_mmgsdi_evt_cb
(
  const mmgsdi_event_data_type *event
)
{
  sys_modem_as_id_e_type as_id = SYS_MODEM_AS_ID_1;
#ifdef FEATURE_GSTK
  boolean imsi_switch_value = FALSE;
#endif
  
  if (event->evt == MMGSDI_CARD_ERROR_EVT)
  {
    as_id = reg_sim_get_as_id_from_slot(event->data.card_error.slot);
    if(as_id != SYS_MODEM_AS_ID_NONE)
    {
      reg_sim_post_sim_card_unavailable_ind(as_id);
#ifdef FEATURE_GSTK
      imsi_switch_value = reg_nv_is_imsi_switch_enabled();
      if(imsi_switch_value)
      {
          MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= GSTK idle event will be sent once sim is available as IMSI_SWITCH value is %d\n ", imsi_switch_value);
          reg_state_set_gstk_event_flag(TRUE);
      }
#endif
    }
    else
    {
      MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= Ignoring Card error event as no session was opened yet");
    }
  }
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
  else if(event->evt == MMGSDI_SIM_BUSY_EVT)
  {
      as_id = reg_sim_get_as_id_from_slot(event->data.sim_busy.slot);
    if(as_id != SYS_MODEM_AS_ID_NONE)
    {
      reg_as_id = as_id;
      reg_send_mmr_sim_busy_ind(event->data.sim_busy.sim_busy);
    }
    else
    {
       MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= Ignoring Card error event as no session was opened yet");
    }
  }
#endif
#ifdef FEATURE_NAS_ECALL
  if (event->evt == MMGSDI_PROFILE_DATA_EVT)
  {
    as_id = reg_sim_get_as_id_from_slot(event->data.card_error.slot);
    if(as_id != SYS_MODEM_AS_ID_NONE)
    {
      if (event->data.profile_data.profile.profile_type == MMGSDI_PROFILE_TYPE_EMERGENCY)
      {
        MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= ECALL Profile %d",event->data.profile_data.profile.profile_type);
        reg_ecall_profile = TRUE;
        reg_send_mmr_profile_type_ind(event->data.profile_data.profile.profile_type);
      }
      else
      {
        MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= Non-ECALL Profile %d",event->data.profile_data.profile.profile_type);
        reg_ecall_profile = FALSE;
        reg_send_mmr_profile_type_ind(event->data.profile_data.profile.profile_type);
      }
    }
    else
    {
      MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= Ignoring Card error event as no session was opened yet");
    }
  }
#endif
  else
  {
    MSG_HIGH_DS_1(REG_LOCAL_SUB, "=REG= Reg does not support MMGSDI event %d", event->evt);
  }
}


/*==============================================================================

FUNCTION NAME

    REG_SIM_MMGSDI_INIT

DESCRIPTION

  Function to get client ID for MMGSDI API calls 

RETURN VALUE
   NONE

==============================================================================*/

void reg_sim_mmgsdi_init
(
  void 
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  /* Register the MMGSDI event callback function */
  mmgsdi_status = mmgsdi_client_id_and_evt_reg(reg_sim_mmgsdi_evt_cb,
                                               reg_sim_mmgsdi_generic_cb,
                                               0);
  MSG_HIGH_DS_0(REG_SUB, "=REG= Reg called MMGSDI to register for client ID and evt");
  if(mmgsdi_status != MMGSDI_SUCCESS)
  {
    MSG_ERROR_DS_1( REG_SUB, "=REG= MMGSDI Registration failed : STATUS %d", mmgsdi_status);
  }
}

boolean reg_sim_open_session
(
  void
)
{
  mmgsdi_return_enum_type mmgsdi_status = MMGSDI_SUCCESS;

  if ((reg_sim_mmgsdi_info.session_type != MMGSDI_GW_PROV_PRI_SESSION) &&
      (reg_sim_mmgsdi_info.session_type != MMGSDI_GW_PROV_SEC_SESSION) 
#ifdef FEATURE_TRIPLE_SIM
      &&(reg_sim_mmgsdi_info.session_type != MMGSDI_GW_PROV_TER_SESSION)
#endif
     )
  {
    MSG_ERROR_DS_1(REG_SUB, "=REG= Invalid session type: %d", reg_sim_mmgsdi_info.session_type);
    reg_sim_card_mode = MMGSDI_APP_UNKNOWN;
    reg_sim_state = SYS_SIM_STATE_NOT_AVAILABLE;
    return FALSE;
  }
  else
  {
    reg_waiting_for_open_session_cnf = TRUE;

    mmgsdi_status = mmgsdi_session_open( 
                                 reg_sim_mmgsdi_info.client_id,
                                 reg_sim_mmgsdi_info.session_type,
                                 0,
                                 reg_sim_session_generic_event_cb,
                                 FALSE,
                                 reg_sim_mmgsdi_generic_cb,
#ifdef FEATURE_DUAL_SIM
                                 (mmgsdi_client_data_type) reg_sim_mmgsdi_info.session_type
#else
                                 0
#endif
                                 );
    if(mmgsdi_status != MMGSDI_SUCCESS)
    {
      MSG_ERROR_DS_1(REG_SUB, "=REG=  MMGSDI REG session open cmd fail : STATUS %d",(uint8) mmgsdi_status);
      return FALSE;
    }
  }

  return TRUE;
}

/*==============================================================================

FUNCTION NAME 
      REG_SIM_REMOVE_PLMNS_FROM_CSPS_FPLMN_LIST

DESCRIPTION
      Removed the PLMNs from FPLMN and GPRS forbidden PLMNs list

RETURN VALUE
     NONE


==============================================================================*/

void reg_sim_remove_plmns_from_csps_fplmn_list
(
  reg_sim_plmn_list_s_type *plmn_list
)
{
   reg_sim_plmn_list_s_type fplmn_list;
 
   boolean status = FALSE;
   boolean is_plmns_defined = FALSE;
   uint32  position = 0;
   uint32  i=0;
#ifdef FEATURE_LTE
   boolean send_fplmn_list_update = FALSE;
   boolean send_gprs_fplmn_list_update = FALSE; 
#endif
   /*
   ** Verify that the PLMN is defined before removing it from the
    ** Forbidden List.
    */
   for(i=0; i<plmn_list->length; i++)
   {
     if ( !sys_plmn_id_is_undefined(plmn_list->plmn[i]) )
     {
       is_plmns_defined = TRUE;
       break;
     }
   } 
 
   if(!is_plmns_defined)
   {
      return;
   }
   else
   {
 
      /*
      ** Read the Forbidden PLMN List from the SIM.
      */
#ifdef FEATURE_DUAL_SIM
      status = reg_sim_read_fplmn_list(&fplmn_list, (sys_modem_as_id_e_type)reg_sub_id);
#else
      status = reg_sim_read_fplmn_list(&fplmn_list);
#endif
   
      if ( status )
      {
        for(i=0; i<plmn_list->length; i++)
        {
          /*Removed PLMN from FPLMN list*/
 
          /*
          ** Search for the PLMN  in the Forbidden PLMN List.
          */
          if ( reg_sim_find_plmn_in_list( plmn_list->plmn[i], &fplmn_list, &position ) )
          {
            /*
            ** If the PLMN  is found in the Forbidden PLMN List then undefine
            ** the PLMN ID at the position where the PLMN  was stored.
            */
            sys_plmn_undefine_plmn_id(&fplmn_list.plmn[position]);
 #ifdef FEATURE_LTE
            send_fplmn_list_update = TRUE;
#endif
            /*
            ** Send a Log packet that PLMN is removed from FPLMN List
            ** FALSE means this PLMN is removed from FPLMN List.
            */ 

#ifdef FEATURE_DUAL_SIM
            reg_log_umts_nas_fplmn_list_update(plmn_list->plmn[i], FALSE, (sys_modem_as_id_e_type)reg_sub_id);
#else
            reg_log_umts_nas_fplmn_list_update(plmn_list->plmn[i], FALSE);
#endif
          } 

          /*Remove PLMN from GPRS forbiden PLMN lis*/
          if (reg_sim_gprs_fplmn_list.length != 0)
          {
#ifdef FEATURE_DUAL_SIM
            (void)reg_sim_remove_plmn_from_gprs_fplmn_list(plmn_list->plmn[i], (sys_modem_as_id_e_type)reg_sub_id);
#else
            if (reg_sim_remove_plmn_from_gprs_fplmn_list(plmn_list->plmn[i]))
            {
    #ifdef FEATURE_LTE 
              send_gprs_fplmn_list_update = TRUE;
    #endif /*FEATURE_LTE*/
            }
#endif /*FEATURE_DUAL_SIM*/ 
          }
        }
      } 
      else
      {
        for(i=0; i<plmn_list->length; i++)
        {
          /*Remove PLMN from GPRS forbiden PLMN lis*/
          if (reg_sim_gprs_fplmn_list.length != 0)
          {
#ifdef FEATURE_DUAL_SIM
            (void)reg_sim_remove_plmn_from_gprs_fplmn_list(plmn_list->plmn[i], (sys_modem_as_id_e_type)reg_sub_id);
#else
            if (reg_sim_remove_plmn_from_gprs_fplmn_list(plmn_list->plmn[i]))
            {
    #ifdef FEATURE_LTE 
              send_gprs_fplmn_list_update = TRUE;
    #endif /*FEATURE_LTE*/
            }
#endif /*FEATURE_DUAL_SIM*/           
          }
        }
      }
   } 
   /*
   ** Write the updated Forbidden PLMN List to the SIM.
   */
#ifdef FEATURE_DUAL_SIM
   status = reg_sim_write_fplmn_list(&fplmn_list, (sys_modem_as_id_e_type)reg_sub_id);
   reg_log_umts_nas_fplmn_list(&fplmn_list, (sys_modem_as_id_e_type)reg_sub_id);
#else
   status = reg_sim_write_fplmn_list(&fplmn_list);
   reg_log_umts_nas_fplmn_list(&fplmn_list);
#endif
 
 #ifdef FEATURE_LTE 
   if(send_fplmn_list_update)
   {
     reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_FPLMN);
   }
   if(send_gprs_fplmn_list_update)
   {
     reg_send_cm_fplmn_list_ind(SYS_FORBIDDEN_LIST_TYPE_GPRS_FPLMN);
   }
#endif
}
#ifdef FEATURE_EQUIVALENT_HPLMN
/*===========================================================================
FUNCTION NAME 
         REG_SIM_DISP_PNLY_HIGHEST_PRIORITY_EHPLMN

DESCRIPTION
  Function to read EHPLMN presentation indicator from SIM

RETURN VALUE
   1 - DSIPLAY only highest priority EHPLMN
   0 - DSIPLAY all  EHPLMNs
===========================================================================*/
boolean reg_sim_disp_only_highest_priority_ehplmn()
{
   boolean is_highest_priority_ehplmn_to_be_disp = TRUE;
   byte    ehplmnpi ;

   mmgsdi_file_enum_type         sim_filename = MMGSDI_USIM_EHPLMNPI;

   if ( reg_sim_card_mode == MMGSDI_APP_USIM )
   {
     if (reg_sim_read(sim_filename,&ehplmnpi,1) == TRUE)
     {
       MSG_HIGH_DS_1(REG_SUB,"=REG= EHPLMNPI - %d", ehplmnpi);

       if (ehplmnpi == 0x02)
       {
          is_highest_priority_ehplmn_to_be_disp = FALSE;
       }
     }
   }

   return is_highest_priority_ehplmn_to_be_disp;
}
#endif

/*===========================================================================
FUNCTION NAME 
         REG_SIM_IS_HPLMN_TO_BE_SELECTED

DESCRIPTION
  Function to read RPLMN SI from SIM and based on that HPLMN/EHPLMN
  or RPLMN will be selected
  
RETURN VALUE
   1 - Selected HPLMN/EHPLMN
   0 - Select RPLMN
===========================================================================*/
boolean reg_sim_is_hplmn_to_be_selected()
{
  boolean is_hplmn_has_to_be_selected = FALSE; 
  byte    lrplmnsi = 0; 

   mmgsdi_file_enum_type         sim_filename = MMGSDI_USIM_LRPLMNSI;

  if (reg_sim_card_mode == MMGSDI_APP_USIM)
  { 
    if (reg_sim_read(sim_filename,&lrplmnsi,1) == TRUE) 
    { 
       if (lrplmnsi == 0x01) 
       { 
         is_hplmn_has_to_be_selected = TRUE; 
       } 
    } 
    else if (reg_nv_gcf_flag_get() == FALSE)
    {
      is_hplmn_has_to_be_selected = reg_nv_is_hplmn_to_be_selected;
    } 
  } 
  else if (reg_nv_gcf_flag_get() == FALSE)
  {
     is_hplmn_has_to_be_selected = reg_nv_is_hplmn_to_be_selected;
  }
  MSG_HIGH_DS_2(REG_SUB, "=REG= is_hplmn_has_to_be_selected is - %d LRPLMNSI is - %d ", is_hplmn_has_to_be_selected,lrplmnsi);
  return is_hplmn_has_to_be_selected; 
}

/*===========================================================================
FUNCTION NAME 
         REG_SIM_UPDATE_CS_RPLMN

DESCRIPTION
  Function to update the CS RPLMN after registration is successful
  
RETURN VALUE
  None
===========================================================================*/
void reg_sim_update_cs_rplmn
(
  sys_plmn_id_s_type rplmn
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
)
{
 MSG_HIGH_DS_0(REG_LOCAL_SUB, "=REG= Updating CS RPLMN");

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
 memscpy(reg_sim_cs_rplmn_sim[as_id].identity, sizeof(reg_sim_cs_rplmn_sim[as_id].identity),rplmn.identity,REG_SIM_PLMN_LENGTH);
#else
 memscpy(reg_sim_cs_rplmn.identity, sizeof(reg_sim_cs_rplmn.identity),rplmn.identity,REG_SIM_PLMN_LENGTH);
#endif
}

#ifdef FEATURE_DUAL_SIM
boolean reg_sim_session_matching_with_other_sub( mmgsdi_session_type_enum_type session_type)
{
  sys_modem_as_id_e_type as_id;
  for (as_id = SYS_MODEM_AS_ID_1; as_id < MAX_NAS_STACKS; as_id++)
  {
    if((as_id != (sys_modem_as_id_e_type)reg_sub_id) && (reg_sim_mmgsdi_info_sim[as_id].session_type== session_type ))
    {
      //*ret_as_id = as_id;
      return TRUE;
    }
  }
  return FALSE;
}
#endif

#ifdef FEATURE_SGLTE
void reg_update_dual_stack_variables(void)
{
  reg_sim_state_sim[REG_SGLTE_STACK_2]= reg_sim_state_sim[reg_ps_stack_id];
  reg_sim_mmgsdi_info_sim[REG_SGLTE_STACK_2] = reg_sim_mmgsdi_info_sim[reg_ps_stack_id];
  reg_sim_card_mode_sim[REG_SGLTE_STACK_2]=reg_sim_card_mode_sim[reg_ps_stack_id];
  reg_sim_hplmn_sim[REG_SGLTE_STACK_2] = reg_sim_hplmn_sim[reg_ps_stack_id];
  reg_state_is_pwr_on_manual_plmn_selection_sim[REG_SGLTE_STACK_2] = reg_state_is_pwr_on_manual_plmn_selection_sim[reg_ps_stack_id];
#ifdef FEATURE_EQUIVALENT_HPLMN
  reg_sim_ehplmn_list_sim[REG_SGLTE_STACK_2] = reg_sim_ehplmn_list_sim[reg_ps_stack_id];
#endif
}
void reg_sim_perform_merge_operation(void)
{
  reg_sim_preferred_plmn_list_s_type local_low_pri_list;
  sys_plmn_id_s_type local_cs_rplmn;
  local_low_pri_list = reg_sim_low_priority_plmn_list_sim[reg_cs_stack_id];
  reg_sim_low_priority_plmn_list_sim[reg_cs_stack_id] = reg_sim_low_priority_plmn_list_sim[reg_ps_stack_id];
  if ( reg_sim_state_sim[mm_ps_stack_id] !=SYS_SIM_STATE_NOT_AVAILABLE  )
  {
    if((reg_sim_state_sim[mm_ps_stack_id]== SYS_SIM_STATE_AVAILABLE) &&
          (reg_sim_state_sim[mm_cs_stack_id]== SYS_SIM_STATE_CS_INVALID))
    {
      reg_sim_state_sim[mm_ps_stack_id] = SYS_SIM_STATE_CS_INVALID;
    }
    else if((reg_sim_state_sim[mm_ps_stack_id]== SYS_SIM_STATE_PS_INVALID) &&
         (reg_sim_state_sim[mm_cs_stack_id]== SYS_SIM_STATE_CS_INVALID))
    {
      reg_sim_state_sim[mm_ps_stack_id] = SYS_SIM_STATE_CS_PS_INVALID;
    }
  }
  reg_sim_low_priority_plmn_list_sim[reg_ps_stack_id] = local_low_pri_list;
  local_cs_rplmn = reg_sim_cs_rplmn_sim[reg_cs_stack_id];
  reg_sim_cs_rplmn_sim[reg_cs_stack_id] = reg_sim_cs_rplmn_sim[reg_ps_stack_id];
  reg_sim_cs_rplmn_sim[reg_ps_stack_id] = local_cs_rplmn;
}

void reg_sim_perform_split_operation(void)
{
  
  reg_sim_preferred_plmn_list_s_type local_low_pri_list;
  sys_plmn_id_s_type local_cs_rplmn;
  local_low_pri_list = reg_sim_low_priority_plmn_list_sim[reg_cs_stack_id];
  reg_sim_low_priority_plmn_list_sim[reg_cs_stack_id] = reg_sim_low_priority_plmn_list_sim[reg_ps_stack_id];
  reg_sim_low_priority_plmn_list_sim[reg_ps_stack_id] = local_low_pri_list;
  local_cs_rplmn = reg_sim_cs_rplmn_sim[reg_cs_stack_id];
  reg_sim_cs_rplmn_sim[reg_cs_stack_id] = reg_sim_cs_rplmn_sim[reg_ps_stack_id];
  if(reg_sim_state_sim[mm_ps_stack_id]== SYS_SIM_STATE_AVAILABLE) 
  {
    reg_sim_state_sim[mm_cs_stack_id] = SYS_SIM_STATE_AVAILABLE;
  }
  else if(reg_sim_state_sim[mm_ps_stack_id]== SYS_SIM_STATE_CS_PS_INVALID) 
  {
    reg_sim_state_sim[mm_ps_stack_id]= SYS_SIM_STATE_PS_INVALID;
    reg_sim_state_sim[mm_cs_stack_id] = SYS_SIM_STATE_CS_INVALID;
  }
  else if(reg_sim_state_sim[mm_ps_stack_id]== SYS_SIM_STATE_PS_INVALID)
  {
    reg_sim_state_sim[mm_cs_stack_id] = SYS_SIM_STATE_AVAILABLE;
  }
  else if (reg_sim_state_sim[mm_ps_stack_id]== SYS_SIM_STATE_CS_INVALID)
  {
    reg_sim_state_sim[mm_ps_stack_id] = SYS_SIM_STATE_AVAILABLE;
    reg_sim_state_sim[mm_cs_stack_id] = SYS_SIM_STATE_CS_INVALID;
  }

  reg_sim_cs_rplmn_sim[reg_ps_stack_id] = local_cs_rplmn;
}

#endif

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================
FUNCTION NAME:  REG_SIM_SET_CSG_LIST_READ_COMPLETE

DESCRIPTION
  Function to update the status of REG variable when CSG list read is complete
  
RETURN VALUE
  None
==============================================================================*/

void reg_sim_set_csg_list_read_complete(boolean list_read)
{
  reg_sim_mm_csg_list_read_complete = list_read;
}

/*==============================================================================
FUNCTION NAME:  REG_SIM_GET_CSG_LIST_READ_COMPLETE

DESCRIPTION
  Function to retrieve the status of CSG read completion 
  
RETURN VALUE
  None
==============================================================================*/

boolean reg_sim_get_csg_list_read_complete(void)
{
  return reg_sim_mm_csg_list_read_complete;
}
#endif

/*==============================================================================

FUNCTION NAME

  reg_sim_get_most_pref_oplmn_plmn

DESCRIPTION

  Function return most preferred plmn from OPLMN list,
  with respect to mcc past as arguement

RETURN VALUE

  plmn id

==============================================================================*/

sys_plmn_id_s_type reg_sim_get_most_pref_oplmn_plmn(
uint32 mcc
#ifdef FEATURE_DUAL_SIM
,sys_modem_as_id_e_type sub_id
#endif
)
{
  boolean plmn_id_is_undefined;
  boolean mnc_includes_pcs_digit;
  uint32  temp_mcc;
  uint32  mnc;
  int32  i;
  mmgsdi_len_type preferred_plmn_length;
  mmgsdi_len_type operator_pplmn_length;
  mmgsdi_len_type user_pplmn_length;
  reg_sim_plmn_w_act_s_type* preferred_plmn_list_p = NULL;
  sys_plmn_id_s_type  result_plmn={0xFF,0xFF,0xFF};
  

  preferred_plmn_list_p = reg_sim_read_preferred_plmn_list(&preferred_plmn_length,
                                                 &operator_pplmn_length,&user_pplmn_length,FALSE);
  
  if(preferred_plmn_list_p == NULL )
  {
    return result_plmn;
  }

  if(operator_pplmn_length > 0)
  {
    for (i = 0; i < operator_pplmn_length; i++)
    {
      //SKIP the undefined plmn
      if(sys_plmn_id_is_undefined(preferred_plmn_list_p[i].plmn))
      {
        continue;
      }
      sys_plmn_get_mcc_mnc
      (
        preferred_plmn_list_p[i].plmn,
        &plmn_id_is_undefined,
        &mnc_includes_pcs_digit,
        &temp_mcc,
        &mnc
      );
      if( sys_mcc_same_country(temp_mcc, mcc) )
      {
        result_plmn = preferred_plmn_list_p[i].plmn;
        break;
      }
    }   
  }

#ifdef FEATURE_MODEM_HEAP
  modem_mem_free( preferred_plmn_list_p , MODEM_MEM_CLIENT_NAS);
#else
  gs_free(preferred_plmn_list_p);
#endif

  return  result_plmn;
}