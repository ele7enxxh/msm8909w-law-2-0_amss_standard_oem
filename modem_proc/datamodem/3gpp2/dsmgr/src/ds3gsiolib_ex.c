/*===========================================================================

                        D A T A   S E R V I C E S

                    E X T E N D E D     D 3 G S I O L I B 

                          S O U R C E   F I L E


DESCRIPTION
  This file contains the definitions of data structures,
  enumerated constants, and function prototypes required for the
  Extended DS3GSIOLIB.
  
Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat.h_v   1.10   08 Nov 2002 12:46:24   wxie  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/src/ds3gsiolib_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/18/16   skc     Fixed issue related to cd assert for DUN call.
08/03/16   skc     Adding support for ESC SEQUENCE for PS Data call.
08/12/14   tk      Fixed issue with ATCoP preprocessor mode in CSVT call.
06/27/14   tk      Added support for Dynamic ATCoP.
05/14/12   ss      Fixed the issue of back to back DUN calls not working
                   as PC was timing out for CONNECT response.
11/15/11   sa      Fixed crash during ds3g_siolib_ex_change_mode.
05/26/11   dvk     Global Variables Cleanup
12/17/10   ttv     Replaced q_last_get function with dsm_queue_cnt.
05/14/10   vs      Initial Extended file

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_LTE_OFFTARGET_TEST 
#include "dsut.h"
#endif /* FEATURE_DATA_LTE_OFFTARGET_TEST */

#include "ds3gsiolib_ex.h"
#include "dsati.h"

#include "data_err_suppr.h"
#include "data_msg.h"
#include "ds3gtimer.h"

/*===========================================================================

                      PUBLIC FUNCTION DEFINITIONS

===========================================================================*/

/* DO NOT Expose outside this file*/

static ds3g_siolib_port_e_type ds3g_siolib_csd_port = DS3G_SIOLIB_PORT_NONE;
static ds3g_siolib_port_e_type ds3g_siolib_psd_port = DS3G_SIOLIB_PORT_NONE;

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_CSD_PORT

DESCRIPTION
  This function sets the CSD port. 

DEPENDENCIES
  None
 
PARAMETERS 
  CSD port Id
 
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_ex_set_csd_port
(
  ds3g_siolib_port_e_type port_id
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_PSD_PORT

DESCRIPTION
  This function sets the PSD port. 

DEPENDENCIES
  None
 
PARAMETERS 
  PSD port Id
 
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_ex_set_psd_port
(
  ds3g_siolib_port_e_type port_id
);

/*=========================================================================

FUNCTION DS3G_SIOLIB_DETERMINE_PORT_TO_BE_ALLOCATED

DESCRIPTION
  This function returns the port that will be allocated.

DEPENDENCIES
 
PARAMETERS 
  Port Allocation Information
 
RETURN VALUE
  Port Id of the port to be allocated

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_determine_port_to_be_allocated
(
  ds3g_siolib_port_alloc_info_s_type  *alloc_info_ptr
)
{
  ds3g_siolib_port_e_type return_port = DS3G_SIOLIB_PORT_NONE;

  /*-------------------------------------------------------------------------
    Check port allocation mode.  For automatic mode, assign modem
    port based on active port or cached answer port.  For manual
    mode, use the port specified in parameter from caller.
  -------------------------------------------------------------------------*/
  if( DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC == alloc_info_ptr->mode )
  {
    /*-------------------------------------------------------------------------
      Incoming calls manually answered will use the manual port.
      Auto-answer mode will use the port which set the auto-answer
      state.
    ------------------------------------------------------------------------*/
    if( DS3G_SIOLIB_PORT_CALL_DIR_TERMINATED == alloc_info_ptr->call_dir ) 
    {
      if( IS_ANSWERMODE( DS3G_SIOLIB_ANSWER_MODE_MANUAL ) )
      {
        /* Clear the manual mode flag */
        CLEAR_ANSWERMODE( DS3G_SIOLIB_ANSWER_MODE_MANUAL );

        return_port = ds3g_siolib_get_active_port();
        /* Clear the manual port which is no longer required. 
           For abort call, this manual port is not cleared as 
           this is based on set concept only. */
        ds3g_siolib_answer_info.manual_port = DS3G_SIOLIB_PORT_NONE;      
      }
      else if( IS_ANSWERMODE( DS3G_SIOLIB_ANSWER_MODE_AUTO ) &&
               (DS3G_SIOLIB_PORT_NONE != ds3g_siolib_answer_info.port))
      {
        return_port = ds3g_siolib_answer_info.port;
      }
      else 
      {
        DS_SIO_MSG0_ERROR("Invalid Answer Port on an Incoming Data Call");
      }
    }
    else 
    {
      /* Mobile originated calls use current active port. */
      return_port = ds3g_siolib_get_active_port();
      DS_SIO_MSG1_ERROR("Return active port: %d",return_port );
    }
  }
  else
  {
    /*-------------------------------------------------------------------------
      Asssign modem port to port specified by caller.  Port state is
      validated by later processing.
    -------------------------------------------------------------------------*/
    return_port = alloc_info_ptr->manual_port;
    DS_SIO_MSG1_ERROR("Return manual port: %d",return_port );
  }

  return return_port;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_ALLOCATE_PORT

DESCRIPTION
  This function attempts to allocate an SIO port for an external data
  call.  It implements a simple semaphore flag for the port resource
  to ensure only one data call per SIO port.  A port must be
  unallocated and in AUTODETECT state for it to be available.  On
  successful allocation, a valid port identifier is returned.
  Otherwise, the NONE identifier is returned.  It is the calling
  routines responsibility to honor the resource semaphore.
                
DEPENDENCIES
  None
 
PARAMETERS 
  1. Port allocation information
  2. Client Id which is trying to allocate the port

RETURN VALUE
  DS3G_SIOLIB_PORTID_NONE on error condition, valid port identifier otherwise.

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type  ds3g_siolib_ex_allocate_port
(
  ds3g_siolib_port_alloc_info_s_type  *alloc_info_ptr,
  ds_call_type_e_type                  call_type
)
{
  ds3g_siolib_state_info_type *state;
  ds3g_siolib_port_e_type      return_port = DS3G_SIOLIB_PORT_NONE;
  uint8                        i;
  ds3g_siolib_port_e_type      port_id;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(alloc_info_ptr == NULL)
  {
    DATA_ERR_FATAL("NULL passed for alloc_info_ptr");
    return return_port;
  }  

  port_id = ds3g_siolib_determine_port_to_be_allocated(alloc_info_ptr);
  DS_SIO_MSG1_MED ("ds3g_siolib_ex_allocate_port: port_id %d", port_id);
  if(port_id == DS3G_SIOLIB_PORT_NONE)
  {
    return DS3G_SIOLIB_PORT_NONE;
  }

  for(i=0; i<DS3G_SIOLIB_PORTID_MAX; i++)
  {
    DS_SIO_MSG1_MED( "ds3g_siolib_get_index_from_allocated_port: "
                     "ds3g_siolib_port_allocated[i] %d", 
                      ds3g_siolib_port_allocated[i]); 
    
    if( DS3G_SIOLIB_PORT_NONE == ds3g_siolib_port_allocated[i] )
    {
      state = ds3g_siolib_get_ds3g_siolib_state(port_id);
   
      if(state == NULL)
      {
        DS_SIO_MSG0_ERROR("NULL state from ds3g_siolib_get_ds3g_siolib_state");
        return DS3G_SIOLIB_PORT_NONE;
      }       
   
      /*---------------------------------------------------------------------
         Ensure modem port in AUTODETECT state.
      ---------------------------------------------------------------------*/
      if( DS3G_SIOLIB_AUTODETECT_SERIAL_STATE == state->serial_info.serial_state )
      {
        /*---------------------------------------------------------------------
          Port is available so allocate it to caller.
         ---------------------------------------------------------------------*/
        ds3g_siolib_port_allocated[i] = port_id;
 
        ds3g_siolib_ex_set_current_call_type(call_type);
        if(call_type == DS_CALL_TYPE_CKT)
        {
          ds3g_siolib_ex_set_csd_port(port_id);
        }
        else if(call_type == DS_CALL_TYPE_PKT)
        {
          ds3g_siolib_ex_set_psd_port(port_id);
        }
      
        ds3g_siolib_set_modem_port(port_id);
   
        return_port = port_id;
        ds3g_siolib_port_signal_status[i].CD_asserted_already = FALSE;
      }
      else
      {
        DS_SIO_MSG0_ERROR ("SIOLIB port not in auto-detect state");
      }

      break;     
    }
  }

  if(i == DS3G_SIOLIB_PORTID_MAX )
  {
  	/* No ports available */
	  DS_SIO_MSG0_ERROR("Failed to allocate SIO Port");
  }

  DS_SIO_MSG2_MED ("ds3g_siolib_ex_allocate_port: allocated port %d port_index %d", 
                   return_port, i );

  return return_port;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_RELEASE_PORT

DESCRIPTION
  This function releases a previously allocated SIO port, clearing the
  semaphore flag.

DEPENDENCIES
  None
 
PARAMETERS 
  1. Port Id
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_ex_release_port
(
  ds3g_siolib_port_e_type port_id
)
{
   ds3g_siolib_portid_e_type port_index = 
    ds3g_siolib_get_index_from_allocated_port(port_id);

  /* Ensure legitimate portid */
  if ((port_index < DS3G_SIOLIB_PORTID_MAX) && 
      (port_index > DS3G_SIOLIB_PORTID_NONE))
  { 
    /* Check integrity of operation */
    if (DS3G_SIOLIB_PORT_NONE == ds3g_siolib_port_allocated[port_index])
    {
      DS_SIO_MSG1_ERROR ("Attempting to release unallocated port[%d]",
                         port_index);
    }
    else
    {
      ds3g_siolib_last_port_allocated[port_index] = ds3g_siolib_port_allocated[port_index];
      /* Clear allocation flag for specified port */
      ds3g_siolib_port_allocated[port_index] = DS3G_SIOLIB_PORT_NONE;
      ds3g_siolib_reset_modem_port();

#ifdef FEATURE_IRDA
      if (DS3G_SIOLIB_IRCOMM_PORT == port_index)
      {
        /* incase IrDA has just been disabled from the UI, the data call is now
        gone down so finish off by closing the IRCOMM port via RDM */
        irda_api_close_ircomm_port();
      }
#endif /* FEATURE_IRDA */

      DS_SIO_MSG1_MED("SIO Port[%d] released",port_index);
    }

  }
  else
  {
    DS_SIO_MSG1_ERROR ("Bad port identifier: %d",port_index);
  }
}
/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_FLUSH_TX_CB

DESCRIPTION
  This is the callback function for sio_flush_tx

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void ds3g_siolib_ex_flush_tx_cb(void)
{
  DS_SIO_MSG0_HIGH("Flush Tx CB called");
}

/*===========================================================================
FUNCTION DS3G_SIOLIB_EX_REGISTER_CALLBACK_FUNC

DESCRIPTION
  This function registers a mode specific call back function table. 

  Caller should check the return value of this function to see if the
  registration succeeded.
 
DEPENDENCIES
  None
 
PARAMETERS 
  Function table to be registered
  Port Id
 
RETURN VALUE
  Returns TRUE if a func_tbl is not registered with SIOLIB already and
  if the passed in function pointer is not NULL, otherwise returns FALSE;

SIDE EFFECTS
  If a mode-specific handler function table was previously registered, 
  it will be over-written.
===========================================================================*/

boolean ds3g_siolib_ex_register_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr,        /* Handler function table */
  ds3g_siolib_port_e_type     port_id
)
{
  boolean result = FALSE;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	  port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DATA_ERR_FATAL("Port Id is DS3G_SIOLIB_PORT_NONE");
    return result;
  }
  
  state = ds3g_siolib_get_ds3g_siolib_state(port_id);

  if(NULL == state)
  {
    DATA_ERR_FATAL(" InValid Port State");
    return result;
  } 
  /*-------------------------------------------------------------------------
    Check to see if the func. ptr. thats being passed in is not null.
  -------------------------------------------------------------------------*/
  if ( func_tbl_ptr != NULL )
  {
    state->func_tbl = func_tbl_ptr;
    if (ds3g_siolib_port_is_in_active_call(port_id))
    {
      /*-----------------------------------------------------------------------
      This is to let ATCOP know that SIO port is now busy
      -----------------------------------------------------------------------*/
      dsat_sio_is_busy(port_id);
    }
    result = TRUE;
  }
  else
  {
    DATA_ERR_FATAL("Callback func. ptr = NULL in SIOLIB");
    return FALSE;
  }
  return result;
} /* ds3g_siolib_ex_register_callback_func */

/*===========================================================================
FUNCTION DS3G_SIOLIB_EX_DEREGISTER_CALLBACK_FUNC

DESCRIPTION
  This function deregisters a mode-specific handler function table.

DEPENDENCIES
  None
 
PARAMETERS 
  1. Function table to be de-registered
  2. Port Id
 
RETURN VALUE
  Returns TRUE if deregister happened successfully. 
  Returns FALSE if func ptr passed in does not match siolib table value.

SIDE EFFECTS
  Resets the callback function table pointer to null. 
===========================================================================*/
boolean  ds3g_siolib_ex_deregister_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr,
  ds3g_siolib_port_e_type     port_id
)
{
  boolean result = FALSE;
  ds3g_siolib_state_info_type *state = NULL;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	  port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DS_SIO_MSG0_ERROR("port_id is DS3G_SIOLIB_PORT_NONE");
    return FALSE;
  }

  /* Lint error handler */
  DATA_MDM_ARG_NOT_CONST(func_tbl_ptr);

  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  if(NULL == state)
  {
    DATA_ERR_FATAL(" InValid Port State");
    return result;
  } 
  /*-------------------------------------------------------------------------
  Check to see if the func. ptr. that was registered before is the one
  trying to deregister.
  -------------------------------------------------------------------------*/
  if (state->func_tbl != func_tbl_ptr)
  {
    DS_SIO_MSG0_ERROR ("Callback func. ptr doesn't match SIOLIB func_tbl value");
  }
  else if(func_tbl_ptr == NULL)
  {
    DATA_ERR_FATAL("Callback func. ptr = NULL in SIOLIB");
    return result;
  }
  else
  {
    state->func_tbl = NULL;

    if (ds3g_siolib_port_is_in_active_call(port_id))
    {
      /*-----------------------------------------------------------------------
      This is to let ATCOP know that SIO port is now free for use
      -----------------------------------------------------------------------*/
      dsat_sio_is_free(port_id);
    }
    result = TRUE;
  }
  return result;
} /* ds3g_siolib_ex_deregister_callback_func() */

/*=========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_CD_STATE

DESCRIPTION
  This function sets the state of the Carrier Detect (CD) signal, based on
  the following:
  1) &C setting
     &C0 - CD is always ON
     &C1 - CD is ON when the remote carrier is ON
     &C2 - CD is always ON except for 1sec 'wink' when a data call is
           disconnected
  2) Remote carrier state
  3) End-of-call winking (only if &C=2)
  4) SMS message entry
     CD is ON during SMS message entry (overrides &C setting)

  This function should be called when a serial port is opened, and whenever
  any event occurs that impacts the CD state.

DEPENDENCIES
  No action is taken if there is no port on which data call exists.
 
PARAMETERS 
  1. DCD event
  2. Port Id
 
RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_set_cd_state
(
  ds3g_siolib_dcd_event_e_type  dcd_event,      /* Event that occurred, if any */
  ds3g_siolib_port_e_type       port_id
)
{
  ds3g_siolib_portid_e_type port_index;
  
  #define DS3G_SIOLIB_ATCOP_PORT_INDEX DS3G_SIOLIB_PORTID_MAX
  boolean                   dcd_state = OFF;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	    port_id > DS3G_SIOLIB_PORT_MAX)
  {
    DS_SIO_MSG0_ERROR("port_id invalid");
    return DS3G_SIOLIB_NO_PORT_OPEN;
  }
  
  if(ds3g_siolib_get_client_type() == DS3G_SIOLIB_CLIENT_ATCOP )
  {
    
	  port_index = DS3G_SIOLIB_ATCOP_PORT_INDEX;
  }

  else
  {
    if ((port_index = ds3g_siolib_get_index_from_allocated_port(port_id)) ==
          DS3G_SIOLIB_PORTID_NONE)
    {
      port_index = ds3g_siolib_get_index_from_last_allocated_port(port_id);
    }
  }

  if(port_index == DS3G_SIOLIB_PORTID_NONE)
  {
    return DS3G_SIOLIB_NO_PORT_OPEN;
  }

  /*------------------------------------------------------------------------- 
    First, update the appropriate internal state variable based on the type
    of event that occurred.
    -------------------------------------------------------------------------*/
  switch( dcd_event )
  {
     case DS3G_SIOLIB_REMOTE_CARRIER_ON:
       ds3g_siolib_port_signal_status[port_index].remote_carrier_state = ON;
       break;
     case DS3G_SIOLIB_REMOTE_CARRIER_OFF:
       ds3g_siolib_port_signal_status[port_index].remote_carrier_state = OFF;
      if( (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE) == 2 )
       {
         switch (port_index)
         {
            case DS3G_SIOLIB_PORTID_0:
              ds3gi_start_timer( DS3G_TIMER_WINK_DCD_1, DS3G_SIOLIB_DCD_WINK_INTERVAL);
              ds3g_siolib_port_signal_status[port_index].dcd_winking = ON;
              break;
            case DS3G_SIOLIB_PORTID_1:
              ds3gi_start_timer( DS3G_TIMER_WINK_DCD_2, DS3G_SIOLIB_DCD_WINK_INTERVAL);
              ds3g_siolib_port_signal_status[port_index].dcd_winking = ON;
              break;
            default:
              /*do nothing*/
              break;
         }
       }
  		break;
     case DS3G_SIOLIB_CD_WINKING_END:
       ds3g_siolib_port_signal_status[port_index].dcd_winking = OFF;
       break;
     case DS3G_SIOLIB_SMS_MSG_START:
       ds3g_siolib_port_signal_status[port_index].sms_entry = ON;
       break;

     case DS3G_SIOLIB_SMS_MSG_END:
       ds3g_siolib_port_signal_status[port_index].sms_entry = OFF;
       break;
     case DS3G_SIOLIB_EVENT_NONE:
     default:
       /* No action needed */
  		break;
  } /* switch( dcd_event ) */

  /*------------------------------------------------------------------------- 
    Now set the CD state appropriately, based on the &C setting and related
    state variables.
    -------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------- 
    &C0 => CD is always ON.
    -------------------------------------------------------------------------*/

  if( (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE) == 0 )
  {
    dcd_state = ON;
  }

  /*------------------------------------------------------------------------- 
    &C1 => CD is ON only if the remote carrier is ON.
    -------------------------------------------------------------------------*/
  else if( (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE) == 1 )
  {
    
    if( ds3g_siolib_port_signal_status[port_index].remote_carrier_state == ON )
    {
      dcd_state = ON;
    }
    else
    {
      dcd_state = OFF;
    }
  }

  /*-------------------------------------------------------------------------
    &C2 => CD is always ON, except during the end-of-call winking interval.
  -------------------------------------------------------------------------*/
  else if( (dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_C_IDX,0,0,NUM_TYPE) == 2 )
  {
    if( ds3g_siolib_port_signal_status[port_index].dcd_winking == ON )
    {
      dcd_state = OFF;
    }
    else
    {
      dcd_state = ON;
    }
  }

/*------------------------------------------------------------------------- 
  If an SMS message is being entered, CD should be ON. This will override
  the &C setting above.
  -------------------------------------------------------------------------*/
  if( ds3g_siolib_port_signal_status[port_index].sms_entry == ON )
  {
    dcd_state = ON;
  }
  /*------------------------------------------------------------------------- 
    Set the state of the Carrier Detect (CD) signal.
    -------------------------------------------------------------------------*/
  /* CD should not be re-asserted when DSR is deasserted
     when dsat_amp_s_val is DS3G_DSR_CONNECT_ON DSR will be
     in de-assert state and it will be asserted only at the
     time of CONNECT
   */

  if( (dcd_state) && ((dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE)== DS3G_DSR_ALWAYS_ON ))
  {
    /* Do not reassert if it is already asserted*/ 
    if( !ds3g_siolib_port_signal_status[port_index].CD_asserted_already )
    {
      if(( ret_val = ds3g_siolib_cd_assert(port_id))== DS3G_SIOLIB_OK )
      {
        ds3g_siolib_port_signal_status[port_index].CD_asserted_already = TRUE;
      }
    }
    else
    {
      DS_SIO_MSG1_HIGH("CD already asserted for port %d", port_id);
    }
  }
  else
  {
    /* If dcd_state is OFF or dsat_amp_s_val is DS3G_DSR_CONNECT_ON
       and if CD is already asserted then de-assert it */
    if( ds3g_siolib_port_signal_status[port_index].CD_asserted_already )
    {
      if( (ret_val = ds3g_siolib_cd_deassert(port_id))== DS3G_SIOLIB_OK )
      {
        ds3g_siolib_port_signal_status[port_index].CD_asserted_already = FALSE;
      }
    }
    else
    {
      DS_SIO_MSG1_MED("CD already de-asserted for port %d", port_id);
    }

  }
  return( ret_val );

}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_CALL_COMING_UP

DESCRIPTION
  This function sets the call coming up boolean variable which determines
  if a data call is in the process of coming up.

DEPENDENCIES
  None
 
PARAMETERS 
  1. Flag indicating that the call is coming up
  2. Port Id
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void  ds3g_siolib_ex_set_call_coming_up
(
  ds3g_siolib_port_e_type port_id,
  boolean                 busy
)
{
  ds3g_siolib_state_info_type *state;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	    port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DATA_ERR_FATAL("port_id invalid");
	return;
  }

  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  state->call_coming_up = busy;
} /* ds3g_siolib_set_call_coming_up() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_IOCTL

DESCRIPTION
  This function allows control of open stream for the port on which data call
  is established.

DEPENDENCIES
  No action is taken if there is no port on which data call exists.
 
PARAMETERS 
  1. SIO IOCTL command
  2. Argument to be passed along with the command.
  3. Port Id

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_ex_ioctl
(
  sio_ioctl_cmd_type       cmd,
  sio_ioctl_param_type    *param,
  ds3g_siolib_port_e_type  port_id  /* Port Id */
)
{
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	  port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DS_SIO_MSG0_ERROR("Port id = PORT_NONE");
    return;
  }
  
  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  if(NULL == state)
  {
    DS_SIO_MSG0_ERROR(" InValid Port State");
    return ;
  } 

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    sio_ioctl ( state->serial_info.stream_id, cmd, param );
  }
  else
  {
    DS_SIO_MSG1_ERROR("siolib_stream_id[%d] = SIO_NO_STREAM_ID or Port Close "
                      "in Progress", port_id);
  }
} /* ds3g_siolib_ex_ioctl() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_CHANGE_MODE
DESCRIPTION
  This function changes the mode of the currently open serial port to the
  specified mode. After the mode change, received data bytes will be stored
  in the SIO RX watermark , and the rx_func will be called each time data is
  enqueued to the watermark.

  Depending on the new serial mode, callback functions are registered for
  serial events of interest, namely:
    Autodetect Mode - Packet detection callback
    Rawdata Mode    - Escape signal detection callback
    Packet Mode     - Escape signal detection callback

DEPENDENCIES
  No action is taken if there is no port on which a data call exists.
 
PARAMETERS 
  1. New Serial Mode
  2. SIO Rx Watermark
  3. SIO Tx Watermark
  4. Function to be invoked everytime data bytes are rx'ed
  5. Port Id

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_change_mode
(
  sio_stream_mode_type    stream_mode,                   /* New stream mode */
  dsm_watermark_type      *rx_wm_ptr,     /* Watermark for rx data from SIO */
  dsm_watermark_type      *tx_wm_ptr,       /* Watermark for tx data to SIO */
  sio_rx_func_ptr_type    rx_func_ptr,                   /* Called each time
                                                            data bytes rx'ed*/
  ds3g_siolib_port_e_type port_id                                /* Port Id */
)
{
  sio_open_type         open_param;
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;
  ds3g_siolib_state_info_type *state = NULL;
  uint32 max_packet_size = 0;
  uint32 current_cnt     = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	  port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DATA_ERR_FATAL("Port Id is DS3G_SIOLIB_PORT_NONE");
    return DS3G_SIOLIB_NO_PORT_OPEN;
  }
  
  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  if(NULL == state)
  {
    DATA_ERR_FATAL(" InValid Port State");
    return DS3G_SIOLIB_NO_PORT_OPEN;
  } 
  
  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    /*-----------------------------------------------------------------------
      Set the stream mode, rx/tx watermarks and rx function to the values
      provided by the caller of this function.
    -----------------------------------------------------------------------*/
    open_param.stream_mode = stream_mode;

    /*-----------------------------------------------------------------------
    In the case of autodetect mode, SIOLIB maintains the watermarks, so
    the values passed in will be null, hence set them to predefined 
    watermarks.
    -----------------------------------------------------------------------*/
    if ( stream_mode == SIO_DS_AUTODETECT_MODE ) 
    {
      if ( (ds3g_siolib_port_is_in_active_call(port_id)) && 
            ((dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE) == DS3G_DSR_CONNECT_ON) )
      {
        (void) ds3g_siolib_dsr_deassert(port_id );
        DS_SIO_MSG0_MED(" AT&S = 1 and stream mode is AUTODETECT; De-assert DSR");
      }
  
      if ( ( rx_wm_ptr == NULL ) && ( tx_wm_ptr == NULL ) )
      {
        open_param.rx_queue = &state->serial_info.rx_wm_ptr;
        open_param.tx_queue = &state->serial_info.tx_wm_ptr;
        open_param.rx_func_ptr = NULL;
        ds3g_siolib_setup_watermarks(port_id);
        state->at_state = DS3G_SIOLIB_COMMAND_AT_STATE;
       }
       else
       {
         DATA_ERR_FATAL("WM pointers not null in autodetect mode ");
         return DS3G_SIOLIB_NO_PORT_OPEN;
       }
    }
    else
    {
      if ( (ds3g_siolib_port_is_in_active_call(port_id)) && 
           ((dsat_num_item_type)dsatutil_get_val(
                                     DSAT_BASIC_N_S_IDX,0,0,NUM_TYPE) == DS3G_DSR_CONNECT_ON) )
      {
        (void)ds3g_siolib_dsr_assert( port_id );
        DS_SIO_MSG0_MED(" AT&S = 1 and stream mode is not in AUTODETECT;  Assert DSR");
      }
  
      open_param.rx_queue = rx_wm_ptr;
      open_param.tx_queue = tx_wm_ptr;
      open_param.rx_func_ptr = rx_func_ptr;
  
    /*-----------------------------------------------------------------------
      Empty autodetect watermarks and queues when switching to any mode
      other than autodetect.
    -----------------------------------------------------------------------*/
      state->at_state = DS3G_SIOLIB_ONLINE_DATA_AT_STATE;
      dsm_empty_queue ( &state->serial_info.rx_wm_ptr );

      if ( (current_cnt = dsm_queue_cnt(&state->serial_info.rx_wm_ptr)) != 0 )
      {
        DS_SIO_MSG1_ERROR("Watermark just emptied but from_queue not empty. wm_count=%d",
                  current_cnt);
      }

      /* If the Tx WM is non-empty flush the pending data on the serial port */
      if ( !dsm_is_wm_empty(&state->serial_info.tx_wm_ptr) )
      {
        (void)ds3g_siolib_ex_flush_tx(ds3g_siolib_ex_flush_tx_cb, port_id);
      }

      if ( (port_id != DS3G_SIOLIB_USB_CDC_PORT) &&
           (current_cnt = dsm_queue_cnt(&state->serial_info.tx_wm_ptr)) != 0 )
      {
        DS_SIO_MSG1_ERROR("Watermark just emptied but to_queue not empty. wm_count=%d",
                  current_cnt);
      }  
    }
  
    /*-----------------------------------------------------------------------
     Set the port id to the currently assigned value.
    -----------------------------------------------------------------------*/
    open_param.port_id = ds3g_siolib_map_ds_port(port_id);
  
    /*-----------------------------------------------------------------------
      Set the remaining parameters to the appropriate values.
    -----------------------------------------------------------------------*/
    open_param.rx_bitrate = SIO_BITRATE_BEST;
    open_param.tx_bitrate = SIO_BITRATE_BEST;
    open_param.tail_char_used = FALSE;
    open_param.rx_flow = SIO_FCTL_BEST;
    open_param.tx_flow = SIO_FCTL_BEST;
  
    /*-----------------------------------------------------------------------
      Change the serial mode to the specified mode.
     -----------------------------------------------------------------------*/
    ioctl_param.open_ptr = &open_param;
    sio_ioctl( state->serial_info.stream_id, SIO_IOCTL_CHANGE_MODE, &ioctl_param );
  
    /*-----------------------------------------------------------------------
      Now do any additional configuration based on the mode.
    -----------------------------------------------------------------------*/
    switch( stream_mode )
    {
      case SIO_DS_AUTODETECT_MODE:
        DS_SIO_MSG0_HIGH("DS SIO = AUTODETECT");
        break;
  
      case SIO_DS_RAWDATA_MODE:
        /*-------------------------------------------------------------------
          Register callback function for Escape sequence (+++) detection.
        -------------------------------------------------------------------*/
        DS_SIO_MSG0_HIGH("DS SIO = RAWDATA");
        ioctl_param.record_escape_func_ptr = ds3g_siolib_escape_detected_cb;
        sio_ioctl( state->serial_info.stream_id, 
        SIO_IOCTL_RECORD_ESC_FUNC_PTR, &ioctl_param );

        if(rx_wm_ptr != NULL)
        {
          /*-------------------------------------------------------------------
            Set max packet size for rx path appropriately..
          -------------------------------------------------------------------*/
          ioctl_param.packet_size = 0;
          sio_ioctl(state->serial_info.stream_id,
                    SIO_IOCTL_GET_PACKET_SIZE,
                    &ioctl_param);
          max_packet_size = 
                  rx_wm_ptr->dont_exceed_cnt - rx_wm_ptr->hi_watermark;
          if (ioctl_param.packet_size != max_packet_size) 
          {
            DS_SIO_MSG1_HIGH("Setting Packet size to %d",max_packet_size);
            ioctl_param.packet_size = max_packet_size;
            sio_ioctl(state->serial_info.stream_id,
                      SIO_IOCTL_SET_PACKET_SIZE,
                      &ioctl_param);
          }
        }
        break;
  
      case SIO_DS_PKT_MODE:
        DS_SIO_MSG0_HIGH("DS SIO = PACKET");
        /* Setting call back for escape sequence */
        ioctl_param.record_escape_func_ptr = ds3g_siolib_escape_detected_cb;
        sio_ioctl( state->serial_info.stream_id, 
        SIO_IOCTL_RECORD_ESC_FUNC_PTR, &ioctl_param );
		
        /*-------------------------------------------------------------------
          Set max packet size for rx path appropriately..
        -------------------------------------------------------------------*/
        ioctl_param.packet_size = 0;
        sio_ioctl(state->serial_info.stream_id,
                  SIO_IOCTL_GET_PACKET_SIZE,
                  &ioctl_param);
  
        if ((ioctl_param.packet_size > 0) && (rx_wm_ptr != NULL))
        {
          max_packet_size = 
          rx_wm_ptr->dont_exceed_cnt - rx_wm_ptr->hi_watermark;
  
          if (ioctl_param.packet_size > max_packet_size) 
          {
            ioctl_param.packet_size = max_packet_size;
            sio_ioctl(state->serial_info.stream_id,
                      SIO_IOCTL_SET_PACKET_SIZE,
                      &ioctl_param);
          }
        }
        break;
  
      default:
        DS_SIO_MSG1_ERROR( "Serial mode set to invalid mode: %d", stream_mode );
        break;
    }
  
    state->serial_info.serial_state = ds3gi_convert_serial_mode(stream_mode);

  }/* if stream_id != NO_STREAM_ID */

  else
  {
    DS_SIO_MSG1_ERROR ("Changing mode w/bad stream id %d", state->serial_info.stream_id);
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }
  
  return( ret_val );

}/* ds3g_siolib_ex_change_mode */


/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_IS_DTR_ASSERTED

DESCRIPTION
  Checks the state of the Data Terminal Ready (DTR) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not DTR is asserted.

DEPENDENCIES
  No action is taken if there is no port on which a data call exists.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_is_dtr_asserted
(
  boolean                  *dtr_status, /* Set to TRUE if DTR asserted, or 
                                          FALSE if not */
  ds3g_siolib_port_e_type  port_id     /* Port Id */
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	  port_id >= DS3G_SIOLIB_PORT_MAX)
  {
        DS_SIO_MSG0_ERROR("siolib_modem_port = PORT_NONE");
        return ret_val;
  }

  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  if(NULL == state)
  {
    DS_SIO_MSG0_ERROR(" InValid Port State");
    return ret_val;
  } 

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    ioctl_param.dte_ready_asserted = dtr_status;
    sio_ioctl( state->serial_info.stream_id, 
               SIO_IOCTL_DTE_READY_ASSERTED, &ioctl_param );
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("Checking DTR w/bad stream id %d", state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_ex_is_dtr_asserted() */



/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_IS_RTS_ASSERTED

DESCRIPTION
  Checks the state of the Request To Send (RTS) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not RTS is asserted.

DEPENDENCIES
  If there is no port currently open, the rts_status value has no meaning.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_is_rts_asserted
(
  boolean                 *rts_status, /* Set to TRUE if RTS asserted, 
                                          or FALSE if not */
  ds3g_siolib_port_e_type  port_id  /* Port Id */
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	    port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DS_SIO_MSG0_ERROR("port is = PORT_NONE");
    return ret_val;
  }
 
  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  if(NULL == state)
  {
    DS_SIO_MSG0_ERROR(" InValid Port State");
    return ret_val;
  } 

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
  	(state->serial_info.close_in_progress == FALSE) )
  {
    ioctl_param.rts_asserted = rts_status;
    sio_ioctl( state->serial_info.stream_id, 
  			 SIO_IOCTL_GET_CURRENT_RTS, &ioctl_param );
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("Checking RTS w/bad stream id %d", 
              state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_is_rts_asserted() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SETUP_TX

DESCRIPTION
  This function queues data for transmission over the serial interface.
  Additionally, Autodetect Mode is re-armed, if specified by the caller.
  Typically, the re-arm feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port currently open.
 
PARAMETERS 
  1. DSM item containing data to transmit
  2. Re-arm Autodetect Mode
  3. Port Id

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_setup_tx
(
  dsm_item_type            *tx_ptr,            /* DSM item containing 
                                                  data to transmit */
  boolean                  rearm_autodetect,   /* TRUE = Re-arm Autodetect 
                                                  Mode */
  ds3g_siolib_port_e_type  port_id             /* Port Id */
)
{ 
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(port_id <= DS3G_SIOLIB_PORT_NONE ||
	 port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DATA_ERR_FATAL("port id ==DS3G_SIOLIB_PORT_NONE");
    return ret_val;
  }

  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  if(NULL == state)
  {
    DATA_ERR_FATAL(" InValid Port State");
    return ret_val;
  } 

  /*-------------------------------------------------------------------------
    Send the input data to the underlying SIO device.  If we haven't been 
    assigned a valid stream ID by SIO, trash the input data.
  -------------------------------------------------------------------------*/
  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    if( rearm_autodetect )
    {
      if( state->serial_info.serial_state == DS3G_SIOLIB_AUTODETECT_SERIAL_STATE )
      {
        sio_ioctl( state->serial_info.stream_id, 
                   SIO_IOCTL_ENABLE_AUTODETECT, &ioctl_param );
      }
      else
      {
        DS_SIO_MSG0_HIGH( "Autodt rearmed in non-Autodt serial mode");
      }
    }

    sio_transmit( state->serial_info.stream_id, tx_ptr );
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    dsm_free_packet ( &tx_ptr );
    DS_SIO_MSG1_ERROR ("Transmitting data w/bad stream id %d", 
              state->serial_info.stream_id);
  }

  return( ret_val );
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_FLUSH_TX

DESCRIPTION
  This function is used to schedule the SIO TX queue to be flushed when
  changed to and from online-command and online-data modes.

  The flush_func_ptr_cb parameter is used to pass a callback function to
  SIO that is called when the flush operation has completed.  The callback
  should be used to notify the task which requested the flush operation
  that the TX queue is now empty.

DEPENDENCIES
  No action is taken if there is no port currently open.
 
PARAMETERS 
  1. Callback to be invoked when flush operation is complete.
  2. Port Id

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_flush_tx
(
  void                     (*flush_func_ptr_cb)(void),
  ds3g_siolib_port_e_type  port_id /* Port Id */
)
{
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
        port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DS_SIO_MSG0_ERROR("port id = PORT_NONE");
    return ret_val;
  }

  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  if(NULL == state)
  {
    DS_SIO_MSG0_ERROR(" InValid Port State");
    return ret_val;
  } 

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    sio_flush_tx( state->serial_info.stream_id, flush_func_ptr_cb );
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR ("Flushing Tx w/bad stream id %d", state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_ex_flush_tx() */

/*===========================================================================
FUNCTION DS3G_SIOLIB_EX_SET_INBOUND_FLOW

DESCRIPTION
  This function controls the enabling or diabling of inbound flow ctl on a 
  specified port.
  This function was required because inbound flow ctl. can be changed
  from several places for different reasons, and we need an arbitrator
  function to handle those changes.

  Basically, the function enables inbound flow, when no caller wants it
  disabled. If even one caller wants the flow disabled, the inbound flow
  is disabled.

DEPENDENCIES
  This is the only function that should be used to toggle the inbound flow.
  This is important because the function keeps track of the state of inbound
  flow, and if the state of inbound flow is changed
 
PARAMETERS 
  1. Flwo mask indicating the kind of flow being enabled/disabled
  2. Flag indicating whether flow is enabled or disabled
  2. Port Id

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/

void ds3g_siolib_ex_set_inbound_flow
( 
  uint32 in_mask,          /* input mask of calling function               */
  ds3g_flow_e_type flow,  /* specifies if flow is to be enabled or disabled */
  ds3g_siolib_port_e_type  port_id   /* DS3G_SIOLIB port id                */
)
{
  sio_ioctl_param_type           ioctl_param;
  ds3g_siolib_state_info_type    *state = NULL;
  sio_stream_id_type             stream_id;
  boolean                        send_sio_ioctl = FALSE;
  sio_ioctl_cmd_type             sio_ioctl_cmd  =
                                        SIO_IOCTL_INBOUND_FLOW_DISABLE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	    port_id >= DS3G_SIOLIB_PORT_MAX)
  {
	  DS_SIO_MSG0_ERROR("port_id invalid");
	  return;
  }

  state = ds3g_siolib_get_ds3g_siolib_state(port_id);

  if(NULL == state)
  {
    DS_SIO_MSG0_ERROR(" InValid Port State");
    return;
  } 

  if ( state->serial_info.stream_id != SIO_NO_STREAM_ID )
  {
    /*-------------------------------------------------------------------------
      If DS is not assigned a serial port, then doing flow control is
      meaningless.
    -------------------------------------------------------------------------*/
    if ( state->serial_info.serial_state ==
         DS3G_SIOLIB_NULL_SERIAL_STATE )
    {
      return;
    }

    /*-------------------------------------------------------------------------
      If we are asked to enable flow control then OR the input mask to static
      mask to get the new value of the static mask.
      If asked to disable flow control, then AND the static mask to the
      complement of the input mask to get the new static mask.
    -------------------------------------------------------------------------*/
    ds3g_siolibi_rx_sig_crit_sect_enter();

    DS_FLOW_CTRL_SET_MASK (flow, state->serial_info.flow_ctl_mask, in_mask);
    stream_id = state->serial_info.stream_id;

    /*-------------------------------------------------------------------------
      If static mask says that all functions have enabled flow control, do the
      actual function call to enable flow control. Even if one function call
      has disabled flow, CTS will stay disabled.
    -------------------------------------------------------------------------*/
    if (state->serial_info.flow_ctl_mask == DS_FLOW_IS_ENABLED)
    {
      if(state->flow_status == DS3G_SIOLIB_FLOW_DISABLED || 
         state->flow_status == DS3G_SIOLIB_FLOW_STATUS_NONE)
      {
        state->flow_status = DS3G_SIOLIB_FLOW_ENABLED;
        send_sio_ioctl     = TRUE;
        sio_ioctl_cmd      = SIO_IOCTL_INBOUND_FLOW_ENABLE;
        DS_SIO_MSG1_HIGH("ds3g_siolib_set_port_inbound_flow() "
                         "Enable flow Port_id: %d",
                         (uint32)port_id);
      }
      else
      {
        send_sio_ioctl = FALSE;
        DS_SIO_MSG1_HIGH("Flow Already Enabled, skipping the FLOW_ENABLE"
                         "command Port_id: %d",
                         (uint32)port_id);
      }
    }
    else
    {
      if(state->flow_status == DS3G_SIOLIB_FLOW_ENABLED || 
         state->flow_status == DS3G_SIOLIB_FLOW_STATUS_NONE)
      {
        state->flow_status = DS3G_SIOLIB_FLOW_DISABLED;
        send_sio_ioctl     = TRUE;
        sio_ioctl_cmd      = SIO_IOCTL_INBOUND_FLOW_DISABLE;
        DS_SIO_MSG1_HIGH("ds3g_siolib_set_port_inbound_flow(): "
                         "Disable flow Port_id: %d",
                         (uint32)port_id);
      }
      else
      {
        send_sio_ioctl = FALSE;
        DS_SIO_MSG1_HIGH("Flow Already Disabled, skipping the FLOW_DISABLE "
                         "command Port_id: %d", (uint32)port_id);
      }
    }

    ds3g_siolibi_rx_sig_crit_sect_leave();

    if (TRUE == send_sio_ioctl)
    {
      sio_ioctl( stream_id, sio_ioctl_cmd, &ioctl_param );
    }

    DS_SIO_MSG2_HIGH("Call Msk: 0x%x FlowMsk: 0x%x",
                      in_mask, state->serial_info.flow_ctl_mask);
  }

} /* ds3g_siolib_set_port_inbound_flow() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_GET_CSD_PORT

DESCRIPTION
  This function returns the CSD port, if allocated. THe function will return
  DS3G_SIOLIB_PORT_NONE if CSD port is not allocated.

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  CSD port, if allocated
  DS3G_SIOLIB_PORT_NONE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_ex_get_csd_port
(
  void
)
{
  return ds3g_siolib_csd_port;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_GET_PSD_PORT

DESCRIPTION
  This function returns the PSD port, if allocated. THe function will return
  DS3G_SIOLIB_PORT_NONE if PSD port is not allocated.

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
  PSD port, if allocated
  DS3G_SIOLIB_PORT_NONE, otherwise

SIDE EFFECTS
  None

===========================================================================*/
ds3g_siolib_port_e_type ds3g_siolib_ex_get_psd_port
(
  void
)
{
  return ds3g_siolib_psd_port;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_CSD_PORT

DESCRIPTION
  This function sets the CSD port. 

DEPENDENCIES
  None
 
PARAMETERS 
  CSD port Id
 
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_ex_set_csd_port
(
  ds3g_siolib_port_e_type port_id
)
{
  DS_SIO_MSG1_HIGH("Set CSD Port: %d", port_id);
  ds3g_siolib_csd_port = port_id;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_PSD_PORT

DESCRIPTION
  This function sets the PSD port. 

DEPENDENCIES
  None
 
PARAMETERS 
  PSD port Id
 
RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_ex_set_psd_port
(
  ds3g_siolib_port_e_type port_id
)
{
  DS_SIO_MSG1_HIGH("Set PSD Port: %d", port_id);
  ds3g_siolib_psd_port = port_id;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_GET_CURRENT_CALL_TYPE

DESCRIPTION
  This function returns the current call type.

DEPENDENCIES
  None
 
PARAMETERS 
  None
 
RETURN VALUE
 Current call type

SIDE EFFECTS
  None

===========================================================================*/
ds_call_type_e_type ds3g_siolib_ex_get_current_call_type
(
  void
)
{
  return current_ds_call_type;
}


/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_SET_CURRENT_CALL_TYPE

DESCRIPTION
  This function sets the current call type.

DEPENDENCIES
  None
 
PARAMETERS 
  DS Call Type
 
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void ds3g_siolib_ex_set_current_call_type
(
  ds_call_type_e_type call_type
)
{
  DS_SIO_MSG1_HIGH("Store current call type: %d", call_type);
  current_ds_call_type = call_type;
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_EX_IS_CD_ASSERTED

DESCRIPTION
  Checks the state of the Carrier Detect (CD) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not DTR is asserted.

DEPENDENCIES

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ex_is_cd_asserted
(
  boolean                 *cd_status,     /* Set to TRUE if DTR asserted, 
                                              or FALSE if not */
  ds3g_siolib_port_e_type  port_id
)
{
  sio_ioctl_param_type  ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  ds3g_siolib_state_info_type *state = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (port_id <= DS3G_SIOLIB_PORT_NONE || 
	    port_id >= DS3G_SIOLIB_PORT_MAX)
  {
    DS_SIO_MSG0_ERROR("Port id = PORT_NONE");
    return ret_val;
  }

  state = ds3g_siolib_get_ds3g_siolib_state(port_id);
  if(NULL == state)
  {
    DS_SIO_MSG0_ERROR(" InValid Port State");
    return ret_val;
  } 

  if( (state->serial_info.stream_id != SIO_NO_STREAM_ID) &&
      (state->serial_info.close_in_progress == FALSE) )
  {
    ioctl_param.cd_asserted = cd_status;
    sio_ioctl( state->serial_info.stream_id, 
               SIO_IOCTL_GET_CURRENT_CD, &ioctl_param );
    ret_val = DS3G_SIOLIB_OK;
  }
  else
  {
    DS_SIO_MSG1_ERROR("Checking CD w/bad stream id %d", state->serial_info.stream_id);
  }

  return( ret_val );

} /* ds3g_siolib_ex_is_cd_asserted() */

