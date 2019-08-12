#ifndef DSAT707PSTATS_H
#define DSAT707PSTATS_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode Protocol Statistics commands )
  processor.

EXTERNALIZED FUNCTIONS
dsat707_exec_qcrlpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD command.
  Dump RLP protocol statistics.

dsat707_exec_qcrlpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR command.
  Reset RLP protocol statistics.
  
dsat707_exec_qcrlpd33_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD33 command.
  Dump RLP 3 protocol statistics.
  
dsat707_exec_qcrlpr33_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR33 command.
  Reset RLP 3 protocol statistics.
  
dsat707_exec_qcpppd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPD command.
  Dump PPP protocol statistics.
  
dsat707_exec_qcpppr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPR command.
  Reset PPP protocol statistics.
  
dsat707_exec_qcipd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPD command.
  Dump IP protocol statistics.
  
dsat707_exec_qcipr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPR command.
  Reset IP protocol statistics.
  
dsat707_exec_qcudpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPD command.
  Dump UDP protocol statistics.
  
dsat707_exec_qcudpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPR command.
  Reset UDP protocol statistics.
  
dsat707_exec_qctcpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPD command.
  Dump TCP protocol statistics.
  
dsat707_exec_qctcpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPR command.
  Reset TCP protocol statistics.
  
  
Copyright (c) 1995,1996,1997 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707pstats.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/12   sk      Feature cleanup.
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#ifdef  FEATURE_DS_PSTATS

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCRLPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD command.
  Dump RLP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcrlpd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcrlpd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCRLPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR command.
  Reset RLP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcrlpr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcrlpr_cmd */

#ifdef FEATURE_DS_RLP3
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCRLPD33_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD33 command.
  Dump RLP 3 protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcrlpd33_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcrlpd33_cmd */
#endif /* FEATURE_DS_RLP3 */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCRLPR33_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR33 command.
  Reset RLP 3 protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcrlpr33_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcrlpr33_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCPPPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPD command.
  Dump PPP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcpppd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcpppd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCPPPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPR command.
  Reset PPP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcpppr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcpppr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCIPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPD command.
  Dump IP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcipd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcipd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCIPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPR command.
  Reset IP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcipr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcipr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCUDPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPD command.
  Dump UDP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcudpd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcudpd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCUDPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPR command.
  Reset UDP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qcudpr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qcudpr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCTCPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPD command.
  Dump TCP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qctcpd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qctcpd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCTCPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPR command.
  Reset TCP protocol statistics.
  
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
dsat_result_enum_type dsat707_exec_qctcpr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_qctcpr_cmd */

#endif /* FEATURE_DS_PSTATS */
#endif /* FEATURE_DATA_IS707 */
#endif /* DSAT707PSTATS_H */
