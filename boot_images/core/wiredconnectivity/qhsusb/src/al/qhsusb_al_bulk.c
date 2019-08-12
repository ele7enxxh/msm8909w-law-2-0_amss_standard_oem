/*=======================================================================*//**
 * @file        qhsusb_al_bulk.c
 * @author:     dpatnaik
 * @date        12-Feb-2011
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) implementation.
 *
 * @details     This file contains the implementation of the APIs to be used by
 *              the boot Sahara downloader.
 *
 * @note        
 *
 *              Copyright 2011 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/al/qhsusb_al_bulk.c#1 $ 
// $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
// 
// when         who        what, where, why
// --------   ---        ----------------------------------------------------------
// 03/28/11   niting     Added callback support
// 02/12/11   dpatnaik Initial revision
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"  // common defines - basic types as byte word uint32 etc
#include <string.h> // For memcpy
#include "qhsusb_al_bulk.h"
#include "qhsusb_fd_hdlc.h"
#include "qhsusb_dcd.h"

#include "HALhwio.h"
#include "qhsusb_dci_api.h" // can remove once the MAGIC NUM workaround in SBL is no longer needed
#include "qhsusb_dci_private.h" // for GPTIMER
#include "qhsusb_dci_hw_registers.h"    // For Multi-Core
#include "qhsusb_log.h"
#include "qhsusb_dci_hw_28nm_phy_registers.h"
#include <stringl/stringl.h>


//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------
#ifdef FEATURE_QHSUSB_BOOT_LOG_OVER_UART

#if defined(BUILD_HOSTDL) || defined(BUILD_EHOSTDL) || defined(FEATURE_EMMCBLD)
void qhsusb_boot_log_message(char * s)
{
}
#else
void qhsusb_boot_log_message(char * s)
{
    boot_log_message(s);
} 
#endif

#else /* FEATURE_QHSUSB_BOOT_LOG_OVER_UART */
void qhsusb_boot_log_message(char * s)
{
}
#endif /*FEATURE_QHSUSB_BOOT_LOG_OVER_UART*/

/* Maximum bulk transfer size */
#define QHSUSB_BULK_LAYER_MAX_TRANSFER_SIZE_IN_BYTES (1024*1024)

#define HSU_BULK_ZLP_TIMER_VAL_USEC 50000  /* 50 milliseconds */

#define QHSUSB_BULK_ENUM_TIMEOUT 0x00FFFFFF /*16 second wait for enumeration timeout*/

static uint8 qhsusb_bulk_str_product[26] = {                       // iProduct
   sizeof(qhsusb_bulk_str_product),USB_DESCTYPE_STRING,
   'Q',0,
   'H',0,
   'S',0,
   'U',0,
   'S',0,
   'B',0,
   '_',0,
   '_',0,
   'B',0,
   'U',0,
   'L',0,
   'K',0,
};

/* For synchronous read within QHSUSB
 * Client does not pass completion callback 
 *  and rely on QHSUSB to poll until transfer completion */
typedef struct
{
  uint32 actual_length;
  boolean pending_read;
} qhsusb_synch_read_ctx;

typedef enum
{
  QHSUSB_CHECK_FOR_ZLP,
  QHSUSB_LACKS_ZLP,
  QHSUSB_SENDS_ZLP
} qhsusb_zlp_mode_e;

static qhsusb_zlp_mode_e qhsusb_zlp_mode = QHSUSB_CHECK_FOR_ZLP;
static boolean qhsusb_pending_rx_zlp = FALSE;

static boolean qhsusb_waiting_for_zlp = FALSE; /* Used to indicate that we are waiting for a ZLP */
static uint32 cached_zlp_data_length = 0; /* Length of data received from the host instead of the impending ZLP */
static uint32 no_zlp_count = 0; /* Number of times host sent data instead of ZLP. Used for tracking/debug. */

static struct qhsusb_urb* qhsusb_al_bulk_tx_urb = NULL;
static struct qhsusb_dcd_dsc_device* qhsusb_al_bulk_dev = NULL;
static boolean qhsusb_enumerated = FALSE;

static bulk_rx_cb_type al_bulk_rx_cb = NULL; /* hold the callback for propagation to upper layer */
static qhsusb_synch_read_ctx qhsusb_synch_rx_ctx;

#define HSU_AL_ZLP_BUF_SIZE 512
static uint8 zlp_read_buf[HSU_AL_ZLP_BUF_SIZE];

#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
static boolean qhsusb_is_disconnected = FALSE;
static boolean qhsusb_is_rx_in_progress = FALSE;
#endif

boolean qhsusb_al_check_for_pbl_dload(uint32 core_id)
{
#ifdef FEATURE_RUMI_BOOT
   return FALSE;
#else
   qhsusb_boot_log_message("USB: qhsusb_al_check_for_pbl_dload");
   return qhsusb_dci_check_for_pbl_dload(core_id);
#endif
}

/* setup GPTIMER0 with timer in microseconds
 * max value is 0x00FFFFFF or 16.777215 seconds
 */
void qhsusb_al_bulk_setup_timer(uint32 timer_val)
{
  DCI_HWIO_OUTM(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_USBINTR, USB_OTG_HS_USBINTR_TIE0_BMSK, 1 << USB_OTG_HS_USBINTR_TIE0_SHFT);

  qhsusb_log(QHSUSB_AL_BULK_SETUP_TIMER_LOG,0,timer_val);
  
  if (timer_val == 0) timer_val = 1; /* handle corner case */
  DCI_HWIO_OUT(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_GPTIMER0LD, ((timer_val - 1) << USB_OTG_HS_GPTIMER0LD_GPTLD_SHFT) & USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);
}

void qhsusb_al_bulk_start_timer(void)
{
  /* clear the interrupt, avoid write to clear registers */
  DCI_HWIO_OUT(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_USBSTS, USB_OTG_HS_USBSTS_TI0_BMSK);

  qhsusb_log(QHSUSB_AL_BULK_START_TIMER_LOG,0,0);
  /* start timer as single shot mode */
  DCI_HWIO_OUT(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_GPTIMER0CTRL, 
          USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK | USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK);
}

void qhsusb_al_bulk_stop_timer(void)
{
  qhsusb_log(QHSUSB_AL_BULK_STOP_TIMER_LOG,0,0);
  DCI_HWIO_OUT(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_GPTIMER0CTRL, 0); 
}

boolean qhsusb_al_check_timer_for_expiry(void)
{
  boolean ret_val = FALSE;

  /* GPTIMER sometimes not trigger interrupt in USBSTS, so we also need to check GPTIMER0CTRL
   * if the timer count down has gone to 0 
   */
  if ((DCI_HWIO_INM(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_USBSTS, USB_OTG_HS_USBSTS_TI0_BMSK))
   || (DCI_HWIO_IN(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_GPTIMER0CTRL) == USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK))
  {
    /* clear the interrupt, avoid write to clear registers */
    DCI_HWIO_OUT(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_USBSTS, USB_OTG_HS_USBSTS_TI0_BMSK);

    qhsusb_al_bulk_stop_timer();
    ret_val = TRUE;
    qhsusb_log(QHSUSB_AL_BULK_ZLP_TIMER_EXPIRED_LOG,0,ret_val);
  }
  
  return ret_val;
}

uint32 qhsusb_al_bulk_receive_zlp(void)
{
  uint32 err_code = FD_HDLC_SUCCESS;
  
  qhsusb_log(QHSUSB_AL_BULK_RECEIVE_ZLP_LOG,0,0);
  /* prepare an empty TD to read ZLP */
  err_code = qhsusb_fd_hdlc_rx_pkt(zlp_read_buf, HSU_AL_ZLP_BUF_SIZE);
  if (err_code != FD_HDLC_SUCCESS)
  {
    /* handle error */
  }
  else
  {
    if (qhsusb_zlp_mode == QHSUSB_CHECK_FOR_ZLP)
    {
      qhsusb_pending_rx_zlp = TRUE;
      qhsusb_al_bulk_setup_timer(HSU_BULK_ZLP_TIMER_VAL_USEC);
      qhsusb_al_bulk_start_timer();
      /* return flow control and let the next iteration of qhsusb_al_bulk_rx_complete() handle ZLP */
    }
    /* else for optimization, do not prime start timer for ZLP */
  }
  return err_code;
}

uint32 qhsusb_al_bulk_handle_rx_zlp_mode(void)
{
  uint32 err_code = FD_HDLC_SUCCESS;
  
  qhsusb_log(QHSUSB_AL_BULK_HANDLE_RX_ZLP_MODE_LOG,0,qhsusb_zlp_mode);
  switch(qhsusb_zlp_mode)
  {
    case QHSUSB_CHECK_FOR_ZLP:
    case QHSUSB_SENDS_ZLP:
      qhsusb_waiting_for_zlp = TRUE;
      err_code = qhsusb_al_bulk_receive_zlp();
      break;
    case QHSUSB_LACKS_ZLP:
      /* host is determined to not send ZLP */
      break;
    default:
      break;
  }
  return err_code;
}

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_rx_complete
 * 
 * @brief   This function will be a called after each URB receive is complete.
 *
 * @details This function will be a called after each URB receive is complete.
 * 
 * @param   *urb   -  [IN/OUT] URB corresponding to the current reception
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qhsusb_al_bulk_rx_complete(struct qhsusb_urb* urb)
{
   static uint32 total_bytes_received = 0;
   boolean short_packet_received = FALSE;
   uint32 err_code = FD_HDLC_SUCCESS;

   qhsusb_log(QHSUSB_AL_BULK_RX_COMPLETE_LOG,0,0);
   qhsusb_boot_log_message("USB: qhsusb_al_bulk_rx_complete, Start");
   if (QHSUSB_URB_STATUS_COMPLETE_OK == urb->transfer_status)
   {
     if (urb->actual_length == 0) /* ZLP */
     {
       qhsusb_log(QHSUSB_AL_BULK_ZLP_ARRIVED_LOG,0,0);
       qhsusb_waiting_for_zlp = FALSE;      
       short_packet_received = TRUE; /* ZLP coming anytime from the host */

       /* also look for ZLP that is pending from last iteration of priming buffers to trap ZLP */
       if (qhsusb_pending_rx_zlp)
       {
         /* host confirmed to send ZLP, can optimize and remov subsequent ZLP timer activation */
         qhsusb_zlp_mode = QHSUSB_SENDS_ZLP;
         qhsusb_pending_rx_zlp = FALSE;
       }
     }
     if ((urb->actual_length % qhsusb_al_bulk_get_max_packet_size()) > 0) /* check for short packet */
     {
       short_packet_received = TRUE; /* end of transfer, return to upper layer */
       qhsusb_log(QHSUSB_AL_BULK_SHORT_PACKET_RECVD_LOG,0,total_bytes_received);
     }
     total_bytes_received += urb->actual_length;
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	qhsusb_is_rx_in_progress = FALSE;
#endif
	qhsusb_log(QHSUSB_TRANSFER_DONE_LOG,0,total_bytes_received);
   }
   else
   {
     /* TODO handle other exceptional cases from lower layer */


     /* signature of ZLP being missing from the host */
     if (QHSUSB_URB_STATUS_COMPLETE_ERROR == urb->transfer_status)
     {
       qhsusb_log(QHSUSB_TRANSFER_ERROR_LOG,0,0);
       qhsusb_boot_log_message("USB: qhsusb_al_bulk_rx_complete, Signature ZLP missing HOST");
       if (qhsusb_pending_rx_zlp)
       {
         short_packet_received = TRUE;
         qhsusb_pending_rx_zlp = FALSE;
       }
     }
   }

   /* For short packets, transfer is complete, return the number of bytes transferred to the upper layer.
    * Otherwise, prime buffers to handle ZLP as needed.
    */
   if (urb->transfer_length == urb->actual_length || short_packet_received)
   {

       /* We expected a ZLP but received data from the host, then call the callback into client layer */
       if (qhsusb_waiting_for_zlp && (urb->actual_length != 0))
       {
         qhsusb_log(QHSUSB_AL_BULK_DATA_RECVD_INSTEAD_OF_ZLP_LOG,0,urb->actual_length);
          qhsusb_boot_log_message("USB: qhsusb_al_bulk_rx_complete, QHSUSB_AL_BULK_DATA_RECVD_INSTEAD_OF_ZLP_LOG");
         al_bulk_rx_cb(total_bytes_received - urb->actual_length, BULK_SUCCESS); /* Adjust bytes received and callback */
         al_bulk_rx_cb = NULL; 
         total_bytes_received = urb->actual_length; /* Update bytes received */
         cached_zlp_data_length = urb->actual_length; /* Update count of how many bytes we are holding on to */
         no_zlp_count++; /* Increment count that keeps track of no ZLP condition */
         return;
       }


     if ((urb->transfer_length == urb->actual_length) &&
         ((urb->transfer_length % qhsusb_al_bulk_get_max_packet_size()) == 0))
     {
        if (qhsusb_zlp_mode == QHSUSB_LACKS_ZLP) /* No ZLP is expected */
        {
          /* transfer is complete, return bytes transferred to upper layer immediately */
          if (al_bulk_rx_cb != NULL)
          {
            qhsusb_log(QHSUSB_AL_BULK_RX_CB_RX_DONE_LOG,0,total_bytes_received);
            qhsusb_boot_log_message("USB: qhsusb_al_bulk_rx_complete, QHSUSB_AL_BULK_RX_CB_RX_DONE_LOG");
            al_bulk_rx_cb(total_bytes_received, BULK_SUCCESS);
            al_bulk_rx_cb = NULL; /* use only once */
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
			qhsusb_is_rx_in_progress = FALSE;
#endif
            total_bytes_received = 0;
          }
          else /* synchronous read */
          {
            qhsusb_synch_rx_ctx.actual_length = total_bytes_received;
            qhsusb_synch_rx_ctx.pending_read = FALSE;

            total_bytes_received = 0;
          }
        }
        else
        {
          /* Handle ZLP only when necessary */
          err_code = qhsusb_al_bulk_handle_rx_zlp_mode();
          if (FD_HDLC_SUCCESS != err_code) /* error condition */
          {
            /* error setting up subsequent priming, return error condition immediately */
            if (al_bulk_rx_cb != NULL)
            {
              qhsusb_boot_log_message("USB: qhsusb_al_bulk_rx_complete, ZLP Error");
              al_bulk_rx_cb(total_bytes_received, BULK_ERR_READ);
              al_bulk_rx_cb = NULL; /* use only once */
              total_bytes_received = 0;
            }
            else /* synchronous read */
            {
              qhsusb_synch_rx_ctx.actual_length = total_bytes_received;
              qhsusb_synch_rx_ctx.pending_read = FALSE;
              total_bytes_received = 0;
            }
          }
        }
        /* return and let the next iteration handle ZLP */
     }
     else
     {
       if (al_bulk_rx_cb != NULL)
       {
         qhsusb_log(QHSUSB_AL_BULK_RX_CB_SHORT_PACKET_LOG,0,total_bytes_received);
         qhsusb_boot_log_message("USB: qhsusb_al_bulk_rx_complete, Short Packet Success");
         al_bulk_rx_cb(total_bytes_received, BULK_SUCCESS);
         al_bulk_rx_cb = NULL; /* use only once */
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
		 qhsusb_is_rx_in_progress = FALSE;
#endif
         total_bytes_received = 0;
       }
       else /* synchronous read */
       {
         qhsusb_synch_rx_ctx.actual_length = total_bytes_received;
         qhsusb_synch_rx_ctx.pending_read = FALSE;

         total_bytes_received = 0;
       }
     }
   }
   else if(urb->actual_length > urb->transfer_length)
   {
     qhsusb_log(QHSUSB_TRANSFER_ERROR_LOG,0,total_bytes_received);
     DCI_ASSERT(QHSUSB_ERR_INVALID_OPERATION);
   }
   else /* more data to read */
   {
     uint32 remaining_read_size = urb->transfer_length - urb->actual_length;
     uint32 current_read_size = 0;

     /* move the iterator to new destination address */
     uint8 *transfer_buffer_iter = (uint8*)(urb->transfer_buffer_ptr);
     qhsusb_log(QHSUSB_AL_BULK_RX_MORE_DATA_LOG,0,remaining_read_size);

     transfer_buffer_iter += urb->actual_length;
     
     current_read_size = (remaining_read_size <= qhsusb_al_bulk_get_max_raw_data_size()) 
         ? remaining_read_size : qhsusb_al_bulk_get_max_raw_data_size();

     err_code = qhsusb_fd_hdlc_rx_pkt((void *)transfer_buffer_iter, current_read_size);
     if (FD_HDLC_SUCCESS != err_code)
     {
       /* error setting up subsequent reads, return error condition immediately */
       if (al_bulk_rx_cb != NULL)
       {
         qhsusb_boot_log_message("USB: qhsusb_al_bulk_rx_complete, Error setting subsequent read, sending BULK_SUCCESS?");
         al_bulk_rx_cb(total_bytes_received, BULK_SUCCESS);
         al_bulk_rx_cb = NULL; /* use only once */
         total_bytes_received = 0;
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
         qhsusb_is_rx_in_progress = FALSE;
#endif 

       }
       else /* synchronous read */
       {
         qhsusb_synch_rx_ctx.actual_length = total_bytes_received;
         qhsusb_synch_rx_ctx.pending_read = FALSE;
         total_bytes_received = 0;
       }
     }
     else
     {
       /* TODO reset the timer if there is more to read */
       /* otherwise let the read proceed */
     }
   }
}

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_tx_complete
 * 
 * @brief   This function will be a called after each URB transmit is complete.
 *
 * @details This function will be a called after each URB transmit is complete.
 * 
 * @param   *urb   -  [IN/OUT] URB corresponding to the current transmission
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qhsusb_al_bulk_tx_complete(struct qhsusb_urb* urb)
{
  qhsusb_log(QHSUSB_AL_BULK_TX_COMPLETE_LOG,0,0);
  qhsusb_boot_log_message("USB: qhsusb_al_bulk_tx_complete, SUCCESS ");
  qhsusb_al_bulk_tx_urb = urb;
}

// ===========================================================================
/**
 * @function    qhsusb_al_enum_complete
 * 
 * @brief   This function is registered to be called when enumeration is
 *          complete.
 *
 * @details This function will set a static variable to indicate enumeration has
 *          completed.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qhsusb_al_enum_complete( void )
{
  qhsusb_log(QHSUSB_AL_ENUM_COMPLETE_LOG,0,0);
  qhsusb_boot_log_message("USB: qhsusb_al_enum_complete, SUCCESS ");
  qhsusb_enumerated = TRUE;
}

#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
// ===========================================================================
/**
 * @function    qhsusb_al_disconnect_handler
 * 
 * @brief   This function will be called if there is a disconnect or bus reset
 *			during transfer
 *
 * @details This function will set a static variable to indicate the error.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qhsusb_al_disconnect_handler( void )
{
  qhsusb_boot_log_message("USB: qhsusb_al_disconnect_handler, USB Disconnected");
	qhsusb_is_disconnected = TRUE;
	return; 
}
#endif

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_init
 * 
 * @brief   This function initializes the USB core and keeps polling until
 *          enumeration is complete.
 *
 * @details This function initializes the USB core and keeps polling until
 *          enumeration is complete.
 * 
 * @param   core_id   -  [IN] USB core ID to initialize
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qhsusb_al_bulk_init(int core_id, qhsusb_bulk_mode_type bulk_mode_type)
{
  qhsusb_al_bulk_tx_urb = NULL;
  qhsusb_enumerated = FALSE;

  qhsusb_synch_rx_ctx.actual_length = 0;
  qhsusb_synch_rx_ctx.pending_read = FALSE;

  qhsusb_boot_log_message("USB: qhsusb_al_bulk_init, Start"); 
  qhsusb_log(QHSUSB_AL_BULK_INIT_ENTRY_LOG,0,bulk_mode_type);
  qhsusb_fd_hdlc_set_core(core_id);
  /*Indicate to lower layers that the bulk transfer is going on*/
  qhsusb_fd_hdlc_bulk_in_progress(TRUE); 
  qhsusb_fd_set_bulk_mode(bulk_mode_type);
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
  qhsusb_al_bulk_dev = qhsusb_fd_hdlc_init(qhsusb_al_bulk_rx_complete,
                                           qhsusb_bulk_str_product,
                                           qhsusb_al_bulk_tx_complete,
                                           qhsusb_al_enum_complete,
										   qhsusb_al_disconnect_handler,
                                           FD_CLIENT_SUPPLIED_BUFFER_MODE);
#else
   qhsusb_al_bulk_dev = qhsusb_fd_hdlc_init(qhsusb_al_bulk_rx_complete,
                                           qhsusb_bulk_str_product,
                                           qhsusb_al_bulk_tx_complete,
                                           qhsusb_al_enum_complete,
                                           FD_CLIENT_SUPPLIED_BUFFER_MODE);
#endif
    if (core_id == QHSUSB_CORE_ID_HS )
    {
        /*Setup the enumeration timeout timer*/
        qhsusb_al_bulk_setup_timer(QHSUSB_BULK_ENUM_TIMEOUT);
        /*start the timer*/
        qhsusb_al_bulk_start_timer();
    }
  while ( !qhsusb_enumerated )
  {
        if (TRUE == qhsusb_al_check_timer_for_expiry() &&  QHSUSB_CORE_ID_HS == core_id)
        {
            qhsusb_boot_log_message("USB: qhsusb_al_bulk_init, ENUM Timer Expired, trying again");
            qhsusb_al_bulk_shutdown();
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
            qhsusb_al_bulk_dev = qhsusb_fd_hdlc_init(qhsusb_al_bulk_rx_complete,
                                               qhsusb_bulk_str_product,
                                               qhsusb_al_bulk_tx_complete,
                                               qhsusb_al_enum_complete,
                                               qhsusb_al_disconnect_handler,
                                               FD_CLIENT_SUPPLIED_BUFFER_MODE);
#else
            qhsusb_al_bulk_dev = qhsusb_fd_hdlc_init(qhsusb_al_bulk_rx_complete,
                                               qhsusb_bulk_str_product,
                                               qhsusb_al_bulk_tx_complete,
                                               qhsusb_al_enum_complete,
                                               FD_CLIENT_SUPPLIED_BUFFER_MODE);
#endif
            /*Setup the enumeration timeout timer*/
            qhsusb_al_bulk_setup_timer(QHSUSB_BULK_ENUM_TIMEOUT);
            /*start the timer*/
            qhsusb_al_bulk_start_timer();
        }
    qhsusb_al_bulk_poll();
  }
    /*If enumeration successful before timer expiry, turn off the timer*/
    if (QHSUSB_CORE_ID_HS == core_id)
    {
        qhsusb_boot_log_message("USB: qhsusb_al_bulk_init, ENUM Success");
        qhsusb_al_bulk_stop_timer();
    }
  qhsusb_fd_hdlc_bulk_in_progress(FALSE);
  qhsusb_log(QHSUSB_AL_BULK_INIT_EXIT_LOG,0,bulk_mode_type);
}

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_poll
 * 
 * @brief   This function polls the hardware for events.
 *
 * @details This function polls the hardware for events.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qhsusb_al_bulk_poll(void)
{
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	uint32 total_bytes_received = 0x0; 
#endif
	qhsusb_dcd_poll(qhsusb_al_bulk_dev);
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	/*Now polling is controlled by application while we receive the data*/
	/* Now if cable is disconnected in the RX path inform the application*/
	if((TRUE== qhsusb_is_rx_in_progress) && (TRUE == qhsusb_is_disconnected))
   {
		/*call their callback and tell higher layers of a disconnect*/
	  if (al_bulk_rx_cb != NULL)
   {
      qhsusb_boot_log_message("USB: qhsusb_al_bulk_poll, Cable Pulled during Transfer ");
			al_bulk_rx_cb(total_bytes_received, BULK_ERR_CABLE_DISCONNECT);
			al_bulk_rx_cb = NULL; /* use only once */
   }
	   /*Clear all the flags used*/
	   qhsusb_is_rx_in_progress = FALSE;
	   qhsusb_is_disconnected = FALSE;

   }
#endif
  /* determine whether host will send ZLP or timeout */
  if (qhsusb_pending_rx_zlp)
  {
    if (qhsusb_al_check_timer_for_expiry())
    {
      /* timer already stopped and cleared at this point */

      /* subsequent actions upon timeout */
      switch (qhsusb_zlp_mode)
      {
        case QHSUSB_CHECK_FOR_ZLP:
          qhsusb_zlp_mode = QHSUSB_LACKS_ZLP;
          qhsusb_pending_rx_zlp = FALSE;
          qhsusb_waiting_for_zlp = FALSE;
          qhsusb_dci_cancel_transfer(qhsusb_al_bulk_dev->core_id, 1, DIR_RX);
          break;
        case QHSUSB_SENDS_ZLP:
          /* error condition, host times out when ZLP is expected */
          qhsusb_log(QHSUSB_AL_BULK_ZLP_TIMER_EXPIRED_LOG,0,0);
          DCI_ASSERT(QHSUSB_ERR_INVALID_OPERATION);
          break;
        case QHSUSB_LACKS_ZLP:
          /* should not arrive here since qhsusb_pending_rx_zlp would not be set */
          break;
        default:
          break;
      }
    }
    /* allow polling to continue as timer has not expired yet */
  }
}

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_shutdown
 * 
 * @brief   This function performs a shutdown of the hardware.
 *
 * @details This function performs a shutdown of the hardware.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qhsusb_al_bulk_shutdown(void)
{
  qhsusb_log(QHSUSB_AL_BULK_SHUTDOWN_LOG,0,0);
  qhsusb_boot_log_message("USB: qhsusb_al_bulk_shutdown, Start");
  if(QHSUSB_CORE_ID_HS == qhsusb_al_bulk_dev->core_id)
  {
#ifndef FEATURE_RUMI_BOOT
    /*For RUMI verification only. ULPI r/w shouldn't be done*/
    /*configure the PHY in Non-Drive Mode*/
    qhsusb_dci_ulpi_write(QHSUSB_CORE_ID_HS, USB_OTG_HS_FNCTL_REG_ADDR, USB_OTG_HS_FNCTL_OPMODE_NONDRIVE_VAL,USB_OTG_HS_FNCTL_OPMODE_MASK);
#endif

    /*Clear the run stop bit*/
	  DCI_HWIO_OUTM(qhsusb_al_bulk_dev->core_id, USB_OTG_HS_USBCMD, USB_OTG_HS_USBCMD_RS_BMSK, USB_OTG_HS_USBCMD_RS_STOP_FVAL); 

    /*call a 10msec delay to allow for D+ settling down*/
    qhsusb_dci_delay_ms(10);
  }
  qhsusb_fd_hdlc_shutdown();
  qhsusb_boot_log_message("USB: qhsusb_al_bulk_shutdown, Complete");
}

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_receive
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the receive
 *          URB is not NULL, upon which the data will be copied to the
 *          input receive buffer.
 * 
 * @param   *rx_buf     -  [IN/OUT] Receive buffer for data
 * @param   len         -  [IN    ] Length of buffer
 * @param   rx_cb       -  [IN    ] Return callback function
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes receieved.
 * 
 */
uint32 qhsusb_al_bulk_receive(byte *rx_buf, /* must be 4 byte aligned       */
                              uint32 len,   /* read buffer size             */
                                            /* must be multiples of max     */
                                            /* packet size                  */
                              bulk_rx_cb_type rx_cb, /* completion callback */
                              uint32 * err_code)     /* error code returned */
{
  uint32 current_read_size = 0;

  uint8 *transfer_buffer_iter = (uint8*)(rx_buf);
  uint32 remaining_read_size = len;
  
  qhsusb_log(QHSUSB_AL_BULK_RECEIVE_LOG,0,len);
  qhsusb_boot_log_message("USB: qhsusb_al_bulk_receive, Start");

  if (err_code == NULL || len == 0)
  {
    return 0;
  }

  if (qhsusb_waiting_for_zlp) // The ZLP buffer has received data due to host skipping sending ZLP
  {
    qhsusb_waiting_for_zlp = FALSE; /* Clear flag */
    
    if(len < cached_zlp_data_length) /* If FH layer asks for less than what host sent us instead of the ZLP */
    {
        cached_zlp_data_length = len;
    }
    
    qhsusb_log(QHSUSB_AL_BULK_RX_ZLP_COPY_LOG ,0, cached_zlp_data_length);
    memscpy((void*)(rx_buf), len,(void*)(zlp_read_buf), cached_zlp_data_length);

    if (cached_zlp_data_length < HSU_AL_ZLP_BUF_SIZE) // We received a short packet instead of the ZLP so we are done.
    {
        if(rx_cb)
        {
            qhsusb_log(QHSUSB_AL_BULK_RX_CB_NO_ZLP_BUT_SHORT_PACKET_LOG,0,cached_zlp_data_length);
            rx_cb(cached_zlp_data_length, BULK_SUCCESS);
        } 
        
        al_bulk_rx_cb = NULL; /* use only once */
        cached_zlp_data_length = 0;
        return 0 ;
    }
    
    remaining_read_size = len - cached_zlp_data_length;

    /* move the iterator to new destination address */
    transfer_buffer_iter += cached_zlp_data_length;

    cached_zlp_data_length = 0;
  }    
 
  /* enforce read only upto qhsusb_al_bulk_get_max_raw_data_size( ) */
  current_read_size = ((remaining_read_size <= qhsusb_al_bulk_get_max_raw_data_size()) ?
          remaining_read_size : qhsusb_al_bulk_get_max_raw_data_size());

  /* setup read */

   qhsusb_log(QHSUSB_AL_BULK_RECEIVE_LOG,0,current_read_size);
  *err_code = qhsusb_fd_hdlc_rx_pkt((void *)transfer_buffer_iter, current_read_size);
  
  if (FD_HDLC_SUCCESS != *err_code)
  {
    return 0; /* error condition */
  }

  if (rx_cb == NULL)
  {
    /* Synchronous read when client does not pass callback.
     * Poll in USB layer until completion */

    /* safe to set flag here on the assumption that there is no polling in between packet setup and here */
    qhsusb_synch_rx_ctx.pending_read = TRUE;
    while (qhsusb_synch_rx_ctx.pending_read)
    {
      qhsusb_al_bulk_poll();
    }
    return qhsusb_synch_rx_ctx.actual_length;
  }
  else
  {
    /* client layer will perform the polling */
    /* transfer completes when client's cb is invoked */
    qhsusb_boot_log_message("USB: qhsusb_al_bulk_receive, Complete");
    al_bulk_rx_cb = rx_cb;
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
    qhsusb_is_rx_in_progress = TRUE;
#endif
    *err_code = BULK_SUCCESS;
    return 0;
  }
}

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_transmit
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the transmit
 *          URB transfer is complete.
 * 
 * @param   *tx_buf     -  [IN/OUT] Transmit buffer for data
 * @param   len         -  [IN    ] Length of data to trasmit in bytes
 * @param   tx_cb       -  [IN    ] Transmit complete callback
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes transmitted.
 * 
 */
uint32 qhsusb_al_bulk_transmit(byte *tx_buf,     /* must be 4 byte aligned   */
                               uint32 len,       /* number of bytes to send  */
                               bulk_tx_cb_type tx_cb, /* completion callback */
                               uint32 * err_code)     /* returned error code */
{
  uint32 tx_length;
  *err_code = BULK_SUCCESS;

  qhsusb_log(QHSUSB_AL_BULK_TRANSMIT_LOG,0,0);
  qhsusb_log(QHSUSB_TRANSFER_LENGTH_LOG,0,len);
  
  qhsusb_boot_log_message("USB: qhsusb_al_bulk_transmit, Start");
  
  qhsusb_fd_hdlc_tx_pkt(tx_buf, len);

  while (qhsusb_al_bulk_tx_urb == NULL)
  {  
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	if(TRUE == qhsusb_is_disconnected)
  {
		qhsusb_is_disconnected = FALSE;
  	*err_code = BULK_ERR_CABLE_DISCONNECT;
	/*Doesn't really matter how much was transferred, in anycase sahara has to begin 
	    again */
	tx_length = 0x0; 
	if ( tx_cb != NULL )
  	{
        tx_cb(tx_length, *err_code);
  	}
	qhsusb_fd_hdlc_shutdown();
    return tx_length;
  }
#endif
    qhsusb_al_bulk_poll();
  }
  tx_length = qhsusb_al_bulk_tx_urb->actual_length;
  if ( qhsusb_al_bulk_tx_urb->transfer_status == QHSUSB_URB_STATUS_COMPLETE_OK )
  {
    qhsusb_al_bulk_tx_urb = NULL;
  }
  else
  {
    qhsusb_log(QHSUSB_AL_BULK_ERR_WRITE_LOG,0,0);
    qhsusb_boot_log_message("USB: qhsusb_al_bulk_transmit, Error");
    qhsusb_al_bulk_tx_urb = NULL;
    *err_code = BULK_ERR_WRITE;
  }

  if ( tx_cb != NULL )
  {
    qhsusb_boot_log_message("USB: qhsusb_al_bulk_transmit, TX Complete");
    tx_cb(tx_length, *err_code);
  }
  return tx_length;
}

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_get_max_packet_size
 * 
 * @brief   This function returns whether USB spec defined max packet sizes
 *          for FS USB or HS USB
 *
 * @details Correct packet sizes are determined internally and will be
 *          returned to external interface
 *          
 * @return  Max packet sizes
 * 
 */
uint32 qhsusb_al_bulk_get_max_packet_size(void)
{
  return 512;
}

// ===========================================================================
/**
 * @function    qhsusb_al_bulk_get_max_raw_data_size
 * 
 * @brief   Returns the maximum bulk transfer level size that can be requested
 *          by the client
 *
 * @details Windows only support maximum bulk transfer size of 1MB, whereas
 *          Linux supports a larger maximum 
 *          
 * @return  Max USB bulk transfer level size
 * 
 */
uint32 qhsusb_al_bulk_get_max_raw_data_size(void)
{
  /* Maximum bulk transfer size */
  return QHSUSB_BULK_LAYER_MAX_TRANSFER_SIZE_IN_BYTES;
}
// ===========================================================================
/**
 * @function    qhsusb_al_bulk_set_zlp_mode
 * 
 * @brief   Sets the ZLP mode based on host type.
 *
 * @param 	[IN] Whether host supports ZLP
 *
 *          
 * @return  None
 * 
 */
 // ===========================================================================
 void qhsusb_al_bulk_set_zlp_mode(boolean host_sends_zlp)
 {
	if(host_sends_zlp)
	{
		qhsusb_zlp_mode = QHSUSB_SENDS_ZLP;
	}
	else
	{
		qhsusb_zlp_mode = QHSUSB_LACKS_ZLP;
	}
 }
