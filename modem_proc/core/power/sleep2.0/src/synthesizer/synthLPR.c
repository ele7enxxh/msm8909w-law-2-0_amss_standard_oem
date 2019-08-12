/*==============================================================================
  FILE:         synthLPR.c
 
  OVERVIEW:     Provides the synth mode functions.
 
  DEPENDENCIES: None

                Copyright (c) 2013-2014 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================
$Header: //components/rel/core.mpss/3.5.c12.3/power/sleep2.0/src/synthesizer/synthLPR.c#1 $
$DateTime: 2016/12/13 07:59:23 $
==============================================================================*/
#include "sleepi.h"
#include "synthRegistry.h"
#include "synthTypes.h"
#include "synthLPR.h"
#include "synthLPRM.h"

/*==============================================================================
                            FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * synthLPR_init
 */
void synthLPR_init(sleep_synth_lpr *synthLPR)
{
  uint32            i;
  sleep_synth_lprm  *synthLPRM;
  
  CORE_VERIFY(synthLPR->mode_count == 
              SLEEP_LPR_NUM_SYNTHESIZED_MODES );

  synthLPR->enabled_mode_count = 0;

  for(i = 0; i < SLEEP_LPR_NUM_SYNTHESIZED_MODES; i++)
  {
    synthLPRM = &(synthLPR->modes[i]);
    synthLPRM_initSynth(synthLPRM);
  }

  return;
}

/*
 * synthLPR_getStatus
 */
sleep_status synthLPR_getStatus(sleep_synth_lpr *synthLPR)
{
  CORE_VERIFY_PTR(synthLPR);

  if( synthLPR->enabled_mode_count > 0 )
  {
    return SLEEP_ENABLED;
  }
  
  return SLEEP_DISABLED;
}

/*
 * synthLPR_modeUpdated
 */
void synthLPR_modeUpdated(sleep_synth_lprm* synthLPRM)
{
  sleep_synth_lpr *parent;

  CORE_VERIFY_PTR( synthLPRM );

  parent = synthLPRM->synth_LPR_parent;

  CORE_VERIFY_PTR( parent );

  if(SLEEP_ENABLED == synthLPRM->mode_status)
  {
    parent->enabled_mode_count++;
  }
  else
  {
    parent->enabled_mode_count--;
  }

  /* Confirm that the number of enabled modes did not go negative, or
   * exceed the total number of modes. */
  CORE_VERIFY( parent->enabled_mode_count <= SLEEP_LPR_NUM_SYNTHESIZED_MODES );
  
  synthRegistry_LPRUpdated(parent);

  return;
}

/*
 * synthLPR_define
 */
void synthLPR_define(sleep_synth_lpr *synthLPR)
{
  synthRegistry_registerSynthLPR(synthLPR);
  return;
}
