/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_state_machine.c_v   1.17   12 Jul 2002 17:14:14   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_state_machine.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/07/01   CD      Clean-up of include header files 

                   Added MSG_HIGH to report state transitions

08/14/01   AB      Renamed connection manager references to CNM from CM.

9/14/01    CD      Modified call to MSG_HIGH to avoid using %s on target

26-Oct-2001  VT   DTMF: Removed #define for A_START_DTMF_ON/OFF.
                   In CNM_cc_respond_to_event(), removed cases 
                     A_START_DTMF_ON/OFF.
                   In ACTION_ENTRYs and index[], replaced DTMF_ON/OFF_entry
                     with DTMF_T336/T337_TO_entry
                   Updated all the calls to DTMF-related functions as they
                     take connection_id as an additional parameter now.

2-Dec-2001   VT   Bug Fixes: MNCC_DISC_REQ_entry is also handled in the 
                    following CC states: CC_STATE_U0_1, CC_STATE_U1 and CC_STATE_U6

01/02/02   AB     Set the cc_transaction_infomation[connection_id].reserved
                     to TRUE on behalfs of CM when performing unit testing.

01/15/02   CD     Backed changes to the MNCC_DISC_REQ_entry to rev1.5

02/07/02   AB     Added CC_STATE_U0 state for STATUS_ENQUIRY event service request.

8-Mar-2002   VT   Added a state change MSG_HIGH inside CNM_set_state_to_U0().

16-March-2002 AB  Based the 2002-03 TS51.010 V4.7.0, the UE will response
                    with RELEASE_COMPLETE for STATUS ENQUIRY of transaction
                    that is in CC_STATE_U0 state.   Therefore back out changes that
                    were made in 2/07/02 to comply with  the older 
                    GSM 11.11, section 31, SS conformance test 
                    spec.

04/19/02   CD     Added entry for MMCC_EST_REJ_event in the state machine table
                  Added entry for MNCC_DISC_REQ_entry in the state table
                  
                  Updated copyright
                  Replaced ERR by MSG_ERROR

04/30/02   CD     Deleted state table entry for a MNCC_DISC_REQ received in U0

05/15/02   CD     Replaced state table action for MNCC_SETUP_REQ_entry to start
                  timer T303 by 'no action'. 
                  Timer T303 should be started when a MMCNM_EST_REQ is sent, and 
                  is now done inside CNM_process_mo_call_request(),  which can be 
                  called by CC, SMS or SS
                  
6/11/02    CD     In CNM_set_state_to_U0(), changed code to report 
                  the state transition correctly

6/12/02    AB     In CNM_set_state_to_U0(), removed the UNIT test conditional 
                  compiling to set the reserved data field as its already 
                  been provided by cm_reserved_connection_id() function. 

6/13/02     CD    In CNM_cc_respond_to_event() corrected reporting of state transitions.
                  Transition from CC_STATE_U4 to CC_STATE_U10 was being reported as CC_STATE_U10 to CC_STATE_U10

6/24/02     CD    In CNM_cc_respond_to_event() moved logging of state transitions
                  inside else statement to avoid duplicate messages

8/8/02      AB    Added DTMF events in CC_STATE_U3, CC_STATE_U4, CC_STATE_U7, CC_STATE_U8, and CC_STATE_U9, in addition to
                  CC_STATE_U10, so that DTMF can be started right away in these states, 
                  instead of beging queue until CC_STATE_U10 for posible inband response.  

                  These additional states for DTMF events is ultilized in 
                  CNM_mo_dtmf_start_req(), which cross checked state with DTMF 
                  events in CNM_cc_check_respond_to_event().

05/10/03    AB    Added calls statistics event logging.

06/10/03    AB    In CNM_set_state_to_U0(), moved the event log after the 
                  call state had already been transitioned.

05/11/05    HS    Added support for FEATURE_CCBS

02/08/06    NR    Adding CNM_clear_dtmf_queue() in CNM_set_state_to_U0()

02/09/06    AB    Adding ability for user to clear a call while UE in in CC
                  in-call modify request, CC_STATE_U26, state.
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"
#include "naslog_v.h"

#include "cmlib.h"
#include "cnm.h"




/*
 * global data declaration
 */



typedef struct action_state_tag
{
   cc_state_T  cur_state;
   byte        action;
   cc_state_T  nextstate;
   struct action_state_tag *nextentry;
} action_state_T;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
static   cc_state_T        next_state_sim[MAX_AS_IDS][MAXNO_CC_TRANSACTIONS];
#define next_state next_state_sim[mn_dsda_as_id]
#else
static   cc_state_T        next_state[MAXNO_CC_TRANSACTIONS];
#endif

/*
 * Definition of the actions to be performed on receipt of an event
 */

#define A_BAD_EVENT                                0
#define A_NO_ACTION                                1
#define A_STOP_ALL_TIMERS                          2
#define A_START_T303                               3
#define A_STOP_T303_START_T310                     4
#define A_STOP_T303_T310                           5
#define A_READ_DTMF_STOP_T303_STOP_T310            6
#define A_START_T313                               7
#define A_READ_DTMF_STOP_T313                      8
#define A_START_T305                               9
#define A_CLEAR_DTMF_STOP_ALL_TIMERS               10
#define A_CLEAR_DTMF_START_T308                    11
#define A_START_T308                               12
#define A_START_T323                               13
#define A_READ_DTMF_STOP_T323                      14
#define A_READ_DTMF                                15
#define A_START_T324                               16
#define A_READ_DTMF_STOP_T324                      17


#define ACTION_ENTRY(label, cur_state, action, nextstate, nextentry) \
static action_state_T label = { cur_state, action, nextstate, nextentry };

#define ENDLIST (VOID_DATA *)0

ACTION_ENTRY(UNDEFc                 , CC_STATE_U27, A_NO_ACTION           ,CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(UNDEFb                 , CC_STATE_U26, A_NO_ACTION           ,CC_STATE_U26,  &UNDEFc)
ACTION_ENTRY(UNDEFa                 , CC_STATE_U19, A_NO_ACTION           ,CC_STATE_U19,  &UNDEFb)
ACTION_ENTRY(UNDEF9                 , CC_STATE_U12, A_NO_ACTION           ,CC_STATE_U12,  &UNDEFa)
ACTION_ENTRY(UNDEF8                 , CC_STATE_U11, A_NO_ACTION           ,CC_STATE_U11,  &UNDEF9)
ACTION_ENTRY(UNDEF7                 , CC_STATE_U10, A_NO_ACTION           ,CC_STATE_U10,  &UNDEF8)
ACTION_ENTRY(UNDEF6                 , CC_STATE_U9 , A_NO_ACTION           ,CC_STATE_U9 ,  &UNDEF7)
ACTION_ENTRY(UNDEF5                 , CC_STATE_U8 , A_NO_ACTION           ,CC_STATE_U8 ,  &UNDEF6)
ACTION_ENTRY(UNDEF4                 , CC_STATE_U7 , A_NO_ACTION           ,CC_STATE_U7 ,  &UNDEF5)
ACTION_ENTRY(UNDEF3                 , CC_STATE_U6 , A_NO_ACTION           ,CC_STATE_U6 ,  &UNDEF4)
ACTION_ENTRY(UNDEF2                 , CC_STATE_U4 , A_NO_ACTION           ,CC_STATE_U4 ,  &UNDEF3)
ACTION_ENTRY(UNDEF1                 , CC_STATE_U3 , A_NO_ACTION           ,CC_STATE_U3 ,  &UNDEF2)
ACTION_ENTRY(UNDEFINED_MESSAGE_entry, CC_STATE_U1,  A_NO_ACTION           ,CC_STATE_U1,   &UNDEF1)

ACTION_ENTRY(MNCC_MODIFY_RES_entry, CC_STATE_U27, A_READ_DTMF_STOP_T324   ,CC_STATE_U10,  (action_state_T*) ENDLIST)

ACTION_ENTRY(MNCC_SETUP_COMPL_RES_entry, CC_STATE_U10 ,A_NO_ACTION        ,CC_STATE_U10,  (action_state_T*) ENDLIST)

ACTION_ENTRY(CRREJ1              ,  CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CC_RETRIEV_REJ_entry,  CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &CRREJ1)

ACTION_ENTRY(CRACK1              ,  CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CC_RETRIEV_ACK_entry,  CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &CRACK1)

ACTION_ENTRY(CHLDR1              ,  CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CC_HOLD_REJ_entry   ,  CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &CHLDR1)

ACTION_ENTRY(CCHLD1              ,  CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CC_HOLD_ACK_entry   ,  CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &CCHLD1)

ACTION_ENTRY(MNRTR1              ,  CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  (action_state_T*) ENDLIST)
ACTION_ENTRY(MNCC_CALL_RETR_entry,  CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &MNRTR1)

ACTION_ENTRY(MNHLD1              ,  CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  (action_state_T*) ENDLIST)
ACTION_ENTRY(MNCC_CALL_HOLD_entry,  CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &MNHLD1)

ACTION_ENTRY(CC_NOTIFY_entry,       CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  (action_state_T*) ENDLIST)

ACTION_ENTRY(MNCC_NOTIFY_REQ_entry, CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  (action_state_T*) ENDLIST)

ACTION_ENTRY(CCONGc               , CC_STATE_U27 ,A_NO_ACTION             ,CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CCONGb               , CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  &CCONGc)
ACTION_ENTRY(CCONGa               , CC_STATE_U19 ,A_NO_ACTION             ,CC_STATE_U19,  &CCONGb)
ACTION_ENTRY(CCONG9               , CC_STATE_U12 ,A_NO_ACTION             ,CC_STATE_U12,  &CCONGa)
ACTION_ENTRY(CCONG8               , CC_STATE_U11 ,A_NO_ACTION             ,CC_STATE_U11,  &CCONG9)
ACTION_ENTRY(CCONG7               , CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &CCONG8)
ACTION_ENTRY(CCONG6               , CC_STATE_U9  ,A_NO_ACTION             ,CC_STATE_U9 ,  &CCONG7)
ACTION_ENTRY(CCONG5               , CC_STATE_U8  ,A_NO_ACTION             ,CC_STATE_U8,  &CCONG6)
ACTION_ENTRY(CCONG4               , CC_STATE_U7  ,A_NO_ACTION             ,CC_STATE_U7 ,  &CCONG5)
ACTION_ENTRY(CCONG3               , CC_STATE_U6  ,A_NO_ACTION             ,CC_STATE_U6 ,  &CCONG4)
ACTION_ENTRY(CCONG2               , CC_STATE_U4  ,A_NO_ACTION             ,CC_STATE_U4 ,  &CCONG3)
ACTION_ENTRY(CCONG1               , CC_STATE_U3  ,A_NO_ACTION             ,CC_STATE_U3 ,  &CCONG2)
ACTION_ENTRY(CC_CONGESTION_entry  , CC_STATE_U1  ,A_NO_ACTION             ,CC_STATE_U1 ,  &CCONG1)

ACTION_ENTRY(CCUSEc               , CC_STATE_U27 ,A_NO_ACTION             ,CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CCUSEb               , CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  &CCUSEc)
ACTION_ENTRY(CCUSEa               , CC_STATE_U19 ,A_NO_ACTION             ,CC_STATE_U19,  &CCUSEb)
ACTION_ENTRY(CCUSE9               , CC_STATE_U12 ,A_NO_ACTION             ,CC_STATE_U12,  &CCUSEa)
ACTION_ENTRY(CCUSE8               , CC_STATE_U11 ,A_NO_ACTION             ,CC_STATE_U11,  &CCUSE9)
ACTION_ENTRY(CCUSE7               , CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &CCUSE8)
ACTION_ENTRY(CCUSE6               , CC_STATE_U9  ,A_NO_ACTION             ,CC_STATE_U9 ,  &CCUSE7)
ACTION_ENTRY(CCUSE5               , CC_STATE_U8  ,A_NO_ACTION             ,CC_STATE_U8,  &CCUSE6)
ACTION_ENTRY(CCUSE4               , CC_STATE_U7  ,A_NO_ACTION             ,CC_STATE_U7 ,  &CCUSE5)
ACTION_ENTRY(CCUSE3               , CC_STATE_U6  ,A_NO_ACTION             ,CC_STATE_U6 ,  &CCUSE4)
ACTION_ENTRY(CCUSE2               , CC_STATE_U4  ,A_NO_ACTION             ,CC_STATE_U4 ,  &CCUSE3)
ACTION_ENTRY(CCUSE1               , CC_STATE_U3  ,A_NO_ACTION             ,CC_STATE_U3 ,  &CCUSE2)
ACTION_ENTRY(CC_USER_INFO_entry   , CC_STATE_U1  ,A_NO_ACTION             ,CC_STATE_U1 ,  &CCUSE1)

ACTION_ENTRY(MNCCUc               , CC_STATE_U27 ,A_NO_ACTION             ,CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(MNCCUb               , CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  &MNCCUc)
ACTION_ENTRY(MNCCUa               , CC_STATE_U19 ,A_NO_ACTION             ,CC_STATE_U19,  &MNCCUb)
ACTION_ENTRY(MNCCU9               , CC_STATE_U12 ,A_NO_ACTION             ,CC_STATE_U12,  &MNCCUa)
ACTION_ENTRY(MNCCU8               , CC_STATE_U11 ,A_NO_ACTION             ,CC_STATE_U11,  &MNCCU9)
ACTION_ENTRY(MNCCU7               , CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &MNCCU8)
ACTION_ENTRY(MNCCU6               , CC_STATE_U9  ,A_NO_ACTION             ,CC_STATE_U9 ,  &MNCCU7)
ACTION_ENTRY(MNCCU5               , CC_STATE_U8  ,A_NO_ACTION             ,CC_STATE_U8,  &MNCCU6)
ACTION_ENTRY(MNCCU4               , CC_STATE_U7  ,A_NO_ACTION             ,CC_STATE_U7 ,  &MNCCU5)
ACTION_ENTRY(MNCCU3               , CC_STATE_U6  ,A_NO_ACTION             ,CC_STATE_U6 ,  &MNCCU4)
ACTION_ENTRY(MNCCU2               , CC_STATE_U4  ,A_NO_ACTION             ,CC_STATE_U4 ,  &MNCCU3)
ACTION_ENTRY(MNCCU1               , CC_STATE_U3  ,A_NO_ACTION             ,CC_STATE_U3 ,  &MNCCU2)
ACTION_ENTRY(MNCC_USER_USER_entry , CC_STATE_U1  ,A_NO_ACTION             ,CC_STATE_U1 ,  &MNCCU1)

ACTION_ENTRY(FACILc               , CC_STATE_U27 ,A_NO_ACTION             ,CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(FACILb               , CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  &FACILc)
ACTION_ENTRY(FACILa               , CC_STATE_U19 ,A_NO_ACTION             ,CC_STATE_U19,  &FACILb)
ACTION_ENTRY(FACIL9               , CC_STATE_U12 ,A_NO_ACTION             ,CC_STATE_U12,  &FACILa)
ACTION_ENTRY(FACIL8               , CC_STATE_U11 ,A_NO_ACTION             ,CC_STATE_U11,  &FACIL9)
ACTION_ENTRY(FACIL7               , CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &FACIL8)
ACTION_ENTRY(FACIL6               , CC_STATE_U9  ,A_NO_ACTION             ,CC_STATE_U9 ,  &FACIL7)
ACTION_ENTRY(FACIL5               , CC_STATE_U8  ,A_NO_ACTION             ,CC_STATE_U8,  &FACIL6)
ACTION_ENTRY(FACIL4               , CC_STATE_U7  ,A_NO_ACTION             ,CC_STATE_U7 ,  &FACIL5)
ACTION_ENTRY(FACIL3               , CC_STATE_U6  ,A_NO_ACTION             ,CC_STATE_U6 ,  &FACIL4)
ACTION_ENTRY(FACIL2               , CC_STATE_U4  ,A_NO_ACTION             ,CC_STATE_U4 ,  &FACIL3)
ACTION_ENTRY(FACIL1               , CC_STATE_U3  ,A_NO_ACTION             ,CC_STATE_U3 ,  &FACIL2)
ACTION_ENTRY(CC_FACILITY_entry    , CC_STATE_U1  ,A_NO_ACTION             ,CC_STATE_U1 ,  &FACIL1)


ACTION_ENTRY(FACRQb               , CC_STATE_U27 ,A_NO_ACTION             ,CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(FACRQa               , CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  &FACRQb)
ACTION_ENTRY(FACRQ9               , CC_STATE_U12 ,A_NO_ACTION             ,CC_STATE_U12,  &FACRQa)
ACTION_ENTRY(FACRQ8               , CC_STATE_U11 ,A_NO_ACTION             ,CC_STATE_U11,  &FACRQ9)
ACTION_ENTRY(FACRQ7               , CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &FACRQ8)
ACTION_ENTRY(FACRQ6               , CC_STATE_U9  ,A_NO_ACTION             ,CC_STATE_U9 ,  &FACRQ7)
ACTION_ENTRY(FACRQ5               , CC_STATE_U8  ,A_NO_ACTION             ,CC_STATE_U8,  &FACRQ6)
ACTION_ENTRY(FACRQ4               , CC_STATE_U7  ,A_NO_ACTION             ,CC_STATE_U7 ,  &FACRQ5)
ACTION_ENTRY(FACRQ3               , CC_STATE_U6  ,A_NO_ACTION             ,CC_STATE_U6 ,  &FACRQ4)
ACTION_ENTRY(FACRQ2               , CC_STATE_U4  ,A_NO_ACTION             ,CC_STATE_U4 ,  &FACRQ3)
ACTION_ENTRY(FACRQ1               , CC_STATE_U3  ,A_NO_ACTION             ,CC_STATE_U3 ,  &FACRQ2)
ACTION_ENTRY(MNCC_FACIL_REQ_entry , CC_STATE_U1  ,A_NO_ACTION             ,CC_STATE_U1 ,  &FACRQ1)


ACTION_ENTRY(CCSTSe               , CC_STATE_U27 ,A_NO_ACTION             ,CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CCSTSd               , CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  &CCSTSe)
ACTION_ENTRY(CCSTSc               , CC_STATE_U19 ,A_NO_ACTION             ,CC_STATE_U19,  &CCSTSd)
ACTION_ENTRY(CCSTSb               , CC_STATE_U12 ,A_NO_ACTION             ,CC_STATE_U12,  &CCSTSc)
ACTION_ENTRY(CCSTSa               , CC_STATE_U11 ,A_NO_ACTION             ,CC_STATE_U11,  &CCSTSb)
ACTION_ENTRY(CCSTS9               , CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &CCSTSa)
ACTION_ENTRY(CCSTS8               , CC_STATE_U9  ,A_NO_ACTION             ,CC_STATE_U9 ,  &CCSTS9)
ACTION_ENTRY(CCSTS7               , CC_STATE_U8  ,A_NO_ACTION             ,CC_STATE_U8,  &CCSTS8)
ACTION_ENTRY(CCSTS6               , CC_STATE_U7  ,A_NO_ACTION             ,CC_STATE_U7 ,  &CCSTS7)
ACTION_ENTRY(CCSTS5               , CC_STATE_U6  ,A_NO_ACTION             ,CC_STATE_U6 ,  &CCSTS6)
ACTION_ENTRY(CCSTS4               , CC_STATE_U4  ,A_NO_ACTION             ,CC_STATE_U4 ,  &CCSTS5)
ACTION_ENTRY(CCSTS3               , CC_STATE_U3  ,A_NO_ACTION             ,CC_STATE_U3 ,  &CCSTS4)
ACTION_ENTRY(CCSTS2               , CC_STATE_U1  ,A_NO_ACTION             ,CC_STATE_U1 ,  &CCSTS3)
ACTION_ENTRY(CCSTS1               , CC_STATE_U0_1,A_NO_ACTION             ,CC_STATE_U0_1, &CCSTS2)
ACTION_ENTRY(CC_STATUS_entry      , CC_STATE_U0  ,A_NO_ACTION             ,CC_STATE_U0 ,  &CCSTS1)

ACTION_ENTRY(CCSTEe               , CC_STATE_U27 ,A_NO_ACTION             ,CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CCSTEd               , CC_STATE_U26 ,A_NO_ACTION             ,CC_STATE_U26,  &CCSTEe)
ACTION_ENTRY(CCSTEc               , CC_STATE_U19 ,A_NO_ACTION             ,CC_STATE_U19,  &CCSTEd)
ACTION_ENTRY(CCSTEb               , CC_STATE_U12 ,A_NO_ACTION             ,CC_STATE_U12,  &CCSTEc)
ACTION_ENTRY(CCSTEa               , CC_STATE_U11 ,A_NO_ACTION             ,CC_STATE_U11,  &CCSTEb)
ACTION_ENTRY(CCSTE9               , CC_STATE_U10 ,A_NO_ACTION             ,CC_STATE_U10,  &CCSTEa)
ACTION_ENTRY(CCSTE8               , CC_STATE_U9  ,A_NO_ACTION             ,CC_STATE_U9 ,  &CCSTE9)
ACTION_ENTRY(CCSTE7               , CC_STATE_U8  ,A_NO_ACTION             ,CC_STATE_U8 ,  &CCSTE8)
ACTION_ENTRY(CCSTE6               , CC_STATE_U7  ,A_NO_ACTION             ,CC_STATE_U7 ,  &CCSTE7)
ACTION_ENTRY(CCSTE5               , CC_STATE_U6  ,A_NO_ACTION             ,CC_STATE_U6 ,  &CCSTE6)
ACTION_ENTRY(CCSTE4               , CC_STATE_U4  ,A_NO_ACTION             ,CC_STATE_U4 ,  &CCSTE5)
ACTION_ENTRY(CCSTE3               , CC_STATE_U3  ,A_NO_ACTION             ,CC_STATE_U3 ,  &CCSTE4)
ACTION_ENTRY(CC_STATUS_ENQ_entry  , CC_STATE_U1  ,A_NO_ACTION             ,CC_STATE_U1 ,  &CCSTE3)

ACTION_ENTRY(DTMF_T337_U12          ,CC_STATE_U12 ,A_NO_ACTION            ,CC_STATE_U12,  (action_state_T*) ENDLIST)
ACTION_ENTRY(DTMF_T337_U9           ,CC_STATE_U9  ,A_NO_ACTION            ,CC_STATE_U9,   &DTMF_T337_U12)
ACTION_ENTRY(DTMF_T337_U8           ,CC_STATE_U8 ,A_NO_ACTION             ,CC_STATE_U8,   &DTMF_T337_U9)
ACTION_ENTRY(DTMF_T337_U7           ,CC_STATE_U7  ,A_NO_ACTION            ,CC_STATE_U7,   &DTMF_T337_U8)
ACTION_ENTRY(DTMF_T337_U4           ,CC_STATE_U4  ,A_NO_ACTION            ,CC_STATE_U4,   &DTMF_T337_U7)
ACTION_ENTRY(DTMF_T337_U3           ,CC_STATE_U3  ,A_NO_ACTION            ,CC_STATE_U3,   &DTMF_T337_U4)
ACTION_ENTRY(DTMF_T337_TO_entry     ,CC_STATE_U10 ,A_NO_ACTION            ,CC_STATE_U10,  &DTMF_T337_U3)

ACTION_ENTRY(DTMF_T336_U12          ,CC_STATE_U12 ,A_NO_ACTION            ,CC_STATE_U12,  (action_state_T*) ENDLIST)
ACTION_ENTRY(DTMF_T336_U9           ,CC_STATE_U9  ,A_NO_ACTION            ,CC_STATE_U9,   &DTMF_T336_U12)
ACTION_ENTRY(DTMF_T336_U8           ,CC_STATE_U8  ,A_NO_ACTION            ,CC_STATE_U8,   &DTMF_T336_U9)
ACTION_ENTRY(DTMF_T336_U7           ,CC_STATE_U7  ,A_NO_ACTION            ,CC_STATE_U7,   &DTMF_T336_U8)
ACTION_ENTRY(DTMF_T336_U4           ,CC_STATE_U4  ,A_NO_ACTION            ,CC_STATE_U4,   &DTMF_T336_U7)
ACTION_ENTRY(DTMF_T336_U3           ,CC_STATE_U3  ,A_NO_ACTION            ,CC_STATE_U3,   &DTMF_T336_U4)
ACTION_ENTRY(DTMF_T336_TO_entry     ,CC_STATE_U10 ,A_NO_ACTION            ,CC_STATE_U10,  &DTMF_T336_U3)

ACTION_ENTRY(START_DTMF_REJ_U12     ,CC_STATE_U12 ,A_NO_ACTION            ,CC_STATE_U12,  (action_state_T*) ENDLIST)
ACTION_ENTRY(START_DTMF_REJ_U9      ,CC_STATE_U9  ,A_NO_ACTION            ,CC_STATE_U9,   &START_DTMF_REJ_U12)
ACTION_ENTRY(START_DTMF_REJ_U8      ,CC_STATE_U8  ,A_NO_ACTION            ,CC_STATE_U8,   &START_DTMF_REJ_U9)
ACTION_ENTRY(START_DTMF_REJ_U7      ,CC_STATE_U7  ,A_NO_ACTION            ,CC_STATE_U7,   &START_DTMF_REJ_U8)
ACTION_ENTRY(START_DTMF_REJ_U4      ,CC_STATE_U4  ,A_NO_ACTION            ,CC_STATE_U4,   &START_DTMF_REJ_U7)
ACTION_ENTRY(START_DTMF_REJ_U3      ,CC_STATE_U3  ,A_NO_ACTION            ,CC_STATE_U3,   &START_DTMF_REJ_U4)
ACTION_ENTRY(CC_START_DTMF_REJ_entry,CC_STATE_U10 ,A_NO_ACTION            ,CC_STATE_U10,  &START_DTMF_REJ_U3)

ACTION_ENTRY(STOP_DTMF_ACK_U12     ,CC_STATE_U12 ,A_NO_ACTION            ,CC_STATE_U12,  (action_state_T*) ENDLIST)
ACTION_ENTRY(STOP_DTMF_ACK_U9       ,CC_STATE_U9  ,A_NO_ACTION            ,CC_STATE_U9,   &STOP_DTMF_ACK_U12)
ACTION_ENTRY(STOP_DTMF_ACK_U8       ,CC_STATE_U8  ,A_NO_ACTION            ,CC_STATE_U8,   &STOP_DTMF_ACK_U9)
ACTION_ENTRY(STOP_DTMF_ACK_U7       ,CC_STATE_U7  ,A_NO_ACTION            ,CC_STATE_U7,   &STOP_DTMF_ACK_U8)
ACTION_ENTRY(STOP_DTMF_ACK_U4       ,CC_STATE_U4  ,A_NO_ACTION            ,CC_STATE_U4,   &STOP_DTMF_ACK_U7)
ACTION_ENTRY(STOP_DTMF_ACK_U3       ,CC_STATE_U3  ,A_NO_ACTION            ,CC_STATE_U3,   &STOP_DTMF_ACK_U4)
ACTION_ENTRY(CC_STOP_DTMF_ACK_entry ,CC_STATE_U10 ,A_NO_ACTION            ,CC_STATE_U10,  &STOP_DTMF_ACK_U3)

ACTION_ENTRY(START_DTMF_ACK_U12     ,CC_STATE_U12 ,A_NO_ACTION            ,CC_STATE_U12,  (action_state_T*) ENDLIST)
ACTION_ENTRY(START_DTMF_ACK_U9      ,CC_STATE_U9  ,A_NO_ACTION            ,CC_STATE_U9,   &START_DTMF_ACK_U12)
ACTION_ENTRY(START_DTMF_ACK_U8      ,CC_STATE_U8  ,A_NO_ACTION            ,CC_STATE_U8,   &START_DTMF_ACK_U9)
ACTION_ENTRY(START_DTMF_ACK_U7      ,CC_STATE_U7  ,A_NO_ACTION            ,CC_STATE_U7,   &START_DTMF_ACK_U8)
ACTION_ENTRY(START_DTMF_ACK_U4      ,CC_STATE_U4  ,A_NO_ACTION            ,CC_STATE_U4,   &START_DTMF_ACK_U7)
ACTION_ENTRY(START_DTMF_ACK_U3      ,CC_STATE_U3  ,A_NO_ACTION            ,CC_STATE_U3,   &START_DTMF_ACK_U4)
ACTION_ENTRY(CC_START_DTMF_ACK_entry,CC_STATE_U10 ,A_NO_ACTION            ,CC_STATE_U10,  &START_DTMF_ACK_U3)

ACTION_ENTRY(STOP_DTMF_U12          ,CC_STATE_U12 ,A_NO_ACTION            ,CC_STATE_U12,  (action_state_T*) ENDLIST)
ACTION_ENTRY(STOP_DTMF_U9           ,CC_STATE_U9  ,A_NO_ACTION            ,CC_STATE_U9,   &STOP_DTMF_U12)
ACTION_ENTRY(STOP_DTMF_U8           ,CC_STATE_U8  ,A_NO_ACTION             ,CC_STATE_U8,   &STOP_DTMF_U9)
ACTION_ENTRY(STOP_DTMF_U7           ,CC_STATE_U7  ,A_NO_ACTION            ,CC_STATE_U7,   &STOP_DTMF_U8)
ACTION_ENTRY(STOP_DTMF_U4           ,CC_STATE_U4  ,A_NO_ACTION            ,CC_STATE_U4,   &STOP_DTMF_U7)
ACTION_ENTRY(STOP_DTMF_U3           ,CC_STATE_U3  ,A_NO_ACTION            ,CC_STATE_U3,   &STOP_DTMF_U4)
ACTION_ENTRY(MNCC_STOP_DTMF_entry   ,CC_STATE_U10 ,A_NO_ACTION            ,CC_STATE_U10,  &STOP_DTMF_U3)

ACTION_ENTRY(START_DTMF_U12         ,CC_STATE_U12 ,A_NO_ACTION            ,CC_STATE_U12,  (action_state_T*) ENDLIST)
ACTION_ENTRY(START_DTMF_U9          ,CC_STATE_U9  ,A_NO_ACTION            ,CC_STATE_U9,   &START_DTMF_U12)
ACTION_ENTRY(START_DTMF_U8          ,CC_STATE_U8  ,A_NO_ACTION            ,CC_STATE_U8,   &START_DTMF_U9)
ACTION_ENTRY(START_DTMF_U7          ,CC_STATE_U7  ,A_NO_ACTION            ,CC_STATE_U7,   &START_DTMF_U8)
ACTION_ENTRY(START_DTMF_U4          ,CC_STATE_U4  ,A_NO_ACTION            ,CC_STATE_U4,   &START_DTMF_U7)
ACTION_ENTRY(START_DTMF_U3          ,CC_STATE_U3  ,A_NO_ACTION            ,CC_STATE_U3,   &START_DTMF_U4)
ACTION_ENTRY(MNCC_START_DTMF_entry  ,CC_STATE_U10 ,A_NO_ACTION            ,CC_STATE_U10,  &START_DTMF_U3)

ACTION_ENTRY(CC_MODIFY_REJ_entry ,CC_STATE_U26 ,A_READ_DTMF_STOP_T323     ,CC_STATE_U26,  (action_state_T*) ENDLIST)

ACTION_ENTRY(CC_MODIFY_COMP_entry,CC_STATE_U26 ,A_READ_DTMF_STOP_T323     ,CC_STATE_U26,  (action_state_T*) ENDLIST)

ACTION_ENTRY(T324_TO_entry     , CC_STATE_U27 ,A_NO_ACTION               ,CC_STATE_U10,  (action_state_T*) ENDLIST)
ACTION_ENTRY(T323_TO_entry     , CC_STATE_U26 ,A_START_T305               ,CC_STATE_U11,  (action_state_T*) ENDLIST)

ACTION_ENTRY(MNCC_MODIFY_REQentry, CC_STATE_U10 ,A_START_T323             ,CC_STATE_U26,  (action_state_T*) ENDLIST)

ACTION_ENTRY(CC_MODIFY_entry   , CC_STATE_U10 ,A_START_T324               ,CC_STATE_U27,  (action_state_T*) ENDLIST)

ACTION_ENTRY(T313_TO_entry      ,CC_STATE_U8 ,A_START_T305                ,CC_STATE_U11,  (action_state_T*) ENDLIST)

ACTION_ENTRY(T310_TO_entry     , CC_STATE_U3  ,A_START_T305               ,CC_STATE_U11,  (action_state_T*) ENDLIST)

ACTION_ENTRY(T308_TO_entry     , CC_STATE_U19 ,A_NO_ACTION                ,CC_STATE_U19,  (action_state_T*) ENDLIST)

ACTION_ENTRY(T305_TO_entry     , CC_STATE_U11 ,A_START_T308               ,CC_STATE_U19,  (action_state_T*) ENDLIST)

ACTION_ENTRY(T3031             , CC_STATE_U1  ,A_START_T305               ,CC_STATE_U11,  (action_state_T*) ENDLIST)
ACTION_ENTRY(T303_TO_entry     , CC_STATE_U0_1,A_NO_ACTION                ,CC_STATE_U0 ,  &T3031)

#ifdef FEATURE_CCBS
ACTION_ENTRY(RELCf             , CC_STATE_U0_6,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  (action_state_T*) ENDLIST)
ACTION_ENTRY(RELCe             , CC_STATE_U27 ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELCf)
#else
ACTION_ENTRY(RELCe             , CC_STATE_U27 ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  (action_state_T*) ENDLIST)
#endif /* FEATURE_CCBS */
ACTION_ENTRY(RELCd             , CC_STATE_U26 ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELCe)
ACTION_ENTRY(RELCc             , CC_STATE_U19 ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELCd)
ACTION_ENTRY(RELCb             , CC_STATE_U12 ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELCc)
ACTION_ENTRY(RELCa             , CC_STATE_U11 ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELCb)
ACTION_ENTRY(RELC9             , CC_STATE_U10 ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELCa)
ACTION_ENTRY(RELC8             , CC_STATE_U9  ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELC9)
ACTION_ENTRY(RELC7             , CC_STATE_U8  ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELC8)
ACTION_ENTRY(RELC6             , CC_STATE_U7  ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELC7)
ACTION_ENTRY(RELC5             , CC_STATE_U6  ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELC6)
ACTION_ENTRY(RELC4             , CC_STATE_U4  ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELC5)
ACTION_ENTRY(RELC3             , CC_STATE_U3  ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELC4)
ACTION_ENTRY(RELC2             , CC_STATE_U1  ,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELC3)
ACTION_ENTRY(CC_REL_COMP_entry , CC_STATE_U0_1,A_CLEAR_DTMF_STOP_ALL_TIMERS          ,CC_STATE_U0 ,  &RELC2)

ACTION_ENTRY(MNCC_REL_REQ_entry , CC_STATE_U12 ,A_START_T308              ,CC_STATE_U19,  (action_state_T*) ENDLIST)

ACTION_ENTRY(CCDISb           , CC_STATE_U27 ,A_STOP_ALL_TIMERS,CC_STATE_U12,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CCDISa           , CC_STATE_U26 ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDISb)
ACTION_ENTRY(CCDIS9           , CC_STATE_U11 ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDISa)
ACTION_ENTRY(CCDIS8           , CC_STATE_U10 ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS9)
ACTION_ENTRY(CCDIS7           , CC_STATE_U9  ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS8)
ACTION_ENTRY(CCDIS6           ,CC_STATE_U8 ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS7)
ACTION_ENTRY(CCDIS5           , CC_STATE_U7  ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS6)
ACTION_ENTRY(CCDIS4           , CC_STATE_U6  ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS5)
ACTION_ENTRY(CCDIS3           , CC_STATE_U4  ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS4)
ACTION_ENTRY(CCDIS2           , CC_STATE_U3  ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS3)
ACTION_ENTRY(CCDIS1           , CC_STATE_U1  ,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS2)
ACTION_ENTRY(CC_DISCONN_entry , CC_STATE_U0_1,A_STOP_ALL_TIMERS,CC_STATE_U12,  &CCDIS1)

ACTION_ENTRY(RELSd            , CC_STATE_U27 ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  (action_state_T*) ENDLIST)
ACTION_ENTRY(RELSc            , CC_STATE_U26 ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELSd)
ACTION_ENTRY(RELSb            , CC_STATE_U19 ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELSc)
ACTION_ENTRY(RELSa            , CC_STATE_U12 ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELSb)
ACTION_ENTRY(RELS9            , CC_STATE_U11 ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELSa)
ACTION_ENTRY(RELS8            , CC_STATE_U10 ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS9)
ACTION_ENTRY(RELS7            , CC_STATE_U9  ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS8)
ACTION_ENTRY(RELS6            ,CC_STATE_U8 ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS7)
ACTION_ENTRY(RELS5            , CC_STATE_U7  ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS6)
ACTION_ENTRY(RELS4            , CC_STATE_U6  ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS5)
ACTION_ENTRY(RELS3            , CC_STATE_U4  ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS4)
ACTION_ENTRY(RELS2            , CC_STATE_U3  ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS3)
ACTION_ENTRY(RELS1            , CC_STATE_U1  ,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS2)
ACTION_ENTRY(CC_RELEASE_entry , CC_STATE_U0_1,A_CLEAR_DTMF_STOP_ALL_TIMERS,CC_STATE_U0 ,  &RELS1)

ACTION_ENTRY(DISC8              , CC_STATE_U27 , A_START_T305     ,        CC_STATE_U11,  (action_state_T*) ENDLIST)
ACTION_ENTRY(DISC7              , CC_STATE_U26 , A_START_T305     ,        CC_STATE_U11,  &DISC8)
ACTION_ENTRY(DISC6              , CC_STATE_U12 , A_START_T308     ,        CC_STATE_U19,  &DISC7)
ACTION_ENTRY(DISC5              , CC_STATE_U10 , A_START_T305     ,        CC_STATE_U11,  &DISC6)
ACTION_ENTRY(DISC4              , CC_STATE_U9  , A_START_T305     ,        CC_STATE_U11,  &DISC5)
ACTION_ENTRY(DISC3              ,CC_STATE_U8  , A_START_T305     ,        CC_STATE_U11,  &DISC4)
ACTION_ENTRY(DISC2              , CC_STATE_U7  , A_START_T305     ,        CC_STATE_U11,  &DISC3)
ACTION_ENTRY(DISC2A             , CC_STATE_U6  , A_CLEAR_DTMF_STOP_ALL_TIMERS,        CC_STATE_U0 ,  &DISC2)  /* Send a RELEASE_COMPLETE msg to the peer CC entiry */
ACTION_ENTRY(DISC1              , CC_STATE_U4  , A_START_T305     ,        CC_STATE_U11,  &DISC2A)
ACTION_ENTRY(DISC1A             , CC_STATE_U3  , A_START_T305     ,        CC_STATE_U11,  &DISC1)
ACTION_ENTRY(DISC1B             , CC_STATE_U1  , A_START_T305     ,        CC_STATE_U11,  &DISC1A) /* Send a DISCONNECT msg to the peer CC entity */
ACTION_ENTRY(MNCC_DISC_REQ_entry, CC_STATE_U0_1, A_CLEAR_DTMF_STOP_ALL_TIMERS,        CC_STATE_U0 ,  &DISC1B) /* Also Send a MMCNM_REL_REQ if last connection */

ACTION_ENTRY(CC_CONNECT_ACK_entry,CC_STATE_U8 , A_READ_DTMF_STOP_T313,    CC_STATE_U10,  (action_state_T*) ENDLIST)

ACTION_ENTRY(SETR1               ,CC_STATE_U9  , A_START_T313  ,CC_STATE_U8 ,  (action_state_T*) ENDLIST)
ACTION_ENTRY(MNCC_SETUP_RSP_entry,CC_STATE_U7  , A_START_T313  ,CC_STATE_U8 ,  &SETR1)

ACTION_ENTRY(MNCC_ALERT_REQ_entry,CC_STATE_U9  , A_NO_ACTION      ,        CC_STATE_U7 ,  (action_state_T*) ENDLIST)

ACTION_ENTRY(REJR1              , CC_STATE_U7  , A_CLEAR_DTMF_STOP_ALL_TIMERS,        CC_STATE_U0 ,  (action_state_T*) ENDLIST)
ACTION_ENTRY(MMCC_REJ_REQ_entry , CC_STATE_U6  , A_CLEAR_DTMF_STOP_ALL_TIMERS,        CC_STATE_U0 ,  &REJR1)

ACTION_ENTRY(MMCC_CALL_CNF_entry,CC_STATE_U6   , A_NO_ACTION      ,        CC_STATE_U9 ,  (action_state_T*) ENDLIST)

ACTION_ENTRY(MMCC_EST_IND_entry , CC_STATE_U0  , A_NO_ACTION      ,        CC_STATE_U6 ,  (action_state_T*) ENDLIST)

ACTION_ENTRY(PROGb              , CC_STATE_U27 , A_STOP_ALL_TIMERS,        CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(PROGa              , CC_STATE_U26 , A_STOP_ALL_TIMERS,        CC_STATE_U26,  &PROGb)
ACTION_ENTRY(PROG9              , CC_STATE_U19 , A_STOP_ALL_TIMERS,        CC_STATE_U19,  &PROGa)
ACTION_ENTRY(PROG8              , CC_STATE_U12 , A_STOP_ALL_TIMERS,        CC_STATE_U12,  &PROG9)
ACTION_ENTRY(PROG7              , CC_STATE_U11 , A_STOP_ALL_TIMERS,        CC_STATE_U11,  &PROG8)
ACTION_ENTRY(PROG6              , CC_STATE_U9  , A_STOP_ALL_TIMERS,        CC_STATE_U9 ,  &PROG7)
ACTION_ENTRY(PROG5              , CC_STATE_U8  , A_STOP_ALL_TIMERS,        CC_STATE_U8 ,  &PROG6)
ACTION_ENTRY(PROG4              , CC_STATE_U7  , A_STOP_ALL_TIMERS,        CC_STATE_U7 ,  &PROG5)
ACTION_ENTRY(PROG3              , CC_STATE_U6  , A_STOP_ALL_TIMERS,        CC_STATE_U6 ,  &PROG4)
ACTION_ENTRY(PROG2              , CC_STATE_U4  , A_STOP_ALL_TIMERS,        CC_STATE_U4 ,  &PROG3)
ACTION_ENTRY(PROG1              , CC_STATE_U3  , A_STOP_ALL_TIMERS,        CC_STATE_U3 ,  &PROG2)
ACTION_ENTRY(CC_PROGRESS_entry  , CC_STATE_U1  , A_STOP_ALL_TIMERS,        CC_STATE_U1 ,  &PROG1)

ACTION_ENTRY(CONN2           ,CC_STATE_U4,A_READ_DTMF_STOP_T303_STOP_T310, CC_STATE_U10,  (action_state_T*) ENDLIST)
ACTION_ENTRY(CONN1           ,CC_STATE_U3,A_READ_DTMF_STOP_T303_STOP_T310, CC_STATE_U10,  &CONN2)
ACTION_ENTRY(CC_CONNECT_entry,CC_STATE_U1,A_READ_DTMF_STOP_T303_STOP_T310, CC_STATE_U10,  &CONN1)

ACTION_ENTRY(SYNC9              , CC_STATE_U27 , A_NO_ACTION     ,         CC_STATE_U27,  (action_state_T*) ENDLIST)
ACTION_ENTRY(SYNC8              , CC_STATE_U26 , A_NO_ACTION     ,         CC_STATE_U26,  &SYNC9)
ACTION_ENTRY(SYNC7              , CC_STATE_U10 , A_NO_ACTION     ,         CC_STATE_U10,  &SYNC8)
ACTION_ENTRY(SYNC6              , CC_STATE_U9  , A_NO_ACTION     ,         CC_STATE_U9,   &SYNC7)
ACTION_ENTRY(SYNC5              , CC_STATE_U8  , A_NO_ACTION     ,         CC_STATE_U8,   &SYNC6)
ACTION_ENTRY(SYNC4              , CC_STATE_U7  , A_NO_ACTION     ,         CC_STATE_U7,   &SYNC5)
ACTION_ENTRY(SYNC3              , CC_STATE_U6  , A_NO_ACTION     ,         CC_STATE_U6,   &SYNC4)
ACTION_ENTRY(SYNC2              , CC_STATE_U4  , A_NO_ACTION     ,         CC_STATE_U4,   &SYNC3)
ACTION_ENTRY(SYNC1              , CC_STATE_U3  , A_NO_ACTION     ,         CC_STATE_U3,   &SYNC2)
ACTION_ENTRY(MMCC_SYNC_IND_entry, CC_STATE_U1  , A_NO_ACTION     ,         CC_STATE_U1,   &SYNC1)

ACTION_ENTRY(ALERT1           ,   CC_STATE_U3  , A_STOP_T303_T310,         CC_STATE_U4,   (action_state_T*) ENDLIST)
ACTION_ENTRY(CC_ALERTING_entry,   CC_STATE_U1  , A_STOP_T303_T310,         CC_STATE_U4,   &ALERT1)

ACTION_ENTRY(CC_CALL_PROC_entry,  CC_STATE_U1  , A_STOP_T303_START_T310,   CC_STATE_U3,   (action_state_T*) ENDLIST)

ACTION_ENTRY(MMCC_REL_IND_entry,  CC_STATE_U0_1, A_CLEAR_DTMF_STOP_ALL_TIMERS,        CC_STATE_U0,   (action_state_T*) ENDLIST)

ACTION_ENTRY(MMCC_EST_CNF_entry,  CC_STATE_U0_1, A_NO_ACTION,              CC_STATE_U1,   (action_state_T*) ENDLIST)

ACTION_ENTRY(ESTREJ1            , CC_STATE_U0 ,  A_NO_ACTION     ,         CC_STATE_U0,   (action_state_T*) ENDLIST)
ACTION_ENTRY(MMCC_EST_REJ_entry , CC_STATE_U0_1, A_CLEAR_DTMF_STOP_ALL_TIMERS,        CC_STATE_U0,   &ESTREJ1)

#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
ACTION_ENTRY(SETUPREQ2,             CC_STATE_U1,   A_NO_ACTION,            CC_STATE_U1,   (action_state_T*) ENDLIST)
ACTION_ENTRY(SETUPREQ1,             CC_STATE_U0_6, A_NO_ACTION,            CC_STATE_U1,   &SETUPREQ2)
ACTION_ENTRY(MNCC_SETUP_REQ_entry,  CC_STATE_U0,   A_NO_ACTION,            CC_STATE_U0_1, &SETUPREQ1)
#else
#ifdef FEATURE_CCBS
ACTION_ENTRY(SETUPREQ1,             CC_STATE_U0_6, A_NO_ACTION,            CC_STATE_U1,  (action_state_T*)  ENDLIST)
ACTION_ENTRY(MNCC_SETUP_REQ_entry,  CC_STATE_U0,   A_NO_ACTION,            CC_STATE_U0_1, &SETUPREQ1)
#else
ACTION_ENTRY(MNCC_SETUP_REQ_entry,  CC_STATE_U0, A_NO_ACTION,              CC_STATE_U0_1, (action_state_T*) ENDLIST)
#endif /* FEATURE_CCBS */
#endif /* FEATURE_MULTIMEDIA_FALLBACK */

#ifdef FEATURE_CCBS
ACTION_ENTRY(MMCNM_PROMPT_IND_entry,     CC_STATE_U0,   A_NO_ACTION,       CC_STATE_U0_2, (action_state_T*) ENDLIST)

ACTION_ENTRY(MNCC_START_CC_REQ_entry,    CC_STATE_U0_2, A_NO_ACTION,       CC_STATE_U0_3, (action_state_T*) ENDLIST)

ACTION_ENTRY(CC_CC_ESTABLISHMENT_entry,  CC_STATE_U0_3, A_NO_ACTION,       CC_STATE_U0_4, (action_state_T*) ENDLIST)

ACTION_ENTRY(MNCC_CC_EST_CONF_REQ_entry, CC_STATE_U0_4, A_NO_ACTION,       CC_STATE_U0_5, (action_state_T*) ENDLIST)

ACTION_ENTRY(CC_RECALL_entry,            CC_STATE_U0_5, A_NO_ACTION,       CC_STATE_U0_6, (action_state_T*) ENDLIST)
#endif /* FEATURE_CCBS */
   
/*
 * The following array is arranged so that each entry contains the address
 * of the start of the linked list associated with the event number
 * corresponding to the index of the array. The event number is defined in
 * cm.h which should not be changed without updating this array
 */

static VOID_DATA *index[] =
{
   &MNCC_SETUP_REQ_entry,              /* 0  */
   &MMCC_EST_CNF_entry,                /* 1  */
   &MMCC_REL_IND_entry,                /* 2  */
   &CC_CALL_PROC_entry,                /* 3  */
   &CC_ALERTING_entry,                 /* 4  */
   &MMCC_SYNC_IND_entry,               /* 5  */
   &CC_CONNECT_entry,                  /* 6  */
   &CC_PROGRESS_entry,                 /* 7  */
   &MMCC_EST_IND_entry,                /* 8  */
   &MMCC_CALL_CNF_entry,               /* 9  */
   &MMCC_REJ_REQ_entry,                /* 10 */
   &MNCC_ALERT_REQ_entry,              /* 11 */
   &MNCC_SETUP_RSP_entry,              /* 12 */
   &CC_CONNECT_ACK_entry,              /* 13 */
   &MNCC_DISC_REQ_entry,               /* 14 */
   &CC_RELEASE_entry,                  /* 15 */
   &CC_DISCONN_entry,                  /* 16 */
   &MNCC_REL_REQ_entry,                /* 17 */
   &CC_REL_COMP_entry,                 /* 18 */
   &T303_TO_entry,                     /* 19 */
   &T305_TO_entry,                     /* 20 */
   &T308_TO_entry,                     /* 21 */
   &T310_TO_entry,                     /* 22 */
   &T313_TO_entry,                     /* 23 */
   &CC_MODIFY_entry,                   /* 24 */
   &MNCC_MODIFY_REQentry,              /* 25 */
   &T323_TO_entry,                     /* 26 */
   &CC_MODIFY_COMP_entry,              /* 27 */
   &CC_MODIFY_REJ_entry,               /* 28 */
   &MNCC_START_DTMF_entry,             /* 29 */
   &MNCC_STOP_DTMF_entry,              /* 30 */
   &CC_START_DTMF_ACK_entry,           /* 31 */
   &CC_STOP_DTMF_ACK_entry,            /* 32 */
   &CC_START_DTMF_REJ_entry,           /* 33 */
   &DTMF_T336_TO_entry,                /* 34 */
   &DTMF_T337_TO_entry,                /* 35 */
   &CC_STATUS_ENQ_entry,               /* 36 */
   &CC_STATUS_entry,                   /* 37 */
   &MNCC_FACIL_REQ_entry,              /* 38 */
   &CC_FACILITY_entry,                 /* 39 */
   &MNCC_USER_USER_entry,              /* 40 */
   &CC_USER_INFO_entry,                /* 41 */
   &CC_CONGESTION_entry,               /* 42 */
   &MNCC_NOTIFY_REQ_entry,             /* 43 */
   &CC_NOTIFY_entry,                   /* 44 */
   &MNCC_CALL_HOLD_entry,              /* 45 */
   &MNCC_CALL_RETR_entry,              /* 46 */
   &CC_HOLD_ACK_entry,                 /* 47 */
   &CC_HOLD_REJ_entry,                 /* 48 */
   &CC_RETRIEV_ACK_entry,              /* 49 */
   &CC_RETRIEV_REJ_entry,              /* 50 */
   &MNCC_SETUP_COMPL_RES_entry,        /* 51 */
   &MNCC_MODIFY_RES_entry,             /* 52 */
   &MMCC_EST_REJ_entry,                /* 53 */
   &T324_TO_entry,                      /* 54 */
#ifdef FEATURE_CCBS
   &MMCNM_PROMPT_IND_entry,            /* 55 */
   &MNCC_START_CC_REQ_entry,           /* 56 */
   &CC_CC_ESTABLISHMENT_entry,         /* 57 */
   &MNCC_CC_EST_CONF_REQ_entry,        /* 58 */
   &CC_RECALL_entry,                   /* 59 */
   &UNDEFINED_MESSAGE_entry,           /* 60 */
#else
   &UNDEFINED_MESSAGE_entry,           /* 55 */
#endif /* FEATURE_CCBS */
   (VOID_DATA *) 0
};

/**************** Begin of   CNM_cc_check_respond_to_event ************************
 *
 *  Function name: CNM_cc_check_respond_to_event()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the cc state event matrix. It is called with
 *  an event identifier, then, depending on the current state, returns an
 *  action code which relates to an action to be performed. (Initiated
 *  by calling CNM_cc_respond_to_event() ). The global store next_state is
 *  updated with the next state that this transaction will enter, if the
 *  message is accepted for processing.+
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN - Identity of the current connection
 *  cc_event : IN - identity of the event triggering this processsing
 *
 *  Returns:
 *  --------
 *  action_code - a value which defines the action to be performed if the
 *  message checks out ok
 *
 ******************************************************************************/

byte CNM_cc_check_respond_to_event( connection_id_T  connection_id,
                                     event_T cc_event )
{

   cc_state_T        cc_state;
   action_state_T    *p_action_state;
   byte              action;

   action = A_BAD_EVENT;

   if ((connection_id & 0x07) != 0x07)
   {
      if(cc_transaction_information_p[connection_id] != NULL)
      {
      
         cc_state = cc_transaction_information_p[connection_id]->cc_state;

         /*
          * save the current cc state for this transaction
         */

         last_state = cc_state;

         if (cc_event <= MAX_CC_EVENT)
         {
            p_action_state = index[cc_event];

            while ( p_action_state->cur_state != cc_state )
            {
               p_action_state = p_action_state->nextentry;
               if (p_action_state == (VOID_DATA *) 0)
               {
                  break;
               }
            }

            if ( p_action_state != (VOID_DATA *)0 )
            {
               action = p_action_state->action;
               next_state[connection_id] = p_action_state->nextstate;
            }
            else
            {
               action = A_BAD_EVENT;
               next_state[connection_id] = cc_state;
            }
         }
         else
         {
            action = A_BAD_EVENT;
            next_state[connection_id] = cc_state;
         }
      }
      else
      {
          action = A_BAD_EVENT;
      }
   }
   else
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid Transaction Id value of 7: received connection_id = %d\n",
                 (dword) connection_id);
      action = A_BAD_EVENT;
      next_state[connection_id] = CC_STATE_U0;
   }

   return ( (byte)action );
}
/**************** End of   CNM_cc_check_respond_to_event *************************/


/**************** Begin of   CNM_cc_respond_to_event ******************************
 *
 *  Function name: CNM_cc_respond_to_event()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  This function performs the actions associated with a state change. The
 *  parameter action_code selects the action to perform
 *
 *  Parameters:
 *  -----------
 *  connection_id - IN  : identity of the current connection
 *  action_code - IN    : parameter which defines the action to perform
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/
VOID_FUNC CNM_cc_respond_to_event( connection_id_T  connection_id,
                                  byte action_code )
{

   cc_state_T      prev_state = CC_STATE_U0;

   if(cc_transaction_information_p[connection_id] != NULL)
   {
     prev_state = cc_transaction_information_p[connection_id]->cc_state;   
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot update prev_state as cc_transaction_information_p[%d] is NULL\n",
               connection_id);
   }

   /* log calls statistics events */
   CNM_log_event_calls_statistics(connection_id, 
                                  prev_state, 
                                  next_state[connection_id]);

   switch (action_code)
   {
      case A_BAD_EVENT:

         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Asked to respond to a BAD_EVENT in cc_respond_to_event()\n");
         break;

      case A_NO_ACTION:

         break;

      case A_STOP_ALL_TIMERS:

         CNM_stop_timer(ALL_CC_TIMERS, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));
         break;

      case A_START_T303:

         CNM_cc_start_timer(TIMER_T303,connection_id);
         break;

      case A_STOP_T303_START_T310:

         CNM_stop_timer(TIMER_T303, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));
         CNM_cc_start_timer(TIMER_T310,connection_id);
         break;

      case A_STOP_T303_T310:

         CNM_stop_timer(TIMER_T303, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));
         CNM_stop_timer(TIMER_T310, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));
         break;

      case A_READ_DTMF_STOP_T303_STOP_T310:

         CNM_stop_timer(TIMER_T303, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));
         CNM_stop_timer(TIMER_T310, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));

         /*
          * Update state to allow dtmf message processing in next state
          */

         cc_transaction_information_p[connection_id]->cc_state =
                                                next_state[connection_id];
         CNM_read_dtmf_queue(connection_id);
         break;

      case A_START_T313:

         CNM_cc_start_timer(TIMER_T313,connection_id);
         break;

      case A_READ_DTMF_STOP_T313:

         CNM_stop_timer(TIMER_T313, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));

         /*
          * Update state to allow dtmf message processing in next state
          */

         cc_transaction_information_p[connection_id]->cc_state =
                                                next_state[connection_id];

         CNM_read_dtmf_queue(connection_id);
         break;

      case A_START_T305:

         CNM_cc_start_timer(TIMER_T305,connection_id);
         break;

      case A_CLEAR_DTMF_STOP_ALL_TIMERS:

         CNM_stop_timer(ALL_CC_TIMERS, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));
         CNM_clear_dtmf_queue(connection_id);
         break;

      case A_CLEAR_DTMF_START_T308:

         CNM_cc_start_timer(TIMER_T308,connection_id);
         CNM_clear_dtmf_queue(connection_id);
         break;

      case A_START_T308:

         CNM_cc_start_timer(TIMER_T308,connection_id);
         break;

      case A_START_T323:

         CNM_cc_start_timer(TIMER_T323,connection_id);
         break;

      case A_START_T324:

         CNM_cc_start_timer(TIMER_T324,connection_id);
         break;

      case A_READ_DTMF_STOP_T323:

         CNM_stop_timer(TIMER_T323, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));

         /*
          * Update state to allow dtmf message processing in next state
          */

         cc_transaction_information_p[connection_id]->cc_state =
                                                   next_state[connection_id];
         CNM_read_dtmf_queue(connection_id);
         break;

      case A_READ_DTMF_STOP_T324:

         CNM_stop_timer(TIMER_T324, (unsigned char)FORM_TI_PD( connection_id, PD_CC ));

         /*
          * Update state to allow dtmf message processing in next state
          */

         cc_transaction_information_p[connection_id]->cc_state =
                                                   next_state[connection_id];
         CNM_read_dtmf_queue(connection_id);
         break;

      case A_READ_DTMF:


      /*
       * Update state to allow dtmf message processing in next state
       */

         cc_transaction_information_p[connection_id]->cc_state =
                                                   next_state[connection_id];
         CNM_read_dtmf_queue(connection_id);
         break;

      default:

         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Unexpected action_code, given action_code = %d\n",
                    (dword) action_code);

         break;

   }

   if ( next_state[connection_id] == CC_STATE_U0 )
   {
      CNM_set_state_to_U0( connection_id );
   }
   else
   {
      cc_transaction_information_p[connection_id]->cc_state = next_state[connection_id];

#ifdef FEATURE_CNM_UNIT_TEST
      MSG_HIGH_DS_3(MN_SUB,"=MNCNM= CC-%d state transition from %s to %s",
                connection_id, 
                DEBUG_get_state_name(prev_state),
                DEBUG_get_state_name(next_state[connection_id]));
#else
      MSG_HIGH_DS_3(MN_SUB,"=MNCNM= CC-%d state transition from %d to %d",
                connection_id, 
                prev_state,
                next_state[connection_id]);
#endif
                                                
   }

}
/**************** End of   CNM_cc_respond_to_event *******************************/


/**************** Begin of   CNM_set_state_to_U0 **********************************
 *
 *  Function name: CNM_set_state_to_U0()
 *  -----------------------------------
 *
 *  Description:
 *  ------------
 *  Function to perform the actions required when a  state returns to CC_STATE_U0
 *
 *  Parameters:
 *  -----------
 *  connection_id : INPUT
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_set_state_to_U0( connection_id_T connection_id )
{
   cc_state_T   prev_state;

   if(cc_transaction_information_p[connection_id] != NULL)
   {
   
       prev_state = cc_transaction_information_p[connection_id]->cc_state;

       /* log calls statistics events */
       CNM_log_event_calls_statistics(connection_id, 
                                       prev_state, 
                                      CC_STATE_U0);

#ifdef FEATURE_CNM_UNIT_TEST
       MSG_HIGH_DS_3(MN_SUB,"=MNCNM= CC-%d state transition from %s to %s",
           connection_id, 
       DEBUG_get_state_name(prev_state),
       DEBUG_get_state_name(cc_transaction_information_p[connection_id]->cc_state));

#else
       MSG_HIGH_DS_3(MN_SUB,"=MNCNM= CC-%d state transition from %d to %d",
           connection_id, 
           prev_state,
           cc_transaction_information_p[connection_id]->cc_state);

#endif

#ifdef FEATURE_MODEM_HEAP
       modem_mem_free(cc_transaction_information_p[connection_id], MODEM_MEM_CLIENT_NAS);
       MSG_HIGH_DS_0(MN_SUB,"=MNCNM= FREE - cc_transaction_information_p\n" );
#endif
       cc_transaction_information_p[connection_id] = NULL;  
   }
}
/**************** End of   CNM_set_state_to_CC_STATE_U0 ***********************************/
