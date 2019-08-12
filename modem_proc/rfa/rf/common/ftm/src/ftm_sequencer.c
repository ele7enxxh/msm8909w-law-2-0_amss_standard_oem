/******************************************************************************/
/**
Program: FTM Sequencer

  $Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_sequencer.c#1 $

  \brief Provides the core functionality of the FTM Sequencer.

 \b QUALCOMM  \b PROPRIETARY 

This document contains propriety information, and except with written
permission of Qualcomm Technologies INC, such information shall not be 
published, or disclosed to others, or used for any purpose, and the   
document shall not be duplicated in whole or in part.  


\note   
Compiler:  Microsoft Visual C++ v6.0 SP4  
*******************************************************************************/

#include "ftm_sequencer.h"
#include "ftm_sequencer_definitions.h"
#include "ftm_sequencer_commandqueue.h"
#include "ftm_sequencer_adaptation.h"
#include "ftm_sequencer_private_definitions.h"
#include "rf_test_task_v.h"

/******************************************************************************
						Internal function Definitions
*******************************************************************************/

	/******************************************************************************/
	/**

	Dispatch a FTM Sequencer command. 

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
		\param iRequestSize = Number of bytes to be sent in the request packet.

		\param piRequestBytes = Pointer to a buffer of request packet contents.
		                        The FTM Sequencer client must destroying this memory.

		\param piResponseSize = Pointer to number of bytes received in the response packet
		                        NOTE: when calling this function, the value must be equal to the
								maximum size of the receive buffer.  When the function returns, the
								value will be the actual number of bytes filled into the receive buffer.

		\param pResponseBytes = Pointer (by reference pointer) to a response packet structure 

		                        If this value is FTM_SEQUENCER_NULL then FTM Sequencer will allocate memory
								using FTM_Sequencer_Adaptation_AllocateMemory() and the client
								must free the memory.

                                If this value is not FTM_SEQUENCER_NULL then it must point to a memory buffer that is
								of size FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE. Because the memory buffer
								is provided by the client then the client must also destory the memory.
 

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning This function should only be called if	the packet is known to be an FTM Sequencer packet.
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Disptacher
				(
				FTM_Sequencer_ContextID       iContextID,
				unsigned short                iRequestPacketSize,
				unsigned char*                pRequestBytes,
				FTM_Sequencer_ResponsePacket* pResponseBytes
				);

	/******************************************************************************/
	/**

	Process the FTM_SEQ_QUEUE_COMMANDS command.

		\param iContextID = context identifier used to determine the FTM Sequencer
							state data when calling the adaptation layer.
							
		\param iRequestSize = Number of bytes to be sent in the request packet.

		\param piRequestBytes = Pointer to a buffer of request packet contents.
								The FTM Sequencer client must destroying this memory.


	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
			Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_QueueCommands
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned short iRequestPacketSize,
				unsigned char* pRequestBytes
				);

	/******************************************************************************/
	/**

	Add a command to the queue

		\param iContextID = context identifier used to determine the FTM Sequencer
							state data when calling the adaptation layer.
		\param iRequestPacketSize = size of request buffer
		\param pRequestBytes = pointer to the request buffer 
		\param iID_Number = Value of the ID_NUMBER field to be associated with this command.
		                    This value is user defined and it is used to identify the 
							response data after the command has executed.
							
	\return Returns FTM_SEQUENCER_TRUE if the FTM_Sequencer_Error_None if the command is processed successfully, 
			Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning This function must be called before any other FTM Sequencer functions are called.
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_AddToCommandQueue
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned short iRequestPacketSize,
				unsigned char* pRequestBytes,
				unsigned long iID_Number
				);

	/******************************************************************************/
	/**

	Handles the FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA message.

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
		\param iRequestSize = Number of bytes in the request packet.

		\param piRequestBytes = Pointer to a buffer of request packet contents.
		                        The FTM Sequencer client must destroying this memory.

		\param piResponseSize = Pointer to number of bytes in the response packet

		\param pResponseBytes = Pointer to a buffer to store the  response packet contents.

		                        If this value is FTM_SEQUENCER_NULL then FTM Sequencer will allocate memory
								using FTM_Sequencer_Adaptation_AllocateMemory() and the client
								must free the memory.

                                If this value is not FTM_SEQUENCER_NULL then it must point to a memory buffer that is
								of size FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE. Because the memory buffer
								is provided by the client then the client must also destory the memory.
 

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning This function should only be called if	the packet is known to be FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_GetMultipleResponseItems
				(
				FTM_Sequencer_ContextID       iContextID,
				unsigned short                iRequestPacketSize,
				unsigned char*                pRequestBytes,
				FTM_Sequencer_ResponsePacket* pResponsePacket
				);

	/******************************************************************************/
	/**

	Determine whether a packet is an FTM Sequencer command

		\param iContextID = context identifier used to determine the FTM Sequencer
							state data when calling the adaptation layer.
		\param iRequestPacketSize = size of request packet
		\param pFTM_Header = pointer to the header of the command packet
							
	\return Returns FTM_SEQUENCER_TRUE if the FTM_Sequencer_Error_None if the command is processed successfully, 
			Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning This function must be called before any other FTM Sequencer functions are called.
	*******************************************************************************/
	unsigned char FTM_Sequencer_IsFTM_SequencerCommand
				(
				unsigned short iRequestPacketSize,
				FTM_Sequencer_Header* pFTM_Header
				);


/******************************************************************************
						Interface Functions
*******************************************************************************/
//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_InitializeState
			(
			FTM_Sequencer_ContextID iContextID
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	// Log message
	FTM_Sequencer_Adaptation_AddDebugMessage( iContextID, "FTM_Sequencer_InitializeState");

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{
		// Initialize the structure
		_pState->isExecuting = FTM_SEQUENCER_FALSE;
		_pState->isCapturing = FTM_SEQUENCER_FALSE;
		_pState->isWaitingForResponse = FTM_SEQUENCER_FALSE;
		_pState->hasCompletionMessageBeenSent = FTM_SEQUENCER_FALSE;
		_pState->StartTime[0] = 0;
		_pState->StartTime[1] = 0;
		_pState->StartTime[2] = 0;
		_pState->StartTime[3] = 0;
		_pState->hasStartTimeBeenCaptured = FTM_SEQUENCER_FALSE;
		_pState->pCommandQueue = FTM_SEQUENCER_NULL;
		_pState->eCommandExecErrorCode = FTM_Sequencer_Error_None;
		_pState->iNextStartTime = 0;	
		_pState->iElapsedTime = 0;
		_pState->iElapsedTimeBeforeCommand = 0;
		_pState->iElapsedTimeAfterCommand = 0;
		_pState->iTimeOverrun = 0;

		// Initialize the Timing Error Info parameters
		_eErrorCode = FTM_Sequencer_InitializeTimingInfoState( iContextID );
		
		// Initialize the Queue
		_eErrorCode = FTM_SequencerQueue_Initialize( iContextID );
	}
	else
	{
		_eErrorCode = FTM_Sequencer_Error_InvalidState;
	}

	return _eErrorCode;
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_InitializeTimingInfoState
			(
			FTM_Sequencer_ContextID iContextID
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	// Log message
	FTM_Sequencer_Adaptation_AddDebugMessage( iContextID, "FTM_Sequencer_InitializeTimingInfoState");

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{
		_pState->TimingErrorInfo.Timing_Overrun_Status = 0;
		_pState->TimingErrorInfo.Cmd_ID_Timing_Overrun = FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER;
		_pState->TimingErrorInfo.Worse_Case_Time_Delta= 0;
		_pState->TimingErrorInfo.Cmd_ID_Worse_Case_Time_Delta = FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER; 
		_pState->iTimingOverrunFirstCommandFlag = 0;
		_pState->iActualStartTimeLastCommand = 0;
		_pState->iExecutionDurationLastCommand = 0;
	}
	else
	{
		_eErrorCode = FTM_Sequencer_Error_InvalidState;
	}

	return _eErrorCode;
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Destructor
				(
				FTM_Sequencer_ContextID iContextID
				)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	// Log message
	FTM_Sequencer_Adaptation_AddDebugMessage( iContextID, "FTM_Sequencer_Destructor");

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{	
		// Initialize the Queue
		_eErrorCode = FTM_SequencerQueue_Uninitialize( iContextID );
	}
	else
	{
		_eErrorCode = FTM_Sequencer_Error_InvalidState;
	}

	return _eErrorCode;
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_ProcessClientRequestPacket
			(
			FTM_Sequencer_ContextID       iContextID,
			unsigned short                iRequestPacketSize,
			unsigned char*                pRequestBytes,
			FTM_Sequencer_ResponsePacket* pResponsePacket
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Check to see if this is a FTM Sequencer command
	FTM_Sequencer_Header* _pFTM_Header = (FTM_Sequencer_Header*) pRequestBytes;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	// Log message
	FTM_Sequencer_Adaptation_AddDebugMessage
		( 
			iContextID, 
			"FTM_Sequencer_ProcessClientRequestPacket( iRequestPacketSize = %d )",
			iRequestPacketSize 
		);

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{
		// Check to see if the packet is addressed to the FTM_SEQUENCER
		if ( FTM_Sequencer_IsFTM_SequencerCommand
					(
					iRequestPacketSize,
					_pFTM_Header
					) == FTM_SEQUENCER_TRUE
			)
		{
			// Call the internal sequencer dispatcher
			_eErrorCode = FTM_Sequencer_Disptacher
						(
						iContextID,
						iRequestPacketSize,
						pRequestBytes,
						pResponsePacket
						);
			
		}	// if ( FTM_Sequencer_IsFTM_SequencerCommand
		else 
		{
			
			// If capturing then store the command in the queue
			if ( _pState->isCapturing )
			{
				// Add using a default ID_NUMBER
				_eErrorCode = FTM_Sequencer_AddToCommandQueue
							(
							iContextID,
							iRequestPacketSize,
							pRequestBytes,
							FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER
							);

			} // if ( _pState->isCapturing )
			
			if ( _pState->isExecuting )
			{
				// Error: cannot dispatch this command while the Sequencer is executing
				_eErrorCode = FTM_Sequencer_Error_CannotDispatchCommandWhileExecuting;

			}	// else if ( _pState->isExecuting )
			else
			{
				// If not Executing...
			
				// Commands are always allowed to pass through for immediate execution
				_eErrorCode = FTM_Sequencer_Adaptation_SendPacketToMobile
								(
								iContextID,
								iRequestPacketSize,
								pRequestBytes,
								pResponsePacket
								);
			}	// if ( _pState->isExecuting )



		} // if ( FTM_Sequencer_IsFTM_SequencerCommand

	} // if ( _pState != FTM_SEQUENCER_NULL )

	return _eErrorCode;


}


/******************************************************************************
						Internal function implementations 
*******************************************************************************/

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Disptacher
			(
			FTM_Sequencer_ContextID       iContextID,
			unsigned short                iRequestPacketSize,
			unsigned char*                pRequestBytes,
			FTM_Sequencer_ResponsePacket* pResponsePacket        // represents the response packet
			)
{

	// Return value
	FTM_Sequencer_ErrorCode                  _eErrorCode = FTM_Sequencer_Error_None;

	//
	// Declare pointers for various response packet 
	//

	FTM_Sequencer_DefaultResponsePayload*    _pResponsePayload = FTM_SEQUENCER_NULL;


	// Check to see if this is a FTM Sequencer command
	FTM_Sequencer_Header*                    _pFTM_Header = (FTM_Sequencer_Header*) pRequestBytes;

	// Get the state structure
	FTM_SequencerState*                      _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );


	// Capture control request
	FTM_Sequencer_Request_CaptureControl*    _pRequestCaptureControl = 
					(FTM_Sequencer_Request_CaptureControl*) pRequestBytes;

	// Add Wait request
	FTM_Sequencer_Request_AddWait*           _pRequestWait = (FTM_Sequencer_Request_AddWait*) pRequestBytes;


	// Status response
	FTM_Sequencer_Response_SequencerStatus* _pStatusResponse = FTM_SEQUENCER_NULL;

	// Store FTM Sequencer Debug Info to File
	FTM_Sequencer_Request_Store_Debug_Info_To_File* _pRequest_Store_Debug_Info_To_File = (FTM_Sequencer_Request_Store_Debug_Info_To_File*) pRequestBytes;

	// Get Response Data -- request
	FTM_Sequencer_Request_GetCommandResponseData* _pGetCommandResponseData_req = FTM_SEQUENCER_NULL;
	// Get Response Data -- response
	FTM_Sequencer_Response_GetCommandResponseData_WithPayload* _pGetCommandResponseData_resp = FTM_SEQUENCER_NULL;

	// Get Timing Error Info -- response
	FTM_Sequencer_Response_TimingErrorInfo_WithPayload* _pTimingErrorInfo_resp = FTM_SEQUENCER_NULL;

	// Intermediate data for FTM_SEQ_GET_COMMAND_RESPONSE_DATA
	unsigned long iResponseBufferSize = 0;
	void* pResponseBuffer = FTM_SEQUENCER_NULL;

	unsigned long iElapsedTime = 0;

	// If !NULL then return the queue pointer
    if ( _pState != FTM_SEQUENCER_NULL )
    {
       if ( pResponsePacket != FTM_SEQUENCER_NULL  )
       {
          if( pResponsePacket->PayLoadPtr == FTM_SEQUENCER_NULL )
          {
            //
            // Allocate the payload.  Use MAX allowed by Diag
            // Later we will indicate how many bytes are 
            // really meaningful.

			FTM_Sequencer_Adaptation_FTM_AllocateMemory
			(
				iContextID,
				(unsigned long)FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE,
				&pResponsePacket->PayLoadPtr
			);

            if(pResponsePacket->PayLoadPtr == FTM_SEQUENCER_NULL)
			{
              _eErrorCode = FTM_Sequencer_Error_MemoryAllocation;
            }
            else
			{
			// Mark the packet for deletion
			pResponsePacket->DeletePayload = FTM_SEQUENCER_TRUE;
            }
		 }
		 else
		 {
			  			 // Mark thie packet for deletion
			pResponsePacket->DeletePayload = FTM_SEQUENCER_FALSE;

         } // end if( pResponsePacket->PayLoadPtr == FTM_SEQUENCER_NULL )

		 // Pointer to cast into the default response structure
		 _pResponsePayload = (FTM_Sequencer_DefaultResponsePayload*) pResponsePacket->PayLoadPtr;

		if (_eErrorCode  == FTM_Sequencer_Error_None)
		{
		      // Fill in response payload header
		      _pResponsePayload->Header.CMD_ID           = _pFTM_Header->CMD_ID;
		      _pResponsePayload->Header.SUBSYS_ID        = _pFTM_Header->SUBSYS_ID;
		      _pResponsePayload->Header.RF_MODE_ID       = _pFTM_Header->RF_MODE_ID;
		      _pResponsePayload->Header.RF_SUB_CMD       = _pFTM_Header->RF_SUB_CMD;
		      _pResponsePayload->Header.FTM_DATA_LEN     = _pFTM_Header->FTM_DATA_LEN;
		      _pResponsePayload->Header.FTM_RSP_PKT_SIZE = _pFTM_Header->FTM_RSP_PKT_SIZE;

              pResponsePacket->Pkt_Len                    = sizeof(FTM_Sequencer_DefaultResponsePayload);
              pResponsePacket->RspCmd                     = 2; // FTM_RSP_DO_NEW_PKT
		}

		// If the sequencer is executing then the only valid command is
		// for status
		if (
				( _pState->isExecuting )
			&&	( _pFTM_Header->RF_SUB_CMD != _FTM_SEQ_STATUS )
			&&  (_eErrorCode  == FTM_Sequencer_Error_None)
			)
		{
			// Error: cannot dispatch this command while the Sequencer is executing
			_eErrorCode = FTM_Sequencer_Error_CannotDispatchCommandWhileExecuting;
			FTM_Sequencer_Adaptation_AddDebugMessage
				    ( 
					   iContextID, 
					   "FTM_Sequencer_Error_CannotDispatchCommandWhileExecuting"
				    );
			if ( _pResponsePayload  != NULL )
			{
			  _pResponsePayload->FTM_ERROR_CODE = FTM_Sequencer_Error_CannotDispatchCommandWhileExecuting;
			}

		}	// else if ( _pState->isExecuting )


		if (_eErrorCode  == FTM_Sequencer_Error_None)
		{              

		      switch ( _pFTM_Header->RF_SUB_CMD )
		      {
		         case _FTM_SEQ_QUEUE_COMMANDS: 


			     // No handler yet
			     _eErrorCode = FTM_Sequencer_QueueCommands
								(
								iContextID,
								iRequestPacketSize,
								pRequestBytes
								);
			     break;


		         case _FTM_SEQ_ADD_WAIT: 
			        _eErrorCode = FTM_Sequencer_AddToCommandQueue
						(
						iContextID,
						iRequestPacketSize,
						pRequestBytes,
						FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER
						);

			        // Log message
			        FTM_Sequencer_Adaptation_AddDebugMessage
				    ( 
					   iContextID, 
					   "FTM_SEQ_ADD_WAIT( WAIT_TIME = %d)",
					   _pRequestWait->WAIT_TIME
				    );

			        break;

		         case _FTM_SEQ_ADD_TIMING_MARKER: 
			        _eErrorCode = FTM_Sequencer_AddToCommandQueue
						(
						iContextID,
						iRequestPacketSize,
						pRequestBytes,
						FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER
						);

			        // Log message
			        FTM_Sequencer_Adaptation_AddDebugMessage
				    ( 
					   iContextID, 
					   "FTM_SEQ_ADD_TIMING_MARKER"
				    );

			        break;

		         case _FTM_SEQ_CAPTURE_CONTROL: 
			        _pState->isCapturing = _pRequestCaptureControl->CAPTURE_STATE;

			        // Log message
			        FTM_Sequencer_Adaptation_AddDebugMessage
				    ( 
					   iContextID, 
					   "FTM_SEQ_CAPTURE_CONTROL( CAPTURE_STATE = %d)",
					   _pRequestCaptureControl->CAPTURE_STATE
				    );

			        _eErrorCode = FTM_Sequencer_Error_None;

			        break;

		         case _FTM_SEQ_EXECUTE_QUEUE: 

			        // If not capturing
			        if ( _pState->isExecuting == FTM_SEQUENCER_FALSE )
			        {
						// Disable flag that indicates waiting for a response
						_pState->isWaitingForResponse = FTM_SEQUENCER_FALSE;

						// Enable execution
						_pState->isExecuting = FTM_SEQUENCER_TRUE;

						// Set the initial start time to zero for the next command
						_pState->iNextStartTime = 0;

						// Clear start time flag
						_pState->hasStartTimeBeenCaptured = FTM_SEQUENCER_FALSE;

						// Clear completion message flag
						_pState->hasCompletionMessageBeenSent = FTM_SEQUENCER_FALSE;


						// Call environment adaptation to start the time
						_eErrorCode = FTM_Sequencer_Adaptation_SetStartTime( iContextID );

						// Initialize the Timing Error Info parameters
						_eErrorCode = FTM_Sequencer_InitializeTimingInfoState( iContextID );

						// Reset the command queue iterator
						FTM_SequencerQueue_ResetIterator( iContextID );


						// Log message
						FTM_Sequencer_Adaptation_AddDebugMessage
						( 
						  iContextID, 
						  "FTM_SEQ_EXECUTE_QUEUE() - execute started"
						);

						// Notify the adaptation layer
						FTM_Sequencer_Adaptation_ExecutionNotification
							(
							iContextID,
							FTM_SEQUENCER_TRUE // bNewStateIsExecuting
							);

			        }
			        else
			        {
				       // Cannot start executing when execution is already active
				       _eErrorCode = FTM_Sequencer_Error_BadSequencerState;

				       // Log message
				       FTM_Sequencer_Adaptation_AddDebugMessage
					   ( 
					      iContextID, 
						  "FTM_SEQ_EXECUTE_QUEUE() - is already started"
					   );

			        }

			        break;

		         case _FTM_SEQ_CLEAR_QUEUE: 

			        _eErrorCode = FTM_SequencerQueue_EmptyQueue( iContextID );

			        // Log message
			        FTM_Sequencer_Adaptation_AddDebugMessage
				    ( 
					   iContextID, 
					   "FTM_SEQ_CLEAR_QUEUE()"
				    );

			        break;

                  case _FTM_SEQ_STATUS: 

                    // Assign the status response structure to the payload
                    _pStatusResponse = (FTM_Sequencer_Response_SequencerStatus*) _pResponsePayload;

                    // Set the response payload length
                    pResponsePacket->Pkt_Len = sizeof(FTM_Sequencer_Response_SequencerStatus);

			        _eErrorCode = FTM_Sequencer_Error_None;

			        _pStatusResponse->IS_EXECUTING = _pState->isExecuting;

			        //!!! This needs to be changed
			        _pStatusResponse->NEXT_ID_NUMBER = FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER;

			        // the current elapsed time
			        FTM_Sequencer_Adaptation_GetElapsedTime( iContextID, &iElapsedTime);
                    
                    // Do not optimize this as it is required for the QDSP6
                    _pStatusResponse->TIME_REFERENCE = iElapsedTime;

			        _eErrorCode = FTM_SequencerQueue_GetItemCount
					(
				       iContextID,
					   (unsigned long *)&_pStatusResponse->QUEUE_ITEM_COUNT
					);

			        // Log message
			        FTM_Sequencer_Adaptation_AddDebugMessage
				    ( 
					   iContextID, 
					   "FTM_SEQ_STATUS() IS_EXECUTING = %d, NEXT_ID = 0x%X, TIME_REFERENCE = %d, QUEUE_ITEM_COUNT = %d",
					   _pStatusResponse->IS_EXECUTING,
					   _pStatusResponse->NEXT_ID_NUMBER,
					   _pStatusResponse->TIME_REFERENCE,
					   _pStatusResponse->QUEUE_ITEM_COUNT
			   	    );

			        break;

				  case _FTM_SEQ_STORE_DEBUG_INFO_TO_FILE:
				
					// If not executing the queue
			        if ( _pState->isExecuting == FTM_SEQUENCER_FALSE )
			        {
			            // Log message
			            FTM_Sequencer_Adaptation_AddDebugMessage
				        ( 
					      iContextID, 
                          "FTM_SEQ_STORE_DEBUG_INFO_TO_FILE File Name=%s FORMAT_VERSION=%d ",
					      _pRequest_Store_Debug_Info_To_File->FILE_NAME,
						  FTM_SEQUENCER_FORMAT_VERSION_1
			   	        );

                        _eErrorCode = FTM_Sequencer_Adaptation_Store_Debug_Info_To_File
                        (
                          iContextID,
                          _pRequest_Store_Debug_Info_To_File->FILE_NAME,
						  FTM_SEQUENCER_FORMAT_VERSION_1
                        );
                    }
                    else
					{
				        // Cannot Store While Executing
				        _eErrorCode = FTM_Sequencer_Error_BadSequencerState;

				        // Log message
						FTM_Sequencer_Adaptation_AddDebugMessage
					    ( 
					      iContextID, 
						  "FTM_SEQ_STORE_DEBUG_INFO_TO_FILE() - Cannot Store While Executing"
					    );
                    }

				    break;
                  case _FTM_SEQ_GET_COMMAND_RESPONSE_DATA: 

					// Cast the specific structure to the request message
					_pGetCommandResponseData_req = (FTM_Sequencer_Request_GetCommandResponseData*) pRequestBytes;

                    // Cast the specific structure to the response message
                    _pGetCommandResponseData_resp = (FTM_Sequencer_Response_GetCommandResponseData_WithPayload*) _pResponsePayload;

					// Get the response data from the queue
					_eErrorCode = FTM_SequencerQueue_GetResponseData
					(
						iContextID,
						_pGetCommandResponseData_req->ID_NUMBER,
						&iResponseBufferSize,
						&pResponseBuffer
					);

					if ( _eErrorCode == FTM_Sequencer_Error_None ) 
					{
					
						//  If the size is >0 then copy the command response memory into the response packet
						if ( iResponseBufferSize > 0 )
						{
							// Copy the FTM response data into the storage space
							_eErrorCode = FTM_Sequencer_Adaptation_CopyMemory
										(
											iContextID,
											iResponseBufferSize,
											pResponseBuffer,
											&_pGetCommandResponseData_resp->RESPONSE_DATA
										);
							
						}
					}

                    // Set the response message length
                    pResponsePacket->Pkt_Len = sizeof(FTM_Sequencer_Response_GetCommandResponseData) + iResponseBufferSize;
					_pGetCommandResponseData_resp->RESPONSE_SIZE = iResponseBufferSize;

			        // Log message
			        FTM_Sequencer_Adaptation_AddDebugMessage
				    ( 
					   iContextID, 
					   "FTM_SEQ_GET_COMMAND_RESPONSE_DATA() ID = %d, SIZE = %d",
						_pGetCommandResponseData_req->ID_NUMBER,
						iResponseBufferSize
			   	    );

			        break;

				  case _FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA:
					_eErrorCode = FTM_Sequencer_GetMultipleResponseItems
								(
									iContextID,
									iRequestPacketSize,
									pRequestBytes,
									pResponsePacket
								);

					break;

				  case _FTM_SEQ_TIMING_ERROR_INFO:
				
					// If not executing the queue
					if ( _pState->isExecuting == FTM_SEQUENCER_FALSE )
					{
						// Log message
						FTM_Sequencer_Adaptation_AddDebugMessage
						( 
						  iContextID, 
						  "FTM_SEQ_TIMING_ERROR_INFO() - Looking for FTM Sequener timing error data"
						);
					
						_pTimingErrorInfo_resp  = (FTM_Sequencer_Response_TimingErrorInfo_WithPayload*)_pResponsePayload;
						
						_pTimingErrorInfo_resp->Header.FTM_RSP_PKT_SIZE = sizeof(FTM_Sequencer_Response_TimingErrorInfo_WithPayload);
						// Set the response payload length
						pResponsePacket->Pkt_Len = sizeof(FTM_Sequencer_Response_TimingErrorInfo_WithPayload);
					
						_eErrorCode = FTM_Sequencer_Error_None;
					
						_pTimingErrorInfo_resp->Timing_Error_Response_Payload.Timing_Overrun_Status = _pState->TimingErrorInfo.Timing_Overrun_Status;
					
						_pTimingErrorInfo_resp->Timing_Error_Response_Payload.Cmd_ID_Timing_Overrun = _pState->TimingErrorInfo.Cmd_ID_Timing_Overrun;	
					
						_pTimingErrorInfo_resp->Timing_Error_Response_Payload.Worse_Case_Time_Delta = _pState->TimingErrorInfo.Worse_Case_Time_Delta;
					
						_pTimingErrorInfo_resp->Timing_Error_Response_Payload.Cmd_ID_Worse_Case_Time_Delta = _pState->TimingErrorInfo.Cmd_ID_Worse_Case_Time_Delta;
					
						// Log message
						FTM_Sequencer_Adaptation_AddDebugMessage
						(	 
							iContextID, 
							"FTM_SEQ_TIMING_ERROR_INFO() Timing_Overrun_Status = %d (Id_Number = %d), Time_Delat_Worst_Case = %d (Id_Number = %d)",
							_pTimingErrorInfo_resp->Timing_Error_Response_Payload.Timing_Overrun_Status,
							_pTimingErrorInfo_resp->Timing_Error_Response_Payload.Cmd_ID_Timing_Overrun,
							_pTimingErrorInfo_resp->Timing_Error_Response_Payload.Worse_Case_Time_Delta,
							_pTimingErrorInfo_resp->Timing_Error_Response_Payload.Cmd_ID_Worse_Case_Time_Delta
						);
					
					}
					else
					{
						// Cannot Run this command while executing queue
						_eErrorCode = FTM_Sequencer_Error_BadSequencerState;
					
						// Log message
						FTM_Sequencer_Adaptation_AddDebugMessage
						( 
						  iContextID, 
						  "FTM_SEQ_TIMING_ERROR_INFO() - Cannot report the infomation while executing"
						);
					}
					
					break;

		         default:
			        // No handler found
			        _eErrorCode = FTM_Sequencer_Error_BadSequencerCommand;

			        break;
		      };


			  // Fill in response data which is just after the header in the payload
		      _pResponsePayload->FTM_ERROR_CODE = (unsigned short) _eErrorCode;
           }
           else
		   {
              //Report error could not allocate memory for response payload
		   }
        }
        else
	    {
	       // Report error could not allocate memory for response pkt
	    }
    }
    else
	{
	   _eErrorCode = FTM_Sequencer_Error_InvalidState;
	}

	return _eErrorCode;

}


//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_QueueCommands
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned short iRequestPacketSize,
			unsigned char* pRequestBytes
			)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Check to see if this is a FTM Sequencer command
	FTM_Sequencer_Header* _pFTM_Header = (FTM_Sequencer_Header*) pRequestBytes;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	//
	// Pointers for various casts
	//

	// Add Wait request
	FTM_Sequencer_Request_QueueCommands* _pRequestQueueCommands = 
			(FTM_Sequencer_Request_QueueCommands*) pRequestBytes;


	// Status response
	FTM_Sequencer_Request_QueueCommands_QueueItem* _pQueueItem = FTM_SEQUENCER_NULL;

	// Index counter for the number of FTM commands in the packet
	unsigned long _iCommandIndex = 0;

	// Current offset into the diagnostic packet's QUEUE_ITEMS field
	unsigned long _iQueueItemOffset = 0;

	// End of next queue item 
	unsigned long _iQueueItemEndOffset = 0;

	// Expected maximum size of the QUEUE_ITEMS field
	unsigned long _iExpectedQueueItemsSize = 
				  iRequestPacketSize 
				- sizeof(FTM_Sequencer_Header) 
				- sizeof( _pRequestQueueCommands->NUM_QUEUE_ITEMS );

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{

		// Check the command ID
		if ( _pFTM_Header->RF_SUB_CMD == _FTM_SEQ_QUEUE_COMMANDS )
		{

			// Set no error
			_eErrorCode = FTM_Sequencer_Error_None;

			// Loop through each item
			for (	_iCommandIndex = 0;
					_iCommandIndex < _pRequestQueueCommands->NUM_QUEUE_ITEMS;
					_iCommandIndex++
				)
			{
				// Cast to the next queue item, based on the offset
				_pQueueItem = (FTM_Sequencer_Request_QueueCommands_QueueItem*)
						&_pRequestQueueCommands->QUEUE_ITEMS[ _iQueueItemOffset ];

				// Calculate the end of the buffer
				_iQueueItemEndOffset = _iQueueItemOffset + _pQueueItem->NUM_BYTES;

				// Check the queue item offset against the request packet size
				if ( _iQueueItemEndOffset <= _iExpectedQueueItemsSize )
				{
					// Add the next command queue data 
					_eErrorCode = FTM_Sequencer_AddToCommandQueue
								(
								iContextID,
								_pQueueItem->NUM_BYTES,
								(unsigned char *) _pQueueItem->FTM_COMMAND_DATA,
								_pQueueItem->ID_NUMBER
								);
				
					
					// Log message
					FTM_Sequencer_Adaptation_AddDebugMessage
						( 
							iContextID, 
							"FTM_SEQ_QUEUE_COMMANDS( INDEX = %d, SIZE = %d)",
							_iCommandIndex,
							_pQueueItem->NUM_BYTES
						);
					
				}
				else
				{
					// Error, size mismatch
					_eErrorCode = FTM_Sequencer_Error_CommandQueueInvalidSize;

				}

				// Increment the offset into the structure
				_iQueueItemOffset += _pQueueItem->NUM_BYTES
					              +  sizeof(_pQueueItem->ID_NUMBER)
								  +  sizeof(_pQueueItem->NUM_BYTES);

				// If there is an error the exit
				if (_eErrorCode != FTM_Sequencer_Error_None) 
				{
					break;
				}	// if (_eErrorCode != FTM_Sequencer_Error_None) 

			}	// for (	_iCommandIndex = 0;
		}
		else
		{
			_eErrorCode = FTM_Sequencer_Error_InvalidCommandQueue;

		} // if ( _pFTM_Header->RF_SUB_CMD == _FTM_SEQ_QUEUE_COMMANDS )

	}
	else
	{
		_eErrorCode = FTM_Sequencer_Error_InvalidState;
	}

	return _eErrorCode;

}


//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_AddToCommandQueue
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned short iRequestPacketSize,
			unsigned char* pRequestBytes,
			unsigned long iID_Number
			)
{
	FTM_Sequencer_CommandItem _CommandItem;

	// Add to queue
	FTM_Sequencer_ErrorCode _eErrorCode = FTM_Sequencer_Error_None;

	// Assign a default number
	_CommandItem.iID = iID_Number;

	// Assign size and pointer to data
	_CommandItem.iCommandSize = iRequestPacketSize;
	_CommandItem.pCommand = pRequestBytes;

	// Add to the queue
	_eErrorCode = FTM_SequencerQueue_AddItem
					(
					iContextID,
					&_CommandItem
					);

	return _eErrorCode;
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_GetMultipleResponseItems
			(
			FTM_Sequencer_ContextID       iContextID,
			unsigned short                iRequestPacketSize,
			unsigned char*                pRequestBytes,
			FTM_Sequencer_ResponsePacket* pResponsePacket
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Check to see if this is a FTM Sequencer command
	FTM_Sequencer_Header* _pFTM_Header = (FTM_Sequencer_Header*) pRequestBytes;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	//
	// Pointers for various casts
	//

	// FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA request
	FTM_Sequencer_Request_GetMultipleCommandResponseData* _pRequestGetMulitpleResponse = 
			(FTM_Sequencer_Request_GetMultipleCommandResponseData*) pRequestBytes;


	// FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA Response
	FTM_Sequencer_Response_GetMultipleCommandResponseData_WithPayload* _pResponseGetMultipleResponse = FTM_SEQUENCER_NULL;

	// Pointer to the storage area for the next response item
	FTM_Sequencer_Response_GetMultipleCommandResponseData_ResponseItem* _pNextResponseItem = FTM_SEQUENCER_NULL;

	// Index counter for the number of items to be processed
	unsigned long _iItemIndex = 0;

	// Response payload size--start with the size of the base response structure
	unsigned long _iResponsePacketSize = sizeof( FTM_Sequencer_Response_GetMultipleCommandResponseData );

	// Next response buffer size
	unsigned long iNextResponseBufferSize = 0;

	// Next response buffer pointer
	void* pNextResponseBuffer = FTM_SEQUENCER_NULL;

	// Next offset into the response packet
	unsigned char* pResponsePacketOffset = FTM_SEQUENCER_NULL; 

	// Size of the next response item (header + data)
	unsigned long iNextResponseItemSize = 0;

	// Determine the next ID_NUMBER
	unsigned long iNext_ID_NUMBER = 0;

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{

		// Check the command ID
		if ( _pFTM_Header->RF_SUB_CMD == _FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA )
		{

			// Set no error
			_eErrorCode = FTM_Sequencer_Error_None;


			// Loop through each item to copy into the response packet
			if ( _eErrorCode == FTM_Sequencer_Error_None )
			{
				// Assign a working pointer, starting at the beginning of the payload
				_pResponseGetMultipleResponse = (FTM_Sequencer_Response_GetMultipleCommandResponseData_WithPayload*) pResponsePacket->PayLoadPtr;
				                                 
				// Assign overhead field(s)
				_pResponseGetMultipleResponse->NUM_ITEMS = _pRequestGetMulitpleResponse->NUM_ITEMS;

				// Assign overhead field(s)
				_pResponseGetMultipleResponse->FTM_ERROR_CODE = _eErrorCode;

				// Offset into the paylay, which will be updated for each item
				pResponsePacketOffset = &_pResponseGetMultipleResponse->ALL_RESPONSE_DATA[0];

				for (	_iItemIndex = 0;
						_iItemIndex< _pRequestGetMulitpleResponse->NUM_ITEMS;
						_iItemIndex++
					)
				{
											
					// Determine the next ID_NUMBER
					iNext_ID_NUMBER = _pRequestGetMulitpleResponse->ID_NUMBER_LIST[ _iItemIndex ];

					// Get the response data from the queue
					_eErrorCode = FTM_SequencerQueue_GetResponseData
					(
						iContextID,
						iNext_ID_NUMBER,
						&iNextResponseBufferSize,
				        &pNextResponseBuffer
					);


					iNextResponseItemSize = sizeof( FTM_Sequencer_Response_GetMultipleCommandResponseData_ResponseItem );

					// Check error code
					// Checks if number or bytes in response packet for _FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA 
					// is within limit i.e. FTM_SEQUENCER_GET_MULTIPLE_RESPONSE_MAX_SIZE
					if ((iNextResponseItemSize + iNextResponseBufferSize + _iResponsePacketSize) > FTM_SEQUENCER_GET_MULTIPLE_RESPONSE_MAX_SIZE)
					{
						_eErrorCode = FTM_Sequencer_Error_DiagnosticResponsePacketSizeExceeded;
						// Logs the error code and ITEM Number that resulted in error.
						FTM_Sequencer_Adaptation_AddDebugMessage
						( 
							iContextID, 
							"FTM_SEQ_GET_MULTIPLE_COMMAND_RESPONSE_DATA( _eErrorCode = %d : ITEM NUM = %d)",
							_eErrorCode,
							_iItemIndex
						);
					}
					else
					{

						// Calculate the size of the next response item (header + data)
						iNextResponseItemSize = sizeof( FTM_Sequencer_Response_GetMultipleCommandResponseData_ResponseItem );
						iNextResponseItemSize += iNextResponseBufferSize;

						// Include the item structure
						_iResponsePacketSize += iNextResponseItemSize;

						// Assign pointer to the storage area for the next response item
						_pNextResponseItem = (FTM_Sequencer_Response_GetMultipleCommandResponseData_ResponseItem*) pResponsePacketOffset;

						// Fill in the header for the next response item 
						_pNextResponseItem->ID_NUMBER = iNext_ID_NUMBER;
						_pNextResponseItem->RESPONSE_SIZE = iNextResponseBufferSize;

						// Update the offset in the payload, for the response item header size
						pResponsePacketOffset += sizeof(*_pNextResponseItem) ;

						// Copy the buffer of the next response item 
						_eErrorCode = FTM_Sequencer_Adaptation_CopyMemory
										(
											iContextID,
											iNextResponseBufferSize,
											pNextResponseBuffer,            // Source
											(void*) pResponsePacketOffset   // destination
										);
						
						// Update the offset in the payload, for the response item bufer size
						pResponsePacketOffset += iNextResponseBufferSize;


					}	// if ( _eErrorCode == FTM_Sequencer_Error_None )

				}	// for ( _iItemIndex = 0;...			
	
                 pResponsePacket->Pkt_Len = _iResponsePacketSize;

			}	// if ( _eErrorCode == FTM_Sequencer_Error_None )
		}
		else
		{
			_eErrorCode = FTM_Sequencer_Error_InvalidCommandQueue;

		} // if ( _pFTM_Header->RF_SUB_CMD == _FTM_SEQ_QUEUE_COMMANDS )

	}
	else
	{
		_eErrorCode = FTM_Sequencer_Error_InvalidState;
	}

	return _eErrorCode;
}

//------------------------------------------------------------------------------------
unsigned char FTM_Sequencer_IsFTM_SequencerCommand
			(
			unsigned short iRequestPacketSize,
			FTM_Sequencer_Header* pFTM_Header
			)
{
	unsigned char _bIsFTM_SequencerPacket = FTM_SEQUENCER_FALSE;
	unsigned long _iMinimumSize = 
						  sizeof(pFTM_Header->CMD_ID)
						+ sizeof(pFTM_Header->SUBSYS_ID)
						+ sizeof(pFTM_Header->RF_MODE_ID);

	if ( iRequestPacketSize > _iMinimumSize )
		{
		// This if statement is broken in to sections instead of using
		// an AND construction, to ensure that no memory bytes are
		// assumed to be present (2nd or 3rd bytes might not be present
		// for a single byte command)
		if ( pFTM_Header->CMD_ID == FTM_SEQUENCER_SUBSYSTEM_COMMAND )
		{
			if ( pFTM_Header->SUBSYS_ID == FTM_SEQUENCER_SUBSYSTEM_ID )
			{
				if ( pFTM_Header->RF_MODE_ID == FTM_SEQUENCER_MODE_ID )
				{
					_bIsFTM_SequencerPacket = FTM_SEQUENCER_TRUE;

				}	// if ( _pFTM_Header->RF_MODE_ID == FTM_SEQUENCER_MODE_ID )
			
			} // if ( _pFTM_Header->SUBSYS_ID == FTM_SEQUENCER_SUBSYSTEM_ID )
		
		}	// if ( _pFTM_Header->CMD_ID == FTM_SEQUENCER_SUBSYSTEM_COMMAND )

	} // if ( iRequestPacketSize > _iMinimumSize )

	return _bIsFTM_SequencerPacket;
}


//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_SendAsyncReponseMessage
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned long iResponsePacketSize,
			unsigned char* pResponseBytes,
			unsigned char bIsQueueCompleted,
			unsigned long iCommandStatus,
			unsigned long iID_Number,
			unsigned long iExpectedStartTime,
			unsigned long iActualStartTime,
			unsigned long iTimeOverrun,
			unsigned long iExecutionDuration
			)
{
	////////////////////////
	// Variables
	//////////////////////////

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Async log packet
	static FTM_SEQUENCER_FTM_SequencerLog LogMessage;

	// Length of entire message
	unsigned short _iFullMessageLength = (unsigned short ) 
						(
						  sizeof( FTM_SEQUENCER_DiagLogHeader )
						+ sizeof( FTM_SEQUENCER_FTM_SequencerLogHeader )
						+ (unsigned short) iResponsePacketSize
						);

	// Length of the LOG_ITEM (not including diagnostic header)
	unsigned short _iLOG_ITEM_Length = (unsigned short) 
						(
						  (unsigned short) sizeof( FTM_SEQUENCER_FTM_SequencerLogHeader )
						+ iResponsePacketSize
						);


	////////////////////////
	// Code
	//////////////////////////
	
	// Fill in log header -- on the embedded side, this section will not be used
	// so this is only necessary for PC-side emulation
	LogMessage.Header.CMD_CODE = FTM_SEQ_DIAG_LOG_F;
	LogMessage.Header.MORE = 0;
	LogMessage.Header.length = _iLOG_ITEM_Length;
	LogMessage.Header.length2 = _iLOG_ITEM_Length;
	LogMessage.Header.log_item = FTM_SEQ_LOG_CODE;
	LogMessage.Header.iTimeStamp[0] = 0;
	LogMessage.Header.iTimeStamp[1] = 0;
	LogMessage.Header.iTimeStamp[2] = 0;
	LogMessage.Header.iTimeStamp[3] = 0;
	LogMessage.Header.iTimeStamp[4] = 0;
	LogMessage.Header.iTimeStamp[5] = 0;
	LogMessage.Header.iTimeStamp[6] = 0;
	LogMessage.Header.iTimeStamp[7] = 0;
	LogMessage.Header.FTM_LOG_ID = FTM_SEQ_FTM_LOG_ID;

	
	// Fill in the FTM Log header
	LogMessage.FTM_SequencerHeader.SEQUENCE_STATUS = bIsQueueCompleted;
	LogMessage.FTM_SequencerHeader.COMMAND_STATUS = iCommandStatus;
	LogMessage.FTM_SequencerHeader.ID_NUMBER = iID_Number;
	LogMessage.FTM_SequencerHeader.EXPECTED_START_TIME = iExpectedStartTime;
	LogMessage.FTM_SequencerHeader.ACTUAL_START_TIME = iActualStartTime;
	LogMessage.FTM_SequencerHeader.TIME_OVERRUN = iTimeOverrun;
	LogMessage.FTM_SequencerHeader.EXECUTION_DURATION = iExecutionDuration;
	LogMessage.FTM_SequencerHeader.FTM_RESPONSE_SIZE = iResponsePacketSize;

	// Check size of message
    if ( iResponsePacketSize > sizeof(LogMessage.aLogMsgData) )
	{
		// Message is too large for diag

		// Log message
		FTM_Sequencer_Adaptation_AddDebugMessage
			( 
				iContextID, 
				"FTM_Sequencer_SendAsyncReponseMessage() -- response message is too large"
			);

		// Indicate the error
		_eErrorCode = FTM_Sequencer_Error_DiagnosticResponsePacketSizeExceeded;
	}
	else
	{
		// Log message
		FTM_Sequencer_Adaptation_AddDebugMessage
			( 
				iContextID, 
				"FTM Sequencer Response Message {SeqStatus = %d, CmdStatus = %d, ID = %d, Start = %d, duration = %d, Overrun = %d}",
				LogMessage.FTM_SequencerHeader.SEQUENCE_STATUS,
				LogMessage.FTM_SequencerHeader.COMMAND_STATUS,
				LogMessage.FTM_SequencerHeader.ID_NUMBER,
				LogMessage.FTM_SequencerHeader.ACTUAL_START_TIME,
				LogMessage.FTM_SequencerHeader.EXECUTION_DURATION,
				LogMessage.FTM_SequencerHeader.TIME_OVERRUN
			);


		// Copy the FTM response data into the storage space
		_eErrorCode = FTM_Sequencer_Adaptation_CopyMemory
					(
						iContextID,
						iResponsePacketSize,
						pResponseBytes,
						LogMessage.aLogMsgData
					);


		// Call the adaptation layer to transport the async message
		if ( _eErrorCode == FTM_Sequencer_Error_None )
		{
			_eErrorCode = FTM_Sequencer_Adaptation_SendAsyncPacketToClient
						(
						iContextID,
						&LogMessage,
						_iFullMessageLength
						);
		
		}	// if ( _eErrorCode == FTM_Sequencer_Error_None )


	}

	return _eErrorCode;
	
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_ProcessLoopIteration
				(
				FTM_Sequencer_ContextID iContextID
				)
{
	////////////////////////
	// Variables
	//////////////////////////

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Command execution code
	FTM_Sequencer_ErrorCode  _eCommandExecErrorCode = FTM_Sequencer_Error_None;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	// Elapsed time
	unsigned long _iElapsedTime = 0;
	unsigned long _iElapsedTimeBeforeCommand = 0;
	unsigned long _iElapsedTimeAfterCommand = 0;

	// Timing overrun--determined by the next WAIT command in the queue
	unsigned long iTimeOverrun = 0;


	// Storage for the next command item from the queue
	FTM_Sequencer_CommandItem* pCommandItem = FTM_SEQUENCER_NULL;

	// Queue size
	unsigned long _iQueueSize = 0;

	// FTM Header structure
	FTM_Sequencer_Header* _pFTM_SequencerHeader = FTM_SEQUENCER_NULL;


	// Add Wait request structure
	FTM_Sequencer_Request_AddWait* _pRequestWait= 
			(FTM_Sequencer_Request_AddWait*) _pFTM_SequencerHeader;

	// Boolean to track whether execution is determined to be completed
	unsigned char _bExecutionCompleted = FTM_SEQUENCER_FALSE;

	FTM_Sequencer_DefaultResponsePayload*    _pResponsePayload = FTM_SEQUENCER_NULL;

	unsigned long iTimeDeltaTemp = 0;

	////////////////////////
	// Code
	//////////////////////////

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{
		// Check execution state
		if ( _pState->isExecuting == FTM_SEQUENCER_TRUE )
		{
			// Check the next start time is >= the elapsed time
			_eErrorCode = FTM_Sequencer_Adaptation_GetElapsedTime
								( 
									iContextID,
									&_iElapsedTime
								);

			
			// Start the next command
			if (   ( _eErrorCode == FTM_Sequencer_Error_None )
				&& ( _iElapsedTime >= _pState->iNextStartTime )
				)
			{
				
				// Check the queue size
				_eErrorCode  =  FTM_SequencerQueue_GetItemCount
				(
					iContextID,
					&_iQueueSize
				);
				
				// If the queue has more than 1 item then process it
				if (
						( _eErrorCode == FTM_Sequencer_Error_None )
					&&	( _iQueueSize >= 1 )
					)
				{
					// Pull the next command off the queue
					_eErrorCode = 
								FTM_SequencerQueue_GetNextItem
								(
								iContextID,
								&pCommandItem
								);

					if ( _eErrorCode == FTM_Sequencer_Error_None )
					{

						FTM_Sequencer_Adaptation_AddDebugMessage
								( 
									iContextID, 
									"FTM_Sequencer_ProcessLoopIteration - executing item (0x%X) at elapsed time (%d), which was scheduled at (%d), delta is (%d) microseconds",
									pCommandItem->iID,
									_iElapsedTime,
									_pState->iNextStartTime,
									_iElapsedTime - _pState->iNextStartTime 
								);

						/*
							At this point there are two types commands to consider
							1) FTM Sequencer instructions (Add Wait)
							2) All other commands
						*/
						_pFTM_SequencerHeader = (FTM_Sequencer_Header*) pCommandItem->pCommand;

						// Record current time offset--before command execution
												FTM_Sequencer_Adaptation_GetElapsedTime
																		( 
																			iContextID,
																			&_iElapsedTimeBeforeCommand
																		);




						if ( FTM_Sequencer_IsFTM_SequencerCommand
									(
									(unsigned short) pCommandItem->iCommandSize,
									_pFTM_SequencerHeader
									) == FTM_SEQUENCER_TRUE
							)
						{
							_pState->ResponseBytes.Pkt_Len       = sizeof(FTM_Sequencer_DefaultResponsePayload);
                            _pState->ResponseBytes.RspCmd        = 2; // FTM_RSP_DO_NEW_PKT
                            _pState->ResponseBytes.DeletePayload = FTM_SEQUENCER_TRUE;

                            FTM_Sequencer_Adaptation_FTM_AllocateMemory
                            (
                            iContextID,
                            (unsigned long)FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE,
                            &_pState-> ResponseBytes.PayLoadPtr
                            );

                            if(_pState->ResponseBytes.PayLoadPtr != FTM_SEQUENCER_NULL)
							{
								_pResponsePayload = (FTM_Sequencer_DefaultResponsePayload*) _pState->ResponseBytes.PayLoadPtr;

                                // Fill in response payload header
                                _pResponsePayload->Header.CMD_ID           = _pFTM_SequencerHeader->CMD_ID;
                                _pResponsePayload->Header.SUBSYS_ID        = _pFTM_SequencerHeader->SUBSYS_ID;
                                _pResponsePayload->Header.RF_MODE_ID       = _pFTM_SequencerHeader->RF_MODE_ID;
                                _pResponsePayload->Header.RF_SUB_CMD       = _pFTM_SequencerHeader->RF_SUB_CMD;
                                _pResponsePayload->Header.FTM_DATA_LEN     = _pFTM_SequencerHeader->FTM_DATA_LEN;
                                _pResponsePayload->Header.FTM_RSP_PKT_SIZE = _pFTM_SequencerHeader->FTM_RSP_PKT_SIZE;
							}

							// Handle the WAIT
					        if ( _pFTM_SequencerHeader->RF_SUB_CMD == _FTM_SEQ_ADD_WAIT )
							{
								// Add Wait request
							    _pRequestWait= (FTM_Sequencer_Request_AddWait*) _pFTM_SequencerHeader;


							    // Set the time for the next wait
							    _pState->iNextStartTime = _pState->iNextStartTime + _pRequestWait->WAIT_TIME;

			                    // Log message
			                    FTM_Sequencer_Adaptation_AddDebugMessage
							    	  		  (
									    	  iContextID,
										      "FTM_SEQ_ADD_WAIT( WAIT_TIME = %d)",
											  _pRequestWait->WAIT_TIME
										      );

                                //Implement the adaptation Wait call                                

                                FTM_Sequencer_Adaptation_Wait
								(
							    iContextID,
								_pState->iNextStartTime
								);

								// See 80-N0528-2  "FTM Sequencer timing example"
                                if(_pState->iNextStartTime < _iElapsedTime)
                                {
									iTimeOverrun = 1;
									if( _pState->iTimingOverrunFirstCommandFlag == 0 )
									{
										_pState->TimingErrorInfo.Timing_Overrun_Status = 1;
                                        _pState->TimingErrorInfo.Cmd_ID_Timing_Overrun = pCommandItem->iID ;
										_pState->iTimingOverrunFirstCommandFlag = 1;
										FTM_Sequencer_Adaptation_AddDebugMessage
										(	 
											iContextID, 	
											"FTM Sequencer Timing Overrun - Timing_Overrun_Status = %d (Id_Number = %d)",
											_pState->TimingErrorInfo.Timing_Overrun_Status,
											_pState->TimingErrorInfo.Cmd_ID_Timing_Overrun
										);
									}
                                }

								FTM_SequencerQueue_SetResponseData
								(
									iContextID,
									pCommandItem,
									_pState->ResponseBytes.Pkt_Len,
									_pState->ResponseBytes.PayLoadPtr,
									(unsigned short) _eCommandExecErrorCode,
									_pState->iNextStartTime,
									_iElapsedTime,
									iTimeOverrun, 
									_pState->iNextStartTime - _iElapsedTime
							    );

								iTimeDeltaTemp = _iElapsedTime - _pState->iActualStartTimeLastCommand - _pState->iExecutionDurationLastCommand;
								if( iTimeDeltaTemp > _pState->TimingErrorInfo.Worse_Case_Time_Delta)
								{
									_pState->TimingErrorInfo.Worse_Case_Time_Delta = iTimeDeltaTemp;
									_pState->TimingErrorInfo.Cmd_ID_Worse_Case_Time_Delta  = pCommandItem->iID;
									FTM_Sequencer_Adaptation_AddDebugMessage
									(	 
										iContextID, 
										"FTM Sequencer Worst Case Time Delta -  Time_Delat_Worst_Case = %d (Id_Number = %d)",
										_pState->TimingErrorInfo.Worse_Case_Time_Delta,
										_pState->TimingErrorInfo.Cmd_ID_Worse_Case_Time_Delta
									);
								}
								_pState->iActualStartTimeLastCommand =  _iElapsedTime;
								_pState->iExecutionDurationLastCommand = _pState->iNextStartTime - _iElapsedTime;
							}

                            // Handle the ADD_TIMING_MARKER
                            else if ( _pFTM_SequencerHeader->RF_SUB_CMD == _FTM_SEQ_ADD_TIMING_MARKER )
							{
                                // Set the time for the next command, to start right now
                                _pState->iNextStartTime = _iElapsedTime;

			                   // Log message
			                   FTM_Sequencer_Adaptation_AddDebugMessage
				               ( 
					             iContextID, 
					             "FTM_SEQ_ADD_TIMING_MARKER"
				               );

							   FTM_SequencerQueue_SetResponseData
							   (
							     iContextID,
								 pCommandItem,
								 _pState->ResponseBytes.Pkt_Len,
								 _pState->ResponseBytes.PayLoadPtr,
								 (unsigned short) _eCommandExecErrorCode,
								 _pState->iNextStartTime,
								 0,
								 iTimeOverrun, 
								 0
							   ); 
							}
							else
							{
								// Every other FTM_SEQUENCER command is a failure during the sequence execution
							}

						}	// if ( FTM_Sequencer_IsFTM_SequencerCommand
						else
						{
							_pState->ResponseBytes.PayLoadPtr = NULL;


							//Set the FTM_SequencerState structure for use in FTM_Sequencer_Adaptation_Process_RspPkt()
							
							_pState->eCommandExecErrorCode = _eCommandExecErrorCode;
							_pState->pCommandItem = pCommandItem;
							_pState->iElapsedTime =  _iElapsedTime;		
							_pState->iElapsedTimeBeforeCommand = _iElapsedTimeBeforeCommand;
							_pState->iElapsedTimeAfterCommand = _iElapsedTimeAfterCommand;
							_pState->iTimeOverrun = iTimeOverrun;
							
							// Send to mobile
							_eCommandExecErrorCode = FTM_Sequencer_Adaptation_SendPacketToMobile
											(
											iContextID, 
											(unsigned short) pCommandItem->iCommandSize,
											pCommandItem->pCommand,
											&_pState->ResponseBytes
											);

							_pState->eCommandExecErrorCode = _eCommandExecErrorCode;

							if( _pState->ResponseBytes.RspCmd != FTM_RSP_NO_RSP)
							{
								//Sets Response packet structure 
								FTM_Sequencer_Adaptation_Process_RspPkt(iContextID, NULL);
							}
							else
							{
								return _eErrorCode;
							}

						}	// if ( FTM_Sequencer_IsFTM_SequencerCommand


						FTM_Sequencer_Adaptation_GetElapsedTime
																		( 
																			iContextID,
																			&_iElapsedTimeAfterCommand
																		);




						// Get the end of QUEUE status
						FTM_SequencerQueue_GetEndOfQueueStatus
						(
						iContextID,
						&_bExecutionCompleted
						);

						// Send async message for command completion
						FTM_Sequencer_SendAsyncReponseMessage
										(
										iContextID,
										_pState->ResponseBytes.Pkt_Len,
										(unsigned char *)_pState->ResponseBytes.PayLoadPtr,
										_bExecutionCompleted,
										_eCommandExecErrorCode, 
										pCommandItem->iID,
										_pState->iNextStartTime,
										_iElapsedTimeBeforeCommand,
										iTimeOverrun, 
										_iElapsedTimeAfterCommand - _iElapsedTimeBeforeCommand
										);

                        // Deallocate memory if the DeletePayload flag is set
                        if (( _pState->ResponseBytes.DeletePayload == FTM_SEQUENCER_TRUE )&&
                             ( _pState->ResponseBytes.PayLoadPtr   != FTM_SEQUENCER_NULL ))
					    {

							FTM_Sequencer_Adaptation_FTM_DeallocateMemory
							(
								iContextID,
								_pState->ResponseBytes.PayLoadPtr
							);
								
						}
					}	// if ( _eErrorCode == FTM_Sequencer_Error_None )
					else
					{
						// Error. The size was checked but an item
						// could not be pulled from queue succssfully
						FTM_Sequencer_Adaptation_AddDebugMessage
								( 
									iContextID, 
									"FTM_Sequencer_ProcessLoopIteration - failure to retrieve queue item when queue size was %d",
									_iQueueSize
								);


					}	// if ( _eErrorCode == FTM_Sequencer_Error_None )  after item count
				
				}	// ... &&	( _iQueueSize >= 1 )

				
			}	// if ( _iElapsedTime >= _pState->iNextStartTime )

         _eErrorCode = FTM_Sequencer_Adaptation_Stop_Sequence_Execution(iContextID);

			if(_eErrorCode != FTM_Sequencer_Error_None )
			{
            FTM_Sequencer_Adaptation_AddDebugMessage
							(
							iContextID,
	             "FTM_Sequencer_ProcessLoopIteration - End of queue not correctly proccessed (ErrorCode = %d)",
                _eErrorCode
							);
         }

			// Make sure completion message is sent under all conditions (including empty queue)
			if (
					( _pState->isExecuting == FTM_SEQUENCER_FALSE )
				&&	( !_pState->hasCompletionMessageBeenSent )
				)
			{
				// Send async message for completion

				// Set completion message flag
				_pState->hasCompletionMessageBeenSent = FTM_SEQUENCER_TRUE;

				FTM_Sequencer_Adaptation_AddDebugMessage
						( 
							iContextID, 
							"FTM_Sequencer_ProcessLoopIteration - completed execution at ElapsedTime = %d.",
							_iElapsedTime
						);

			}
		
		}	// if ( _pState->isExecuting == FTM_SEQUENCER_TRUE )
	}
	else
	{
		_eErrorCode = FTM_Sequencer_Error_InvalidState;
	}

	return _eErrorCode;

}

