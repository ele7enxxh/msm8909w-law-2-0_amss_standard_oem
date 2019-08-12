/*==============================================================================

FILE:      BIMC.c

DESCRIPTION: This file implements NOC Error Handler.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

Edit History

//#CHANGE - Update when put in the depot
$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/icb/src/common/BIMC_error.c#1 $
$DateTime: 2016/06/17 14:31:11 $
$Author: pwbldsvc $
$Change: 10727476 $

When        Who    What, where, why
----------  ---    ----------------------------------------------------------- 
2014/06/06  tb     Created

        Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/
#include "BIMC_error.h"
//#include "ClockPIL.h"
#include "ClockTZ.h"
#include "tzbsp_isr.h"
#include "tzbsp_log.h"
#include "tzbsp.h"

/*============================================================================
                          MACROs
============================================================================*/
#define BIMC_OUT32(base, offset, data)  (*((volatile uint32 *) (base+offset)) = ((uint32) (data)))

#define BIMC_IN32(base, offset)  ((uint32)(*((volatile uint32 *) (base+offset))))

/*============================================================================
                          DEFINEs
============================================================================*/


/*============================================================================
                        DEVICE CONFIG PROPERTY DATA
============================================================================*/

extern const BIMC_propdata_type BIMC_propdata;
static const BIMC_BRIC_info_type* BRIC_info;
static const BIMC_DEFAULT_info_type* DEFAULT_info;
static const BIMC_DDR_SCMO_info_type* DDR_SCMO_info;

/*============================================================================
                          HW Register DEFINITION 
============================================================================*/

/*============================================================================
                                 FUNCTIONS
============================================================================*/
/*===========================================================================*/
/**
@brief 
      Handler function for BIMC error interrupt.
 
@param[in]  void* arg Context data for client.

@return    
      void* arg.

@dependencies
      None.
 
@sideeffects 
      None. 
*/ 
/*==============================================================================*/
void* BIMC_Handle_Interrupt(void* arg)
{

    int i, err_fatal = FALSE;

    // Dump Interrupt Status

    for(i=0; i < BIMC_propdata.BRIC_len; i++)
    {
        if(BRIC_info[i].turn_on_intr && (BIMC_IN32(BRIC_info[i].base_addr,BRIC_info[i].intr_status) != 0x0))
        {
            TZBSP_LOG_ERR("BIMC INTERRUPT: %s STATUS: 0x%08x", BRIC_info[i].name, BIMC_IN32(BRIC_info[i].base_addr,BRIC_info[i].intr_status));
            if( BRIC_info[i].err_fatal )
            {
                err_fatal = TRUE;
            }
        }
    }

    for(i=0; i < BIMC_propdata.DEFAULT_len; i++)
    {
        if(DEFAULT_info[i].turn_on_intr && (BIMC_IN32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_status) != 0x0))
        {
            TZBSP_LOG_ERR("BIMC INTERRUPT: %s STATUS: 0x%08x", DEFAULT_info[i].name, BIMC_IN32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_status));
            if( DEFAULT_info[i].intr_status0 != REG_NOT_AVAIL && (BIMC_IN32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_status0) != 0x0 ))
            {
                TZBSP_LOG_ERR("BIMC INTERRUPT: %s STATUS0 (addr lower): 0x%08x", DEFAULT_info[i].name, BIMC_IN32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_status0));
            }
            if( DEFAULT_info[i].intr_status1 != REG_NOT_AVAIL && (BIMC_IN32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_status1) != 0x0 ))
            {
                TZBSP_LOG_ERR("BIMC INTERRUPT: %s STATUS1 (addr upper): 0x%08x", DEFAULT_info[i].name, BIMC_IN32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_status1));
            }
            if( DEFAULT_info[i].intr_status2 != REG_NOT_AVAIL && (BIMC_IN32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_status2) != 0x0 ))
            {
                TZBSP_LOG_ERR("BIMC INTERRUPT: %s STATUS2: 0x%08x", DEFAULT_info[i].name, BIMC_IN32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_status2));
            }
            if( DEFAULT_info[i].err_fatal )
            {
                err_fatal = TRUE;
            }
        }
    }

    for(i=0; i < BIMC_propdata.DDR_SCMO_len; i++)
    {
        if(DDR_SCMO_info[i].turn_on_intr && (BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_status) != 0x0))
        {
            TZBSP_LOG_ERR("BIMC INTERRUPT: %s STATUS: 0x%08x", DDR_SCMO_info[i].name, BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_status));
            if( DDR_SCMO_info[i].intr_addr != REG_NOT_AVAIL && (BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_addr) != 0x0 ))
            {
                TZBSP_LOG_ERR("BIMC INTERRUPT: %s ADDR: 0x%08x", DDR_SCMO_info[i].name, BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_addr));
            }
            if( DDR_SCMO_info[i].intr_addr1 != REG_NOT_AVAIL && (BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_addr1) != 0x0 ))
            {
                TZBSP_LOG_ERR("BIMC INTERRUPT: %s ADDR1: 0x%08x", DDR_SCMO_info[i].name, BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_addr1));
            }
            if( DDR_SCMO_info[i].intr_packet0 != REG_NOT_AVAIL && (BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_packet0) != 0x0 ))
            {
                TZBSP_LOG_ERR("BIMC INTERRUPT: %s PACKET0: 0x%08x", DDR_SCMO_info[i].name, BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_packet0));
            }
            if( DDR_SCMO_info[i].intr_packet1 != REG_NOT_AVAIL && (BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_packet1) != 0x0 ))
            {
                TZBSP_LOG_ERR("BIMC INTERRUPT: %s PACKET1: 0x%08x", DDR_SCMO_info[i].name, BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_packet1));
            }
            if( DDR_SCMO_info[i].intr_packet2 != REG_NOT_AVAIL && (BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_packet2) != 0x0 ))
            {
                TZBSP_LOG_ERR("BIMC INTERRUPT: %s PACKET2: 0x%08x", DDR_SCMO_info[i].name, BIMC_IN32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_packet2));
            }
            if( DDR_SCMO_info[i].err_fatal )
            {
                err_fatal = TRUE;
            }
        }
    }

    // Clear Interrupts

    for(i=0; i < BIMC_propdata.DEFAULT_len; i++)
    {
        if(DEFAULT_info[i].turn_on_intr)
        {
            BIMC_OUT32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_clear, 0x1);
        }
    }

    for(i=0; i < BIMC_propdata.DDR_SCMO_len; i++)
    {
        if(DDR_SCMO_info[i].turn_on_intr)
        {
            BIMC_OUT32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_clear, 0x1);
        }
    }

    for(i=0; i < BIMC_propdata.BRIC_len; i++)
    {
        if(BRIC_info[i].turn_on_intr)
        {
            BIMC_OUT32(BRIC_info[i].base_addr,BRIC_info[i].intr_clear, 0xFF);
        }
    }

    if( err_fatal )
    {
        tzbsp_err_fatal(TZBSP_ERR_FATAL_BIMC_ERROR);
    }

    return arg;
}

//*============================================================================*/
/**
@brief
      Performs initialization for BIMC Error Handler.
      It enables interrupts required to handle BIMC errors.

@param[in]  None.

@return
      None.

@dependencies
      None.

@sideeffects
      None.
*/
/*============================================================================*/
void BIMC_Error_Init( void )
{
    
    uint8 i;

    //Check if this target's data is valid
    if(BIMC_propdata.vector == 0)
    {
        return; //This target does not support BIMC error interrupts
    }

    BRIC_info = BIMC_propdata.BRIC_info;
    DEFAULT_info = BIMC_propdata.DEFAULT_info;
    DDR_SCMO_info = BIMC_propdata.DDR_SCMO_info;

    if(BRIC_info == NULL || DEFAULT_info == NULL || DDR_SCMO_info == NULL)
    {
        return;  //BIMC data structure is not initialized in data file
    }

    //Enable NOC cfg Clocks
    //Clock_EnableBusAccess(TRUE);
    Clock_EnableClockGroup(CLOCK_GROUP_BUS);

    // TZ register FIQ handler
    if(0 != tzbsp_register_isr(BIMC_propdata.vector,
                               "BIMC",
                               BIMC_Handle_Interrupt,
                               (void*)BIMC_propdata.vector,
                               TZBSP_INTF_TRIGGER_LEVEL,
                               TRUE))
    {
        TZBSP_LOG_ERR("BIMC init error !");
        tzbsp_err_fatal(TZBSP_ERR_FATAL_BIMC_ERROR);
        return;
    }

    // Enable Interrupts

    for(i=0; i < BIMC_propdata.BRIC_len; i++)
    {
        if(BRIC_info[i].turn_on_intr)
        {
            BIMC_OUT32(BRIC_info[i].base_addr,BRIC_info[i].intr_enable, 0xFF);
        }
    }

    for(i=0; i < BIMC_propdata.DEFAULT_len; i++)
    {
        if(DEFAULT_info[i].turn_on_intr)
        {
            BIMC_OUT32(DEFAULT_info[i].base_addr,DEFAULT_info[i].intr_enable, 0x1);
        }
    }

    for(i=0; i < BIMC_propdata.DDR_SCMO_len; i++)
    {
        if(DDR_SCMO_info[i].turn_on_intr)
        {
            BIMC_OUT32(DDR_SCMO_info[i].base_addr,DDR_SCMO_info[i].intr_enable, 0x1);
        }
    }

    //Disable NOC cfg Clocks, TZ will restore the state of clocks.
    //Clock_EnableBusAccess(FALSE);
    Clock_DisableClockGroup(CLOCK_GROUP_BUS);
}


