/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                   T C X O    M A N A G E R   C O N T R O L

                           TCXO Manager Source File

GENERAL DESCRIPTION

  This module maintains the routines for client control of the vctcxo.

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by this document
  are confidential and proprietary information of Qualcomm Technologies
  Incorporated and all rights therein are expressly reserved.  By accepting
  this material the recipient agrees that this material and the information
  contained therein are held in confidence and in trust and will not be used,
  copied, reproduced in whole or in part, nor its contents revealed in any
  manner to others without the express written permission of Qualcomm
  Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/tcxomgr/src/tcxomgr_ctl.c#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
01/30/15     tl      Remove obsolete init functions and featurization
05/01/14     tl      Remove obsolete NV function
10/04/13     mn      Support to save and retrieve subscription and system 
                     RGS to/from EFS.
01/29/13     sk      Support for frequency estimate separtion
09/12/13     mn      New Rpush API to support multi-SIM targets.
07/23/13     mn      New API to maintain tech specific RGS.
04/20/13     hh      Updated tcxomgr state when a client releases source
                     before acknowledging.
12/10/12     mn      Logging optimization on tcxomgr messages.
12/10/12     mn      TCXOMGR should flush XO NV contents during powerdown.
11/29/12     sk      support for change of nv item in tcxomr struct to pointer
10/29/12     sk      Added an api to get rpush priority
02/10/12     sr      TCXOMGR Feature Cleanup
02/08/12     sr      Code refractoring and code changes for rcinit.
01/31/12     ns      Dont start IFC in Tcxo mode
01/27/12     ns      Support for IFC
01/15/12     sg      support for fast field cal v3.1
11/17/11     mn      Got rid of the tcb_ptr argument to tcxomgr_nv_init
11/01/11     ns      Add support for Tcxo mode
08/15/11     sg      Added support for TDSCDMA
07/06/11     ag      Remove ADCDAL featurization
05/15/11     ag      Remove npa.h
05/15/11     ag      Remove modem web clock access.
04/26/11     ag      Remove feature wrap around FREQEST client in clients table.
04/19/11     sg      Changed to CX8 compensation based on Polling
04/06/11     sg      Added support for CX8 compensation based on restrictions
30/03/11     ns      New API to return RPUSH frequency
10/02/10     ns      Validate tcxomgr structure
02/03/10     ns      Do not update override value for XO targets
02/03/10     ns      Fast field cal support for 2.5 degree c1 calibration
01/24/11     jg      Fixed LINT high warnings.
12/13/10     ag      Wrap CHIPXN usage under revisit feature for 8960.
12/01/10     ag      Initialize the NV critical section.
11/24/10     hh      CMI phase-4 changes.
11/20/10     ag      Disable PMIC read when GPS is off
11/04/10     sm      Added Support For Fast Field Cal
11/02/10     ns      Remove coarse temp,time and fine temp variable.
10/14/10     hh      Included Coarse and fine cache temp variables.
09/07/10     ag      Disabled occurence of Pmic XO core enable in tcxomgr init
08/20/10     sg      Fixed KW errors
07/19/10     sg      HWIO macro Lint fixes
07/15/10     sg      Compiler warnings and Lint fixes for genesis targets
06/15/10     hh      Register tcxomgr as a client to enable MODEM_WEB_CLK for
                     Poseidon targets.
06/03/10     hh      Register tcxomgr as a client to enable OFFLINE_CLK.
05/17/10     hh      Fixed KW error
04/19/10     ns      Remove MND control from tcxomgr, only exception being
                     initializing MND to ARM control during tcxomgr_init()
04/19/10     ns      Fix KW error
03/26/10     sm      Disable TRK_L0_ADJ for XO Targets
03/26/10     sm      Remove  include of tlmm_gpio.h
03/22/10     sm      TCXOMGR no longer votes for TCXO PDM clock on xo targets
03/25/10     rc      Change MND to ARM control when no technology needs MDSP
03/12/10     bb      Changes to set "CDMA_UMTS_MODE_SEL" HW register with
                     correct values for Genesis Modem
01/20/09     sm      Changed GPIO init code to make use of the DAL API
11/24/09     ag      Call tcxomgr_adc_client_init() for non-XO builds.
09/14/09     sb      Use CDMA_UMTS_MODE_SEL to select TRK_LO in genesis modem
07/29/09     ag      Replacing adc_read with the DAL api.
08/25/09     ns      Replace msm_help.h with msm.h
07/24/09     sb      Check if vco_cb is NULL before using it.
07/22/09     sb      Use the cached temp value instead of direct ADC read
06/19/09     ag      Macros used for critical section which defines to rex
                     critical section on Q6 targets and INTLOCK on non Q6.
05/01/09     ns      Feature wrap set_polarity changes
04/14/09     ns      Clean up to support SCMM and 8650 builds only
04/13/09     ag      Merged from //depot
10/20/08     adw     Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
09/24/08     adw     Modified CLIENT_FREQ_EST featurization to be based on
                     XO_TASK as well as MCS_HAS_NO_GPS.
09/15/08     sg      Fix lint/klocwork warnings
09/15/08     sst     Featurize CLIENT_FREQ_EST under XO_TASK
09/09/08     sg      Don't switch source for PDM clock when FTM_MODE to avoid
                       upsetting RF calibration
08/28/08     adw     Added FEATURE_XO_CTL_1X_ONLY and FEATURE_XO_DIAG to
                     featurize for ULC.
08/12/08     sg      Don't turn off PDM clock when in FTM_MODE to avoid
                       upsetting GSM RF Calibration
06/20/08     mca     Merge from //depot
06/04/08     sg      Added TCXOMGR_SOURCE_POWER_OFF source adn refactored out
                       sleep mode to use this source instead
04/10/08     sg      Notify registered clients of current restriction at
                     callback registration time
03/21/08     sg      enable/disable pdm clock regime in sync with tcxomgr
03/13/08     nnv     Added TCXOMGR_UBM_L1 for tcxomgr priority string array.
11/05/07     cab     Add in processing to notify freq_est of restr changes
10/26/07     cab     Keep restriction set to XO on XO targets
10/02/07     va      Added FREQ_EST client. Removed slope_est stuff.
09/28/07     cab     Set rpush flag before notifying
09/19/07     va      Save rot client state for slope estimation
09/12/07     sst     Added UMB L1 to client list
09/06/07     sst     Lint fixes
09/04/07     cab     Write field cal data on every field cal iteration
06/18/07     aps     Read in fac cal values tcxomgr_init()
06/18/07     va      Support for XO Compensation
06/12/07     cab     Added back in fac cal changes
06/12/07     cab     Reverted fac cal, added rpush_get_flag
06/06/07     sst     Moved xo_trim read/write and processing to _data
05/17/07     cab     Added uncertainty for passive clients
05/16/07     cab     Updated rpush_set_flag to allow multiple client request
05/07/07     cab     Added rotator callback, get_current_rot
03/20/07     cab     Featurize reference to 1x srch header file
03/12/07     cab     Change time value in rpush to signed, include adc.h.
03/02/07     nnv     Modified tcxomgr_rpush and rpush_cb functions for rpush update
03/02/07     nnv     Added code to support XO targets
01/19/07     cab     Added support for xo_trim init and write back at pwr down
11/08/06     cab     Fixed critical sections to prevent dog error
11/03/06     cab     Added rotator pushing functionality
                     Added client TV OUT
10/19/06     cab     Added PPM client
10/12/06     cab     Added priority strings for WCDMA and UMTS
10/02/06     cab     Initial version

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "mcs_variation.h"
#include "customer.h"

#include "modem_mcs_defs.h"

#include "msg.h"
#include "rex.h"
#include "rf.h"
#include "err.h"
#include "timetick.h"

#include "tcxomgr.h"
#include "tcxomgr_i.h"
#include "tcxomgr_rot_client_handling_i.h"
#include "tcxomgr_adc_client.h"

#include "tcxomgr_diag.h"

#include "tcxomgr_xo_trim.h"
#include "pm.h"

/*============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

============================================================================*/

/*---------------------------------------------------------------------------
  TCXO Manager local variables
---------------------------------------------------------------------------*/

tcxomgr_client_type clients[TCXOMGR_NUM_CLIENTS] =
{
  /* Client TCXOMGR_CLIENT_NONE */
  {
    TCXOMGR_CLIENT_NONE,         /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "NONE",                      /* Name */
    TCXOMGR_SOURCE_OVERRIDE,     /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_GPS */
  {
    TCXOMGR_CLIENT_GPS,          /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "GPS",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_BT */
  {
    TCXOMGR_CLIENT_BT,           /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "BLUETOOTH",                 /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_MEDIA_FLO */
  {
    TCXOMGR_CLIENT_MEDIA_FLO,    /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "MEDIA FLO",                 /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_GSM */
  {
    TCXOMGR_CLIENT_GSM,          /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "GSM",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_WCDMA */
  {
    TCXOMGR_CLIENT_WCDMA,        /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "WCDMA",                     /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_CDMA_1X */
  {
    TCXOMGR_CLIENT_CDMA_1X,      /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "1X",                        /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_CDMA_HDR */
  {
    TCXOMGR_CLIENT_CDMA_HDR,     /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "HDR",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_PPM */
  {
    TCXOMGR_CLIENT_PPM,          /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "PPM",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_TV_OUT */
  {
    TCXOMGR_CLIENT_TV_OUT,       /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "TV OUT",                    /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_WLAN */
  {
    TCXOMGR_CLIENT_WLAN,         /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "WLAN",                      /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_UBM_L1 */
  {
    TCXOMGR_CLIENT_UBM_L1,       /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "UBM L1",                    /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_LTE */
  {
    TCXOMGR_CLIENT_LTE,       /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "LTE ML1",                   /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_TDSCDMA */
  {
    TCXOMGR_CLIENT_TDSCDMA,      /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "TDSCDMA",                   /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  },

  /* Client TCXOMGR_CLIENT_IFC */
  {
    TCXOMGR_CLIENT_IFC,          /* Client ID */
    TCXOMGR_DEFAULT_PRIORITY,    /* Priority */
    "IFC",                       /* Name */
    TCXOMGR_SOURCE_NONE,         /* Source */
    TCXOMGR_RESTRICTION_NONE,    /* Restriction Level */
    FALSE,                       /* Response Flag */
    NULL,                        /* VCO CallBack */
    NULL,                        /* Grant CallBack */
    NULL,                        /* Change CallBack */
    NULL,                        /* Release CallBack */
    NULL,                        /* Deny CallBack */
    NULL,                        /* Notify CallBack */
    NULL,                        /* Rotator Push Enable/Disable */
    NULL,                        /* Rotator Push Data Xfer */
    NULL                         /* Rotator CallBack*/
  }

};

/* The global storage object for tcxomgr */
tcxomgr_struct_type tcxomgr;

/*global nv structure for tcxomgr nv operations*/
tcxomgr_nv_struct_type  tcxomgr_nv;

/* Keep sections of code safe from task switching */
rex_crit_sect_type     tcxomgr_crit_sect;

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_SET_POLARITY

DESCRIPTION
  This function sets the polarity as requested and also sets the PDM value.
  The function should be called by the technology team (1x/HDR/WCDMA/GSM/....)
  to set the polarity required by it. It also changes the PDM value to reflect
  the new polarity. If its an XO build, it just returns success.

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_trk_lo_return_enum_type which specifies if the polarity change was
  successful, could not change the polarity or an invalid polarity was passed.

SIDE EFFECTS
  None.

============================================================================*/

tcxomgr_trk_lo_return_enum_type tcxomgr_set_polarity
(
  tcxomgr_trk_lo_enum_type    polarity
)
{
  tcxomgr_trk_lo_return_enum_type tcxomgr_state_flag;

  /* Initialize to success */
  tcxomgr_state_flag = TCXOMGR_TRK_LO_POLARITY_SUCCESS;

  return tcxomgr_state_flag;

} /* tcxomgr_set_polarity */


/*============================================================================

FUNCTION TCXOMGR_GET_CURRENT_VCO

DESCRIPTION
  This function gets the VCO value from the active client controlling
  VCO by calling its registered callback function.  If no one is controlling
  the txcomgr override value is returned.

DEPENDENCIES
  None

RETURN VALUE
  VCO value from the active client or the frozen value if no active.

SIDE EFFECTS
  None.

============================================================================*/

tcxomgr_vco_type tcxomgr_get_current_vco( void )
{
  if ( tcxomgr.client_in_control >= TCXOMGR_NUM_CLIENTS )
  {
    ERR_FATAL( "Invalid client_in_control: %d",
                (int) tcxomgr.client_in_control, 0, 0 );
  }

  if ( tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE )
  {
    /* TBD:  does it matter if we are sleeping or not */
    return tcxomgr.override_val;
  }
  else
  {
    /* Client must have vco_cb, so vco can be retrieved by other clients */
    if ( clients[tcxomgr.client_in_control].vco_cb == NULL )
    {
      /* We really should not be here. */
      /* Should trap the situation where callbacks */
      /* are not set for controlling client */
      DBG_1( ERROR, "Client %d, no vco_cb",
                tcxomgr.client_in_control );
      return tcxomgr.override_val;
    }
    else
    {
      /* Callback function to the active client to update the log packet */
      return clients[tcxomgr.client_in_control].vco_cb();
    } /* if vco_cb == NULL */
  } /* if client in control is NONE */

} /* tcxomgr_get_current_vco */


/*============================================================================

FUNCTION TCXOMGR_GET_CURRENT_ROT

DESCRIPTION
  This function gets the ROT value from the active client by calling its
  registered callback function.  If no one is controlling the rgs value is
  returned.

DEPENDENCIES
  None

RETURN VALUE
  ROT value from the active client or the rgs ROT value if no active.

SIDE EFFECTS
  None.

============================================================================*/

tcxomgr_rotator_type tcxomgr_get_current_rot( void )
{
  tcxomgr_rgs_struct_type system_rgs;

  if ( tcxomgr.client_in_control >= TCXOMGR_NUM_CLIENTS )
  {
    ERR_FATAL( "Invalid client_in_control", (int) tcxomgr.client_in_control,
                0, 0 );
  }

  if ( tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE )
  {
    /* TBD:  does it matter if we are sleeping or not */
    tcxomgr_get_system_rgs(&system_rgs);

    return system_rgs.rot;
  }
  else
  {
    /* Callback function to the active client to get the rot val */
    return clients[tcxomgr.client_in_control].rot_cb();
  }

} /* tcxomgr_get_current_rot */


/*============================================================================

FUNCTION TCXOMGR_WRITE_MUX

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

============================================================================*/
void tcxomgr_write_mux
(
  tcxomgr_source_type    source
)
{
  TCXOMGR_ENTER_ISR_MSM_LOCK_SECT(tcxomgr_crit_sect);

  if ( source != TCXOMGR_SOURCE_POWER_OFF )
  {
    if ( tcxomgr.vco_source == TCXOMGR_SOURCE_POWER_OFF )
    {
      tcxomgr_vco_info_type rgs_vco;

      /* Freeze the vco with the best guess value */
      rgs_vco = tcxomgr_get_stored_vco();
      tcxomgr.override_val = rgs_vco.vco_value;
    }
  }

  tcxomgr.vco_source = source;
  TCXOMGR_LEAVE_ISR_MSM_LOCK_SECT(tcxomgr_crit_sect);

} /* tcxomgr_write_mux */


/*============================================================================

FUNCTION TCXOMGR_SEND_NOTIFICATIONS

DESCRIPTION
  This function sends a notification to all clients that have registered a
  notification callback (assuming the notification level has changed).

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
void tcxomgr_send_notifications
(
  tcxomgr_restriction_type  restriction
)
{
  uint8 i;
  /*------------------------------------------------------------------------*/

  if ( restriction != tcxomgr.working_restr )
  {
    /* Send notification to all clients who've signed up (safe to skip NONE) */
    for (i=(uint8)TCXOMGR_CLIENT_NONE+1; i<(uint8)TCXOMGR_NUM_CLIENTS; i++)
    {
      /* If client has registered to receive notification of this restr level */

      if (clients[i].notify_cb != NULL)
      {
        tcxomgr.cb_id = (tcxomgr_client_id_type)i;
        (clients[i].notify_cb)( restriction );
      }
    }

    tcxomgr.working_restr = restriction;
  }

} /* tcxomgr_send_notifications */


/*============================================================================

FUNCTION TCXOMGR_GET_RESTRICTION

DESCRIPTION

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
tcxomgr_restriction_type tcxomgr_get_restriction
(
  tcxomgr_client_id_type    id
)
{

  tcxomgr_restriction_type restriction = TCXOMGR_RESTRICTION_NONE;

/*--------------------------------------------------------------------------*/

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* If it is XO target, restriction is always TCXOMGR_RESTRICTION_XO */
  restriction = TCXOMGR_RESTRICTION_XO;

  if (restriction != tcxomgr.curr_restr )
  {
    if((restriction < TCXOMGR_MAX_RESTRICTION) && (tcxomgr.curr_restr < TCXOMGR_MAX_RESTRICTION))
    {
      DBG_2( LOW, "Restriction has changed: best %d and current %d",
              restriction, tcxomgr.curr_restr);
    }
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

  return restriction;

}


/*============================================================================

FUNCTION TCXOMGR_GET_WORKING_RESTRICTION

DESCRIPTION

  Return the current working restriction of the TCXO manager.

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
static tcxomgr_restriction_type tcxomgr_get_working_restriction( void )
{
  return tcxomgr.working_restr;
} /* tcxomgr_get_working_restriction */


/*============================================================================

FUNCTION TCXOMGR_FIND_BEST_CLIENT

DESCRIPTION
  This function searches the list for the client with the highest service
  level.

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

============================================================================*/
static tcxomgr_client_id_type tcxomgr_find_best_client( void )
{

  uint8 i;
  tcxomgr_client_id_type best_client = TCXOMGR_CLIENT_NONE;
  tcxomgr_priority_type temp_priority = TCXOMGR_NO_PRIORITY;

/*--------------------------------------------------------------------------*/

  /* Search for those that want control, and find the one with highest pri */
  for (i = 0; i <(uint8)TCXOMGR_NUM_CLIENTS; i++)
  {
    if ( (clients[i].source != TCXOMGR_SOURCE_NONE) &&
         (clients[i].priority > temp_priority) )
    {
        best_client   = (tcxomgr_client_id_type)i;
        temp_priority = clients[i].priority;
    }
  }

  /* If there are multiple at the same priority, give precedence to
     client in control */
  if ( (tcxomgr.client_in_control < TCXOMGR_NUM_CLIENTS) &&
       (clients[best_client].priority <=
         clients[tcxomgr.client_in_control].priority)  )
  {
    best_client = tcxomgr.client_in_control;
  }

  return best_client;

} /* find_best_client */


/*============================================================================

FUNCTION TCXOMGR_PROCESS_CHANGES

DESCRIPTION
  This function process the current list state and sends cmd to new
  client if necessary

DEPENDENCIES
  TCXO Manager must be initialized.

RETURN VALUE
  None

SIDE EFFECTS
  Possibly a new best active client, and a command will be sent.

============================================================================*/
static void tcxomgr_process_changes( void )
{

  tcxomgr_restriction_type restriction;
  uint16 i;

/*--------------------------------------------------------------------------*/
  if ( tcxomgr.client_in_control >= TCXOMGR_NUM_CLIENTS )
  {
    ERR_FATAL( "Invalid client_in_control", (int) tcxomgr.client_in_control,
                0, 0 );
  }

  tcxomgr.best_client = tcxomgr_find_best_client();

  /* Currently have a controlling client, but a new client has pre-empted */
  if ( (tcxomgr.client_in_control != TCXOMGR_CLIENT_NONE) &&
       (tcxomgr.client_in_control != tcxomgr.best_client) )
  {
    if ( clients[tcxomgr.client_in_control].release_cb != NULL )
    {
      /* Set state to rel pending so other service reqs will be put on hold */
      tcxomgr.state = TCXOMGR_REPLY_PENDING;

      /* Record id of who should be acking us shortly */
      tcxomgr.ack_client = tcxomgr.client_in_control;

      DBG_1( LOW, "Client %d release",
               tcxomgr.client_in_control );

      /* Call the client's release callback function */
      tcxomgr.cb_id = tcxomgr.client_in_control;
      (clients[tcxomgr.client_in_control].release_cb) ();
    }
    else
    {
      DBG_1( ERROR, "Client %d release callback is NULL",
                tcxomgr.client_in_control );
    }
  }

  /* Currently no controlling client, but one has requested control */
  else if ( (tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE) &&
            (tcxomgr.best_client != TCXOMGR_CLIENT_NONE) )
  {
    if ( clients[tcxomgr.best_client].grant_cb != NULL )
    {
      /* Set state to ack so other service reqs will be put on hold */
      tcxomgr.state = TCXOMGR_REPLY_PENDING;

      /* Record id of who should be acking us shortly */
      tcxomgr.ack_client = tcxomgr.best_client;

      /* Obtain highest restriction, excluding the best client */
      restriction = tcxomgr_get_restriction( tcxomgr.best_client );

      /* Note that this restriction can be changed by the client */
      /* ACK should take care of it                              */
      tcxomgr.curr_restr = restriction;

      DBG_3( LOW, "Client %d grant:  %d, %d",
                tcxomgr.best_client,
                clients[tcxomgr.best_client].source,
                restriction );

      /* Call the client's grant callback function to send command */
      tcxomgr.cb_id = tcxomgr.best_client;
      (clients[tcxomgr.best_client].grant_cb)
        ( clients[tcxomgr.best_client].source, restriction,
          tcxomgr.override_val );

      clients[tcxomgr.best_client].resp_req = FALSE;
    }
    else
    {
      DBG_1( ERROR, "Client %d grant callback is NULL",
                tcxomgr.client_in_control );
    }
  }

  /* A client has changed restr level while another client is in control? */
  else if ( tcxomgr.client_in_control != TCXOMGR_CLIENT_NONE )
  {
    /* Obtain highest restriction, excluding the controlling client */
    restriction = tcxomgr_get_restriction( tcxomgr.client_in_control );
    if ( restriction != tcxomgr.curr_restr )
    {
      tcxomgr.curr_restr = restriction;
      if ( clients[tcxomgr.best_client].change_cb != NULL )
      {
        /* Set state to ack so other service reqs will be put on hold */
        tcxomgr.state = TCXOMGR_REPLY_PENDING;

        /* Record id of who should be acking us shortly */
        tcxomgr.ack_client = tcxomgr.client_in_control;

        DBG_3( LOW, "Client %d change:  %d, %d",
                  tcxomgr.client_in_control,
                  clients[tcxomgr.client_in_control].source,
                  restriction );

        /* Call the client's change callback function */
        tcxomgr.cb_id = tcxomgr.client_in_control;
        (clients[tcxomgr.client_in_control].change_cb)
          ( clients[tcxomgr.client_in_control].source, restriction );
      }
      else
      {
        DBG_1( ERROR, "Client %d change callback is NULL",
                  tcxomgr.client_in_control );
      }
    }
  }

  /* Cycle through clients and notify denial to those that need it */
  for (i = 0; i < (uint8)TCXOMGR_NUM_CLIENTS; i++)
  {
    if ( clients[i].resp_req && (i != (uint8)(tcxomgr.best_client)) )
    {
      DBG_1( LOW, "Client %d denied", i );
      (clients[i].deny_cb) ();
      clients[i].resp_req = FALSE;
    }
  }

  /* No controlling client, no client needs control, tcxomgr takes over */
  if ( (tcxomgr.client_in_control == TCXOMGR_CLIENT_NONE) &&
       (tcxomgr.best_client == TCXOMGR_CLIENT_NONE) )
  {
    /* TODO: is this unconditionally idle? */
    tcxomgr.state = TCXOMGR_IDLE;
    restriction = tcxomgr_get_restriction( TCXOMGR_CLIENT_NONE );

    /* If highest restriction is NONE... */
    if (  restriction == TCXOMGR_RESTRICTION_NONE )
    {
      /* Allowed to rail the PDM */
      tcxomgr_send_notifications( TCXOMGR_RESTRICTION_NONE );

      tcxomgr_write_mux( TCXOMGR_SOURCE_POWER_OFF );
    }
    else if (  restriction != TCXOMGR_RESTRICTION_XO )
    {
      /* Client has requested that tcxo remain active, no sleep */
      tcxomgr_send_notifications( TCXOMGR_RESTRICTION_FROZEN );

      tcxomgr_write_mux( TCXOMGR_SOURCE_OVERRIDE );
    }
  }

  tcxomgr.restr_changed = FALSE;
  tcxomgr.pending_request = FALSE;

} /* tcxomgr_process_changes */


/*============================================================================

FUNCTION TCXOMGR_SET_CALLBACKS

DESCRIPTION
  Sets the callback functions for the client indicated by id.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs callback functions into the clients[] data array

============================================================================*/

void tcxomgr_set_callbacks
(
  tcxomgr_client_id_type  id,
  tcxomgr_vco_type (*vco_cb)    (void),
  void             (*grant_cb)  (tcxomgr_source_type, tcxomgr_restriction_type,
                                 tcxomgr_vco_type),
  void             (*change_cb) (tcxomgr_source_type, tcxomgr_restriction_type),
  void             (*release_cb)(void),
  void             (*deny_cb)   (void),
  void             (*notify_cb) (tcxomgr_restriction_type),
  void             (*rpush_flag_cb) (boolean),
  void             (*rpush_cb)      (tcxomgr_client_id_type,
                                     tcxomgr_vco_type,
                                     tcxomgr_rotator_type,
                                     uint32,
                                     timetick_type,
                                     timetick_type,
                                     int32,
                                     tcxomgr_client_state_type),
  tcxomgr_rotator_type (*rot_cb) (void)
)
{
  boolean rpush_flag;

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  rpush_flag = ( tcxomgr_get_rpush_req_mode() != TCXOMGR_RPUSH_MODE_OFF ) ?
          TRUE : FALSE;

  clients[id].vco_cb          = vco_cb;
  clients[id].grant_cb        = grant_cb;
  clients[id].change_cb       = change_cb;
  clients[id].release_cb      = release_cb;
  clients[id].deny_cb         = deny_cb;
  clients[id].notify_cb       = notify_cb;
  clients[id].rpush_flag_cb   = rpush_flag_cb;
  clients[id].rpush_cb        = rpush_cb;
  clients[id].rot_cb          = rot_cb;

  /* Inform the client of the current state */
  if (clients[id].notify_cb != NULL)
  {
    tcxomgr_restriction_type restriction = tcxomgr_get_working_restriction();
    tcxomgr.cb_id = id;
    (clients[id].notify_cb)( restriction );
  }
  if (clients[id].rpush_flag_cb != NULL)
  {
    (clients[id].rpush_flag_cb)(rpush_flag);
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_set_callbacks */


/*============================================================================

FUNCTION TCXOMGR_CHANGE_SOURCE

DESCRIPTION
  For the client specified by id, who must be in control, it indicates a
  desire to change to a new tcxo control source (i.e. switch from DSP
  closed loop control to open loop control via the override register).

DEPENDENCIES
  Caller must have control of the tcxo

RETURN VALUE
  None

SIDE EFFECTS
  Changes the clients[].source data field

============================================================================*/

void tcxomgr_change_source
(
  tcxomgr_client_id_type  id,
  tcxomgr_source_type     source
)
{
  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  if ( tcxomgr.client_in_control == id )
  {
    DBG_2( LOW, "Client %d, change to source %d",
              id, source );

    /* Switch the mux to the new source */
    tcxomgr_write_mux(source);
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);
} /* tcxomgr_change_source */


/*============================================================================

FUNCTION TCXOMGR_SET_SOURCE

DESCRIPTION
  For the client specified by id, it indicates to tcxomgr that the client
  wants to control the tcxo with the source specified.  If the tcxomgr is
  currently involved in an operation with another client, the request is
  flagged and handled at a later time.  If the current client in control
  want to change source, the tcxomgr_change_source API should be used, not
  this one.

DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  Installs source into the clients[].source data field
  Command may be sent to one or more active clients.

============================================================================*/

void tcxomgr_set_source
(
  tcxomgr_client_id_type  id,
  tcxomgr_source_type     source
)
{

  if ( (source >= TCXOMGR_MAX_SOURCE) || (source == TCXOMGR_SOURCE_NONE) )
  {
    DBG_1( ERROR, "Client %d, source invalid", id );
    return;
  }

  /* Client must have vco_cb, so vco value can be retrieved by other clients */
  if ( clients[id].vco_cb == NULL )
  {
    DBG_1( ERROR, "Client %d, no vco_cb", id );
    return;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Make sure the requested service level is from a non-controlling client
     and is a change from the previous source request */
  if ( (id != tcxomgr.client_in_control) && (clients[id].source != source) )
  {
    DBG_2( MED, "Client %d request and notify, source %d",
                id, source );

    /* Set the source and response flag for the client */
    clients[id].source = source;
    clients[id].resp_req = TRUE;

    if ( tcxomgr.state != TCXOMGR_REPLY_PENDING )
    {
      /* Now crunch the client list for any needed changes */
      tcxomgr_process_changes();
    }
    else
    {
      /* If in the middle of an ack, wait until ack comes back */
      tcxomgr.pending_request = TRUE;
    }

  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_set_source */


/*============================================================================

FUNCTION TCXOMGR_SET_PRIORITY

DESCRIPTION
  Allows client specified by id to set his priority in relation to the other
  tcxomgr clients.  The priority is used by tcxomgr to determine who should
  get control of tcxomgr when more than one client requests it at the same
  time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs priority into the clients[].priority data field
  Command may be sent to one or more active clients.

============================================================================*/

void tcxomgr_set_priority
(
  tcxomgr_client_id_type    id,
  tcxomgr_priority_type     priority
)
{

  if ( priority >= TCXOMGR_MAX_PRIORITY )
  {
    DBG_1( ERROR, "Client %d, priority invalid", id );
    return;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  if ( clients[id].priority != priority )
  {
    /* Store the requested priority for this client */
    clients[id].priority = priority;

    /* If someone is in control process the change */
    if ( tcxomgr.client_in_control != TCXOMGR_CLIENT_NONE )
    {
      if ( tcxomgr.state != TCXOMGR_REPLY_PENDING )
      {
        /* Now crunch the client list for any needed changes */
        tcxomgr_process_changes();
      }
      else
      {
        /* If in the middle of an ack, wait until ack comes back */
        tcxomgr.pending_request = TRUE;
      }
    }
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_set_priority */


/*============================================================================

FUNCTION TCXOMGR_SET_RESTRICTION

DESCRIPTION
  This function sets the desired restriction for the client specified by
  id.  The restriction is a request to any controlling client to limit the
  rate of change of tcxo.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Installs restriction into the clients[].restriction data field
  Command may be sent to one or more active clients.

============================================================================*/

void tcxomgr_set_restriction
(
  tcxomgr_client_id_type    id,
  tcxomgr_restriction_type  restriction
)
{

  if ( restriction >= TCXOMGR_MAX_RESTRICTION )
  {
    DBG_1( ERROR, "Client %d, restriction invalid", id );
    return;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  /* Store the requested restriction for this client */
  clients[id].restriction = restriction;

  DBG_2( LOW, "Client %d, restriction set to %d",
            id, restriction );

  if ( tcxomgr.state != TCXOMGR_REPLY_PENDING )
  {
    /* Now crunch the client list for any needed changes */
    tcxomgr_process_changes();
  }
  else
  {
    /* If in the middle of an ack, wait until ack comes back */
    tcxomgr.pending_request = TRUE;
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_set_restriction */


/*============================================================================

FUNCTION TCXOMGR_ACK

DESCRIPTION
  This function is used by client id to acknowledge a recieved grant.  Once
  the acknowledgement is received, tcxomgr sets the mux to the client source
  and notifies all other clients of the restriction being adhered to.  The
  restriction may be different than that sent in the original command.  If new
  data has arrived from another client while waiting on this ack, the new
  request will be processed withing this function context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Hardware muxes will be set to client source
  Command may be sent to one or more active clients.

============================================================================*/
void tcxomgr_ack
(
  tcxomgr_client_id_type    id,
  tcxomgr_source_type       source,
  tcxomgr_restriction_type  restriction
)
{

  /* Range check */
  if ( source >= TCXOMGR_MAX_SOURCE )
  {
    DBG_1( ERROR, "Client %d, ack service level invalid", id );
    return;
  }

  /* Range check */
  if ( restriction >= TCXOMGR_MAX_RESTRICTION )
  {
    DBG_1( ERROR, "Client %d, ack restriction invalid", id );
    return;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  if ( id != tcxomgr.ack_client )
  {
    DBG_1( ERROR, "Client %d, ack received when not granted", id );
  }
  else
  {
    /* Client officially has control of the vco */
    tcxomgr_write_mux( source );
    tcxomgr.client_in_control = id;
    tcxomgr.ack_client = TCXOMGR_CLIENT_NONE;
    tcxomgr.state = TCXOMGR_CONTROLLED;
    /* restriction might have changed. Set the new restriction */
    tcxomgr.curr_restr = restriction;

    /* Notify other clients of new service */
    tcxomgr_send_notifications(restriction);

    /* If a new request/restriction has arrived, process it */
    if ( tcxomgr.pending_request )
    {
      tcxomgr_process_changes();
    }
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_ack */


/*============================================================================

FUNCTION TCXOMGR_RELEASE

DESCRIPTION
  This function is used by client id to release control of the tcxo.  If the
  client voluntarily does this, then other lower priority clients may be
  waiting for control.  If it has been commanded to release (i.e.
  pre-emption), the new higher priority client is waiting to be granted
  service.  In either case, need to process changes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Command may be sent to one or more active clients.
  vco is frozen in hardware and the override register is in control

============================================================================*/
void tcxomgr_release
(
  tcxomgr_client_id_type    id,
  tcxomgr_vco_type          curr_vco_val
)
{
  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  if ( id != tcxomgr.client_in_control )
  {
    /* If client is not in control, may be cancelling a prior request */
    clients[id].source = TCXOMGR_SOURCE_NONE;
    if( id == tcxomgr.ack_client )
    {
      DBG_1( MED, "Client %d cancelled request before ack. Clearing ack flag", id );
      /* Clear pending acknowledgement if a client has called release
         before acknowledging for the grant callback */
      tcxomgr.ack_client = TCXOMGR_CLIENT_NONE;
      tcxomgr.state = TCXOMGR_IDLE;
      /* Check for any new changes */
      tcxomgr_process_changes();
    }
  }
  else
  {
    tcxomgr_write_mux( TCXOMGR_SOURCE_OVERRIDE );

    DBG_2( LOW, "Client %d released, vco frozen to: %d",
              id, tcxomgr.override_val );

    /* Clear pending acknowledgement if a client has called release
       before acknowledging for the change callback or release callback */
    tcxomgr.ack_client = TCXOMGR_CLIENT_NONE;

    clients[id].source = TCXOMGR_SOURCE_NONE;
    tcxomgr.client_in_control = TCXOMGR_CLIENT_NONE;
    tcxomgr.state = TCXOMGR_IDLE;

    /* Check for any new changes, esp. for pre-empting client */
    tcxomgr_process_changes();
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_release */


/*============================================================================

FUNCTION TCXOMGR_REPORT

DESCRIPTION
  The client specified in id issues a report when the restriction to which
  that client is adhering changes for whatever reason.  Whent he client
  requires operation under a new restriction, he issues a report which in
  turn issues a notification to all other client of the change in rate by
  which tcxo is changing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Notification may be sent to one or more active clients.

============================================================================*/
void tcxomgr_report
(
  tcxomgr_client_id_type    id,
  tcxomgr_source_type       source,
  tcxomgr_restriction_type  restriction
)
{

  if ( source >= TCXOMGR_MAX_SOURCE )
  {
    DBG_1( ERROR, "Client %d, report invalid", id );
    return;
  }

  if ( restriction >= TCXOMGR_MAX_RESTRICTION )
  {
    DBG_1( ERROR, "Client %d, ack restriction invalid", id );
    return;
  }

  /* Start of critical section */
  TCXOMGR_ENTER_CRIT_SECT(tcxomgr_crit_sect);

  if ( id == tcxomgr.client_in_control )
  {

    /* Notify other clients of new service */
    tcxomgr_send_notifications( restriction );
  }

  /* End of critical section */
  TCXOMGR_LEAVE_CRIT_SECT(tcxomgr_crit_sect);

} /* tcxomgr_report */


/*============================================================================

FUNCTION FUNCTION TCXOMGR_GET_STRUCT_SIZE

DESCRIPTION
  This function returns the size of tcxomgr struct

DEPENDENCIES

RETURN VALUE
  Size of tcxomgr structure

SIDE EFFECTS
  None

============================================================================*/

size_t tcxomgr_get_struct_size(void)
{
  return sizeof(tcxomgr);
}


/*============================================================================

FUNCTION FUNCTION TCXOMGR_TASKINIT

DESCRIPTION
  Init function for the task.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_taskinit(void)
{
  /* any init for global data should be done here */
  /* Enter Atomic section to initialize the critical section lock. */
  TCXOMGR_ENTER_ATOMIC_SECT();

  if ( tcxomgr.state == TCXOMGR_UNINITIALIZED )
  {
    /* Prevent re-initialization */
    tcxomgr.state = TCXOMGR_INITIALIZING;

    /* create a crit section */
    TCXOMGR_INIT_CRIT_SECT(tcxomgr_crit_sect);

    /*point nv to global nv struct data*/
    tcxomgr.nv = &tcxomgr_nv;

    /* Initialize critical section for NV read/writes */
    rex_init_crit_sect(&tcxomgr.nv->crit_sect);

  }

  /* Leave atomic section */
  TCXOMGR_LEAVE_ATOMIC_SECT();
} /* tcxomgr_taskinit */
