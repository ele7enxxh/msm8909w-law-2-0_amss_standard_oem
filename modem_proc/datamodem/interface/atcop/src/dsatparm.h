#ifndef DSATPARM_H
#define DSATPARM_H
/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   ( P A R A M E T E R 
                
                C O M M A N D S )   P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( parameter commands ) processor.

Copyright (c) 2001 - 2012,2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatparm.h_v   1.1   30 Sep 2002 15:34:08   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatparm.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/14   tk      Optimized debug macros usage in ATCoP.
05/18/12   tk      Migrated to MSG 2.0 macros
01/19/12   sk      Feature cleanup.
02/18/08   sa      Added modification for Passport Feature support.
04/11/07   ua      Modifications as per KDDI requirements. 
04/14/05   sb      Changes for incoming CS call when serializer is enabled
02/10/05   sb      Fix compile issues.
12/04/03   ar      Added dsatparm_gen_ampv_response() prototype.
08/27/01   sb	   ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
05/04/01   SB      created file

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsati.h"

#ifdef FEATURE_DATA_IS707

#include "ds707_roaming.h"

/* Test for JCDMA operating modes */

#define IS_JCDMA_MODE() \
       (ds707_roaming_is_curr_mode_jcdma() == TRUE )

#define NOT_SUPPORTED_IN_JCDMA() \
  if(dsatcmdp_not_supported_in_jcmda())\
  {\
    return DSAT_ERROR;\
  }

#endif /* FEATURE_DATA_IS707 */
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSATPARM_EXEC_PARAM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes parameter / S register
  commands.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    OK : if the command has been successfully executed
    ERROR : if there was any problem in executing the command
    
SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
dsat_result_enum_type dsatparm_exec_param_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_EXT_PARAM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes commands only in NON-JCDMA mode.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatparm_exec_ext_param_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_IPR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +IPR command.
  at+ipr is used to specify the data rate at which the DCE will 
  accept the command

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    OK : if the command has been successfully executed
    ERROR : if there was any problem in executing the command
    
SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
dsat_result_enum_type dsatparm_exec_ipr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATPARM_EXEC_AMPV_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at&v command.
  at&v is used to dump configuration parameters.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
dsat_result_enum_type dsatparm_exec_ampv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATPARM_GEN_AMPV_RESPONSE

DESCRIPTION
  This function generates the response for the &V command.  A separate
  routine is used to accomodate asychronous calls to query functions
  in other tasks.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatparm_gen_ampv_response
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATPARM_EXEC_DTYPE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATT and ATP commands.
  ATT and ATP are used to select type of dialling (tone or pulse) 
  to be used in subsequent dials. This commands will have no effect 
  in our code

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatparm_exec_dtype_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_FCLASS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +FCLASS command.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NORESPONSE : set for certain commands

SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
dsat_result_enum_type dsatparm_exec_fclass_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_STHREE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATS3 command. ATS3 is used to set the 
  command line termination character . 
  
DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  None.
  
===========================================================================*/
dsat_result_enum_type dsatparm_exec_sthree_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_SFOUR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATS4 command. ATS4 is used to set the 
  response formatting character . 
  
DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  None.
  
===========================================================================*/
dsat_result_enum_type dsatparm_exec_sfour_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_SFIVE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATS5 command. ATS5 is used to set the 
  command line editing character. 
  
DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  None.
  
===========================================================================*/
dsat_result_enum_type dsatparm_exec_sfive_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_SSEVEN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATS7 command. ATS7 is used to set the 
  number of seconds for establishing an end to end data connection. 
  
DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  None.
  
===========================================================================*/
dsat_result_enum_type dsatparm_exec_sseven_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
/*===========================================================================

FUNCTION DSATPARM_EXEC_SZERO_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATS0 command. ATS0 is used to set the 
  number of rings for auto answering the call. In case if serializer
  is enabled, SIOLIB has to know whic port enabled the auto answer
  option.
  
DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  Incoming CS Data calls are assigned to the port setting ATS0 command.
  
===========================================================================*/
dsat_result_enum_type dsatparm_exec_szero_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


#endif /* DSATPARM_H */
