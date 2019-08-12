#ifndef PDSMBUF_H
#define PDSMBUF_H

#ifdef __cplusplus
extern "C" {
#endif

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Position Determination Session Manager Dynamic Buffer Module

GENERAL DESCRIPTION
  This module manages the Dynamic buffer pools of PDSM.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/inc/pdapibuf.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/17/16   rn      Decreased Buffpool size for IS801 Fwd PDDM reduction.
05/29/15   sj      Increased Buffpool size for IS801 Fwd PDDM
07/16/13   ah      Reverted back to original PDSM heap implementation
07/11/13   ah      Removed PDSM heap, instead use OS API for dynamic memory allocation
05/05/04   gk      Pre-SUPL changes
10/25/02   SK      Increased Pool 5 from #5 to #7 for MSBased.
07/03/02   SK      Modified buffer pools based on PE usage and MS-Based functionality.
04/11/02   cah     Fixed Header for file revision.
05/17/01   SK      Increased pool num for _05.
02/23/01   SK      Added FEATURE_GPS_MODE.
10/17/00   SK      Modified Buffers.
09/20/00   YLL     Changes for GPS Factory Test Mode.
09/15/00   SK      Optimized buffer sizes for RAM Savings
08/10/00   SK      Increased the Buffer count for buf_04 to 2 from 1
03/15/00   SK      Initial release.

===========================================================================*/

#include "comdef.h"

/**********************************************************
**  Defines related to dynamic buffer pools 
**********************************************************/

/* Define PDSM buffer pool realted fields. 
*/
#define PDSM_MAX_NBPOOLS           7       /* Max num of buffer pools */
#define PDSM_BPMAXB                8000    /* Max buffer pool size */
#define PDSM_BPMINB                0       /* Min buffer pool size */
#define PDSM_BPMAXN                50      /* Max buffers in one pool */

#define GNSS_PDAPI_BUFF_POOL_MUTEX_TAKE  gnss_pdapi_buff_pool_mutex_take_api()  
#define GNSS_PDAPI_BUFF_POOL_MUTEX_GIVE  gnss_pdapi_buff_pool_mutex_give_api()

/* Typedef enum pool_size_e is used for indicating the size of buffer in
** a pool. When increasing any Buffer pool size or number, dont forget to 
** also increase PDSM_BPMAXB and PDSM_DYN_MEMORY.
*/
typedef enum pool_size_e
{
  BPOOL_SIZE_MIN = 0,     /* End markers */
  BPOOL_SIZE_00  = 20,    /* used for aa_req,sa_req,msi_resp,lr_req */
  BPOOL_SIZE_01  = 36,    /* used for aa_req,sa_req,msi_resp,lr_req */
  BPOOL_SIZE_02  = 96,    /* Client commands */
  BPOOL_SIZE_03  = 260,   /* used by pmdsm_read and diag. 200 + misc */
  BPOOL_SIZE_04  = 732,   /* used for PR and PPM( Size for 1 part only.*/
                          /* NOTE: Tune This buffer if PPM/PR struct size is changed */
  BPOOL_SIZE_05  = 3800,  /* pdsm_srch_msg_u_type, used init time */
  BPOOL_SIZE_06  = 8000,  /* for IS801 fwd link PDDM */
  BPOOL_SIZE_MAX
} pool_size_e_type;

/* Typedef enum is used to indicate the number of buffers of size pool_size_e in
** a pool.
*/
#define PDSM_DYN_MEMORY              0x161E0   /* Static buffer space */

/* Make sure the Buffer Number does not go above PDSM_BPMAXN */
typedef enum pool_num_e_type
{
  BPOOL_NUM_MIN = 0,     /* End markers */
  BPOOL_NUM_00 = 12,
  BPOOL_NUM_01 = 12,      
  BPOOL_NUM_02 = 40,     /* Used for client commands, tune with num clients */  
  BPOOL_NUM_03 = 16,
  BPOOL_NUM_04 = 12,
  BPOOL_NUM_05 = 8,
  BPOOL_NUM_06 = 5,
  BPOOL_NUM_MAX
} pool_num_e_type;

/* typdef pdsm_pool_info_s is used to store the static pool information. Size of
** buffer and number of buffers of that size.
*/ 
typedef struct pdsm_pool_info_s {
    pool_size_e_type     size;     /* Size of buffer */
    pool_num_e_type      num;      /* Number of buffers of this size */
} pdsm_pool_info_s_type;

/* Structure which holds buffer pool information. 
*/
typedef struct pdsm_bpool {        /* Description of single pool */
    int bpsize;                    /* Size of this pool */
    int *bpnext;                   /* Pointer to free buffer list */
} pdsm_bpool_type;

/*===========================================================================
 * Function gnss_pdapi_buff_pool_mutex_take_api
 *
 * Description:
 *
 *  Function to get the buffer pool mutex lock
 *
 * Parameters:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
===========================================================================*/
void gnss_pdapi_buff_pool_mutex_take_api(void);

/*===========================================================================
 * Function gnss_pdapi_buff_pool_mutex_give_api
 *
 * Description:
 *
 *  Function to release the buffer pool mutex lock
 *
 * Parameters:
 *
 *  None
 *
 * Return value:
 *
 *  None
 *
===========================================================================*/
void gnss_pdapi_buff_pool_mutex_give_api(void);

/*===========================================================================

FUNCTION PDSM_POOL_INIT

DESCRIPTION
    This function is used to initialize the Dynamic buffer pools. This function 
    creates pools depending upon what PDSM requirement.

RETURN VALUE
  None

DEPENDENCIES
  Called only once from PDSM_TASK

===========================================================================*/

extern void pdsm_pool_init ( void );


/*===========================================================================

FUNCTION PDSM_GETBUF

DESCRIPTION
    This function is used to dynamically allocate a free buffer. It searches
    in the buffer pool corresponding to the size requested. At the most 
    two pools are searched for a free buffer to avoid memory waistage.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

extern char* pdsm_getbuf (word size);

/*===========================================================================

FUNCTION PDSM_FREEBUF

DESCRIPTION
    This function is used to dynamically deallocate a used buffer.

RETURN VALUE
  None

DEPENDENCIES

===========================================================================*/

extern boolean pdsm_freebuf (char* buf);

#ifdef __cplusplus
}
#endif

#endif /* PDSMBUF_H */
