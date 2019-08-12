/*!
  @file
  Common.h

  @brief
  Common code used by the generic QXDM parser DLL

  @page QXDM_Parser_DLL QXDM Log Parser DLL

  QXDM has the ability to use an externally supplied DLL to parse DIAG log
  data.  This allows a group to build a DLL that allows them to use the
  standard QXDM tool to analyze log packets that may not be supported
  or properly formatted for maximum readability.

  <br><h1>Theory of Operation</h1>

  QXDM must be informed of the location of the custom parser DLL by means of
  the 'Dynamic Parsers' dialog under the File menu.  If multiple custom
  parser DLLs are necessary, QXDM requires that they all exist in the same
  directory.  Upon being informed of the location of the DLLs, QXDM allows
  the user to set the order of precedence (in the case where two or more DLLs
  may handle the same information.)

  With respect to each parser DLL, the theory of operation is as follows:
    -# QXDM dynamically loads the DLL and calls the RegisterParserW() or
    RegisterParserA() export, depending upon if it is operating in Unicode
    or ANSI mode.
    -# The RegisterParserW() or RegisterParserA() method returns the log
    codes which this DLL can parse.
    -# After registration, QXDM calls the ParseBufferToTextW() or 
    ParseBufferToTextA() export for a full-text parse of a log packet matching
    one of the log codes for which the DLL previous registered.
    The ParseBufferToSummaryW() or ParseBufferToSummaryA() export are called
    when a brief one-line summary is required for the matching log packet.

  In the existing framework, it is not required (or even suggested) to
  get into the details of the above DLL mechanics.  Instead, a framework
  has been implemented that allows the parser writer simply to focus on
  writing the code to parse their log packets, and glue them into the DLL
  via a simple table-based registration scheme.

  <br><h1>Writing a Custom Log Parser</h1>

  Before writing a log parser, one must consider whether they are parsing
  an entire DIAG log packet, or a subpacket within a generalized log packet.
  The generalized log packet format allows using one DIAG log code as a
  container for many subpacket logs.  For details, see 80-V1294-* for the
  DIAG ICD.  An example of a full log parser (ie. non-generalized log packet
  can be found in the file @ref SampleParser.cpp and @ref SampleParser.h)

  There are two directories of interest under the src/ directory of the
  QXDM DLL subcomponent.  The Parsers/ directory is intended for registered
  whole-log parsers, while the SubParsers/ directory is intended for the
  log subpacket parsers.  To illustrate how to add a new generalized log packet
  and parsers for the subpackets that lie within it, the following
  are a basic path to follow:

    -# In the file Parsers/Parsers.cpp, find the @ref LogPacketParsers table.
       Add an entry to the table of type @ref LogPacketParserType that contains
       the log code for which to register, a full-text parsing function, and
       a summary text parsing function.
       - In the case of a generalized log packet, the full-text parsing function
       for the generalized packet is already implemented as
       @ref GenLogTextParser(). This function will unwrap the generalized log
       packet and call each subpacket parser individually (see below.)
       - Also in the case of a generalized log packet, the summary parsing
       function usually only highlights one of the subpackets in the overall
       generalized log packet.  For this reason, the summary text parser is
       usually handled directly by the subpacket parser.
    -# To add a new subpacket to the list of subpackets handled by the
       generalized log packet processor, find the @ref SubpacketParsers table
       in the file SubParsers/SubParsers.cpp.  Add an entry of type
       @ref SubpacketParserType that contains the subpacket id, version,
       short string name of the subpacket, and the full-text parsing function
       for the subpacket.
    -# Implement your subpacket parser as a file in SubParsers/...  The
       subpacket parser will expose the two methods referenced above.  For
       an example, see the files @ref RFStatus.cpp and @ref RFStatus.h

  To assist with parsing regular log and generalized log subpackets, there are
  several helper classes used by this framework:

  - @ref COutputBuffer is a class reference passed into all parser and
    subparser functions.  It serves as a convenient wrapper to send the
    parsed output back to QXDM, hiding all of the shared buffer details.

  - @ref CGenLogPacket is a class used to wrap a generalized log packet.
    It can be used to locate a specific subpacket within a generalized
    log packet, or retrieve more information about the packet itself.

  <br><h1>Building the DLL</h1>

  Qbuild support is in place to build the DLL with minimal component-level
  support.

    - Add the following line to the .qbd file that builds your component.
      This assumes your QXDM parser DLL directory structure is named
      qxdm_parser/
    - Choose a short, but meaningful prefix unique to
      your component (e.g. 'umbsrch' for the UMB SRCH component)

  @code
  build $(ProjectPath)/qxdm_parser/ using qxdm_parser_dll.qbd projectname=<your component prefix>;
  @endcode

    - The following top-level qbuild command should invoke building of
      your QXDM DLL, prefixed with the component prefix of your choosing.

  @code
  qbuild -t=msdev -D"TreeConfig=" <your component prefix>_qxdm.dll
  @endcode

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/RRC/qxdm_parser/src/Common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/30/07   sb      Initial version
===========================================================================*/

#ifndef COMMON_H
#define COMMON_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

// Windows Header Files
// Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


/*! @brief Number of bytes per column in the COutputBuffer hexdump method */
#define HEXDUMP_BYTES_PER_COL   16


// diag struct packing on
#pragma pack( push, 1 )

/*! @brief General DIAG Log Header */
typedef struct
{
  // DIAG header for log items
  BYTE mCommandCode;   /*!< DIAG cmd code (16 for log response) */
  BYTE mMore;          /*!< More packets to come, generally 0 */
  WORD mPacketLength;  /*!< Overall payload length (not include this header) */

  // Log header
  WORD mLogLength;     /*!< Log packet length, including this header */
  WORD mLogCode;       /*!< Log code */
  ULONGLONG mTimestamp;/*!< Timestamp of logging response */

} LogPacketHeaderType;

// diag struct packing off
#pragma pack( pop )



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*! @brief Class that handles the output buffer written to the QXDM Window */
class COutputBuffer
{

  LPWSTR  pBuffer;  /*!< Pointer to the output buffer to write to */
  UINT    Size;     /*!< Maximum size of the output buffer */
  UINT    Index;    /*!< Current index into output buffer */
  INT     Status;   /*!< Status of the last write to the output buffer */

public:

  /*! Constructor, flags Status as -1 if pBuff == NULL || Sz = 0 */
  COutputBuffer(const LPWSTR pBuff, UINT Sz) :
  pBuffer(pBuff), Size(Sz), Index(0)
  {
    Status = ((pBuff == NULL) || (Sz == 0)) ? -1 : 0;
  };

  /*! Returns the status of the last write to the buffer
      @returns Number of characters last written to buffer or < 0 upon 
      truncation of last write (ie. out of space in output buffer) */
  INT LastStatus(void)
  {
    return Status;
  }

  /*! Writes to the buffer using a formatted sprintf() style */
  void Print(const LPWSTR, ...);

  /*! Writes a hexdump to the buffer */
  void PrintHexDump(const BYTE* pInBuf,UINT InBufSize);
};

#endif /* COMMON_H */
