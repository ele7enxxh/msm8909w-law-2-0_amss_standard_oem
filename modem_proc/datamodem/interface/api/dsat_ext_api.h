#ifndef DSAT_EXT_API_H
#define DSAT_EXT_API_H
/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   P R O C E S S O R

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes which are exposed to 
  be used by any other modem SUs.
  
Copyright (c) 2009,2013 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/api/dsat_ext_api.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/28/09   vs      Removed the inclusion of customer.h
09/07/09   ps      New file created for CMI SU level changes.

===========================================================================*/

/*===========================================================================
                 AT Command Processor Data Structures
===========================================================================*/

/*---------------------------------------------------------------------------
     The following enumerated type represents the states of ATCOP: command, 
     online data, and online command. The ME is in command state when the 
     transport layer is not in the ESTABLISHED state. When in command state
     or online command state, all AT commands received on Rm are processed.
     When the ME is in the online data state, the ME passes all data received
     on the Rm directly to the IWF.  
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_CMD, 
  DSAT_ONLINE_DATA,
  DSAT_ONLINE_CMD
} dsat_mode_enum_type;


/*===========================================================================
                         EXTERNAL FUNCTION DECLRATIONS
===========================================================================*/

#endif /* DSAT_EXT_API_H */
