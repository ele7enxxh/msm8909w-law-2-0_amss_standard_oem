#ifndef QURT_PMU_H
#define QURT_PMU_H
/*=============================================================================

                 qurt_pmu.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of pipe interface API  
	 This is a pipe or message queue
	 It will block if too full (send) or empty (receive)
	 Unless you use a nonblocking option
	 All datagrams are 64 bits.

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_pmu.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/

/*=============================================================================
												FUNCTIONS
=============================================================================*/

/**
 * Set (write) PMU register
 *
 * reg_id    - QURT_PMUCNT0 : PMUCNT0 register
 *             QURT_PMUCNT1 : PMUCNT1 register
 *             QURT_PMUCNT2 : PMUCNT2 register
 *             QURT_PMUCNT3 : PMUCNT3 register
 *             QURT_PMUCNT4 : PMUCFG register
 *             QURT_PMUCNT5 : PMUEVTCFG register
 *
 * reg_value - value to be written to the corresponding register
 *
 * @return void
 *
 * Note      - writing PMUEVTCFG will clear PMUCNT0..3 counters.
 */
void qurt_pmu_set (int reg_id, unsigned int reg_value);
 
/**
 * Get (read) PMU register.
 *
 * reg_id    - QURT_PMUCNT0 : PMUCNT0 register
 *             QURT_PMUCNT1 : PMUCNT1 register
 *             QURT_PMUCNT2 : PMUCNT2 register
 *             QURT_PMUCNT3 : PMUCNT3 register
 *             QURT_PMUCNT4 : PMUCFG register
 *             QURT_PMUCNT5 : PMUEVTCFG register
 *
 * @ return  reg_value - corresponding register value
 */
unsigned int  qurt_pmu_get (int red_id);
 
/**
 * Enabled or Disable PMU
 *
 * enable = 0, turns OFF profiling
 * enable = 1, turns ON profiling
 *
 * @return void
 */
void qurt_pmu_enable (int enable);

#endif /* QURT_PMU_H */
