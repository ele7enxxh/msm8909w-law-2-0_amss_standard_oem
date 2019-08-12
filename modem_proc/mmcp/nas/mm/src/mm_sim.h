#ifndef _MM_SIM_H
#define _MM_SIM_H
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm_sim.h_v   1.6   08 Feb 2002 00:24:30   vdrapkin  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/src/mm_sim.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/01   jca     Added function prototypes to get/put GMM SIM data.
08/30/02   mks     Added function prototypes to get/put BCCH info in SIM
12/12/02   ks      Added sys_srv_domain_e_type parameter in mmsim_sim_valid()
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mmcp_variation.h"
#include "rrcmmif.h"
#include "rrccmd.h"
#include "gmm_v.h"
#include "sys.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

extern rrc_lo_access_class_e_type mm_sim_get_lo_access_class(void);
extern byte mm_sim_get_hi_access_class(void);

extern void mm_load_csloci_data
(
  void
);

extern void mmsim_get_ps_location_information
(
  gmm_ptmsi_type           *ptmsi,            /* P-TMSI                      */
  gmm_ptmsi_signature_type *ptmsi_signature,  /* P-TMSI signature value      */
  routing_area_id_T        *rai,              /* Routing area identification */
  gmm_update_status_type   *ra_update_status  /* Routing area update status  */
);

extern void mmsim_save_ps_location_information
(
  gmm_ptmsi_type           ptmsi,            /* P-TMSI                      */
  gmm_ptmsi_signature_type ptmsi_signature,  /* P-TMSI signature value      */
  routing_area_id_T        rai,              /* Routing area identification */
  gmm_update_status_type   ra_update_status  /* Routing area update status  */
);

//extern void mmsim_get_ps_ciphering_and_integrity_keys
//(
//  gmm_key_set_id_type    *ksips,              /* PS key set identifier */
//  gmm_ciphering_key_type *ckps,               /* PS ciphering key      */
//  gmm_integrity_key_type *ikps                /* PS integrity key      */
//);

//extern void mmsim_get_gprs_ciphering_key
//(
//  gmm_gprs_ciphering_key_type    *kcgprs,  /* GPRS ciphering key                 */
//  gmm_gprs_cksn_type             *cksn     /* GPRS ciphering key sequence number */
//);

extern boolean mmsim_sim_valid( sys_srv_domain_e_type domain);

void mmsim_write_loci(void);
void mmsim_mark_sim_as_illegal(void);


boolean mm_sim_open_session
(
  mmgsdi_session_type_enum_type session_type,
  mmgsdi_client_id_type client_id
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
);

boolean mm_sim_read (mmgsdi_file_enum_type ef, byte *data, dword data_length);
boolean mm_sim_write(mmgsdi_file_enum_type ef, byte *data, dword data_length);

#if defined(FEATURE_DUAL_SIM) ||defined(FEATURE_SGLTE)
extern uint8 mm_sim_max_subscription;
#endif 

#ifdef FEATURE_FEMTO_CSG


/*diplay indicator structure*/
typedef struct mm_sim_display_indicator_info_s
{
  sys_plmn_id_s_type plmn;
  byte              indicator;
}mm_sim_display_indicator_info_s_type;

typedef struct mm_sim_display_indicator_list_s
{
  mm_sim_display_indicator_info_s_type *list;
  int     length;
}mm_sim_display_indicator_list_s_type;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern sys_csg_list_s_type             mm_sim_active_operator_csg_list_sim[MAX_NAS_STACKS];
#define  mm_sim_active_operator_csg_list  mm_sim_active_operator_csg_list_sim[mm_as_id]

extern sys_csg_list_s_type             mm_sim_active_allowed_csg_list_sim[MAX_NAS_STACKS];
#define  mm_sim_active_allowed_csg_list  mm_sim_active_allowed_csg_list_sim[mm_as_id]

extern byte                       mm_sim_operator_csg_list_read_started_sim[MAX_NAS_STACKS];
#define  mm_sim_operator_csg_list_read_started  mm_sim_operator_csg_list_read_started_sim[mm_as_id]

extern byte                       mm_sim_allowed_csg_list_read_started_sim[MAX_NAS_STACKS];
#define  mm_sim_allowed_csg_list_read_started  mm_sim_allowed_csg_list_read_started_sim[mm_as_id]

extern byte                       mm_ad_csg_indicator_sim[MAX_NAS_STACKS];
#define  mm_ad_csg_indicator  mm_ad_csg_indicator_sim[mm_as_id]
 
extern mm_sim_display_indicator_list_s_type  mm_sim_active_csg_indicator_list_sim[MAX_NAS_STACKS];
#define  mm_sim_active_csg_indicator_list  mm_sim_active_csg_indicator_list_sim[mm_as_id]

extern boolean mm_using_allowed_csg_nv_sim[MAX_NAS_STACKS];
#define  mm_using_allowed_csg_nv  mm_using_allowed_csg_nv_sim[mm_as_id]

#else
extern sys_csg_list_s_type             mm_sim_active_operator_csg_list;
extern sys_csg_list_s_type             mm_sim_active_allowed_csg_list;
extern byte                       mm_sim_operator_csg_list_read_started;
extern byte                       mm_sim_allowed_csg_list_read_started;
extern byte                       mm_ad_csg_indicator;
extern mm_sim_display_indicator_list_s_type  mm_sim_active_csg_indicator_list;
extern boolean mm_using_allowed_csg_nv;
#endif

/*===========================================================================

FUNCTION mm_sim_read_allowed_csg_from_nv

DESCRIPTION
  

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void mm_sim_read_allowed_csg_from_nv(sys_csg_list_s_type *csg_list);



/*===========================================================================

FUNCTION mm_sim_write_allowed_csg_to_nv

DESCRIPTION
  

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void mm_sim_write_allowed_csg_to_nv(sys_csg_list_s_type *csg_list);

/*===========================================================================

FUNCTION mmsim_read_csg_list

DESCRIPTION


DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void mm_sim_read_csg_list(void);

/*===========================================================================

FUNCTION mmsim_process_get_file_attr_cnf

DESCRIPTION
  This function process get file attr cnf 

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void mmsim_process_get_file_attr_cnf(mm_cmd_type *message);

/*===========================================================================

FUNCTION mmsim_process_read_file_cnf

DESCRIPTION
  

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void mmsim_process_read_file_cnf(mm_cmd_type *message);

/*===========================================================================

FUNCTION mmsim_update_allowed_csg_list

DESCRIPTION


DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void mmsim_update_allowed_csg_list
(
  sys_plmn_id_s_type plmn_id,
  sys_csg_info_s_type  *csg_info,
  mmgsdi_update_acsgl_enum_type type
);

/*===========================================================================

FUNCTION mmsim_reset_csg_data

DESCRIPTION
  This function is used to reset the CSG data in cache

DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
extern void mmsim_reset_csg_data(void);

#endif /*FEATURE_FEMTO_CSG*/

#ifdef FEATURE_ENHANCED_NW_SELECTION 
extern boolean mmsim_is_ef_rat_present(void);
#endif 

extern boolean is_mcc_invalid(sys_plmn_id_s_type plmn_id );

/*===========================================================================
FUNCTION      MM_UPDATE_SIM_STATE

DESCRIPTION
  updates the sim state
 
DEPENDENCIES
  None 

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void mm_update_sim_state
(
  sys_sim_state_e_type  sim_state
);


#endif /* _MM_SIM_H */
