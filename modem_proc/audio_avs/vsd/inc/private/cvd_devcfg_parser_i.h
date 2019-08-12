#ifndef __CVD_DEVCFG_PARSER_I_H__
#define __CVD_DEVCFG_PARSER_I_H__

/*
  Copyright (C) 2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/vsd/inc/private/cvd_devcfg_parser_i.h#1 $
  $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "vss_public_if.h"

#ifndef WINSIM
  #include "mmpm.h"
#endif /* !WINSIM */

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/****************************************************************************
 * DEFINITIONS                                                              *
 ****************************************************************************/

typedef struct cvd_devcfg_parser_voice_use_case_t cvd_devcfg_parser_voice_use_case_t;

struct cvd_devcfg_parser_voice_use_case_t
{
  uint16_t num_voice_sessions;
    /**< Number of active voice sessions in the system. */
  uint16_t num_nb_streams;
    /**< Number of narrow-band (8 KHz) streams in the system. */
  uint16_t num_wb_plus_streams;
    /**< Number of wide-band (16 KHz) or higher streams in the system. */
  uint16_t num_nb_vocprocs;
    /**< Number of narrow-band (8 KHz) vocprocs in the system. */
  uint16_t num_wb_plus_vocprocs;
    /**< Number of wide-band (16 Khz) or higher vocprocs in the system. */
  uint32_t tx_topology_id;
    /**< Tx vocproc topology ID. Applicable only if there is a single active  
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint32_t rx_topology_id;
    /**< Rx vocproc topology ID. Applicable only if there is a single active  
         voice session which has a single vocproc attached to it. It is ignored
         otherwise. */
  uint16_t vfr_mode;
    /**< The VFR mode. Applicable only if there is a single active voice 
         session in the system, which has a single vocproc and/or stream 
         attached to it. It is ignored otherwise.
         The supported values:\n
         #VSS_ICOMMON_VFR_MODE_SOFT \n
         #VSS_ICOMMON_VFR_MODE_HARD */
  uint16_t num_timewarp_streams;
    /**< Number of streams with time warping enabled. */
};

#ifndef WINSIM

typedef struct cvd_devcfg_parser_mmpm_core_info_t cvd_devcfg_parser_mmpm_core_info_t;

struct cvd_devcfg_parser_mmpm_core_info_t
{
  MmpmCoreIdType core_id;
    /**< Indicate which H/W core to use. */
  MmpmCoreInstanceIdType instance_id;
    /**< Indicate which instance of the same H/W core. */
};

typedef struct cvd_devcfg_parser_clock_and_bus_config_t cvd_devcfg_parser_clock_and_bus_config_t;

struct cvd_devcfg_parser_clock_and_bus_config_t
{
  uint32_t per_hw_thread_clock_hz;
    /**< Required VDSP per hardware thread clock in Hz. */
  uint32_t cycles_per_thousand_instr_packets;
    /**< Number of clock cycles required to execute 1000 instruction packets. */
  MmpmGenBwReqType bw_requirement;
    /**< Required bandwidth. */
  uint32_t sleep_latency_us;
    /**< Required sleep latency in microseconds. */
};

#endif /* !WINSIM */

/****************************************************************************
 * PROTOTYPES                                                               *
 ****************************************************************************/

/** 
  Initialize the CVD devcfg parser. Must be called before any other CVD devcfg
  APIs can be called, and must be called only once.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_init ( void );

/** 
  Deinitialize the CVD devcfg parser.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_deinit ( void );

#ifndef WINSIM

/** 
  Retrieve the clock and bus configurations for the client specified voice use
  case. Based on the returned configurations, the client can then request 
  resources from MMPM.

  @param[in] use_case                  Client specified voice use case.
  @param[out] ret_clock_and_bus_config Returns the clock and bus configurations.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_clock_and_bus_config ( 
  cvd_devcfg_parser_voice_use_case_t* use_case,
  cvd_devcfg_parser_clock_and_bus_config_t* ret_clock_and_bus_config
);

/** 
  Retrieve the MMPM HW core and core instance information to be used for MMPM 
  registration.

  @param[out] ret_mmpm_core_info Returns the HW core and core instance information.

  @return APR_EOK when successful.
*/
APR_INTERNAL int32_t cvd_devcfg_parser_get_mmpm_core_info ( 
  cvd_devcfg_parser_mmpm_core_info_t* ret_mmpm_core_info
);

#endif /* !WINSIM */

#endif /* __CVD_DEVCFG_PARSER_I_H__ */

