/*
    v2g-guru-exi - an EXI (= "Efficient XML Interchange") processor
    Copyright (C) 2022 Tomas Prerovsky <cepsdev@hotmail.com>

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

#include "v2g-guru-exi.h"


namespace v2g_guru_exi{
    
    void Processor::insert(GenericGrammar g){
        generic_grammars[g.pattern_to_str()] = g.get_grammar();

    }                     
 
    void Processor::set_start_grammar(Grammar g){
        grammars = stack<Grammar>{};
        grammars.push(g);
    }
    
    void Processor::set_event_stream(EventStream ev_stream){
        event_stream = ev_stream;
    }

    bool Processor::match(Grammar::Terminal terminal){
         auto tok = event_stream.get_event().as_terminal();
         return tok == terminal;                 
    }

    void Processor::emit_eventcode(Grammar& g, Grammar::Production prod){
        if(!emitter) return;
        for(auto it = prod.begin(); it != prod.end(); ++it){
            if (!(*it).is_eventcode()) continue;
            auto ev_code{(*it).as_eventcode()};
            emitter->emit(ev_code);
            std::cout << "    EMIT: " << ev_code << "\n"; 
            break;
        }
    }

    void Processor::parse(Grammar& g, Grammar::Production prod) {
        if (debug_output) std::cout << "Processor::parse(Grammar& g, Grammar::Production prod):\n";
        if (debug_output) std::cout << "  " << *prod.get_lhs().get_rep() << ": " << *prod.get_rhs_rep() << "\n"; 
        if (debug_output) std::cout << "  peek = " <<  *event_stream.peek().get_rep() << "\n\n\n";
        
        emit_eventcode(g, prod);

        for(auto it = prod.begin(); it != prod.end(); ++it) {
            auto rhs_elem = *it;
            if (debug_output) std::cout << " parse: checking " << *rhs_elem.rep << ": " << " terminal? " << rhs_elem.is_terminal() << " nonterminal? " << rhs_elem.is_nonterminal() << "\n";
            if (rhs_elem.is_terminal()){  
                auto lookahead = event_stream.peek();
                if (debug_output) std::cout << "match("<< *lookahead.get_rep() << ", " << *rhs_elem.rep << ")\n";
                if (!match(rhs_elem.as_terminal())) throw parser_exception{"match failed."};
            } else if (rhs_elem.is_nonterminal()){
                auto nt = rhs_elem.as_nonterminal();
                auto nt_rhs = g.right_hand_sides(nt);
                if (nt_rhs.size()){
                    auto lookahead = event_stream.peek();
                    if (debug_output) std::cout << " lookahead = " << *lookahead.get_rep() << "\n";
                    if (!lookahead) throw parser_exception{};
                    bool prod_found{};
                    for(auto prod : nt_rhs){
                        std::cout << "--- Is this one right? " << *prod.get_rhs_rep() << "\n";
                        for(auto entry : prod){
                            if (entry.is_annotation()) continue;
                            if (entry.is_nonterminal()) break;
                            if (entry.as_terminal() == lookahead.as_terminal()){
                                if (debug_output) std::cout << " Found rhs with entry = " << *entry.rep << "\n";
                                auto it_gg = generic_grammars.find(entry.as_terminal().as_str());
                                if (it_gg != generic_grammars.end()) {
                                    if (debug_output) std::cout << "Generic grammar found" << "\n";
                                    auto slookahead = lookahead.as_terminal().as_str();
                                    auto it_global_g = global_grammars.find(slookahead);
                                    if (it_global_g == global_grammars.end()){
                                        global_grammars[slookahead] = Grammar{it_gg->second};
                                        it_global_g = global_grammars.find(slookahead);
                                    }
                                    if (!match(entry.as_terminal())) throw parser_exception{"match failed (B)."};
                                    emit_eventcode(g, prod);
                                    parse(it_global_g->second); 
                                } prod_found = true;
                                break;                                
                            }
                        }
                        if (prod_found) {
                            parse(g, prod);
                            break;
                        }
                    }
                    if (!prod_found) throw parser_exception{"Missing rule.(A)"};                    
                } else throw parser_exception{"Missing rule.(B)"};
            }
        }
    }

    void Processor::parse(Grammar& g){
        if (debug_output)std::cout << "Processor::parse(Grammar& g)\n";
        //if (debug_output)std::cout << "G=\n";
        //if (debug_output)std::cout << *g.grammar_rep << std::endl;
        for (;event_stream;){
             auto tok = event_stream.peek();
             if (!tok) throw parser_exception{};
             if (debug_output) std::cout << "tok=" << *tok.get_rep() << "\n";
             std::cout << "-------------------\n";
             auto production = g.find_production_starting_with(tok.as_terminal());
             //std::cout << *g.grammar_rep << "\n";
             std::cout << "-------------------\n";
             if (!production) throw parser_exception{"parse(G): No rule found."};
             parse(g,*production);
        }
    }

    void Processor::encode(){
        bool error_occured{};
        if (debug_output)std::cout << "Processor::encode():\n";
        while(grammars.size() && !error_occured){
            auto current_grammar = grammars.top();
            grammars.pop();
            try{
                parse(current_grammar);
            } catch (parser_exception& pe){
                std::cerr << "*** Fatal Error [EXI Processor]: " << pe.msg << std::endl;
                error_occured = true;
            }
        }
        if (debug_output) std::cout << "Processor::encode(): exit.\n";
    }
}