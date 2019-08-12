/*!
  @file
  genlog_marshal.h

  @brief
  This header includes all resources necessary for marshaling data for genlog.

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/api/genlog_marshal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/07/10   ck      CMI Phase 4 changes - moving to mcs/api/... directory
05/06/10   ck      Removing FEATURIZATIONs as part of CMI Phase 4 clean-up effort
12/04/07   tjc     Initial version
===========================================================================*/

#ifndef GENLOG_MARSHAL_H
#define GENLOG_MARSHAL_H

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include <comdef.h>
#include <amssassert.h>


/*===========================================================================

                      EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! Dummy macro to check byte alignment 
   @note: This macro was created to help with genlog marshal debugging.
          Please read commented code in the header file to learn more.*/
#define GENLOG_MARSHAL_CHECK_BYTE_ALIGNMENT( ptr, type )
/* ------------------------------------------------------------------------*/
/*! Macro to check byte alignment (to be used only during debugging)
    @note: This macro generates a ton of lint warnings about defining a static
             variable in an inline function.  These warnings do not prevent the
             compile from succeeding, but means that for the default should be
             disabled, and should only used for debugging purposes.
           To debug, comment out the earlier blank definition of this macro
             and uncomment this macro */
/*
#define GENLOG_MARSHAL_CHECK_BYTE_ALIGNMENT( ptr, type )  \
    ASSERT( ( (uint32)( ptr ) &                           \
          ( MIN( sizeof( void *), sizeof( type ) ) - 1 ) ) == 0 )
*/
/* ------------------------------------------------------------------------*/

/*! Macro to declare marshaling */
#define GENLOG_MARSHAL_DECL()  \
  void *__data


/*! Macro to initialize marshaling */
#define GENLOG_MARSHAL_INIT( ptr ) \
  __data = (void *)ptr


/*! Macro to marshal data */
#define GENLOG_MARSHAL( type, data )                   \
  GENLOG_MARSHAL_CHECK_BYTE_ALIGNMENT( __data, type ); \
  *( (type *)__data ) = data;                          \
  __data = (uint8 *)__data + sizeof( type )


/*! Macro to demarshal data */
#define GENLOG_DEMARSHAL( type ) \
  *( (type *)__data );             \
  __data = (uint8 *)__data + sizeof( type )


/*! Macro to return the size of data marshaled */
#define GENLOG_MARSHAL_SIZE( ptr ) \
  ( (uint8 *)__data - (uint8 *)ptr )


/*! Macro to provide a compile time switch for MARSHAL/DEMARSHAL for use in
    a marshaling script 
    @note: The "MARSHAL" based definition of this macro is for the build
             functions, which is enabled by default for compiling genlog.
           The "DEMARSHAL based definition of this macro is for use in QXDM 
             parsers */
#define GENLOG_MARSHAL_DATA( type, data ) \
  GENLOG_MARSHAL( type, data )
/* ------------------------------------------------------------------------*/
/*! For the QXDM Parsers, define this macro as DEMARSHAL
    @note: to develop & use parsers uncomment this code/define and comment out
             the earlier one*/
/*
#define GENLOG_MARSHAL_DATA( type, data ) \
  data = GENLOG_DEMARSHAL( type )
*/
/* ------------------------------------------------------------------------*/


/*===========================================================================

                       EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

#endif /* GENLOG_MARSHAL_H */
