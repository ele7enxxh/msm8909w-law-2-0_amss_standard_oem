// railway_stub.c - stubbed implementation of Railway Driver
//
// Copyright 2011 - 2013 by Qualcomm Technologies, Inc.
// All Rights Reserved
// Confidental and Proprietary
//
// $ Header$
// $ Author$
// $ DateTime$


#include "railway.h"
#include "boot_api.h"
#include "comdef.h"

void railway_init(void)
{}

int rail_id(const char* rail)
{
    return RAIL_NOT_SUPPORTED_BY_RAILWAY;
}

void railway_transition_rails(void)
{}

railway_voter_t railway_create_voter(int rail, int id)
{
    //BOOT_ERR_FATAL();    //API not supported in stubbed implementation
    return NULL;
}

void railway_corner_vote(railway_voter_t voter, railway_corner corner)
{
    //BOOT_ERR_FATAL();    //API not supported in stubbed implementation
}

void railway_set_corner_voltage(int rail, railway_corner corner, uint32 microvolts)
{
    //BOOT_ERR_FATAL();    //API not supported in stubbed implementation
}

uint32 railway_get_corner_voltage(int rail, railway_corner corner)
{
    //BOOT_ERR_FATAL();    //API not supported in stubbed implementation
    return 0;
}

void railway_get_current_settings(int rail, railway_settings* settings)
{
    //BOOT_ERR_FATAL();    //API not supported in stubbed implementation
}
