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
  dsat_process_sio_command
    This function is to be called from the data services task to process 
    incoming data from the serial port, when the AT command processor
    is in AT command mode or online command mode.
    
EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT    
  dsatprep_set_preprocessor_mode
    Changes the SIO data preprocessor mode of operation.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The AT command processor must be initialized by calling the function
  dsat_init on initial power-up and calling dsat_init_service_mode
  each time a new operating service mode is entered.

  Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatprep_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
-------    ---     ---------------------------------------------------------
06/27/14   tk      Initial revision (created file for Dynamic ATCoP).

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
#include "dsatdl.h"
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

/* point to current dsm item under processing */
dsm_item_type * atprep_dsm_item_ptr = NULL;

/*--------------------------------------------------------------------------
   SIO data preprocessor mode of operation.
---------------------------------------------------------------------------*/
dsati_preproc_mode_enum_type preprocessor_mode = DSATI_AT_CMD_MODE;

/*---------------------------------------------------------------------------
    Local function declarations to allow the desired function ordering.    
---------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION dsatprep_set_preprocessor_mode

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

void dsatprep_set_preprocessor_mode
(
  dsati_preproc_mode_enum_type mode,    /* mode to set data preprocessor to */
  ds3g_siolib_port_e_type      port     /* port to set preprocessor mode    */
)
{
  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatprepi_set_preprocessor_mode_fp);

  dsatdl_vtable.dsatprepi_set_preprocessor_mode_fp(mode, port);

  return;
} /* dsatprep_set_preprocessor_mode( ) */


/*===========================================================================

FUNCTION DSAT_PROCESS_SIO_COMMAND

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
boolean dsat_process_sio_command
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
)
{
  boolean result;

  DSAT_DL_CHECK_SYMBOL_ADDR(dsatdl_vtable.dsatprep_process_sio_command_fp);
  result = dsatdl_vtable.dsatprep_process_sio_command_fp(at_state, wm_item_ptr);

  dsatcmdp_track_async_cmd();

  return result;
} /* dsat_process_sio_command( ) */

