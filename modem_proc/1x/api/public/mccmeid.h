#ifndef MCCMEID_H
#define MCCMEID_H

/*==========================================================================

      C D M A   M A I N   C O N T R O L   FOR   MEID   SUPPORT
*/
/** 
@file mccmeid.h

@brief CDMA Main Control For MEID Support

This header file contains the functions that supports MEID.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CDMA2k_1X_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the CDMA2k_1X_rapi_mainpage.dox file or 
      contact Tech Pubs.

      The above description for this file is part of the "main_control_meid" group 
      description in the CDMA2k_1X_rapi_mainpage.dox file. 
===========================================================================*/

/*===========================================================================
  Copyright (c) 2004 - 2013 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/public/mccmeid.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/14   gga     Added handling for 1x-NV callback registration
03/31/11   ssh     Added Doxygen comments. 
11/01/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
09/01/10   azh     Added Doxygen comments. 
10/12/09   jj      Meta-Comments cleanup for API's exported to DS.
05/14/09   jj      CMI phase2 changes.
04/17/06   fc      Added meta comments.
09/22/04   ht      Created file.    

===========================================================================*/


/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/

#include "comdef.h"
#include "qw.h"
#include "nv.h"

/** @addtogroup main_control_meid 
@{ */

/*===========================================================================
 
                           DATA DECLARATIONS

===========================================================================*/



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/



/* <EJECT> */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/* <EJECT> */

/*===========================================================================*/
/* mcc_esn_programming_is_allowed_with_meid_support */
/**
  Checks whether Electronic Serial number (ESN) programming is allowed when 
  the MEID is supported.

  @return 
  TRUE -- Mobile can be programmed with a true ESN (not a pseudo-ESN) for
          pre-PREV 11 mobiles.\n
  FALSE -- MEID and pseudo-ESN are used. The mobile does not accept ESN programming.

  @dependencies 
  None.
*/
boolean mcc_esn_programming_is_allowed_with_meid_support (void);


/*===========================================================================*/
/* mcc_meid_create_sha1_digest */
/**
  Computes the message digest (160 bits) using the Secure Hash Algorithm (SHA-1).

  @return 
  TRUE -- Success.\n
  FALSE -- Failure.

  @dependencies 
  None.
*/
boolean mcc_meid_create_sha1_digest(qword, unsigned long *);


#define  MCC_PESN_PREFIX       0x80000000UL     /**< Pseudo-ESN prefix. */
#define  MCC_PESN_PREFIX_MASK  0xFF000000UL     /**< Pseudo-ESN prefix mask. */

#define  MCC_GET_PSEUDO_ESN   (((meidhash[4]) & (~MCC_PESN_PREFIX_MASK)) | (MCC_PESN_PREFIX))
         /**< Gets the pseudo-ESN prefix or mask. */


/*===========================================================================*/
/* mcc_compute_pseudo_esn */
/**
  Creates the pseudo-ESN from the MEID. This function is provided for diagnostic 
  use to compute the pseudo-ESN from an MEID.  

  @return 
  Value of the pseudo-ESN.
  
  @dependencies 
  None.
*/
dword mcc_compute_pseudo_esn(qword);


/*===========================================================================*/
/* mcc_req_handler */
/**
  Handles the command used for Callback Registration with NV to get appropriate
  action done

  @return 
  TRUE -- Valid command processing
  FALSE -- Invalid command processing

  @dependencies 
  None.
*/
boolean mcc_req_handler (uint32, void *, void *);

/** @} */ /* end_addtogroup main_control_meid */

#endif /* MCCMEID_H */
