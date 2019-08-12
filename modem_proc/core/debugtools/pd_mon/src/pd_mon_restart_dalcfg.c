/** vi: tw=128 ts=3 sw=3 et
@file pd_dump.c
@brief This file contains the API details for the Protection Domain Monitor, API 1.0
*/
/*=============================================================================
NOTE: The @brief description above does not appear in the PDF.
The tms_mainpage.dox file contains the group/module descriptions that
are displayed in the output PDF generated using Doxygen and LaTeX. To
edit or update any of the group/module text in the PDF, edit the
tms_mainpage.dox file or contact Tech Pubs.
===============================================================================*/
/*=============================================================================
Copyright (c) 2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
Edit History
$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/pd_mon/src/pd_mon_restart_dalcfg.c#1 $
$DateTime: 2016/12/13 07:59:23 $
$Change: 11985146 $
$Author: mplcsds1 $
===============================================================================*/

#include "pd_mon_restart.h"
#include "DALSys.h"
#include "DALPropDef.h"

/**
Initialize PD MON Restart Dal Config and check if PD restart is enabled.
@return
0 - PD restart Disabled.
1 - PD restart Enabled

*/

uint32 pd_mon_restart_dalcfg_check(void)
{
  DALSYSPropertyVar propValue;
  DALSYS_PROPERTY_HANDLE_DECLARE(pHandle);

  DALSYS_GetDALPropertyHandleStr("pd_mon_restart_dalcfg",pHandle);

  // Check for devcfg PD restart enabled
  if (DAL_SUCCESS == DALSYS_GetPropertyValue(pHandle,"pd_mon_restart_enable", 0, &propValue))
  {
    
    if(1 == propValue.Val.dwVal)
    {
      return 1;
    }
    return 0;
  }

  return 0;
}
