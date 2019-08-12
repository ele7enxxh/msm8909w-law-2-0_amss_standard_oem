#ifndef mn_cnmtask_int_h
#define mn_cnmtask_int_h

/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/mn_cnmtask.h_v   1.12   22 Apr 2002 09:48:00   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/inc/mn_cnmtask_int.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/23/03   sbs     Added cm_lcs_molr_completed to mn_cnm_cmd_type
22-Mar-2001  VT    Introduced extern cm_reserve_connection_id()

4/24/01     CD     Added function prototype for mn_cm_get_cmd_buffer
                   Added IMH_T to the 'cmd' union in the definition of mn_cnm_cmd_type

23-May-2001  VT    Added include for "cc_iei.h" to make this file self-contained

6/8/01      CD     Removed mmi-sms commands and added SM_RL_ABORT_SM_REQ

7/12/01     CD     Removed unnecesary includes cm_mn.h, mn_sms.h, mn_ss.h, cc_iei.h

7/18/01     CD     Added external declaration of is_gsm_mode and is_umts_mode

7/19/01     CD     Added include of queue.h

7/31/01     JC     Add watch dog reporting

8/02/01     CD     Replaced mn.h by mn_exp.h

08/14/01    AB     Renamed connection manager references to CNM from CM.

01/25/02    CD     Removed #include for mn_exp.h and included mn header 
                   files instead since mn_exp.h is now empty

04/19/02    CD     Added MMCC_REEST_REJ primitive per new MM-CNM interface
                   Added MMCNM_EST_REJ primitive per new MM-CNM interface
                   Added MMCNM_REL_CNF primitive per new MM-CNM interface

07/19/02    AB     Added cm_4_x_send_req_T into mn_cnm_cmd_type, to reflect 
                   the new member of this union command buffer type.  This
                   command buffer is generally requested by CM/UI to build
                   and send command to MN.
 
07/29/02    AB     Added the mn_cnm_main_task() prototypes for ANSI C 
                   compliant.
03/21/03    KS     Renamed the mn_cnm_main_task() to mn_cnm_main().


04/15/03   sbs     Added support for Location Services related SS messages

05/24/05    HS     Added support for FEATURE_CCBS
===========================================================================*/


#include "mmcp_variation.h"
#include "timers_v.h"
#include "ms_timer_v.h"


#include "reg_cm_v.h"
#include "mn_cm_exp_v.h"

#include "mn_uasms_v.h"
#include "mn_sms_v.h"
#include "mn_cc_v.h"
#include "mn_ss_v.h"

#include "cc_cc.h"
#include "ms_cc_mm_v.h"


#include "mn_cnmtask_v.h"
#include "cc_state.h"






/* -----------------------------------------------------------------------
** Constant/Define Declarations
** ----------------------------------------------------------------------- */

#define MN_CNM_CMD_Q_SIG_MASK      0x0001
#define MN_CNM_CMD_Q_SIG           0x0001
#define MN_CNM_DOGRPT_TIMER_SIG    0x0002
#define MN_CNM_CMD_INTERNAL_Q_SIG  0x0004



extern q_type mn_cnm_cmd_q;
extern q_type mn_cnm_internal_cmd_q;

/* =======================================================================
**                            Function Definitions
** ======================================================================= */


extern mn_cnm_cmd_type *mn_cnm_get_cmd_buf(void);
extern void mn_cnm_timer_expiry(unsigned long param);
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
extern void mn_cnm_timer_expiry_subs_2(unsigned long param);
#endif

#ifdef FEATURE_DUAL_SIM
extern boolean is_cs_gsm_mode( sys_modem_as_id_e_type as_id );
#endif
extern boolean is_gsm_mode(void);



extern boolean is_lte_mode(void);

#endif  /* ifdef mn_cnmtask_v_h */

