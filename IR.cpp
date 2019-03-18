#include "IR.h"

IRStore::add_cfg(CFG* cfg) {
	cfgs.push_back(cfg);
}

IRStore::~IRStore() {
    for (CFG* cfg : cfgs){
        delete cfg;
    }
}

CFG::CFG(DefFonction* funcAst) : ast(funcAst), nextFreeSymbolIndex(0), nextBBnumber(0) {
    bbs.push_back(new BasicBlock(this, "input"));
    bbs.push_back(new BasicBlock(this, "output"));
}

string CFG::new_BB_name() {
    string newName = "block_"+to_string(nextBBnumber++);
}

void CFG::add_bb(BasicBlock* bb) {
    bbs.insert(bbs.end()-1, bb);
}

void CFG::add_to_symbol_table(string name, Type t) {
    SymbolType.insert(pair<string,Type>(name,t));
    SymbolIndex.insert(pair<string,Type>(name,nextFreeSymbolIndex));
    nextFreeSymbolIndex -= 4; //offset sera variable en fonction du type -> nextFreeSymbolIndex += t.size;
}

string CFG::create_new_tempvar(Type t) {
    SymbolType.insert(pair<string,Type>("!temp"+to_string(nextFreeSymbolIndex),t));
    SymbolIndex.insert(pair<string,Type>(name,nextFreeSymbolIndex));
    nextFreeSymbolIndex -= 4; //offset sera variable en fonction du type -> nextFreeSymbolIndex += t.size;
}
