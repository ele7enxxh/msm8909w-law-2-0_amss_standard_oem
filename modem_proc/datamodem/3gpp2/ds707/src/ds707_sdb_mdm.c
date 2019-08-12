/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   

      S H O R T   D A T A   B U R S T   M O D E M   S I D E   S U P P O R T

GENERAL DESCRIPTION
  This file contains functions used for implementing MT-DOS data path

EXTERNALIZED FUNCTIONS
  DSHDR_DOS_MDM_RX_HANDLER
    Processes MT-DOS packets and send them to the APPS processor for further
    routing to higher layers
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

   Copyright (c) 2007 - 2010 by Qualcomm Technologies Incorporated.  All Rights Reserved.
   
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_sdb_mdm.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/10   op      Migrated to MSG 2.0 macros
02/27/09   ms      Offtarget Lint fixes.
04/17/07   ssr     New file: contains code that implements MT-DOS data path
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_HDR_QOS
#ifdef FEATURE_HDR_DOS

#include "msg.h"
#include "data_msg.h"
#include "dsm.h"
#include "dsrlp_v.h"
#include "dsrlpi.h"

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/


/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION      ds707_sdb_mdm_rx_handler

DESCRIPTION   Redirects received DoS packets back from the APPS processor, 
              to the SHIM layer, for rerouting to APPS and subsequent 
              processing by higher layers
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_sdb_mdm_rx_handler
(
  void *dsm_payload_ptr,
  uint32 rlp_flow,
  byte   route
)
{
  dsm_item_type *dos_item_ptr = (dsm_item_type*)dsm_payload_ptr;
  byte           rscb_index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  DATA_IS707_MSG3(MSG_LEGACY_MED, "Received DOS Message back in MODEM dsm_payload_ptr %0x rlp_flow %d \
           route %d" , dsm_payload_ptr, rlp_flow, route);

  rscb_index = dsrlp_get_rlp_rscb_index((byte)rlp_flow, \
                                        DSRLP_VER_HDR_MULTIFLOW_FORWARD);
  if(rscb_index == DSRLPI_INVALID_RSCB_INDEX )
  {
    DATA_IS707_MSG2(MSG_LEGACY_ERROR,
                    "RLP: %d ver: %d not found. Not accepting DoS pkt",
                    rlp_flow,
                    DSRLP_VER_HDR_MULTIFLOW_FORWARD);
    dsm_free_packet(&dos_item_ptr);
    return;
  }
  /*---------------------------------------------------------------------
     queue the pkt on the appropriate DoRA RLP's rx WM. 
     Take care of route as well.
  ---------------------------------------------------------------------*/
  dsm_enqueue(dsrlpi_rscb_array[rscb_index].cfg.rx_wm_ptr[route], 
              &dos_item_ptr);

  /*-------------------------------------------------------------------------
      Now call the RX function. This will set the flag informing incoming data
    -------------------------------------------------------------------------*/
  dsrlpi_rscb_array[rscb_index].cfg.rx_func_ptr \
     (dsrlpi_rscb_array[rscb_index].cfg.rx_data);

} /* ds707_sdb_mdm_rx_handler */

#endif /* FEATURE_HDR_QOS */
#endif /* FEATURE_HDR_DOS */
