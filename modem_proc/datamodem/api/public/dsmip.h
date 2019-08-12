#ifndef _DSMIP_H
#define _DSMIP_H

/**
  @file dsmip.h
   
  @brief
   The Data Services Mobile IP configuration header file
*/
/*===========================================================================

                         D S M I P . H

EXTERNALIZED DATA
   mip_ses_info_array       -  MIP session info array
   mip_fa_info              -  MIP foreign agent info
   mip_pwd_info             -  holds MIP shared secrets entered via UI
   mip_get_session_info_cb  -  callback registered by ui which provides
                               session info for next call
   mip_get_pwd_cb           -  callback registered by ui which provides
                               password info for next call

EXTERNALIZED FUNCTIONS
   mip_config_session()
     Load MIP session defaults from NV to all sessions.
   mip_reg_ses_info_cb()
     Register a callback to fill in MIP session information
   mip_reg_pwd_info_cb()
     Register a callback to fill in passwords for MIP session
   mip_get_rm_nai()
     Retrieves the NAIs for a given MIP profile for the
     tethered device and stores them in the mip session info.  
===========================================================================*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the mip group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2002-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip.h_v   1.9   10 Oct 2002 15:54:16   jayanthm  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dsmip.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
02/28/11    op     Doxygen writeup
08/01/08    mv/sa  Added support for DS NAA refresh.
12/10/07    as     Made MIP constants public with MOBILE_IP feature turned off
06/11/06    as     Added MIP_DLFT_PROFILE_NUM for use by otasp
12/07/06    as     Added new REGSM states, updated the rsminfo struct and
                   defined new mip events for MIP deregistration
11/13/06    ks     Added mip_get_rm_nai() to retrieve the RM_NAI from nv.
07/22/05    kvd    Add support for MIP event generation & report for DCTM;
                   mip_call_event_callback,mip_event_alloc,mip_event_register
03/21/05    ssh    Changes pursuant to the new typedef ps_timer_handle_type
06/22/03    kvd    Added  extern definition for new function mip_reset_config.
06/22/03    kvd    Added  extern definition for new function mip_config_valid.
04/28/04    jd     Added definitions for shared secret lengths, linted file
04/16/04    jd     MIP session keeps track of iface ptr instead of iface id
                   Moved MIP_MAX_SESSIONS into new dsmip_cfg.h
02/09/04    aku    Include dssocket_defs.h instead of ps_iface.h
02/02/04    jd     Remove app id from rsm_info, add iface to session info
06/25/03    jd     Mainlined FEATURE_DS_IS_835_B since all targets using this
                   archive enable the feature.
06/11/03    jd     Added RTT estimate to rsm_info
10/09/02    jay    Moved #defines from resgsm.h
08/16/02    jay    Created a bit field for various MIP related test AT
                   command
07/18/02    jay    Added DMU flag to the MIP session
04/29/02    jd     Created module
===========================================================================*/
/* group: mip */

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
/** @addtogroup constants 
  @{
*/
/*---------------------------------------------------------------------------
  Home Agent IP address constants
---------------------------------------------------------------------------*/
#define UNSET_HA_ADDR       0           /**< Unset home agent IP address. */
#define DYNAMIC_HA_ADDR     0xFFFFFFFF  /**< Dynamic home agent IP address. */

#define DYNAMIC_HOME_ADDR   0           /**< Dynamic home IP address. */

/** @name SPI Definitions
  The security policy index constants are based on the following:\n
  @verbatim

Entities     SPI    Algorithm      Authentication length    Reference
--------     ----   ----------     ---------------------    ----------
 MN/AAA       2      CHAP           16                       RFC 1994
 MN/HA        3      MD5            16                       RFC 1321
  @endverbatim
  @{
*/
#define MIP_CHAP_SPI       (2)          
  /**< SPI for CHAP (MN-AAA). Refer to RFC 1994 @xhyperref{S42,[S42]}. */

#define MIP_MD5_SPI        (3)          
  /**< SPI for MD5 (MN-HA). Refer to RFC 1321 @xhyperref{S43,[S43]}. */

#define MIP_DLFT_PROFILE_NUM (0)        /**< Default profile number. */
/** @} */ /* end_name SPI Definitions */

//TODO Clean up inclusions
//#include "dsmip_cfg.h"
#include "ps_iface_defs.h"
#include "pstimer.h"

/** @} */ /* end_addtogroup constants */

#endif /* _DSMIP_H */
