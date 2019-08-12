#ifndef SLPC_HWIO_H
#define SLPC_HWIO_H

/*=========================================================================
  FILE: slpc_hwio.h
 
  OVERVIEW:
 

  DEPENDENCIES:
 
 
  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.
=========================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE
 
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/slpc/src/slpc_hwio.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ----------------------------------------------------------- 
05/16/2014   btl   Removed old target-specific macros (8974)
10/17/2013   cab   Initial version 

============================================================================*/



/*============================================================================

                           INCLUDE FILES

============================================================================*/

#include <mqueue.h>
#include <fcntl.h>
#include <qurt_timer.h>

#include "mcs_hwio.h"
#include "DDIChipInfo.h"


/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typedefs,
and other items needed by this module.

============================================================================*/

#ifndef HWIO_SYS_ONLINE_CTRLn_MAXn
  #define HWIO_SYS_ONLINE_CTRLn_MAXn -1
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_GSM_EN_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_GSM_EN_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_DO_EN_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_DO_EN_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_SYS_1X_EN_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_SYS_1X_EN_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_UMTS_EN_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_UMTS_EN_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_LTE_EN_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_LTE_EN_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_TDS_EN_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_TDS_EN_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_IRQ_EN_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_IRQ_EN_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_GSM_EN_G1_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_GSM_EN_G1_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_GSM_EN_G2_BMSK
  #define HWIO_SYS_ONLINE_CTRLn_GSM_EN_G2_BMSK 0
#endif

#ifndef HWIO_SYS_ONLINE_CTRLn_OUTMI
  #define HWIO_SYS_ONLINE_CTRLn_OUTMI(n, m, v)
#endif
#ifndef HWIO_SYS_ONLINE_CTRLn_RMSK
  #define HWIO_SYS_ONLINE_CTRLn_RMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_TIMEn_ONLINE_TIME_BMSK
  #define HWIO_SYS_ONLINE_TIMEn_ONLINE_TIME_BMSK 0
#endif
#ifndef HWIO_SYS_ONLINE_CMDn_TRIG_BMSK
  #define HWIO_SYS_ONLINE_CMDn_TRIG_BMSK 0
#endif

#if !defined( HWIO_G_STMR_TIME_ON_LINE_TIME_ON_LINE_BMSK )
  #define HWIO_G_STMR_TIME_ON_LINE_TIME_ON_LINE_BMSK 0
#endif
#if !defined( HWIO_G_STMR_TIME_SYNC_CTL_ON_LINE_LOAD_EN_BMSK )
  #define HWIO_G_STMR_TIME_SYNC_CTL_ON_LINE_LOAD_EN_BMSK 0
#endif
#if !defined( HWIO_G_STMR_TIME_ON_LINE_OUT )
  #define HWIO_G_STMR_TIME_ON_LINE_OUT( v )
#endif
#if !defined( HWIO_G_STMR_TIME_SYNC_CTL_OUTM )
  #define HWIO_G_STMR_TIME_SYNC_CTL_OUTM(m, v)
#endif

#if !defined( HWIO_G_STMR_TIME_ON_LINE_G1_TIME_ON_LINE_BMSK )
  #define HWIO_G_STMR_TIME_ON_LINE_G1_TIME_ON_LINE_BMSK 0
#endif
#if !defined( HWIO_G_STMR_TIME_SYNC_CTL_G1_ON_LINE_LOAD_EN_BMSK )
  #define HWIO_G_STMR_TIME_SYNC_CTL_G1_ON_LINE_LOAD_EN_BMSK 0
#endif
#if !defined( HWIO_G_STMR_TIME_ON_LINE_G1_OUT )
  #define HWIO_G_STMR_TIME_ON_LINE_G1_OUT( v )
#endif
#if !defined( HWIO_G_STMR_TIME_SYNC_CTL_G1_OUTM )
  #define HWIO_G_STMR_TIME_SYNC_CTL_G1_OUTM(m, v)
#endif

#if !defined( HWIO_G_STMR_TIME_ON_LINE_G2_TIME_ON_LINE_BMSK )
  #define HWIO_G_STMR_TIME_ON_LINE_G2_TIME_ON_LINE_BMSK 0
#endif
#if !defined( HWIO_G_STMR_TIME_SYNC_CTL_G2_ON_LINE_LOAD_EN_BMSK )
  #define HWIO_G_STMR_TIME_SYNC_CTL_G2_ON_LINE_LOAD_EN_BMSK 0
#endif
#if !defined( HWIO_G_STMR_TIME_ON_LINE_G2_OUT )
  #define HWIO_G_STMR_TIME_ON_LINE_G2_OUT( v )
#endif
#if !defined( HWIO_G_STMR_TIME_SYNC_CTL_G2_OUTM )
  #define HWIO_G_STMR_TIME_SYNC_CTL_G2_OUTM(m, v)
#endif

#if !defined( HWIO_RTC_LOAD_OUT )
  #define HWIO_RTC_LOAD_OUT(v)
#endif
#if !defined( HWIO_RTC_LOAD_RTC_LOAD_BMSK )
  #define HWIO_RTC_LOAD_RTC_LOAD_BMSK 0
#endif

#if !defined( HWIO_HDR_RTC_MODIFIER_OUT )
  #define HWIO_HDR_RTC_MODIFIER_OUT(v)
#endif
#if !defined( HWIO_HDR_RTC_MODIFIER_VALUE_CX8_BMSK )
  #define HWIO_HDR_RTC_MODIFIER_VALUE_CX8_BMSK 0
#endif

#if !defined( HWIO_STMR_WALL_TIME_MODIFIER_OUT )
  #define HWIO_STMR_WALL_TIME_MODIFIER_OUT(v)
#endif
#if !defined( HWIO_STMR_WALL_TIME_MODIFIER_WALL_TIME_MODIFIER_BMSK )
  #define HWIO_STMR_WALL_TIME_MODIFIER_WALL_TIME_MODIFIER_BMSK 0
#endif
#if !defined( HWIO_STMR_TIME_CMD_PRI_OUT )
  #define HWIO_STMR_TIME_CMD_PRI_OUT(v)
#endif
#if !defined( HWIO_STMR_TIME_CMD_PRI_WALL_TIME_CMD_BMSK )
  #define HWIO_STMR_TIME_CMD_PRI_WALL_TIME_CMD_BMSK 0
#endif

#if !defined( HWIO_O_STMR_MSTMR_LD_VALUE_OUTM )
  #define HWIO_O_STMR_MSTMR_LD_VALUE_OUTM(m, v)
#endif
#if !defined( HWIO_O_STMR_TIMER_CMD_OUT )
  #define HWIO_O_STMR_TIMER_CMD_OUT(v)
#endif
#if !defined( HWIO_O_STMR_MSTMR_LD_VALUE_MSTMR_LD_VALUE_BMSK )
  #define HWIO_O_STMR_MSTMR_LD_VALUE_MSTMR_LD_VALUE_BMSK 0
#endif
#if !defined( HWIO_O_STMR_TIMER_CMD_MSTMR_CMD_BMSK )
  #define HWIO_O_STMR_TIMER_CMD_MSTMR_CMD_BMSK 0
#endif

#if !defined( HWIO_TDS_STMR_WALL_TIME_MODIFIER_OUT )
  #define HWIO_TDS_STMR_WALL_TIME_MODIFIER_OUT(v)
#endif
#if !defined( HWIO_TDS_STMR_WALL_TIME_MODIFIER_WALL_TIME_MODIFIER_BMSK )
  #define HWIO_TDS_STMR_WALL_TIME_MODIFIER_WALL_TIME_MODIFIER_BMSK 0
#endif
#if !defined( HWIO_TDS_STMR_WALLTIME_CMD_OUT )
  #define HWIO_TDS_STMR_WALLTIME_CMD_OUT(v)
#endif
#if !defined( HWIO_TDS_STMR_WALLTIME_CMD_WALL_TIME_CMD_BMSK )
  #define HWIO_TDS_STMR_WALLTIME_CMD_WALL_TIME_CMD_BMSK 0
#endif

#if !defined( HWIO_GSM_TIME_STATUS_RD_IN )
  #define HWIO_GSM_TIME_STATUS_RD_IN 0
#endif
#if !defined( HWIO_GSM_PHASE_STATUS_RD_IN )
  #define HWIO_GSM_PHASE_STATUS_RD_IN 0
#endif
#if !defined( HWIO_RTC_1X_TIME_STATUS_RD_IN )
  #define HWIO_RTC_1X_TIME_STATUS_RD_IN 0
#endif
#if !defined( HWIO_RTC_1X_PHASE_STATUS_RD_IN )
  #define HWIO_RTC_1X_PHASE_STATUS_RD_IN 0
#endif
#if !defined( HWIO_RTC_HDR_TIME_STATUS_RD_IN )
  #define HWIO_RTC_HDR_TIME_STATUS_RD_IN 0
#endif
#if !defined( HWIO_RTC_HDR_PHASE_STATUS_RD_IN )
  #define HWIO_RTC_HDR_PHASE_STATUS_RD_IN 0
#endif
#if !defined( HWIO_UMTS_STMR_TIME_STATUS_RD_IN )
  #define HWIO_UMTS_STMR_TIME_STATUS_RD_IN 0
#endif
#if !defined( HWIO_UMTS_STMR_PHASE_STATUS_RD_IN )
  #define HWIO_UMTS_STMR_PHASE_STATUS_RD_IN 0
#endif
#if !defined( HWIO_O_STMR_TIME_STATUS_RD_IN )
  #define HWIO_O_STMR_TIME_STATUS_RD_IN 0
#endif
#if !defined( HWIO_O_STMR_PHASE_STATUS_RD_IN )
  #define HWIO_O_STMR_PHASE_STATUS_RD_IN 0
#endif
#if !defined( HWIO_TDS_WALLTIME_STATUS_RD_IN )
  #define HWIO_TDS_WALLTIME_STATUS_RD_IN 0
#endif
#if !defined( HWIO_TDS_PHASE_STATUS_RD_IN )
  #define HWIO_TDS_PHASE_STATUS_RD_IN 0
#endif
#if !defined( HWIO_GSM_TIME_STATUS_RD_G1_IN )
  #define HWIO_GSM_TIME_STATUS_RD_G1_IN 0
#endif
#if !defined( HWIO_GSM_PHASE_STATUS_RD_G1_IN )
  #define HWIO_GSM_PHASE_STATUS_RD_G1_IN 0
#endif
#if !defined( HWIO_GSM_TIME_STATUS_RD_G2_IN )
  #define HWIO_GSM_TIME_STATUS_RD_G2_IN 0
#endif
#if !defined( HWIO_GSM_PHASE_STATUS_RD_G2_IN )
  #define HWIO_GSM_PHASE_STATUS_RD_G2_IN 0
#endif

#if !defined( HWIO_UNIV_STMR_STATUS_DUMP_CMD_STATUS_DUMP_UP_BMSK )
  #define HWIO_UNIV_STMR_STATUS_DUMP_CMD_STATUS_DUMP_UP_BMSK 0
#endif
#if !defined( HWIO_UNIV_STMR_STATUS_DUMP_CMD_OUT )
  #define HWIO_UNIV_STMR_STATUS_DUMP_CMD_OUT(v)
#endif
#if !defined( HWIO_UNIV_STMR_TIME_STATUS_RD_IN )
  #define HWIO_UNIV_STMR_TIME_STATUS_RD_IN 0
#endif
#if !defined( HWIO_SYS_ONLINE_TIMEn_OUTMI )
  #define HWIO_SYS_ONLINE_TIMEn_OUTMI(n, mask, val)
#endif
#if !defined( HWIO_SYS_ONLINE_CMDn_OUTI )
  #define HWIO_SYS_ONLINE_CMDn_OUTI(n, mask)
#endif

/*----------------------------------------------------------------*/

#define SLPC_READ_USTMR_TIME() HWIO_UNIV_STMR_MSTMR_IN

#define SLPC_USTMR_RESET() \
  HWIO_UNIV_STMR_RESET_OUT( HWIO_UNIV_STMR_RESET_DATA0_BMSK )

#define SLPC_USTMR_ENABLE() \
  HWIO_UNIV_STMR_ENABLE_OUT( HWIO_UNIV_STMR_ENABLE_MSTMR_EN_BMSK )

#define SLPC_USTMR_SYNC() \
  HWIO_UNIV_STMR_SYNC_CMD_OUT( HWIO_UNIV_STMR_SYNC_CMD_SYNC_BMSK )

#define SLPC_HW_READ_SYNC_CMD() \
  HWIO_UNIV_STMR_STATUS_DUMP_CMD_OUT( \
    HWIO_UNIV_STMR_STATUS_DUMP_CMD_STATUS_DUMP_UP_BMSK )

#define SLPC_READ_USTMR_TIME_SYNC() HWIO_UNIV_STMR_TIME_STATUS_RD_IN

#define SLPC_READ_TSTMR_TIME_GSM()       HWIO_GSM_TIME_STATUS_RD_IN
#define SLPC_READ_TSTMR_PHASE_GSM()      HWIO_GSM_PHASE_STATUS_RD_IN
#define SLPC_READ_TSTMR_TIME_1X()        HWIO_RTC_1X_TIME_STATUS_RD_IN
#define SLPC_READ_TSTMR_PHASE_1X()       HWIO_RTC_1X_PHASE_STATUS_RD_IN
#define SLPC_READ_TSTMR_TIME_HDR()       HWIO_RTC_HDR_TIME_STATUS_RD_IN
#define SLPC_READ_TSTMR_PHASE_HDR()      HWIO_RTC_HDR_PHASE_STATUS_RD_IN
#define SLPC_READ_TSTMR_TIME_WCDMA()     HWIO_UMTS_STMR_TIME_STATUS_RD_IN
#define SLPC_READ_TSTMR_PHASE_WCDMA()    HWIO_UMTS_STMR_PHASE_STATUS_RD_IN
#define SLPC_READ_TSTMR_TIME_LTE()       HWIO_O_STMR_TIME_STATUS_RD_IN
#define SLPC_READ_TSTMR_PHASE_LTE()      HWIO_O_STMR_PHASE_STATUS_RD_IN
#define SLPC_READ_TSTMR_TIME_TDSCDMA()   HWIO_TDS_WALLTIME_STATUS_RD_IN
#define SLPC_READ_TSTMR_PHASE_TDSCDMA()  HWIO_TDS_PHASE_STATUS_RD_IN
#define SLPC_READ_TSTMR_TIME_GSM2()      HWIO_GSM_TIME_STATUS_RD_G1_IN
#define SLPC_READ_TSTMR_PHASE_GSM2()     HWIO_GSM_PHASE_STATUS_RD_G1_IN
#define SLPC_READ_TSTMR_TIME_GSM3()      HWIO_GSM_TIME_STATUS_RD_G2_IN
#define SLPC_READ_TSTMR_PHASE_GSM3()     HWIO_GSM_PHASE_STATUS_RD_G2_IN

#define SLPC_WRITE_TSTMR_TIME_GSM( v ) \
  HWIO_G_STMR_TIME_ON_LINE_OUT( \
    HWIO_G_STMR_TIME_ON_LINE_TIME_ON_LINE_BMSK & v )
#define SLPC_WRITE_TSTMR_PHASE_GSM()
#define SLPC_GSM_LOAD_ON_NEXT_ONLINE_START  0x1
#define SLPC_WRITE_TSTMR_CMD_GSM() \
  HWIO_G_STMR_TIME_SYNC_CTL_OUTM( \
    HWIO_G_STMR_TIME_SYNC_CTL_ON_LINE_LOAD_EN_BMSK, \
    SLPC_GSM_LOAD_ON_NEXT_ONLINE_START )

#define SLPC_WRITE_TSTMR_TIME_GSM2( v ) \
  HWIO_G_STMR_TIME_ON_LINE_G1_OUT( \
    HWIO_G_STMR_TIME_ON_LINE_G1_TIME_ON_LINE_BMSK & v )
#define SLPC_WRITE_TSTMR_PHASE_GSM2()
#define SLPC_GSM_LOAD_ON_NEXT_ONLINE_START_G1  0x1
#define SLPC_WRITE_TSTMR_CMD_GSM2() \
  HWIO_G_STMR_TIME_SYNC_CTL_G1_OUTM( \
    HWIO_G_STMR_TIME_SYNC_CTL_G1_ON_LINE_LOAD_EN_BMSK, \
    SLPC_GSM_LOAD_ON_NEXT_ONLINE_START_G1 )

#define SLPC_WRITE_TSTMR_TIME_GSM3( v ) \
  HWIO_G_STMR_TIME_ON_LINE_G2_OUT( \
    HWIO_G_STMR_TIME_ON_LINE_G2_TIME_ON_LINE_BMSK & v )
#define SLPC_WRITE_TSTMR_PHASE_GSM3()
#define SLPC_GSM_LOAD_ON_NEXT_ONLINE_START_G2  0x1
#define SLPC_WRITE_TSTMR_CMD_GSM3() \
  HWIO_G_STMR_TIME_SYNC_CTL_G2_OUTM( \
    HWIO_G_STMR_TIME_SYNC_CTL_G2_ON_LINE_LOAD_EN_BMSK, \
    SLPC_GSM_LOAD_ON_NEXT_ONLINE_START_G2 )

#define SLPC_WRITE_TSTMR_TIME_1X( v ) \
  HWIO_RTC_LOAD_OUT( \
    HWIO_RTC_LOAD_RTC_LOAD_BMSK & v )
#define SLPC_WRITE_TSTMR_PHASE_1X()
      
#define SLPC_WRITE_TSTMR_TIME_HDR( v ) \
  HWIO_HDR_RTC_MODIFIER_OUT( \
    HWIO_HDR_RTC_MODIFIER_VALUE_CX8_BMSK & v )
#define SLPC_WRITE_TSTMR_PHASE_HDR()
     
#define SLPC_WRITE_TSTMR_TIME_WCDMA( v ) \
  HWIO_STMR_WALL_TIME_MODIFIER_OUT( \
    HWIO_STMR_WALL_TIME_MODIFIER_WALL_TIME_MODIFIER_BMSK & v )
#define SLPC_WCDMA_LOAD_ON_NEXT_ONLINE_START  0x5
#define SLPC_WRITE_TSTMR_CMD_WCDMA() \
  HWIO_STMR_TIME_CMD_PRI_OUT( SLPC_WCDMA_LOAD_ON_NEXT_ONLINE_START & \
                              HWIO_STMR_TIME_CMD_PRI_WALL_TIME_CMD_BMSK )
   
#define SLPC_WRITE_TSTMR_TIME_LTE( v ) \
  HWIO_O_STMR_MSTMR_LD_VALUE_OUTM( \
    HWIO_O_STMR_MSTMR_LD_VALUE_MSTMR_LD_VALUE_BMSK, v )
#define SLPC_LTE_LOAD_ON_NEXT_ONLINE_START  0x5
#define SLPC_WRITE_TSTMR_CMD_LTE() \
  HWIO_O_STMR_TIMER_CMD_OUT( SLPC_LTE_LOAD_ON_NEXT_ONLINE_START & \
                             HWIO_O_STMR_TIMER_CMD_MSTMR_CMD_BMSK )

#define SLPC_WRITE_TSTMR_TIME_TDSCDMA( v ) \
  HWIO_TDS_STMR_WALL_TIME_MODIFIER_OUT( \
    HWIO_TDS_STMR_WALL_TIME_MODIFIER_WALL_TIME_MODIFIER_BMSK & v )
#define SLPC_WRITE_TSTMR_PHASE_TDSCDMA() 
#define SLPC_TDSCDMA_LOAD_ON_NEXT_ONLINE_START  0x4
#define SLPC_WRITE_TSTMR_CMD_TDSCDMA() \
  HWIO_TDS_STMR_WALLTIME_CMD_OUT( SLPC_TDSCDMA_LOAD_ON_NEXT_ONLINE_START & \
                                  HWIO_TDS_STMR_WALLTIME_CMD_WALL_TIME_CMD_BMSK )

#define SLPC_USTMR_EN_INT( n ) \
  HWIO_SYS_ONLINE_CTRLn_OUTMI( n, \
      HWIO_SYS_ONLINE_CTRLn_IRQ_EN_BMSK, \
      HWIO_SYS_ONLINE_CTRLn_IRQ_EN_BMSK )

#define SLPC_USTMR_DIS_INT( n ) \
    HWIO_SYS_ONLINE_CTRLn_OUTMI( n, \
      HWIO_SYS_ONLINE_CTRLn_IRQ_EN_BMSK, \
      ~HWIO_SYS_ONLINE_CTRLn_IRQ_EN_BMSK )

#define SLPC_USTMR_SET_ROUTING( n, val ) \
    HWIO_SYS_ONLINE_CTRLn_OUTMI( n, HWIO_SYS_ONLINE_CTRLn_RMSK, val )

#define SLPC_USTMR_SET_TIME( n, val ) \
    HWIO_SYS_ONLINE_TIMEn_OUTMI( n, \
    HWIO_SYS_ONLINE_TIMEn_ONLINE_TIME_BMSK, val)

#define SLPC_USTMR_START_TIMER( n ) \
    HWIO_SYS_ONLINE_CMDn_OUTI( n, HWIO_SYS_ONLINE_CMDn_TRIG_BMSK )

#endif /* SLPC_HWIO_H */

