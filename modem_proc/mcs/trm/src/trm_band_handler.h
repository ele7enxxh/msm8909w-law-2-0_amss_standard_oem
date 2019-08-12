#ifndef TRM_BAND_HANDLER_H
#define TRM_BAND_HANDLER_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

              Transceiver Resource Manager Band Handler Header File

GENERAL DESCRIPTION

  This file provides some declarations for TRM band handler.


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trm_band_handler.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
12/11/2014   sk      Band compatibility check enhancement changes(768580)
11/25/2014   sr      Cache support for quicker band concurrency check(CR:754184).
09/29/2014   mn      Added a new TRM_DIVERSITY_IRAT_MEASUREMENT reason (CR: 712921).
08/12/2014   mn      Support for a unified IRAT mechanism which works across all 
                      TRM modes (CR: 705286).
08/04/2014   mn      Increase the size of the concurrency restriction hash 
                      table (CR: 703878).
08/01/2014   mn      Restriction in the size of the hash table for concurrency 
                      restrictions in TRM causes a crash. (CR: 702823).
07/17/2014   mn      TRM should map 1x and GSM2 to chain 2 when LTE is holding 
                      both chain 0 and chain 1 (CR: 695108).
04/11/2014   mn      TRM re-factoring. 

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"
#include "modem_mcs_defs.h"

extern "C"
{
  #include "trm.h"
}

#include "trmi.h"
#include "queue.h"

#define TRM_CONCURRENCY_RESTRICTIONS_MAX_SIZE      1200

/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

typedef struct trm_band_concurrency_index_entry_type
{
  /* Link */
  q_link_type                             link;

  /* Pointer to the concurrency restriction table entry */
  const rfm_concurrency_restriction_type* table_entry;
} trm_band_concurrency_index_entry_type;

typedef struct trm_band_3_way_concurrency_index_entry_type
{
  /* Pointer to the concurrency restriction table entry */
  const rfm_3mode_support_type* table_entry;

  /* Pointer to the next concurrency restriction index entry */
  trm_band_3_way_concurrency_index_entry_type* next;
} trm_band_3_way_concurrency_index_entry_type;

/* Structure to log band information of a client */
typedef PACK( struct )
{  
  /* Band Information */
  int32    bands[MAX_BAND_INFO];
  
  /* Number of bands */
  uint32                num_bands;
} trm_log_band_info_type;

/*-----------------------------------------------------------------------------
  TRM Client Band Class

    Class which handles band class used/requested by clients.

-----------------------------------------------------------------------------*/
class TRMClientBandClass
{
  private:
    trm_band_t bands[MAX_BAND_INFO];
    uint32     num_bands;

  public:
    TRMClientBandClass(void);
    ~TRMClientBandClass(void);

    void operator = (TRMClientBandClass     other_band_class);
    void operator = (trm_freq_input_type*   freq_info);
    boolean operator != (trm_freq_input_type  other_band);
    boolean operator == (trm_band_t  other_band);
    trm_band_t operator[](uint8 i) const ;
    void get_freq_info(trm_freq_input_type* freq_info);
    trm_band_t  get_home_band(void);
    void get_log_info(trm_log_band_info_type* log_info) const;
};

/*----------------------------------------------------------------------------
  Band class information - 8 bit struct
----------------------------------------------------------------------------*/
typedef struct
{
  TRMClientBandClass      bc_current;    /* Current Band          */
  TRMClientBandClass      bc_requested;  /* Requested Band        */
  TRMClientBandClass      bc_previous;   /* Previous Band         */
  trm_bc_enum_t           bc_state;      /* Band Class State Enum */
  trm_client_enum_t       bc_waiting_on; /* Client waiting on ... */
  boolean                 bc_registered;
  boolean                 reserved;      /* Reserved */
  /* Until this, logging have been modified to work as 16 bytes.
     The modification was needed to keep it backward compatible
     with NikeL where trm_band_t was 32 bits while it is 16 bits
     on Dime. Any future modifications would need either 8 byte
     boundary rule to work seamlessly w.r.t. logging. */
} trm_bc_info_type;

/*----------------------------------------------------------------------------
  Transceiver RF mode mapping information
----------------------------------------------------------------------------*/
typedef struct
{
  /* RF Mode for this object */
  rfm_mode_enum_type rfmode;

  /* Band class information */
  trm_bc_info_type   bc_info;
}trm_rf_mode_map_type;

/*----------------------------------------------------------------------------
  Frequency Information
----------------------------------------------------------------------------*/
typedef struct
{
  /* Band Information */
  trm_band_t            band;

  /* Band allowed flag */
  boolean               allowed;
} trm_band_handler_freq_info_type;

/*----------------------------------------------------------------------------
  Frequency Input Information
----------------------------------------------------------------------------*/
typedef struct
{
  /* Number of bands */
  uint32                          num_bands;

  /* Band Information */
  trm_band_handler_freq_info_type bands[MAX_BAND_INFO];
} trm_band_handler_freq_input_type;

/*----------------------------------------------------------------------------
  Client1 info for compatibility check
----------------------------------------------------------------------------*/
typedef struct
{
  trm_client_enum_t                 id;
  trm_client_enum_t                 measured_client;
  trm_band_handler_freq_input_type  freq_input;
  rfm_device_enum_type              rf_device;
} trm_band_compatibility_client1_input_type;

/*----------------------------------------------------------------------------
  Activity compatibility check input time
----------------------------------------------------------------------------*/
typedef struct
{
  trm_band_compatibility_client1_input_type client1_info;
  trm_client_enum_t                         client2;
}trm_band_compatibility_check_input_type;

/*----------------------------------------------------------------------------
  Transceiver RF mode mapping information
----------------------------------------------------------------------------*/
typedef struct
{
  /* Chain whose capability needs to be checked */
  trm_chain_enum        chain;

  /* client requesting band info */
  trm_client_enum_t     client;

  /* resource type being requested */
  trm_resource_enum_t   resource;

  /* Number of bands */
  uint32                num_bands;

  /* Band information */
  trm_band_handler_freq_info_type bands[TRM_DEVICE_MAPPING_INPUT_SIZE];

} trm_chain_band_capability_input_type;

/*----------------------------------------------------------------------------
  Restriciton table entry data info 
----------------------------------------------------------------------------*/
typedef struct
{
   uint8    dev1;
   uint8    mode1;
   uint8    band1;
   uint8    dev2;
   uint8    mode2;
   uint8    band2;
}trm_rf_restriction_info_type;

#define TRM_CACHE_NUM (RFM_NUM_MODES*(RFM_NUM_MODES+1))/2

#define TRM_CACHE_SIZE_MAX   10

/*----------------------------------------------------------------------------
  Cache entry data
----------------------------------------------------------------------------*/
typedef struct
{
   /* restriction element */
   uint64   element;

   /* compatibility flag */
   boolean  is_compatible;
}trm_band_cache_entry_type;

/*----------------------------------------------------------------------------
  Cache class
----------------------------------------------------------------------------*/
class TRMBandCompatibilityCache
{
  /* array of cache elements */
  trm_band_cache_entry_type cache_array[TRM_CACHE_SIZE_MAX];
  /* num of valid elements */
  uint8     num_elements;
  /* inserting index */
  uint8     insert_index;
public:
  void      insert(uint64, boolean);
  boolean   find(uint64, uint64, boolean&);
};

/*----------------------------------------------------------------------------
  Restriction tree class
----------------------------------------------------------------------------*/
class TRMConcRestrictionTree
{
   /* 64-bit mask created from rf retriction entry */
   uint64      restriction_entry;

   TRMConcRestrictionTree *left_ptr;
   TRMConcRestrictionTree *right_ptr;

public:

   TRMConcRestrictionTree(void);
   TRMConcRestrictionTree(uint64);
   void delete_node(void);
   void insert_left( TRMConcRestrictionTree* );
   void insert_right( TRMConcRestrictionTree* );
   boolean find( uint64& element );
  
};


/*-----------------------------------------------------------------------------
  TRM Band Handler class.

    Class which handles band compatibility checks.

-----------------------------------------------------------------------------*/
class TRMBandHandler
{
private:

  /* cache objects based on the mode comination */
  TRMBandCompatibilityCache                     band_cache[TRM_CACHE_NUM];

  /* Restriciton trees:
      Normal concurrency restriciton tree, entrees with no ALL_DEV/ALL_BANDS */
  TRMConcRestrictionTree                        *concurrency_restriction;

    /* Restriciton trees:
      MISC concurrency restriciton tree, the entires with ALL_BANDS/ALL_DEV info */
  TRMConcRestrictionTree                        *misc_all_restriction;

  /* Dev incompatibilty array */
  uint8                                         dev_restriction[TRM_MAX_CHAINS][TRM_MAX_CHAINS];
  
  /* counts for each restriction tpe entrees */
  int32                                         num_conc_restrictions;
  int32                                         num_dev_restrictions;
  int32                                         num_misc_restrictions;

  /* bitmask to represent what all type of restrictions exists for different mode combinations */
  uint8                                         restriction_type[RFM_NUM_MODES][RFM_NUM_MODES];
  
  /* pointer to the band cache objects */
  TRMBandCompatibilityCache*                   band_cache_ptr[RFM_NUM_MODES][RFM_NUM_MODES];

  trm_band_3_way_concurrency_index_entry_type*  three_way_concurrency_index[RFM_NUM_MODES];
  int32                                         num_three_way_restrictions;
  uint32                                        num_restrictions_failed;

  /* populate trees with restriction entries */
  void populate_restriction_tables(uint64*, uint64*, rfm_devices_configuration_type* );
  boolean check_conc_restriction( trm_rf_restriction_info_type* ,
                                    trm_freq_input_type* ,
                                    trm_freq_input_type* );
  boolean check_misc_restriction( trm_rf_restriction_info_type* ,
                                    trm_freq_input_type* ,
                                    trm_freq_input_type* );
  
public:
  TRMBandHandler(void);

  ~TRMBandHandler(void);

  void de_initialize(void);

  void initialize(rfm_devices_configuration_type* config);

  boolean allocate_3_way_conc_restriction_index_entry
  (
    trm_band_3_way_concurrency_index_entry_type** head,
    const rfm_3mode_support_type* restriction_table
  );

  boolean is_band_compatible( trm_client_enum_t client1, trm_client_enum_t client2 );
  boolean is_band_compatible( trm_band_compatibility_check_input_type* input );

  rfm_device_enum_type get_preferred_device
  ( 
    trm_rf_mode_map_type* rf_map1, 
    trm_rf_mode_map_type* rf_map2, 
    trm_rf_mode_map_type* rf_map3 
  );

  trm_chain_enum get_chain_from_band
  (
    trm_client_enum_t     client,     
    trm_resource_enum_t   resource,
    trm_band_t            band       
  );

  trm_chain_enum get_chain_from_band
  (
    trm_client_enum_t     client,   
    trm_resource_enum_t   resource, 
    trm_freq_input_type  freq_info  
  );

  boolean are_bands_supported_on_chain
  (
    trm_chain_band_capability_input_type *input
  );

  boolean is_chain_dual_receive_capable
  (
    const trm_chain_band_capability_input_type *input
  );

  boolean check_band_mask
  (
    uint64 *device_band_map,
    uint64 band
  );

};

/*============================================================================

Member Function trm_band_handler_initialize_freq_input

Description
  Translation function from trm_freq_input_type to trm_band_handler_freq_input_type
  
Dependencies
  None

Return Value
  None

Side Effects
  None

============================================================================*/
void trm_band_handler_initialize_freq_input
(
  trm_freq_input_type*              input,
  trm_band_handler_freq_input_type* output
);

#endif /* TRM_BAND_HANDLER_H */
