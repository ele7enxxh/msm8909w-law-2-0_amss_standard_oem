/*!
  @file
  Parsers.h

  @brief
  QXDM log parsers declaration and registration module header

*/

/*===========================================================================

  Copyright (c) 2007 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/qxdm_parser/src/Parsers/Parsers.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/30/07   trc     Initial version
===========================================================================*/

#ifndef PARSERS_H
#define PARSERS_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "Common.h"
#include "ParserRegistration.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! @brief Parser (Text/Summary) handler function
*/
typedef void (*PFF)(const LogPacketHeaderType   *pInBuf,  
                    UINT                        nBufSize,  
                    COutputBuffer               &pOutBuf);


/*! @brief Log packet parser definition record
*/
typedef struct
{
  WORD  LogCode;        /*!< Log Code to be handled */
  PFF   TextParser;     /*!< Text parsing function */
  PFF   SummaryParser;  /*!< Summary parsing function */
} LogPacketParserType;




/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

// Register for all supported packets
sDynaParserReg Register_Parsers
(
  void
);

// Parse item buffer to a text string 
DWORD Parse_Text
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  LPWSTR                     pParsed,
  UINT                       parsedSz,
  BOOL                       summary
);

#endif /* PARSERS_H */
