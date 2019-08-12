#ifndef GNSS_GDT_PROTECTED_H
#define GNSS_GDT_PROTECTED_H
/*=============================================================================

                             GDT Protected Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for Protected GDT (Generic Data Transfer) in GNSS module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2014 Qualcomm Atheros, Inc. All Rights Reserved. 
  Qualcomm Atheros Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/gdt/1_0/inc/gnss_gdt_protected.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-25   ah  Initial version
  

=============================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "gnss_gdt.h"


/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/


/*=============================================================================

       Macros

=============================================================================*/


/*=============================================================================

       Function prototypes

=============================================================================*/


/*===========================================================================
FUNCTION gdt_ProtectedExtendedSend

DESCRIPTION
  This function receives a chunk of data that needs to be transported to AP.
  Different chunks of data will be accumulated and will be sent to AP at once,
  when the sender calls gdp_SendEnd().  This function is expected to only be
  called in the context of FS task and NOT by service users of GDT.  GDT
  services should call gdt_SendData which will result in this function being
  called.
 
PARAMETERS 
 
  pz_SendBlob - Blob containing buffer, length and session info for sending
  (input)

DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has accepted the data to be uploaded.
 
   Note: This does not confirm that data is transferred.
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_ProtectedExtendedSend(const gdt_SendBlobType * const pz_SendBlob);
                                        

#endif /* GNSS_GDT_PROTECTED_H */


