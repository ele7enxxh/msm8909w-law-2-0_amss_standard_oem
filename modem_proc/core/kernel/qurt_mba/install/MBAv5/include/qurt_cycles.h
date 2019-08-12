
#ifndef QURT_CYCLES_H
#define QURT_CYCLES_H 1
/*======================================================================
                        qurt_cond.h

GENERAL DESCRIPTION
  Prototypes of Kernel pcycle API functions      

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_cycles.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/
	/*=====================================================================
	 Functions
	======================================================================*/
	 
/*======================================================================*/
/**
	FUNCTION				qurt_profile_reset_idle_pcycles 

	DESCRIPTION 	  Clear the counters maintained to collect the IDLE time.
	
	DEPENDENCIES 	None

	@return 				None 
		 
	SIDE EFFECTS		None

	Note			Since idle counter is no longer used exclusively by 
					Task Profiling and it is shared with other modules, 
					for eg PMU profiling, it becomes dangerous to allow 
					user to reset idle pcycle counter. 

					Therefore, qurt_profile_reset_idle_pcycles is deprecated. 

					The correct useage will be reading the idle counter when 
					profiling starts and ends. The idle pcycles will be the 
					delta of two readings.
*/
/* ======================================================================*/
void qurt_profile_reset_idle_pcycles (void);
	 
/*======================================================================*/
/**
	FUNCTION				qurt_profile_get_thread_pcycles

	DESCRIPTION 		Get pcycle that current thread has consumed..
	
	DEPENDENCIES 	None

	@return 				processor cycles that current thread has consumed
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
unsigned long long int qurt_profile_get_thread_pcycles(void);

/*======================================================================*/
/**
	FUNCTION				qurt_profile_get_thread_tcycles

	DESCRIPTION 		Get tcycle that current thread has consumed.
	
	DEPENDENCIES 	None

	@return 				thread cycles that current thread has consumed 
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
static inline unsigned long long int qurt_profile_get_thread_tcycles(void) 
{ 
        return qurt_profile_get_thread_pcycles()/3; 
}
	
/*======================================================================*/
/**
	FUNCTION				qurt_get_core_pcycles

	DESCRIPTION 		Get pcycles that has elapsed since the qdsp6 core is up  
	
	DEPENDENCIES 	None

	@return 				pcycles that has elapsed since the qdsp6 core is up 
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
unsigned long long int qurt_get_core_pcycles(void);

/*======================================================================*/
/**
	FUNCTION				qurt_profile_get_idle_pcycles 

	DESCRIPTION 	  Collects the IDLE time spent on each HW thread in Pcycles. 
	                This doesn't include ALL WAIT time, during with Q6 core clock
	                is turned off.
	
	  @param pcycles [OUT] is an array of MAX_HTREAD
	
	DEPENDENCIES 	None

	@return 				None
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
void qurt_profile_get_idle_pcycles (unsigned long long *pcycles);

/*======================================================================*/
/**
	FUNCTION				qurt_profile_get_threadid_pcycles 

	DESCRIPTION 	  Collects the run time of a SW thread on each HW thread in Pcycles.

	  @param thread_id [IN] is the qurt thread ID of interest
	  @param pcycles [OUT] is an array of MAX_HTREAD
	
	DEPENDENCIES 	None

	@return 				None 
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
void qurt_profile_get_threadid_pcycles (int thread_id, unsigned long long  *pcycles);

/*======================================================================*/
/**
	FUNCTION				qurt_profile_reset_threadid_pcycles 

	DESCRIPTION 	  Clear the counters maintained to collect the run time of
	                the SW thread.
	  @param thread_id [IN] is the qurt thread ID of interest
	
	DEPENDENCIES 	None

	@return 				None 
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
void qurt_profile_reset_threadid_pcycles (int thread_id);

/*======================================================================*/
/**
	FUNCTION				qurt_profile_enable 

	DESCRIPTION 		Enabled or Disable the profiling
	
		@param enable [IN] enable = 0, turns OFF profiling
		                   enable = 1, turns ON profiling
	
	DEPENDENCIES 	None

	@return 				None 
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
void qurt_profile_enable (int enable);

#endif

