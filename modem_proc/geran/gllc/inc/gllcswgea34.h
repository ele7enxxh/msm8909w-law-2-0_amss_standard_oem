/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/inc/gllcswgea34.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/12   NDL     First release
===========================================================================*/

#ifndef GLLCSWGEA34_H
#define GLLCSWGEA34_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "gllcswgea34def.h"

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*---------------------------------------------------------------------------
  Constant declarations
  -------------------------------------------------------------------------*/

//! Kasumi KOIi buffer size
#define GFW_KASUMI_KOI_SIZE 96

//! Kasumi KLi buffer size
#define GFW_KASUMI_KLI_SIZE 32

/*---------------------------------------------------------------------------
  Type declarations
  -------------------------------------------------------------------------*/

typedef struct
{
   vect64 KOi[GFW_KASUMI_KOI_SIZE / 8];
   vect64 KLi[GFW_KASUMI_KLI_SIZE / 8];

} GfwKasumiScratchType;

//! Control structure for the GEA/3 and GEA/4 algorithms
typedef struct 
{
   // The following three fields must be initialized by the caller
   boolean bFirst;        //!< TRUE if this is the first call to the GEA algorithm
   int32   nBytesTotal;   //!< Total number of bytes to generate
   int32   nBytesPerCall; //!< Number of bytes to generate at each call

   // The following fields are updated by the gfw_gea_34() function. Caller must not modify them
   int32   nBytesLeft;    //!< Number of bytes still to be generated
   vect64  key128[2];     //!< Extended key
   vect64  keyCopy[2];    //!< Copy of the key
   vect64  keyMod[2];     //!< Modified key
   vect64  mainReg;       //!< Main shift register
   vect64  tempReg;       //!< Temporary register
   uint32  blockCount;    //!< Block counter
   GfwKasumiScratchType kasumiScratch; //!< Kasumi scratch buffer

} GfwGea34ControlStruct;

/*===========================================================================

                      EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

  FUNCTION: gfw_gea_34()

===========================================================================*/
/*!
  @brief
  GEA/3 and GEA/4 function

  @details
  This function generates ciphered streams according to the GEA/3 and GEA/4
  algorithms.
  User specifies the key, the input (frame number) and the number of bytes
  that need to be generated.
  The function can be operated in two modes.

  MODE A - The whole ciphered stream is generated at once.
  To achieve this, the user must set both nBytesPerCall and nBytesTotal to
  the number of bytes that need to be generated. bFirst must be set to TRUE.
  Sample code:
  
  control.bFirst = TRUE;
  control.nBytesTotal = NBYTES_TOT; // e.g. 1500 bytes
  control.nBytesPerCall = NBYTES_TOT;

  gfw_gea_34(pKey, len, fn, dir, &ciph[0], &control);

  MODE B - The function generates nBytesPerCall at a time.
  It is responsibility of the caller:
  - to ensure that bFirst is set to TRUE prior to the first call
  - to ensure that nBytesPerCall is a multiple of 8
  - to control the number of function calls. 'nBytesLeft' can be used for shit purpose,
    which is valid after the first call
  Sample code:
  
  control.bFirst = TRUE;
  control.nBytesTotal = NBYTES_TOT; // e.g. 1500 bytes
  control.nBytesPerCall = NBYTES_PER_CALL; // e.g. 8 bytes

  do {

    gfw_gea_34(pKey, len, fn, dir, &ciph[0], &control);

    // Use ciphered streams here...

  } while (control.nBytesLeft > 0); // nBytesLeft valid only after 1st call
  
  @param[in] pKey
     Pointer to the key buffer
  @param[in] kLen
     Key length, in bits
  @param[in] input
     Input to the algorithm (frame number)
  @param[in] direction
     Direction (1 or 0)
  @param[out] pCipheredStream
     Pointer to the generated data stream. Must contain at least 'nBytesPerCall' bytes
  @param[out] pControl
     Pointer to the I/O control structure. See code samples above for inputs.
     The structure state must be retained between calls
  @return
     None
*/
void gfw_gea_34(vect64                *pKey, 
                int32                  kLen, 
                uint32                 input, 
                uint8                  direction, 
                vect64                *pCipheredStream, 
                GfwGea34ControlStruct *pControl);

#endif /* GLLCSWGEA34_H */
