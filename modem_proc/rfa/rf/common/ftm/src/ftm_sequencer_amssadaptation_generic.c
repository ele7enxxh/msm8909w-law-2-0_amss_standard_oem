/******************************************************************************/
/**
Program: FTM Sequencer

  $Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/src/ftm_sequencer_amssadaptation_generic.c#1 $

  \brief Provides the AMSS Adaptation for the FTM Sequencer.

 \b QUALCOMM  \b PROPRIETARY 

This document contains propriety information, and except with written
permission of Qualcomm Technologies INC, such information shall not be 
published, or disclosed to others, or used for any purpose, and the   
document shall not be duplicated in whole or in part.  


\note   
Compiler:  Microsoft Visual C++ v6.0 SP4  
*******************************************************************************/

#include "rfa_variation.h"
#include "ftm_sequencer.h"
#include "ftm_sequencer_definitions.h"
#include "ftm_sequencer_adaptation.h"
#include "ftm_sequencer_commandqueue.h"
#include "rf_test_task_v.h"
#include "ftm_log.h"
#include "ftm_task.h"
#include "fs_public.h"
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#include "log_dmss.h"
#endif

#include <stdio.h>	// for the variable argument sprintf
#include <stdarg.h>	// for the variable arguments
#include <stringl/stringl.h>
#ifdef FEATURE_RF_RFCA
  #include <rfca.h>
#endif  

#ifdef FEATURE_GNSS_ONLY_NO_WWAN
#define FTM_SEQ_SIG 0x00400000
/* FTM Log Type
*/
/* LOG_FTM_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  dword fch_total_frames;       /* total count FCH frames */
  dword fch_fer;                /* total FCH frame error count */
  dword sch_total_frames;       /* total count of SCH frames */
  dword sch_fer;                /* total SCH frame error count */
  dword sch_num_dtx;            /* total SCH DTX count */
  /* Support FEATURE_FTM_DCCH */
  dword dcch_total_frames;      /* total count DCCH frames */
  dword dcch_fer;               /* total DCCH frame error count */
  dword dcch_num_dtx;           /* total DCCH DTX count */
  /* End of FEATURE_FTM_DCCH */
}
log_ftm_type;

/* LOG_FTM_GEN_C */
typedef PACKED struct
{
  log_hdr_type hdr;

  word         ftm_log_id;      /* FTM log id */
  byte         data[1];         /* Variable length payload,
                                   look at FTM log id for contents */
}
log_ftm_gen_type;
#endif

/******************************************************************************

						Constant Definitions
*******************************************************************************/
//! Directory to store a FTM sequence
const char QSEQ_DIR[] = "FTM_SEQ/";

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_AllocateMemory
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned long           iSize,
			void**                  pBuffer
			)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Allocate memory
	*pBuffer = ftm_malloc(iSize);

	// Report error
	if ( *pBuffer == FTM_SEQUENCER_NULL )
	{
		_eErrorCode = FTM_Sequencer_Error_MemoryAllocation;
        FTM_Sequencer_Adaptation_AddDebugMessage( iContextID, "FTM_Sequencer_Adaptation_AllocateMemory : Memory Allocation Failed");
	}
	else
	{
		// No error
		_eErrorCode = FTM_Sequencer_Error_None;
FTM_Sequencer_Adaptation_AddDebugMessage( iContextID, "FTM_Sequencer_Adaptation_AllocateMemory : Memory Allocation Success");
	}
	
	return _eErrorCode;

}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_FTM_AllocateMemory
			(
			FTM_Sequencer_ContextID iContextID,
			unsigned long           iSize,
			void**                  pBuffer
			)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;

	// Allocate memory from FTM Heap
	*pBuffer = (void*)ftm_malloc(iSize);

	// Report error
	if ( *pBuffer == FTM_SEQUENCER_NULL )
	{
		_eErrorCode = FTM_Sequencer_Error_MemoryAllocation;
        	FTM_Sequencer_Adaptation_AddDebugMessage( iContextID, "FTM_Sequencer_Adaptation_FTM_AllocateMemory : FTM Memory Allocation Failed");
	}
	else
	{
		// No error
		_eErrorCode = FTM_Sequencer_Error_None;
        	FTM_Sequencer_Adaptation_AddDebugMessage( iContextID, "FTM_Sequencer_Adaptation_FTM_AllocateMemory : FTM Memory Allocation Success");
	}
	
	return _eErrorCode;

}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_CopyMemory
			(
			FTM_Sequencer_ContextID iContextID,
				unsigned long iSize,
				void* pSourceBuffer,
				void* pDestinationBuffer
			)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_General;


	// Report error
	if (	( pSourceBuffer == FTM_SEQUENCER_NULL )
		||	( pDestinationBuffer == FTM_SEQUENCER_NULL )
		)
	{
		_eErrorCode = FTM_Sequencer_Error_MemoryCopy;
	}
	else
	{
		// Allocate memory
		memscpy(pDestinationBuffer, iSize,
                        pSourceBuffer, iSize );

		// No error
		_eErrorCode = FTM_Sequencer_Error_None;
	}
	
	return _eErrorCode;

}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_FTM_DeallocateMemory
			(
			FTM_Sequencer_ContextID iContextID,
			void* pBuffer
			)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Free memory
	ftm_free( pBuffer );
	
	return _eErrorCode;

}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_DeallocateMemory
			(
			FTM_Sequencer_ContextID iContextID,
			void* pBuffer
			)
{
	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Free memory
	ftm_free( pBuffer );
	
	return _eErrorCode;

}


//------------------------------------------------------------------------------------
void FTM_Sequencer_Adaptation_AddDebugMessage
			(
			FTM_Sequencer_ContextID iContextID,
			const char *format,
			...
			)
{

    static char _pStrBuffer[FTM_SEQUENCER_MAX_DEBUG_MESSAGE_SIZE];

    // make a va list
    va_list _vaList;

    // Initialize
    va_start( _vaList,format);

    // Perform the sprintf() to the buffer that includes 
    vsnprintf(_pStrBuffer, FTM_SEQUENCER_MAX_DEBUG_MESSAGE_SIZE, format, _vaList );

    // go to end
    va_end( _vaList );

    // Send to the final handler
    FTM_Sequencer_Adaptation_HandleDebugMessage
        (
            iContextID,
            _pStrBuffer
        );

}


//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_SetStartTime
			(
			FTM_Sequencer_ContextID iContextID
			)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_InvalidState;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	// Log message
	FTM_Sequencer_Adaptation_AddDebugMessage( iContextID, "FTM_Sequencer_Adaptation_SetStartTime");

	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{
		// Initialize the structure

		_pState->StartTime[0] = timetick_get();

		_pState->hasStartTimeBeenCaptured = FTM_SEQUENCER_TRUE;

		_eErrorCode = FTM_Sequencer_Error_None;

	}

	return _eErrorCode;
	
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_GetElapsedTime
				(
				FTM_Sequencer_ContextID iContextID,
				unsigned long* piElapsedTime
				)
{

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_InvalidState;

	// Get the state structure
	FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );

	// Current time


	// If !NULL then return the queue pointer
	if ( _pState != FTM_SEQUENCER_NULL )
	{
		if ( _pState->hasStartTimeBeenCaptured == FTM_SEQUENCER_TRUE )
		{
			// Calculate delta in us
			*piElapsedTime = timetick_get_elapsed (_pState->StartTime[0], T_USEC);

			_eErrorCode = FTM_Sequencer_Error_None;

		}
		else
		{
			_eErrorCode = FTM_Sequencer_Error_StartTimeNotSpecified;
		}

	}

	return _eErrorCode;

}


//------------------------------------------------------------------------------------
FTM_SequencerState* FTM_Sequencer_Adaptation_GetState
			(
			FTM_Sequencer_ContextID iContextID
			)
{

	// Declare a static instance of the FTM_FTM_SequencerState for all clients
	// to use as this adaptation is not multi threaded.
	static FTM_SequencerState SequencerState;
	return &SequencerState;
}

//------------------------------------------------------------------------------------
	FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_SendPacketToMobile
				(
				FTM_Sequencer_ContextID        iContextID,
				unsigned short                 iRequestPacketSize,
				unsigned char*                 pRequestBytes,
				FTM_Sequencer_ResponsePacket*  pResponseBytes
				)
{
	FTM_Sequencer_ErrorCode _eErrorCode = FTM_Sequencer_Error_General;
    unsigned short piResponsePacketSize = sizeof(FTM_Sequencer_ResponsePacket);

	_eErrorCode =  ftm_dispatch( iContextID,
 					             iRequestPacketSize,
							     pRequestBytes,
							     &piResponsePacketSize,
						         pResponseBytes
					            );

	return _eErrorCode;
}


//------------------------------------------------------------------------------------
void FTM_Sequencer_Adaptation_HandleDebugMessage
			(
			FTM_Sequencer_ContextID iContextID,
			char* pDebugString
			)
{
  MSG_SPRINTF_1(MSG_SSID_FTM, MSG_LEGACY_LOW, "%s", pDebugString);
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_SendAsyncPacketToClient
            (
			FTM_Sequencer_ContextID iContextID,
			FTM_SEQUENCER_FTM_SequencerLog* pLogMessage,
			unsigned short iMessageLength 
			)
{
	FTM_Sequencer_ErrorCode _eErrorCode = FTM_Sequencer_Error_None;

#ifdef FTM_HAS_LOGGING
    unsigned short payload_size;
    log_ftm_gen_type *log_ptr;
#endif

#ifdef FEATURE_RF_RFCA
      /* send log to RFCA task as well */
      RFCA_ftm_seq_log(pLogMessage);
#endif

   if (iMessageLength > sizeof(FTM_SEQUENCER_DiagLogHeader))
     {
#ifdef FTM_HAS_LOGGING
   /* create log packet to return results */
   payload_size = iMessageLength -sizeof(FTM_SEQUENCER_DiagLogHeader);
       log_ptr      = (log_ftm_gen_type*)ftm_log_malloc((uint16)FTM_LOG_SEQUENCER, (sizeof(ftm_log_gen_type)+ payload_size));

   if (log_ptr == NULL)
   {
      _eErrorCode = FTM_Sequencer_Error_MemoryAllocation;
   }
   else
   {
      /*lint -e(419) */  
      memscpy((void*)&(log_ptr->data[0]),payload_size,
             (void*)(&pLogMessage->FTM_SequencerHeader),payload_size);
      /* send log */
      log_commit( (ftm_log_type*)log_ptr );
   }
#endif /* FTM_HAS_LOGGING */
     }
   else
     {
       _eErrorCode = FTM_Sequencer_Error_MemoryAllocation;
     }
   return _eErrorCode;

}


FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_ExecutionNotification
			(
			   FTM_Sequencer_ContextID iContextID,
			   unsigned char bNewStateIsExecuting
			)
{
	FTM_Sequencer_ErrorCode _eErrorCode = FTM_Sequencer_Error_None;

	if (bNewStateIsExecuting == FTM_SEQUENCER_TRUE)
	{
	   (void) rex_set_sigs( &ftm_tcb, FTM_SEQ_SIG ); /* Signal to process FTM SEQ Q */
	}
	else
	{
	   (void) rex_clr_sigs(&ftm_tcb, FTM_SEQ_SIG);  /* Signal to stop processing FTM SEQ Q */
	}

	return _eErrorCode;
}


FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_Wait
            (
               FTM_Sequencer_ContextID iContextID,
			   unsigned long iNextStartTime
            )
{
	FTM_Sequencer_ErrorCode _eErrorCode = FTM_Sequencer_Error_None;

	unsigned long _iElapsedTime = 0;
	unsigned long _iMargin = 1000; // Amount of time to actually wake up, before requested to do so (in mircoseconds)
	unsigned long iSleep_ms = 0;

	// Get the elapsed time
	_eErrorCode = FTM_Sequencer_Adaptation_GetElapsedTime
                 ( 
                    iContextID,
                    &_iElapsedTime
                 );

	if((_iElapsedTime + _iMargin) < iNextStartTime ) 
	{
        //Convert Microseconds to ms

        iSleep_ms = ( iNextStartTime - _iElapsedTime - _iMargin ) / 1000;

		if(iSleep_ms > 0)
		{
		    if(iSleep_ms > 5)
            {
              iSleep_ms = 5;
            }
			rex_sleep(iSleep_ms);
		}
	}
	
	return _eErrorCode;
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_Store_Debug_Info_To_File
            (
            FTM_Sequencer_ContextID iContextID,
            unsigned char           FILE_NAME[MAX_FILE_NAME],
            unsigned short          FORMAT_VERSION
            )
{
   	////////////////////////
	// Variables
	//////////////////////////

    //! Full pathname leaving room for null terminator
    char PATH_NAME[sizeof(QSEQ_DIR)+MAX_FILE_NAME+1];

	// Return value
	FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

	// Storage for the next command item from the queue
	FTM_Sequencer_CommandItem* pCommandItem = FTM_SEQUENCER_NULL;

	// Boolean to track whether execution is determined to be completed
	unsigned char _bExecutionCompleted = FTM_SEQUENCER_FALSE;

	// Queue size
	unsigned long _iQueueSize = 0;

    struct fs_stat sbuf;

    int fs_handle             = 0;
    fs_ssize_t l_BytesWritten = 0;
    // Header information wrtten at the begining of the file
    FTM_Sequencer_File_Debug_Info_Header_Type FTM_Sequencer_File_Debug_Info_Header;

	////////////////////////
	// Code
	//////////////////////////

    if(efs_stat(QSEQ_DIR, &sbuf) != 0)
    {
        if(efs_mkdir(QSEQ_DIR, 0755) == -1)
        {
            _eErrorCode = FTM_Sequencer_Error_CannotCreateDir;
        }
    }

    if(_eErrorCode == FTM_Sequencer_Error_None)
    {
        if(strlen((const char *)FILE_NAME) > MAX_FILE_NAME)
        {
            _eErrorCode = FTM_Sequencer_InvalidFileName;
        }
        else
        {            
            strlcpy((char *)PATH_NAME,(char *)QSEQ_DIR,sizeof(PATH_NAME));
            strlcat((char *)PATH_NAME,(char *)FILE_NAME,sizeof(PATH_NAME));

            fs_handle = efs_open(PATH_NAME, O_RDWR | O_CREAT | O_TRUNC, 0755);
            
            if(fs_handle == -1)
            {
                _eErrorCode = FTM_Sequencer_Error_CannotOpenFile;
            }
        }
    }

    if( _eErrorCode == FTM_Sequencer_Error_None)
    {
  
        // Reset the command queue iterator
        FTM_SequencerQueue_ResetIterator( iContextID );
       
        // Check the queue size
        _eErrorCode  =  FTM_SequencerQueue_GetItemCount(
                                                       iContextID,
                                                       &_iQueueSize
                                                       );

        if( _eErrorCode == FTM_Sequencer_Error_None )
        {
            if(_iQueueSize == 0)
            {
                _eErrorCode = FTM_Sequencer_Error_CommandQueueNoItems;
            }
        }
        
        if( _eErrorCode == FTM_Sequencer_Error_None )
        {
            if(FORMAT_VERSION == FTM_SEQUENCER_FORMAT_VERSION_1)
            {
                FTM_Sequencer_File_Debug_Info_Header.FORMAT_VERSION = FORMAT_VERSION;
                FTM_Sequencer_File_Debug_Info_Header.NUM_ITEMS = _iQueueSize;

                // Write the header info 
                l_BytesWritten = efs_write(fs_handle, 
                                           &FTM_Sequencer_File_Debug_Info_Header,
                                           sizeof(FTM_Sequencer_File_Debug_Info_Header));

                if(l_BytesWritten != sizeof(FTM_Sequencer_File_Debug_Info_Header))
                {
                    _eErrorCode = FTM_Sequencer_EFS_WRITE_ERROR;
                }

                // Start writing FTM_Sequencer_CommandItem information to EFS File
                while( (_bExecutionCompleted == FTM_SEQUENCER_FALSE)  && (_eErrorCode == FTM_Sequencer_Error_None) )
                {
                    // Pull the next Command Item
                    _eErrorCode = FTM_SequencerQueue_GetNextItem(
                                                                iContextID,
                                                                &pCommandItem
                                                                );

                    if ( _eErrorCode == FTM_Sequencer_Error_None )
                    {
                        // Write Command ID to the file
                        l_BytesWritten = efs_write(fs_handle, 
                                                   &pCommandItem->iID,
                                                   sizeof(pCommandItem->iID));

                        if(l_BytesWritten != sizeof(pCommandItem->iID))
                        {
                            _eErrorCode = FTM_Sequencer_EFS_WRITE_ERROR;
                        }
                        else
                        {
                            // Write Command Status to the file

                            l_BytesWritten = efs_write(fs_handle, 
                                                      &pCommandItem->iCommandStatus,
                                                      sizeof(pCommandItem->iCommandStatus));

                            if(l_BytesWritten != sizeof(pCommandItem->iCommandStatus))
                            {
                                _eErrorCode = FTM_Sequencer_EFS_WRITE_ERROR;
                            }
                            else
                            {
                                // Write Command Timing info to the file
                                // For details refer to FTM_Sequencer_CommandItem_TimeInfo data structure

                                l_BytesWritten = efs_write(fs_handle, 
                                                           &pCommandItem->iTimeInfo,
                                                           sizeof(pCommandItem->iTimeInfo));

                                if(l_BytesWritten != sizeof(pCommandItem->iTimeInfo))
                                {
                                    _eErrorCode = FTM_Sequencer_EFS_WRITE_ERROR;
                                }
                                else
                                {
                                    // Write Command Request Packet Size to the file

                                    l_BytesWritten = efs_write(fs_handle, 
                                                               &pCommandItem->iCommandSize,
                                                               sizeof(pCommandItem->iCommandSize));

                                    if(l_BytesWritten != sizeof(pCommandItem->iCommandSize))
                                    {
                                        _eErrorCode = FTM_Sequencer_EFS_WRITE_ERROR;
                                    }
                                    else
                                    {
                                        // Write Command Request Packet Data to the file

                                        l_BytesWritten = efs_write(fs_handle, 
                                                                   pCommandItem->pCommand,
                                                                   pCommandItem->iCommandSize);

                                        if(l_BytesWritten != pCommandItem->iCommandSize)
                                        {
                                            _eErrorCode = FTM_Sequencer_EFS_WRITE_ERROR;
                                        }
                                        else
                                        {
                                            // Write Command Response Packet Size to the file

                                            l_BytesWritten = efs_write(fs_handle, 
                                                                       &pCommandItem->iResponseSize,
                                                                       sizeof(pCommandItem->iResponseSize));
                                            
                                            if(l_BytesWritten != sizeof(pCommandItem->iResponseSize))
                                            {
                                                _eErrorCode = FTM_Sequencer_EFS_WRITE_ERROR;
                                            }
                                            else
                                            {
                                                // Write Command Response Packet Data to the file

                                                l_BytesWritten = efs_write(fs_handle, 
                                                                           pCommandItem->pResponse,
                                                                           pCommandItem->iResponseSize);
                                            
                                                
                                                if(l_BytesWritten != pCommandItem->iResponseSize)
                                                {
                                                    _eErrorCode = FTM_Sequencer_EFS_WRITE_ERROR;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }

                    // Get the end of QUEUE status
                    FTM_SequencerQueue_GetEndOfQueueStatus
                    (
                        iContextID,
                        &_bExecutionCompleted
                    );
                } //while( (_bExecutionCompleted == FTM_SEQUENCER_FALSE)  && (_eErrorCode == FTM_Sequencer_Error_None) )
            } //if(FORMAT_VERSION == FTM_SEQUENCER_FORMAT_VERSION_1)
            else
            {
                _eErrorCode = FTM_Sequencer_FORMAT_VERSION_NOT_SUPPORTED;
            }
        }
    }

    if(fs_handle != -1)
    {
        efs_close(fs_handle);
    }

    return _eErrorCode;
}

//------------------------------------------------------------------------------------
FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_Process_RspPkt
				(
					FTM_Sequencer_ContextID iContextID,
                    void*      rsp_pkt
				)
{
 
    FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;
    
    unsigned char _bExecutionCompleted = FTM_SEQUENCER_FALSE;
    
    FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );
    
    unsigned long iTimeDeltaTemp = 0;
    
    
    // Check for sequencer execution state
    if ( _pState->isExecuting == FTM_SEQUENCER_TRUE )
    {
        if(rsp_pkt != NULL)
        {
            memscpy(&_pState->ResponseBytes, sizeof(FTM_Sequencer_ResponsePacket),
                    rsp_pkt , sizeof(FTM_Sequencer_ResponsePacket) );
        }

        // Record current time offset--after command execution
        FTM_Sequencer_Adaptation_GetElapsedTime
                ( 
                    iContextID,
                    &_pState->iElapsedTimeAfterCommand
                );	
        
        // Push the response data back into the command queue item
        FTM_SequencerQueue_SetResponseData
                (
                    iContextID,
                    _pState->pCommandItem,
                    _pState->ResponseBytes.Pkt_Len,
                    _pState->ResponseBytes.PayLoadPtr,
                    (unsigned short) _pState->eCommandExecErrorCode,
                    _pState->iNextStartTime,
                    _pState->iElapsedTimeBeforeCommand,
                    _pState->iTimeOverrun,
                    _pState->iElapsedTimeAfterCommand - _pState->iElapsedTimeBeforeCommand
                );

        if( _pState->iTimingOverrunFirstCommandFlag == 0 && _pState->iTimeOverrun == 1 )
        {
            _pState->TimingErrorInfo.Timing_Overrun_Status = 1;
            _pState->TimingErrorInfo.Cmd_ID_Timing_Overrun = _pState->pCommandItem->iID;
            _pState->iTimingOverrunFirstCommandFlag = 1;
        }

        iTimeDeltaTemp = _pState->iElapsedTimeBeforeCommand - _pState->iActualStartTimeLastCommand - _pState->iExecutionDurationLastCommand;
        if( iTimeDeltaTemp > _pState->TimingErrorInfo.Worse_Case_Time_Delta)
        {
            _pState->TimingErrorInfo.Worse_Case_Time_Delta = iTimeDeltaTemp;
            _pState->TimingErrorInfo.Cmd_ID_Worse_Case_Time_Delta  = _pState->pCommandItem->iID;
            FTM_Sequencer_Adaptation_AddDebugMessage
            (	 
                iContextID, 
                "FTM Sequencer Worst Case Time Delta -  Time_Delat_Worst_Case = %d (Id_Number = %d)",
                _pState->TimingErrorInfo.Worse_Case_Time_Delta,
                _pState->TimingErrorInfo.Cmd_ID_Worse_Case_Time_Delta
            );
        }
        _pState->iActualStartTimeLastCommand =  _pState->iElapsedTimeBeforeCommand;
        _pState->iExecutionDurationLastCommand = _pState->iElapsedTimeAfterCommand - _pState->iElapsedTimeBeforeCommand;

        // For all task based commands
        if(rsp_pkt != NULL) 
        {
        
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
                        (unsigned char *) _pState->ResponseBytes.PayLoadPtr,
                        _bExecutionCompleted,
                        _pState->eCommandExecErrorCode,
                        _pState->pCommandItem->iID,
                        _pState->iNextStartTime,
                        _pState->iElapsedTimeBeforeCommand,
                        _pState->iTimeOverrun,
                        _pState->iElapsedTimeAfterCommand - _pState->iElapsedTimeBeforeCommand
                    );
		
            // Deallocate memory if the DeletePayload flag is set
            if (( _pState->ResponseBytes.DeletePayload == FTM_SEQUENCER_TRUE )&&
                ( _pState->ResponseBytes.PayLoadPtr != FTM_SEQUENCER_NULL ))
            {
                FTM_Sequencer_Adaptation_FTM_DeallocateMemory
                    (
                        iContextID,
                        _pState->ResponseBytes.PayLoadPtr
                    );					
            }
        }
    }

    return _eErrorCode;
}

//------------------------------------------------------------------------------------

FTM_Sequencer_ErrorCode FTM_Sequencer_Adaptation_Stop_Sequence_Execution
				(
					FTM_Sequencer_ContextID iContextID
				)
{
    FTM_Sequencer_ErrorCode  _eErrorCode = FTM_Sequencer_Error_None;

    unsigned char _bExecutionCompleted = FTM_SEQUENCER_FALSE;

    FTM_SequencerState* _pState = FTM_Sequencer_Adaptation_GetState ( iContextID );
     
    // Check for sequencer execution state
    if ( _pState->isExecuting == FTM_SEQUENCER_TRUE )
    {
        // Get the end of QUEUE status
        _eErrorCode = FTM_SequencerQueue_GetEndOfQueueStatus
                        (
                            iContextID,
                            &_bExecutionCompleted
                         );

        if(_eErrorCode != FTM_Sequencer_Error_None )
        {
           FTM_Sequencer_Adaptation_AddDebugMessage
           ( 
               iContextID, 
	            "FTM_Sequencer_Adaptation_Stop_Sequence_Execution - End of queue not correctly proccessed (ErrorCode = %d)",
               _eErrorCode
	       );
        }
        else
        {
            if ( _bExecutionCompleted )
            {
                // Set the state variable to that holds the execution state
                _pState->isExecuting = FTM_SEQUENCER_FALSE;

                _pState->hasStartTimeBeenCaptured = FTM_SEQUENCER_FALSE;
			
                // Notify the adaptation layer
                FTM_Sequencer_Adaptation_ExecutionNotification
                    (
                        iContextID,
                        FTM_SEQUENCER_FALSE 
                    );                
	
            } // if ( bExecutionComplete )
        } //if(_eErrorCode != FTM_Sequencer_Error_None )
    } // if ( _pState->isExecuting == FTM_SEQUENCER_TRUE )

    return _eErrorCode; 
}
