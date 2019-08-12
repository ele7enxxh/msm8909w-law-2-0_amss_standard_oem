#ifndef QURT_LIFO_H
#define QURT_LIFO_H
/*======================================================================
                        qurt_lifo.h

GENERAL DESCRIPTION
  provide lock free LastInFirstOut algorithm, which can be used in a
  variaty of situation for allocation/free fixed size buffer    
  This implementation will touch the first word of your FREED buffer. Even
	though it doesn't matter how you use it when it is allocated, you might want
	to be a bit careful not to put your MAGIC number as the first field.
	Because it will not hold the magic value for "freed"

 EXTERNALIZED FUNCTIONS
  none

 INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

 Copyright (c) 2009-2011
             by Qualcomm Technologies, Inc. All Rights Reserved.
 ======================================================================*/
/*======================================================================
 
 											 EDIT HISTORY FOR FILE
 
 	 This section contains comments describing changes made to the
 	 module. Notice that changes are listed in reverse chronological
 	 order.
 
 	$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_lifo.h#1 $ 
 	$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $
 
 
  when 				who 		what, where, why
  ---------- 	--- 		------------------------------------------------
  2011-02-25 	op			Add Header file
 ======================================================================*/
	/*=====================================================================
	 Functions
	======================================================================*/

/*======================================================================*/
/**
	FUNCTION				qurt_lifo_pop

	DESCRIPTION 		Pop an element out the LIFO 
	
		@param freelist [IN] pointer to the head of your list 
	
	DEPENDENCIES 	  None

	@return 				the top object from the list 
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
void * qurt_lifo_pop(void *freelist);

 
/*======================================================================*/
/**
	FUNCTION				qurt_lifo_push

	DESCRIPTION 		Push an element into the LIFO
	
		@param freelist [IN] pointer to the head of your list 
	  @param buf [IN] pointer to your buffer to push into the list 
	
	DEPENDENCIES 	  None

	@return 				None 
		 
	SIDE EFFECTS		None
*/
/* ======================================================================*/
void qurt_lifo_push(void *freelist, void *buf);

#endif /* QURT_LIFO_H */

