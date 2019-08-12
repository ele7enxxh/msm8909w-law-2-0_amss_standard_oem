#ifndef MMOCDBG_H
#define MMOCDBG_H
/*====*====*===*====*====*====*====*====*====*====*====*====*====*====*====*
               M U L T I M O D E   C O N T R O L L E R ( MMoC )

                     D E B U G      M O D U L E

GENERAL DESCRIPTION
   This module contains the debug related functionality of MMoC.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  The MMoC task should be started before DEBUG APIs are called.


Copyright (c) 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/mmoc/src/mmocdbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/05/13   jvo     Added 5 mmocdbg_print_x functions to functionalize usage
                   of MMOC_PRINT_MSG_HIGH under MMOC_STRING_MAPS_ACTIVE
08/16/11   fc      Added QTF support.
10/16/08   sv      Correcting Featurization for Memory Reduction
09/19/08   jd      Memory reduction changes
04/28/08   fj      Lint cleanup, CR 144389
03/12/08   aj      Edit header to reflect p4 path and revision
04/03/06   ka      Changing copyright.
12/30/05   ka      Initial revision to support string maps

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h" /* Customer configuration file.                      */
#include "comdef.h"   /* Definition for basic types and macros             */
#include "msg.h"      /* Message output services.                          */
#include "mmoci.h"    /* Internal interface definitions for MMoC.          */

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */


/*===========================================================================
=============================================================================
=============================================================================
============================== MMOC DEBUG STRING NAMES ======================
=============================================================================
=============================================================================
===========================================================================*/

#ifndef FEATURE_MMOC_LOW_MEM_TARGET
#ifdef MMOC_STRING_MAPS_ACTIVE

/* Provide strings for  values mmoc_cmd_e_type */
extern const char* mmocdbg_cmd_e_type_string_map [MMOC_CMD_MAX];


  /* Provide strings for  values mmoc_rpt_e_type */
extern const char* mmocdbg_rpt_e_type_string_map [MMOC_RPT_MAX];


  /* Provide strings for  values mmoc_trans_state_e_type */
extern const char* mmocdbg_trans_state_e_type_string_map [MMOC_TRANS_STATE_MAX];

  /* Provide strings for  values mmoc_trans_e_type */
extern const char* mmocdbg_trans_e_type_string_map [MMOC_TRANS_MAX];

/* Provide strings for  values prot_state_e_type */
extern const char* mmocdbg_prot_state_e_type_string_map [PROT_STATE_MAX];
#endif


/* <EJECT> */
/*===========================================================================
=============================================================================
=============================================================================
============================== MMOC DEBUG API's =============================
=============================================================================
=============================================================================
===========================================================================*/

/* <EJECT> */
/*===========================================================================

FUNCTION MMOCDBG_PRINT_MESSAGE

DESCRIPTION

 Behaves as printf() - takes the very same format string and any number of
 parameters and allows to print <stuff> to diagnostic monitor output
 screen.

 The problem is that %s in the format string does not work for
 MSG_HIGH (and other MSG_ macros) plus those macros support only
 maximum of 3 arguments, so this function was provided to remedy both problems.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void mmocdbg_print_message
(

     const char *string,
       /* Format in which string needs to be printed */
     ...
);

/*===========================================================================

FUNCTION mmocdbg_print_protocol_state

DESCRIPTION
  Prints out the current protocol state information

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mmocdbg_print_protocol_state
(
  mmoc_state_info_s_type *mmoc_info_ptr
    /* Pointer to MMoC's state information */
);


#ifdef MMOC_STRING_MAPS_ACTIVE
/*===========================================================================

FUNCTION mmocdbg_print_protocol_state_interpreted

DESCRIPTION
  Prints out the current protocol state information with interpreted strings

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mmocdbg_print_protocol_state_interpreted
(
  mmoc_state_info_s_type *mmoc_info_ptr,
    /* Pointer to MMoC's state information */
  
  prot_state_e_type 	  main_stack_prot_state,
    /* main stack's protocol state */
  
  prot_state_e_type 	  hybr2_stack_prot_state,
    /* hybr2 stack's protocol state */
  
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  prot_state_e_type 	  hybr3_stack_prot_state,
    /* hybr3 stack's protocol state */
  #endif
  
  prot_state_e_type 	  hybr_hdr_stack_prot_state
    /* HDR stack's protocol state */

);
#endif


/*===========================================================================

FUNCTION mmocdbg_print_protocol_state_not_interpreted

DESCRIPTION
  Prints out the current protocol state information without interpreted strings

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mmocdbg_print_protocol_state_not_interpreted
(
  mmoc_state_info_s_type *mmoc_info_ptr,
    /* Pointer to MMoC's state information */

  prot_state_e_type	      main_stack_prot_state,
    /* main stack's protocol state */
  
  prot_state_e_type	      hybr2_stack_prot_state,
    /* hybr2 stack's protocol state */
  
  #if defined(FEATURE_MMODE_TRIPLE_SIM) || defined(FEATURE_MMODE_SXLTE_G)
  prot_state_e_type	      hybr3_stack_prot_state,
    /* hybr3 stack's protocol state */
  #endif
  
  prot_state_e_type	      hybr_hdr_stack_prot_state
    /* HDR stack's protocol state */
);


/*===========================================================================

FUNCTION mmocdbg_print_transaction_state

DESCRIPTION
  Prints out the current transaction state

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mmocdbg_print_transaction_state
(
  mmoc_state_info_s_type *mmoc_info_ptr
    /* Pointer to MMoC's state information */
);


/*===========================================================================

FUNCTION mmocdbg_print_report_received

DESCRIPTION
  Prints out the received report

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mmocdbg_print_report_received
(
  mmoc_rpt_msg_s_type *rpt_ptr
    /* The report that was received */
);


/*===========================================================================

FUNCTION mmocdbg_print_command_received

DESCRIPTION
  Prints out the received command

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mmocdbg_print_command_received
(
  mmoc_cmd_msg_s_type *cmd_ptr
    /* The report that was received */
);

/*===========================================================================

FUNCTION mmocdbg_print_before_trans

DESCRIPTION
  Prints out the transaction state and protocol state before the transaction

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mmocdbg_print_before_trans
(
  mmoc_state_info_s_type *mmoc_info_ptr
    /* Pointer to MMoC's state information */
);


/*===========================================================================

FUNCTION mmocdbg_print_after_trans

DESCRIPTION
  Prints out the transaction state and protocol state after the transaction

DEPENDENCIES
  mmocdbg_print_message()

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mmocdbg_print_after_trans
(
  mmoc_state_info_s_type *mmoc_info_ptr
    /* Pointer to MMoC's state information */
);


#endif /* FEATURE_MMOC_LOW_MEM_TARGET */

#endif /* MMOCDBG_H */
