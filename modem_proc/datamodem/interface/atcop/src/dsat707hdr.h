#ifndef DSAT707HDR_H
#define DSAT707HDR_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode HDR commands ) processor.

EXTERNALIZED FUNCTIONS
dsat707_exec_qchdrc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRC command.
  For 1xEV select a fixed DCR value. The default value is F, for
  variable rate mode.

dsat707_exec_qchdrr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRR command.
  For 1xEV, select a fixed reverse rate. Default value is F,
  for variable rate mode.

dsat707_exec_qchdrt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRT command.
  For 1xEV, toggle early termination. Default value is 1.
  
Copyright (c) 1995 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707hdr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/12   sk      Feature cleanup.
03/04/09   sa      AU level CMI modifications.
10/30/06   spn     Changed Featurization from DS_AT_TEST_ONLY to 
                   HDR_AT_TEST_ONLY
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#ifdef FEATURE_HDR

#include "dsati.h"

#ifdef FEATURE_HDR_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCHDRC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRC command.
  For 1xEV select a fixed DCR value. The default value is F, for
  variable rate mode.
  0 - Null Rate
  1 - 38.4 kbps
  2 - 76.8 kbps
  3 - 153.6 kbps
  4 - 307.2 kbps (short)
  5 - 307.2 kbps (long)
  6 - 614.4 kbps (short)
  7 - 614.4 kbps (long)
  8 - 921.6 kbps
  9 - 1288.8 kbps (short)
  A - 1288.8 kbps (long)
  B - 1843.2 kbps
  C - 2457.6 kbps
  F - Variable Rate Mode
  
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
dsat_result_enum_type dsat707_exec_qchdrc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qchdrc_cmd */
#endif /* FEATURE_HDR_AT_TEST_ONLY */

#ifdef FEATURE_HDR_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCHDRR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRR command.
  For 1xEV, select a fixed reverse rate. Default value is F,
  for variable rate mode.
  1 - 9.6 kbps
  2 - 19.2 kbps
  3 - 38.4 kbps
  4 - 76.8 kbps (short)
  5 - 153.6 kbps (long)
  F - Variable Rate Mode
  
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
dsat_result_enum_type dsat707_exec_qchdrr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qchdrr_cmd */
#endif /* FEATURE_HDR_AT_TEST_ONLY */

#ifdef FEATURE_HDR_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCHDRT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRT command.
  For 1xEV, toggle early termination. Default value is 1.
  0 - Early termination disabled.
  1 - Early termination enabled.
  
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
dsat_result_enum_type dsat707_exec_qchdrt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qchdrt_cmd */
#endif /* FEATURE_HDR_AT_TEST_ONLY */

#ifdef FEATURE_HDR_AT_TEST_ONLY
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCHDRET_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRET command.
  For 1xEV Rev A reverse link, force RL packet termination to indicated
  subpacket. The value 0 refers to default behavior.
  
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
dsat_result_enum_type dsat707_exec_qchdret_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                 /*  Place to put response       */
); /* dsat707_exec_qchdret_cmd */
#endif /* FEATURE_HDR_AT_TEST_ONLY */


#endif /* FEATURE_HDR */
#endif /* FEATURE_DATA_IS707 */

#endif /* DSAT707HDR_H */
