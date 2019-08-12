import sys, re, os
from base_parser import Parser
from target_data import get_icb_master_name, get_icb_slave_name

class ICBInit: 
  __metaclass__ = Parser 
  id = 64 
  def parse(self, data): 
    return 'icb_init_cb complete'

class ICBSlavesTranslate: 
  __metaclass__ = Parser 
  id = 65 
  def parse(self, data): 
    slave = get_icb_slave_name(data[0])
    bandwidth = data[1] << 32 | data[2]
    return 'icb_slaves_translate_cb (slave: %s) (bandwidth: %i)' % (slave, bandwidth)

class ICBSlavesApply: 
  __metaclass__ = Parser 
  id = 66 
  def parse(self, data): 
    slave = get_icb_slave_name(data[0])
    bandwidth = data[1] << 32 | data[2]
    return 'icb_slaves_apply_cb (slave: %s) (bandwidth: %i)' % (slave, bandwidth)

class ICBMastersTranslate: 
  __metaclass__ = Parser 
  id = 67 
  def parse(self, data): 
    master = get_icb_master_name(data[0])
    bandwidth = data[1] << 32 | data[2]
    return 'icb_masters_translate_cb (master: %s) (bandwidth: %i)' % (master, bandwidth)

class ICBMastersApply: 
  __metaclass__ = Parser 
  id = 68 
  def parse(self, data): 
    master = get_icb_master_name(data[0])
    bandwidth = data[1] << 32 | data[2]
    return 'icb_masters_apply_cb (master: %s) (bandwidth: %i)' % (master, bandwidth)

class ICBSpdmReqTranslate: 
  __metaclass__ = Parser 
  id = 69 
  def parse(self, data): 
    return 'icb_spdm_req_translate_cb (request: %d)' % (data[0])

class ICBSpdmReqApply: 
  __metaclass__ = Parser 
  id = 70 
  def parse(self, data): 
    return 'icb_spdm_req_apply_cb (request: %d)' % (data[0])

class ICBSpdmClockStatus: 
  __metaclass__ = Parser 
  id = 71 
  def parse(self, data): 
    return 'request_spdm_clock (request: %d)' % (data[0])

class SPMIFatalError: 
  __metaclass__ = Parser 
  id = 80 
  def parse(self, data): 
    return 'spmi_fatal_error: (result %d) (file %x) (line %d)' % (data[0], data[1], data[2])

class SPMIError: 
  __metaclass__ = Parser 
  id = 81 
  def parse(self, data): 
    return 'spmi_error: (result %x) (file %x) (line %d)' % (data[0], data[1], data[2])

class SPMIWarning: 
  __metaclass__ = Parser 
  id = 82 
  def parse(self, data): 
    return 'spmi_warning: (file %x) (line %d)' % (data[0], data[1])
