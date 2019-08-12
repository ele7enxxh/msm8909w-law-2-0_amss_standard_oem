/*!
  @file
  rfc_card.c

  @brief
  This file contains functions to support the RF Card (RFC) module which
  manages the RF HW configuration.

  @details
  RF Configuration file is specific to RF CARD (or HW).  The configuration data
  is implemented as sets of data structures pointers within a configuration
  data tree.  To support configuration flexibility, the pointers are assigned
  based on path, mode and band selections.  Thus, each path can operate
  independently with regards to mode and band configurations.

  The framework for assignments of the function pointers are provided by the
  functions in this file.  The data configuration structures which are specific
  to each RF Card needs to be maintained in the card specific RFC file.

  A top level RF Card configuration pointer is rfc_config.  Within that
  structure are 3 pointers representing the mode independent rf core data
  parameters, set of feature data parameters and a top level mode dependent set
  of configuration pointers.

  Within the each mode configuration, there are data structures for gpio/grfc
  settings and any addition core and feature parameters which are specfic to
  the mode for this card.

  In addition, each mode can have independent GPIO/GRFC and configuration
  settings which are band specific.

  rfc_config*
     |
     +-- rfc_core_config_ptr->core data struct[]
     |
     +-- rfc_feature_config_ptr->feature data struct[]
     |
     +-- rfc_<mode>_ptr[path]-> rfc_<mode>_gpio_config_ptr
                             -> rfc_<mode>_core_config_ptr
     .                       -> rfc_<mode>_feature_config_ptr
     .                       -> rfc_band_rx_config_ptr->gpio/grfc
     .                       -> rfc_band_tx_config_ptr->gpio/grfc

   Common settings between modes or bands can utilize common configuration
   structures to reduce the overall amount of configuration data structures..
   In other words, it will not be uncommon to have multiple pointers reference
   to the same data structure for common configurations within the same card
   file.
*/

/*===========================================================================

Copyright (c) 2008 - 2011 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/rfc/src/rfc_card.c#1 $

when       who     what, where, why
------------------------------------------------------------------------------- 
12/05/11   sar     Fixed featurization for non-lte build. 
10/13/11   tnt     Add configurable falling/rising sampling edge support 
08/18/11   dw      Move rfc_command_dispatch() to rfc.cpp
08/15/11   aak     New RFC interface for rfc_get_awake_resource_request()  
08/10/11   pl      Remove all references to legacy RFC structure and related API
08/06/11   sr      RFC Cleanup: removed un-used code.
08/05/11   sty     Code cleanup for CDMA
07/12/11   vb      rfc cleanup and rfc interface changes to suit the new auto-gen 
                   of rf card files
07/11/11   cd      Removed support for legacy RFC functionality for CDMA
06/01/11   aro     Fixed SV Chain RFC query
05/27/11   bmg     ANSI C fix
05/26/11   aro     Compiler Warning Fix
05/26/11   aro     Compiler Warning Fix
05/26/11   aro     Added debug flag for device switch in SV mode
05/23/11   aak     Added rfc_get_awake_resource_request()
03/15/11   sty     Deleted rfc_get_mode_data
03/03/11   ap      Adding Set Dynamic Voltage Scaling Update API 
02/17/11   sty     Renamed rfc_card_cdma.h to rfc_cdma.h
01/25/11   sr      removed the clk.h dependency.  
01/18/11   pl      Clean up
01/10/11   dw      WCDMA new RFC design integration
01/10/11   sty     Deleted rfc_default.h
01/10/11   sty     Changed args passed to rfc_cdma_get_rx_band_config
01/07/11   sty     Mainlined RF_GRFC_COMMON_DESIGN_INTERIM_FEATURE
01/06/11   sty     Check against deprecated API calls to CDMA, cleanups
01/06/11   sty     Used rfc_card_cdma.h instead of rfc_card_1x.h
01/04/11   sty     Changed all rfc_1xhdr_* to rfc_cdma_*
01/04/11   pl      LTE RFC re-arch -- remove LTE from legacy interface
12/20/10   sty     Replaced rf_path_enum_type with rfm_path_enum_type
12/20/10   sty     used new variables and .h file
10/19/10   vb      Removed rfdevice_interface.h 
09/13/10   shb     Deleted rfc_manage_secondary_vco()
09/01/10   av      GSM RFC Rearch
07/29/10   aro     Common GRFC Design Implementation for 1xHDR
07/08/10   tnt     RFC re-architecture to support GRFC
05/06/10   ac      tx issue fix
04/01/10   pl      Featurize WCDMA specific cases properly
03/19/10   lcl     Pull in changes from QSC6295
03/15/10   tws     Add protection to rfc fucntions to stop null pointer access
03/03/10   Vish    Merged from prerel/poseidon2.1.x: 01/18/10 - 02/12/10
02/12/10   xw      Null pointer check before gpio_grfc_config is dereferenced 
01/28/10   sar     Removed Lint warnings.
01/26/10   sar     Lint fixes.
01/18/10   sr      Added new rfc_config_tx_band_data() api
12/2/09    can     Added rfc_lte_get_tx_band_config.
11/12/09   aro     Added Check for RFCOM_1XEVDO_MODE
09/11/09   ad      Fix WPLT linker errors due to undefined GSM symbols  
08/25/09   aro     RFC 1x Design Update : Changed the function/variable name to 
                   have "1xhdr" instead of just "1x"
08/21/09   bmg     Featurized LTE RF card support for non-LTE builds
12/29/09   av      Remove rfc_is_set variable declaration. Re-format Cleanup.
12/15/09   av      RFC Re-format
12/08/09   ntn     temporarily forcing rfc_command_dispatch() to return 1 to appease warnings 
11/25/09   sr      Deleted redundant API - rfc_manage_vreg_for_1x()
11/15/09   aak     Remove access to RF vreg in rfc_manage_vreg_for_1x(). 
11/10/09   aak     Uncommented out the call to rfc_vreg_funct_ptr() within rfc_manage_vregs() 
10/27/09   sr      Added rfc_manage_vreg_for_1x()
10/06/09   sr      Fixed warnings
08/11/09   sr      Added case for RFCOM_1XEVDO_MODE
07/12/09   jps     Updated file to support QRF8600 RFC
07/08/09   av      Cleanup and added port configuration (RTR) data  in RFC
06/09/09   gy      Add gen8 (Callisto) support
05/28/09   shb     Support for 1x GPIO mode function pointer
05/26/09   bmg     Added null checking for rfc_manage_vregs data structure
                   traversal.
04/16/09   dbz     Add manage_vreg instance for GPS
03/25/09   sr      Check for NULL ptr reference before using it.
03/15/09   dyc     Default qsc6x95_global platform
03/13/09   sar     Changes needed for porting to SCMM
03/09/09   clk     Code cleanup
01/30/09   av      Support for default card file.
01/30/09   av      Support for MDM8200_Global Target
01/30/09   av      Added data structure and functions for Common
                   MC and core support.
11/21/08   dyc     Design updates.
11/10/08   dyc     Initial version.

============================================================================*/

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rfa_variation.h"
#include "rfc_card.h"
#ifdef FEATURE_WCDMA
#include "rfc_card_wcdma.h"
#endif /* FEATURE_WCDMA */
#if defined(FEATURE_GSM) && !defined(FEATURE_WPLT)
#include "rfc_card_gsm.h"
#endif /* FEATURE_GSM */
#ifdef FEATURE_CGPS
#include "rfc_card_gnss.h"
#endif /* FEATURE_CGPS */
#ifdef FEATURE_LTE
#include "rfc_card_lte.h"
#endif

#include "rfm_internal.h"

#include "rfcommon_mc.h"
#include "msg.h"
#include "pm.h"
/*===========================================================================
                           Data Definitions
===========================================================================*/
/*Data structure to hold all the protocol parameter for RAFT*/
rfc_spi_ant_tuner_protocol_type rfc_spi_ant_tuner_para = 
                                             {0,      /* SPI = 1, RFFE = 2, 0 = o.w. */
                                              0,      /* 1 = clk to slave is always on between transaction */
                                              0,      /* number of bits per transaction */
                                              0,      /* deassert between transaction, clk cycle*/
                                              0,      /* 1= cs active hi, 0 = cs active low */
                                              0,      /* GSBI core id */
                                              0,      /* first CS  */
                                              0,      /* second CS */
                                              0,      /* 1=send initialization codeword, 0 = o.w. */
                                              0,      /* 1=send shutdown codeword, 0 = o.w. */
                                              0,      /* bus clk rate (KHz) */
                                              0,      /* 1 = MOSI data is sampled on falling edge, 0 = rising edge*/
                                              0,      /*number of bins in a current band*/
                                              0,      /* initialization codeword*/
                                              0};      /* shutdown codeword*/
/*===========================================================================
                           Functions
===========================================================================*/
/* ----------------------------------------------------------------------- */
/*!
  @brief
  Default (empty) function that returns nothing.

  @details
  To be assigned to function pointers when the function pointer needs to point
  to a function that does nothing. Used primarily for initialization of
  function pointers.

  @parameter
  void *

*/
void rfc_empty_func( void* param1, ... )/*lint -esym(715, param1) -e818 unused */
{
    MSG_LOW( "Default (empty) function called in rfc", 0, 0, 0);
}

/* ----------------------------------------------------------------------- */
/*!
  @brief
  Default (empty) function that returns 0.

  @details
  To be assigned to function pointers when the function pointer needs to point
  to a function that does nothing. Used primarily for initialization of
  function pointers.

  @parameter
  void *

  @return
  (int16) 0

*/
int16 rfc_empty_func_return_0( void* param1, ... )/*lint -esym(715, param1) -e818 unused */
{
  MSG_LOW( "Default (empty) function called in rfc", 0, 0, 0);
  return 0;
}