/******************************************************************************/
/**
Program: FTM Sequencer

	$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/ftm/inc/ftm_sequencer_private_definitions.h#1 $

	\brief Provides the private defintions used by the FTM Sequencer.

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

#if !defined(_FTM_SEQUENCER_PRIVATE_DEFINITIONS_H)
#define _FTM_SEQUENCER_PRIVATE_DEFINITIONS_H


/******************************************************************************
						Callback function definitions
*******************************************************************************/


/******************************************************************************
						Types
*******************************************************************************/



/******************************************************************************
						Structures
*******************************************************************************/


/**
	Structure of a processed queue item for the FTM Sequencer
*/
typedef struct
{
	unsigned long iID;        //!< ' Identifier number of the queue item
	unsigned long iSize;      //!< ' Size of the pResponse buffer, in bytes
	unsigned char* pResponse; //!< ' Pointer to the response data

	// !!! add response data

	unsigned long iExpectedTimeStart;       //!< ' Start time relative to T0
	unsigned long iActualTimeStart;         //!< ' Start time in microseconds
	unsigned long iActualTimeDuration;      //!< ' Duration in microseconds
	unsigned char bTimingOverrunDetected;   //!< ' Start time relative to T0

} FTM_Sequencer_ProcessedCommandItem;


#endif // !defined(_FTM_SEQUENCER_PRIVATE_DEFINITIONS_H)
