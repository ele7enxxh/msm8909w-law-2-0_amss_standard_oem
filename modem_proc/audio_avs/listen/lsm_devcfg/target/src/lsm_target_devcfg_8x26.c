/*========================================================================
   This file contains LSM device configuration functions

  Copyright (c) 2009-2015 Qualcomm Technologies, Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary.  Export of this technology or software is regulated
  by the U.S. Government, Diversion contrary to U.S. law prohibited.

 ====================================================================== */

#include "lsm_target_devcfg.h"

lpasshwio_prop_lsm_mmpm_struct_t lsm_mmpm_prop =
{     1,
        {22300 , 27000, 31000, 34000, 65000, 100000, 111000, 134000, 160000, 175000}, // These are bw levels in kbitps
        {150   , 170  , 180  , 190  , 200   , 210  , 220   , 240   , 260   , 290   }, // These are CPP factors based on [BW level]
};
