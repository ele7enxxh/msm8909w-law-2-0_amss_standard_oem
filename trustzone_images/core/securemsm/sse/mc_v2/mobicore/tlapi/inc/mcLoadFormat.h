/**
 * @defgroup MCLF   MobiCore Load Format
 *
 * @defgroup MCLF_VER    MCLF Versions
 * @ingroup MCLF
 *
 * @addtogroup MCLF
 * @{
 *
 * MobiCore Load Format declarations.
 *
 * Holds the definitions for the layout of MobiCore Trustlet Blob.
 *
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All rights reserved
 * 
 * The present software is the confidential and proprietary information of
 * TRUSTONIC LIMITED. You shall not disclose the present software and shall
 * use it only in accordance with the terms of the license agreement you
 * entered into with TRUSTONIC LIMITED. This software may be subject to
 * export or import laws in certain countries.
 */
#ifndef MCLOADFORMAT_H_
#define MCLOADFORMAT_H_

#include "mcUuid.h"
#include "mcSuid.h"
#include "mcDriverId.h"

#define MCLF_VERSION_MAJOR   2
#define MCLF_VERSION_MINOR   3
#define MCLF_VERSION_MINOR_CURRENT   3

#define MC_SERVICE_HEADER_MAGIC_BE         ((uint32_t)('M'|('C'<<8)|('L'<<16)|('F'<<24))) /**< "MCLF" in big endian integer representation */
#define MC_SERVICE_HEADER_MAGIC_LE         ((uint32_t)(('M'<<24)|('C'<<16)|('L'<<8)|'F')) /**< "MCLF" in little endian integer representation */
#define MC_SERVICE_HEADER_MAGIC_STR         "MCLF"                                        /**< "MCLF" as string */

/** @name MCLF flags */
/*@{*/
#define MC_SERVICE_HEADER_FLAGS_PERMANENT               (1U << 0) /**< Loaded service cannot be unloaded from MobiCore. */
#define MC_SERVICE_HEADER_FLAGS_NO_CONTROL_INTERFACE    (1U << 1) /**< Service has no WSM control interface. */
#define MC_SERVICE_HEADER_FLAGS_DEBUGGABLE              (1U << 2) /**< Service can be debugged. */
/*@}*/

#if !defined(ADDR_T_DEFINED)
#define ADDR_T_DEFINED
typedef void*    addr_t;                /**< an address, can be physical or virtual */
#endif // !defined(ADDR_T_DEFINED)

/** Service type.
 * The service type defines the type of executable.
 */
typedef enum {
    SERVICE_TYPE_ILLEGAL    = 0,        /**< Service type is invalid. */
    SERVICE_TYPE_DRIVER     = 1,        /**< Service is a driver. */
    SERVICE_TYPE_SP_TRUSTLET   = 2,     /**< Service is a Trustlet. */
    SERVICE_TYPE_SYSTEM_TRUSTLET = 3    /**< Service is a system Trustlet. */
} serviceType_t;

/**
 * Memory types.
 */
typedef enum {
    MCLF_MEM_TYPE_INTERNAL_PREFERRED = 0, /**< If available use internal memory; otherwise external memory. */
    MCLF_MEM_TYPE_INTERNAL = 1, /**< Internal memory must be used for executing the service. */
    MCLF_MEM_TYPE_EXTERNAL = 2, /**< External memory must be used for executing the service. */
} memType_t;

/**
 * Descriptor for a memory segment.
 */
typedef struct {
    addr_t      start;  /**< Virtual start address. */
    uint32_t    len;    /**< Length of the segment in bytes. */
} segmentDescriptor_t, *segmentDescriptor_ptr;

/**
 * MCLF intro for data structure identification.
 * Must be the first element of a valid MCLF file.
 */
typedef struct {
    uint32_t        magic;      /**< Header magic value ASCII "MCLF". */
    uint32_t        version;    /**< Version of the MCLF header structure. */
} mclfIntro_t, *mclfIntro_ptr;

/** @} */


// Version 2 /////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @defgroup MCLF_VER_V2   MCLF Version 32
 * @ingroup MCLF_VER
 *
 * @addtogroup MCLF_VER_V2
 * @{
 */

/**
 * Version 2.1/2.2 MCLF header.
 */
typedef struct {
    mclfIntro_t             intro;           /**< MCLF header start with the mandatory intro. */
    uint32_t                flags;           /**< Service flags. */
    memType_t               memType;         /**< Type of memory the service must be executed from. */
    serviceType_t           serviceType;     /**< Type of service. */

    uint32_t                numInstances;    /**< Number of instances which can be run simultaneously. */
    mcUuid_t                uuid;            /**< Loadable service unique identifier (UUID). */
    mcDriverId_t            driverId;        /**< If the serviceType is SERVICE_TYPE_DRIVER the Driver ID is used. */
    uint32_t                numThreads;      /**<
                                              * <pre>
                                              * <br>Number of threads (N) in a service depending on service type.<br>
                                              *
                                              *   SERVICE_TYPE_SP_TRUSTLET: N = 1
                                              *   SERVICE_TYPE_SYSTEM_TRUSTLET: N = 1
                                              *   SERVICE_TYPE_DRIVER: N >= 1
                                              * </pre>
                                              */
    segmentDescriptor_t     text;           /**< Virtual text segment. */
    segmentDescriptor_t     data;           /**< Virtual data segment. */
    uint32_t                bssLen;         /**< Length of the BSS segment in bytes. MUST be at least 8 byte. */
    addr_t                  entry;          /**< Virtual start address of service code. */
    uint32_t                serviceVersion; /**< Version of the interface the driver exports. */

// These should be put on next MCLF update:
//    mcSuid_t                permittedSuid;  /**< Starting 2.3: If nonzero, suid which is allowed to execute binary */
//    uint32_t                permittedHwCf;  /**< Starting 2.3: If nonzero, hw configuration which is allowed to execute binary */

} mclfHeaderV2_t, *mclfHeaderV2_ptr;
/** @} */


/**
 * Version 2.3 MCLF header.
 */
typedef struct {
    mclfHeaderV2_t          mclfHeaderV2;
    mcSuid_t                permittedSuid;  /**< Starting 2.3: If nonzero, suid which is allowed to execute binary */
    uint32_t                permittedHwCfg; /**< Starting 2.3: If nonzero, hw configuration which is allowed to execute binary */
} mclfHeaderV23_t, *mclfHeaderV23_ptr;
/** @} */

/**
 * Version 2 MCLF text segment header.
 * Required to be present in MobiCore 1.2 components at address (0x1080).
 * This extension is initialized already at trustlet compile time,
 * but may be modified later by configuration tools and by MobiCore at load time.
 */
typedef struct {
    uint32_t                version;        /**< Version of the TextHeader structure. */
    uint32_t                textHeaderLen;  /**< Size of this structure (fixed at compile time) */
    uint32_t                requiredFeat;   /**< Flags to indicate features that Mobicore must understand/interprete when loading.
                                                 Initial value set at compile time.
                                                 Required always. */
    addr_t                  mcLibEntry;     /**< Address for McLib entry.
                                                 Mobicore sets at load time for trustlets / drivers.
                                                 Required always. */
    segmentDescriptor_t     mcLibData;      /**< Segment for McLib data.
                                                 Set at compile time.
                                                 Required always. */
    addr_t                  mcLibBase;      /**< McLib base address.
                                                 Mobicore sets at load time for trustlets / drivers.
                                                 Required always. */
    uint32_t                tlApiVers;      /**< TlApi version used when building trustlet.
                                                 Value set at compile time.
                                                 Required always. */
    uint32_t                drApiVers;      /**< DrApi version used when building trustlet.
                                                 Value set at compile time for drivers. 0 for trustlets.
                                                 Required always. */
} mclfTextHeader_t, *mclfTextHeader_ptr;

// Version 2 ///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @addtogroup MCLF
 * @{
 */

/** MCLF header */
typedef union {
    mclfIntro_t    intro;           /**< Intro for data structure identification. */
    mclfHeaderV2_t mclfHeaderV2;    /**< Version 2 header */
} mclfHeader_t, *mclfHeader_ptr;

// Version 2.3 changes header definition
// Above structure is hard-coded into many places.
// So new changes are made into separate structure.
#define MCLF_HEADER_SIZE_V23 (0x080)

// Actual (known) length can be calculated using macro
#define MCLF_HEADER_SIZE(version) ((version)>0x20002?(MCLF_HEADER_SIZE_V23):sizeof(mclfHeader_t))

// This is only minimum size, so nothing below this makes sense.
#define MCLF_BINARY_MIN_SIZE(version) (MCLF_HEADER_SIZE_V23+sizeof(mclfTextHeader_t))

#endif /* MCLOADFORMAT_H_ */

/** @} */
