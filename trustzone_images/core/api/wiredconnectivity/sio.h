#ifndef SIO_H
#define SIO_H

/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                       H E A D E R    F I L E

===========================================================================*/

/**
  @file sio.h 
  @brief 
      This file contains the definitions of the constants, data 
  structures, and interfaces that comprise the SIO API. 
  Only the interfaces declared shall be used by the client for
  accessing the SIO API. 
  
*/ 


/*
Copyright (c) 1990-1993, 1997-2010 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary

*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/sio.h_v   1.20   17 Apr 2003 17:19:50   chenc  $
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/sio.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/14/09   amv     Added SDIO MUX Rmnet ports
11/17/09   amv     Used processor specific SMD major number for Three way Diag ports
08/27/09   amv     Added MUX driver major and its ports
07/16/09   amv     Removed private file from public header (CMI #2)
06/24/09   amv     Assigned SMD_BRIDGE Minor numbers as SMD major numbers
06/23/09   amv     Reverted n-way diag ports to legacy n-way diag ports
06/23/09   amv     Added sio critical sections, smd bridge forward compatiblity 
                   and 8200 target specific changes
05/02/09   amv     Added SPS support in sio_open param
12/08/08   jas     (Per Brian Costello) Adding new union defs for the SMD
                   IOCTLs
11/25/08   amv     Added SIO IOCTLS for SMD 
09/18/08   vkm     SIO Defeaturization, Making SIO a DLL, Commonizing WM 6 and WM 7 SIO
09/12/08   amv     Added SMD ports for MODEM, APP and QDSP processor
07/30/07   rh      Created WM specific branch
10/11/06   rh      Adding support for FEATURE_HS_USB
09/25/06   rh      Add support for custom baud rate
10/24/06   bfc     Modified some enumarations that are used by the SMD
09/27/06   bfc     Added get/set drop pend ioctl (for SMD ports)
09/05/06   ptm     Add get/set buffer size IOCTL (for SMD ports).
07/31/06   sr      Add support for 3000kbps
07/27/06   rh      Add new sio port SIO_PORT_SMD_GPS_NMEA
07/05/06   bfc     Added some metacomments that are needed by smd_bridge
06/19/06   bt      Added FEATURE_USB_BREW_EXTENDED_DEVICE support. 
05/24/06   bfc     Added additional support for RI and CD signals
05/18/06   sr      Add support for 921.6kbps
05/10/06   rh      Adding a new flow control type SIO_CTSRFR_AUTO_FCTL
05/01/06   bfc     Added featurization for smd_bridge related code
04/25/06   ptm     Added SMD port for GPS NMEA and RPC reply.
03/28/06   bfc     Changed the arguement for extended callback function from
                   a uint32 to a void *
01/26/06   bfc     Added the DISABLE/ENABLE_CD_EVENT_EXT feature for SMD streams.
                   Also added sio_close_ext and sio_flush_tx_ext which work 
                   specifically with SMD streams.
01/04/06   ptm     Restore SMEM code, expanded SMD ports as required.
09/28/05   TMR     Added SIO_PORT_SMEMDS_BRIDGE_1 and other SMD definitions
08/19/05   cp      Support for APS virtual port
06/05/05   TMR     Merged in MSM7500 code
06/01/05   adm     Added support for additional SMD DIAG pipe SIO_PORT_SMEMDS_DIAG_RSP
04/14/05   adm     Added support for dataxfr_mode and xfrflow_type
                   attributes for SMD
03/08/05   cc      Merged in OSIFDev_Dec04 branck. 
01/11/05   cc      Duplicate USB port entries with old names in 
                   sio_port_id_type for better backwards compatibility.
12/23/04   cc      Renamed SIO_PORT_USB_CDC_ACM, _DIAG, _NMEA to be 
                   SIO_PORT_USB_MDM,_SER1 and _SER2 respectively. 
11/24/04   cc      Added SIO_PORT_USB_RRDM and SIO_PORT_USB_NET_WWAN support. 
                   Added sio_control_open/close/transmit() APIs. 
11/09/04   adm     Modified sio_get_stream_id to return fixed stream_is'd
                   in case of SMEMDS related ports 
12/09/04   cc      Define SIO_PAYLOAD define to be DSM_PS_RAW_APP_PAYLOAD for 
                   WCDMA_PLT and GSM_PLT.
10/20/04   adm     Added support for multiple DS pipes. 
12/02/03   cc      Changed SIO_PAYLOAD define to be DSM_PS_RAW_APP_PAYLOAD
                   Or PS_RAW_APP_PAYLOAD to sync with new DSM. 
11/24/03   cc      Added FEATURE_JCDMA_SIO and OR it with FEATURE_JCDMA_DS.                     
09/28/03   dsp     Added BT SPP status support.
02/21/03   cc      Added FEATURE_USB_DIAG_NMEA support. 
12/03/02   wal     Added FEATURE_BT_SPP.  Removed 
                   FEATURE_BT_SIO_API_ENHANCEMENT.
11/22/02   ltan    Replaced FEATURE_MMGPS with FEATURE_MMGPS_CSD 
07/15/02   ry      Added support for the LSM_CSD port
07/15/02   ak      Added FEATURE_DATA_MM support.
02/21/02   ry    Added the support for FEATURE_BT_SIO_API_ENHANCEMENT. 
02/06/02   cc      Add third UART support. 
11/07/01   cc      Move #define SIO_PAYLOAD from siors232.c to sio.h
10/16/01   cc      Merge in Mods of Simba from MSM5200_SIO.00.00.17, includes 
                   following 1 item
05/01/01   dwp     Add ioctl command SIO_IOCTL_GET_CURRENT_RTS and support it
                   with rts_asserted in sio_ioctl_param_type.  
06/18/01   dwp     Add support for FEATURE_UART_RATE1.
06/07/01   dwp     Modify sio_port_id_type such that when a minimal DMSS build
                   is made, only three entries will be enumerated.
06/01/01   dwp     Removed dependencies on DS.
03/26/01   dl      Add support for SMEM_DS port.
02/19/01   dwp     Reordered sio_bitrate_type to coincide w/new
                   nv_sio_baudrate_type.
02/13/01   dwp     Removed sizing of sio_bitrate_type.
01/26/01   ych     Merged T-53/JCDMA features for SK.
11/27/00   dwp     Change multiple SIO_PORT_BT_RFCOMM's to SIO_PORT_BT_SPP.
                   Add SIO_PORT_BT_NA. Remove SIO_IOCTL_BT_ENABLE and
                   SIO_IOCTL_BT_DISABLE.
11/18/00   dwp     Added additional requirement of FEATURE_USB on port 
                   definition SIO_PORT_USB_MMC.
11/14/00   dwp     Added support for FEATURE_MMC which is used in support of
                   FEATURE_RUNTIME_DEVMAP.
09/05/00   dwp     Additional FEATURE_BT modifications.
07/27/00   dwp     Added support for FEATURE_USB_DIAG and FEATURE_BT.
07/20/00   jwh     Added support for SIO_IOCTL_GET_FLOW_CTL and
                   FEATURE_SIO_INT_LOCKOUT.
06/08/00   jwh     Update comments.
05/22/00   jwh     Changed sio_bitrate_type and sio_port_id_type enum
                   featurization to allow compile time checking and keep
                   numeric values consistent for NV. Add sio_is_baud_valid().
04/15/00   jwh     Add SIO_IOCTL_CHANGE_BAUD_NOW, clarify operation of 
                   SIO_IOCTL_CHANGE_BAUD_RATE.
04/11/00   jwh     Make SIO_BITRATE_230400 defn unconditional.
02/16/00   jwh     Removed support for OLD SIO.  Added support for 230,400
                   baud for MSM5000.
09/09/99   jwh     Changes to NEW SIO API: sio_port_id_type name changes and
                   now #ifdef'd with FEATUREs; deleted sio_priority_type and
                   sio_bandwidth_type and the four associated (unused) 
                   members of sio_open_type; added SIO_IOCTL_FLUSH_TX.  Also
                   improved some comments.  Merged with MSM3100 1.0C.
08/05/99   mk      Added support for old SIO if T_MSM31 defined.
04/25/99   ak      Added ioctl def's to support mode change and baudrate 
                   change via an ioctl, versus using open/close.
04/09/99   jwh     Made sio_hdr_type typedef PACKED to avoid ARM 2.5 error.
02/17/99   ak      Added extern for sio_tx_flush.
01/17/99   ak      Featurized file (after INCLUDE FILES section), based on
                   whether user wants old or new SIO.  (FEATURE_NEW_SIO)
06/08/98   jjn     Exports new partitioned SIO.
10/29/97   jjn     Implemented DM Enter and DM Exit
10/10/97   grl     Allowed customerization for DM
09/09/97   ldg     (jjn) Changed and renamed flow control functions.
06/16/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/20/97   jjn     Implemented 617 transparency in SIO
05/24/96   jjw     Gemini & ISS2 Data Services release.
04/23/96   rdh     Added support for 115.2K DM.
11/09/95   jjw     Completed changes to support fully compliant IS-99
08/18/95   jmk     Took out T_SP conditional compile.
08/02/95   jmk     Added T_SP conditional compilation (exclude rex, protos, etc)
07/21/95   jjw     Fixed boo-boo within #ifdef DS definitions
07/13/95   jjw     Changes to support IS-99 protocol integration
03/23/95   rdh     Added ISS1 to sio_baud_clock_ctl prototype.
01/22/95   jjw     Added 2nd generation definitions and function prototypes
05/25/94   jjw     Made change to Ring buffer type parameter
05/18/94   jjw     Added changes for DM multi-drop
08/19/93   jjw     Modfied some existing function names, added all Data 
                   services functionality
12/07/93   jah     Added prototype for sio_baud_clock_ctl().
06/07/93   jjw     Made code review changes, added some more changes for Beta
                   II initial functionality
05/14/93   jjw     Added T_B2 compile-time directive for Beta II target
05/11/93   jjw     Cleaned up for code review, phase 1
03/12/93   jjw     Added typedef for Async. SIO channel A usage.
01/28/92   jah     Improved comments before passing this module to Jim Willkie.
08/25/92   jah     Improved error logging to differentiate errors more clearly.
07/23/92   jah     Added comments.
06/22/92   jah     Ported from brassboard, removed async.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.                 */
#include "queue.h"      /* Queue declaration package.                       */
#include "dsm.h"        /* Data service buffer definition.                  */





/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/** This macro encapsulates the major and minor number and minor number 
    of device and returns device_id/port_id. */
#define DEVICE_ID(major, minor) ((major << 8) | (minor & 0x00ff))

/** This macro retrieves the major number from device id/port id.*/
#define SIO_MAJOR_NUM_OF_DEVICE(device_id) (((device_id & 0xff00) >> 8 ))

/** This macro retrieve the minor number from device id/port id. */
#define SIO_MINOR_NUM_OF_DEVICE(device_id) ((device_id & 0x00ff))

/*---------------------------------------------------------------------------*/
/** Identifies SIO flow control types for devices. It is used to configure 
    flow control for SIO stream. */
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_FCTL_BEST = 0,                   /**< -- Best Flow control method.  Can
                                                mean default or no change. */
  SIO_FCTL_OFF,                        /**< -- Flow control disabled */
  SIO_XONXOFF_STRIP_FCTL_FS,           /**< -- Use fail-safe XON/XOFF flow 
                                                control but strip XON/XOFF 
                                                characters from stream */
  SIO_XONXOFF_STRIP_FCTL_NFS,          /**< -- Use non-failsafe XON/XOFF flow
                                                control but strip XON/XOFF 
                                                characters from stream  */
  SIO_CTSRFR_FCTL,                     /**< -- Use CTS/RFR flow control*/
  SIO_XONXOFF_NSTRIP_FCTL_FS,          /**< -- Use fail-safe XON/XOFF flow 
                                                control and leave in stream */
  SIO_XONXOFF_NSTRIP_FCTL_NFS,         /**< -- Use non-failsafe XON/XOFF flow 
                                                control and leave in stream */                                        
  SIO_MAX_FLOW,                        /**< -- For bounds checking only  */
  SIO_CTSRFR_AUTO_FCTL                 /**< -- Use CTS/RFR flow control with
                                                auto RX RFR signal generation  */
} sio_flow_ctl_type;

/*---------------------------------------------------------------------------*/
/** SIO Stream Identifier type.  */
/*---------------------------------------------------------------------------*/
typedef int2 sio_stream_id_type;

/*---------------------------------------------------------------------------*/
/** Identifier for stream which is opened. */
/*---------------------------------------------------------------------------*/
typedef int2 stream_id_type;


/*---------------------------------------------------------------------------*/
/** SIO_NO_STREAM_ID is returned when sio_open or sio_control_open fails. */
/*---------------------------------------------------------------------------*/
#define SIO_NO_STREAM_ID -1


/*---------------------------------------------------------------------------*/
/** Identifies SIO Stream mode type. */
/*---------------------------------------------------------------------------*/
typedef enum    
{
  SIO_DS_AUTODETECT_MODE,          /**< -- AutoDetect Mode (Backwards comp.)    */
  SIO_DS_RAWDATA_MODE,             /**< -- RawData Mode (Backwards compatible)  */
  SIO_DS_PKT_MODE,                 /**< -- Packet mode                          */
  SIO_DM_MODE,                     /**< -- DM mode                              */
  SIO_GENERIC_MODE,                /**< -- Generic Mode.  Most streams should 
                                            use this mode.                      */
  SIO_MAX_MODE                     /**< -- Maximum Mode Value.                  */
} sio_stream_mode_type;

/*---------------------------------------------------------------------------*/
/** Identifies the SMD data transfer mode type.*/
/*---------------------------------------------------------------------------*/
typedef enum    
{
  SIO_SMD_MEMCPY_MODE,          /**< -- SMD memory copy mode */
  SIO_SMD_DM_MODE,              /**< -- SMD Data mover mode */
  SIO_SMD_INVALID_MODE          /**< -- SMD invalid stream mode */
} sio_dataxfr_mode_type;

/*---------------------------------------------------------------------------*/
/** Identifies the SMD transfer flow type.*/
/*---------------------------------------------------------------------------*/
typedef enum    
{
  SIO_SMD_STREAMING_TYPE,        /**< -- SMD stream transfer flow type */
  SIO_SMD_PKTXFR_TYPE,           /**< -- SMD packet transfer flow type */ 
  SIO_SMD_INVALID_TYPE           /**< -- SMD invalid transfer flow type */
} sio_xfrflow_type;

/*---------------------------------------------------------------------------*/
/** Identifies SIO bitrate type.  The unit of measure is bits per second.
 The "ILLEGAL" entries are to retain compile time range checking 
 while keeping the actual values invariant for NVRAM consistency.*/
/*---------------------------------------------------------------------------*/
/* !!!! Must keep siors232.c:sio_is_baud_valid() in sync with this. !!!! */
typedef enum     
{
  SIO_BITRATE_ILLEGAL_1,            /**<-- Illegal bit-rate 1*/
  SIO_BITRATE_ILLEGAL_3,            /**<-- Illegal bit-rate 2*/
  SIO_BITRATE_ILLEGAL_4,            /**<-- Illegal bit-rate 4*/
  SIO_BITRATE_ILLEGAL_5,            /**<-- Illegal bit-rate 5*/
  SIO_BITRATE_ILLEGAL_6,            /**<-- Illegal bit-rate 6*/
  SIO_BITRATE_ILLEGAL_7,            /**<-- Illegal bit-rate 7*/
  SIO_BITRATE_300,                  /**<-- 300  bit-rate     */
  SIO_BITRATE_600,                  /**<-- 600  bit-rate     */
  SIO_BITRATE_1200,                 /**<-- 1200 bit-rate     */
  SIO_BITRATE_2400,                 /**<-- 2400 bit-rate     */
  SIO_BITRATE_4800,                 /**<-- 4800 bit-rate     */
  SIO_BITRATE_9600,                 /**<-- 9600 bit-rate     */
  SIO_BITRATE_14400,                /**<-- 14400 bit-rate    */
  SIO_BITRATE_ILLEGAL_8 = SIO_BITRATE_14400,
  SIO_BITRATE_19200,                /**<-- 19200  bit-rate   */
  SIO_BITRATE_38400,                /**<-- 38400  bit-rate   */
  SIO_BITRATE_57600,                /**<-- 57600  bit-rate   */
  SIO_BITRATE_76800,                /**<-- 76800  bit-rate   */
  SIO_BITRATE_ILLEGAL_9 = SIO_BITRATE_76800,
  SIO_BITRATE_115200,               /**<-- 115200 bit-rate   */
  SIO_BITRATE_230400,               /**<-- 230400 bit-rate   */
  SIO_BITRATE_ILLEGAL_2 = SIO_BITRATE_230400,
  SIO_BITRATE_460800,               /**<-- 460800 bit-rate   */
  SIO_BITRATE_ILLEGAL_10 = SIO_BITRATE_460800,
  SIO_BITRATE_921600,               /**<-- 921600 bit-rate   */
  SIO_BITRATE_2900000,              /**<-- 2900000 bit-rate  */
  SIO_BITRATE_3200000,              /**<-- 3200000 bit-rate  */
  SIO_BITRATE_3686400,              /**<-- 3686400 bit-rate  */
  SIO_BITRATE_4000000,              /**<-- 4000000 bit-rate  */
  SIO_BITRATE_HS_CUSTOM,            /**<-- HS custom bit-rate*/      
  SIO_BITRATE_ILLEGAL_11 = SIO_BITRATE_HS_CUSTOM,
  SIO_BITRATE_BEST = 0x7FFE,  /**< -- Best bitrate (default, fastest, etc) */
  SIO_BITRATE_MAX = 0x7FFF    /**< -- For bounds checking only             */
} sio_bitrate_type;



/*---------------------------------------------------------------------------*/
/** Maximum number of streams allowed.*/
/*---------------------------------------------------------------------------*/
#define SIO_MAX_STREAM 64

/*---------------------------------------------------------------------------*/
/** This symbol defined for backwards compatibility when the SMD
  names were changed. */
/*---------------------------------------------------------------------------*/
/*It should be deleted when the references  are removed.*/
#define SIO_PORT_SMEMDS_DS SIO_PORT_SMD_DS

/*---------------------------------------------------------------------------*/
/**
@name SIO Major numbers
  The following constants are grouped as SIO major numbers. Sio major numbers
  are assigned to each unique driver-interface.
  @{
*/
/*---------------------------------------------------------------------------*/
 
#define SIO_MAJOR_LEGACY        0       /**< SIO Major for Legacy device driver */ 
#define SIO_MAJOR_UART          1       /**< SIO Major for UART driver */ 
#define SIO_MAJOR_USB           2       /**< SIO Major for USB driver */
#define SIO_MAJOR_HSUSB_ECM     3       /**< SIO Major for HS-USB-ECM driver */
#define SIO_MAJOR_SMD           4       /**< SIO Major for SMD DRIVER */
#define SIO_MAJOR_SMD_BRIDGE    5       /**< SIO Major for SMD-BRIDGE driver */
#define SIO_MAJOR_SMEM          6       /**< SIO Major for SMEM driver */
#define SIO_MAJOR_BLUETOOTH     7       /**< SIO Major for BT driver */
#define SIO_MAJOR_MMGPS         8       /**< SIO Major for MMGPS driver */
#define SIO_MAJOR_APS           9       /**< SIO Major for APS driver */
#define SIO_MAJOR_SMD_BRIDGE_NPROC 10   /**< SIO Major for SMD Bridge driver for N-proc */
#define SIO_MAJOR_SMD_TO_APPS   11      /**< SIO Major for SMD Bridge to Apps*/
#define SIO_MAJOR_SMD_TO_MODEM  12      /**< SIO Major for SMD Bridge to Modem */
#define SIO_MAJOR_SMD_TO_QDSP6  13      /**< SIO Major for SMD Bridge to QDSP6 */
#define SIO_MAJOR_A2            14      /**< SIO Major for A2 driver */
#define SIO_MAJOR_DATA_MUX      15      /**< SIO Major for MUX driver  */
#define SIO_MAJOR_HSUSB_EEM     16      /**< SIO Major for HSUSB EEM driver */
#define SIO_MAJOR_ONDEVICE_DIAG 17      /**< SIO Major for Diag OnDevice driver  */
#define SIO_MAJOR_SDIOC         18      /**< SIO Major for SDIO client driver        */
#define SIO_NULL_DRIVER         19      /**< SIO Major for NULL driver */
/** 
   @}
*/


/*---------------------------------------------------------------------------*/
/**
@name SIO Minor numbers
  The following constants are grouped as SIO Minor numbers. 
  SIO minor number indemnifies the logical device for device driver.
  Maximum minor numbers allowed per major number is 64.
  @{
*/
/*---------------------------------------------------------------------------*/

/* SIO_LEGACY_MINOR */
#define SIO_MINOR_LEGACY_NULL   0       /**< SIO Minor for NULL Driver */
 
/* UART DRIVER */
#define SIO_MINOR_UART_1        1       /**< SIO Minor for UART 1 */
#define SIO_MINOR_UART_2        2       /**< SIO Minor for UART 2 */
#define SIO_MINOR_UART_3        3       /**< SIO Minor for UART 3 */
#define SIO_PORT_MAX            64

/* GENERIC (FS/HS) USB DRIVER */
#define SIO_MINOR_USB_MDM       1       /**< SIO Minor for USB MDM device  */
#define SIO_MINOR_USB_SER1      2       /**< SIO Minor for USB SER1 device */
#define SIO_MINOR_USB_SER2      3       /**< SIO Minor for USB SER2 device */
#define SIO_MINOR_USB_SER3      4       /**< SIO Minor for USB SER3 device */
#define SIO_MINOR_USB_MMC       5       /**< SIO Minor for USB MMC device  */
#define SIO_MINOR_USB_RRDM      6       /**< SIO Minor for USB RRDM Device */
#define SIO_MINOR_USB_NET_WWAN  7       /**< SIO Minor for USB WWAN/RMNET1 device */
#define SIO_MINOR_USB_RMNET2    8       /**< SIO Minor for USB RMENT2 device */
#define SIO_MINOR_USB_RMNET3    9       /**< SIO Minor for USB RMNET3 device */
#define SIO_MINOR_USB_BREW_X_DATA 10    /**< SIO Minor for BREW_X_DATA device */
#define SIO_MINOR_USB_BREW_X_NOTIFICATION 11    /**< SIO Minor for USB Brew notification device */
#define SIO_MINOR_USB_EEM       12      /**< SIO Minor for USB EEM device */

/* FS-USB DRIVER */
#define SIO_MINOR_FSUSB_MDM       SIO_MINOR_USB_MDM
#define SIO_MINOR_FSUSB_SER1      SIO_MINOR_USB_SER1
#define SIO_MINOR_FSUSB_SER2      SIO_MINOR_USB_SER2
#define SIO_MINOR_FSUSB_SER3      SIO_MINOR_USB_SER3
#define SIO_MINOR_FSUSB_MMC       SIO_MINOR_USB_MMC
#define SIO_MINOR_FSUSB_RRDM      SIO_MINOR_USB_RRDM
#define SIO_MINOR_FSUSB_NET_WWAN  SIO_MINOR_USB_NET_WWAN
#define SIO_MINOR_FSUSB_RMNET2    SIO_MINOR_USB_RMNET2
#define SIO_MINOR_FSUSB_RMNET3    SIO_MINOR_USB_RMNET3

/* HS-USB DRIVER */

#define SIO_MINOR_HSUSB_MDM       SIO_MINOR_USB_MDM
#define SIO_MINOR_HSUSB_SER1      SIO_MINOR_USB_SER1
#define SIO_MINOR_HSUSB_SER2      SIO_MINOR_USB_SER2
#define SIO_MINOR_HSUSB_SER3      SIO_MINOR_USB_SER3
#define SIO_MINOR_HSUSB_MMC       SIO_MINOR_USB_MMC
#define SIO_MINOR_HSUSB_RRDM      SIO_MINOR_USB_RRDM
#define SIO_MINOR_HSUSB_NET_WWAN  SIO_MINOR_USB_NET_WWAN
#define SIO_MINOR_HSUSB_RMNET2    SIO_MINOR_USB_RMNET2
#define SIO_MINOR_HSUSB_RMNET3    SIO_MINOR_USB_RMNET3
#define SIO_MINOR_HSUSB_EEM       SIO_MINOR_USB_EEM

/* HS-USB-LCM DRIVER */

#define SIO_MINOR_HSUSB_ECM_NET_WWAN    SIO_MINOR_USB_NET_WWAN

/* SMD DRIVER */

#define SIO_MINOR_SMD_FIRST             1   /* used for registration loop */
#define SIO_MINOR_SMD_DS                1   /**< SIO Minor for SMD DS device*/
#define SIO_MINOR_SMD_DIAG              2   /**< SIO Minor for SMD Diag device*/
#define SIO_MINOR_SMD_DIAG_APPS         3   /**< SIO Minor for SMD Diag-to-apps device*/
#define SIO_MINOR_SMD_DIAG_MODEM        4   /**< SIO Minor for SMD Diag-to-modem device*/
#define SIO_MINOR_SMD_DIAG_QDSP         5   /**< SIO Minor for SMD Diag-to-Qdsp6 device*/
#define SIO_MINOR_SMD_RPC_CALL          6   /**< SIO Minor for SMD RPC device*/
#define SIO_MINOR_SMD_RPC_REPLY         7   /**< SIO Minor for SMD RPC reply device*/    
#define SIO_MINOR_SMD_BT                8   /**< SIO Minor for SMD Bluetooth device*/
#define SIO_MINOR_SMD_CONTROL           9   /**< SIO Minor for SMD Control device*/
#define SIO_MINOR_SMD_MEMCPY_SPARE1     10  /**< SIO Minor for SMD Memcopy spare 1 device*/
#define SIO_MINOR_SMD_DATA1             11  /**< SIO Minor for SMD data1 device*/
#define SIO_MINOR_SMD_DATA2             12  /**< SIO Minor for SMD data2 device*/
#define SIO_MINOR_SMD_DATA3             13  /**< SIO Minor for SMD data3 device*/
#define SIO_MINOR_SMD_DATA4             14  /**< SIO Minor for SMD data4 device*/
#define SIO_MINOR_SMD_DATA5             15  /**< SIO Minor for SMD data5 device*/
#define SIO_MINOR_SMD_DATA6             16  /**< SIO Minor for SMD data6 device*/
#define SIO_MINOR_SMD_DATA7             17  /**< SIO Minor for SMD data7 device*/
#define SIO_MINOR_SMD_DATA8             18  /**< SIO Minor for SMD data8 device*/
#define SIO_MINOR_SMD_DATA9             19  /**< SIO Minor for SMD data9 device*/    
#define SIO_MINOR_SMD_DATA10            20  /**< SIO Minor for SMD data10 device*/
#define SIO_MINOR_SMD_DATA11            21  /**< SIO Minor for SMD data11 device*/
#define SIO_MINOR_SMD_DATA12            22  /**< SIO Minor for SMD data12 device*/
#define SIO_MINOR_SMD_DATA13            23  /**< SIO Minor for SMD data13 device*/
#define SIO_MINOR_SMD_DATA14            24  /**< SIO Minor for SMD data14 device*/
#define SIO_MINOR_SMD_DATA15            25  /**< SIO Minor for SMD data15 device*/
#define SIO_MINOR_SMD_DATA16            26  /**< SIO Minor for SMD data16 device*/
#define SIO_MINOR_SMD_DATA17            27  /**< SIO Minor for SMD data17 device*/
#define SIO_MINOR_SMD_DATA18            28  /**< SIO Minor for SMD data18 device*/
#define SIO_MINOR_SMD_DATA19            29  /**< SIO Minor for SMD data19 device*/
#define SIO_MINOR_SMD_DATA20            30  /**< SIO Minor for SMD data20 device*/
#define SIO_MINOR_SMD_DATA21            31  /**< SIO Minor for SMD data21 device*/    
#define SIO_MINOR_SMD_DATA22            32  /**< SIO Minor for SMD data22 device*/    
#define SIO_MINOR_SMD_DATA23            33  /**< SIO Minor for SMD data23 device*/
#define SIO_MINOR_SMD_DATA24            34  /**< SIO Minor for SMD data24 device*/
#define SIO_MINOR_SMD_DATA25            35  /**< SIO Minor for SMD data25 device*/
#define SIO_MINOR_SMD_DATA26            36  /**< SIO Minor for SMD data26 device*/
#define SIO_MINOR_SMD_DATA27            37  /**< SIO Minor for SMD data27 device*/
#define SIO_MINOR_SMD_DATA28            38  /**< SIO Minor for SMD data28 device*/
#define SIO_MINOR_SMD_DATA29            39  /**< SIO Minor for SMD data29 device*/
#define SIO_MINOR_SMD_DATA30            40  /**< SIO Minor for SMD data30 device*/
#define SIO_MINOR_SMD_DATA31            41  /**< SIO Minor for SMD data31 device*/
#define SIO_MINOR_SMD_DATA32            42  /**< SIO Minor for SMD data32 device*/
#define SIO_MINOR_SMD_DATA33            43  /**< SIO Minor for SMD data33 device*/
#define SIO_MINOR_SMD_DATA34            44  /**< SIO Minor for SMD data34 device*/
#define SIO_MINOR_SMD_DATA35            45  /**< SIO Minor for SMD data35 device*/
#define SIO_MINOR_SMD_DATA36            46  /**< SIO Minor for SMD data36 device*/
#define SIO_MINOR_SMD_DATA37            47  /**< SIO Minor for SMD data37 device*/
#define SIO_MINOR_SMD_DATA38            48  /**< SIO Minor for SMD data38 device*/
#define SIO_MINOR_SMD_DATA39            49  /**< SIO Minor for SMD data39 device*/
#define SIO_MINOR_SMD_DATA40            50  /**< SIO Minor for SMD data40 device*/
#define SIO_MINOR_SMD_GPS_NMEA          51  /**< SIO Minor for SMD GPS NMEA device*/
#define SIO_MINOR_SMD_MAX               51  /* used for registration loop */

/* SMD-BRIDGE DRIVER */

#define SIO_MINOR_SMD_BRIDGE_FIRST      1   /* used for registration loop */
#define SIO_MINOR_SMD_BRIDGE_LEGACY     0   /**< SIO Minor for SMD Bridge device legacy*/
#define SIO_MINOR_SMD_BRIDGE_1          1   /**< SIO Minor for SMD Bridge device 1*/
#define SIO_MINOR_SMD_BRIDGE_2          2   /**< SIO Minor for SMD Bridge device 2*/
#define SIO_MINOR_SMD_BRIDGE_3          3   /**< SIO Minor for SMD Bridge device 3*/
#define SIO_MINOR_SMD_BRIDGE_4          4   /**< SIO Minor for SMD Bridge device 4*/
#define SIO_MINOR_SMD_BRIDGE_5          5   /**< SIO Minor for SMD Bridge device 5*/
#define SIO_MINOR_SMD_BRIDGE_MAX        5   /* used for registration loop */

/* SMD-BRIDGE DRIVER NEW */
/** Use SIO_MINOR_SMD_BRIDGE_LEGACY for legacy dual proc target's driver */
/** SIO Minor for SMD Bridge to application processor */
#define SIO_MINOR_SMD_BRIDGE_TO_APPS    SIO_MAJOR_SMD_TO_APPS
/** SIO Minor for SMD Bridge to modem processor */
#define SIO_MINOR_SMD_BRIDGE_TO_MODEM   SIO_MAJOR_SMD_TO_MODEM
/** SIO Minor for SMD Bridge to qdsp6 processor */
#define SIO_MINOR_SMD_BRIDGE_TO_QDSP6   SIO_MAJOR_SMD_TO_QDSP6

/* SMEM DRIVER */

#define SIO_MINOR_SMEM_DS               1   /**< SIO Minor for SMEM DS device*/
        
/* BT DRIVER */

#define SIO_MINOR_BLUETOOTH_SPP         1   /**< SIO Minor for Bluetooth SPP device*/ 
#define SIO_MINOR_BLUETOOTH_NA          2   /**< SIO Minor for Bluetooth NA device*/ 

/* MMGPS DRIVER */

#define SIO_MINOR_MMGPS_LSM_CSD         1   /**< SIO Minor for MMGPS LSM CSD device*/ 

/* APS DRIVER */

#define SIO_MINOR_APS_DEV               1   /**< SIO Minor for APS_DEV device*/ 

/* A2 DRIVER */

#define SIO_MINOR_A2_MDM                0   /**< SIO Minor for A2 MDM device*/ 
#define SIO_MINOR_A2_RMNET_1            1   /**< SIO Minor for A2 RMNET1 device*/ 
#define SIO_MINOR_A2_RMNET_2            2   /**< SIO Minor for A2 RMNET2 device*/ 
#define SIO_MINOR_A2_RMNET_3            3   /**< SIO Minor for A2 RMNET3 device*/ 
/* For SDIO-A2 MUX  */
#define SIO_MINOR_SDIO_MUX_A2_RMNET_0   4   /**< SIO Minor for A2 MUX RMNET0 device*/ 
#define SIO_MINOR_SDIO_MUX_A2_RMNET_1   5   /**< SIO Minor for A2 MUX RMNET1 device*/ 
#define SIO_MINOR_SDIO_MUX_A2_RMNET_2   6   /**< SIO Minor for A2 MUX RMNET2 device*/ 
#define SIO_MINOR_SDIO_MUX_A2_RMNET_3   7   /**< SIO Minor for A2 MUX RMNET3 device*/ 
#define SIO_MINOR_SDIO_MUX_A2_RMNET_4   8   /**< SIO Minor for A2 MUX RMNET4 device*/ 
#define SIO_MINOR_SDIO_MUX_A2_RMNET_5   9   /**< SIO Minor for A2 MUX RMNET5 device*/ 
#define SIO_MINOR_SDIO_MUX_A2_RMNET_6   10  /**< SIO Minor for A2 MUX RMNET6 device*/ 
#define SIO_MINOR_SDIO_MUX_A2_RMNET_7   11  /**< SIO Minor for A2 MUX RMNET7 device*/ 

/* DATA MUX Driver */

#define SIO_MINOR_DATA_MUX_1             1  /**< SIO Minor for Data MUX1  device*/
#define SIO_MINOR_DATA_MUX_2             2  /**< SIO Minor for Data MUX2  device*/
#define SIO_MINOR_DATA_MUX_3             3  /**< SIO Minor for Data MUX3  device*/
#define SIO_MINOR_DATA_MUX_4             4  /**< SIO Minor for Data MUX4  device*/
#define SIO_MINOR_DATA_MUX_5             5  /**< SIO Minor for Data MUX5  device*/
#define SIO_MINOR_DATA_MUX_6             6  /**< SIO Minor for Data MUX6  device*/
#define SIO_MINOR_DATA_MUX_7             7  /**< SIO Minor for Data MUX7  device*/
#define SIO_MINOR_DATA_MUX_8             8  /**< SIO Minor for Data MUX8  device*/
#define SIO_MINOR_DATA_MUX_9             9  /**< SIO Minor for Data MUX9  device*/
#define SIO_MINOR_DATA_MUX_10            10 /**< SIO Minor for Data MUX10 device*/
#define SIO_MINOR_DATA_MUX_11            11 /**< SIO Minor for Data MUX11 device*/
#define SIO_MINOR_DATA_MUX_12            12 /**< SIO Minor for Data MUX12 device*/
#define SIO_MINOR_DATA_MUX_13            13 /**< SIO Minor for Data MUX13 device*/
#define SIO_MINOR_DATA_MUX_14            14 /**< SIO Minor for Data MUX14 device*/
#define SIO_MINOR_DATA_MUX_15            15 /**< SIO Minor for Data MUX15 device*/
#define SIO_MINOR_DATA_MUX_16            16 /**< SIO Minor for Data MUX16 device*/
#define SIO_MINOR_DATA_MUX_MAX           16 /* Used for registration */

/* ONDEVICE Diag driver */

#define SIO_MINOR_ONDEVICE_DIAG          1 /**< SIO Minor for Ondevice diag device*/

/* SDIOC client driver */

#define SIO_MINOR_SDIOC_RPC              1  /**< SIO Minor for SDIOC RPC device*/  
#define SIO_MINOR_SDIOC_WWAN             2  /**< SIO Minor for SDIOC WWAN device*/  
#define SIO_MINOR_SDIOC_DIAG             3  /**< SIO Minor for SDIOC DIAG device*/  

/** 
   @}
*/

#define SIO_MAC_ADDR_STR_SIZE 13        

/*---------------------------------------------------------------------------*/
/** sio_port_id_type is used to identify various SIO devices.
    Sio port id is 16 bit number, which comprises SIO_MAJOR_NUMBER and
    SIO_MINOR_NUMBER. (SIO_PORT_ID = DEVICE_ID(SIO_MAJOR_NUMBER,
    SIO_MINOR_NUMBER). Upper byte indicates the major number while lower byte
    indicates the minor number. Clients can use SIO port id to communicate with
    given logical port of a particular driver, where logical port is identified
    by minor number and the driver is identified by major number. SIO major
    number is assigned to each unique driver interface. Each Major number can
    support certain number of minor numbers (logical ports), maximum minor
    number is limited to 64. In other words, drivers can support as many as 64
    logical ports with one major number.  */
/*---------------------------------------------------------------------------*/

typedef enum    
{
    /** Non-existent/NULL SIO Port.*/
  SIO_PORT_NULL = DEVICE_ID(SIO_MAJOR_LEGACY, SIO_MINOR_LEGACY_NULL ), 

  /*--------------------- UART DRIVER --------------------- */

    /** -- SIO Port id for UART 1/main device */
  SIO_PORT_UART_MAIN = DEVICE_ID(SIO_MAJOR_UART , SIO_MINOR_UART_1 ),
    /** -- SIO Port id for UART 2/AUX device */
  SIO_PORT_UART_AUX = DEVICE_ID(SIO_MAJOR_UART , SIO_MINOR_UART_2 ), 
    /** -- SIO Port id for UART 3 device */
  SIO_PORT_UART_THIRD = DEVICE_ID(SIO_MAJOR_UART , SIO_MINOR_UART_3 ), 

  /*---------------------  USB DRIVER --------------------- */

    /** -- SIO Port id for USB MDM device */
  SIO_PORT_USB_MDM = DEVICE_ID(SIO_MAJOR_USB , SIO_MINOR_FSUSB_MDM ), 
    /** -- SIO Port id for USB CDC ACM device */
  SIO_PORT_USB_CDC_ACM = SIO_PORT_USB_MDM,
    /** -- SIO Port id for USB serial 1 device */
  SIO_PORT_USB_SER1 = DEVICE_ID(SIO_MAJOR_USB , SIO_MINOR_USB_SER1 ),
    /** -- SIO Port id for USB Diag device */
  SIO_PORT_USB_DIAG = SIO_PORT_USB_SER1,
    /** -- SIO Port id for USB MMC device */
  SIO_PORT_USB_MMC = DEVICE_ID(SIO_MAJOR_USB , SIO_MINOR_USB_MMC ), 
    /** -- SIO Port id for USB serial 2 device */
  SIO_PORT_USB_SER2 = DEVICE_ID(SIO_MAJOR_USB , SIO_MINOR_USB_SER2 ),
    /** -- SIO Port id for USB NMEA device */
  SIO_PORT_USB_NMEA = SIO_PORT_USB_SER2,
    /** -- SIO Port id for USB RRDM device */
  SIO_PORT_USB_RRDM = DEVICE_ID(SIO_MAJOR_USB , SIO_MINOR_USB_RRDM ),
    /** -- SIO Port id for USB NET WWAN/RMNET1 device */
  SIO_PORT_USB_NET_WWAN = DEVICE_ID(SIO_MAJOR_HSUSB_ECM , SIO_MINOR_USB_NET_WWAN ),
    /** -- SIO Port id for USB RMNET 2 device */
  SIO_PORT_USB_RMNET2 = DEVICE_ID(SIO_MAJOR_HSUSB_ECM , SIO_MINOR_USB_RMNET2 ),
    /** -- SIO Port id for USB RMNET 3 device */
  SIO_PORT_USB_RMNET3 = DEVICE_ID(SIO_MAJOR_HSUSB_ECM , SIO_MINOR_USB_RMNET3 ),
    /** -- SIO Port id for USB serial 3 device */
  SIO_PORT_USB_SER3 = DEVICE_ID(SIO_MAJOR_USB , SIO_MINOR_USB_SER3 ),
    /** -- SIO Port id for USB BREW extended data device */
  SIO_PORT_USB_BREW_EXTENDED_DATA = DEVICE_ID(SIO_MAJOR_USB , SIO_MINOR_USB_BREW_X_DATA ),  
    /** -- SIO Port id for USB BREW notification device */
  SIO_PORT_USB_BREW_EXTENDED_NOTIFICATION = DEVICE_ID(SIO_MAJOR_USB , SIO_MINOR_USB_BREW_X_NOTIFICATION ), 
    /** -- SIO Port id for USB EEM device */
  SIO_PORT_USB_EEM = DEVICE_ID(SIO_MAJOR_HSUSB_EEM , SIO_MINOR_HSUSB_EEM),

  /*---------------------  SMD DRIVER --------------------- */

    /** -- SIO Port id for SMD First device */
  SIO_PORT_SMD_FIRST = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DS ),
    /** -- SIO Port id for SMD Data(DS) device */
  SIO_PORT_SMD_DS = SIO_PORT_SMD_FIRST,
    /** -- SIO Port id for SMD Diag device */
  SIO_PORT_SMD_DIAG = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DIAG ),
    /** -- SIO Port id for SMD Diag-to-apps device */
  SIO_PORT_SMD_DIAG_APPS = DEVICE_ID(SIO_MAJOR_SMD_TO_APPS , SIO_MINOR_SMD_DIAG ),
    /** -- SIO Port id for SMD Diag-to-modem Ddevice */
  SIO_PORT_SMD_DIAG_MODEM = DEVICE_ID(SIO_MAJOR_SMD_TO_MODEM , SIO_MINOR_SMD_DIAG ),
    /** -- SIO Port id for SMD Diag-to-QDSP6 device */
  SIO_PORT_SMD_DIAG_QDSP = DEVICE_ID(SIO_MAJOR_SMD_TO_QDSP6 , SIO_MINOR_SMD_DIAG ),
    /** -- SIO Port id for SMD RPC call device */
  SIO_PORT_SMD_RPC_CALL = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_RPC_CALL ),
    /** -- SIO Port id for SMD RPC reply device */
  SIO_PORT_SMD_RPC_REPLY = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_RPC_REPLY),
    /** -- SIO Port id for SMD Bluetooth device */
  SIO_PORT_SMD_BT = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_BT ),
    /** -- SIO Port id for SMD control device */
  SIO_PORT_SMD_CONTROL = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_CONTROL ),
    /** -- SIO Port id for SMD Memcopy spare 1 device */
  SIO_PORT_SMD_MEMCPY_SPARE1 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_MEMCPY_SPARE1 ),
    /** -- SIO Port id for SMD Data first device */
  SIO_PORT_SMD_DATA_FIRST = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA1 ),
    /** -- SIO Port id for SMD data1 device */
  SIO_PORT_SMD_DATA1 = SIO_PORT_SMD_DATA_FIRST,
    /** -- SIO Port id for SMD Winmobile modem port1 device */
  SIO_PORT_SMD_WINMOB_MODEM_PORT1 = SIO_PORT_SMD_DATA1,
    /** -- SIO Port id for SMD data2 device */
  SIO_PORT_SMD_DATA2 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA2 ),
    /** -- SIO Port id for SMD Winmobile modem port2 device */
  SIO_PORT_SMD_WINMOB_MODEM_PORT2 = SIO_PORT_SMD_DATA2,
    /** -- SIO Port id for SMD data3 device */
  SIO_PORT_SMD_DATA3 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA3 ),
    /** -- SIO Port id for SMD winmobile modem port3 device */
  SIO_PORT_SMD_WINMOB_MODEM_PORT3 = SIO_PORT_SMD_DATA3,
    /** -- SIO Port id for SMD data4 device */
  SIO_PORT_SMD_DATA4 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA4 ),
    /** -- SIO Port id for SMD winmobile modem port4 device */
  SIO_PORT_SMD_WINMOB_MODEM_PORT4 = SIO_PORT_SMD_DATA4,
    /** -- SIO Port id for SMD data5 device */
  SIO_PORT_SMD_DATA5 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA5 ),
    /** -- SIO Port id for SMD winmobile QMI WWAN device */
  SIO_PORT_SMD_WINMOB_QMI_WWAN = SIO_PORT_SMD_DATA5,
    /** -- SIO Port id for SMD RMNET1 device */
  SIO_PORT_SMD_RMNET1 = SIO_PORT_SMD_DATA5,
    /** -- SIO Port id for SMD data6 device */
  SIO_PORT_SMD_DATA6 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA6 ),
    /** -- SIO Port id for SMD RMNET2 device */
  SIO_PORT_SMD_RMNET2 = SIO_PORT_SMD_DATA6,
    /** -- SIO Port id for SMD data7 device */
  SIO_PORT_SMD_DATA7 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA7 ),
    /** -- SIO Port id for SMD RMNET3 device */
  SIO_PORT_SMD_RMNET3 = SIO_PORT_SMD_DATA7,
    /** -- SIO Port id for SMD data8 device */
  SIO_PORT_SMD_DATA8 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA8 ),
    /** -- SIO Port id for SMD RMNET4 device */
  SIO_PORT_SMD_RMNET4 = SIO_PORT_SMD_DATA8,
    /** -- SIO Port id for SMD data9 device */
  SIO_PORT_SMD_DATA9 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA9 ),
    /** -- SIO Port id for SMD RMNET5 device */
  SIO_PORT_SMD_RMNET5 = SIO_PORT_SMD_DATA9,
    /** -- SIO Port id for SMD data10 evice */
  SIO_PORT_SMD_DATA10 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA10 ),
    /** -- SIO Port id for SMD winmobile QVP MVS device */
  SIO_PORT_SMD_WINMOB_QVP_MVS = SIO_PORT_SMD_DATA10,
    /** -- SIO Port id for SMD data11 device */
  SIO_PORT_SMD_DATA11 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA11 ),
    /** -- SIO Port id for SMD winmobile QVP DATA device */
  SIO_PORT_SMD_WINMOB_QVP_DATA = SIO_PORT_SMD_DATA11,
    /** -- SIO Port id for SMD data12 device */
  SIO_PORT_SMD_DATA12 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA12 ),
    /** -- SIO Port id for SMD RMNET6 device */
  SIO_PORT_SMD_RMNET6 = SIO_PORT_SMD_DATA12,
    /** -- SIO Port id for SMD data13 device */
  SIO_PORT_SMD_DATA13 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA13 ),
    /** -- SIO Port id for SMD RMNET7 device */
  SIO_PORT_SMD_RMNET7 = SIO_PORT_SMD_DATA13,
    /** -- SIO Port id for SMD data14 device */
  SIO_PORT_SMD_DATA14 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA14 ),
    /** -- SIO Port id for SMD RMNET8 device */
  SIO_PORT_SMD_RMNET8 = SIO_PORT_SMD_DATA14,
    /** -- SIO Port id for SMD data15 device */
  SIO_PORT_SMD_DATA15 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA15 ),
    /** -- SIO Port id for SMD data16 device */
  SIO_PORT_SMD_DATA16 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA16 ),
    /** -- SIO Port id for SMD data17 device */
  SIO_PORT_SMD_DATA17 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA17 ),
    /** -- SIO Port id for SMD data18 device */
  SIO_PORT_SMD_DATA18 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA18 ),
    /** -- SIO Port id for SMD data19 device */
  SIO_PORT_SMD_DATA19 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA19 ),
    /** -- SIO Port id for SMD data20 device */
  SIO_PORT_SMD_DATA20 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA20 ),
    /** -- SIO Port id for SMD data21 device */
  SIO_PORT_SMD_DATA21 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA21 ),
    /** -- SIO Port id for SMD data22 device */
  SIO_PORT_SMD_DATA22 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA22 ),
    /** -- SIO Port id for SMD data23 device */
  SIO_PORT_SMD_DATA23 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA23 ),
    /** -- SIO Port id for SMD data24 device */
  SIO_PORT_SMD_DATA24 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA24 ),
    /** -- SIO Port id for SMD data25 device */
  SIO_PORT_SMD_DATA25 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA25 ),
    /** -- SIO Port id for SMD data26 device */
  SIO_PORT_SMD_DATA26 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA26 ),
    /** -- SIO Port id for SMD data27 device */
  SIO_PORT_SMD_DATA27 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA27 ),
    /** -- SIO Port id for SMD data28 device */
  SIO_PORT_SMD_DATA28 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA28 ),
    /** -- SIO Port id for SMD data29 device */
  SIO_PORT_SMD_DATA29 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA29 ),
    /** -- SIO Port id for SMD data30 device */
  SIO_PORT_SMD_DATA30 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA30 ),
    /** -- SIO Port id for SMD data31 device */
  SIO_PORT_SMD_DATA31 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA31 ),
    /** -- SIO Port id for SMD data32 device */
  SIO_PORT_SMD_DATA32 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA32 ),
    /** -- SIO Port id for SMD data33 device */
  SIO_PORT_SMD_DATA33 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA33 ),
    /** -- SIO Port id for SMD data34 device */
  SIO_PORT_SMD_DATA34 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA34 ),
    /** -- SIO Port id for SMD data35 device */
  SIO_PORT_SMD_DATA35 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA35 ),
    /** -- SIO Port id for SMD data36 device */
  SIO_PORT_SMD_DATA36 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA36 ),
    /** -- SIO Port id for SMD data37 device */
  SIO_PORT_SMD_DATA37 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA37 ),
    /** -- SIO Port id for SMD data38 device */
  SIO_PORT_SMD_DATA38 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA38 ),
    /** -- SIO Port id for SMD data39 device */
  SIO_PORT_SMD_DATA39 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA39 ),
    /** -- SIO Port id for SMD data40 device */
  SIO_PORT_SMD_DATA40 = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_DATA40 ),
    /** -- SIO Port id for SMD GPS NMEAdevice */
  SIO_PORT_SMD_GPS_NMEA = DEVICE_ID(SIO_MAJOR_SMD , SIO_MINOR_SMD_GPS_NMEA ),
    /** -- SIO Port id for SMD Last device */
  SIO_PORT_SMD_DATA_LAST = SIO_PORT_SMD_GPS_NMEA,
  

  /*---------------------  SMD-BRIDGE DRIVER --------------------- */

  /** -- SIO Port id for SMD BRIDGE legacy device */
  SIO_PORT_SMD_BRIDGE_LEGACY = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE , SIO_MINOR_SMD_BRIDGE_LEGACY),
    /** -- SIO Port id for SMD BRIDGE first device */
  SIO_PORT_SMD_BRIDGE_FIRST = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE , SIO_MINOR_SMD_BRIDGE_1 ),
    /** -- SIO Port id for SMD BRIDGE 1 device */
  SIO_PORT_SMD_BRIDGE_1 = SIO_PORT_SMD_BRIDGE_FIRST,
    /** -- SIO Port id for SMD BRIDGE 2 device */
  SIO_PORT_SMD_BRIDGE_2 = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE , SIO_MINOR_SMD_BRIDGE_2 ),
    /** -- SIO Port id for SMD BRIDGE 3 device */
  SIO_PORT_SMD_BRIDGE_3 = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE , SIO_MINOR_SMD_BRIDGE_3 ),
    /** -- SIO Port id for SMD BRIDGE 4 device */
  SIO_PORT_SMD_BRIDGE_4 = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE , SIO_MINOR_SMD_BRIDGE_4 ),
    /** -- SIO Port id for SMD BRIDGE 5 device */
  SIO_PORT_SMD_BRIDGE_5 = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE, SIO_MINOR_SMD_BRIDGE_5 ),
    /** -- SIO Port id for SMD BRIDGE last device */
  SIO_PORT_SMD_BRIDGE_LAST = SIO_PORT_SMD_BRIDGE_5,

  /*---------------------  SMD-BRIDGE DRIVER - NEW --------------------- */

    /** -- SIO Port id for SMD Bridge-to-legacy device */
  SIO_PORT_SMD_BRIDGE_TO_LEGACY = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE_NPROC, SIO_MINOR_SMD_BRIDGE_LEGACY), 
    /** -- SIO Port id for SMD Bridge-to-apps device */
  SIO_PORT_SMD_BRIDGE_TO_APPS = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE_NPROC, SIO_MINOR_SMD_BRIDGE_TO_APPS),
    /** -- SIO Port id for SMD Bridge-to-modem device */
  SIO_PORT_SMD_BRIDGE_TO_MODEM = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE_NPROC, SIO_MINOR_SMD_BRIDGE_TO_MODEM),
    /** -- SIO Port id for SMD Bridge-to-qdsp6 device */
  SIO_PORT_SMD_BRIDGE_TO_QDSP6 = DEVICE_ID(SIO_MAJOR_SMD_BRIDGE_NPROC, SIO_MINOR_SMD_BRIDGE_TO_QDSP6),
  /*---------------------  SMEM DRIVER --------------------- */

    /** -- SIO Port id for SMEM DS device */
  SIO_PORT_SMEM_DS = DEVICE_ID(SIO_MAJOR_SMEM , SIO_MINOR_SMEM_DS ), 

  /*---------------------  BT DRIVER --------------------- */

    /** -- SIO Port id for Bluetooth serial port profile device */
  SIO_PORT_BT_SPP = DEVICE_ID(SIO_MAJOR_BLUETOOTH , SIO_MINOR_BLUETOOTH_SPP ), 
    /** -- SIO Port id for Bluetooth network access device */
  SIO_PORT_BT_NA = DEVICE_ID(SIO_MAJOR_BLUETOOTH , SIO_MINOR_BLUETOOTH_NA ),

  /*---------------------  MMGPS DRIVER --------------------- */

    /** -- SIO Port id for LSM LSD device */
  SIO_PORT_LSM_CSD = DEVICE_ID(SIO_MAJOR_MMGPS , SIO_MINOR_MMGPS_LSM_CSD ), 

  /*---------------------  APS DRIVER --------------------- */

    /** -- SIO Port id for APS serial VSP device */
  SIO_PORT_APS_SERIAL_VSP = DEVICE_ID(SIO_MAJOR_APS , SIO_MINOR_APS_DEV ),

  /*---------------------  QMIP DRIVER --------------------- */

    /** -- SIO Port id for QMIP device */
  SIO_PORT_QMIP = DEVICE_ID(SIO_MAJOR_LEGACY , SIO_MINOR_LEGACY_NULL ), 

  /*---------------------  A2 DRIVER --------------------- */

    /** -- SIO Port id for A2 Modem device */
  SIO_PORT_A2_MDM = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_A2_MDM ),
    /** -- SIO Port id for A2 RMNET1 device */
  SIO_PORT_A2_RMNET_1 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_A2_RMNET_1),
    /** -- SIO Port id for A2 RMNET2 device */
  SIO_PORT_A2_RMNET_2 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_A2_RMNET_2),
    /** -- SIO Port id for A2 RMNET3 device */
  SIO_PORT_A2_RMNET_3 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_A2_RMNET_3),

  /*---------------------  Generic A2 Ports --------------------- */

    /** -- SIO Port id for A2 RMNET0 device */
  SIO_PORT_SDIO_MUX_A2_RMNET_0 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_SDIO_MUX_A2_RMNET_0),
    /** -- SIO Port id for A2 RMNET1 device */
  SIO_PORT_SDIO_MUX_A2_RMNET_1 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_SDIO_MUX_A2_RMNET_1),
    /** -- SIO Port id for A2 RMNET2 device */
  SIO_PORT_SDIO_MUX_A2_RMNET_2 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_SDIO_MUX_A2_RMNET_2),
    /** -- SIO Port id for A2 RMNET3 device */
  SIO_PORT_SDIO_MUX_A2_RMNET_3 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_SDIO_MUX_A2_RMNET_3),
    /** -- SIO Port id for A2 RMNET4 device */
  SIO_PORT_SDIO_MUX_A2_RMNET_4 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_SDIO_MUX_A2_RMNET_4),
    /** -- SIO Port id for A2 RMNET5 device */
  SIO_PORT_SDIO_MUX_A2_RMNET_5 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_SDIO_MUX_A2_RMNET_5),
    /** -- SIO Port id for A2 RMNET6 device */
  SIO_PORT_SDIO_MUX_A2_RMNET_6 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_SDIO_MUX_A2_RMNET_6),
    /** -- SIO Port id for A2 RMNET7 device */
  SIO_PORT_SDIO_MUX_A2_RMNET_7 = DEVICE_ID(SIO_MAJOR_A2 , SIO_MINOR_SDIO_MUX_A2_RMNET_7),

  /*---------------------  Data MUX Ports --------------------- */

    /** -- SIO Port id for DATA MUX 1 device */
  SIO_PORT_DATA_MUX_1  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_1),
    /** -- SIO Port id for DATA MUX 2 device */
  SIO_PORT_DATA_MUX_2  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_2),
    /** -- SIO Port id for DATA MUX 3 device */
  SIO_PORT_DATA_MUX_3  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_3),
    /** -- SIO Port id for DATA MUX 4 device */
  SIO_PORT_DATA_MUX_4  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_4),
    /** -- SIO Port id for DATA MUX 5 device */
  SIO_PORT_DATA_MUX_5  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_5),
    /** -- SIO Port id for DATA MUX 6 device */
  SIO_PORT_DATA_MUX_6  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_6),
    /** -- SIO Port id for DATA MUX 7 device */
  SIO_PORT_DATA_MUX_7  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_7),
    /** -- SIO Port id for DATA MUX 8 device */
  SIO_PORT_DATA_MUX_8  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_8),
    /** -- SIO Port id for DATA MUX 9 device */
  SIO_PORT_DATA_MUX_9  = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_9),
    /** -- SIO Port id for DATA MUX 10 device */
  SIO_PORT_DATA_MUX_10 = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_10),
    /** -- SIO Port id for DATA MUX 11 device */
  SIO_PORT_DATA_MUX_11 = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_11),
    /** -- SIO Port id for DATA MUX 12 device */
  SIO_PORT_DATA_MUX_12 = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_12),
    /** -- SIO Port id for DATA MUX 13 device */
  SIO_PORT_DATA_MUX_13 = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_13),
    /** -- SIO Port id for DATA MUX 14 device */
  SIO_PORT_DATA_MUX_14 = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_14),
    /** -- SIO Port id for DATA MUX 15 device */
  SIO_PORT_DATA_MUX_15 = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_15),
    /** -- SIO Port id for DATA MUX 16 device */
  SIO_PORT_DATA_MUX_16 = DEVICE_ID(SIO_MAJOR_DATA_MUX, SIO_MINOR_DATA_MUX_16),

  /*---------------------  ONDEVICE DIAG Ports --------------------- */

    /** -- SIO Port id for Ondevice diag device */
  SIO_PORT_ONDEVICE_DIAG = DEVICE_ID(SIO_MAJOR_ONDEVICE_DIAG, SIO_MINOR_ONDEVICE_DIAG),

  /*---------------------  SDIOC Ports --------------------- */

    /** -- SIO Port id for SDIOC RPC device */
  SIO_PORT_SDIOC_RPC  = DEVICE_ID(SIO_MAJOR_SDIOC, SIO_MINOR_SDIOC_RPC),
    /** -- SIO Port id for SDIOC WWAN device */
  SIO_PORT_SDIOC_WWAN = DEVICE_ID(SIO_MAJOR_SDIOC, SIO_MINOR_SDIOC_WWAN),
    /** -- SIO Port id for SDIOC DIAG device */
  SIO_PORT_SDIOC_DIAG = DEVICE_ID(SIO_MAJOR_SDIOC, SIO_MINOR_SDIOC_DIAG)

  /*---------------------------------------------------------*/
} sio_port_id_type;


/*---------------------------------------------------------------------------*/
/** Resolve the maximum port number for a particular device group. */
/*---------------------------------------------------------------------------*/
  #define SIO_PORT_SMD_LAST   SIO_PORT_SMD_BRIDGE_LAST

  #define SIO_PORT_UART_MAX   SIO_PORT_UART_THIRD

/*---------------------------------------------------------------------------*/
/** Identifies the stream type for device.*/
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_STREAM_RX_ONLY = 0,                      /**< -- Stream only used for RX  */
  SIO_STREAM_TX_ONLY,                          /**< -- Stream only used for TX  */
  SIO_STREAM_RXTX,                             /**< -- Stream used for RX & TX  */
  SIO_STREAM_MAX                               /**< -- Used for bounds checking */
} sio_stream_type;

/*---------------------------------------------------------------------------*/
/** Function pointer type used by device driver to pass incoming data
 packaged in DSM item to Client.*/
/*---------------------------------------------------------------------------*/

typedef void (*sio_rx_func_ptr_type)( dsm_item_type **);

/*---------------------------------------------------------------------------*/
/** Identifies the usb mode type for SPS subsystem. */
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_USB_MODE_LEGACY         = 0,/**< -- Data goes through legacy path */
  SIO_USB_MODE_SPS_USB_BAM    = 1,/**< -- Data goes to USB BAM from another 
                                     BAM,for use with A2 or DMA BAMs */
  SIO_USB_MODE_SPS_USB_SYSTEM = 2,/**< -- Data goes to USB with a CPU setting
                                     up the USB BAM in System mode, for 
                                     use with NAT and possibly Diag */
}sio_usb_mode_enum_type;

/*---------------------------------------------------------------------------*/
/** Identifies the usb endpoint info type. */
/*---------------------------------------------------------------------------*/
typedef struct{
    sio_usb_mode_enum_type mode;
}sio_usb_end_point_info_type;

/*---------------------------------------------------------------------------*/
/** Used by sio client to pass DSM pool information to driver.*/
/*---------------------------------------------------------------------------*/
typedef struct {
    dsm_mempool_id_enum_type dsm_mempool_id;    /**< DSM pool id type. */
    uint32                   mempool_set;       /**< Mempool set. */
    sio_usb_mode_enum_type   usb_mode;          /**< USB mode type. */
    uint32                   smd_fifo_size;     /**< Fifo size for SMD. */
}sio_dsm_pool_type;

/*---------------------------------------------------------------------------*/
/** Identifies the special param open Parameters : to pass special parameter to driver in
 sio_open() and sio_control_open() function calls. */
/*---------------------------------------------------------------------------*/

typedef union sio_open_param{
    sio_dsm_pool_type           dsm_pool_param;     /**< Dsm pool information. */
    sio_usb_end_point_info_type usb_end_point_info; /**< To be set appropriately for 
                                                       data SIO ports (RmNET or PPP). */
}sio_open_param_type;


/*---------------------------------------------------------------------------*/
/** A variable of this type needs to be passed to sio_open.
 sio_open will not modify any of the parameters (hence declaration of
 parameter to sio_open as "const").*/
/*---------------------------------------------------------------------------*/

typedef struct
{
  sio_stream_id_type    stream_id;          /**< Stream ID Type.  Filled in
                                               by SIO for internal use.    */
  sio_stream_type       stream_type;        /**< Type of stream being opened.
                                               Filled in by SIO for 
                                               internal use only.          */
  sio_stream_mode_type  stream_mode;        /**< Stream Mode Type.           */
  dsm_watermark_type   *rx_queue;           /**< Received Data Queue.         */
  dsm_watermark_type   *tx_queue;           /**< Transmit Data Queue.         */
  sio_bitrate_type      rx_bitrate;         /**< Bit-rate for reception.      */
  sio_bitrate_type      tx_bitrate;         /**< Bit-rate for transmission.   */
  sio_port_id_type      port_id;            /**< Port which needs to be used. */
  boolean               tail_char_used;     /**< True, if tail character is used*/
  
  byte                  tail_char;          /**< If tail character is used,
                                               this is the tail character. */

  sio_rx_func_ptr_type  rx_func_ptr;        /**< If non-NULL, this function 
                                               will be called for each. 
                                               packet which is received.   */
  sio_flow_ctl_type     tx_flow;            /**< TX flow control method.      */
  sio_flow_ctl_type     rx_flow;            /**< RX flow control method.      */

  struct bt_spp_open_struct*  bt_open_ptr;  /**< Additional data to open Bluetooth port via SIO.   */
  sio_open_param_type   *open_param;        /**< special open parameter for driver. */
  uint32                forward_bridge_id;  /**< used only for smd_bridge by SIO (not for client use). */
} sio_open_type;

/*---------------------------------------------------------------------------*/
/** Identifies the different return values of SIO operation.*/
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_DONE_S=0,                    /**< -- SIO done status, for successful case. */
  SIO_BADP_S,                      /**< -- Bad parameter.                        */
  SIO_UNAVAIL_S,                   /**< -- No more streams can be allocated.    */
  SIO_BUSY_S,                      /**< -- Command is still being processed.    */
  SIO_RX_NOT_OPEN_S,               /**< -- RX was not opened for this stream.    */
  SIO_TX_NOT_OPEN_S,               /**< -- TX was not opened for this stream.    */
  SIO_BITRATE_CHANGE_FAILED_S,     /**< -- Attempt to change bitrate failed.     */
  SIO_MAX_S                        /**< -- For bounds checking only.             */
} sio_status_type;

/*---------------------------------------------------------------------------*/
/** These are the commands which can be carried out by sio_ioctl.
    These are to be used in conjunction with the above command type.*/
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_IOCTL_CD_ASSERT=0,             /**< -- Assert Carrier Detection Indicator   */
  SIO_IOCTL_CD_DEASSERT,             /**< -- Deassert Carrier Detection Indicator */
  SIO_IOCTL_GET_CURRENT_CD,          /**< -- Return a boolean (in parameter) which
                                        indicates whether or not the CD
                                        signal is asserted                   */
  SIO_IOCTL_RI_ASSERT,               /**< -- Assert Ring Indication               */
  SIO_IOCTL_RI_DEASSERT,             /**< -- Dassert Ring Indication              */
  SIO_IOCTL_GET_CURRENT_RI,          /**< -- Return a boolean (in parameter) which
                                        indicates whether or not the RI
                                        signal is asserted                   */
  SIO_IOCTL_ENABLE_DTR_EVENT,        /**< -- Register clock call-back which 
                                        informs caller of change in state of
                                        data terminal ready indication       */  
  SIO_IOCTL_DISABLE_DTR_EVENT,       /**< -- De-register clock call-back for
                                        informing of change in state of 
                                        data terminal ready indication       */
  SIO_IOCTL_INBOUND_FLOW_ENABLE,     /**< -- Enable inbound flow using current
                                        flow control method                  */
  SIO_IOCTL_INBOUND_FLOW_DISABLE,    /**< -- Disable inbound fow using current
                                        flow control method                  */
  SIO_IOCTL_DSR_ASSERT,              /**< -- Assert Data Set Ready Indication     */
  SIO_IOCTL_DSR_DEASSERT,            /**< -- Deassert Data Set Ready Indication   */
  SIO_IOCTL_DTE_READY_ASSERTED,      /**< -- Return a boolean (in parameter) which
                                        indicates whether or not DTE ready
                                        signal is asserted                   */
  SIO_IOCTL_GET_CURRENT_RTS,         /**< -- Read the current status of RTS from
                                        the PC(DTE),i.e. the MSM(DCE) CTS pin*/
  SIO_IOCTL_SET_FLOW_CTL,            /**< -- Allows setting of flow control 
                                        method                               */
  SIO_IOCTL_GET_FLOW_CTL,            /**< -- Allows getting the flow control
                                          methods                            */
  SIO_IOCTL_ENABLE_AUTODETECT,       /**< -- Enable AutoDetect.  Has no effect if
                                        AutoDetect already enabled.          */
  SIO_IOCTL_RECORD_ESC_FUNC_PTR,     /**< -- Register clock call-back function
                                        which informs caller of arrival of
                                        escape code ( +++ )                  */
  SIO_IOCTL_RECORD_PKT_FUNC_PTR,     /**< -- Register clock call-back function
                                        which informs caller of arrival of
                                        PPP packet                           */
  SIO_IOCTL_BEGIN_FAX_IMAGE,         /**< -- Used by caller to signal beginning
                                        of fax image.  From the beginning of
                                        fax image to the end of fax image on
                                        given line, software flow control 
                                        needs to be ignored because they 
                                        may be unescaped data bytes          */
  SIO_IOCTL_CHANGE_BAUD_RATE,        /**< -- Used to change the baud rate of the  
                                        serial port.  THIS IS FOR BACKWARD
                                        COMPATIBILITY WITH DATA SERVICES ONLY.
                                        The change takes place _only_  _after_
                                        a transition from a non-empty TX FIFO
                                        to an empty TX FIFO. If the TX FIFO
                                        is empty when this ioctl is received, 
                                        no baud change occurs until more data 
                                        is Tx'd.                             */
  SIO_IOCTL_CHANGE_BAUD_NOW,         /**< -- Used to change the baud rate of the
                                        serial port.  Change takes place 
                                        immediately without regard to any data
                                        in the TX FIFO. Hence it is up to the 
                                        app to flush the tx beforehand. The 
                                        usage is otherwise identical to 
                                        SIO_IOCTL_CHANGE_BAUD_RATE.          */
  SIO_IOCTL_FLUSH_TX,                /**< -- Register call-back function which
                                        informs caller when all pending
                                        output data has been transmitted
                                        WARNING: Unreliable results may occur
                                        if data is queued after this function
                                        call is made and before the callback
                                        function has been called.            */
  SIO_IOCTL_CHANGE_MODE,             /**< -- Change the serial port's mode, i.e.,
                                        from AUTODETECT to RAWDATA for
                                        example                              */
  SIO_IOCTL_REG_TLP_CHANGE_CB,       /**< -- Register a callback to be invoked when
                                        the version and/or use of the USB TLP
                                        changes.  Callback invoked immediately
                                        to provide initial state.            */
  SIO_IOCTL_GET_PACKET_SIZE,         /**< -- Read the maximum packet size for
                                        packet devices */
  SIO_IOCTL_SET_PACKET_SIZE,         /**< -- Change the maximum packet size for
                                        packet devices */
  SIO_IOCTL_GET_BUFFER_SIZE,         /**< -- Read the buffer size for devices that
                                        have buffers */
  SIO_IOCTL_SET_BUFFER_SIZE,         /**< -- Change the buffer size for devices
                                        that have variable buffers */
  SIO_IOCTL_GET_XFR_MODE,            /**< -- Read the transfer mode for device */
  SIO_IOCTL_SET_XFR_MODE,            /**< -- Set the transfer mode for device */
  SIO_IOCTL_GET_XFR_TYPE,            /**< -- Read the transfer type for device */
  SIO_IOCTL_SET_XFR_TYPE,            /**< -- Set the transfer type for device */
  SIO_IOCTL_ENABLE_DTR_EVENT_EXT,    /**< -- Register callback which informs caller
                                        of change in state of data terminal
                                        ready indication - callback gets
                                        port_id and user data as args */  
  SIO_IOCTL_DISABLE_DTR_EVENT_EXT,   /**< -- De-register call-back for informing of
                                        change in state of data terminal ready
                                        indication       */
  SIO_IOCTL_ENABLE_CD_EVENT_EXT,     /**< -- Register callback which informs caller
                                        of change in state of carrier detect
                                        indication - callback gets port_id 
                                        and user data as args */  
  SIO_IOCTL_DISABLE_CD_EVENT_EXT,    /**< -- De-register call-back for informing of
                                        change in state of carrier detect
                                        indication       */
  SIO_IOCTL_ENABLE_RI_EVENT_EXT,     /**< -- Register callback for informing of
                                        change in state of ring indication  */
  SIO_IOCTL_DISABLE_RI_EVENT_EXT,    /**< -- De-register callback for informing of
                                        change in state of ring indication  */
  SIO_IOCTL_SET_DROP_PEND,           /**< -- Set the Drop Pending state  */
  SIO_IOCTL_GET_DROP_PEND,           /**< -- Get the Drop Pending state  */
  SIO_IOCTL_BT_CONFIGURE,            /**< -- Configure Bluetooth port.            */
  SIO_IOCTL_BT_DISCONNECT,           /**< -- Disconnect Bluetooth port.           */
  SIO_IOCTL_BT_GET_STATUS,           /**< -- Get Bluetooth status.                */
  SIO_IOCTL_POWERDOWN,               /**< -- Power down the port                  */
  SIO_IOCTL_WAKEUP,                  /**< -- Wake up the port                     */
  SIO_IOCTL_TX_START_BREAK,          /**< -- Start a break event on the TX side   */
  SIO_IOCTL_TX_END_BREAK,            /**< -- End a break event on the TX side     */
  SIO_IOCTL_RX_HIGH_EVENT,           /**< -- Register a RX line high event CB     */
  SIO_IOCTL_CHECK_RX_STATE,          /**< -- Get the current state of RX line     */ 
  SIO_IOCTL_WM_SET_WAIT,             /**< -- Sets the wait flag in SMD            */
  SIO_IOCTL_WM_GET_WAIT,              /**< -- Gets the wait flag in SMD            */
  SIO_IOCTL_NETWORK_CONNECTION_NOTIF, /**< -- used to notify the host whether the physical 
                                         link is up. Triggers sending NETWORK_CONNECTION 
                                         notification on the ECM control pipe*/
  SIO_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, /**< -- Triggers sending the CONNECTION_SPEED_CHANGE 
                                         notification to the host on the ECM control pipe.
                                         This notification is sent to the device by the host 
                                         in order to notify the host upon change in the 
                                         upstream/downstream speed connection */
  SIO_IOCTL_ENABLE_SET_PACKET_FILTER_EVENT, /**< -- This IOCTL is used by the application to provide 
                                         the AL with a cb to be invoked once SET_PACKET_FILTER
                                         notification is received from the host on the ECM
                                         control pipe */
  SIO_IOCTL_GET_MAC_ADDRESS,           /**< -- Used by the application to retrieve the MAC 
                                          address from the ECM AL */
  SIO_IOCTL_IS_STANDARD_ECM,           /**< -- Used by the application to get ECM operational mode:
                                          standard or proprietary */
  SIO_IOCTL_DIAG_CONFIG,               /**< -- Mask-configuration change in On-device Diag */
  SIO_IOCTL_DIAG_QUERY                 /**< -- Query On-device Diag parameters */
} sio_ioctl_cmd_type;


/*---------------------------------------------------------------------------*/
/** Default parameter to be used with ioctl command.  Should
  only be used as place-holder for operations which do not
  require parameters. */
/*---------------------------------------------------------------------------*/
typedef word sio_ioctl_default_param_type;

/*---------------------------------------------------------------------------*/
/** It is used to specify TX flow and RX flow
    control methods. Because standard allows setting of these
    methods separately, SIO interface has to do so as well.    */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_flow_ctl {
  sio_flow_ctl_type tx_flow;    /**< Transmit flow control type. */
  sio_flow_ctl_type rx_flow;    /**< Receive flow control type. */
} sio_ioctl_flow_ctl_type;

/*---------------------------------------------------------------------------*/
/** Structure with current TLP settings to be passed to callback
    registered via SIO_IOCTL_REG_TLP_CHANGE_CB ioctl. */
/*---------------------------------------------------------------------------*/
typedef struct
{
  uint8    version;             /**< Version for TLP settings. */
  boolean  downlink_tlp_on;     /**< Downlink TLP on/off. */
  boolean  uplink_tlp_on;       /**< Uplink TLP on/off. */
} sio_ioctl_usb_tlp_settings_type;

/*---------------------------------------------------------------------------*/
/** Prototype for callback registered via SIO_IOCTL_REG_TLP_CHANGE_CB ioctl. */
/*---------------------------------------------------------------------------*/
typedef void (* sio_usb_tlp_change_cb_f_type)
(
  sio_ioctl_usb_tlp_settings_type,
  void *
);

/*---------------------------------------------------------------------------*/
/** Structure to be passed as SIO_IOCTL_REG_TLP_CHANGE_CB ioctl parameter. */
/*---------------------------------------------------------------------------*/
typedef struct
{
  sio_usb_tlp_change_cb_f_type  callback;    /**< Function to call when TLP
                                                settings change.            */
  void *                        user_data;   /**< will be passed to callback. */
} sio_ioctl_usb_reg_tlp_change_cb_type;

/*---------------------------------------------------------------------------*/
/** Prototype for callback registered via
    SIO_IOCTL_ENABLE_SET_PACKET_FILTER_EVENT ioctl.  */
/*---------------------------------------------------------------------------*/
typedef void (* sio_enable_set_packet_filter_event_func_ptr_type)
(
  uint16 packet_filter_bitmask,  /**< Packet filter bitmask.*/
  void *cb_data                  /**< Pointer to callback data.*/
);

/*---------------------------------------------------------------------------*/
/** Structure to be passed as
    SIO_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF ioctl parameter. */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_connection_speed_change_notif
{
  uint32                         upstream_bitrate;   /**< Upstream bit rate.*/
  uint32                         downstream_bitrate; /**< Downstream bit rate.*/
}sio_ioctl_connection_speed_change_notif_type;

/*---------------------------------------------------------------------------*/
/** Structure to be passed as
    SIO_IOCTL_ENABLE_SET_PACKET_FILTER_EVENT ioctl parameter.  */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_enable_set_packet_filter_event
{
  sio_enable_set_packet_filter_event_func_ptr_type cb_func;
  void *                cb_data;
} sio_ioctl_enable_set_packet_filter_event_type;

/*---------------------------------------------------------------------------*/
/** Function pointer type which points to a function that takes no arguments
 and returns nothing (i.e. void). */
/*---------------------------------------------------------------------------*/

typedef void (*sio_vv_func_ptr_type)( void);

/*---------------------------------------------------------------------------*/
/** Function pointer type which points to a function that takes a uint32 as 
 arguments and returns nothing (i.e void). This is meant to be
 used for callbacks.*/
/*---------------------------------------------------------------------------*/
typedef void (*sio_vpu_func_ptr_type)(void * cb_data);


/*---------------------------------------------------------------------------*/
/** Structure to be passed as SIO_IOCTL_ENABLE_DTR_EVENT_EXT ioctl parameter. */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_enable_dtr_event_ext{
  sio_vpu_func_ptr_type cb_func;
  void *                cb_data;
} sio_ioctl_enable_dtr_event_ext_type;

/*---------------------------------------------------------------------------*/
/** Structure to be passed as SIO_IOCTL_ENABLE_CD_EVENT_EXT ioctl parameter.  */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_enable_dtr_event_ext sio_ioctl_enable_cb_event_ext_type;

/*---------------------------------------------------------------------------*/
/** Structure to be passed as SIO_IOCTL_ENABLE_RI_EVENT_EXT ioctl parameter.  */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_enable_dtr_event_ext sio_ioctl_enable_ri_event_ext_type;

/*---------------------------------------------------------------------------*/
/** Aggregate SIO IOCTL parameter type. It is used to pass additional parameter
    in sio_ioctl() or sio_control_ioctl(). */
/*---------------------------------------------------------------------------*/

typedef union sio_ioctl_param {
  sio_ioctl_default_param_type   carrier_detect_assert;     /**< Carrier detect assert */
  sio_ioctl_default_param_type   carrier_detect_deassert;   /**< Carrier detect deassert */
  sio_ioctl_default_param_type   ring_indicator_assert;     /**< Ring indicator assert */
  sio_ioctl_default_param_type   ring_indicator_deassert;   /**< Ring indicator deassert */
  sio_vv_func_ptr_type           enable_dte_ready_event;    /**< Enable dte ready event */
  sio_ioctl_default_param_type   disable_dte_ready_event;   /**< Disable dte ready event */
  sio_ioctl_default_param_type   inbound_flow_enable;       /**< Inbound flow enable */
  sio_ioctl_default_param_type   inbound_flow_disable;      /**< Inbound flow disable */
  sio_ioctl_default_param_type   data_set_ready_assert;     /**< Data set ready assert */
  sio_ioctl_default_param_type   data_set_ready_deassert;   /**< Data set ready deassert */
  boolean                       *dte_ready_asserted;        /**< DTE ready asserted */
  boolean                       *rts_asserted;              /**< RTS asserted */
  boolean                       *cd_asserted;               /**< Carrier detect asserted */
  boolean                       *ri_asserted;               /**< Ring indicator asserted */
  sio_ioctl_flow_ctl_type        flow_ctl;                  /**< Flow control type */
  sio_ioctl_default_param_type   rearm_autobaud;            /**< Rearm Auto baudrate */
  sio_vv_func_ptr_type           record_escape_func_ptr;    /**< Record escape function pointer */
  sio_vv_func_ptr_type           record_pkt_func_ptr;       /**< Record packet function pointer */
  sio_ioctl_default_param_type   begin_fax_image;           /**< Begin fax image */
  sio_bitrate_type               bitrate;                   /**< Bitrate type */
  sio_stream_mode_type           stream_mode;               /**< Stream mode type */
  sio_open_type                 *open_ptr;                  /**< Sio Open type */
  sio_vv_func_ptr_type           record_flush_func_ptr;     /**< Record flush function pointer */
  uint32                         packet_size;               /**< Packet size */
  uint32                         buffer_size;               /**< Buffer size */
  boolean                        drop_asserted;             /**< Drop asserted*/
  sio_dataxfr_mode_type          dataxfr_mode;              /**< Data transfer mode */
  sio_xfrflow_type               xfrflow_type;              /**< Transfer flow type*/
  sio_ioctl_enable_dtr_event_ext_type   enable_dte_ready_event_ext; /**< Enable DTE ready event ext */
  sio_ioctl_enable_cb_event_ext_type    enable_cd_ready_event_ext;  /**< Enable CD ready event ext */
  sio_ioctl_enable_ri_event_ext_type    enable_ri_ready_event_ext;  /**< Enable RI ready event ext */
  sio_ioctl_usb_reg_tlp_change_cb_type  tlp_change_cb;      /**< TLP change callback */        
  struct bt_spp_config_struct*   bt_spp_config_ptr;         /**< Bluetooth_SPP config pointer */
  struct bt_spp_status_struct*   bt_spp_status_ptr;         /**< Bluetooth SPP status pointer*/
  sio_vv_func_ptr_type           rx_high_event_func_ptr;    /**< RX high event function pointer*/
  boolean                       *rx_line_state;             /**< RX line state */
  boolean                        wm_wait;                   /**< WM wait*/
  boolean                        link_up;                   /**< Link up*/
  sio_ioctl_connection_speed_change_notif_type connection_speed_change_notif;   /**< Connection speed change notification*/
  sio_ioctl_enable_set_packet_filter_event_type set_packet_filter_event;        /**< Set packet filter event*/
  uint8                          mac_address[SIO_MAC_ADDR_STR_SIZE];  /**< Mac address*/
  boolean                        is_standard_ecm;                     /**< Standard ECM boolean*/
} sio_ioctl_param_type;


  /*~ PARTIAL sio_ioctl_param */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_RATE sio_ioctl_param.rearm_autobaud */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_NOW sio_ioctl_param.rearm_autobaud */
  /*~ CASE SIO_IOCTL_NETWORK_CONNECTION_NOTIF sio_ioctl_param.link_up */
  /*~ CASE SIO_IOCTL_IS_STANDARD_ECM sio_ioctl_param.is_standard_ecm */
  /*~ CASE SIO_IOCTL_GET_MAC_ADDRESS sio_ioctl_param.mac_address */
  /*~ CASE SIO_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF sio_ioctl_param.connection_speed_change_notif */
  /*~ DEFAULT sio_ioctl_param.void */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================*/
/** 
@brief 
  This function opens a stream across the serial link. 

  This function calls the driver's open api associated with port_id being
  passed by client in *open_ptr. This function creates a stream connection 
  between the calling application and the specified hardware port. Assuming 
  a successful completion, the stream_id returned is used to specify this 
  stream in future SIO API calls.

@param[in]  *open_ptr -- pointer to sio_open_type structure

        Client defined members in *open_ptr:
        <ul>
        <li>stream_mode -- The mode specifies the internal SIO behavior required 
                       of this stream. Most new applications should consider 
                       using the SIO_GENERIC_MODE.
        -- SIO_DS_AUTODETECT_MODE and SIO_DS_RAWDATA_MODE support backward 
           compatibility for existing data applications.
        -- SIO_DS_PKT_MODE specifies Packet Data mode.
        -- SIO_DM_MODE supports the DIAG application for communication with 
            the external Diagnostic Module program.

        <li>rx_queue,tx_queue -- These members contain pointers to 
                             dsm_watermark_type structures that describe 
                             the Rx and Tx watermark queues

        <li>rx_bitrate,tx_bitrate -- For UART ports these will determine the 
                        actual baud rate used. Currently, the Rx bit rate 
                        and Tx bit rate must be the same.
        <li>port_id -- The port_id specifies which hardware device to open
                   for the calling application.

        <li>tail_char_used -- This is a Boolean variable, which specifies
                          whether or not a tail character is used to delimit 
                          packets in the data stream.

        <li>tail_char -- If the tail_char_used flag above is nonzero, 
                    the tail character must be specified here.

        <li>rx_func_ptr -- If this pointer is non-NULL, it is assumed to point
                       to a function to be called for each packet received.

        <li>tx_flow, rx_flow -- These specify the Tx and Rx flow control 
                            methods to be used on this port.
        </ul>


@dependencies 
  sio_init must have been called.  sio_init needs to be called by just one
  task and only once upon power-up.

@sideeffects 
  None

@return 
  If successful, a nonnegative stream identifier (that is, a handle) will be 
  returned. This identifier will be used in subsequent SIO requests to specify
   this port.

  If the request could not be completed, the value SIO_NO_STREAM_ID will be 
  returned to indicate the failure, as indicated below.

@sa
  sio_open_type
  sio_stream_id_type
*/ 
/*===========================================================================*/

extern sio_stream_id_type sio_open
( 
  sio_open_type *open_ptr                      /* Configurations of stream */
);

/*==========================================================================*/
/**
@brief
  This procedure does all the book-keeping necessary to close a stream.
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.

  This function will call the close apis of device driver associated with
  stream_id. Closes a serial stream and optionally notifies the caller 
  when all pending output data has been transmitted via the callback 
  function pointer.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@param[in]  *close_func_ptr -- Pointer to the function to be invoked when
      all pending output data has been transmitted. Use NULL if no
      callback is specified.

@dependencies
  None

@sideeffects
  close_func_ptr will be called when all pending data has left
  transmitter.

@return
  None

@sa
  None
*/
/*=====================================================================*/

extern void sio_close
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  void             (*close_func_ptr)(void)    /* Function to call when 
                                                 transmission is complete. */
);

/*=========================================================================*/
/** 
@brief
  This function will transmit a packet or if transmitter is busy, schedule
  the transmission of a packet.

  This function will call driver's transmit api associated with stream_id.
  Device driver will queue the specified data for transmitting.
  The SIO subsystem(driver) will free the dsm item when the data has been sent.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@dependencies
  The specified stream_id must be open and owned by the caller.
  The pointer parameter tx_ptr must point to a valid dsm item.
  
@sideeffects
  None

@return
  None

@sa
  None
*/
/*=========================================================================*/

extern void sio_transmit
( 
  sio_stream_id_type stream_id,    /* Stream ID                            */
  dsm_item_type *tx_ptr            /* Packet which needs to be transmitted */
);

/*=========================================================================*/
/** 
@brief
  This function will transmit any queued data and then call the registered
  callback.

  This function will call driver's flush api, if supported, associated
  with stream_id. callback_function is called by driver when flush is
  completed.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.

@dependencies
  The specified stream_id must be open and owned by the caller.
  
@sideeffects
  None

@return
  None

@sa
  None
*/  
/*=========================================================================*/

extern void sio_flush_tx
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  void                   (*close_func_ptr)(void) /* Function to call when 
                                                 transmission is complete. */    
);


/*=========================================================================*/
/**
@brief
  Allows control of open stream.

  The I/O control operation specified by the cmd and param parameters is performed.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@param[in]  cmd      --  Type of control operation (command) being requested.
@param[in/out] *param -- Extra parameter depending on cmd_type.

@dependencies
  The specified stream_id must be open and owned by the caller.
  The cmd and param parameters must be valid.

@sideeffects
  None

@return
  None (*param may return some value depending on cmd)

@sa
  sio_ioctl_cmd_type
  sio_ioctl_param_type
*/
/*=========================================================================*/

void sio_ioctl
(
  sio_stream_id_type    stream_id, /* Stream ID                            */ 
  sio_ioctl_cmd_type    cmd,       /* IOCTL command                        */
  sio_ioctl_param_type *param      /* Parameter to be used for command     */
);

/*=========================================================================*/
/** 
@brief
  Allows control of open stream.

  The I/O control operation specified by the cmd and param parameters is performed.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@param[in]  cmd      --  Type of control operation (command) being requested.
@param[in/out] *param -- Extra parameter depending on cmd_type.

@dependencies
  The specified stream_id must be open and owned by the caller.
  The cmd and param parameters must be valid.

@sideeffects
  None

@return
  None (*param may return some value depending on cmd)

@sa
  sio_ioctl_cmd_type
  sio_ioctl_param_type
*/
/*=========================================================================*/

void sio_control_ioctl
(
  sio_stream_id_type    stream_id, /* Stream ID                            */ 
  sio_ioctl_cmd_type    cmd,       /* IOCTL command                        */
  sio_ioctl_param_type *param      /* Parameter to be used for command     */
);

/*=========================================================================*/
/** 
@brief
  This routine checks if the specified baud rate is valid under the 
  current feature set.  

  The primary purpose of this utility is to validate baud rate values 
  recovered from NVRAM, which may not be valid in the current feature set.  
  For example, SIO_BITRATE_AUTO is only valid when FEATURE_AUTOBAUD is 
  defined.  

@param[in] check_baud -- Baud rate being checked 

@dependencies
  None
                
@sideeffects
  None

@return
  TRUE if the baud rate is valid under the current feature set.
  FALSE otherwise.

@sa
  None
*/
/*=========================================================================*/

extern boolean sio_is_baud_valid
(
  sio_bitrate_type   check_baud              /* SIO baud rate              */
);

/*=========================================================================*/
/** 
@brief 
  This function opens a control stream across the serial link. 

  This function calls driver's control_open api associated with port_id being
  passed by client in *open_ptr. This function creates a stream connection 
  between the calling application and the specified hardware port. Assuming 
  a successful completion, the stream_id returned is used to specify this 
  stream in future SIO API calls.

@param[in]  *open_ptr -- pointer to sio_open_type structure

        Client defined members in *open_ptr:
        <ul>
        <li>stream_mode -- The mode specifies the internal SIO behavior required 
                       of this stream. Most new applications should consider 
                       using the SIO_GENERIC_MODE.
        -- SIO_DS_AUTODETECT_MODE and SIO_DS_RAWDATA_MODE support backward 
           compatibility for existing data applications.
        -- SIO_DS_PKT_MODE specifies Packet Data mode.
        -- SIO_DM_MODE supports the DIAG application for communication with 
            the external Diagnostic Module program.

        <li>rx_queue,tx_queue -- These members contain pointers to 
                             dsm_watermark_type structures that describe 
                             the Rx and Tx watermark queues

        <li>rx_bitrate,tx_bitrate -- For UART ports these will determine the 
                        actual baud rate used. Currently, the Rx bit rate 
                        and Tx bit rate must be the same.
        <li>port_id -- The port_id specifies which hardware device to open
                   for the calling application.

        <li>tail_char_used -- This is a Boolean variable, which specifies
                          whether or not a tail character is used to delimit 
                          packets in the data stream.

        <li>tail_char -- If the tail_char_used flag above is nonzero, 
                    the tail character must be specified here.

        <li>rx_func_ptr -- If this pointer is non-NULL, it is assumed to point
                       to a function to be called for each packet received.

        <li>tx_flow, rx_flow -- These specify the Tx and Rx flow control 
                            methods to be used on this port.
        </ul>


@dependencies 
  sio_init must have been called.  sio_init needs to be called by just one
  task and only once upon power-up.

@sideeffects 
  None

@return 
  If successful, a nonnegative stream identifier (that is, a handle) will be 
  returned. This identifier will be used in subsequent SIO requests to specify
   this port.

  If the request could not be completed, the value SIO_NO_STREAM_ID will be 
  returned to indicate the failure, as indicated below.

@sa
  sio_open_type
  sio_stream_id_type
*/ 
/*=========================================================================*/
sio_stream_id_type sio_control_open
( 
  sio_open_type *open_ptr                       /* SIO Open structure      */
);

/*=========================================================================*/
/**                                                                             
@brief
  This procedure does all the book-keeping necessary to close a stream.
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.

  This function will call the control_close apis of device driver associated 
  with stream_id.  Closes a serial stream and optionally notifies the caller 
  when all pending output data has been transmitted via the callback 
  function pointer.  

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@param[in]  *close_func_ptr -- Pointer to the function to be invoked when
      all pending output data has been transmitted. Use NULL if no
      callback is specified.

@dependencies
  None

@sideeffects
  close_func_ptr will be called when all pending data has left
  transmitter.

@return
  None

@sa
  None
*/
/*=========================================================================*/

void sio_control_close
( 
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
);


/*=========================================================================*/
/**
@brief
  This function will transmit a packet or if transmitter is busy, schedule
  the transmission of a packet.

  This function will call driver's control_transmit api associated with 
  stream_id. Device driver will queue the specified data for transmitting.
  The SIO subsystem(driver) will free the dsm item when the data has been sent.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@dependencies
  The specified stream_id must be open and owned by the caller.
  The pointer parameter tx_ptr must point to a valid dsm item.
  
@sideeffects
  None

@return
  None

@sa
  None
*/
/*=========================================================================*/

void sio_control_transmit
( 
  sio_stream_id_type stream_id,           /* Stream ID                     */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
);

/*---------------------------------------------------------------------------*/
/** Structure used to register Driver side interface. This structure is 
    populated by device driver with corresponding driver's api and is 
    registered with SIO for specific SIO Port owned by device driver.*/
/*---------------------------------------------------------------------------*/

typedef struct
{
    /** *HW_open api to be called when sio_open() is called by client */
    sio_status_type (*HW_open) (sio_open_type *open_ptr);
    /** *HW_close api to be called when sio_close() is called by client */
    void (*HW_close) (sio_stream_id_type stream_id,
                      sio_port_id_type      port_id,
                        void (*close_func_ptr)(void));
    /** *HW_ioctl api to be called when sio_ioclt() is called by client */
    void (*HW_ioctl) (sio_stream_id_type stream_id,
                      sio_port_id_type      port_id,
                      sio_ioctl_cmd_type    cmd,
                      sio_ioctl_param_type *param);
    /** *HW_transmit api to be called when sio_transmit() is called by client */
    void (*HW_transmit) ( sio_stream_id_type stream_id,
                      sio_port_id_type   port_id,
                      dsm_item_type     *tx_ptr);
    /** *HW_flush_tx api to be called when sio_flush_tx() is called by client */
    void (*HW_flush_tx) (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         void (*flush_func_ptr)(void));
    /** *HW_disable_device api to be called when sio_disable_device()
     is called by client */
    void (*HW_disable_device) (sio_port_id_type   port_id);

    /** *HW_control_open api to be called when sio_control_open() 
    is called by client */
    sio_status_type (*HW_control_open) (sio_open_type *open_ptr);
    /** *HW_control_close api to be called when sio_control_close()
     is called by client */
    void (*HW_control_close) (sio_stream_id_type stream_id,
                              sio_port_id_type      port_id,
                              void (*close_func_ptr)(void));
    /** *HW_control_ioctl api to be called when sio_control_ioctl() is 
    called by client */
    void (*HW_control_ioctl) (sio_stream_id_type stream_id,
                             sio_port_id_type      port_id,
                             sio_ioctl_cmd_type    cmd,
                             sio_ioctl_param_type *param);
    /** *HW_control_transmit api to be called when sio_control_transmit() 
    is called by client */
    void (*HW_control_transmit) (sio_stream_id_type stream_id,
                                 sio_port_id_type      port_id,
                                 dsm_item_type     *tx_ptr);
    
    boolean is_registered;    /** Used by sio layer to indicated if driver is 
                                 registered or not */

}sio_driver_interface;

/*---------------------------------------------------------------------------*/
/** SIO device interface.
    Structure used to register Driver side interface. This structure is 
    populated by device driver with corresponding driver's api and is 
    registered with SIO for specific SIO Port owned by device driver.*/
/*---------------------------------------------------------------------------*/

typedef struct
{
    uint32      magic_num;          /**< Magic number for sanity check */
    /* Data related to the device being used */
    uint32      device_id;          /**< Port_id/Device_id being used for 
                                         device driver registration */
    boolean     forward_port;       /**< Non zero = forward this port  */
    boolean     driver_available;   /**< Non zero = No driver available */
    sio_port_id_type forward_port_id;  /**< identifies which bridge to use.
                                            use default value SIO_PORT_NULL
                                            for legacy bridge (2-proc targets) */
    sio_driver_interface * driver_interface;  /**< Pointer to a driver_interface
                                                   structure.*/
}sio_device_interafce;

/*---------------------------------------------------------------------------*/
/** Prototype for SIO register function callback*/
/*---------------------------------------------------------------------------*/
typedef uint32 (*sio_register_device_driver_cb_type)(sio_device_interafce * device_iface_ptr);

/*---------------------------------------------------------------------------*/
/** Prototype for Driver init function */
/*---------------------------------------------------------------------------*/
typedef uint32 (*sio_driver_init_func_type)(sio_register_device_driver_cb_type register_driver_cb_fn);

/*=========================================================================*/
/**
@brief
  Registers driver interface in driver_list for given major number

@param[in]  major_num -- major number associated with device driver.
@param[in]  *driver_iface_ptr -- pointer to driver's interface structure

@dependencies
  None

@sideeffects
  None

@return
  1 -- if successful
  0 -- if not successful

@sa
  None
*/ 
/*=========================================================================*/

uint32 sio_register_driver
(
    uint32 major_num,  /* Major number of driver */
    sio_driver_interface * driver_iface_ptr  /* Pointer to driver interface */
); 

/*=========================================================================*/
/**
@brief
  Registers devices of driver with SIO 

@param[in]  *device_iface_ptr -- pointer to driver's interface structure
      
@dependencies
  None
                                                       
@sideeffects
  None

@return
  0 -- Failed to register the SIO device with SIO service
  1 -- Success

@sa
  None
*/
/*=========================================================================*/

uint32 sio_register_device_driver(sio_device_interafce * device_iface_ptr);


#endif /* SIO_H */

