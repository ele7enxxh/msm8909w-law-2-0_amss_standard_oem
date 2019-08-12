#ifndef DSUCSDLOG_H
#define DSUCSDLOG_H
/*===========================================================================
              U C S D   L O G   F U N C T I O N S  

DESCRIPTION
  This source file contains the UMTS CSD log functionality interface.  
  

Copyright (c) 1991 - 2008 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdlog.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/06/08   SA      Updated RX Frame to include FCS error.
07/24/03   TMR     Updated for UMTS log codes and RLP V2
02/17/03   TMR     Created

===========================================================================*/


/*===========================================================================
Header files
===========================================================================*/
#include "dsucsd.h"





/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/




/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_init

DESCRIPTION
   Initializes log header information that should be done prior to 
   starting to do any packet logging.  This sets up the basic log
   packet header information that is present on every log packet.
         
DEPENDENCIES
   None.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Sets some global header variables

===========================================================================*/
void
ds_ucsd_log_init
(
   uint8                   num_data_bits,
   uint8                   num_stop_bits,
   uint8                   num_parity_bits,
   boolean                 transparent_call,
   ds_ucsd_data_rate_T     data_rate,
   sys_sys_mode_e_type     curr_mode,
   uint8                   rlp_version_number   
);

/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_fax_init

DESCRIPTION
   Initializes fax log header information that should be done prior to 
   starting to do any packet logging.  This sets up the FAX information
   that is present on every log packet associated with a FAX call.
         
DEPENDENCIES
   None.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Sets some global header variables

===========================================================================*/
void
ds_ucsd_log_fax_init
(
   ds_ucsd_data_rate_T fax_rate
);


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_rx_data

DESCRIPTION
   Logs UMTS Async RX data.  Pointer to the packet to log and frame_size
   are the parameters.  Note that in the DS_UMTS_CSD_FRAME_SIZE_288_BIT
   frame size case that corresponds to GSM 14400, the first 16 bits
   of the data contain the M1 and M2 bits. 
         
DEPENDENCIES
   ds_ucsd_log_init() should be called prior to calling this.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   logs data

===========================================================================*/
void 
ds_ucsd_log_rx_data
(
   uint8                 *radio_rx_data,
   ds_ucsd_frame_size_T   frame_size,
   boolean                fcs_error
);

/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_tx_data

DESCRIPTION
   Logs UMTS Async TX data.  Pointer to the packet to log and frame_size
   are the parameters.  Note that in the DS_UMTS_CSD_FRAME_SIZE_288_BIT
   frame size case that corresponds to GSM 14400, the first 16 bits
   of the data contain the M1 and M2 bits. 
         
DEPENDENCIES
   ds_ucsd_log_init() should be called prior to calling this.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   logs data

===========================================================================*/
void 
ds_ucsd_log_tx_data
(
   uint8                 *radio_tx_data,
   ds_ucsd_frame_size_T  frame_size
);

#endif /* DSUCSDLOG_H */
