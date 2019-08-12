/*===========================================================================
                              svs.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                     Includes and Variable Definitions
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------------
#include "npa.h"
#include "railway.h"
#include "rpm_sched.h"
#include "time_service.h"
#include "CoreVerify.h"
#include "railway.h"
#include "svs.h"
#include "swevent.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//--------------------------------------------------------------------------
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

// Reasons to choose your clock rate (for logging).
#define SVS_IDLE                0
#define SVS_SPEEDUP             1
#define SVS_NO_SPEEDUP          2
#define SVS_IMMINENT_PROCESSING 3
#define SVS_SCHEDULE_IS_FULL    4
#define SVS_EXTERNAL_VOTE       5

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct
{
  unsigned int   clock_freq_khz;
  railway_corner voltage_requirement;
} svs_mode;

//---------------------------------------------------------------------------
// Global Constant Definitions
//---------------------------------------------------------------------------
unsigned int svs_switch_time = 0;

//---------------------------------------------------------------------------
// Local Object Definitions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Static Variable Definitions
//---------------------------------------------------------------------------
static const char *svs_resources[] = {"/clk/cpu"};
static npa_client_handle cpu_handle;
static railway_voter_t vddcx_handle;
static int vddcx_rail_id;

// Our current operating mode--initially as fast as possible.
static unsigned int svs_inited             = 0;
static unsigned int svs_current_mode       = SVS_FAST;
static unsigned int svs_last_clock_request = 0;

static const svs_mode svs_modes[SVS_NUM_MODES] =
#if (DAL_CONFIG_TARGET_ID == 0x9645)
{
  { .clock_freq_khz = 200000, .voltage_requirement = RAILWAY_SVS_SOC }, // standard RPM "slow" mode
  { .clock_freq_khz = 240000, .voltage_requirement = RAILWAY_NOMINAL }, // standard RPM "fast" mode
  { .clock_freq_khz = 300000, .voltage_requirement = RAILWAY_SUPER_TURBO }, // standard RPM "faster" mode
};
#else
{
  { .clock_freq_khz = 100000, .voltage_requirement = RAILWAY_SVS_SOC }, // standard RPM "slow" mode
  { .clock_freq_khz = 150000, .voltage_requirement = RAILWAY_NOMINAL }, // standard RPM "fast" mode
};
#endif

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// External References
//---------------------------------------------------------------------------


//===========================================================================
//                           Function Definitions
//===========================================================================

/*===========================================================================
FUNCTION: svs_current_speed

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
svs_mode_id svs_current_speed(void)
{
  if (svs_modes[SVS_FASTER].clock_freq_khz == svs_last_clock_request)
    return SVS_FASTER;
  if (svs_modes[SVS_FAST].clock_freq_khz == svs_last_clock_request)
    return SVS_FAST;
  if (svs_modes[SVS_SLOW].clock_freq_khz == svs_last_clock_request)
    return SVS_SLOW;
  CORE_VERIFY(0);
}

/*===========================================================================
FUNCTION: svs_get_freq

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
unsigned int svs_get_freq(svs_mode_id mode)
{
  return svs_modes[mode].clock_freq_khz;
}

/*===========================================================================
FUNCTION: svs_set_mode

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void svs_set_mode(unsigned int mode, bool passive)
{
  uint64_t start, end;

  CORE_VERIFY(mode < SVS_NUM_MODES);

  start = time_service_now();

  if(mode != svs_current_mode || !svs_inited)
  {
    svs_inited = 1;
    svs_current_mode = mode;
    railway_corner_vote(vddcx_handle, svs_modes[mode].voltage_requirement);

    // If passive is true, we just need to put our vote in and not actually transition the rail.
    // This will be the case if svs_set_mode is being called from a railway callback.
    if(!passive)
    {
      railway_transition_rail(vddcx_rail_id);
    }
  }

  // Record how long this took if it's longer than we've seen before.
  end = time_service_now();
  if(end - start > svs_switch_time)
    svs_switch_time = end - start;
}

/*===========================================================================
FUNCTION: svs_prechange_cb

DESCRIPTION:
// The VDDCX prechange callback lets us lower our clock vote when the voltage
// has aggregated to a lower status (we also have a vote on the voltage, which
// is how we forcibly raise the clock).

RETURN VALUE:
===========================================================================*/
static void svs_prechange_cb(const railway_settings *settings, void* cookie)
{
  unsigned int clock_vote = 0;
  unsigned int mode_vote = 0;

  for(unsigned int i = 0; ((i < SVS_NUM_MODES) && (svs_modes[i].clock_freq_khz)); ++i)
  {
    if(svs_modes[i].voltage_requirement <= settings->mode)
    {
      clock_vote = svs_modes[i].clock_freq_khz; // voltage is sufficient for this clock rate
      mode_vote  = i;
    }
  }

  if(clock_vote < svs_last_clock_request)
  {
    // The acceptable clock rate is lower, so we need to lower our vote.
    npa_issue_suppressible_request(cpu_handle, clock_vote);
    svs_last_clock_request = clock_vote;
    sched_cpu_speed_change(clock_vote);

    if (clock_vote > svs_modes[svs_current_mode].clock_freq_khz)
    {
      // External vote TURBO->NOMINAL, and we're asking for slow
      SWEVENT(RPM_SVS_FAST_EXTERNAL_VOTE, svs_current_mode, mode_vote);
      // Lock in our decision as being for high clock; we don't want to have to change again until we go idle.
      svs_set_mode(mode_vote, true);
    }

    // We hold vddcx high when we want a higher clock, so at this point we
    // should be ok with the clock level being lower, or something is wrong.
    if(clock_vote != svs_modes[svs_current_mode].clock_freq_khz)
      ERR_FATAL("RPM clock lowered below expected floor.", 0, 0, 0);
  }
}

/*===========================================================================
FUNCTION:

DESCRIPTION:
The VDDCX postchange callback gives us the opportunity to ensure we're
running as fast as we can for the current voltage selection.

RETURN VALUE:
===========================================================================*/
static void svs_postchange_cb(const railway_settings *settings, void* cookie)
{
  unsigned int clock_vote = 0;
  unsigned int mode_vote = 0;

  for(unsigned int i = 0; ((i < SVS_NUM_MODES) && (svs_modes[i].clock_freq_khz)); ++i)
  {
    if(svs_modes[i].voltage_requirement <= settings->mode)
    {
      clock_vote = svs_modes[i].clock_freq_khz; // voltage is sufficient for this clock rate
      mode_vote  = i;
    }
  }

  if(clock_vote > svs_last_clock_request)
  {
    // The acceptable clock rate is higher, so we can safely increase our clock vote.
    npa_issue_suppressible_request(cpu_handle, clock_vote);
    svs_last_clock_request = clock_vote;
    sched_cpu_speed_change(clock_vote);

    // Put a handy log message noting that we raised the clock because we had the opportunity.
    CORE_VERIFY(mode_vote >= SVS_FAST);
    SWEVENT(RPM_SVS_FAST_EXTERNAL_VOTE, svs_current_mode, mode_vote);

    // Lock in our decision as being for high clock; we don't want to have to change again until we go idle.
    svs_set_mode(mode_vote, true);
  }
}

/*===========================================================================
FUNCTION: svs_resources_ready

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
static void svs_resources_ready(void *unused_context, unsigned int unused, const char **resources, unsigned int num_resources)
{
//  npa_client_handle sfpb_handle;

  // Create the requisite NPA clients.
  CORE_VERIFY_PTR(cpu_handle   = npa_create_sync_client("/clk/cpu",    "rpm svs", NPA_CLIENT_SUPPRESSIBLE));

  vddcx_rail_id = rail_id("vddcx");
  CORE_VERIFY(vddcx_rail_id!=RAIL_NOT_SUPPORTED_BY_RAILWAY);

  vddcx_handle = railway_create_voter(vddcx_rail_id, true, RAILWAY_SVS_VOTER_ID);

  // Attach ourselves to the VDDCX callback system.
  railway_set_callback(vddcx_rail_id, RAILWAY_PRECHANGE_CB,  svs_prechange_cb, NULL);
  railway_set_callback(vddcx_rail_id, RAILWAY_POSTCHANGE_CB, svs_postchange_cb, NULL);

  svs_set_mode(svs_current_mode, false);
  if (!svs_last_clock_request)
  {
    unsigned int clock_vote = svs_modes[svs_current_mode].clock_freq_khz;
    npa_issue_suppressible_request(cpu_handle, clock_vote);
    svs_last_clock_request = clock_vote;
    sched_cpu_speed_change(svs_modes[svs_current_mode].clock_freq_khz);
  }
}

/*===========================================================================
FUNCTION: svs_init

DESCRIPTION:

RETURN VALUE:
===========================================================================*/
void svs_init(void)
{
  npa_resources_available_cb(ARRAY_SIZE(svs_resources), svs_resources, (npa_callback)svs_resources_ready, NULL);
}

/*===========================================================================
FUNCTION: svs_get_switch_time

DESCRIPTION:

RETURN VALUE: duration for switching SVS speed
===========================================================================*/

unsigned svs_get_switch_time(void)
{
  return svs_switch_time;
}

/*===========================================================================
FUNCTION: svs_switch_to_fast_possible

DESCRIPTION:
Determine RPM clk switch possible for given slow_clock duration

RETURN VALUE:
===========================================================================*/
static bool svs_switch_to_fast_possible(uint64_t old_duration, bool deep_sleep_ctx)
{
  uint64_t new_duration = ((3 * old_duration) / 4);  // rough estimate though

  if ((new_duration + svs_switch_time) < old_duration)
  {
    if(!deep_sleep_ctx)
      SWEVENT(RPM_SVS_FAST_SPEEDUP, old_duration, new_duration, svs_switch_time);
    else
      SWEVENT(RPM_SVS_SCHED_UPDATED_TO_FAST, old_duration, new_duration, svs_switch_time);

    return true;
 }

  SWEVENT(RPM_SVS_SLOW_NO_SPEEDUP, old_duration, new_duration, svs_switch_time);
  return false;
}

/*===========================================================================
FUNCTION: svs_update_speed

DESCRIPTION:
This is "it" -- the meat, the business logic, the decider, the place we pick... fast or slow?

RETURN VALUE:
===========================================================================*/
bool svs_update_speed(int change_speed)
{
  if(!cpu_handle)
    return false; // the system is not fully booted yet

  int have_imminent_work = ((time_service_now() + 3 * svs_switch_time) >= sched_get_next_start());

  // There are 2 independent boolean states that affect the SVS decision making
  // which gives us 4 possible paths to take.  2 of those paths short circuit,
  // and 2 need to go on to make a "smarter" decision.

  // The first short-circuit path: we haev no immediate work to do, and either
  //   1) we're already running slow, so we have nothing to do, or
  //   2) XO shutdown is enabled
  //
  // In this case we're likely on our way to sleep.  The current theory is that
  // it isn't worth increasing VDDCX just so it can leak more during XO
  // shutdown.  On the flip side of that though, if we're imminently headed for
  // XO shutdown / Vdd Min then we want to get into that mode as quickly as
  // possible--that is, stay at whatever clock we're at, even if that's fast.
  if(!have_imminent_work && !change_speed)
    return false;

  if(!have_imminent_work && (svs_current_mode == SVS_SLOW))
    return false;

  // The second short-circuit path: we're running fast and we have work.  No
  // reason to slow down now--so just keep chugging away at high.  We have to
  // reissue the fast request though, to make sure that handling the request
  // doesn't cause us to drop lower than we want.
  if((svs_last_clock_request >= svs_modes[SVS_FAST].clock_freq_khz) && have_imminent_work)
  {
    SWEVENT(RPM_SVS_FAST_IMMINENT_PROCESSING, svs_last_clock_request);
    svs_set_mode(SVS_FAST, false);
    return false;
  }

  // Now the "smart" logic where we need to make a real decision.  First, if
  // we're requesting that we run fast and we have no work, ask the scheduler
  // if we can safely lower the clock rate while we're idle.
  // FIXME: probably should check whether vddcx would be held high anyway,
  //        before wasting time on checking to see if our schedule allows this
  //        (checking the schedule is expensive)
  if(SVS_FAST <= svs_current_mode && !have_imminent_work)
  {
    if(!sched_cpu_speed_change_ok(svs_modes[SVS_SLOW].clock_freq_khz)) // don't lower if it would make us late
    {
      SWEVENT(RPM_SVS_FAST_SCHEDULE_IS_FULL);
      return false;
    }

    SWEVENT(RPM_SVS_SLOW_IDLE);
    svs_set_mode(SVS_SLOW, false);
    return true;
  }

  bool ret_val = false;

  // On the flip side, if we're asking for slow and we have enough work that it
  // makes sense to switch up to the next threshold, do it.
  if(SVS_SLOW == svs_current_mode && have_imminent_work)
  {
    unsigned int old_duration = sched_get_next_duration(svs_modes[SVS_SLOW].clock_freq_khz);

    if(svs_switch_to_fast_possible(old_duration, 0))
    {
      svs_set_mode(SVS_FAST, false);
      ret_val = true;
    }
  }
  return ret_val;
}

bool svs_switch_to_fast_mode = false;

/*===========================================================================
FUNCTION: svs_get_svs_switch_to_fast

DESCRIPTION:
return svs_switch_fast mode

RETURN VALUE:
===========================================================================*/
bool svs_get_svs_switch_to_fast(void)
{
  return svs_switch_to_fast_mode;
}

/*===========================================================================
FUNCTION: svs_set_svs_switch_to_fast

DESCRIPTION:
set svs_switch_fast mode

RETURN VALUE:
===========================================================================*/
void svs_set_svs_switch_to_fast(bool value)
{
  svs_switch_to_fast_mode = value;
}

/*===========================================================================
FUNCTION: svs_speed_check_and_update

DESCRIPTION:
Determine if there is possible RPM clk switch happens after coming out of deep sleep

RETURN VALUE:
===========================================================================*/
bool svs_speed_check_and_update(void)
{
  svs_switch_to_fast_mode = false;

  if(SVS_SLOW == svs_current_mode)
  {
    uint64_t old_duration = sched_get_next_workload();

    //check if we jump to FAST mode based on svs algorithm
    if(svs_switch_to_fast_possible(old_duration, 1))
    {
      //re-estimate task start time for FAST clock
      sched_cpu_speed_for_fast(svs_modes[SVS_FAST].clock_freq_khz);
      svs_switch_to_fast_mode = true;
    }
  }

  return svs_switch_to_fast_mode;
}

void svs_rpm_clock_xo(void)
{
    npa_issue_suppressible_request(cpu_handle, 0);
}

void svs_rpm_clock_svs_slow(void)
{
  npa_issue_suppressible_request(cpu_handle, svs_last_clock_request);
}

