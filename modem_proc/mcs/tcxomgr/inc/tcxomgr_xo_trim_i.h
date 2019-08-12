#ifndef TCXOMGR_XO_TRIM_I_H
#define TCXOMGR_XO_TRIM_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                T C X O    M A N A G E R    X O    T R I M
                     TCXO Manager XO Trim Internal Header File

GENERAL DESCRIPTION

 
EXTERNALIZED FUNCTIONS



REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS



  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=


==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/inc/tcxomgr_xo_trim_i.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who     what, where, why
--------    ---     ---------------------------------------------------------
07/15/11    sm      Search for trim using binary search
29/11/10    hh      Initial Version. CMI phase-4 changes.
============================================================================*/


/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "customer.h"
#include "tcxomgr.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

#define TCXOMGR_XO_TRIM_INVALID ( (uint8)(~0) )

/* define the valid XO trim tuning codes */
#define TCXOMGR_XO_TRIM_NUM_TUNING_CODES  63
#define TCXOMGR_XO_TRIM_MID_TRIM  ((TCXOMGR_XO_TRIM_NUM_TUNING_CODES+1)/2)

typedef uint8 tcxomgr_trim_type;

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_RES

DESCRIPTION
  This function returns the XO Trim/Capacitor/Coarse Rotator calculated
  resolution in hrppm between two xo trim settings

DEPENDENCIES
  None

RETURN VALUE
  XO Trim/Capacitor/Coarse Rotator calculated resolution

SIDE EFFECTS
  None

============================================================================*/

extern tcxomgr_hr_ppm_type tcxomgr_xo_trim_get_res
(
  /* xo_trim value to start calculation from (i.e. current) */
  uint8 start_val,
  /* xo_trim value to end on (i.e. destination) */
  uint8 end_val
);


/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_TO_SWITCH

DESCRIPTION
  This function returns the XO Trim/Capacitor/Coarse Rotator value to correct
  for a given error

DEPENDENCIES
  None

RETURN VALUE
  XO trim value to program

SIDE EFFECTS
  None

============================================================================*/

extern uint8 tcxomgr_xo_trim_get_to_switch
(
  /* xo_trim value to start calculation from (i.e. current) */
  uint8 start_val,
  /* error to transfer to xo_trim */
  tcxomgr_ppm_type ppm_err,
  /* error that will be left over if a transfer takes place */
  tcxomgr_ppm_type *residual_err
);


/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_TABLE_VAL

DESCRIPTION
  This function returns XO Trim/Capacitor/Coarse Rotator value that
  corresponds to the current Freq/Temp Table

DEPENDENCIES
  None

RETURN VALUE
  The XO Trim/Capacitor/Coarse Rotator value that corresponds to the
  current Freq/Temp Table

SIDE EFFECTS
  None

============================================================================*/

extern uint8 tcxomgr_xo_trim_get_table_val( void );


/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_SWITCHED

DESCRIPTION
  This function gets a flag indicating that the XO Trim/Capacitor/Coarse
  Rotator has been modified

DEPENDENCIES
  None

RETURN VALUE
  a flag indicateing that the XO Trim/Capacitor/Coarse Rotator has been
  modified

SIDE EFFECTS
  None

============================================================================*/

extern boolean tcxomgr_xo_trim_get_switched( void );


/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_SWITCH_BY_ONE

DESCRIPTION
  This function determines if the XO Trim need to be switched to account
  for the err passed in. If the XO Trim needs to be switched, this
  function will do so.

DEPENDENCIES
  None

RETURN VALUE
  PPM error change pulled in by the XO Trim switch

SIDE EFFECTS
  XO Trim could be switched

============================================================================*/

boolean tcxomgr_xo_trim_switch_by_one
(
    tcxomgr_ppm_type        to_switch                /* +/- xo_trim to switch   */
);


/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_ERR_EXCEED

DESCRIPTION
  This function returns true if we have exceeded the maximum error the PMIC
  can handle from the current trim value.

DEPENDENCIES
  None

RETURN VALUE
  TRUE, if we exceed the pmic range. False otherwise

SIDE EFFECTS
  None

============================================================================*/

boolean tcxomgr_xo_trim_err_exceed
(
  tcxomgr_hr_ppm_type    freq_err    /* The frequency error to validate */  
);

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_SWITCH_BINARY

DESCRIPTION
  Something

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

============================================================================*/

extern boolean tcxomgr_xo_trim_switch_binary
(
  tcxomgr_ppm_type      err_to_switch           /* error in ppm to switch  */
);



#endif /* !TCXOMGR_XO_TRIM_H */
