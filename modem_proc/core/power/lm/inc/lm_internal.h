/*==============================================================================

  FILE:         lm_internal.h

  OVERVIEW:     This file provides declarations needed internal to LM module

  DEPENDENCIES: None

                Copyright (c) 2014 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: 
$DateTime: 
==============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "CoreVerify.h"
#include "CoreConfig.h"
#include "qurt.h"
#include "sleep_lpr.h"
#include "ULogFront.h"
#include "msmhwiobase.h"
#include "npa_resource.h"
#include "CoreMutex.h"
#include "CoreIni.h"
#include "npa.h"
#include "pmapp_npa.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
   
 /**
 * @brief Node Names
 * 
 */
 #define SLEEP_LPR_NODE_NAME "/sleep/lpr"
 #define VDD_CX_NODE "/pmic/client/rail_cx"
 
 #define LM_DEFAULT_LOG_NAME "LM log"
 #define LM_ULOG_DEFAULT_BUFFER_SIZE_BYTES 4096
 
 /* LM Mutexes to protect critical sections of the code  */
 #define LM_MUTEX_ACQUIRE() Core_MutexLock(gLMContext.lm_mutex_handle);
 #define LM_MUTEX_RELEASE() Core_MutexUnlock(gLMContext.lm_mutex_handle); 
 
 
 /**
 * MAX_SECTION_NAME_LEN 
 *  
 * @brief Max number of characters for a section name in the 
 *        lm ini file.
 */
#define MAX_SECTION_NAME_LEN  128

/**
 * MAX_INI_PATH_LEN
 *
 * @brief Max number of characters in the full path to the sleep
 *        ini file.
 */
#define MAX_INI_PATH_LEN           64
 
 
/* Global structure to store MPSS S1 LM driver context */
 typedef struct 
 {
    /* Pointer to the lock  */
    CoreMutexType *lm_mutex_handle;
	
	/* Ulog pointer */
	ULogHandle ulog_handle;
	
	/* Query Handle for "/pmic/client/rail_cx" resource */
    npa_query_handle cx_query_handle;
	
	/* Current state of cx node */
	npa_resource_state cx_state;
	
	/* Cx Change event handle */
	npa_event_handle cx_event_handle;
	
	/* CLient handle for lpr register */
	npa_client_handle sleep_lpr_client_handle;
	
	/* Final LM vote to Apps */
	uint32 lm_vote;
	
}LMContext;


 /* Structure to maintain s1lm CX value --> vote */
 typedef struct
 {
   /* Current cx npa state */
   uint32 cx_npa_state;
   
   /* Modem vote to apps according to s1lm table */
   uint32 lm_vote;
   
 }lm_table_type;
 
 
/* LM internal functions  */
void   lm_efs_initialize( void );
void   lm_apply_limits_vote(uint32 lm_vote);
uint32 lm_get_limts_vote( uint32 cx_state);
void   lm_remove_lmits_vote(void);
void   lm_send_interrupt_data(uint32 cx_state);
