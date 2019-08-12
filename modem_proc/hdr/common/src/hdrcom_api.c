/*===========================================================================

           H D R   C O M M O N   A P I   W R A P P E R   M O D U L E 


DESCRIPTION
  This file is the interface exported by HDR COMMON 

EXTERNALIZED FUNCTIONS (Global)
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.


Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/src/hdrcom_api.c#1 $ 
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/24/12   smd     Removed feature defines from SU API header files.
10/19/11   smd     SU API clean up updates.
09/15/11   smd     Created this file.

===========================================================================*/

/*===========================================================================

                    INCLUDE  FILES  FOR  MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "hdrcom_api.h"
#include "hdrtrace.h"
#include "hdrlog.h"
#include "hdrind.h"
#include "hdrrx.h"
#include "hdrutil.h"
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/

/*===========================================================================

                      FUNCTION  DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRTRACE_REPORT2

DESCRIPTION
  This function is a wrapper function of hdrtrace_report2

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcom_trace_report2(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size
)
{
  hdrtrace_report2(
    event_id,
    item1,
    item1_size,
    item2,
    item2_size );

}/* hdrcom_trace_report2 */
/*===========================================================================

FUNCTION HDRTRACE_REPORT3

DESCRIPTION
  This the wrapper function of hdrtrace_report3

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcom_trace_report3(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size
)
{
  hdrtrace_report3(
     event_id,
     item1,
     item1_size,
     item2,
     item2_size,
     item3,
     item3_size );

} /* hdrcom_trace_report3 */

/*===========================================================================

FUNCTION HDRCOM_TRACE_REPORT4

DESCRIPTION
  This is the wrapper function of hdrtrace_report4

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void hdrcom_trace_report4(
  trace_id_enum_type event_id,
  uint32 item1,
  uint8 item1_size,
  uint32 item2,
  uint8 item2_size,
  uint32 item3,
  uint8 item3_size,
  uint32 item4,
  uint8 item4_size
)
{
   hdrtrace_report4(
        event_id,
        item1,
        item1_size,
        item2,
        item2_size,
        item3,
        item3_size,
        item4,
        item4_size );

} /* hdrcom_trace_report4 */


/*=========================================================================

FUNCTION HDRCOM_LOG_SET_DIPSW_CB

DESCRIPTION
  This is a wrapper function.
 
DEPENDENCIES
  None

PARAMETERS
  dipsw - The dip switch which is to be associated with callbacks.
  set  - Function to call when dip switch is set.
  reset - Function to call when dip switch is reset (cleared).

RETURN VALUE
  None

SIDE EFFECTS
  None

=========================================================================*/
void hdrcom_log_set_dipsw_cb
(
  hdrcom_log_dipsw_enum_type dipsw,
  void (*set)(void),
  void (*reset)(void)
)
{
  hdrlog_set_dipsw_cb
  (
    dipsw,
    set,
    reset
  );
} /* hdrcom_log_set_dipsw_cb */

/*===========================================================================

FUNCTION HDRCOM_IND_GET_IND_DATA_SIZE

DESCRIPTION
  This function returns the data size of the specificed indication name

DEPENDENCIES
  None
  
PARAMETERS
  ind_name - indication name

RETURN VALUE
  Size of the indication data

SIDE EFFECTS
  None

===========================================================================*/
uint32 hdrcom_ind_get_ind_data_size 
(
  hdrcom_ind_name_enum_type ind_name
)
{
	return hdrind_get_ind_data_size( ind_name );
} /* hdrcom_ind_get_ind_data_size */


/*===========================================================================

FUNCTION HDRCOM_UTIL_READ_NV_ITEM

DESCRIPTION
  This function reads an item from NV

DEPENDENCIES
  None.

RETURN VALUE
  NV command return value

SIDE EFFECTS
  ERR_FATAL if the command return is not DONE or NOTACTIVE

===========================================================================*/
nv_stat_enum_type hdrcom_util_read_nv_item
(
  nv_items_enum_type item_code, /* item to read */
  nv_item_type * item_ptr /* pointer to where to put the item */
)
{
  return hdrutil_read_nv_item( item_code, item_ptr ); 
} /* hdrcom_util_read_nv_item */


/*============================================================================
  HDRCOM_RX_QUEUE_IND
    This is a wrapper function.

  @context HDRRX task. 

  @sideeffects None 
============================================================================*/
void hdrcom_rx_queue_ind
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
    /**< Who the indication is for */
  hdrcom_ind_name_enum_type ind_name,
    /**< Name of the indication given */
  void *input_ind_data_ptr
    /**< Size of the data associated with this indication */
)
{
  hdrrx_queue_ind
  ( 	    
	protocol_name,
    ind_name,
    input_ind_data_ptr
  );
} /* hdrl1_rx_queue_ind */

/*===========================================================================

FUNCTION HDRCOM_DSM_SET_TRACER

DESCRIPTION
  This function sets dsm tracer

DEPENDENCIES
  None
  
PARAMETERS
  item_ptr - A pointer to an item to be reset

RETURN VALUE
  none

SIDE EFFECTS
  None

===========================================================================*/
void hdrcom_dsm_set_tracer 
(
  dsm_item_type * item_ptr
)
{
#ifdef FEATURE_DSM_MEM_CHK
    DSM_TOUCH_ITEM( item_ptr );
#endif
} /* hdrcom_dsm_set_tracer */

