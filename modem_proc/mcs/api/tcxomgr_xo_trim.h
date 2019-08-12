#ifndef TCXOMGR_XO_TRIM_H
#define TCXOMGR_XO_TRIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                T C X O    M A N A G E R    X O    T R I M
                     TCXO Manager XO Trim Header File

GENERAL DESCRIPTION

  This module .

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/tcxomgr_xo_trim.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
29/11/10   hh      CMI phase-4 changes.
10/16/07   sst     Move XO Trim NV functions to tcxomgr_data.c
07/26/07   sst     Added function tcxomgr_xo_trim_switch_by_one
07/09/07   sst     Added function update_temp_table_after_xo_trim_switch
03/07/07   sst     Initial Version

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/

#include "customer.h"
#include "tcxomgr.h"

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_GET_VAL

DESCRIPTION
  This function returns the current XO Trim/Capacitor/Coarse Rotator value
  as stored in the tcxomgr

DEPENDENCIES
  None

RETURN VALUE
  the current XO Trim/Capacitor/Coarse Rotator value as stored in the tcxomgr

SIDE EFFECTS
  None

============================================================================*/

extern uint8 tcxomgr_xo_trim_get_val( uint8 *hw_val );


/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_SET_VAL

DESCRIPTION
  This function sets the new value at which the XO Trim/Capacitor/Coarse
  Rotator has been changed to.

DEPENDENCIES
  None

RETURN VALUE
  The new xo_trim value
  TCXOMGR_XO_TRIM_INVALID - ndicates that xo_trim switching is not allowed

SIDE EFFECTS
  None

============================================================================*/

extern uint8 tcxomgr_xo_trim_set_val
(
  uint8 new_val
);

/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_SWITCH

DESCRIPTION
  Something

DEPENDENCIES
  None

RETURN VALUE


SIDE EFFECTS
  None

============================================================================*/

extern boolean tcxomgr_xo_trim_switch
(
  tcxomgr_ppm_type      err_to_switch,           /* error in ppm to switch  */
  tcxomgr_ppm_type      threshold,               /* max error to txfr in ppm*/
  tcxomgr_ppm_type     *err_txfred,              /* ppm of error txfr'ed    */
  tcxomgr_ppm_type     *residual_err             /* residual error in ppm   */
);


/*============================================================================

FUNCTION TCXOMGR_UPDATE_TEMP_TABLE_AFTER_XO_TRIM_SWITCH

DESCRIPTION
  This function will re-calculate the Freq/Temp table, based upon the number
  of Caps switched and a calculated xo_trim resolution
  Currently it just clears the table to be the default value.

DEPENDENCIES
  This function should only be called if (tcxomgr.xo_trim.switched == TRUE)
  and a new value is about to be stored into the Freq/Temp Table

RETURN VALUE
  None

SIDE EFFECTS
  F/T (vco) table is cleared

============================================================================*/

extern void tcxomgr_update_temp_table_after_xo_trim_switch
(
  uint8 temp,
  tcxomgr_ppm_type old_val,
  tcxomgr_ppm_type new_val
);

#endif /* !TCXOMGR_XO_TRIM_H */
