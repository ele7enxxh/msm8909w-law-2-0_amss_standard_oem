/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                      D S H D R _ Q O S _ E F S I F               
           
GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2004-2010 by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/dshdr_qos_efsif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/16/10   ls      Change dshdr_efsif.h to dsutil.h
09/15/04   sy     Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "datamodem_variation.h"
#ifdef FEATURE_HDR_QOS
#include "dsutil.h"
#include "dss_iface_ioctl.h"

/*-----------------------------------------------------------------------------
  Defines   
-----------------------------------------------------------------------------*/
#define DSHDR_QOS_READ_BUFFER_SZ   128
#define DSHDR_QOS_MAX_QOS_RECORDS  50
  

/*-----------------------------------------------------------------------------
  Structure of QOS parameters that are needed to be read from the 
  efs file. This structure is used by the Packet Classifier to
  specify the flows.
-----------------------------------------------------------------------------*/
typedef struct 
{
  uint8  protocol;                                /* Protocol used              */
  ip_addr_type ip_address;                        /* Flow IP address            */
  ip_addr_type subnet_mask;                       /* Flow subner mask           */
  uint16 start_port;                              /* Port range                 */
  uint16 port_range;                              /* Valid port change          */
  uint8  rlp_rev_flw_id;                          /* RLP ID for the Rev rlp flow*/
}dshdr_qos_file_record_type;


/*-----------------------------------------------------------------------------
  Enum to specify the various return values 
  SUCCESS : Success
  EOL     : End of line is reached => record end
  END     : End of file is reached => file end => feof
  FAILURE : Failed 
-----------------------------------------------------------------------------*/

typedef enum 
{
  DSHDR_QOS_SUCCESS,
  DSHDR_QOS_EOL,
  DSHDR_QOS_EOF,
  DSHDR_QOS_FAILURE,
  DSHDR_QOS_END_OF_RECORD
}dshdr_qos_ret_val_enum_type;


/*===========================================================================
FUNCTION      DSHDR_QOS_GET_MRLP_TOTAL_RECORDS

DESCRIPTION   The function gets the maximum number of records presents in the
              EFS file.

DEPENDENCIES  None.

RETURN VALUE 
              0 : No records.
              n : Number of records successfully read from the EFS file.

SIDE EFFECTS  None.
===========================================================================*/

uint32 dshdr_qos_get_mrlp_total_records(void);


/*===========================================================================
FUNCTION      DSHDR_QOS_GET_MRLP_REC

DESCRIPTION   The function gets the nth record present.

DEPENDENCIES  None.

RETURN VALUE 
              
SIDE EFFECTS  None.
===========================================================================*/
int dshdr_qos_get_mrlp_rec(dshdr_qos_file_record_type*, uint32);


/*===========================================================================
FUNCTION      DSHDR_QOS_POWERUP_INIT

DESCRIPTION   The function intializes the state machine and 
              also opens the file

DEPENDENCIES  None.

RETURN VALUE 
              0  : DSHDR_QOS_SUCCESS: The file is good, readable, 
                            State Machine Initialized.
              -1 : DSHDR_QOS_FAILURE: The file cannot be opened/ readable. 

SIDE EFFECTS  None.
===========================================================================*/
uint32 dshdr_qos_powerup_init(void);

#endif /*FEATURE_HDR_QOS*/


