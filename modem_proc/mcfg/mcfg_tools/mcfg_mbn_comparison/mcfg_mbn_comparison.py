import os
import sys

delta_tool_folder_path = os.path.dirname(os.path.abspath(__file__))
mcfg_tools_folder_path = os.path.split(delta_tool_folder_path)[0]
mcfg_folder_path = os.path.split(mcfg_tools_folder_path)[0]
mcfg_test_folder_path = os.path.join(mcfg_folder_path, 'mcfg_test')
mcfg_translate_folder_path = os.path.join(mcfg_test_folder_path, 'mcfg_translate')
sys.path.append(mcfg_translate_folder_path)

from translate_mcfg_parse_mbn import parse_mbn
from translate_mcfg_settings import NV_ITEM_TYPES, EFS_ITEM_TYPES

import argparse
from pprint import pprint

class Item(object):
    def __init__ (self, item_dict):
        self.item_dict = item_dict
        self.type = self.item_dict['item_type']

        if self.type in NV_ITEM_TYPES:
            self.identifier = 'NV #%d' % self.item_dict['nv_number']
        elif self.type in EFS_ITEM_TYPES:
            self.identifier = 'Filename: %s' % self.item_dict['filename']
        else:
            raise NotImplementedError('Item type: %s not yet supported' % item_type)

        self.payload = self.item_dict['payload']

    def __str__ (self):
        return self.identifier

    def __repr__ (self):
        return self.__str__()


class MBN(object):
    def __init__ (self, mbn_path):
        self.parsed = parse_mbn(mbn_path)
        self.nv_items = []
        self.efs_items = []

        for item_dict in self.parsed['items']:
            item = Item(item_dict)
            if item.type in NV_ITEM_TYPES:
                self.nv_items.append(item)
            elif item.type in EFS_ITEM_TYPES:
                self.efs_items.append(item)
            else:
                raise NotImplementedError('Item type: %s not yet supported' % item_type)

        self.items = self.nv_items + self.efs_items

    def __iter__ (self):
        return iter(self.items)

    def nv_number_exists (self, nv_number):
        for item in self.nv_items:
            if item.identifier == nv_number:
                return True

    def file_path_exists (self, file_path):
        for item in self.efs_items:
            if item.identifier == file_path:
                return True

    def item_exists(self, identifier):
        return self.nv_number_exists(identifier) or self.file_path_exists(identifier)


class Delta(object):
    def __init__ (self, mbn_left, mbn_right):
        self.mbn_left = mbn_left
        self.mbn_right = mbn_right

    def get_identical_items (self):
        items = []
        for item_left in self.mbn_left:
            for item_right in self.mbn_right:
                if item_left.identifier == item_right.identifier:
                    if item_left.payload == item_right.payload:
                        items.append(item_left)#

        return items

    def get_mismatched_items (self):
        items = []
        for item_left in self.mbn_left:
            for item_right in self.mbn_right:
                if item_left.identifier == item_right.identifier:
                    if item_left.payload != item_right.payload:
                        item_tuple = (item_left, item_right)
                        items.append(item_tuple)

        return items

    # Items that are only present on the left sided mbn
    def get_left_sided_items (self):
        items = []
        for item_left in self.mbn_left:
            if not self.mbn_right.item_exists(item_left.identifier):
                items.append(item_left)

        return items

    def get_right_sided_items (self):
        items = []
        for item_right in self.mbn_right:
            if not self.mbn_left.item_exists(item_right.identifier):
                items.append(item_right)

        return items




description = '''
MCFG mbn delta tool
-----------------

Compare two mbn's directly.
'''

epilog = '''
Example Usage:

1) python delta.py --left tmo1.mbn --right tmo2.mbn --show_payloads
This lists all items that are identical on both sides, items that appear in only one mbn, and items that appear in both but differ.
The show payloads option displays the contents of each nv/efs item listed.

2) python delta.py --left tmo1.mbn --right tmo2.mbn --show_identical
This lists all items that are identical on left and right hand side

3) python delta.py --left tmo1.mbn --right tmo2.mbn --show_mismatched
This lists all items that exist in both mbns, but differ in content

4) python delta.py --left tmo1.mbn --right tmo2.mbn --show_left_sided
This lists all items that are only present in the left sided mbn
'''


parser = argparse.ArgumentParser(description=description,
                                 formatter_class=argparse.RawTextHelpFormatter,
                                 epilog=epilog)
parser.add_argument('--left',
                    help='mbn to compare "left side"',
                    required=True)
parser.add_argument('--right',
                    help='mbn to compare "right side"',
                    required=True)
parser.add_argument('--output',
                    help='File to output results to')
parser.add_argument('--show_identical', action='store_true',
                    help='Show items that are identical in both mbn\'s')
parser.add_argument('--show_left_sided', action='store_true',
                    help='Show items that are only present on the left mbn')
parser.add_argument('--show_right_sided', action='store_true',
                    help='Show items that are only present on the right mbn')
parser.add_argument('--show_mismatched', action='store_true',
                    help='Show items that are present in both mbn\'s but with' \
                    'differing payloads')
parser.add_argument('--show_payloads', action='store_true',
                    help='Show payloads of items')
parser.add_argument('--show_all', action='store_true',
                    help='Show identical, one sided, and differing items. ' \
                    'This is the default if no other options are selected.')

if __name__ == '__main__':
    args = parser.parse_args()

    mbn_left = MBN(args.left)
    mbn_right = MBN(args.right)

    if not args.show_identical and not args.show_left_sided \
                               and not args.show_right_sided \
                               and not args.show_mismatched:
        args.show_all = True

    delta = Delta(mbn_left, mbn_right)

    if args.show_identical or args.show_all:
        print 'The following items are identical:'
        print '================================='
        identical = delta.get_identical_items()
        if args.show_payloads:
            for item in identical:
                print '%s - %s' % (item, item.payload)
        else:
            pprint(identical)

    if args.show_left_sided or args.show_all:
        statement = '\nThe following items exist only on %s:' % args.left
        print statement
        print '=' * len(statement)
        one_sided = delta.get_left_sided_items()
        if args.show_payloads:
            for item in one_sided:
                print '%s - %s' % (item, item.payload)
        else:
            pprint(one_sided)

    if args.show_right_sided or args.show_all:
        statement = '\nThe following items exist only on %s:' % args.right
        print statement
        print '=' * len(statement)
        one_sided = delta.get_right_sided_items()
        if args.show_payloads:
            for item in one_sided:
                print '%s - %s' % (item, item.payload)
        else:
            pprint(one_sided)

    if args.show_mismatched or args.show_all:
        statement = '\nThe following items have mismatched payloads:'
        print statement
        print '=' * len(statement)
        mismatched = delta.get_mismatched_items()
        if args.show_payloads:
            for left, right in mismatched:
                print left
                print '\tLeft: %s' % left.payload
                print '\tRight: %s' % right.payload
        else:
            pprint(mismatched)
