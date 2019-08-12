/*===========================================================================
                              rbcpr_resource.c

SERVICES:

DESCRIPTION:

INITIALIZATION AND SEQUENCING REQUIREMENTS:
  Description...

Copyright (c) 2013 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

#include "rpm.h"
#include "rpmserver.h"
#include "rbcpr.h"

//For now, keep this simple. Right now we only need to potentially disable 1 sensor on the CPR chain.
//If we ever need to support disabling more than one sensor then we can revisit.

typedef struct {
  boolean disable_sensor;
} rbcpr_sensor_disable_irep;

typedef enum {
  LPASS_Q6_SENSOR = 0,
} rbcpr_sensors;

static int disable_lpass_q6_sensor_count = 0;

void rbcpr_xlate(rpm_translation_info *info)
{
  uint32               type, length;
  uint32               *value;
  rbcpr_sensor_disable_irep *req = info->dest_buffer;

  /* Read until we run out of KVPs. */
  while( !kvp_eof(info->new_kvps) )
  {
    kvp_get( info->new_kvps, &type, &length, (const char **)&value);

    if(length==0)
    {   /* Implies we need to invalidate any previous request the client 
           might have on this kvp type. */
      switch(type)
      {
        case LPASS_Q6_SENSOR:
          req->disable_sensor = false;
          break;
        default:
          break;
      }
    }
    else
    {
      switch(type)
      {
        case LPASS_Q6_SENSOR:
          assert(sizeof(uint32) == length);
          req->disable_sensor = (*value ? true : false);
          break;
        default:
          break;
      }
    }
  }
}

void rbcpr_apply(rpm_application_info *info)
{
    if(!info->old_state ||
        (((rbcpr_sensor_disable_irep*)info->new_state)->disable_sensor!=
         ((rbcpr_sensor_disable_irep*)info->old_state)->disable_sensor))
    {
        //Need to re-aggregate, something has changed
        
        if(((rbcpr_sensor_disable_irep*)info->new_state)->disable_sensor)
        {
            //Since something's change and we now have a vote to disable the sensor,
            //the old vote must have been to enable the sensor. Inc the count.
            disable_lpass_q6_sensor_count++;
        } else if(info->old_state) {
            //Since something's change and we now don't have a vote to disable the sensor,
            //the old vote must have been to disable the sensor. Dec the count.
            disable_lpass_q6_sensor_count--;
        }
        
        //Should never get a negative count. If we do then something's gone wrong.
        assert(disable_lpass_q6_sensor_count >=0);
        
        //Now update the master agregation in the RPM server. This is factored into the
        //RPM server estimates.
        ((rbcpr_sensor_disable_irep*)info->current_aggregation)->disable_sensor =
            disable_lpass_q6_sensor_count ? true : false;
        
        if(disable_lpass_q6_sensor_count==0)
        {
            //This implies that the count has just hit 0. Need to reenable the sensor.
            rbcpr_disable_lpass_q6_sensor(false);
        } else if ((disable_lpass_q6_sensor_count==1) && 
                  ((rbcpr_sensor_disable_irep*)info->new_state)->disable_sensor)
        {   //This implies that we've just got a new vote to disable the sensor and
            //we didn't have one previously. Time to disable the lpass q6 sensor.
            rbcpr_disable_lpass_q6_sensor(true);
        }
    }
}

void init_rbcpr_resource(void)
{
    static boolean already_initted = false;
    if(already_initted)
        return;
    already_initted = true;

    rpm_register_resource(RPM_RBCPR_REQ, 1, sizeof(rbcpr_sensor_disable_irep), rbcpr_xlate, rbcpr_apply, 0);
}
