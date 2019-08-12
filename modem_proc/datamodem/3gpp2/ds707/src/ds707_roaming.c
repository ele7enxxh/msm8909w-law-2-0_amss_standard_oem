/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   R O A M I N G 

                                 

GENERAL DESCRIPTION
  This file contains functions used to handle the roaming between the 
  Japan's network for KDDI and North America network.

EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    Must call ds707_epzid_mgr_init() at startup.

   Copyright (c) 2007-2012 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_roaming.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------   
06/30/14   niv     Added wrapper functions & API change for 
                   NV_REFRESH 
08/13/12   ss      Moving efs write operation to dsutil performed during 
                   bootup.
04/18/12   jz      Seperate old JCDMA and Simplified JCDMA
01/18/12   jz      JCDMA feature cleanup
01/28/11   jee     JCDMA feature cleanup
03/04/09   sa      AU level CMI modifications.
07/25/08   hs      Fixed compilation warnings
02/18/08   sa      Added initialization function for ATCoP. 
12/06/07   sy      Initial revision.


===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "ds707_roaming.h"
#include "dsat_v.h"
#include "ds707_so_async.h"
#include "ds707_pkt_mgr.h"
#include "ds3gcfgmgr.h"
#include "ps_iface_defs.h"
#include "ds707_pkt_mgri.h"
#include "ps_iface.h"
#include "ds707_so_g3fax.h"
#include "dsat707faxctab.h"
#include "dsat707extctab.h"
#include "ds707_so_pkt.h"
#include "ds707_so_jcdma.h"
#include "ds707_jcdma_m51.h"
#include "ds707_jcdma_apprate.h"
#include "ds707_gonulltmri.h"
#include "ds707_data_session_profile.h"
#include "dsutil.h"
#include "data_msg.h"
extern void ds707_so_pkti_register(void);
/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

#define DS707_ROAMING_DATA_EFS_CONF_FILE_PATH \
         "/nv/item_files/conf/data_config_info.conf"

/*---------------------------------------------------------------------------
  NV item path & the variable to store JCDMA mode info
---------------------------------------------------------------------------*/
#define DS707_JCDMA_MODE_FILE_NAME  "/nv/item_files/jcdma/JCDMA_feature_all"
#define DS707_SIMPLIFIED_JCDMA_MODE_NV_PATH  "/nv/item_files/jcdma/jcdma_mode"

/*---------------------------------------------------------------------------
  Set to TRUE when nv "/nv/item_files/jcdma/jcdma_full_mode" is set to 1, 
  indicating all the features for JCDMA, old+simplified, are enabled. 
---------------------------------------------------------------------------*/
static boolean jcdma_mode_active = FALSE;

/*---------------------------------------------------------------------------
  Set to TRUE if nv "/nv/item_files/jcdma/jcdma_mode" is set to 1, indicating
  only simplified JCDMA features are enabled.
---------------------------------------------------------------------------*/
static boolean simplified_jcdma_mode = FALSE;

/*===========================================================================
                       FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================

FUNCTION  DS707_ROAMING_INIT 

DESCRIPTION  This initialization function will take care of the data roaming
             between Japan and North America.

DEPENDENCIES 
  
RETURN VALUE
  
SIDE EFFECTS
===========================================================================*/
void ds707_roaming_init( void )
{
  ps_iface_type             *iface_ptr;      /* ptr to IPV4 1x iface     */
  uint8                     curr_iface_index;

  for ( curr_iface_index = 0; 
        curr_iface_index < DS707_MAX_IFACES; 
        curr_iface_index++ )
  {
    iface_ptr = NULL ;
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(curr_iface_index);
    if (ds707_roaming_is_curr_mode_jcdma() == TRUE)
    {
      PS_IFACE_SET_GRACEFUL_CLOSE(iface_ptr, FALSE);
    }
    else
    {
      PS_IFACE_SET_GRACEFUL_CLOSE(iface_ptr, TRUE);
    }
  }  
 
#ifdef FEATURE_DATA_IS707_ASYNC  
  ds707_so_async_init();
  ds707_so_g3fax_init();
#endif /* FEATURE_DATA_IS707_ASYNC */

  ds707_so_pkti_register();
  ds707_so_pkt_recal();
  
#ifdef FEATURE_DATA_IS707_ASYNC  
  dsat707faxctab_update_fcc_dflm();
#endif /* FEATURE_DATA_IS707_ASYNC */

  dsat707extctab_update_crm_dflm();

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    /*-------------------------------------------------------------------------
      Initialize M51 and apprate JCDMA modules.  Must be done after SO's have
      been initialized.
    -------------------------------------------------------------------------*/
    ds707_so_jcdma_init();
    ds707_jcdma_m51_init();
    ds707_jcdma_apprate_init();
    dsat_init_for_jcdma();
    /*-------------------------------------------------------------------------
      Initialize gonull timer
    -------------------------------------------------------------------------*/
    ds707_gonulltmri_init();
  }

  ds707_data_session_profile_init();
  ds707_data_session_profile_cache_init();
}

/*===========================================================================
FUNCTION  DS707_ROAMING_READ_JCDMA_NV

DESCRIPTION   Called to read the full JCDMA mode NV value

DEPENDENCIES   

RETURN VALUE
  
SIDE EFFECTS
===========================================================================*/
void ds707_roaming_read_jcdma_nv (void)
{
  char item_file_path[] = DS707_JCDMA_MODE_FILE_NAME;
  boolean                 current_jcdma_mode=FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  current_jcdma_mode = jcdma_mode_active ;
  jcdma_mode_active = FALSE;

  (void)ds3gcfgmgr_read_efs_nv_ex (
            item_file_path, &jcdma_mode_active, sizeof(jcdma_mode_active), ds707_curr_subs_id());

  if( (current_jcdma_mode == TRUE) && (jcdma_mode_active == FALSE) )
  {
    ds707_roaming_handoff_jcdma_to_nonjcdma();
  }
  else if ((current_jcdma_mode == FALSE) && (jcdma_mode_active == TRUE) )
  {
    ds707_roaming_handoff_nonjcdma_to_jcdma();
  }
  return;
}
/*===========================================================================
FUNCTION  DS707_ROAMING_READ_SIMPLIFIED_JCDMA_NV

DESCRIPTION   Called to read the simplified JCDMA mode NV value

DEPENDENCIES   

RETURN VALUE
  
SIDE EFFECTS
===========================================================================*/
void ds707_roaming_read_simplified_jcdma_nv (void)
{
  nv_stat_enum_type  status;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  status = ds3gcfgmgr_read_efs_nv_ex (
            DS707_SIMPLIFIED_JCDMA_MODE_NV_PATH, 
            &simplified_jcdma_mode, 
            sizeof(simplified_jcdma_mode), ds707_curr_subs_id());

  if (0 != status)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR,
        "EFS item not configured for simplified_jcdma_nv ");
    simplified_jcdma_mode = FALSE;
  }

  return;
}

/*===========================================================================
FUNCTION  DS707_ROAMING_IS_CURR_MODE_JCDMA

DESCRIPTION   Called to check if it is in JCDMA mode. 
              Return true for JCDMA mode is set otherwise 
              return false.

DEPENDENCIES   This parameter will be set with System Determination
               API.

RETURN VALUE
  
SIDE EFFECTS
===========================================================================*/
boolean ds707_roaming_is_curr_mode_jcdma(void)
{
  return jcdma_mode_active;
}
/*===========================================================================
FUNCTION DS707_ROAMING_IS_SIMPLIFIED_JCDMA_MODE

DESCRIPTION
  If current is in simplified JCDMA mode

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  TRUE - simplified JCDMA mode
  FALSE - otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean ds707_roaming_is_simplified_jcdma_mode(void)
{
  return simplified_jcdma_mode;
}

/*===========================================================================
FUNCTION  DS707_ROAMING_HANDOFF_JCDMA_TO_NONJCDMA

DESCRIPTION   Moves the mobile to non-JCDMA mode

DEPENDENCIES  

RETURN VALUE
  
SIDE EFFECTS
===========================================================================*/
void ds707_roaming_handoff_jcdma_to_nonjcdma(void)
{
  ps_iface_type             *iface_ptr;      /* ptr to IPV4 1x iface     */
  uint8 i=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  jcdma_mode_active = FALSE;

  ds707_jcdma_m51_set_val(JCDMA_VAL_511, SYS_JCDMA_M511_PKT); 
  ds707_jcdma_m51_set_val(JCDMA_VAL_512, SYS_JCDMA_M512_STD);
  ds707_jcdma_m51_set_val(JCDMA_VAL_512, SYS_JCDMA_M513_AUTO);


  for (i = 0; i < DS707_MAX_IFACES; i++)
  {
    iface_ptr = NULL ;
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(i);
    /*---------------------------------------------------------------------
    Set PPP termination policy.
    ---------------------------------------------------------------------*/
    PS_IFACE_SET_GRACEFUL_CLOSE(iface_ptr, TRUE);
  }

  ds707_cback_deregister(DS707_CBACK_PHYS_LINK_DOWN_EV,
                             ds707_so_jcdma_pkti_cback);

  ds707_cback_deregister(DS707_CBACK_PHYS_LINK_UP_EV,
                        ds707_so_jcdma_pkti_cback);

  ds707_cback_deregister(DS707_CBACK_ABORT_SESSION_EV,
                        ds707_so_jcdma_pkti_cback);
  return;
}

/*===========================================================================
FUNCTION  DS707_ROAMING_HANDOFF_NONJCDMA_TO_JCDMA

DESCRIPTION   Moves the mobile to JCDMA mode

DEPENDENCIES   

RETURN VALUE
  
SIDE EFFECTS
===========================================================================*/

void ds707_roaming_handoff_nonjcdma_to_jcdma(void)
{
    ps_iface_type             *iface_ptr;      /* ptr to IPV4 1x iface     */
    uint8 i=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  jcdma_mode_active = TRUE;

  ds707_cback_deregister( DS707_CBACK_HOLDDOWN_TIMER_STOPPED_EV,
                        epzidi_event_cback );

  for (i = 0; i < DS707_MAX_IFACES; i++)
  {
    iface_ptr = NULL ;
    iface_ptr = ds707_pdncntx_get_iface_ptr_from_iface_tbl_index(i);
    /*---------------------------------------------------------------------
      Set PPP termination policy.
    ---------------------------------------------------------------------*/
    PS_IFACE_SET_GRACEFUL_CLOSE(iface_ptr, FALSE);
  }
  return;
}