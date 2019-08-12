#ifndef RABMTASK_V_H
#define RABMTASK_V_H
/*===========================================================================
                          R A B M task Interface

DESCRIPTION

 This module defines contains the defines, structures and function 
 prototypes required to send a command to the RAB Management sub task.
 
 Note:  RABM runs under SM task context as a sub-task.
   
EXTERNALIZED FUNCTIONS

 rabm_put_cmd_buf() :       Put a RABM message on the SM command queue
 rabm_get_cmd_buf() :       Get a RABM command buffer
   
INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2000 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rabm/vcs/rabmtask.h_v   1.8   23 Apr 2002 16:29:08   kishores  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/rabm/inc/rabmtask_v.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/22/05   kvk     Now defining rabm_timer_status as external.
06/30/05   kvk     Prototype defined for function RABM_CHECK_RAB_REEST_PEND.
03/31/05   kvk     rabm_init_rabid() function defined to clear the RAB state
11/23/04   kvk     MT PDP support added
09/21/04   kvk     CR 49365: TIME_RAB_EST_PENDING changed to 30s to give window for RRC
                    to indicate RL release.
09/02/04   kvk     Added new function call RAB_CHECK_DORMANT to verify whether a paticular
                   RAB is in dormant or not.
06/07/04   kvk     New RABMSM_DEACTIVATE_IMPLICIT added for implicit RAB deactivation
05/25/04   kvk     Added new enum rabm_timer_status_T for maintaining timer status.
                   New function prototypes defined for new timer interface.
06/23/02   ks      Changed RABM_CMD_Q_SIG from 0x2 to 0x80.
02/27/03   mks     Added support for PS data continuity feature - Added new 
                   primitives RABMSM_RAT_CHANGE_IND & RABMSM_RAT_RESUME_IND
                   in SM-RABM interface.
04/22/02   ks      Added support for RAB reestablish/release procedures during
                   PDP context preservation procedures.
01/30/02   ks      Added support for Test Control (TC) entity
08/01/01   ks      Added a function rabm_rab_established(), for SM to check
                   the current state of the RAB of an associated PDP context.
06/04/01   ks      Changed rrc_est_ind to rrc_establish_ind and rrc_rel_ind
                   to rrc_release_ind in rabm_cmd_type. 
02/11/01   KS      Initial Revision.


===========================================================================*/

#include "mmcp_variation.h"
#include "smtask_v.h"
#include "ms_timer_v.h"
#include "rabmtask.h"

#ifdef FEATURE_MBMS
#error code not present
#else
#define MAX_RAB_NUM 16  // MAX_RAB_NUM is set to 16 to have rab up to 15. Index 0~4 on rab_list[] and rabm_timer_status[] will not be used.
#define MIN_RAB_NUM 5   // minimum NSAPI index is 5
#endif

/* -----------------------------------------------------------------------
** Enumerated Type Declarations
** ----------------------------------------------------------------------- */

/* Enumerations for RABM timer status*/
typedef enum
{
   RABM_TIMER_STOPPED,
   RABM_TIMER_PAUSED,
   RABM_TIMER_ACTIVE
} rabm_timer_status_T;

/* -----------------------------------------------------------------------
** Constant/Define Declarations
** ----------------------------------------------------------------------- */
#define RABM_CMD_Q_SIG         0x80
        
#define TIME_RAB_EST_PENDING    30000

#ifdef FEATURE_DUAL_DATA
extern LOCAL byte rabm_timer_status_sim[MAX_AS_IDS][MAX_RAB_NUM];    /* RABM Timer Status List */
#define rabm_timer_status rabm_timer_status_sim[sm_as_id]
#else
extern byte rabm_timer_status[MAX_RAB_NUM];
#endif

/* =======================================================================
**           Function Definitions (Internal Use)
** ======================================================================= */

/* RABM_CMD_Q type */
extern q_type rabm_cmd_q;

/* This function is called by the SM task to retrive as_id */
extern boolean rabm_get_as_id( rabm_cmd_type* cmd_ptr );

/* This function is called by the SM task to process a received RABM cmd */
extern void rabm_process_cmd( rabm_cmd_type* cmd);

/* This functions is called by the SM task to initialize RABM subtask */
extern void rabm_init( void );

/*This function is called by the SM task to initialize the RAB*/
extern void rabm_init_rabid(word rab_id);

/* This functions is called by the SM task to check if a RAB is established */
extern boolean rabm_rab_established( byte rab_id);

/* -----------------------------------------------------------------------
** FUNCTION     : RABM_START_TIMER

** DESCRIPTION  : This function starts an SM timer with given ID
** ----------------------------------------------------------------------- */
extern void rabm_start_timer( word timer_id );

/* -----------------------------------------------------------------------
** FUNCTION     : RABM_STOP_TIMER

** DESCRIPTION  : This function stops the sm timer specified by timer_id
** ----------------------------------------------------------------------- */
extern void rabm_stop_timer( word timer_id ); 

/* -----------------------------------------------------------------------
** FUNCTION     :RABM_TIMER_EXPIRY

** DESCRIPTION  : This function handles RABM timer expiry command
** ----------------------------------------------------------------------- */
extern void rabm_timer_expiry(unsigned long param);

/* -----------------------------------------------------------------------
** FUNCTION     :RABM_PROCESS_TIMER_EXPIRY_CMD

** DESCRIPTION  : This function handles RABM timer expiry command
** ----------------------------------------------------------------------- */
extern void rabm_process_timer_expiry_cmd ( rabm_cmd_type *cmd_ptr );

/* -----------------------------------------------------------------------
** FUNCTION     : sm_proc_timer_expired()

** DESCRIPTION  : This function processes the expiration of a PDP timer.
** ----------------------------------------------------------------------- */
extern void rabm_rab_est_timer_expired(word rab_id);

/* -----------------------------------------------------------------------
**FUNCTION     :RABM_CHECK_RAB_DORMANT

**DESCRIPTION  :This function returns TRUE if RAB state is inactive in RABM.

===========================================================================*/
extern boolean rabm_check_rab_dormant(word rab_id);

/* -----------------------------------------------------------------------
**FUNCTION     :RABM_VERIFY_RAB_REEST_PEND_STATE

**DESCRIPTION  :This function returns TRUE if RAB state is RAB_REESTBLISH_PENDING
                in RABM.

===========================================================================*/
extern boolean rabm_verify_rab_reest_pend_state(word rab_id);

/* -----------------------------------------------------------------------
**FUNCTION     :rabm_is_rab_active

**DESCRIPTION  :This function returns TRUE if any of the RAB is active.

===========================================================================*/

extern boolean rabm_is_rab_active
(
#ifdef FEATURE_DUAL_SIM
      sys_modem_as_id_e_type as_id
#else
      void
#endif

);

/* -----------------------------------------------------------------------
**FUNCTION     :RABM_PROCEDURE_PENDING

**DESCRIPTION  :This function returns TRUE if any of rab state is either active pending, 
                        deactive pending or modify pending

===========================================================================*/
extern boolean rabm_check_procedure_pending(void);

/* -----------------------------------------------------------------------
**FUNCTION     :RABM_PROCEDURE_PENDING_FOR_SCRI

**DESCRIPTION  :This function returns TRUE if any of rab state is either active active pending, 
                        deactive pending or modify pending

===========================================================================*/
extern boolean rabm_check_procedure_pending_for_SCRI(void);

/*===========================================================================
FUNCTION RABM_PACK_UPLINK_DATA_STATUS

DESCRIPTION
  This function packs the Ul Data status IE for 16 NSAPIs(0-15) in 16 bits
===========================================================================*/
extern void rabm_pack_uplink_data_status(byte *ext_buf, sys_modem_as_id_e_type as_id);


/* -----------------------------------------------------------------------
** FUNCTION     : RABM_GET_3GPP_GLOBAL_THROTTLING_ENABLED()
** DESCRIPTION  : This function returns TRUE if  3GPP_GLOBAL_THROTTLING NV is enabled
** ----------------------------------------------------------------------- */
extern  boolean rabm_get_3gpp_global_throtling_enabled(void);
#endif

