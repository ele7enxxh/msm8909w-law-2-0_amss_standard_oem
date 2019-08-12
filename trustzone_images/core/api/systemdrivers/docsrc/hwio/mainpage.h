/**
  @mainpage Usage 

  The HWIO DAL is used for other drivers to map memory-mapped hardware regions
  (HWIO) into virtual memory.  It is designed to work even in a non-virtual
  environment and in such cases will simply return the physical address of
  the region being mapped.
  The APIs take a string name of the base region to map, as well as a
  pointer to fill in with the address of the region (either mapped virtually
  or the physical address depending on the environment).  The region will
  be mapped with the appropriate cache attributes.

  There are two general usage models for the HWIO DAL.  The first is for
  straight HWIO macro users, and the second is for more generic register
  offset handling without going through HWIO macros.

  @section S1 Usage for Standard HWIO Macros

  To map a HWIO region for use by the HWIO_* macros (i.e. HWIO_OUT, HWIO_IN -
  see HALhwio.h) the following method should be used:

  1. Declare base pointers globally.  The same base pointer can be defined in
  multiple files as they are automatically declared as "weak" references.
  The MYBASE value below should be the name of base to be mapped which
  can be found in the chip SWI, or from the msmhwioreg.h file in 
  core/api/systemdrivers/hwio.

  @code
    
  DECLARE_HWIO_BASE_PTR(MYBASE); 

  @endcode

  2. Map the region in some convenient initialization function:
    
  @code

  eResult = DAL_DeviceAttach(DALDEVICEID_HWIO, &phDalHWIO);
  if (eResult == DAL_SUCCESS)
  {
    DALHWIO_MAP_REGION(phDalHWIO, MYBASE);
  }

  @endcode

  3. Unmap the region when no long needed (i.e. DLL is unloaded):

  @code

  if (phDalHWIO != NULL)
  {
    DALHWIO_UNMAP_REGION(phDalHWIO, MYBASE);
  }

  @endcode

  @section S2 Generic Usage
  To map a HWIO region for general use by code without using the standard
  HWIO macros the below DAL APIs can be used directly:

  1. Declare the base pointer (here the name is arbitrary):

  @code
    
  uint8 *my_base_ptr = NULL;

  @endcode

  2. Map the region in some convenient initialization function:
    
  @code

  eResult = DAL_DeviceAttach(DALDEVICEID_HWIO, &phDalHWIO);
  if (eResult == DAL_SUCCESS)
  {
    DalHWIO_MapRegion(phDalHWIO, "MYBASE_BASE", &my_base_ptr);
  }

  @endcode

  3. Unmap the region when no long needed (i.e. DLL is unloaded):

  @code

  if (phDalHWIO != NULL)
  {
    DalHWIO_UnMapRegion(phDalHWIO, my_base_ptr);
  }

  @endcode

  @section S3 Suggested Methods for HAL/PDD Integration

  As a rule HAL (or "PDD") code should invoke DAL interfaces directly.
  Here are two suggested methods for using the HWIO DAL with a HAL-based
  driver:

  1. Pass the HAL the base address as part of the HAL_xxx_Init function.
  Driver layer retrieves the name of the base region from its properties
  file, calls into the HWIO DAL to map it, and passes the address into
  the HAL.
  This is simple but exports some target specific data outside of the
  HAL (namely the name of the base).  For example (all error checking
  removed for clarity):

  @code

  DALSYS_GetPropertyValue(hProps, "MyHALBaseName", 0, &PropVar);

  DalHWIO_MapRegion(phHWIO, PropVar.Val.pszVal, &nBasePtr);

  HAL_myhal_Init(nBasePtr);

  @endcode

  2. Export an API from the HAL that retrieves the name of the base
  and a pointer to a base pointer to fill in.  This is a bit more complex
  of an implementation but depending on the HAL design may make more sense.
  This method allows keeping the base name itself abstracted by the HAL, and
  also lets the HAL declare and use the normal HWIO base pointer that
  the standard HWIO macros (see HALhwio.h) use.

  @code

  void HAL_myhal_GetBaseAddress(char **pszName, uint8 **pnBaseAddress);

  @endcode

  For example (again, no error checking code here):

  @code

  HAL_myhal_GetBaseAddress(&szBaseName, &pnBasePtr);

  DalHWIO_MapRegion(phHWIO, szBaseName, pnBasePtr);

  HAL_myhal_Init();

  @endcode

*/

/**
@defgroup macros Macros

@defgroup functions Functions

@defgroup hwio_macros Register Access Macros
@brief These macros are used for doing abstracted, named, register access.

@defgroup deprecated_functions Deprecated Functions
@brief These functions are supported for now, but their use is discouraged.

*/

