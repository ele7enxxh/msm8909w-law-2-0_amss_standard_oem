#ifndef DSAT707FAX_H
#define DSAT707FAX_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode fax commands ) processor.

EXTERNALIZED FUNCTIONS

dsat707_exec_faa_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FAA command.
  Adaptive answer parameter.

dsat707_exec_fap_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FAP command.
  Addressing and polling capabilities parameter.

dsat707_exec_fbo_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FBO command.
  Phase-C data bit order parameter.

dsat707_exec_fbs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FBS command.
  Buffer size, read only parameter.
  
dsat707_exec_fbu_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FBU command.
  HDLC frame reporting parameter.

dsat707_exec_fcc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCC command.
  DCE capabilities parameter.

dsat707_exec_fcq_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCQ command.
  Copy quality checking parameter.

dsat707_exec_fcr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCR command.
  Capability to receive parameter.
  
dsat707_exec_fct_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FCT command.
  DTE Phase-C timeout parameter.
  
dsat707_exec_fdr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FDR command.
  Receive Phase-C data.

dsat707_exec_fea_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FEA command.
  Phase-C received EOL alignment parameter.

dsat707_exec_ffc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FFC command.
  Format conversion parameter.
  
dsat707_exec_fhs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FHS command.
  Call-termination status parameter.
  
dsat707_exec_fie_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FIE command.
  Procedure interrupt enable parameter.
  
dsat707_exec_fip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FIP command.
  Initialize fax parameters.
  
dsat707_exec_fis_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FIS command.
  Current session negotiation parameters.
  
dsat707_exec_fks_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FKS command.
  Terminate session.
  
dsat707_exec_fli_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FLI command.
  Local id string parameter (TSI or CSI).
  
dsat707_exec_flo_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FLO command.
  Flow control select parameter.
  
dsat707_exec_flp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FLP command.
  Indicate document to poll parameter.
  
dsat707_exec_fms_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FMS command.
  Minimum Phase-C speed parameter.
  
dsat707_exec_fnr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FNR command.
  Negotiation message reporting control parameters.
  
dsat707_exec_fns_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FNS command.
  Non standard frame FIF parameter.
  
dsat707_exec_fpa_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPA command.
  Selective polling address parameter.
  
dsat707_exec_fpi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPI command.
  Local polling ID string parameter.
  
dsat707_exec_fpp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPP command.
  Packet protocol control parameter.
  
dsat707_exec_fpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPR command.
  Serial port rate control parameter.
  
dsat707_exec_fps_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPS command.
  Page status parameter.
  
dsat707_exec_fpw_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FPW command.
  Password parameter (sending or polling).
  
dsat707_exec_frq_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FRQ command.
  Receive quality threshold parameters.
  
dsat707_exec_fry_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FRY command.
  ECM retry value parameter.
  
dsat707_exec_fsa_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FSA command.
  Subaddress parameter.
  
dsat707_exec_fsp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+FSP command.
  Request to poll parameter.
  

  
Copyright (c) 1995 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707fax.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/12   sk      Feature cleanup.
03/04/09   sa      AU level CMI modifications.
04/11/07   ua      Modifications as per KDDI requirements. 
04/03/02   rsl     Initial version of file.
08/13/03   rsl     Moved at+fclass to dsatctab.c

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707_ASYNC

#include "dsati.h"

/*===========================================================================
  FUNCTION DSAT707_EXEC_FAA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FAA command.
  Adaptive answer parameter.
  
  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
      
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_faa_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_faa_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FAP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FAP command.
  Addressing and polling capabilities parameter.
  
  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
      
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fap_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fap_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FBO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FBO command.
  Phase-C data bit order parameter.
  
  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
      
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_fbo_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_dbo_cmd */

/*==========================================================================
  FUNCTION DSAT707_EXEC_FBS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FBS command.
  Buffer size, read only parameter.
  
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
dsat_result_enum_type dsat707_exec_fbs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fbs_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FBU_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FBU command.
  HDLC frame reporting parameter.
  
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
dsat_result_enum_type dsat707_exec_fbu_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fbu_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FCC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCC command.
  DCE capabilities parameter.
  
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
dsat_result_enum_type dsat707_exec_fcc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fcc_cmd */


/*===========================================================================
  FUNCTION DSAT707_EXEC_FCQ_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCQ command.
  Copy quality checking parameter.
  
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
dsat_result_enum_type dsat707_exec_fcq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fcq_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FCR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCR command.
  Capability to receive parameter.
  
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
dsat_result_enum_type dsat707_exec_fcr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fcr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FCT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FCT command.
  DTE Phase-C timeout parameter.
  
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
dsat_result_enum_type dsat707_exec_fct_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fct_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FDR command.
  Receive Phase-C data.
  
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
dsat_result_enum_type dsat707_exec_fdr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fdr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FDT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FDT command.
  Receive Phase-C data.
  
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
dsat_result_enum_type dsat707_exec_fdt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fdr_cmd */
/*===========================================================================
  FUNCTION DSAT707_EXEC_FEA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FEA command.
  Phase-C received EOL alignment parameter.
  
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
dsat_result_enum_type dsat707_exec_fea_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fea_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FFC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FFC command.
  Format conversion parameter.
  
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
dsat_result_enum_type dsat707_exec_ffc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ffc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FHS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FHS command.
  Call-termination status parameter.
  
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
dsat_result_enum_type dsat707_exec_fhs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fhs_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FIE_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FIE command.
  Procedure interrupt enable parameter.
  
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
dsat_result_enum_type dsat707_exec_fie_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fie_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FIP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FIP command.
  Initialize fax parameters.
  
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
dsat_result_enum_type dsat707_exec_fip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fip_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FIS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FIS command.
  Current session negotiation parameters.
  
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
dsat_result_enum_type dsat707_exec_fis_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fis_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FKS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FKS command.
  Terminate session.
  
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
dsat_result_enum_type dsat707_exec_fks_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fks_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FLI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FLI command.
  Local id string parameter (TSI or CSI).
  
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
dsat_result_enum_type dsat707_exec_fli_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fli_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FLO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FLO command.
  Flow control select parameter.
  
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
dsat_result_enum_type dsat707_exec_flo_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_flo_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FLP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FLP command.
  Indicate document to poll parameter.
  
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
dsat_result_enum_type dsat707_exec_flp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_flp_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FMS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FMS command.
  Minimum Phase-C speed parameter.
  
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
dsat_result_enum_type dsat707_exec_fms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fms_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FNR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FNR command.
  Negotiation message reporting control parameters.
  
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
dsat_result_enum_type dsat707_exec_fnr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fnr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FNS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FNS command.
  Non standard frame FIF parameter.
  
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
dsat_result_enum_type dsat707_exec_fns_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fns_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPA command.
  Selective polling address parameter.
  
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
dsat_result_enum_type dsat707_exec_fpa_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fpa_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPI command.
  Local polling ID string parameter.
  
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
dsat_result_enum_type dsat707_exec_fpi_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fpi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPP command.
  Packet protocol control parameter.
  
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
dsat_result_enum_type dsat707_exec_fpp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fpp_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPR command.
  Serial port rate control parameter.
  
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
dsat_result_enum_type dsat707_exec_fpr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fpr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPS command.
  Page status parameter.
  
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
dsat_result_enum_type dsat707_exec_fps_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fps_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FPW_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FPW command.
  Password parameter (sending or polling).
  
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
dsat_result_enum_type dsat707_exec_fpw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fpw_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FRQ_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FRQ command.
  Receive quality threshold parameters.
  
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
dsat_result_enum_type dsat707_exec_frq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_frq_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FRY_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FRY command.
  ECM retry value parameter.
  
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
dsat_result_enum_type dsat707_exec_fry_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fry_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FSA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FSA command.
  Subaddress parameter.
  
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
dsat_result_enum_type dsat707_exec_fsa_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fsa_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_FSP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+FSP command.
  Request to poll parameter.
  
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
dsat_result_enum_type dsat707_exec_fsp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_fsp_cmd */

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* DSAT707FAX_H */
