#ifndef QHSUSB_URB_H
#define QHSUSB_URB_H
/*=======================================================================*//**
  @file         qhsusb_urb.h
 
  @brief        URB
 
  @details      
                Theory of operation 
                 
                URB stands for USB Request Buffer; it facilitate data 
                exchange between application and DCI. 
                 
                URB is allocated by application. Application provides also 
                transmit buffer. Application fills: 
                 - signature 
                 - transfer_buffer_ptr
                 - transfer_length
                 - ep
                 - is_tx
                 - complete_callback
                 - app_priv_ptr
                 
                Then, application calls qhsusb_submit_urb. This function 
                returns error code. If error occurs (returned is negative 
                value), submission failed and URB was not passed to DCI. If 
                success (0) is returned, from this moment, ownership over URB 
                passed to DCI and application can't access URB. 
                 
                Eventually, when DCI finished with this URB, it call URB's 
                complete() callback. At this moment, ownership over URB 
                returns to application. Application analyse transfer_status 
                to decide on next actions. DCI fills: 
                - transfer_status 
                - actual_length 
                - in case of Rx, buffer filled with data 
 
               Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
*//*========================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dcd/qhsusb_urb.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
struct qhsusb_dcd_dsc_device;

/**
 * USB Request buffer
 */
typedef struct qhsusb_urb {
   uint32 signature; /**< should match \a QHSUSB_URB_SIGNATURE */
#define QHSUSB_URB_SIGNATURE (0x42525551) // 'QURB'
   /********************* control *********************/
   //struct qhsusb_urb* next; /**< link URB's for the EP */
   /**
    * Transaction status.
    *  - DCD set to 0 prior to submission;
    *  - DCI modifies to ACTIVE when submited, and to
    *    COMPLETE/ERROR on completeion.
    *  - in complete(), DCD check status.
    */
   uint32 transfer_status;
#define QHSUSB_URB_STATUS_ACTIVE          (1)   /**< Transfer in progress */
#define QHSUSB_URB_STATUS_COMPLETE_OK     (2)   /**< Completed, success */
#define QHSUSB_URB_STATUS_COMPLETE_ERROR  (3)   /**< completed, error */

   struct qhsusb_dcd_dsc_device* usb_device;    /**< DCD context */
   uint32 core_id;                           /**< DCI context */
   /**
    * queued for this EP, 
    *  - bits 3:0 - EP number, 
    *  - bit 7 - IN(1)/OUT(0)
    */
   uint8 endpoint_address;
   uint8 reserved0[3]; // align to 32 bits
   /*************** data *****************/
   /**
    * Data buffer
    */
   void* transfer_buffer_ptr;
   /**
    * Length, bytes, of buffer
    */
   uint32 transfer_length;
   /**
    * Bytes user in buffer. Filled by DCI upon Rx
    */
   uint32 actual_length;
   /**
    * Flag to let FD layer to specify whether we want to transmit a ZLP 
    */
   uint8 send_zlp;
   
   uint8 reserved1[3]; // align to 32 bits
   /******************** application use ********************/
   /**
    * callback to be executed upon transmission completion.
    */
   void (*complete_callback)(struct qhsusb_urb* urb_ptr);

   /**
    * private data to be used by application / driver
    */
   void* app_priv_ptr; // high level
   void* dcd_priv_ptr;
   void* dci_priv_ptr; // low level 
   /**
    * Boolean variable which tells whether for zero address dTD chaining
	* is needed or not. 
	* Note - If this flag is set, for first TD, there will not be any Endpoint
	* priming.
    */
    boolean is_zero_address_chain_required;
} qhsusb_urb_t;


/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 * Submit URB
 * 
 * URB should be filled by upper layer as required
 * 
 * After this function, ownership over URB passed to
 * DCI and one should not access any fields. 
 *  
 * Non blocking. 
 * 
 * Submitter will regain ownership over URB when 
 * \a complete_callback called.
 * 
 * @param urb    Buffer to transfer. Can't be NULL.
 * 
 * @return - 0 if OK
 *         - negative for error
 */
int qhsusb_submit_urb(struct qhsusb_urb* urb);

#endif /*QHSUSB_URB_H*/

