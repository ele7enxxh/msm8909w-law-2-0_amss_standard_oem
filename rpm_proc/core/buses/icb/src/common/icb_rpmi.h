#ifndef ICB_RPMI_H
#define ICB_RPMI_H
/*
===========================================================================

FILE:         icb_rpmi.h

DESCRIPTION:  Internal header file for the ICB RPM library.

===========================================================================

                             Edit History

$Header: //components/rel/rpm.bf/2.1.1/core/buses/icb/src/common/icb_rpmi.h#1 $

when         who     what, where, why
----------   ---     ------------------------------------------------------
2014/09/04   sds     Add DSAT per perf level danger modulation.
2014/08/21   sds     Prevent div by 0 when calculating clocks.
2014/06/20   sds     Extend request types to add latency request in addition to BW.
2014/05/20   sds     Change the hardware base address to a uint8_t*.
2014/01/15   sds     Remove unused threshold keys, and support threshold percentages.
2013/04/03   sds     Add APIs for saving and restoring icb hardware.
2011/11/08   sds     Initial revision.

===========================================================================
             Copyright (c) 2011-2014 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
===========================================================================
*/

/* -----------------------------------------------------------------------
** Includes
** ----------------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>
#include "icbid.h"
#include "npa.h"
#include "icb_rpm_soc_data.h"
#include "comdef.h"
#include "Clock.h"
#include "ClockDefs.h"

/* -----------------------------------------------------------------------
** Definitions
** ----------------------------------------------------------------------- */
#define CEILING_DIV(dividend, divisor) (((dividend)+(divisor)-1)/(divisor))
#define CLK_DIV(bps, width, ports)     CEILING_DIV((bps), ((ports)!=0?(ports):1)*(width)*1000)
#define HIGH(val)                      (uint32_t)((val) >> 32)
#define LOW(val)                       (uint32_t)((val) & 0xFFFFFFFFULL)

/* -----------------------------------------------------------------------
** Types
** ----------------------------------------------------------------------- */
/* Clock request information */
typedef struct
{
  char              const *const name;        /**< The npa name of the clock. */
  npa_client_handle              handle;      /**< The npa handle for the clock. */
  uint32_t          const        num_clients; /**< Number of clients on this clock. */
  uint32_t                *const client_reqs; /**< Current client requests. */
} icb_clock_type;

/* Clock handle type. */
typedef struct
{
  icb_clock_type       *const clock; /**< Pointer to the clock struct. */
  uint32_t       const        idx;   /**< Index for this client. */
} icb_clock_handle_type;

/* SoC data format for slaves */
typedef struct
{
  icb_slave_def_type    const        soc_data;       /**< SoC info for this slave. */
  uint32_t              const        slave_width;    /**< The width of the slave interface on the bus in bytes. */
  icb_clock_handle_type const *const bus_clk;        /**< A pointer to the bus clock info struct. */
  uint32_t              const        num_slave_clks; /**< The number items in the slave array. */
  icb_clock_handle_type const *const slave_clks;     /**< A pointer to the array of slave clock info structures. */
} icb_slave_type;

/* Storage for slave client requests */
typedef uint64_t icb_slave_req_type;

/* SoC data format for masters */
typedef struct
{
  icb_master_def_type   const        soc_data;        /**< SoC info for this slave. */
  uint32_t              const        master_width;    /**< The width of the master interface on the bus in bytes. */  
  icb_clock_handle_type const *const bus_clk;         /**< A pointer to the bus clock info struct. */
  uint32_t              const        num_master_clks; /**< The number items in the master clock array. */
  icb_clock_handle_type const *const master_clks;     /**< A pointer to the array of master clock info structures. */
} icb_master_type;

/* Storage for master bandwidth client requests */
typedef uint64_t icb_master_bw_req_type;

/* Storage for spdm clock client requests */
typedef uint32_t icb_spdm_clk_req_type;

/* Bus definition structure for buses to be retained */
typedef struct
{
  char *          pszBaseName;
  uint8_t *       pBaseAddr;
  icb_bus_type    busType;
  uint32_t        uDataWidth;
  void *          pInfo; /* Expected to be BIMC or NOC info type. */
  icb_clock_type *clock; 
} icb_ret_bus_type;

/* Storage for master latency requests */
typedef uint32_t icb_master_latency_req_type;

/* Latency nodes */
typedef struct
{
  uint32_t const               uNumLatencies;
  uint32_t const               uNumFreqs;
  uint16_t const *const        aLatencies;
  uint16_t const *const        aFreqs;
  uint16_t const *const *const aBWs;
} icb_latency_table_type;

typedef struct
{
  const ICBId_MasterType        masterId;
  const ICBId_SlaveType         slaveId;       /**< The slave we depend on */
  const icb_latency_table_type *latency_table; /**< A pointer to the latency table. */
  char const *const             clk;           /**< The npa name of the clock. */
  npa_client_handle             handle;        /**< The npa handle for the clock. */
} icb_master_latency_type;

/* DSAT context structures */
typedef struct
{
  uint32_t const freq; /**< Frequency for this level in KHz. */
  uint32_t const bw;   /**< BW for this level in MBps */
} icb_dsat_freq_bw_type;

typedef struct
{
  HAL_bimc_InfoType *                info;       /**< A pointer to the BIMC info struct. */
  char const *const                  clock;      /**< The clock name associated with this context. */
  ClockIdType                        clock_id;   /**< The clock id for the named clock. */
  ClockCallbackHandle                handle;     /**< The clock callback handle */
  uint32_t const                     port;       /**< The port number. */
  uint32_t const                     context;    /**< The context number. */
  uint32_t const                     gp;         /**< The grant period in microseconds. */
  uint32_t const                     num_levels; /**< The number of freq/bw levels. */
  icb_dsat_freq_bw_type const *const levels;     /**< A pointer to the array of freq/bw levels. */
} icb_dsat_context_type;

/* SoC data format for system information */
typedef struct
{
  uint32_t         const                 num_bus_defs;  /**< Number of buses. */
  icb_bus_def_type const *const *const   bus_defs;      /**< Bus definitions. */
  uint32_t         const                 num_npa_deps;  /**< Number of npa resources in the required array. */
  char             const **              npa_deps;      /**< A pointer to the array of npa resource names. */
  icb_clock_type         *const *const   clocks;        /**< A pointer to the array of icb clocks. */
  uint32_t         const                 num_slaves;    /**< Number of slaves in the system. */
  ICBId_SlaveType  const *const          slave_ids;     /**< A pointer to the array of slave ids. */
  icb_slave_type   const *const *const   slaves;        /**< A pointer to the array of slaves. */
  uint32_t         const                 num_masters;   /**< Number of masters */
  ICBId_MasterType const *const          master_ids;    /**< A pointer to the array of master ids. */
  icb_master_type  const *const *const   masters;       /**< A pointer to the array of masters. */
  uint32_t         const                 num_rtem_clks; /**< Number of realtime spdm clocks. */
  char             const **              rtem_clks;     /**< A pointer to the array of realtime spdm clock names */
  ClockIdType            *const          rtem_clk_ids;  /**< A pointer to the array of realtime spdm clock ids. */
  uint32_t         const                 num_olem_clks; /**< Number of offline spdm clocks. */
  char             const **              olem_clks;     /**< A pointer to the array of offline spdm clock names */
  ClockIdType            *const          olem_clk_ids;  /**< A pointer to the array of offline spdm clock ids. */
  uint32_t         const                 num_ret_defs;  /**< Number of buses requiring retention. */
  icb_ret_bus_type const *const *const   ret_defs;      /**< Retention bus definitions. */
  uint32_t         const                 num_ret_clks;  /**< Number of retention clocks. */
  char             const **              ret_clks;      /**< A pointer to the array of add'l clocks for retention */
  ClockIdType            *const          ret_clk_ids;   /**< A pointer to the array of retention clock ids. */
  uint32_t         const                 num_latency_masters; /**< Number of masters */
  ICBId_MasterType const *const          latency_master_ids; /**< A pointer to the array of master ids. */
  icb_master_latency_type *const *const  latency_masters; /**< A pointer to the array of masters. */
  uint32_t         const                 num_dsat_contexts; /**< Number of danger contexts. */
  icb_dsat_context_type *const *const    dsat_contexts; /** A pointer to the array of danger contexts. */
} icb_system_info_type;

/* -----------------------------------------------------------------------
** Function Declarations (to be implemented by common)
** ----------------------------------------------------------------------- */
/**
@brief This function retrieves a pointer to the target information.

@returns A pointer to the target information, NULL if there was an error.
*/
icb_system_info_type *icb_get_target_resources( void );

/**
@brief This makes the request for the indicated clock.
*/
void icb_clock_request( icb_clock_handle_type const *handle, uint32_t request );

/* -----------------------------------------------------------------------
** Function Declarations (to be implemented by modules)
** ----------------------------------------------------------------------- */
/**
@brief This function initializes the ICB slave aggregator module.
*/
void icb_slaves_init( void );

/**
@brief This function initializes the ICB master aggregator module.
*/
void icb_masters_init( void );

/**
@brief This funciton initializes the ICB SPDM clock request module.
*/
void icb_spdm_req_init( void );

/**
@brief This function initializes the ICB master latency clock request module.
*/
void icb_master_latency_init( void );

/**
@brief This function initializes the ICB DSAT danger modulation module.
*/
void icb_dsat_init( void );

/**
@brief This function is called by monitored slaves to update their bandwidth usage to
       the latency module.
*/
void icb_master_latency_update_bw( ICBId_SlaveType slaveId, icb_slave_req_type req );

#endif /* ICB_RPMI_H */
