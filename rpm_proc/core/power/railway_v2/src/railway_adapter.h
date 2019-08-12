/*===========================================================================

  railway_adapter.h - handles proxying votes through RPM server, passing
                      through non-voltage requests and directing voltage
                      requests at railway itself

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RAILWAY_ADAPTER_H
#define RAILWAY_ADAPTER_H

//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------
typedef struct
{
  uint32 sw_enable;
  uint32 microvolts;
  railway_corner corner;
  rail_voltage_level voltage_level;
  pm_npa_sw_mode_smps_type sw_mode;
  railway_corner active_floor;
  rail_voltage_level active_floor_voltage_level;
} railway_voter_irep;
/*===========================================================================
  FUNCTION: railway_adapter_init

  DESCRIPTION:

  RETURN VALUE:
  ===========================================================================*/
void railway_adapter_init(void);


/*===========================================================================
  FUNCTION: rpm_rail_level_remap

  DESCRIPTION:map voltage level to corner

  RETURN VALUE:
  ===========================================================================*/
railway_corner rpm_rail_level_remap(rail_voltage_level voltage_level);

/*===========================================================================
  FUNCTION: rpm_rail_corner_remap

  DESCRIPTION:map corner to voltage level

  RETURN VALUE:
  ===========================================================================*/
rail_voltage_level rpm_rail_corner_remap(railway_corner corn);

#endif // RAILWAY_ADAPTER_H

