 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
GENERAL DESCRIPTION
   This driver is implemented for Cortex M3 ETM and architecture v3.5

Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           EDIT HISTORY FOR FILE
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/kernel/arch/src/cortexm3/etm.c#1 $
$DateTime: 2015/02/25 02:53:43 $

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "HALcomdef.h"
#include "QDSSLite.h"

// ETM base
// TODO: once HW team has properly defined the ETM register programming

// ETM base
#define QDSS_RPM_BASE   0xE0041000

// ETM register addresses
#define ETMCR        (QDSS_RPM_BASE + 0x000*4)
#define ETMCCR       (QDSS_RPM_BASE + 0x001*4)
#define ETMTRIGGER   (QDSS_RPM_BASE + 0x002*4)
#define ETMSR        (QDSS_RPM_BASE + 0x004*4)
#define ETMTSSCR     (QDSS_RPM_BASE + 0x006*4)
#define ETMTECR2     (QDSS_RPM_BASE + 0x007*4)
#define ETMTEEVR     (QDSS_RPM_BASE + 0x008*4)
#define ETMTECR1     (QDSS_RPM_BASE + 0x009*4)
#define ETMFFRR      (QDSS_RPM_BASE + 0x00a*4)
#define ETMFFLR      (QDSS_RPM_BASE + 0x00b*4)
#define ETMVDEVR     (QDSS_RPM_BASE + 0x00c*4)
#define ETMSYNCFR    (QDSS_RPM_BASE + 0x078*4)
#define ETMIDR       (QDSS_RPM_BASE + 0x079*4)
#define ETMTRACEIDR  (QDSS_RPM_BASE + 0x080*4)
#define ETMLAR       (QDSS_RPM_BASE + 0x3ec*4)
#define ETMLSR       (QDSS_RPM_BASE + 0x3ed*4)

// ETM register values
// main control
#define ETMCR_PROGRAMMING  0x00000400
#define ETMCR_CONFIG       0x00000840
// status
#define ETMSR_PROGRAMMING  0x00000002
// Resource events
#define ETM_EVENT_TRUE     0x37EF
#define ETM_EVENT_FALSE    0x77EF
// TraceEnable Control 1
#define ETMTECR1_EXCLUDE   0x01000000
// ID
#define ETMIDR_VALUE       0x4114F253
// CoreSight Trace ID
#define ETMTRACEIDR_RPM 0x20  // 32
// Lock Access
#define ETMLAR_LOCK_VAL    0x0
#define ETMLAR_UNLOCK_VAL  0xC5ACCE55

/*===========================================================================

                     DATA DECLARATIONS

===========================================================================*/
static int etm_enabled = 0;

/*===========================================================================

                     MODULE FUNCTIONS

===========================================================================*/
static void ETM_program_start(void)
{
   uint32 reg_value;

   // unlock ETMLAR
   outpdw(ETMLAR, ETMLAR_UNLOCK_VAL);

   // enter programming mode
   reg_value = inpdw(ETMCR);
   reg_value |= ETMCR_PROGRAMMING;
   do
   {
      outpdw(ETMCR, reg_value);
   }
   while (!(inpdw(ETMSR) & ETMSR_PROGRAMMING));
}

static void ETM_program_stop(void)
{
   uint32 reg_value;

   // leave programming mode
   reg_value = inpdw(ETMCR);
   reg_value &= (~ETMCR_PROGRAMMING);
   do
   {
      outpdw(ETMCR, reg_value);
   }
   while (inpdw(ETMSR) & ETMSR_PROGRAMMING);

   // lock ETMLAR
   outpdw(ETMLAR, ETMLAR_LOCK_VAL);

   // memory barrier
   __dsb(0xf);
}

static void ETM_program_trace_on(void)
{
   ETM_program_start();

   outpdw(ETMCR, (ETMCR_PROGRAMMING | ETMCR_CONFIG));

   outpdw(ETMTSSCR, 0);
   outpdw(ETMTECR2, 0);

   // exclude no addresses for tracing
   outpdw(ETMTECR1, ETMTECR1_EXCLUDE);

   // enable tracing
   outpdw(ETMTEEVR, ETM_EVENT_TRUE);

   // program trace ID
   outpdw(ETMTRACEIDR, ETMTRACEIDR_RPM);

   ETM_program_stop();
}

/*
 * @brief Enable ETM instruction tracing
 *
 * @param None
 *
 * @return None
 *
 * @dependencies
 * QDSSInit is already called.
 */
void ETM_trace_enable(void)
{
   QDSSEnableTrace();
   ETM_program_trace_on();
   etm_enabled = 1;
}

/*
 * @brief Disble ETM instruction tracing
 *
 * @param None
 *
 * @return None
 */
void ETM_trace_disable(void)
{
   ETM_program_start();

   // disable tracing
   outpdw(ETMTEEVR, ETM_EVENT_FALSE);

   ETM_program_stop();

   etm_enabled = 0;
   QDSSDisableTrace();
}

/*
 * @brief Resume ETM state after low power mode
 *
 * @param None
 *
 * @return None
 */
void ETM_restore_state(void)
{
   if (etm_enabled) ETM_program_trace_on();
}

/*
 * @brief Save ETM state before low power mode
 *
 * @param None
 *
 * @return None
 */
void ETM_save_state(void)
{
   // currently NOP
}
