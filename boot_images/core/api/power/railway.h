// railway.h - interface description of 'railway' driver
//
// Copyright 2011 - 2012 by Qualcomm Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header$
// $ Author$
// $ DateTime$


#ifndef RAILWAY_H
#define RAILWAY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "../dal/DALStdDef.h"

//Forward Declerations
typedef struct railway_voter_s* railway_voter_t;

#define RAIL_NOT_SUPPORTED_BY_RAILWAY -1

//To do - remove this and replace with the levels supported by PMIC driver.
typedef enum
{
  RAILWAY_NO_REQUEST         = 0,
  RAILWAY_RETENTION          = 1,
  RAILWAY_SVS_KRAIT          = 2,
  RAILWAY_SVS_SOC            = 3,
  RAILWAY_SVS_HIGH           = 4,
  RAILWAY_NOMINAL            = 5,
  RAILWAY_TURBO              = 6,
  RAILWAY_TURBO_HIGH         = 7,
  RAILWAY_SUPER_TURBO        = 8,
  RAILWAY_SUPER_TURBO_NO_CPR = 9,
  RAILWAY_CORNERS_COUNT,
} railway_corner;

typedef enum
{
  RAILWAY_PRECHANGE_CB = 0,
  RAILWAY_POSTCHANGE_CB,
  RAILWAY_LAST_PRECHANGE_CB,
  RAILWAY_FIRST_POSTCHANGE_CB,
  RAILWAY_NUM_CB_TYPES,
} railway_cb_type;

typedef enum
{
    RAILWAY_SVS_VOTER_ID = 100,
    RAILWAY_RPM_CX_VOTER_ID,   //The RPM itself makes a supressible vote for SVS on Cx.
    RAILWAY_RPM_MX_VOTER_ID,   //The RPM itself makes a supressible vote for SVS on Mx.
    RAILWAY_DDR_TRAINING_VOTER_ID,
    RAILWAY_RPM_BRINGUP_VOTER, //Voter to pin Cx to super turbo for bringup.
    RAILWAY_RPM_INIT_VOTER, //Voter to pin Mx to super turbo.
    RAILWAY_CLOCK_DRIVER_VOTER_ID,
    RAILWAY_CPR_SETTLING_VOTER,
    RAILWAY_RPM_CX_SLEEP_VOTER_ID,
    RAILWAY_RPM_MX_SLEEP_VOTER_ID,
    RAILWAY_RPM_EBI_VOTER_ID,
    RAILWAY_RPM_EBI_SLEEP_VOTER_ID,
    RAILWAY_VSENSE_VOTER_ID,
} railway_voter_id;

// railway_settings structure passed back to client of railway callbacks.
// mode       - the highest corner vote for this rail.
// microvolts - the voltage that the rail is set to.                
typedef struct
{
  railway_corner mode;
  uint32   microvolts;
} railway_settings;

typedef void (*railway_cb)(const railway_settings *settings, void* cookie);

/// Initialize the railway driver and all of its associated rail control.
void railway_init(void);

// Return the ID for the named rail on this target.
// The rail ID will be differnt from target to target depending on which rails railway is controlling.
// Returns RAIL_NOT_SUPPORTED_BY_RAILWAY if the rail isn't controlled by railway.
int rail_id(const char* rail);

// Sets a callback on a particular event (pre/post change) for a particular voltage rail.
// The cookie passed in will be passed back in the callback, can be NULL.
// This function asserts that there is no error and that the callback can be correctly added.
void railway_set_callback(int rail, railway_cb_type type, railway_cb cb, void* cookie);

// After changing votes, call this function to invoke a voltage change to the lowest acceptible
// level on all Railway-controlled rails.
void railway_transition_rails(void);

// Will assert that voter can be alloced - will not return NULL.
railway_voter_t railway_create_voter(int rail, int id);

// Update which corner this voter is voting for.
// Will NOT invoke the railway transitioner to transition the voltage - call railway_transition_rails
// to invoke the voltage change.
void railway_corner_vote(railway_voter_t voter, railway_corner corner);

//API used by CPR to set the value for a corner. Does not actually transition the rail.
void railway_set_corner_voltage(int rail, railway_corner corner, uint32 microvolts);

//API used by CPR to get the value for a corner.
uint32 railway_get_corner_voltage(int rail, railway_corner corner);

//Returns a copy of the current settings of the specified rail.
void railway_get_current_settings(int rail, railway_settings* settings);

// Returns a 16-bit checksum of the specified rail setting.
uint16 railway_setting_checksum(uint32 rail_id);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // RAILWAY_H

