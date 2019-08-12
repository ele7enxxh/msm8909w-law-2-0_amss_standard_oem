/******************************************************************************/
/**
Program: FTM Sequencer

	$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_sequencer_definitions.h#1 $

	\brief Provides the public defintions used by the FTM Sequencer.

 \b QUALCOMM  \b PROPRIETARY

This document contains propriety information, and except with written
permission of Qualcomm Technologies INC, such information shall not be
published, or disclosed to others, or used for any purpose, and the
document shall not be duplicated in whole or in part.

\note
	1. Need to separate the "internal" definitions from "external" so that the
	   clients see the simplest possible interface.

Compiler:  Microsoft Visual C++ v6.0 SP4
*******************************************************************************/

#if !defined(_FTM_SEQUENCER_DEFINITIONS_H)
#define _FTM_SEQUENCER_DEFINITIONS_H

#include "rfa_variation.h"
#include "comdef.h"

/******************************************************************************
						Callback function definitions
*******************************************************************************/


/******************************************************************************
						Types
*******************************************************************************/

/**
   Data type used for the context identifier for FTM Sequencer state data
*/
typedef unsigned long FTM_Sequencer_ContextID;


/******************************************************************************
						Structures
*******************************************************************************/

/**
    Structure of queue item Time information for the FTM
    Sequencer
*/
typedef struct
{
    unsigned long iExpectedStartTime;  //!< ' Expected start time offset in microseconds
    unsigned long iActualStartTime;    //!< ' Actual start time offset in microseconds
    unsigned long iTimeOverrun;        //!< ' Time overrun from last ADD_WAIT Command, in microseconds
    unsigned long iExecutionDuration;  //!< ' Execution duration in microseconds

} FTM_Sequencer_CommandItem_TimeInfo;

/**
	Structure of a queue item for the FTM sequencer
*/
typedef struct
{
	unsigned long iID;           //!< ' Identifier number of the queue item
	unsigned long iCommandSize;  //!< ' Size of the pCommand buffer, in bytes
	unsigned char* pCommand;     //!< ' Pointer to the command data
	unsigned long iResponseSize; //!< ' Size of the pCommand buffer, in bytes
	unsigned char* pResponse;    //!< ' Pointer to the response data
    unsigned short iCommandStatus;                  //!< ' Error code return from command execution 
    FTM_Sequencer_CommandItem_TimeInfo iTimeInfo;   //!< ' Time information of the queue item
    
} FTM_Sequencer_CommandItem;

/******************************************************************************
						Enumerations
*******************************************************************************/
/**
	FTM Sequencer Error Codes
*/
typedef enum
{
	//! ' No error
	FTM_Sequencer_Error_None = 0,                   

	//! ' Unspecified error
	FTM_Sequencer_Error_General = 1,                

	//! ' Invalid state pointer
	FTM_Sequencer_Error_InvalidState = 2,           

	//! ' Invalid command queue 
	FTM_Sequencer_Error_InvalidCommandQueue = 3,    

	//! ' Command queue  is already initialized
	FTM_Sequencer_Error_CommandQueueAlreadyInitialized = 4, 

	//! ' Command queue  is not initialized
	FTM_Sequencer_Error_CommandQueueNotInitialized = 5, 

	//! ' Maximum command queue size
	FTM_Sequencer_Error_CommandQueueMaxmimumSizeExceeded = 6, 

	//! ' No items remaining
	FTM_Sequencer_Error_CommandQueueNoItems = 7, 

	//! ' Invalid size specified in the queue--mismatch between declared size and actual size
	FTM_Sequencer_Error_CommandQueueInvalidSize = 8, 

	//! ' Invalid command item was specified internal to the FTM Sequencer
	FTM_Sequencer_Error_CommandItemInvalid = 9, 

	//! ' Maximum diagnostic response packet size exceeded
	FTM_Sequencer_Error_DiagnosticResponsePacketSizeExceeded = 10, 

	//! ' Command item is too large
	FTM_Sequencer_Error_CommandItemMaxSizeExceeded = 11, 

	//! ' Memory allocation failed
	FTM_Sequencer_Error_MemoryAllocation = 12, 

	//! ' Memory copy failed
	FTM_Sequencer_Error_MemoryCopy = 13, 

	//! ' Bad FTM Sequencer command
	FTM_Sequencer_Error_BadSequencerCommand = 14, 

	//! ' Bad FTM Sequencer command parameter
	FTM_Sequencer_Error_BadSequencerCommandParameter = 15, 

	//! ' A command was called that is incompatible with the FTM Sequencer state
	FTM_Sequencer_Error_BadSequencerState = 16, 

	//! ' Start time has not been specified when an elapsed duration was requested
	FTM_Sequencer_Error_StartTimeNotSpecified = 17,

	//! ' A command cannot be dispatched while the FTM Sequencer is executing
	FTM_Sequencer_Error_CannotDispatchCommandWhileExecuting = 18,

    //! ' Connot create EFS directory
	FTM_Sequencer_Error_CannotCreateDir = 19,

	//! ' Invalid file name
	FTM_Sequencer_InvalidFileName = 20,

	//! ' Invalid file name
	FTM_Sequencer_Error_CannotOpenFile = 21,

	//! ' EFS Write Error
	FTM_Sequencer_EFS_WRITE_ERROR = 22,

	//! ' EFS Read Error
	FTM_Sequencer_EFS_READ_ERROR = 23,

	//! ' FORMAT VERSION NOT SUPPORTED
	FTM_Sequencer_FORMAT_VERSION_NOT_SUPPORTED = 24,

    //! ' The expected vs actual data items disagree
	FTM_Sequencer_NUM_ITEMS_MISMATCH = 25,

	//! ' Maximum error code number 
	FTM_Sequencer_Error_MAX 

} FTM_Sequencer_ErrorCode;

/**
	FTM Sequencer Commands

	The _ is placed in front of each one to avoid any duplicate symbols
	that might come from using the same function name.
*/
typedef enum
{
	//! ' Add a set of FTM command messages to the FTM Sequence Queue.
	_FTM_SEQ_QUEUE_COMMANDS = 0,

	/** 
		' Add a wait to the FTM Sequence Queue, relative to the 
	      last wait that was received.
	*/
	_FTM_SEQ_ADD_WAIT = 1,

	/** 
		Start/stop capture of all FTM commands that are passed through 
		the FTM Sequencer. Rather than being immediately executed, subsequent 
		commands will be captured in the FTM Sequencer Queue.
	*/
	_FTM_SEQ_CAPTURE_CONTROL = 2,

	//! ' Execute the existing FTM Sequence Queue of FTM messages
	_FTM_SEQ_EXECUTE_QUEUE = 3,

	//! ' Erase all messages in the FTM Sequence Queue
	_FTM_SEQ_CLEAR_QUEUE = 4,

	//! ' Return status of the FTM Sequence Queue.
	_FTM_SEQ_STATUS = 5,

	//! ' Load FTM Command items from an EFS file.
	_FTM_SEQ_LOAD_QUEUE_FROM_FILE = 6,

	//! ' Start logging FTM Command to an EFS file.
	_FTM_SEQ_STORE_QUEUE_TO_FILE = 7,

	//! ' Return a execution response message (FTM response) for a certain item ID
	_FTM_SEQ_GET_COMMAND_RESPONSE_DATA = 8,

	//! ' Return execution response messages (FTM responses) for a multiple item ID's
	_FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA = 9,

	// Put down a timing marker.  This works in conjunction with the ADD_WAIT command
    // When present ADD_WAIT will base its time off of this marker
	_FTM_SEQ_ADD_TIMING_MARKER = 10,

    //! ' Stores available debug information into EFS file after execution has completed
	_FTM_SEQ_STORE_DEBUG_INFO_TO_FILE = 11,

    //! ' Stores Timing error infomation during FTM Sequencer execution
	_FTM_SEQ_TIMING_ERROR_INFO = 12

} FTM_Sequencer_Commands;


/******************************************************************************
						Defintions
*******************************************************************************/

//! Maximum number of commands
#define FTM_SEQUENCER_MAX_QUEUE_SIZE (20000)


//! Maximum size of diagnostic packet
#define FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE (4096)

//! Maximum size of diagnostic response packet header
//! cmd,subsys_id,mode,error
#define FTM_SEQUENCER_MAX_DIAG_PACKET_RSP_PAYLOAD_HDR (12)

//! Maximum size of diagnostic response payload
#define FTM_SEQUENCER_MAX_DIAG_PACKET_RSP_PAYLOAD_SIZE (FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE - FTM_SEQUENCER_MAX_DIAG_PACKET_RSP_PAYLOAD_HDR)

//! Maximum debug message size (bytes)
//! Maximum debug message size should be same as Maximum diagnostic pakcet size to 
//! prevent issues related to buffer overflow.
#define FTM_SEQUENCER_MAX_DEBUG_MESSAGE_SIZE (4096)

//! File name size (bytes)
#define MAX_FILE_NAME (40)

//! Null indicator
#define FTM_SEQUENCER_NULL (0)

//! False indicator
#define FTM_SEQUENCER_FALSE (0)

//! True indicator
#define FTM_SEQUENCER_TRUE (1)

//! Diagnostic Subsystem command
#define FTM_SEQUENCER_SUBSYSTEM_COMMAND (75)

#ifdef FEATURE_RF_RFCA
  //! RFCA mangled Diagnostic Subsystem command
  #define FTM_SEQUENCER_SUBSYSTEM_COMMAND_FROM_RFCA (0xFF)
#endif

//! Diagnostic subsystem ID
#define FTM_SEQUENCER_SUBSYSTEM_ID (11)

//! Mode ID
#define FTM_SEQUENCER_MODE_ID (37)

//! 0=QMSL  1= MODEM
#define FTM_SEQUENCER_CREATOR_ID (1)

//! FORMAT VERSIONS
#define FTM_SEQUENCER_FORMAT_VERSION_1 (1)


// ID for command queue items that do not have a user defined value
#define FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER (0xFFFFFFFF)

/******************************************************************************
						Packet Definitions
*******************************************************************************/

/* 
	DM <--> DMSS packet definitions are byte packed 
	This pragma is specifcally positioned around the diagnostic packet
	structures
*/
     

/**
	Top level FTM Sequencer header for request and response
*/
typedef PACK(struct) 
{
	unsigned char CMD_ID;
	unsigned char SUBSYS_ID;
	unsigned short RF_MODE_ID;
	unsigned short RF_SUB_CMD;
	unsigned short FTM_DATA_LEN;
	unsigned short FTM_RSP_PKT_SIZE;
} FTM_Sequencer_Header;

/**
	FTM general response packet payload
*/
typedef PACK(struct)  
{
	FTM_Sequencer_Header Header;
	unsigned short       FTM_ERROR_CODE;
} FTM_Sequencer_DefaultResponsePayload;

/**
	FTM response packet
*/
typedef PACK(struct) 
{
	unsigned long     RspCmd;
	unsigned long     Pkt_Len;
    void *            PayLoadPtr;
    unsigned long     DeletePayload;
} FTM_Sequencer_ResponsePacket;

/**
	Default FTM Sequencer request packet
*/
typedef PACK(struct) 
{
	FTM_Sequencer_Header Header;
} FTM_Sequencer_DefaultRequest;

/**
	FTM_SEQ_QUEUE_COMMANDS request
*/
typedef PACK(struct) 
{
	FTM_Sequencer_Header Header;
	unsigned short NUM_QUEUE_ITEMS;

	//!< ' Storage area for the entire set of queue items. The size is variable and the maximum is defined here.
	unsigned char QUEUE_ITEMS[ FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE ];	
} FTM_Sequencer_Request_QueueCommands;

/**
	QUEUE_DATA support structure for FTM_SEQ_QUEUE_COMMANDS request.
	Models the header for each individul queue item.
*/
typedef PACK(struct) 
{
	unsigned short NUM_BYTES;
	unsigned long ID_NUMBER;

	//!< ' Storage area for the FTM request packat. The size is variable and the maximum is defined here.
	unsigned char FTM_COMMAND_DATA[ FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE ];	
} FTM_Sequencer_Request_QueueCommands_QueueItem;


/**
	FTM_SEQ_ADD_WAIT request
*/
typedef PACK(struct) 
{
	FTM_Sequencer_Header Header;
	unsigned long WAIT_TIME;
} FTM_Sequencer_Request_AddWait;

/**
	FTM_SEQ_CAPTURE_CONTROL request
*/
typedef PACK(struct) 
{
	FTM_Sequencer_Header Header;
	unsigned char CAPTURE_STATE;
} FTM_Sequencer_Request_CaptureControl;

/**
	FTM_SEQ_STATUS response
*/
typedef PACK(struct) 
{
	FTM_Sequencer_Header Header;
    unsigned short       FTM_ERROR_CODE;
	unsigned long        QUEUE_ITEM_COUNT;
	unsigned char        IS_EXECUTING;
	unsigned long        NEXT_ID_NUMBER;
	unsigned long        TIME_REFERENCE;

} FTM_Sequencer_Response_SequencerStatus;

/**
	FTM_SEQ_STORE_DEBUG_INFO_TO_FILE request
*/

typedef PACK(struct) 
{
    unsigned short       FORMAT_VERSION;
    unsigned long        NUM_ITEMS;
}FTM_Sequencer_File_Debug_Info_Header_Type;

typedef PACK(struct) 
{
	FTM_Sequencer_Header           Header;
	unsigned char                  FILE_NAME[MAX_FILE_NAME];
} FTM_Sequencer_Request_Store_Debug_Info_To_File;

/**
	FTM_SEQ_GET_COMMAND_RESPONSE_DATA request
*/
typedef PACK(struct) 
{
	FTM_Sequencer_Header Header;
	unsigned long ID_NUMBER;

} FTM_Sequencer_Request_GetCommandResponseData;

/**
	FTM_SEQ_GET_COMMAND_RESPONSE_DATA response
*/
typedef PACK(struct)  
{
	FTM_Sequencer_Header Header;
    unsigned short       FTM_ERROR_CODE;
	unsigned long        RESPONSE_SIZE;
} FTM_Sequencer_Response_GetCommandResponseData;

/**
	FTM_SEQ_GET_COMMAND_RESPONSE_DATA response
	This version of the structure includes the full payload.
*/
typedef PACK(struct) 
{
	FTM_Sequencer_Header Header;
    unsigned short       FTM_ERROR_CODE;
	unsigned long        RESPONSE_SIZE;
	unsigned char        RESPONSE_DATA[ FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE ];

} FTM_Sequencer_Response_GetCommandResponseData_WithPayload;

/**
	FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA request

//! Maximum number of ID numbers for the FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA request 
*/ 
#define FTM_SEQUENCER_GET_MULTIPLE_RESPONSE_MAX_ITEMS  \
    (                                                  \
        (                                              \
            FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE         \
		- sizeof (FTM_Sequencer_Header)                \
		- sizeof (unsigned short )                     \
         )                                             \
         / sizeof( unsigned long)                      \
     )
//< end of #define

//! Structure of the first part of the FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA request
typedef PACK(struct)  
{
	FTM_Sequencer_Header Header;
	unsigned long NUM_ITEMS;
	unsigned long ID_NUMBER_LIST [ FTM_SEQUENCER_GET_MULTIPLE_RESPONSE_MAX_ITEMS  ];

} FTM_Sequencer_Request_GetMultipleCommandResponseData;

/**
	FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA response
*/
//! Structure of the first part of the response for FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA 
typedef PACK(struct) 
{
	FTM_Sequencer_Header Header;
    unsigned short FTM_ERROR_CODE;
	unsigned long  NUM_ITEMS;
} FTM_Sequencer_Response_GetMultipleCommandResponseData;

/** 
	Maximum number of bytes the variable length section of the response for 
    FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA request
*/
#define FTM_SEQUENCER_GET_MULTIPLE_RESPONSE_MAX_SIZE    \
    (                                                   \
		  FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE            \
        - sizeof (FTM_Sequencer_Header)                 \
        - sizeof (unsigned short )                     \
    )
//< end of #define

/**
	Structure of the response for FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA,
	including a field for the variable size payload.

	This version of the structure includes the full payload.
*/
typedef PACK(struct)  
{
	FTM_Sequencer_Header Header;
    unsigned short FTM_ERROR_CODE;
	unsigned long  NUM_ITEMS;
	unsigned char  ALL_RESPONSE_DATA[ FTM_SEQUENCER_GET_MULTIPLE_RESPONSE_MAX_SIZE  ];

} FTM_Sequencer_Response_GetMultipleCommandResponseData_WithPayload;


/**
	Structure of the each response in the FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA 
	response packet

	This version of the structure includes does not include the full payload.
*/
typedef PACK(struct) 
{
	//! ID Number of the response
	unsigned long  ID_NUMBER;

	//! Size of the number of bytes in RESPONSE_DATA
	unsigned long  RESPONSE_SIZE;


} FTM_Sequencer_Response_GetMultipleCommandResponseData_ResponseItem;

/**
	Structure of the each response in the FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA 
	response packet, includes a field for the maximum size of the payload

	This version of the structure includes the full payload.
*/
typedef PACK(struct) 
{
	//! ID Number of the response
	unsigned long  ID_NUMBER;

	//! Size of the number of bytes in RESPONSE_DATA
	unsigned long  RESPONSE_SIZE;

	//! Variable length field containing the response data
	unsigned char  RESPONSE_DATA[ FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE ];

} FTM_Sequencer_Response_GetMultipleCommandResponseData_ResponseItem_WithPayload;


/*===========================================================================*/
/**
	Structure of a diagnostic log message, as described in WCDMA ICD section 4.1 - Log record structure
*/
/*===========================================================================*/


#define FTM_SEQ_LOG_CODE            0x117C  //!<' FTM Version 2 log file

#define FTM_SEQ_FTM_LOG_ID          0x0C    //!<' FTM log identifer for FTM_LOG_ID field
#define FTM_SEQ_DIAG_LOG_F          0x11    //!<' Diagnostic Log packet Request/Response command ID

//! Number of bytes used for time stamp in diagnostic async messages
#define FTM_SEQ_DIAG_TIME_STAMP_SIZE 8

/**
	Structure of a log message, as described in WCDMA ICD section 4.1 - Log record structure
*/
typedef PACK(struct) 
{
	unsigned char CMD_CODE;
	unsigned char MORE;
	unsigned short length;
	unsigned short length2;		// Not sure why length is being put in twice...could be a receieve data error
	unsigned short log_item;
	unsigned char iTimeStamp[FTM_SEQ_DIAG_TIME_STAMP_SIZE];
	unsigned short FTM_LOG_ID;

} FTM_SEQUENCER_DiagLogHeader;

/**
	Structure of the FTM Sequencer header
*/
typedef PACK(struct) 
{

	unsigned short SEQUENCE_STATUS;
	unsigned long COMMAND_STATUS;
	unsigned long ID_NUMBER;
	unsigned long EXPECTED_START_TIME;
	unsigned long ACTUAL_START_TIME;
	unsigned long TIME_OVERRUN;
	unsigned long EXECUTION_DURATION;
	unsigned long FTM_RESPONSE_SIZE;

} FTM_SEQUENCER_FTM_SequencerLogHeader;

/**
	Structure of the entire FTM Sequencer structure, including data
*/
typedef PACK(struct) 
{

	// Log header
	FTM_SEQUENCER_DiagLogHeader Header;
	FTM_SEQUENCER_FTM_SequencerLogHeader FTM_SequencerHeader;

	//! Log payload
	unsigned char aLogMsgData[ 
								FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE 
								- sizeof(FTM_SEQUENCER_DiagLogHeader) 
								- sizeof(FTM_SEQUENCER_FTM_SequencerLogHeader) 
							] ;
} FTM_SEQUENCER_FTM_SequencerLog;

/**
	Structure to store FTM Sequencer timing error information
*/
typedef PACK(struct) 
{
    unsigned char     Timing_Overrun_Status;
    unsigned long     Cmd_ID_Timing_Overrun;
    unsigned long     Worse_Case_Time_Delta;
    unsigned long     Cmd_ID_Worse_Case_Time_Delta;
} FTM_Sequencer_Timing_Error_Info;

/**
	FTM_SEQ_TIMING_ERROR_INFO response
*/
typedef PACK(struct) 
{
    FTM_Sequencer_Header Header;
    unsigned short       FTM_ERROR_CODE;
    FTM_Sequencer_Timing_Error_Info Timing_Error_Response_Payload;
} FTM_Sequencer_Response_TimingErrorInfo_WithPayload;

/**
	Structure for the state of the FTM Sequencer
*/
typedef struct
{
    //!' Flag to indicate that the sequencer is executing
    unsigned char isExecuting;             
    
    //! ' Flag to indicate that the sequencer is capturing commands
    unsigned char isCapturing;             
    
    //! ' Flag to indicate that the sequencer is waiting for a response from the mobile
    unsigned char isWaitingForResponse;    
    
    //! ' Flag to indicate whether completion message has been sent
    unsigned char hasCompletionMessageBeenSent;    
    
    //! ' Start time of the sequence -- the adaptation layer
    //! can decide how to use the reserved data.
    unsigned long StartTime[4];
    
    //! Indicator that the start time has been captured
    //! This should be set & checked by the adaptation layer
    unsigned char hasStartTimeBeenCaptured;
    
    
    //! Pointer to the command queue, to be filled in by the CommandQueue implementation
    void* pCommandQueue;
    
    //! Return value
    FTM_Sequencer_ErrorCode  eCommandExecErrorCode;
    
    //! Storage for the next command item from the queue
    FTM_Sequencer_CommandItem* pCommandItem;
    
    //! Response data for the command item in the Queue
    FTM_Sequencer_ResponsePacket ResponseBytes;
    
    //! Used for AddWait, to indicate what is the next start time
    //! This will normally be zero to indicate the next command
    //! should start immediately
    unsigned long iNextStartTime;
    
    //! Elapsed time
    unsigned long iElapsedTime;
    
    //! Elapsed time before command execution
    unsigned long iElapsedTimeBeforeCommand;
    
    //! Elapsed time after command execution
    unsigned long iElapsedTimeAfterCommand;
    
    //! Time overrun from last ADD_WAIT Command, in microseconds
    unsigned long iTimeOverrun;
    
    //! Saves information about the command that lead to timing error during
    //! sequence execution
    FTM_Sequencer_Timing_Error_Info TimingErrorInfo;
    
    //! Flag to track first command that caused timing overrun.
    unsigned short iTimingOverrunFirstCommandFlag;
    
    //! Stores Actual Start time for the last executed command
    unsigned long iActualStartTimeLastCommand;
    
    //! Stores Execution duration for the last executed command
    unsigned long iExecutionDurationLastCommand;

} FTM_SequencerState;

#endif // !defined(_FTM_SEQUENCER_DEFINITIONS_H)
