/******************************************************************************
  @file    OptsHandler.h
  @brief   Implementation for handling operations

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __OPTS_HANDLER__H_
#define __OPTS_HANDLER__H_

#include "ResourceInfo.h"
#include "OptsData.h"

enum req_action_type {
    ADD_NEW_REQUEST = 0,
    ADD_AND_UPDATE_REQUEST = 1,
    PEND_REQUEST = 2
};

typedef int (*ApplyOpts)(Resource &, OptsData &);
typedef int (*ResetOpts)(Resource &, OptsData &);
typedef int (*CompareOpts)(unsigned int, unsigned int);

typedef struct OptsTable {
    ApplyOpts mApplyOpts;
    ResetOpts mResetOpts;
    CompareOpts mCompareOpts;
}OptsTable;

class OptsHandler {
    private:
        OptsTable mOptsTable[MAX_MINOR_RESOURCES];

        //singleton
        static OptsHandler mOptsHandler;

    private:
        OptsHandler();
        OptsHandler(OptsHandler const& oh);
        OptsHandler& operator=(OptsHandler const& oh);

    private:
        //all minor resource actions support functions
        static int update_core_based_param(int iscorebased, const char node[NODE_MAX],
                                           const char node_core[NODE_MAX],
                                           const char node_strg[NODE_MAX], int node_strg_l,
                                           const char *str);
    private:
        //all minor resource actions
        static int dummy(Resource &r, OptsData &d);

        /*pc*/
        static int toggle_power_collapse(Resource &r, OptsData &d);
        /*set pmQos latency values*/
        static int pmQoS_cpu_dma_latency(Resource &r, OptsData &d);

        /*cpu freq actions*/
        static int cpu_options(Resource &r, OptsData &d);

        /*sched actions*/
        static int sched_prefer_idle(Resource &r, OptsData &d);
        static int sched_migrate_cost(Resource &r, OptsData &d);
        static int sched_small_task(Resource &r, OptsData &d);
        static int sched_mostly_idle_load(Resource &r, OptsData &d);
        static int sched_mostly_idle_nr_run(Resource &r, OptsData &d);
        static int sched_mostly_idle_freq(Resource &r,  OptsData &d);
        static int sched_init_task_load(Resource &r, OptsData &d);
        static int sched_upmigrate(Resource &r, OptsData &d);
        static int sched_downmigrate(Resource &r, OptsData &d);
        static int sched_boost(Resource &r, OptsData &d);
        static int sched_spill_nr_run(Resource &r, OptsData &d);
        static int sched_static_cpu_pwr_cost(Resource &r, OptsData &d);
        static int sched_restrict_cluster_spill(Resource &r, OptsData &d);
        static int sched_add_freq_aggr_group(Resource &r, OptsData &d);
        static int sched_reset_freq_aggr_group(Resource &r, OptsData &d);
        static int sched_update_cpuset(Resource &r, char *cpuset_node);
        static int sched_update_top_app_cpuset(Resource &r, OptsData &d);
        static int sched_update_fg_cpuset(Resource &r, OptsData &d);
        static int sched_update_sys_bg_cpuset(Resource &r, OptsData &d);
        static int sched_update_bg_cpuset(Resource &r, OptsData &d);
        static int sched_restore_cpuset(Resource &r, char *cpuset_node);
        static int sched_restore_top_app_cpuset(Resource &r, OptsData &d);
        static int sched_restore_fg_cpuset(Resource &r, OptsData &d);
        static int sched_restore_sys_bg_cpuset(Resource &r, OptsData &d);
        static int sched_restore_bg_cpuset(Resource &r, OptsData &d);
        static int sched_set_freq_aggr(Resource &r, OptsData &d);
        static int sched_set_enable_thread_grouping(Resource &r, OptsData &d);

        /*corectl actions*/
        static int lock_min_cores(Resource &r, OptsData &d);
        static int lock_max_cores(Resource &r, OptsData &d);

        /*interactive actions*/
        static int interactive_above_hispeed_delay(Resource &r, OptsData &d);
        static int interactive_boost(Resource &r, OptsData &d);
        static int interactive_boostpulse(Resource &r, OptsData &d);
        static int interactive_boostpulse_duration(Resource &r, OptsData &d);
        static int interactive_go_hispeed_load(Resource &r, OptsData &d);
        static int interactive_hispeed_freq(Resource &r, OptsData &d);
        static int interactive_io_is_busy(Resource &r, OptsData &d);
        static int interactive_min_sample_time(Resource &r, OptsData &d);
        static int interactive_target_loads(Resource &r, OptsData &d);
        static int interactive_timer_rate(Resource &r, OptsData &d);
        static int interactive_timer_slack(Resource &r, OptsData &d);
        static int interactive_max_freq_hysteresis(Resource &r, OptsData &d);
        static int interactive_use_sched_load(Resource &r, OptsData &d);
        static int interactive_use_migration_notif(Resource &r, OptsData &d);
        static int interactive_ignore_hispeed_notif(Resource &r, OptsData &d);

        /*cpubw hwmon actions*/
        static int cpubw_hwmon_min_freq(Resource &r, OptsData &d);
        static int cpubw_hwmon_decay_rate(Resource &r, OptsData &d);
        static int cpubw_hwmon_io_percent(Resource &r, OptsData &d);
        static int cpubw_hwmon_hyst_opt(Resource &r, OptsData &d);
        static int cpubw_hwmon_low_power_ceil_mbps(Resource &r, OptsData &d);
        static int cpubw_hwmon_low_power_io_percent(Resource &r, OptsData &d);
        static int cpubw_hwmon_max_freq(Resource &r, OptsData &d);
        static int cpubw_hwmon_polling_interval(Resource &r, OptsData &d);
        static int cpubw_hwmon_sample_ms(Resource &r, OptsData &d);
        static int cpubw_hwmon_idle_mbps(Resource &r, OptsData &d);

        /*video hints actions*/
        static int handle_disp_hint(Resource &r, OptsData &d);
        static int handle_vid_decplay_hint(Resource &r, OptsData &d);
        static int handle_vid_encplay_hint(Resource &r, OptsData &d);

        /*ksm actions*/
        static int disable_ksm(Resource &r, OptsData &d);
        static int enable_ksm(Resource &r, OptsData &d);
        static int set_ksm_param(Resource &r, OptsData &d);
        static int reset_ksm_param(Resource &r, OptsData &d);

        /*ondemand actions*/
        static int sampling_rate(Resource &r, OptsData &d);
        static int ondemand_io_is_busy(Resource &r, OptsData &d);
        static int ondemand_sampling_down_factor(Resource &r, OptsData &d);
        static int sync_freq(Resource &r, OptsData &d);
        static int optimal_freq(Resource &r, OptsData &d);
        static int toggle_step_up(Resource &r, OptsData &d);
        static int write_max_intr_steps(Resource &r, OptsData &d);
        static int toggle_thread_migration_sync(Resource &r, OptsData &d);

        /*gpu actions*/
        static int gpu_default_pwrlvl(Resource &r, OptsData &d);
        static int gpu_min_pwrlvl(Resource &r, OptsData &d);
        static int gpu_max_pwrlvl(Resource &r, OptsData &d);

        /*miscellaneous actions, irq*/
        static int unsupported(Resource &r, OptsData &d);
        static int irq_balancer(Resource &r, OptsData &d);
        static int input_boost_reset(Resource &r, OptsData &d);
        static int add_sched_group(Resource &r, OptsData &d);
        static int reset_sched_group(Resource &r, OptsData &d);
        static int swap_ratio(Resource &r, OptsData &d);
        static int storage_clk_scaling_disable(Resource &r, OptsData &d);
        static int keep_alive(Resource &r, OptsData &d);
        static void* keep_alive_thread(void*);

        /*compare functions*/
        static int lower_is_better(unsigned int reqLevel, unsigned int curLevel);
        static int higher_is_better(unsigned int reqLevel, unsigned int curLevel);
        static int always_apply(unsigned int reqLevel, unsigned int curLevel);

        /*Validating function for cluster and core*/
        static int ValidateClusterAndCore(int cluster, int core, int resourceStatus);

        /*miscellaneous utility functions*/
        static int write_sched_freq_aggr_group(int tid, bool enable);
        static char *cpuset_bitmask_to_str(unsigned int cpuset_bitmask);

    public:
        ~OptsHandler();

        int Init();

        int ApplyOpt(Resource &r);
        int ResetOpt(Resource &r);
        int CompareOpt(int qindx, unsigned int reqVal, unsigned int curVal);

        static OptsHandler &getInstance() {
            return mOptsHandler;
        }
};

#endif /*__OPTS_HANDLER__H_*/
