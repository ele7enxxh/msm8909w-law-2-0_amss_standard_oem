/**
@file SpiDriver.c
@brief Serial Peripheral Interface (SPI) BUS API implementation 
*/

/*===========================================================================
Copyright (c) 2014-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/core.mpss/3.5.c12.3/buses/spi/src/driver/SpiDriver.c#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

when         who      what, where, why
----------   ---      ------------------------------------
06/08/16     ska	  Modified condition to check array bounds
10/07/15     ddk      Added APIs for init and deinit and separate dal attach/detach.
05/21/2015   vmk      Added QDSS SW Events
05/06/2014   vmk      Created

===========================================================================*/
#include "stringl.h"
#include "SpiDriverTypes.h"
#include "SpiDriver.h"
#include "SpiDevice.h"
#include "SpiLog.h"
#include "ddispi.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"


//#include "tracer_micro.h"

static boolean gSpi_init_done = FALSE;



spi_dev_info_s spi_dev_info[MAX_SUPPORTED_SPI_INSTANCES];

static DALDEVICEID spi_get_deviceid(spi_device_id_t device_id)
{
   DALDEVICEID dal_device_id;

   switch (device_id)
   {
      case SPI_DEVICE_1:
         dal_device_id = DALDEVICEID_SPI_DEVICE_1;
         break;
      case SPI_DEVICE_2:
         dal_device_id = DALDEVICEID_SPI_DEVICE_2;
         break;
      case SPI_DEVICE_3:
         dal_device_id = DALDEVICEID_SPI_DEVICE_3;
         break;
      case SPI_DEVICE_4:
         dal_device_id = DALDEVICEID_SPI_DEVICE_4;
         break;
      case SPI_DEVICE_5:
         dal_device_id = DALDEVICEID_SPI_DEVICE_5;
         break;
      case SPI_DEVICE_6:
         dal_device_id = DALDEVICEID_SPI_DEVICE_6;
         break;

      default:
         dal_device_id = DALDEVICEID_SPI_DEVICE_1;
         break;
   }

   return dal_device_id;
}

static void set_spi_device_params (const spi_config_t *p_config, SpiDeviceInfoType *p_spi_info)
{
   p_spi_info->deviceParameters.eClockMode = (SpiClockModeType)p_config->spi_clk_always_on;
   p_spi_info->deviceParameters.eClockPolarity = (SpiClockPolarityType)p_config->spi_clk_polarity;
   p_spi_info->deviceParameters.eShiftMode = (SpiShiftModeType)p_config->spi_shift_mode;
   p_spi_info->deviceParameters.u32DeassertionTime = p_config->deassertion_time_ns;
   p_spi_info->deviceParameters.u32MinSlaveFrequencyHz = p_config->min_slave_freq_hz;
   p_spi_info->deviceParameters.u32MaxSlaveFrequencyHz = p_config->max_slave_freq_hz;
   p_spi_info->deviceParameters.eCSPolarity = (SpiCSPolarityType)p_config->spi_cs_polarity;
   p_spi_info->deviceParameters.eCSMode = (SpiCSModeType)p_config->spi_cs_mode;
   p_spi_info->deviceBoardInfo.nSlaveNumber = p_config->spi_slave_index;
   p_spi_info->transferParameters.nNumBits = p_config->spi_bits_per_word;
   p_spi_info->transferParameters.eTransferMode = SPI_TRANSFER_MODE_DEFAULT;
   p_spi_info->transferParameters.eLoopbackMode = p_config->loopback_mode; 
   p_spi_info->transferParameters.eInputPacking = SPI_INPUT_PACKING_DISABLED;
   p_spi_info->transferParameters.eOutputUnpacking = SPI_OUTPUT_UNPACKING_DISABLED;
   p_spi_info->transferParameters.slaveTimeoutUs = 0;
}

void SpiInit (void)
{
   if (! gSpi_init_done )
   {
      SpiLog_Init();
      memset(&spi_dev_info[0],0,(sizeof(spi_dev_info)));
   }
   else
   {
      return;
   }
}


int32 spi_init(spi_device_id_t device_id)
{
   DALResult result = DAL_SUCCESS;
   int32 retval = SPI_OP_SUCCESS;

   do
   {
      if ( (device_id < SPI_DEVICE_1) || 
            (device_id >= MAX_SUPPORTED_SPI_INSTANCES) )
      {
         retval  = SPI_ERROR_INVALID_PARAM;
         SPIDEVICE_LOG(ERROR,"spi_init: Invalid device id \n");
         break;
      }

      if (!(spi_dev_info[device_id].status & SPI_DEVICE_INIT_STATE))
      {
         /* Init all the necessary settings */
         SpiInit();

         /* Device is now in init state */
         spi_dev_info[device_id].status |= SPI_DEVICE_INIT_STATE;
         SPIDEVICE_LOG(INFO,"spi_init: Completed \n");         
      }
  
      if ((spi_dev_info[device_id].phDevice) == NULL)
      {
         result = DAL_DeviceAttach(spi_get_deviceid(device_id), 
                                         &(spi_dev_info[device_id].phDevice));

         if(DAL_SUCCESS != result)
         {
            retval  = SPI_ERROR_DAL_ATTACH_FAILURE;
            SPIDEVICE_LOG(ERROR,"spi_init: Dal Attach failure result = 0x%x \n", result);         
            break;
         }
      }
     
   }while(0);
   
   return retval;
}

int32 spi_open(spi_device_id_t device_id)
{
   DALResult result = DAL_SUCCESS;
   int32 retval = SPI_OP_SUCCESS;
   uint32 retval2;

   do
   {
      if ( (device_id < SPI_DEVICE_1) || 
            (device_id >= MAX_SUPPORTED_SPI_INSTANCES) )
      {
         retval  = SPI_ERROR_INVALID_PARAM;
         SPIDEVICE_LOG(ERROR,"spi_open: Invalid device id \n");
         break;
      }

      if (!(spi_dev_info[device_id].status & SPI_DEVICE_INIT_STATE))
      {
         /* Init all the necessary settings */
         SpiInit();

         /* Device is now in init state */
         spi_dev_info[device_id].status |= SPI_DEVICE_INIT_STATE;
         SPIDEVICE_LOG(INFO,"spi_open: Completed \n");         
      }

      /* Check if there is already an open client. 
      **  This wrapper supports only one open client per device.
      */
      
      if ((spi_dev_info[device_id].status & SPI_DEVICE_OPEN_STATE))
      {
         SPIDEVICE_LOG(ERROR, "spi_open() device_id already open: 0x%x", device_id);
         retval = SPI_ERROR_DEVICE_IN_USE;
         break;
      }      

      if ((spi_dev_info[device_id].phDevice) != NULL)
      {
         result = DalDevice_Open(spi_dev_info[device_id].phDevice, 
                                             DAL_OPEN_SHARED);

         if( DAL_SUCCESS != result )
         {           
            retval  = SPI_ERROR_DAL_OPEN_FAILURE;
            SPIDEVICE_LOG(ERROR,"spi_open: Dal Open failure result = 0x%x \n", result);                  
            break;       
         }
         else
         {
            /* Device is now open */
            spi_dev_info[device_id].status |= SPI_DEVICE_OPEN_STATE;
         }
      }
      else
      {
         retval  = SPI_ERROR_NULL_HANDLE;
         SPIDEVICE_LOG(ERROR,"spi_open: Failure result = 0x%x \n", result);                  
      }
     
   }while(0);

   if (retval)
   {
      retval2 = spi_close(device_id);

      if( SPI_OP_SUCCESS != retval2 )
      {           
         SPIDEVICE_LOG(ERROR,"spi_open: spi_close failure result = 0x%x \n", retval2);                  
      } 
   }
   
   return retval;
}

/* @copydoc spi_read */
int32 spi_read (spi_device_id_t device_id, const spi_config_t *p_config, spi_transaction_t *p_read_info)
{
   DALResult result = DAL_SUCCESS;
   int32 retval = SPI_OP_SUCCESS;
   uint32  u32NumInputTransfers = 0;
   SpiDataAddrType  inputAddr ;
   int32 inputDataLen = 0;
   int inputDataLenReq;
   SpiDeviceInfoType deviceInfo;
   
   do
   {
      if ( (device_id < SPI_DEVICE_1) || 
            (device_id >= MAX_SUPPORTED_SPI_INSTANCES) )
      {
         retval  = SPI_ERROR_INVALID_PARAM;
         SPIDEVICE_LOG(ERROR,"spi_read: Invalid device id \n");
         break;
      }

      /* Check for valid input parameters */
      if ((p_config == NULL) ||
          (p_read_info == NULL) ||
          (p_read_info == NULL) ||
          (p_read_info->buf_len == 0)  ||
          (p_read_info->buf_virt_addr == 0) )
      {
         SPIDEVICE_LOG(ERROR, "spi_read() invalid input parameters");
         retval = SPI_ERROR_INVALID_PARAM;
         break;
      }
      
      if ((spi_dev_info[device_id].phDevice) != NULL)
      {

         /* Check the device is open for writing */
         if (!(spi_dev_info[device_id].status & SPI_DEVICE_OPEN_STATE))
         {
            SPIDEVICE_LOG(ERROR, "spi_read() SPI device not open");
            retval = SPI_ERROR_DEVICE_NOT_OPEN;
            break;
         }

         /*Set SPI config*/
         set_spi_device_params(p_config, &deviceInfo);

         result = DalSpi_ConfigureDevice(spi_dev_info[device_id].phDevice,
                                              &deviceInfo);

         if(DAL_SUCCESS != result)
         {
            retval  = SPI_ERROR_DAL_CONFIGURE_FAILURE;
            SPIDEVICE_LOG(ERROR,
                  "spi_read: DalSpi_ConfigureDevice failure result = 0x%x \n", result);         
            break;
         }

         /* Set the number of transfers */
         u32NumInputTransfers = 
            (p_read_info->buf_len)/(p_config->spi_bits_per_word);
         
         if (! (p_read_info->buf_len)%(p_config->spi_bits_per_word))
         {
            u32NumInputTransfers++;
         }

         inputAddr.virtualAddr       =  p_read_info->buf_virt_addr;
         inputAddr.physicalAddr       =  p_read_info->buf_phys_addr;         
         inputDataLen  =  p_read_info->buf_len;

         result = DalSpi_SendAndReceive(spi_dev_info[device_id].phDevice,
                           NULL,
                           0,
                           u32NumInputTransfers,
                           &inputAddr,  
                           inputDataLen,  
                           &inputDataLenReq);

         if(SPI_COMPLETE != result)
         {
            retval  = SPI_ERROR_READ_FAILURE;
            SPIDEVICE_LOG(ERROR,
                  "spi_read: DalSpi_SendAndReceive failure result = 0x%x \n", result);         
            break;
         }
      }     
      else
      {
         retval  = SPI_ERROR_NULL_HANDLE;
         SPIDEVICE_LOG(ERROR,"spi_read: Dal Open failure result = 0x%x \n", result);                  
      }

   }while(0);
  
   return retval;
}

/* @copydoc spi_write */
int32 spi_write(spi_device_id_t device_id, const spi_config_t *p_config, spi_transaction_t *p_write_info)
{
   DALResult result = DAL_SUCCESS;
   int32 retval = SPI_OP_SUCCESS;
   SpiDataAddrType  outputAddr ;
   int inputDataLenReq;
   int32 outputDataLen = 0;
   SpiDeviceInfoType deviceInfo;
   
   do
   {
      if ( (device_id < SPI_DEVICE_1) || 
            (device_id >= MAX_SUPPORTED_SPI_INSTANCES) )
      {
         retval  = SPI_ERROR_INVALID_PARAM;
         SPIDEVICE_LOG(ERROR,"spi_write: Invalid device id \n");
         break;
      }

      /* Check for valid input parameters */
      if ((p_config == NULL) ||
          (p_write_info == NULL) ||
          (p_write_info == NULL) ||
          (p_write_info->buf_len == 0) ||
          (p_write_info->buf_virt_addr == 0))
      {
         SPIDEVICE_LOG(ERROR, "spi_write() invalid input parameters");
         retval = SPI_ERROR_INVALID_PARAM;
         break;
      }

      if ((spi_dev_info[device_id].phDevice) != NULL)
      {

         /* Check the device is open for writing */
         if (!(spi_dev_info[device_id].status & SPI_DEVICE_OPEN_STATE))
         {
            SPIDEVICE_LOG(ERROR, "spi_write() SPI device not open");
            retval = SPI_ERROR_DEVICE_NOT_OPEN;
            break;
         }

         /*Set SPI config*/
         set_spi_device_params(p_config, &deviceInfo);

         result = DalSpi_ConfigureDevice(spi_dev_info[device_id].phDevice,
                                              &deviceInfo);

         if(DAL_SUCCESS != result)
         {
            retval  = SPI_ERROR_DAL_CONFIGURE_FAILURE;
            SPIDEVICE_LOG(ERROR,
                  "spi_write: DalSpi_ConfigureDevice failure result = 0x%x \n", result);         
            break;
         }

         outputAddr.virtualAddr     =  p_write_info->buf_virt_addr;
         outputAddr.physicalAddr   =  p_write_info->buf_phys_addr;         
         outputDataLen                    =  p_write_info->buf_len;

         result = DalSpi_SendAndReceive(spi_dev_info[device_id].phDevice,
                           &outputAddr,
                           outputDataLen,
                           0,
                           NULL,  
                           0,  
                           &inputDataLenReq);


         if(SPI_COMPLETE != result)
         {
            retval  = SPI_ERROR_WRITE_FAILURE;
            SPIDEVICE_LOG(ERROR,
                  "spi_write: DalSpi_SendAndReceive failure result = 0x%x \n", result);         
            break;
         }
      }         
      else
      {
         retval  = SPI_ERROR_NULL_HANDLE;
         SPIDEVICE_LOG(ERROR,"spi_write: Failure result = 0x%x \n", result);                  
      }      
   }while(0);
  
   return retval;
}

int32 spi_full_duplex (spi_device_id_t device_id, const spi_config_t *p_config, spi_transaction_t *p_write_info, spi_transaction_t *p_read_info)
{
   DALResult result = DAL_SUCCESS;
   int32 retval = SPI_OP_SUCCESS;
   SpiDataAddrType  outputAddr ;
   SpiDataAddrType  inputAddr ;   
   int32 outputDataLen = 0;
   uint32  u32NumInputTransfers = 0;
   int32 inputDataLen = 0;
   int inputDataLenReq;
   SpiDeviceInfoType deviceInfo;
   
   do
   {
      if ( (device_id < SPI_DEVICE_1) || 
            (device_id >= MAX_SUPPORTED_SPI_INSTANCES) )
      {
         retval  = SPI_ERROR_INVALID_PARAM;
         SPIDEVICE_LOG(ERROR,"spi_full_duplex: Invalid device id \n");
         break;
      }

      /* Check for valid input parameters */
      if ((p_config == NULL) ||
          (p_write_info == NULL) ||
          (p_read_info == NULL) ||
          ((p_read_info->buf_len == 0) && (p_write_info->buf_len == 0)) ||
          ((p_read_info->buf_virt_addr == 0) && (p_write_info->buf_virt_addr == 0)))
      {
         SPIDEVICE_LOG(ERROR, "spi_full_duplex() invalid input parameters");
         retval = SPI_ERROR_INVALID_PARAM;
         break;
      }
      
      if ((spi_dev_info[device_id].phDevice) != NULL)
      {

         /* Check the device is open for writing */
         if (!(spi_dev_info[device_id].status & SPI_DEVICE_OPEN_STATE))
         {
            SPIDEVICE_LOG(ERROR, "spi_full_duplex() SPI device not open");
            retval = SPI_ERROR_DEVICE_NOT_OPEN;
            break;
         }

         /*Set SPI config*/
         set_spi_device_params(p_config, &deviceInfo);

         result = DalSpi_ConfigureDevice(spi_dev_info[device_id].phDevice,
                                              &deviceInfo);

         if(DAL_SUCCESS != result)
         {
            retval  = SPI_ERROR_DAL_CONFIGURE_FAILURE;
            SPIDEVICE_LOG(ERROR,
                  "spi_full_duplex: DalSpi_ConfigureDevice failure result = 0x%x \n", result);         
            break;
         }

         /* Set the number of transfers */
         u32NumInputTransfers = 
            (p_read_info->buf_len)/(p_config->spi_bits_per_word);
         
         if (! (p_read_info->buf_len)%(p_config->spi_bits_per_word))
         {
            u32NumInputTransfers++;
         }

         inputAddr.virtualAddr       =  p_read_info->buf_virt_addr;
         inputAddr.physicalAddr    =  p_read_info->buf_phys_addr;         
         inputDataLen                     =  p_read_info->buf_len;         

         outputAddr.virtualAddr     =  p_write_info->buf_virt_addr;
         outputAddr.physicalAddr  =  p_write_info->buf_phys_addr;         
         outputDataLen                   =  p_write_info->buf_len;

         result = DalSpi_SendAndReceive(spi_dev_info[device_id].phDevice,
                           &outputAddr,
                           outputDataLen,
                           u32NumInputTransfers,
                           &inputAddr,  
                           inputDataLen,  
                           &inputDataLenReq);

         if(SPI_COMPLETE != result)
         {
            retval  = SPI_ERROR_FULL_DUPLEX_FAILURE;
            SPIDEVICE_LOG(ERROR,
                  "spi_full_duplex: DalSpi_SendAndReceive failure result = 0x%x \n", result);         
            break;
         }
         else
         {
            p_write_info->total_bytes = p_write_info->buf_len;
            p_read_info->total_bytes = p_read_info->buf_len;
         }
      }     
      else
      {
         retval  = SPI_ERROR_NULL_HANDLE;
         SPIDEVICE_LOG(ERROR,"spi_full_duplex: Failure result = 0x%x \n", result);                  
      }
   }while(0);
  
   return retval;
}

int32 spi_close(spi_device_id_t device_id)
{
   DALResult result = DAL_SUCCESS;
   int32 retval = SPI_OP_SUCCESS;

   do
   {
      if ( (device_id < SPI_DEVICE_1) || 
            (device_id >= MAX_SUPPORTED_SPI_INSTANCES) )
      {
         retval  = SPI_ERROR_INVALID_PARAM;
         SPIDEVICE_LOG(ERROR,"spi_close: Invalid device id \n");
         break;
      }

      if ((spi_dev_info[device_id].phDevice) != NULL)
      {
         result = DalDevice_Close(spi_dev_info[device_id].phDevice);

         if(DAL_SUCCESS != result)
         {
            retval  = SPI_ERROR_DAL_CLOSE_FAILURE;
            SPIDEVICE_LOG(ERROR,"spi_close: Dal Close failure result = 0x%x \n", result);         
            break;
         }
         else
         {
            /* Device is now closed */
            spi_dev_info[device_id].status &= ~SPI_DEVICE_OPEN_STATE;
         }
      } 
      else
      {
         retval  = SPI_ERROR_NULL_HANDLE;
         SPIDEVICE_LOG(ERROR,"spi_close: Failure result = 0x%x \n", result);                  
      }    
   }while(0);
  
   return retval;
}

int32 spi_deinit(spi_device_id_t device_id)
{
   DALResult result = DAL_SUCCESS;
   int32 retval = SPI_OP_SUCCESS;

   do
   {
      if ( (device_id < SPI_DEVICE_1) || 
            (device_id >= MAX_SUPPORTED_SPI_INSTANCES) )
      {
         retval  = SPI_ERROR_INVALID_PARAM;
         SPIDEVICE_LOG(ERROR,"spi_deinit: Invalid device id \n");
         break;
      }

      if ((spi_dev_info[device_id].phDevice) != NULL)
      {
         result = DAL_DeviceDetach(spi_dev_info[device_id].phDevice);

         if( DAL_SUCCESS != result )
         {           
            retval  = SPI_ERROR_DAL_DETACH_FAILURE;
            SPIDEVICE_LOG(ERROR,"spi_deinit: Dal Detach failure result = 0x%x \n", result);                  
            break;       
         }           
         else
         {
            /*Set the device context to NULL */
            spi_dev_info[device_id].phDevice = NULL;
         }
      }
      else
      {
         retval  = SPI_ERROR_NULL_HANDLE;
         SPIDEVICE_LOG(ERROR,"spi_close: Failure result = 0x%x \n", result);                  
      }
   }while(0);
  
   return retval;
}


