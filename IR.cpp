#include "IR.h"
#include "Writer.h"

// ****************************************************************************

std::ostream& operator<<(std::ostream& os, const IRInstr::Operation& op)
{
    std::string operation;
    switch(op){
        case IRInstr::Operation::ldconst:
            operation = "ldconst";
        break;
        case IRInstr::Operation::add:
            operation = "add";
        break;
        case IRInstr::Operation::sub:
            operation = "sub";
        break;
        case IRInstr::Operation::mul:
            operation = "mul";
        break;
        case IRInstr::Operation::div:
            operation = "div";
        break;
        case IRInstr::Operation::mod:
            operation = "mod";
        break;
        case IRInstr::Operation::rmem:
            operation = "rmem";
        break;
        case IRInstr::Operation::wmem:
            operation = "wmem";
        break;
        case IRInstr::Operation::call:
            operation = "call";
        break;
        case IRInstr::Operation::cmp_eq:
            operation = "cmp_eq";
        break;
        case IRInstr::Operation::cmp_lt:
            operation = "cmp_lt";
        break;
        case IRInstr::Operation::cmp_le:
            operation = "cmp_le";
        break;
    }
    return os << operation;
}

// ****************************************************************************


IRInstr::IRInstr(BasicBlock* bb, Operation op, VarType t, std::vector<std::string> params)
    : bb(bb), op(op), params(params) {
}

void IRInstr::gen_asm(std::ostream& os){
    switch(op){
        case Operation::ldconst:
            os << "movq $" << params[1] << ", " << bb->cfg->get_var_index(params[0]) << "%(rbp)";
        break;
        case Operation::add:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp), %eax" << std::endl;
            os << "addq " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "movq %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)";
        break;
        case Operation::sub:
            os << "movq " << bb->cfg->get_var_index(params[1]) << "(%rbp), %eax" << std::endl;
            os << "subq " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "movq %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)";
        break;
        case Operation::mul:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "(%rbp)" << ", %eax" << std::endl;
            os << "imull " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "movl %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)";
        break;
        case Operation::div:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "(%rbp)" << ", %ebx" << std::endl;
            os << "movl " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "cltd" << std::endl;
            os << "idivl %ebx " << std::endl;
            os << "movl %eax, " << bb->cfg->get_var_index(params[0]) << "(%rbp)";
        break;
        case Operation::mod:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "(%rbp)" << ", %ebx" << std::endl;
            os << "movl " << bb->cfg->get_var_index(params[2]) << "(%rbp), %eax" << std::endl;
            os << "cltd" << std::endl;
            os << "idivl %ebx" << std::endl;
            os << "movl %edx, " << bb->cfg->get_var_index(params[0]) << "(%rbp)";
        break;
        case Operation::rmem:
            os << "movl " << bb->cfg->get_var_index(params[1]) << "%(rbp), " << bb->cfg->get_var_index(params[0]) << "(%rbp)";
            // attention difference registre/variable
        break;
        case Operation::wmem:
            
        break;
        case Operation::call:
            
        break;
        case Operation::cmp_eq:
            
        break;
        case Operation::cmp_lt:
            
        break;
        case Operation::cmp_le:
            
        break;
    }
    os << std::endl;
}

void IRInstr::print(){
    std::clog << "Type de retour : " << t << ", Operation : " << op << std::endl;
    std::clog << "Parametres : ";
    for (std::string param : params){
        std::clog << param << ", ";
    }
    std::clog << std::endl;
}


// ****************************************************************************


BasicBlock::BasicBlock(CFG* cfg, std::string entry_label) : exit_true(nullptr), exit_false(nullptr), label(entry_label) {
    // jump to the next block default ?
    this->cfg = cfg;
}

BasicBlock::~BasicBlock(){
    for (IRInstr* instr : instrs){
        delete instr;
    }
}

void BasicBlock::gen_asm(std::ostream &o){

}

void BasicBlock::add_IRInstr(IRInstr::Operation op, VarType t, std::vector<std::string> params){
    instrs.push_back(new IRInstr(this, op, t, params));
}

void BasicBlock::print(){
    std::clog << "Basic Bloc : " << label << std::endl;
    // Amelioration : ajouter les noms des blocs suivants (exit_true, exit_false)
    for (IRInstr* instr : instrs){
        instr->print();
    }
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
    return SymbolIndex[name];
}

VarType CFG::get_var_type(std::string name){
    return SymbolType[name];
}

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

void CFG::print() {
    for (BasicBlock* bb : bbs){
        bb->print();
    }
}

void CFG::printVariables(){
    for (std::map<std::string, VarType>::iterator it = SymbolType.begin(); it!=SymbolType.end(); ++it){
        std::clog << "Nom variable : " << it->first << ", Type : " << it->second << ", Valeur : " << SymbolIndex[it->first] << std::endl;
    }
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

void IRStore::print_IR(){
    int i = 0;
    std::clog << "Affichage de l'IR : " << std::endl;
    for (CFG* cfg : cfgs){
        std::clog << "CFG " << i << " : " << std::endl;
        cfg->print();
        ++i;
    }
}

