#ifndef DSWCSDNTRLCIF_H
#define DSWCSDNTRLCIF_H
/*===========================================================================


                 W C D M A   N O N - T R A N S P A R E N T 
             
                 C I R C U I T - S W I T C H E D   D A T A
                             
             R A D I O   L I N K   C O N T R O L   I N T E R F A C E

                          H E A D E R   F I L E

DESCRIPTION
  This file contains data declarations and function prototypes for WCDMA 
  Non-Transparent Circuit-Switched Data to send and receive data from Radio 
  Link Control.

Copyright (c) 2003 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dswcsdntrlcif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/05/03   TMR     Updated watermark callbacks for DSM 2
03/25/03   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#ifdef FEATURE_DATA_WCDMA_CS
#include "comdef.h"

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================


FUNCTION dswcsd_nt_set_proc_ul_sig_cb


DESCRIPTION
  This function is used to set the signal DS_GCSD_PROC_UL_SIG in the GCSD
  TCB. It should be registered as a WM callback function. It gets called
  when RLC has sent the data in its Uplink watermark.


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None


===========================================================================*/
extern void dswcsd_nt_set_proc_ul_sig_cb
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
);
#else
(
   void
);
#endif


/*===========================================================================


FUNCTION dswcsd_nt_set_proc_dl_sig_cb


DESCRIPTION
  This function is used to set the signal DS_GCSD_PROC_DL_SIG in the GCSD
  TCB. It should be registered as a WM callback function. It gets called
  when RLC receives Downlink data.


DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None


===========================================================================*/
extern void dswcsd_nt_set_proc_dl_sig_cb
#ifdef FEATURE_DSM_WM_CB
(
   dsm_watermark_type  *wm_ptr, 
   void                *user_data_ptr
);
#else
(
   void
);
#endif


/*===========================================================================


FUNCTION dswcsd_nt_read_dl_ota_data_cb


DESCRIPTION
  Function used to read downlink OTA data from lower layers.  
  dl_data_ptr parameter is set by the callback function to 
  point to the block of downlink data.   

 
DEPENDENCIES
  None


RETURN VALUE
  Boolean value returned indicates whether or not downlink 
  data was successfully read or not.


SIDE EFFECTS
  None


===========================================================================*/
extern boolean dswcsd_nt_read_dl_ota_data_cb
(
  uint8 **dl_data_ptr               /* Pointer to a block of Downlink data */
);


/*===========================================================================


FUNCTION dswcsd_nt_write_ul_ota_data_cb


DESCRIPTION
  Function used to write uplink data to the lower layers.
  ul_data_ptr points to the data block to be written.
 
  
DEPENDENCIES
  None


RETURN VALUE
  Boolean indicating whether or not write operation was successful.


SIDE EFFECTS
  None


===========================================================================*/
extern boolean dswcsd_nt_write_ul_ota_data_cb 
(
  uint8 *ul_data_ptr                  /* Pointer to a block of Uplink data */
);


/*===========================================================================


FUNCTION dswcsd_nt_get_ul_ota_buf_cb


DESCRIPTION
  This function is used to return a buffer to be used to 
  for uplink data prior to it being written via the above
  function call.  Function returns the uplink data buffer
  via the ul_data_ptr parameter.
 
  
DEPENDENCIES
  None


RETURN VALUE
  Boolean indicating whether or not the operation was successful.


SIDE EFFECTS
  None


===========================================================================*/
extern boolean dswcsd_nt_get_ul_ota_buf_cb
(
  uint8 **ul_data_ptr                 /* Pointer to the Uplink data buffer */
);


/*===========================================================================


FUNCTION dswcsd_nt_init_ota_cb


DESCRIPTION
  This function is used to initialize the OTA subsystem prior
  to starting up a call.  In the below function used to register
  this callback function, this function pointer may be set to 
  NULL if no initialization function is needed
  
  
DEPENDENCIES
  None


RETURN VALUE
  None


SIDE EFFECTS
  None


===========================================================================*/
extern void dswcsd_nt_init_ota_cb( void );

#endif /* FEATURE_DATA_WCDMA_CS */


#endif  /* DSWCSDNTRLCIF_H */
