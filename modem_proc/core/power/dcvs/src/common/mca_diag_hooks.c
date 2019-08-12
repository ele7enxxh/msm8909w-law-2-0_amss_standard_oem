/*============================================================================
@file mca_diag_hooks.c


Copyright (c) 2012-2013 QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/common/mca_diag_hooks.c#1 $
============================================================================*/
#include <stdio.h>
#include "npa.h"
#include "ULogFront.h"
#include "mca_internal.h"
#include "mca_diag_hooks.h"
#include "dcvs_npa.h"
#include "DALStdErr.h"

/**
 * <!-- mca_ulog_diag_plugin -->
 * 
 * @brief Used with the ULog Diag Plugin function, this creates a dump log.
 *
 * Once installed this function generates a "NPA Dump Log" ulog when the ULog 
 * Diag handler plugin command is called with the ULOG_DIAG_PLUGIN_ID_NPADUMP
 * plugin id number. 
 *
 * @param command : ULOG_DIAG_PLUGIN_ID_NPADUMP is the only recognized command
 * currently. 
 *
 * @return DAL_SUCCESS if the command is supported
 */
int mca_ulog_diag_plugin( uint32 command )
{
  int ret_val = DAL_ERROR;
  ULogHandle log;
  #ifdef MCA_ENABLE_DIAG_HOOKS
  char * modeString;
  #endif

  ULogFront_RealTimeInit( &log,
                          "MCA Diag Log",
                          0x1000,
                          ULOG_MEMORY_LOCAL,
                          ULOG_LOCK_OS );

  #ifdef MCA_ENABLE_DIAG_HOOKS
  ret_val = DAL_SUCCESS;
  switch ( command )
  {
    case MCA_SET_OUTPUT_MODE_FIXED:
      if ( !mcaNextParamIsValue )
      {
        mcaHookMode = 1;
        ULOG_RT_PRINTF_4( log, "MCA Output Mode Set to Fixed (Q6 CLK : %d) (IBW : %d) (ABW : %d) (Latency : %d)", mcaFixedQ6, mcaFixedIBW, mcaFixedABW, mcaFixedLatency );
        npa_issue_internal_request( mcaDiagClient );
        break;
      }
    case MCA_SET_OUTPUT_MODE_REAL:
      if ( !mcaNextParamIsValue )
      {
        mcaHookMode = 0;
        ULOG_RT_PRINTF_0( log, "MCA Output Mode Set to Real" );
        break;
      }
    case MCA_SET_Q6:
      if ( !mcaNextParamIsValue )
      {
        mcaNextParamIsValue = 1;
        mcaNextParamIsQ6 = 1;
        ULOG_RT_PRINTF_0( log, "Setting Q6 Clk Value to...." );
        break;
      }
    case MCA_SET_IBW:
      if ( !mcaNextParamIsValue ) 
      {
        mcaNextParamIsValue = 1; 
        mcaNextParamIsIBW = 1;
        ULOG_RT_PRINTF_0( log, "Setting IBW Value to...." );
        break;
      }
    case MCA_SET_ABW:
      if ( !mcaNextParamIsValue )
      {
        mcaNextParamIsValue = 1;
        mcaNextParamIsABW = 1;
        ULOG_RT_PRINTF_0( log, "Setting ABW Value to...." );
        break;
      }
    case MCA_SET_LATENCY:
      if ( !mcaNextParamIsValue ) 
      {
        mcaNextParamIsValue = 1; 
        mcaNextParamIsLatency = 1;
        ULOG_RT_PRINTF_0( log, "Setting Latency Value to...." );
        break;
      }
    case MCA_QUERY:
      if ( !mcaNextParamIsValue )
      {
        if ( mcaHookMode )
        {
          modeString = "Fixed";
        }
        else
        {
          modeString = "Real";
        }
        ULOG_RT_PRINTF_5( log, "CURRENT SETTINGS : (Mode : %s) (Q6 Clk : %d kHz) (IBW : %d MBps) (ABW : %d MBps) (Latency : %d ns)",
                          modeString, mcaFixedQ6, mcaFixedIBW, mcaFixedABW, mcaFixedLatency );
        break; 
      }
    default:
      if ( mcaNextParamIsValue )
      {
         if( mcaNextParamIsQ6 == 1 )
         {
           mcaNextParamIsQ6 = 0;
           mcaFixedQ6 = command;
           ULOG_RT_PRINTF_1(log, "%d kHz", command);
         }
         else if( mcaNextParamIsIBW == 1 )
         {
           mcaNextParamIsIBW = 0;
           mcaFixedIBW = command;
           ULOG_RT_PRINTF_1(log, "%d MBps", command);
         }
         else if( mcaNextParamIsABW == 1 )
         {
           mcaNextParamIsABW = 0; 
           mcaFixedABW = command;
           ULOG_RT_PRINTF_1(log, "%d MBps", command);
         }
         else if ( mcaNextParamIsLatency == 1 )
         {
           mcaNextParamIsLatency = 0; 
           mcaFixedLatency = command;
           ULOG_RT_PRINTF_1( log, "%d ns", command );
         }
      }
       mcaNextParamIsValue = 0;
       break;
  }
  #else
  ULOG_RT_PRINTF_0(log, "DEFINE MCA_ENABLE_DIAG_HOOKS IN core\\power\\dcvs\\inc\\dcvs_npa.h TO USE THIS TOOL");
  #endif

  return ret_val;
}
