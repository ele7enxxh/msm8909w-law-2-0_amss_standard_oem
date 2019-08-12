/*!
  @file mmal_ciq_gsm_metric.c

  @brief

  @detail

*/

/*==============================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

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

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/mmal/src/mmal_ciq_gsm_metric.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------- 
11/15/11   sk      Handle the dl_reconnect_req primitive as part of the CFA parsing. 
                   Fix for CR 318906
11/11/11   sk      handle CFA log packet for gprs mssags Fix for CR 317155
09/08/11   sk      Delete the DSM items when the payload is 0. Fixes CR 305535
08/18/11   sk      process the CFA log packet to get information about all 
                   signalling message flow b/w RR->GL2
06/27/11   sk       Fix the channel type of the RR message RR messages following 
                   immediate assignment    
06/23/11   sk     1. Fix order of the RACH, Immediate assignment, 
                  2. Fix the NULL content for the RACH messages
				          3. Fix the channel type for the NAS messages
06/10/11   sk     Fixes for GS30/31
05/05/11   sk     Modified the stats structure
05/04/11   sk     Explicitly include the file mmal_util.h
03/28/11   sk     Initial Checkin
           
==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/
#include "mcs_variation.h"
#include <customer.h>
#include "mmal_cfg.h"
#include "mmal_lte_metrics.h"

#ifdef FEATURE_MMAL_CIQ

  #include <stdio.h>
  #include <string.h>         // For memcpy()
  #include <bit.h>
  #include <log_codes_gsm.h>
  #include <log.h>            // For log_commit() and log_alloc() and log codes
  #include <event.h>          // For logging RAR event and event codes
  #include <IQMetricsGS.h>
  #include <dsm.h>
  #include <dsmutil.h>
  #include <mmal_ciq_metrics.h>
  #include <diag.h>
  #include <mmal_util.h>
/*==============================================================================

                   INTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*==============================================================================

INTERNAL FUNCTION PROTOTYPES

==============================================================================*/
#define MMAL_CIQ_GSM_METRIC_RR_MSG_SIZE 251

typedef struct
{
  dsm_item_type *gs30_ptr;
  dsm_item_type *gs31_ptr;
  uint8 NECI_bit;
  iq_metric_GS30_t gs30_metric;
  iq_metric_GS31_t gs31_metric;
  uint8 scratch_pad[20];
  uint8 msg[MMAL_CIQ_GSM_METRIC_RR_MSG_SIZE];
  iq_channel_t   channel;
}mmal_ciq_gsm_metric_s ;

mmal_ciq_gsm_metric_s ciq_gsm_metric = {0,0,0,0,0, IQ_CHANNEL_NOT_APPLICABLE};

/*==============================================================================

FUNCTIONS

==============================================================================*/

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_submit_gs30

===========================================================================*/
/*!
@brief

This function will be submitted only when both the NAS and the WCDMA info is
populated in the metric. The flag is reset once the metric is submitted.
@detail


@return


@Note
Metric GS30 - RR 3 Downlink Message


*/
/*=========================================================================*/
void mmal_ciq_gsm_metric_submit_gs30( void )
{
  iq_iovec_t iov[3];

  iov[0].data = &ciq_gsm_metric.gs30_metric;
  iov[0].len = sizeof(ciq_gsm_metric.gs30_metric);

  iov[1].len = 0;
  iov[1].data=NULL;

  if (ciq_gsm_metric.gs30_ptr == NULL)
  {
    iov[1].data = NULL; // terminate the iq_iovec_t
    iov[1].len = 0; // belt-and-suspenders, as either one set to NULL/0 will do
  }
  else
  {

    if ((ciq_gsm_metric.gs30_ptr->used <= MMAL_CIQ_GSM_METRIC_RR_MSG_SIZE) && 
	    (ciq_gsm_metric.gs30_ptr->used > 0))
    {
       iov[1].data=ciq_gsm_metric.gs30_ptr->data_ptr;
       iov[1].len =ciq_gsm_metric.gs30_ptr->used;
    }
    else
    {
      iov[1].data = NULL; // terminate the iq_iovec_t
      iov[1].len = 0; //belt-and-suspenders, as either one set to NULL/0 will do
    }
    iov[2].data = NULL; // terminate the iq_iovec_t
    iov[2].len = 0; //belt-and-suspenders, as either one set to NULL/0 will do
  }

  /*! Call the RRC utility function to pase the RRC encoded bit string to obtain 
  the   RRC message type, transaction id and the channel type*/
  /*! Send the metric */
  IQ_SubmitMetricV(IQ_METRIC_ID_GS30, iov);

  MSG_HIGH("GS30 - DL msg submitted",0,0,0);

}

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_submit_gs31

===========================================================================*/
/*!
@brief

This function will be submitted only when both the NAS and the WCDMA info is
populated in the metric. The flag is reset once the metric is submitted.
@detail


@return


@Note
Metric GS30 - RR 3 Downlink Message


*/
/*=========================================================================*/
void mmal_ciq_gsm_metric_submit_gs31( void )
{
  iq_iovec_t iov[3];

  iov[0].data = &ciq_gsm_metric.gs31_metric;
  iov[0].len = sizeof(ciq_gsm_metric.gs31_metric);

  iov[1].len = 0;
  iov[1].data=NULL;

  if (ciq_gsm_metric.gs31_ptr == NULL)
  {
    iov[1].data = NULL; // terminate the iq_iovec_t
    iov[1].len = 0; // belt-and-suspenders, as either one set to NULL/0 will do
  }
  else
  {
    if ((ciq_gsm_metric.gs31_ptr->used <= MMAL_CIQ_GSM_METRIC_RR_MSG_SIZE) && 
	    (ciq_gsm_metric.gs31_ptr->used > 0))
    {
       iov[1].data=ciq_gsm_metric.gs31_ptr->data_ptr;
       iov[1].len =ciq_gsm_metric.gs31_ptr->used;
    }
    else
    {
      iov[1].data = NULL; // terminate the iq_iovec_t
      iov[1].len = 0; // belt-and-suspenders, as either one set to NULL/0 will do
    }
    iov[2].data = NULL; // terminate the iq_iovec_t
    iov[2].len = 0; // belt-and-suspenders, as either one set to NULL/0 will do
  }

  /*! Call the RRC utility function to pase the RRC encoded bit string to obtain
  the  RRC message type, transaction id and the channel type*/
  /*! Send the metric */
  IQ_SubmitMetricV(IQ_METRIC_ID_GS31, iov);

}

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_pack_and_submit_gs30

===========================================================================*/
/*!
@brief

This function intercepts the 0x512F GSM log packet and packages into the GS30 metric
@detail

@return

@Note 
Message type as provided in the Spec 24.008
*/
/*=========================================================================*/
void mmal_ciq_gsm_metric_pack_and_submit_gs30 
(
  dsm_item_type *dsm_log_ptr
)
{
  uint8 trid_proto, index=0;
  uint8 *log_ptr=NULL;
  uint16 read_len , bytes_read;
  uint8 message_type, channel_type, message_len;

  /*uint8 hdr_size = sizeof(log_hdr_type);
  index += hdr_size;*/
  MMAL_ASSERT(dsm_log_ptr!=NULL);

  read_len = sizeof(log_hdr_type);

  bytes_read = dsmi_pullup( &dsm_log_ptr, 
  ciq_gsm_metric.scratch_pad, 
  read_len,
  __FILENAME__,
  __LINE__ );

  MMAL_ASSERT(bytes_read == read_len);

  /*! Advance the pointer to the first byte after the log hdr field */
  //log_ptr = dsm_log_ptr->data_ptr;

  //ciq_gsm_metric.gs30_ptr = dsm_log_ptr;

  //Unpack 8bits starting at the offset
  channel_type = dsmi_pull8(&dsm_log_ptr, __FILENAME__, __LINE__); 
  //log_ptr[index++];//b_unpackb(log_ptr, hdr_size, 8);

  /* if it is a downlink message */
  if (channel_type & 0x80)
  {
    channel_type = channel_type & 0x7F; //remove the msb bit setting of 1
    message_type = dsmi_pull8(&dsm_log_ptr, __FILENAME__, __LINE__);

    /*! 
    For the transaction identifier field 
    10.3.2 Transaction identifier
    Bits 5 to 8 of the first octet of every message belonging to the 
    protocols "Call Control; call related SS messages" and
    "Session Management"contain the transaction identifier (TI). The 
    transaction identifier and its use are defined in 3GPP
    TS 24.007 [20].
    */
    /* If the message is either a SIB6, SIB5 or a SIB5 ter sent on 
    SACCH the L2 pseudo length    field should be present */
    if ((channel_type == 0x4) && ((message_type == 29) || 
      (message_type == 30) || (message_type == 6) || (message_type == 5)))
    {
      log_ptr = dsm_log_ptr->data_ptr;
      message_len = log_ptr[index];
      log_ptr[index] = ((message_len << 2) + 1);
      index++;
    }
    else
    {
      message_len = dsmi_pull8(&dsm_log_ptr, __FILENAME__, __LINE__); 
      log_ptr = dsm_log_ptr->data_ptr;
    }

    /* CCCH or BCCH index - 2*/
    if ((channel_type == 0x3) || (channel_type == 0x1))
    {
      index++;
    }
    trid_proto = log_ptr[index++] ; //b_unpackb(log_ptr, hdr_size+2*8, 8);

    /*Bits 1 to 4 of the first octet of a standard L3 message contain the protocol
    discriminator (PD) information element. The
    PD identifies the L3 protocol to which the standard layer 3 message belongs.
    The correspondence between L3 protocols
    and PDs is one-to-one */
    ciq_gsm_metric.gs30_metric.ucProtocol = ((trid_proto & 0x0F));

    /* Call Control or session management */
    if ((ciq_gsm_metric.gs30_metric.ucProtocol == 0x3) ||
        (ciq_gsm_metric.gs30_metric.ucProtocol == 0xA))
    {
      ciq_gsm_metric.gs30_metric.ucTransactionID = ((trid_proto & 0xF0) >> 4);
    }
    else
    {
      ciq_gsm_metric.gs30_metric.ucTransactionID = 0xFF;
    }

    /* Copy the message type 
    The layer 3 message type, e.g.
    IMMEDIATE ASSIGNMENT, CHANNEL
    RELEASE, etc
    Values defined as 3GPP TS 24.008/10.4
    and 3GPP TS 44.018/10.4.
    */

    if ((message_type == 0x80) && (channel_type == 0x4))
    {
      ciq_gsm_metric.gs30_metric.ucMessageType = ((log_ptr[index]&0x7c)>>2);
    }
    else
    {
      ciq_gsm_metric.gs30_metric.ucMessageType = message_type;
    }

    /*#define DCCH 0
    #define BCCH 1
    #define L2_RACH 2
    #define CCCH 3
    #define SACCH 4
    #define SDCCH 5
    #define FACCH_F 6
    #define FACCH_H 7
    #define L2_RACH_WITH_NO_DELAY 8
    */

    /* Map the logged channel type to the IQ specific channel type */
    switch (channel_type)
    {
    case 0: //DCCH:
    ciq_gsm_metric.gs30_metric.ucChannel = ciq_gsm_metric.channel; //IQ_CHANNEL_SDCCH;
    break;
    case 1: //BCCH:
    ciq_gsm_metric.gs30_metric.ucChannel = IQ_CHANNEL_BCCH;
    break;
    case 2: //L2_RACH:
    ciq_gsm_metric.gs30_metric.ucChannel = IQ_CHANNEL_RACH;
    break;
    case 3: //CCCH:
    {
      switch (message_type)
      {
    /*IMMEDIATE_ASSIGNMENT (0x3f)
    IMMEDIATE_ASSIGNMENT_EXTENDED (0x39)
    IMMEDIATE_ASSIGNMENT_REJECT (0x3A)
    */
      case 0x3F:
      case 0x39:
      case 0x3A:
	      ciq_gsm_metric.gs30_metric.ucChannel = IQ_CHANNEL_AGCH;
	      break;
      /*
    PAGING_REQUEST_TYPE_1 (0x21)
    PAGING_REQUEST_TYPE_2 (0x22)
    PAGING_REQUEST_TYPE_3 (0x24
    */
      case 0x21:
      case 0x22:
      case 0x24:
          ciq_gsm_metric.gs30_metric.ucChannel = IQ_CHANNEL_PCH;
	      break;

      }

    }
    break;
    case 4: //SACCH:
    ciq_gsm_metric.gs30_metric.ucChannel = IQ_CHANNEL_SACCH;
    break;
    case 5: //SDCCH
    ciq_gsm_metric.gs30_metric.ucChannel = ciq_gsm_metric.channel; //IQ_CHANNEL_SDCCH;
    break;
    case 6: //FACCH_F:
    ciq_gsm_metric.gs30_metric.ucChannel = ciq_gsm_metric.channel; //IQ_CHANNEL_FACCH;
    break;
    case 7: //FACCH_H:
    ciq_gsm_metric.gs30_metric.ucChannel = ciq_gsm_metric.channel; //IQ_CHANNEL_FACCH;
    break;
  }
  /*! Copy the message over */
  ciq_gsm_metric.gs30_ptr = dsm_log_ptr;
  /*! Send the metric */
  mmal_ciq_gsm_metric_submit_gs30();
  } //if its a DL msg 
}

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_pack_gs31

===========================================================================*/
/*!
@brief

This function intercepts the 0x512F GSM log packet and packages into the GS31 metric

@detail

@return

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)

// Indicates if this was sent to or from the UE 
uint8 nas_message_direction;

// Length of the NAS ota message 
uint32 nas_message_length;

// NAS ota Message 
uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END
@Note 

=========================================================================*/
void mmal_ciq_gsm_metric_pack_gs31(dsm_item_type **dsm_log_ptr)
{
  uint8 *byte_ptr;
  uint8 msg_type;
  uint8 trid_proto;

  MMAL_ASSERT(dsm_log_ptr!=NULL);

  byte_ptr = (*dsm_log_ptr)->data_ptr;
  trid_proto = byte_ptr[0]; //dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);
  msg_type = byte_ptr[1]; //dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);

  ciq_gsm_metric.gs31_metric.ucProtocol = (trid_proto &0xF);

  if (ciq_gsm_metric.gs31_metric.ucProtocol == 10)
  {
    ciq_gsm_metric.gs31_metric.ucMessageType = (msg_type);
  }
  else
  {
	  ciq_gsm_metric.gs31_metric.ucMessageType = (msg_type & 0x3F);
  }
  
  ciq_gsm_metric.gs31_ptr = *dsm_log_ptr;

  if (ciq_gsm_metric.gs31_metric.ucProtocol == 0x3) // Call control or session management (TBD)
  {
	  ciq_gsm_metric.gs31_metric.ucSendSeqNum = ((msg_type & 0xC0) >> 6);
	  ciq_gsm_metric.gs31_metric.ucTransactionID = ((trid_proto & 0xF0) >> 4);
  }
  else
  {
    ciq_gsm_metric.gs31_metric.ucSendSeqNum = 0xFF;
	  ciq_gsm_metric.gs31_metric.ucTransactionID = 0xFF;
  }

  ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_NOT_APPLICABLE;

  /*if (ciq_gsm_metric.gs31_metric.ucProtocol != 0x8) //GMM_MESSAGES
  {
   ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_FACCH;
  }
  else*/
  {
    ciq_gsm_metric.gs31_metric.ucChannel = ciq_gsm_metric.channel;
  }

  mmal_ciq_gsm_metric_submit_gs31();
}

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_pack_gs30

===========================================================================*/
/*!
@brief

This function intercepts the 0x512F GSM log packet and packages into the GS31 metric

@detail

@return

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)

// Indicates if this was sent to or from the UE 
uint8 nas_message_direction;

// Length of the NAS ota message 
uint32 nas_message_length;

// NAS ota Message 
uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END
@Note 

=========================================================================*/
void mmal_ciq_gsm_metric_pack_gs30(dsm_item_type **dsm_log_ptr)
{
  uint8 *byte_ptr;
  uint8 msg_type;
  uint8 trid_proto;

  MMAL_ASSERT(dsm_log_ptr!=NULL);
  MMAL_ASSERT(*dsm_log_ptr!=NULL);

  byte_ptr = (*dsm_log_ptr)->data_ptr;

  trid_proto = byte_ptr[0]; //dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);
  msg_type = byte_ptr[1]; //dsmi_pull8(dsm_log_ptr, __FILENAME__, __LINE__);

  ciq_gsm_metric.gs30_metric.ucProtocol = (trid_proto &0xF);

  if (ciq_gsm_metric.gs30_metric.ucProtocol == 10)
  {
    ciq_gsm_metric.gs30_metric.ucMessageType = (msg_type);
  }
  else
  {
	  ciq_gsm_metric.gs30_metric.ucMessageType = (msg_type & 0x3F);
  }

  /* Only the call control and the session mgmt messages have the
  transaction id field */
  if ((ciq_gsm_metric.gs30_metric.ucProtocol == 0x3) ||
  (ciq_gsm_metric.gs30_metric.ucProtocol == 0xA))
  {
    ciq_gsm_metric.gs30_metric.ucTransactionID = ((trid_proto & 0xF0) >> 4);
  }
  else
  {
    ciq_gsm_metric.gs30_metric.ucTransactionID = 0xFF;
  }

  ciq_gsm_metric.gs30_ptr = *dsm_log_ptr;

  /*if (ciq_gsm_metric.gs30_metric.ucProtocol != 0x8) //GMM_MESSAGES
  {
   ciq_gsm_metric.gs30_metric.ucChannel = IQ_CHANNEL_FACCH;
  }
  else*/
  {
   ciq_gsm_metric.gs30_metric.ucChannel = ciq_gsm_metric.channel;
  }

  mmal_ciq_gsm_metric_submit_gs30();
}


/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_pack_and_submit_gs31

===========================================================================*/
/*!
@brief

This function intercepts the 0x512F GSM log packet and packages into the GS31 metric

@detail

@return

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)

// Indicates if this was sent to or from the UE 
uint8 nas_message_direction;

// Length of the NAS ota message 
uint32 nas_message_length;

// NAS ota Message 
uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END
channel_type
direction
message_type;
message_len
msg in hex - 
@Note 

=========================================================================*/
void mmal_ciq_gsm_metric_pack_and_submit_gs31
(
dsm_item_type *dsm_log_ptr
)
{
  iq_metric_GS31_t gs31_metric;
  uint8 index=0;
  uint8 *log_ptr=NULL;
  uint8 message_type, channel_type, message_len;
  uint8 trid_proto;
  uint16 read_len;
  uint16 bytes_read;

  gs31_metric.ucTransactionID = 0xFF;

  //uint8 hdr_size = sizeof(log_hdr_type);
  //index += hdr_size;
  //MMAL_ASSERT(dsm_log_ptr!=NULL);

  /*Skip past the header */
  /* Read the log header and the first 4 bytes - discard them */
  read_len = sizeof(log_hdr_type);

  bytes_read = dsmi_pullup( &dsm_log_ptr, 
  ciq_gsm_metric.scratch_pad, read_len,__FILENAME__,__LINE__ );

  MMAL_ASSERT(bytes_read == read_len);

  /*! Advance the pointer to the first byte after the log hdr field */
  //log_ptr = dsm_log_ptr->data_ptr;

  //ciq_gsm_metric.gs31_ptr = dsm_log_ptr;

  //Unpack 8bits starting at the offset
  channel_type = dsmi_pull8(&dsm_log_ptr, __FILENAME__, __LINE__); 


  /* if it is a downlink message */
  if (!(channel_type & 0x80))
  {
    channel_type = channel_type & 0x7F; //remove the msb bit setting of 1

    message_type = dsmi_pull8(&dsm_log_ptr, __FILENAME__, __LINE__); 

    message_len = dsmi_pull8(&dsm_log_ptr, __FILENAME__, __LINE__); 


    /*! Advance the pointer to the first byte after the log hdr field */
    log_ptr = dsm_log_ptr->data_ptr;

    /* CCCH or BCCH index - 2*/
    if ((channel_type == 0x3) || (channel_type == 0x1))
    {
      index++;
    }

    /*! 
    For the transaction identifier field 
    10.3.2 Transaction identifier
    Bits 5 to 8 of the first octet of every message belonging to the protocols 
    "Call Control; call related SS messages" and
    "Session Management"contain the transaction identifier (TI). The transaction 
    identifier and its use are defined in 3GPP
    TS 24.007 [20].
    */
    trid_proto = log_ptr[index++] ; //b_unpackb(log_ptr, hdr_size+2*8, 8);


    /*Bits 1 to 4 of the first octet of a standard L3 message contain the protocol
    discriminator (PD) information element. The
    PD identifies the L3 protocol to which the standard layer 3 message belongs.
    correspondence between L3 protocols
    and PDs is one-to-one */
    ciq_gsm_metric.gs31_metric.ucProtocol = (trid_proto & 0x0F);
    /* Copy the message type 
    The layer 3 message type, e.g.
    IMMEDIATE ASSIGNMENT, CHANNEL
    RELEASE, etc
    Values defined as 3GPP TS 24.008/10.4
    and 3GPP TS 44.018/10.4.
    */
    /* For 0x80 - measurement information and channel type == SACCH,
    its a short header  only 5 bits */
    if ((message_type == 0x80 )&& (channel_type == 0x4 ))
    {
      ciq_gsm_metric.gs31_metric.ucMessageType = ((log_ptr[index] &0x7c )>>2);
    }
    else
    {
      ciq_gsm_metric.gs31_metric.ucMessageType = message_type;
    }

    /*
    When the radio connection started with a core network node of R'99 or later,
    bits 7 and 8 are reserved for the send
    sequence number in messages sent from the mobile station. In messages sent
    from the network, bits 7 and 8 are coded with a "0". See 3GPP TS 24.007 [20].
    Only call control and the session mgmt messages have the protocol field
    */
    if ((ciq_gsm_metric.gs31_metric.ucProtocol == 0x6) ||
    (ciq_gsm_metric.gs31_metric.ucProtocol == 0xA))
    {
      ciq_gsm_metric.gs31_metric.ucTransactionID = 0xFF;
      ciq_gsm_metric.gs31_metric.ucSendSeqNum = 0xFF;
    }
    else
    {
      ciq_gsm_metric.gs31_metric.ucTransactionID = ((trid_proto & 0xF0) >> 4);
      ciq_gsm_metric.gs31_metric.ucSendSeqNum = ((message_type & 0xC0) >> 6);
    }

    /* Map the logged channel type to the IQ specific channel type */
    switch (channel_type)
    {
      case 0: //DCCH:
      ciq_gsm_metric.gs31_metric.ucChannel = ciq_gsm_metric.channel; //IQ_CHANNEL_SDCCH;
      break;
      case 1: //BCCH:
      ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_BCCH;
      break;
      case 2: //L2_RACH:
      ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_RACH;
      break;
      case 3: //CCCH:
      {
        switch (message_type)
        {
      /*IMMEDIATE_ASSIGNMENT (0x3f)
      IMMEDIATE_ASSIGNMENT_EXTENDED (0x39)
      IMMEDIATE_ASSIGNMENT_REJECT (0x3A)
      */
        case 0x3F:
        case 0x39:
        case 0x3A:
	        ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_AGCH;
	        break;
        /*
      PAGING_REQUEST_TYPE_1 (0x21)
      PAGING_REQUEST_TYPE_2 (0x22)
      PAGING_REQUEST_TYPE_3 (0x24
      */
        case 0x21:
        case 0x22:
        case 0x24:
            ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_PCH;
	        break;
        }
      }
      break;
      case 4: //SACCH:
      ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_SACCH;
      break;
      case 5: //SDCCH
      ciq_gsm_metric.gs31_metric.ucChannel = ciq_gsm_metric.channel; //IQ_CHANNEL_SDCCH;
      break;
      case 6: //FACCH_F:
      ciq_gsm_metric.gs31_metric.ucChannel = ciq_gsm_metric.channel; //IQ_CHANNEL_FACCH;
      break;
      case 7: //FACCH_H:
      ciq_gsm_metric.gs31_metric.ucChannel = ciq_gsm_metric.channel; //IQ_CHANNEL_FACCH;
      break;
    }
    ciq_gsm_metric.gs31_ptr = dsm_log_ptr;
    /*! Send the metric */
    mmal_ciq_gsm_metric_submit_gs31();
  }
}

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_pack_and_submit_rach_gs31

===========================================================================*/
/*!
@brief

This function intercepts the 
event EVENT_RANDOM_ACCESS_REQUEST and sends the 
GS31 metric with fields as requested by CIQ
NECI bit is obtained from
LOG_GSM_RR_CELL_RESELECTION_PARAMS_C
@detail

@return


@Note 

=========================================================================*/
void mmal_ciq_gsm_metric_pack_and_submit_rach_gs31
(
  dsm_item_type *dsm_log_ptr
)
{
  /*! We dont even need to parse this message */
  MMAL_ASSERT(dsm_log_ptr);

  ciq_gsm_metric.gs31_metric.ucTransactionID = 0xFF;
  ciq_gsm_metric.gs31_metric.ucProtocol = IQ_PROT_DISCR_RR;
  ciq_gsm_metric.gs31_metric.ucSendSeqNum = 0xFF;
  ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_RACH;

  /*NECI bit from log packet 0x5130
  which is called LOG_GSM_RR_CELL_RESELECTION_PARAMS_C*/

  ciq_gsm_metric.gs31_metric.ucMessageType = ciq_gsm_metric.NECI_bit;

  /* Trim the packet to size 1*/
  dsmi_trim_packet(&dsm_log_ptr,1, __FILENAME__,__LINE__);

  ciq_gsm_metric.gs31_ptr = dsm_log_ptr;

  /*! Send the metric */
  mmal_ciq_gsm_metric_submit_gs31();

  MSG_LOW("MMAL RACH message submitted",0,0,0);

  //mmal_ciq_metrics_free_item(&dsm_log_ptr,__FILENAME__, __LINE__);

} /*mmal_ciq_gsm_metric_extract_neci_bit*/

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_extract_neci_bit

===========================================================================*/
/*!
@brief

This function intercepts the 
event EVENT_RANDOM_ACCESS_REQUEST and sends the 
GS31 metric with fields as requested by CIQ
NECI bit is obtained from
LOG_GSM_RR_CELL_RESELECTION_PARAMS_C
@detail

@return


@Note 

LOG_RECORD_DEFINE(LOG_GSM_RR_CELL_RESELECTION_PARAMS_C)
byte CELL_RESELECT_HYSTERESIS;
byte MS_TXPWR_MAX_CCH;
byte RXLEV_ACCESS_MIN;
byte power_offset_valid;
byte POWER_OFFSET;
byte NECI;
byte ACS;
byte CELL_RESELECT_PARAM_IND;
byte CELL_BAR_QUALIFY;
byte CELL_RESELECT_OFFSET;
byte TEMPORARY_OFFSET;
byte PENALTY_TIME;
LOG_RECORD_END

=========================================================================*/
void mmal_ciq_gsm_metric_extract_neci_bit
(
dsm_item_type *log_payload_ptr
)
{
  uint16 read_len;
  uint16 bytes_read;

  MMAL_ASSERT(log_payload_ptr);

  /*Skip past the header */
  /* Read the log header and the first 4 bytes - discard them */
  read_len = sizeof(log_hdr_type) + 5;

  bytes_read = dsmi_pullup( &log_payload_ptr, 
  ciq_gsm_metric.scratch_pad, 
  read_len,
  __FILENAME__,
  __LINE__ );

  MMAL_ASSERT(bytes_read == read_len);

  ciq_gsm_metric.NECI_bit = dsmi_pull8( 
  &log_payload_ptr, 
  __FILENAME__,
  __LINE__ );

  mmal_ciq_metrics_free_item(&log_payload_ptr,__FILENAME__, __LINE__);

}
/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_process_l3_msg

===========================================================================*/
/*!
@brief


@detail

@return


@Note 

=========================================================================*/
void mmal_ciq_gsm_metric_process_l3_msg
(
  dsm_item_type *dsm_log_ptr
)
{
	uint8 index=0;
	uint8 *log_ptr=NULL;
	uint8 trid_proto;

  MMAL_ASSERT(dsm_log_ptr);

	/*! Advance the pointer to the first byte after the log hdr field */
	log_ptr = dsm_log_ptr->data_ptr;

	/*! 
	For the transaction identifier field 
	10.3.2 Transaction identifier
	Bits 5 to 8 of the first octet of every message belonging to the protocols 
	"Call Control; call related SS messages" and
	"Session Management"contain the transaction identifier (TI). The transaction 
	identifier and its use are defined in 3GPP
	TS 24.007 [20].
	*/
	trid_proto = log_ptr[index++] ; //b_unpackb(log_ptr, hdr_size+2*8, 8);


	/*Bits 1 to 4 of the first octet of a standard L3 message contain the protocol
	discriminator (PD) information element. The
	PD identifies the L3 protocol to which the standard layer 3 message belongs.
	correspondence between L3 protocols
	and PDs is one-to-one */
	ciq_gsm_metric.gs31_metric.ucProtocol = (trid_proto & 0x0F);

	/* Copy the message type 
	The layer 3 message type, e.g.
	IMMEDIATE ASSIGNMENT, CHANNEL
	RELEASE, etc
	Values defined as 3GPP TS 24.008/10.4
	and 3GPP TS 44.018/10.4.
	*/
	/* For 0x80 - measurement information and channel type == SACCH, its a short header 
	only 5 bits */
	if (ciq_gsm_metric.gs31_metric.ucChannel == IQ_CHANNEL_SACCH )
	{
	  ciq_gsm_metric.gs31_metric.ucMessageType = ((log_ptr[index] & 0x1F ));
	}
	else
	{
    if ((ciq_gsm_metric.gs31_metric.ucProtocol == 10) ||
      (ciq_gsm_metric.gs31_metric.ucProtocol == 0x6)) //RR
    {
      ciq_gsm_metric.gs31_metric.ucMessageType = (log_ptr[index]);
    }
    else  //GSM MOB, GSM CC
    {
	    ciq_gsm_metric.gs31_metric.ucMessageType = (log_ptr[index] & 0x3F);
    }
    
	}

	/*
	When the radio connection started with a core network node of R'99 or later,
	bits 7 and 8 are reserved for the send
	sequence number in messages sent from the mobile station. In messages sent
	from the network, bits 7 and 8 are coded with a "0". See 3GPP TS 24.007 [20].
	Only call control and the session mgmt messages have the protocol field
	*/
	if (ciq_gsm_metric.gs31_metric.ucProtocol == 0x3)
	{
    /* Bits 5-8 of the Call control call related SS messages and session
    management contain the transaction identifier field */
    ciq_gsm_metric.gs31_metric.ucTransactionID = ((trid_proto & 0xF0) >> 4);
  }
  else //ciq_gsm_metric.gs31_metric.ucProtocol == 0x6 || 0xA
  {
    ciq_gsm_metric.gs31_metric.ucTransactionID = 0xFF;
	}

  
  if((ciq_gsm_metric.gs31_metric.ucProtocol == 0x3) ||
    (ciq_gsm_metric.gs31_metric.ucProtocol == 0x5))//MM,CC,SM
  {
    /* Bits 7,8 of the message type field contains the send sequence number */
	  ciq_gsm_metric.gs31_metric.ucSendSeqNum = 
      ((log_ptr[index]& 0xC0) >> 6);
  }
  else //For RR, GPRS
  {
    ciq_gsm_metric.gs31_metric.ucSendSeqNum = 0xFF;
  }
	
	ciq_gsm_metric.gs31_ptr = dsm_log_ptr;
	/*! Send the metric */
	mmal_ciq_gsm_metric_submit_gs31();

}

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_process_nas_msg

===========================================================================*/
/*!
@brief

This function intercepts the 0x512F GSM log packet and packages into the GS31 metric

@detail

@return

LOG_RECORD_DEFINE(LOG_UMTS_NAS_OTA_MESSAGE_LOG_PACKET_C)

// Indicates if this was sent to or from the UE 
uint8 nas_message_direction;

// Length of the NAS ota message 
uint32 nas_message_length;

// NAS ota Message 
uint8 nas_message[LOG_MAX_NAS_OTA_MESSAGE_SIZE];

LOG_RECORD_END

@Note 

=========================================================================*/
void mmal_ciq_gsm_metric_process_nas_msg
(
  uint8          dir,
  dsm_item_type **dsm_log_ptr
)
{
  MMAL_ASSERT(dsm_log_ptr != NULL); 
  MMAL_ASSERT(*dsm_log_ptr != NULL); 

  if (dir == 0) //CN_TO_UE
  {
    mmal_ciq_gsm_metric_pack_gs30(dsm_log_ptr);
    ciq_gsm_metric.gs30_ptr = NULL;
  } 
  else if (dir == 1)
  {
    // The regular 0x713A log packet should not be used for uplink, instead CFA is used */
    //mmal_ciq_gsm_metric_pack_gs31(dsm_log_ptr);
    ciq_gsm_metric.gs31_ptr = NULL;
  }
  else if (dir == 2) // Only in cases where from CFA we want to extract the NAS msg */
  {
    mmal_ciq_gsm_metric_pack_gs31(dsm_log_ptr);
    ciq_gsm_metric.gs31_ptr = NULL;
  }
  mmal_ciq_metrics_free_item(dsm_log_ptr, __FILENAME__, __LINE__);
}


/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_parse_cfa_pkt

===========================================================================*/
/*!
@brief

This function intercepts the 0x512F GSM log packet and packages into the GS31 metric

@detail

@return
#define CFA_MAX_PACKET_LENGTH (255)
LOG_RECORD_DEFINE(LOG_UMTS_CALL_FLOW_ANALYSIS_C)
  byte   Version;
  byte   Sequence;
  byte   Direction; 
  byte   Task;
  byte   Queue;
  byte   MsgSize;                           // Size sent in packet 
  byte   MsgSet;                            // Mesasge Set         
  byte   MsgId;                             // Message ID          
  word   MsgLen;                            // Actual size of message 
  byte   Message[CFA_MAX_PACKET_LENGTH-4];  // First MsgSize bytes of data 
LOG_RECORD_END

#define MS_RR_L2          17   // messages between RR and layer 2    
#define GS_QUEUE_LAYER_2   2
#define GS_QUEUE_RR        3

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8              l3_message_length;
   boolean           short_header;
   uint8              layer3_message[N201_MAX];
} dl_unit_data_req_T;
typedef struct
{
  IMH_T message_header;
  SAPI_T SAPI;
  l2_channel_type_T l2_channel_type;
  establish_mode_T establish_mode;
  uint8 l3_message_length;
  uint8 layer3_message[N201_MAX];
} dl_establish_req_T;

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8              random_access_content;
#ifdef FEATURE_GSM_EGPRS
   boolean           egprs_ra_content_present;
   uint16            egprs_ra_content;
   uint8             egprs_training_seq;
#endif 

} dl_random_access_req_T;

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8              l3_message_length;
   uint8              l3_message_number;
   uint8              layer3_message[MAX_OCTETS_L3_MESSAGE];
} dl_data_req_T;

typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   boolean           forced_resume;
   uint8              l3_message_length;
   uint8              layer3_message[N201_MAX];
   #ifdef FEATURE_GSM_DTM
   boolean           l1_already_connected;
   #endif // FEATURE_GSM_DTM 
} dl_resume_req_T;

GMM -> LLC LL service primitive message identifier definition. 

typedef enum
{
  GMMLLC_LL_UNITDATA_REQ      = 0x20,
  GMMLLC_LL_XID_REQ           = 0x21,
  GMMLLC_LL_XID_RES           = 0x22

} gmmllc_ll_prim_e_type;

 LL-UNITDATA-REQ message type definition. Unacknowledged data transmission
   request.

   NOTE: It is the sending entity's responsibility to de-allocate any heap
         memory that is linked to the l3_pdu ptr. 

typedef struct
{
  gmmllc_ll_prim_e_type   prim;           // GMMLLC_LL_UNITDATA_REQ         

  uint32                  tlli;           // Temporary Logical Link Id      
  boolean                 ciphered;       // TRUE if data is to be ciphered 
  uint8                   *l3_pdu;        // Ptr to PDU in heap alloc mem
  uint16                  l3_pdu_len;     // Len of PDU in heap alloc mem
#ifdef FEATURE_GPRS_LAYER3_TX_CONFIRM
  uint8                   protocol_disc;  // Protocol Discriminator of UL NAS
                                             signalling PDU 
  uint8                   transaction_id; // Transaction Identifier of UL NAS
                                             signalling PDU
#endif // FEATURE_GPRS_LAYER3_TX_CONFIRM
} gmmllc_ll_unitdata_req_type;

gllcmm_ll_unitdata_ind_type        ll_unitdata_ind;

#define GS_QUEUE_LLC                   34
#define GS_QUEUE_MM                   4

typedef struct
{
  IMH_T                    hdr;               // Inter-task message header             

  uint32                   tlli;              // Temporary Logical Link Identifier     
  boolean                  ciphered;          // TRUE if PDU was received ciphered     
  uint8                    *l3_pdu;           // Pointer to PDU in heap allocated mem   
  uint16                   l3_pdu_len;        // Length of PDU in heap allocated mem   
#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type   as_id;
#endif
} gllcmm_ll_unitdata_ind_type;
typedef struct
{
   IMH_T             message_header;
   SAPI_T            SAPI;
   l2_channel_type_T l2_channel_type;
   uint8              l3_message_length;
   uint8              layer3_message[N201_MAX];
} dl_reconnect_req_T;
============================================================================*/
void mmal_ciq_gsm_metric_parse_cfa_pkt
(
  dsm_item_type *log_payload_ptr
)
{
  uint16 read_len, msg_len;
  uint16 bytes_read;
  uint8 queue, msg_set, msg_id, msg_size,scrap ;
  uint8 lsb, msb;

  MMAL_ASSERT(log_payload_ptr);

  /*Skip past the header */
  /* Read the log header + 4 bytes discard them */
  read_len = sizeof(log_hdr_type)+4;

  bytes_read = dsmi_pullup( &log_payload_ptr, 
  ciq_gsm_metric.scratch_pad, 
  read_len,
  __FILENAME__,
  __LINE__ );

  MMAL_ASSERT(bytes_read == read_len);

  queue = dsmi_pull8(&log_payload_ptr, __FILENAME__, __LINE__);

  if ((queue != 0x2) && (queue != 0x22) && (queue != 0x4))
  {
    mmal_ciq_metrics_free_item(&log_payload_ptr, __FILENAME__, __LINE__);
    /* Not a message intended for L2, hence dont tap*/
    return;
  }

  msg_size = dsmi_pull8(&log_payload_ptr, __FILENAME__, __LINE__);

  msg_set = dsmi_pull8(&log_payload_ptr, __FILENAME__, __LINE__);

  if (
    ((queue == 0x2)&&(msg_set != 0x11)) || 
    ((queue == 0x22) && (msg_set != 0xCE)) ||
    ((queue == 0x4) && (msg_set != 0xCE))) /* RR->L2*/
  {
    mmal_ciq_metrics_free_item(&log_payload_ptr, __FILENAME__, __LINE__);
    return;
  }

  msg_id = dsmi_pull8(&log_payload_ptr, __FILENAME__, __LINE__);

  lsb = dsmi_pull8(&log_payload_ptr, __FILENAME__, __LINE__);

  msb = dsmi_pull8(&log_payload_ptr, __FILENAME__, __LINE__);

  msg_len = ((msb << 8 ) | lsb);

  MSG_HIGH("MMAL: GSM Parsing the CFA len 0x%x, msg_set 0x%x, msg_id 0x%x", 
    msg_len,msg_set,msg_id);
  
  if (msg_set == 0xCE)
  {
    if (queue == 0x22)
      mmal_ciq_gsm_metric_process_nas_msg(2, &log_payload_ptr); //UE=>NW only for the CFA
    else if (queue == 0x4)
      mmal_ciq_gsm_metric_process_nas_msg(0, &log_payload_ptr); //NW=>UE
    return;
  }
  
  memset(&ciq_gsm_metric.gs31_metric, 0x0, sizeof(iq_metric_GS31_t));
  
  if (msg_len > 0 )
  {
    read_len = 2;

    bytes_read = dsmi_pullup( &log_payload_ptr, 
    ciq_gsm_metric.scratch_pad, 
    read_len,
    __FILENAME__,
    __LINE__ );

    MMAL_ASSERT(bytes_read == read_len);

    switch (msg_id)
    {
	    /*MSG_ID:	DL_DATA_REQ (0x00)*/
	    /* 3rd byte gives the len , 5th byte onwards the L3 msg starts */
      case 0x0:
      msg_len = dsmi_pull8(&log_payload_ptr, __FILENAME__,__LINE__ );
      scrap = dsmi_pull8(&log_payload_ptr, __FILENAME__,__LINE__ );
      ciq_gsm_metric.gs31_metric.ucChannel = ciq_gsm_metric.channel;
      mmal_ciq_gsm_metric_process_l3_msg(log_payload_ptr);
	    break;
	    /*MSG_ID:	DL_ESTABLISH_REQ (0x01) DL_RESUME_REQ (0x05)*/
	    /* 4th byte is msg_len and 5th byte onwards L3 msg starts*/
      case 0x1:
      case 0x5:
      scrap = dsmi_pull8(&log_payload_ptr,__FILENAME__, __LINE__);
      msg_len = dsmi_pull8(&log_payload_ptr,__FILENAME__, __LINE__);
      ciq_gsm_metric.gs31_metric.ucChannel = ciq_gsm_metric.channel;
      mmal_ciq_gsm_metric_process_l3_msg(log_payload_ptr);
	    break;
      /*MSG_ID : DL_RECONNECT_REQ 0x3*/
      case 0x3:
      msg_len = dsmi_pull8(&log_payload_ptr, __FILENAME__,__LINE__ );
      ciq_gsm_metric.gs31_metric.ucChannel = ciq_gsm_metric.channel;
      mmal_ciq_gsm_metric_process_l3_msg(log_payload_ptr);
	    break;
	    /*MSG_ID:	DL_RANDOM_ACCESS_REQ (0x02)*/
	    /* 3rd byte contains the content*/
      case 0x2:
      mmal_ciq_gsm_metric_pack_and_submit_rach_gs31(log_payload_ptr);
	    break;
	    /*MSG_ID:	DL_UNIT_DATA_REQ (0x07)*/
	    /* 3rd byte is len, 5th bytes is l3 msg, only meas report goes in this*/
      case 0x7:
      msg_len = dsmi_pull8(&log_payload_ptr, __FILENAME__,__LINE__ );
      scrap = dsmi_pull8(&log_payload_ptr, __FILENAME__,__LINE__ );
      ciq_gsm_metric.gs31_metric.ucChannel = IQ_CHANNEL_SACCH;
      mmal_ciq_gsm_metric_process_l3_msg(log_payload_ptr);
  	  break;
      default:
      break;
    }
  }

  mmal_ciq_metrics_free_item(&log_payload_ptr, __FILENAME__, __LINE__);
}

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_pack_and_submit_gprs_gs30_gs31

===========================================================================*/
/*!
@brief

This function intercepts the 0x5226 GSM GPRS og packet and packages into the GS31 metric

@detail

@return

LOG_RECORD_DEFINE(LOG_GPRS_MAC_SIGNALLING_MESSAGE_C)
  byte channel_type;
  byte message_type;
  byte message_length;
  byte message[1];
LOG_RECORD_END

GS-30
unsigned char       ucTransactionID;
    unsigned char       ucProtocol;				// Use Enum: iq_prot_discr_t
    unsigned char       ucMessageType;
    unsigned char       ucChannel;	    		// Use Enum: iq_channel_t

	GS31 
	unsigned char       ucTransactionID;
    unsigned char       ucProtocol;				// Use Enum: iq_prot_discr_t
    unsigned char       ucSendSeqNum;
    unsigned char       ucMessageType;
    unsigned char       ucChannel;	   
IQ_CHANNEL_PBCCH       // GPRS Channels
    ,IQ_CHANNEL_PPCH
    ,IQ_CHANNEL_PRACH
    ,IQ_CHANNEL_PAGCH
    ,IQ_CHANNEL_PACCH
@Note 

=========================================================================*/
void mmal_ciq_gsm_metric_pack_and_submit_gprs_gs30_gs31
(
  dsm_item_type *log_payload_ptr
)
{
  uint16 read_len;
  uint16 bytes_read;
  uint8 channel_type, message_type, message_len;
  iq_channel_t channel = IQ_CHANNEL_NOT_APPLICABLE;

  MMAL_ASSERT(log_payload_ptr);

  /*Skip past the header */
  /* Read the log header - discard them */
  read_len = sizeof(log_hdr_type) ;

  bytes_read = dsmi_pullup( &log_payload_ptr, 
  ciq_gsm_metric.scratch_pad, 
  read_len,
  __FILENAME__,
  __LINE__ );

  MMAL_ASSERT(bytes_read == read_len);
    
  channel_type = dsmi_pull8( &log_payload_ptr, __FILENAME__,__LINE__ );
  message_type = dsmi_pull8( &log_payload_ptr, __FILENAME__,__LINE__ );
  message_len = dsmi_pull8( &log_payload_ptr, __FILENAME__,__LINE__ );

  switch (channel_type)
  {
    case 0x1: //PRACH_11BIT_CHANNEL: 
    channel = IQ_CHANNEL_PRACH;
    break;
    case 0x2: //PRACH_8BIT_CHANNEL: 
    channel = IQ_CHANNEL_PRACH;
    break;
    case 0x3: //PACCH_RRBP_CHANNEL:
    channel = IQ_CHANNEL_PACCH;
    break;
    case 0x4: //UL_PACCH_CHANNEL:
    channel = IQ_CHANNEL_PACCH;
    break;
    case 0x81: //PCCCH_CHANNEL:
    {
    switch (message_type)
    {
    /*IMMEDIATE_ASSIGNMENT (0x3f)
    IMMEDIATE_ASSIGNMENT_EXTENDED (0x39)
    IMMEDIATE_ASSIGNMENT_REJECT (0x3A)
    */
      case 0x3F:
      case 0x39:
      case 0x3A:
	      channel = IQ_CHANNEL_PAGCH;
	      break;
      /*
    PAGING_REQUEST_TYPE_1 (0x21)
    PAGING_REQUEST_TYPE_2 (0x22)
    PAGING_REQUEST_TYPE_3 (0x24
    */
      case 0x21:
      case 0x22:
      case 0x24:
          channel = IQ_CHANNEL_PPCH;
	      break;

      }

    }
    break;
    case 0x82: //PBCCH_CHANNEL:
    channel = IQ_CHANNEL_PBCCH;
    break;
    case 0x83: //DL_PACCH_CHANNEL:
    channel = IQ_CHANNEL_PACCH;
    break;
    case 0xFF:
    channel = IQ_CHANNEL_NOT_APPLICABLE;
    break;

  }

  /* Submit GS30 - DL */
  if ((channel_type & 0x80 ) != 0 )
  {
    ciq_gsm_metric.gs30_metric.ucTransactionID = 0xFF;
    ciq_gsm_metric.gs30_metric.ucProtocol = 0xFF;
    ciq_gsm_metric.gs30_metric.ucMessageType = message_type;
    ciq_gsm_metric.gs30_metric.ucChannel = channel;
    ciq_gsm_metric.gs30_ptr = log_payload_ptr;
    mmal_ciq_gsm_metric_submit_gs30();
    
  }
  else //GS31
  {
    ciq_gsm_metric.gs31_metric.ucTransactionID = 0xFF;
    ciq_gsm_metric.gs31_metric.ucProtocol = 0xFF;
    ciq_gsm_metric.gs31_metric.ucSendSeqNum = 0xFF;
    ciq_gsm_metric.gs31_metric.ucMessageType = message_type;
    ciq_gsm_metric.gs31_metric.ucChannel = channel;
    ciq_gsm_metric.gs31_ptr = log_payload_ptr;
    mmal_ciq_gsm_metric_submit_gs31();
  }
}

/*===========================================================================

FUNCTION: mmal_ciq_gsm_metric_get_channel_info

===========================================================================*/
/*!
@brief

This function intercepts the GSM RR Channel configuration packet
0x5135 and packages into the GS31 metric

@detail

@return

#define FULL_RATE_TRAFFIC 0
#define HALF_RATE_TRAFFIC 1
#define SDCCH_4 2
#define SDCCH_8 3


@Note 

=========================================================================*/
void mmal_ciq_gsm_metric_get_channel_info
(
  dsm_item_type *log_payload_ptr
)
{
  uint16 read_len;
  uint16 bytes_read;
  uint8 channel_type;
  MMAL_ASSERT(log_payload_ptr);

  /*Skip past the header */
  /* Read the log header - discard them */
  read_len = sizeof(log_hdr_type) + 10 ;

  bytes_read = dsmi_pullup( &log_payload_ptr, 
   ciq_gsm_metric.scratch_pad, 
	read_len,
	__FILENAME__,
	__LINE__ );

  MMAL_ASSERT(bytes_read == read_len);

  channel_type = dsmi_pull8(&log_payload_ptr, __FILENAME__, __LINE__);
  
  //FACCCH
  if ((channel_type == 0x0) || (channel_type == 0x1))
  {
    ciq_gsm_metric.channel = IQ_CHANNEL_FACCH; 
  	MSG_LOW("GS30/31 Channel = FACCH",0,0,0);
  }
  //SDCCH
  else if((channel_type == 0x2) || (channel_type == 0x3)) 
  {
    ciq_gsm_metric.channel = IQ_CHANNEL_SDCCH;
	  MSG_LOW("GS30/31 Channel = SDCCH",0,0,0);
  }
  else
  {
    ciq_gsm_metric.channel = IQ_CHANNEL_NOT_APPLICABLE;
	  MSG_LOW("GS30/31 Channel = NA",0,0,0);
  }
}


#endif /* FEATURE_MMAL*/

