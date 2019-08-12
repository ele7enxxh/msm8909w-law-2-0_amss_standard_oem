/*===========================================================================

  rpm_config.h - data related to platform-specific customization

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_CONFIG_H
#define RPM_CONFIG_H

#include "smd_lite.h"
#include "rpmserver.h"
#include "rpm_estimator.h"
#include "rpm_messageram.h"

struct Handler;
struct SetChanger;

typedef struct
{
    unsigned  numCores;
    unsigned *bringupInts;
    unsigned *bringupAcks;
    unsigned *shutdownInts;
    unsigned *shutdownAcks;
} SPMDescription;

typedef struct
{
  unsigned         priority;
  smd_channel_type edge;
  unsigned         smd_fifo_sz;
  unsigned         ee_buflen;
  SPMDescription   spm;
  unsigned         wakeupInt;

  // Pointers to tasks for this EE.  Eventually embed the task itself;
  // unfortunately we can't do that with C++.
  struct Handler    *handler;
  struct SetChanger *changer;

  // SPM runtime state.
  rpm_spm_status_type subsystem_status;
  bool                is_soft_transition;
  unsigned            num_active_cores;
  unsigned            pending_bringups;

  // stats blob
  rpm_master_stats_t *stats;
} EEData;

typedef struct ict_s
{
    unsigned      client_type;
    unsigned      inrep_size;
    struct ict_s *next;
} InternalClientType;

typedef struct
{
    unsigned type;
    void    *request;
} InternalClient;

typedef struct
{
    uint32_t            type;           // The 'type' portion of the (type, id) pair identifying a resource.
    uint8_t             inrep_size;     // The size of data buffers required for holding state and xlate'd EE requests.
    rpm_translate_cb    xlate;          // Callback for converting KVP's to an inrep.
    rpm_apply_cb        apply;          // Callback for applying an inrep "vote" to the resource state.
    void               *cb_data;        // Data saved at resource registration, passed to both callbacks.
    InternalClientType *internal_types; // List of known internal client types (for looking up their inrep_size).
    unsigned            settling_estimate;
} ResourceClassData;

typedef enum
{
    NO_REQUEST           = 0,
    ACTIVE_REQUEST       = 1,
    SLEEP_REQUEST        = 2,
    NEXT_ACTIVE_REQUEST  = 3
} rpm_request_type;

typedef struct EEClient
{
    uint8_t          selected;
	uint8_t 		ee_id;
    rpm_request_type req_type[2];
    void            *request[2];
	struct EEClient *next;
} EEClient;

typedef struct ResourceData
{
    uint8_t         class_idx;     // Index into rpm.classes where this resources class lives.
    uint8_t         shadow_idx;    // Index of this resource's shadow should it have one.
    uint8_t         num_ics;       // Number of internal clients.
	uint8_t         num_eecs;      // Number of EE clients in linked list
    uint32_t        id;            // The 'id' portion of the (type, id) pair identifying this resource.
    void           *current_state; // Pointer to the buffer where the current aggregated state of this resource lives.
    EEClient       *ees;           // Linked list of per-EE request bookkeeping.
    InternalClient *ics;           // Array of internal clients.

    unsigned        estimates[NUM_CLOCK_RATES];
} ResourceData;

typedef struct
{
    unsigned num_ees;             // The number of EEData structures in this configuration.
    unsigned supported_classes;   // The number of ResourceClassData structures in the classes buffer.
    unsigned supported_resources; // The number of ResourceData structures in the resources/seeds buffers.

    // All storage and configuration that is required on a per-EE basis.  Add
    // support for a new EE by simply incrementing the size of this array and
    // filling in the appropriate configuration data.
    EEData *ees;

    // Runtime storage for resource class / instance data.  These need to be
    // sized appropriately for this target (i.e., there need to be enough of
    // each) but do not require any particular customization; they'll be filled
    // in as resources are registered.
    ResourceClassData *classes;   // Stored in order of registration.
    ResourceData      *resources; // Indexed by perfect hash lookup.  See below.

    // First level table for the hash lookup.
    int16_t *resource_seeds;
} SystemData;

extern SystemData * const rpm;

#endif // RPM_CONFIG_H

