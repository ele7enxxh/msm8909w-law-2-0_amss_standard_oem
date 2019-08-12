#ifndef _POLICYMAN_TOOLS_H_
#define _POLICYMAN_TOOLS_H_

/**
  @file policyman_tools.h

  @brief  Public interface to policyman's public tools functions
*/

/*
    Copyright (c) 2015 QUALCOMM Technologies Incorporated.
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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/policyman_tools.h#1 $
  $DateTime: 2016/12/13 08:00:05 $
  $Author: mplcsds1 $
*/


/*-------- policyman_efs_obliterate --------*/
/**
@brief  Remove the /policyman directory and all of its contents.

@return
        int:  0 if successful, -1 otherwise.
*/
int policyman_efs_obliterate(
  void
);


/*-------- policyman_efs_obliterate_subs --------*/
/**
@brief  Remove subs-specific files from the /policyman directory
        SYS_MODEM_AS_ID_NONE: removes device-specific policyman files
        SYS_MODEM_AS_ID_1: \
        SYS_MODEM_AS_ID_2:  removes subs-specific files

@return
        int:  The total number of files and/or directories removed
*/
int policyman_efs_obliterate_subs(
  sys_modem_as_id_e_type  subsId
);

#endif /* _POLICYMAN_TOOLS_H_ */

