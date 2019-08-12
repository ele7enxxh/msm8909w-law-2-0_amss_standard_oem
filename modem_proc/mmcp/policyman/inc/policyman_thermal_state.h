/**
  @file policyman_thermal_state.h

  @brief  Module to handle changes in thermal state of UE.
*/
	/*
		Copyright (c) 2013,2015 QUALCOMM Technologies Incorporated.
		All Rights Reserved.
		Qualcomm Technologies Confidential and Proprietary
	
	  Export of this technology or software is regulated by the U.S. Government.
	  Diversion contrary to U.S. law prohibited.
	
	  All ideas, data and information contained in or disclosed by this
	  document are confidential and proprietary information of
	  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
	  By accepting this material the recipient agrees that this material
	  and the information contained therein are held in confidence and in
	  trust and will not be used, copied, reproduced in whole or in part,
	  nor its contents revealed in any manner to others without the express
	  written permission of QUALCOMM Technologies Incorporated.
	
	  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/policyman/inc/policyman_thermal_state.h#1 $
	  $DateTime: 2016/12/13 08:00:05 $
	  $Author: mplcsds1 $
	*/



#ifndef _POLICYMAN_THERMAL_STATE_H_
#define _POLICYMAN_THERMAL_STATE_H_


#include "policyman_i.h"
#include "cfcm.h"

/*  Thermal Level indicates.
 */
typedef enum
{
  POLICYMAN_THERM_LEVEL_0    = 0,    /* thermal state is normal */
  POLICYMAN_THERM_LEVEL_1    = 1,    /* thermal state level 1 */
  POLICYMAN_THERM_LEVEL_2    = 2,    /* thermal state level 2. 4G disabled in this state */
  POLICYMAN_THERM_LEVEL_3    = 3,    /* thermal state level 3. Only emergency usage */
} policyman_thermal_level_t;

#endif /* _POLICYMAN_THERMAL_STATE_H_ */

