#ifndef _RESOURCE_CLEANUP_HANDLER_H_
#define _RESOURCE_CLEANUP_HANDLER_H_

/*===========================================================================
   Copyright (c) 2011-2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
   Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14    tp     Modified by running "atyle --style=allman" command.
07/05/13    ck     Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "com_dtypes.h"


#define IN
#define OUT
#define INOUT


#define TZAPP_RSCLR_MAX_TABLES							20
#define TZAPP_RSCLR_DEFAULT_SLOTS						50
#define TZAPP_RSCLR_MAX_SLOTS							500
#define TZAPP_RSCLR_MAX_PARAMETERS						5


typedef enum
{
    TZAPP_RSCLR_INDEX_TYPE_NULL = 0,
    TZAPP_RSCLR_INDEX_TYPE_QSEECOM,
    TZAPP_RSCLR_INDEX_TYPE_PROCESSID,
} TZAPP_RSCLR_INDEX_TYPE;

typedef enum
{
    TZAPP_RSCLR_RTYPE_NULL = 0,
    TZAPP_RSCLR_RTYPE_FUNCTION,		// Need to provide function pointer to free.
    TZAPP_RSCLR_RTYPE_TZAPP_HEAP = 100,
    TZAPP_RSCLR_RTYPE_SFS_HANDLE = 200,
} TZAPP_RSCLR_R_TYPE;


typedef int32	(*rsclr_func_0)				(void);
typedef int32	(*rsclr_func_1)				(uint32);
typedef int32	(*rsclr_func_2)				(uint32, uint32);
typedef int32	(*rsclr_func_3)				(uint32, uint32, uint32);
typedef int32	(*rsclr_func_4)				(uint32, uint32, uint32, uint32);
typedef int32	(*rsclr_func_5)				(uint32, uint32, uint32, uint32, uint32);


#define TZAPP_RSCLR_R_OK								0
#define TZAPP_RSCLR_ERR_INVALID_PARAM					-1		// Start of General Errors.
#define TZAPP_RSCLR_ERR_NOT_ENOUGH_MEMORY				-2

#define TZAPP_RSCLR_ERR_MAX_TABLE_EXEEDED				-100	// Start of TABLE Errors.
#define TZAPP_RSCLR_ERR_TABLE_NOT_FOUND					-101
#define TZAPP_RSCLR_ERR_TABLE_ALREADY_EXIST				-102
#define TZAPP_RSCLR_ERR_EMPTY_SLOT_NOT_FOUND			-103
#define TZAPP_RSCLR_ERR_RECORD_NOT_FOUND				-104
#define TZAPP_RSCLR_ERR_RECORD_EXIST					-105


#ifdef __cplusplus
extern "C" {
#endif


    int32 rsclr_create_resource_table(
        IN	int32		index_type,
        IN	uint32		index,
        IN	uint32		app_context,
        IN	int32		max_resource_slots
    );

    int32 rsclr_delete_resource_table(
        IN	int32		index_type,
        IN	uint32		index,
        IN	uint32		app_context
    );

    int32 rsclr_cleanup_and_delete_resource_table(
        IN	int32		index_type,
        IN	uint32		index
    );

    int32 rsclr_add_resource_record(
        IN	int32		index_type,
        IN	uint32		index,
        IN	uint32		app_context,
        IN	int32		resource_type,
        IN	void		*reference,
        IN	void		*cleanup_function,
        IN	int32		number_of_parameters,
        IN	uint32		*parameters
    );

    int32 rsclr_delete_resource_record(
        IN	int32		index_type,
        IN	uint32		index,
        IN	uint32		app_context,
        IN	int32		resource_type,
        IN	void		*reference,
        IN	void		*cleanup_function
    );


#ifdef __cplusplus
}
#endif

#endif /* _RESOURCE_CLEANUP_HANDLER_H_ */
