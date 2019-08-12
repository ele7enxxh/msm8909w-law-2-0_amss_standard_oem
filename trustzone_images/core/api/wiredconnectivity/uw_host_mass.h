/************************************************************************** 
*************************   Restricted access   *************************** 
*************************************************************************** 


This file must only be used for the development of the HSUSB 
driver for the AMSS / BREW SW baselines using the Jungo USB Stack.
This file must not be used in any way for the development of any
functionally equivalent USB driver not using the Jungo USB stack.

For any questions please contact: Sergio Kolor, Liron Manor,
Yoram Rimoni, Dedy Lansky.
*/


/**@file uw_host_mass.h

Contains the Host Mass Storage class driver.

Jungo Confidential, Copyright (c) 2008 Jungo Ltd. (http://www.jungo.com)
*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/uw_host_mass.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
    
  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  06/26/10  sw    (Tech Pubs) Edited/added Doxygen comments and markup.
    
==============================================================================*/


/************************************************************************** 
*************************   Restricted access   ************************** 
**************************************************************************/ 

/* Jungo Confidential, Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com 
**************************************************************************/

#ifndef _UW_HOST_MASS_H_
#define _UW_HOST_MASS_H_

#include "jtypes.h"


/** @ingroup hsusb_jungo_api 
@{ */

/** Mass storage media characteristics for usage of the upper layer.
*/    
struct media_info {
    juint32_t    sector_count;    /**< Sector count. */
    juint32_t    sector_size;     /**< Sector size. */
    juint8_t     medium_type;     /**< Medium type. */
    juint8_t     write_protect;   /**< Write protect. */
    juint8_t     heads;           /**< Media heads. */
    juint8_t     sectors;         /**< Media sectors. */
    juint32_t    cylinders;       /**< Media cylinders. */
};

/** Mass storage device information.
*/
struct device_info {
    juint8_t     subclass;    /**< Subclass of MS class supported. */
    juint8_t     devtype;     /**< Type of SCSI device (e.g, CDROM, HD, etc) */
    juint8_t     lun;         /**< Number of the logical unit. */
    juint8_t     *vendor;     /**< ASCII vendor ID. */
    juint8_t     *product;    /**< ASCII product ID. */
    juint16_t    release;     /**< Four ASCI hex digits describing product
							       release. */
    juint8_t     removable;   /**< Whether the device is removable. */
};

#define TO_DEVICE       0
#define FROM_DEVICE     1

jresult_t disk_sync_transfer(void *dev, juint8_t *buffer, juint8_t flags,
    juint32_t start_sector, juint32_t num_sectors, jint_t direction);

jresult_t disk_get_media_info(void *dev, struct media_info *pinfo);

jresult_t disk_get_device_info(void *dev, struct device_info *pinfo);

void *disk_get_usbd_handle(void *dev);

jresult_t disk_suspend(void *dev);
jresult_t disk_resume(void *dev);

/* Implemented by upper layer */

/** Callbacks for disk handling.
*/
struct disk_handle_cb {
    jresult_t (*media_insert)(void *dev, void **context);
      /**< Notify the application upon media insertion. */
    void (*media_remove)(void *dev, void *context);
      /** Notify application on media insertion. */

    jresult_t (*device_attach)(void *dev, void **context);
      /**< Optional callbacks; the application is notified when a device 
           is connected before any media exploring is performed. */
    void (*device_detach)(void *dev, void *context);
      /**< Notify the application upon device detachment. */
 };

/** OS/Filesystem must call this function opon initialization to let the
  disk layer know to whom to report disk operations.
 */
jresult_t register_disk_handler(struct disk_handle_cb *functions);
void unregister_disk_handler(void);

/** Number of threads used for an asynchronous operation.
*/
#define JMS_ASYNC_THREADS 2

/** Callback provided to an asynchronous transfer for completion notification.
*/
typedef void (*disk_async_cb)(void *arg, jresult_t status);

/** Asynchronous disk transfer initiator.
*/
jresult_t disk_async_transfer(void *dev, disk_async_cb callback, void *arg,
    juint8_t *buffer, juint8_t flags, 
    juint32_t start_sector, juint32_t num_sectors, juint_t direction);

/** @} */ /* end_group hsusb_jungo_api */

#endif
