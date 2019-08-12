/*!
  @file
  genlog.c

  @brief
  This module contains the common helper functions needed for logging log
  packets containing multiple sub packets.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/genlog/src/genlog.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/08/10   tjc     Simplified the interface for constant size subpackets
09/24/09   awj     Klocwork cleanup
11/30/07   tjc     Initial version
===========================================================================*/

/*===========================================================================

                             INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <stringl/stringl.h>
#include <log.h>
#include <msg.h>
#include <amssassert.h>

#include "genlog.h"


/*===========================================================================

                      INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*----------------------------------------------------------------------------
  Alignment Macros
----------------------------------------------------------------------------*/

/*! Mask to determine 32-bit alignment */
#define GENLOG_ALIGN_MASK_32BIT  0x03


/*! Macro to align size to 32-bit */
#define GENLOG_ALIGN_SIZE_TO_32BIT( size )                                        \
  ( size & GENLOG_ALIGN_MASK_32BIT ) ?                                            \
  (uint16)( size + ( sizeof( uint32 ) - ( size & GENLOG_ALIGN_MASK_32BIT ) ) ) :  \
  (uint16)( size )


/*----------------------------------------------------------------------------
  Genlog header definitions
----------------------------------------------------------------------------*/

/*! Genlog subpacket header structure. */
typedef struct
{
  /*! Subpacket id */
  uint8   id;
  /*! Subpacket version */
  uint8   ver;
  /*! Subpacket size */
  uint16  size;

} genlog_subpkt_hdr_s;


/*! Genlog packet header structure. */
typedef struct
{
  /*! Generalized packet version */
  uint8   ver;
  /*! Number of subpackets */
  uint8   num_subpkts;
  /*! Padding so subpackets are aligned */
  uint16  reserved;

} genlog_pkt_hdr_s;


/*----------------------------------------------------------------------------
  Subpacket data
----------------------------------------------------------------------------*/

/*! Subpacket type */
typedef struct genlog_subpkt_s
{
  /*! Subpacket header */
  genlog_subpkt_hdr_s hdr;
  /*! Subpacket scratchpad area */
  void  *data;
  /*! Size required for log packet */
  uint16  size;
  /* Subpacket payload */
  void  *payload;
  /*! Subpacket build function */
  genlog_subpkt_build_f  build_f;
  /*! Memcpy function for copying the data */
  genlog_subpkt_memcpy_f  memcpy_f;

} genlog_subpkt_s;


/*----------------------------------------------------------------------------
  Log packet data
----------------------------------------------------------------------------*/

/*! Log packet type */
typedef struct genlog_pkt_s
{
  /*! Log code */
  log_code_type  log_code;
  /*! Log packet header */
  genlog_pkt_hdr_s  hdr;
  /*! Subpackets */
  genlog_subpkt_s  subpkt[ GENLOG_MAX_NUM_SUBPKTS ];

} genlog_pkt_s;


/*===========================================================================

                             LOCAL VARIABLES

===========================================================================*/


/*===========================================================================

                                FUNCTIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  genlog_get_pkt_def

===========================================================================*/
/*!
    @brief
    This function returns the log packet definition for the specified log code.

    @return
    const genlog_pkt_def_s *
*/
/*=========================================================================*/
static const genlog_pkt_def_s * genlog_get_pkt_def
(
  genlog_s  *genlog,  /*!< Genlog structure */
  log_code_type  log_code  /*!< Log code id */
)
{
  const genlog_pkt_def_s  *pkt_def;
  uint8  i;

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( genlog == NULL )
  {
    return NULL;
  }

  /*-----------------------------------------------------------------------*/

  /* Intialize the log packet definition to NULL */
  pkt_def = NULL;

  /* Walk the list of log packet definitions */
  for ( i = 0; i < genlog->num_pkts; i++ )
  {
    if ( log_code == genlog->pkt_def[ i ].log_code )
    {
      pkt_def = &genlog->pkt_def[ i ];
      break;
    }
  }

  return pkt_def;

} /* genlog_get_pkt_def() */


/*===========================================================================

  FUNCTION:  genlog_build_pkt

===========================================================================*/
/*!
    @brief
    This function builds the log packet and returns the size needed required
    for the log packet.

    @return
    uint16
*/
/*=========================================================================*/
static uint16 genlog_build_pkt
(
  genlog_s  *genlog,  /*!< Genlog structure */
  genlog_pkt_s  *pkt,  /*!< Log packet structure */
  const genlog_pkt_def_s  *pkt_def  /*!< Log packet definition */
)
{
  uint8  i, num_subpkts;
  uint16  size;

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( ( genlog == NULL ) || ( pkt == NULL ) || ( pkt_def == NULL ) )
  {
    /* This is a failure, so just return a size of 0 */
    return 0;
  }

  /* Size check, befor we use this to index into the subpkt_id array  */
  if ( pkt_def->num_subpkts >= GENLOG_MAX_NUM_SUBPKTS )
  {
    /* This is a failure, so just return a size of 0 */
    return 0;
  }

  /*-----------------------------------------------------------------------*/

  /* Initialize the size to 0 */
  size = 0;

  /* Clear the number of subpackets */
  num_subpkts = 0;

  /* Walk the subpackets */
  for ( i = 0; i < pkt_def->num_subpkts; i++ )
  {
    uint32                subpkt_id  = pkt_def->subpkt_id[ i ];
    genlog_subpkt_def_s  *subpkt_def = genlog->subpkt_def[ subpkt_id ];
    genlog_subpkt_s      *subpkt     = &pkt->subpkt[ num_subpkts ];

    /* NULL check the subpacket definition */
    if ( subpkt_def == NULL )
    {
      /* Skip the subpacket */
      continue;
    }

    /* Intialize the subpacket data */
    subpkt->hdr.id    = subpkt_def->const_data.id;
    subpkt->hdr.ver   = subpkt_def->const_data.ver;
    subpkt->hdr.size  = 0;
    subpkt->build_f   = subpkt_def->const_data.build_f;
    subpkt->memcpy_f  = subpkt_def->const_data.memcpy_f;
    subpkt->data      = subpkt_def->data;
    subpkt->size      = subpkt_def->const_data.size;
    subpkt->payload   = subpkt_def->payload;

    /* Build the subpacket if the function is defined */
    if ( subpkt->build_f != NULL )
    {
      subpkt->size = subpkt->build_f( subpkt->data, subpkt->payload );
    }

    /* If the subpacket returned a size, set the size */
    if ( subpkt->size != 0 )
    {
      /* 32-bit align the size of the subpacket */
      uint16  subpkt_size = GENLOG_ALIGN_SIZE_TO_32BIT( subpkt->size );
      /* Record the total size of the subpacket */
      subpkt->hdr.size = sizeof( genlog_subpkt_hdr_s ) + subpkt_size;
      /* Add the size of the subpacket to the size of the log packet */
      size += subpkt->hdr.size;
      /* Increment the number of subpackets */
      num_subpkts++;
    }
  }

  /* Set the log code */
  pkt->log_code        = pkt_def->log_code;
  /* Initialize the log packet header */
  pkt->hdr.ver         = (uint8)pkt_def->ver;
  pkt->hdr.num_subpkts = num_subpkts;
  /* Set the size of the log packet to the size of the header */
  size += (uint16)sizeof( genlog_pkt_hdr_s );

  return size;

} /* genlog_build_pkt() */


/*===========================================================================

  FUNCTION:  genlog_commit_pkt

===========================================================================*/
/*!
    @brief
    This function commits the log packet using the log interface.

    @return
    errno_enum_type

    @retval E_INVALID_ARG - If the arguments passed are invalid
    @retval E_NO_MEMORY   - If a log packet cannot be allocated
    @retval E_SUCCESS     - Otherwise
*/
/*=========================================================================*/
static errno_enum_type genlog_commit_pkt
(
  genlog_s  *genlog,  /*!< Genlog structure */
  genlog_pkt_s  *pkt,  /*!< Log packet structure */
  uint16  size  /*!< Size required by the log packet */
)
{
  errno_enum_type  retval;
  void  *diag_pkt, *genlog_pkt;
  uint8  i;

  uint16 genlog_pkt_remaining_size;
  uint32 copied_size;

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( ( genlog == NULL ) || ( pkt == NULL ) )
  {
    return E_INVALID_ARG;
  }

  /* Size check, befor we use this to index into the subpkt array  */
  if( pkt->hdr.num_subpkts >= GENLOG_MAX_NUM_SUBPKTS )
  {
    return E_INVALID_ARG;
  }

  /*-----------------------------------------------------------------------*/

  do
  {
    /* We have not succeeded yet */
    retval = E_FAILURE;

    /*-----------------------------------------------------------------------
      Allocate a log buffer
    -----------------------------------------------------------------------*/
    diag_pkt = (void *)log_alloc( pkt->log_code,
                                  size + sizeof( log_hdr_type ) );
    if ( diag_pkt == NULL )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "Unable to allocate diag packet for log packet: %d", pkt->log_code);
      /* This is a failure */
      retval = E_NO_MEMORY;
      break;
    }

    /*-----------------------------------------------------------------------
      Fill in the buffer
    -----------------------------------------------------------------------*/
    /* Get a pointer to the start of the Genlog packet */
    genlog_pkt = (uint8 *)diag_pkt + sizeof( log_hdr_type );


    genlog_pkt_remaining_size = size;

    /* Copy the header information */
    copied_size = memscpy( genlog_pkt, genlog_pkt_remaining_size,
                           &pkt->hdr, sizeof( genlog_pkt_hdr_s ) );
    ASSERT( copied_size == sizeof( genlog_pkt_hdr_s ) );

    /* Advance the genlog pointer */
    genlog_pkt = (uint8 *)genlog_pkt + copied_size;

    genlog_pkt_remaining_size = size - copied_size; 

    /* Walk the subpackets */
    for ( i = 0; i < pkt->hdr.num_subpkts; i++ )
    {
      genlog_subpkt_s  *subpkt = &pkt->subpkt[ i ];

      if ( subpkt->hdr.size != 0 )
      {
        void  *sub_pkt;

        /* Copy the subpacket header information */
        copied_size = memscpy( genlog_pkt, 
                               genlog_pkt_remaining_size, 
                               &subpkt->hdr, 
                               sizeof( genlog_subpkt_hdr_s ) );
        ASSERT( copied_size == sizeof( genlog_subpkt_hdr_s ) );

        /* Get a pointer to the start of the subpacket */
        sub_pkt = (uint8 *)genlog_pkt + copied_size;

        /* Copy the subpacket  */
        (void)subpkt->memcpy_f( sub_pkt, 
                                genlog_pkt_remaining_size 
                                - copied_size, 
                                subpkt->data, 
                                subpkt->size );

        /* Advance the genlog pointer */
        genlog_pkt = (uint8 *)genlog_pkt + subpkt->hdr.size;
        /* Advance the pointer to account for entire sub packet (hdr+data)*/
        genlog_pkt_remaining_size = genlog_pkt_remaining_size - subpkt->hdr.size;
      }
    }

    /*-----------------------------------------------------------------------
      Commit the log buffer
    -----------------------------------------------------------------------*/
    log_commit( diag_pkt );

    /* Ok, we have success */
    retval = E_SUCCESS;

  } while ( 0 );

  return retval;

} /* genlog_commit_pkt() */


/*===========================================================================

  FUNCTION:  genlog_init

===========================================================================*/
/*!
    @brief
    This function initializes the genlog structure by verifying the contents
    and clearing out the subpacket array.

    @return
    errno_enum_type

    @retval E_INVALID_ARG - If the arguments passed are invalid
    @retval E_BAD_ADDRESS - If the log/subpacket arrays are invalid
    @retval E_SUCCESS     - Otherwise
*/
/*=========================================================================*/
errno_enum_type genlog_init
(
  genlog_s         *genlog  /*!< Genlog structure */
)
{

  errno_enum_type   ret_val = E_SUCCESS;

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( genlog == NULL )
  {
    ret_val = E_INVALID_ARG;
  }
  /* Verify the contents of the Genlog structure */
  else if ( ( genlog->pkt_def == NULL ) || ( genlog->subpkt_def == NULL ) )
  {
    ret_val = E_BAD_ADDRESS;
  }
  /* Clear out the subpacket array */
  else {
    memset( (void *)genlog->subpkt_def, 0, sizeof( *genlog->subpkt_def ) *
                                           genlog->num_subpkts );
  }

  return ret_val;

} /* genlog_init() */


/*===========================================================================

  FUNCTION:  genlog_subpkt_register

===========================================================================*/
/*!
    @brief
    This function registers the subpacket definition with the specified
    subpacket id.

    @return
    errno_enum_type

    @retval E_INVALID_ARG  - If the arguments passed are invalid
    @retval E_SUCCESS      - Otherwise
*/
/*=========================================================================*/
errno_enum_type genlog_subpkt_register
(
  genlog_s  *genlog,  /*!< Genlog structure */
  uint8  num_subpkts,  /*!< Number of subpackets */
  genlog_subpkt_def_s  *subpkt_def  /*!< Subpacket definition array */
)
{
  uint8  i;

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( ( genlog == NULL ) || ( subpkt_def == NULL ) )
  {
    return E_INVALID_ARG;
  }

  /*-----------------------------------------------------------------------*/

  for ( i = 0; i < num_subpkts; i++ )
  {
    /* Range check */
    if ( subpkt_def[ i ].const_data.id >= genlog->num_subpkts )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "Subpacket definition %d in registration is out of range",
            subpkt_def[ i ].const_data.id);
      /* Skip this sector */
      continue;
    }

    /*! @todo: How do we handle simultaneous requests? */

    /* Is the subpacket definition slot open? */
    //if ( genlog->subpkt_def[ subpkt_def[ i ].id ] == NULL )
    {
      /* Assign the subpacket defintion to the slot */
      genlog->subpkt_def[ subpkt_def[ i ].const_data.id ] = &subpkt_def[ i ];
    }
    //else
    //{
    //  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
    //        "Definition for subpacket %d already exists", subpkt_def[ i ]->id);
    //  /* Skip this sector */
    //  continue;
    //}
  }

  return E_SUCCESS;

} /* genlog_subpkt_register() */


/*===========================================================================

  FUNCTION:  genlog_subpkt_deregister

===========================================================================*/
/*!
    @brief
    This function deregisters the subpacket definition with the specified
    subpacket id.

    @return
    errno_enum_type

    @retval E_INVALID_ARG  - If the arguments passed are invalid
    @retval E_SUCCESS      - Otherwise
*/
/*=========================================================================*/
errno_enum_type genlog_subpkt_deregister
(
  genlog_s  *genlog,  /*!< Genlog structure */
  uint8  num_subpkts,  /*!< Number of subpackets */
  genlog_subpkt_def_s  *subpkt_def  /*!< Subpacket definition array */
)
{
  uint8  i;

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( ( genlog == NULL ) || ( subpkt_def == NULL ) )
  {
    return E_INVALID_ARG;
  }

  /*-----------------------------------------------------------------------*/

  for ( i = 0; i < num_subpkts; i++ )
  {
    /* Range check */
    if ( subpkt_def[ i ].const_data.id >= genlog->num_subpkts )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "Subpacket definition %d in deregistration is out of range",
            subpkt_def[ i ].const_data.id);
      /* Skip this sector */
      continue;
    }

    /* Is the subpacket definition slot open? */
    if ( genlog->subpkt_def[ subpkt_def[ i ].const_data.id ] ==
         &subpkt_def[ i ] )
    {
      /* There was a subpacket defintion match, blow away the definition */
      genlog->subpkt_def[ subpkt_def[ i ].const_data.id ] = NULL;
    }
    else if ( genlog->subpkt_def[ subpkt_def[ i ].const_data.id ] == NULL )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "No subpacket definition for subpacket %d",
            subpkt_def[ i ].const_data.id);
      /* Skip this sector */
      continue;
    }
    else
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_MED,
            "Definition for subpacket %d has been changed",
            subpkt_def[ i ].const_data.id);
      /* Skip this sector */
      continue;
    }
  }

  return E_SUCCESS;

} /* genlog_subpkt_deregister() */


/*===========================================================================

  FUNCTION:  genlog_subpkt_set_data

===========================================================================*/
/*!
    @brief
    This function sets the data pointer for the specified subpacket.

    @return
    errno_enum_type

    @retval E_INVALID_ARG  - If the arguments passed are invalid
    @retval E_OUT_OF_RANGE - If the subpacket id is not within range
    @retval E_BAD_ADDRESS  - If the subpacket definition is NULL
    @retval E_SUCCESS      - Otherwise
*/
/*=========================================================================*/
errno_enum_type genlog_subpkt_set_data
(
  genlog_s  *genlog,  /*!< Genlog structure */
  uint8  subpkt_id,  /*!< Subpacket for which the data is being set */
  void  *data  /*!< Subpacket data */
)
{

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( genlog == NULL )
  {
    return E_INVALID_ARG;
  }

  /* Range check */
  if ( subpkt_id >= genlog->num_subpkts )
  {
    return E_OUT_OF_RANGE;
  }

  /* Check if the subpacket definition is valid */
  if ( genlog->subpkt_def[ subpkt_id ] == NULL )
  {
    return E_BAD_ADDRESS;
  }

  /*-----------------------------------------------------------------------*/

  /* Set the subpacket data */
  genlog->subpkt_def[ subpkt_id ]->data = data;

  return E_SUCCESS;

} /* genlog_subpkt_set_data() */


/*===========================================================================

  FUNCTION:  genlog_subpkt_set_payload

===========================================================================*/
/*!
    @brief
    This function sets the payload pointer for the specified subpacket.

    @return
    errno_enum_type

    @retval E_INVALID_ARG  - If the arguments passed are invalid
    @retval E_OUT_OF_RANGE - If the subpacket id is not within range
    @retval E_BAD_ADDRESS  - If the subpacket definition is NULL
    @retval E_SUCCESS      - Otherwise
*/
/*=========================================================================*/
errno_enum_type genlog_subpkt_set_payload
(
  genlog_s  *genlog,  /*!< Genlog structure */
  uint8  subpkt_id,  /*!< Subpacket for which the data is being set */
  void  *payload  /*!< Subpacket payload */
)
{

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( genlog == NULL )
  {
    return E_INVALID_ARG;
  }

  /* Range check */
  if ( subpkt_id >= genlog->num_subpkts )
  {
    return E_OUT_OF_RANGE;
  }

  /* Check if the subpacket definition is valid */
  if ( genlog->subpkt_def[ subpkt_id ] == NULL )
  {
    return E_BAD_ADDRESS;
  }

  /*-----------------------------------------------------------------------*/

  /* Set the subpacket data */
  genlog->subpkt_def[ subpkt_id ]->payload = payload;

  return E_SUCCESS;

} /* genlog_subpkt_set_payload() */


/*===========================================================================

  FUNCTION:  genlog_pkt_commit

===========================================================================*/
/*!
    @brief
    This function compiles the log packet specified by the log packet defintion
    and sends it to DIAG.

    @return
    errno_enum_type

    @retval E_INVALID_ARG - If the arguments passed are invalid
    @retval E_FAILURE     - If the log packet definition cannot be found
    @retval E_NO_MEMORY   - If a log packet cannot be allocated
    @retval E_SUCCESS     - Otherwise
*/
/*=========================================================================*/
errno_enum_type genlog_pkt_commit
(
  genlog_s  *genlog,  /*!< Genlog structure */
  log_code_type  log_code  /*!< Log code id */
)
{
  errno_enum_type  retval;
  const genlog_pkt_def_s  *pkt_def;
  genlog_pkt_s  pkt;
  uint16  size;

  /*-----------------------------------------------------------------------*/

  /* NULL check */
  if ( genlog == NULL )
  {
    return E_INVALID_ARG;
  }

  /*-----------------------------------------------------------------------*/

  do
  {
    /* We have not succeeded yet */
    retval = E_FAILURE;

    /* Get the log packet definition for the specified log code */
    pkt_def = genlog_get_pkt_def( genlog, log_code );
    if ( pkt_def == NULL )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "No log packet defintion for log code %d", log_code);
      /* This is a failure */
      retval = E_FAILURE;
      break;
    }

    /* Make sure that the packet is enabled */
    if ( !log_status( pkt_def->log_code ) )
    {
      /* The log packet is not available (this could be converted into a
         failure, but the common use case is to just ignore if the log packet
         is not enabled) so just return success */

      retval = E_SUCCESS;
      break;
    }

    /* Build the subpackets */
    size = genlog_build_pkt( genlog, &pkt, pkt_def );
    if ( size == 0 )
    {
      /* The size of the log packet is 0, there is nothing to send so lets get
         out of here */
      retval = E_SUCCESS;
      break;
    }

    /* Commit the log packet */
    retval = genlog_commit_pkt( genlog, &pkt, size );
    if ( retval != E_SUCCESS )
    {
      MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Failed to commit log packet: %d", log_code);
      break;
    }

  } while ( 0 );

  return retval;

} /* genlog_pkt_commit() */
