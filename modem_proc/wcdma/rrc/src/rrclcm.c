/*===========================================================================

                                  R R C L C M

GENERAL DESCRIPTION
 The RRCLCM module consists of Layer 2 ACK Manager and RLC Logical Channel ID 
 Manager.

EXTERNALIZED FUNCTIONS

  rrclcm_init_ul_rlc_lc_info()
    This function will initialize Uplink RLC Logical Channel Info to the 
    default values. All the Uplink RLC Logical Channel entities will be set to
    "Not in use" status.

  rrclcm_init_dl_rlc_lc_info()
    This function will initialize Downlink RLC Logical Channel Info to the 
    default values. All the Downlink RLC Logical Channel entities will be set
    to "Not in use" status.

  rrclcm_allocate_ul_rlc_lc_id()
    This function will allocate Uplink RLC Logical Channel Id for the 
    requested RLC service entity.

  rrclcm_allocate_dl_rlc_lc_id()
    This function will allocate Downlink RLC Logical Channel Id for the
    requested RLC service entity.

  rrclcm_check_ul_rlc_lc_id()
    This function will check whether Uplink RLC Logical Channel Id is alloated
    or not for the requested RLC service entity.

  rrclcm_check_dl_rlc_lc_id()
    This function will check whether Downlink RLC Logical Channel Id is alloated
    or not for the requested RLC service entity.

  rrclcm_deallocate_ul_rlc_lc_id()
    This function will deallocate Uplink RLC Logical Channel Id requested

  rrclcm_deallocate_dl_rlc_lc_id()
    This function will deallocate Downlink RLC Logical Channel Id requested
    
  rrclcm_get_dl_lc_type()
    This function will identify the Logical Channel to which the OTA message
    belongs. The RRC procedure uses this infomation for processing the 
    received message.

  rrclcm_get_ul_lc_type()
    This function will identify the Logical Channel to which the OTA message
    belongs. The RRC procedure uses this infomation for sending the OTA 
    message.

  rrclcm_identify_proc()
    This function will processes the event from the Dispatcher and identifies
    the procedure to which the L2 ACK belongs.

  rrclcm_get_mui()
    This function builds the MUI for the RRC procedure and RRC procedure can
    include the same in the SDU if it is transmitting SDU in Acknowledged mode.
    
  rrclcm_init_l2_ack_info()
    This function will initialize the L2 ACK info of all RRC procedures.

  rrclcm_post_event_for_dl_sdu()
    This function will post an internal event to RRC for the downlink SDU.

  rrclcm_get_dl_watermark_ptr()
    This function will get watermark pointer corresponding to the RLC Logical
    channel Id.

  rrclcm_get_ul_watermark_ptr()
    This function will get watermark pointer corresponding to the RLC Logical
    channel Id.

  rrclcm_get_dl_rb_id()
    This function will get radio bearer Id  corresponding to the RLC Logical
    channel Id.

  rrclcm_get_ul_rb_id()
    This function will get radio bearer Id  corresponding to the RLC Logical
    channel Id.

  rrclcm_clear_rlc_lc_info()
    This function clears and initializes the RLC Logical Channel Id
    info for both Uplink and Downlink.

  rrclcm_get_ul_rlc_mode
    This function will check whether Uplink RLC Logical Channel Id is alloated 
    or not for the requested RLC service entity. If allocated then it returns
    assoctaed mode of transmission.

  rrclcm_get_dl_rlc_mode
    This function will check whether Downlink RLC Logical Channel Id is alloated 
    or not for the requested RLC service entity. If allocated then it returns 
    associated mode reception.

Copyright (c) 2000-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/vcs/rrclcm.c_v   1.22   12 Jun 2002 06:54:58   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrclcm.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/12/15   sa      Made changes for QTF with JO CRM
04/02/14   vg      Made changes to Systematically log all the internal and external primitive handled in RRC and corresponding top level action.
08/13/13   gv      Changes to not set L2 ack request mask to block TA while
                   sending RRC Status message from RRC_PROCEDURE_NONE
08/13/13   gv      Do not allow TA if l2 is waiting for ACK from network for its internal data
08/13/13   gv      Set L2 ack req mask when there is a pending SRB3 message
05/23/13   as      Made changes to consider the successful decode of DDT as implicit L2 ACK
                   for Security mode complete
01/03/13   ad      Made changes to reduce the redundant F3s
10/18/12   gv      Replace FEATURE_DUAL_SIM_WCDMA_TUNEAWAY with FEATURE_DUAL_SIM
10/01/12   as      Fixed compilation warnings
09/28/12   pm      Made changes to enable the feature 'FEATURE_WCDMA_RLC_UNREC_ERROR_AT_COUNT_C_ACT'
                   only during non-zero T314 values.
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/16/12   ad      Changed the header name from rrclcmv to rrclcm and remove the CMI 
                   featuriztion.
04/13/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.95.50
04/10/11   ad      Fixed compilation warnings
01/28/11   rl      Merged with VU_MODEM_WCDMA_RRC.01.89.57
01/12/11   ad      Added changes for RRC SW decoupling
12/31/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.86.50
11/09/10   rl      Merged with VU_MODEM_WCDMA_RRC.01.84.50
11/02/10   as      Fixed compiler error
10/28/10   as      Added changes to check for DL ACK for Tuneaway
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
06/29/10   as      Removed feature flag WCDMA_UNIT_TEST for rrc stub code
02/10/10   as      Added feature falg WCDMA_UNIT_TEST for rrc stub code
12/07/09   ss      Changes to add Stub code under feature flag TEST_FRAMEWORK
11/05/09   ss      Fixed lint error.
11/04/09   ss      Fixed lint error.
10/30/09   ss      Fixed lint and Klockwork errors.
10/28/09   gkg     Fixed KW Errors.
06/23/09   ps      Made changes to ignore L2 ack for CRLC REL all req
05/10/09   kp      Added support for demand paging RRC code using pragma.
05/08/09   ss      Updated Copyright Information
04/01/09   rm      Featurisation due to a qbuild issue for MOB and CMI targets
02/27/09   ps      Made changes to not submit a L2 ACk Ind in internal Q if
                   procedure doesnt expect an ack
01/05/09   ps      Made changes to call dsm_queue_init for watermark queues
                   only at RRC initialisation stage
12/22/08   ps      Made changes to call dsm_queue_init instaed of q_init 
                   for watermark queues created by RRC.
03/06/08   da      Changed FEATURE_MBMS to FEATURE_MODEM_MBMS
12/27/07   rm      Added function rrclcm_get_nchan_dl_tm to count number of
                   DL TM channels based on LCM db
12/21/07   da      Support for FEATURE_MBMS
03/23/07   da      changed f3 to show status of RRC_DOWNLINK_L2ACK_IND
01/22/07   vr      Fixed lint errors.
08/22/06   tkk     Featurized RLC control logical channels to reduce global 
                   memory utilization.
08/21/06   tkk     Added SRB#5 support.
04/14/06   vr      Changed function definition of rrclcm_post_event_for_dl_l2ack_ind
                   to support failure reason in L2 Ack Ind from RLC.
04/29/05   da      Added msg_highs for debugging purpose.
04/06/05   da      Added initial support for the handling of CTCH.
10/01/04   kc      Reverted back the number of MUI's that can be outstanding to the
                   number of procedures that RRC supports.
09/16/04   vk      Increased the number of procedures in static array that stores
                   items for L2 ACK management and RLC Logical Channel ID Management 
                   by a factor of 5
09/10/04   svk     Initialized the rrclcm_current_mui only once at powerup.
08/19/04   sgk     Initialized rrclcm_current_mui in a loop to fix lint error 
                   'Too few initializers for aggregate' in function 
                   rrclcm_get_mui  
12/29/03   vsk     Handle the change in RLC callback
01/20/03   sk      Fixed the following errors detected by lint
                   Changed the type of local variable 'i' from int to uint8
                   and initialised 'i' with 0
                   Changed the type of local variable 'rb_idx' from int to uint32
                   Corrected the indentation.
06/11/02   rj      Modified MUI management to per Radio Bearer basis and 
                   updated functions rrclcm_get_mui, rrclcm_identify_proc and
                   rrclcm_init_l2_ack_info to send MUI for every uplink
                   transmission and also to ensure continuity in MUIs.
06/07/02   vn      While deallocating UL and DL logical channels for User plane
                   RBs, set the logical channel type to UE_LOGCHAN_DTCH.
05/07/02   rj      Modified rrclcm_post_event_for_dl_sdu function to change
                   the interface  between RRC and RLC to dequeue multiple 
                   SDUs if RLC enqueues before calling callback function.
03/26/02   rj      Updated function rrclcm_init_ul_rlc_lc_info to initialize
                   spare UM Logical Channel entities to DTCH.
03/26/02   rj      Updated functions rrclcm_allocate_ul_rlc_lc_id and
                   rrclcm_allocate_dl_rlc_lc_id to fix a problem with statically 
                   allocated radio bearers.
12/03/01   rj      Added new interface functions rrclcm_get_ul_rlc_mode and
                   rrclcm_get_dl_rlc_mode to return the mode of transmission or
                   reception for the allocated RLC Logical Channel Id.
09/14/01   rj      Updated function rrclcm_clear_rlc_lc_info to clear L2 Ack
                   information while clearing RLC Id information.
08/30/01   rj      Added new interface functions rrclcm_get_ul_rlc_lc_id and
                   rrclcm_get_dl_rlc_lc_id for RRCRBE module to get RLC ID for 
                   a particular Radio Bearer Id. Changed 
                   UE_MODE_ACKNOWLEDGED_DATA_PLANE and  
                   UE_MODE_ACKNOWLEDGED_CONTROL_PLANE to UE_MODE_ACKNOWLEDGED_DATA
                   and UE_MODE_ACKNOWLEDGED_CONTROL respectively.
06/19/01   kmp     Updated rrclcm_post_event_for_dl_sdu() to put in more 
                   detailed debug info.
06/15/01   vn      In rrclcm_init_dl_rlc_lc_info and rrclcm_init_ul_rlc_lc_info,
                   set the logical chanel type to DTCH for all non SRBs.
06/14/01   vn      Fixed a bug where rrclcm_deallocate_ul_rlc_lc_id was calling
                   rrclcm_get_dl_watermark_ptr. Added functionality for clearing
                   all logical channel info (UL and DL) in rrclcm_clear_rlc_lc_info.
                   Call dsm_free_packet instead of dsm_dequeue to free all DSM items
                   in rrclcm_deallocate_ul_rlc_lc_id and rrclcm_deallocate_dl_rlc_lc_id.
06/08/01   rj      Modified interfaces to callback function 
                   rrclcm_post_event_for_dl_l2ack_ind to match the RLC 
                   declarations.
05/24/01   rj      Corrected some comments.
05/23/01   rj      Modified rrclcm_deallocate_rlc_lc_id function into 
                   rrclcm_deallocate_ul_rlc_lc_id and 
                   rrclcm_deallocate_dl_rlc_lc_id to de-allocate RLC Logical
                   channel IDs. This functions take RLC Logical Channel ID
                   as formal paramter instead of Radio Bearer ID and flushes
                   out any left over DSM items from the water mark queues.
05/11/01   rj      Added a new function rrclcm_check_dl_rlc_lc_id 
                   to check whether RLC Logical Channel Id is allocated for 
                   requested RLC Service entity or not.
                   In the function rrclcm_post_event_for_dl_l2ack_ind moved the
                   call to rrc_put_int_cmd after the MSG_LOW macro since the message
                   display was out of sequence.
03/30/01   rj      Changed dl_sdu_type in function rrclcm_post_event_for_dl_sdu
                   to uint32 from uint8 to be compatible with latest LED 
                   rrcasn1_v.h
03/08/01   ram     In the function rrclcm_post_event_for_dl_sdu moved the call
                   to rrc_put_int_cmd after the MSG_LOW macro since the message
                   display was out of sequence.
02/01/01   rj/ram  Included msg.h and err.h in this file.The RLC Define constants
                   are replaced with uecomdef define constants.
01/30/01   rj      rrclcm_lc_id_type, rrclcm_channel_type_e_type  and 
                   rlc_mode_e_type are replaced with rlc_lc_id_type,
                   uecomdef_logchan_e_type and uecomdef_logch_mode_e_type                  
                   repectively from uecomdef.h. 
                   In function rrclcm_post_event_for_dl_sdu, the initialization
                   of dl_sdu_type is changed to Radio Bearer instead from 
                   Logical Channel.
                   All enums for rrclcm_lc_id_type, rrclcm_channel_type_e_type and
                   rlc_mode_e_type are replaced with uecomdef.h enums.
01/25/01   rj      Included uecomdef.h in this file and modified the code for
                   Radio Bearer constants in the uecomdef.h
12/22/00   rj      Modified interface for rrclcm_check_ul_rlc_lc_id. Removed
                   Watermark ptr from the formal parameters of this function.
                   RRC procedures does not have deal with the watermark queues.
                   Only SEND CHAIN will deal with the uplink watermark queues.
                   Added identification of SDU type to 
                   rrclcm_post_event_for_dl_sdu function.
12/13/00   rj      Added rrclcm_get_ul_rb_id, rrclcm_get_dl_rb_id,
                   rrclcm_get_ul_watermark_ptr,  and rrclcm_get_dl_watermark_ptr
                   and  deleted rrclcm_get_watermark_ptr and rrclcm_get_rb_id.
12/07/00   rj      Updated functions rrclcm_post_event_for_dl_l2ack_ind,
                   rrclcm_post_event_for_dl_sdu, and rrclcm_identify_proc to
                   work with unified RRC command type.
12/01/00   rj      Command Ids are initialized in the functions
                   rrclcm_post_event_for_dl_sdu and 
                   rrclcm_post_event_for_dl_l2ack_ind.
11/30/00   rj      Changed name from rrclcp.c to rrclcm.c and implemented
                   REX++ updates.
11/20/00   rj      Editorial corrections. changed rrclcm_status_type to
                   rrclcm_status_e_type
11/19/00   rj      Updates for the code review comments.
                   Added a new function rrclcm_get_rb_id.
                   Implementation for L2 ACK management is changed to take 
                   care of multiple instances of same RRC procedure.
11/13/00   rj      modified interfaces to functions 
                   rrclcm_allocate_ul_rlc_lc_id, rrclcm_allocate_dl_rlc_lc_id
                   rrclcm_check_ul_rlc_lc_id. Also logic for RB ID allocation
                   and deallocation is modified.
                   Added a new function rrclcm_get_watermark_ptr.
10/30/00   rj      modifed code for rrclcm_identify_proc function and 
                   corrected typos.
10/29/00   rj      Editorial corrections for compilation errors.
10/25/00   rj      Created file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "wcdma_variation.h"
#include "rrclcm.h"

#include "msg.h"
#include "err.h"
#include "uecomdef.h"
#include "rrcllc.h"

#ifdef FEATURE_DUAL_SIM
#include "rrcwrm.h"
#endif

#include "rrcdata_v.h" 
#include "rlcrrcif_v.h" 
#ifdef TEST_FRAMEWORK
#error code not present
#endif


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/*===========================================================================

               Data structures needed for communicating with RLC.

===========================================================================*/
               
LOCAL dsm_watermark_type rrclcm_rb32_bcch_dl_wmq;   
                                           /* WaterMark Queue used when SDU
                                              is received from RLC on RB 32 
                                              using BCCH Logical channel
                                              for serving cell. */
LOCAL q_type rrclcm_rb32_bcch_dl_q;        /* Queue around which 
                                              rrclcm_rb32_bcch_dl_wmq is 
                                              built. */

LOCAL dsm_watermark_type rrclcm_rb33_bcch_dl_wmq;
                                           /* WaterMark Queue used when SDU
                                              is receiveed from RLC on RB 33
                                              using BCCH Logical Channel for 
                                              neighbor cell. */

LOCAL q_type rrclcm_rb33_bcch_dl_q;         /* Queue around which 
                                              rrclcm_rb33_bcch_dl_wmq is 
                                              built. */
LOCAL dsm_watermark_type rrclcm_rb34_bcch_fach_dl_wmq;
                                           /* WaterMark Queue used when SDU
                                              is receiveed from RLC on RB 34
                                              using BCCH Logical Channel 
                                              which is mapped to FACH transport
                                              channel. */

LOCAL q_type rrclcm_rb34_bcch_fach_dl_q;   /* Queue around which 
                                              rrclcm_rb34_bcch_fach_dl_wmq is 
                                              built. */
LOCAL dsm_watermark_type rrclcm_rb35_pcch_dl_wmq;
                                           /* WaterMark Queue used when SDU
                                              is receiveed from RLC on RB 35
                                              using PCCH Logical Channel. */

LOCAL q_type rrclcm_rb35_pcch_dl_q;        /* Queue around which 
                                              rrclcm_rb35_pcch_dl_wmq is 
                                              built. */
LOCAL dsm_watermark_type rrclcm_rb0_ccch_dl_wmq;   
                                           /* WaterMark Queue used when SDU
                                              is received from RLC on RB 0 
                                              using CCCH Logical channel.*/
LOCAL q_type rrclcm_rb0_ccch_dl_q;         /* Queue around which 
                                              rrclcm_rb0_ccch_dl_wmq is 
                                              built. */

LOCAL dsm_watermark_type rrclcm_rb0_ccch_ul_wmq;
                                           /* WaterMark Queue used when SDU
                                              is transmitted to RLC on RB 0
                                              using CCCH Logical Channel. */

LOCAL q_type rrclcm_rb0_ccch_ul_q;         /* Queue around which 
                                              rrclcm_rb0_ccch_ul_wmq is 
                                              built. */
LOCAL dsm_watermark_type rrclcm_rb1_dcch_dl_wmq;   
                                           /* WaterMark Queue used when SDU
                                              is received from RLC on RB 1 
                                              using DCCH Logical channel. */
LOCAL q_type rrclcm_rb1_dcch_dl_q;         /* Queue around which 
                                              rrclcm_rb1_dcch_dl_wmq is 
                                              built. */

LOCAL dsm_watermark_type rrclcm_rb1_dcch_ul_wmq;
                                           /* WaterMark Queue used when SDU
                                              is transmitted to RLC on RB 1
                                              using DCCH Logical Channel. */

LOCAL q_type rrclcm_rb1_dcch_ul_q;         /* Queue around which 
                                              rrclcm_rb1_dcch_ul_wmq is 
                                              built. */
LOCAL dsm_watermark_type rrclcm_rb2_dcch_dl_wmq;   
                                           /* WaterMark Queue used when SDU
                                              is received from RLC on RB 2 
                                              using DCCH Logical channel. */
LOCAL q_type rrclcm_rb2_dcch_dl_q;         /* Queue around which 
                                              rrclcm_rb2_dcch_dl_wmq is 
                                              built. */

LOCAL dsm_watermark_type rrclcm_rb2_dcch_ul_wmq;
                                           /* WaterMark Queue used when SDU
                                              is transmitted to RLC on RB 2
                                              using DCCH Logical Channel. */

LOCAL q_type rrclcm_rb2_dcch_ul_q;         /* Queue around which 
                                              rrclcm_rb2_dcch_ul_wmq is 
                                              built. */
LOCAL dsm_watermark_type rrclcm_rb3_dcch_dl_wmq;   
                                           /* WaterMark Queue used when SDU
                                              is received from RLC on RB 3 
                                              using DCCH Logical channel. */
LOCAL q_type rrclcm_rb3_dcch_dl_q;         /* Queue around which 
                                              rrclcm_rb3_dcch_dl_wmq is 
                                              built. */

LOCAL dsm_watermark_type rrclcm_rb3_dcch_ul_wmq;
                                           /* WaterMark Queue used when SDU
                                              is transmitted to RLC on RB 3
                                              using DCCH Logical Channel. */

LOCAL q_type rrclcm_rb3_dcch_ul_q;         /* Queue around which 
                                              rrclcm_rb3_dcch_ul_wmq is 
                                              built. */
LOCAL dsm_watermark_type rrclcm_rb4_dcch_dl_wmq;   
                                           /* WaterMark Queue used when SDU
                                              is received from RLC on RB 4 
                                              using DCCH Logical channel. */
LOCAL q_type rrclcm_rb4_dcch_dl_q;         /* Queue around which 
                                              rrclcm_rb4_dcch_dl_wmq is 
                                              built. */

LOCAL dsm_watermark_type rrclcm_rb4_dcch_ul_wmq;
                                           /* WaterMark Queue used when SDU
                                              is transmitted to RLC on RB 4
                                              using DCCH Logical Channel. */

LOCAL q_type rrclcm_rb4_dcch_ul_q;         /* Queue around which 
                                              rrclcm_rb4_dcch_ul_wmq is 
                                              built. */

LOCAL dsm_watermark_type rrclcm_srb5_dcch_dl_wmq;   
                                           /* WaterMark Queue used when SDU
                                              is received from RLC on SRB#5 
                                              using DCCH_TM Logical channel
                                              for serving cell. */
LOCAL q_type rrclcm_srb5_dcch_dl_q;        /* Queue around which 
                                              rrclcm_srb5_dcch_dl_wmq is 
                                              built. */


static boolean rrclcm_init_current_mui_array_at_poweup = TRUE;
/*===========================================================================
  Defining the static array that stores items for L2 ACK management and RLC
  Logical Channel ID Management
===========================================================================*/

LOCAL rrclcm_l2_ack_info_type 
  rrclcm_l2_ack_info[MAX_NUM_OF_PROCEDURES][UE_MAX_UL_AM_DATA_CHANNEL - 1];
                                           
LOCAL rrclcm_ul_rlc_lc_info_type rrclcm_ul_rlc_lc_info
                                  [UE_MAX_UL_LOGICAL_CHANNEL];

LOCAL rrclcm_dl_rlc_lc_info_type rrclcm_dl_rlc_lc_info
                                  [UE_MAX_DL_LOGICAL_CHANNEL];       

/*===========================================================================
** -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/*===========================================================================
** -----------------------------------------------------------------------------------
**-- ----------NON-Demand paging section Srart--------------------------------------------
** -----------------------------------------------------------------------------------
  This section will have code section that will not be demand paged. Function which should be in this section are
  -RRC functions that are called by L1/L2 which are higher priority then RRC
  -RRC functions that are called in ISR context or RTOS timer call back
  -RRC functions that indirectly gets called by L1/L2 /ISR/timer call back   
  For example: Fun1() gets called by L1. Fun2() is called by Fun1(). Now both  Fun1() & Fun2() should be NON demand paged, 
  so both should be added in this section  
  Usage of pragma 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void foo(void)
  {
    function body here
  }
        __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
  If you have another function to be added in the same section then again it needs to be wrapped with these pragma 
  for example:- function bar() is just below function foo() then function bar() needs to be written like this …. 
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__
  void bar(void)
  {
    function body here
  }
  __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__
** ----------------------------------------------------------------------- */


/*===========================================================================


FUNCTION rrclcm_check_dl_rlc_lc_id

DESCRIPTION
  This function will check whether Downlink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. RRC LLC can use this
  function to check whether a particular Logical channel entity is already 
  setup or not. If this function returns RRCLCM_RLC_LC_ID_NOT_FOUND, then the
  RRC LLC needs to setup Logical Channel entity  and the corresponding 
  physical channel for receiving SDU on the downlink.

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If it is not alreday allocated,
   it'll return RRCLCM_RLC_LC_ID_NOT_FOUND.

SIDE EFFECTS
  None
===========================================================================*/
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/

rlc_lc_id_type rrclcm_check_dl_rlc_lc_id
(    
  uecomdef_logchan_e_type dl_lc_type,       /* Downlink Logical Channel Type. */
  rrc_RB_Identity rb_id,                    /* Radio Bearer Id assoicated with
                                               the above Downlink Logical 
                                               Channel type. */
  uecomdef_logch_mode_e_type mode           /* Mode of transmission for the RLC 
                                               service entity. */
 
)
{
  int i=0;
  boolean dl_lc_id_found = FALSE;

  switch (mode) 
  {
  /* Checks for Logical Channel Type and Radio Bearer Id match for Transparent
     mode. */
    case (UE_MODE_TRANSPARENT): 
      for( i =RRCLCM_DL_TM_START_IDX ; i < UE_MAX_DL_TM_CHANNEL ; i++ )
      {
        if( dl_lc_type == rrclcm_dl_rlc_lc_info[i].lc_type && 
             rb_id == rrclcm_dl_rlc_lc_info[i].rb_id &&
             rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
        {
          dl_lc_id_found = TRUE;           /* Match for the requested service 
                                              is found */
          break;
        }
      }
      break;
  /* Checks for Logical Channel Type and Radio Bearer Id match for 
     Unacknowledged mode. */
    case (UE_MODE_UNACKNOWLEDGED): 
      for( i = RRCLCM_DL_UM_START_IDX ; i < (RRCLCM_DL_UM_START_IDX +
               UE_MAX_DL_UM_CHANNEL) ; i++ )
      {
        if( dl_lc_type == rrclcm_dl_rlc_lc_info[i].lc_type && 
             rb_id == rrclcm_dl_rlc_lc_info[i].rb_id &&
             rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
        {
          dl_lc_id_found = TRUE;           /* Match for the requested service 
                                              is found */
          break;
        }
      }
      break;  
  /* Checks for Logical Channel Type and Radio Bearer Id match for 
     Acknowledged Data mode. */
    case (UE_MODE_ACKNOWLEDGED_DATA): 
      for( i = (RRCLCM_DL_AM_DATA_START_IDX); 
           i < (RRCLCM_DL_AM_DATA_START_IDX + UE_MAX_DL_AM_DATA_CHANNEL) ;
           i++ )
      {
        if( dl_lc_type == rrclcm_dl_rlc_lc_info[i].lc_type && 
             rb_id == rrclcm_dl_rlc_lc_info[i].rb_id &&
             rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
        {
          dl_lc_id_found = TRUE;           /* Match for the requested service 
                                              is found */
          break;
        }
      }
      break;              
    default:
      WRRC_MSG0_ERROR("This should never happen !!");
      break;
  }
  if(dl_lc_id_found)
  {
    return (rrclcm_dl_rlc_lc_info[i].lc_id);
  }
  else
  {
    return (RRCLCM_RLC_LC_ID_NOT_FOUND);   /* Match for the requested service 
                                              is not found. The RRC LLC needs 
                                              to request for allocation. */
  }
} /* rrclcm_check_dl_rlc_lc_id */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/
/*===========================================================================

FUNCTION rrclcm_get_ul_rb_id

DESCRIPTION
  This function will get radio bearer Id  corresponding to the RLC Logical
  channel Id.

DEPENDENCIES
  None

RETURN VALUE
  rrc_RB_Identity.

SIDE EFFECTS
  None
===========================================================================*/
 /***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED__ /***/
rrc_RB_Identity rrclcm_get_ul_rb_id
(    
  rlc_lc_id_type ul_lc_id                  /* The uplink RLC Logical
                                              Channel Id. */   
)
{
  if(ul_lc_id >= UE_MAX_UL_LOGICAL_CHANNEL)
  {
    ERR_FATAL("ul_lc_id out of bounds : 0x%x",ul_lc_id,0,0);
  }
  return rrclcm_ul_rlc_lc_info[ul_lc_id].rb_id ;
} /* rrclcm_get_rb_dl_id */
/***/ __WCDMA_RRC_CODE_SEGMENT_NON_DEMANDPAGED_END__ /***/

/*===========================================================================

FUNCTION rrclcm_create_ul_dl_rlc_lc_q

DESCRIPTION
  This function will crete the Uplink/Downlinkwatermark queues

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rrclcm_create_ul_dl_rlc_lc_q
(
  void
)
{
    /* Set up Queue for Transmitting the data to RLC on RB 0 using CCCH 
     Logical Channe. */
  (void) dsm_queue_init(&rrclcm_rb0_ccch_ul_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb0_ccch_ul_q);
  rrclcm_rb0_ccch_ul_wmq.q_ptr = &rrclcm_rb0_ccch_ul_q;
  rrclcm_rb0_ccch_ul_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb0_ccch_ul_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb0_ccch_ul_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for Transmitting the data to RLC on RB 1 using DCCH 
     Logical Channe. */
  (void) dsm_queue_init(&rrclcm_rb1_dcch_ul_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb1_dcch_ul_q);
  rrclcm_rb1_dcch_ul_wmq.q_ptr = &rrclcm_rb1_dcch_ul_q;
  rrclcm_rb1_dcch_ul_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb1_dcch_ul_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb1_dcch_ul_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for Transmitting the data to RLC on RB 2 using DCCH 
     Logical Channe. */
  (void) dsm_queue_init(&rrclcm_rb2_dcch_ul_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb2_dcch_ul_q);
  rrclcm_rb2_dcch_ul_wmq.q_ptr = &rrclcm_rb2_dcch_ul_q;
  rrclcm_rb2_dcch_ul_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb2_dcch_ul_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb2_dcch_ul_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for Transmitting the data to RLC on RB 3 using DCCH 
     Logical Channe. */
  (void) dsm_queue_init(&rrclcm_rb3_dcch_ul_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb3_dcch_ul_q);
  rrclcm_rb3_dcch_ul_wmq.q_ptr = &rrclcm_rb3_dcch_ul_q;
  rrclcm_rb3_dcch_ul_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb3_dcch_ul_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb3_dcch_ul_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for Transmitting the data to RLC on RB 4 using DCCH 
     Logical Channe. */
  (void) dsm_queue_init(&rrclcm_rb4_dcch_ul_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb4_dcch_ul_q);
  rrclcm_rb4_dcch_ul_wmq.q_ptr = &rrclcm_rb4_dcch_ul_q;
  rrclcm_rb4_dcch_ul_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb4_dcch_ul_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb4_dcch_ul_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

   /* Set up Queue for receiving the data from RLC on RB 32 using BCCH 
     Logical Channel for serving cell. */
  (void) dsm_queue_init(&rrclcm_rb32_bcch_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb32_bcch_dl_q);
  rrclcm_rb32_bcch_dl_wmq.q_ptr = &rrclcm_rb32_bcch_dl_q;
  rrclcm_rb32_bcch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb32_bcch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb32_bcch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for receiving the data from RLC on RB 33 using BCCH 
     Logical Channel for neighbor cell. */
  (void) dsm_queue_init(&rrclcm_rb33_bcch_dl_wmq, 8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb33_bcch_dl_q);
  rrclcm_rb33_bcch_dl_wmq.q_ptr = &rrclcm_rb33_bcch_dl_q;
  rrclcm_rb33_bcch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb33_bcch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb33_bcch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for receiving the data from RLC on RB 34 using BCCH 
     Logical Channel mapped to FACH transport channel. */
  (void) dsm_queue_init(&rrclcm_rb34_bcch_fach_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb34_bcch_fach_dl_q);
  rrclcm_rb34_bcch_fach_dl_wmq.q_ptr = &rrclcm_rb34_bcch_fach_dl_q;
  rrclcm_rb34_bcch_fach_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb34_bcch_fach_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb34_bcch_fach_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

   /* Set up Queue for receiving the data from RLC on RB 35 using PCCH 
     Logical Channel. */
  (void) dsm_queue_init(&rrclcm_rb35_pcch_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb35_pcch_dl_q);
  rrclcm_rb35_pcch_dl_wmq.q_ptr = &rrclcm_rb35_pcch_dl_q;
  rrclcm_rb35_pcch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb35_pcch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb35_pcch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

/* Set up Queue for receiving the data from RLC on RB 0 using CCCH 
     Logical Channel. */
  (void) dsm_queue_init(&rrclcm_rb0_ccch_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb0_ccch_dl_q);
  rrclcm_rb0_ccch_dl_wmq.q_ptr = &rrclcm_rb0_ccch_dl_q;
  rrclcm_rb0_ccch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb0_ccch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb0_ccch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for receiving the data from RLC on RB 1 using DCCH 
     Logical Channel. */
  (void) dsm_queue_init(&rrclcm_rb1_dcch_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb1_dcch_dl_q);
  rrclcm_rb1_dcch_dl_wmq.q_ptr = &rrclcm_rb1_dcch_dl_q;
  rrclcm_rb1_dcch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb1_dcch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;  
  rrclcm_rb1_dcch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for receiving the data from RLC on RB 2 using DCCH 
     Logical Channel. */
  (void) dsm_queue_init(&rrclcm_rb2_dcch_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb2_dcch_dl_q);
  rrclcm_rb2_dcch_dl_wmq.q_ptr = &rrclcm_rb2_dcch_dl_q;
  rrclcm_rb2_dcch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb2_dcch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb2_dcch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for receiving the data from RLC on RB 3 using DCCH 
     Logical Channel. */
  (void) dsm_queue_init(&rrclcm_rb3_dcch_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb3_dcch_dl_q);
  rrclcm_rb3_dcch_dl_wmq.q_ptr = &rrclcm_rb3_dcch_dl_q;
  rrclcm_rb3_dcch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb3_dcch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb3_dcch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for receiving the data from RLC on RB 4 using DCCH 
     Logical Channel. */
  (void) dsm_queue_init(&rrclcm_rb4_dcch_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_rb4_dcch_dl_q);
  rrclcm_rb4_dcch_dl_wmq.q_ptr = &rrclcm_rb4_dcch_dl_q;
  rrclcm_rb4_dcch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb4_dcch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_rb4_dcch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

  /* Set up Queue for Transmitting the data to RLC on RB 5 using DCCH 
     Logical Channel. */
  (void) dsm_queue_init(&rrclcm_srb5_dcch_dl_wmq,8 * RRCLCM_WATERMARK_SIZE,&rrclcm_srb5_dcch_dl_q);
  rrclcm_srb5_dcch_dl_wmq.q_ptr = &rrclcm_srb5_dcch_dl_q;
  rrclcm_srb5_dcch_dl_wmq.dont_exceed_cnt = 8 * RRCLCM_WATERMARK_SIZE;
  rrclcm_srb5_dcch_dl_wmq.hi_watermark = 6 * RRCLCM_WATERMARK_SIZE;
  rrclcm_srb5_dcch_dl_wmq.lo_watermark = 2 * RRCLCM_WATERMARK_SIZE;

}
/*===========================================================================

FUNCTION rrclcm_init_ul_rlc_lc_info

DESCRIPTION
  This function will initialize the Uplink RLC Logical Channel Id Info with 
  the default value. This function also maps the Watermark queues with the
  respective RLC Logical Channel IDs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_init_ul_rlc_lc_info( void)
{
  uint8 i;

  
 /* Intializes Transparent mode Uplink RLC Logical Channel Ids. */
  for( i = RRCLCM_UL_TM_START_IDX; i < UE_MAX_UL_TM_CHANNEL; i++ )
  {
    switch(i)
  {
      case (RRCLCM_UL_TM_START_IDX): 
         rrclcm_ul_rlc_lc_info[i].lc_id = i;
         rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
         rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_CCCH;
         rrclcm_ul_rlc_lc_info[i].mode = UE_MODE_TRANSPARENT;
         rrclcm_ul_rlc_lc_info[i].rb_id = CCCH_RADIO_BEARER_ID;
         rrclcm_ul_rlc_lc_info[i].tx_queue = &rrclcm_rb0_ccch_ul_wmq;
         break;
      default:
         rrclcm_ul_rlc_lc_info[i].lc_id = i;
         rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
         rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_DTCH;
         rrclcm_ul_rlc_lc_info[i].mode = UE_MODE_TRANSPARENT;
         rrclcm_ul_rlc_lc_info[i].rb_id = RRCLCM_RB_ID_NOT_ALLOCATED;
         rrclcm_ul_rlc_lc_info[i].tx_queue = NULL;
         break;
    }
  }

/* Intializes Unacknowledged mode Uplink RLC Logical Channel Ids. */

  for( i = RRCLCM_UL_UM_START_IDX; i < (RRCLCM_UL_UM_START_IDX + 
       UE_MAX_UL_UM_CHANNEL); i++ )
  {
     
    switch(i)
    {
      case (RRCLCM_UL_UM_START_IDX) : 
        rrclcm_ul_rlc_lc_info[i].lc_id = i;
        rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_DCCH;
        rrclcm_ul_rlc_lc_info[i].mode = UE_MODE_UNACKNOWLEDGED;
        rrclcm_ul_rlc_lc_info[i].rb_id = DCCH_UM_RADIO_BEARER_ID;
        rrclcm_ul_rlc_lc_info[i].tx_queue = &rrclcm_rb1_dcch_ul_wmq;
        break;
      default :
        rrclcm_ul_rlc_lc_info[i].lc_id = i;
        rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_DTCH;
        rrclcm_ul_rlc_lc_info[i].mode = UE_MODE_UNACKNOWLEDGED;
        rrclcm_ul_rlc_lc_info[i].rb_id = RRCLCM_RB_ID_NOT_ALLOCATED;
        rrclcm_ul_rlc_lc_info[i].tx_queue = NULL;
        break;
    }
  }

/* Intializes Acknowledged mode Uplink RLC Logical Channel Ids for Data. */

  for( i = RRCLCM_UL_AM_DATA_START_IDX ; 
       i < (RRCLCM_UL_AM_DATA_START_IDX + UE_MAX_UL_AM_DATA_CHANNEL) ;
       i++ )
  {
    switch(i)
    {
      case (RRCLCM_UL_AM_DATA_START_IDX) :
        rrclcm_ul_rlc_lc_info[i].lc_id = i;
        rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_DCCH;
        rrclcm_ul_rlc_lc_info[i].mode = UE_MODE_ACKNOWLEDGED_DATA;
        rrclcm_ul_rlc_lc_info[i].rb_id = DCCH_AM_RADIO_BEARER_ID;
        rrclcm_ul_rlc_lc_info[i].tx_queue = &rrclcm_rb2_dcch_ul_wmq;
        break;
      case (RRCLCM_UL_AM_DATA_START_IDX +1) :
        rrclcm_ul_rlc_lc_info[i].lc_id = i;
        rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_DCCH;
        rrclcm_ul_rlc_lc_info[i].mode = UE_MODE_ACKNOWLEDGED_DATA;
        rrclcm_ul_rlc_lc_info[i].rb_id = DCCH_DT_HIGH_PRI_RADIO_BEARER_ID;
        rrclcm_ul_rlc_lc_info[i].tx_queue = &rrclcm_rb3_dcch_ul_wmq;
        break;
      case (RRCLCM_UL_AM_DATA_START_IDX+2) :
        rrclcm_ul_rlc_lc_info[i].lc_id = i;
        rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_DCCH;
        rrclcm_ul_rlc_lc_info[i].mode = UE_MODE_ACKNOWLEDGED_DATA;
        rrclcm_ul_rlc_lc_info[i].rb_id = DCCH_DT_LOW_PRI_RADIO_BEARER_ID;
        rrclcm_ul_rlc_lc_info[i].tx_queue = &rrclcm_rb4_dcch_ul_wmq;
        break;
      default:
        rrclcm_ul_rlc_lc_info[i].lc_id = i;
        rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_DTCH;
        rrclcm_ul_rlc_lc_info[i].mode = UE_MODE_ACKNOWLEDGED_DATA;
        rrclcm_ul_rlc_lc_info[i].rb_id = RRCLCM_RB_ID_NOT_ALLOCATED;
        rrclcm_ul_rlc_lc_info[i].tx_queue = NULL;
        break;
    }
  }

} /* rrclcm_init_ul_rlc_lc_info */


/*===========================================================================

FUNCTION rrclcm_init_dl_rlc_lc_info

DESCRIPTION
  This function will initialize the Downlink RLC Logical Channel Id Info with 
  the default value. This function also maps the Watermark queues with the
  respective RLC Logical Channel IDs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_init_dl_rlc_lc_info
(
 void
)
{  
  uint8 i;

 
  /* Intializes Transparent mode Downlink RLC Logical Entities. */

  for( i = RRCLCM_DL_TM_START_IDX; i < UE_MAX_DL_TM_CHANNEL; i++ )
  {
    switch(i)
    {
      case (RRCLCM_DL_TM_START_IDX): 
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_BCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_TRANSPARENT;
        rrclcm_dl_rlc_lc_info[i].rb_id = BCCH_S_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb32_bcch_dl_wmq;
        break;
      case (RRCLCM_DL_TM_START_IDX + 1): 
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_BCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_TRANSPARENT;
        rrclcm_dl_rlc_lc_info[i].rb_id = BCCH_N_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb33_bcch_dl_wmq;
        break;
      case (RRCLCM_DL_TM_START_IDX + 2): 
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_BCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_TRANSPARENT;
        rrclcm_dl_rlc_lc_info[i].rb_id = BCCH_FACH_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb34_bcch_fach_dl_wmq;
        break;
      case (RRCLCM_DL_TM_START_IDX + 3): 
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_PCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_TRANSPARENT;
        rrclcm_dl_rlc_lc_info[i].rb_id = PCCH_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb35_pcch_dl_wmq;
        break;
      default:
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_DTCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_TRANSPARENT;
        rrclcm_dl_rlc_lc_info[i].rb_id = RRCLCM_RB_ID_NOT_ALLOCATED;
        rrclcm_dl_rlc_lc_info[i].rx_queue = NULL;
        break;
    }
  }

  /* Intializes Unacknowledged mode RLC Logical Entities. */

  for( i = RRCLCM_DL_UM_START_IDX; i < (RRCLCM_DL_UM_START_IDX + 
       UE_MAX_DL_UM_CHANNEL); i++ )
  {     
    switch(i)
    {
      case (RRCLCM_DL_UM_START_IDX) : 
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_CCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_UNACKNOWLEDGED;
        rrclcm_dl_rlc_lc_info[i].rb_id = CCCH_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb0_ccch_dl_wmq;
        break;
      case (RRCLCM_DL_UM_START_IDX + 1) : 
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_DCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_UNACKNOWLEDGED;
        rrclcm_dl_rlc_lc_info[i].rb_id = DCCH_UM_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb1_dcch_dl_wmq;
        break;

     case (RRCLCM_DL_UM_START_IDX + 2) : 
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_CTCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_UNACKNOWLEDGED;
        rrclcm_dl_rlc_lc_info[i].rb_id = CTCH_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = NULL;
        break;

      default :
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_DTCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_UNACKNOWLEDGED;
        rrclcm_dl_rlc_lc_info[i].rb_id = RRCLCM_RB_ID_NOT_ALLOCATED;
        rrclcm_dl_rlc_lc_info[i].rx_queue = NULL;
        break;
    }
  }
  /* Intializes Acknowledged mode RLC Logical Entities for Data. */

  for( i = RRCLCM_DL_AM_DATA_START_IDX; 
       i < (RRCLCM_DL_AM_DATA_START_IDX + UE_MAX_DL_AM_DATA_CHANNEL) ;
       i++ ) 
  {
    switch(i)
    {
      case (RRCLCM_DL_AM_DATA_START_IDX) :
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_DCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_ACKNOWLEDGED_DATA;
        rrclcm_dl_rlc_lc_info[i].rb_id = DCCH_AM_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb2_dcch_dl_wmq;
        break;
      case (RRCLCM_DL_AM_DATA_START_IDX + 1) :
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_DCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_ACKNOWLEDGED_DATA;
        rrclcm_dl_rlc_lc_info[i].rb_id = DCCH_DT_HIGH_PRI_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb3_dcch_dl_wmq;
        break;
      case (RRCLCM_DL_AM_DATA_START_IDX + 2) :
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_DCCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_ACKNOWLEDGED_DATA;
        rrclcm_dl_rlc_lc_info[i].rb_id = DCCH_DT_LOW_PRI_RADIO_BEARER_ID;
        rrclcm_dl_rlc_lc_info[i].rx_queue = &rrclcm_rb4_dcch_dl_wmq;
        break;
      default:
        rrclcm_dl_rlc_lc_info[i].lc_id = i;
        rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
        rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_DTCH;
        rrclcm_dl_rlc_lc_info[i].mode = UE_MODE_ACKNOWLEDGED_DATA;
        rrclcm_dl_rlc_lc_info[i].rb_id = RRCLCM_RB_ID_NOT_ALLOCATED;
        rrclcm_dl_rlc_lc_info[i].rx_queue = NULL;
    }
  }
} /* rrclcm_init_dl_rlc_lc_info() */

/*===========================================================================

FUNCTION rrclcm_clear_rlc_lc_info

DESCRIPTION
  This function clears and initializes the RLC Logical Channel Id
  info for both Uplink and Downlink.
  DSM items in all watermark queues are freed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_clear_rlc_lc_info
( 
 void
)
{
  rlc_lc_id_type ul_lc_id;
  rlc_lc_id_type dl_lc_id;
  dsm_item_type *dsm_item;                  /* Local variable for DSM Item */
  dsm_watermark_type *ul_wm_ptr;            /* uplink Water Mark pointer */
  dsm_watermark_type *dl_wm_ptr;            /* downlink Water Mark pointer */
  for(ul_lc_id = 0; ul_lc_id < UE_MAX_UL_LOGICAL_CHANNEL; ul_lc_id ++)
  {
    /* Dequeue all DSM items in the Uplink watermark queue */
    ul_wm_ptr = rrclcm_get_ul_watermark_ptr(ul_lc_id);

    if(ul_wm_ptr != NULL)
    {
      /* Flush out DSM items in the watermark queue */
      while((dsm_item = dsm_dequeue(ul_wm_ptr)) != NULL)
      {
        /* Free all packets in the chain */
        dsm_free_packet(&dsm_item);
      }
    }
  } /* for(UE_MAX_UL_LOGICAL_CHANNEL) */

  for(dl_lc_id = 0; dl_lc_id < UE_MAX_DL_LOGICAL_CHANNEL; dl_lc_id ++)
  {
    {
      /* Dequeue all DSM items in the Downlink watermark queue */
      dl_wm_ptr = rrclcm_get_dl_watermark_ptr(dl_lc_id);

      if(dl_wm_ptr != NULL)
      {
        /* Flush out DSM items in the watermark queue */
        while((dsm_item = dsm_dequeue(dl_wm_ptr)) != NULL)
        {
          /* Free all packets in the chain */
          dsm_free_packet(&dsm_item);
        }
      }
    }
  } /* for(UE_MAX_DL_LOGICAL_CHANNEL) */

  /* Initialize the UL RLC Logical channel info */
  rrclcm_init_ul_rlc_lc_info();
  /* Initialize the DL RLC Logical channel info */
  rrclcm_init_dl_rlc_lc_info();
  /* Initialize the L2 Ack information */
  rrclcm_init_l2_ack_info();
} /* rrclcm_clear_rlc_lc_info */

/*===========================================================================

FUNCTION rrclcm_allocate_ul_rlc_lc_id

DESCRIPTION
  This function will allocate Uplink RLC Logical Channel Id for the 
  requested RLC service entity. The RRC LLC module uses this function
  to allocate uplink RLC Logical Channel Id.

DEPENDENCIES
  None

RETURN VALUE
  The allocated RLC Logical Channel Id. If failed, it'll return a value 
  RRCLCM_RLC_LC_ID_ERROR.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_allocate_ul_rlc_lc_id
(    
  uecomdef_logchan_e_type ul_lc_type,      /* Uplink Logical Channel Type. */
  rrc_RB_Identity rb_id,                   /* Radio Bearer Id assoicated with
                                              the above Uplink Logical Channel
                                              Type. */
  uecomdef_logch_mode_e_type mode,         /* Mode of transmission for the RLC 
                                              service entity. */
  dsm_watermark_type **dsm_tx_queue        /* Pointer to the DSM Water Mark 
                                              associated with Uplink RLC Logical
                                              Channel Id. For Radio Bearers other
                                              than 0,1,2,3and 4 this will be 
                                              set to NULL pointer. */
)
{
  int i=0;
  boolean ul_lc_id_allocated = FALSE;

  switch(mode) 
  {
  /* Checks for Logical Channel Type and Radio Bearer Id match for Transparent
     mode. */
    case (UE_MODE_TRANSPARENT): 
      for( i = RRCLCM_UL_TM_START_IDX ; i < UE_MAX_UL_TM_CHANNEL ; i++ )
      {
        if(ul_lc_type == rrclcm_ul_rlc_lc_info[i].lc_type &&              
           rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_NOT_IN_USE)
        {
          if( rrclcm_ul_rlc_lc_info[i].rb_id == rb_id ||
              rrclcm_ul_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED )
              
          {
            if(rrclcm_ul_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED )
            {
              rrclcm_ul_rlc_lc_info[i].rb_id = rb_id;
            }
            *dsm_tx_queue = rrclcm_ul_rlc_lc_info[i].tx_queue;
                                           /* Uplink DSM Watermark queue is 
                                              assigned */
            rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_IN_USE;
            ul_lc_id_allocated = TRUE;       /* RLC Logical Channel Id is 
                                                allocated. */
            break;                           /* Comes out of for loop */
          }
           
        }
      }
      break;
 /* Checks for Logical Channel Type and Radio Bearer Id match for 
    Unacknowledged mode. */
    case (UE_MODE_UNACKNOWLEDGED): 
      for( i = RRCLCM_UL_UM_START_IDX ; i < (RRCLCM_UL_UM_START_IDX +
               UE_MAX_UL_UM_CHANNEL) ; i++ )
      {
        if( ul_lc_type == rrclcm_ul_rlc_lc_info[i].lc_type && 
            rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_NOT_IN_USE)
        {
          if( rrclcm_ul_rlc_lc_info[i].rb_id == rb_id ||
              rrclcm_ul_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
          {
            if( rrclcm_ul_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED )
            {  
              rrclcm_ul_rlc_lc_info[i].rb_id = rb_id;
            }
            *dsm_tx_queue = rrclcm_ul_rlc_lc_info[i].tx_queue;
                                             /* Uplink DSM Watermark queue is 
                                                 assigned */
            rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_IN_USE;
            ul_lc_id_allocated = TRUE;       /* RLC Logical Channel Id is 
                                                allocated. */
            break;                           /* Comes out of for loop */
          }

        }
      }
      break;  
 /* Checks for Logical Channel Type and Radio Bearer Id match for 
    Acknowledged Data mode. */
    case (UE_MODE_ACKNOWLEDGED_DATA): 
      for( i = (RRCLCM_UL_AM_DATA_START_IDX); 
           i < (RRCLCM_UL_AM_DATA_START_IDX + UE_MAX_UL_AM_DATA_CHANNEL) ;
           i++ )
      {
        if( ul_lc_type == rrclcm_ul_rlc_lc_info[i].lc_type && 
            rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_NOT_IN_USE)
        {
          if( rrclcm_ul_rlc_lc_info[i].rb_id == rb_id ||
              rrclcm_ul_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
          {
            if(rrclcm_ul_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED )
            {
              rrclcm_ul_rlc_lc_info[i].rb_id = rb_id;
            }
            *dsm_tx_queue = rrclcm_ul_rlc_lc_info[i].tx_queue;
                                             /* Uplink DSM Watermark queue is 
                                                assigned. */
            rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_IN_USE;
            ul_lc_id_allocated = TRUE;       /* RLC Logical Channel Id is 
                                                allocated. */
            break;                           /* Comes out of for loop */
          }
        }
      }
      break;

    default:
      WRRC_MSG0_ERROR("This should never happen !!");
      break;
  }
  if(ul_lc_id_allocated)
  {
    WRRC_MSG3_MED("Allocated UL lc_id: %d, rb_id: %d, lc_type:%d",
             rrclcm_ul_rlc_lc_info[i].lc_id,
             rrclcm_ul_rlc_lc_info[i].rb_id,
             rrclcm_ul_rlc_lc_info[i].lc_type);
    return (rrclcm_ul_rlc_lc_info[i].lc_id);
  }
  else
  {
    MSG_HIGH("Failed to allocate UL rb_id:%d, lc_type:%d, mode:%d",rb_id,ul_lc_type,mode);
    return (RRCLCM_RLC_LC_ID_ERROR);       /* RLC Logical Channel Id allocation
                                              failed. */
  }
}/*  rrclcm_allocate_ul_rlc_lc_id */                                     

/*===========================================================================

FUNCTION rrclcm_allocate_dl_rlc_lc_id

DESCRIPTION
  This function will allocate Downlink RLC Logical Channel Id for the 
  requested RLC service entity. The RRC LLC module uses this function
  to allocate Downlink RLC Logical Channel Id.

DEPENDENCIES
  None

RETURN VALUE
  The allocated RLC Logical Channel Id. If failed, it'll return a value 
  RRCLCM_RLC_LC_ID_ERROR.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_allocate_dl_rlc_lc_id
(    
  uecomdef_logchan_e_type dl_lc_type,      /* Downlink Logical Channel
                                              Type. */
  rrc_RB_Identity rb_id,                   /* Radio Bearer Id assoicated with
                                              the above Downlink Logical 
                                              Channel Type. */
  uecomdef_logch_mode_e_type mode,         /* Mode of transmission for the RLC 
                                              service entity. */
  dsm_watermark_type **dsm_rx_queue         /* Pointer to the DSM Water Mark 
                                              associated with Downlink RLC 
                                              Logical Channel Id. For Radio
                                              Bearers other than 0,1,2,3,4,32
                                              33 and 34,this will be set to 
                                              NULL pointer. */
)
{
  int i=0;
  boolean dl_lc_id_allocated = FALSE;

  switch (mode) 
  {
  /* Checks for Logical Channel Type and Radio Bearer Id match for Transparent
     mode. */
    case (UE_MODE_TRANSPARENT): 
      for( i = RRCLCM_DL_TM_START_IDX ; i < UE_MAX_DL_TM_CHANNEL ; i++ )
      {
        if(dl_lc_type == UE_LOGCHAN_TM_DCCH_DL && 
          rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_NOT_IN_USE)
        {
          if( rrclcm_dl_rlc_lc_info[i].rb_id == rb_id ||
              rrclcm_dl_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
          {
            if(rrclcm_dl_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
            {
              rrclcm_dl_rlc_lc_info[i].rb_id = rb_id;
            }
            
            rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_TM_DCCH_DL;

            WRRC_MSG0_HIGH("Allocating LC Id for SRB#5");

            /* Initially would be NULL;
               Is updated right before registering DL queue
               with RLC */
            *dsm_rx_queue = rrclcm_dl_rlc_lc_info[i].rx_queue;
                                             /* Downlink DSM Watermark queue is 
                                                assigned */
            rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_IN_USE;
            dl_lc_id_allocated = TRUE;       /* RLC Logical Channel Id is 
                                                allocated. */
            break;                           /* Comes out of for loop */
          }
        }
        else
        if( dl_lc_type == rrclcm_dl_rlc_lc_info[i].lc_type && 
            rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_NOT_IN_USE)
        {
          
          if( rrclcm_dl_rlc_lc_info[i].rb_id == rb_id ||
              rrclcm_dl_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
          {
            if(rrclcm_dl_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
            {
              rrclcm_dl_rlc_lc_info[i].rb_id = rb_id;
            }
            *dsm_rx_queue = rrclcm_dl_rlc_lc_info[i].rx_queue;
                                             /* Downlink DSM Watermark queue is 
                                                assigned */
            rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_IN_USE;
            dl_lc_id_allocated = TRUE;       /* RLC Logical Channel Id is 
                                                allocated. */
            break;                           /* Comes out of for loop */
          }
        }
      }
      break;
  /* Checks for Logical Channel Type and Radio Bearer Id match for 
     Unacknowledged mode. */
    case (UE_MODE_UNACKNOWLEDGED): 
      for( i = RRCLCM_DL_UM_START_IDX ; i < (RRCLCM_DL_UM_START_IDX +
               UE_MAX_DL_UM_CHANNEL) ; i++ )
      {
        if( dl_lc_type == rrclcm_dl_rlc_lc_info[i].lc_type &&
            rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_NOT_IN_USE)
        {
          if( rrclcm_dl_rlc_lc_info[i].rb_id == rb_id ||
              rrclcm_dl_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
          {
            if(rrclcm_dl_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
            {
              rrclcm_dl_rlc_lc_info[i].rb_id = rb_id;
            }
            *dsm_rx_queue = rrclcm_dl_rlc_lc_info[i].rx_queue;
                                             /* Downlink DSM Watermark queue is 
                                                assigned */
            rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_IN_USE;
            dl_lc_id_allocated = TRUE;       /* RLC Logical Channel Id is 
                                                allocated. */
            break;                           /* comes out of for loop */
          }
        }
      }
      break;
 /* Checks for Logical Channel Type and Radio Bearer Id match for 
    Acknowledged Data mode. */  
    case (UE_MODE_ACKNOWLEDGED_DATA): 
      for( i = (RRCLCM_DL_AM_DATA_START_IDX); 
           i < (RRCLCM_DL_AM_DATA_START_IDX + UE_MAX_DL_AM_DATA_CHANNEL) ;
           i++ )
      {
        if( dl_lc_type == rrclcm_dl_rlc_lc_info[i].lc_type &&
            rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_NOT_IN_USE)
        {
          
          if( rrclcm_dl_rlc_lc_info[i].rb_id == rb_id ||
              rrclcm_dl_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
          {
            if(rrclcm_dl_rlc_lc_info[i].rb_id == RRCLCM_RB_ID_NOT_ALLOCATED)
            {
              rrclcm_dl_rlc_lc_info[i].rb_id = rb_id;
            }
            *dsm_rx_queue = rrclcm_dl_rlc_lc_info[i].rx_queue;
                                             /* Downlink DSM Watermark queue is 
                                                assigned */
            rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_IN_USE;
            dl_lc_id_allocated = TRUE;       /* RLC Logical Channel Id is 
                                                allocated. */
            break;                           /* Comes out of for loop */
          }
        }
      }
      break;

    default:
      WRRC_MSG0_ERROR("This should never happen !!");
      break;
  }
  if(dl_lc_id_allocated)
  {
    WRRC_MSG3_MED("Allocated DL lc_id: %d, rb_id: %d, lc_type:%d",
             rrclcm_dl_rlc_lc_info[i].lc_id,
             rrclcm_dl_rlc_lc_info[i].rb_id,
             rrclcm_dl_rlc_lc_info[i].lc_type);
    return (rrclcm_dl_rlc_lc_info[i].lc_id);
  }
  else
  {
    MSG_HIGH("Failed to allocate DL rb_id:%d, lc_type:%d, mode:%d", rb_id, dl_lc_type, mode);
    return (RRCLCM_RLC_LC_ID_ERROR);       /* RLC Logical Channel Id allocation
                                              failed. */
  }
} /* rrclcm_allocate_dl_rlc_lc_id */

/*===========================================================================

FUNCTION rrclcm_check_ul_rlc_lc_id

DESCRIPTION
  This function will check whether Uplink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. Any RRC procedure can use this
  function to check whether a particular Logical channel entity is already 
  setup or not. If this function returns RRCLCM_RLC_LC_ID_NOT_FOUND, then the
  RRC procedure will request RRC LLC module to setup Logical Channel entity
  and the corresponding physical channel for transmitting SDU on the uplink.

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If it is not alreday allocated,
   it'll return RRCLCM_RLC_LC_ID_NOT_FOUND.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_check_ul_rlc_lc_id
(    
  uecomdef_logchan_e_type ul_lc_type,       /* Uplink Logical Channel Type. */
  rrc_RB_Identity rb_id,                    /* Radio Bearer Id assoicated with
                                              the above Downlink Logical 
                                              Channel type. */
  uecomdef_logch_mode_e_type mode           /* Mode of transmission for the RLC 
                                              service entity. */
 
)
{
  int i=0;
  boolean ul_lc_id_found = FALSE;

  switch (mode) 
  {
  /* Checks for Logical Channel Type and Radio Bearer Id match for Transparent
     mode. */
    case (UE_MODE_TRANSPARENT): 
      for( i = RRCLCM_UL_TM_START_IDX ; i < UE_MAX_UL_TM_CHANNEL ; i++ )
      {
        if( ul_lc_type == rrclcm_ul_rlc_lc_info[i].lc_type && 
             rb_id == rrclcm_ul_rlc_lc_info[i].rb_id &&
             rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
        {
          ul_lc_id_found = TRUE;           /* Match for the requested service 
                                              is found */
          break;
        }
      }
      break;
  /* Checks for Logical Channel Type and Radio Bearer Id match for 
     Unacknowledged mode. */
    case (UE_MODE_UNACKNOWLEDGED): 
      for( i = RRCLCM_UL_UM_START_IDX ; i < (RRCLCM_UL_UM_START_IDX +
               UE_MAX_UL_UM_CHANNEL) ; i++ )
      {
        if( ul_lc_type == rrclcm_ul_rlc_lc_info[i].lc_type && 
             rb_id == rrclcm_ul_rlc_lc_info[i].rb_id &&
             rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
        {
          ul_lc_id_found = TRUE;           /* Match for the requested service 
                                              is found */
          break;
        }
      }
      break;  
  /* Checks for Logical Channel Type and Radio Bearer Id match for 
     Acknowledged Data mode. */
    case (UE_MODE_ACKNOWLEDGED_DATA): 
      for( i = (RRCLCM_UL_AM_DATA_START_IDX); 
           i < (RRCLCM_UL_AM_DATA_START_IDX + UE_MAX_UL_AM_DATA_CHANNEL) ;
           i++ )
      {
        if( ul_lc_type == rrclcm_ul_rlc_lc_info[i].lc_type && 
             rb_id == rrclcm_ul_rlc_lc_info[i].rb_id &&
             rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
        {
          ul_lc_id_found = TRUE;           /* Match for the requested service 
                                              is found */
          break;
        }
      }
      break;              
    default:
      WRRC_MSG0_ERROR("This should never happen !!");
      break;
  }
  if(ul_lc_id_found)
  {
    return (rrclcm_ul_rlc_lc_info[i].lc_id);
  }
  else
  {
    return (RRCLCM_RLC_LC_ID_NOT_FOUND);   /* Match for the requested service 
                                              is not found. The RRC LLC needs 
                                              to request for allocation. */
  }
} /* rrclcm_check_ul_rlc_lc_id */

/*===========================================================================

FUNCTION rrclcm_get_ul_rlc_lc_id

DESCRIPTION
  This function will check whether Uplink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. Any RRC procedure can use this
  function to check whether a particular Logical channel entity is already 
  setup or not.

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If it is not alreday allocated,
   it'll return RRCLCM_RLC_LC_ID_NOT_FOUND.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_get_ul_rlc_lc_id
(    
  rrc_RB_Identity rb_id                     /* Radio Bearer Id assoicated with
                                               the above Downlink Logical 
                                               Channel type. */ 
)
{
  int i=0;
  
  /* Checks for Logical Channel Type and Radio Bearer Id match for all modes. */
  for( i = 0 ; i < (UE_MAX_UL_TM_CHANNEL + UE_MAX_UL_UM_CHANNEL +
       UE_MAX_UL_AM_DATA_CHANNEL); i++ )
  {
    if( rb_id == rrclcm_ul_rlc_lc_info[i].rb_id &&
        rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
    {
      /* Return the lc id */
      return (rrclcm_ul_rlc_lc_info[i].lc_id);
    }
  }

  /* If we have reached this point, we have not found the LC. Return Not Found */
  return (RRCLCM_RLC_LC_ID_NOT_FOUND);   

} /* rrclcm_get_ul_rlc_lc_id */

/*===========================================================================

FUNCTION rrclcm_get_dl_rlc_lc_id

DESCRIPTION
  This function will check whether Downlink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. RRC LLC can use this
  function to check whether a particular Logical channel entity is already 
  setup or not. 

DEPENDENCIES
  None

RETURN VALUE
   The allocated RLC Logical Channel Id. If it is not alreday allocated,
   it'll return RRCLCM_RLC_LC_ID_NOT_FOUND.

SIDE EFFECTS
  None
===========================================================================*/

rlc_lc_id_type rrclcm_get_dl_rlc_lc_id
(    
  rrc_RB_Identity rb_id                     /* Radio Bearer Id assoicated with
                                               the above Downlink Logical 
                                               Channel type. */
 
)
{
  int i=0;

  /* Checks for Logical Channel Type and Radio Bearer Id match for all modes. */
  for( i = 0 ; i < (UE_MAX_DL_TM_CHANNEL + UE_MAX_DL_UM_CHANNEL +
       UE_MAX_DL_AM_DATA_CHANNEL); i++ )
  {
    if( rb_id == rrclcm_dl_rlc_lc_info[i].rb_id &&
        rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
    {
      /* Return the lc id */
      return (rrclcm_dl_rlc_lc_info[i].lc_id);  
    }
  }

  /* If we have reached this point, we have not found the LC. Return Not Found */
  return (RRCLCM_RLC_LC_ID_NOT_FOUND);   

} /* rrclcm_get_dl_rlc_lc_id */
/*===========================================================================

FUNCTION rrclcm_deallocate_ul_rlc_lc_id

DESCRIPTION
  This function will deallocate Uplink RLC Logical Channel Id requested. It also 
  flushes out DSM items in the watermark queue before deallocating RLC Logical
  Channel Id.

DEPENDENCIES
  None

RETURN VALUE
   TRUE if deallocation is successful. Otherwise it returns FALSE.

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_deallocate_ul_rlc_lc_id
(    
  rlc_lc_id_type ul_lc_id               /* Uplink  RLC Logical Channel ID */
)
{

  int i=0;
  dsm_item_type *dsm_item;                  /* Local variable for DSM Item */
  dsm_watermark_type *ul_wm_ptr;            /* uplink Water Mark pointer */

  rrclcm_status_e_type rlc_lc_id_deallocated = RRCLCM_FAILURE;

  for( i = 0; i < UE_MAX_UL_LOGICAL_CHANNEL ; i++ )
  {
  /* Checks for RLC Logical Channel Id match for Uplink RLC Logiacal Channel 
     entities. */
    if( ul_lc_id == rrclcm_ul_rlc_lc_info[i].lc_id &&
        rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
    { 
      MSG_HIGH("Deallocating UL lc_id: %d, rb_id: %d, lc_type:%d", 
               rrclcm_ul_rlc_lc_info[i].lc_id,
               rrclcm_ul_rlc_lc_info[i].rb_id,
               rrclcm_ul_rlc_lc_info[i].lc_type);
      ul_wm_ptr = rrclcm_get_ul_watermark_ptr(ul_lc_id);
      if(ul_wm_ptr != NULL)
      {
        /* Flush out DSM items in the watermark queue */
        while((dsm_item = dsm_dequeue(ul_wm_ptr)) != NULL)
        {
          /* Free all packets in the chain */
          dsm_free_packet(&dsm_item);
        }
      }
      switch(rrclcm_get_ul_rb_id(ul_lc_id))
      {
        case CCCH_RADIO_BEARER_ID:
        case DCCH_UM_RADIO_BEARER_ID:
        case DCCH_AM_RADIO_BEARER_ID:
        case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
        case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
          rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
          rlc_lc_id_deallocated = RRCLCM_SUCCESS;           
          break;                            /* Match found and RLC Logical 
                                               Channel id is deallocated. */
        default:
          rrclcm_ul_rlc_lc_info[i].rb_id = RRCLCM_RB_ID_NOT_ALLOCATED;
          rrclcm_ul_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
          rrclcm_ul_rlc_lc_info[i].lc_type = UE_LOGCHAN_DTCH;
          rlc_lc_id_deallocated = RRCLCM_SUCCESS;
          break;
       }
    }
  }
  if (rlc_lc_id_deallocated == RRCLCM_FAILURE)
  {
    WRRC_MSG1_HIGH("Failed to deallocated lc_id_type:%d ",ul_lc_id);
  }
  return(rlc_lc_id_deallocated);

} /* rrclcm_deallocate_ul_rlc_lc_id */

/*===========================================================================

FUNCTION rrclcm_deallocate_dl_rlc_lc_id

DESCRIPTION
  This function will deallocate Downlink RLC Logical Channel Id requested. It 
  also flushes out DSM items in the watermark queue before deallocating RLC 
  Logical Channel Id.

DEPENDENCIES
  None

RETURN VALUE
   TRUE if deallocation is successful. Otherwise it returns FALSE.

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_deallocate_dl_rlc_lc_id
(    
  rlc_lc_id_type dl_lc_id               /* Downlink  RLC Logical Channel ID */
)
{
  int i=0;
  dsm_item_type *dsm_item;                  /* Local variable for DSM Item */
  dsm_watermark_type *dl_wm_ptr;            /* Downlink Water Mark pointer */

  rrclcm_status_e_type rlc_lc_id_deallocated = RRCLCM_FAILURE;

  for( i = 0; i < UE_MAX_DL_LOGICAL_CHANNEL ; i++ )
  {
  /* Checks for RLC Logical Channel Id match for Downlink RLC Logiacal Channel 
     entities. */
    if(dl_lc_id == rrclcm_dl_rlc_lc_info[i].lc_id &&
        rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
    {
      MSG_HIGH("Deallocating DL lc_id: %d, rb_id: %d, lc_type: %d", 
               rrclcm_dl_rlc_lc_info[i].lc_id,
               rrclcm_dl_rlc_lc_info[i].rb_id,
               rrclcm_dl_rlc_lc_info[i].lc_type);
      dl_wm_ptr = rrclcm_get_dl_watermark_ptr(dl_lc_id);
      if(dl_wm_ptr != NULL)
      {
        /* Flush out DSM items in the watermark queue */
        while((dsm_item = dsm_dequeue(dl_wm_ptr)) != NULL)
        {
          /* Free all packets in the chain */
          dsm_free_packet(&dsm_item);
        }
      }
      switch(rrclcm_get_dl_rb_id(dl_lc_id))
      {
        case CCCH_RADIO_BEARER_ID:
        case DCCH_UM_RADIO_BEARER_ID:
        case DCCH_AM_RADIO_BEARER_ID:
        case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
        case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
        case BCCH_S_RADIO_BEARER_ID:
        case BCCH_N_RADIO_BEARER_ID:
        case BCCH_FACH_RADIO_BEARER_ID:
        case PCCH_RADIO_BEARER_ID:
        case CTCH_RADIO_BEARER_ID:
          rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
          rlc_lc_id_deallocated = RRCLCM_SUCCESS;
                                            /* Match found and RLC Logical 
                                               Channel id is deallocated. */
          break;

        default:
          rrclcm_dl_rlc_lc_info[i].rb_id = RRCLCM_RB_ID_NOT_ALLOCATED;
          rrclcm_dl_rlc_lc_info[i].status = RRCLCM_LC_NOT_IN_USE;
          rrclcm_dl_rlc_lc_info[i].lc_type = UE_LOGCHAN_DTCH;
          rlc_lc_id_deallocated = RRCLCM_SUCCESS;
          break;
      }    
    }
  }
  if (rlc_lc_id_deallocated == RRCLCM_FAILURE)
  {
    WRRC_MSG1_HIGH("Failed to deallocated lc_id_type:%d ",dl_lc_id);
  }
  return (rlc_lc_id_deallocated); 
} /* rrclcm_deallocate_dl_rlc_lc_id */

/*===========================================================================

FUNCTION rrclcm_get_dl_lc_type

DESCRIPTION
  This function will identify the Logical Channel type to which the OTA message
  belongs. The RRC procedure uses this infomation for processing the received 
  message.

DEPENDENCIES
  None

RETURN VALUE
   RRCLCM_SUCCESS if match was found. Otherwise returns RRCLCM_FAILURE.


SIDE EFFECTS
  This function should normally never return RRCLCM_FAILURE. 
  If it returns RRCLCM_FAILURE, then there is a race condition between Logical
  Channel Manager and RRC LLC. 
===========================================================================*/

rrclcm_status_e_type rrclcm_get_dl_lc_type
(    
  rlc_lc_id_type dl_lc_id,                    /* The Downlink RLC Logical
                                                 Channel Id. */
  uecomdef_logchan_e_type *dl_lc_type_ptr     /* Pointer to the Logical Channel
                                                 type assocated with the above 
                                                 Id. */
)

{
  int i=0;
  rrclcm_status_e_type match_found = RRCLCM_FAILURE;

  for( i = 0; i < UE_MAX_DL_LOGICAL_CHANNEL ; i++ )
  {
  /* Checks for mapping between RLC Logical Channel id and RLC Logical Channel
     Type. */
    if(dl_lc_id == rrclcm_dl_rlc_lc_info[i].lc_id )
    {
      *dl_lc_type_ptr = rrclcm_dl_rlc_lc_info[i].lc_type;
      match_found = RRCLCM_SUCCESS;       /* The corresponding RLC logical
                                              channel type is identified. */
      break;                                        
    }
  }
  return(match_found);
} /* rrclcm_get_dl_lc_type */
/*===========================================================================

FUNCTION rrclcm_get_ul_lc_type

DESCRIPTION
  This function will identify the Logical Channel to which the OTA message
  belongs. The RRC procedure uses this infomation for sending the OTA 
  message.

DEPENDENCIES
  None

RETURN VALUE
   RRCLCM_SUCCESS if match was found. Otherwise returns RRCLCM_FAILURE.

SIDE EFFECTS
  This function should normally never return RRCLCM_FAILURE. 
  If it returns RRCLCM_FAILURE, then there is a race condition between Logical
  Channel Manager and RRC LLC. 
===========================================================================*/

rrclcm_status_e_type rrclcm_get_ul_lc_type
(    
  rlc_lc_id_type ul_lc_id,                    /* The uplink RLC Logical
                                                 Channel Id. */
  uecomdef_logchan_e_type *ul_lc_type_ptr     /* Pointer to the Logical Channel
                                                 type assocated with the above 
                                                 Id. */
)
{
  int i=0;
  rrclcm_status_e_type match_found = RRCLCM_FAILURE;

  for( i = 0; i < UE_MAX_UL_LOGICAL_CHANNEL ; i++ )
  {
  /* Checks for mapping between RLC Logical Channel id and RLC Logical Channel
     Type. */
    if(ul_lc_id == rrclcm_ul_rlc_lc_info[i].lc_id )
    {
      *ul_lc_type_ptr = rrclcm_ul_rlc_lc_info[i].lc_type;
      match_found = RRCLCM_SUCCESS;       /* The corresponding RLC logical
                                              channel type is identified. */
      break;
                                              
    }
  }
  return(match_found);
} /* rrclcm_get_ul_lc_type */   
/*===========================================================================

FUNCTION rrclcm_identify_proc

DESCRIPTION
  This function will processes the event from the Dispatcher and identifies 
  the procedure to which the L2 ACK belongs.

DEPENDENCIES
  None

RETURN VALUE
  RRCLCM_SUCCESS if there is a match for MUI received. Other it returns
  RRCLCM_FAILURE

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_identify_proc
(    
  rrc_cmd_type *cmd_ptr,                    /* RRC Command Type */
  rrc_RB_Identity rb_id,                    /* RB Id for which MUI need to
                                               be allocated. */  
  rrc_proc_e_type *proc_name                /* RRC procedure    */
)
{
  uint8 i=0;
  rrclcm_status_e_type match_found = RRCLCM_FAILURE;
  uint8 rb_id_idx = 0;                           /* Local variable for RB Id 
                                              Index. */
  rb_id_idx = rb_id - 2;

  if(cmd_ptr->cmd_hdr.cmd_id == RRC_DOWNLINK_L2ACK_IND)
  {
    MSG_HIGH("Rcvd MUI: %d for RB id: %d status %d [0:S, 1:F]",cmd_ptr->cmd.downlink_l2ack_ind.mui,
             rb_id, cmd_ptr->cmd.downlink_l2ack_ind.status);
 
    for(i = 0; i <  (uint8)MAX_NUM_OF_PROCEDURES ; i++)
    {
      if(rb_id_idx < (uint8)(UE_MAX_UL_AM_DATA_CHANNEL - 1))
      {    
        /* Searches for MUI match in the Look Up table*/
        if(rrclcm_l2_ack_info[i][rb_id_idx].mui == 
          cmd_ptr->cmd.downlink_l2ack_ind.mui)
        {
          *proc_name = rrclcm_l2_ack_info[i][rb_id_idx].rrc_proc;
          rrclcm_l2_ack_info[i][rb_id_idx].rrc_proc = RRC_PROCEDURE_NONE;
                                            /* Deallocates MUI in the LUT */
          match_found = RRCLCM_SUCCESS;
          break;                              /* exit from for loop */
        }    
      }
    }
  }                                      
  else                                  /* Invalid Command Identifier*/
  {
    *proc_name = RRC_PROCEDURE_NONE; 
     WRRC_MSG1_ERROR("Invalid Command Id : %d", cmd_ptr->cmd_hdr.cmd_id);     
  }
  return (match_found);
} /* rrclcm_identify_proc */

/*===========================================================================

FUNCTION rrclcm_get_mui

DESCRIPTION
  This function builds the MUI for the RRC procedure and RRC procedure can 
  include the same in the SDU if it is transmitting SDU in Acknowledged mode.

DEPENDENCIES
  None

RETURN VALUE
  RRCLCM_SUCCESS if allocation of MUI is successful. Other it returns
  RRCLCM_FAILURE

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_get_mui
(    
  rrc_proc_e_type rrc_proc,                /* The RRC Procedure type for 
                                              which MUI required. */
  rrc_RB_Identity rb_id,                   /* RB Id for which MUI need to
                                              be allocated. */ 
  boolean l2ack_status,                    /* Indicates whether RRC needs
                                               L2 ACK or not */
  rrclcm_mui_type *mui                     /* The MUI allocated for the RRC
                                              procedure. */ 
)
{
  static rrclcm_mui_type rrclcm_current_mui[UE_MAX_UL_AM_DATA_CHANNEL - 1];
  uint8 i=0;                                   /* Local varible for count */

  uint8 rb_id_idx = 0;                           /* Local variable for RB Id 
                                              Index. */
  rrclcm_status_e_type temp_status = RRCLCM_FAILURE;

  rb_id_idx = rb_id - 2;
  
  // Initialize only once at powerup.
  if(rrclcm_init_current_mui_array_at_poweup == TRUE)
  { 
    for(i=0; i < UE_MAX_UL_AM_DATA_CHANNEL-1; i++)
    {
      rrclcm_current_mui[i] = 0;
    }
    rrclcm_init_current_mui_array_at_poweup = FALSE;
  }

  if(rb_id_idx < (UE_MAX_UL_AM_DATA_CHANNEL - 1))
  {
    rrclcm_current_mui[rb_id_idx] = rrclcm_current_mui[rb_id_idx] + 1; 
                                          /* Increment the MUI */

    if(l2ack_status == TRUE)
    {
      for(i = 0; i <  (uint8)MAX_NUM_OF_PROCEDURES ; i++)
      {
        /* Checks for the available index that is not allocated for any
           procedure */
        if(rrclcm_l2_ack_info[i][rb_id_idx].rrc_proc == RRC_PROCEDURE_NONE)
        {
          rrclcm_l2_ack_info[i][rb_id_idx].mui = rrclcm_current_mui[rb_id_idx]; 
    
          *mui = rrclcm_current_mui[rb_id_idx];    /* Assigns the MUI to procedure.*/
    
          rrclcm_l2_ack_info[i][rb_id_idx].rrc_proc = rrc_proc;
                                              /* Updates the LUT with the Procedure
                                                 type. */
          temp_status = RRCLCM_SUCCESS;
          break;                              /* exit from for loop */
        }
      }
    }
    else
    {
      *mui = rrclcm_current_mui[rb_id_idx];    /* Assigns the MUI to procedure.*/
      temp_status = RRCLCM_SUCCESS;
    }
#ifdef FEATURE_DUAL_SIM
    if ((rrc_proc != RRC_PROCEDURE_MCMR) && (rrc_proc != RRC_PROCEDURE_NONE) &&
        ((rb_id == DCCH_AM_RADIO_BEARER_ID) || (rb_id == DCCH_DT_HIGH_PRI_RADIO_BEARER_ID)))
    {
      rrc_set_l2_ack_req_mask(rrc_proc, *mui);
    }
#endif
  }
  return (temp_status);  
} /* rrclcm_get_mui */

/*===========================================================================

FUNCTION rrclcm_init_l2_ack_info

DESCRIPTION
  This function will initailize the L2 ACK info of all RRC procedures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_init_l2_ack_info
(    
  void                  
)
{
  uint8 i,j;
  for(j = 0; j < (UE_MAX_UL_AM_DATA_CHANNEL - 1); j++)
  {
    for(i = 0; i <  (uint8)MAX_NUM_OF_PROCEDURES ; i++)
    {

      rrclcm_l2_ack_info[i][j].rrc_proc = RRC_PROCEDURE_NONE;
                                             /* Sets all RRC procedures to 
                                                NONE. */
      rrclcm_l2_ack_info[i][j].mui = 0;      /* Set the Initial MUIs to 0 */    
    }
  }
} /* rrclcm_init_l2_ack_info */

/*===========================================================================

 FUNCTION rrclcm_post_event_for_dl_sdu

DESCRIPTION
  This function will post an internal event to RRC for the downlink SDU.

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None
===========================================================================*/
void rrclcm_post_event_for_dl_sdu
(    
  rlc_lc_id_type dl_lc_id,                    /* The Downlink RLC Logical
                                                 Channel Id. */ 
  uint8 no_of_sdus,                            /* No of SDUs that are
                                                 queued */                   
 void *context

)
{
  uint32 dl_sdu_type;                          /* To store SDU type received*/

  rrc_cmd_type *cmd_ptr;                      /* Pointer to RRC Command */

  rrc_RB_Identity dl_rb_id;                   /* To store the channel on which
                                                 message was received. */
  uint8 count;                                /* Count to keep track of SDUs */

  rrclcm_status_e_type    lcm_status = RRCLCM_FAILURE;
  uecomdef_logchan_e_type dl_spl_sdu_type = UE_LOGCHAN_NONE;

  count = no_of_sdus;

  if(context != NULL)
  {
    MSG_LOW("0x%x", context, 0, 0);
  }

  while( count > 0)
  {
    /* Reset the command pointer to NULL */
    cmd_ptr = NULL;

    /* Get a command buffer from RRC task for internal command. */
    cmd_ptr = rrc_get_int_cmd_buf();

    cmd_ptr->cmd_hdr.cmd_id = RRC_DOWNLINK_SDU_IND;
                                            /* Fill in the Command Id */
    cmd_ptr->cmd.downlink_sdu_ind.lc_id= dl_lc_id; 
                                           /* Updates the RLC Loagical Channel
                                              Id. */
    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu = NULL; 
    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_length = 0;
    lcm_status = rrclcm_get_dl_lc_type(dl_lc_id, &dl_spl_sdu_type);

    /* Check if the logical channel ID passed by RLC exists.
       Also see if this is a special DCCH logical channel
       type. If both conditions are met, declare it to
       be of type "rrc_DL_DCCH_Message_PDU"

       This condition is added because contrary to SRBs 0 to 4,
       there is no fixed relationship between SRB#5 RB Id & its logical channel ID 
    */
    if(lcm_status == RRCLCM_SUCCESS &&
       dl_spl_sdu_type == UE_LOGCHAN_TM_DCCH_DL)
    {
      WRRC_MSG0_HIGH("RRC_DOWNLINK_SDU_IND on DCCH for SRB#5");
      dl_sdu_type = RRCLCM_DL_DCCH_SRB5_SDU_TYPE;
    }
    else      
    {
      /* Get SDU type of the OTA message */
      dl_rb_id = rrclcm_get_dl_rb_id(dl_lc_id);
      switch(dl_rb_id)
      {
        case BCCH_S_RADIO_BEARER_ID: 
        case BCCH_N_RADIO_BEARER_ID:        /* Broadcast Control Channel.*/
     
          dl_sdu_type = rrc_BCCH_BCH_Message_PDU;
          break;

        case BCCH_FACH_RADIO_BEARER_ID:    /* Broadcast Control Channel.*/

          dl_sdu_type = rrc_BCCH_FACH_Message_PDU;
          break;

        case CCCH_RADIO_BEARER_ID:         /* Common Control Channel    */
          MSG_LOW("RRC_DOWNLINK_SDU_IND on CCCH RB ID %d",dl_rb_id,0,0);
          dl_sdu_type = rrc_DL_CCCH_Message_PDU;
          break;

        case DCCH_UM_RADIO_BEARER_ID:       /* Dedicated Control Channel */
        case DCCH_AM_RADIO_BEARER_ID:
        case DCCH_DT_HIGH_PRI_RADIO_BEARER_ID:
        case DCCH_DT_LOW_PRI_RADIO_BEARER_ID:
          MSG_LOW("RRC_DOWNLINK_SDU_IND on DCCH RB ID %d",dl_rb_id,0,0);
          dl_sdu_type = rrc_DL_DCCH_Message_PDU;
          break;

        case PCCH_RADIO_BEARER_ID:         /* Paging Control Channel    */
          MSG_LOW("RRC_DOWNLINK_SDU_IND on PCCH",0,0,0);
          dl_sdu_type = rrc_PCCH_Message_PDU;
          break;

        default:
          WRRC_MSG1_HIGH("PDU from unknown RB Id: %d", dl_rb_id);
          dl_sdu_type = RRCLCM_INVALID_SDU_TYPE;
          break;

      }/*end switch*/
    } 

    cmd_ptr->cmd.downlink_sdu_ind.dl_sdu_type = dl_sdu_type; 
                                   
    cmd_ptr->cmd.downlink_sdu_ind.decoded_msg = (void *) NULL;

    rrc_put_int_cmd(cmd_ptr);

  /* Decrement the count */
    count--;
  }
} /* rrclcm_post_event_for_dl_sdu */

/*===========================================================================

 FUNCTION rrclcm_post_event_for_dl_l2ack_ind

DESCRIPTION
  This function will post an internal event to RRC for the downlink L2 ACK 
  confirm indication.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_post_event_for_dl_l2ack_ind
(    
  rlc_lc_id_type dl_lc_id,                      /* The Downlink RLC Logical
                                                   Channel Id. */  
  uecomdef_status_e_type  status,               /* Indicates the status of 
                                                   success or failure of SDU
                                                   transmission */
  uint8 mui,                                    /* Message Unit Id */
  rlc_sdu_tx_fail_reason_e_type failure_reason  /* Failure reason */
)
{
  rrc_cmd_type *cmd_ptr;
#ifndef FEATURE_DUAL_SIM
  uint8 i = 0;
  boolean mui_found= FALSE;
  uint8 rb_id_idx = 0; /* Local variable for RB Id 
                                          Index. */
  rb_id_idx = rrclcm_get_ul_rb_id(dl_lc_id) - 2;
#endif
  
  if(rrcllc_curr_cmd_rel_all_req() == TRUE)
  {
    MSG_HIGH("No need of posting a cmd as it is CRLC REL ALL REQ mui %d, status %d lc id %d",
                   mui,status,dl_lc_id);
    return;
  }
#ifndef FEATURE_DUAL_SIM
  for(i=0;i<MAX_NUM_OF_PROCEDURES;i++)
  {
    if(rb_id_idx < (UE_MAX_UL_AM_DATA_CHANNEL - 1))
    {
      /* Searches for MUI match in the Look Up table*/
      if(rrclcm_l2_ack_info[i][rb_id_idx].mui ==  mui)
      {
        mui_found =TRUE;
        break;
      }  
    }
  }    
    
  
  if(mui_found == FALSE)
  {
    MSG_HIGH("L2 ack for MUI %d for unregistered  proc, status %d [0:S, 1:F] lc id %d",
                       mui,status,dl_lc_id);
    return;
  }
#endif
  /* Get a command buffer from RRC task for internal command. */
  cmd_ptr = rrc_get_int_cmd_buf();

  cmd_ptr->cmd_hdr.cmd_id = RRC_DOWNLINK_L2ACK_IND;
                                         /* Fill in the Command Id */
  cmd_ptr->cmd.downlink_l2ack_ind.lc_id = dl_lc_id;     
                                         /* Updates the RLC Loagical Channel
                                            Id. */
  cmd_ptr->cmd.downlink_l2ack_ind.status = status;
                                         /* Updates the Success or Failure
                                            status SDU transmission. */
  cmd_ptr->cmd.downlink_l2ack_ind.mui = mui; 
                                         /* Updates the MUI */

  cmd_ptr->cmd.downlink_l2ack_ind.failure_reason = failure_reason;
                                         /* Failure reason */

  MSG_LOW("RRC_DOWNLINK_L2ACK_IND command is posted thru callback.Failure reason %d",
             failure_reason,0,0);

  rrc_put_int_cmd(cmd_ptr);

  
} /* rrclcm_post_event_for_dl_l2ack_ind */
/*===========================================================================

 FUNCTION rrclcm_get_watermark_ptr

DESCRIPTION
  This function will get watermark pointer corresponding to the RLC Logical
  channel Id.

DEPENDENCIES
  None

RETURN VALUE
   Pointer the DSM items' queue.

SIDE EFFECTS
  None
===========================================================================*/

dsm_watermark_type *rrclcm_get_dl_watermark_ptr
(    
  rlc_lc_id_type dl_lc_id                  /* The Downlink RLC Logical
                                              Channel Id. */    
)
{
  return rrclcm_dl_rlc_lc_info[dl_lc_id].rx_queue ;
} /* rrclcm_get_watermark_ptr */
/*===========================================================================

 FUNCTION rrclcm_get_ul_watermark_ptr

DESCRIPTION
  This function will get watermark pointer corresponding to the RLC Logical
  channel Id.

DEPENDENCIES
  None

RETURN VALUE
   Pointer the DSM items' queue.

SIDE EFFECTS
  None
===========================================================================*/

dsm_watermark_type *rrclcm_get_ul_watermark_ptr
(    
  rlc_lc_id_type ul_lc_id                  /* The uplink RLC Logical
                                              Channel Id. */    
)
{
  if(ul_lc_id >= UE_MAX_UL_LOGICAL_CHANNEL)
  {
    ERR_FATAL("ul_lc_id out of bounds : 0x%x",ul_lc_id,0,0);
  }
  return rrclcm_ul_rlc_lc_info[ul_lc_id].tx_queue ;
} /* rrclcm_get_ul_watermark_ptr */
/*===========================================================================

FUNCTION rrclcm_get_dl_rb_id

DESCRIPTION
  This function will get radio bearer Id  corresponding to the RLC Logical
  channel Id.

DEPENDENCIES
  None

RETURN VALUE
  rrc_RB_Identity.

SIDE EFFECTS
  None
===========================================================================*/

rrc_RB_Identity rrclcm_get_dl_rb_id
(    
  rlc_lc_id_type dl_lc_id                  /* The Downlink RLC Logical
                                              Channel Id. */   
)
{
  return rrclcm_dl_rlc_lc_info[dl_lc_id].rb_id ;
} /* rrclcm_get_rb_dl_id */
/*=========================================================================*/

/*===========================================================================

FUNCTION rrclcm_get_ul_rlc_mode

DESCRIPTION
  This function will check whether Uplink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. If allocated then it returns
  assoctaed mode of transmission.

DEPENDENCIES
  None

RETURN VALUE
   Returns RRCLCM_SUCCESS if RLC Logical Channel Id allocated. Otherwise returns
   RRCLCM_FAILURE.

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_get_ul_rlc_mode
(    
  rlc_lc_id_type ul_lc_id,                      /* RLC Logical Channel Id */
  uecomdef_logch_mode_e_type *tx_mode           /* Mode of transmission */
)
{
  rrclcm_status_e_type status = RRCLCM_FAILURE;
                                                /* Initialize status to 
                                                   Failure */
  uint32 i;                                     /* Local counter */

  for( i = 0; i < UE_MAX_UL_LOGICAL_CHANNEL ; i++ )
  {
  /* Checks for RLC Logical Channel Id match for Uplink RLC Logiacal Channel 
     entities. */
    if( ul_lc_id == rrclcm_ul_rlc_lc_info[i].lc_id &&
        rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
    {
      /* When match is found update the mode of transmission from Database */
      *tx_mode = rrclcm_ul_rlc_lc_info[i].mode;
      status = RRCLCM_SUCCESS;
      break;
    }
  }
  return(status);
}

/*===========================================================================

FUNCTION rrclcm_get_dl_rlc_mode

DESCRIPTION
  This function will check whether Downlink RLC Logical Channel Id is alloated 
  or not for the requested RLC service entity. If allocated then it returns 
  associated mode reception.

DEPENDENCIES
  None

RETURN VALUE
   Returns RRCLCM_SUCCESS if RLC Logical Channel Id allocated. Otherwise returns
   RRCLCM_FAILURE.

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_status_e_type rrclcm_get_dl_rlc_mode
(    
  rlc_lc_id_type dl_lc_id,                   /* RLC Logical Channel Id */
  uecomdef_logch_mode_e_type *rx_mode        /* Mode of reception */

 
)
{
  rrclcm_status_e_type status = RRCLCM_FAILURE;
                                                /* Initialize status to 
                                                   Failure */
  uint32 i;                                     /* Local counter */

  for( i = 0; i < UE_MAX_DL_LOGICAL_CHANNEL ; i++ )
  {
  /* Checks for RLC Logical Channel Id match for Downlink RLC Logiacal Channel 
     entities. */
    if( dl_lc_id == rrclcm_dl_rlc_lc_info[i].lc_id &&
        rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
    {
      /* When match is found update the mode of reception from Database */
      *rx_mode = rrclcm_dl_rlc_lc_info[i].mode;
      status = RRCLCM_SUCCESS;
      break;
    }
  }
  return(status);
}

/*===========================================================================

FUNCTION rrclcm_update_db_with_srb5_wmq_info

DESCRIPTION
   This function ideally is expected to be called after a logical
   channel ID gets allocated to SRB#5 RB. During LCM initialization,
   watermark pointer is set to NULL. 

DEPENDENCIES
  None

RETURN VALUE
   Returns TRUE SRB#5 watermark is successfully updated.
           FALSE otherwise.

SIDE EFFECTS
  None
===========================================================================*/
boolean rrclcm_update_db_with_srb5_wmq_info(rlc_lc_id_type srb5_lc_id)
{
  boolean wm_ptr_initialized = FALSE;

  if(srb5_lc_id < UE_MAX_DL_TM_CHANNEL &&
     rrclcm_dl_rlc_lc_info[srb5_lc_id].status == RRCLCM_LC_IN_USE &&
     rrclcm_dl_rlc_lc_info[srb5_lc_id].lc_type == UE_LOGCHAN_TM_DCCH_DL)
  {
    rrclcm_dl_rlc_lc_info[srb5_lc_id].rx_queue = &rrclcm_srb5_dcch_dl_wmq;
    wm_ptr_initialized = TRUE;
  }

  return wm_ptr_initialized;
} /* rrclcm_update_db_with_srb5_wmq_info */

/*===========================================================================

FUNCTION rrclcm_get_rb_id_if_dl_lc_type_exists

DESCRIPTION
   This function checks whether Downlink RLC Logical Channel type exists in LCM
   database or not & returns the RB Id if present. See side effect.

DEPENDENCIES
  None

RETURN VALUE
   Returns RRCLCM_SUCCESS if DL RLC Logical Channel type exists.
   Otherwise returns RRCLCM_FAILURE.

SIDE EFFECTS
  Please note that for the given logical channel type, only the first 
  occurence of RB ID is returned.
===========================================================================*/
rrclcm_status_e_type rrclcm_get_rb_id_if_dl_lc_type_exists
(    
  uecomdef_logchan_e_type  lc_type,        /* Mode of reception */
  rrc_RB_Identity         *rb_id           /* Radio bearer ID */
)
{
  rrclcm_status_e_type status = RRCLCM_FAILURE;
                                                /* Initialize status to 
                                                   Failure */
  uint32 i;                                     /* Local counter */

  for( i = 0; i < UE_MAX_DL_TM_CHANNEL ; i++ )
  {
  /* Checks for RLC Logical Channel Id match for Downlink RLC Logical Channel 
     entities. */
    if(rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE &&
       rrclcm_dl_rlc_lc_info[i].lc_type == lc_type)
    {
      *rb_id = rrclcm_dl_rlc_lc_info[i].rb_id;
      status = RRCLCM_SUCCESS;
      break;
    }
  }
  return(status);
} /* rrclcm_get_rb_id_if_dl_lc_type_exists */

#ifdef FEATURE_DUAL_SIM

/*===========================================================================

FUNCTION rrc_check_for_pending_ul_srb_data

DESCRIPTION
  This function checks whether UE is waiting for ul srb pending data
  for SRB

DEPENDENCIES
  None

RETURN VALUE
   TRUE : If any SRB is waiting for UL L2 srb data
   else returns  FALSE

SIDE EFFECTS
  None
===========================================================================*/

boolean rrc_check_for_pending_ul_srb_data(boolean *l2_int_data_pending)
{
  int i=0;
  
  /* Checks for Logical Channel Type and Radio Bearer Id match for AM modes. */
  for( i = ( UE_MAX_UL_TM_CHANNEL + UE_MAX_UL_UM_CHANNEL );
        i < (UE_MAX_UL_TM_CHANNEL + UE_MAX_UL_UM_CHANNEL + UE_MAX_UL_AM_DATA_CHANNEL); i++ )
  {
    if((rrclcm_ul_rlc_lc_info[i].status == RRCLCM_LC_IN_USE) &&
        ((DCCH_AM_RADIO_BEARER_ID == rrclcm_ul_rlc_lc_info[i].rb_id)||
         (DCCH_DT_HIGH_PRI_RADIO_BEARER_ID == rrclcm_ul_rlc_lc_info[i].rb_id) ||
         (DCCH_DT_LOW_PRI_RADIO_BEARER_ID == rrclcm_ul_rlc_lc_info[i].rb_id)
        )
      )
    {
      if(rlc_check_pending_sdu(rrclcm_ul_rlc_lc_info[i].lc_id,l2_int_data_pending) == TRUE)
      {
        return (TRUE);
      }
    }
  }

  /* UE is not waiting for any for L2 ACK for SRB */
  return (FALSE);   

}

/*===========================================================================

FUNCTION rrc_check_for_pending_dl_srb_data

DESCRIPTION
  This function checks whether UE is waiting for dl srb pending data
  for SRB

DEPENDENCIES
  None

RETURN VALUE
   TRUE : If SRB is waiting for dl srb pending data
   else returns  FALSE

SIDE EFFECTS
  None
===========================================================================*/

boolean rrc_check_for_pending_dl_srb_data(void)
{
  int i=0;
  
  /* Checks for Logical Channel Type and Radio Bearer Id match for AM modes. */
  for( i = ( UE_MAX_DL_TM_CHANNEL + UE_MAX_DL_UM_CHANNEL );
        i < (UE_MAX_DL_TM_CHANNEL + UE_MAX_DL_UM_CHANNEL + UE_MAX_DL_AM_DATA_CHANNEL); i++ )
  {
    if((rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE) &&
        ((DCCH_AM_RADIO_BEARER_ID == rrclcm_dl_rlc_lc_info[i].rb_id)||
         (DCCH_DT_HIGH_PRI_RADIO_BEARER_ID == rrclcm_dl_rlc_lc_info[i].rb_id) ||
         (DCCH_DT_LOW_PRI_RADIO_BEARER_ID == rrclcm_dl_rlc_lc_info[i].rb_id)
        )
      )
    {
      if(rlc_check_pending_dl_sdu(rrclcm_dl_rlc_lc_info[i].lc_id) == TRUE)
      {
        return (TRUE);
      }
    }
  }

  /* UE is not waiting for any for dl srb data */
  return (FALSE);   

}
#endif

/*===========================================================================

FUNCTION rrclcm_get_nchan_dl_tm

DESCRIPTION
  This function will return number of DL TM channel that are used

DEPENDENCIES
  None

RETURN VALUE
   Returns number of CL TM channels that are used

SIDE EFFECTS
  None
===========================================================================*/

uint8 rrclcm_get_nchan_dl_tm
(    
  void
)
{
  uint8 nchan_dl_tm =0;
  int i=0;
  for(i=0;i<UE_MAX_DL_TM_CHANNEL;i++)
  {
    if(rrclcm_dl_rlc_lc_info[i].status == RRCLCM_LC_IN_USE)
    {
      nchan_dl_tm++;
    }
  }
  return (nchan_dl_tm);
}
/*===========================================================================

FUNCTION rrclcm_get_mui_from_proc_id

DESCRIPTION
  This function gets the mui from the procedure id

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None
===========================================================================*/

rrclcm_mui_type rrclcm_get_mui_from_proc_id
(    
  rrc_proc_e_type proc_id,
  rrc_RB_Identity rb_id
)
{
  uint8 i=0;
  uint8 rb_id_idx = 0;                           /* Local variable for RB Id 
                                              Index. */
  rb_id_idx = rb_id - 2;

  for(i = 0; i <  (uint8)MAX_NUM_OF_PROCEDURES ; i++)
  {
    /* Checks for the available index that is not allocated for any
       procedure */
    if(rrclcm_l2_ack_info[i][rb_id_idx].rrc_proc == proc_id)
    {
      return rrclcm_l2_ack_info[i][rb_id_idx].mui; 
    }
  }
  WRRC_MSG2_ERROR("Failed to find mui for proc_id %d, rb_id %d",proc_id, rb_id);
  return 0;
}

/*===========================================================================

FUNCTION rrclcm_internal_post_l2_ack_ind

DESCRIPTION
  This function posts l2 ack indication

DEPENDENCIES
  None

RETURN VALUE
   None

SIDE EFFECTS
  None
===========================================================================*/

void rrclcm_internal_post_l2_ack_ind_for_proc
(    
  rrc_proc_e_type proc_id,
  rrc_RB_Identity rb_id
)
{

  rrc_cmd_type *cmd_ptr;
  rrclcm_mui_type mui;
  rlc_lc_id_type dl_lc_id;

  mui = rrclcm_get_mui_from_proc_id(proc_id, rb_id);;
  dl_lc_id = rrclcm_get_dl_rlc_lc_id(rb_id);

  /* Get a command buffer from RRC task for internal command. */
  cmd_ptr = rrc_get_int_cmd_buf();

  if(cmd_ptr != NULL)
  {
    cmd_ptr->cmd_hdr.cmd_id = RRC_DOWNLINK_L2ACK_IND;
                                           /* Fill in the Command Id */
    cmd_ptr->cmd.downlink_l2ack_ind.lc_id = dl_lc_id;     
                                           /* Updates the RLC Loagical Channel
                                              Id. */
    cmd_ptr->cmd.downlink_l2ack_ind.status = SUCCESS;
                                           /* Updates the Success or Failure
                                              status SDU transmission. */
    cmd_ptr->cmd.downlink_l2ack_ind.mui = mui; 
                                           /* Updates the MUI */

  
    MSG_HIGH("RRC_DOWNLINK_L2ACK_IND command is posted for proc-id rrc_proc_e_type_value%d, rb-id %d mui %d",
               proc_id,rb_id,mui);

    rrc_put_int_cmd(cmd_ptr);

  }
  else                                     /* No free command buffers
                                              available */
  {
    WRRC_MSG1_ERROR("RRC Unable to allocate CMD buffer: %d", RRC_DOWNLINK_L2ACK_IND);
  }
}

