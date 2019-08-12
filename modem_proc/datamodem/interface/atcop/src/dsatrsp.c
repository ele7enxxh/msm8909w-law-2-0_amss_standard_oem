/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
          A T   C O M M A N D   R E S P O N S E  G E N E R A T O R

GENERAL DESCRIPTION
  This module provides the functionality to build, format, and send
  AT command responses to the appropriate destination; TE or PS
  protocol stack.

EXTERNALIZED FUNCTIONS
  dsat_fmt_response
    Formats the response from an AT command, and from the line as
    a whole, according to the values of the ATE and ATV parameters.

  dsatrsp_send_intermediate_result_code
    Sends ITU-T V.25 ter basic code or mode specific extended result code
    to TE or PS protocol stack based on response routing provided if ATQ
    is 0. Also, modifies ATCOP internal state variables.  This function
    should be used when the result code to be sent is an intermediate code.
    That is, one that occurs before the final AT response is sent when 
    the call terminates such as with a T.30 FAX call which sends multiple
    AT result codes throughout the duration of the call.

  dsatrsp_send_result_code
    Sends ITU-T V.25 ter basic code or mode specific extended result code
    to TE or PS protocol stack based on response routing provided if ATQ
    is 0. Also, modifies ATCOP internal state variables.
    
  dsat_send_dr_result
    Sends intermediate data compression reporting result code to TE or
    PS protocol stack based on response routing provided by response
    router.
    
  dsatrsp_report_rate_on_connect
    Writes the value to the current rate variable dsat_report_rate_val. 
    If dsat_x_val, set by the ATX command, is greater than 0.this rate will 
    be reported with the next CONNECT result. The dsat_report_rate_val 
    variable will be reset to 0 on the next call to 
    dsat_sio_is_free(). 

EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT
  dsatrsp_send_response
    Sends AT response based on routing function registered or to TE
    if no function is registerd.  Routes response to TE or PS protocol
    stack based on operating service mode and AT command state.
    
  dsatrsp_fmt_result_code
    Places a formatted result code in the output buffer.  The type of
    result code is identified in the input parameter.  Formatting is
    based on whether or not the mode is verbose.

  dsatrsp_set_basic_result_code
    Produces and unformatted ITU-T V.25 ter basic result code and
    returns in DSM item.

  dsatrsp_send_cell_result
    Sends ITU-T V.25 ter basic code or mode specific extended result code
    to TE or PS protocol stack based on response routing provided if ATQ
    is 0.

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2015 by Qualcomm Technologies, Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatrsp.c_v   1.3   07 Nov 2002 16:20:12   wxie  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatrsp.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/14   pg      Fixed CSD call port none issue.
07/31/14   pg      Fixed MOB failures in 3GPP2.
06/27/14   tk      Added support for Dynamic ATCoP.
04/17/14   sc      Fixed ATCoP KW errors.
03/27/14   pg      Fixed NOCARRIER response for command issue.
03/14/14   tk      Optimized debug macros usage in ATCoP.
20/01/13   pg      Fixed C+G MT call crash on G subs.
03/27/13   tk      Fixed issues in dual stack commands.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/22/12   sk      Mainlining Nikel Changes.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   sk      Adding support for forwarding CFUN.
01/19/12   sk      Feature cleanup.
05/26/11   dvk     Global Variables Cleanup
04/20/11   bs      NIKEL Phase I support.
03/03/11   nc      Fixed CLCC for Fusion.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/07/10   ad      Added MSG_HIGH for command response.
07/23/10   nc      Added 1x SMS Support for FUSION Architecture.
05/10/10   kk      Added support for ^HC SMS commands. 
12/15/09   nc      Featurisation changes for LTE.
04/29/09   ua      Fixed compiler warnings. 
04/20/09   sa      Fixed intermediate response issue.
01/20/09   nc      Fixed ATD Response issue for a CSD call.
12/12/08   ua      Fixed Off target lint errors.
10/23/08   bs      Added support for 1X AT Phonebook commands.
09/30/08   ua      Extending the "connect rate" for 1x data calls. Supported rates
                   for 1x RTT, HDR REV0, HDR REVA.
05/26/08   sa      Correcting CONNECT result code display (MultiPort Issue).
01/04/08   bs      Corrected "connect rate" reporting during CONNECT.
11/24/07   ua      Fixed Lint-lows. 
10/17/07   ua      Modified dsat_report_rate_on_connect() to report data rate 
                   for CS data and PS data.
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
09/27/07   sa      Modified dsat_send_dr_result not to change ds3g_siolib_active_port.
08/28/07   sa      Attempt to allocate first large DSM item or on failing that
                   try small item in dsat_fmt_response().
05/09/07   az      Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X
05/02/07   ua      Fixed lint medium errors. 
04/12/07   ua      Corrected DR result code to send it to answerport
                   when auto answer is choosen. 
01/29/06   ua      Added support for GSM 7bit '@' character (0x00).
                   Extended support for 0x00 - 0x7F  charactes in SMS
02/26/07   sa      Fixed issues related to MULTIPORT support.
02/15/07   ss      Fixed lint high errors
01/30/07   ss      Replaced banned string API calls
05/01/06   vp      Split result codes into DSAT_ERROR and others for serializer
                   in dsatrsp_send_result_code().
10/02/05   snb     Changes to implement a second layer of flow control, 
                   ATCOP to USB, to avoid exhaustion of DSM large items.
09/07/05   ar      Add call response mode handling.
04/15/05   sb      Fixed lint errors
04/05/05   sb      When Serializer feature define is enabled, use different
                   ds3gsiolib_setup_tx() function.
03/15/05   sb      Fixes from testing the serializer changes.
02/10/05   sb      Accessories related changes.
09/03/04   snb     Add +CHLD support.
06/16/04   snb     Add function for sending intermediate result code to 
                   correct problem with fax calls.
05/24/04   snb     Correct low and high water callbacks.
11/14/03   snb     Add support for CSD call data rate reporting on CONNECT.
07/11/03   snb     Correct feature definitions.
05/28/03   sb      Fix for WCDMA CS Calls
05/01/03   sb      modified dsat_send_result_code() to handle ATH command.
04/28/03   sb      AT command flow control. Removed FEATURE_DSAT_FLOW_CONTROL
02/26/03   wx      Put dialed number into last dial ph book.
07/13/02   atp     Added function for ability to register format response.
                   In dsat_fmt_result_code(), added check for registered
                   function, and if registered calls it instead of default.
08/10/01   sjd     Initial release to MSM5200 archives.

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

#include "target.h"
#include "err.h"
#include "dsati.h"
#include "dsatctab.h"
#include "dsatcmif.h"
#include "dsm.h"
#include "sio.h"
#include "ds3gsiolib.h"
#include "amssassert.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#include "dsatetsictab.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DSAT_MDM_FUSION
#include "ds707_rmsm_proxy.h"
#endif /* FEATURE_DSAT_MDM_FUSION */


/*===========================================================================

                         EXTERNAL DECLARATIONS

===========================================================================*/

#ifdef FEATURE_DSAT_EXTENDED_SMS
extern boolean dsat_unicode_response;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

#ifdef FEATURE_DSAT_MDM_FUSION
LOCAL dsm_item_type * atproxy_dsm_item_ptr = NULL;
LOCAL dsm_item_type *proxy_res_buff_ptr = NULL;
#endif /* FEATURE_DSAT_MDM_FUSION */

/* Pointer to function defining how responses should be routed. */
extern dsat_routing_func_type route_at_cmd_rsp_func_ptr;

/* Pointer to function defining how responses should be formated.          */
extern dsat_fmting_func_type fmt_at_cmd_rsp_func_ptr;

/* dsati_free_item_timer_expired_handler() will call this on timer expiration
   if not NULL. Handlers for PB entry/SMS reads can set this function to
   reenter processing following delay to allow DSM items to be freed by USB
   task. */

dsati_async_event_handler_type dsati_handle_free_item_timer_expiry = NULL;

dsat_connect_rate_s_type dsat_report_rate_val = {0};

/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/



/*-------------------------------------------------------------------------
    Prototypes for local functions:
-------------------------------------------------------------------------*/
LOCAL dsm_item_type *set_result_code
(
  dsat_result_enum_type result_code             /* Result code             */
);

LOCAL dsat_result_enum_type dsat_fmt_ext_result_code
(
  dsm_item_type *res_buff_ptr
);

#ifdef FEATURE_DSAT_MDM_FUSION
LOCAL dsat_result_enum_type parse_proxy_result
(
  dsm_item_type * proxy_res_buff 
);
#endif /* FEATURE_DSAT_MDM_FUSION */

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/


/*===========================================================================

FUNCTION  DSATRSP_SEND_RESPONSE 

DESCRIPTION
  Sends AT response based on routing function registered or to TE
  if no function is registered.  Routing function routes response to TE
  or PS protocol stack based on operating service mode and AT command
  state.

DEPENDENCIES
  Routing is only performed if routing function is registered, otherwise
  response is sent to TE.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_response
(
  dsm_item_type *rsp_ptr,                 /* Ptr to response buffer to send */
  const boolean rearm_autodetect          /* Flag to rearm SIO autodetect */
)
{
#define MAX_RESPONSE_BUF 100
  byte response[MAX_RESPONSE_BUF] = {0};
  ASSERT( rsp_ptr != NULL );
  /* Display Final response as OK /ERROR or some byte of output
                      app_field distinguish between response and echo*/
  if(rsp_ptr->app_field != 1)
  {

    (void) dsatutil_memscpy((void*)response,
            MAX_RESPONSE_BUF,(void*)rsp_ptr->data_ptr,
                         ((rsp_ptr->used < MAX_RESPONSE_BUF)?(rsp_ptr->used):(MAX_RESPONSE_BUF -1)));
    response[MAX_RESPONSE_BUF -1] = '\0'; 
    DS_AT_MSG_SPRINTF_1_HIGH(
                   "Command output %s", response);
  }
  else
  {
     rsp_ptr->app_field = 0;
     /*Make flag false to remove future impact of this flag*/
  }
  
  if ( route_at_cmd_rsp_func_ptr != NULL )
  {
    /* Routing function is registered, so send response based on routing
       function. */
    route_at_cmd_rsp_func_ptr( rsp_ptr, dsatcmdp_at_state, rearm_autodetect );
  }
  else
  {
    /* No routing function is registered, so send response to SIO. */

    ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );
    (void) ds3g_siolib_setup_at_tx( rsp_ptr, rearm_autodetect );
  }

} /* dsatrsp_send_response( ) */


/*===========================================================================

FUNCTION DSAT_FMT_RESPONSE

DESCRIPTION
  This function formats the response according whether or not
  the mode is verbose and whether or not the response is a result
  code or informational text.  

  The dsm item passed in as in_ptr_ptr can contain chained dsm items with
  the dsm "used" value defining the length of each item.  These chained
  items are assumed to contain a NULL terminated string, which will be
  formatted up until the dsm "used" value is reached for each buffer
  or a NULL character is encountered, whichever occurs first.  *in_ptr_ptr
  will be returned as NULL to indicate the dsm input items have been freed.

  The type of data passed in the dsm item pointed to by in_ptr_ptr is
  specified by the type_of_rsp parameter.  This controls response
  formatting and identifies if header, trailer, both header and trailer,
  or no header and trailer should be added to the response.  This allows
  formatting of partial responses, as oppossed to only supporting
  formatting of complete responses, to reduce DSM buffer usage. See enum
  type definition for details.

  The dsm item passed in as out_ptr_ptr is filled with the formatted
  response.  If there is not enough room in this dsm item, the full item
  is queued to send as a command line response and additional dsm
  items are allocated, with out_ptr_ptr pointing to the last item
  allocated.  out_ptr_ptr may be returned as NULL, if no more dsm
  buffers are available. 
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_fmt_response
(
  dsm_item_type **in_ptr_ptr,         /* Response to format in chained
                                         buffers                           */
  dsat_rsp_enum_type type_of_rsp,     /* Type of response data in 
                                         in_ptr_ptr                        */
  boolean result_code,                /* Flag to indicate if response is a
                                         result code or information
                                         response                          */
  dsm_item_type **out_ptr_ptr         /* Location to store formatted
                                         response, output will NOT be NULL
                                         terminated                        */
)
{
  byte    c;                     /* Location to store character            */
  dsat_num_item_type temp_s3_val;
  dsat_num_item_type temp_s4_val;
#ifdef FEATURE_DSAT_EXTENDED_SMS
  byte    p = '\0';              /* Location to store previour character   */
#endif /*FEATURE_DSAT_EXTENDED_SMS */
  byte   *src_ptr;               /* Pointer to source (input) buffer       */
  byte   *src_end_ptr;           /* Pointer to end of source data          */
  byte   *dest_ptr;              /* Pointer to destination (output) buffer */
  byte   *dest_end_ptr;          /* Pointer to end of destination buffer   */
  boolean write_data;            /* Flag to indicate write to destination  */
  boolean header;                /* Flag to write header                   */
  boolean trailer;               /* Flag to write trailer                  */
  dsm_item_type *in_ptr;         /* Pointer to input dsm item              */
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
  dsat_num_item_type cscs_val ;
#endif /* FEATURE_DSAT_ETSI_MODE */
  /*-------------------------------------------------------------------------
    If input pointer is NULL, there is nothing to do so exit.
  -------------------------------------------------------------------------*/
  if ( in_ptr_ptr == NULL || *in_ptr_ptr == NULL )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Initialize pointer to input dsm item and verify output pointer is
    valid.  Exit if output pointer is invalid.
  -------------------------------------------------------------------------*/
  in_ptr = *in_ptr_ptr;
  
  /* Input item is always freed, so initialize the return value here. */
  *in_ptr_ptr = NULL;

  ASSERT( out_ptr_ptr != NULL && *out_ptr_ptr != NULL );  
  
  /*-------------------------------------------------------------------------
    Initialize header and trailer flags.
  -------------------------------------------------------------------------*/
  switch ( type_of_rsp )
  {
  case DSAT_START_OF_RSP:
    header  = TRUE;
    trailer = FALSE;
    break;

  case DSAT_END_OF_RSP:
    header  = FALSE;
    trailer = TRUE;
    break;

  case DSAT_INTERMEDIATE_RSP:
#ifdef FEATURE_DSAT_MDM_FUSION
  case DSAT_PROXY_RSP:
#endif /* FEATURE_DSAT_MDM_FUSION */
    header = trailer = FALSE;
    break;

  case DSAT_COMPLETE_RSP:
    header = trailer = TRUE;
    break;
    
  default:
    header = trailer = TRUE;                  /* Make lint happy. */      
    DS_AT_MSG0_ERROR("Invalid type of response.");
    break;
  }
  
  /*-------------------------------------------------------------------------
      Initialize source data pointers.
  -------------------------------------------------------------------------*/
  src_ptr = in_ptr->data_ptr;
  src_end_ptr = &in_ptr->data_ptr[in_ptr->used];

  /*-------------------------------------------------------------------------
    Initialize destination pointers.
  -------------------------------------------------------------------------*/
  dest_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->used];
  dest_end_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->size];
  ASSERT( dest_ptr < dest_end_ptr );

  /*-------------------------------------------------------------------------
    Format Response and write to output (destination) buffer.  Allocate
    new output buffers as necessary. Handle chained dsm items for source
    data.  
  -------------------------------------------------------------------------*/
  for ( write_data = TRUE; write_data; )
  {
    /* Check to see if we have enough room in the destination
       buffer for one more execution of this loop and the addition
       of a trailer after the loop, and if not allocate a new one.
       Worst case a two byte header, two bytes for a newline, and
       a two byte trailer could be placed in the buffer. */
    if ( dest_ptr >= (dest_end_ptr - 6) )
    {
      /* Update the number of bytes used in output buffer. */
      (*out_ptr_ptr)->used += (word) (dest_ptr -
                       &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->used]);

      /* Queue formatted response. */
      dsatcmdp_queue_cmd_results( *out_ptr_ptr );      

      /* Check in both large and small item pool for a new dsm item */ 
      *out_ptr_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
      if (NULL == *out_ptr_ptr)
      {
        *out_ptr_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      }

      /* Initialize destination pointers. */
      dest_ptr = (*out_ptr_ptr)->data_ptr;
      dest_end_ptr = &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->size];
    }

    /* Determine if we need to write response header at beginning of
       buffer. */
    if ( header )
    {
      /* Write header if verbose mode. */
      header = FALSE;

      if ( (dsat_num_item_type)dsatutil_get_val(
                                  DSAT_BASIC_V_IDX,0,0,NUM_TYPE) == 1 )
      {
         temp_s3_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
         temp_s4_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
        *dest_ptr++  = (byte) temp_s3_val;
        *dest_ptr++  = (byte) temp_s4_val;
      }
    }

    /* Copy a byte of source data to destination buffer. */

    /* Source is a dsm item with possible chained buffers.  The
       dsm "used" value or a NULL character determines the end of
       data in the buffer, whichever occurs first. */

    if ( src_ptr < src_end_ptr )
    {
      /* There is more data in source buffer so copy data. */
      c = *src_ptr++;
      if ( c == '\0' 
#ifdef FEATURE_DSAT_EXTENDED_SMS
          && (!dsat_unicode_response) 
#endif /* FEATURE_DSAT_EXTENDED_SMS */
         )

      {
        /* Assume end of source data string because of NULL termination. */
        write_data = FALSE;
        dsm_free_packet( &in_ptr );
      }
      else if ( c == '\n'
#ifdef FEATURE_DSAT_EXTENDED_SMS
               && (p != '\r') && (!dsat_unicode_response) 
#endif /* FEATURE_DSAT_EXTENDED_SMS */
                )     
      {
        /* Format text for newline */
       temp_s3_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
       temp_s4_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
      *dest_ptr++  = (byte) temp_s3_val;
      *dest_ptr++  = (byte) temp_s4_val;

      }
      else
      {
        /* Copy source data to destination. */
#ifdef FEATURE_DSAT_ETSI_MODE
        cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
        if (IS_ETSI_MODE(current_mode) && 
            ( cscs_val == (dsat_num_item_type)ALPHA_GSM ) && ( c == GSM_AT_CHAR_REPLACEMENT ))
        {
          /* When sending out to terminal, replace the special char of '@'
          to its original value 0x00 */
          *dest_ptr++ = GSM_AT_CHAR;
        }
        else
#endif /* FEATURE_DSAT_ETSI_MODE */
        {
          *dest_ptr++ = c;
#ifdef FEATURE_DSAT_EXTENDED_SMS
          p = c;
#endif /* FEATURE_DSAT_EXTENDED_SMS */
        }
      }
    }
    else
    {
      /* End of dsm source buffer reached, so free buffer and check
         for another buffer in chain. */
      in_ptr = dsm_free_buffer( in_ptr );
      if ( in_ptr != NULL )
      {
        /* Another buffer in chain, so initialize source pointers. */
        src_ptr = in_ptr->data_ptr;
        src_end_ptr = &in_ptr->data_ptr[in_ptr->used];
      }
      else
      {
        /* End of source data, no more buffers in chain. */
        write_data = FALSE;
      }
    }
    
  } /* end for( ) */

  /*-------------------------------------------------------------------------
    Add trailer to destination buffer if a trailer should be added.
    Note, there is room for a two byte trailer in the destination buffer
    because this was verified in the previous for( ) loop.
  -------------------------------------------------------------------------*/
  if ( trailer == TRUE )
  {
    /* Set up trailer: <CR> always, <LF> if verbose or re. */
    temp_s3_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
    *dest_ptr++ = (byte) temp_s3_val;
    if ( (dsat_num_item_type)dsatutil_get_val(
                   DSAT_BASIC_V_IDX,0,0,NUM_TYPE)== 1 || !result_code )
    {
      temp_s4_val = (dsat_num_item_type)dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
      *dest_ptr++ = (byte) temp_s4_val;
    }
  }

#ifdef FEATURE_DSAT_EXTENDED_SMS
  dsat_unicode_response = FALSE;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

  /*-------------------------------------------------------------------------
    Update the number of bytes used in output buffer.
  -------------------------------------------------------------------------*/
  (*out_ptr_ptr)->used += (word) (dest_ptr -
        &(*out_ptr_ptr)->data_ptr[(*out_ptr_ptr)->used]);

} /* dsat_fmt_response( ) */


/*===========================================================================

FUNCTION DSATRSP_FMT_RESULT_CODE

DESCRIPTION
  Places a formatted result code in the output buffer.  The type of
  result code is identified in the input parameter.  Formatting is
  based on whether or not the mode is verbose.  The result codes
  generated are based on mode specific result code generating
  functions.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_fmt_result_code
(
  dsat_result_enum_type result_code,   /* Result code                      */
  dsm_item_type **out_ptr_ptr          /* Location to store formatted
                                          response, output will NOT be NULL
                                          terminated                       */
)
{
  dsm_item_type *raw_rsp_ptr;

    raw_rsp_ptr = set_result_code(result_code);
    if ( raw_rsp_ptr != NULL &&
         result_code == DSAT_CONNECT &&
         (dsat_num_item_type)dsatutil_get_val(
          DSAT_BASIC_X_IDX,0,0,NUM_TYPE) > 0 )
    {
      /* Format the extended result code based on settings of \V and &E*/
      (void)dsat_fmt_ext_result_code( raw_rsp_ptr );
    }
    if ( fmt_at_cmd_rsp_func_ptr != NULL )
    {
      /*-------------------------------------------------------------------
        If mode-specific handler has registered a function to format
        responses, it should be invoked when in ONLINE DATA mode. This
        is needed for IS-707 async reflected AT cmds which need to format
        responses with 617 codes.
      -------------------------------------------------------------------*/
      fmt_at_cmd_rsp_func_ptr (
                                  &raw_rsp_ptr,
                                  DSAT_COMPLETE_RSP,
                                  TRUE,
                                  out_ptr_ptr,
                                  result_code
                              );
    }
    else
    {
      dsat_fmt_response(
                         &raw_rsp_ptr,
                         DSAT_COMPLETE_RSP,
                         TRUE,
                         out_ptr_ptr
                       );
    }
} /* dsatrsp_fmt_result_code( ) */

/*===========================================================================

FUNCTION DSATRSP_SET_BASIC_RESULT_CODE

DESCRIPTION
  Produces and unformatted ITU-T V.25 ter basic result code and returns
  in DSM item.  Result code returned will be either numeric or verbose,
  based on input parameter.

DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to DSM item containing unformatted basic result code.

SIDE EFFECTS
  None
  
===========================================================================*/
dsm_item_type *dsatrsp_set_basic_result_code
(
  dsat_result_enum_type result_code,    /* Basic result code               */
  boolean               verbose         /* verbose result code or numeric  */
)
{
  int index = 0;
  dsat_result_enum_type basic_result_code;
  char *result_code_str;
  dsm_item_type *raw_rsp_ptr;
  byte *tmp_ptr;
#ifdef FEATURE_DSAT_GOBI_MAINLINE
  sys_modem_as_id_e_type  subs_id = dsat_get_current_subs_id(FALSE);
  cm_num_s_type call_info;
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
  /* Basic result codes specified in ITU-T V.25 ter. */
  static const struct
  {
    dsat_result_enum_type code;
    char *str_ptr;
  } result_code_table[ ] =
  {
    {DSAT_OK,             "OK"}, 
    {DSAT_CONNECT,        "CONNECT"},   
    {DSAT_RING,           "RING"},   
    {DSAT_NO_CARRIER,     "NO CARRIER"},
    {DSAT_ERROR,          "ERROR"},     
    {DSAT_NO_DIALTONE,    "NO DIALTONE"},
    {DSAT_BUSY,           "BUSY"},
    {DSAT_NO_ANSWER,      "NO ANSWER"}, 
    {DSAT_DELAYED,        "DELAYED"},
    {DSAT_MAX_BASIC,      NULL}
  };
    
  /* Search the basic result code table for a match. */
  do
  {
    basic_result_code = result_code_table[index].code;
    result_code_str = result_code_table[index++].str_ptr;
  } 
  while ( (basic_result_code != result_code) && 
          (result_code_str != NULL )
        );

  if ( result_code_str == NULL )
  {
    DS_AT_MSG1_ERROR("Incorrect basic result code %d", result_code);
    return NULL;
  }

  /* Get item for unformatted raw response. */
  raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
  if ( NULL != raw_rsp_ptr )
  {
    /* Generate verbose or numeric basic result code. */
    if ( verbose )
    {
      /* Generate verbose ITU-T V.25 ter result code. */
      raw_rsp_ptr->used = (word) snprintf( (char*) raw_rsp_ptr->data_ptr,
                                                  raw_rsp_ptr->size, "%s",
                                                  result_code_str );

#ifdef FEATURE_DSAT_GOBI_MAINLINE
      memset(&call_info,0,sizeof(call_info));
      dsatcmif_cm_get_incoming_call_num(subs_id, &call_info);
      if( call_info.buf[0]!= '\0')
      {
        raw_rsp_ptr->used = (word) strlcat( (char*) raw_rsp_ptr->data_ptr,
                                                "\nIncoming call number:",
                                                raw_rsp_ptr->size );
        raw_rsp_ptr->used = (word) strlcat( (char*) raw_rsp_ptr->data_ptr,
                                                (char*)call_info.buf,
                                                raw_rsp_ptr->size );
      }
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
    }
    else
    {
      /* Generate numeric ITU-T V.25 ter result code. */
      uint32 result_code_basic = (uint32) ((int32)basic_result_code );
      
      tmp_ptr = dsatutil_itoa( result_code_basic,
                               raw_rsp_ptr->data_ptr, 10 );
      raw_rsp_ptr->used = (uint16) ( ( tmp_ptr - raw_rsp_ptr->data_ptr ) + 1);
    }
  }

  return raw_rsp_ptr;

} /* dsatrsp_set_basic_result_code( ) */

/*===========================================================================

FUNCTION DSATRSP_SEND_CELL_RESULT

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatrsp_send_cell_result
(
  dsat_result_enum_type result_code         /* Command result code */
)
{
  dsm_item_type *fmt_rsp_ptr;

  if ( (dsat_num_item_type)dsatutil_get_val(
                           DSAT_BASIC_Q_IDX,0,0,NUM_TYPE) == 0 )
  {
    /* Result codes are not suppressed so send result code. */

    /* Get buffer to hold formatted response. */
    fmt_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    /* Format the result code and place in response buffer. */
    dsatrsp_fmt_result_code( result_code, &fmt_rsp_ptr );

    /* Send the formatted result code. */
    if ( fmt_rsp_ptr != NULL )
    {
      ds3g_siolib_tx_e_type rsp_type;
      ds3g_siolib_port_e_type port = DS3G_SIOLIB_PORT_NONE;
      
      switch (result_code) 
      {
        case DSAT_RING:
          /* Specify the tx_type before sending result code                */
          /* for RING, we need to process differently in siolib */
          rsp_type = (DSAT_IS_CALL_MODE( DSAT_CALL_MODE_FAX ))?
                     DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE :
                     DS3G_SIOLIB_UNSOLICTED_AT_ERROR_CODE;
          ds3g_siolib_set_tx_type( rsp_type, DS3G_SIOLIB_DATA_PORT); 
          break;
        case DSAT_OK:
        case DSAT_NO_CARRIER:
        case DSAT_ERROR:
        case DSAT_BUSY:
        case DSAT_NO_ANSWER:
        case DSAT_NO_DIALTONE:
        case DSAT_CONNECT:
          /* get the active port id */
          port = ds3g_siolib_get_active_port();
          if ( port == DS3G_SIOLIB_PORT_NONE )
          {
            /* Send result to all port */
            port= DS3G_SIOLIB_DATA_PORT;   
          }
          /* Specify the rsp_type before sending result code */
          rsp_type = (DSAT_IS_CALL_MODE( DSAT_CALL_MODE_FAX ))?
                      DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE :
                      DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE;
          ds3g_siolib_set_tx_type( rsp_type, port );
          
          break;
        
        default:
          /* Specify the tx_type before sending result code */
          rsp_type = (DSAT_IS_CALL_MODE( DSAT_CALL_MODE_FAX ))?
                    DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE :
                    DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE;
          ds3g_siolib_set_tx_type( rsp_type, DS3G_SIOLIB_DATA_PORT );
          break;
      }

      dsatrsp_send_response( fmt_rsp_ptr, FALSE );
    }
  }

} /* dsatrsp_send_cell_result( )  */
/*===========================================================================

FUNCTION DSATRSP_SEND_CELL_RESULT_EX

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0.
  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatrsp_send_cell_result_ex
(
  dsat_result_enum_type result_code,         /* Command result code */
  ds3g_siolib_port_e_type port /*Port Id*/
)
{
  dsm_item_type *fmt_rsp_ptr;

  if ( (dsat_num_item_type)dsatutil_get_val(
                           DSAT_BASIC_Q_IDX,0,0,NUM_TYPE) == 0 )
  {

            if (port == DS3G_SIOLIB_PORT_NONE)
            {
              port= DS3G_SIOLIB_DATA_PORT;
       DS_AT_MSG0_ERROR("Port is None setting it to Data Port");
   }
    /* Result codes are not suppressed so send result code. */

    /* Get buffer to hold formatted response. */
    fmt_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
    if (NULL == fmt_rsp_ptr)
    {
      return;
    }

    /* Format the result code and place in response buffer. */
    dsatrsp_fmt_result_code( result_code, &fmt_rsp_ptr );

    /* Send the formatted result code. */
    if ( fmt_rsp_ptr != NULL )
    {
      ds3g_siolib_tx_e_type rsp_type;
      
      switch (result_code) 
      {
        case DSAT_RING:
          /* Specify the tx_type before sending result code                */
          /* for RING, we need to process differently in siolib */
          rsp_type = (DSAT_IS_CALL_MODE( DSAT_CALL_MODE_FAX ))?
                     DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE :
                     DS3G_SIOLIB_UNSOLICTED_AT_ERROR_CODE;
          ds3g_siolib_set_tx_type( rsp_type, DS3G_SIOLIB_DATA_PORT); 
          break;
        case DSAT_OK:
        case DSAT_NO_CARRIER:
        case DSAT_ERROR:
        case DSAT_BUSY:
        case DSAT_NO_ANSWER:
        case DSAT_NO_DIALTONE:
        case DSAT_CONNECT:

          /* Specify the rsp_type before sending result code */
          rsp_type = (DSAT_IS_CALL_MODE( DSAT_CALL_MODE_FAX ))?
                      DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE :
                      DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE;
          ds3g_siolib_set_tx_type( rsp_type, port );
          
          break;
        
        default:
          /* Specify the tx_type before sending result code */
          rsp_type = (DSAT_IS_CALL_MODE( DSAT_CALL_MODE_FAX ))?
                    DS3G_SIOLIB_UNSOLICTED_FAX_RESULT_CODE :
                    DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE;
          ds3g_siolib_set_tx_type( rsp_type, DS3G_SIOLIB_DATA_PORT );
          break;
      }
      dsatrsp_send_response( fmt_rsp_ptr, FALSE );
    }
  }
} /* dsatrsp_send_cell_result_ex( )  */


#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT

/*===========================================================================

FUNCTION DSATRSP_SEND_CELL_RESULT_TO_EXT_CLIENT

DESCRIPTION
  This function is identical to dsatrsp_send_cell result but sends the result code
  only to external clients registered through SW API.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_cell_result_to_ext_client
(
  dsat_result_enum_type result_code         /* Command result code */
)
{
  dsm_item_type *fmt_rsp_ptr;
  ds3g_siolib_status_e_type ret_val;
  if ( (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_BASIC_Q_IDX,0,0,NUM_TYPE) == 0 )
  {
    /* Result codes are not suppressed so send result code. */

    /* Get buffer to hold formatted response. */
    fmt_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

    /* Format the result code and place in response buffer. */
    dsatrsp_fmt_result_code( result_code, &fmt_rsp_ptr );

    /* Send the formatted result code. */
    if ( fmt_rsp_ptr != NULL )
    {
      dsatclient_send_response(fmt_rsp_ptr,&ret_val,TRUE);
    }
  }

}/*dsatrsp_send_cell_result_to_ext_client*/
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT*/
/*===========================================================================
FUNCTION  DSATRSP_SEND_INTERMEDIATE_RESULT_CODE
DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.  This function
  should be used when the result code to be sent is an intermediate code.
  That is, one that occurs before the final AT response is sent when 
  the call terminates such as with a T.30 FAX call which sends multiple
  AT result codes throughout the duration of the call.
  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.
  Extended cellular result codes are returned if AT+CRC is 1.
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsatrsp_send_intermediate_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
)
{
  /* Indicate abort command processing has completed if not disabled.
     This function performs no action if abort command processing is
     not currently in progress. */
  if ( dsatcmdp_end_abort_in_send_rsp )
  {
    dsatcmdp_command_abort_complete( );
  }

  /* Format and send result code */
  dsatrsp_send_cell_result( result_code );
    
  /* If result code is not CONNECT, clear the regional block indications
     flag to indicate indications can now be sent to the TE. */
  if ( result_code != DSAT_CONNECT )
  {
    dsatcmdp_set_block_indications_all_ports(FALSE);
  }
} /* dsatrsp_send_intermediate_result_code( )  */


/*===========================================================================

FUNCTION  DSATRSP_SEND_RESULT_CODE

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsatrsp_send_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
)
{
  /* If we are processing ATH command we are supposed to continue
     processing the command line. Check to see if we are processing 
     ATH. If so then brach out from here. This function sends the 
     result code but also continues processing the AT command line.
     Final result code is sent out after processing the command line. */
  if (dsati_processing_ath_cmd)
  {
    dsat_finished_ath_cmd (result_code);
    return; /* Need not execute rest of the function */
  }

  /* Indicate abort command processing has completed if not disabled.
     This function performs no action if abort command processing is
     not currently in progress. */
  if ( dsatcmdp_end_abort_in_send_rsp )
  {
    dsatcmdp_command_abort_complete( );
  }

  /* Format and send result code */
  dsatrsp_send_cell_result( result_code );

  /* If result code is not CONNECT, clear the regional block indications
     flag to indicate indications can now be sent to the TE. */
  if ( result_code != DSAT_CONNECT )
  {
    dsatcmdp_set_block_indications_all_ports(FALSE);
  }
} /* dsatrsp_send_result_code( )  */
/*===========================================================================

FUNCTION  DSATRSP_SEND_RESULT_CODE_EX

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
void dsatrsp_send_result_code_ex
(
  dsat_result_enum_type result_code,         /* Command result code */
  ds3g_siolib_port_e_type port /*Port Id*/
  
)
{
  if( DS3G_SIOLIB_PORT_NONE == port )
  {
    port = ds3g_siolib_get_active_port();
    if( DS3G_SIOLIB_PORT_NONE == port )
    {
      DS_AT_MSG1_ERROR("Invalid port %d",port);
      return;
    }
  }
  /* If we are processing ATH command we are supposed to continue
     processing the command line. Check to see if we are processing 
     ATH. If so then brach out from here. This function sends the 
     result code but also continues processing the AT command line.
     Final result code is sent out after processing the command line. */
  if (dsati_processing_ath_cmd)
  {
    dsat_finished_ath_cmd (result_code);
    return; /* Need not execute rest of the function */
  }

  /* Indicate abort command processing has completed if not disabled.
     This function performs no action if abort command processing is
     not currently in progress. */
  if ( dsatcmdp_end_abort_in_send_rsp )
  {
    dsatcmdp_command_abort_complete( );
  }

  /* Format and send result code */
  dsatrsp_send_cell_result_ex( result_code,port );

  /* If result code is not CONNECT, clear the regional block indications
     flag to indicate indications can now be sent to the TE. */
  if ( result_code != DSAT_CONNECT )
  {
    dsatcmdp_set_block_indications(port, FALSE);
  }
} /* dsatrsp_send_result_code_ex( )  */

/*===========================================================================

FUNCTION DSAT_FMT_EXT_RESULT_CODE

DESCRIPTION
  This function formats the extended result code after CONNECT. This should be 
  called when &X is greater than 0. 
  \V=0, &E=0 CONNECT <radio speed>
  \V=0, &E=1 CONNECT <DTE speed>
  \V=1, &E=0 CONNECT <radio speed> <communication protocol> <text>
  \V=1, &E=1 CONNECT <DTE speed> <communication protocol> <text>
  <text> is applicable only for PS calls. 
        
DEPENDENCIES
  None
  
RETURN VALUE
  NONE

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type dsat_fmt_ext_result_code
(
  dsm_item_type *raw_rsp_ptr
)
{
#define ENABLE_EXT_RESULT_CODE 1
#define AMP_E_DTE_RATE 1
#define AMP_E_RADIO_RATE 0
#define BYTES_TO_KB_FACTOR 1000

  byte rate_str[11]={0};
  const char comm_prot_val[5][10]={"NONE","PPPoverUD", "AV32K", "AV64K", "PACKET"};

  if ( raw_rsp_ptr == NULL )
  {
    return DSAT_ERROR;
  }
  /* convert data rate integer */
  /* &E1 the data rate should be the serial connection rate
     &E0 the data rate is the wireless connection speed 
  */
  if ( (dsat_num_item_type)dsatutil_get_val(
                 DSAT_BASIC_N_E_IDX,0,0,NUM_TYPE) == AMP_E_RADIO_RATE )
  {
    if (dsat_report_rate_val.call_type == DSAT_CALL_TYPE_CS )
    {
      (void)dsatutil_itoa( dsat_report_rate_val.rate.cs_call.waiur,
                     rate_str,
                     10 );
    }
    else if ( dsat_report_rate_val.call_type == DSAT_CALL_TYPE_PS )
    {
      /*For \V =0 and &E = 0 only one radio rate should be displayed
        and for PS calls displaying only DL rate */
      (void)dsatutil_itoa( dsat_report_rate_val.rate.ps_call.data_rate,
                     rate_str,
                     10 );
    }
    else if ( dsat_report_rate_val.call_type == DSAT_CALL_TYPE_1X )
    {
      (void)dsatutil_itoa( dsat_report_rate_val.rate.cdma_call.data_rate,
                     rate_str,
                     10 );
    }
    else
    {
      DS_AT_MSG0_ERROR("Invalid call type for data rate ");
    }
  }
  else if ( (dsat_num_item_type)dsatutil_get_val(
                   DSAT_BASIC_N_E_IDX,0,0,NUM_TYPE) == AMP_E_DTE_RATE )
  {
    /* DTE rate is from +IPR value 
    NOTE: No need to check the validity of dsat_ipr_val as it is
    properly checked and updated everywhere */
    (void)strlcpy((char*)rate_str, 
                      (char*)dsat_ipr_valstr[
                      (dsat_num_item_type)dsatutil_get_val(
                       DSAT_EXT_IPR_IDX,0,0,NUM_TYPE)],sizeof(rate_str));

  } 
  /* append <speed> to CONNECT */
  raw_rsp_ptr->used += 
    (word)snprintf((char*)&raw_rsp_ptr->data_ptr[raw_rsp_ptr->used], 
                        raw_rsp_ptr->size - raw_rsp_ptr->used,
                          " %s",(char*) rate_str );
  /* Extended result code is 
  <CONNECT> <speed> <communication protocol> [<text>])*/
  if ( (dsat_num_item_type)dsatutil_get_val(
        DSAT_BASIC_DS_V_IDX,0,0,NUM_TYPE) == ENABLE_EXT_RESULT_CODE )
  {
    /* append <communication protocol> to CONNECT */
    raw_rsp_ptr->used += 
      (word)snprintf((char*)&raw_rsp_ptr->data_ptr[raw_rsp_ptr->used], 
                            raw_rsp_ptr->size - raw_rsp_ptr->used,
                            " %s",(char*) comm_prot_val[dsat_report_rate_val.comm_prot]);
    /* [<text>] is applicable only for PS calls */
    if ( dsat_report_rate_val.call_type == DSAT_CALL_TYPE_PS )
    {
      /* append <text> to CONNECT 
      <text>: APN/UL/DL. UL/DL should be in KB 
      NOTE:Included quotes for APN just to understand when there 
           is no APN given */
      raw_rsp_ptr->used += 
        (word)snprintf((char*)&raw_rsp_ptr->data_ptr[raw_rsp_ptr->used], 
                              raw_rsp_ptr->size - raw_rsp_ptr->used,
#ifdef FEATURE_DSAT_ETSI_MODE
                              " \"%s\"/%d/%d",(char*)dsat_report_rate_val.rate.ps_call.apn,
#else
                              " \"\"/%d/%d",
#endif /* FEATURE_DSAT_ETSI_MODE */
                              (int)dsat_report_rate_val.rate.ps_call.ul_rate/BYTES_TO_KB_FACTOR,
                              (int)dsat_report_rate_val.rate.ps_call.dl_rate/BYTES_TO_KB_FACTOR );
    }
  }
  return DSAT_OK;
}/* dsat_fmt_ext_result_code */
#if ( defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) )

/*===========================================================================

FUNCTION  DSAT_SEND_DR_RESULT

DESCRIPTION
  Sends intermediate data compression reporting result code to TE or
  PS protocol stack based on response routing provided by response
  router.

  Should be called from mode specific protocol stack when compression
  negotiation completed and before connect result code sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsat_send_dr_result
(
  dsat_dr_mode_e_type compression          /*  Compression mode            */
)
{
  /* The table of return_code_text is according to
     V.25ter series V section 6.6.2
     Data Compression reporting control +DR */
  LOCAL const struct 
  {
    dsat_dr_mode_e_type return_code;
    char * return_code_text;
  } return_code_table[] = 
  {
    { DSAT_DR_MODE_NONE,     "NONE"},
    { DSAT_DR_MODE_BOTH,     "V42B"},
    { DSAT_DR_MODE_RD,       "V42B RD"},
    { DSAT_DR_MODE_TD,       "V42B TD"},
    { DSAT_DR_MODE_MAX,      NULL }
  };
  
  /* the service reporting is done only when +CR set to be 1 */
  if((dsat_num_item_type)dsatutil_get_val(
                         DSAT_EXT_DR_IDX,0,0,NUM_TYPE) == 1)
  {
    int index = 0;
    char * return_code_string;

    dsm_item_type *raw_rsp_ptr, *formatted_rsp_ptr;
    char * buff_ptr;

    /* search through the serv_table for serv_text */
    do
    {
      return_code_string = return_code_table[index].return_code_text;
    } while (return_code_table[index++].return_code != compression &&
             return_code_string != NULL);

    if(return_code_string == NULL)
    {
      DS_AT_MSG1_ERROR("Incorrect return code type %d", compression);
      return;
    }

    /* use dsm buffer to output results */
    raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
    if (NULL != raw_rsp_ptr)
    {
      buff_ptr = (char *) raw_rsp_ptr->data_ptr;
      (void)snprintf(buff_ptr, raw_rsp_ptr->size,
                           "+DR: %s", return_code_string);
      raw_rsp_ptr->used = (word)strlen(buff_ptr) + 1;

      formatted_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
    
      /* format the result code and send response */
      dsat_fmt_response( &raw_rsp_ptr, DSAT_COMPLETE_RSP, TRUE,
                         &formatted_rsp_ptr );

      if (NULL != formatted_rsp_ptr)
      {
        /* If auto answer is set and DR is enabled, then there wont be 
           a valid active port to send the DR result, get the port 
           from the auto answer port */
        if ( ds3g_siolib_get_active_port() == DS3G_SIOLIB_PORT_NONE )
        {
          /* If the accessory feature is enabled, inform SIOLIB which port 
           this should go to */
          ds3g_siolib_set_tx_type( DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE, 
            ds3g_siolib_get_answer_port()); 
        }
        dsatrsp_send_response( formatted_rsp_ptr, FALSE );
      }
    }
  }

} /* dsat_send_dr_result( ) */
/*===========================================================================

FUNCTION  DSATRSP_SEND_DR_RESULT_EX

DESCRIPTION
  Sends intermediate data compression reporting result code to TE or
  PS protocol stack based on response routing provided by response
  router.

  Should be called from mode specific protocol stack when compression
  negotiation completed and before connect result code sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatrsp_send_dr_result_ex
(
  dsat_dr_mode_e_type compression,          /*  Compression mode            */
  ds3g_siolib_port_e_type port /*Port Id*/
)
{
  /* The table of return_code_text is according to
     V.25ter series V section 6.6.2
     Data Compression reporting control +DR */
  LOCAL const struct 
  {
    dsat_dr_mode_e_type return_code;
    char * return_code_text;
  } return_code_table[] = 
  {
    { DSAT_DR_MODE_NONE,     "NONE"},
    { DSAT_DR_MODE_BOTH,     "V42B"},
    { DSAT_DR_MODE_RD,       "V42B RD"},
    { DSAT_DR_MODE_TD,       "V42B TD"},
    { DSAT_DR_MODE_MAX,      NULL }
  };
  
  /* the service reporting is done only when +CR set to be 1 */
  if((dsat_num_item_type)dsatutil_get_val(
                         DSAT_EXT_DR_IDX,0,0,NUM_TYPE) == 1)
  {
    int index = 0;
    char * return_code_string;

    dsm_item_type *raw_rsp_ptr, *formatted_rsp_ptr;
    char * buff_ptr;

    /* search through the serv_table for serv_text */
    do
    {
      return_code_string = return_code_table[index].return_code_text;
    } while (return_code_table[index++].return_code != compression &&
             return_code_string != NULL);
  
    if(return_code_string == NULL)
    {
      DS_AT_MSG1_ERROR("Incorrect return code type %d", compression);
      return;
    }
  
    /* use dsm buffer to output results */
    raw_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

    buff_ptr = (char *) raw_rsp_ptr->data_ptr;
            (void)snprintf(buff_ptr, raw_rsp_ptr->size,
                         "+DR: %s", return_code_string);
    raw_rsp_ptr->used = (word)strlen(buff_ptr) + 1;

    formatted_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

    /* format the result code and send response */
    dsat_fmt_response( &raw_rsp_ptr, DSAT_COMPLETE_RSP, TRUE,
                       &formatted_rsp_ptr );

      /* If the accessory feature is enabled, inform SIOLIB which port 
       this should go to */
      ds3g_siolib_set_tx_type( DS3G_SIOLIB_UNSOLICTED_DS_RESULT_CODE, port); 
    dsatrsp_send_response( formatted_rsp_ptr, FALSE );
  }

} /* dsatrsp_send_dr_result_ex( ) */

#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */


/*===========================================================================

FUNCTION DSATRSP_REPORT_RATE_ON_CONNECT

DESCRIPTION
  This function is called by other modules to write the value to the current 
  rate variable dsat_report_rate_val. If dsat_x_val, set by the ATX command,
  is greater than 0 this rate will be reported with the next CONNECT result
  and during the call with any CONNECT result due to re-entry of online data
  mode. The dsat_report_rate_val variable will be reset to 0
  on the next call to dsat_sio_is_free(). 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies the value of the dsat_report_rate_val.
===========================================================================*/
void dsatrsp_report_rate_on_connect
(
  dsat_connect_rate_s_type connect_rate   /* Rate to report with next CONNECT */
)
{
#define CBST_FNUR_116 116  /* CBST Fixed network data rate speed 116 */
#define CBST_FNUR_134 134  /* CBST Fixed network data rate speed 134 */

  memset( &dsat_report_rate_val, 0, sizeof( dsat_report_rate_val ) );

  if ( connect_rate.call_type == DSAT_CALL_TYPE_PS )
  {
    /* Update the recieved data rate in bytes to KB*/
    dsat_report_rate_val.call_type = DSAT_CALL_TYPE_PS;
    dsat_report_rate_val.rate.ps_call.dl_rate = 
                         connect_rate.rate.ps_call.dl_rate;
    dsat_report_rate_val.rate.ps_call.ul_rate = 
                         connect_rate.rate.ps_call.ul_rate;
   /* radio zone speed is max of UL and DL */
    dsat_report_rate_val.rate.ps_call.data_rate = 
        MAX ( dsat_report_rate_val.rate.ps_call.dl_rate, 
              dsat_report_rate_val.rate.ps_call.ul_rate );
#ifdef FEATURE_DSAT_ETSI_MODE
(void) dsatutil_memscpy((void*)dsat_report_rate_val.rate.ps_call.apn,
        DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1,(void*)connect_rate.rate.ps_call.apn,
        DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1);
#endif /* FEATURE_DSAT_ETSI_MODE */
    dsat_report_rate_val.comm_prot = DSAT_COMM_PROT_PACKET;

    DS_AT_MSG3_HIGH("PS call rate updated UL: %d DL: %d radio zone speed: %d",
        dsat_report_rate_val.rate.ps_call.dl_rate,
        dsat_report_rate_val.rate.ps_call.ul_rate, 
        dsat_report_rate_val.rate.ps_call.data_rate);
  }
  else if ( connect_rate.call_type == DSAT_CALL_TYPE_CS )
  {
    dsat_report_rate_val.call_type = DSAT_CALL_TYPE_CS;
    dsat_report_rate_val.rate.cs_call.waiur = connect_rate.rate.cs_call.waiur;
    dsat_report_rate_val.rate.cs_call.fnur  = connect_rate.rate.cs_call.fnur;
    dsat_report_rate_val.rate.cs_call.name  = connect_rate.rate.cs_call.name;
    dsat_report_rate_val.rate.cs_call.conn_elem = connect_rate.rate.cs_call.conn_elem;
    /* Currently, only two communication protocols are supported
       PPPoverUD(1) ->  fnur=116, name=1, conn_elem=0
       AV64K(3)     ->  fnur=134, name=1, conn_elem=0
    */
    if ( dsat_report_rate_val.rate.cs_call.name == 1 &&
         dsat_report_rate_val.rate.cs_call.conn_elem == 0 )
    {
      if ( dsat_report_rate_val.rate.cs_call.fnur == CBST_FNUR_116 )
      {
        dsat_report_rate_val.comm_prot = DSAT_COMM_PROT_PPPOVERUD;
      }
      else if ( dsat_report_rate_val.rate.cs_call.fnur == CBST_FNUR_134 )
      {
        dsat_report_rate_val.comm_prot = DSAT_COMM_PROT_AV64K;
      }
    }
    DS_AT_MSG2_HIGH("CS call rate updated: COMM Protocol: %d data rate %d",
                                  dsat_report_rate_val.comm_prot,
                                  dsat_report_rate_val.rate.cs_call.waiur);
  }
  else if ( connect_rate.call_type == DSAT_CALL_TYPE_1X )
  {
    dsat_report_rate_val.call_type = DSAT_CALL_TYPE_1X;
    dsat_report_rate_val.rate.cdma_call.ul_rate = 
                         connect_rate.rate.cdma_call.ul_rate;
    dsat_report_rate_val.rate.cdma_call.dl_rate = 
                         connect_rate.rate.cdma_call.dl_rate;
    dsat_report_rate_val.rate.cdma_call.data_rate = 
        MAX ( dsat_report_rate_val.rate.cdma_call.dl_rate, 
              dsat_report_rate_val.rate.cdma_call.ul_rate );
  }
  else
  {
    DS_AT_MSG0_ERROR("Invalid call type for connect rate updation");
  }
  
}/* dsatrsp_report_rate_on_connect */

/*===========================================================================

FUNCTION SET_RESULT_CODE

DESCRIPTION
  Produces ITU-T V.25 ter basic code or ETSI extended format result code
  if ATQ is 0 and returns in DSM item.
  
  If AT+CRC is 0, only the basic result codes are returned and extended
  RING codes are translated to the basic RING code.

  If AT+CRC is 1, the extendted format RING <type> is returned.
                  <type> is defined in Section 6.11 of 3GPP TS 27.007

  NOTE: This function should be removed to an ETSI specific file at a
        later date.  SJD_HACK
        
DEPENDENCIES
  None
  
RETURN VALUE
  Pointer to DSM item containing result code.

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsm_item_type *set_result_code
(
  dsat_result_enum_type result_code             /* Result code             */
)
{
  dsm_item_type *raw_rsp_ptr = NULL;
  dsat_num_item_type temp_v_val;

  if ( (dsat_num_item_type)dsatutil_get_val(
                                   DSAT_BASIC_Q_IDX,0,0,NUM_TYPE)== 0 )
  {
    /* Result codes are not suppressed so display result code. */

    if ( result_code < DSAT_MAX_BASIC )
    {
      /* Basic result code */
      temp_v_val = (dsat_num_item_type) dsatutil_get_val(
                                      DSAT_BASIC_V_IDX,0,0,NUM_TYPE);
      raw_rsp_ptr = dsatrsp_set_basic_result_code( 
                                      result_code,
                                     (boolean) temp_v_val);
    }
    /* Removed ETSI condition check as we already checking sysmode in calling function */
    else
    {
      /* Mode specific result code */
      raw_rsp_ptr = dsatetsicall_set_crc_result( result_code );
    }
  }
  
  return raw_rsp_ptr;
  
} /* etsi_set_result_code( ) */

/*===========================================================================

FUNCTION  DSATI_FREE_ITEM_TIMER_EXPIRED_HANDLER

DESCRIPTION
  This is the handler function for expiration of timer waiting for DSM large
  items to be freed by USB when listing many PB entries/SMSes.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD: if the handler succeeds.
  DSAT_ERROR    : if the handler fails.

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsati_free_item_timer_expired_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_timer_id_e_type *timer_id_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  if ( (unsigned int)DSAT_TIMER_FREE_ITEM == *timer_id_ptr  &&
       NULL != dsati_handle_free_item_timer_expiry )
  {
    return dsati_handle_free_item_timer_expiry( cmd_ptr );
  }
  else
  {
    DS_AT_MSG1_ERROR("Free item timer handler incorrectly called: timer ID - %d", 
               *timer_id_ptr );
    return DSAT_ASYNC_EVENT;
  }
} /* dsati_free_item_timer_expired_handler */

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DSATRSP_PROCESS_PROXY_RESPONSE

DESCRIPTION
  This functions receives the fully formatted response from Proxy interface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
void dsatrsp_process_proxy_response
(
  dsm_watermark_type *wm_item_ptr       /* Response Watermark pointer */
)
{
#define FIRST_DSM (1)
   static boolean sms_entry = FALSE; 
   boolean send_rsp = FALSE;
   uint32  size = 0;
   
   /* Initialize result code to invalid, For valid result code, first 
    * byte would indicate OK/ERROR/CME/CMS/NO CARRIER/RING and second byte 
    * would indicate CME or CMS error codes if applicable 
    */
   dsat_result_enum_type result_code = DSAT_MAX_BASIC;
   uint16 dsm_item_no = 0;

  while ( (atproxy_dsm_item_ptr != NULL) || 
          (atproxy_dsm_item_ptr = dsm_dequeue( wm_item_ptr ) )!= NULL )
  {
    size = atproxy_dsm_item_ptr->used;
    DS_AT_MSG1_HIGH("Dequeued %d bytes from Response Watermark pointer",size);

    if( send_rsp == TRUE )
    {
      DS_AT_MSG1_ERROR("Data of size %d received after End Of Response",
        size);
      dsm_free_packet(&atproxy_dsm_item_ptr);
      continue;
    }

    DS_AT_MSG3_HIGH("dsat_process_proxy_response: DSM 3 bytes %c%c%c",
                                 atproxy_dsm_item_ptr->data_ptr[0],
                                 atproxy_dsm_item_ptr->data_ptr[1],
                                 atproxy_dsm_item_ptr->data_ptr[2]);

    if(++dsm_item_no == FIRST_DSM)
    {
      result_code = parse_proxy_result(atproxy_dsm_item_ptr);

      if(result_code == DSAT_SMS_PROMPT )
      {
        /* All data following PROMPT, should be Echoed to the terminal */
        sms_entry = TRUE;
      }
      else if( result_code == DSAT_SMS_EOM )
      {
        /* Echoing stops after EOM (This can be received after CTRL+Z or 
           after a ESC character) */
        sms_entry = FALSE;
      }

      if( result_code == DSAT_SMS_PROMPT )
      {
        dsm_item_type * res_buff = NULL;
        res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
        /* Send the Prompt across */
        res_buff->used = 
               strlcpy((char *)res_buff->data_ptr,"\r\n> ",
                                 res_buff->size);
        dsatrsp_send_response(res_buff , FALSE );
        atproxy_dsm_item_ptr = dsm_free_buffer(atproxy_dsm_item_ptr);
        continue;
      }

      if( result_code == DSAT_SMS_EOM )
      {
        dsm_item_type * res_buff = NULL;

        res_buff = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
        /* Send the EOM across */
        res_buff->used = 
               strlcpy((char *)res_buff->data_ptr,"\r\n",
                                 res_buff->size);
        dsatrsp_send_response(res_buff , FALSE );
        atproxy_dsm_item_ptr = dsm_free_buffer(atproxy_dsm_item_ptr);

        /* Reset the Result code so that 
              Parser would send the Final Result code */
        result_code = DSAT_MAX_BASIC;
        continue;
      }

      if(result_code == DSAT_CONNECT)
      {
         /* Inform RM-SM Proxy here */
         ds707_rmsm_proxy_atd_connect_cb();
         atproxy_dsm_item_ptr = dsm_free_buffer(atproxy_dsm_item_ptr);
         dsatcmdp_processing_async_cmd = FALSE;
         /* When a command is in processing there is a possibility of
            QCSIMAPP value to be updated. */
         dsat_update_qcsimapp_val();
         dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE,
                                  ds3g_siolib_get_ds3g_siolib_active_port());
         continue;
      }

      if((result_code == DSAT_NO_CARRIER)||(result_code == DSAT_ERROR))
      {
        /* Check if it is ATD command in execution */
        if(dsat_proxy_curr_abort_handler != NULL)
        {
          dsat_proxy_curr_abort_handler(result_code);
          atproxy_dsm_item_ptr = dsm_free_buffer(atproxy_dsm_item_ptr);
          dsatcmdp_processing_async_cmd = FALSE;
          /* When a command is in processing there is a possibility of
             QCSIMAPP value to be updated. */
          dsat_update_qcsimapp_val();
          dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE,
                                   ds3g_siolib_get_ds3g_siolib_active_port() );
          continue;
        }
      }
    }/* if(++dsm_item_no == FIRST_DSM) */

    /* If result not updated, see if OK is received */
    if(result_code == DSAT_MAX_BASIC)
    {
      uint16 ok_string_location = atproxy_dsm_item_ptr->used - 4;
      if ( dsatutil_strncmp_ig_sp_case(
        (const byte*)&atproxy_dsm_item_ptr->data_ptr[ok_string_location],
           "OK",2)==0)
      {
        result_code = DSAT_OK;
        size = atproxy_dsm_item_ptr->used - 6;
      }
    }
    /* Echo back all the data between PROMPT and EOM */
    if( sms_entry == TRUE )
    {
      dsm_item_type * res_buff = NULL;
      res_buff = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      res_buff->used = 0;
      (void) dsatutil_memscpy((void*)res_buff->data_ptr,
              res_buff->size,(void*)atproxy_dsm_item_ptr->data_ptr,
              atproxy_dsm_item_ptr->used);
      res_buff->used = atproxy_dsm_item_ptr->used;

      dsatrsp_send_response(res_buff, FALSE );
      atproxy_dsm_item_ptr = dsm_free_buffer(atproxy_dsm_item_ptr);
      continue;
    }

    /* Response population */
    if((result_code == DSAT_OK)||
       (result_code == DSAT_CMD_ERR_RSP)||
       (result_code == DSAT_MAX_BASIC))
    {
      if ( proxy_res_buff_ptr == NULL )
      {
        proxy_res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      }

      /* pushdown the dsm items received to make up the complete response */
      dsat_dsm_pushdown_tail(&proxy_res_buff_ptr,
                             atproxy_dsm_item_ptr->data_ptr,
                             size,
                             DSM_ITEM_POOL(proxy_res_buff_ptr),
                             FALSE);
    }
    atproxy_dsm_item_ptr = dsm_free_buffer(atproxy_dsm_item_ptr);
      
    /* If result code is updated, then End of Response is received */
    if(result_code != DSAT_MAX_BASIC)
    {
      send_rsp = TRUE;
    }
    
    DS_AT_MSG1_HIGH("Response from MSM dequeued so far is of size %d",
                                      dsm_length_packet(proxy_res_buff_ptr));
  }/* while */

  /* When it is end of response send the response to TE as a normal async 
     procedure*/
  if ( (send_rsp == TRUE) && (sms_entry == FALSE) )
  {
    if ( !dsatcmdp_processing_async_cmd )
    {
      if ( proxy_res_buff_ptr != NULL )
      {
        dsm_free_packet( &proxy_res_buff_ptr );
      }
      DS_AT_MSG0_HIGH("Unexpected Data in watermark received!");
      return;
    }
  
    DS_AT_MSG0_HIGH("Prepared the final proxy response ");
    if ( (result_code == DSAT_OK) || (result_code == DSAT_CMD_ERR_RSP) )
    {
      uint32 cmd_attrib = dsat_get_forward_cmd_attrib();
      /* If the Response received from MSM for a COMMON_CMD or if the MSM response 
         is not set to be appended Ignore the Response */
      if(((COMMON_CMD & cmd_attrib) == 0) || ( TRUE == dsat_get_append_rsp_state() ))
      {
        DS_AT_MSG2_HIGH("Sending MSM response - %d attr - app_rsp_state %d",cmd_attrib,
                                 dsat_get_append_rsp_state());
        dsat_set_append_rsp_state(FALSE);
        dsatcmdp_handle_async_cmd_rsp(proxy_res_buff_ptr, DSAT_PROXY_RSP );
      }
      else
      {
        DS_AT_MSG2_HIGH("Ignoring the MSM response - %d attr - app_rsp_state %d ",
                                 cmd_attrib,dsat_get_append_rsp_state());
        if ( proxy_res_buff_ptr != NULL )
        {
          dsm_free_packet( &proxy_res_buff_ptr );
        }
      }
      proxy_res_buff_ptr = NULL;
    }
    
    dsatcmdp_send_proxy_response(result_code);
  }
  else
  {
    DS_AT_MSG0_HIGH("Watermarks gone empty but EOR not received");
  }
}/* dsatrsp_process_proxy_response */

/*===========================================================================

FUNCTION DSATRSP_PROCESS_PROXY_URC

DESCRIPTION
  This function process the data received from the other Processor and packs 
  the unsolicited response and sends it out. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None  

===========================================================================*/
extern void dsatrsp_process_proxy_urc
(
  dsm_watermark_type *wm_item_ptr       /* Response Watermark pointer */
)
{
  if ( ( !dsatcmdp_block_indications() ) &&
       ( DSAT_CMD == dsatcmdp_at_state ) )
  {
    atproxy_dsm_item_ptr = dsm_dequeue( wm_item_ptr );
    dsatcmdp_send_respose_to_te(atproxy_dsm_item_ptr, DSAT_PROXY_RSP);
    atproxy_dsm_item_ptr = NULL;
  }
}/* dsatrsp_process_proxy_urc */

/*===========================================================================

FUNCTION PARSE_PROXY_RESULT

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type parse_proxy_result
( 
  dsm_item_type * proxy_res_buff 
)
{
#define RESPONSE_OFFSET (2) /* <CR><LF> are the first two characters */
  dsat_result_enum_type result_code = DSAT_MAX_BASIC;

  if ( dsatutil_strncmp_ig_sp_case(
        (const byte*)&proxy_res_buff->data_ptr[RESPONSE_OFFSET],
               "ERROR",5)==0)
  {
    result_code = DSAT_ERROR;
  }
  else if ( dsatutil_strncmp_ig_sp_case(
       (const byte*)&proxy_res_buff->data_ptr[RESPONSE_OFFSET],
              "+CM",3)==0)
  {
    result_code = DSAT_CMD_ERR_RSP;
  }
  else if( dsatutil_strncmp_ig_sp_case(
    (const byte*)&proxy_res_buff->data_ptr[RESPONSE_OFFSET],
           "CONNECT",7)==0)
  {
    result_code = DSAT_CONNECT;
  }
  else if( dsatutil_strncmp_ig_sp_case(
    (const byte*)&proxy_res_buff->data_ptr[RESPONSE_OFFSET],
           "NO CARRIER",10)==0)
  {
    result_code = DSAT_NO_CARRIER;
  }
  else if( dsatutil_strncmp_ig_sp_case(
    (const byte*)&proxy_res_buff->data_ptr[0],
           "PROMPT",6)==0)
  {
    result_code = DSAT_SMS_PROMPT;
  }
  else if( dsatutil_strncmp_ig_sp_case(
    (const byte*)&proxy_res_buff->data_ptr[0],
           "ENDOFM",6)==0)
  {
    result_code = DSAT_SMS_EOM;
  }
  return result_code;
}/* parse_proxy_result */

#endif /* FEATURE_DSAT_MDM_FUSION */

/*===========================================================================

FUNCTION DSATRSP_SEND_URC

DESCRIPTION
  Send result code to TE or Queues if command in process. 

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_urc
(
  dsat_result_enum_type result_code
)
{
  dsm_item_type *fmt_rsp_ptr;

  /* Get buffer to hold formatted response. */
  fmt_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  if (NULL != fmt_rsp_ptr)
  {
    /* Format the result code and place in response buffer. */
    dsatrsp_fmt_result_code( result_code, &fmt_rsp_ptr );

    if ( fmt_rsp_ptr != NULL )
    {
      /* Send the formatted result code. */
      dsatcmdp_send_urc(fmt_rsp_ptr, DSAT_COMPLETE_RSP);
    }
  }
  return;
}/* dsatrsp_send_urc */

/*===========================================================================

FUNCTION DSATRSP_SEND_URC_PER_SUBS

DESCRIPTION
  Send result code per subscription to TE or Queues if command in process. 

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatrsp_send_urc_per_subs
(
  sys_modem_as_id_e_type subs_id,
  dsat_result_enum_type  result_code
)
{
  dsm_item_type *fmt_rsp_ptr;

  /* Get buffer to hold formatted response. */
  fmt_rsp_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  if (NULL != fmt_rsp_ptr)
  {
    /* Format the result code and place in response buffer. */
    dsatrsp_fmt_result_code( result_code, &fmt_rsp_ptr );

    if ( fmt_rsp_ptr != NULL )
    {
      /* Send the formatted result code. */
      dsatcmdp_send_urc_per_subs(subs_id, fmt_rsp_ptr, DSAT_COMPLETE_RSP);
    }
  }
  return;
}/* dsatrsp_send_urc_per_subs */

