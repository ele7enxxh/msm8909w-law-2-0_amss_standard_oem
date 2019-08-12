from __future__ import print_function

import logging
logger = logging.getLogger(__name__)

import os
import itertools
from locators.core_dump import locate as locate_core_dump
from dwarf import decode_object

#
# Similar to the ULog dump scripts, this is copied directly from the rpm_npadump.cmm.
#
def dump(dump_path, memory, debug_info):
    address = locate_core_dump(memory, debug_info)
    dump_type = debug_info.variables['rpm_core_dump'].vartype
    rpm_core_dump = decode_object('rpm_core_dump', address, dump_type, memory, debug_info)

    npa_type = debug_info.variables['npa'].vartype
    npa_state = rpm_core_dump.npa_state.address
    npa = decode_object('npa', npa_state, npa_type, memory, debug_info)

    dump_file = open(os.path.join(dump_path, 'npa-dump.txt'), 'w')

    link = npa.resources
    while link.address != 0:
        dump_link(dump_file, link)
        link = link.next

def dump_link(dump_file, link):
    link_name  = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in link.name)))
    resource   = link.resource
    definition = resource.definition

    if definition.address == 0:
        dump_placeholder(dump_file, link_name)
    else:
        resource_name = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in definition.name)))
        if resource_name == link_name:
            dump_resource(dump_file, resource, resource_name)
        elif resource_name == 'marker resource':
            dump_marker(dump_file, link_name)
        else:
            dump_alias(dump_file, link_name, resource_name)

def dump_placeholder(dump_file, link_name):
    print('npa_placeholder (name: "%s")' % link_name, file=dump_file)

def dump_marker(dump_file, link_name):
    print('npa_marker (name: "%s")' % link_name, file=dump_file)

def dump_alias(dump_file, link_name, resource_name):
    print('npa_alias (alias_name: "%s") (resource_name: "%s")' % (link_name, resource_name), file=dump_file)

def dump_resource(dump_file, resource, resource_name):
    dump_resource_state(dump_file, resource, resource_name)
    dump_resource_clients(dump_file, resource)
    dump_resource_events(dump_file, resource)

    print('        end npa_resource (handle: 0x%x)' % resource.address, file=dump_file)

def dump_resource_state(dump_file, resource, resource_name):
    units = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in resource.definition.units)))
    resource_max    = resource.definition.max
    active_state    = resource.active_state
    active_max      = resource.active_max
    active_headroom = resource.active_headroom
    request_state   = resource.request_state

    dumpstr = 'npa_resource (name: "%s") (handle: 0x%x) (units: %s) (resource max: %d) (active max: %d) (active state: %d) (active headroom: %d) (request state: %d)'
    dumpdat = (resource_name, resource.address, units, resource_max, active_max, active_state, active_headroom, request_state)
    print(dumpstr % dumpdat, file=dump_file)

def dump_resource_clients(dump_file, resource):
    client = resource.clients

    while client.address != 0:
        client_name = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in client.name)))
        client_type = client.type
        request = client.work[client.index].state

        string = '        npa_client (name: %s) (handle: 0x%x) (resource: 0x%x) (type: %s) (request: %d)'
        dat = (client_name, client.address, resource.address, client_type, request)
        print(string % dat, file=dump_file)

        client = client.next

def dump_resource_events(dump_file, resource):
    event = resource.events

    while event.address != 0:
        event_name = ''.join(itertools.takewhile(lambda c: c != '\0', (chr(c) for c in event.name)))
        trigger_type = event.trigger_type

        if trigger_type == 'NPA_TRIGGER_RESERVED_EVENT':
            dumpstr = '        npa_reserved_event (name: "%s") (handle: 0x%x) (resource: 0x%x)'
            dumpdat = (event_name, event.address, resource.address)
        elif trigger_type == 'NPA_TRIGGER_CHANGE_EVENT':
            dumpstr = '        npa_change_event (name: "%s") (handle: 0x%x) (resource: 0x%x)'
            dumpdat = (event_name, event.address, resource.address)
        elif trigger_type == 'NPA_TRIGGER_WATERMARK_EVENT':
            lo = event.lo.watermark
            hi = event.hi.watermark
            dumpstr  = '        npa_watermark_event (name: "%s") (handle: 0x%x) (resource: 0x%x)'
            dumpstr += ' (lo_watermark: %d) (hi_watermark: %d)'
            dumpdat  = (event_name, event.address, resource.address, lo, hi)
        elif trigger_type == 'NPA_TRIGGER_THRESHOLD_EVENT':
            lo = event.lo.threshold
            hi = event.hi.threshold
            dumpstr  = '        npa_threshold_event (name: "%s") (handle: 0x%x) (resource: 0x%x)'
            dumpstr += ' (lo_threshold: %d) (hi_threshold: %d)'
            dumpdat  = (event_name, event.address, resource.address, lo, hi)

        print(dumpstr % dumpdat, file=dump_file)
        event = event.next

