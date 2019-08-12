#ifndef PDCPSIG_H
#define PDCPSIG_H
/*===========================================================================

                              P D C P  S I G N A L

                            H E A D E R   F I L E

DESCRIPTION
  This is the external header file for the PDCP Signal handlers. This file
  contains all the functions, definitions and data types needed handling
  PDCP signals.

  Signal and command definitions for the PDCP Task, and functions
  that should be used by other tasks to send commands to the PDCP
  Task are defined here.

  Copyright (c) 2006-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/pdcp/src/pdcpsig.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/09   ss      SU level CMI modifications.
08/31/06   rsl     Featurized PDCP
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES

===========================================================================*/
/*lint -e617*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_WCDMA_PS
#include "queue.h"
#include "task.h"
#include "pdcpi.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION PDCP_GET_INST_GIVEN_SIG

DESCRIPTION
  This function returns the pdcp_inst corresponding to a given signal value.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

uint8 pdcp_get_inst_given_sig 
( 
  dword                sig
);

/*===========================================================================

FUNCTION PDCP_SET_DATA_AVAILABLE_SIG

DESCRIPTION
  This function sets the given PDCP signal in the pdcp_tcb provided
  the corresponding pdcp instance is in UP state.  IF PDCP is in
  any other state, the signal is just ignored.
  
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void pdcp_set_data_available_sig 
(
  uint8         pdcp_inst,
  comp_sig_enum_type  sig
);

/*===========================================================================

FUNCTION PDCP_CLR_DATA_AVAILABLE_SIG

DESCRIPTION
  This function clears the given PDCP signal in the pdcp_tcb provided
  the corresponding pdcp instance is in UP state.  IF PDCP is in
  any other state, the signal is ignored.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void pdcp_clr_data_available_sig 
(
  uint8         pdcp_inst,
  comp_sig_enum_type sig
);

/*===========================================================================

FUNCTION PDCP_TX_SIG_HDLR

DESCRIPTION
  This function clears the given PDCP signal in the pdcp_tcb provided
  the corresponding pdcp instance is in UP state.  IF PDCP is in
  any other state, the signal is ignored.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean pdcp_tx_sig_hdlr
(
  comp_sig_enum_type sig,
  void *user_data_ptr
);

/*===========================================================================

FUNCTION PDCP_RX_SIG_HDLR

DESCRIPTION
  This function clears the given PDCP signal in the pdcp_tcb provided
  the corresponding pdcp instance is in UP state.  IF PDCP is in
  any other state, the signal is ignored.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

boolean pdcp_rx_sig_hdlr
(
  comp_sig_enum_type sig,
  void *user_data_ptr
);
#endif /* FEATURE_DATA_WCDMA_PS */
#endif /* PDCPSIG_H */
