#ifndef REG_LOG_H
#define REG_LOG_H
/*==============================================================================


                    R E G  -  L O G   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains functions for generating REG log packets for QXDM.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_log.h_v   1.0   10 May 2002 14:45:52   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_log.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/15/02   kwa     Initial version.
04/28/04   kwa     Added reg_log_umts_nas_eplmn_list.
12/21/04   up      Added reg_log_umts_nas_fplmn_list_update, 
                   reg_log_umts_nas_fplmn_list, 
                   reg_log_umts_nas_hplmn_search_timer_start,
                   reg_log_umts_nas_hplmn_search_timer_expire,
                   reg_log_umts_nas_hplmn_search_start and
                   reg_log_umts_nas_hplmn_search_end.
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "reg_state.h"
#include "sys_plmn_selection_v.h"
#include "reg_sim_v.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_log_reg_packet

DESCRIPTION

  Function that fills and writes a REG LOG PACKET for QXDM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_log_reg_packet
(
  reg_state_e_type                   reg_state,
  sys_network_selection_mode_e_type  network_selection_mode
);


/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_eplmn_list

DESCRIPTION

  Function that fills and writes a UMTS NAS EPLMN LIST LOG PACKET for QXDM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_log_umts_nas_eplmn_list
(
        sys_plmn_id_s_type    rplmn,
  const sys_plmn_list_s_type* eplmn_list_p
);


/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_fplmn_list_update

DESCRIPTION

  Function fills & writes a UMTS NAS FPLMN LIST UPDATE LOG PACKET for QXDM.
  This is to report the addition/deletion of a PLMN to/from FPLMN List.

RETURN VALUE

  None

==============================================================================*/

extern void reg_log_umts_nas_fplmn_list_update
(
  sys_plmn_id_s_type    plmn,
  boolean               is_added
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_fplmn_list

DESCRIPTION

  Function fills & writes a UMTS NAS FPLMN LIST LOG PACKET for QXDM.
  This is to report the current FPLMN List.

RETURN VALUE

  None

==============================================================================*/

extern void reg_log_umts_nas_fplmn_list
(
  const reg_sim_plmn_list_s_type* fplmn_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_timer_start

DESCRIPTION

  Function that fills and writes a LOG PACKET 
  of type UMTS NAS HPLMN SEARCH TIMER START for QXDM.
  This is to report the start of HPLMN timer.
  Timer value (in minutes) is given in the log packet.

RETURN VALUE

  None

==============================================================================*/

extern void reg_log_umts_nas_hplmn_search_timer_start
(
  dword   hplmn_timer
);

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_timer_expire

DESCRIPTION

  Function that fills and writes a LOG PACKET 
  of type UMTS NAS HPLMN SEARCH TIMER EXPIRY for QXDM.
  This is to report the expiry of HPLMN timer.

RETURN VALUE

  None

==============================================================================*/

extern void reg_log_umts_nas_hplmn_search_timer_expire
(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
  sys_modem_as_id_e_type as_id
#else
  void
#endif
);

#ifdef FEATURE_GTOW_BACKGROUND_PLMN_SEARCH
/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_start

DESCRIPTION

  Function that fills and writes a LOG PACKET 
  of type UMTS NAS HPLMN SEARCH START for QXDM.
  This is to report that Higher priority PLMN search is started.
  LOG contains the list of higher priority PLMNs.

RETURN VALUE

  None

==============================================================================*/

extern void reg_log_umts_nas_hplmn_search_start
(
  const sys_plmn_rat_list_s_type* pplmn_list_p
);

/*==============================================================================

FUNCTION NAME

  reg_log_umts_nas_hplmn_search_end

DESCRIPTION

  Function that fills and writes a LOG PACKET 
  of type UMTS NAS HPLMN SEARCH END for QXDM.
  This is to report that Higher priority PLMN search is finished.
  LOG contains the available PLMN list.

RETURN VALUE

  None

==============================================================================*/

extern void reg_log_umts_nas_hplmn_search_end
(
  const sys_detailed_plmn_list_s_type* avail_plmn_list_p
);

#endif

/*==============================================================================
Function Name: REG_LOG_UMTS_NAS_PPLMN_LIST

Description: This function sends the log packet for Preferred PLMN list

Return Value: None
==============================================================================*/
extern void reg_log_umts_nas_pplmn_list
(
  const reg_sim_plmn_w_act_s_type* pplmn_list_p,
  uint16                           num_opplmn,
  uint16                           num_upplmn,
  uint16                           num_pplmn,
  sys_modem_as_id_e_type           as_id
);

#endif
