/*!
   @file
   lm_buffer.cpp

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
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rflm/dtr/src/lm_buffer.cpp#1 $

when       who     what, where, why
-------------------------------------------------------------------------------
06/24/13   sar     Replaced memcpy calls with safe memscpy. 
09/02/11   vrb     kw fixes 
04/21/11   vrb     Support for Reading portions of LM Buffer
03/04/11   aro     Fixed get_buffer_addr function
02/28/11   aro     Added interface to get buffer address
01/21/11   sr      code cleanup based on the code review.
12/14/10   sr      Initial version.

============================================================================*/ 

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "rflm.h"
#include "comdef.h"
#include <stringl/stringl.h>
#include "lm_buffer.h"
#if (RFLM_FEATURE_BUILD_MODE!=RFLM_FEATURE_BUILD_FW_LIB)
#include "rex.h"
#endif

/*----------------------------------------------------------------------------*/
/*!
   @brief
      lm buffer constructor.
 
   @details
      constructs the lm buffer object. Initializes the buffer address and size
      information of the object.

   @param  'chain' : The Link Mgr Chain this buffer object will be associated
           with. 'buffer_id' : ID of the buffer object which will be associated
           with it.
           'mem_addr' : Address of the lm buffer.
           'size' : size of this lm buffer.
   
*/
lm_buffer::lm_buffer(uint32 chain, uint32 buffer_id, uint8 *mem_addr, uint32 size)
:buf_mem_addr(mem_addr)
{

  buf_header.buffer_id = buffer_id;
  buf_header.free_flag = TRUE;
  buf_header.chain = chain;
  buf_header.tech = 0;

  if (mem_addr == NULL || size == 0)
  {
    buf_header.free_flag = FALSE;   
    buf_size = 0; 
  }
  else
  {
    /* adjust the usable size by removing the size of the header overhead. */
    //there is no header for now:  buf_size = size - sizeof(buf_header);
    buf_size = size;
  }

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      returns TRUE if this buffer is not being used by clients else FALSE.
 
   @details
      returns TRUE if the buffer is free.

   @param  'chain' : The Link Mgr Chain this buffer object will be associated
   with.

   @retval 'TRUE' : if the buffer is free.
           'FALSE' : if the buffer is being used.
   
*/
boolean lm_buffer::is_free(uint32 chain)
{
  boolean status = FALSE;

  if (buf_header.chain == chain)
  {
    status =  buf_header.free_flag;
  }

  return status;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      returns buffer_id associated with this buffer object.
 
   @details
      returns buffer_id associated with this buffer object.

   @param  None.

   @return The buffer_id associated with this LM buffer.
   
*/
uint32 lm_buffer::get_buffer_id()
{
  return buf_header.buffer_id;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      returns lm chain associated with this buffer object.
 
   @details
      returns lm chain associated with this buffer object.

   @return The RXF chain associated with this LM buffer.
   
*/
uint32 lm_buffer::get_chain()
{
  return buf_header.chain;
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      returns technology associated with this buffer object.
 
   @details
      returns technology associated with this buffer object.

   @return The lm_tech_type associated with this LM buffer.
   
*/
lm_tech_type lm_buffer::get_tech()
{
  return (lm_tech_type)(buf_header.tech);
}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Reserves a lm buffer for the specified tech.
 
   @details
      reserves a rxlm buffer for the requested tech. 

   @param  'tech' : the technology client which is requesting the buffer.

   @retval
   LM_BUFFERS_UNAVAILABLE : if there is no LM buffers to allocate.
   LM_SUCCESS             : if the LM buffer is allocated successfully.
   
*/
lm_status_type lm_buffer::allocate(lm_tech_type tech) 
{
  lm_status_type status = LM_SUCCESS;
  
  if (buf_header.free_flag)
  {
    buf_header.tech = tech;
    buf_header.free_flag = FALSE;
    // don't copy the header for now 
    // memcpy(buf_mem_addr, &buf_header, sizeof(buf_header));  
  }
  else
  {  
    status = LM_BUFFERS_UNAVAILABLE;
  }
  
  return status;

}

/*----------------------------------------------------------------------------*/
/*!
   @brief
      Un-reserves a lm buffer.
 
   @details
      Un-reserves a lm buffer. 

   @retval LM_SUCCESS : sucessfully de-allocated the buffer.
   
*/

lm_status_type lm_buffer::deallocate()
{
  lm_status_type status = LM_SUCCESS;
  
  buf_header.free_flag = TRUE;

  // don't copy the header for now.
  // memcpy(buf_mem_addr, &buf_header, sizeof(buf_header));

  return status;
}
/*----------------------------------------------------------------------------*/
/*!
   @brief
      Update a dynamic portion of rxlm buffer.
 
   @details
      Update the dynamic part of the rxlm buffer which was reserved previously
      with new settings based on the new configuration parameters.

   @param  
           'cfg'          : new configuration parameters based on which
                            rxlm buffer need to be updated.

   @retval
   LM_BUFFER_NOT_ALLOCATED : if the LM buffer is not already allocated.
   LM_INCORRECT_BUF_SIZE   : if the requested size of the data execeeds the
                             allocated buffer size.
 
   LM_SUCCESS              : if the buffer is updated with the requested data.
*/
lm_status_type lm_buffer::update_buffer(uint8 *addr, uint32 size, uint32 offset)
{
  uint8 *dest_addr = 0;

  lm_status_type status = LM_SUCCESS;

  // make sure that the buffer is reserved before updating this buffer contents.
  if (buf_header.free_flag == TRUE)
  {
    status = LM_BUFFER_NOT_ALLOCATED;  // buffer is not allocated; so, can not be updated.
    return status;
  }

  if ( (offset + size) <= buf_size)
  {
    //don't copy the header : dest_addr = buf_mem_addr + sizeof(lm_buffer_header_type) + offset;
    dest_addr = buf_mem_addr + offset;
    memscpy(dest_addr, size, (void *)addr, size);
  }
  else
  {
    status = LM_INCORRECT_BUF_SIZE;
  }

  return status;
}



/*----------------------------------------------------------------------------*/
/*!
   @brief
      Return buffer contents for a portion of lm buffer.
 
   @details
      Return buffer contents for a portion of lm buffer.

   @param  
           '*addr'          : Address into which buffer contents shoulb be 
                              copied
           'size'           : Size of the buffer section to be copied
           'offset'         : Offset into the buffer, to indicate start memory
                              of the section in interest

   @retval
   LM_BUFFER_NOT_ALLOCATED : if the LM buffer is not already allocated.
   LM_INCORRECT_BUF_SIZE   : if the requested size of the data execeeds the
                             allocated buffer size.
 
   LM_SUCCESS              : if the buffer is updated with the requested data.
*/
lm_status_type lm_buffer::read_buffer(uint8 *addr, uint32 size, uint32 offset)
{
  uint8 *buf_src_addr = 0;

  lm_status_type status = LM_SUCCESS;

  // make sure that the buffer is reserved before reading this buffer contents.
  if (buf_header.free_flag == TRUE)
  {
    status = LM_BUFFER_NOT_ALLOCATED;  // buffer is not allocated; so, cannot be read.
    return status;
  }

  if ( (offset + size) <= buf_size)
  {
    buf_src_addr = buf_mem_addr + offset;
    memscpy((void *)addr, size, buf_src_addr, size);
  }
  else
  {
    status = LM_INCORRECT_BUF_SIZE;
  }

  return status;
}



/*----------------------------------------------------------------------------*/
/*!
  @brief
  Return Buffer address
 
  @retval
  LM_BUFFER_NOT_ALLOCATED : if the LM buffer is not already allocated.
  LM_INCORRECT_BUF_SIZE   : if the requested size of the data execeeds the
                             allocated buffer size.
 
  LM_SUCCESS              : if the buffer is updated with the requested data.
*/
lm_status_type lm_buffer::get_buffer_addr( uint32 *dest_addr, uint32 offset )
{
  lm_status_type status = LM_SUCCESS;

  // make sure that the buffer is reserved before updating this buffer contents.
  if (buf_header.free_flag == TRUE)
  {
    status = LM_BUFFER_NOT_ALLOCATED;  // buffer is not allocated; so, can not be updated.
    return status;
  }

  //*dest_addr = (uint32)(buf_mem_addr + sizeof(lm_buffer_header_type) + offset);
  *dest_addr = (uint32)(buf_mem_addr + offset);

  return status;
}



