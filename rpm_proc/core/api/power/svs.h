#ifndef SVS_H
#define SVS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef enum
{
  SVS_SLOW = 0,
  SVS_FAST,
  SVS_FASTER,
  SVS_NUM_MODES
} svs_mode_id;

void svs_init(void);

// Request that the SVS driver re-assess the RPM's workload and adjust the RPM
// speed accordingly.
//
// Typical use scenarios:
//    - The RPM is going into halt (not chip sleep!)
//        - In this case, the RPM speed may be reduced to allow system-wide SVS to occur.
//        - The RPM speed may even be reduced below the "normal" SVS speed if there is extra time.
//    - The RPM is coming out of halt or chip sleep
//        - In this case, the RPM speed may need to be increased to optimally process the incoming workload.
//    - The RPM receives new work (while running).
//        - In this case, the RPM speed may need to be increased to optimally process the incoming workload.
//
// Note: we do *not* call this function when we complete work.  In such a case,
// we *might* want to lower the RPM clock speed--but instead of doing that
// immediately, we instead churn through our remaining workload in order to get
// to sleep faster.  Stopping to "slow down" is going to burn more power than
// just finishing and getting to sleep.
bool svs_update_speed(int change_speed);

// Return the duration for switching SVS speed
unsigned svs_get_switch_time(void);

// Return the current speed at which the RPM is running.
svs_mode_id svs_current_speed(void);

// return true if there is possibility of cpu clk switch
bool svs_speed_check_and_update(void);

// get svs_switch_to_fast mode
bool svs_get_svs_switch_to_fast(void);
// set svs_switch_to_fast mode
void svs_set_svs_switch_to_fast(bool);

// set RPM speed
void svs_set_mode(unsigned int mode, bool passive);

// Return the current frequency in kHz at which the RPM is running.
unsigned int svs_get_freq(svs_mode_id mode);

void svs_rpm_clock_xo(void);
void svs_rpm_clock_svs_slow(void);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // SVS_H

