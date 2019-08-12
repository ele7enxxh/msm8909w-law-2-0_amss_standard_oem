/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef DALRPMFWESTIMATOR_H
#define DALRPMFWESTIMATOR_H

#include <stdint.h>

#include "../common/inmap.h"
#include "time_service.h"
#include "rpm.h"

#if (DAL_CONFIG_TARGET_ID == 0x9645)
#define NUM_CLOCK_RATES (3)
#else
#define NUM_CLOCK_RATES (2)
#endif

#ifdef __cplusplus

struct Observation
{
    uint64_t time;
    uint32_t clockRate;
};

#define INVALID_ESTIMATE 0xFFFFFFFFu

class Estimator;
Estimator &theEstimator(void);

class Estimator
{
public:
    // 32-bit estimates give "good enough" range:
    //      @19.2 MHz: ~3.73 minutes

    void updateClockRate(unsigned int clockFreqkHz);

    unsigned currentOperatingPoint(void) const { return selectedClock_; }

    unsigned getClockRate(unsigned operatingPoint) const __value_in_regs
    {
        return clockFreqkHz_[operatingPoint];
    }
    
    // Common function to retrieve timestamp in the right format for estimation
    static Observation startObservation() __value_in_regs
    {
        Observation obs;
        obs.time = time_service_now();
        obs.clockRate = theEstimator().selectedClock_;
        return obs;
    }

    // Common function to calculate estimation (including truncation to max)
    static Observation completeObservation(Observation &obs) __value_in_regs
    {
        obs.time = time_service_now() - obs.time;
        return obs;
    }

private:
    Estimator();
    friend Estimator &theEstimator();

    // Currently "bin" into the first two clock frequencies observed.
    unsigned clockFreqkHz_[NUM_CLOCK_RATES];
    unsigned selectedClock_;
};


class EstimateCache
{
public:
    bool lookup(uint32_t systemState, uint32_t actionHash, uint32_t *resultState, uint32_t *timeEstimate);
    void observe(uint32_t startState, uint32_t actionHash, uint32_t resultState, uint32_t elapsed, bool save_estimate);

private:
    EstimateCache(uint32_t size);
    friend EstimateCache &theEstimateCache();
    friend EstimateCache &theEstimateCacheImmediate();

    struct CacheEntry
    {
        uint32_t systemTag;   // the corresponding system state (where we're at now)
        uint32_t actionTag;   // the corresponding sub-state (what is being applied)
        uint32_t resultState; // the resulting system state after applying the action on the systemTag state
        uint32_t estimate;    // how long this (system, sub) pairing has taken in the past
        uint32_t usedSeq;     // LRU replacement bookkeeping
    };

    CacheEntry *cache_;
    uint32_t   cacheSize_;
    uint32_t   cacheSeq_;
};

EstimateCache &theEstimateCache(void);
EstimateCache &theEstimateCacheImmediate(void);

#endif // __cplusplus

#endif // DALRPMFWESTIMATOR_H

