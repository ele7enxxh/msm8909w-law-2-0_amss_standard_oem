#ifndef DS707_SO_G3FAX_H
#define DS707_SO_G3FAX_H
/*===========================================================================

                          D S 7 0 7 _ G 3 F A X _ S O
GENERAL DESCRIPTION

 Copyright (c) 2002-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_so_g3fax.h_v   1.2   14 Oct 2002 09:48:26   ajithp  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_so_g3fax.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who    what, where, why
--------   ---    ----------------------------------------------------------
02/11/11   ack    Global Variable Cleanup
10/14/02   atp    Cleanup, added ds707_so_g3fax_so_is_g3fax().
10/10/02   ak     Updated for JCDMA M51/1X by making so_list public and
                  moving #define of number fax SO's into .h.
02/20/02   ak     First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "ds707.h"

/*===========================================================================
                          TYPEDEFS and DECLARATIONS
===========================================================================*/
#define DS707_SO_G3FAX_NUM   4


/*===========================================================================
                         EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SO_G3FAX_RECAL

DESCRIPTION   Called when the G3 FAX SO's need to be recalibrated for
              enabling/disabling with SNM, when some mobile setting has
              changed.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_g3fax_recal(void);

/*===========================================================================
FUNCTION      DS707_SO_G3FAX_INIT

DESCRIPTION   Wrapper function for register_g3fax_so() and set_g3fax_so
              state() functions, in order to simplify the external interface.
              Any power-up initialization that needs to be performed for
              Group 3 fax service options should be done here.

DEPENDENCIES  This function should be called only after ATCOP has been
              initialized, since it invokes functions that use some AT
              command settings.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_so_g3fax_init(void);

/*===========================================================================
FUNCTION       DS707_SO_G3FAX_GET_ORIG

DESCRIPTION    Returns the SO to orig with.

DEPENDENCIES   NONE

RETURN VALUE   Service option to originate with.

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_so_g3fax_get_orig
(
  ds707_rateset_type  rateset
);

/*===========================================================================
FUNCTION       DS707_SO_G3FAX_SO_IS_G3FAX

DESCRIPTION    Returns TRUE if passed in SO is a g3fax SO.  Else returns
               FALSE.

DEPENDENCIES   NONE

RETURN VALUE   See DESCRIPTION

SIDE EFFECTS   None
===========================================================================*/
boolean ds707_so_g3fax_so_is_g3fax
(
  ds707_so_type    so
);

/*===========================================================================
FUNCTION       DS707_GET_SO_G3FAX_TYPE

DESCRIPTION    Accessor function for G3 Fax Service Option

DEPENDENCIES   NONE

RETURN VALUE   Returns the requested G3 Fax SO

SIDE EFFECTS   None
===========================================================================*/
ds707_so_type ds707_get_so_g3fax_type
(
  byte index
);

#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* DS707_SO_G3FAX_H   */

