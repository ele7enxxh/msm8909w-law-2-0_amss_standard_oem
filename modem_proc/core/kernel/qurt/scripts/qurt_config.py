#!/usr/bin/env python

class qurt_config:
    def genheader_subcommand(self, arglist):
        from lib.genheader import genheader_cmd
        return genheader_cmd(arglist)
    def update_subcommand(self, arglist):
        from lib.merge import merge_cmd
        return merge_cmd(arglist)
    def usage(self):
        cmds = sorted([z.rsplit('_',1)[0] for z in dir(self) if z.endswith('_subcommand')])
        str = 'First argument must be one of:\n  ' + ', '.join(cmds)
        raise Exception(str)
    def run_command(self, argv):
        from traceback import format_exc as tbstr
        progname = argv[0]
        try:
            print ' '.join(argv)
            raw_args = argv[1:]
            args = [s for s in raw_args if not s == '--traceback']
            if args == raw_args:
                tbstr = None
            try:
                subfunc = getattr(self, '%s_subcommand' % args[0])
            except StandardError:
                self.usage()
            return subfunc(args[1:])
        except (SystemExit, KeyboardInterrupt):
            raise
        except Exception, err:
            if tbstr:
                print tbstr()
            print '%s: Error:\n*** %s' % (progname, err)
        except:
            raise
        return 1
    def main(self):
        import sys
        sys.exit(self.run_command(sys.argv))

qurt_config().main()    # Never returns
