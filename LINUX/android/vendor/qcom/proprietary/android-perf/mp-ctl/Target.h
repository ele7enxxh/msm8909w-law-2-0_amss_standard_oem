/******************************************************************************
  @file    Target.h
  @brief   Implementation of target

  DESCRIPTION

  ---------------------------------------------------------------------------
  Copyright (c) 2011-2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ---------------------------------------------------------------------------
******************************************************************************/

#ifndef __TARGET_SPECIFICS__H_
#define __TARGET_SPECIFICS__H_

#include "ResourceInfo.h"
#include "mp-ctl.h"

//TODO Remove the usage for this.
#define MAX_CORES 8
#define NODE_MAX 150

/* Structure for defining value maps.
 * */
typedef struct value_map{
    int *map;
    int mapSize;
}value_map;

enum {
    CLUSTER_BASED_GOV_INSTANCE, /*both cluster based/core based*/
    SINGLE_GOV_INSTANCE,
}IntGovInstanceType;

/* Target class: It contains target
 * specific information. It is a singleton
 * class. On mpctl-server-init a object for this
 * class is initialized with the current target
 * specific information.
 * */
class Target {

private:
    bool mSyncCore;  /*true- if target is sync core*/
    unsigned short mNumCluster;  /*total number of cluster*/
    unsigned short mTotalNumCores;    /*total number of cores*/
    int *mCorePerCluster; /*number of cores per cluster*/
    int *mPhysicalClusterMap; /*logical to physical cluster map*/
    int *mPredefineClusterMap; /*pre-defined logical cluster to physical cluster map*/
    int *mLastCoreIndex; /*Maintain end core index for each physical cluster*/

    /*resource supported bitmaps which will let us know which resources are
     *supported on this target. If minor types are greater than 32 then     *
     *this needs to be resized. This is a bitmap of 32bits per Major        *
     *resource. Every bit representing the minor resource in the major      *
     *resource.                                                             */
    unsigned long mResourceSupported[MAX_MAJOR_RESOURCES];

    int mCoreCtlCpu; /*core_ctl is enabled on which physical core*/
    int mMinCoreOnline; /*Minimum number of cores needed to be online*/

    char mStorageNode[NODE_MAX];

    /* Value map is declared for each resource and defined
     * by a target for specific resources.*/
    struct value_map mValueMap[MAX_MINOR_RESOURCES];

    //defines which type of int gov instance target has
    //paths will vary based on this
    unsigned short mGovInstanceType;

    // Singelton object of this class
    static Target cur_target;

    //Init target functions
    void mInitMSM8909();
    void mInitMSM8916();
    void mInitMSM8939();
    void mInitMSM8976();
    void mInitMSM8952();
    void mInitMSM8956();
    void mInitMSM8992();
    void mInitMSM8994();
    void mInitMSM8996();
    void mInitMSM8998();
    void mInitMSM8937();
    void mInitMSM8953();
    void mInitMSMGold();

    void mLogicalPerfMapPerfCluster();
    void mLogicalPerfMapPowerCluster();
    void mCalculateCoreIdx();
    void mSetAllResourceSupported();
    void mResetResourceSupported(unsigned short, unsigned short);
    void mInitAllResourceValueMap();
    int getSocID();

    Target();
    Target(Target const& oh);
    Target& operator=(Target const& oh);

public:
    ~Target();

    void InitializeTarget();
    //get functions
    bool isSyncCore() {
        return mSyncCore;
    }
    unsigned short getNumCluster() {
        return mNumCluster;
    }
    unsigned short getTotalNumCores()  {
        return mTotalNumCores;
    }
    int getCoresInCluster(int cluster);
    int getPhysicalCluster(unsigned short logicalCluster);
    int getFirstCoreOfPerfCluster();
    int getLastCoreIndex(int cluster);
    int getPhysicalCore(int cluster, unsigned short logicalCore);
    bool isResourceSupported(unsigned short major, unsigned short minor);
    int getClusterForCpu(int cpu, int &startcpu, int &endcpu);
    int getMappedValue(int qindx, int val);
    int getAllCpusBitmask();

    int getCoreCtlCpu() {
        return mCoreCtlCpu;
    }

    int getMinCoreOnline() {
        return mMinCoreOnline;
    }

    int getGovInstanceType() {
        return mGovInstanceType;
    }

    char* getStorageNode() {
        return mStorageNode;
    }

    int OverrideTables();

    void Dump();


    //Initialize cur_target
    static Target &getCurTarget()
    {
       return cur_target;
    }
};

#endif /*__TARGET_SPECIFICS__H_*/
