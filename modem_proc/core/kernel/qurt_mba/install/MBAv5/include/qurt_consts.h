#ifndef QURT_CONSTS_H
#define QURT_CONSTS_H

/*======================================================================

                        qurt_defs.h 

GENERAL DESCRIPTION
  QURT constant and definitions 

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
 
	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_consts.h#1 $ 
	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
	when				who 		what, where, why
	----------	--- 		------------------------------------------------
	2011-02-25	op			Add Header file
 ======================================================================*/

/*=====================================================================
 Constants and macros
 ======================================================================*/

/* Definitions of system events. System events will cause 
 * a thread to be suspended and put into suspending_list.
 * The system event number will be saved in CONTEXT::error::cause field
 * of the suspended thread. An event handler thread such as
 * page fault handler, system error handler may wake up the suspended
 * thread.
 */
#define QURT_EVENT_PAGEFAULT      0x1 /**< Page fault Event */
#define QURT_EVENT_SYSTEM_ERR     0x2 /**< System Error Event */
#define QURT_EVENT_SUSPEND        0x3
#define QURT_EVENT_PROCESS_EXIT   0x4 /*process termination event*/

#define QURT_SYSENV_MAX_THREADS_TYPE           1 /**< max threads object */
#define QURT_SYSENV_PROCNAME_TYPE              2 /**< process name object */
#define QURT_SYSENV_MAX_PI_PRIO_TYPE           3 /**< max pi prio object */
#define QURT_SYSENV_VERSION_TYPE               4 /**< Architecture version object */
#define QURT_SYSENV_SWAP_POOL0_TYPE            5 /**< Swap spool object */
#define QURT_SYSENV_SWAP_POOL1_TYPE            6 /**< Swap spool object */
#define QURT_SYSENV_APP_HEAP_TYPE              7 /**< Application heap object */
#define QURT_SYSENV_TIMER_HW_ATTR_TYPE         8 /**< Timer object */
#define QURT_SYSENV_TIMER_SERV_ATTR_TYPE       9 
#define QURT_SYSENV_TIMER_IST_ATTR_TYPE        10 


/** 
 PMU  
*/
#define QURT_PMUCNT0    0
#define QURT_PMUCNT1    1
#define QURT_PMUCNT2    2
#define QURT_PMUCNT3    3
#define QURT_PMUCFG     4
#define QURT_PMUEVTCFG  5

/** 
 Power collapse operation 
*/
#define QURT_POWER_SHUTDOWN       0
#define QURT_TCXO_SHUTDOWN        1
#define QURT_POWER_CMD_PREPARE    0
#define QURT_POWER_CMD_PERFORM    1
#define QURT_POWER_CMD_EXIT       2
#define QURT_POWER_CMD_FAIL_EXIT  3
#define QURT_POWER_CMD_PERFORM_L2_RETENTION 4

/**
 The limit on the Maximum no. of HW threads supported by QURT for any
 Hexagon version. This definition can be used to define arrays etc. in
 target independent code.
 */
#define QURT_MAX_HTHREAD_LIMIT    6

/**
  L2 cache retention mode
*/
#define QURT_POWER_SHUTDOWN_TYPE_L2NORET QURT_POWER_CMD_PERFORM
#define QURT_POWER_SHUTDOWN_TYPE_L2RET   QURT_POWER_CMD_PERFORM_L2_RETENTION

/** 
  QURT_system_state - power collapse stage
  use for debuging the shutdown/startup process
*/
#define QURT_PREPARE_SINGLE_MODE 1
#define QURT_PREPARE_END 2
#define QURT_PERFORM_IPEND 3
#define QURT_PERFORM_SAVE_ISDP 4
#define QURT_PERFORM_SAVE_PMU 5
#define QURT_PERFORM_SAVE_TLB 6
#define QURT_PERFORM_SWITCH_PC 7
#define QURT_PERFORM_EXIT 8
#define QURT_FLUSH_L1CACHE 9
#define QURT_FLUSH_L2CACHE 10
#define QURT_FLUSH_CACHE_DONE 11
#define QURT_SWITCH_PC_DONE 12
#define QURT_BOOT_SETUP_ISDP 13
#define QURT_WBOOT_INIT_TLB 14
#define QURT_WBOOT_SET_1TO1_MAP 15
#define QURT_WBOOT_CFG_ADV_SYSCFG 16
#define QURT_WBOOT_REMOVE_1TO1_MAP 17
#define QURT_CBOOT_BSP_INIT 18
#define QURT_CBOOT_END_CLEAN_L1CACHE 19
#define QURT_CBOOT_END_CLEAN_INIT 20
#define QURT_CBOOT_END_OS_INIT 21


/**
  Thread attributes
*/

#define QURT_THREAD_ATTR_GP              0x00000002
#define QURT_THREAD_ATTR_UGP             0x00000003
#define QURT_THREAD_ATTR_PREFETCH        0x00000004
#define QURT_THREAD_ATTR_TID             0x00000005
#define QURT_THREAD_ATTR_L1IC_PART       0x00000007
#define QURT_THREAD_ATTR_L1DC_PART       0x00000008
#define QURT_THREAD_ATTR_L2C_PART        0x00000009

/**  Cache operations*/
#define QURT_DCCLEAN       0   /**< clean Dcache */ 
#define QURT_DCINV         1   /**< invalidate Dcache */
#define QURT_DCCLEANINV    2   /**< clean and invalidate Dcache */
#define QURT_ICINV         3   /**< invalidate Icache */
#define QURT_DUMP_DCTAGS   4  /**< This is for testing purpose */
#define QURT_FLUSH_ALL     5  /**< Flush L1 and L2 cache  */

/** QURT API version */
#define QURT_API_VERSION    3

/** Process state **/
#define QURT_UPDATE_PROCESS_STATE   0
#define QURT_MP_INIT        1
#define QURT_MP_RUNNING     2
#define QURT_MP_STOPPED     3

/** Qurt Reset Reason */
#define QURT_NORMAL_BOOT               0  /**<Normal Boot */
#define QURT_WARM_BOOT                 1  /**<Power collapse Warm Boot */
#define QURT_WARM_BOOT_L2_RETENTION    2  /**<Power collapse with L2  Retention Warml Boot */ 

#define QURT_MAX_NAME_LEN   32

#endif /* QURT_CONSTS_H */
