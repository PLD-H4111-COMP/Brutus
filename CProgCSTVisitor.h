#pragma once

// ------------------------------------------------------------- Project Headers
#include "antlr4-runtime.h"
#include "CProgBaseVisitor.h"

// ---------------------------------------------------------- C++ System Headers
#include <string>

class CProgCSTVisitor : public CProgBaseVisitor {
public:
    virtual antlrcpp::Any visitProgram(CProgParser::ProgramContext *ctx) override;
    virtual antlrcpp::Any visitFuncdef(CProgParser::FuncdefContext *ctx) override;
    virtual antlrcpp::Any visitStatement(CProgParser::StatementContext *ctx) override;
    virtual antlrcpp::Any visitReturn_statement(CProgParser::Return_statementContext *ctx) override;
    virtual antlrcpp::Any visitDeclaration(CProgParser::DeclarationContext *ctx) override;
    virtual antlrcpp::Any visitAssignment(CProgParser::AssignmentContext *ctx) override;
    virtual antlrcpp::Any visitExpr(CProgParser::ExprContext *ctx) override;
    virtual antlrcpp::Any visitAsgn_expr(CProgParser::Asgn_exprContext *ctx) override;
    virtual antlrcpp::Any visitOr_expr(CProgParser::Or_exprContext *ctx) override;
    virtual antlrcpp::Any visitAnd_expr(CProgParser::And_exprContext *ctx) override;
    virtual antlrcpp::Any visitBor_expr(CProgParser::Bor_exprContext *ctx) override;
    virtual antlrcpp::Any visitXor_expr(CProgParser::Xor_exprContext *ctx) override;
    virtual antlrcpp::Any visitBand_expr(CProgParser::Band_exprContext *ctx) override;
    virtual antlrcpp::Any visitEq_expr(CProgParser::Eq_exprContext *ctx) override;
    virtual antlrcpp::Any visitRel_expr(CProgParser::Rel_exprContext *ctx) override;
    virtual antlrcpp::Any visitAdd_expr(CProgParser::Add_exprContext *ctx) override;
    virtual antlrcpp::Any visitMult_expr(CProgParser::Mult_exprContext *ctx) override;
    virtual antlrcpp::Any visitUnary_expr(CProgParser::Unary_exprContext *ctx) override;
    virtual antlrcpp::Any visitPostfix_expr(CProgParser::Postfix_exprContext *ctx) override;
    //virtual antlrcpp::Any visitFunc_call(CProgParser::Func_callContext *ctx) override;
    //virtual antlrcpp::Any visitArg_list(CProgParser::Arg_listContext *ctx) override;
    virtual antlrcpp::Any visitAtom_expr(CProgParser::Atom_exprContext *ctx) override;
};
