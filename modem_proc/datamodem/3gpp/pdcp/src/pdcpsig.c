/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     C O M P   T A S K

GENERAL DESCRIPTION
  This file contains all the functions, definitions and data types needed 
  for other tasks to interface to the COMP Task.

  Signal and command definitions for the COMP Task, and functions
  that should be used by other tasks to send commands to the COMP 
  Task are defined here.

EXTERNALIZED FUNCTIONS

EXTERNALIZED FUNCTIONS INTERNAL TO COMP SUBSYSTEM

INITIALIZATION AND SEQUENCING REQUIREMENTS


 
  Copyright (c) 2001 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/pdcp/src/pdcpsig.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/04/09   sa      AU level CMI modifications.
08/31/06   rsl     Featurized PDCP
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_WCDMA_PS
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "dog.h"
#include "task.h"
#include "pdcpi.h"
#include "pdcprlcif.h"
#include "pdcprrcif.h"

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
)
{
  uint8 pdcp_inst;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-----------------------------------------------------------------------
    Loop through all pdcp instances and find the instance that has the
    given signal value in its tx/rx_sig.
  -----------------------------------------------------------------------*/
  for (pdcp_inst = 0; pdcp_inst < MAX_PDCP_INST; pdcp_inst++) 
  {
	/*lint -e641*/
    if ((pdcp_ctl_blk[pdcp_inst].rlc_to_pdcp_sig == sig) ||
        (pdcp_ctl_blk[pdcp_inst].ps_to_pdcp_sig == sig))
    /*lint +e641*/
    {
      return pdcp_inst;
    }
  }
  return MAX_PDCP_INST;
} 
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
  comp_sig_enum_type sig
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-----------------------------------------------------------------------
    Make sure given pdcp_inst is valid and if it is in UP state, set the
    right rex signal.
  -----------------------------------------------------------------------*/
  if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP) 
  {
#ifdef FEATURE_COMP_LO_TASK
	if ( (pdcp_ctl_blk[pdcp_inst].traf_class == DS_UMTS_QOS_TCLASS_CONV) ||
		 (pdcp_ctl_blk[pdcp_inst].traf_class == DS_UMTS_QOS_TCLASS_STRM) )
#endif
	{
          DATA_MSG2_HIGH("setting pdcp inst %d sig %d",
                          pdcp_inst, sig);
	/*lint -e641*/
	COMP_SET_SIGNAL(sig);
	/*lint +e641*/
  }
#ifdef FEATURE_COMP_LO_TASK
	else
	{
	  DATA_MSG2_HIGH("setting pdcp inst %d sig %d", pdcp_inst, sig);
	  /*lint -e641*/
	  COMP_LO_SET_SIGNAL(sig);
	  /*lint +e641*/
	}
#endif
  }
}

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
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Make sure given pdcp_inst is in UP state, clear the right rex signal.
  -----------------------------------------------------------------------*/
  if (pdcp_ctl_blk[pdcp_inst].pdcp_state == PDCP_UP) 
  {
    DATA_MSG2_HIGH("clearing pdcp inst %d sig %d", pdcp_inst, sig);
	/*lint -e641*/
	COMP_CLR_SIGNAL(sig);
	/*lint +e641*/
  }
}

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
  void          *user_data_ptr
)
{
  /*lint -e734*/
  uint8 pdcp_inst = (uint8) ((uint32) user_data_ptr);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pdcp_tx_data_hdlr(pdcp_inst) == FALSE)
  {
    pdcp_set_data_available_sig(pdcp_inst, sig);
  }
  return TRUE;
  /*lint +e734*/
}

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
  void          *user_data_ptr
)
{
  /*lint -e734*/
  uint8 pdcp_inst = (uint8)((uint32) user_data_ptr);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pdcp_rx_data_hdlr(pdcp_inst) == FALSE)
  {
    pdcp_set_data_available_sig (pdcp_inst, sig );
  }
  return TRUE;
  /*lint +e734*/
}
#endif

