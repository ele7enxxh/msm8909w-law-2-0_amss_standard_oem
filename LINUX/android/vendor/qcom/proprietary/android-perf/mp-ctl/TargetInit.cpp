/******************************************************************************
  @file    TargetInit.cpp
  @brief   Implementation of targets

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGET-INIT"
#include <cutils/log.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>

#include "Request.h"

#include "Target.h"
#include "OptsData.h"
#include "MpctlUtils.h"

#define KPM_NUM_CLUSTERS        "/sys/module/msm_performance/parameters/num_clusters"
#define KPM_MAX_CPUS            "/sys/module/msm_performance/parameters/max_cpus"
#define KPM_MANAGED_CPUS        "/sys/module/msm_performance/parameters/managed_cpus"

// Adding define for min freq nodes
#define PROP_MIN_FREQ_0    "ro.min_freq_0"
#define PROP_MIN_FREQ_4    "ro.min_freq_4"

/* Define various target init functions
 * and other data structures.
 * Steps for adding a new target:
 * 1. Define if it is a sync core or async core.(mSyncCore)
 * 2. Define number of clusters in it.(mNumCluster)
 * 3. Define total number of cores in a target.(mTotalNumCores)
 * 4. Define number of cores per cluster (mCorePerCluster)
 * 5. Define logical to physical cluster map.(mPhysicalClusterMap)
 * 6. Define pre-defined logical cluster to physical cluster
 *    map (mPredefineClusterMap). this can be done through using
 *    default map or a target specific map.
 * 7. Specify if any resources are not supported for this
 *    target(mResourceSupported).
 * 8. Specify the cpu on which corectl is enabled.
 * 9. Specify the minimum cores that should be online
 * 10. Call the CalculateCoreIndex function
 * 11. Specify a value map for a resource. Most commonly
 *     used value map is freq map.
 * */

//Default frequency map for value mapping
int msmDefaultFreqMap[FREQ_MAP_MAX] = {
    800,    /*LOWEST_FREQ*/
    1100,   /*LEVEL1_FREQ*/
    1300,   /*LEVEL2_FREQ*/
    1500,   /*LEVEL3_FREQ*/
    1650    /*HIGHEST_FREQ*/
};

//target specific Freq Map
int msm8939FreqMap[FREQ_MAP_MAX] = {
    800,
    1100,
    1300,
    1400,
    1600
};

int msm8952FreqMap[FREQ_MAP_MAX] = {
    960,
    1000,
    1300,
    1400,
    1600
};

int msm8956FreqMap[FREQ_MAP_MAX] = {
    400,
    800,
    1100,
    1400,
    1600
};

int msm8976FreqMap[FREQ_MAP_MAX] = {
    400,
    800,
    1100,
    1300,
    1700
};

int msm8937FreqMap[FREQ_MAP_MAX] = {
    998,
    1094,
    1248,
    1344,
    1401
};

int msm8953FreqMap[FREQ_MAP_MAX] = {
    652,
    1036,
    1401,
    1689,
    2300
};

int msmGoldFreqMap[FREQ_MAP_MAX] = {
    960,
    1094,
    1248,
    1400,
    1400
};

/*Target Specific pre-defined cluster maps*/
//Default pre-defined cluster map .
int msmDefaultPreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,  /*LAUNCH_CLUSTER*/
    1,  /*SCROLL_CLUSTER*/
    1,  /*ANIMATION_CLUSTER*/
};

int msm8939PreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,
    0,
    0
};

int msm8909PreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,
    0,
    0
};

int msm8916PreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,
    0,
    0
};


int msm8953PreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    1,
    1,
    1
};

int msmGoldPreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,
    0,
    0
};

int msm8937PreDefinedClusterMap[MAX_PRE_DEFINE_CLUSTER - START_PRE_DEFINE_CLUSTER] = {
    0,
    0,
    0
};

/* Initialize target based on socid.
 * 1. set that all resources are supported
 * 2. Initialize value map for all resources
 * to NULL.
 * 3. Call target specific init functions.
 * */
void Target::InitializeTarget() {
    int socid = 0;
    char tmp_s[NODE_MAX];
    int rc = 0;
    int qindx = 0;
    char clk_scaling_s[NODE_MAX];
    ResourceInfo tmpr;

    QLOGE("Inside InitializeTarget");
    mSetAllResourceSupported();
    mInitAllResourceValueMap();
    mResetResourceSupported(MISC_MAJOR_OPCODE, UNSUPPORTED_OPCODE);

    socid = getSocID();

    /* Define default cluster, value maps
     */
    mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msmDefaultFreqMap;
    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msmDefaultFreqMap;

    mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE;
    // Identifying storage device type. (UFS/EMMC)
    FREAD_STR(STORAGE_UFS_CLK_SCALING_DISABLE, clk_scaling_s, NODE_MAX, rc);
    if (rc > 0) {
        strlcpy(mStorageNode, STORAGE_UFS_CLK_SCALING_DISABLE,
                strlen(STORAGE_UFS_CLK_SCALING_DISABLE)+1);
    } else {
        strlcpy(mStorageNode, STORAGE_EMMC_CLK_SCALING_DISABLE,
                strlen(STORAGE_EMMC_CLK_SCALING_DISABLE)+1);
    }
    rc = 0;

    switch (socid) {
    case 239:
    case 263:
    case 241:
    case 268:
    case 269:
    case 270:
    case 271:
        mInitMSM8939();
        break;
    case 206:
    case 247:
    case 248:
    case 249:
    case 250:
        mInitMSM8916();
        break;
    case 245:
    case 258:
    case 259:
    case 265:
    case 260:
    case 261:
    case 262:
    case 300: /*8909w*/
        mInitMSM8909();
        break;
    case 264:
    case 289:
        mInitMSM8952();
        break;
    case 266:
    case 274:
        mInitMSM8956();
        break;
    case 277:
    case 278:
        mInitMSM8976();
        break;
    case 207:
        mInitMSM8994();
       break;
    case 251: /*8992*/
    case 252: /*8092*/
        mInitMSM8992();
        break;
    case 246: /*8996*/
    case 291: /*8096*/
    case 305: /*8996Pro*/
    case 312: /*8096Pro*/
        mInitMSM8996();
        break;
    case 292: /*8998*/
        mInitMSM8998();
        break;
    case 294: /*8937*/
    case 295: /*8937*/
    case 313: /*8940*/
        mInitMSM8937();
        break;
    case 293: /*MSM8953*/
    case 304: /*APQ8053*/
        mInitMSM8953();
        break;
    case 303: /*MSMGOLD*/
    case 307: /*APQGOLD*/
    case 308: /*MSMGOLD*/
    case 309: /*MSMGOLD*/
        mInitMSMGold();
        break;
    default:
        QLOGE("Target not supported");
        break;
    }

    //overriding predefined target based tables with values from
    //read from system properties
    OverrideTables();
}

void Target::mInitMSM8909() {
    QLOGE("init msm8909");
    mSyncCore = true;
    mNumCluster = 1;
    mTotalNumCores = 4;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = 0;
    mLogicalPerfMapPerfCluster();
    mPredefineClusterMap = msm8909PreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 1;
    mGovInstanceType = SINGLE_GOV_INSTANCE;
    Dump();
}

void Target::mInitMSM8916() {
    QLOGE("Init msm8916");
    mSyncCore = true;
    mNumCluster = 1;
    mTotalNumCores = 4;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = 0;
    mLogicalPerfMapPerfCluster();
    mPredefineClusterMap = msm8916PreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 1;
    mGovInstanceType = SINGLE_GOV_INSTANCE;

    // set prop in here
    property_set(PROP_MIN_FREQ_0, "800000");

    Dump();
}

void Target::mInitMSM8939() {
    ResourceInfo tmpr;
    int qindx = 0;

    QLOGE("Init msm8939");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 8;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = 0;
    mLogicalPerfMapPerfCluster();
    mPredefineClusterMap = msm8939PreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 0;
    /* Define Value maps. Construct Major and minor
     * temp resource for cpufreq
     * Define for min_freq. Freq mapped in Mhz.
     */
    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8939FreqMap;

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8939FreqMap;

    // set prop in here
    property_set(PROP_MIN_FREQ_0, "800000");
    property_set(PROP_MIN_FREQ_4, "499200");

    Dump();
}

void Target::mInitMSM8952() {
    ResourceInfo tmpr;
    int qindx = 0;

    QLOGE("Init msm8952");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 8;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = 0;
    mLogicalPerfMapPerfCluster();
    mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 0;

    /* Define Value maps. Construct Major and minor
     * temp resource for cpufreq
     * Define for min_freq. Freq mapped in Mhz.
     */

    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    QLOGI("Define value map for Cpufreq min resource with qindx %d ", qindx);
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8952FreqMap;

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8952FreqMap;

    Dump();
}

void Target::mInitMSM8976() {
    ResourceInfo tmpr;
    int qindx = 0;

    QLOGI("Init msm8976");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 8;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = 4;

    /** Create a logical to physical cluster mapping       **
     ** in which logical cluster "0" maps to power cluster */
    mLogicalPerfMapPowerCluster();

    //predefined cluster mapping ex. UX mappings
    mPredefineClusterMap = msmDefaultPreDefinedClusterMap;

    mCalculateCoreIdx();
    mMinCoreOnline = 0;

    /* Define Value maps. Construct Major and minor
     * temp resource for cpufreq
     * Define for min_freq. Freq mapped in Mhz.
     */
    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8976FreqMap;

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8976FreqMap;

    Dump();
}

void Target::mInitMSM8956() {
    QLOGI("Init msm8956");
    ResourceInfo tmpr;
    int qindx = 0;
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 6;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 2;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = 4;
    mLogicalPerfMapPowerCluster();
    mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 0;

    /* Define Value maps. Construct Major and minor
     * temp resource for cpufreq
     * Define for min_freq. Freq mapped in Mhz.
     */
    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8956FreqMap;

    //Define for max_freq. Freq mapped in Mhz
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8956FreqMap;

    Dump();
}

void Target::mInitMSM8994() {
    char tmp_s[NODE_MAX];
    int rc = 0;

    QLOGI("Init msm8994");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 8;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = 4;
    mLogicalPerfMapPowerCluster();
    mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 0;

    //KPM node initialization
    FREAD_STR(KPM_MANAGED_CPUS, tmp_s, NODE_MAX, rc);
    if(rc < 0) {
        QLOGE("Error reading KPM nodes. Does KPM exist\n");
    } else {
        FWRITE_STR(KPM_NUM_CLUSTERS, "2", 1, rc);
        FWRITE_STR(KPM_MANAGED_CPUS, "0-3", 3, rc);
        FWRITE_STR(KPM_MANAGED_CPUS, "4-7", 3, rc);
    }

    Dump();
}

void Target::mInitMSM8992() {
    char tmp_s[NODE_MAX];
    int rc = 0;

    QLOGI("Init msm8992");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 6;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 2;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = -1;
    mLogicalPerfMapPowerCluster();
    mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 0;

    //KPM node initialization
    FREAD_STR(KPM_MANAGED_CPUS, tmp_s, NODE_MAX, rc);
    if(rc < 0) {
        QLOGE("Error reading KPM nodes. Does KPM exist\n");
    } else {
       FWRITE_STR(KPM_NUM_CLUSTERS, "2", 1, rc);
       FWRITE_STR(KPM_MANAGED_CPUS, "0-3", 3, rc);
       FWRITE_STR(KPM_MANAGED_CPUS, "4-5", 3, rc);
    }

    Dump();
}

void Target::mInitMSM8998() {
    char tmp_s[NODE_MAX];
    int rc = 0;

    QLOGI("Init msm8998");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 8;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster) {
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = -1;
    mLogicalPerfMapPowerCluster();
    mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline= 0;

    //KPM node initialization
    FREAD_STR(KPM_MANAGED_CPUS, tmp_s, NODE_MAX, rc);
    if(rc < 0) {
        QLOGE("Error reading KPM nodes. Does KPM exist\n");
    } else {
        FWRITE_STR(KPM_NUM_CLUSTERS, "2", 1, rc);
        FWRITE_STR(KPM_MANAGED_CPUS, "0-3", 3, rc);
        FWRITE_STR(KPM_MANAGED_CPUS, "4-7", 3, rc);
    }

}

void invoke_wa_libs() {
#define KERNEL_WA_NODE "/sys/module/app_setting/parameters/lib_name"
   int rc;
   const char *wl_libs[] = {
      "libvoH264Dec_v7_OSMP.so",
      "libvossl_OSMP.so",
      "libTango.so"
   };
   int i;

   int len = sizeof (wl_libs) / sizeof (*wl_libs);

   for(i = 0; i < len; i++) {
      FWRITE_STR(KERNEL_WA_NODE, wl_libs[i], strlen(wl_libs[i]), rc);
      QLOGI("Writing to node (%s)  (%s) rc:%d\n", KERNEL_WA_NODE, wl_libs[i], rc);
   }

}

void Target::mInitMSM8996() {
    char tmp_s[NODE_MAX];
    int rc = 0;

    QLOGI("Init msm8996");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 4;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster) {
        mCorePerCluster[0] = 2;
        mCorePerCluster[1] = 2;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = -1;
    mLogicalPerfMapPowerCluster();
    mPredefineClusterMap = msmDefaultPreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline= 0;

    //KPM node initialization
    FREAD_STR(KPM_MANAGED_CPUS, tmp_s, NODE_MAX, rc);
    if(rc < 0) {
        QLOGE("Error reading KPM nodes. Does KPM exist\n");
    } else {
        FWRITE_STR(KPM_NUM_CLUSTERS, "2", 1, rc);
        FWRITE_STR(KPM_MANAGED_CPUS, "0-1", 3, rc);
        FWRITE_STR(KPM_MANAGED_CPUS, "2-3", 3, rc);
    }

    // set prop in here
    property_set(PROP_MIN_FREQ_0, "384000");
    property_set(PROP_MIN_FREQ_4, "384000");

    invoke_wa_libs();
    Dump();
}

void Target::mInitMSM8937() {
    ResourceInfo tmpr;
    int qindx = 0;

    QLOGE("Init msm8937");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 8;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }
    mCoreCtlCpu = 0;
    mLogicalPerfMapPerfCluster();
    mPredefineClusterMap = msm8937PreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 0;

    /* Define Value maps. Construct Major and minor
     * temp resource for cpufreq
     * Define for min_freq. Freq mapped in Mhz.
     */

    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    QLOGI("Define value map for Cpufreq min resource with qindx %d ", qindx);
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8937FreqMap;

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8937FreqMap;

    Dump();
}

void Target::mInitMSM8953() {
    ResourceInfo tmpr;
    int qindx = 0;
    char tmp_s[NODE_MAX];
    int rc = 0;

    QLOGE("Init msm8953");
    mSyncCore = true;
    mNumCluster = 2;
    mTotalNumCores = 8;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
        mCorePerCluster[1] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }

    mLogicalPerfMapPowerCluster();
    mPredefineClusterMap = msm8953PreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 0;

    //single gov instance for both clusters
    mGovInstanceType = SINGLE_GOV_INSTANCE;

    //unsupported opcodes
    mResetResourceSupported(SCHED_MAJOR_OPCODE, SCHED_PREFER_IDLE_OPCODE);
    mResetResourceSupported(SCHED_MAJOR_OPCODE, SCHED_MOSTLY_IDLE_LOAD_OPCODE);
    mResetResourceSupported(SCHED_MAJOR_OPCODE, SCHED_MOSTLY_IDLE_NR_RUN_OPCODE);
    mResetResourceSupported(SCHED_MAJOR_OPCODE, SCHED_UPMIGRATE_OPCODE);
    mResetResourceSupported(SCHED_MAJOR_OPCODE, SCHED_DOWNMIGRATE_OPCODE);

    /* Define Value maps. Construct Major and minor
     * temp resource for cpufreq
     * Define for min_freq. Freq mapped in Mhz.
     */

    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    QLOGI("Define value map for Cpufreq min resource with qindx %d ", qindx);
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8953FreqMap;

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msm8953FreqMap;

    //KPM node initialization
    FREAD_STR(KPM_MANAGED_CPUS, tmp_s, NODE_MAX, rc);
    if(rc < 0) {
        QLOGE("Error reading KPM nodes. Does KPM exist\n");
    } else {
       FWRITE_STR(KPM_NUM_CLUSTERS, "2", 1, rc);
       FWRITE_STR(KPM_MANAGED_CPUS, "0-3", 3, rc);
       FWRITE_STR(KPM_MANAGED_CPUS, "4-7", 3, rc);
    }

    Dump();
}

void Target::mInitMSMGold() {
    ResourceInfo tmpr;
    int qindx = 0;
    char tmp_s[NODE_MAX];
    int rc = 0;

    QLOGE("Init msmGold");
    mSyncCore = true;
    mNumCluster = 1;
    mTotalNumCores = 4;
    mCorePerCluster = new int[mNumCluster];
    if (mCorePerCluster){
        mCorePerCluster[0] = 4;
    } else {
        QLOGE("Error: Could not initialize cores in cluster \n");
    }

    mLogicalPerfMapPerfCluster();
    mPredefineClusterMap = msmGoldPreDefinedClusterMap;
    mCalculateCoreIdx();
    mMinCoreOnline = 1;

    /* Define Value maps. Construct Major and minor
     * temp resource for cpufreq
     * Define for min_freq. Freq mapped in Mhz.
     */

    tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
    tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    QLOGI("Define value map for Cpufreq min resource with qindx %d ", qindx);
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msmGoldFreqMap;

    //Define for max_freq. Freq mapped in Mhz.
    tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
    qindx = tmpr.DiscoverQueueIndex();
    mValueMap[qindx].mapSize = FREQ_MAP_MAX;
    mValueMap[qindx].map = msmGoldFreqMap;

    //KPM node initialization
    FREAD_STR(KPM_MANAGED_CPUS, tmp_s, NODE_MAX, rc);
    if(rc < 0) {
        QLOGE("Error reading KPM nodes. Does KPM exist\n");
    } else {
       FWRITE_STR(KPM_NUM_CLUSTERS, "1", 1, rc);
       FWRITE_STR(KPM_MANAGED_CPUS, "0-3", 3, rc);
    }
    Dump();
}

