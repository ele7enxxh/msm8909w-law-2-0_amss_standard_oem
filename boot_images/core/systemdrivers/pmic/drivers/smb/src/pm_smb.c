/*! \file
*
*  \brief  pm_smb.c
*  \details Implementation file for SMB resourece type.
*
*  &copy; Copyright 2014-2015 QUALCOMM Technologies Incorporated, All Rights Reserved
*/

/*===========================================================================

EDIT HISTORY FOR MODULE

This document is created by a code generator, therefore this section will
not contain comments describing changes made to the module.

when       who     what, where, why
---------- ---     ----------------------------------------------------------
01/15/2015 pxm     Fix the problem that charging will be momentarily disabled when enabling smb1360 charging in weak charging New Features: Fix the problem that charging will be momentarily disabled when enabling smb1360 charging in weak charging
01/09/2015 pxm     CR770199 Fix msm8909+SMB1360 weak charging didn't work issue
08/19/2014 pxm     Creation
===========================================================================*/
#include <stdio.h>

#include "I2cDevice.h"
#include "DalDevice.h"
#include "DALStdErr.h"
#include "DDITlmm.h"

#include "pm_smb.h"
#include "boot_logger.h"
#include "pm_smb_driver.h"

#define I2C_VERIFY(result) \
    do \
    { \
        if(PM_ERR_FLAG__SUCCESS != (result)) { \
            snprintf(s_buffer, BUFFER_SIZE - 1, "In %s,result = %d line = %d failed\n",__func__,result,__LINE__); \
            boot_log_message(s_buffer); \
            return PM_ERR_FLAG__I2C_OPT_ERR; }\
    } while(0)

#define I2C_BIT_SET(address, bit)  smb_write_mask(address, bit, bit)

#define I2C_BIT_CLEAR(address, bit) smb_write_mask(address, 0, bit)

#define BUFFER_SIZE                     64

static I2cClientConfig config;
static I2CDEV_HANDLE  handle = NULL;
static char s_buffer[BUFFER_SIZE];
static boolean get_config_successful = FALSE;
static pm_smb_info_type* smb_info = NULL;

static pm_err_flag_type smb_write(uint8 offset, uint8  pWrBuff );
static pm_err_flag_type smb_read(uint8 offset, uint8* data );
static pm_err_flag_type smb_write_mask (uint8 offset, uint8  pWrBuff, uint8 mask);
static int get_index_from_ascent_list(int data, int* list, uint8 size);

/*
 * Read a byte from SMB with given offset/address.
 * @offset: The internal address of SMB to read a byte.
 * @data: The buffer to receive the reading result.
 *
 * @return PM_ERR_FLAG__I2C_OPT_ERR if I2C error happened
 *         PM_ERR_FLAG__SUCCESS if success
 */
pm_err_flag_type smb_read(uint8 offset, uint8* data )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    int32 i2cResult = I2C_RES_SUCCESS;

    // I2C read sequence need 2 transfer operation
    // Start | SMB address(W) | Register address | Start | SMB address(R) | DATA
    I2cBuffDesc rdIoVecs[2]; // 0 for register, 1 for buffer to receive data
    I2cTransfer rdTrans[2]; // 0 for write sequence writing register address
                            // 1 for read sequence to receive data
    I2cSequence rdSeq;
    I2cIoResult ioRes;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    rdIoVecs[0].pBuff             = &offset;
    rdIoVecs[0].uBuffSize         = 1;                  
    rdIoVecs[1].pBuff             = data;
    rdIoVecs[1].uBuffSize         = 1;

    rdTrans[0].pI2cBuffDesc       = &rdIoVecs[0];
    rdTrans[0].uTrSize            = rdIoVecs[0].uBuffSize;
    rdTrans[0].tranCfg.uSlaveAddr = smb_info->hw_info->slave_address;
    rdTrans[0].eTranDirection     = I2cTranDirOut;
    rdTrans[0].eTranCtxt          = I2cTrCtxSeqStart;

    rdTrans[1].pI2cBuffDesc       = &rdIoVecs[1];
    rdTrans[1].uTrSize            = rdIoVecs[1].uBuffSize;
    rdTrans[1].tranCfg.uSlaveAddr = smb_info->hw_info->slave_address; 
    rdTrans[1].eTranDirection     = I2cTranDirIn;
    rdTrans[1].eTranCtxt          = I2cTrCtxSeqEnd;

    rdSeq.pTransfer      = rdTrans;
    rdSeq.uNumTransfers  = 2;

    i2cResult = I2CDEV_BatchTransfer(handle, &rdSeq, &config, &ioRes);
    if(i2cResult != I2C_RES_SUCCESS)
    {
        err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
    }
    return err_flag;
}

/*
 * Write a byte into SMB in given offset/address.
 * @offset: The internal address of SMB to write the data.
 * @data: The data to be written into SMB.
 *
 * @return PM_ERR_FLAG__I2C_OPT_ERR if I2C error happened
 *         PM_ERR_FLAG__SUCCESS if success
 */
pm_err_flag_type smb_write(uint8 offset, uint8 data)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    int32 i2cResult = I2C_RES_SUCCESS;

    I2cBuffDesc wrIoVecs[2];
    I2cTransfer wrTrans;
    uint32 uWriteSize;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    wrIoVecs[0].pBuff          = &offset;
    wrIoVecs[0].uBuffSize      = 1; // 1 byte register offset
    wrIoVecs[1].pBuff          = &data;
    wrIoVecs[1].uBuffSize      = 1; // 1 byte data would be written

    wrTrans.pI2cBuffDesc       = wrIoVecs;
    wrTrans.uTrSize            = wrIoVecs[0].uBuffSize + wrIoVecs[1].uBuffSize;
    wrTrans.tranCfg.uSlaveAddr = smb_info->hw_info->slave_address;
    wrTrans.eTranDirection     = I2cTranDirOut;
    wrTrans.eTranCtxt          = I2cTrCtxNotASequence;

    i2cResult = I2CDEV_Write(handle, &wrTrans, &config, &uWriteSize);
    if(i2cResult != I2C_RES_SUCCESS)
    {
        err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
    }

    return err_flag;
}

/*
 * Write a byte into SMB in given offset/address with mask.
 * With mask the unnecessary bits would keep the old value.
 * @offset: The internal address of SMB to write the data.
 * @data: The data to be written into SMB.
 * @mask:
 *
 * @return PM_ERR_FLAG__I2C_OPT_ERR if I2C error happened
 *         PM_ERR_FLAG__SUCCESS if success
 */
pm_err_flag_type smb_write_mask (uint8 offset, uint8  data, uint8 mask)
{
   pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
   uint8 rdata;

   err_flag = smb_read(offset, &rdata);
   if(PM_ERR_FLAG__SUCCESS != err_flag)
   {
        return PM_ERR_FLAG__I2C_OPT_ERR;
   }

   rdata = (rdata & (~mask)) | data;

   err_flag = smb_write(offset, rdata);

   return err_flag;
}

pm_err_flag_type pm_smb_enable_volatile_access()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    I2C_VERIFY(I2C_BIT_SET(smb_info->feature_list->nv_access.offset_nv_access,
                        smb_info->feature_list->nv_access.bit_nv_access));
    return err_flag;
}

pm_err_flag_type pm_smb_enable_aicl(boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    if(enable)
    {
        I2C_VERIFY(I2C_BIT_SET(smb_info->feature_list->aicl.offset_aicl_enable,
                        smb_info->feature_list->aicl.bit_aicl_enable));
    }
    else
    {
        I2C_VERIFY(I2C_BIT_CLEAR(smb_info->feature_list->aicl.offset_aicl_enable,
                        smb_info->feature_list->aicl.bit_aicl_enable));
    }
    return err_flag;
}

pm_err_flag_type pm_smb_enable_charging(boolean enable)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean active_low = FALSE;
    uint8 data = 0;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    // get active polarity
    I2C_VERIFY(smb_read(smb_info->feature_list->charger_enable.offset_active_low, &data));
    active_low = (smb_info->feature_list->charger_enable.bit_active_low & data) != 0;

    if((active_low && enable) || (!active_low && !enable))
    {
        I2C_VERIFY(I2C_BIT_CLEAR(smb_info->feature_list->charger_enable.offset_charger_enable,
        smb_info->feature_list->charger_enable.bit_charger_enable));
    }
    else
    {
        I2C_VERIFY(I2C_BIT_SET(smb_info->feature_list->charger_enable.offset_charger_enable,
        smb_info->feature_list->charger_enable.bit_charger_enable));
    }

    // disable charger inhibit
    I2C_VERIFY(I2C_BIT_CLEAR(smb_info->feature_list->charger_enable.offset_charger_inhit,
        smb_info->feature_list->charger_enable.bit_charger_inhit));

    // charger enable by register instead of pin
    I2C_VERIFY(smb_read(smb_info->feature_list->charger_enable.offset_enable_by_pin, &data));
    if((data & smb_info->feature_list->charger_enable.bit_enable_by_pin) != 0)
    {
        I2C_VERIFY(I2C_BIT_CLEAR(smb_info->feature_list->charger_enable.offset_enable_by_pin,
            smb_info->feature_list->charger_enable.bit_enable_by_pin));
    }

    return err_flag;
}

pm_err_flag_type pm_smb_set_icl_mode(InputLimitMode mode)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean reset_to_500 = FALSE;
    uint8 data = 0;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    if(IL_AC == mode)
    {
        I2C_VERIFY(I2C_BIT_SET(smb_info->feature_list->mode_control.offset_usb_mode_ac,
            smb_info->feature_list->mode_control.bit_usb_mode_ac));
    }
    else
    {
        I2C_VERIFY(I2C_BIT_CLEAR(smb_info->feature_list->mode_control.offset_usb_mode_ac,
            smb_info->feature_list->mode_control.bit_usb_mode_ac));

        I2C_VERIFY(smb_read(smb_info->feature_list->mode_control.offset_usb51_reset_to_500, &data));
        if((data & smb_info->feature_list->mode_control.bit_usb51_reset_to_500) != 0)
        {
            // reset status 00b means USB500 mode
            reset_to_500 = TRUE;
        }

        if((reset_to_500 && (IL_USB500 == mode))
            || (!reset_to_500 && (IL_USB100 == mode)))
        {
            I2C_VERIFY(I2C_BIT_CLEAR(smb_info->feature_list->mode_control.offset_usb_mode_500_100,
                smb_info->feature_list->mode_control.bit_usb_mode_500_100));
        }
        else
        {
            I2C_VERIFY(I2C_BIT_SET(smb_info->feature_list->mode_control.offset_usb_mode_500_100,
                smb_info->feature_list->mode_control.bit_usb_mode_500_100));
        }
    }

    // usb ac mode controlled by register
    I2C_VERIFY(I2C_BIT_CLEAR(smb_info->feature_list->mode_control.offset_usb_ac_by_pin,
        smb_info->feature_list->mode_control.bit_usb_ac_by_pin));

    return err_flag;
}

pm_err_flag_type pm_smb_set_icl(uint32 current)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    int idx = -1;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    idx = get_index_from_ascent_list(current, smb_info->feature_list->icl_list, smb_info->feature_list->icl_list_size);

    if(-1 == idx)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    I2C_VERIFY(smb_write_mask(smb_info->feature_list->icl.offset_icl,
        idx << smb_info->feature_list->icl.bit_low,
        SMB_MASK(smb_info->feature_list->icl.bit_high, smb_info->feature_list->icl.bit_low)));

    return err_flag;
}

pm_err_flag_type pm_smb_set_ibat_fast(uint32 current)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    int idx = -1;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    idx = get_index_from_ascent_list(current, smb_info->feature_list->ibat_list, smb_info->feature_list->ibat_list_size);

    if(-1 == idx)
    {
        return PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
    }

    I2C_VERIFY(smb_write_mask(smb_info->feature_list->ibat.offset_ibat,
        idx << smb_info->feature_list->ibat.bit_low,
        SMB_MASK(smb_info->feature_list->ibat.bit_high, smb_info->feature_list->ibat.bit_low)));

    return err_flag;
}

pm_err_flag_type pm_smb_init()
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    uint32 result = 0;

    pm_smb_driver_init();
    smb_info = pm_smb_get_info();

    do
    {
        if(NULL != smb_info && NULL != smb_info->feature_list && NULL != smb_info->hw_info
            && NULL != smb_info->feature_list->ibat_list && NULL != smb_info->feature_list->icl_list)
        {
            get_config_successful = TRUE;
        }
        else
        {
            err_flag = PM_ERR_FLAG__SMB_CONFIG_ERROR;
            break;
        }

        config.uBusFreqKhz = smb_info->hw_info->bus_frequency;
        config.uByteTransferTimeoutUs = smb_info->hw_info->transfer_timeout_us;

        result = I2CDEV_Init(smb_info->hw_info->device_id, &handle);
        if(I2C_RES_SUCCESS != result)
        {
            err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
            break;
        }

        // Bus on, clock on
        result = I2CDEV_SetPowerState(handle, I2CDEV_POWER_STATE_2);
        if(I2C_RES_SUCCESS != result)
        {
            err_flag = PM_ERR_FLAG__I2C_OPT_ERR;
            I2CDEV_DeInit(handle);
            break;
        }
    } while(0);

    return err_flag;
}

pm_err_flag_type pm_smb_deinit()
{
    uint32 result = 0;

    if(!get_config_successful)
    {
        return PM_ERR_FLAG__SUCCESS;
    }

    result = I2CDEV_SetPowerState(handle, I2CDEV_POWER_STATE_0);
    I2CDEV_DeInit(handle); // return code of deinit isn't error code!

    if(I2C_RES_SUCCESS != result)
    {
        return PM_ERR_FLAG__I2C_OPT_ERR;
    }
    else
    {
        return PM_ERR_FLAG__SUCCESS;
    }
}

// return the closet index which satisfy list[index] <= data
// if data < list[0], return -1
int get_index_from_ascent_list(int data, int* list, uint8 size)
{
    int idx = -1;

    if(NULL != list && size > 0 && data >= list[0])
    {
        for(idx = size - 1; idx >= 0; --idx)
        {
            if(list[idx] <= data)
                break;
        }
    }

    return idx;
}

