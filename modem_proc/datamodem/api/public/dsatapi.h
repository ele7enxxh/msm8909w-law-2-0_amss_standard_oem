#ifndef DSATAPI_H
#define DSATAPI_H

/** 
  @file dsatapi.h  
  @brief
  This file contains the ATCOP functions that
  are remotely used by the Applications processor in the multiprocessor
  builds where the data is on the Modem processor.
*/
/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the Modem_Data_mainpage file. 
		 To change any of the the file/group text for the PDF, edit the 
		 Modem_Data_mainpage file, or contact Tech Pubs.

         The above description for this file is part of the 
		 "dsatapi" group description in the Modem_Data_mainpage 
		 file. 
*/
/*===========================================================================

                         D S A T  A P I 

  Copyright (c) 2007 - 2010,2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsatapi.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
30/10/12   ad      Added ATCoP Reset FWD function as a Public API.
10/06/10   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
07/12/10   km      Doxygen tags added
10/14/09   ss      CMI AU level cleanup.
03/04/09   sa      AU level CMI modifications.
08/08/07   sa      Created file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "sio.h"

/** @addtogroup dsatapi
@{ */

/*===========================================================================
                     TYPEDEFS & VARIABLES
===========================================================================*/



/*===========================================================================
                 AT Command Processor Data Structures
===========================================================================*/

/** @} */ /* end_addtogroup dsatapi */
/** @addtogroup dsatapi
@{ */

/** Number of item types; unsigned integer. */
typedef unsigned int dsat_num_item_type;

/*
Caller should call ATcoP API with valid status call back function.
ATcop will intimate reboot status(success/failure) to the caller by this call back function.
Command status 
          0  FAILURE 
          1. SUCCESS
User info data
         Contain user data pointer received in API call.
*/

typedef void (*dsat_cmd_status_cb_fn_type)
(
  boolean  cmd_status,      /* CMD status    */
  void *user_info_ptr     /* Client data   */
);

/** @} */ /* end_addtogroup dsatapi */
/** @addtogroup dsatapi
@{ */

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/** 
  Called by other modules to write the value to the current 
  baud rate variable, ds_atcop_ipr_val. Whenever the baud rate is changed
  it should also be entered into the NV if FEATURE_DS_DEFAULT_BITRATE_NV
  is defined. This function also writes to the NV if that feature is
  defined.

  The second parameter in this function is a boolean type. It informs 
  the function if the baud rate should be changed <i>now</i> or later (later
  implies after sending an acknowledgement). This parameter is required
  because when the UI changes the baud rate, the baud rate should take 
  effect immediately. But when an AT command changes the baud rate, 
  the baud rate should be changed after an "OK" is sent to 
  the Terminal Equipment type 2 (TE2). 
  
  @param new_baud_rate [in]    Requested baud rate. 
  @param change_baud_now [in]  Immediate or post ACK.

  @return
  None.

  @dependencies 
  This function does not check whether or not autobaud is enabled. It
  is the responsibility of the calling function to ascertain
  that if FEATURE_AUTOBAUD is not enabled, the value 0 should
  not be passed.
 
  @sideeffects
  Modifies the value of the dsat_ipr_val and its corresponding
  value in the NV, ds_default_baud. 
*/

extern void dsat_change_baud
(
    sio_bitrate_type new_baud_rate,
    boolean change_baud_now
);
/*~ FUNCTION dsat_change_baud */

 
/** 
  Called by other modules to get the current 
  baud rate value set by the +IPR AT command. 

  @return
  Baud rate.
    
  @dependencies
  This function does not check whether or not autobaud is enabled. It 
  is the responsibility of the calling function to ascertain that if
  FEATURE_AUTOBAUD is not enabled, the value 0 should not be passed.
    
  @sideeffects
  Modifies the value of the dsat_ipr_val and its corresponding
  value in the NV, ds_default_baud.
*/ 

extern dsat_num_item_type dsat_get_baud
( 
  void 
);
/*~ FUNCTION dsat_get_baud */

/**
  Called by other modules to get any S register value
  set by the S register command.

  @param sreg_num [in]  Register number whose value must be returned.

  @return
  S register value -- Corresponding S register is present. \n
  -1 -- S register is not present.

  @dependencies
  Whenever a new S register is added, the corresponding entry should
  be made in this function as well.
*/ 

extern int32 dsat_get_sreg_val
(
  uint8 sreg_num
);/*~ FUNCTION dsat_get_sreg_val */
/*===========================================================================
FUNCTION DSAT_RESET_CMD_FORWARDING

DESCRIPTION  
  Caller can call this API for phone reboot request.ATcop will check and honour this request.

PARAMETERS 

  status_func_cb  ATcop will intimate reboot status(success/failure) to caller by calling this call back function.
  user_info_ptr    Contain user data.
  
DEPENDENCIES 
  AT forward demon up and running.

RETURN VALUE
  1 : Input prameter validation successful
  0 : Otherwise

SIDE EFFECTS  None
===========================================================================*/


boolean dsat_reset_cmd_forwarding
(
  dsat_cmd_status_cb_fn_type   status_func_cb,      /* Status callback */
  void                        *user_info_ptr        /* Client data       */
); /*~ FUNCTION dsat_reset_cmd_forwarding */

/*===========================================================================
FUNCTION DSAT_SHUTDOWN_CMD_FORWARDING

DESCRIPTION  
  Caller can call this API for phone shutdown request.ATcop will check and honour this request.

PARAMETERS 

  status_func_cb  ATcop will intimate reboot status(success/failure) to caller by calling this call back function.
  user_info_ptr    Contain user data.
  
DEPENDENCIES 
  AT forward demon up and running.

RETURN VALUE
  1 : Input prameter validation successful
  0 : Otherwise

SIDE EFFECTS  None
===========================================================================*/
boolean dsat_shutdown_cmd_forwarding
(
  dsat_cmd_status_cb_fn_type   status_func_cb,      /* Status callback */
  void                        *user_info_ptr        /* Client data       */
); /*~ FUNCTION dsat_reset_cmd_forwarding */


/** @} */ /* end_addtogroup dsatapi */

#endif /* DSATAPI_H */
