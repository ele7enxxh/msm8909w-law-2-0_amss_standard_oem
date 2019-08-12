#ifndef mm_mm_v_h
#define mm_mm_v_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mm/vcs/mm_mm.h_v   1.6   21 Oct 2001 11:19:42   skoganti  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mm/inc/mm_mm_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/04/01   jca     Added missing includes to quiet LINT.
08/14/01   ab      Renamed CM to CNM.
08/22/01   jca     Removed changes from version 1.3 and 1.4.
11/14/02   mks     Added Message type for MM_INFORMATION message
06/06/05   abe     Added code for CCBS support
08/18/11   abhi    Added support for CSMO flag in CM_SERVICE_REQ 
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "com_iei_v.h"
#include "mm_mm.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
 * Local  Defines
 * --------------
 */

/* 
 * Message Types for mm to mm peer messages - These are as defined in
 * GSM 04.08
 */


#ifdef FEATURE_CCBS
#define CM_SERVICE_PROMPT           0x25 
#endif

/*
 * General type definitions:
 *
 */

typedef struct classmark_2_tag
{
   byte        iei;
   byte        length;
   byte        data[3];
} ie_classmark_2_T;

typedef struct ie_mobile_identity_tag
{
   byte        iei;
   byte        length;
   byte        data[9];
} ie_mobile_identity_T;


/*
 * Message header for use with all intertask messages
 */

typedef struct mm_msg_head_tag
{
   IMH_T IMH;        /* Intertask Message Header                     */
   byte  skip_pd;    /* skip indicator, protocol discrim.            */
   byte  type;       /* Message type                                 */
} mm_msg_head_T;

#ifdef FEATURE_NAS_REL11
typedef struct
{
  byte value;
}gmm_ie_LV_resp_ptmsi_type_T;

typedef struct
{
  byte iei;
  byte length;
  byte value[6];
}gmm_ie_routing_area_identification2_T;

typedef struct
{
  byte iei;
  byte length;
  byte value[3];
}gmm_ie_LV_resp_ptmsi_sig_T;
#endif


/*
 * Definitions of structures of messages
 */

/*********************************************************************
 * AUTHENTICATION_REJECT MESSAGE
 *
 */

typedef struct authentication_reject_msg_tag
{
   mm_msg_head_T     msg_head;

} authentication_reject_msg_T;

/********************************************************************
 * AUTHENTICATION REQUEST MESSAGE
 */

typedef struct authentication_request_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              cipher_key_seq_no;
   byte              rand[16];

} authentication_request_msg_T;

/********************************************************************
 * AUTHENTICATION RESPONSE MESSAGE
 */

typedef struct authentication_response_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              sres[4];

} authentication_response_msg_T;



/********************************************************************
 * AUTHENTICATION RESPONSE MESSAGE
 */

typedef struct authentication_response_msg_usc_tag
{
   mm_msg_head_T     msg_head;
   byte              res[4];
   byte              res_ext[14]; /* TLV */

} usc_authentication_response_msg_T;


/********************************************************************
 * AUTHENTICATION FAILURE MESSAGE
 */

typedef struct authentication_failure_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              reject_cause;
        byte              auth_failure_parameter[16];
} authentication_failure_msg_T;





/********************************************************************
 * CM_REESTABLISHMENT_REQUEST MESSAGE
 */

typedef struct cm_reestablishment_req_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              cipher_key_seq_no;
   byte              data[
                        ((sizeof( ie_classmark_2_T ) -1)  +       /* no type */
                        (sizeof( ie_mobile_identity_T ) - 1)) +   /* no type */
                        sizeof( ie_lai_T )
                         ];

} cm_reestablishment_req_msg_T;

/********************************************************************
 * CM_SERVICE_ACCEPT MESSAGE
 */

typedef struct cm_service_accept_msg_tag
{
   mm_msg_head_T     msg_head;

} cm_service_accept_msg_T;

/********************************************************************
 * CM_SERVICE_REJECT MESSAGE
 */

typedef struct cm_service_reject_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              reject_cause;

} cm_service_reject_msg_T;

/********************************************************************
 * CM_SERVICE_ABORT MESSAGE
 */

typedef struct cm_service_abort_msg_tag
{
   mm_msg_head_T     msg_head;

} cm_service_abort_msg_T;

#ifdef FEATURE_CCBS 
typedef struct cm_service_prompt_msg_tag
{
    mm_msg_head_T     msg_head;
    byte              pd_sapi;
} cm_service_prompt_msg_T;
#endif 
/********************************************************************
 * ABORT MESSAGE
 */

typedef struct abort_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              reject_cause;

} abort_msg_T;

/********************************************************************
 * CM_SERVICE_REQUEST MESSAGE
 */

typedef struct cm_service_request_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              cipher_key_seq_no_service_type;
   byte              data[
                        (sizeof( ie_classmark_2_T ) - 1) +    /* no type */
                        (sizeof( ie_mobile_identity_T ) - 1) +  /* no type */
                        1    /*Additional Update parameter*/                      
                         ];
} cm_service_request_msg_T;

/********************************************************************
 * IDENTITY_REQUEST MESSAGE
 */

typedef struct identity_request_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              identity_type;

} identity_request_msg_T;

/********************************************************************
 * IDENTITY_RESPONSE MESSAGE
 */

typedef struct identity_response_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              mobile_identity_data[10];
#ifdef FEATURE_NAS_REL11
   gmm_ie_LV_resp_ptmsi_type_T PTMSI_type;
   gmm_ie_routing_area_identification2_T routing_area_identification2;
   gmm_ie_LV_resp_ptmsi_sig_T PTMSI_signature;
#endif
} identity_response_msg_T;

/********************************************************************
 * IMSI_DETACH_INDICATION MESSAGE
 */

typedef struct imsi_detach_indication_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              classmark_1_data;          /* classmark_1 */
   byte              mobile_identity_data[10];

} imsi_detach_indication_msg_T;

/********************************************************************
 * LOCATION_UPDATING_ACCEPT MESSAGE
 */

typedef struct location_updating_accept_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              location_area_identification_data[LAI_SIZE];
   byte              data[
                        sizeof(ie_mobile_identity_T) +
                        sizeof( byte )       /* follow on proceed */
                         ];

} location_updating_accept_msg_T;

/********************************************************************
 * LOCATION_UPDATING_REJECT MESSAGE
 */

typedef struct location_updating_reject_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              reject_cause;

} location_updating_reject_msg_T;

/********************************************************************
 * LOCATION_UPDATING_REQUEST MESSAGE
 */

typedef struct location_updating_request_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              ciphseq_lutype;
   byte              location_area_identification_data[LAI_SIZE];
   byte              classmark_data;            /* classmark_1 */
   byte              data[
                            (sizeof( ie_mobile_identity_T ) - 1) + /* no type */
                            (sizeof( ie_classmark_2_T ) - 1) + /* no type */
                            1 /* Additional update parameters IE */
                         ];
} location_updating_request_msg_T;

/********************************************************************
 * MM_STATUS MESSAGE
 */

typedef struct mm_status_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              reject_cause;

} mm_status_msg_T;

/********************************************************************
 * TMSI_REALLOCATION_COMMAND MESSAGE
 */

typedef struct tmsi_reallocation_command_msg_tag
{
   mm_msg_head_T     msg_head;
   byte              location_area_identification_data[LAI_SIZE];
   byte              mobile_identity_data[9];

} tmsi_reallocation_command_msg_T;

/********************************************************************
 * TMSI_REALLOCATION_COMPLETE MESSAGE
 */

typedef struct tmsi_reallocation_complete_msg_tag
{
   mm_msg_head_T     msg_head;

} tmsi_reallocation_complete_msg_T;


/********************************************************************
 * PAGING RESPONS MESSAGE
 */

typedef struct paging_response_msg_tag
{
   mm_msg_head_T       msg_head;
   byte                cksn_spare;
   classmark_2_ie_T    classmark2;
   byte                data[10];                   /*raw data size of Mobile identity + addition Ie*/
} paging_response_msg_T;

#endif /* mm_mm_h */
