/*==============================================================================

FILE:      npa_target_config.c

DESCRIPTION: NPA target configuration for RPM

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright (c) 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

$Header: //components/rel/rpm.bf/2.1.1/core/power/npa/src/target/bf/rpm/npa_target_config.c#1 $

==============================================================================*/
#include "npa_config.h"

static npa_config npa_config_data =
{
  1024,                       /* Log Buffer Size */

  /* pool configuration */
  { 0x0A, 1 },                /* resources */
  { 0x10, 1 },                /* clients */
  { 0x00, 1 },                /* events */
  { 0x10, 1 },                /* links */
  { 1,    1 },                /* event queue elements */

  /* Workloop configuration */
  { 0,  0 },                  /* Async Request Workloop */
  { 0xFFFFFFFF, 1 }           /* Async Event Workloop */
};


const npa_config *npa_target_config( void )
{
  return &npa_config_data;
}
