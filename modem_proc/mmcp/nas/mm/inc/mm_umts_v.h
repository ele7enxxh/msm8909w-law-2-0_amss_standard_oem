#ifndef _MM_UMTS_V_H
#define _MM_UMTS_V_H
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm_umts.h_v   1.16   13 Jun 2002 12:09:24   jault  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/mm_umts_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/12/01   jca     Modified mm_send_nas_msg_to_rrc() function prototype.
04/24/01   sbk     Added function prototypes for rrc conn management
05/03/01   jca     Rewrote functions for RRC<->MM PLMN type conversion.
05/11/01   jca     Added establishment cause to send_L3_message_to_rrc().
09/18/01   sbk     New macros to set bits for the CSN.1 encoding
04/23/02   jca     Deleted redundant function mm_send_nas_msg_to_rrc().
06/05/02   ks      Added support for Dual Mode interfaces.
09/05/02   mks     Updated the function prototype for mm_check_for_integrity_protection()
12/04/03   jca     Deleted unused function get_lai_from_service_info().
01/21/05   ks      Added function prototype for mm_waiting_for_nw_command()
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "mm_v.h"
#include "mm_umts.h"
#include "sys.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/




/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

rrc_initial_ue_id_type mm_get_initial_ue_id(rrc_initial_ue_id_choice_e_type *choice);


rrc_establish_cause_e_type convert_est_cause_to_rrc(establishment_cause_T est_cause);

VOID_FUNC   mm_initiate_rrc_connection(
                              rrc_establish_cause_e_type establishment_cause,
                              IMH_T * message );

void fill_rrc_forbidden_lai_list(rrc_lai_list_type *);

void convert_rrc_service_cnf_to_rr_plmn_select_cnf(IMH_T *message);

void mmconvert_rrc_lai_type_to_inter_task_lai( inter_task_lai_T *rr_lai,
                                               rrc_lai_type * rrc_lai);


void convert_lai_list_to_rr( rrc_lai_list_type     *rrc_list,
                             inter_task_lai_list_T *rr_list);

void convert_lai_list_cnf_to_rr(IMH_T *message);

boolean mm_check_for_integrity_protection(mm_cmd_type *msg_p);

void convert_rrc_service_ind_to_rr(IMH_T *message);

void mm_send_nas_msg_to_rrc_by_domain(byte *nas_msg_p,
                                      rrc_cn_domain_identity_e_type domain);

boolean mm_get_hplmn(rrc_plmn_identity_type *home_plmn);



/*
 * EXTERNS
 */
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern boolean umts_security_context_sim[MAX_NAS_STACKS];

#define  umts_security_context umts_security_context_sim[mm_as_id]

#else
extern boolean umts_security_context;
#endif /* FEATURE_DUAL_SIM || FEATURE_SGLTE*/

boolean has_valid_tmsi(void);
uint32 get_tmsi_as_number(void);

rrc_imsi_gsm_map_type convert_imsi_to_digits(imsi_data_T imsi);
rrc_imei_gsm_map_type get_imei_as_digits(void);

/* -------------------
** Function prototypes
** ------------------- */
void send_rrc_est_req( rrc_cn_domain_identity_e_type cn_domain,
                       rrc_establish_cause_e_type cause );
void send_open_session_req( rrc_cn_domain_identity_e_type cn_domain );
//#if defined (FEATURE_GSM) || defined (FEATURE_WCDMA)
void coord_rrc_conn_management( mm_cmd_type *message );
//#endif /*defined (FEATURE_GSM) || defined (FEATURE_WCDMA)*/

/* Peek at certain commands sent to RRC */
void send_cmd_to_rrc
( 
  rrc_cmd_type                 *cmd_buf,
  sys_radio_access_tech_e_type rat
);

/* Used by GMM to send an initial L3 message by establishing RRC connection */
extern void mmrrcconn_send_L3_message_to_rrc
(
  void                          *message,
  rrc_establish_cause_e_type    cause,
  rrc_cn_domain_identity_e_type domain
);

/* Used to send an OTA message to send to the active low level layer */
void mm_send_ota_message( byte *nas_msg_p, dword length);





uint32 mm_pack_tmsi(byte *id_p);
#ifdef FEATURE_NAS_REL11 
uint32 mm_pack_ptmsi(byte * write_ptr);
#endif

uint32 mm_pack_imsi(byte *write_ptr);
imsi_data_T mm_read_emergency_imsi(void);
#endif /* _MM_UMTS_H */
