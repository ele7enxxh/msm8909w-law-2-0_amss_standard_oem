 #ifndef cc_mm_v_h
#define cc_mm_v_h
/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/ms_cc_mm.h_v   1.5   22 Apr 2002 09:45:14   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/inc/ms_cc_mm_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/14/01   AB      Renamed connection manager references to CNM from CM.

04/19/02   CD      Replaced cause by rel_type in MMCNM_REL_REQ
                   Deleted CNM_service_type from MMCNM_EST_CNF, MMCNM_REL_IND
                   Deleted reestablishment_success flag from mmcc_reest_cnf
                   Deleted MMCNM_ABORT_IND, MMCNM_ERR_IND, MMCNM_EST_IND

                   Added MMCNM_EST_REJ, MMCNM_REL_CNF, MMCNM_REEST_REJ

                   Updated Copyright

09/19/02   CD      Added 'rej_cause' element into MM established reject 
                   message structure, mmcnm_est_rej_T, to include the lower
                   layer specific cause value, which can be passsed to 
                   application layer.
                    
                   Added 'reason' element in mmcnm_sync_ind_T, to determine 
                   handoff direction between WCDM and GSM.

05/11/05   HS      Added support for FEATURE_CCBS

06/15/06   NR      Adding cause type and value fields in mmcc_reest_rej_tag

07/14/06   NR      Adding changes for SMS optimization CR 93099 

03/05/06   NR      Adding changes for WB_AMR

04/09/07   RD      SMS optmization changes are incorporated 

05/21/07   NR      Adding new field to mmcnm_est_req for passing call_type to MM
===========================================================================*/

#include "mmcp_variation.h"
#include "ms_cc_mm.h"
#include "sys_cnst_v.h"

/*
 * These messages all use the message set MS_CC_MM 
 *
 */


/*
 * Definition of message types
 * ---------------------------
 */

/*
 * Messages from CC to  MM
 */

#define MMCNM_DATA_REQ               0x00
#define MMCNM_EST_REQ                0x01
#define MMCC_REEST_REQ               0x02
#define MMCNM_REL_REQ                0x03
#define MMCNM_UNIT_DATA_REQ          0x04
#ifdef FEATURE_CCBS
#define MMCNM_PROMPT_RES             0x05
#endif /* FEATURE_CCBS */

/*
 * messages from MM to CC
 */

#define MMCNM_DATA_IND               0x80
#define MMCNM_EST_REJ                0x81 
#define MMCNM_EST_CNF                0x82
#define MMCNM_REL_IND                0x83
#define MMCNM_REL_CNF                0x84
#define MMCC_REEST_CNF               0x85
#define MMCC_SYNC_IND                0x86
#define MMCNM_UNIT_DATA_IND          0x87
#define MMCC_REEST_REJ               0x88
#ifdef FEATURE_CCBS
#define MMCNM_PROMPT_IND             0x89
#endif /* FEATURE_CCBS */
#define MMCC_SIG_CONN_REL_IND        0x90
#define MMCNM_SRVCC_HANDOVER_COMPLETE_IND   0x91


/**********************************************************************
 **********************************************************************
 *******                                                        *******
 *******           MESSAGES FROM CC TO MM                       *******
 *******                                                        *******
 **********************************************************************
 ********************************************************************** 
 */














/************************************************************************
 ************************************************************************
 *******                                                          *******
 *******            MESSAGES FROM MM TO CC                        *******
 *******                                                          *******
 ************************************************************************
 ************************************************************************
 */












#ifdef FEATURE_CCBS
/***********************************************************************
 *
 * MMCNM_PROMPT_IND
 */

typedef struct mmcnm_prompt_ind_tag {
   IMH_T          header;
   byte           pd_sapi;  /* encoded according to TS24.008 10.5.1.10a */
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type  as_id;
#endif
} mmcnm_prompt_ind_T;
#endif /* FEATURE_CCBS */

/***********************************************************************
 *
 * MMCC_SIG_CONN_REL_IND
 */

typedef struct mmcc_sig_conn_rel_ind_tag {
   IMH_T                   header;
#ifdef FEATURE_DUAL_SIM
   sys_modem_as_id_e_type  as_id;
#endif
} mmcc_sig_conn_rel_ind_T;

uint8      is_cp_data(byte *nas_msg_p);

#endif


