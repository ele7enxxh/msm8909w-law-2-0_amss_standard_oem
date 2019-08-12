import sys, re, os
from base_parser import Parser
from target_data import get_branch

#pmic_ids format = pmic_index : pmic_id
pmic_ids = {
    0 : 'A',
    1 : 'B',
    2 : 'C',
    3 : 'D',
    4 : 'E',
    5 : 'F',
    6 : 'G',
}

#periph_names format = periph_type : periph_name
periph_names = {

    3  : 'SMPS',
    4  : 'LDO',
    5  : 'VS',
    6  : 'CLK',
    27 : 'BOOST',
    28 : 'SMPS',
    31 : 'BOOST_BYP',
    33 : 'LDO',
    34 : 'SMPS',
}

#enable_states format = value : en_state
enable_states = {
    0  : 'DISABLED',
    1  : 'ENABLED',
}

#pinctrl_states format = value : pc_state
pinctrl_states = {
    1  : 'FOLLOW_HW_EN0',
    2  : 'FOLLOW_HW_EN1',
    4  : 'FOLLOW_HW_EN2',
    8  : 'FOLLOW_HW_EN3',
}

#pinctrl_mode_states format = value : pc_mode_state
pinctrl_mode_states = {
    1  : 'NPM_FOLLOW_HW_EN0',
    2  : 'NPM_FOLLOW_HW_EN1',
    4  : 'NPM_FOLLOW_HW_EN2',
    8  : 'NPM_FOLLOW_HW_EN3',
}

#mode_states format = value : mode
mode_states = {
    0  : 'LPM',
    1  : 'BYPASS',
    2  : 'AUTO',
    4  : 'NPM',
}

#error_info format = error_value : error_info
error_info = {
    9  : 'SPMI Bus Error!',
    14 : 'First Param out of range!',
    15 : 'Second Param out of range!',
    16 : 'Third Param out of range!',
    22 : 'Feature not supported!',
    36 : 'Macro not recognized!',
    47 : 'Invalid Periph Index!',
    114 : 'Invalid pointer!',
}

def get_pmic_rsrc_name(pmic_index, periph_type, periph_index):
    rsrc_name = ''
    try:
        rsrc_name = periph_names[periph_type] + '%d' % (periph_index+1) + pmic_ids[pmic_index]
    except:
        rsrc_name = '[pmic_index = %d, periph_type = %d, periph_index = %d]' % (pmic_index, periph_type, periph_index)
    return rsrc_name

def get_enable_state(enable_value):
    return enable_states.get(enable_value, 'Unknown state %d' % enable_value)

def get_pinctrl_state(pin_value):
    pc_state = ''
    try:
        if pin_value <= 0x0F:
            if (pin_value & 0x01) == 0x01:
                pc_state = pinctrl_states[1] + ' '
            if (pin_value & 0x02) == 0x02:
                pc_state = pc_state + pinctrl_states[2] + ' '
            if (pin_value & 0x04) == 0x04:
                pc_state = pc_state + pinctrl_states[4] + ' '
            if (pin_value & 0x08) == 0x08:
                pc_state = pc_state + pinctrl_states[8]

        if (pc_state == ''):
            pc_state = 'Unknown pin value %d' % pin_value
    except:
        pc_state = 'Unknown pin value %d' % pin_value
    return pc_state

def get_pinctrl_mode_state(pin_value):
    pc_mode_state = ''
    try:
        if pin_value <= 0x0F:
            if (pin_value & 0x01) == 0x01:
                pc_mode_state = pinctrl_mode_states[1] + ' '
            if (pin_value & 0x02) == 0x02:
                pc_mode_state = pc_mode_state + pinctrl_mode_states[2] + ' '
            if (pin_value & 0x04) == 0x04:
                pc_mode_state = pc_mode_state + pinctrl_mode_states[4] + ' '
            if (pin_value & 0x08) == 0x08:
                pc_mode_state = pc_mode_state + pinctrl_mode_states[8]

        if (pc_mode_state == ''):
            pc_mode_state = 'Unknown pin value %d' % pin_value
    except:
        pc_mode_state = 'Unknown pin value %d' % pin_value
    return pc_mode_state

def get_mode_state(mode_value):
    return mode_states.get(mode_value, 'Unknown mode %d' % mode_value)

def get_error_info(err_value):
    return error_info.get(err_value, 'Unknown error %d' % err_value)

class PMIC_MSG_x201:
	# PMICLdoStartApply
	# PMICDrvPwrSwEn
	__metaclass__ = Parser
	id = 0x201
	def parse_v1(self,data):
		if data[1] == 0:
			return 'START Apply() LDO%dA' % data[0]
		else:
			return 'START Apply() LDO%dB' % data[0]
	def parse_v2(self,data):
		return 'PMIC %s SW Enable state = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_enable_state(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)
				

class PMIC_MSG_x202:
	# PMICLdoStartPreDep
	# PMICDrvPwrPinCtrl
	__metaclass__ = Parser
	id = 0x202
	def parse_v1(self, data):
		if data[1] == 0:
			return 'START Pre-Dep() LDO%dA' % data[0]
		else:
			return 'START Pre-Dep() LDO%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Pin Ctrl state = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_pinctrl_state(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x203:
	# PMICLdoEndPreDep
	# PMICDrvPwrPinCtrlMode
	__metaclass__ = Parser
	id = 0x203
	def parse_v1(self, data):
		if data[1] == 0:
			return 'END Pre-Dep() LDO%dA' % data[0]
		else:
			return 'END Pre-Dep() LDO%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Pin Ctrl Mode state = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_pinctrl_mode_state(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x204:
	# PMICLdoStartPostDep
	# PMICDrvPwrVoltLvl
	__metaclass__ = Parser
	id = 0x204
	def parse_v1(self, data):
		if data[1] == 0:
			return 'START Post-Dep() LDO%dA' % data[0]
		else:
			return 'START Post-Dep() LDO%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Volt Level = %d micro volts' % (get_pmic_rsrc_name(data[0], data[1], data[2]), data[3])
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x205:
	# PMICLdoEndPostDep
	# PMICDrvPwrPinVoltLvl
	__metaclass__ = Parser
	id = 0x205
	def parse_v1(self, data):
		if data[1] == 0:
			return 'END Post-Dep() LDO%dA' % data[0]
		else:
			return 'END Post-Dep() LDO%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Pin Volt Level = %d micro volts' % (get_pmic_rsrc_name(data[0], data[1], data[2]), data[3])
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x206:
	# PMICLdoAggregation1
	# PMICDrvPwrMode
	__metaclass__ = Parser
	id = 0x206
	def parse_v1(self, data):
		if data[1] == 0:
			return ("START Execute_Driver()\n"
				"\t\tLDO%dA setting:\n"
				"\t\tsw_en = %d\n"
				"\t\tldo_sw_mode = %d") %\
				(data[0], data[2], data[3])
		else:
			return ("START Execute_Driver()\n"
				"\t\tLDO%dB setting:\n"
				"\t\tsw_en = %d\n"
				"\t\tldo_sw_mode = %d") %\
				(data[0], data[2], data[3])
	def parse_v2(self, data):
		return 'PMIC %s Mode = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_mode_state(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x207:
	# PMICLdoAggregation2
	# PMICDrvPullDownErr
	__metaclass__ = Parser
	id = 0x207
	def parse_v1(self, data):
		return ("\tpc_en = %d\n"
			"\t\tpc_mode = %d\n"
			"\t\tis_en_transition = %d\n"
			"\t\tip = %d") %\
			(data[0], data[1], data[2], data[3])
	def parse_v2(self, data):
		return 'PMIC %s Pull Down Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x208:
	# PMICLdoAggregation3
	# PMICDrvSwEnErr
	__metaclass__ = Parser
	id = 0x208
	def parse_v1(self, data):
		return ("\tregulated_uvol = %d\n"
			"\t\tbypass_uv = %d\n"
			"\t\tnoise_hr = %d\n"
			"\t\tbyp_allowed = %d") %\
			(data[0], data[1], data[2], data[3])
	def parse_v2(self, data):
		return 'PMIC %s SW Enable Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x209:
	# PMICLdoAggregation4
	# PMICDrvPinCtrlErr
	__metaclass__ = Parser
	id = 0x209
	def parse_v1(self, data):
		return ("\tis_en_transition = %d\n"
			"\t\tnoise_hr = %d\n"
			"\t\ten_byp = %d\n"
			"\t\tbypass_uv = %d") %\
			(data[0], data[1], data[2], data[3])
	def parse_v2(self, data):
		return 'PMIC %s Pin Ctrl Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x20A:
	# PMICLdoAggregation5
	# PMICDrvVoltLevelErr
	__metaclass__ = Parser
	id = 0x20A
	def parse_v1(self, data):
		return ("\tbyp_allowed = %d") % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Volt Level Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x20B:
	# PMICLdoPowerMode
	# PMICDrvPinVoltLevelErr
	__metaclass__ = Parser
	id = 0x20B
	def parse_v1(self, data):
		return {
			0: "Setting Mode: IPEAK - NPM",
			1: "Setting Mode: IPEAK - PFM",
			2: "Setting Mode: NPM"
			} [data[0]]
	def parse_v2(self, data):
		return 'PMIC %s Pin Volt Level Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x20C:
	# PMICLdoNoChange
	# PMICDrvPwrModeErr
	__metaclass__ = Parser
	id = 0x20C
	def parse_v1(self, data):
		if data[1] == 0:
			return 'NO CHANGES DETECTED LDO%dA' % data[0]
		else:
			return 'NO CHANGES DETECTED LDO%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Mode Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x20D:
	# PMICSmpsStartApply
	# PMICDrvPwrIrqErr
	__metaclass__ = Parser
	id = 0x20D
	def parse_v1(self, data):
		if data[1] == 0:
			return 'START Apply() SMPS%dA' % data[0]
		else:
			return 'START Apply() SMPS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Irq Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x20E:
	# PMICSmpsStartPreDep
	# PMICDrvSmpsIlimErr
	__metaclass__ = Parser
	id = 0x20E
	def parse_v1(self, data):
		if data[1] == 0:
			return 'START Pre-Dep() SMPS%dA' % data[0]
		else:
			return 'START Pre-Dep() SMPS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Ilim error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x20F:
	# PMICSmpsEndPreDep
	# PMICDrvSmpsQmErr
	__metaclass__ = Parser
	id = 0x20F
	def parse_v1(self, data):
		if data[1] == 0:
			return 'END Pre-Dep() SMPS%dA' % data[0]
		else:
			return 'END Pre-Dep() SMPS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Quiet mode error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x210:
	# PMICSmpsStartPostDep
	# PMICDrvSmpsSwFreqErr
	__metaclass__ = Parser
	id = 0x210
	def parse_v1(self, data):
		if data[1] == 0:
			return 'START Post-Dep() SMPS%dA' % data[0]
		else:
			return 'START Post-Dep() SMPS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Switch Freq error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x211:
	# PMICSmpsEndPostDep
	# PMICDrvSmpsPhaseErr
	__metaclass__ = Parser
	id = 0x211
	def parse_v1(self, data):
		if data[1] == 0:
			return 'END Post-Dep() SMPS%dA' % data[0]
		else:
			return 'END Post-Dep() SMPS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Phase error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x212:
	# PMICSmpsAggregation1
	# PMICDrvSmpsSoftstartErr
	__metaclass__ = Parser
	id = 0x212
	def parse_v1(self, data):
		if data[1] == 0:
			return ("START Execute_Driver()\n"
				"\t\tSMPS%dA setting:\n"
				"\t\tsw_en = %d\n"
				"\t\tsmps_sw_mode = %d") %\
				(data[0], data[2], data[3])
		else:
			return ("START Execute_Driver()\n"
				"\t\tSMPS%dB setting:\n"
				"\t\tsw_en = %d\n"
				"\t\tsmps_sw_mode = %d") %\
				(data[0], data[2], data[3])
	def parse_v2(self, data):
		return 'PMIC %s Softstart error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x213:
	# PMICSmpsAggregation2
	# PMICDrvSmpsVstepErr
	__metaclass__ = Parser
	id = 0x213
	def parse_v1(self, data):
		return ("\tpc_en = %d\n"
			"\t\tpc_mode = %d\n"
			"\t\tglobal_byp_en = %d\n"
			"\t\tuvol = %d") %\
			(data[0], data[1], data[2], data[3])
	def parse_v2(self, data):
		return 'PMIC %s Vstep error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x214:
	# PMICSmpsAggregation3
	# PMICDrvClkSwEn
	__metaclass__ = Parser
	id = 0x214
	def parse_v1(self, data):
		return ("\tip = %d\n"
			"\t\tfreq = %d\n"
			"\t\tfreq_reason = %d\n"
			"\t\tquiet_mode = %d") %\
			(data[0], data[1], data[2], data[3])
	def parse_v2(self, data):
		return 'PMIC %s SW Enable state = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_enable_state(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x215:
	# PMICSmpsAggregation4
	# PMICDrvClkPinCtrl
	__metaclass__ = Parser
	id = 0x215
	def parse_v1(self, data):
		return ("\tbyp_allowed = %d\n"
			"\t\thr = %d") %\
			(data[0], data[1])
	def parse_v2(self, data):
		return 'PMIC %s Pin Ctrl state = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_enable_state(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x216:
	# PMICSmpsPowerMode
	# PMICDrvSwEnErr
	__metaclass__ = Parser
	id = 0x216
	def parse_v1(self, data):
		return {
			0: "Setting Mode: AUTO",
			1: "Setting Mode: IPEAK - NPM",
			2: "Setting Mode: IPEAK - LPM",
			3: "Setting Mode: NPM"
			} [data[0]]
	def parse_v2(self, data):
		return 'PMIC %s SW Enable Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x217:
	# PMICSmpsNoChange
	# PMICDrvPinCtrlErr
	__metaclass__ = Parser
	id = 0x217
	def parse_v1(self, data):
		if data[1] == 0:
			return 'NO CHANGES DETECTED SMPS%dA' % data[0]
		else:
			return 'NO CHANGES DETECTED SMPS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Pin Ctrl Error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x218:
	# PMICVsStartApply
	# PMICRpmVregSettlingErr
	__metaclass__ = Parser
	id = 0x218
	def parse_v1(self, data):
		if data[1] == 0:
			return 'START Apply() VS%dA' % data[0]
		else:
			return 'START Apply() VS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Vreg Settling error' % (get_pmic_rsrc_name(data[0], data[1], data[2]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x219:
	# PMICVsStartPreDep
	# PMICRpmStepperSettlingErr
	__metaclass__ = Parser
	id = 0x219
	def parse_v1(self, data):
		if data[1] == 0:
			return 'START Pre-Dep() VS%dA' % data[0]
		else:
			return 'START Pre-Dep() VS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s Stepper Settling' % (get_pmic_rsrc_name(data[0], data[1], data[2]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMIC_MSG_x21A:
	# PMICVsEndPreDep
	# PMICDrvSmpsOcpErr
	__metaclass__ = Parser
	id = 0x21a
	def parse_v1(self, data):
		if data[1] == 0:
			return 'END Pre-Dep() VS%dA' % data[0]
		else:
			return 'END Pre-Dep() VS%dB' % data[0]
	def parse_v2(self, data):
		return 'PMIC %s OCP error = %s' % (get_pmic_rsrc_name(data[0], data[1], data[2]), get_error_info(data[3]))
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 3:
				return self.parse_v1(data)
			elif get_branch('minor') >= 4:
				return self.parse_v2(data)
		elif get_branch('major') == 2:
			return self.parse_v1(data)

class PMICVsStartPostDep:
	__metaclass__ = Parser
	id = 0x21b
	def parse(self, data):
		if data[1] == 0:
			return 'START Post-Dep() VS%dA' % data[0]
		else:
			return 'START Post-Dep() VS%dB' % data[0]

class PMICVsEndPostDep:
	__metaclass__ = Parser
	id = 0x21c
	def parse(self, data):
		if data[1] == 0:
			return 'END Post-Dep() VS%dA' % data[0]
		else:
			return 'END Post-Dep() VS%dB' % data[0]

class PMICVsAggregation1:
	__metaclass__ = Parser
	id = 0x21d
	def parse(self, data):
		if data[1] == 0:
			return ("START Execute_Driver()\n"
				"\t\tVS%dA setting:\n"
				"\t\tsw_en = %d\n"
				"\t\tpc_en = %d") %\
				(data[0], data[2], data[3])
		else:
			return ("START Execute_Driver()\n"
				"\t\tVS%dB setting:\n"
				"\t\tsw_en = %d\n"
				"\t\tpc_en = %d") %\
				(data[0], data[2], data[3])

class PMICVsAggregation2:
	__metaclass__ = Parser
	id = 0x21e
	def parse(self, data):
		return ("\tuvol = %d\n"
			"\t\tip = %d\n"
			"\t\tisInitialized = %d") %\
			(data[0], data[1], data[2])

class PMICVsNoChange:
	__metaclass__ = Parser
	id = 0x21f
	def parse(self, data):
		if data[1] == 0:
			return 'NO CHANGES DETECTED VS%dA' % data[0]
		else:
			return 'NO CHANGES DETECTED VS%dB' % data[0]

class PMICClkBufStartApply:
	__metaclass__ = Parser
	id = 0x220
	def parse(self, data):
		if data[1] == 0:
			return 'START Apply() CLK BUFFER%dA' % data[0]
		else:
			return 'START Apply() CLK BUFFER%dB' % data[0]

class PMICClkBufStartPreDep:
	__metaclass__ = Parser
	id = 0x221
	def parse(self, data):
		if data[1] == 0:
			return 'START Pre-Dep() CLK BUFFER%dA' % data[0]
		else:
			return 'START Pre-Dep() CLK BUFFER%dB' % data[0]

class PMICClkBufEndPreDep:
	__metaclass__ = Parser
	id = 0x222
	def parse(self, data):
		if data[1] == 0:
			return 'END Pre-Dep() CLK BUFFER%dA' % data[0]
		else:
			return 'END Pre-Dep() CLK BUFFER%dB' % data[0]

class PMICClkBufStartPostDep:
	__metaclass__ = Parser
	id = 0x223
	def parse(self, data):
		if data[1] == 0:
			return 'START Post-Dep() CLK BUFFER%dA' % data[0]
		else:
			return 'START Post-Dep() CLK BUFFER%dB' % data[0]

class PMICClkBufEndPostDep:
	__metaclass__ = Parser
	id = 0x224
	def parse(self, data):
		if data[1] == 0:
			return 'END Post-Dep() CLK BUFFER%dA' % data[0]
		else:
			return 'END Post-Dep() CLK BUFFER%dB' % data[0]

class PMICClkBufAggregation1:
	__metaclass__ = Parser
	id = 0x225
	def parse(self, data):
		if data[1] == 0:
			return ("START Execute_Driver()\n"
				"\t\tCLK BUFFER%dA setting:\n"
				"\t\tsw_en = %d\n"
				"\t\tpc_en = %d") %\
				(data[0], data[2], data[3])
		else:
			return ("START Execute_Driver()\n"
				"\t\tCLK BUFFER%dB setting:\n"
				"\t\tsw_en = %d\n"
				"\t\tpc_en = %d") %\
				(data[0], data[2], data[3])

class PMICClkBufAggregation2:
	__metaclass__ = Parser
	id = 0x226
	def parse(self, data):
		return ("\tisInitialized = %d") % data[0]

class PMICClkBufNoChange:
	__metaclass__ = Parser
	id = 0x227
	def parse(self, data):
		if data[1] == 0:
			return 'NO CHANGES DETECTED CLK BUFFER%dA' % data[0]
		else:
			return 'NO CHANGES DETECTED CLK BUFFER%dB' % data[0]

class PMIC_MSG_x228:
	# PMICDrvPullDownErr
	# PMICSwEnableSetting
	__metaclass__ = Parser
	id = 0x228
	def parse_v1(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV PULL DOWN ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV PULL DOWN ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV PULL DOWN ERROR -- Resource%d Spmi Bus Error!' % data[0]
	def parse_v2(self, data):
		return "\tWrote value to sw_en register: 0x%02x" % data[0]
	def parse(self, data):
		if get_branch('major') == 1:
			return self.parse_v1(data)
		elif get_branch('major') == 2:
			return self.parse_v2(data)

class PMIC_MSG_x229:
	# PMICDrvSwEnableErr
	# PMICSwEnableerror
	__metaclass__ = Parser
	id = 0x229
	def parse_v1(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV SW EN ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV SW EN ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV SW EN ERROR -- Resource%d Spmi Bus Error!' % data[0]
		elif data[1] == 0x72:
			return 'PMIC DRV SW EN ERROR -- Resource%d Invalid Pointer!' % data[0]
	def parse_v2(self, data):
		return {
			0: ('\tinternalResourceIndex out of range - '
			    'skipping writing mode setting to register'),
			1: ('\tmode is an invalid command - skipping '
			    'writing mode setting to register'),
			2: ('\tWriting value 0x00 to sw_en register failed '),
			3: ('\tWriting value 0x80 to sw_en register failed')
			}[data[0]]
	def parse(self, data):
		if get_branch('major') == 1:
			return self.parse_v1(data)
		elif get_branch('major') == 2:
			return self.parse_v2(data)

class PMIC_MSG_x22A:
	# PMICDrvPinCtrlErr
	# PMICVoltLevelSetting
	__metaclass__ = Parser
	id = 0x22A
	def parse_v1(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV PIN CTRL ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV PIN CTRL ERROR -- Resource%d Spmi Bus Error!' % data[0]
	def parse_v2(self, data):
		return '\tWrote value to voltage level register: 0x%02x' %\
			data[0]
	def parse(self, data):
		if get_branch('major') == 1:
			return self.parse_v1(data)
		elif get_branch('major') == 2:
			return self.parse_v2(data)

class PMIC_MSG_x22B:
	# PMICDrvPinCtrlModeErr
	# PMICVoltRangeSetting
	__metaclass__ = Parser
	id = 0x22B
	def parse_v1(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV PIN CTRL MODE ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV PIN CTRL MODE ERROR -- Resource%d Spmi Bus Error!' % data[0]
	def parse_v2(self, data):
		return '\tWrote value to voltage range register: 0x%02x' %\
			data[0]
	def parse(self, data):
		if get_branch('major') == 1:
			return self.parse_v1(data)
		elif get_branch('major') == 2:
			return self.parse_v2(data)

class PMIC_MSG_x22C:
	# PMICDrvVoltLevelErr
	# PMICVoltLevelError
	__metaclass__ = Parser
	id = 0x22C
	def parse_v1(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV VOLT LVL ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV VOLT LVL ERROR -- Resource%d Spmi Bus Error!' % data[0]
		elif data[1] == 0x72:
			return 'PMIC DRV VOLT LVL ERROR -- Resource%d Invalid Pointer!' % data[0]
		elif data[1] == 0x14:
			return 'PMIC DRV VOLT LVL ERROR -- Resource%d Unsupported Voltage!' % data[0]
	def parse_v2(self, data):
		return {	
			0: ("\tFeature not supported - skipping writing "
			    "range and level setting to register"),
			1: "\tWriting range setting failed - skipping "
			    "writing level setting to register",
			2: ("\tWriting level setting to register was "
			    "unsuccessful"),
			3: ("\tVoltage level out of range - skipping "
			    "writing range and level setting to register")
				} [data[0]]
	def parse(self, data):
		if get_branch('major') == 1:
			return self.parse_v1(data)
		elif get_branch('major') == 2:
			return self.parse_v2(data)

class PMIC_MSG_x22D:
	# PMICDrvSmpsClkSrc
	# PMICPwrModeSetting
	__metaclass__ = Parser
	id = 0x22D
	def parse_v1(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV SMPS CLK SRC ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV SMPS CLK SRC ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV SMPS CLK SRC ERROR -- Resource%d Spmi Bus Error!' % data[0]
	def parse_v2(self, data):
		return '\tWrote value to power mode register: 0x%02x' %\
			data[0]
	def parse(self, data):
		if get_branch('major') == 1:
			return self.parse_v1(data)
		elif get_branch('major') == 2:
			return self.parse_v2(data)

class PMIC_MSG_x22E:
	# PMICDrvPwrModeErr
	# PMICPwrModeError
	__metaclass__ = Parser
	id = 0x22E
	def parse_v1(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV PWR MODE ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV PWR MODE ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV PWR MODE ERROR -- Resource%d Spmi Bus Error!' % data[0]
		elif data[1] == 0x72:
			return 'PMIC DRV PWR MODE ERROR -- Resource%d Invalid Pointer!' % data[0]
	def parse_v2(self, data):
		return {
			0: ("\tinternalResourceIndex out of range - "
			    "skipping writing power mode to register"),
			1: ("\tSoftware mode is invalid - skipping "
			    "writing power mode to register"),
			2: ("\tWriting power mode to register was "
			    "unsuccessful")
				} [data[0]]
	def parse(self, data):
		if get_branch('major') == 1:
			return self.parse_v1(data)
		elif get_branch('major') == 2:
			return self.parse_v2(data)

class PMICBoostStartApply:
	__metaclass__ = Parser
	id = 0x22f
	def parse(self, data):
		if data[1] == 0:
			return 'START Apply() BOOST%dA' % data[0]
		else:
			return 'START Apply() BOOST%dB' % data[0]

class PMICBoostStartPreDep:
	__metaclass__ = Parser
	id = 0x230
	def parse(self, data):
		if data[1] == 0:
			return 'START Pre-Dep() BOOST%dA' % data[0]
		else:
			return 'START Pre-Dep() BOOST%dB' % data[0]

class PMICBoostEndPreDep:
	__metaclass__ = Parser
	id = 0x231
	def parse(self, data):
		if data[1] == 0:
			return 'END Pre-Dep() BOOST%dA' % data[0]
		else:
			return 'END Pre-Dep() BOOST%dB' % data[0]

class PMICBoostStartPostDep:
	__metaclass__ = Parser
	id = 0x232
	def parse(self, data):
		if data[1] == 0:
			return 'START Post-Dep() BOOST%dA' % data[0]
		else:
			return 'START Post-Dep() BOOST%dB' % data[0]

class PMICBoostEndPostDep:
	__metaclass__ = Parser
	id = 0x233
	def parse(self, data):
		if data[1] == 0:
			return 'END Post-Dep() BOOST%dA' % data[0]
		else:
			return 'END Post-Dep() BOOST%dB' % data[0]

class PMICBoostAggregation1:
	__metaclass__ = Parser
	id = 0x234
	def parse(self, data):
		return "\tsw_en = %d\n" %	data[0]

class PMICBoostNoChange:
	__metaclass__ = Parser
	id = 0x235
	def parse(self, data):
		if data[1] == 0:
			return 'NO CHANGES DETECTED BOOST%dA' % data[0]
		else:
			return 'NO CHANGES DETECTED BOOST%dB' % data[0]

class PMICRailway:
	__metaclass__ = Parser
	id = 0x236
	def parse(self, data):
		if data[1] == 1:
			if data[0] == 0:      
				return 'PMIC RAILWAY -- MX Settling Timeout!'
			elif data[0] == 1: 
				return 'PMIC RAILWAY -- CX Settling Timeout!'      
			elif data[0] == 2: 
				return 'PMIC RAILWAY -- GFX Settling Timeout!'      
		elif data[1] == 2:
			return  'PMIC RAILWAY -- Invalid Pointer'    
		else:
			return 'PMIC RAILWAY -- Success'   

class PMICRpmError:
	__metaclass__ = Parser
	id = 0x237
	def parse(self, data):
		if get_branch('major') == 1:
			if get_branch('minor') <= 2:
				if data[1] == 1:
					if data[0] == 1:      
						return 'PMIC RPM ERROR -- Null Clk Call Back Data!'
					elif data[0] == 2: 
						return 'PMIC RPM ERROR -- Null Ldo Call Back Data!'      
					elif data[0] == 3: 
						return 'PMIC RPM ERROR -- Null Smps Call Back Data!'      
					elif data[0] == 4:
						return 'PMIC RPM ERROR -- Null Vs Call Back Data!'    
					elif data[0] == 5:
						return 'PMIC RPM ERROR -- Null Boost Call Back Data!'  
				elif data[1] == 2:  
					if data[0] == 2: 
						return 'PMIC RPM ERROR -- Ldo Settling Timeout!'      
					elif data[0] == 3: 
						return 'PMIC RPM ERROR -- Smps Settling Timeout!'      
					elif data[0] == 4:
						return 'PMIC RPM ERROR -- Vs Settling Timeout!'    
					elif data[0] == 5:
						return 'PMIC RPM ERROR -- Boost Settling Timeout!'
			elif get_branch('minor') >= 3:
				if data[0] == 1: 
					return 'PMIC RPM ERROR -- Null Call Back Data!'
				elif data[0] == 2: 
					return 'PMIC RPM ERROR -- Settling Timeout!'   
				elif data[0] == 3: 
					return 'PMIC RPM ERROR -- NULL Pointer!' 

class PMICDrvSmpsIlimErr:
	__metaclass__ = Parser
	id = 0x238
	def parse(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV SMPS ILIM ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV SMPS ILIM ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV SMPS ILIM ERROR -- Resource%d Spmi Bus Error!' % data[0]
		elif data[1] == 0x72:
			return 'PMIC DRV SMPS ILIM ERROR -- Resource%d Invalid Pointer!' % data[0]

class PMICDrvSmpsQmErr:
	__metaclass__ = Parser
	id = 0x239
	def parse(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV SMPS QM ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV SMPS QM ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV SMPS QM ERROR -- Resource%d Spmi Bus Error!' % data[0]
		elif data[1] == 0x14:
			return 'PMIC DRV SMPS QM ERROR -- Resource%d Unsupported Voltage!' % data[0]

class PMICDrvSmpsSwFreqErr:
	__metaclass__ = Parser
	id = 0x23A
	def parse(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV SMPS SWITCH FREQ ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV SMPS SWITCH FREQ ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV SMPS SWITCH FREQ ERROR -- Resource%d Spmi Bus Error!' % data[0]
		elif data[1] == 0x72:
			return 'PMIC DRV SMPS SWITCH FREQ ERROR -- Resource%d Invalid Pointer!' % data[0]

class PMICDrvClkErr:
	__metaclass__ = Parser
	id = 0x23B
	def parse(self, data):
		if data[1] == 0x3B:
			return 'PMIC DRV CLK ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV CLK ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV CLK ERROR -- Resource%d Spmi Bus Error!' % data[0]
		elif data[1] == 0x72:
			return 'PMIC DRV CLK ERROR -- Resource%d Invalid Pointer!' % data[0]

class PMICLdoStartSettling:
	__metaclass__ = Parser
	id = 0x23C
	def parse(self, data):
		if data[1] == 0:
			return ("START Settling()\n"
				"\t\tLDO%dA setting_time %d \n") %\
				(data[0], data[2])
		else:
			return ("START Settling()\n"
				"\t\tLDO%dB setting_time %d \n") %\
				(data[0], data[2])

class PMICLdoEndSettling:
	__metaclass__ = Parser
	id = 0x23D
	def parse(self, data):
		if data[1] == 0:
			return 'END Settling() LDO%dA' % data[0]
		else:
			return 'END Settling() LDO%dB' % data[0]

class PMICSmpsStartSettling:
	__metaclass__ = Parser
	id = 0x23E
	def parse(self, data):
		if data[1] == 0:
			return ("START Settling()\n"
				"\t\tSMPS%dA setting_time %d \n") %\
				(data[0], data[2])
		else:
			return ("START Settling()\n"
				"\t\tSMPS%dB setting_time %d \n") %\
				(data[0], data[2])

class PMICSmpsEndSettling:
	__metaclass__ = Parser
	id = 0x23F
	def parse(self, data):
		if data[1] == 0:
			return 'END Settling() SMPS%dA' % data[0]
		else:
			return 'END Settling() SMPS%dB' % data[0]

class PMICVsStartSettling:
	__metaclass__ = Parser
	id = 0x240
	def parse(self, data):
		if data[1] == 0:
			return ("START Settling()\n"
				"\t\tVS%dA setting_time %d \n") %\
				(data[0], data[2])
		else:
			return ("START Settling()\n"
				"\t\tVS%dB setting_time %d \n") %\
				(data[0], data[2])

class PMICVsEndSettling:
	__metaclass__ = Parser
	id = 0x241
	def parse(self, data):
		if data[1] == 0:
			return 'END Settling() VS%dA' % data[0]
		else:
			return 'END Settling() VS%dB' % data[0]

class PMICBoostStartSettling:
	__metaclass__ = Parser
	id = 0x242
	def parse(self, data):
		if data[1] == 0:
			return ("START Settling()\n"
				"\t\tBOOST%dA setting_time %d \n") %\
				(data[0], data[2])
		else:
			return ("START Settling()\n"
				"\t\tBOOST%dB setting_time %d \n") %\
				(data[0], data[2])     
        
class PMICBoostEndSettling:
	__metaclass__ = Parser
	id = 0x243
	def parse(self, data):
		if data[1] == 0:
			return 'END Settling() BOOST%dA' % data[0]
		else:
			return 'END Settling() BOOST%dB' % data[0]             
            
class PMICDrvSmpsPhaseErr:
	__metaclass__ = Parser
	id = 0x244
	def parse(self, data):
		if data[1] == 0x2F:
			return 'PMIC DRV SMPS PHASE ERROR -- Resource%d Invalid Resource Index!' % data[0]
		elif data[1] == 0x24:
			return 'PMIC DRV SMPS PHASE ERROR -- Resource%d Invalid Cmd!' % data[0]
		elif data[1] == 0x09:
			return 'PMIC DRV SMPS PHASE ERROR -- Resource%d Spmi Bus Error!' % data[0]
		elif data[1] == 0x72:
			return 'PMIC DRV SMPS PHASE ERROR -- Resource%d Invalid Pointer!' % data[0]

class PMICSettlingTime:
	__metaclass__ = Parser
	id = 0x245
	def parse(self, data):
		return 'PMIC Regulator Settling: expected settling time %d uS, actual settling time %d uS' % (data[0], data[1])
