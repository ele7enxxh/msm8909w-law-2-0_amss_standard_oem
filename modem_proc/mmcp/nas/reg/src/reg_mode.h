#ifndef REG_MODE_H
#define REG_MODE_H
/*==============================================================================


                   R E G  -  M O D E   H E A D E R   F I L E


                                  DESCRIPTION

This header file defines the types and functions required to implement the
functionality associated with each network selection mode.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_mode.h_v   1.3   06 Jun 2002 17:31:38   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_mode.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/13/02   kwa     Initial release
05/29/02   kwa     Changes to support sending a single RAT rather than a mode
                   preference in the mmr_reg_req message.
05/31/02   kwa     Added functionality to initialize the equivalent PLMN list
                   from NVRAM and save the equivalent PLMN list to NVRAM.
06/06/02   kwa     Replaced rat_search_order_p with rat_p in 
                   reg_mode_next_higher_priority_plmn
09/23/02   kwa     Added function reg_mode_allowed_service_domain to handle
                   the allowed service domain if a PLMN is forbidden for GPRS
                   services.
10/01/02   kwa     Added req_service_domain to definition of 
                   reg_mode_start_limited_service_f_type.
12/31/02   kwa     Added function reg_mode_equivalent_plmn.
04/03/03   kwa     Added function reg_mode_higher_priority_network_available.
05/21/03   kwa     Fixed assorted lint warnings.
06/04/03   kwa     Fixed assorted lint warnings.
09/07/03   kwa     Added function reg_mode_reinitiate_plmn_selection.
09/18/03   kwa     Added sys_band_mask_type to various functions.
10/22/03   kwa     Removed the parameters update_equivalent_plmn_list and
                   equivalent_plmn_list_p from reg_mode_finished_f_type.
11/16/03   kwa     Added REG_MODE_AUTOMATIC_MODE_MANUAL, 
                   reg_mode_available_plmn_list_get, and
                   reg_mode_initial_manual_rat.
11/19/03   kwa     Removed obsolete functions for retrieving the mode name.
01/20/04   kwa     Added reg_mode_start_plmn_selection_on_specified_plmn to
                   properly handle a location updating reject with cause #15.
01/22/04   kwa     Added prototype for reg_mode_network_selection_mode_set.
02/04/04   kwa     Added prototype for reg_mode_ps_for_request_get.
                   Added support for PS follow on request.
05/16/04   kwa     Added prototype for reg_mode_cs_for_request_get.
                   Added support for CS follow on request.
05/20/04   kwa     Removed the static variable reg_mode_service_state and
                   associated functions.  Added the static variable
                   reg_mode_req_rat and associated functions.
11/12/04   kwa     Added the parameter higher_priority_plmn_list_p to
                   reg_mode_higher_priority_network_available.
01/24/05   up      Added function reg_mode_req_mode_pref_get to get actual
                   preferred mode.
02/17/05   up      Modified reg_mode_start_manual_plmn_selection_f_type to add
                   a new argument of type sys_network_selection_mode_e_type.
02/22/05   up      Updated for Cingular ENS feature.
                   Added functions reg_mode_restart_plmn_selection. 
                   Updated reg_mode_initialize_equivalent_plmn_list 
                   to delete eplmn list forcefully.
07/27/06   sn      Added Uniform OOS changes.
                   FEATURE_UMTS_UNIFORM_OOS_HANDLING
10/05/06   sn      Added WCDMA_1X mode preference handling function.
11/23/06   sn      FEATURE_PRESERVE_MANUAL_PLMN_LIST
12/15/09   abhi   Removing UOOS code
01/31/11   rajesh  voice domain preference and UE usage setting related changes
11/15/11  abhi   For EFRPLMNSI Selecting RLPMN if HPLMN is not found
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity
11/28/12  am       Fixing compilation warning
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "reg_cm_v.h"
#include "reg_mm_v.h"
#include "sys.h"
#include "sys_v.h"
#include "sys_plmn_selection_v.h"
#include "reg_sim_v.h"
#include "mm_v.h"
/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

typedef enum reg_mode_e
{
  REG_MODE_NONE = -1,                            /* FOR INTERNAL USE ONLY! */
  REG_MODE_AUTOMATIC_PLMN_SELECTION,
  REG_MODE_LIMITED_SERVICE,
  REG_MODE_HIGHER_PRIORITY_PLMN_SELECTION,
  REG_MODE_MANUAL_PLMN_SELECTION,
  REG_MODE_PLMN_SELECTION,
  REG_MODE_AUTOMATIC_CSG_SELECTION,
  REG_MODE_MANUAL_CSG_SELECTION,
  REG_MODE_MAX                                   /* FOR INTERNAL USE ONLY! */
} reg_mode_e_type;

typedef enum reg_mode_automatic_mode_e
{
  REG_MODE_AUTOMATIC_MODE_NONE = -1,             /* FOR INTERNAL USE ONLY! */
  REG_MODE_AUTOMATIC_MODE_NORMAL,
  REG_MODE_AUTOMATIC_MODE_USER_RESELECTION,
  REG_MODE_AUTOMATIC_MODE_HIGH_PRIORITY_SEARCH,
  REG_MODE_AUTOMATIC_MODE_MANUAL,
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
  REG_MODE_AUTOMATIC_MODE_SUITABLE_SEARCH,
#endif
  REG_MODE_AUTOMATIC_MODE_BETTER_SYSTEM_RESELECTION,
  REG_MODE_AUTOMATIC_MODE_EF_RAT_REFRESH,
  REG_MODE_AUTOMATIC_MODE_MMSS_HPLMN,
  REG_MODE_AUTOMATIC_MODE_MMSS_RPLMN,
  REG_MODE_AUTOMATIC_MODE_NO_PREFERENCE,
  REG_MODE_AUTOMATIC_MODE_IRAT_HIGH_PRIORITY_SEARCH,
  REG_MODE_AUTOMATIC_MODE_ROAMING_PLMN,
  REG_MODE_AUTOMATIC_MODE_CAMPED_PLMN_RAT,
  REG_MODE_AUTOMATIC_MODE_MAX                    /* FOR INTERNAL USE ONLY! */
} reg_mode_automatic_mode_e_type;

/**********************************
** Enhanced HPLMN Search
**********************************/
#define MAX_MCC_IN_SET 20
#define MAX_MCC_SET_IN_TBL 5

/*********************************
** Invalid EARFCN 
*********************************/

#define INVALID_EARFCN     0xFFFFFFFF                                 

/* Configuration values */
/* Set config parameter in hdr to any of these values
** to give info of the manner in which tbl is stored
*/
#define TBL_MCC_SET 1
#define TBL_ALL_MCC 2
/*
** Function pointers used in the definition of reg_mode_s_type.
*/
typedef void reg_mode_start_automatic_plmn_selection_f_type
(
  sys_network_selection_mode_e_type    network_selection_mode,
  reg_mode_automatic_mode_e_type       automatic_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
  sys_srv_domain_e_type                req_service_domain,
  sys_band_mask_type                   band_pref
#else
  sys_srv_domain_e_type                req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
);

typedef boolean reg_mode_start_higher_priority_plmn_selection_f_type
(
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
);
typedef boolean reg_mode_start_higher_priority_irat_plmn_selection_f_type
(
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
);
typedef void reg_mode_start_limited_service_f_type
(
  boolean                              cs_for_request,
  boolean                              ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
  sys_srv_domain_e_type                req_service_domain,
  sys_band_mask_type                   band_pref
#else
  sys_srv_domain_e_type                req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type           type
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif 
);

typedef void reg_mode_start_manual_plmn_selection_f_type
(
  sys_network_selection_mode_e_type network_selection_mode,
  sys_plmn_id_s_type                   plmn,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                   band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type        type
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif 

);

typedef void reg_mode_update_plmn_selection_mode_f_type
(
   reg_mode_automatic_mode_e_type  plmn_selection_mode
);

typedef void reg_mode_start_plmn_selection_f_type
(
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
  boolean                              clear_hplmn_search_timer
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                   band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type        type
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
);

typedef void reg_mode_start_plmn_selection_on_specified_plmn_f_type
(
  sys_plmn_id_s_type                   plmn,
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
#ifdef FEATURE_GSM_BAND_PREF
  sys_band_mask_type                   band_pref,
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif
  sys_radio_access_tech_e_type         active_rat
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
, mmr_reg_req_e_type reg_req_type
);

typedef void reg_mode_start_foreground_plmn_search_f_type
(
  sys_network_selection_mode_e_type    network_selection_mode,  
  sys_mode_pref_e_type                 mode_pref,               
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  , sys_band_mask_type                 band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  , const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
);


#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
typedef void reg_mode_start_suitable_cell_selection_f_type
(
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                  band_pref
#endif
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif

);
#endif 

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================

FUNCTION NAME

  reg_mode_start_automatic_csg_selection

==============================================================================*/

typedef void reg_mode_start_automatic_csg_selection_f_type
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type          plmn,
  sys_radio_access_tech_e_type  rat,
  sys_csg_id_type                          csg_id,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info 
#endif 
);

/*==============================================================================
FUNCTION POINTER:  reg_mode_start_automatic_csg_selection_app_f_type
==============================================================================*/
typedef void reg_mode_start_automatic_csg_selection_app_f_type
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type   *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean                             sms_only,
  uint32                               additional_info 
#endif 
);

/*==============================================================================

FUNCTION NAME

  reg_mode_start_manual_csg_selection_f_type

==============================================================================*/

typedef void reg_mode_start_manual_csg_selection_f_type
(
  sys_network_selection_mode_e_type network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type                   plmn,
  sys_radio_access_tech_e_type  rat,
  sys_csg_id_type                          csg_id,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info 
#endif 
);

/*==============================================================================
FUNCTION POINTER:  reg_mode_start_manual_csg_selection_app_f_type
==============================================================================*/
typedef void reg_mode_start_manual_csg_selection_app_f_type
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type                   plmn,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type   *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean                             sms_only,
  uint32                               additional_info 
#endif 
);

#endif  /*FEATURE_FEMTO_CSG*/


typedef boolean reg_mode_finished_f_type
(
        sys_plmn_service_state_s_type  service_state,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
);

typedef reg_mode_e_type reg_mode_number_f_type
(
  void
);

/*
** Structure that contains pointers to the functions that define the
** functionality supported by each mode.
*/
typedef struct reg_mode_s
{
  reg_mode_start_automatic_plmn_selection_f_type*              start_automatic_plmn_selection;
  reg_mode_start_higher_priority_plmn_selection_f_type*        start_higher_priority_plmn_selection;
  reg_mode_start_limited_service_f_type*                       start_limited_service;
  reg_mode_start_manual_plmn_selection_f_type*                 start_manual_plmn_selection;
  reg_mode_start_plmn_selection_f_type*                        start_plmn_selection;
  reg_mode_start_plmn_selection_f_type*                        start_plmn_selection_roaming_plmn;
  reg_mode_start_plmn_selection_on_specified_plmn_f_type*      start_plmn_selection_on_specified_plmn;
  reg_mode_start_foreground_plmn_search_f_type*                start_foreground_plmn_search;
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
  reg_mode_start_suitable_cell_selection_f_type*               start_suitable_cell_selection;
#endif
  reg_mode_start_higher_priority_irat_plmn_selection_f_type*   start_higher_priority_irat_plmn_selection;
#ifdef FEATURE_FEMTO_CSG
  reg_mode_start_automatic_csg_selection_f_type*               start_automatic_csg_selection;
  reg_mode_start_automatic_csg_selection_app_f_type*           start_automatic_csg_selection_app;
  reg_mode_start_manual_csg_selection_f_type*                  start_manual_csg_selection;
  reg_mode_start_manual_csg_selection_app_f_type*              start_manual_csg_selection_app;
#endif 
  reg_mode_update_plmn_selection_mode_f_type*                  update_plmn_selection_mode;
  reg_mode_finished_f_type*                                    finished;
  reg_mode_number_f_type*                                      number;
} reg_mode_s_type;

/**********************************
** Enhanced HPLMN Search
**********************************/

typedef struct reg_mode_srch_tbl_mcc_set_s
{
  sys_mcc_type mcc[MAX_MCC_IN_SET];
  byte no_of_mcc; /* No. of mnc records */ 
} reg_mode_srch_tbl_mcc_set_s_type;


typedef struct reg_mode_srch_tbl_hdr_s
{
  byte config;    /* Configuration parameters */
  byte no_of_rec; /* number of MCC records in tbl */

} reg_mode_srch_tbl_hdr_s_type;

typedef struct reg_mode_srch_tbl_s
{
  reg_mode_srch_tbl_hdr_s_type hdr;
  /*
  * Union of all different configurations 
  * Now only MCC SET configuration is supported
  */
  union
  {
    reg_mode_srch_tbl_mcc_set_s_type mcc_set[MAX_MCC_SET_IN_TBL];
  } tbl;
} reg_mode_srch_tbl_s_type;
/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*
** A reg_mode_s_type is declared for each of the network selection modes.
*/
extern reg_mode_s_type reg_mode_automatic_plmn_selection;
extern reg_mode_s_type reg_mode_higher_priority_plmn_selection;
extern reg_mode_s_type reg_mode_limited_service;
extern reg_mode_s_type reg_mode_manual_plmn_selection;
extern reg_mode_s_type reg_mode_plmn_selection;
#ifdef FEATURE_FEMTO_CSG
extern reg_mode_s_type reg_mode_automatic_csg_selection;
extern reg_mode_s_type reg_mode_manual_csg_selection;
#endif 

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_mode_output_detailed_plmn_list

DESCRIPTION

  Function that outputs a detailed plmn list using debug messages.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_output_detailed_plmn_list
(
  const sys_detailed_plmn_list_s_type* plmn_list_p
#ifdef FEATURE_FEMTO_CSG
  ,boolean csg_list
#endif 
);


/*==============================================================================

FUNCTION NAME

  reg_mode_network_selection_mode_get

DESCRIPTION

  Function that returns the network selection mode maintained within reg_mode.

RETURN VALUE

  sys_network_selection_mode_e_type Network selection mode maintained within
                                    reg_mode.

==============================================================================*/

extern sys_network_selection_mode_e_type reg_mode_network_selection_mode_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_network_selection_mode_set

DESCRIPTION

  Function that sets the network selection mode maintained within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_network_selection_mode_set
(
  sys_network_selection_mode_e_type network_selection_mode
);


/*==============================================================================

FUNCTION NAME

  reg_mode_mode_pref_get

DESCRIPTION

  Function that returns the requested mode preference maintained
  within reg_mode.

RETURN VALUE

  sys_mode_pref_e_type Requested mode preference maintained within reg_mode.

==============================================================================*/

extern sys_mode_pref_e_type reg_mode_mode_pref_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_mode_scan_scope_get

DESCRIPTION

  Function that returns the scan scope typre requested CM
RETURN VALUE

  sys_scan_scope_e_type requested scan scope type

==============================================================================*/
extern sys_scan_scope_e_type reg_mode_scan_scope_get              
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_mode_scan_scope_set
DESCRIPTION


  Function that sets the scan scope type requested by CM

RETURN VALUE

none

==============================================================================*/
extern void reg_mode_scan_scope_set
(
  sys_scan_scope_e_type scan_scope
);

#ifdef FEATURE_DUAL_DATA
/*==============================================================================

FUNCTION NAME

  reg_mode_volte_only_ps_set

==============================================================================*/

extern void reg_mode_volte_only_ps_set
(
  boolean is_volte_only_ps
);


/*==============================================================================

FUNCTION NAME

  reg_mode_volte_only_ps_get

==============================================================================*/

extern boolean reg_mode_volte_only_ps_get
(
  void
);

#endif

/*==============================================================================

FUNCTION NAME

  reg_mode_req_mode_pref_get

DESCRIPTION

  Function that returns the original mode preference, maintained in reg_mode.

RETURN VALUE

  sys_mode_pref_e_type Original mode preference maintained within reg_mode.

==============================================================================*/

extern sys_mode_pref_e_type reg_mode_req_mode_pref_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_mode_req_mode_pref_set

DESCRIPTION

  Function sets the mode preference to the new value.

RETURN VALUE

  Void.

==============================================================================*/

extern void reg_mode_req_mode_pref_set
(
  sys_mode_pref_e_type
);

/*==============================================================================

FUNCTION NAME

  reg_mode_is_dual_mode

DESCRIPTION

  Function that checks if the user selected mode is dual or single.

RETURN VALUE

  boolean. TRUE if it is dual and FALSE otherwise.

==============================================================================*/

extern boolean reg_mode_is_dual_mode
(
  sys_mode_pref_e_type
);


/*==============================================================================

FUNCTION NAME

  reg_mode_cs_for_request_get

DESCRIPTION

  Function that returns the CS follow on request flag maintained
  within reg_mode.

RETURN VALUE

  boolean Follow on request flag maintained within reg_mode.

==============================================================================*/

extern boolean reg_mode_cs_for_request_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_ps_for_request_get

DESCRIPTION

  Function that returns the PS follow on request flag maintained
  within reg_mode.

RETURN VALUE

  boolean Follow on request flag maintained within reg_mode.

==============================================================================*/

extern boolean reg_mode_ps_for_request_get
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_mode_call_for_bit_set

DESCRIPTION

  Function that sets the CS follow on request and PS follow on request flag maintained
  within reg_mode to latest value

RETURN VALUE

Void

==============================================================================*/

extern void reg_mode_call_for_bit_set
(
  boolean cs_for_request,
  boolean ps_for_request
);


#ifdef FEATURE_GSM_BAND_PREF
/*==============================================================================

FUNCTION NAME

  reg_mode_band_pref_get

DESCRIPTION

  Function that returns the requested band preference maintained
  within reg_mode.

RETURN VALUE

  sys_band_mask_type   Requested band preference maintained within reg_mode.

==============================================================================*/

extern sys_band_mask_type reg_mode_band_pref_get
  
(
  void
);


#endif

#ifdef FEATURE_RAT_PRIORITY_LIST

/*==============================================================================

FUNCTION NAME

  reg_mode_rat_pri_list_get

DESCRIPTION

  Function that returns the rat priority list  maintained within reg_mode

RETURN VALUE

  sys_rat_pri_list_info_s_type   Requested band preference maintained within reg_mode.

==============================================================================*/

extern sys_rat_pri_list_info_s_type * reg_mode_rat_pri_list_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_mode_bst_band_match

DESCRIPTION

  Function compares bst band capability field of a particular rat for the two
  rat priority lists that are passed.

RETURN VALUE
  boolean

==============================================================================*/
extern boolean reg_mode_bst_band_match
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_1, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_2,
  sys_sys_mode_e_type acq_sys_mode
);


/*==============================================================================

FUNCTION NAME

  reg_mode_rat_pri_list_match

DESCRIPTION

  Function compares two rat priority lists for match. Two lists match if all rat and band 
  combination in one is also present in other

RETURN VALUE
  

==============================================================================*/
extern boolean reg_mode_rat_pri_list_match
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_1, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_2
);

/*==============================================================================

FUNCTION NAME

  reg_mode_rat_pri_list_order_match

DESCRIPTION

  Function returns true if two rat priority lists match exactly(Order is also taken into consideration while matching).

RETURN VALUE
  

==============================================================================*/
extern boolean reg_mode_rat_pri_list_order_match
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_1, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_2
);
/*==============================================================================

FUNCTION NAME

  reg_mode_is_rat_pri_list_empty

==============================================================================*/
boolean reg_mode_is_rat_pri_list_empty
(
  sys_rat_pri_list_info_s_type *rat_pri_list
);


/*==============================================================================

FUNCTION NAME

  reg_mode_rat_pri_list_3gpp_sys_mode_match

  This function compares the rat priority lists passed to see both rat priority list has same 3GPP rat acq mode.
  Eg. GW to GWL, GW to G, GL to GW etc. will return FALSE
  Rat order change(GWL to LGW) or band capability change will not be considered, function will still return true if there is
  no change in of 3GPP sys mode entries.

==============================================================================*/
boolean reg_mode_rat_pri_list_3gpp_sys_mode_match
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_1, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_2
);


/*==============================================================================

FUNCTION NAME

  reg_mode_req_mode_pref_set

DESCRIPTION
  Set ue capability maintained within reg_mode
  
RETURN VALUE  

==============================================================================*/

extern void reg_mode_rat_pri_list_set
(
   const sys_rat_pri_list_info_s_type *rat_pri_list_info
);

/*==============================================================================

FUNCTION NAME

  reg_mode_rat_pri_list_reset

==============================================================================*/

void reg_mode_rat_pri_list_reset
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_mode_bst_status_set

DESCRIPTION
  Set the BST STATUS so as to send the corresponding rat priority list
  to the lower layers
  
RETURN VALUE  

==============================================================================*/

extern void reg_mode_bst_status_set
(
  boolean bst_status
);

/*==============================================================================

FUNCTION NAME

  reg_mode_bst_status_get

DESCRIPTION
  To get the BST status
  
RETURN VALUE  
  boolean
==============================================================================*/

extern boolean reg_mode_bst_status_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_is_bst_set_for_rat

DESCRIPTION

  This function checks if the bst info is set for the rat passed as the parameter.

RETURN VALUE
  TRUE if bst set for rat, FALSE otherwise

==============================================================================*/

extern boolean reg_mode_is_bst_set_for_rat
(
  const sys_rat_pri_list_info_s_type *rat_pri_list,
  const sys_radio_access_tech_e_type rat
);



/*==============================================================================

FUNCTION NAME

  reg_mode_get_3gpp_rat_capabilty

DESCRIPTION
  Extracts 3GPP rat capabilty from UE capabilty 
  
RETURN VALUE  

==============================================================================*/

extern void reg_mode_get_3gpp_rat_capabilty
(
  reg_mode_3gpp_capability_s_type *rat_3gpp_capabilty,
  const sys_rat_pri_list_info_s_type *rat_pri_list_info
#ifdef FEATURE_SGLTE
      ,boolean sglte_rat_filter
#endif                                  
);


/*==============================================================================

FUNCTION NAME

  reg_mode_is_rat_present_in_rat_pri_list

DESCRIPTION
  Search for rat to be prsent in rat priority list. If use_bst is TRUE then bst_rat_acq_required is 
  considered to decided if RAT is present or not.
  
RETURN VALUE  

==============================================================================*/

extern boolean reg_mode_is_rat_present_in_rat_pri_list
(
  sys_radio_access_tech_e_type rat,
  const sys_rat_pri_list_info_s_type *rat_pri_list_info,
  boolean use_bst
);

#ifdef FEATURE_LTE

/*===========================================================================
FUNCTION    reg_bit_mask_check

DESCRIPTION
  Function checks value passed to see if particular element  is set

DEPENDENCIES
  None

RETURN VALUE
  returns TRUE if particular bit is set, FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/

extern boolean reg_bit_mask_check
(
  uint32 value,
  byte bit_mask
);


/*==============================================================================

FUNCTION NAME

  reg_mode_is_only_req_rat_present_in_rat_pri_list

==============================================================================*/

extern boolean reg_mode_is_only_req_rat_present_in_rat_pri_list
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info, sys_sys_mode_e_type rat_sys_mode
);
#endif
/*==============================================================================

FUNCTION NAME

  reg_mode_convert_acq_mode_to_access_tech

DESCRIPTION
  
  Function to convert acq mode to access tech.
  
RETURN VALUE  

==============================================================================*/

extern sys_radio_access_tech_e_type reg_mode_convert_acq_mode_to_access_tech
(
   sys_sys_mode_e_type acq_sys_mode
);

#ifdef FEATURE_EOOS
#error code not present
#endif /*FEATURE_EOOS*/

#endif /*FEATURE_RAT_PRIORITY_LIST*/

#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
/*==============================================================================

FUNCTION NAME

  reg_mode_start_suitable_cell_selection

DESCRIPTION
  
  Function to start suitable cell selection process
  
RETURN VALUE  

==============================================================================*/

void reg_mode_start_suitable_cell_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                   band_pref
#endif
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif
);


#endif 
/*==============================================================================

FUNCTION NAME

  reg_mode_req_service_domain_get

DESCRIPTION

  Function that returns the requested service domain maintained within reg_mode.

RETURN VALUE

  sys_srv_domain_e_type Requested service domain maintained within reg_mode.

==============================================================================*/

extern sys_srv_domain_e_type reg_mode_req_service_domain_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_allowed_service_domain

DESCRIPTION

  Function that returns the allowed service domain for the specified PLMN.

RETURN VALUE

  sys_srv_domain_e_type Allowed service domain for the specified PLMN.

==============================================================================*/

extern sys_srv_domain_e_type reg_mode_allowed_service_domain
(
  sys_plmn_id_s_type                   plmn,
  sys_srv_domain_e_type                req_service_domain
);


/*==============================================================================

FUNCTION NAME

  reg_mode_req_plmn_get

DESCRIPTION

  Function that returns the requested PLMN maintained within reg_mode.

RETURN VALUE

  sys_plmn_id_s_type Requested PLMN maintained within reg_mode.

==============================================================================*/

extern sys_plmn_id_s_type reg_mode_req_plmn_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_req_plmn_set

DESCRIPTION

  Function that sets the requested PLMN maintained within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_req_plmn_set
(
  sys_plmn_id_s_type plmn
);

/*==============================================================================

FUNCTION NAME

  reg_mode_req_rat_get

DESCRIPTION

  Function that returns the requested RAT maintained within reg_mode.

RETURN VALUE

  sys_radio_access_tech_e_type  Requested RAT maintained within reg_mode.

==============================================================================*/

extern sys_radio_access_tech_e_type reg_mode_req_rat_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_req_rat_set

DESCRIPTION

  Function that sets the requested RAT maintained within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_req_rat_set
(
  sys_radio_access_tech_e_type rat
);

#ifdef FEATURE_ENHANCED_NW_SELECTION
/*==============================================================================

FUNCTION NAME

  reg_mode_restart_plmn_selection

DESCRIPTION

  Function that restarts the PLMN selection process.

RETURN VALUE

  boolean - Returns true is PLMN rescan is started.

==============================================================================*/
extern boolean reg_mode_restart_plmn_selection
(
  void
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_mode_start_automatic_plmn_selection

DESCRIPTION

  Function that starts the automatic PLMN selection process.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_start_automatic_plmn_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  reg_mode_automatic_mode_e_type       automatic_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
  sys_srv_domain_e_type                req_service_domain,
  sys_band_mask_type                   band_pref
#else
  sys_srv_domain_e_type                req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif
);


/*==============================================================================

FUNCTION NAME

  reg_mode_start_higher_priority_plmn_selection

DESCRIPTION

  Function that starts the higher priority PLMN selection process.

RETURN VALUE

  boolean  Flag indicating whether registration on a higher priority PLMN
           was initiated (TRUE) or not (FALSE).

==============================================================================*/

extern boolean reg_mode_start_higher_priority_plmn_selection
(
  const sys_detailed_plmn_list_s_type*       available_plmn_list_p
);
extern boolean reg_mode_start_higher_priority_irat_plmn_selection
(
  const sys_detailed_plmn_list_s_type*       available_plmn_list_p
);


/*==============================================================================

FUNCTION NAME

  reg_mode_start_limited_service

DESCRIPTION

  Function that starts the search for limited service.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_start_limited_service
(
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
  sys_srv_domain_e_type                req_service_domain,
  sys_band_mask_type                   band_pref
#else
  sys_srv_domain_e_type                req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type            type
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif

);


/*==============================================================================

FUNCTION NAME

  reg_mode_start_manual_plmn_selection

DESCRIPTION

  Function that starts the manual PLMN selection process.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_start_manual_plmn_selection
(
  sys_network_selection_mode_e_type network_selection_mode,
  sys_plmn_id_s_type                   plmn,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                   band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type        type
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif

);


/*==============================================================================

FUNCTION NAME

  reg_mode_start_plmn_selection

DESCRIPTION

  Function that starts the PLMN selection process.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_start_plmn_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                   band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type        type
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif

);


/*==============================================================================

FUNCTION NAME

  reg_mode_start_plmn_selection_on_specified_plmn

DESCRIPTION

  Function that starts the PLMN selection process.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_start_plmn_selection_on_specified_plmn
(
  sys_plmn_id_s_type                   plmn,
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
#ifdef FEATURE_GSM_BAND_PREF
  sys_band_mask_type                   band_pref,
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif
  sys_radio_access_tech_e_type         active_rat
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif
  , mmr_reg_req_e_type reg_req_type
);


/*==============================================================================

FUNCTION NAME

  reg_mode_start_manual_plmn_selection_on_specified_plmn

DESCRIPTION

  Function that starts the PLMN selection process.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_start_manual_plmn_selection_on_specified_plmn
(
  sys_plmn_id_s_type                   plmn,
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
#ifdef FEATURE_GSM_BAND_PREF
  sys_band_mask_type                   band_pref,
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif
  sys_radio_access_tech_e_type         active_rat
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info
#endif
  , mmr_reg_req_e_type reg_req_type
);


/*==============================================================================

FUNCTION NAME

  reg_mode_reinitiate_plmn_selection

DESCRIPTION

  Function that reinitiates the PLMN selection process.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_reinitiate_plmn_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
#ifdef FEATURE_GSM_BAND_PREF
  sys_srv_domain_e_type                req_service_domain,
  sys_band_mask_type                   band_pref
#else
  sys_srv_domain_e_type                req_service_domain
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
#ifdef FEATURE_LTE 
  , boolean sms_only,
    uint32  additional_info
#endif
);

extern void reg_mode_start_foreground_plmn_search
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_mode_finished

DESCRIPTION

  Function that calls the appropriate mode to determine if the PLMN selection
  process has finished.

RETURN VALUE

  boolean  Flag indicating whether the PLMN selection process is finished
           (TRUE) or not (FALSE).

==============================================================================*/

extern boolean reg_mode_finished
(
        sys_plmn_service_state_s_type  service_state,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
);


/*==============================================================================

FUNCTION NAME

  reg_mode_number

DESCRIPTION

  Function that returns the enumerated type associated with the current mode.

RETURN VALUE

  reg_mode_e_type Enumerated type associated with the current mode.

==============================================================================*/

extern reg_mode_e_type reg_mode_number
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_successful_registration

DESCRIPTION

  Function that indicates if the attempted registration was successful.

RETURN VALUE

  boolean  Flag indicating whether the MS registered with the network
           successfully (TRUE) or unsuccessfully (FALSE).

==============================================================================*/

extern boolean reg_mode_successful_registration
(
  sys_plmn_id_s_type                   plmn,
  sys_srv_status_e_type                service_status,
  sys_radio_access_tech_e_type         rat,
  sys_reg_domain_e_type                plmn_reg_type
);


/*==============================================================================

FUNCTION NAME

  reg_mode_initialize_equivalent_plmn_list

DESCRIPTION

  Function that initializes the equivalent PLMN list by reading from NVRAM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_initialize_equivalent_plmn_list
(
#ifdef FEATURE_ENHANCED_NW_SELECTION
  boolean force_delete
#else
  void
#endif
);

/*===========================================================================

FUNCTION  reg_mode_parse_nas_config

DESCRIPTION
  This function parses the nas configuration file read from USIM or NV

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void reg_mode_parse_nas_config
(
  byte *nas_config_byte_stream, 
  int32 file_size
);


/*==============================================================================

FUNCTION NAME

  reg_mode_t3245_efnas_config_behavior

DESCRIPTION

  Function checks if T3245 timer needs to be started after reading neccessary parameters
  from SIM and EFS.

RETURN VALUE

  None

==============================================================================*/
extern void reg_mode_t3245_efnas_config_behavior(void);


/*==============================================================================

FUNCTION NAME

  reg_mode_save_equivalent_plmn_list

DESCRIPTION

  Function that writes the equivalent PLMN list to NVRAM.

RETURN VALUE

  None

==============================================================================*/

extern boolean reg_mode_save_equivalent_plmn_list
(
  sys_plmn_id_s_type                   rplmn,
  sys_plmn_list_s_type*                equivalent_plmn_list_p
);


/*==============================================================================

FUNCTION NAME

  reg_mode_equivalent_plmn

DESCRIPTION

  Function that determines if the serving PLMN is an equivalent PLMN.

RETURN VALUE

  boolean  Flag indicating whether the serving PLMN is an equivalent PLMN
           (TRUE) or not (FALSE).

==============================================================================*/

extern boolean reg_mode_equivalent_plmn
(
  sys_plmn_id_s_type                   rplmn,
  sys_plmn_id_s_type                   serving_plmn
);


/*==============================================================================

FUNCTION NAME

  reg_mode_next_equivalent_rplmn

DESCRIPTION

  Function that returns the highest priority PLMN in the available PLMN list
  maintained within reg_mode that is equivalent to the RPLMN.  

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_next_equivalent_rplmn
(
  sys_plmn_id_s_type*                  plmn_p,
  sys_radio_access_tech_e_type*        rat_p
);

/*==============================================================================   

FUNCTION NAME

  reg_mode_get_rat_order

DESCRIPTION

  Function reads the two bytes of access technology and returns an array of type sys_radio_access_tech_e_type
  with the supported rats.

RETURN VALUE

  uint8 Number of supported rats
  sys_radio_access_tech_e_type* array of supported rats.

==============================================================================*/

extern uint8 reg_mode_get_rat_order
(
#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type                mode_pref,
#else 
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif
  sys_radio_access_tech_e_type *rat,
  reg_sim_plmn_w_act_s_type plmn_w_act,
  reg_mode_3gpp_capability_s_type *rat_list_order
);
/*==============================================================================

FUNCTION NAME

  reg_mode_available_plmn_list_get

DESCRIPTION

  Function that returns a constant pointer to the available PLMN list maintained
  within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern const sys_detailed_plmn_list_s_type* reg_mode_available_plmn_list_get
(
  void
);

#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  reg_mode_get_3GPP_coverage

  FUNCTION PARAMETERS
  sys_radio_access_tech_e_type rat

  DESCRIPTION
  For a given RAT, returns TRUE, if there is a coverage for this RAT. Otherwise,
  it returns FALSE.


==============================================================================*/
boolean reg_mode_get_3GPP_coverage(sys_detailed_plmn_list_s_type available_plmn_list,
                                   sys_radio_access_tech_e_type rat);

#endif
/*==============================================================================

FUNCTION NAME

  reg_mode_available_plmn_list_manual_get

DESCRIPTION

  Function that returns a constant pointer to the available PLMN list manual maintained
  within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern const sys_detailed_plmn_list_s_type* reg_mode_available_plmn_list_manual_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_get_available_backgnd_list

DESCRIPTION

  Function that returns a constant pointer to the available PLMN list manual maintained
  within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern const sys_detailed_plmn_list_s_type* reg_mode_get_available_backgnd_list
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_mode_clear_available_plmn_list

DESCRIPTION

  Function that clears the contents of the available PLMN list maintained
  within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_clear_available_plmn_list
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_reset_available_plmn_list

DESCRIPTION

  Function that first clears the contents of the available PLMN list maintained
  within reg_mode and then seeds the list with the initial PLMN-RAT combinations
  as specified by the PLMN and requested mode preference.

RETURN VALUE

  None

==============================================================================*/
#ifndef FEATURE_RAT_PRIORITY_LIST
extern void reg_mode_reset_available_plmn_list
(
  sys_plmn_id_s_type                   plmn,
  sys_mode_pref_e_type                 req_mode_pref
);
#else
extern void reg_mode_reset_available_plmn_list
(
  sys_plmn_id_s_type                   plmn,
  const reg_mode_3gpp_capability_s_type *rat_3gpp_capabilty
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_mode_update_available_plmn_list

DESCRIPTION

  Function that adds the contents of the available PLMN list to the available
  PLMN list maintained within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_update_available_plmn_list
(
        reg_mode_automatic_mode_e_type       automatic_mode,
#ifndef FEATURE_RAT_PRIORITY_LIST         
        sys_mode_pref_e_type           req_mode_pref,
#else
        const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
#endif 
        sys_srv_domain_e_type                req_service_domain,
  const sys_detailed_plmn_list_s_type*       available_plmn_list_p
);

/*==============================================================================

FUNCTION NAME

  reg_mode_update_available_plmn_list_manual

DESCRIPTION

  Function that adds the contents of the available PLMN list Manual to the available
  PLMN list maintained within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_update_available_plmn_list_manual
(
  reg_mode_automatic_mode_e_type       automatic_mode,
#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type                           req_mode_pref,
#else
  sys_rat_pri_list_info_s_type                   *rat_pri_list_info,
#endif
  sys_srv_domain_e_type                req_service_domain,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p,
  sys_service_search_e_type   service_search
);


/*==============================================================================

FUNCTION NAME

  reg_mode_clear_attempted_plmn_list

DESCRIPTION

  Function that clears the contents of the attempted PLMN list maintained
  within reg_mode.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_clear_attempted_plmn_list
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_update_attempted_plmn_list

DESCRIPTION

  Function that adds the specified PLMN and RAT to the attempted PLMN list.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_update_attempted_plmn_list
(
  sys_plmn_id_s_type                   plmn,
  sys_radio_access_tech_e_type         rat
);


/*==============================================================================

FUNCTION NAME

  reg_mode_next_available_plmn

DESCRIPTION

  Function that returns the highest priority PLMN-RAT combination in the
  available PLMN list maintained within reg_mode.  

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_next_available_plmn
(
  sys_plmn_id_s_type*                  plmn_p,
  sys_radio_access_tech_e_type*        rat_p
);


/*==============================================================================

FUNCTION NAME

  reg_mode_next_higher_priority_plmn

DESCRIPTION

  Function that returns the highest priority PLMN in the available PLMN list
  maintained within reg_mode that also has higher priority than the RPLMN.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_next_higher_priority_plmn
(
#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type                 req_mode_pref,
#endif
  sys_plmn_id_s_type*                  plmn_p,
  sys_radio_access_tech_e_type*        rat_p
);


/*==============================================================================

FUNCTION NAME

  reg_mode_higher_priority_network_available

DESCRIPTION

  Function that determines if a network of higher priority than the RPLMN
  is available in the same country as the RPLMN.

RETURN VALUE

  boolean  Flag indicating whether a higher priority PLMN is available
           (TRUE) or not (FALSE).


==============================================================================*/

extern boolean reg_mode_higher_priority_network_available
(
#if defined(FEATURE_GTOW_BACKGROUND_PLMN_SEARCH) || defined (FEATURE_RAT_PRIORITY_LIST)
  sys_radio_access_tech_e_type active_rat,
  sys_plmn_rat_list_s_type*    higher_priority_plmn_list_p
#else
  void
#endif
);


/*==============================================================================

FUNCTION NAME

  reg_mode_prioritize_plmn_list

DESCRIPTION

  Function that prioritizes a list of PLMNs as specified in 3G TS 23.122.

RETURN VALUE

  None

==============================================================================*/

extern void reg_mode_prioritize_plmn_list
(
  reg_mode_automatic_mode_e_type automatic_mode,
#ifndef FEATURE_RAT_PRIORITY_LIST
  sys_mode_pref_e_type           req_mode_pref,
#else
  const sys_rat_pri_list_info_s_type *rat_pri_list_info,
#endif 
  sys_detailed_plmn_list_s_type* plmn_list_p
);


/*==============================================================================

FUNCTION NAME

  reg_mode_initial_rat

DESCRIPTION

  Function that returns the initial radio access technology to use when 
  searching for service based upon the specified mode preference.

RETURN VALUE

  sys_radio_access_tech_e_type  Initial radio access technology.

==============================================================================*/

extern sys_radio_access_tech_e_type reg_mode_initial_rat
(
  sys_mode_pref_e_type           mode_pref
);

extern boolean reg_mode_get_available_plmn_list_populated(void);
extern void reg_mode_set_available_plmn_list_populated(boolean value);

/*==============================================================================

FUNCTION NAME

  reg_mode_initial_manual_rat

DESCRIPTION

  Function that returns the initial radio access technology to use when 
  searching for service in manual network selection mode.

RETURN VALUE

  sys_radio_access_tech_e_type  Initial radio access technology.

==============================================================================*/
#ifndef FEATURE_RAT_PRIORITY_LIST
    extern sys_radio_access_tech_e_type reg_mode_initial_manual_rat
    (
      sys_plmn_id_s_type             plmn,
      sys_mode_pref_e_type           mode_pref
    );
#else
    sys_radio_access_tech_e_type reg_mode_initial_manual_rat
    (
      sys_plmn_id_s_type             plmn,
      const sys_rat_pri_list_info_s_type *rat_pri_list_info
    );
#endif


extern reg_mode_start_automatic_plmn_selection_f_type         reg_mode_unexpected_start_automatic_plmn_selection;

extern reg_mode_start_higher_priority_plmn_selection_f_type   reg_mode_unexpected_start_higher_priority_plmn_selection;

extern reg_mode_start_higher_priority_irat_plmn_selection_f_type   reg_mode_unexpected_start_higher_priority_irat_plmn_selection;


extern reg_mode_start_limited_service_f_type                  reg_mode_unexpected_start_limited_service;

extern reg_mode_start_manual_plmn_selection_f_type            reg_mode_unexpected_start_manual_plmn_selection;

extern reg_mode_start_plmn_selection_f_type                   reg_mode_unexpected_start_plmn_selection;

extern reg_mode_start_plmn_selection_f_type                   reg_mode_unexpected_start_plmn_selection_roaming_plmn;

extern reg_mode_update_plmn_selection_mode_f_type             reg_mode_update_plmn_selection_mode;

extern reg_mode_start_plmn_selection_on_specified_plmn_f_type reg_mode_unexpected_start_plmn_selection_on_specified_plmn;
extern reg_mode_start_foreground_plmn_search_f_type           reg_mode_unexpected_start_foreground_plmn_search;
#if defined(FEATURE_LTE) || defined(FEATURE_WRLF_SYSTEM_SEL)
extern reg_mode_start_suitable_cell_selection_f_type          reg_mode_unexpected_start_suitable_cell_selection;
#endif

extern void reg_mode_remove_from_attempted_plmn_list
(
  sys_plmn_id_s_type                   plmn,
  sys_radio_access_tech_e_type         rat
);

extern void reg_mode_remove_from_available_plmn_list
(
  sys_plmn_id_s_type                   plmn,
  sys_radio_access_tech_e_type         rat
);

extern reg_cm_scan_status_e_type reg_mode_scan_status_get
(
  void
);

#ifdef FEATURE_UMTS_1X_HANDOVER_UMTSMSM
#error code not present
#endif
extern void reg_mode_copy_available_plmn_list_manual
(
  boolean direction
);

extern void reg_mode_clear_available_plmn_list_manual
(
  void
);

extern void reg_mode_copy_available_plmn_backgnd_list
(
  void
);
extern void reg_mode_clear_plmn_backgnd_list
(
  void
);

/*==============================================================================
FUNCTION NAME  
reg_mode_is_cm_service_request_same_as_last_except_call_for_request
DESCRIPTION
Function that compares the current cm service request with the last mmr reg request sent except cs for request and PS for request flag.
RETURN VALUE  boolean  
Flag indicating result of comparision (TRUE/FALSE)
==============================================================================*/
extern boolean reg_mode_is_cm_service_request_same_as_last_except_call_for_request
(
  const cm_service_req_s_type* msg_p
);

/*==============================================================================
FUNCTION NAME  
reg_mode_rat_pri_list_all_params_match_except_bst_for_LTE
DESCRIPTION  
Function compares two rat priority lists for match for all parameters.
RETURN VALUE    TRUE/FALSE if two lists match or not
==============================================================================*/

extern boolean reg_mode_rat_pri_list_all_params_match_except_bst_for_LTE
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_1, 
  const sys_rat_pri_list_info_s_type *rat_pri_list_2
);

/*==============================================================================

FUNCTION NAME

  reg_mode_found_attempted_plmn_rat

DESCRIPTION

  Function that searches for the specified PLMN and RAT in the attempted
  PLMN list.

RETURN VALUE

  boolean  Flag indicating whether the PLMN-RAT combination was found in the
           attempted PLMN list (TRUE) or not (FALSE).

==============================================================================*/

extern boolean reg_mode_found_attempted_plmn_rat
(
  sys_plmn_id_s_type           plmn,
  sys_radio_access_tech_e_type rat
);
/*==============================================================================

FUNCTION NAME

  reg_mode_update_service_available_rat

==============================================================================*/
extern void reg_mode_update_service_available_rat
(
  sys_radio_access_tech_e_type rat
);


/*==============================================================================

FUNCTION NAME

  reg_mode_update_vplmn_plmn_rat

DESCRIPTION

  Function that updates the vplmn rat to the global and writes the plmn rat to EFS as well

RETURN VALUE

none

==============================================================================*/

int16 reg_mode_update_vplmn_plmn_rat
(
  sys_plmn_id_s_type plmn,
  sys_radio_access_tech_e_type rat
);
/*==============================================================================

FUNCTION NAME

  reg_mode_service_available_rat_get

==============================================================================*/
extern sys_radio_access_tech_e_type reg_mode_service_available_rat_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_filter_available_plmn_list_rat

DESCRIPTION

  Function that removes all PLMN entries of a given 'rat' from available list.

RETURN VALUE

  None.

==============================================================================*/
extern void reg_mode_filter_available_plmn_list_rat
(
  sys_radio_access_tech_e_type         rat
);

sys_detailed_plmn_list_info_s_type reg_mode_managed_roaming_plmn_get
(
  void
);

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE 
void reg_per_nas_stacks_mode_managed_roaming_plmn_reset
(
  sys_modem_as_id_e_type as_id
);
#endif

void reg_mode_managed_roaming_plmn_reset
(
  void
);

extern mmr_reg_req_e_type reg_mode_convert_cm_req_type_to_mm
(
  reg_cm_service_req_e_type type, 
  sys_network_selection_mode_e_type network_selection_mode
);
/*===========================================================================

FUNCTION reg_mode_enhanced_hplmn_srch_init

DESCRIPTION
  This function initializes the enhanced hplmn search tbl and re-populates
  the tbl from the NV.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void reg_mode_enhanced_hplmn_srch_init(void);


#if defined(FEATURE_LTE) && defined(FEATURE_MMSS_3_1)
/*==============================================================================

FUNCTION NAME

  reg_mode_is_multimode_enabled

DESCRIPTION

DEPENDENCIES
  none

RETURN VALUE
  True is rat priority list contains CDMA or HDR

SIDE EFFECTS
  none


==============================================================================*/

extern boolean reg_mode_is_multimode_enabled
(
  const sys_rat_pri_list_info_s_type *rat_pri_list_info
);
#endif


#ifdef FEATURE_FEMTO_CSG
/*==============================================================================

FUNCTION NAME

  reg_mode_update_available_csg_list

DESCRIPTION
  Priortize CSG list 

DEPENDENCIES
  none

RETURN VALUE
  

SIDE EFFECTS
  none


==============================================================================*/
extern void reg_mode_update_available_csg_list
(
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info,
  const sys_detailed_plmn_list_s_type* available_plmn_list_p
);


/*==============================================================================

FUNCTION NAME

  reg_mode_start_automatic_csg_selection

==============================================================================*/

extern void reg_mode_start_automatic_csg_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type          plmn,
  sys_radio_access_tech_e_type  rat,
  sys_csg_id_type                          csg_id,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info 
#endif  
);

/*==============================================================================
FUNCTION NAME:  reg_mode_start_automatic_csg_selection_app
==============================================================================*/
extern void reg_mode_start_automatic_csg_selection_app
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean                             sms_only,
   uint32                              additional_info  
#endif
);

/*==============================================================================

FUNCTION NAME

  reg_mode_start_manual_csg_selection

==============================================================================*/

extern void reg_mode_start_manual_csg_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type          plmn,
  sys_radio_access_tech_e_type  rat,
  sys_csg_id_type                          csg_id,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info   
#endif
);

/*==============================================================================
FUNCTION NAME:  reg_mode_start_manual_csg_selection_app
==============================================================================*/
extern void reg_mode_start_manual_csg_selection_app
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type                   plmn,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean                             sms_only,
   uint32                              additional_info  
#endif
);

/*==============================================================================

FUNCTION NAME

  reg_mode_unexpected_start_manual_csg_selection

==============================================================================*/

extern void reg_mode_unexpected_start_manual_csg_selection
(
  sys_network_selection_mode_e_type network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type                   plmn,
  sys_radio_access_tech_e_type  rat,
  sys_csg_id_type                          csg_id,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info 
#endif 
);

/*==============================================================================
FUNCTION NAME:  reg_mode_unexpected_start_manual_csg_selection_app
==============================================================================*/
extern void reg_mode_unexpected_start_manual_csg_selection_app
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type                   plmn,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type   *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean                             sms_only,
  uint32                               additional_info 
#endif 
);

/*==============================================================================

FUNCTION NAME

  reg_mode_unexpected_start_automatic_csg_selection

==============================================================================*/
extern void reg_mode_unexpected_start_automatic_csg_selection
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_plmn_id_s_type          plmn,
  sys_radio_access_tech_e_type  rat,
  sys_csg_id_type                          csg_id,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean sms_only,
   uint32  additional_info   
#endif
);

/*==============================================================================
FUNCTION NAME:  reg_mode_unexpected_start_automatic_csg_selection_app
==============================================================================*/
extern void reg_mode_unexpected_start_automatic_csg_selection_app
(
  sys_network_selection_mode_e_type    network_selection_mode,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain,
  const sys_rat_pri_list_info_s_type   *rat_pri_list_info
#ifdef FEATURE_LTE
  ,boolean                             sms_only,
  uint32                               additional_info 
#endif 
);

/*==============================================================================

FUNCTION NAME

  reg_mode_req_csg_id_get

==============================================================================*/
extern sys_csg_id_type reg_mode_req_csg_id_get(void);

/*==============================================================================

FUNCTION NAME

  reg_mode_req_csg_id_set

==============================================================================*/
extern void reg_mode_req_csg_id_set(sys_csg_id_type   csg_id);

#endif /*FEATURE_FEMTO_CSG*/
/*==============================================================================

FUNCTION NAME

  reg_mode_unexpected_update_plmn_selection_mode

DESCRIPTION
  Update the plmn selection mode

DEPENDENCIES
  none

RETURN VALUE
  

SIDE EFFECTS
  none


==============================================================================*/
extern void reg_mode_unexpected_update_plmn_selection_mode
(
   reg_mode_automatic_mode_e_type  plmn_selection_mode
);
/*==============================================================================

FUNCTION NAME

  reg_mode_update_plmn_selection_mode

DESCRIPTION
  Update the plmn selection mode

DEPENDENCIES
  none

RETURN VALUE
  

SIDE EFFECTS
  none


==============================================================================*/
extern void reg_mode_update_plmn_selection_mode
(
  reg_mode_automatic_mode_e_type  plmn_selection_mode
);

/*==============================================================================

FUNCTION NAME

  reg_set_cm_stack_id

==============================================================================*/
extern sys_modem_stack_id_e_type reg_set_cm_stack_id(void);

#if defined(FEATURE_LTE) 
/*==============================================================================

FUNCTION NAME

  reg_mode_sms_only_get

==============================================================================*/
boolean reg_mode_sms_only_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_mode_additional_info_get

==============================================================================*/
uint32 reg_mode_additional_info_get
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_mode_reset_addtnl_info

==============================================================================*/
void reg_mode_reset_addtnl_info
(
  sys_addtnl_info_e_type  bit_mask
);


/*==============================================================================

FUNCTION NAME

  reg_mode_lte_cs_domain_param_get

==============================================================================*/
sys_lte_cs_domain_param_s_type reg_mode_lte_cs_domain_param_get
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_mode_lte_cs_domain_param_set

==============================================================================*/
void reg_mode_lte_cs_domain_param_set
(
  sys_lte_cs_domain_param_s_type  lte_cs_domain_param
);

/*==============================================================================

FUNCTION NAME

  reg_mode_start_plmn_selection_to_gw

DESCRIPTION Start PLMN selection to move to UE GSM/UMTS 

DEPENDENCIES
  none

RETURN VALUE
  T

SIDE EFFECTS
  none


==============================================================================*/
void reg_mode_start_plmn_selection_to_gw
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_mode_remove_rat_from_rat_pri_list

DESCRIPTION 
  Removes the requested rat from the rat priority list passed as
  the parameter 

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none

==============================================================================*/
extern void reg_mode_remove_rat_from_rat_pri_list
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info,
  const sys_sys_mode_e_type rat
);

/*==============================================================================

FUNCTION NAME

  reg_mode_update_curr_rat_pri_list

DESCRIPTION 
  Updates the current rat priority list if needed.

DEPENDENCIES
  none

RETURN VALUE
  Returns TRUE if curr rat priority list is updated, FALSE otherwise

SIDE EFFECTS
  none

==============================================================================*/

extern boolean reg_mode_update_curr_rat_pri_list
(
  const sys_plmn_id_s_type *plmn,
  const sys_radio_access_tech_e_type active_rat
);

/*==============================================================================

FUNCTION NAME

  reg_mode_curr_rat_pri_list_set

DESCRIPTION 
  Sets the current rat priority list with the parameter passed.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none

==============================================================================*/

extern void reg_mode_curr_rat_pri_list_set
(
  sys_rat_pri_list_info_s_type *rat_pri_list_info
);

/*==============================================================================

FUNCTION NAME

  reg_mode_curr_rat_pri_list_get

DESCRIPTION 
  Returns the current rat priority list info,

DEPENDENCIES
  none

RETURN VALUE
  Current rat priority list info

SIDE EFFECTS
  none

==============================================================================*/

sys_rat_pri_list_info_s_type* reg_mode_curr_rat_pri_list_get(void);


/*==============================================================================

FUNCTION NAME

  reg_mode_reenable_lte

  
DESCRIPTION 
  If LTE is not currently reported to MM for per plmn blocking
  and if current camped PLMN or its equivalent PLMN is not
  backoff forbidden anymore then
     a) if mm connection status is reporting IDLE then enable LTE
     b) otherwise start Reenable LTE timer if it is not running

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none
==============================================================================*/

extern boolean reg_mode_reenable_lte(void);

/*==============================================================================

FUNCTION NAME

  reg_mode_check_eplmn_backoff_forbidden

 
DESCRIPTION 
  Thsi function checks if PLMN passed as parameter has an equivalent PLMN which is
  backoff forbidden or gprs forbidden PLMN

DEPENDENCIES
  none

RETURN VALUE
  It return TRUE if PLMN has equivalent PLMN which is backoff or gprs forbidden, False otherwise

SIDE EFFECTS
  none

==============================================================================*/


extern boolean reg_mode_check_eplmn_backoff_forbidden
(
  const sys_plmn_id_s_type *plmn, 
  rex_timer_cnt_type *max_timer_count,
  sys_block_plmn_cause_e_type *cause,
  sys_modem_as_id_e_type as_id
);

/*==============================================================================

FUNCTION NAME

  reg_mode_remove_eplmn_rplmn_backoff_forbidden

 
DESCRIPTION 
  Thsi function checks if PLMN passed as parameter has an equivalent RPLMN which is
  backoff forbidden  and removes the eplmn rplmn from backoff list

DEPENDENCIES
  none

RETURN VALUE
none

SIDE EFFECTS
  none

==============================================================================*/

extern void reg_mode_remove_eplmn_rplmn_backoff_forbidden
(
  const sys_plmn_id_s_type *plmn,
  sys_block_plmn_cause_e_type cause
);


/*==============================================================================

FUNCTION NAME

   reg_mode_check_validate_sim_counter
 
DESCRIPTION 
 
   This function returns checks if validate SIM counter value exceeeded max counter.
==============================================================================*/
boolean reg_mode_check_validate_sim_counter(void);


/*==============================================================================

FUNCTION NAME

  reg_mode_adjust_rat_order_plmn_backoff_forbidden

==============================================================================*/
extern void reg_mode_adjust_rat_order_plmn_backoff_forbidden
(
  const sys_plmn_id_s_type *plmn, 
  reg_mode_3gpp_capability_s_type *rplmn_rat_search_order
);

#endif /*FEATURE_LTE*/

extern void reg_mode_set_hplmn_to_be_selected( boolean hplmn_to_be_selected) ;
extern boolean reg_mode_get_hplmn_to_selected
(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
   sys_modem_as_id_e_type as_id
#else
   void
#endif
);

#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  reg_is_usim_compliant
 
DESCRIPTION 
 
   TDSCDMA oems will populate the EFS with EHPLMNs. This function performs
   a check if the card inserted in the UE belongs to the TDSCDMA operators

==============================================================================*/
extern boolean reg_mode_is_usim_compliant(void);
/*==============================================================================

FUNCTION NAME

  reg_mode_set_usim_compliancy
 
DESCRIPTION 
 
   TDSCDMA oems will populate the EFS with EHPLMNs. This function sets
   internal variables to reflect if the usim is TDSCDMA operators

==============================================================================*/
extern void    reg_mode_set_usim_compliancy(void);
/*==============================================================================

FUNCTION NAME

  reg_mode_is_plmn_td_compliant
 
DESCRIPTION 
 
   This function returns a boolean value that indicates if the passed in
   plmn is compliant with TDSCDMA OEMs

==============================================================================*/
extern boolean reg_mode_is_plmn_td_compliant(sys_plmn_id_s_type plmn);

#endif /*  FEATURE_TDSCDMA */

/*==============================================================================

FUNCTION NAME

   reg_mode_revalidate_sim_allowed
 
DESCRIPTION 
 
   This function returns a boolean value that indicates if Validate SIM timer needs to be started.
   This function will check if USIM card is compliant with TDSCDM OEMs and if passed PLMN is 
   compliant with TDSCDMA OEMs.

==============================================================================*/
extern boolean  reg_mode_revalidate_sim_allowed(sys_plmn_id_s_type plmn);

/*==============================================================================

FUNCTION NAME

   reg_mode_increment_validate_sim_counter
 
DESCRIPTION 
 
   This function increments the validate sim counter

==============================================================================*/
void reg_mode_increment_validate_sim_counter(void);

/*==============================================================================

FUNCTION NAME

   reg_mode_reset_validate_sim_counter
 
DESCRIPTION 
 
   This function resets the validate sim counter.

==============================================================================*/
void reg_mode_reset_validate_sim_counter(void);

#ifdef FEATURE_SGLTE

/*==============================================================================

FUNCTION NAME

   reg_mode_get_validate_sim_counter
 
DESCRIPTION 
 
   This function returns the validate sim timer value.
==============================================================================*/
extern uint8 reg_mode_get_validate_sim_counter(void);



/*==============================================================================

FUNCTION NAME

   reg_mode_set_validate_sim_counter
 
DESCRIPTION 
 
   This function sets the validate sim timer value.
==============================================================================*/
extern void reg_mode_set_validate_sim_counter(uint8 value);

#endif

#ifdef FEATURE_SGLTE
extern void reg_mode_perform_merge_operation(void);
extern void reg_mode_perform_split_operation(void);
extern void reg_mode_update_dual_stack_variables(void);
#endif
#ifdef FEATURE_SGLTE
/*==============================================================================

FUNCTION NAME

  reg_mode_copy_available_plmn_lists
 
DESCRIPTION 
 
   This function transfers the available PLMN list between stacks

==============================================================================*/
extern void reg_mode_copy_available_plmn_lists(sys_service_search_e_type service_search);
#endif

/*==============================================================================

FUNCTION NAME

  reg_mode_rat_search_start
 
DESCRIPTION 
 
   This function starts a high priority rat search if certain conditions
   are met

==============================================================================*/
extern boolean reg_mode_rat_search_start (void);
/*==============================================================================

FUNCTION NAME

  reg_mode_rat_search_stop
 
DESCRIPTION 
 
   This function stops a high priority rat search if certain conditions
   are met

==============================================================================*/
extern boolean reg_mode_rat_search_stop (sys_radio_access_tech_e_type  rat);

/*==============================================================================

FUNCTION NAME

  reg_mode_update_lte_lists
 
DESCRIPTION 
 
   This function sUpdates lte avaialble and non avaialble lists
   are met

==============================================================================*/

void reg_mode_update_lte_lists(void);

void reg_mode_reset_retry_counter(void);

/*==============================================================================

FUNCTION NAME

  reg_mode_start_appropriate_irat_timers
 
DESCRIPTION 
 
   This function starts appropriate IRAT timers
   are met

==============================================================================*/

void reg_mode_start_appropriate_irat_timers(void);


/*==============================================================================

FUNCTION NAME

  reg_mode_trm_timeout_value_get

==============================================================================*/

extern uint32 reg_mode_trm_timeout_value_get(sys_radio_access_tech_e_type rat);

#ifdef FEATURE_SVLTE_DUAL_SIM
/*==============================================================================

FUNCTION NAME

  reg_mode_timed_service_req_set

==============================================================================*/
extern void reg_mode_timed_service_req_set(uint32 additional_info);
#endif

#if defined(FEATURE_SVLTE_DUAL_SIM) || defined(FEATURE_SGLTE_DUAL_SIM)
/*==============================================================================

FUNCTION NAME

  reg_mode_service_req_aborted_set

==============================================================================*/
extern void reg_mode_service_req_aborted_set(boolean service_req_aborted);
#endif

#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_mode_lte_cs_domain_param_match

DESCRIPTION

  This function compares lte_cs_domain_param

RETURN VALUE
  Returns TRUE if match is successful.

==============================================================================*/
extern boolean reg_mode_lte_cs_domain_param_match
(
  const sys_lte_cs_domain_param_s_type lte_cs_domain_param_from_cm, 
  const sys_lte_cs_domain_param_s_type lte_cs_domain_param_in_reg
);
#endif


extern boolean reg_mode_is_china_mcc
(
  sys_plmn_id_s_type plmn
);

/*==============================================================================

FUNCTION NAME

  reg_mode_filter_china_hplmn

DESCRIPTION

  Function that updates the available plmn list for china hplmn or rplmn

RETURN VALUE

none

==============================================================================*/

extern void reg_mode_filter_china_hplmn
(
  sys_detailed_plmn_list_s_type *plmn_list
);

extern reg_mode_automatic_mode_e_type reg_mode_get_plmn_selection_mode (void);

extern void reg_mode_start_plmn_selection_roaming_rplmn
(

  sys_network_selection_mode_e_type    network_selection_mode,
  sys_mode_pref_e_type                 mode_pref,
  boolean                              cs_for_request,
  boolean                              ps_for_request,
  sys_srv_domain_e_type                req_service_domain
#ifdef FEATURE_GSM_BAND_PREF
  ,sys_band_mask_type                   band_pref
#endif
#ifdef FEATURE_RAT_PRIORITY_LIST
  ,const sys_rat_pri_list_info_s_type  *rat_pri_list_info
#endif
  ,reg_cm_service_req_e_type           type
#ifdef FEATURE_LTE
  , boolean sms_only,
    uint32  additional_info
#endif
);

#ifdef FEATURE_LTE
/*==============================================================================
  FUNCTION NAME  reg_mode_remove_rat_from_3gpp_rat_list   
 -- Removed desired rat from rat list
==============================================================================*/
extern void reg_mode_remove_rat_from_3gpp_rat_list(reg_mode_3gpp_capability_s_type *reg_3gpp_rat_list,const sys_radio_access_tech_e_type rat_to_remove);
#endif

/*==============================================================================
  FUNCTION NAME  reg_mode_determine_first_plmn
 -- This function determines the first plmn with which PLMN SELECTION needs to be started 
==============================================================================*/
extern void reg_mode_determine_first_plmn
(
  sys_plmn_id_s_type *plmn, 
  reg_mode_3gpp_capability_s_type *rat_list_3gpp,
  const sys_rat_pri_list_info_s_type *rat_pri_list_info
);

#define REG_MODE_MAX_3GPP_CSFB_RAT 3

typedef struct reg_mode_3gpp_csfb_rat_s
{
  sys_radio_access_tech_e_type   rat;
  int16                          priority;
}reg_mode_3gpp_csfb_rat_s_type;

typedef struct reg_mode_3gpp_csfb_rat_list_s
{
   uint8                            num_items;
   reg_mode_3gpp_csfb_rat_s_type    rat_list[REG_MODE_MAX_3GPP_CSFB_RAT];
}reg_mode_3gpp_csfb_rat_list_s_type;

/*==============================================================================
FUNCTION NAME

  reg_mode_process_rrc_sib_info_ind

DESCRIPTION 
  This function processes SIB6 and SIB7 info received from AS.

DEPENDENCIES
  none

RETURN VALUE
  None

SIDE EFFECTS
  none

==============================================================================*/

extern void reg_mode_process_rrc_sib_info_ind
(
  reg_cmd_type *reg_cmd_ptr
);

#ifdef FEATURE_DUAL_DATA
/*===========================================================================

FUNCTION  reg_is_volte_registration_allowed

DESCRIPTION
      DESCRIPTION
           This function checks if LTE PLMN selection	is allowed or not when LTE is in non DDS SUB for VOLTE. REG can perform PLMN selection in LTE if:
               a) UE camps on PLMN present in “non DDS VOLTE allowed PLMN list”
               b) UE camps on PLMN which is equivalent PLMN to a PLMN present in "“non DDS VOLTE allowed PLMNs”.
               c) PM will return whether current PLMN is present in "non DDS VOLTE allowed PLMN list” 

DEPENDENCIES
 PM will return whether current queried PLMN is present in “non DDS VOLTE allowed PLMNs”.


RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
extern boolean reg_is_volte_registration_allowed(sys_plmn_id_s_type plmn_id);
#endif

#endif /* REG_MODE_H */
