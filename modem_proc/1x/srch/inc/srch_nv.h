#ifndef SRCH_NV_H
#define SRCH_NV_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               S E A R C H   T A S K  --  S R C H _ N V


GENERAL DESCRIPTION
   This file contains the NV access interface for srch.

EXTERNALIZED FUNCTIONS
   srch_nv_read_nv_items
   srch_nv_get_read_items

  Copyright (c) 2005 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_nv.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/05/16   ab      1x2GTA Feature changes
03/04/15   ab      Added NV for DrX feature
12/16/14   pap     Adding changes for new EFS file.
12/08/14   pap     Adding hysteresis for hybrid fallback mode.
04/09/14   ssh     Added 1x Acq Priority Inversion support.
09/06/13   srk     Fix EFS read/write issues caused due to recent
                   reorganization of NV read/write functionality.
01/29/13   vks     Remove NV 70262 support (deprecated)
01/23/13   vks     Add new NV to support entire asdiv including the new
                   type2 algorithm implementation
11/09/12   vks     Add NV support for antenna selection diversity feature.
06/21/12   sst     Add EFS support
01/25/11   adw     Support querying of previously read NV items.
11/30/10   adw     Cleaned up unnecessarily exported symbols.
10/15/10   adw     Pre-Candidate acceleration based on active set size.
10/21/09   adw     Removed QLIC disable mask and shift value.
04/01/09   adw     Categorized included header files and standardized format.
03/25/09   adw     Added comdef.h include.
11/07/08   pa      Fixed #define for header
11/06/08   pk      Fixes for Q6 compiler
02/16/07   sst     Add QLIC support
04/03/06   awj     Added translation for TRM_RX_BEST and TRM_RX_BEST_POSSIBLE
10/25/05   awj     Migrated here from srch.c
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "comdef.h"
#include "modem_1x_defs.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*--------------------------------------------------------------------------*
 *                  Srch EFS defines                                        *
 *--------------------------------------------------------------------------*/

/* Name and path of the conf file for 1X Srch */
#define SRCH_EFS_CONF_FILE "/nv/item_files/conf/onex_srch.conf"

/* Enum of EFS item files */
typedef enum
{
  SRCH_EFS_DYN_RID_THRESHOLDS = 0,
  SRCH_EFS_DYN_RID_SEL_PARMS,
  SRCH_EFS_DYN_RIN_THRESHOLDS,
  SRCH_EFS_RDDS_PARAMS,
  SRCH_EFS_DIV_ASD_INT_THRESHOLDS,
  SRCH_EFS_DIV_ASD,
  SRCH_EFS_ACQ_PRI_TIMER,
  SRCH_EFS_DYN_RID_IDLE_PARAMS,
  SRCH_EFS_1X_DRX_PARAMS,
  SRCH_EFS_1X_TRAFFIC_TA_PARAMS,
  SRCH_EFS_ITEM_FILE_MAX
} srch_efs_item_file_name_type;

/* EFS file name containing default thresholds */
/* This EFS will be obsolete in latest version of NV 
 * due to SRCH_EFS_DYN_RID_IDLE_PARAMS 
 */
#define SRCH_EFS_DYN_RID_THRESHOLDS_FILENAME \
        "/nv/item_files/modem/1x/zz2_2_thresh"

/* EFS file name containing default thresholds */
/* This EFS will be obsolete in latest version of NV 
 * due to SRCH_EFS_DYN_RID_IDLE_PARAMS 
 */
#define SRCH_EFS_DYN_RID_SEL_PARMS_FILENAME \
        "/nv/item_files/modem/1x/zz2_2_sel_parms"

/* EFS file name containing default thresholds */
#define SRCH_EFS_DYN_RIN_THRESHOLDS_FILENAME \
        "/nv/item_files/modem/1x/rin_thresh"

/* EFS file name containing default thresholds */
#define SRCH_EFS_RDDS_PARAMS_FILENAME \
       "/nv/item_files/modem/1x/new_rdds_params"

/* EFS file name containing default thresholds */
#define SRCH_EFS_DIV_ASD_INT_THRESHOLDS_FILENAME \
        "/nv/item_files/modem/1x/div_asd_int_thresh"

/* EFS file name containing default thresholds */
#define SRCH_EFS_DIV_ASD_FILENAME \
        "/nv/item_files/modem/1x/div_asd"

/* EFS file name containing timers related to 1x throttling mechanism */
#define SRCH_EFS_ACQ_PRI_TIMER_FILENAME \
        "/nv/item_files/modem/1x/acq_pri_timers"

/* EFS file name containing default values for idle module */
#define SRCH_EFS_DYN_RID_IDLE_PARAMS_FILENAME \
          "/nv/item_files/modem/1x/zz2_3_params"

/* EFS file name containing default values 1x DRX */
#define SRCH_EFS_1X_DRX_PARAMS_FILENAME \
          "/nv/item_files/modem/1x/1x_drx_params"



/* EFS file name containing default values 1x DATA TA */
#define SRCH_EFS_1X_TRAFFIC_TA_PARAMS_FILENAME \
          "/nv/item_files/modem/1x/1x_traffic_ta_params"
          
/*--------------------------------------------------------------------------*
 *                  SRCH<->TRM chain translation values                     *
 *--------------------------------------------------------------------------*/
#define SRCH_NV_RX_DEFAULT                         0x0
#define SRCH_NV_RX_0                               0x1
#define SRCH_NV_RX_1                               0x2

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

typedef uint32  srch_nv_dbg_mask_type;  /* element from NV debug item type */

typedef struct
{
  uint16 cdma_rx_diversity_ctrl_i;
  uint16 cdma_rx_chain_sel_thresh_upper_i;
  uint16 cdma_rx_chain_sel_thresh_lower_i;
  uint64 srch_dbg_mask_i;
  uint32 cdma_rx_div_bc_enable_mask_i;
}
srch_nv_items_type;


/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_NV_READ_NV_ITEMS

DESCRIPTION    This function reads the RX_DIVERSITY NV item

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_nv_read_nv_items( void );

/*===========================================================================

FUNCTION       SRCH_NV_GET_READ_ITEMS

DESCRIPTION    This function returns the values read from NV.

DEPENDENCIES   srch_nv_read_nv_items() should have been called previously.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern srch_nv_items_type srch_nv_get_read_items( void );
#endif /* SRCH_NV_H */
