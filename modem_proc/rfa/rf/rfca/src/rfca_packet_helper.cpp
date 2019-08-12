/******************************************************************************/
/**

	@brief Implementation of the RfcaPacketHelper class

*******************************************************************************/

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/src/rfca_packet_helper.cpp#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
01/14/14   aki     Dynamic allocation for request buffer
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
06/10/13   aki     Mainline RFCA  
06/10/13   aki     Reuse AppendRequest function
08/23/12   aki     Printing log messages if buffer is full
04/24/12   aki     AllocateResponse takes 16-bit arg instead of 32-bit
02/24/12   aki     Added log messages for error cases
02/08/12   aki     Removed type cast to dword* and unnecessary functions
10/28/11   aki     Initial revision

==============================================================================*/


#include "rfa_variation.h"
#include "comdef.h"
#include "stringl.h" /* for memscpy */

#include "rfca_packet_helper.h"
#include "rfca_helper.h"
#include "rfca_mem_helper.h"
#include "msg.h"

#include <memory.h>	// Included for memcpy()

/**
	\brief Deallocates response buffer

*/
void RfcaPacketHelper::FreeResponse()
{
	if( NULL != m_aResponseData )
	{
			RFCA_Free(m_aResponseData);
	}
	m_aResponseData = NULL;
	m_iResponseLen = 0;
	m_iResponseMaxLen = 0;
}

void RfcaPacketHelper::FreeRequest()
{
	if(NULL != m_aRequestData)
	{
		RFCA_Free(m_aRequestData);
	}
	m_aRequestData = NULL;
	m_iRequestLen = 0;
}

void RfcaPacketHelper::ResizeRequest()
{
	byte* oldRequestData = m_aRequestData;
	m_aRequestData = (byte*)RFCA_Realloc(m_aRequestData, this->m_iRequestLen);
	if(NULL == m_aRequestData)
	{
		// Realloc failed, restore old pointer
		m_aRequestData = oldRequestData;
	}
}

/**
  \brief Allocates response buffer of given size. 
   
  \param maxResponseSize Buffer size to be callocated 
   
  \return true - Success, false - Memory allocation failed

*/
bool RfcaPacketHelper::AllocateResponse(uint16 maxResponseSize)
{
	if(NULL != m_aResponseData)
	{
		FreeResponse();
	}
	m_iResponseMaxLen = 0;
	m_iResponseLen = 0;

	m_aResponseData = (byte*)RFCA_Malloc(maxResponseSize);
	if(NULL == m_aResponseData)
	{
		MSG_ERROR("Failed to allocate memory for FTM response", 0, 0 ,0);
		return false;
	}
  memset(m_aResponseData, 0xBB, maxResponseSize);
  m_iResponseMaxLen = maxResponseSize;
	return true;

}

/**
   
  \brief Initialize member variables

	\return none
*/
RfcaPacketHelper::RfcaPacketHelper( )
{
	// Reset all variables
	Init();
}

/**
   
  \brief Cleans up RfcaPacketHelper object

	\return none
*/
RfcaPacketHelper::~RfcaPacketHelper(  )
{
	FreeRequest();
	FreeResponse();	
}

/**
   
  \brief Append an 8-bit value to the request data

	\param iValue = value to add to the request packet

	\return None
*/
void RfcaPacketHelper::AppendRequest( byte iValue )
{
	this->AppendRequest((byte*)&iValue, 1);
}

/**
   
	\brief Append a 16-bit value to the request data

	\param iValue = value to add to the request packet

	\return None
*/
void RfcaPacketHelper::AppendRequest( word iValue )
{
	this->AppendRequest((byte*)(&iValue), sizeof(word));
}

/**

	\brief Append a buffer the request data

	\param piBuffer = pointer to the buffer of data that will be added
	\param iNumBytes = number of bytes to apped to the request packet

	\return None
*/
void RfcaPacketHelper::AppendRequest( byte* piBuffer, word iNumBytes)
{
	if(NULL == m_aRequestData)
	{
		MSG_ERROR("Request buffer has not been allocated", 0 ,0 ,0);
		return;
	}

	// Make sure there will not be an overflow
	if ( m_iRequestLen < (int)(RFCA_PACKET_HELPER_MAX_PACKET_SIZE - iNumBytes) )
	{
		// Store in memory
		memscpy( &m_aRequestData[m_iRequestLen], iNumBytes, piBuffer, iNumBytes );

		// Increment the length
		m_iRequestLen += (word) iNumBytes;
	}
  else
  {
    MSG_ERROR("Warning: Trying to add bytes to request buffer which is full", 0, 0, 0);
  }

}


/**

	\brief Reset the request and response packets, including data storage space and counters

	\return None
*/
void RfcaPacketHelper::Init( void )
{

	m_aRequestData = NULL;
	FreeRequest();
	m_aResponseData = NULL;
	FreeResponse();
		
	// Allocate max size for request, user is expected to call ResizeRequest if memory needs to be saved
	m_aRequestData = (byte*)RFCA_Malloc(RFCA_PACKET_HELPER_MAX_PACKET_SIZE);
	if(NULL == m_aRequestData)
	{
		MSG_ERROR("Failed to allocate memory for FTM request", 0, 0 ,0);
		return;
	}
	// Zero out the data storage areas--use patterns so debuggers can see memory as it is filled in
	memset( m_aRequestData, 0xAA, RFCA_PACKET_HELPER_MAX_PACKET_SIZE );	

}

/**

	\brief Gets 'word' from response buffer from given offset

  \param offset	Offset to response buffer
	\param data   Pointer to memory where data is copied

  \return true - Success, false - No reponse data or response 
   				too short.
*/
bool RfcaPacketHelper::GetResponseWordFromOffset( word offset, word *data)
{
  if((NULL == m_aResponseData) || (m_iResponseLen-1+sizeof(word))<offset)
  {
		MSG_ERROR("Response buffer is either NULL or too short to extract a WORD", 0, 0 ,0);
    return false;
  }
  memscpy(data, sizeof(word), m_aResponseData + offset, sizeof(word));
  return true;
}

/**
	\brief Gets 'dword' from response buffer from given offset

  \param offset	Offset to response buffer
	\param data   Pointer to memory where data is copied

  \return true - Success, false - No reponse data or response 
   				too short.
*/
bool RfcaPacketHelper::GetResponseDwordFromOffset( word offset, dword *data)
{
  if((NULL == m_aResponseData) || (m_iResponseLen-1+sizeof(dword))<offset)
  {
    MSG_ERROR("Response buffer is either NULL or too short to extract a DWORD", 0 ,0 ,0);
    return false;
  }
  memscpy(data, sizeof(dword), m_aResponseData + offset, sizeof(dword));
  return true;
}
