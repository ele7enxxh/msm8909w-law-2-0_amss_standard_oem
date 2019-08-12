#  ===========================================================================
#
#  Copyright (c) 2011 Qualcomm Technologies Incorporated.
#  All Rights Reserved.
#  QUALCOMM Proprietary and Confidential.
#
#  ===========================================================================


from __future__ import print_function

import logging
logger = logging.getLogger(__name__)

import os
import itertools
from binascii import hexlify
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object, Structure
from hansei_utils import *

ee_names_family = {
    'badger' : {
        0 : 'APPS',
        1 : 'MSS',
        2 : 'LPASS',
        3 : 'WCSS',
        4 : 'TZ',
    },
    'honey' : {
        0 : 'APPS',
        1 : 'MSS',
        2 : 'ADSP',
        3 : 'SENSORS',
        4 : 'TZ',
    },
    'bear' : {
        0 : 'APPS',
        1 : 'MSS',
        2 : 'WCSS',
        3 : 'TZ',
        4 : 'LPASS',
    }
}

ee_names = {}

def dump(dump_path, memory, debug_info, target_family):
    global ee_names
    ee_names = ee_names_family[target_family]
    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['rpm_core_dump'].vartype
    rpm_core_dump = decode_object('rpm_core_dump', address, dump_type, memory, debug_info)

    #import pdb; pdb.set_trace()
    rpm = cast(rpm_core_dump.rpmserver_state, 'SystemData', memory, debug_info)
    '''
    rpm_type = rpm_core_dump._find_basic_type(debug_info.variables['rpm'].die)
    rpm_type = rpm_core_dump._unwrap(rpm_type) # "dereference" the type--rpm_core_dump already points at the state itself
    rpm_type = rpm_core_dump._find_basic_type(rpm_type)
    rpm_state = rpm_core_dump.rpmserver_state.address
    rpm = decode_object('rpm', rpm_state, None, memory, debug_info, die=rpm_type)
    '''

    #save_logger_level = logging.getLogger('dwarf').getEffectiveLevel()
    #logging.getLogger('dwarf').setLevel(logging.DEBUG)
    dump_ee_status(dump_path, rpm, memory, debug_info)
    #logging.getLogger('dwarf').setLevel(save_logger_level)
    dump_ee_requests(dump_path, rpm, memory, debug_info)
    dump_resource_requests(dump_path, rpm, memory, debug_info)
    dump_lookup_table(dump_path, rpm, memory, debug_info)
    dump_estimate_cache(dump_path, rpm, memory, debug_info)


def dump_ee_status(dump_path, rpm, memory, debug_info):
    ee_status_file_name = os.path.join(dump_path, 'ee-status.txt')
    with open(ee_status_file_name, 'w') as ee_status_file:

        for ee in range(rpm.num_ees):
            ee_data = rpm.ees[ee]

            subsystem_status = ee_data.subsystem_status
            num_active_cores = ee_data.num_active_cores
            pending_bringups = ee_data.pending_bringups

            print('\n\n *** %s *** %d' % (ee_names[ee], ee), file=ee_status_file)
            print('\tstatus:           %s' % subsystem_status, file=ee_status_file)
            print('\tnum_active_cores: %d' % num_active_cores, file=ee_status_file)
            print('\tpending_bringups: 0x%0.8x' % pending_bringups, file=ee_status_file)
            print('\tMaster Stats:', file=ee_status_file)
            try:
                if ee_data.stats.address != 0 :
                    num_shutdowns    = ee_data.stats.num_shutdowns
                    shutdown_req     = ee_data.stats.shutdown_req
                    wakeup_ind       = ee_data.stats.wakeup_ind
                    bringup_req      = ee_data.stats.bringup_req
                    bringup_ack      = ee_data.stats.bringup_ack
                    wakeup_reason    = "scheduled" if ee_data.stats.wakeup_reason else "rude"
                    sleep_duration   = ee_data.stats.last_sleep_transition_duration
                    wake_duration    = ee_data.stats.last_wake_transition_duration
                    print('\t\tnum_shutdowns:          %d' % num_shutdowns, file=ee_status_file)
                    print('\t\tshutdown_req:           %s' % hex(shutdown_req), file=ee_status_file)
                    print('\t\twakeup_ind:             %s' % hex(wakeup_ind), file=ee_status_file)
                    print('\t\tbringup_req:            %s' % hex(bringup_req), file=ee_status_file)
                    print('\t\tbringup_ack:            %s' % hex(bringup_ack), file=ee_status_file)
                    print('\t\twakeup_reason:          %s' % wakeup_reason, file=ee_status_file)
                    print('\t\tlast_sleep_transition_duration: %s' % hex(sleep_duration), file=ee_status_file)
                    print('\t\tlast_wake_transition_duration:  %s' % hex(wake_duration), file=ee_status_file)
                else :
                    print('\t\tERROR ee.stats is null', file=ee_status_file)

            except:
                print('\t\tERROR READING ee.stats', file=ee_status_file)

            print('\tCHANGER:', file=ee_status_file)
            #import pdb; pdb.set_trace()
            try:
                if ee_data.changer.address != 0:
                    # cast changer here to pick up more detailed typedef
                    changer = cast(ee_data.changer, 'SetChanger', memory, debug_info)
                    changer_task = cast(changer, 'Task', memory, debug_info)
                    handler = changer.handler_
                else :
                    handler = cast(ee_data.handler, 'Handler', memory, debug_info)
                    changer_task = None
            except:
                print('\t\tFailed to cast changer', file=ee_status_file)
            try:
                smdl_port = cast(handler.smdlPort_, 'smdl_handle_type', memory, debug_info)
                # This is a hack because apparently smdl_handle_type is only a pointer type...
                smdl_port.address = handler.smdlPort_.address
                smdl_port.pointer = handler.smdlPort_.address
                tx = smdl_port.tx_shared_info_ptr_algn
                rx = smdl_port.rx_shared_info_ptr_algn
            except:
                print('\t\tFailed to parse smdlPort', file=ee_status_file)
                #return

            try:
                deadline         = changer_task.deadline_
                start_           = changer_task.start_
                end_             = changer_task.end_
                haveDeadline     = changer_task.haveDeadline_
                vioDeadlines     = changer_task.violatedDeadlines_
                currentSet       = changer.currentSet_
                destinationSet   = changer.destinationSet_

                print('\t\tTask::haveDeadline:     %s' % haveDeadline, file=ee_status_file)
                print('\t\tTask::deadline:         %s' % hex(deadline), file=ee_status_file)
                print('\t\tTask::start:            %s' % hex(start_), file=ee_status_file)
                print('\t\tTask::end:              %s' % hex(end_), file=ee_status_file)
                print('\t\tTask::violatedDeadlines:%s' % hex(vioDeadlines), file=ee_status_file)
                print('\t\tcurrentSet:             %s' % currentSet, file=ee_status_file)
                print('\t\tdestinationSet:         %s' % destinationSet, file=ee_status_file)
            except:
                print('\t\tERROR READING [ee].changer', file=ee_status_file)
            try:
                #handler
                print('\t\tHANDLER:', file=ee_status_file)
                #SMDL
                print('\t\t\tSMDL info:', file=ee_status_file)
                print('\t\t\t\tport_id:                %d' % smdl_port.port_id, file=ee_status_file)
                print('\t\t\t\tchannel_type:           %s' % smdl_port.channel_type, file=ee_status_file)
                #TX (rpm->master)
                print('\t\t\t\tFIFO TX:', file=ee_status_file)
                print('\t\t\t\t\tread_index:             %d' % tx.read_index, file=ee_status_file)
                print('\t\t\t\t\twrite_index:            %d' % tx.write_index, file=ee_status_file)
                #RX (master->rpm)
                print('\t\t\t\tFIFO RX:', file=ee_status_file)
                print('\t\t\t\t\tread_index:             %d' % rx.read_index, file=ee_status_file)
                print('\t\t\t\t\twrite_index:            %d' % rx.write_index, file=ee_status_file)

            except:
                print('\t\tERROR READING [ee].handler', file=ee_status_file)


def dump_struct(address, memory, debug_info, struct_type, raw_size):
    try:
        obj = decode_object('request', address, None, memory, debug_info, die=debug_info.types[struct_type])
    except Exception as e:
        logger.info('Failed to parse data via magic structure parsing of %s -> (%s); falling back to dump mode.' % (struct_type, e))
        return hexlify(memory.read(address, raw_size))

    if type(obj) != Structure:
        return str(obj)

    out_lines = ['\n\t{']
    for member_name, member_value in obj.members():
        out_lines.append('\t\t%s : %s,' % (member_name, member_value))
    out_lines.append('\t}')
    return '\n'.join(out_lines)

def format_request(address, rcd, memory, debug_info):
    xlate = rcd.xlate.address & 0xFFFFFFFE

    xlate_funcs_to_req_type = {
        'rpm_npa_xlate'                 : 'npa_request_data_t',
        'railway_xlate'                 : 'railway_voter_irep',
        'pm_rpm_smps_translation'       : 'pm_npa_smps_int_rep',
        'pm_rpm_ldo_translation'        : 'pm_npa_ldo_int_rep',
        'pm_rpm_vs_translation'         : 'pm_npa_vs_int_rep',
        'pm_rpm_pbs_translation'        : 'pm_npa_pbs_int_rep',
        'pm_rpm_clk_buffer_translation' : 'pm_npa_clk_buffer_int_rep',
        'icb_masters_translate_cb'      : 'icb_master_bw_req_type',
        'icb_slaves_translate_cb'       : 'icb_slave_req_type',
    }

    # Try to find an exact structure dumper...
    for func, req_type in xlate_funcs_to_req_type.iteritems():
        if xlate == debug_info.functions.get(func):
            return dump_struct(address, memory, debug_info, req_type, rcd.inrep_size)

    # Give up and dump just the raw binary.
    return hexlify(memory.read(address, rcd.inrep_size))

def dump_ee_requests(dump_path, rpm, memory, debug_info):
    dump_path = os.path.join(dump_path, 'reqs_by_master')
    if not os.path.exists(dump_path):
        os.makedirs(dump_path)

    est_address = debug_info.variables['rpm_estimator'].address
    est_type = debug_info.variables['rpm_estimator'].vartype
    rpm_estimator = decode_object('rpm_estimator', est_address, est_type, memory, debug_info)
    cpu_kHz = rpm_estimator.clockFreqkHz_;

    for ee in range(rpm.num_ees):
        ee_reqs_file_name = os.path.join(dump_path, ee_names[ee] + '.txt')
        with open(ee_reqs_file_name, 'w') as ee_reqs_file:
            ee_reqs = []

            print('\n\tcpu_kHz : [%d, %d]' % (cpu_kHz[0], cpu_kHz[1]), file=ee_reqs_file)

            for idx in range(rpm.supported_resources):
                resource = rpm.resources[idx]
                if resource.current_state.address == 0:
                    logger.debug('[dump_ee_requests]: Skipping unused resource idx %d' % idx)
                    continue

                rcd = rpm.classes[resource.class_idx]

                #import pdb; pdb.set_trace()
                if (resource.ees.address == 0):
                    logger.debug('[dump_ee_requests]: Skipping NULL resource idx %d for ee %d' % (idx, ee))
                    continue

                if (True == check_member('ee_id',resource.ees)):
                    #New memory saving EEClient structure
                    ee_client = resource.ees
                    while(ee_client.ee_id != ee):
                        if ee_client.next.address == 0:
                            ee_reqs.append({
                                'rcd'              : rcd,
                                'type'             : rcd.type,
                                'id'               : resource.id,
                                'estimates'        : resource.estimates,
                                'selected_type'    : 'NO_REQUEST',
                                'selected_request' : 'n/a',
                                'spare_type'       : 'NO_REQUEST',
                                'spare_request'    : 'n/a',
                                'message'          : '{not allocated}',
                            })
                            break;
                        ee_client = ee_client.next
                    if (ee_client.ee_id != ee and ee_client.next.address == 0):
                        continue;
                else:
                    #Old EEClient structure
                    ee_client = resource.ees[ee]

                selected = ee_client.selected
                selected_type = ee_client.req_type[selected]
                selected_request = ee_client.request[selected].address

                spare = 1 ^ selected
                spare_type = ee_client.req_type[spare]
                spare_request = ee_client.request[spare].address

                ee_reqs.append({
                    'rcd'              : rcd,
                    'type'             : rcd.type,
                    'id'               : resource.id,
                    'estimates'        : resource.estimates,
                    'selected_type'    : selected_type,
                    'selected_request' : selected_request,
                    'spare_type'       : spare_type,
                    'spare_request'    : spare_request,
                    'message'          : '',
                })

            for request in sorted(ee_reqs, key = lambda req: (req['type'],req['id'])):
                rcd   = request['rcd']
                rtype = type_to_str(request['type'])
                rid   = request['id']
                msg   = request['message']
                ests  = request['estimates']
                print('\n*** %s %d ***' % (rtype, rid), file=ee_reqs_file)

                if (request['selected_type'] == 'NO_REQUEST') and (request['spare_type'] == 'NO_REQUEST'):
                    continue

                print('\testimates: [%d, %d]' % (ests[0], ests[1]), file=ee_reqs_file)

                selected_request = 'n/a' if request['selected_type'] == 'NO_REQUEST' else format_request(request['selected_request'], rcd, memory, debug_info)
                print('\n\tselected request [%s] -> %s %s' % (request['selected_type'], selected_request, msg), file=ee_reqs_file)

                spare_request = 'n/a' if request['spare_type'] == 'NO_REQUEST' else format_request(request['spare_request'], rcd, memory, debug_info)
                print('\n\tspare request [%s] -> %s %s\n' % (request['spare_type'], spare_request, msg), file=ee_reqs_file)

def dump_resource_requests(dump_path, rpm, memory, debug_info):
    dump_path = os.path.join(dump_path, 'reqs_by_resource')

    est_address = debug_info.variables['rpm_estimator'].address
    est_type = debug_info.variables['rpm_estimator'].vartype
    rpm_estimator = decode_object('rpm_estimator', est_address, est_type, memory, debug_info)
    cpu_kHz = rpm_estimator.clockFreqkHz_;

    resources = []
    for idx in range(rpm.supported_resources):
        resource = rpm.resources[idx]
        if resource.current_state.address == 0:
            logger.debug('[dump_resource_requests]: Skipping unused resource idx %d' % idx)
            continue

        rcd = rpm.classes[resource.class_idx]
        rtype = type_to_str(rcd.type)

        rdir = os.path.join(dump_path, rtype)
        if not os.path.exists(rdir):
            os.makedirs(rdir)
        rpath = os.path.join(rdir, '%d.txt' % resource.id)

        with open(rpath, 'w') as f:
            status = format_request(resource.current_state.address, rcd, memory, debug_info)
            print('\nStatus -> %s\n' % status, file=f)

            print('cpu_kHz ->\n\t[%d, %d]\n' % (cpu_kHz[0], cpu_kHz[1]), file=f)
            print('estimates ->\n\t[%d, %d]\n' % (resource.estimates[0], resource.estimates[1]), file=f)
            print('settling estimate ->\n\t%d\n' % (rcd.settling_estimate), file=f)

            print('-=| External Requests (from other processors) |=-', file=f)
            for ee in range(rpm.num_ees):
                if (resource.ees.address == 0):
                    logger.debug('[dump_resource_requests]: Skipping NULL resource idx %d for ee %d' % (idx, ee))
                    continue
                if (True == check_member('ee_id',resource.ees)):
                    #New memory saving EEClient structure
                    ee_client = resource.ees
                    while(ee_client.ee_id != ee):
                        if ee_client.next.address == 0:
                            print('\n*** %s ***' % ee_names[ee], file=f)
                            print('\tselected request [%s] -> %s {not allocated}' % ('NO_REQUEST', 'n/a'), file=f)
                            print('\n\tspare request [%s] -> %s {not allocated}\n' % ('NO_REQUEST', 'n/a'), file=f)
                            break;
                        ee_client = ee_client.next
                    if (ee_client.ee_id != ee and ee_client.next.address == 0):
                        continue;
                else:
                    #Old EEClient structure
                    ee_client = resource.ees[ee]

                print('\n*** %s ***' % ee_names[ee], file=f)

                selected = ee_client.selected
                selected_type = ee_client.req_type[selected]
                selected_request = ee_client.request[selected].address
                selected_request = 'n/a' if selected_type == 'NO_REQUEST' else format_request(selected_request, rcd, memory, debug_info)
                print('\tselected request [%s] -> %s' % (selected_type, selected_request), file=f)

                spare = 1 ^ selected
                spare_type = ee_client.req_type[spare]
                spare_request = ee_client.request[spare].address
                spare_request = 'n/a' if spare_type == 'NO_REQUEST' else format_request(spare_request, rcd, memory, debug_info)
                print('\n\tspare request [%s] -> %s\n' % (spare_type, spare_request), file=f)

            if 0 == resource.num_ics:
                continue

            print('\n\n-=| Internal Requests (from RPM software) |=-\n\n', file=f)
            for ic in range(resource.num_ics):
                client = resource.ics[ic]
                client_type = client.type

                if client_type == 0:
                    client_size = rcd.inrep_size
                else:
                    client_size = None
                    #import pdb; pdb.set_trace()
                    ict = rcd.internal_types
                    while ict.address != 0:
                        if ict.client_type == client_type:
                            client_size = ict.inrep_size
                            break
                        ict = ict.next
                    if client_size is None:
                        print('\tWARNING: broken internal client #%d [0x%0.8x] -- no matching internal client type' % (ic, client.address), file=f)
                        continue

                # We don't have a good way of knowing how internal clients are
                # interpreted, but here's a heuristic that seems to work for
                # now: if the inresp_size and the client_size match, parse like
                # a normal request.
                if rcd.inrep_size == client_size:
                    print('\tClient #%d -> %s' % (ic, format_request(client.request.address, rcd, memory, debug_info)), file=f)
                else:
                    print('\tWARNING: parsing may be incorrect', file=f)
                    print('\traw hex:', file=f)
                    print('\t\tClient #%d -> %s' % (ic, hexlify(memory.read(client.request.address, client_size))), file=f)
                    print('\tAttempted parsing:', file=f)
                    print('\tClient #%d -> %s' % (ic, format_request(client.request.address, rcd, memory, debug_info)), file=f)

allf = 0xFFFFFFFF
c1 = 0xcc9e2d51
c2 = 0x1b873593
def __rol(v, rot):
    return ((v << rot) & allf) | ((v >> (32-rot)) & allf)
def resource_hash(seed, type_int, res_id):
    h1 = seed & allf

    k1 = type_int & allf
    k1 = (k1 * c1) & allf
    k1 = __rol(k1, 15)
    k1 = (k1 * c2) & allf
    h1 = (h1 ^ k1) & allf
    h1 = __rol(h1, 13)
    h1 = (h1 * 5 + 0xe6546b64) & allf

    k1 = res_id & allf
    k1 = (k1 * c1) & allf
    k1 = __rol(k1, 15)
    k1 = (k1 * c2) & allf;
    h1 = (h1 ^ k1) & allf
    h1 = __rol(h1, 13)
    h1 = (h1 * 5 + 0xe6546b64) & allf

    h1 = (h1 ^ 8)          & allf
    h1 = (h1 ^ (h1 >> 16)) & allf
    h1 = (h1 * 0x85ebca6b) & allf
    h1 = (h1 ^ (h1 >> 13)) & allf
    h1 = (h1 * 0xc2b2ae35) & allf
    h1 = (h1 ^ (h1 >> 16)) & allf

    return h1

def is_correct_resource(rpm, lookup_idx, type_int, res_id):
    r = rpm.resources[lookup_idx]
    return (r.id == res_id and rpm.classes[r.class_idx].type == type_int)
def rpmserver_lookup_resource(rpm, type_int, res_id):
    resource_slots = rpm.supported_resources
    first_idx = resource_hash(0, type_int, res_id) % resource_slots;
    first_val = rpm.resource_seeds[first_idx]

    # If the first level table contains 0, we've never inserted anything at
    # this idx before.  The lookup has definitely failed and there is no such
    # resource.
    if first_val == 0:
        return (False, 'FAILURE: missed at the first level!')

    # If the first level table contains a negative number, then we have a
    # single resource that hashed to this idx in the past.  It is stored
    # directly at (-first_val - 1).  We need to check if that resource
    # matches, and if so we've found the right one.
    if first_val < 0:
        lookup_idx = -first_val - 1;
        description = 'direct-mapped to %d' % lookup_idx
    else:
        lookup_idx = resource_hash(first_val, type_int, res_id) % resource_slots;
        description = 'first level %d -> second level %d' % (first_val, lookup_idx)

    # If everything matches, we found it.  Otherwise, it must not exist.
    if is_correct_resource(rpm, lookup_idx, type_int, res_id):
        return (True, description)
    else:
        return (False, 'FAILURE: no match after ' + description)

def dump_lookup_table(dump_path, rpm, memory, debug_info):
    lookups = []
    for idx in range(rpm.supported_resources):
        resource = rpm.resources[idx]

        # skip unused indices
        if resource.current_state.address == 0:
            lookups.append((True, '<unused>'))
            continue

        rcd = rpm.classes[resource.class_idx]
        type_int = rcd.type
        res_id   = resource.id
        lookups.append(rpmserver_lookup_resource(rpm, type_int, res_id))

    with open(os.path.join(dump_path, 'lookup_table.txt'), 'w') as f:
        if all(result[0] for result in lookups):
            print('\n*** lookup hash table ok; all resources accessible ***\n\n', file=f)
        else:
            print('\n*** WARNING *** ERROR *** INCONSISTENCIES EXIST IN THE LOOKUP HASH TABLE *** ERROR *** WARNING ***\n\n', file=f)

        result_dump = '\n'.join('[%d] %s -> %s' % (i, 'ok' if result[0] else 'FAILED', result[1]) for i, result in enumerate(lookups))
        print(result_dump, file=f)

def dump_estimate_cache(dump_path, rpm, memory, debug_info):
    est_cache_immediate_address = debug_info.variables['rpm_estimate_cache_immediate'].address
    est_cache_immediate_type = debug_info.variables['rpm_estimate_cache_immediate'].vartype
    rpm_estimate_cache_immediate = decode_object('rpm_estimate_cache_immediate', est_cache_immediate_address, est_cache_immediate_type, memory, debug_info)

    est_cache_address = debug_info.variables['rpm_estimate_cache'].address
    est_cache_type = debug_info.variables['rpm_estimate_cache'].vartype
    rpm_estimate_cache = decode_object('rpm_estimate_cache', est_cache_address, est_cache_type, memory, debug_info)

    with open(os.path.join(dump_path, 'estimate_cache.txt'), 'w') as f:
        print('\n*** rpm_estimate_cache_immediate (Active -> Sleep) ***\n', file=f)
        for idx in range(rpm_estimate_cache_immediate.cacheSize_):
            systemTag   = rpm_estimate_cache_immediate.cache_[idx].systemTag
            actionTag   = rpm_estimate_cache_immediate.cache_[idx].actionTag
            resultState = rpm_estimate_cache_immediate.cache_[idx].resultState
            estimate    = rpm_estimate_cache_immediate.cache_[idx].estimate
            usedSeq     = rpm_estimate_cache_immediate.cache_[idx].usedSeq
            print('\tsystemTag = 0x%0.8x, actionTag = 0x%0.8x, resultState = 0x%0.8x, estimate = %d, usedSeq = %d\n' \
                  % (systemTag, actionTag, resultState, estimate, usedSeq), file=f)

        print('\n*** rpm_estimate_cache (Sleep -> Active) ***\n', file=f)
        for idx in range(rpm_estimate_cache.cacheSize_):
            systemTag   = rpm_estimate_cache.cache_[idx].systemTag
            actionTag   = rpm_estimate_cache.cache_[idx].actionTag
            resultState = rpm_estimate_cache.cache_[idx].resultState
            estimate    = rpm_estimate_cache.cache_[idx].estimate
            usedSeq     = rpm_estimate_cache.cache_[idx].usedSeq
            print('\tsystemTag = 0x%0.8x, actionTag = 0x%0.8x, resultState = 0x%0.8x, estimate = %d, usedSeq = %d\n' \
                  % (systemTag, actionTag, resultState, estimate, usedSeq), file=f)

