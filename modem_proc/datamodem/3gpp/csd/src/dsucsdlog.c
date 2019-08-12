/*===========================================================================

              U C S D   L O G   F U N C T I O N S  

DESCRIPTION
  This source file contains the UMTS CSD log functionality.  
  

   Copyright (c) 2003 - 2014 by Qualcomm Technologies INCORPORATED.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/src/dsucsdlog.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/29/11   SA      Added support for TD-SCDMA.
07/09/09   BS      Fixed offtarget lint medium and low's.
03/07/09   MS      CMI Integration fixes
02/06/08   SA      Updated RX Frame to include FCS error.
03/28/07   AR      More Lint error corrections.
02/02/06   AR      Fix L4 build issue on log buffer allocation.
                   Lint corrections.
08/16/04   AR      Relocate CS Data packets from local definitions
05/30/04   AR      Lint corrections.
03/19/03   TMR     Initial release

===========================================================================*/




/*===========================================================================
Data definitions
===========================================================================*/


/*===========================================================================
Function definitions
===========================================================================*/ 

/*===========================================================================
Header files
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS)


#include "memory.h"
#include "err.h"
#include "dsucsdnti.h"
#include "dsucsdlog.h"
#ifndef BOA_APPS

#include "dsucsdbc.h"
#include "log.h"
#include "log_codes_umts.h"

/*-------------------------------------------------------------------------

--------------------------------
Frame Info BIT MASK description
--------------------------------

Bit  15   14   13   12  11  10  9   8   7    6    5    4    3   2   1   0
   ------------------------------------------------------------------------
   |    |    |    |   |   |   |   |   |    |    |   |   |    |    |   |   |
   | M1 | M2 |T/NT| S | P | P | P | D | Mo | Mo |RLP|RLP| FS | FS |Fcs|Res|
   |    |    |    |   |   |   |   |   |    |    |   |   |    |    |   |   |
   ------------------------------------------------------------------------
  M1   = indicates whether this frame is first half /second half of 576 bit
         RLP frame.
         0 -> first half 
         1 -> 2nd half.
  M2   = (0,1) DTX indicator present or not
  T/NT = Transmission mode
         0 -> transparent
         1 -> Non-transarent
  S    = Stop bits
         0 -> 1 stop bit
         1 -> 2 stop bit
  P    = Parity Bits (000,010,011,100,101) equivalent to
         (Odd,Even,None,forced-to-0,forced-to-1)
  Mo   = System Mode 
         00 -> GSM
         01 -> WCDMA
         10/11 -> N/A
  RLP  = 00 -> RLP ver 0
         01 -> RLP ver 1
         10 -> RLP ver 2
         11 -> N/A
  FS   = Frame size
         00 - 240 bits
         01 - 288 bits
         10 - 576 bits
         11 - 640 bits
  Fcs  = Frame Check Sum Error
         0 -> No Error
         1 -> Error is present
  Res  = Reserved for future use.

-------------------------------------
  Rates: User and FAX data rate info
-------------------------------------
    User rate - 4 bits MSB
    Fax rate  - 4 bits LSB

    0000 - None
    0001 - 300 bps
    0010 - 600 bps
    0011 - 1200 bps
    0100 - 1200/300 bps
    0101 - 2400 bps
    0110 - 4800 bps
    0111 - 7200 bps
    1000 - 9600 bps
    1001 - 12000 bps
    1010 - 14400 bps
    rest are reserved.
  
--------------------------------------------------------------------------*/

#define DS_UCSD_LOG_FCS_ERR_PRESENT    0x0002
#define DS_UCSD_LOG_288_BIT_FRAME_MASK 0x0004
#define DS_UCSD_LOG_576_BIT_FRAME_MASK 0x0008
#define DS_UCSD_LOG_640_BIT_FRAME_MASK 0x000C
#define DS_UCSD_LOG_8_BITS_PER_CHAR    0x0100
#define DS_UCSD_LOG_2_STOP_BITS        0x1000
#define DS_UCSD_LOG_NT_CALL            0x2000
#define DS_UCSD_LOG_SYS_WCDMA          0x0040
#define DS_UCSD_LOG_SYS_UNKNOWN        0x00C0
#define DS_UCSD_LOG_NA_RLP_NUMBER      0x0030

#define DS_UCSD_LOG_PARITY_BITS_OFFSET 9
#define DS_UCSD_LOG_RLP_VERSION_OFFSET 4

/* Define log structures */
PACKED struct ucsd_cs_log_hdr
{
   uint16 frame_info;
   uint8  reserved;
   uint8  rates;
};

/*lint -e754 */
LOG_RECORD_DEFINE(LOG_UMTS_CS_DATA_RX_C)
   struct ucsd_cs_log_hdr ucsd_hdr;
   uint8 data[DS_UMTS_CSD_FRAME_SIZE_MAX];
LOG_RECORD_END

LOG_RECORD_DEFINE(LOG_UMTS_CS_DATA_TX_C)
   struct ucsd_cs_log_hdr ucsd_hdr;
   uint8 data[DS_UMTS_CSD_FRAME_SIZE_MAX];
LOG_RECORD_END


LOCAL uint16 Log_header_frame_info = 0; 
LOCAL uint8  Log_header_rates = 0;



/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_init

DESCRIPTION
   Initializes log header information that should be done prior to 
   starting to do any packet logging.  This sets up the basic log
   packet header information that is present on every log packet.
         
DEPENDENCIES
   None.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Sets some global header variables

===========================================================================*/
void
ds_ucsd_log_init
(
   uint8                   num_data_bits,
   uint8                   num_stop_bits,
   uint8                   num_parity_bits,
   boolean                 transparent_call,
   ds_ucsd_data_rate_T     data_rate,
   sys_sys_mode_e_type     curr_mode,
   uint8                   rlp_version_number    
)
{   
   /* Initialize headers to 0 */
   Log_header_frame_info = 0;
   
   
   /* If it is a transparent call, set the number of data bits, 
   ** number of stop bits and parity bits frame_info.  Note that the
   ** transparent/non-transparent bit is not touched because
   ** the correct value is 0 for transparent calls 
   */
   if (transparent_call)
   {
      Log_header_frame_info = (uint16)
                    (num_parity_bits & 0x07) << DS_UCSD_LOG_PARITY_BITS_OFFSET;
   
      if (num_data_bits == _8_BITS_PER_CHAR)
      {
         Log_header_frame_info |= DS_UCSD_LOG_8_BITS_PER_CHAR;
      }
      
      if (num_stop_bits == _2_STOP_BITS)
      {
         Log_header_frame_info |= DS_UCSD_LOG_2_STOP_BITS;
      }
      
      /* RLP version number is N/A in transparent mode */
      Log_header_frame_info |= DS_UCSD_LOG_NA_RLP_NUMBER;
   }
   /* If non-transparent call, indicate it */
   else
   {
      Log_header_frame_info |= DS_UCSD_LOG_NT_CALL;
      
      /* Now fill in the mode and RLP version info (2 bits) */
      Log_header_frame_info |= (uint16) 
                 (rlp_version_number & 0x03) << DS_UCSD_LOG_RLP_VERSION_OFFSET;
   }
    
   switch (curr_mode)
   {
      case SYS_SYS_MODE_GSM:
         /* Mode bits are 0 for GSM */
         break;

      case SYS_SYS_MODE_WCDMA:
#ifdef FEATURE_TDSCDMA
      case SYS_SYS_MODE_TDS:
#endif /* FEATURE_TDSCMA */
         Log_header_frame_info |= DS_UCSD_LOG_SYS_WCDMA;
         break;
      
      default:
         Log_header_frame_info |= DS_UCSD_LOG_SYS_UNKNOWN;
   }
   
      
   Log_header_rates = (uint8)((uint8)data_rate << 4);
}
   

/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_fax_init

DESCRIPTION
   Initializes log header information that should be done prior to 
   starting to do any packet logging.  This sets up the basic log
   packet header information that is present on every log packet.
         
DEPENDENCIES
   None.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Sets some global header variables

===========================================================================*/ 
void
ds_ucsd_log_fax_init
(
   ds_ucsd_data_rate_T fax_rate
)
{
   /* Set the fax rate in the rates header */
   Log_header_rates |= ((uint8)fax_rate & 0x0F);
}  


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_rx_data

DESCRIPTION
   Logs UMTS Async RX data.  Pointer to the packet to log and frame_size
   are the parameters.  Note that in the DS_UMTS_CSD_FRAME_SIZE_288_BIT
   frame size case that corresponds to GSM 14400, the first 16 bits
   of the data contain the M1 and M2 bits. 
         
DEPENDENCIES
   ds_ucsd_log_init() should be called prior to calling this.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   logs data

===========================================================================*/
void 
ds_ucsd_log_rx_data
(
   uint8                 *radio_rx_data,
   ds_ucsd_frame_size_T   frame_size,
   boolean                fcs_error
)
{
   register uint32                     M1 = 0;
   register LOG_UMTS_CS_DATA_RX_C_type *log_ptr;
   register uint16                     m_bits = 0;
   register uint16                     tmp_hdr_frame_info;   
   
   ASSERT( NULL != radio_rx_data );
   
   /* Allocate the log buffer, TODO: subtract 1 from the frame size to make up
   ** for the data array of 1 element in the declaration?  Logging
   ** funkiness.
   */              
   log_ptr = (LOG_UMTS_CS_DATA_RX_C_type *)log_alloc(LOG_UMTS_CS_DATA_RX_C, 
                sizeof (LOG_UMTS_CS_DATA_RX_C_type) + (uint32) frame_size-1);
                
   /* Log the RX data */             
   if (log_ptr != NULL)
   {
      tmp_hdr_frame_info = Log_header_frame_info;
      log_ptr->ucsd_hdr.rates = Log_header_rates;
      log_ptr->ucsd_hdr.reserved = 0;
      
      /* Save log header size */
      
      switch (frame_size)
      {
         case DS_UMTS_CSD_FRAME_SIZE_240_BIT:
            /* 240 bit frame mask is 0's */
            break;
            
         case DS_UMTS_CSD_FRAME_SIZE_288_BIT:
            tmp_hdr_frame_info |= DS_UCSD_LOG_288_BIT_FRAME_MASK;
      
            /* Log M1 and M2 bits */
            DS_UCSD_GET_M1_M2_BITS (radio_rx_data,M1);
            if (M1)
            {
               m_bits |= OTA_M1_BIT;
            }
            tmp_hdr_frame_info |= m_bits;  
            break;
            
         case DS_UMTS_CSD_FRAME_SIZE_576_BIT:
            tmp_hdr_frame_info |= DS_UCSD_LOG_576_BIT_FRAME_MASK;
            break;
            
         case DS_UMTS_CSD_FRAME_SIZE_640_BIT:
         default:
           tmp_hdr_frame_info |= DS_UCSD_LOG_640_BIT_FRAME_MASK;
           break;
      }        

      /* Set the fcs error in frame info, if present */
      if(TRUE == fcs_error)
      {
        tmp_hdr_frame_info |= DS_UCSD_LOG_FCS_ERR_PRESENT;
      }
      /* Save the temporary header frame data to log packet */
      log_ptr->ucsd_hdr.frame_info = tmp_hdr_frame_info;
      
      /* Log the rest of the data */
      memscpy ((void *)log_ptr->data, (uint32) frame_size, 
               radio_rx_data, (uint32) frame_size);
      log_commit(log_ptr);
   } 
}


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_tx_data

DESCRIPTION
   Logs UMTS Async TX data.  Pointer to the packet to log and frame_size
   are the parameters.  Note that in the DS_UMTS_CSD_FRAME_SIZE_288_BIT
   frame size case that corresponds to GSM 14400, the first 16 bits
   of the data contain the M1 and M2 bits. 
         
DEPENDENCIES
   ds_ucsd_log_init() should be called prior to calling this.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   logs data

===========================================================================*/
void 
ds_ucsd_log_tx_data
(
   uint8                 *radio_tx_data,
   ds_ucsd_frame_size_T  frame_size
)
{
  
   register uint32                     M1 = 0;
   register LOG_UMTS_CS_DATA_TX_C_type *log_ptr;
   register uint16                     m_bits = 0;
   register uint16                     tmp_hdr_frame_info;   
   
   ASSERT( NULL != radio_tx_data );   
   
   /* Allocate the log buffer, TODO: subtract 1 from the frame size to make up
   ** for the data array of 1 element in the declaration?  Logging
   ** funkiness.
   */              
   log_ptr = (LOG_UMTS_CS_DATA_TX_C_type *)log_alloc(LOG_UMTS_CS_DATA_TX_C, 
                sizeof (LOG_UMTS_CS_DATA_TX_C_type) + (uint32) frame_size-1);
                
   /* Log the RX data */             
   if (log_ptr != NULL)
   {
      tmp_hdr_frame_info = Log_header_frame_info;
      log_ptr->ucsd_hdr.rates = Log_header_rates;
      log_ptr->ucsd_hdr.reserved = 0;
      
      /* Save log header size */
      
      switch (frame_size)
      {
         case DS_UMTS_CSD_FRAME_SIZE_240_BIT:
            /* 240 bit frame mask is 0's */
            break;
            
         case DS_UMTS_CSD_FRAME_SIZE_288_BIT:            
            tmp_hdr_frame_info |= DS_UCSD_LOG_288_BIT_FRAME_MASK;
      
            /* Log M1 and M2 bits */
            DS_UCSD_GET_M1_M2_BITS (radio_tx_data,M1);
            if (M1)
            {
               m_bits |= OTA_M1_BIT;
            }
            tmp_hdr_frame_info |= m_bits;            
            break;
            
         case DS_UMTS_CSD_FRAME_SIZE_576_BIT:
            tmp_hdr_frame_info |= DS_UCSD_LOG_576_BIT_FRAME_MASK;
            break;
            
         case DS_UMTS_CSD_FRAME_SIZE_640_BIT:
         default:
           tmp_hdr_frame_info |= DS_UCSD_LOG_640_BIT_FRAME_MASK;
           break;
      }        
      
      /* Save the temporary header frame data to log packet */
      log_ptr->ucsd_hdr.frame_info = tmp_hdr_frame_info;
      
      /* Log the rest of the data */
      memscpy ((void *)log_ptr->data, (uint32) frame_size, 
                radio_tx_data, (uint32) frame_size);
      log_commit(log_ptr);
   } 
}

#else /* BOA_APPS */

/* Currently just define empty log functions for BOA_APPS
*/


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_init

DESCRIPTION
   Initializes log header information that should be done prior to 
   starting to do any packet logging.  This sets up the basic log
   packet header information that is present on every log packet.
         
DEPENDENCIES
   None.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Sets some global header variables

===========================================================================*/
void
ds_ucsd_log_init
(
   uint8                   num_data_bits,
   uint8                   num_stop_bits,
   uint8                   num_parity_bits,
   boolean                 transparent_call,
   ds_ucsd_data_rate_T     data_rate,
   sys_sys_mode_e_type     curr_mode,
   uint8                   rlp_version_number    
)
{   
}
   

/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_fax_init

DESCRIPTION
   Initializes log header information that should be done prior to 
   starting to do any packet logging.  This sets up the basic log
   packet header information that is present on every log packet.
         
DEPENDENCIES
   None.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   Sets some global header variables

===========================================================================*/ 
void
ds_ucsd_log_fax_init
(
   ds_ucsd_data_rate_T fax_rate
)
{
}  


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_rx_data

DESCRIPTION
   Logs UMTS Async RX data.  Pointer to the packet to log and frame_size
   are the parameters.  Note that in the DS_UMTS_CSD_FRAME_SIZE_288_BIT
   frame size case that corresponds to GSM 14400, the first 16 bits
   of the data contain the M1 and M2 bits. 
         
DEPENDENCIES
   ds_ucsd_log_init() should be called prior to calling this.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   logs data

===========================================================================*/
void 
ds_ucsd_log_rx_data
(
   uint8                 *radio_rx_data,
   ds_ucsd_frame_size_T  frame_size,
   boolean                fcs_error
)
{
 
}


/* <EJECT> */
/*===========================================================================

FUNCTION  ds_ucsd_log_tx_data

DESCRIPTION
   Logs UMTS Async TX data.  Pointer to the packet to log and frame_size
   are the parameters.  Note that in the DS_UMTS_CSD_FRAME_SIZE_288_BIT
   frame size case that corresponds to GSM 14400, the first 16 bits
   of the data contain the M1 and M2 bits. 
         
DEPENDENCIES
   ds_ucsd_log_init() should be called prior to calling this.
  
RETURN VALUE
   None
    
SIDE EFFECTS
   logs data

===========================================================================*/
void 
ds_ucsd_log_tx_data
(
   uint8                 *radio_tx_data,
   ds_ucsd_frame_size_T  frame_size
)
{
}

#endif /* BOA_APPS */

#endif /* #if defined(FEATURE_DATA_GCSD) || defined (FEATURE_DATA_WCDMA_CS) */

