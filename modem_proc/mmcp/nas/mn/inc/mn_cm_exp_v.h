#ifndef mn_cm_exp_v_h
#define mn_cm_exp_v_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_cm_exp_h_v   1.17   10 Jul 2002 13:31:12   cdealy  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/inc/mn_cm_exp_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/29/08   fj      Move some macros to cm_gw.h for WM6 build
07/30/08   rm      CMI defeaturisation
08/01/01   CD      Initial version

08/06/01   CD      Removed TP_BCD_NO_LENGTH

08/14/01   AB      Renamed connection manager references to CNM from CM.

08/16/01   AB      Renamed MMI references to CM.

01/25/02   CD      Modified structures per new CM-to-MN interface. Replaced IEI
                   fields by boolean flag the indicates if the IE is present or not

                   Updated Copyright

                   Moved cli_restriction_indo_T, InterrogateSS_Res_T and
                   cm_progress_indicator_T from mn_cm.h

                   Moved contents of mn_exp.h here

02/11/02   AB      Added CM_CLIR_DEFAULT and CM_CAUSE_DEFAULT default value so 
                   that CM can use to set the appropriate default value for 
                   the related information element (ie), that are send to the
                   network.

02/26/02   AB      Added MN_MAXNO_CALLS_IN_MPTY, max no of remote parties 
                     allowed in MPTY.

03/02/02   AB      Increased number of ForwardToNumber to array of  
                     MAXNO_FORWARD_TO_NUMBER, 0x9, in forwardToFeatureList 
                     structure. 

03/12/02   AB      Added cm_cc_invoke_T to contains the directory number 
                     to support CD.

                   Added ie_name_indicator_T, ie_ect_indicator_T, ie_ccbs_feature_T
                     and the ie_multicall_indicator_T to completed the 
                     cm_notifySS_T message structure.

01-Apr-2002  VT    GSM EFR: Added octets 3a, 3b, and 3c of type, 
                     gsm_speech_ver_e_type, in cm_bearer_capability_T, 
                     inside FEATURE_GSM_EFR.
               GSM EFR: In the definition of cm_bearer_capability_T,
                     added comment for octets 3a, 3b, and 3c:  We'll 

                   GSM EFR: Introduced the definition of gsm_speech_ver_e_type
                     inside #ifdef FEATURE_GSM_EFR.
                   GSM EFR: Introduced the definition of 
                     gsm_coding_bearer_cap_octet3x_e_type inside 
                     #ifdef FEATURE_GSM_EFR.
                   Actually, merged all the above changes from rev 1.11.1.1
                     (a branch) of this file.

08-Apr-2002  AB    Delete the NEW_CONNECTION_ID liternal as no longer needed
                     for the deferred notification SS implementation.

06-May-2002  AB    Added SS_VERSION literals.  Featurize the notification
                      interface message for ECT, CD and CNAP SS feature.

07/02/02     CD    Added cause of no CLI structure

07/29/02     AB    Moved the notification description here from 
                   mn_cm.h, so that CM/UI can reference it without
                   having to includes other MN header files.

                   Added CNAP SS code, extracted from UMTS TS29.02 spec.

08/08/02    AB     Set the MAXNO_FORWARD_TO_NUMBER to 9 as specified
                   in UMTS TS24.080 spec.- now that CM/UI can handle  
                   larger command buffer size. 

08/14/02    AB     Reset the MAXNO_FOwARD_TO_NUMBER to 2 because
                   of stability issue with larger command buffer
                   size at the CM/UI.

09/16/02    AB     Removed FEATURE_GSM_EFR, code exclusion by 
                   definition.

11/26/02    AB     Added the multimedia other rate adaption.
 
04/09/03    AB     Added ie_cm_cc_rej_T that use supports silent
                   redial feature.

05/12/03    AB     Added BC speech version pref 4 and 5 to supports
                      GSM AMR FR & HR.

06/02/03    AB     Reduce the MAXNO_FORWARD_TO_NUMBER further to 1 from 2 to 
                      save data memory as UI/CM only use 1 entry.

07/02/02    AB     Added call back servcies of CS handover notification 
                      and CM/UI AOC response.

07/25/03    AB     Updated the InterrogateSS_Res_T fix overlap of ss_status
                      and BSG data.  CR#33317.

02/24/05    AB     Added ATCOP client for HO notification.

03/07/05    HS     Added supports for FEATURE_TTY (CTM bit).

04/19/05    HS     Added support for FEATURE_CCBS

06/12/05    AB     Increase the added location estimate to 91 from 90 per
                       TS23.032, encoding of the polygon shape. 

11/09/05    AB     Added supports for FEATURE_EMERGENCY_CATEGORY

04/10/06    NR     Adding HTORPC metacomments to the remote CM APIs

05/02/06    NR     Correcting numberOfPW_AttemptsViolation SS error code

07/11/06    NR     Adding FEATURE_HTORPC_METACOMMENTS for remote_api working.

07/27/06    NR     Modifying the remote_api comments for removing error fatal

09/14/06    NR     Adding changes for passing raw byte stream to CATAPPS

12/22/06    RD     Defined enums to help removing lint errors
===========================================================================*/
#include "sys_cnst_v.h"
#include "mn_cm_exp.h"
       /* Defintions from NAS required by clients */


/************************************************************************
*                                                                       *
*               Basic Service Codes                                     *
*                                                                       *
*************************************************************************
*/



/* SS Version Indicator Value */
#define   SS_VERSION_PHASE1            0x00
#define   SS_VERSION_PHASE2            0x00


/* Absolete definition */
#define NEW_CONNECTION_ID                   (16) /* TBD: waiting for CM to delete */

/* User to User Data Buffers */
#define MN_MAXNO_USER_USER_DATA             (6) /* No. of User to User Data buffers  = at least 1 for each MO transactions */

enum
{
   NO_CUG_RESTRICTIONS,
   CUG_IC_CALLED_BARRED,
   CUG_OG_CALL_BARRED
};

/*
** enum for forwarding_reason
*/

enum
{
   MS_NOT_REACHABLE,
   MS_BUSY,
   NO_REPLY,
   UNCONDITIONAL
};


enum {
   PERMANENT,
   TEMPORARY_DEFAULT_RESTRICTED,
   TEMPORARY_DEFAULT_ALLOWED
};


/* Tags for facility components */

#define   INVOKE_ID_TAG              0x02
#define   LINKED_ID_TAG              0x80
#define   OP_CODE_TAG                0x02
#define   NULL_TAG                   0x05


/* Ref: 24.008, Sec: 10.5.4.5  Octets 3a, etc., in  Bearer Capability IE */
/* For the definitions of other octets in Bearer Capability IE, see ds_cap.h */

/* Enum for Coding  */
typedef enum gsm_coding_bearer_cap_octet3x_e {
    GSM_OCTET_FOR_EXTN_OF_ITC = 0x00,
    GSM_OCTET_FOR_OTHER_EXTN  = 0x01
} gsm_coding_bearer_cap_octet3x_e_type;

/* Codec Selected Type */
#define CODEC_ID_GSM_FULL_RATE                          (0x00)
#define CODEC_ID_GSM_HALF_RATE                          (0x01)
#define CODEC_ID_GSM_ENHANCED_FULL_RATE                 (0x02)
#define CODEC_ID_GSM_FULL_RATE_AMR                      (0x03)
#define CODEC_ID_GSM_HALF_RATE_AMR                      (0x04)
#define CODEC_ID_UMTS_AMR                               (0x05)
#define CODEC_ID_UMTS_AMR2                              (0x06)
#define CODEC_ID_TDMA_ENHANCED_FULL_RATE                (0x07)
#define CODEC_ID_PDC_ENHANCED_FULL_RATE                 (0x08)
 
/* CM CB function prototypes */
typedef void (*mn_cm_cb_T)(uint8 connection_id, void *data); 

/* DS + ATCOP call back function prototypes */
typedef void (*mn_sync_notify_cb_T)(sync_enum_T direction, uint8 rab_id);


#endif


