
//DEBUG
#include "DALDeviceId.h"
#include "I2cDevice.h"

#define I2C_MAX_DATA   256
#define I2C_SLAVE_ADDR 0x52
#define I2C_BUS_FREQ   400
#define EEPROM_OFFSET  0X2000


static uint32 i2cTestLen  = I2C_MAX_DATA;
static uint32 i2c_test_id = DALDEVICEID_I2C_DEVICE_5;

void i2c_read_eeprom(I2CDEV_HANDLE hI2C, uint32 offset, uint8* pRdBuff, uint32 size)
{
    I2cClientConfig clntCfg = {I2C_BUS_FREQ, 2500};
    uint8           tmp_offset[2] = {0x20, 0x0};
    I2cBuffDesc     rdIoVecs[2];
    I2cTransfer     rdTrans[2];
    I2cSequence     rdSeq;
    I2cIoResult     ioRes;
	uint32			uReadSize;
	uint32          uWriteSize;
	static boolean batchTxfer = FALSE;


    tmp_offset[0] = (uint8)((offset >>8) & 0xff);
    tmp_offset[1] = (uint8)(offset & 0xff);
    rdIoVecs[0].pBuff     = &tmp_offset[0];
    rdIoVecs[0].uBuffSize = 2;

    rdTrans[0].pI2cBuffDesc       = &rdIoVecs[0];
    rdTrans[0].uTrSize            = rdIoVecs[0].uBuffSize;
    rdTrans[0].tranCfg.uSlaveAddr = I2C_SLAVE_ADDR;
    rdTrans[0].eTranDirection     = I2cTranDirOut;
    rdTrans[0].eTranCtxt          = I2cTrCtxSeqStart;

    if(!batchTxfer)
   	  I2CDEV_Write(hI2C, &rdTrans[0], &clntCfg, &uWriteSize);

    rdIoVecs[1].pBuff             = pRdBuff;
    rdIoVecs[1].uBuffSize         = size;
    rdTrans[1].pI2cBuffDesc       = &rdIoVecs[1];
    rdTrans[1].uTrSize            = rdIoVecs[1].uBuffSize;
    rdTrans[1].tranCfg.uSlaveAddr = I2C_SLAVE_ADDR;
    rdTrans[1].eTranDirection     = I2cTranDirIn;
    rdTrans[1].eTranCtxt          = I2cTrCtxSeqEnd;
   
    if(!batchTxfer)
	  I2CDEV_Read(hI2C, &rdTrans[1], &clntCfg , &uReadSize);

	if(batchTxfer)
	{
	  rdSeq.pTransfer     = rdTrans;
      rdSeq.uNumTransfers  = 2;
      I2CDEV_BatchTransfer(hI2C, &rdSeq, &clntCfg, &ioRes);	
	}

}

void i2c_write_eeprom(I2CDEV_HANDLE hI2C, uint32 offset, uint8* pRdBuff, uint32 size)
{
    I2cClientConfig clntCfg = {I2C_BUS_FREQ, 2500};
    uint8           tmp_offset[2] = {0x20, 0x0};

    I2cBuffDesc     wrIoVecs[2];
    I2cTransfer     wrTrans;
    uint32          uWriteSize;

    tmp_offset[0] = (uint8)((offset >>8) & 0xff);
    tmp_offset[1] = (uint8)(offset & 0xff);

    wrIoVecs[0].pBuff     = &tmp_offset[0];
    wrIoVecs[0].uBuffSize = 2;
    wrIoVecs[1].pBuff     = pRdBuff;
    wrIoVecs[1].uBuffSize = size;

    wrTrans.pI2cBuffDesc       = &wrIoVecs[0];
    wrTrans.uTrSize         = wrIoVecs[0].uBuffSize + wrIoVecs[1].uBuffSize;
    wrTrans.tranCfg.uSlaveAddr = I2C_SLAVE_ADDR;
    wrTrans.eTranDirection     = I2cTranDirOut;
    wrTrans.eTranCtxt = I2cTrCtxNotASequence;

    I2CDEV_Write(hI2C, &wrTrans, &clntCfg, &uWriteSize);

}


void i2c_test(void)
{
   I2CDEV_HANDLE  hI2C = NULL;
   int32          i2cResult;
   uint8          store_data[I2C_MAX_DATA];
   uint8          rd_data[I2C_MAX_DATA] = {0xA5,0x5A,0xA5,0x5A,0xA5, 0x5A,0xA5,0x5A,0xA5,0x5A};
   uint8          wr_data[I2C_MAX_DATA] = {1,2,3,4,5,6,7,8,9,10};
   int32          ind=0, i=0,j=0;
   

   i2cResult = I2CDEV_Init(i2c_test_id, &hI2C);
   if (I2C_RES_SUCCESS != i2cResult)
   {
      return;     
   }

   i2cResult = I2CDEV_SetPowerState(hI2C, I2CDEV_POWER_STATE_2);
   if (I2C_RES_SUCCESS != i2cResult)
   {
      return;     
   }

   if(i2cTestLen > I2C_MAX_DATA)
   {
     i2cTestLen = I2C_MAX_DATA;
     return;
   }

   //Gen write data
   i = 0;
   while(i < i2cTestLen)
   {
     wr_data[i] = i;
	 i++;
   }
    
   //Gen read data
   j = 0;
   while(j < i2cTestLen)
   {
     if(j%2)
     {
        rd_data[j] = 0xA5;
     }
	 else
	 {
        rd_data[j] = 0x5A;
	 }
	 
	 j++;
   }   
   
   // store the data
   i2c_read_eeprom(hI2C,  EEPROM_OFFSET, store_data, i2cTestLen); 

   i2c_write_eeprom(hI2C, EEPROM_OFFSET, wr_data,    i2cTestLen); 

   i2c_read_eeprom(hI2C,  EEPROM_OFFSET, rd_data,    i2cTestLen);

   for (ind=0; ind < i2cTestLen; ind++)
   {
      if (rd_data[ind] != wr_data[ind]) break;
   }

   // restore previous content
   i2c_write_eeprom(hI2C, EEPROM_OFFSET, store_data, i2cTestLen);

   I2CDEV_SetPowerState(hI2C, I2CDEV_POWER_STATE_0);

}

