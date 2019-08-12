#ifndef GSTK_TERM_PROFILE_H
#define GSTK_TERM_PROFILE_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


         G S T K   T E R M I N A L   P R O F I L E   H E A D E R


GENERAL DESCRIPTION
  This header file contains function definitions for gstk_term_profile.c


FUNCTIONS
  gstk_uim_terminal_profile_report
    This is the callback function for terminal profile response.

  gstk_cache_tp_info
    This function caches the terminal profile data received from GSTK clients

  gstk_pack_tp_cmd_to_uim
    This function puts the gstk_terminal_profile_cache info into uim format
    and put onto UIM's command queue

  gstk_toolkit_mode_change
    Inform clients that there is a mode change

  gstk_process_mode_change
    This function will perform UIM reset and toolkit change based on new CM
	mode if required

INITIALIZATION AND SEQUENCING REQUIREMENTS


                        COPYRIGHT INFORMATION

Copyright (c) 2003-2012,2014 QUALCOMM Technologies, Inc (QTI) and its licensors.
All Rights Reserved.  QUALCOMM Technologies Proprietary.
Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/gstk/src/gstk_term_profile.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/15   dy      Enabling Toolkit-initiated GBA TP bit (byte 25 bit 5)
08/05/14   lxu     Use NV Bitmask to control the Dual STK App
08/05/14   lxu     Support feature FEATURE_MODE_SWITCH_STK_SWITCH
07/11/14   shr     Add Support for NV Refresh
01/14/14   lxu     Cust 25th TP values definitions
02/27/12   sg      Cust TP values definitions
12/20/11   nb      GSDI Cleanup
09/20/11   bd      Fixed compiler error
03/26/09   xz      Fix the issue of TP DL failing if GSTK fails to get app_id
                   when parse client TP and simplify TP download procedure
10/22/08   sk      Changes parameter prototype for gstk_cache_tp_info_ext
02/06/08   sk      Lint fixes
11/29/07   sk      Added support for differentiating SAT and USAT TP dl
10/01/04   tml     Change function prototype for terminal profile caching
09/04/03   tml     dual slot support
02/07/03   tml     Initial Version

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "gstk.h"
#include "gstki.h"
#include "gstk_exp.h"
#include "uim.h"

/* TP byte 25, 31 and 32 pointing to array positions 25, 30 and 31 respectively */
#define GSTK_TP_BYTE_25         24
#define GSTK_TP_BYTE_31         30
#define GSTK_TP_BYTE_32         31
/* Cust TP values - proprietary*/
#define GSTK_CUST_TP_BYTE_25    0x00
#define GSTK_CUST_TP_BYTE_31    0x43
#define GSTK_CUST_TP_BYTE_32    0x4B

/*===========================================================================
FUNCTION gstk_uim_terminal_profile_report

DESCRIPTION

  This is the callback function for terminal profile response.  UIM will call
  this function when it receives terminal profile response from the Card
  Based on the return status of the terminal profile response, this function
  will build the gstk_term_profile_rsp_type and set GSTK_CMD_Q_SIG so that
  this command can be processed in the gstk state machine

PARAMETERS
  report: [Input] Pointer to the UIM terminal profile report

DEPENDENCIES
  None

RETURN VALUE
  None

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern void gstk_uim_terminal_profile_report (
  uim_rpt_type *report
);

/*===========================================================================
FUNCTION gstk_update_default_app_specific_tp

DESCRIPTION
  This function sends app specific terminal profile that GSTK supports by
  default

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_update_default_app_specific_tp(
  uint8 gstk_instance_idx
);

/*===========================================================================
FUNCTION gstk_pack_tp_cmd_to_uim

DESCRIPTION

  This function puts the gstk_terminal_profile_cache info into uim format
  and put onto UIM's command queue

PARAMETERS
  None

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_pack_tp_cmd_to_uim (void);

/*===========================================================================
FUNCTION gstk_cache_tp_info

DESCRIPTION
  This function caches the terminal profile data received from GSTK clients

PARAMETERS
  term_profile_ptr: [Input] Mobile Terminal Profile info from GSTK client

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
extern gstk_status_enum_type gstk_cache_tp_info(
  const gstk_cmd_type  *term_profile_ptr
);

/*===========================================================================
FUNCTION gstk_pack_tp_cmd_to_uim_with_df

DESCRIPTION

  This function puts the gstk_curr_inst_ptr->gstk_terminal_profile_cache info into uim format
  and put onto UIM's command queue

PARAMETERS
  dir  :  GSM DF or CDMA DF

DEPENDENCIES
  None

RETURN VALUE
  gstk_status_enum_type

COMMENTS
  None

SIDE EFFECTS
  None

SEE ALSO
  None
===========================================================================*/
gstk_status_enum_type gstk_pack_tp_cmd_to_uim_with_df (
  uim_df_type df);

/*===========================================================================
FUNCTION: gstk_tp_value_needs_to_be_disabled

DESCRIPTION:
  This function will return TRUE if the TP bits of this feature need to be disabled.

PARAMETERS:
  location_mask - the TP bit mask
  cat_version - the CAT version of the card

DEPENDENCIES:
  None

RETURN VALUE:
 TRUE - TP bit should be disabled
 FALSE - TP bit should be left as is

COMMENTS:
  None

SIDE EFFECTS:
  None

===========================================================================*/
boolean gstk_tp_value_needs_to_be_disabled(
  uint32 location_mask,
  uint8  cat_version
);
#endif /*GSTK_TERM_PROFILE_H */
