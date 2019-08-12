#ifndef __RESTRACKER_H__
#define __RESTRACKER_H__

/**
   @file tracker.h

   @brief resource tracker.
   Helper module to track allocated resources or memory

   This module provides a simple resource tracking mechanism for things that 
   are allocated, opened, or otherwise used, where the termination of the 
   object is required.  A new trackerlist can be made for any type of resource,
   and when needed, the restrack_dump() function can be called to show all
   currently used objects.  At the end of a program, this would show you what
   was left open/allocated/etc.

   Externalized Functions:
   restrack_init,
   restrack_add,
   restrack_delete,
   restrack_summary,
   restrack_dump,
   restrack_clear.

   Initialization and sequencing requirements:
   None.
*/

/*=========================================================================
                               H E A D E R
                    R E S T R A C K E R    M O D U L E

Copyright (c) 2007-2011 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                         EDIT HISTORY FOR MODULE

$Header: //components/rel/core.mpss/3.5.c12.3/api/securemsm/smecom/tracker.h#1 $
$DateTime: 2016/12/13 07:59:23 $ 2005/10/06 
$Author: mplcsds1 $ 

when         who     what, where, why
--------     ---     --------------------------------------------------------
01/18/11     leo     (Tech Pubs) Edited Doxygen comments and markup.
12/14/10     nk      Doxygenated.
05/17/07     avm     Fixed Compiler Warnings.
03/28/07     je      Initial Revision.

===========================================================================*/

#include "rex.h"
#include "UxCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SMECOMUTIL_MEMORY 
    @{ 
*/

/** @name Resource Tracker Definitions and Constants
@{
*/
/**
   Resource tracker maximum file length.
*/
#define RESTRACKER_FILE_MAX 80

/**
   Resource tracker maximum task length.
*/
#define RESTRACKER_TASK_MAX 32


/*===========================================================================
                        type declarations
===========================================================================*/
/**
   @brief Placeholder for a tracker record.
*/
typedef struct t_trackrec {
unsigned long __ptr;               /**< Tracker record pointer.             */
unsigned long __size;              /**< Tracker record size.                */
char __file[RESTRACKER_FILE_MAX];  /**< File information buffer.            */
char __task[RESTRACKER_TASK_MAX];  /**< Task information buffer.            */
int __line;                        /**< Line number.                        */
int __alloccount;                  /**< Allocated count.                    */
boolean __freed;                   /**< Freed flag.                         */
struct t_trackrec *__prev;         /**< Pointer to previous tracker record. */
struct t_trackrec *__next;         /**< Pointer to next tracker record.     */
} trackerrec;

/**
  @brief Structure for a doubly linked list of tracker records.
*/
typedef struct t_trackerlist {
trackerrec *__head;                /**< Pointer to the head tracker record. */
trackerrec *__tail;                /**< Pointer to the tail tracker record. */
int __count;                       /**< Tracker record count.               */
int __maxcount;                    /**< Tracker record maximum count.       */
int __total;                       /**< Tracker record total count.         */
int __maxtotal;                    /**< Tracker record maximum total count. */
int __report_num;                  /**< Report number.                      */
char __name[RESTRACKER_FILE_MAX];  /**< File name.                          */
rex_crit_sect_type crit_sect;      /**< Critical section type.              */
} trackerlist;

/** @} */ /* end_name resource tracker definitions and constants */

/*===========================================================================
   external function declarations
===========================================================================*/

/**
   Intializes the resource tracker.

   @param[in] list Pointer to the tracker record list.
   @param[in] name Pointer to the file name.

   @return
   None.

   @dependencies
   None. 
*/
void restrack_init( trackerlist *list, const char *name );

/**
   Adds the tracker record to the resource tracker.

   @param[in] list Pointer to the tracker record list.
   @param[in] ipr  Pointer to the tracker record.

   @return
   None.

   @dependencies
   None. 
*/
void restrack_add( trackerlist *list, trackerrec *ipr );

/**
   Deletes the tracker record from the resource tracker.

   @param[in] list Pointer to the tracker record list.
   @param[in] ptr  Pointer to the tracker record.

   @return
   None.

   @dependencies
   None.
*/
void restrack_delete( trackerlist *list, unsigned long ptr );

/**
   Provides a summary of the tracker record list.

   @param[in] list Pointer to the tracker record list.

   @return
   None.

   @dependencies
   None. 
*/
void restrack_summary( const trackerlist *list );

/**
   Dumps the tracker record list.

   @param[in] list Pointer to the tracker record list.

   @return
   None.

   @dependencies
   None.
*/
void restrack_dump( const trackerlist *list );

/**
   Dumps the tracker record list to a file.

   @param[in] list Pointer to the tracker record list.

   @return
    None.

   @dependencies
   None.
*/
void restrack_dumpleaktofile( const trackerlist *list );

/**
   Dumps the tracker record list history to a file.

   @return
    None.

   @dependencies
   None.
*/
void restrack_dumphistorytofile( void );

/**
   Clears the tracker record list.

   @param[in] list Pointer to the tracker record list.

   @return
   None.

   @dependencies
   None.
*/
void restrack_clear( trackerlist *list );

/*===========================================================================
   global variable declarations 
===========================================================================*/
extern trackerlist g_timerlist;   /**< Tracker timers.                   */
extern trackerlist g_heaplist;    /**< Tracker memory allocation for the
                                       heap list.                        */
extern trackerlist g_historylist; /**< Tracker memory allocation for the
                                       history list.                     */
extern boolean g_heaplistinit;    /**< Flag for heap list initilization. */
extern int g_trackerallocnum;     /**< Tracker memory allocation number. */
extern int g_trackerallocsize;    /**< Tracker memory allocation size.   */
extern int g_trackeraddlist;      /**< Tracker memory add list.
                                       @latexonly \newpage @endlatexonly      */

// rex_init_crit_sect, rex_enter_crit_sect and rex_leave_crit_sect.

/** @} */  /* end_addtogroup SMECOMUTIL_MEMORY */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __RESTRACKER_H__

