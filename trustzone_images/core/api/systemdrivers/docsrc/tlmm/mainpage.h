/**
 
@defgroup const_macros Constants and Macros 
  Defined constants and macros that are publicly available for manipulating GPIO 
  configurations and accessing supported TLMM hardware (ports). 

@defgroup dir_enums Directive GPIO Enumerations 
  Enumerations that are not strictly associated with a GPIO configuration, but 
  are used to provide specific functionality and information with regard to a 
  particular GPIO. 

@defgroup config_enums Configuration-Specific Enumerations
  Enumerations that are strictly associated with the configuration of a GPIO 
  pin. Each enumeration is used as a parameter in the DAL_GPIO_CFG and 
  DAL_GPIO_CFG_OUT macros. 
     
  Following is an example of setting a GPIO as a software-driven output pin 
  (generic I/O):
  
  @code
    DAL_GPIO_CFG(29, 0, DAL_GPIO_OUTPUT, DAL_GPIO_NO_PULL, DAL_GPIO_2MA);
  @endcode
  
@defgroup block_configs TLMM Block Configurations 
  Enumerations that define the TLMM configurations that are not multiplexed on 
  a GPIO pin or whose configuration is not performed with GPIO pins.

@defgroup driver_apis TLMM DAL Driver APIs
  Interfaces that define public access to the TLMM DAL driver.
  
@defgroup code_examples TLMM DAL Coding Examples
  Provides examples on how to invoke the TLMM DAL driver to configure and 
  manipulate a GPIO.
  
  The first example demostrates how to get a device handle to TLMM DAL and 
  configure a GPIO.

  @note Call DAL_DeviceAttach() only once with the DalDeviceHandle pointer 
        declared in the global scope. The same handle can be used on a per-device 
        basis. For example, an I2S driver is to do only one device attach and 
        maintain one handle to the TLMM DAL device for all API calls.
  
  Each configuration of type DALGpioSignalType can contain target/device-specific 
  information. Because of this, we recommend that you do not keep the 
  target-specific parameters (such as GPIO number and function select) in the 
  common code. Keep these parameters in a board-specific file such as a BSP or 
  registry.

  @code
    #include "DDITlmm.h"
    #include "DalDevice.h"
    #include "DALDeviceID.h"

    // Declare a handle to the TLMM object.
    DalDeviceHandle * hTlmm
    DAL_Result nResult = DAL_ERROR;
    DALGpioSignalType i2s_config;
  
    // Call upon the DAL to retrieve a handle to the DALTLMM
    nResult = DAL_DeviceAttach(DALDEVICEID_TLMM, &hTlmm);
  
    if( DAL_ERROR == nResult )
    {
      // This is a fatal error.
      ERR_FATAL("Failed to acquire a handle to TLMM.",0,0,0);
    }
  
    // create a GPIO configuration (not in common code).
    i2s_config = DAL_GPIO_CFG(46,
                               1,
                               DAL_GPIO_OUTPUT,
                               DAL_GPIO_PULL_DOWN,
                               DAL_GPIO_2MA);
  
    // Call the TLMM DAL API to configure the GPIO.
    nResult = DalTlmm_ConfigGpio(hTlmm,
                                i2s_config,
                                DAL_TLMM_GPIO_ENABLE);
  
    if(DAL_SUCCESS == nResult)
    {
      // The GPIO was configured properly.
    }
    else
    {
      // Failed to configure the GPIO.  This may be due to not
      // having configuration access.
    }
  @endcode
  
@latexonly
\newpage
@endlatexonly
  The next example demonstrates how to request ownership of a GPIO. This may be 
  necessary on some multiprocessor systems where GPIO access is granted by the 
  secure root of trust processor. It is assumed that the device handle is already 
  initialized, as in the previous example.
  
  @code
    result = DalTlmm_RequestGpio(hTlmm, i2s_config);
  
    if( DAL_SUCCESS == result )
    {
      // We have ownership of the GPIO.
    }
    else
    {
      // This GPIO is not allowed to be remotely owned.
    }
  @endcode
  
*/

