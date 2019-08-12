/**
@file sec_debug_policy.h
@brief Trustzone Fuse Provisioning debug policy specific definitions/routines

This file contains the target specific information for debug policy Provisioning

*/

/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/secdbgplcy/inc/sec_debug_policy.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

 when           who         what, where, why
 --------       ---         --------------------------------------------------
 2014/07/08     st          Initial version
=============================================================================*/

#ifndef SEC_DEBUG_POLICY_H
#define SEC_DEBUG_POLICY_H

#define DBG_POLICY_ENABLE_ONLINE_CRASH_DUMPS       0
#define DBG_POLICY_ENABLE_OFFLINE_CRASH_DUMPS      1
#define DBG_POLICY_ENABLE_JTAG                     2
#define DBG_POLICY_ENABLE_LOGGING                  3

//#define DBG_POLICY_HASH_DIGEST_SIZE_SHA256        32
//#define DBG_POLICY_ID_ARRAY_SIZE                  32
#define DBG_POLICY_CERT_ARRAY_SIZE                4

// A policy can apply to a range of msm serial numbers.
// This value limits that range to a default value.
#define DBG_POLICY_SERIAL_NUM_RANGE_LIMIT         1
#define DBG_POLICY_ELF_IMAGE_ID                   0x200
#define DP_ELF_HASH_OFFSET	                  0x1000 // TODO: remove
#define DP_ELF_CONTENT_OFFSET	                  0x3000 // TODO: remove

#define DBG_POLICY_REVISION_NUMBER                 2

//#define DBG_POLICY_RMB_FLAG_ONLINE_CRASH_DUMPS     1

/* Atleast one is required */ 
#define SEC_DBG_SERIAL_NUM_MIN_COUNT 1

/* unique serial numbers that are allowed */
#define SEC_DBG_SERIAL_NUM_MAX_COUNT 200

typedef struct __attribute__((__packed__))  
    {
        uint32 magic; /* Version of Debug Policy */
        uint32 size;  /* Fixed size debug policy*/
        uint32 revision; /* Revision of the debug policy*/
        struct __attribute__((__packed__)){
                uint64 enable_online_crash_dumps   :  1;
                uint64 enable_offline_crash_dumps  :  1;
                uint64 enable_jtag                 :  1;
                uint64 enable_logs                 :  1;
                uint64 reserved_bits               : 44; // reserved for QCT, must be 0
                uint64 oem_reserved_bits           : 16; // reserved for OEM
        } flags;
        uint32 image_id_bitmap; /* Image ID Bit Map */
        uint32 root_cert_hash_count; /* Root Cert Hash Count */
        uint8  root_cert_hash_array[DBG_POLICY_CERT_ARRAY_SIZE][CEML_HASH_DIGEST_SIZE_SHA256]; /* Hash Array*/
        uint32 serial_num_count; /* Serial Number Count */
        uint32 serial_num_array[SEC_DBG_SERIAL_NUM_MAX_COUNT]; /* Serial Numbers */
} dbg_policy_t;

#endif
