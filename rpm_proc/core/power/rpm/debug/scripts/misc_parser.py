import sys, re, os
from base_parser import Parser
from target_data import get_master_name, get_set_name, get_sleep_mode, get_resource_name, get_name_from_resource_id

class MISCGFXPhase:
    __metaclass__ = Parser
    id = 0x2C6
    def parse(self, data):
        return 'misc_gfx_phase: (phase: %d) (corner: %d) (IDDQ: %d) (temp level: %d)' % (data[0],data[1],data[2],data[3])

class MISCCXSinglePhase:
    __metaclass__ = Parser
    id = 0x2C7
    def parse(self, data):
        return 'misc_cx_singlephase: (corner: %d) (IDDQ: %d) (VPU: %d) (temp level: %d)' % (data[0],data[1],data[2],data[3])

class MISCCXDualPhase:
    __metaclass__ = Parser
    id = 0x2C8
    def parse(self, data):
        return 'misc_cx_dualphase: (corner: %d) (IDDQ: %d) (VPU: %d) (temp level: %d)' % (data[0],data[1],data[2],data[3])

class MISCCXLimitChange:
    __metaclass__ = Parser
    id = 0x2D8 # 728
    def parse(self, data):
        return 'misc_cx_limiter_change: (old limit: %d) (new limit: %d)' % (data[0], data[1])

class MISCCXLimitExceed:
    __metaclass__ = Parser
    id = 0x2D9 # 729
    def parse(self, data):
        return 'misc_cx_limiter_exceed: (corner: %d) (limit: %d)' % (data[0], data[1])

