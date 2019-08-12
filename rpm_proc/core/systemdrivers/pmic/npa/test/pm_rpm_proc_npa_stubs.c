/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             P M    NPA   RPM PROC STUBS

GENERAL DESCRIPTION
  This file contains initialization functions for NPA

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010           by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/rpm.bf/2.1.1/core/systemdrivers/pmic/npa/test/pm_rpm_proc_npa_stubs.c#1 $
$DateTime: 2015/02/25 02:53:43 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/10   umr     Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

#include "npa.h"

#include "pm_npa_device_clk_buff.h"
#include "pm_npa_device_ldo.h"
#include "pm_npa_device_smps.h"
#include "pm_npa_device_ncp.h"
#include "pm_npa_device_vs.h"

#include "npa_resource.h"
#include "npa_init.h"

#ifdef PMIC_OFFTARGET_TESTING

void*
pm_rpm_device_rsrc_set_vreg_cb_offtarget ( void *usr_data, uint32 arg2,
                                            void *req, uint32 req_len );

/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/
#define PMIC_RPM_DEV_RSRC_RAIL_NAME_VEC_IN(chipid, rtype, index)  \
    "/pmic/device/rpm/" #rtype "/" #chipid "/" #rtype #index "/vec"

#define PMIC_RSRC_SMPS( index, chipid, rtype)                     \
  {                                                               \
    PMIC_RPM_DEV_RSRC_RAIL_NAME_VEC_IN(chipid, rtype, index ),    \
    "vector",                                                     \
    sizeof (pm_npa_smps_kvps) / sizeof( npa_resource_state ),     \
    &npa_identity_plugin,                                         \
    NPA_RESOURCE_REMOTE_ACCESS_ALLOWED|NPA_RESOURCE_VECTOR_STATE, \
    (void *)NULL,                                                 \
    NULL,                                                         \
  }

#define PMIC_RSRC_LDO( index, chipid, rtype)                      \
  {                                                               \
    PMIC_RPM_DEV_RSRC_RAIL_NAME_VEC_IN(chipid, rtype, index ),    \
    "vector",                                                     \
    sizeof (pm_npa_ldo_kvps) / sizeof( npa_resource_state ),      \
    &npa_identity_plugin,                                         \
    NPA_RESOURCE_REMOTE_ACCESS_ALLOWED|NPA_RESOURCE_VECTOR_STATE, \
    (void *)NULL,                                                 \
    NULL,                                                         \
  }

#define PMIC_RSRC_VSWITCH( index, chipid, rtype)                  \
  {                                                               \
    PMIC_RPM_DEV_RSRC_RAIL_NAME_VEC_IN(chipid, rtype, index ),    \
    "vector",                                                     \
    sizeof (pm_npa_vs_kvps) / sizeof( npa_resource_state ),       \
    &npa_identity_plugin,                                         \
    NPA_RESOURCE_REMOTE_ACCESS_ALLOWED|NPA_RESOURCE_VECTOR_STATE, \
    (void *)NULL,                                                 \
    NULL,                                                         \
}

#define PMIC_RSRC_NCP( index, chipid, rtype)                       \
  {                                                                \
    PMIC_RPM_DEV_RSRC_RAIL_NAME_VEC_IN(chipid, rtype, index ),     \
    "vector",                                                      \
    sizeof (pm_npa_ncp_kvps) / sizeof( npa_resource_state ),       \
    &npa_identity_plugin,                                          \
    NPA_RESOURCE_REMOTE_ACCESS_ALLOWED|NPA_RESOURCE_VECTOR_STATE,  \
    (void *)NULL,                                                  \
    NULL,                                                          \
  }

#define PMIC_RSRC_CLK( index, chipid, rtype)                       \
{                                                                  \
    PMIC_RPM_DEV_RSRC_RAIL_NAME_VEC_IN(chipid, rtype, index ),     \
    "vector",                                                      \
    sizeof (pm_npa_clk_buff_kvps) / sizeof( npa_resource_state ),  \
    &npa_identity_plugin,                                          \
    NPA_RESOURCE_REMOTE_ACCESS_ALLOWED|NPA_RESOURCE_VECTOR_STATE,  \
    (void *)NULL,                                                  \
    NULL,                                                          \
}

// PMIC Device A Resource Numbers
#define PMIC_NPA_SMPS_A_RSRC_COUNT       3
#define PMIC_NPA_LDO_A_RSRC_COUNT       25
#define PMIC_NPA_VS_A_RSRC_COUNT         5
#define PMIC_NPA_NCP_A_RSRC_COUNT        0
#define PMIC_NPA_CLK_BUFF_A_RSRC_COUNT   9

// PMIC Device B Resource Numbers
#define PMIC_NPA_SMPS_B_RSRC_COUNT       8
#define PMIC_NPA_LDO_B_RSRC_COUNT        0
#define PMIC_NPA_VS_B_RSRC_COUNT         0
#define PMIC_NPA_NCP_B_RSRC_COUNT        0
#define PMIC_NPA_CLK_BUFF_B_RSRC_COUNT   0

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/
static npa_resource_state
pm_npa_rpm_sudo_rsrc_driver_fcn (npa_resource      *resource,
                                 npa_client_handle  client,
                                 npa_resource_state state );


/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/

/* NCP  Resource Definition   */
//static npa_resource_definition
//pm_npa_rpm_dev_rsrc_ncp_8921 [] =
//{
//    PMIC_RSRC_NCP( 0, A, DAL_RPM_RESOURCE_NCP ), /* NCP ID, Chip ID */
//};

/* Device NPA Nodes */ 
//static npa_node_definition 
//pm_npa_rpm_dev_node_ncp_8921 [] =
//{
//    {
//        "/pmic/device/rpm/ncp/A" ,           // Node Name
//        pm_npa_rpm_sudo_rsrc_driver_fcn,     // Driver Function
//        NPA_NODE_DEFAULT,                    // Node Attributes
//        NULL,                                // Node User Data
//        0,                                   // Number of deps
//        NULL,                                // Dependencies
//        NPA_ARRAY(pm_npa_rpm_dev_rsrc_ncp_8921) // Resource Count & Resources
//    }
//};

/* VSWITCH  Resource Definition   */
static npa_resource_definition
pm_npa_rpm_dev_rsrc_vs_8941 [PMIC_NPA_VS_A_RSRC_COUNT] =
{                   
  PMIC_RSRC_VSWITCH( 1, A, vs), 
  PMIC_RSRC_VSWITCH( 2, A, vs),
  PMIC_RSRC_VSWITCH( 3, A, vs), 
  PMIC_RSRC_VSWITCH( 4, A, vs),
  PMIC_RSRC_VSWITCH( 5, A, vs), 
};

/* Device NPA Nodes */ 
static npa_node_definition 
pm_npa_rpm_dev_node_vs_8941[] =
{
    {
        "/pmic/device/rpm/vs/A" ,              // Node Name
        pm_npa_rpm_sudo_rsrc_driver_fcn,       // Driver Function
        NPA_NODE_DEFAULT,                      // Node Attributes
        NULL,                                  // Node User Data
        0,                                     // Number of deps
        NULL,                                  // Dependencies
        NPA_ARRAY(pm_npa_rpm_dev_rsrc_vs_8941) // Resource Count & Resources
    },
};


/* LDO  Resource Definition   */
static npa_resource_definition
pm_npa_rpm_dev_rsrc_ldo_8941 [PMIC_NPA_LDO_A_RSRC_COUNT] =
{                
  PMIC_RSRC_LDO( 1, A, ldo), /* LDO ID, Chip ID */
  PMIC_RSRC_LDO( 2, A, ldo),
  PMIC_RSRC_LDO( 3, A, ldo),
  PMIC_RSRC_LDO( 4, A, ldo),
  PMIC_RSRC_LDO( 5, A, ldo),
  PMIC_RSRC_LDO( 6, A, ldo),
  PMIC_RSRC_LDO( 7, A, ldo),
  PMIC_RSRC_LDO( 8, A, ldo),
  PMIC_RSRC_LDO( 9, A, ldo),
  PMIC_RSRC_LDO( 10,A, ldo),
  PMIC_RSRC_LDO( 11, A, ldo),
  PMIC_RSRC_LDO( 12, A, ldo),
  PMIC_RSRC_LDO( 13, A, ldo),
  PMIC_RSRC_LDO( 14, A, ldo),
  PMIC_RSRC_LDO( 15, A, ldo),
  PMIC_RSRC_LDO( 16, A, ldo),
  PMIC_RSRC_LDO( 17, A, ldo),
  PMIC_RSRC_LDO( 18, A, ldo),
  PMIC_RSRC_LDO( 19, A, ldo),
  PMIC_RSRC_LDO( 20, A, ldo),
  PMIC_RSRC_LDO( 21, A, ldo),
  PMIC_RSRC_LDO( 22, A, ldo),
  PMIC_RSRC_LDO( 23, A, ldo),
  PMIC_RSRC_LDO( 24, A, ldo),
  PMIC_RSRC_LDO( 25, A, ldo),
};

/* Device NPA Nodes */ 
static npa_node_definition 
pm_npa_rpm_dev_node_ldo_8941 [] =
{
    {
        "/pmic/device/rpm/ldo/A" ,          // Node Name
        pm_npa_rpm_sudo_rsrc_driver_fcn,    // Driver Function
        NPA_NODE_DEFAULT,                   // Node Attributes
        NULL,                               // Node User Data
        0,                                  // Number of deps
        NULL,                               // Dependencies
        NPA_ARRAY(pm_npa_rpm_dev_rsrc_ldo_8941)  // Resource Count & Resources
    }
};

/* 8941 SMPS  Resource Definition   */
static npa_resource_definition
pm_npa_rpm_dev_rsrc_smps_8941 [PMIC_NPA_SMPS_A_RSRC_COUNT] =
{
  /* SMPS_1 and SMPS_3 are created as part of source in RPM */
  PMIC_RSRC_SMPS( 1, A, smps), /* SPMS ID, Chip ID, Shadow offset */
  PMIC_RSRC_SMPS( 2, A, smps),
  PMIC_RSRC_SMPS( 3, A, smps),
};

/* Device NPA Nodes */ 
static npa_node_definition 
pm_npa_rpm_dev_node_smps_8941 [] =
{
    {
        "/pmic/device/rpm/smps/A" ,          // Node Name
        pm_npa_rpm_sudo_rsrc_driver_fcn,          // Driver Function
        NPA_NODE_DEFAULT,                         // Node Attributes
        NULL,                                     // Node User Data
        0,                                        // Number of deps
        NULL,                                     // Dependencies
        NPA_ARRAY(pm_npa_rpm_dev_rsrc_smps_8941)  // Resource Count & Resources
    }
};

/* 8941 CLK Buffer Resource Definition   */
static npa_resource_definition
pm_npa_rpm_dev_rsrc_clk_8941 [PMIC_NPA_CLK_BUFF_A_RSRC_COUNT] =
{
    PMIC_RSRC_CLK( 1, A, clk), 
    PMIC_RSRC_CLK( 2, A, clk),
    PMIC_RSRC_CLK( 3, A, clk),
    PMIC_RSRC_CLK( 4, A, clk), 
    PMIC_RSRC_CLK( 5, A, clk),
    PMIC_RSRC_CLK( 6, A, clk),
    PMIC_RSRC_CLK( 7, A, clk), 
    PMIC_RSRC_CLK( 8, A, clk),
    PMIC_RSRC_CLK( 9, A, clk),
};

/* Device NPA Clk Buffer Nodes */ 
static npa_node_definition 
pm_npa_rpm_dev_node_clk_8941 [] =
{
    {
        "/pmic/device/rpm/clk/A" ,          // Node Name
            pm_npa_rpm_sudo_rsrc_driver_fcn,          // Driver Function
            NPA_NODE_DEFAULT,                         // Node Attributes
            NULL,                                     // Node User Data
            0,                                        // Number of deps
            NULL,                                     // Dependencies
            NPA_ARRAY(pm_npa_rpm_dev_rsrc_clk_8941)  // Resource Count & Resources
    }
};

/* 8941 SMPS  Resource Definition   */
static npa_resource_definition
pm_npa_rpm_dev_rsrc_smps_8841 [PMIC_NPA_SMPS_B_RSRC_COUNT] =
{\
    PMIC_RSRC_SMPS( 1, B, smps), 
    PMIC_RSRC_SMPS( 2, B, smps), 
    PMIC_RSRC_SMPS( 3, B, smps), 
    PMIC_RSRC_SMPS( 4, B, smps),
    PMIC_RSRC_SMPS( 5, B, smps),
    PMIC_RSRC_SMPS( 6, B, smps),
    PMIC_RSRC_SMPS( 7, B, smps),
    PMIC_RSRC_SMPS( 8, B, smps),
};

/* Device NPA Nodes */ 
static npa_node_definition 
pm_npa_rpm_dev_node_smps_8841 [] =
{
    {
        "/pmic/device/rpm/smps/B" ,               // Node Name
        pm_npa_rpm_sudo_rsrc_driver_fcn,          // Driver Function
        NPA_NODE_DEFAULT,                         // Node Attributes
        NULL,                                     // Node User Data
        0,                                        // Number of deps
        NULL,                                     // Dependencies
        NPA_ARRAY(pm_npa_rpm_dev_rsrc_smps_8841)  // Resource Count & Resources
    }
};


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION pm_rpm_proc_sudo_rsrc_init

DESCRIPTION
    This function creates sudo rsrc for RPM proc.

    It does the following:
    * It initializes the NPA PMIC Node and resources.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  NPA Framework should be available in the build being compiled for

SIDE EFFECTS
  NONE.

===========================================================================*/
void
pm_rpm_proc_sudo_rsrc_init(void)
{
  /* Initialize SMPS Node and Resources for 8941 */
  npa_define_node (pm_npa_rpm_dev_node_smps_8941, NULL, NULL);

  /* Initialize LDO Node and Resources for 8941 */
  npa_define_node (pm_npa_rpm_dev_node_ldo_8941, NULL, NULL);

  /* Initialize Switchers Node and Resources for 8941 */
  npa_define_node (pm_npa_rpm_dev_node_vs_8941, NULL, NULL);

  /* Initialize Clock Node and Resources for 8941 */
  npa_define_node (pm_npa_rpm_dev_node_clk_8941, NULL, NULL);

  /* Initialize Switchers Node and Resources for 8941 */
  //npa_define_node (pm_npa_rpm_dev_node_ncp_8921, NULL, NULL);

  /* Initialize SMPS Node and Resources for 8841 */
  npa_define_node (pm_npa_rpm_dev_node_smps_8841, NULL, NULL);
} /* end of pm_rpm_proc_npa_test() */

//DAL_rpmfw_RequestType rpm_req;

static npa_resource_state
pm_npa_rpm_sudo_rsrc_driver_fcn (npa_resource      *resource,
                                 npa_client_handle  client,
                                 npa_resource_state state )
{
    (void)resource;
    (void) client;
    (void) state;
//  if (client->type == NPA_CLIENT_INITIALIZE)
//  {
//  }
//  else
//  {
//    rpm_req.Resource = (DAL_rpm_ResourceType)resource->definition->data;
//#ifdef PM_IMAGE_MODEM_PROC
//	  rpm_req.Master = DAL_RPMFW_MASTER_1; /* Modem */
//#else
//      //rpm_req.Master = DAL_RPMFW_MASTER_0;  /* Apps */
//#endif
    //if (resource->definition->max == 2)
    //{
    //  /* LDO or a SMPS request */
    //  rpm_req.DataLen = 8;
    //  rpm_req.Data = (void *)(NPA_PENDING_REQUEST( client ).pointer.reference);
    //}
    //else
    //{
    //  rpm_req.DataLen = 4;
    //  rpm_req.Data = &state;
    //}

    //pm_rpm_device_rsrc_set_vreg_cb_offtarget ( resource->node->data, 0,
    //                                           &rpm_req, sizeof(DAL_rpmfw_RequestType) );
  //}
  return state;
}

#endif // PMIC_OFFTARGET_TESTING

