/******************************************************************************/
/**
Program: FTM Sequencer

	$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_sequencer_adaptation.h#1 $

	\brief Provides the queue functionality of the FTM Sequencer.

 \b QUALCOMM  \b PROPRIETARY

This document contains propriety information, and except with written
permission of Qualcomm Technologies INC, such information shall not be
published, or disclosed to others, or used for any purpose, and the
document shall not be duplicated in whole or in part.

\note
Compiler:  Microsoft Visual C++ v6.0 SP4
*******************************************************************************/

#include "ftm_sequencer_definitions.h"

#if !defined(_FTM_SEQUENCER_ADAPTATION_H)
#define _FTM_SEQUENCER_ADAPTATION_H


#ifdef __cplusplus
extern "C" {
#endif 

	/*****************************************************************************
		Functions required in this layer:

		
		1) Get the state information for a give context ID
		2) Set Start Time 
		3) Calculate elapsed time since Start Time was set
		4) Allocate memory
		5) Deallocate memory
		6) Send request packets to mobile
		7) Send asynchronous packets to the client of FTM Sequencer
		6)	Send request packets to mobile
		7)	Send response packets to the client of FTM Sequencer
		8)	Handle a debug message with a variable argument list
		9)	Handle a debug message as a single string
		10)	Notify adaptation of execution start/stop


	*******************************************************************************/


	/******************************************************************************/
	/**

	Get the state information for a give context ID


		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
	\return Returns a pointer to the FTM_SequencerState state variable to use
	        for this context ID. If the context ID is invalid then NULL should be 
			returned.

	\warning 
	*******************************************************************************/
	FTM_SequencerState* FTM_Sequencer_Adaptation_GetState
				(
				FTM_Sequencer_ContextID iContextID
				);

	/******************************************************************************/
	/**

	Set start time marker. The adaptation layer needs to remember the current 
	system time so that it can be referred to in the elapsed time function.


		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_SetStartTime
				(
				FTM_Sequencer_ContextID iContextID
				);

	/******************************************************************************/
	/**

	Calculate elapsed time since FTM_SequencerAdaptation_SetStartTime was called. 


		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param piElapsedTime = pointer to store the elapsed time since SetStartTime
		                       was called.  Units are microseconds
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_GetElapsedTime
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned long* piElapsedTime
				);

	/******************************************************************************/
	/**

	Allocate memory. 
	
	Handle the allocation of memory.  The memory will later be deallocated using
	FTM_Sequencer_AllocateMemory

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param iSize = number of bytes to allocate
		\param pBuffer = reference to the storage location of the pointer 
		                 to the allocated memory, will be updated by this 
		                 function.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_AllocateMemory
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned long iSize,
				void** pBuffer
				);

	/******************************************************************************/
	/**
	
	Allocate memory from the FTM HEAP
	
	Handle the allocation of memory.  The memory will later be deallocated using
	FTM_Sequencer_Adaptation_FTM_DeallocateMemory

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param iSize = number of bytes to allocate
		\param pBuffer = reference to the storage location of the pointer 
		                 to the allocated memory, will be updated by this 
		                 function.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/

    FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_FTM_AllocateMemory
			    (
                FTM_Sequencer_ContextID iContextID,
			    unsigned long           iSize,
			    void**                  pBuffer
			    );

	/******************************************************************************/
	/**
	
	Copy memory from one buffer to another

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param iSize = number of bytes to allocate
		\param pSourceBuffer = reference to the source storage location .
		\param pDestinationBuffer = reference to the source storage location .
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_CopyMemory
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned long iSize,
				void* pSourceBuffer,
				void* pDestinationBuffer
				);

	/******************************************************************************/
	/**

	Deallocate memory. 
	
	Handle the deallocation of memory that was allocated using FTM_Sequencer_AllocateMemory.

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_DeallocateMemory
				(
				FTM_Sequencer_ContextID iContextID,
				void* pBuffer
				);

	/******************************************************************************/
	/**

	Deallocate memory from the FTM HEAP 
	
	Handle the deallocation of memory that was allocated using FTM_Sequencer_AllocateMemory.

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
    FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_FTM_DeallocateMemory
			(
			FTM_Sequencer_ContextID iContextID,
			void* pBuffer
			);

	/******************************************************************************/
	/**

	Send request packets to mobile
	
	Handle sending a command packet to the mobile.  The packet will not have any
	framing (HDLC escape sequences) or error detection (CRC)

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param iRequestSize = Number of bytes to be sent in the request packet.

		\param piRequestBytes = Pointer to a buffer of request packet contents.
		                        The FTM Sequencer client must destroying this memory.

		\param piResponseSize = Pointer to number of bytes received in the response packet
		                        NOTE: when calling this function, the value must be equal to the
								maximum size of the receive buffer.  When the function returns, the
								value will be the actual number of bytes filled into the receive buffer.

		\param iResponseBytes = Pointer to a buffer to store the response packet contents.
		                        The FTM Sequencer client must destroying this memory.

		\param pCommand = pointer to memory storage of the command.  The
		                  receiptent should make a copy of the memory because
						  the pointer storage can change at any time after this
						  funtion is completed.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_SendPacketToMobile
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned short iRequestPacketSize,
				unsigned char* pRequestBytes,
				FTM_Sequencer_ResponsePacket* pResponseBytes
				);

	/******************************************************************************/
	/**

	Send an async packets to the client of FTM Sequencer
	
	Handle sending a response packets (synchronous responses and asynchronous messages)
	to the FTM Sequencer client.  The packet will not have any framing (HDLC escape 
	sequences) or error detection (CRC)

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param iPacketSize = number of bytes in the pCommand buffer

		\param pResponse = pointer to memory storage of the response.  The
		                  receipient should make a copy of the memory because
						  the pointer storage can change at any time after this
						  function is completed.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	    If the environment will handle the diagnostic log header then
	    the code should access the structure at the FTM_SequencerHeader level
	    and ignore the FTM_SEQUENCER_DiagLogHeader field


	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_SendAsyncPacketToClient
				(
				FTM_Sequencer_ContextID iContextID,
				FTM_SEQUENCER_FTM_SequencerLog* pLogMessage,
				unsigned short iMessageLength 
				);

	/******************************************************************************/
	/**

	Adds a debug message to the adaptation output stream and handles
	expansion of the variable argument list.
	
	This function should be part of the general environment adaptation layer 
	(e.g. Windows) rather than a specific software unit adaptation layer (e.g. QMSL)

	This function will call FTM_Sequencer_Adaptation_HandleDebugMessage

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param pDebugString = null terminated string containing debug information.

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	void FTM_Sequencer_Adaptation_AddDebugMessage
				(
				FTM_Sequencer_ContextID iContextID,
				const char *format,
				...
				);

	/******************************************************************************/
	/**

	Handle a debug message.
	
	Debug messages in a simple format (single string buffer, not variable argument
	list).  
	
	This function should be part of the specific software unit adaptation layer 
	(e.g. QMSL) rather than a general environment adaptation layer (e.g. Windows)

	This function will be called from FTM_Sequencer_Adaptation_AddDebugMessage

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param pDebugString = null terminated string containing debug information.

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	void FTM_Sequencer_Adaptation_HandleDebugMessage
				(
				FTM_Sequencer_ContextID iContextID,
				char* pDebugString
				);
	
	/******************************************************************************/
	/**

	Notification when the sequencer starts or stops execution.
	This notification can be used to setup the environment for execution state or
	exit the execution state.

	For example, it could be used to set a signal that indicates execution is active or
	set a signal to indicate execution is completed.
	

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param bNewStateIsExecuting = FTM_SEQUENCER_TRUE if the sequencer is transitioning
		                              into execution mode. Otherwise FTM_SEQUENCER_FALSE

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_ExecutionNotification
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned char bNewStateIsExecuting
				);
    /******************************************************************************/
	/**

    Adaptation function to perform a sleep operation for periods of 
    time in which the FTM Sequencer is executing a sequence but does 
    not need to actively use the CPU. 
	

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

        \param iNextStartTime = expected start time offset in
               microseconds

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning 
	*******************************************************************************/
    FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_Wait
                (
                FTM_Sequencer_ContextID iContextID,
                unsigned long iNextStartTime
                );

    /******************************************************************************/
	/**
     
    Stores a set of debug information that is available after 
    FTM Sequencer Queue exectuion has completed, to the EFS file 
    specified as parameter. 
     
        \param iContextID     = context identifier used to determine
                                the FTM Sequencer state data when
                                calling the adaptation layer.

        \param FILE_NAME      = Filename is the modem EFS
                                location where to store the FTM
                                Sequencer queue debug information
                                items. The string is a series of
                                signed 8-bit ASCII characters and it
                                must be terminated with a NULL
                                character (0x00).

        \param FORMAT_VERSION = Qualcomm version format identifier
	                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
    FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_Store_Debug_Info_To_File
                (
                FTM_Sequencer_ContextID iContextID,
                unsigned char           FILE_NAME[MAX_FILE_NAME],
                unsigned short          FORMAT_VERSION
                );

    /******************************************************************************/
	/**
    Sets the response packet structure for all commands processed 
    using FTM_Sequencer and also sends ASYNC response messages for 
    task based commands. 
     
        \param iContextID     = context identifier used to determine
                                the FTM Sequencer state data when
                                calling the adaptation layer.

        \param rsp_pkt        = pointer to rsp_pkt when called for
                                task based command.
                                NULL for all other commands
	                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

    *******************************************************************************/

    FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_Process_RspPkt
				(
					FTM_Sequencer_ContextID iContextID,
					void*      rsp_pkt
				);

   /******************************************************************************/
   /** 
    Clears the FTM_SEQ_SIG signal after correctly handling the
    response packet for the last command in FTM sequencer command queue
      
        \param iContextID     = context identifier used to determine
                                the FTM Sequencer state data when
                                calling the adaptation layer.

                       
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

    *******************************************************************************/

    FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_Stop_Sequence_Execution
				(
					FTM_Sequencer_ContextID iContextID
				);


#ifdef __cplusplus
}	// extern "C"
#endif 	

#endif // !defined(_FTM_SEQUENCER_ADAPTATION_H)
