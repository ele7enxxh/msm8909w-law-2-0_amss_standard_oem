#ifndef HDRSAPRX_H
#define HDRSAPRX_H

/*===========================================================================

       H D R SIGNALING ADAPTATION P R O T O C O L TRANSMIT MODULE


DESCRIPTION
  This file implements the Signaling Adaptation Protocol Transmit module

EXTERNALIZED FUNCTIONS (Global)

EXTERNALIZED FUNCTIONS (Regional)

INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2011 - 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/05/12   sju     Added verbose debug f3 support for SAP protocols. 
11/08/11   cnx     Fixed compiler warnings.
10/28/11   pba     Optimized handoff phase 2 support
04/12/11   cnx     Fixed compiler error.
03/22/11   sju     Created module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "qw.h"
#include "comdef.h"
#include "rex.h"
#include "dsm.h"
#include "hdrhai.h"
#include "hdrerrno.h"
#include "hdrbit.h"

#ifdef FEATURE_LTE_TO_HDR_OH
#include "hdrind.h"
#include "lte_irat_types.h"
#include "emm_irat_if_msg.h"

/*===========================================================================

          DEFIDLEIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/


/*===========================================================================

               DATA DECLARATIONS

===========================================================================*/
      

/*===========================================================================
FUNCTION HDRSAPTX_IND_CB

DESCRIPTION
  This function processes incoming indications for this protocol.
  
DEPENDENCIES
  None.

PARAMETERS
  ind_name     - Name of the indication from hdrhai.h
  ind_data_ptr - Pointer to any indication-specific data
  
RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrsaptx_ind_cb
(
  hdrind_ind_name_enum_type ind_name,
  void *ind_data_ptr 
);


/*===========================================================================
FUNCTION HDRSAPTX_TIMER_CB

DESCRIPTION
  This function is the callback for all saptx module timers.
  This function is called by REX when the timer expires, and runs 
  in the context of the task that defined the timer (HDRTX).

DEPENDENCIES
  None.

PARAMETERS
  timer_id - The ID of the timer that has expired

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrsaptx_timer_cb
(
  uint32 timer_id
);

/*===========================================================================
FUNCTION HDRSAPTX_PROCESS_TIMER                                       EXTERNAL

DESCRIPTION
  This function processes timer expiry event for this protocol.

DEPENDENCIES
  None.

PARAMETERS
  timer_id: Timer Id

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrsaptx_process_timer
(
 uint32 timer_id
);

/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_IND

DESCRIPTION
  This function processes indications given to SAPRX.

DEPENDENCIES
  None.

PARAMETERS
  ind_name - Name of the indication for SAPRX to process
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrsaprx_process_ind
(
  hdrind_ind_name_enum_type ind_name,
  hdrind_ind_data_union_type * ind_data_ptr
);


#ifdef FEATURE_IRAT_DSM_SUPPORT
/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_LTE_DL_DSM_MSG

DESCRIPTION
  This function processes LTE DownLink message received event.

DEPENDENCIES
  None.

PARAMETERS
  dl_msg_ind - pointer to the DownLink Info Transfer indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrsaprx_process_lte_dl_msg
(
  emm_irat_3gpp2_dl_msg_ind_type *dl_msg_ind
);

#else

/*===========================================================================

FUNCTION HDRSAPRX_PROCESS_LTE_DL_MSG

DESCRIPTION
  This function processes LTE DownLink message received event.

DEPENDENCIES
  None.

PARAMETERS
  ind_data_ptr - pointer to data associated with the indication

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrsaprx_process_lte_dl_msg
(
  lte_irat_tunnel_dl_msg_ind_s *rx_msg_ptr
);
#endif /* FEATURE_IRAT_DSM_SUPPORT */

/*===========================================================================

FUNCTION HDRSAPRX_IND_CB

DESCRIPTION
  This function is registered to be called whenever a protocol gives an
  indication of interest to SAPRX.

DEPENDENCIES
  None

PARAMETERS
  ind_name - Indication to process, is a global enum for all
                    possible indications in HDR signaling
  ind_data - Optional data pointer for indication specific data

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void hdrsaprx_ind_cb (hdrind_ind_name_enum_type ind_name, void *ind_data);

/*===========================================================================
FUNCTION HDRSAPRX_SET_VERBOSE_DEBUG

DESCRIPTION
  This function initializes the verbose debug setting .
  
DEPENDENCIES
  None.

PARAMETERS
  verbose_debug_setting - current verbose debug setting

RETURN VALUE
  None.

SIDE EFFECTS
  None.
  
===========================================================================*/
extern void hdrsaprx_set_verbose_debug
(
  uint8 verbose_debug_setting
);
#endif /* FEATURE_LTE_TO_HDR_OH */

#endif /* HDRSAPRX_H */

