/*!
  @file ParserRegistration.h

  @brief Declaration of dynamic parser registration structure

  @detail
   sDynaParserReg
      This class describes the registration info for a dynamic parser.
      i.e. the items (by code) a dynamic parser can parse
*/
/*===========================================================================

Copyright (C) 2003 Qualcomm Technologies Incorporated. All rights reserved.
                   QUALCOMM Proprietary/GTDR

All data and information contained in or disclosed by this document is
confidential and proprietary information of Qualcomm Technologies Incorporated and all
rights therein are expressly reserved.  By accepting this material the
recipient agrees that this material and the information contained therein
is held in confidence and in trust and will not be used, copied, reproduced
in whole or in part, nor its contents revealed in any manner to others
without the express written permission of Qualcomm Technologies Incorporated.
===========================================================================*/

//---------------------------------------------------------------------------
// Pragmas
//---------------------------------------------------------------------------
#pragma once

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "Common.h"
#include <set>

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

/*=========================================================================*/
// eParserType Enumeration
//
//    Types of items a parser may parse
/*=========================================================================*/
enum eParserType
{
   ePARSER_ENUM_BEGIN = 0,    // We start from 0 as it makes detecting
                              // string to enum conversions errors simpler
                              // (strtol, strtoul return 0 upon error)

   ePARSER_DIAG_RX,           // Target DIAG response (minus following)
   ePARSER_DIAG_TX,           // Target DIAG request (minus following)
   ePARSER_SUBSYS_RX,         // Target DIAG subsystem dispatch response
   ePARSER_SUBSYS_TX,         // Target DIAG subsystem dispatch request
   ePARSER_EVENT,             // Target event
   ePARSER_LOG,               // Target log

   ePARSE_ENUM_END
};

/*=========================================================================*/
// Struct sDynaParserReg
/*=========================================================================*/
struct sDynaParserReg
{
   public:
      // (Inline) Constructor - default
      sDynaParserReg()
      { };

      // Constructor - populate registration info from a string
      sDynaParserReg( LPWSTR pRegInfo );
      sDynaParserReg( LPSTR pRegInfo );

      // Convert registration info to a string allocated on the system heap
      operator LPWSTR( void );
      operator LPSTR( void );

      /* DIAG reqs/rsps a parser handles (excludes subsystem dispatch) */
      std::set <UINT> mDiagReqItems;
      std::set <UINT> mDiagRspItems;

      /* DIAG subsystem dispatch reqs/rsps a parser handles */
      typedef std::pair <UINT, UINT> tSubsystemKey;
      std::set <tSubsystemKey> mSubsysReqItems;
      std::set <tSubsystemKey> mSubsysRspItems;

      /* Specific events a parser handles */
      std::set <UINT> mEventItems;

      /* Specific log items a parser handles */
      std::set <UINT> mLogItems;
};

//---------------------------------------------------------------------------
// Inline Methods
//---------------------------------------------------------------------------

/*===========================================================================
METHOD:
   IsValid (Inline Method)

DESCRIPTION:
   Is this a valid eParserType?
  
PARAMETERS:
   parserType  [ I ] - The enum value being validated

RETURN VALUE:
   bool
===========================================================================*/
inline bool IsValid( eParserType parserType )
{
   bool bRC = false;
   if (parserType > ePARSER_ENUM_BEGIN && parserType < ePARSE_ENUM_END)
   {
      bRC = true;
   }

   return bRC;
};
