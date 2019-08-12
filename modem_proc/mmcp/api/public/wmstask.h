#ifndef WMSTASK_H
#define WMSTASK_H

/**
  @file wmstask.h
  @brief Provide Modem AU level API to initialize wms task.

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The WMS_API_mainpage.dox file contains all file/group descriptions that 
      are in the output PDF generated using Doxygen and LaTeX. To edit or 
      update any of the file/group text in the PDF, edit the 
      WMS_API_mainpage.dox file or contact Tech Pubs.

===========================================================================*/

/*
Copyright (c) 2009-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/wmstask.h#1 $ $DateTime: 2016/12/13 08:00:05 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/14/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.

===========================================================================*/

#include "comdef.h"

/** @addtogroup client_group
@{ */

/**
  Internal function that initializes the WMS task ??does all the edited content 
  for this function read OK??.

  @param taskId [IN] Task ID.

  @return
  None.

  @dependencies
  None.
*/
extern void wms_task( uint32 taskId );

/** @} */ /*end_addtogroup client_group */

#endif
