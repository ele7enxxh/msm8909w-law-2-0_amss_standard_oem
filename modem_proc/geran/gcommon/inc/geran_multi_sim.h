#ifndef __GERAN_MULTI_SIM_H
#define __GERAN_MULTI_SIM_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            GERAN MULTI SIM HEADER

GENERAL DESCRIPTION
  Provides common definitions for use by GERAN entities supporting Multi SIM

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2014 Qualcomm Technologies, Inc.

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gcommon/inc/geran_multi_sim.h#1 $
$DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
20/08/14    am      CR712143 Initialize conversion arrays as_id_to_gas_id & gas_id_to_as_id for GPLT
08/08/14    ka      CR706866 FR18739: DR-DSDS RxD Idle, part 2 - new TRM clients
06/08/14    ka      CR699964 Implement Idle RXD support in GL1
31/07/14    cos     CR697355 Bolt 2.0 GL1 updates required for enabling RxD cal with the new DR-DSDS TRM apis
24/07/14    ka      CR692214 TRM APS feature changes required with new TRM api's
16/06/14    cs      Updated for new TRM API
31/05/13    SJW     Updated for Triple SIM
28/11/12    SJW     Created from legacy geran_dual_sim.h
dd/mm/yy    SJW     Sample text for edit history
===========================================================================*/
#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif
#include "sys.h"
#include "geran_dual_sim.h"
#include "trm.h"

#ifdef FEATURE_TRIPLE_SIM
  #define NUM_GERAN_DATA_SPACES    3
#else
  #ifdef FEATURE_DUAL_SIM
  #define NUM_GERAN_DATA_SPACES    2
  #else
  #define NUM_GERAN_DATA_SPACES    1
  #endif /* FEATURE_DUAL_SIM */
#endif /* FEATURE_TRIPLE_SIM */

/* GERAN_ACCESS_STRATUM_ID_MAX represents the highest **valid** GAS ID */
#define GERAN_ACCESS_STRATUM_ID_MAX ((gas_id_t)(NUM_GERAN_DATA_SPACES-1))

/* set of integers that can be used as data space array indices */
#define GERAN_DATA_SPACE_INDEX_1 0
#define GERAN_DATA_SPACE_INDEX_2 1
#define GERAN_DATA_SPACE_INDEX_3 2

#ifdef FEATURE_GSM_RX_DIVERSITY
#define GERAN_RX_DIV_DATA_INDEX_1 2
#define GERAN_RX_DIV_DATA_INDEX_2 3
#endif /* FEATURE_GSM_RX_DIVERSITY */

/* casting conversion for Data Space Indices to GAS IDs */
#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#define GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gid) (geran_get_data_space_index(gid))
#define GERAN_MAP_DATA_SPACE_INDEX_TO_GAS_ID(dsi) ((gas_id_t)(dsi))
#else
#define GERAN_MAP_GAS_ID_TO_DATA_SPACE_INDEX(gid) (GERAN_DATA_SPACE_INDEX_1)
#define GERAN_MAP_DATA_SPACE_INDEX_TO_GAS_ID(dsi) (GERAN_ACCESS_STRATUM_ID_1)
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#if NUM_GERAN_DATA_SPACES == 3
#define INITIAL_VALUE(data)         data, data, data
#define INITIAL_VALUE_S(data)       {data}, {data}, {data}
#define INITIAL_VALUE_DIFFERENT(data1, data2, data3)     data1, data2, data3
#define INITIAL_VALUE_DIFFERENT_S(data1, data2, data3)     {data1}, {data2}, {data3}
#define INITIAL_VALUE_INDEXED(data) data[GERAN_DATA_SPACE_INDEX_1], data[GERAN_DATA_SPACE_INDEX_2], data[GERAN_DATA_SPACE_INDEX_3]
#elif NUM_GERAN_DATA_SPACES == 2
#define INITIAL_VALUE(data)         data, data
#define INITIAL_VALUE_S(data)       {data}, {data}
#define INITIAL_VALUE_DIFFERENT(data1, data2, data3)     data1, data2
#define INITIAL_VALUE_DIFFERENT_S(data1, data2, data3)     {data1}, {data2}
#define INITIAL_VALUE_INDEXED(data) data[GERAN_DATA_SPACE_INDEX_1], data[GERAN_DATA_SPACE_INDEX_2]
#else
#define INITIAL_VALUE(data)         data
#define INITIAL_VALUE_S(data)       {data}
#define INITIAL_VALUE_DIFFERENT(data1, data2, data3)     data1
#define INITIAL_VALUE_DIFFERENT_S(data1, data2, data3)     {data1}
#define INITIAL_VALUE_INDEXED(data) data[GERAN_DATA_SPACE_INDEX_1]
#endif

typedef enum
{
  GERAN_EXCHANGE_NULL,
  GERAN_EXCHANGE_XTOG,
  GERAN_EXCHANGE_GTOX
} geran_exchange_direction_t;

typedef enum
{
  GERAN_TRAFFIC_DEFAULT = 0,
  GERAN_TRAFFIC_CS_VOICE,
  GERAN_TRAFFIC_CS_DATA,
  GERAN_TRAFFIC_CS_SIGNALLING,
  GERAN_TRAFFIC_PS_DATA,
  GERAN_TRAFFIC_PS_OTHER,
  GERAN_TRAFFIC_PS_SIGNALLING
} geran_traffic_reason_t;

typedef void (*grm_unlock_callback_t)( trm_unlock_callback_data *data );

/* the following functions are implemented in geran_multi_sim.c */

extern int geran_get_data_space_index(
 const gas_id_t
);

extern gas_id_t geran_get_multi_mode_gas_id(
  void
);

extern sys_modem_as_id_e_type geran_get_multi_mode_sub(
  void
);

extern boolean geran_is_multi_mode_gas_id(
  gas_id_t gas_id
);

extern sys_modem_as_id_e_type geran_map_gas_id_to_nas_id(
  gas_id_t gas_id
);

extern gas_id_t geran_map_nas_id_to_gas_id(
  sys_modem_as_id_e_type as_id
);

extern byte geran_get_diag_sub_id(
  const gas_id_t gas_id
);

extern boolean geran_set_sys_multi_sim_config(
  const sys_modem_device_mode_e_type new_device_mode,
  const sys_modem_dual_standby_pref_e_type new_standby_pref
);

extern sys_modem_dual_standby_pref_e_type geran_get_sys_multi_sim_pref(
  void
);

extern sys_modem_device_mode_e_type geran_get_multi_sim_mode(
  void
);

extern void geran_set_multi_mode_sub(
  sys_modem_as_id_e_type as_id
);

extern void geran_clear_lock_exchanged(
  const geran_exchange_direction_t direction,
  const gas_id_t gas_id
);

extern boolean geran_was_lock_exchanged(
  const geran_exchange_direction_t direction,
  const gas_id_t gas_id
);

extern boolean geran_exchange_lock(
  const geran_exchange_direction_t direction,
  const gas_id_t gas_id
);

extern void geran_set_exchange_on_release(
  const geran_exchange_direction_t direction,
  const gas_id_t gas_id
);

extern geran_exchange_direction_t geran_get_exchange_on_release(
  const gas_id_t gas_id
);

extern void  geran_abandon_lock_exchange(
  const geran_exchange_direction_t direction,
  const gas_id_t gas_id
);

extern boolean geran_in_multi_standby_mode(
  void
);

extern void geran_multi_sim_init(
  void
);

extern boolean geran_is_multi_mode_sub(
  sys_modem_as_id_e_type as_id
);

extern void geran_set_traffic_reason(
  const geran_traffic_reason_t traffic_reason,
  const gas_id_t gas_id
);

extern geran_traffic_reason_t geran_get_traffic_reason(
  boolean *ps_ptr,
  const gas_id_t gas_id
);

/* New GRM based API's for TRM interface */
/* the following functions are implemented in gpl1_dual_sim.c */
extern void grm_retain_lock( trm_client_enum_t     client_id,
                             grm_unlock_callback_t unlock_callback,
                             gas_id_t              gas_id );

extern void grm_release( trm_client_enum_t client_id,
                         gas_id_t          gas_id );

extern trm_grant_return_enum_type grm_get_granted(
                                    trm_client_enum_t         client_id,
                                    trm_get_info_return_type *output_data_p,
                                    gas_id_t                  gas_id);

extern trm_reason_enum_t grm_get_reason(
                           trm_client_enum_t client_id,
                           gas_id_t          gas_id );

extern rfm_device_enum_type grm_get_rf_device(
                              trm_client_enum_t client_id,
                              gas_id_t          gas_id );

extern void grm_update_aps_support(
              const boolean  aps_state,
              gas_id_t       gas_id);

extern sys_subs_feature_t geran_get_multi_mode_subs_feature(void);
    
extern void geran_set_multi_mode_subs_feature( sys_subs_feature_t geran_subs_feature);

extern void geran_as_id_gas_id_init( void );

extern boolean geran_device_in_svlte_dsda_mode( void );

#if defined(FEATURE_DUAL_SIM) || defined(FEATURE_TRIPLE_SIM)
#ifdef FEATURE_LTE

#define LTE_ACTIVITY_NONE  0x00000000
#define LTE_ACTIVITY_EMBMS 0x00000001
#define LTE_ACTIVITY_CDRX  0x00000002

extern void   geran_set_lte_activity_status(const uint32 activity_mask, const gas_id_t gas_id);
extern uint32 geran_get_lte_activity_status(const gas_id_t gas_id);
#endif /* FEATURE_LTE */
#endif /* FEATURE_DUAL_SIM || FEATURE_TRIPLE_SIM */

#endif /* __GERAN_MULTI_SIM_H */

/* EOF */

