#ifndef DS707_HI_SPD_CLK_H
#define DS707_HI_SPD_CLK_H
/*===========================================================================

               D S 7 0 7 _ H I _ S P D _ C L K
               
GENERAL DESCRIPTION
  This file is used to increase/decrease the CPU clock speed when a 
  pkt data call starts.
  
EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_hi_spd_clk.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/15/11    mg     Global variable cleanup
02/04/04    ak     Created file.
===========================================================================*/



/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_NPA
  #include "npa.h"
#endif


/*===========================================================================
                               TYPEDEFS
===========================================================================*/

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
void ds707_hi_spd_clk_init(void);

#ifdef FEATURE_NPA
/*===========================================================================
FUNCTION     DS707_GET_IMPULSE_CPU_HDL

DESCRIPTION  Accessor function to impulse_cpu_hdl
  
PARAMETERS   None
  
RETURN VALUE Returns impulse_cpu_hdl

DEPENDENCIES None

SIDE EFFECTS None
===========================================================================*/
npa_client_handle ds707_get_impulse_cpu_hdl( void );

#endif /* FEATURE_NPA */
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_HI_SPD_CLK_H */

