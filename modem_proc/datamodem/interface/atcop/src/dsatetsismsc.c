
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes the AT commands. It mainly executes the sms commands.

EXTERNALIZED FUNCTIONS
  dsatetsisms_exec_csas_cmd
  This function executes AT+CSAS command. +CSAS saves the settings of +CSCA,
  +CSMP, +CSCB into NV. 

  dsatetsisms_exec_cres_cmd
  This function executes AT+CRES command. +CRES saves the settings of +CSCA,
  +CSMP, +CSCB from NV which got saved through +CSAS command. 
  
  dsatetsisms_exec_cgsms_cmd
  This function executes AT+CGSMS command. +CGSMS is used to select service 
  for MO SMS messages.

  dsatetsisms_exec_csms_cmd
  This function executes AT+CSMS command. +CSMS is used to select messaging 
  service.

  dsatetsisms_exec_cpms_cmd
  This function executes AT+CPMS command. +CPMS is used to select preferred 
  memory storage for reading, writing etc.

  dsatetsisms_exec_csca_cmd
  This function executes AT+CSCA command. +CSCA is used to select the SMSC 
  address, through which mobile originated SMs are transmitted.

  dsatetsisms_exec_csmp_cmd
  This function executes AT+CSMP command. +CSMP is used to select additional 
  parameters that are needed to send or to store SM.

  dsatetsisms_exec_cnmi_cmd
  This function executes AT+CNMI command. +CNMI is used to select the 
  procedure of how receiving new messages from the network is indicated to TE.

  dsatetsisms_exec_cmgl_cmd
  This function executes AT+CMGL command. +CMGL is used to list message with 
  status value from message storage <mem1> to TE.
  
  dsatetsisms_exec_cmgr_cmd
  This function executes AT+CMGR command. +CMGR is used to read message with 
  location value <index> from message storage <mem1>.

  dsatetsisms_exec_cmgd_cmd
  This function executes AT+CMGD command. +CMGD is used to delete message with 
  location value <index> from message storage. Wild cards (<del Flags>) can 
  also be used in deleting messages.

  dsatetsisms_exec_cmss_cmd
  This function executes AT+CMSS command. +CMSS is used to send a message with 
  location value (<index>) from preferred message storage to the network..

  dsatetsisms_exec_cmgs_cmd
  This function executes AT+CMGS command. +CMGS is used to send a message from
  TE to the network.

  dsatetsisms_exec_cmgw_cmd
  This function executes AT+CMGW command. +CMGW is used to store message to 
  memory storage <mem2>

  dsatetsisms_exec_cmgc_cmd
  This function executes AT+CMGC command. +CMGC is used to senc command message 
  to the network.

  dsatetsisms_exec_cnma_cmd
  This function executes AT+CNMA command. +CNMI confirms correct reception 
  of a new which is routed directly to TE.

  dsatetsisms_exec_cscb_cmd
  This function executes AT+CSCB command. +CSCB command is used to select
  which type of Cell Broadcast Messages are to be received by TE.

dsatetsisms_exec_cmms_cmd
  This function executes AT+CMMS command. +CMMS controls the continuity of SMS
  relay protocol link. 

  dsatetsisms_send_sms_msg
  This function is used to transmit a SMS message (After SMS message entry 
  mode is entered). It is used in both text mode as well as PDU mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2002 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsismsc.c_v   1.6   12 Jul 2002 10:07:18   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsismsc.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/01/16   skc     Fixed CPMS display issue.
12/17/15   skc     Fixed KW Issues
12/08/15   skc     Fixed compilation issue due to FEATURE_DUAL_SIM disabled
10/06/15   skc     Fixed KW Issues
02/10/15   tk      Fixed issues in subscription specific WMS API calls.
01/05/14   sc      Fixed +CSCB AT command issue.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.
05/20/14   pg      Fixed +CSCA and +CSMP issue.
03/11/14   tk      Optimized debug macros usage in ATCoP.
03/27/13   tk      Fixed issues in dual stack commands.
01/10/13   tk      Fixed +CMGL and +CMGD issues with MO SENT messages.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   nc      Fixed operating mode checks for SMS related commands for NikeL targets. 
01/19/12   sk      Feature cleanup.
09/27/11   ht      Included dsut header to support offtarget stubs.
10/25/10   ad      Init Changes for DSDS. 
05/10/10   kk      Updating pres_cmd from to struct to store more data.
12/15/09   nc      Mainlining FEATURE_DATA_SERIALIZER.
09/09/09   bs      Fixed Klocwork criticals.
08/20/09   sa      Fixed feature wrap issue for a GSM/GPRS only build. 
06/19/09   ua      CRES should return DSAT_OK when resuming empty MIDS list.
05/26/09   nc      Removed the FEATURE_GWSMS_UDH specific code.
12/02/08   ss      Off target Lint fix.
10/23/08   bs      Added support for 1X AT Phonebook commands.
07/03/08   nc      Fixed USSD broken charecter set problems
05/28/08   nc      Fixed +CPMS command for SR and related issues 
                   in +CMGD and +CMGL.
05/06/08   ua      Removing the adjustments for <service> value in CGSMS.
04/09/08   bs      Fixed Klocwork criticals.
03/24/08   bs      Fixed Klocwork criticals.
03/03/08   bs      Fixing CSCA regression issues.
12/10/07   sa      Fixed the page default exception caused by +CNMI read command.
11/13/07   pp      Lint low - fixes.
10/31/07   ua      Added support for +CSAS and +CRES commands. 
08/17/07   ua      Modified +CNMI cmd execution for $QCCNMI command to help
                   pass the GCF test cases. 
04/26/07   sa      Fixed lint medium errors.
04/23/07   ua      Fixed lint high errors. 
03/24/07   ua      Correcting Banned API changes. 
02/15/07   ss      Fixed lint high errors
02/15/07   ph      Lint cleanup
02/13/07   pkp     Fixed Klocwork (NULL dereference) errors.
02/13/07   pkp     Fixed RVCT compiler warnings.
02/06/07   ua      Correcting <bm> parameter range for CNMI
01/30/07   ss      Replaced banned string API calls.
08/22/05   snb     Clear dsatsmsi_pres_cmd on error conditions.
05/13/05   sb      Enable direct routing of SMS STATUS-REPORT
03/29/05   snb     Correct +CSCA <toa> setting when not specified or in 
                   invalid combination.
03/15/05   sb      Fixes from testing the serializer changes.
03/02/05   dvp     Fixed bugs in ^SMG[LRO]
02/13/05   dvp     Added support for ^SMGL command.
02/10/05   sb      Accessories related changes.
12/16/04   snb     Change CMMS_IDLE_TIMEOUT_VAL to 4s so that link teardown 
                   and subsequent CP-ACK occur before NW timeout.
11/18/04   snb     Add +CSCS character set support and additional +CGSMS parms.
10/21/04   snb     Add template support to SMS code.
09/03/04   snb     Add support for +CMMS command and allow +CMGW to default to
                   SMS_SUBMIT.
07/06/04   snb     Return +CME ERROR if WMS not ready.
05/24/04   snb     Clear DCSes on accept no CBM types.
02/24/04   snb     Default to SMS-SUBMIT on SMS send with no <fo> setting.
01/26/04   sb      Fixed the error code in +CMGS command processing.
11/28/03   sb      CDMA SMS changes
10/22/03   sb      CB integration changes
10/03/03   snb     Support for 8 bit and UCS2 alphabets, changes to +CSMP 
                   parameter validation
09/09/03   sb      Fix for ECR33797 and CR34269: Do not expect an ACK for an
                   message with WMS_GW_MSG_WAITING_DISCARD
08/24/03   sb      Cell broadcast editions (first cut)
07/23/03   sb      Added assert.h to remove warnings
05/06/03   snb     Make use of ERROR/+CMS ERROR consistent.
05/01/03   snb     Err on attempt to set <fo> indicating SMS-COMMAND in +CSMP.
04/17/03   sb      Event payload optimization
04/08/03   sb      Status-report and command support
03/05/03   wx      Add +CGSMS command
01/07/02   sb      Fixed ECR 26379 : When +CSCA not set +CMGS doesn't err 
                   until after msg entry terminated
09/26/02   sb      Code review changes
06/27/02   wx      Request memory status one after another for +CPMS
02/07/02   sb      Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_ETSI_SMS
/* this file should be included only for SMS in GSM or WCDMA mode */
#include "dsatsmsi.h"
#include "dsatetsismsi.h"
#include "dsatetsictab.h"
#include "dsatparm.h"
#include "dsatetsime.h"
#include "err.h"
#include "amssassert.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include <stringl/stringl.h>
#include "stdio.h"
#include "wms.h"
#include "dsatctab.h"
#include "dsati.h"



/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for variables that are used
  in ATCOP SMS SUB SYSTEM
===========================================================================*/
/* import variable from other files inside sms module */

/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type   dsatsmsi_pres_cmd;

/* Variable used in CMGL SM */
extern sms_cmd_sm_type  dsatsmsi_cmd_sm;

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

/* flag for wms readiness */
extern boolean dsatsmsi_wms_ready;

/* remembers the mem being reported */
extern cpms_mem_status_data_s_type dsatsmsi_mem_report;

/* variables for handling +CNMA */
/* Flag to indicate if we are in a waiting for an ACK state */
extern boolean waiting_for_an_ack;
extern wms_transaction_id_type act_tid;

/* Variable to retain destination address for send commands (text mode) */         
wms_address_s_type   dsatetsismsi_send_addr = {0};

/* Variable to retain the type of <STAT> for send commands */         
LOCAL wms_message_tag_e_type  dsatetsismsi_send_stat = WMS_TAG_NONE;

/* Variable to retain the type of TPDU for send commands (text mode ) */  
LOCAL wms_gw_tpdu_type_e_type dsatetsismsi_send_tpdu = WMS_TPDU_MAX;

/* Variable to retain destination address for send commands (PDU mode) */   
LOCAL dsat_num_item_type dsatetsismsi_send_length = 0;


/* Variable to retain  message reference number for send command */
dsat_num_item_type dsatetsismsi_send_mr = 0;

/* Variable to find out what type of an ACK we are sending */
LOCAL boolean dsatetsismsi_ack_type;


/* Variable to retain the ACK PDU length */
dsat_num_item_type dsatetsismsi_ack_len;

/* Variable to retain the <fo> of the CMGC command */
dsat_num_item_type dsatetsismsi_cmgc_fo = 0;

/* Variable to retain the <pid> of the CMGC command */
dsat_num_item_type dsatetsismsi_cmgc_pid = 0;

/* Variable to retain the <ct> of the CMGC command */
dsat_num_item_type dsatetsismsi_cmgc_ct = 0;

/* Variable to retain the <mn> of the CMGC command */
dsat_num_item_type dsatetsismsi_cmgc_mn = 0;

#ifdef FEATURE_ETSI_SMS_TEMPLATE
boolean dsatetsismsi_template_present = FALSE;
/* Variable to write SCA, PID and DCS to templates */
wms_client_message_s_type *template_ptr = NULL;
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

extern dsat_mixed_param_val_type dsat_csca_prev_val[2];

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.
===========================================================================*/
/* LOCAL CONSTANTS: */
#define MAX_PDU_LENGTH 510

/* For +CMMS command the time in seconds of no SMS transmission after which
   the link will be brought down */
#define CMMS_IDLE_TIMEOUT_VAL 4

/* LOCAL DEFINITIONS */
#ifdef FEATURE_ETSI_SMS_CB
typedef struct{
  uint16 from;
  uint16 to;
} dsatetsismsi_dcs_val_type;

typedef enum
{
  DSAT_CSCB_MODE_ADD_ALL = 0,
  DSAT_CSCB_MODE_DEL_ALL,
  DSAT_CSCB_MODE_ADD,
  DSAT_CSCB_MODE_DEL,
  DSAT_CSCB_MODE_NONE
}dsatetsisms_cscb_mode_e_type;
#endif /* FEATURE_ETSI_SMS_CB */

#ifdef FEATURE_ETSI_SMS_CB
#define MAX_SMSI_MIDS 50
#endif /* FEATURE_ETSI_SMS_CB */

/* PROTOTYPES FOR LOCAL FUNCTIONS: */
LOCAL dsati_cms_err_e_type send_smspdu_msg
(
  byte  *pdu_ptr, /* Ptr to the pdu message that has to be sent */
  int   len       /* Len of the txt message that has to be sent */
); 

LOCAL dsati_cms_err_e_type send_smstxt_msg
(
  byte  *txt_ptr, /* Ptr to the pdu message that has to be sent */
  int   len       /* Len of the txt message that has to be sent */
);

LOCAL dsati_cms_err_e_type send_pdu_msg
(
  byte  *pdu_ptr, /* Ptr to the pdu message that has to be sent */
  int   len       /* Len of the txt message that has to be sent */
);

LOCAL dsati_cms_err_e_type send_smspdu_ack
(
  byte  *pdu_ptr, /* Ptr to the pdu message that has to be sent */
  int   len       /* Len of the txt message that has to be sent */
);

LOCAL dsat_result_enum_type send_smstxt_ack
(
 boolean ack_type /* RP-ACK or RP-ERROR */
);

LOCAL wms_message_tag_e_type stat_lookup
( 
 byte * stat_val, /* pointer to the stat value */
 boolean type,    /* PDU or text mode          */
 int  * all_flag  /* Ptr for "ALL",4 FLAG      */
);

LOCAL wms_gw_tpdu_type_e_type tpdu_lookup
( 
 wms_message_tag_e_type stat /* stat type enum */
);

#ifdef FEATURE_ETSI_SMS_CB
LOCAL boolean parse_cscb_param 
( 
 byte  * param,                          /* ptr to the unparsed str */
 dsatetsismsi_dcs_val_type * dcs,        /* ptr to the place holder */
 uint8 *  num_of_param                   /* ptr to the count        */
);

 LOCAL dsat_result_enum_type process_cscb_cmd 
(
  dsatetsisms_cscb_mode_e_type mode,           /* CSCB mode value*/
  uint8 num_of_ids,                            /* Number of id's to be processed*/
  const wms_bc_mm_service_info_s_type *smsi_cb_mids  /* MIDS value in WMS expected struct*/
);

LOCAL boolean init_mids();

LOCAL dsat_result_enum_type read_cscb_settings 
(
  sms_cmd_name_e_type cmd_type
);
#endif /* FEATURE_ETSI_SMS_CB */


/* FUNCTION DEFINITIONS */

#ifdef FEATURE_ETSI_SMS_PS
/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CGSMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CGSMS command. 
  +CGSMS is used to select service for MO SMS messages.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD : if the command has been successfully executed
    DSAT_ERROR     : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if WMS not ready

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cgsms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type         result;
  dsat_num_item_type            srv_num;
  wms_gw_domain_pref_e_type     domain_pref;
  dsat_num_item_type cgsms_val ;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  if(!dsatsmsi_wms_ready)
  {
    /* WMS subsystem is not ready: our status CB is called by WMS on
       SIM init completed so report SIM busy */
    dsatme_set_cme_error(DSAT_CME_SIM_BUSY, res_buff_ptr);
    return DSAT_CMD_ERR_RSP;
  }

  if (dsatsmsi_pres_cmd.cmd_name != SMS_NONE)
  {
    DS_AT_MSG1_ERROR("dsatsmsi_pres_cmd not clear, equals %d",
                                  dsatsmsi_pres_cmd.cmd_name);
    DSAT_SMS_STATE_CLEAN();
  }
  
  /*---------------------------------  
   Processing WRITE command
   ----------------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if(tok_ptr->args_found != 1)
    {
      /* number of arguments is not correct */
      return DSAT_ERROR;
    }
    
    /* convert the argument into numbers */
    if(dsatutil_atoi(&srv_num, tok_ptr->arg[0], 10))
    {
      /* wrong argument */
      return DSAT_ERROR;
    }

    /* Check if srv_num is in the supported range 0-3 */
    if(srv_num > dsat_cgsms_dflm.upper || srv_num < dsat_cgsms_dflm.lower )
    {
      return DSAT_ERROR;
    }

    /* After verification it's in supported range save value */
    DSATUTIL_SET_VAL(DSATETSI_EXT_CGSMS_IDX,subs_id,0,0,srv_num,NUM_TYPE)
    if ( !dsatsmsi_srv_to_domain_pref(srv_num, &domain_pref) )
    {
      /* wrong srv_num */
      return DSAT_ERROR;
    }
    
    dsatsmsi_pres_cmd.cmd_name = SMS_CGSMS;
#ifdef FEATURE_DUAL_SIM
    if(wms_cfg_ms_set_gw_domain_pref(dsatsmsi_client_id,
                                  subs_id,
                                  dsatsmsi_cmd_cb_func,
                                  NULL,
                                  domain_pref) == WMS_OK_S)
#else
    if(wms_cfg_set_gw_domain_pref(dsatsmsi_client_id,
                                  dsatsmsi_cmd_cb_func,
                                  NULL,
                                  domain_pref) == WMS_OK_S)
#endif /* FEATURE_DUAL_SIM */
    {
      DS_AT_MSG2_HIGH("Requested CGSMS mode: %d->%d",srv_num,domain_pref);
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      DSAT_SMS_STATE_CLEAN();
      result = DSAT_ERROR;
    }
  }

  /*---------------------------------  
   Processing READ command
   ----------------------------------*/
  else if ( tok_ptr->op == (NA|QU) )
  {
    cgsms_val =(dsat_num_item_type)dsatutil_get_val(
                  DSATETSI_EXT_CGSMS_IDX,subs_id,0,NUM_TYPE);
    if((cgsms_val <= dsat_cgsms_dflm.upper) && (cgsms_val >= dsat_cgsms_dflm.lower ))
    {
      result = dsatparm_exec_param_cmd ( mode,
                                         parse_table,
                                         tok_ptr,
                                         res_buff_ptr
                                         );
    }
    else
    {
      result = DSAT_ERROR;
    }
  }

  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    result = dsatparm_exec_param_cmd ( mode,
                                       parse_table,
                                       tok_ptr,
                                       res_buff_ptr
                                       );
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator:
   --------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatetsisms_exec_cgsms_cmd */

#endif /* FEATURE_ETSI_SMS_PS */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CSMS command. 
  +CSMS is used to select messaging service.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_csms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  dsm_item_type *buff_ptr;
  const byte *name_ptr;
  byte c;

  buff_ptr = res_buff_ptr;
  name_ptr = (const byte *)parse_table->name;   
  
  /*---------------------------------  
   Processing WRITE or READ command :
   ----------------------------------*/
  if ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA|QU)) )
  {
    /* do the usual parameter processing */
    result = dsatparm_exec_param_cmd ( 
                                      mode,
                                      parse_table,
                                      tok_ptr,
                                      res_buff_ptr
                                     );
    if ( result == DSAT_OK )
    {
      /* Formatting the response */
      /* response should include supported mt, mo, bm values */
      if ( tok_ptr->op == (NA|EQ|AR) )
      {
        /* if it is a write operation, we need some extra formatting */
        /*  "+<cmd name>: " */
        while ( (c = *name_ptr++) !=0 )
        {
          buff_ptr->data_ptr[buff_ptr->used++] = c;
        }
        
        buff_ptr->data_ptr[buff_ptr->used++] = ':';
        buff_ptr->data_ptr[buff_ptr->used++] = ' ';
      }
      else if ( tok_ptr->op == (NA|QU) )
      {
        /* don't need ',' if it is a write operattion */
        buff_ptr->used--;
        buff_ptr->data_ptr[buff_ptr->used++] = ',';
      }

      buff_ptr->data_ptr[buff_ptr->used++] = '1';
      buff_ptr->data_ptr[buff_ptr->used++] = ',';
      buff_ptr->data_ptr[buff_ptr->used++] = '1';
      buff_ptr->data_ptr[buff_ptr->used++] = ',';
#ifdef FEATURE_ETSI_SMS_CB
      buff_ptr->data_ptr[buff_ptr->used++] = '1';
#else
      buff_ptr->data_ptr[buff_ptr->used++] = '0';
#endif /* FEATURE_ETSI_SMS_CB */
      buff_ptr->data_ptr[buff_ptr->used] = '\0';
    }
    else 
    {
      /* return ERROR */
      result = DSAT_ERROR;
    }
    
    return result;
  }
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* do the usual parameter processing */
    result = dsatparm_exec_param_cmd ( 
                                       mode,
                                       parse_table,
                                       tok_ptr,
                                       res_buff_ptr
                                      );    
    return result;
  
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator:
   --------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
    return result;
  }

} /* dsatetsisms_exec_csms_cmd */


/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CPMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CPMS command. 
  +CPMS is used to select preferred memory storage for reading, writing etc.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:.
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cpms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  /* save the old mem3 type */
  dsat_num_item_type mem3_type;

  dsatsmsi_mem_report.mem3_changed = FALSE;

  /*----------------------------  
   Processing WRITE and READ command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) || (tok_ptr->op == (NA|QU)) )
  {
    /* for WRITE do the usual parameter processing */
    if ( tok_ptr->op == (NA|EQ|AR) )
    {
      /* do parameter checking */

      /* mem1 is compulsory */
      if(tok_ptr->arg[0][0] == '\0')
      {
        DS_AT_MSG0_ERROR("CPMS: mem1 is needed");
        return DSAT_ERROR;
      }
      
      mem3_type = (dsat_num_item_type)dsatutil_get_val(
          DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,CPMS_MEM3,NUM_TYPE);
      result = dsatparm_exec_param_cmd ( 
                                        mode,
                                        parse_table,
                                        tok_ptr,
                                        res_buff_ptr
                                        );

      if(mem3_type != (dsat_num_item_type)dsatutil_get_val(
           DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,CPMS_MEM3,NUM_TYPE))
      {
        dsatsmsi_mem_report.mem3_changed = TRUE;
      }

      dsatsmsi_mem_report.is_read = FALSE;
    }
    else /* for READ command */
    {
      dsatsmsi_mem_report.is_read = TRUE;
    }

    if ( result == DSAT_OK ) /* Set command passed */
    {
      /* get memory status for mem1, mem2, and mem3 */
      wms_status_e_type status;
      wms_memory_store_e_type mem_store;
      dsatsmsi_mem_report.mem = CPMS_MEM1;
      
      /* get memory status of msg store one at a time */
      
      /* set mem_store */
      mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
          DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,dsatsmsi_mem_report.mem,NUM_TYPE), 
          dsatsmsi_mem_report.mem,ETSI_CMD_MODE);

      /* call the wms function to get memory status */
      dsatsmsi_pres_cmd.cmd_name = SMS_CPMS;
      (void)strlcpy( dsatsmsi_pres_cmd.cmd_hdr, "+CPMS",
                     sizeof(dsatsmsi_pres_cmd.cmd_hdr));
      /* Check if the mem_store is "SR" , if so call the 
         function using the tag WMS_TAG_STATUS_RPT  */
      if( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CPMS_ETSI_IDX,
                      subs_id,dsatsmsi_mem_report.mem,NUM_TYPE) == 3 )
	  {
#ifdef FEATURE_DUAL_SIM
        status = wms_cfg_ms_get_memory_status(
                          dsatsmsi_client_id,
                          subs_id,
                          dsatsmsi_cmd_cb_func,
                          &dsatsmsi_pres_cmd,
                          mem_store,
                          WMS_TAG_STATUS_RPT); 
#else
        status = wms_cfg_get_memory_status(
                                            dsatsmsi_client_id,
                                            dsatsmsi_cmd_cb_func,
                                            &dsatsmsi_pres_cmd,
                                            mem_store,
                                            WMS_TAG_STATUS_RPT); 
#endif /* FEATURE_DUAL_SIM */
      }
      else
      {
#ifdef FEATURE_DUAL_SIM
        status = wms_cfg_ms_get_memory_status(
                          dsatsmsi_client_id,
                          subs_id,
                          dsatsmsi_cmd_cb_func,
                          &dsatsmsi_pres_cmd,
                          mem_store,
                          WMS_TAG_NONE);
#else
        status = wms_cfg_get_memory_status(
                                            dsatsmsi_client_id,
                                            dsatsmsi_cmd_cb_func,
                                            &dsatsmsi_pres_cmd,
                                            mem_store,
                                            WMS_TAG_NONE);
#endif /* FEATURE_DUAL_SIM */
      }

      /* If the command is not accepted, the command callback will
           not be called, the mem event will not be invoked */
      if(status == WMS_OK_S)
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        result = DSAT_ERROR;
        DSAT_SMS_STATE_CLEAN();
      }

    } /* if ( result == DSAT_OK ) */

    else
    {
      DS_AT_MSG0_ERROR("+CPMS write command fails");
      result = DSAT_ERROR;
      DSAT_SMS_STATE_CLEAN();
    }
    return result;
  }

  /*---------------------------------------
   Processing command with no args :
   ----------------------------------------*/
  else if (tok_ptr->op == (NA) ) 
  {
    DS_AT_MSG0_ERROR("+CPMS need at least one argument");
    result = DSAT_ERROR;
    DSAT_SMS_STATE_CLEAN();
    return result;
  }
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* do the usual parameter processing */
    result = dsatparm_exec_param_cmd ( 
                                      mode,
                                      parse_table,
                                      tok_ptr,
                                      res_buff_ptr
                                     );
    DSAT_SMS_STATE_CLEAN();
    return result;
  }
  else  /*  Wrong Operator  */
  {
    DSAT_SMS_STATE_CLEAN();
    result = DSAT_ERROR;
    return result;
  }
}/* dsatetsisms_exec_cpms_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSCA_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CSCA command. 
  +CSCA is used to select the SMSC address, through which mobile
  originated SMs are transmitted.

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
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_csca_cmd
(
 dsat_mode_enum_type mode,             /*  AT command mode:            */
 const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
 const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
 dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define TON_MASK  0x70
#define INATL_IND 0x10

  dsat_result_enum_type result;
  wms_address_s_type dummy_addr;
  boolean addr_result = FALSE;
  byte s[32];
  dsat_num_item_type cscs_val ;
  dsat_string_item_type * str_item;
  dsat_num_item_type temp_val =0;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  uint16 len_quoted_string = 0;

  memset ((void*)s, 0, sizeof(s));


  /*---------------------------------  
   Processing WRITE command :
   ----------------------------------*/
  if (tok_ptr->op == (NA|EQ|AR))
  {
    /* Check to see if the address is formatted correctly         */
    /* Strip out the quotes (if any ) from the address string     */
    /* The generic function we have gives an error if we give     */
    /* the destination address in quotes. We haven't done the     */
    /* Actual param processing yet, but this processing is        */
    /* local to this command and we would have to return an error */
    /* before actual param processing begins                      */

    /* Check for null parameter */
    if( !VALID_TOKEN(0) )
    {
      /* This is a mandatory argument */
      /* Error if not given           */
      return DSAT_ERROR;
    }

    len_quoted_string = (uint16)strlen((char*)tok_ptr->arg[0]);
    if( !dsatsmsi_strip_quotes ( tok_ptr->arg[0], tok_ptr->arg[0], len_quoted_string ) )
    {
      /* This is a mandatory argument */
      /* Error if not given           */
      return DSAT_ERROR;
    }

    /* Convert from TE's character set to IRA,
       return ERROR on inconvertible character */
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    if ( dsatutil_convert_chset( (const char *)tok_ptr->arg[0], 
                                 (dsati_chset_type)cscs_val,
                                 (char *)&s[0],
                                 ALPHA_IRA, 
                                 sizeof(s),
                                 FALSE ) == NULL )
    {
      return DSAT_ERROR;
    }

    if ( tok_ptr->args_found == 2 )
    {
      addr_result = dsatsmsi_str_to_addr( &dummy_addr, 
                                          s, 
                                          tok_ptr->arg[1], 
                                          DSATSMS_TEXT_MODE );
    }
    else
    {
      addr_result = dsatsmsi_str_to_addr( &dummy_addr,
                                          s,
                                          NULL,
                                          DSATSMS_TEXT_MODE );
    }

    if( !addr_result )
    {
      /* Bad address */

      return DSAT_ERROR;
    }
    else
    {
      /* We need to modify the token pointer here     */
      /* "+" in SCA takes precedence. So change toa   */
      /* Based on the "+". Also if you see 145 as toa */
      /* add a "+". */
      dsat_num_item_type toa;

#ifdef FEATURE_ETSI_SMS_TEMPLATE
      wms_status_e_type msg_status;
#endif

      if ( tok_ptr->args_found == 2 )
      {
        (void)dsatutil_atoi ( &toa, tok_ptr->arg[1], 10 );
      }
      else if ( VALID_TOKEN(0) && (*tok_ptr->arg[0] == '+' ) )
      {
        toa = 145;
      }
      else
      {
        toa = 129;
      }
      /* Storing the previous values */
      str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,subs_id,0,MIX_STR_TYPE);
      (void) strlcpy( (char *)dsat_csca_prev_val[0].string_item,
                        (const char *)str_item,
                        CSCA_SCA_STR_MAX_LEN+1);
      str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,subs_id,1,MIX_STR_TYPE);
      (void) strlcpy( (char *)dsat_csca_prev_val[1].string_item,
                        (const char *)str_item,
                        CSCA_TOSCA_STR_MAX_LEN+1);

      if ( *s == '+' )
      {
        (void)snprintf((char*)dsat_csca_sca_val[subs_id],
                             CSCA_SCA_STR_MAX_LEN +1,"%s", s);

        /* If number begins with '+' indicating international number but 
           <toa> does not indicate international, change <toa> */
        if ( (toa & TON_MASK) != INATL_IND )
        {
          toa = (toa & ~TON_MASK) | INATL_IND;
        }
      }
      else if ( toa == 145 )
      {
        (void)snprintf ( (char*)dsat_csca_sca_val[subs_id],
                               CSCA_SCA_STR_MAX_LEN+1,"+%s", s);
      }
      else
      {
        (void)snprintf ( (char*)dsat_csca_sca_val[subs_id],
                               CSCA_SCA_STR_MAX_LEN+1, "%s", s);
      }

      (void)snprintf ( (char*)dsat_csca_tosca_val[subs_id],
                             CSCA_TOSCA_STR_MAX_LEN+1, "%d", toa );

      result = DSAT_OK;
   
#ifdef FEATURE_ETSI_SMS_TEMPLATE
      /* Now save the address in SIM */
      /*template pointer should be NULL here*/
      ASSERT(template_ptr == NULL); 
      template_ptr = (wms_client_message_s_type *) dsat_alloc_memory(sizeof(wms_client_message_s_type),
                                                                     FALSE);

      template_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      template_ptr->msg_hdr.tag = WMS_TAG_MO_TEMPLATE;
      template_ptr->msg_hdr.index = 0;
      template_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_SIM;
      template_ptr->u.gw_template.mask = 
          template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_PID);
      template_ptr->u.gw_template.mask = 
          template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_DCS);
      template_ptr->u.gw_template.mask = 
          template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_SC_ADDR);

      temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,2,MIX_NUM_TYPE);
        
      template_ptr->u.gw_template.pid = (wms_pid_e_type)temp_val;

      temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,3,MIX_NUM_TYPE);

      dsatetsismsi_decode_sms_dcs(&template_ptr->u.gw_template.dcs, 
                                     temp_val);

      template_ptr->u.gw_template.sc_addr = dummy_addr;

      dsatsmsi_pres_cmd.cmd_name = SMS_CSCA;
        
#ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_read_template (dsatsmsi_client_id,
                                             subs_id,
                                             dsatsmsi_cmd_cb_func, NULL,
                                             WMS_MEMORY_STORE_SIM,0);
#else
      msg_status = wms_msg_read_template ( 
        dsatsmsi_client_id, dsatsmsi_cmd_cb_func, NULL,
        WMS_MEMORY_STORE_SIM,0);

#endif /* FEATURE_DUAL_SIM */

      if(msg_status == WMS_OK_S )
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        DSAT_SMS_STATE_CLEAN();
        /*Free the allocated memory for template_ptr*/
        dsatutil_free_memory_ext((void **)&template_ptr);
        template_ptr = NULL;
        result = DSAT_ERROR;
      }
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

    } /* if( !addr_result ) */
  }
  /*---------------------------------  
   Processing  READ command :
   ----------------------------------*/
  else if (tok_ptr->op == (NA|QU))
  {
    char *buff_ptr;
    cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
    res_buff_ptr->used = (word) snprintf( (char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "+CSCA: \"" );

    /* Convert <sca> from IRA to TE's char set,
       dropping inconvertible characters */
    buff_ptr = dsatutil_convert_chset( 
      (const char *)dsat_csca_sca_val[subs_id], 
      ALPHA_IRA,
      (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
      (dsati_chset_type)cscs_val, 
      res_buff_ptr->size - res_buff_ptr->used,
      TRUE );    

    res_buff_ptr->used = (uint16)( (uint32)buff_ptr - 
                                   (uint32)res_buff_ptr->data_ptr );

    res_buff_ptr->used += 
      (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                             res_buff_ptr->size - res_buff_ptr->used, 
                             "\",%s", (char*)&dsat_csca_tosca_val[subs_id][0] );

    result = DSAT_OK;  
  }
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return OK */
    result = DSAT_OK;  
  }
  /*---------------------------------------------------
   Processing command with no args and wrong operator :
  -----------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;  
}/* dsatetsisms_exec_csca_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSMP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CSMP command. 
  +CSMP is used to select additional parameters that are needed to 
  send or to store SM.

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
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_csmp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  dsat_num_item_type temp_num;
  unsigned int i = 0;
  wms_gw_validity_s_type timestamp;

  dsat_num_item_type  temp_num_val = 0;
  byte               *temp_str_val = NULL;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  memset(&timestamp, 0, sizeof(wms_gw_validity_s_type));
  
  /*---------------------------------  
   Processing WRITE or READ command :
   ----------------------------------*/
  if ( (tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA|QU)))
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      /* Check for the range of the individual arguments */
      /* For <fo>, <pid> check to see if they are greater than 255 */
      while ( i < tok_ptr->args_found && i < 4 )
      {
        /* Do not do this check for <vp> */
        if((VALID_TOKEN(i)) && (i != 1))
        {
          if (dsatutil_atoi ( &temp_num, tok_ptr->arg[i], 10 ) != ATOI_OK)
          {
            /* We got a bad argument */
            return DSAT_ERROR;
          }
          else
          {
            if( temp_num > 255 )
            {
              /* out of bound */
              return DSAT_ERROR;
            }
          }
        }
        i++;
      }
      temp_str_val = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,0,MIX_STR_TYPE);
      /* If null <fo> parameter on set command line and no */
      /* previously defined value then err */
      if ((VALID_TOKEN(0) && (0 == *tok_ptr->arg[0])) &&
          ( 0 == *temp_str_val ))
      {
        return DSAT_ERROR;
      }

      /* if attempting to set <fo> and can't convert to int or */
      /* <fo>'s UDHI bit is true then err  */
      if ( VALID_TOKEN(0) )
      {
        if ( dsatutil_atoi(&temp_num, tok_ptr->arg[0], 10) != ATOI_OK )
        {
          return DSAT_ERROR;
        }

      }
    
      if ( tok_ptr->args_found == 4 &&
           !dsatetsismsi_validate_sms_dcs(tok_ptr) )
      {
        return DSAT_ERROR;
      }

      /* always validate vp: */
      /* user could change <fo> to indicate absolute <vpf> and didn't */
      /* provided needed <vp>, this could be the case regardless of */
      /* number of args indicated e.g. AT+CSMP=25 or AT+CSMP=25,,, */
      /* Upon a valid vp entry get the value in the timestamp for */
      /* sending it to WMS*/
      if ( !dsatetsismsi_validate_vp( tok_ptr ,&timestamp ) )
      {
        return DSAT_ERROR;
      }
    }

    /* do the usual parameter processing */
    result = dsatparm_exec_param_cmd (  mode,
                                        parse_table,
                                        tok_ptr,
                                        res_buff_ptr
                                      );

#ifdef FEATURE_ETSI_SMS_TEMPLATE
    if ( ( result == DSAT_OK ) && ( tok_ptr->op == (NA|EQ|AR) ) )
    {
      /* All the parameter validation is OK, so write the  */
      /* values into a template stored in SIM.             */
      /* Write to template only if it is a WRITE operation */
      wms_status_e_type msg_status;
      wms_address_s_type addr;

      /*template pointer should be NULL here*/
      ASSERT(template_ptr == NULL);

      template_ptr = (wms_client_message_s_type *) dsat_alloc_memory(sizeof(wms_client_message_s_type),
                                                                     FALSE);

      template_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      template_ptr->msg_hdr.tag = WMS_TAG_MO_TEMPLATE;
      template_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_SIM;
      template_ptr->msg_hdr.index = 0;
      
      template_ptr->u.gw_template.mask = 
        template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_PID);
      template_ptr->u.gw_template.mask = 
        template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_DCS);

      temp_num_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,2,MIX_NUM_TYPE);
       
      template_ptr->u.gw_template.pid = (wms_pid_e_type)temp_num_val;


      temp_num_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,3,MIX_NUM_TYPE);
      
      dsatetsismsi_decode_sms_dcs(&template_ptr->u.gw_template.dcs, 
                                  temp_num_val);
      
      template_ptr->u.gw_template.mask = 
        template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_VALIDITY);
      template_ptr->u.gw_template.relative_validity = timestamp.u.time;

      if ( dsatetsismsi_get_sca ( &addr, SMS_TEXT_MODE) )
      {
        template_ptr->u.gw_template.mask = 
          template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_SC_ADDR);
        template_ptr->u.gw_template.sc_addr = addr;
      }

      dsatsmsi_pres_cmd.cmd_name = SMS_CSMP;

#ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_read_template ( 
        dsatsmsi_client_id,subs_id,
        dsatsmsi_cmd_cb_func, NULL,
        WMS_MEMORY_STORE_SIM,0);
#else
      msg_status = wms_msg_read_template ( 
        dsatsmsi_client_id, dsatsmsi_cmd_cb_func, NULL,
        WMS_MEMORY_STORE_SIM,0);

#endif /* FEATURE_DUAL_SIM */
      if(msg_status == WMS_OK_S )
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        DSAT_SMS_STATE_CLEAN();
        /*Free the allocated memory for template_ptr*/
        dsatutil_free_memory_ext((void **)&template_ptr);
        result = DSAT_ERROR;
      }

    } /* if ( ( result == DSAT_OK ) && ( tok_ptr->op == (NA|EQ|AR) ) ) */
#endif /* FEATURE_ETSI_SMS_TEMPLATE */

  } /* Processing WRITE or READ command */
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return OK */
    result = DSAT_OK;
    return result;
  
  }
  /*---------------------------------------------------
   Processing command with no args and wrong operator :
  -----------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

  return result;
}/* dsatetsisms_exec_csmp_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CNMI_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CNMI command. 
  +CNMI is used to select the procedure of how received new messages
  from the network is indicated to TE.

  This function is also used for $QCCNMI. This simulates CNMI but to 
  pass the GCF testcases 34.2.5.1, 34.2.3, 34.2.1; little modifications 
  is done. for $QCCNMI=<mode>,<mt=2> this is applicable. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cnmi_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  unsigned int used_val;
  dsm_item_type *buff_ptr;
  dsat_num_item_type mode_val;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  buff_ptr = res_buff_ptr;

  if( ( tok_ptr->args_found > 0 ) && ( VALID_TOKEN(0) ) )
  {
    if ( dsatutil_atoi ( &mode_val, tok_ptr->arg[0], 10 ) )
    {
      return DSAT_ERROR;
    }
  }
  else
  {
    mode_val = (dsat_num_item_type)dsatutil_get_val(
                      DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_MODE_VAL,NUM_TYPE);
  }
  
  if( tok_ptr->op == (NA|EQ|AR) )
  {
    if ( ( ( VALID_TOKEN(3) && ( *tok_ptr->arg[3] == '1' ) ) && ( mode_val != 1 ) ) ||
         ( ( VALID_TOKEN(1) && ( *tok_ptr->arg[1] == '2' ) ) && ( mode_val != 1 ) ) ||
         ( ( VALID_TOKEN(1) && ( *tok_ptr->arg[1] == '3' ) ) && ( mode_val != 1 ) ) ||
	       ( VALID_TOKEN(2) && ( *tok_ptr->arg[2] == '1' ) ) )/* Currently <bm> = 1 is not supported */
    {
      /* we got an error, format a +CMS ERROR */
      used_val = 
        dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_OP_NOT_SUPPORTED);
      buff_ptr->used += (byte)used_val;
      result = DSAT_CMD_ERR_RSP;
      return result;
    }
  }
  

  result = dsatparm_exec_param_cmd ( mode,
                                      parse_table,
                                      tok_ptr,
                                      res_buff_ptr
                                    );
  
  if ( result == DSAT_ERROR )
  {
    if ( ( tok_ptr->op == (NA|EQ|AR) )&& 
		 ( tok_ptr->args_found == 1 ) && 
		 ( !( VALID_TOKEN(0) ) ) )
    {
      /* We got badly formatted command */
      return result;
    }
    else
    {
      /* we got an error, format a +CMS ERROR */
      used_val = 
        dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_OP_NOT_SUPPORTED);
      buff_ptr->used += (byte)used_val;
      result = DSAT_CMD_ERR_RSP;
      return result;
    }
  }
  else
  {
    if ( ( tok_ptr->op == (NA|EQ|AR) ) || ( tok_ptr->op == (NA) ) )
    {
      dsat_num_item_type mt_val = (dsat_num_item_type)dsatutil_get_val(
          DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_MT_VAL,NUM_TYPE);
      dsat_num_item_type ds_val = (dsat_num_item_type)dsatutil_get_val(
          DSATETSI_EXT_ACT_CNMI_ETSI_IDX,subs_id,CNMI_DS_VAL,NUM_TYPE);

      /*To pass the GCF test cases 34.2.5.1, 34.2.3, 34.2.1; introduced,
        $QCCNMI command which is exactly same as +CNMI with only difference
        being for <mt> value 2; In this case, when mt=2, set the route
        config as similar to mt=0 (mt=9 is identical to mt=0 but to separate
        it out from spec defined value, mt=9 is used) but the dsat_cnmi_val[1]=2 
        which is furthur used to route the messages as required by GCF testcase*/
      if ( (dsatutil_strcmp_ig_sp_case(
              (const byte*)tok_ptr->name, (const byte*)"$QCCNMI") == 0) && 
           ( mt_val == 2 ) )
      {
        mt_val = 9; /* mt val is set to 4 to ensure that it doesnt
                       really change as in mt=2 as explained above*/
      }
      dsatsmsi_pres_cmd.cmd_name = SMS_CNMI;
      result = dsatsmsi_route_config ( mt_val, ds_val,subs_id ); 
      if(result != DSAT_ASYNC_CMD)
      {
        DSAT_SMS_STATE_CLEAN();
      }
    }
  }


  return result;
}/* dsatetsisms_exec_cnmi_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGL command. 
  +CMGL is used to list message with status value from message
  storage <mem1> to TE.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmgl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  byte *data_ptr;
  dsat_string_item_type txt_tststr [] = 
    "(\"REC UNREAD\",\"REC READ\",\"STO UNSENT\",\"STO SENT\",\"ALL\")";
  LOCAL const dsat_string_item_type pdu_tststr [] = "(0-4)";
  int all_flag=1;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  data_ptr = res_buff_ptr->data_ptr;
  
  mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                   DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,0,NUM_TYPE),
                   CPMS_MEM1,ETSI_CMD_MODE);
  
  /*-------------------------------------  
   Processing WRITE and DEFAULT command :
   -------------------------------------*/
  if ( ( tok_ptr->op == (NA|EQ|AR) ) || ( tok_ptr->op == (NA) ) )
  {
    mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                   DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,0,NUM_TYPE),
                   CPMS_MEM1,ETSI_CMD_MODE);
    if ( tok_ptr->op == (NA|EQ|AR) )
    {
      dsatsmsi_pres_cmd.cmd_name = SMS_CMGL;
      if( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) )
      {
        tag = stat_lookup(tok_ptr->arg[0], 0, &all_flag);
      }
      else
      {
        tag = stat_lookup( tok_ptr->arg[0], 1, &all_flag);
      }
      DSAT_SMS_STATE_CLEAN();
      
      if ( !tag )
      {
        /* Bad Tag */
        /* Return an error */
        return DSAT_ERROR;
      }
      else
      {
        dsatsmsi_cmd_sm.stat_list[0] = tag;
      }
    }
    else
    {
      /* We are in NA case */
      tag = WMS_TAG_MT_NOT_READ;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_NOT_READ;
    }
    

    if (!all_flag)
    {
      /* We got an ALL flag */
      /* If the memory is for SR, query it ONLY using STATUS_RPT tag*/
      if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_ACT_CPMS_ETSI_IDX,
                                     subs_id,CPMS_MEM1,NUM_TYPE) == 3)
      {
        dsatsmsi_cmd_sm.num_of_stat = 1;
        dsatsmsi_cmd_sm.present_stat = 1;
        dsatsmsi_cmd_sm.list_begin = TRUE;
        dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_STATUS_RPT;
        tag = WMS_TAG_STATUS_RPT;
      }
      else
      {
        dsatsmsi_cmd_sm.num_of_stat = 7;
        dsatsmsi_cmd_sm.present_stat = 1;
        dsatsmsi_cmd_sm.list_begin = TRUE;
        dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
        dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MT_NOT_READ;
        dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_SENT;
        dsatsmsi_cmd_sm.stat_list[3] = WMS_TAG_MO_NOT_SENT;
        dsatsmsi_cmd_sm.stat_list[4] = WMS_TAG_MO_SENT_ST_NOT_RECEIVED;
        dsatsmsi_cmd_sm.stat_list[5] = WMS_TAG_MO_SENT_ST_NOT_STORED;
        dsatsmsi_cmd_sm.stat_list[6] = WMS_TAG_MO_SENT_ST_STORED;
      }
    }
    else if (tag == WMS_TAG_MO_SENT)
    {
      /* We got WMS_TAG_MO_SENT */
      dsatsmsi_cmd_sm.num_of_stat = 4;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.list_begin = TRUE;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MO_SENT;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MO_SENT_ST_NOT_RECEIVED;
      dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_SENT_ST_NOT_STORED;
      dsatsmsi_cmd_sm.stat_list[3] = WMS_TAG_MO_SENT_ST_STORED;
    }
    else
    {
      /* We are listing a single stat */
      dsatsmsi_cmd_sm.num_of_stat = 1;
      dsatsmsi_cmd_sm.list_begin = TRUE;
      dsatsmsi_cmd_sm.present_stat = 1;
    }

    dsatsmsi_pres_cmd.cmd_name = SMS_CMGL;
   #ifdef FEATURE_DUAL_SIM
    msg_status = wms_cfg_ms_get_message_list(
                              dsatsmsi_client_id,
                              subs_id,
                              dsatsmsi_cmd_cb_func,
                              (void *)&dsatsmsi_pres_cmd,
                              mem_store,
                              tag
                             );
#else
    msg_status = wms_cfg_get_message_list(
                              dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void *)&dsatsmsi_pres_cmd,
                              mem_store,
                              tag
                             );
#endif /* FEATURE_DUAL_SIM */

    if(msg_status == WMS_OK_S )
    {
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      DSAT_SMS_STATE_CLEAN();
      result = DSAT_ERROR;
    }

    return result;
  }
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    res_buff_ptr->used = (word)snprintf((char*)data_ptr,
                                              res_buff_ptr->size,
                                              "%s: ",
                                              tok_ptr->name); 
    data_ptr += res_buff_ptr->used;

    if((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE))
    {
      res_buff_ptr->used += (word)snprintf ( (char*)data_ptr,
                                        res_buff_ptr->size - res_buff_ptr->used,
                                        "%s", txt_tststr);
    }
    else
    {
      res_buff_ptr->used += (word)snprintf ( (char*)data_ptr,
                                        res_buff_ptr->size - res_buff_ptr->used,
                                        "%s", pdu_tststr);
    }
    result = DSAT_OK;
    return result;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

}/* dsatetsisms_exec_cmgl_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGR command. 
  +CMGR is used to read message with location value <index> from message
  storage <mem1>.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmgr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  dsm_item_type *buff_ptr;
  wms_memory_store_e_type mem_store;
  wms_status_e_type msg_status;
  dsat_num_item_type index;
  unsigned int used_val;
  byte sr_storage[] = "SR";
  const dsat_string_item_type * mem_valstr;
  const def_list_type * list_ptr;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  buff_ptr = res_buff_ptr;
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                   DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,0,NUM_TYPE),
                   CPMS_MEM1,ETSI_CMD_MODE);
    
    if (dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK)
    {
      /* We got a bad index */
      return DSAT_ERROR;
    }
    dsatsmsi_pres_cmd.cmd_name = SMS_CMGR;

    /* get the right mem_valstr */
    list_ptr = dsatsmsi_get_mem_list(CPMS_MEM1,ETSI_CMD_MODE);

    /* sanity check */
    if (list_ptr == NULL)
    {
      /* we are in wrong mode: neither ETSI nor CDMA. Its a bigger problem */
      return DSAT_ERROR;
    }
	
    mem_valstr = list_ptr->list_v[dsatsmsi_get_mem_val(CPMS_MEM1,
                                                       subs_id,
                                                       ETSI_CMD_MODE)];

    /* lookup for the type of memory set */
    if( strcmp( (char *)sr_storage, (char *) mem_valstr ) == 0 )
    {
      /* we are trying to read STATUS-REPORT */
   #ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_read_sts_report
                       (
                         dsatsmsi_client_id,
                         subs_id,
                         dsatsmsi_cmd_cb_func,
                         (void*)&dsatsmsi_pres_cmd,
                         mem_store,
                         index
                       );

#else
      msg_status = wms_msg_read_sts_report
                       (
                         dsatsmsi_client_id,
                         dsatsmsi_cmd_cb_func,
                         (void*)&dsatsmsi_pres_cmd,
                         mem_store,
                         index
                       );
#endif /* FEATURE_DUAL_SIM */
    }
    else
    {
#ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_read(
                     dsatsmsi_client_id,
                     subs_id,
                     dsatsmsi_cmd_cb_func,
                     (void*)&dsatsmsi_pres_cmd,
                     mem_store,
                     index
                     );
#else
      msg_status = wms_msg_read(
                     dsatsmsi_client_id,
                     dsatsmsi_cmd_cb_func,
                     (void*)&dsatsmsi_pres_cmd,
                     mem_store,
                     index
                     );
#endif /* FEATURE_DUAL_SIM */
    }

    if (msg_status == WMS_OK_S)
    {
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      /* we got an error, format a +CMS ERROR */
      DSAT_SMS_STATE_CLEAN();
      used_val = dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_NONE);
      buff_ptr->used += (byte)used_val;
      result = DSAT_CMD_ERR_RSP;
    }
    return result;
  }
  /*---------------------------  
   Processing READ command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|QU) )
  {
    /* It is an error to query this command */
    result = DSAT_ERROR;
    return result;
  }
  /*--------------------------------------
   Processing a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return OK */
    result = DSAT_OK;
    return result;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

}/* dsatetsisms_exec_cmgr_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGD command. 
  +CMGD is used to delete message with location value <index> from message
  storage. Wild cards (<del Flags>) can also be used in deleting messages.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmgd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type    result;
  dsm_item_type           *buff_ptr;
  wms_memory_store_e_type  mem_store;
  dsat_num_item_type       index;
  wms_status_e_type        msg_status;
  unsigned int             used_val;
  sys_modem_as_id_e_type   subs_id = dsat_get_current_subs_id(FALSE);

  buff_ptr = res_buff_ptr;
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if( (tok_ptr->args_found == 1) || (*tok_ptr->arg[1] == '0') )
    {
      mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                                    DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,0,NUM_TYPE),
                                    CPMS_MEM1,ETSI_CMD_MODE);
      if(dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK )
      {
        /* We received a bad index */
        return DSAT_ERROR;
      }
      dsatsmsi_pres_cmd.cmd_name = SMS_CMGD;
#ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_delete(
                    dsatsmsi_client_id,
                    subs_id,
                    dsatsmsi_cmd_cb_func,
                    (void*)&dsatsmsi_pres_cmd,
                    mem_store,
                    index
                   );
#else
      msg_status = wms_msg_delete(
                    dsatsmsi_client_id,
                    dsatsmsi_cmd_cb_func,
                    (void*)&dsatsmsi_pres_cmd,
                    mem_store,
                    index
                   );
#endif /* FEATURE_DUAL_SIM */

      dsatsmsi_cmd_sm.num_of_stat = 0;
      dsatsmsi_cmd_sm.present_index = 0;
      dsatsmsi_cmd_sm.num_of_indices = 0;
      dsatsmsi_cmd_sm.is_test =  FALSE;
      dsatsmsi_cmd_sm.one_index = TRUE;

      if (msg_status == WMS_OK_S)
      {
        result = DSAT_ASYNC_CMD;
      }
      else 
      {
        /* we got an error, format a +CMS ERROR */
        DSAT_SMS_STATE_CLEAN();
        used_val = dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_NONE);
        buff_ptr->used += (byte)used_val;
        result = DSAT_CMD_ERR_RSP;
      }
      return result;
    }
    else if ( tok_ptr->args_found == 2 )
    {
      dsatsmsi_pres_cmd.cmd_name = SMS_CMGD;
      result = dsatsmsi_delete_messages(tok_ptr->arg[1]);
      if (result == DSAT_OK)
      {
        result = DSAT_ASYNC_CMD;
      }
      else 
      {
        /* we got an error, format a +CMS ERROR */
        DSAT_SMS_STATE_CLEAN();
        used_val = dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_NONE);
        buff_ptr->used += (byte)used_val;
        result = DSAT_ERROR;
      }
      return result;
    }
    else
    {
      /* Wrong number of arguments */
      return DSAT_ERROR;
    }
  } /* Write command */
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                                   DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,0,NUM_TYPE),
                                   CPMS_MEM1,ETSI_CMD_MODE);
    dsatsmsi_pres_cmd.cmd_name = SMS_CMGD;
 #ifdef FEATURE_DUAL_SIM
    msg_status = wms_cfg_ms_get_message_list(
                             dsatsmsi_client_id,
                             subs_id,
                             dsatsmsi_cmd_cb_func,
                             (void*)&dsatsmsi_pres_cmd,
                             mem_store,
                             WMS_TAG_NONE
                            );
#else
    msg_status = wms_cfg_get_message_list(
                             dsatsmsi_client_id,
                             dsatsmsi_cmd_cb_func,
                             (void*)&dsatsmsi_pres_cmd,
                             mem_store,
                             WMS_TAG_NONE
                            );
#endif /* FEATURE_DUAL_SIM */

    dsatsmsi_cmd_sm.is_test =  TRUE;
    dsatsmsi_cmd_sm.one_index = FALSE;

    if (msg_status == WMS_OK_S)
    {
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      /* we got an error, format a +CMS ERROR */
      DSAT_SMS_STATE_CLEAN();
      used_val = dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_NONE);
      buff_ptr->used += (byte)used_val;
      result = DSAT_CMD_ERR_RSP;
    }
    return result;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

}/* dsatetsisms_exec_cmgd_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMSS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMSS command. 
  +CMSS is used to send a message with location value (<index>) from
  preferred message storage to the network..

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmss_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type    result; 

  /* data structures used in wms_msg_send */
  wms_send_mode_e_type     send_mode;
  wms_status_e_type        msg_status;
  wms_memory_store_e_type  mem_store;
  dsm_item_type           *buff_ptr;
  atoi_enum_type           err_type;
  dsat_num_item_type       index;
  byte                     s[32];
  dsat_num_item_type       cscs_val ;
  sys_modem_as_id_e_type   subs_id = dsat_get_current_subs_id(FALSE);
  wms_client_message_s_type *sms_msg_ptr = NULL;
  uint16 len_quoted_string = 0;
  
  memset ((void*)s, 0, sizeof(s));
  buff_ptr = res_buff_ptr;

  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    /* Case when we have to just send the message from specified index */
    if( tok_ptr->args_found == 1 )
    {
      /*convert the index into a suitable number */
      err_type = dsatutil_atoi(&index, tok_ptr->arg[0], 10);
      if (err_type != ATOI_OK)
      {
        return DSAT_ERROR;
      }

      sms_msg_ptr = (wms_client_message_s_type *) dsat_alloc_memory(sizeof(wms_client_message_s_type),
                                                                    FALSE);
      /* get the memory from which the message has to be picked up */
      sms_msg_ptr->msg_hdr.mem_store = 
        dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                   DSATETSI_EXT_ACT_CPMS_ETSI_IDX,dsat_get_qcsimapp_val(),1,NUM_TYPE), 
                   CPMS_MEM2,ETSI_CMD_MODE);

      sms_msg_ptr->msg_hdr.index = index;
    
      /* fill out rest of the enums */
      sms_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
      sms_msg_ptr->msg_hdr.tag = WMS_TAG_NONE;
      /* We only have index, assume SMS-SUBMIT */
      sms_msg_ptr->u.gw_message.raw_ts_data.tpdu_type = WMS_TPDU_SUBMIT;
      send_mode = WMS_SEND_MODE_MEMORY_STORE;

      /* finally we are done stuffing all the required parameters */
      /* send the message to WMS                                */
      dsatsmsi_pres_cmd.cmd_name = SMS_CMSS;
#ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_send(
                                 dsatsmsi_client_id,
                                 subs_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 send_mode,
                                 sms_msg_ptr
                                );
#else
      msg_status = wms_msg_send(
                                 dsatsmsi_client_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 send_mode,
                                 sms_msg_ptr
                                );
#endif /* FEATURE_DUAL_SIM */

      /*Free the allocated memory for sms_msg_ptr*/
      dsatutil_free_memory_ext((void **)&sms_msg_ptr);
    }
    /* We have to overwrite the <da> */
    /* Read the message first        */
    else
    {
      /* Save the <da>                                                      */
      /* It will be used to replace existing <da> after we read the message */

      memset( (void *) &dsatetsismsi_send_addr, 0, sizeof(wms_address_s_type));

      if( tok_ptr->args_found >=2 )
      {
        if( VALID_TOKEN(1) )
        {
          len_quoted_string = (uint16)strlen((char*)tok_ptr->arg[1]);
          /* Strip out the '"' from the <da> in place */
          if ( !dsatsmsi_strip_quotes( tok_ptr->arg[1], tok_ptr->arg[1], len_quoted_string ) )
          {
            /* We got an address with out quotes */
            return DSAT_ERROR;
          }

          /* Convert from TE's character set to IRA,
             return ERROR on inconvertible character */
          cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
          if ( dsatutil_convert_chset( (const char *)tok_ptr->arg[1], 
                                       (dsati_chset_type)cscs_val,
                                       (char *)&s[0],
                                       ALPHA_IRA, 
                                       sizeof(s),
                                       FALSE ) == NULL )
          {
            return DSAT_ERROR;
          }
        }
        else
        {
          return DSAT_ERROR;
        }
      }

      switch(tok_ptr->args_found)
      {
        /* Note: <toda> is always decimal number */
        case 2:
          if(!dsatsmsi_str_to_addr
             (&dsatetsismsi_send_addr,s,NULL,DSATSMS_TEXT_MODE)
            )
          {
            /* Bad Address */
            return DSAT_ERROR;
          }
          break;
      
        case 3:
          if (!dsatsmsi_str_to_addr
              ( &dsatetsismsi_send_addr, s, tok_ptr->arg[2], DSATSMS_TEXT_MODE )
             )
          {
            /* Bad Address */
            return DSAT_ERROR;
          }
          break;

        default:
          /* we shouldn't be here */
          break;
      }
      
      mem_store = dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
                   DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,0,NUM_TYPE),
                   CPMS_MEM1,ETSI_CMD_MODE);
    
      if (dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK)
      {
        /* We got a bad index */
        return DSAT_ERROR;
      }
      /* Read the message */
      /* once we get the message back from the WMS */
      /* Overwrite the present <da> with the new one (dsatetsismsi_send_addr) */
      dsatsmsi_pres_cmd.cmd_name = SMS_CMSS;
#ifdef FEATURE_DUAL_SIM
      msg_status = wms_msg_ms_read(
                                dsatsmsi_client_id,
                                subs_id,
                                dsatsmsi_cmd_cb_func,
                                (void*)&dsatsmsi_pres_cmd,
                                mem_store,
                                index
                               );
#else
      msg_status = wms_msg_read(
                                dsatsmsi_client_id,
                                dsatsmsi_cmd_cb_func,
                                (void*)&dsatsmsi_pres_cmd,
                                mem_store,
                                index
                               );
#endif /* FEATURE_DUAL_SIM */
    }

    if (msg_status == WMS_OK_S)
    {
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      DSAT_SMS_STATE_CLEAN();
      result = DSAT_ERROR;
    }
    return result;
  }
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return a NULL string */
    buff_ptr->data_ptr[0]='\0';
    result = DSAT_OK;
    return result;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

}/* dsatetsisms_exec_cmss_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGS command. 
  +CMGS is used to send a message from TE to the network.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmgs_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsm_item_type *buff_ptr;
  dsat_result_enum_type result;
  byte s[32];
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
  dsat_num_item_type cscs_val ;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  uint16 len_quoted_string = 0;

  memset ((void*)s, 0, sizeof(s));
  buff_ptr = res_buff_ptr;
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    /* Retain the parameters from the this command */
    /* they will be used later when sending the message */
    /* In case of text mode, <da> is one of the parameter */
    /* In case of pdu mode, <length> is the parameter */
    if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) )
    {
      byte *toda = NULL, *temp_fo;
      wms_address_s_type addr;
      dsat_num_item_type fo;

      /* verify that +CSMP's <fo> parm indicates SMS-SUBMIT */
      temp_fo = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,0,MIX_STR_TYPE);
      
      if ( *temp_fo == '\0' ||
           dsatutil_atoi(&fo, temp_fo, 10) )
      {
        /* Assume SMS-SUBMIT if <fo> not set or 
           we get an error in converting the <fo> */
        fo = 17;
      }
      if ( (fo & 0x03) != 1 )
      {
        res_buff_ptr->used =  
          (byte)dsatsmsi_cms_error( res_buff_ptr, 
                                    (int)DSAT_CMS_INVALID_TXT_PARAM
                                  );
        return DSAT_CMD_ERR_RSP;
      }
     
      /* If user specified a destination address... */
      if( VALID_TOKEN(0) )
      {
        len_quoted_string = (uint16)strlen((char*)tok_ptr->arg[0]);
        /* Strip quotes in place */
        if ( !dsatsmsi_strip_quotes( tok_ptr->arg[0], tok_ptr->arg[0], len_quoted_string ) )
        {
          /* We got an address with out quotes */
          return DSAT_ERROR;
        }

        /* Convert from TE's character set to IRA,
           return ERROR on inconvertible character */
        cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
        if ( dsatutil_convert_chset( (const char *)tok_ptr->arg[0], 
                                     (dsati_chset_type)cscs_val,
                                     (char *)&s[0],
                                     ALPHA_IRA, 
                                     sizeof(s),
                                     FALSE ) == NULL )
        {
          return DSAT_ERROR;
        }
      }
      else
      {
        return DSAT_ERROR;
      }
    
      if ( tok_ptr->args_found == 2 )
      {
        toda = tok_ptr->arg[1];
      }
      
      /* Check to see if service center address from previous +CSCA command */
      if ( !dsatetsismsi_get_sca (&addr, SMS_TEXT_MODE) )
      {
        /* Service center is not given. Abort */
        res_buff_ptr->used =  
          (byte)dsatsmsi_cms_error(res_buff_ptr, 
                                   (int)DSAT_CMS_SCA_ADDR_UNKNOWN );
        return DSAT_CMD_ERR_RSP;
      }

      if(!dsatsmsi_str_to_addr
         (&dsatetsismsi_send_addr, s, toda, DSATSMS_TEXT_MODE)
         )
      {
        /* We got an error in address processing */
	       return DSAT_ERROR;
      }
    }
    else
    {
      /* pdu mode */
      if(
        !(
          dsatutil_atoi(&dsatetsismsi_send_length,tok_ptr->arg[0],10)==ATOI_OK 
          && 
          dsatetsismsi_send_length > 0 
          && 
          dsatetsismsi_send_length <= MAX_PDU_LENGTH
         )
         )
      {
        /* We got a bad length */
        res_buff_ptr->used = 
          (byte) dsatsmsi_cms_error(res_buff_ptr, 
                                    (int)DSAT_CMS_INVALID_PDU_PARAM );
        return DSAT_CMD_ERR_RSP;
      }
    }
      
    dsatetsismsi_send_stat = WMS_TAG_MO_NOT_SENT;
    dsatsmsi_pres_cmd.cmd_name = SMS_CMGS;
    /* Send mesasge to SIO data preprocessor to change state to */
    /* handle SMS message entry */
    dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );

    return DSAT_ASYNC_CMD;
  }
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return a NULL string */
    buff_ptr->data_ptr[0]='\0';
    result = DSAT_OK;
    return result;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

}/* dsatetsisms_exec_cmgs_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGW command. 
  +CMGW is used to store message to memory storage <mem2>

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmgw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte s[32];
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
  dsat_num_item_type cscs_val ;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  uint16 len_quoted_string = 0;

  memset ((void*)s, 0, sizeof(s));
  /*-------------------------
   Processing WRITE command:
   --------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) )
    {
      byte *temp_fo;
      dsat_num_item_type fo_mti;

      len_quoted_string = (uint16)strlen((char*)tok_ptr->arg[0]);
      /* We are in TEXT mode */
      /* convert the <da> to suitable a data structure */

      /* Strip out the '"' from the <da> in place */
      if ( !dsatsmsi_strip_quotes( tok_ptr->arg[0], tok_ptr->arg[0], len_quoted_string ) )
      {
        result = DSAT_CMD_ERR_RSP;
      }

      /* Convert from TE's character set to IRA,
         return ERROR on inconvertible character */
      cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
      if ( dsatutil_convert_chset( (const char *)tok_ptr->arg[0], 
                                   (dsati_chset_type)cscs_val,
                                   (char *)&s[0],
                                   ALPHA_IRA, 
                                   sizeof(s),
                                   FALSE ) == NULL )
      {
        return DSAT_ERROR;
      }

      /* extract TP-MTI from <fo> */
      temp_fo = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,0,MIX_STR_TYPE);
      if ( *temp_fo == '\0' )
      {
        /* If +CSMP 's first parm not set, default to submit */
        fo_mti = 1;
      }     
      else
      {
        if ( dsatutil_atoi(&fo_mti, temp_fo, 10) != ATOI_OK )
        {
          return DSAT_ERROR;
        }

        fo_mti = fo_mti & 0x03;
      }

      switch ( tok_ptr->args_found )
      {
        /* no <stat> given */
        case 1:
        case 2:
        {
          byte * toa_ptr = NULL;
          if(tok_ptr->args_found == 2)
          {
            toa_ptr = tok_ptr->arg[1];
          }

          if(!dsatsmsi_str_to_addr
             (&dsatetsismsi_send_addr, s, toa_ptr, DSATSMS_TEXT_MODE)
            )
          {
            /* We got a bad address */
            result = DSAT_CMD_ERR_RSP;
          }
          else
          {
            /* judge tpdu type from +CSMP <fo>'s MTI bits */
            dsatetsismsi_send_tpdu = (fo_mti == 1) ? 
                                     WMS_TPDU_SUBMIT : 
                                     WMS_TPDU_DELIVER;

            /* judge <stat> from +CSMP <fo>'s MTI bits1 */
            dsatetsismsi_send_stat = (fo_mti == 1) ?
                                     WMS_TAG_MO_NOT_SENT :
                                     WMS_TAG_MT_NOT_READ;      
          }
        }
        break;

        /* <stat> given */
        case 3:
          if ( !dsatsmsi_str_to_addr ( 
                                           &dsatetsismsi_send_addr,
                                           s,
                                           tok_ptr->arg[1],
                                           DSATSMS_TEXT_MODE
                                         ))
          {
            /* We got a bad address */
            result = DSAT_CMD_ERR_RSP;
          }
          /* find out what type of STAT we are about to store */
          else if((dsatetsismsi_send_stat=stat_lookup(tok_ptr->arg[2],0,NULL))
                    == WMS_TAG_NONE ) 
          {
            /* We got a bad <stat> */
            result = DSAT_CMD_ERR_RSP;
          }
          /* find out what type of TPDU we are about to store judging by the */
          /* <stat> parm to this command and verify that it agrees with the */
          /* TP-MTI bits extracted from +CSMP's <fo> parm */
          else if ( (dsatetsismsi_send_tpdu = 
                      tpdu_lookup(dsatetsismsi_send_stat)) == WMS_TPDU_MAX ||
                    (fo_mti == 1 && 
                      dsatetsismsi_send_tpdu != WMS_TPDU_SUBMIT) ||
                    (fo_mti != 1 &&
                      dsatetsismsi_send_tpdu != WMS_TPDU_DELIVER) ) 
          {
            /* We got a bad <STAT> */
            result = DSAT_CMD_ERR_RSP;
          }
          break;
                            
        default:
          /* We shouldn't be here    */
          /* Bad number of arguments */
          result = DSAT_CMD_ERR_RSP;
          break;
      }

      if ( result == DSAT_CMD_ERR_RSP )
      {
        /* We got an error in processing PDU */
        res_buff_ptr->used = 
          (byte) dsatsmsi_cms_error (res_buff_ptr,
                                    (int)DSAT_CMS_INVALID_TXT_PARAM );
        return result;
      }

    } /* if(text mode) */
    else
    {
      /* We are in PDU mode */
      /* convert the <length> to suitable a data structure */
      if(dsatutil_atoi(&dsatetsismsi_send_length,tok_ptr->arg[0],10)!=ATOI_OK)
      {
        /* We got a bad Index */
        result = DSAT_CMD_ERR_RSP;
      }

      if ( dsatetsismsi_send_length == 0 || 
           dsatetsismsi_send_length > MAX_PDU_LENGTH )
      {
        /* We got a bad length value */
        result = DSAT_CMD_ERR_RSP;
      }

      switch ( tok_ptr->args_found )
      {
        case 2:
          /* User specified STAT */
          if ( !VALID_TOKEN(1) )
          {
            /* stat is not given default it */
            dsatetsismsi_send_stat = WMS_TAG_MO_NOT_SENT;
          }
          else
          {
            dsatetsismsi_send_stat = stat_lookup(tok_ptr->arg[1], 1, NULL);
            if( !dsatetsismsi_send_stat )
            {
              /* Bad Tag */
              /* Return an error */
              result = DSAT_CMD_ERR_RSP;
            }
          }
          break;
        
        default:
          /* stat is not given default it */
          dsatetsismsi_send_stat = WMS_TAG_MO_NOT_SENT;
          break;
      }
      
      if ( result == DSAT_CMD_ERR_RSP )
      {
        /* We got an error in processing PDU */
        res_buff_ptr->used = 
          (byte) dsatsmsi_cms_error (res_buff_ptr, 
                                     (int)DSAT_CMS_INVALID_PDU_PARAM );
        return result;
      }
    }/* else(pdu mode) */

    /* Send mesasge to SIO data preprocessor to change state to */
    /* handle SMS message entry */
    dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
    dsatsmsi_pres_cmd.cmd_name = SMS_CMGW;
    result = DSAT_ASYNC_CMD;
    return result;
  }
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return a NULL string */
    res_buff_ptr->data_ptr[0]='\0';
    result = DSAT_OK;
    return result;
  }
  /*------------------------------------ 
   Processing command with no arguments:
   -------------------------------------*/
  if ( tok_ptr->op == (NA) )
  {
    /* This is valid only in text mode */
    if( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) )
    {
      /* We didn't get a <da> from user */
      /* Set the LOCAL structure to 0   */
      memset( (void *) &dsatetsismsi_send_addr, 0, sizeof(wms_address_s_type));

      /* stat is not given default it */
      dsatetsismsi_send_stat = WMS_TAG_MO_NOT_SENT;
      
      /* Default the type of TPDU to be sent */
      dsatetsismsi_send_tpdu = WMS_TPDU_SUBMIT;

      /* Send mesasge to SIO data preprocessor to change state to */
      /* handle SMS message entry */
      dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
      dsatsmsi_pres_cmd.cmd_name = SMS_CMGW;
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      result = DSAT_ERROR;
    }
    return result;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
    return result;
  }

}/* dsatetsisms_exec_cmgw_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMGC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT+CMGC command. 
  +CMGC is used to send command message to the network.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmgc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte s[32];
  dsat_num_item_type temp_arg;
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();

  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    /* text mode */
    if( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) ) 
    {
      /* Set the LOCAL structure to 0   */
      memset((void *)&dsatetsismsi_send_addr,0,sizeof(wms_address_s_type));

      /* convert the <da> to suitable a data structure */
      if( tok_ptr->args_found >= 5 )
      {
        if( VALID_TOKEN(4) )
        {
          /* Strip out the '"' from the <da> */
          if ( !dsatsmsi_strip_quotes( tok_ptr->arg[4], s, sizeof(s) ) )
          {
            result = DSAT_CMD_ERR_RSP;
          }
        }
        else
        {
          result = DSAT_CMD_ERR_RSP;
        }
      }

      if ( tok_ptr->args_found == 6 )
      {
        
        if ( !dsatsmsi_str_to_addr
             ( &dsatetsismsi_send_addr, s, tok_ptr->arg[1], DSATSMS_TEXT_MODE )
           )
        {
          /* Bad Address */
          result = DSAT_CMD_ERR_RSP;
        }
      }
      else if ( tok_ptr->args_found == 5 )
      {
        if(!dsatsmsi_str_to_addr 
           (&dsatetsismsi_send_addr,s,NULL,DSATSMS_TEXT_MODE)
          )
        {
          /* Bad Address */
          result = DSAT_CMD_ERR_RSP;
        }
      }
      else if ( tok_ptr->args_found == 4 )
      {
        if ( VALID_TOKEN(3) )
        {
          if (ATOI_OK == dsatutil_atoi(&temp_arg, tok_ptr->arg[3], 10))
          {
            dsatetsismsi_cmgc_mn = temp_arg;
          }
          else
          {
            result = DSAT_CMD_ERR_RSP;
          }
        }
        else
        {
          dsatetsismsi_cmgc_mn = 0;
        }
      }
      else if ( tok_ptr->args_found == 3 )
      {
        if ( VALID_TOKEN(2) )
        {
          if (ATOI_OK == dsatutil_atoi(&temp_arg, tok_ptr->arg[2], 10))
          {
            dsatetsismsi_cmgc_pid = temp_arg;
          }
          else
          {
            result = DSAT_CMD_ERR_RSP;
          }
        }
        else
        {
          dsatetsismsi_cmgc_pid = 0;
        }
      }
      else if ( tok_ptr->args_found == 2 )
      {
        if ( VALID_TOKEN(1) )
        {
          if (ATOI_OK == dsatutil_atoi(&temp_arg, tok_ptr->arg[1], 10))
          {
            dsatetsismsi_cmgc_ct = temp_arg;
          }
          else
          {
            result = DSAT_CMD_ERR_RSP;
          }
        }
        else
        {
          result = DSAT_CMD_ERR_RSP;
        }
      }
      else
      {
        result = DSAT_CMD_ERR_RSP;
      }
    }
    /* PDU mode */
    else
    {
      if ( ( tok_ptr->args_found > 1 )  || ( tok_ptr->args_found == 0 ))
      {
        result = DSAT_CMD_ERR_RSP;
      }
      /* convert the <length> to suitable a data structure */
      if(dsatutil_atoi(&dsatetsismsi_send_length,tok_ptr->arg[0],10)!=ATOI_OK)
      {
        /* We got a bad Index */
        result = DSAT_CMD_ERR_RSP;
      }
    }
    
    /* Check to see if there are any processing errors */
    if ( result != DSAT_OK )
    {
      dsati_cms_err_e_type  error;

      /* Reset the state variables */
      dsatetsismsi_cmgc_fo  = 0;
      dsatetsismsi_cmgc_ct  = 0;
      dsatetsismsi_cmgc_mn  = 0;
      dsatetsismsi_cmgc_pid = 0;

      if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) )
      {
        error = DSAT_CMS_INVALID_TXT_PARAM;
      }
      else
      {
        error = DSAT_CMS_INVALID_PDU_PARAM;
      }

      res_buff_ptr->used = 
        (byte)dsatsmsi_cms_error(res_buff_ptr, (int)error);
    }
    else
    {
      /* Send mesasge to SIO data preprocessor to */
      /* change state to handle SMS message entry */
      dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
      dsatsmsi_pres_cmd.cmd_name = SMS_CMGC;
      dsatetsismsi_send_stat = WMS_TAG_MO_NOT_SENT;
      result = DSAT_ASYNC_CMD;
    }
  }
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return a NULL string */
    res_buff_ptr->data_ptr[0]='\0';
    result = DSAT_OK;
  }
  /*-------------------------------
   Processing command with no args   
  ---------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
}/* dsatetsisms_exec_cmgc_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CNMA_CMD

DESCRIPTION
  This function executes AT+CNMA command. +CNMI confirms correct reception 
  of a new message ( SMS-DELIVER, SMS-STATUS-REPORT ) which is routed 
  directly to TE.

DEPENDENCIES
  This acknowledment command is used only if <service> of +CSMS command
  equals 1.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cnma_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type    result = DSAT_OK;
  dsat_num_item_type       n_val;
  byte                    *data_ptr;
  boolean                  ack_required = TRUE;
  ds3g_siolib_port_e_type  port = ds3g_siolib_get_active_port();
  sys_modem_as_id_e_type   subs_id = dsat_get_current_subs_id(FALSE);

  data_ptr = res_buff_ptr->data_ptr;
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CSMS_IDX,
                                    subs_id,0,NUM_TYPE) != 1 )
    {
      /* We are not allowed to send an ACK */
      /* remove the message for queue      */
      dsatetsismsi_free_element ( ack_required,subs_id);
      res_buff_ptr->used = 
        (byte)dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_UNKNOWN_ERR);

      return DSAT_CMD_ERR_RSP;
    }

    if ( !waiting_for_an_ack )
    {
      res_buff_ptr->used = 
        (byte)dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_CNMA_NOT_EXP);

      return DSAT_CMD_ERR_RSP;
    }

    /* text mode */
    if( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) ) 
    {
      /* not valid in text mode */
      result = DSAT_ERROR;
    }
    /* PDU mode */
    else
    {
      if ( VALID_TOKEN(0) )
      {
        if ( dsatutil_atoi( &n_val, tok_ptr->arg[0], 10 ) != ATOI_OK )
        {
          return DSAT_ERROR;
        }

        if ( n_val > 2 )
        {
          return DSAT_ERROR;
        }
      }
      else
      {
        return DSAT_ERROR;
      }

      if ( tok_ptr->args_found == 1 )
      {
        if ( n_val == 0 || n_val == 1 )
        {
          /* send RP-ACK */
          result = send_smstxt_ack(TRUE);
        }
        else
        {
          /* send RP-ERROR */
          result = send_smstxt_ack(FALSE);
        }

        dsatetsismsi_free_element ( ack_required,subs_id );
      }
      else if ( tok_ptr->args_found == 2 )
      {
        /* Send mesasge to SIO data preprocessor to change state to */
        /* handle SMS message entry */
        if(
        dsatutil_atoi(&dsatetsismsi_send_length,tok_ptr->arg[1],10) != ATOI_OK
          )
        {
          return DSAT_ERROR;
        }
        
        if ( dsatetsismsi_send_length == 0 )
        {
          return DSAT_ERROR;
        }
        else if ( n_val == 0 )
        {
          /* send RP-ACK */
          return  send_smstxt_ack(TRUE); 
        }
        else if ( n_val == 1 )
        {
          /* send RP-ACK */
          dsatetsismsi_ack_type = TRUE;
        }
        else
        {
          /* send RP-ERROR */
          dsatetsismsi_ack_type = FALSE;
        }

        dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
        dsatsmsi_pres_cmd.cmd_name = SMS_CNMA;
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        return DSAT_ERROR;
      }

    }
  }
  /*------------------------------------ 
   Processing command with no arguments:
   -------------------------------------*/
  else if ( tok_ptr->op == (NA) )
  {
    /* Send an SMS-DELIVER-REPORT */
    /* Send an RP-ACK             */
    if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CSMS_IDX,
                                     subs_id,0,NUM_TYPE) != 1 )
    {
      /* We are not allowed to send an ACK */
      /* remove the message for queue      */
      dsatetsismsi_free_element ( ack_required,subs_id );
      res_buff_ptr->used = 
        (byte)dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_UNKNOWN_ERR);

      return DSAT_CMD_ERR_RSP;
    }

    if ( !waiting_for_an_ack )
    {
      res_buff_ptr->used = 
        (byte)dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_CNMA_NOT_EXP);

      return DSAT_CMD_ERR_RSP;
    }
    
    result = send_smstxt_ack(TRUE);
    if(result == DSAT_ASYNC_CMD)
    {
      dsatsmsi_pres_cmd.cmd_name = SMS_CNMA;
    }

    dsatetsismsi_free_element ( ack_required,subs_id );
  }
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) )
    {
      /* do nothing, just return a NULL string */
      res_buff_ptr->data_ptr[0]='\0';
    }
    else
    {
      res_buff_ptr->used = (word)snprintf((char*)data_ptr,
                                                res_buff_ptr->size,
                                                "+CNMA: (0-2)");
      data_ptr += res_buff_ptr->used;      
    }
    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatetsisms_exec_cnma_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CMMS_CMD

DESCRIPTION
  This function executes AT+CMMS command. +CMMS controls the continuity of SMS
  relay protocol link. 

DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if +CMS ERROR is being generated
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cmms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  /*----------------------------  
   Processing WRITE  and name-only commands :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) ||
       tok_ptr->op == (NA) )
  {
    if ( tok_ptr->op == (NA|EQ|AR) )
    {
      /* do the usual parameter processing */
      result = dsatparm_exec_param_cmd (  mode,
                                          parse_table,
                                          tok_ptr,
                                          res_buff_ptr
                                        );
    }
    else
    {
      /* No value given, use default */
      DSATUTIL_SET_VAL(DSATETSI_EXT_ACT_CMMS_ETSI_IDX,
                        subs_id,0,0,dsat_cmms_dflm.default_v,NUM_TYPE)
    }

    if ( result == DSAT_OK )  
    {
      dsat_num_item_type temp_cmms;
      temp_cmms = (dsat_num_item_type)dsatutil_get_val(
                   DSATETSI_EXT_ACT_CMMS_ETSI_IDX,subs_id,0,NUM_TYPE);
#ifdef FEATURE_DUAL_SIM
      if ( WMS_OK_S == wms_cfg_ms_set_link_control( 
        dsatsmsi_client_id,
        subs_id,
        dsatsmsi_cmd_cb_func,
        NULL,
        (wms_cfg_link_control_mode_e_type)temp_cmms,
        CMMS_IDLE_TIMEOUT_VAL ) )
#else
      if ( WMS_OK_S == wms_cfg_set_link_control( 
        dsatsmsi_client_id,
        dsatsmsi_cmd_cb_func,
        NULL,
        (wms_cfg_link_control_mode_e_type)temp_cmms,
        CMMS_IDLE_TIMEOUT_VAL ) )
#endif /* FEATURE_DUAL_SIM */
      {
        dsatsmsi_pres_cmd.cmd_name = SMS_CMMS;
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        dsatme_set_cme_error(DSAT_CME_PHONE_FAILURE, res_buff_ptr);
        return DSAT_CMD_ERR_RSP;
      }
    }
    else
    {
      /* We got a bad argument */
      result = DSAT_ERROR;
    }
  }
  /*------------------------------------ 
   Processing READ and TEST commands :
   -------------------------------------*/
  else if ( tok_ptr->op == (NA|QU) || tok_ptr->op == (NA|EQ|QU) )
  {
    result = dsatparm_exec_param_cmd ( mode,
                                       parse_table,
                                       tok_ptr,
                                       res_buff_ptr
                                     );
  }
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatetsisms_exec_cmms_cmd */
/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSAS_CMD

DESCRIPTION
  This function executes AT+CSAS command. +CSAS saves the settings of +CSCA,
  +CSMP, +CSCB into NV. 

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ASYNC_CMD : if the command has been successfully executed
  DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_csas_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type   result = DSAT_ASYNC_CMD;
  dsat_string_item_type  *str_item;
  dsat_num_item_type      num_item = 0;
  sys_modem_as_id_e_type  subs_id = dsat_get_current_subs_id(FALSE);

/* -----------------------------------------------------------
                  Standard processing                         
-------------------------------------------------------------*/
  if ( (tok_ptr->op == (NA|EQ|AR)) ||
       (tok_ptr->op == (NA)) )
  {
    if ( DSAT_OK != dsatparm_exec_param_cmd ( mode,
                                       parse_table,
                                       tok_ptr,
                                       res_buff_ptr
                                    ))
    {
      return DSAT_ERROR;
    }
  }
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* Currently only profile 0 is supported */
    res_buff_ptr->used = (word)snprintf( (char *)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size ,
                                                "+CSAS: 0");
    return DSAT_OK;
  }
  else
  { 
    /* Read command is an  ERROR */
    return DSAT_ERROR;
  }
  /* Save settings of +CSCA, +CSMP, +CSCB into NV*/
  /* -----------------------------------------------------------
                   Save +CSCA settings 
  -------------------------------------------------------------*/
  memset ( (void*)&ds_nv_item, 0, sizeof(ds_nv_item));
  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,subs_id,
                                                         CSCA_SCA,MIX_STR_TYPE);
  (void) dsatutil_memscpy((void*)ds_nv_item.ds_at_csas_csca_settings.sca,
          NV_CSCA_SCA_STR_MAX_LEN,(void*)str_item,CSCA_SCA_STR_MAX_LEN +1);
  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,subs_id,
                                                       CSCA_TOSCA,MIX_STR_TYPE);
  (void) dsatutil_memscpy((void*)ds_nv_item.ds_at_csas_csca_settings.tosca,
          NV_CSCA_TOSCA_STR_MAX_LEN,(void*)str_item,CSCA_TOSCA_STR_MAX_LEN +1);
  if ( NV_DONE_S != dsatutil_put_nv_item(NV_DS_AT_CSAS_CSCA_SETTINGS_I, &ds_nv_item) )
  {
    return DSAT_ERROR;
  }
  /* -----------------------------------------------------------
                   Save +CSMP settings 
  -------------------------------------------------------------*/
  memset ( (void*)&ds_nv_item, 0, sizeof(ds_nv_item));

  str_item = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,CSMP_FO,MIX_STR_TYPE);

  (void) dsatutil_memscpy((void*)ds_nv_item.ds_at_csas_csmp_settings.fo,
          NV_CSMP_FO_STR_MAX_LEN,
         (void*)str_item,
           CSMP_FO_STR_MAX_LEN+1 );
    str_item = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,CSMP_VP,MIX_STR_TYPE);
    
  (void) dsatutil_memscpy((void*)ds_nv_item.ds_at_csas_csmp_settings.vp,
          NV_CSMP_VP_STR__MAX_LEN,
         (void*)str_item,
           CSMP_VP_STR_MAX_LEN+1 );
  
  num_item = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,CSMP_PID,MIX_NUM_TYPE);
  
  ds_nv_item.ds_at_csas_csmp_settings.pid = (uint8)num_item;

  num_item = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,CSMP_DCS,MIX_NUM_TYPE);
  
  ds_nv_item.ds_at_csas_csmp_settings.dcs = (uint8)num_item;
  
  if ( NV_DONE_S != dsatutil_put_nv_item(NV_DS_AT_CSAS_CSMP_SETTINGS_I, &ds_nv_item) )
  {
    return DSAT_ERROR;
  }
#ifdef FEATURE_ETSI_SMS_CB
/* -----------------------------------------------------------
                   Save +CSCB settings 
-------------------------------------------------------------*/
/* Read the current CSCB settings from WMS and update in the NV*/
  result = read_cscb_settings( SMS_CSAS_CSCB );

#endif /* FEATURE_ETSI_SMS_CB */
  return result;
}/* dsatetsisms_exec_csas_cmd */

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CRES_CMD

DESCRIPTION
  This function executes AT+CRES command. +CRES saves the settings of +CSCA,
  +CSMP, +CSCB from NV which got saved through +CSAS command. 

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ASYNC_CMD : if the command has been successfully executed
  DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cres_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type   result = DSAT_OK;
  uint8                   num_of_ids = 0;
  dsat_string_item_type  *str_item;
  sys_modem_as_id_e_type  subs_id = dsat_get_current_subs_id(FALSE);

/* -----------------------------------------------------------
                  Standard processing                         
-------------------------------------------------------------*/
  if ( (tok_ptr->op == (NA|EQ|AR)) ||
       (tok_ptr->op == (NA)) )
  {
    if ( DSAT_OK != dsatparm_exec_param_cmd ( mode,
                                       parse_table,
                                       tok_ptr,
                                       res_buff_ptr
                                    ))
    {
      return DSAT_ERROR;
    }
  }
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* Currently only profile 0 is supported */
    res_buff_ptr->used += (word)snprintf(
                      (char *)res_buff_ptr->data_ptr,
                      res_buff_ptr->size - res_buff_ptr->used,
                      "+CRES: 0");
    return DSAT_OK;
  }
  else
  {
    return DSAT_ERROR;
  }
  /* Restore settings of +CSCA, +CSMP, +CSCB from NV*/
  /* -----------------------------------------------------------
                   Restore +CSCA settings 
  -------------------------------------------------------------*/
  memset( (void*)&ds_nv_item, 0, sizeof(ds_nv_item));
  if ( dsatutil_get_nv_item_per_subs( NV_DS_AT_CSAS_CSCA_SETTINGS_I,
                                      &ds_nv_item,
                                      subs_id) != NV_DONE_S )
  {
    return DSAT_ERROR;
  }
  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,subs_id,CSCA_SCA,MIX_STR_TYPE);
  (void) dsatutil_memscpy((void*)str_item,CSCA_SCA_STR_MAX_LEN +1,
            (void*)ds_nv_item.ds_at_csas_csca_settings.sca,NV_CSCA_SCA_STR_MAX_LEN);
  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCA_IDX,subs_id,CSCA_TOSCA,MIX_STR_TYPE);
  (void) dsatutil_memscpy((void*)str_item,CSCA_TOSCA_STR_MAX_LEN +1,
            (void*)ds_nv_item.ds_at_csas_csca_settings.tosca, NV_CSCA_TOSCA_STR_MAX_LEN);
  /* -----------------------------------------------------------
                     Restore +CSMP settings 
  -------------------------------------------------------------*/
  memset ( (void*)&ds_nv_item, 0, sizeof(ds_nv_item));
  if ( dsatutil_get_nv_item_per_subs( NV_DS_AT_CSAS_CSMP_SETTINGS_I,
                                      &ds_nv_item,
                                      subs_id) != NV_DONE_S )
  {
    return DSAT_ERROR;
  }
  str_item = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,CSMP_FO,MIX_STR_TYPE);
  
  (void) dsatutil_memscpy((void*)str_item,
          CSMP_FO_STR_MAX_LEN+1,
          (void*)ds_nv_item.ds_at_csas_csmp_settings.fo,NV_CSMP_FO_STR_MAX_LEN);
  str_item = (byte *)dsatutil_get_val(
                            DSATETSI_EXT_CSMP_IDX,subs_id,CSMP_VP,MIX_STR_TYPE);
  
  (void) dsatutil_memscpy((void*)str_item,
          CSMP_VP_STR_MAX_LEN+1,
          (void*)ds_nv_item.ds_at_csas_csmp_settings.vp,NV_CSMP_VP_STR__MAX_LEN);

  DSATUTIL_SET_VAL(DSATETSI_EXT_CSMP_IDX,subs_id,CSMP_PID,0,ds_nv_item.ds_at_csas_csmp_settings.pid,MIX_NUM_TYPE)
  DSATUTIL_SET_VAL(DSATETSI_EXT_CSMP_IDX,subs_id,CSMP_DCS,0,ds_nv_item.ds_at_csas_csmp_settings.dcs,MIX_NUM_TYPE)

#ifdef FEATURE_ETSI_SMS_CB
  /* -----------------------------------------------------------
                     Restore +CSCB settings 
  -------------------------------------------------------------*/
  if(!init_mids())
  {
    return DSAT_ERROR;
  }
  /* Send it across to WMS*/
  dsatsmsi_pres_cmd.cmd_name = SMS_CRES_CSCB;

  /* Delete all entries at WMS before restoring the values from NV */
  result = process_cscb_cmd ( DSAT_CSCB_MODE_DEL_ALL, num_of_ids, dsat_get_cmd_mem_ptr());

#endif /* FEATURE_ETSI_SMS_CB */

  return result;
}/* dsatetsisms_exec_cres_cmd */

#ifdef FEATURE_ETSI_SMS_CB
/*===========================================================================

FUNCTION DSATETSISMS_EXEC_CSCB_CMD

DESCRIPTION
  This function executes AT+CSCB command. +CSCB command is used to select
  which type of Cell Broadcast Messages are to be received by TE.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Will govern the incoming CB message types
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_exec_cscb_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  uint8 num_of_ids=0;
  int loop;
  dsatetsismsi_dcs_val_type cscb_param_val[50];
  dsatetsisms_cscb_mode_e_type mode_val = DSAT_CSCB_MODE_NONE;
  dsat_error_info_s_type    err_info;
  wms_bc_mm_service_info_s_type *smsi_cb_mids = NULL;
  memset( (void*) cscb_param_val, 0, sizeof(dsatetsismsi_dcs_val_type) * 50);

  /*---------------------------------  
   Processing WRITE command :
  ----------------------------------*/
  if (tok_ptr->op == (NA|EQ|AR))
  {
    if(!init_mids())
    {
      return DSAT_ERROR;
    }
    
    smsi_cb_mids = dsat_get_cmd_mem_ptr();
    switch(tok_ptr->args_found)
    {  
      case 3 :
        if ( !parse_cscb_param ( tok_ptr->arg[2], cscb_param_val, &num_of_ids ) )
        {
          err_info.arg_num = 2;
          err_info.errval = DSAT_ERR_INVALID_TOKENS;
          goto send_error;
        }
      /*lint -fallthrough*/
      /* This fallthrough is intentional!*/
      case 2:
        if ( !parse_cscb_param ( tok_ptr->arg[1], cscb_param_val, &num_of_ids ) )
        {
          err_info.arg_num = 1;
          err_info.errval = DSAT_ERR_INVALID_TOKENS;
          goto send_error;
        }

        if(num_of_ids >= ARR_SIZE(cscb_param_val))
        {
          err_info.arg_num = 1;
          err_info.errval = DSAT_ERR_INVALID_OPERATION;
          goto send_error;
        }
        
        for ( loop=0; loop<num_of_ids; loop++ )
        {
          smsi_cb_mids[loop].srv_id.gw_cb_range.from = 
                                                    cscb_param_val[loop].from;
          smsi_cb_mids[loop].srv_id.gw_cb_range.to =  cscb_param_val[loop].to;
        }
        break;
      default:
        break;
    }

    /* Parsing was OK now save the parameters */
    result = dsatparm_exec_param_cmd (   mode,
                                         parse_table,
                                         tok_ptr,
                                         res_buff_ptr
                                      );
    if ( result != DSAT_ERROR )
    {
      if ( ( tok_ptr->args_found == 1 ) && (VALID_TOKEN(0) ) )
      {
        if( *tok_ptr->arg[0] == '0' )
        {
          /* mode 0 : We are accepting all service IDs */
          num_of_ids = 2;
          smsi_cb_mids[0].srv_id.gw_cb_range.from = 0;
          smsi_cb_mids[0].srv_id.gw_cb_range.to =  4369;

        /* WMS doesn’t allow client to delete/modify any cell broadcast 
           message id that is at presidential alert level which is 
           WMS_GW_CB_SRV_ID_CMAS_MSGID1 viz 4370 */

          smsi_cb_mids[1].srv_id.gw_cb_range.from = 4371;
          smsi_cb_mids[1].srv_id.gw_cb_range.to =  65535;

          mode_val = DSAT_CSCB_MODE_ADD_ALL;
        }
        else if ( *tok_ptr->arg[0] == '1' )
        {
          mode_val = DSAT_CSCB_MODE_DEL_ALL;
        }
        else
        {
          DSAT_SMS_STATE_CLEAN();
          err_info.arg_num = 0;
          err_info.errval = DSAT_ERR_INVALID_TOKENS;
          goto send_error;
        }
      }
      else if( VALID_TOKEN(0) && (*tok_ptr->arg[0] == '0') )
      {
       /* mode 0: Accept the service ids*/
        mode_val = DSAT_CSCB_MODE_ADD;
      }
      else if ( VALID_TOKEN(0) && (*tok_ptr->arg[0] == '1') )
      {
        /* mode 1: Reject the service ids*/
        mode_val = DSAT_CSCB_MODE_DEL;
      }
      else
      {
        /* Invalid Parameter */
        DSAT_SMS_STATE_CLEAN();
        err_info.arg_num = 0;
        err_info.errval = DSAT_ERR_INVALID_OPERATION;
        goto send_error;
      }
      /* Send it across to WMS*/
      dsatsmsi_pres_cmd.cmd_name = SMS_CSCB;
      result = process_cscb_cmd ( mode_val, num_of_ids, smsi_cb_mids);

    }/* result != ERROR */
  }   
  /*---------------------------------  
   Processing READ command
  ----------------------------------*/
  else if ( tok_ptr->op == (NA|QU) )
  {
    result = read_cscb_settings( SMS_CSCB);
  }
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "+CSCB: (0-1)");
    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;

send_error:
  DS_AT_MSG2_ERROR("err_info: %d, arg_num: %d", err_info.errval,err_info.arg_num);
  return DSAT_ERROR;

} /* dsatetsisms_exec_cscb_cmd */

#endif /* FEATURE_ETSI_SMS_CB */

/*===========================================================================

FUNCTION DSATETSISMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It is used in both text mode as well as PDU mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatetsisms_send_sms_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
)
{
  dsati_cms_err_e_type result;

  if ( (dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,0,0,NUM_TYPE) ) /*we are in text mode, so send text mesage*/
  {
    result = send_smstxt_msg ( msg, len );
  }
  else /*we are in PDU mode, so send a PDU message*/
  {    
    result = send_smspdu_msg ( msg, len );
  }
  
  if( result != DSAT_CMS_NONE )
  {
    /* We got an error while processing message   */
    /* Send an ASYNC Event to tell ATCOP about it */
    dsatsms_abort_cmd ( result );
  }

  return;
}/* dsatetsisms_send_sms_msg */

/*===========================================================================

FUNCTION DSATETSISMS_PROCESS_TEMPLATE_READ

DESCRIPTION
  This function processes the template read handling when invoked during the middle
  of AT commands - CSMP or CSCA. For both these commands which involve updating the 
  WMS template in the SIM card, a Read of the same is needed. This way ATCoP will only
  update those fields expected to be updated in the template keeping the others untouched.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatetsisms_process_template_read 
(
 wms_gw_template_s_type *read_templ_ptr
)
{
  wms_status_e_type      msg_status = WMS_OK_S;
  wms_write_mode_e_type  write_mode = WMS_WRITE_MODE_INSERT;
  dsat_result_enum_type  result = DSAT_ASYNC_EVENT;

#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
#endif /* FEATURE_DUAL_SIM */

  if(read_templ_ptr != NULL && template_ptr != NULL)
  {    
    DS_AT_MSG2_HIGH("dsatetsisms_process_template_read %d read mask, %d write mask",
           read_templ_ptr->mask,template_ptr->u.gw_template.mask);

     /* Template already present. Use the existing one */
    write_mode = WMS_WRITE_MODE_REPLACE;

    if( (!(template_ptr->u.gw_template.mask & WMS_GW_TEMPLATE_MASK_DEST_ADDR)) &&
        (read_templ_ptr->mask & WMS_GW_TEMPLATE_MASK_DEST_ADDR) )
    {
      template_ptr->u.gw_template.mask = 
          template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_DEST_ADDR);
    
  	(void) dsatutil_memscpy((void*)&template_ptr->u.gw_template.dest_addr,sizeof(wms_address_s_type),
  			  (void*)&read_templ_ptr->dest_addr,sizeof(wms_address_s_type));
  	
    }
    if( (!(template_ptr->u.gw_template.mask & WMS_GW_TEMPLATE_MASK_VALIDITY)) && 
        (read_templ_ptr->mask & WMS_GW_TEMPLATE_MASK_VALIDITY) )
    {
      template_ptr->u.gw_template.mask = 
          template_ptr->u.gw_template.mask | (WMS_GW_TEMPLATE_MASK_VALIDITY);

    (void) dsatutil_memscpy((void*)&template_ptr->u.gw_template.relative_validity,sizeof(wms_timestamp_s_type),
  			(void*)&read_templ_ptr->relative_validity,sizeof(wms_timestamp_s_type));

    }
    (void) dsatutil_memscpy((void*)&template_ptr->u.gw_template.alpha_id,sizeof(wms_alpha_id_s_type),
  		  (void*)&read_templ_ptr->alpha_id,sizeof(wms_alpha_id_s_type));
  }

#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_write_template( 
                     dsatsmsi_client_id,
                     subs_id,
                     dsatsmsi_cmd_cb_func,
                     NULL, 
                     write_mode,
                     template_ptr
                     );
#else
    msg_status = wms_msg_write_template( 
                                    dsatsmsi_client_id,
                                    dsatsmsi_cmd_cb_func,
                                    NULL, 
                                    write_mode,
                                    template_ptr
                                    );
#endif /* FEATURE_DUAL_SIM */

  if(msg_status == WMS_OK_S)
  {
    result = DSAT_ASYNC_EVENT;
  }
  else
  {
    DSAT_SMS_STATE_CLEAN();
    /*Free the allocated memory for template_ptr*/
    dsatutil_free_memory_ext((void **)&template_ptr);
    result = DSAT_ERROR;
  }

  return result;
}/* dsatetsisms_process_template_read */

/* LOCAL FUNCTION DEFINITIONS */
/*===========================================================================

FUNCTION SEND_SMSPDU_MSG

DESCRIPTION
  Local function that is used to send PDU mesasge to Service Center.

DEPENDENCIES
  None

RETURN VALUE
  Returns a enum (dsati_cms_err_e_type) indicating the +CMS error(if any).

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsati_cms_err_e_type send_smspdu_msg
(
  byte  *pdu_ptr, /* Ptr to the pdu message that has to be sent */
  int   len       /* Len of the txt message that has to be sent */
)
{
  dsati_cms_err_e_type result;

  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CNMA )
  {
    /* We are executing +CNMA command, send an ACK */
    result = send_smspdu_ack ( pdu_ptr, len );
  }
  else
  {
    /* We are trying to send or write a PDU */
    result = send_pdu_msg ( pdu_ptr, len );
  }

  return result;
} /* send_smspdu_msg */

/*===========================================================================

FUNCTION SEND_PDU_MSG

DESCRIPTION
  Local function that is used to send PDU mesasge to Service Center.

DEPENDENCIES
  None

RETURN VALUE
  Returns a enum (dsati_cms_err_e_type) indicating the +CMS error(if any).

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsati_cms_err_e_type send_pdu_msg
(
  byte  *pdu_ptr, /* Ptr to the pdu message that has to be sent */
  int   len       /* Len of the txt message that has to be sent */
)
{
  /* data structures used in wms_msg_send */
  #define ARRAY_LEN (32)
  wms_send_mode_e_type send_mode;
  wms_status_e_type msg_status;
  wms_write_mode_e_type write_mode;
  uint8 loop = 0;
  byte * temp_pdu_ptr;
  byte * temp_ptr;
  byte s[ARRAY_LEN], c, oct[3], type_of_addr[3];
  dsat_num_item_type sc_len, fo;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  dsat_err_e_type   err_info = DSAT_ERR_NO_ERROR;
  dsati_cms_err_e_type  cms_err = DSAT_CMS_NONE;

  wms_client_message_s_type     *sms_msg_ptr = NULL;
  
  sms_msg_ptr = (wms_client_message_s_type *) dsat_alloc_memory(sizeof(wms_client_message_s_type),
                                                                    FALSE);
  temp_pdu_ptr = pdu_ptr;

  if ( ( len < ( (int)dsatetsismsi_send_length * 2 ) ) || 
       ( len > MAX_PDU_LENGTH ) )
  {
    /* we got a PDU with length less than or more than expected */
    err_info = DSAT_ERR_INVALID_ARGUMENT;
    cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
    goto send_error;
  }

  if( (dsatsmsi_pres_cmd.cmd_name ==SMS_CMGS) || 
      (dsatsmsi_pres_cmd.cmd_name==SMS_CMGC) )
  {
    sms_msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
  }
  else /* SMS_CMGW */
  {
    sms_msg_ptr->msg_hdr.mem_store = 
      dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
        DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,1,NUM_TYPE), CPMS_MEM2,ETSI_CMD_MODE);
  }

  send_mode = WMS_SEND_MODE_CLIENT_MESSAGE;
  write_mode = WMS_WRITE_MODE_INSERT;

  /* Check to see if SCA is given with the PDU */
  if( len != ((int)dsatetsismsi_send_length * 2) )
  {
    /* SCA is given with the PDU */
    /* Get the length of the SCA */
    oct[0] = *temp_pdu_ptr++;
    oct[1] = *temp_pdu_ptr++;
    oct[2] = '\0';
  
    /* Convert the SCA to an int */
    if( dsatutil_atoi(&sc_len, oct, 16) != ATOI_OK)
    {
      err_info = DSAT_ERR_ATOI_CONVERSION_FAILURE;
      cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
      goto send_error;
    }

    if( sc_len  == 0 )
    {
      /* We got a 0 for SMSC length octet            */
      /* SC address is not present. Use CSCA command */
      if (!dsatetsismsi_get_sca
          (&sms_msg_ptr->u.gw_message.sc_address, SMS_TEXT_MODE)
         )
      {
        /* Get the length of the <fo> */
        temp_ptr = temp_pdu_ptr;
        oct[0] = *temp_ptr++;
        oct[1] = *temp_ptr++;
        oct[2] = '\0';
  
        /* Convert the <fo> to an int */
        if( dsatutil_atoi(&fo, oct, 16) != ATOI_OK)
        {
          err_info = DSAT_ERR_ATOI_CONVERSION_FAILURE;
          cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
          goto send_error;
        }
  
        if( dsatetsismsi_get_tpdu_type ( dsatetsismsi_send_stat, (uint8)fo )
            != WMS_TPDU_DELIVER )
        {
          /* It is OK to write DELIVER without SCA else ABORT */
          err_info = DSAT_ERR_INVALID_OPERATION;
          cms_err  = DSAT_CMS_SCA_ADDR_UNKNOWN;
          goto send_error;
        }
      }
      sms_msg_ptr->u.gw_message.raw_ts_data.len = (uint32)(len - 2)/2;
    }
    else
    {
      /* We got SC address along with the TPDU */
      
      /* This length gives the number of useful octets in the SCA */
      /* convert it into a length of semi-octets                  */
      sc_len = ( sc_len - 1 ) * 2;

      /* Check to see if the PDU is formatted correctly */

      if( ( sc_len >= (ARRAY_LEN - 1) ) || 
          ( len < (int)sc_len ) || 
          ( len - ((int)sc_len+4 )) != ((int)dsatetsismsi_send_length * 2) )
      {
        err_info = DSAT_ERR_INVALID_ARGUMENT;
        cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
        goto send_error;
      }

      /* Strip it out of TPDU                  */
      type_of_addr[0] = *temp_pdu_ptr++;
      type_of_addr[1] = *temp_pdu_ptr++;
      type_of_addr[2] = '\0';

      loop=0;
      for( loop=0; loop < sc_len ; loop+=2 )
      {
        c = (byte) UPCASE (*temp_pdu_ptr);
        /* check to see if the last byte is 'F' */
        /* Refer to 3G TS 23.040 section 9.1.2.3 for more details */
        if( (loop == (sc_len-2)) && c == 'F')
        {
          /* Neglect this semi-octect */
          /* Get the next one         */
          temp_pdu_ptr++;
          s[loop] = *temp_pdu_ptr;
          temp_pdu_ptr++;
          s[loop+1] = '\0';
        }
        else
        {
          s[loop+1] = *temp_pdu_ptr++;
          s[loop]   = *temp_pdu_ptr++;
        }
      }

      s[loop] = '\0';

      if ( !dsatsmsi_str_to_addr
           (&sms_msg_ptr->u.gw_message.sc_address,s,type_of_addr,DSATSMS_PDU_MODE)
          )
      {
        err_info = DSAT_ERR_INVALID_OPERATION;
        cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
        goto send_error;
      }
      sms_msg_ptr->u.gw_message.raw_ts_data.len = 
        ((uint32)len - ((uint32)sc_len + 4))/2;
    }/* if( sc_len  != 0 ) */
  }/* if( len != (dsatetsismsi_send_length * 2) ) */
  else
  {
    /* We did not get the SCA from the PDU     */
    /* Get the SCA from previous +CSCA command */
    if (!dsatetsismsi_get_sca
        (&sms_msg_ptr->u.gw_message.sc_address, SMS_TEXT_MODE)
       )
    {
      /* Get the length of the <fo> */
      temp_ptr = temp_pdu_ptr;
      oct[0] = *temp_ptr++;
      oct[1] = *temp_ptr++;
      oct[2] = '\0';
  
      /* Convert the <fo> to an int */
      if( dsatutil_atoi(&fo, oct, 16) != ATOI_OK)
      {
        /* We got a wrong <fo> value */
        err_info = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
        goto send_error;
      }
  
      if( dsatetsismsi_get_tpdu_type ( dsatetsismsi_send_stat, (uint8)fo )
          != WMS_TPDU_DELIVER )
      {
        /* It is OK to write DELIVER without SCA else ABORT */
        err_info = DSAT_ERR_INVALID_OPERATION;
        cms_err  = DSAT_CMS_SCA_ADDR_UNKNOWN;
        goto send_error;
      }

    }

    sms_msg_ptr->u.gw_message.raw_ts_data.len = (uint32)len/2;
  }

  if( 
    dsatetsismsi_encode_pdu
    ( 
     temp_pdu_ptr, 
     sms_msg_ptr->u.gw_message.raw_ts_data.data, 
     (int)sms_msg_ptr->u.gw_message.raw_ts_data.len 
    ) 
    == DSAT_ERROR 
    )
  {
    /* We got an error while encoding */
    /* Inform ATCOP about it          */
    err_info = DSAT_ERR_INVALID_OPERATION;
    cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
    goto send_error;
  }

  /* fill out rest of the fields */
  /*No need for an index */
  sms_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
  sms_msg_ptr->u.gw_message.is_broadcast = FALSE;
  sms_msg_ptr->msg_hdr.tag = dsatetsismsi_send_stat;
  sms_msg_ptr->u.gw_message.raw_ts_data.format = WMS_FORMAT_GW_PP;
  
  /* See what type of TPDU we are sending */
  sms_msg_ptr->u.gw_message.raw_ts_data.tpdu_type = 
    dsatetsismsi_get_tpdu_type
    (dsatetsismsi_send_stat, sms_msg_ptr->u.gw_message.raw_ts_data.data[0]);
  
  if ( sms_msg_ptr->u.gw_message.raw_ts_data.tpdu_type == WMS_TPDU_MAX )
  {
    /* We got a unsupported TPDU */
    err_info = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
    cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
    goto send_error;

  }

  if ( (sms_msg_ptr->u.gw_message.raw_ts_data.tpdu_type==WMS_TPDU_STATUS_REPORT)
       && ( sms_msg_ptr->msg_hdr.mem_store != WMS_MEMORY_STORE_SIM ) )
  {
    /* Status reports can only be written into SIM */
    err_info = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
    cms_err  = DSAT_CMS_INVALID_PDU_PARAM;
    goto send_error;
  }

  /* finally we are done stuffing all the required */
  /* parameters send the message to WMS */
  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGW )
  {
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_write(
                                 dsatsmsi_client_id,
                                 subs_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 write_mode,
                                 sms_msg_ptr
                              );
#else
    msg_status = wms_msg_write(
                                 dsatsmsi_client_id,
                                 dsatsmsi_cmd_cb_func,
                                 (void*)&dsatsmsi_pres_cmd,
                                 write_mode,
                                 sms_msg_ptr
                              );
#endif /* FEATURE_DUAL_SIM */
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_send(
                              dsatsmsi_client_id,
                              subs_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              send_mode,
                              sms_msg_ptr
                             );
#else
    msg_status = wms_msg_send(
                              dsatsmsi_client_id,
                              dsatsmsi_cmd_cb_func,
                              (void*)&dsatsmsi_pres_cmd,
                              send_mode,
                              sms_msg_ptr
                             );
#endif /* FEATURE_DUAL_SIM */
  }

  /* Clean up the state variables */
  dsatetsismsi_send_stat = WMS_TAG_NONE;
  dsatetsismsi_send_length = 0;


  if( msg_status != WMS_OK_S )
  {
    err_info = DSAT_ERR_INVALID_OPERATION;
    cms_err  = DSAT_CMS_ME_FAILURE;
    goto send_error;

  }
  /*Free the allocated memory for sms_msg_ptr*/
  dsatutil_free_memory_ext((void **)&sms_msg_ptr);
  return DSAT_CMS_NONE;

send_error:
  DS_AT_MSG2_ERROR ("send_smspdu_msg() Err_info: %d, CMS_err:" ,
                    err_info,cms_err);

  /*Free the allocated memory for sms_msg_ptr*/
  dsatutil_free_memory_ext((void **)&sms_msg_ptr);
  return cms_err;

}/* send_smspdu_msg */

/*===========================================================================

FUNCTION SEND_SMSTXT_MSG

DESCRIPTION
  Local function that is used to send TEXT mesasge to Service Center.

DEPENDENCIES
  None

RETURN VALUE
  Returns a enum (dsati_cms_err_e_type) indicating the +CMS error(if any).

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsati_cms_err_e_type send_smstxt_msg
(
  byte  *txt_ptr, /* Ptr to the txt message that has to be sent */
  int   len       /* Len of the txt message that has to be sent */
)
{
  /* data structures used in wms_msg_send */
  wms_send_mode_e_type send_mode;
  wms_status_e_type msg_status = WMS_OK_S;
  wms_write_mode_e_type write_mode;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  dsati_cms_err_e_type  err_info = DSAT_CMS_NONE;

  wms_client_ts_data_s_type     *txt_msg_ptr = NULL;
  wms_client_message_s_type     *sms_msg_ptr = NULL;

  sms_msg_ptr = (wms_client_message_s_type *)
                           dsat_alloc_memory(sizeof(wms_client_message_s_type),
                                             FALSE);

  txt_msg_ptr = (wms_client_ts_data_s_type *)
                           dsat_alloc_memory(sizeof(wms_client_message_s_type),
                                             FALSE);

  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGS )
  {
    txt_msg_ptr->u.gw_pp.tpdu_type = WMS_TPDU_SUBMIT;

    if( !dsatetsismsi_encode_data 
        ( &txt_msg_ptr->u.gw_pp.u.submit.user_data, txt_ptr, len )
      )
    {
       err_info = DSAT_CMS_INVALID_TXT_PARAM;
       goto send_result;
    }

    sms_msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
  }
  else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGW )
  {
    txt_msg_ptr->u.gw_pp.tpdu_type = dsatetsismsi_send_tpdu;
    sms_msg_ptr->msg_hdr.mem_store = 
      dsatsmsi_memory_lookup((dsat_num_item_type)dsatutil_get_val(
       DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,1,NUM_TYPE), CPMS_MEM2,ETSI_CMD_MODE);
    
    if(dsatetsismsi_send_tpdu == WMS_TPDU_SUBMIT)
    { 
      if( !dsatetsismsi_encode_data 
          (&txt_msg_ptr->u.gw_pp.u.submit.user_data, txt_ptr, len)
        )
      {
         err_info = DSAT_CMS_INVALID_TXT_PARAM;
         goto send_result;
      }
    }
    else /* WMS_TPDU_DELIVER */
    {
      if( !dsatetsismsi_encode_data 
          (&txt_msg_ptr->u.gw_pp.u.deliver.user_data, txt_ptr, len)
        )
      {
         err_info = DSAT_CMS_INVALID_TXT_PARAM;
         goto send_result;
      }

      /* Attempting to write an SMS-DELIVER in text mode,
         We must invent a SCTS value to appease WMS,
         Use start of CDMA time in BCD. */
      txt_msg_ptr->u.gw_pp.u.deliver.timestamp.year = 128;
      txt_msg_ptr->u.gw_pp.u.deliver.timestamp.month = 1;
      txt_msg_ptr->u.gw_pp.u.deliver.timestamp.day = 4;
      txt_msg_ptr->u.gw_pp.u.deliver.timestamp.hour = 00;
      txt_msg_ptr->u.gw_pp.u.deliver.timestamp.minute = 00;
      txt_msg_ptr->u.gw_pp.u.deliver.timestamp.second = 00;
      txt_msg_ptr->u.gw_pp.u.deliver.timestamp.timezone = 00;
    }
  }
  else if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGC )
  {
    if ( (len < 2) || (len%2) )
    {
       err_info = DSAT_CMS_INVALID_TXT_PARAM;
       goto send_result;
    }

    txt_msg_ptr->u.gw_pp.tpdu_type = WMS_TPDU_COMMAND;
    sms_msg_ptr->msg_hdr.mem_store = WMS_MEMORY_STORE_NONE;
    
    txt_msg_ptr->u.gw_pp.u.command.command_data_len = (uint8)len/2;
    if( dsatetsismsi_encode_pdu
        ( txt_ptr, txt_msg_ptr->u.gw_pp.u.command.command_data, len/2 ) 
        == DSAT_ERROR 
      )
    {
      /* We got an error       */
      /* Inform ATCOP about it */
       err_info = DSAT_CMS_INVALID_TXT_PARAM;
       goto send_result;
    }
  }

  /* Fill in the text mode parameters */
  dsatetsismsi_fill_pdu_fields( &txt_msg_ptr->u.gw_pp );

  send_mode = WMS_SEND_MODE_CLIENT_MESSAGE;
  write_mode = WMS_WRITE_MODE_INSERT; 

  /* First encode the PDU message into a TPDU */
  txt_msg_ptr->format = WMS_FORMAT_GW_PP;
  
  msg_status = wms_ts_encode (
                               txt_msg_ptr,
                               &sms_msg_ptr->u.gw_message.raw_ts_data
                              );
  
  if( msg_status != WMS_OK_S )
  {
     err_info = DSAT_CMS_INVALID_TXT_PARAM;
     goto send_result;
  }
  
  /* fill out rest of the enums */
  sms_msg_ptr->msg_hdr.message_mode = WMS_MESSAGE_MODE_GW;
  /* Get the TAG type from previously entered value */
  sms_msg_ptr->msg_hdr.tag = dsatetsismsi_send_stat;
  
  /* get the service center address from previous +CSCA command */
  if (( !dsatetsismsi_get_sca
        (&sms_msg_ptr->u.gw_message.sc_address, SMS_TEXT_MODE) ) &&
      ( txt_msg_ptr->u.gw_pp.tpdu_type != WMS_TPDU_DELIVER )
     )
  {
    /* Service center is not given. Abort */
    err_info = DSAT_CMS_SCA_ADDR_UNKNOWN;
    goto send_result;
  }

  /* finally we are done stuffing all the required */
  /* parameters send the message to WMS */
  if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGW )
  {
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_write(
                               dsatsmsi_client_id,
                               subs_id,
                               dsatsmsi_cmd_cb_func,
                               (void*)&dsatsmsi_pres_cmd,
                               write_mode,
                               sms_msg_ptr
                               );
#else
    msg_status = wms_msg_write(
                               dsatsmsi_client_id,
                               dsatsmsi_cmd_cb_func,
                               (void*)&dsatsmsi_pres_cmd,
                               write_mode,
                               sms_msg_ptr
                               );
#endif /* FEATURE_DUAL_SIM */
  }
  else
  {
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_send(
                               dsatsmsi_client_id,
                               subs_id,
                               dsatsmsi_cmd_cb_func,
                               (void*)&dsatsmsi_pres_cmd,
                               send_mode,
                               sms_msg_ptr
                              );
#else
    msg_status = wms_msg_send(
                               dsatsmsi_client_id,
                               dsatsmsi_cmd_cb_func,
                               (void*)&dsatsmsi_pres_cmd,
                               send_mode,
                               sms_msg_ptr
                              );
#endif /* FEATURE_DUAL_SIM */
  }

  /* Clean up the state variables */
  dsatetsismsi_send_stat = WMS_TAG_NONE;
  dsatetsismsi_send_tpdu = WMS_TPDU_MAX;

  if( msg_status != WMS_OK_S )
  {
    err_info = DSAT_CMS_ME_FAILURE;
    goto send_result;
  }
  /*else the there is no error so we will de-allocate the buffers and return 
    DSAT_CMS_NONE*/

send_result:

  dsatutil_free_memory_ext((void **)&txt_msg_ptr);
  dsatutil_free_memory_ext((void **)&sms_msg_ptr);


  DS_AT_MSG1_HIGH("send_smstxt_msg() err_info: %d" , err_info);
  return err_info;

}/* send_smstxt_msg */

/*===========================================================================

FUNCTION SEND_SMSPDU_ACK

DESCRIPTION
  Local function that is used to send SMS-DELIVER-REPORT in PDU mode.
  This function handles both RP-ACK and RP-ERROR

DEPENDENCIES
  None

RETURN VALUE
  Returns a enum (dsati_cms_err_e_type) indicating the +CMS error(if any).

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsati_cms_err_e_type send_smspdu_ack
(
  byte  *pdu_ptr, /* Ptr to the pdu message that has to be sent */
  int   len       /* Len of the txt message that has to be sent */
)
{
  wms_status_e_type msg_status;
  wms_ack_info_s_type *ack_info_ptr = NULL;
  wms_gw_user_data_s_type * data_ptr;
  dsat_num_item_type pdu_vals[2] = {0,0};
  dsat_num_item_type first_octet=0, second_octet=0, third_octet=0, sm_len=0;
  byte s[3];
  byte opt_param[3];
  int i, j;
  boolean ack_required = TRUE;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  /* Check to see if SCA is given with the PDU */
  if ( (( len - 2 ) != ((int)dsatetsismsi_send_length * 2)) || ( len > MAX_PDU_LENGTH ) )
  {
    /* we got a PDU with length less than or more than expected */
    return DSAT_CMS_INVALID_PDU_PARAM;
  }

  if ( *pdu_ptr != '"' || *(pdu_ptr+(len-1)) != '"' )
  {
    return DSAT_CMS_INVALID_PDU_PARAM;
  }

  pdu_ptr++;
  
  for ( i = 0; i < 2; i++ )
  {
    s[i] = *pdu_ptr++;
  }
  s[2] = '\0';

  if ( dsatutil_atoi( &first_octet, s, 16 ) != ATOI_OK )
  {
    return DSAT_CMS_INVALID_PDU_PARAM;
  }

  for ( i = 0; i < 2; i++ )
  {
    s[i] = *pdu_ptr++;
  }
  s[2] = '\0';

  if ( dsatutil_atoi( &second_octet, s, 16 ) != ATOI_OK )
  {
    return DSAT_CMS_INVALID_PDU_PARAM;
  }
  else
  {
    opt_param[0] = second_octet & 0x01;
    opt_param[1] = second_octet & 0x02;
    opt_param[2] = second_octet & 0x04;
  }

  if ( !dsatetsismsi_ack_type )
  {
    for ( i = 0; i < 2; i++ )
    {
      s[i] = *pdu_ptr++;
    }
    s[2] = '\0';

    if ( dsatutil_atoi( &third_octet, s, 16 ) != ATOI_OK )
    {
      return DSAT_CMS_INVALID_PDU_PARAM;
    }
    else
    {
      opt_param[0] = third_octet & 0x01;
      opt_param[1] = third_octet & 0x02;
      opt_param[2] = third_octet & 0x04;
    }
  }

  for ( i = 0; i < 2; i++ )
  {
    if ( opt_param[i] )
    {
      for( j = 0; j < 2; j++ )
      {
        s[j] = *pdu_ptr++;
      }/* for j */
      s[2] = '\0';
    
      if ( dsatutil_atoi ( &pdu_vals[i], s, 16 ) != ATOI_OK )
      {
        return DSAT_CMS_INVALID_PDU_PARAM;
      }
    }
  }/* for i */

  if ( opt_param[2] )
  {
    for( j = 0; j < 2; j++ )
    {
      s[j] = *pdu_ptr++;
    }/* for j */
    s[2] = '\0';
  
    if ( dsatutil_atoi ( &sm_len, s, 16 ) != ATOI_OK )
    {
      return DSAT_CMS_INVALID_PDU_PARAM;
    }
  }
  else
  {
    sm_len = 0;
  }
  ack_info_ptr = (wms_ack_info_s_type *)dsat_alloc_memory(sizeof(wms_ack_info_s_type),FALSE);
  
  ack_info_ptr->message_mode = WMS_MESSAGE_MODE_GW;
  ack_info_ptr->transaction_id = act_tid;

  if ( dsatetsismsi_ack_type )
  {
    /* We are sending an RP-ACK */
    ack_info_ptr->u.gw.success = TRUE;
    ack_info_ptr->u.gw.u.ack.user_data_header_present = ((first_octet & 0x64)>>6);
    ack_info_ptr->u.gw.u.ack.mask = second_octet;
    ack_info_ptr->u.gw.u.ack.pid = (wms_pid_e_type)pdu_vals[0];
    dsatetsismsi_decode_sms_dcs
      (&ack_info_ptr->u.gw.u.ack.dcs, pdu_vals[1]);
    data_ptr = &ack_info_ptr->u.gw.u.ack.user_data;
  }
  else
  {
    /* We are sending an RP-ERROR */
    ack_info_ptr->u.gw.success = FALSE;
    ack_info_ptr->u.gw.u.error.user_data_header_present = ((first_octet & 0x64)>>6);
    ack_info_ptr->u.gw.u.error.tp_cause = (wms_tp_cause_e_type)second_octet;
    ack_info_ptr->u.gw.u.error.mask = third_octet;
    ack_info_ptr->u.gw.u.error.pid = (wms_pid_e_type)pdu_vals[0];
    dsatetsismsi_decode_sms_dcs
      (&ack_info_ptr->u.gw.u.error.dcs, pdu_vals[1]);
  }

  data_ptr = &ack_info_ptr->u.gw.u.error.user_data;
  data_ptr->sm_len = (uint16)sm_len;

  if(
      dsatetsismsi_encode_pdu
        (pdu_ptr,data_ptr->sm_data,(int)data_ptr->sm_len)
      == DSAT_ERROR 
    )
  {
    /* We got an error while encoding */
    /* Inform ATCOP about it          */
    dsatutil_free_memory(ack_info_ptr);
    return DSAT_CMS_INVALID_PDU_PARAM;
  }

  /* finally we are done stuffing all the required */
  /* parameters send the message to WMS */
#ifdef FEATURE_DUAL_SIM
  msg_status = wms_msg_ms_ack(
                           dsatsmsi_client_id,
                           subs_id,
                           dsatsmsi_cmd_cb_func,
                           (void*)&dsatsmsi_pres_cmd,
                           ack_info_ptr
                          );
#else
  msg_status = wms_msg_ack(
                           dsatsmsi_client_id,
                           dsatsmsi_cmd_cb_func,
                           (void*)&dsatsmsi_pres_cmd,
                           ack_info_ptr
                          );
#endif /* FEATURE_DUAL_SIM */
  
  /* Clean up the state variables */
  act_tid = 0;
  dsatetsismsi_send_stat = WMS_TAG_NONE;
  dsatetsismsi_send_length = 0;
  dsatetsismsi_free_element ( ack_required,subs_id );
  dsatutil_free_memory(ack_info_ptr);
  
  if( msg_status == WMS_OK_S )
  {
    return DSAT_CMS_NONE;
  }
  else
  {
    return DSAT_CMS_ME_FAILURE;
  }
}/* send_smspdu_ack */

/*===========================================================================

FUNCTION SEND_SMSTXT_ACK

DESCRIPTION
  Local function that is used to send SMS-DELIVER-REPORT in TEXT mode.
  This function handles both RP-ACK and RP-ERROR

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type send_smstxt_ack
(
 boolean ack_type /* RP-ACK or RP-ERROR */
)
{
  wms_status_e_type msg_status;
  wms_ack_info_s_type *ack_info_ptr;
  dsat_num_item_type temp_val =0;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  ack_info_ptr = (wms_ack_info_s_type *)dsat_alloc_memory(sizeof(wms_ack_info_s_type),FALSE);

  ack_info_ptr->message_mode = WMS_MESSAGE_MODE_GW;
  ack_info_ptr->transaction_id = act_tid;

  if ( ack_type )
  {
    ack_info_ptr->u.gw.success = TRUE;
    ack_info_ptr->u.gw.u.ack.user_data_header_present = FALSE;
    ack_info_ptr->u.gw.u.ack.mask = 3;
    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,2,MIX_NUM_TYPE);
    ack_info_ptr->u.gw.u.ack.pid = (wms_pid_e_type)temp_val;
    
    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,3,MIX_NUM_TYPE);
    dsatetsismsi_decode_sms_dcs
      (&ack_info_ptr->u.gw.u.ack.dcs, temp_val);
    ack_info_ptr->u.gw.u.ack.user_data.sm_len = 0;
  }
  else
  {
    ack_info_ptr->u.gw.success = FALSE;
    ack_info_ptr->u.gw.u.error.user_data_header_present = FALSE;
    ack_info_ptr->u.gw.u.error.tp_cause = WMS_TP_CAUSE_UNSPECIFIED_ERROR;
    ack_info_ptr->u.gw.u.error.mask = 3;
    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,2,MIX_NUM_TYPE);
    ack_info_ptr->u.gw.u.error.pid = (wms_pid_e_type)temp_val;
        
    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSATETSI_EXT_CSMP_IDX,subs_id,3,MIX_NUM_TYPE);
    dsatetsismsi_decode_sms_dcs
      (&ack_info_ptr->u.gw.u.error.dcs,temp_val);
    ack_info_ptr->u.gw.u.error.user_data.sm_len = 0;
  }

  /* finally we are done stuffing all the required */
  /* parameters send the message to WMS */
#ifdef FEATURE_DUAL_SIM
  msg_status = wms_msg_ms_ack(
                           dsatsmsi_client_id,
                           subs_id,
                           dsatsmsi_cmd_cb_func,
                           (void*)&dsatsmsi_pres_cmd,
                           ack_info_ptr
                          );
#else
  msg_status = wms_msg_ack(
                           dsatsmsi_client_id,
                           dsatsmsi_cmd_cb_func,
                           (void*)&dsatsmsi_pres_cmd,
                           ack_info_ptr
                          );
 #endif /* FEATURE_DUAL_SIM */
  
  /* Clean up the state variables */
  dsatetsismsi_send_stat = WMS_TAG_NONE;
  dsatutil_free_memory(ack_info_ptr);
  
  if( msg_status == WMS_OK_S )
  {
    return DSAT_ASYNC_CMD;
  }
  else
  {
    return DSAT_ERROR;
  }

}/* send_smstxt_ack */

/*===========================================================================

FUNCTION    STAT_LOOKUP

DESCRIPTION
  Converts a string parameter to a TAG type ( wms_message_tage_e_type ).
  It also chacks to see if the "ALL" wild card is being used as a stat.
  If so then it sets the pointer to the flag

DEPENDENCIES
  None

RETURN VALUE
  Returns an wms_message_tage_e_type enum.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL wms_message_tag_e_type stat_lookup
( 
 byte * stat_val, /* pointer to the stat value */
 boolean type,    /* PDU or text mode          */
 int  * all_flag  /* Ptr for "ALL",4 FLAG      */
)
{
  wms_message_tag_e_type tag;
  byte tag_read[] = "\"REC READ\"";
  byte tag_unread[] = "\"REC UNREAD\"";
  byte tag_sent[] = "\"STO SENT\"";
  byte tag_unsent[] = "\"STO UNSENT\"";
  byte tag_all[] = "\"ALL\"";
  byte s[32];

  if(type)
  {
    switch(*stat_val)
    {
    case '0':
      tag = WMS_TAG_MT_NOT_READ;
      break;
    case '1':
      tag = WMS_TAG_MT_READ;
      break;
    case '2':
      tag = WMS_TAG_MO_NOT_SENT;
      break;
    case '3':
      tag = WMS_TAG_MO_SENT;
      break;
    case '4':
      /* Valid only in CMGL */
      if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
      {
        tag = WMS_TAG_MT_READ;
        if ( all_flag != NULL )
        {
          *all_flag = 0;
        }
      }
      else
      {
        tag = WMS_TAG_NONE;
      }
      break;
    default:
      tag = WMS_TAG_NONE;
      break;
    }
  }
  else
  {
    /* Check to see if the stat is wrapped in quotes */
    if( !dsatsmsi_strip_quotes ( stat_val, s, sizeof(s) ) )
    {
      return WMS_TAG_NONE;
    }

    if ( *s == '\0' )
    {
      tag = WMS_TAG_MO_NOT_SENT;
    }
    else if(!dsatutil_strcmp_ig_sp_case(stat_val,tag_unread))
    {
      tag = WMS_TAG_MT_NOT_READ;
    }
    else if (!dsatutil_strcmp_ig_sp_case(stat_val,tag_read))
    {
      tag = WMS_TAG_MT_READ;
    }
    else if (!dsatutil_strcmp_ig_sp_case(stat_val,tag_unsent))
    {
      tag = WMS_TAG_MO_NOT_SENT;
    }
    else if (!dsatutil_strcmp_ig_sp_case(stat_val,tag_sent))
    {
      tag = WMS_TAG_MO_SENT;
    }
    else if (!dsatutil_strcmp_ig_sp_case(stat_val,tag_all))
    {
      /* Valid only in CMGL */
      if ( dsatsmsi_pres_cmd.cmd_name == SMS_CMGL )
      {
        tag = WMS_TAG_MT_READ;
        if ( all_flag != NULL )
        {
          *all_flag = 0;
        }
      }
      else
      {
        tag = WMS_TAG_NONE;
      }
    }
    else
    {
      tag = WMS_TAG_NONE;
    }
  }

  return tag;
}/* stat_lookup */

/*===========================================================================

FUNCTION    TPDU_LOOKUP

DESCRIPTION
  Converts a string parameter to a TPDU type ( wms_gw_tpdu_type_e_type ).
  Used by send commands in text mode to convert the <stat> given by the user
  to a TPDU type that has to be sent.

DEPENDENCIES
  None

RETURN VALUE
  Returns an wms_gw_tpdu_type_e_type enum.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL wms_gw_tpdu_type_e_type tpdu_lookup
( 
 wms_message_tag_e_type stat /* stat type enum */
)
{
  wms_gw_tpdu_type_e_type tpdu;

  if ( stat == WMS_TAG_MO_NOT_SENT || stat == WMS_TAG_MO_SENT )
  {
    tpdu = WMS_TPDU_SUBMIT;
  }
  else if ( stat == WMS_TAG_MT_NOT_READ || stat == WMS_TAG_MT_READ )
  {
    tpdu = WMS_TPDU_DELIVER;
  }
  else
  {
    tpdu = WMS_TPDU_MAX;
  }

  return tpdu;
}/* tpdu_lookup */

#ifdef FEATURE_ETSI_SMS_CB
/*===========================================================================

FUNCTION    PARSE_CSCB_PARAM

DESCRIPTION
  Parses the arguments of +CSCB command. Utility will parse strings of 
  type "0,1,5,320-478,922" and creates a list of numbers.

DEPENDENCIES
  None

RETURN VALUE
  Returns a boolean to indicate if the parsing went right.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean parse_cscb_param 
( 
 byte  * param,                          /* ptr to the unparsed str */
 dsatetsismsi_dcs_val_type * dcs,        /* ptr to the place holder */
 uint8 *  num_of_param                   /* ptr to the count        */
)
{
  int i=0, loop=0, param_count=0;
  byte temp[6], stripped[50];
  boolean range = FALSE;
  dsat_num_item_type value;

  *num_of_param = 0;

  if ( !dsatsmsi_strip_quotes ( param, stripped, sizeof(stripped) ) )
  {
    return FALSE;
  }

  while ( stripped[loop] != '\0' )
  {
    i=0;
    while ( ( stripped[loop] != ',' ) && ( stripped[loop] != '-' ) && 
            ( stripped[loop] != '\0' ) )
    {
      temp[i++] = stripped[loop++];
      if ( i >= 6 )
      {
        return FALSE;
      }
    }
  
    temp[i] = '\0';

    if ( range )
    {
      if(ATOI_OK != dsatutil_atoi(&value,temp,10))
      {
        return FALSE;
      }

      if ( value > 65535 )
      {
        return FALSE;
      }
      if(param_count > 0)
      {
        dcs[param_count - 1].to =  (uint16)value;
      }
      else
      {
        return FALSE;
      }
      range = FALSE;
    }
    else
    {
      if ( ATOI_OK != dsatutil_atoi( &value, temp, 10 ) )
      {
        return FALSE;
      }
      dcs[param_count].from =  (uint16)value;
      dcs[param_count].to =  (uint16)value;
      (*num_of_param)++;
      param_count++;
    }

    if ( stripped[loop] == '-' )
    {
      range = TRUE;
    }
    else
    {
      range = FALSE;
    }
    
    loop++;

    if ( param_count >= 50 )
    {
      return FALSE;
    }
    else if ( stripped[loop-1] == '\0' )
    {
      return TRUE;
    }
  }

  return TRUE;
} /* parse_cscb_param */
/*===========================================================================

FUNCTION    PROCESS_CSCB_CMD

DESCRIPTION
  +CSCB mids value is updated to WMS based on the mode chosen. 
DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: If successfully sent to WMS
  DSAT_ERROR: If failed to send it to WMS

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type process_cscb_cmd
(
  dsatetsisms_cscb_mode_e_type mode,
  uint8 num_of_ids,
  const wms_bc_mm_service_info_s_type *sms_cb_mids
)
{
  wms_status_e_type status = WMS_OK_S;
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  dsat_string_item_type * str_item;
  DS_AT_MSG2_HIGH("Processing CSCB cmd mode = %d num_of_ids = %d",
                                              (int)mode, num_of_ids);

  switch( mode )
  {
    case DSAT_CSCB_MODE_ADD_ALL:
    {
#ifdef FEATURE_DUAL_SIM
      status = wms_bc_ms_add_services ( 
                                        dsatsmsi_client_id, 
                                        dsat_get_current_subs_id(FALSE),
                                        dsatsmsi_cmd_cb_func, 
                                        (void*)&dsatsmsi_pres_cmd, 
                                        WMS_MESSAGE_MODE_GW, 
                                        num_of_ids,
                                        sms_cb_mids
                                      );
#else
      status = wms_bc_mm_add_services ( 
                                        dsatsmsi_client_id, 
                                        dsatsmsi_cmd_cb_func, 
                                        (void*)&dsatsmsi_pres_cmd, 
                                        WMS_MESSAGE_MODE_GW, 
                                        num_of_ids,
                                        sms_cb_mids
                                      );
#endif /* FEATURE_DUAL_SIM */
      break;
    }
    case DSAT_CSCB_MODE_DEL_ALL:
    {
#ifdef FEATURE_DUAL_SIM
      status = wms_bc_ms_delete_all_services(
                                           dsatsmsi_client_id,
                                           dsat_get_current_subs_id(FALSE),
                                           dsatsmsi_cmd_cb_func,
                                           (void*)&dsatsmsi_pres_cmd,
                                            WMS_MESSAGE_MODE_GW
                                           );
#else
      status = wms_bc_mm_delete_all_services(
                                           dsatsmsi_client_id,
                                           dsatsmsi_cmd_cb_func,
                                           (void*)&dsatsmsi_pres_cmd,
                                            WMS_MESSAGE_MODE_GW
                                           );
#endif /* FEATURE_DUAL_SIM */
      str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,2,MIX_STR_TYPE);
      str_item[0] = '\0';
      break;
    }
    case DSAT_CSCB_MODE_ADD:
    {
#ifdef FEATURE_DUAL_SIM
      status = wms_bc_ms_add_services ( 
                                        dsatsmsi_client_id, 
                                        dsat_get_current_subs_id(FALSE),
                                        dsatsmsi_cmd_cb_func, 
                                        (void*)&dsatsmsi_pres_cmd, 
                                        WMS_MESSAGE_MODE_GW, 
                                        num_of_ids,
                                        sms_cb_mids
                                      );
#else
      status = wms_bc_mm_add_services ( 
                                        dsatsmsi_client_id, 
                                        dsatsmsi_cmd_cb_func, 
                                        (void*)&dsatsmsi_pres_cmd, 
                                        WMS_MESSAGE_MODE_GW, 
                                        num_of_ids,
                                        sms_cb_mids
                                      );
#endif /* FEATURE_DUAL_SIM */
      break;
    }
    case DSAT_CSCB_MODE_DEL:
    {
      wms_bc_mm_srv_id_type cscb_del_mid_val[50];
      uint8 loop = 0;
      uint8 num_of_mids = 0;

      memset( (void *)cscb_del_mid_val, 0, sizeof(cscb_del_mid_val) );
      num_of_mids = MIN( num_of_ids, ARR_SIZE(cscb_del_mid_val) );

      for ( loop=0; loop<num_of_mids; loop++ )
      {
        cscb_del_mid_val[loop].gw_cb_range.from = sms_cb_mids[loop].srv_id.gw_cb_range.from;
        cscb_del_mid_val[loop].gw_cb_range.to =  sms_cb_mids[loop].srv_id.gw_cb_range.to;
      }
#ifdef FEATURE_DUAL_SIM
      status = wms_bc_ms_delete_services (
                                          dsatsmsi_client_id,
                                          dsat_get_current_subs_id(FALSE),
                                          dsatsmsi_cmd_cb_func,
                                          (void*)&dsatsmsi_pres_cmd,
                                          WMS_MESSAGE_MODE_GW,
                                          num_of_mids,
                                          cscb_del_mid_val
                                        );
#else
      status = wms_bc_mm_delete_services (
                                          dsatsmsi_client_id,
                                          dsatsmsi_cmd_cb_func,
                                          (void*)&dsatsmsi_pres_cmd,
                                          WMS_MESSAGE_MODE_GW,
                                          num_of_mids,
                                          cscb_del_mid_val
                                        );
#endif /* FEATURE_DUAL_SIM */
       break;
    }
    default:
    {
      DSAT_SMS_STATE_CLEAN();
      result = DSAT_ERROR;
    }
  }

  if ( status != WMS_OK_S )
  {
    DSAT_SMS_STATE_CLEAN();
    result = DSAT_ERROR;
  }

  return result;

}/* process_cscb_cmd*/
/*===========================================================================

FUNCTION    READ_CSCB_SETTINGS
  Read the current CSCB settings stored at WMS
DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: If successfully sent to WMS
  DSAT_ERROR: If failed to send it to WMS

SIDE EFFECTS
  None
===========================================================================*/
LOCAL dsat_result_enum_type read_cscb_settings
(
  sms_cmd_name_e_type cmd_type
)
{
  dsatsmsi_pres_cmd.cmd_name = cmd_type;
#ifdef FEATURE_DUAL_SIM
  if ( WMS_OK_S !=  wms_bc_ms_get_all_service_ids (
                                             dsatsmsi_client_id,
                                             dsat_get_current_subs_id(FALSE),
                                             dsatsmsi_cmd_cb_func,
                                             (void*)&dsatsmsi_pres_cmd,
                                             WMS_MESSAGE_MODE_GW
                                     ))
#else
  if ( WMS_OK_S !=  wms_bc_mm_get_all_service_ids (
                                             dsatsmsi_client_id,
                                             dsatsmsi_cmd_cb_func,
                                             (void*)&dsatsmsi_pres_cmd,
                                             WMS_MESSAGE_MODE_GW
                                     ))
#endif /* FEATURE_DUAL_SIM */
   {
      DSAT_SMS_STATE_CLEAN();
     return DSAT_ERROR;
   }
   else
   {
     return DSAT_ASYNC_CMD;
   }
}

/*===========================================================================

FUNCTION  DSATETSISMS_PROCESS_CRES

DESCRIPTION
  This function processes the CRES command. In particular, after deleting the 
  entries of CSCB at WMS this function adds the service id's retrieved from 
  NV to WMS. 

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD : if the command has been successfully sent to WMS. 
  DSAT_ERROR: if NV read failed. 

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsisms_process_cres
(
  void
)
{
  dsatetsismsi_dcs_val_type cscb_param_val[CSCB_MIDS_STR_MAX_LEN];
  char temp_cscb_mids[CSCB_MIDS_STR_MAX_LEN + 3]; /* Providing space for quotes */
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  uint8 num_of_ids = 0;
  uint8 loop = 0;
  dsat_string_item_type * str_item;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  wms_bc_mm_service_info_s_type *smsi_cb_mids = dsat_get_cmd_mem_ptr();

  memset ( (void*)&ds_nv_item, 0, sizeof(ds_nv_item));
  memset ( (void *) temp_cscb_mids, 0 , ( CSCB_MIDS_STR_MAX_LEN + 1 ));
  
  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,CSCB_MIDS,MIX_STR_TYPE);
  memset ( (void *)str_item, 0x0, CSCB_MIDS_STR_MAX_LEN);

  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,CSCB_DCSS,MIX_STR_TYPE);
  memset ( (void *) str_item, 0x0, CSCB_DCSS_STR_MAX_LEN);
  
  if ( dsatutil_get_nv_item_per_subs( NV_DS_AT_CSAS_CSCB_SETTINGS_I,
                                      &ds_nv_item,
                                      subs_id) != NV_DONE_S )
  {
    return DSAT_ERROR;
  }
  DSATUTIL_SET_VAL(DSATETSI_EXT_CSCB_IDX,0,CSCB_MODE,0,
                     ds_nv_item.ds_at_csas_cscb_settings.mode,MIX_NUM_TYPE)
  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,CSCB_MIDS,MIX_STR_TYPE);
  (void) dsatutil_memscpy((void*)str_item,
          CSCB_MIDS_STR_MAX_LEN + 1,
          (void*)ds_nv_item.ds_at_csas_cscb_settings.mids,NV_CSCB_MIDS_STR_MAX_LEN);

  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,CSCB_DCSS,MIX_STR_TYPE);
  (void) dsatutil_memscpy((void*)str_item,
          CSCB_DCSS_STR_MAX_LEN+1,
          (void*)ds_nv_item.ds_at_csas_cscb_settings.dcss,NV_CSCB_DCSS_STR_MAX_LEN);

  memset( (void*) cscb_param_val, 0, sizeof(dsatetsismsi_dcs_val_type) * 50);
  /* MIDS values is expected to be in double quotes */
  str_item = (uint8 *)dsatutil_get_val(DSATETSI_EXT_CSCB_IDX,0,CSCB_MIDS,MIX_STR_TYPE);
  (void)snprintf( (char *)temp_cscb_mids ,
                  (CSCB_MIDS_STR_MAX_LEN + 3),
                  "\"%s\"",
                  (char *)str_item);
  /* convert the range values into from and to fields */
  (void)parse_cscb_param( (byte *)temp_cscb_mids,
                    cscb_param_val,
                    &num_of_ids );
  /* Return early OK if the num of ids is 0 (empty list)*/
  if ( num_of_ids == 0)
  {
    DS_AT_MSG0_HIGH("Retreived an empty list. return OK");
    result = DSAT_OK;
    goto send_result;
  }
  /* Even if there are too many mids, exit gracefully. */
  else if (num_of_ids >= MAX_SMSI_MIDS)
  {
    DS_AT_MSG1_MED("No of MIDS:%d is greater than MAX_MIDS:50",num_of_ids);
    num_of_ids = MAX_SMSI_MIDS;
  }

  /*Check if memory is allocated for smsi_cb_mids by either +CRES/CSCB*/
  if(smsi_cb_mids == NULL)
  {
    result = DSAT_ERROR;
    goto send_result;
  }

  /* Copy the values into a WMS expected struct type */
  for ( loop=0; loop<num_of_ids; loop++ )
  {
    smsi_cb_mids[loop].srv_id.gw_cb_range.from = 
                                              cscb_param_val[loop].from;
    smsi_cb_mids[loop].srv_id.gw_cb_range.to =  cscb_param_val[loop].to;
  }
  dsatsmsi_pres_cmd.cmd_name = SMS_CRES_CSCB;
  result = process_cscb_cmd ( DSAT_CSCB_MODE_ADD, 
                              num_of_ids, 
                              smsi_cb_mids );
send_result:
  return result;
}
/*===========================================================================

FUNCTION    INIT_MIDS

DESCRIPTION
  Initializes the wms_bc_mm_service_info_s_type data structure.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean init_mids()
{
  int loop = 0;
  wms_bc_mm_service_info_s_type *smsi_cb_mids = NULL;
  /*Allocate mids if mids is not allocated*/

  smsi_cb_mids = dsat_alloc_cmd_mem_ptr(sizeof(wms_bc_mm_service_info_s_type) * MAX_SMSI_MIDS);
  for (loop = 0; loop<MAX_SMSI_MIDS; loop++ )
  {
    smsi_cb_mids[loop].priority = WMS_PRIORITY_NORMAL;
    smsi_cb_mids[loop].label_encoding = WMS_ENCODING_ASCII;
    smsi_cb_mids[loop].alert = WMS_BC_ALERT_DEFAULT;
    smsi_cb_mids[loop].selected =  TRUE;
    smsi_cb_mids[loop].max_messages = 10;
    (void)snprintf(smsi_cb_mids[loop].label,
                         WMS_BC_MM_SRV_LABEL_SIZE,
                         "AT Command Interface");
  }
  return TRUE;
}/* init_mids */
#endif /* FEATURE_ETSI_SMS_CB */
#endif /* FEATURE_ETSI_SMS */
