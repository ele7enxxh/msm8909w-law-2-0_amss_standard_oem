#ifndef __DDR_CONFIG_H__
#define __DDR_CONFIG_H__


/*==============================================================================
                      Warning: This file is auto-generated
================================================================================
                   Copyright 2012-2013 Qualcomm Technologies Incorporated
                              All Rights Reserved
                     Qualcomm Confidential and Proprietary
==============================================================================*/
/*==============================================================================
                                  INCLUDES
==============================================================================*/
#include "ddr_common.h"


/*==============================================================================
                                  DATA
==============================================================================*/
extern uint32 ddr_bimc_config_static[][2];
extern uint32 ddr_phy_config_static[][2];
extern uint32 ddr_caphy_config_static[][2];
extern uint32 ddr_dqphy_config_static[][2];


/*==============================================================================
                                  FUNCTIONS
==============================================================================*/
void ddr_set_config(uint32 ddr_config[][2], uint32 offset);


#endif
