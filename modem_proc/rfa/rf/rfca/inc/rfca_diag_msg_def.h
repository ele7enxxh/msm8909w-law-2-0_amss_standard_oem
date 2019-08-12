/*!
  @file
  rfca_diag_msg_def.h
 
  @brief
  Defines DIAG messages for RFCA communication.

	@verbatim
	 
	REQ_RESP Model 1

	This model represents a case where HAL request is responded with two response packages.
	The first response is just an acknowledgment indicating that a delayed reponse will be sent later.
	The second reponse is the delayed reponse which will contain the actual HAL reponse data.
	This request-response sequence is used for all HAL commands that queue messages to FTM sequencer, like HAL_RF_RX_WAIT_REQ.

	+---------------------------+
	|       DIAG_RFCA_REQ       |
	|---------------------------|
	|                           |
	|                           |+---------------------->
	|                           |
	|                           |
	+---------------------------+
                                                          +---------------------------+
                                                          |      DIAG_RFCA_RESP       |
                                                          |---------------------------|
                                                          |                           |
                                  <----------------------+| *DELAYED_RESP_ID > 0 (N)  |
                                                          | *RESP_COUNT      = 0      |
                                                          |                           |
                                                          |                           |
                                                          +---------------------------+


                                                          +---------------------------+
                                                          |      DIAG_RFCA_RESP       |
                                                          |---------------------------|
                                                          |                           |
                                  <----------------------+| *DELAYED_RESP_ID = N      |
                                                          | *RESP_COUNT      = 1      |
                                                          |                           |
                                                          |                           |
                                                          | *HAL_RESP is included     |
                                                          +---------------------------+


	REQ_RESP Model 2

	This model represents scenario where HAL request is responded with only one response package.
	The first and only response is both an acknowledgment and a data reponse and contains the actual HAL reponse data.
	This request-response sequence is used for all HAL commands that do NOT queue messages to FTM sequencer, like HAL_RF_INIT.

	+---------------------------+
	|       DIAG_RFCA_REQ       |
	|---------------------------|
	|                           |
	|                           |+---------------------->
	|                           |
	|                           |
	+---------------------------+
                                                      +-------------------------------+
                                                      |      DIAG_RFCA_RESP           |
                                                      |-------------------------------|
                                                      |                               |
                              <----------------------+| *DELAYED_RESP_ID = 0          |
                                                      | *RESP_COUNT      = 0          |
                                                      | *RESP_SIZE = sizeof(HAL_RESP) |
                                                      |                               |
                                                      | *HAL_RESP is included         |
                                                      +-------------------------------+


	@endverbatim
  
*/

/*==============================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/rfca/inc/rfca_diag_msg_def.h#1 $
   
when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
06/10/13   aki     Mainline RFCA
04/23/12   aki     Removed warning of zero-length array when compiled in VC++
03/01/12   aki     Changed <> to ""
02/08/12   aki     Setting variable len array to 0 size
01/25/12   aki     Diag V2 delayed response implementation
10/28/11   aki     Initial revision

==============================================================================*/


#ifndef _RFCA_DIAG_MESSAGE_DEFINITION_H_
#define _RFCA_DIAG_MESSAGE_DEFINITION_H_

#include "rfa_variation.h"
#include "comdef.h"
#include "rfca_custom_types.h"

#ifdef _WIN32
  #pragma pack(push,1) // Save previous, and turn on 1 byte alignment
  #pragma warning( push )
  #pragma warning( disable : 4200 ) // warning C4200: nonstandard extension used : zero-sized array in struct/union
#endif

/*!

  @brief DIAG request message to send RFCA commands to modem.

 */
typedef PACK(struct) {
  uint8  command_code;    //! See \ref DIAG_SUBSYS_CMD_VER_2_F
  uint8  subsys_id;       //! See \ref DIAG_SUBSYS_FTM
  uint16 rf_mode_id;      //! See \ref FTM_RFCA_C
  uint16 rfca_msg_id;     //! See \ref HAL_MESSAGE_ID 
  uint16 ftm_data_len;    //! Payload size 
  uint16 resp_size;       //! Expected response size
  uint8  data[];         //! Hal request message data, see hal_chipset_api.h
} DIAG_RFCA_REQ;

/*!
  @brief DIAG response message received from modem.
 
  @details This response package is returned from RFCA to the caller.
           There are threee ways this reponse can be sent: as ack only reponse,
           as data reponse or as combined ack and data response.
 
  */
typedef PACK(struct) {
  uint8  command_code;          //!< See \ref DIAG_SUBSYS_CMD_VER_2_F
  uint8  subsys_id;             //!< See \ref DIAG_SUBSYS_FTM
  uint16 rf_mode_id;            //!< See \ref FTM_RFCA_C
  uint32 status;                //!< 0 Always 
  uint16 delayed_resp_id;       //!< 0 – Only one response (immediate response), >0 – Multiple responses (immediate response followed by delayed response)
  uint16 resp_count;            //!< 0 – First response (immediate), 1 – Second response (delayed/final)
  uint16 rfca_msg_id;           //!< See \ref HAL_MESSAGE_ID
  uint16 ftm_data_len;          //!< Payload size
  uint16 resp_size;             //!< Size of this response package
  uint8  data[];               //!< Hal response message data, see hal_chipset_api.h
} DIAG_RFCA_RESP;


#ifdef _WIN32
  // Restore warings and alignment
  #pragma warning( pop ) 
  #pragma pack(pop)
#endif

#endif //_RFCA_DIAG_MESSAGE_DEFINITION_H_
