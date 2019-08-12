import sys, re, os
from base_parser import Parser
from target_data import *

class RBCPRPreSwitchEntry:
    __metaclass__ = Parser
    id = 0x29E
    def parse(self, data):
        if get_branch('major') == 1:
            return 'Place Holder, should not print'
        elif get_branch('major') == 2:
            return 'rbcpr_pre_swith_entry: (rail %d) (corner %d) (microvolts %d)' % (data[0], data[1], data[2])

class RBCPR_MSG_x29E:
    # RBCPRCornerUpdateRec
    # RBCPRPostSwitchEntry
    __metaclass__ = Parser
    id = 0x29F
    def parse(self, data):
        if get_branch('major') == 1:
            rail = get_rail_name(data[0])
            corner = get_corner_name(data[1])
            return 'rbcpr_corner_update_rec: (rail: %s) (corner: %s) (step error[%s] %d)' % (rail, corner, ('step up' if data[2] == 2 else ('step down' if data[2] == 1 else 'no step')), data[3])
        elif get_branch('major') == 2:
            return 'rbcpr_post_swith_entry: (rail %d) (corner %d) (microvolts %d)' % (data[0], data[1], data[2])
        
class RBCPRCornerUpdateAct:
    __metaclass__ = Parser
    id = 0x2A0
    def parse(self, data):
        rail = get_rail_name(data[0])
        return 'rbcpr_corner_update_act: (rail: %s) (hit floor? %s) (hit ceiling? %s) (result microvolts: %d)' % (rail, 'yes' if data[1] == 1 else 'no', 'yes' if data[2] == 1 else 'no', data[3])

class RBCPRIsr:
    __metaclass__ = Parser
    id = 0x2A1
    def parse(self, data):
        return 'rbcpr_isr: (task 0x%x)' % data[0] 
