import sys, re, os
from base_parser import Parser
from target_data import get_master_name, get_set_name, get_sleep_mode, get_resource_name

class RAILWAYChangeVoltage:
    __metaclass__ = Parser
    id = 0x28A
    def parse(self, data):
        if data[0] == 0:
            return 'railway_change_voltage (rail: Mx) (new microvolts: %d)' % data[1]
        elif data[0] == 1:
            return 'railway_change_voltage (rail: Cx) (new microvolts: %d)' % data[1]
        else:
            return 'railway_change_voltage (rail: Gfx) (new microvolts: %d)' % data[1]
