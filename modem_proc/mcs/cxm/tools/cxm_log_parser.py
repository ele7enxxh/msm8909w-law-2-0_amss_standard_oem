# --------------------------------------------------------------------------- #
# File Name :: cxm_log_parser.py
# Purpose   :: Parses the output from wp.v.v for cxm logs and
# outputs a human-readable text file. Use the -h option on the
# command line for more details.
# --------------------------------------------------------------------------- #

# Copyright (c) 2013 Qualcomm Technologies Incorporated.
# All Rights Reserved.
# Qualcomm Confidential and Proprietary

# Export of this technology or software is regulated by the U.S. Government.
# Diversion contrary to U.S. law prohibited.

# All ideas, data and information contained in or disclosed by
# this document are confidential and proprietary information of
# Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
# By accepting this material the recipient agrees that this material
# and the information contained therein are held in confidence and in
# trust and will not be used, copied, reproduced in whole or in part,
# nor its contents revealed in any manner to others without the express
# written permission of Qualcomm Technologies Incorporated.
# --------------------------------------------------------------------------- #

# --------------------------------------------------------------------------- #
#                         RETURN CODES
# --------------------------------------------------------------------------- #
# This script returns the following exit codes:
#   0) executed successfully
#   1) general error
#   2) problem with input file specified
#   3) problem writing output file
#   4) dump_dir specified not valid

#==============================================================================
#
#                        EDIT HISTORY FOR MODULE
#
# This section contains comments describing changes made to the module.
# Notice that changes are listed in reverse chronological order.
#
# $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcs/cxm/tools/cxm_log_parser.py#1 $
#
# when       who    what, where, why
# --------   ---    -----------------------------------------------------------
# 11/06/14   btl    Add WCI2 trace buffer processing
# 05/21/13   btl    Remove Windows-specific code
# 01/14/12   btl    Add cxm counters parsing
# 01/14/12   btl    Update logging structure, add cxm state info parsing
# 11/09/12   btl    initial rev
#
#==============================================================================


# --------------------------------------------------------------------------- #
#                       IMPORT MODULES & DEFINITONS                           #
# --------------------------------------------------------------------------- #
import sys
import os
import re
import time
import fileinput
from optparse import OptionParser
from collections import namedtuple
from operator import attrgetter

# --------------------------------------------------------------------------- #
#                       GLOBAL VARIABLES
# --------------------------------------------------------------------------- #
num_events_log = 2048
umid_dict = {}
qmi_ids = None
techs = None
uart_states = None
ecodes = None
uart_modes = None
baud_rates = None

# --------------------------------------------------------------------------- #
#                       CLASSES
# --------------------------------------------------------------------------- #
# class to take care of organizing and printing CXM State info
class CxmStateInfo:
  State = namedtuple('State', ['valid', 'lte_sleep', 'policy', 'uart_npa'])
  PolicyInfo = namedtuple('PolicyInfo', ['valid', 'policy', 'power_threshold', 'rb_threshold',
                                         'lte_tx_continuous_subframe_denials_threshold',
                                         'lte_max_allowed_frame_denials',
                                         'lte_frame_denial_window', 'apt_table',
                                         'controller_tx_power_limit', 'wci2_power_limit',
                                         'link_path_loss_threshold', 'rb_filter_alpha',
                                         'filtered_rb_threshold', 'wci2_tx_pwrlmt_timeout',
                                         'controller_tx_pwrlmt_timeout'])
  LTEState = namedtuple('LTEState', ['valid', 'connected_mode', 'high_priority',
                                     'valid_mask', 'start_time', 'end_time', 'dl_earfcn',
                                     'ul_earfcn', 'dl_bw_mhz', 'ul_bw_mhz',
                                     'frame_structure', 'tdd_config', 'ssp', 'dl_cp',
                                     'ul_cp', 'dl_freq', 'ul_freq', 'frame_offset',
                                     'off_duration'])
  PwrLmt = namedtuple('PwrLmt', ['valid', 'controller', 'wci2'])
  MetricsState = namedtuple('MetricsState', ['valid', 'bler', 'sinr'])
  BandFilter = namedtuple('BandFilter', ['valid', 'number'])

  def __init__(self):
    # initialize data structures to defaults
    self.state = CxmStateInfo.State(False, '?', '?', '?')
    self.policy = CxmStateInfo.PolicyInfo(False, '?', '?', '?', '?', '?', '?', '?', '?', '?',
                             '?', '?', '?', '?', '?')
    self.lte = CxmStateInfo.LTEState(False, '?', '?', '?', '?', '?', '?', '?', '?', '?',
                        '?', '?', '?', '?', '?', '?', '?', '?', '?')
    self.power_limit = CxmStateInfo.PwrLmt(False, '?', '?')
    self.metrics = CxmStateInfo.MetricsState(False, '?', '?')
    self.band_filter = CxmStateInfo.BandFilter(False, '?')

  # set_... methods to fill in valid info as it becomes available
  def set_state(self, lte_sleep, policy, uart_npa):
    self.state = CxmStateInfo.State(True, lte_sleep, policy, uart_npa)

  def set_policy(self, policy, power_threshold, rb_threshold,
                 lte_tx_continuous_subframe_denials_threshold,
                 lte_max_allowed_frame_denials, lte_frame_denial_window, apt_table,
                 controller_tx_power_limit, wci2_power_limit,
                 link_path_loss_threshold, rb_filter_alpha,
                 filtered_rb_threshold, wci2_tx_pwrlmt_timeout,
                 controller_tx_pwrlmt_timeout):
    self.policy = CxmStateInfo.PolicyInfo(True, policy, power_threshold, rb_threshold,
                     lte_tx_continuous_subframe_denials_threshold,
                     lte_max_allowed_frame_denials, lte_frame_denial_window, apt_table,
                     controller_tx_power_limit, wci2_power_limit,
                     link_path_loss_threshold, rb_filter_alpha,
                     filtered_rb_threshold, wci2_tx_pwrlmt_timeout,
                     controller_tx_pwrlmt_timeout)

  def set_lte_state(self, connected_mode, high_priority,
                          valid_mask, start_time, end_time, dl_earfcn,
                          ul_earfcn, dl_bw_mhz, ul_bw_mhz,
                          frame_structure, tdd_config, ssp, dl_cp,
                          ul_cp, dl_freq, ul_freq, frame_offset,
                          off_duration):
    self.lte = CxmStateInfo.LTEState(True, connected_mode, high_priority,
                          valid_mask, start_time, end_time, dl_earfcn,
                          ul_earfcn, dl_bw_mhz, ul_bw_mhz,
                          frame_structure, tdd_config, ssp, dl_cp,
                          ul_cp, dl_freq, ul_freq, frame_offset,
                          off_duration)

  def set_power_limit(self, controller, wci2):
    self.power_limit = CxmStateInfo.PwrLmt(True, controller, wci2)

  def set_contr_power_limit(self, controller):
    self.power_limit = self.power_limit._replace(valid = True, controller=controller)

  def set_wci2_power_limit(self, wci2):
    self.power_limit = self.power_limit._replace(valid = True, wci2=wci2)

  def set_metrics(self, bler, sinr):
    self.metrics = CxmStateInfo.MetricsState(True, bler, sinr)

  def set_bler_metrics(self, bler):
    self.metrics = self.metrics._replace(valid=True, bler=bler)

  def set_sinr_metrics(self, sinr):
    self.metrics = self.metrics._replace(valid=True, sinr=sinr)

  def set_band_filter(self, number):
    self.band_filter = CxmStateInfo.BandFilter(True, number)

  # print the entire state to a string in human-readable format
  def to_string(self):
    cxm_state_str = """-----------------------------------------------------------------------------------------------------------
CXM_COEX_STATE_INFO
-----------------------------------------------------------------------------------------------------------
LTE Sleep State:   {0:<20}     Controller Power Limit: {3}
COEX Policy State: {1:<20}     WCI-2 Power Limit:      {4}
CXM UART State:    {2:<20}     BLER Metrics Started: {5}
                                            SINR Metrics Started: {6}

""".format(self.state.lte_sleep, self.state.policy, self.state.uart_npa,
               self.power_limit.controller, self.power_limit.wci2,
               self.metrics.bler, self.metrics.sinr)
    final_str = """CURRENT COEX POLICY INFO:
-----------------------------------------------------------------------------------------------------------\n"""

    # construct col0 strings with COEX Policy Info
    col0 = []
    if self.policy.valid:
      policy_list = decode_policy_mask(self.policy.policy)
      col0.append("Policy Mask = {0:#x}".format(self.policy.policy))
      for entry in policy_list:
        col0.append("  |-> {0}".format(entry))
      col0.append("Power Thresh = {0}".format(self.policy.power_threshold))
      col0.append("RB Thresh = {0}".format(self.policy.rb_threshold))
      col0.append("Cont Frame Denial Thresh = {0}".format(
        self.policy.lte_tx_continuous_subframe_denials_threshold))
      col0.append("Max allowd Frame Denials = {0}".format(
        self.policy.lte_max_allowed_frame_denials))
      col0.append("Frame Denial Window = {0}".format(
        self.policy.lte_frame_denial_window))
      col0.append("APT Table = {0}".format(self.policy.apt_table))
      col0.append("Controller Pwr Lmt = {0}".format(
        self.policy.controller_tx_power_limit))
      col0.append("WCI-2 Pwr Lmt = {0}".format(
        self.policy.wci2_power_limit))
      col0.append("Link Path Loss Thresh = {0}".format(
        self.policy.link_path_loss_threshold))
      col0.append("RB Filter Alpha = {0}".format(
        self.policy.rb_filter_alpha))
      col0.append("Filtered RB Thresh = {0}".format(
        self.policy.filtered_rb_threshold))
      col0.append("Contr Pwr Lmt Timeout = {0}".format(
        self.policy.controller_tx_pwrlmt_timeout))
      col0.append("WCI-2 Pwr Lmt Timeout = {0}".format(
        self.policy.wci2_tx_pwrlmt_timeout))

    # construct col1 strings with LTE State Info
    col1 = []
    if self.lte.valid:
      col1.append("Valid Mask = {0:#x}".format(self.lte.valid_mask))
      col1.append("Connected Mode = {0}".format(self.lte.connected_mode))
      col1.append("High Priority = {0}".format(self.lte.high_priority))
      col1.append("Start Time = {0}".format(self.lte.start_time))
      col1.append("End Time = {0}".format(self.lte.end_time))
      col1.append("DL EARFCN = {0}".format(self.lte.dl_earfcn))
      col1.append("UL EARFCN = {0}".format(self.lte.ul_earfcn))
      col1.append("DL BW MHz = {0}".format(self.lte.dl_bw_mhz))
      col1.append("UL BW MHz = {0}".format(self.lte.ul_bw_mhz))
      col1.append("Frame Structure = {0}".format(self.lte.frame_structure))
      col1.append("TDD Config = {0}".format(self.lte.tdd_config))
      col1.append("SSP = {0}".format(self.lte.ssp))
      col1.append("DL CP = {0}".format(self.lte.dl_cp))
      col1.append("UL CP = {0}".format(self.lte.ul_cp))
      col1.append("DL Freq = {0}".format(self.lte.dl_freq))
      col1.append("UL Freq = {0}".format(self.lte.ul_freq))
      col1.append("Frame Offset = {0}".format(self.lte.frame_offset))
      col1.append("Off Duration = {0}".format(self.lte.off_duration))

    # equalize the column lengths
    diff = len(col0) - len(col1)
    if diff < 0:
      for i in range(len(col0), len(col1)):
        col0.append("")
    elif diff > 0:
      for i in range(len(col1), len(col0)):
        col1.append("")

    # print the table to the string
    for i in range(0, len(col0)):
      final_str += "{0:<40}    {1:<}\n".format(col0[i], col1[i])

    # done building the string; return it
    return cxm_state_str + final_str + '\n'

class TraceBufferLine:
  """one line from a CXM or WCI2 trace buffer log"""
  def __init__(self):
    self.seq_id = 0
    self.ts = 0
    self.event = 0
    self.a8 = 0
    self.b16 = 0
    self.c32 = 0

  def __init__(self, seq_id, ts, event, a8, b16, c32):
    # logType is cxm or wci2
    self.seq_id = seq_id
    self.ts = ts
    self.event = event
    self.a8 = a8
    self.b16 = b16
    self.c32 = c32

  def toString(self):
    sec_time = self.ts*0.000030517578125
    raw_time = self.ts
    return 

class CxmTraceBufferLine(TraceBufferLine):
  def __init__(self, seq_id, ts, event, a8, b16, c32, service):
    TraceBufferLine.__init__(self, seq_id, ts, event, a8, b16, c32)
    self.service = service

  def toString(self):
    # convert timetick to seconds
    sec_time = self.ts*0.000030517578125
    raw_time = self.ts
    mask = ""
    ul = ""
    dl = ""
    # remove all empty rows from the final output
    # if both seq_id & timestamp is 0 then it is an empty row in the logs
    if self.seq_id == 0 and self.ts == 0:
      return
    # map service enum to readable string
    if self.service == 2:
      service = 'INTERNAL'
    elif self.service == 0:
      service = 'COEX'
    else:
      service = 'UNKNOWN'
    # CXM_TRC_INVALID
    if self.event == 'CXM_TRC_INVALID':
      # this means entry is invalid, so no need to interpret it
      return
    # CXM_TRC_WDOG_RPT
    elif self.event == 'CXM_TRC_WDOG_RPT':
      event = "Wdog Report:"
      m0 = "Wdog timer={0:#x},".format(self.a8)
      m1 = "report timeout={0:#x}".format(self.b16)
      m2 = ""
    # CXM_TRC_MSGR_RCV_MSG
    elif self.event == 'CXM_TRC_MSGR_RCV_MSG':
      event = "MSGR_PROC_RECV_MSG:"
      m0 = "UMID={0:#x}".format(self.c32)
      m1 = ""
      m2 = ""
      # look up UMID
      umid_name = umid_dict.get(self.a8, None)
      if umid_name:
        m0 = m0 + '=' + umid_name
    # CXM_TRC_MSGR_SND_MSG
    elif self.event == 'CXM_TRC_MSGR_SND_MSG':
      event = "MSGR_SEND_MESSAGE:"
      # look up UMID
      m0 = "UMID={0:#x},".format(self.c32)
      m1 = ""
      m2 = ""
      umid_name = umid_dict.get(self.a8, None)
      if umid_name:
        m0 = m0 + '=' + umid_name
    # CXM_TRC_MSGR_LTE_SFN
    elif self.event == 'CXM_TRC_MSGR_LTE_SFN':
      event = "(MSGR_HNDLR) LTE Subframe #:"
      m0 = "LTE_ADV_TX_SFN={0}".format(self.a8)
      m1 = ""
      m2 = ""
    # CXM_TRC_MSGR_LTE_BLER
    elif self.event == 'CXM_TRC_MSGR_LTE_BLER':
      event = "(MSGR_HNDLR) LTE BLER stats:"
      m0 = "TB Count={0},".format(self.a8)
      m1 = "TB Error Cnt={0}".format(self.b16)
      m2 = ""
    # CXM_TRC_MSGR_LTE_SINR
    elif self.event == 'CXM_TRC_MSGR_LTE_SINR':
      event = "(MSGR_HNDLR) LTE SINR stats:"
      m0 = "status={0},".format(self.a8)
      m1 = "sinr_valid={0},".format(self.b16)
      m2 = "sinr={0}dB".format(self.c32)
    # CXM_TRC_MSGR_LTE_TX_PWR_LMT
    elif self.event == 'CXM_TRC_MSGR_LTE_TX_PWR_LMT':
      event = "(MSGR_HNDLR) LTE TX Pwr Lmt:"
      m0 = "path loss={0},".format(self.a8)
      m1 = "filtered_rb={0:f},".format(self.b16/256.0)
      m2 = "RRC state={0}".format(self.c32)
    # CXM_TRC_MSGR_LTE_RRC_STATE
    elif self.event == 'CXM_TRC_MSGR_LTE_RRC_STATE':
      # determine controller and wci2 power limit states
      if self.b16 == 0:
        ctrl = "Off"
      elif self.b16 == 1:
        ctrl = "Pending"
      else:
        ctrl = "Active"
      if self.c32 == 0:
        wci = "Off"
      elif self.c32 == 1:
        wci = "Pending"
      else:
        wci = "Active"
      event = "(MSGR_HNDLR) LTE RRC STATE:"
      m0 = "RRC state={0},".format(self.a8)
      m1 = "CTRL_PWR_LMT_State={0},".format(ctrl)
      m2 = "WCI2_state={0}".format(wci)
    # CXM_TRC_MSGR_WCN_RPT
    elif self.event == 'CXM_TRC_MSGR_WCN_RPT':
      event = "(MSGR_HNDLR) FW WCN thres exceeded:"
      m0 = "error type={0},".format(self.a8)
      m1 = "# of requests={0}".format(self.b16)
      m2 = ""
    # CXM_TRC_QMI_SVC_OPEN
    elif self.event == 'CXM_TRC_QMI_SVC_OPEN':
      event = "QMI Service Open:"
      m0 = "name (ptr)={0:#x},".format(self.a8)
      m1 = "svc_id={0},".format(self.b16)
      m2 = "svc_qmi_handle={0}".format(self.c32)
    # CXM_TRC_QMI_SVC_CLOSE
    elif self.event == 'CXM_TRC_QMI_SVC_CLOSE':
      event = "QMI Service Close:"
      m0 = "svc_id={0},".format(self.a8)
      m1 = "retval={0}".format(self.b16)
      m2 = ""
    # CXM_TRC_QMI_CLNT_CONNECT_CB
    elif self.event == 'CXM_TRC_QMI_CLNT_CONNECT_CB':
      event = "QMI Client connect CB:"
      m0 = "svc_id={0},".format(self.a8)
      m1 = "client_index={0},".format(self.b16)
      m2 = "retval={0}".format(self.c32)
    # CXM_TRC_QMI_CLNT_DISCONNECT_CB
    elif self.event == 'CXM_TRC_QMI_CLNT_DISCONNECT_CB':
      event = "QMI Client discnnct CB:"
      m0 = "svc_id={0},".format(self.a8)
      m1 = "client_ptr={0:#x}".format(self.b16)
      m2 = ""
    # CXM_TRC_QMI_SVC_HNDL_REQ_CB
    elif self.event == 'CXM_TRC_QMI_SVC_HNDL_REQ_CB':
      event = "QMI Svc Handle REQ CB:"
      # look up QMI message corresponding to the msg id
      qmi_msg = qmi_ids.get(self.b16, "UNKNOWN") + '_REQ'
      m0 = "svc_id={0},".format(self.a8)
      m1 = "msg_id={0}={1}".format(self.b16, qmi_msg)
      m2 = "retval={0}".format(self.c32)
    # CXM_TRC_QMI_SND_IND
    elif self.event == 'CXM_TRC_QMI_SND_IND':
      event = "QMI Send Indication:"
      # look up QMI message corresponding to the msg id
      qmi_msg = qmi_ids.get(self.b16, "UNKNOWN")
      m0 = "svc_id={0},".format(self.a8)
      m1 = "msg_id={0}={1},".format(self.b16, qmi_msg)
      m2 = "# of receivers={0}".format(self.c32)
    # CXM_TRC_QMI_SND_RESP
    elif self.event == 'CXM_TRC_QMI_SND_RESP':
      event = "QMI Send Response:"
      # look up QMI message corresponding to the msg id
      qmi_msg = qmi_ids.get(self.a8, "UNKNOWN") + '_RESP'
      m0 = "msg_id={0}={1},".format(self.a8, qmi_msg)
      m1 = "client_ptr={0:#x},".format(self.b16)
      m2 = "retval={0}".format(self.c32)
    # CXM_TRC_QMI_PUSH_DEF_REQ
    elif self.event == 'CXM_TRC_QMI_PUSH_DEF_REQ':
      event = "QMI Push Deferred Resp:"
      m0 = "svc_id={0},".format(self.a8)
      m1 = "def_QID={0},".format(self.b16)
      m2 = "retval={0}".format(self.c32)
    # CXM_TRC_QMI_POP_DEF_REQ
    elif self.event == 'CXM_TRC_QMI_POP_DEF_REQ':
      event = "QMI Pop Deferred Resp:"
      m0 = "svc_id={0},".format(self.a8)
      m1 = "def_QID={0},".format(self.b16)
      m2 = "retval={0}".format(self.c32)
    # CXM_TRC_QMI_HNDL_REG_IND_REQ
    elif self.event == 'CXM_TRC_QMI_HNDL_REG_IND_REQ':
      event = "(QMI HNDLR) IND Register REQ:"
      m0 = "clnt_ptr={0:#x},".format(self.a8)
      m1 = "clnt_ind_map={0},".format(self.b16)
      m2 = "retval={0}".format(self.c32)
    # CXM_TRC_QMI_SET_POLICY_REQ
    elif self.event == 'CXM_TRC_QMI_SET_POLICY_REQ':
      event = "(QMI HNDLR) Set Policy REQ:"
      m0 = "policy={0:#x}".format(self.a8)
      m1 = ""
      m2 = ""
    # CXM_TRC_QMI_STRT_BLER_REQ
    elif self.event == 'CXM_TRC_QMI_STRT_BLER_REQ':
      event = "(QMI HNDLR) Start BLER REQ:"
      m0 = "TB_Count={0},".format(self.a8)
      m1 = "Threshld_err_tb_cnt={0}".format(self.b16)
      m2 = ""
    # CXM_TRC_QMI_STRT_SINR_REQ
    elif self.event == 'CXM_TRC_QMI_STRT_SINR_REQ':
      event = "(QMI HNDLR) Start SINR REQ:"
      m0 = "Alpha={0}".format(self.a8)
      m1 = ""
      m2 = ""
    # CXM_TRC_QMI_LTE_TX_PWR_LMT_SUCC
    elif self.event == 'CXM_TRC_QMI_LTE_TX_PWR_LMT_SUCC':
      event = "(QMI_SND) LTE PWRLMT Cond Succ:"
      m0 = "this_mask={0:#x},".format(self.a8)
      m1 = "sent_mask={0:#x},".format(self.b16)
      m2 = "drop_count={0}".format(self.c32)
    # CXM_TRC_QMI_LTE_TX_PWR_LMT_FAIL
    elif self.event == 'CXM_TRC_QMI_LTE_TX_PWR_LMT_FAIL':
      event = "(QMI_SND) LTE PWRLMT Cond Fail:"
      m0 = "cont mask={0:#x},".format(self.a8)
      m1 = "wci2 mask={0:#x},".format(self.b16)
      m2 = "drop_count={0}".format(self.c32)
    # CXM_TRC_QMI_TX_SUBFRAME_DENIAL
    elif self.event == 'CXM_TRC_QMI_TX_SUBFRAME_DENIAL':
      event = "(QMI_SND) WCN frame Denial Fail:"
      m0 = "this_mask={0:#x},".format(self.a8)
      m1 = "sent_mask={0:#x},".format(self.b16)
      m2 = "drop_count={0}".format(self.c32)
    # CXM_TRC_UART_RCV_MSG
    elif self.event == 'CXM_TRC_UART_RCV_MSG':
      event = "UART RECV MSG:"
      m0 = "byte={0:#x},".format(self.a8)
      m1 = "FIFO_index={0},".format(self.b16)
      m2 = "RX_FIFO_PTR={0:#x}".format(self.c32)
    # CXM_TRC_UART_SND_MSG
    elif self.event == 'CXM_TRC_UART_SND_MSG':
      event = "UART SEND MSG:"
      # interpret uart data differently depending on protocol
      if self.a8 == 0:
        m0 = "format=RAW_BYTE,"
        m1 = "data={0:#x},".format(self.b16)
        m2 = ""
      elif self.a8 == 1:
        m0 = "format=WCI-2,"
        m1 = "type={0:#x},".format(self.b16)
        m2 = "data={0:#x}".format(self.c32)
      else:
        m0 = "format=ARRAY,"
        m1 = "array_size={0},".format(self.b16)
        m2 = ""
    # CXM_TRC_QMI_UPDATE_WLAN_CONN
    elif self.event == 'CXM_TRC_QMI_UPDATE_WLAN_CONN':
      event = "(QMI HNDLR) Update WLAN Conn:"
      if self.b16 == 0:
        state = 'CONN_DISABLED'
      elif self.b16 == 1:
        state = 'CONN_SETUP'
      else:
        state = 'CONN_STEADY'
      if self.c32 == 0:
        mode = 'NONE'
      elif self.c32 == 1:
        mode = 'STATION'
      elif self.c32 == 2:
        mode = 'SOFTAccessPoint'
      elif self.c32 == 3:
        mode = 'P2P_GROUP_OWNER'
      elif self.c32 == 4:
        mode = 'P2P_CLIENT'
      else:
        mode = 'Amplify_Conn'
      m0 = "Conn_hndl={0:#x}, ".format(self.a8)
      m1 = "state={0}, ".format(state)
      m2 = "conn_mode={0}".format(mode)
    # CXM_TRC_QMI_UPDATE_WLAN_SCAN
    elif self.event == 'CXM_TRC_QMI_UPDATE_WLAN_SCAN':
      event = "(QMI HNDLER) Update WLAN Scan Info:"
      if self.b16 == 0:
        state = 'SCAN_STOP'
      else:
        state = 'SCAN_START'
      m0 = "Scan_id={0:#x}, ".format(self.a8)
      m1 = "state={0}, ".format(state)
      m2 = "band_freq={0}".format(self.c32)
    # CXM_TRC_COEX_CONFLICT_START
    elif self.event == 'CXM_TRC_COEX_CONFLICT_START':
      event = "COEX Conflict Start:"
      m0 = "tech1_freq={0:#x}, ".format(self.a8)
      m1 = "tech2_freq={0:#x}, ".format(self.b16)
      m2 = "rb_tbl_index={0}".format(self.c32)
    # CXM_TRC_COEX_CONFLICT_END
    elif self.event == 'CXM_TRC_COEX_CONFLICT_END':
      event = "COEX Conflict End:"
      m0 = ""
      m1 = ""
      m2 = ""
    # CXM_TRC_SET_LTE_TX_PWR_LMT
    elif self.event == 'CXM_TRC_SET_LTE_TX_PWR_LMT':
      event = "Set LTE TX Pwr Lmt:"
      m0 = "limit={0}, ".format(self.a8)
      m1 = "old_limit={0}, ".format(self.b16)
      m2 = "RF_retval={0}, ".format(self.c32)
    # CXM_TRC_QMI_SET_SLP_NOTFY_THRESH
    elif self.event == 'CXM_TRC_QMI_SET_SLP_NOTFY_THRESH':
      tech_str = techs.get(self.a8, "UNKNOWN")
      event = "(QMI HNDLR) Set Slp Notfy Thresh:"
      m0 = "tech={0}, ".format(tech_str)
      m1 = "thresh={0}".format(self.b16)
      m2 = ""
    # CXM_TRC_QMI_SND_WWAN_STATE_IND
    elif self.event == 'CXM_TRC_QMI_SND_WWAN_STATE_IND':
      event = "(QMI_SND) WWAN State Ind:"
      m0 = "retval={0}".format(self.a8)
      m1 = ""
      m2 = ""
    # CXM_TRC_MSGR_WWAN_TECH_STATE_INFO
    elif self.event == 'CXM_TRC_MSGR_WWAN_TECH_STATE_INFO':
      event = "(MSGR) WWAN Tech State Info:"
      tech_str = techs.get(self.a8, "UNKNOWN")
      m0 = "tech={0}, ".format(tech_str)
      m1 = "state_changed={0}, ".format(self.b16)
      m2 = "num_info_sets={0}".format(self.c32)
    # CXM_TRC_MSGR_WWAN_TECH_SLEEP_WAKEUP
    elif self.event == 'CXM_TRC_MSGR_WWAN_TECH_SLEEP_WAKEUP':
      event = "(MSGR) WWAN Tech Sleep/Wakeup:"
      tech_str = techs.get(self.a8, "UNKNOWN")
      m0 = "tech={0}, ".format(tech_str)
      m1 = "going_to_slp={0}, ".format(self.b16)
      m2 = "duration={0}".format(self.c32)
    elif self.event == 'CXM_TRC_QMI_SVC_INIT':
      event = "Init QMI Service:"
      m0 = "service id={0}, ".format(self.a8)
      m1 = "IDL major ver={0}, ".format(self.b16)
      m2 = "IDL minor ver={0}".format(self.c32)
    elif self.event == 'CXM_TRC_TSK_INIT':
      event = "CXM Task Init :"
      m0 = ""
      m1 = ""
      m2 = ""
    elif self.event == 'CXM_ANT_RELEASE_AND_REQUEST':
      event = "Antenna release:"
      m0 = "client_id={0}, ".format(self.a8)
      m1 = "req_antenna={0}, ".format(self.b16)
      m2 = "duration={0}, ".format(self.c32)
    elif self.event == 'CXM_ANT_GRANT_CB':
      event = "Antenna Grant CB:"
      m0 = ""
      m1 = "grant_dur={0}, ".format(self.b16)
      m2 = "duration={0}, ".format(self.c32)
    elif self.event == 'CXM_ANT_UNLOCK_CB':
      event = "Antenna Unlock CB:"
      m0 = "event={0}, ".format(self.a8)
      m1 = "sclk_delta={0}, ".format(self.b16)
      m2 = "unlock_by_sclk={0}, ".format(self.c32)
    elif self.event == 'CXM_TRC_WCI2_RX_CB':
      event = "WCI2 RX CB:"
      m0 = "status_mask={0:#x}, ".format(self.a8)
      m1 = ""
      m2 = ""
    elif self.event == 'CXM_TRC_WCI2_PROC_RX':
      event = "WCI2 RX Msg:"
      m0 = "Byte={0:#x}, ".format(self.a8)
      m1 = "Type={0}, ".format(self.b16)
      m2 = ""
    # default (no case written)
    else:
      event = "{0}:".format(self.event)
      m0 = "a={0},".format(self.a8)
      m1 = "b={0},".format(self.b16)
      m2 = "c={0}".format(self.c32)
    # print line to file
    return '{0:14.5f}  {1:#x}  {2:>30} {3:45}  {4:>4} {5:>3} {6:>3}\n'.format(sec_time, raw_time, event, m0 + m1 + m2, mask, ul, dl)

class Wci2TraceBufferLine(TraceBufferLine):
  def toString(self):
    # remove all empty rows from the final output
    # if both seq_id & timestamp is 0 then it is an empty row in the logs
    if self.seq_id == 0 and self.ts == 0:
      return
    # convert timetick to seconds
    sec_time = self.ts*0.000030517578125
    raw_time = self.ts
    # WCI2_TRC_INVALID
    if self.event == 'WCI2_TRC_INVALID':
      # this means entry is invalid, so no need to interpret it
      return
    # WCI2_TRC_TSK_INIT
    elif self.event == 'WCI2_TRC_TSK_INIT':
      event = "WCI2 init:"
      m0 = ""
      m1 = ""
      m2 = ""
    # WCI2_TRC_TSK_DEINIT
    elif self.event == 'WCI2_TRC_TSK_DEINIT':
      event = "WCI2 deinit:"
      m0 = ""
      m1 = ""
      m2 = ""
    # WCI2_TRC_UART_RCV_MSG
    elif self.event == 'WCI2_TRC_UART_RCV_MSG':
      event = "UART RX:"
      m0 = "byte={0:#x}, ".format(self.a8)
      m1 = "fifo_index={0}, ".format(self.b16)
      m2 = "rx_fifo_ptr={0:#x}".format(self.c32)
    # WCI2_TRC_UART_SND_MSG
    elif self.event == 'WCI2_TRC_UART_SND_MSG':
      event = "UART TX:"
      m0 = "size={0}".format(self.a8)
      m1 = ""
      m2 = ""
    # WCI2_TRC_DIR_READ_CB
    elif self.event == 'WCI2_TRC_DIR_READ_CB':
      event = "SIO Direct Read CB:"
      m0 = "status={0} ".format(self.a8)
      m1 = "en_mask={0:#x} ".format(self.b16)
      m2 = "intr_status_mask={0:#x} ".format(self.c32)
    # WCI2_TRC_SIO_RX_CB
    elif self.event == 'WCI2_TRC_SIO_RX_CB':
      event = "SIO DSM RX CB:"
      m0 = "CXM_mode={0}, ".format(self.a8)
      m1 = "Adv_hw_en={0}, ".format(self.b16)
      m2 = ""
    # WCI2_TRC_RX_WCI2
    elif self.event == 'WCI2_TRC_RX_WCI2':
      event = "WCI2 RX:"
      m0 = "byte={0:#x}, ".format(self.a8)
      m1 = "type={0}, ".format(self.b16)
      m2 = "cxm_mode={0}".format(self.c32)
    # WCI2_TRC_TX_WCI2
    elif self.event == 'WCI2_TRC_TX_WCI2':
      event = "WCI2 TX:"
      m0 = "byte={0:#x}, ".format(self.a8)
      m1 = "type={0}".format(self.b16)
      m2 = ""
    # WCI2_TRC_T2_MISALIGNED
    elif self.event == 'WCI2_TRC_T2_MISALIGNED':
      event = "RX Misaligned t2 byte:"
      m0 = "count={0}, ".format(self.a8)
      m1 = "byte={0:#x}".format(self.b16)
      m2 = ""
    # WCI2_TRC_SET_CXM_MODE
    elif self.event == 'WCI2_TRC_SET_CXM_MODE':
      event = "Set CXM_UART Mode:"
      m0 = "new_mode={0}, ".format(self.a8)
      m1 = "old_mode={0}".format(self.b16)
      m2 = ""
    # WCI2_TRC_SET_BAUD
    elif self.event == 'WCI2_TRC_SET_BAUD':
      event = "Set baud:"
      baud = baud_rates.get(self.a8, 'UNKNOWN')
      m0 = "new_baud={0}, ".format(baud)
      baud = baud_rates.get(self.b16, 'UNKNOWN')
      m1 = "old_baud={0}".format(baud)
      m2 = ""
    # WCI2_TRC_UART_POWER_STATE
    elif self.event == 'WCI2_TRC_UART_POWER_STATE':
      event = "UART Power state change:"
      state = uart_states.get(self.a8, "UNKNOWN")
      m0 = "new_state={0}, ".format(state)
      state = uart_states.get(self.b16, "UNKNOWN")
      m1 = "old_state={0}".format(state)
      m2 = ""
    # WCI2_TRC_CLIENT_REG
    elif self.event == 'WCI2_TRC_CLIENT_REG':
      event = "WCI2 client register:"
      retval = ecodes.get(self.a8, 'UNKNOWN')
      m0 = "retval={0}, ".format(retval)
      mode = uart_modes.get(self.b16, 'UNKOWN')
      m1 = "mode={0}, ".format(mode)
      m2 = "handle={0}".format(self.c32)
    # WCI2_TRC_CLIENT_DEREG
    elif self.event == 'WCI2_TRC_CLIENT_DEREG':
      event = "WCI2 client deregister:"
      retval = ecodes.get(self.a8, 'UNKNOWN')
      m0 = "retval={0}, ".format(retval)
      m1 = "handle={0}".format(self.b16)
      m2 = ""
    # WCI2_TRC_CLNT_NTFY_T2_RX
    elif self.event == 'WCI2_TRC_CLNT_NTFY_T2_RX':
      event = "Notify WCI2 client t2 RX:"
      m0 = "ref_count={0}, ".format(self.a8)
      m1 = "num_notified={0}, ".format(self.b16)
      m2 = "msg_len={0}".format(self.c32)
    # WCI2_TRC_CLNT_NTFY_DIR_RX
    elif self.event == 'WCI2_TRC_CLNT_NTFY_DIR_RX':
      event = "Notify WCI2 client direct RX:"
      m0 = "num_notified={0}, ".format(self.a8)
      m1 = "status={0}".format(self.b16)
      m2 = ""
    # WCI2_TRC_CLNT_NTFY_BASIC_RX
    elif self.event == 'WCI2_TRC_CLNT_NTFY_BASIC_RX':
      event = "Notify basic client RX:"
      m0 = "num_notified={0}, ".format(self.a8)
      m1 = "size={0}".format(self.b16)
      m2 = ""
    else:
      event = "{0}:".format(self.event)
      m0 = "a={0},".format(self.a8)
      m1 = "b={0},".format(self.b16)
      m2 = "c={0}".format(self.c32)
    # print line to file
    return '{0:14.5f}  {1:#x}  {2:>30} {3:45}\n'.format(sec_time, raw_time, event, m0 + m1 + m2)

class TraceBuffer:
  """collection of TraceBufferLines for easy printing"""
  def __init__(self):
    self.buf = []

  def add(self, traceLine):
    self.buf.append(traceLine)

  def merge(self, trace):
    # merge an existing trace with the current one, interleaving the rows
    for row in trace.buf:
      self.add(row)
    self.sort()

  def sort(self):
    self.buf.sort(key=attrgetter('ts', 'seq_id')) # sort by timestamp then seq_id

  def writeToFile(self, out_file):
    for row in self.buf:
      rowStr = row.toString()
      if rowStr is not None:
        out_file.write(rowStr)


# --------------------------------------------------------------------------- #
#                       MAIN
# --------------------------------------------------------------------------- #

def main():
  parser = OptionParser()
  parser.add_option('-f', '--file', type='string', dest='file',
                    help='Specify file containg cxm log to parse')
  parser.add_option('-d', '--dir', dest='dir', help='Specify output dump directory')
  parser.add_option('-v', '--version', dest='version', type='int', default=1,
                    help='Specify CXM_TRACE buffer version number to use')
  parser.add_option('-u', '--umid_path', type='string', dest='umid_path',
                    help='Specify path and file to __msgr_global_umids.h\n likely ..\\..\\..\\msgr\\modem_proc\\qdsp6\\[BUILD_ID]\\__msgr_global_umids.h')
  parser.add_option('-w', '--wci2', type='string', dest='wci2_parsing', default='no',
                    help="Process WCI2 buffer too? 'no' (default), 'yes', or 'interleave'.")
  (options, args) = parser.parse_args()

  # Check if new dump directory specifed. If not, use working directory
  if options.dir:
    working_dir = options.dir
    print "Dump directory specified: " + working_dir
    # check if directory valid
    if os.path.isdir(working_dir) == False:
      print "ERROR: specified directory not valid"
      sys.exit(4)
  else:
    working_dir = cur_dir

  # If given a file to parse, parse the logs and print output
  if options.file:
    global umid_dict
    global qmi_ids
    qmi_ids = create_qmi_msg_id_dict()
    global techs
    techs = create_tech_id_dict()
    global uart_states
    uart_states = create_uart_state_dict()
    global ecodes
    ecodes = create_uart_retval_dict()
    global uart_modes
    uart_modes = create_uart_mode_dict()
    global baud_rates
    baud_rates = create_uart_baud_dict()

    # read in umids
    if options.umid_path:
      umid_dict = create_umid_dictionary(options.umid_path)
    try:
      # do the work - generate output
      in_file = open(options.file, 'r')
      cxm_trace_table, state_str, counters_str, wwan_state_str, fw_counters_str, wci2_trace_table = read_and_parse_in_file(in_file, options.version)
      in_file.close()
      generate_out_file(cxm_trace_table, state_str, counters_str, wwan_state_str, fw_counters_str, wci2_trace_table, working_dir, options.wci2_parsing)
      sys.exit(0)
    except IOError:
      print "ERROR: Could not open input file " + options.file
      sys.exit(2)
  else:
    print "Please specify a file to parse. Use '-h' for help"
    sys.exit(0)

# --------------------------------------------------------------------------- #
#                            HELPER FUNCTIONS                                 #
# --------------------------------------------------------------------------- #

def read_and_parse_in_file(in_file, trace_version):
  cxm_log_table = None
  wci2_log_table = None
  state_str = ""
  counters_str = ""
  wwan_state_str = ""
  fw_counters_str = ""
  line = in_file.readline()
  while line != '':
    if 'coex_state_info' in line:
      state_str, line = read_and_parse_coex_state_info(in_file, line)
    elif 'coex_wwan_state_info' in line:
      wwan_state_str, line = read_and_parse_coex_wwan_state_info(in_file, line)
    elif 'cxm_counters' in line:
      counters_str, line = read_and_parse_cxm_counters(in_file, line)
    elif 'cxm_fw_counters' in line:
      fw_counters_str, line = read_and_parse_fw_counters(in_file, line)
    elif 'cxm_trace_buffer' in line:
      cxm_log_table, line = read_and_parse_cxm_trace_buffer(in_file, trace_version, line)
    elif 'wci2_trace_buffer' in line:
      wci2_log_table, line = read_and_parse_wci2_trace_buffer(in_file, line)
    else:
      # continue while loop
      line = in_file.readline()
  return cxm_log_table, state_str, counters_str, wwan_state_str, fw_counters_str, wci2_log_table

def read_and_parse_cxm_counters(in_file, line):
  # read in the lines corresponding to the cxm counters and convert
  # them into a string to be printed
  counter_dict = create_counter_dict()
  r_count = re.compile('\[\d+\] = \(count = \d+, last_value = \d+, last_tstamp = \d+\).*')
  r_num = re.compile('\d+')
  string = "\
-----------------------------------------------------------------------------------------------------------\n\
CXM_COUNTERS\n\
-----------------------------------------------------------------------------------------------------------\n\
{0:^30} {1:^14} {2:^30} {3:^14} \n\
-----------------------------------------------------------------------------------------------------------\n\
".format('Name', 'Count', 'Last Value', 'Last Timestamp')
  # skip lines until actual counter structure starts
  while line != '' and None == r_count.match(line.strip()):
    line = in_file.readline()
  # find and process all instances of counters in a row
  while line != '' and r_count.match(line.strip()):
    # extract all the decimal numbers and save into a tuple for use later
    values = r_num.findall(line)
    # counter enum
    name = counter_dict.get(int(values[0]), "UNKNOWN")
    # counter value
    count = int(values[1])
    # get last value at the last counter event
    last_value = "{0:#x} ({0})".format(int(values[2]))
    # get timestamp of last counter event
    last_tstamp = int(values[3])
    # save add values to string to print later
    string += "{0:30} {1:14} {2:^30} {3:#x}\n".format(name, count, last_value, last_tstamp)
    # prep to continue while loop
    line = in_file.readline()
  # done interpreting counters! return the produced string and the place in file
  string += '\n'
  return string, line

def read_and_parse_coex_state_info(in_file, line):
  # instantiate object to handle keeping track of data simply
  state = CxmStateInfo()

  while line != '':
    # read in all variables inside coex_state_info, and construct and return
    # an object representing the state
    if '  state = ' in line:
      lte_state    = in_file.readline().split('=')[1].strip()
      policy_state = in_file.readline().split('=')[1].strip()
      npa_state    = in_file.readline().split('=')[1].strip()
      state.set_state(lte_state, policy_state, npa_state)

    # read in policy info from file, and convert from hex to int where necessary
    # every other line of this struture is not relevant info, so ignore it
    elif 'policy_info = ' in line:
      in_file.readline()
      policy = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      in_file.readline()
      power_threshold = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      in_file.readline()
      rb_threshold = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      in_file.readline()
      lte_tx_continuous_subframe_denials_threshold = \
        int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      in_file.readline()
      line = in_file.readline()
      lte_max_allowed_frame_denials = int(
        line.split(',')[0].split('=')[2].strip(), 16)
      lte_frame_denial_window = int(
        line.split(',')[1].split('=')[1].strip(' ),\n'), 16)
      in_file.readline()
      apt_table = in_file.readline().split('=')[1].strip()
      in_file.readline()
      controller_tx_power_limit = in_file.readline().split('=')[1].strip(' ),\n')
      in_file.readline()
      wci2_power_limit = in_file.readline().split('=')[1].strip(' ),\n')
      in_file.readline()
      link_path_loss_threshold = in_file.readline().split('=')[1].strip(' ),\n')
      in_file.readline()
      rb_filter_alpha = in_file.readline().split('=')[1].strip(' ),\n')
      in_file.readline()
      filtered_rb_threshold = in_file.readline().split('=')[1].strip(' ),\n')
      in_file.readline()
      wci2_tx_pwrlmt_timeout = int(
        in_file.readline().split('=')[1].strip(' ),\n'), 16)
      in_file.readline()
      controller_tx_pwrlmt_timeout = int(
        in_file.readline().split('=')[1].strip(' ),\n'), 16)
      # fill the info into the object
      state.set_policy(policy, power_threshold, rb_threshold,
                      lte_tx_continuous_subframe_denials_threshold,
                      lte_max_allowed_frame_denials, lte_frame_denial_window, apt_table,
                      controller_tx_power_limit, wci2_power_limit,
                      link_path_loss_threshold, rb_filter_alpha,
                      filtered_rb_threshold, wci2_tx_pwrlmt_timeout,
                      controller_tx_pwrlmt_timeout)

    # read in LTE State Info
    elif 'lte_state_info = ' in line:
      line = in_file.readline()
      if (line.split('=')[1].strip(' ),\n'), 16) == 1:
        connected_mode = True
      else:
        connected_mode = False
      line = in_file.readline()
      if (line.split('=')[1].strip(' ),\n'), 16) == 1:
        high_priority = True
      else:
        high_priority = False
      valid_mask = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      start_time = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      end_time = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      dl_earfcn = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      ul_earfcn = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      in_file.readline()
      in_file.readline()
      dl_bw_mhz = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      ul_bw_mhz = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      frame_structure = in_file.readline().split('=')[1].strip()
      tdd_config = in_file.readline().split('=')[1].strip()
      ssp = in_file.readline().split('=')[1].strip()
      dl_cp = in_file.readline().split('=')[1].strip()
      ul_cp = in_file.readline().split('=')[1].strip()
      dl_freq = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      ul_freq = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      frame_offset = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      off_duration = int(in_file.readline().split('=')[1].strip(' ),\n'), 16)
      # fill the info into the object
      state.set_lte_state(connected_mode, high_priority, valid_mask, start_time,
                          end_time, dl_earfcn, ul_earfcn, dl_bw_mhz, ul_bw_mhz,
                          frame_structure, tdd_config, ssp, dl_cp, ul_cp, dl_freq,
                          ul_freq, frame_offset, off_duration)

    # parse controller and wci2 power limit states
    elif 'cont_pwr_lmt_state' in line:
      controller_pl = line.split('=')[1].split('>')[1].strip()
      state.set_contr_power_limit(controller_pl)
    elif 'wci2_pwr_lmt_state' in line:
      wci2_pl = line.split('=')[1].split('>')[1].strip()
      state.set_wci2_power_limit(wci2_pl)

    # parse LTE BLER and SINR Metrics state
    elif 'metrics_bler_started' in line:
      if int(in_file.readline().split('=')[1].strip(' ),\n'), 16) == 1:
        bler_started = True
      else:
        bler_started = False
      state.set_bler_metrics(bler_started)
    elif 'metrics_sinr_started' in line:
      if int(in_file.readline().split('=')[1].strip(' ),\n'), 16) == 1:
        sinr_started = True
      else:
        sinr_started = False
      state.set_sinr_metrics(sinr_started)

    # another variable hit, must mean we're done. break, process, and return
    elif 'coex_wwan_state_info' in line:
      break
    elif 'cxm_trace_buffer' in line:
      break
    elif 'cxm_counters' in line:
      break
    elif 'cxm_fw_counters' in line:
      break

    # continue the while loop until out of lines or hit next variable
    line = in_file.readline()

  return state.to_string(), line

def read_and_parse_coex_wwan_state_info(in_file, line):
  #wwan state string
  wwan_state_str = """WWAN STATE INFO:
-----------------------------------------------------------------------------------------------------------\n"""
  for tech_id in range(0,len(techs)-1):
    #go to starting position (where num sets is)
    while 'num_link_info_sets' not in line:
      line = in_file.readline()
    num_lnks = int( line.split('=')[1].strip(' ,\n'), 16)
    #if there is no band data, do not parse this tech
    if num_lnks == 0:
      line = in_file.readline()
      continue
    #go to starting point for band data
    band_data = []
    while 'frequency' not in line:
      line = in_file.readline()
    for i in range(0,num_lnks):
      match = re.search(r'frequency = (\w+), bandwidth = (\w+), band = \w+, channel = \w+, rf_device = \w+ = \w+, direction = (\w+) = \w+', line)
      band_data.append('frequency = {0} KHz'.format( int(match.group(1), 16) ) )
      band_data[i] += ', bandwidth = {0} Hz'.format( int(match.group(2), 16) )
      band_data[i] += ', direction = {0}'.format( match.group(3))
      line = in_file.readline()

    #read tech specific params
    tech_params = ''
    while 'params = (' not in line:
      line = in_file.readline()
    #step to LTE params
    line = in_file.readline()
    if tech_id == 0:
      match = re.search(r'frame_structure = (\w+) = \w+, tdd_config = (\w+) = \w+, ssp = (\w+) = \w+, dl_cp = (\w+) = \w+, ul_cp = (\w+) = \w+, frame_offset = (\w+)', line)
      tech_params += 'frame_structure = {0}\ntdd_config = {1}\nssp = {2}\ndl_cp = {3}\
                      \nul_cp = {4}\nframe offset = {5} \n'\
                      .format(match.group(1), match.group(2), match.group(3),
                      match.group(4), match.group(5), int(match.group(6), 16) )

    #read sleep threshold
    while 'sleep_thresh' not in line:
      line = in_file.readline()
    sleep_thresh = 'Sleep Notification Threshold = ' + str ( int( line.split('=')[1].strip(' ),\n'), 16 ) )

    #print state info for this tech
    wwan_state_str += techs[tech_id] + '\n'
    wwan_state_str += 'Number of band sets = ' + str(num_lnks) + '\n'
    wwan_state_str += "\n".join(band_data) + '\n'
    wwan_state_str += tech_params
    wwan_state_str += sleep_thresh + '\n\n'

  return wwan_state_str, line

def read_and_parse_fw_counters(in_file, line):
  events = get_fw_counter_events()
  fw_count_str = """CXM FIRMWARE COUNTERS:
-----------------------------------------------------------------------------------------------------------\n"""
  for tech_id in range(0,len(techs)-1):
    #go to your next counter event
    while 'counter_event' not in line:
      if "NULL" in line:
        line = in_file.readline()
        return fw_count_str, line
      line = in_file.readline()
    cnt_match = re.findall(r'count = (\w+), last_tstamp = (\w+)', line)
    #tot_count used to determine whether to print this tech or not
    tot_count = 0
    for cnt_val in cnt_match:
      tot_count+= int(cnt_val[0], 16)
    if tot_count > 0:
      fw_count_str += techs[tech_id] + '\n'
      fw_count_str += '{0:^30} {1:^30} {2:^30}'\
                        .format('EVENT', 'COUNT', 'TIMESTAMP (Qtimer)') + "\n"
      for event_id in range(0, len(events)) :
        fw_count_str += '{0:30} {1:^30} {2:^30}'\
                        .format( events[event_id], cnt_match[event_id][0], cnt_match[event_id][1] ) + '\n'
      fw_count_str += '\n'
    line = in_file.readline()

  return fw_count_str, line

def get_trace_event_name(index):
  cxm_trace_events = \
    [
      'CXM_TRC_INVALID',
      'CXM_TRC_TSK_INIT',
      'CXM_TRC_TSK_DEINIT',

      'CXM_TRC_MSGR_RCV_MSG',
      'CXM_TRC_MSGR_SND_MSG',
      'CXM_TRC_MSGR_WWAN_TECH_STATE_INFO',
      'CXM_TRC_MSGR_WWAN_TECH_SLEEP_WAKEUP',
      'CXM_TRC_MSGR_LTE_SFN',
      'CXM_TRC_MSGR_LTE_BLER',
      'CXM_TRC_MSGR_LTE_SINR',
      'CXM_TRC_MSGR_LTE_TX_PWR_LMT',
      'CXM_TRC_MSGR_LTE_RRC_STATE',
      'CXM_TRC_MSGR_WCN_RPT',

      'CXM_TRC_QMI_SVC_INIT',
      'CXM_TRC_QMI_SVC_DEINIT',
      'CXM_TRC_QMI_SVC_OPEN',
      'CXM_TRC_QMI_SVC_CLOSE',
      'CXM_TRC_QMI_CLNT_CONNECT_CB',
      'CXM_TRC_QMI_CLNT_DISCONNECT_CB',
      'CXM_TRC_QMI_SVC_HNDL_REQ_CB',
      'CXM_TRC_QMI_SND_IND',
      'CXM_TRC_QMI_SND_RESP',
      'CXM_TRC_QMI_PUSH_DEF_REQ',
      'CXM_TRC_QMI_POP_DEF_REQ',

      'CXM_TRC_QMI_HNDL_REG_IND_REQ',
      'CXM_TRC_QMI_SET_POLICY_REQ',
      'CXM_TRC_QMI_STRT_BLER_REQ',
      'CXM_TRC_QMI_STRT_SINR_REQ',
      'CXM_TRC_QMI_LTE_TX_PWR_LMT_SUCC',
      'CXM_TRC_QMI_LTE_TX_PWR_LMT_FAIL',
      'CXM_TRC_QMI_TX_SUBFRAME_DENIAL',
      'CXM_TRC_UART_RCV_MSG',
      'CXM_TRC_UART_SND_MSG',
      'CXM_TRC_QMI_UPDATE_WLAN_CONN',
      'CXM_TRC_QMI_UPDATE_WLAN_SCAN',
      'CXM_TRC_COEX_CONFLICT_START',
      'CXM_TRC_COEX_CONFLICT_END',
      'CXM_TRC_SET_LTE_TX_PWR_LMT',
      'CXM_TRC_QMI_SET_SLP_NOTFY_THRESH',
      'CXM_TRC_QMI_SND_WWAN_STATE_IND',
      'CXM_TRC_ERR_FATAL',
      'CXM_ANT_RELEASE_AND_REQUEST',
      'CXM_ANT_GRANT_CB',
      'CXM_ANT_UNLOCK_CB',
      'CXM_TRC_SMDL_RCV_NOT',
      'CXM_TRC_SMDL_RCV_MSG',
      'CXM_TRC_SMDL_SND_MSG',
      'CXM_TRC_WCI2_RX_CB',
      'CXM_TRC_WCI2_PROC_RX'
    ]

  if index>= len(cxm_trace_events):
    return 'UNKNOWN'
  else:
    return cxm_trace_events[index]



def read_and_parse_cxm_trace_buffer(in_file, trace_version, line):
  trace_log_table = TraceBuffer()
  last_index = ''
  buff_index = ''
  while line != '':
    if 'seq_num' in line:
      # next line is the seq number
      line = in_file.readline()
      # parse the ending point in the circular log buffer
      values = line.split('=')
      last_index = int(values[1].strip(' ),\n'), 16) # convert to hex
      break
    # prep to continue while loop
    line = in_file.readline()

  #determine the sequence number of the first logged item
  if num_events_log > last_index:
    buff_index = 0
  else:
    #this is the index we want the first entry in the buffer to have. It may not
    #be zero since this is a circular buffer
    buff_index = num_events_log - ( (last_index+1) % num_events_log )

  # find the starting point of the trace buffer so we can parse it
  while line != '':
    if 'events' in line:
      # the next line is the start of the trace buffer
      break
    # prep to continue loop
    line = in_file.readline()

  # assume we've found the start of the trace buffer, and parse it
  line = in_file.readline()
  # in version 2, sequence ID has been removed so we calculate it so we can use
  #  it for sorting the buffer
  if trace_version == 2:
    while line != '':
      values = line.split(',')
      if len(values) > 5:
        #assign seq id
        seq_id = buff_index % num_events_log
        #increment our buff index
        buff_index+=1
        # get event enum
        event = get_trace_event_name( int(values[0].split('=')[1].strip(), 16) )
        # get a32
        a32 = int(values[1].split('=')[1].strip(), 16)
        # get b32
        b32 = int(values[2].split('=')[1].strip(), 16)
        # get c32
        c32 = int(values[3].split('=')[1].strip(), 16)
        # get ts
        ts = int(values[4].split('=')[1].strip(' ),\n'), 16)
        # save read values to interpret and print later
        trace_log_table.add(CxmTraceBufferLine(seq_id, ts, event, a32, b32, c32, 0))
      else:
        # this line doesn't match the trace buffer expression, so we must be done
        break
      # prep to continue while loop
      line = in_file.readline().strip()

  # earlier version of the trace has the service ID, later ones do not.
  elif trace_version == 1:
    while line != '':
      values = line.split(',')
      if len(values) > 6:
        # get seq_id
        seq_id = int(values[0].split('=')[1].strip(), 16)
        # get event enum
        event = values[1].split('=')[1].strip()
        # get a32
        a32 = int(values[2].split('=')[1].strip(), 16)
        # get b32
        b32 = int(values[3].split('=')[1].strip(), 16)
        # get c32
        c32 = int(values[4].split('=')[1].strip(), 16)
        # get ts
        ts = int(values[5].split('=')[1].strip(' ),\n'), 16)
        # save read values to interpret and print later
        trace_log_table.add(CxmTraceBufferLine(seq_id, ts, event, a32, b32, c32, 0))
      else:
        # this line doesn't match the trace buffer expression, so we must be done
        break
      # prep to continue while loop
      line = in_file.readline().strip()
  else:
    while line != '':
      values = line.split(',')
      if len(values) > 6:
        # get seq_id
        seq_id = int(values[0].split('=')[1].strip(), 16)
        # get srvc_id
        srvc_id = int(values[1].split('=')[-1].strip(), 16)
        # get event enum
        event = values[2].split('=')[1].strip()
        # get a32
        a32 = int(values[3].split('=')[1].strip(), 16)
        # get b32
        b32 = int(values[4].split('=')[1].strip(), 16)
        # get c32
        c32 = int(values[5].split('=')[1].strip(), 16)
        # get ts
        ts = int(values[6].split('=')[1].strip(), 16)
        # save read values to interpret and print later
        trace_log_table.add(CxmTraceBufferLine(seq_id, ts, event, a32, b32, c32, srvc_id))
      else:
        # this line doesn't match the trace buffer expression, so we must be done
        break
      # prep to continue while loop
      line = in_file.readline().strip()

  # sort table by timestamp and return
  trace_log_table.sort()
  return trace_log_table, line

def decode_policy_mask(policy):
  masks = (('Toggle_Frame_Sync', 0x01),
           ('Toggle_TX_Active',  0x02),
           ('Toggle_RX_Priority', 0x04),
           ('React_To_WCN_Prio', 0x08),
           ('Send_WCI2_Type3_Inact_Dur', 0x10),
           ('Send_WCI2_Type6_TX_Adv_Notice', 0x20),
           ('Enforce_Contr_TX_Power_Limit', 0x40),
           ('Enforce_WCI-2_TX_Power_Limit', 0x80))
  policy_list = []
  # check each mask bit in turn; if on, append to the list
  for i in range(0, len(masks)):
    if policy & masks[i][1]:
      policy_list.append(masks[i][0])
  return policy_list

def create_umid_dictionary(path):
  umids = {}
  try:
    print "Reading UMIDs from " + path
    umid_file = open(path, "r")
    regex = re.compile('0x\w{8}, "\w+"')
    for line in umid_file:
      # generate dictionary mapping UMID to string
      match = regex.search(line)
      if match:
        sub1 = match.group()
        sub2 = sub1.split(',')
        # extract umid and umid_names from the line, if present
        umid = int(sub2[0].strip(), 16)
        name = sub2[1].strip(' ",')
        # add to dictionary
        umids[umid] = name
    umid_file.close()
  except IOError:
    print "ERROR: Could not open umid file. Continuing without."
  return umids

def create_qmi_msg_id_dict():
  # dictionary mapping QMI MSG IDs to a string name
  # these should never change. found in coexistence_service_v01.h
  msgs = {
    0x00 : 'COEX_RESET',
    0x20 : 'COEX_INDICATION_REGISTER',
    0x21 : 'COEX_WWAN_STATE_IND',
    0x22 : 'COEX_GET_WWAN_STATE',
    0x23 : 'COEX_SET_WWAN_STATE',
    0x24 : 'COEX_GET_WLAN_SCAN_STATE',
    0x25 : 'COEX_GET_WLAN_CONN_STATE',
    0x26 : 'COEX_SET_POLICY',
    0x27 : 'COEX_GET_POLICY',
    0x28 : 'COEX_METRICS_LTE_BLER_START',
    0x29 : 'COEX_METRICS_LTE_BLER_IND',
    0x2A : 'COEX_METRICS_LTE_BLER_STOP',
    0x2B : 'COEX_METRICS_LTE_SINR_START',
    0x2C : 'COEX_METRICS_LTE_SINR_READ',
    0x2D : 'COEX_METRICS_LTE_SINR_STOP',
    0x2E : 'COEX_SET_BAND_FILTER_INFO',
    0x2F : 'COEX_GET_BAND_FILTER_INFO',
    0x30 : 'COEX_CONDITION_FAIL_IND',
    0x31 : 'COEX_CONDITION_SUCCESS_IND',
    0x32 : 'COEX_GET_WCI2_MWS_PARAMS'
  }
  return msgs

def create_counter_dict():
  # dictionary mapping counter index to a string name
  # these will have to be updated if the enum changes
  counters = {
    0  : 'POLICY_RESET',
    1  : 'POLICY_UPDATES',
    2  : 'UART_RX',
    3  : 'UART_RX_WCI2_TYPE1',
    4  : 'UART_RX_WCI2_TYPE6',
    5  : 'UART_RX_WCI2_TYPE_UNSUPPORTED',
    6  : 'UART_TX',
    7  : 'UART_TX_WCI2_TYPE1',
    8  : 'UART_TX_WCI2_TYPE3',
    9  : 'UART_TX_WCI2_TYPE6',
    10 : 'UART_TX_WCI2_TYPE_UNSUPPORTED',
    11 : 'LTE_ML1_ADVANCE_NOTICE_RECVD',
    12 : 'LTE_ML1_SLEEP_DURN_RECVD',
    13 : 'CONTR_PWR_LMT_REQ',
    14 : 'CONTR_PWR_LMT_COND_FAIL',
    15 : 'CONTR_PWR_LMT_HONORED',
    16 : 'WCI2_PWR_LMT_REQ',
    17 : 'WCI2_PWR_LMT_COND_FAIL',
    18 : 'WCI2_PWR_LMT_HONORED',
    19 : 'TX_ADVANCED_MSG_LATE',
    20 : 'TX_ADVANCED_TRANSMIT'
  }
  return counters

def create_tech_id_dict():
  # dictionary mapping tech ID enum to a string name
  techs = {
    -1 : 'INVLD',
    0 : 'LTE',
    1 : 'TDSCDMA',
    2 : 'GSM1',
    3 : 'ONEX',
    4 : 'HDR',
    5 : 'WCDMA',
    6 : 'GSM2'
  }
  return techs

def get_fw_counter_events():
  events = [\
    'CXM_CNT_FRAME_SYNC',
    'CXM_CNT_TX_ACTIVE',
    'CXM_CNT_RX_PRIO',
    'CXM_CNT_WCN_PRIO_BLANK_REQ',
    'CXM_CNT_WCN_PRIO_BLANK_DENIED',
    'CXM_CNT_RESERVED'\
  ]
  return events

def generate_out_file(cxmTrace, state_str, counters_str, wwan_state_str, fw_counters_str, wci2Trace, working_dir, wci2_parsing):
  ofilename = 'cxm_log_output' + str(time.time()) + '.txt'
  ofilepath = os.path.join(working_dir, ofilename)
  os.path.normpath(ofilepath)
  try:
    out_file = open(ofilepath, 'w')

    # first, write the CXM STATE string to the file
    if state_str != "":
      out_file.write(state_str)

    # next, write the WWAN STATE string to the file
    if wwan_state_str != "":
      out_file.write(wwan_state_str)

    # next, write the counters string if it exists.
    if counters_str != "":
      out_file.write(counters_str)

    # next, write the fw counters string if it exists.
    if fw_counters_str != "":
      out_file.write(fw_counters_str)

    # next, write the trace buffer (formatted) to the file
    out_file.write('-----------------------------------------------------------------------------------------------------------\n')
    if wci2_parsing == 'yes' and wci2Trace is not None:
      out_file.write('CXM TRACE BUFFER\n')
      out_file.write('-----------------------------------------------------------------------------------------------------------\n')
      cxmTrace.writeToFile(out_file)
      out_file.write('\n\n-----------------------------------------------------------------------------------------------------------\n')
      out_file.write('WCI2 TRACE BUFFER\n')
      out_file.write('-----------------------------------------------------------------------------------------------------------\n')
      wci2Trace.writeToFile(out_file)
    elif wci2_parsing == 'interleave' and wci2Trace is not None:
      out_file.write('CXM/WCI2 COMBINED TRACE BUFFER\n')
      out_file.write('-----------------------------------------------------------------------------------------------------------\n')
      # print interleaved wci2 and cxm lines to file
      cxmTrace.merge(wci2Trace)
      cxmTrace.writeToFile(out_file)
    else:
      # only cxm trace wanted
      out_file.write('CXM TRACE BUFFER\n')
      out_file.write('-----------------------------------------------------------------------------------------------------------\n')
      cxmTrace.writeToFile(out_file)

    out_file.close()
    print "DONE: output file = " + ofilename

  except IOError:
    print "ERROR: Could not write output file " + ofilepath
    sys.exit(3)

# --------------------------------------------------------------------------- #
#                             WCI2 TRACE PROCESSING
# --------------------------------------------------------------------------- #
def read_and_parse_wci2_trace_buffer(in_file, line):
  # right now, wci2 struct layout is exactly the same as cxm trace buffer,
  # so just reuse the same parser
  trace_log_table = TraceBuffer()
  last_index = ''
  buff_index = ''
  while line != '':
    if 'seq_num' in line:
      # next line is the seq number
      line = in_file.readline()
      # parse the ending point in the circular log buffer
      values = line.split('=')
      last_index = int(values[1].strip(' ),\n'), 16) # convert to hex
      break
    # prep to continue while loop
    line = in_file.readline()

  # determine the sequence number of the first logged item
  if num_events_log > last_index:
    buff_index = 0
  else:
    # this is the index we want the first entry in the buffer to have. It may not
    # be zero since this is a circular buffer
    buff_index = num_events_log - ( (last_index+1) % num_events_log )

  # find the starting point of the trace buffer so we can parse it
  while line != '':
    if 'events' in line:
      # the next line is the start of the trace buffer
      break
    # prep to continue loop
    line = in_file.readline()

  # assume we've found the start of the trace buffer, and parse it
  line = in_file.readline()
  while line != '':
    values = line.split(',')
    if len(values) > 5:
      #assign seq id
      seq_id = buff_index % num_events_log
      #increment our buff index
      buff_index+=1
      # get event enum
      event = get_wci2_trace_event_name( int(values[0].split('=')[1].strip(), 16) )
      # get a32
      a8 = int(values[1].split('=')[1].strip(), 16)
      # get b32
      b16 = int(values[2].split('=')[1].strip(), 16)
      # get c32
      c32 = int(values[3].split('=')[1].strip(), 16)
      # get ts
      ts = int(values[4].split('=')[1].strip(' ),\n'), 16)

      # save read values to interpret and print later
      trace_log_table.add(Wci2TraceBufferLine(seq_id, ts, event, a8, b16, c32))
    else:
      # this line doesn't match the trace buffer expression, so we must be done
      break
    # prep to continue while loop
    line = in_file.readline().strip()

  return trace_log_table, line

def get_wci2_trace_event_name(index):
  wci2_trace_events = \
    [
      'WCI2_TRC_INVALID',
      'WCI2_TRC_TSK_INIT',
      'WCI2_TRC_TSK_DEINIT',
      'WCI2_TRC_UART_RCV_MSG',
      'WCI2_TRC_UART_SND_MSG',
      'WCI2_TRC_DIR_READ_CB',
      'WCI2_TRC_SIO_RX_CB',
      'WCI2_TRC_RX_WCI2',
      'WCI2_TRC_TX_WCI2',
      'WCI2_TRC_T2_MISALIGNED',
      'WCI2_TRC_SET_CXM_MODE',
      'WCI2_TRC_SET_BAUD',
      'WCI2_TRC_UART_POWER_STATE',
      'WCI2_TRC_CLIENT_REG',
      'WCI2_TRC_CLIENT_DEREG',
      'WCI2_TRC_CLNT_NTFY_T2_RX',
      'WCI2_TRC_CLNT_NTFY_DIR_RX',
      'WCI2_TRC_CLNT_NTFY_BASIC_RX'
    ]
  if index >= len(wci2_trace_events):
    return 'UNKNOWN'
  else:
    return wci2_trace_events[index]

def create_uart_state_dict():
  # dictionary mapping tech ID enum to a string name
  uart_state = {
    0 : 'OFF',
    1 : 'FLUSHING',
    2 : 'ON'
  }
  return uart_state

def create_uart_baud_dict():
  baud = {
    0: '115200',
    1: '2000000',
    2: '3000000'
  }
  return baud

def create_uart_mode_dict():
  # dictionary mapping CXM_UART mode enum to readable string
  uart_mode = {
    0: 'CLIENT_MODE_BASIC',
    1: 'CLIENT_MODE_WCI2'
  }
  return uart_mode

def create_uart_retval_dict():
  retval = {
    0: 'WCI2_E_SUCCESS',
    1: 'WCI2_E_FAIL',
    2: 'WCI2_E_INVALID_ARG',
    3: 'WCI2_E_NO_MORE_CLIENTS',
    4: 'WCI2_E_UART_MODE_CONFLICT',
    5: 'WCI2_E_BAUD_CONFLICT',
    6: 'WCI2_E_FRAME_MODE_CONFLICT',
    7: 'WCI2_E_FRAME_CRC_MISMATCH',
    8: 'WCI2_E_FRAME_INVALID',
    9: 'WCI2_E_OUT_OF_MEM'
  }
  return retval

# --------------------------------------------------------------------------- #
#                             ENTRY POINT                                     #
# --------------------------------------------------------------------------- #

print "Running " + sys.argv[0] + "\n"
cur_dir =  os.getcwd()
main()

