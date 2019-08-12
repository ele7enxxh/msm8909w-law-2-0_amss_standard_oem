#ifndef mm_as_types_h
#define mm_as_types_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/vcs/mm_as_types.h_v   1.0   06 Jun 2002 15:22:00   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/mm_as_types.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/08/05   abe     Modified the enum definition mm_as_status_T for 
                   FEATURE_INTERRAT_PCCO_GTOW
07/20/05   abe     Modified the enum definition mm_as_status_T for 
                   FEATURE_SPEC_COMPLIANT_WTOG_CELL_CHANGE_ORDER
24/05/05   up      Modified sys_PLMN.h and sys_PLMN_selection.h to lower cases.
03/04/04   ks      Added MM_AS_ABORTED in the mm_as_status for background
                   plmn search.
05/28/03   Rao     Added INTERRAT_CC_ORDER to mm_as_status.
08/20/02   cd      Corrected typo in MM_SIM_EFKC_GPRS_SIZE
07/22/02   Rao     Added INTERRAT_HANDOVER reason to mm_as_status
05/02/02   rmc     Initial revision
11/21/11   gps     Removed refrence for header file "gsdi_exp.h"

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "sys_stru.h"
#include "sys_type.h"
#include "sys_cnst.h"


#include "sys.h"
#include "sys_v.h"
#include "sys_plmn.h"
#include "sys_plmn_selection.h"



#include "mmgsdilib.h"
#include "dog.h"



// size of EFKC in bytes
#define MM_SIM_EFKC_SIZE MMGSDI_KC_LEN

// size of EFACC in bytes
#define MM_SIM_EFACC_SIZE MMGSDI_ACC_LEN

// size of BCCH allocation in bytes
#define MM_SIM_EFBCCH_SIZE MMGSDI_BCCH_LEN

// size of the IMSI in bytes
#define MM_SIM_IMSI_SIZE MMGSDI_IMSI_LEN

// size of GPRS EFKC
#define MM_SIM_EFKC_GPRS_SIZE MMGSDI_KCGPRS_LEN




// size of the TMSI in bytes
#define MM_SIM_TMSI_SIZE 4

// size of the PTMSI in bytes
#define MM_SIM_PTMSI_SIZE 4

//Size of the new GSM 128 bit kc 
#define MM_GSM_128BIT_KC_SIZE 16
// size of LAI within EF-LOCI
#define MM_SIM_EFLOCI_LAI_SIZE LAI_SIZE
// size of RAI within EF-LOCIGPRS
#define MM_SIM_EFLOCIGPRS_RAI_SIZE RAI_SIZE


/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

// efkc is a byte array containing ciphering key information from SIM
typedef byte mm_sim_efkc_T[MM_SIM_EFKC_SIZE];

// efkcsn if a byte containing the ciphering key sequence number
typedef byte mm_sim_efkcsn_T;

// efacc is a 2 byte array containing access class information
typedef byte mm_sim_efacc_T[MM_SIM_EFACC_SIZE];

// efimsi
typedef byte mm_sim_imsi_T[MM_SIM_IMSI_SIZE];

// tmsi
typedef byte mm_sim_tmsi_T[MM_SIM_TMSI_SIZE];

// efad is a single byte
typedef byte mm_sim_efad_T;

// ptmsi
typedef byte mm_sim_ptmsi_T[MM_SIM_PTMSI_SIZE];

// GPRS ciphering key
typedef byte mm_sim_efkcgprs_T[MM_SIM_EFKC_GPRS_SIZE];

typedef byte mm_sim_efkcsngprs_T;

// BCCH allocation information
typedef byte mm_sim_efbcch_T[MM_SIM_EFBCCH_SIZE];


// LAI from EF-LOCI
typedef byte mm_sim_efloci_lai_T[MM_SIM_EFLOCI_LAI_SIZE];
// RAI from EF-LOCIGPRS
typedef byte mm_sim_eflocigprs_rai_T[MM_SIM_EFLOCIGPRS_RAI_SIZE];
// Update Status (1 byte)
typedef byte mm_sim_update_status_T;
//GSM 128 bit kc */
typedef byte mm_gsm_kc_128bit_T[MM_GSM_128BIT_KC_SIZE];

// generic status codes for responses to MM requests
typedef enum
{
  MM_AS_SUCCESS,
  MM_AS_CONNECTED,
  MM_AS_INTERRAT_RESELECTION,
  MM_AS_INTERRAT_HANDOVER,
  MM_AS_INTERRAT_CC_ORDER,
  MM_AS_WTOG_INTERRAT_CC_ORDER,
  MM_AS_WTOG_CC_ORDER_FAILED,  
  MM_AS_GTOW_CC_ORDER,
  MM_AS_GTOW_CC_ORDER_FAILED,  
  MM_AS_INTERRAT_REDIRECTION,
  MM_AS_ABORTED,

  MM_AS_GTOW_REDIRECTION,

  MM_AS_INTERRAT_PS_HANDOVER,
  MM_AS_LTOW_SRVCC_CS_PS_HANDOVER,
  MM_AS_LTOW_SRVCC_CS_HANDOVER,
  MM_AS_LTOG_SRVCC_CS_HANDOVER,
  MM_AS_LTOG_CCO,
  MM_AS_SGLTE_INTER_RAT_RESELECTION,
  MM_AS_SGLTE_INTER_RAT_REDIRECTION,
  MM_AS_LTOG_SGLTE_SRVCC_CS_HANDOVER,
  MM_AS_SEARCH_PARTIAL
} mm_as_status_T;

typedef enum
{
  MM_AS_NO_SERVICE        = 0,
  MM_AS_SERVICE_AVAILABLE = 1
} mm_as_service_state_T;

typedef enum
{
  MM_AS_ACCESS_NORMAL_ONLY = 0,
  MM_AS_ACCESS_EMERGENCY_ONLY,
  MM_AS_ACCESS_NO_CALLS,
  MM_AS_ACCESS_ALL_CALLS
} mm_as_cell_access_status_T;

/****************************************
 * The location_area_identification
 * is an element broadcast on the BCCH
 * channel, in the system information
 * messages
 ****************************************/

typedef struct
{
   byte              no_of_entries;
   inter_task_lai_T  location_area_identification[MAX_NO_AVAILABLE_LAIS];
} inter_task_lai_list_T;

#define MAX_NO_FORBIDDEN_LAIS    10


typedef struct lai_reject_list_tag
{
   inter_task_lai_T        lai[MAX_NO_FORBIDDEN_LAIS];
   byte                    write_ptr;
   byte                    length;
}lai_reject_list_T;
//enum for VAMOS support in CLassmark 3 IE
typedef enum
{
  VAMOS_OFF = 0,
  VAMOS_1,
  VAMOS_2
}vamos_support_e;


#define NAS_MAX_EARFCN_COUNT 4           

typedef struct
{
  uint8 count;
  uint32 earfcn_nas[NAS_MAX_EARFCN_COUNT]; 
}earfcn_nas_type;

typedef struct 
{
  sys_radio_access_tech_e_type rat;
  sys_plmn_id_s_type plmn;
  sys_lac_type  lac;
  uint16 cell_id;
}inter_task_cell_id_type;

boolean get_lte_earfcn_list_from_nas(inter_task_cell_id_type *earfcn_nas_param,earfcn_nas_type *earfcn_ptr);


#endif // mm_as_types_h
