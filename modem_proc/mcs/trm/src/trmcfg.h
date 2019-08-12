#ifndef TRMCFG_H
#define TRMCFG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

            T R A N S C E I V E R   R E S O U R C E   M A N A G E R

               Transceiver Resource Manager Configuration Header

GENERAL DESCRIPTION

  This file supports configuration Transceiver Resource Manager


EXTERNALIZED FUNCTIONS

  None
  

REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trmcfg.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
12/11/2014   mn      LTE EMBMS functional changes (CR: 769538).
10/20/2014   sk      Fix memory issue on offtarget(734841)
06/18/2014   mn      Changes for DR-DSDS (CR: 651800)
02/27/2014   sr      SLTE Changes.
11/13/2013   mn      Opp. SGLTE/SGTDS/SVLTE +G DSDA support.
04/21/2011   ag      Added an API to check whether SHDR mode is supported.
02/10/2011   sg      Increasd the size of TRM modes to uint32
04/06/2010   sm      Added Support for PAM functionality
07/31/2009   ag      Fixed lint warnings. 
02/17/2009   cpk     Added support for Off Target support
08/28/2008   adw     Added FEATURE_MCS_TRM to featurize TRM for ULC.
05/22/2008   sg      Integrated changes from old depot
05/02/2006   cab     Added ifdef's for offline compilation and testing
11/21/2005   cab     Moved NV item id's to compile time assign outside library
08/29/2005   grl     Added support for dynamic compatibility masks.
07/11/2005   grl     Added TRM config mask field to set_compatible_clients.
06/21/2005   ajn     Linting changes.
06/14/2005   ajn     Removed secondary client association.
03/28/2005   ajn     Added client/reason compatibility
03/02/2005   ajn     Initial AMSS version

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include <customer.h>

extern "C"
{
  #include "trm.h"
  #include "trmi.h"
  #include <nv.h>
};            


/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

/* The following value corresponds to the size of the TRMClient structure,
   declared in the trm.cpp library file, and must only be changed if the
   size of the structure in the library is changed. */
#ifndef TEST_FRAMEWORK
#define TRM_BYTES_PER_CLIENT  444
#else /* TEST FRAMEWORK */
#error code not present
#endif /* TEST FRAMEWORK */


/*-----------------------------------------------------------------------------
  Define trm mask bits from NV_TRM_CONFIG_I
-----------------------------------------------------------------------------*/

/* Simultaneous 1x Paging with HDR Acq and HDR Idle */
#define TRMCFG_MSK_1X_HDRACQ_ENABLED_BIT         2

/* Simultaneous 1x Acq and HDR */
#define TRMCFG_MSK_1XACQ_HDR_ENABLED_BIT         3

/* Simultaneous 1X and DO */
#define TRMCFG_MSK_SVDO_ENABLED_BIT              4

/* Simultaneous 1X and LTE */
#define TRMCFG_MSK_SVLTE_ENABLED_BIT             5

/* Dual Subscription Dual Active */
#define TRMCFG_MSK_DSDA_ENABLED_BIT              6

/* Simultaneous GSM and LTE */
#define TRMCFG_MSK_SGLTE_SGTDS_ENABLED_BIT       7

/*-----------------------------------------------------------------------------
  Define Simultaneous 1x HDR Modes from mask in NV_TRM_CONFIG_I
-----------------------------------------------------------------------------*/
typedef enum
{
  /* The default SHDR mode for the build is on */
  TRMCFG_MODE_S1XHDR_UNSET       = 0,
  
  /* SHDR is off. Normal hybrid mode is turned on */
  TRMCFG_MODE_S1XHDR_DISABLED    = 1,
  
  /* SHDR in Access, Traffic, and BCMCS is turned on */
  TRMCFG_MODE_S1XHDR_ENABLED     = 2,
  
  /* SHDR in BCMCS is turned on */
  TRMCFG_MODE_S1XHDR_BCMCS_ONLY  = 3,

  /* Number of possible modes */
  TRMCFG_MODE_S1XHDR_NUM
}
trmcfg_mode_s1xhdr_enum_type;

/*-----------------------------------------------------------------------------
  TRM Constants & Configuration Class
-----------------------------------------------------------------------------*/

class TRM
{
public:

  struct Memory
  {
    /* Memory buffer, for TRM usage */
    uint8 *                       buffer;

    /* Size of TRM's memory buffer */
    uint32                        size;
  };

  typedef struct 
  {
    uint32                mask;
    uint32                dual_receive_bitmask;
    boolean               independent_chains;
    boolean               pam_status;
  } Set_Compatibility_Info;

  /* Number of clients in trm_client_enum_t */
  static const uint32             num_clients;

  /* TRM configuration nv item id */
  static const nv_items_enum_type nv_trm_config;

  /* GPS RF configuration nv item id */
  static const nv_items_enum_type nv_gps_rf_config;

  /* Configurable-Size Storage for TRM library */
  static Memory                   memory;

  /* Client/Reason to priority map */
  static trm_pri_t                pri(trm_client_enum_t c, trm_reason_enum_t r);

  /* Client/Reason to compatibility mode map */
  static trm_compatible_mask_t    get_compatibility_mode(
                                    trm_client_enum_t     client,
                                    trm_reason_enum_t     reason);

  /* Get compatibility mask for the client/reason pair */
  static trm_compatible_mask_t    get_compatibility(
                                    trm_client_enum_t     client,
                                    trm_reason_enum_t     reason);

  /* Client to log code map */
  static uint8                    client_log_id(trm_client_enum_t c);

  /* Storage */
  static void *                   get_storage(uint32 size);

  /* Identify which clients can run simultaneously with other clients */
  static void                     set_compatible_clients(Set_Compatibility_Info info);


  /* Enable the specified mode */
  static void                     enable_mode(trm_mode_enum_t mode);

  /* Disable the specified mode */
  static void                     disable_mode(trm_mode_enum_t mode);
  
  /* Check is SHDR is enabled */
  static boolean                  is_shdr_enabled(uint32 mask);

};

#endif /* !TRMCFG_H */
