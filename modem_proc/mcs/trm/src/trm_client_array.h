#ifndef TRM_CLIENT_ARRAY_H
#define TRM_CLIENT_ARRAY_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==

           T R A N S C E I V E R   R E S O U R C E   M A N A G E R

              Transceiver Resource Manager Client Array Header File

GENERAL DESCRIPTION

  This file provides some declarations of TRMClient Array handling.


EXTERNALIZED FUNCTIONS

  None


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

  None


  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*==



===============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/trm/src/trm_client_array.h#1 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ----------------------------------------------------------
12/25/2014   mn      TRM should use relative position in the order list for priority
                      comparisons. (CR: 774749).
10/20/2014   mn      Earliest client overshadows an incompatible higher priority 
                      client (CR: 741714).
04/11/2014   mn      TRM re-factoring. 

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES FOR MODULE

=============================================================================*/

#include "customer.h"
#include "modem_mcs_defs.h"

extern "C"
{
  #include "trm.h"
}

/*=============================================================================

                   DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, typesdefs,
and other items needed by this module.

=============================================================================*/

/*----------------------------------------------------------------------------
  Client Array
----------------------------------------------------------------------------*/

class TRMClientArray
{
protected:
  /* Array of clients */
  trm_client_enum_t         client[ TRM_MAX_CLIENTS ];

  /* Number of clients in the array */
  uint32                    num;

public:
  TRMClientArray() : num( 0 ) 
  {
    int i;

    for (i=0; i<(int)TRM_MAX_CLIENTS; i++ )
    {
      client[i] = TRM_NO_CLIENT; 
    } 
  } 

  /* Number of clients in the array */                     
  uint32                    length()          const
  {
    return num;
  }

  /* Clear the contents of the array */
  virtual void                      clear()
  {
    num = 0;
  }

  /* Finds a client in the array. Returns -1 if not found */
  int32                     find(trm_client_enum_t client_to_find);

  /* Retrieve client at the indicated position */
  trm_client_enum_t         operator[](int i) const;
};



/*----------------------------------------------------------------------------
  TRM Client Order Array
----------------------------------------------------------------------------*/

class TRMClientOrderArray : public TRMClientArray
{
private:
  uint8                     indices[TRM_MAX_CLIENTS];

public:
  virtual void              clear();

  /* Insert the client at the appropriate position */
  void                      insert( trm_client_enum_t client_id );

  /* Remove the client, if present in the array */
  void                      remove( trm_client_enum_t client_id );

  /* Update the client's position in the array by removing and re-inserting */
  void                      update( trm_client_enum_t client_id );

  /* Returns true if client1 is higher in priority than client2 */
  boolean                   compare(trm_client_enum_t client1, trm_client_enum_t client2);
};

/*----------------------------------------------------------------------------
  TRM Client Order Array
----------------------------------------------------------------------------*/

class TRMConflictArray : public TRMClientArray
{
public:

  /* Insert the client at the appropriate position */
  void                      append( trm_client_enum_t client_id );

  /* Remove a client from the conflict */
  void                      remove_client( trm_client_enum_t client_id );
};

/* TRM feature array is the list of client that has active features,
	this list has same attributes as conflict array */
typedef TRMConflictArray  TRMFeatureArray;

#endif /* TRM_CLIENT_ARRAY_H */