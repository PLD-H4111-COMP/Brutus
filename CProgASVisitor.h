#pragma once

#include "antlr4-runtime.h"
#include "CProgBaseVisitor.h"
#include <iostream>
#include <string>

class CProgASVisitor : public CProgBaseVisitor {
public:

    virtual antlrcpp::Any visitProg(CProgParser::ProgContext *ctx) override {
        std::cout << ".text" << std::endl;
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitFuncdef(CProgParser::FuncdefContext *ctx) override {
        std::cout << ".global " << ctx->identifier()->IDENTIFIER()->getText() << std::endl;
        std::cout << ctx->identifier()->IDENTIFIER()->getText() << ":" << std::endl;
        std::cout << "    pushq %rbp" << std::endl;
        std::cout << "    movq %rsp, %rbp" << std::endl;

        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitType(CProgParser::TypeContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitIdentifier(CProgParser::IdentifierContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitBlock(CProgParser::BlockContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitStatement(CProgParser::StatementContext *ctx) override {
        return visitChildren(ctx);
    }

    virtual antlrcpp::Any visitReturn_statement(CProgParser::Return_statementContext *ctx) override {
        std::cout << "    movl $" << ctx->INT_LITTERAL()->getText() << ", %eax" << std::endl;
        std::cout << "    popq %rbp" << std::endl;
        std::cout << "    ret" << std::endl;
        return visitChildren(ctx);
    }


};

