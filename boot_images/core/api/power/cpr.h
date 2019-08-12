// cpr.h - interface description of cpr driver
//
// Copyright 2013 by Qualcomm Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header$
// $ Author$
// $ DateTime$


#ifndef CPR_H
#define CPR_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "DALStdDef.h"

//This function returns the hash of the CPR settings that are related to Cx/Mx.
//On targets with fixed Mx voltages, this hash will factor in the static table of
//Mx voltages as well as the Cx CPR settings.
//On targets with CPR-managed Mx voltages, this hash will factor in the
//Mx CPR settings.
//NOTE: This function MUST be used after rbcpr_init().
uint32 rbcpr_cx_mx_settings_checksum(void);

//Function to be called to initialize the CPR driver.
//Must be called after railway_init.
//Will do the necessary settling of voltages as required for any DDR-related
//training on the target.
void rbcpr_init(void);

//Function to be called after SMEM is initialized to push out the CPR settings
//to SMEM. These settings are to be picked up by the RPM CPR driver during boot.
//Must be done before the RPM FW execution begins.
void rbcpr_externalize_state(void);

//Function to init Cx and Mx to safe Turbo voltages. Should only be called once.
//This function is only supported for 9x45.
//For other targets, the Cx/Mx voltage should be adjusted via voting on Railway.
void rbcpr_set_cx_mx_to_safe_turbo_voltage(void);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // CPR_H

