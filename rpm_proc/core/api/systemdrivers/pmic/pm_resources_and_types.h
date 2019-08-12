#ifndef PM_RESOURCES_AND_TYPES__H
#define PM_RESOURCES_AND_TYPES__H

/** @file pm_resources_and_types.h
 * 
 *  This file contains enumerations with resource names for different
 *  module types that should be used by external clients when accessing
 *  the resources that are required
*/
/*
 *  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_resources_and_types.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
09/03/12   rk      Added some comment.
09/25/12   hw      moving some npa type to this file so we don't need to include pmapp_npa.h
09/29/11   hs      Added CLK resource IDs.
09/14/11   dy      Move UICC resource IDs to this file
04/25/11   wra     Adding Battery Temperature Management BTM, LPG, more LVS, ADC_ARB,
                    OVP & INTerrupt channel enumeration needed for PM8921
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
07/05/10   wra     Modified Header includes to accomodate MSM8660
=============================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "DALStdDef.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/** @addtogroup pm_resources_and_types
@{ */


/** Definitions of all chargers in the target.
 */
enum
{
  PM_CHG_1, /**< Charger 1. */
  PM_CHG_INVALID
};

/**
 * PMIC power module voltage level.
 */
typedef uint32 pm_volt_level_type;

/**< On/off setting. */
typedef enum
{
   PM_OFF,  /**< Off. */
   PM_ON,   /**< On. */
   PM_INVALID
}pm_on_off_type;

/** Software mode setting. */
typedef enum 
{
    PM_SW_MODE_LPM     = 0,	/**< Low Power mode. */	
    PM_SW_MODE_BYPASS  = 1,	/**< Bypass mode, bit 5.  */
    PM_SW_MODE_AUTO    = 2, /**< Auto mode, bit 6.  */
    PM_SW_MODE_NPM     = 4, /**< Normal Power mode, bit 7. */
    PM_SW_MODE_INVALID
}pm_sw_mode_type;

/** Number of phases available to the autonomous phase controller (APC).
 */
typedef enum 
{
    PM_PHASE_CNT__1,  /**< One phase. */		
    PM_PHASE_CNT__2,  /**< Two phases. */	
    PM_PHASE_CNT__3,  /**< Three phases. */ 
    PM_PHASE_CNT__4,  /**< Four phases. */
    PM_PHASE_CNT__INVALID
}pm_phase_cnt_type;

/** Contains peripheral information. 
 */
typedef struct 
{
    uint16 base_address;		   /**< Base address. */
    uint8  peripheral_type;		   /**< Peripheral type. */
    uint8  peripheral_subtype;     /**< Peripheral subtype. */
    uint8  analog_major_version;   /**< Analog major version. */
    uint8  analog_minor_version;   /**< Analog minor version. */
    uint8  digital_major_version;  /**< Digital major version. */
    uint8  digital_minor_version;  /**< Digital minor version. */
}peripheral_info_type;

 /** 
 * Generic enable/disable. */
typedef enum
{
    PM_NPA_GENERIC_DISABLE,  /**< Generic disable (default). */
    PM_NPA_GENERIC_ENABLE,   /**< Generic enable. */
    PM_NPA_GENERIC_INVALID
}pm_npa_generic_enable_type;

/**
 * Bypass allowed/not allowed. */
typedef enum
{
    PM_NPA_BYPASS_ALLOWED,     /**< Bypass allowed (default). */
    PM_NPA_BYPASS_DISALLOWED,  /**< Bypass not allowed. */
    PM_NPA_BYPASS_INVALID
}pm_npa_bypass_allowed_type;

/**
 * LDO software mode. */
typedef enum
{
    PM_NPA_SW_MODE_LDO__IPEAK,  /**< LDO current peak (default). */
    PM_NPA_SW_MODE_LDO__NPM,    /**< LDO Normal Power mode. */
    PM_NPA_SW_MODE_LDO__INVALID
}pm_npa_sw_mode_ldo_type;

/**
 * SMPS software mode. */
typedef enum
{
    PM_NPA_SW_MODE_SMPS__AUTO,   /**< SMPS auto (default). */
    PM_NPA_SW_MODE_SMPS__IPEAK,  /**< SMPS current peak. */
    PM_NPA_SW_MODE_SMPS__NPM,    /**< SMPS Normal Power mode. */
    PM_NPA_SW_MODE_SMPS__INVALID
}pm_npa_sw_mode_smps_type;

/**
 * Pin control enable. */
typedef enum
{
    PM_NPA_PIN_CONTROL_ENABLE__NONE = 0, /**< None (default). */
    PM_NPA_PIN_CONTROL_ENABLE__EN1 = 1,  /**< Enable 1. */
    PM_NPA_PIN_CONTROL_ENABLE__EN2 = 2,  /**< Enable 2. */
    PM_NPA_PIN_CONTROL_ENABLE__EN3 = 4,  /**< Enable 4. */
    PM_NPA_PIN_CONTROL_ENABLE__EN4 = 8,  /**< Enable 8. */
    PM_NPA_PIN_CONTROL_ENABLE__INVALID
}pm_npa_pin_control_enable_type;

/** @cond */
/**
 Pin control Power mode.
 */
typedef enum
{
    PM_NPA_PIN_CONTROL_POWER_MODE__NONE = 0,     /**< No pin control Power mode
                                                      (default). */
    PM_NPA_PIN_CONTROL_POWER_MODE__EN1 = 1,      /**< Pin control Power mode
                                                      enable 1. */
    PM_NPA_PIN_CONTROL_POWER_MODE__EN2 = 2,      /**< Pin control Power mode
                                                      enable 2. */
    PM_NPA_PIN_CONTROL_POWER_MODE__EN3 = 4,      /**< Pin control Power mode
                                                      enable 3. */
    PM_NPA_PIN_CONTROL_POWER_MODE__EN4 = 8,      /**< Pin control Power mode
                                                      enable 4. */
    PM_NPA_PIN_CONTROL_POWER_MODE__SLEEPB = 16,  /**< Pin control Power mode
                                                      sleep B. */
    PM_NPA_PIN_CONTROL_POWER_MODE__INVALID
}pm_npa_pin_control_power_mode_type;

/* Corner mode. */
typedef enum
{
    PM_NPA_CORNER_MODE__NONE = 0,  /* No corner mode (default). */
    PM_NPA_CORNER_MODE__1 = 1,     /* Corner mode 1. */
    PM_NPA_CORNER_MODE__2 = 2,     /* Corner mode 2. */
    PM_NPA_CORNER_MODE__3 = 3,     /* Corner mode 3. */
    PM_NPA_CORNER_MODE__4 = 4,     /* Corner mode 4. */
    PM_NPA_CORNER_MODE__5 = 5,     /* Corner mode 5. */
    PM_NPA_CORNER_MODE__6 = 6,     /* Corner mode 6. */
    PM_NPA_CORNER_MODE__INVALID
}pm_npa_corner_mode_type;
/** @endcond */

/** Buffer pin control enable. */
typedef enum
{
    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__NONE = 0,  /**< No buffer pin control
                                                          (default). */
    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__EN1 = 1,   /**< Buffer pin control
                                                          enable 1. */
    PM_NPA_CLK_BUFFER_PIN_CONTROL_ENABLE__INVALID
}pm_npa_clk_buff_pin_control_enable_type;


/** Output drive strength. */
typedef enum
{
    PM_CLK_BUFF_OUT_DRV__1X,  /**< Drive strength 1X. */
    PM_CLK_BUFF_OUT_DRV__2X,  /**< Drive strength 2X. */
    PM_CLK_BUFF_OUT_DRV__3X,  /**< Drive strength 3X. */
    PM_CLK_BUFF_OUT_DRV__4X,  /**< Drive strength 4X. */
} pm_clk_buff_output_drive_strength_type;

/** 
  Types of IRQ bit fields of power rails. 
 */
typedef enum 
{
  PM_PWR_IRQ_VREG_OK,     /**< IRQ VREG OK. */
  PM_PWR_IRQ_VREG_ERROR,  /**< IRQ VREG error. */
  PM_PWR_IRQ_INVALID
}pm_pwr_irq_type;

/**
 *  Type definition for the IRQ triggers.
 */
typedef enum 
{
  PM_IRQ_TRIGGER_ACTIVE_LOW,    /**< IRQ trigger active low. */
  PM_IRQ_TRIGGER_ACTIVE_HIGH,   /**< IRQ trigger active high. */ 
  PM_IRQ_TRIGGER_RISING_EDGE,   /**< IRQ trigger rising edge. */
  PM_IRQ_TRIGGER_FALLING_EDGE,  /**< IRQ trigger falling edge. */
  PM_IRQ_TRIGGER_DUAL_EDGE,     /**< IRQ trigger dual edge. */
  PM_IRQ_TRIGGER_INVALID
} pm_irq_trigger_type;


/** Type definition for the IRQ status. */
typedef enum
{
  PM_IRQ_STATUS_RT,       /**< IRQ status = RT. */
  PM_IRQ_STATUS_LATCHED,  /**< IRQ status = latched. */
  PM_IRQ_STATUS_PENDING,  /**< IRQ status = pending. */
  PM_IRQ_STATUS_INVALID
} pm_irq_status_type;

/** @} */ /* end_addtogroup pm_resources_and_types */

#endif /* PM_RESOURCES_AND_TYPES__H */

