#ifndef __VS_VOC_PRIVATE_IF_H__
#define __VS_VOC_PRIVATE_IF_H__

/*
   Copyright (C) 2012-2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.


   $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/private/vs_voc_private_if.h#1 $
   $Author: mplcsds1 $
*/


/* [Synchronous non-blocking API]
 *
 * This command writes a buffer to the decoder.
 *
 * The buffer does not return until the decoder consumes the buffer or due to
 * the VS_VOC_CMD_FLUSH_BUFFERS command. In all cases, a buffer is
 * returned to the client after the client receives the
 * VS_VOC_EVENT_WRITE_BUFFER_RETURNED event that notifies the specific buffer
 * that is returned.
 */

#define VS_VOC_CMD_WRITE_BUFFER_THROUGH ( 0x000130ED )


/**
 * The feature is decoder packet request event structure to MVS.
 *
 * There is no parameter for this event. 
 *  
 * Client would send VS_VOC_CMD_WRITE_BUFFER_THROUGH in response to 
 * this event. 
 */
#define VS_VOC_EVENT_WRITE_REQUEST ( 0x000130EC )


#endif /* __VS_VOC_PRIVATE_IF_H__ */

