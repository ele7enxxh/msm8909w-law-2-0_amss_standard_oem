#ifndef CPU_MONITOR_H__
#define CPU_MONITOR_H__

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               C P U    M O N I T O R     H E A D E R    F I L E

DESCRIPTION
  Public Header file for the CPU Monitor. Contains types and externalized
  functions of the Flow Controller module.

Copyright (c) 2000-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/fc/inc/cpu_monitor.h#1 $ $DateTime: 2016/12/13 07:57:51 $ $Author: mplcsds1 $

when       who      what, where, why
--------   -------- --------------------------------------------------------
01/20/11    pj       Fixed compiler warnings
06/23/09    pj       Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#ifdef CUST_H
#include "customer.h"
#endif

#include "rex.h"
#include "queue.h"

/*---------------------------------------------------------------------------
 *  Type containing the processor type
 *-------------------------------------------------------------------------*/

typedef enum
{
  FC_MODEM_CPU   = 0x00,
  FC_APPS_CPU    = 0x01,
  FC_MAX_CPU     = 0x02,
  FC_UNKNOWN_CPU = 0xFF
} fc_processor_type;

/*---------------------------------------------------------------------------
 *  Type containing the cpu reporting types
 *-------------------------------------------------------------------------*/
typedef enum 
{
  FC_CPU_EVENT_BASED   = 0,
  FC_CPU_PERIODIC      = 1,
  FC_CPU_ON_DEMAND     = 2,
  FC_CPU_UNKNOWN_REPORTING
} fc_cpu_reporting_type;

/*---------------------------------------------------------------------------
 *  Type containing the cpu measurement types
 *-------------------------------------------------------------------------*/
typedef enum 
{
  FC_CPU_RAW_VALUE  = 0,
  FC_CPU_SCALED     = 1,
  FC_CPU_UNKNOWN_MEASUREMENT
} fc_cpu_measurement_type;

typedef struct
{
  fc_processor_type           processor_id;
  fc_cpu_reporting_type       fc_cpu_reporting;
  fc_cpu_measurement_type     fc_cpu_meas;
} fc_report_config_type;

/*---------------------------------------------------------------------------
 *  Type containing the cpu threshold values for the registering client
 *-------------------------------------------------------------------------*/
typedef struct
{
  uint8  high;  /* High CPU threshold */
  uint8  low;  /* Low CPU threshold */
  uint8  off;    /* Off CPU threshold */
} fc_cpu_threshold_value_type;

/*---------------------------------------------------------------------------
 *  Type containing info with which a client registers with the Flow Controller
 *-------------------------------------------------------------------------*/
typedef struct fc_register_s
{
  void                        *tcb;       /*~ FIELD fc_register_s.tcb POINTER */
  void                        *q;         /*~ FIELD fc_register_s.q POINTER */
  rex_sigs_type               sig;
  uint32                      cmd_id;  
  fc_cpu_threshold_value_type fc_cpu_threshold;
  /* period is an integer indicating mutiple of 50ms,
   * Periodic reporting: the cpu load at reporting time (NOT average)
   * On-demand reporting: CPU reports averaging time over the period
   */
  uint16                       period; 
  fc_report_config_type       fc_rpt_cfg;
} fc_register_type;

/*---------------------------------------------------------------------------
 *  Type containing info with which a client de-registers with the Flow Controller
 *-------------------------------------------------------------------------*/
typedef struct fc_deregister_s
{
  void                        *tcb;     /*~ FIELD fc_deregister_s.tcb POINTER */
  fc_processor_type           processor_id;
} fc_deregister_type;

/*---------------------------------------------------------------------------
 *  In Command data type for Flow Controller
 *-------------------------------------------------------------------------*/
typedef struct fc_in_cmd_data_s
{
  fc_register_type        fc_reg;     
  fc_deregister_type      fc_dereg;   
} fc_in_cmd_data_type;

/*---------------------------------------------------------------------------
 *  Types of command into Flow Controller.
 *-------------------------------------------------------------------------*/
typedef enum
{
  FC_IN_REGISTER_CMD,
  FC_IN_DEREGISTER_CMD,
  FC_IN_MEASUREMENT_REQ_CMD,
  FC_IN_MAX_CMD
} fc_in_cmd_enum_type;

/*---------------------------------------------------------------------------
 *  In Command Header type for Flow Controller.
 *-------------------------------------------------------------------------*/
typedef struct
{
  q_link_type           link;
  fc_in_cmd_enum_type   cmd_id;
} fc_in_cmd_hdr_type;
  
/*---------------------------------------------------------------------------
 *  In Command for Flow Controller
 *-------------------------------------------------------------------------*/
typedef struct
{
  fc_in_cmd_hdr_type  cmd_hdr;
  fc_in_cmd_data_type cmd_data;
} fc_in_cmd_type;

/*---------------------------------------------------------------------------
 *  Types of command out of Flow Controller.
 *-------------------------------------------------------------------------*/
typedef enum
{
  FC_OUT_OFF_CMD      = 0,
  FC_OUT_UP_CMD       = 4,
  FC_OUT_FREEZE_CMD   = 6,
  FC_OUT_DN_CMD       = 7,
  FC_OUT_MAX_CMD,
  FC_OUT_UNKNOWN_CMD  = FC_OUT_MAX_CMD
} fc_out_cmd_enum_type;

/*
  "111" = DOWN, "110" = FREEZE, "100" = UP, "000" = OFF.
*/

/*---------------------------------------------------------------------------
 *  Out Command Header type for Flow Controller.
 *-------------------------------------------------------------------------*/
typedef struct
{
  q_link_type             link;
  fc_out_cmd_enum_type    cmd_id; /* reporting command if reporting type is event based*/
  fc_report_config_type   fc_rpt_cfg;
  uint8                   cpu_loading; /* report value if reporting type is on_demand or periodic*/
} fc_out_cmd_hdr_type;

/*---------------------------------------------------------------------------
 *  Out Command Header type for Flow Controller.
 *-------------------------------------------------------------------------*/
typedef struct
{
  fc_out_cmd_hdr_type  cmd_hdr;
} fc_out_cmd_type;

#endif /* CPU_MONITOR_H__*/
