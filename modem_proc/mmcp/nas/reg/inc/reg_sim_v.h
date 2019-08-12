#ifndef REG_SIM_V_H
#define REG_SIM_V_H
/*==============================================================================


                    R E G  -  S I M   H E A D E R   F I L E


                                  DESCRIPTION

  This header file contains functions for reading the data from a SIM or USIM
  required by REG.

                             COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/reg/vcs/reg_sim.h_v   1.1   22 May 2002 15:12:26   kabraham  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/inc/reg_sim_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/15/02   kwa     Initial version.
05/22/02   kwa     Removed dependency on sys_detailed_plmn_list_s_type and
                   sys_plmn_list_s_type.
08/26/02   kwa     Added function reg_sim_write_rplmn_rat to write RPLMNACT EF.
09/20/02   kwa     Added functions to support forbidden PLMNs for GPRS service.
10/01/02   kwa     Added functions to manage the state of the SIM.
05/22/03   kwa     Modified reg_sim_find_plmn_in_list to pass the plmn_list_p
                   parameter as a const pointer to eliminate a lint warning.
03/17/04   kwa     Added function reg_sim_read_imsi.
04/15/04   kwa     Added function reg_sim_state_print.
04/26/04   kwa     Doubled the size of the info array within 
                   reg_sim_preferred_plmn_list_s_type to account for the fact
                   that each PLMN can be entered into the array two times - one
                   time for each PLMN-RAT combination.
02/22/05   up      Updated for Cingular ENS feature.
                   Added functions reg_sim_read_ens_flag, reg_sim_hplmn_get
                   reg_sim_force_read_act_hplmn, reg_sim_refresh_complete,
                   reg_sim_force_read_hplmn_search_period,
                   reg_sim_gsdi_generic_cb, reg_sim_refresh_fcn_ind_is_valid,
                   reg_sim_refresh_file_change_notification_cb
03/01/05   up      ENS Fix. Added function reg_sim_process_refresh_ind.
                   Added sim_refresh_ind_s_type for message REFRESH_IND.
01/31/06   up      Increase REG_SIM_PLMN_LIST_LENGTH and 
                   REG_SIM_PREFERRED_PLMN_LIST_LENGTH to 128.
02/20/06   sn      EHPLMN Feature.
04/20/06   sn      Added function reg_sim_ehplmn_list_get 
09/12/11   HC      Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                   as a part of CMI activity 
01/12/12   ay      Changed the definition of REG_MODE_MAX_3GPP_RAT
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/



#include "mmcp_variation.h"
#include "sys_plmn_selection_v.h"
#include "sys_stru_v.h"
#include "rex.h"
#include "reg_sim.h"
#include "reg_mm_v.h"
#include "sys_cnst_v.h"





/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*
** Maximum length of the preferred PLMN list containing all of the PLMN-RAT
** combinations for both the user and operator preferred PLMN lists.
*/
#define REG_SIM_PREFERRED_PLMN_LIST_LENGTH 256
#define REG_SIM_MAX_PREFERRED_PLMN_LIST_LENGTH 1000

#define REG_SIM_ACT_LENGTH        (2)


#define REG_SIM_NUM_REFRESH_FILE 9

#ifdef FEATURE_ENHANCED_NW_SELECTION
#define TST_HPPLMN_IDX 0
#define TST_ACT_HPLMN_IDX 2
#define GSDI_CINGULAR_ACT_HPLMN_LEN 3
#define GSDI_CINGULAR_TST_LEN 8

#define GSDI_SUPPORT_HPLMN_TIMER  0x01
#define GSDI_SUPPORT_MENU_CONTROL 0x02
#define GSDI_SUPPORT_ACTING_HPLMN 0x04
#define GSDI_SUPPORT_3G_FLAG  0x08
#define GSDI_SUPPORT_RAT_BALANCE 0x10
#define GSDI_SUPPORT_BIP_FEATURE 0x20
#define GSDI_SUPPORT_LTE_FLAG 0x40
#define GSDI_SUPPORT_CSG_FLAG 0x80
#endif
/*
** Message ID for REFRESH and FCN from GSDI to REG.
** REFRESH is sent to indicate the changes in SIM.
** FCN is sent to indicate that GSDI cache is refreshed.
*/
#define SIM_REFRESH_IND     0x01
#define SIM_REFRESH_FCN_IND 0x02
#define SIM_REG_SUBS_READY_IND 0x03

#define SIM_REG_CLIENT_ID_IND 0x04
#define REG_SIM_CARD_UNAVAILABLE_IND 0x05
#ifdef MMOC_API_PH_STAT_CNF_WITH_STATUS
#define REG_SIM_BUSY_IND           0x06
#endif
#ifdef FEATURE_BUILTIN_PLMN 
#define REG_SIM_MAX_BUILTIN_OPLMN_ENTRIES 500               
/* structure for built-in PLMN list */
typedef struct
{

  /* Is OPLMN List present */
  uint16 oplmn_list_len;

  /* structure for OPLMN list */
  struct
  {
    /* PLMN. */
    uint8 plmn[3]; 
    
                /* Access Technology identifier. */
    uint16 access_tech;
    
  } oplmn_entry[REG_SIM_MAX_BUILTIN_OPLMN_ENTRIES]; /* Encoding as per 3GPP TS 31.102 Section 4.2.53 */

}reg_sim_builtin_plmn_list_type;
#endif

/**
** Enumeration of Active Subscription ID's 
*/
typedef enum {

  REG_AS_ID_1 = 0,
    /**< Subscription ID 1 */

  REG_AS_ID_2,
    /**< Subscription ID 2 */
#if defined(FEATURE_TRIPLE_SIM)  || defined(FEATURE_SGLTE_DUAL_SIM)
  REG_AS_ID_3
#endif
    /**< Subscription ID 3 */

} reg_as_id_e_type;

#ifdef FEATURE_SGLTE_DUAL_SIM
#define REG_SGLTE_STACK_2 REG_AS_ID_3
#elif defined FEATURE_SGLTE
#define REG_SGLTE_STACK_2 REG_AS_ID_2
#endif 



extern reg_as_id_e_type reg_as_id;

#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE


extern reg_as_id_e_type reg_cs_stack_id;
extern reg_as_id_e_type reg_ps_stack_id;

extern nas_mmgsdi_info_type reg_sim_mmgsdi_info_sim[MAX_NAS_STACKS];
#define reg_sim_mmgsdi_info reg_sim_mmgsdi_info_sim[reg_as_id]

#ifdef FEATURE_BUILTIN_PLMN 
extern reg_sim_builtin_plmn_list_type *reg_sim_ota_plmn_list_sim[MAX_NAS_STACKS];      
#define reg_sim_ota_plmn_list (reg_sim_ota_plmn_list_sim[reg_as_id])
#endif
#else
extern nas_mmgsdi_info_type reg_sim_mmgsdi_info;
#ifdef FEATURE_BUILTIN_PLMN 
extern reg_sim_builtin_plmn_list_type *reg_sim_ota_plmn_list;               
#endif
#endif

#if defined(FEATURE_DUAL_SIM)|| defined(FEATURE_SGLTE)
// This variable represents the SIM subscription.
extern reg_as_id_e_type reg_sub_id_stack[MAX_NAS_STACKS];
#endif
extern reg_as_id_e_type reg_sub_id;
#if defined(FEATURE_LTE)
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
extern reg_sim_plmn_list_s_type reg_sim_gprs_fplmn_list_sim[MAX_NAS_STACKS];
#define reg_sim_gprs_fplmn_list reg_sim_gprs_fplmn_list_sim[reg_sub_id]
#else
extern reg_sim_plmn_list_s_type reg_sim_gprs_fplmn_list;
#endif
#endif

typedef struct reg_sim_list_info_s
{
  sys_plmn_id_s_type                     plmn;
  sys_radio_access_tech_e_type           rat;
  sys_detailed_plmn_list_category_e_type list_category;
} reg_sim_list_info_s_type;

typedef struct reg_sim_preferred_plmn_list_s
{
  uint32                   length;
  reg_sim_list_info_s_type info[2*REG_SIM_PREFERRED_PLMN_LIST_LENGTH];
} reg_sim_preferred_plmn_list_s_type;

typedef struct reg_sim_plmn_w_act_s
{
  sys_plmn_id_s_type plmn;
  byte               act[REG_SIM_ACT_LENGTH];
} reg_sim_plmn_w_act_s_type;

#ifdef FEATURE_LTE
typedef enum
{
  REG_SIM_BACKOFF_FPLMN_CAUSE_MOD_NONE = -1,
  REG_SIM_BACKOFF_FPLMN_CAUSE_MOD_EMM,
  REG_SIM_BACKOFF_FPLMN_CAUSE_MOD_REG, /* REG adds when EPLMN is backoff forbidden */
  REG_SIM_BACKOFF_FPLMN_CAUSE_MOD_CM 
}reg_sim_backoff_fplmn_cause_mod_type; 

typedef struct reg_sim_plmn_timer_info_s
{
  sys_plmn_id_s_type       plmn;
  rex_timer_cnt_type        timer_count;
  sys_block_plmn_cause_e_type  cause;
} reg_sim_plmn_timer_info_s_type;

typedef struct reg_sim_backoff_fplmn_list_info_s
{
  uint32                   length;
  reg_sim_plmn_timer_info_s_type       plmn_timer_list[REG_SIM_PLMN_LIST_LENGTH];
} reg_sim_backoff_fplmn_list_info_s_type;
#endif

#define REG_SIM_IMSI_MAX_LENGTH 8

typedef struct reg_sim_imsi_s
{
  byte digits[REG_SIM_IMSI_MAX_LENGTH];
} reg_sim_imsi_s_type;


#ifdef FEATURE_RAT_PRIORITY_LIST
#define REG_MODE_MAX_3GPP_RAT 4

typedef struct reg_mode_3gpp_capability_s
{
   /* Indicate how many valid items are in this list.
   */
   uint8 num_items;  

   sys_radio_access_tech_e_type   rat_list[REG_MODE_MAX_3GPP_RAT];
   
}reg_mode_3gpp_capability_s_type;
#endif

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  reg_sim_load_card_mode

DESCRIPTION

  Function that reads the card mode (SIM or USIM) and loads it into the
  reg_sim_card_mode static data element.

RETURN VALUE

  boolean  Flag indicating if the card mode was read successfully (TRUE) or
           not (FALSE).

==============================================================================*/

extern boolean reg_sim_load_card_mode
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_sim_reset_card

DESCRIPTION

  Function that resets the card mode and the sim availability flag.

RETURN VALUE

  None

==============================================================================*/

extern void reg_sim_reset_card
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_sim_state_set

DESCRIPTION

  Function that sets the state of the SIM/USIM.  If the SIM/USIM has been
  previously marked as invalid for CS or PS services, this function does 
  not override the previous setting.

RETURN VALUE

  None

==============================================================================*/

extern void reg_sim_state_set
(
  sys_sim_state_e_type sim_state
);


/*==============================================================================

FUNCTION NAME

  reg_sim_reset_sim_state

DESCRIPTION

  Function resets sim state to available on t3245 expiry

RETURN VALUE

  none


==============================================================================*/
extern void reg_sim_reset_sim_state
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_sim_state_get

DESCRIPTION

  Function that returns the state of the SIM/USIM.

RETURN VALUE

  sys_sim_state_e_type  Current state of the SIM/USIM.

==============================================================================*/

extern sys_sim_state_e_type reg_sim_state_get
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_sim_state_print

DESCRIPTION

  Function that prints the state of the SIM/USIM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_sim_state_print
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_sim_allowed_service_domain

DESCRIPTION

  Function that returns the allowed service domain based upon the current
  state of the SIM/USIM.

RETURN VALUE

  sys_srv_domain_e_type Allowed service domain.

==============================================================================*/

extern sys_srv_domain_e_type reg_sim_allowed_service_domain
(
  sys_srv_domain_e_type req_service_domain
);


/*==============================================================================

FUNCTION NAME

  reg_sim_read_imsi

DESCRIPTION

  Function that reads the IMSI stored on the SIM.

RETURN VALUE

  boolean  Flag indicating whether the reg_sim_read_imsi function read the
           IMSI from the SIM successfully (TRUE) or unsuccessfully (FALSE).

==============================================================================*/

extern boolean reg_sim_read_imsi
(
  reg_sim_imsi_s_type* imsi_p
);


/*==============================================================================

FUNCTION NAME

  reg_sim_read_hplmn

DESCRIPTION

  Function that reads the ID of the Home PLMN stored on the SIM.

RETURN VALUE

  sys_plmn_id_s_type  ID of the Home PLMN stored on the SIM.

==============================================================================*/

extern sys_plmn_id_s_type reg_sim_read_hplmn
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_sim_read_hplmn_rat_search_order

DESCRIPTION

  Function that reads the search order to be used when attempting to
  acquire service on the Home PLMN.

RETURN VALUE

  sys_mode_pref_e_type  Order to use the radio access technologies when
                        searching for service on the HPLMN.

==============================================================================*/
#ifndef FEATURE_RAT_PRIORITY_LIST
extern sys_mode_pref_e_type reg_sim_read_hplmn_rat_search_order
(
  sys_mode_pref_e_type req_mode_pref
);
#else 
void reg_sim_read_hplmn_rat_search_order
(
  reg_mode_3gpp_capability_s_type *rat_order
);
#endif /*ifndef FEATURE_RAT_PRIORITY_LIST*/

/*==============================================================================

FUNCTION NAME

  reg_sim_read_hplmn_search_period

DESCRIPTION

  Function that reads the value of the HPLMN Search Period stored on the SIM.

RETURN VALUE

  byte       Value of the HPLMN Search Period in deci-hours.

==============================================================================*/

extern byte reg_sim_read_hplmn_search_period
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_sim_read_rplmn

DESCRIPTION

  Function that reads the appropriate registered PLMN for the specified
  service domain from the SIM or USIM.

RETURN VALUE

  sys_plmn_id_s_type  ID of the RPLMN read from the SIM.  If the RPLMN is not
                      stored on the SIM then the RPLMN return by this function
                      is undefined.

==============================================================================*/

extern sys_plmn_id_s_type reg_sim_read_rplmn
(
  sys_srv_domain_e_type req_service_domain
);


/*==============================================================================

FUNCTION NAME

  reg_sim_read_cs_rplmn

DESCRIPTION

  Function that reads the registered PLMN for circuit switched service from
  the SIM or USIM.

RETURN VALUE

  sys_plmn_id_s_type  ID of the RPLMN read from the SIM.  If the RPLMN is not
                      stored on the SIM then the RPLMN return by this function
                      is undefined.

==============================================================================*/

extern sys_plmn_id_s_type reg_sim_read_cs_rplmn
(
  void
);


extern void reg_sim_imp_variable_prints(void);


/*==============================================================================

FUNCTION NAME

  reg_sim_read_ps_rplmn

DESCRIPTION

  Function that reads the registered PLMN for packet switched service from
  the SIM or USIM.

RETURN VALUE

  sys_plmn_id_s_type  ID of the RPLMN read from the SIM.  If the RPLMN is not
                      stored on the SIM then the RPLMN return by this function
                      is undefined.

==============================================================================*/

extern sys_plmn_id_s_type reg_sim_read_ps_rplmn
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_sim_is_rplmn

==============================================================================*/
extern boolean reg_sim_is_rplmn
(
  sys_plmn_service_state_s_type *current_service_state
);

#ifdef FEATURE_LTE

/*==============================================================================

FUNCTION NAME

  reg_sim_read_eps_rplmn

DESCRIPTION

  Function that reads the registered PLMN for Evolved packet switched service from
  the SIM or USIM.

RETURN VALUE

  sys_plmn_id_s_type  ID of the RPLMN read from the SIM.  If the RPLMN is not
                      stored on the SIM then the RPLMN return by this function
                      is undefined.

==============================================================================*/

extern sys_plmn_id_s_type reg_sim_read_eps_rplmn
(
  void
);

#endif/*FEATURE_LTE*/

/*==============================================================================

FUNCTION NAME

  reg_sim_read_rplmn_rat_search_order

DESCRIPTION

  Function that returns the search order to be used when attempting to
  acquire service on the RPLMN.

RETURN VALUE

  sys_mode_pref_e_type  Order to use the radio access technologies when
                        searching for service on the RPLMN.

==============================================================================*/
#ifndef FEATURE_RAT_PRIORITY_LIST
extern sys_mode_pref_e_type reg_sim_read_rplmn_rat_search_order
(
  sys_mode_pref_e_type req_mode_pref
);
#else
extern void reg_sim_read_rplmn_rat_search_order
(
  reg_mode_3gpp_capability_s_type *req_rat_list
);
#endif /*FEATURE_RAT_PRIORITY_LIST*/

sys_radio_access_tech_e_type reg_sim_read_rplmn_rat
(
  void
);


/*==============================================================================

FUNCTION NAME

  reg_sim_write_rplmn_rat

DESCRIPTION

  Function that writes the radio access technology used to access the RPLMN
  to the SIM.

RETURN VALUE

  None

==============================================================================*/

extern void reg_sim_write_rplmn_rat
(
  sys_radio_access_tech_e_type rat
);

/*==============================================================================

FUNCTION NAME

  reg_sim_is_preferred_rat

==============================================================================*/
boolean reg_sim_is_preferred_rat
(
  reg_sim_plmn_w_act_s_type plmn,
  sys_radio_access_tech_e_type  rat
);


/*==============================================================================

FUNCTION NAME

  reg_sim_is_plmn_not_preferred_plmn

DESCRIPTION

  Function that check if higher priority plmn or not

RETURN VALUE

  None

==============================================================================*/
extern boolean reg_sim_is_plmn_not_preferred_plmn
(
  sys_plmn_id_s_type plmn,
  sys_radio_access_tech_e_type rat
);


/*==============================================================================

FUNCTION NAME

  reg_sim_read_preferred_plmn_list

DESCRIPTION

  Function that reads the preferred PLMN list(s) from the SIM or USIM.

RETURN VALUE

  boolean  Flag indicating whether the reg_sim_read_preferred_plmn_list function
           read the preferred PLMN list from the SIM successfully (TRUE) or
           unsuccessfully (FALSE). 

==============================================================================*/

extern reg_sim_plmn_w_act_s_type* reg_sim_read_preferred_plmn_list
(
  int32 *preferred_plmn_length,
  int32 *operator_pplmn_length,
  int32 *user_pplmn_length,
  boolean read_upplmn
);

/*==============================================================================

FUNCTION NAME

  reg_sim_read_fplmn_list

DESCRIPTION

  Function that reads the forbidden list from the SIM.

RETURN VALUE

  boolean  Flag indicating whether the Forbidden PLMN List was read from the
           SIM successfully (TRUE) or unsuccessfully (FALSE).

==============================================================================*/

extern boolean reg_sim_read_fplmn_list
(
  reg_sim_plmn_list_s_type* fplmn_list_p
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type   as_id
#endif

);


/*==============================================================================

FUNCTION NAME

  reg_sim_remove_plmn_from_fplmn_list

DESCRIPTION

  Function that deletes the RPLMN from the forbidden PLMN list if it is
  contained in the forbidden PLMN list.  If the RPLMN is removed from the 
  forbidden PLMN list, the updated forbidden PLMN list is written back to the
  SIM or USIM.

RETURN VALUE

  None

==============================================================================*/

extern boolean reg_sim_remove_plmn_from_fplmn_list
(
  sys_plmn_id_s_type      rplmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif

);


/*==============================================================================

FUNCTION NAME

  reg_sim_add_plmn_to_fplmn_list

DESCRIPTION

  Function that adds the specified PLMN to the forbidden PLMN list and writes
  the updated forbidden PLMN list back to the SIM or USIM.  If the forbidden
  PLMN list is full, the first PLMN in the list is deleted and the new PLMN is
  placed at the bottom of the list.

RETURN VALUE

  None

==============================================================================*/

extern boolean reg_sim_add_plmn_to_fplmn_list
(
  sys_plmn_id_s_type plmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);




#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_sim_handle_backoff_fplmn_search_timer_expiry

==============================================================================*/
extern void reg_sim_handle_backoff_fplmn_search_timer_expiry
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_sim_remove_plmn_from_backoff_fplmn_list

==============================================================================*/

extern boolean reg_sim_remove_plmn_from_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  sys_block_plmn_cause_e_type cause,
  sys_modem_as_id_e_type as_id
);

/*==============================================================================

FUNCTION NAME

  reg_sim_add_eplmn_to_backoff_fplmn_list

==============================================================================*/

boolean reg_sim_add_eplmn_to_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  rex_timer_cnt_type  timer,
  sys_block_plmn_cause_e_type cause,
  sys_modem_as_id_e_type as_id
);


/*==============================================================================

FUNCTION NAME

  reg_sim_add_plmn_to_backoff_fplmn_list

==============================================================================*/

extern boolean reg_sim_add_plmn_to_backoff_fplmn_list
(
  sys_plmn_id_s_type plmn,
  uint32                    timer,
  sys_block_plmn_cause_e_type   cause,
  sys_modem_as_id_e_type as_id
);

/*==============================================================================

FUNCTION NAME

  reg_sim_clear_backoff_fplmn_list

==============================================================================*/

extern void reg_sim_clear_backoff_fplmn_list
(
  void
);

/*==============================================================================

FUNCTION NAME

  reg_sim_eplmn_list_backoff_forbidden

==============================================================================*/
boolean reg_sim_eplmn_list_backoff_forbidden
(
  const sys_plmn_list_s_type* eplmn_list_p,
  rex_timer_cnt_type *max_timer_count,
  sys_block_plmn_cause_e_type *cause
);

#endif
/*==============================================================================

FUNCTION NAME

  reg_sim_remove_plmn_from_gprs_fplmn_list

DESCRIPTION

  Function that deletes the RPLMN from the GPRS forbidden PLMN list if it is
  contained in the GPRS forbidden PLMN list.

RETURN VALUE

  None

==============================================================================*/

extern boolean reg_sim_remove_plmn_from_gprs_fplmn_list
(
  sys_plmn_id_s_type rplmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);


/*==============================================================================

FUNCTION NAME

  reg_sim_add_plmn_to_gprs_fplmn_list

DESCRIPTION

  Function that adds the specified PLMN to the GPRS forbidden PLMN list.  If
  the forbidden PLMN list is full, the first PLMN in the list is deleted and
  the new PLMN is placed at the bottom of the list.

RETURN VALUE

  None

==============================================================================*/

extern boolean reg_sim_add_plmn_to_gprs_fplmn_list
(
  sys_plmn_id_s_type plmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);

/*==============================================================================
FUNCTION NAME
  reg_sim_set_low_pri_cfg

DESCRIPTION
  Function sets NAS signalling low priority config read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
extern void reg_sim_set_low_pri_cfg
(
  boolean nas_sig_low_priority
);

/*==============================================================================
FUNCTION NAME
  reg_sim_set_min_periodic_search_timer_value

DESCRIPTION
  Function sets Minimum periodic search timer value read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
extern void reg_sim_set_min_periodic_search_timer_value
(
  byte min_periodic_search_timer
);

/*==============================================================================
FUNCTION NAME
  reg_sim_per_subs_get_min_periodic_search_timer_value

DESCRIPTION
  Function gets Minimum periodic search timer value read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
extern byte reg_sim_per_subs_get_min_periodic_search_timer_value
(
  sys_modem_as_id_e_type sub_id
);

/*==============================================================================
FUNCTION NAME
  reg_sim_set_eab_cfg

DESCRIPTION
  Function sets Extended access barring value read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
extern void reg_sim_set_eab_cfg
(
  boolean eab_cfg
);

/*==============================================================================
FUNCTION NAME
  reg_sim_set_t3245_behavior_config

DESCRIPTION
  Function sets T3245 behavior config read from NAS configuration file

RETURN VALUE
  None
==============================================================================*/
extern void reg_sim_set_t3245_behavior_config
(
  boolean t3245_behavior
);

/*==============================================================================

FUNCTION NAME

  reg_sim_check_if_t3245_behavior_enabled.

DESCRIPTION

  Function checks if T3245 behavior is enabled in NAS configuration file

RETURN VALUE


==============================================================================*/
extern boolean reg_sim_check_if_t3245_behavior_enabled
(
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type as_id
#else
  void
#endif
);



/*==============================================================================

FUNCTION NAME

  reg_sim_delete_fplmn_and_gprs_fplmn_list


DESCRIPTION

  Function deletes FPLMN and GPRS FPLMN list.

RETURN VALUE

==============================================================================*/

extern void reg_sim_delete_fplmn_and_gprs_fplmn_list
(
  void
);



/*==============================================================================

FUNCTION NAME

  reg_sim_plmn_gprs_forbidden

DESCRIPTION

  Function that returns a flag indicating if the specified PLMN is included
  in the GPRS forbidden PLMN list.

RETURN VALUE

  boolean  Flag indicating whether the specified PLMN is contained in the
           GPRS forbidden PLMN list (TRUE) or not (FALSE).

==============================================================================*/

extern boolean reg_sim_plmn_gprs_forbidden
(
  sys_plmn_id_s_type plmn
#ifdef FEATURE_DUAL_SIM
  ,sys_modem_as_id_e_type as_id
#endif
);


/*==============================================================================

FUNCTION NAME

  reg_sim_check_mcc_same_country

==============================================================================*/
boolean reg_sim_check_mcc_same_country
(
  sys_plmn_id_s_type plmn
);


/*==============================================================================

FUNCTION NAME

  reg_sim_find_plmn_in_list

DESCRIPTION

  Function that searches for a PLMN in list of PLMNs.

RETURN VALUE

  boolean  Flag indicating whether the specified PLMN was found (TRUE) or
           not found (FALSE).

==============================================================================*/

extern boolean reg_sim_find_plmn_in_list
(
        sys_plmn_id_s_type        plmn,
  const reg_sim_plmn_list_s_type* plmn_list_p,
        uint32*                   position_p
);
#ifdef FEATURE_LTE
/*==============================================================================

FUNCTION NAME

  reg_sim_plmn_backoff_forbidden

==============================================================================*/

extern boolean reg_sim_plmn_backoff_forbidden
(
  sys_plmn_id_s_type        plmn,
  sys_modem_as_id_e_type    as_id
);


/*==============================================================================

FUNCTION NAME

  reg_sim_plmn_rat_forbidden

==============================================================================*/
extern boolean reg_sim_plmn_rat_forbidden
(
  sys_plmn_id_s_type        plmn,
  sys_radio_access_tech_e_type   rat,
  sys_modem_as_id_e_type    as_id
);
#endif


/*==============================================================================

FUNCTION NAME

  reg_sim_refresh_fcn_ind_is_valid

DESCRIPTION

  Function that check for the validation of SIM_REFRESH_FCN_IND.

ARGUMENTS

  msg_p - message structure contains the list of changed files.

RETURN VALUE

  boolean  True if message is valid.

==============================================================================*/
extern boolean reg_sim_refresh_fcn_ind_is_valid
(
  const sim_refresh_fcn_ind_s_type * msg_p
);

#ifdef FEATURE_ENHANCED_NW_SELECTION

/*==============================================================================

FUNCTION NAME

  reg_sim_force_read_act_hplmn

DESCRIPTION

  Function to forcefully read EF ACTING HPLMN from SIM/USIM.

ARGUMENTS

  None.

RETURN VALUE

  boolean  True if read is done successfully.

==============================================================================*/

extern boolean reg_sim_force_read_act_hplmn
(
  void
);
#endif

/*==============================================================================

FUNCTION NAME

  reg_sim_refresh_complete

DESCRIPTION

  This is an intermediate function to call gsdi refresh complete function.
  This function is called when REG finished the REFRESH procedure.

ARGUMENTS

  num_files - number of files in refresh file list.
  file_list - list of files updated/refreshed.
  status - True if REG is able to read all the required files which are updated

RETURN VALUE

  boolean  True if message is valid.

==============================================================================*/

extern void  reg_sim_refresh_complete
(
  boolean status
);

/*==============================================================================

FUNCTION NAME

  reg_sim_gsdi_generic_cb

DESCRIPTION

  Generic Callback function is used while registering for FCN in GSDI.

ARGUMENTS

  gsdi_cnf - GSDI sends the status.

RETURN VALUE

  None.

==============================================================================*/

extern void reg_sim_mmgsdi_generic_cb
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type        *cnf_ptr
);


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
void reg_sim_mmgsdi_evt_cb (const mmgsdi_event_data_type *event);


/*==============================================================================

FUNCTION NAME

  reg_sim_process_refresh_ind

DESCRIPTION

  Function to handle the REFRESH_IND. This initiates the force reading
  of SIM files which are changed.

RETURN VALUE

  None

==============================================================================*/
void reg_sim_process_refresh_ind
(
  const sim_refresh_ind_s_type* msg_p
);

#ifdef FEATURE_EQUIVALENT_HPLMN
/*==============================================================================

FUNCTION NAME

  reg_sim_is_ehplmn_in_eplmn

DESCRIPTION

  This function will search for EHPLMN in EPLMN list and returns TRUE if any one of the 
  EHPLMN present in the EPLMN list

RETURN VALUE

  TRUE/FALSE
==============================================================================*/

extern boolean reg_sim_is_ehplmn_in_eplmn 
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_sim_read_ehplmn_list

DESCRIPTION

  Function to read EHPLMN list from SIM 

RETURN VALUE

  TRUE/FALSE

==============================================================================*/
extern boolean reg_sim_read_ehplmn_list
(
  void
);



/*==============================================================================

FUNCTION NAME

  reg_sim_ehplmn_list_get

DESCRIPTION

  Function to get the ehplmn_list. 

RETURN VALUE

  TRUE/FALSE

==============================================================================*/
extern reg_sim_plmn_list_s_type reg_sim_ehplmn_list_get
(
  void
);
/*==============================================================================

FUNCTION NAME

  reg_sim_ehplmn_list_get_ptr

DESCRIPTION

  Function to get the ehplmn_list. 

RETURN VALUE

  void

==============================================================================*/

extern void reg_sim_ehplmn_list_get_ptr
(
  reg_sim_plmn_list_s_type *plmn
);

/*==============================================================================

FUNCTION NAME

  reg_sim_per_nas_stacks_ehplmn_list_get_ptr

DESCRIPTION

  Function to get the ehplmn_list in a pointer. 

RETURN VALUE

  none

==============================================================================*/

extern void reg_sim_per_nas_stacks_ehplmn_list_get_ptr
(
  reg_sim_plmn_list_s_type *plmn_src,
  sys_modem_as_id_e_type as_id
);

#endif

extern void reg_sim_add_to_low_priority_plmn_list
(
  sys_plmn_id_s_type plmn,  
  sys_radio_access_tech_e_type           rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
);

extern void reg_sim_clear_low_priority_plmn_list
(
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  sys_modem_as_id_e_type as_id
#else
  void
#endif
);

extern reg_sim_preferred_plmn_list_s_type* reg_sim_low_priority_plmn_list_get
(
   void
);

extern boolean reg_sim_plmn_in_low_priority_plmn_list
(
  sys_plmn_id_s_type plmn,  
  sys_radio_access_tech_e_type rat
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
);

/*==============================================================================

FUNCTION NAME

    REG_SIM_MMGSDI_INIT

DESCRIPTION

  Function to get client ID for MMGSDI API calls 

RETURN VALUE
   NONE
==============================================================================*/

extern void reg_sim_mmgsdi_init( void );




/*===========================================================================
FUNCTION NAME 
         REG_SIM_REFERSH_EVENT_CB

DESCRIPTION
  Callback function from MMGSDI generated events.

RETURN VALUE
   NONE

===========================================================================*/
extern void reg_sim_refresh_event_cb
(
  const mmgsdi_event_data_type *event
);

/*===========================================================================
FUNCTION NAME 
         REG_SIM_OPEN_SESSION

DESCRIPTION
  Function to get session ID for MMGSDI API calls.

RETURN VALUE
   NONE

===========================================================================*/

boolean reg_sim_open_session
(
  void
);

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
);
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
extern  boolean reg_sim_disp_only_highest_priority_ehplmn(void);
#endif /*FEATURE_EQUIVALENT_HPLMN*/

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
extern  boolean reg_sim_is_hplmn_to_be_selected(void);

#ifdef FEATURE_LTE
extern reg_sim_backoff_fplmn_list_info_s_type reg_sim_backoff_fplmn_list;
extern rex_crit_sect_type  reg_backoff_fplmn_crit_sect;
#endif

/*===========================================================================
FUNCTION NAME 
         REG_SIM_UPDATE_CS_RPLMN

DESCRIPTION
  Function to update the CS RPLMN after registration is successful
  
RETURN VALUE
  None
===========================================================================*/
extern void reg_sim_update_cs_rplmn
(
  sys_plmn_id_s_type rplmn
#if defined FEATURE_DUAL_SIM || defined FEATURE_SGLTE
  ,sys_modem_as_id_e_type as_id
#endif
);


#ifdef FEATURE_DUAL_SIM
extern sys_plmn_id_s_type reg_sim_per_subs_read_hplmn
(
  sys_modem_as_id_e_type as_id
);
extern boolean reg_sim_session_matching_with_other_sub( mmgsdi_session_type_enum_type session_type);

byte reg_sim_per_nas_stacks_read_ens_flag
(
  sys_modem_as_id_e_type stack_id_nas
);

#endif

#ifdef FEATURE_SGLTE
extern void reg_update_dual_stack_variables(void);
extern void reg_sim_perform_merge_operation(void);
extern void reg_sim_perform_split_operation(void);
#endif

#ifdef FEATURE_FEMTO_CSG
/*==============================================================================
FUNCTION NAME:  REG_SIM_SET_CSG_LIST_READ_COMPLETE

DESCRIPTION
  Function to update the status of REG variable when CSG list read is complete
  
RETURN VALUE
  None
==============================================================================*/
extern void reg_sim_set_csg_list_read_complete(boolean list_read);

/*==============================================================================
FUNCTION NAME:  REG_SIM_GET_CSG_LIST_READ_COMPLETE

DESCRIPTION
  Function to retrieve the status of CSG read completion 
  
RETURN VALUE
  None
==============================================================================*/

extern boolean reg_sim_get_csg_list_read_complete(void);
#endif 

/*==============================================================================

FUNCTION NAME

  reg_sim_get_most_pref_oplmn_plmn

DESCRIPTION

  Function return most preferred plmn from OPLMN list,
  with respect to mcc past as argument

RETURN VALUE

  plmn id

==============================================================================*/

extern sys_plmn_id_s_type reg_sim_get_most_pref_oplmn_plmn(
uint32 mcc
#ifdef FEATURE_DUAL_SIM
,sys_modem_as_id_e_type sub_id
#endif
);

#endif

