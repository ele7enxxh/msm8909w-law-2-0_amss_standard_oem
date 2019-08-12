/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific extended
  AT commands.

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
  and executes it. It executes AT+CDV=<value> command.

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

Copyright (c) 1995 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707ext.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
06/27/14   pg      Added support for Dynamic ATCoP.
05/23/14   pg      Fixed LLVM warnings.
04/17/14   sc      Fixed ATCoP KW errors.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
12/30/13   sc      Fixed static code bugs.
08/19/13   tk      Fixed +CDV issue with 1x CSFB voice calls in LTE.
05/08/13   tk      Renamed 3GPP2 +CMUX command to +QCMUX command.
02/22/13   tk      ATCoP - CM interface changes for Triton DSDA.
11/30/12   tk      ATCoP changes for Triton DSDA.
05/18/12   tk      Migrated to MSG 2.0 macros
03/07/12   sk      Made CLCC common for all modes.
09/27/11   ht      Included dsut header to support offtarget stubs.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
05/13/11   ad      Fixed +CPIN in CDMA mode. 
02/24/11   ttv     Fixed compilation warnings.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
11/29/10   bs      Fixed +CIMI issue in 1X mode.
09/13/10   ad      Fixed +PZID issue. 
07/27/10   ps      Added code changes to decouple Async and related modules
                   from INET.
06/29/10   kk      Removing bbver.h dependency for BOOT_BLOCK_VERSION.
06/04/10   bs      MMGSDI SESSION Migration.
05/10/10   kk      CDMA PIN fixes.
05/10/10   kk      Added support for +CIMI, ^GSN, ^MEID, ^CGSN, +MDN.
01/06/10   ua      Added support for +PZID command.
01/12/10   vg      Add support for +CLCC command
01/12/10   vg      Added support for +CPIN,^CPIN and +CLCK for RUIM cards.
08/27/09   sa      Removing hs.h header file inclusion.
12/12/08   ua      Fixed Off target lint errors.
08/27/08   bs      Fixed +CRM issue for JCDMA builds.
02/18/08   sa      Added modification for Passport Feature support.
05/10/07   ac      Lint fix
03/20/07  as/sq    Fixed Lint error and banned apis
04/27/05   gr      Renamed the list_type in dsati.h to ds_ati_list_type and
                   moved it into this file 
09/13/04   snb     Changes due to ATCOP API change.
08/18/03   sv      Modified code to use V4mappedV6 addresses.
03/23/03   rsl/ak  Fixes for AT+CMUX
02/18/03   rsl     Changes relating to cm end_params.
04/03/02   rsl     Initial version of file.

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "cm.h"
#include "db.h"
#include "dsati.h"
#include "dsatme.h"
#include "dsat707ext.h"
#include "dsat707extctab.h"
#include "dsat707util.h"
#include "dsatparm.h"
#include "mobile.h"
#include "msg.h"
#include "snm.h"
#include "ps_iface.h"
#include "ps_tcp_async.h"
#include "ps_in.h"
#include "ds3gmgrint.h"
#include "dsatvoice.h"
#include "dsatcmif.h"
#include "ds_1x_profile.h"
#include "ds707_pkt_mgr.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include <stringl/stringl.h>
#include "ds707_epzid.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DSAT_CDMA_PIN
#include "dsatme.h"
#endif /* FEATURE_DSAT_CDMA_PIN */

#ifdef FEATURE_DSAT_EXTENDED_CMD
#include "cmutil.h"
#endif /*FEATURE_DSAT_EXTENDED_CMD*/

/*===========================================================================
                        TYPEDEFS AND VARIABLES
===========================================================================*/
#define RF_FM_RSSI_SPAN             250
#define CDMA_RSSI_THRESH            125
#define CDMA_RSSI_SPAN              (CDMA_RSSI_THRESH - 75)

#ifdef FEATURE_DSAT_CDMA_PIN

/*---------------------------------------------------------------------------
  Personal Identity Number definitions
---------------------------------------------------------------------------*/
/* Required code table according to Section 8.3 of 3GPP TS 27.007. */
/* Order of records dictates precedence of reporting.              */
static const struct {
  uint32  req_code;
  char   *pin_text;
  char   *puk_text; 
} cpin_707req_table[] =
{
  { (uint32)DSAT_CPIN_SIM_PIN_REQUIRED,         "SIM PIN",       "SIM PUK" },
  { (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED,        "SIM PIN2",      "SIM PUK2" },
  { (uint32)DSAT_CPIN_NONE_REQUIRED,            "READY",         "READY" },
  { (uint32)DSAT_CPIN_MAX,                       NULL,            NULL }
};



/* Bit array for PIN status */
extern uint32 dsat707_pin_required;

/* Bit array for PIN blocked status: only the bits indicating a PIN apply to
   this mask, not "change pending" or "none required" bits */
extern uint32 dsat707_pin_blocked;

/* Indicates if SIM PIN2 status should be reported by +CPIN command:
   "SIM PIN2" and "SIM PUK2" codes is returned only when the last
   executed command resulted in PIN2 authentication failure */
boolean dsat707_mask_pin2_state = TRUE;

/* PIN error tracking (assume no SIM at initialization) */
extern dsat_cme_error_e_type dsat707_pin_error;

/*--------------------------------------------------------------------------
  Macros to flag PIN state setting
---------------------------------------------------------------------------*/
#define IS_707_PIN_STATE(state) \
    ((uint32)state == (dsat707_pin_required & (uint32)state))
#define IS_707_PUK_STATE(state) \
    ((uint32)state == (dsat707_pin_blocked & (uint32)state))

  /* Macros to ease use of GSDI functions that change name and parms 
     for dual slot use */

  #define GET_707_ALL_PIN_STATUS( client_ref ) \
    mmgsdi_status = mmgsdi_session_get_all_pin_status \
                          (dsat_get_current_1x_session_id(), \
                          dsat707_mmgsdi_req_cb,\
                          (mmgsdi_client_data_type)client_ref); 



  #define VERIFY_707_PIN( pin_id, pin_data, client_ref ) \
    mmgsdi_status = mmgsdi_session_verify_pin\
                         (dsat_get_current_1x_session_id(), \
                          pin_id, \
                          pin_data, \
                          dsat707_mmgsdi_req_cb, \
                          (mmgsdi_client_data_type)client_ref);

#define ENABLE_707_PIN_STATUS(  pin_id, pin_data, client_ref ) \
  { \
    mmgsdi_status = mmgsdi_session_enable_pin \
                         (dsat_get_current_1x_session_id(), \
                          pin_id, \
                          pin_data, \
                          dsat707_mmgsdi_req_cb, \
                          (mmgsdi_client_data_type)client_ref); \
  } 
#define DISABLE_707_PIN_STATUS(  pin_id, pin_data, client_ref ) \
  { \
    mmgsdi_status = mmgsdi_session_disable_pin \
                         (dsat_get_current_1x_session_id(), \
                          pin_id, \
                          MMGSDI_PIN_NOT_REPLACED_BY_UNIVERSAL, \
                          pin_data, \
                          dsat707_mmgsdi_req_cb, \
                          (mmgsdi_client_data_type)client_ref); \
  }

  #define UNBLOCK_707_PIN( pin_id, old_pin_data, new_pin_data, client_ref ) \
    mmgsdi_status = mmgsdi_session_unblock_pin \
                           (dsat_get_current_1x_session_id(), \
                            pin_id, \
                            old_pin_data, \
                            new_pin_data, \
                            dsat707_mmgsdi_req_cb, \
                            (mmgsdi_client_data_type)client_ref); 

  #define CHANGE_707_PIN(pin_id, old_pin_data, new_pin_data, client_ref ) \
    mmgsdi_status = mmgsdi_session_change_pin \
                           (dsat_get_current_1x_session_id(), \
                            pin_id, \
                            old_pin_data, \
                            new_pin_data, \
                            dsat707_mmgsdi_req_cb, \
                            (mmgsdi_client_data_type)client_ref); 

LOCAL dsat_result_enum_type ext_cpin_unblock_pin
(
  const tokens_struct_type *tok_ptr,       /* Command tokens from parser  */
  const mmgsdi_pin_enum_type pin_id,       /* PIN identifier              */
  const mmgsdi_slot_id_enum_type slot_id   /* SIM slot identifier         */
);

LOCAL dsat_result_enum_type ext_report_clck_fdn_status
(
  ds_at_cmd_status_type *cmd_info_ptr
);

LOCAL dsat_result_enum_type ext_cpin_verify_pin_status
(
   ds_at_cmd_status_type *cmd_info_ptr
);

LOCAL dsat_result_enum_type ext_process_cpin_state
(
  const tokens_struct_type *tok_ptr       /*  Command tokens from parser  */
);

LOCAL char* ext_get_code_string
(
  void
);


#endif /* FEATURE_DSAT_CDMA_PIN */

#define DSAT_TA_UNKNOWN       129 /* 0x80|CM_TON_UNKNOWN      |CM_NPI_ISDN */
#define DSAT_TA_INTERNATIONAL 145 /* 0x80|CM_TON_INTERNATIONAL|CM_NPI_ISDN */
#define DSAT_TA_INTER_PREFIX  '+' /* ETSI international call dial prefix */

/*---------------------------------------------------------------------------
     Data type for a list strings. Used in function dsat_srch_list.    
---------------------------------------------------------------------------*/  
typedef byte ds_ati_list_type[][8];    

extern cm_call_id_type   voice_call_id;

LOCAL dsat_result_enum_type ds_atcop_put_cmux_arg
(
  const dsati_cmd_type     *tab_entry,  /*  Entry in parse table  */
  tokens_struct_type *tok_ptr     /*  Results of parse      */
) ;

LOCAL atoi_enum_type ds_atcop_srch_list
(
  dsat_num_item_type *val_arg_ptr,
  const ds_ati_list_type l,
  byte * arg
) ;

/* Constants for MIN conversion routines */
#define DSAT_IMSI_STR_MAX_LEN      16
#define DSAT_IMSI_MCC_OFFSET 0
#define DSAT_IMSI_MNC_OFFSET 3
#define DSAT_IMSI_MIN2_OFFSET 5
#define DSAT_IMSI_MIN1_OFFSET 8

LOCAL void dsat707ext_mobnum_pcs_to_text
(
  char *                  dest, 
  nv_mob_dir_data_type *  src
);

dsat_dial_val_e_type ds_atcop_proc_dial_str
(
  byte * in_ptr,
  byte  * dial_ptr,
  const byte * accept_list_ptr,
  const byte * reject_list_ptr
) ;

int ds_atcop_strchr
(
  const byte *s,
  const byte c,
  const uint16 len
) ;

/*===========================================================================
                   FUNCTION DEFINITIONS
===========================================================================*/
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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cfg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cfg_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cad_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  db_items_value_type dbi;
  dsat_result_enum_type result = DSAT_OK;
  byte  *rb_ptr = res_buff_ptr->data_ptr;
/*------------------------------------------------------------------------*/
  memset((void*)&dbi , 0 , sizeof(dbi) );
  db_get (DB_NO_SVC, &dbi);
  if (dbi.no_svc)
  {
    *rb_ptr++ = '0';
  }
  else
  {
    db_get (DB_DIGITAL, &dbi);
    *rb_ptr++ = (dbi.digital ? '1' : '3');
  }
  *rb_ptr= '\0';
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
 
  return result;

} /* dsat707_exec_cad_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cbip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  byte *rb_ptr = res_buff_ptr->data_ptr;
  uint32 remote_address;
  struct ps_in6_addr temp_address;
/*------------------------------------------------------------------------*/
  /*lint -save -e740 -e572 */
  temp_address = ps_tcp_async_get_remote_address();
  remote_address = 
    dss_ntohl(PS_IN6_GET_V4_FROM_V4_MAPPED_V6( &temp_address));
  /*lint -restore */
  if (mode == DSAT_CMD || mode == DSAT_ONLINE_CMD)
  {
	rb_ptr = dsatutil_itoa((word)((remote_address >> 24) & 0xFF),
						   rb_ptr,
						   10);
	*rb_ptr++ = '.';

	rb_ptr = dsatutil_itoa((word)((remote_address >> 16) & 0xFF),
						   rb_ptr,
						   10
						   );
	*rb_ptr++ = '.';

	rb_ptr = dsatutil_itoa((word)((remote_address >>  8) & 0xFF),
						   rb_ptr,
						   10
						   );
	*rb_ptr++ = '.';
	
	rb_ptr = dsatutil_itoa((word)((remote_address) & 0xFF),
						   rb_ptr,
						   10
						   );
  }
  else
  {
	*rb_ptr = '\0';  /*  and reset pointer     */
    /*  if no result to ouput */
  }
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return(DSAT_OK);
} /* dsat707_exec_cbip_cmd */
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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cdr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cdr_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cds_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cds_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cfc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cfc_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CDV_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CDV=<value> command.

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cdv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  const byte cdv_allowed    [] = "";
  const byte cdv_reject     [] = "TP!@";
  dsat_result_enum_type result = DSAT_OK;
  dsat_dial_val_e_type digit_mode = DSAT_DIAL_EMPTY;
  boolean                       call_status;
  sys_modem_as_id_e_type        subs_id = dsat_get_current_1x_subs_id();
  cm_num_s_type                 dial_num;
  cm_num_s_type                 calling_number;
  cm_cdma_orig_params_s_type    call_orig_params;
  cm_orig_alpha_s_type          alpha_buf;
  uint16                        dial_str_len;
  /*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA();

  if (tok_ptr->arg[0] == NULL) 
  {
    DS_AT_MSG0_ERROR("tok_ptr->arg[0]= NULL");
    return DSAT_ERROR;
  }
  
  digit_mode = ds_atcop_proc_dial_str (tok_ptr->arg[0],
                                       tok_ptr->arg[0],
                                       cdv_allowed, 
                                       cdv_reject);

  if ((digit_mode == DSAT_DIAL_EMPTY) || (digit_mode == DSAT_DIAL_ERROR))
  {
    DS_AT_MSG0_ERROR("empty or error in dial string");
    return DSAT_ERROR;
  }
  dial_str_len = (uint16)(strlen((char *)(tok_ptr->arg[0])));

  /* If not currently in a +CDV originated voice call */
  if (voice_call_id == CM_CALL_ID_INVALID)
  {
#ifdef FEATURE_DUAL_ACTIVE
    if (FALSE == dsatvoice_validate_call_cmd(subs_id, DSAT_CALL_CMD_ORIG))
    {
      return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
    }
#endif /* FEATURE_DUAL_ACTIVE */

    /* check number length */
    if (dial_str_len > CM_MAX_NUMBER_CHARS)
    {
      return DSAT_ERROR;
    } 

    /* set dial num */
    memset(&dial_num, 0, sizeof(cm_num_s_type));
    cm_num_init( &dial_num );
    if (digit_mode == DSAT_DIAL_DIGIT) 
    {
      /* dial_str_len is <= CM_MAX_NUMBER_CHARS (64), the cast is safe */
      cm_num_fill( &dial_num, tok_ptr->arg[0], (byte) dial_str_len, CM_DIGIT_MODE_4BIT_DTMF);
    }
    else if (digit_mode == DSAT_DIAL_ASCII_ABC) 
    {
      /* dial_str_len is <= CM_MAX_NUMBER_CHARS (64), the cast is safe */
      cm_num_fill( &dial_num, tok_ptr->arg[0], (byte) dial_str_len, CM_DIGIT_MODE_4BIT_DTMF);
    }

    /* We don't have calling number or alpha, 
      just init calling number and alpha */
    memset(&calling_number, 0, sizeof(cm_num_s_type));
    cm_num_init(&calling_number);
    memset(&alpha_buf, 0, sizeof(cm_orig_alpha_s_type));
    /* set mode specific parameters */
    memset(&call_orig_params,CM_CALL_CMD_PARAM_DEFAULT_VALUE,sizeof(call_orig_params));

    call_status = cm_mm_call_cmd_orig(dsatcmif_call_cmd_cb_func,
                                      NULL,
                                      dsatcm_client_id,
                                      CM_CALL_TYPE_VOICE,
                                      CM_SRV_TYPE_AUTOMATIC,
                                      &calling_number,
                                      &dial_num,
                                      &alpha_buf,
                                      &call_orig_params, /* cdma orig params */
                                      NULL, /* GW CS parameters */
                                      NULL, /* GW PS parameters */
                                      &voice_call_id
                                      );

    if ( call_status && (voice_call_id < CM_CALL_ID_MAX) )
    {
      dsatvoice_go_to_connecting(subs_id, voice_call_id);
      /* the command callback function will be called */
      DS_AT_MSG0_LOW("call status successful");
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      /* fail it, release atcop cmd processor */
      DS_AT_MSG0_ERROR("call status error");
          voice_call_id = CM_CALL_ID_INVALID;
      result = DSAT_ERROR;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_cdv_cmd */


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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_chv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
#ifdef FEATURE_DUAL_ACTIVE
  sys_modem_as_id_e_type subs_id = dsat_get_current_1x_subs_id();
#endif /* FEATURE_DUAL_ACTIVE */
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  if (tok_ptr->op != (NA))   /*  Has optional 0 "argument" without   */
                             /*  operator. No operators allowed      */
  {                          /*  '0' is in parse table               */
    result = DSAT_ERROR;
  }
  else if (mode ==  DSAT_CMD || mode == DSAT_ONLINE_CMD)
  {
#ifdef  FEATURE_CM_MULTIMODE
    /* If in a +CDV originated voice call */
    if(voice_call_id != CM_CALL_ID_INVALID)
    {
#ifdef FEATURE_DUAL_ACTIVE
      if (FALSE == dsatvoice_validate_call_cmd(subs_id, DSAT_CALL_CMD_END))
      {
        return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      }
#endif /* FEATURE_DUAL_ACTIVE */

      if (dsatcmif_end_calls(1,&voice_call_id))
      {
        /* the command callback function will be called */
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        DS_AT_MSG0_ERROR("End call request failed");
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_OK;
    }
#else
#ifdef FEATURE_DUAL_ACTIVE
    if (FALSE == dsatvoice_validate_call_cmd(subs_id, DSAT_CALL_CMD_END))
    {
      return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
    }
#endif /* FEATURE_DUAL_ACTIVE */

    cm_call_cmd_end( NULL, NULL, dsi_cm_client);
#endif /*FEATURE_CM_MULTIMODE*/
  }
  return result;
} /* dsat707_exec_chv_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_chv0_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  NOT_SUPPORTED_IN_JCDMA()
  return(dsat707_exec_chv_cmd (mode, tab_entry, tok_ptr, rb_ptr));
} /* dsat707_exec_chv0_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cmip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
  uint32 local_address;
  struct ps_in6_addr temp_address;
/*------------------------------------------------------------------------*/
  /*lint -save -e572 suppressing error 572*/
  NOT_SUPPORTED_IN_JCDMA()

  temp_address = ps_tcp_async_get_local_address();
  local_address = 
    dss_ntohl((uint32)PS_IN6_GET_V4_FROM_V4_MAPPED_V6( &temp_address ));

  if (mode == DSAT_CMD || mode == DSAT_ONLINE_CMD)
  {
    rb_ptr = dsatutil_itoa((word)((local_address >> 24)& 0xFF),
                            rb_ptr,
                            10
                          );
    *rb_ptr++ = '.';

    rb_ptr = dsatutil_itoa((word)((local_address >> 16)& 0xFF),
                           rb_ptr,
                           10
                          );
    *rb_ptr++ = '.';

    rb_ptr = dsatutil_itoa((word)((local_address >>  8)& 0xFF),
                           rb_ptr,
                           10
                          );
    *rb_ptr++ = '.';

    rb_ptr = dsatutil_itoa((word)(local_address & 0xFF),
                           rb_ptr,
                           10
                          );
  }
  else
  {
    *rb_ptr = '\0';  /*  and reset pointer     */
  }
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return result;
  /*lint -restore Restore lint error 572*/
} /* dsat707_exec_cmip_cmd */
#endif /* # FEATURE_DATA_IS707_ASYNC  */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cmux_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
#ifdef FEATURE_DS_IS707A_CMUX
  word                  fwd_mux;
  tokens_struct_type    tmp_token_struct;
  uint8                 old_mux_val[2];
#endif /* FEATURE_DS_IS707A_CMUX */
  dsat_result_enum_type result = DSAT_OK;
  uint8    temp_val[2];
/*------------------------------------------------------------------------*/
  
  /*------------------------------------------------------------------------
    Get the forward multiplex option.  Save the old value, as the below
    doesn't do bounds checking for MDR mux options
  ------------------------------------------------------------------------*/   
#ifdef FEATURE_DS_IS707A_CMUX
  ds707_pkt_mgr_get_1x_profile_val(DS707_CMUX_VAL,(void*)old_mux_val,2);
#endif /* FEATURE_DS_IS707A_CMUX */
  if(tok_ptr->op == NA || tok_ptr->op == (NA|EQ|AR) )
  {
  result = dsatparm_exec_param_cmd(
                                    mode,
                                    tab_entry,
                                    tok_ptr,
                                    rb_ptr
                                  );
    temp_val[0] = (uint8)((dsat_num_item_type)dsatutil_get_val(DSAT707_EXT_PARA_CMUX_IDX,0,0,NUM_TYPE));
#ifdef FEATURE_DS_IS707A_CMUX
    temp_val[1] = (uint8)((dsat_num_item_type)dsatutil_get_val(DSAT707_EXT_PARA_CMUX_IDX,0,1,NUM_TYPE));
#endif /* FEATURE_DS_IS707A_CMUX */
    (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CMUX_VAL,(void *)temp_val,2);
  }else if(tok_ptr->op == (NA|QU))
  {
    ds707_pkt_mgr_get_1x_profile_val(DS707_CMUX_VAL,(void *)temp_val,2); 
#ifdef FEATURE_DS_IS707A_CMUX
    if( temp_val[0] < 15 && temp_val[1] < 2)
    {
      rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                     rb_ptr->size,
                                     "+CMUX: %s,%s\n",
                                     ((def_list_type *) tab_entry->def_lim_ptr)[0].list_v[temp_val[0]],
                                     ((def_list_type *) tab_entry->def_lim_ptr)[1].list_v[temp_val[1]]);
    }
    else
    {
      return DSAT_ERROR;
    }

#else
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                       rb_ptr->size,
                                       "+CMUX: %d\n",
                                       temp_val[0]);
#endif /*FEATURE_DS_IS707A_CMUX*/
  }else if(tok_ptr->op == (NA|EQ|QU))
  {
    result = dsatparm_exec_param_cmd(
                                    mode,
                                    tab_entry,
                                    tok_ptr,
                                    rb_ptr
                                  );
  }
#ifdef FEATURE_DS_IS707A_CMUX
  if ((result == DSAT_OK) && (tok_ptr->op == (NA|EQ|AR)))
  {
   (void) dsatutil_memscpy((void*)&tmp_token_struct,sizeof(tokens_struct_type),
                           (void*)tok_ptr,sizeof(tokens_struct_type));
    result = ds_atcop_put_cmux_arg(tab_entry, &tmp_token_struct);

    if (result == DSAT_OK)
    {
      ds707_pkt_mgr_get_1x_profile_val(DS707_CMUX_VAL,(void *)temp_val,2);
      fwd_mux = dsat707_cmux_fwd_table[temp_val[0]];

      /* Set the maximum number of forward supplementals as per the
           forward multiplex option specified by the user */
      if( fwd_mux & 0x01 )
      {
        /* Odd multiplex option => rate set 1 */
        (void) snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS1_RRS1,
                                    (byte)((fwd_mux-1)/2) );
      }
      else
      {
        /* Even multiplex option => rate set 2 */
        (void) snm_set_max_for_sups( CAI_SO_MDR_PKT_DATA_FRS2_RRS2,
                              (byte)((fwd_mux-2)/2) );
      }
    }
  }
  
  if (result != DSAT_OK)
  {
    /*-----------------------------------------------------------------------
      Error.  Restore the old values.
    -----------------------------------------------------------------------*/
    (void) ds707_pkt_mgr_set_1x_profile_val(DS707_CMUX_VAL,
                                            (void *)old_mux_val,2);
  }
#endif /* FEATURE_DS_IS707A_CMUX_*/
  return result;
} /* dsat707_exec_cmux_cmd */
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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cqd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cqd_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_crc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_crc_cmd */

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
  Default is 2 in JCDMA and non-JCDMA mode with FEATURE_DS_MOBILE_IP
  turned OFF. Default is 2 in JCDMA mode when FEATURE_DS_MOBILE_IP
  is turned ON.

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_crm_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type    temp_val  = 0;
  uint8                 temp_mip_val;
  dflm_type            *crm_dlfm  = (dflm_type *) tab_entry->def_lim_ptr;
  uint8                 default_crm_val = 2;

  temp_val = crm_dlfm->default_v;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE ==( ((tok_ptr->op == NA)|| ((tok_ptr->op == (NA|EQ|AR)) && 
       (tok_ptr->args_found == 1 && (VALID_TOKEN(0)) && ATOI_OK == dsatutil_atoi(&temp_val,tok_ptr->arg[0],10) && 
       (temp_val <= crm_dlfm->upper) && (temp_val >= crm_dlfm->lower))))&& 
       (TRUE == ds707_pkt_mgr_set_1x_profile_val(DS707_CRM_VAL, (void *)&temp_val , 0))))
    {
      DS_AT_MSG1_ERROR("+CRM: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "+CRM: (%d - %d)\n",crm_dlfm->lower,crm_dlfm->upper);
  }
  else  if(tok_ptr->op == (NA|QU))
  {
    ds707_pkt_mgr_get_1x_profile_val(DS707_CRM_VAL,(void *)&temp_val,0);
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "+CRM: %d\n",
                                           temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
/*------------------------------------------------------------------------*/
  if ( IS_JCDMA_MODE())
  {
    ds707_pkt_mgr_get_1x_profile_val(DS707_CRM_VAL,(void *)&temp_val,0);
    if( tok_ptr->op == (NA) )
    {
      ds707_pkt_mgr_set_1x_profile_val(DS707_CRM_VAL,(void *)&default_crm_val,0);
    }
    else if( ( tok_ptr->op == (NA|EQ|AR) ) &&
             ( (dsat_num_item_type)temp_val < 2 ) )
    {
      /*---------------------------------------------------------------------
        CRM value of 2.is the only supported value in JCDMA mode.
      ---------------------------------------------------------------------*/
      ds707_pkt_mgr_set_1x_profile_val(DS707_CRM_VAL,(void *)&default_crm_val,0);
      return DSAT_ERROR;
    }
    else if (tok_ptr->op == (NA|EQ|QU))
    {
      rb_ptr->used = (word)snprintf((char*)rb_ptr->data_ptr,
                                         rb_ptr->size,
                                         "%s: (2)",
                                         tok_ptr->name);
    }
    return DSAT_OK;
  }
#ifdef FEATURE_DS_MOBILE_IP
  /*-------------------------------------------------------------------------
    If CRM is less than 2, check QCMIP to determine correct behaviour.
  -------------------------------------------------------------------------*/
  ds707_pkt_mgr_get_1x_profile_val(DS707_CRM_VAL,(void *)&temp_val,0);
  dsmip_get_1x_profile_val_num(DS_QCMIP_VAL,(void *)&temp_mip_val,0);
  if ( (dsat_num_item_type)temp_val < 2 &&
       (dsat_num_item_type)temp_mip_val > 0 )
  {
    /*-----------------------------------------------------------------------
      If QCMIP is greater than 0, restore original value of CRM and return 
      an error.
    -----------------------------------------------------------------------*/
      /*---------------------------------------------------------------------
        If QCMIP is 2, CRM had to have taken on the value of 2.
      ---------------------------------------------------------------------*/
    ds707_pkt_mgr_set_1x_profile_val(DS707_CRM_VAL,(void *)&default_crm_val,0);
    return DSAT_ERROR; 
  }
#endif /* FEATURE_DS_MOBILE_IP */
  return DSAT_OK;
} /* dsat707_exec_crm_cmd */


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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_csq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dword temp;
  db_items_value_type dbi;
  byte *rb_ptr = res_buff_ptr->data_ptr;
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
   memset((void*)&dbi , 0 , sizeof(dbi) );
  NOT_SUPPORTED_IN_JCDMA()
/* Read command */
  if (tok_ptr->op == NA)
  {
    db_get (DB_NO_SVC, &dbi);
    if(dbi.no_svc)
    {
      temp = 99;                /* Return signal quality unknown */
    }
    else
    {
      db_get (DB_DIGITAL, &dbi);
      if (dbi.digital)
      {
        db_get (DB_CDMA_RSSI, &dbi);
        temp = 31 * (CDMA_RSSI_THRESH - dbi.cdma_rssi);
        temp /= CDMA_RSSI_SPAN;
      }
      else
      {
        db_get (DB_RSSI, &dbi);
        temp = 31 * dbi.rssi;
        temp /= RF_FM_RSSI_SPAN;
      }
      temp = (temp > 31 ? 31 : temp); /*  Clip to 0 - 31 */
    }
#ifdef FEATURE_DSAT_EXTENDED_CMD  
    rb_ptr += snprintf( (char *)rb_ptr, res_buff_ptr->size,
                              "%s: ", tab_entry->name );
#endif/* FEATURE_DSAT_EXTENDED_CMD */
    rb_ptr = dsatutil_itoa (temp, rb_ptr, 10);

    *rb_ptr++ = ',';
    *rb_ptr++ = ' ';
    *rb_ptr++ = '9'; /*  Always return frame error rate  */
    *rb_ptr++ = '9'; /*  as unknown for now.             */
    *rb_ptr = '\0';
    res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  }
  /* Test command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* Generate supported values response */
    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                                                 res_buff_ptr->size,
                                                 "%s: %s,%s", 
                                                 "+CCSQ", 
                                                 "(0-31,99)",
                                                 "(99)"
                                                 );
  }
  /* Return Error in all other cases */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_csq_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_CSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+CSS? command.
  This command returns the  <SID> only
  
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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_css_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  db_items_value_type dbi;
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*------------------------------------------------------------------------*/
  memset((void*)&dbi , 0 , sizeof(dbi) );
  *rb_ptr++ = '?';
  *rb_ptr++ = ',';
  *rb_ptr++ = ' ';
  db_get (DB_SID, &dbi);
  rb_ptr = dsatutil_itoa (dbi.sid, rb_ptr, 10);
  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return result;
} /* dsat707_exec_css_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cta_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
#if defined(FEATURE_GOBI_VZW)
  dsat_num_item_type    temp_val   = 30;
#elif defined (FEATURE_GOBI_SPRINT)
  dsat_num_item_type    temp_val   = 60;
#else 
  dsat_num_item_type    temp_val   = DS707_CTA_DEFAULT_VALUE;
#endif
  dsat_num_item_type    dlfm_value = DS707_CTA_UPPER_LIMIT;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == NA)) 
  {
    if(FALSE == DSAT_VALIDATE_SET_707_PKTMGR(DS707_CTA_VAL,dlfm_value ) )
    {
      DS_AT_MSG1_ERROR("+CTA: Invalid Number of arguements  %d", tok_ptr->args_found);
      return DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                          "+CTA: (0 - %d)\n",dlfm_value);
  }
  else  if(tok_ptr->op == (NA|QU))
  {
    ds707_pkt_mgr_get_1x_profile_val(DS707_CTA_VAL,(void *)&temp_val,0);
    rb_ptr->used = (word) snprintf((char *)rb_ptr->data_ptr,
                                           rb_ptr->size,
                                           "+CTA: %d\n",
                                           temp_val);
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_cta_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cxt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_cxt_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_eb_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA() 
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_eb_cmd */


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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_efcs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_efcs_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_er_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_esr_cmd */

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
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_etbm_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_goi_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK; 
  /*------------------------------------------------------------------------*/
  rb_ptr->data_ptr[0] = '\0';
  rb_ptr->used = (word)strlen((char *)rb_ptr->data_ptr);
  return result;
} /* dsat707_exec_goi_cmd */

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
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,         /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *rb_ptr                    /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_ilrr_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_ma_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_ma_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_mr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_mr_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_ms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_ms_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_mv18r_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
  return result;
} /* dsat707_exec_mv18r_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_mv18s_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*------------------------------------------------------------------------*/
  NOT_SUPPORTED_IN_JCDMA()
  result = dsatparm_exec_param_cmd (mode, tab_entry, tok_ptr, rb_ptr);
 return result;
} /* dsat707_exec_mv18s_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_PZID_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+PZID command.This gives the 
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
/*ARGSUSED*/
dsat_result_enum_type dsat707_exec_pzid_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define DSAT_INVALID_PZID 0xFF 
/* Reference of INVALID_PZID from ds707_epzid_util.h */

  ds707_epzid_type curr_epzid;

  memset(&curr_epzid,0xFF,sizeof(curr_epzid));
  if ( tok_ptr->op == ( NA | QU ))
  {
    /* Get the current EPZID */
    ds707_epzid_get_current_epzid ( &curr_epzid );
    res_buff_ptr->used = snprintf( (char *)res_buff_ptr->data_ptr,
                                         res_buff_ptr->size,
                                         "%s:%d",
                                         tok_ptr->name, (uint8)curr_epzid.pzid );
  }
  else
  {
    /* No requirement for Set / Test commands */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsat707_exec_pzid_cmd */

/*===========================================================================
FUNCTION DSAT707_EXEC_CIMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CIMI command.
  This command compiles the International Mobile Subscriber Identity (IMSI)
  number of the device from the relevant NV items or the RUIM card and prints
  out the information.

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cimi_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  byte                   dsat_imsi_str[DSAT_IMSI_STR_MAX_LEN];
  byte                  *txt_ptr = NULL;
#define RETURN_RESPONSE() \
  if(dsat_sysinfo_cmd.dsat_rtre_config == DSAT_RTRE_CONTROL_RUIM)\
  {\
    return dsat_send_cme_error(DSAT_CME_SIM_FAILURE);\
  }\
  else\
  {\
    return dsat_send_cme_error(DSAT_CME_UNKNOWN);\
  }

  /*-------------------------------------------------------
  General commands with no arguments should return the 
  information requested in the form of a string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    memset( &dsat_imsi_str[0], 0, DSAT_IMSI_STR_MAX_LEN );
    /* Read the MCC from the NV first. */
    txt_ptr = &dsat_imsi_str[DSAT_IMSI_MCC_OFFSET];
    if (FALSE == ds707_pkt_mgr_read_imsi_mcc(txt_ptr))
    {
      DS_AT_MSG0_ERROR("Error retrieving IMSI MCC from NV");
      RETURN_RESPONSE()
    }
    else
    {
      /* Append the MNC to the existing string. */
      txt_ptr = &dsat_imsi_str[DSAT_IMSI_MNC_OFFSET];
      if (FALSE == ds707_pkt_mgr_read_imsi_mnc(txt_ptr))
      {
        DS_AT_MSG0_ERROR("Error retrieving IMSI MNC from NV");
        RETURN_RESPONSE()
      }
      else
      {
        txt_ptr = &dsat_imsi_str[DSAT_IMSI_MIN2_OFFSET];
        if (FALSE == ds707_pkt_mgr_read_min2(txt_ptr))
        {
          DS_AT_MSG0_ERROR("Error retrieving MIN2 from NV");
          RETURN_RESPONSE()
        }
        else
        {
          txt_ptr = &dsat_imsi_str[DSAT_IMSI_MIN1_OFFSET];
          if (FALSE == ds707_pkt_mgr_read_min1(txt_ptr))
          {
            DS_AT_MSG0_ERROR("Error retrieving MIN1 from NV");
            RETURN_RESPONSE()
          }
        }
      }
    }
    res_buff_ptr->used = (word)snprintf(
                           (char*)res_buff_ptr->data_ptr,
                            res_buff_ptr->size,
                            "%s",
                            dsat_imsi_str);
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    return dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
  }

  return DSAT_OK;

}/* dsat707_exec_cimi_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_mdn_cmd
(
  dsat_mode_enum_type  mode,              /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  nv_stat_enum_type nv_status;
  static char info[NV_DIR_NUMB_PCS_SIZ+1];/* NULL pointer */
  /*-------------------------------------------------------
  General commands with no arguments and read commands should 
  return the information requested in the form of a string
  ---------------------------------------------------------*/
  if ((tok_ptr->op == NA)||(tok_ptr->op == (NA|QU)))
  {
    nv_status = dsatutil_get_nv_item(NV_CURR_NAM_I, &ds_nv_item);
    if(nv_status == NV_NOTACTIVE_S || ds_nv_item.curr_nam >= NV_MAX_NAMS)
    {
      /*-----------------------------------------------------------------------
        Default to the first NAM
      -----------------------------------------------------------------------*/
      ds_nv_item.curr_nam = 0;
    }
    ds_nv_item.mob_dir_number.nam = ds_nv_item.curr_nam;

    /* MDN Reporting */
    nv_status = dsatutil_get_nv_item (NV_DIR_NUMBER_PCS_I, &ds_nv_item);
    if( nv_status == NV_DONE_S )
    {
      /* Convert the 32 bit unsigned number to hex */
      dsat707ext_mobnum_pcs_to_text(info, &ds_nv_item.mob_dir_number);
      res_buff_ptr->used = (word)snprintf( (char*)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "%s",
                                               info); 
    }
    else
    {
      result = DSAT_ERROR;
    }
  }

  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }

  /*-------------------------------------------------------
  Any argument with the command should result in an error
  ---------------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_mdn_cmd */

/*===========================================================================

FUNCTION DS_ATCOP_PUT_CMUX_ARG

DESCRIPTION
  This function checks the value and number of numeric arguments for the
  IS-707-A AT+QCMUX command, and, if they are correct, stores the value(s) in
  the parameter. This function also performs special processing and error
  checking for the following cases:
  (a) If <rev> is not specified, sets <rev> = <fwd>.
  (b) Checks that <fwd> & <rev> are either both odd or both even.

  If the range or number of values is out of bounds, or if the special check
  fails, an error will be returned.

DEPENDENCIES
  Note that this function assumes the that parse table entry for AT+QCMUX is
  of type LIST, and is not of type HEX.

RETURN VALUE
  DSAT_ERROR if parsing failed.  Else DSAT_OK.

SIDE EFFECTS
  The global "ds_atcop_result" will be set as a result of the processing.

===========================================================================*/

LOCAL dsat_result_enum_type ds_atcop_put_cmux_arg
(
  const dsati_cmd_type     *tab_entry,      /*  Entry in parse table        */
  tokens_struct_type *tok_ptr              /*  Command tokens from parser  */
)
{
  dsat_num_item_type scr_val [MAX_ARG];
  byte flag [MAX_ARG];
  atoi_enum_type e;
  unsigned int j;

  ASSERT( ( (tab_entry->attrib & HEX) == 0 ) && 
          ( (tab_entry->attrib & LIST) != 0 ) );

  memset( (void *)&flag[0], 0x0, MAX_ARG );
  memset( (void *)&scr_val[0], 0x0, MAX_ARG );

  /* Check the number of arguments.                                        */
  if (tok_ptr->args_found > tab_entry->compound)
  {
    DS_AT_MSG0_ERROR("DSAT_ERROR");
    return(DSAT_ERROR);
  }

  if ( tok_ptr->args_found  > MAX_ARG )
  {
    DS_AT_MSG0_ERROR("Invalid number of args found");
    return DSAT_ERROR;
  }
  /* If <rev> is not specified i.e. the user issued AT+QCMUX = <fwd>, then  */
  /* set <rev> equal to <fwd>.                                             */
  if( tok_ptr->args_found == 1 )
  {
    tok_ptr->arg[1] = tok_ptr->arg[0];
    tok_ptr->args_found++;
  }

  /* For each argument, check if the parameter value specified is valid.   */
  for (j = 0; j < tok_ptr->args_found; ++j)
  {
    flag [j] = 0;
    /*lint -save -e605 */
    e = ds_atcop_srch_list (&scr_val [j], 
              ((def_list_type *) tab_entry->def_lim_ptr)[j].list_v,
              tok_ptr->arg[j]);
    
    /*lint -restore */
    if (e == ATOI_OUT_OF_RANGE)
    {
      return(DSAT_PARAM);
    }
    else if (e == ATOI_OK)
    {
      flag [j] = 1;
    }
    /* else if (e == ATOI_NO_ARG) do nothing: flag[j] remains unset, and   */
    /* the value is not stored.                                            */
  }

  /* If <rev> is specified but <fwd> is not specified i.e. the user issued */
  /* AT+QCMUX = , <rev>  then set <fwd> equal to the previous value.        */
  if( (flag[0] == 0) && (flag[1] != 0) )
  {
    scr_val[0] = (dsat_num_item_type)dsatutil_get_val(
                               DSAT707_EXT_PARA_CMUX_IDX,0,0,NUM_TYPE);
  }

  /* Check that <fwd> and <rev> are either both odd or both even. Odd mux  */
  /* options correspond to rate set 1 and even mux options corrrespond to  */
  /* rate set 2. The hardware does not support asymmetric rate sets on the */
  /* forward and reverse links.                                            */
  if( (flag[0] != 0) || (flag[1] != 0) ) /* At least one argument was present */
  {
#ifdef FEATURE_DS_IS707A_CMUX
    if( ((dsat707_cmux_fwd_table[(scr_val[0])] ^ 
             dsat707_cmux_rev_table[(scr_val[1])]) & 0x01) != 0 )
    {
      /* Forward & reverse rate sets are not equal                         */
      return(DSAT_ERROR);
    }
    else
#endif /* FEATURE_DS_IS707A_CMUX */
    {
       /* The rate set check is ok => write the parameter value(s).         */
      for (j = 0; j < tok_ptr->args_found; ++j)
      {
        if (flag[j])
        {
          DSATUTIL_SET_VAL(DSAT707_EXT_PARA_CMUX_IDX,0,j,0,scr_val[j],NUM_TYPE)
        }
      }
    }
  }
  return(DSAT_OK);
} /*  ds_atcop_put_cmux_arg  */

/*===========================================================================
FUNCTION DS_ATCOP_SRCH_LIST

DESCRIPTION
  This function searchs a list of strings, and returns the index to the
  item found.  (The first item index is 0).  -1 is returned if no match.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
===========================================================================*/

LOCAL atoi_enum_type ds_atcop_srch_list
(
  dsat_num_item_type *val_arg_ptr,  /*  value returned  */
  const ds_ati_list_type l,
  byte * arg
)
{
  unsigned int i = 0;
  byte c;
  
  while (*l[i] != '\0')
  {
    if (dsatutil_strcmp_ig_sp_case ((byte *)l[i], arg) == 0)
    {
      *val_arg_ptr = i;
      return (ATOI_OK);
    }
    ++i;
  }

  i = 0; 
  
  while ( (c = arg[i++]) != '\0')
  {
    if (c != ' ')
    {
      return (ATOI_OUT_OF_RANGE);
    }
  }
  return (ATOI_NO_ARG);
}
/*===========================================================================

FUNCTION DS_ATCOP_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES

RETURN VALUE
  Returns enum values EMPTY, DIGITS, ASCII, or ERROR as appropriate.


SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type ds_atcop_proc_dial_str
(
  byte * in_ptr,          /*  Input string, null terminated   */
  byte * out_ptr,         /*  Output string, null terminated  */
  const byte * accept_list_ptr, /*  MUST be upper case, null term   */
  const byte * reject_list_ptr  /*  MUST be upper case, null term   */
)
{
  byte c, cu;
  dsat_dial_val_e_type result = DSAT_DIAL_EMPTY;

  while ((c = *in_ptr++) != '\0')
  {
    cu = c;
    cu = UPCASE (cu);
    if ((c >= '0' && c <= '9') || c == '#' || c == '*')
    {
      *out_ptr++ = c;
      if (result == DSAT_DIAL_EMPTY)
      {
        result = DSAT_DIAL_DIGIT;
      }
    }
    else if (ds_atcop_strchr ( accept_list_ptr, 
                               cu, 
                               (uint16)strlen((char *)accept_list_ptr)) != 0)
    {
      *out_ptr++ = c;
      result = DSAT_DIAL_ASCII_ABC;
    }
    else if (ds_atcop_strchr (reject_list_ptr, 
                              cu, 
                              (uint16)strlen((char *)reject_list_ptr)) != 0)
    {
      result = DSAT_DIAL_ERROR;
      break;
    }
  }
  *out_ptr = '\0';
  return result;
}

/*===========================================================================

FUNCTION DS_ATCOP_STRCHR

DESCRIPTION
  Looks for a byte in a string.

DEPENDENCIES

RETURN VALUE
  Return 1 if the byte is found, else 0.


SIDE EFFECTS

===========================================================================*/
int ds_atcop_strchr
(
  const byte *s,
  const byte c,
  const uint16 len
)
{
  uint16 i;

  for(i=0; i<len && s[i] != '\0';i++)
    {
    if (s[i] == c)
      {
      return 1;
      }
    }
  return 0;
}

/*=========================================================================*/

#ifdef FEATURE_DSAT_CDMA_PIN

/*===========================================================================

FUNCTION  EXT_CPIN_UNBLOCK_PIN

DESCRIPTION
  This function verifies the unblocking code and new PIN code are specified
  then initiates interaction with MMGSDI to unblock specified SIM PIN.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the execution.
  possible values:
    DSAT_CMD_ERR_RSP: if +CME ERROR is being generated
    DSAT_ASYNC_CMD :  excuting the action command and waiting for the
                      callback function be called.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type ext_cpin_unblock_pin
(
  const tokens_struct_type *tok_ptr,       /* Command tokens from parser  */
  const mmgsdi_pin_enum_type pin_id,       /* PIN identifier              */
  const mmgsdi_slot_id_enum_type slot_id   /* SIM slot identifier         */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_CMD;
  mmgsdi_return_enum_type       mmgsdi_status;
  mmgsdi_data_type old_pin_data, new_pin_data;
  
  /* Verify user entered newpin */
  if (2 == tok_ptr->args_found)
  {
    /* Initiate request to MMGSDI */
    cb_cmd_e_type req_submitter = (MMGSDI_PIN1 == pin_id)?
                DSAT_CDMA_CPIN_MMGSDI_PIN1_VERIFY : 
                DSAT_CDMA_CPIN_MMGSDI_PIN2_VERIFY;

    old_pin_data.data_ptr =(uint8 *)dsatutil_get_val(
                            DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_STR_TYPE);
    old_pin_data.data_len = strlen((char*)dsatutil_get_val(
                            DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_STR_TYPE));

    new_pin_data.data_ptr =(uint8 *)dsatutil_get_val(
                            DSAT707_EXT_ACT_QCPIN_IDX,0,1,MIX_STR_TYPE);
    new_pin_data.data_len = strlen((char*)dsatutil_get_val(
                            DSAT707_EXT_ACT_QCPIN_IDX,0,1,MIX_STR_TYPE));

    UNBLOCK_707_PIN( pin_id, 
                old_pin_data, 
                new_pin_data, 
                req_submitter );

    if (mmgsdi_status == MMGSDI_SUCCESS)
    {
      result = DSAT_ASYNC_CMD;
    }
    else if (mmgsdi_status == MMGSDI_INCORRECT_PARAMS)
    {
      /* We sent incorrect parameters to MMGSDI  */
      result = dsat_send_cme_error (DSAT_CME_PARAMETER_ERROR);
    }
    else
    {
      /* MMGSDI command queue is full */
      result = dsat_send_cme_error (DSAT_CME_UNKNOWN);
    }
  }
  else
  {
    /* Report error */
    if(MMGSDI_PIN1 == pin_id)
    {
      result = dsat_send_cme_error(DSAT_CME_SIM_PUK_REQUIRED);
    }
    else
    {
      result = dsat_send_cme_error(DSAT_CME_SIM_PUK2_REQUIRED);
    }

  }
  return result;
} /* ext_cpin_unblock_pin */

/*===========================================================================

FUNCTION  EXT_REPORT_CLCK_FDN_STATUS

DESCRIPTION
  This function reports the CLCK FDN status received from GSDI.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the execution.
  Possible values:
    DSAT_ERROR :    if there was any problem in execution.
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type ext_report_clck_fdn_status
(
  ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_OK;
  mmgsdi_return_enum_type mmgsdi_status = cmd_info_ptr->cmd_status;

  /* See if GSDI call was unsuccessful */
  switch (mmgsdi_status)
  {
    case MMGSDI_SUCCESS:
      /* Supplied PIN was correct so update PIN state tracking */
    switch (cmd_info_ptr->cmd_id)
    {
      case DSAT_CDMA_CLCK_MMGSDI_FDN_ENABLE_DISABLE:
           break;
      case DSAT_CDMA_CLCK_MMGSDI_FDN_QUERY:
        {
          dsm_item_type *res_buff_ptr;
          res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
          res_buff_ptr->used = (word)
            snprintf((char*)res_buff_ptr->data_ptr,
                   res_buff_ptr->size,
                  "+CLCK: %d", cmd_info_ptr->cmd_data);
          dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
        }
        break;
      default:
        result = DSAT_ERROR;
        break;
    }
    break;

    case MMGSDI_INCORRECT_CODE:
      result = dsat_send_cme_error (DSAT_CME_INCORRECT_PASSWORD);
      break;

    case MMGSDI_CODE_BLOCKED:
      /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
      dsat707_mask_pin2_state = FALSE;
      result = 
        dsat_send_cme_error ( DSAT_CME_SIM_PUK2_REQUIRED );
      break;
    default:
      DS_AT_MSG1_ERROR(" CLCK query status is %d (NOT SUCCESS)",mmgsdi_status);
      result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
      break;
  }

  return result;  
} /* ext_report_clck_fdn_status */

/*===========================================================================

FUNCTION EXT_CPIN_VERIFY_PIN_STATUS

DESCRIPTION
  Process the pin verification status.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_CMD_ERR_RSP : if the data parameter was not correct
    DSAT_ERROR :       if there was any problem in executing the command 
    DSAT_OK :          if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type ext_cpin_verify_pin_status
(
   ds_at_cmd_status_type *cmd_info_ptr
)
{
  dsat_result_enum_type result = DSAT_OK;
  
  mmgsdi_slot_id_enum_type slot_id = 
                   (mmgsdi_slot_id_enum_type)((uint16)cmd_info_ptr->cmd_status >> 8);
  mmgsdi_return_enum_type   mmgsdi_status =  (mmgsdi_return_enum_type)
                                (cmd_info_ptr->cmd_status & DSAT_MMGSDI_STATUS_MASK);
  
  DS_AT_MSG3_HIGH("cmd_id: %d mmgsdi_status: %d slot_id: %d",
               cmd_info_ptr->cmd_id,mmgsdi_status,slot_id); 

  /* See if GSDI call was unsuccessful */
  switch (mmgsdi_status)
  {
  case MMGSDI_SUCCESS:
    /* Supplied PIN was correct so update PIN state tracking */
    switch (cmd_info_ptr->cmd_id)
    {
    case DSAT_CDMA_CPIN_MMGSDI_PIN1_VERIFY:
    case DSAT_CDMA_CPIN_MMGSDI_PIN2_VERIFY:
    case DSAT_CDMA_CLCK_MMGSDI_PIN1_VERIFY:
    case DSAT_CDMA_CLCK_MMGSDI_FDN_ENABLE_DISABLE:
    case DSAT_CDMA_CPWD_MMGSDI_PIN1_CHANGE:
    case DSAT_CDMA_CPWD_MMGSDI_PIN2_CHANGE:
      /* Do nothing */      
      break;
    default:
      DS_AT_MSG2_ERROR("Invalid PIN Verify cmd_id: %d %d",
               cmd_info_ptr->cmd_id,slot_id); 
      result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }
    break; /* End case GSDI_SUCCESS */

  case MMGSDI_INCORRECT_CODE:
    switch (cmd_info_ptr->cmd_id)
    {
      case DSAT_CDMA_CPIN_MMGSDI_PIN1_VERIFY:
      case DSAT_CDMA_CPIN_MMGSDI_PIN2_VERIFY:
      case DSAT_CDMA_CLCK_MMGSDI_PIN1_VERIFY:
      case DSAT_CDMA_CLCK_MMGSDI_FDN_ENABLE_DISABLE:
      case DSAT_CDMA_CPWD_MMGSDI_PIN1_CHANGE:
      case DSAT_CDMA_CPWD_MMGSDI_PIN2_CHANGE:
        result = dsat_send_cme_error (DSAT_CME_INCORRECT_PASSWORD);
        break;
      default:
        DS_AT_MSG2_ERROR("Invalid PIN Verify cmd_id: %d %d",
               cmd_info_ptr->cmd_id,slot_id); 
        result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
      break;
    }
    break;

  case MMGSDI_CODE_BLOCKED:
    switch (cmd_info_ptr->cmd_id)
    {
    case DSAT_CDMA_CPIN_MMGSDI_PIN1_VERIFY:
    case DSAT_CDMA_CLCK_MMGSDI_PIN1_VERIFY:
    case DSAT_CDMA_CPWD_MMGSDI_PIN1_CHANGE:
      if(cmd_info_ptr->pin1.status == MMGSDI_PIN_PERM_BLOCKED)
      {
        DS_AT_MSG0_ERROR("PIN1 status is permanently blocked");
        result = dsat_send_cme_error ( DSAT_CME_SIM_FAILURE );
      }
      else
      {
        result = dsat_send_cme_error ( DSAT_CME_SIM_PUK_REQUIRED );
      }
      break;
          
    case DSAT_CDMA_CPIN_MMGSDI_PIN2_VERIFY:
    case DSAT_CDMA_CLCK_MMGSDI_FDN_ENABLE_DISABLE:
    case DSAT_CDMA_CPWD_MMGSDI_PIN2_CHANGE:
      /* If attempted verification of pin2 failed due to incorrect pin
         show this result in next +CPIN command response */
      if(cmd_info_ptr->pin2.status == MMGSDI_PIN_PERM_BLOCKED)
      {
        DS_AT_MSG0_ERROR("PIN2 status is permanently blocked");
        result = dsat_send_cme_error ( DSAT_CME_SIM_FAILURE );
      }
      else
      {
        dsat707_mask_pin2_state = FALSE;
        result = dsat_send_cme_error ( DSAT_CME_SIM_PUK2_REQUIRED );
      }
      break;
    default:
      DS_AT_MSG2_MED("Invalid PIN Verify cmd_id: %d %d",
              cmd_info_ptr->cmd_id,slot_id); 
      result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }
    break;

  case MMGSDI_WARNING_NO_INFO_GIVEN:
    /* Assume this corresponds to state requested same as current */
    break;

  case MMGSDI_INCOMPAT_PIN_STATUS:
    result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    break;

  case MMGSDI_ERROR:
  case MMGSDI_ERROR_NO_EVENT_NEEDED:
    result = dsat_send_cme_error (DSAT_CME_SIM_FAILURE);
    break;

  case MMGSDI_INCORRECT_PARAMS:
    result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    break;

  default:
    DS_AT_MSG1_HIGH("Unsupported MMGSDI event received: %d",
              mmgsdi_status); 
    result = dsat_send_cme_error (DSAT_CME_SIM_FAILURE);
    break;
  }

  return result;
} /* ext_cpin_verify_pin_status */

/*===========================================================================

FUNCTION  EXT_PROCESS_CPIN_STATE

DESCRIPTION
  This function acts as the state machine for the +CPIN  and ^CPIN 
  command handler. Based on the PIN status bitfield array, the appropriate 
  request to MMGSDI is initiated.  All call to MMGSDI are done asychronously.

  The state machine uses the following PIN precendence based on processing
  order:
  PUK1  
  PIN1
  PUK2
  PIN2

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : if async call successfully made
  DSAT_ERROR     : if async call failed due to bad parameters

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type ext_process_cpin_state
(
  const tokens_struct_type *tok_ptr       /*  Command tokens from parser  */
)
{
  dsat_result_enum_type  result = DSAT_ASYNC_CMD;
  mmgsdi_return_enum_type mmgsdi_status;
  mmgsdi_data_type pin_data;
  dsat_mixed_param_val_type *temp0_mix_type;
  dsat_mixed_param_val_type *temp1_mix_type;
  /* Macro to verify PIN/PUK max length of MMGSDI_PIN_MAX_LEN:
  */
#define PIN_LEN_OK(pinstr)\
  (strlen((char *)(pinstr->string_item)) <= MMGSDI_PIN_MAX_LEN)

  /* Set the flag set for PIN Verify */
  SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_VERIFY_CMD)

  /* State machine based on PIN/PUK state bit arrays: */
  /* PUK1 */
  if ( IS_707_PUK_STATE(DSAT_CPIN_SIM_PIN_REQUIRED) )
  {
    temp0_mix_type  = (dsat_mixed_param_val_type *)dsatutil_get_val(
                             DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_INDEX_TYPE);
    temp1_mix_type  =(dsat_mixed_param_val_type *)dsatutil_get_val(
                             DSAT707_EXT_ACT_QCPIN_IDX,0,1,MIX_INDEX_TYPE);
    if ( PIN_LEN_OK(temp0_mix_type) 
        && PIN_LEN_OK(temp1_mix_type) )
    {
      result = ext_cpin_unblock_pin ( tok_ptr, MMGSDI_PIN1, MMGSDI_SLOT_1 );
    }
    else
    {
      result = DSAT_ERROR;
    }
  }

  /* SLOT1 PIN1 */
  else if ( IS_707_PIN_STATE(DSAT_CPIN_SIM_PIN_REQUIRED) )
  {
    temp0_mix_type  = (dsat_mixed_param_val_type *)dsatutil_get_val(
                             DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_INDEX_TYPE);
    if ( PIN_LEN_OK(temp0_mix_type) ) 
    {

      pin_data.data_ptr = (uint8 *)dsatutil_get_val(
                            DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_STR_TYPE);
      pin_data.data_len = 
        (mmgsdi_len_type)strlen((char*) dsatutil_get_val(
                           DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_STR_TYPE));
    
      /* Verify PIN password */
      VERIFY_707_PIN( MMGSDI_PIN1, 
                      pin_data, 
                      DSAT_CDMA_CPIN_MMGSDI_PIN1_VERIFY );

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_ERROR;
    }
  }

  /* PUK2 */
  else if ( IS_707_PUK_STATE(DSAT_CPIN_SIM_PIN2_REQUIRED) )
  {
    temp0_mix_type  = (dsat_mixed_param_val_type *)dsatutil_get_val(
                             DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_INDEX_TYPE);
    temp1_mix_type  =(dsat_mixed_param_val_type *)dsatutil_get_val(
                             DSAT707_EXT_ACT_QCPIN_IDX,0,1,MIX_INDEX_TYPE);
    if ( PIN_LEN_OK(temp0_mix_type) && 
         PIN_LEN_OK(temp1_mix_type) ) 
    {
      result = ext_cpin_unblock_pin ( tok_ptr, MMGSDI_PIN2, MMGSDI_SLOT_1 );
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  /* SLOT1 PIN2 */
  else if ( IS_707_PIN_STATE(DSAT_CPIN_SIM_PIN2_REQUIRED) )
  {
    temp0_mix_type  = (dsat_mixed_param_val_type *)dsatutil_get_val(
                             DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_INDEX_TYPE);
    if ( PIN_LEN_OK(temp0_mix_type) ) 
    {
      pin_data.data_ptr = (uint8 *)dsatutil_get_val(
                             DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_STR_TYPE);
      pin_data.data_len = 
              (mmgsdi_len_type)strlen((char*) dsatutil_get_val(
                           DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_STR_TYPE));

      /* Verify PIN password */
      VERIFY_707_PIN( MMGSDI_PIN2, 
                      pin_data, 
                      DSAT_CDMA_CPIN_MMGSDI_PIN2_VERIFY );

      if (mmgsdi_status == MMGSDI_SUCCESS)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  /* ERROR CONDITION */
  else
  {
    DS_AT_MSG1_MED("Unrecognized PIN state: %d",dsat707_pin_required);
    result = DSAT_ERROR;
  }
  
  /* Clear the passwords from memory to be safe */
  dsatutil_reset_password(dsatutil_get_val(
     DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_STR_TYPE), MMGSDI_PIN_MAX_LEN+1);
  dsatutil_reset_password(dsatutil_get_val(
     DSAT707_EXT_ACT_QCPIN_IDX,0,1,MIX_STR_TYPE), MMGSDI_PIN_MAX_LEN+1);
  
  if ( result != DSAT_ASYNC_CMD )
  {
    /* Clear the flag set */
    SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
  }
  
  return result; 
} /* ext_process_cpin_state */


/*===========================================================================

FUNCTION  EXT_GET_CODE_STRING

DESCRIPTION
  This function verifies the pin required and pin blocked
  state and gives the corresponding string that needs to be displayed 
  for +CPIN or ^CPIN query command.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL char* ext_get_code_string
(
  void
)
{
  /* To mask bits related to SIM PIN2 */
#define PIN2_707_MASK DSAT_CPIN_SIM_PIN2_REQUIRED
   boolean hit = FALSE;
   uint8 index;
  char *code_string = NULL;
   /* To save actual pin2 state during manipulation */
   uint32 previous_pin_reqd = (uint32)DSAT_CPIN_NONE_REQUIRED;
   uint32 previous_pin_blkd = (uint32)DSAT_CPIN_NONE_REQUIRED;
  
   /* Save actual pin2 state and mask any pin2 bits set */
   if ( dsat707_mask_pin2_state )
   {
     previous_pin_reqd = dsat707_pin_required;
     previous_pin_blkd = dsat707_pin_blocked;
     dsat707_pin_required &= ~(uint32)PIN2_707_MASK;
     dsat707_pin_blocked  &= ~(uint32)PIN2_707_MASK;
   }
   /* Search the table for the response code/text */
   index = 0;
   do
   {
     hit = (cpin_707req_table[index].req_code ==
          ((uint32)dsat707_pin_required & cpin_707req_table[index].req_code));
  
     /* If PIN is blocked use PUK text from table else PIN text */
     code_string = ((uint32)dsat707_pin_blocked & 
                    cpin_707req_table[index].req_code) ?
       cpin_707req_table[index].puk_text : cpin_707req_table[index].pin_text;
  
     index++;
   } while (!hit && (code_string != NULL));
  
   if ( dsat707_mask_pin2_state )
   {
     /* Restore actual pin state */
     dsat707_pin_required = previous_pin_reqd;
     dsat707_pin_blocked = previous_pin_blkd;
   }
   else
   {
     /* "SIM PIN2" and "SIM PUK2" will
        be returned only when the last executed command resulted in PIN2 
        authentication failure: Once PIN2 required info has been displayed
        once, don't show it any more */
     dsat707_mask_pin2_state = TRUE;
   }
   return code_string;

}/* ext_get_code_string */

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
   ds_at_cmd_status_type *cmd_info_ptr
)
{

  dsm_item_type *res_buff_ptr = NULL;
  char *code_string = NULL;
  dsat_result_enum_type result = DSAT_ASYNC_EVENT;
  uint8 clck_status = 0;
  
  dsat707_pin_error = DSAT_CME_NO_ERROR;
  dsat707_pin_required = (uint32)DSAT_CPIN_NONE_REQUIRED;
  dsat707_pin_blocked = (uint32)DSAT_CPIN_NONE_REQUIRED;

  if( CHECK_PENDING(DSAT707_EXT_ACT_QCPIN_IDX,0,DSAT_PENDING_CLCK_MMGSDI_FAC_QUERY_CMD) )
  {
    /* Flag completion of user-initiated GSDI action */
    SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
    
    if(DSAT_CDMA_CLCK_MMGSDI_FDN_QUERY == (cb_cmd_e_type)cmd_info_ptr->cmd_id)
    {
      return ext_report_clck_fdn_status(cmd_info_ptr);
    }
    else
    {
      /*-------- Ensure GSDI call was successful --------*/
      if ((int16)MMGSDI_SUCCESS != 
       (cmd_info_ptr->cmd_status & (int16)DSAT_MMGSDI_STATUS_MASK))
      {
        DS_AT_MSG1_ERROR("MMGSDI call was not sucess %d",cmd_info_ptr->cmd_status);
        return dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
      }
      result = DSAT_OK;
      /* Map MMGSDI PIN status to response */
      switch (cmd_info_ptr->pin1.status)
      {
        case MMGSDI_PIN_DISABLED:
        case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
          /* Assume no pin; equivalent to disabled */
          clck_status = 0;
          break;

        case MMGSDI_PIN_ENABLED_VERIFIED:
        case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        case MMGSDI_PIN_BLOCKED:
          clck_status = 1;
          break;

        case MMGSDI_PIN_PERM_BLOCKED:
          DS_AT_MSG0_ERROR("PIN1 is permanently blocked");
          result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
          break;

        default:
          DS_AT_MSG1_MED("Unsupported MMGSDI PIN state: %d",cmd_info_ptr->pin1.status);
          result = DSAT_ERROR;
          break;
      }
      if (result != DSAT_OK)
      {
        return result;
      }

      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      /* Generate response */
      res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                           "%s: %d",
                                           dsat_curr_cmd_var.parse_table->name,
                                           clck_status);

      /* format response, send the response to the user as information text, */
      /* Clear the regional block indications flag, & free res_buff_ptr */
      dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );  
      return result;
    }
  }
  else if ( CHECK_PENDING(DSAT707_EXT_ACT_QCPIN_IDX,0,DSAT_PENDING_CLCK_MMGSDI_FAC_LOCK_UNLOCK_CMD) )
  {
    /* Flag completion of user-initiated GSDI action */
    SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
    
    if( DSAT_CDMA_CLCK_MMGSDI_FDN_ENABLE_DISABLE == (cb_cmd_e_type)cmd_info_ptr->cmd_id)
    {
        return ext_report_clck_fdn_status(cmd_info_ptr);
    }
    else
    {
      return ext_cpin_verify_pin_status(cmd_info_ptr);
    }
  }
  else if ( CHECK_PENDING(DSAT707_EXT_ACT_QCPIN_IDX,0,DSAT_PENDING_CPWD_MMGSDI_PWD_CHANGE) )
  {
    /* Flag completion of user-initiated GSDI action */
    SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
    return ext_cpin_verify_pin_status(cmd_info_ptr);
  }
  /* from here till down are all +CPIN/^CPIN related handling */

  DS_AT_MSG2_HIGH("MMGSDI command PIN1 (%d), PIN2(%d)",
             cmd_info_ptr->pin1.status,cmd_info_ptr->pin2.status); 
  if( cmd_info_ptr->cmd_id == (uint16)DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS)
  {
    switch (cmd_info_ptr->pin1.status)
    {
      case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
        dsat707_pin_error = DSAT_CME_SIM_FAILURE;
        break;
      case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        break;
      case MMGSDI_PIN_BLOCKED:
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        break;
      case MMGSDI_PIN_PERM_BLOCKED:
        dsat707_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_error = DSAT_CME_SIM_FAILURE;
        break;
      case MMGSDI_PIN_DISABLED:
      case MMGSDI_PIN_UNBLOCKED:
      case MMGSDI_PIN_ENABLED_VERIFIED:
        dsat707_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_required &= ~(uint32)DSAT_CPIN_SIM_PIN_REQUIRED;
        dsat707_pin_error = DSAT_CME_NO_ERROR;
        break;
      default:
        break;
    }
    switch (cmd_info_ptr->pin2.status)
    {
      case MMGSDI_PIN_STATUS_NOT_INITIALIZED:
        dsat707_pin_error = DSAT_CME_SIM_FAILURE;
        break;
      case MMGSDI_PIN_ENABLED_NOT_VERIFIED:
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        break;
      case MMGSDI_PIN_BLOCKED:
        dsat707_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        break;
      case MMGSDI_PIN_PERM_BLOCKED:
        dsat707_pin_blocked |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_required |= (uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        break;
      case MMGSDI_PIN_DISABLED:
      case MMGSDI_PIN_UNBLOCKED:
      case MMGSDI_PIN_ENABLED_VERIFIED:
        dsat707_pin_blocked &= ~(uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_required &= ~(uint32)DSAT_CPIN_SIM_PIN2_REQUIRED;
        dsat707_pin_error = DSAT_CME_NO_ERROR;
        break;
      default:
        break;
    }
  }
  if (dsat707_pin_error == DSAT_CME_NO_ERROR)
  {
    /*-------- Ensure MMGSDI call was successful --------*/
    if (MMGSDI_SUCCESS != cmd_info_ptr->cmd_status )
    {
      DS_AT_MSG1_HIGH("MMGSDI call was not sucess %d",cmd_info_ptr->cmd_status);
      switch(cmd_info_ptr->cmd_status)
      {
        case MMGSDI_INCORRECT_CODE:
          result = dsat_send_cme_error (DSAT_CME_INCORRECT_PASSWORD);
          break;
        case MMGSDI_CODE_BLOCKED:
          switch(cmd_info_ptr->cmd_id)
          {
            case DSAT_CDMA_CPIN_MMGSDI_PIN1_VERIFY:
             result = dsat_send_cme_error ( DSAT_CME_SIM_PUK_REQUIRED );
             break;
            case DSAT_CDMA_CPIN_MMGSDI_PIN2_VERIFY:
             result = dsat_send_cme_error ( DSAT_CME_SIM_PUK2_REQUIRED );
            break;
            default:
             result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
            break;
          }
          break;
        default:
         result = dsat_send_cme_error(DSAT_CME_SIM_FAILURE);
         break;
      }  
      return result;
    }
    /* handle PIN query commands */
    if( CHECK_PENDING(DSAT707_EXT_ACT_QCPIN_IDX,0,DSAT_PENDING_CPIN_MMGSDI_QUERY_CMD) )
    {
      /* Clear the flag set */
      SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
      code_string = ext_get_code_string();
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      res_buff_ptr->used = (word) snprintf (
                                       (char*)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
                                        "%s: %s",
                                         dsat_curr_cmd_var.parse_table->name,
                                         code_string);

      if( !dsatutil_strncmp_ig_sp_case((const byte *)dsat_curr_cmd_var.parse_table->name, 
                                      (const byte *)("^CPIN"),5 ) )
      {
        /* Display ^CPIN: code,,puk_retries_left,pin_retries_left,
                                puk2_retries_left,pin2-retries_left
        */
        res_buff_ptr->used = 
              (word) snprintf (
                                     (char*)res_buff_ptr->data_ptr,
                                      res_buff_ptr->size,
                                      "%s,,%d,%d,%d,%d",
                                      res_buff_ptr->data_ptr,
                                      (int)cmd_info_ptr->pin1.num_unblock_retries,
                                      (int)cmd_info_ptr->pin1.num_retries,
                                      (int)cmd_info_ptr->pin2.num_unblock_retries,
                                      (int)cmd_info_ptr->pin2.num_retries);
      }
      dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );
      result = DSAT_OK;
    }
    /* Handle PIN status for verifing commands */
    else if ( CHECK_PENDING(DSAT707_EXT_ACT_QCPIN_IDX,0,DSAT_PENDING_CPIN_MMGSDI_EXECUTE_CMD))
    {
      /* Clear the flag set */
      SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
      /* Verify second string provided & not empty when appropriate: */
      /* if SIM PIN is blocked OR */
      if ( IS_707_PUK_STATE(DSAT_CPIN_SIM_PIN_REQUIRED) ||
      /* if SIM PIN2 is blocked AND not awaiting SIM PIN,SIM2 PIN */
          (IS_707_PUK_STATE(DSAT_CPIN_SIM_PIN2_REQUIRED) && 
           !(dsat707_pin_required & (  (uint32)DSAT_CPIN_SIM_PIN_REQUIRED 
          ))) )
      {
        /* Incase of invalid PIN values, contents will be set to 0xFF */
        if ( 0xFF == ((char *)dsatutil_get_val(
                      DSAT707_EXT_ACT_QCPIN_IDX,0,1,MIX_STR_TYPE))[0] )
        {
          DS_AT_MSG0_ERROR("Null new PIN string invalid");
          return DSAT_ERROR;
        }
      }
      /* Report error if no PIN required */
      /* Must use exact match to enum here due to bitfield */
      if ((uint32)DSAT_CPIN_NONE_REQUIRED == dsat707_pin_required)
      {
        result = dsat_send_cme_error ( DSAT_CME_OP_NOT_ALLOWED );
      }
      else
      {
        result = ext_process_cpin_state(dsat_curr_cmd_var.tok_ptr);
      }
    }
    /* Handle PIN status after verifing */
    else if ( CHECK_PENDING(DSAT707_EXT_ACT_QCPIN_IDX,0,DSAT_PENDING_CPIN_MMGSDI_VERIFY_CMD) )
    {
      /* Clear the flag set */
      SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
      result = ext_cpin_verify_pin_status(cmd_info_ptr);
    }
    else
    {
      DS_AT_MSG0_HIGH("NO pending Status command");
    }
   
  }
  else
  {
    result = dsat_send_cme_error(dsat707_pin_error);
  }
  return result;
}/* dsat707_status_event_handler */


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
)
{
  ds_cmd_type            *cmd_ptr  = NULL;
  byte data = 0;
  ds_at_cmd_status_type*     status_cmd_ptr;

  /* tell atcop that the async return is done */
  cmd_ptr  = dsat_get_cmd_buf(sizeof(ds_at_cmd_status_type), FALSE);
  
  DS_AT_MSG1_HIGH("MMGSDI callback with status: %d", 
            status); 
  
  status_cmd_ptr = cmd_ptr->cmd_payload_ptr;

  switch (cnf)
  {
    case MMGSDI_GET_ALL_PIN_STATUS_CNF:
       switch (cnf_ptr->response_header.client_data) {
          case DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS:
          case DSAT_CDMA_CLCK_MMGSDI_PIN1_STATUS:
             status_cmd_ptr->pin1 = 
                   cnf_ptr->get_all_pin_status_cnf.pin1;
             status_cmd_ptr->pin2 =
                   cnf_ptr->get_all_pin_status_cnf.pin2;
             break;
          default:
            break;
        }
       
      break;
    case MMGSDI_PIN_OPERATION_CNF:
      DS_AT_MSG2_HIGH("pin retries = %d, puk retries = %d",
               cnf_ptr->pin_operation_cnf.pin_info.num_retries,
               cnf_ptr->pin_operation_cnf.pin_info.num_unblock_retries); 
      data = (byte) cnf_ptr->pin_operation_cnf.pin_info.num_retries;
      if(data == 0)/* PIN retires are over so copy PUK data */
      {
        data = (byte) cnf_ptr->pin_operation_cnf.pin_info.num_unblock_retries;
      }
      break;
    default:
      DS_AT_MSG1_ERROR("Not a valid mmgsdi cb rsp: %d", 
           cnf_ptr->response_header.response_type);
      break;
  }
  cmd_ptr->hdr.cmd_id = DS_CMD_ATCOP_STATUS_CMD;
  status_cmd_ptr->cmd_id = 
      (uint16)cnf_ptr->response_header.client_data;
  status_cmd_ptr->cmd_status = 
      (mmgsdi_return_enum_type)cnf_ptr->response_header.mmgsdi_status;
  status_cmd_ptr->cmd_data = data;
  ds_put_cmd(cmd_ptr);
} /* dsat707_mmgsdi_req_cb */


/*===========================================================================

FUNCTION DSAT707_EXEC_CPIN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPIN  or ^CPIN command.
  This set commands sends to the ME a password which is necessary before 
  operation.

  For ^CPIN query command response will be:

  AT^CPIN?
  ^CPIN:<code>,,<SIMPUKretries>,<SIM PIN retries>,<SIM PUK2 retries>,SIM PIN2 retries>
  OK 

  <code> -- describes the current pin state
  <XXX retries> -- it defines the number of retries left for each PIN's

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_cpin_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  word index;
  char *c_ptr;

  /* process command */
  if(tok_ptr->op == (NA|EQ|AR))
  {
    mmgsdi_return_enum_type mmgsdi_status;

    /* Verify first string provided & not empty */
    if (!VALID_TOKEN(0) ||
        ('"' == tok_ptr->arg[0][0] && '"' == tok_ptr->arg[0][1]))
    {
      DS_AT_MSG0_HIGH("Null PIN string invalid");
      return DSAT_ERROR;
    }
    /*---------------------------------------------------------------
    Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );

    if (DSAT_OK == result)
    {
      /* Verify allowed digits in PIN and PUK */
      for(index = 0; index < 2 && VALID_TOKEN(index); index++)
      {
        c_ptr = (char *)dsatutil_get_val(
                       DSAT707_EXT_ACT_QCPIN_IDX,0,index,MIX_STR_TYPE);
        while ( '\0' != *c_ptr )
        {
          /* Report error if char is not numeric */
          if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
          { 
            DS_AT_MSG0_ERROR("PIN char outside range [0-9]");
            return DSAT_ERROR;
          }
          c_ptr++;
        }
      }
      c_ptr = NULL;
      
      /* Query MMGSDI to get the PIN status */
      
      /* Flag start of user-initiated MMGSDI action */
      SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_EXECUTE_CMD)
      GET_707_ALL_PIN_STATUS(DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS);
      if (mmgsdi_status == MMGSDI_SUCCESS) 
      {
        result =  DSAT_ASYNC_CMD;
      }
      else
      {
        DS_AT_MSG1_ERROR("MMGSDI returned false for PIN status query %d",
                   (int)mmgsdi_status); 
        /* Clear Flag of user-initiated MMGSDI action */
        SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
        /* Clear the passwords from memory to be safe */
        dsatutil_reset_password((byte *)dsatutil_get_val(
                                DSAT707_EXT_ACT_QCPIN_IDX,0,0,MIX_STR_TYPE), 
                                MMGSDI_PIN_MAX_LEN+1);
        dsatutil_reset_password(dsatutil_get_val(
                                DSAT707_EXT_ACT_QCPIN_IDX,0,1,MIX_STR_TYPE), 
                                MMGSDI_PIN_MAX_LEN+1);
        result = DSAT_ERROR;
      }
    }
  }
  /* process the READ command */
  else if (tok_ptr->op == (NA|QU))
  {
    mmgsdi_return_enum_type mmgsdi_status;
    /* Flag start of user-initiated MMGSDI action */
    SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_QUERY_CMD)
    GET_707_ALL_PIN_STATUS(DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS);
    if (mmgsdi_status == MMGSDI_SUCCESS) 
    {
      return DSAT_ASYNC_CMD;
    }
    else
    {
      DS_AT_MSG1_ERROR("MMGSDI returned false for PIN status query %d",
                 (int)mmgsdi_status); 
      /* Clear Flag start of user-initiated MMGSDI action */
      SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
      return DSAT_ERROR;
    }
  }

  /* process the TEST command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* Do nothing; return success */
  }

  /* command does not make sense */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707_exec_cpin_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsat707_exec_clck_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  mmgsdi_return_enum_type  mmgsdi_status = MMGSDI_SUCCESS;
  mmgsdi_data_type pin_data;
  char *c_ptr;

  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if (tok_ptr->op == (NA|EQ|AR))
  {
    dsat_num_item_type clck_mode;
    dsat_num_item_type clck_fac;
    byte clck_passwd[MMGSDI_PIN_MAX_LEN+1]={0};
    byte  passwd_len = 0;
    boolean is_FDN = FALSE;

   /* Define list of PIN oriented facilities, as listed in */
   /* dsat707_clck_fac_valstr[] */
    const dsat_string_item_type pin_facs[3][8] = { "\"SC\"", "\"FD\"", "" };

    /* Validate FACILITY parameter  (no default) */
    if (VALID_TOKEN(0))
    {
      /* Search for token in PIN facilities */
      if (PARAM_HIT != dsatutil_srch_list (
                         &clck_fac, 
                         pin_facs,
                         tok_ptr->arg[0],
                         ACTION_QUOTE_NO_QUOTE ))
      {
        DS_AT_MSG0_HIGH("+CLCK Facility not supported");
        return dsat_send_cme_error(DSAT_CME_SUPS_UNKNOWN_CLASS);
      }
    }
    else
    {
      DS_AT_MSG0_HIGH("+CLCK Facility parameter mandatory");
      return dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }

    /* Validate MODE parameter  (no default) */
    if (VALID_TOKEN(1))
    {
      /* Convert to number */
      if (ATOI_OK != dsatutil_atoi(&clck_mode, tok_ptr->arg[1], 10))
      {
        DS_AT_MSG0_ERROR("Cannot convert MODE parameter");
        return dsat_send_cme_error(DSAT_CME_UNKNOWN);
      }
      /* Test for in-range */
      if ((dsat_num_item_type)DSAT_CLCK_MODE_MAX <= clck_mode)
      {
        DS_AT_MSG0_ERROR("MODE parameter outside range");
        return dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
      }
    }
    else
    {
      DS_AT_MSG0_ERROR("+CLCK MODE parameter mandatory");
      return dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }

    /* Validate PASSWORD parameter  (no default) */
    if (VALID_TOKEN(2))
    {
      /* Test string length */
      passwd_len = (dsat_num_item_type)strlen((char*)tok_ptr->arg[2]);
      
      if ((MMGSDI_PIN_MAX_LEN+2) < passwd_len)
      {
        DS_AT_MSG0_ERROR("Password string too long");
        return dsat_send_cme_error(DSAT_CME_UNKNOWN);
      }
      /* Strip quotes */
      if (!dsatutil_strip_quotes_out(tok_ptr->arg[2],
                                     clck_passwd,
                                     (word)passwd_len))
      {
        DS_AT_MSG0_ERROR("Problem stripping quotes");
        return dsat_send_cme_error(DSAT_CME_UNKNOWN);
      }
      passwd_len = (byte)strlen((char *)clck_passwd);
      
      /* Clear password */
      dsatutil_reset_password (tok_ptr->arg[2], (word)passwd_len+1);

      /* Check whether the password has only digits */
      c_ptr = (char *)clck_passwd;
      while ( '\0' != *c_ptr )
      {
        /* Report error if char is not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        { 
          DS_AT_MSG0_ERROR("PIN char outside range [0-9]");
          return dsat_send_cme_error(DSAT_CME_UNKNOWN);
        }
        c_ptr++;
      }
    }
    /* Maximum number of arguments allowed is 3. */
    if (tok_ptr->args_found > 3)
    {
      DS_AT_MSG1_ERROR("Incorrect number of Args (%d)",
                 tok_ptr->args_found); 
      return dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }

    if(!dsatutil_strncmp_ig_sp_case(tok_ptr->arg[0], (const byte *)"\"FD\"",4))
    {
      is_FDN = TRUE;
    }

    pin_data.data_ptr = (uint8 *)clck_passwd;
    pin_data.data_len = (mmgsdi_len_type) passwd_len;
     
    /* Initiate GSDI/MMGSDI call */
    switch (clck_mode)
    {
      case DSAT_CLCK_MODE_LOCK:
        /* Flag that the PIN state is being changed */
        SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CLCK_MMGSDI_FAC_LOCK_UNLOCK_CMD)
        if (is_FDN) 
        { /* Operation is for FD phone book */
          VERIFY_PIN( dsat_get_current_1x_session_id(),
                  MMGSDI_PIN2, 
                  pin_data, 
                  CLCK_MMGSDI_FDN_LOCK );
          if (mmgsdi_status != MMGSDI_SUCCESS) 
          {
            /* MMGSDI command queue was full */
            SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
            DS_AT_MSG0_ERROR("MMGSDI returned error for FDN enabling");
            result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
          }
        }
        else 
        { /* Operation is for SC */

          /* Invoke state change */
          ENABLE_707_PIN_STATUS( MMGSDI_PIN1,
                                 pin_data, 
                                 DSAT_CDMA_CLCK_MMGSDI_PIN1_VERIFY);

          if (mmgsdi_status != MMGSDI_SUCCESS) 
          {
            DS_AT_MSG1_ERROR("MMGSDI returned false for SET PIN status %d",
                       (int)mmgsdi_status);
            /* MMGSDI command queue was full */
            SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
            result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
          }
        }
       break;

      case DSAT_CLCK_MODE_UNLOCK:
        /* Flag that the PIN state is being changed */
        SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CLCK_MMGSDI_FAC_LOCK_UNLOCK_CMD)
        if (is_FDN) 
        { /* Operation is for FD phone book */
          VERIFY_PIN( dsat_get_current_1x_session_id(),
                      MMGSDI_PIN2, 
                      pin_data, 
                      CLCK_MMGSDI_FDN_UNLOCK );
          if(mmgsdi_status != MMGSDI_SUCCESS)
          {
            SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
            DS_AT_MSG0_ERROR("GSDI returned error for FDN disabling");
            result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
          }
        }
        else 
        { /* Operation is for SC */
          /* Invoke state change */
          DISABLE_707_PIN_STATUS( MMGSDI_PIN1,
                                  pin_data, 
                                  DSAT_CDMA_CLCK_MMGSDI_PIN1_VERIFY);
        
          if (mmgsdi_status != MMGSDI_SUCCESS) 
          {
            DS_AT_MSG1_ERROR("MMGSDI returned false for SET PIN status %d",
                       (int)mmgsdi_status);
            /* MMGSDI command queue was full */
            SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
            result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
          }
        }
        break;

      case DSAT_CLCK_MODE_QUERY:
        /* Flag start of user-initiated MMGSDI action */
        SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CLCK_MMGSDI_FAC_QUERY_CMD)
        if (is_FDN) 
        { 
          /* Operation is for FD phone book */
          if (MMGSDI_SUCCESS != mmgsdi_session_get_app_capabilities(
                                        dsat_get_current_1x_session_id(),
                                        dsatme_mmgsdi_perso_req_cb,
                                        (uint32)CLCK_MMGSDI_FDN_QUERY))
          {
            DS_AT_MSG0_ERROR("GSDI returned false for FDN status query");
            result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
          }
        }
        else 
        { /* Operation is for SC */
          /* Query MMGSDI to get the PIN status */
          GET_707_ALL_PIN_STATUS(DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS);
          if (mmgsdi_status != MMGSDI_SUCCESS) 
          {
            DS_AT_MSG1_ERROR("MMGSDI returned false for PIN status query %d",
                       (int)mmgsdi_status); 
            /* Clear Flag of user-initiated MMGSDI action */
            SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
            result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
          }

        }
        break;

      default:
        result = dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
        break;
      }
      /* Clear password */
      dsatutil_reset_password (clck_passwd, (word)MMGSDI_PIN_MAX_LEN);
  }
  /*--------------------------------------
  Processing for a TEST command:
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "%s: %s",
                                                parse_table->name, 
                                                dsat707_clck_fac_tststr);
    result = DSAT_OK;
  }
  else
  {
    /* wrong operation */
    result = dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
  }
  
  return result;

} /* dsat707_exec_clck_cmd */

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
/* ARGSUSED */
boolean dsat707_clck_abort_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
)
{
  return FALSE;
}/* dsat707_clck_abort_handler */

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
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if (tok_ptr->op == (NA|EQ|AR))
  {
#define CPWD_MAX_ARG (3)
    dsat_num_item_type fac = 0;
    byte  min_pw_len = 0;
    byte  max_pw_len = 0;
    byte  passwd_len = 0;
    mmgsdi_data_type old_pin_data,new_pin_data;
    byte old_passwd[MMGSDI_PIN_MAX_LEN+1];
    byte new_passwd[MMGSDI_PIN_MAX_LEN+1];
    cb_cmd_e_type client_ref = CB_NONE;
    char *c_ptr;
    
    /* Define list of PIN oriented facilities, as listed in */
    /* dsat_cpwd_fac_valstr[] */
    const dsat_string_item_type pin_facs[3][8] = { "\"SC\"", "\"P2\"", "" };
    /* MMGSDI PIN identifiers in same order as above */
    const mmgsdi_pin_enum_type pin_ids[] = {MMGSDI_PIN1, MMGSDI_PIN2};
    mmgsdi_pin_enum_type  pin_id = MMGSDI_MAX_PIN_ENUM;
    mmgsdi_return_enum_type mmgsdi_status;

    memset(&old_pin_data,0,sizeof(old_pin_data));
    memset(&new_pin_data,0,sizeof(new_pin_data));

    /* Check number of valid arguments */
    if( CPWD_MAX_ARG != tok_ptr->args_found)
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    
    /* Validate FACILITY parameter  (no default) */
    if (VALID_TOKEN(0))
    {

      /* Search for token in PIN list and set flag */
      if (PARAM_HIT == dsatutil_srch_list (
                         &fac, 
                         pin_facs,
                         tok_ptr->arg[0],
                         ACTION_QUOTE_NO_QUOTE ))
      {
        pin_id = pin_ids[fac];
        min_pw_len =  MMGSDI_PIN_MIN_LEN;
        max_pw_len =  MMGSDI_PIN_MAX_LEN;
      }
      else
      {
        return dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
      }
    }

    /* Validate old PASSWORD parameter  (no default) */
    if (VALID_TOKEN(1))
    {
      /* Strip quotes */
      if ('"' == tok_ptr->arg[1][0])
      {
        if (!dsatutil_strip_quotes_out(tok_ptr->arg[1],
                                       old_passwd,
                                       MMGSDI_PIN_MAX_LEN+1))
        {
          err_info.errval = DSAT_ERR_QUOTE_STRIP_FAILURE;
          err_info.arg_num = 1;
          goto send_error;
        }
      }
      else
      {
        passwd_len = strlen((char*)tok_ptr->arg[1]);
        if ( passwd_len > max_pw_len)
        {
          err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
          err_info.arg_num = 1;
          goto send_error;
        }
        (void)strlcpy ((char*)old_passwd,
                       (char*)tok_ptr->arg[1], max_pw_len+1);
      }

      /* Clear password */
      dsatutil_reset_password (tok_ptr->arg[1],
                               (word)strlen((char*)tok_ptr->arg[1])+1);

      /* Test string length */
      passwd_len = (byte)strlen((char*)old_passwd);

      if ((max_pw_len < passwd_len) || (min_pw_len > passwd_len))
      {
        err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
        err_info.arg_num = 1;
        goto send_error;
      }
      old_pin_data.data_len = (mmgsdi_len_type)passwd_len;
      old_pin_data.data_ptr = (uint8*)old_passwd;
      passwd_len=0;

      /* Verify allowed digits */
      c_ptr = (char*)old_passwd;
      while ( '\0' != *c_ptr )
      {
        /* Report error if char not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        { 
          DS_AT_MSG1_HIGH("PIN char outside range [0-9]: %c",*c_ptr);
          return DSAT_ERROR;
        }
        c_ptr++;
      }
    }
    
    /* Validate new PASSWORD parameter  (no default) */
    if (VALID_TOKEN(2))
    {
      /* Strip quotes */
      if ('"' == tok_ptr->arg[2][0])
      {
        if (!dsatutil_strip_quotes_out(tok_ptr->arg[2],
                                       new_passwd,
                                       MMGSDI_PIN_MAX_LEN+1))
        {
          err_info.errval = DSAT_ERR_QUOTE_STRIP_FAILURE;
          err_info.arg_num = 2;
          goto send_error;
        }
      }
      else
      {
        passwd_len = strlen((char*)tok_ptr->arg[2]);
        if ( passwd_len > max_pw_len)
        {
          err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
          err_info.arg_num = 2;
          goto send_error;
        }
        (void)strlcpy ((char*)new_passwd,
                           (char*)tok_ptr->arg[2], max_pw_len+1);
      }

      /* Clear password */
      dsatutil_reset_password (tok_ptr->arg[2],
                               (word)strlen((char*)tok_ptr->arg[2])+1);

      /* Test string length */
      passwd_len = (byte)strlen((char*)new_passwd);
      if ((max_pw_len < passwd_len) || (min_pw_len > passwd_len))
      {
         err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
         err_info.arg_num = 2;
         goto send_error;
      }
      /* Verify allowed digits */
      c_ptr = (char*)new_passwd;
      while ( '\0' != *c_ptr )
      {
        /* Report error if char not numeric */
        if (!(('0' <= *c_ptr) && ('9' >= *c_ptr)))
        { 
          DS_AT_MSG1_HIGH("PIN char outside range [0-9]: %c",*c_ptr);
          return DSAT_ERROR;
        }
        c_ptr++;
      }
      new_pin_data.data_len = (mmgsdi_len_type)passwd_len;
      new_pin_data.data_ptr = (uint8*)new_passwd;
    }
    if (MMGSDI_PIN1 == pin_id)
    {
      client_ref =  DSAT_CDMA_CPWD_MMGSDI_PIN1_CHANGE;
    }
    else
    {
      client_ref =  DSAT_CDMA_CPWD_MMGSDI_PIN2_CHANGE;
    }

    /* Set Flag of user-initiated MMGSDI action */
    SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0,DSAT_PENDING_CPWD_MMGSDI_PWD_CHANGE)

    
    /* Initiate GSDI PIN change */
    CHANGE_707_PIN(pin_id,old_pin_data,new_pin_data,client_ref)
    
    if (mmgsdi_status != MMGSDI_SUCCESS)
    {
      DS_AT_MSG1_ERROR("MMGSDI returned ERROR = %d",(int)mmgsdi_status);
      /* Clear Flag of user-initiated MMGSDI action */
      SET_PENDING(DSAT707_EXT_ACT_QCPIN_IDX ,0, DSAT_PENDING_CPIN_MMGSDI_CMD_NONE)
      
      result = DSAT_ERROR;
    }

    /* Clear passwords */
    dsatutil_reset_password (old_passwd,
                               MMGSDI_PIN_MAX_LEN+1);
    dsatutil_reset_password (new_passwd,
                               MMGSDI_PIN_MAX_LEN+1);
  }

  /*--------------------------------------
  Processing for a TEST command:
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    const struct cpwd_tst_entry
    {
      dsat_string_item_type fac[5];
      dsat_num_item_type len;
    } cpwd_tst_table[] =
    {
      { "\"SC\"",  MMGSDI_PIN_MAX_LEN},
      { "\"P2\"",  MMGSDI_PIN_MAX_LEN},
      { "",        0}
    };
    byte i = 0;

    res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "%s: ",
                                                parse_table->name);

    while ('\0' != cpwd_tst_table[i].fac[0])
    {
      res_buff_ptr->used = (word) snprintf ((char*)res_buff_ptr->data_ptr,
                                                  res_buff_ptr->size,
                                                  "%s(%s,%d),",
                                                  res_buff_ptr->data_ptr,
                                                  cpwd_tst_table[i].fac,
                                                  cpwd_tst_table[i].len);
      i++;
    }
    /* Overwrite last comma */
    res_buff_ptr->data_ptr[--res_buff_ptr->used] = '\0';
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX ||
      err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
}/* dsat707_exec_cpwd_cmd */

#endif /* FEATURE_DSAT_CDMA_PIN */

/*===========================================================================
  FUNCTION DSAT707EXT_MOBNUM_PCS_TO_TEXT()

  DESCRIPTION
    convert MDN NV format to ascii-text representation
    
  PARAMETERS
    dest : will contain the mobile number in ascii-text format
    src  : mobile number in NV format

  RETURN VALUE
    None
    
  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

LOCAL void dsat707ext_mobnum_pcs_to_text
(
  char *                  dest, 
  nv_mob_dir_data_type *  src
)
{
  int i;
  int len;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT((dest) && (src));

  len = src->n_digits;

  if(len > NV_DIR_NUMB_PCS_SIZ)
  {
    DS_AT_MSG0_HIGH("Ignoring Mobile Number digits 16 and higher");
  }

  /*-------------------------------------------------------------------------
    MDN NV format is sort of like BCD, but 8 bits instead of 4, and 
    '0' is represented as 0x0A.
  -------------------------------------------------------------------------*/
  for (i=0; i < MIN(len,NV_DIR_NUMB_PCS_SIZ); i++)
  {
    dest[i] = (src->digitn[i]==10) ? (char)'0' : (char)src->digitn[i] + '0';
  }
  dest[i] = '\0';
}/* dsat707ext_mobnum_pcs_to_text */

#endif /* FEATURE_DATA_IS707 */
