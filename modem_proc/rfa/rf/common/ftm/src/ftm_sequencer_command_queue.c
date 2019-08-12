/******************************************************************************/
/**
Program: FTM Sequencer

  $Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_sequencer_command_queue.c#1 $

  \brief The FTM Sequencer Command Queue implementation

 \b QUALCOMM  \b PROPRIETARY 

This document contains propriety information, and except with written
permission of Qualcomm Technologies INC, such information shall not be 
published, or disclosed to others, or used for any purpose, and the   
document shall not be duplicated in whole or in part.  


\note   
Compiler:  Microsoft Visual C++ v6.0 SP4  
*******************************************************************************/

// FTM Sequencer files
#include "ftm_sequencer.h"
#include "ftm_sequencer_definitions.h"
#include "ftm_sequencer_commandqueue.h"
#include "ftm_sequencer_adaptation.h"
#include "ftm_sequencer_private_definitions.h"



/**
	A single item in a linked list
*/
typedef struct 
{
	FTM_Sequencer_CommandItem CommandItem;

	void* pNext;
}FTM_Sequencer_Command_Queue_Item;

/**
	The link list structure
*/
typedef struct
{
	// Start of linked list
	FTM_Sequencer_Command_Queue_Item*  pFTM_Sequencer_Queue_Head;

	// End of linked list
	FTM_Sequencer_Command_Queue_Item* pFTM_Sequencer_Queue_Tail;

	// Pointer into list
	FTM_Sequencer_Command_Queue_Item*  pFTM_Sequencer_Queue_CurrentItem;


	// Flag to indicate if queue is initalized
	unsigned char isQueueInitialized;
	
	// Store the last command sequence ID
	unsigned long iLastCommandSequenceID;

} FTM_Sequencer_CommandQueue;





/*****************************************************************************
	
	Support functions

*******************************************************************************/

/******************************************************************************/
/**

	Return the command queue pointer for a specified context ID
	Does not check the pointer value or state 

	\param iContextID = context identifier used to determine the FTM Sequencer
		                state data when calling the adaptation layer.
		                

\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	    Returns FTM_Sequencer_Error_General if it is not processed successfully.

*******************************************************************************/
FTM_Sequencer_ErrorCode  GetCommandQueueNoCheck
				(
				FTM_Sequencer_ContextID iContextID,
				FTM_Sequencer_CommandQueue** ppQueue
				)
{
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;


	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{
		// Assign the return value to the command queue in the sequencer state structure
		// Assignment of NULL is ok
		*ppQueue = (FTM_Sequencer_CommandQueue *)_pState->pCommandQueue;

		_eErrorCode = FTM_Sequencer_Error_None;
	}
	else
	{
		_eErrorCode = FTM_Sequencer_Error_InvalidState;
	}

	return _eErrorCode;
}


/******************************************************************************/
/**

	Return the command queue pointer for a specified context ID
	Checks the pointer value and state 

	\param iContextID = context identifier used to determine the FTM Sequencer
		                state data when calling the adaptation layer.
		                

\return Returns FTM_Sequencer_Error_None if the command is processed successfully, 
	    Returns FTM_Sequencer_Error_General if it is not processed successfully.

*******************************************************************************/
FTM_Sequencer_ErrorCode  GetCommandQueueWithCheck
				(
				FTM_Sequencer_ContextID iContextID,
				FTM_Sequencer_CommandQueue** ppQueue
				)
{

	// Call the base function
	FTM_Sequencer_ErrorCode  _eErrorCode = GetCommandQueueNoCheck
				(
				iContextID,
				ppQueue
				);

	// Error check
	if ( _eErrorCode == FTM_Sequencer_Error_None ) 
	{
		// Check for NULL
		if ( *ppQueue == FTM_SEQUENCER_NULL )
		{
			_eErrorCode = FTM_Sequencer_Error_InvalidCommandQueue;
		}
		// Check for uninitalized
		else if ( (*ppQueue)->isQueueInitialized != FTM_SEQUENCER_TRUE )
		{
			_eErrorCode = FTM_Sequencer_Error_CommandQueueNotInitialized;
		}
		else
		{
			// No error
			_eErrorCode = FTM_Sequencer_Error_None;
		
		}
	
	}

	return _eErrorCode;
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode NewQueueItem(

		FTM_Sequencer_ContextID iContextID,
		FTM_Sequencer_CommandItem* pInputCommandItem,
		FTM_Sequencer_Command_Queue_Item** pNewCommandQueueItem
		)
{

	FTM_Sequencer_Command_Queue_Item* pNewItem = FTM_SEQUENCER_NULL;
	void* pTempNewItem                         = FTM_SEQUENCER_NULL;
	
	// Allocate memory for a new FTM_Sequencer_Command_Queue_Item
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;
		
				
	_eErrorCode = FTM_Sequencer_Adaptation_AllocateMemory
				(
				iContextID,
				sizeof(FTM_Sequencer_Command_Queue_Item),
				&pTempNewItem
				);

	if ( _eErrorCode == FTM_Sequencer_Error_None )
	{
		pNewItem = (FTM_Sequencer_Command_Queue_Item*)pTempNewItem;
		// Setup the simple data types
		pNewItem->pNext = FTM_SEQUENCER_NULL;
		pNewItem->CommandItem.iID = pInputCommandItem->iID;
		pNewItem->CommandItem.iCommandSize = pInputCommandItem->iCommandSize;
		pNewItem->CommandItem.pResponse = FTM_SEQUENCER_NULL;
		pNewItem->CommandItem.iResponseSize = 0;

		// Deep copy of the data for the item
		_eErrorCode = FTM_Sequencer_Adaptation_AllocateMemory
					(
					iContextID,
					pNewItem->CommandItem.iCommandSize,
					(void **)&pNewItem->CommandItem.pCommand
					);

		// Copy the command data values
		if ( _eErrorCode == FTM_Sequencer_Error_None ) 
		{
			// Deep copy of the data for the item
			_eErrorCode = FTM_Sequencer_Adaptation_CopyMemory
						(
						iContextID,
						pInputCommandItem->iCommandSize,
						(void*) pInputCommandItem->pCommand,
						(void*) pNewItem->CommandItem.pCommand
						);

			if ( _eErrorCode == FTM_Sequencer_Error_None )
			{
				// Copy the pointer into the user-defined storage locaiton
				*pNewCommandQueueItem = pNewItem;
			}
		
		}	// if ( _eErrorCode == FTM_Sequencer_Error_None ) 

				
	} // if ( _eErrorCode == FTM_Sequencer_Error_None )

	
	return _eErrorCode;
}



/*****************************************************************************
	
	Interface functions

*******************************************************************************/



//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_SequencerQueue_Initialize
	(
		FTM_Sequencer_ContextID iContextID

	)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );


	// Check to see if the state pointer is NULL. If it is then no action can be
	// performed.
	if ( _pState != FTM_SEQUENCER_NULL )
	{
		// Make sure the command queue is NULL
		if (_pState->pCommandQueue == FTM_SEQUENCER_NULL ) 	
		{
			// Create a new FTM_Sequencer_CommandQueue structure
			_eErrorCode = FTM_Sequencer_Adaptation_AllocateMemory
					(
						iContextID,
						sizeof( FTM_Sequencer_CommandQueue ),
						&_pState->pCommandQueue
					);

			// Check error code
			if (_eErrorCode == FTM_Sequencer_Error_None ) 
			{
				// Get the queue pointer
				_eErrorCode = GetCommandQueueNoCheck( iContextID, &_pQueue );
				if ( _pQueue != FTM_SEQUENCER_NULL )
				{
					// Zero out the head pointer
					_pQueue->pFTM_Sequencer_Queue_Head = FTM_SEQUENCER_NULL;

					// Zero out the current pointer
					_pQueue->pFTM_Sequencer_Queue_CurrentItem = FTM_SEQUENCER_NULL;

					// Zero out the tail pointer
					_pQueue->pFTM_Sequencer_Queue_Tail = FTM_SEQUENCER_NULL;

					_pQueue->isQueueInitialized = FTM_SEQUENCER_TRUE;

					_pQueue->iLastCommandSequenceID = FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER;

				}
				else
				{
					_eErrorCode = FTM_Sequencer_Error_InvalidCommandQueue;

				}	// if ( _pQueue != FTM_SEQUENCER_NULL )

			}

		}
		else
		{
			// Command queue is already initialized
			_eErrorCode  = FTM_Sequencer_Error_CommandQueueAlreadyInitialized;

		}	// if (_pState->pCommandQueue == FTM_SEQUENCER_NULL ) 	
	
	}	// if ( _pQueue != FTM_SEQUENCER_NULL )
	else
	{
		// Invalid state pointer
		_eErrorCode  = FTM_Sequencer_Error_InvalidState;
	}
	
	return _eErrorCode;

}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_SequencerQueue_Uninitialize
	(
		FTM_Sequencer_ContextID iContextID

	)
{

	// Clear the queue first
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_SequencerQueue_EmptyQueue( iContextID );

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	// State of FTM Sequencer
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );


	// Check the current return value 
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{
		_eErrorCode = GetCommandQueueNoCheck( iContextID, &_pQueue );
		if ( _pQueue != FTM_SEQUENCER_NULL	)
		{

			// The storage memory will be freed when the EMPTY command is called

			// Indicate queue is not valid
			_pQueue->isQueueInitialized = FTM_SEQUENCER_FALSE;

			_pQueue->iLastCommandSequenceID = FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER;

			// Erase all items in queue
			FTM_SequencerQueue_EmptyQueue( iContextID );

			// Deallocate the memory for the queue structure
			if ( _pQueue != FTM_SEQUENCER_NULL )
			{
				_eErrorCode = FTM_Sequencer_Adaptation_DeallocateMemory
							(
							iContextID,
							_pQueue
							);		
			
			}


			// Check to see if the state pointer is NULL. If it is then no action can be
			// performed.
			if ( _pState != FTM_SEQUENCER_NULL )
			{
				_pState->pCommandQueue = FTM_SEQUENCER_NULL;
			}
			else
			{
				// Command queue is already initialized
				_eErrorCode  = FTM_Sequencer_Error_CommandQueueAlreadyInitialized;

			}	// if (_pState->pCommandQueue == FTM_SEQUENCER_NULL ) 	
			
		}
		else
		{
			_eErrorCode = FTM_Sequencer_Error_InvalidCommandQueue;
		}	// if ( _pQueue != FTM_SEQUENCER_NULL	)
	}


	return _eErrorCode;
}


//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_SequencerQueue_EmptyQueue
			(
			FTM_Sequencer_ContextID iContextID
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{
		FTM_Sequencer_Command_Queue_Item* _pItem = FTM_SEQUENCER_NULL;
		FTM_Sequencer_Command_Queue_Item* _pTemp = FTM_SEQUENCER_NULL;

		_pQueue->iLastCommandSequenceID = FTM_SEQUENCER_DEFAULT_COMMAND_ID_NUMBER;

		_pItem = _pQueue->pFTM_Sequencer_Queue_Head;

		while(_pItem != FTM_SEQUENCER_NULL )
		{
			// Clear the command data
			if ( _pItem->CommandItem.pCommand != FTM_SEQUENCER_NULL )
			{
				FTM_Sequencer_Adaptation_DeallocateMemory
							(
							iContextID,
							_pItem->CommandItem.pCommand
							);
				_pItem->CommandItem.pCommand = FTM_SEQUENCER_NULL;
			
			}

			// Clear the response data
			if ( _pItem->CommandItem.pResponse != FTM_SEQUENCER_NULL )
			{
				FTM_Sequencer_Adaptation_DeallocateMemory
							(
							iContextID,
							_pItem->CommandItem.pResponse
							);
				_pItem->CommandItem.pResponse = FTM_SEQUENCER_NULL;
			}
			_pItem->CommandItem.iResponseSize = 0;
			
			_pTemp = _pItem;
			_pItem = (FTM_Sequencer_Command_Queue_Item *)_pItem->pNext;

			// Clear the queue item
			if ( _pTemp != FTM_SEQUENCER_NULL )
			{
				FTM_Sequencer_Adaptation_DeallocateMemory
							(
							iContextID,
							_pTemp
							);

				_pTemp = FTM_SEQUENCER_NULL;
			}
		}

		// Reset the head/tail
		_pQueue->pFTM_Sequencer_Queue_Head = FTM_SEQUENCER_NULL;
		_pQueue->pFTM_Sequencer_Queue_CurrentItem = FTM_SEQUENCER_NULL;
		_pQueue->pFTM_Sequencer_Queue_Tail = FTM_SEQUENCER_NULL;

		// OK error code
		_eErrorCode = FTM_Sequencer_Error_None;
	

	}	// if ( _eErrorCode != FTM_Sequencer_Error_None)
		
	return _eErrorCode;
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_SequencerQueue_GetItemCount
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned long* piItemCount 
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	// Iterate through thte items
	FTM_Sequencer_Command_Queue_Item *pItem = FTM_SEQUENCER_NULL;

	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{


		// Store the initial return value
		*piItemCount = 0;

		// Check head for NULL
		if(_pQueue->pFTM_Sequencer_Queue_Head == FTM_SEQUENCER_NULL )
		{
			_eErrorCode = FTM_Sequencer_Error_None;
		}
		else
		{
			// Iterate through items...
			pItem = _pQueue->pFTM_Sequencer_Queue_Head;
			
			
			while(pItem != FTM_SEQUENCER_NULL )
			{
				// Increment the value
				*piItemCount  = (*piItemCount) + 1;

				// Go to the next item
				pItem = (FTM_Sequencer_Command_Queue_Item *)pItem->pNext;

			}	// while(pItem != FTM_SEQUENCER_NULL )

			_eErrorCode = FTM_Sequencer_Error_None;	
			
		} // if(_pQueue->pFTM_Sequencer_Queue_Head == FTM_SEQUENCER_NULL )

	}	// if ( _eErrorCode != FTM_Sequencer_Error_None)

	return _eErrorCode;
}


//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_SequencerQueue_AddItem
			(
			FTM_Sequencer_ContextID iContextID,
			FTM_Sequencer_CommandItem* pCommandItem
			)
{

	FTM_Sequencer_Command_Queue_Item* pNewItem = FTM_SEQUENCER_NULL;
	FTM_Sequencer_Command_Queue_Item* pTail = FTM_SEQUENCER_NULL;

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	// Current size
	unsigned long _iCurrentItemCount = 0;


	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{
		// Get the current size
		_eErrorCode = FTM_SequencerQueue_GetItemCount
				(
				iContextID,
				&_iCurrentItemCount
				);

		if ( _iCurrentItemCount < FTM_SEQUENCER_MAX_QUEUE_SIZE )
		{
			// Create the new item
			 _eErrorCode = NewQueueItem(
							iContextID, 
							pCommandItem,
							&pNewItem);

			if (_eErrorCode == FTM_Sequencer_Error_None ) 
			{
				// Map the new item into the linked list
				if(_pQueue->pFTM_Sequencer_Queue_Head == FTM_SEQUENCER_NULL)
				// The queue is empty
				{
				
					_pQueue->pFTM_Sequencer_Queue_Head = pNewItem;
					_pQueue->pFTM_Sequencer_Queue_CurrentItem = pNewItem;
					_pQueue->pFTM_Sequencer_Queue_Tail = pNewItem;
				
				}
				else
				// Add to tail
				{
		
					pTail = _pQueue->pFTM_Sequencer_Queue_Tail;
					
					pTail->pNext = pNewItem;

					_pQueue->pFTM_Sequencer_Queue_Tail = pNewItem;
				
				}	// if(_pQueue->pFTM_Sequencer_Queue_Head == FTM_SEQUENCER_NULL)

				// Increment the command sequence ID

				_pQueue->iLastCommandSequenceID++;
			}	// if (_eErrorCode == FTM_Sequencer_Error_None ) 

		
		}
		else
		{
			_eErrorCode = FTM_Sequencer_Error_CommandQueueMaxmimumSizeExceeded;
		}	// if ( _iCurrentItemCount < FTM_SEQUENCER_MAX_QUEUE_SIZE )


	}	// if ( _eErrorCode != FTM_Sequencer_Error_None)

	return _eErrorCode;




}



//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_SequencerQueue_GetNextItem
			(
			FTM_Sequencer_ContextID iContextID,
			FTM_Sequencer_CommandItem** pCommandItem
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{

		if(_pQueue->pFTM_Sequencer_Queue_CurrentItem == FTM_SEQUENCER_NULL)
		{
			_eErrorCode = FTM_Sequencer_Error_CommandQueueNoItems;
		}
		else
		{

			*pCommandItem = &(_pQueue->pFTM_Sequencer_Queue_CurrentItem->CommandItem);
					

			// Update the Current item to point to the next item
			// If the next item is NULL then the end of the list has been
			// reached
			_pQueue->pFTM_Sequencer_Queue_CurrentItem = (FTM_Sequencer_Command_Queue_Item *)_pQueue->pFTM_Sequencer_Queue_CurrentItem->pNext;

			
		}	// if(_pQueue->pFTM_Sequencer_Queue_Head != FTM_SEQUENCER_NULL)


	}	// if ( _eErrorCode != FTM_Sequencer_Error_None)

	return _eErrorCode;

}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_GetLastCommandSequenceIDFromQueue
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned long *iLastCommandSequenceID
			)
{	
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{
		*iLastCommandSequenceID = _pQueue->iLastCommandSequenceID;		
	}
	return _eErrorCode;
}


//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_SequencerQueue_GetEndOfQueueStatus
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned char* pbHasEndOfQueueBeenReached
			)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;


	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{
		// The end of the queue is reached if pNext is pointing to FTM_SEQUENCER_NULL
		*pbHasEndOfQueueBeenReached =  (unsigned char) (_pQueue->pFTM_Sequencer_Queue_CurrentItem == FTM_SEQUENCER_NULL );
	}


	return _eErrorCode;

}


//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_SequencerQueue_ResetIterator
			(
			FTM_Sequencer_ContextID iContextID
			)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;


	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{
		// The end of the queue is reached if pNext is pointing to FTM_SEQUENCER_NULL
		_pQueue->pFTM_Sequencer_Queue_CurrentItem = _pQueue->pFTM_Sequencer_Queue_Head;
	}


	return _eErrorCode;

}


//------------------------------------------------------------------------------------
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
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	// Iterate through thte items
	FTM_Sequencer_Command_Queue_Item *pItem = FTM_SEQUENCER_NULL;

	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{

		// Check head for NULL
		if(_pQueue->pFTM_Sequencer_Queue_Head == FTM_SEQUENCER_NULL )
		{
			_eErrorCode = FTM_Sequencer_Error_CommandQueueNotInitialized;
		}
		else
		{
			// Iterate through items...
			pItem = _pQueue->pFTM_Sequencer_Queue_Head;
			
			
			// Look for the user-supplied command item or the end of the list
			while(		( pItem != FTM_SEQUENCER_NULL )
					&&	( &pItem->CommandItem != pCommandItem )
				)
			{
				// Go to the next item
				pItem = (FTM_Sequencer_Command_Queue_Item *)pItem->pNext;

			}	// while(pItem != FTM_SEQUENCER_NULL )

			// Is a match found?
			if ( &pItem->CommandItem == pCommandItem  )
			{
				_eErrorCode = FTM_Sequencer_Error_None;	

				// Deallocate memory if the response pointer is not null
				if ( pItem->CommandItem.pResponse != FTM_SEQUENCER_NULL )
				{
				
					_eErrorCode = FTM_Sequencer_Adaptation_DeallocateMemory
								(
								iContextID,
								pItem->CommandItem.pResponse
								);		
					pItem->CommandItem.pResponse = FTM_SEQUENCER_NULL;
					pItem->CommandItem.iResponseSize = 0;
					pItem->CommandItem.iCommandStatus = 0;	
					pItem->CommandItem.iTimeInfo.iExpectedStartTime =0;
					pItem->CommandItem.iTimeInfo.iActualStartTime = 0;
					pItem->CommandItem.iTimeInfo.iTimeOverrun = 0;
					pItem->CommandItem.iTimeInfo.iExecutionDuration = 0;				
				}

				// Allocate space
				if ( iResponseBufferSize > FTM_SEQUENCER_MAX_DIAG_PACKET_SIZE ) 
				{
					_eErrorCode = FTM_Sequencer_Error_CommandItemMaxSizeExceeded;
				}
				else if ( _eErrorCode == FTM_Sequencer_Error_None )
				{
				
					_eErrorCode = FTM_Sequencer_Adaptation_AllocateMemory
								(
								iContextID,
								iResponseBufferSize,
								(void **)&pItem->CommandItem.pResponse
								);

					pItem->CommandItem.iResponseSize = iResponseBufferSize;

				}

				// Copy buffer
				if ( _eErrorCode == FTM_Sequencer_Error_None )
				{
					// Deep copy of the data for the item
					_eErrorCode = FTM_Sequencer_Adaptation_CopyMemory
								(
								iContextID,
								iResponseBufferSize,                   // size
								(void*) pResponseBuffer,               // source
								(void*) pItem->CommandItem.pResponse   // destination
								);
					pItem->CommandItem.iCommandStatus = iCommandStatus;
					pItem->CommandItem.iTimeInfo.iExpectedStartTime = iExpectedStartTime;
					pItem->CommandItem.iTimeInfo.iActualStartTime = iActualStartTime;
					pItem->CommandItem.iTimeInfo.iTimeOverrun = iTimeOverrun;
					pItem->CommandItem.iTimeInfo.iExecutionDuration = iExecutionDuration;				
				}
			}
			else
			{
				_eErrorCode = FTM_Sequencer_Error_CommandItemInvalid;	
			}

			
			
		} // if(_pQueue->pFTM_Sequencer_Queue_Head == FTM_SEQUENCER_NULL )

	}	// if ( _eErrorCode != FTM_Sequencer_Error_None)

	return _eErrorCode;

}


FTM_Sequencer_ErrorCode FTM_SequencerQueue_GetResponseData
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned long iID_Number,
			unsigned long* piResponseBufferSize,
			void** pResponseBuffer
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Get the queue pointer
	FTM_Sequencer_CommandQueue* _pQueue = FTM_SEQUENCER_NULL;

	// Iterate through thte items
	FTM_Sequencer_Command_Queue_Item *pItem = FTM_SEQUENCER_NULL;

	_eErrorCode = GetCommandQueueWithCheck( iContextID, &_pQueue );		
	if ( _eErrorCode == FTM_Sequencer_Error_None)
	{

		// Check head for NULL
		if(_pQueue->pFTM_Sequencer_Queue_Head == FTM_SEQUENCER_NULL )
		{
			_eErrorCode = FTM_Sequencer_Error_CommandQueueNotInitialized;
		}
		else
		{
			// Iterate through items...
			pItem = _pQueue->pFTM_Sequencer_Queue_Head;
			
			
			// Look for the user-supplied command item or the end of the list
			while(		( pItem != FTM_SEQUENCER_NULL )
					&&	( pItem->CommandItem.iID != iID_Number )
				)
			{
				// Go to the next item
				pItem = (FTM_Sequencer_Command_Queue_Item *)pItem->pNext;

			}	// while(pItem != FTM_SEQUENCER_NULL )

			if ( pItem != FTM_SEQUENCER_NULL )
			{

				// Is a match found?
				if ( pItem->CommandItem.iID == iID_Number )
				{
					_eErrorCode = FTM_Sequencer_Error_None;	

					// Set pointer to the response data
					*pResponseBuffer = pItem->CommandItem.pResponse;

					// Set pointer to the size
					*piResponseBufferSize = pItem->CommandItem.iResponseSize;
				}
				else
				{
					_eErrorCode = FTM_Sequencer_Error_CommandItemInvalid;	

				} // if ( pItem->CommandItem.iID == iID_Number )

			} // if ( pItem != FTM_SEQUENCER_NULL )
            else
			{
              _eErrorCode = FTM_Sequencer_Error_CommandQueueNoItems;
            }
			
		} // if(_pQueue->pFTM_Sequencer_Queue_Head == FTM_SEQUENCER_NULL )

	}	// if ( _eErrorCode != FTM_Sequencer_Error_None)

	return _eErrorCode;

}
