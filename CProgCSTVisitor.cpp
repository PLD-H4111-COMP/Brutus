#include "CProgCSTVisitor.h"
#include <iostream>
#include <string>

antlrcpp::Any CProgCSTVisitor::visitProgram(CProgParser::ProgramContext *ctx)
{
    std::cout << ".text" << std::endl;
    return visitChildren(ctx);
}

antlrcpp::Any CProgCSTVisitor::visitFuncdef(CProgParser::FuncdefContext *ctx)
{
    std::cout << ".global " << ctx->IDENTIFIER()->getText() << std::endl;
    std::cout << ctx->IDENTIFIER()->getText() << ":" << std::endl;
    std::cout << "    pushq %rbp" << std::endl;
    std::cout << "    movq %rsp, %rbp" << std::endl;
    return visitChildren(ctx);
}

antlrcpp::Any CProgCSTVisitor::visitReturn_statement(CProgParser::Return_statementContext *ctx)
{
    std::string tmp_res = visit(ctx->int_expr());
    std::cout << "    movl " << tos[tmp_res].index << "(%rbp) , %eax" << std::endl;
    std::cout << "    popq %rbp" << std::endl;
    std::cout << "    ret" << std::endl;
    return 0; // nothing to return, children were already visited
}

antlrcpp::Any CProgCSTVisitor::visitDeclaration(CProgParser::DeclarationContext *ctx)
{
    for(auto identifier : ctx->IDENTIFIER())
    {
        std::string name = identifier->getText();
        if(!tos.declared(name))
        {
            tos.add_variable(name);
        }
    }
    for(auto assignment : ctx->assignment())
    {
        std::string lhs_name = assignment->IDENTIFIER()->getText();
        if(!tos.declared(lhs_name))
        {
            tos.add_variable(lhs_name);
        }
        visit(assignment);
    }
    return 0; // nothing to return, children were already visited
}

antlrcpp::Any CProgCSTVisitor::visitAssignment(CProgParser::AssignmentContext *ctx)
{
    std::string lhs_name = ctx->IDENTIFIER()->getText();
    if(!tos.declared(lhs_name))
    {
        std::cerr << "error: use of undeclared identifier '" << lhs_name << "'" << std::endl;
    }
    std::string rhs_name = visit(ctx->int_expr());
    tos[lhs_name].initialized = tos[rhs_name].initialized;
    std::cout << "    movl " << tos[rhs_name].index << "(%rbp)" << ", %eax" << std::endl;
    std::cout << "    movl " << "%eax, " << tos[lhs_name].index << "(%rbp)" << std::endl;
    return lhs_name;
}

antlrcpp::Any CProgCSTVisitor::visitInt_expr(CProgParser::Int_exprContext *ctx)
{
    if(ctx->int_terms() != nullptr)
    {
        return visit(ctx->int_terms());
    }
    return visit(ctx->assignment());
}

antlrcpp::Any CProgCSTVisitor::visitInt_terms(CProgParser::Int_termsContext *ctx)
{
    if(ctx->rhs_int_terms().empty())
    {
        return visit(ctx->int_factors());
    }
    std::string lhs_name = visit(ctx->int_factors());
    std::string tmp_res = tos.add_tmp_result();
    std::cout << "    movl " << tos[lhs_name].index << "(%rbp)" << ", %eax" << std::endl;
    std::cout << "    movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
    tos[tmp_res].initialized = tos[lhs_name].initialized;
    for(auto term : ctx->rhs_int_terms())
    {
        std::string rhs_name = visit(term);
        std::cout << "    movl " << tos[rhs_name].index << "(%rbp)" << ", %eax" << std::endl;
        if(term->OP_ADD() != nullptr)
        {
            std::cout << "    addl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        }
        if(term->OP_SUB() != nullptr)
        {
            std::cout << "    subl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        }
        if(!tos[rhs_name].initialized)
        {
            tos[tmp_res].initialized = false;
        }
    }
    return tmp_res;
}

antlrcpp::Any CProgCSTVisitor::visitRhs_int_terms(CProgParser::Rhs_int_termsContext *ctx)
{
    return visit(ctx->int_factors());
}

antlrcpp::Any CProgCSTVisitor::visitInt_factors(CProgParser::Int_factorsContext *ctx)
{
    if(ctx->rhs_int_factors().empty())
    {
        return visit(ctx->int_signed_atom());
    }
    std::string lhs_name = visit(ctx->int_signed_atom());
    std::string tmp_res = tos.add_tmp_result();
    std::cout << "    movl " << tos[lhs_name].index << "(%rbp)" << ", %eax" << std::endl;
    std::cout << "    movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
    tos[tmp_res].initialized = tos[lhs_name].initialized;
    for(auto factor : ctx->rhs_int_factors())
    {
        std::string rhs_name = visit(factor);
        std::cout << "    movl " << tos[rhs_name].index << "(%rbp)" << ", %eax" << std::endl;
        if(factor->OP_MUL() != nullptr)
        {
            std::cout << "    imull " << tos[tmp_res].index << "(%rbp), %eax" << std::endl;
            std::cout << "    movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        }
        if(factor->OP_DIV() != nullptr)
        {
            std::cout << "    movl %eax, %ebx" << std::endl;
            std::cout << "    movl " << tos[tmp_res].index << "(%rbp), %eax" << std::endl;
            std::cout << "    cltd" << std::endl;
            std::cout << "    idivl %ebx " << std::endl;
            std::cout << "    movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        }
        if(factor->OP_MOD() != nullptr)
        {
            std::cout << "    movl %eax, %ebx" << std::endl;
            std::cout << "    movl " << tos[tmp_res].index << "(%rbp), %eax" << std::endl;
            std::cout << "    cltd" << std::endl;
            std::cout << "    idivl %ebx" << std::endl;
            std::cout << "    movl %edx, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        }
        if(!tos[rhs_name].initialized)
        {
            tos[tmp_res].initialized = false;
        }
    }
    return tmp_res;
}

antlrcpp::Any CProgCSTVisitor::visitRhs_int_factors(CProgParser::Rhs_int_factorsContext *ctx)
{
    return visit(ctx->int_signed_atom());
}

antlrcpp::Any CProgCSTVisitor::visitInt_atom(CProgParser::Int_atomContext *ctx)
{
    if(ctx->INT_LITTERAL() != nullptr)
    {
        std::string tmp_res = tos.add_tmp_result();
        std::string value = ctx->INT_LITTERAL()->getText();
        std::cout << "    movl " << "$" << value << ", " << tos[tmp_res].index << "(%rbp)" << std::endl;
        tos[tmp_res].initialized = true;
        tos[tmp_res].used = true;
        return tmp_res;
    }
    if(ctx->IDENTIFIER() != nullptr)
    {
        std::string id = ctx->IDENTIFIER()->getText();
        if(!tos.declared(id))
        {
            std::cerr << "error: use of undeclared identifier '" << id << "'" << std::endl;
        }
        else
        {
            tos[id].used = true;
            if(!tos[id].initialized)
            {
                std::cerr << "warning: use of uninitialized variable '" << id << "'" << std::endl;
            }
        }
        return id;
    }
    return visit(ctx->int_expr());
}

antlrcpp::Any CProgCSTVisitor::visitInt_signed_atom(CProgParser::Int_signed_atomContext *ctx)
{
    if(ctx->OP_SUB() != nullptr) // will lead to an overflow for extreme litteral values
    {
        std::string tmp_res = tos.add_tmp_result();
        std::string atom_name = visit(ctx->int_signed_atom());
        std::cout << "    movl " << tos[atom_name].index << "(%rbp)" << ", %eax" << std::endl;
        std::cout << "    imull $-1, %eax" << std::endl;
        std::cout << "    movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        tos[tmp_res].initialized = tos[atom_name].initialized;
        return tmp_res;
    }
    if(ctx->OP_ADD() != nullptr)
    {
        return visit(ctx->int_signed_atom());
    }
    return visit(ctx->int_atom());
}

