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

# Signatures of the files that this depends on
# e9daea6efb23d6d128aa41b797d8d374 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/Input/cust_config_template.c
# 97390e756c9580d51948f80785760891 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/Input/default_build_config.def
# 62a6e8efe69bbd281dfccbb53b479ee1 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/Input/static_build_config.def
# fb029620d16a7ed5f97289883f996027 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/Input/qurt_tlb_unlock.xml
# 206b9e12f5643aab9b5c41da144c95f3 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/__init__.py
# 992309777e70b115ee88d151ceea0f4d /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/build_qurt_config.py
# 9dbe95fb17059e23e02557c064bf4bee /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/build_xml.py
# 85c5b7ff7ae8a1f0340321ae61bf0367 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/ezxml.py
# 476321ba022da3266d0982e4e716cad1 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/genheader.py
# c083132b08dbc9fe4f0e2ad37e4cab81 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/interrupt_xml.py
# edd684f63b9092e3f6bc79d66fe58ec2 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/kernel_xml.py
# a5d2007355824d86b008938e8e710b0d /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/machine_xml.py
# 3ecbf3d77adf335100794a93a5b4ce91 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/merge.py
# 4ffa9ed630bd335beb3517b8494d7c1b /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/parse_build_params.py
# 7ce8928c359e18db33c8cd5dd370b30f /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/parse_spec.py
# 31733f66f57d2783f3b7e99338fdb9ae /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/physpool_xml.py
# b9f6f32884a32971e856ae24cd47ad5b /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/program_xml.py
# 8dc1fab9af2d318d04c7fd08524ca7b3 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/lib/qurt.py
# 2e0bf6952aab826a3950694c911335ab /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/Input/build_params.txt
# 3fe800c0b514571956b8024beb0a032b /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/Input/cust_config.c
# 59c18a4e157a7be8eabfe1aa4ac92292 /local/mnt/workspace/CRMBuilds/MPSS.JO.2.0.c1.4-00005-8909_GENS_PACK-1_20170119_051958/b/modem_proc/core/kernel/qurt/build/mapss_b/qdsp6/8909.gpss.prod/install/modemv56_l2tags_rs/scripts/qurt-image-build.py
