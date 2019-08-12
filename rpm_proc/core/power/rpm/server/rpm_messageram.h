/*==============================================================================

FILE:      rpm_messageram.h

      Copyright (c) 2012-2014 QUALCOMM Technologies Incorporated.
               All Rights Reserved.
         QUALCOMM Proprietary and Confidential

$Header: //components/rel/rpm.bf/2.1.1/core/power/rpm/server/rpm_messageram.h#1 $
$Date: 2015/02/25 $

==============================================================================*/

#ifndef RPM_MESSAGERAM_H
#define RPM_MESSAGERAM_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>
#include "smem_v.h"
#include "vmpm.h"

typedef struct
{
  uint32_t reserved;           // reserved word to keep vmpm data a multiple of 4
  uint32_t mba_done;           // Used by MBA to let the RPM clock driver know if MBA init is done. this will be used before accessing DEHR register
  uint64_t wakeup_time;        // QTimer count of planned wakeup.  '0' does *not* mean infinite, it means "wakeup immediately."
  uint32_t vmpm_enable[2];     // '1' = enabled, '0' = don't care (might still be enabled by someone else)
  uint32_t vmpm_detect_ctl[2]; // requires both ctl bits and polarity to determine trigger type
  uint32_t vmpm_detect_ctl3[2]; // requires both ctl bits and polarity to determine trigger type 
  uint32_t vmpm_polarity[2];   // '0' = low/falling, '1' = high/rising
  uint32_t vmpm_status[2];     // Bits will be set to '1' for each interrupt pending when deep sleep is exited.
                               // Clear directly; RPM does not read or modify status contents unless all masters are asleep.
} vmpm_data_t;

typedef struct
{
  uint32_t active_cores; // Bitmask of active cores - core 0 = bit 0, core 1 = bit 1 ..
  uint32_t num_shutdowns; // number of times all cores have power collapsed
  uint64_t shutdown_req; // timestamp of last shutdown of all cores
  uint64_t wakeup_ind; // timestamp of last master wakeup ind
  uint64_t bringup_req; // timestamp of last bring up req
  uint64_t bringup_ack; // timestamp of last bring up ack
  uint32_t wakeup_reason; // 0 = rude wakeup, 1 = scheduled wakeup
  uint32_t last_sleep_transition_duration; // time taken (in ticks) for the last W->S transition
  uint32_t last_wake_transition_duration; // time taken (in ticks) for the last S->W transition
  uint32_t xo_count;                // Shows how many times the SS has power collapsed and voted for XO shutdown
  uint64_t xo_last_entered_at;      // Last timestamp the SS power collapsed and voted for XO shutdown
  uint64_t xo_last_exited_at;       // Last timestamp the SS exited PC( with vote for XO shutdown) and woken up by RPM
  uint64_t xo_accumulated_duration; // Shows how long the SS has been in a sleep mode since it has booted 
  uint32_t reserved[10];
} rpm_master_stats_t;
  
//per master message ram memory mapping
typedef struct
{
  uint32_t    small_alloc_space[84];
  rpm_master_stats_t master_stats;
  vmpm_data_t vmpm;
  uint32 reserved[384];
  uint32_t    large_alloc_space[512];
} message_ram_master_t;

//per master block allocation data
typedef struct
{
  uint32_t small_alloc_used; 
  uint32_t large_alloc_used; 
} message_ram_master_data_t;

//API for SMD to allocate message RAM based on master. 
//Each master has 2.5k, in two 1k blocks and a 336 byte block.
//Returns a pointer to the memory region for the given master
void * message_ram_malloc(smem_host_type master, uint32_t size_in_bytes);

vmpm_data_t *message_ram_vmpm(vmpm_masters master);
rpm_master_stats_t *message_ram_master_stats (unsigned ee);
void message_ram_update_master_xo_stats(unsigned ee, int vote);

#ifdef __cplusplus
};
#endif // __cplusplus

#endif // RPM_MESSAGERAM_H

