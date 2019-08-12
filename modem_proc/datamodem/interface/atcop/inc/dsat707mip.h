#ifndef DSAT707MIP_H
#define DSAT707MIP_H
/*===========================================================================

                    D A T A   S E R V I C E S  M I P

                A T   C O M M A N D   P R O C E S S O R
                
                I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode Mobile IP commands ) processor.

EXTERNALIZED FUNCTIONS
dsat707_exec_qcmip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIP command.
  Enable or disable Mobile IP.

dsat707_exec_qcmipp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPP command.
  Selects MIP user profile to be active. Takes a profile number
  between 0 and 5.

dsat707_exec_qcmipt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPT command.
  Enable/disable the use of RFC 2002bis authentication.

dsat707_exec_qcmipep_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPEP command.
  Enable/disable currently active profile.

dsat707_exec_qcmipmass_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASS command.

dsat707_exec_qcmipmhss_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSS command.
  Set MN-HA shared secrets for the currently active profile.

dsat707_exec_qcmipmaspi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASPI command.
  Set MN-AAA SPIs for the currently active profile.

dsat707_exec_qcmipmhspi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSPI command.
  Set MN-HA SPIs for the currently active profile.

dsat707_exec_qcmiprt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPRT command.
  Set the reverse tunneling for currently active profile.
  
dsat707_exec_qcmipnai_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPNAI command.
  Set the NAI for the currently active profile.
  
dsat707_exec_qcmipha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPHA command.
  
dsat707_exec_qcmippha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPPHA command.

dsat707_exec_qcmipsha_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPSHA command.
  
dsat707_exec_qcmipgetp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPGETP command.
  Return all information corresponding to the specified profile
  number.
  
  Copyright (c) 1995 - 2013 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707mip.h_v   1.6   02 Dec 2002 20:00:02   akhare  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsat707mip.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/12   sk      Feature cleanup.
07/22/11   dvk     Global Variable Cleanup
12/10/09   ps      Mainlining OMH feature based on new profile management
                   design.
09/04/09   ss      CMI SU level modifications.
03/04/09   sa      AU level CMI modifications.
07/21/08   bs      Added support for CDMA NAARefresh.
04/28/04   jd      Changed shadow NV structure to include explicit lengths
                   and 'set' members to solve various bugs in dsat707mip.c
12/02/02   ak      Made set_mdr_val and set_crm_val external functions.
11/14/02   jd      Removed prototypes for dsat707_erase_shadow_nv 
09/18/02   jay     Removed AT_TEST feature and formatted the code.
07/12/02   ak      Extern for dsat707_check_mdr_val.
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#ifdef FEATURE_DS_MOBILE_IP
#include "dsati.h"
#include "dsat707mip_api.h"
#include "dsmip_v.h"
#endif /* FEATURE_DS_MOBILE_IP */
#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIP command.
  Enable or disable Mobile IP.
  0 - Mobile IP disabled, Simple IP only.
  1 - Mobile IP preferred.
  2 - Mobile IP only.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmip_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmip_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPP command.
  Selects MIP user profile to be active. Takes a profile number
  between 0 and 5.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipp_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipp_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPT command.
  Enable/disable the use of RFC 2002bis authentication.
  0 - Use of RFC 2002bis authentication is disabled. RFC 2002
  style authentication is used instead.
  1 - Use of RFC 2002bis authentication is enabled.  
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipt_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
);  /* dsat707_exec_qcmipt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPEP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPEP command.
  Enable/disable currently active profile.
  0 - Disable the currently active profile.
  1 - Enable the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipep_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
);

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASS command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipmass_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipep_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSS command.
  Set MN-HA shared secrets for the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipmhss_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipmhss_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASPI command.
  Set MN-AAA SPIs for the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipmaspi_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipmaspi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSPI command.
  Set MN-HA SPIs for the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipmhspi_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipmhspi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPRT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPRT command.
  Set the reverse tunneling for currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmiprt_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmiprt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPNAI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPNAI command.
  Set the NAI for the currently active profile.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipnai_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipnai_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPHA command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipha_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPPHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPPHA command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmippha_cmd
(
  dsat_mode_enum_type      mode,         /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,   /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,     /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr /*  Place to put response       */
); /* dsat707_exec_qcmippha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPSHA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPSHA command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipsha_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipsha_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPGETP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPGETP command.
  Return all information corresponding to the specified profile
  number.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipgetp_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
); /* dsat707_exec_qcmipgetp_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMASSX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMASSX command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipmassx_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
);
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCMIPMHSSX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMIPMHSSX command.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
extern dsat_result_enum_type dsat707_exec_qcmipmhssx_cmd
(
  dsat_mode_enum_type      mode,        /*  AT command mode:            */
  const dsati_cmd_type     *tab_entry,  /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type            *rb_ptr      /*  Place to put response       */
);
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_IS707     */
#endif /* DSAT707MIP_H         */
