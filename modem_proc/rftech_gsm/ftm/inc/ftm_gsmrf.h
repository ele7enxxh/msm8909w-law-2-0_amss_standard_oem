#ifndef FTM_GSMRF_H
#define FTM_GSMRF_H

/*===========================================================================

        R F  C O N T R O L  D E F I N I T I O N S  H E A D E R  F I L E

DESCRIPTION
   This file contains  definitions for RF controls which are also 
   contained in a file on the PC.  This keeps them in a place where they 
   can be easily modified and updated as necessary to aid in it's maintenance.

 
   Copyright (c) 2001, 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  Initially, comments will be entered manually. Later, as the module is
  placed under version control, the version control tool will automatic-
  ally insert change comments into this section. [Delete this comment].

  
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_gsm/ftm/inc/ftm_gsmrf.h#1 $ $DateTime: 2016/12/13 07:59:51 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/10/02   xw      Cleaned up the code and renamed it to ftm_gsmrf.h
06/11/01   avi     Created

=============================================================================*/

#include "comdef.h"

typedef enum {  
  PDM_GSM_VCTCXO_ADJ,
  PDM_GSM_I_DC_OFFSET,
  PDM_GSM_Q_DC_OFFSET,
  PDM_GSM_MAX_NUM
} gsm_pdm_id_type;

typedef enum {
  LNA_R0,			// 00: Maximum Gain
  LNA_R1,			// 01: 
  LNA_R2,			// 10: 
  LNA_R3			// 11: Minimum Gain
} lna_range_type;
 
/*===========================================================================

FUNCTION GSM_SET_PA_START_DELTA

DESCRIPTION
   This function Modifies the pa start time
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   It modifies pa_start_delta used by gsm_prepare_tx_burst()

===========================================================================*/
void gsm_set_pa_start_delta(int16 delta);

/*===========================================================================

FUNCTION GSM_SET_PA_STOP_DELTA

DESCRIPTION
   This function Modifies the pa stop time
   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   It modifies pa_start_delta used by gsm_prepare_tx_burst()

===========================================================================*/
void gsm_set_pa_stop_delta(int16 delta);

#endif /* #ifdef FTM_GSMRF_H */

