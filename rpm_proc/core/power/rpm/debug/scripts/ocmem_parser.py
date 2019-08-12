import sys, re, os
from base_parser import Parser
from target_data import get_branch

class OCMemApply:
    __metaclass__ = Parser
    id = 0x280
    def parse(self, data):
        return 'rpm_ocmem_apply: applying change for (client: %d) (region: %d) (bank: %d)' % (data[0], data[1], data[2])

class OCMemUpdateBankSetting:
    __metaclass__ = Parser
    id = 0x281
    def parse(self, data):
        if get_branch('major') == 1:
            return 'rpm_ocmem_update_bank_setting: (old_vote: %d) (new_vote: %d)' % (data[0], data[1])
        elif get_branch('major') == 2:
            return 'rpm_ocmem_update_bank_setting: (old_vote: %d) (new_vote: %d) (active_votes: %d) (retention_votes: %d)' % (data[0], data[1], data[2], data[3])
