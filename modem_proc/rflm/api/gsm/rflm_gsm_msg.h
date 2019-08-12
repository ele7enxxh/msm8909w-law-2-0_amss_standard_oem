/*!
  @file
  rflm_gsm_msg.h

  @brief
  Describes modules available for GSM using message router under Tech RFLM. 

  @details
  This file contains all Tech_Module definitions under Tech RFLM
  that needs to use the message router. RFLM driver of all modes
  (LTE, Wimax, 1x, HDR, WCDMA, GSM, GPS) that needs to use the 
  mesasge router needs to define the Tech_Module in this file in order
  to use the message router. This is a common header for rflm of all modes.
  
*/


/*===========================================================================

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/api/gsm/rflm_gsm_msg.h#1 $ 
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/02/14   sml     Read WTR or RF front end devices status in between Rx/Tx 
                   burst along with GRFC status
07/04/14   jr      Changes for TSTS support 
07/01/14   jr      Enable therm read on sub-2 
03/12/14   sw      Add meaninfgul comments for ASD
03/04/14   sw      Antenna Switch Diversity updates
02/24/14   sc      Add therm read abort handling
01/21/14   ch      Initial checkin RFLM msgr GSM definition
===========================================================================*/
 
#ifndef RFLM_GSM_MSG_H
#define RFLM_GSM_MSG_H

/*=============================================================================

                   INCLUDE FILES

=============================================================================*/
#include "rflm_msg.h"

/*! @brief Status enum type to be set by RFLM for analysis by RFSW */
typedef enum
{
  RFLM_GSM_STATUS_DEFAULT,
  RFLM_GSM_STATUS_OK,
  RFLM_GSM_STATUS_ERROR,
  RFLM_GSM_STATUS_THERM_READ_ABORTED,
  RFLM_GSM_STATUS_THERM_READ_TIMEOUT,
  RFLM_GSM_STATUS_MAX
}rflm_gsm_resp_status;

/*! @brief Asynchronous thermistor read response message

*/
typedef struct
{
	msgr_hdr_s   hdr;               //!< MSGR header
    rflm_gsm_resp_status status;    //!< Status of therm read                              
	uint32       payload;           //!< Generic payload
} rflm_gsm_therm_read_rsp_t;


/*! @brief Asynchronous asd read response message
*/
typedef struct
{
	msgr_hdr_s   hdr;               //!< MSGR header
    rflm_gsm_resp_status status;    //!< Status of therm read                              
	uint32       payload;           //!< Generic payload
} rflm_gsm_asd_rsp_t;

/*! @brief Asynchronous device status read response message
*/
typedef struct
{
  msgr_hdr_s  hdr;                 //!< MSGR header
  rflm_gsm_resp_status  status;    //!< Status of therm read                              
  uint32  payload;                 //!< Generic payload
} rflm_gsm_device_status_read_rsp_t;


/* SUB1 IDs*/
#define RFLM_GSM_MSG_ID_THERM_READ_RSP  0x00
#define RFLM_GSM_MSG_ID_ASD_RSP         0x01
#define RFLM_GSM_MSG_ID_DEVICE_STATUS_READ_RSP  0x02 


/* SUB2 IDs*/
#define RFLM_GSM_MSG_ID_ASD_RSP_SUB2    0x10
#define RFLM_GSM_MSG_ID_THERM_READ_RSP_SUB2 0x20
#define RFLM_GSM_MSG_ID_DEVICE_STATUS_READ_RSP_SUB2  0x30 


/* SUB3 IDs*/
#define RFLM_GSM_MSG_ID_ASD_RSP_SUB3                 0x40

/*! @brief     */
enum
{
  /*! RFLM uses this message to send async commands to RF TASK */
  MSGR_DEFINE_UMID( RFLM, GSM, RSP, THERM_READ,  RFLM_GSM_MSG_ID_THERM_READ_RSP, rflm_gsm_therm_read_rsp_t),  
  MSGR_DEFINE_UMID( RFLM, GSM, RSP, ASD,  RFLM_GSM_MSG_ID_ASD_RSP, rflm_gsm_asd_rsp_t),  
  MSGR_DEFINE_UMID( RFLM, GSM, RSP, DEVICE_STATUS_READ,  RFLM_GSM_MSG_ID_DEVICE_STATUS_READ_RSP, rflm_gsm_device_status_read_rsp_t),
};

/* Unlike ASD, Therm Read response does not use different UMIDs for different subscriptions */
/* Instead, for therm read, RFLM always sends the same UMID back to the RF apps task */
/* The RF apps task then does the appropriate processing (based on subscription) */
/*! @brief     */
enum
{
  /*! RFLM uses this message to send async commands to RF TASK */
  MSGR_DEFINE_UMID( RFLM, GSM, RSP, THERM_READ_SUB2,  RFLM_GSM_MSG_ID_THERM_READ_RSP_SUB2, rflm_gsm_therm_read_rsp_t),  
  MSGR_DEFINE_UMID( RFLM, GSM, RSP, ASD_SUB2,  RFLM_GSM_MSG_ID_ASD_RSP_SUB2, rflm_gsm_asd_rsp_t),  
  MSGR_DEFINE_UMID( RFLM, GSM, RSP, DEVICE_STATUS_READ_SUB2,  RFLM_GSM_MSG_ID_DEVICE_STATUS_READ_RSP_SUB2, rflm_gsm_device_status_read_rsp_t), 
};


enum
{
  /*! RFLM uses this message to send async commands to RF TASK */
  MSGR_DEFINE_UMID( RFLM, GSM, RSP, ASD_SUB3,  RFLM_GSM_MSG_ID_ASD_RSP_SUB3, rflm_gsm_asd_rsp_t),  
};

#endif /*RFLM_GSM_MSG_H*/
