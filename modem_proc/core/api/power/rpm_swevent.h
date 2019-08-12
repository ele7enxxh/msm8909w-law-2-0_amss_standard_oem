#include "comdef.h"

/* RPM Software Event Configure API  
 * 
 *  Configure a subset of RPM software events with the new settings                 
 *  RPM software events are partitioned into groups of 64                 
 *  This API configures one of those partitions                 
 *
 * @param  cfg_setting      The setting to applied to this subset of RPM software events 
 * @param  type             The type (subset) of RPM software events on which to apply the setting 
 * @param  swevent_bitmask  The bitmask of events to apply the setting 
 */
void rpm_swevent_cfg_send(uint32 setting, uint32 type, uint64 bitmask);

