from SCons.Script import Builder, Dir
import os

COMMON_HEADERS = """#include "comdef.h"
#include "rfc_hwid.h"
#include "rfc_vreg_mgr_wtr1605_sv.h"
#include "rfc_common_data.h"
#if (defined T_WINNT || defined T_RUMI_EMULATION)
#include "rfc_configurable_test_card_cmn_data.h"
#include "rfc_configurable_test_vreg_mgr.h"
#endif
"""

FUNCTION_PROLOGUE = """boolean
rfc_factory
(
  rf_hw_type rf_hw,
  rfc_vreg_mgr** vreg_mgr_pp,
  rfc_common_data** rfc_data_pp
)
{
  
  boolean status = TRUE;
  
  switch(rf_hw)
  {
"""

FUNCTION_EPILOGUE = """  default:
    #if (defined T_WINNT || defined T_RUMI_EMULATION)
    *vreg_mgr_pp = rfc_configurable_test_vreg_mgr::get_instance();
    *rfc_data_pp = rfc_configurable_test_card_cmn_data::get_instance(rf_hw);
    #else
    *vreg_mgr_pp = NULL;
    *rfc_data_pp = NULL;
    status = FALSE;
    #endif
    break;
  }
  return status;
}
"""

RFC_FACTORY_GLOBAL_NAME = 'RF_RFC_FACTORY_2'

class CardList:
    """Container to hold the list of cards that have been advertised in the environment"""
    the_list = dict()
    debug = False

def exists(env):
    debuginfo_opt = env.get('DEBUGINFO_OPT')
    if 'rfcfactory' in debuginfo_opt:
        print "rfc_factory::exists()"
    return True

def generate(env):
    debuginfo_opt = env.get('DEBUGINFO_OPT')
    if 'rfcfactory' in debuginfo_opt:
        print "rfc_factory::generate()"

    rootenv = env.get('IMAGE_ENV')
    if RFC_FACTORY_GLOBAL_NAME not in rootenv:
        cardlist = CardList()
        cardlist.debug = 'rfcfactory' in debuginfo_opt

        # Card factory data structure and AddRfCard method go into the top-level
        # environment
        rootenv[RFC_FACTORY_GLOBAL_NAME] = cardlist
        rootenv.AddMethod(rfc_factory_add_rf_card, "AddRfCard")
    else:
        cardlist = rootenv[RFC_FACTORY_GLOBAL_NAME]

    # The builder (the thing that creates the .cpp file) only needs to 
    # be in the local environment, since the component that is loading
    # this module is the one that is expected to create the factory.
    rfc_factory_builder = Builder(action=rfc_factory_builder_func,
                                  suffix='.cpp',
                                  emitter=rfc_factory_builder_emitter)
    env.Append(BUILDERS = {'RfcFactoryBuilder' : rfc_factory_builder})
    if cardlist.debug:
        print "  cardlist in rootenv: ", rootenv[RFC_FACTORY_GLOBAL_NAME]

def rfc_factory_add_rf_card(env, card, header, factory_func):
    cardlist = env[RFC_FACTORY_GLOBAL_NAME]
    include_dir = os.path.join(env.GetBuildPath(Dir('..')), 'api')
    if cardlist.debug:
        print "rfc_factory::rfc_factory_add_rf_card()"
        print "  ", env, card, header, factory_func
        print "  cardlist in env: ", env[RFC_FACTORY_GLOBAL_NAME]
        print "  Before cardlist.the_list:", cardlist.the_list
        print "  Header directory:", include_dir

    if card in cardlist.the_list:
        print "Error:", card, "was already defined in this build by another module."
        Exit(1)
    cardlist.the_list[card] = dict(header=header, function=factory_func, include_dir=include_dir)
    if cardlist.debug:
        print "  After cardlist.the_list:", cardlist.the_list

def rfc_factory_builder_func(target, source, env):
    cardlist = env[RFC_FACTORY_GLOBAL_NAME]
    if cardlist.debug:
        print "rfc_factory::rfc_factory_builder_func()"
        print "  ", target, source, env
        print "  cardlist in env: ", env[RFC_FACTORY_GLOBAL_NAME]
        print "  CPPPATH:", env['CPPPATH']

    env.Append(CPPPATH=[cardlist.the_list[card]['include_dir'] for card in cardlist.the_list])
    if cardlist.debug:
        print "  New CPPPATH:", env['CPPPATH']

    filename = str(target[0])
    if cardlist.debug:
        print "  Creating file:", filename
    factory_file = open(filename, 'w')
    factory_file.write(env.CreateFileBanner(filename, style="C"))
    factory_file.write(COMMON_HEADERS)
    for card in cardlist.the_list:
        if cardlist.debug:
            print "    Adding card:", card
        factory_file.write('#include "{header}"\n'.format(header=cardlist.the_list[card]['header']))

    factory_file.write("\n\n")
    factory_file.write(FUNCTION_PROLOGUE)

    '''for card in cardlist.the_list:
        factory_file.write('  case {card}:\n    return {func}(rf_hw, vreg_mgr_pp, rfc_data_pp);\n\n'.format(card=card, func=cardlist.the_list[card]['function']))'''

    for card in cardlist.the_list:
        if card == 'RF_HW_UNDEFINED':
            factory_file.write('   case {card}:\n   *vreg_mgr_pp = rfc_configurable_test_vreg_mgr::get_instance();\n   *rfc_data_pp = {card_cmn_ag}::get_instance(rf_hw);\n   break;\n\n'.format(card=card, card_cmn_ag=cardlist.the_list[card]['function']))
        else:
            factory_file.write('   case {card}:\n   *vreg_mgr_pp = rfc_vreg_mgr_wtr1605_sv::get_instance();\n   *rfc_data_pp = {card_cmn_ag}::get_instance(rf_hw);\n   break;\n\n'.format(card=card, card_cmn_ag=cardlist.the_list[card]['function']))

    factory_file.write(FUNCTION_EPILOGUE)
    factory_file.close()

    return None

def rfc_factory_builder_emitter(target, source, env):
    cardlist = env[RFC_FACTORY_GLOBAL_NAME]
    if cardlist.debug:
        print "rfc_factory::rfc_factory_builder_emitter()"
        print "  ", target, source
        print "  cardlist in env: ", env[RFC_FACTORY_GLOBAL_NAME]
    return target, []

