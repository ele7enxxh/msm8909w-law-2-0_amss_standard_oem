/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     D S _ Q M U X _ L O G G I N G . C

GENERAL DESCRIPTION

  The Data Services Qualcomm MSM Interface QMUX Logging source file.
     
EXTERNALIZED FUNCTIONS

  qmi_log_packet()
    This function logs the QMUX RX & TX PDU sent over a QMI Link

Copyright(c) 2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/qmicore/src/ds_qmux_logging.c#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/06/06    apm    Created file.
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include "msg.h"
#include "log.h"
#include "err.h"
#include "amssassert.h"
#include "log_codes.h"
#include "dsm.h"
#include "ds_qmux_logging.h"
#include "ds_qmux_msg.h"
#include "ds_Utils_DebugMsg.h"

/*===========================================================================
							
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
  Size includes IF Type (1), QMUX Header and QMUX SDU Header (3)
---------------------------------------------------------------------------*/
#define QMUX_QMI_HEADER_SIZE    (sizeof(byte) + sizeof(qmux_hdr_type) + 3)


/*===========================================================================

                           FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================
  FUNCTION QMI_LOG_PACKET

  DESCRIPTION
    Function to log RX & TX QMUX PDUs

  PARAMETERS
    item_ptr     - pointer to the dsm chained to be logged
    qmi_instance - the QMI instance being used
    direction    - TX or RX

  This function constructs the appropriate log code using the 
  qmi_instance and direction parameters.

  LOG CODE RANGE      DIRECTION      FRAMING
  --------------      ---------      -------
  5006-5038             RX/TX          DIAG

  DEPENDENCIES
    None

  RETURN VALUE
    None

  SIDE EFFECTS
    None
===========================================================================*/
void qmi_log_packet
(
  dsm_item_type *        item_ptr, 
  qmi_instance_e_type    qmi_instance,
  qmux_direction_e_type  direction
)
{
  uint8 *                  log_ptr;      /* pointer to log buffer          */
  uint16                   len;          /* length of log frame payload    */
  uint16                   log_code;     /* the log_code of the log packet */
  uint16                   offset;       /* contains the offset value      */
  uint32                   log_code_range_1;
  uint32                   log_code_range_2;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  len = 0;

  if(item_ptr == NULL) 
  {
    /*-----------------------------------------------------------------------
      Log item passed in is NULL, so do nothing.
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR_0("NULL item ptr, frame skipped.");
    return;
  }

  if (qmi_instance >= QMI_INSTANCE_MAX) 
  {
    LOG_MSG_ERROR_1("invalid instance! %d", qmi_instance);
    ASSERT(0);
  }

  if(direction == QMUX_TX) 
  {
    LOG_MSG_INFO3_0("Logging QMUX_TX");
  }
  else if(direction == QMUX_RX)
  {
    LOG_MSG_INFO3_0("Logging QMUX_RX");
  }
  else
  {
    LOG_MSG_ERROR_1("invalid direction! %d", direction);
    ASSERT(0);
  }

  /*-------------------------------------------------------------------------
    Calculating log code
    The log code specifies which QMI link and direction the QMUX PDU was sent.
   
    NOTE: no range checking is done so incoming parameters MUST be correct.
  -------------------------------------------------------------------------*/
  log_code_range_1 = (LOG_QMI_LAST_C - LOG_QMI_RESERVED_CODES_BASE_C) / 2;
  log_code_range_2 = (LOG_QMI_NEW_LAST_C - LOG_QMI_RESERVED_NEW_CODES_BASE_C) / 2;
  if ((uint32)qmi_instance < log_code_range_1)
  {
    log_code = (uint16)(LOG_QMI_RESERVED_CODES_BASE_C + (qmi_instance * 2) +
                     (uint8) direction);
  }
  else if ((uint32)qmi_instance < log_code_range_1 + log_code_range_2)
  {
    log_code = (uint16)(LOG_QMI_RESERVED_NEW_CODES_BASE_C +
          ((qmi_instance - log_code_range_1) * 2) + (uint8) direction);
  }
  else
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Determine length of data packet chain.
  -------------------------------------------------------------------------*/
  len = (uint16)dsm_length_packet(item_ptr);

  /*-------------------------------------------------------------------------
    Allocate entire log buffer space
  -------------------------------------------------------------------------*/
  offset = 0;
  log_ptr = (uint8*)log_alloc(log_code, len + sizeof(log_hdr_type));
  if(log_ptr == NULL) 
  {
    /*-----------------------------------------------------------------------
      If the entire packet could not be logged then log only the QMUX QMI
      header.  Skip first byte, we'll set it explicitly to 0xFE below to
      indicate this is a partially logged QMI packet.
    -----------------------------------------------------------------------*/
    len = QMUX_QMI_HEADER_SIZE;
    log_ptr = (uint8*)log_alloc(log_code, len + sizeof(log_hdr_type));
    offset = 1;
    len -= 1;
  }

  if(log_ptr == NULL) 
  {
    LOG_MSG_INFO3_0("Can't log QMI packet - no diag bufs!");
    return;
  }

  /*-----------------------------------------------------------------------
    Extract data from item_ptr into log buffer(log_ptr).
  -----------------------------------------------------------------------*/
  if(dsm_seek_extract(&item_ptr, 
                      &offset, 
                      log_ptr + sizeof(log_hdr_type) + offset, 
                      &len) == FALSE)
  {
    LOG_MSG_INFO1_0("seek extract failed!");
    log_free(log_ptr);
    return;
  }

  /*-----------------------------------------------------------------------
    If offset was not 0, diag memory is low and we were only able to get a
    small diag buffer, enough to log a partial QMI message (header only).

    Set the I/F Type field to 0xFE to let DM and reader know that this is
    the case.
  -----------------------------------------------------------------------*/
  if(offset)
  {
    *(log_ptr + sizeof(log_hdr_type)) = 0xFE;
  }

  /*-------------------------------------------------------------------------
    Dispatch log buffer to diag for delivery to diagnostic monitor
  -------------------------------------------------------------------------*/
  log_commit(log_ptr);

} /* qmi_log_packet() */
