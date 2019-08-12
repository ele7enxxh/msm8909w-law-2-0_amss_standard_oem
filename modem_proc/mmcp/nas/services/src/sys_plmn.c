/*==============================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/sys_PLMN.c_v   1.0   04 Feb 2002 12:20:50   kabraham  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/services/src/sys_plmn.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
02/04/02   kwa     Initial version.
07/22/05   up      Change sys_PLMN.h to small case.
==============================================================================*/

/*==============================================================================

                     INCLUDE FILES FOR MODULE

==============================================================================*/
#include "sys_plmn.h"

/*==============================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

==============================================================================*/

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_PLMN_id_digit_is_valid

DESCRIPTION

  Function that checks if the input digit is between 0 and 9 or equal to 0xF.

PARAMETERS

  byte       digit                     Digit to be checked for validity.

RETURN VALUE

  boolean  Flag indicating if the digit is valid (TRUE) or not (FALSE).

==============================================================================*/

static boolean sys_PLMN_id_digit_is_valid
(
  byte digit
)
{
  return ( (digit <= 0x9) || (digit == 0xF) );
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_PLMN_id_is_undefined

==============================================================================*/

boolean sys_PLMN_id_is_undefined
(
  PLMN_id_T PLMN_id
)
{
  return ( (PLMN_id.identity[0] == 0xFF) && 
           (PLMN_id.identity[1] == 0xFF) && 
           (PLMN_id.identity[2] == 0xFF));
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_PLMN_undefine_PLMN_id

==============================================================================*/

void sys_PLMN_undefine_PLMN_id
(
  PLMN_id_T* PLMN_id_p
)
{
  PLMN_id_p->identity[0] = 0xFF;
  PLMN_id_p->identity[1] = 0xFF;
  PLMN_id_p->identity[2] = 0xFF;
}


/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_PLMN_get_MCC_MNC

==============================================================================*/

void sys_PLMN_get_MCC_MNC
(
  PLMN_id_T PLMN_id,
  boolean*  PLMN_id_is_undefined_p,
  boolean*  MNC_includes_PCS_digit_p,
  uint32*   MCC_p,
  uint32*   MNC_p
)
{
  uint32 MCC_digit_1;
  uint32 MCC_digit_2;
  uint32 MCC_digit_3;

  uint32 MNC_digit_1;
  uint32 MNC_digit_2;
  uint32 MNC_digit_3;

  if (sys_PLMN_id_is_undefined (PLMN_id))
  {
    *PLMN_id_is_undefined_p = TRUE;
    *MCC_p = 0;
    *MNC_p = 0;
  }
  else
  {
    *PLMN_id_is_undefined_p = FALSE;

    MCC_digit_1 = PLMN_id.identity[0] & 0x0F;
    MCC_digit_2 = PLMN_id.identity[0] / 0x10;
    MCC_digit_3 = PLMN_id.identity[1] & 0x0F;

    MNC_digit_1 = PLMN_id.identity[2] & 0x0F;
    MNC_digit_2 = PLMN_id.identity[2] / 0x10;
    MNC_digit_3 = PLMN_id.identity[1] / 0x10;

    *MCC_p = 100 * MCC_digit_1 + 10 * MCC_digit_2 + MCC_digit_3;

    if (MNC_digit_3 == 0xF)
    {
      *MNC_includes_PCS_digit_p = FALSE;
      *MNC_p = 10 * MNC_digit_1 + MNC_digit_2;
    }
    else
    {
      *MNC_includes_PCS_digit_p = TRUE;
      *MNC_p = 100 * MNC_digit_1 + 10 * MNC_digit_2 + MNC_digit_3;
    }
  }
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_PLMN_set_MCC_MNC

==============================================================================*/

boolean sys_PLMN_set_MCC_MNC
(
  PLMN_id_T* PLMN_id_p,
  boolean    MNC_includes_PCS_digit,
  uint32     MCC,
  uint32     MNC
)
{
  uint32 MCC_digit_1;
  uint32 MCC_digit_2;
  uint32 MCC_digit_3;

  uint32 MNC_digit_1;
  uint32 MNC_digit_2;
  uint32 MNC_digit_3;

  if ( (MCC > 999) ||
       (MNC_includes_PCS_digit && MNC > 999) ||
       (!MNC_includes_PCS_digit && MNC > 99) )
  {
    sys_PLMN_undefine_PLMN_id (PLMN_id_p);
    return FALSE;
  }

  PLMN_id_p->identity[0] = 0x00;
  PLMN_id_p->identity[1] = 0x00;
  PLMN_id_p->identity[2] = 0x00;

  MCC_digit_1 = MCC/100;
  MCC_digit_2 = (MCC - 100 * MCC_digit_1) / 10;
  MCC_digit_3 = (MCC - (100 * MCC_digit_1)) - (10 * MCC_digit_2);

  if (MNC_includes_PCS_digit)
  {
    MNC_digit_1 = MNC/100;
    MNC_digit_2 = (MNC - 100 * MNC_digit_1) / 10;
    MNC_digit_3 = (MNC - (100 * MNC_digit_1)) - (10 * MNC_digit_2);
  }
  else
  {
    MNC_digit_1 = MNC/10;
    MNC_digit_2 = MNC - 10 * MNC_digit_1;
    MNC_digit_3 = 0xF;
  }

  PLMN_id_p->identity[0] = (byte)((MCC_digit_2 << 4) + MCC_digit_1);
  PLMN_id_p->identity[1] = (byte)((MNC_digit_3 << 4) + MCC_digit_3);
  PLMN_id_p->identity[2] = (byte)((MNC_digit_2 << 4) + MNC_digit_1);

  return TRUE;
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_PLMN_id_is_valid

==============================================================================*/

boolean sys_PLMN_id_is_valid
(
  PLMN_id_T PLMN_id
)
{
  return ( sys_PLMN_id_digit_is_valid( (byte)(PLMN_id.identity[0] & 0x0F) ) &&
           sys_PLMN_id_digit_is_valid( (byte)(PLMN_id.identity[0] / 0x10) ) &&
           sys_PLMN_id_digit_is_valid( (byte)(PLMN_id.identity[1] & 0x0F) ) &&
           sys_PLMN_id_digit_is_valid( (byte)(PLMN_id.identity[2] & 0x0F) ) &&
           sys_PLMN_id_digit_is_valid( (byte)(PLMN_id.identity[2] / 0x10) ) &&
           sys_PLMN_id_digit_is_valid( (byte)(PLMN_id.identity[1] / 0x10) ) );
}

/*==============================================================================

              W A R N I N G - R E E N T R A N T   F U N C T I O N

FUNCTION NAME

  sys_PLMN_match

==============================================================================*/

boolean sys_PLMN_match
(
  PLMN_id_T PLMN_id_1,
  PLMN_id_T PLMN_id_2
)
{
  return ( ( (PLMN_id_1.identity[0] & 0x0F) == (PLMN_id_2.identity[0] & 0x0F) ) &&
           ( (PLMN_id_1.identity[0] / 0x10) == (PLMN_id_2.identity[0] / 0x10) ) &&
           ( (PLMN_id_1.identity[1] & 0x0F) == (PLMN_id_2.identity[1] & 0x0F) ) &&
           ( ( (PLMN_id_2.identity[1] / 0x10) == 0x0F)
           ? (
               ( (PLMN_id_1.identity[2] & 0x0F) == (PLMN_id_2.identity[2] & 0x0F) ) &&
               ( (PLMN_id_1.identity[2] / 0x10) == (PLMN_id_2.identity[2] / 0x10) )
             )
           : (
               ( (PLMN_id_1.identity[2] & 0x0F) == (PLMN_id_2.identity[2] & 0x0F) ) &&
               ( (PLMN_id_1.identity[2] / 0x10) == (PLMN_id_2.identity[2] / 0x10) ) &&
               ( (PLMN_id_1.identity[1] / 0x10) == (PLMN_id_2.identity[1] / 0x10) )
             )
           )
         );
}
