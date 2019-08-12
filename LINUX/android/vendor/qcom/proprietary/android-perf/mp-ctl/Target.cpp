/******************************************************************************
  @file    Target.cpp
  @brief   Implementation of targets

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#define LOG_TAG "ANDR-PERF-TARGET"
#include <cutils/log.h>
#include <fcntl.h>
#include <cstdlib>
#include <string.h>
#include <cutils/properties.h>

#include "Request.h"

#include "Target.h"
#include "OptsData.h"
#include "MpctlUtils.h"

Target Target::cur_target;

#define PROPERTY_PREDEFINED_CLUSTER_MAP "perflocks.predefined_clust_map"
#define PROPERTY_PREDEFINED_FREQUENCY_MAP "perflocks.predefined_freq_map"

using namespace std;

/*
 * Definition for various get functions
 * */
int Target::getCoresInCluster(int cluster) {
    if (mCorePerCluster) {
        if ((cluster >= 0) && (cluster < mNumCluster)) {
            int tmp = mCorePerCluster[cluster];
            if (tmp <= mTotalNumCores) {
                return tmp;
            }
        } else {
            QLOGE("Error: Invalid cluster id %d",cluster);
        }
    } else {
        QLOGE("Error: Target not initialized");
    }
    return -1;
}

int Target::getPhysicalCluster(unsigned short logicalCluster) {
    int cluster = -1;
    // If logicalCluster lies  within the range of the
    // physical clusters, then return the cluster from
    // the physicalCLusterMap.
    if (logicalCluster < mNumCluster) {
        if (mPhysicalClusterMap) {
            cluster = mPhysicalClusterMap[logicalCluster];
        }
    } else if ((logicalCluster >= START_PRE_DEFINE_CLUSTER) &&
               (logicalCluster < MAX_PRE_DEFINE_CLUSTER)) {
        //logical cluster lies in pre-defined cluster range
        if (mPredefineClusterMap) {
            cluster = mPredefineClusterMap[logicalCluster - START_PRE_DEFINE_CLUSTER];
        }
    } else {
        QLOGE("Error: Invalid logical cluster id %d", logicalCluster);
    }

    if (cluster > mNumCluster) {
        cluster = -1;
    }
    return cluster;
}

int Target::getFirstCoreOfPerfCluster() {
    int perfCluster = -1;
    int supportedCore = -1;

    if ((NULL == mPhysicalClusterMap) || (NULL == mLastCoreIndex)) {
        return supportedCore;
    }

    perfCluster = mPhysicalClusterMap[0];
    if ((perfCluster >= 0) && (perfCluster < mNumCluster)) {
        if (perfCluster == 0) {
            supportedCore = 0;
        } else {
            supportedCore = mLastCoreIndex[perfCluster-1] + 1;
        }
    }
    return supportedCore;
}

int Target::getPhysicalCore(int cluster, unsigned short logicalCore) {
    int physicalCore = -1;

    if ((cluster > 0) && (cluster < mNumCluster)) {
        if (NULL != mLastCoreIndex) {
            physicalCore = logicalCore + mLastCoreIndex[cluster-1] + 1;
        }
    } else if (cluster == 0) {
        physicalCore = logicalCore;
    }
    if ((NULL != mLastCoreIndex) && (physicalCore > mLastCoreIndex[cluster])) {
        physicalCore = -1;
    }

    return physicalCore;
}

int Target::getLastCoreIndex(int cluster) {
    if (mLastCoreIndex) {
        if (cluster >=0 && cluster < mNumCluster) {
            return mLastCoreIndex[cluster];
        } else {
            QLOGE("Error: Invalid cluster id %d", cluster);
        }
    } else {
        QLOGE("Eror: Target not initialized");
    }
    return -1;
}

Target::Target() {
    QLOGI("Target constructor");
    mSyncCore = true;
    mNumCluster = 0;
    mTotalNumCores = 0;
    mCorePerCluster = NULL;
    mPhysicalClusterMap = NULL;
    mPredefineClusterMap = NULL;
    mLastCoreIndex = NULL;
    memset(mResourceSupported, 0x00, sizeof(mResourceSupported));
    mCoreCtlCpu = -1;
    mMinCoreOnline = -1;
    mGovInstanceType = CLUSTER_BASED_GOV_INSTANCE;
    memset(mValueMap, 0x00, sizeof(mValueMap));
    memset(mStorageNode, '\0', sizeof(mStorageNode));
}

Target::~Target() {
    QLOGI("Target destructor");

    if (mCorePerCluster) {
        delete [] mCorePerCluster;
        mCorePerCluster = NULL;
    }

    if (mPhysicalClusterMap){
        delete [] mPhysicalClusterMap;
        mPhysicalClusterMap = NULL;
    }

    if (mPredefineClusterMap){
        mPredefineClusterMap = NULL;
    }

    if (mLastCoreIndex) {
        delete [] mLastCoreIndex;
        mLastCoreIndex = NULL;
    }
}

/* Create a logical to physical cluster
 * mapping, in which Logical cluster "0"
 * maps to physical cluster "0". Useful
 * for the targets in which cluster0 is
 * the perf cluster.
 * */
void Target::mLogicalPerfMapPerfCluster() {
    int i;
    mPhysicalClusterMap = new int[mNumCluster];
    if(mPhysicalClusterMap) {
        for(i=0; i < mNumCluster; i++) {
            mPhysicalClusterMap[i] = i ;
        }
   } else {
     QLOGE("Error: Could not map Logical perf cluster to perf\n");
   }
}

/* Create a logical to physical cluster mapping
 * in which logical cluster "0" maps to power
 * cluster
 * */
void Target::mLogicalPerfMapPowerCluster() {
    int i, cluster = mNumCluster-1;
    mPhysicalClusterMap = new int[mNumCluster];
    if(mPhysicalClusterMap) {
        for(i=0; i < mNumCluster && cluster >= 0; i++,cluster--) {
            mPhysicalClusterMap[i] = cluster;
        }
    } else {
        QLOGE("Error: Could not map logical perf to power cluster\n");
    }
}

/* Calculate the number/index for last
 * core in this cluster
 * */
void Target::mCalculateCoreIdx() {
    int cumlative_core, i;
    mLastCoreIndex = new int[mNumCluster];
    if(mLastCoreIndex && mCorePerCluster) {
        cumlative_core = 0;
        for(i =0;i < mNumCluster; i++) {
            mLastCoreIndex[i] = cumlative_core + (mCorePerCluster[i] - 1);
            cumlative_core = mLastCoreIndex[i] + 1;
        }
    } else {
        QLOGE("Error: Initializing core index failed\n");
    }
}

/* Resource support functions. */

/* Return the bitmap value for the asked resource.
 * If Resource is supported it will return 1,
 * else 0.
 * */
bool Target::isResourceSupported(unsigned short major, unsigned short minor) {
    unsigned long tmp = 0;
    bool ret = false;

    if (major < MAX_MAJOR_RESOURCES) {
        tmp = mResourceSupported[major];
        ret = ((tmp >> minor) & 0x1);
    }

    return ret;
}

/* This function sets all the minor resources for a
 * major resource to be supported.
 * */
void Target::mSetAllResourceSupported() {
    unsigned int minorIdx,rshIdx;
    for (unsigned int i = 0; i < MAX_MAJOR_RESOURCES; i++) {
        minorIdx = ResourceInfo::GetMinorIndex(i);
        rshIdx = sizeof(unsigned long)*8 - minorIdx;
        mResourceSupported[i] = ((unsigned long)~0) >> rshIdx;
    }
}

/* This function resets the minor rsource in the specified
 * major resource to false (not supported).
 * */
void Target::mResetResourceSupported(unsigned short major, unsigned short minor) {
    unsigned long tmp;
    if (major < MAX_MAJOR_RESOURCES) {
        if (minor < ResourceInfo::GetMinorIndex(major)) {
            mResourceSupported[major] &= ~(1 << minor);
        }
    } /*if (major < MAX_MAJOR_RESOURCES)*/
}

/** This function returns the cluster number to which a
 * given cpu belongs. It also updates input parameters
 * with first cpu and last cpu in that cluster.
 * On error the return value is -1
 */
int Target::getClusterForCpu(int cpu, int &startcpu, int &endcpu) {
    int i, cluster = -1;
    bool success = false;

    startcpu = 0;
    endcpu = -1;

    if ((cpu < 0) || (cpu > mTotalNumCores) || (NULL == mLastCoreIndex)) {
        QLOGE("Invalid cpu number %d. Cannot find cluster.", cpu);
        return -1;
    }

    for (i = 0 ; i < mNumCluster && !success; i++) {
        endcpu = mLastCoreIndex[i];
        if (cpu >= startcpu && cpu <= endcpu) {
            cluster = i;
            success = true;
        }
        else {
            startcpu = mLastCoreIndex[i] + 1;
        }
    }

    if (!success){
        cluster = -1;
        startcpu = -1;
        endcpu = -1;
    }

    return cluster;
}


/* Functions to support value maps. */

/* This function initializes the valueMap for all
 * the resources. The default is "false" and
 * no map is specified.
 *
 * Whichever resource needs a map and has to be
 * specified should be done in specifc target
 * functions.
 * */
void Target::mInitAllResourceValueMap() {
    int i = 0;
    for (i = 0; i < MAX_MINOR_RESOURCES; i++) {
        mValueMap[i].mapSize = -1;
        mValueMap[i].map = NULL;
    }
}

int Target::getSocID() {
    int fd;
    int soc = -1;
    if (!access("/sys/devices/soc0/soc_id", F_OK)) {
        fd = open("/sys/devices/soc0/soc_id", O_RDONLY);
    } else {
        fd = open("/sys/devices/system/soc/soc0/id", O_RDONLY);
    }
    if (fd != -1)
    {
        char raw_buf[5];
        read(fd, raw_buf,4);
        raw_buf[4] = 0;
        soc = atoi(raw_buf);
        close(fd);
    }
    else {
        close(fd);
    }
    return soc;
}

/* This function returns the mapped value for
 * a major resource and minor resource, specified
 * through the index.
 * */
int Target::getMappedValue(int resrcIndex, int val) {
    if ((resrcIndex >= 0) && (resrcIndex < MAX_MINOR_RESOURCES)
           && (resrcIndex != UNSUPPORTED_Q_INDEX)) {
        if(mValueMap[resrcIndex].map != NULL) {
            if (val >= 0 && val < mValueMap[resrcIndex].mapSize) {
                return mValueMap[resrcIndex].map[val];
            } else {
                QLOGE("Value Map not defined for this %d index for the %d value", resrcIndex, val);
            }
        } else {
            QLOGE("Value map not defined for this %d index", resrcIndex);
        }
    } else {
        QLOGE("Error: Cannot find mapped value for the resource with index %d", resrcIndex);
    }
    return -1;
}

/* Calculate bitmask of all CPUs in target.
 * For example, if total number of CPUs is 4,
 * then bitmask will be 0b1111 (or 0xF).
 * */
int Target::getAllCpusBitmask() {
    if(mTotalNumCores) {
        return (1 << mTotalNumCores) - 1;
    } else {
        QLOGE("Error: Initializing total cores failed\n");
        return -1;
    }
}

int Target::OverrideTables() {
    char property[PROPERTY_VALUE_MAX];
    int idx = -1, value = -1;
    char *ch = NULL;
    char *tmp_token = NULL;

    if (property_get(PROPERTY_PREDEFINED_CLUSTER_MAP, property, NULL) > 0) {
        QLOGI("  PROPERTY_PREDEFINED_CLUSTER_MAP_INDEX to %s", property);
        //ex. expect input "0,1,1,0,2,1,3,1"

        ch = strtok_r(property, ",", &tmp_token);
        while (NULL != ch) {
            idx = atoi(ch);
            ch = strtok_r(NULL, ",", &tmp_token);
            if (NULL != ch) {
                value = atoi(ch);
                if ((idx >= START_PRE_DEFINE_CLUSTER) && (idx < MAX_PRE_DEFINE_CLUSTER) &&
                    (value >= 0) && (value < mNumCluster) && (NULL != mPredefineClusterMap)) {
                    mPredefineClusterMap[idx] = value;
                }
            }
            ch = strtok_r(NULL, ",", &tmp_token);
        } /*while (NULL != ch)*/
    }

    if (property_get(PROPERTY_PREDEFINED_FREQUENCY_MAP, property, NULL) > 0) {
        QLOGI("  PROPERTY_PREDEFINED_FREQUENCY_MAP to %s", property);
        //ex. expect input "0,800,1,1600,2,2300"
        ResourceInfo tmpr;
        int qindx1 = 0, qindx2 = 0;

        tmpr.SetMajor(CPUFREQ_MAJOR_OPCODE);
        tmpr.SetMinor(CPUFREQ_MIN_FREQ_OPCODE);
        qindx1 = tmpr.DiscoverQueueIndex();
        tmpr.SetMinor(CPUFREQ_MAX_FREQ_OPCODE);
        qindx2 = tmpr.DiscoverQueueIndex();

        ch = strtok_r(property, ",", &tmp_token);
        while (NULL != ch) {
            idx = atoi(ch);
            ch = strtok_r(NULL, ",", &tmp_token);
            if (NULL != ch) {
                value = atoi(ch);
                if ((idx >= LOWEST_FREQ) && (idx < FREQ_MAP_MAX) &&
                    (value > 0)) {
                    if (mValueMap[qindx1].map) {
                        mValueMap[qindx1].map[idx] = value;
                    }
                    if (mValueMap[qindx2].map) {
                        mValueMap[qindx2].map[idx] = value;
                    }
                }
            } /*if (NULL != ch)*/
            ch = strtok_r(NULL, ",", &tmp_token);
        } /*while (NULL != ch)*/
    }
    return 0;
}

void Target::Dump() {
    int i;
    QLOGI("Number of cluster %d \n", mNumCluster);
    QLOGI("Number of cores %d \n", mTotalNumCores);
    for (i =0; i < mNumCluster; i++) {
        if (mCorePerCluster) {
            QLOGI("Cluster %d has %d cores", i, mCorePerCluster[i]);
        }
        if (mPhysicalClusterMap) {
            QLOGI("Logical cluster %d is mapped to physical cluster %d", i, mPhysicalClusterMap[i]);
        }
        if (mLastCoreIndex) {
            QLOGI("End index for physical cluster %d is %d",i, mLastCoreIndex[i]);
        }
    }
    for (i=0; i < MAX_PRE_DEFINE_CLUSTER-START_PRE_DEFINE_CLUSTER; i++) {
        if (mPredefineClusterMap) {
            QLOGI("Logical pre-defined cluster %d is mapped to physical cluster %d",
                  i+START_PRE_DEFINE_CLUSTER, mPredefineClusterMap[i]);
        }
    }
    QLOGI("Core_ctl_cpu %d", mCoreCtlCpu);
    QLOGI("min_core_online %d", mMinCoreOnline);
    return;
}


