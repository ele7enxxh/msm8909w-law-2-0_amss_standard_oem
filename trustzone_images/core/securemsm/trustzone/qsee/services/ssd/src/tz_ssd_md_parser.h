#ifndef SSD_MD_PARSER_H
#define SSD_MD_PARSER_H

/**
   @file ssd_md_parser.h
   @brief implement parser for SSD metadata
*/

/*===========================================================================
   Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/services/ssd/src/tz_ssd_md_parser.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
12/27/12   pre      Initial version.

===========================================================================*/


/*===========================================================================

                              DEFINES

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "tz_ssd.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
  PARSE_SUCCESS    = 0,
  PARSE_INCOMPLETE = 1,
  PARSE_FAILURE    = 2
} parse_ret_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  Returns the address of the start of the metadata

  @param [in]     start  - pointer to beginning of memory to scan
  @param [in]     len    - length of memory area to scan for metadata
  @param [out]    ret_ptr_ptr - Pointer to the start of the MD
*/
int ssd_tz_find_md_start(uint32* start, uint32 len, uint32** md_start_ptr_ptr);

/**
   Parses the input buffer and fills in relevant structure elements
   for the given ssd metadata element.  Also allocates memory and
   copies the signed metadata region into the allocated buffer for
   authentication.

   @param[in]  md_buf           Input buffer to parse
   @param[in]  md_buf_len       Length of buffer to parse
   @param[out] pmd              metadata structure to fill
   @param[out] psigned_xml      copy of the signed SSD area
   @param[out] psigned_xml_len  length of the signed SSD area

   @return PARSE_SUCCESS    Parsing completes successfully
           PARSE_INCOMPLETE Beginning tag found, but no end tag found
           PARSE_FAILURE    Problem with parsing

   @sa Allocates memory on successful return.  May allocates memory
       on unsuccessful return.  Never allocates memory on failure.
 */
parse_ret_t ssd_parse_md
(
   uint8*             md_buf,
   uint32             md_buf_len,
   ssd_metadata_type* pmd,
   uint8**            psigned_xml,
   uint32*            psigned_xml_len
);

#endif
