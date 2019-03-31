#include "CProgCSTVisitor.h"
#include "Writer.h"
#include <iostream>
#include <string>

CProgCSTVisitor::CProgCSTVisitor(Writer &writer) : writer(writer), tos()
{

}

antlrcpp::Any CProgCSTVisitor::visitProgram(CProgParser::ProgramContext *ctx)
{
    writer.assembly(0) << ".text" << std::endl;
    return visitChildren(ctx);
}

antlrcpp::Any CProgCSTVisitor::visitFuncdef(CProgParser::FuncdefContext *ctx)
{
    writer.assembly(0) << ".global " << ctx->IDENTIFIER()->getText() << std::endl;
    writer.assembly(0) << ctx->IDENTIFIER()->getText() << ":" << std::endl;
    writer.assembly(1) << "pushq %rbp" << std::endl;
    writer.assembly(1) << "movq %rsp, %rbp" << std::endl;
    return visitChildren(ctx);
}

antlrcpp::Any CProgCSTVisitor::visitReturn_statement(CProgParser::Return_statementContext *ctx)
{
    std::string tmp_res = visit(ctx->int_expr());
    writer.assembly(1) << "movl " << tos[tmp_res].index << "(%rbp) , %eax" << std::endl;
    writer.assembly(1) << "popq %rbp" << std::endl;
    writer.assembly(1) << "ret" << std::endl;
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
        writer.error() << "use of undeclared identifier '" << lhs_name << "'" << std::endl;
    }
    std::string rhs_name = visit(ctx->int_expr());
    tos[lhs_name].initialized = tos[rhs_name].initialized;
    writer.assembly(1) << "movl " << tos[rhs_name].index << "(%rbp)" << ", %eax" << std::endl;
    writer.assembly(1) << "movl " << "%eax, " << tos[lhs_name].index << "(%rbp)" << std::endl;
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
    writer.assembly(1) << "movl " << tos[lhs_name].index << "(%rbp)" << ", %eax" << std::endl;
    writer.assembly(1) << "movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
    tos[tmp_res].initialized = tos[lhs_name].initialized;
    for(auto term : ctx->rhs_int_terms())
    {
        std::string rhs_name = visit(term);
        writer.assembly(1) << "movl " << tos[rhs_name].index << "(%rbp)" << ", %eax" << std::endl;
        if(term->OP_ADD() != nullptr)
        {
            writer.assembly(1) << "addl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        }
        if(term->OP_SUB() != nullptr)
        {
            writer.assembly(1) << "subl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
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
    writer.assembly(1) << "movl " << tos[lhs_name].index << "(%rbp)" << ", %eax" << std::endl;
    writer.assembly(1) << "movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
    tos[tmp_res].initialized = tos[lhs_name].initialized;
    for(auto factor : ctx->rhs_int_factors())
    {
        std::string rhs_name = visit(factor);
        writer.assembly(1) << "movl " << tos[rhs_name].index << "(%rbp)" << ", %eax" << std::endl;
        if(factor->OP_MUL() != nullptr)
        {
            writer.assembly(1) << "imull " << tos[tmp_res].index << "(%rbp), %eax" << std::endl;
            writer.assembly(1) << "movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        }
        if(factor->OP_DIV() != nullptr)
        {
            writer.assembly(1) << "movl %eax, %ebx" << std::endl;
            writer.assembly(1) << "movl " << tos[tmp_res].index << "(%rbp), %eax" << std::endl;
            writer.assembly(1) << "cltd" << std::endl;
            writer.assembly(1) << "idivl %ebx " << std::endl;
            writer.assembly(1) << "movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        }
        if(factor->OP_MOD() != nullptr)
        {
            writer.assembly(1) << "movl %eax, %ebx" << std::endl;
            writer.assembly(1) << "movl " << tos[tmp_res].index << "(%rbp), %eax" << std::endl;
            writer.assembly(1) << "cltd" << std::endl;
            writer.assembly(1) << "idivl %ebx" << std::endl;
            writer.assembly(1) << "movl %edx, " << tos[tmp_res].index << "(%rbp)" << std::endl;
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
        writer.assembly(1) << "movl " << "$" << value << ", " << tos[tmp_res].index << "(%rbp)" << std::endl;
        tos[tmp_res].initialized = true;
        tos[tmp_res].used = true;
        return tmp_res;
    }
    if(ctx->IDENTIFIER() != nullptr)
    {
        std::string id = ctx->IDENTIFIER()->getText();
        if(!tos.declared(id))
        {
            writer.error() << "use of undeclared identifier '" << id << "'" << std::endl;
        }
        else
        {
            tos[id].used = true;
            if(!tos[id].initialized)
            {
                writer.warning() << "use of uninitialized variable '" << id << "'" << std::endl;
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
        writer.assembly(1) << "movl " << tos[atom_name].index << "(%rbp)" << ", %eax" << std::endl;
        writer.assembly(1) << "imull $-1, %eax" << std::endl;
        writer.assembly(1) << "movl %eax, " << tos[tmp_res].index << "(%rbp)" << std::endl;
        tos[tmp_res].initialized = tos[atom_name].initialized;
        return tmp_res;
    }
    if(ctx->OP_ADD() != nullptr)
    {
        return visit(ctx->int_signed_atom());
    }
    return visit(ctx->int_atom());
}

