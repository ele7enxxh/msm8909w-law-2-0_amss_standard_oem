#ifndef _SYSDBG_SPMI_H
#define _SYSDBG_SPMI_H

#include "comdef.h"

// This is the channel on which all SPMI transactions will be made 
#define SPMI_OWNER_ID 0

#define ENUM_32BITS(x) x##_INVALID = 0x7FFFFFFF

// We are only supporting polling for the driver 

// Total number of slaves supported by the SPMI bus
// Each slave maps to a PMIC. (hopefully !)
typedef enum 
{
	SPMI_SLAVE_0,
	SPMI_SLAVE_1,
	SPMI_SLAVE_2,
	SPMI_SLAVE_3,
	SPMI_SLAVE_4,
	SPMI_SLAVE_5,
	SPMI_SLAVE_6,
	SPMI_SLAVE_7,
	SPMI_SLAVE_COUNT,
}spmi_slave_type;

// Various commands to be issued to the SPMI Controller 
typedef enum 
{
	SPMI_CMD_EXT_REG_WRITE_LONG  	 = 0x0,
	SPMI_CMD_EXT_REG_READ_LONG  	 = 0x1,
	SPMI_CMD_EXT_REG_READ_LONG_DEL   = 0x2,
	SPMI_CMD_RESET					 = 0x4,
	SPMI_CMD_SLEEP					 = 0x5,
	SPMI_CMD_SHUTDOWN				 = 0x6,
	SPMI_CMD_WAKEUP					 = 0x7,
	SPMI_CMD_EXT_REG_WRITE			 = 0x8,
	SPMI_CMD_EXT_REG_READ			 = 0x9,
	SPMI_CMD_REG_WRITE				 = 0xA,
    SPMI_CMD_REG_READ				 = 0xB,
	SPMI_CMD_REG_WRITE_ZER0			 = 0xC,
	SPMI_CMD_MASTER_READ			 = 0xD,
	SPMI_CMD_MASTER_WRITE			 = 0xE,
	ENUM_32BITS(SPMI_CMD),
}spmi_command_type;

typedef struct 
{
	spmi_slave_type slaveid; // Slave to access 
	spmi_command_type command; // Command to send
	uint32 address; // Address of the register
	uint32 datalen; // Length of the data
    uint32 data[4]; // must be a 32 bit aligned address. We have a max read write size on Aragorn of 8 bytes. But allocating two extra words for this. 	
	// We could add priority here but we are going to use max priority of 1 always
}spmidatatype;

/*===========================================================================
**  Function : SPMI_INIT
** ==========================================================================
*/
/*!
* 
* @brief
*	SPMI driver init
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*
*
*/
boolean spmi_init(void);

/*===========================================================================
**  Function : SPMI_READDATA. SPMI_WRITEDATA
** ==========================================================================
*/
/*!
* 
* @brief
*	API to make SPMI reads / writes
*
* @param[in] 
*	None
*  
* @par Dependencies
*	None
* 
* @retval
*   None
* 
* @par Side Effects
*   None
*
*
*/
boolean spmi_readdata(spmidatatype *data);
boolean spmi_writedata(spmidatatype *data);

#endif /* _SYSDBG_SPMI_H */
