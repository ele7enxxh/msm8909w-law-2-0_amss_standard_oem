#ifndef LM_BUFFER_H
#define LM_BUFFER_H

/*!
   @file
   lm_buffer.h

   @brief
   This file implements the LM Buffer functionality.
   These are platform independent.

*/

/*===========================================================================

Copyright (c) 2010 by Qualcomm Technologies, Inc.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$DateTime: 2016/12/13 07:59:54 $ $Author: mplcsds1 $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/inc/lm_buffer.h#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
04/21/11   vrb     Support for Reading portions of LM Buffer
03/24/11   vrb     LM Intf changes for CMI Compliance
03/04/11   aro     Fixed get_buffer_addr function
02/28/11   aro     Added interface to get buffer address
02/08/11   sty     Added C++ protection
01/21/11   sr      code cleanup based on the code review.
12/14/10   sr      Initial version.

============================================================================*/ 

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/

#include "rxlm_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

class lm_buffer
{
public:
  lm_buffer(uint32 chain, uint32 buffer_id, uint8 *mem_addr, uint32 size);

  lm_status_type allocate(lm_tech_type tech);
  lm_status_type deallocate();
  boolean is_free(uint32 chain);
  uint32 get_buffer_id(void);
  uint32 get_chain(void);
  lm_tech_type get_tech(void);

  lm_status_type update_buffer(uint8 *src_addr, uint32 size, uint32 offset_addr=0);
  lm_status_type read_buffer(uint8 *addr, uint32 size, uint32 offset);

  lm_status_type get_buffer_addr( uint32 *dest_addr, uint32 offset = 0 );

protected:

private:

  lm_buffer_header_type buf_header;
  uint8 *buf_mem_addr;
  uint32 buf_size;
};

#ifdef __cplusplus
}
#endif


#endif /* LM_BUFFER_H */

