/*==============================================================================

                             COPYRIGHT INFORMATION

Copyright (c) 2009,2010 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

                            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/src/reg_ecall.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---       -------------------------------------------------------------
09/12/11   HC        Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                     as a part of CMI activity 
02/11/10   sarugola     Created file
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE

==============================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_NAS_ECALL

#include "reg_ecall_v.h"
#include "reg_mm_v.h"

/*==============================================================================

                                TYPE DEFINITIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DECLARATIONS

==============================================================================*/

/*==============================================================================

                               DATA DECLARATIONS

==============================================================================*/

/*==============================================================================

                          STATIC FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

                              FUNCTION DEFINITIONS

==============================================================================*/

/*==============================================================================

FUNCTION NAME

  ecall_reg_mode_ind_is_valid

==============================================================================*/

boolean ecall_reg_mode_ind_is_valid
(
 const ecall_reg_mode_ind_s_type* msg_p
)
{
 boolean valid = FALSE;
  
 if(msg_p)
 {
   if((msg_p->mode_ind > REG_ECALL_MODE_NONE)  && (msg_p->mode_ind < REG_ECALL_MODE_MAX) )
   {
     valid = TRUE;
   }
 }
 
 return valid;
 
}
/*==============================================================================

FUNCTION NAME

  ecall_reg_convert_ecall_mode_type_to_mm

==============================================================================*/
mmr_ecall_mode_e_type ecall_reg_convert_ecall_mode_type_to_mm
(
 reg_ecall_mode_e_type mode_type
)
{
 mmr_ecall_mode_e_type mm_mode_type;
 
 switch(mode_type)
 {
   case REG_ECALL_MODE_NORMAL:
    mm_mode_type = MMR_ECALL_MODE_NORMAL;
    break;

   case REG_ECALL_MODE_ECALL_ONLY:
    mm_mode_type = MMR_ECALL_MODE_ECALL_ONLY;
    break;

   default:
    mm_mode_type = MMR_ECALL_MODE_NORMAL;
    break;
   
 }
 return mm_mode_type;
}
#endif /*FEATURE_NAS_ECALL*/
