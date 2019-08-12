/*!
  @file
  GenLogPacket.h

  @brief
  Generalized log packet class header (CGenLogPacket)

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/lte/L2/common/qxdm_parser/src/SubParsers/GenLogPacket.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/22/08   tjc     Ported to Genlog2
04/30/07   trc     initial version
===========================================================================*/

#ifndef GENLOGPACKET_H
#define GENLOGPACKET_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "Common.h"
#include "Parsers.h"


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

//---------------------------------------------------------------------------
// Packet Definitions
//---------------------------------------------------------------------------

// diag struct packing on
#pragma pack( push, 1 )

/*! @brief General packet header */
typedef struct
{
  BYTE Version;  /*!< General header version */
  BYTE NumSubs;  /*!< Number of subpackets that follow this header */
  WORD Reserved; /*!< reserved */
} GenLogHeaderType;

/*! @brief Subpacket header */
typedef struct
{
  BYTE Id;       /*!< Subpacket ID */
  BYTE Version;  /*!< Subpacket version */
  WORD Size;     /*!< Size of this subpacket */
} GenLogSubHeaderType;

// diag struct packing off
#pragma pack( pop )


/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*! @brief Generalized Log Packet Helper Class */
class CGenLogPacket
{
  UINT                       LogPacketSize;   /*!< Overall log packet size */
  const LogPacketHeaderType *pLogPacketHeader;/*!< Pointer to diag log packet */
  const GenLogHeaderType    *pGenLogHeader;   /*!< Pointer to gen log header */
  const GenLogSubHeaderType *pFirstSubPacket; /*!< Pointer to first subpacket */

  const GenLogSubHeaderType *pCurrSubPacket;  /*!< Pointer to first subpacket */
  UINT                       CurSub;          /*!< Number of subpacket that
                                                   pCurrSubPacket points to */
  BOOL                       isValid;         /*!< Is the packet valid? */

public:

  /*! CGenLogPacket Constructor */
  CGenLogPacket(const LogPacketHeaderType* pPacket,const UINT Size);

  /*! Iterates through each subpacket one at a time.  Each time
      this function is called it returns the next subpacket.
      @retval TRUE if advanced to next subpacket
      @retval FALSE if no longer can advance to next subpacket (ie. last one) */
  BOOL GetNextSubpacket(void);

  /*! Get the first subpacket matching the given Id and Version */
  BOOL GetMatchingSubpacket(const BYTE Id,const BYTE Version);

  /*! Reset subpacket iteration back to first subpacket */
  void ResetSubpacketIndex(void)
  {
    if (isValid)
    {
      CurSub = 0;
      pCurrSubPacket = pFirstSubPacket;
    }
  }

  /*! Get the Log Code for this packet
      @return DIAG Log Code of this packet */
  WORD GetLogCode(void)
  {
    return ((pLogPacketHeader == NULL) ? 0 : pLogPacketHeader->mLogCode);
  }

  /*! Get the Generalized Log Version
      @return Generalized log version used in this packet */
  BYTE GetGenLogVersion(void)
  {
    return ((pGenLogHeader == NULL) ? 0 : pGenLogHeader->Version);
  }

  /*! Get the # of subpackets
      @return Number of subpackets in the generalized log packet */
  BYTE GetNumSubpackets(void)
  {
    return ((pGenLogHeader == NULL) ? 0 : pGenLogHeader->NumSubs);
  }

  /*! Get the Current Subpacket ID
      @return Id of current subpacket */
  BYTE GetSubpacketId(void)
  {
    return ((pCurrSubPacket == NULL) ? 0 : pCurrSubPacket->Id);
  }

  /*! Get the Current Subpacket Version
      @return Version of current subpacket */
  BYTE GetSubpacketVersion(void)
  {
    return ((pCurrSubPacket == NULL) ? 0 : pCurrSubPacket->Version);
  }

  /*! Get the Current Subpacket Size
      @return Size of current subpacket */
  WORD GetSubpacketSize(void)
  {
    return ((pCurrSubPacket == NULL) ? 0 : pCurrSubPacket->Size);
  }

  /*! Get the Current Subpacket Data pointer
      @return BYTE pointer to current subpacket (NULL upon invalid packet) */
  const BYTE *GetSubpacketDataPointer(void)
  {
    return ((!isValid || (pCurrSubPacket == NULL)) ?
             NULL : (BYTE *)pCurrSubPacket + sizeof( GenLogSubHeaderType ) );
  }

  /*! Get the Packet validity status
      @retval TRUE Packet is valid
      @retval FALSE Packet is not valid */
  BOOL IsValid(void)
  {
    return (isValid);
  }
};

#endif /* GENLOGPACKET_H */
