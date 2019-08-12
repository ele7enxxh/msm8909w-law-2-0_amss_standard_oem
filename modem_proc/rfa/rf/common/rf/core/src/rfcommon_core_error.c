/*!
   @file
   rfcommon_core_error.c

   @brief
    This file contains the external definition of the rf_error function,
    which is used in the RF_ASSERT and RF_FATAL_ERROR macros.


   @details
   
*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/rf/core/src/rfcommon_core_error.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/12   sar     Feature reduction changes.
01/28/10   mkv/pl  Merge mdm9k bring to main\latest
08/20/09   can     Fix LTE compile errors.
03/15/09   ckl     Changed comments to compatible with Doxgen
12/14/05   sar     Eliminated lint error.
10/11/05   bmg     Initial Revision. 
===========================================================================*/

#include "rfa_variation.h"
#include "err.h"
#include "msg.h"
#include "rfcommon_core_error.h"

/*----------------------------------------------------------------------------*/
/*!
  @brief

  @details
*/
void rfcommon_core_error( const unsigned int line, const char *file, 
                          const char *message )
{

  
  MSG_SPRINTF_2( MSG_SSID_RF, 
                 MSG_LEGACY_ERROR, 
                 "RF library error occurred in file %s at line %d",file,line );
  #if defined(FEATURE_GSM) || defined (FEATURE_CDMA1X) || defined(FEATURE_WCDMA)
  err_put_log ( line, file ); /*lint !e734*/
  #endif

}


