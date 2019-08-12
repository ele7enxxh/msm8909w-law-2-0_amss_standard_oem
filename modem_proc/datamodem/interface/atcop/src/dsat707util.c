/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains generic function definitions used by the IS707
  AT command processor.
  
EXTERNALIZED FUNCTIONS
  dsat707_find_cmd_name
    This function matches the command name that gets passed in with the
    command table entry and returns the corresponding index.

  dsat707I_unrec_func_ptr
    This function processess unrecognized commands, and commands with
    parameter errors which will cause a connection to the base station
    if +CXT=1.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 1995-2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707util.c_v   1.11   18 Feb 2003 10:39:58   sramacha  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707util.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Added support for Dynamic ATCoP.
03/11/14   tk      Optimized debug macros usage in ATCoP.
05/18/12   tk      Migrated to MSG 2.0 macros
03/13/12   dvk     Merged enabling mdr val update always during powerup.
01/19/12   sk      Feature cleanup.
07/22/11   dvk     Global Variable Cleanup
03/30/11   ms      Global variable cleanup.
03/15/11   mg      Global variable cleanup.
01/11/11   ad      Fixed security vulnerabilities in 1X.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
05/10/10   kk      Added functions to read and parse MIN and IMSI NV items.
09/09/09   bs      Fixed Klocwork criticals.
07/01/09   vg      Replacing deprecated MSG_XXX macro with MSG_SPRINTFX_ macro.
06/15/09   vg	   Fixed Lint medium.
03/03/09   ms      Update the HAT timer value with the value read from 
                   NV or RUIM card entry.
03/04/09   sa      AU level CMI modifications.
12/12/08   ua      Fixed Off target lint errors.
09/02/08   mga     Removed return in some cases in dsat707_nv_sync() and changed
                   accordingly. Changed ERR_FATAL to MSG_ERROR in some cases.
08/11/08   bs      Fixed Klocwork criticals.
06/24/08   mga     Made changes to support CTA timer NV item
06/18/08   nc      Fixed the memory leak caused by item_ptr in 
                   dsat707_unrec_func_ptr().
06/10/08   mga     Made changes to support CTA timer NV item
04/29/08   mga     Added return in some cases after error checking
04/09/08   bs      Fixed Klocwork errors.
01/15/08   mga     Merged changes to change ERR_FATAL to MSG_ERROR
10/26/07   ua      Correcting sending of unrecognized commands to IWF only 
                   when +CXT value is set to 1. 
05/10/07   ac      Lint fix
04/11/07   ua      Modifications as per KDDI requirements. 
10/30/06   spn     Changed Featurization from DS_AT_TEST_ONLY to 
                   HDR_AT_TEST_ONLY
04/05/05   gr      Integrated EPZID functionality onto the main line
08/11/04   jd      Fix buffer overrun check in dsat707_strip_quotes
06/29/04   sy      Added bound check for the pzid hysteresis NV items.
05/16/04   vr      Moved uint32_to_str and str_to_uint32 from dsat707mip.c
                   Added mcast_str_to_uint32()
04/08/04   gr      Added support for force cdma dial str nv item
09/17/03   sy      Added support for nv items for pzid hysteresis under
                   FEATURE_DS_EPZID_HYSTERESIS.
01/28/03   rsl     Don't append 'T' to dial string because of ATCOP changes.
01/20/03   ar      Added dialstr_modifiers to ds707_async_dial_str_cb 
                   interface.
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
09/30/02   ar      Accomodate changed interface to proc_dial_string
07/13/02   atp     Changed signature of dsat707_send_config() to make param
                   passed in a ptr to const byte.
07/13/02   atp     In dsat707_send_config() functions, calls 
                   ds707_async_ps_tcp_enqueue() to queue data.
4/3/01     rsl     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "amssassert.h"
#include "dsati.h"
#include "dsat707extctab.h"
#include "dsat707faxctab.h"
#include "ps_in.h"
#ifdef FEATURE_HDR
#include "dsat707hdrctab.h"
#endif /* FEATURE_HDR */
#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif /* FEATURE_DS_MOBILE_IP */
#ifdef FEATURE_DS_PSTATS
#include "dsat707pstatsctab.h"
#endif /* FEATURE_DS_PSTATS */
#include "dsat707vendctab.h"
#include "dsat707util.h"
#include "dsatparm.h"
#include "msg.h"
#include "dstaski.h"
#include "ds707_async_mgr.h"
#include "dsatact.h"
#include "dsat707mip.h"
#include "dsat707mipctab.h"
#include "ds_1x_profile.h"
#include "err.h"

#include <stringl/stringl.h>

#include "ds707_jcdma_m51.h"
#include "ds707_epzid.h"
#ifdef FEATURE_DS_EPZID_HYSTERESIS
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_EPZID_HYSTERESIS */

#include "dsat707extctab.h"
#include "dsat707faxctab.h"

#include "ds707_roaming.h"

/*-------------------------------------------------------------------------
    Definition of strings used for verbose result codes:

    Assignment to the pointer arrays "xxx_codes" must match
    the definition of "ds_atcop_result_enum_type" in dsatcop.h
-------------------------------------------------------------------------*/
const byte res_code_string_ok []        = "OK";         /*  0  */
const byte res_code_string_con []       = "CONNECT";    /*  1  */
const byte res_code_string_ring []      = "RING";       /*  2  */
const byte res_code_string_no_car []    = "NO CARRIER"; /*  3  */
const byte res_code_string_error []     = "ERROR";      /*  4  */
const byte res_code_string_nu []        = "(not used)"; /*  5  */
const byte res_code_string_no_dial []   = "NO DIALTONE";/*  6  */
const byte res_code_string_busy []      = "BUSY";       /*  7  */
const byte res_code_string_no_ans []    = "NO ANSWER";  /*  8  */
const byte res_code_string_delayed []   = "DELAYED";    /*  9  */

const byte res_code_string_rng_amps  [] = "RING AMPS";  /* 10  */
const byte res_code_string_rng_async [] = "RING ASYNC"; /* 11  */
const byte res_code_string_rng_fax []   = "RING FAX";   /* 12  */
const byte res_code_string_rng_pkt   [] = "RING PACKET";/* 13  */

/*20*/
const byte res_code_string_no_amps[]    = "NO AMPS SERVICE";
/*21*/
const byte res_code_string_no_srv []    = "NO SERVICE";
/*22 */
const byte res_code_string_no_async []  = "NO ASYNC SERVICE";
/*23*/
const byte res_code_string_no_fax []    = "NO FAX SERVICE";
/*24*/
const byte res_code_string_no_pkt []    = "NO PACKET SERVICE";
/*25*/
const byte res_code_string_no_intr[]    = "BAD REQUEST";
/*26*/
const byte res_code_string_paged []     = "PAGED";
const byte res_code_string_reoder []    = "RETRY";      /* 27 */
const byte res_code_string_page_fail [] = "PAGE FAIL";  /* 28 */
const byte res_code_string_link_fail [] = "LINK FAIL";  /* 29 */

const byte res_code_string_released []  = "RELEASE";    /* 30 */

/*  Stand alone */
const byte res_code_string_cerror []    = "+CERROR: ";

const byte *const res_codes [] =
      { res_code_string_ok,
        res_code_string_con,
        res_code_string_ring,
        res_code_string_no_car,
        res_code_string_error,
        res_code_string_nu,
        res_code_string_no_dial,
        res_code_string_busy,
        res_code_string_no_ans,
        res_code_string_delayed
      };

const byte *const cell_res_codes [] =
      { res_code_string_rng_amps,
        res_code_string_rng_async,
        res_code_string_rng_fax,
        res_code_string_rng_pkt
      };

const byte *const cell_error_res_codes [] =
      { res_code_string_no_amps,
        res_code_string_no_srv,
        res_code_string_no_async,
        res_code_string_no_fax,
        res_code_string_no_pkt,
        res_code_string_no_intr,
        res_code_string_paged,
        res_code_string_reoder,
        res_code_string_page_fail,
        res_code_string_link_fail,
        res_code_string_released
      };

boolean unrec_cmd_rcvd = FALSE;

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_DATA_IS707_ASYNC
LOCAL void dsat707_send_config_action
(
  const dsati_cmd_type * table,
  const unsigned int sizeof_table,
  dsm_item_type **item_ptr_ptr
) ;


/*=========================================================================

FUNCTION DSAT707I_UNREC_FUNC_PTR

DESCRIPTION
  This function processess unrecognized commands, and commands with parameter
  errors which will cause a connection to the base station if +CXT=1.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsat707i_unrec_func_ptr
(
  byte* cmd_line_ptr
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type *revert_ptr = NULL;
  boolean digit_mode;
  byte c;
  dsm_item_type *item_ptr; /* Data item for configuration */
  dsat_num_item_type temp_val;

  /*-------------------------------------------------------------------------*/
  /* If cxt_val is 0 then no further processing is required */
  if( (dsat_num_item_type)dsatutil_get_val(
                           DSAT707_EXT_PARA_CXT_IDX,0,0,NUM_TYPE) != 1 )
  {
    DS_AT_MSG0_ERROR("dsat707_cxt_val is 0;Need no further processing");
    return DSAT_ERROR;
  }
  /* Unrecognized commands should be passed only when +CXT is set to 1
     for more information please check +CXT command */
  if ( (dsat_num_item_type)dsatutil_get_val(
                            DSAT707_EXT_PARA_CXT_IDX,0,0,NUM_TYPE) == 1)
  {
    unrec_cmd_rcvd = TRUE;
  }
  if (dsatcmdp_at_state == DSAT_CMD) 
  {
    dsat_dial_modifier_info_type  dialstr_modifiers;
    memset( (void*)&dialstr_modifiers, 0,sizeof(dialstr_modifiers) );

    item_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    /*-------------------------------------------------------------------------
      Get an item to hold command parameters that may need reversion,
      and copy the parameters.  Command mode only.
    -------------------------------------------------------------------------*/
    revert_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    (void) dsatutil_memscpy((void*)revert_ptr->data_ptr, revert_ptr->size,
                  (void*)dsatutil_get_val(DSAT707_FAX_FNS_IDX,0,0,STR_TYPE),
             dsat707_sizeof_fns_val);

    revert_ptr->used = (uint16)dsat707_sizeof_fns_val;

    /*-------------------------------------------------------------------------
      Unrecognized commands, and commands with parameter errors
      will cause a connection to the base station if +CXT=1.
      In this case, revert dsat707_fns_val to its value at the
      beginning of the command line to avoid concatenating the
      value twice (once here, and once when the command is reflected).
      The reversion is benign if the there was no AT+FNS command.
      
    ---------------------------------------------------------------------------*/

    digit_mode = 1;
    if (ds707_async_get_active_iface_ptr() == NULL) 
    {
      (void)ds707_async_dial_str_cb ( cmd_line_ptr,
                                digit_mode,&dialstr_modifiers,
                                dsat_get_current_1x_subs_id());
      result = DSAT_NO_RSP;
    }
    (void) dsatutil_memscpy((void*)dsatutil_get_val(DSAT707_FAX_FNS_IDX,0,0,STR_TYPE),
                            dsat707_sizeof_fns_val,
                            (void*)revert_ptr->data_ptr,
                            dsat707_sizeof_fns_val);

    item_ptr->data_ptr[item_ptr->used++] = 'A';
    item_ptr->data_ptr[item_ptr->used++] = 'T';
    while ( (c = *cmd_line_ptr++) != 0)
    {
      item_ptr->data_ptr[item_ptr->used++] = c;
    }
    temp_val = (dsat_num_item_type)dsatutil_get_val(
                                         DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
    item_ptr->data_ptr[item_ptr->used++] = (byte) temp_val;
    item_ptr->kind = DSM_PS_APP_PAYLOAD;
    ds707_async_ps_tcp_enqueue( &item_ptr);
    dsm_free_packet ( &revert_ptr );
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}

/*===========================================================================

FUNCTION DSAT707I_SEND_CONFIG

DESCRIPTION
  This function transmits the mobile configuration to the base station.
  Configuration parameters are those which have the DOWN_LOAD attribute
  in the parse table.  The only DOWN_LOAD paramters transmitted are those
  with the ALWAYS attribute in the parse table, and those which
  have been changed from their default values.  Parameters with
  the FCLASS2P0 attribute are not transmitted unless the current value
  of the FCLASS parameter is 2.0.

  Follow the configuration with the +CFG string (always, even if empty),
  and then the current command.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

void dsat707i_send_config
(
  const byte * cur_cmd
)
{
  byte c;
  dsm_item_type *item_ptr; /* Data item for configuration */
  int index;
  byte cfg [] = "+CFG";
  dsat_num_item_type temp_val;

  item_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
  if (NULL != item_ptr)
  {
    dsat707_send_config_action (dsat_sreg_table,
                                dsat_sreg_table_size,
                                &item_ptr);

    dsat707_send_config_action (dsat_basic_table,
                                dsat_basic_table_size,
                                &item_ptr);

    dsat707_send_config_action (dsat_basic_action_table,
                                dsat_basic_action_table_size, 
                                &item_ptr);

    dsat707_send_config_action (dsat_ext_table,
                                dsat_ext_table_size, 
                                &item_ptr);

    dsat707_send_config_action (dsat707_ext_para_table,
                                dsat707_ext_para_table_size, 
                                &item_ptr);

    if(ds707_roaming_is_curr_mode_jcdma() == FALSE)
    {
      dsat707_send_config_action (dsat707_ext_action_table,
                                  dsat707_ext_action_table_size, 
                                  &item_ptr);

      dsat707_send_config_action (dsat707_fax_table,
                                  dsat707_fax_table_size,
                                  &item_ptr);
    }

#ifdef FEATURE_HDR_AT_TEST_ONLY
    dsat707_send_config_action (dsat707_hdr_table,
                                dsat707_hdr_table_size,
                                &item_ptr);
#endif /* FEATURE_HDR */

#ifdef FEATURE_DS_MOBILE_IP
    dsat707_send_config_action (dsat707_mip_table,
                                dsat707_mip_table_size,
                                &item_ptr);
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_PSTATS
    dsat707_send_config_action (dsat707_pstats_table,
                                dsat707_pstats_table_size,
                                &item_ptr);
#endif /* FEATURE_DS_PSTATS */

    dsat707_send_config_action (dsat707_vend_para_table,
                                dsat707_vend_para_table_size,
                                &item_ptr);

    dsat707_send_config_action (dsat707_vend_action_table,
                                dsat707_vend_action_table_size,
                                &item_ptr);
    if(NULL != item_ptr)
    {
      if (item_ptr->used > item_ptr->size - MIN_ROOM)
      {
        item_ptr->kind = DSM_PS_APP_PAYLOAD;
        ds707_async_ps_tcp_enqueue( &item_ptr);
        item_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
      }
    }
    if (NULL != item_ptr)
    {
      item_ptr->data_ptr[item_ptr->used++] = 'A';
      item_ptr->data_ptr[item_ptr->used++] = 'T';
      index = dsat707_find_cmd_name (cfg);
      if (index != -1) 
      {
        /*lint -save -e734*/
        item_ptr->used += dsatutil_fmt_string_val 
        (dsat707_ext_para_table + index,
         &item_ptr->data_ptr[item_ptr->used], 
         SET);
        /*lint -restore */
      }
      else
      {
        DS_AT_MSG0_HIGH("AT+CFG not sent to IWF");
      }
      temp_val =(dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
      item_ptr->data_ptr[item_ptr->used++] = (byte) temp_val;
    
      if (item_ptr->used > item_ptr->size - MIN_ROOM)
      {
        item_ptr->kind = DSM_PS_APP_PAYLOAD;
        ds707_async_ps_tcp_enqueue( &item_ptr);
        item_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
      }

      if (NULL != item_ptr)
      {
        item_ptr->data_ptr[item_ptr->used++] = 'A';
        item_ptr->data_ptr[item_ptr->used++] = 'T';
    
        if ((unrec_cmd_rcvd == FALSE) && (*cur_cmd != 'A'))
        {
          item_ptr->data_ptr[item_ptr->used++] = 'D';
        }
    
        while ( (c = *cur_cmd++) != 0)
        {
          item_ptr->data_ptr[item_ptr->used++] = c;
        }
        temp_val =(dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
        item_ptr->data_ptr[item_ptr->used++] = (byte) temp_val;
    
        item_ptr->kind = DSM_PS_APP_PAYLOAD;
        ds707_async_ps_tcp_enqueue( &item_ptr);
				/* Re-setting unrec_cmd_rcvd as the purpose of this boolean
					 will end with this function */
				unrec_cmd_rcvd = FALSE;
      }
    }
  }
} /*  dsat707i_send_config  */


/*===========================================================================

FUNCTION DSAT707_SEND_CONFIG_ACTION

DESCRIPTION
  This function transmits the mobile configuration to the base station.
  Configuration parameters are those which have the DOWN_LOAD attribute
  in the parse table.  The only DOWN_LOAD paramters transmitted are those
  with the ALWAYS attribute in the parse table, and those which
  have been changed from their default values.  Parameters with
  the FCLASS2P0 attribute are not transmitted unless the current value
  of the FCLASS parameter is 2.0.

  This function does one parse table.

DEPENDENCIES
  None
  
RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void dsat707_send_config_action
(
  const dsati_cmd_type * table,        /*  Table to send     */
  const unsigned int sizeof_table,  /*  Size of the table */
  dsm_item_type **item_ptr_ptr      /*  Output item       */

)
{
  unsigned int i, j, flag;
  byte c;
  const byte *q_ptr;
  dsat_num_item_type temp_val;
  void *base_add = NULL;

/*---------------------------------------------------------------------------
    Loop thru the table, looking for something to download.
---------------------------------------------------------------------------*/

  for (i = 0; i < sizeof_table; ++i)
    {
      flag = 0;
      if (table[i].attrib & DOWN_LOAD
          && ((table[i].attrib & FCLASS2P0) == 0
              || (dsat_num_item_type)dsatutil_get_val(
                               DSAT_EXT_FCLASS_IDX,0,0,NUM_TYPE) == 2 ))
      {
        base_add = (void*)dsatutil_get_val(table[i].cmd_id,0,0,STR_TYPE);
        
          if (table[i].attrib & ALWAYS)
          {
            flag = 1;
          }
          else if (table[i].attrib & STRING)
          {
            if (* (dsat_string_item_type *)base_add != '\0')
            {
              flag = 1;
            }
          }
          else
          {
            for (j = 0; j < table[i].compound; ++j)
            {
              if (* ((dsat_num_item_type *)base_add + j)
                  != ((dflm_type *) table[i].def_lim_ptr)[j].default_v)
              {
                flag = 1;
              }
            }
          }
      }
      
      if (flag)
      {
/*---------------------------------------------------------------------------
  Item found: Generate an AT command to transmit its
  current value to the base station.
  
  If there is not enough room in the data item, queue
  it and get a new one.
  
  Format the item according to it's type. Then terminate
  with a <CR>.
  ---------------------------------------------------------------------------*/
        if (NULL != *item_ptr_ptr)
        {
          if ((*item_ptr_ptr)->used > (*item_ptr_ptr)->size - MIN_ROOM)
          {
            (*item_ptr_ptr)->kind = DSM_PS_APP_PAYLOAD;
            ds707_async_ps_tcp_enqueue( item_ptr_ptr );
            *item_ptr_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
          }
        }

        if (NULL != *item_ptr_ptr)
        {
          (*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used++] = 'A';
          (*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used++] = 'T';
      
          if (table[i].attrib & STRING)           /*  String item */
          {
            /*lint -save -e734*/
            (*item_ptr_ptr)->used += dsatutil_fmt_string_val
            (table + i,
            &(*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used],
            SET);
            /*lint -restore */
          }
          else if (table[i].attrib & SIMPLE_DEC)  /*  Simple decimal item */
          {
            q_ptr = table[i].name;
            while ( (c = *q_ptr++) != 0)
            {
              (*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used++] = c;
            }
            /*lint -save -e734*/
            (*item_ptr_ptr)->used += dsatutil_fmt_num_val
              (table + i,
               &(*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used]);
            /*lint -restore */
          }
          else                                    /*  Extended item */
          {
            /*lint -save -e734*/
            (*item_ptr_ptr)->used += dsatutil_fmt_num_val_ex
              (table + i,
               &(*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used],
               SET);
            /*lint -restore */
          }
          temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
          (*item_ptr_ptr)->data_ptr[(*item_ptr_ptr)->used++] = (byte) temp_val;
        }
      }
    }
}

/*===========================================================================

FUNCTION DSAT707_FIND_CMD_NAME

DESCRIPTION
This function matches the command name that gets passed in with the command
table entry and returns the corresponding index.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns the table index that matches.  Returns -1 if no match is found.

SIDE EFFECTS
  None
  
===========================================================================*/
int dsat707_find_cmd_name
(
 byte *s1
)
{
  unsigned int result = 0;
  unsigned int index = 0;
  int no_match = 1;

  /*lint -save -e574 -e737*/
  for ( index = 0; index < dsat707_ext_para_table_size && no_match; index++ )
  {
     no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_ext_para_table[index].name );
     if (!no_match) 
     {
       result = index;
     }
     DS_AT_MSG_SPRINTF_1_LOW("%s found in ext_table", s1);
  }

  if(ds707_roaming_is_curr_mode_jcdma() == FALSE)
  {
    for ( index = 0; index < dsat707_fax_table_size && no_match; index++ )
    {
      no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_fax_table[index].name );
      if (!no_match) 
      {
        result = index;
      }
      DS_AT_MSG_SPRINTF_1_LOW("%s found in fax_table",s1);
    }
  }

#ifdef FEATURE_HDR_AT_TEST_ONLY
  for ( index = 0; index < dsat707_hdr_table_size && no_match; index++ )
  {
    no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_hdr_table[index].name );
    if (!no_match) 
    {
      result = index;
    }
    DS_AT_MSG_SPRINTF_1_LOW("%s found in hdr_table",s1);
  }
#endif /* FEATURE_HDR_AT_TEST_ONLY */

#ifdef FEATURE_DS_MOBILE_IP
  for ( index = 0; index < dsat707_mip_table_size && no_match; index++ )
  {
    no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_mip_table[index].name );
    if (!no_match) 
    {
      result = index;
    }
    DS_AT_MSG_SPRINTF_1_LOW("%s found in mip_table", s1);
  }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_PSTATS
 for ( index = 0; index < dsat707_pstats_table_size && no_match; index++ )
 {
   no_match = dsatutil_strcmp_ig_sp_case( s1, dsat707_pstats_table[index].name );
   if (!no_match) 
   {
     result = index;
   }
   DS_AT_MSG_SPRINTF_1_LOW("%s found in pstats_table",s1);
  }
#endif /* FEATURE_DS_PSTATS */

  for ( index = 0; index < dsat707_vend_para_table_size && no_match; index++ )
  {
    no_match = dsatutil_strcmp_ig_sp_case( s1, 
                                     dsat707_vend_para_table[index].name );
    if (!no_match) 
    {
      result = index;
    }
    DS_AT_MSG_SPRINTF_1_LOW("%s found in vend_table", s1);
  }
  /*lint -restore */
  if (no_match) 
  {
    return -1;
  }
  else
  {
    return result;
  }
}
#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================

FUNCTION DSAT707_IS_STATUS

DESCRIPTION
  This function returns TRUE if the argument string begins with
  the numeric character (1-8)  or the word corresponding to the
  result specified by the second arg.  Trailing characters are ignored.
  Other wise if returns FALSE.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None
===========================================================================*/

boolean dsat707_is_status
(
  byte *a_ptr,
  dsat_result_enum_type result
)
{
  byte c;
  const byte *c_ptr;

  ASSERT(a_ptr != NULL );
  
  if (*a_ptr == '0' + (byte) result)
  {
    return TRUE;
  }

  if((result >= DSAT_MAX_BASIC)||(result < DSAT_OK))
  {
    /* Applicable till 9 only! */
    return FALSE;
  }

  c_ptr = res_codes [result];

  while ( (c = *c_ptr++) != '\0')
  {
    if (c != *a_ptr++)
    {
      return FALSE;
    }
  }
  return TRUE;
} /* dsat707_is_status */


/*===========================================================================
  FUNCTION DSAT707_IP_STR_TO_UINT32

  DESCRIPTION
    This function takes an IP address in string format (which includes
    "." as delimiters. e.g. 123.23.123.231) and convert the string into
    a uint32 value.

  DEPENDENCIES
    None

  RETURN VALUE
    status ( success or error)

  SIDE EFFECTS
    None
===========================================================================*/
boolean dsat707_ip_str_to_uint32
(
  char   *ip_string,
  uint32 *ip_uint32
)
{
  byte                   buff[4];
  dsat_num_item_type     ip_val[4];
  uint8 buff_index     = 0;
  uint8 ip_index       = 0;
  uint8 n_dots         = 0;
  boolean loop         = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  ip_val[0] = ip_val[1] = ip_val[2] = ip_val[3] = 0;
  /*-------------------------------------------------------------------------
    The following loop processes the string representing the IP address. Each
    one of the four-tuples is converted into decimal integer and stored in a
    temporary array. If the value is greater than 255, we return an error,
    because each tuple in the IP cannot be greater than 255.
  -------------------------------------------------------------------------*/
  while(loop != FALSE)
  {
    if( (*ip_string != '.') &&
        (*ip_string != '\0') )
    {
      /* check to see if > 3 digits specified on for this tuple */
      if(buff_index >= 3)
      {
        DS_AT_MSG0_ERROR("Too many digits");
        return FALSE;
      }
      buff[buff_index] = (byte) *ip_string;
      buff_index++;
    }
    else
    {
      n_dots++;
      buff[buff_index]= '\0';  /* null terminate the string */

      /* check to see if > 3 digits */
      if(ip_index >= 4)
      {
        DS_AT_MSG0_ERROR("Too many digits");
        return FALSE;
      }
      
      /*---------------------------------------------------------------------
        Convert ascii to integer and return error if an invalid ip address
        is entered. eg A.12.14.45
      ---------------------------------------------------------------------*/
      if( ATOI_OK != dsatutil_atoi( (ip_val + ip_index),buff,10 ) )
      {
        DS_AT_MSG0_ERROR("Error while converting from atoi");
        return FALSE;
      }
      else if(ip_val[ip_index] > 255)
      {
        DS_AT_MSG0_ERROR("IP address tuple greater than 255");
        return FALSE;
      }
      buff_index = 0;
      ip_index++;
      if(*ip_string == '\0')
      {
        loop=FALSE;

        /*-------------------------------------------------------------------
          If the ip address has more than 3 dots it is an invalid 
          ip address eg: 129.23.34  , 129.34.45.65.78 etc.
        -------------------------------------------------------------------*/
        if(n_dots != 4)
        {
          DS_AT_MSG0_ERROR("Invalid IP address");
          return FALSE;
        }
      }
    }/* else */
    ip_string++;
  } /* while */

  /*-------------------------------------------------------------------------
    Store the converted IP address into a uint32
  --------------------------------------------------------------------------*/
  *ip_uint32 = (((uint32) ip_val[0]) << 24) |
               (((uint32) ip_val[1]) << 16) |
               (((uint32) ip_val[2]) << 8)  |
               (uint32) ip_val[3];

  DS_AT_MSG1_LOW("ip_uint32 is %ld", *ip_uint32);
  return TRUE;
} /* dsat707_ip_str_to_uint32 () */

/*===========================================================================
FUNCTION DSAT707_MCAST_IP_STR_TO_UINT32

DESCRIPTION
  This function takes an IP address in string format (which includes
  "." as delimiters. e.g. 123.23.123.231) and convert the string into
  a uint32 value. It checks to make sure that the IP address is in the
  multicast range or is 0.0.0.0

  The string may have more information than the IP address and this 
  function will stop processing once it has found or failed to find a valid
  IP address. The number of bytes processed by the function is returned in
  len_curr_ip.

DEPENDENCIES
  None

RETURN VALUE
  OK if the IP address is a valid multicast IPv4 address or 0.0.0.0,
  ERROR otherwise. The number of bytes processed is returned in 
  len_curr_ip independent of success or failure.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsat707_mcast_ip_str_to_uint32
(
  char   *ip_string,
  uint32 *ip_uint32,
  uint32 *len_curr_ip
)
{
  byte                   buff[4];
  dsat_num_item_type     ip_val[4];
  atoi_enum_type         atoi_result;
  uint8 buff_index     = 0;
  uint8 ip_index       = 0;
  uint8 n_dots         = 0;
  boolean loop         = TRUE;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*lint -save -e572 suppressing error 572*/
  ip_val[0] = ip_val[1] = ip_val[2] = ip_val[3] = 0;
  /*-------------------------------------------------------------------------
    The following loop processes the string representing the IP address. Each
    one of the four-tuples is converted into decimal integer and stored in a
    temporary array. If the value is greater than 255, we return an error,
    because each tuple in the IP cannot be greater than 255.
  -------------------------------------------------------------------------*/
  if(len_curr_ip != NULL)
  {
    (*len_curr_ip) = 0;
  }

  while(loop != FALSE)
  {
    if( (*ip_string != '.') &&
        (*ip_string != '\0') &&
        (*ip_string != ' ' ))
    {
      /* check to see if > 3 digits specified on for this tuple */
      if(buff_index >= 3)
      {
        DS_AT_MSG0_ERROR("Too many digits");
        return FALSE;
      }
      buff[buff_index] = *ip_string;
      buff_index++;
    }
    else
    {
      n_dots++;
      buff[buff_index]= '\0';  /* null terminate the string */

      /* check to see if > 3 digits */
      if(ip_index >= 4)
      {
        DS_AT_MSG0_ERROR("Too many digits");
        return FALSE;
      }

      /*---------------------------------------------------------------------
        Convert ascii to integer and return error if an invalid ip address
        is entered. eg A.12.14.45
      ---------------------------------------------------------------------*/
      atoi_result = dsatutil_atoi( (ip_val + ip_index),buff,10 );
      if(atoi_result != ATOI_OK)
      {
        DS_AT_MSG0_ERROR("Error while converting from atoi");
        return FALSE;
      }
      else if(ip_val[ip_index] > 255)
      {
        DS_AT_MSG0_ERROR("IP address tuple greater than 255");
        return FALSE;
      }
      buff_index = 0;
      ip_index++;
      if(*ip_string == '\0' || *ip_string == ' ')
      {
        loop=FALSE;

        /*-------------------------------------------------------------------
        If the ip address has more than 3 dots it is an invalid 
        ip address eg: 129.23.34  , 129.34.45.65.78 etc.
        -------------------------------------------------------------------*/
        if(n_dots != 4)
        {
        DS_AT_MSG0_ERROR("Invalid IP address");
        return FALSE;
        }
      }
    }/* else */
    ip_string++;
    if(len_curr_ip != NULL)
    {
      (*len_curr_ip) += 1;
    }
  } /* while */

  /*-------------------------------------------------------------------------
    Store the converted IP address into a uint32
  --------------------------------------------------------------------------*/
  *ip_uint32 = (((uint32) ip_val[0]) << 24) |
         (((uint32) ip_val[1]) << 16) |
         (((uint32) ip_val[2]) << 8)  |
         (uint32) ip_val[3];

  /*-------------------------------------------------------------------------
    Validate the IP address for multicast range or 0.0.0.0
  --------------------------------------------------------------------------*/
  if(!PS_IN_IS_ADDR_MULTICAST((dss_htonl(*ip_uint32))) && (*ip_uint32 != 0))
  {
    DS_AT_MSG0_ERROR("IP not in mcast range");
    return FALSE;
  }

  DS_AT_MSG1_LOW("ip_uint32 is %ld", *ip_uint32);
  return TRUE;
  /*lint -restore Restore lint error 572*/
} /* dsat707_mcast_ip_str_to_uint32 */


/*===========================================================================
FUNCTION DSAT707_STRIP_QUOTES

DESCRIPTION
  This fuction 
  - takes an input quoted string,
  - puts a copy of the string in "parsed_string" with the quotes stripped off
  - NULL-terminates the parsed_string by placing \0 at the end.
  - Validates that the output string length falls within the range specified
    in the ? 
returning  an ERROR if out 
  The function also checks for the range of the AT command and throws out an
  error if the range is exceeded.

  For examples,
    "123.12.23.21"           ---> 123.12.23.21\0
    "nobody,one@nowhere.com" ---> nobody,one@nowhere.com\0

DEPENDENCIES
  None

PARAMETERS
   raw_string - Returned by the standard ATCoP parser, ds_atcop_parse_ex().
   parsed_string - Buffer where the parsed argument shall be returned.
   table_entry  -  To find out the size of the argument defined in the table.

RETURN VALUE
  FALSE  if - null input or output string pointers are provided
            - output string length is out of range

  TRUE   if conversion goes well

SIDE EFFECTS
  None
===========================================================================*/
#define STRIP_QUOTES_MAX_LEN  (128)

boolean dsat707_strip_quotes
(
  char *  in_s,                   /* input string (to be stripped)         */
  char *  parsed_s,               /* null-term output string placed here   */
  uint16  max_parsed_len          /* size of provided output buffer        */
)
{
  char     temp_s[STRIP_QUOTES_MAX_LEN];     /* temporary output storage   */
  char *   out_s;                 /* working pointer for stripped output   */
  char *   max_out_s;             /* limit for output pointer              */
  boolean  escape;                /* state flag - escape next character    */
  boolean  quote;                 /* state flag - currently in quotes      */
  boolean  copy;                  /* state flag - copy current character   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Make sure there are no NULL de-reference and that at least one 
    character (+ null terminator) can be placed in output buffer.
  -------------------------------------------------------------------------*/
  if( parsed_s == NULL || max_parsed_len < 2 )
  {
    DS_AT_MSG0_ERROR("No output buffer provided!");
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Parse the input string
    " xxx " - whitespace enclosed between quotes will be preserved
            - quotes need not be matched
      \     - escape character, next character is 
  -------------------------------------------------------------------------*/
  out_s      = &temp_s[0];
  max_out_s  = &temp_s[MIN(max_parsed_len, STRIP_QUOTES_MAX_LEN) - 1];  /* leave room for \0 */
  if (in_s)
  {
    escape = FALSE;
    quote  = FALSE;
    copy   = FALSE;
    while( *in_s )   /* terminate if NULL in input string is reached */
    {
      if (escape)    /* if last byte was escape, don't interpret this byte */
      {
        copy   = TRUE;
        escape = FALSE;
      }
      else
      {
        switch(*in_s)
        {
          case '"':
          {
            quote = !quote;
          }
          break;

          case '\\':   /* allow escape character - note escaping req'd here! */
          { 
            escape = TRUE;
          }
          break;

          case ' ':   /* space is included only if currently quoting */
          case '\t': //0x9:   /* tab is included only if currently quoting */
          case '\n':  /* newline */
          case '\r':  /* linefeed */
          case '\f':  /* formfeed */
          case '\b':  /* backspace */
          case '\v':  /* vertical tab */
          {
            if (!quote)
            {
              break;
            }
            /* else fall through */
          } /*lint -save -e616 */

          default: /*lint -restore */
          {
            copy = TRUE;
          }
          break;
        }
      }

      if (copy)
      {
        if (out_s == max_out_s)
        {
          DS_AT_MSG1_ERROR("Unquoted value length exceeds maximum (%d)!",
                     max_parsed_len);
          return FALSE;
        }
        *out_s++ = *in_s;  /* copy current character to output string */
        copy = FALSE;
      }

      in_s++;        /* done with current character, go to next */

    }/* while */
  }

  /* Terminate the string */
  *out_s++ = '\0';

  /* copy output to user's buffer */
  (void) strlcpy (parsed_s, temp_s, max_parsed_len);

  return TRUE;

} /* dsat707_strip_quotes() */

#endif /* FEATURE_DATA_IS707 */
