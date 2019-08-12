#ifndef _ADSP_AVCS_API_H_
#define _ADSP_AVCS_API_H_

/**
@file adsp_core_api.h

@brief This file contains the structure definitions for the AVCS
commands used in client-AVCS communications.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The core_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      core_mainpage.dox file or contact Tech Pubs.
===========================================================================*/

/*===========================================================================
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*========================================================================
Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/avcs/inc/adsp_core_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
02/27/13   sw      (Tech Pubs) Updated doxy markup/comments for 2.2 PDF.
01/03/12   AP      Created the file
11/30/11   sw      (Tech Pubs) Updated doxy markup/comments for PDF (Rev D)
08/01/11   AP       Added ADSP_CMD_SET_PERIODIC_PROFILING_EVENTS_3 and
                    ADSP_EVENT_PERIODIC_PROFILING_3 API
06/27/11   AZ       Added ADSP_EVENT_GET_STATE and ADSP_EVENT_STATE API
04/27/11   leo     (Tech Pubs) Updated doxy comments for Interface Spec.
12/08/10   VI       Added Remote Bus BW and Power Collapse API
09/22/10   SD      (Tech Pubs) Edited Doxygen markup and comments.
05/21/10   KK      Created file.
========================================================================== */

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @ingroup core_get_version
  Queries the AVS for the build version and versions of static services.

  @apr_hdr_fields
    Opcode -- AVCS_GET_VERSIONS

  @msg_payload
    None.

  @return
    #AVCS_GET_VERSIONS_RSP

  @dependencies
    None.
*/
#define AVCS_GET_VERSIONS       0x00012905

/** @ingroup core_resp_version
  Response to an #AVCS_GET_VERSIONS command, which queries for versions of
  supported services.

  @apr_hdr_fields
    Opcode -- AVCS_GET_VERSIONS_RSP

  @msg_payload{avcs_service_cmd_rsp_get_api_versions_t}
    @tablens{weak__avcs__service__cmd__rsp__get__api__versions__t}

  @par Service information (avcs_svc_info)
    @table{weak__avcs__svc__info}

  @return
    None.

  @dependencies
    None.
*/
#define AVCS_GET_VERSIONS_RSP   0x00012906

/** @ingroup versions
    Version of the AVCS core module. */
#define AVCS_CORE_VERSION   0x00040000

typedef struct avcs_service_cmd_rsp_get_api_versions_t avcs_service_cmd_rsp_get_api_versions_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_service_cmd_rsp_get_api_versions_t
@{ */
/* Payload of the AVCS_GET_VERSIONS_RSP command response, which
    provides a listing of service IDs and their API version numbers. */
/** Immediately following this structure are num_services instances of
    %avcs_svc_info.
*/
struct avcs_service_cmd_rsp_get_api_versions_t
{

    uint32_t                  build_id;
    /**< Must be set to zero. */     
    /* This field might have some meaningful nonzero value once the
       multiprocess design is implemented. */

    uint32_t                  num_services;
    /**< Number of supported services in the current build. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_service_cmd_rsp_get_api_versions_t */

typedef struct avcs_svc_info avcs_svc_info;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_svc_info
@{ */
/* For each instance in the AVCS_GET_VERSIONS_RSP payload, this
  structure contains per-service information.
*/
struct avcs_svc_info
{

    uint32_t                  service_id;
    /**< Service ID.

         @values 0 to 255 */

    uint32_t                  version;
    /**< Service API version.

         @values
         - Bits 0 to 15 -- Minor version
         - Bits 16 to 31 -- Major version @tablebulletend */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_svc_info */

/** @ingroup core_cmd_mem_stats
  Requests information on AVS memory consumption.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_GET_MEMORY_STATS

  @msg_payload
    None.

  @return
   #AVCS_CMDRSP_GET_MEMORY_STATS

  @dependencies
    None.
*/
#define AVCS_CMD_GET_MEMORY_STATS                    0x00012907

/* NOTE: Q9 = QuRT_Elite API Interface Specification (80-N2962-1) */
/** @ingroup core_resp_mem_stats
  In response to the #AVCS_CMD_GET_MEMORY_STATS command, returns statistics
  related to QuRT_Elite memory allocation use. For more information on
  QuRT_Elite, refer to @xhyperref{Q9,[Q9]}.

  This event can be used to detect how fully the AVS heap is being used and
  to track memory allocations/free counts for memory leaks.

  @apr_hdr_fields
    Opcode -- AVCS_CMDRSP_GET_MEMORY_STATS

  @msg_payload{avcs_cmd_rsp_get_memory_stats_t}
    @table{weak__avcs__cmd__rsp__get__memory__stats__t}

  @return
    None.

  @dependencies
    None.
*/
#define AVCS_CMDRSP_GET_MEMORY_STATS                  0x00012908

typedef struct avcs_cmd_rsp_get_memory_stats_t avcs_cmd_rsp_get_memory_stats_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_rsp_get_memory_stats_t
@{ */
/* Payload of the AVCS_CMDRSP_GET_MEMORY_STATS command response, which
  returns statistics related to QuRT_Elite memory allocation use.
*/
struct avcs_cmd_rsp_get_memory_stats_t
{

    uint32_t              num_elite_mallocs;
    /**< Number of Elite memory allocations that have occurred since bootup or
         since statistics were reset. */

    uint32_t              num_elite_frees;
    /**< Number of times Elite memory was freed since bootup or since
         statistics were reset. */

    uint32_t              current_elite_heap_use;
    /**< Current number of bytes allocated from the heap since statistics were
         reset for elite. */

    uint32_t              peak_elite_heap_use;
    /**< Peak number of bytes allocated from the heap since bootup or since
         statistics were reset for elite.*/

    uint32_t              num_non_elite_mallocs;
    /**< Number of non-Elite memory allocations that have occurred since bootup
         or since statistics were reset. */

    uint32_t              num_non_elite_frees;
    /**< Number of times non-Elite memory was freed since bootup or since
         statistics were reset. */

    uint32_t              current_non_elite_heap_use;
    /**< Current number of bytes allocated from the heap since statistics were
         reset for non-elite. */

    uint32_t              peak_non_elite_heap_use;
    /**< Peak number of bytes allocated from the heap since bootup or since
         statistics were reset for non-elite. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_rsp_get_memory_stats_t */

/** @ingroup core_cmd_peak_heap_use
  Requests that the AVS reset the peak heap usage to zero, as reported in
  profiling events and acknowledgments.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_RESET_PEAK_HEAP_USE

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
*/
#define AVCS_CMD_RESET_PEAK_HEAP_USE                  0x00012909

/** @addtogroup core_cmd_profiling
@{ */
/**
  Requests that periodic profiling be provided at regular intervals. This
  command provides additional information: AHB BW, L1 and L2 cache events, and
  AXI BW vote.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS

  @msg_payload{avcs_cmd_get_periodic_profiling_events}
    @table{weak__avcs__cmd__get__periodic__profiling__events}

  @detdesc
    At requested intervals, the AVS writes profiling information to the
    provided memory location and sends the #AVCS_EVENT_PERIODIC_PROFILING
    event.
    @par
    The physical_address field is automatically mapped in the aDSP before
    profiling starts, and is unmapped after profiling is complete. There is no
    need to send memory map commands before this command.
    @par
    The size field indicates how much space is reserved. If the entire
    profiling data does not fit in this field, the profiler sends partial
    profiling information covering as many software threads as will fit. For
    example, 1 KB can fit up approximately 80 profiled software threads.
    Currently, the maximum number of software threads that can be profiled is
    64 (#AVS_MAX_PROFILED_SW_THREADS).

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
*/
#define AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS   0x0001290A

/** Disables profiling. */
#define AVS_PROFILING_OFF                0

/** Enables profiling to collect summary information of MIPS/memory. */
#define AVS_PROFILING_SUMMARY            1

/** Enables profiling to collect summary information of MIPS/memory,
  per-software-thread MIPS, and stack consumption. AXI BW, AHB BW, MPPS,
  and cache miss ratios (L1 I$, L1D$, L2$) are also collected.
*/
#define AVS_PROFILING_PER_THREAD         2

/** Maximum number of software threads to profile. This value limits the size
  of the profiling event message.
*/
#define AVS_MAX_PROFILED_SW_THREADS 64

/** Shared memory is virtually mapped. */
#define SHARED_MEM_PROPERTY_VIRTUAL   1

/** Shared memory is physically mapped. */
#define SHARED_MEM_PROPERTY_PHYSICAL  0

/** @} */ /* end_addtogroup core_cmd_profiling */

typedef struct avcs_cmd_get_periodic_profiling_events avcs_cmd_get_periodic_profiling_events;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_get_periodic_profiling_events
@{ */
/* Payload of the AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS command,
  which requests periodic events with profiling information.
*/
struct avcs_cmd_get_periodic_profiling_events
{

    uint32_t              profiling_level;
    /**< Level of profiling to be collected.

         @values
           - #AVS_PROFILING_OFF
           - #AVS_PROFILING_SUMMARY
           - #AVS_PROFILING_PER_THREAD @tablebulletend */

    uint32_t              physical_address_lsw;
    /**< Lower 32 bits of the physical address of the buffer into which the AVS
         writes the profiling data for each event. */

    uint32_t              physical_address_msw;
    /**< Upper 32 bits of the physical address of the buffer into which the AVS
         writes the profiling data for each event.
         
         The 64-bit address formed by physical_address_lsw and
         physical_address_msw must be 4&nbsp;KB aligned. */

    uint32_t              size;
    /**< Number of bytes available in the shared buffer. This size must be in
         multiples of 4 KB pages. */

    uint32_t              sampling_period;
    /**< Number of microseconds between the profiling measurements.

         @values @ge 100000 */

    uint32_t              property_flag;
   /** Indicates whether the shared memory provided is virtual (0) or physical
       (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_get_periodic_profiling_events */

/* NOTE: Q8 = Hexagon QuRT RTOS User Guide */
/** @ingroup core_event_profiling
  In response to the #AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS command, provides
  statistics for the last measurement event. For detailed information on how to
  process this data, refer to @xhyperref{Q8,[Q8]}.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_PERIODIC_PROFILING

  @msg_payload{avcs_event_periodic_profiling}
    @table{weak__avcs__event__periodic__profiling}

  @par Profiling payload (avcs_event_periodic_profiling_t)
    @tablens{weak__avcs__event__periodic__profiling__t}

  @par Software thread information (weak_avcs_sw_thread_info)
    @table{weak__avcs__sw__thread__info} @newpage

  @par PMU mode information
    A PMU is a hardware block that supports measurements of only four events.
    To calculate different metrics (like AXI BW, AHB BW, MPPS, and Cache miss
    ratios), PMU must be configured to count different events in successive
    profiling events. The pmu_mode field provides the information of which
    events are measured by PMU in a given profiling period.
    @par
    Table @xref{tbl:PmuModeMapping} lists the mapping of pmu_mode to measured
    PMU events.
    @par
    @inputtable{AVS_EVENT_PERIODIC_PROFILING_PMU_mode_info.tex} @newpage

  @par Performance metric formulas
    @xreflabel{hdr:PerfMetricFormulas}
    The parameters in this event allow clients to calculate AXI BW, AHB BW,
    MPPS, and cache miss ratios of the aDSP during any application. The
    following formulas can be used to calculate the different parameters.
      @par
    - AXI Read BW (in KB)
      @code
((axi_read_cached*32) + ((axi_read_total - axi_read_cached) * 4) + /
(1024 * profiling _duration)
      @endcode
      @par
    - AXI Write BW (in KB)
      @code
((axi_write_cached*32) + ((axi_write_total - axi_write_cached) * 4) /
(1024 * profiling _duration)
      @endcode
      @par
    - AHB Read BW (in KB)
      @code
(AHB read request * 4) / (1024 * profiling_duration)
      @endcode
      @par
    - AHB Write BW (in KB)
      @code
(AHB write request * 4) / (1024 * profiling_duration)
      @endcode
      @par
    - L1 I$ Miss Per Instruction Packet
      @code
(L1 I$ miss event) / (number of instructions executed)
      @endcode
      @par
    - L1 D$ Miss Per Instruction Packet
      @code
(L1 D$ miss event) / (number of instructions executed)
      @endcode
      @par
    - L2 Miss Ratio
      @code
(instruction misses + data misses ) / (instruction accesses + data accesses)
      @endcode
      @par
    - MPPS
      @code
(number of packets executed) / (profiling_duration * 1000000)
      @endcode

  @return
    None.

  @dependencies
    None.
    */
#define AVCS_EVENT_PERIODIC_PROFILING   0x0001290B

/** @ingroup core_event_get_state
  Used by AVS clients to query the AVCS for the readiness of AVS static
  services. If the AVCS itself is not ready (it is not registered with APR),
  the event is discarded in the APR layer.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_GET_STATE

  @msg_payload
    None.

  @return
    None.

  @dependencies
    None.
*/
#define AVCS_EVENT_GET_STATE   0x0001290C

/** @ingroup core_event_state
  Used by the AVCS to respond to the #AVCS_EVENT_GET_STATE event.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_STATE

  @msg_payload{avcs_event_state_t}
    @table{weak__avcs__event__state__t}

  @description
    When all static services are ready, the payload is set to 0x1. If a static
    service is not ready, the AVCS either does not respond or it responds with
    a payload other than 0x1.
    @par
    AVS clients can periodically send #AVCS_EVENT_GET_STATE to the AVCS at
    start up. If the AVS services are all up and ready, the AVCS must respond
    with AVCS_EVENT_STATE with a payload of 0x1 to indicate the state.
    @par
    If AVS services are not all up and ready, the AVCS may not respond or it
    may respond with AVCS_EVENT_STATE with a payload other than 0x1. The
    clients must be able to handle this behavior appropriately.
    @par
    If the AVCS service (static service) is not ready (not registered with the
    APR), the APR discards the AVCS_EVENT_GET_STATE message.
    @par
    After receiving the AVCS_EVENT_STATE from the AVCS, with the indication
    that all AVS static services are up and ready, the clients stop sending
    AVCS_EVENT_GET_STATE and ignore any further responses (which could be the
    latent response for previous requests). The clients proceed to excercise
    AVS static services.
    @par
    Currently, the interface does not support query of individual service
    readiness, which is reserved for future requirements. It conveys only the
    overall readiness of all AVS static services.

  @return
    None. @newpage

  @dependencies
    #AVCS_EVENT_GET_STATE event must have been sent by the client.
*/
#define AVCS_EVENT_STATE   0x0001290D

typedef struct avcs_event_periodic_profiling avcs_event_periodic_profiling;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_event_periodic_profiling
@{ */
/* Payload of the AVS_EVENT_PERIODIC_PROFILING event,
   which responds to the AVS_CMD_SET_PERIODIC_PROFILING_EVENTS command.
*/
struct avcs_event_periodic_profiling
{

    uint32_t              profiling_payload_address_lsw;
      /**< Pointer to the lower 32 bits of the physical address of the
           profiling payload (avcs_event_periodic_profiling_t ). */

    uint32_t              profiling_payload_address_msw;
      /**< Pointer to the upper 32 bits of the physical address of the
           profiling payload (avcs_event_periodic_profiling_t ). */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_event_periodic_profiling */

typedef struct avcs_event_periodic_profiling_t avcs_event_periodic_profiling_t;

#include "adsp_begin_pack.h"

/** @ingroup core_event_state
    Version of the periodic profiling interface. */
#define AVCS_API_VERSION_PERIODIC_PROFILING     0x1

/** @weakgroup weak_avcs_event_periodic_profiling_t
@{ */
/* Profiling payload of the AVCS_EVENT_PERIODIC_PROFILING event,
    which provides a periodic update of profiling statistics from the AVS. */
/** The payload contains one instance of this structure, followed by
    profiled_sw_threads instances of avcs_sw_thread_info.
*/
struct avcs_event_periodic_profiling_t
{
    uint16_t              avcs_profiling_minor_version;
   /**< Minor version used for tracking the periodic profiling interface
        version. */

    uint16_t              swthreadinfo_offset;
    /**< Offset in number of bytes from this field to avcs_sw_thread_info of
         profiled software threads. */

    uint32_t              core_clock_freq;
    /**< Configured frequency in kHz of the aDSP core clock. */

    uint32_t              bus_clock_freq;
    /**< Configured frequency in kHz of the aDSP AHB bus clock. */

    uint32_t              wall_clock_ticks;
    /**< Number of ticks from the previous event.

         This is used to estimate the duration since the previous measurement
         event. Each tick is a cycle of the 32768 Hz sleep timer. */

    uint32_t             wall_clock_freq;
    /**< Wall clock frequency in Hz.

         This is used to estimate the duration since the previous measurement
         event.*/

    uint32_t              proc_cycles;
    /**< Number of processor cycles that occurred since the previous
         measurement event. */

    uint32_t              current_elite_heap_use;
    /**< Current number of bytes allocated from the heap. */

    uint32_t              peak_elite_heap_use;
    /**< Peak heap consumption in bytes since bootup or the last
         #AVCS_CMD_RESET_PEAK_HEAP_USE. */

    uint32_t              current_non_elite_heap_use;
    /**< Current number of bytes allocated from the heap for non-Elite memory
         allocations. */

    uint32_t              peak_non_elite_heap_use;
    /**< Peak number of bytes allocated from the heap since bootup for
         non-Elite memory allocations. */

    uint32_t              tot_num_hw_threads;
    /**< Number of hardware threads on the existing platform.

         @values 1 to 4 */

    uint32_t              hw_idle_cycles[4];
    /**< Per hardware thread, the number of idle cycles since the last
         measurement event.

         The array size of 4 accommodates all anticipated platforms to be
         supported. */

    uint32_t              profiled_sw_threads;
    /**< Number of profiled software threads.

         @values
         - 0 -- Software thread profiling is not present
         - > 0 -- Number of active software threads in the AVS

         The payload for this event comprises one instance of
         avcs_event_periodic_profiling_t, followed immediately by
         profiled_sw_threads number of instances of avcs_sw_thread_info. */

    uint32_t              pmu_event_counters[4];
    /**< Counts the occurrence of configured PMU events. */

    uint32_t              pmu_mode;
    /**< Specifies the events that are measured in PMU mode in a given
         profiling period.

         @values
         - 0 -- AXI BW
         - 1 -- AHB BW
         - 2 -- L1 cache events, MPPS
         - 3 -- L2 cache events
         - 4 -- Events related to the thread's running cycles
         - 5 -- Events related to the thread loading in Dynamic Multi-Threading
                (DMT) mode @tablebulletend */

    uint32_t              axi_bw_vote_ab;
    /**< AXI average bandwidth vote in bytes per second (for more information,
         refer to @xhyperref{Q6,[Q6]}).    */
    /* NOTE: Q6 = Hexagon Multimedia: Elite Firmware Architecture Description */

    uint32_t              axi_bw_vote_ib;
    /**< AXI instantaneous bandwidth vote in bytes per second (for more
         information, refer to @xhyperref{Q6,[Q6]}). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_event_periodic_profiling_t */

typedef struct avcs_sw_thread_info avcs_sw_thread_info;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_sw_thread_info
@{ */
/* Per-software-thread profiling information that is part of the
  avcs_event_periodic_profiling and avcs_event_periodic_profiling_t
  payload structures.
*/
struct avcs_sw_thread_info
{

    uint32_t              thread_id;
      /**< Unique identifier of a software thread.

           The ID comprises the first four ASCII characters of the thread name.
           It can be formatted as a string and displayed after appending the
           EOS character. */

    uint32_t              run_cycles;
      /**< Number of cycles this thread has been actively running on a hardware
           thread since the last measurement event. */

    uint16_t              stack_size;
      /**< Allocation size in bytes of this thread stack. */

    uint16_t              stack_free;
      /**< Estimated stack amount that has not been touched since thread
           inception. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_sw_thread_info */

typedef struct avcs_event_state_t avcs_event_state_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_event_state_t
@{ */
/* Payload of the AVCS_EVENT_STATE event, which responds to an
    AVCS_EVENT_GET_STATE event sent by clients.
*/
struct avcs_event_state_t
{

    uint32_t              state;
    /**< Indicates the AVS state.

         @values
         - 0x1 -- AVS services are up and ready
         - Other values -- Reserved for future use; ignore any other values @tablebulletend */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_event_state_t */

/** @ingroup core_cmd_dolby_license
  Sets the license information for Dolby<sup>&reg;</sup> modules.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_SET_DOLBY_LICENSE

  @msg_payload{avcs_service_cmd_set_dolby_license_t}
    @table{weak__avcs__service__cmd__set__dolby__license__t}

  @par Payload format
    @inputtable{AVCS_CMD_SET_DOLBY_LICENSE_Payload_format.tex}

  @description
    To enable the modules provided by Dolby (like the Dolby Digital Plus
    decoder and the Dolby Audio Kernel), valid license information must be
    sent to the aDSP. The license information consists of two parts: ID
    information (such as the manufacturer ID) and a license file.
    @par
    @keep{8} This command allows the client to send this license information to
    the aDSP before opening any sessions that use the Dolby modules. All the
    license information is sent at one time. A success result from the aDSP
    indicates that the license information was successfully stored internally.
    It does not indicate whether the aDPS found the license to be valid.
    @par
    This command can be called only once.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
*/
#define AVCS_CMD_SET_DOLBY_LICENSE               0x0001290E

typedef struct avcs_service_cmd_set_dolby_license_t avcs_service_cmd_set_dolby_license_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_service_cmd_set_dolby_license_t
@{ */
/* Payload of the AVCS_CMD_SET_DOLBY_LICENSE command, which sends
  Dolby license information to the AVS.
*/
struct avcs_service_cmd_set_dolby_license_t
{
    uint32_t                  payload_addr_lsw;
    /**< Lower 32 bits of the physical address of the payload containing the
         license information. */

    uint32_t                  payload_addr_msw;
    /**< Upper 32 bits of the physical address of the payload containing the
         license information.

         The 64-bit address formed by payload_addr_lsw and payload_addr_msw
         must be 4 KB aligned.*/

    uint32_t                  num_id_params;
    /**< Number of ID-related parameters in the payload.

         The payload format is an array of ID-related parameters for the
         license. */

    uint32_t                  license_data_size;
    /**< Size in bytes of the license data in the payload.

         The payload format is an array of of the contents of the license file.
         The size is given in the APR message. See the <b>Payload format</b>
         table below. */

    uint32_t                 property_flag;
   /**< Indicates whether the shared memory provided is virtual (0) or physical
       (1). */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_service_cmd_set_dolby_license_t */

/** @ingroup core_cmd_dolby_set_mfg_id
  Sets the Manufacturer ID information for Dolby modules.

@apr_hdr_fields
   Opcode -- AVCS_CMD_SET_DMID

  @msg_payload{avcs_service_cmd_set_dmid_t}
    @table{weak__avcs__service__cmd__set__dmid__t}
    
  @description
    To enable the modules provided by Dolby (like the Dolby Digital Plus
    decoder and the Dolby Audio Processing), a valid manufacturer ID must be
    sent to the aDSP.
      @par
    This command allows the client to send the manufacturer ID to the
    aDSP before opening any sessions that use the Dolby modules. This
    manufacturer ID is validated against the actual license present in the
    aDSP image (the OEM licensee modifies the aDSP image with the actual
    license file provided by Dolby.
    @par
    A success result from the aDSP indicates that the manufacturer ID was
    properly communicated to aDSP. It does not indicate
    whether the aDSP found the manufacturer ID to be valid.
      @par
    This command can be called multiple times, with only the first call being
    accepted by the aDSP. Subsequent calls before aDSP shutdown are ignored.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
*/
#define AVCS_CMD_SET_DMID               0x00012918

typedef struct avcs_service_cmd_set_dmid_t avcs_service_cmd_set_dmid_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_service_cmd_set_dmid_t
@{ */
/* Payload of the AVCS_CMD_SET_DMID command, which sends
  Dolby mfg id information to the AVS.
*/
struct avcs_service_cmd_set_dmid_t
{
    uint32_t                  dm_id;  /**< Manufacturer ID. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_service_cmd_set_dmid_t */


/** @ingroup core_cmd_get_low_power_segment
  Gets low-power memory segment information.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_GET_LOW_POWER_SEGMENTS_INFO

  @msg_payload
    None.

  @description
    This command is used to query for the physical addresses of memory sections
    that would be advantageous to move to On-Chip Memory (OCMEM) during a
    low-power, single-stream audio use case and also the required OCMEM
    read/write bandwidth to support the use case.
    @par
    A low-power audio use case is one in which a single stream playback or a
    gapless playback is done on a single SLIMbus@reg device or a single I2S
    device. The content must be 16-bit mono/stereo in PCM/MP3/AAC/AAC+/eAAC+
    formats only.

  @return
    #AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO

  @dependencies
    None.
*/
#define AVCS_CMD_GET_LOW_POWER_SEGMENTS_INFO              0x00012903

/** @ingroup core_cmd_resp_low_power_segment
  In response to the #AVCS_CMD_GET_LOW_POWER_SEGMENTS_INFO command, returns low
  power memory segment information.

  @apr_hdr_fields
   Opcode -- AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO

  @msg_payload{avcs_cmd_rsp_get_low_power_segments_info_t}
    @tablens{weak__avcs__cmd__rsp__get__low__power__segments__info__t}

  @par Payload format (avcs_mem_segment_t)
    @table{weak__avcs__mem__segment__t} @newpage

  @description
    This response provides the list of memory sections requested
    to be mapped to OCMEM for low-power audio playback.

  @return
    None.

  @dependencies
    #AVCS_CMD_GET_LOW_POWER_SEGMENTS_INFO must have been sent by the client.
*/
#define AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO           0x00012904

typedef struct avcs_cmd_rsp_get_low_power_segments_info_t avcs_cmd_rsp_get_low_power_segments_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_rsp_get_low_power_segments_info_t
@{ */
/* Payload of the AVCS_CMDRSP_GET_LOW_POWER_SEGMENTS_INFO structure.
*/
/** This payload comprises one instance of
    %avcs_cmd_rsp_get_low_power_segments_info_t, followed immediately by
    num_segments number of instances of %avcs_mem_segment_t.
*/
struct avcs_cmd_rsp_get_low_power_segments_info_t
{
    uint32_t              num_segments;
      /**< Number of segments in this response.

         @values
         - 0 -- There are no known sections to be mapped from DDR to OCMEM
         - > 0 -- The number of memory segments in the list @tablebulletend */

    uint32_t              bandwidth;
    /**< Required OCMEM read/write bandwidth (in bytes per second) if OCMEM is
         granted.

         @values
         - 0 -- If num_segments = 0
         - > 0 -- If num_segments > 0 @tablebulletend */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_rsp_get_low_power_segments_info_t */

/** @addtogroup core_cmd_resp_low_power_segment
@{ */
/** Type of memory segment is read only. */
#define READ_ONLY_SEGMENT      1

/** Type of memory segment is read write. */
#define READ_WRITE_SEGMENT     2

/** Category of memory segment is audio. */
#define AUDIO_SEGMENT          1

/** Category of memory segment is OS (for Hexagon). */
#define OS_SEGMENT             2

/** @} */ /* end_addtogroup core_cmd_resp_low_power_segment */

typedef struct avcs_mem_segment_t avcs_mem_segment_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_mem_segment_t
@{ */
/* Payload format for AVS low power memory segment structure.
*/
struct avcs_mem_segment_t
{
    uint16_t              type;
      /**< Indicates which type of memory this segment is.
           Allowed values: READ_ONLY_SEGMENT or READ_WRITE_SEGMENT only. */

    uint16_t              category;
      /**< Indicates whether audio or OS segments.
           Allowed values: AUDIO_SEGMENT or OS_SEGMENT only. */

    uint32_t              size;
      /**< Size (in bytes) of this segment.
           Will be a non-zero value.*/

    uint32_t              start_address_lsw;
      /**< Lower 32 bits of the 64-bit physical start address
           of this segment. */

    uint32_t              start_address_msw;
      /**< Upper 32 bits of the 64-bit physical start address
           of this segment. */
}
#include "adsp_end_pack.h"
;
/** @} */  /* end_weakgroup weak_avcs_mem_segment_t */

/** @ingroup core_cmd_request_remote_avtimer_vote
  Handles audio video timer (avtimer) vote requests from clients.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST

  @msg_payload{avcs_cmd_remote_avtimer_vote_request}
    @table{weak__avcs__cmd__remote__avtimer__vote__request}
   
  @description
    Clients can use this command to request the avtimer be enabled.
    Remote clients can access the avtimer only after this vote request.
    @par
    The AVS returns a handle to the avtimer in response to this command. 
    The client must ensure that it votes the avtimer only once. 
    @par
    After the use case finishes, the client must request
    #AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST to release the avtimer.
    @par
    The client must not send multiple vote request commands. If the client
    sends multiple vote commands, the same avtimer handle will be sent in
    response.

  @return
    #AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST

  @dependencies
    None.
*/
#define AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST              0x00012914

typedef struct avcs_cmd_remote_avtimer_vote_request avcs_cmd_remote_avtimer_vote_request_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_remote_avtimer_vote_request
@{ */
/* Payload of the AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST command.
*/
struct avcs_cmd_remote_avtimer_vote_request
{
   char client_name[8];
   /**< Client name, up to a maximum of eight characters.
        The avtimer driver takes the first six characters. */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_remote_avtimer_vote_reques */

/** @ingroup core_cmd_resp_request_remote_avtimer_vote
  In response to the #AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST command, sends the
  handle to the avtimer.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST

  @msg_payload{avcs_cmd_rsp_remote_avtimer_vote_request}
    @table{weak__avcs__cmd__rsp__remote__avtimer__vote__request}

  @description
    After the use case finishes, the client must request
    #AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST to release the avtimer.

  @return
    None.

  @dependencies
    #AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST must have been sent by the client.
*/
#define AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST              0x00012915

typedef struct avcs_cmd_rsp_remote_avtimer_vote_request avcs_cmd_rsp_remote_avtimer_vote_request_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_rsp_remote_avtimer_vote_request
@{ */
/* Payload of the AVCS_CMD_RSP_REMOTE_AVTIMER_VOTE_REQUEST command.
*/
struct avcs_cmd_rsp_remote_avtimer_vote_request
{
   uint32_t avtimer_handle;
   /**< Handle of the avtimer. */     
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_rsp_remote_avtimer_vote_request */

/** @ingroup core_cmd_request_remote_avtimer_release
  Handles client requests to disable the avtimer. Clients must send the valid
  avtimer handle.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST

  @msg_payload{avcs_cmd_remote_avtimer_release_request}
    @table{weak__avcs__cmd__remote__avtimer__release__request}

  @return
    None.

  @dependencies
    The #AVCS_CMD_REMOTE_AVTIMER_VOTE_REQUEST must have been sent by the
    client.
*/
#define AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST              0x00012916

typedef struct avcs_cmd_remote_avtimer_release_request avcs_cmd_remote_avtimer_release_request_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_remote_avtimer_release_request
@{ */
/* Payload of the AVCS_CMD_REMOTE_AVTIMER_RELEASE_REQUEST command.
*/
struct avcs_cmd_remote_avtimer_release_request
{
   uint32_t avtimer_handle;
      /**< Handle of the avtimer. */     
}
#include "adsp_end_pack.h"
;


/**
  Used to send license information.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_SET_LICENSE

  @return
    None.

  @dependencies
    None.
*/
#define AVCS_CMD_SET_LICENSE                               0x00012919

typedef struct avcs_cmd_set_license avcs_cmd_set_license_t;

#include "adsp_begin_pack.h"

/* Payload header of the AVCS_CMD_SET_LICENSE command.
*/
struct avcs_cmd_set_license
{
   uint32_t id; /**< A unique ID used to refer to this license */
   uint32_t overwrite; /**< 0 = do not overwrite an existing license with this id.
                            1 = overwrite an existing license with this id. */
   uint32_t size; /**< Size in bytes of the license data following this header. */
}
#include "adsp_end_pack.h"
;

/**
  Used to query for license validation results.

  @apr_hdr_fields
   Opcode -- AVCS_CMD_GET_LICENSE_VALIDATION_RESULT

  @return
    None.

  @dependencies
    A license with the given ID must have been set. The aDSP must have
    a function which validates this license.
*/
#define AVCS_CMD_GET_LICENSE_VALIDATION_RESULT        0x0001291A

typedef struct avcs_cmd_get_license_validation_result avcs_cmd_get_license_validation_result_t;

#include "adsp_begin_pack.h"

/* Payload of the AVCS_CMD_GET_LICENSE_VALIDATION_RESULT command.
*/
struct avcs_cmd_get_license_validation_result
{
   uint32_t id; /**< A unique ID used to refer to this license */
}
#include "adsp_end_pack.h"
;

/**
  Returns the license validation results to the client.

  @apr_hdr_fields
   Opcode -- AVCS_CMDRSP_GET_LICENSE_VALIDATION_RESULT

  @return
    None.

  @dependencies
    AVCS_CMD_GET_LICENSE_VALIDATION_RESULT must have been sent.
*/
#define AVCS_CMDRSP_GET_LICENSE_VALIDATION_RESULT        0x0001291B

typedef struct avcs_cmdrsp_get_license_validation_result avcs_cmdrsp_get_license_validation_result_t;

#include "adsp_begin_pack.h"

/* Payload header of the AVCS_CMD_GET_LICENSE_VALIDATION_RESULT command.
*/
struct avcs_cmdrsp_get_license_validation_result
{
   uint32_t result; /**< ADSP_EOK if the license validation result was successfully retrieved.
                         ADSP_ENOTEXIST if there is no license with the given id.
                         ADSP_ENOTIMPL if there is no validation function for a license with this id. */
   uint32_t size; /**< Length in bytes of the validation result that follows this structure. */
}
#include "adsp_end_pack.h"
;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ACS_API_H_ */
