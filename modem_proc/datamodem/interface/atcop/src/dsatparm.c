/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   ( C O M M O N 
                                
                P A R A M E T E R  C O M M A N D S )   

                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It executes parameter based AT commands.
  These parameter commands are common to any mode of operation.

  
EXTERNALIZED FUNCTIONS

  dsatparm_exec_param_cmd
    This function takes the result from the command line parser
  and executes it. This function executes parameter/S register
  commands.
  
  dsatparm_exec_ipr_cmd
    This function takes the result from the command line parser
  and executes it. It executes +IPR command.
  at+ipr is used to specify the data rate at which the DCE will 
  accept the command

  dsatparm_exec_ampv_cmd
    This function takes the result from the command line parser
  and executes it. It executes at&v command.
  at&v is used to dump configuration parameters.

  dsatparm_exec_fclass_cmd
    This function takes the result from the command line parser
  and executes it. It executes +FCLASS command.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatparm.c_v   1.4   07 Nov 2002 16:15:36   wxie  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatparm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/14   tk      Optimized debug macros usage in ATCoP.
08/01/13   sc      Fixed KW errors.
05/18/12   tk      Migrated to MSG 2.0 macros
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
03/16/11   bs      Restructured command tables to be mode independant.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
03/18/10   nc      Added Support for +CGEQOS for LTE.
12/15/09   nc      Featurisation changes for LTE.
09/09/09   bs      Fixed Klocwork criticals.
04/20/09   ua      Fixed issues related with call commands followed by &V.
05/12/08   bs      Merging Lint fix from ULC.
03/24/08   bs      Fixed Klocwork criticals.
02/18/08   sa      Added modification for Passport Feature support.
12/20/07   sa      Fixed feature wrap issues.
11/04/07   pp      Lint low - fix.
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
08/06/07   sa      Removed +CGEQREQ and +CGEQMIN support in GSM only builds.
04/23/07   pp      Lint Medium fixes.
04/11/07   ua      Modifications as per KDDI requirements. 
03/13/07   ua      Fixed critical lint errors.
02/14/07   pkp     Fixed Klocwork (NULL dereference) errors.
01/30/07   ss      Replaced banned string API calls
11/03/06   snb     Dual processor changes.
02/22/06   snb     ds3g_siolib_set_answer_port() API change for serializer.
04/14/05   sb      Changes for incoming CS call when serializer is enabled
03/08/05   ar      Add BROKEN_RANGE support to test syntax handling.
02/09/05   tkk     Add support for ^SGDCONT in &V generation.
12/04/03   ar      Partition &V cmd handler for asychronous query to CM.
11/19/03   ar      Add support for +CGDSCONT and +CGTFT in &V generation.
08/24/03   sb      More error checking in +FCLASS command
08/14/03   sb      Fixed +FCLASS formating in ETSI mode
08/13/03   rsl     Moved at+fclass to dsatctab.c
07/30/03   ar      Added +CGRLP multi-version response generation to &V.
03/17/03   ar      Add BROKEN_RANGE attribute in default command processing.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/27/03   wx      Be strict on the number of argument and the length of 
                   string input for mixed parameter
02/26/03   ar      Skip test response if LOCAL_TEST not set for mixed param.
                   Added +CGACT response generation to &V command.
02/20/03   wx      Return (x) instead of (x-x) for test command when a
                   command's numerical range is only one value
02/12/03   rsl     In process_read_cmd and test_cmd, replaced (byte) typecast
                   to (word) typecase to avoid large numbers being cut short.
01/31/03   ar      Add +CGEQMIN to &V response.
01/22/03   ar      Restrict &V respose to commands in current operating mode.
11/13/02   ar      Restored baud rate change to IPR handler, removed unused
                   change_baud() function
11/07/02   ar      Added FEATURE_GSM_GPRS to FEATURE_DATA_WCDMA_PS wrappers
10/09/02   sb      IPR should not store the baud rate into NV (fixed it)
07/12/02   ak      fixed minor compiler warning.
07/12/02   ak      Changes to support writing new IPR value into nv.
06/27/02   ar      Call command specific reporting function on MULTI_INDEX 
                   command attribute
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
05/15/01   sb      Created module.


===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#include <stdio.h>
#include <string.h>

#include "dsati.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "err.h"
#include "nv.h"
#include "dstaski.h"

#include "ds3gsiolib.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsicmif.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DATA_IS707
#include "ds707_jcdma_m51.h"
#include "ds707_roaming.h"
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

#define DSM_PUSHDOWN_TAIL(dsm,buffer,size) \
      dsat_dsm_pushdown_tail (&dsm, buffer, (uint16)size, \
                              DSM_ITEM_POOL(dsm), FALSE)



/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/
LOCAL const dsat_string_item_type string_range [] = "(20,21,23-7E)";

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL boolean process_default_cmd
(
  const dsati_cmd_type *parse_table  /*Ptr to cmd in parse tbl*/
);

LOCAL boolean process_mixed_param_default_cmd
(
  const dsati_cmd_type *parse_table /*  Ptr to cmd in parse table */
);

LOCAL boolean process_read_cmd
(
  const dsati_cmd_type *parse_table, /* Ptr to cmd in parse table */
  dsm_item_type * buf_ptr                        /* Place to put response     */
);

LOCAL boolean process_read_mixed_param_cmd
(
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table */
  dsm_item_type * buf_ptr                        /*  Place to put response     */
);

LOCAL boolean process_write_cmd
(
  const tokens_struct_type *tok_ptr, /* Cmd tokens from parser    */
  const dsati_cmd_type *parse_table  /* Ptr to cmd in parse table */
);

LOCAL boolean process_write_mixed_param_cmd
(
  const tokens_struct_type *tok_ptr, /*Cmd tokens from parser*/
  const dsati_cmd_type *parse_table  /*Ptr to cmd in parse table*/
);

LOCAL boolean process_test_cmd
(
  const dsati_cmd_type *parse_table, /* Ptr to cmd in parse tbl */
  dsm_item_type *buff_ptr                        /* Place to put response   */
);

LOCAL boolean process_test_mixed_param_cmd
(
  const dsati_cmd_type *parse_table, /* Start of parse tbl    */
  dsm_item_type *buf_ptr             /* Place to put response */
);

LOCAL dsm_item_type * do_settings_action
(
  const dsati_cmd_type * table,     /*  Parse Table to dump             */
  unsigned int index,               /*  Size of Table                   */
  unsigned int * char_per_line,     /*  Used to count char across calls */
  dsm_item_type * res_buff_ptr      /* Place to put response            */
);

LOCAL dsm_item_type * do_settings_dump
(
  const dsati_cmd_type * table,     /*  Parse Table to dump             */
  const unsigned int sizeof_table,  /*  Size of Table                   */
  unsigned int * char_per_line,         /*  Count of char across calls */
  dsm_item_type *res_buff_ptr           /*  Place to put response     */
);


/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION DSATPARM_EXEC_PARAM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes parameter/S register
  commands.

DEPENDENCIES
  None
  
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
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_param_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*---------------------------------------------  
   There is no argument. Set the deafult values  
  ----------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    if ( process_default_cmd (parse_table) )
    {
      result = DSAT_OK;
      return result;
    }
    else 
    {
      result = DSAT_ERROR;
      return result;
    }
  }

  /*----------------------------------------------------------------
   Standard processing for a command with an argument (WRITE command):
   -----------------------------------------------------------------*/
  
  else if (tok_ptr->op == (NA|EQ|AR))
  {
    if ( process_write_cmd (tok_ptr, parse_table) )
    {
      result = DSAT_OK;
      return result;
    }
    else 
    {
      result = DSAT_ERROR;
      return result;
    }
  }

  /*--------------------------------------  
   Standard processing for a READ command:
   ----------------------------------------*/
        
  else if ( tok_ptr->op == (NA|QU) )
  {
    if ( process_read_cmd (parse_table, res_buff_ptr) )
    {
      result = DSAT_OK;
      return result;
    }
    else 
    {
      result = DSAT_ERROR;
      return result;
    }
  }

  /*--------------------------------------
  Standard processing for a TEST command:
  ----------------------------------------*/

  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    if ( process_test_cmd (parse_table, res_buff_ptr) )
    {
      result = DSAT_OK;
      return result;
    }
    else 
    {
      result = DSAT_ERROR;
      return result;
    }
  }

  /*--------------------------------------------------
  Command error: Operator(s) found did not make sense.
  ----------------------------------------------------*/

  else
  {
    result = DSAT_ERROR;
    return result;
  }

} /*  dsatparm_exec_param_cmd */


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
/*ARGSUSED*/
dsat_result_enum_type dsatparm_exec_fclass_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
  dsati_mode_e_type             current_mode;
  static const byte fclass_707_range[] = "0,2.0";
  static const byte fclass_0[] = "0";
  static const byte fclass_2p0[] = "2.0";
  static const byte fclass_etsi_range[] = "+FCLASS: (0-1)";
  const byte *s_ptr;
  dsat_num_item_type etsi_fclass_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
#ifdef FEATURE_DATA_IS707
  NOT_SUPPORTED_IN_JCDMA()
#endif /* FEATURE_DATA_IS707 */
  *rb_ptr = '\0';
  current_mode = dsatcmdp_get_current_mode();
  
  if ((current_mode == DSAT_MODE_GSM) || (current_mode == DSAT_MODE_WCDMA))
  {
    if (tok_ptr->op == (NA|EQ|QU)) /*  TEST command  */
    {
      s_ptr = fclass_etsi_range;
      while ( (*rb_ptr++ = *s_ptr++) != 0) {}
    }
    else if (tok_ptr->op == (NA|EQ|AR)) /*  SET command   */
    {
      if ( dsatutil_atoi(&etsi_fclass_val, tok_ptr->arg[0], 10) )
      {
        /* invalid argument */
        return DSAT_ERROR;
      }

      if (etsi_fclass_val == 0)
      {
        DSATUTIL_SET_VAL(tab_entry->cmd_id,0,0,0,0,NUM_TYPE)
      }
      else if (etsi_fclass_val == 1)
      {
        DSATUTIL_SET_VAL(tab_entry->cmd_id,0,0,0,1,NUM_TYPE)
      }
      else
      {
        /* Invalid argument */
        return DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA|QU))    /*  READ command  */
    {
      (void) dsatutil_fmt_num_val_ex (tab_entry, rb_ptr, RESPONSE);
    }
    else if (tok_ptr->op == (NA))            /*  Name only - set to default  */
    {
      DSATUTIL_SET_VAL(tab_entry->cmd_id,0,0,0,((dflm_type *) tab_entry->def_lim_ptr)[0].default_v,NUM_TYPE)
    }
    else
    {
      /* Invalid command */
      return DSAT_ERROR;
    }
  }
  else
  {
    /* We are in CDMA mode */
    if ((tab_entry->attrib & FCLASS2P0) && 
        ((dsat_num_item_type)dsatutil_get_val(DSAT_EXT_FCLASS_IDX,0,0,NUM_TYPE) != 2))
    {
      result = DSAT_ERROR;
      return result;
    }

    if (tok_ptr->op == (NA|QU))    /*  READ command  */
    {
      (void) dsatutil_fmt_num_val_ex (tab_entry, rb_ptr, RESPONSE);
    }
    else if (tok_ptr->op == (NA))            /*  Name only - set to default  */
    {
      DSATUTIL_SET_VAL(tab_entry->cmd_id,0,0,0,((dflm_type *) tab_entry->def_lim_ptr)[0].default_v,NUM_TYPE)
    }
    else if (tok_ptr->op == (NA|EQ|QU)) /*  TEST command  */
    {
      s_ptr = fclass_707_range;
      while ( (*rb_ptr++ = *s_ptr++) != 0)
        ;
    }
    else if (tok_ptr->op == (NA|EQ|AR)) /*  SET command   */
    {
      if (dsatutil_strcmp_ig_sp_case(tok_ptr->arg[0], fclass_0) == 0)
      {
        DSATUTIL_SET_VAL(tab_entry->cmd_id,0,0,0,0,NUM_TYPE)
      }
      else if (dsatutil_strcmp_ig_sp_case(tok_ptr->arg[0], fclass_2p0) == 0)
      {
        DSATUTIL_SET_VAL(tab_entry->cmd_id,0,0,0,2,NUM_TYPE)
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else/*  Didn't make sense - ERROR */
    {
      result = DSAT_ERROR;
    }
  }

  res_buff_ptr->used = (word)strlen((char *)res_buff_ptr->data_ptr);
  return result;

}/* dsatparm_exec_fclass_cmd */


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
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NORESPONSE : set for certain commands

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
)
{
  dsat_num_item_type    old_ipr_val;
  dsat_num_item_type    temp_val;
  dsat_result_enum_type result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  old_ipr_val = (dsat_num_item_type)dsatutil_get_val(
                                      DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);

  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
#ifdef FEATURE_DATA_IS707
  if( (result == DSAT_OK) && (IS_JCDMA_MODE()))
  {
    temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
    if((tok_ptr->op == (NA|EQ|AR) &&
       (SIO_BITRATE_115200 != (sio_bitrate_type)temp_val) ))
    {
      /* Baudrate not allowed, tell user "ERROR".  Restore old IPR value. */
      result = DSAT_ERROR;
      DSATUTIL_SET_VAL(DSAT_EXT_IPR_IDX,0,0,0,old_ipr_val,NUM_TYPE)

    }
    else if (tok_ptr->op == (NA|EQ|QU))
    {
      res_buff_ptr->used =
        (word) snprintf((char*)res_buff_ptr->data_ptr,
                               res_buff_ptr->size,
                               "+IPR:(115200)");
    }
  }
#endif /* FEATURE_DATA_IS707 */
  if ((result == DSAT_OK) && 
    ((tok_ptr->op == (NA|EQ|AR)) || (tok_ptr->op == (NA))))
  { 
    temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
    if (ds3g_siolib_is_baud_valid ((sio_bitrate_type)temp_val) == FALSE)
    {
      /* Baudrate not allowed, tell user "ERROR".  Restore old IPR value. */
      result = DSAT_ERROR;
      DSATUTIL_SET_VAL(DSAT_EXT_IPR_IDX,0,0,0,old_ipr_val,NUM_TYPE)
    }
    else
    {
#ifdef FEATURE_DATA_IS707
      /*--------------------------------------------------------------------
         Note that this if statement does the function call internally, which
         will result in the baud rate being changed, if the passed in ipr_val
         is okay
       --------------------------------------------------------------------*/
      temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
      if ((IS_JCDMA_MODE()) && 
          (ds707_jcdma_chng_m513_via_at((sio_bitrate_type)temp_val)
          == FALSE))
      {
        result = DSAT_ERROR;
        DSATUTIL_SET_VAL(DSAT_EXT_IPR_IDX,0,0,0,old_ipr_val,NUM_TYPE)
      }
      else
#endif /* FEATURE_DATA_IS707 */
      {
        /*-------------------------------------------------------------------------
          Inform SIO that the baud rate should be scheduled after sending OK to the 
          TE device.
         -------------------------------------------------------------------------*/
         temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_EXT_IPR_IDX,0,0,NUM_TYPE);
        (void)ds3g_siolib_change_baud_rate((sio_bitrate_type) temp_val, FALSE);
      }
    }
  }
  
  return result;

}/* dsatparm_exec_ipr_cmd */



/*===========================================================================

FUNCTION DSATPARM_EXEC_AMPV_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at&v command.
  at&v is used to dump configuration parameters.

DEPENDENCIES
  Call Manager must be ready to accept query command  

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_CMD : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_ampv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type     result;
  if (tok_ptr->op == NA)
  {
    /* Generate read response */
    result = dsatparm_gen_ampv_response( mode, res_buff_ptr );
  } /* mode == DSAT_CMD */
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}/* dsatparm_exec_ampv_cmd */


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
/* ARGSUSED */
dsat_result_enum_type dsatparm_gen_ampv_response
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type     result = DSAT_OK;
  dsm_item_type             *buff_ptr;
  dsati_at_cmd_table_entry_type *array_ptr; /*Ptr to array of cmd tbl entries */
  unsigned int              j, r;           /*Count indices*/
  unsigned int              char_per_line;  /*Used to count char across calls*/
  boolean async_rsp = FALSE;

  /* Allocate a buffer for the asychronous response */ 
  if ( NULL == res_buff_ptr )
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    async_rsp = TRUE;
  }

  /* Let buff_ptr point to the buffer */
  /* If the response buffer gets full, a new buffer */
  /* is allocated and then buff_ptr will point to the new buffer */
  buff_ptr = res_buff_ptr;
  *buff_ptr->data_ptr = '\0';    /*  Initialize response to null */
  char_per_line = 0;             /*  Initialize character count to zero */

  for ( r = 0; r < (unsigned int)NUM_AT_CMD_CATEGORIES; r++ )
  {
    /* Get a pointer to array of command table entries for 
       operating mode from command table. */
    array_ptr = at_cmd_table[r];  
    
    if ( array_ptr != NULL )
    {
      /* An array of command table entries exists for this element 
         of the table, now look for command tables within the array. */
      for ( j = 0; array_ptr[j].table_ptr != NULL; j++ )
      {
        buff_ptr = do_settings_dump( array_ptr[j].table_ptr,
                                     *(array_ptr[j].table_size),
                                     &char_per_line,
                                     buff_ptr );

      }/* for j* */
      
    }/*array_ptr != NULL*/ 
    
  }/* NUM_AT_CMD_CATEGORIES */
  
  /*-----------------------------------------------------------------------
    Close the last line, overwriting the trailing "; 
  ------------------------------------------------------------------------*/
  /* We fail in next function call, if we really hit a NULL */
  if ( buff_ptr != NULL ) 
  {
    buff_ptr->data_ptr[buff_ptr->used-2] = '\0';
  }

  /* Send async response the TE */
  if ( async_rsp )
  {
    dsatcmdp_handle_async_cmd_rsp( res_buff_ptr, DSAT_COMPLETE_RSP );
  }

  return result;
}/* dsatparm_gen_ampv_response */



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
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_dtype_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*-----------------------------------------
   Processing for a command with no argument
  -------------------------------------------*/
  if (tok_ptr->op == NA)
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
}/* dsatparm_exec_dtype_cmd */


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
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_ext_param_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*-----------------------------------------
   Processing of the command
  -------------------------------------------*/
#ifdef FEATURE_DATA_IS707
  NOT_SUPPORTED_IN_JCDMA()
#endif /* FEATURE_DATA_IS707 */
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                   );

  return result;
}/* dsatparm_exec_ext_param_cmd */
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
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_sthree_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DATA_IS707
  dsat_num_item_type old_dsat_s3_val = (dsat_num_item_type)dsatutil_get_val(
                                        DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
#endif /* FEATURE_DATA_IS707 */
  /*----------------------------------------------------------------
   Standard processing for a command
   -----------------------------------------------------------------*/  
  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
#ifdef FEATURE_DATA_IS707
  if(IS_JCDMA_MODE())
  {
    if ( (tok_ptr->op == (NA|EQ|AR)) && ( result == DSAT_OK ) )
    {
      /* In JCDMA mode S3 values >13, should return ERROR */
      if ( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_SREG_S3_IDX,0,0,NUM_TYPE) > 13 )
      {
        DSATUTIL_SET_VAL(DSAT_SREG_S3_IDX,0,0,0,old_dsat_s3_val,NUM_TYPE)
        return DSAT_ERROR;
      }
    }
  }
#endif /* FEATURE_DATA_IS707 */
  return result; 
} /* dsatparm_exec_sthree_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_sfour_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DATA_IS707
  dsat_num_item_type old_dsat_s4_val = (dsat_num_item_type)dsatutil_get_val(
                                        DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
#endif /* FEATURE_DATA_IS707 */
  /*----------------------------------------------------------------
   Standard processing for a command
   -----------------------------------------------------------------*/  
  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
#ifdef FEATURE_DATA_IS707
  if( IS_JCDMA_MODE())
  {
    if ( (tok_ptr->op == (NA|EQ|AR)) && ( result == DSAT_OK ) )
    {
      /* In JCDMA mode S4 values >10, should return ERROR */
      if ( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_SREG_S4_IDX,0,0,NUM_TYPE) > 10 )
      {
        DSATUTIL_SET_VAL(DSAT_SREG_S4_IDX,0,0,0,old_dsat_s4_val,NUM_TYPE)
        return DSAT_ERROR;
      }
    }
  }
#endif /* FEATURE_DATA_IS707 */
  return result; 
} /* dsatparm_exec_sfour_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_sfive_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
#ifdef FEATURE_DATA_IS707
  dsat_num_item_type old_dsat_s5_val = (dsat_num_item_type)dsatutil_get_val(
                                        DSAT_SREG_S5_IDX,0,0,NUM_TYPE);
#endif /* FEATURE_DATA_IS707 */
  /*----------------------------------------------------------------
   Standard processing for a command
   -----------------------------------------------------------------*/  
  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
#ifdef FEATURE_DATA_IS707
  if( IS_JCDMA_MODE() )
  {
    if ( (tok_ptr->op == (NA|EQ|AR)) && ( result == DSAT_OK ) )
    {
      /* In JCDMA mode S5 values >8, should return ERROR */
      if ( (dsat_num_item_type)dsatutil_get_val(
                                    DSAT_SREG_S5_IDX,0,0,NUM_TYPE)> 8 )
      {
        DSATUTIL_SET_VAL(DSAT_SREG_S5_IDX,0,0,0,old_dsat_s5_val,NUM_TYPE)
        return DSAT_ERROR;
      }
    }
  }
#endif /* FEATURE_DATA_IS707 */
  return result; 
} /* dsatparm_exec_sfive_cmd */

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
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_sseven_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*----------------------------------------------------------------
   Standard processing for a command
   -----------------------------------------------------------------*/  
  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
#ifdef FEATURE_DATA_IS707
  if( IS_JCDMA_MODE() )
  {
    if ( (tok_ptr->op == (NA|EQ|AR)) && ( result == DSAT_OK ) )
    {
      /* In JCDMA mode S7 values >50, should be trunctated to 50 */
      if ( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_SREG_S7_IDX,0,0,NUM_TYPE) > 50 )
      {
        DSATUTIL_SET_VAL(DSAT_SREG_S7_IDX,0,0,0,50,NUM_TYPE)
      }
    }
  }
#endif /* FEATURE_DATA_IS707 */
  return result; 
} /* dsatparm_exec_sseven_cmd */


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
/* ARGSUSED */
dsat_result_enum_type dsatparm_exec_szero_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result= DSAT_OK;

  /*----------------------------------------------------------------
   Standard processing for a command
   -----------------------------------------------------------------*/  
  result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
  if ( (tok_ptr->op == (NA|EQ|AR)) && ( result == DSAT_OK ) )
  {
    ds3g_siolib_set_answer_port (DS3G_SIOLIB_ANSWER_MODE_AUTO);
  }
  return result;
}

/*===========================================================================

FUNCTION PROCESS_DEFAULT_CMD

DESCRIPTION
  This function is called by dsatparm_exec_param_cmd.
  It is called when there are no aguments to the command that is being
  processed

DEPENDENCIES
  None

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets default parameter value for the command

===========================================================================*/
LOCAL boolean process_default_cmd
(
  const dsati_cmd_type *parse_table /*  Ptr to cmd in parse table */
)
{
  int i;

  /*  S-reg illegal without '=' */
  if ( (parse_table->attrib & 
        (CONFIG | EXTENDED | STRING | SIMPLE_DEC  | MIXED_PARAM))
    == (CONFIG))
  {
    /* failure in execution of cmd*/
    return ( FALSE );
  }
  
  /*  Single letter - set to 0  */
  else if ( (parse_table->attrib 
             & (CONFIG | EXTENDED | STRING | SIMPLE_DEC | MIXED_PARAM))
            == (CONFIG | SIMPLE_DEC))
  {
    dsat_num_item_type *val_ptr = (dsat_num_item_type *)EXEC_VAL_PTR(parse_table);
    if( NULL != val_ptr)
     {
       *val_ptr = 0;
    /* Success in execution of cmd */
    return ( TRUE );
  }
     return ( FALSE );
  }

        /*  Extended - set to default */
  else if ( (parse_table->attrib & (CONFIG | EXTENDED | STRING | MIXED_PARAM))
  == (CONFIG | EXTENDED))
  {
    uint16 range_offset = 0;
    dsat_num_item_type *val_ptr = (dsat_num_item_type*) EXEC_VAL_PTR(parse_table);	
    for ( i = 0; (NULL != val_ptr)  && (i < parse_table->compound); ++i )
    {
      * (val_ptr + i) = ((dflm_type *) parse_table->def_lim_ptr)[i+range_offset].default_v;

      /* For BROKEN_RANGE, advance to next range segment */
      if (0 != (parse_table->attrib & BROKEN_RANGE))
      {
        while (MAX_BROKEN_RANGE !=
                ((dflm_type *)parse_table->
                  def_lim_ptr)[i + range_offset].default_v)
        {
          range_offset++;
        }
      }
    }
    /* Success in execution of cmd*/
    return ( TRUE );
  }

  /*  String - set to null      */
  else if ( (parse_table->attrib & (CONFIG | EXTENDED | STRING | MIXED_PARAM))
    == (CONFIG | EXTENDED | STRING))
  {
   dsat_string_item_type *val_ptr = (dsat_string_item_type*) EXEC_VAL_PTR(parse_table);
   if(NULL != val_ptr)
   {
     *val_ptr = '\0';
    /* Success in execution of cmd*/
    return ( TRUE );
  }
    return ( FALSE );

  }

  /* deal with mixed parameter case */
  else if ( (parse_table->attrib & (CONFIG | MIXED_PARAM | LIST))
    == (CONFIG | MIXED_PARAM | LIST) )
  {
    return process_mixed_param_default_cmd(parse_table);
  }
  
  else
  {
    return ( FALSE );
  }

}/*process_default_cmd*/


/*===========================================================================

FUNCTION PROCESS_MIXED_PARAM_DEFAULT_CMD

DESCRIPTION
  This function is called by process_default_cmd
  It is called when the argument is of MIXED_PARAM type.

DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets default parameter value for the command

===========================================================================*/
LOCAL boolean process_mixed_param_default_cmd
(
  const dsati_cmd_type *parse_table /*  Ptr to cmd in parse table */
)
{
  int index;
  dsat_mixed_param_val_type * val_list = 
    (dsat_mixed_param_val_type *) EXEC_VAL_PTR(parse_table);

  mixed_def_s_type ** mixed_def_list = 
    (mixed_def_s_type **) parse_table->def_lim_ptr;

	  
  if(NULL == val_list)
  {
    return (FALSE);
  }

    /* For MIXED_PARAM parameters, we take action from individual
       argument.
       If the argument is a string, we set it to NULL.
       If the argument is an index into string list, we set to its default.
       If the argument is an integar with range, we set it to its default.
    */
  for (index = 0; index < parse_table->compound; index++)
  {
    /* The argument is an index into string list */
    if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
        (CONFIG | LIST) )
    {
      const def_list_type * def_list_ptr = 
        &mixed_def_list[index]->def->def_list;
      
      val_list[index].num_item = def_list_ptr->default_v;
    }

    /* The argument is an integar with range */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG) )
    {
      const dflm_type * dflm_ptr = &mixed_def_list[index]->def->dflm;
      
      val_list[index].num_item = dflm_ptr->default_v;
    }

    /* the argument is a string, set it to NULL */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
        (CONFIG | STRING) )
    {
      val_list[index].string_item[0] = '\0';
    }

    /* the argument is not anticipated */
    else
    {
      DS_AT_MSG3_ERROR("Mixed parameter default cmd %d, %dth param has wrong attrib %d",
          parse_table->name, index, mixed_def_list[index]->attrib);
      return FALSE;
    }

  }

  return TRUE;
} /* process_mixed_param_default_cmd */


/*===========================================================================

FUNCTION PROCESS_READ_CMD

DESCRIPTION
  This function is called by dsparmc_exec_parm_cmd.
  It is called when a command with READ operation is received

DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed
 
SIDE EFFECTS
  Sets response in supplied buffer for "READ" operation.
 
===========================================================================*/
LOCAL boolean process_read_cmd
(
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table */
  dsm_item_type * buf_ptr                        /*  Place to put response     */
)
{
  dsm_item_type * temp_buf_ptr;
  byte * rb_ptr;
  unsigned int return_val;

  temp_buf_ptr = buf_ptr;
  rb_ptr = temp_buf_ptr->data_ptr;

  if ( (parse_table->attrib & (CONFIG | STRING | MIXED_PARAM)) == CONFIG)
  {
    /*  Extended */
    if (parse_table->attrib & EXTENDED)
    {
      return_val = dsatutil_fmt_num_val_ex (parse_table, rb_ptr, RESPONSE);
      temp_buf_ptr->used = (word)return_val + 1;
      temp_buf_ptr->data_ptr[temp_buf_ptr->used]='\0';
      return ( TRUE );
    }
    /*  Single letter  */
    else
    {
      return_val = dsatutil_fmt_num_val (parse_table, rb_ptr);
      temp_buf_ptr->used = (word)return_val + 1;
      temp_buf_ptr->data_ptr[temp_buf_ptr->used]='\0';
      return ( TRUE );
    }
  }
  /*  String */
  else if ( (parse_table->attrib & (CONFIG | STRING | MIXED_PARAM)) ==
    (CONFIG | STRING))
  {
    return_val = dsatutil_fmt_string_val (parse_table, rb_ptr, RESPONSE);
    temp_buf_ptr->used = (word)return_val + 1;
    temp_buf_ptr->data_ptr[temp_buf_ptr->used]='\0';
    return ( TRUE );
  }
  
  /* deal with MIXED_PARAM */
  else if ( (parse_table->attrib & (MIXED_PARAM | LIST))
    == (MIXED_PARAM | LIST) )
  {
    return process_read_mixed_param_cmd(parse_table, buf_ptr);
  }

  else
  {
    return ( FALSE );
  }

} /* process_read_cmd */


/*===========================================================================

FUNCTION PROCESS_READ_MIXED_PARAM_CMD

DESCRIPTION
  This function is called by process_read_command
  It is called when a process_read_command process MIXED_PARAM type 
  parameters.

DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed
 
SIDE EFFECTS
  Sets response in supplied buffer for "READ" operation.
 
===========================================================================*/
LOCAL boolean process_read_mixed_param_cmd
(
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table */
  dsm_item_type * buf_ptr                        /*  Place to put response     */
)
{
  word written_size;

  written_size =
    (word) dsatutil_fmt_mixed_param_val (parse_table, buf_ptr, RESPONSE_ALL);
  
  if(written_size == 0)
  {
    return FALSE;
  }
  else
  {
    buf_ptr->used = (word) written_size;
  }
      
  return ( TRUE );
} /* process_read_mixed_param_cmd */


/*===========================================================================

FUNCTION PROCESS_WRITE_CMD

DESCRIPTION
  This function is called by dsparmc_exec_parm_cmd.
  It is called for standard processing of a command with an 
  argument (WRITE command)
  
DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets parameter value for "SET"  command

===========================================================================*/
LOCAL boolean process_write_cmd
(
  const tokens_struct_type *tok_ptr, /*Cmd tokens from parser*/
  const dsati_cmd_type *parse_table  /*Ptr to cmd in parse table*/
)
{
  if ( (parse_table->attrib & (CONFIG | STRING | LIST | MIXED_PARAM)) 
       == CONFIG)
  {
    if ( dsatutil_put_num_arg(parse_table, tok_ptr) )
    {
      return ( TRUE );
    }
    else 
    {
      return ( FALSE );
    }
  }
  else if ( (parse_table->attrib & (CONFIG | STRING | LIST | MIXED_PARAM))
            == (CONFIG | LIST) )
  {
    if ( dsatutil_put_num_arg(parse_table, tok_ptr) )
    {
      return ( TRUE );
    }
    else 
    {
      return ( FALSE );
    }
  }
  else if ( (parse_table->attrib & (CONFIG | STRING | LIST | MIXED_PARAM))
    == (CONFIG | STRING))
  {
    if ( dsatutil_put_string_arg (parse_table, tok_ptr, STORE) )
    {
      return ( TRUE );
    }
    else 
    {
      return ( FALSE );
    }
  }

  /* deal with MIXED_PARAM */
  else if ( (parse_table->attrib & (CONFIG | MIXED_PARAM | LIST))
    == (CONFIG | MIXED_PARAM | LIST) )
  {
    return process_write_mixed_param_cmd(tok_ptr, parse_table);
  }
  
  else
  {
    return ( FALSE );
  }
}/* process_write_cmd */


/*===========================================================================

FUNCTION PROCESS_WRITE_MIXED_PARAM_CMD

DESCRIPTION
  This function is called by process_write_cmd.
  It is called for process_write_cmd process MIXED_PARAM type parameters.
  
DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets parameter value for "SET"  command

===========================================================================*/
LOCAL boolean process_write_mixed_param_cmd
(
  const tokens_struct_type *tok_ptr, /*Cmd tokens from parser*/
  const dsati_cmd_type *parse_table  /*Ptr to cmd in parse table*/
)
{
  uint8 index;
  boolean hit = FALSE;
  param_srch_enum_type srchrc;
  dsat_mixed_param_val_type * val_list = 
    (dsat_mixed_param_val_type *)EXEC_VAL_PTR(parse_table);
	
  mixed_def_s_type ** mixed_def_list = 
    (mixed_def_s_type **) parse_table->def_lim_ptr;
	
  if(NULL == val_list)
  {
    return (FALSE);
  }
  
  
  if(tok_ptr->args_found > parse_table->compound)
  {
    return FALSE;
  }

  for (index = 0; index < tok_ptr->args_found; index++)
  {
    byte * token_arg_ptr = tok_ptr->arg[index];

    /* the index-th parameter is of type STRING, it has no 
       def values, save the token to the val */
    if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
        (CONFIG | STRING) )
    {
      dsat_string_item_type * val_string = 
        val_list[index].string_item;
      word val_len;
      byte * use_arg = token_arg_ptr;
      byte stripped_arg[MAX_LINE_SIZE];

      /* Do nothing if null parameter */
      if (0 != *use_arg)
      {
        val_len = mixed_def_list[index]->def->string_len;
        
        /* if the argument is quoted, strip out the quotes for internal storage
         * unless quoting attribute absent. */
        if(((( mixed_def_list[index]->attrib &
              ( CONFIG | STRING | YES_QUOTE )) == ( CONFIG | STRING | YES_QUOTE )
            ) ||
            ((mixed_def_list[index]->attrib &
              ( CONFIG | STRING | NO_QUOTE )) == ( CONFIG | STRING | NO_QUOTE )
            )
           ) && ('"' == *use_arg)
          )
        {
          if(!dsatutil_strip_quotes_out(token_arg_ptr, stripped_arg, MAX_LINE_SIZE))
          {
            return FALSE;
          }
          use_arg = stripped_arg;
        }
        
        /* check if the user input is too big */
        if(strlen((char *) use_arg) > val_len)
        {
          return FALSE;
        }
        
        /* The max length of the argument string is val_len, but the space
           allocated for the string is val_len + 1 bytes */
        (void) strlcpy((char *) val_string, (char *) use_arg,val_len +1);
      }
    }
      
    /* the index-th parameter is a string within a string list,
       we will determine its index into the string list.  Use
       this index to set the val_ptr */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG | LIST) )
    {
      dsat_num_item_type val_num;
      const def_list_type * def_list_ptr = &mixed_def_list[index]->def->def_list;
      srchrc = dsatutil_mixed_param_set_num_arg(&val_num, def_list_ptr,
                                            token_arg_ptr, 
                                                mixed_def_list[index]->attrib);
      /* Continue if parameter found */
      if (PARAM_NULL != srchrc)
      {
        if(PARAM_MISS == srchrc)
        {
          return FALSE;
        }
        val_list[index].num_item = val_num;
      }
    }
      
    /* the index-th parameter is a number with a range */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG) )
    {
      dsat_num_item_type * val_num_ptr = &val_list[index].num_item;
      const dflm_type * dflm_ptr = &mixed_def_list[index]->def->dflm;
      word radix;
      dsat_num_item_type arg_in_num;
      atoi_enum_type atoi_return;
        
      radix = ( (mixed_def_list[index]->attrib & HEX) ? 16 : 10);
      atoi_return = dsatutil_atoi (&arg_in_num, token_arg_ptr, radix);
        
      if (atoi_return == ATOI_OK)
      {
        /* Check to see if current parameter range is segmented */
        if (0 == (mixed_def_list[index]->attrib & BROKEN_RANGE))
        {
          /* Continuous range */
          if (arg_in_num < dflm_ptr->lower ||
              arg_in_num > dflm_ptr->upper)
          {
            DS_AT_MSG1_ERROR("The argument %d is out of range",arg_in_num);
            return FALSE;
          }
          else
          {
            *val_num_ptr = arg_in_num;
          }
        } 
        else
        {
          /* Discontinuous range so loop over each segment */
          const dflm_type *dflm_ptr_orig = dflm_ptr;
          while (MAX_BROKEN_RANGE != dflm_ptr->default_v)
          {
            if ((arg_in_num >= dflm_ptr->lower) &&
                (arg_in_num <= dflm_ptr->upper))
            {
              *val_num_ptr = arg_in_num;
              hit = TRUE;
              break;
            }
            dflm_ptr++;
            if ( (dflm_ptr - dflm_ptr_orig) >= 2)
            {
              /* Currently only CFUN command enters this loop
                It has only 2 valid broken range entries. */
              DS_AT_MSG0_ERROR("Currently this is not supported ");
              return FALSE;
            }
          }
          if (FALSE == hit)
          {
            DS_AT_MSG1_ERROR("The argument %d is out of range",arg_in_num);
            return FALSE;
          }
        }
      } 
      else if (atoi_return == ATOI_OUT_OF_RANGE)
      {
        DS_AT_MSG0_ERROR("Argument cannot be converted to number");
        return FALSE;
      }
      /*else if (e == ATOI_NO_ARG) do nothing: flag[j] remains unset  */
      /*    and the value is not stored                               */

    }

    else
    {
      DS_AT_MSG3_ERROR("Mixed parameter write cmd %d, %dth param has wrong attrib %d",
          parse_table->name, index, mixed_def_list[index]->attrib);
      return FALSE;
    }
  }
  return ( TRUE );
} /* process_write_mixed_param_cmd */


/*===========================================================================

FUNCTION PROCESS_TEST_CMD

DESCRIPTION
  This function is called by dsparmc_exec_parm_cmd.
  It is called when a command with TEST operation is received
  
DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets response in supplied buffer for "TEST" operation.

===========================================================================*/
LOCAL boolean process_test_cmd 
(
  const dsati_cmd_type *parse_table, /* Start of parse tbl    */
  dsm_item_type *buf_ptr                         /* Place to put response */
)
{
  dsm_item_type * temp_buf_ptr;
  byte * rb_ptr;
  unsigned int return_val;

  temp_buf_ptr = buf_ptr;
  rb_ptr = temp_buf_ptr->data_ptr;

  if ( (parse_table->attrib &
        (EXTENDED | CONFIG | STRING | MIXED_PARAM)) == (EXTENDED | CONFIG ))
  {
    return_val = dsatutil_fmt_num_range (parse_table, rb_ptr);
    temp_buf_ptr->used = (word)return_val + 1;
    temp_buf_ptr->data_ptr[temp_buf_ptr->used]='\0';
    return ( TRUE );
  }
  else if ( (parse_table->attrib & (EXTENDED | CONFIG | STRING | MIXED_PARAM)) ==
              (EXTENDED | CONFIG | STRING))
  {
    dsatutil_fmt_string_range (parse_table, rb_ptr, string_range);
    return ( TRUE );
  }

  /* deal with MIXED_PARAM */
  else if ( (parse_table->attrib & (MIXED_PARAM | LIST | LOCAL_TEST))
    == (MIXED_PARAM | LIST | LOCAL_TEST) )
  {
    return process_test_mixed_param_cmd(parse_table, buf_ptr);
  }

  else  /*  Command does not accept TEST  */
  {
    rb_ptr[0] = '\0';
    return ( FALSE );
  }

}/* process_test_cmd */


/*===========================================================================

FUNCTION PROCESS_TEST_MIXED_PARAM_CMD

DESCRIPTION
  This function is called by process_test_cmd.
  It is called when a process_test_cmd processes MIXED_PARAM type
  parameters.
  
DEPENDENCIES

RETURN VALUE
  Returns an boolean, indicating the result of the execution
    Returns FALSE: if there is any error in exection
    Returns TRUE: if the command has been successfully executed

SIDE EFFECTS
  Sets response in supplied buffer for "TEST" operation.

===========================================================================*/
LOCAL boolean process_test_mixed_param_cmd
(
  const dsati_cmd_type *parse_table, /* Start of parse tbl    */
  dsm_item_type *buf_ptr             /* Place to put response */
)
{
  dsm_item_type * temp_buf_ptr;
  byte * rb_ptr;

  int index;
  mixed_def_s_type ** mixed_def_list = 
    (mixed_def_s_type **) parse_table->def_lim_ptr;
  word written_size;

  temp_buf_ptr = buf_ptr;
  rb_ptr = temp_buf_ptr->data_ptr;

  /* put cmd_name: at the beginning of the response buffer */
  written_size = (word) snprintf((char *)rb_ptr,  
                           temp_buf_ptr->size - temp_buf_ptr->used,
                           "%s: ", parse_table->name);
  rb_ptr += written_size;
  temp_buf_ptr->used += written_size;

  for (index = 0; index < parse_table->compound; index++)
  {
    /* for string with LOCAL_TEST and YES_QUOTE */
    if( (mixed_def_list[index]->attrib & (STRING | LIST | LOCAL_TEST | YES_QUOTE)) == 
        (STRING | LOCAL_TEST | YES_QUOTE) )
    {
      *rb_ptr = '"';
      rb_ptr++;
      temp_buf_ptr->used++;
      *rb_ptr = '"';
      rb_ptr++;
      temp_buf_ptr->used++;
      *rb_ptr = ',';
      rb_ptr++;
      temp_buf_ptr->used++;
    }

    /* for string, leave it empty */
    else if( (mixed_def_list[index]->attrib & STRING) == 
              STRING )
    {
      *rb_ptr = ',';
      rb_ptr++;
      temp_buf_ptr->used++;
    }

    /* the index-th parameter is a string within a string list, the string
       list also contains the test string */
    else if( (mixed_def_list[index]->attrib & (STRING | LIST | LOCAL_TEST)) == 
             (LIST | LOCAL_TEST) )
    {
      const dsat_string_item_type * test_string =
        mixed_def_list[index]->def->def_list.list_t;

      written_size = (word) snprintf((char *)rb_ptr,
                                 temp_buf_ptr->size - temp_buf_ptr->used,
                                 "%s,", test_string);
      rb_ptr += written_size;
      temp_buf_ptr->used += written_size;
    }
      
    /* the index-th parameter is a number with a range, print its range */
    else if( (mixed_def_list[index]->attrib & (CONFIG | LIST | LOCAL_TEST)) == 
             (CONFIG | LOCAL_TEST) )
    {
      const dflm_type * dflm_ptr = &mixed_def_list[index]->def->dflm;
      const dflm_type * dflm_ptr_orig = dflm_ptr;

      if (0 == (mixed_def_list[index]->attrib & BROKEN_RANGE))
      {
        /* Continuous range processing */
        if(dflm_ptr->lower == dflm_ptr->upper)
        {
          written_size = (word) snprintf((char *)rb_ptr,
                                    temp_buf_ptr->size - temp_buf_ptr->used,
                                    "(%u),", dflm_ptr->lower);
        }
        else
        {
          written_size = (word) snprintf((char *)rb_ptr,
                                    temp_buf_ptr->size - temp_buf_ptr->used,
                                    "(%u-%u),",
                                    dflm_ptr->lower,
                                    dflm_ptr->upper);
        }

        rb_ptr += written_size;
        temp_buf_ptr->used += written_size;
      }
      else
      {
        /* Broken range processing */
        char buffer[40];

        DSM_PUSHDOWN_TAIL(buf_ptr, "(", 1);
        
        while (MAX_BROKEN_RANGE != dflm_ptr->default_v)
        {
          if(dflm_ptr->lower == dflm_ptr->upper)
          {
            written_size = (word) snprintf(buffer,sizeof(buffer),"%u,",
                                                 dflm_ptr->lower);
            DSM_PUSHDOWN_TAIL(buf_ptr, buffer, written_size);
          }
          else
          {
            written_size = (word) snprintf(buffer,sizeof(buffer),
                                                 "%u-%u,",
                                                 dflm_ptr->lower,
                                                 dflm_ptr->upper);
            DSM_PUSHDOWN_TAIL(buf_ptr, buffer, written_size);
          }
          dflm_ptr++;
          if ( (dflm_ptr - dflm_ptr_orig) >= 2)
          {
            /* Currently only CFUN command enters this loop
              It has only 2 valid broken range entries. */
            DS_AT_MSG0_ERROR("Currently this is not supported ");
            break;
          }
        }
        temp_buf_ptr->used--;  /* Overwrite trailing comma */ 
        DSM_PUSHDOWN_TAIL(buf_ptr, "),", 2);
        
        rb_ptr = (temp_buf_ptr->data_ptr + temp_buf_ptr->used);
      }
    }
    else if( (mixed_def_list[index]->attrib & LOCAL_TEST) == 0 )
    {
      /* Do nothing if LOCAL_TEST is not set */
    }
    else
    {
      DS_AT_MSG3_ERROR("Mixed parameter test cmd %p, %dth param has wrong attrib %d",
          parse_table, index, mixed_def_list[index]->attrib);
      return FALSE;
    }

  } /* for loop */
  
  /* get rid of the last ',' */
  temp_buf_ptr->used--;
      
  return ( TRUE );
}


/*===========================================================================

FUNCTION DO_SETTINGS_DUMP

DESCRIPTION
  This function is used by dsatparm_exec_ampv_cmd.
  This function gets a pointer to a AT commands table.
  It loops through the table and dumps the response in response buffer
  If the response buffer is full, then a new buffer is allocated and
  dsm_append is used to append the two buffers.

DEPENDENCIES
  None  

RETURN VALUE
  Pointer to dsm_item_type

SIDE EFFECTS
  dsm_append() is called to append multiple response buffers

===========================================================================*/
LOCAL dsm_item_type * do_settings_dump
(
  const dsati_cmd_type * table,         /*  Parse Table to dump       */
  const unsigned int sizeof_table,      /*  Size of Table             */
  unsigned int * char_per_line,         /*  Count of char across calls */
  dsm_item_type *res_buff_ptr           /*  Place to put response      */
)
{
  unsigned int i;
  dsm_item_type *cur_buff_ptr = res_buff_ptr;
  
  for (i = 0; i < sizeof_table; ++i)
  {
    /* Loop thru the table, and process all entries which have a 
       variable and whose NAME does not begin with '$'.  Proprietary 
       items are not dumped. */
    if ( (void*)dsatutil_get_val_ext(table[i].cmd_id,0,0,STR_TYPE) != NULL && (table + i)->name[0] != '$')
    {
      /* Write the formatted response in response buffer */
      if (cur_buff_ptr == NULL )
      {
        /* Some problem with DSM items, return immediately */
        break;
      }
      cur_buff_ptr = do_settings_action ( table, i, char_per_line,
                                          (dsm_item_type *)cur_buff_ptr );

    } /* if table != NULL */
  
  }/* for i */

  return cur_buff_ptr;

}/*do_settings_dump*/


/*===========================================================================

FUNCTION DO_SETTINGS_ACTION

DESCRIPTION
  This is a support function for DO_SETTINGS_DUMP.
  Takes the individual AT command, formats the response and dumps it
  in the reponse buffer.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to dsm_item_type

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsm_item_type * do_settings_action
(
  const dsati_cmd_type * table,         /* Parse Table to dump             */
  unsigned int index,                   /* Size of Table                   */
  unsigned int * char_per_line,         /* Used to count char across calls */
  dsm_item_type * res_buff_ptr          /* Place to put response           */
)
{
  unsigned int return_val = 0;
  unsigned int cnt = 0;
  unsigned int multi_index_tab_length = 0;
  unsigned int offset = 1;
  word * used_ptr;
  byte * buff_ptr;
  dsm_item_type *cur_buff_ptr = res_buff_ptr;
  dsm_item_type *prv_buff_ptr = res_buff_ptr;
  
  static const struct
  {
    byte name[10];
    dsat_num_item_type (*fmt_func)( const struct dsati_cmd_struct*,
                                    dsm_item_type* const*,
                                    multi_index_fmt_e_type );
  } multi_index_response_fmt_table[] =
  {
#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_DSAT_ETSI_DATA

    { "+CGDCONT", dsatetsipkt_cgdcont_response_fmt },
#ifdef FEATURE_SECONDARY_PDP
    { "+CGDSCONT",  dsatetsipkt_cgdscont_response_fmt },
    { "+CGTFT",     dsatetsipkt_cgtft_response_fmt },
#endif /* FEATURE_SECONDARY_PDP */
#ifdef FEATURE_DATA_WCDMA_PS
    { "+CGEQREQ", dsatetsipkt_cgeqreq_response_fmt },
    { "+CGEQMIN", dsatetsipkt_cgeqmin_response_fmt },
#endif /* FEATURE_DATA_WCDMA_PS */
#ifdef FEATURE_DSAT_LTE
    { "+CGEQOS",  dsatetsipkt_cgeqos_response_fmt },
#endif /* FEATURE_DSAT_LTE */
    { "+CGQREQ",  dsatetsipkt_cgqreq_response_fmt },
    { "+CGQMIN",  dsatetsipkt_cgqmin_response_fmt },
    { "+CGACT",   dsatetsipkt_cgact_response_fmt },
#endif /* FEATURE_DSAT_ETSI_DATA */

    { "+CUSD",    dsatetsicall_cusd_response_fmt },
#endif /* FEATURE_DSAT_ETSI_MODE */

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
    { "+CRLP",    dsatetsicall_crlp_response_fmt },
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */
    { "", NULL }
  };

  multi_index_tab_length = sizeof(multi_index_response_fmt_table)/sizeof(multi_index_response_fmt_table[0]);

  /* Advance DSM pointer to last packet in chain */
  while( NULL != cur_buff_ptr->pkt_ptr )
  {
    cur_buff_ptr = cur_buff_ptr->pkt_ptr;
  }
  
  /* Check to see if a new DSM item is required */
  cur_buff_ptr = dsatutil_append_dsm_item(cur_buff_ptr,
                                          DSM_BUFFER_THRESHOLD);
  if (prv_buff_ptr != cur_buff_ptr)
  {
    /* On DSM switch, start with space to allow linefeed insert */
    cur_buff_ptr->data_ptr[cur_buff_ptr->used++] = ' '; 
  }

  
  buff_ptr = &cur_buff_ptr->data_ptr[cur_buff_ptr->used];
  used_ptr = &cur_buff_ptr->used;

  /*-------------------------------------------------------------------------
      Process the table item, using the command attribute to determine
      the format.
   ----------------------------------------------------------------------*/
  if ( (table + index)->attrib & MULTI_INDEX )
  {
    /* Multiple parameter case */

    /* Lookup command-specific response formatting function */
    while (cnt < multi_index_tab_length)
    {
      if(0 == dsatutil_strcmp_ig_sp_case(
                multi_index_response_fmt_table[cnt].name,
                (const byte *)(table + index)->name))
      {
        /* Save current buffer */
        prv_buff_ptr = cur_buff_ptr;

        /* Call response format function */
        return_val = multi_index_response_fmt_table[cnt].fmt_func (
                       (table + index),
                       &cur_buff_ptr,
                       MULTI_INDEX_AMPV);
        
        /* Advance DSM pointer to last packet in chain */
        while( NULL != cur_buff_ptr->pkt_ptr )
        {
          cur_buff_ptr = cur_buff_ptr->pkt_ptr;
        }

        /* Check for DSM item change; adjust local pointers if so */
        if (prv_buff_ptr != cur_buff_ptr)
        {
          buff_ptr = &cur_buff_ptr->data_ptr[cur_buff_ptr->used];
          used_ptr = &cur_buff_ptr->used;

          /* Force a new line; bypass subsequent pagination logic */
          *buff_ptr++ = '\n';
          *used_ptr += 1; 
          *char_per_line = 0;
          return_val = 0;
          offset = 0;
        }
        break;
      }
      cnt++;
    }
  }
  else if ( (table + index)->attrib & MIXED_PARAM )
  {
    /* Mixed parameter case */
    return_val = (unsigned int)
      dsatutil_fmt_mixed_param_val (table + index, cur_buff_ptr, RESPONSE_ALL);
    
  }
  else if ( (table + index)->attrib & STRING)
  {
    /* String case */
    return_val = dsatutil_fmt_string_val(table + index,
                                         buff_ptr, RESPONSE_ALL);
  }
  else
  {
    /* Numeric case */
    return_val = dsatutil_fmt_num_val_ex(table + index,
                                         buff_ptr, RESPONSE_ALL);
  }

  *char_per_line += return_val;
  buff_ptr += return_val;

  /*-------------------------------------------------------------------------
      If the current item causes the output line to be longer than
      72 characters, replace the "; " preceding it with <CR><LF>
  -------------------------------------------------------------------------*/
  if (*char_per_line > 72)
  {
    *(buff_ptr-(return_val+offset)) = '\n';
    *char_per_line = return_val;
    offset = 1;
  }
  *used_ptr += (word)return_val;
  
  /*-------------------------------------------------------------------------
      Terminate the current item with "; ".
  -------------------------------------------------------------------------*/
  *buff_ptr++ = ';';
  *buff_ptr++ = ' ';
  *used_ptr += 2;
  *char_per_line += 2;
  
  return cur_buff_ptr;
}/* do_settings_action */

