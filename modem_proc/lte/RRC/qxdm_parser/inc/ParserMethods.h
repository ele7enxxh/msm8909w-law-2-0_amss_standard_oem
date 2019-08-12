/*===========================================================================
FILE: 
   ParserMethods.h

DESCRIPTION:
   Declaration of dynamic parser exported methods

PUBLIC CLASSES AND METHODS:
   RegisterParserW/A()
      Register for the items this DLL is able to parse (UNICODE/ANSI versions)

   ParseBufferToTextW/A()
      Parse a DIAG buffer to a text string (UNICODE/ANSI versions)

   ParseBufferToSummaryW/A()
      Parse a DIAG buffer to a summary (line) text string (UNICODE/ANSI)


   NOTE: The following assumptions are made for dynamic parsing DLLs

      a) Each DLL exports the methods listed above

      b) Each DLL supports both the UNICODE/ANSI versions of the above

      c) 'wchar_t' is *NOT* defined as built in type (for VC6 compatibility),
         i.e. UNICODE string pointers are mangled as unsigned short pointers

      d) The use of different types of C/C++ libraries (version 6 or 7,
         debug/release, dynamic library/static library) should not matter


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
#include "ParserRegistration.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

// Decorated names - for use when calling GetProcAddress()

#include "lte_variation.h"
#ifdef UNICODE
LPCSTR REGISTER_NAME = "?RegisterParserW@@YAPAGXZ";
LPCSTR PARSE_TEXT_NAME = "?ParseBufferToTextW@@YAKPBEI_NPAGI@Z";
LPCSTR PARSE_SUMMARY_NAME = "?ParseBufferToSummaryW@@YAKPBEI_NPAGI@Z";
#else
LPCSTR REGISTER_NAME = "?RegisterParserA@@YAPADXZ";
LPCSTR PARSE_TEXT_NAME = "?ParseBufferToTextA@@YAKPBEI_NPADI@Z";
LPCSTR PARSE_SUMMARY_NAME = "?ParseBufferToSummaryA@@YAKPBEI_NPADI@Z";
#endif

//---------------------------------------------------------------------------
// Typedefs
//---------------------------------------------------------------------------

// RegisterParserW/A() method pointer type
typedef LPWSTR ( * tFnRegisterW)
(
  void
);

typedef LPSTR ( * tFnRegisterA)
(
  void
);

// ParseBufferToTextW() method pointer type
typedef DWORD ( * tFnParseTextW)
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  bool                       bOutbound,
  LPWSTR                     pParsed,
  UINT                       parsedSz
);

// ParseBufferToTextA() method pointer type
typedef DWORD ( * tFnParseTextA)
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  bool                       bOutbound,
  LPSTR                      pParsed,
  UINT                       parsedSz 
);

// ParseBufferToSummaryW() method pointer type
typedef DWORD ( * tFnParseSummaryW)
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  bool                       bOutbound,
  LPWSTR                     pParsed,
  UINT                       parsedSz 
);

// ParseBufferToSummaryA() method pointer type
typedef DWORD ( * tFnParseSummaryA)
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  bool                       bOutbound,
  LPSTR                      pParsed,
  UINT                       parsedSz 
);

#ifdef UNICODE
  #define tFnRegister tFnRegisterW
  #define tFnParseText tFnParseTextW
  #define tFnParseSummary tFnParseSummaryW
#else
  #define tFnRegister tFnRegisterA
  #define tFnParseText tFnParseTextA
  #define tFnParseSummary tFnParseSummaryA
#endif

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

// Register for the items this DLL is able to parse (UNICODE/ANSI versions)
__declspec( dllimport ) LPWSTR RegisterParserW
(
  void
);
__declspec( dllimport ) LPSTR RegisterParserA
(
  void
);

// Parse a DIAG buffer to a text string (UNICODE version)
__declspec( dllimport ) DWORD ParseBufferToTextW
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  bool                       bOutbound,
  LPWSTR                     pParsed,
  UINT                       parsedSz 
);

// Parse a DIAG buffer to a text string (ANSI version)
__declspec( dllimport ) DWORD ParseBufferToTextA
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  bool                       bOutbound,
  LPSTR                      pParsed,
  UINT                       parsedSz 
);

// Parse a DIAG buffer to summary (line) text string (UNICODE version)
__declspec( dllimport ) DWORD ParseBufferToSummaryW
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  bool                       bOutbound,
  LPWSTR                     pParsed,
  UINT                       parsedSz 
);

// Parse a DIAG buffer to summary (line) text string (ANSI version)
__declspec( dllimport ) DWORD ParseBufferToSummaryA
(
  const BYTE *               pBuf,
  UINT                       bufSz,
  bool                       bOutbound,
  LPSTR                      pParsed,
  UINT                       parsedSz 
);


#ifdef UNICODE
  #define RegisterParser RegisterParserW
  #define ParseBufferToText ParseBufferToTextW
  #define ParseBufferToSummary ParseBufferToSummaryW
#else
  #define RegisterParser RegisterParserA
  #define ParseBufferToText ParseBufferToTextA
  #define ParseBufferToSummary ParseBufferToSummaryA
#endif

// NOTE: The size of the DIAG buffer is specified in bytes, the size of
// the output text buffer is specified in characters (including the
// terminating NULL character)


