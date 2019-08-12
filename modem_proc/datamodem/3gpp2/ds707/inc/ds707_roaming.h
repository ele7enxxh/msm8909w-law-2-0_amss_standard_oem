#ifndef DS707_ROAMING_H
#define DS707_ROAMING_H

/*===========================================================================

                        D S 7 0 7 _ R O A M I N G 
GENERAL DESCRIPTION
  This file contains the roaming set up needed for the mobile when 
  the phone roams between the North America and Japan's KDDI network.
  
  
EXTERNALIZED FUNCTIONS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
  
 Copyright (c) 2007-2008 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_roaming.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
-----------------------------------------------------------------------------  
when        who    what, where, why
--------    ---    -------------------------------------------------------
06/30/14    niv    Added wrapper functions & API change for 
                   NV_REFRESH 
12/06/07     sy    Initial revision.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "ds707_cback.h"

/*===========================================================================

FUNCTION  DS707_ROAMING_INIT 

DESCRIPTION  This initialization function will take care of the data roaming
             between Japan and North America.

DEPENDENCIES
  

RETURN VALUE
  
SIDE EFFECTS
===========================================================================*/
void ds707_roaming_init( void );


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
boolean ds707_roaming_is_curr_mode_jcdma(void);
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
boolean ds707_roaming_is_simplified_jcdma_mode(void);

/*===========================================================================
FUNCTION     DS707_ROAMING_HANDOFF_JCDMA_TO_NONJCDMA

DESCRIPTION  Actions to take when mode changes from 
             JCDMA to NON JCDMA 
 
DEPENDENCIES None. 

PARAMETERS   None. 

RETURN VALUE None.

SIDE EFFECTS None. 

===========================================================================*/
void ds707_roaming_handoff_jcdma_to_nonjcdma(void);

/*===========================================================================
FUNCTION      DS707_ROAMING_HANDOFF_NONJCDMA_TO_JCDMA

DESCRIPTION   Actions to take when mode changes from 
              NONJCDMA to JCDMA 
 
DEPENDENCIES  None.
  

PARAMETERS    None. 

RETURN VALUE  None.

SIDE EFFECTS  None.   

===========================================================================*/
void ds707_roaming_handoff_nonjcdma_to_jcdma(void);

/*===========================================================================
FUNCTION  DS707_ROAMING_READ_JCDMA_NV

DESCRIPTION   Called to read the JCDMA mode NV value

DEPENDENCIES  None. 

RETURN VALUE  None.
  
SIDE EFFECTS  None.
===========================================================================*/
void ds707_roaming_read_jcdma_nv (void);
/*===========================================================================
FUNCTION  DS707_ROAMING_READ_SIMPLIFIED_JCDMA_NV

DESCRIPTION   Called to read the simplified JCDMA mode NV value

DEPENDENCIES  None.

RETURN VALUE  None.
  
SIDE EFFECTS  None.
===========================================================================*/
void ds707_roaming_read_simplified_jcdma_nv (void);

#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION      EPZIDI_EVENT_CBACK

DESCRIPTION   Callback when a phys link event occurs.

DEPENDENCIES  None.  

RETURN VALUE  None.
  
SIDE EFFECTS  None.
===========================================================================*/
void epzidi_event_cback
(
  ds707_cback_event_type               event,
  const ds707_cback_event_info_type   *event_info_ptr
);

#endif /* FEATURE_DATA_IS707 */
#endif
