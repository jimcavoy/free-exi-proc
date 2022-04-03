/*
    v2g-guru-exi - an EXI (= "Efficient XML Interchange") processor
    Copyright (C) 2022  Tomas Prerovsky <cepsdev@hotmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <chrono>
#include <sstream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <unordered_map>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <map>
#include <algorithm>
#include <future>
#include <netinet/sctp.h> 
#include "ceps_ast.hh"
#include "core/include/state_machine_simulation_core.hpp"

namespace v2g_guru_exi{
    static Ism4ceps_plugin_interface* plugin_master = nullptr;
    static const std::string version_info = "v2g-guru-exi v0.1";
    static constexpr bool print_debug_info{true}; 
    ceps::ast::node_t plugin_entrypoint_add_start_grammar(ceps::ast::node_callparameters_t params);
}

ceps::ast::node_t v2g_guru_exi::plugin_entrypoint_add_start_grammar(ceps::ast::node_callparameters_t params){
    std::cerr << "***Warning: function exi_processor_add_start_grammar(Grammar{...}) not implemented yet.\n"; 
    return nullptr;
}

extern "C" void init_plugin(IUserdefined_function_registry* smc)
{
  v2g_guru_exi::plugin_master = smc->get_plugin_interface();
  v2g_guru_exi::plugin_master->reg_ceps_plugin("exi_processor_add_start_grammar", v2g_guru_exi::plugin_entrypoint_add_start_grammar);
  if(v2g_guru_exi::print_debug_info) std::cout << v2g_guru_exi::version_info << " registered.\n";
}

