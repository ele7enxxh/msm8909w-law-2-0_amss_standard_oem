#ifndef PM_RPM_PROC_NPA_H
#define PM_RPM_PROC_NPA_H

/*! \file pm_rpm_npa.h
 *  
 *  \brief This file contains PMIC resources internal rep struct types, internal
 *         client types enum and railway/npa related funtion prototype definitions.
 *  
 *    &copy; Copyright(c) 2010-2014 QUALCOMM Technologies Incorporated, All Rights Reserved
 */

/*===========================================================================

                EDIT HISTORY FOR MODULE

  This document is created by a code generator, therefore this section will
  not contain comments describing changes made to the module over time.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/inc/pm_rpm_npa.h#1 $ 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/04/15   akt      Updated waiting delay to 2 times for ignored rails  (CR-787855)
06/11/13   hs       Adding settling time for regulators.
04/23/13   hs       Fixed the naming convention in \config.
04/12/13   hs       Code refactoring.
1/29/13    aks      Adding support for 5v boost as a separate driver
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES 

===========================================================================*/
#include "rpm.h"
#include "npa.h"
#include "npa_resource.h"
#include "pm_npa_irep.h"

/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/

typedef struct
{
   rpm_resource_type resource_type;
   unsigned resource_id;
   unsigned client_type;
   unsigned internal_client_index;
}pm_rpm_local_resource_client_dependency;

typedef struct
{
   unsigned                                 dependency_count;
   pm_rpm_local_resource_client_dependency *dependencies;
}pm_rpm_resource_client_dependency_info;

#ifdef _WIN32
#pragma pack(push,1) // Save previous, and turn on 1 byte alignment
#endif

typedef struct
{
   unsigned sw_en:1; // [Disable (default), Enable] ? max aggregation (left to right)
   unsigned pc_en:4; // [NONE, EN1, EN2, EN3, EN4] - ORed value of list
   // 32 bit boundary
   unsigned ip:13; // mA > 8 Amps
   unsigned reserve1:14;
    // 32 bit boundary
} pm_npa_vs_int_rep;

typedef struct
{
   unsigned sw_en:1; // [Disable (default), Enable] ? max aggregation (left to right)
   unsigned reserve1:31;
   // 32 bit boundary
   unsigned start_address:32; // uV
}pm_npa_pbs_int_rep;

typedef struct
{
   unsigned sw_enable:1; // Software Enable
   unsigned pc_enable:4; // Pin Control Enable
   unsigned reserved:27;
    // 32 bit boundary
}pm_npa_clk_buffer_int_rep;

typedef struct
{
   unsigned sw_en:1; // Software Enable
   unsigned pc_en:4; // [NONE, EN1, EN2, EN3, EN4] - ORed value of list
   unsigned reserved:27; //32-bit boundary
}pm_npa_boost_int_rep;

typedef struct
{
   unsigned sw_en:1; // Software Enable
   unsigned byp_allowed:1; // Bypass Allowed, Bypass Disallowed while entering sleep - max aggregation (left to right)
   unsigned mode:3; // [FORCED_BYPASS (default), AUTO_BOOST] - max aggregation (left to right)
   unsigned reserved:27; //32-bit boundary
   // 32 bit boundary
   unsigned uvol  :32; // requested voltage from the client
   // 32 bit boundary
   unsigned pin_uvol :32; // MODE_BB pin voltage
   //32-bit boundary
}pm_npa_boost_byp_int_rep;

#ifdef _WIN32
#pragma pack(pop) // Revert alignment to what it was previously
#endif

/*===========================================================================

                        DEFINITIONS

===========================================================================*/

/* The number of times we multiply the estimated settling time to
   calculate the maximum allowed settling time on a resource */
#define PM_RPM_MAX_SETTLING_TIME_MULTIPLIER   10

/* The number of times we multiply the estimated settling time to
   calculate the maximum allowed settling time on a resource */
#define PM_RPM_MIN_SETTLING_TIME_MULTIPLIER   2

#define PM_RPM_SETTLING_TIME_POLL_INTERVAL    25 //uS
#define PM_RPM_SETTTLING_TIME_THRESHOLD       50 //uS

typedef enum
{
   // Begin Internal regular client types
   PM_RPM_NPA_CLIENT_SMPS_REQ = 1, // This must be assigned to 1 to avoid the "invalid client type" error in the RPM code.
   PM_RPM_NPA_CLIENT_BOOST_REQ,
   PM_RPM_NPA_CLIENT_LDO_REQ,
   PM_RPM_NPA_CLIENT_NCP_REQ,
   PM_RPM_NPA_CLIENT_VS_REQ,
   PM_RPM_NPA_CLIENT_CLK_BUFFER_REQ,
   PM_RPM_NPA_CLIENT_BOOST_BYP_REQ,
   
   // Begin Source Dependency based client types
   PM_RPM_NPA_INT_CLIENT_SMPS_DEPENDENT, // Internal client for SMPS sources
   PM_RPM_NPA_INT_CLIENT_BOOST_DEPENDENT, // Internal client for BOOST sources
   PM_RPM_NPA_INT_CLIENT_LDO_DEPENDENT, // Internal client for LDO sources
   PM_RPM_NPA_INT_CLIENT_VS_DEPENDENT, // Internal client for VS sources
   PM_RPM_NPA_INT_CLIENT_CLK_DEPENDENT, // Internal client for CLK sources
   PM_RPM_NPA_INT_CLIENT_BOOST_BYP_DEPENDENT, // Internal client for BOOST_BYP sources
   
   // Begin Operational Dependency based Client Types
   PM_RPM_NPA_INT_CLIENT_SW_ENABLE_DEPENDENT, // Internal client that registers voltage vote with another resource
   PM_RPM_NPA_INT_CLIENT_SW_MODE_DEPENDENT, // Internal client that registers SW Mode vote with another resource
   PM_RPM_NPA_INT_CLIENT_PIN_CONTROL_ENABLE_DEPENDENT, // Internal client that registers pc_en vote with another resource
   PM_RPM_NPA_INT_CLIENT_CLK_PIN_CONTROL_ENABLE_DEPENDENT, // Internal client that registers clock buffer pc_en vote with another resource
   PM_RPM_NPA_INT_CLIENT_PIN_CONTROL_MODE_DEPENDENT, // Internal client that registers pc_mode vote with another resource
   PM_RPM_NPA_INT_CLIENT_VOLTAGE_DEPENDENT, // Internal client that registers voltage vote with another resource
   PM_RPM_NPA_INT_CLIENT_CURRENT_DEPENDENT, // Internal client that registers peak current vote with another resource
   PM_RPM_NPA_INT_CLIENT_FREQ_DEPENDENT, // Internal client that registers voltage vote with another resource
   PM_RPM_NPA_INT_CLIENT_FREQ_REASON_DEPENDENT, // Internal client that registers voltage vote with another resource
   PM_RPM_NPA_INT_CLIENT_QUIET_MODE_DEPENDENT, // Internal client that registers voltage vote with another resource
   PM_RPM_NPA_INT_CLIENT_HEADROOM_DEPENDENT, // Internal client that registers headroom voltage vote with another resource
   PM_RPM_NPA_INT_CLIENT_BYPASS_EN_DEPENDENT, // Internal client that registers bypass enabled vote with another resource
   PM_RPM_NPA_INT_CLIENT_BYPASS_ALLOWED_DEPENDENT, // Internal client that registers bypass allowed vote with another resource
   PM_RPM_NPA_INT_CLIENT_CORNER_LEVEL_DEPENDENT, // Internal client that registers corner level vote with another resource
}pm_rpm_nonstandard_internal_client_types;

typedef struct
{
   unsigned npa_key;
   pm_rpm_nonstandard_internal_client_types int_client_type;
   unsigned int_client_size;
}pm_rpm_keys_to_dependent_map;

extern pm_rpm_keys_to_dependent_map keys_to_dependent_map[14];
extern npa_client_handle handle_rpm_init;

/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION pm_rpm_proc_npa_init

DESCRIPTION
This function initializes the NPA for rpm.

It does the following:
1)  It initializes the PMIC NPA software driver for nodes and resources.

INPUT PARAMETERS
None.

RETURN VALUE
None.

DEPENDENCIES
None.

SIDE EFFECTS
None.

===========================================================================*/
void pm_rpm_proc_npa_init(void);

/*===========================================================================

FUNCTION pm_rpm_railway_init

DESCRIPTION
This function initializes the PMIC interface for rpm railway.

It does the following:
1)  This function initializes the PMIC interface for rpm railway.

INPUT PARAMETERS
None.

RETURN VALUE
None.

DEPENDENCIES
None.

SIDE EFFECTS
None.

===========================================================================*/
void pm_rpm_railway_init(void);

//void pm_rpm_register_nonstandard_internal_client_types();

npa_resource_state pm_rpm_npa_resource_set (npa_resource      *resource,
                                            npa_client_handle  client,
                                            npa_resource_state mode_id);

npa_resource_state pm_rpm_npa_process_rsrc (npa_resource *resource,
                                            pm_rpm_local_resource_client_dependency *dep_info,
                                            npa_resource_state mode_id,
                                            uint32 index);


#endif //PM_RPM_PROC_NPA_H



