
// Generated from CProg.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "CProgBaseVisitor.h"
#include <iostream>
#include <string>

/**
 * This class provides an empty implementation of CProgVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  CProgASVisitor : public CProgBaseVisitor {
public:

  virtual antlrcpp::Any visitProg(CProgParser::ProgContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual antlrcpp::Any visitFuncdef(CProgParser::FuncdefContext *ctx) override {
    std::cout << ctx->identifier()->MAIN()->getText() << ":" << std::endl;
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
    std::cout << "  movl $" << ctx->INT_LITTERAL()->getText() << ", %eax" << std::endl;
    std::cout << "  ret" << std::endl;
    return visitChildren(ctx);
  }


};

