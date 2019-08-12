/******************************************************************************
  @file    OptsHandler.cpp
  @brief   Implementation for handling operations

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <dirent.h>


#define LOG_TAG "ANDR-PERF-OPTSHANDLER"
#include <cutils/log.h>

#include "Request.h"

#include "MpctlUtils.h"
#include "OptsData.h"
#include "OptsHandler.h"
#include "ResourceQueues.h"
#include "Target.h"

#define ENABLE_PREFER_IDLE  1
#define WRITE_MIGRATE_COST  0

#define DISABLE_PC_LATENCY  1

//Assumes max # cpus == 32
#define CPUSET_STR_SIZE 86

#define RELEASE_LOCK     0

/*TODO Confirm the usage of these node*/
static char c00[NODE_MAX] = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq";                     // cpu0 min scaling freq node
static char c01[NODE_MAX] = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq";                     // cpu0 max scaling freq node
static char c10[NODE_MAX] = "/sys/devices/system/cpu/cpu1/cpufreq/scaling_min_freq";                     // cpu1 min scaling freq node
static char c11[NODE_MAX] = "/sys/devices/system/cpu/cpu1/cpufreq/scaling_max_freq";                     // cpu1 max scaling freq node
static char c20[NODE_MAX] = "/sys/devices/system/cpu/cpu2/cpufreq/scaling_min_freq";                     // cpu2 min scaling freq node
static char c21[NODE_MAX] = "/sys/devices/system/cpu/cpu2/cpufreq/scaling_max_freq";                     // cpu2 max scaling freq node
static char c30[NODE_MAX] = "/sys/devices/system/cpu/cpu3/cpufreq/scaling_min_freq";                     // cpu3 min scaling freq node
static char c31[NODE_MAX] = "/sys/devices/system/cpu/cpu3/cpufreq/scaling_max_freq";                     // cpu3 max scaling freq node
static char c40[NODE_MAX] = "/sys/devices/system/cpu/cpu4/cpufreq/scaling_min_freq";                     // cpu4 min scaling freq node
static char c41[NODE_MAX] = "/sys/devices/system/cpu/cpu4/cpufreq/scaling_max_freq";                     // cpu4 max scaling freq node
static char c50[NODE_MAX] = "/sys/devices/system/cpu/cpu5/cpufreq/scaling_min_freq";                     // cpu5 min scaling freq node
static char c51[NODE_MAX] = "/sys/devices/system/cpu/cpu5/cpufreq/scaling_max_freq";                     // cpu5 max scaling freq node
static char c60[NODE_MAX] = "/sys/devices/system/cpu/cpu6/cpufreq/scaling_min_freq";                     // cpu6 min scaling freq node
static char c61[NODE_MAX] = "/sys/devices/system/cpu/cpu6/cpufreq/scaling_max_freq";                     // cpu6 max scaling freq node
static char c70[NODE_MAX] = "/sys/devices/system/cpu/cpu7/cpufreq/scaling_min_freq";                     // cpu7 min scaling freq node
static char c71[NODE_MAX] = "/sys/devices/system/cpu/cpu7/cpufreq/scaling_max_freq";                     // cpu7 max scaling freq node
static char c0o[NODE_MAX] = "/sys/devices/system/cpu/cpu0/online";  // cpu0 online node
static char c1o[NODE_MAX] = "/sys/devices/system/cpu/cpu1/online";  // cpu1 online node
static char c2o[NODE_MAX] = "/sys/devices/system/cpu/cpu2/online";;  // cpu2 online node
static char c3o[NODE_MAX] = "/sys/devices/system/cpu/cpu3/online";;  // cpu3 online node
static char c4o[NODE_MAX] = "/sys/devices/system/cpu/cpu4/online";;  // cpu4 online node
static char c5o[NODE_MAX] = "/sys/devices/system/cpu/cpu5/online";;  // cpu5 online node
static char c6o[NODE_MAX] = "/sys/devices/system/cpu/cpu6/online";;  // cpu6 online node
static char c7o[NODE_MAX] = "/sys/devices/system/cpu/cpu7/online";;  // cpu7 online node

static pthread_t id_ka_thread;
static bool tKillFlag = true;
static bool isThreadAlive = false;
pthread_mutex_t ka_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifndef PERFD
extern int mpdecision_toggle_power_collapse(int);
extern int mpdecision_lock_min_cores(int);
extern int mpdecision_lock_max_cores(int);
#endif


OptsHandler OptsHandler::mOptsHandler;

/* This array tells us information about each CPU such as:
 *     1. what is the sysfs for min and max freq
 *     2. is there a mapping to it
 */
struct cpu_freq_mapping cpu_freq_map[] = {
    {c00, c01, c0o, 0},
    {c10, c11, c1o, 0},
    {c20, c21, c2o, 0},
    {c30, c31, c3o, 0},
    {c40, c41, c4o, 0},
    {c50, c51, c5o, 0},
    {c60, c61, c6o, 0},
    {c70, c71, c7o, 0},
};

//opts handler
OptsHandler::OptsHandler():
    /* Set up function pointers for resources */
    mOptsTable {
        {dummy, dummy, higher_is_better},

        {toggle_power_collapse, toggle_power_collapse, higher_is_better},

        /*cpu freq*/
        {cpu_options, cpu_options, higher_is_better},
        {cpu_options, cpu_options, higher_is_better},

        /*sched group*/
        {sched_boost, sched_boost, higher_is_better},
        {sched_prefer_idle, sched_prefer_idle, higher_is_better},
        {sched_migrate_cost, sched_migrate_cost, higher_is_better},
        {sched_small_task, sched_small_task, higher_is_better},
        {sched_mostly_idle_load, sched_mostly_idle_load, higher_is_better},
        {sched_mostly_idle_nr_run, sched_mostly_idle_nr_run, higher_is_better},
        {sched_init_task_load, sched_init_task_load, higher_is_better},
        {sched_upmigrate, sched_upmigrate, lower_is_better},
        {sched_downmigrate, sched_downmigrate, lower_is_better},
        {sched_mostly_idle_freq, sched_mostly_idle_freq, higher_is_better},
        {add_sched_group, reset_sched_group, always_apply},
        {sched_spill_nr_run, sched_spill_nr_run, higher_is_better},
        {sched_static_cpu_pwr_cost, sched_static_cpu_pwr_cost, lower_is_better},
        {sched_restrict_cluster_spill, sched_restrict_cluster_spill, lower_is_better},
        {sched_add_freq_aggr_group, sched_reset_freq_aggr_group, always_apply},
        {sched_update_top_app_cpuset, sched_restore_top_app_cpuset, always_apply},
        {sched_update_fg_cpuset, sched_restore_fg_cpuset, always_apply},
        {sched_update_sys_bg_cpuset, sched_restore_sys_bg_cpuset, always_apply},
        {sched_update_bg_cpuset, sched_restore_bg_cpuset, always_apply},
        {sched_set_freq_aggr, sched_set_freq_aggr, higher_is_better},
        {sched_set_enable_thread_grouping, sched_set_enable_thread_grouping, higher_is_better},

        /*hotplug*/
        {lock_min_cores, lock_min_cores, higher_is_better},
        {lock_max_cores, lock_max_cores, lower_is_better},

        /*interactive*/
        {interactive_above_hispeed_delay, interactive_above_hispeed_delay, higher_is_better},
        {interactive_boost, interactive_boost, higher_is_better},
        {interactive_boostpulse, interactive_boostpulse, higher_is_better},
        {interactive_boostpulse_duration, interactive_boostpulse_duration, higher_is_better},
        {interactive_go_hispeed_load, interactive_go_hispeed_load, higher_is_better},
        {interactive_hispeed_freq, interactive_hispeed_freq, higher_is_better},
        {interactive_io_is_busy, interactive_io_is_busy, higher_is_better},
        {interactive_min_sample_time, interactive_min_sample_time, higher_is_better},
        {interactive_target_loads, interactive_target_loads, higher_is_better},
        {interactive_timer_rate, interactive_timer_rate, lower_is_better},
        {interactive_timer_slack, interactive_timer_slack, lower_is_better},
        {interactive_max_freq_hysteresis, interactive_max_freq_hysteresis, lower_is_better},
        {interactive_use_sched_load, interactive_use_sched_load, higher_is_better},
        {interactive_use_migration_notif, interactive_use_migration_notif, higher_is_better},
        {interactive_ignore_hispeed_notif, interactive_ignore_hispeed_notif, higher_is_better},

        /*cpubw hwmon*/
        {cpubw_hwmon_min_freq, cpubw_hwmon_min_freq, higher_is_better},
        {cpubw_hwmon_decay_rate, cpubw_hwmon_decay_rate, higher_is_better},
        {cpubw_hwmon_io_percent, cpubw_hwmon_io_percent, higher_is_better},
        {cpubw_hwmon_hyst_opt, cpubw_hwmon_hyst_opt, higher_is_better},
        {cpubw_hwmon_low_power_ceil_mbps, cpubw_hwmon_low_power_ceil_mbps, higher_is_better},
        {cpubw_hwmon_low_power_io_percent, cpubw_hwmon_low_power_io_percent, higher_is_better},
        {cpubw_hwmon_max_freq, cpubw_hwmon_max_freq, higher_is_better},
        {cpubw_hwmon_polling_interval, cpubw_hwmon_polling_interval, higher_is_better},
        {cpubw_hwmon_sample_ms, cpubw_hwmon_sample_ms, higher_is_better},
        {cpubw_hwmon_idle_mbps, cpubw_hwmon_idle_mbps, higher_is_better},

        /*video*/
        {handle_vid_encplay_hint, handle_vid_encplay_hint, higher_is_better},
        {handle_vid_decplay_hint, handle_vid_decplay_hint, higher_is_better},
        {handle_disp_hint, handle_disp_hint, higher_is_better},

        /*ksm*/
        {disable_ksm, enable_ksm, higher_is_better},
        {set_ksm_param, reset_ksm_param, higher_is_better},

        /*ondemand*/
        {sampling_rate, sampling_rate, lower_is_better},
        {ondemand_io_is_busy, ondemand_io_is_busy, higher_is_better},
        {ondemand_sampling_down_factor, ondemand_sampling_down_factor, higher_is_better},
        {sync_freq, sync_freq, higher_is_better},
        {optimal_freq, optimal_freq, higher_is_better},
        {toggle_step_up, toggle_step_up, higher_is_better},
        {write_max_intr_steps, write_max_intr_steps, higher_is_better},
        {toggle_thread_migration_sync, toggle_thread_migration_sync, higher_is_better},

        /*gpu*/
        {gpu_default_pwrlvl, gpu_default_pwrlvl, higher_is_better},
        {gpu_min_pwrlvl, gpu_min_pwrlvl, lower_is_better},
        {gpu_max_pwrlvl, gpu_max_pwrlvl, higher_is_better},

        /*miscellaneous*/
        {unsupported, unsupported, higher_is_better},
        {irq_balancer, irq_balancer, higher_is_better},
        {input_boost_reset, input_boost_reset, higher_is_better},
        {swap_ratio, swap_ratio, higher_is_better},
        {storage_clk_scaling_disable, storage_clk_scaling_disable, always_apply},
        {keep_alive, dummy, higher_is_better},
        /*PM QoS locks*/
        {pmQoS_cpu_dma_latency, pmQoS_cpu_dma_latency, lower_is_better}
    }
{
}

OptsHandler::~OptsHandler() {
}

int OptsHandler::Init() {
    return 0;
}

int OptsHandler::ApplyOpt(Resource &resObj) {
    int ret = FAILED;
    int idx = resObj.qindex;
    OptsData &dataObj = OptsData::getInstance();
    if (idx >= 0 && idx < MAX_MINOR_RESOURCES) {
        ret = mOptsTable[idx].mApplyOpts(resObj, dataObj);
    } else {
        QLOGE("Failed to call apply optimization for 0x%x", resObj.qindex);
    }
    return ret;
}

int OptsHandler::ResetOpt(Resource &resObj) {
    int ret = FAILED;
    int idx = resObj.qindex;
    OptsData &dataObj = OptsData::getInstance();
    if (idx >= 0 && idx < MAX_MINOR_RESOURCES) {
        ret = mOptsTable[idx].mResetOpts(resObj, dataObj);
    } else {
        QLOGE("Failed to call reset optimization for 0x%x", idx);
    }
    return ret;
}

int OptsHandler::CompareOpt(int qindx, unsigned int reqVal, unsigned int curVal) {
    int ret = ADD_NEW_REQUEST;

    if (curVal != 0) {
        //First resource id present
        if (qindx >=0 && qindx < MAX_MINOR_RESOURCES) {
            ret = mOptsTable[qindx].mCompareOpts(reqVal, curVal);
        } else {
            QLOGE("Cannot find a compareOpt function");
            return FAILED;
        }
    }
    return ret;
}

int OptsHandler::ValidateClusterAndCore(int cluster, int core, int resourceStatus) {
    Target &t = Target::getCurTarget();
    int supportedCore = -1, core_no = -1;

    //First check cluster and core are in valid range
    if ((cluster < 0) || (cluster > t.getNumCluster())) {
        QLOGE("Invalid cluster no. %d", cluster);
        return FAILED;
    }
    if ((core < 0) || (core > t.getLastCoreIndex(cluster))) {
        QLOGE("Invalid core no. %d", core);
        return FAILED;
    }

    //Second check resource is supported on that core or not
    core_no = t.getLastCoreIndex(0)+1;
    switch (resourceStatus) {
        case SYNC_CORE: //Resource is sync core based, only first core of cluster is supported
            if ((core != 0) && (core != core_no)) {
                QLOGE("Core %d is not supported for this perflock resource, instead use core 0/%d", core, core_no);
                return FAILED;
            }
            break;
       case ASYNC_CORE: //Resource is async core based, all core accepted
            QLOGI("Core %d is supported for this perflock resource", core);
            break;
       case CORE_INDEPENDENT: //Resource does not depend on core, only first core of perf cluster is accepted
            supportedCore = t.getFirstCoreOfPerfCluster();
            if ((supportedCore == -1) || (core != supportedCore)) {
                QLOGE("Core %d is not supported for this perflock resource, instead use core %d", core, supportedCore);
                return FAILED;
            }
            break;
       default:
            QLOGE("Invalid resource based core status");
            return FAILED;
    }
    return SUCCESS;
}

//support functions
//Update core based parameter
int OptsHandler::update_core_based_param(int iscorebased,
                                         const char node[NODE_MAX],
                                         const char node_core[NODE_MAX],
                                         const char node_strg[NODE_MAX],
                                         int node_strg_l,
                                         const char *str) {
    int rc = -1;
    int i;
    char tmp_node[NODE_MAX];

    if (!iscorebased) {
         FWRITE_STR(node, node_strg, node_strg_l, rc);
         QLOGI("%s: updated %s with %s return value %d", str, node, node_strg, rc);
    } else {
         for (i = 0; i < Target::getCurTarget().getTotalNumCores(); i++) {
              snprintf(tmp_node, NODE_MAX, node_core, i);
              FWRITE_STR(tmp_node, node_strg, node_strg_l, rc);
              QLOGI("%s: updated %s with %s return value %d", str, tmp_node, node_strg, rc);
         }
    }
    return rc;
}

/* Wrapper functions for function pointers */
int OptsHandler::dummy(Resource &r,  OptsData &d) {
    return 0;
}

/* Unsupported resource opcode*/
int OptsHandler::unsupported(Resource &r, OptsData &d) {
    QLOGE("Error: This resource is not supported");
    return FAILED;
}

int OptsHandler::pmQoS_cpu_dma_latency(Resource &r, OptsData &d) {
    int rc = FAILED;
    static char pmqos_cpu_dma_s[NODE_MAX];// pm qos cpu dma latency string
    unsigned int latency = r.value;
    static int fd = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("%s:Request on invalid core or cluster", __func__);
        return FAILED;
    }

    /*If there is a lock acquired, then we close, to release
     *that fd and open with new value.Doing this way
     * in this case actually handles concurrency.
     */
    if (fd >= 0) { //close the device node that was opened.
        rc = close(fd);
        fd = FAILED;
        QLOGI("%s:Released the PMQos Lock and dev node closed, rc=%d", __func__, rc);
    }

    /*Check if Perf lock request to acquire(!= MAX_LVL)*/
    if (latency != MAX_LVL) {
        //FWRITE_STR not used since it closes the fd. This shouldnt be done in this case
        ///kernel/msm-4.4/Documentation/power/pm_qos_interface.txt#95
        fd = open(PM_QOS_CPU_DMA_DEV, O_WRONLY);
        if (fd >= 0) {
           snprintf(pmqos_cpu_dma_s, NODE_MAX, "%x", latency);
           rc = write(fd, pmqos_cpu_dma_s, strlen(pmqos_cpu_dma_s));// Write the CPU DMA Latency value
           if (rc < 0) {
               QLOGE("%s:Writing to the PM QoS node failed=%d", __func__, rc);
               close(fd);
               fd = FAILED;
           } else {
               //To remove the user mode request for a target value simply close the device node
               QLOGI("%s:Sucessfully applied PMQos Lock for fd=%d, latency=0x%s, rc=%d", __func__, fd, pmqos_cpu_dma_s, rc);
           }
       } else {
           rc = FAILED;
           QLOGE("%s:Failed to Open PMQos Lock for fd=%d, latency=0x%s, rc=%d", __func__, fd, pmqos_cpu_dma_s, rc);
       }
    }
    return rc;
}
/* Enable/disable power collapse on all CPUS */
int OptsHandler::toggle_power_collapse(Resource &r,  OptsData &d) {
    unsigned int reqval = r.value;
    char tmp_s[NODE_MAX];
    int rc = FAILED;
    static unsigned int stored_pwr_clps = 0;
    static char tggl_pwr_clps[NODE_MAX]; // toggle power collapse storage
    static int  tggl_pwr_clpsl = -1; // toggle power collapse string length

#ifdef PERFD
    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (tggl_pwr_clpsl > 0) {
            FWRITE_STR(PM_QOS, tggl_pwr_clps, tggl_pwr_clpsl, rc);
            stored_pwr_clps = 0;
            QLOGI("perf_lock_rel: writing %s with %s and rc is %d", PM_QOS, tggl_pwr_clps, rc);
            if (rc < 0) {
                QLOGE("Releasing the power collapse perflock failed\n");
            }
        }
        return rc;
    }

    if (!stored_pwr_clps) {
        FREAD_STR(PM_QOS, tggl_pwr_clps, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", PM_QOS, tggl_pwr_clps, rc);
            tggl_pwr_clpsl = strlen(tggl_pwr_clps);
            stored_pwr_clps = 1;
        } else {
            //better do not acquire lock as we can not release
            QLOGE("Failed to read %s ", PM_QOS);
            return FAILED;
        }
    }

    reqval = !!reqval; /* 0 or 1 */

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(PM_QOS, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: writing %s with %s and rc is %d ", PM_QOS, tmp_s, rc);
    if (rc < 0) {
        QLOGE("Acquiring the power collapse perflock failed\n");
    }
    return rc;
#else
    return mpdecision_toggle_power_collapse(reqval);
#endif
}


/* CPU options */
int OptsHandler::cpu_options(Resource &r,  OptsData &d) {
    int rc = -1;
    int i = 0;
    char fmtStr[NODE_MAX];
    unsigned int reqval  = r.value;
    int valToUpdate;
    char nodeToUpdate[NODE_MAX];
    int min_freq = (CPUFREQ_MIN_FREQ_OPCODE == r.minor) ? 1 : 0;
    int cpu = -1;
    static bool is_sync_cores; //Updating a single core freq, updates all cores freq.
    Target &t = Target::getCurTarget();
    int startCpu, endCpu, cluster, coresInCluster;

    is_sync_cores = t.isSyncCore();
    cpu = r.core;

    /* For sync_cores, where changing frequency for one core changes, ignore the requests
     * for resources other than for core0 and core4. This is to ensure that we may not end
     * up loosing concurrency support for cpufreq perflocks.
     */
#ifdef PERFD
    cluster = t.getClusterForCpu(cpu, startCpu, endCpu);
    if ((startCpu < 0) || (endCpu < 0) || (cluster < 0)) {
        QLOGE("Could not find a cluster corresponding the core %d", cpu);
        return FAILED;
    }
    if (is_sync_cores && (cpu > startCpu && cpu <= endCpu)) {
        QLOGW("Warning: Resource [%d, %d] not supported for core %d. Instead use resource for core %d", r.major, r.minor,cpu,startCpu);
        return FAILED;
    }
    /* Calculate the value that needs to be updated to KPM.
     * If lock is being released (reqVal == 0) then
     * update with 0 (for min_freq) UNIT_MAX (for max_freq)
     * If lock is being acquired then reqVal is multiplied
     * with 100000 and updated by finding next closest frequency.
     * */
     if (reqval == MAX_LVL) {
         if (min_freq){
             QLOGI("Releasing min frequency perf lock");
             valToUpdate = 0;
             strlcpy(nodeToUpdate, KPM_CPU_MIN_FREQ_NODE, strlen(KPM_CPU_MIN_FREQ_NODE)+1);
         } else {
             QLOGI("Releasing max frequency perf lock");
             valToUpdate = UINT_MAX;
             strlcpy(nodeToUpdate, KPM_CPU_MAX_FREQ_NODE, strlen(KPM_CPU_MAX_FREQ_NODE)+1);
         }
     } else {
         reqval *= FREQ_MULTIPLICATION_FACTOR;
         valToUpdate = d.find_next_cpu_frequency(r.core, reqval);
         if (min_freq) {
             strlcpy(nodeToUpdate, KPM_CPU_MIN_FREQ_NODE, strlen(KPM_CPU_MIN_FREQ_NODE)+1);
         } else {
             strlcpy(nodeToUpdate, KPM_CPU_MAX_FREQ_NODE, strlen(KPM_CPU_MAX_FREQ_NODE)+1);
         }
     }
    QLOGI("Freq value to be updated %d", valToUpdate);

    /* Construct the formatted string with which KPM node is updated
     */
    coresInCluster = t.getCoresInCluster(cluster);
    if (coresInCluster < 0){
        QLOGE("Cores in a cluster cannot be %d", coresInCluster);
        return FAILED;
    }

    if (cluster == 0) {
        rc = update_freq_node(0, t.getLastCoreIndex(0), valToUpdate, fmtStr, NODE_MAX);
        if (rc == FAILED) {
            QLOGE("Perflock failed, frequency format string is not proper");
            return FAILED;
        }
    } else if (cluster == 1) {
        rc = update_freq_node(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1), valToUpdate, fmtStr, NODE_MAX);
        if (rc == FAILED) {
            QLOGE("Peflock failed, frequency format string is not proper");
            return FAILED;
        }
    } else {
        QLOGE("Cluster Id %d not supported\n",cluster);
        return FAILED;
    }

    /* Finally update the KPM Node. The cluster based node is updated in
     * fmtStr, and KPM node is available in nodeToUpdate.
     * */
     FWRITE_STR(nodeToUpdate, fmtStr, strlen(fmtStr), rc);
     QLOGI("Updated %s with %s return value %d",nodeToUpdate , fmtStr, rc );
     return rc;

#endif

#if 0

    /* This is the gap value for each CPU between its scaling_min and scaling_max resource
     * This will be used to compute the resource id for scaling_max when given
     * resource for scaling_min and vice versa
     */
    const int min_max_gap = 19;
    int assoc_min_freq = 0;
    int assoc_max_freq = 0;
    cpu = r.core;

#ifdef PERFD
//    int is_clustr_0 = (0 == r.cluster) ? 1 : 0;
    int is_clustr_0 = ((cpu >= 0) && (cpu <= t.getLastCoreIndex(0))) ? 1 : 0;

    /* For sync_cores, where changing frequency for one core changes, ignore the requests
     * for resources other than for core0 and core4. This is to ensure that we may not end
     * up loosing concurrency support for cpufreq perflocks.
     */
    if (is_sync_cores && (r.core >= CPU1 && r.core <= CPU3)){
       QLOGW("Warning: Resource [%d, %d] not supported. Instead use resource for core0", r.major, r.minor);
       return FAILED;
    }
    if (is_sync_cores && (r.core >= CPU5 && r.core <= CPU7)){
       QLOGW("Warning: Resource [%d, %d] not supported. Instead use resource for core4", r.major, r.minor);
       return FAILED;
    }

    if (!reqval) {
        if (r.cluster) {
            if (min_freq) {
                snprintf(tmp_s, NODE_MAX, "0:%d 1:%d 2:%d 3:%d", reqval, reqval, reqval, reqval);
                QLOGI("reset min freq req for CPUs 0-3: %s", tmp_s);
                FWRITE_STR(KPM_CPU_MIN_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
            } else {
                snprintf(tmp_s, NODE_MAX, "0:%u 1:%u 2:%u 3:%u", UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX);
                QLOGI("reset max freq req for CPUs 0-3: %s", tmp_s);
                FWRITE_STR(KPM_CPU_MAX_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
            }
        } else {
            if (min_freq) {
                snprintf(tmp_s, NODE_MAX, "4:%d 5:%d 6:%d 7:%d", reqval, reqval, reqval, reqval);
                QLOGI("reset min freq req for CPUs 4-7: %s", tmp_s);
                FWRITE_STR(KPM_CPU_MIN_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
            } else {
                snprintf(tmp_s, NODE_MAX, "4:%u 5:%u 6:%u 7:%u", UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX);
                QLOGI("reset max freq req for CPUs 4-7: %s", tmp_s);
                FWRITE_STR(KPM_CPU_MAX_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
            }
        }
        return rc;
    }

    reqval *= 100000;
    reqval = find_next_cpu_frequency(cpu, reqval);
    if (reqval == FAILED)
       return FAILED;

    if (is_clustr_0) {
        snprintf(tmp_s, NODE_MAX, "0:%d 1:%d 2:%d 3:%d", reqval, reqval, reqval, reqval);
    } else {
        snprintf(tmp_s, NODE_MAX, "4:%d 5:%d 6:%d 7:%d", reqval, reqval, reqval, reqval);
    }

    if (min_freq) {
        QLOGI("set min freq req: %s", tmp_s);
        FWRITE_STR(KPM_CPU_MIN_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
    } else {
        QLOGI("set max freq req: %s", tmp_s);
        FWRITE_STR(KPM_CPU_MAX_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
    }

    return rc;
#else
    if (min_freq) {
        int invalid_min_freq = ((r.core < CPU0) || (r.core > CPU3)) ? 1 : 0;
        assoc_max_freq = reqtype+min_max_gap;
        int invalid_assoc_max_freq = ((assoc_max_freq < CPU0_MAX_FREQ) || (assoc_max_freq > CPU3_MAX_FREQ)) ? 1 : 0;
        if (invalid_min_freq || invalid_assoc_max_freq) {
            QLOGE("Invalid inputs for cpu min options");
            return FAILED;
        }
    } else {
        int invalid_max_freq = ((reqtype < CPU0_MAX_FREQ) || (reqtype > CPU3_MAX_FREQ)) ? 1 : 0;
        assoc_min_freq = reqtype-min_max_gap;
        int invalid_assoc_min_freq = ((assoc_min_freq < CPU0_MIN_FREQ) || (assoc_min_freq > CPU3_MIN_FREQ)) ? 1 : 0;
        if (invalid_max_freq || invalid_assoc_min_freq) {
            QLOGE("Invalid inputs for cpu max options");
            return FAILED;
        }
    }
    /* If the CPU requested is CPU0 we will immediately assign the proper sysfs
     * node to use based on whether its request for scaling_min or scaling_max
     * If not CPU 0 then we'll check to see if the designated
     * CPU for that resource is online. If it is not then we'll have to
     * cycle through the remaining available CPUs and check if its online.
     * Once we find an available CPU then we'll map the cpu to the resource
     * identifier and mark it as in_use.
     */
    if ((r.minor == CPUFREQ_MIN_FREQ_OPCODE) && (r.core == CPU0)) {
        cpu_node = c00;
    } else if ((r.minor == CPUFREQ_MAX_FREQ_OPCODE) && (r.core == CPU0)) {
        cpu_node = c01;
    } else {
        /* get the cpu to use for this resource */
        cpu = cpu_freq_state[reqtype].mapped_cpu;

        if (reqval) {
            /* we are acquiring the lock for CPU freq */

            if (!cpu) {
                /* there is no mapped cpu for the requested resource */

                if (1 > get_online_cpus(core_status, sizeof(core_status)/sizeof(core_status[0]))) {
                    QLOGE("Invalid number of cpus are online for frequency request");
                    return FAILED;
                }

                /* search for an available CPU */
                for (resource_id = CPU1_MIN_FREQ; resource_id < (CPU3_MIN_FREQ+1); resource_id++) {

                    cpu = cpu_freq_state[resource_id].ideal_cpu_id;

                    if (1 == get_core_status(cpu) && !cpu_freq_map[cpu].in_use) {
                        /* found a CPU, so use it */

                        cpu_freq_map[cpu].in_use = 1;
                        cpu_freq_state[reqtype].mapped_cpu = cpu;

                        if (min_freq) {
                            cpu_node = cpu_freq_map[cpu].min;
                        } else {
                            cpu_node = cpu_freq_map[cpu].max;
                        }
                        break;
                    }
                }

                if (cpu_node == NULL) {
                    QLOGE("Unable to find an available cpu");
                    return FAILED;
                }

            } else {
                /* there is already a mapped cpu for this request, so use it */
                if (min_freq) {
                    cpu_node = cpu_freq_map[cpu].min;
                } else {
                    cpu_node = cpu_freq_map[cpu].max;
                }
            }
        } else {
            /* we are releasing the lock on the CPU */

            if (!cpu) {
                /* there is no mapped resource for this release request */
                QLOGE("Unable to find the cpu assigned for this resource");
                return FAILED;
            }

            //todo: resource_qs
            #if 0
            /* if there are no more requests for the CPU in use, unmap to release the CPU */
            if (min_freq) {
                cpu_node = cpu_freq_map[cpu].min;
                if (!resource_qs[assoc_max_freq].level) {
                    cpu_freq_state[reqtype].mapped_cpu = 0;
                    cpu_freq_state[assoc_max_freq].mapped_cpu = 0;
                    cpu_freq_map[cpu].in_use = 0;
                }
            } else {
                cpu_node = cpu_freq_map[cpu].max;
                if (!resource_qs[assoc_min_freq].level) {
                    cpu_freq_state[reqtype].mapped_cpu = 0;
                    cpu_freq_state[assoc_min_freq].mapped_cpu = 0;
                    cpu_freq_map[cpu].in_use = 0;
                }
            }
            #endif
        }
    }
#endif
    if (!reqval) {
        if (min_freq) {
           freqtoupdate = get_reset_cpu_freq(cpu, MIN_FREQ_REQ);
           if (freqtoupdate == FAILED)
              return FAILED;
        } else {
           freqtoupdate = get_reset_cpu_freq(cpu, MAX_FREQ_REQ);
           if (freqtoupdate == FAILED)
               return FAILED;
        }
        snprintf(tmp_s, NODE_MAX, "%d", freqtoupdate);
        FWRITE_STR(cpu_node, tmp_s, strlen(tmp_s), rc);

        /* Core is offline while releasing perf-lock. Bring it online first */
        if ( rc <= 0 )  {
           const char* cpu_online_node = cpu_freq_map[cpu].online;

           QLOGW("CPU %d is offline. Bring it online to reset to default min_freq", cpu);

           /* Disable KTM and bring core to online */
           FWRITE_STR(KTM_ENABLE_NODE, "0", 1, rc);
           FWRITE_STR(cpu_online_node, "1", 1, rc);

           /* Now, set the default min_freq */
           FWRITE_STR(cpu_node, tmp_s, strlen(tmp_s), rc);

           if ( rc <= 0 ) {
              QLOGW("Still, Unable to set default min_freq for CPU %d", cpu);
           }

           /* Shutdown core and enable KTM back */
           FWRITE_STR(cpu_online_node, "0", 1, rc);
           FWRITE_STR(KTM_ENABLE_NODE, "1", 1, rc);

           if ( rc <= 0 ) {
              QLOGW("Unable to enable KTM core-control !");
           }
        }
        return rc;
    }

    reqval *= 100000;
    reqval =  find_next_cpu_frequency(cpu, reqval);
    if (reqval == FAILED)
       return FAILED;
    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(cpu_node, tmp_s, strlen(tmp_s), rc);
#endif
    return rc;
}

/* Set sched_prefer_idle.
   In current implementation we are considering
   all cores should have same value. If this
   assumption changes, code needs to be updated */
int OptsHandler::sched_prefer_idle(Resource &r,  OptsData &d) {
    int rc = 0;
    static int ispercore = -1;
    char tmp_s[NODE_MAX], prefidlenode[NODE_MAX];
    static unsigned int stored_sched_prefer_idle = 0;
    static char schedpi[NODE_MAX];                 // sched_prefer_idle node storage
    static int schedpil;                           // sched_prefer_idle node storage length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (ispercore == -1) {
        FREAD_STR(SCHED_PREFER_IDLE_NODE, tmp_s, NODE_MAX, rc);
        if (rc > 0) {
            ispercore = 0;
        } else {
            snprintf(prefidlenode, NODE_MAX, SCHED_PREFER_IDLE_NODE_CORE, 0);
            FREAD_STR(prefidlenode, tmp_s, NODE_MAX, rc);
            if (rc > 0) {
                ispercore = 1;
            } else {
                QLOGE("Error: sched_prefer_idle node is not present");
                return FAILED;
            }
        }
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (schedpil > 0) {
           rc = update_core_based_param(ispercore, SCHED_PREFER_IDLE_NODE, SCHED_PREFER_IDLE_NODE_CORE, \
                                        schedpi, schedpil, "perf_lock_rel");
           stored_sched_prefer_idle = 0;
        }
        return rc;
    }

    if (!stored_sched_prefer_idle) {
        if (!ispercore) {
            FREAD_STR(SCHED_PREFER_IDLE_NODE, schedpi, NODE_MAX, rc);
            if (rc > 0) {
                QLOGI("%s read with %s return value %d", SCHED_PREFER_IDLE_NODE, schedpi, rc);
                schedpil = strlen(schedpi);
                stored_sched_prefer_idle = 1;
            } else {
                QLOGE("Failed to read %s",SCHED_PREFER_IDLE_NODE);
                return FAILED;
            }
        } else {
            snprintf(prefidlenode, NODE_MAX, SCHED_PREFER_IDLE_NODE_CORE, 0);
            FREAD_STR(prefidlenode, schedpi, NODE_MAX, rc);
            if (rc > 0) {
                QLOGI("%s read with %s return value %d", prefidlenode, schedpi, rc);
                schedpil = strlen(schedpi);
                stored_sched_prefer_idle = 1;
            } else {
                QLOGE("Failed to read %s",prefidlenode);
                return FAILED;
            }
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", ENABLE_PREFER_IDLE);
    return update_core_based_param(ispercore, SCHED_PREFER_IDLE_NODE, SCHED_PREFER_IDLE_NODE_CORE, \
                                   tmp_s, strlen(tmp_s), "perf_lock_acq");
}

int OptsHandler::sched_migrate_cost(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_migrate_cost = 0;
    static char schedmc[NODE_MAX];                 //sched_migrate_cost node storage
    static int schedmcl;                           // shced_migrate_cost node storage length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (schedmcl > 0) {
            FWRITE_STR(SCHED_MIGRATE_COST_NODE, schedmc, schedmcl, rc);
            stored_sched_migrate_cost = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_MIGRATE_COST_NODE, schedmc, rc);
        }
        return rc;
    }

    if (!stored_sched_migrate_cost) {
        FREAD_STR(SCHED_MIGRATE_COST_NODE, schedmc, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_MIGRATE_COST_NODE, schedmc, rc);
            schedmcl = strlen(schedmc);
            stored_sched_migrate_cost = 1;
        }
        else {
             QLOGE("Failed to read %s ", SCHED_MIGRATE_COST_NODE);
             return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", WRITE_MIGRATE_COST);
    FWRITE_STR(SCHED_MIGRATE_COST_NODE, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", SCHED_MIGRATE_COST_NODE, tmp_s, rc);
    return rc;
}

/* Set sched_small_task */
int OptsHandler::sched_small_task(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_small_task = 0;
    static char sch_sml_tsk_s[NODE_MAX]; // sched small task storage
    static int  sch_sml_tsk_sl = -1; // sched small task string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_sml_tsk_sl > 0) {
            FWRITE_STR(SCHED_SMALL_TASK, sch_sml_tsk_s, sch_sml_tsk_sl, rc);
            stored_sched_small_task = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_SMALL_TASK, sch_sml_tsk_s, rc);
        }
        return rc;
    }

    if (!stored_sched_small_task) {
        FREAD_STR(SCHED_SMALL_TASK, sch_sml_tsk_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_SMALL_TASK, sch_sml_tsk_s, rc);
            sch_sml_tsk_sl = strlen(sch_sml_tsk_s);
            stored_sched_small_task = 1;
        } else {
            //better do not acquire lock as we can not release
            QLOGE("Failed to read %s ", SCHED_SMALL_TASK);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_SMALL_TASK, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", SCHED_SMALL_TASK, tmp_s, rc);
    return rc;
}

/* Set sched_mostly_idle_load.
   In current implementation we are considering
   all cores should have same value. If this
   assumption changes, code needs to be updated */
int OptsHandler::sched_mostly_idle_load(Resource &r,  OptsData &d) {
    int rc = 0;
    static int ispercore = -1;
    char tmp_s[NODE_MAX], idleloadnode[NODE_MAX];
    static unsigned int stored_sched_mostly_idle_load = 0;
    static char sch_mst_idl_ld_s[NODE_MAX];           // sched mostly idle load storage
    static int  sch_mst_idl_ld_sl = -1;               // sched mostly idle load string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (ispercore == -1) {
        FREAD_STR(SCHED_MOSTLY_IDLE_LOAD, tmp_s, NODE_MAX, rc);
        if (rc > 0) {
            ispercore = 0;
        } else {
            snprintf(idleloadnode, NODE_MAX, SCHED_MOSTLY_IDLE_LOAD_CORE, 0);
            FREAD_STR(idleloadnode, tmp_s, NODE_MAX, rc);
            if (rc > 0) {
                ispercore = 1;
            } else {
                QLOGE("Error: sched_mostly_idle_load node is not present");
                return FAILED;
            }
        }
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_mst_idl_ld_sl > 0) {
           rc = update_core_based_param(ispercore, SCHED_MOSTLY_IDLE_LOAD, SCHED_MOSTLY_IDLE_LOAD_CORE, \
                                        sch_mst_idl_ld_s, sch_mst_idl_ld_sl, "perf_lock_rel");
            stored_sched_mostly_idle_load = 0;
        }
        return rc;
    }

    if (!stored_sched_mostly_idle_load) {
        if (!ispercore) {
            FREAD_STR(SCHED_MOSTLY_IDLE_LOAD, sch_mst_idl_ld_s, NODE_MAX, rc);
            if (rc > 0) {
                QLOGI("%s read with %s return value %d", SCHED_MOSTLY_IDLE_LOAD, sch_mst_idl_ld_s, rc);
                sch_mst_idl_ld_sl = strlen(sch_mst_idl_ld_s);
                stored_sched_mostly_idle_load = 1;
            } else {
                QLOGE("Failed to read %s",SCHED_MOSTLY_IDLE_LOAD);
                return FAILED;
            }
        } else {
            snprintf(idleloadnode, NODE_MAX, SCHED_MOSTLY_IDLE_LOAD_CORE, 0);
            FREAD_STR(idleloadnode, sch_mst_idl_ld_s, NODE_MAX, rc);
            if (rc > 0) {
                QLOGI("%s read with %s return value %d", idleloadnode, sch_mst_idl_ld_s, rc);
                sch_mst_idl_ld_sl = strlen(sch_mst_idl_ld_s);
                stored_sched_mostly_idle_load = 1;
            } else {
                //better return failed as we should not acquire lock which we can not release
                QLOGE("Failed to read %s",idleloadnode);
                return FAILED;
            }
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    return update_core_based_param(ispercore, SCHED_MOSTLY_IDLE_LOAD, SCHED_MOSTLY_IDLE_LOAD_CORE, \
                                   tmp_s, strlen(tmp_s), "perf_lock_acq");
}

/* Set sched_mostly_idle_nr_run
   In current implementation we are considering
   all cores should have same value. If this
   assumption changes, code needs to be updated */
int OptsHandler::sched_mostly_idle_nr_run(Resource &r,  OptsData &d) {
    int rc = 0;
    static int ispercore = -1;
    char tmp_s[NODE_MAX], nrrunnode[NODE_MAX];
    static unsigned int stored_sched_mostly_idle_nr_run = 0;
    static char sch_mst_idl_nr_rn_s[NODE_MAX];        // sched mostly idle nr run storage
    static int  sch_mst_idl_nr_rn_sl = -1;            // sched mostly idle nr run string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (ispercore == -1) {
        FREAD_STR(SCHED_MOSTLY_IDLE_NR_RUN, tmp_s, NODE_MAX, rc);
        if (rc > 0) {
            ispercore = 0;
        } else {
            snprintf(nrrunnode, NODE_MAX, SCHED_MOSTLY_IDLE_NR_RUN_CORE, 0);
            FREAD_STR(nrrunnode, tmp_s, NODE_MAX, rc);
            if (rc > 0) {
                ispercore = 1;
            } else {
                QLOGE("Error: sched_mostly_idle_nr_run node is not present");
                return FAILED;
            }
        }
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_mst_idl_nr_rn_sl > 0) {
           rc = update_core_based_param(ispercore, SCHED_MOSTLY_IDLE_NR_RUN, SCHED_MOSTLY_IDLE_NR_RUN_CORE, \
                                        sch_mst_idl_nr_rn_s, sch_mst_idl_nr_rn_sl, "perf_lock_rel");
            stored_sched_mostly_idle_nr_run = 0;
        }
        return rc;
    }

    if (!stored_sched_mostly_idle_nr_run) {
        if (!ispercore) {
            FREAD_STR(SCHED_MOSTLY_IDLE_NR_RUN, sch_mst_idl_nr_rn_s, NODE_MAX, rc);
            if (rc > 0) {
                QLOGI("%s read with %s return value %d", SCHED_MOSTLY_IDLE_NR_RUN, sch_mst_idl_nr_rn_s, rc);
                sch_mst_idl_nr_rn_sl = strlen(sch_mst_idl_nr_rn_s);
                stored_sched_mostly_idle_nr_run = 1;
            } else {
                QLOGE("Failed to read %s",SCHED_MOSTLY_IDLE_NR_RUN);
                return FAILED;
            }
        } else {
            snprintf(nrrunnode, NODE_MAX, SCHED_MOSTLY_IDLE_NR_RUN_CORE, 0);
            FREAD_STR(nrrunnode, sch_mst_idl_nr_rn_s, NODE_MAX, rc);
            if (rc > 0) {
                QLOGI("%s read with %s return value %d", nrrunnode, sch_mst_idl_nr_rn_s, rc);
                sch_mst_idl_nr_rn_sl = strlen(sch_mst_idl_nr_rn_s);
                stored_sched_mostly_idle_nr_run = 1;
            } else {
                QLOGE("Failed to read %s",nrrunnode);
                return FAILED;
            }
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    return update_core_based_param(ispercore, SCHED_MOSTLY_IDLE_NR_RUN, SCHED_MOSTLY_IDLE_NR_RUN_CORE, \
                                   tmp_s, strlen(tmp_s), "perf_lock_acq");
}

/* Set sched_mostly_idle_freq */
int OptsHandler::sched_mostly_idle_freq(Resource &r,  OptsData &d)
{
    int rc = 0;
    int i = 0, cpu = -1;
    static int ispercore = -1;
    char tmp_s[NODE_MAX], idlefreqnode[NODE_MAX];
    static char sch_mst_idl_f_s[NODE_MAX];            // sched mostly idle freq storage	505
    static int  sch_mst_idl_f_sl = -1;                // sched mostly idle freq string length
    static unsigned int stored_sched_mostly_idle_freq = 0;
    unsigned int reqval = r.value;
    unsigned int valCluster1 = 0, valCluster2 = 0;
    Target &t = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (ispercore == -1) {
        FREAD_STR(SCHED_MOSTLY_IDLE_FREQ, tmp_s, NODE_MAX, rc);
        if (rc > 0) {
            ispercore = 0;
        } else {
            snprintf(idlefreqnode, NODE_MAX, SCHED_MOSTLY_IDLE_FREQ_CORE, 0);
            FREAD_STR(idlefreqnode, tmp_s, NODE_MAX, rc);
            if (rc > 0) {
                ispercore = 1;
            } else {
                QLOGE("Error: sched_mostly_idle_freq node is not present");
                return FAILED;
            }
        }
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_mst_idl_f_sl > 0) {
           rc = update_core_based_param(ispercore, SCHED_MOSTLY_IDLE_FREQ, SCHED_MOSTLY_IDLE_FREQ_CORE, \
                                        sch_mst_idl_f_s, sch_mst_idl_f_sl, "perf_lock_rel");
            stored_sched_mostly_idle_freq = 0;
        }
        return rc;
    }

    if (!stored_sched_mostly_idle_freq) {
        if (!ispercore) {
            FREAD_STR(SCHED_MOSTLY_IDLE_FREQ, sch_mst_idl_f_s, NODE_MAX, rc);
            if (rc > 0) {
                QLOGI("%s read with %s return value %d", SCHED_MOSTLY_IDLE_FREQ, sch_mst_idl_f_s, rc);
                sch_mst_idl_f_sl = strlen(sch_mst_idl_f_s);
                stored_sched_mostly_idle_freq = 1;
            } else {
                QLOGE("Failed to read %s",SCHED_MOSTLY_IDLE_FREQ);
                return FAILED;
            }
        } else {
            snprintf(idlefreqnode, NODE_MAX, SCHED_MOSTLY_IDLE_FREQ_CORE, 0);
            FREAD_STR(idlefreqnode, sch_mst_idl_f_s, NODE_MAX, rc);
            if (rc > 0) {
                QLOGI("%s read with %s return value %d", idlefreqnode, sch_mst_idl_f_s, rc);
                sch_mst_idl_f_sl = strlen(sch_mst_idl_f_s);
                stored_sched_mostly_idle_freq = 1;
            } else {
                QLOGE("Failed to read %s",idlefreqnode);
                return FAILED;
            }
        }
    }

    if (reqval > 0) {
        reqval *= FREQ_MULTIPLICATION_FACTOR;
        if ((cpu = get_online_core(0, t.getLastCoreIndex(0)))!= FAILED) {
             valCluster1 =  d.find_next_cpu_frequency(cpu, reqval);
             QLOGI("valCluster1=%d", valCluster1);
        }
        if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED) {
             valCluster2 =  d.find_next_cpu_frequency(cpu, reqval);
             QLOGI("valCluster2=%d", valCluster2);
        }
    }

    if (!ispercore) {
        snprintf(tmp_s, NODE_MAX, "%d", valCluster1);
        FWRITE_STR(SCHED_MOSTLY_IDLE_FREQ, tmp_s, strlen(tmp_s), rc);
        QLOGI("perf_lock_acq: updated %s with %s return value %d", SCHED_MOSTLY_IDLE_FREQ, tmp_s, rc);
    } else if (ispercore == 1) {
        snprintf(tmp_s, NODE_MAX, "%d", valCluster1);
        for (i = 0; i <= t.getLastCoreIndex(0); i++) {
            snprintf(idlefreqnode, NODE_MAX, SCHED_MOSTLY_IDLE_FREQ_CORE, i);
            FWRITE_STR(idlefreqnode, tmp_s, strlen(tmp_s), rc);
            QLOGI("perf_lock_acq: updated %s with %s return value %d", idlefreqnode, tmp_s, rc);
        }
        snprintf(tmp_s, NODE_MAX, "%d", valCluster2);
        for (i = t.getLastCoreIndex(0)+1; i <= t.getLastCoreIndex(1); i++) {
            snprintf(idlefreqnode, NODE_MAX, SCHED_MOSTLY_IDLE_FREQ_CORE, i);
            FWRITE_STR(idlefreqnode, tmp_s, strlen(tmp_s), rc);
            QLOGI("perf_lock_acq: updated %s with %s return value %d", idlefreqnode, tmp_s, rc);
        }
    }

    return rc;
}

/* Set sched_init_task_load */
int OptsHandler::sched_init_task_load(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_init_task_load = 0;
    static char sch_init_tsk_ld_s[NODE_MAX];          // sched init task load storage
    static int  sch_init_tsk_ld_sl = -1;              // sched init task load string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_init_tsk_ld_sl > 0) {
            FWRITE_STR(SCHED_INIT_TASK_LOAD, sch_init_tsk_ld_s, sch_init_tsk_ld_sl, rc);
            stored_sched_init_task_load = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_INIT_TASK_LOAD, sch_init_tsk_ld_s, rc);
        }
        return rc;
    }

    if (!stored_sched_init_task_load) {
        FREAD_STR(SCHED_INIT_TASK_LOAD, sch_init_tsk_ld_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_INIT_TASK_LOAD, sch_init_tsk_ld_s, rc);
            sch_init_tsk_ld_sl = strlen(sch_init_tsk_ld_s);
            stored_sched_init_task_load = 1;
        } else {
            QLOGE("Failed to read %s", SCHED_INIT_TASK_LOAD);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_INIT_TASK_LOAD, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", SCHED_INIT_TASK_LOAD, tmp_s, rc);
    return rc;
}

/* Set sched_upmigrate */
int OptsHandler::sched_upmigrate(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_upmigrate = 0;
    static char sch_upmig_s[NODE_MAX];                // sched upmigrate storage
    static int  sch_upmig_sl = -1;                    // sched upmigrate string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_upmig_sl > 0) {
            FWRITE_STR(SCHED_UPMIGRATE, sch_upmig_s, sch_upmig_sl, rc);
            stored_sched_upmigrate = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_UPMIGRATE, sch_upmig_s, rc);
        }
        return rc;
    }

    if (!stored_sched_upmigrate) {
        FREAD_STR(SCHED_UPMIGRATE, sch_upmig_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_UPMIGRATE, sch_upmig_s, rc);
            sch_upmig_sl = strlen(sch_upmig_s);
            stored_sched_upmigrate = 1;
        }
        else {
            //better return fail, we should not acq lock when we can not release
            QLOGE("Failed to read %s", SCHED_UPMIGRATE);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_UPMIGRATE, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", SCHED_UPMIGRATE, tmp_s, rc);
    return rc;
}

/* Set sched_downmigrate */
int OptsHandler::sched_downmigrate(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_downmigrate = 0;
    static char sch_dwnmig_s[NODE_MAX];               // sched downmigrate storage
    static int  sch_dwnmig_sl = -1;                   // sched downmigrate string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_dwnmig_sl > 0) { //todo: verify: bug in original code
            FWRITE_STR(SCHED_DOWNMIGRATE, sch_dwnmig_s, sch_dwnmig_sl, rc);
            stored_sched_downmigrate = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_DOWNMIGRATE, sch_dwnmig_s, rc);
        }
        return rc;
    }

    if (!stored_sched_downmigrate) {
        FREAD_STR(SCHED_DOWNMIGRATE, sch_dwnmig_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_DOWNMIGRATE, sch_dwnmig_s, rc);
            sch_dwnmig_sl = strlen(sch_dwnmig_s);
            stored_sched_downmigrate = 1;
        } else {
            QLOGE("Failed to read %s", SCHED_DOWNMIGRATE);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_DOWNMIGRATE, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", SCHED_DOWNMIGRATE, tmp_s, rc);
    return rc;
}

/* Set sched boost */
int OptsHandler::sched_boost(Resource &r, OptsData &d) {
    int rc = -1;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_boost = 0;
    static char sch_boost_s[NODE_MAX]; // sched boost storage
    static int  sch_boost_sl = -1; // sched boost string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_boost_sl > 0) {
            FWRITE_STR(SCHED_BOOST_NODE, sch_boost_s, sch_boost_sl, rc);
            stored_sched_boost = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_BOOST_NODE, sch_boost_s, rc);
        }
        d.is_active = false;
        return rc;
    }

    if (d.is_active) {
        QLOGE("Sched boost lock is already acquired, concurrency is not supported for this resource");
        return FAILED;
    }


    if (!stored_sched_boost) {
        FREAD_STR(SCHED_BOOST_NODE, sch_boost_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_BOOST_NODE, sch_boost_s, rc);
            sch_boost_sl = strlen(sch_boost_s);
            stored_sched_boost = 1;
        } else {
            QLOGE("Failed to read %s", SCHED_BOOST_NODE);
            return FAILED;
        }
    }

    reqval = !!reqval; /* 0 or 1 */
    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_BOOST_NODE, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock: updated %s with %s return value %d", SCHED_BOOST_NODE, tmp_s, rc);
    if (rc > 0) {
        d.is_active = 1 ;
    }
    return rc;
}

/* Set sched frequency aggregation */
int OptsHandler::sched_set_freq_aggr(Resource &r, OptsData &d) {
    int rc = -1;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_freq_aggr = 0;
    static char sch_freq_aggr_s[NODE_MAX]; // sched freq aggr storage
    static int  sch_freq_aggr_sl = -1; // sched freq aggr string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_freq_aggr_sl > 0) {
            FWRITE_STR(SCHED_FREQ_AGGREGATE_NODE, sch_freq_aggr_s, sch_freq_aggr_sl, rc);
            stored_sched_freq_aggr = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_FREQ_AGGREGATE_NODE, sch_freq_aggr_s, rc);
        }
        d.mSchedFreqAggrGroupData.checkSchedFreqAggrPresence();
        return rc;
    }

    if (!stored_sched_freq_aggr) {
        FREAD_STR(SCHED_FREQ_AGGREGATE_NODE, sch_freq_aggr_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_FREQ_AGGREGATE_NODE, sch_freq_aggr_s, rc);
            sch_freq_aggr_sl = strlen(sch_freq_aggr_s);
            stored_sched_freq_aggr = 1;
        } else {
            QLOGE("Failed to read %s", SCHED_FREQ_AGGREGATE_NODE);
            return FAILED;
        }
    }

    reqval = !!reqval; /* 0 or 1 */
    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_FREQ_AGGREGATE_NODE, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock: updated %s with %s return value %d", SCHED_FREQ_AGGREGATE_NODE, tmp_s, rc);
    d.mSchedFreqAggrGroupData.checkSchedFreqAggrPresence();
    return rc;
}

/* Set sched enable thread grouping */
int OptsHandler::sched_set_enable_thread_grouping(Resource &r, OptsData &d) {
    int rc = -1;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_enable_grouping = 0;
    static char sch_thread_grouping_s[NODE_MAX]; // sched freq aggr storage
    static int  sch_thread_grouping_sl = -1; // sched freq aggr string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_thread_grouping_sl > 0) {
            FWRITE_STR(SCHED_ENABLE_THREAD_GROUPING, sch_thread_grouping_s, sch_thread_grouping_sl, rc);
            stored_sched_enable_grouping = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_ENABLE_THREAD_GROUPING, sch_thread_grouping_s, rc);
        }
        return rc;
    }

    if (!stored_sched_enable_grouping) {
        FREAD_STR(SCHED_ENABLE_THREAD_GROUPING, sch_thread_grouping_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_ENABLE_THREAD_GROUPING, sch_thread_grouping_s, rc);
            sch_thread_grouping_sl = strlen(sch_thread_grouping_s);
            stored_sched_enable_grouping = 1;
        } else {
            QLOGE("Failed to read %s", SCHED_ENABLE_THREAD_GROUPING);
            return FAILED;
        }
    }

    reqval = !!reqval; /* 0 or 1 */
    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_ENABLE_THREAD_GROUPING, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock: updated %s with %s return value %d", SCHED_ENABLE_THREAD_GROUPING, tmp_s, rc);
    return rc;
}

static thread_group get_threads_of_interest_for_sched_group(int tid) {
    char name[NODE_MAX], data[NODE_MAX];
    int rc, *tids = new int[HWUI_SCHED_GROUP_SIZE], num_tids = 0;
    memset(tids, 0, HWUI_SCHED_GROUP_SIZE * sizeof(int));
    memset(data, 0, NODE_MAX);
    memset(name, 0, NODE_MAX);
    snprintf(name, NODE_MAX, "/proc/%d/task", tid);
    DIR* proc_dir = opendir(name);
    if (proc_dir) {
        tids[0] = tid;
        num_tids++;
        struct dirent *ent;
        while (((ent = readdir(proc_dir)) != NULL) && (num_tids < 5)) {
            if (strcmp(ent->d_name, ".") && strcmp(ent->d_name, "..")) {
                memset(name, 0, NODE_MAX);
                snprintf(name, NODE_MAX, "/proc/%s/status", ent->d_name);
                memset(data, 0, NODE_MAX);
                FREAD_STR(name, data, NODE_MAX - 1, rc)
                if (sched_group_data::isHWUIThread(data)) {
                    tids[num_tids] = atoi(ent->d_name);
                    num_tids++;
                }
            }
        }
        closedir(proc_dir);
    }
    thread_group t;
    /*
     * Apply sched group only for HWUI Threads.
     */
    if (!num_tids ||
               (num_tids < (HWUI_SCHED_GROUP_SIZE - 1))) {
        num_tids = 0;
        delete[] tids;
        tids = NULL;
        QLOGI("sched_group NOT Found");
    }

    t.tids = tids;
    t.num_tids = num_tids;
    return t;
}

static void write_sched_group(thread_group t, int enable) {
    if (!t.num_tids)
        return;
    char value[NODE_MAX];
    memset(value, 0, NODE_MAX);
    if (enable) {
        snprintf(value, NODE_MAX, "%d", t.tids[0]);
    } else
        value[0] = '0';
    int len = strlen(value), rc;

    for (int i = 0; i < t.num_tids; i++) {
        if (t.tids[i] != 0) {
            char file_name[NODE_MAX];
            memset(file_name, 0, NODE_MAX);
            snprintf(file_name, NODE_MAX, "/proc/%d/sched_group_id",
                                                             t.tids[i]);
            FWRITE_STR(file_name, value, len, rc);
            QLOGI("sched_group_id for tid = %d is %s", t.tids[i], value);
        }
    }
}

/* Add sched group */
int OptsHandler::add_sched_group(Resource &r, OptsData &d) {
    int rc = 0;
    char *bptr;
    char value[NODE_MAX];

    if (d.mSchedGroupData.Found(r.value))
        return rc;

    thread_group t = get_threads_of_interest_for_sched_group(r.value);
    d.mSchedGroupData.Add(r.value, t);
    write_sched_group(t, 1);
    return rc;
}

/* Reset sched group */
int OptsHandler::reset_sched_group(Resource &r, OptsData &d) {
    int rc = 0;
    char *bptr;
    char value[NODE_MAX];

    if (!d.mSchedGroupData.Found(r.value))
        return rc;

    thread_group t = d.mSchedGroupData.Get(r.value);
    if (t.tids) {
        write_sched_group(t, 0);
        delete[] t.tids;
        t.tids = 0;
        t.num_tids = 0;
    }
    d.mSchedGroupData.Remove(r.value);
    return rc;
}



int OptsHandler::write_sched_freq_aggr_group(int tid, bool enable) {
    DIR *dir;
    struct dirent *readDir;
    char file_name[NODE_MAX], dir_name[NODE_MAX], pid_str[NODE_MAX];
    int fd, rc=-1,rc1=-1;

    memset(dir_name, 0, NODE_MAX);
    snprintf(dir_name, NODE_MAX, "/proc/%d/task", tid);

    memset(pid_str, 0, NODE_MAX);
    if (enable) {
        snprintf(pid_str, NODE_MAX, "%d", tid);
    } else {
        pid_str[0] = '0';
    }

    /*find all tasks related to passed TID and set sched_group_id*/
    if((dir = opendir(dir_name))) {
        while((readDir = readdir(dir))) {
            memset(file_name, 0, NODE_MAX);
            snprintf(file_name, NODE_MAX, "/proc/%s/sched_group_id", readDir->d_name);
            fd = open(file_name, O_WRONLY);
            if (fd >= 0) {
                rc1 = write(fd, pid_str, strlen(pid_str));// set sgid
                close(fd);
            }
        } //while(readDir)
        closedir(dir);
    } else { //if(opendir)
        QLOGE("%s, opendir() failed on /proc/%d/task", __FUNCTION__, tid);
        rc=-1;
        return rc;
    }
    /*Scheduler group_id inheritance feature ensures setting of sched_group_id for any
      new child thread getting created after sched_group_id is set for the App process.
      Need to check if there exists any window where above while loop may miss settting
      sched_group_id for any child thread created during above mentioned boundary*/
    rc=0;
    return rc;
}

/* Add sched freq aggr group */
int OptsHandler::sched_add_freq_aggr_group(Resource &r, OptsData &d) {
    int rc = 0, instanceRefCount=1;
    bool enableFA = true;

    if (!d.mSchedFreqAggrGroupData.IsSchedFreqAggrPresent()) {
        QLOGE("%s, Error: Sched Frequency Aggregation support not present", __FUNCTION__);
        return FAILED;
    }
    QLOGI("%s, pid=%d",__FUNCTION__, r.value);

    if (d.mSchedFreqAggrGroupData.Found(r.value)) {
        /*tid entry already exists, increment instanceRefCount and exit
          as Frequency Aggregation is already enabled on tid*/
        QLOGI("%s, tid %d already in hastable", __FUNCTION__, r.value);
        d.mSchedFreqAggrGroupData.IncrementInsRefCntVal(r.value);
        return rc;
    }
    /*Add instanceRefCount as 1 using tid as key*/
    d.mSchedFreqAggrGroupData.Add(r.value, instanceRefCount);
    write_sched_freq_aggr_group(r.value, enableFA);
    return rc;
}

int OptsHandler::sched_restore_cpuset(Resource &r, char *cpuset_node) {
    int rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }
    Target &t = Target::getCurTarget();
    int all_cpus_bitmask = t.getAllCpusBitmask();
    if(all_cpus_bitmask > 0) {
        char *cpuset_str = cpuset_bitmask_to_str(all_cpus_bitmask);
        if(cpuset_str != NULL) {
            FWRITE_STR(cpuset_node, cpuset_str, CPUSET_STR_SIZE, rc);
            delete[] cpuset_str;
        } else {
            QLOGE("Error generating string from bitmask.");
        }
    } else {
        QLOGE("Failed to restore cpuset.");
    }
    return rc;
}

int OptsHandler::sched_update_cpuset(Resource &r, char *cpuset_node) {
    int rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }
    char *cpuset_str = cpuset_bitmask_to_str(r.value);
    if(cpuset_str != NULL) {
        QLOGI("%s: BITMASK=%d, STRING=%s", cpuset_node, r.value, cpuset_str);
        FWRITE_STR(cpuset_node, cpuset_str, CPUSET_STR_SIZE, rc);
        delete[] cpuset_str;
    } else {
        QLOGE("Error generating string from bitmask.");
    }
    return rc;
}

int OptsHandler::sched_update_top_app_cpuset(Resource &r, OptsData &d) {
    return sched_update_cpuset(r, SCHED_TOP_APP_CPUSET);
}

int OptsHandler::sched_restore_top_app_cpuset(Resource &r, OptsData &d) {
    return sched_restore_cpuset(r, SCHED_TOP_APP_CPUSET);
}

int OptsHandler::sched_update_fg_cpuset(Resource &r, OptsData &d) {
    return sched_update_cpuset(r, SCHED_FOREGROUND_CPUSET);
}

int OptsHandler::sched_restore_fg_cpuset(Resource &r, OptsData &d) {
    return sched_restore_cpuset(r, SCHED_FOREGROUND_CPUSET);
}

int OptsHandler::sched_update_sys_bg_cpuset(Resource &r, OptsData &d) {
    return sched_update_cpuset(r, SCHED_SYSTEM_BACKGROUND_CPUSET);
}

int OptsHandler::sched_restore_sys_bg_cpuset(Resource &r, OptsData &d) {
    return sched_restore_cpuset(r, SCHED_SYSTEM_BACKGROUND_CPUSET);
}

int OptsHandler::sched_update_bg_cpuset(Resource &r, OptsData &d) {
    return sched_update_cpuset(r, SCHED_BACKGROUND_CPUSET);
}

int OptsHandler::sched_restore_bg_cpuset(Resource &r, OptsData &d) {
    return sched_restore_cpuset(r, SCHED_BACKGROUND_CPUSET);
}

char *OptsHandler::cpuset_bitmask_to_str(unsigned int bitmask) {
    int i = 0;
    int str_index = 0;
    char *cpuset_str = new char[CPUSET_STR_SIZE];
    if(cpuset_str != NULL) {
        memset(cpuset_str, NULL, CPUSET_STR_SIZE);
        while(bitmask != 0) {
            if(bitmask & 1 && str_index < CPUSET_STR_SIZE - 1) {
                int chars_copied = snprintf(cpuset_str + str_index, CPUSET_STR_SIZE - str_index, "%d,", i);
                str_index += chars_copied;
            }
            bitmask = bitmask >> 1;
            i++;
        }
        if(str_index > 0) {
            cpuset_str[str_index-1] = NULL;
        }
    }
    return cpuset_str;
}

/* Reset sched freq aggr group */
int OptsHandler::sched_reset_freq_aggr_group(Resource &r, OptsData &d) {
    int rc = 0, instanceRefCount;
    bool disableFA = false;

    if (!d.mSchedFreqAggrGroupData.IsSchedFreqAggrPresent())
        return FAILED;
    QLOGI("%s, pid=%d",__FUNCTION__, r.value);

    if (!d.mSchedFreqAggrGroupData.Found(r.value)) {
        QLOGI("%s, tid %d not found in hastable", __FUNCTION__, r.value);
        return rc;
    }

    if ((instanceRefCount = d.mSchedFreqAggrGroupData.Get(r.value)) == 1) {
        write_sched_freq_aggr_group(r.value, disableFA);
        d.mSchedFreqAggrGroupData.Remove(r.value);
    } else if (instanceRefCount > 1) {
        /*more reference to tid entry exists, decrement instanceRefCount
          and exit*/
        d.mSchedFreqAggrGroupData.DecrementInsRefCntVal(r.value);
    }
    return rc;
}


/* Set sched_spill_nr_run */
int OptsHandler::sched_spill_nr_run(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_spill_nr_run = 0;
    static char sch_spl_nr_run_s[NODE_MAX]; // sched spill nr run storage
    static int  sch_spl_nr_run_sl = -1; // sched spill nr run string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_spl_nr_run_sl > 0) {
            FWRITE_STR(SCHED_SPILL_NR_RUN_NODE, sch_spl_nr_run_s, sch_spl_nr_run_sl, rc);
            stored_sched_spill_nr_run = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_SPILL_NR_RUN_NODE, sch_spl_nr_run_s, rc);
        }
        return rc;
    }

    if (!stored_sched_spill_nr_run) {
        FREAD_STR(SCHED_SPILL_NR_RUN_NODE, sch_spl_nr_run_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_SPILL_NR_RUN_NODE, sch_spl_nr_run_s, rc);
            sch_spl_nr_run_sl = strlen(sch_spl_nr_run_s);
            stored_sched_spill_nr_run = 1;
        } else {
            //better do not acquire lock as we can not release
            QLOGE("Failed to read %s ", SCHED_SPILL_NR_RUN_NODE);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_SPILL_NR_RUN_NODE, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", SCHED_SPILL_NR_RUN_NODE, tmp_s, rc);
    return rc;
}

/* Set sched_static_cpu_pwr_cost */
int OptsHandler::sched_static_cpu_pwr_cost(Resource &r,  OptsData &d) {
    int rc = 0, i;
    char tmp_s[NODE_MAX], tmp_node[NODE_MAX];
    static char sch_cpu_pwr_cost_s[MAX_CPUS][NODE_MAX];            // sched static cpu pwr cost storage
    static int  sch_cpu_pwr_cost_sl[MAX_CPUS];                     // sched static cpu pwr cost string length
    static unsigned int stored_sched_cpu_pwr_cost[MAX_CPUS];
    unsigned int reqval = r.value;
    int cpu = r.core;
    Target &t = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, ASYNC_CORE);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_cpu_pwr_cost_sl[cpu] > 0) {
            snprintf(tmp_node, NODE_MAX, SCHED_STATIC_CPU_PWR_COST_CORE, cpu);
            FWRITE_STR(tmp_node, sch_cpu_pwr_cost_s[cpu], sch_cpu_pwr_cost_sl[cpu], rc);
            stored_sched_cpu_pwr_cost[cpu] = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", tmp_node, sch_cpu_pwr_cost_s[cpu], rc);
        }
        return rc;
    }

    if (!stored_sched_cpu_pwr_cost[cpu]) {
        snprintf(tmp_node, NODE_MAX, SCHED_STATIC_CPU_PWR_COST_CORE, cpu);
        FREAD_STR(tmp_node, sch_cpu_pwr_cost_s[cpu], NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", tmp_node, sch_cpu_pwr_cost_s[cpu], rc);
            sch_cpu_pwr_cost_sl[cpu] = strlen(sch_cpu_pwr_cost_s[cpu]);
            stored_sched_cpu_pwr_cost[cpu] = 1;
        } else {
            QLOGE("Failed to read %s", tmp_node);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    snprintf(tmp_node, NODE_MAX, SCHED_STATIC_CPU_PWR_COST_CORE, cpu);
    FWRITE_STR(tmp_node, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", tmp_node, tmp_s, rc);
    return rc;
}

/* Set sched_restrict_cluster_spill */
int OptsHandler::sched_restrict_cluster_spill(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sched_restrict_cluster_spill = 0;
    static char sch_rstrct_clust_spill_s[NODE_MAX]; // sched restrict cluster spill storage
    static int  sch_rstrct_clust_spill_sl = -1; // sched restrict cluster spill string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (sch_rstrct_clust_spill_sl > 0) {
            FWRITE_STR(SCHED_RESTRICT_CLUSTER_SPILL_NODE, sch_rstrct_clust_spill_s, sch_rstrct_clust_spill_sl, rc);
            stored_sched_restrict_cluster_spill = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SCHED_RESTRICT_CLUSTER_SPILL_NODE, sch_rstrct_clust_spill_s, rc);
        }
        return rc;
    }

    if (!stored_sched_restrict_cluster_spill) {
        FREAD_STR(SCHED_RESTRICT_CLUSTER_SPILL_NODE, sch_rstrct_clust_spill_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SCHED_RESTRICT_CLUSTER_SPILL_NODE, sch_rstrct_clust_spill_s, rc);
            sch_rstrct_clust_spill_sl = strlen(sch_rstrct_clust_spill_s);
            stored_sched_restrict_cluster_spill = 1;
        } else {
            //better do not acquire lock as we can not release
            QLOGE("Failed to read %s ", SCHED_RESTRICT_CLUSTER_SPILL_NODE);
            return FAILED;
        }
    }

    reqval = !!reqval; /* 0 or 1 */

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SCHED_RESTRICT_CLUSTER_SPILL_NODE, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", SCHED_RESTRICT_CLUSTER_SPILL_NODE, tmp_s, rc);
    return rc;
}

int OptsHandler::interactive_above_hispeed_delay(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_ahd_s[NODE_MAX];                 // interactive cpu 0 above_hispeed_delay storage
    static int  ip_0_ahd_sl = -1;                     // interactive cpu 0 above_hispeed_delay string length
    static char ip_4_ahd_s[NODE_MAX];                 // interactive cpu 4 above_hispeed_delay storage
    static int  ip_4_ahd_sl = -1;                     // interactive cpu 4 above_hispeed_delay string length
    unsigned int reqval = r.value;
    char *ipahd_s;
    int *ipahd_sl;
    Target &t = Target::getCurTarget();
    int cpu = -1;

    /*Assigning pointer to use default value at release time
      based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0)))!= FAILED) {
                 d.iahd[27] = cpu + '0';
                 ipahd_s = ip_0_ahd_s;
                 ipahd_sl = &ip_0_ahd_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED) {
                 d.iahd[27] = cpu + '0';
                 ipahd_s = ip_4_ahd_s;
                 ipahd_sl = &ip_4_ahd_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        ipahd_s = ip_0_ahd_s;
        ipahd_sl = &ip_0_ahd_sl;
    }

    if (reqval == MAX_LVL) {
        if (*ipahd_sl > 0) {
            FWRITE_STR(d.iahd, ipahd_s, *ipahd_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.iahd, ipahd_s, rc);
            *ipahd_sl = -1;
        }
        goto check_write_failure;
    }

    if (*ipahd_sl == -1) {
        FREAD_STR(d.iahd, ipahd_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.iahd, ipahd_s, rc);
            *ipahd_sl = strlen(ipahd_s);
        }
        else {
            QLOGE("Failed to read %s",d.iahd);
            return FAILED;
        }
    }

    if (reqval == 0xFE) {
        snprintf(tmp_s, NODE_MAX, "19000 1400000:39000 1700000:19000");
    } else if (reqval == 0xFD) {
        snprintf(tmp_s, NODE_MAX, "%d", 19000);
    } else {
        reqval *= 10000;
        snprintf(tmp_s, NODE_MAX, "%d", reqval);
    }
    FWRITE_STR(d.iahd, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.iahd, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.iahd, rc);
    }
    return rc;
}

int OptsHandler::interactive_boost(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_b_s[NODE_MAX];                   // interactive cpu 0 boost storage
    static int  ip_0_b_sl = -1;                       // interactive cpu 0 boost string length
    static char ip_4_b_s[NODE_MAX];                   // interactive cpu 4 boost storage
    static int  ip_4_b_sl = -1;                       // interactive cpu 4 boost string length
    unsigned int reqval = r.value;
    char *ipb_s;
    int *ipb_sl;
    Target &t = Target::getCurTarget();
    int cpu = -1;

    /*Assigning pointer to use default value at release time
       based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED){
                d.intb[27] = cpu + '0';
                ipb_s = ip_0_b_s;
                ipb_sl = &ip_0_b_sl;
            }
            else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
                d.intb[27] = cpu + '0';
                ipb_s = ip_4_b_s;
                ipb_sl = &ip_4_b_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }
        ipb_s = ip_0_b_s;
        ipb_sl = &ip_0_b_sl;
    }

    if (reqval == MAX_LVL) {
        if (*ipb_sl > 0) {
            FWRITE_STR(d.intb, ipb_s, *ipb_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.intb, ipb_s, rc);
            *ipb_sl = -1;
        }
        goto check_write_failure;
    }

    if (*ipb_sl == -1) {
        FREAD_STR(d.intb, ipb_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.intb, ipb_s, rc);
            *ipb_sl = strlen(ipb_s);
        }
        else {
            QLOGE("Failed to read %s",d.intb);
            return FAILED;
        }
    }

    reqval = !!reqval; /* 0 or 1 */

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.intb, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.intb, tmp_s, rc);
    return rc;

    //We should not wait on acquire. it should always be best effort
    //If we can't, simply return failure.
    //this should be confirmed for all the interactive methods
check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.intb, rc);
    }
    return rc;
}

int OptsHandler::interactive_boostpulse(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    unsigned int reqval = r.value;
    Target &t = Target::getCurTarget();
    int cpu = -1 ;

    /*Assigning pointer to use default value at release time
      based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED) {
                d.ibp[27] = cpu + '0';
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
                d.ibp[27] = cpu + '0';
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.ibp, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.ibp, tmp_s, rc);
    return rc;
}

int OptsHandler::interactive_boostpulse_duration(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_bpd_s[NODE_MAX];                 // interactive cpu 0 boostpulse_duration storage
    static int  ip_0_bpd_sl = -1;                     // interactive cpu 0 boostpulse_duration string length
    static char ip_4_bpd_s[NODE_MAX];                 // interactive cpu 4 boostpulse_duration storage
    static int  ip_4_bpd_sl = -1;                     // interactive cpu 4 boostpulse_duration string length
    unsigned int reqval = r.value;
    char *ipbpd_s;
    int *ipbpd_sl;
    Target &t = Target::getCurTarget();
    int cpu = -1;

    /*Assigning pointer to use default value at release time
      based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED) {
                d.ibd[27] = cpu + '0';
                ipbpd_s = ip_0_bpd_s;
                ipbpd_sl = &ip_0_bpd_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
                d.ibd[27] = cpu + '0';
                ipbpd_s = ip_4_bpd_s;
                ipbpd_sl = &ip_4_bpd_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        ipbpd_s = ip_0_bpd_s;
        ipbpd_sl = &ip_0_bpd_sl;
    }

    if (reqval == MAX_LVL) {
        if (*ipbpd_sl > 0) {
            FWRITE_STR(d.ibd, ipbpd_s, *ipbpd_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.ibd, ipbpd_s, rc);
            *ipbpd_sl = -1;
        }
        goto check_write_failure;
    }

    if (*ipbpd_sl == -1) {
        FREAD_STR(d.ibd, ipbpd_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.ibd, ipbpd_s, rc);
            *ipbpd_sl = strlen(ipbpd_s);
        }
        else {
            QLOGE("Failed to read %s",d.ibd);
            return FAILED;
        }
    }

    reqval *= TIMER_MULTIPLICATION_FACTOR;

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.ibd, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.ibd, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.ibd, rc);
	}
    return rc;
}

/* Set go_hispeed_load */
int OptsHandler::interactive_go_hispeed_load(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_ghl_s[NODE_MAX];                 // interactive cpu 0 go_hispeed_load storage
    static int  ip_0_ghl_sl = -1;                     // interactive cpu 0 go_hispeed_load string length
    static char ip_4_ghl_s[NODE_MAX];                 // interactive cpu 4 go_hispeed_load storage
    static int  ip_4_ghl_sl = -1;                     // interactive cpu 4 go_hispeed_load string length
    unsigned int reqval = r.value;
    char *ipghl_s;
    int *ipghl_sl;
    Target &t = Target::getCurTarget();
    int cpu = -1;
    static int numCluster = t.getNumCluster();

     /*Assigning pointer to use default value at release time
       based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED){
                 d.ighl[27] = cpu + '0';
                 ipghl_s = ip_0_ghl_s;
                 ipghl_sl = &ip_0_ghl_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
                 d.ighl[27] = cpu + '0';
                 ipghl_s = ip_4_ghl_s;
                 ipghl_sl = &ip_4_ghl_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        ipghl_s = ip_0_ghl_s;
        ipghl_sl = &ip_0_ghl_sl;
    }

    if (reqval == MAX_LVL) {
        if (*ipghl_sl > 0) {
            FWRITE_STR(d.ighl, ipghl_s, *ipghl_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.ighl, ipghl_s, rc);
            *ipghl_sl = -1;
        }
        goto check_write_failure;
    }

    if (*ipghl_sl == -1) {
        FREAD_STR(d.ighl, ipghl_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.ighl, ipghl_s, rc);
            *ipghl_sl = strlen(ipghl_s);
        }
        else {
            QLOGE("Failed to read %s", d.ighl);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.ighl, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.ighl, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.ighl, rc);
	}
    return rc;
}

int OptsHandler::interactive_hispeed_freq(Resource &r, OptsData &d) {
    int rc = 0;
    int i = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_hf_s[NODE_MAX];                  // interactive cpu 0 hispeed_freq storage
    static int  ip_0_hf_sl = -1;                      // interactive cpu 0 hispeed_freq string length
    static char ip_4_hf_s[NODE_MAX];                  // interactive cpu 4 hispeed_freq storage
    static int  ip_4_hf_sl = -1;                      // interactive cpu 4 hispeed_freq string length
    unsigned int reqval = r.value;
    char *iphf_s;
    int *iphf_sl;
    Target &t = Target::getCurTarget();
    int cpu = -1;
    static int numCluster = t.getNumCluster();

     /*Assigning pointer to use default value at release time
       based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED){
                 d.ihf[27] = cpu + '0';
                 iphf_s = ip_0_hf_s;
                 iphf_sl = &ip_0_hf_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
                 d.ihf[27] = cpu + '0';
                 iphf_s = ip_4_hf_s;
                 iphf_sl = &ip_4_hf_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        iphf_s = ip_0_hf_s;
        iphf_sl = &ip_0_hf_sl;
    }

    if (reqval == MAX_LVL) {
        if (*iphf_sl > 0) {
            FWRITE_STR(d.ihf, iphf_s, *iphf_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.ihf, iphf_s, rc);
            *iphf_sl = -1;
        }
        goto check_write_failure;
    }

    if (reqval > 0) {
        reqval *= FREQ_MULTIPLICATION_FACTOR;
        reqval =  d.find_next_cpu_frequency(r.core, reqval);
    }

    if (*iphf_sl == -1) {
        FREAD_STR(d.ihf, iphf_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.ihf, iphf_s, rc);
            *iphf_sl = strlen(iphf_s);
        }
        else {
            QLOGE("Failed to read %s", d.ihf);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.ihf, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.ihf, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.ihf, rc);
	}
    return rc;
}

/* Set io_is_busy */
int OptsHandler::interactive_io_is_busy(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_ioib_s[NODE_MAX];                // interactive cpu 0 io_is_busy storage
    static int  ip_0_ioib_sl = -1;                    // interactive cpu 0 io_is_busy string length
    static char ip_4_ioib_s[NODE_MAX];                // interactive cpu 4 io_is_busy storage
    static int  ip_4_ioib_sl = -1;                    // interactive cpu 4 io_is_busy string length
    unsigned int reqval = r.value;
    char *ipioib_s;
    int *ipioib_sl;
    Target &t = Target::getCurTarget();
    int cpu = -1;
    static int numCluster = t.getNumCluster();

     /*Assigning pointer to use default value at release time
       based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED){
                 d.iiib[27] = cpu + '0';
                 ipioib_s = ip_0_ioib_s;
                 ipioib_sl = &ip_0_ioib_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
                 d.iiib[27] = cpu + '0';
                 ipioib_s = ip_4_ioib_s;
                 ipioib_sl = &ip_4_ioib_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        ipioib_s = ip_0_ioib_s;
        ipioib_sl = &ip_0_ioib_sl;
    }

    if (reqval == MAX_LVL) {
        if (*ipioib_sl > 0) {
            FWRITE_STR(d.iiib, ipioib_s, *ipioib_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.iiib, ipioib_s, rc);
            *ipioib_sl = -1;
        }
        goto check_write_failure;
    }

    if (*ipioib_sl == -1) {
        FREAD_STR(d.iiib, ipioib_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.iiib, ipioib_s, rc);
            *ipioib_sl = strlen(ipioib_s);
        }
        else {
            QLOGE("Failed to read %s", d.iiib);
            return FAILED;
        }
    }

    reqval = !!reqval; /* 0 or 1 */

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.iiib, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.iiib, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.iiib, rc);
	}
    return rc;
}

int OptsHandler::interactive_min_sample_time(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_mst_s[NODE_MAX];                 // interactive cpu 0 min_sample_time storage
    static int  ip_0_mst_sl = -1;                     // interactive cpu 0 min_sample_time string length
    static char ip_4_mst_s[NODE_MAX];                 // interactive cpu 4 min_sample_time storage
    static int  ip_4_mst_sl = -1;                     // interactive cpu 4 min_sample_time string length
    unsigned int reqval = r.value;
    char *ipmst_s;
    int *ipmst_sl;
    Target &t = Target::getCurTarget();
    int cpu = -1;

    /*Assigning pointer to use default value at release time
       based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED){
                d.imst[27] = cpu + '0';
                ipmst_s = ip_0_mst_s;
                ipmst_sl = &ip_0_mst_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED) {
                d.imst[27] = cpu + '0';
                ipmst_s = ip_4_mst_s;
                ipmst_sl = &ip_4_mst_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
       }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        ipmst_s = ip_0_mst_s;
        ipmst_sl = &ip_0_mst_sl;
    }

    if (reqval == MAX_LVL) {
        if (*ipmst_sl > 0) {
            FWRITE_STR(d.imst, ipmst_s, *ipmst_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.imst, ipmst_s, rc);
            *ipmst_sl = -1;
        }
        goto check_write_failure;
    }

    if (*ipmst_sl == -1) {
        FREAD_STR(d.imst, ipmst_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.imst, ipmst_s, rc);
            *ipmst_sl = strlen(ipmst_s);
        }
        else {
            QLOGE("Failed to read %s", d.imst);
            return FAILED;
        }
    }

    reqval *= TIMER_MULTIPLICATION_FACTOR;

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.imst, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.imst, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.imst, rc);
	}
    return rc;
}

int OptsHandler::interactive_target_loads(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_tl_s[NODE_MAX];                  // interactive cpu 0 target_loads storage
    static int  ip_0_tl_sl = -1;                      // interactive cpu 0 target_loads string length
    static char ip_4_tl_s[NODE_MAX];                  // interactive cpu 4 target_loads storage
    static int  ip_4_tl_sl = -1;                      // interactive cpu 4 target_loads string length
    unsigned int reqval = r.value;
    char *iptl_s;
    int *iptl_sl;
    Target &t = Target::getCurTarget();
    int cpu = -1;

    /*Assigning pointer to use default value at release time
      based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED) {
                d.itl[27] = cpu + '0';
                iptl_s = ip_0_tl_s;
                iptl_sl = &ip_0_tl_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED) {
                d.itl[27] = cpu + '0';
                iptl_s = ip_4_tl_s;
                iptl_sl = &ip_4_tl_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        iptl_s = ip_0_tl_s;
        iptl_sl = &ip_0_tl_sl;
    }

    if (reqval == MAX_LVL) {
        if (*iptl_sl > 0) {
            FWRITE_STR(d.itl, iptl_s, *iptl_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.itl, iptl_s, rc);
            *iptl_sl = -1;
        }
        goto check_write_failure;
    }

    if (*iptl_sl == -1) {
        FREAD_STR(d.itl, iptl_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.itl, iptl_s, rc);
            *iptl_sl = strlen(iptl_s);
        } else {
            QLOGE("Failed to read %s", d.itl);
            return FAILED;
        }
    }

    if (reqval == 0xFE) {
        snprintf(tmp_s, NODE_MAX, "85 1500000:90 1800000:70");
    } else {
        snprintf(tmp_s, NODE_MAX, "%d", reqval);
    }
    FWRITE_STR(d.itl, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.itl, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.itl, rc);
	}
    return rc;
}

/* Set the timer rate. */
int OptsHandler::interactive_timer_rate(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_tr_s[NODE_MAX];                  // interactive cpu 0 timer_rate storage
    static int  ip_0_tr_sl = -1;                      // interactive cpu 0 timer_rate string length
    static char ip_4_tr_s[NODE_MAX];                  // interactive cpu 4 timer_rate storage
    static int  ip_4_tr_sl = -1;                      // interactive cpu 4 timer_rate string length
    unsigned int reqval = r.value; //0xff - (data & 0xff);
    char *iptr_s;
    int *iptr_sl;
    int cpu;
    Target &t = Target::getCurTarget();
    static int numCluster = t.getNumCluster();

     /*Assigning pointer to use default value at release time
       based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED) {
                 d.itr[27] = cpu + '0';
                 iptr_s = ip_0_tr_s;
                 iptr_sl = &ip_0_tr_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED) {
                 d.itr[27] = cpu + '0';
                 iptr_s = ip_4_tr_s;
                 iptr_sl = &ip_4_tr_sl;
            } else {
                 QLOGE("Error! No core is online for cluster %d", r.cluster);
                 return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        iptr_s = ip_0_tr_s;
        iptr_sl = &ip_0_tr_sl;
    }

    if (reqval == MAX_LVL) {
        if (*iptr_sl > 0) {
            FWRITE_STR(d.itr, iptr_s, *iptr_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.itr, iptr_s, rc);
            *iptr_sl = -1;
        }
        goto check_write_failure;
    }

    if (*iptr_sl == -1) {
        FREAD_STR(d.itr, iptr_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.itr, iptr_s, rc);
            *iptr_sl = strlen(iptr_s);
        } else {
            QLOGE("Failed to read %s", d.itr);
            return FAILED;
        }
    }

    reqval *= TIMER_MULTIPLICATION_FACTOR;

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.itr, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.itr, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.itr, rc);
	}
    return rc;
}

int OptsHandler::interactive_timer_slack(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_ts_s[NODE_MAX];                  // interactive cpu 0 timer_slack storage
    static int  ip_0_ts_sl = -1;                      // interactive cpu 0 timer_slack string length
    static char ip_4_ts_s[NODE_MAX];                  // interactive cpu 4 timer_slack storage
    static int  ip_4_ts_sl = -1;                      // interactive cpu 4 timer_slack string length
    unsigned int reqval = r.value; //0xff - (data & 0xff); //todo:
    char *ipts_s;
    int *ipts_sl;
    int cpu = -1;
    Target &t = Target::getCurTarget();

        /*Assigning pointer to use default value at release time
      based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED) {
                d.its[27] = cpu + '0';
                ipts_s = ip_0_ts_s;
                ipts_sl = &ip_0_ts_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED) {
                d.its[27] = cpu + '0';
                ipts_s = ip_4_ts_s;
                ipts_sl = &ip_4_ts_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        ipts_s = ip_0_ts_s;
        ipts_sl = &ip_0_ts_sl;
    }

    if (reqval == MAX_LVL) {
        if (*ipts_sl > 0) {
            FWRITE_STR(d.its, ipts_s, *ipts_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.its, ipts_s, rc);
            *ipts_sl = -1;
        }
        goto check_write_failure;
    }

    if (*ipts_sl == -1) {
        FREAD_STR(d.its, ipts_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.its, ipts_s, rc);
            *ipts_sl = strlen(ipts_s);
        } else {
            QLOGE("Failed to read %s", d.iahd);
            return FAILED;
        }
    }

    reqval *= TIMER_MULTIPLICATION_FACTOR;

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.its, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.its, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.its, rc);
	}
    return rc;
}

int OptsHandler::interactive_max_freq_hysteresis(Resource &r, OptsData &d) {
    int rc = 0;
    int i = 0;
    char tmp_s[NODE_MAX];
    static char ip_0_mfh_s[NODE_MAX];                 // interactive cpu 0 max_freq_hysteresis storage
    static int  ip_0_mfh_sl = -1;                     // interactive cpu 0 max_freq_hysteresis string length
    static char ip_4_mfh_s[NODE_MAX];                 // interactive cpu 4 max_freq_hysteresis storage
    static int  ip_4_mfh_sl = -1;                     // interactive cpu 4 max_freq_hysteresis string length
    unsigned int reqval = r.value;
    char *ipmfh_s;
    int *ipmfh_sl;
    int cpu = -1;
    Target &t = Target::getCurTarget();

    /*Assigning pointer to use default value at release time
      based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED) {
                d.imf[27] = cpu + '0';
                ipmfh_s = ip_0_mfh_s;
                ipmfh_sl = &ip_0_mfh_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED) {
                d.imf[27] = cpu + '0';
                ipmfh_s = ip_4_mfh_s;
                ipmfh_sl = &ip_4_mfh_sl;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        ipmfh_s = ip_0_mfh_s;
        ipmfh_sl = &ip_0_mfh_sl;
    }

    if (reqval == MAX_LVL) {
        if (*ipmfh_sl > 0) {
            FWRITE_STR(d.imf, ipmfh_s, *ipmfh_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.imf, ipmfh_s, rc);
            *ipmfh_sl = -1;
        }
        goto check_write_failure;;
    }

    if (*ipmfh_sl == -1) {
        FREAD_STR(d.imf, ipmfh_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.imf, ipmfh_s, rc);
            *ipmfh_sl = strlen(ipmfh_s);
        } else {
            QLOGE("Failed to read %s", d.imf);
            return FAILED;
        }
    }

    if (reqval > 0) {
        reqval *= TIMER_MULTIPLICATION_FACTOR;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.imf, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.imf, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.imf, rc);
    }
    return rc;
}

/* Set interactive_use_sched_load */
int OptsHandler::interactive_use_sched_load(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char intusl_clust0[NODE_MAX];              // interactive use_sched_load storage for cluster 0
    static int  intusll_clust0 = -1;                  // interactive use_sched_load string length for cluster 0
    static char intusl_clust1[NODE_MAX];           // interactive use_sched_load storage for cluster 1
    static int  intusll_clust1 = -1;               // interactive use_sched_load string length for cluster 1
    unsigned int reqval = r.value;
    char *iusl_s;
    int *iusl_sl;
    int cpu = -1;
    Target &t = Target::getCurTarget();

    /*Assigning pointer to use default value at release time
      based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED){
                d.iusl[27] = cpu + '0';
                iusl_s = intusl_clust0;
                iusl_sl = &intusll_clust0;
            }
            else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
                d.iusl[27] = cpu + '0';
                iusl_s = intusl_clust1;
                iusl_sl = &intusll_clust1;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        iusl_s = intusl_clust0;
        iusl_sl = &intusll_clust0;
    }

    if (reqval == MAX_LVL) {
        if (*iusl_sl > 0) {
            FWRITE_STR(d.iusl, iusl_s, *iusl_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.iusl, iusl_s, rc);
            *iusl_sl = -1;
        }
        goto check_write_failure;
    }

    if (*iusl_sl == -1) {
        FREAD_STR(d.iusl, iusl_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.iusl, iusl_s, rc);
            *iusl_sl = strlen(iusl_s);
        } else {
            QLOGE("Failed to read %s", d.iusl);
            return FAILED;
        }
    }

    //todo: check this
    snprintf(tmp_s, NODE_MAX, "%d", 0);
    FWRITE_STR(d.iusl, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.iusl, tmp_s, rc);

    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.iusl, rc);
    }
    return rc;
}

/* Set interactive_use_migration_notif */
int OptsHandler::interactive_use_migration_notif(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char intumn_clust0[NODE_MAX];              // interactive use_migration_notif storage for cluster 0
    static int  intumnl_clust0 = -1;                  // interactive use_migration_notif string length for cluster 0
    static char intumn_clust1[NODE_MAX];           // interactive use_migration_notif storage for cluster 1
    static int  intumnl_clust1 = -1;               // interactive use_migration_notif string length for cluster 1
    unsigned int reqval = r.value;
    char *iumn_s;
    int *iumn_sl;
    int cpu = -1;
    Target &t = Target::getCurTarget();

    /*Assigning pointer to use default value at release time
      based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        if (CLUSTER0 == r.cluster) {
            if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED){
                d.iumn[27] = cpu + '0';
                iumn_s = intumn_clust0;
                iumn_sl = &intumnl_clust0;
            }
            else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        } else {
            if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
                d.iumn[27] = cpu + '0';
                iumn_s = intumn_clust1;
                iumn_sl = &intumnl_clust1;
            } else {
                QLOGE("Error! No core is online for cluster %d", r.cluster);
                return FAILED;
            }
        }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        iumn_s = intumn_clust0;
        iumn_sl = &intumnl_clust0;
    }

    if (reqval == MAX_LVL) {
        if (*iumn_sl > 0) {
            FWRITE_STR(d.iumn, iumn_s, *iumn_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.iumn, iumn_s, rc);
            *iumn_sl = -1;
        }
        goto check_write_failure;
    }

    if (*iumn_sl == -1) {
        FREAD_STR(d.iumn, iumn_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.iumn, iumn_s, rc);
            *iumn_sl = strlen(iumn_s);
        } else {
            QLOGE("Failed to read %s", d.iumn);
            return FAILED;
        }
    }

    //todo: need to ask power team to send a value instead of setting it to zero over here
    snprintf(tmp_s, NODE_MAX, "%d", 0);
    FWRITE_STR(d.iumn, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.iumn, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        signal_chk_poll_thread(d.iumn, rc);
    }
    return rc;
}

/* Set interactive_ignore_hispeed_notif */
int OptsHandler::interactive_ignore_hispeed_notif(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char intihn_clust0[NODE_MAX];              // interactive ignore_hispeed_notif storage for cluster 0
    static int  intihnl_clust0 = -1;                  // interactive ignore_hispeed_notif string length for cluster 0
    static char intihn_clust1[NODE_MAX];              // interactive ignore_hispeed_notif storage for cluster 1
    static int  intihnl_clust1 = -1;                  // interactive ignore_hispeed_notif string length for cluster 1
    unsigned int reqval = r.value;
    char *iihn_s;
    int *iihn_sl;
    int cpu = -1;
    Target &t = Target::getCurTarget();

    /*Assigning pointer to use default value at release time
       based on cluster*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
       rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
       if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
       }

       if (CLUSTER0 == r.cluster) {
           if ((cpu = get_online_core(0, t.getLastCoreIndex(0))) != FAILED){
               d.iihn[27] = cpu + '0';
               iihn_s = intihn_clust0;
               iihn_sl = &intihnl_clust0;
           }
           else {
               QLOGE("Error! No core is online for cluster %d", r.cluster);
               return FAILED;
           }
       } else {
           if ((cpu = get_online_core(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1))) != FAILED){
               d.iihn[27] = cpu + '0';
               iihn_s = intihn_clust1;
               iihn_sl = &intihnl_clust1;
           } else {
               QLOGE("Error! No core is online for cluster %d", r.cluster);
               return FAILED;
           }
       }
    } else {
        rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        iihn_s = intihn_clust0;
        iihn_sl = &intihnl_clust0;
    }

    if (reqval == MAX_LVL) {
        if (*iihn_sl > 0) {
            FWRITE_STR(d.iihn, iihn_s, *iihn_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s, return value %d", d.iihn, iihn_s, rc);
            *iihn_sl = -1;
        }
        goto check_write_failure;
    }

    if (*iihn_sl == -1) {
        FREAD_STR(d.iihn, iihn_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.iihn, iihn_s, rc);
            *iihn_sl = strlen(iihn_s);
        } else {
            QLOGE("Failed to read %s", d.iihn);
            return FAILED;
        }
    }

    /* turn off ignore_hispeed_on_notify */
    snprintf(tmp_s, NODE_MAX, "%d", 0);
    FWRITE_STR(d.iihn, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s, return value %d", d.iihn, tmp_s, rc);
    return rc;

check_write_failure:
    if (rc <= 0 && CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
       signal_chk_poll_thread(d.iihn, rc);
    }
    return rc;
}

int OptsHandler::cpubw_hwmon_min_freq(Resource &r,  OptsData &d) {
    int i = 0;
    int rc = FAILED;
    int setval = -1;
    unsigned int reqval = r.value;
    static unsigned int stored_cpubw_hwmon_min_freq = 0;
    char tmp_s[NODE_MAX];
    char hwmon_max_freq[NODE_MAX];

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_mf_sl > 0) {
            FWRITE_STR(d.cpubw_minfreq_path, d.cpubw_mf_s, d.cpubw_mf_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_minfreq_path, d.cpubw_mf_s, rc);
            stored_cpubw_hwmon_min_freq = 0;
        }
        return rc;
    }

    if (reqval > 0) {
        reqval *= 100;
        for (i = 0; i < d.cpubw_avail_freqs_n; i++) {
            if (d.cpubw_avail_freqs[i] >= reqval) {
                setval = d.cpubw_avail_freqs[i];
                break;
            }
        }

        if (i == d.cpubw_avail_freqs_n) {
            setval = d.cpubw_avail_freqs[i - 1];
        }
    }

    if (!stored_cpubw_hwmon_min_freq) {
        FREAD_STR(d.cpubw_minfreq_path, d.cpubw_mf_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_minfreq_path, d.cpubw_mf_s, rc);
            d.cpubw_mf_sl = strlen(d.cpubw_mf_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_minfreq_path);
            return FAILED;
        }
        stored_cpubw_hwmon_min_freq = 1;
    }

    if (setval == FAILED) {
        QLOGE("Error! Perflock failed, invalid freq value %d", setval);
        return FAILED;
    }

    snprintf(tmp_s, NODE_MAX, "%d", setval);

    /* read hwmon max_freq  */
    FREAD_STR(d.cpubw_maxfreq_path, hwmon_max_freq, NODE_MAX, rc);

    /* min_freq cannot be more than intended value of
     * max_freq, so min_freq maintained same as max_freq
     */
    if (rc > 0) {
       if (atoi(tmp_s) > atoi(hwmon_max_freq)) {
          FWRITE_STR(d.cpubw_minfreq_path, hwmon_max_freq, strlen(hwmon_max_freq), rc);
          QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_minfreq_path, hwmon_max_freq, rc);
       }
       else {
          FWRITE_STR(d.cpubw_minfreq_path, tmp_s, strlen(tmp_s), rc);
          QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_minfreq_path, tmp_s, rc);
       }
   }

   return rc;
}

int OptsHandler::cpubw_hwmon_max_freq(Resource &r,  OptsData &d) {
    int i = 0;
    int rc = FAILED;
    int setval = FAILED;
    unsigned int reqval = r.value;
    static unsigned int stored_cpubw_hwmon_max_freq = 0;
    char tmp_s[NODE_MAX];
    char hwmon_min_freq[NODE_MAX];

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_maxf_sl > 0) {
            FWRITE_STR(d.cpubw_maxfreq_path, d.cpubw_maxf_s, d.cpubw_maxf_sl, rc);
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_maxfreq_path, d.cpubw_maxf_s, rc);
            stored_cpubw_hwmon_max_freq = 0;
        }
        return rc;
    }

    if (reqval > 0) {
        reqval *= 100;
        for (i = 0; i < d.cpubw_avail_freqs_n; i++) {
            if (d.cpubw_avail_freqs[i] >= reqval) {
                setval = d.cpubw_avail_freqs[i];
                break;
            }
        }

        if (i == d.cpubw_avail_freqs_n) {
            setval = d.cpubw_avail_freqs[i - 1];
        }
    }

    if (!stored_cpubw_hwmon_max_freq) {
        FREAD_STR(d.cpubw_maxfreq_path, d.cpubw_maxf_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_maxfreq_path, d.cpubw_maxf_s, rc);
            d.cpubw_maxf_sl = strlen(d.cpubw_maxf_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_maxfreq_path);
            return FAILED;
        }
        stored_cpubw_hwmon_max_freq = 1;
    }

    if (setval == FAILED) {
        QLOGE("Error! Perflock failed, invalid freq value %d", setval);
        return FAILED;
    }

    snprintf(tmp_s, NODE_MAX, "%d", setval);

   /* read hwmon min_freq  */
    FREAD_STR(d.cpubw_minfreq_path, hwmon_min_freq, NODE_MAX, rc);

    /* min_freq cannot be more than intended value of
     * max_freq, so min_freq maintained same as max_freq
     */
    if (rc > 0) {
       if (atoi(hwmon_min_freq) > atoi(tmp_s)) {
          FWRITE_STR(d.cpubw_minfreq_path, tmp_s, strlen(tmp_s), rc);
          QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_minfreq_path, tmp_s, rc);
       }
    }

    FWRITE_STR(d.cpubw_maxfreq_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_maxfreq_path, tmp_s, rc);
    return rc;
}

int OptsHandler::cpubw_hwmon_polling_interval(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_poll_int_sl > 0) {
            FWRITE_STR(d.cpubw_polling_interval_path, d.cpubw_poll_int_s, d.cpubw_poll_int_sl, rc);
            d.cpubw_stored = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_polling_interval_path, d.cpubw_poll_int_s, rc);
        }
        return rc;
    }

    if (!d.cpubw_stored) {
        FREAD_STR(d.cpubw_polling_interval_path, d.cpubw_poll_int_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_polling_interval_path, d.cpubw_poll_int_s, rc);
            d.cpubw_poll_int_sl = strlen(d.cpubw_poll_int_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_polling_interval_path);
            return FAILED;
        }
        d.cpubw_stored = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.cpubw_polling_interval_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_polling_interval_path, tmp_s, rc);
    return rc;
}

int OptsHandler::cpubw_hwmon_decay_rate(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_dr_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_decay_rate_path, d.cpubw_dr_s, d.cpubw_dr_sl, rc);
            d.cpubw_stored = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_hwmon_decay_rate_path, d.cpubw_dr_s, rc);
        }
        return rc;
    }

    if (!d.cpubw_stored) {
        FREAD_STR(d.cpubw_hwmon_decay_rate_path, d.cpubw_dr_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_hwmon_decay_rate_path, d.cpubw_dr_s, rc);
            d.cpubw_dr_sl = strlen(d.cpubw_dr_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_decay_rate_path);
            return FAILED;
        }
        d.cpubw_stored = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.cpubw_hwmon_decay_rate_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_hwmon_decay_rate_path, tmp_s, rc);
    return rc;
}

int OptsHandler::cpubw_hwmon_io_percent(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_ip_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_io_percent_path, d.cpubw_ip_s, d.cpubw_ip_sl, rc);
            stored = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_hwmon_io_percent_path, d.cpubw_ip_s, rc);
        }
        return rc;
    }

    if (!stored) {
        FREAD_STR(d.cpubw_hwmon_io_percent_path, d.cpubw_ip_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_hwmon_io_percent_path, d.cpubw_ip_s, rc);
            d.cpubw_ip_sl = strlen(d.cpubw_ip_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_io_percent_path);
            return FAILED;
        }
        stored = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.cpubw_hwmon_io_percent_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_hwmon_io_percent_path, tmp_s, rc);
    return rc;
}

int OptsHandler::cpubw_hwmon_hyst_opt(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_hyst_opt = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_hc_sl > 0) {
           FWRITE_STR(d.cpubw_hwmon_hyst_count_path, d.cpubw_hc_s, d.cpubw_hc_sl, rc);
           if (rc < 0) {
               QLOGE("Failed to write %s", d.cpubw_hwmon_hyst_count_path);
               return FAILED;
            }
        }
        if (d.cpubw_hm_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_hist_memory_path, d.cpubw_hm_s, d.cpubw_hm_sl, rc);
            if (rc < 0) {
               QLOGE("Failed to write %s", d.cpubw_hwmon_hist_memory_path);
               return FAILED;
            }
        }
        if (d.cpubw_hl_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_hyst_length_path, d.cpubw_hl_s, d.cpubw_hl_sl, rc);
            if (rc < 0) {
               QLOGE("Failed to write %s", d.cpubw_hwmon_hyst_length_path);
               return FAILED;
            }
        }
        if (rc >= 0)
           stored_hyst_opt = 0;
        return rc;
    }

    if (!stored_hyst_opt) {
        FREAD_STR(d.cpubw_hwmon_hyst_count_path, d.cpubw_hc_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hc_sl = strlen(d.cpubw_hc_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_hyst_count_path);
            return FAILED;
        }
        FREAD_STR(d.cpubw_hwmon_hist_memory_path, d.cpubw_hm_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hm_sl = strlen(d.cpubw_hm_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_hist_memory_path);
            return FAILED;
        }
        FREAD_STR(d.cpubw_hwmon_hyst_length_path, d.cpubw_hl_s, NODE_MAX, rc);
        if (rc >= 0) {
            d.cpubw_hl_sl = strlen(d.cpubw_hl_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_hyst_length_path);
            return FAILED;
        }
        stored_hyst_opt = 1;
    }

    snprintf(tmp_s, NODE_MAX, "0");
    FWRITE_STR(d.cpubw_hwmon_hyst_count_path, tmp_s, strlen(tmp_s), rc);
    FWRITE_STR(d.cpubw_hwmon_hist_memory_path, tmp_s, strlen(tmp_s), rc);
    FWRITE_STR(d.cpubw_hwmon_hyst_length_path, tmp_s, strlen(tmp_s), rc);
    return rc;
}

int OptsHandler::cpubw_hwmon_low_power_ceil_mbps(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_lpcm = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_lpcm_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_lpcm_path, d.cpubw_lpcm_s, d.cpubw_lpcm_sl, rc);
            stored_lpcm = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_hwmon_lpcm_path, d.cpubw_lpcm_s, rc);
        }
        return rc;
    }

    if (!stored_lpcm) {
        FREAD_STR(d.cpubw_hwmon_lpcm_path, d.cpubw_lpcm_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_hwmon_lpcm_path, d.cpubw_lpcm_s, rc);
            d.cpubw_lpcm_sl = strlen(d.cpubw_lpcm_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_lpcm_path);
            return FAILED;
        }
        stored_lpcm = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.cpubw_hwmon_lpcm_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_hwmon_lpcm_path, tmp_s, rc);
    return rc;
}

int OptsHandler::cpubw_hwmon_low_power_io_percent(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_lpip = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_lpip_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_lpip_path, d.cpubw_lpip_s, d.cpubw_lpip_sl, rc);
            stored_lpip = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_hwmon_lpip_path, d.cpubw_lpip_s, rc);
        }
        return rc;
    }

    if (!stored_lpip) {
        FREAD_STR(d.cpubw_hwmon_lpip_path, d.cpubw_lpip_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_hwmon_lpip_path, d.cpubw_lpip_s, rc);
            d.cpubw_lpip_sl = strlen(d.cpubw_lpip_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_lpip_path);
            return FAILED;
        }
        stored_lpip = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.cpubw_hwmon_lpip_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_hwmon_lpip_path, tmp_s, rc);
    return rc;
}

int OptsHandler::cpubw_hwmon_sample_ms(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sm = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_sm_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_sample_ms_path, d.cpubw_sm_s, d.cpubw_sm_sl, rc);
            stored_sm = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_hwmon_sample_ms_path, d.cpubw_sm_s, rc);
        }
        return rc;
    }

    if (!stored_sm) {
        FREAD_STR(d.cpubw_hwmon_sample_ms_path, d.cpubw_sm_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_hwmon_sample_ms_path, d.cpubw_sm_s, rc);
            d.cpubw_sm_sl = strlen(d.cpubw_sm_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_sample_ms_path);
            return FAILED;
        }
        stored_sm = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.cpubw_hwmon_sample_ms_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_hwmon_sample_ms_path, tmp_s, rc);
    return rc;
}

int OptsHandler::cpubw_hwmon_idle_mbps(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_imbps = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.cpubw_imbps_sl > 0) {
            FWRITE_STR(d.cpubw_hwmon_idle_mbps_path, d.cpubw_imbps_s, d.cpubw_imbps_sl, rc);
            stored_imbps = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.cpubw_hwmon_idle_mbps_path, d.cpubw_imbps_s, rc);
        }
        return rc;
    }

    if (!stored_imbps) {
        FREAD_STR(d.cpubw_hwmon_idle_mbps_path, d.cpubw_imbps_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.cpubw_hwmon_idle_mbps_path, d.cpubw_imbps_s, rc);
            d.cpubw_imbps_sl = strlen(d.cpubw_imbps_s);
        } else {
            QLOGE("Failed to read %s", d.cpubw_hwmon_idle_mbps_path);
            return FAILED;
        }
        stored_imbps = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.cpubw_hwmon_idle_mbps_path, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.cpubw_hwmon_idle_mbps_path, tmp_s, rc);
    return rc;
}

/* Function to handle single layer display hint
 * state = 0, is not single layer
 * state = 1, is single layer
 */
int OptsHandler::handle_disp_hint(Resource &r, OptsData &d) {
    unsigned int reqval = r.value;
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    d.mHintData.disp_single_layer = reqval;
    QLOGI("Display sent layer=%d", d.mHintData.disp_single_layer);

    /* Video is started, but display sends multiple
     * layer hint, so rearm timer, handled as
     * condition2 in slvp callback
     */
    if (d.mHintData.slvp_perflock_set == 1 && d.mHintData.disp_single_layer != 1) {
        QLOGI("Display to rearm timer,layer=%d",d.mHintData.disp_single_layer);

        /* rearm timer here, release handle in SLVP callback */
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

/* Function to receive video playback hint
 * state = 0, video stopped
 * state = 1, single instance of video
 */
int OptsHandler::handle_vid_decplay_hint(Resource &r, OptsData &d) {
    unsigned int reqval = r.value;
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    d.mHintData.vid_hint_state = reqval;

    /* timer is created only once here on getting video hint */
    if (d.mHintData.vid_hint_state == 1 && !d.mHintData.timer_created) {
        QLOGI("Video sent hint, create timer");
        return vid_create_timer(&d.mHintData);
    } else {
        /* only rearm here, handle conditions in SLVP callback */
        QLOGI("Video rearm timer");
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

/* Function to recieve video encode hint
 * for WFD use case.
 */
int OptsHandler::handle_vid_encplay_hint(Resource &r, OptsData &d) {
    unsigned int reqval = r.value;
    int rc = FAILED;;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    /* reqval - 1, encode start
     * reqval - 0, encode stop
     */
    d.mHintData.vid_enc_start = reqval;
    /* rearm timer if encode is atarted
     * handle condition4 in callback */
    if (d.mHintData.slvp_perflock_set == 1) {
        rearm_slvp_timer(&d.mHintData);
    }

    return 0;
}

int OptsHandler::disable_ksm(Resource &r, OptsData &d) {
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    return d.toggle_ksm_run(0);
}

int OptsHandler::enable_ksm(Resource &r, OptsData &d) {
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    return d.toggle_ksm_run(1);
}

int OptsHandler::set_ksm_param(Resource &r, OptsData &d) {
    int rc = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if(d.is_ksm_supported == 0)
    {
       char sleep_time[PROPERTY_VALUE_MAX];
       char scan_page[PROPERTY_VALUE_MAX];
       memset(sleep_time, 0, sizeof(sleep_time));
       memset(scan_page, 0, sizeof(scan_page));
       property_get("system.ksm_sleeptime", sleep_time, "20");
       property_get("system.ksm_scan_page", scan_page, "300");
       FWRITE_STR(d.ksm_param_sleeptime, sleep_time, strlen(sleep_time), rc);
       FWRITE_STR(d.ksm_param_pages_to_scan, scan_page, strlen(scan_page), rc);
    }
    return rc;
}

int OptsHandler::reset_ksm_param(Resource &r, OptsData &d) {
    int rc = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if(d.is_ksm_supported == 0)
    {
       FWRITE_STR(d.ksm_param_sleeptime, d.ksm_sleep_millisecs, strlen(d.ksm_sleep_millisecs), rc);
       FWRITE_STR(d.ksm_param_pages_to_scan, d.ksm_pages_to_scan, strlen(d.ksm_pages_to_scan), rc);
    }
    return rc;
}

/* Set swap_ratio */
int OptsHandler::swap_ratio(Resource &r,  OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_swap_ratio = 0;
    static char swap_ratio_s[NODE_MAX]; // swap ratio storage
    static int  swap_ratio_sl = -1; // swap ratio string length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (swap_ratio_sl > 0) {
            FWRITE_STR(SWAP_RATIO_NODE, swap_ratio_s, swap_ratio_sl, rc);
            stored_swap_ratio = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", SWAP_RATIO_NODE, swap_ratio_s, rc);
        }
        return rc;
    }

    if (!stored_swap_ratio) {
        FREAD_STR(SWAP_RATIO_NODE, swap_ratio_s, NODE_MAX, rc);
        if (rc > 0) {
            QLOGI("%s read with %s return value %d", SWAP_RATIO_NODE, swap_ratio_s, rc);
            swap_ratio_sl = strlen(swap_ratio_s);
            stored_swap_ratio = 1;
        } else {
            //better do not acquire lock as we can not release
            QLOGE("Failed to read %s ", SWAP_RATIO_NODE);
            return FAILED;
        }
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(SWAP_RATIO_NODE, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", SWAP_RATIO_NODE, tmp_s, rc);
    return rc;
}

/* Set the sampling rate */
int OptsHandler::sampling_rate(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_sampling_rate = 0;
    unsigned int reqval = r.value; // 0xff - (data & 0xff); //todo

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.sratel > 0) {
            FWRITE_STR(d.sr, d.srate, d.sratel, rc);
            stored_sampling_rate = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.sr, d.srate, rc);
        }
        return rc;
    }

    if (!stored_sampling_rate) {
        FREAD_STR(d.sr, d.srate, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.sr, d.srate, rc);
            d.sratel = strlen(d.srate);
        } else {
            QLOGE("Failed to read %s", d.sr);
            return FAILED;
        }
        stored_sampling_rate = 1;
    }
    reqval *= TIMER_MULTIPLICATION_FACTOR;

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.sr, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.sr, tmp_s, rc);
    return rc;
}

/* Set io_is_busy */
int OptsHandler::ondemand_io_is_busy(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ondiob[NODE_MAX];                  // ondemand io_is_busy storage
    static int  ondiobl = -1;                      // ondemand io_is_busy string length
    static unsigned int stored_ondemand_io_is_busy = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (ondiobl > 0) {
            FWRITE_STR(d.oiib, ondiob, ondiobl, rc);
            stored_ondemand_io_is_busy = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.oiib, ondiob, rc);
        }
        return rc;
    }

    if (!stored_ondemand_io_is_busy) {
        FREAD_STR(d.oiib, ondiob, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.oiib, ondiob, rc);
            ondiobl = strlen(ondiob);
        } else {
            QLOGE("Failed to read %s", d.oiib);
            return FAILED;
        }
        stored_ondemand_io_is_busy = 1;
    }

    reqval = !!reqval; /* 0 or 1 */

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.oiib, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.oiib, tmp_s, rc);
    return rc;
}

/* Set sampling_down_factor */
int OptsHandler::ondemand_sampling_down_factor(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static char ondsdf[NODE_MAX];                  // ondemand sampling_down_factor storage
    static int  ondsdfl = -1;                      // ondemand sampling_down_factor string length
    static unsigned int stored_ondemand_sampling_down_factor = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (ondsdfl > 0) {
            FWRITE_STR(d.osdf, ondsdf, ondsdfl, rc);
            stored_ondemand_sampling_down_factor = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.osdf, ondsdf, rc);
        }
        return rc;
    }

    if (!stored_ondemand_sampling_down_factor) {
        FREAD_STR(d.osdf, ondsdf, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.osdf, ondsdf, rc);
            ondsdfl = strlen(ondsdf);
        } else {
            QLOGE("Failed to read %s", d.osdf);
            return FAILED;
        }
        stored_ondemand_sampling_down_factor = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.osdf, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.osdf, tmp_s, rc);
    return rc;
}

int OptsHandler::sync_freq(Resource &r, OptsData &d) {
    int rc = 0;
    int i = 0;
    char tmp_s[NODE_MAX];
    static char sf[NODE_MAX];                      // ondemand sync_freq storage
    static int  sfl = -1;                          // ondemand sync_freq string length
    unsigned int reqval = r.value;
    static unsigned int stored_sync_freq = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (sfl > 0) {
            FWRITE_STR(d.sfreq, sf, sfl, rc);
            stored_sync_freq = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.sfreq, sf, rc);
        }
        return rc;
    }

    //TODO Change this to find_next_cpu_frequency
    if (reqval > 0) {
        reqval *= FREQ_MULTIPLICATION_FACTOR;
        for (i = 0; i < d.c0fL_n; i++) {
            if (d.c0fL[i] >= reqval) {
                reqval = d.c0fL[i];
                break;
            }
        }
        if (i == d.c0fL_n)
            reqval = d.c0fL[i - 1];
    }

    if (!stored_sync_freq) {
        FREAD_STR(d.sfreq, sf, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.sfreq, sf, rc);
            sfl = strlen(sf);
        } else {
            QLOGE("Failed to read %s", d.sfreq);
            return FAILED;
        }
        stored_sync_freq = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.sfreq, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.sfreq, tmp_s, rc);
    return rc;
}

int OptsHandler::optimal_freq(Resource &r, OptsData &d) {
    int rc = 0;
    int i = 0;
    char tmp_s[NODE_MAX];
    static char of[NODE_MAX];                      // ondemand optimal_freq storage
    static int  ofl = -1;                          // ondemand optimal_freq string length
    unsigned int reqval = r.value;
    static unsigned int stored_optimal_freq = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (ofl > 0) {
            FWRITE_STR(d.ofreq, of, ofl, rc);
            stored_optimal_freq = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.ofreq, of, rc);
        }
        return rc;
    }

    //TODO change this to find_next_cpu_frequency
    if (reqval > 0) {
        reqval *= FREQ_MULTIPLICATION_FACTOR;
        for (i = 0; i < d.c0fL_n; i++) {
            if (d.c0fL[i] >= reqval) {
                reqval = d.c0fL[i];
                break;
            }
        }
        if (i == d.c0fL_n)
            reqval = d.c0fL[i - 1];
    }

    if (!stored_optimal_freq) {
        FREAD_STR(d.ofreq, of, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.ofreq, of, rc);
            ofl = strlen(of);
        } else {
            QLOGE("Failed to read %s", d.ofreq);
            return FAILED;
        }
        stored_optimal_freq = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.ofreq, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.ofreq, tmp_s, rc);
    return rc;
}

int OptsHandler::toggle_step_up(Resource &r, OptsData &d) {
    int rc = FAILED;
    unsigned int reqval = r.value;
    static char enbl_stp_up_s[NODE_MAX];                  // enable_stepupnode storage
    static int  enbl_stp_up_l = -1;                      // enable_stepupnode string length
    static unsigned int stored_step_up = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL && stored_step_up) {
        if (enbl_stp_up_l > 0) {
            FWRITE_STR(d.enbl_stp_up, enbl_stp_up_s, enbl_stp_up_l, rc);
            stored_step_up = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.enbl_stp_up, enbl_stp_up_s, rc);
        }
        return rc;
    }
    if (!stored_step_up) {
        FREAD_STR(d.enbl_stp_up, enbl_stp_up_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.enbl_stp_up, enbl_stp_up_s, rc);
            enbl_stp_up_l = strlen(enbl_stp_up_s);
        } else {
            QLOGE("Failed to read %s", d.enbl_stp_up);
            return FAILED;
        }
        stored_step_up = 1;
    }

    if (reqval) {
        FWRITE_STR(d.enbl_stp_up, "1", 1, rc);
        QLOGI("perf_lock_acq: updated %s with %s return value %d", d.enbl_stp_up, "1", rc);
    }
    else {
        FWRITE_STR(d.enbl_stp_up, "0", 1, rc);
        QLOGI("perf_lock_acq: updated %s with %s return value %d", d.enbl_stp_up, "0", rc);
    }

    return rc;
}

int OptsHandler::write_max_intr_steps(Resource &r, OptsData &d) {
    int rc = FAILED;
    char tmp_s[NODE_MAX];
    static char max_int_steps_s[NODE_MAX];                  // max_int_steps storage
    static int  max_int_steps_l = -1;                      // max_int_steps string length
    unsigned int reqval = r.value;
    static unsigned int stored_max_intr_steps = 0;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL && stored_max_intr_steps) {
        if (max_int_steps_l > 0) {
            FWRITE_STR(d.max_int_steps, max_int_steps_s, max_int_steps_l, rc);
            stored_max_intr_steps = 0;
            QLOGI("perf_lock_rel: updated %s with %s return value %d", d.max_int_steps, max_int_steps_s, rc);
        }
        return rc;
    }

    if (!stored_max_intr_steps) {
        FREAD_STR(d.max_int_steps, max_int_steps_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", d.max_int_steps, max_int_steps_s, rc);
            max_int_steps_l = strlen(max_int_steps_s);
        } else {
            QLOGE("Failed to read %s", d.max_int_steps);
            return FAILED;
        }
        stored_max_intr_steps = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(d.max_int_steps, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updated %s with %s return value %d", d.max_int_steps, tmp_s, rc);
    return rc;
}

int OptsHandler::toggle_thread_migration_sync(Resource &r, OptsData &d) {
    int rc = FAILED;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == 1) {
        FWRITE_STR(d.sctm, "1", 1, rc);
        QLOGI("Updating %s with %s, return value %d", d.sctm, "1", rc);
    } else {
        FWRITE_STR(d.sctm, "0", 1, rc);
        QLOGI("Updating %s with %s, return value %d", d.sctm, "0", rc);
    }

    return rc;
}

int OptsHandler::lock_min_cores(Resource &r,  OptsData &d) {
#ifdef PERFD
    Target &t = Target::getCurTarget();
    if (d.core_ctl_present && t.isResourceSupported(r.major, r.minor)) {
        int rc = -1, coresInCluster;
        char tmp_s[NODE_MAX];
        unsigned int reqval = r.value;

        rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
        if (rc == FAILED) {
            QLOGE("Request on invalid core or cluster");
            return FAILED;
        }

        //TODO confirm this check implementation
        if (((t.getCoreCtlCpu() == 0) && (r.cluster != 0)) ||
            ((t.getCoreCtlCpu() == t.getLastCoreIndex(0)+1) && (r.cluster != 1))) {
            QLOGE("Warning: Core control support not present for lock_min_cores on cluster %d for this target", r.cluster);
            return FAILED;
        }

        if (reqval == MAX_LVL) {
            d.min_cores = atoi(d.core_ctl_min_cpu);
            FWRITE_STR(d.core_ctl_min_cpu_node, d.core_ctl_min_cpu, strlen(d.core_ctl_min_cpu), rc);
            QLOGI("perf_lock_rel: updating %s with %s", d.core_ctl_min_cpu_node, d.core_ctl_min_cpu);
            return rc;
        }

        d.min_cores = reqval;
        coresInCluster = t.getCoresInCluster(r.cluster);
        if ((coresInCluster >= 0) && (d.min_cores > coresInCluster)) {
            d.min_cores = coresInCluster;
        }

        if (d.min_cores > d.max_cores) {
            d.min_cores = d.max_cores;
        }

        snprintf(tmp_s, NODE_MAX, "%d", d.min_cores);
        FWRITE_STR(d.core_ctl_min_cpu_node, tmp_s, strlen(tmp_s), rc);
        QLOGI("perf_lock_acq: updating %s with %s", d.core_ctl_min_cpu_node, tmp_s);
        return rc;
    }
    else {
       QLOGI("lock_min_cores perflock is not supported");
       return FAILED;
    }
#else
#if 0
    if (reqval == 0x77 || reqval == RELEASE_LOCK) {
        min_cores = 0;
        return mpdecision_lock_min_cores(min_cores);
    }

    if (reqval < 2) {
        return FAILED;
    }

    min_cores = reqval;

    if (min_cores > nocs()) {
        min_cores = nocs();
    }

    if (min_cores > max_cores) {
        min_cores = max_cores;
    }

    return mpdecision_lock_min_cores(min_cores);
#endif
#endif
    return 0;
}

int OptsHandler::lock_max_cores(Resource &r,  OptsData &d) {
#ifdef PERFD
    char tmp_s[NODE_MAX];
    Target &t = Target::getCurTarget();
    unsigned int reqval = r.value;
    int rc = FAILED;

    rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (d.core_ctl_present > 0) {

       //TODO confirm this check implementation
       if (((t.getCoreCtlCpu() == 0) && (r.cluster != 0)) ||
           ((t.getCoreCtlCpu() == t.getLastCoreIndex(0)+1) && (r.cluster != 1))) {
           QLOGE("Warning: Core control support not present for lock_max_cores on cluster %d for this target", r.cluster);
           return FAILED;
       }

       if (reqval == MAX_LVL) {
           /* Update max_core first otherwise \
              min_core wiil not update */
           d.max_cores = atoi(d.core_ctl_max_cpu);
           FWRITE_STR(d.core_ctl_max_cpu_node, d.core_ctl_max_cpu, strlen(d.core_ctl_max_cpu), rc);
           QLOGI("perf_lock_rel: updating %s with %s ", d.core_ctl_max_cpu_node, d.core_ctl_max_cpu);
           if (d.min_cores > 0) {
               snprintf(tmp_s, NODE_MAX, "%d", d.min_cores);
               FWRITE_STR(d.core_ctl_min_cpu_node, tmp_s, strlen(tmp_s), rc);
               QLOGI("perf_lock_rel: updating %s with %s ", d.core_ctl_min_cpu_node, tmp_s);
           }
           return rc;
       }

       if (((int)reqval < t.getMinCoreOnline()) || ((int)reqval > t.getCoresInCluster(r.cluster))) {
           QLOGE("Error: perf-lock failed, invalid no. of cores requested to be online");
           return FAILED;
       }

       d.max_cores = reqval;
       snprintf(tmp_s, NODE_MAX, "%d", d.max_cores);
       FWRITE_STR(d.core_ctl_max_cpu_node, tmp_s, strlen(tmp_s), rc);
       QLOGI("perf_lock_acq: updating %s with %s ", d.core_ctl_max_cpu_node, tmp_s);
       return rc;
    }
    else if (d.kpm_hotplug_support > 0) {
       if (t.getCoresInCluster(r.cluster) <= 0) {
           QLOGW("Warning: Cluster %d does not exist, resource is not supported", r.cluster);
           return FAILED;
       }

       if (reqval == MAX_LVL) {
           if (r.cluster == 0) {
               d.lock_max_clust0 = -1;
           } else if (r.cluster == 1) {
               d.lock_max_clust1 = -1;
           }
       } else {
           d.max_cores = reqval;

           if (d.max_cores > t.getCoresInCluster(r.cluster)) {
               QLOGE("Error! perf-lock failed, invalid no. of cores requested to be online");
               return FAILED;
           }

           if (r.cluster == 0) {
               d.lock_max_clust0 = d.max_cores;
           } else if (r.cluster == 1) {
               d.lock_max_clust1 = d.max_cores;
           }
       }

       snprintf(tmp_s, NODE_MAX, "%d:%d", d.lock_max_clust0, d.lock_max_clust1);
       QLOGE("Write %s into %s", tmp_s, KPM_MAX_CPUS);
       FWRITE_STR(KPM_MAX_CPUS, tmp_s, strlen(tmp_s), rc);
       return rc;
    }
    else
       return FAILED;
#else
#if 0
    int reqval = (opt_data & 0xf);

    if (reqval == RELEASE_LOCK) {
        if (min_cores > 0) {
            mpdecision_lock_min_cores(min_cores);
        }
        max_cores = 0xFF;
        return mpdecision_lock_max_cores(max_cores);
    }

    max_cores = 0xF-reqval;
    return mpdecision_lock_max_cores(max_cores);
#endif
#endif
    return 0;
}

/* Set gpu default pwrlevel */
int OptsHandler::gpu_default_pwrlvl(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_gpu_default_pwrlvl = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.gpu_default_pwrlvl_sl > 0) {
            FWRITE_STR(GPU_DEFAULT_PWRLVL, d.gpu_default_pwrlvl_s, d.gpu_default_pwrlvl_sl, rc);
            stored_gpu_default_pwrlvl = 0;
            QLOGI("perf_lock_rel: updating %s with %s, return value %d", GPU_DEFAULT_PWRLVL, d.gpu_default_pwrlvl_s, rc);
        }
        return rc;
    }

    if (!stored_gpu_default_pwrlvl) {
        FREAD_STR(GPU_DEFAULT_PWRLVL, d.gpu_default_pwrlvl_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", GPU_DEFAULT_PWRLVL, d.gpu_default_pwrlvl_s, rc);
            d.gpu_default_pwrlvl_sl = strlen(d.gpu_default_pwrlvl_s);
        } else {
            QLOGE("Failed to read %s", GPU_DEFAULT_PWRLVL);
            return FAILED;
        }
        stored_gpu_default_pwrlvl = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(GPU_DEFAULT_PWRLVL, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updating %s with %s, return value %d", GPU_DEFAULT_PWRLVL, tmp_s, rc);
    return rc;
}

/* Set gpu min pwrlevel */
int OptsHandler::gpu_min_pwrlvl(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_gpu_min_pwrlvl = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.gpu_min_pwrlvl_sl > 0) {
            FWRITE_STR(GPU_MIN_PWRLVL, d.gpu_min_pwrlvl_s, d.gpu_min_pwrlvl_sl, rc);
            stored_gpu_min_pwrlvl = 0;
            QLOGI("perf_lock_rel: updating %s with %s, return value %d", GPU_MIN_PWRLVL, d.gpu_min_pwrlvl_s, rc);
        }
        return rc;
    }

    if (!stored_gpu_min_pwrlvl) {
        FREAD_STR(GPU_MIN_PWRLVL, d.gpu_min_pwrlvl_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", GPU_MIN_PWRLVL, d.gpu_min_pwrlvl_s, rc);
            d.gpu_min_pwrlvl_sl = strlen(d.gpu_min_pwrlvl_s);
        } else {
            QLOGE("Failed to read %s", GPU_MIN_PWRLVL);
            return FAILED;
        }
        stored_gpu_min_pwrlvl = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(GPU_MIN_PWRLVL, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updating %s with %s, return value %d", GPU_MIN_PWRLVL, tmp_s, rc);
    return rc;
}

/* Set gpu max pwrlevel */
int OptsHandler::gpu_max_pwrlvl(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_gpu_max_pwrlvl = 0;
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.gpu_max_pwrlvl_sl > 0) {
            FWRITE_STR(GPU_MAX_PWRLVL, d.gpu_max_pwrlvl_s, d.gpu_max_pwrlvl_sl, rc);
            stored_gpu_max_pwrlvl = 0;
            QLOGI("perf_lock_rel: updating %s with %s, return value %d", GPU_MAX_PWRLVL, d.gpu_max_pwrlvl_s, rc);
        }
        return rc;
    }

    if (!stored_gpu_max_pwrlvl) {
        FREAD_STR(GPU_MAX_PWRLVL, d.gpu_max_pwrlvl_s, NODE_MAX, rc);
        if (rc >= 0) {
            QLOGI("%s read with %s return value %d", GPU_MAX_PWRLVL, d.gpu_max_pwrlvl_s, rc);
            d.gpu_max_pwrlvl_sl = strlen(d.gpu_max_pwrlvl_s);
        } else {
            QLOGE("Failed to read %s", GPU_MAX_PWRLVL);
            return FAILED;
        }
        stored_gpu_max_pwrlvl = 1;
    }

    snprintf(tmp_s, NODE_MAX, "%d", reqval);
    FWRITE_STR(GPU_MAX_PWRLVL, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updating %s with %s, return value %d", GPU_MAX_PWRLVL, tmp_s, rc);
    return rc;
}

/* Set irq_balancer */
int OptsHandler::irq_balancer(Resource &r, OptsData &d) {
    int rc = 0;
    int i = 0;
    int tmp_s[MAX_CPUS];
    unsigned int reqval = r.value;
    Target &target = Target::getCurTarget();

    rc = ValidateClusterAndCore(r.cluster, r.core, SYNC_CORE);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (d.irq_bal_sp) {
            send_irq_balance(d.irq_bal_sp, NULL);
            d.stored_irq_balancer = 0;
            free(d.irq_bal_sp);
            d.irq_bal_sp = NULL;
        }
        return rc;
    }

    if (reqval > 0) {
        for (i = 0; i < target.getTotalNumCores(); i++)
            tmp_s[i] = (reqval & (1 << i)) >> i;

        if (!d.stored_irq_balancer) {
            if (!d.irq_bal_sp) {
                d.irq_bal_sp = (int *) malloc(sizeof(*d.irq_bal_sp) * MAX_CPUS);
            }
            send_irq_balance(tmp_s, &d.irq_bal_sp);
            d.stored_irq_balancer = 1;
        } else {
            send_irq_balance(tmp_s, NULL);
        }
    }

    return rc;
}

/* Removes input boost during keypress scenarios */
int OptsHandler::input_boost_reset(Resource &r, OptsData &d) {
    int rc = 0;
    char *bptr;
    char tmp_s[NODE_MAX];
    static unsigned int stored_input_boost = 0;
    static char input_boost_reset_s[NODE_MAX];        // input boost reset storage
    static int  input_boost_reset_sl = -1;            // input boost reset length
    unsigned int reqval = r.value;

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    if (reqval == MAX_LVL) {
        if (input_boost_reset_sl > 0) {
            FWRITE_STR(INPUT_BOOST_FREQ, input_boost_reset_s, input_boost_reset_sl, rc);
            stored_input_boost = 0;
            QLOGI("perf_lock_rel: updating %s with %s, return value %d", INPUT_BOOST_FREQ, input_boost_reset_s, rc);
        }
        return rc;
    }

    if (!stored_input_boost) {
        FREAD_STR(INPUT_BOOST_FREQ, input_boost_reset_s, NODE_MAX, rc);
        if (rc >= 0) {
            /* Ensure the string we are storing has no trailing whitespaces or newlines */
            bptr = input_boost_reset_s + strlen(input_boost_reset_s);
            while(bptr != input_boost_reset_s && (*(bptr - 1) == '\n' || *(bptr - 1) == ' ')) {
                --bptr;
            }
            *bptr = '\0';
            QLOGI("%s read with %s return value %d", INPUT_BOOST_FREQ, input_boost_reset_s, rc);
            input_boost_reset_sl = strlen(input_boost_reset_s);
        } else {
            QLOGE("Failed to read %s", INPUT_BOOST_FREQ);
            return FAILED;
        }
        stored_input_boost = 1;
    }

    snprintf(tmp_s, NODE_MAX, "0");
    FWRITE_STR(INPUT_BOOST_FREQ, tmp_s, strlen(tmp_s), rc);
    QLOGI("perf_lock_acq: updating %s with %s, return value %d", INPUT_BOOST_FREQ, tmp_s, rc);
    return rc;
}


/* Disable storage clock scaling */
int OptsHandler::storage_clk_scaling_disable(Resource &r, OptsData &d) {
    int rc = 0;
    char tmp_s[NODE_MAX];
    static unsigned int stored_clk_scaling = 0;
    static char clk_scaling_s[NODE_MAX];
    static int clk_scaling_sl = -1;
    unsigned int reqval = r.value;
    static char *mStorageNode = Target::getCurTarget().getStorageNode();

    rc = ValidateClusterAndCore(r.cluster, r.core, CORE_INDEPENDENT);
    if (rc == FAILED) {
        QLOGE("Request on invalid core or cluster");
        return FAILED;
    }

    rc = FAILED;
    if (reqval == MAX_LVL) {
        if (clk_scaling_sl > 0) {
            FWRITE_STR(mStorageNode, clk_scaling_s, clk_scaling_sl, rc);
            if (rc > 0) {
                QLOGE("perf_lock_rel: updated %s with %s return value %d",
                     mStorageNode, clk_scaling_s, rc);
            } else {
                QLOGI("perf_lock_rel: failed to update %s with %s return value %d",
                     mStorageNode, clk_scaling_s, rc);
            }
            stored_clk_scaling = 0;
        }
        return rc;
    }

    if (!stored_clk_scaling) {
       FREAD_STR(mStorageNode, clk_scaling_s, NODE_MAX, rc);
       if (rc > 0) {
           QLOGI("%s read with %s return value %d", mStorageNode,
                clk_scaling_s, rc);
           clk_scaling_sl = strlen(clk_scaling_s);
           stored_clk_scaling = 1;
       } else {
           QLOGE("Failed to read %s ", mStorageNode);
           return FAILED;
       }
    }

    if (reqval == 1)
        snprintf(tmp_s, NODE_MAX, "%d", 0);
    else
        snprintf(tmp_s, NODE_MAX, "%d", 1);
    FWRITE_STR(mStorageNode, tmp_s, strlen(tmp_s), rc);
    if (rc > 0) {
        QLOGI("perf_lock_acq: updated %s with %s return value %d", mStorageNode,
             tmp_s, rc);
    } else {
        QLOGI("perf_lock_acq: failed to update %s with %s return value %d", mStorageNode,
             tmp_s, rc);
    }
    return rc;
}

int OptsHandler::keep_alive(Resource &r, OptsData &d) {
    int rc = -1;
    tKillFlag = (r.value == 0) ? false : true;

    QLOGI("keep alive tKillFlag:%d\n", tKillFlag);
    pthread_mutex_lock(&ka_mutex);
    if (!isThreadAlive) {
        if(!tKillFlag) {
            rc = pthread_create(&id_ka_thread, NULL, keep_alive_thread, NULL);
            if (rc!=0)
            {
                QLOGE("Unable to create keepAlive thread, and error code is %d\n",rc);
            }
        }
    }
    pthread_mutex_unlock(&ka_mutex);
    return rc;
}

void* OptsHandler::keep_alive_thread(void*) {
    int len;
    int i;
    int tmp;
    char cmd[50];

    while (true) {
        pthread_mutex_lock(&ka_mutex);
        if (!tKillFlag) {
            isThreadAlive = true;
            pthread_mutex_unlock(&ka_mutex);
            len = sizeof (dnsIPs) / sizeof (*dnsIPs);
            i = rand()%len;
            snprintf(cmd, 30, "ping -c 1 %s",dnsIPs[i]);
            system(cmd);
            QLOGI("Hello KeepAlive~\n");
            sleep(5);
        } else {
            isThreadAlive = false;
            pthread_mutex_unlock(&ka_mutex);
            break;
        }
    }
    QLOGI("Keep Alive Thread has gone.~~~\n");
    pthread_exit(0);
    return NULL;
}

/* Return ADD_AND_UPDATE_REQUEST if reqVal is greater than curVal
 * Return PEND_REQUEST if reqVal is less or equal to curVal
 * */
int OptsHandler::higher_is_better(unsigned int reqLevel, unsigned int curLevel) {
    int ret;

    if (reqLevel > curLevel) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGI("higher is better called , returning %d", ret);
    return ret;
}

/* Return ADD_AND_UPDATE_REQUEST if reqVal is lower than curVal
 * Return PEND_REQUEST if reqVal is less or equal to curVal
 * */
int OptsHandler::lower_is_better(unsigned int reqLevel, unsigned int curLevel) {
    int ret;

    if (reqLevel < curLevel) {
        ret = ADD_AND_UPDATE_REQUEST;
    } else {
        ret = PEND_REQUEST;
    }

    QLOGI("lower_is_better called, returning %d", ret);
    return ret;
}

/* ALways return ADD_AND_UPDATE_REQUEST
 */
int OptsHandler::always_apply(unsigned int reqLevel, unsigned int curLevel) {

    QLOGI("always_apply called, returning %d", ADD_AND_UPDATE_REQUEST);
    return ADD_AND_UPDATE_REQUEST;
}
