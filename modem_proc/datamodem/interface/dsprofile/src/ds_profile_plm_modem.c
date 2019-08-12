/******************************************************************************
  @file    ds_profile_plm_modem.c
  @brief   

  DESCRIPTION
  This file implements the modem (AMSS) specific routines

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/src/ds_profile_plm_modem.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/

#include "datamodem_variation.h"
#include "ds_profile_plm.h"

/*---------------------------------------------------------------------------
                     PUBLIC FUNCTION IMPLEMENTATIONS 
---------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_DS_PROFILE_3GPP2
#include "ds_profile_3gpp2i.h"
extern uint8 ds_profile_3gpp2_modem_init ( tech_fntbl_type *fntbl );
#endif

#include "ds_profile_3gppi.h"
extern uint8 ds_profile_3gpp_modem_init ( tech_fntbl_type *fntbl );


/*===========================================================================
FUNCTION PLM_TECH_OPS_INIT

DESCRIPTION
  This function calls the tech init function for operations module

PARAMETERS
  store :  pointer to table for all techs, having table of function
           pointers to be initialized in the tech specific init functions
DEPENDENCIES 
  
RETURN VALUE 
  mask : ORed value of tech masks 
SIDE EFFECTS 
 
===========================================================================*/
uint8 plm_tech_ops_init( 
  plm_store_type *store 
)
{
  uint8 mask = 0;
  mask |= ds_profile_3gpp_init( &(store->vtbl) ); 

#ifdef FEATURE_DATA_DS_PROFILE_3GPP2
  mask |= ds_profile_3gpp2_init ( &(store->vtbl) );
#endif

  return mask;
}

/*===========================================================================
FUNCTION PLM_TECH_ACCESS_INIT

DESCRIPTION
  This function calls the tech init function for access module

PARAMETERS
  store :  pointer to table for all techs, having table of function
           pointers to be initialized in the tech specific init functions
DEPENDENCIES 
  
RETURN VALUE 
  mask : ORed value of tech masks 
SIDE EFFECTS 
 
===========================================================================*/
uint8 plm_tech_access_init( 
  plm_store_type *store 
)
{
  uint8 mask = 0;
  mask |= ds_profile_3gpp_modem_init( &(store->vtbl) );

#ifdef FEATURE_DATA_DS_PROFILE_3GPP2
  mask |= ds_profile_3gpp2_modem_init ( &(store->vtbl) );
#endif

  return mask;
}


