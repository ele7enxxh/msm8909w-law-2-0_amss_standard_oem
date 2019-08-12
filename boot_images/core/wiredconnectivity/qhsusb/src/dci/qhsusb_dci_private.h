#ifndef QHSUSB_DCI_PRIVATE_H
#define QHSUSB_DCI_PRIVATE_H

/*=======================================================================*//**
 * @file        qhsusb_dci_private.h
 * @author:     amirs
 * @date        14-July-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) Private defines, types and API.
 *
 * @details     The Chip-Idea USB Hardware is managed via descriptors called dQH and dTD.
 *              The dQH describes some of the endpoint configuration
 *              The dTD describes the transfer parameters.
 *              
 * @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *              Chapter 7 Device Data Structures
 *
 *              Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/

//===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dci/qhsusb_dci_private.h#1 $ $DateTime: 2015/09/01 00:30:35 $ $Author: pwbldsvc $
// 
// when       who      what, where, why
// --------   ---      ----------------------------------------------------------
// 07/14/08   amirs    First Draft
// 08/08/08   amirs    First working DLOAD process
// 08/11/08   amirs    Add files and functions headers according to coding standard.
// 08/30/10   dpatnaik Port QHSUSB to RPM, add ULPI based charger detection
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"     // common defines and types
#include "qhsusb_log.h"

//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

//+++++++++++++++++++++++++++++++++
// DCI PRIVATE FLAGS
//+++++++++++++++++++++++++++++++++

// This flag should be enabled during development and disable for release
// to reduce the code size.
//#define QHSUSB_DEBUG_DCI_ENABLE

void qhsusb_loop_here(void); 

#ifdef QHSUSB_DEBUG_DCI_ENABLE
    #define DCI_ASSERT(val) dci_assert(val,__FILE__,__LINE__)
    #define DCI_INC_DEBUG_COUNTER(x) x++
    #define DCI_CHECK_CORE_ID(core_id) \
            if (core_id >= DCI_MAX_CORES)     \
            {                                 \
                DCI_ASSERT(QHSUSB_ERR_BAD_PARAM); \
                return(QHSUSB_ERR_BAD_PARAM); \
            }
#else
    #define DCI_ASSERT(val)   qhsusb_loop_here() 
    #define DCI_INC_DEBUG_COUNTER(x)
    #define DCI_CHECK_CORE_ID(core_id) \
            if(core_id >= DCI_MAX_CORES) \
            {                             \
                qhsusb_log(QHSUSB_CORE_ID_ERROR_LOG, 0, __LINE__); \
                DCI_ASSERT(QHSUSB_ERR_BAD_PARAM); \
            }     
#endif

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

#define DCI_MAX_CORES               1 

// DCI Error Codes
#define DCI_OK                      0

#define DCI_SETUP_BUFFER_SIZE       8

#define DCI_MAX_ENDPOINTS           2
#define DCI_MAX_TDS                 64
#define DCI_MAX_DIRECTION           2  // RX=OUT=0,TX=IN=1
#define DCI_EP0_MAX_PACKET_SIZE     64

#define DCI_EP0                     0

#define DIR_RX                      0 // OUT Token
#define DIR_TX                      1 // IN Token

#define DCI_ZLP_TD                  1

// dQH defines for bitmask and shif
#define DQH_ZLT_BMSK                (1<<29)
#define DQH_IOS_BMSK                (1<<15)
#define DQH_MAX_PACKET_LEN_SHFT     16

// dTD defines for bitmask and shif
#define DTD_TERMINATE_BMSK          0x001
#define DTD_IOC_BMSK                (1<<15)

#define DTD_STATUS_BMSK                      0xFF   
#define DTD_STATUS_ACTIVE_BMSK               (1<<7)
#define DTD_STATUS_HALTED_BMSK               (1<<6)
#define DTD_STATUS_DATA_BUFFER_ERROR_BMSK    (1<<5)
#define DTD_STATUS_TRANSACTION_ERROR_BMSK    (1<<3)

#define DTD_TOTAL_BYTES_SHFT        16

#define DTD_MAX_TRANSFER_16KB   (16*1024)

#define USB_REG_ID_VALUE    0x0042FA05

#define VAL_TO_BOOL(x)  ((boolean)((x) != 0))

#define DCI_MAX_TIMERS 2

#define QHSUSB_COOKIE_SINGLE_ENUM_UNINIT_VALUE 0xFFFFFFFF

// -----------------------------------------------------------------
// ULPI Registers accessed via the Chip-Idea ULPI View register (0x170)
// This register set is for the 45nm 
// -----------------------------------------------------------------
typedef enum
{
    ULPI_REG_VENDOR_ID              = 0x00,
    ULPI_REG_PRODUCT_ID             = 0x02,
    ULPI_REG_FUNCTION_CONTROL       = 0x04,
    ULPI_REG_INTERFACE_CONTROL      = 0x07,
    ULPI_REG_OTG_CONTROL            = 0x0A,
    ULPI_REG_INTERRUPT_RISE_ENABLE  = 0x0D,
    ULPI_REG_INTERRUPT_FALL_ENABLE  = 0x10,
    ULPI_REG_INTERRUPT_STATUS       = 0x13,
    ULPI_REG_INTERRUPT_LATCH        = 0x14,
    ULPI_REG_DEBUG                  = 0x15,
    ULPI_REG_PHY_CONFIG1            = 0x30,
    ULPI_REG_PHY_CONFIG2            = 0x31,
    ULPI_REG_PHY_CONFIG3            = 0x32,
    ULPI_REG_PHY_CONFIG4            = 0x33,
    ULPI_REG_CHGDET_CONTROL         = 0x34
} ulpi_reg_t;

typedef enum
{
    ULPI_MODE_DIRECT        = 0,
    ULPI_MODE_SET_BIT       = 1,
    ULPI_MODE_CLEAR_BIT     = 2
} ulpi_mode_t;

// Simple Read/Write register
#define DCI_HWIO_IN(core, usb_reg)                      in_dword((usb_reg))
#define DCI_HWIO_OUT(core, usb_reg, val)                out_dword((usb_reg),(val))
// Read/Write register with Mask
#define DCI_HWIO_INM(core, usb_reg, mask)               ( DCI_HWIO_IN((core), (usb_reg)) & (mask))
#define DCI_HWIO_OUTM(core, usb_reg, mask, val)         DCI_HWIO_OUT((core), (usb_reg), ((DCI_HWIO_IN((core), (usb_reg)) & (~(mask))) | ((val) & (mask))))
// Read/Write register with Index
#define DCI_HWIO_OUTMI(core, usb_reg, index, mask, val) DCI_HWIO_OUTM((core), ((usb_reg)+(index)*4), (mask), (val))
#define DCI_HWIO_INI(core, usb_reg, index)              DCI_HWIO_IN((core), ((usb_reg)+(index)*4))

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

/** @struct dTD = Device-Transfer-Descriptor , Total 32 bytes
 * @note should be 32 byte aligned
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 7.2 Endpoint Transfer Descriptor (dTD)
 *  
 */
typedef  volatile struct dTD_Tag
{
   uint32   next_dtd_ptr;        // Next dTD Pointer (32 byte aligned) + Terminate bit 
   // Note: avoid bit-fields-struct for multiple-fields 32 bits
   uint32   size_ioc_mult_sts;    // Total bytes (16..30), IOC (15), MULT (10..11), STS (0-7) 
                                          
   uint32   buffer0_ptr;         // Buffer pointer Page 0 (12-31) 
   uint32   buffer1_ptr;         // Buffer pointer Page 1 (12-31) 
   uint32   buffer2_ptr;         // Buffer pointer Page 2 (12-31) 
   uint32   buffer3_ptr;         // Buffer pointer Page 3 (12-31) 
   uint32   buffer4_ptr;         // Buffer pointer Page 4 (12-31) 
                                   
   uint32  urb_ptr;             // Points to software management info, in case there are several TDs, urb_ptr will be stored in TD0.
} dTD_t; 

/** @struct dQH = Device-Queue-Head , Total 64 bytes
 * @note should be 64 byte aligned
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 7.1 Endpoint Queue Head (dQH)
 *  
 */
typedef volatile struct 
{
   // Note: avoid bit-fields-struct for multiple-fields 32 bits
   uint32   mult_zlt_packetlen_ios;    // Mult (31..30), Zlt (29), max_packet_length (26..16), ios (15)
                                        
   uint32   current_dtd_ptr;           // current dtd pointer 

   // overlay transfer = Shadow copied by the USB Hardware from the current dTD.
   uint32   next_dtd_ptr;        // Next dTD Pointer (32 byte aligned) + Terminate bit 
   // Note: avoid bit-fields-struct for multiple-fields 32 bits
   uint32   size_ioc_int_sts;    // Total bytes (16..30), IOC (15), INT (8), STS (0-7) 

   uint32   buffer0_ptr;           // Buffer pointer Page 0 (12-31) + offset (0..11) = full address.
   uint32   buffer1_ptr;           // Buffer pointer Page 1 (12-31) 
   uint32   buffer2_ptr;           // Buffer pointer Page 2 (12-31) 
   uint32   buffer3_ptr;           // Buffer pointer Page 3 (12-31) 
   uint32   buffer4_ptr;           // Buffer pointer Page 4 (12-31) 
   // end of transfer_overlay                                   

   uint32   reserved1;
   uint8    setup_buffer[8];        // 8 bytes of setup data that follows the setup pid 
   uint32   reserved2[4];           // for 64 bytes alignment
} dQH_t;


typedef enum
{
    DCI_PHY_TYPE_PMIC_3_WIRE    = 1,
    DCI_PHY_TYPE_ULPI           = 2,
    DCI_PHY_TYPE_FSIC           = 3,
    DCI_PHY_TYPE_HSIC           = 4,

    // TBD

    DCI_PHY_TYPE_MAX
} phy_type_t;

struct qhsusb_dcd_dsc_device;

// Note: For Multi-Core Support
typedef struct 
{
    uint32 base_address;
    phy_type_t phy_type;
    struct qhsusb_dcd_dsc_device*  dcd_context_ptr;    // For DCD internal use (ask by Vladimir)
} core_info_t;

typedef enum
{
    QHSUSB_CHG_PHY_MODE_NORMAL,
    QHSUSB_CHG_PHY_MODE_NONDRIVE
} qhsusb_phy_mode_type;
//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------
void dci_assert(uint32 val,const char* filename_ptr,int line);
 
//----------------------------------------------------------------------------
// Target Specific Function Declarations and Documentation
//----------------------------------------------------------------------------
void dci_init_phy(uint32 core_id);
boolean dci_is_streaming_mode_disable_required(void);
void dci_dcache_inval(uint32 *addr, uint32 length); /* DCache Invalidate */
void dci_dcache_flush(uint32 *addr, uint32 length); /* DCache Clean & Invalidate */


//=============================================================================
/**
 * Detects charger port type
 * 
 * Should be called after usb stack initialization
 * 
 * @param core_id
 *        
 *        
 *  
 * @return Charger port type as a raw uint32.
 */
//=============================================================================
uint32 dci_detect_charger_type(uint32 core_id);
// ------------------------------------------------------------------------------------------------
// Global Variables
// ------------------------------------------------------------------------------------------------

extern uint32 dci_core_base_address[DCI_MAX_CORES];
extern core_info_t dci_core_info[DCI_MAX_CORES];


#endif // QHSUSB_DCI_PRIVATE_H
