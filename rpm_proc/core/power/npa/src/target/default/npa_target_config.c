/*==============================================================================

FILE:      npa_target_config.c

DESCRIPTION: NPA target configuration

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //components/rel/rpm.bf/2.1.1/core/power/npa/src/target/default/npa_target_config.c#1 $

==============================================================================*/
#include "npa_config.h"

static npa_config npa_config_data =
{
  8192,                         /* Log Buffer Size */

  /* pool configuration */
  { 10, 10 },                   /* resources */
  { 20, 10 },                   /* clients */
  { 10, 10 },                   /* events */
  { 10, 10 },                   /* links */
  { 10, 10 },                   /* event queue elements */

  /* Workloop configuration */
  { 237, 32 },                  /* Async Request Workloop */
  {   0,  1 }                   /* Async Event Workloop */
};

const npa_config *npa_target_config( void )
{
  return &npa_config_data;
}
