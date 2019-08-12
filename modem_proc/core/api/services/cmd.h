#ifndef CMD_H
#define CMD_H

/**
  @file cmd.h
  @brief This file contains a global command header.  This header is used
  as the common header within queues which are used for intertask
  communication.  

*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The Utility_Services_API_mainpage.dox file contains all file/group 
      descriptions that are in the output PDF generated using Doxygen and 
      Latex. To edit or update any of the file/group text in the PDF, edit 
      the Utility_Services_API_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the "utils_services" 
	  group description in the Utility_Services_API_mainpage.dox file. 
===========================================================================*/
/*===========================================================================

                      C O M M A N D    H E A D E R  

DESCRIPTION
  This file contains a global command header.  This header is used
  as the common header within queues which are used for intertask
  communication.  

Copyright (c) 1992-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary. 
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/COMMON/vcs/cmd.h_v   1.0   Aug 21 2000 11:51:04   lpetrucc  $
$Header: //components/rel/core.mpss/3.5.c12.3/api/services/cmd.h#1 $ $DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
   
when       who     what, where, why
--------   ---     -------------------------------------------------------
02/11/11   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
11/09/10   EBR     Doxygenated file.
09/21/98   jct     Incorporate ARM porting changes and update copyright
07/13/92    ip     Initial creation.

===========================================================================*/

#include "rex.h"
#include "queue.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/** @addtogroup utils_services
@{ */

/** @brief Command header type.
*/
typedef struct {
  q_link_type        link;         /**< Queue link. */
  rex_tcb_type       *task_ptr;    /**< Pointer to the requesting task's 
                                        TCB. */
  rex_sigs_type      sigs;         /**< Signal to set upon the command 
                                        completion. */
  q_type             *done_q_ptr;  /**< Queue on which to place this command 
                                        when done. */
} cmd_hdr_type;

/**
  Indicates that the command is done.

  @param[in] cmd_hdr_type Command header type.
  @param[in] hdr_ptr      Pointer to the header.

  @return
  None.

  @dependencies
  None.
*/
extern void cmd_done ( cmd_hdr_type *hdr_ptr );

/** @} */ /* end_addtogroup utils_services */
#endif /* CMD_H */
