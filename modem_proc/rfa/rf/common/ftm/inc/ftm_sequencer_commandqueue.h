/******************************************************************************/
/**
Program: FTM Sequencer

	$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_sequencer_commandqueue.h#1 $

	\brief Provides the queue functionality of the FTM Sequencer.

 \b QUALCOMM  \b PROPRIETARY

This document contains propriety information, and except with written
permission of Qualcomm Technologies INC, such information shall not be
published, or disclosed to others, or used for any purpose, and the
document shall not be duplicated in whole or in part.

\note
Compiler:  Microsoft Visual C++ v6.0 SP4
*******************************************************************************/

#if !defined(_FTM_SEQUENCER_QUEUE_H)
#define _FTM_SEQUENCER_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif 

	/*****************************************************************************
		
		The FTM Sequencer queue is a first-in-first-out list of items of the
		type FTM_Sequencer_CommandItem.

		Each item has a data buffer that is copied when it is placed into the queue
		and 
		  
		Functions required in this layer:
        1) Initialize the state of the queue
		2) Un-initilaize the state of the queue
		3) Clear the queue items
		4) Get the queue item count
		5) Add an item to the queue
		6) Get the next item in the queue


	*******************************************************************************/

	/******************************************************************************/
	/**

	Initialize the queue

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		
		                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_Initialize
		(
			FTM_Sequencer_ContextID iContextID

		);

	/******************************************************************************/
	/**

	Add an item to the queue
		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

	                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_Uninitialize
		(
			FTM_Sequencer_ContextID iContextID
		);



	/******************************************************************************/
	/**

	Empty the queue.  The memory allocated for queue item will be deallocated.

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

	                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_EmptyQueue
				(
				FTM_Sequencer_ContextID iContextID
				);

	/******************************************************************************/
	/**

	Get the queue item count (number of items in the queue)

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param piItemCount = pointer to storage location of item count
		                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_GetItemCount
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned long* piItemCount 
				);

	/******************************************************************************/
	/**

	Add an item to the queue

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param CommandItem = Item to the queue.  The memory pointed to by the queue
		                     is not managed by the sequencer
		                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_AddItem
				(
				FTM_Sequencer_ContextID iContextID,
				FTM_Sequencer_CommandItem* pCommandItem
				);

	/******************************************************************************/
	/**

	Get the next item in the queue

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param pCommandItem = Double pointer to the storage location for the next command item
		                      reference. This data will be managed be the queue functions
							  and the user should not modify it at all.
		                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.
			Returns FTM_Sequencer_Error_CommandQueueNoItems if no items are remaining

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_GetNextItem
				(
				FTM_Sequencer_ContextID iContextID,
				FTM_Sequencer_CommandItem** pCommandItem
				);


	/******************************************************************************/
	/**

	Return the ID_NUMBER of the last Command Queue item.

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param piLastCommandSequenceID = pointer to the storage for the ID_NUMBER.
		                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.
			Returns FTM_Sequencer_Error_CommandQueueNoItems if no items are remaining

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_Sequencer_GetLastCommandSequenceIDFromQueue
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned long *piLastCommandSequenceID
			);

	/******************************************************************************/
	/**

	Return the state of the command queue iterator: whether or not it has reached the 
	end of the queue

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param pbHasEndOfQueueBeenReached = FTM_SEQUENCER_TRUE if the end of the queue
		                         has bee reached. Otherwise FTM_SEQUENCER_FALSE
		                    

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.
			Returns FTM_Sequencer_Error_CommandQueueNoItems if no items are remaining

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_GetEndOfQueueStatus
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned char* pbHasEndOfQueueBeenReached
				);

	/******************************************************************************/
	/**

	Reset the interator to the beginning of the queue.

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.


	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.
			Returns FTM_Sequencer_Error_CommandQueueNoItems if no items are remaining

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_ResetIterator
				(
				FTM_Sequencer_ContextID iContextID
				);



	/******************************************************************************/
	/**

	Set the response data for a certain queue item

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param pCommandItem = pointer to the command item to be modified
		\param iResponseBufferSize = number of bytes in the response buffer
        \param pResponseBuffer = pointer to the reponse buffer
        \param iCommandStatus = Error code return from command
               execution
        \param iExpectedStartTime = Expected start time offset, relative to the start of 
                                  the sequence execution in microseconds
        \param iActualStartTime = Actual start time offset, relative to the start of 
                                  the sequence execution in microseconds
        \param iTimeOverrun = Timeoverrun is a flag that applies only to ADD_WAIT commands.
                              It indicates if the previously executed commands took more time 
                              that the ADD_WAIT was budgeted for.
        \param iExecutionDuration = Execution duration in
               microseconds

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.
			Returns FTM_Sequencer_Error_CommandQueueNoItems if no items are remaining

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_SetResponseData
				(
				FTM_Sequencer_ContextID iContextID,
				FTM_Sequencer_CommandItem* pCommandItem,
				unsigned long iResponseBufferSize,
				void* pResponseBuffer,
                unsigned short iCommandStatus,
                unsigned long iExpectedStartTime,
                unsigned long iActualStartTime,  
                unsigned long iTimeOverrun,    
                unsigned long iExecutionDuration
				);

	/******************************************************************************/
	/**

	Get the response data for a certain queue item

		\param iContextID = context identifier used to determine the FTM Sequencer
		                    state data when calling the adaptation layer.

		\param iID_Number = ID_NUMBER of the item to be returned
		\param piResponseBufferSize = output, number of bytes in the response buffer
		\param pResponseBuffer = output, pointer to the reponse buffer

	\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	        Returns FTM_Sequencer_Error_General if it is not processed successfully.
			Returns FTM_Sequencer_Error_CommandQueueNoItems if no items are remaining

	*******************************************************************************/
	FTM_Sequencer_ErrorCode FTM_SequencerQueue_GetResponseData
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned long iID_Number,
				unsigned long* piResponseBufferSize,
				void** pResponseBuffer
				);


#ifdef __cplusplus
}	// extern "C"
#endif 

#endif // !defined(_FTM_SEQUENCER_QUEUE_H)
