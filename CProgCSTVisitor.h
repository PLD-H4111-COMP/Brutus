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
    virtual antlrcpp::Any visitInt_expr(CProgParser::Int_exprContext *ctx) override;
    virtual antlrcpp::Any visitInt_terms(CProgParser::Int_termsContext *ctx) override;
    virtual antlrcpp::Any visitRhs_int_terms(CProgParser::Rhs_int_termsContext *ctx) override;
    virtual antlrcpp::Any visitInt_factors(CProgParser::Int_factorsContext *ctx) override;
    virtual antlrcpp::Any visitRhs_int_factors(CProgParser::Rhs_int_factorsContext *ctx) override;
    virtual antlrcpp::Any visitInt_signed_atom(CProgParser::Int_signed_atomContext *ctx) override;
    virtual antlrcpp::Any visitInt_atom(CProgParser::Int_atomContext *ctx) override;
};
