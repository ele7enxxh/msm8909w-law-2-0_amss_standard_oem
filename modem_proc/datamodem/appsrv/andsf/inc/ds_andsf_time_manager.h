/*==============================================================================

                          ds_andsf_time_manager.h

GENERAL DESCRIPTION
  ANDSF Time Manager

  Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/23/15    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_ANDSF_TIME_MANAGER_H
#define DS_ANDSF_TIME_MANAGER_H

#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_ASVector.h"

extern "C"
{
  #include "time_jul.h"
}


namespace Appsrv
{
namespace XML
{

/* Forward declaration */
class  ANDSF_Isrp;
class  ANDSF_TimeOfDay;
struct ANDSF_Date;
struct ANDSF_Time;

} /* namespace XML */


namespace ANDSF
{



class ANDSFTimeManager
{
public:
  bool check_time_criteria(const ASVector<XML::ANDSF_TimeOfDay*>& time_of_day) const;
  uint32 find_next_rule_timeout(const ASVector<XML::ANDSF_Isrp*>& isrp_list);
  void update_time();


private:
  bool check_time_criteria_helper(const XML::ANDSF_TimeOfDay* time_criteria) const;

  uint32 update_min_timeout(
                            XML::ANDSF_Date&      current_date,
                            XML::ANDSF_Time&      current_time,
                            uint8                 current_day_of_week,
                            XML::ANDSF_TimeOfDay* andsf_tod,
                            uint32                cur_sec,
                            uint32                min_sec
                            );
  uint32 time_to_start(
                       XML::ANDSF_Date&      current_date,
                       XML::ANDSF_Time&      current_time,
                       uint8                 current_day_of_week,
                       XML::ANDSF_TimeOfDay* andsf_tod
                       );
  uint32 time_to_stop(
                      XML::ANDSF_Date&      current_date,
                      XML::ANDSF_Time&      current_time,
                      uint8                 current_day_of_week,
                      XML::ANDSF_TimeOfDay* andsf_tod
                      );

  bool rule_is_active(
                      const XML::ANDSF_Date&      current_date,
                      const XML::ANDSF_Time&      current_time,
                      const XML::ANDSF_TimeOfDay* andsf_tod
                      ) const;
  bool rule_expired(
                    const XML::ANDSF_Date&      current_date,
                    const XML::ANDSF_Time&      current_time,
                    const XML::ANDSF_TimeOfDay* andsf_tod
                    ) const;
  uint32 get_secs(const XML::ANDSF_Date& input_date, const XML::ANDSF_Time& input_time);


  uint32 next_valid_weekday(uint8 today, uint8 rule_days);
  uint32 next_invalid_weekday(uint8 today, uint8 rule_days);

  uint32 day_offset(uint8 day);


private:
  // Time in Julian format
  time_julian_type current_julian_time;
};



} /* namespace ANDSF */
} /* namespace Appsrv */

#endif /* FEATURE_DATA_ANDSF_RULE_MGR */
#endif /* DS_ANDSF_TIME_MANAGER_H */
