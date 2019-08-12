// railway.h - interface description of 'railway' driver
//
// Copyright 2011 - 2012 by Qualcomm Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header$
// $ Author$
// $ DateTime$
//
// Most voltage rails are voted on by their users directly--their state can be
// sufficiently described as merely a function of voltage (and possibly some
// other bits of mode data).  However, there exist some rails with more complex
// control schemes.
//
// "railway" is a way for rails that require a more complex control scheme to
// implement a sort of collaborative voting interface.  It is intended to
// provide a mechansim for two known requirements:
//
// 1) the ability to couple a clock speed to an aggregated rail request (e.g.,
//    keeping the RPM clock running as fast as possible given the current VDDCX
//    voltage corner
// 2) the ability to provide modifications to actual voltages represented by a
//    corner (e.g., using RapidBridge Core Power Reduction [RBCPR] sensors to
//    adjust the voltage of the rail given some conditions)
//
// The mechanism by which this is enforced is relatively simple.  Upon the
// calling of railway_init, the railway driver will take control of all known
// voter inputs to its associated voltage rails (NPA nodes, RPMFW resources,
// etc.).  The railway driver will aggregate the votes from these sources into
// a sort of tuple.  The first element of thise tuple is the "mode" -- or
// associated "voltage corner" or "characterization point."  The vast majority
// of inputs are supplied in terms of known characterization points, such as
// "clock frequency X requires turbo voltage" and the maximum of these modes is
// used at any time.  The second element is voltage, which is fairly self
// explanatory--it is also aggregated to be the maximum of all requests.
//
// In order to facilitate the advanced control mechansims mentioned above,
// railway allows for the registration of pre-change and post-change callbacks
// on each voltage rail it supports.  These callbacks can then be used to
// implement the algorithms, typically using one of each (pre & post).  The
// callbacks receive input settings--these are the proposed new settings for
// prechange callbacks, and the currently effecitve settings in postchange
// callbacks.  The callback functions then return their desired modifications
// to those settings (which may be no modification at all).
//
// Each time the aggregated vote for a rail controlled by railway changes, the
// following algorithm is used to find the final voltage for the rail and place
// it into effect:
//   1) Determine the new proposed settings.
//   2) Call all prechange callbacks.  If any prechange callback returns a
//      modified setting, consider that the new proposed settings and return to
//      step (1).
//   3) Once all prechange callbacks have agreed on a proposed setting, make
//      that setting effective (i.e., program it in the PMIC).
//   4) Call all postchange callbacks.  If any postchange callback returns a
//      modified setting, consider that the new proposed settings and return to
//      step (1).
//   5) Once all postchange callbacks have agreed the existing setting is
//      satisfactory, the update is complete.
//
// The above process has the *potential* to be lengthy, but in practice the two
// features that railway supports (SVS and RBCPR) don't cause many unnecessary
// cycles back to step (1), and the transitions are relatively quick.
//
// SVS can be implemented as follows:
//  - a pre-change callback which needs to implement the following logic:
//      - if the voltage corner is being lowered and the clock required is
//        higher than that corner supports, then propse new settings with the
//        minimum required corner
//      - if the voltage corner is being lowered and the clock required is at
//        or below the level the proposed corner supports, then lower the clock
//        speed to the maximum clock supported by that corner
//  - a post-change callback which needs to implement the following logic:
//      - ensure the clock is running at the maximum speed for the selected
//        corner
//      - in practice, this may raise the clock speed if the corner was raised
//  - if an event occurs that causes a higher clock rate to become necessary,
//    railway_refresh can be used to begin another voltage adjustment pass.
//
// RBCPR can be implemented as follows:
//  - a pre-change callback that modifies the voltage (but not the corner!) to
//    some initial setting
//  - a post-change callback which establishes that voltage as sufficient, and
//    potentially modifies the voltage further if more voltage is required
//  - the post-change callback may additionally set up periodic/continuous
//    monitoring of the voltage rail.  in the event that continuous monitoring
//    indicates that a change is required, railway_refresh can be used to begin
//    another voltage adjustment pass.
//

#ifndef RAILWAY_H
#define RAILWAY_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdbool.h>
#include "pm_resources_and_types.h"
#include "voltage_level.h"	

//Forward Declerations
typedef struct railway_voter_s* railway_voter_t;
typedef struct railway_sw_mode_voter_s* railway_sw_mode_voter_t;


#define RAIL_NOT_SUPPORTED_BY_RAILWAY -1

typedef enum
{
  RAILWAY_NO_REQUEST         = 0,
  RAILWAY_RETENTION          = 1,
  RAILWAY_SVS_LOW            = 2,
  RAILWAY_SVS_SOC            = 3,
  RAILWAY_SVS_HIGH           = 4,
  RAILWAY_NOMINAL            = 5,
  RAILWAY_NOMINAL_HIGH       = 6,
  RAILWAY_TURBO              = 7,
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
    RAILWAY_OXILI_WORKAROUND_VOTER,
    RAILWAY_RPM_EBI_VOTER_ID,
    RAILWAY_RPM_EBI_SLEEP_VOTER_ID,
} railway_voter_id;

/**
 *   get device configuration data out of the CDT.
 */
typedef enum
{
  RAILWAY_CX_APC_COMBINED                     = 0x00,
  RAILWAY_CX_APC_SPLIT_EXTERNAL_BUCK          = 0x01,
  RAILWAY_CX_APC_SPLIT                        = 0x02,
  /** @cond */
  RAILWAY_32BITS  = 0x7FFFFFFF
  /** @endcond */
} railway_TargetType;


// railway_settings structure passed back to client of railway callbacks.
// mode       - the highest corner vote for this rail.
// microvolts - the voltage that the rail is set to.                
typedef struct
{
  railway_corner mode;
  rail_voltage_level  vol_level;
  unsigned int   microvolts;
} railway_settings;

typedef void (*railway_cb)(const railway_settings *settings, void* cookie);

/// Initialize the railway driver and all of its associated rail control.
void railway_init(void);

//Do the target specific things to initialize rails to voltages. Needs to be done
//just after the PMIC driver is initialized.
void railway_init_proxies_and_pins(void);

//Needs to be called at the end of RPM init to finalize Railway init
void railway_finalize_init(void);

// Return the ID for the named rail on this target.
// The rail ID will be differnt from target to target depending on which rails railway is controlling.
// Returns RAIL_NOT_SUPPORTED_BY_RAILWAY if the rail isn't controlled by railway.
int rail_id(const char* rail);

// Sets a callback on a particular event (pre/post change) for a particular voltage rail.
// The cookie passed in will be passed back in the callback, can be NULL.
// This function asserts that there is no error and that the callback can be correctly added.
void railway_set_callback(int rail, railway_cb_type type, railway_cb cb, void* cookie);

// After changing votes, call this function to invoke a voltage change to the lowest acceptible
// value on the given rail.
void railway_transition_rail(int rail);

// Will assert that voter can be alloced - will not return NULL.
// suppressible voters are those whose votes can be ignored for vdd_min calculations. Probably only
// svs will make a supressible vote. All votes on behalf of masters should be non-suppressible.
// id should be the master id if the voter is created on behalf of a master, or an entry from railway_voter_id
// for internal voters.
railway_voter_t railway_create_voter(int rail, bool suppressible, int id);

// Update which corner this voter is voting for.
// Will NOT update the vote that this voter is making for explicit voltages should it have one.
// Will NOT invoke the railway transitioner to transition the voltage - call railway_invoke_voltage_change
// to invoke the voltage change.
void railway_corner_vote(railway_voter_t voter, railway_corner corner);

// Update which voltage level this voter is voting for.
// Will NOT update the vote that this voter is making for explicit voltages should it have one.
// Will NOT invoke the railway transitioner to transition the voltage - call railway_invoke_voltage_change
// to invoke the voltage change.
void railway_level_vote(railway_voter_t voter, rail_voltage_level vol_level);

// Update the explicit voltage that this voter is voting for.
// Will NOT update the vote that this voter is making for an explicit voltage should it have one.
// Will NOT invoke the railway transitioner to transition the voltage - call railway_invoke_voltage_change
// to invoke the voltage change.
void railway_uv_vote(railway_voter_t voter, uint32 microvolts);

void railway_sw_enable_vote(railway_voter_t voter, bool sw_enable);

// Update the active floor that this voter is voting for.
// Will NOT update the vote that this voter is making for explicit voltages should it have one.
// Will NOT invoke the railway transitioner to transition the voltage - call railway_invoke_voltage_change
// to invoke the voltage change.
void railway_active_floor_vote(railway_voter_t voter, railway_corner corner);

// Update the active floor that this voter is voting for.
// Will NOT update the vote that this voter is making for explicit voltages should it have one.
// Will NOT invoke the railway transitioner to transition the voltage - call railway_invoke_voltage_change
// to invoke the voltage change.
void railway_active_floor_voltage_level_vote(railway_voter_t voter,rail_voltage_level vol_level);

//API used by CPR to set the value for a corner. Does not actually transition the rail.
void railway_set_corner_voltage(int rail, railway_corner corner, uint32 microvolts);

//API used by CPR to get the value for a corner.
uint32 railway_get_corner_voltage(int rail, railway_corner corner);

//Returns a copy of the current settings of the specified rail.
void railway_get_current_settings(int rail, railway_settings* settings);

//Returns a copy of the sleep settings of the specified rail.
void railway_get_sleep_settings(int rail, railway_settings* settings);

//Temp fn to enable a proxy vote on mx from apps in early boot.
//Also required to remove the proxy vote during DDR training.
void set_apps_proxy_vote_on_mx(bool proxy_vote_enabled);

// Returns whether the voter with the specified id is preventing vdd min. If the voter is preventing
// vdd min then this function will return true, otherwise it will return false.
// If there is no voter with the specified id then then function will return false.
// IDs 0-3 correspond to the master votes on cx.
bool railway_voter_preventing_vdd_min(int id);

// Returns the retention voltage of the given railway
void railway_get_sleep_voltages(int cx_id, int mx_id, uint32 *cx_microvolts, uint32 *mx_microvolts);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // RAILWAY_H

