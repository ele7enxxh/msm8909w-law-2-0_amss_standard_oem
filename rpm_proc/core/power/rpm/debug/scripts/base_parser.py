
#
# Parser self-registration boilerplate
#

parsers = {}

class Parser(type):
    def __new__(cls, name, bases, attrs):
        if 'id' not in attrs:
            raise 'Must provide the ID of the log message this parser handles.'
        return super(Parser, cls).__new__(cls, name, bases, attrs)

    def __init__(self, name, bases, attrs):
        #import pdb; pdb.set_trace()
        super(Parser, self).__init__(name, bases, attrs)
        parsers[attrs['id']] = self

