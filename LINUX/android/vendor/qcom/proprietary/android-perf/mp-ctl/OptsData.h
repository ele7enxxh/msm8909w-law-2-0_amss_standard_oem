/******************************************************************************
  @file    OptsData.h
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __OPTS_DATA__H_
#define __OPTS_DATA__H_

#include <cstdlib>
#include <fcntl.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <unordered_set>
#include "Target.h"
#include "MpctlUtils.h"
#include <utils/TypeHelpers.h>

#define SYSFS_PREFIX            "/sys/devices/system/"
#define DEVFREQ_PATH            "/sys/class/devfreq/"
#define CPUP                    "/sys/devices/system/cpu/present"

#define SCHED_UPMIGRATE         "/proc/sys/kernel/sched_upmigrate"
#define SCHED_DOWNMIGRATE       "/proc/sys/kernel/sched_downmigrate"
#define SCHED_SMALL_TASK        "/proc/sys/kernel/sched_small_task"
#define SCHED_MOSTLY_IDLE_LOAD  "/proc/sys/kernel/sched_mostly_idle_load"
#define SCHED_MOSTLY_IDLE_LOAD_CORE "/sys/devices/system/cpu/cpu%d/sched_mostly_idle_load"
#define SCHED_MOSTLY_IDLE_NR_RUN "/proc/sys/kernel/sched_mostly_idle_nr_run"
#define SCHED_MOSTLY_IDLE_NR_RUN_CORE "/sys/devices/system/cpu/cpu%d/sched_mostly_idle_nr_run"
#define SCHED_MOSTLY_IDLE_FREQ "/proc/sys/kernel/sched_mostly_idle_freq"
#define SCHED_MOSTLY_IDLE_FREQ_CORE "/sys/devices/system/cpu/cpu%d/sched_mostly_idle_freq"
#define SCHED_INIT_TASK_LOAD    "/proc/sys/kernel/sched_init_task_load"
#define SCHED_PREFER_IDLE_NODE  "/proc/sys/kernel/sched_prefer_idle"
#define SCHED_PREFER_IDLE_NODE_CORE  "/sys/devices/system/cpu/cpu%d/sched_prefer_idle"
#define SCHED_MIGRATE_COST_NODE "/proc/sys/kernel/sched_migration_cost_ns"
#define SCHED_BOOST_NODE "/proc/sys/kernel/sched_boost"
#define SCHED_SPILL_NR_RUN_NODE "/proc/sys/kernel/sched_spill_nr_run"
#define SCHED_STATIC_CPU_PWR_COST_CORE  "/sys/devices/system/cpu/cpu%d/sched_static_cpu_pwr_cost"
#define SCHED_RESTRICT_CLUSTER_SPILL_NODE  "proc/sys/kernel/sched_restrict_cluster_spill"
#define SCHED_FREQ_AGGREGATE_NODE  "/proc/sys/kernel/sched_freq_aggregate"
#define SCHED_ENABLE_THREAD_GROUPING  "/proc/sys/kernel/sched_enable_thread_grouping"
#define SCHED_TOP_APP_CPUSET "/dev/cpuset/top-app/cpus"
#define SCHED_FOREGROUND_CPUSET "/dev/cpuset/foreground/cpus"
#define SCHED_BACKGROUND_CPUSET "/dev/cpuset/background/cpus"
#define SCHED_SYSTEM_BACKGROUND_CPUSET "/dev/cpuset/system-background/cpus"


#define INTERACTIVE_ABOVE_HISPEED_DELAY_NODE      "/sys/devices/system/cpu/cpufreq/interactive/above_hispeed_delay"
#define INTERACTIVE_ABOVE_HISPEED_DELAY_CORE "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/above_hispeed_delay"
#define INTERACTIVE_BOOST_NODE                    "/sys/devices/system/cpu/cpufreq/interactive/boost"
#define INTERACTIVE_BOOST_CORE               "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/boost"
#define INTERACTIVE_BOOSTPULSE_NODE               "/sys/devices/system/cpu/cpufreq/interactive/boostpulse"
#define INTERACTIVE_BOOSTPULSE_CORE          "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/boostpulse"
#define INTERACTIVE_BOOSTPULSE_DURATION_NODE      "/sys/devices/system/cpu/cpufreq/interactive/boostpulse_duration"
#define INTERACTIVE_BOOSTPULSE_DURATION_CORE "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/boostpulse_duration"
#define INTERACTIVE_GO_HISPEED_LOAD_NODE          "/sys/devices/system/cpu/cpufreq/interactive/go_hispeed_load"
#define INTERACTIVE_GO_HISPEED_LOAD_CORE     "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/go_hispeed_load"
#define INTERACTIVE_HISPEED_FREQ_NODE             "/sys/devices/system/cpu/cpufreq/interactive/hispeed_freq"
#define INTERACTIVE_HISPEED_FREQ_CORE        "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/hispeed_freq"
#define INTERACTIVE_IO_IS_BUSY_NODE               "/sys/devices/system/cpu/cpufreq/interactive/io_is_busy"
#define INTERACTIVE_IO_IS_BUSY_CORE          "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/io_is_busy"
#define INTERACTIVE_MIN_SAMPLE_TIME_NODE          "/sys/devices/system/cpu/cpufreq/interactive/min_sample_time"
#define INTERACTIVE_MIN_SAMPLE_TIME_CORE     "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/min_sample_time"
#define INTERACTIVE_TARGET_LOADS_NODE             "/sys/devices/system/cpu/cpufreq/interactive/target_loads"
#define INTERACTIVE_TARGET_LOADS_CORE        "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/target_loads"
#define INTERACTIVE_TIMER_RATE_NODE               "/sys/devices/system/cpu/cpufreq/interactive/timer_rate"
#define INTERACTIVE_TIMER_RATE_CORE          "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/timer_rate"
#define INTERACTIVE_TIMER_SLACK_NODE              "/sys/devices/system/cpu/cpufreq/interactive/timer_slack"
#define INTERACTIVE_TIMER_SLACK_CORE         "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/timer_slack"
#define INTERACTIVE_MAX_FREQ_HYSTERESIS_NODE      "/sys/devices/system/cpu/cpufreq/interactive/max_freq_hysteresis"
#define INTERACTIVE_MAX_FREQ_HYSTERESIS_CORE "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/max_freq_hysteresis"
#define INTERACTIVE_USE_SCHED_LOAD_NODE          "/sys/devices/system/cpu/cpufreq/interactive/use_sched_load"
#define INTERACTIVE_USE_SCHED_LOAD_CORE          "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/use_sched_load"
#define INTERACTIVE_USE_MIGRATION_NOTIF_NODE     "/sys/devices/system/cpu/cpufreq/interactive/use_migration_notif"
#define INTERACTIVE_USE_MIGRATION_NOTIF_CORE     "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/use_migration_notif"
#define INTERACTIVE_IGNORE_HISPEED_NOTIF_NODE     "/sys/devices/system/cpu/cpufreq/interactive/ignore_hispeed_on_notif"
#define INTERACTIVE_IGNORE_HISPEED_NOTIF_CORE     "/sys/devices/system/cpu/cpu%d/cpufreq/interactive/ignore_hispeed_on_notif"

#define KPM_MAX_CPUS            "/sys/module/msm_performance/parameters/max_cpus"
#define KPM_CPU_MIN_FREQ_NODE       "/sys/module/msm_performance/parameters/cpu_min_freq"
#define KPM_CPU_MAX_FREQ_NODE       "/sys/module/msm_performance/parameters/cpu_max_freq"

#define KSM_PREFIX              "/sys/kernel/mm/ksm/"
#define KSM_RUN_NODE  "/sys/kernel/mm/ksm/run"
#define KSM_SLEEP_MILLI_SECS_NODE "/sys/kernel/mm/ksm/sleep_millisecs"
#define KSM_PAGES_TO_SCAN_NODE "/sys/kernel/mm/ksm/pages_to_scan"

#define SWAP_RATIO_NODE "/proc/sys/vm/swap_ratio"

#define OND_SAMPLING_RATE_NODE "/sys/devices/system/cpu/cpufreq/ondemand/sampling_rate"
#define OND_IO_IS_BUSY_NODE "/sys/devices/system/cpu/cpufreq/ondemand/io_is_busy"
#define OND_SAMPLING_DOWN_FACTOR_NODE "/sys/devices/system/cpu/cpufreq/ondemand/sampling_down_factor"
#define OND_SYNC_FREQ_NODE "/sys/devices/system/cpu/cpufreq/ondemand/sync_freq"
#define OND_OPTIMAL_FREQ_NODE "/sys/devices/system/cpu/cpufreq/ondemand/optimal_freq"
#define OND_MAX_INT_STEPS "/sys/devices/system/cpu/cpufreq/ondemand/max_intermediate_steps"
#define OND_ENABLE_STEPUP "/sys/devices/system/cpu/cpufreq/ondemand/enable_stepup"

#define GPU_DEFAULT_PWRLVL      "/sys/class/kgsl/kgsl-3d0/default_pwrlevel"
#define GPU_MIN_PWRLVL      "/sys/class/kgsl/kgsl-3d0/min_pwrlevel"
#define GPU_MAX_PWRLVL      "/sys/class/kgsl/kgsl-3d0/max_pwrlevel"

#define KTM_ENABLE_NODE         "/sys/module/msm_thermal/core_control/enabled"
#define PM_QOS                "/sys/module/lpm_levels/parameters/sleep_disabled"
#define PM_QOS_CPU_DMA_DEV    "/dev/cpu_dma_latency"
#define CORE_CTL_MIN_CPU        (SYSFS_PREFIX"cpu/cpu%d/core_ctl/min_cpus")
#define CORE_CTL_MAX_CPU        (SYSFS_PREFIX"cpu/cpu%d/core_ctl/max_cpus")

#define AVL_FREQ_NODE          (SYSFS_PREFIX"cpu/cpu%d/cpufreq/scaling_available_frequencies")
#define CPUINFO_FREQ_NODE      (SYSFS_PREFIX"cpu/cpu%d/cpufreq/cpuinfo_%s_freq")
#define NOTIFY_ON_MIGRATE     "/dev/cpuctl/cpu.notify_on_migrate"
#define INPUT_BOOST_FREQ      "/sys/module/cpu_boost/parameters/input_boost_freq"
#define STORAGE_EMMC_CLK_SCALING_DISABLE "/sys/class/mmc_host/mmc0/clk_scaling/enable"
#define STORAGE_UFS_CLK_SCALING_DISABLE "/sys/class/scsi_host/host0/../../../clkscale_enable"

/*TODO Check its usage*/
#define c0f   "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_frequencies"                    // cpu0 available freq
/*TODO Not used till now, confirm once*/
#define c4f   "/sys/devices/system/cpu/cpu4/cpufreq/scaling_available_frequencies"                    // cpu4 available freq

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define NODE_MAX                150

#define MAX_CPUS            8

#define MAX_FREQ_CPUBW      20
#define FREQLIST_MAX            32
#define FREQLIST_STR            1024
#define ENABLE_PC_LATENCY   0

#define MAX_FREQ            50
#define FREQ_MULTIPLICATION_FACTOR  1000
#define TIMER_MULTIPLICATION_FACTOR 1000
#define FREAD_STR(fn, pstr, len, rc)    { int fd;                        \
                                          rc = -1;                       \
                                          fd = open(fn, O_RDONLY);       \
                                          if (fd >= 0) {                 \
                                              rc = read(fd, pstr, len);  \
                                              pstr[len-1] = '\0';        \
                                              close(fd); \
                                          } \
                                        }
#define FWRITE_STR(fn, pstr, len, rc)   { int fd;                        \
                                          rc = -1;                       \
                                          fd = open(fn, O_WRONLY);       \
                                          if (fd >= 0) {                 \
                                              rc = write(fd, pstr, len); \
                                              close(fd);                 \
                                          }                              \
                                        }
#define FOVRWT_STR(fn, pstr, len, rc)   { int fd;                        \
                                          rc = -1;                       \
                                          fd = open(fn, O_CREAT|O_WRONLY, S_IRUSR|S_IWUSR); \
                                          if (fd >= 0) {                 \
                                              rc = write(fd, pstr, len); \
                                              fsync(fd);                 \
                                              close(fd);                 \
                                          }                              \
                                        }

static const char *dnsIPs[] = {
    "8.8.8.8",/* Google */
    "8.8.4.4",/* Google */
    "114.114.114.114", /* 114DNS */
    "114.114.115.115", /* 114DNS */
    "1.2.4.8", /* CNNIC SDNS */
    "210.2.4.8", /* CNNIC SDNS */
    "223.5.5.5", /*  Ali DNS */
    "223.6.6.6", /* Ali DNS */
    "216.146.35.35", /* Dyn DNS */
    "216.146.36.36", /* Dyn DNS */
    "208.67.222.123", /* OpenDNS */
    "208.67.220.123" /* OpenDNS */
};

struct cpu_freq_mapping {
    const char * min;
    const char * max;
    const char * online;
    int in_use;
};

struct cpu_freq_resource_value {
    int avl_freq[MAX_FREQ];
    int count;
    int min_freq_pos;
    int valid;
};

typedef struct hint_associated_data {
    struct sigevent mSigEvent;
    struct itimerspec mInterval;
    timer_t tTimerId;
    int disp_single_layer;
    int vid_hint_state;
    int slvp_perflock_set;
    int vid_enc_start;
    int timer_created;
}hint_associated_data;

#define HWUI_SCHED_GROUP_SIZE 5
struct thread_group {
    int *tids;
    int num_tids;
    thread_group(): tids(NULL), num_tids(0) {
    }
};

class sched_group_data {
private:
    typedef android::key_value_pair_t<int,
                         thread_group> sched_group_key_value;

    struct HashForEntry : private std::unary_function<sched_group_key_value*, android::hash_t> {
        android::hash_t operator() (const sched_group_key_value* entry) const {
            return android::hash_type(entry->key);
        };
    };

    struct EqualityForHashedEntries : private std::unary_function<sched_group_key_value*, bool> {
        bool operator() (const sched_group_key_value* lhs, const sched_group_key_value* rhs) const {
            return lhs->key == rhs->key;
        };
    };


    typedef std::unordered_set<sched_group_key_value*, HashForEntry, EqualityForHashedEntries> mSchedGroupSet;

    typename mSchedGroupSet::iterator findByKey(const int& key) {
        sched_group_key_value entryForSearch(key, thread_group());
        typename mSchedGroupSet::iterator result = mSchedGroupHashSet.find(&entryForSearch);
        return result;
    }

    mSchedGroupSet mSchedGroupHashSet;

public:
    bool Add(int tid, thread_group tids_of_interest) {
        if (findByKey(tid) != mSchedGroupHashSet.end())
            return false;
        sched_group_key_value* newEntry = new sched_group_key_value(tid, tids_of_interest);
        mSchedGroupHashSet.insert(newEntry);

        return true;
    }

    thread_group Get(int tid) {
        thread_group t;
        typename mSchedGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedGroupHashSet.end())
            return t;
        sched_group_key_value *entry = *find_result;
        return entry->value;
    }

    thread_group Remove(int tid) {
        thread_group ret;
        typename mSchedGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedGroupHashSet.end())
            return ret;
        sched_group_key_value *entry = *find_result;
        ret = entry->value;
        mSchedGroupHashSet.erase(entry);
        delete(entry);
        return ret;
    }

    static bool isHWUIThread(char *name) {
        const char *thread_names[3] = { "RenderThread",
                                        "hwuiTask1",
                                        "hwuiTask2" };
        for (int i = 0; i < 3; i++) {
            char *eoline = strstr(name, "\n");
            char *out = strstr(name, thread_names[i]);
            if (eoline == (out + strlen(thread_names[i])))
                return true;
        }
        return false;
    }

    bool Found(int tid) {
        typename mSchedGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedGroupHashSet.end())
            return false;
        return true;
    }

    ~sched_group_data () {
        for (auto entry : mSchedGroupHashSet) {
            delete entry;
        }
        mSchedGroupHashSet.clear();
    }
};

class sched_freq_aggr_group_data {
private:
    typedef android::key_value_pair_t<int,
                         int> sched_freq_aggr_group_key_value;

    struct HashForEntry : private std::unary_function<sched_freq_aggr_group_key_value*, android::hash_t> {
        android::hash_t operator() (const sched_freq_aggr_group_key_value* entry) const {
            return android::hash_type(entry->key);
        };
    };

    struct EqualityForHashedEntries : private std::unary_function<sched_freq_aggr_group_key_value*, bool> {
        bool operator() (const sched_freq_aggr_group_key_value* lhs, const sched_freq_aggr_group_key_value* rhs) const {
            return lhs->key == rhs->key;
        };
    };


    typedef std::unordered_set<sched_freq_aggr_group_key_value*, HashForEntry, EqualityForHashedEntries> mSchedFreqAggrGroupSet;

    typename mSchedFreqAggrGroupSet::iterator findByKey(const int& key) {
        sched_freq_aggr_group_key_value entryForSearch(key, 0);
        typename mSchedFreqAggrGroupSet::iterator result = mSchedFreqAggrGroupHashSet.find(&entryForSearch);
        return result;
    }

    mSchedFreqAggrGroupSet mSchedFreqAggrGroupHashSet;
    bool sched_freq_aggr_present;
public:
    bool Add(int tid, int instanceRefCount) {
        sched_freq_aggr_group_key_value* newEntry = new sched_freq_aggr_group_key_value(tid, instanceRefCount);
        mSchedFreqAggrGroupHashSet.insert(newEntry);
        return true;
    }

    int Get(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return 0;
        sched_freq_aggr_group_key_value *entry = *find_result;
        return entry->value;
    }

    void Remove(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return;
        sched_freq_aggr_group_key_value *entry = *find_result;
        mSchedFreqAggrGroupHashSet.erase(entry);
        delete(entry);
    }

    void IncrementInsRefCntVal(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return;
        sched_freq_aggr_group_key_value *entry = *find_result;
        QLOGI("%s, pid=%d instanceRefCount=%d",__FUNCTION__, entry->key, entry->value);
        entry->value++;
    }

    void DecrementInsRefCntVal(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return;
        sched_freq_aggr_group_key_value *entry = *find_result;
        QLOGI("%s, pid=%d instanceRefCount=%d",__FUNCTION__, entry->key, entry->value);
        if (entry->value > 1)
            entry->value--;
    }

    bool Found(int tid) {
        typename mSchedFreqAggrGroupSet::const_iterator find_result = findByKey(tid);
        if (find_result == mSchedFreqAggrGroupHashSet.end())
            return false;
        return true;
    }

    ~sched_freq_aggr_group_data () {
        for (auto entry : mSchedFreqAggrGroupHashSet) {
            delete entry;
        }
        mSchedFreqAggrGroupHashSet.clear();
    }

    bool IsSchedFreqAggrPresent() {
        return sched_freq_aggr_present;
    }

    void checkSchedFreqAggrPresence() {
        int rc = -1;
        char sched_freq_aggregate_node_val[NODE_MAX];
        memset(sched_freq_aggregate_node_val, 0, NODE_MAX);
        FREAD_STR(SCHED_FREQ_AGGREGATE_NODE, sched_freq_aggregate_node_val, NODE_MAX, rc);

        if (rc > 0 && atoi(sched_freq_aggregate_node_val)) {
            sched_freq_aggr_present = 1;
            QLOGI("Sched Frequency Aggregation support present");
        } else {
            sched_freq_aggr_present = 0;
            QLOGE("Error: Sched Frequency Aggregation support not present");
        }
    }
};


class OptsData {
private:
    static OptsData mOptsData;

    friend class OptsHandler;
    friend class ResetHandler;

private:
    OptsData();
    OptsData(OptsData const& d);
    OptsData& operator=(OptsData const& d);

private:
    char iahd[NODE_MAX]; // interactive above_hispeed_delay node string
    char intb[NODE_MAX]; // interactive boost node
    char ibp[NODE_MAX];  // interactive boostpulse node
    char ibd[NODE_MAX];  // interactive boostpulse_duration node
    char ighl[NODE_MAX]; // interactive go_hispeed_load node
    char ihf[NODE_MAX];  // interactive hispeed_freq node
    char iiib[NODE_MAX]; // interactive io_is_busy node
    char imst[NODE_MAX]; // interactive min_sample_time node
    char itl[NODE_MAX];  // interactive target_loads node
    char itr[NODE_MAX];  // interactive timer_rate node
    char its[NODE_MAX];  // interactive timer_slack node
    char imf[NODE_MAX];  // interactive max_freq_hysteresis node
    char iusl[NODE_MAX]; // interactive use_sched_load
    char iumn[NODE_MAX]; // interactive use_migration_notif
    char iihn[NODE_MAX]; //interactive pro ignore_hispeed_notif

    char srate[NODE_MAX];  // ondemand sampling rate storage
    int  sratel;           // sampling rate string length
    char sr[NODE_MAX];     // sampling rate node string
    char oiib[NODE_MAX];   // ondemand io is busy node string
    char osdf[NODE_MAX];   // ondemand sampling down factor node string
    char sfreq[NODE_MAX];  // ondemand sync freq node string
    char ofreq[NODE_MAX];  // ondemand optimal freq node string
    char enbl_stp_up[NODE_MAX];    // enable_stepupnode
    char max_int_steps[NODE_MAX];  // max_int_steps

    char cpubw_path[NODE_MAX];
    char cpubw_minfreq_path[NODE_MAX];
    char cpubw_maxfreq_path[NODE_MAX];
    char cpubw_polling_interval_path[NODE_MAX];
    char cpubw_hwmon_decay_rate_path[NODE_MAX];
    char cpubw_hwmon_io_percent_path[NODE_MAX];
    char cpubw_hwmon_sample_ms_path[NODE_MAX];
    char cpubw_hwmon_hist_memory_path[NODE_MAX];
    char cpubw_hwmon_hyst_length_path[NODE_MAX];
    char cpubw_hwmon_hyst_count_path[NODE_MAX];
    char cpubw_hwmon_lpcm_path[NODE_MAX];
    char cpubw_hwmon_gbm_path[NODE_MAX];
    char cpubw_hwmon_up_scale_path[NODE_MAX];
    char cpubw_hwmon_lpip_path[NODE_MAX];
    char cpubw_hwmon_idle_mbps_path[NODE_MAX];
    unsigned int  cpubw_avail_freqs[MAX_FREQ_CPUBW];
    int  cpubw_avail_freqs_n;
    char cpubw_mf_s[NODE_MAX];             // cpubw min freq storage
    int  cpubw_mf_sl;                      // cpubw min freq string length
    char cpubw_maxf_s[NODE_MAX];           // cpubw max freq storage
    int  cpubw_maxf_sl;                    // cpubw max freq string length
    char cpubw_poll_int_s[NODE_MAX];       // cpubw poll interval storage
    int  cpubw_poll_int_sl;                // cpubw poll interval string length
    char cpubw_dr_s[NODE_MAX];             // cpubw decay rate storage
    int  cpubw_dr_sl;                      // cpubw decay rate string length
    unsigned int cpubw_stored;
    char cpubw_ip_s[NODE_MAX];             // cpubw io percent storage
    int  cpubw_ip_sl;                      // cpubw io percent string length
    char cpubw_sm_s[NODE_MAX];             // cpubw sample ms storage
    int  cpubw_sm_sl;                      // cpubw sample ms string length
    char cpubw_hm_s[NODE_MAX];             // cpubw hist memory storage
    int  cpubw_hm_sl;                      // cpubw hist memory string length
    char cpubw_hl_s[NODE_MAX];             // cpubw hyst length storage
    int  cpubw_hl_sl;                      // cpubw hyst length string length
    char cpubw_hc_s[NODE_MAX];             // cpubw hyst count storage
    int  cpubw_hc_sl;                      // cpubw hyst count string length
    char cpubw_lpcm_s[NODE_MAX];           // cpubw low power ceil mbps storage
    int  cpubw_lpcm_sl;                    // cpubw low power ceil mbps string length
    char cpubw_gbm_s[NODE_MAX];            // cpubw guard band mbps storage
    int  cpubw_gbm_sl;                     // cpubw guard band mbps string length
    char cpubw_us_s[NODE_MAX];             // cpubw up scale storage
    int  cpubw_us_sl;                      // cpubw up scale string length
    char cpubw_lpip_s[NODE_MAX];           // cpubw low power io percent storage
    int  cpubw_lpip_sl;                    // cpubw low power io percent string length
    char cpubw_imbps_s[NODE_MAX];          // cpubw idle mbps storage
    int  cpubw_imbps_sl;                   // cpubw idle mbps string length


    char schedb_n[NODE_MAX];                  // sched boost node string

    int vid_hint_state;
    int slvp_perflock_set;
    int vid_enc_start;

    char ksm_run_node[NODE_MAX];
    char ksm_param_sleeptime[NODE_MAX];
    char ksm_param_pages_to_scan[NODE_MAX];
    char ksm_sleep_millisecs[PROPERTY_VALUE_MAX];
    char ksm_pages_to_scan[PROPERTY_VALUE_MAX];
    int is_ksm_supported;

    char gpu_default_pwrlvl_s[NODE_MAX];       // gpu default pwrlevel storage
    int  gpu_default_pwrlvl_sl;           // gpu default pwrlevel string length
    char gpu_min_pwrlvl_s[NODE_MAX];        // gpu min pwrlevel storage
    int  gpu_min_pwrlvl_sl;                 // gpu min pwrlevel string length
    char gpu_max_pwrlvl_s[NODE_MAX];        // gpu max pwrlevel storage
    int  gpu_max_pwrlvl_sl;                 // gpu max pwrlevel string length

    char sctm[NODE_MAX];                    // synchronize cores on foreground thread migration node

    unsigned int  c0fL[FREQLIST_MAX];                // cpu0 list of available freq
    char c0fL_s[FREQLIST_STR];              // cpu0 list of available freq
    int  c0fL_n;                            // cpu0 #freq
    int  c4fL[FREQLIST_MAX];                // cpu4 list of available freq
    char c4fL_s[FREQLIST_STR];              // cpu4 list of available freq string
    int  c4fL_n;                            // cpu4 #freq

    int kpm_hotplug_support; //1 represent hotplug is supported through KPM, 0 hotplug not supported, -1 not initialized
    int lock_max_clust0;     //store cluster0 value for lock max core perflock, through KPM
    int lock_max_clust1;     //store cluster1 value for lock max core perflock, through KPM

    char core_ctl_min_cpu_node[NODE_MAX];   //Actual path of min_cpu node based on target
    char core_ctl_max_cpu_node[NODE_MAX];   //Actual path of max_cpu node based on target

    int core_ctl_present;    //1 represent core_ctl is present, 0 core ctl not present, -1 not initialized

    char core_ctl_min_cpu[NODE_MAX];        // core control min_cpu storage
    char core_ctl_max_cpu[NODE_MAX];        // core control max_cpu storage

    int min_cores;
    int max_cores;

    /* This array will be used to store max/min frequency
     * for each cpu.
     */
    struct cpu_freq_resource_value cpu_freq_val[MAX_CPUS];

    bool min_freq_prop_0_set;
    bool min_freq_prop_4_set;
    int  min_freq_prop_0_val;
    int  min_freq_prop_4_val;

    //hint data
    hint_associated_data mHintData;

    //irq data
    int  *irq_bal_sp; // irq balancer storage pointer
    unsigned int stored_irq_balancer;
    bool is_active;

    /* sched group data */
    sched_group_data mSchedGroupData;

    /* sched freq aggr group data */
    sched_freq_aggr_group_data mSchedFreqAggrGroupData;

public:
    int init_available_cpubw_freq();
    int init_cpubw_hwmon_path();

    int vid_create_timer();

    int init_ksm();
    int toggle_ksm_run(int run);

    void parse_avail_freq_list();
    int kpm_support_for_hotplug();

    int core_ctl_init();
    int check_core_ctl_presence();

    int setup_cpu_freq_values();

    int get_reset_cpu_freq(int cpu, int ftype);
    int find_next_cpu_frequency(int cpu, int freq);
    int init_available_freq(int cpu);
    void check_min_freq_prop_set(int cpu);
    int find_frequency_pos(int cpu, int freq);
public:
    ~OptsData();

    int Init();

    static OptsData &getInstance() {
        return mOptsData;
    }

};

#endif /*__OPTS_DATA__H_*/
