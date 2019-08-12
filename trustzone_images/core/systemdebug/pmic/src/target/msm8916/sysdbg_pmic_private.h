/*=============================================================================
						P M I C   D R I V E R   P R I V A T E   H E A D E R 

GENERAL DESCRIPTION     
  PMIC private header file
  
  

	Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
=============================================================================*/

/*=============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/systemdebug/pmic/src/target/msm8916/sysdbg_pmic_private.h#1 $

when       who          what, where, why
--------   ---          -------------------------------------------------- 
01/29/14    ntonupun    Ported to Bear
03/10/12	AJCheriyan	First cut for Aragorn. 

===========================================================================*/

#ifndef _SYSDBG_PMIC_PRIVATE_H
#define _SYSDBG_PMIC_PRIVATE_H

#include "sysdbg_pmic.h"
#include "sysdbg_spmi.h"

// move this to the scons script
#define ADDRSPACE_PER_PERPH		0x100
#define	ADDRSPACE_PER_SLAVE		0x8000 // reduced for PM8916
#define SLAVES_PER_PM			0x2
// This would be the total number of peripherals per slave
#define	PERIPHERALS_PER_SLAVE	ADDRSPACE_PER_SLAVE/ADDRSPACE_PER_PERPH

// This would be the total number of peripherals working with the "chipset"
// This occupies a lot of space statically and if its not going to be used trim it. 
// This number should be in sync with the entries in the BSP (or at least the valid 
// entries ).
#define MAX_NO_PERIPHERALS	PERIPHERALS_PER_SLAVE * SLAVES_PER_PM * PMIC_COUNT
// This is the number of peripherals that are currently defined by SW
#define VALID_NO_PERIPHERALS   VALID_PERIPH_COUNT
// This is the number of peripherals we need defined in the BSP
#define MIN_NO_PERIPHERALS	0x18 
// Thus the relationship is as follows
// MIN_NO_PERIPHERALS < VALID_NO_PERIPHERALS < MAX_NO_PERIPHERALS
// efficient < practical < ideal

// Change the size on a need by need basis to one of the 3 sizes above -
// Valid_no_peripherals or Max_no_peripherals or Min_no_peripherals
#define BSP_SIZE MIN_NO_PERIPHERALS	


typedef struct
{
	PMPerphName 			name:8;
	spmi_slave_type 		slave_id:8;
	const PeripheralRegValid_type *mask;
	uint16					base:16;
}PMRegEntryType;

// The size of this structure should be one of the three possible sizes defined
// above. Depending on the space, use any one
typedef struct
{
	PMRegEntryType pmap[BSP_SIZE] ;
}PMMapType;

// This api should be used to get info about the peripheral to read and write data
// void pm_getmapinfo(PMPerphName, PMRegEntryType *);


#endif /* _SYSDBG_PMIC_PRIVATE_H */

