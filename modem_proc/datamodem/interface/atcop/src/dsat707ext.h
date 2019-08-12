#ifndef DSAT707EXT_H
#define DSAT707EXT_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode extended commands ) processor.

EXTERNALIZED FUNCTIONS

dsat707_exec_pzid_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+PZID command.This gives the 
  information if the Packet Zone ID is set in the Extended System
  Parameter Message.

dsat707_exec_cfg_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CFG="string" command. The string
  will be stored by the MT2 and sent to the base station prior
  to dialing.

dsat707_exec_cad_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CAD? command.
  This command queries analog or digital service.

dsat707_exec_cbip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CBIP? command.
  This command returns the base station's IP address and is read-only.

dsat707_exec_cdr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CDR command.
  This command controls whether the extended format +CDR: intermediate
  result code is transmitted by the MT2.

dsat707_exec_cds_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CDS command.
  This command controls the V.42bis data compression function on
  the Um interface.

dsat707_exec_cfc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CFC=<value> command.

dsat707_exec_cdv_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CDV command.

dsat707_exec_chv_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CHV=<value> command.

dsat707_exec_chv0_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CHV0=<value> command.

dsat707_exec_cmip_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CMIP? command.
  This command returns the mobile station IP address and is read only.

dsat707_exec_cmux_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+QCMUX=<n> command.
  This command selects multiplex option.

dsat707_exec_cqd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CQD=<n> command.
  This command sets the command state inactivity timer.

dsat707_exec_crc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CRC=<n> command.
  This command enables/disables cellular result codes.

dsat707_exec_crm_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CRM=<n> command.
  This command sets the Rm interface protocol.

dsat707_exec_csq_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CSQ? command.
  Query received signal quality. Returns the signal quality measure
  <SQM> and the frame error rate <FER>.

dsat707_exec_css_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CSS? command.
  This command returns the serving system and is read only.

dsat707_exec_cta_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CTA = <value> command.
  Set/Read/Test Um packet data inactivity timer.

dsat707_exec_cxt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+CXT = <value> command.
  Set Cellular extension.

dsat707_exec_eb_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+EB command.
  Break handling in error control operation. This extended-format
  compound parameter is used to control the manner of V.42 operation
  on the PSTN link (if present in the IWF).

dsat707_exec_efcs_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+EFCS command.
  This extended format numeric parameter controls the use of the
  32-bit frame check sequence option in V.42 on the PSTN link.

dsat707_exec_er_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ER command.
  Error control reporting. This extended format numeric parameter
  controls whether the extended format +ER: intermediate result
  code is transmitted from the IWF over the Um interface.

dsat707_exec_esr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ESR command.
  This extended format numeric parameter controls the use of
  selective repeat option in V.42 on the PSTN link.

dsat707_exec_etbm_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ETBM command.
  This extended format compound parameter controls the handling of
  data remaining in IWF buffers upon service termination.

dsat707_exec_goi_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+GOI command.
  Returns null.

dsat707_exec_ilrr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+ILRR command.
  Local Rate Reporting. Mobile accepts only "OFF".

dsat707_exec_ma_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MA command.
  Modulation Automode control.

dsat707_exec_mr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MR command.
  Modulation Reporting control.

dsat707_exec_ms_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MS command.
  Modulation Selection. Controls the manner of operation of
  the modulation capabilities of the IWF.

dsat707_exec_mv18r_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MV18R command.
  V.18 reporting control.

dsat707_exec_mv18s_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+MV18S command.
  V.18 selection.

Copyright (c) 1995 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707ext.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
06/27/14   pg      Added support for Dynamic ATCoP.
05/08/13   tk      Renamed 3GPP2 +CMUX command to +QCMUX command.
03/07/12   sk      Made CLCC common for all modes.
01/19/12   sk      Feature cleanup.
06/04/10   bs      MMGSDI SESSION Migration.
05/10/10   kk      Added support for +CIMI, ^GSN, ^MEID, ^CGSN, +MDN.
01/06/10   ua      Added support for +PZID command.
01/12/10   vg      Added support for +CPIN,^CPIN and +CLCK for RUIM cards.
03/04/09   sa      AU level CMI modifications.
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707

#include "dsati.h"
/*Macro Defiantion*/
#define DSAT_VALIDATE_SET_707_PKTMGR(val_type,dflm) \
  ( ((tok_ptr->op == NA)|| ((tok_ptr->op == (NA|EQ|AR)) && \
    (tok_ptr->args_found == 1 && (VALID_TOKEN(0)) && \
     ATOI_OK == dsatutil_atoi(&temp_val,tok_ptr->arg[0],10) && (temp_val <= dflm))))&& \
    (TRUE == ds707_pkt_mgr_set_1x_profile_val(val_type, (void *)&temp_val , 0))) \


/*===========================================================================
  FUNCTION DSAT707_EXEC_CFG_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CFG="string" command. The string
  will be stored by the MT2 and sent to the base station prior
  to dialing.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707_exec_cfg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cfg_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_PZID_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+PZID command. This gives the 
  information if the Packet Zone ID is set in the Extended System
  Parameter Message. 

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
dsat_result_enum_type dsat707_exec_pzid_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *resbuff_ptr            /*  Place to put response       */
); /* dsat707_exec_pzid_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CAD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CAD? command.
  This command queries analog or digital service.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  Returns:
  0: if no service is available.
  1: if CDMA digital service is available.
  2: if TDMA digital service is available.
  3: if Analog service is available.
===========================================================================*/
dsat_result_enum_type dsat707_exec_cad_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cad_cmd */

#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
  FUNCTION DSAT707_EXEC_CBIP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CBIP? command.
  This command returns the base station's IP address and is read-only.

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
dsat_result_enum_type dsat707_exec_cbip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cbip_cmd */
#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CDR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CDR command.
  This command controls whether the extended format +CDR: intermediate
  result code is transmitted by the MT2.

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
dsat_result_enum_type dsat707_exec_cdr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cdr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CDS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CDS command.
  This command controls the V.42bis data compression function on
  the Um interface.

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
dsat_result_enum_type dsat707_exec_cds_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cds_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CFC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CFC=<value> command.
  Um interface fax compression:
  0 - No compression.
  1 - V.42bis compression with parameters as set by the +CDS command.
  2 - Modifies the modified read compression.

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
dsat_result_enum_type dsat707_exec_cfc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cfc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CDV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CDV=<value> command.
  Initiate voice call.

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
dsat_result_enum_type dsat707_exec_cdv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_chv_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CHV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CHV=<value> command.
  Hangup voice:
  0 - Hangup voice call.
  1-255 - reserved.

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
dsat_result_enum_type dsat707_exec_chv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_chv_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CHV0_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CHV0=<value> command.
  Hangup voice:
  0 - Hangup voice call.
  1-255 - reserved.

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
dsat_result_enum_type dsat707_exec_chv0_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_chv0_cmd */

#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
  FUNCTION DSAT707_EXEC_CMIP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CMIP? command.
  This command returns the mobile station IP address and is read only.

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
dsat_result_enum_type dsat707_exec_cmip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cmip_cmd */
#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CMUX_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+QCMUX=<n> command.
  This command selects multiplex option:
  1 - Multiplex option 1.
  2 - Multiplex option 2.

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
dsat_result_enum_type dsat707_exec_cmux_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cmux_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CQD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CQD=<n> command.
  This command sets the command state inactivity timer:
  0 - Ignored.
  1-255 - Release call after 5x<n> seconds have elapsed without
  activity.  The default <n> shall be 10, corresponding to 50 seconds.

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
dsat_result_enum_type dsat707_exec_cqd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cqd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CRC_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CRC=<n> command.
  Cellular result codes:
  0 - Disable Cellular Result Codes.
  1 - Enable Cellular Result Codes.

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
dsat_result_enum_type dsat707_exec_crc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_crc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CRM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CRM=<n> command.
  Set Rm interface protocol:
  0 - Asynchronous data or fax.
  1 - Packet data service, Relay layer Rm interface.
  2 - Packet data service, Network layer Rm interface, PPP.
  3 - Packet data service, Network layer Rm interdace, SLIP.
  4 - STU-III service.
  5-127 - Reserved for future use.
  128-255 - Reserved for manufacturer specific use.
  Default is 0.

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
dsat_result_enum_type dsat707_exec_crm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_crm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CSQ_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CSQ? command.
  Query received signal quality. Returns the signal quality measure <SQM>
  and the frame error rate <FER> as follows:
  <SQM>:
  0-31 - <SQM>
  99 - <SQM> not known or is detectable.
  <FER>:
  0 - <0.01%
  1 - 0.01 to < 0.1%
  2 - 0.1  to < 0.5%
  3 - 0.5  to < 1.0%
  4 - 1.0  to < 2.0%
  5 - 2.0  to < 4.0%
  6 - 4.0  to < 8.0%
  7 - >= 8.0%
  99 - <FER> is not known or is not detectable.

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
dsat_result_enum_type dsat707_exec_csq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_csq_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CSS? command.
  This command returns the serving system and is read only.  Returns
  <Band Class>, <Band>, <SID>
  Band Class:
  C - The Mobile Station is registered with a cellular system.
  P - The Mobile Station is registered with a PCS system.
  Band:
  CA - The Mobile Station is registeres with a cellular A-band system.
  CB - The Mobile Station is registeres with a cellular B-band system.
  PA - The Mobile Station is registeres with a PCS A-band system.
  PB - The Mobile Station is registeres with a PCS B-band system.
  PC - The Mobile Station is registeres with a PCS C-band system.
  PD - The Mobile Station is registeres with a PCS D-band system.
  PE - The Mobile Station is registeres with a PCS E-band system.
  PF - The Mobile Station is registeres with a PCS F-band system.
  Z - The Mobile Station is not registered.
  SID:
  0-16383 - The mobile station is registered with the system indicated.
  99999 - The mobile station is not registered.

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
dsat_result_enum_type dsat707_exec_css_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_css_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CTA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CTA = <value> command.
  Set/Read/Test Um packet data inactivity timer.
  0 - Traffic channel not released during inactivity periods.
  1-255 - Release traffic channel after <value> 1-second intervals have
  elapsed since last sending or receiving RLP data frames on the Um
  interface.
  20 - (default)

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
dsat_result_enum_type dsat707_exec_cta_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_cta_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CXT_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CXT = <value> command.
  Cellular extension.
  0 - Do not pass unrecognized commands to IWF.
  1 - When detectomg an unrecognized command, open transport layer
  connection and pass unrecognized commands to the IWF.

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
dsat_result_enum_type dsat707_exec_cxt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
);  /* dsat707_exec_cxt_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_EB_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+EB command.
  Break handling in error control operation. This extended-format
  compound parameter is used to control the manner of V.42 operation
  on the PSTN link (if present in the IWF).

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
dsat_result_enum_type dsat707_exec_eb_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_eb_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_EFCS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+EFCS command.
  This extended format numeric parameter controls the use of the
  32-bit frame check sequence option in V.42 on the PSTN link.

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
dsat_result_enum_type dsat707_exec_efcs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_efcs_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ER_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ER command.
  Error control reporting. This extended format numeric parameter
  controls whether the extended format +ER: intermediate result
  code is transmitted from the IWF over the Um interface.

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
dsat_result_enum_type dsat707_exec_er_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_er_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ESR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ESR command.
  This extended format numeric parameter controls the use of
  selective repeat option in V.42 on the PSTN link.

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
dsat_result_enum_type dsat707_exec_esr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_esr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ETBM_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ETBM command.
  This extended format compound parameter controls the handling of
  data remaining in IWF buffers upon service termination.

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
dsat_result_enum_type dsat707_exec_etbm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_etbm_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_GOI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+GOI command.
  Returns null.

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
dsat_result_enum_type dsat707_exec_goi_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_goi_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_ILRR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+ILRR command.
  Local Rate Reporting. Mobile accepts only "OFF".

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
dsat_result_enum_type dsat707_exec_ilrr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ilrr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MA_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MA command.
  Modulation Automode control.

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
dsat_result_enum_type dsat707_exec_ma_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ma_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MR command.
  Modulation Reporting control.

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
dsat_result_enum_type dsat707_exec_mr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_mr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MS command.
  Modulation Selection. Controls the manner of operation of
  the modulation capabilities of the IWF.

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
dsat_result_enum_type dsat707_exec_ms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_ms_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MV18R_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MV18R command.
  V.18 reporting control.

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
dsat_result_enum_type dsat707_exec_mv18r_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_mv18r_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_MV18S_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+MV18S command.
  V.18 selection.

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
dsat_result_enum_type dsat707_exec_mv18s_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
); /* dsat707_exec_mv18s_cmd */


#ifdef FEATURE_DSAT_CDMA_PIN
/*===========================================================================

FUNCTION DSAT707_MMGSDI_REQ_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat707_mmgsdi_req_cb
(
  mmgsdi_return_enum_type status, /* MMGSDI return status */
  mmgsdi_cnf_enum_type    cnf,    /* Type of command confirmation */
  const mmgsdi_cnf_type *cnf_ptr  /* Pointer to confirmation data */
);

/*===========================================================================

FUNCTION DSAT707_EXEC_CPIN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPIN  or ^CPIN command.
  This set commands sends to the ME a password which is necessary before 
  operation.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :      if there was any problem in executing the command
    DSAT_OK :         if it is a success.
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707_exec_cpin_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSAT707_EXEC_CLCK_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
   it. It executes +CLCK to lock/unlock and query SC and FD facility.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was an problem in executing the command
    DSAT_CMD_ERR_RSP : if there was a problem during aynchronous execution
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsat707_exec_clck_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSAT707_CLCK_ABORT_HANDLER

DESCRIPTION
  This function does nothing but return a result to indicate the
  +CLCK command should be aborted.  It should be
  installed in the command table as the abort handler function for
  abortable commands.

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsat707_clck_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
);

/*===========================================================================

FUNCTION DSAT707_EXEC_CPWD_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
   it. It executes +CPWD to change SIM PIN and SIM PIN2.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was an problem in executing the command
    DSAT_CMD_ERR_RSP : if there was a problem during aynchronous execution
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cpwd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSAT707_STATUS_EVENT_HANDLER_EX

DESCRIPTION
  This function is called after the MMGSDI fulfilled the request to
  get the pin status.  The MMGSDI reply is in cmd_status.

 
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD -- if the request is sent to mmgsdi to verify pin
                  
  DSAT_CMD_ERR_RSP -- if a ME error happens. A CME error is returned
                   to TE.
  DSAT_ERROR  -- Any other ERROR

  DSAT_ASYNC_EVENT -- if no action needs to be taken..
    
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707_status_event_handler_ex
(
   ds_at_cmd_status_type *cmd_info
);

/*===========================================================================

FUNCTION DSAT707_STATUS_EVENT_HANDLER

DESCRIPTION
  This function is called after the MMGSDI fulfilled the request to
  get the pin status.  The MMGSDI reply is in cmd_status.

 
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD -- if the request is sent to mmgsdi to verify pin
                  
  DSAT_CMD_ERR_RSP -- if a ME error happens. A CME error is returned
                   to TE.
  DSAT_ERROR  -- Any other ERROR

  DSAT_ASYNC_EVENT -- if no action needs to be taken..
    
SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707_status_event_handler
(
   ds_at_cmd_status_type *cmd_info
);
#endif /* FEATURE_DSAT_CDMA_PIN */

/*===========================================================================

FUNCTION DSAT707_EXEC_CIMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CIMI command.
  This command compiles the International Mobile Subscriber Identity (IMSI)
  number of the device from the relevant NV items and prints out the 
  information.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707_exec_cimi_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);
/*===========================================================================
FUNCTION DSAT707_EXEC_MDN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +MDN command.
  This command returns mobile directory number.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat707_exec_mdn_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DSAT707EXT_H */
