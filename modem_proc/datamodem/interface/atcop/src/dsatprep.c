/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
       A T   C O M M A N D   S I O   D A T A   P R E P R O C E S S O R

GENERAL DESCRIPTION
  This module contains the externally callable functions which interface
  the AT Command Processor to data received by serial I/O.  It
  preprocesses the received data differently for the three modes of
  operation it supports:
        - AT command mode
        - Abort command mode
        - SMS message entry mode
  
EXTERNALIZED FUNCTIONS
  dsatprep_process_sio_command
    This function is to be called from the data services task to process 
    incoming data from the serial port, when the AT command processor
    is in AT command mode or online command mode.
    
EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT    
  dsatprepi_set_preprocessor_mode
    Changes the SIO data preprocessor mode of operation.

  process_md5_msg_entry_mode
    This function performs the MD5 message entry data preprocessing
    necessary to accept a MD5 message.

    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The AT command processor must be initialized by calling the function
  dsat_init on initial power-up and calling dsat_init_service_mode
  each time a new operating service mode is entered.

  Copyright (c) 2001 - 2012,2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatprep.c_v   1.7   22 Oct 2002 13:33:12   sbandaru  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatprep.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
-------    ---     ---------------------------------------------------------
08/12/14   tk      Fixed issue with ATCoP preprocessor mode in CSVT call.
06/27/14   tk      Added support for Dynamic ATCoP.
03/11/14   tk      Optimized debug macros usage in ATCoP.
11/30/12   tk      ATCoP changes for Triton DSDA.
08/22/12   sk      Mainlining Nikel Changes.
05/18/12   tk      Migrated to MSG 2.0 macros
04/11/12   nc      Fixed operating mode checks for SMS related commands for NikeL targets. 
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
01/19/12   sk      Feature cleanup.
05/26/11   dvk     Global Variables Cleanup
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
10/07/10   ad      Added MSG_HIGH for command response.
10/06/10   ad      Added Error msg before Assert(0).
07/23/10   nc      Added 1x SMS Support for FUSION Architecture.
05/10/10   kk      Added support for ^HC SMS commands. 
11/16/09   ca      Added support the MD5 message entry mode.
12/15/09   nc      Featurisation changes for LTE.
12/18/08   ss      Lint Fixes
12/12/08   ua      Fixed Off target lint errors.
12/11/08   nc      Fixed Off Target Lint Errors
12/03/08   bs      Fixed prev_line_buffer size.
11/13/08   nc      Replacing '\r' in user data with '\n' in SMS entry mode.
09/25/08   bs      Fixed parallel AT commands entry using different ports.
08/22/08   nc      Corrected the echo of second AT command during processing
                   of first one.
07/16/08   ua      Added support for external client support.
12/10/07   sa      Added check for atprep_dsm_item_ptr to avoid NULL exception.
10/10/07   sa      Added check for echo_ptr to avoid NULL exception.
10/09/07   ss      Correcting the behaviour for GSM+1x targets.
08/18/07   ua      Corrected to traverse all the packets in a  DSM Pkt chain 
                   while reading data from DSM items in a water mark.
05/02/07   ua      Fixed lint medium errors. 
01/29/06   ua      Added support for GSM 7bit '@' character (0x00).
                   Added support for 0x00 - 0x7F  charactes in SMS
02/15/07   ss      Fixed lint high errors
01/12/06   snb     Allow character values 0x01-0x7F within AT commands. 
11/28/05   snb     Reset active port on garbled packet.
04/15/05   sb      If serializer is enabled, do not rearm the sutodetect mode
                   when abort mode is complete. Eventual result code generation
                   will rearm the autodetect mode.
04/15/05   sb      Fixed lint errors
04/12/05   sb      If serializer is enabled, do not rearm the sutodetect mode
                   when ESC or ^Z characters are received. Eventual result code
                   generation will rearm the autodetect mode.
03/15/05   sb      Fixes from testing the serializer changes.
02/10/05   sb      Accessories related changes.
04/21/04   snb     Fix for ME restting when SMS message contains embedded <ESC>
                   or Ctrl-Z characters. Fixes CR42497.
02/24/04   snb     Corrections to handle carriage return in SMS message entry
                   while in CDMA mode.
12/03/03   sb      Setting the preprocessor mode to SMS message entry should
                   be common to ETSI and CDMA
11/28/03   sb      CDMA SMS changes
08/19/03   ar      Truncate DSM item used value once S3 seen command line.
02/14/03   wx      Add ABORTING state to the abortable command to 
                   handle the aborting in async command processing.
10/22/02   sb      Merge of latest ETSI related changes
10/09/02   sb      Merged the latest ETSI file
09/12/02    ar     Merged in SIO interface changes from MM_DATA
06/11/02   wx      Exit process_at_cmd_mode function for every complete
                   AT cmd line to check preprocessor_mode.
08/10/01   sjd     Initial release to MSM5200 archives.

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */


#include "target.h"
#include "err.h"
#include "amssassert.h"
#include "dsati.h"
#include "dsatme.h"
#include "ds3gsiolib.h"

#ifdef TEST_FRAMEWORK
#error code not present
#endif

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include "dsatsms.h"
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */


#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
#include "dsatctab.h"
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

/*===========================================================================

                         EXTERNAL DEFINITIONS 

===========================================================================*/


#ifdef FEATURE_DSAT_EXTENDED_SMS
extern wms_user_data_encoding_e_type dsat707smsi_send_encoding;
#endif /* FEATURE_DSAT_EXTENDED_SMS */

/* point to current dsm item under processing */
extern dsm_item_type * atprep_dsm_item_ptr;

/*--------------------------------------------------------------------------
   SIO data preprocessor mode of operation.
---------------------------------------------------------------------------*/
extern dsati_preproc_mode_enum_type preprocessor_mode;

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#define DEBUG( x )

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
/*--------------------------------------------------------------------------
   Character definitions and strings for SMS message entry mode.
---------------------------------------------------------------------------*/
#define CTRL_Z_CHAR  '\x1A'
#define ESC_CHAR     '\x1B'

#ifdef FEATURE_DSAT_MSM_FUSION
LOCAL const char *prompt = "PROMPT\r\n> ";
LOCAL const char *eom    = "ENDOFM\r\n";
#else 
LOCAL const char *prompt = "\r\n> ";
LOCAL const char *eom    = "\r\n";
#endif /* FEATURE_DSAT_MSM_FUSION */

#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

/*---------------------------------------------------------------------------
    Local function declarations to allow the desired function ordering.    
---------------------------------------------------------------------------*/

static void dump_echo
(
  dsm_item_type **
);

static void store_echo
(
  byte,  
  dsm_item_type **
);

static boolean process_at_cmd_mode
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);

static boolean process_abort_cmd_mode
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);

#ifdef FEATURE_DSAT_MDM_FUSION
static boolean process_proxy_cmd_mode
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);
#endif /* FEATURE_DSAT_MDM_FUSION */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
static boolean process_sms_msg_entry_mode
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) || defined(FEATURE_DSAT_MMGSDI_AUTH)
static void send_string
(
  const char *
);
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) || defined(FEATURE_DSAT_MMGSDI_AUTH) */

#ifdef FEATURE_DSAT_MMGSDI_AUTH
static boolean process_md5_msg_entry_mode
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);
#endif /* FEATURE_DSAT_MMGSDI_AUTH */ 

/*===========================================================================

FUNCTION DSATPREP_GET_PREPROCESSOR_MODE

DESCRIPTION
  This function returns the current preprocessor mode.
 
DEPENDENCIES
  None

RETURN VALUE
  Current pre-processor mode.

SIDE EFFECTS
  None
===========================================================================*/

dsati_preproc_mode_enum_type dsatprep_get_preprocessor_mode(void)
{
  return preprocessor_mode;
}
/*===========================================================================

FUNCTION dsatprepi_set_preprocessor_mode

DESCRIPTION
  Changes the SIO data preprocessor mode of operation.  Operating modes
  can be: AT command mode, abort command mode, or SMS message entry mode.
  AT command mode is the default mode of SIO data preprocessor operation.
  It handles normal AT command line entry.  Abort command mode provides
  processing to abort an abortable command when any character is received.
  SMS message entry mode handles input of SMS messages.

  A state machine in this function controls transitions between SMS
  message entry mode and abort command mode when abortable commands are
  being processed.  When an abortable command is being processed and
  current mode is SMS message entry mode, entry into abort command mode
  will be delayed until the next call to this function.  Once abort
  command mode is exited, AT command mode will always be entered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Mode changes may be delayed when abortable SMS commands are being
  processed (see description above).  SIO AUTODETECT mode is rearmed
  on entry into AT command line mode.

===========================================================================*/

void dsatprepi_set_preprocessor_mode
(
  dsati_preproc_mode_enum_type mode,    /* mode to set data preprocessor to */
  ds3g_siolib_port_e_type      port     /* port to set preprocessor mode    */
)
{
  dsat_sio_info_s_type *sio_info_ptr;

  DS_AT_MSG2_MED("Preprocessor mode = %d, Port = %d", mode, port);

  if(( port >= DS3G_SIOLIB_PORT_MAX ) ||( port < DS3G_SIOLIB_DATA_PORT ))
  {
    DS_AT_MSG1_ERROR("Invalid SIO port: %d",port);
    return;
  }

  sio_info_ptr = dsatcmdp_get_sio_info_ptr(port);

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  /*------------------------------------------------------------------------
    This simple state machine is used to control transitions between
    abort command mode and SMS message entry mode when an abortable
    command is being processed.  It insures proper abort command
    processing of abortable SMS commands using SMS message entry mode.
  ------------------------------------------------------------------------*/
  {
    static dsati_preproc_mode_enum_type next_mode = DSATI_INVALID_MODE;

    if ( dsatcmdp_abortable_state != NOT_ABORTABLE )
    {
      /* Processing an abortable command. */
      
      if (( mode == DSATI_ABORT_CMD_MODE ) &&
          ( ( preprocessor_mode == DSATI_SMS_MSG_ENTRY_MODE ) ||
            ( preprocessor_mode == DSATI_MD5_MSG_ENTRY_MODE ) ))
      {
        /* Currently in SMS mode and directed to abort mode, so delay
           entry into abort mode until next call to this function. */
        next_mode = mode;
        
        return;
      }
      else if ( next_mode == DSATI_ABORT_CMD_MODE )
      {
        /* Entry into abort mode was delayed, so enter it now.  Once
           abort command processing is completed, AT command mode
           will be entered. */
        mode = next_mode;
        next_mode = DSATI_INVALID_MODE;
      }
    }
    else
    {
      /* Insure we reset next mode when not processing an abortable
         command. */
      next_mode = DSATI_INVALID_MODE;
    }
  }
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

  /*------------------------------------------------------------------------
    Normal processing of changes between SIO preprocessor modes.
  ------------------------------------------------------------------------*/
  switch ( mode )
  {
  case DSATI_AT_CMD_MODE:
    /* Enter AT command mode. */
    preprocessor_mode = mode;

    /* Reset AT command preprocessor state machine. */
    sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
    break;

  case DSATI_ABORT_CMD_MODE:
    /* Enter abort command mode. */
    preprocessor_mode = mode;    
    break;

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  case DSATI_SMS_MSG_ENTRY_MODE:
    /* Enter SMS message entry mode. */
    preprocessor_mode = mode;    

    /* Send SMS message entry prompt to TE. */
    send_string( prompt );

    /* Empty current dsm_item */
    if(atprep_dsm_item_ptr != NULL)
    {
      atprep_dsm_item_ptr->used = 0;
      (void) dsm_free_buffer( atprep_dsm_item_ptr );
      atprep_dsm_item_ptr = NULL;     
    }

    /* Assert the DCD line for SMS message entry. */
    ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
    (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_START );

    break;

  case DSATI_SMS_MSG_END_MODE:
    /* We have either received the message complete character (^Z) or */
    /* escape character. Reset the AT command mode to AT state but do */
    /* not enable the autodetect yet. Eventual result code generation */
    /* will enable the autodetect mode                                */
    preprocessor_mode = DSATI_AT_CMD_MODE;

    /* Reset AT command preprocessor state machine. */
    sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
    break;

#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */
#ifdef FEATURE_DSAT_MMGSDI_AUTH
  case DSATI_MD5_MSG_ENTRY_MODE:
    /* Enter MD5 message entry mode. */
    preprocessor_mode = mode;    
  
    /* Send MD5 message entry prompt to TE. */
    send_string( prompt );
  
    /* Empty current dsm_item */
    if(atprep_dsm_item_ptr != NULL)
    {
      atprep_dsm_item_ptr->used = 0;
      (void) dsm_free_buffer( atprep_dsm_item_ptr );
      atprep_dsm_item_ptr = NULL;     
    }
    /* Assert the DCD line for MD5 message entry. */
    ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
    (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_MD5_MSG_START );
    break;
  case DSATI_MD5_MSG_END_MODE:
    /* We have either received the message complete character (^Z) or */
    /* escape character. Reset the AT command mode to AT state but do */
    /* not enable the autodetect yet. Eventual result code generation */
    /* will enable the autodetect mode                                */
    preprocessor_mode = DSATI_AT_CMD_MODE;
  
    /* Reset AT command preprocessor state machine. */
    sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
    break;
#endif /* FEATURE_DSAT_MMGSDI_AUTH */
#ifdef FEATURE_DSAT_MDM_FUSION
  case DSATI_AT_PROXY_CMD_MODE:
    preprocessor_mode = mode;
    break;
#endif /* FEATURE_DSAT_MDM_FUSION */
  default:
    DS_AT_MSG1_ERROR("Invalid SIO data preprocessor mode %d", mode);
  }
  
} /* dsatprepi_set_preprocessor_mode( ) */


/*===========================================================================

FUNCTION DSATPREP_PROCESS_SIO_COMMAND

DESCRIPTION
 This function invokes SIO data preprocessor that filters and echoes
 received characters, handles line editing, based on internal operating
 mode.  Operating modes can be AT command line, abort command, or SMS
 message entry mode.  Operating modes are changed during SMS command
 processing and when abortable commands are being processed.

 In AT command line mode, once AT command line parsing is completed
 AT commands are processed.  AT commands are processed based on AT
 state (Command or Online Command) and operating service mode.
 
 In abort command mode, any received character causes the command
 executing to be aborted.  The SIO data preprocessor returns a Boolean
 indicating the call should be aborted for call related commands or it
 calls a function to terminate other abortable commands.

 In SMS message entry mode, the execution of SMS command is completed
 after complete SMS message is received or the command is cancelled.

 Called from 3G DSMGR when data is received from SIO.

DEPENDENCIES
  None

RETURN VALUE
  Abort call result; TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  There are side effects associated with processing a command line,
  aborting a command, and SMS message entry.

===========================================================================*/
boolean dsatprep_process_sio_command
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
)
{
  boolean abort_call = FALSE;           

  /*-------------------------------------------------------------------------
    Initialize the regional AT state variable.
  -------------------------------------------------------------------------*/  
  dsatcmdp_at_state = at_state;

  /*-------------------------------------------------------------------------
    Process data in watermark item based on SIO data preprocessor mode.
  -------------------------------------------------------------------------*/  
  /* We process one AT command line each time when a port gets priority.
  ** active_port will be reset back to ATCOP_NONE whenever we finish one 
  ** AT command line.
  */
  while ((ds3g_siolib_get_ds3g_siolib_active_port()!= DS3G_SIOLIB_PORT_NONE)&& 
         (atprep_dsm_item_ptr != NULL || 
         (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL ))
  {
    switch ( preprocessor_mode )
    {
    case DSATI_AT_CMD_MODE:
      abort_call = process_at_cmd_mode( at_state, wm_item_ptr );
      break;
    
    case DSATI_ABORT_CMD_MODE:
      abort_call = process_abort_cmd_mode( at_state, wm_item_ptr );
      break;

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
    case DSATI_SMS_MSG_ENTRY_MODE:
      abort_call = process_sms_msg_entry_mode( at_state, wm_item_ptr );
      break;
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */
#ifdef FEATURE_DSAT_MMGSDI_AUTH
    case DSATI_MD5_MSG_ENTRY_MODE:
      abort_call = process_md5_msg_entry_mode( at_state, wm_item_ptr );
      break;
#endif /* FEATURE_DSAT_MMGSDI_AUTH */
#ifdef FEATURE_DSAT_MDM_FUSION
    case DSATI_AT_PROXY_CMD_MODE:
      abort_call = process_proxy_cmd_mode( at_state, wm_item_ptr );
      break;
#endif /* FEATURE_DSAT_MDM_FUSION */
    default:
      ERR_FATAL("Invalid preprocessor_mode: %d",(int)preprocessor_mode,0,0);
      break;
      
    } /* switch ( preprocessor_mode ) */

    /* free current dsm_item if all the data in it has been processed */
    if((atprep_dsm_item_ptr != NULL) && !(atprep_dsm_item_ptr->used))
    {
       /* Get next packet pointer in the DSM packet chain*/
       atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );
      
    }
  }
    
  return abort_call;
  
} /* dsatprep_process_sio_command( ) */

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DSAT_PROCESS_PROXY_COMMAND

DESCRIPTION
This Function forwards the input to the Proxy port(MSM) in the PROXY_MODE.
Thus any input from the user is simply forwarded without processing.

DEPENDENCIES
MSM should be available for proper execution.

RETURN VALUE
Return value should be ignored.

SIDE EFFECTS
None.
===========================================================================*/
static boolean process_proxy_cmd_mode
(
  dsat_mode_enum_type at_state,     /* Command, online data, or online command state */
  dsm_watermark_type *wm_item_ptr   /* Pointer to watermark item to
                                     retrieve command line data from */
)
{
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

  ds3g_siolib_send_data_to_port(&ret_val,atprep_dsm_item_ptr,FALSE,ds3g_siolib_get_proxy_port());
  if( ret_val != DS3G_SIOLIB_OK)
  {
    DS_AT_MSG1_ERROR("Unable to send data to proxy port %d  ",ret_val);
    ASSERT(0);
  }
  else
  {
    atprep_dsm_item_ptr = NULL;
  }
  return FALSE;
}/* dsat_process_proxy_command */
#endif /* FEATURE_DSAT_MDM_FUSION */

/*===========================================================================

FUNCTION PROCESS_AT_CMD_MODE

DESCRIPTION
  This function builds the AT command line from data obtained from
  the dsm item passed in. The command line begins with the "AT"
  characters (either case or mixed case, but with nothing in between), 
  and is terminated with the end of line character stored in modem 
  register S3.  The backspace character (in modem register S5) is processed.
  
  When a complete line has been accumulated, dsat_process_cmd_line( ) is
  called. The line will have been edited per the backspace (above), and the
  AT removed.  The end of line character will have been replaced with the null
  character, '\0'.  Any spaces will still be present. The eighth bit of all 
  characters will have been set to zero.

  If the character combination, "A/" or "a/" is encountered while looking
  for the beginning of a command line, the previous command line, if any,
  will be immediately passed to dsat_process_cmd_line( ).

  If the command line buffer overflows, the data will be discarded and
  an error flag set.  The error will be reported when the end of line
  character in S3 is encountered.

  Input data will be echoed to the serial port.  Data is echoed at the end
  of each input data item, when a small data item (used for echo output) is
  filled, or when the S3 character is found, or the buffer overflows,
  whichever comes first.  The echo is controlled by the 'ATE' flag.

DEPENDENCIES
  None

RETURN VALUE
  Always FALSE

SIDE EFFECTS
  Buffer accumulates data until a full line is found.

  See also dsat_process_cmd_line which has many side effects.

===========================================================================*/
static boolean process_at_cmd_mode
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
)
{
  byte *item_data_ptr;                  /* item from queue data pointer    */
  dsm_item_type *echo_item_ptr = NULL;  /* echo item                       */
  byte cc;                              /* current character               */
  byte cce;                             /* current echo character          */
  boolean processing_cmd_line = TRUE;   /* this flag will be set to FALSE 
                                           if one complete at cmd line is 
                                           processed */
  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();
  boolean at_detect = FALSE;
  boolean rearm_autodetect_pending = FALSE;
  dsat_sio_info_s_type *sio_info_ptr = dsatcmdp_init_sio_info(port);
#ifdef FEATURE_DSAT_ETSI_MODE
  dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
  dsat_num_item_type cscs_val ;
#endif /* FEATURE_DSAT_ETSI_MODE */

  /*-------------------------------------------------------------------------

      The following loop implements a state machine, with the incoming 
      characters being the events.

                                                _____
                                               /     | incoming stream 
   +--------+  A,a   +---------+       +---------+   | copied to 
   |        |------->|         | T, t  |         |___| cmd_line_buffer.
 ~>|  HUNT  | /,A,a  | FOUND_A |------>| FOUND_AT|                   +-------+
   |        |<-------|         |       |         |------------------>| ERROR |
   +--------+        +---------+       +---------+ cmd_line_buffer   +-------+
       ^                                   |       overflow.             |
       |                                   |                             |
       |___________________________________V_____________________________|
                                       EOL


      In the HUNT state, the characters 'A' or 'a' are looked for.
      If they are found, the machine goes to the FOUND_A state.

      The FOUND_A state looks for 't', 'T', or '/', or another 'a' or 'A'.
      't' or 'T' causes a transition to the FOUND_AT state.
      '/' causes immediate processing of the previous AT command line
      which is still in cmd_line_buffer, followed by a transition to 
      the HUNT state.  'a' or 'A' causes the machine to remain in the 
      FOUND_A state.  Anything else causes a return to the HUNT state.

      The FOUND_AT state causes the incoming stream to be copied to 
      cmd_line_buffer[].  If a backspace character (dsat_s5_val) 
      is encountered, one character is erased from cmd_line_buffer[].  
      When the buffer is emptied, next backspace sets state to FOUND_A.
      If an end of line character (dsat_s3_val) is found, 
      dsat_process_cmd_line( ) is called with cmd_line_buffer[] as an
      argument, followed by a transition to the HUNT state.
      A buffer over flow causes a transition to the ERROR state.

      The ERROR state does nothing until an end of line character 
      (dsat_s35_val) is found.  Then, the machine returns to the
      HUNT state.

  -------------------------------------------------------------------------*/

  while (processing_cmd_line && 
         ( atprep_dsm_item_ptr != NULL ||
           (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL) )
  {
    /*---------------------------------------------------------------------
      Dequeue each item from watermark, extract AT command line, and pass
      to AT command parser.
    ---------------------------------------------------------------------*/

    item_data_ptr = atprep_dsm_item_ptr->data_ptr;
    while ( processing_cmd_line && atprep_dsm_item_ptr->used )
    {
      atprep_dsm_item_ptr->used--;               /* Decrement used count.            */
      cce = *item_data_ptr++;         /* Assign to echo character.        */
      cc = cce & 0x7F ;               /* Only the 7 upper bits are used.  */
      port = ds3g_siolib_get_active_port(); /* Get active port, port can become DS3G_SIOLIB_PORT_NONE in between  */

      if(( port >= DS3G_SIOLIB_PORT_MAX ) ||( port<DS3G_SIOLIB_DATA_PORT )||(FALSE == dsat_is_atcop_allowed()))
      {
        DS_AT_MSG1_ERROR("Invalid SIO port: %d freed remain DSM item in WM ",port);
        do
        {
          if( atprep_dsm_item_ptr != NULL)
          {
             /* Get the next packet in the DSM packet chain*/
             atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );
          }
        }while((atprep_dsm_item_ptr != NULL)||(atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL);
        
        return FALSE;
      }
    
      DEBUG(
             DS_AT_MSG3_LOW("cce:0x%x, cc:%c, cmd_prep_state: %d",
             cce, cc, sio_info_ptr->at_cmd_prep_state);
           );

       store_echo( cce, &echo_item_ptr );

      /*-------------------------------------------------------------------
        The following switch statements is a multi-way decison that tests 
        the current state of the AT State Machine and processes the incoming
        characters accordingly. The state machine starts initially in the 
        HUNT state. 
      -------------------------------------------------------------------*/
      switch ( sio_info_ptr->at_cmd_prep_state )
      {
       
      case DSAT_CMD_PREP_STATE_HUNT:                 
        /*-----------------------------------------------------------------
          In the HUNT state, the characters 'A' or 'a' are looked for.  
          Everything else is ignored. If 'A' or 'a' is detected, the   
          machine goes into the FOUND_A state.                              
        -----------------------------------------------------------------*/
        if ( UPCASE( cc ) == 'A' )
        {
          /* Transition into the FOUND_A state. */
          sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_A;

          /* Set regional flag to indicate indications should not be
             sent to TE at this time. */
          dsatcmdp_set_block_indications(port, TRUE);
        }
        else
        {
          if (echo_item_ptr != NULL)
          {
            echo_item_ptr->used = 0;      /*  Kill echo */
          }
        }
    
        break;
    
       
      case DSAT_CMD_PREP_STATE_FOUND_A:              
        /*-----------------------------------------------------------------
          In the FOUND_A state, detection of 'T' or 't' will take the state
          machine into the FOUND_AT state; 'A'or 'a' will result in
          a transition looping back to the FOUND_A state; '/' will result
          in the immediate processing of the previous AT command line;
          everything else will result in a transition to HUNT state and
          rearming the SIO autodetect.
        -----------------------------------------------------------------*/
        if ( cc == '/' )
        {
            /* echo it now before process the previous cmd */
            dump_echo( &echo_item_ptr );

          /*--------------------------------------------------------------
            'A/', request for the re-execution of previous command,   
            is detected. Process the previous AT command line     
            immediately.                                              
           --------------------------------------------------------------*/
          dsatcmdp_process_cmd_line( at_state, sio_info_ptr->prev_line_buffer );
          /* Transition back to the HUNT state.*/
          sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
          processing_cmd_line = FALSE;
          at_detect = TRUE;
        }
        else if ( UPCASE( cc ) == 'T' )
        {
          /*-------------------------------------------------------------- 
            Write 'T' or 't' into the buffer and transition to the
            the FOUND_AT state. 
          --------------------------------------------------------------*/
          sio_info_ptr->build_cmd_ptr = &sio_info_ptr->cmd_line_buffer[0];
          sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_AT;             
        }
        else if ( UPCASE( cc ) != 'A' )
        {
          /*-------------------------------------------------------------- 
            Transition back to the HUNT state. 
          --------------------------------------------------------------*/
          sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;          

          /*  Kill echo if not backspace */
          if ( cc != (dsat_num_item_type)dsatutil_get_val(
                                        DSAT_SREG_S5_IDX,0,0,NUM_TYPE))
          {
            if (echo_item_ptr != NULL)
            {
              echo_item_ptr->used = 0;
            }
          }

          /* Clear regional flag to indicate indications can now be
             sent to TE. */
          dsatcmdp_set_block_indications(port, FALSE);

          /* Re-arm SIO autodetect mode. */
          if ( !(wm_item_ptr->current_cnt) && !(atprep_dsm_item_ptr->used) ) 
          {
            if( (echo_item_ptr != NULL ) && !(echo_item_ptr->used) )
            {
              (void) ds3g_siolib_rearm_autodetect( port );
            }
            else
            {
              rearm_autodetect_pending = TRUE;
            }
          }
        }
        break;
    
    
      case DSAT_CMD_PREP_STATE_FOUND_AT:            
        /*-----------------------------------------------------------------
          When in the FOUND_AT state, the incoming stream is written
          to cmd_line_buffer[]. A backspace character will result        
          in the removal of a single character from the                 
          cmd_line_buffer. When EOF is encountered, the content of       
          cmd_line_buffer is processed as an argument and state      
          machine goes back to the HUNT state. The buffer status       
          is being monitored, an overflow will result in a            
          transition into the ERROR state.                             
        -----------------------------------------------------------------*/
        if ( cc != (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_SREG_S3_IDX,0,0,NUM_TYPE) )   
        {
          /*---------------------------------------------------------------
            If not backspace, fill buffer.
          ---------------------------------------------------------------*/
          if ( cc != (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_SREG_S5_IDX,0,0,NUM_TYPE) )  
          {
            /* Providing support for GSM 7bit '@' character whose hex value is
               0x00 */
#ifdef FEATURE_DSAT_ETSI_MODE
            if ( IS_ETSI_MODE(current_mode) )
            {
              /* If character set is GSM 7bit and the input character is 
              '@' (0x00) replace it with a special character and use it with
              in ATCOP Module. */
              cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
              if ( cscs_val == (dsat_num_item_type)ALPHA_GSM && cc == GSM_AT_CHAR )
              {
                cc = GSM_AT_CHAR_REPLACEMENT;
              }
            }
#endif /* FEATURE_DSAT_ETSI_MODE */
            /*-------------------------------------------------------------
              When overflow occurs,  transition into the ERROR state 
              will take place and the buffer will be reinitialized. 
            -------------------------------------------------------------*/
            if ( sio_info_ptr->build_cmd_ptr >= (sio_info_ptr->cmd_line_buffer
                                   + sizeof( sio_info_ptr->cmd_line_buffer ) - 2) )
            {
              sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_ERROR;             
              sio_info_ptr->cmd_line_buffer[0] = '\0';   
            }
            /*------------------------------------------------------------
              If no overflow, fill buffer.  
            ------------------------------------------------------------*/
            else if ( (cc >= 0x01 && cc <= 0x7F ) 
#ifdef FEATURE_DSAT_ETSI_MODE
            || ( IS_ETSI_MODE(current_mode) && (cc == GSM_AT_CHAR_REPLACEMENT) )
#endif /*FEATURE_DSAT_ETSI_MODE */
            )
            {
              *sio_info_ptr->build_cmd_ptr++ = cc;
            }
          }
    
          /*--------------------------------------------------------------
            Backspace encountered. Remove the most immediate character
            from the buffer.  Go back to FOUND_A state. 
          --------------------------------------------------------------*/
          else  
          {
            if ( --sio_info_ptr->build_cmd_ptr < &sio_info_ptr->cmd_line_buffer[0] )
            {
              sio_info_ptr->build_cmd_ptr = &sio_info_ptr->cmd_line_buffer[0];
              cce = '\0';      /*  Kill echo */
              sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_FOUND_A;             
            }
          }
        }
    
        /*-----------------------------------------------------------------
          EOL found, terminate and parse  
        -----------------------------------------------------------------*/
        else                       
        {
          /* Update DSM used to reflect EOL reached.                     */
          /* This avoids issue where software sends null bytes after S3. */
          atprep_dsm_item_ptr->used = 0;
          
          *sio_info_ptr->build_cmd_ptr = '\0';
          if (sio_info_ptr->build_cmd_ptr >= (sio_info_ptr->cmd_line_buffer
                                  + sizeof(sio_info_ptr->cmd_line_buffer)))
          {
            /* This should never happen */
            DS_AT_MSG0_HIGH("OVERFLOW");
          }
          
          /*---------------------------------------------------------------
            Update previous command buffer.  Second buffer used as the main 
            buffer can be overwritten before the A/ sequence is processed.  
          ---------------------------------------------------------------*/
          sio_info_ptr->build_cmd_ptr = &sio_info_ptr->cmd_line_buffer[0];
          sio_info_ptr->prev_cmd_ptr = &sio_info_ptr->prev_line_buffer[0];
          while ('\0' != (*sio_info_ptr->prev_cmd_ptr++ = *sio_info_ptr->build_cmd_ptr++)) {}
          
          /*---------------------------------------------------------------
              Dump echo so it will go out port before the result 
            of the command does (!) Then process the current content of 
            the buffer. 
          ---------------------------------------------------------------*/
          dump_echo( &echo_item_ptr );

          dsatcmdp_process_cmd_line( at_state, sio_info_ptr->cmd_line_buffer );
          sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
          
          processing_cmd_line = FALSE;
          at_detect = TRUE;
        }
    
        break;
    
        
      case DSAT_CMD_PREP_STATE_ERROR:                 
        /*-----------------------------------------------------------------
          The ERROR state does nothing until an end of line character is 
          found.  Then, the machine returns to the HUNT state.
        -----------------------------------------------------------------*/
        if ( cc == (dsat_num_item_type)dsatutil_get_val(
                                       DSAT_SREG_S3_IDX,0,0,NUM_TYPE) )
        {
          /*---------------------------------------------------------------
            Echo line character, send error result code to TE, and
            return to HUNT state.
          ---------------------------------------------------------------*/
    
          dump_echo( &echo_item_ptr );
    
          DSATI_ERROR_MSG( );
          dsat_send_result_code( DSAT_ERROR );
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
          dsatrsp_send_cell_result_to_ext_client(DSAT_ERROR);
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT*/
          /* Transition back to HUNT. */
          sio_info_ptr->at_cmd_prep_state = DSAT_CMD_PREP_STATE_HUNT;
        }
        break;
        
      }  /* end switch ( at_cmd_prep_state ) */
    
    }  /* end while ( atprep_dsm_item_ptr ) */
    
    dump_echo( &echo_item_ptr );

    if ( !(wm_item_ptr->current_cnt) && 
         ( (atprep_dsm_item_ptr == NULL) || !(atprep_dsm_item_ptr->used) ) )
    {
      if( rearm_autodetect_pending )
      {
        (void) ds3g_siolib_rearm_autodetect( port );
        rearm_autodetect_pending = FALSE;
      }
    }

    if((atprep_dsm_item_ptr != NULL) && !(atprep_dsm_item_ptr->used))
    {
      /* Get the next packet in the DSM packet chain*/
      atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );
    }

  } /*  end of input from Rm  */
      
  ASSERT( echo_item_ptr == NULL );

  if(at_detect == FALSE) 
  {
    ds3g_siolib_service_ports();
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
    dsatrsp_send_cell_result_to_ext_client(DSAT_ERROR);
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT*/

  }

  return FALSE;
  
} /* process_at_cmd_mode( ) */

/*===========================================================================

FUNCTION PROCESS_ABORT_CMD_MODE

DESCRIPTION
  This function performs the processing of receive characters to
  abort the current command in progress.  It performs the following
  functions during abort command mode:
  
   - Initiates aborting of command when any character is received
   - Flushes all remaining characters from watermark after abort
     character is received
   - Sets SIO data preprocessor mode to DSATI_AT_CMD_MODE after
     abort character is received

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
static boolean process_abort_cmd_mode
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           abort command data from */
)
{
  boolean abort_call = FALSE;               /* return value for aborting call  */

  
  /*-------------------------------------------------------------------------
    Dequeue all items from watermark and empty each item.
  -------------------------------------------------------------------------*/
  while ( atprep_dsm_item_ptr != NULL ||
          (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL )
  {

    if ( atprep_dsm_item_ptr->used > 0)
    {
      atprep_dsm_item_ptr->used = 0;               /* reset used count.            */
      abort_call = TRUE;
    }  
    dsm_free_packet( &atprep_dsm_item_ptr );
    atprep_dsm_item_ptr = NULL;

  } /*  end of input from Rm  */
      
  /*-------------------------------------------------------------------------
    Abort current commmand in progress.  Note, this function only
    calls the handler to abort the command once during the abort
    processing of a command.  This is required since this function
    may be called multiple times during the abort processing of a
    command.
  -------------------------------------------------------------------------*/
  if(abort_call)
  {
    abort_call = dsatcmdp_abort_command( );
  }

  return abort_call;

} /* process_abort_cmd_mode( ) */


#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)

#ifdef FEATURE_DSAT_EXTENDED_SMS
/*===========================================================================

FUNCTION PROCESS_SMS_MSG_ENTRY_MODE

DESCRIPTION
  This function performs the SMS message entry data preprocessing
  necessary to accept an SMS message.  It performs the following
  functions on SMS data entered:
  
   - Echo characters entered based on E command setting
   - Perform line editing functions based on command line editing
     character (S5 parameter) for SMS text mode only
   - Detect <ESC> character and cancel message sending or writing
     command when received
   - Detect <ctrl-Z> to indicate end of message body  
   - Complete processing of SMS send or write command when <ctrl-Z> is
     received
   - Each time <CR> is received the TA sends the
     <CR><LF><greater_than_sign><space> prompt to the TE  (text mode only)
   - Set the SIO data preprocessor mode to DSATI_AT_CMD_MODE once <ctrl-Z>
     or <ESC> is received and return DCD to its previous state

NOTE: FEATURE_DSAT_EXTENDED_SMS feature supports UNICODE. 
                
DEPENDENCIES
  None

RETURN VALUE
  Always returns FALSE.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
static boolean process_sms_msg_entry_mode
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve SMS message from */
)
{
  dsati_operating_cmd_mode_type op_mode;
  dsm_item_type *echo_item_ptr = NULL;        /* echo item                 */
  byte *item_data_ptr;                        /* item from queue data ptr  */  
  static dsm_item_type *msg_item_ptr = NULL;  /* ptr to SMS message item   */
  static byte *msg_ptr;                       /* pointer to SMS message    */
  static byte *start_line_ptr;                /* start of line pointer     */
  byte cc = 0;                                /* current character         */
  byte cce = 0;                               /* current echo character    */
  word ccw = 0;
  byte *ccw_ptr = (byte *)&ccw;
  dsat_num_item_type cscs_val ;
  enum                                        /* SMS message entry states  */
  {
    NORMAL_STATE,
    ERROR_STATE
  } sms_msg_entry_state;
  boolean unicode_mode = FALSE; 
  boolean octet_mode = FALSE; 

  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();

   /* Update the encoding details */
   DS_AT_MSG1_MED("SMS Encoding value is  %d",dsat707smsi_send_encoding);

   if ( dsat707smsi_send_encoding == WMS_ENCODING_UNICODE )
   {
     unicode_mode = TRUE;
   }
   else if (dsat707smsi_send_encoding == WMS_ENCODING_OCTET)
   {
     octet_mode = TRUE;
   }
  /*----------------------------------------------------------------------
    Get a new buffer to hold the SMS message if one is not already
    allocated.
  ----------------------------------------------------------------------*/
  if ( msg_item_ptr == NULL )
  {
    msg_item_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    msg_ptr = start_line_ptr = msg_item_ptr->data_ptr;
  }
  
  /* Current operating mode needed to handle "\r" in message entry */
  op_mode = dsatcmdp_get_operating_cmd_mode();
  
  /*---------------------------------------------------------------------
    Dequeue each item from watermark, extract SMS message, and pass
    to SMS message handler.
  ---------------------------------------------------------------------*/

  sms_msg_entry_state = NORMAL_STATE;
  while ( atprep_dsm_item_ptr != NULL ||
          (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL )
  {
    item_data_ptr = atprep_dsm_item_ptr->data_ptr;
    while ( atprep_dsm_item_ptr->used &&
            msg_item_ptr )
    {
      boolean store_char = TRUE;    /* flag for storing current byte */

      if (unicode_mode) 
      {
        /* UNICODE Mode */
        /* the unicode buffer has to be in multiple of 2 */
        if (atprep_dsm_item_ptr->used % 2)
          break;

        atprep_dsm_item_ptr->used -= 2;  /* Decrement used count.           */
        ccw_ptr[1] = *item_data_ptr++;
        ccw_ptr[0] = *item_data_ptr++;   /* Assign to echo character.       */
      }
      else
      {
        atprep_dsm_item_ptr->used--;   /* Decrement used count.           */
        cce = *item_data_ptr++;        /* Assign to echo character.       */
        cc = octet_mode ? cce : (cce & 0x7F); 
      }

      /*------------------------------------------------------------------
        Process data received for SMS message.
      ------------------------------------------------------------------*/
      switch ( sms_msg_entry_state )
      {
      case NORMAL_STATE:
        /*----------------------------------------------------------------
          Normal processing of data for SMS message.
        ----------------------------------------------------------------*/
        if ( ((unicode_mode && (ccw == '\b')) || (!unicode_mode && (cc == '\b')) )
#ifdef FEATURE_ETSI_SMS 
             && ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
             0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) ||
             (dsatcmdp_is_cdma_cmd())
             )
#endif /* FEATURE_ETSI_SMS  */
                     )
        {
          /*--------------------------------------------------------------
            Line editing character (backspace) and SMS text message
            format mode - Perform line editing function.
          --------------------------------------------------------------*/

          if ( msg_ptr > start_line_ptr )
          {
            /* Not at the start of line yet so backup. */
            msg_ptr -= (unicode_mode ? 2 : 1);
            msg_item_ptr->used -= (unicode_mode ? 2 : 1);
          }
          else
          {
            /*  Kill echo - don't backspace over prompt */        
              store_char = FALSE;
          }
        }
        else if ((unicode_mode && (ccw == CTRL_Z_CHAR)) || 
                 (!unicode_mode && (cc == CTRL_Z_CHAR)))
        {
          /*--------------------------------------------------------------
            Ctrl-Z - Indicates end of message body
          --------------------------------------------------------------*/  
          
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );

          /* Send SMS message to SMS command handler. */
          dsatsms_send_sms_msg( msg_item_ptr->data_ptr, msg_item_ptr->used );

          /* Return DCD to its state before SMS message entry and set
             SIO data preprocessor back to AT command mode. */      

          ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  

          (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );

          dsatprepi_set_preprocessor_mode( DSATI_SMS_MSG_END_MODE, port );

          /* Free the DSM item holding SMS message. */
          (void) dsm_free_buffer( msg_item_ptr );
          msg_item_ptr = NULL;
        }
        else if ((unicode_mode && (ccw == ESC_CHAR)) || 
                 (!unicode_mode && (cc == ESC_CHAR)))
        {
          /*--------------------------------------------------------------
            ESC character - Cancel SMS message send or write command
          --------------------------------------------------------------*/
            
          /* Notify SMS handler command has been cancelled. */
          dsatsms_abort_cmd( DSAT_CMS_NONE );
          
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );

          /* Return DCD to its state before SMS message entry and set
             SIO data preprocessor back to AT command mode. */      

          ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  

          (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );

          dsatprepi_set_preprocessor_mode( DSATI_SMS_MSG_END_MODE, port );


          /* Free the DSM item holding SMS message. */
          (void) dsm_free_buffer( msg_item_ptr );
          msg_item_ptr = NULL;
        }
        else
        {
          /*--------------------------------------------------------------
            Any other IRA character - Write character to message buffer
          --------------------------------------------------------------*/  
         cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
          if ( (unicode_mode && ccw >= 0x01) 
               || (!unicode_mode && !octet_mode && cc >= 0x01 && cc <= 0x7F) 
               || (octet_mode && cc >= 0x01) 
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
               || ( ( cc == GSM_AT_CHAR ) && 
                   ( cscs_val == (dsat_num_item_type)ALPHA_GSM ))
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
               || (unicode_mode && (ccw == '\n' || ccw == '\r' )) 
               || (!unicode_mode && (cc == '\n' || cc == '\r' )) )
          {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
            if (op_mode == ETSI_CMD_MODE && cc == GSM_AT_CHAR ) /* GSM 7bit @ character */
            {
              cc = GSM_AT_CHAR_REPLACEMENT;
            }
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

            if ( (unicode_mode && 
                  ((msg_item_ptr->used+1) < msg_item_ptr->size) ) ||
                 (!unicode_mode && msg_item_ptr->used < msg_item_ptr->size ) )
            {
              /* There is room in buffer so save character to buffer. */
              if ( (unicode_mode && (ccw == '\r')) || 
                   (!unicode_mode && (cc == '\r')) )
              {
                if (unicode_mode) 
                {
                  *msg_ptr++ = 0;
                }
                *msg_ptr++ = '\r';
              }
              else
              {
                if (unicode_mode) 
                {
                  *msg_ptr++ = ccw_ptr[1];
                  *msg_ptr++ = ccw_ptr[0];
                }
                else
                {
                  *msg_ptr++ = cc;
                }
              }
              msg_item_ptr->used += (unicode_mode ? 2 : 1);
            }
            else
            {
              /* Buffer overflow, SMS message is too large. */
              sms_msg_entry_state = ERROR_STATE;
              
              /* Notify SMS handler command error has occurred and command
                 should be aborted. */
              dsatsms_abort_cmd( DSAT_CMS_ME_FAILURE );
      
              /* Free the DSM item holding SMS message. */
              (void) dsm_free_buffer( msg_item_ptr );
              msg_item_ptr = NULL;
              
              /* Return DCD to its state before SMS message entry and set
                 SIO data preprocessor back to AT command mode. */      
              ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
              (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );
              dsatprepi_set_preprocessor_mode( DSATI_AT_CMD_MODE, port );
              DS_AT_MSG0_ERROR("SMS message is too large!");
            }
          }

          if ( ( (unicode_mode && ccw == '\r') || (!unicode_mode && cc == '\r') )
#ifdef FEATURE_ETSI_SMS 
               && ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
               0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) ||
               ( dsatcmdp_is_cdma_cmd() )

             )
#endif /* FEATURE_ETSI_SMS  */
                     )
          {
            /* Carriage return character and SMS text message format
               mode, so generate new line prompt to TE and kill echo of
               carriage return. */
              store_char = FALSE;
            send_string( prompt );
            
            /* Point to start of the new line. */
            start_line_ptr = msg_ptr;
          }
          else if ( 
#ifdef FEATURE_ETSI_SMS 
                   ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
                   0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) || 
                   ( dsatcmdp_is_cdma_cmd() )
                   )&&
#endif /* FEATURE_ETSI_SMS  */                     
                   ((unicode_mode && (ccw == '\r' || ccw == '\n' || ccw == '\b' )) ||
                   (!unicode_mode && (cc == '\r' || cc == '\n' || cc == '\b' ))) )
          {
            /* PDU mode and character is carriage return, linefeed,
               or backspace, so kill echo of character. */
              store_char = FALSE;
          }
        }

        break;


      case ERROR_STATE:
        /*----------------------------------------------------------------
          Error processing of data for SMS message. In this state just
          echo the remaining data left in the watermark to empty the
          watermark.
        ----------------------------------------------------------------*/

        if ( (unicode_mode && ccw == '\b' ) || (!unicode_mode && cc == '\b' ) )
        {
          /* Kill echo on backspace. */
            store_char = FALSE;
        }
        else if ( (unicode_mode && (ccw == ESC_CHAR || ccw == CTRL_Z_CHAR) ) ||
                  (!unicode_mode && (cc == ESC_CHAR || cc == CTRL_Z_CHAR) ) )
        {
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );
        }          
        else if ( ((unicode_mode && ccw == '\r') ||
                  (!unicode_mode && cc == '\r') ) 
#ifdef FEATURE_ETSI_SMS 
                   && ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
                   0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) || 
                  ( dsatcmdp_is_cdma_cmd() )
             )
#endif /* FEATURE_ETSI_SMS  */
                     )
        {
          /* Carriage return character and SMS text message format
             mode, so generate new line prompt to TE and kill echo of
             carriage return. */
            store_char = FALSE;
          send_string( prompt );
        }
        else if ( 
#ifdef FEATURE_ETSI_SMS 
                 ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
                 0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) ||
                 ( dsatcmdp_is_cdma_cmd() )
             )&&
#endif /* FEATURE_ETSI_SMS  */
                 (( unicode_mode && (ccw == '\r' || ccw == '\n' || ccw == '\b' ) ) ||
                 ( !unicode_mode && (cc == '\r' || cc == '\n' || cc == '\b' ) )) )
        {
          /* PDU mode and character is carriage return, linefeed,
             or backspace, so kill echo of character. */
            store_char = FALSE;
        }
        
        break;
        
      }   /* end switch ( ) */
      
        if(store_char)
        {
          if (unicode_mode) 
          {
            store_echo( ccw_ptr[1], &echo_item_ptr );
            store_echo( ccw_ptr[0], &echo_item_ptr );
          } 
          else
          {
            store_echo( cce, &echo_item_ptr );
          }
        }
    
    }   /* end while ( atprep_dsm_item_ptr ) */

    dump_echo( &echo_item_ptr );
    /* Get the next packet in the DSM packet chain*/
    atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );

  } /*  end of input from Rm  */
      
  ASSERT( echo_item_ptr == NULL );
 
  return FALSE;
  
} /* process_sms_msg_entry_mode( ) */

#else    /* FEATURE_DSAT_EXTENDED_SMS */

/*===========================================================================

FUNCTION PROCESS_SMS_MSG_ENTRY_MODE

DESCRIPTION
  This function performs the SMS message entry data preprocessing
  necessary to accept an SMS message.  It performs the following
  functions on SMS data entered:
  
   - Echo characters entered based on E command setting
   - Perform line editing functions based on command line editing
     character (S5 parameter) for SMS text mode only
   - Detect <ESC> character and cancel message sending or writing
     command when received
   - Detect <ctrl-Z> to indicate end of message body  
   - Complete processing of SMS send or write command when <ctrl-Z> is
     received
   - Each time <CR> is received the TA sends the
     <CR><LF><greater_than_sign><space> prompt to the TE  (text mode only)
   - Set the SIO data preprocessor mode to DSATI_AT_CMD_MODE once <ctrl-Z>
     or <ESC> is received and return DCD to its previous state
                
DEPENDENCIES
  None

RETURN VALUE
  Always returns FALSE.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
static boolean process_sms_msg_entry_mode
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve SMS message from */
)
{
#ifdef FEATURE_ETSI_SMS
  dsati_operating_cmd_mode_type op_mode;
#endif /* FEATURE_ETSI_SMS */
  dsm_item_type *echo_item_ptr = NULL;        /* echo item                 */
  byte *item_data_ptr;                        /* item from queue data ptr  */  
  static dsm_item_type *msg_item_ptr = NULL;  /* ptr to SMS message item   */
  static byte *msg_ptr;                       /* pointer to SMS message    */
  static byte *start_line_ptr;                /* start of line pointer     */
  byte cc;                                    /* current character         */
  byte cce;                                   /* current echo character    */
#ifdef FEATURE_DSAT_ETSI_MODE
  dsat_num_item_type cscs_val ;
#endif /* FEATURE_DSAT_ETSI_MODE */

  enum                                        /* SMS message entry states  */
  {
    NORMAL_STATE,
    ERROR_STATE
  } sms_msg_entry_state;

  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();

  /*----------------------------------------------------------------------
    Get a new buffer to hold the SMS message if one is not already
    allocated.
  ----------------------------------------------------------------------*/
  if ( msg_item_ptr == NULL )
  {
    msg_item_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    msg_ptr = start_line_ptr = msg_item_ptr->data_ptr;
  }
  
  /* Current operating mode needed to handle "\r" in message entry */
#ifdef FEATURE_ETSI_SMS
  op_mode = dsatcmdp_get_operating_cmd_mode();
#endif /* FEATURE_ETSI_SMS */
  
  /*---------------------------------------------------------------------
    Dequeue each item from watermark, extract SMS message, and pass
    to SMS message handler.
  ---------------------------------------------------------------------*/

  sms_msg_entry_state = NORMAL_STATE;
  while ( atprep_dsm_item_ptr != NULL ||
          (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL )
  {

    item_data_ptr = atprep_dsm_item_ptr->data_ptr;
    while ( atprep_dsm_item_ptr->used &&
            msg_item_ptr )
    {
        boolean store_char = TRUE;    /* flag for storing current byte */

      atprep_dsm_item_ptr->used--;              /* Decrement used count.           */
      cce = *item_data_ptr++;        /* Assign to echo character.       */
      cc = cce & 0x7F ;              /* Only the 7 upper bits are used. */
  
      /*------------------------------------------------------------------
        Process data received for SMS message.
      ------------------------------------------------------------------*/
      switch ( sms_msg_entry_state )
      {
      case NORMAL_STATE:
        /*----------------------------------------------------------------
          Normal processing of data for SMS message.
        ----------------------------------------------------------------*/
        if ( cc == '\b' 
#ifdef FEATURE_ETSI_SMS 
               && ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
               0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) || op_mode == CDMA_CMD_MODE )
#endif /* FEATURE_ETSI_SMS  */
          )
        {
          /*--------------------------------------------------------------
            Line editing character (backspace) and SMS text message
            format mode - Perform line editing function.
          --------------------------------------------------------------*/

          if ( msg_ptr > start_line_ptr )
          {
            /* Not at the start of line yet so backup. */
            msg_ptr--;
            msg_item_ptr->used--;
          }
          else
          {
            /*  Kill echo - don't backspace over prompt */        
              store_char = FALSE;
          }
        }
        else if ( cc == CTRL_Z_CHAR )
        {
          /*--------------------------------------------------------------
            Ctrl-Z - Indicates end of message body
          --------------------------------------------------------------*/  
          
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );

          /* Send SMS message to SMS command handler. */
          dsatsms_send_sms_msg( msg_item_ptr->data_ptr, msg_item_ptr->used );

          /* Return DCD to its state before SMS message entry and set
             SIO data preprocessor back to AT command mode. */      
          ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
          (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );
          dsatprepi_set_preprocessor_mode( DSATI_SMS_MSG_END_MODE, port );

          /* Free the DSM item holding SMS message. */
          (void) dsm_free_buffer( msg_item_ptr );
          msg_item_ptr = NULL;
        }
        else if ( cc == ESC_CHAR )
        {
          /*--------------------------------------------------------------
            ESC character - Cancel SMS message send or write command
          --------------------------------------------------------------*/
            
          /* Notify SMS handler command has been cancelled. */
          dsatsms_abort_cmd( DSAT_CMS_NONE );
          
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );

          /* Return DCD to its state before SMS message entry and set
             SIO data preprocessor back to AT command mode. */      
          ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
          (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );
          dsatprepi_set_preprocessor_mode( DSATI_SMS_MSG_END_MODE, port );

          /* Free the DSM item holding SMS message. */
          (void) dsm_free_buffer( msg_item_ptr );
          msg_item_ptr = NULL;
        }
        else
        {
          /*--------------------------------------------------------------
            Any other IRA character - Write character to message buffer
          --------------------------------------------------------------*/  
#ifdef FEATURE_DSAT_ETSI_MODE
          cscs_val =(dsat_num_item_type)dsatutil_get_val(
                                             DSATETSI_EXT_CSCS_IDX,0,0,NUM_TYPE);
#endif /* FEATURE_DSAT_ETSI_MODE */
          if ( (cc >= 0x01 && cc <= 0x7F) || 
#ifdef FEATURE_DSAT_ETSI_MODE
               (cc == GSM_AT_CHAR && cscs_val == (dsat_num_item_type)ALPHA_GSM )|| 
#endif /* FEATURE_DSAT_ETSI_MODE */
               ( cc == '\n' || cc == '\r' ))
          {
#ifdef FEATURE_DSAT_ETSI_MODE
            if (op_mode == ETSI_CMD_MODE && cc == GSM_AT_CHAR ) /* GSM 7bit @ character */
            {
              cc = GSM_AT_CHAR_REPLACEMENT;
            }
#endif /* FEATURE_DSAT_ETSI_MODE */

            if ( msg_item_ptr->used < msg_item_ptr->size )
            {
              /* There is room in buffer so save character to buffer. */
              if ( cc == '\r')
              {
                *msg_ptr++ = '\n';
              }
              else
              {
              *msg_ptr++ = cc;
              }
              msg_item_ptr->used++;
            }
            else
            {
              /* Buffer overflow, SMS message is too large. */
              sms_msg_entry_state = ERROR_STATE;
              
              /* Notify SMS handler command error has occurred and command
                 should be aborted. */
              dsatsms_abort_cmd( DSAT_CMS_ME_FAILURE );
      
              /* Free the DSM item holding SMS message. */
              (void) dsm_free_buffer( msg_item_ptr );
              msg_item_ptr = NULL;
              
              /* Return DCD to its state before SMS message entry and set
                 SIO data preprocessor back to AT command mode. */      

              ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
              (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );
              dsatprepi_set_preprocessor_mode( DSATI_AT_CMD_MODE, port );
  
              DS_AT_MSG0_ERROR("SMS message is too large!");
            }
          }

          if ( cc == '\r' 
#ifdef FEATURE_ETSI_SMS 
               && ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
             0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) ||
             (dsatcmdp_is_cdma_cmd())
               )
#endif /* FEATURE_ETSI_SMS  */ 
             )
          {
            /* Carriage return character and SMS text message format
               mode, so generate new line prompt to TE and kill echo of
               carriage return. */
              store_char = FALSE;
            send_string( prompt );
            
            /* Point to start of the new line. */
            start_line_ptr = msg_ptr;
          }
          else if ( 
#ifdef FEATURE_ETSI_SMS 
                   ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
                   0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) || op_mode == CDMA_CMD_MODE ) && 
#endif /* FEATURE_ETSI_SMS  */
                    (cc == '\r' || cc == '\n' || cc == '\b' ) )
          {
            /* PDU mode and character is carriage return, linefeed,
               or backspace, so kill echo of character. */
              store_char = FALSE;
          }
        }

        break;


      case ERROR_STATE:
        /*----------------------------------------------------------------
          Error processing of data for SMS message. In this state just
          echo the remaining data left in the watermark to empty the
          watermark.
        ----------------------------------------------------------------*/

        if ( cc == '\b' )
        {
          /* Kill echo on backspace. */
            store_char = FALSE;
        }
        else if ( cc == ESC_CHAR || cc == CTRL_Z_CHAR )
        {
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );
        }          
        else if ( cc == '\r' 
#ifdef FEATURE_ETSI_SMS 
                   && ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
                   0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) ||  op_mode == CDMA_CMD_MODE )
#endif /* FEATURE_ETSI_SMS  */
                 )
        {
          /* Carriage return character and SMS text message format
             mode, so generate new line prompt to TE and kill echo of
             carriage return. */
            store_char = FALSE;
          send_string( prompt );
        }
        else if ( 
#ifdef FEATURE_ETSI_SMS 
                 ( ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CMGF_IDX,
                 0,0,NUM_TYPE) == 1 && op_mode == ETSI_CMD_MODE) || op_mode == CDMA_CMD_MODE ) && 
#endif /* FEATURE_ETSI_SMS  */
                  (cc == '\r' || cc == '\n' || cc == '\b' ) )
        {
          /* PDU mode and character is carriage return, linefeed,
             or backspace, so kill echo of character. */
            store_char = FALSE;
        }
        
        break;
        
      }   /* end switch ( ) */
      
        if(store_char)
        {
          store_echo( cce, &echo_item_ptr );
        }
    
    }   /* end while ( atprep_dsm_item_ptr ) */

    dump_echo( &echo_item_ptr );
    /* Get the next packet in the DSM packet chain*/
    atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );

  } /*  end of input from Rm  */
      
  ASSERT( echo_item_ptr == NULL );
 
  return FALSE;
  
} /* process_sms_msg_entry_mode( ) */
#endif /* FEATURE_DSAT_EXTENDED_SMS */
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) || defined(FEATURE_DSAT_MMGSDI_AUTH)

/*===========================================================================

FUNCTION SEND_STRING

DESCRIPTION
  Sends string passed in to the TE.

DEPENDENCIES
  String length must be short enough to fit into DSM small item.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void send_string
(
  const char *string_ptr      /* Pointer to NULL terminated string to send */
)
{
  dsm_item_type *item_ptr;

  /* Get a DSM item to place prompt in and send prompt. */
  item_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, TRUE);
  if (NULL != item_ptr)
  {
    /* Copy string to buffer and send response. */
  
    while ( (*string_ptr != '\0') && (item_ptr->used < item_ptr->size) )
    {
      item_ptr->data_ptr[item_ptr->used++] = *string_ptr++;
    }
  
    dsatrsp_send_response( item_ptr, FALSE );
  }
} /* send_string( ) */
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) || defined(FEATURE_DSAT_MMGSDI_AUTH) */

#ifdef FEATURE_DSAT_MMGSDI_AUTH
/*===========================================================================

FUNCTION PROCESS_MD5_MSG_ENTRY_MODE

DESCRIPTION

  This function performs the MD5 message entry data preprocessing
  necessary to accept a MD5 message.  It performs the following
  functions on MD5 data entered:
  
   - Echo characters entered based on E command setting
   - Perform line editing functions based on command line editing
   - Detect <ESC> character and cancel message sending or writing
     command when received
   - Detect <ctrl-Z> to indicate end of message body  
   - Complete processing of MD5 send or write command when <ctrl-Z> is
     received
   - Each time <CR> is received the TA sends the
     <CR><LF><greater_than_sign><space> prompt to the TE  (text mode only)
   - Set the SIO data preprocessor mode to DSATI_AT_CMD_MODE once <ctrl-Z>
     or <ESC> is received and return DCD to its previous state

                
DEPENDENCIES
  None

RETURN VALUE
  Always returns FALSE.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
static boolean process_md5_msg_entry_mode
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve MD5 message from */
)
{
  dsm_item_type *echo_item_ptr = NULL;        /* echo item                 */
  byte *item_data_ptr;                        /* item from queue data ptr  */  
  static dsm_item_type *msg_item_ptr = NULL;  /* ptr to MD5 message item   */
  static byte *msg_ptr;                       /* pointer to MD5 message    */
  static byte *start_line_ptr;                /* start of line pointer     */
  byte cc;                                    /* current character         */
  byte cce;                                   /* current echo character    */
  enum                                        /* MD5 message entry states  */
  {
    NORMAL_STATE,
    ERROR_STATE
  } md5_msg_entry_state;

  ds3g_siolib_port_e_type port = ds3g_siolib_get_active_port();

  /*----------------------------------------------------------------------
    Get a new buffer to hold the MD5 message if one is not already
    allocated.
  ----------------------------------------------------------------------*/
  if ( msg_item_ptr == NULL )
  {
    msg_item_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
    msg_ptr = start_line_ptr = msg_item_ptr->data_ptr;
  }
  
  /*---------------------------------------------------------------------
    Dequeue each item from watermark, extract MD5 message, and pass
    to MD5 message handler.
  ---------------------------------------------------------------------*/

  md5_msg_entry_state = NORMAL_STATE;
  while ( atprep_dsm_item_ptr != NULL ||
          (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL )
  {

    item_data_ptr = atprep_dsm_item_ptr->data_ptr;
    while ( atprep_dsm_item_ptr->used &&
            msg_item_ptr )
    {
        boolean store_char = TRUE;    /* flag for storing current byte */

      atprep_dsm_item_ptr->used--;              /* Decrement used count.           */
      cce = *item_data_ptr++;        /* Assign to echo character.       */
      cc = cce & 0x7F ;              /* Only the 7 upper bits are used. */
  
      /*------------------------------------------------------------------
        Process data received for MD5 message.
      ------------------------------------------------------------------*/
      switch ( md5_msg_entry_state )
      {
      case NORMAL_STATE:
        /*----------------------------------------------------------------
          Normal processing of data for MD5 message.
        ----------------------------------------------------------------*/
        if ( cc == '\b' )
        {
          /*--------------------------------------------------------------
            Line editing character (backspace) - Perform line editing function.
          --------------------------------------------------------------*/

          if ( msg_ptr > start_line_ptr )
          {
            /* Not at the start of line yet so backup. */
            msg_ptr--;
            msg_item_ptr->used--;
          }
          else
          {
            /*  Kill echo - don't backspace over prompt */        
              store_char = FALSE;
          }
        }
        else if ( cc == CTRL_Z_CHAR )
        {
          /*--------------------------------------------------------------
            Ctrl-Z - Indicates end of message body
          --------------------------------------------------------------*/  
          
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );
          /* NULL Terminate the string */
          msg_item_ptr->data_ptr[msg_item_ptr->used]='\0';
          /* Send SMS message to MD5 command handler. */
          dsatme_send_md5_msg( msg_item_ptr->data_ptr, msg_item_ptr->used );

          /* Return DCD to its state before MD5 message entry and set
             SIO data preprocessor back to AT command mode. */      
          ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
          (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_MD5_MSG_END );
          dsatprepi_set_preprocessor_mode( DSATI_MD5_MSG_END_MODE, port );

          /* Free the DSM item holding SMS message. */
          (void) dsm_free_buffer( msg_item_ptr );
          msg_item_ptr = NULL;
        }
        else if ( cc == ESC_CHAR )
        {
          /*--------------------------------------------------------------
            ESC character - Cancel MD5 message send or write command
          --------------------------------------------------------------*/
            
          /* Notify MD5 handler command has been cancelled. */
          dsatme_md5_abort_cmd( DSAT_CME_NO_ERROR );
          
          /* Kill echo and send EOM string. */
          store_char = FALSE;
          send_string( eom );

          /* Return DCD to its state before MD5 message entry and set
             SIO data preprocessor back to AT command mode. */      
          ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
          (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_MD5_MSG_END );
          dsatprepi_set_preprocessor_mode( DSATI_MD5_MSG_END_MODE, port );

          /* Free the DSM item holding SMS message. */
          (void) dsm_free_buffer( msg_item_ptr );
          msg_item_ptr = NULL;
        }
        else
        {
          /*--------------------------------------------------------------
            Any other IRA character - Write character to message buffer
          --------------------------------------------------------------*/  
          if ( (cc >= 0x01 && cc <= 0x7F) || 
               ( cc == '\n' || cc == '\r' ))
          {
            if ( msg_item_ptr->used < msg_item_ptr->size )
            {
              /* There is room in buffer so save character to buffer. */
              if ( cc == '\r')
              {
                *msg_ptr++ = '\n';
              }
              else
              {
                *msg_ptr++ = cc;
              }
              msg_item_ptr->used++;
            }
            else
            {
              /* Buffer overflow, MD5 message is too large. */
              md5_msg_entry_state = ERROR_STATE;
              
              /* Notify MD5 handler command error has occurred and command
                 should be aborted. */
              dsatme_md5_abort_cmd( DSAT_CME_MMGSDI_AUTH_ERROR);
      
              /* Free the DSM item holding MD5 message. */
              dsm_free_packet( &msg_item_ptr );

              /* Return DCD to its state before MD5 message entry and set
                 SIO data preprocessor back to AT command mode. */      
              ds3g_siolib_set_client_type ( DS3G_SIOLIB_CLIENT_ATCOP );  
              (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_MD5_MSG_END );
              dsatprepi_set_preprocessor_mode( DSATI_AT_CMD_MODE, port );
              DS_AT_MSG0_ERROR("SMS message is too large!");
            }
          }

          if ( cc == '\r' )
          {
            /* Carriage return character and MD5 text message format
               mode, so generate new line prompt to TE and kill echo of
               carriage return. */
            store_char = FALSE;
            send_string( prompt );
            
            /* Point to start of the new line. */
            start_line_ptr = msg_ptr;
          }
        }
        break;

      case ERROR_STATE:
        /*----------------------------------------------------------------
          Error processing of data for MD5 message. In this state just
          echo the remaining data left in the watermark to empty the
          watermark.
        ----------------------------------------------------------------*/

        if ( cc == '\b' )
        {
          /* Kill echo on backspace. */
          store_char = FALSE;
        }
        else if ( cc == ESC_CHAR || cc == CTRL_Z_CHAR )
        {
          /* Kill echo and send EOM string. */
          store_char = FALSE;
          send_string( eom );
        }          
        else if ( cc == '\r' )
        {
          /* Carriage return character and MD5 text message format
             mode, so generate new line prompt to TE and kill echo of
             carriage return. */
          store_char = FALSE;
          send_string( prompt );
        }
        break;
        
      }   /* end switch ( ) */
      
      if(store_char)
      {
        store_echo( cce, &echo_item_ptr );
      }
    }/* end while ( atprep_dsm_item_ptr ) */

    dump_echo( &echo_item_ptr );
    /* Get the next packet in the DSM packet chain*/
    atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );

  } /*  end of input from Rm  */
      
  ASSERT( echo_item_ptr == NULL );
 
  return FALSE;
  
} /* process_md5_msg_entry_mode( ) */
#endif /* FEATURE_DSAT_MMGSDI_AUTH */


/*===========================================================================

FUNCTION STORE_ECHO

DESCRIPTION
  This function acquires a small item buffer (if needed), and stores
  the echo character in it.  If the buffer becomes full, the buffer
  is output via dsatrsp_send_response( ), and another buffer acquired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static void store_echo
(
  byte c,                         /*  character to echo         */
  dsm_item_type ** echo_item_ptr  /*  output: pointer to buffer */
)
{


#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  ds3g_siolib_port_e_type open_port = ds3g_siolib_get_active_port();
  dsat_modem_port_e_type  modem_port = dsatctab_get_modem_port_id (open_port);
  if ( (modem_port != DSAT_MODEM_PORT_NONE) && 
      ((dsat_num_item_type)dsatutil_get_val(
                         DSAT_BASIC_E_IDX,0,modem_port,NUM_TYPE) == 1 ) 
#else
  if ( ((dsat_num_item_type)dsatutil_get_val(
                                  DSAT_BASIC_E_IDX,0,0,NUM_TYPE) == 1 ) 
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
       /* Do not store echo for external clients  */
       && (ds3g_siolib_get_active_port () != DS3G_SIOLIB_CLIENT_VSP_PORT)   
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT */
)
  {
    /*-----------------------------------------------------------------------
      Check if a free buffer is avalable. If none, require a   
      new buffer space.                                        
    -----------------------------------------------------------------------*/
    if ( *echo_item_ptr == NULL )
    {
      *echo_item_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    }

    /*-----------------------------------------------------------------------
      Stores the echo character into the buffer.               
    -----------------------------------------------------------------------*/
    (*echo_item_ptr)->data_ptr[(*echo_item_ptr)->used++] = c;
    
    if ( (*echo_item_ptr)->used >= (*echo_item_ptr)->size )
    {
       /* Make flag true for avoid displaying echo character in response */
      (*echo_item_ptr)->app_field = 1;     
      dsatrsp_send_response( *echo_item_ptr, FALSE );
      *echo_item_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    }
  }
} /* store_echo( ) */


/*===========================================================================

FUNCTION DUMP_ECHO

DESCRIPTION
  This function outputs the echo characters stored by store_echo( ) 
  to the serial port via  dsatrsp_send_response(), and sets the item 
  pointer argument to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dump_echo
(
  dsm_item_type ** echo_item_ptr  /*  buffer containing echo chars  */
)
{
#ifdef FEATURE_DSAT_MSM_FUSION
  ds3g_siolib_port_e_type current_active_port = ds3g_siolib_get_active_port();
#endif /* FEATURE_DSAT_MSM_FUSION */

  if ( *echo_item_ptr != NULL )
  {
    if ( (*echo_item_ptr)->used != 0 )
    {
      if ( dsatcmdp_processing_async_cmd && 
           ( ( preprocessor_mode != DSATI_SMS_MSG_ENTRY_MODE) &&
             ( preprocessor_mode != DSATI_MD5_MSG_ENTRY_MODE)))
      {
        /* Don't process the AT command as ATCoP is */
        /* processing async_cmd */
        dsm_free_packet(echo_item_ptr);
      }
      else
      {
        /* Make flag true for avoid displaying echo character in response */
        (*echo_item_ptr)->app_field = 1;
#ifndef FEATURE_DSAT_MSM_FUSION
        dsatrsp_send_response( *echo_item_ptr, FALSE );
#else
        /* Do not ECHO back for HSUART1, HSUART2, HSUART3 unless the 
           preprocessor_mode is DSATI_SMS_MSG_ENTRY_MODE */
        if(( current_active_port == DS3G_SIOLIB_HSUART1_PORT )||
           ( current_active_port == DS3G_SIOLIB_HSUART2_PORT )||
           ( current_active_port == DS3G_SIOLIB_HSUART3_PORT ))
        {
          if( (preprocessor_mode == DSATI_SMS_MSG_ENTRY_MODE))
        {
          dsatrsp_send_response( *echo_item_ptr, FALSE );
        }
        else
        {
          dsm_free_packet(echo_item_ptr);
        }
        }
        /* Echo for all other ports */
        else
        {
          dsatrsp_send_response( *echo_item_ptr, FALSE );
        }
#endif /* FEATURE_DSAT_MSM_FUSION */
      }
    }
    else
    {
      dsm_free_packet(echo_item_ptr);
    }
    *echo_item_ptr = NULL;
  }
} /* dump_echo( ) */


