#ifndef __VS_TASK_H__
#define __VS_TASK_H__

/*
  ============================================================================

   Copyright (C) 2014-2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/protected/vs_task.h#1 $
  $DateTime: 2016/12/13 07:59:59 $
  $Author: mplcsds1 $
*/

/* VS High Priority Task */
#define VS_HIGH_TASK_NAME ( "VS_HIGH" )
#define VS_HIGH_TASK_STACK_SIZE ( 2 * 1024 )

/* VS Medium Priority Task */
#define VS_MED_TASK_NAME ( "VS_MED" )
#define VS_MED_TASK_STACK_SIZE ( 2 * 1024 )

/* VS Low Priority Task */
#define VS_LOW_TASK_NAME ( "VS_LOW" )
#define VS_LOW_TASK_STACK_SIZE ( 4 * 1024 )

/* MVS Task */
#define MVS_TASK_NAME ( "MVS" )
#define MVS_TASK_STACK_SIZE ( 4 * 1024 )

/* MVA Task */
#define MVA_TASK_NAME ( "MVA" )
#define MVA_TASK_STACK_SIZE ( 2 * 1024 )

/* CVA Task */
#define CVA_TASK_NAME ( "CVA" )
#define CVA_TASK_STACK_SIZE ( 4 * 1024 )

#ifdef AVS_MPSS_TA

/* AVS thread priorities specific to Tabasco. */
#define VS_HIGH_TASK_PRIORITY ( 74 )
#define VS_MED_TASK_PRIORITY ( 82 )
#define VS_LOW_TASK_PRIORITY ( 119 )
#define MVS_TASK_PRIORITY ( 82 )
#define MVA_TASK_PRIORITY ( 84 )
#define CVA_TASK_PRIORITY ( 82 )

#else

/* AVS thread priorities specific to Bear family chipsets. */
#define VS_HIGH_TASK_PRIORITY ( 84 )
#define VS_MED_TASK_PRIORITY ( 85 )
#define VS_LOW_TASK_PRIORITY ( 88 )
#define MVS_TASK_PRIORITY ( 85 )
#define MVA_TASK_PRIORITY ( 84 )
#define CVA_TASK_PRIORITY ( 84 )


#endif /* AVS_MPSS_TA */

#endif /* __VS_TASK_H__ */

