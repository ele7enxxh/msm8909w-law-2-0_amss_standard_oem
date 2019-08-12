/*===========================================================================

             GM_BATCH_BREACH_LIST Header File

DESCRIPTION
  This header file contains definitions of data structure necessary for
  GM batch breach reporting

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
  Copyright (c) 2015 Qualcomm Technologies, Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header:

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/14   ssu   Additional Fix details parameters for Geofence Breach reports
10/08/13   ssu   Initial creation of file.
===========================================================================*/
#ifndef GM_BATCH_BREACH_LIST_H
#define GM_BATCH_BREACH_LIST_H

#include "gm_core.h"
#include "gm_ebee.h"


#define GEOFENCE_BREACH_TYPE_MAX 4
#define GEOFENCE_BREACH_CONFIDENCE_TYPE_MAX 3

/* Number of IPCs we send at a time in case of batch breach reporting.
   After the limit exceeds , we stop sending the IPCs for some time to
   allow the de-throttling of the IPC queue and then we continue . */
#define GEOFENCE_IPC_SENT_AT_A_TIME_THRESHOLD 10

/* The duration for which we need to keep the IPCs inactive for batch breach reporting .
   This inactivity is invoked to enable the IPC queue to get cleared up before sending the
  next set of IPCs.*/
#define GEOFENCE_BATCH_BREACH_IPC_INVACTIVE_DURATION_MS 100

/*Event type client is interested to receive*/
typedef enum
{
  GEOFENCE_BREACH_TYPE_ENTERING_AREA = 0, /*Entering Area Event*/
  GEOFENCE_BREACH_TYPE_INSIDE_AREA,       /*Inside Area Event*/
  GEOFENCE_BREACH_TYPE_OUTSIDE_AREA,       /*Outside Area Event*/
  GEOFENCE_BREACH_TYPE_LEAVING_AREA      /*Leaving Area Event*/
}sm_gm_breach_type;

typedef struct sm_gm_breach_info_type_s
{
  /* Confidence with which the breach is reported */
  uint32 breach_confidence;
  /* Type of geofence breach */
  sm_gm_breach_type breach_type;
}sm_gm_breach_info_type;

typedef struct sm_gm_batch_breach_node_type_s
{
  /* The starting range of continuous geofence ID */
  uint32 q_id_low;
  /* The ending range of continuous geofence ID */
  uint32 q_id_high;
  /* Confidence with which the breach is reported */
  uint32 q_breach_confidence;
  /* Type of geofence breach */
  sm_gm_geofence_event_type e_breach_type;

  struct sm_gm_batch_breach_node_type_s* p_next;
  struct sm_gm_batch_breach_node_type_s* p_prev;
}sm_gm_batch_breach_node_type;


/*===========================================================================

FUNCTION gm_batch_breach_info_init

DESCRIPTION
  This function is used to initialise the internal states and variables
  for batch breach reporting.

DEPENDENCIES

RETURN VALUE
  Current

SIDE EFFECTS
  None.

===========================================================================*/
void gm_batch_breach_info_init(void);


/*===========================================================================

FUNCTION gm_batch_breach_list_dyn_memory_deallocate

DESCRIPTION
  This function deallocates the dynamically allocated memory for the batch breach list

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gm_batch_breach_list_dyn_memory_deallocate(void);

/*===========================================================================

FUNCTION gm_batch_breach_list_add

DESCRIPTION
  This function adds a geofence node to the linked list based on the breach info

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
sm_gm_batch_breach_node_type* gm_batch_breach_list_add(sm_gm_breach_info_type* p_breach_info, uint32 *p_error_code, uint32 geofence_id);




/*===========================================================================

FUNCTION gm_batch_breach_list_send_ipc

DESCRIPTION
  This function is used to send the IPC to the client which has the batched breach report

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gm_batch_breach_list_send_ipc(sm_gm_breach_info_type *p_breach_info,
                                   sm_GnssFixRptStructType *p_gm_cached_pos_fix_rpt);


/*===========================================================================

FUNCTION gm_batch_breach_list_delete

DESCRIPTION
  This function is used to delete a geofence ID from the breach list .

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void gm_batch_breach_list_delete(sm_gm_breach_info_type* p_breach_info, uint32 *p_error_code, uint32 q_geofence_id);

#endif /*GM_BATCH_BREACH_LIST_H*/
