/*===========================================================================

               D S 7 0 7 _ H I _ S P D _ C L K
               
GENERAL DESCRIPTION
  This file is used to increase/decrease the CPU clock speed when a 
  pkt data call starts.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Must call ds707_hi_spd_clk_init() at power-up.

  Copyright (c) 2004-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_hi_spd_clk.c#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/21/12    msh    TCB changes due to Core changes  
08/11/11    var    Resolve Compiler warnings & KW errors.
06/07/11    mg     Added featurization !defined(FEATURE_MCPM) for clkregime APIs
03/15/11    mg     Global variable cleanup
02/11/11    ack    Global Variable Cleanup
01/11/11    ssh    Featurize NPA code
09/13/10    op     Changed DS707_CBACK_MSG macro to use MSG 2.0 format
08/03/10    ssh    Create impulse client once and never delete
07/21/10    ssh    Added impulse client for data - kicks in on high fwd um wm.
10/21/09    ac     Change to support FEATURE_NPA_DS
04/22/04    ak     Lint changes.
04/09/04    ak     Make event callback take a const.
02/04/04    ak     Created file.
===========================================================================*/



/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707

#include "ds707_cback.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"

#ifdef FEATURE_NPA
  #include "npa.h"
static npa_client_handle impulse_cpu_hdl = NULL;
static DALSYSEventHandle impulse_node_create_hdl;
#endif /* FEATURE_NPA */


/*===========================================================================
                            TYPEDEFS
===========================================================================*/

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
#if 0
/*===========================================================================
FUNCTION       DS707_HI_SPD_CLK_EV_CBACK

DESCRIPTION    Callback when the phys link goes up/down.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
/*lint -save -e715
 *           - unused params 
 */
void ds707_hi_spd_clki_ev_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG0(MSG_LEGACY_MED, "Hi speed clock switching not supported");
} /* ds707_hi_spd_clk_ev_cback() */
/*lint -restore */
#endif/* #if 0 */

#ifdef FEATURE_NPA
/*===========================================================================
FUNCTION       DS707_IMPULSE_CLIENT_CREATE_CBACK

DESCRIPTION    Callback that is called when the CPU resource is available 
               for creating client. Creates the impulse CPU client.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
static void * ds707_impulse_client_create_cback
(
  void * context,
  uint32 event_type, 
  void * data,       
  uint32 data_size 
)
{
  DALResult result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( impulse_cpu_hdl == NULL )
  {
    impulse_cpu_hdl = npa_create_sync_client("/core/cpu",     /* resource name */  
                                             "data service impulse", /* client name   */  
                                             NPA_CLIENT_IMPULSE /* work model    */
                                            );
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Create NPA CPU resource handle - impulse");
    /* Check to see handle was created */
    if ( impulse_cpu_hdl == NULL ) 
    {
      DATA_IS707_MSG0( MSG_LEGACY_ERROR, "Unable to create impulse client!" );
    }
    
    /*-----------------------------------------------------------------------
      Delete the DAL event object
    -----------------------------------------------------------------------*/
    result = DALSYS_DestroyObject( impulse_node_create_hdl );
    if( result != DAL_SUCCESS )
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "DALSYS_DestroyObject error %d", 
                       result );
    }
  }

  return NULL;
} /* ds707_impulse_client_create_cback() */
#endif /* FEATURE_NPA */

/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
FUNCTION       DS707_HI_SPD_CLK_INIT

DESCRIPTION    Initialize module which controls start/stop of hi-speed clock.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_hi_spd_clk_init(void)
{
#ifdef FEATURE_NPA
  DALResult result;
#endif
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#if 0
  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_DOWN_EV,
                            ds707_hi_spd_clki_ev_cback
                          ) == FALSE)
  {
    ERR_FATAL("Cant register for phys link down cback",0,0,0);
  }

  if (ds707_cback_register(
                            DS707_CBACK_PHYS_LINK_UP_EV,
                            ds707_hi_spd_clki_ev_cback
                          ) == FALSE)
  {
    ERR_FATAL("Cant register for phys link up cback",0,0,0);
  }
#endif/* #if 0*/
 
#ifdef FEATURE_NPA
  /*-------------------------------------------------------------------------
    Set up callback to be notified when resource is available for creating
    impulse client
  -------------------------------------------------------------------------*/
  if( impulse_cpu_hdl == NULL )
  {
    result = DALSYS_EventCreate( DALSYS_EVENT_ATTR_CALLBACK_EVENT, 
                                 &impulse_node_create_hdl,
                                 NULL );
    if( result != DAL_SUCCESS )
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "DALSYS_EventCreate error %d", 
                       result );
      return;
    }
    
    result = DALSYS_SetupCallbackEvent( impulse_node_create_hdl,
                                        ds707_impulse_client_create_cback,
                                        NULL );
    if( result != DAL_SUCCESS )
    {
      DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                       "DALSYS_SetupCallbackEvent error %d", 
                       result );
      /*---------------------------------------------------------------------
        Delete the DAL event object
      ---------------------------------------------------------------------*/
      result = DALSYS_DestroyObject( impulse_node_create_hdl );
      if( result != DAL_SUCCESS )
      {
        DATA_IS707_MSG1( MSG_LEGACY_ERROR, 
                         "DALSYS_DestroyObject error %d", 
                         result );
      }
      return;
    }
    
    npa_resource_available("/core/cpu", impulse_node_create_hdl);
  }
#endif /* FEATURE_NPA */
} /* ds707_hi_spd_clk_init() */

#ifdef FEATURE_NPA
/*===========================================================================
FUNCTION     DS707_GET_IMPULSE_CPU_HDL

DESCRIPTION  Accessor function to impulse_cpu_hdl
  
PARAMETERS   None
  
RETURN VALUE Returns impulse_cpu_hdl

DEPENDENCIES None

SIDE EFFECTS None
===========================================================================*/
npa_client_handle ds707_get_impulse_cpu_hdl( void )
{
  return impulse_cpu_hdl;
} /* ds707_get_impulse_cpu_hdl */
#endif /* FEATURE_NPA */


#endif /* FEATURE_DATA_IS707 */
