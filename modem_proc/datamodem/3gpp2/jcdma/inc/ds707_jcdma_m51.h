#ifndef DS707_JCDMA_M51_H
#define DS707_JCDMA_M51_H
/*===========================================================================

                          D S 7 0 7 _ J C D M A _ M 5 1
GENERAL DESCRIPTION

 This file contains the API's and functionality to implement the JCDMA
 M51 data functionality.

 Copyright (c) 2002 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_jcdma_m51.h_v   1.7   12 Dec 2002 17:07:08   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/jcdma/inc/ds707_jcdma_m51.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/11   ms      Global variable cleanup.
10/28/03   ak      Externs for functions to see if async/fax or pkt allowed.
12/12/02   ak      Change m512 default to STD.
12/02/02   ak      Changed make_pkt_call func name to include m51.
10/22/02   rsl     Updated FEATURE_JCDMA_DS_1X to FEATURE_JCDMA_1X
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/02/02   ak      First version of file.
===========================================================================*/



/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "sio.h"
#include "sys.h"


/*===========================================================================
                         TYPDEFS & DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  Variables used to track state of the M51* values.  UI should not use
  these.
---------------------------------------------------------------------------*/

#define DS707_JCDMA_M511_DEF  (SYS_JCDMA_M511_PKT)
#define DS707_JCDMA_M512_DEF  (SYS_JCDMA_M512_STD)
#define DS707_JCDMA_M513_DEF  (SYS_JCDMA_M513_AUTO)

/*===========================================================================
                          ENUMS USED
===========================================================================*/
typedef enum
{
   JCDMA_VAL_511 =0,
   JCDMA_VAL_512 =1,
   JCDMA_VAL_513 =2
}jcdma_mode_val;


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
void ds707_jcdma_m51_init(void);

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
int verify_jcdma_mode_val (uint8, jcdma_mode_val, uint8 *);


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
);

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
);

/*===========================================================================
FUNCTION      DS707_JCDMA_CHNG_M513_MODE

DESCRIPTION   Called by UI when the user selects a particular M513 setting.

DEPENDENCIES  None

RETURN VALUE  TRUE if passed in value is okay.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_chng_m513_mode
(
  sys_jcdma_m513_e_type  new_mode
);

/*===========================================================================
FUNCTION      DS707_JCDMA_M51_MAKE_PKT_CALL

DESCRIPTION   Returns TRUE or FALSE, indicating if the next call should be
              pkt or async/fax.  This is decided by the M511 setting.

DEPENDENCIES  None

RETURN VALUE  TRUE  - M511 indicates make a packet call
              FALSE - M511 indicates a fax/async call.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_make_pkt_call(void);

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
);

#ifdef FEATURE_DATA_IS707_ASYNC
/*===========================================================================
FUNCTION      DS707_JCDMA_M51_IS_ASYNC_FAX_ALLOWED

DESCRIPTION   Returns TRUE/FALSE, indicating if the M511 value allows
              async/fax.

DEPENDENCIES  None

RETURN VALUE  TRUE if async/fax allowed.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_is_async_fax_allowed(void);
#endif /* FEATURE_DATA_IS707_ASYNC */

/*===========================================================================
FUNCTION      DS707_JCDMA_M51_IS_PKT_ALLOWED

DESCRIPTION   Returns TRUE/FALSE, indicating if the M511 value allows
              packet.

DEPENDENCIES  None

RETURN VALUE  TRUE if packet allowed.  else returns FALSE.

SIDE EFFECTS  None
===========================================================================*/
boolean ds707_jcdma_m51_is_pkt_allowed(void);

/*===========================================================================
FUNCTION      DS707_JCDMA_M51_GET_VAL

DESCRIPTION   Returns m511 value

DEPENDENCIES  None

RETURN VALUE  m511 value..

SIDE EFFECTS  None
===========================================================================*/
int32 ds707_jcdma_m51_get_val
(
  jcdma_mode_val m51_val_type
);
/*===========================================================================
FUNCTION      DS707_JCDMA_M51_SET_VAL

DESCRIPTION   Sets JCDMA Mode value for m511, or m512 or m513

DEPENDENCIES  None

RETURN VALUE  None..

SIDE EFFECTS  None
===========================================================================*/
void ds707_jcdma_m51_set_val
(
  jcdma_mode_val m511_val_type,
  int32         jcdma_m51_val
);

#endif /* DS707_JCDMA_M51_H    */

