#include "CProgASVisitor.h"
#include <iostream>

antlrcpp::Any CProgASVisitor::visitPreproc(CProgParser::PreprocContext *ctx) {
    std::cerr << "Ignoring " << ctx->getText() << std::endl;
    return visitChildren(ctx);
}
    
antlrcpp::Any CProgASVisitor::visitProg(CProgParser::ProgContext *ctx) {
    std::cout << ".text" << std::endl;
    return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitFuncdef(CProgParser::FuncdefContext *ctx) {
    std::cout << ".global " << ctx->identifier()->IDENTIFIER()->getText() << std::endl;
    std::cout << ctx->identifier()->IDENTIFIER()->getText() << ":" << std::endl;
    std::cout << "    pushq %rbp" << std::endl;
    std::cout << "    movq %rsp, %rbp" << std::endl;

    return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitType(CProgParser::TypeContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitIdentifier(CProgParser::IdentifierContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitBlock(CProgParser::BlockContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitStatement(CProgParser::StatementContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitReturn_statement(CProgParser::Return_statementContext *ctx) {
    std::cout << "    movl $" << ctx->INT_LITTERAL()->getText() << ", %eax" << std::endl;
    std::cout << "    popq %rbp" << std::endl;
    std::cout << "    ret" << std::endl;
    return visitChildren(ctx);
}

