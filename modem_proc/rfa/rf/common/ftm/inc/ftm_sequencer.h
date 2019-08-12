/******************************************************************************/
/**
Program: FTM Sequencer

	$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_sequencer.h#1 $

	\brief Provides the core functionality of the FTM Sequencer.

 \b QUALCOMM  \b PROPRIETARY

This document contains propriety information, and except with written
permission of Qualcomm Technologies INC, such information shall not be
published, or disclosed to others, or used for any purpose, and the
document shall not be duplicated in whole or in part.

\note
Compiler:  Microsoft Visual C++ v6.0 SP4
*******************************************************************************/


#include "ftm_sequencer_definitions.h"
#include "ftmdiag.h"

#if !defined(_FTM_SEQUENCER_H)
#define _FTM_SEQUENCER_H


#ifdef __cplusplus
extern "C" {
#endif 

	/*****************************************************************************
		Functions required in this layer:

		1) Initialize FTM Sequencer state information .
		2) Destroy all FTM Sequencer data (destruct)
		3) Process a request packet from the FTM Sequencer client
		4) Loop iteration to be called by the client thread for
		   processing the sequencer activities


		\warning The interface defined in ftm_sequencer_adaptation.h must 
		         exist in order to compile successfully.
	*******************************************************************************/
	

	/******************************************************************************/
	/**

	Initialize FTM Sequencer state information .

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning This function must be called before any other FTM Sequencer functions are called.
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_InitializeState
				(
				FTM_Sequencer_ContextID iContextID
				);

	/******************************************************************************/
	/**

    Initialize FTM Sequencer Timing Error Info parameters.

    \param iContextID = context identifier used to determine the FTM 
                            Sequencer state data when calling the
                            adaptation layer.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

    \warning This function must be called 
        1) While initializing FTM Sequencer State information
        2) Before the sequence starts execution.
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_InitializeTimingInfoState
				(
				FTM_Sequencer_ContextID iContextID
				);

	/******************************************************************************/
	/**

	Destroy all FTM Sequencer data 

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	\warning This function must be called before any other FTM Sequencer functions are called.
	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_Destructor
				(
				FTM_Sequencer_ContextID iContextID
				);

	/******************************************************************************/
	/**

	Process a request packet from the FTM Sequencer client

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    
		\param iRequestSize = Number of bytes to be sent in the request packet.

		\param piRequestBytes = Pointer to a buffer of request packet contents.
		                        The FTM Sequencer client must destroying this memory.

		\param piResponseSize = Pointer to number of bytes received in the response packet
		                        NOTE: when calling this function, the value must be equal to the
								maximum size of the receive buffer.  When the function returns, the
								value will be the actual number of bytes filled into the receive buffer.

		\param pResponseBytes = Double pointer (by reference pointer) to a buffer to store the 
		                        response packet contents.

		                        If this value is FTM_SEQUENCER_NULL then FTM Sequencer will allocate memory
								using FTM_Sequencer_Adaptation_AllocateMemory() and the client
								must free the memory.

                                If this value is not FTM_SEQUENCER_NULL then it must point to a memory buffer that is
								of size FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE. Because the memory buffer
								is provided by the client then the client must also destory the memory.

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_ProcessClientRequestPacket
				(
				FTM_Sequencer_ContextID       iContextID,
				unsigned short                iRequestPacketSize,
				unsigned char*                pRequestBytes,
				FTM_Sequencer_ResponsePacket* pResponseBytes
				);


	/******************************************************************************/
	/**

	Process a one loop iteration of the state machine

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.
		                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_ProcessLoopIteration
				(
				FTM_Sequencer_ContextID iContextID
				);

    /******************************************************************************/
    /** 
     
	Process the asynchronous message sent after each command

		\param iContextID = context identifier used to determine the FTM Sequencer
							state data when calling the adaptation layer.
							
		\param iResponseSize = Number of bytes received in the response packet

		\param pResponseBytes = Pointer to a buffer to store the response packet contents.
								The FTM Sequencer client must destroying this memory.
		\param bIsQueueCompleted = FTM_SEQUENCER_TRUE if the queue processing is completed. 
		                           FTM_SEQUENCER_FALSE otherwise.
		\param iCommandStatus = error code return from command execution (type FTM_Sequencer_ErrorCode)
		\param iID_Number = 	ID_NUMBER of the command assigned during the FTM_SEQ_QUEUE_COMMANDS command.
		\param iExpectedStartTimeOffset = expected start time offset in microseconds
		\param iActualStartTimeOffset   = actual start time offset in microseconds
		\param iExecutionDuration       = execution duration in microseconds

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
                      Returns FTM_Sequencer_Error_General if it is not processed successfully.
     
    **************************************************************************************/
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

                                );

#ifdef __cplusplus
}	// extern "C"
#endif 

#endif // !defined(_FTM_SEQUENCER_H)
