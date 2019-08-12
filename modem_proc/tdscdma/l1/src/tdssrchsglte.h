/*===========================================================================
FILE: tdssrchsglte.h


GENERAL DESCRIPTION
  This file contains code responsible for SGLTE mode IRAT Searches.


Copyright (c) 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/l1/src/tdssrchsglte.h#1 $ DateTime: $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/13   bw     Initial Version. Added support for SGLTE mode GSM measurements under FEATURE_SGLTE.

===========================================================================*/

#ifndef TDSSRCH_SGLTE_H
#define TDSSRCH_SGLTE_H

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "tdscdma_variation.h"
#include "comdef.h"
#include "customer.h"

#include "tdsl1rrcif.h"
#include "geran_grr_msg.h"
/*-------------------------------------------------------------------------*/
/*                        MACROS                                           */
/*-------------------------------------------------------------------------*/
//#define FEATURE_SGLTE_UNIT_TEST

#define TDSSRCHSGLTE_SEND_REQUEST_PERIOD  32 /*32 sub-frames to periodically send the measurement request in SGLTE mode*/


/*-------------------------------------------------------------------------*/
/*                        TYPE DEFINITIONS                                 */
/*-------------------------------------------------------------------------*/
typedef struct
{
  /* measurement mode
     0 - normal mode
     1 - sglte mode
   */ 
  tdsl1_meas_mode_enum_type   meas_mode; 

  /* sfn when the last request is sent */
  uint16 send_meas_req_timestamp;
  
  /* whether need to do meas */
  boolean start_meas;
   
}tdssrchsglte_global_struct_type;


/*-------------------------------------------------------------------------*/
/*                        GLOBAL VARIABLES                                 */
/*-------------------------------------------------------------------------*/
extern tdssrchsglte_global_struct_type  tdssrchsglte;


/*===========================================================================

FUNCTION:  TDSSRCHLTE_INIT

DESCRIPTION:
  Inits the full internal variables as well as initializing functions to
  allow this module to perform its operations.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchsglte_init(void);

/*===========================================================================
FUNCTION        tdssrchsglte_meas_timer_expire

DESCRIPTION    
this function decides if the timer to send the measurement request expired.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
boolean tdssrchsglte_meas_timer_expire(void);


/*===========================================================================
FUNCTION        tdssrchsglte_meas_timer_update

DESCRIPTION    
this function update the timer to send the measurement to the current frame number.


DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
void tdssrchsglte_meas_timer_update(void);

/*===========================================================================

FUNCTION tdssrchsglte_send_gsm_meas_req

DESCRIPTION:
  This function send the measurement request message to GSM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchsglte_send_gsm_meas_req(void);

/*===========================================================================
FUNCTION        tdssrchsglte_meas_timer_expire

DESCRIPTION    
this function decides if the timer to send the measurement request expired.

DEPENDENCIES    

RETURN VALUE    None

SIDE EFFECTS    
===========================================================================*/
boolean tdssrchsglte_meas_timer_expire(void);


/*===========================================================================
FUNCTION tdssrchsglte_handle_meas_result

DESCRIPTION
  Check if the GSM serving cell is within the neighbor list from NW
	Yes
		update the RSSI and BSIC info for that cell
		Clear the stored measurement info for other cells
		do event/periodic reporting in DCH or cell reselection evaluation in FACH/DRX.
	No
		Do not update the cell info and no cell evaluation.
		Clear the stored measurement info for all cells 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tdssrchsglte_handle_gsm_meas_result(msgr_hdr_struct_type* hdr);

#endif /* TDSSRCH_SGLTE_H */

