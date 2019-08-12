/*===========================================================================

  Copyright (c) 2014, 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_enablement.h"
#include "cpr_logging.h"
#include "CoreVerify.h"

/* Get Value from xml*/
#define GET_XML_VALUE(cpr_DevCfg,str,val) \
         CORE_DAL_VERIFY(DALSYS_GetPropertyValue( cpr_DevCfg, str, 0,val)); 

/* Read value and store in respective field*/  
#define MERGE_STATIC_MARGIN(val,static_margin)\
     	if( val != 0xDEADBEFF)\
          static_margin += val; 


extern cpr_config_rail_voltage_ranges_t mss_8937_voltage_ranges;
extern cpr_closed_loop_rail_config_t mss_8937_closed_loop_config;

static const cpr_enablement_versioned_rail_config_t* cpr_enablement_find_device_config(const cpr_enablement_rail_config_t* enablement_config)
{
    for(int i=0; i<enablement_config->versioned_rail_config_count; i++)
    {
        const cpr_enablement_versioned_rail_config_t* versioned_enablement_config = enablement_config->versioned_rail_config[i];
        const cpr_config_hw_version_range* hw_versions = &versioned_enablement_config->hw_versions;
        //CPR_LOGGING(1, "cpr_enablement_find_device_config for rail: %s  ", cpr_logging_get_ascii_rail_id_name(enablement_config->rail_id));
        if(cpr_device_hw_version_matches_this_device(hw_versions))
        {
                return versioned_enablement_config;
        }
    }   
    return NULL;
}

uint32 cpr_enablement_number_of_rails(void)
{
    return cpr_bsp_enablement_config.rail_enablement_config_count;
}

void cpr_enablement_rail_info(uint32 index, cpr_rail_id_t* rail_id, const cpr_enablement_versioned_rail_config_t** rail_enablement_versioned_config)
{
    CORE_VERIFY(index<cpr_bsp_enablement_config.rail_enablement_config_count);
   
    const cpr_enablement_rail_config_t* rail_enablement_config = cpr_bsp_enablement_config.rail_enablement_config[index];
    *rail_id = rail_enablement_config->rail_id;
    *rail_enablement_versioned_config = cpr_enablement_find_device_config(rail_enablement_config);
}

const cpr_enablement_versioned_rail_config_t* cpr_enablement_find_versioned_rail_config(cpr_rail_id_t cpr_rail_id)
{
    for(int i = 0; i < cpr_bsp_enablement_config.rail_enablement_config_count; i++)
    {
        if(cpr_bsp_enablement_config.rail_enablement_config[i]->rail_id == cpr_rail_id)
        {
            return cpr_enablement_find_device_config(cpr_bsp_enablement_config.rail_enablement_config[i]);
        }
    }
    return NULL;
}
  
/* Based on XML read modify different parameters of CPR*/
void cpr_enablement_from_xml(int32 rail_id, cpr_enablement_versioned_rail_config_t** rail_enablement_config)
{
    unsigned int xml_read;                      //flag to decide reading data from XML or not.
    DALSYS_PROPERTY_HANDLE_DECLARE(xml_read_devcfg);
    DALSYSPropertyVar prop;

    CPR_LOGGING(6, "cpr_enablement_from_xml(): rail_enablement_config: 0x%x, *rail_enablement_config: 0x%x,"
                   " enablement_init_params: 0x%x,"
                   " supported_level: 0x%x, supported_level_count: %d, raid_id: %d",
                    rail_enablement_config, *rail_enablement_config,
                    (*rail_enablement_config)->supported_level,
                    (*rail_enablement_config)->supported_level_count,
		    rail_id);
					
    //Get the handle for xml_read in XML using DAL
    CORE_DAL_VERIFY(DALSYS_GetDALPropertyHandleStr("/dev/core/power/cpr",xml_read_devcfg)); 
    GET_XML_VALUE(xml_read_devcfg,"XML_READ",&prop);
    xml_read = (unsigned int)prop.Val.dwVal;
    
    if (xml_read == 0) return;

    if (CPR_RAIL_MSS == rail_id)
    {
        unsigned int disable_aging;
        unsigned int fused_floor_disable;
        unsigned int mss_cpr_config; //to hold mss configuration from XML
        DALSYS_PROPERTY_HANDLE_DECLARE(mss_cpr_DevCfg);

        //Get the handle for cx cpr configuration in XML using DAL
        CORE_DAL_VERIFY(DALSYS_GetDALPropertyHandleStr("/dev/core/power/cpr",mss_cpr_DevCfg));
        GET_XML_VALUE(mss_cpr_DevCfg,"MSS_CPR_ENABLEMENT",&prop);
        mss_cpr_config = (unsigned int)prop.Val.dwVal;
    
	//Based on the value in XML, switch between the various cpr configurations 
   	switch(mss_cpr_config)
	{
            case 0  :
                (*rail_enablement_config)->enablement_init_params = &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE;
                return;
            case 1  :
                (*rail_enablement_config)->enablement_init_params = &CPR_ENABLE_OPEN_LOOP;
                return;
            case 2  : 
                (*rail_enablement_config)->enablement_init_params = &CPR_ENABLE_CLOSED_LOOP; 
	        break;
            case 0XDEADBEFF :
                CPR_LOGGING(1,"XML read is enabled : Default Rail Enablement = %x ",(*rail_enablement_config)->enablement_init_params);
	        return;
     
            default :
	        CPR_LOGGING(0,"MSS CPR ENABLEMENT in XML config is not valid");
	        CORE_VERIFY(0);
				
        }

        for(int i  = 0;i < (*rail_enablement_config)->supported_level_count ; i++)
        {
            switch((*rail_enablement_config)->supported_level[i].voltage_mode)
            {
                case CPR_VOLTAGE_MODE_SVS2:
                    GET_XML_VALUE(mss_cpr_DevCfg,"SVS_LOW_STATIC_MARGIN",&prop);
                    break;

                case CPR_VOLTAGE_MODE_SVS:
                    GET_XML_VALUE(mss_cpr_DevCfg,"SVS_STATIC_MARGIN",&prop);
                    break;

                case CPR_VOLTAGE_MODE_SVS_L1:
                    GET_XML_VALUE(mss_cpr_DevCfg,"SVS_HIGH_STATIC_MARGIN",&prop);
                    break;

                case CPR_VOLTAGE_MODE_NOMINAL:
                    GET_XML_VALUE(mss_cpr_DevCfg,"NOM_STATIC_MARGIN",&prop);
                    break;

                case CPR_VOLTAGE_MODE_NOMINAL_L1:
                    GET_XML_VALUE(mss_cpr_DevCfg,"NOM_HIGH_STATIC_MARGIN",&prop);
                    break;

                case CPR_VOLTAGE_MODE_TURBO:
                    GET_XML_VALUE(mss_cpr_DevCfg,"TURBO_STATIC_MARGIN",&prop);
                    break;

                default:
                    /* Invalid Mode */
                    CORE_VERIFY(0);
                    break;
            }  
            MERGE_STATIC_MARGIN((int32)prop.Val.dwVal,(*rail_enablement_config)->supported_level[i].static_margin_mv);
    
        }
	    //finding whether to enable or disable aging based on the values from XML 
        GET_XML_VALUE(mss_cpr_DevCfg,"AGING_DISABLE",&prop);
        disable_aging = (uint32)prop.Val.dwVal;
	   
	    //finding whether to disable fused floor or not based on the values from XML 
        GET_XML_VALUE(mss_cpr_DevCfg,"FUSED_FLOOR",&prop);
        fused_floor_disable = (uint32)prop.Val.dwVal;
		   
	    //Based on the value in XML, disable or enable dynamic fused floor
   	    switch(fused_floor_disable)
	    {
                case 0  :
                   mss_8937_voltage_ranges.disable_fused_floor = true;
                   break;
                case 1  :
                    mss_8937_voltage_ranges.disable_fused_floor = false;
                    break;
                default :
                    break;
            }
		   
	    //Based on the value in XML, enable or disable aging measurements 
        if(disable_aging == 1 )
            mss_8937_closed_loop_config.aging_sensor_count = 0;
	else
	    return; 
    } 
    return;
}
