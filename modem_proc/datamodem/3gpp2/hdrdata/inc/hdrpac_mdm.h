#ifndef HDRPAC_MDM_H
#define HDRPAC_MDM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        H I G H     D A T A     R A T E

          PACKET APPLICATION CONTROLLER DATA SERVICES EXTERNAL FILE

GENERAL DESCRIPTION
  This file contains functionality that needs interaction between HDR Call
  Processing and Data Services in a dual processor environment.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2006-2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrpac_mdm.h#1 $

when       who    what, where, why
--------   ---    -----------------------------------------------------------
09/04/09   ss     CMI SU level modifications.
04/09/09   ls     Added hdrpac_get_rtcmac_flow()
07/24/06   mpa    Moved hdrpac_get_rtcmac_flow() to hdrpac.c
06/20/06   mpa    Added hdrpac_mdm_powerup_init().
06/14/06   vrk    Created file by moving fns from hdrpacdsif and hdrpac 
                  to support dual processor architectures.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_HDR
#include "hdrpac.h"
#include "hdrpac_api.h"

/*===========================================================================

                        FUNCTION DEFINITIONS            

===========================================================================*/
/*===========================================================================
FUNCTION HDRPAC_CLOSE_CONNECTION

DESCRIPTION
  This function closes a connection through the hdr protocol (ALMP). 

DEPENDENCIES
  None.

PARAMETERS
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpac_close_connection( void );

#endif /*FEATURE_HDR*/
#endif /*HDRPAC_MDM_H*/
