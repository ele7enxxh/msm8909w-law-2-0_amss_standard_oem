
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( S M S  C O M M A N D S 
                  U T I L I T Y )
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module is used by SMS AT commands. It mainly contains utility functions
  that are used in executing the sms commands. These functions are used 
  intenally in SMS module.

EXTERNALIZED FUNCTIONS INTERNAL TO ATCOP SMS UNIT
  dsatsmsi_delete_messages
    This function is used for +CMGD/$QCMGD command when a <delflag> is 
    present.
    Tries to get the list of message with that particular <delflag> in 
    memory.

  dsatsmsi_delete_msg_list
    Local function that is used to delete a message from the list of indices.
    Handles one list or lists of multiple <stat>s.

  dsatsmsi_get_mem_val
    Returns the current sms memory storage setting.
    It returns dsat_cpms_val[mem_index]/
             dsat707sms_qcpms_val[mem_index]
             depending on modes.

  dsatsmsi_get_mem_list
    Returns the current sms memory storage setting.
    It returns dsat_cpms_list[mem_index]/
               dsat707sms_qcpms_list[mem_index]
               depending on modes.

  dsatsmsi_tag_lookup
    Converts the message TAG type (wms_message_tag_e_type) to a string on
    text mode or to an integer on a pdu mode and writes the result into the
    result buffer

  dsatsmsi_str_to_addr
    Checks to see if <toda> field is specified by the user. If not, parses the 
    <da> string for address length, type of number and numbering plan.If <toda> 
    filed is specified, <toda> is split into type of number and numberign plan.

  dsatsmsi_cms_error
    Fills a response buffer with a +CMS ERROR.

  dsatsmsi_strip_quotes
    Strips out quotes from the string that is wrapped in quotes.
    Resultant string will be placed in the out pointer.
    Gives an error if the string does not begin or end with a quote

  dsatsmsi_itoa
    This function converts an integer to ASCII, using a specified
    radix. The resulting string is null terminated.

  dsatsmsi_srv_to_domain_pref
  Maps tnullhe +CGSMS <service> number to the wms_gw_domain_pref 
  enum type.


INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2002 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatsmsu.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/17/15   skc     Fixed KW Issues
04/15/15   sc      Fixed issue in event handling to use subs_id provided in 
                   event info.
06/27/14   tk      Added support for Dynamic ATCoP.
03/11/14   tk      Optimized debug macros usage in ATCoP.
02/01/14   pg      Fixed +CSCA issue when Extended SMS feature defined
03/28/13   tk      Mainlined FEATURE_THIN_UI using new NV-EFS item.
01/10/13   tk      Fixed +CMGL and +CMGD issues with MO SENT messages.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/22/12   sk      Mainlining Nikel Changes.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   nc      Fixed operating mode checks for SMS related commands for NikeL targets. 
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
01/19/12   sk      Feature cleanup.
09/27/11   ht      Included dsut header to support offtarget stubs.
07/03/11   ad      Fixed memory corruption. .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
05/10/10   kk      Fix to hold back SMS notifications based on CNMI MT value.
12/15/09   nc      Featurisation changes for LTE.
09/09/09   bs      Fixed Klocwork criticals.
01/27/09   nc      Added support for *,# in <oa>/<da> for 
                   QCMGR and QCMGS.
12/12/08   ua      Fixed Off target lint errors.
11/13/08   nc      Added Support for QCMGW,QCMSS,QCMGL,QCMGF Commands
03/24/08   bs      Fixed Klocwork criticals.
03/03/08   bs      Fixing CSCA regression issues.
11/24/07   ua      Fixed Lint-lows. 
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
09/24/07   sa      Use DSM_SMALL_ITEMS in dsatsmsi_send_async_indication_to_te().
05/02/07   ua      Fixed lint medium errors. 
02/14/07   pkp     Fixed Klocwork (NULL dereference) errors.
02/13/07   pkp     Fixed RVCT compiler warnings.
02/05/07   ua      Corrected CPMS values to be in sync with NV item at powerup
01/30/07   ss      Replaced banned string API calls.
11/30/06   ua      Correcting route configurations for CNMI and +CNMA
09/26/06   snb     Bt default don't store MT Class 0 SMS.
03/13/06   snb     Correction to feature wrapping.
10/02/05   snb     Changes to implement a second layer of flow control, 
                   ATCOP to USB, to avoid exhaustion of DSM large items.
06/21/05   sb      Bug fixes from direct routing of SMS STATUS-REPORT testing
05/13/05   sb      Enable direct routing of SMS STATUS-REPORT
04/05/05   sb      Check if BTAG port is enabled before sending the SMS ind.
02/10/05   sb      Accessories related changes.
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "msg.h"
#include "err.h"
#include "wms.h"
#include "dsatsmsi.h"
#include "dsm.h"

#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#ifdef FEATURE_ETSI_SMS
#include "dsatetsictab.h"
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#endif /* FEATURE_CDMA_SMS */

#define VALID_CPMS(x) if((x <= CPMS_NONE)||(x >= CPMS_MAX))\
{\
  DS_AT_MSG0_ERROR("Invalid CPMS memory type");\
  return 0;\
}

/*===========================================================================
            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE
  This section contains reginal definitions for external variables 
  that are declared else where in ATCOP SMS SUB SYSTEM
===========================================================================*/
/*--------------------------------------------------------------------------
  import variable from other files in sms module
---------------------------------------------------------------------------*/

/* Variable to find out what command is being processed currently */
extern dsat_sms_cmd_s_type  dsatsmsi_pres_cmd;

/* WMS client (ATCOP) ID */
extern wms_client_id_type dsatsmsi_client_id;

/* Variable used in CMGL SM */
extern sms_cmd_sm_type  dsatsmsi_cmd_sm;

#ifdef FEATURE_CDMA_SMS
/* variable used in +CMTI / QCMTI generation */
extern boolean dsat_use_qcnmi;
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_ETSI_SMS_PS
typedef struct {
  wms_gw_domain_pref_e_type     pref;
  dsat_num_item_type            srv;
} domain_pref_tab_type;

extern const domain_pref_tab_type domain_pref_table[];

extern const int domain_pref_table_size;
#endif /* FEATURE_ETSI_SMS_PS */

LOCAL dsat_result_enum_type etsismsi_reenter_read_msg_list
(
  ds_cmd_type* cmd_ptr
);


LOCAL byte * itoa_fill
(
  word v,       /* Number to convert   */
  byte *rb_ptr  /* Destination Pointer */ 
);

#ifdef FEATURE_ETSI_SMS
extern const smsu_wms_mem_table_type wms_memory_gw_table[];
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_CDMA_SMS
extern const smsu_wms_mem_table_type wms_memory_is707_table[];
#endif /* FEATURE_CDMA_SMS */

/*-------------------------------------------------------------------------
            Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION    DSATSMSU_GET_MEM_STORE_STRING

DESCRIPTION
  This function returns TRUE if it finds mem store string corresponding to
  mem_store provided.
DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if string corresponding to mem_store provided is found.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
LOCAL boolean dsatsmsu_get_mem_store_string
(
 wms_memory_store_e_type       mem_store,  /* Type of Memory Store  */
 dsati_operating_cmd_mode_type cmd_mode,
 char                          *mem_str,
 size_t                         mem_str_size
)
{
  uint8 i = 0;
  const smsu_wms_mem_table_type *wms_memory_table = NULL;

#ifdef FEATURE_CDMA_SMS
  if (cmd_mode == CDMA_CMD_MODE)
  {
    wms_memory_table = wms_memory_is707_table;
  }
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_ETSI_SMS
  if (cmd_mode == ETSI_CMD_MODE)
  {
    wms_memory_table = wms_memory_gw_table;
  }
#endif /* FEATURE_ETSI_SMS */

  if (wms_memory_table != NULL)
  {
    for (i = 0; i < WMS_MEM_TABLE_SIZE; i++)
    {
      if(wms_memory_table[i].wms_memory == mem_store)
      {
        break;
      }
    }
    
    if (i != WMS_MEM_TABLE_SIZE)
    {
      dsatutil_memscpy(mem_str, mem_str_size,
                       wms_memory_table[i].mem_str,
                       sizeof(wms_memory_table[i].mem_str));
      return TRUE;
    }
  }
  
  return FALSE;
}/* dsatsmsu_get_mem_store_string */

/*===========================================================================
FUNCTION    DSATSMSU_SEND_ASYNC_INDICATION_TO_TE

DESCRIPTION
  If ATCOP get an unsolicited asynchronous event, This function formats 
  the data that needs to be sent to TE. It handles both SMS-DELIVERs and
  SMS-STATUS-REPORTs.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns number of bytes that are used in formatting the result buffer

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
void dsatsmsu_send_async_indication_to_te
(
 sms_mt_msg_e_type       msg_type,   /* Type of message       */
 wms_message_index_type  msg_index,  /* Index Number          */
 wms_memory_store_e_type mem_store,  /* Type of Memory Store  */
 dsati_operating_cmd_mode_type cmd_mode,
 sys_modem_as_id_e_type        subs_id
)
{
  char *data_ptr;
  dsm_item_type * res_buff;
  dsm_item_type * formatted_res_buff;
  char           mem_str[3];

  res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  if (NULL == res_buff)
  {
    return;
  }

  memset(mem_str, 0x00, sizeof(mem_str));

  data_ptr = (char *)res_buff->data_ptr;

  if ( msg_type == MT_DELIVER )
  {
    if ( cmd_mode == CDMA_CMD_MODE )
    {
#ifdef FEATURE_DSAT_EXTENDED_SMS
       if ( ((dsat_num_item_type)dsatutil_get_val(
              DSAT707_SMS_QCNMI_IDX,0,CNMI_MODE_VAL,NUM_TYPE) != 0) && 
            (((dsat_num_item_type)dsatutil_get_val(
              DSAT707_SMS_QCNMI_IDX,0,CNMI_MT_VAL,NUM_TYPE) == 1) )) 
       {
         res_buff->used += (word)snprintf( (char*)data_ptr,
                                                 res_buff->size - res_buff->used,
                                                 "+CMTI: ");
       }
       else
       {
         dsm_free_packet(&res_buff);
         return;
       }
#else
       res_buff->used += (word)snprintf((char*)data_ptr,
                                              res_buff->size - res_buff->used,
                                              "$QCMTI: ");
      
#endif /* FEATURE_DSAT_EXTENDED_SMS */

    }
    else
    {
     res_buff->used += (word)snprintf((char*)data_ptr,
                                             res_buff->size - res_buff->used,
                                             "+CMTI: ");
     }
  }
  else
  {
    res_buff->used += (word)snprintf((char*)data_ptr,
                                           res_buff->size - res_buff->used,
                                           "+CDSI: ");
  }
  data_ptr = (char *) res_buff->data_ptr + res_buff->used;

#ifndef FEATURE_DSAT_EXTENDED_SMS
  if ( msg_type == MT_STATUS_REPORT )
  {
    res_buff->used += (word) snprintf ((char*)data_ptr,
                                             res_buff->size -  res_buff->used,
                                             "\"SR\"");
  }
  else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  {
    if ( cmd_mode == CDMA_CMD_MODE )
    {
#ifdef FEATURE_DSAT_EXTENDED_SMS
      if (mem_store != WMS_MEMORY_STORE_NONE)  /* Update PRL download mode */
      {
        res_buff->used += (word) snprintf(data_ptr,
                                                res_buff->size - res_buff->used  ,"\"%s\"",
                                                (mem_store == WMS_MEMORY_STORE_RUIM) ? "SM" : "ME");
      }
      else
#endif /* FEATURE_DSAT_EXTENDED_SMS */
      {
        if (TRUE == dsatsmsu_get_mem_store_string(mem_store, cmd_mode, mem_str,sizeof(mem_str)))
        {
          res_buff->used += (word) snprintf(data_ptr,
                                            res_buff->size - res_buff->used  ,"\"%s\"",
                                            mem_str);
        }
      }
    }
    else
    {
        if (TRUE == dsatsmsu_get_mem_store_string(mem_store, cmd_mode, mem_str,sizeof(mem_str)))
      {
        res_buff->used += (word) snprintf(data_ptr,
                                          res_buff->size - res_buff->used  ,"\"%s\"",
                                          mem_str);
      }
    }
  }
  data_ptr = (char *)res_buff->data_ptr + res_buff->used;

  *data_ptr++ = ',';
  res_buff->used++;

  res_buff->used += (word)snprintf(data_ptr,
                                         res_buff->size - res_buff->used,
                                         "%d",(int)msg_index);
  data_ptr = (char *) res_buff->data_ptr + res_buff->used;

  *data_ptr = '\0';
  
  /* We are done with filling up raw response buffer */
  /* Format it and send it to TE                     */
  formatted_res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  /* send the response to the user */
  dsat_fmt_response( &res_buff, DSAT_COMPLETE_RSP, FALSE, 
                     &formatted_res_buff );

  if (NULL != formatted_res_buff)
  {
    /* Flush the queue to the Rm interface. */
    dsatcmdp_send_queued_cmd_results( );

    /* Check the formatted response data item. If it is not empty, send it
       to the Rm interface. */
    if ( formatted_res_buff->used != 0 )
    {

      /* Tell SIOLIB that this is SMS and should be sent to Modem port */
      ds3g_siolib_set_tx_type 
        ( DS3G_SIOLIB_UNSOLICTED_SMS, DS3G_SIOLIB_DATA_PORT );

      dsatrsp_send_response( formatted_res_buff, TRUE );


    }
    else
    {
      (void) dsm_free_buffer( formatted_res_buff );
    }
  }

  return;
} /* dsatsmsu_send_async_indication_to_te */


/*===========================================================================

FUNCTION    DSATSMSI_DELETE_MESSAGES

DESCRIPTION
  This function is used for +CMGD/$QCMGD command when a <delflag> is present.
  Tries to get the list of message with that particular <delflag> in memory.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatsmsi_delete_messages
(
  const byte * del_flag                  /* pointer to the delete flag */
)
{
  wms_memory_store_e_type mem_store;
  dsat_result_enum_type result;
  wms_status_e_type msg_status;
  dsati_operating_cmd_mode_type cmd_mode;

    if(dsatcmdp_is_cdma_cmd())
    {
      cmd_mode = CDMA_CMD_MODE;
    }
    else if(dsatcmdp_is_etsi_cmd())
    {
      cmd_mode = ETSI_CMD_MODE;
    }
    else
    {
      return DSAT_ERROR;
    }

  switch (*del_flag)
  {
    case '1':
      dsatsmsi_cmd_sm.num_of_stat = 1;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      break;
    case '2':
      dsatsmsi_cmd_sm.num_of_stat = 5;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MO_SENT;
      dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_SENT_ST_NOT_RECEIVED;
      dsatsmsi_cmd_sm.stat_list[3] = WMS_TAG_MO_SENT_ST_NOT_STORED;
      dsatsmsi_cmd_sm.stat_list[4] = WMS_TAG_MO_SENT_ST_STORED;
      break;
    case '3':
      dsatsmsi_cmd_sm.num_of_stat = 6;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MO_SENT;
      dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_NOT_SENT;
      dsatsmsi_cmd_sm.stat_list[3] = WMS_TAG_MO_SENT_ST_NOT_RECEIVED;
      dsatsmsi_cmd_sm.stat_list[4] = WMS_TAG_MO_SENT_ST_NOT_STORED;
      dsatsmsi_cmd_sm.stat_list[5] = WMS_TAG_MO_SENT_ST_STORED;
      break;
    case '4':
      dsatsmsi_cmd_sm.num_of_stat = 7;
      dsatsmsi_cmd_sm.present_stat = 1;
      dsatsmsi_cmd_sm.stat_list[0] = WMS_TAG_MT_READ;
      dsatsmsi_cmd_sm.stat_list[1] = WMS_TAG_MO_SENT;
      dsatsmsi_cmd_sm.stat_list[2] = WMS_TAG_MO_NOT_SENT;
      dsatsmsi_cmd_sm.stat_list[3] = WMS_TAG_MT_NOT_READ;
      dsatsmsi_cmd_sm.stat_list[4] = WMS_TAG_MO_SENT_ST_NOT_RECEIVED;
      dsatsmsi_cmd_sm.stat_list[5] = WMS_TAG_MO_SENT_ST_NOT_STORED;
      dsatsmsi_cmd_sm.stat_list[6] = WMS_TAG_MO_SENT_ST_STORED;
      break;
    default:
      return DSAT_ERROR;
  }
  
  /* We have to get a list */
  /* Seperate it from just one index */
  dsatsmsi_cmd_sm.one_index = 0;
  dsatsmsi_cmd_sm.is_test =  0;

  mem_store = dsatsmsi_memory_lookup((int)dsatsmsi_get_mem_val(CPMS_MEM1,dsat_get_current_subs_id(FALSE),cmd_mode), 
                                     CPMS_MEM1,cmd_mode);
  /* Get the message list first and then delete from the list */
#ifdef FEATURE_DUAL_SIM
  msg_status = wms_cfg_ms_get_message_list(
                       dsatsmsi_client_id,
                       dsat_get_current_subs_id(FALSE),
                       dsatsmsi_cmd_cb_func,
                       (void *)&dsatsmsi_pres_cmd,
                       mem_store,
                       WMS_TAG_MT_READ
                       );
#else
  msg_status = wms_cfg_get_message_list(
                       dsatsmsi_client_id,
                       dsatsmsi_cmd_cb_func,
                       (void *)&dsatsmsi_pres_cmd,
                       mem_store,
                       WMS_TAG_MT_READ
                       );
#endif /* FEATURE_DUAL_SIM */
  if ( msg_status == WMS_OK_S )
  {
    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result ;
} /* dsatsmsi_delete_messages */


/*===========================================================================

FUNCTION DSATSMSI_DELETE_MSG_LIST

DESCRIPTION
  Local function that is used to delete a message from the list of indices.
  Handles one list or lists of multiple <stat>s.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatsmsi_delete_msg_list
(
  sys_modem_as_id_e_type subs_id
)
{
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  dsat_result_enum_type result;
  dsati_operating_cmd_mode_type cmd_mode;

    if(dsatcmdp_is_cdma_cmd())
    {
      cmd_mode = CDMA_CMD_MODE;
    }
    else if(dsatcmdp_is_etsi_cmd())
    {
      cmd_mode = ETSI_CMD_MODE;
    }
    else
    {
      return DSAT_ASYNC_EVENT;
    }

  mem_store = dsatsmsi_memory_lookup((int)dsatsmsi_get_mem_val(CPMS_MEM1,subs_id,cmd_mode), 
                                      CPMS_MEM1,cmd_mode);
  if ( dsatsmsi_cmd_sm.num_of_indices != 0 )
  {
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_delete(
                 dsatsmsi_client_id,
                 subs_id,
                 dsatsmsi_cmd_cb_func,
                 (void*)&dsatsmsi_pres_cmd,
                 mem_store,
                 dsatsmsi_cmd_sm.indices[0]
                );
#else
    msg_status = wms_msg_delete(
                 dsatsmsi_client_id,
                 dsatsmsi_cmd_cb_func,
                 (void*)&dsatsmsi_pres_cmd,
                 mem_store,
                 dsatsmsi_cmd_sm.indices[0]
                );
#endif /* FEATURE_DUAL_SIM */
    /* Always read from the tip */
    dsatsmsi_cmd_sm.num_of_indices--;
    dsatsmsi_cmd_sm.present_index = 0;
    if (msg_status == WMS_OK_S)
    {
      /* still more to come */
      /* tell ATCOP about that */
      result = DSAT_ASYNC_CMD;
    }
    else 
    {
      /* we got an error      */
      /* return with an error */ 
      result = DSAT_CMD_ERR_RSP;
    }
  }
  else if ( dsatsmsi_cmd_sm.num_of_stat > 1 )
  {
    if(dsatsmsi_cmd_sm.present_stat < dsatsmsi_cmd_sm.num_of_stat)
    {
      tag = dsatsmsi_cmd_sm.stat_list[dsatsmsi_cmd_sm.present_stat];
      dsatsmsi_cmd_sm.present_stat++;
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
      result = DSAT_ASYNC_CMD;
    }
    else
    {
      /* We are finally done */
      /* tell ATCOP we are done */
      result = DSAT_OK;
    }
  }
  else 
  {
    /* this is a case of just single index delete or */
    /* we are done deleting messages                 */
    result = DSAT_OK;
  }

  return result;

}/* dsatsmsi_delete_msg_list */


/*===========================================================================

FUNCTION    DSATSMSI_GET_MEM_VAL

DESCRIPTION
  Returns the current sms memory storage setting.
  It returns dsat_cpms_val[mem_index]/
             dsat707sms_qcpms_val[mem_index]
  depending on modes.

DEPENDENCIES
  None

RETURN VALUE
  Returns a dsat_num_item_type.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatsmsi_get_mem_val
(
 cpms_mem_e_type               mem_index,   /* Memory (mem[1-3]) setting we are looking for */
 sys_modem_as_id_e_type        subs_id,
 dsati_operating_cmd_mode_type cmd_mode
)
{
  dsat_num_item_type            mem_val;

  VALID_CPMS(mem_index);


#ifdef FEATURE_ETSI_SMS 
  if(cmd_mode == ETSI_CMD_MODE)
  {
    mem_val = (dsat_num_item_type)dsatutil_get_val(
                     DSATETSI_EXT_ACT_CPMS_ETSI_IDX,subs_id,mem_index,NUM_TYPE);
  }
  else 
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_CDMA_SMS 
  if (cmd_mode == CDMA_CMD_MODE)
  {
    mem_val = (int)dsatutil_get_val(
                     DSAT707_SMS_QCPMS_IDX,0,mem_index,NUM_TYPE);
  }
  else
#endif /* FEATURE_CDMA_SMS */
  {
    DS_AT_MSG1_ERROR("We are in wrong mode: %d",cmd_mode);
    return 0;
  }

  return mem_val;

} /* dsatsmsi_get_mem_val */


/*===========================================================================

FUNCTION    DSATSMSI_TAG_LOOKUP

DESCRIPTION
  Converts the message TAG type (wms_message_tag_e_type) to a string on
  text mode or to an integer on a pdu mode and writes the result into the
  result buffer
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of result buffer 

SIDE EFFECTS
  None

===========================================================================*/
byte * dsatsmsi_tag_lookup
( 
 wms_message_tag_e_type tag,   /* TAG enum             */
 boolean mode,                 /* mode: PDU or TEXT    */
 byte * res_ptr                /* ptr to result buffer */
)
{
  char tag_table[8][11] =
  {"REC UNREAD","REC READ","STO UNSENT","STO SENT",
   "STO SENT","STO SENT","STO SENT","ALL"};
  byte i = 0, j = 0;
  wms_message_tag_e_type tag_array[7] = 
  { 
    WMS_TAG_MT_NOT_READ, 
    WMS_TAG_MT_READ, 
    WMS_TAG_MO_NOT_SENT, 
    WMS_TAG_MO_SENT, 
    WMS_TAG_MO_SENT_ST_NOT_RECEIVED,
    WMS_TAG_MO_SENT_ST_NOT_STORED,
    WMS_TAG_MO_SENT_ST_STORED
  };

  while ( i < 7 )
  {
    if ( tag == tag_array[i] )
    {
#ifdef FEATURE_ETSI_SMS
      /* we don't support PDU mode in CDMA */
      if(mode)
      {
        /* mode TRUE: PDU mode */
        res_ptr = dsatutil_itoa(i, res_ptr, 10);
      }
      else
#endif /* FEATURE_ETSI_SMS */
      {
        /* mode FALSE: TEXT mode */
        *res_ptr++ = '"';
        while((j < ARR_SIZE(tag_table[i]))&&(tag_table[i][j] != '\0'))
        {
          *res_ptr++ =  tag_table[i][j++];
        }
        *res_ptr++ = '"';
      }

      return res_ptr;
    }
    else
    {
      i ++;
    }
  }

  return res_ptr;

}/* dsatsmsi_tag_lookup */


/*===========================================================================

FUNCTION    DSATSMSI_STR_TO_ADDR

DESCRIPTION
  Checks to see if <toda> field is specified by the user.
  If not, parses the <da> string for address length, type of number and 
  numbering plan.
  If <toda> filed is specified, <toda> is split into type of number
  and numberign plan.
  
DEPENDENCIES
  None

RETURN VALUE
  return a boolean to indicate success or failure in the operation.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatsmsi_str_to_addr
(
 wms_address_s_type      *addr,   /* Pointer to address structure to 
                                     be filled                             */
 byte                    * da,    /* Pointer to destination address        */
 const byte              * toda,  /* Pointer to type of address            */
 dsatsms_fmt_mode_e_type sms_mode /* SMS mode Enum                         */
)
{
  uint8 loop=0, toa=0, toda_type= 0;
  uint8 start_i = 0;
  byte * s;
  byte c;
  dsat_num_item_type type_of_address;
  boolean result = TRUE;
  dsati_mode_e_type op_mode = dsatcmdp_get_current_mode();

  /*check to see if optional <da> and <toda> are supplied by the user*/
  /* new <da>: no type of address */
  if ((toda == NULL) || (*toda == '\0'))
  {
    /* if <da> begins with '+' type-of-address is 145 */
    if ( *da == '+' )
    {
      addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
      addr->number_type = WMS_NUMBER_INTERNATIONAL;
      addr->number_plan = WMS_NUMBER_PLAN_TELEPHONY;
      da++;
#ifdef FEATURE_DSAT_EXTENDED_SMS
      addr->digits[0] = 0x0a;
      addr->digits[1] = 0x0a;
      start_i = 2;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    }
    else /* default type-of-address is 129 */
    {
      addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
      addr->number_type = WMS_NUMBER_UNKNOWN;
      addr->number_plan = WMS_NUMBER_PLAN_TELEPHONY;
    }
  }
  /* We got a type of address */
  else
  {
    if ( sms_mode == DSATSMS_TEXT_MODE )
    {
      /* It is a decimal number if we are in text mode */
      toda_type = 10;
    }
    else
    {
      /* It is a hex number if we are in pdu mode */
      toda_type = 16;
    }

    if( dsatutil_atoi(&type_of_address, toda, toda_type ) != ATOI_OK)
    {
      /* Bad type of address */
      return FALSE;
    }

    if( type_of_address > 255 )
    {
      /* Over flow */
      return FALSE;
    }

    toa = (uint8)type_of_address;
    addr->digit_mode = WMS_DIGIT_MODE_4_BIT;
    addr->number_type = (wms_number_type_e_type)((toa & 0x70)>>4);
    addr->number_plan = (wms_number_plan_e_type)(toa & 0x0F);

    /* Ignore the '+' */
    if ( *da == '+' )
    {
      da++;
      /* Needs to be International */
      addr->number_type = WMS_NUMBER_INTERNATIONAL;
#ifdef FEATURE_DSAT_EXTENDED_SMS
      addr->digits[0] = 0x0a;
      addr->digits[1] = 0x0a;
      start_i = 2;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

    }
  }
  
  s = da;
  /* Initializing the number mode*/
  addr->number_mode = WMS_NUMBER_MODE_DATA_NETWORK_MAX32;

  while((c = *s++) != '\0')
  { 
    dsat_dial_val_e_type vresult = dsatutil_validate_dial_char(&c);
    if (DSAT_DIAL_DIGIT == vresult)
    {
      c -= '0';
    }
    else if (DSAT_DIAL_PLUS == vresult)
    {
      return FALSE;
    }
    else
    {
      if ( sms_mode == DSATSMS_TEXT_MODE )
      {
        switch(vresult)
        {
          case DSAT_DIAL_ASCII_ABC:
            c -= 53;  /* 'A' - 12 */
            break;
          case DSAT_DIAL_STAR:
            c = 10;
            break;
          case DSAT_DIAL_POUND:
            c = 11;
            break;
          default:
            result = FALSE;
            break;
        }/* switch */
      }
      else /* SMS_PDU_MODE */
      {
        /* Mapping in PDU mode: */
        /*   'A' --> '*'        */
        /*   'B' --> '#'        */
        /*   'C' --> 'A'        */
        /*   'D' --> 'B'        */
        /*   'E' --> 'C'        */
        switch(vresult)
        {
          case DSAT_DIAL_ASCII_ABC:
          case DSAT_DIAL_ASCII_D:
          case DSAT_DIAL_ASCII_E:
            c -= 55;  /* 'A' - 10 */
            break;
          default:
            result = FALSE;
            break;
        }/* switch */
      }/* else */
    }
#ifdef FEATURE_ETSI_SMS 
    if( IS_ETSI_MODE(op_mode) )
    {
      addr->digits[loop+start_i] = c;
    }
#endif /* FEATURE_ETSI_SMS */
    loop++;
    if(loop > 20)
    {
      return FALSE;
    }
  }/* while "" */
  
#ifdef FEATURE_CDMA_SMS
  if(dsatcmdp_is_cdma_cmd())
  {
    da[loop] = '\0';
#ifdef FEATURE_DSAT_EXTENDED_SMS
    if(wms_ts_ascii2dtmf((const char *) da,(uint8 *) &addr->digits[start_i])== 0)
#else
    if(wms_ts_ascii2dtmf((const char *) da,(uint8 *) &addr->digits[0])== 0)
#endif /* FEATURE_DSAT_EXTENDED_SMS */
    {
      DS_AT_MSG0_ERROR("Unsupported Address character");
      return FALSE;
    }
  }
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_DSAT_EXTENDED_SMS
  addr->number_of_digits = loop + start_i;
#else
  addr->number_of_digits = loop;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  if(!addr->number_of_digits)
  {
    DS_AT_MSG0_HIGH("No Number available");
    return FALSE;
  }
  
  return result;
}/* dsatsmsi_str_to_addr */


/*===========================================================================

FUNCTION    DSATSMSI_CMS_ERROR

DESCRIPTION
  Fills a response buffer with a +CMS ERROR.
  
DEPENDENCIES
  None

RETURN VALUE
  returns number of bytes that are used in filling up the response buffer.

SIDE EFFECTS
  None

===========================================================================*/
unsigned int dsatsmsi_cms_error
(
  const dsm_item_type *res_buff_ptr,             /*  Place to put response    */
  int                  error                     /*  Error Number             */
)
{
  byte * w_ptr;
  uint16 buff_len = 0;
  
  w_ptr = res_buff_ptr->data_ptr + res_buff_ptr->used;
  buff_len = res_buff_ptr->size - res_buff_ptr->used;


  w_ptr += (word)snprintf((char*)w_ptr, buff_len, "+CMS ERROR: ");

  w_ptr = dsatutil_itoa((uint32)error, w_ptr, 10);

  return (unsigned int) (w_ptr - (res_buff_ptr->data_ptr + res_buff_ptr->used));
}/* dsatsmsi_cms_error */


/*===========================================================================

FUNCTION    DSATSMSI_STRIP_QUOTES

DESCRIPTION
  Strips out quotes from the string that is wrapped in quotes.
  Resultant string will be placed in the out pointer.
  Gives an error if the string does not begin or end with a quote
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean : 
    TRUE : if successful in stripping the quotes out
    FALSE : if there is any error

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatsmsi_strip_quotes
(
  const byte * in_ptr,   /* Pointer to the string to be processed  */
  byte       * out_ptr,  /* Pointer to the resultant string buffer */
  int          len       /* Length of destination */   
) 
{
  /* Variable to trace number of characters copied */
  int i = 1;
  if(*in_ptr != '\0' )
  {
    if( *in_ptr++ == '"' )
    {
      while ( *in_ptr != '"' && *in_ptr != '\0' )
      {
        if(i < len)
        {            
          *out_ptr++ = *in_ptr++;
        }
	else
	{
          DS_AT_MSG2_HIGH("dsatsmsi_strip_quotes(): dst size = %d i = %d", len, i);
	  return FALSE;
	}
	i++;
      }
      /* Check to see if the strring ends with a null */
      if( *in_ptr == '\0' )
      {
        /* We got a string without ending quotes */
        return FALSE;
      }
      else
      {
        if(i <= len )
        {
          /* Everything is happy */
          *out_ptr = '\0';
          return TRUE;
        }
	else
        {
          return FALSE;
        }
      }
    }
    else
    {
      /* We got a string with out quotes */
      return FALSE;
    }    
  }
  else
  {
    return FALSE;
  }
} /* dsatsmsi_strip_quotes */

/*===========================================================================

FUNCTION DSATSMSI_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFF and a radix of 2,
  which returns 17 bytes (16 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
byte * dsatsmsi_itoa
(
  uint32 v,       /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  word r          /*  Conversion radix              */
)
{
  byte buf[17], c;
  int n;

  n = sizeof(buf) - 1;

  buf[n] = '\0';


  do
  {
    c = (byte) (v % r);

    if (n <= 0)
    {
      DS_AT_MSG0_HIGH("OVERFLOW");
      break;
    }

    buf[--n] = (byte) ( (c > 9) ? c + 'A' - 10 : c + '0');

  } while ( (v /= r) > 0);

  while ( (*s++ = buf[n++]) != 0)
      ;

  return (s-1);
} /*  dsatsmsi_itoa */


#ifdef FEATURE_ETSI_SMS_PS

/*===========================================================================

FUNCTION    DSATSMSI_SRV_TO_DOMAIN_PREF

DESCRIPTION
  Maps tnullhe +CGSMS <service> number to the wms_gw_domain_pref enum type.
  
DEPENDENCIES
  None

RETURN VALUE
  TRUE : succeeds
  FALSE: fails

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatsmsi_srv_to_domain_pref
( 
 dsat_num_item_type             srv,       /* pointer to +CGSMS <service> */
 wms_gw_domain_pref_e_type      * pref_ptr   /* wms domain pref enum        */
)
{
  int index;

  for(index = 0; index < domain_pref_table_size; index++)
  {
    if(domain_pref_table[index].srv == srv)
    {
      *pref_ptr = domain_pref_table[index].pref;
      return TRUE;
    }
  }

  return FALSE;
} /* dsatsmsi_srv_to_domain_pref */
#endif /* FEATURE_ETSI_SMS_PS */

/*===========================================================================

FUNCTION DSATSMSI_READ_MSG_LIST

DESCRIPTION
  Local function that is used to read a message from the list of indices.
  Handles one list or lists of multiple <stat>s

DEPENDENCIES
  none

RETURN VALUE
  TRUE if the request to wms to read the msg is accepted
       or if the function quits because of ds to sio watermake high.
  FALSE if the request to wms to read the msg is rejected


SIDE EFFECTS
  None
===========================================================================*/
boolean dsatsmsi_read_msg_list
( 
 sys_modem_as_id_e_type subs_id 
)
{
  wms_memory_store_e_type mem_store;
  wms_status_e_type msg_status;
  boolean result;
  wms_message_index_type index;
  const dsat_string_item_type * mem_valstr;
  const def_list_type * list_ptr;
  dsat_num_item_type mem_val;
  dsati_operating_cmd_mode_type cmd_mode;
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
#endif /* FEATURE_DSAT_ETSI_MODE */

  if(dsatcmdp_is_cdma_cmd())
  {
    cmd_mode = CDMA_CMD_MODE;
  }
  else if(dsatcmdp_is_etsi_cmd())
  {
    cmd_mode = ETSI_CMD_MODE;
  }
  else
  {
    return FALSE;
  }
  mem_val = dsatsmsi_get_mem_val(CPMS_MEM1,subs_id,cmd_mode);
  mem_store = dsatsmsi_memory_lookup((int)mem_val,
                                     CPMS_MEM1,cmd_mode);
  index = dsatsmsi_cmd_sm.indices[dsatsmsi_cmd_sm.present_index];

  if(dsati_tx_wm_high)
  {
    dsati_to_sio_low_handler = etsismsi_reenter_read_msg_list;
    return TRUE;
  }

#ifdef FEATURE_DSAT_ETSI_MODE
  if ( IS_ETSI_MODE(current_mode) )
  {
    /* If DSM large item free count has gone below a threshold quantity... */
    if ( DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL) < DSAT_FREE_ITEM_TRIGGER )
    {
      dsat_timer_s_type *t_ptr = &dsatutil_timer_table[DSAT_TIMER_FREE_ITEM];

      /* Set timer expiry handler function */
      dsati_handle_free_item_timer_expiry = etsismsi_reenter_read_msg_list;

      /* Set timer and return TRUE */
      (void) rex_set_timer( t_ptr->timer_ptr, t_ptr->duration );
      t_ptr->is_running = TRUE;
      return TRUE;
    }
  }
#endif /* FEATURE_DSAT_ETSI_MODE */
  list_ptr = dsatsmsi_get_mem_list(CPMS_MEM1,cmd_mode);
  if(list_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("SMS memory settings unavailable");
    return FALSE;
  }
  mem_valstr = list_ptr->list_v[mem_val];
  /* Check if the CMGL is for Status report */
  if ( strcmp ("SR",(char *)mem_valstr) == 0)
  {
#ifdef FEATURE_DUAL_SIM
    msg_status = wms_msg_ms_read_sts_report(
                                dsatsmsi_client_id,
                                subs_id,
                                dsatsmsi_cmd_cb_func,
                                (void*)&dsatsmsi_pres_cmd,
                                mem_store,
                                index
                                );
#else
    msg_status = wms_msg_read_sts_report(
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
  dsatsmsi_cmd_sm.present_index++;

  /* Check to see if we got an error */
  if (msg_status == WMS_OK_S)
  {
    result = TRUE;
  }
  else 
  {
    /* we got an error */
    result = FALSE;
  }

  return result;
}/* dsatetsismsi_read_msg_list */

/*===========================================================================

FUNCTION DSATSMSI_GET_MSG_LIST

DESCRIPTION
  Local function that is used to get a message list.
  Handles one list or lists of multiple <stat>s

DEPENDENCIES
  none

RETURN VALUE
  Boolean.
  TRUE: If the next message list was asked successfully
  FALSE: If there is an error or if we are done reading the lists

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatsmsi_get_msg_list
(
  sys_modem_as_id_e_type subs_id 
)
{
  wms_memory_store_e_type mem_store;
  wms_message_tag_e_type tag;
  wms_status_e_type msg_status;
  boolean result = FALSE;
  dsati_operating_cmd_mode_type cmd_mode;
  if(dsatcmdp_is_etsi_cmd())
  {
    cmd_mode = ETSI_CMD_MODE;
  }
  else if(dsatcmdp_is_cdma_cmd())
  {
    cmd_mode = CDMA_CMD_MODE;
  }
  else
  {
    return FALSE;
  }
  mem_store = dsatsmsi_memory_lookup((int)dsatsmsi_get_mem_val(CPMS_MEM1,subs_id,cmd_mode), 
                                     CPMS_MEM1,cmd_mode);

  if ( dsatsmsi_cmd_sm.num_of_stat > 1 )
  {
    if(dsatsmsi_cmd_sm.present_stat < dsatsmsi_cmd_sm.num_of_stat)
    {
      tag = dsatsmsi_cmd_sm.stat_list[dsatsmsi_cmd_sm.present_stat];
      dsatsmsi_cmd_sm.present_stat++;
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
      if( msg_status == WMS_OK_S )
      {
        result = TRUE;
      }
      else
      {
        result = FALSE;
      }
    }
    else
    {
      result = FALSE;
    }
  }

  return result;
}/* dsatsmsi_get_msg_list */


/*===========================================================================

FUNCTION ETSISMSI_REENTER_READ_MSG_LIST

DESCRIPTION
  Local function that is used to reenter reading a message from the list 
  of indices: it's called to reenter this processing following ATCOP to SIOLIB
  flow control or after waiting for DSM items to be freed by USB when listing
  many SMSes.

DEPENDENCIES
  none

RETURN VALUE
  DSAT_ASYNC_CMD: if the request to wms to read the msg is accepted
  DSAT_ERROR    : if the request to wms to read the msg is rejected

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsismsi_reenter_read_msg_list
(
  ds_cmd_type* cmd_ptr
)
{
  boolean read_msg_result;
  dsat_result_enum_type result;

  /* Reset handler function pointers */
  dsati_to_sio_low_handler = NULL;
  dsati_handle_free_item_timer_expiry = NULL;

  read_msg_result = dsatsmsi_read_msg_list(dsat_get_current_subs_id(FALSE));

  if(read_msg_result)
  {
    result = DSAT_ASYNC_CMD;
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* etsismsi_reenter_read_msg_list */


/*===========================================================================

FUNCTION    ITOA_FILL

DESCRIPTION
  Converts an integer to a string. With leading zeros. Assumes that it is a
  2 digit number.
  
DEPENDENCIES
  None

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL byte * itoa_fill
(
  word v,       /* Number to convert   */
  byte *rb_ptr  /* Destination Pointer */ 
)
{
  int n;
  byte c, *ptr;
  ASSERT(rb_ptr != NULL);
  ptr = rb_ptr + 2;
  *ptr = '\0';

  for (n = 0; n < 2; ++n)
  {
    c = (byte) (v % 10);
    v /= 10;
    *--ptr = (c + '0');
  }
  
  return rb_ptr + 2;
} /*  itoa_fill */


/*===========================================================================

FUNCTION    DSATSMSI_TIMESTAMP

DESCRIPTION
  Converts the message SC timestamp type (wms_timestamp_s_type) to a 
  string. It converts it into "yy/MM/dd,hh:mm:ss+/-zz" format.
  writes the result into the result buffer.
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of result buffer 

SIDE EFFECTS
  None
===========================================================================*/
byte * dsatsmsi_timestamp
( 
 wms_timestamp_s_type  time,  /* Pointer to Time Stamp        */
 byte                 *res_ptr, /* Pointer to the result buffer */
 boolean               is_hc_cmd /* If its for HC response */
)
{
  uint8 tmp;
  ASSERT( res_ptr != NULL );
  /* Convert BCDs to strings */
  if ( FALSE == is_hc_cmd )
  {
    *res_ptr++ = '"';
    tmp = time.year;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10), 
                         res_ptr ); 
    *res_ptr++ = '/';
    tmp = time.month;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr );
    *res_ptr++ = '/';
    tmp = time.day;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr);
    *res_ptr++ = ',';
    tmp = time.hour;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr);
    *res_ptr++ = ':';
    tmp = time.minute;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr);
    *res_ptr++ = ':';
    tmp = time.second;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr);
    if( time.timezone < 0 )
    {
      *res_ptr++ = '-';
      tmp = (uint8)(time.timezone * (-1));
    }
    else
    {
      *res_ptr++ = '+';
      tmp = (uint8)time.timezone;
    }
    res_ptr = itoa_fill( tmp, res_ptr);
    *res_ptr++ = '"';
  }
  else
  {
    *res_ptr++ = '2';
    *res_ptr++ = '0';

    tmp = time.year;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10), 
                         res_ptr ); 

    *res_ptr++ = ',';
    tmp = time.month;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr );

    *res_ptr++ = ',';
    tmp = time.day;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr);

    *res_ptr++ = ',';
    tmp = time.hour;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr);

    *res_ptr++ = ',';
    tmp = time.minute;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr);

    *res_ptr++ = ',';
    tmp = time.second;
    res_ptr = itoa_fill( (tmp & 0x0F) + ((tmp >> 4) * 10),
                         res_ptr);

  }
  return res_ptr;
}/* dsatsmsi_timestamp */

#ifdef FEATURE_DSAT_EXTENDED_SMS
/*===========================================================================

FUNCTION    DSATSMSI_TAG_LOOKUP_EXT

DESCRIPTION
  Converts the message TAG type (wms_message_tag_e_type) to a tag_id.
  
DEPENDENCIES
  None

RETURN VALUE
  returns a pointer to the end of result buffer 

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatsmsi_tag_lookup_ext
( 
  wms_message_tag_e_type tag   /* TAG enum             */
)
{
  switch (tag) {
    case WMS_TAG_MT_NOT_READ:
      return 0;
    case WMS_TAG_MT_READ:
      return 1;
    case WMS_TAG_MO_NOT_SENT:
      return 2; 
    case WMS_TAG_MO_SENT:
      return 3;
    default:
      return 0;
  }

  return 0;
}/* dsatsmsi_tag_lookup_ext */
#endif /* FEATURE_DSAT_EXTENDED_SMS */
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */
