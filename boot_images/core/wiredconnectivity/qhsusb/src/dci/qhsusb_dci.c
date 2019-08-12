//============================================================================
/**
 * @file        qhsusb_dci.c
 * @author      amirs
 * @date        14-July-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) implementation.
 *
 * @details     The QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) 
 *              acts like a HAL (Hardware Abtraction Layer) for Chip-Idea USB Hardware.
 *              The DCI should supply generic API that hides USB Hardware registers access from the upper layers.
 *              It should not be aware of Chapter-9 or specific function drivers (Mass-Storage , CDC , HID etc).
 *              It should be simple without any management algorithms.
 *              
 *              The USB Device Core Driver (DCD) shall use the DCI to implement
 *              the USB protocol chapter 9 enumeration,
 *              and handling of special BUS signaling : Reset / Suspend / Resume.
 * 
 * @note        This DCI is implementing the ChipIdea hardware core.
 *              The DCI depends on the USB Core (a.k.a LINK) connection to the PHY:
 *              When a 3-wired PHY is connected (Full-Speed only) then the LINK
 *              can not control the VBUS D+ wire via the Run/Stop bit.
 * 
 * @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *
 *              Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
 */
//============================================================================

// ===========================================================================  
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/wiredconnectivity/qhsusb/src/dci/qhsusb_dci.c#2 $$DateTime: 2016/03/08 23:08:52 $$Author: pwbldsvc $
// 
// when       who      what, where, why
// --------   ---      ----------------------------------------------------------
// 07/11/11   dpatnaik Updated Initialization sequence as per HPG guidelines.
// 08/30/10   dpatnaik Port QHSUSB to RPM, add ULPI based charger detection
// 03/17/09   dhaval   Removed compiler warnings.
// 07/14/08   amirs    First Draft
// 08/08/08   amirs    First working DLOAD process
// 08/11/08   amirs    Add files and functions headers according to coding standard.
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "busywait.h"               // busywait APIs
#include "comdef.h"                 // common defines - basic types as uint32 etc
#include "qhsusb_dci_api.h"         // DCI API Prototypes
#include "qhsusb_dci_private.h"     // dQH_t
#include "qhsusb_urb.h"             // URB structure - qhsusb_urb_t
#include "qhsusb_ch9.h"                                    // 
#include "qhsusb_dci_hw_registers.h"    // For Multi-Core
#include "msmhwio.h"
#include "qhsusb_dcd.h"
#ifdef FEATURE_QHSUSB_MS
#include "qhsusb_fd_ms.h"
#endif /* FEATURE_QHSUSB_MS */
#include "qhsusb_log.h"
#include <stringl/stringl.h>
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
/** All new Badger family will not get VBus Directly**/
#include "pm.h"
#endif
//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
// None

//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------
// None

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------

static qhsusb_dci_callback_table_t *dci_callback_table_ptr = NULL;
static uint8 dci_setup_buffer[DCI_SETUP_BUFFER_SIZE];  // 8 bytes buffer

// @note dQH and dTD are access by hardware and therefore volatile
__align(2*1024) // Queue-Head MUST be 2K aligned
static dQH_t dci_dqh_list[DCI_MAX_ENDPOINTS][DCI_MAX_DIRECTION];  // 2 ep * 2 dir
__align(32) // dTD MUST be 32 bytes aligned
static dTD_t dci_dtd_list[DCI_MAX_ENDPOINTS][DCI_MAX_DIRECTION][DCI_MAX_TDS+DCI_ZLP_TD];  // 2 ep * 2 dir * TDs, extra one for TX ZLP

static uint32 dci_max_packet_size = 0;
static uint32 dci_dummy_buffer_for_zlp = 0;
static boolean is_bulk_mode_in_progress=FALSE; 

#ifdef QHSUSB_DEBUG_DCI_ENABLE
#pragma arm section rwdata = "USB_DEBUG_DATA", zidata = "USB_DEBUG_DATA"
    static uint32 dci_assert_val=0;
    static const char *dci_assert_filename_ptr=0;
    static int dci_assert_line=0;

// debug counters for events
    static uint32 dci_handle_setup_counter=0;
    static uint32 dci_handle_system_error_counter=0;
    static uint32 dci_handle_usb_error_counter=0;
    
    static uint32 dci_handle_port_change_counter=0;
    static uint32 dci_handle_usb_reset_counter=0;
    static uint32 dci_handle_suspend_counter=0;
    
    static uint32 dci_handle_transfer_complete_counter=0;
    
    static uint32 dci_transfer_request_counter=0;
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
	static uint32 dci_handle_usb_disconnect_counter = 0;
#endif

#pragma arm section rwdata, zidata 
#endif // QHSUSB_DEBUG_DCI_ENABLE

static boolean dci_timer_is_running[DCI_MAX_TIMERS] = {FALSE,FALSE};

static uint8  dci_usb_address=0;

static boolean  dci_start_enum=FALSE;

static boolean is_first_TD_occupied = FALSE;
static uint32  zero_address_tx_length = 0x0;

//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------
static void dci_init_dqh_list(uint32 core_id);
// Events handlers
static void dci_handle_setup(uint32 core_id);
static void dci_handle_transfer_complete(uint32 core_id);
static void dci_handle_usb_reset(uint32 core_id);
static void dci_handle_port_status_changed(uint32 core_id);
static void dci_handle_suspend(uint32 core_id);
static uint32 qhsusb_dci_get_max_packet_size(void);
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
static void dci_handle_usb_disconnect(uint32 core_id);
static void dci_handle_reset_during_transfer(uint32 core_id); 
#endif

//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------

// ===========================================================================
/**
 * @function    qhsusb_dci_enable_test_mode
 * 
 * @brief   Enables the USB test mode.
 * 
 * @note   
 *  
 * @param   
 *
 * @return 
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.15 PORTSCx
 *      Chapter 3.6.15.1 Device Controller
 *
 */
// ===========================================================================

void qhsusb_dci_enable_test_mode(struct qhsusb_dcd_dsc_device* dev)
{ 
   switch(dev->test_mode) 
   {
      case USB_PORT_TEST_J_STATE:
      case USB_PORT_TEST_K_STATE:
      case USB_PORT_TEST_SE0_NAK:
      case USB_PORT_TEST_PACKET:
      case USB_PORT_TEST_FORCE_ENABLE:
      DCI_HWIO_OUTM(dev->core_id, USB_OTG_HS_PORTSC, USB_OTG_HS_PORTSC_PTC_BMSK, (dev->test_mode << USB_OTG_HS_PORTSC_PTC_SHFT));

      break;

      case USB_PORT_TEST_DISABLE:	
      DCI_HWIO_OUTM(dev->core_id, USB_OTG_HS_PORTSC, USB_OTG_HS_PORTSC_PTC_BMSK, (dev->test_mode << USB_OTG_HS_PORTSC_PTC_SHFT));
      dev->qhsusb_test_mode_enabled = FALSE;

      break;	  

      default:
      qhsusb_log(USB_TEST_MODE_INVALID_LOG, 0, 0);
      break;
  }	   
}



//============================================================================
/**
 * @function  qhsusb_dci_init
 *
 * @brief
 *    This function initializes the USB device controller according to the ChipIdea spec:
 *    1. Set Controller Mode in the USBMODE register to device mode.
 *    2. Allocate and Initialize device queue heads in system memory.
 *    3. Configure ENDPOINTLISTADDR Pointer.
 *    4. Enable the microprocessor interrupt associated with the USBHS
 *    5. Set Run/Stop bit to Run Mode.
 *
 *  @Note : Endpoint 0 is designed as a control endpoint only and does not need to be
 *      configured using ENDPTCTRL0 register.
 *      It is also not necessary to initially prime Endpoint 0
 *      because the first packet received will always be a setup packet. 
 *      The contents of the first setup packet will require a response
 *      in accordance with USB device framework (Chapter 9) command set.
 *
 * @param core_id    
 *
 * @param dcd_context_ptr    
 *              A pointer to a DCD initernal context
 * @param callback_table_ptr
 *              A pointer to a callback table
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 8.1 "Device Controller Initialization"
 *      Chapter 8.4.3.1.1 Setup Packet Handling (Pre2.3 hardware)
 *
 */
//============================================================================
uint32 qhsusb_dci_init(uint32 core_id, struct qhsusb_dcd_dsc_device* dcd_context_ptr, 
                       qhsusb_dci_callback_table_t *callback_table_ptr)
{
    uint32 status=DCI_OK;

    DCI_CHECK_CORE_ID(core_id);

    //  Save the Callbcak Table Pointer and DCD context
    dci_callback_table_ptr = callback_table_ptr;
    dci_core_info[core_id].dcd_context_ptr = dcd_context_ptr;

#ifdef FEATURE_QHSUSB_PBL
    {
    
    // Init the PHY
      dci_init_phy(core_id);  
    }
#else
    {
      if (qhsusb_dci_clk_is_usb_system_clk_on())
      {
        /* most use cases USB clocks are already turned on by PBL */
        if (!dci_skip_enumeration(core_id))
        {
          // Init the PHY
          dci_init_phy(core_id);  
        }
      }
      else
      {
        /* This would be SBL2 DLOAD after crash scenario.
         * The agreed pre-condition is to always re-enumerate */
        dci_init_phy(core_id);
      }
    }
#endif  // FEATURE_QHSUSB_PBL

    // 1 Set Controller Mode in the USBMODE register to device mode.
    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBMODE,USB_OTG_HS_USBMODE_CM_BMSK,USB_OTG_HS_USBMODE_CM_DEVICE_CONTROLLER_DEFAULT_FOR_DEVICE_ONLY_CONTROLLER_FVAL << USB_OTG_HS_USBMODE_CM_SHFT); 

    // 2. Allocate and Initialize device queue heads in system memory.
    dci_init_dqh_list(core_id);

    // 3. Configure ENDPOINTLISTADDR Pointer.
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPOINTLISTADDR,(uint32)dci_dqh_list);

    // 4. Enable the microprocessor interrupt associated with the USBHS
    // From 3.6.3 USBINTR : 
    //      The USB Status register (USBSTS) still shows interrupt sources even if they are
    //      disabled by the USBINTR register, allowing polling of interrupt events by the software.
    DCI_HWIO_OUT(core_id,USB_OTG_HS_USBINTR,0);   // disable interrupts  

    if(dci_is_streaming_mode_disable_required())
	{
	    // Disable streaming mode for inverted CRC issues observed on some hosts.
        DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBMODE,USB_OTG_HS_USBMODE_SDIS_BMSK, 1 << USB_OTG_HS_USBMODE_SDIS_SHFT);
	}
     
    // Use SETUP TripWire instead of Lockout mechanism
    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBMODE,USB_OTG_HS_USBMODE_SLOM_BMSK, USB_OTG_HS_USBMODE_SLOM_SETUP_LOCKOUTS_OFF_FVAL << USB_OTG_HS_USBMODE_SLOM_SHFT);
 

    // Select PHY Type (PORTSC reg) - see dci_init_phy()

    // Make sure the 16 MSBs of this register are 0s (CI Ref code)
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENPDTSETUPSTAT,0);

    // Leave AHB2AHB Bypass Disabled, change AHB HPROT
    //  When “10” the HPROT signal out of the USB Wrapper
    //  alternates according to the context of the AHB bus access.
    //  Control structures are non-posted while data transfer is posted.
    DCI_HWIO_OUT(core_id, USB_OTG_HS_AHB_MODE, USB_OTG_HS_AHB_MODE_HPROT_ONLY_CTRL_XFER_POSTED_FVAL << USB_OTG_HS_AHB_MODE_HPROT_MODE_SHFT);
    
    // 5. Set Run/Stop bit to Run Mode. Dont do this we will set R/S bit after charger detection.
    // Note: This might be not-relevant when using an external PHY.
    // If we are a HID device we will do charger detection after this and set R/S bit.
    if(qhsusb_dci_get_enum_flag() == TRUE)
    {
        DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_RS_BMSK,USB_OTG_HS_USBCMD_RS_RUN_FVAL); 
    }
  
    // =======================================================================
    // NOTE: 
    //      Endpoint#0 dQH is configured in dci_init_dqh_list() and some HW is config by default
    //      It is the upper layers responsibility to call qhsusb_dci_config_endpoint() for non-control endpoints.
    // =======================================================================
    
    // Verify the HW is ok by checking the ID
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    {
        uint32 hwid = DCI_HWIO_IN(core_id,USB_OTG_HS_ID);
    
        if (hwid != USB_REG_ID_VALUE) 
        {
            status = QHSUSB_ERR_HW_FAILURE;
            DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
        }
    }
    #endif // QHSUSB_DEBUG_DCI_ENABLE

    return(status);
}

//============================================================================
/**
 * @function    dci_init_dqh_list
 * @brief       Init the dQH and dTD list with default values.
 *
 * @param core_id    
 *
 * @return  none.
 *
 * @see qhsusb_dci_init
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 8.5.1 Queue Head Initialization
 *
 */
//============================================================================
static void dci_init_dqh_list(uint32 core_id)
{
    int ep,dir,td;

    memset((uint8*)dci_dqh_list,0,sizeof(dci_dqh_list));
    memset((uint8*)dci_dtd_list,0,sizeof(dci_dtd_list));

    // NOTE: dQH and dTD are not relevant before endpoints are primed.
    //       setup packet has deticated 8 bytes buffer in the dQH

    for (ep=0;ep<DCI_MAX_ENDPOINTS;ep++) 
    {
        for (dir=0;dir<DCI_MAX_DIRECTION;dir++) 
        {
          dci_dqh_list[ep][dir].next_dtd_ptr = (uint32) &dci_dtd_list[ep][dir][0];
          for (td = 0; td < DCI_MAX_TDS; td++)
          { /* TDs are not linked at this stage */
            dci_dtd_list[ep][dir][td].next_dtd_ptr = DTD_TERMINATE_BMSK;
          }
        }
    }

    // Endpoint#0 RX max packet length is 64 bytes - for full-speed
    dci_dqh_list[DCI_EP0][DIR_RX].mult_zlt_packetlen_ios =
        (DCI_EP0_MAX_PACKET_SIZE << DQH_MAX_PACKET_LEN_SHFT);
    // Endpoint#0 TX max packet length is 64 bytes - for full-speed
    dci_dqh_list[DCI_EP0][DIR_TX].mult_zlt_packetlen_ios =
        (DCI_EP0_MAX_PACKET_SIZE << DQH_MAX_PACKET_LEN_SHFT);

    // Enable Endpoint#0 IOS ( Interrupt-On-Setup )
    dci_dqh_list[DCI_EP0][DIR_RX].mult_zlt_packetlen_ios |= DQH_IOS_BMSK;
    dci_dcache_flush((void*)dci_dqh_list,sizeof(dci_dqh_list));
    dci_dcache_flush((void*)dci_dtd_list,sizeof(dci_dtd_list));
}


//============================================================================
/**
 * @function qhsusb_dci_poll_events
 * @brief   Check for any pending events (interrupts are disabled) and call the relevant callbacks.
 *
 * @param core_id    
 *
 * @return  none.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 8.4.3.1.2 Setup Packet Handling (2.3 hardware and later)
 *      Chapter 3.6.2 USBSTS
 *
 */
//============================================================================
void qhsusb_dci_poll_events(uint32 core_id)
{
    uint32 usbsts = DCI_HWIO_IN(core_id,USB_OTG_HS_USBSTS);
    uint32 setup_status = DCI_HWIO_IN(core_id,USB_OTG_HS_ENPDTSETUPSTAT);

    // Clear status bits
    DCI_HWIO_OUT(core_id,USB_OTG_HS_USBSTS,usbsts);

    // USB Reset
    // When the device controller detects a USB Reset and enters the default state, 
    // this bit will be set to a one
    if (usbsts & USB_OTG_HS_USBSTS_URI_BMSK) 
    {
        DCI_INC_DEBUG_COUNTER(dci_handle_usb_reset_counter);
		qhsusb_log(QHSUSB_USBSTS_URI_LOG,0,0);
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
        if (TRUE == qhsusb_is_device_configured())
        {
			/*handle the disconnect here*/
            DCI_INC_DEBUG_COUNTER(dci_handle_usb_disconnect_counter);
            /* Handle reset during a transfer error */ 
            dci_handle_reset_during_transfer(core_id); 
			/*Clear the run stop bit*/
			DCI_HWIO_OUTM(core_id, USB_OTG_HS_USBCMD, USB_OTG_HS_USBCMD_RS_BMSK, USB_OTG_HS_USBCMD_RS_STOP_FVAL << USB_OTG_HS_USBCMD_RS_SHFT);
			dci_handle_usb_disconnect(core_id);		
        }
        else
        {   
			/*handle regular reset*/
            dci_handle_usb_reset(core_id);
        }
#else
        dci_handle_usb_reset(core_id);
#endif
    }

    // Device Suspend
    // When a device controller enters a suspend state from an active state, this bit will be set to a one.
    if (usbsts & USB_OTG_HS_USBSTS_SLI_BMSK) 
    {
        DCI_INC_DEBUG_COUNTER(dci_handle_suspend_counter);
		qhsusb_log(QHSUSB_USBSTS_SLI_LOG,0,0);
        dci_handle_suspend(core_id);
    }

    // Port Change Detect = Resume / Speed
    // The Device Controller sets this bit to a one when it detects resume signaling or the port
    // controller enters the full or highspeed operational state. 
    // When the port controller exits the full  or highspeed operation states due to Reset or Suspend events, 
    // the notification mechanisms are the USB Reset Received bit and the DCSuspend bits respectively
    if (usbsts & USB_OTG_HS_USBSTS_PCI_BMSK) 
    {
        DCI_INC_DEBUG_COUNTER(dci_handle_port_change_counter);
		qhsusb_log(QHSUSB_USBSTS_PCI_LOG,0,0);
        dci_handle_port_status_changed(core_id);
    }

    // System Error
    // System Error is currently not implemented in CI controller and is always set to 0x0.
    if (usbsts & USB_OTG_HS_USBSTS_SEI_BMSK) 
    {
        DCI_INC_DEBUG_COUNTER(dci_handle_system_error_counter);
		qhsusb_log(QHSUSB_USBSTS_SEI_LOG,0,0);
	    DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
    }

    // USB Error Interrupt (USBERRINT)
    // When completion of a USB transaction results in an error condition, 
    // this bit is set by the Host/Device Controller. 
    // This bit is set along with the USBINT bit, 
    // if the TD on which the error interrupt occurred also had its interrupt on complete (IOC) bit set
    if (usbsts & USB_OTG_HS_USBSTS_UEI_BMSK) 
    {
        DCI_INC_DEBUG_COUNTER(dci_handle_usb_error_counter);
		qhsusb_log(QHSUSB_USBSTS_UEI_LOG,0,0);
        DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
    }

    // USB Interrupt (USBINT) = Transfer Completed
    // This bit is set by the Host/Device Controller when the cause of an interrupt 
    // is a completion of a USB transaction where the Transfer Descriptor (TD) has an interrupt on complete (IOC) bit set.
    if (usbsts & USB_OTG_HS_USBSTS_UI_BMSK) 
    {
        DCI_INC_DEBUG_COUNTER(dci_handle_transfer_complete_counter);
        dci_handle_transfer_complete(core_id);
    }

    // Setup Received
    if (setup_status & 0x01) 
    {
        DCI_INC_DEBUG_COUNTER(dci_handle_setup_counter);
        dci_handle_setup(core_id);
    }
}

// ===========================================================================
/**
 * @function    dci_handle_setup
 * 
 * @brief       This function handles setup receive event
 * 
 * @details 
 *      After receiving an interrupt and inspecting ENDPTSETUPSTAT to determine that a setup packet was received
 *      on a particular pipe:
 *       1. Write '1' to clear corresponding bit ENDPTSETUPSTAT.
 *       2. Write ‘1’ to Setup Tripwire (SUTW) in USBCMD register.
 *       3. Duplicate contents of dQH.SetupBuffer into local software byte array.
 *       4. Read Setup TripWire (SUTW) in USBCMD register. (if set continue; if cleared goto 2)
 *       5. Write '0' to clear Setup Tripwire (SUTW) in USBCMD register.
 *       6. Process setup packet using local software byte array copy and execute status/handshake phases.
 *       7. Before priming for status/handshake phases ensure that ENDPTSETUPSTAT is ‘0’.
 *      
 *      
 *       A poll loop should be used to wait until ENDPTSETUPSTAT transitions to ‘0’ after step 1) above and
 *      before priming for the status/handshake phases.
 *       In core versions 3.2 and later, the time from writing a ‘1’ to ENDPTSETUPSTAT and reading back a
 *      ‘0’ is very short (~12 us) so a poll loop in the DCD will not be harmful.
 *       In core versions 3.1 and earlier, the time from writing a ‘1’ to ENDPTSETUPSTAT and reading back a
 *      ‘0’ may vary according to the type of traffic on the bus up to nearly a 1ms, however the it is absolutely
 *      necessary to ensure ENDPTSETUPSTAT has transitioned to ‘0’ after step 1) and before priming for the
 *      status/handshake phases.
 *   
 *  @note : After receiving a new setup packet the status and/or handshake phases
 *      may still be pending from a previous control sequence. These should be
 *      flushed & deallocated before linking a new status and/or handshake dTD for
 *      the most recent setup packet
 *   
 *  @note : A setup transfer does not use a dTD 
 *      but instead stores the incoming data from a setup packet in an 8byte buffer within the dQH
 * 
 * @param core_id    
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *     Chapter 8.4.3.1.2 Setup Packet Handling (2.3 hardware and later)
 *     Chapter 8.5.2 Operational Model For Setup Transfers
 *
 */
// ===========================================================================
static void dci_handle_setup(uint32 core_id)
{
    uint32 usbcmd=0;
    uint32 setup_status=0;

    //  1. Write '1' to clear corresponding bit ENDPTSETUPSTAT.
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENPDTSETUPSTAT,0x01);
    do
    {
        setup_status = DCI_HWIO_INM(core_id,USB_OTG_HS_ENPDTSETUPSTAT,0x01);
    } while( setup_status != 0);

    do
    {
        //  2. Write ‘1’ to Setup Tripwire (SUTW) in USBCMD register.
        DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_SUTW_BMSK,1<<USB_OTG_HS_USBCMD_SUTW_SHFT);
        //  3. Duplicate contents of dQH.SetupBuffer into local software byte array.
        dci_dcache_inval((uint32*)dci_dqh_list[0][0].setup_buffer,sizeof(dci_setup_buffer));
        memscpy((uint8*)dci_setup_buffer,sizeof(dci_setup_buffer),(uint8*)dci_dqh_list[0][0].setup_buffer,sizeof(dci_setup_buffer));
        //  4. Read Setup TripWire (SUTW) in USBCMD register. (if set continue; if cleared goto 2)
        usbcmd = DCI_HWIO_INM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_SUTW_BMSK);
    } while(usbcmd == 0);
    
    //  5. Write '0' to clear Setup Tripwire (SUTW) in USBCMD register.
    DCI_HWIO_OUTM(core_id,USB_OTG_HS_USBCMD,USB_OTG_HS_USBCMD_SUTW_BMSK,0 << USB_OTG_HS_USBCMD_SUTW_SHFT);
    //  6. Process setup packet using local software byte array copy and execute status/handshake phases.
    //  7. Before priming for status/handshake phases ensure that ENDPTSETUPSTAT is ‘0’.

    // Call the DCD callback
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    if(dci_callback_table_ptr->setup_callback_fn != NULL) 
    #endif // QHSUSB_DEBUG_DCI_ENABLE
    {
       /**
        * VK: @todo get actual core index
        */
       uint32 core_index = core_id;
       void *dcd_context_ptr = dci_core_info[core_index].dcd_context_ptr;

       dci_callback_table_ptr->setup_callback_fn(dcd_context_ptr, dci_setup_buffer);
    }
}

// ===========================================================================
/**
 * @function    dci_handle_transfer_complete
 * 
 * @brief       This function handles transfer complete event.
 * 
 * @details     
 *      After a dTD has been initialized and the associated endpoint primed 
 *      the device controller will execute the transfer upon the hostinitiated request. 
 *      The DCD will be notified with a USB interrupt if the Interrupt On Complete bit was set or alternately,
 *      the DCD can poll the endpoint complete register to find when the dTD had been executed.
 *      
 *      After a dTD has been executed, DCD can check the status bits to determine success or failure
 * 
 * @param core_id    
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 8.6.4 Transfer Completion
 *      Chapter 3.6.22 ENDPTCOMPLETE
 */
// ===========================================================================
static void dci_handle_transfer_complete(uint32 core_id)
{
    uint8  ep;
    uint8  dir;
    uint32 ep_complete = 0;
    dQH_t *dqh_ptr = NULL;
    dTD_t *dtd_ptr = NULL;

    uint32 remain_bytes = 0;
    uint32 status = 0;

    qhsusb_urb_t    *urb_ptr;

    ep_complete = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTCOMPLETE);

    // Clear the indication for the next transfers
    // Note: The variable ep_complete holds the current status
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTCOMPLETE,ep_complete);

    for(ep=0;ep<DCI_MAX_ENDPOINTS;ep++) 
    {
        for(dir=DIR_RX;dir<=DIR_TX;dir++)
        {
            uint32 mask = (1<<(ep+dir*USB_OTG_HS_ENDPTCOMPLETE_ETCE_SHFT));
            if(ep_complete & mask )
            {
                dqh_ptr = &dci_dqh_list[ep][dir];
                dci_dcache_inval((void*)dqh_ptr,sizeof(*dqh_ptr));

                // 1. Extract info from dQH and dTD
                dtd_ptr = &dci_dtd_list[ep][dir][0];
                status = 0;
                remain_bytes = 0;
				
                do
                {
                  dci_dcache_inval((void*)dtd_ptr,sizeof(*dtd_ptr));
                  remain_bytes += (dtd_ptr->size_ioc_mult_sts) >> DTD_TOTAL_BYTES_SHFT;
                  if (status == 0)
                  {
                    status =  (dtd_ptr->size_ioc_mult_sts) & DTD_STATUS_BMSK;
                  }
                  if (dtd_ptr->next_dtd_ptr != DTD_TERMINATE_BMSK)
                  {
                    dtd_ptr = (dTD_t*)dtd_ptr->next_dtd_ptr;
                  }
                  else
                  {
                   dtd_ptr = NULL;
                  }
                }while (dtd_ptr);

                // 2. Update URB
                urb_ptr = (void*)(dci_dtd_list[ep][dir][0].urb_ptr);
                urb_ptr->actual_length = urb_ptr->transfer_length - remain_bytes; // completed on ok
                dci_dcache_inval((uint32*)urb_ptr->transfer_buffer_ptr,urb_ptr->actual_length);

                if (status == 0 ) 
                {
                    urb_ptr->transfer_status = QHSUSB_URB_STATUS_COMPLETE_OK;
				    				
                    // 3. call the URB callbacks
                    if(urb_ptr->complete_callback != NULL) 
                    {
                        urb_ptr->complete_callback(urb_ptr);
                    }

                    if(dci_core_info[core_id].dcd_context_ptr->qhsusb_test_mode_enabled == TRUE)
                    {
                        qhsusb_dci_enable_test_mode(dci_core_info[core_id].dcd_context_ptr);
                    }	
                }
                else if (status == 0x80) 
                {
                    urb_ptr->transfer_status = QHSUSB_URB_STATUS_ACTIVE;
                }
                else 
                {
                    urb_ptr->transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
                      // 3. call the URB callbacks
                    if(urb_ptr->complete_callback != NULL) 
                    {
                        urb_ptr->complete_callback(urb_ptr);
                    }
                }
            } // complete detected
         } // for dir
    } // for ep
}



// ===========================================================================
/**
 * @function    dci_handle_usb_reset
 * 
 * @brief       This function handles USB Reset event.
 * 
 * @details     
 *      A bus reset is used by the host to initialize downstream devices. 
 *      When a bus reset is detected, the DEVICE CONTROLLER will:
 *       1. Renegotiate its attachment speed, 
 *       2. Reset the device address to 0, 
 *       3. Notify the DCD by interrupt (assuming the USB Reset Interrupt Enable is set). 
 *      
 *      After a reset is received, all endpoints (except endpoint 0) are disabled 
 *      and any primed transactions will be canceled by the device controller. 
 *      
 *      The DCD must perform the following tasks when a reset is received:
 *       1. Clear all setup token semaphores by reading the ENDPTSETUPSTAT register and writing the same value back to the ENDPTSETUPSTAT register.
 *       2. Clear all the endpoint complete status bits by reading the ENDPTCOMPLETE register and writing the same value back to the ENDPTCOMPLETE register.
 *       3. Cancel all primed status by waiting until all bits in the ENDPTPRIME are 0 and then writing 0xFFFFFFFF to ENDPTFLUSH.
 *       4. Read the reset bit in the PORTSCx register and make sure that it is still active. 
 *       5. Free all allocated dTDs because they will no longer be executed by the device controller. 
 * 
 * @param core_id    
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *       Chapter 8.2.1 Bus Reset
 */
// ===========================================================================
static void dci_handle_usb_reset(uint32 core_id)
{

    // Call DCD callback
    uint32 core_index = core_id;
    void *dcd_context_ptr = dci_core_info[core_index].dcd_context_ptr;

	  qhsusb_log(DCI_HANDLE_USB_RESET_LOG,0,0);

    dci_callback_table_ptr->usb_reset_callback_fn(dcd_context_ptr);

}
#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
// ===========================================================================
/**
 * @function    dci_handle_usb_disconnect
 * 
 * @brief       This function handles USB device disconnect
 * 
 * @details     This function should clear all the pending transfers, and unconfigure
 *				the USB. 
 * 
 * @param 		core_id    
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *       Chapter 3.6.16 PORTSCx
 */
// ===========================================================================
static void dci_handle_usb_disconnect(uint32 core_id)
{
	uint32 core_index = core_id;
    void *dcd_context_ptr = dci_core_info[core_index].dcd_context_ptr;
	dci_callback_table_ptr->usb_disconnect_callback_fn(dcd_context_ptr);
} 
#endif

// ===========================================================================
/**
 * @function    dci_handle_port_status_changed
 * 
 * @brief       This function handles USB port status changed.
 * 
 * @details     This function notify the DCD about USB-Resume signal or Speed-Detection.
 * 
 * @param core_id    
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *       Chapter 3.6.16 PORTSCx
 */
// ===========================================================================
static void dci_handle_port_status_changed(uint32 core_id)
{
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    if(dci_callback_table_ptr->port_status_changed_callback_fn != NULL) 
    #endif // QHSUSB_DEBUG_DCI_ENABLE
    {
       uint32 core_index = core_id;
       void *dcd_context_ptr = dci_core_info[core_index].dcd_context_ptr;

       dci_callback_table_ptr->port_status_changed_callback_fn(dcd_context_ptr);
    }
}

// ===========================================================================
/**
 * @function    dci_handle_suspend
 * 
 * @brief       This function handles USB suspend event
 * 
 * @details     This function notify the DCD.
 * 
 * @param core_id    
 *
 * @return  none.
 *
 * @see qhsusb_dci_poll_events
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *       Chapter 3.6.16 PORTSCx
 */
// ===========================================================================
static void dci_handle_suspend(uint32 core_id)
{
    // Notify DCD
    dci_handle_port_status_changed(core_id);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_set_address
 * 
 * @brief       This function set the device address
 * 
 * @details     
 *      USBADRA Device Address Advance. Default=0. 
 *      When this bit is ‘0’, any writes to USBADR are instantaneous. 
 *      When this bit is written to a ‘1’ at the same time or before USBADR is written, 
 *      the write to the USBADR field is staged and held in a hidden register. 
 *      After an IN occurs on endpoint 0 and is ACKed, USBADR will be loaded from the holding register.
 *      
 *      Hardware will automatically clear this bit on the following conditions:
 *      1) IN is ACKed to endpoint 0. (USBADR is updated from staging register).
 *      2) OUT/SETUP occur to endpoint 0. (USBADR is not updated).
 *      3) Device Reset occurs (USBADR is reset to 0).
 *      
 *      Note: After the status phase of the SET_ADDRESS descriptor, the DCD has 2 ms to program the USBADR field. 
 *      This mechanism will ensure this specification is met when the DCD can not write of the device address within 2ms from the SET_ADDRESS status
 *      phase. If the DCD writes the USBADR with USBADRA=1 after the SET_ADDRESS data phase (before the prime of the status phase), 
 *      the USBADR will be programmed instantly at the correct time and meet the 2ms USB requirement
 * 
 * @param core_id    
 *
 * @param address
 *      New Device Address.
 * 
 * @return  none.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 3.6.6.2 Device Controller (USB DEVICEADDR)
 */
// ===========================================================================
void qhsusb_dci_set_address(uint32 core_id, uint8 address)
{
    uint32 val=0;

    //DCI_CHECK_CORE_ID(core_id);

    val = (address << USB_OTG_HS_DEVICEADDR_USBADR_SHFT) | USB_OTG_HS_DEVICEADDR_USBADRA_BMSK;
    DCI_HWIO_OUT(core_id,USB_OTG_HS_DEVICEADDR,val);

    if (dci_usb_address==0)
    {
      /* debug code */
      /* this was added to remove warning issued by compiler
         as this variable set but not used anywhere in code */
    }

    dci_usb_address = address; // debug
}

// ===========================================================================
/**
 * @function    qhsusb_dci_stall_endpoint
 * 
 * @brief       This function stall an endpoint
 * 
 * @details     
 *      There are two occasions where the device controller may need to return to the host a STALL.
 *  
 *      The first occasion is the FUNCTIONAL stall, 
 *      which is a condition set by the DCD as described in the USB 2.0 device framework (chapter 9). 
 *      A functional stall is only used on noncontrol endpoints and can be enabled in the device
 *      controller by setting the endpoint stall bit in the ENDPTCTRLx register 
 *      associated with the given endpoint and the given direction. 
 *      
 *      In a functional stall condition, the device controller will continue to return STALL responses to all
 *      transactions occurring on the respective endpoint and direction until the endpoint stall bit is cleared by the DCD.
 *      
 *      A PROTOCOL stall, unlike a function stall, is used on control endpoints is automatically cleared by the device
 *      controller at the start of a new control transaction (setup phase). 
 *      When enabling a protocol stall, the DCD should enable the stall bits (both directions) as a pair. 
 *      A single write to the ENDPTCTRLx register can ensure that both stall bits are set at the same instant.
 *  
 * @note : Any write to the ENDPTCTRLx register during operational mode must preserve the endpoint type field (i.e. perform a readmodifywrite).
 *  
 * @param core_id    
 *
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *   Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *   Chapter 8.3.2 Stalling
 */
// ===========================================================================
uint32 qhsusb_dci_stall_endpoint(uint32 core_id, uint8 ep, uint8 dir)
{
    uint32 status=DCI_OK;
    uint32 mask=0;

    DCI_CHECK_CORE_ID(core_id);
    if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
    {
        /* No need to error out. Just don't perform the STALL*/
        return(status); 
    }

    if (ep == 0) 
    {
        mask =  USB_OTG_HS_ENDPTCTRLn_TXS_BMSK     // TXS = TX Endpoint Stall 
              | USB_OTG_HS_ENDPTCTRLn_RXS_BMSK;    // RXS = RX Endpoint Stall 
    }
    else
    {
        if (dir) 
            mask = USB_OTG_HS_ENDPTCTRLn_TXS_BMSK; // TXS = TX Endpoint Stall 
        else                                                                       
            mask = USB_OTG_HS_ENDPTCTRLn_RXS_BMSK; // RXS = RX Endpoint Stall 
    
    }

    DCI_HWIO_OUTMI(core_id,USB_OTG_HS_ENDPTCTRLn,ep,mask,mask); 

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_unstall_endpoint
 * 
 * @brief       This function unstall a stalled endpoint
 * 
 * @details
 *      There are two occasions where the device controller may need to return to the host a STALL.
 *  
 *      The first occasion is the FUNCTIONAL stall, 
 *      which is a condition set by the DCD as described in the USB 2.0 device framework (chapter 9). 
 *      A functional stall is only used on noncontrol endpoints and can be enabled in the device
 *      controller by setting the endpoint stall bit in the ENDPTCTRLx register 
 *      associated with the given endpoint and the given direction. 
 *      
 *      In a functional stall condition, the device controller will continue to return STALL responses to all
 *      transactions occurring on the respective endpoint and direction until the endpoint stall bit is cleared by the DCD.
 *      
 *      A PROTOCOL stall, unlike a function stall, is used on control endpoints is automatically cleared by the device
 *      controller at the start of a new control transaction (setup phase). 
 *      When enabling a protocol stall, the DCD should enable the stall bits (both directions) as a pair. 
 *      A single write to the ENDPTCTRLx register can ensure that both stall bits are set at the same instant.
 *  
 * @note : Any write to the ENDPTCTRLx register during operational mode must preserve the endpoint type field (i.e. perform a readmodifywrite).
 *  
 * @note : Reset Data-Toggle as mentioned in Chapter 8.3.3.1 Data Toggle Reset
 *  
 * @param core_id    
 *
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qhsusb_dci_stall_endpoint
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *   Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *   Chapter 8.3.2 Stalling
 *   Chapter 8.3.3.1 Data Toggle Reset
 */
// ===========================================================================
uint32 qhsusb_dci_unstall_endpoint(uint32 core_id, uint8 ep, uint8 dir)
{
    uint32 status=DCI_OK;
    uint32 mask=0;
    uint32 val=0;

    DCI_CHECK_CORE_ID(core_id);
    if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
    {
        /* No need to error out. Just don't perform the UNSTALL*/
        return(status); 
    }

    if (ep == 0) 
    {
        mask =  USB_OTG_HS_ENDPTCTRL0_TXS_BMSK     // TXS = TX Endpoint Stall 
              + USB_OTG_HS_ENDPTCTRL0_RXS_BMSK;    // RXS = RX Endpoint Stall 
              
        val  = 0; // unstall control endpoints
    }
    else
    {
        if (dir) 
        {
            mask = USB_OTG_HS_ENDPTCTRLn_TXS_BMSK  // TXS = TX Endpoint Stall 
                 + USB_OTG_HS_ENDPTCTRLn_TXR_BMSK; // TXR = TX Data Toggle Reset

            val  = USB_OTG_HS_ENDPTCTRLn_TXR_BMSK;
    
        }
        else
        {
            mask = USB_OTG_HS_ENDPTCTRLn_RXS_BMSK  // RXS = RX Endpoint Stall 
                 + USB_OTG_HS_ENDPTCTRLn_RXR_BMSK; // RXR = RX Data Toggle Reset

            val  = USB_OTG_HS_ENDPTCTRLn_RXR_BMSK;
        }
    }

    DCI_HWIO_OUTMI(core_id,USB_OTG_HS_ENDPTCTRLn,ep,mask,val); 

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_config_endpoint
 * 
 * @brief       This function configures the endpoint
 * 
 * @details     This function set the ENDPTCTRLX register and the relevant dQH.
 * 
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 * 
 * @param core_id    
 *
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * @param ep_type 
 *          endpoint type:    
 *          QHSUSB_EP_TYPE_CONTROL      0
 *          QHSUSB_EP_TYPE_ISO          1
 *          QHSUSB_EP_TYPE_BULK         2    
 *          QHSUSB_EP_TYPE_INTERRUPT    3
 * @param max_packet_length    
 *          maximum packet length - depends on the connection speed and the Hardware FIFOs.
 * @param zero_length_transfer    
 *          if set to TRUE, and the transfer length is a multiply of the max_packet_length
 *          then a zero lenfth packet is added.
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *        Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *        Chapter 8.3.1 Endpoint Initialization
 */
// ===========================================================================
uint32 qhsusb_dci_config_endpoint(uint32 core_id, uint8 ep, uint8 dir, 
                                  uint8 ep_type, uint16 max_packet_length, 
                                  boolean zero_length_transfer)
{
    uint32 status=DCI_OK;
    uint32 ep_control = 0;
    uint32 mask = 0;
#ifdef FEATURE_QHSUSB_MS
    /* no zero length transfer for MS */
    if (ep == MS_EP_IDX && zero_length_transfer == TRUE)
    {
       zero_length_transfer = FALSE;
    }
#endif /* FEATURE_QHSUSB_MS */
    if (ep == 0) 
    {
        return(DCI_OK);  // can't change ep#0 type or packet length
    }

    if ((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION) || (core_id >= DCI_MAX_CORES) )
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    if (dir == QHSUSB_EP_DIR_TX) 
    {
        ep_control =    USB_OTG_HS_ENDPTCTRLn_TXE_BMSK // TXE = TX Endpoint Enable
                      + ((dci_skip_enumeration(core_id)) ? 0 : USB_OTG_HS_ENDPTCTRLn_TXR_BMSK) // TXR = TX Data Toggle Reset
                      + (ep_type << USB_OTG_HS_ENDPTCTRLn_TXT_SHFT); // TXT = TX Endpoint Type
        mask = 0xffff0000;
    }
    else
    {
        ep_control =    USB_OTG_HS_ENDPTCTRLn_RXE_BMSK // RXE = RX Endpoint Enable
                      + ((dci_skip_enumeration(core_id)) ? 0 : USB_OTG_HS_ENDPTCTRLn_RXR_BMSK) // RXR = RX Data Toggle Reset
                      + (ep_type << USB_OTG_HS_ENDPTCTRLn_RXT_SHFT); // RXT = RX Endpoint Type
        mask = 0x0000ffff;
    }

    // Currently Hard-coded FS 64 bytes
    dci_dqh_list[ep][dir].mult_zlt_packetlen_ios = (max_packet_length << DQH_MAX_PACKET_LEN_SHFT);
    if (zero_length_transfer == TRUE) // allow 
    {
        dci_dqh_list[ep][dir].mult_zlt_packetlen_ios &= (~DQH_ZLT_BMSK); // ZLT=0=Enable-Zero-Length-Transfer
    }
    else
    {
        dci_dqh_list[ep][dir].mult_zlt_packetlen_ios |= DQH_ZLT_BMSK; // ZLT=1=Disable-Zero-Length-Transfer
    }

    DCI_HWIO_OUTMI(core_id,USB_OTG_HS_ENDPTCTRLn,ep,mask,ep_control);

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_unconfig_endpoint
 * 
 * @brief       This function de-configures the endpoint
 * 
 * @details     This function set the dQH with Max-Packet-Size=0,
 *              which results bus error when the host send IN/OUT tokens
 * 
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 * 
 * @note        caution: if one endpoint direction is enabled and the paired endpoint of opposite
 *              direction is disabled
 *              then the unused direction type must be changed from the default controltype
 *              to any other type (ie. bulktype).
 *              leaving an unconfigured endpoint control will cause undefined behavior 
 *              for the data pid tracking on the active endpoint/direction.
 * 
 * @param core_id    
 *
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qhsusb_dci_config_endpoint
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *        Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *        Chapter 8.3.1 Endpoint Initialization
 */
// ===========================================================================
uint32 qhsusb_dci_unconfig_endpoint(uint32 core_id, uint8 ep, uint8 dir)
{
    uint32 status=DCI_OK;

    if ((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION) || (core_id >= DCI_MAX_CORES) )
    {
        status = QHSUSB_ERR_BAD_PARAM;
    }
    else
    {
        // @note: set max-packet-length to zero disable any data packet from the host.
        dci_dqh_list[ep][dir].mult_zlt_packetlen_ios = 0;

        // @note: ep#0 RX & TX are always enabled.
        if (ep > DCI_EP0) 
        {
            uint32 mask = 0;
            uint32 value = 0;

            if (dir == QHSUSB_EP_DIR_TX) 
            {
                mask = USB_OTG_HS_ENDPTCTRLn_TXE_BMSK; // TXE = TX Endpoint Enable
            }
            else
            {
                mask = USB_OTG_HS_ENDPTCTRLn_RXE_BMSK; // RXE = RX Endpoint Enable
            }

            DCI_HWIO_OUTMI(core_id,USB_OTG_HS_ENDPTCTRLn,ep,mask,value);
        }
    }

    return(status);
}

// ===========================================================================
/**
 * @function    prepare_TDs_for_transfer
 * 
 * @brief   This function prepare the TDs for each transfer
 * 
 * @note    The number of TDs is limited to DCI_MAX_TDS
 * 
 * @param ep    
 *          endpoint number
 *
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 *
 * @param buffer_addr    
 *          pointer to buffer
 *
 * @param byte_count    
 *          number of bytes to send (dir) 
 *          or
 *          maximum number of bytes to receive (rx) 
 *
 * @param send_zlp    
 *          specify whether to transmit a ZLP at the end of the transfer.
 *
 * @param is_zero_address_chain_required
 *          specifies whether chaining is needed for the zero address.
 *
 * @return None.
 */
// ===========================================================================
static void prepare_TDs_for_transfer(uint8 ep, uint8 dir, uint32 buffer_addr, uint32 byte_count, uint8 send_zlp, boolean is_zero_address_chain_required)
{
  uint32 transfer_length = byte_count;
  uint32 total_bytes_for_TD;
  int i=-1;
  dTD_t *dtd_ptr;
  dTD_t *prev_td;

  if(is_first_TD_occupied)
  {
    /*increment the counter so that you don't overwrite*/
    i++;
    /*set the is_first_TD_occupied flag to zero*/
    is_first_TD_occupied = FALSE;
  }

  do
  {
    i++;
    total_bytes_for_TD = MIN (byte_count,DTD_MAX_TRANSFER_16KB);
    /*Initialize first 7 DWords to 0. */
    memset((uint8*)&dci_dtd_list[ep][dir][i],0,sizeof(dci_dtd_list[ep][dir][i]));
    dtd_ptr = &dci_dtd_list[ep][dir][i];

    /* Fill in total bytes and initialize the status field with the active bit set
       to “1” and all remaining status bits set to “0”. */
    dtd_ptr->size_ioc_mult_sts =  
      (total_bytes_for_TD << DTD_TOTAL_BYTES_SHFT) + DTD_STATUS_ACTIVE_BMSK;
    /* link the previous TD with this new one */
    if (i > 0)
    {
      prev_td = &(dci_dtd_list[ep][dir][i-1]);
      prev_td->next_dtd_ptr = (uint32)dtd_ptr;
    }
    /*Fill in buffer pointer page 0 and the current offset to point to the start of the data buffer. */
    dtd_ptr->buffer0_ptr = buffer_addr;
    /* Initialize buffer pointer page 1 through page 4 to be one greater than each of the previous buffer pointer. */
    dtd_ptr->buffer1_ptr = (dtd_ptr->buffer0_ptr) & 0xfffff000;
    dtd_ptr->buffer1_ptr += 4*1024;
    dtd_ptr->buffer2_ptr = dtd_ptr->buffer1_ptr+4*1024;
    dtd_ptr->buffer3_ptr = dtd_ptr->buffer2_ptr+4*1024;
    dtd_ptr->buffer4_ptr = dtd_ptr->buffer3_ptr+4*1024;
    byte_count -=total_bytes_for_TD;
    buffer_addr += total_bytes_for_TD;

//    /* for debug purposes only, maybe Virtio issue with chaining? */
//	dtd_ptr->size_ioc_mult_sts |= DTD_IOC_BMSK;
    } while (byte_count>0 && i<(DCI_MAX_TDS-1));

    if(is_zero_address_chain_required)
    {
        is_first_TD_occupied = TRUE; 
    }


    /* If function driver specifies that we send a ZLP, and for TX only, append a ZLP if byte_count is a multiple of max packet length */
  if ((!is_zero_address_chain_required) && (send_zlp) && (dir == DIR_TX) && ((transfer_length % qhsusb_dci_get_max_packet_size()) == 0) && (transfer_length != 0) && (i != -1))
  {
    ++i;
    memset((uint8*)&dci_dtd_list[ep][dir][i],0,sizeof(dci_dtd_list[ep][dir][i]));
    dtd_ptr = &dci_dtd_list[ep][dir][i];
    dtd_ptr->size_ioc_mult_sts =  
      (0 << DTD_TOTAL_BYTES_SHFT) + DTD_STATUS_ACTIVE_BMSK;
    /* link the previous TD with this new one */
    if (i > 0)
    {
      prev_td = &(dci_dtd_list[ep][dir][i-1]);
      prev_td->next_dtd_ptr = (uint32)dtd_ptr;
    }
    /*Fill in buffer pointer page 0 and the current offset to point to the start of the data buffer. */
    dtd_ptr->buffer0_ptr = (uint32)(&dci_dummy_buffer_for_zlp);
    dtd_ptr->buffer1_ptr = (uint32)(&dci_dummy_buffer_for_zlp);
    dtd_ptr->buffer2_ptr = (uint32)(&dci_dummy_buffer_for_zlp);
    dtd_ptr->buffer3_ptr = (uint32)(&dci_dummy_buffer_for_zlp);
    dtd_ptr->buffer4_ptr = (uint32)(&dci_dummy_buffer_for_zlp);
  }

  if(!is_zero_address_chain_required)
  {
  /* Set the terminate bit to “1”. */
  dtd_ptr->next_dtd_ptr = DTD_TERMINATE_BMSK;
  /* Set the interrupt on complete if desired. */
  dtd_ptr->size_ioc_mult_sts |= DTD_IOC_BMSK;
  }

}

// ===========================================================================
/**
 * @function    transfer_data_request
 * 
 * @brief       This function initate rx/dir data transfer
 * 
 * @details     
 * 
 *  The behaviors of the device controller for interrupt and bulk endpoints are identical. 
 *  All valid IN and OUT transactions to bulk pipes will handshake with a NAK unless the endpoint had been primed. 
 *  Once the endpoint has been primed, data delivery will commence.
 *  
 *  A dTD will be retired by the device controller when the packets described in the transfer descriptor have
 *  been completed. 
 *  
 *  Each dTD describes N packets to be transferred.
 *    With Zero Length Termination (ZLT) = 0
 *       N = INT(Number Of Bytes/Max. Packet Length) + 1
 *    With Zero Length Termination (ZLT) = 1
 *       N = MAXINT(Number Of Bytes/Max. Packet Length)
 *  
 *  Bit    | Status Field Description
 *  =================================
 *   7     |  Active.
 *   6     |  Halted.
 *   5     |  Data Buffer Error.
 *   3     |  Transaction Error.
 *   4,2,0 |  Reserved.
 * 
 * @param core_id    
 *
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * @param buffer_ptr    
 *          pointer to buffer
 * @param byte_count    
 *          number of bytes to send (dir) 
 *          or
 *          maximum number of bytes to receive (rx) 
 * @param send_zlp    
 *          specify whether to transmit a ZLP at the end of the transfer.
 *
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *   Chapter 3.6.19 ENDPTPRIME
 *   Chapter 8.4.1.1 Priming Transmit Endpoints
 *   Chapter 8.4.2 Interrupt/Bulk Endpoint Operational Model
 *   Chapter 8.6.2 Building a Transfer Descriptor
 *   Chapter 8.6.3 Executing A Transfer Descriptor
 */
// ===========================================================================
static uint32 transfer_data_request(uint32 core_id, uint8 ep, uint8 dir,void* buffer_ptr,uint32 byte_count, uint8 send_zlp, boolean is_zero_address_chain_required)
{
    uint32 buffer_addr = (uint32)buffer_ptr;
    dQH_t *dqh_ptr = &dci_dqh_list[ep][dir];
    dTD_t *dtd_ptr;
    uint32 mask = (1<<(ep+dir*USB_OTG_HS_ENDPTPRIME_PETB_SHFT));
    uint32 endptstat;
    uint32 i;

    // 8.4.3.1.2 Setup Packet Handling (2.3 hardware and later) :
    //   7. Before priming for status/handshake phases ensure that ENDPTSETUPSTAT is ‘0’.
    // NOTE: This is a rare case. 
    //      The USB 2.0 spec allow the device the following timeout to respond:
    //      - no data phase - up to 50 millisec
    //      - with data phase - up to 500 millisec
    if (ep==0) 
    {
        uint32 setup_status = DCI_HWIO_INM(core_id,USB_OTG_HS_ENPDTSETUPSTAT,0x01);
        if (setup_status) 
        {
            DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
            return(QHSUSB_ERR_SETUP_PENDING);
        }
    }
   /* max transfer to be 1MB */
   if (byte_count > DTD_MAX_TRANSFER_16KB*(DCI_MAX_TDS)) 
    {
        DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
        return(QHSUSB_ERR_BAD_PARAM);
    }

    // Check if the endpoint is already primed
    // Note: Currently in Boot code dTD chain is not supported.
    endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,mask);
    if(endptstat) // check if the endpoint is active, if there is a pending transfer.
    {
        DCI_ASSERT(QHSUSB_ERR_INVALID_OPERATION);
        return(QHSUSB_ERR_INVALID_OPERATION);
    }
    
     prepare_TDs_for_transfer(ep, dir, buffer_addr, byte_count, send_zlp,is_zero_address_chain_required);
    
    // 1. Write dQH next pointer AND dQH terminate bit to 0 as a single DWord operation.
    dqh_ptr->next_dtd_ptr = (uint32) &dci_dtd_list[ep][dir][0];

    // 2. Clear active & halt bit in dQH (in case set from a previous error).
    dqh_ptr->size_ioc_int_sts = 0;
   
    dci_dcache_flush(buffer_ptr, byte_count);
    for (i=0; i<DCI_MAX_TDS+DCI_ZLP_TD; i++)
    {
        dtd_ptr = &dci_dtd_list[ep][dir][i];
        dci_dcache_flush((void*)dtd_ptr,sizeof(*dtd_ptr)); 
    }
    dci_dcache_flush((void*)dqh_ptr,sizeof(*dqh_ptr));
   
   
    /*Set the Data Memory Barrier*/
	__dmb(0xF);
   
    if(!is_zero_address_chain_required)
    {
#ifdef FEATURE_RUMI_BOOT
        /** On RUMI we may need it for proper enumeration **/
        qhsusb_log(QHSUSB_DEFAULT_LOG,0,0);
#endif
        // 3. Prime endpoint by writing ‘1’ to correct bit position in ENDPTPRIME.
        DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTPRIME,mask);

        // 4. Verify Endpoint was primed
        {
            uint32 ep_status    = 0;
            uint32 ep_complete  = 0;
            int32  retries      = 0;

            do
            {
                ep_status = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTSTAT);
                ep_complete = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTCOMPLETE);

                ep_status &= mask;
                ep_complete &= mask;

                retries++;

                if (retries >= 5)
                {
                  DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTPRIME,mask);
                  retries = 0;
                }

            } while ( (ep_status == 0) && (ep_complete == 0)); 
        }
    }

    return(DCI_OK);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_transfer_request
 * 
 * @brief       This function initiates data transfer according to URB.
 * 
 * @details     
 *              This function calls transfer_data_request() and update the URB.
 * 
 * @note        The USB is a Master-Slave protocol where the Device is the Slave.
 *              The Device's data is actually sent/received on the USB bus 
 *              only when the Host send SETUP/IN/OUT token.
 *              Therefore, the caller only start a REQUEST.
 *              A callback is called upon complete/error detection.
 *              The caller May poll the status in the URB.
 * 
 * @param urb_ptr
 *              A pointer to URB structure.
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see transfer_data_request
 * 
 */
// ===========================================================================
uint32 qhsusb_dci_transfer_request(qhsusb_urb_t* urb_ptr)
{
    uint32  status=0;
    uint8   ep=0;
    boolean    dir=FALSE;
    void    *buffer_ptr;
    uint32  byte_count = 0;
    uint32  core_id=urb_ptr->core_id;
    uint32  endpoint_control=0;
    uint32  mask=0;
    uint8   send_zlp=0;
    boolean is_zero_address_chain_required = FALSE;

    DCI_INC_DEBUG_COUNTER(dci_transfer_request_counter);

    // 1. Check Signature
    #ifdef QHSUSB_DEBUG_DCI_ENABLE
    if (urb_ptr->signature != QHSUSB_URB_SIGNATURE) 
    {
        return(QHSUSB_ERR_URB_SIGNATURE);
    }
    #endif // QHSUSB_DEBUG_DCI_ENABLE

    DCI_CHECK_CORE_ID(core_id);

    // 2. extract parameters
    //usb 2.0 spec: bmRequestType 1 Bitmap Characteristics of request:
    //D7: Data transfer direction
    //0 = Host-to-device
    //1 = Device-to-host

    ep = urb_ptr->endpoint_address & 0x0F;
    dir = VAL_TO_BOOL(urb_ptr->endpoint_address & 0x80);
    buffer_ptr = urb_ptr->transfer_buffer_ptr;
    byte_count = urb_ptr->transfer_length;
	send_zlp = urb_ptr->send_zlp;
    is_zero_address_chain_required = urb_ptr->is_zero_address_chain_required;
    if(is_zero_address_chain_required)
    {
        zero_address_tx_length = urb_ptr->transfer_length;
    }

    if (ep >= DCI_MAX_ENDPOINTS)
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    // Verify the endpoint is configured
    endpoint_control = DCI_HWIO_INI(core_id,USB_OTG_HS_ENDPTCTRLn,ep);
    mask = (dir == DIR_TX) ? USB_OTG_HS_ENDPTCTRLn_TXE_BMSK : USB_OTG_HS_ENDPTCTRLn_RXE_BMSK;

    if ( (endpoint_control & mask) == 0)
    {
        return(QHSUSB_ERR_INVALID_OPERATION);
    }

    status = transfer_data_request(core_id,ep,dir,buffer_ptr,byte_count,send_zlp,is_zero_address_chain_required);


    if (status == 0) 
    {
        // Mark as active
        urb_ptr->transfer_status = QHSUSB_URB_STATUS_ACTIVE;
        urb_ptr->actual_length   = 0;

        if((!is_zero_address_chain_required) && (0 != zero_address_tx_length ))
        {
            urb_ptr->transfer_length += zero_address_tx_length;
            zero_address_tx_length = 0x0;
        }
        // Set dTD to point at the URB
        dci_dtd_list[ep][dir][0].urb_ptr = (uint32)urb_ptr;

    }
    else
    {
        urb_ptr->transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;
    }

    return(status);
}
// ===========================================================================

// ===========================================================================
/**
 * @function    qhsusb_dci_cancel_transfer
 * 
 * @brief       This function terminates an active transfers
 * 
 * @details     
 * 
 * @param core_id    
 *
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qhsusb_dci_shutdown
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 8.6.5 Flushing/Depriming an Endpoint
 */
// ===========================================================================
uint32 qhsusb_dci_cancel_transfer(uint32 core_id,uint8 ep,uint8 dir)
{
    uint32 mask,endptstat,endptflush,i;

    DCI_CHECK_CORE_ID(core_id);

    if((ep >= DCI_MAX_ENDPOINTS) || (dir >= DCI_MAX_DIRECTION))
    {
        /* No need to error out. Just don't cancel transfer for EPs which
        are not even primed or used */
        return(0); 
    }

    mask = (1<<(ep+dir*USB_OTG_HS_ENDPTSTAT_ETBR_SHFT));

    // Check if the endpoint is primed
    endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,mask);

    if(endptstat) // check if the endpoint is active, if there is a pending transfer.
    {
        dQH_t *dqh_ptr = NULL;
        dTD_t *dtd_ptr = NULL;
        qhsusb_urb_t    *urb_ptr = NULL;

        dqh_ptr = &dci_dqh_list[ep][dir];
        dci_dcache_inval((void*)dqh_ptr,sizeof(*dqh_ptr));
        for(i=0; i<DCI_MAX_TDS+DCI_ZLP_TD; i++)
        {
          dtd_ptr = &dci_dtd_list[ep][dir][i];
          dci_dcache_inval((void*)dtd_ptr,sizeof(*dtd_ptr));
        }
      
        urb_ptr = (void*)(dci_dtd_list[ep][dir][0].urb_ptr);

        while(endptstat != 0 )
        {
            // 1. Write a ‘1’ to the corresponding bit(s) in ENDPTFLUSH.
            DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTFLUSH,mask);
            // 2. Wait until all bits in ENDPTFLUSH are ‘0’.
            do
            {
                endptflush = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTFLUSH,mask);
            } while (endptflush != 0);
            // 3. Read ENDPTSTAT to ensure that for all endpoints commanded to be flushed, that the corresponding
            endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,mask);
        }
        if (urb_ptr != NULL)
        {
          urb_ptr->transfer_status = QHSUSB_URB_STATUS_COMPLETE_ERROR;

          // 3. call the URB callbacks
          if(urb_ptr->complete_callback != NULL) 
          {
            urb_ptr->complete_callback(urb_ptr);
          }
        }
    }

    return(0);
}

uint32 qhsusb_dci_cancel_transfer_wo_dqh_dtd(uint32 core_id,uint8 ep,uint8 dir)
{
    uint32 mask,endptstat,endptflush;
    uint32 endptcomplete;

    DCI_CHECK_CORE_ID(core_id);

    mask = (1<<(ep+dir*USB_OTG_HS_ENDPTSTAT_ETBR_SHFT));

    // Check if the endpoint is primed
    endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,mask);

    if(endptstat) // check if the endpoint is active, if there is a pending transfer.
    {
        while(endptstat != 0 )
        {
            // 1. Write a ‘1’ to the corresponding bit(s) in ENDPTFLUSH.
            DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTFLUSH,mask);
            // 2. Wait until all bits in ENDPTFLUSH are ‘0’.
            do
            {
                endptflush = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTFLUSH,mask);
            } while (endptflush != 0);
            // 3. Read ENDPTSTAT to ensure that for all endpoints commanded to be flushed, that the corresponding
            endptstat = DCI_HWIO_INM(core_id,USB_OTG_HS_ENDPTSTAT,mask);
        }
    }

    // in addition to the above, also forcefully clear ENDPTCOMPLETE that are leftover from the previous image
    endptcomplete = DCI_HWIO_IN(core_id, USB_OTG_HS_ENDPTCOMPLETE);
    DCI_HWIO_OUT(core_id, USB_OTG_HS_ENDPTCOMPLETE, endptcomplete);

    return(0);
}


// ===========================================================================

// ===========================================================================
/**
 * @function    qhsusb_dci_shutdown
 * 
 * @brief       This function cancel any active transfer.
 * 
 * @note        This function is called at the end of the DLOAD program before jumping to ARMPROG.
 *              Since ep#1 rx is still active , it need to be canceled.
 * 
 * @param core_id    
 *
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qhsusb_dci_cancel_transfer
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 8.6.5 Flushing/Depriming an Endpoint
 */
// ===========================================================================
uint32 qhsusb_dci_shutdown(uint32 core_id)
{
    uint8 ep;
    boolean  dir;

    DCI_CHECK_CORE_ID(core_id);

    for (ep=0;ep<DCI_MAX_ENDPOINTS;ep++) 
    {
        for (dir=0;dir<DCI_MAX_DIRECTION;dir++) 
        {
            qhsusb_dci_cancel_transfer(core_id,ep,dir);
        }
    }

    // de-configure all endpoints
    // When Max-Packet-Len = 0, the host will not get a NAK but a bus error.
    for (ep=0;ep<DCI_MAX_ENDPOINTS;ep++) 
    {
        for (dir=0;dir<DCI_MAX_DIRECTION;dir++) 
        {
            qhsusb_dci_unconfig_endpoint(core_id,ep,dir);
        }
    }

    return(DCI_OK);
}
// ===========================================================================

#ifdef QHSUSB_DEBUG_DCI_ENABLE
// ===========================================================================
/**
 * @function    dci_assert
 * 
 * @brief   This function log error detected.
 * 
 * @note    This function should be use for debug purpose only.
 * 
 * @param   val
 *              0 = ok , otherwise error.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void dci_assert(uint32 val,const char* filename_ptr,int line)
{
    if ((dci_assert_line==0) || (dci_assert_filename_ptr==0))
    {
      /* debug code */
      /* this was added to remove warning issued by compiler
         as these variables are set but not used anywhere in code */
    }
    if (val) 
    {
		while(1);
        if (dci_assert_val == 0) // hold the first error found
        {
            dci_assert_val = val;
            dci_assert_filename_ptr = filename_ptr;
            dci_assert_line = line;
        }
    }
}
#endif // QHSUSB_DEBUG_DCI_ENABLE

// ------------------------------------------------------------------------------------------------
// Non implemented APIs
// ------------------------------------------------------------------------------------------------

// ===========================================================================
/**
 * @function    qhsusb_dci_start_timer
 * 
 * @brief   Start one of the USB Core general-purpose timers
 * 
 * @note    Time#0 is for free run, Timer#1 is for short delays.
 * 
 * @param core_id    
 *
 * @param timer    
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @param repeat    
 *          1=Repeat , 0=One-Shot
 * 
 * @return status
 *              0 = ok , otherwise error.
 */
// ===========================================================================
uint32  qhsusb_dci_start_timer(uint32 core_id,uint8 timer, boolean repeat)
{
    uint32 status=DCI_OK;
    uint32 control=0;

    if ((timer >= DCI_MAX_TIMERS) || (core_id >= DCI_MAX_CORES))
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    if ( dci_timer_is_running[timer] )
    {
        return(QHSUSB_ERR_TIMER_IS_RUNNING);
    }

    control = USB_OTG_HS_GPTIMER0CTRL_GTPRUN_BMSK | USB_OTG_HS_GPTIMER0CTRL_GPTRST_BMSK;

    if (repeat) 
    {
        control |= USB_OTG_HS_GPTIMER0CTRL_GPTMODE_BMSK;
    }

    if (timer == 0)
    {
        // Load Maximum Value (0xffffff =~ 16 seconds)
        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER0LD,USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);

        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER0CTRL,control);
    }
    else
    {
        // Load Maximum Value (16 seconds)
        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER1LD,USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);

        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER1CTRL,control);
    }

    dci_timer_is_running[timer] = TRUE;

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_stop_timer
 * 
 * @brief   Stop one of the USB Core general-purpose timers
 * 
 * @note    Time#0 is for free run, Timer#1 is for short delays.
 * 
 * @param core_id    
 *
 * @param timer    
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @return status
 *              0 = ok , otherwise error.
 */
// ===========================================================================
uint32  qhsusb_dci_stop_timer(uint32 core_id,uint8 timer)
{
    uint32 status=DCI_OK;

    if ((timer >= DCI_MAX_TIMERS) || (core_id >= DCI_MAX_CORES))
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    if (timer == 0)
    {
        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER0CTRL,USB_OTG_HS_GPTIMER0CTRL_GTPRUN_TIMER_STOP_FVAL);
    }
    else
    {
        DCI_HWIO_OUT(core_id,USB_OTG_HS_GPTIMER1CTRL,USB_OTG_HS_GPTIMER0CTRL_GTPRUN_TIMER_STOP_FVAL);
    }

    dci_timer_is_running[timer] = FALSE;

    return(status);
}

// ===========================================================================
/**
 * @function    qhsusb_dci_get_time_usec
 * 
 * @brief   Get the elapsed time in micro seconds (usec)
 * 
 * @note    If the timer is in repeat mode it is the caller responsibility to handle rollover situation.
 * 
 * @param core_id    
 *
 * @param timer    
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @return  time
 *          elapsed time in micro seconds
 */
// ===========================================================================
uint32  qhsusb_dci_get_time_usec(uint32 core_id,uint8 timer)
{
    uint32 value=0;

    if ((timer >= DCI_MAX_TIMERS) || (core_id >= DCI_MAX_CORES))
    {
        return(QHSUSB_ERR_BAD_PARAM);
    }

    if (timer == 0)
    {
        value = DCI_HWIO_INM(core_id,USB_OTG_HS_GPTIMER0CTRL,USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);
    }
    else if (timer == 1)
    {
        value = DCI_HWIO_INM(core_id,USB_OTG_HS_GPTIMER1CTRL,USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK);
    }

    value = USB_OTG_HS_GPTIMER0LD_GPTLD_BMSK - value;

    return(value);
}

//============================================================================
/**
 * @function  qhsusb_dci_get_speed
 *
 * @brief Report the USB connection speed.
 *
 * @Note : Although the device is high-speed, the Host might be full-speed.
 *
 * @Note : The USB HW can be forced to Full-Speed.
 *
 * @param core_id    
 *
 * @return Speed
 *              QHSUSB_DEVICE_SPEED_FULL        
 *              QHSUSB_DEVICE_SPEED_LOW         
 *              QHSUSB_DEVICE_SPEED_HIGH        
 *              QHSUSB_DEVICE_SPEED_UNKNOWN     
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *       Chapter 3.6.16 PORTSCx
 *
 */
//============================================================================
uint32 qhsusb_dci_get_speed(uint32 core_id)
{
    uint32 speed = DCI_HWIO_INM(core_id,USB_OTG_HS_PORTSC,USB_OTG_HS_PORTSC_PSPD_BMSK) >> USB_OTG_HS_PORTSC_PSPD_SHFT;

    return(speed);
}

void qhsusb_dci_set_max_packet_size(uint32 max_packet_size)
{
  dci_max_packet_size = max_packet_size;
}

static uint32 qhsusb_dci_get_max_packet_size(void)
{
  return dci_max_packet_size;
}

//============================================================================
/**
 * @function  qhsusb_dci_is_suspended
 *
 * @brief report if the device is in suspend mode
 *
 * @param core_id    
 *
 * @return is_suspended
 *         1=Suspended,0=Not-Suspended
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *       Chapter 3.6.16 PORTSCx
 *
 */
//============================================================================
boolean qhsusb_dci_is_suspended(uint32 core_id)
{
    uint32 susp = DCI_HWIO_INM(core_id,USB_OTG_HS_PORTSC,USB_OTG_HS_PORTSC_SUSP_BMSK);
    susp = VAL_TO_BOOL(susp);

   return((boolean)susp);
}

//============================================================================
/**
 * @function  qhsusb_dci_get_line_state
 *
 * @brief Read line state from portsc register
 *
 *          
 * @param core_id 
 *           USB core id
 *
 * @return uint8 
 *           line state
 *
 * @ref 
 *      
 *
 */
//============================================================================
uint32 qhsusb_dci_get_line_state(uint32 core_id)
{
   return DCI_HWIO_INM(core_id, USB_OTG_HS_PORTSC, USB_OTG_HS_PORTSC_LS_BMSK) >> USB_OTG_HS_PORTSC_LS_SHFT;
}

//============================================================================
/**
 * @function  qhsusb_dci_set_runstop_bit
 *
 * @brief Set the Run Stop bit in the USB controller
 *
 *          
 * @param core_id 
 *           USB core id
 *
 * @return none
 *           
 *
 * @ref 
 *      
 */
 //============================================================================
void qhsusb_dci_set_runstop_bit(uint32 core_id)
{
    DCI_HWIO_OUTM(core_id, USB_OTG_HS_USBCMD, USB_OTG_HS_USBCMD_RS_BMSK, USB_OTG_HS_USBCMD_RS_RUN_FVAL); 
}

//============================================================================
/**
 * @function  qhsusb_dci_ulpi_read
 *
 * @brief write to the ulpi register
 *
 * @Note : For performing tasks that require ULPI reads eg. charger detection
 *          
 * @param ulpi_reg 
 *           ULPI register from which to read
 *
 * @return uint8 ULPI register value
 *
 * @ref 
 *      
 *
 */
//============================================================================
uint32 qhsusb_dci_ulpi_read(uint32 core_id, uint32 ulpi_reg)
{
    uint32 ulpi_cmd = 0;
    uint32 read_val = 0;
    uint32 loop_count = 0;
    uint32 temp = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;

    ulpi_cmd = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK +
               ( ulpi_reg << USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT );   

    DCI_HWIO_OUT(core_id, USB_OTG_HS_ULPI_VIEWPORT, ulpi_cmd);

    /* poll until the run bit transitions to 0. */
    while((temp == USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK) && (loop_count++ < QHSUSB_UPLI_LOOP_COUNT))
    {
        read_val = DCI_HWIO_IN(core_id, USB_OTG_HS_ULPI_VIEWPORT) ;
        temp = read_val & USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;
        qhsusb_dci_delay_us(10); /* poll for 10us */
    }
    
    return (read_val & USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_BMSK) >> USB_OTG_HS_ULPI_VIEWPORT_ULPIDATRD_SHFT;
   
}

// ===========================================================================
/**
 * @function    qhsusb_dci_ulpi_write
 * 
 * @brief   Writes to UPLI view port register
 * 
 * 
 * @param core_id    
 *
 * @param ulpi_reg    
 *          ULPI register name
 *
 * @param data    
 *          Data to write to the ulpi register
 *
 * @param mask    
 *          Mask to be used with specified data
 *
 * @return  none
 *          
 */
// ===========================================================================
void qhsusb_dci_ulpi_write(uint32 core_id, uint32 ulpi_reg, uint8 data, uint8 mask)
{
    uint32 ulpi_cmd = 0;
    uint8  write_data = 0;
    uint32 loop_count = 0;
    uint32 temp = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK;

    if(mask != 0xFF)
    {
        write_data = qhsusb_dci_ulpi_read(core_id, ulpi_reg);

        write_data &= (~mask);
        write_data |= (mask & data);
    }
    else
    {
        write_data = data;
    }

    ulpi_cmd = USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK +
               USB_OTG_HS_ULPI_VIEWPORT_ULPIRW_BMSK  +
               ( ulpi_reg << USB_OTG_HS_ULPI_VIEWPORT_ULPIADDR_SHFT ) +
               ( write_data );  

    DCI_HWIO_OUT(core_id, USB_OTG_HS_ULPI_VIEWPORT, ulpi_cmd);

    //poll until the run bit transitions to 0.
    while((temp == USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK) && (loop_count++ < QHSUSB_UPLI_LOOP_COUNT))
    {
        temp = DCI_HWIO_INM(core_id,USB_OTG_HS_ULPI_VIEWPORT,USB_OTG_HS_ULPI_VIEWPORT_ULPIRUN_BMSK);
        qhsusb_dci_delay_us(10); /* poll for 10us */
    }
}


//=============================================================================
/**
 * @function  qhsusb_dci_detect_charger_type
 *
 * Detects charger port type
 * 
 * Should be called after usb stack initialization
 * 
 * @param core_id
 *        
 *        
 *  
 * @return Charger port type.
 */
//=============================================================================
qhsusb_chg_port_type qhsusb_dci_detect_charger_type(uint32 core_id)
{
    return (qhsusb_chg_port_type) dci_detect_charger_type(core_id);
}

//=============================================================================
/**
 * @function  qhsusb_dci_set_enum_flag
 *
 * Stores whether R/S bit should be set during dci_init sequence
 * 
 * 
 * 
 * @param true or false
 *        
 *        
 *  
 * @return None
 */
//=============================================================================
void qhsusb_dci_set_enum_flag(boolean start_enum)
{
    dci_start_enum = start_enum;
}

//=============================================================================
/**
 *
 * @function  qhsusb_dci_get_enum_flag
 *
 * Returns whether R/S bit should be set during dci_init sequence
 * 
 * 
 * @param None
 *        
 *        
 *  
 * @return boolean
 */
//=============================================================================
boolean qhsusb_dci_get_enum_flag(void)
{
    return dci_start_enum;
}

// ===========================================================================
/**
 * @function    qhsusb_dci_write_scratch
 * 
 * @brief   Write scratch data on Scratch Register only
 * 
 * @note    
 * 
 * @param core_id    
 *
 * @param register offset 0-15    
 *
 * @param value to be written 
 */
// ===========================================================================
void qhsusb_dci_write_scratch(uint32 core_id,uint8 offset,uint32 data)
{

   DCI_HWIO_OUTMI(core_id,USB_OTG_HS_SCRATCH_RAMn,offset,USB_OTG_HS_SCRATCH_RAMn_BMSK,data);

}

// ===========================================================================
/**
 * @function    qhsusb_dci_read_scratch
 * 
 * @brief   Read scratch data on Scratch RAM
 * 
 * @note    
 * 
 * @param core_id    
 *
 * @param offset - offset of the RAM Scratch Pad register    
 *
 * @param data - scratch data to retrieve.    
 *
 */
// ===========================================================================
void qhsusb_dci_read_scratch(uint32 core_id,uint8 offset,uint32 *data )
{
   if(data)
   {     
      *data = DCI_HWIO_INI(core_id,USB_OTG_HS_SCRATCH_RAMn,offset);
   }
}

// ===========================================================================
/**
 * @function    dci_set_bulk_progress
 * 
 * @brief   Indicates that the Bulk Download is in Progress (Sahara/DLOAD)
 * 
 * @note    This API is known to support download mode in weak battery conditions          
 *  
 * @param   TRUE indicates it is in progress else false.
 *
 * @return none
 *
 * @ref 
 *
 */
// ===========================================================================
void dci_set_bulk_progress(boolean is_bulk_progress)
{
	is_bulk_mode_in_progress = is_bulk_progress;
	return;
}

// ===========================================================================
/**
 * @function    dci_skip_enumeration
 * 
 * @brief   Determine whether enumeration in subsequent stages can be skipped
 * 
 * @note    Wrapper function to lookup whether enumeration in subsequent stages can be skipped          
 *  
 * @param   TRUE signifies enumeration is skipped
 *
 */
// ===========================================================================
boolean dci_skip_enumeration(uint32 core_id)
{
#if !defined(BUILD_HOSTDL) && !defined(BUILD_EHOSTDL)
   
   static uint32 qhsusb_cookie_single_enum = QHSUSB_COOKIE_SINGLE_ENUM_UNINIT_VALUE;
   uint32 offset = 0;
   uint32 data = 0;
   if (qhsusb_cookie_single_enum == QHSUSB_COOKIE_SINGLE_ENUM_UNINIT_VALUE)
   {
		/*Read the cookie*/
		qhsusb_dci_read_scratch(core_id, offset, &qhsusb_cookie_single_enum);

		/*Clear the cookie*/
		qhsusb_dci_write_scratch (core_id, offset, 0x0);
   }
   /*assign the cookie to data*/
   data = qhsusb_cookie_single_enum; 
   if (data == QHSUSB_SINGLE_BOOT_ENUM_MAGIC_DL)
   {
		qhsusb_log(DCI_SKIP_ENUMERATION_LOG,0,TRUE);
#ifdef FEATURE_QHSUSB_MS
		if (TRUE == is_bulk_mode_in_progress)
		{
			qhsusb_log(QHSUSB_IS_BULK_MODE_IN_PROGRESS_LOG,0,0x1);
			return FALSE;
		}
#endif
		qhsusb_log(QHSUSB_SINGLE_BOOT_ENUM_MAGIC_DL_LOG,0,0);
		return TRUE;
   }
   else if (data == QHSUSB_SINGLE_BOOT_ENUM_MAGIC_FLCB)
   {
		qhsusb_log(DCI_SKIP_ENUMERATION_LOG,0,TRUE);
		/*Now we also have to support enumeration under weak battery*/
		/*conditions*/
		if (TRUE == is_bulk_mode_in_progress)
		{
			qhsusb_log(QHSUSB_IS_BULK_MODE_IN_PROGRESS_LOG,0,0x1);
			return FALSE;
		}
		qhsusb_log(QHSUSB_SINGLE_BOOT_ENUM_MAGIC_FLCB_LOG,0,0);
		return TRUE;
   }
   else
   {
	  qhsusb_log(DCI_SKIP_ENUMERATION_LOG,0,FALSE);
      return FALSE;
   }
#else
   /**
    * EHOSTDL and HOSTDL will always have to skip the enumeration as enumeration should have happend
    * in PBL and SBL respectively 
    **/
   /**This needs to be set as false as for eHOSTDL, this has no significance**/	
   if (TRUE == is_bulk_mode_in_progress)
   {   
		is_bulk_mode_in_progress = FALSE;
   }
   qhsusb_log(DCI_SKIP_ENUMERATION_LOG,0,TRUE);
   return  TRUE;  
#endif /* BUILD_HOSTDL ||BUILD_EHOSTDL  */   
}

#ifdef FEATURE_QHSUSB_SAHARA_DOWNLOAD_PIPO
// ===========================================================================
/**
 * @function    dci_handle_reset_during_transfer
 * 
 * @brief   This API handles USB Bus Reset during Bulk transfers
 * 
 * @note    This API cleans up the USB hardware registers only       
 *  
 * @param   Core_id
 *
 * @return  Void 
 */
// ===========================================================================
void dci_handle_reset_during_transfer(uint32 core_id)
{

    uint32 tmp=0;
    int ep,dir,td;

    DCI_CHECK_CORE_ID(core_id);

    //  1. Clear all setup token semaphores by reading the ENDPTSETUPSTAT register and writing the same value back to the ENDPTSETUPSTAT register.
    tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_ENPDTSETUPSTAT);
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENPDTSETUPSTAT,tmp);

    //  2. Clear all the endpoint complete status bits by reading the ENDPTCOMPLETE register and writing the same value back to the ENDPTCOMPLETE register.
    tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTCOMPLETE);
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTCOMPLETE,tmp);

    //  3. Cancel all primed status by waiting until all bits in the ENDPTPRIME are 0 and then writing 0xFFFFFFFF to ENDPTFLUSH.
    do
    {
        tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_ENDPTPRIME);
    } while (tmp != 0);
    DCI_HWIO_OUT(core_id,USB_OTG_HS_ENDPTFLUSH,0xFFFFFFFF);

    //  4. Read the reset bit in the PORTSCx register and make sure that it is still active. 
    tmp = DCI_HWIO_IN(core_id,USB_OTG_HS_PORTSC);
    if ((tmp & USB_OTG_HS_PORTSC_PR_BMSK) == 0) // PR=Port-Reset 
    {
        //DCI_ASSERT(QHSUSB_ERR_HW_FAILURE);
    }

    //  5. Free all allocated dTDs because they will no longer be executed by the device controller. 
    // NOTE: dQH and dTD are not relevant before endpoints are primed.
    //       setup packet has deticated 8 bytes buffer in the dQH
    memset(dci_dtd_list,0,sizeof(dci_dtd_list));

    for (ep=0;ep<DCI_MAX_ENDPOINTS;ep++) 
        for (dir=0;dir<DCI_MAX_DIRECTION;dir++) 
    {
          for (td = 0; td < DCI_MAX_TDS; td++)
          { 
            /* TDs are not linked at this stage */
            dci_dtd_list[ep][dir][td].next_dtd_ptr = DTD_TERMINATE_BMSK;
          } 
    }
}
#endif
// ===========================================================================
/**
 * @function    qhsusb_loop_here
 * 
 * @brief   This API loops here incase of any error in parameter checks
 * 
 * @note    This API will just loop in case of DCI_ASSERT being used here          
 *  
 * @param   none
 *
 */
// ===========================================================================
void qhsusb_loop_here(void)
{
    static volatile uint32 loop_here = 0x1; 
    while(loop_here);
}


