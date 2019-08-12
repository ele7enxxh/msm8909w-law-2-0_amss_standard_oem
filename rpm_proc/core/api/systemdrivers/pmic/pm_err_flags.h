#ifndef _PM_ERR_FLAGS_H
#define _PM_ERR_FLAGS_H

/** @file pm_err_flags.h
 *  
 *  This file defines error codes returned by PMIC library APIs 
 */
/*
 *  Copyright (c) 2009-2014 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
/* ======================================================================= */

/* =======================================================================
                             Edit History
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/rpm.bf/2.1.1/core/api/systemdrivers/pmic/pm_err_flags.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/30/14   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
03/27/12   hs      Added PM_ERR_FLAG__SPMI_TRANSCOMM_ERR.
02/10/11   hw      Added PM_ERR_FLAG__INVALID_DISPBACKLIGHT_APP_INDEXED
02/08/11   hw      Merging changes from the PMIC Distributed Driver Arch branch
01/07/10   wra     Adding SMBC and BMS error codes
08/10/10   wra     Adding PMIC Application Event error codes
06/28/10   wra     Adding 8660 error codes
12/02/09   jtn     Add error code for keypad
08/06/09   jtn     Add PM_ERR_MAX() macro
07/23/09   wra     added vbatt error definitions.
06/30/09   jtn     Updated file documentation
05/01/09   jtn     New file
=============================================================================*/
/** @addtogroup pm_err_flags
@{ */

/**
 * Error codes returned by PMIC functions.
 *
 * PMIC functions most commonly return errors if the caller has tried to use
 * parameters that are out of range or if the feature is not supported by the
 * PMIC model detected at run-time.
 */
typedef enum
{
    
	PM_ERR_FLAG__SUCCESS,/**< Function completed successfully. */
	
    PM_ERR_FLAG__SBI_OPT_ERR,  /**< SBI driver failed to communicate with the PMIC. */
    PM_ERR_FLAG__SBI_OPT2_ERR,   /**< Not used. */
    PM_ERR_FLAG__SBI_OPT3_ERR,   /**< Not used. */
    PM_ERR_FLAG__SBI_OPT4_ERR,   /**< Not used. */

    PM_ERR_FLAG__I2C_OPT_ERR,    /**< I2C communication error. */
    PM_ERR_FLAG__I2C_OPT2_ERR,   /**< Not used. */
    PM_ERR_FLAG__I2C_OPT3_ERR,   /**< Not used. */
    PM_ERR_FLAG__I2C_OPT4_ERR,   /**< Not used. */

    PM_ERR_FLAG__SPMI_OPT_ERR,   /**< SPMI communication error. */
    PM_ERR_FLAG__SPMI_OPT2_ERR,  /**< Not used. */
    PM_ERR_FLAG__SPMI_OPT3_ERR,  /**< Not used. */
    PM_ERR_FLAG__SPMI_OPT4_ERR,  /**< Not used. */

    PM_ERR_FLAG__SPMI_TRANSCOMM_ERR, /**< SPMI transcomm error. */


    PM_ERR_FLAG__PAR1_OUT_OF_RANGE, /**< Input parameter one is out of range. */
    PM_ERR_FLAG__PAR2_OUT_OF_RANGE, /**< Input parameter two is out of range. */
    PM_ERR_FLAG__PAR3_OUT_OF_RANGE, /**< Input parameter three is out of range. */
    PM_ERR_FLAG__PAR4_OUT_OF_RANGE, /**< Input parameter four is out of range. */
    PM_ERR_FLAG__PAR5_OUT_OF_RANGE, /**< Input parameter five is out of range. */
    PM_ERR_FLAG__PAR6_OUT_OF_RANGE, /**< Input parameter six is out of range. */
    PM_ERR_FLAG__VLEVEL_OUT_OF_RANGE, /**< Voltage level is out of range. */
    PM_ERR_FLAG__VREG_ID_OUT_OF_RANGE, /**< Voltage regulator ID is out of
                                            range. */
    PM_ERR_FLAG__FEATURE_NOT_SUPPORTED, /**< Feature is not supported by this
                                             PMIC. */
    PM_ERR_FLAG__INVALID_PMIC_MODEL,  /**< Invalid PMIC model. */
    PM_ERR_FLAG__SECURITY_ERR,        /**< Security error. */
    PM_ERR_FLAG__IRQ_LIST_ERR,        /**< IRQ list error. */
    PM_ERR_FLAG__DEV_MISMATCH,        /**< Device mismatch. */
    PM_ERR_FLAG__ADC_INVALID_RES,     /**< Invalid response from the ADC. */
    PM_ERR_FLAG__ADC_NOT_READY,       /**< ADC is not ready. */
    PM_ERR_FLAG__ADC_SIG_NOT_SUPPORTED, /**< ADC signal is not supported. */
    PM_ERR_FLAG__RTC_BAD_DIS_MODE,    /**< RTC displayed mode read from the
                                           PMIC is invalid. */

    PM_ERR_FLAG__RTC_READ_FAILED,  /**< Failed to read the time from the PMIC
                                        RTC. */
    PM_ERR_FLAG__RTC_WRITE_FAILED,  /**< Failed to write the time to the PMIC
                                         RTC. */
    PM_ERR_FLAG__RTC_HALTED,  /**< RTC is not running. */
   
        PM_ERR_FLAG__DBUS_IS_BUSY_MODE, /**< DBUS is already in use by another MPP. */
    PM_ERR_FLAG__ABUS_IS_BUSY_MODE, /**< ABUS is already in use by another MPP. */
    PM_ERR_FLAG__MACRO_NOT_RECOGNIZED, /**< Value that is not in the macro_type
                                            enum was used. */
    
    PM_ERR_FLAG__DATA_VERIFY_FAILURE, /**< Data read from a register does not
                                           not match the setting data. */
    PM_ERR_FLAG__SETTING_TYPE_NOT_RECOGNIZED, /**< Value that is not in the
                                                   pm_register_type enum was
                                                   used. */

    PM_ERR_FLAG__MODE_NOT_DEFINED_FOR_MODE_GROUP, /**< Value that is not in the
                                                       pm_mode_group enum was
                                                       used. */
    PM_ERR_FLAG__MODE_GROUP_NOT_DEFINED, /**< Value that is not in the pm_mode
                                              enum was used. */
    PM_ERR_FLAG__PRESTUB_FAILURE, /**< PRESTUB function returned FALSE. */
    PM_ERR_FLAG__POSTSTUB_FAILURE, /**< POSTSTUB function returned FALSE. */
    PM_ERR_FLAG__MODE_NOT_RECORDED_CORRECTLY, /**< Modes that were set for a
                                                   mode group were not recorded
                                                   correctly. */
    PM_ERR_FLAG__MODE_GROUP_STATE_NOT_FOUND, /**< Unable to find the mode group
                                                  in the mode group recording
                                                  structure. Fatal memory error. */
    PM_ERR_FLAG__SUPERSTUB_FAILURE, /**< SUPERSTUB function returned FALSE. */
    PM_ERR_FLAG__TARGET_PROCESSOR_CAN_NOT_ACCESS_RESOURCE, /**< Processor does
                                                                not have access
                                                                to this resource. */
    PM_ERR_FLAG__INVALID_RESOURCE_ACCESS_ATTEMPTED, /**< Resource is invalid.
                                                         The resource index was
                                                         passed into a router 
                                                         that is not defined
                                                         in the build. */
    
   PM_ERR_FLAG__VBATT_CLIENT_TABLE_FULL, /**< Nonzero value means the call was
                                               unsuccessful. Here it means that
                                               registration failed because the 
                                               driver ran out of memory.
                                               MAX_CLIENTS_ALLOWED must be
                                               increased, and the code must be
                                               recompiled. */
    PM_ERR_FLAG__VBATT_REG_PARAMS_WRONG, /**< One of the parameters to the
                                              function call was invalid. */
    PM_ERR_FLAG__VBATT_DEREGISTRATION_FAILED, /**< Client could not be
                                                   deregistered. May be because
                                                   the client does not exist. */
    PM_ERR_FLAG__VBATT_MODIFICATION_FAILED, /**< Client could not be modified.
                                                 May be because the client does
                                                 not exist. */
    PM_ERR_FLAG__VBATT_INTERROGATION_FAILED, /**< Client could not be queried.
                                                  May be because the client
                                                  does not exist. */
    PM_ERR_FLAG__VBATT_SET_FILTER_FAILED, /**< Client's filter could not be set.
                                               May be because the client does
                                               not exist. */
    PM_ERR_FLAG__VBATT_LAST_ERROR, /**< Keeps a count of all errors. Any error
                                        code equal to or greater than this one
                                        means an unknown error. VBATT_LAST_ERROR
                                        must always be the last error code, with
                                        the highest value. */  
    PM_ERR_FLAG__PMIC_NOT_SUPPORTED,  /**< PMIC is not supported. */
    
    PM_ERR_FLAG__INVALID_VIBRATOR_INDEXED,  /**< Invalid vibrator module is
                                                 being indexed. */
    PM_ERR_FLAG__INVALID_PWM_GENERATOR_INDEXED, /**< Invalid PWM generator is
                                                     being indexed. */
    PM_ERR_FLAG__INVALID_CHG_INDEXED,   /**< Invalid charger module is being
                                             indexed. */
    PM_ERR_FLAG__INVALID_CLK_INDEXED,   /**< Invalid clock is being indexed. */
    PM_ERR_FLAG__INVALID_XO_INDEXED,    /**< Invalid XO is being indexed. */
    PM_ERR_FLAG__INVALID_XOADC_INDEXED, /**< Invalid XO ADC is being indexed. */
    PM_ERR_FLAG__INVALID_TCXO_INDEXED,  /* Invalid TCXO is being indexed. */
    PM_ERR_FLAG__INVALID_RTC_INDEXED,   /**< Invalid RTC is being indexed. */
    PM_ERR_FLAG__INVALID_SMBC_INDEXED,  /**< Invalid SMBC is being indexed. */
    PM_ERR_FLAG__INVALID_BMS_INDEXED,   /**< Invalid BMS is being indexed. */
    PM_ERR_FLAG__API_NOT_IMPLEMENTED,   /**< API is not implemented. */
    PM_ERR_FLAG__INVALID_PAONCNTRL_INDEXED, /**< Invalid PAONCNTRL is being
                                                 indexed. */
    PM_ERR_FLAG__INVALID_COINCELL_INDEXED, /**< Invalid coincell module is being
                                                indexed. */
    PM_ERR_FLAG__INVALID_FLASH_INDEXED,    /**< Invalid Flash module is being
                                                indexed. */
    PM_ERR_FLAG__INVALID_OVP_INDEXED,  /**< Invalid OVP module is being indexed. */
    PM_ERR_FLAG__INVALID_KEYPAD_INDEXED, /**< Invalid keypad module is being
                                               indexed. */
    PM_ERR_FLAG__INVALID_LVS_INDEXED ,  /**< Invalid LVS module is being indexed. */
    PM_ERR_FLAG__INVALID_HSED_INDEXED, /**< Invalid HSED module is being indexed. */
    PM_ERR_FLAG__INVALID_TALM_INDEXED, /**< Invalid TALM module is being indexed. */
    PM_ERR_FLAG__INVALID_NCP_INDEXED, /**< Invalid NCP module is being indexed. */
    PM_ERR_FLAG__INVALID_NFC_INDEXED, /**< Invalid NFC module is being indexed. */
    PM_ERR_FLAG__INVALID_MVS_INDEXED, /**< Invalid MVS module is being indexed. */
    PM_ERR_FLAG__INVALID_HDMI_INDEXED, /**< Invalid HDMI module is being indexed. */
    PM_ERR_FLAG__INVALID_VS_INDEXED,  /**< Invalid VS module is being indexed. */
    PM_ERR_FLAG__INVALID_UVLO_INDEXED, /**< Invalid UVLO module is being indexed. */
    PM_ERR_FLAG__INVALID_AMUX_INDEXED, /**< Invalid AMUX module is being indexed. */
    PM_ERR_FLAG__INVALID_KEYPAD_EVENT_COUNTER, /**< Invalid keypad event counter. */
    PM_ERR_FLAG__INVALID_MPP_INDEXED,  /**< Invalid MPP module is being indexed. */
    PM_ERR_FLAG__INVALID_BATTERY_CELL_NUMBER, /**< Invalid battery cell number. */
    PM_ERR_FLAG__INVALID_PWRON_INDEXED, /**< Invalid power on module is being
                                             indexed. */
    PM_ERR_FLAG__INVALID_VBATT_INDEXED, /**< Invalid VBATT module is being indexed. */
    PM_ERR_FLAG__INVALID_HSDKYPD_APP_INDEXED, /**< Invalid HSD keypad application
                                                   is being indexed. */
    PM_ERR_FLAG__INVALID_PWRKEY_APP_INDEXED, /**< Invalid power key application
                                                  is being indexed. */
    PM_ERR_FLAG__INVALID_EVENT_CALLBACK, /**< Invalid event callback. */
    PM_ERR_FLAG__SHADOW_REGISTER_INIT_FAILED , /**< Shadow register intialization
                                                    failed. */

    /* PSDTE Error Functionality */
    PM_ERR_FLAG__PSDTE_ENV_FAILURE,  /**< PSDTE ENV failed. */
    PM_ERR_FLAG__PSDTE_PMIC_POWERUP_FAILED, /**< PSDTE PMIC power up failed. */
    PM_ERR_FLAG__PSDTE_PMIC_POWERDOWN_FAILED, /**< PSDTE PMIC power down failed. */
    PM_ERR_FLAG__FTS_CALCULATION_ERROR,  /**< FTS calculation error. */
    PM_ERR_FLAG__API_DEPRICATED_ERROR,   /**< API is deprecated. */
    PM_ERR_FLAG__RPC_PROCESSOR_NOT_RECOGNIZED_ERROR, /**< RPC processor not
                                                          recognized. */

    /* VREG Errors */
    PM_ERR_FLAG__VREG_VOTE_DEREGISTER_ERROR,  /**< Voltage regulator vote
                                                   deregistration error. */
    PM_ERR_FLAG__VREG_VOTE_REGISTER_ERROR,    /**< Voltage regulator vote
                                                   registration error. */
    /* VS Errors */
    PM_ERR_FLAG__INVALID_VS_STATUS,  /**< Invalid VS status. */

    PM_ERR_FLAG__DAL_SERVICE_REGISTRATION_FAILED, /**< DAL service registration
                                                       failed. */
    PM_ERR_FLAG__DAL_SERVICE_FAILED,  /**< DAL service failed. */
    PM_ERR__MUTEX_CREATION_FAILED,    /**< MUTEX creation failed. */
    PM_ERR__MUTEX_RELEASE_FAILED,     /**< MUTEX release failed. */
    PM_ERR__MUTEX_DELETION_FAILED,    /**< MUTEX deletion failed. */
    PM_ERR__MUTEX_UNAVAILBLE,         /**< MUTEX is unavailable. */
    PM_ERR_FLAG__COMM_TYPE_NOT_RECOGNIZED, /**< Create Comm Channel failed to
                                                find the correct commType. */
    PM_ERR_FLAG__IRQ_GPIO_SET_FAILED, /**< IRQ GPIO set failed. */
    PM_ERR_FLAG__PMAPP_EVENT_CLIENT_TABLE_FULL, /**< Event information table
                                                     has run out of client
                                                     entries. */
    PM_ERR_FLAG__PMAPP_EVENT_DEREGISTRATION_FAILED,  /**< Client could not be
                                                          deregistered. May be
                                                          because the client
                                                          does not exist. */
    PM_ERR_FLAG__PMAPP_EVENT_CLIENT_INDEX_FAILURE, /**< Client is attempting to
                                                        use a client index that
                                                        is incorrect or has been
                                                        previously deregisterd. */

    PM_ERR_FLAG__INVALID_MISC_INDEXED,  /**< Invalid MISC module is being indexed. */
    PM_ERR_FLAG__INVALID_MISC_TBD,      /**< Invalid MISC. */
    PM_ERR_FLAG__INVALID_VREG_INDEXED, /**< Invalid voltage regulator module is
                                            being indexed. */
    PM_ERR_FLAG__INVALID_POINTER,      /**< Invalid pointer. */
    PM_ERR_FLAG__FUNCTION_NOT_SUPPORTED_AT_THIS_LEVEL, /**< Function is not
                                                            supported at this
                                                            level. */
    PM_ERR_FLAG__INVALID_LPG_INDEXED,  /**< Invalid LPG module is being indexed. */
    PM_ERR_FLAG__INVALID_DISPBACKLIGHT_APP_INDEXED, /**< Invalid display backlight
                                                         application is being
                                                         indexed. */

    PM_ERR_FLAG__INVALID

} pm_err_flag_type;

/**
 * Macro used to return the greater of PM_ERR_FLAG__SUCCESS
 * or whichever error a function may have encountered.
 */
#define PM_ERR_MAX(a,b) ((a > b) ? (a) : (b))

/** @} */ /* end_addtogroup pm_err_flags */

#endif
