#ifndef REG_ECALL_V_H
#define REG_ECALL_V_H
/*===========================================================================
          R E G  -  E C A L L   H E A D E R   F I L E

DESCRIPTION
  This header file contains message definitions for the REG and ECALL MODEM
  Task interface.

Copyright (c) 2009,2010 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/reg/inc/reg_ecall_v.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
09/12/11   HC         Replacing the #include "reg_mm.h" with #include "reg_mm_v.h" 
                      as a part of CMI activity 
02/11/10    ajt       Created file
===========================================================================*/
/* <EJECT> */

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/


#include "mmcp_variation.h"
#ifdef FEATURE_NAS_ECALL

#include "sys_stru_v.h"
#include "reg_mm_v.h"

#include "reg_ecall.h"

/*===========================================================================
                     M A C R O  D E F I N I T I O N S
===========================================================================*/


/*===========================================================================
                   T Y P E  D E C L A R A T I O N S
===========================================================================*/
/*******************************************************************************
********************************************************************************
**                                                                            **
**                  ECALL MODEM to REG Primitives                             **
**                                                                            **
********************************************************************************
*******************************************************************************/

/*******************************************************************************
**                                                                            **
**  ECALL_REG_MODE_IND                                                        **
**                                                                            **
*******************************************************************************/



/*===========================================================================
                F U N C T I O N    D E C L A R A T I O N S
===========================================================================*/
/*==============================================================================

 FUNCTION NAME

   ecall_reg_mode_ind_is_valid

 DESCRIPTION
 
   Function to check whether ECALL_REG_MODE_IND is valid or not


 RETURN VALUE

   bolean


 ==============================================================================*/

boolean ecall_reg_mode_ind_is_valid
(
 const ecall_reg_mode_ind_s_type* msg_p
);
/*==============================================================================

 FUNCTION NAME

   ecall_reg_convert_ecall_mode_type_to_mm

 DESCRIPTION

   Function to convert reg_ecall_mode_e_type to mmr_ecall_mode_e_type

 RETURN VALUE

   mmr_ecall_mode_e_type


 ==============================================================================*/

extern mmr_ecall_mode_e_type ecall_reg_convert_ecall_mode_type_to_mm
(
 reg_ecall_mode_e_type mode_type
);

#endif/*FEATURE_NAS_ECALL*/

#endif /* #ifndef REG_ECALL_H */
