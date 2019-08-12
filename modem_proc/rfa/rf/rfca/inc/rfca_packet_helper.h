/*!
  @file
  rfca_packet_helper.h

  @brief
  Class to handle creation of packets for request and response.

*/

/*==============================================================================

  Copyright (c) 2011-2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

==============================================================================*/

/*==============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_packet_helper.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/03/14   aki     Dynamic request allocation 
06/10/13   aki     Mainline RFCA  
06/07/13   aki     Private copy constructor and assignment operator
04/25/13   aki     Increased max packet size to 3600
09/14/12   aki     Added GetResponseMaxLengthPtr()
04/23/12   aki     Max msg size increased from 1024 to 3072
03/01/12   aki     Documentation update
02/08/12   aki     Dynamic response allocation, bigger req buffer
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_PACKETHELPER_H_
#define _RFCA_PACKETHELPER_H_

#include "rfa_variation.h"
#include "comdef.h"

#include "rfca_custom_types.h"

//! Maximum size of a Diagnostic Packet
#define RFCA_PACKET_HELPER_MAX_PACKET_SIZE 3600

/*--------------------------------------------------------------------------------------*/
/**
  Class RfcaPacketHelper

  \brief Class to handle creation of packets for request and response.

  This class allows the user to append values to the request data one at a time.  As
  each request is added, a "packed" memory buffer is created, which models contains
  the request packet data, exactly as the user requested.  For example, there is no
  filler space between elements in order to align on word boundaries.

  To handle the reponse data, values are "popped" from the end of the response packet,
  which is the location where data is returned from the phone.

  This is from QMLS source code.

  @todo Make make allocation size dynamic and use exact request size.
*/
/*--------------------------------------------------------------------------------------*/
class RfcaPacketHelper
{
public:

  RfcaPacketHelper();

  ~RfcaPacketHelper();

  void AppendRequest( byte iValue );

  void AppendRequest( word iValue );

  void AppendRequest( byte* piBuffer, word iNumBytes);

  word GetRequestLength( void ) { return m_iRequestLen;}

  word GetResponseLength( void ) { return m_iResponseLen;}

  word GetResponseMaxLength( void ) { return m_iResponseMaxLen;}

  word* GetResponseMaxLengthPtr( void ) { return &m_iResponseMaxLen;}

  byte* GetRequestDataPtr( void ) { return m_aRequestData;}

  byte* GetResponseDataPtr( void ) { return m_aResponseData;}

  word* GetResponseLengthPtr( void ) { return &m_iResponseLen;}

  bool GetResponseWordFromOffset( word offset, word *data);

  bool GetResponseDwordFromOffset( word offset, dword *data);

  void Init( void );

  bool AllocateResponse(uint16 size);

  void FreeResponse();

  void ResizeRequest();

  void FreeRequest();

private:

  //! Request Data Storage
  //byte m_aRequestData[RFCA_PACKET_HELPER_MAX_PACKET_SIZE];
  byte *m_aRequestData;

  //! Response Data Storage
  //byte m_aResponseData[RFCA_PACKET_HELPER_MAX_PACKET_SIZE];
  byte *m_aResponseData;

  //! Request data length
  word m_iRequestLen;

  //! Response data length
  word m_iResponseLen;

  //! Response max data length
  word m_iResponseMaxLen;

  // Private assign and copy operators to remove KW CL.FFM.ASSIGN and CL.FFM.COPY warnings
  // If there is need to these then a deep copy versions need to be implemented
  RfcaPacketHelper& operator=(const RfcaPacketHelper&){ return *this;}
  RfcaPacketHelper(const RfcaPacketHelper& src){ /* do not create copies */ }

};


#endif // _RFCA_PACKETHELPER_H_
