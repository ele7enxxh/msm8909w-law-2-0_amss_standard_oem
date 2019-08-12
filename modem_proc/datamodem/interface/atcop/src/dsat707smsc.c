/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific sms related
  AT commands.

  The cmd is not specified in 707 spec.  We (qcom) add them to automate sms
  testing.

EXTERNALIZED FUNCTIONS

  dsat707sms_exec_qcsmp_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCSMP command.
    $QCSMP is used to set parameters for sending text sms messages.

  dsat707sms_exec_qcpms_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCPMS command.
    $QCPMS is used to select preferred memory storage for reading, 
    writing etc.

  dsat707sms_exec_qcnmi_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCNMI command.
    $QCNMI is used to select the procedure of how receiving new messages
    from the network is indicated to TE.

  dsat707sms_exec_qcmgs_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCMGS command.
    $QCMGS is used to send a message from TE to the network.

  dsat707sms_exec_qcmgr_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$CMGR command.
    $QCMGR is used to read a sms message.

  dsat707sms_exec_qcmgl_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCMGL command. 
    $QCMGL command lists all the SMS saved in the loaction.

  dsat707sms_exec_qcmss_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCMSS command.
    $QCMSS is used to send a message already stored from Memory to the network.

  dsat707sms_exec_qcmgd_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT$QCMGD command.
    $QCMGD is used to delete sms messages from <mem1>.

  dsat707sms_exec_qcmgw_cmd
    This function takes the result from the command line parser
    and executes it. This function executes AT$QCMGW command. 
    $QCMGW is used to store message to memory storage <mem2>

  dsat707sms_send_sms_msg
    Central place/function call for sending sms message.
    In case of actual message transmission, it is done in 2 phases. First you 
    get the actual command with all the parameters. Parameters are stored and 
    user is sent a prompt to indicate that message has to be typed. Then the 
    message is transmited. This function is used to transmit the actual message.

  is707_send_smstxt_msg
    Local function that is used to send TEXT message.

Copyright (c) 2002 - 2012,2014-2015 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707smsc.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/15/15   skc     Fixed KW Issues
03/11/14   tk      Optimized debug macros usage in ATCoP.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   nc      Fixed operating mode checks for SMS related commands for NikeL targets. 
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
01/19/12   sk      Feature cleanup.
09/27/11   ht      Included dsut header to support offtarget stubs.
01/31/11   nc      Fixed a crash due to incorrect Featurisation.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
05/10/10   kk      Added support for ^HC SMS commands. 
12/15/09   nc      Mainlining FEATURE_DATA_SERIALIZER.
06/28/09   vg      Fixed Lint errors.
01/27/09   nc      Fixed crashes in QCMGR and QCMGS. Also adding check
                   from user setting "SM" for a RUIM build in NV only mode. 
12/12/08   ua      Compilation fix in multimode targets. 
11/13/08   nc      Added Support for QCMGW,QCMSS,QCMGL Commands
06/01/07   sa      Mainlining SMS C2K changes
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include <stringl/stringl.h>

#ifdef FEATURE_CDMA_SMS

#include <stdlib.h>

#include "dsati.h"
#include "dsatsmsi.h"
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#include "dsatparm.h"
#include "dsatcmif.h"
#include "err.h"
#include "wms.h"

#include <stringl/stringl.h>

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for variables that are used
  in ATCOP SMS SUB SYSTEM
===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#define DEBUG( x )

/*===========================================================================
            Data types
===========================================================================*/

/* import variable from other files inside sms module */

/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type      dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type       dsatsmsi_client_id;

/* Variable used in CMGL SM */
extern sms_cmd_sm_type          dsatsmsi_cmd_sm;

/* Variables needed for QCPMS */
/* remembers the mem being reported */
extern cpms_mem_status_data_s_type dsatsmsi_mem_report;

/* actual data item     */
extern nv_item_type         ds_nv_item;


/* Variable to retain destination address for send commands (text mode) */         
wms_address_s_type                send_addr;

wms_message_tag_e_type  dsat707smsi_send_stat = WMS_TAG_NONE;
wms_address_s_type   dsat707smsi_send_addr;

#ifdef FEATURE_DSAT_EXTENDED_SMS
boolean cmgw_has_timestamp = FALSE;
wms_user_data_encoding_e_type dsat707smsi_send_encoding;
dsat_num_item_type            dsat707smsi_send_type;
wms_timestamp_s_type          dsat707smsi_send_mc_time;
wms_priority_e_type           dsat707smsi_send_priority;
wms_language_e_type           dsat707smsi_send_language;
dsat_num_item_type            hcmgr_mode;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/* LOCAL variables */
/* Note: Following variables are kept    */
/* Global to avoid SP corruption problem */
LOCAL wms_client_message_s_type         *is707_client_msg = NULL;

/* LOCAL Function Delarations */

LOCAL boolean is707_send_smstxt_msg
(
  byte  *txt_ptr, /* Ptr to the txt message to be sent */
  int   len       /* Len of the txt message to be sent */
);
LOCAL wms_message_tag_e_type stat707_lookup
( 
 byte * stat_val, /* pointer to the stat value */
 int  * all_flag  /* Ptr for "ALL",4 FLAG      */
);

#ifdef FEATURE_DSAT_EXTENDED_SMS
LOCAL wms_message_tag_e_type stat707_lookup_ext
( 
 byte * stat_val, /* pointer to the stat value */
 int  * all_flag  /* Ptr for "ALL",4 FLAG      */
);
LOCAL wms_user_data_encoding_e_type convert_encoding_ext
( 
  dsat_num_item_type encod_arg
);
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/* FUNCTION DEFINITIONS */
/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCSMP_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSMP command.
  $QCSMP is used to set parameters for sending text sms messages.

  DEPENDENCIES
  None

  RETURN VALUE
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcsmp_cmd
(
  dsat_mode_enum_type mode,           /* AT command mode:            */
  const dsati_cmd_type *tab_entry,    /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,  /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr         /* Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*-------------------------------------------------------------------------*/

  /*---------------------------------------
    Processing WRITE and READ command :
  -----------------------------------------*/
  if( tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA|QU) )
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      int arg_index;

      /* Try to validate the format of the valid period and 
       *  Deferred Delivery Time
       */
      for (arg_index = 1; arg_index < 4; arg_index +=2 )
      {
        /* iterate the loop for arg_index = 1 (vpf), 3 (ddtf) */
        if(tok_ptr->args_found >= (unsigned int) (arg_index + 2))
        {
          int fmt = atoi((char *)tok_ptr->arg[arg_index]);

          if(fmt == 0)
          {
            /* Absolute format */
            if ( !dsat707smsi_string_to_timestamp(
                    (char *)tok_ptr->arg[arg_index + 1], NULL) )
            {
              result = DSAT_ERROR;
            }
          }
          else if(fmt == 1)
          {
            /* Relative format */
            dsat_num_item_type rela_val;
          
            if ( ATOI_OK != 
                 dsatutil_atoi(&rela_val, tok_ptr->arg[arg_index + 1], 10) )
            {
              result = DSAT_ERROR;
            }
          
            if( rela_val >248 )
            {
              result = DSAT_ERROR;
            }
          }
          else
          {
            result = DSAT_ERROR;
          }
        }
      } /* for loop */
    } /* (tok_ptr->op == (NA|EQ|AR)) */

    if(result == DSAT_OK)
    {
      /* do the usual parameter processing */
      result = dsatparm_exec_param_cmd (  mode,
                                          tab_entry,
                                          tok_ptr,
                                          res_buff_ptr
                                          );
    }
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
} /* dsat707sms_exec_qcsmp_cmd */


/*===========================================================================

FUNCTION DSAT707SMS_EXEC_QCPMS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPMS command.
  $QCPMS is used to select preferred memory storage for reading, 
  writing etc.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:.
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD: An async call has made to wms.

SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcpms_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define SM_MEM 2

  dsat_result_enum_type result = DSAT_OK;
  nv_stat_enum_type     status = NV_DONE_S;
  dsat_num_item_type mem1_type;   /* save the old mem types */
  dsat_num_item_type mem2_type;
  dsat_num_item_type mem3_type;
  /* get memory status for mem1, mem2, and mem3 */
  wms_status_e_type mem_status;
  wms_memory_store_e_type mem_store;
  boolean temp_hc_cmd = FALSE;

  dsatsmsi_mem_report.mem3_changed = FALSE;
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( 0 == dsatutil_strcmp_ig_sp_case((const byte *) tok_ptr->name,(const byte *)"^HCPMS"))
  {
    temp_hc_cmd = TRUE;
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */ 

/* Read NV_RTRE_CONFIG_I to determine RUIM-Config */
  status = dsatutil_get_nv_item( NV_RTRE_CONFIG_I, &ds_nv_item );
  if( status != NV_DONE_S )
  {
    /* Setting to NV only*/
    ds_nv_item.rtre_config = NV_RTRE_CONFIG_NV_ONLY;
  }

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
        DS_AT_MSG0_ERROR("QCPMS: mem1 is needed");
#ifdef FEATURE_DSAT_EXTENDED_SMS
        res_buff_ptr->used = 
          dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
        return DSAT_CMD_ERR_RSP;
#else
        return DSAT_ERROR;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      }
      mem3_type = (dsat_num_item_type)dsatutil_get_val(
        DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM3,NUM_TYPE);
      mem2_type = (dsat_num_item_type)dsatutil_get_val(
        DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM2,NUM_TYPE);
      mem1_type = (dsat_num_item_type)dsatutil_get_val(
        DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM1,NUM_TYPE);
#ifdef FEATURE_DSAT_EXTENDED_SMS
      dsatsmsi_mem_report.prev_mem[CPMS_MEM1] = mem1_type;
      dsatsmsi_mem_report.prev_mem[CPMS_MEM2] = mem2_type;
      dsatsmsi_mem_report.prev_mem[CPMS_MEM3] = mem3_type;
      dsatsmsi_mem_report.used_tag_slots[CPMS_MEM1] = 0;
      dsatsmsi_mem_report.used_tag_slots[CPMS_MEM2] = 0;
      dsatsmsi_mem_report.used_tag_slots[CPMS_MEM3] = 0;
      dsatsmsi_mem_report.max_slots[CPMS_MEM1] = 0;
      dsatsmsi_mem_report.max_slots[CPMS_MEM2] = 0;
      dsatsmsi_mem_report.max_slots[CPMS_MEM3] = 0;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

      result = dsatparm_exec_param_cmd ( 
                                        mode,
                                        parse_table,
                                        tok_ptr,
                                        res_buff_ptr
                                        );
      if(result != DSAT_OK)
      {
        DS_AT_MSG0_HIGH("Erroneous input params in $QCPMS");
#ifdef FEATURE_DSAT_EXTENDED_SMS
       res_buff_ptr->used = 
         dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
       return DSAT_CMD_ERR_RSP;
#else
        return DSAT_ERROR;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      }

      /* 
         Check if any of qcpms_val is set to "SM" when not allowed 
         ( eg: NV only mode in RUIM build )
      */
      if( (ds_nv_item.rtre_config == NV_RTRE_CONFIG_NV_ONLY) && 
          (((int)dsatutil_get_val(
          DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM3,NUM_TYPE) == SM_MEM)||
           ((int)dsatutil_get_val(
           DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM2,NUM_TYPE) == SM_MEM)||
           ((int)dsatutil_get_val(
           DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM1,NUM_TYPE) == SM_MEM) )
        )
      {
        /* Revert the old qcpms_val */
        DSATUTIL_SET_VAL(DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM3,0,mem3_type,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM2,0,mem2_type,NUM_TYPE)
        DSATUTIL_SET_VAL(DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM1,0,mem1_type,NUM_TYPE)
        DS_AT_MSG0_HIGH("Erroneous input params in $QCPMS in NV only mode");
#ifdef FEATURE_DSAT_EXTENDED_SMS
        res_buff_ptr->used = 
          dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
        return DSAT_CMD_ERR_RSP;
#else
        return DSAT_ERROR;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      }

      if(mem3_type != (int)dsatutil_get_val(
                        DSAT707_SMS_QCPMS_IDX,0,CPMS_MEM3,NUM_TYPE))
      {
        dsatsmsi_mem_report.mem3_changed = TRUE;
      }
      
      dsatsmsi_mem_report.is_read = FALSE;
    }
    else /* for READ command */
    {
      dsatsmsi_mem_report.is_read = TRUE;
    }

    
    dsatsmsi_mem_report.mem = CPMS_MEM1;
      
    /* get memory status of msg store one at a time */
      
    /* set mem_store */
    mem_store = dsatsmsi_memory_lookup(
                (int)dsatutil_get_val(
                DSAT707_SMS_QCPMS_IDX,0,dsatsmsi_mem_report.mem,NUM_TYPE), 
                dsatsmsi_mem_report.mem,CDMA_CMD_MODE);

    DSAT_SMS_SET_STATE(SMS_CPMS,tok_ptr->name, temp_hc_cmd);
#ifdef FEATURE_DUAL_SIM
    mem_status = wms_cfg_ms_get_memory_status(
                                dsatsmsi_client_id,
                                dsat_get_current_1x_subs_id(),
                                dsatsmsi_cmd_cb_func,
                                &dsatsmsi_pres_cmd,
                                mem_store,
                                WMS_TAG_NONE); 
#else
   mem_status = wms_cfg_get_memory_status(
                                dsatsmsi_client_id,
                                dsatsmsi_cmd_cb_func,
                                &dsatsmsi_pres_cmd,
                                mem_store,
                                WMS_TAG_NONE);
#endif /* FEATURE_DUAL_SIM */
    if(mem_status == WMS_OK_S)
    {
        result = DSAT_ASYNC_CMD;
    }
    else
    {
      /* Clear the header info too */
      DSAT_SMS_STATE_CLEAN();
#ifdef FEATURE_DSAT_EXTENDED_SMS
      res_buff_ptr->used = 
        dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_UNKNOWN_ERR);
      result = DSAT_CMD_ERR_RSP;
#else
      result = DSAT_ERROR;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    }
  } /* ( tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA|QU) ) */

  /*---------------------------------------
   Processing command with no args :
   ----------------------------------------*/
  else if (tok_ptr->op == (NA) ) 
  {
    DS_AT_MSG0_ERROR("$QCPMS need at least one argument");
#ifdef FEATURE_DSAT_EXTENDED_SMS
    res_buff_ptr->used = 
      dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
    return DSAT_CMD_ERR_RSP;
#else
    result = DSAT_ERROR;
    return result;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  }
  /*--------------------------------------
   Processing for a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    char cpms_test_nv_mode[] = 
       "(\"ME\",\"MT\"),(\"ME\",\"MT\"),(\"ME\",\"MT\")";
    char cpms_test_ruim_mode[] = 
       "(\"ME\",\"MT\",\"SM\"),(\"ME\",\"MT\",\"SM\"),(\"ME\",\"MT\",\"SM\")";

    res_buff_ptr->used = 
      (uint16) snprintf((char*)res_buff_ptr->data_ptr,
                               res_buff_ptr->size,
                              "%s: %s",
                              tok_ptr->name,
                               ( ( ds_nv_item.rtre_config == 
                                            NV_RTRE_CONFIG_NV_ONLY ) ?
                               (const char*)cpms_test_nv_mode:
                               (const char*)cpms_test_ruim_mode) );
    return DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    DSAT_SMS_STATE_CLEAN();
#ifdef FEATURE_DSAT_EXTENDED_SMS
    res_buff_ptr->used = 
      dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
    result = DSAT_CMD_ERR_RSP;
#else
    result = DSAT_ERROR;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  }

  return result;
} /* dsat707_exec_qcpms_cmd */


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCNMI_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCNMI command.
  $QCNMI is used to select the procedure of how receiving new messages
  from the network is indicated to TE.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
  DSAT_CMD_ERR_RSP : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcnmi_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type mode_val;
  dsat_num_item_type mt_val;
  dsm_item_type *buff_ptr;
  unsigned int used_val;
  boolean temp_hc_cmd = FALSE;

/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( 0 == dsatutil_strcmp_ig_sp_case((const byte *) tok_ptr->name,(const byte *)"^HCNMI"))
  {
    temp_hc_cmd = TRUE;
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */ 

  buff_ptr = res_buff_ptr;

  if( tok_ptr->op == (NA|EQ|AR) )
  {

#ifdef FEATURE_DSAT_EXTENDED_SMS
    if( tok_ptr->args_found > 6)
    {
      DS_AT_MSG0_HIGH("Only six parameters are accepted for $QCNMI");
      res_buff_ptr->used = 
        dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
      return DSAT_CMD_ERR_RSP;
    }
#else
    if( tok_ptr->args_found > 3)
    {
      DS_AT_MSG0_HIGH("Only three parameters are accepted for $QCNMI");
      return DSAT_ERROR;
    }
#endif /* FEATURE_DSAT_EXTENDED_SMS */

    if( VALID_TOKEN(0))
    {
      if ( ATOI_OK != dsatutil_atoi ( &mode_val, tok_ptr->arg[0], 10 ) )
      {
#ifdef FEATURE_DSAT_EXTENDED_SMS
        res_buff_ptr->used = 
          dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
        return DSAT_CMD_ERR_RSP;
#else
        return DSAT_ERROR;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      }
    }
    else
    {
      mode_val = (dsat_num_item_type)dsatutil_get_val(
        DSAT707_SMS_QCNMI_IDX,0,CNMI_MODE_VAL,NUM_TYPE);
    }

    if( VALID_TOKEN(1))
    {
      if ( ATOI_OK != dsatutil_atoi ( &mt_val, tok_ptr->arg[1], 10 ) 
#ifdef FEATURE_DSAT_EXTENDED_SMS
          || ( mt_val == 0 )
#endif /* FEATURE_DSAT_EXTENDED_SMS */
         )
      {
#ifdef FEATURE_DSAT_EXTENDED_SMS
        res_buff_ptr->used = 
          dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
        return DSAT_CMD_ERR_RSP;
#else
        return DSAT_ERROR;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      }
    }
    else
    {
      mt_val = (dsat_num_item_type)dsatutil_get_val(
                      DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE);
    }

    if ( ( ( mt_val == 2 ) && ( mode_val != 1 ) ) ||
         ( ( mt_val == 3 ) && ( mode_val != 1 ) ) )
    {
      /* we got an error, format a +CMS ERROR */
      res_buff_ptr->used =
        dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
      return DSAT_CMD_ERR_RSP;
    }
  }
#ifdef FEATURE_DSAT_EXTENDED_SMS
  else if (tok_ptr->op == (NA) ) 
  {
    return DSAT_OK;
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  
  result = dsatparm_exec_param_cmd ( mode,
                                      tab_entry,
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
      DSAT_SMS_SET_STATE(SMS_CNMI,tok_ptr->name,temp_hc_cmd);
      result=dsat707smsi_route_config((dsat_num_item_type)dsatutil_get_val(
                      DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE), 
                                      (dsat_num_item_type)dsatutil_get_val(
                      DSAT707_SMS_QCNMI_IDX,0,CNMI_DS_VAL,NUM_TYPE) ); 
      if(result != DSAT_ASYNC_CMD)
      {
        /* Clear the header info too */
        DSAT_SMS_STATE_CLEAN();
      }
    }
  }

  return result;
} /* dsat707sms_exec_qcnmi_cmd */


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMGS command.
  $CMGS is used to send a message from TE to the network.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

  SIDE EFFECTS
  None.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcmgs_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
)
{
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
  dsat_result_enum_type result = DSAT_OK;
  boolean temp_hc_cmd = FALSE;
  uint16 len_quoted_string = 0;
  
#ifdef FEATURE_DSAT_EXTENDED_CMD
  dsatcmif_servs_state_ss_info  *ph_ss_val = NULL;
#endif /* FEATURE_DSAT_EXTENDED_CMD */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( 0 == dsatutil_strcmp_ig_sp_case((const byte *) tok_ptr->name,(const byte *)"^HCMGS"))
  {
    temp_hc_cmd = TRUE;
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */ 

  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if((dsat_num_item_type)dsatutil_get_val(
                                DSAT707_SMS_QCMGF_IDX,0,0,NUM_TYPE))
    {
      /* Retain the parameters from the this command */
      /* they will be used later when sending the message */
      /* In case of text mode, <da> is one of the parameter */
      /* We don't support pdu mode */
      /* get the send address */
      byte * toda = NULL;
      if( tok_ptr->args_found > 2 )
      {
        DS_AT_MSG0_ERROR("More then two Args in QCMGS");
        return DSAT_ERROR;
      }
      
      /* Check if we are in HDR mode */
#ifdef FEATURE_DSAT_EXTENDED_CMD
      ph_ss_val = dsat_get_base_addr(DSAT_SS_PH_SS_VALS, FALSE);

      if (ph_ss_val->dsat_sys_mode == SYS_SYS_MODE_HDR)
      {
        /* we got an error, format a +CMS ERROR */
        res_buff_ptr->used = (uint16) dsatsmsi_cms_error(
                                                  res_buff_ptr,
                                                  (int)DSAT_CMS_OP_NOT_ALLOWED);
        return DSAT_CMD_ERR_RSP;
      }
#endif /* FEATURE_DSAT_EXTENDED_CMD */

      /* If user specified a <da>, store it now. It will be used later */
  
      /* Strip out the '"' from the <da> */
      if(*tok_ptr->arg[0] != '\0' )
      {
        len_quoted_string =(uint16)strlen((char*)tok_ptr->arg[0]);
        if ( !dsatsmsi_strip_quotes( tok_ptr->arg[0], tok_ptr->arg[0], len_quoted_string) )
        {
          /* We got an address without quotes */
          DS_AT_MSG0_ERROR("<oa> without quotes - not supported");
          res_buff_ptr->used = (uint16) dsatsmsi_cms_error(
            res_buff_ptr, (int)DSAT_CMS_INVALID_TXT_PARAM);
          result = DSAT_CMD_ERR_RSP;
        }
        if ( strlen((const char *)tok_ptr->arg[0]) > WMS_GW_ADDRESS_MAX )
        {
          DS_AT_MSG0_ERROR("<oa> length greater than 20 not supported");
          res_buff_ptr->used = (uint16) dsatsmsi_cms_error(
            res_buff_ptr, (int)DSAT_CMS_INVALID_TXT_PARAM);
          result = DSAT_CMD_ERR_RSP;

        }

      }
      else
      {
        /* <da> is null */
        result = DSAT_ERROR;
      }
      
      /* continue processing if there is not error */
      if(result == DSAT_OK)
      {
        if ( tok_ptr->args_found == 2 )
        {
          toda = tok_ptr->arg[1];
        }
        memset( &send_addr, 0x0, sizeof(wms_address_s_type) );
        if(!dsatsmsi_str_to_addr
           (&send_addr, (byte*)tok_ptr->arg[0], toda, DSATSMS_TEXT_MODE)
           )
        {
          /* We got an error in address preocessing */
          res_buff_ptr->used = (uint16) dsatsmsi_cms_error(
                               res_buff_ptr, (int)DSAT_CMS_INVALID_TXT_PARAM);
          result =  DSAT_CMD_ERR_RSP;
        }
        else
        {
          /* Send mesasge to SIO data preprocessor to change state to */
          /* handle SMS message entry */
#ifdef FEATURE_DSAT_EXTENDED_SMS
          dsat707smsi_send_encoding = convert_encoding_ext(
                             (dsat_num_item_type)dsatutil_get_val(
                              DSAT707_SMS_HSMSSS_IDX,0,2,NUM_TYPE));
#endif /* FEATURE_DSAT_EXTENDED_SMS */
          dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
          DSAT_SMS_SET_STATE(SMS_CMGS,tok_ptr->name,temp_hc_cmd);
          result = DSAT_ASYNC_CMD;
        }
      } /* ( result == DSAT_OK ) */
    }
    else
    {
      /* PDU Mode Not Supported */
      DS_AT_MSG1_ERROR("PDU Mode not supported in Cmd %d",dsatsmsi_pres_cmd.cmd_name);
      return DSAT_ERROR;
    }
  } /* ( tok_ptr->op == (NA|EQ|AR) ) */

  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    result = DSAT_OK;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsat707sms_exec_qcmgs_cmd */ 


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$CMGR command.
  $CMGR is used to read a sms message.

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
dsat_result_enum_type dsat707sms_exec_qcmgr_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
)
{
  dsat_result_enum_type         result;
  wms_memory_store_e_type       mem_store;
  wms_status_e_type             msg_status;
  dsat_num_item_type            index;
  boolean                       temp_hc_cmd = FALSE;
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( 0 == dsatutil_strcmp_ig_sp_case((const byte *) tok_ptr->name,(const byte *)"^HCMGR"))
  {
    temp_hc_cmd = TRUE;
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */ 

  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    mem_store = dsatsmsi_memory_lookup((int)dsatutil_get_val(
                                DSAT707_SMS_QCPMS_IDX,0,0,NUM_TYPE),
                                CPMS_MEM1,CDMA_CMD_MODE);

#ifdef FEATURE_DSAT_EXTENDED_SMS
    if( ( TRUE == temp_hc_cmd) && ( tok_ptr->args_found > 2 ))
#else
    if ( tok_ptr->args_found > 1 )
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    {
      DS_AT_MSG1_ERROR("Invalid %d Arg in QCMGR",tok_ptr->args_found);
      return DSAT_ERROR;
    }
    if (dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK)
    {
      /* We got a bad index */
      return DSAT_ERROR;
    }
#ifdef FEATURE_DSAT_EXTENDED_SMS
    hcmgr_mode = 0;
    if( tok_ptr->args_found == 2 )
    {
      if (dsatutil_atoi (&hcmgr_mode, tok_ptr->arg[1], 10) != ATOI_OK)
      {
        /* We got a bad mode */
        return DSAT_ERROR;
      }
    }
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    DSAT_SMS_SET_STATE(SMS_CMGR,tok_ptr->name,temp_hc_cmd);
#ifdef FEATURE_DUAL_SIM
     msg_status = wms_msg_ms_read
                                (
                                      dsatsmsi_client_id,
                                      dsat_get_current_1x_subs_id(),
                                      dsatsmsi_cmd_cb_func,
                                      (void*)&dsatsmsi_pres_cmd,
                                      mem_store,
                                      index
                                      );
#else
    msg_status = wms_msg_read
                            (
                                      dsatsmsi_client_id,
                                      dsatsmsi_cmd_cb_func,
                                      (void*)&dsatsmsi_pres_cmd,
                                      mem_store,
                                      index
                                      );
#endif /* FEATURE_DUAL_SIM */

    if (msg_status == WMS_OK_S)
    {
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      /* we got an error, format a +CMS ERROR */
      res_buff_ptr->used += (uint16) dsatsmsi_cms_error(res_buff_ptr,
                                           (int)DSAT_CMS_UNKNOWN_ERR);
      result = DSAT_CMD_ERR_RSP;
      DSAT_SMS_STATE_CLEAN();
    }
  }

  /*---------------------------  
   Processing READ command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|QU) )
  {
    /* It is an error to query this command */
    result = DSAT_ERROR;
  }

  /*--------------------------------------
   Processing a TEST command :
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* do nothing, just return OK */
    result = DSAT_OK;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsat707sms_exec_qcmgr_cmd */ 


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGL_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMGL command. $QCMGL command lists
  all the SMS saved in the loaction.
  
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
dsat_result_enum_type dsat707sms_exec_qcmgl_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /* Place to put response       */
)
{
  dsat_result_enum_type result;
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  byte *data_ptr;
  int all_flag=1;
  dsat_string_item_type txt_tststr [70];
  boolean               temp_hc_cmd = FALSE;
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( 0 == dsatutil_strcmp_ig_sp_case((const byte *)tok_ptr->name,
                                         (const byte *)"^HCMGL"))
  {
    temp_hc_cmd = TRUE;
    (void)strlcpy((char *)txt_tststr,"(0,1,2,3,4)",sizeof(txt_tststr));
  }
  else
#endif /* FEATURE_DSAT_EXTENDED_SMS */ 
  {
    (void)strlcpy((char *)txt_tststr,
     "(\"REC UNREAD\",\"REC READ\",\"STO UNSENT\",\"STO SENT\",\"ALL\")",
     sizeof(txt_tststr));
  }
  data_ptr = res_buff_ptr->data_ptr;
  
  mem_store = dsatsmsi_memory_lookup((int)dsatutil_get_val(
                                 DSAT707_SMS_QCPMS_IDX,0,0,NUM_TYPE),
                                 CPMS_MEM1,CDMA_CMD_MODE);
  
  /*-------------------------------------  
   Processing WRITE and DEFAULT command :
   -------------------------------------*/
  if ( ( tok_ptr->op == (NA|EQ|AR) ) || ( tok_ptr->op == (NA) ) )
  {
    mem_store = dsatsmsi_memory_lookup((int)dsatutil_get_val(
                                DSAT707_SMS_QCPMS_IDX,0,0,NUM_TYPE),
                                         CPMS_MEM1,CDMA_CMD_MODE);
    dsatsmsi_pres_cmd.cmd_name = SMS_CMGL;
    if ( tok_ptr->op == (NA|EQ|AR) )
    {
      if( tok_ptr->args_found > 1 )
      {
        DS_AT_MSG0_ERROR("More then one Args in QCMGL");
        return DSAT_ERROR;
      }
#ifdef FEATURE_DSAT_EXTENDED_SMS
      if (TRUE == temp_hc_cmd )
      {
        tag = stat707_lookup_ext(tok_ptr->arg[0], &all_flag);
      }
      else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      {
        tag = stat707_lookup(tok_ptr->arg[0], &all_flag);
      }
      if ( !tag )
      {
        /* Bad Tag */
        /* Return an error */
        DS_AT_MSG1_ERROR("Bad tag input in %d cmd ",dsatsmsi_pres_cmd.cmd_name);
        DSAT_SMS_STATE_CLEAN();
        res_buff_ptr->used = dsatsmsi_cms_error(res_buff_ptr, 
                                                (int)DSAT_CMS_OP_NOT_SUPPORTED);
        return DSAT_CMD_ERR_RSP;
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
      dsatsmsi_cmd_sm.num_of_stat = 4;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.list_begin = TRUE;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MT_NOT_READ;
      dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_SENT;
      dsatsmsi_cmd_sm.stat_list[3] = WMS_TAG_MO_NOT_SENT;
    }
    else
    {
      /* We are listing a single stat */
      dsatsmsi_cmd_sm.num_of_stat = 1;
      dsatsmsi_cmd_sm.list_begin = TRUE;
      dsatsmsi_cmd_sm.present_stat = 1;
    }
    DSAT_SMS_SET_STATE(SMS_CMGL,tok_ptr->name,temp_hc_cmd);
#ifdef FEATURE_DUAL_SIM
          msg_status = wms_cfg_ms_get_message_list(
                                        dsatsmsi_client_id,
                                        dsat_get_current_1x_subs_id(),
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
      res_buff_ptr->used = 
           dsatsmsi_cms_error(res_buff_ptr, (int)DSAT_CMS_UNKNOWN_ERR);
      result = DSAT_CMD_ERR_RSP;
      DSAT_SMS_STATE_CLEAN();
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
  

    res_buff_ptr->used += (word)snprintf ( (char*)data_ptr,
                                      res_buff_ptr->size - res_buff_ptr->used,
                                      "%s", txt_tststr);

    result = DSAT_OK;
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsat707sms_exec_qcmgl_cmd */ 


/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMSS_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMSS command.
  $QCMSS is used to send a message already stored from Memory to the network.

  DEPENDENCIES
  None

  RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if command is successfully sent to WMS

  SIDE EFFECTS
  None.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_qcmss_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result; 

  /* data structures used in wms_msg_send */
  wms_send_mode_e_type send_mode = WMS_SEND_MODE_MEMORY_STORE;
  wms_status_e_type msg_status = WMS_OK_S;
  wms_memory_store_e_type mem_store;
  dsm_item_type *buff_ptr = res_buff_ptr;
  atoi_enum_type err_type = ATOI_OK;
  dsat_num_item_type index =0 ;
  boolean            temp_hc_cmd = FALSE;

  wms_client_message_s_type         *is707_client_msg = NULL;
  uint16 len_quoted_string = 0;

#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( 0 == dsatutil_strcmp_ig_sp_case((const byte *)tok_ptr->name,
                                         (const byte *)"^HCMSS"))
  {
    temp_hc_cmd = TRUE;
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */ 
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if( tok_ptr->args_found > 3 )
    {
      DS_AT_MSG0_ERROR("More then three Args in QCMSS");
      return DSAT_ERROR;
    }
    /* Case when we have to just send the message from specified index */
    if( tok_ptr->args_found == 1 )
    {
      /*convert the index into a suitable number */
      err_type = dsatutil_atoi(&index, tok_ptr->arg[0], 10);
      if (err_type != ATOI_OK)
      {
        return DSAT_ERROR;
      }

      is707_client_msg = (wms_client_message_s_type *) 
                            dsat_alloc_memory(sizeof(wms_client_message_s_type), FALSE);

      /* get the memory from which the message has to be picked up */
      is707_client_msg->msg_hdr.mem_store = 
        dsatsmsi_memory_lookup((int)dsatutil_get_val(
                    DSAT707_SMS_QCPMS_IDX,0,1,NUM_TYPE), CPMS_MEM2,CDMA_CMD_MODE);

      is707_client_msg->msg_hdr.index = index;
    
      /* fill out rest of the enums */
      is707_client_msg->msg_hdr.message_mode = WMS_MESSAGE_MODE_CDMA;
      is707_client_msg->msg_hdr.tag = WMS_TAG_NONE;
      /* We only have index, assume SMS-SUBMIT */
      is707_client_msg->u.cdma_message.raw_ts.format = WMS_FORMAT_CDMA;
      send_mode = WMS_SEND_MODE_MEMORY_STORE;

      /* finally we are done stuffing all the required parameters */
      /* send the message to WMS                                */
      DSAT_SMS_SET_STATE(SMS_CMSS,tok_ptr->name,temp_hc_cmd);
#ifdef FEATURE_DUAL_SIM
              msg_status = wms_msg_ms_send(
                                           dsatsmsi_client_id,
                                           dsat_get_current_1x_subs_id(),
                                           dsatsmsi_cmd_cb_func,
                                           (void*)&dsatsmsi_pres_cmd,
                                           send_mode,
                                           is707_client_msg
                                          );
          
#else
              msg_status = wms_msg_send(
                                           dsatsmsi_client_id,
                                           dsatsmsi_cmd_cb_func,
                                           (void*)&dsatsmsi_pres_cmd,
                                           send_mode,
                                           is707_client_msg
                                          );
#endif /* FEATURE_DUAL_SIM */
      if (msg_status  != WMS_OK_S)
      {
        DS_AT_MSG0_ERROR("Error sending the SMS");
        DSAT_SMS_STATE_CLEAN();
        result =  DSAT_ERROR;
      }

      /* Remove the allocated buffer */
      dsatutil_free_memory((void *)is707_client_msg);
    }
    /* We have to overwrite the <da> */
    /* Read the message first        */
    else
    {
      /* Save the <da>                                                      */
      /* It will be used to replace existing <da> after we read the message */

      memset( (void *) &send_addr, 0, sizeof(wms_address_s_type));

      if( tok_ptr->args_found >= 2 )
      {
        if( VALID_TOKEN(1) )
        {
          len_quoted_string =(uint16)strlen((char*)tok_ptr->arg[1]);
          /* Strip out the '"' from the <da> in place */
          if ( !dsatsmsi_strip_quotes( tok_ptr->arg[1], tok_ptr->arg[1], len_quoted_string ) )
          {
            /* We got an address with out quotes */
            return DSAT_ERROR;
          }
          if ( strlen((const char *)tok_ptr->arg[1]) > WMS_GW_ADDRESS_MAX )
          {
            DS_AT_MSG0_HIGH("<oa> length greater than 20 not supported");
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
          if(!dsatsmsi_str_to_addr(&send_addr,tok_ptr->arg[1],
                                              NULL,DSATSMS_TEXT_MODE))
          {
            /* Bad Address */
            return DSAT_ERROR;
          }
          break;
      
        case 3:
          if (!dsatsmsi_str_to_addr( &send_addr, tok_ptr->arg[1], 
                                     tok_ptr->arg[2], DSATSMS_TEXT_MODE )
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
      /* <mem1> is used for reading the SMS */
      mem_store = dsatsmsi_memory_lookup((int)dsatutil_get_val(
                                DSAT707_SMS_QCPMS_IDX,0,0,NUM_TYPE),
                                          CPMS_MEM1,CDMA_CMD_MODE);
    
      if (dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK)
      {
        /* We got a bad index */
        return DSAT_ERROR;
      }
      /* Read the message */
      /* once we get the message back from the WMS */
      /* Overwrite the present <da> with the new one (dsatetsismsi_send_addr) */
      DSAT_SMS_SET_STATE(SMS_CMSS,tok_ptr->name,temp_hc_cmd);
#ifdef FEATURE_DUAL_SIM
                    msg_status = wms_msg_ms_read
                                             (
                                            dsatsmsi_client_id,
                                            dsat_get_current_1x_subs_id(),
                                            dsatsmsi_cmd_cb_func,
                                            (void*)&dsatsmsi_pres_cmd,
                                            mem_store,
                                            index
                                            );
#else
                    msg_status = wms_msg_read
                                             (
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

}/* dsat707sms_exec_qcmss_cmd */

/*===========================================================================
  FUNCTION DSAT707SMS_EXEC_QCMGD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCMGD command.
  +CMGD is used to delete sms messages from <mem1>.

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
dsat_result_enum_type dsat707sms_exec_qcmgd_cmd
(
  dsat_mode_enum_type mode,             /* AT command mode:            */
  const dsati_cmd_type *tab_entry,      /* Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /* Command tokens from parser  */
  dsm_item_type *rb_ptr                 /* Place to put response       */
)
{
  dsat_result_enum_type result;
  dsm_item_type *buff_ptr;
  wms_memory_store_e_type mem_store;
  wms_status_e_type msg_status;
  unsigned int used_val;
  boolean      temp_hc_cmd = FALSE;
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( 0 == dsatutil_strcmp_ig_sp_case((const byte *)tok_ptr->name,
                                         (const byte *)"^HCMGD"))
  {
    temp_hc_cmd = TRUE;
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */ 

  buff_ptr = rb_ptr;
  
  mem_store = dsatsmsi_memory_lookup((int)dsatutil_get_val(
                                DSAT707_SMS_QCPMS_IDX,0,0,NUM_TYPE),
                                CPMS_MEM1,CDMA_CMD_MODE);
  /*----------------------------  
   Processing WRITE command :
   -----------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    dsat_num_item_type index;
    if( ( tok_ptr->args_found == 1 )
#ifdef FEATURE_DSAT_EXTENDED_SMS
        || ((tok_ptr->args_found == 2) && (*tok_ptr->arg[1] == '0'))
#endif /* FEATURE_DSAT_EXTENDED_SMS */
       )
    {
      if(dsatutil_atoi (&index, tok_ptr->arg[0], 10) != ATOI_OK )
      {
        /* We received a bad index */
        return DSAT_ERROR;
      }
      DSAT_SMS_SET_STATE(SMS_CMGD,tok_ptr->name,temp_hc_cmd);
      dsatsmsi_cmd_sm.num_of_stat = 0;
      dsatsmsi_cmd_sm.present_index = 0;
      dsatsmsi_cmd_sm.num_of_indices = 0;
      dsatsmsi_cmd_sm.is_test =  FALSE;
      dsatsmsi_cmd_sm.one_index = TRUE;
#ifdef FEATURE_DUAL_SIM
          msg_status = wms_msg_ms_delete(
                                    dsatsmsi_client_id,
                                    dsat_get_current_1x_subs_id(),
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

      if (msg_status == WMS_OK_S)
      {
        result = DSAT_ASYNC_CMD;
      }
      else 
      {
        /* we got an error, format a +CMS ERROR */
        /* Clear the header info too */
        DSAT_SMS_STATE_CLEAN();
        buff_ptr->used += dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_UNKNOWN_ERR);
        result = DSAT_CMD_ERR_RSP;
      }
      return result;
    }
    else if ( tok_ptr->args_found == 2 )
    {
      if ((dsatutil_atoi (&index, tok_ptr->arg[1], 10) != ATOI_OK ) ||
          (index > 4))
      {
        /* We received a bad status */
        buff_ptr->used = dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_OP_NOT_ALLOWED);
        return DSAT_CMD_ERR_RSP;
      }
      DSAT_SMS_SET_STATE(SMS_CMGD,tok_ptr->name,temp_hc_cmd);
      result = dsatsmsi_delete_messages(tok_ptr->arg[1]);
      if (result == DSAT_OK)
      {
        result = DSAT_ASYNC_CMD;
      }
      else 
      {
        /* we got an error, format a +CMS ERROR */
        DSAT_SMS_STATE_CLEAN();
        used_val = dsatsmsi_cms_error(buff_ptr, (int)DSAT_CMS_UNKNOWN_ERR);
        buff_ptr->used += (byte)used_val;
        result = DSAT_CMD_ERR_RSP;
      }
      return result;
    }
    else
    {
      /* Wrong number of arguments */
      result = DSAT_ERROR;
    }
  } /* Write command */
  /*---------------------------  
   Processing TEST command :
  -----------------------------*/
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    DSAT_SMS_SET_STATE(SMS_CMGD,tok_ptr->name,temp_hc_cmd);

#ifdef FEATURE_DUAL_SIM
          msg_status = wms_cfg_ms_get_message_list(
                                        dsatsmsi_client_id,
                                        dsat_get_current_1x_subs_id(),
                                        dsatsmsi_cmd_cb_func,
                                        (void *)&dsatsmsi_pres_cmd,
                                        mem_store,
                                        WMS_TAG_NONE
                                       );
#else
          msg_status = wms_cfg_get_message_list(
                                                dsatsmsi_client_id,
                                                dsatsmsi_cmd_cb_func,
                                                (void *)&dsatsmsi_pres_cmd,
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
      buff_ptr->used += dsatsmsi_cms_error(buff_ptr, 
                                  (int)DSAT_CMS_UNKNOWN_ERR);
      result = DSAT_CMD_ERR_RSP;
      DSAT_SMS_STATE_CLEAN();
    }
  }
  /*--------------------------------------------------
   Processing command with no args and wrong operator
   ---------------------------------------------------*/
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsat707sms_exec_qcmgd_cmd */ 

/*===========================================================================

FUNCTION DSATETSISMS_EXEC_QCMGW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes AT$QCMGW command. 
  $QCMGW is used to store message to memory storage <mem2>

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
dsat_result_enum_type dsat707sms_exec_qcmgw_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  boolean temp_hc_cmd = FALSE;
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
  uint16 len_quoted_string = 0;

#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( 0 == dsatutil_strcmp_ig_sp_case((const byte *) tok_ptr->name,(const byte *)"^HCMGW"))
  {
    temp_hc_cmd = TRUE;
  }
#endif /* FEATURE_DSAT_EXTENDED_SMS */ 

  /*-------------------------
   Processing WRITE command:
   --------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if( (dsat_num_item_type)dsatutil_get_val(
                               DSAT707_SMS_QCMGF_IDX,0,0,NUM_TYPE) )
    {
      len_quoted_string =(uint16)strlen((char*)tok_ptr->arg[0]);

      /* convert the <da> to suitable a data structure */

      /* Strip out the '"' from the <da> in place */
      if ( !dsatsmsi_strip_quotes( tok_ptr->arg[0], tok_ptr->arg[0], len_quoted_string ) )
      {
        return DSAT_ERROR;
      }
      if ( strlen((const char *)tok_ptr->arg[0]) > WMS_GW_ADDRESS_MAX )
      {
        DS_AT_MSG0_HIGH("<oa> length greater than 20 not supported");
        return DSAT_ERROR;
      }
#ifdef FEATURE_DSAT_EXTENDED_SMS
      if ( FALSE == temp_hc_cmd )
#endif /* FEATURE_DSAT_EXTENDED_SMS */

      {
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
               (&send_addr, tok_ptr->arg[0], toa_ptr, DSATSMS_TEXT_MODE)
              )
            {
              /* We got a bad address */
              result = DSAT_CMD_ERR_RSP;
            }
            
          }
          break;
  
          /* <stat> given */
          case 3:
            if ( !dsatsmsi_str_to_addr ( 
                                          &send_addr, tok_ptr->arg[0],
                                          tok_ptr->arg[1],
                                          DSATSMS_TEXT_MODE
                                        ) )
            {
              /* We got a bad address */
              result = DSAT_CMD_ERR_RSP;
            }
            /* find out what type of STAT we are about to store */
            else if((dsat707smsi_send_stat = stat707_lookup(tok_ptr->arg[2],NULL))
                      == WMS_TAG_NONE ) 
            {
              /* We got a bad <stat> */
              result = DSAT_CMD_ERR_RSP;
            }
            break;
                              
          default:
            /* We shouldn't be here    */
            /* Bad number of arguments */
            result = DSAT_CMD_ERR_RSP;
            break;
        }
      }
#ifdef FEATURE_DSAT_EXTENDED_SMS
      else  /* HCMGW */
      {
        boolean timestamp_given = FALSE;
        uint8  arg_i = 0;
        boolean toa_given = FALSE;
        dsat_num_item_type  index = 0;

        switch ( tok_ptr->args_found )
        {
          case 6:  /* no <toa> no <timestamp> */
          case 7:  /* <toa> given, but no <timestamp> */
          case 12: /* no <toa>, but <timestamp> given  */
          case 13: /* <toa> + <timestamp> */
            toa_given = ( (tok_ptr->args_found ==  7) || (tok_ptr->args_found ==  13));
            timestamp_given = ( (tok_ptr->args_found ==  12) || (tok_ptr->args_found ==  13));

            if ( !dsatsmsi_str_to_addr ( 
                            &send_addr, tok_ptr->arg[0],
                            (toa_given ? tok_ptr->arg[1] : NULL),
                            DSATSMS_TEXT_MODE
                          ) )
            {
              /* We got a bad address */
              result = DSAT_CMD_ERR_RSP;
            }
            /* find out what type of STAT we are about to store */
            else if((dsat707smsi_send_stat = 
                              stat707_lookup_ext((toa_given ? 
                              tok_ptr->arg[2] : tok_ptr->arg[1]), 
                              NULL)) == WMS_TAG_NONE )
            {
              /* We got a bad <stat> */
              result = DSAT_CMD_ERR_RSP;
            }
            arg_i = (toa_given ? 3 : 2);
            /* Update <Priority> */
            if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
            {
              /* We received a bad index */
              result = DSAT_CMD_ERR_RSP;
              break;
            }
            if ( (wms_priority_e_type)index > WMS_PRIORITY_EMERGENCY) 
            {
              result = DSAT_CMD_ERR_RSP;
              break;
            }
            dsat707smsi_send_priority = (wms_priority_e_type)index;
  
            /* Update <Type> */
            if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
            {
              /* We received a bad index */
              result = DSAT_CMD_ERR_RSP;
              break;
            }
            if ( index > 3 ) 
            {
              result = DSAT_CMD_ERR_RSP;
              break;
            }
            dsat707smsi_send_type = index;
  
            /* Update <format> */
            if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
            {
              /* We received a bad index */
              result = DSAT_CMD_ERR_RSP;
              break;
            }
            if ( index > 6 ) 
            {
              result = DSAT_CMD_ERR_RSP;
              break;
            }
            dsat707smsi_send_encoding = convert_encoding_ext(index);
  
            /* Update <lang> */
            if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
            {
              /* We received a bad index */
              result = DSAT_CMD_ERR_RSP;
              break;
            }
            if ( (wms_language_e_type)index > WMS_LANGUAGE_HEBREW ) 
            {
              result = DSAT_CMD_ERR_RSP;
              break;
            }
            dsat707smsi_send_language =(wms_language_e_type)index;
  
            if (timestamp_given) 
            {
              /* If containing time_stamp i.e args_found = 14 */
              /* Update <year> */
              if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
              {
                /* We received a bad index */
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              if ( index > 2099 ) 
              {
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              /* WMS timestamp year is 1B with internal limits to 2099 */
              dsat707smsi_send_mc_time.year = index;
    
              /* Update <month> */
              if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
              {
                /* We received a bad index */
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              if ( (index == 0) || (index > 12) )
              {
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              dsat707smsi_send_mc_time.month = index;
    
              /* Update <day> */
              if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
              {
                /* We received a bad index */
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              if ( (index == 0) || (index > 31) )
              {
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              dsat707smsi_send_mc_time.day = index;
    
              /* Update <hour> */
              if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
              {
                /* We received a bad index */
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              if (index > 23)
              {
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              dsat707smsi_send_mc_time.hour = index;
    
              /* Update <minute> */
              if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
              {
                /* We received a bad index */
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              if ( index > 59) 
              {
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              dsat707smsi_send_mc_time.minute = index;
    
              /* Update <second> */
              if(dsatutil_atoi (&index, tok_ptr->arg[arg_i++], 10) != ATOI_OK )
              {
                /* We received a bad index */
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              if ( index > 59) 
              {
                result = DSAT_CMD_ERR_RSP;
                break;
              }
              dsat707smsi_send_mc_time.second = index;
              cmgw_has_timestamp = TRUE;
            }
            break;
                                  
          default:
            /* We shouldn't be here    */
            /* Bad number of arguments */
            result = DSAT_CMD_ERR_RSP;
            break;
        }

      }
#endif /* FEATURE_DSAT_EXTENDED_SMS */

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
      /* PDU Mode Not Supported */
      return DSAT_ERROR;
    }
  
    /* Send mesasge to SIO data preprocessor to change state to */
    /* handle SMS message entry */
    dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
    DSAT_SMS_SET_STATE(SMS_CMGW, tok_ptr->name, temp_hc_cmd);
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
    if( (dsat_num_item_type)dsatutil_get_val(
                               DSAT707_SMS_QCMGF_IDX,0,0,NUM_TYPE) )
    {
      /* We didn't get a <da> from user */
      /* Set the LOCAL structure to 0   */
      memset( (void *) &dsat707smsi_send_addr, 0, sizeof(wms_address_s_type));

      /* stat is not given default it */
      dsat707smsi_send_stat = WMS_TAG_MO_NOT_SENT;

      /* Send mesasge to SIO data preprocessor to change state to */
      /* handle SMS message entry */
      dsatprep_set_preprocessor_mode ( DSATI_SMS_MSG_ENTRY_MODE, port );
      DSAT_SMS_SET_STATE(SMS_CMGW, tok_ptr->name, temp_hc_cmd);
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

}/* dsat707sms_exec_qcmgw_cmd */

/*===========================================================================

FUNCTION DSAT707SMS_SEND_SMS_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat707sms_send_sms_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
)
{
  boolean result = FALSE;
  if((dsat_num_item_type)dsatutil_get_val(
                                DSAT707_SMS_QCMGF_IDX,0,0,NUM_TYPE))
  {
    result = is707_send_smstxt_msg ( msg, len );
  }
  else
  {
    /* PDU Mode not Supported */
    DS_AT_MSG0_ERROR("PDU Mode not Supported");
    dsatsms_abort_cmd (DSAT_CMS_OP_NOT_SUPPORTED);
  }
  if(!result)
  {
    /* We got an error while processing message   */
    /* Send an ASYNC Event to tell ATCOP about it */
    dsatsms_abort_cmd (DSAT_CMS_ME_FAILURE);
  }

}/* dsat707sms_send_sms_msg */

/*************** the local functions ***************************************/

/*===========================================================================

FUNCTION IS707_SEND_SMSTXT_MSG

DESCRIPTION
  Local function that is used to send TEXT message.

DEPENDENCIES

RETURN VALUE
  Returns a boolean.
  TRUE  : If the message has been sent successfully
  FALSE : If there is an error in sending the message
SIDE EFFECTS

===========================================================================*/
LOCAL boolean is707_send_smstxt_msg
(
  byte  *txt_ptr, /* Ptr to the txt message to be sent */
  int   len       /* Len of the txt message to be sent */
)
{
  wms_send_mode_e_type        send_mode;
  wms_write_mode_e_type       write_mode;
  wms_client_msg_hdr_s_type * msg_hdr_ptr = NULL;
  wms_cdma_message_s_type   * cdma_msg_ptr = NULL;
  wms_client_bd_s_type      * cdma_bd_ptr = NULL;
  wms_client_ts_data_s_type * cl_ts_data_ptr = NULL;
  wms_status_e_type           wms_status = WMS_OK_S;
  boolean                     result = FALSE;
  dsat_err_e_type             err_info = DSAT_ERR_NO_ERROR;

  wms_client_message_s_type         *is707_client_msg = NULL;

#ifdef FEATURE_DSAT_EXTENDED_SMS
  const dsat_string_item_type  teleservice_type_str [][5] =
  { "4098", "4097", "4099", "4098", ""};
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/*-------------------------------------------------------------------------*/

  DS_AT_MSG2_HIGH("In is707_send_smstxt_msg : due to Command %d HC cmd %d", 
              dsatsmsi_pres_cmd.cmd_name,dsatsmsi_pres_cmd.is_hc_cmd);

  send_mode = WMS_SEND_MODE_CLIENT_MESSAGE;
  write_mode = WMS_WRITE_MODE_INSERT; 

  is707_client_msg = (wms_client_message_s_type *) 
                        dsat_alloc_memory(sizeof(wms_client_message_s_type), FALSE);

  msg_hdr_ptr = &is707_client_msg->msg_hdr;
  msg_hdr_ptr->message_mode = WMS_MESSAGE_MODE_CDMA;
  msg_hdr_ptr->tag = WMS_TAG_MO_NOT_SENT;
  msg_hdr_ptr->mem_store = WMS_MEMORY_STORE_NONE;
  msg_hdr_ptr->index = (wms_message_index_type) WMS_DUMMY_MESSAGE_INDEX;

  cdma_msg_ptr = &is707_client_msg->u.cdma_message;
  cdma_msg_ptr->is_mo = TRUE;
  
  if( (dsat_num_item_type)dsatutil_get_val(
                          DSAT707_SMS_QCSMP_IDX,0,0,MIX_NUM_TYPE) < 8 )
  {
    cdma_msg_ptr->teleservice = dsat707smsi_teleservice_num_to_enum(
      (char *) qcsmp_tid_valstr[(dsat_num_item_type)dsatutil_get_val(
                             DSAT707_SMS_QCSMP_IDX,0,0,MIX_NUM_TYPE)]);
  }
  else
  {
    result = FALSE;
    err_info = DSAT_ERR_INVALID_OPERATION;
    goto send_result;
  }

  /* Fill out the origination address from the first 
     argu of cmgs */
  cdma_msg_ptr->address = send_addr;

  /* we don't support subaddress for now */
  cdma_msg_ptr->subaddress.number_of_digits = 0;

#ifdef FEATURE_DSAT_EXTENDED_SMS
  /* Turn ON Bearer Reply Option */
  cdma_msg_ptr->is_tl_ack_requested = TRUE;
#else
  cdma_msg_ptr->is_tl_ack_requested = FALSE;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

  /* this is point to point submit message, not a service message */
  cdma_msg_ptr->is_service_present = FALSE;

  /*------------------------------------------------------------------*/
  /* fill out the data field using wms_ts_encode_CDMA_bd
     according to Table 4.4.2-1. of TIA/EIA-637B */
  cl_ts_data_ptr = (wms_client_ts_data_s_type *)dsat_alloc_memory(sizeof(wms_client_ts_data_s_type),FALSE);
  cl_ts_data_ptr->format = WMS_FORMAT_CDMA;
  cdma_bd_ptr = &cl_ts_data_ptr->u.cdma;
  
  /*- Mesage Identifier ----------------------------------------------*/
  cdma_bd_ptr->message_id.type = WMS_BD_TYPE_SUBMIT;
  /* Leave cdma_bd_ptr->message_id.id_number as it is, wms will fill 
     it out when sending it over the air */
#ifdef FEATURE_CDSMS_UDH
  /* don't support User Data Header in User Data Subparameter 
     for now */
  cdma_bd_ptr->message_id.udh_present = FALSE;
#endif /* FEATURE_CDSMS_UDH */
  cdma_bd_ptr->mask = WMS_MASK_BD_MSG_ID;

  /*- User Data ------------------------------------------------------*/
#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ((dsat_num_item_type)dsatutil_get_val(
                             DSAT707_SMS_HSMSSS_IDX,0,2,NUM_TYPE) == 0) 
  {
    cdma_bd_ptr->user_data.encoding = WMS_ENCODING_GSM_7_BIT_DEFAULT;
  }
  else if ((dsat_num_item_type)dsatutil_get_val(
                             DSAT707_SMS_HSMSSS_IDX,0,2,NUM_TYPE) == 1) 
  {
    cdma_bd_ptr->user_data.encoding = WMS_ENCODING_ASCII;
  } 
  else if ((dsat_num_item_type)dsatutil_get_val(
                             DSAT707_SMS_HSMSSS_IDX,0,2,NUM_TYPE) == 6) 
  {
    cdma_bd_ptr->user_data.encoding = WMS_ENCODING_UNICODE;
  }

#else
  cdma_bd_ptr->user_data.encoding = WMS_ENCODING_ASCII;
#endif /*  */

  /* we don't support is91ep for now */
#ifdef FEATURE_DSAT_EXTENDED_SMS
  if ( ((cdma_bd_ptr->user_data.encoding == WMS_ENCODING_OCTET) && 
        (len > 100 )) || /* WMS_CDMA_USER_DATA_OCTET_MAX_EXT */
       ((cdma_bd_ptr->user_data.encoding != WMS_ENCODING_OCTET) && 
        (len > 50 )) ) /* WMS_CDMA_USER_DATA_MAX_EXT */
#else
  if(len > WMS_CDMA_USER_DATA_MAX )
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  {
    result = FALSE;
    err_info = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
    goto send_result;
  }
  txt_ptr[len] = '\0';
  cdma_bd_ptr->user_data.data_len = (uint8) len;
#ifdef FEATURE_DSAT_EXTENDED_SMS
  if (cdma_bd_ptr->user_data.encoding == WMS_ENCODING_UNICODE) 
  {
    cdma_bd_ptr->user_data.number_of_digits = (uint8) len/2;
    (void) dsatutil_memscpy((void*)cdma_bd_ptr->user_data.data,
                            WMS_CDMA_USER_DATA_MAX,
                            (void*)txt_ptr,
                            cdma_bd_ptr->user_data.data_len + 1);
  }
  else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  {
  (void) strlcpy((char *) cdma_bd_ptr->user_data.data,
                 (char *) txt_ptr,
                 cdma_bd_ptr->user_data.data_len + 1);
  }
  cdma_bd_ptr->user_data.padding_bits = 0;

  /* using 8-bit chars number of digits is equal to the lenght in bytes  */
  cdma_bd_ptr->user_data.number_of_digits = (uint8) len;
  cdma_bd_ptr->mask |= WMS_MASK_BD_USER_DATA;

  /*- Valid Period ---------------------------------------------------*/
  if (((char *)dsatutil_get_val(
                   DSAT707_SMS_QCSMP_IDX,0,2,MIX_STR_TYPE))[0] != '\0')
  {
    if((dsat_num_item_type)dsatutil_get_val(DSAT707_SMS_QCSMP_IDX,
                                            0,1,
                                            MIX_NUM_TYPE) == 0)
    {
      /* absolute format */
      (void) dsat707smsi_string_to_timestamp(
               (char *) dsatutil_get_val(DSAT707_SMS_QCSMP_IDX,
                                         0,2,
                                         MIX_STR_TYPE),
                                          &cdma_bd_ptr->validity_absolute);

      cdma_bd_ptr->mask |= WMS_MASK_BD_VALID_ABS;
    }
    else
    {
      /* relative format */
      /* fixme wms_ts_decode_relative_time has minor problem in 
         CDMA mode */
      uint8 rela_time = (uint8) atoi((char *) dsatutil_get_val(DSAT707_SMS_QCSMP_IDX,
                                                               0,2,
                                                               MIX_STR_TYPE));

      wms_ts_decode_relative_time(rela_time, &cdma_bd_ptr->validity_relative);
      cdma_bd_ptr->mask |= WMS_MASK_BD_VALID_REL;
    }
  }

  /*- Deferred Delivery Time -----------------------------------------*/
  if (((char *)dsatutil_get_val(DSAT707_SMS_QCSMP_IDX,
                                0,4,
                                MIX_STR_TYPE))[0] != '\0')
  {
    if((dsat_num_item_type)dsatutil_get_val(DSAT707_SMS_QCSMP_IDX,
                                            0,3,
                                            MIX_NUM_TYPE) == 0)
    {
      /* absolute format */
      (void) dsat707smsi_string_to_timestamp(
               (char *) dsatutil_get_val(DSAT707_SMS_QCSMP_IDX,
                                         0,4,
                                         MIX_STR_TYPE),
                                         &cdma_bd_ptr->deferred_absolute);

      cdma_bd_ptr->mask |= WMS_MASK_BD_DEFER_ABS;
    }
    else
    {
      /* relative format */
      /* fixme wms_ts_decode_relative_time has minor problem in 
         CDMA mode */
      uint8 rela_def = (uint8) atoi((char *) dsatutil_get_val(DSAT707_SMS_QCSMP_IDX,
                                                              0,4,
                                                              MIX_STR_TYPE));

      wms_ts_decode_relative_time(rela_def, &cdma_bd_ptr->deferred_relative);
      cdma_bd_ptr->mask |= WMS_MASK_BD_DEFER_REL;
    }
  }

#ifdef FEATURE_DSAT_EXTENDED_SMS
  /* initialize the mc_time */
  dsat707smsi_current_wms_timestamp( &cdma_bd_ptr->mc_time );
  cdma_bd_ptr->mask |= WMS_MASK_BD_MC_TIME;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

  /*- Priority Indicator ---------------------------------------------*/
#ifdef FEATURE_DSAT_EXTENDED_SMS
  cdma_bd_ptr->priority = (wms_priority_e_type)dsatutil_get_val(DSAT707_SMS_HSMSSS_IDX,
                                                                0,1,
                                                                NUM_TYPE);
  if (cdma_bd_ptr->priority != WMS_PRIORITY_NORMAL) 
  {
    cdma_bd_ptr->mask |= WMS_MASK_BD_PRIORITY;
  }
#else
  cdma_bd_ptr->priority = WMS_PRIORITY_NORMAL;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  cdma_bd_ptr->mask |= WMS_MASK_BD_PRIORITY;

  /*- Privacy Indicator ----------------------------------------------*/
#ifdef FEATURE_DSAT_EXTENDED_SMS
  cdma_bd_ptr->privacy  = (wms_privacy_e_type) dsatutil_get_val(DSAT707_SMS_HSMSSS_IDX,
                                                                0,3,
                                                                NUM_TYPE);
#else
  cdma_bd_ptr->privacy  = WMS_PRIVACY_NORMAL;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  cdma_bd_ptr->mask |= WMS_MASK_BD_PRIVACY;

  /*- Reply Option ---------------------------------------------------*/
#ifdef FEATURE_DSAT_EXTENDED_SMS
  cdma_bd_ptr->reply_option.user_ack_requested = 
               ( ((dsat_num_item_type)dsatutil_get_val(DSAT707_SMS_HSMSSS_IDX,
                                                       0,0,NUM_TYPE) ) && 
                 ((dsat_num_item_type)dsatutil_get_val(DSAT707_SMS_QCNMI_IDX,
                                                       0,CNMI_DS_VAL,NUM_TYPE) == 1));
#else
  cdma_bd_ptr->reply_option.user_ack_requested = FALSE;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
#ifdef FEATURE_DSAT_EXTENDED_SMS
  cdma_bd_ptr->reply_option.delivery_ack_requested = 
                ( ((dsat_num_item_type)dsatutil_get_val(DSAT707_SMS_HSMSSS_IDX,
                                                        0,0,NUM_TYPE)) && 
                ((dsat_num_item_type)dsatutil_get_val(DSAT707_SMS_QCNMI_IDX,
                                                      0,CNMI_DS_VAL,NUM_TYPE) == 2));
#else
  cdma_bd_ptr->reply_option.delivery_ack_requested = FALSE;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

  cdma_bd_ptr->mask |= WMS_MASK_BD_REPLY_OPTION;

#ifndef FEATURE_DSAT_EXTENDED_SMS
  /*- Alert on Message Delivery --------------------------------------*/
  cdma_bd_ptr->alert_mode = WMS_ALERT_MODE_ON;
  cdma_bd_ptr->mask |= WMS_MASK_BD_ALERT;

  /*- Language Indicator ---------------------------------------------*/
  cdma_bd_ptr->language = WMS_LANGUAGE_ENGLISH;
  cdma_bd_ptr->mask |= WMS_MASK_BD_LANGUAGE;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

  /*- Call-Back Number -----------------------------------------------*/
  /* don't support call back number for now */

  /*- Multiple Encoding User Data ------------------------------------*/
  /* wms_client_bd_s_type does not provide this field, we don't 
     support it */

  /*- Message Deposit Index ------------------------------------------*/
  /* wms_client_bd_s_type does not provide this field, we don't 
     support it */

  /*- Service Category Program Results -------------------------------*/
  /* wms_client_bd_s_type does not provide this field, we don't 
     support it */

  if (dsatsmsi_pres_cmd.cmd_name == SMS_CMGW)
  {
    msg_hdr_ptr->mem_store = 
      dsatsmsi_memory_lookup((int)dsatutil_get_val(DSAT707_SMS_QCPMS_IDX,0,1,NUM_TYPE), 
                             CPMS_MEM2,CDMA_CMD_MODE);

    if( dsat707smsi_send_stat != WMS_TAG_NONE )
    {
      msg_hdr_ptr->tag = dsat707smsi_send_stat;
    }
    else
    {
      msg_hdr_ptr->tag = WMS_TAG_MO_NOT_SENT;
    }
#ifdef FEATURE_DSAT_EXTENDED_SMS
    /* override the settings (below) for CMGW command */
    cdma_bd_ptr->user_data.encoding = dsat707smsi_send_encoding;
    
    cdma_msg_ptr->teleservice = dsat707smsi_teleservice_num_to_enum(
      (char *) teleservice_type_str[dsat707smsi_send_type]);
    
    if (cmgw_has_timestamp) 
    {
      time_julian_type      jtime;
      /* if current-timestamp, then ignore the below update */
      jtime.year = dsat707smsi_send_mc_time.year;
      jtime.month = dsat707smsi_send_mc_time.month;
      jtime.day = dsat707smsi_send_mc_time.day;
      jtime.hour = dsat707smsi_send_mc_time.hour;
      jtime.minute = dsat707smsi_send_mc_time.minute;
      jtime.second = dsat707smsi_send_mc_time.second;

      dsat707smsi_set_wms_timestamp(&cdma_bd_ptr->mc_time, &jtime );
      cmgw_has_timestamp = FALSE;
    } 
    else
    {
      dsat707smsi_current_wms_timestamp( &cdma_bd_ptr->mc_time );
    }
    cdma_bd_ptr->mask |= WMS_MASK_BD_MC_TIME;

    /* For HCMGW, we store the parameters with mask */
    cdma_bd_ptr->priority = dsat707smsi_send_priority;
    cdma_bd_ptr->mask |= WMS_MASK_BD_PRIORITY;          
    cdma_bd_ptr->language = dsat707smsi_send_language;
    cdma_bd_ptr->mask |= WMS_MASK_BD_LANGUAGE;          

#endif /* FEATURE_DSAT_EXTENDED_SMS */

  }

  if ( (cdma_bd_ptr->user_data.encoding != WMS_ENCODING_UNICODE)
       && (cdma_bd_ptr->user_data.encoding != WMS_ENCODING_OCTET) 
      && (cdma_bd_ptr->user_data.encoding != WMS_ENCODING_GSM_7_BIT_DEFAULT))
  {
  /* Pack the User Payload to Client TS data*/
  cdma_bd_ptr->user_data.number_of_digits = 
    wms_ts_pack_ascii((char *)txt_ptr,
                      &(cl_ts_data_ptr->u.cdma.user_data.data[0]),
                      &(cdma_bd_ptr->user_data.data_len), 
                      &(cdma_bd_ptr->user_data.padding_bits));
  }
  /* Encode the Client Ts data to Raw Ts data */
  wms_status = wms_ts_encode(cl_ts_data_ptr,
                             &cdma_msg_ptr->raw_ts);

  if(wms_status == WMS_OK_S)
  {
    if (dsatsmsi_pres_cmd.cmd_name == SMS_CMGS)
    {
#ifdef FEATURE_DUAL_SIM
                      wms_status = wms_msg_ms_send(
                                                   dsatsmsi_client_id,
                                                   dsat_get_current_1x_subs_id(),
                                                   dsatsmsi_cmd_cb_func,
                                                   (void*)&dsatsmsi_pres_cmd,
                                                   send_mode,
                                                   is707_client_msg
                                                  );
                  
#else
                      wms_status = wms_msg_send(
                                                   dsatsmsi_client_id,
                                                   dsatsmsi_cmd_cb_func,
                                                   (void*)&dsatsmsi_pres_cmd,
                                                   send_mode,
                                                   is707_client_msg
                                                  );
#endif /* FEATURE_DUAL_SIM */
      if(wms_status == WMS_OK_S)
      {
        result = TRUE;
      }
    }
    else if (dsatsmsi_pres_cmd.cmd_name == SMS_CMGW)
    {
#ifdef FEATURE_DUAL_SIM
            wms_status = wms_msg_ms_write(
                                       dsatsmsi_client_id,
                                       dsat_get_current_1x_subs_id(),
                                       dsatsmsi_cmd_cb_func,
                                       (void*)&dsatsmsi_pres_cmd,
                                       write_mode,
                                       is707_client_msg
                                      );
#else
            wms_status = wms_msg_write(
                                       dsatsmsi_client_id,
                                       
                                       dsatsmsi_cmd_cb_func,
                                       (void*)&dsatsmsi_pres_cmd,
                                       write_mode,
                                       is707_client_msg
                                      );
#endif /* FEATURE_DUAL_SIM */
      if(wms_status == WMS_OK_S)
      {
        result = TRUE;
      }
    }
  }

  /* Clean up the state variables */
  dsat707smsi_send_stat = WMS_TAG_NONE;

send_result:

  dsatutil_free_memory((void *)cl_ts_data_ptr);
  dsatutil_free_memory((void *)is707_client_msg);

  DS_AT_MSG1_HIGH("is707_send_smstxt_msg() err_info: ", err_info);
  return result;

} /* is707_send_smstxt_msg */

/*===========================================================================

FUNCTION STAT707_LOOKUP

DESCRIPTION
  Converts a string parameter to a TAG type ( wms_message_tage_e_type ).
  It also checks to see if the "ALL" wild card is being used as a stat.
  If so then it sets the pointer to the flag

DEPENDENCIES
  None

RETURN VALUE
  Returns an wms_message_tage_e_type enum.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL wms_message_tag_e_type stat707_lookup
( 
 byte * stat_val, /* pointer to the stat value */
 int  * all_flag  /* Ptr for "ALL" FLAG      */
)
{
  wms_message_tag_e_type tag;
  byte tag_read[] = "\"REC READ\"";
  byte tag_unread[] = "\"REC UNREAD\"";
  byte tag_sent[] = "\"STO SENT\"";
  byte tag_unsent[] = "\"STO UNSENT\"";
  byte tag_all[] = "\"ALL\"";
  byte s[32] = {0};

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

  return tag;
}/* stat707_lookup */

#ifdef FEATURE_DSAT_EXTENDED_SMS
/*===========================================================================

FUNCTION DSAT707SMS_EXEC_HSMSSS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^HSMSSS command.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:.
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None.
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat707sms_exec_hsmsss_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  if (tok_ptr->op == (NA) ) 
  {
    result = DSAT_OK;
  }
  /*--------------------------------------
   Processing for a TEST, Write and Read commands :
  ----------------------------------------*/
  else 
  {
    result = dsatparm_exec_param_cmd ( mode,
                                       tab_entry,
                                       tok_ptr,
                                       res_buff_ptr
                                       );
  }

  return result;
} /* dsat707sms_exec_hsmsss_cmd */

/*===========================================================================

FUNCTION STAT707_LOOKUP_EXT

DESCRIPTION
  Converts a string parameter to a TAG type ( wms_message_tage_e_type ).
  It also checks to see if the "ALL" wild card is being used as a stat.
  If so then it sets the pointer to the flag

DEPENDENCIES
  None

RETURN VALUE
  Returns an wms_message_tage_e_type enum.

SIDE EFFECTS
  None
===========================================================================*/

LOCAL wms_message_tag_e_type stat707_lookup_ext
( 
  byte *stat_val, /* stat value */
  int  * all_flag  /* Ptr for "ALL",4 FLAG      */
)
{
  wms_message_tag_e_type tag = WMS_TAG_NONE;
  dsat_num_item_type stat_v;

  if (dsatutil_atoi(&stat_v, stat_val, 10) != ATOI_OK)
  {
    return WMS_TAG_NONE;
  }

  switch ( stat_v )
  {
    case 0:
      tag = WMS_TAG_MT_NOT_READ;
      break;
    case 1:
      tag = WMS_TAG_MT_READ;
      break;
    case 2:
      tag = WMS_TAG_MO_NOT_SENT;
      break;
    case 3:
      tag = WMS_TAG_MO_SENT;
      break;
    case 4:
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
  }

  return tag;
}/* stat707_lookup_ext */

/*===========================================================================

FUNCTION CONVERT_ENCODING_EXT

DESCRIPTION
  Function converts ecoding argument to WMS encoding type.

DEPENDENCIES
  None

RETURN VALUE
  Returns an wms_message_tage_e_type enum.

SIDE EFFECTS
  None
===========================================================================*/

LOCAL wms_user_data_encoding_e_type convert_encoding_ext
( 
  dsat_num_item_type encod_arg
)
{
  switch (encod_arg) 
  {
  case 0:
    return WMS_ENCODING_GSM_7_BIT_DEFAULT;
  case 1:
    return WMS_ENCODING_ASCII;
  case 2:
    return WMS_ENCODING_IA5;
  case 3:
    return WMS_ENCODING_OCTET;
  case 4:
    return WMS_ENCODING_LATIN;
  case 5:
    return WMS_ENCODING_LATIN_HEBREW;
  case 6:
    return WMS_ENCODING_UNICODE;
  }
  /* default : ASCII */
  return WMS_ENCODING_ASCII;
}/* convert_encoding_ext */

#endif /* FEATURE_DSAT_EXTENDED_SMS */
#endif /* FEATURE_CDMA_SMS */
