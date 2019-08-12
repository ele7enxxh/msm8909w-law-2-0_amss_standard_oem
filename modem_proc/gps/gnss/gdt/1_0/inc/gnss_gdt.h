#ifndef GNSS_GDT_H
#define GNSS_GDT_H
/*=============================================================================

                             GDT Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for GDT (Generic Data Transfer) in GNSS module.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  

Copyright (c) 2014-2015 Qualcomm Atheros, Inc. All Rights Reserved. 
  Qualcomm Atheros Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/


/*=============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/gdt/1_0/inc/gnss_gdt.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-25   ah  Initial version
  

=============================================================================*/

/*=============================================================================

                                INCLUDE FILES

=============================================================================*/

#include "customer.h"
#include "comdef.h"
#include "rfs_types.h"
#include "rfs_api.h"
#include "rfs_config.h"
#include "rfs_errno.h"
#include "rfs_fcntl.h"
#include "log.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/*=============================================================================

       Constants

=============================================================================*/


/*=============================================================================

       Macros

=============================================================================*/

#define C_GDT_BIG_ENDIAN        0  /* big endian architecture */
#define C_GDT_LITTLE_ENDIAN     1  /* little endian architecture */

#define C_GDT_BLOB_MAX_DATA_SIZE (1000)
#define C_GDT_BLOB_FORMAT_BINARY (0)
#define C_GDT_BLOB_FORMAT_ASCII  (1)

/*=============================================================================

       Typedefs

=============================================================================*/
typedef unsigned char* gdt_MemHandle;

typedef enum
{
  e_GDT_SUCCESS = 0,
  e_GDT_FAIL,
  e_GDT_MAX = 0xFFFF
} e_GDTReturnCodesVal;
typedef uint16 e_GDTReturnCodesType;

/* Defines the service which uses GDT */
typedef enum
{
  e_GDT_SERVICE_INVALID = -1,
  e_GDT_SERVICE_GTP_WWAN = 0,
  /* Update last service */
  e_GDT_SERVICE_LAST = e_GDT_SERVICE_GTP_WWAN,
  e_GDT_SERVICE_MAX = 0xFFFF
} e_GDTServiceIDVal;
typedef int16 e_GDTServiceIDType;

typedef struct
{
  e_GDTServiceIDType e_ServiceId; 
  uint32             q_SessId; /* Session ID for this service */
} gdt_SessionType;

/* Defines the request type which a service is requesting from GDT */
typedef enum
{
  e_GDT_UPLOAD_BEGIN = 0,
  e_GDT_UPLOAD_END, 
  e_GDT_DOWNLOAD_BEGIN,
  e_GDT_DOWNLOAD_MAX = 0xFFFF
} e_GDTServiceRequestVal;
typedef uint16 e_GDTServiceRequestType;
/* Defines the response to an access request (gdt_UploadBegin()) */
typedef enum
{
  e_GDT_ACCESS_ALLOWED = 0,       /* AP confirmed, that access to the shared data
                                 location is allowed */
  e_GDT_ACCESS_FAILED,        /* Any type of GDT access error */
  e_GDT_ACCESS_NOT_ALLOWED,   /* AP does not allow access to the shared data
                                 location */
  e_GDT_ACCESS_MAX = 0xFFFF
} e_GDTResponseVal;
typedef uint16 e_GDTResponseType;

/* Defines the status of a record which was sent to GDT for upload */
typedef enum
{
  e_GDT_ACK_SUCCESS = 0, /* The sent data is accepeted by GDT */
  e_GDT_ACK_FAILED,  /* The sent data was not accepted by GDT */
  e_GDT_ACK_MAX = 0xFFFF
} e_GDTAckVal;
typedef uint16 e_GDTAckType;

typedef struct
{
  gdt_SessionType   z_SessionInfo;
  e_GDTResponseType e_StartStatus;
  int             l_Client_handle;
  uint32          q_TransactionId;
} gdt_StartResponseType;

typedef struct
{
  gdt_SessionType       z_SessionInfo;
  e_GDTReturnCodesType  e_OpenStatus;
} gdt_OpenResponseType;

typedef struct
{
  gdt_SessionType       z_SessionInfo;
  e_GDTReturnCodesType  e_CloseStatus;
} gdt_CloseResponseType;


typedef struct
{
  gdt_SessionType z_SessionInfo;
  e_GDTAckType    e_EndStatus;
  int             l_Client_handle;
  uint32          q_TransactionId;
} gdt_EndResponseType;

typedef struct
{
  gdt_SessionType z_SessionInfo;
  e_GDTAckType    e_EndStatus;
} gdt_SendAckResponseType;

typedef struct
{
  gdt_SessionType z_SessionInfo;
  gdt_MemHandle p_ByteBuffer;
  uint32        q_DataSize;  
} gdt_SendBlobType;


/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadBegin() in this data type */
typedef union
{
  gdt_StartResponseType *pz_StartResponse;
} gdt_StartArgumentType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadSend() in this data type*/
typedef union
{
  gdt_SendAckResponseType *pz_AckResponse;  
} gdt_SendAckArgumentType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadEnd() in this data type */
typedef union 
{
  gdt_EndResponseType *pz_EndResponse;
} gdt_EndArgumentType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadOpen() in this data type */
typedef union 
{
  gdt_OpenResponseType *pz_OpenResponse;
} gdt_OpenArgumentType;

/* Every client of GDT needs to provide its required parameter for the call back API
   for gdt_UploadClose() in this data type */
typedef union 
{
  gdt_CloseResponseType *pz_CloseResponse;
} gdt_CloseArgumentType;

typedef struct 
{
  gdt_SessionType z_SessInfo;
  gdt_MemHandle   z_GdtBuffer;
  uint32          q_GdtBufferSize; /* This the total buffer size */ 
  uint32          q_MsgLength;     /* This is the number of bytes in the buffer */
  uint8           u_ArchEndian;    /* C_GDT_BIG_ENDIAN or C_GDT_LITTLE_ENDIAN*/
} gdt_InterfaceType;

/* Packed data structures */
#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(push)
#pragma pack(1)
#endif

typedef PACKED struct PACKED_POST 
{
  uint32  q_SrcTask;     /* Tthread ID of the client task */

  uint16   w_ServiceId;   /* 0 : Invalid
                             1 : GDT_SERVICE_GTP_WWAN */

  uint32  q_SessionId;  /* GDT Session ID  */

  uint16 w_ContentType; /* 0:Invalid
                           1: TDP Client information
                           2: TDP Mobile status data
                           3: TDP cell array */

  uint8  u_Format;         /* Output format:
                              0: Binary
                              1: ASCII */

  uint16 w_Part;           /* If data is larger than a single log packet it will be
                              logged in multiple log instances (PART of TOTAL_PARTS). */  
  uint16 w_TotalParts;
  uint32 q_Length;         /* Number of valid bytes in data field */
  unsigned char sz_Data[C_GDT_BLOB_MAX_DATA_SIZE];
} gdt_PackedDataBlobType;


typedef PACKED struct PACKED_POST 
{
  /* log header.  DO NOT ACCESS DIRECTLY! */
  log_hdr_type           z_Hdr;
  uint8                  u_Version;
  gdt_PackedDataBlobType z_Blob;
} gdt_DataBlobDmLogType;

#ifdef FEATURE_CGPS_PLAYBACK
#pragma pack(pop)
#endif
/*===========================================================================

                     DATA TYPE DEFINITIONS FOR MODULE

                        !!!!!!! ATTENTION !!!!!!!!
Size/Order of data fields in the following data types should not
be changed as they are synchronized with external tool software.
===========================================================================*/


/* SendBegin Event */
/* EVENT_GNSS_GDT_SESS_START_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;      
} gdt_SessStartReqEventType;

/* EVENT_GNSS_GDT_SESS_START_RESP */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint16 w_GdtResp;
  int32 l_ClientHandle;
  uint32 q_TransactionId;
} gdt_SessStartRespEventType;

/* SendData Event */
/* EVENT_GNSS_GDT_SEND_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint32 q_DataSize;
} gdt_SendReqEventType;

/* EVENT_GNSS_GDT_SEND_RESP */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint16 w_GdtResp;
} gdt_SendRespEventType;

/* SendEnd Event */
/* EVENT_GNSS_GDT_SESS_END_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint16 w_GdtResp;
} gdt_SessEndReqEventType;

/* EVENT_GNSS_GDT_SESS_END_RESP */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;
  uint16 w_GdtAckResp;
  int32 l_ClientHandle;
  uint32 q_TransactionId;
} gdt_SessEndRespEventType;

/* SendOpen Event */
/* EVENT_GNSS_GDT_SESS_OPEN_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;      
} gdt_SessOpenReqEventType;

/* SendClose Event */
/* EVENT_GNSS_GDT_SESS_CLOSE_REQ */
typedef PACKED struct PACKED_POST
{
  uint16 w_ServiceId;
  uint32 q_SessionId;      
} gdt_SessCloseReqEventType;



/*=============================================================================

       Function prototypes

=============================================================================*/


/*===========================================================================
FUNCTION gdt_Deinit

DESCRIPTION
  This function de-initializes GDT and frees up all registered buffers

  WARNING - This function should *NOT* be used on target because it can be very
            dangerous if the buffer is freed incorrectly.
 
PARAMETERS 
 
DEPENDENCIES

RETURN VALUE 

SIDE EFFECTS

===========================================================================*/
void gdt_Deinit();

/*===========================================================================
FUNCTION gdt_SendBegin

DESCRIPTION
  This function starts a data transposrt session between a module in MP and
  the corresponding module in AP.
 
PARAMETERS 
 
  pz_Session - Points to the session information
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the request, and sent the access request
                 to AP to get the permission
 
  e_GDT_FAIL: Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendBegin(gdt_SessionType * const pz_Session);

/*===========================================================================
FUNCTION gdt_SendData

DESCRIPTION
  This function receives a chunk of data that needs to be transported to AP.
  Different chuncks of data will be accumulated and will be sent to AP at once,
  when the sender calls gdp_SessionEnd().
 
PARAMETERS 
 
  cpz_GdtIface - Points to the GDT interface
  (input)

DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has accepted the data to be uploaded.
 
   Note: This does not confirm that data is transferred.
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendData(const gdt_InterfaceType *cpz_GdtIface);
                                        

/*===========================================================================
FUNCTION gdt_SendEnd

DESCRIPTION
  This function finalizes the data transmission session between MP and AP.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  e_Status - The final status of this session
  (input)

  b_ForceCleanup - If TRUE, force cleanup of remaining session information
  (input)


RETURN VALUE 
  e_GDT_SUCCESS: GDT has received the final upload session status from AP 
 
  e_GDT_FAIL  - There is an internal error in GDT
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendEnd(const gdt_SessionType * const cpz_Session, 
                                 e_GDTResponseType e_Status, boolean b_ForceCleanup);
                                 

/*===========================================================================
FUNCTION gdt_SendOpen

DESCRIPTION
  This function initializes data transmission session from a client to GDT.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly initialized to start data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendOpen(const gdt_SessionType * const cpz_Session);

/*===========================================================================
FUNCTION gdt_SendClose

DESCRIPTION
  This function finalizes data transmission session from a client to GDT.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly finalized data transmission
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_SendClose(const gdt_SessionType * const cpz_Session);

/*===========================================================================
FUNCTION gdt_BufAlloc

DESCRIPTION
  This function allocates a requested buffer from the requesting user service.  Buffer will be capped at a set
  maximum (currently 500 Kb).  At this time, each service is only allowed to hold a single buffer.  Due to this
  limitation, any subsequent calls to gdt_BufAlloc will return with an error if the service already holds a
  buffer.

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)
  
  q_ReqBufferSize - The requested buffer size in bytes
  (input)
  
  cp_ByteBuffer - Local handle to the allocated buffer
  (output)
  
  q_BufferSize - Size of the allocated buffer
  (output)


RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly allocated a buffer
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_BufAlloc(const gdt_SessionType * const cpz_Session, 
                                    uint32 q_ReqBufferSize,
                                    gdt_MemHandle * const p_ByteBuffer,
                                    uint32 * const q_BufferSize);


/*===========================================================================
FUNCTION gdt_BufFree

DESCRIPTION
  This function frees the buffer associated with the service

DEPENDENCIES 
 
PARAMETERS 
  cpz_Session - Points to the session information from MP to AP
  (input)

  cp_ByteBuffer - Local handle to the allocated buffer
  (input)
  

RETURN VALUE 
  e_GDT_SUCCESS: GDT has successfuly allocated a buffer
 
  e_GDT_FAIL  - Otherwise
 
SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_BufFree(const gdt_SessionType * const cpz_Session,
                                   gdt_MemHandle p_ByteBuffer);


/*===========================================================================
FUNCTION gdt_RcvdSendBeginResponse

DESCRIPTION
  This function is the callback for the upload begin request. GDT needs
  to forward the response to the corresponding module API
 
PARAMETERS 
 
  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to the user
                 API
 
  e_GDT_FAIL:   Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendBeginResponse(gdt_StartArgumentType z_Response);

/*===========================================================================
FUNCTION gdt_RcvdSendEndResponse

DESCRIPTION
  This function is the callback for the upload end message. GDT needs
  to forward the response to the corresponding module API
 
PARAMETERS 
 
  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to the 
                 user API
 
  e_GDT_FAIL:   Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendEndResponse(gdt_EndArgumentType z_Response);

/*===========================================================================
FUNCTION gdt_RcvdSendAckResponse

DESCRIPTION
  This function is the callback for the send ACK message. GDT needs
  to forward the response to the corresponding module API. 
 
PARAMETERS 
 
  cpz_Session - Points to the session information from MP to AP
  (input)

  z_Response - The recieved response
  (input)
 
DEPENDENCIES

RETURN VALUE 
  e_GDT_SUCCESS: GDT has validated the response, and sent the response to
                 the user API
 
  e_GDT_FAIL: Otherwise

SIDE EFFECTS

===========================================================================*/
e_GDTReturnCodesType gdt_RcvdSendAckResponse(gdt_SendAckArgumentType z_Response);

/*===========================================================================

FUNCTION gdt_ReportEventSessionStartReq

DESCRIPTION
  Report event: GDT Session initiated

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionStartReq (const gdt_SessionType * const cpz_Session);

/*===========================================================================

FUNCTION gdt_ReportEventSessionStartResp

DESCRIPTION
  Report event: GDT Session initiated response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionStartResp (gdt_StartArgumentType z_Response);

/*===========================================================================

FUNCTION gdt_ReportEventSendReq

DESCRIPTION
  Report event: GDT Session send data request

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSendReq (const gdt_SessionType * const cpz_Session, uint32 q_DataSize);

/*===========================================================================

FUNCTION gdt_ReportEventSendResp

DESCRIPTION
  Report event: GDT Session send data response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSendResp (gdt_SendAckArgumentType z_Response);

/*===========================================================================

FUNCTION gdt_ReportEventSessionEndReq

DESCRIPTION
  Report event: GDT Session send end req

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionEndReq (const gdt_SessionType * const cpz_Session, uint16 w_Status);

/*===========================================================================

FUNCTION gdt_ReportEventSessionEndResp

DESCRIPTION
  Report event: GDT Session end response

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionEndResp (gdt_EndArgumentType z_Response);

/*===========================================================================

FUNCTION gdt_ReportEventSessionOpenReq

DESCRIPTION
  Report event: GDT Session open

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionOpenReq (const gdt_SessionType * const cpz_Session);

/*===========================================================================

FUNCTION gdt_ReportEventSessionCloseReq

DESCRIPTION
  Report event: GDT Session close

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
void gdt_ReportEventSessionCloseReq (const gdt_SessionType * const cpz_Session);


/*===========================================================================

FUNCTION
  gdt_LogDataBlob

DESCRIPTION
  Logs a GDT BLOB of data

PARAMETERS
  cpz_Iface - Points to the GDT interface containing the raw data to be logged
  (input)
 
  u_Format  - The format of data that needs to be used by the log parser
  (input)     C_GDT_BLOB_FORMAT_BINARY or C_GDT_BLOB_FORMAT_BINARY
 
 w_ContentType - The content type of the raw data used by parser
 (input)

DEPENDENCIES
  None

RETURN VALUE
  TRUE  - If the log is generated successfuly
  FALSE - Otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean gdt_LogDataBlob( const gdt_InterfaceType * cpz_Iface,
                         uint8 u_Format, uint16 w_ContentType);
#endif /* GNSS_GDT_H */


