#ifndef DATA_ERR_SUPPR_H
#define DATA_ERR_SUPPR_H
/*===========================================================================

DESCRIPTION
  File containing macros to deal with various Lint warnings. 

  Copyright (c) 2008  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsmgr/inc/data_err_suppr.h#1 $
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/11/08   dh      Created new file
===========================================================================*/


/** 
** Macro to prevent lint warning 715 'Symbol not referenced' 
** (cleaner than lint suppressing comments)
** Have the macro expand with Lint suppression 
** -e774 Boolean within 'if' always evaluates to False/True 
*/
#include "datamodem_variation.h"
#ifndef DATA_MDM_ARG_NOT_USED
#define DATA_MDM_ARG_NOT_USED(arg) /*lint -save -e774*/if(arg){}/*lint -restore*/
#endif

/**
** Macro to prevent lint warning 818 'Symbol could be declared as pointing to
** const'.
*/
#ifndef DATA_MDM_ARG_NOT_CONST
#define DATA_MDM_ARG_NOT_CONST(arg) /*lint -save -e717 -e506 -e774 */ (arg)=(arg);/*lint -restore*/
#endif

/**
*  Macro to prevent lint warning 750: local macro not
*  referenced.
* 
*/
#ifndef DATA_MDM_SYMBOL_NOT_USED
#define DATA_MDM_SYMBOL_NOT_USED(arg) /*lint -save -e506 -e774*/if(arg){}/*lint -restore*/
#endif



#endif /* DATA_ERR_SUPPR_H */

