/*===========================================================================

          M U L T I - F L O W   R A D I O   L I N K   P R O T O C O L 
                                     A R D A 

DESCRIPTION
  This module is responsible for starting/stopping Adaptive Receive Diversity
  Algorithm (ARDA). 
  
Copyright (c) 2008 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds_ehrpd_pdn_mgr.c#1 $
$Author: mplcsds1 $
$DateTime: 2016/12/13 08:00:03 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/07/08   spn     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"

#ifdef FEATURE_EHRPD

#ifndef FEATURE_HDR
#error "FEATURE_HDR_QOS defined without FEATURE_HDR being defined"
#endif /* FEATURE_HDR */


#include "ds_ehrpd_pdn_mgr.h"
#include "ds707_pdn_context.h"

/*===========================================================================
                          VARIABLES AND MACRO DEFINITIONS
===========================================================================*/

#define DS_EHRPD_PDN_MGR_MAX_PDN_ID 0x0E

/*---------------------------------------------------------------------------
  Structure that hold the mapping of a PDN ID to the PDN info itself. 
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 pdn_id;
  void* pdn_info_ptr;
}ds_ehrpd_pdn_mgr_map_type;

static ds_ehrpd_pdn_mgr_map_type ds_ehrpd_pdn_mgr_map[DS707_MAX_PDN_INSTANCES];


/*===========================================================================
                       INTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION DS_EHRPD_PDN_MGRI_IS_PDN_INFO_PRESENT

DESCRIPTION
  This function find if the pdn info is present in the map table or not.
  
DEPENDENCIES
  Since this is an internal function the assumption is that the parameter is 
  always non-NULL.

RETURN VALUE
  TRUE: If the pdn info is found
  FALSE: If the pdn info is found

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ehrpd_pdn_mgr_is_pdn_info_present
(
  void*   pdn_info_ptr
)
{
  uint8   pdn_map_loop = 0;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  for ( pdn_map_loop = 0;
        pdn_map_loop < DS707_MAX_PDN_INSTANCES;
        pdn_map_loop ++ )
  {
    if ( ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr == pdn_info_ptr )
    {
      ret_val = TRUE;
      break;
    }
  }

  return ret_val;
} /* ds_ehrpd_pdn_mgr_is_pdn_info_present() */


/*===========================================================================
                       EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================

FUNCTION DS_EHRPD_PDN_MGR_POWERUP_INIT

DESCRIPTION
  This function does the required initializations required for this module. 
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void ds_ehrpd_pdn_mgr_powerup_init
(
  void
)
{
  uint8   pdn_map_loop = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  for ( pdn_map_loop = 0;
        pdn_map_loop < DS707_MAX_PDN_INSTANCES;
        pdn_map_loop ++ )
  {
    ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_id = 0 ;
    ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr = NULL;
  }
} /* ds_ehrpd_pdn_mgr_powerup_init() */

/*===========================================================================

FUNCTION DS_EHRPD_PDN_MGR_ALLOCATE_PDN_ID

DESCRIPTION
  This function is called to allocate a PDN ID for a given PDN
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If a new PDN ID is allocated
          If TRUE pdn_id_ptr will be populated
  FALSE: If a new PDN ID cannot be allocated

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ehrpd_pdn_mgr_allocate_pdn_id
(
  void           *pdn_info_ptr,
  uint8          *pdn_id_ptr
)
{
  uint8   pdn_map_loop = 0;
  boolean ret_val = FALSE;
  boolean pdn_val = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  if ( ( pdn_info_ptr != NULL ) &&
       ( pdn_id_ptr != NULL ) )
  {
    /* If a pdn id is not allocated for this info_ptr */
    if ( ds_ehrpd_pdn_mgr_is_pdn_info_present(pdn_info_ptr) == FALSE )
    {
      /* find an unused entry in the map table */
      for ( pdn_map_loop = 0;  
            pdn_map_loop < DS707_MAX_PDN_INSTANCES;
            pdn_map_loop++ )
      {
        if ( ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr == NULL )
        {
          break;
        }
      }

      if ( pdn_map_loop != DS707_MAX_PDN_INSTANCES )
      {
        /* find an unused pdn id */
        for ( pdn_val = 0;
              pdn_val < DS_EHRPD_PDN_MGR_MAX_PDN_ID;
              pdn_val ++ )
        {
          if ( ds_ehrpd_pdn_mgr_is_pdn_id_valid(pdn_val) == FALSE ) 
          {
            /* Fill the entry in the map table */
            ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr = pdn_info_ptr;
            ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_id = pdn_val;

            DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                             "allocate_pdn_id(): PDN ID %d allocated",
                             ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_id );

            /* Fill in the return values */
            *pdn_id_ptr = pdn_val;
            ret_val = TRUE;
            
            break;
          }
        }
      } /* If there is an entry row in the table  */
    } /* If PDN info is not allocated a pdn id */
  } /* If Valid Parameters */
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "allocate_pdn_id(): Invalid Parameters");
  }

  return ret_val;

}/* ds_ehrpd_pdn_mgr_allocate_pdn_id() */

/*===========================================================================

FUNCTION DS_EHRPD_PDN_MGR_FREE_PDN_ID

DESCRIPTION
  This function is called to release a PDN ID so it can be reused.
  
DEPENDENCIES
  None.

RETURN VALUE
  TRUE: If the PDN ID is successfully free'ed
  FALSE: If the PDN ID is not free'ed

SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_ehrpd_pdn_mgr_free_pdn_id
(
  void*    pdn_info_ptr
)
{
  uint8   pdn_map_loop = 0;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if  ( pdn_info_ptr !=NULL ) 
  {
      /* find the matching entry in the map table */
      for ( pdn_map_loop = 0;  
            pdn_map_loop < DS707_MAX_PDN_INSTANCES;
            pdn_map_loop++ )
      {
        if ( ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr == 
                                                              pdn_info_ptr )
        {
          if ( ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr != NULL )
          {
            ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr = NULL;
            DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                             "free_pdn_id(): PDN ID %d freed",
                             ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_id );
            ret_val = TRUE;
          }
          else
          {
            DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                             "free_pdn_id(): PDN INFO is already freed");
          }

          break;
        } /* If a Match is found */
      }
  } /* If Valid Parameters */
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, "free_pdn_id(): Invalid Parameters");
  }

  return ret_val;
} /* ds_ehrpd_pdn_mgr_free_pdn_id() */


/*=========================================================================== 
 
FUNCTION       DS_EHRPD_PDN_MGR_GET_PDN_ID
 
DESCRIPTION    
  This function is called to get a PDN ID for a given IFACE
   
DEPENDENCIES  
  None.
 
RETURN VALUE  
  TRUE: If a valid PDN ID is returned
          If TRUE pdn_id_ptr will be populated
  FALSE: If the PDN ID cannot be found
    
 
SIDE EFFECTS  
  This function does not de-register the callbacks.
===========================================================================*/ 
boolean ds_ehrpd_pdn_mgr_get_pdn_id
(
  void             *pdn_info_ptr,
  uint8            *pdn_id_ptr
)
{
  uint8   pdn_map_loop = 0;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  

  if ( ( pdn_info_ptr != NULL ) &&
       ( pdn_id_ptr != NULL ) )
  {     
    for ( pdn_map_loop = 0;
          pdn_map_loop < DS707_MAX_PDN_INSTANCES;
          pdn_map_loop ++ )
    {
      if ( ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr == pdn_info_ptr )
      {
        *pdn_id_ptr = ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_id;
        ret_val = TRUE;
        DATA_IS707_MSG1( MSG_LEGACY_HIGH, 
                         "ds_ehrpd_pdn_mgr_get_pdn_id(): PDN id: %d",
                         *pdn_id_ptr );
        break;
      }
    }
  }  
  else
  {
    DATA_IS707_MSG0( MSG_LEGACY_ERROR, 
                     "get_pdn_id(): Invalid Parameters");
  }

  return ret_val;
} /* ds_ehrpd_pdn_mgr_get_pdn_id() */


/*=========================================================================== 
 
FUNCTION       DS_EHRPD_PDN_MGR_IS_PDN_ID_VALID
 
DESCRIPTION    
  This function is called to verify if the PDN ID is valid
   
DEPENDENCIES  
  None.
 
RETURN VALUE  
  TRUE: If the PDN id is valid
  FALSE: If the PDN ID is invalid
    
 
SIDE EFFECTS  
  This function does not de-register the callbacks.
===========================================================================*/ 
boolean ds_ehrpd_pdn_mgr_is_pdn_id_valid
(
  uint8        pdn_id
)
{
  uint8   pdn_map_loop = 0;
  boolean ret_val = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if  ( pdn_id < DS_EHRPD_PDN_MGR_MAX_PDN_ID ) 
  {
      /* find the matching entry in the map table */
      for ( pdn_map_loop = 0;  
            pdn_map_loop < DS707_MAX_PDN_INSTANCES;
            pdn_map_loop++ )
      {
        if ( ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_id == pdn_id )
        {
          if ( ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr != NULL )
          {
            ret_val = TRUE;
          }
        } /* If a Match is found */
      }
  } /* If Valid Parameters */
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "is_pdn_id_valid(): Invalid Parameters");
  }

  return ret_val;
} /* ds_ehrpd_pdn_mgr_is_pdn_id_valid() */

/*=========================================================================== 
 
FUNCTION       DS_EHRPD_PDN_MGR_GET_NUM_ALLOCATED_PDN
 
DESCRIPTION    
  Utility function to return the number of PDNs that are allocated. 
   
DEPENDENCIES  
  None.
 
RETURN VALUE  
  Returns the number of PDNs that are allocated. 
    
 
SIDE EFFECTS  
  None.
===========================================================================*/ 
uint8 ds_ehrpd_pdn_mgr_get_num_allocated_pdn
(
  void
)
{
  uint8 ret_val = 0;
  uint8   pdn_map_loop = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* find the matching entry in the map table */
  for ( pdn_map_loop = 0;  
        pdn_map_loop < DS707_MAX_PDN_INSTANCES;
        pdn_map_loop++ )
  {
    if ( ds_ehrpd_pdn_mgr_map[pdn_map_loop].pdn_info_ptr != NULL )
    {
      ret_val ++;
    }
  }

  return ret_val;
} /* ds_ehrpd_pdn_mgr_get_num_allocated_pdn() */


#endif /*FEATURE_EHRPD */
