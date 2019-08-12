#ifndef PM_PON__H
#define PM_PON__H
/*! \file 
 *  \n
 *  \brief  
 *  \details  This file contains functions and variable declarations to support 
 *   the pon driver module.
 *
 *
 *  \n &copy; Copyright (c) 2012    by Qualcomm Technologies, Inc.  All Rights Reserved.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/api/systemdrivers/pmic/pm_pon.h#1 $
  $DateTime: 2015/09/01 00:30:35 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/03/15   rk      Changed PMIC PS_HOLD config API call for multi-PMIC support.
12/04/13   aab     Added PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET to pm_pon_reset_cfg_type
11/01/13   sv      Applying CR538994 Changes with power up failure fix. (CR-570053)
10/24/13   sv      Reverting CR538994 Changes (CR-566323)
06/24/13   ps      Added PON Stage3 reset config API CR#495834 
04/10/13   kt      Added PMIC Watchdog config support  
02/27/13   kt      Added PON IRQ related APIs  
12/06/12   hw      Remove comdef.h and use com_dtypes.h
22/11/12   umr     Created PON API.
===========================================================================*/
/*===========================================================================

                        HEADER FILES

===========================================================================*/
#include "pm_err_flags.h"
#include "com_dtypes.h"
#include "pm_resources_and_types.h"

/*===========================================================================

                        TYPE DEFINITIONS 

===========================================================================*/
/*! \enum pm_pon_reset_cfg_type
   \brief Reset configure type.
 */
typedef enum
{
  PM_PON_RESET_CFG_WARM_RESET,
  PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
  /* Shutdown to a state of main battery removal */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN,
   /* Shutdown to a state of main battery and coin cell removal*/
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
   /* Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_HARD_RESET,
  /* Main Batt and coin cell remove Shutdown + Auto pwr up 8: DVDD_HARD_RESET */
  PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_D_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_IMMEDIATE_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  /* Warm Reset and Main Batt/coin cell remove Shutdown */
  PM_PON_RESET_CFG_WARM_RESET_AND_D_VDD_BATT_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_AND_X_VDD_COIN_CELL_REMOVE_SHUTDOWN,
  PM_PON_RESET_CFG_WARM_RESET_AND_SHUTDOWN,
  /* Warm Reset then Main Batt/coin cell remove Shutdown + Auto pwr up */
  PM_PON_RESET_CFG_WARM_RESET_THEN_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_D_VDD_BATT_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_WARM_RESET_THEN_X_VDD_COIN_CELL_REMOVE_HARD_RESET,
  PM_PON_RESET_CFG_INVALID  
}pm_pon_reset_cfg_type;

/*! \enum pm_pon_reset_source_type
   \brief Reset source type.
   DO NOT CHANGE THE SEQUENCE OF TOP 4 enums
 */
typedef enum
{
  PM_PON_RESET_SOURCE_KPDPWR,           /* Power key */
  PM_PON_RESET_SOURCE_RESIN,            /* Resin in form MSM */
  PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR, /* Pwr Key + Resin */
  PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR,  /* Pwr Key or Resin */
  PM_PON_RESET_SOURCE_GP1,              /* General purpose-1 */
  PM_PON_RESET_SOURCE_GP2,              /* General purpose-1 */
  PM_PON_RESET_SOURCE_INVALID  
}pm_pon_reset_source_type;

/*! \struct pm_pon_pon_reason_type
   \brief Power On reason type.
 */
typedef struct
{
  unsigned hard_reset : 1;     /* Hard reset event trigger */
  unsigned smpl : 1;           /* SMPL trigger */             
  unsigned rtc : 1;            /* RTC trigger */             
  unsigned dc_chg : 1;         /* DC Charger trigger */             
  unsigned usb_chg : 1;        /* USB Charger trigger */             
  unsigned pon1 : 1;           /* PON1 trigger */
  unsigned cblpwr : 1;         /* CBL_PWR1_N trigger */
  unsigned kpdpwr : 1;         /* KPDPWR_N trigger */
}pm_pon_pon_reason_type;

/*! \struct pm_pon_poff_reason_type
   \brief Power Off reason type.
 */
typedef struct
{
  unsigned soft : 1;               /* Software trigger */
  unsigned ps_hold : 1;            /* PS_HOLD trigger */             
  unsigned pmic_wd : 1;            /* PMIC Watchdog trigger */             
  unsigned gp1 : 1;                /* Keypad_Reset1 trigger */             
  unsigned gp2 : 1;                /* Keypad_Reset2 trigger */             
  unsigned kpdpwr_and_resin : 1;   /* KPDPWR_N and RESIN_N trigger */
  unsigned resin : 1;              /* RESIN_N trigger */
  unsigned kpdpwr : 1;             /* KPDPWR_N trigger */
  unsigned reserved : 3;           /* Reserved bits */
  unsigned charger : 1;            /* Charger (BOOT_DONE) trigger */
  unsigned tft : 1;                /* TFT trigger */
  unsigned uvlo : 1;               /* UVLO trigger */
  unsigned otst3 : 1;              /* Overtemp trigger */
  unsigned stage3 : 1;             /* Stage3 reset trigger */
}pm_pon_poff_reason_type;

/*! \struct pm_pon_warm_reset_reason_type
   \brief Warm Reset reason type.
 */
typedef struct
{
  unsigned soft : 1;               /* Software trigger */
  unsigned ps_hold : 1;            /* PS_HOLD trigger */             
  unsigned pmic_wd : 1;            /* PMIC Watchdog trigger */             
  unsigned gp1 : 1;                /* Keypad_Reset1 trigger */             
  unsigned gp2 : 1;                /* Keypad_Reset2 trigger */             
  unsigned kpdpwr_and_resin : 1;   /* KPDPWR_N and RESIN_N trigger */
  unsigned resin : 1;              /* RESIN_N trigger */
  unsigned kpdpwr : 1;             /* KPDPWR_N trigger */
  unsigned reserved : 4;           /* Reserved bits */
  unsigned tft : 1;                /* TFT trigger */
  unsigned reserved1 : 3;          /* Reserved bits */
}pm_pon_warm_reset_reason_type;

/*! \struct pm_pon_soft_reset_reason_type
   \brief Soft Reset reason type.
 */
typedef struct
{
  unsigned soft : 1;               /* Software trigger */
  unsigned ps_hold : 1;            /* PS_HOLD trigger */             
  unsigned pmic_wd : 1;            /* PMIC Watchdog trigger */             
  unsigned gp1 : 1;                /* Keypad_Reset1 trigger */             
  unsigned gp2 : 1;                /* Keypad_Reset2 trigger */             
  unsigned kpdpwr_and_resin : 1;   /* KPDPWR_N and RESIN_N trigger */
  unsigned resin : 1;              /* RESIN_N trigger */
  unsigned kpdpwr : 1;             /* KPDPWR_N trigger */
  unsigned reserved : 4;           /* Reserved bits */
  unsigned tft : 1;                /* TFT trigger */
  unsigned reserved1 : 3;          /* Reserved bits */
}pm_pon_soft_reset_reason_type;


/*! \enum pm_pon_irq_type
   \brief different types of irq bit fields of pon irq module
 */
typedef enum {
  PM_PON_IRQ_KPDPWR_ON          = 0,  
  PM_PON_IRQ_RESIN_ON           = 1,  
  PM_PON_IRQ_CBLPWR_ON          = 2,  
  PM_PON_IRQ_KPDPWR_BARK        = 3,  
  PM_PON_IRQ_RESIN_BARK         = 4,  
  PM_PON_IRQ_KPDPWR_RESIN_BARK  = 5,  
  PM_PON_IRQ_PMIC_WD_BARK       = 6,  
  PM_PON_IRQ_SOFT_RESET 	      = 7,
  /*INVALID*/
  PM_PON_IRQ_INVALID
}pm_pon_irq_type;

/*! \enum pm_pon_pbl_status
   \brief types of PON_PBL RB occurance 
 */
typedef enum {
  PM_PON_PBL_DVDD_RB_OCCURRED,
  PM_PON_PBL_XVDD_RB_OCCURRED,
  PM_PON_PBL_STATUS_INVALID
}pm_pon_pbl_status_type;

typedef enum {
  PM_PON_PERPH_RB_SPARE			 = 1,  
  PM_PON_DVDD_RB_SPARE           = 2,  
  PM_PON_XVDD_RB_SPARE           = 3,  
  PM_PON_SOFT_RB_SPARE           = 4,  
  /*INVALID*/
  PM_PON_SPARE_INVALID
}pm_pon_spare_reg_type;
 
 /*! \enum pmapp_ps_hold_cfg_type
   \brief PS_HOLD reset configure type.
  */
typedef enum
{
  PMAPP_PS_HOLD_CFG_WARM_RESET,
  PMAPP_PS_HOLD_CFG_HARD_RESET,
  PMAPP_PS_HOLD_CFG_NORMAL_SHUTDOWN,
  PMAPP_PS_HOLD_CFG_INVALID  
}pmapp_ps_hold_cfg_type;
 
 
  
/*===========================================================================

                        HEADER FILES

===========================================================================*/
/**
 * @brief Configure MSM PS_HOLD behavior.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param ps_hold_cfg 
 *          PM_PON_RESET_CFG_WARM_RESET,
 *          PM_PON_RESET_CFG_NORMAL_SHUTDOWN,
 *          PM_PON_RESET_CFG_D_VDD_BATT_REMOVE_SHUTDOWN, Shutdown + dVdd_rb remove main battery
 *          PM_PON_RESET_CFG_X_VDD_COIN_CELL_REMOVE_SHUTDOWN, Shutdown + xVdd_rb remove main and coin cell battery
 *          PM_PON_RESET_CFG_HARD_RESET,     Shutdown + Auto pwr up
 *   
 *                   
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_ps_hold_cfg(uint8 pmic_device_index, pm_pon_reset_cfg_type ps_hold_cfg);

/**
 * @brief Configure PON reset sources
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *  
 * @param reset_source: Select source for reset.
 *            PM_PON_RESET_SOURCE_KPDPWR,
 *            PM_PON_RESET_SOURCE_RESIN,
 *            PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR,
 *            PM_PON_RESET_SOURCE_GP1,
 *            PM_PON_RESET_SOURCE_GP2,
 *  
 * @param s1_timer: Select S1 timer in ms for bark.
 *            Supported time in ms for all the reset source
 *            types, Rounds UP to next largest setting if
 *            value passed is in between the permitted values:
 *            0, 32, 56, 80, 128, 184, 272, 408, 608, 904, 1352,
 *            2048, 3072, 4480, 6720, 10256
 *  
 * @param s2_timer: Select S2 timer in ms for bite. This is the 
 *                time after bark S1.
 *            Supported time in ms for all the reset source
 *            types, rounds up to next largest number in ms if
 *            value passed is in between the permitted values:0,
 *            10, 50, 100, 250, 500, 1000, 2000
 *  
 * @param reset_cfg_type: Configure the type of reset to be 
 *                      performed on the event.
 *  
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_reset_source_cfg(uint8 pmic_device_index, 
                        pm_pon_reset_source_type reset_source,
                        uint32 s1_timer, uint32 s2_timer, pm_pon_reset_cfg_type reset_cfg_type);



/**
 * @brief Configure PON Overtemp reset sources
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *  
 *  
 * @param reset_cfg_type: Configure the type of reset to be 
 *                      performed on the event.
 *  
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_overtemp_reset_cfg(uint8 pmic_device_index, pm_pon_reset_cfg_type overtemp_cfg);


/**
 * @brief Configure PON reset Control
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param reset_source source of the reset to be 
 *          enabled / disabled.
 *                   
 * @param enable Select enable / disable 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_reset_source_ctl(uint8 pmic_device_index, 
                        pm_pon_reset_source_type reset_source, pm_on_off_type on_off);

/**
 * @brief Configure PON stage3 reset source and timer
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *  
 * @param reset_source: Select source for reset.
 *            PM_PON_RESET_SOURCE_KPDPWR,
 *            PM_PON_RESET_SOURCE_RESIN,
 *            PM_PON_RESET_SOURCE_RESIN_AND_KPDPWR,
 *            PM_PON_RESET_SOURCE_RESIN_OR_KPDPWR
 *  
 * @param s3_timer: Select S3 timer in sec
 *            Supported time in sec for all the reset source
 *            types, rounds up to next largest number in sec if
 *            value passed is in between the permitted values:
 *            0, 2, 4, 8, 16, 32, 64, 128
 *  
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_stage3_reset_source_cfg(uint8 pmic_device_index, 
                               pm_pon_reset_source_type reset_source,
                               uint32 s3_timer);
                        
/**
 * @brief Configure PON PMIC Watchdog reset source
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *  
 * @param s1_timer: Select S1 timer in seconds for bark.
 *            Supported time in seconds: 0 to 127
 *  
 * @param s2_timer: Select S2 timer in seconds for bite. This is
 *                the time after bark S1.
 *            Supported time in seconds: 0 to 127
 *  
 * @param reset_cfg_type: Configure the type of reset to be 
 *                      performed on the event.
 *  
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_wdog_cfg(uint8 pmic_device_index, uint32 s1_timer, uint32 s2_timer,
                pm_pon_reset_cfg_type reset_cfg_type);

/**
 * @brief PON PMIC Watchdog reset Control (enable/disable)
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 *                   
 * @param enable Select enable / disable 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_wdog_enable(uint8 pmic_device_index, pm_on_off_type enable);

/**
 * @brief Clears the PMIC Watchdog timer (Pets the PMIC 
 *        Watchdog).
 * 
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero. 
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type
pm_pon_wdog_pet(uint8 pmic_device_index);

/**
 * @brief Returns reason for Power On 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                PON reason type. Refer enum
 *                pm_pon_pon_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_get_pon_reason(uint8 pmic_device_index, pm_pon_pon_reason_type* reason);

/**
 * @brief Returns reason for Power Off 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                POFF reason type. Refer enum
 *                pm_pon_poff_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_get_poff_reason(uint8 pmic_device_index, pm_pon_poff_reason_type* reason);

/**
 * @brief Returns reason for Warm Reset
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                Warm reset reason type. Refer enum
 *                pm_pon_warm_reset_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_get_warm_reset_reason(uint8 pmic_device_index, pm_pon_warm_reset_reason_type* reason);

/**
 * @brief Returns reason for Soft Reset
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] reason:  
 *                Soft reset reason type. Refer enum
 *                pm_pon_soft_reset_reason_type.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_get_soft_reset_reason(uint8 pmic_device_index, pm_pon_soft_reset_reason_type* reason);

/**
 * @brief Returns reasons for Power On, Power Off, Warm Reset
 *        and Soft Reset.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] pon_reasons:  
 *                Returns a uint64 value with all the PON
 *                reasons (includes PON, POFF, WARM RESET and
 *                SOFT RESET reasons).
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_get_all_pon_reasons(uint8 pmic_device_index, uint64* pon_reasons);

/**
 * @brief Configure PON Debounce time delay for KPD, CBL, 
 *        General Purpose PON, RESIN, RESIN AND KPD, GP1 and GP2
 *        state change and interrupt triggering.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] time_delay:  
 *                Select time delay in milli seconds. Supported
 *                time in ms, rounds up to the next largest
 *                number in ms if value passed is in between the
 *                permitted values: 15, 31, 62, 125, 250, 500,
 *                1000, 2000
 *
 *
 * @return  pm_err_flag_type
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__PAR2_OUT_OF_RANGE = Time delay value
 *          out of range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 */
pm_err_flag_type
pm_pon_debounce_ctl(uint8 pmic_device_index, uint32 time_delay);

/**
 * @brief This function enables or disables pon irq
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.
 *  
 * @param[in] enable:  
 *                TRUE: Enable corresponding PON interrupt 
 *                FALSE:Disable corresponding PON interrupt
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_irq_enable(uint8 pmic_device_index, pm_pon_irq_type irq, boolean enable);


/**
 * @brief This function clears the PON irq 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.  
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_irq_clear(uint8  pmic_device_index, pm_pon_irq_type irq);


/**
 * @brief This function configures the PON irq trigger 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type.  
 * @param[in] trigger:  
 *                One of different irq triggers. Refer enum pm_irq_trigger_type 
 *                from pm_resources_and_types.h for more details
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_irq_set_trigger(uint8 pmic_device_index, pm_pon_irq_type irq, pm_irq_trigger_type trigger);


/**
 * @brief This function configures the PON for irq 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[in] irq:  
 *                PON irq type. Refer enum pm_pon_irq_type. 
 * @param[in] type:  
 *                Type of IRQ status to read. Refer enum pm_irq_status_type 
 *                from pm_resources_and_types.h for more details
 * @param[out] status:  
 *                IRQ status.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this
 *          version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_irq_status(uint8 pmic_device_index, pm_pon_irq_type irq, pm_irq_status_type type, boolean *status);


/**
 * @brief This function configures UVLO Rising Voltage threshold and HYST Voltage threshold
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0
 * @param[in] :  
 *          uvlo_rising_thresh_voltage_in_mv (Range between 2875 and 3225 )
 *          hyst_voltage_in_mv (Range between 175 and 425 )
 *            
 * @return  pm_err_flag_type 
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_config_uvlo(uint8 pmic_device_index, uint32 uvlo_rising_thresh_voltage_in_mv, uint32 hyst_voltage_in_mv);


/**
 * @brief This function returns WARM RESET status whether the 
 *        reset occured or not and this status needs to be
 *        explicitly cleared using the
 *        pm_pon_warm_reset_status_clear API.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 * @param[out] status:  
 *                WARM RESET status (returns TRUE if warm reset
 *                occured, FALSE otherwise).
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in as an argument.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not
 *          available on this version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_warm_reset_status(uint8 pmic_device_index, boolean *status);


/**
 * @brief This function clears the WARM RESET status.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1 
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not
 *          available on this version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_warm_reset_status_clear(uint8 pmic_device_index);


/**
 * @brief This function returns XVDD/DVDD status 
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
 *                   pbl_status ,  DVDD_RB_OCCURRED: to get the status of DVDD RB occurance  
                                          XVDD_RB_OCCURRED: to get the status of XVDD RB occurance  
 * @param[out] status:  
 *                XVDD/DVDD status (returns TRUE if warm reset
 *                occured, FALSE otherwise).
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in as an argument.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not
 *          available on this version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_pbl_status(unsigned pmic_device_index, pm_pon_pbl_status_type pbl_status, boolean *status);


/**
 * @brief This function is to get the spare register value
 *        pm_pon_get_spare_reg  API.
 * 
 * @param[in] pmic_device_index. Primary PMIC: 0 Secondary PMIC: 1
                     spare_reg.  PM_PON_PERPH_RB_SPARE,  PM_PON_DVDD_RB_SPARE, 
                                 PM_PON_XVDD_RB_SPARE,  PM_PON_SOFT_RB_SPARE  
 * @param[out] status:  
 *                 returns the value of the specified spare register.
 *
 *
 * @return  pm_err_flag_type 
 *          PM_ERR_FLAG__INVALID_POINTER = Null pointer passed
 *          in as an argument.
 *          PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not
 *          available on this version of the PMIC.
 *          PM_ERR_FLAG__PAR1_OUT_OF_RANGE = Device Index out of
 *          range.
 *          PM_ERR_FLAG__SUCCESS = SUCCESS.
 *
 */
pm_err_flag_type 
pm_pon_get_spare_reg_data(uint8 pmic_device_index,pm_pon_spare_reg_type spare_reg ,uint8* data_ptr);



/*!
 * @brief  This function returns the phone uvlo reset status. Used in boot
 *         to determine if we should go into DLOAD mode.
 *
 * INPUT PARAMETERS
 * @param pmic_device_index When the target has more than
 *          one pmic, this is the index of the PMIC in which
 *          the power on module is physically located. The device
 *          index starts at zero.
 * @param status
 *    - pointer to boolean that indicates if we reset due to UVLO reset
 *
 * @return pm_err_flag_type.
 *         PM_ERR_FLAG__PAR1_OUT_OF_RANGE     = Input Parameter one is out of range.
 *         PM_ERR_FLAG__FEATURE_NOT_SUPPORTED = Feature not available on this 
 *                                       version of the PMIC.
 *         PM_ERR_FLAG__SUCCESS               = SUCCESS.
 *
 * @par Dependencies
 *      and pm_init() must have been called.
*/
pm_err_flag_type 
pm_pon_uvlo_reset_status(uint8 pmic_device_index,boolean * status);

/**
 * @brief Configures PMIC to act on MSM PS_HOLD toggle. This
 *        is an app level API which handles all the required
 *        PS_HOLD config for all the applicable PMICs
 *        internally.
 *  
 * @details Configure PMIC to act on MSM PS_HOLD state. 
 *  
 * @param ps_hold_cfg 
 *          PMAPP_PS_HOLD_CFG_WARM_RESET,
 *          PMAPP_PS_HOLD_CFG_HARD_RESET,
 *          PMAPP_PS_HOLD_CFG_NORMAL_SHUTDOWN.
 *   
 *                   
 * @return pm_err_flag_type PM_ERR_FLAG__SUCCESS = SUCCESS else 
 *         ERROR.
 */
pm_err_flag_type
pmapp_ps_hold_cfg(pmapp_ps_hold_cfg_type ps_hold_cfg);

/** @} */ /* end_addtogroup pm_pon */
#endif // PM_PON__H
