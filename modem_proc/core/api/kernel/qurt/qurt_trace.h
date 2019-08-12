#ifndef QURT_TRACE_H
#define QURT_TRACE_H
/**
  @file qurt_trace.h 
  @brief  Prototypes of system call tracing helpers API  

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2013  by Qualcomm Technologies, Inc.  All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

=============================================================================*/



/*=============================================================================
												GLOBAL VARIABLES
=============================================================================*/
/** @cond RTOS_user_guide_supplement */
/** @addtogroup etm_macros
@{ */
/* ETM trace types. */
#define QURT_ETM_TYPE_PC_ADDR                           (1<<0) /**< PC address.*/
#define QURT_ETM_TYPE_MEMORY_ADDR                       (1<<1) /**< Memory address. */
#define QURT_ETM_TYPE_TESTBUS                           (1<<2) /**< Test bus. */
#define QURT_ETM_TYPE_CYCLE_ACCURATE                    (1<<3) /**< Cycle accurate. */
#define QURT_ETM_TYPE_CYCLE_COARSE                      (1<<4) /**< Cycle coarse. */
#define QURT_ETM_TYPE_PC_AND_MEMORY_ADDR                (QURT_ETM_TYPE_PC_ADDR|QURT_ETM_TYPE_MEMORY_ADDR) /**< PC and memory address. */
#define QURT_ETM_TYPE_PC_ADDR_AND_TESTBUS               (QURT_ETM_TYPE_PC_ADDR|QURT_ETM_TYPE_TESTBUS) /**< PC address and test bus. */
#define QURT_ETM_TYPE_MEMORY_ADDR_AND_TESTBUS           (QURT_ETM_TYPE_MEMORY_ADDR|QURT_ETM_TYPE_TESTBUS) /**< Memory address and test bus.*/
#define QURT_ETM_TYPE_PC_AND_MEMORY_ADDR_AND_TESTBUS    (QURT_ETM_TYPE_PC_ADDR|QURT_ETM_TYPE_MEMORY_ADDR|QURT_ETM_TYPE_TESTBUS) /**< PC, memory address, and test bus. */

/* ETM routes. */
#define QURT_ETM_ROUTE_TO_QDSS      0 /**< ETM route to QDSS. */
#define QURT_ETM_ROUTE_TO_Q6ETB     1 /**< ETM route to Q6ETB. */

/* ETM filters. */
#define QURT_ETM_TRACE_FILTER_ALL_DEFAULT   0  /**< Filter all default. */
#define QURT_ETM_TRACE_FILTER_HNUM0         1  /**< Filter HNUM0. */    
#define QURT_ETM_TRACE_FILTER_HNUM1         2  /**< Filter HNUM1. */     
#define QURT_ETM_TRACE_FILTER_HNUM2         4  /**< Filter HNUM2. */     
#define QURT_ETM_TRACE_FILTER_HNUM3         8  /**< Filter HNUM3. */  
#define QURT_ETM_TRACE_FILTER_ALL           15 /**< Filter all. */

/* ETM Enable flags */
#define QURT_ETM_OFF                0  /**< ETM off. */
#define QURT_ETM_ON                 1  /**< ETM on. */

/* ETM setup return values */
#define QURT_ETM_SETUP_OK                   0
#define QURT_ETM_SETUP_ERR                  1

/* ETM breakpoint types */
#define QURT_ETM_READWRITE_BRKPT            0
#define QURT_ETM_READ_BRKPT                 1
#define QURT_ETM_WRITE_BRKPT                2
#define QURT_ETM_BRKPT_INVALIDATE           3

/** @} */ /* end_addtogroup etm_macros */
/** @endcond */
/*=============================================================================
												GLOBAL FUNCTIONS
=============================================================================*/


/**@ingroup func_qurt_trace_get_marker
  Gets the kernel trace marker.\n
  Returns the current value of the kernel trace marker.
  The marker consists of a hardware thread identifier and an index into the kernel trace
  buffer. The trace buffer records various kernel events.

  @note1hang This function is used with qurt_trace_changed() to
             determine whether certain kernel events occurred in a block of code.

  @return
  Integer -- Kernel trace marker.

  @dependencies
  None.
*/
unsigned int qurt_trace_get_marker(void);

/**@ingroup func_qurt_trace_changed  
  Determines whether specific kernel events have occurred. \n
  Returns a value indicating whether the specified kernel events have been recorded in the
  kernel trace buffer since the specified kernel trace marker was obtained.

  The prev_trace_marker parameter specifies a kernel trace marker which was obtained by calling 
  qurt_trace_get_marker().
  For more information on the mask value refer to the description of the trace_mask element in
   @xhyperref{Q3,[Q3]}. \n

  @note1hang This function is used with qurt_trace_get_marker to determine whether
             certain kernel events occurred in a block of code.\n
  @note1cont This function cannot determine whether a specific kernel event type has
             occurred unless that event type has been enabled in the trace_mask element
             of the system configuration file. \n
  @note1cont QuRT supports the recording of interrupt and context switch events only (i.e.,
             a trace_mask value of 0x3).

  @param[in] prev_trace_marker Previous kernel trace marker.
  @param[in] trace_mask        Mask value indicating the kernel events to check for.

  @returns
  1 -- Kernel events of the specified type have occurred since the
       specified trace marker was obtained.\n
  0 -- No kernel events of the specified type have occurred since the
       specified trace marker was obtained.

  @dependencies
  None.
*/
int qurt_trace_changed(unsigned int prev_trace_marker, unsigned int trace_mask);

/*=============================================================================
												CONSTANTS AND MACROS
=============================================================================*/
/** @addtogroup function_tracing_macro
@{ */
#ifndef QURT_DEBUG 
#define QURT_TRACE(str, ...) __VA_ARGS__
  /**< Function tracing is implemented with a debug macro (QURT_TRACE), that
       optionally generates printf statements both before and after every function call that is
       passed as a macro argument. 

       For example, the following macro call in the source code:
       @code
       QURT_TRACE(myfunc, my_func(33))
       
       @endcode
       generates the following debug output:
       @code
       myfile:nnn: my_func >>> calling my_func(33)
       myfile:nnn: my_func >>> returned my_func(33)
       @endcode
       The debug output includes the source file and line number of the function call, along with
       the text of the call itself.

       The debug output is generated using the library function qurt_printf.
       Generation of the debug output is controlled by the symbol QURT_DEBUG. If this symbol is
       not defined, function tracing is not generated.\n
       @note1hang The debug macro is accessed through the QuRT API header file. 
        */
#else
#define QURT_TRACE(str, ...) \
	do { \
		qurt_printf("%s:%d: %s: >>> calling %s\n",__FILE__,__LINE__,str,#__VA_ARGS__); \
		__VA_ARGS__; \
		qurt_printf("%s:%d: %s: <<< %s returned\n",__FILE__,__LINE__,str,#__VA_ARGS__); \
	} while (0);
#endif
/** @} */ /* end_addtogroup function_tracing_macro */


/**@ingroup func_qurt_etm_set_config
  Sets the configuration for the ETM. This operation specifies the types of program information 
  to trace, the destination of the trace information, and various filters to limit the amount 
  of trace information generated. This function must be called before enabling the ETM, 
  or after stopping the ETM.  

  @returns
    QURT_ETM_SETUP_OK -- Sucess. \n
    QURT_ETM_SETUP_ERR -- Failure.

  @dependencies
  None.
*/
unsigned int qurt_etm_set_config(unsigned int type, unsigned int route, unsigned int filter);


/**@ingroup func_qurt_etm_enable
  Enables or disables the ETM trace.
  The ETM is disabled by default, and must be configured before it is enabled.
  The default ETM configuration after system bootup is of type QURT_ETM_TYPE_PC_ADDR, 
  route QURT_ETM_ROUTE_TO_QDSS, and filter QURT_ETM_TRACE_FILTER_ALL. \n
  These values are applied to qurt_etm_enable() when no qurt_etm_set_config() operation is called.
  

  @param[in] enable_flag Values: \n
                         - QURT_ETM_ON \n
						 - QURT_ETM_OFF @tablebulletend 
      
  @returns
    QURT_ETM_SETUP_OK -- Success. \n
    QURT_ETM_SETUP_ERR -- Failure

  @dependencies
  None.
*/
unsigned int qurt_etm_enable(unsigned int enable_flag);


/**@ingroup func_qurt_etm_testbus_set_config
  Set configuration data for Hexagon ETM testbus

  @note1hang The ETM testbus design and configuration can be changed per Hexagon chipset

    @param[in] cfg_data    configuration data

  @returns
    QURT_ETM_SETUP_OK -- Sucess. \n
    QURT_ETM_SETUP_ERR -- Failure.

  @dependencies
  None.
*/
unsigned int qurt_etm_testbus_set_config(unsigned int cfg_data);


/**@ingroup func_qurt_etm_set_breakpoint
  Set a break-point at the specified memory access.
  The ETM interrupt is generated when the ETM hardware detects that there is memory access to 
  the specified address. The function supports detecting the memory access of memory read, 
  memory write, or memory read/write. And the ETM interrupt handler raises the NMI exception 
  after receiving the ETM interrupt. 

  @note1hang The maximum number of supported breakpoints (plus breakareas) is limited by 
             the ETM hardware, and the maximum number is 4 in Hexagon V5/V55/V56 cores.

    @param[in] type      operation types,
           QURT_ETM_READWRITE_BRKPT  
           QURT_ETM_READ_BRKPT    
           QURT_ETM_WRITE_BRKPT   
           QURT_ETM_BRKPT_INVALIDATE 
    @param[in] address   ETM breakpoint address
    @param[in] data      The 32-bit comparison data. When any bit in the comparison bitmask 
                         is set to 0, the corresponding bit in the comparison data has to be set 
                         to 0 as well. 
    @param[in] mask      The comparison bitmask. when a mask bit is set to 1, the corresponding 
                         comparison data bit has to be set to 0, and the bit is ignored at the data 
                         comparison.  When the mask is set to 0xffffffff, no data comparison will 
                         be performed at the breakpoint.

    @returns
           QURT_ETM_SETUP_OK
           QURT_ETM_SETUP_ERR 

    @dependencies
    None.
*/
 
unsigned int qurt_etm_set_breakpoint(unsigned int type, unsigned int address, unsigned int data, unsigned int mask);  
 

/**@ingroup func_qurt_etm_set_breakarea
  Set a break-area at the specified memory access.
  The ETM interrupt is generated when the ETM hardware detects that there is memory access to 
  the specified area from start_address to end_address. The function supports detecting 
  the memory access of memory read, memory write, or memory read/write. And the ETM interrupt 
  handler raises the NMI exception after receiving the ETM interrupt. 

  @note1hang The maximum number of supported breakpoints (plus breakareas) is limited by 
             the ETM hardware, and the maximum number is 4 in Hexagon V5/V55/V56 cores.

    @param[in] type      operation types,
           QURT_ETM_READWRITE_BRKPT  
           QURT_ETM_READ_BRKPT    
           QURT_ETM_WRITE_BRKPT   
           QURT_ETM_BRKPT_INVALIDATE 
    @param[in] start_address   start address of the memory area
    @param[in] end_address     end address of the memory area
    @param[in] count           number of the memory access before generating the ETM interrupt. 

    @returns
           QURT_ETM_SETUP_OK
           QURT_ETM_SETUP_ERR 

    @dependencies
    None.
*/
 
unsigned int qurt_etm_set_breakarea(unsigned int type, unsigned int start_address, unsigned int end_address, unsigned int count); 
 


#endif /* QURT_TRACE_H */
