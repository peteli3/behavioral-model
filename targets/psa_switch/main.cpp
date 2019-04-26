/* Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Antonin Bas (antonin@barefootnetworks.com)
 *
 */

/* Switch instance */
#include <iostream>
#include <fstream>
#include <string>

#include <bm/PsaSwitch.h>
#include <bm/bm_runtime/bm_runtime.h>
#include <bm/bm_sim/target_parser.h>

#include "psa_switch.h"

namespace {
PsaSwitch *psa_switch;
bm::TargetParserBasic *psa_switch_parser;
}  // namespace

namespace pswitch_runtime {
shared_ptr<PsaSwitchIf> get_handler(PsaSwitch *sw);
}  // namespace pswitch_runtime

int
main(int argc, char* argv[]) {
  std::cout << "-> PSA MAIN new psa switch\n";
  psa_switch = new PsaSwitch();
  std::cout << "-> PSA MAIN new target parser basic\n";
  psa_switch_parser = new bm::TargetParserBasic();
  std::cout << "-> PSA MAIN init from cmd line\n";
  psa_switch_parser->add_flag_option("enable-swap",
                                        "enable JSON swapping at runtime");
  std::cout << "-> PSA ADD FLAG OPTION DONE\n";
  int status = psa_switch->init_from_command_line_options(
      argc, argv, psa_switch_parser);
  std::cout << "-> PSA MAIN init from cmd line EXIT STAT " << status << "\n";

  std::cout << "-> PSA MAIN enable config swap\n";
  bool enable_swap_flag = false;
  if (psa_switch_parser->get_flag_option("enable-swap", &enable_swap_flag)
      != bm::TargetParserBasic::ReturnCode::SUCCESS)
    std::exit(1);
  if (enable_swap_flag) psa_switch->enable_config_swap();

  int thrift_port = psa_switch->get_runtime_port();
  bm_runtime::start_server(psa_switch, thrift_port);
  using ::pswitch_runtime::PsaSwitchIf;
  using ::pswitch_runtime::PsaSwitchProcessor;
  bm_runtime::add_service<PsaSwitchIf, PsaSwitchProcessor>(
      "psa_switch", pswitch_runtime::get_handler(psa_switch));
  psa_switch->start_and_return();

  while (true) std::this_thread::sleep_for(std::chrono::seconds(100));

  return 0;
}
