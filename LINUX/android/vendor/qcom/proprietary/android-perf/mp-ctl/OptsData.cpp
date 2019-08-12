/******************************************************************************
  @file    OptsData.cpp
  @brief   Implementation of performance server module

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#include <cstdio>
#include <cstring>

#define LOG_TAG "ANDR-PERF-OPTSDATA"
#include <cutils/log.h>

#include <dirent.h>
#include "Request.h"

#include "OptsData.h"
#include "Target.h"
#include "MpctlUtils.h"

OptsData OptsData::mOptsData;

OptsData::OptsData() {
    cpubw_avail_freqs_n = 0;
    is_ksm_supported = -1;
    cpubw_mf_sl = -1;
    cpubw_dr_sl = -1;
    cpubw_stored = 0;
    cpubw_ip_sl = -1;
    gpu_default_pwrlvl_sl = -1;
    gpu_min_pwrlvl_sl = -1;
    gpu_max_pwrlvl_sl = -1;
    kpm_hotplug_support = -1;
    lock_max_clust0 = -1;
    lock_max_clust1 = -1;
    core_ctl_present = -1;
    min_cores = 0;
    max_cores = 0xFF;
    min_freq_prop_0_set = false;
    min_freq_prop_4_set = false;
    min_freq_prop_0_val = 0;
    min_freq_prop_4_val = 0;
    irq_bal_sp = NULL;
    stored_irq_balancer = 0;
    is_active = false;
}

OptsData::~OptsData() {
    if (mHintData.timer_created) {
        timer_delete(mHintData.tTimerId);
    }
}

int OptsData::Init() {
    int rc = 0;
    Target &t = Target::getCurTarget();

    init_cpubw_hwmon_path();

    parse_avail_freq_list();

    /*interactive init*/
    if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
        snprintf(iahd, NODE_MAX, INTERACTIVE_ABOVE_HISPEED_DELAY_CORE, 0);
        snprintf(intb, NODE_MAX, INTERACTIVE_BOOST_CORE, 0);
        snprintf(ibp, NODE_MAX, INTERACTIVE_BOOSTPULSE_CORE, 0);
        snprintf(ibd, NODE_MAX, INTERACTIVE_BOOSTPULSE_DURATION_CORE, 0);
        snprintf(ighl, NODE_MAX, INTERACTIVE_GO_HISPEED_LOAD_CORE, 0);
        snprintf(ihf, NODE_MAX, INTERACTIVE_HISPEED_FREQ_CORE, 0);
        snprintf(iiib, NODE_MAX, INTERACTIVE_IO_IS_BUSY_CORE, 0);
        snprintf(imst, NODE_MAX, INTERACTIVE_MIN_SAMPLE_TIME_CORE, 0);
        snprintf(itl, NODE_MAX, INTERACTIVE_TARGET_LOADS_CORE, 0);
        snprintf(itr, NODE_MAX, INTERACTIVE_TIMER_RATE_CORE, 0);
        snprintf(its, NODE_MAX, INTERACTIVE_TIMER_SLACK_CORE, 0);
        snprintf(imf, NODE_MAX, INTERACTIVE_MAX_FREQ_HYSTERESIS_CORE, 0);
        snprintf(iusl, NODE_MAX, INTERACTIVE_USE_SCHED_LOAD_CORE, 0);
        snprintf(iumn, NODE_MAX, INTERACTIVE_USE_MIGRATION_NOTIF_CORE, 0);
        snprintf(iihn, NODE_MAX, INTERACTIVE_IGNORE_HISPEED_NOTIF_CORE, 0);
    }
    else {
        snprintf(iahd, NODE_MAX, INTERACTIVE_ABOVE_HISPEED_DELAY_NODE);
        snprintf(intb, NODE_MAX, INTERACTIVE_BOOST_NODE);
        snprintf(ibp, NODE_MAX, INTERACTIVE_BOOSTPULSE_NODE);
        snprintf(ibd, NODE_MAX, INTERACTIVE_BOOSTPULSE_DURATION_NODE);
        snprintf(ighl, NODE_MAX, INTERACTIVE_GO_HISPEED_LOAD_NODE);
        snprintf(ihf, NODE_MAX, INTERACTIVE_HISPEED_FREQ_NODE);
        snprintf(iiib, NODE_MAX, INTERACTIVE_IO_IS_BUSY_NODE);
        snprintf(imst, NODE_MAX, INTERACTIVE_MIN_SAMPLE_TIME_NODE);
        snprintf(itl, NODE_MAX, INTERACTIVE_TARGET_LOADS_NODE);
        snprintf(itr, NODE_MAX, INTERACTIVE_TIMER_RATE_NODE);
        snprintf(its, NODE_MAX, INTERACTIVE_TIMER_SLACK_NODE);
        snprintf(imf, NODE_MAX, INTERACTIVE_MAX_FREQ_HYSTERESIS_NODE);
        snprintf(iusl, NODE_MAX, INTERACTIVE_USE_SCHED_LOAD_NODE);
        snprintf(iumn, NODE_MAX, INTERACTIVE_USE_MIGRATION_NOTIF_NODE);
        snprintf(iihn, NODE_MAX, INTERACTIVE_IGNORE_HISPEED_NOTIF_NODE);
    }

    /*ondemand init*/
    snprintf(sr, NODE_MAX, OND_SAMPLING_RATE_NODE);
    snprintf(oiib, NODE_MAX, OND_IO_IS_BUSY_NODE);
    snprintf(osdf, NODE_MAX, OND_SAMPLING_DOWN_FACTOR_NODE);
    snprintf(sfreq, NODE_MAX, OND_SYNC_FREQ_NODE);
    snprintf(ofreq, NODE_MAX, OND_OPTIMAL_FREQ_NODE);
    snprintf(enbl_stp_up, NODE_MAX, OND_ENABLE_STEPUP);
    snprintf(max_int_steps, NODE_MAX, OND_MAX_INT_STEPS);

    snprintf(sctm, NODE_MAX, NOTIFY_ON_MIGRATE);
    snprintf(schedb_n, NODE_MAX, SCHED_BOOST_NODE);

    //check core control presence
    check_core_ctl_presence();
    //initailize core control min/max cpu nodes
    rc = core_ctl_init();
    if (rc > 0) {
        QLOGI("Core control cpu nodes initialized successfully");
    } else {
        QLOGE("Error: core_ctl not present or min/max cpu nodes initialization failed");
    }

    //check for KPM hot plug support
    kpm_hotplug_support = kpm_support_for_hotplug();

    if (core_ctl_present == 1 && kpm_hotplug_support == 1) {
        QLOGE("Error: KPM hotplug support and core control both are present at the same time");
    }


    if(FAILED == setup_cpu_freq_values()) {
      QLOGE("Frequency initialization failed. Exit perflock server");
      return -1;
    }

    memset(&mHintData, 0x00, sizeof(mHintData));

    irq_bal_sp = NULL;
    stored_irq_balancer = 0;

    //check sched frequency aggregation support presence
    mSchedFreqAggrGroupData.checkSchedFreqAggrPresence();

    return 0;
}

int OptsData::init_available_cpubw_freq() {
   int rc = -1;
   char listf[FREQLIST_STR];
   char *cfL = NULL, *pcfL = NULL;
   char avlnode[NODE_MAX];

   snprintf(avlnode, NODE_MAX, "%s/available_frequencies", cpubw_path);

   FREAD_STR(avlnode, listf, FREQLIST_STR, rc);

   if (rc > 0) {
      listf[rc - 1] = '\0';
      QLOGI("Initializing available cpubw freq as %s", listf);
      cfL = strtok_r(listf, " ", &pcfL);
      if (cfL) {
         cpubw_avail_freqs[cpubw_avail_freqs_n++] = atoi(cfL);
         while ((cfL = strtok_r(NULL, " ", &pcfL)) != NULL) {
             cpubw_avail_freqs[cpubw_avail_freqs_n++] = atoi(cfL);
             if(cpubw_avail_freqs_n >= MAX_FREQ) {
                QLOGE("Number of cpubw frequency is more than the size of the array. Exiting");
                return FAILED;
             }
         }
     }
   } else {
     QLOGW("Initialization of available cpubw freq failed, as %s not present", avlnode);
   }
   return SUCCESS;
}

int OptsData::init_cpubw_hwmon_path() {
    DIR *devfreq_path;
    struct dirent *ep;
    int ret = FAILED;

    devfreq_path = opendir(DEVFREQ_PATH);
    if (!devfreq_path) {
        QLOGE("Could not find devfreq path for cpubw");
        return FAILED;
    }
    while ((ep = readdir (devfreq_path)) != NULL) {
        if (strstr(ep->d_name, "qcom,cpubw") != NULL) {
            snprintf(cpubw_path, NODE_MAX, "%s%s", DEVFREQ_PATH, ep->d_name);
            ret = SUCCESS;
            break;
        }
    }
    closedir(devfreq_path);

    if (SUCCESS == ret) {
        snprintf(cpubw_minfreq_path, NODE_MAX, "%s/min_freq", cpubw_path);
        snprintf(cpubw_maxfreq_path, NODE_MAX, "%s/max_freq", cpubw_path);
        snprintf(cpubw_polling_interval_path, NODE_MAX, "%s/polling_interval", cpubw_path);
        init_available_cpubw_freq();
        snprintf(cpubw_hwmon_decay_rate_path, NODE_MAX, "%s/bw_hwmon/decay_rate", cpubw_path);
        snprintf(cpubw_hwmon_io_percent_path, NODE_MAX, "%s/bw_hwmon/io_percent", cpubw_path);
        snprintf(cpubw_hwmon_sample_ms_path, NODE_MAX, "%s/bw_hwmon/sample_ms", cpubw_path);
        snprintf(cpubw_hwmon_hist_memory_path, NODE_MAX, "%s/bw_hwmon/hist_memory", cpubw_path);
        snprintf(cpubw_hwmon_hyst_length_path, NODE_MAX, "%s/bw_hwmon/hyst_length", cpubw_path);
        snprintf(cpubw_hwmon_hyst_count_path, NODE_MAX, "%s/bw_hwmon/hyst_trigger_count", cpubw_path);
        snprintf(cpubw_hwmon_lpcm_path, NODE_MAX, "%s/bw_hwmon/low_power_ceil_mbps", cpubw_path);
        snprintf(cpubw_hwmon_gbm_path, NODE_MAX, "%s/bw_hwmon/guard_band_mbps", cpubw_path);
        snprintf(cpubw_hwmon_up_scale_path, NODE_MAX, "%s/bw_hwmon/up_scale", cpubw_path);
        snprintf(cpubw_hwmon_lpip_path, NODE_MAX, "%s/bw_hwmon/low_power_io_percent", cpubw_path);
        snprintf(cpubw_hwmon_idle_mbps_path, NODE_MAX, "%s/bw_hwmon/idle_mbps", cpubw_path);
    }

    return ret;
}

int OptsData::init_ksm() {
    int rc = 0;

    snprintf(ksm_run_node, NODE_MAX, KSM_RUN_NODE);
    snprintf(ksm_param_sleeptime, NODE_MAX, KSM_SLEEP_MILLI_SECS_NODE);
    snprintf(ksm_param_pages_to_scan, NODE_MAX, KSM_PAGES_TO_SCAN_NODE);

    is_ksm_supported = access(ksm_run_node, F_OK);
    if(is_ksm_supported == 0)
    {
        memset(ksm_sleep_millisecs, 0, sizeof(ksm_sleep_millisecs));
        memset(ksm_pages_to_scan, 0, sizeof(ksm_pages_to_scan));
        FREAD_STR(ksm_param_sleeptime, ksm_sleep_millisecs, sizeof(ksm_sleep_millisecs), rc);
        FREAD_STR(ksm_param_pages_to_scan, ksm_pages_to_scan, sizeof(ksm_pages_to_scan), rc);
    }
    return is_ksm_supported;
}

/* toggle KSM
   0 -- KSM won't run
   1 -- KSM will run */
int OptsData::toggle_ksm_run(int run) {
    int rc=0;
    if(is_ksm_supported == 0)
    {
        if(run == 0)
        {
            /* Disable KSM */
            FWRITE_STR(ksm_run_node, "0", 1, rc);
        }
        else if(run == 1)
        {
            /* Enable KSM */
            FWRITE_STR(ksm_run_node, "1", 1, rc);
        }
    }
    return rc;
}

void OptsData::parse_avail_freq_list() {
    int rc = 0;
    char *cfL = NULL, *pcfL = NULL;

    /* Parse list of available frequencies */
    FREAD_STR(c0f, c0fL_s, FREQLIST_STR, rc);
    if (rc > 0) {
        c0fL_s[rc - 1] = '\0';
        cfL = strtok_r(c0fL_s, " ", &pcfL);
        if (cfL) {
            c0fL[c0fL_n++] = atoi(cfL);
            while ((cfL = strtok_r(NULL, " ", &pcfL)) != NULL) {
                c0fL[c0fL_n++] = atoi(cfL);
            }
        }
    }
    return;
}

int OptsData::kpm_support_for_hotplug() {
    int rc = 0;
    char kpm_max_cpus[NODE_MAX];

    //check for KPM hot plug support
    FREAD_STR(KPM_MAX_CPUS, kpm_max_cpus, NODE_MAX, rc);
    if (rc > 0) {
        kpm_hotplug_support = 1;
    } else
        kpm_hotplug_support = 0;

    return kpm_hotplug_support;
}


int OptsData::check_core_ctl_presence() {
    int rc = -1;
    Target &t = Target::getCurTarget();

    if (t.getCoreCtlCpu() == -1) {
        QLOGE("Error: core_ctl_cpu is not initialized for this target");
        return -1;
    } else {
        snprintf(core_ctl_min_cpu_node,NODE_MAX,CORE_CTL_MIN_CPU,t.getCoreCtlCpu());
        snprintf(core_ctl_max_cpu_node,NODE_MAX,CORE_CTL_MAX_CPU,t.getCoreCtlCpu());
        FREAD_STR(core_ctl_min_cpu_node, core_ctl_min_cpu, NODE_MAX, rc);
        if (rc > 0) {
            core_ctl_present = 1;
        } else {
            core_ctl_present = 0;
        }
    }
    return 1;
}

int OptsData::core_ctl_init() {
    int rc = -1;
    if(core_ctl_present > 0) {
       /* update min_cpu and max_cpus node */
       FREAD_STR(core_ctl_max_cpu_node, core_ctl_max_cpu, NODE_MAX, rc);
       if (rc > 0) {
           QLOGI("%s read with %s return value %d", core_ctl_max_cpu_node, core_ctl_max_cpu, rc);
           max_cores = atoi(core_ctl_max_cpu);
       } else {
           QLOGE("Unable to read %s", core_ctl_max_cpu_node);
       }

       FREAD_STR(core_ctl_min_cpu_node, core_ctl_min_cpu, NODE_MAX, rc);
       if (rc > 0) {
           QLOGI("%s read with %s return value %d", core_ctl_min_cpu_node, core_ctl_min_cpu, rc);
           min_cores = atoi(core_ctl_min_cpu);
       } else {
           QLOGE("Unable to read %s", core_ctl_min_cpu_node);
       }
       return rc;
    } else {
       QLOGE("Error: Core ctl not present");
       return -1;
    }
}

int OptsData::setup_cpu_freq_values()
{
  int i = 0;
  int rc;
  char min_freq_prop_0[PROPERTY_VALUE_MAX];
  char min_freq_prop_4[PROPERTY_VALUE_MAX];
  //TODO: this should be removed now.
  /* Check for min_freq_prop and if set read the min_freq*/
  if (property_get("ro.min_freq_0", min_freq_prop_0, NULL) > 0) {
     if (isdigit(min_freq_prop_0[0])) {
        min_freq_prop_0_val = atoi(min_freq_prop_0);
        min_freq_prop_0_set = true;
     }
  }

  if (property_get("ro.min_freq_4", min_freq_prop_4, NULL) > 0) {
     if (isdigit(min_freq_prop_4[0])) {
        min_freq_prop_4_val = atoi(min_freq_prop_4);
        min_freq_prop_4_set = true;
     }
  }

  for (i = 0; i < Target::getCurTarget().getTotalNumCores(); i++ )
      if(FAILED == init_available_freq(i))
         return FAILED;

  return SUCCESS;
}

int OptsData::get_reset_cpu_freq(int cpu, int ftype)
{
   if (cpu_freq_val[cpu].valid != 1) {
      QLOGE("Error: Perf-lock release for an un-initialized cpu %d", cpu);
      return FAILED;
   }
   else {
      if (ftype == MIN_FREQ_REQ)
         return cpu_freq_val[cpu].avl_freq[cpu_freq_val[cpu].min_freq_pos];
      else if (ftype == MAX_FREQ_REQ)
         return cpu_freq_val[cpu].avl_freq[cpu_freq_val[cpu].count-1];
   }
  return FAILED;
}

int OptsData::find_next_cpu_frequency(int cpureq, int freq) {
   int i, clust_id, clust_cpu_num, cluster = -1;
   int is_success = 0;
   Target &t = Target::getCurTarget();
   int startcpu, endcpu;
   int cpu;

   cluster = t.getClusterForCpu(cpureq, startcpu, endcpu);
   if ((cluster < 0) || (startcpu < 0) || (endcpu < 0)){
       QLOGE("Invalid first and last cpu in the cluster for cpu %d", cpureq);
       return FAILED;
   }

   for (cpu = startcpu; cpu <= endcpu; cpu++) {
        if (cpu_freq_val[cpu].valid != 1)
            if(FAILED == init_available_freq(cpu))
               continue;

        if (cpu_freq_val[cpu].valid != 1)
            continue;

        is_success = 1;
        break;
   }
   if (!is_success)
       return FAILED;

   for (i = cpu_freq_val[cpu].min_freq_pos; i < cpu_freq_val[cpu].count ; i++)
       if (cpu_freq_val[cpu].avl_freq[i] >= freq)
          return cpu_freq_val[cpu].avl_freq[i];

   if (i == cpu_freq_val[cpu].count)
      return cpu_freq_val[cpu].avl_freq[i-1];

   return FAILED;
}

int OptsData::init_available_freq(int cpu) {
   int rc = -1;
   char listf[FREQLIST_STR];
   char *cfL = NULL, *pcfL = NULL;
   char avlnode[NODE_MAX];

   if (cpu < 0 || cpu >= Target::getCurTarget().getTotalNumCores()) {
      QLOGE("Incorrect cpu%d" ,cpu);
      return 0;
   }

   snprintf(avlnode, NODE_MAX, AVL_FREQ_NODE, cpu);
   FREAD_STR(avlnode, listf, FREQLIST_STR, rc);
   if (rc > 0) {
      listf[rc - 1] = '\0';
      QLOGI("Initializing available freq for core %d as %s", cpu, listf);
      cfL = strtok_r(listf, " ", &pcfL);
      if (cfL) {
         cpu_freq_val[cpu].avl_freq[cpu_freq_val[cpu].count++] = atoi(cfL);
         while ((cfL = strtok_r(NULL, " ", &pcfL)) != NULL) {
               cpu_freq_val[cpu].avl_freq[cpu_freq_val[cpu].count++] = atoi(cfL);
               if(cpu_freq_val[cpu].count >= MAX_FREQ) {
                  QLOGE("Number of frequency is more than the size of the array.Exiting");
                  return FAILED;
               }
         }
         cpu_freq_val[cpu].valid = 1;
         cpu_freq_val[cpu].min_freq_pos = 0;
         check_min_freq_prop_set(cpu);
     }
   } else {
     QLOGW("Initialization of available freq for core %d failed, as %s not present", cpu, avlnode);
   }
   return SUCCESS;
}

void OptsData::check_min_freq_prop_set(int cpu) {
    Target &t = Target::getCurTarget();

    if ((cpu <= t.getLastCoreIndex(0)) && (min_freq_prop_0_set))
    {
        cpu_freq_val[cpu].min_freq_pos = find_frequency_pos(cpu, min_freq_prop_0_val);
        if (cpu_freq_val[cpu].min_freq_pos == -1) {
            QLOGW("Warning: min prop frequency not found setting it to default" );
            cpu_freq_val[cpu].min_freq_pos = 0;
        }
    }
    else if ((cpu > t.getLastCoreIndex(0)) && (cpu <= t.getLastCoreIndex(1)) && min_freq_prop_4_set)
    {
        cpu_freq_val[cpu].min_freq_pos = find_frequency_pos(cpu, min_freq_prop_4_val);
        if (cpu_freq_val[cpu].min_freq_pos == -1) {
            QLOGW("Warning: min prop frequency not found setting it to default" );
            cpu_freq_val[cpu].min_freq_pos = 0;
        }
    }
    return;
}


/*  Caller of the function should take care of the
 *  error condition when frequency is not found
 */
int OptsData::find_frequency_pos(int cpu, int freq) {
  int i;
  for (i = 0; i < cpu_freq_val[cpu].count; i++) {
      if (cpu_freq_val[cpu].avl_freq[i] == freq)
             return i;
  }
  return -1;
}


