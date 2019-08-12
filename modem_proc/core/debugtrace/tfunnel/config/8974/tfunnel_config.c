/*==============================================================================

  Trace Funnel Configuration    

DESCRIPTION
  This file has port configurations for trace funnels for a given chipset.

REFERENCES

        Copyright © 2011 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/debugtrace/tfunnel/config/8974/tfunnel_config.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
2/7/2011   prasads  First draft created. 
===========================================================================*/
#include "com_dtypes.h"

#define IN_FUN0  0
#define IN_FUN1  1
#define MERG_FUN 2

/*Port assignments based on QDSS HPG*/
#define STM_PORT_NUM        7 
#define PERIPH_NOC_PORT_NUM 3
#define BIMC_NOC_PORT_NUM   5
#define SYSTEM_NOC_PORT_NUM 4
typedef struct TypeTFunnelPortEnableConfig TFunnelPortEnableConfig;

/*
This structure defines the topology of a funnel configuration.
*/
struct TypeTFunnelPortEnableConfig{
   uint32 funnel_id;
   uint32 port_id;
   TFunnelPortEnableConfig *pFunnelConfig;
};

/*
Define supported funnel configurations.
*/
static TFunnelPortEnableConfig tfunnel_merge_port1[]={
{MERG_FUN,1,NULL}
};

static TFunnelPortEnableConfig tfunnel_merge_port0[]={
   {MERG_FUN,0,tfunnel_merge_port1}
};


TFunnelPortEnableConfig tfunnel_port_stm[]={
   {IN_FUN1,STM_PORT_NUM,tfunnel_merge_port1}
};

 TFunnelPortEnableConfig tfunnel_port_peripheral_noc[] = {
{IN_FUN1,PERIPH_NOC_PORT_NUM,tfunnel_merge_port0}
};

TFunnelPortEnableConfig tfunnel_port_bimc_noc[] = {
   {IN_FUN0,BIMC_NOC_PORT_NUM,tfunnel_merge_port1}
};

TFunnelPortEnableConfig tfunnel_port_system_noc[] = {
   {IN_FUN0,SYSTEM_NOC_PORT_NUM,tfunnel_merge_port1}
};


