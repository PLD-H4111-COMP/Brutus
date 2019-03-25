#include "IR.h"


// ****************************************************************************


IRInstr::IRInstr(BasicBlock* bb_, Operation op, VarType t, std::vector<std::string> params){
}

void IRInstr::gen_asm(std::ostream &o){
}


// ****************************************************************************


BasicBlock::BasicBlock(CFG* cfg, std::string entry_label){
}

void BasicBlock::gen_asm(std::ostream &o){
}

void BasicBlock::add_IRInstr(IRInstr::Operation op, VarType t, std::vector<std::string> params){
}


// ****************************************************************************


void CFG::gen_asm(std::ostream& o){
}
std::string CFG::IR_reg_to_asm(std::string reg){
    return "";
}

void CFG::gen_asm_prologue(std::ostream& o){
}

void CFG::gen_asm_epilogue(std::ostream& o){
}


int CFG::get_var_index(std::string name){
    return 0;
}

/*VarType CFG::get_var_type(std::string name){

}*/

CFG::CFG(DefFonction* funcAst) : ast(funcAst), nextFreeSymbolIndex(0), nextBBnumber(0) {
    bbs.push_back(new BasicBlock(this, "input"));
    bbs.push_back(new BasicBlock(this, "output"));
}


std::string CFG::new_BB_name() {
    std::string newName = "block_" + std::to_string(nextBBnumber++);
}


void CFG::add_bb(BasicBlock* bb) {
    bbs.insert(bbs.end()-1, bb);
}

void CFG::add_to_symbol_table(std::string name, VarType t) {
    SymbolType[name] = t;
    SymbolIndex[name] = nextFreeSymbolIndex;
    nextFreeSymbolIndex -= 4; //offset sera variable en fonction du type -> nextFreeSymbolIndex += t.size;
}

std::string CFG::create_new_tempvar(VarType t) {
    std::string name = "!temp" + std::to_string(nextFreeSymbolIndex);
    SymbolType[name] = t;
    SymbolIndex[name] = nextFreeSymbolIndex;
    nextFreeSymbolIndex -= 4; //offset sera variable en fonction du type -> nextFreeSymbolIndex += t.size;
}


// ****************************************************************************



void IRStore::add_cfg(CFG* cfg) {
	cfgs.push_back(cfg);
}

IRStore::~IRStore() {
    for (CFG* cfg : cfgs){
        delete cfg;
    }
}

