/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2011 by Qualcomm Technologies INCORPORATED.  
  Copyright (c) 2012 by Qualcomm Technologies INCORPORATED.  
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                           
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rftech_tdscdma/ftm/inc/ftm_tdscdma_hsupa.h#1 $ 
  $DateTime: 2016/12/13 07:59:50 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/30/14   qzh    Enable FTM NS new intf and remove the dependency
11/20/13   qzh    Featurized FTM NS new interface to disable it on BO 0.1 
07/06/13   qzh    Update Non-signaling R4&HSUPA code with new TL1-RFSW interface and function partition
12/06/12   ms     Merged 
11/07/12   qzh    Add support for HSUPA freq reconfiguration and merge HSUPA FTM related msg to only one setup msg.
10/11/12   qzh    Created and add support for TDSCDMA FTM HSUPA.
===========================================================================*/
#ifndef FTM_TDSCDMA_HSUPA_H
#define FTM_TDSCDMA_HSUPA_H

/*===========================================================================

          INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#define FEATURE_RF_TL1_FTM_NEW_INTF
#endif

#include "target.h"
#include "customer.h"
#include "ftm.h"
#include "ftmdiag.h"
#include "ftm_log.h"
#include "queue.h"
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tdsl1rrcif.h"
#include "tdsrrccmd.h"
#include "tdsrrccmd_v.h"
#endif
#ifdef FTM_HAS_TDS_HSUPA
#ifndef FEATURE_RF_TL1_FTM_NEW_INTF
#include "tdseulsg.h"
#endif
#include "ftm_tdscdma_ber.h"


/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================
 
                    LOCAL GLOBAL VARIABLES AND CONSTANTS

===========================================================================*/


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
enum
{
    FTM_TDSCDMA_HSUPA_CMD_SETUP                  = FTM_TDSCDMA_HSUPA_CMD_BASE,
};


typedef PACK(struct)
{
  uint16 rf_chan;
  uint8 dl_dpch_slot;
  uint8 ul_dpch_slot;
  int8  ul_max_tx_pwr;
  uint8 is_hsupa_on_flag;
  uint8 epuch_slot;
  uint8 epuch_code;
  int8 epuch_tx_power;
  uint8 num_eucch;
  uint8 mod_type;
} ftm_tdscdma_hsupa_setup_config_type;


typedef PACK(struct)
{

    diagpkt_subsys_header_type               header;
    ftm_cmd_header_type                      ftm_cmd_header;
    ftm_tdscdma_hsupa_setup_config_type  hsupa_config;
} ftm_tdscdma_hsupa_setup_type;


typedef enum
{
    FTM_TDSCDMA_HSUPA_CHAN_STATE_INACTIVE,
    FTM_TDSCDMA_HSUPA_CHAN_STATE_ACTIVE

} ftm_tdscdma_hsupa_chan_state_type;

#ifdef FEATURE_RF_TL1_FTM_NEW_INTF
typedef struct
{
    boolean                               isInitialized;
    ftm_tdscdma_hsupa_chan_state_type     current_state;
} ftm_tdscdma_hsupa_db_type;
#else
typedef struct
{
  tdseulsg_mod_type_enum_type epuch_mod_type;
  int16 epuch_tx_pwr;
  uint8 num_e_ucch;
  uint8 eagch_crri;
  uint8 eagch_trri;
  uint8 eagch_ecsn;
} ftm_tdscdma_hsupa_config_struct_type;

typedef struct
{

    boolean                               isInitialized;
    ftm_tdscdma_hsupa_chan_state_type     current_state;
    ftm_tdscdma_hsupa_config_struct_type  eagch_epuch_cfg;
    tdsl1_e_info_struct_type hsupa_edch_cfg;
    uint16 hsupa_req_mask;
    uint16 hsupa_e_req_mask;
} ftm_tdscdma_hsupa_db_type;
#endif


void ftm_tdscdma_hsupa_init(void);

#endif /* FTM_HAS_TDS_HSUPA */

#endif /* FTM_TDSCDMA_BER_H */

