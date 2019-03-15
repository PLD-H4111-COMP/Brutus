#include "CProgASVisitor.h"
#include <iostream>
#include <string>

antlrcpp::Any CProgASVisitor::visitPreproc(CProgParser::PreprocContext *ctx) {
    // std::cerr << "Ignoring " << ctx->getText() << std::endl;
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
    std::string ret;
    if(ctx->INT_LITTERAL() != nullptr)
    {
        ret = "$" + ctx->INT_LITTERAL()->getText();
    }
    else
    {
        ret = std::to_string(tos[ctx->identifier()->IDENTIFIER()->getText()].index) + "(%rbp)";
    }
    std::cout << "    movl " << ret << ", %eax" << std::endl;
    std::cout << "    popq %rbp" << std::endl;
    std::cout << "    ret" << std::endl;
    return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitDeclaration(CProgParser::DeclarationContext *ctx) {
    for(CProgParser::IdentifierContext *id : ctx->identifier())
    {
        if(tos.find(id->IDENTIFIER()->getText()) == tos.end())
        {
            tos_index -= 4;
            if (ctx->type()->INT() != nullptr)
                tos[id->IDENTIFIER()->getText()] = {SymbolType::INTEGER, tos_index, false, false};
        }
    }
    for(CProgParser::AssignmentContext *as : ctx->assignment())
    {
        std::string lhs_name = as->identifier(0)->IDENTIFIER()->getText();
        if(tos.find(lhs_name) == tos.end())
        {
            tos_index -= 4;
            if (ctx->type()->INT() != nullptr)
                tos[lhs_name] = {SymbolType::INTEGER, tos_index, false, false};
        }
    }
    return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitAssignment(CProgParser::AssignmentContext *ctx) {
    std::string lhs_name = ctx->identifier(0)->IDENTIFIER()->getText();
    if(tos.find(lhs_name) == tos.end())
    {
        std::cerr << "error: use of undeclared identifier '" << lhs_name << "'" << std::endl;
    }
    if(ctx->INT_LITTERAL() != nullptr)
    {
        tos[lhs_name].initialized = true;
        std::cout << "    movl $" << ctx->INT_LITTERAL()->getText() << ", ";
        std::cout << tos[lhs_name].index << "(%rbp)" << std::endl;
    }
    else
    {
        std::string rhs_name = ctx->identifier(1)->IDENTIFIER()->getText();
        if(tos.find(rhs_name) == tos.end())
        {
            std::cerr << "error: use of undeclared identifier '" << rhs_name << "'" << std::endl;
        }
        tos[lhs_name].initialized = tos[rhs_name].initialized;
        if(!tos[rhs_name].initialized)
        {
            std::cerr << "warning: use of uninitialized variable '" << rhs_name << "'" << std::endl;
        }
        std::cout << "    movl " << tos[rhs_name].index << "(%rbp)" << ", %eax" << std::endl;
        std::cout << "    movl " << "%eax, " << tos[lhs_name].index << "(%rbp)" << std::endl;
    }
    
    return visitChildren(ctx);
}

