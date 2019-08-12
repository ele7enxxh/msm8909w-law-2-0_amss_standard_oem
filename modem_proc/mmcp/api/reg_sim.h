#ifndef REG_SIM_H
#define REG_SIM_H
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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/reg_sim.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

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
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "comdef.h"
#include "sys_plmn_selection.h"
#include "uim.h"
#include "sys_stru.h"


#include "mmgsdilib.h"

#include "mmgsdisessionlib_v.h"


/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/



/* Used to send REFRESH IND to REG from GSDI */
typedef struct sim_refresh_ind_s
{
  IMH_T               message_header;

  mmgsdi_refresh_evt_info_type refresh;

  sys_modem_as_id_e_type                    as_id;


}sim_refresh_ind_s_type;

/* Used for FCN Message to REG from GSDI*/
typedef struct sim_refresh_fcn_ind_s
{
  IMH_T               message_header;

  mmgsdi_file_enum_type  file_id;


  sys_modem_as_id_e_type                    as_id;

}sim_refresh_fcn_ind_s_type;

typedef struct reg_sim_card_unavailable_ind_s
{
  IMH_T               message_header;

  sys_modem_as_id_e_type                    as_id;
}reg_sim_card_unavailable_ind_s_type;

typedef struct reg_sim_card_busy_ind_s
{
  IMH_T               message_header;
  boolean             is_sim_busy;

  sys_modem_as_id_e_type                    as_id;
}reg_sim_card_busy_ind_s_type;

/*
** Maximum length of the PLMN list.
*/
#define REG_SIM_PLMN_LIST_LENGTH 128 

typedef struct reg_sim_plmn_list_s
{
  uint32                   length;
  sys_plmn_id_s_type       plmn[REG_SIM_PLMN_LIST_LENGTH];
} reg_sim_plmn_list_s_type;


/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                             FUNCTION DECLARATIONS

==============================================================================*/







/*==============================================================================

FUNCTION NAME

  reg_sim_plmn_forbidden

DESCRIPTION

  Function that returns a flag indicating if the specified PLMN is included
  in the forbidden PLMN list.

RETURN VALUE

  boolean  Flag indicating whether the specified PLMN is contained in the
           forbidden PLMN list (TRUE) or not (FALSE).

==============================================================================*/

extern boolean reg_sim_plmn_forbidden
(
  sys_plmn_id_s_type plmn
);


boolean reg_sim_per_subs_plmn_forbidden
(
  sys_plmn_id_s_type      plmn
  ,sys_modem_as_id_e_type as_id
);


boolean reg_sim_per_subs_int_voice_forbidden
(
  sys_plmn_id_s_type      plmn,
  sys_modem_as_id_e_type sub_id
);

boolean reg_sim_int_voice_forbidden
(
  sys_plmn_id_s_type      plmn
);



/*==============================================================================

FUNCTION NAME

  reg_sim_hplmn_get

DESCRIPTION

  Interface/Function for MM/RR/RRC to get the most recent HPLMN, which is
  derived either from IMSI or from Acting HPLMN - managed by REG.

ARGUMENTS

  None.

RETURN VALUE

  sys_plmn_id_s_type Returns the current HPLMN (True HPLMN or Acting HPLMN).

==============================================================================*/

extern sys_plmn_id_s_type reg_sim_hplmn_get
(
  void
);


extern sys_plmn_id_s_type reg_sim_per_subs_hplmn_get
(
  sys_modem_as_id_e_type as_id
);



//#ifdef FEATURE_ENHANCED_NW_SELECTION
/*==============================================================================

FUNCTION NAME

  reg_sim_read_ens_flag

DESCRIPTION

  This reads the EF-TST and determine if the ENS features are supported or not.

ARGUMENTS

  None.

RETURN VALUE

  byte  Returns the ENS supported flag
        (each bit of this byte represents an ENS feature)

==============================================================================*/

byte reg_sim_per_subs_read_ens_flag
(
  sys_modem_as_id_e_type as_id
);

extern byte reg_sim_read_ens_flag
(
  void
);



//#endif






/*==============================================================================

FUNCTION NAME

  reg_sim_is_ehplmn

DESCRIPTION

  Function to find if the passed PLMN is an EHPLMN or not. If EHPLMN list is  
  not present, present but empty or present-non empty but all plmn undefined
  then IMSI derived PLMN will be compared.

RETURN VALUE

  TRUE/FALSE

==============================================================================*/
extern boolean reg_sim_is_ehplmn
(
  sys_plmn_id_s_type plmn  
);


extern boolean reg_sim_per_subs_is_ehplmn
(
  sys_plmn_id_s_type plmn,
  sys_modem_as_id_e_type as_id
);

/*==============================================================================

FUNCTION NAME

  reg_sim_per_subs_ehplmn_list_get_ptr

DESCRIPTION

  Function to get the ehplmn_list. 

RETURN VALUE

  reg_sim_plmn_list_s_type

==============================================================================*/
extern void reg_sim_per_subs_ehplmn_list_get_ptr
(
  reg_sim_plmn_list_s_type *plmn_list,
  sys_modem_as_id_e_type sub_id
);

#ifdef FEATURE_TDSCDMA
/*==============================================================================

FUNCTION NAME

  reg_mode_is_plmn_td_compliant
 
DESCRIPTION 
 
   This function returns a boolean value that indicates if the passed in
   plmn is compliant with TDSCDMA OEMs

==============================================================================*/
extern boolean reg_mode_is_plmn_td_compliant(sys_plmn_id_s_type plmn);
#endif 

/*==============================================================================
FUNCTION NAME

  reg_sim_per_subs_chk_low_pri_cfg

DESCRIPTION

  Function returns NAS signalling low priority config status read from NAS configuration file. 

RETURN VALUE

  TRUE if NAS signalling low priority config enabled else FALSE
==============================================================================*/
extern boolean reg_sim_per_subs_chk_low_pri_cfg
(
  sys_modem_as_id_e_type sub_id
);

/*==============================================================================
FUNCTION NAME

  reg_sim_per_subs_chk_eab_cfg

DESCRIPTION

  Function returns Extended access barring config status read from NAS configuration file. 

RETURN VALUE

  TRUE if Extended access barring config enabled else FALSE
==============================================================================*/
extern boolean reg_sim_per_subs_chk_eab_cfg
(
  sys_modem_as_id_e_type sub_id
);

#endif

