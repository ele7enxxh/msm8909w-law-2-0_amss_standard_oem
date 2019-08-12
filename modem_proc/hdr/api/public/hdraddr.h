#ifndef HDRADDR_H
#define HDRADDR_H


/** 
  @file hdraddr.h
  @brief
  Contains the definitions and declaration related to 128-bit HDR 
  addressing scheme.
*/


/*===========================================================================

  Copyright (c) 2010 by Qualcomm Technologies, Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: L:/src/asw/MSM4500/vcs/hdraddr.h_v   1.6   19 Mar 2001 21:12:58   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/api/public/hdraddr.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/10/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
07/19/10   cnx     Documented the public interfaces
08/28/09   wsh     CMI: Removing conditional defines from API headers
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
02/24/09   wsh     Created Module from
                   //source/qcom/qct/modem/hdr/cp/main/latest/inc/hdraddr.h#1

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/* <EJECT> */ 
/*===========================================================================

              DEFINITIONS AND CONSTANTS FOR ERROR CODES

===========================================================================*/
/** @addtogroup hdr_api High Data Rate API
    @{ */

/** @name HDR Address Length
    @{ */

#define HDRADDR_LENGTH_IN_BITS 128 /**< Length of the address in bits. */
#define HDRADDR_LENGTH_IN_BYTES 16 /**< Length of the address in bytes. */

/* @} */ 


/** Type definition for a 128-bit address. This includes the Access Terminal 
    Identifier (ATI), Sector ID, and Subnet ID. */ 
typedef unsigned char hdraddr_type[HDRADDR_LENGTH_IN_BYTES]; 

/** @} */ /* end_addtogroup hdr_api */

#endif /* HDRADDR_H */
