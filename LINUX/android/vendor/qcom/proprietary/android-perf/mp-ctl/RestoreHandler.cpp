/******************************************************************************
  @file    RestoreHandler.cpp
  @brief   Implementation of nodes restoration

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

#define LOG_TAG "ANDR-PERF-RESETHANDLER"
#include <cutils/log.h>

#include "Request.h"

#define RESET_SCHED_BOOST   0

#include "OptsData.h"
#include "RestoreHandler.h"
#include "MpctlUtils.h"

extern struct cpu_freq_mapping cpu_freq_map[];

static struct {
    const char* node_name;
    const char* val_arr;
} reset_node_data [] = {
    {INTERACTIVE_HISPEED_FREQ_CORE, "ihf%u"},
    {INTERACTIVE_ABOVE_HISPEED_DELAY_CORE, "iahd%u"},
    {INTERACTIVE_TARGET_LOADS_CORE, "itl%u"},
    {INTERACTIVE_MIN_SAMPLE_TIME_CORE, "imst%u"},
    {INTERACTIVE_GO_HISPEED_LOAD_CORE, "ighl%u"},
    {INTERACTIVE_MAX_FREQ_HYSTERESIS_CORE, "imf%u"},
    {INTERACTIVE_TIMER_RATE_CORE, "itr%u"},
    {INTERACTIVE_TIMER_SLACK_CORE, "its%u"},
    {INTERACTIVE_IO_IS_BUSY_CORE, "iiib%u"},
    {INTERACTIVE_BOOST_CORE, "intb%u"},
    {INTERACTIVE_BOOSTPULSE_DURATION_CORE, "ibd%u"},
    {INTERACTIVE_USE_SCHED_LOAD_CORE, "iusl%u"},
    {INTERACTIVE_USE_MIGRATION_NOTIF_CORE, "iumn%u"},
    {INTERACTIVE_IGNORE_HISPEED_NOTIF_CORE, "iihn%u"}
};

ResetHandler ResetHandler::mResetHandler;

ResetHandler::ResetHandler() {
}

ResetHandler::~ResetHandler() {
}

int ResetHandler::Init() {
    return 0;
}

void ResetHandler::reset_to_default_values(OptsData &d) {
    FILE *defval;
    int j;
    int i, rc, cpu;
    char buf[NODE_MAX], val_str[NODE_MAX], buf2[NODE_MAX];
    Target &t = Target::getCurTarget();
    int clusterNum, startCpu, endCpu;

    defval = fopen("/data/system/perfd/default_values", "a+");
    if (defval == NULL) {
        QLOGE("Cannot open/create default values file");
        return;
    }

    fseek (defval, 0, SEEK_END);

    if (ftell(defval) == 0) {
        write_to_file(defval, "sr", d.sr);
        write_to_file(defval, "oiib", d.oiib);
        write_to_file(defval, "osdf", d.osdf);
        write_to_file(defval, "sfreq", d.sfreq);
        write_to_file(defval, "ofreq", d.ofreq);
        write_to_file(defval, "sctm", d.sctm);
        write_to_file(defval, "ksm_run_node", d.ksm_run_node);
        write_to_file(defval, "ksm_param_sleeptime", d.ksm_param_sleeptime);
        write_to_file(defval, "ksm_param_pages_to_scan", d.ksm_param_pages_to_scan);
        write_to_file(defval, "swrn", SWAP_RATIO_NODE);
        write_to_file(defval, "gpu_default_pwrlvl", GPU_DEFAULT_PWRLVL);
        write_to_file(defval, "gpu_min_pwrlvl", GPU_MIN_PWRLVL);
        write_to_file(defval, "gpu_max_pwrlvl", GPU_MAX_PWRLVL);
        write_to_file(defval, "sst", SCHED_SMALL_TASK);
        write_to_file(defval, "smcn", SCHED_MIGRATE_COST_NODE);
        write_to_file(defval, "sitl", SCHED_INIT_TASK_LOAD);
        write_to_file(defval, "sum", SCHED_UPMIGRATE);
        write_to_file(defval, "sdm", SCHED_DOWNMIGRATE);
        write_to_file(defval, "ssnr", SCHED_SPILL_NR_RUN_NODE);
        write_to_file(defval, "srcs", SCHED_RESTRICT_CLUSTER_SPILL_NODE);
        write_to_file(defval, "cbmf", d.cpubw_minfreq_path);
        write_to_file(defval, "cbmaxf", d.cpubw_maxfreq_path);
        write_to_file(defval, "cbpollintf", d.cpubw_polling_interval_path);
        write_to_file(defval, "cbhdr", d.cpubw_hwmon_decay_rate_path);
        write_to_file(defval, "cbhip", d.cpubw_hwmon_io_percent_path);
        write_to_file(defval, "cbsm", d.cpubw_hwmon_sample_ms_path);
        write_to_file(defval, "cbhm", d.cpubw_hwmon_hist_memory_path);
        write_to_file(defval, "cbhl", d.cpubw_hwmon_hyst_length_path);
        write_to_file(defval, "cbhc", d.cpubw_hwmon_hyst_count_path);
        write_to_file(defval, "cblpcm", d.cpubw_hwmon_lpcm_path);
        write_to_file(defval, "cbgbm", d.cpubw_hwmon_gbm_path);
        write_to_file(defval, "cbus", d.cpubw_hwmon_up_scale_path);
        write_to_file(defval, "cblpip", d.cpubw_hwmon_lpip_path);
        write_to_file(defval, "ecs", STORAGE_EMMC_CLK_SCALING_DISABLE);
        write_to_file(defval, "ucs", STORAGE_UFS_CLK_SCALING_DISABLE);
        write_to_file(defval, "cbimbps", d.cpubw_hwmon_idle_mbps_path);
        write_to_file(defval, "ta_cpuset", SCHED_TOP_APP_CPUSET);
        write_to_file(defval, "fg_cpuset", SCHED_FOREGROUND_CPUSET);
        write_to_file(defval, "sbg_cpuset", SCHED_SYSTEM_BACKGROUND_CPUSET);
        write_to_file(defval, "bg_cpuset", SCHED_BACKGROUND_CPUSET);
        write_to_file(defval, "sfa", SCHED_FREQ_AGGREGATE_NODE);
        write_to_file(defval, "etg", SCHED_ENABLE_THREAD_GROUPING);


        if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
            //Assuming that this code shall run for all the clusters once
            // with the first cpu in that cluster
            clusterNum = t.getNumCluster();
            startCpu = 0; //core 0 cluster 0
            for (i = 0; i < clusterNum; i++) {
                for (j =0; j < (int)ARRAY_SIZE(reset_node_data); ++j) {
                    if (i == 0)
                        cpu = get_online_core(0, t.getLastCoreIndex(0));
                    else
                        cpu = get_online_core(t.getLastCoreIndex(i-1)+1, t.getLastCoreIndex(i));
                    snprintf(buf, NODE_MAX, reset_node_data[j].node_name, cpu);
                    snprintf(val_str, NODE_MAX, reset_node_data[j].val_arr, i);
                    QLOGI("writing from node %s to default file as : %s", buf, val_str);
                    write_to_file(defval, val_str, buf);
                }
                startCpu = t.getLastCoreIndex(i)+1;
            }
        } else {
            write_to_file(defval, "iiib", d.iiib);
            write_to_file(defval, "itr", d.itr);
            write_to_file(defval, "intb", d.intb);
            write_to_file(defval, "ihf", d.ihf);
            write_to_file(defval, "iahd", d.iahd);
            write_to_file(defval, "ighl", d.ighl);
            write_to_file(defval, "itl", d.itl);
            write_to_file(defval, "imst", d.imst);
            write_to_file(defval, "imf", d.imf);
            write_to_file(defval, "its", d.its);
            write_to_file(defval, "ibd", d.ibd);
            write_to_file(defval, "iusl", d.iusl);
            write_to_file(defval, "iumn", d.iumn);
        }

        FREAD_STR(SCHED_MOSTLY_IDLE_NR_RUN, buf, NODE_MAX, rc);
        if (rc > 0) {
            write_to_file(defval, "sminr", SCHED_MOSTLY_IDLE_NR_RUN);
        } else {
            snprintf(buf, NODE_MAX, SCHED_MOSTLY_IDLE_NR_RUN_CORE, 0);
            write_to_file(defval, "sminr", buf);
        }
        FREAD_STR(SCHED_MOSTLY_IDLE_LOAD, buf, NODE_MAX, rc);
        if (rc > 0) {
            write_to_file(defval, "smil", SCHED_MOSTLY_IDLE_LOAD);
        } else {
            snprintf(buf, NODE_MAX, SCHED_MOSTLY_IDLE_LOAD_CORE, 0);
            write_to_file(defval, "smil", buf);
        }
        FREAD_STR(SCHED_PREFER_IDLE_NODE, buf, NODE_MAX, rc);
        if (rc > 0) {
            write_to_file(defval, "spi", SCHED_PREFER_IDLE_NODE);
        } else {
            snprintf(buf, NODE_MAX, SCHED_PREFER_IDLE_NODE_CORE, 0);
            write_to_file(defval, "spi", buf);
        }
        FREAD_STR(SCHED_MOSTLY_IDLE_FREQ, buf, NODE_MAX, rc);
        if (rc > 0) {
            write_to_file(defval, "smif", SCHED_MOSTLY_IDLE_FREQ);
        } else {
            snprintf(buf, NODE_MAX, SCHED_MOSTLY_IDLE_FREQ_CORE, 0);
            write_to_file(defval, "smif", buf);
        }

        for (i = 0; i < t.getTotalNumCores(); i++) {
            snprintf(buf, NODE_MAX, SCHED_STATIC_CPU_PWR_COST_CORE, i);
            snprintf(buf2, NODE_MAX, "sscpc%d", i);
            write_to_file(defval, buf2, buf);
        }

        fwrite("File created", sizeof(char), strlen("File created"), defval);
   } else {
        reset_freq_to_default(d);
#ifdef PERFD
        reset_cores_status(d);
#endif
        write_to_node(defval, "sr", d.sr);
        write_to_node(defval, "oiib", d.oiib);
        write_to_node(defval, "osdf", d.osdf);
        write_to_node(defval, "sfreq", d.sfreq);
        write_to_node(defval, "ofreq", d.ofreq);
        write_to_node(defval, "sctm", d.sctm);
        write_to_node(defval, "ksm_run_node", d.ksm_run_node);
        write_to_node(defval, "ksm_param_sleeptime", d.ksm_param_sleeptime);
        write_to_node(defval, "ksm_param_pages_to_scan", d.ksm_param_pages_to_scan);
        write_to_node(defval, "swrn", SWAP_RATIO_NODE);
        write_to_node(defval, "gpu_default_pwrlvl", GPU_DEFAULT_PWRLVL);
        write_to_node(defval, "gpu_min_pwrlvl", GPU_MIN_PWRLVL);
        write_to_node(defval, "gpu_max_pwrlvl", GPU_MAX_PWRLVL);
        write_to_node(defval, "sst", SCHED_SMALL_TASK);
        write_to_node(defval, "smcn", SCHED_MIGRATE_COST_NODE);
        write_to_node(defval, "sitl", SCHED_INIT_TASK_LOAD);
        write_to_node(defval, "sum", SCHED_UPMIGRATE);
        write_to_node(defval, "sdm", SCHED_DOWNMIGRATE);
        write_to_node(defval, "srcs", SCHED_RESTRICT_CLUSTER_SPILL_NODE);
        write_to_node(defval, "ssnr", SCHED_SPILL_NR_RUN_NODE);
        write_to_node(defval, "cbmf", d.cpubw_minfreq_path);
        write_to_node(defval, "cbmaxf", d.cpubw_maxfreq_path);
        write_to_node(defval, "cbpollintf", d.cpubw_polling_interval_path);
        write_to_node(defval, "cbhdr", d.cpubw_hwmon_decay_rate_path);
        write_to_node(defval, "cbhip", d.cpubw_hwmon_io_percent_path);
        write_to_node(defval, "cbsm", d.cpubw_hwmon_sample_ms_path);
        write_to_node(defval, "cbhm", d.cpubw_hwmon_hist_memory_path);
        write_to_node(defval, "cbhl", d.cpubw_hwmon_hyst_length_path);
        write_to_node(defval, "cbhc", d.cpubw_hwmon_hyst_count_path);
        write_to_node(defval, "cblpcm", d.cpubw_hwmon_lpcm_path);
        write_to_node(defval, "cbgbm", d.cpubw_hwmon_gbm_path);
        write_to_node(defval, "cbus", d.cpubw_hwmon_up_scale_path);
        write_to_node(defval, "cblpip", d.cpubw_hwmon_lpip_path);
        write_to_node(defval, "ecs", STORAGE_EMMC_CLK_SCALING_DISABLE);
        write_to_node(defval, "ucs", STORAGE_UFS_CLK_SCALING_DISABLE);
        write_to_node(defval, "cbimbps", d.cpubw_hwmon_idle_mbps_path);
        write_to_node(defval, "ta_cpuset", SCHED_TOP_APP_CPUSET);
        write_to_node(defval, "fg_cpuset", SCHED_FOREGROUND_CPUSET);
        write_to_node(defval, "sbg_cpuset", SCHED_SYSTEM_BACKGROUND_CPUSET);
        write_to_node(defval, "bg_cpuset", SCHED_BACKGROUND_CPUSET);
        write_to_node(defval, "sfa", SCHED_FREQ_AGGREGATE_NODE);
        write_to_node(defval, "etg", SCHED_ENABLE_THREAD_GROUPING);

        if (CLUSTER_BASED_GOV_INSTANCE == t.getGovInstanceType()) {
            clusterNum = t.getNumCluster();
            startCpu = 0, endCpu = -1;
            for (j =0; j < clusterNum ; j++) {
                endCpu = t.getLastCoreIndex(j);
                if ((i = get_online_core(startCpu, endCpu))!=FAILED){
                    reset_cpu_nodes(i);
                }
                //For next cluster startCpu
                startCpu = endCpu +1;
            }
        } else {
            write_to_node(defval, "iiib", d.iiib);
            write_to_node(defval, "itr", d.itr);
            write_to_node(defval, "intb", d.intb);
            write_to_node(defval, "ihf", d.ihf);
            write_to_node(defval, "iahd", d.iahd);
            write_to_node(defval, "ighl", d.ighl);
            write_to_node(defval, "itl", d.itl);
            write_to_node(defval, "imst", d.imst);
            write_to_node(defval, "imf", d.imf);
            write_to_node(defval, "its", d.its);
            write_to_node(defval, "ibd", d.ibd);
            write_to_node(defval, "iusl", d.iusl);
            write_to_node(defval, "iumn", d.iumn);
        }

        FREAD_STR(SCHED_PREFER_IDLE_NODE, buf, NODE_MAX, rc);
        if (rc > 0) {
             write_to_node(defval, "spi", SCHED_PREFER_IDLE_NODE);
        } else {
             for (i = 0; i < t.getTotalNumCores(); i++) {
                  snprintf(buf, NODE_MAX, SCHED_PREFER_IDLE_NODE_CORE, i);
                  write_to_node(defval, "spi", buf);
             }
        }

        FREAD_STR(SCHED_MOSTLY_IDLE_LOAD, buf, NODE_MAX, rc);
        if (rc > 0) {
             write_to_node(defval, "smil", SCHED_MOSTLY_IDLE_LOAD);
        } else {
             for (i = 0; i < t.getTotalNumCores(); i++) {
                  snprintf(buf, NODE_MAX, SCHED_MOSTLY_IDLE_LOAD_CORE, i);
                  write_to_node(defval, "smil", buf);
             }
        }

        FREAD_STR(SCHED_MOSTLY_IDLE_NR_RUN, buf, NODE_MAX, rc);
        if (rc > 0) {
             write_to_node(defval, "sminr", SCHED_MOSTLY_IDLE_NR_RUN);
        } else {
             for (i = 0; i < t.getTotalNumCores(); i++) {
                  snprintf(buf, NODE_MAX, SCHED_MOSTLY_IDLE_NR_RUN_CORE, i);
                  write_to_node(defval, "sminr", buf);
             }
        }

        FREAD_STR(SCHED_MOSTLY_IDLE_FREQ, buf, NODE_MAX, rc);
        if (rc > 0) {
             write_to_node(defval, "smif", SCHED_MOSTLY_IDLE_FREQ);
        } else {
             for (i = 0; i < t.getTotalNumCores(); i++) {
                  snprintf(buf, NODE_MAX, SCHED_MOSTLY_IDLE_FREQ_CORE, i);
                  write_to_node(defval, "smif", buf);
             }
        }

        for (i = 0; i < t.getTotalNumCores(); i++) {
            snprintf(buf, NODE_MAX, SCHED_STATIC_CPU_PWR_COST_CORE, i);
            snprintf(buf2, NODE_MAX, "sscpc%d", i);
            write_to_node(defval, buf2, buf);
        }

#ifdef PERFD
        reset_pc();
        reset_sched_boost(d);
#endif
    }
    fclose(defval);
}

void ResetHandler::reset_freq_to_default(OptsData &d) {
    int i = 0;
    int rc = -1;
    char maxf[NODE_MAX], minf[NODE_MAX];
    char freqnode[NODE_MAX];
    Target &t = Target::getCurTarget();
    int startcpu, endcpu;
#ifdef PERFD
    char tmp_s[NODE_MAX];
    int reqval = 0;
    rc = update_freq_node(0, t.getLastCoreIndex(0), reqval, tmp_s, NODE_MAX);
    if (rc >= 0) {
        QLOGI("reset_freq_to_default reset min freq req for CPUs 0-%d: %s", t.getLastCoreIndex(0), tmp_s);
        FWRITE_STR(KPM_CPU_MIN_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
    }
    rc = update_freq_node(0, t.getLastCoreIndex(0), UINT_MAX, tmp_s, NODE_MAX);
    if (rc >= 0) {
        QLOGI("reset_freq_to_default reset max freq req for CPUs 0-%d: %s", t.getLastCoreIndex(0), tmp_s);
        FWRITE_STR(KPM_CPU_MAX_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
    }
    if (t.getCoresInCluster(1) > 0) {
        rc = update_freq_node(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1), reqval, tmp_s, NODE_MAX);
        if (rc >= 0) {
            QLOGI("reset_freq_to_default reset min freq req for CPUs %d-%d: %s", t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1), tmp_s);
            FWRITE_STR(KPM_CPU_MIN_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
        }
        rc = update_freq_node(t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1), UINT_MAX, tmp_s, NODE_MAX);
        if (rc >= 0) {
            QLOGI("reset_freq_to_default reset max freq req for CPUs %d-%d: %s", t.getLastCoreIndex(0)+1, t.getLastCoreIndex(1), tmp_s);
            FWRITE_STR(KPM_CPU_MAX_FREQ_NODE, tmp_s, strlen(tmp_s), rc);
        }
    }
#else
    for ( i = 0; i < t.getTotalNumCores(); i++) {
        maxf[0] = '\0';
        minf[0] = '\0';
        //read the max_freq from cpuinfo node.
        snprintf(freqnode, NODE_MAX, CPUINFO_FREQ_NODE, i,"max");
        FREAD_STR(freqnode, maxf, NODE_MAX, rc);
        if (rc > 0) {
           maxf[rc-1] = '\0';
        } else {
           QLOGW("Could not read %s ", freqnode);
        }
        //read the min_freq from cpuinfo node or property
        if (i >= 0 && i <= t.getLastCoreIndex(0) ) {
           if (d.min_freq_prop_0_set) {
              snprintf(minf, NODE_MAX,"%d", d.min_freq_prop_0_val);
           } else {
              snprintf(freqnode, NODE_MAX, CPUINFO_FREQ_NODE, i,"min");
              FREAD_STR(freqnode, minf, NODE_MAX, rc);
              if (rc > 0) {
                 minf[rc-1] = '\0';
              } else {
                 QLOGW("Could not read %s ", freqnode);
              }
            }
        } else if ((i > t.getLastCoreIndex(0))&& (i <= t.getLastCoreIndex(1))){
           if (d.min_freq_prop_4_set) {
              snprintf(minf, NODE_MAX,"%d", d.min_freq_prop_4_val);
           } else {
              snprintf(freqnode, NODE_MAX, CPUINFO_FREQ_NODE, i,"min");
              FREAD_STR(freqnode, minf, NODE_MAX, rc);
              if(rc > 0) {
                 minf[rc-1] = '\0';
              } else {
                 QLOGW("Could not read %s ", freqnode);
              }
            }
        }
        //Update the max node first.
        if (strlen(maxf) > 0) {
           FWRITE_STR(cpu_freq_map[i].max, maxf, strlen(maxf), rc);
           if (rc <=0) {
              QLOGW("Initialization of %s with %s failed", cpu_freq_map[i].max, maxf);
           }
        } else {
            QLOGW("Initialization of %s failed as freq invalid ", cpu_freq_map[i].max);
        }
        if (strlen(minf) > 0) {
           FWRITE_STR(cpu_freq_map[i].min, minf, strlen(minf), rc);
           if (rc <=0) {
               QLOGW("Initialization of %s with %s failed", cpu_freq_map[i].min, minf);
           }
        } else {
            QLOGW("Initialization of %s failed as freq invalid ", cpu_freq_map[i].min);
        }
  }//end of for
#endif
}//end of function

void ResetHandler::reset_cores_status(OptsData &d) {
    char tmp_s[NODE_MAX];
    int rc = FAILED;
    Target &t = Target::getCurTarget();

    if (d.core_ctl_present > 0) {
        if (property_get("ro.core_ctl_max_cpu", tmp_s, NULL) > 0) {
            FWRITE_STR(d.core_ctl_max_cpu_node, tmp_s, strlen(tmp_s), rc);
            if (rc < 1) {
                QLOGE("Warning: core_ctl_max_cpu not updated, write failed");
            } else {
                QLOGI("Updating %s with %s and return value %d", d.core_ctl_max_cpu_node, tmp_s, rc);
            }
        } else {
            QLOGE("Warning: core_ctl_max_cpu property not defined, can not reset");
        }

        if (property_get("ro.core_ctl_min_cpu", tmp_s, NULL) > 0) {
            FWRITE_STR(d.core_ctl_min_cpu_node, tmp_s, strlen(tmp_s), rc);
            if (rc < 1) {
                QLOGE("Warning: core_ctl_min_cpu not updated, write failed");
            } else {
                QLOGI("Updating %s with %s and return value %d", d.core_ctl_min_cpu_node, tmp_s, rc);
            }
        } else {
            QLOGE("Warning: core_ctl_min_cpu property not defined, can not reset");
        }
    }
    else if (d.kpm_hotplug_support > 0){
        snprintf(tmp_s, NODE_MAX, "%d:%d", t.getCoresInCluster(0), t.getCoresInCluster(1));
        FWRITE_STR(KPM_MAX_CPUS, tmp_s, strlen(tmp_s), rc);
        if (rc > 0) {
            QLOGE("Reset cores success");
        } else {
            //QLOGE("Could not update the %s node \n", KPM_MANAGED_CPUS);
        }
    }
    else {
        QLOGE("Error: KPM hotplug and core control both are not present, can't reset");
    }
}

void ResetHandler::reset_pc() {
    char tmp_s[NODE_MAX];
    int rc = FAILED;
    snprintf(tmp_s, NODE_MAX, "%d", ENABLE_PC_LATENCY);
    FWRITE_STR(PM_QOS, tmp_s, strlen(tmp_s), rc);
}

void ResetHandler::reset_sched_boost(OptsData &d) {
    char tmp_s[NODE_MAX];
    int rc = FAILED;
    snprintf(tmp_s, NODE_MAX, "%d", RESET_SCHED_BOOST);
    FWRITE_STR(d.schedb_n, tmp_s, strlen(tmp_s), rc);
}

void ResetHandler::reset_cpu_nodes(int cpu) {
    FILE *defval;
    char buf[NODE_MAX];
    char val_str[NODE_MAX];
    int  rc, cluster;
    unsigned int i;
    int startcpu, endcpu;
    Target &t = Target::getCurTarget();

    defval = fopen("/data/system/perfd/default_values", "r");
    if(defval == NULL) {
        QLOGE("Cannot read default values file");
        return;
    }
    //TODO: confirm
    cluster = t.getClusterForCpu(cpu, startcpu, endcpu);
    if (cluster >= 0) {
        for (i = 0; i < ARRAY_SIZE(reset_node_data); ++i) {
            snprintf(buf, NODE_MAX, reset_node_data[i].node_name, cpu);
            snprintf(val_str, NODE_MAX, reset_node_data[i].val_arr, cluster);
            QLOGI("writing from %s to node: %s", val_str, buf);
            rc = write_to_node(defval, val_str, buf);
            if ((rc <= 0) && (SINGLE_GOV_INSTANCE != t.getGovInstanceType())) {
                signal_chk_poll_thread(buf, rc);
            }
        }
    }

    fclose(defval);
    QLOGI("CPU:%u Reset Nodes relevant to Profile Manager", cpu);
    return;
}


