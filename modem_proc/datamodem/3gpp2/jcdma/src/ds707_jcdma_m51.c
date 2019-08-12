/*===========================================================================

                          D S 7 0 7 _ J C D M A _ M 5 1
GENERAL DESCRIPTION

 This file contains the API's and functionality to implement the JCDMA
 M51 data functionality.

 Copyright (c) 2002-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_jcdma_m51.c_v   1.6   02 Dec 2002 17:30:06   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/jcdma/src/ds707_jcdma_m51.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
02/08/12    sk     Feature cleanup.
03/30/11    ms     Global variable cleanup.
10/18/10    op     Migrated to MSG 2.0 macros
04/29/08    mga    Added return in some cases after error checking
01/15/08    mga    Merged changes to change ERR_FATAL to MSG_ERROR
08/15/07    ack    Fixed Lint Warnings
07/27/07  ack/squ  Fixed Compile Warnings
10/28/03    ak     Add function to see if pkt or async/fax allowed.
09/18/03    km     Wrapped support for SIO_BITRATE_230400 under 
                   FEATURE_UART_TCXO_CLK_FREQ
12/02/02    ak     Changed make_pkt_call func name to include m51.
10/22/02    rsl    Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/02/02    ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"

#include "cm.h"
#include "ds3gsiolib.h"
#include "ds707_jcdma_m51.h"
#include "ds707_pkt_mgr.h"
#include "ds707_roaming.h"
#include "ds707_nv_util.h"
#include "ds3gcfgmgr.h"
#include "ds707_so_pkt.h"
#include "dstaski.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "nv.h"
#include "ps_phys_link.h"
#include "sio.h"
#include "sys.h"

/*===========================================================================
                         PUBLIC VARIABLES
===========================================================================*/
/*lint -save -e641*/
/*---------------------------------------------------------------------------
  Variables used to track state of the M51* values.
---------------------------------------------------------------------------*/
static sys_jcdma_m511_e_type     ds707_jcdma_m511_val;
static sys_jcdma_m512_e_type     ds707_jcdma_m512_val;
static sys_jcdma_m513_e_type     ds707_jcdma_m513_val;

/*===========================================================================
                          LOCAL VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  Variable used for writing to NV
---------------------------------------------------------------------------*/
static nv_item_type  ds707_jcdmai_nv_item;

/*===========================================================================
FUNCTION VERIFY_JCDMA_MODE_VAL()

DESCRIPTION
  This function checks the boundary condition for the NV item values and 
  if they exceed boundary,  sets them to the Min or Max value accordingly

PARAMETERS
  jcdma_nv_item_m51X_mode_val: The value that is set in the NV item
  jcdma_val_type: Specifies the m51X type (i.e 511, 512 or 513)

RETURN VALUE
   returns the max or min value if the boundary condition is exceeded for the
   particular m51X type. Else returns the same value that is set in the nv item

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int verify_jcdma_mode_val 
(
  uint8 jcdma_nv_item_m51X_mode_val,
  jcdma_mode_val jcdma_val_type,
  uint8 *return_val
)
{
  switch (jcdma_val_type) 
  {
  case JCDMA_VAL_511:
    /* Comparison with SYS_JCDMA_M511_MIN is not done since jcdma_nv_item_m51X_mode_val
     is Uint8 */
    if (jcdma_nv_item_m51X_mode_val >= (uint8)SYS_JCDMA_M511_MAX) 
    {
      *return_val =  (uint8)SYS_JCDMA_M511_MAX;
    }
    else
    {
      *return_val =  jcdma_nv_item_m51X_mode_val;
    }
    break;
  case JCDMA_VAL_512:
    /* Comparison with SYS_JCDMA_M512_MIN is not done since jcdma_nv_item_m51X_mode_val
     is Uint8 */
    if (jcdma_nv_item_m51X_mode_val >= (uint8)SYS_JCDMA_M512_MAX) 
    {
      *return_val =  (uint8)SYS_JCDMA_M512_MAX;
    }
    else
    {
      *return_val =  jcdma_nv_item_m51X_mode_val;
    }
    break;
  case JCDMA_VAL_513:
    /* Comparison with SYS_JCDMA_M513_MIN is not done since jcdma_nv_item_m51X_mode_val
     is Uint8 */
    if (jcdma_nv_item_m51X_mode_val >= (uint8)SYS_JCDMA_M513_MAX) 
    {
      *return_val =  (uint8)SYS_JCDMA_M513_MAX;
    }
    else
    {
      *return_val =  jcdma_nv_item_m51X_mode_val;
    }
    break;
  default:
    return -1;
  }
  return 0;
} /* verify_jcdma_mode_val */

/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_JCDMAI_SET_SIO_BAUDRATE

DESCRIPTION   Sets the SIO baudrate based on M51* settings.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdmai_set_sio_baudrate(void)
{
#ifdef FEATURE_AUTOBAUD
  sio_bitrate_type   bitrate = SIO_BITRATE_AUTO;
#else
  sio_bitrate_type   bitrate = SIO_BITRATE_19200;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  switch(ds707_jcdma_m513_val)
  {
    case SYS_JCDMA_M513_AUTO:
      if ((ds707_jcdma_m511_val == SYS_JCDMA_M511_PKT) &&
          (ds707_jcdma_m512_val == SYS_JCDMA_M512_HIGH)
         )
      {
#ifdef FEATURE_UART_TCXO_CLK_FREQ
        bitrate = SIO_BITRATE_230400;
#else 
        bitrate = SIO_BITRATE_115200;
#endif
      }
      else
      {
        bitrate = SIO_BITRATE_115200;
      }
      break;

    case SYS_JCDMA_M513_19200:
#ifdef FEATURE_AUTOBAUD
   bitrate = SIO_BITRATE_AUTO;
#else
   bitrate = SIO_BITRATE_19200;
#endif      
      break;

    case SYS_JCDMA_M513_115200:
      bitrate = SIO_BITRATE_115200;
      break;

#ifdef FEATURE_UART_TCXO_CLK_FREQ
    case SYS_JCDMA_M513_230400:
        bitrate = SIO_BITRATE_230400;
      break;
#endif

    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown M513 val %d",
                      ds707_jcdma_m513_val);
      break;
  }

  /*-------------------------------------------------------------------------
    The following function updates IPR in NV as well as updates the SIO
    baudrate.  This function is special in that it can be called from any
    task (i.e, it does not require being in the DS task context).
  -------------------------------------------------------------------------*/
  dsat_change_sio_params (SIO_PORT_UART_MAIN,
                          SIO_FCTL_BEST,
                          SIO_FCTL_BEST,
                          bitrate,
                          TRUE);
} /* ds707_jcdmai_set_sio_baudrate() */

/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_JCDMA_M51_INIT

DESCRIPTION   Initializes the JCDMA M51 module.  Called only once, at 
              startup.  Will read NV and if the values are bogus, then will
              write defaults.

DEPENDENCIES  Executes only in DS task.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_m51_init(void)
{
  nv_stat_enum_type status;
  uint8 jcdma_m51_return_val;
  uint32 err_code = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds707_roaming_is_curr_mode_jcdma() == FALSE )
  {
   DATA_IS707_MSG0(MSG_LEGACY_ERROR,"JCDMA NV not enabled");
   return;
  }
  do
  {
    /*-------------------------------------------------------------------------
      M51   1
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_JCDMA_M511_MODE_I, &ds707_jcdmai_nv_item, ds707_curr_subs_id());
    if( status == NV_NOTACTIVE_S )
    {
      /*-----------------------------------------------------------------------
         At time of first boot, the NV item may not have been initialized yet.
         Initialize it from statically-initialized default.
      -----------------------------------------------------------------------*/
      ds707_jcdmai_nv_item.jcdma_m511_mode = (uint8)DS707_JCDMA_M511_DEF;
      (void) ds3gcfgmgr_write_legacy_nv_ex( NV_JCDMA_M511_MODE_I, &ds707_jcdmai_nv_item, ds707_curr_subs_id() );
    }
    else if( status != NV_DONE_S)
    {
      err_code = 1 << JCDMA_VAL_511;
      break;
    }
    /*-------------------------------------------------------------------------
      Store number retrieved from NV.
    -------------------------------------------------------------------------*/
    if ( verify_jcdma_mode_val(ds707_jcdmai_nv_item.jcdma_m511_mode, 
                               JCDMA_VAL_511, 
                               &jcdma_m51_return_val) == 0)
    {
      ds707_jcdma_m511_val = (sys_jcdma_m511_e_type)jcdma_m51_return_val;
    }
    else
    {
      err_code |= 1 << JCDMA_VAL_511;
    }
    
    /*-------------------------------------------------------------------------
      M51   2
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_JCDMA_M512_MODE_I, &ds707_jcdmai_nv_item, ds707_curr_subs_id());
    if( status == NV_NOTACTIVE_S )
    {
      /*-----------------------------------------------------------------------
         At time of first boot, the NV item may not have been initialized yet.
         Initialize it from statically-initialized default.
      -----------------------------------------------------------------------*/
      ds707_jcdmai_nv_item.jcdma_m512_mode = (uint8)DS707_JCDMA_M512_DEF;
      (void) ds3gcfgmgr_write_legacy_nv_ex( NV_JCDMA_M512_MODE_I, &ds707_jcdmai_nv_item, ds707_curr_subs_id() );
    }
    else if( status != NV_DONE_S)
    {
      err_code = 1 << JCDMA_VAL_512;
      break;
    }
    /*-------------------------------------------------------------------------
      Store number retrieved from NV.
    -------------------------------------------------------------------------*/
    if ( verify_jcdma_mode_val (ds707_jcdmai_nv_item.jcdma_m512_mode,
                                JCDMA_VAL_512,
                                &jcdma_m51_return_val) == 0)
    {
      ds707_jcdma_m512_val = (sys_jcdma_m512_e_type)jcdma_m51_return_val;
    }
    else
    {
      err_code |= 1 << JCDMA_VAL_512;
    }
  
    /*-------------------------------------------------------------------------
      M51   3
    -------------------------------------------------------------------------*/
    status = ds3gcfgmgr_read_legacy_nv_ex( NV_JCDMA_M513_MODE_I, &ds707_jcdmai_nv_item, ds707_curr_subs_id());
    if( status == NV_NOTACTIVE_S )
    {
      /*-----------------------------------------------------------------------
         At time of first boot, the NV item may not have been initialized yet.
         Initialize it from statically-initialized default.
      -----------------------------------------------------------------------*/
      ds707_jcdmai_nv_item.jcdma_m513_mode = (uint8)DS707_JCDMA_M513_DEF;
      (void) ds3gcfgmgr_write_legacy_nv_ex( NV_JCDMA_M513_MODE_I, &ds707_jcdmai_nv_item, ds707_curr_subs_id());
    }
    else if( status != NV_DONE_S)
    {
      err_code = 1 << JCDMA_VAL_513;
      break;
    }
    /*-------------------------------------------------------------------------
      Store number retrieved from NV.
    -------------------------------------------------------------------------*/
    if (verify_jcdma_mode_val (ds707_jcdmai_nv_item.jcdma_m513_mode,
                               JCDMA_VAL_513,
                               &jcdma_m51_return_val) == 0)
    {
      ds707_jcdma_m513_val = (sys_jcdma_m513_e_type)jcdma_m51_return_val;
    }
    else
    {
      err_code |= 1 << JCDMA_VAL_513;
    }
  }while (0);
  
  if (err_code)
  {
    if (status != NV_DONE_S)
    {
      DATA_IS707_MSG2(MSG_LEGACY_ERROR,  
                      "Bad NV read status %d for M51 %d", 
                      status, err_code);
      return;
    }
    else
    {
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, 
                      "Unable to verify jcdma m51 values with err_code %d",
                      err_code);
    }
  }

  /*-------------------------------------------------------------------------
    Based on these settings, set up mobile to follow user settings.
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();

  /*-------------------------------------------------------------------------
    Update the SIO baud rate, based on NV setting.
  -------------------------------------------------------------------------*/
  ds707_jcdmai_set_sio_baudrate();

  /* Register handlers for M51 mode changes */
  if (cm_m51_mode_reg(ds707_jcdma_chng_m511_mode,
				  ds707_jcdma_chng_m512_mode,
				      ds707_jcdma_chng_m513_mode)== FALSE)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Feature FEATURE_JCDMA_1X is undefined");
  }

} /* ds707_jcdma_m51_init() */


/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M511_MODE

DESCRIPTION   Called by UI when the user selects a particular M511 setting.
              If it is okay to update the setting, will send a msg to DS
              so that the value can be changed.  This is because the new
              value is written into NV, and that should happen in DS task.

DEPENDENCIES  None

RETURN VALUE  TRUE - it is okay to change the setting.
              FALSE - invalid input.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_chng_m511_mode
(
  sys_jcdma_m511_e_type  new_mode
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((new_mode <= SYS_JCDMA_M511_MIN) ||
      (new_mode >= SYS_JCDMA_M511_MAX)
     )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid mode for M511 %d", new_mode);
  }
  else
  {
    ds707_jcdma_m511_val = new_mode;
    ds707_so_pkt_recal();
    ds707_jcdmai_set_sio_baudrate();
  }
  return(TRUE);
}

/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M512_MODE

DESCRIPTION   Called by UI when the user selects a particular M512 setting.
              If it is okay to change the settings, then this will send a
              msg to DS, so that it can write the value into NV.  This 
              function is called in UI context.

DEPENDENCIES  None

RETURN VALUE  TRUE - it is okay to change the setting.
              FALSE - we are dormant or in an active call.  User not allowed
                      to change the setting.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_chng_m512_mode
(
  sys_jcdma_m512_e_type  new_mode
)
{ 
  ps_phys_link_type   *phys_link_ptr;
  boolean              ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  phys_link_ptr = ds707_pkt_get_pri_phys_link_ptr();

  if ((new_mode <= SYS_JCDMA_M512_MIN) ||
      (new_mode >= SYS_JCDMA_M512_MAX)
     )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid mode for M512 %d", new_mode);
  }
  else if ((PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_NULL) ||
           (PS_PHYS_LINK_GET_STATE(phys_link_ptr) == PHYS_LINK_GOING_NULL))
  {
    ds707_jcdma_m512_val = new_mode;
    ds707_so_pkt_recal();
    ds707_jcdmai_set_sio_baudrate();
    ret_val = TRUE;
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Cant do M512 - data call not null");
  }

  return(ret_val);
}/* ds707_jcdma_chng_m512_mode() */

/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M513_MODE

DESCRIPTION   Called by UI when the user selects a particular M513 setting.

DEPENDENCIES  None

RETURN VALUE  TRUE if input value is okay.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean  ds707_jcdma_chng_m513_mode
(
  sys_jcdma_m513_e_type  new_mode
)
{
  boolean          ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((new_mode <= SYS_JCDMA_M513_MIN) ||
      (new_mode >= SYS_JCDMA_M513_MAX)
     )
  {
    DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Invalid mode for M513 %d", new_mode);
  }
  else
  {
    ds707_jcdma_m513_val = new_mode;
    ds707_so_pkt_recal();
    ds707_jcdmai_set_sio_baudrate();
    ret_val = TRUE;
  }

  return(ret_val);
} /* ds707_jcdma_chng_m513_mode() */

/*===========================================================================
FUNCTION      DS707_JCDMA_M51_MAKE_PKT_CALL

DESCRIPTION   Returns TRUE or FALSE, indicating if the next call should be
              pkt or async/fax.  This is decided by the M511 setting.

DEPENDENCIES  None

RETURN VALUE  TRUE  - M511 indicates make a packet call
              FALSE - M511 indicates a fax/async call.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_make_pkt_call(void)
{ 
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_jcdma_m511_val == SYS_JCDMA_M511_PKT)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_jcdma_make_pkt_call() */


/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M513_VIA_AT

DESCRIPTION   Called by AT when the user enters an AT+IPR or AT$QCTER
              command.
              
              Note that the input is not the enum used in other functions.
              It must be translated correctly.

DEPENDENCIES  None

RETURN VALUE  TRUE if input value is okay.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean  ds707_jcdma_chng_m513_via_at
(
  sio_bitrate_type   new_ipr_val
)
{
  sys_jcdma_m513_e_type   new_mode;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  switch (new_ipr_val)
  {
#ifdef FEATURE_AUTOBAUD
   case SIO_BITRATE_AUTO:
#else
   case SIO_BITRATE_19200:
#endif    
      new_mode = SYS_JCDMA_M513_19200;
      break;
    case SIO_BITRATE_115200:
      new_mode = SYS_JCDMA_M513_115200;
      break;
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    case SIO_BITRATE_230400:
      new_mode = SYS_JCDMA_M513_230400;
      break;
#endif
    default:
      DATA_IS707_MSG1(MSG_LEGACY_ERROR, "Unknown SIO bitrate %d", new_ipr_val);
      ASSERT(0);
      return(FALSE);
  }

  /*-------------------------------------------------------------------------
     Write value to NV
  -------------------------------------------------------------------------*/
  ASSERT(IS_IN_DS_TASK());
  ds707_jcdma_m513_val = new_mode;
  ds707_jcdmai_nv_item.jcdma_m513_mode = (uint8)ds707_jcdma_m513_val;
  (void) ds3gcfgmgr_write_legacy_nv_ex( NV_JCDMA_M513_MODE_I, &ds707_jcdmai_nv_item, ds707_curr_subs_id() );

  /*-------------------------------------------------------------------------
    Re-calibrate SO's and then set the baudrate.  This will do the write
    to the SIO driver.  Note that this will also store the value in NV.
  -------------------------------------------------------------------------*/
  ds707_so_pkt_recal();
  ds707_jcdmai_set_sio_baudrate();

  return(TRUE);
} /* ds707_jcdma_chng_m513_via_at() */


#ifdef FEATURE_DATA_IS707_ASYNC	
/*===========================================================================
FUNCTION      DS707_JCDMA_M51_IS_ASYNC_FAX_ALLOWED

DESCRIPTION   Returns TRUE/FALSE, indicating if the M511 value allows
              async/fax.

DEPENDENCIES  None

RETURN VALUE  TRUE if async/fax allowed.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_is_async_fax_allowed(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_jcdma_m511_val == SYS_JCDMA_M511_ASYNC_FAX)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_jcdma_m51_is_async_fax_allowed() */
#endif /* FEATURE_DATA_IS707_ASYNC */


/*===========================================================================
FUNCTION      DS707_JCDMA_M51_IS_PKT_ALLOWED

DESCRIPTION   Returns TRUE/FALSE, indicating if the M511 value allows
              packet.

DEPENDENCIES  None

RETURN VALUE  TRUE if packet allowed.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_is_pkt_allowed(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_jcdma_m511_val == SYS_JCDMA_M511_PKT)
  {
    return(TRUE);
  }
  else
  {
    return(FALSE);
  }
} /* ds707_jcdma_m51_is_pkt_allowed() */

/*===========================================================================
FUNCTION      DS707_JCDMA_M51_GET_M511_VAL

DESCRIPTION   Returns m511 value

DEPENDENCIES  None

RETURN VALUE  m511 value..

SIDE EFFECTS  None
===========================================================================*/
int32 ds707_jcdma_m51_get_val
(
  jcdma_mode_val m511_val_type
)
{
  int32  m51_value = -1;

  switch(m511_val_type)
  {
    case JCDMA_VAL_511:
          m51_value = (int32) ds707_jcdma_m511_val;
          break;
    case JCDMA_VAL_512:
          m51_value = (int32) ds707_jcdma_m512_val;
          break;
    case JCDMA_VAL_513: 
          m51_value = (int32) ds707_jcdma_m513_val;
          break;
    default: 
          ASSERT(0);
          break;
  }

  return m51_value;
}
/*===========================================================================
FUNCTION      DS707_JCDMA_M51_GET_M512_VAL

DESCRIPTION   Returns m512 value

DEPENDENCIES  None

RETURN VALUE  m512 value..

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_m51_set_val
(
  jcdma_mode_val m511_val_type,
  int32         jcdma_m51_val
)
{
  DATA_IS707_MSG1(MSG_LEGACY_MED, "m511 type passed: %d",
                  m511_val_type);

  switch(m511_val_type)
  {
    case JCDMA_VAL_511 :
          ds707_jcdma_m511_val = (sys_jcdma_m511_e_type)jcdma_m51_val;
          break;
    case JCDMA_VAL_512 :
          ds707_jcdma_m512_val = (sys_jcdma_m512_e_type)jcdma_m51_val;
          break;
    case JCDMA_VAL_513 : 
          ds707_jcdma_m513_val = (sys_jcdma_m513_e_type)jcdma_m51_val;
          break;
    default : 
          break;
  }

  return;
}
/*lint -restore Restore lint error 641*/

