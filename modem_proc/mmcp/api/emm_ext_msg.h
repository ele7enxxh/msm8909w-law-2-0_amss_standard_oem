/*===========================================================================

  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/emm_ext_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/2011 sar     moved to API folder and removed the ifdef
===========================================================================*/
#ifndef _EMM_EXT_MSG_H_
#define _EMM_EXT_MSG_H_

#include <customer.h>

#include "comdef.h"
#include "lte_rrc_ext_msg.h"
#include "msgr_umid.h"
#include "msgr_types.h"
#include "msgr_lte.h"
#include "msgr_nas.h"
#include "sys.h"

/*===========================================================================

                        MACROS DEFINITION

===========================================================================*/


/*===========================================================================

                        TYPES DEFINITION

===========================================================================*/
enum
{
  MSGR_DEFINE_UMID(NAS, EMM, IND, PLMN_CHANGE, 0x05, emm_plmn_change_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND,ATTACH_COMPLETE, 0x06, emm_attach_complete_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, T3402_CHANGED, 0x0C, emm_t3402_changed_ind_s_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, DEACT_NON_EMC_BEARER, 0x0D, emm_deact_non_emc_bearer_ind_s_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, RESET_APN_SWITCH, 0x0E, emm_reset_apn_switch_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, TAI_LIST, 0x0F, emm_tai_list_ind_type),
  MSGR_DEFINE_UMID(NAS, EMM, IND, TAU_COMPLETE, 0x0B, emm_tau_complete_ind_type)

};

#define MAX_NO_OF_TAI 16  /* Refer Section: 9.9.3.29; There can be a maximum of 16 TAIs*/
#define MAX_TAI_LST_LENGTH 16

typedef enum
{
  NAS_EMM_TAI_LST0,
  NAS_EMM_TAI_LST1,
  NAS_EMM_TAI_LST2

} lte_nas_tai_lst_type ;

typedef struct
{
  sys_plmn_id_s_type      plmn;
  byte                    tac_lst_length ;
  word                    tac_lst[MAX_NO_OF_TAI]; /*(section 9.9.3.29.4) There can be a maximum of 16 elements, with two octets each*/
}lte_nas_tai_lst0_type; /*lte_nas_tai_...*/

/*Required for tai_type */
typedef struct
{
  sys_plmn_id_s_type     plmn;
  word                   tac; /*(section 9.9.3.29.4) Each TAC takes two octets*/
}lte_nas_tai_lst1_type; /*lte_nas_tai_list_type*/ 

/*Required for tai_type */
typedef struct{
  byte                    tai_lst_length ;
  lte_nas_tai_lst1_type   diff_plmn[LTE_RRC_MAX_NUM_TRACKING_AREAS]; /*(section 9.9.3.33) we can have 40 entries */
}lte_nas_tai_lst2_type; /*lte_nas_tai_plmns*/

/*Required for emm_tai_entry_type */
typedef union{
    lte_nas_tai_lst0_type    tai_lst0;
    lte_nas_tai_lst1_type    tai_lst1;
    lte_nas_tai_lst2_type    tai_lst2;
}tai_type;              

typedef struct  
{
  lte_nas_tai_lst_type tai_lst;
  tai_type             tai;   /* particular structure*/

} emm_tai_lst_elem_type ; 


/*Define TAI entry Type*/
typedef struct{

  byte                  tai_lst_length; /* Number of elements*/
  emm_tai_lst_elem_type tai_lst_elem[MAX_TAI_LST_LENGTH];

}lte_nas_emm_tai_lst_info_type;

typedef struct
{
   msgr_hdr_s     msg_hdr;    
   lte_nas_emm_tai_lst_info_type  tai_lst_info;
}emm_tai_list_ind_type;

/* PLMN change ind */
typedef struct
{
  /* Message router header */
  msgr_hdr_s          msg_hdr;  

  /* New PLMN */
  sys_plmn_id_s_type  plmn;

}emm_plmn_change_ind_type;

/*APN reset ind*/
typedef struct
{
  /* Message router header */
  msgr_hdr_s          msg_hdr;  
}emm_reset_apn_switch_ind_type;



typedef struct
{
  /* Message router header */
  msgr_hdr_s          msg_hdr; 

}emm_attach_complete_ind_type;

typedef struct
{
  /* Message router header */
  msgr_hdr_s                    msg_hdr; 
  sys_plmn_id_s_type      plmn;
  sys_lac_type                  tac;
}emm_tau_complete_ind_type;


/* T3402 change ind */
typedef struct emm_t3402_changed_ind_s
{
  /* Message router header */
  msgr_hdr_s  msg_hdr;

  /* T3402 value */  
  dword  t3402_value;

} emm_t3402_changed_ind_s_type;

/* Deactivate non emergency bearer ind */
typedef struct emm_deact_non_emc_bearer_ind_s
{
  /* Message router header */
  msgr_hdr_s  msg_hdr;

} emm_deact_non_emc_bearer_ind_s_type;


/*===========================================================================

                        EXTERN FUNCTION PROTOTYPES

===========================================================================*/

extern boolean emm_search_tai_list
(
  /* Provided TAI */
  lte_nas_tai_lst1_type *provided_tai,
  /* Stored TAI list */
  lte_nas_emm_tai_lst_info_type *tai_list
);

#endif /* _EMM_EXT_MSG_H_ */



