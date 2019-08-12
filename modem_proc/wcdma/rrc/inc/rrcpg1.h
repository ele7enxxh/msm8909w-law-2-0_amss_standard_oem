#ifndef RRCPG1_H
#define RRCPG1_H
/*===========================================================================
                  R R C  P A G I N G  T Y P E 1

DESCRIPTION
  This header file contains prototype declarations for Paging Type 1 
  procedure.

Copyright (c) 2000, 2001,2003,2005,2007,2008 Qualcomm Technologies, Inc.  
All Rights Reserved.QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rrc/vcs/rrcpg1.h_v   1.0   29 Apr 2001 21:22:50   rjeenaga  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/inc/rrcpg1.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/15   ad      Made changes to log important parms from OTAs
08/23/13   sr      Made changes to send PAGE_IND to NAS if CN page is received in PCH
03/07/12   gv      Modified the function name to fix compilation issue with free floating
12/12/11   gv      Added code to support free floating of tasks
05/30/11   rl      CMI de-featurization
05/08/09   ss      Updated Copyright Information
04/11/07   da      Added prototype's for fn rrcpg1_get_num_pages_matched() 
                   and rrcpg1_get_num_pages()
03/28/07   kp      Added prototype's for functions & enum's to support
                   Implementation of BCCH Mod Timer.
11/09/05   kc      Added prototype to compare group release information
04/12/01   rj      Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */
#include "wcdma_variation.h"
#include "rrcdata_v.h"


/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/*Max value to set up PCCPCH by L1 is 5 frames+ some guard time of 30msec = (80msec)*/
  #define RRCPG1_MAX_TIME_TO_SET_UP_PCCPCH 8

/*Value of One Frame is 10msec for SFN*/
  #define RRCPG1_VALUE_OF_ONE_FRAME 10

  /*ASN1 valid values for BCCH-ModificationTime 
  are in the range of::= INTEGER (0 . . 511 ) */
  #define RRCPG1_INVALID_BCCH_MOD_TIMER_VAL 512

  /*MIB-ValueTag ::= INTEGER (1 . . 8 ) */
  #define RRCPG1_INVALID_MIB_V_TAG 9
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* Enum which indicates different phases of  BCCH 
Modification info I.E process */
typedef enum
{
  RRCPG1_BCCH_MOD_INFO_NOT_IN_PROCESS,                   /* Currently BCCH ModInfo i.e is not processed.*/
  RRCPG1_BCCH_MOD_INFO_IN_PROCESS,   /* Currently BCCH ModInfo i.e is being processed.*/
  RRCPG1_BCCH_MOD_INFO_DUPLICATE /* Currently BCCH ModInfo i.e is being processed & received I.E is duplicate of the one in progress.*/
}rrcpg1_bcch_mod_info_status_e_type;

/* Data structure to store information about BCCH 
Modification info I.E received over the air */
typedef struct
{
boolean bcch_mod_info_db_valid; /* Indicates if the stored BCCH
                                      mod info values are valid or not.*/
uint16 bcch_mod_timer_value; /* BCCH modification timer value 
                                       received in latest BCCH mod info I.E */
uint8 bcch_mod_mib_v_tag;    /* MIB value tag received in latest 
                                      latest BCCH mod info I.E */
}rrcpg1_paging_bcch_mod_info;


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */

/*===========================================================================

FUNCTION rrc_compare_group_release_info  

DESCRIPTION

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
uecomdef_status_e_type rrc_compare_group_release_info
(
  rrc_GroupReleaseInformation * group_release_info
);

/*===========================================================================

FUNCTION rrcpg1_init_procedure

DESCRIPTION
  This function initializes everything that the Paging Type 1 procedure
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcpg1_init_procedure(void);

/*===========================================================================

FUNCTION rrcpg1_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the Paging Type 1 procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC)


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcpg1_procedure_event_handler(rrc_cmd_type *cmd_ptr);

/*===========================================================================

FUNCTION rrcpg1_post_update_sibs_cmd

DESCRIPTION
  This function is used to post UPDATE_SIBS_CMD
  to SIB procedure

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/
void rrcpg1_post_update_sibs_cmd
(
  void
);
/*===========================================================================
FUNCTION rrcpg1_check_for_duplicate_bcch_mod_info

DESCRIPTION
This function checks for duplicate received BCCH Mod Info I.E.
If there is BCCH Mod Info I.E in process & the newly received 
I.E has the same parameter as the one under process then this 
function will Indicate duplicate. Other wise it will return appropriate 
status..

DEPENDENCIES
  None.

RETURN VALUE
  RRCPG1_BCCH_MOD_INFO_NOT_IN_PROCESS = Currently BCCH ModInfo i.e is not processed. 
  RRCPG1_BCCH_MOD_INFO_DUPLICATE =  Currently BCCH ModInfo i.e is being processed & received I.E
                                    is duplicate of the one in progress.
  RRCPG1_BCCH_MOD_INFO_IN_PROCESS =  Currently BCCH ModInfo i.e is being processed.
===========================================================================*/
rrcpg1_bcch_mod_info_status_e_type rrcpg1_check_for_duplicate_bcch_mod_info
(
  uint8 mib_v_tag,  /* MIB value tag in rexd OTA message*/
  uint16 bcch_mod_timer_val  /* BCCH modification timer value*/
);
/*===========================================================================
FUNCTION rrcpg1_process_bcch_mod_info()

DESCRIPTION
This function will process BCCH ModInfo I.E.
It will take care of starting timer based on current
SFN & the SFN where BCCH modifiction will happen.

This will also update BCCH ModInfo DB with the 
MIB value tag & BCCH Mod timer received in OTA
message.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcpg1_process_bcch_mod_info
(
  uint8 mib_v_tag,  /* MIB value tag in rexd OTA message*/
  uint16 bcch_mod_timer  /* BCCH modification timer value*/
);
/*===========================================================================
FUNCTION rrcpg1_check_and_abort_bcch_mod_info_processing()

DESCRIPTION
This function will check & abort processing of BCCH
ModInfo & also it takes care of stopping this timer 
RRC_BCCH_MODIFICATION_TIMER if its running. 

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcpg1_check_and_abort_bcch_mod_info_processing
(
void
);

/*===========================================================================
FUNCTION rrcpg1_set_bcch_mod_info_db()

DESCRIPTION
This function will validate the input parameters & then update
bcch ModInfo db accordingly.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
boolean rrcpg1_set_bcch_mod_info_db
(
  uint8 mib_v_tag,  /* MIB value tag in rexd OTA message*/
  uint16 bcch_mod_timer  /* BCCH modification timer value*/
);
/*===========================================================================
FUNCTION rrcpg1_process_bcch_mod_info_msg_ie()

DESCRIPTION
This function will process BCCHModInfo I.E received in 
OTA message.

DEPENDENCIES
  None.

RETURN VALUE
  None
===========================================================================*/
void rrcpg1_process_bcch_mod_info_msg_ie
(
  uint8 mib_v_tag,  /* MIB value tag in rexd OTA message*/
  uint16 bcch_mod_time,  /* BCCH modification timer value*/
  boolean bcch_mod_timer_present  /*Boolean to indicate whether BCCH Time present in BCCHModInfo i.e*/
);
/*===========================================================================
FUNCTION rrcpg1_return_bcch_mod_info_status()

DESCRIPTION
This function will return Bcch mod info
status 

DEPENDENCIES
  None.

RETURN VALUE
  TRUE = If BCCH ModIfo in progress
  FALSE = If BCCH ModIfo is not in progress
===========================================================================*/
boolean rrcpg1_return_bcch_mod_info_status
(
void
);
/*===========================================================================

FUNCTION rrcpg1_get_num_pages

DESCRIPTION
  This function returns the value of rrcpg1_num_pages.  This indicates
  the total number of pages received [false alarm + valid pages.
  This function will be called to display counter in QXDM

DEPENDENCIES
  None.

RETURN VALUE
  uint32

===========================================================================*/
uint32 rrcpg1_get_num_pages(void);
/*===========================================================================

FUNCTION rrcpg1_get_num_pages_matched

DESCRIPTION
  This function returns the value of rrcpg1_num_pages_matched.  This indicates
  the total number of pages minus the false alarms.
  This function will be called to display counter in QXDM

DEPENDENCIES
  None.

RETURN VALUE
  uint32

===========================================================================*/
uint32 rrcpg1_get_num_pages_matched(void);

#ifdef FEATURE_WCDMA_FREE_FLOATING_TASK
/*===========================================================================

FUNCTION rrcpg1_ff_modify_sibs_for_sfn_update

DESCRIPTION
  

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrcpg1_ff_modify_sibs_for_sfn_update(void);
#endif

/*===========================================================================

FUNCTION rrcpg1_send_page_ind  

DESCRIPTION
  This function allocates memory and sends RRC_PAGE_IND to NAS when a page match is found

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/

void rrcpg1_send_page_ind(void);

/*===========================================================================

FUNCTION rrc_qsh_log_paging_params  

DESCRIPTION
  This function allocates memory and sends RRC_PAGE_IND to NAS when a page match is found

DEPENDENCIES
  None.

RETURN VALUE
  None

===========================================================================*/
void rrc_qsh_log_paging_params(rrc_proc_e_type proc_id, rrc_log_pg_ota *page_info);
#endif /* RRCPG1_H */

