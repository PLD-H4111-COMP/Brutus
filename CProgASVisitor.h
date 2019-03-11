#pragma once

#include "antlr4-runtime.h"
#include "CProgBaseVisitor.h"

class CProgASVisitor : public CProgBaseVisitor {
public:
    virtual antlrcpp::Any visitPreproc(CProgParser::PreprocContext *ctx) override;
    virtual antlrcpp::Any visitProg(CProgParser::ProgContext *ctx) override;
    virtual antlrcpp::Any visitFuncdef(CProgParser::FuncdefContext *ctx) override;
    virtual antlrcpp::Any visitType(CProgParser::TypeContext *ctx) override;
    virtual antlrcpp::Any visitIdentifier(CProgParser::IdentifierContext *ctx) override;
    virtual antlrcpp::Any visitBlock(CProgParser::BlockContext *ctx) override;
    virtual antlrcpp::Any visitStatement(CProgParser::StatementContext *ctx) override;
    virtual antlrcpp::Any visitReturn_statement(CProgParser::Return_statementContext *ctx) override;
};

